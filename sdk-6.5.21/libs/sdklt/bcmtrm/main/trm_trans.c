/*! \file trm_trans.c
 *
 * This module implements the transaction handling backend
 * of the transaction manager (TRM)
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_assert.h>
#include <sal/sal_types.h>
#include <sal/sal_msgq.h>
#include <sal/sal_sem.h>
#include <sal/sal_sleep.h>
#include <bsl/bsl.h>
#include <shr/shr_error.h>
#include <shr/shr_lmem_mgr.h>
#include <bcmltd/bcmltd_lt_types.h>
#include <bcmltm/bcmltm.h>
#include <bcmtrm/trm_api.h>
#include <bcmtrm/generated/bcmtrm_ha.h>
#include "trm_internal.h"

#define BSL_LOG_MODULE BSL_LS_BCMTRM_TRANS

/*
 * This function being called by the PTM WAL thread upon completion
 * of the commit operation into H/W. This function being called for
 * asynchronous transactions only. The transaction can be batch or
 * atomic.
 */
static void trm_async_trans_cb(uint32_t trans_id,
                               shr_error_t status,
                               void *user_data)
{
    bcmtrm_entry_t *entry = (bcmtrm_entry_t *)user_data;
    bcmtrm_hw_notif_struct_t hw_notif;
    bcmtrm_trans_t *trans;

    if (!entry) {
        return;
    }
    trans = entry->p_trans;
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(trans->unit, "trans_id=%d status=%d\n"),
                 trans_id, status));

    if (trans->state == T_ACTIVE || trans->state == T_IDLE) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(trans->unit,
                             "callback for non active trans trans_id=%u\n"),
                  trans_id));
        return;
    }
    bcmtrm_trans_ha_state.done_f(entry->info.unit, trans_id, entry->ha_trn_hdl);

    hw_notif.type = TRANS_CB;
    hw_notif.ltm_entry = entry;
    hw_notif.status = status;
    /* Post the event to the notification thread */
    sal_msgq_post(bcmtrm_hw_notif_q, &hw_notif, SAL_MSGQ_NORMAL_PRIORITY,
                  SAL_MSGQ_FOREVER);
}

/*
 * This function called by the PTM WAL thread on completion of an atomic
 * transaction that was synchronous transaction.
 */
static void sync_atomic_trans_cb(uint32_t trans_id,
                                 shr_error_t status,
                                 void *user_data)
{
    bcmtrm_trans_t *trans = (bcmtrm_trans_t *)user_data;

    bcmtrm_trans_ha_state.done_f(trans->unit, trans_id,
                                 trans->l_entries->ha_trn_hdl);
    trans->info.status = status;
    if (status != SHR_E_NONE) {
        bcmtrm_entry_t *entry_it = trans->l_entries;
        /* Update the state of all entries */
        while (entry_it) {
            entry_it->info.status = status;
            entry_it = entry_it->next;
        }
    }

    /*
     * Call the transaction callback function. The notification action will
     * be taked by the bcmtrm_trans_req() function
     */
    bcmtrn_trans_cb_and_clean(trans, BCMLT_NOTIF_OPTION_HW, status);
}

/*
 * This function processes atomic transaction.
 */
