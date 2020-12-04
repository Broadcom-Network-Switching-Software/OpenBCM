/*! \file tr_mgr_no_trd.c
 *
 * Transaction manager no thread implementation.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <bsl/bsl.h>
#include <sal/sal_mutex.h>
#include <sal/sal_sem.h>
#include <sal/sal_alloc.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_lmem_mgr.h>
#include <shr/shr_fmm.h>
#include <bcmdrd_config.h>
#include <bcmltm/bcmltm.h>
#include <bcmtrm/trm_api.h>
#include <bcmtrm/generated/bcmtrm_ha.h>
#include "trm_internal.h"

#define BSL_LOG_MODULE BSL_LS_BCMTRM_ENTRY

/*!
 * \brief Process modeled path entry.
 *
 * This function processes modeled path entry by staging it first and then
 * handle the staging results. For successful staging the function will commit
 * the entry using the provided callback function. For failed staging the
 * function will abort the transaction.
 *
 * \param [in] entry The entry to process.
 * \param [in] unit_res Is the unit resources used for the processing of the
 * entry.
 * \param [in] cb Is a caller provided callback function that will be called
 * once the entry was installed into h/w.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int process_modeled_entry(bcmtrm_entry_t *entry,
                                 bcmtrm_unit_resources_t *unit_res,
                                 bcmptm_trans_cb_f  cb)
{
    int rv;

    rv = bcmtrm_stage_entry(entry, 0, unit_res->trans_id);
    if (rv != SHR_E_NONE) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(entry->info.unit,
                             "Stage failed rv=%d opcode=%d\n"),
                  rv, entry->opcode.lt_opcode));
        bcmltm_abort(unit_res->unit, unit_res->trans_id);
    } else {
        rv = bcmltm_commit(unit_res->unit, unit_res->trans_id, cb, entry);
        INCREMENT_OP_ID(unit_res->trans_id);
    }
    return rv;
}

/*!
 * \brief Syncronous entry operation callback.
 *
 * This function being called by the WAL read thread after the entry was
 * installed into the h/w.
 * The function signals the entry user data so that the entry commit request
 * function can return knowing that the operation has been installed into the
 * hardware.
 *
 * \param [in] trans_id Is the transaction ID of the complete operation.
 * \param [in] status Is the status of the h/w operation.
 * \param [in] user_data Is the same user_data that was provided to the commit
 * (bcmltm_commit()) function.
 *
 * \return None.
 */
static void sync_entry_cb(uint32_t trans_id,
                          shr_error_t status,
                          void *user_data)
{
    bcmtrm_entry_t *entry = (bcmtrm_entry_t *)user_data;

    entry->info.status = status;
    sal_sem_give(entry->usr_data);
}

/*!
 * \brief Syncronous transaction operation callback.
 *
 * This function being called by the WAL read thread after an entry from a
 * transaction was installed into the h/w.
 * The function increments the transaction processed entries count. If this is
 * the last entry of the batch transaction the function signals the transaction
 * request function by signaling the transaction user data field.
 *
 * \param [in] trans_id Is the transaction ID of the complete operation.
 * \param [in] status Is the status of the h/w operation.
 * \param [in] user_data Is the same user_data that was provided to the commit
 * (bcmltm_commit()) function.
 *
 * \return None.
 */
static void sync_trans_cb(uint32_t trans_id,
                          shr_error_t status,
                          void *user_data)
{
    bcmtrm_entry_t *entry = (bcmtrm_entry_t *)user_data;
    bcmtrm_trans_t *trans = entry->p_trans;
    uint32_t processed;

    entry->state = E_ACTIVE;
    sal_mutex_take(trans->lock_obj->mutex, SAL_MUTEX_FOREVER);
    processed = ++trans->processed_entries;
    if (processed == trans->info.num_entries) {
        /*
         * Set commit_success to 1 to indicate that the last entry was
         * processed via the callback and therefore the semaphore was
         * signaled.
         */
        trans->commit_success = 1;
    }
    sal_mutex_give(trans->lock_obj->mutex);
    /* If processed the last entry */
    if (processed == trans->info.num_entries) {
        sal_sem_give(trans->usr_data);
    }
}

/*================================================================*
 *     C O M P O N E N T    I N T E R N A L   F U N C T I O N S   *
 *================================================================*/
int bcmtrm_no_trd_entry_req(bcmtrm_entry_t *entry,
                            bcmtrm_unit_resources_t *unit_res)
{
    int rv;

    entry->asynch = false;

    if (entry->interactive || entry->pt) {
        sal_mutex_take(unit_res->interact_mtx, SAL_SEM_FOREVER);
        bcmtrm_entry_rec(entry);
        unit_res->active_sync_trans_int++;
        rv = bcmtrm_stage_entry(entry, 0, 0);
        unit_res->active_sync_trans_int--;
        sal_mutex_give(unit_res->interact_mtx);
        entry->info.status = rv;
    } else {
        sal_mutex_take(unit_res->mtx, SAL_SEM_FOREVER);
        bcmtrm_entry_rec(entry);
        unit_res->active_sync_trans++;
        entry->usr_data = unit_res->sync_sem;
        entry->info.status = process_modeled_entry(entry,
                                                   unit_res,
                                                   sync_entry_cb);
        if (entry->info.status == SHR_E_NONE) {
            sal_sem_take(unit_res->sync_sem, SAL_SEM_FOREVER);
        }
        unit_res->active_sync_trans--;
        sal_mutex_give(unit_res->mtx);
    }
    if (entry->info.status == SHR_E_NONE) {
        bcmtrm_proc_entry_results(entry, entry->ltm_entry);
        bcmtrm_tbl_chg_event(entry);
    }
    entry->state = E_ACTIVE;

    return SHR_E_NONE;
}