static int handle_atomic(bcmtrm_trans_t *trans,
                         uint32_t *op_id,
                         uint32_t *trans_id,
                         void *user_data)
{
    int rv = SHR_E_PARAM;
    bcmtrm_entry_t *entry;
    bcmptm_trans_cb_f  cb;
    void *context;
    uint32_t ha_trn_hdl;
    sal_mutex_t mtx = bcmtrm_unit_mutex_get(trans->unit);

    /* Take exclusive access to the unit for modeled path */
    sal_mutex_take(mtx, SAL_MUTEX_FOREVER);

    bcmtrm_trans_rec(trans); /* Record the transaction */

    bcmtrm_trans_ha_state.set_f(trans->unit,
                                BCMTRM_STATE_STAGING,
                                *trans_id,
                                &ha_trn_hdl);
    for (entry = trans->l_entries; entry; entry = entry->next) {
        assert(entry->p_trans == trans);
        trans->committed_entries++;
        rv = bcmtrm_stage_entry(entry, *op_id, *trans_id);
        entry->info.status = rv;

        if (rv != SHR_E_NONE) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(entry->info.unit,
                             "Atomic transaction operation failed trans_id=%u"),
                      *trans_id));
            /* Cancel the transaction */
            bcmltm_abort(trans->unit, *trans_id);
            bcmtrm_trans_ha_state.cancel_f(trans->unit, *trans_id, ha_trn_hdl);

            trans->processed_entries = trans->info.num_entries;
            break;
        }
        trans->commit_success++;
    }

    if (rv != SHR_E_NONE) {
        INCREMENT_OP_ID(*trans_id);
        /*
         * Done with this transaction. Open the device to other modeled
         * operations.
         */
        sal_mutex_give(mtx);

        trans->info.status = SHR_E_FAIL;
        trans->state = T_ACTIVE;
        bcmtrn_trans_cb_and_clean(trans,
                                  BCMLT_NOTIF_OPTION_COMMIT,
                                  rv);
        return SHR_E_NONE;
    }

    trans->state = T_COMMITTED;
    bcmtrm_trans_entries_state_set(trans, E_COMMITTED);
    if (!trans->syncronous) {
        /* The transaction is now committed, inform the app. */
        bcmtrm_appl_trans_inform(trans,
                              SHR_E_NONE,
                              BCMLT_NOTIF_OPTION_COMMIT);
        cb = trm_async_trans_cb;
        context = trans->l_entries;
    } else {
        cb = sync_atomic_trans_cb;
        context = trans;
    }

    trans->l_entries->ha_trn_hdl = ha_trn_hdl;
    bcmtrm_trans_ha_state.update_f(trans->unit,
                                   BCMTRM_STATE_COMMITTED,
                                   *trans_id,
                                   ha_trn_hdl);
    /*
     * Commit the transaction. Do not change anything regarding this
     * transaction past this call to commit as the callback may occur prior to
     * the return of the commit.
     * The only activity allowed post the call to commit is error handling from
     * the commit itself.
     * This is under the assumption that the WAL callback will not happen if
     * there was an error during commit (remember that WAL calback indicates
     * error or completion of the operation.
     */
    rv = bcmltm_commit(trans->unit, *trans_id, cb, context);

    if (rv != SHR_E_NONE) {
        bcmtrm_trans_ha_state.cancel_f(trans->unit, *trans_id, ha_trn_hdl);
    }

    INCREMENT_OP_ID(*trans_id);
    /* Allow other threads to access the unit's modeled path */
    sal_mutex_give(mtx);

    if (rv != SHR_E_NONE) {
        if (!trans->syncronous) {
            sal_mutex_take(trans->lock_obj->mutex, SAL_MUTEX_FOREVER);
            trans->state = T_ACTIVE;
            trans->info.status = SHR_E_FAIL;
            sal_mutex_give(trans->lock_obj->mutex);
            bcmtrn_trans_cb_and_clean(trans,
                                      BCMLT_NOTIF_OPTION_HW,
                                      rv);
        } else {
            bcmtrm_trans_entries_state_set(trans, E_ACTIVE);
        }
    }

    return rv;
}

/*
 * Important to remember that once the last entry was staged than the
 * application must be notified before the commit. The reason is that as a
 * result of commit, it is possible that the h/w commit callback will happen
 * before we inform the application about the transaction status of committed.
 * The result might be that the application will be notified about h/w
 * completion before commitment. Informing the application about transaction
 * committed after the last entry was staged solves this issue.
 */
static int handle_batch(bcmtrm_trans_t *trans,
                        uint32_t *op_id,
                        uint32_t *trans_id,
                        void *user_data)
{
    int rv;
    bcmtrm_entry_t *entry;
    uint32_t committed_count = 0;
    uint32_t ha_trn_hdl;
    sal_mutex_t mtx;
    uint32_t num_of_entries = trans->info.num_entries;
    uint32_t count;
    int last_unit = -1;
    bool mutex_taken = false;
    bool recorded = false;

    for (entry = trans->l_entries, count = 0;
         entry && (count < num_of_entries);
         entry = entry->next, count++) {
        assert (entry->p_trans == trans);
        /* Send interactive entries into the interactive queue */
        if ((entry->interactive) || (entry->pt)) {
            bcmtrm_q_interactive_entry(entry, trans->priority);
            continue;
        }

        /* Each entry should be execute exclusively (per unit) */
        if (mutex_taken && entry->info.unit != last_unit) {
            sal_mutex_give(mtx);   /* Allow other transactions */
            mutex_taken = false;
        }
        if (!mutex_taken) {
            mtx = bcmtrm_unit_mutex_get(entry->info.unit);
            last_unit = entry->info.unit;
            sal_mutex_take(mtx, SAL_MUTEX_FOREVER);
            mutex_taken = true;
        }
        if (!recorded) {
            /* Record the transaction once */
            bcmtrm_trans_rec(trans);
            recorded = true;
        }
        bcmtrm_trans_ha_state.set_f(entry->info.unit,
                                    BCMTRM_STATE_STAGING,
                                    *trans_id,
                                    &ha_trn_hdl);

        rv = bcmtrm_stage_entry(entry,
                               *op_id,
                               *trans_id);
        entry->info.status = rv;

        if (rv != SHR_E_NONE) {
            bcmltm_abort(trans->unit, *trans_id);
            bcmtrm_trans_ha_state.cancel_f(trans->unit, *trans_id, ha_trn_hdl);

            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(entry->info.unit,
                              "Batch transaction operation failed trans_id=%u"),
                      *trans_id));
            entry->state = E_ACTIVE;
            trans->info.status = SHR_E_PARTIAL;
            sal_mutex_take(trans->lock_obj->mutex, SAL_MUTEX_FOREVER);
            committed_count = ++trans->processed_entries;
            trans->committed_entries++;
            sal_mutex_give(trans->lock_obj->mutex);
        } else {   /* Staging was successful */
            entry->state = E_COMMITTED;
            sal_mutex_take(trans->lock_obj->mutex, SAL_MUTEX_FOREVER);
            trans->committed_entries++;
            trans->commit_success++;
            sal_mutex_give(trans->lock_obj->mutex);
            /* If this is the last entry check if need to inform the app */
            if (entry->next == NULL) {
                if (trans->commit_success == trans->committed_entries) {
                    trans->info.status = SHR_E_NONE;
                } else {
                    trans->info.status = SHR_E_PARTIAL;
                }

                if (!trans->syncronous) {
                    bcmtrm_appl_trans_inform(trans,
                                          trans->info.status,
                                          BCMLT_NOTIF_OPTION_COMMIT);
                }
            }

            if (trans->syncronous) {
                trans->usr_data = user_data;
            }

            entry->ha_trn_hdl = ha_trn_hdl;
            bcmtrm_trans_ha_state.update_f(trans->unit,
                                           BCMTRM_STATE_COMMITTED,
                                           *trans_id,
                                           ha_trn_hdl);
            if (trans->state == T_COMMITTING) {
                trans->state = T_COMMITTED;
            }
            rv = bcmltm_commit(trans->unit, *trans_id,
                               trm_async_trans_cb, entry);
            if (rv != 0) {
                LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META_U(trans->unit,
                                "Batch transaction entry failed commit "\
                                "trans_id=%d error=%s"),
                          *trans_id, shr_errmsg(rv)));
                bcmtrm_trans_ha_state.cancel_f(trans->unit,
                                               *trans_id,
                                               ha_trn_hdl);
                entry->info.status = SHR_E_FAIL;
                entry->state = E_ACTIVE;
                sal_mutex_take(trans->lock_obj->mutex, SAL_MUTEX_FOREVER);
                committed_count = ++trans->processed_entries;
                trans->commit_success--;
                sal_mutex_give(trans->lock_obj->mutex);
                trans->info.status = SHR_E_PARTIAL;
            }
        } /* End of else (handling successful staging) */

        INCREMENT_OP_ID(*trans_id);
    }  /* End of for loop */

    if (mutex_taken) {
        sal_mutex_give(mtx);
        mutex_taken = false;
    }
    /*
     * Beyond this point we can't trust the transaction pointer unless the if
     * statement below is true.
     * If the last entry failed then the notification thread will not be able to
     * detect this. So we make the callback here.
     */
    if (committed_count == num_of_entries) {
        if (trans->commit_success == 0) {
            trans->info.status = SHR_E_FAIL;
        }
        if (!bcmtrm_trans_complete(trans)) {
            trans->state = T_ACTIVE;
            bcmtrn_trans_cb_and_clean(trans,
                                      BCMLT_NOTIF_OPTION_HW,
                                      trans->info.status);
        }
    }

    return SHR_E_NONE;
}

void bcmtrm_appl_trans_inform(bcmtrm_trans_t *trans,
                              shr_error_t status,
                              bcmlt_notif_option_t notif_opt)
{
    bcmlt_notif_option_t notif_actual;

    /*
     * No callback for transactions which are:
     * a. Have no callback or
     * b. Asked not to be notified or
     * c. Marked for deletion
     */
    if ((!trans->cb) ||
        (trans->info.notif_opt == BCMLT_NOTIF_OPTION_NO_NOTIF) ||
        trans->delete) {
        return;
    }

    if (status != SHR_E_NONE) {
        if ((trans->info.notif_opt == BCMLT_NOTIF_OPTION_ALL) ||
            (trans->info.notif_opt == notif_opt)) {
            notif_actual = notif_opt;
        } else {
            notif_actual = trans->info.notif_opt;
        }
        /*
         * For error this should be the last notification. Partial error,
         * however, is a special case where multiple notifications are
         * justified.
         */
        if (status != SHR_E_PARTIAL) {
            trans->info.notif_opt = BCMLT_NOTIF_OPTION_NO_NOTIF;
        }
    } else if ((trans->info.notif_opt != BCMLT_NOTIF_OPTION_ALL) &&
               (trans->info.notif_opt != notif_opt)) {
        return;
    } else {
        notif_actual = notif_opt;
    }
    /*
     * Past this point we can't refer to the transaction as it is possible that
     * within the callback the OS will switch into a new thread that will free
     * this transaction and will allocate it for something else or simply
     * repurposed the same transaction for another commit.
     */
    trans->cb(notif_actual, &trans->info, trans->usr_data);
}