int bcmtrm_no_trd_trans_req(bcmtrm_trans_t *trans,
                            bcmtrm_unit_resources_t *unit_res)
{
    int rv;
    bcmtrm_entry_t *entry;
    uint32_t success = 0;
    uint32_t num_of_entries = trans->info.num_entries;
    uint32_t processed;

    trans->syncronous = true;
    trans->processed_entries = 0;
    trans->commit_success = 0;

    if (trans->pt_trans) {
        sal_mutex_take(unit_res->interact_mtx, SAL_SEM_FOREVER);
        bcmtrm_trans_rec(trans);
        unit_res->active_sync_trans_int++;
        for (entry = trans->l_entries; entry; entry = entry->next) {
            assert(entry->p_trans == trans);
            rv = bcmtrm_stage_entry(entry, 0, 0);
            entry->info.status = rv;
            if (rv == SHR_E_NONE) {
                success++;
            }
            sal_mutex_take(trans->lock_obj->mutex, SAL_MUTEX_FOREVER);
            trans->processed_entries++;
            sal_mutex_give(trans->lock_obj->mutex);
            entry->state = E_ACTIVE;
        }
        unit_res->active_sync_trans_int--;
        sal_mutex_give(unit_res->interact_mtx);
    } else {
        trans->usr_data = unit_res->sync_sem;
        sal_mutex_take(unit_res->mtx, SAL_SEM_FOREVER);
        bcmtrm_trans_rec(trans);
        unit_res->active_sync_trans++;
        for (entry = trans->l_entries; entry; entry = entry->next) {
            assert(entry->p_trans == trans);
            if (entry->interactive) {
                sal_mutex_take(unit_res->interact_mtx, SAL_SEM_FOREVER);
                rv = bcmtrm_stage_entry(entry, 0, 0);
                sal_mutex_give(unit_res->interact_mtx);
                sal_mutex_take(trans->lock_obj->mutex, SAL_MUTEX_FOREVER);
                trans->processed_entries++;
                sal_mutex_give(trans->lock_obj->mutex);
                entry->info.status = rv;
                if (rv == SHR_E_NONE) {
                    success++;
                }
                entry->state = E_ACTIVE;
            } else {
                rv = process_modeled_entry(entry, unit_res, sync_trans_cb);
                if (rv != SHR_E_NONE) {
                    sal_mutex_take(trans->lock_obj->mutex, SAL_MUTEX_FOREVER);
                    trans->processed_entries++;
                    sal_mutex_give(trans->lock_obj->mutex);
                    entry->state = E_ACTIVE;
                }
                entry->info.status = rv;
                if (rv == SHR_E_NONE) {
                    success++;
                }
            }
        }
        unit_res->active_sync_trans--;

        sal_mutex_take(trans->lock_obj->mutex, SAL_MUTEX_FOREVER);
        processed = trans->processed_entries;
        sal_mutex_give(trans->lock_obj->mutex);
        /*
         * Need to wait for h/w Ack? Or the last entry was already acked.
         * If we staged all the entries and the number of processed entries is
         * smaller than the number of entries within the transaction it means
         * that there are other entries still under processing and will be
         * reported to the commit callback function sync_trans_cb().
         * We are using the commit_success counter of the transaction to
         * indicate that the callback function signaled (or about to signal)
         * the semaphore.
         */
        if (processed < num_of_entries || trans->commit_success > 0) {
            sal_sem_take(unit_res->sync_sem, SAL_SEM_FOREVER);
        }
        /*
         * The usage of unit_res->sync_sem should be exclusive to this unit. So
         * don't free the unit until the operation had completed.
         */
        sal_mutex_give(unit_res->mtx); /* Free the unit to other operations */
    }

    /* Process all the entries from the transaction */
    if (success > 0) {
        for (entry = trans->l_entries; entry; entry = entry->next)
        {
            /* Process only successful operations */
            if (entry->info.status == SHR_E_NONE) {
                if (entry->ltm_entry) {
                    bcmtrm_proc_entry_results(entry, entry->ltm_entry);
                }
                bcmtrm_tbl_chg_event(entry);
            }
        }
    }
    trans->state = T_ACTIVE;
    if (success == 0) {
        trans->info.status = SHR_E_FAIL;
    } else if (success == num_of_entries) {
        trans->info.status = SHR_E_NONE;
    } else {
        trans->info.status = SHR_E_PARTIAL;
    }

    return SHR_E_NONE;
}