void bcmtrn_trans_cb_and_clean(bcmtrm_trans_t *trans,
                               bcmlt_notif_option_t notif_opt,
                               shr_error_t status)
{
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(trans->unit, "notif_opt=%d\n"), notif_opt));

    /* Set the state of all entries to ACTIVE */
    bcmtrm_trans_entries_state_set(trans, E_ACTIVE);

    /* Inform the application */
    bcmtrm_appl_trans_inform(trans, status, notif_opt);
}

bool bcmtrm_trans_complete(bcmtrm_trans_t *trans)
{
    bool del_trans = false;

    if (!trans->syncronous) {
        sal_mutex_take(trans->lock_obj->mutex, SAL_MUTEX_FOREVER);
        trans->state = T_ACTIVE;
        del_trans = trans->delete;
        sal_mutex_give(trans->lock_obj->mutex);

        if (del_trans) {
            /* Protect the transaction done with the transaction mutex */
            sal_mutex_take(trans->trans_free_sync, SAL_MUTEX_FOREVER);
            bcmtrm_trans_done(trans);
            sal_mutex_give(trans->trans_free_sync);
        }
    }
    return del_trans;
}

int bcmtrm_handle_transaction(bcmtrm_trans_t *trans,
                              uint32_t *op_id,
                              uint32_t *trans_id,
                              void *user_data)
{
    int rv;

    trans->processed_entries = 0;
    switch (trans->info.type) {
        case BCMLT_TRANS_TYPE_ATOMIC:
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(trans->unit,
                                    "Processing atomic transaction id=%d\n"),
                         *trans_id));
            rv = handle_atomic(trans, op_id, trans_id, user_data);
            break;
        case BCMLT_TRANS_TYPE_BATCH:
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(trans->unit,
                                    "Processing batch transaction id=%d\n"),
                         *trans_id));
            rv = handle_batch(trans, op_id, trans_id, user_data);
            break;
        default:
            rv = SHR_E_INTERNAL;
    }

    return rv;
}

/*
 * Interactive transaction contains only interactive entries. In this case
 * the implementation simply goes through each entry of the transaction
 * until it completes. The entries results will be processed by the
 * bcmtrm_process_interactive_entry_req() functions and by the notification
 * thread which eventually will call the transaction callback.
 */
int bcmtrm_handle_interactive_transaction(bcmtrm_trans_t *trans,
                                          uint32_t *op_id,
                                          trn_interact_info_t *ha_mem)
{
    bcmtrm_entry_t *entry;
    uint32_t count, j;

    count = trans->info.num_entries;
    for (j = 0, entry = trans->l_entries;
         j < count && entry;
         j++, entry = entry->next) {
        assert(entry->p_trans == trans);
        bcmtrm_process_interactive_entry_req(entry, *op_id, ha_mem);
        INCREMENT_OP_ID(*op_id);
    }

    return SHR_E_NONE;
}

static void recovery_cb(uint32_t trans_id,
                        shr_error_t status,
                        void *context)
{
    trn_state_t *trn_state = (trn_state_t *)context;

    assert(trn_state != NULL);
    assert(trans_id == trn_state->trn_id);
    trn_state->state = BCMTRM_STATE_NONE;
}

int bcmtrm_recover_trans (int unit, trn_state_t *trn_state)
{
    int rv;
    int wait_count = 0;

    if (trn_state->state == BCMTRM_STATE_NONE) {
        return SHR_E_NONE;
    } else if (trn_state->state == BCMTRM_STATE_STAGING) {
        rv = bcmltm_abort(unit, trn_state->trn_id);
    } else {    /* Commit again */
        rv = bcmltm_commit(unit, trn_state->trn_id, recovery_cb, trn_state);
        if (rv == SHR_E_NONE) {
            while (trn_state->state != BCMTRM_STATE_NONE &&
                   wait_count < 100000) {
                sal_usleep(100);
                wait_count++;
            }
            if (wait_count >= 100000) {
                rv = SHR_E_BUSY;
            }
        }
    }
    return rv;
}

