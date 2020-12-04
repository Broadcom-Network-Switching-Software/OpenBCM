/*! \file trm_notify.c
 *
 * This module implements the notification functionality of the
 * transaction manager. The notification being made to the application
 * in case where a transaction or entry operation changes state or in
 * the case that a table had changed and required notification.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <bsl/bsl.h>
#include <sal/sal_alloc.h>
#include <sal/sal_msgq.h>
#include <sal/sal_mutex.h>
#include <sal/sal_sem.h>
#include <sal/sal_assert.h>
#include <sal/sal_sleep.h>
#include <shr/shr_error.h>
#include <bcmdrd_config.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltm/bcmltm.h>
#include <bcmtrm/trm_api.h>
#include "trm_internal.h"

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMTRM_ENTRY

typedef struct {
    bcmlt_table_sub_cb cb;
    void *user_data;
} table_event_info;

static table_event_info *trm_unit_tbl_event[BCMDRD_CONFIG_MAX_UNITS];

static bcmtrm_entry_alloc_assist_f e_alloc_assist;
static bcmtrm_entry_free_assist_f e_free_assist;
static bcmlt_replay_entry_record_f e_rec;
static bcmlt_replay_trans_record_f t_rec;


bool bcmltm_notify_units[BCMDRD_CONFIG_MAX_UNITS];
static size_t max_tbl_id[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Public functions
 */
void bcmtrm_tbl_chg_event(bcmtrm_entry_t *p_entry)
{
    bcmlt_opcode_t opcode = p_entry->opcode.lt_opcode;
    if (p_entry->pt) {    /* no change events for PT tables */
        return;
    }
    if (((opcode == BCMLT_OPCODE_INSERT) ||
         (opcode == BCMLT_OPCODE_UPDATE) ||
         (opcode == BCMLT_OPCODE_DELETE)) &&
        (max_tbl_id[p_entry->info.unit] > (size_t)p_entry->table_id) &&
        trm_unit_tbl_event[p_entry->info.unit][p_entry->table_id].cb) {
        table_event_info *p_table_event =
            &trm_unit_tbl_event[p_entry->info.unit][p_entry->table_id];
        bcmlt_table_notif_info_t table_info;
        table_info.unit = p_entry->info.unit;
        table_info.entry_hdl = p_entry->info.entry_hdl;
        table_info.table_name = p_entry->info.table_name;
        p_table_event->cb(&table_info, p_table_event->user_data);
    }
}


void bcmtrm_notify_thread(shr_thread_t t_hdl, void *arg)
{
#define ONE_SEC_WAIT    1000000
    bcmtrm_hw_notif_struct_t hw_notif;
    int rv;

    while (!shr_thread_stopping(t_hdl)) {
        rv = sal_msgq_recv(bcmtrm_hw_notif_q, (void *)&hw_notif, ONE_SEC_WAIT);
        if (rv == SAL_MSGQ_E_TIMEOUT) {
            continue;
        }
        if (rv != 0) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META("Failed to receive hw notify message rv=%d\n"),
                      rv));
            break;
        }
        switch (hw_notif.type) {
        case NOTIF_EXIT:
            break;
        case ENTRY_CB:
            {
                bcmtrm_entry_t *entry = hw_notif.ltm_entry;
                bool entry_complete;
                /* Don't process invalid units */
                if (!bcmltm_notify_units[entry->info.unit]) {
                    break;
                }
                if (hw_notif.status == SHR_E_NONE && entry->info.entry_hdl) {
                    bcmtrm_tbl_chg_event(entry);
                }
                /*
                 * Set the status. The function
                 * bcmtrm_proc_entry_results() may override the status.
                 */
                entry->info.status = hw_notif.status;
                bcmtrm_proc_entry_results(entry, entry->ltm_entry);
                if (entry->asynch) {
                    entry_complete =
                        entry->info.notif_opt == BCMLT_NOTIF_OPTION_NO_NOTIF ||
                        entry->delete;
                    bcmtrm_entry_complete(entry);
                } else {
                    entry_complete = false;
                    entry->state = E_ACTIVE;   /* The entry is done */
                }
                /* Inform the application if needed */
                if (!entry_complete) {
                    bcmtrm_appl_entry_inform(entry,
                                             hw_notif.status,
                                             BCMLT_NOTIF_OPTION_HW);
                }
                break;
            }
        case TRANS_CB:
            {
                bcmtrm_entry_t *entry = hw_notif.ltm_entry;
                bcmtrm_trans_t *trans = entry->p_trans;
                bool trans_complete = false;
                /* Don't process invalid units */
                if (!bcmltm_notify_units[entry->info.unit]) {
                    break;
                }
                if (trans->info.type == BCMLT_TRANS_TYPE_ATOMIC) {
                    /*
                     * This is the only notification. Update the status of each
                     * entry with this status - maybe not necessary
                     */
                    bcmtrm_entry_t *entry_it;
                    for (entry_it = trans->l_entries;
                          entry_it;
                          entry_it = entry_it->next) {
                        /*
                         * Set the status. The function
                         * bcmtrm_proc_entry_results() may override the status.
                         */
                        entry_it->info.status = hw_notif.status;
                        if (entry_it->ltm_entry) {
                            bcmtrm_proc_entry_results(entry_it,
                                                      entry_it->ltm_entry);
                        }
                        if (hw_notif.status == SHR_E_NONE &&
                            entry_it->info.entry_hdl) {
                            bcmtrm_tbl_chg_event(entry_it);
                        }
                    }
                    trans->info.status = hw_notif.status;
                    if (trans->syncronous) {
                        trans->state = T_ACTIVE;
                        trans_complete = false;
                    } else {
                        trans_complete =
                            trans->info.notif_opt ==
                            BCMLT_NOTIF_OPTION_NO_NOTIF || trans->delete;
                        bcmtrm_trans_complete(trans);
                    }
                    if (!trans_complete) {
                        bcmtrn_trans_cb_and_clean(trans,
                                                  BCMLT_NOTIF_OPTION_HW,
                                                  hw_notif.status);
                    }
                } else {
                    uint32_t processed_entries;
                    /*
                     * Set the status. The function
                     * bcmtrm_proc_entry_results() may override the status.
                     */
                    entry->info.status = hw_notif.status;
                    if (entry->ltm_entry) {
                        bcmtrm_proc_entry_results(entry,
                                                  entry->ltm_entry);
                    }
                    if (hw_notif.status == SHR_E_NONE && entry->info.entry_hdl) {
                        bcmtrm_tbl_chg_event(entry);
                    }
                    /* Done with the entry so set its state accordingly */
                    entry->state = E_ACTIVE;
                    /* Lock transaction db */
                    sal_mutex_take(trans->lock_obj->mutex, SAL_MUTEX_FOREVER);
                    processed_entries = ++trans->processed_entries;
                    sal_mutex_give(trans->lock_obj->mutex);
                    if (processed_entries == trans->info.num_entries) {
                        if (trans->syncronous) {
                            /*
                             * For synchronous we can set it all here. We know
                             * that nothing can change the status or
                             * delete flag values. We have to be sure that the
                             * callback is the last thing since after that
                             * the entry can be deleted under our feet (since
                             * the state changes to T_ACTIVE (required for
                             * synchronous operations before calling the
                             * callback)).
                             */
                            trans->state = T_ACTIVE;
                            trans_complete = false;
                        } else {
                            /*
                             * In asynchronous mode we simply set the
                             * transaction complete, so it can be cleaned up
                             * at the end. Here the state is not necessarily
                             * has to be T_ACTIVE to free the transaction.
                            */
                            trans_complete = bcmtrm_trans_complete(trans);
                        }
                        if (!trans_complete) {
                            if (trans->commit_success == processed_entries) {
                                trans->info.status = SHR_E_NONE;
                            } else if (trans->commit_success == 0) {
                                trans->info.status = SHR_E_FAIL;
                            } else {
                                trans->info.status = SHR_E_PARTIAL;
                            }
                            bcmtrn_trans_cb_and_clean(trans,
                                                      BCMLT_NOTIF_OPTION_HW,
                                                      trans->info.status);
                        }
                    }
                }

                break;
            }
        case ENTRY_NOTIF:
            {
                bcmtrm_entry_t *entry = hw_notif.ltm_entry;

                bcmtrm_tbl_chg_event(entry);

                if (e_free_assist) {
                    e_free_assist(entry);
                }
                break;
            }
        default:
            assert(0);
        }
    }
}

int bcmtrm_table_event_subscribe(int unit,
                                 uint32_t table_id,
                                 bcmlt_table_sub_cb event_func,
                                 void *user_data)
{
    table_event_info *p_event = &trm_unit_tbl_event[unit][table_id];

    p_event->cb = event_func;
    p_event->user_data = user_data;
    return SHR_E_NONE;
}

int bcmtrm_table_event_alloc(int unit)
{
    size_t num_of_tables;
    int rv = SHR_E_NONE;

    num_of_tables = BCMLRD_TABLE_COUNT;
    do {
        if (num_of_tables > 0) {
            trm_unit_tbl_event[unit] =
                sal_alloc(num_of_tables * sizeof(table_event_info),
                          "bcmtrmNotify");
            if (!trm_unit_tbl_event[unit]) {
                rv = SHR_E_MEMORY;
                break;
            }
            sal_memset(trm_unit_tbl_event[unit],
                       0,
                       num_of_tables * sizeof(table_event_info));
            max_tbl_id[unit] = num_of_tables;
        }
    } while (0);
    return rv;
}

void bcmtrm_table_event_free(int unit)
{
    if (trm_unit_tbl_event[unit] != NULL) {
        sal_free(trm_unit_tbl_event[unit]);
        trm_unit_tbl_event[unit] = NULL;
        max_tbl_id[unit] = 0;
    }
}

void bcmtrm_register_entry_assist(bcmtrm_entry_alloc_assist_f alloc_cb,
                                  bcmtrm_entry_free_assist_f free_cb,
                                  bcmlt_replay_entry_record_f entry_rec,
                                  bcmlt_replay_trans_record_f trans_rec)
{
    e_alloc_assist = alloc_cb;
    e_free_assist = free_cb;
    e_rec = entry_rec;
    t_rec = trans_rec;
}

int bcmtrm_sb_table_notify(int unit,
                           bool high_pri,
                           uint32_t table_id,
                           bcmlt_opcode_t opcode,
                           bcmltm_field_list_t *fields)
{
    bcmtrm_entry_t *entry;
    bcmtrm_hw_notif_struct_t hw_notif;

    if (!fields) {
        return SHR_E_PARAM;
    }
    if (!e_alloc_assist) {
        return SHR_E_INIT;
    }

    if (!trm_unit_tbl_event[unit] || !trm_unit_tbl_event[unit][table_id].cb) {
        /* No callback */
        bcmltm_field_list_t *tmp_f;
        bcmltm_field_list_t *working_fld = fields;

        while (working_fld) {
            tmp_f = working_fld;
            working_fld = working_fld->next;
            shr_fmm_free(tmp_f);
        }
        return SHR_E_NONE;
    }
    if (e_alloc_assist(unit, table_id, opcode, fields, &entry) != SHR_E_NONE) {
        return SHR_E_PARAM;
    }

    entry->info.status = SHR_E_NONE;

    hw_notif.type = ENTRY_NOTIF;
    hw_notif.ltm_entry = entry;
    hw_notif.status = SHR_E_NONE;
    /* post the event to the notification thread */
    sal_msgq_post(
        bcmtrm_hw_notif_q,
        &hw_notif,
        (high_pri ? SAL_MSGQ_HIGH_PRIORITY : SAL_MSGQ_NORMAL_PRIORITY),
        SAL_MSGQ_FOREVER);

    return SHR_E_NONE;
}

int bcmtrm_entry_rec(bcmtrm_entry_t *entry)
{
    if (e_rec) {
        return e_rec(entry);
    }
    return SHR_E_NONE;
}

int bcmtrm_trans_rec(bcmtrm_trans_t *trans)
{
    if (t_rec) {
        return t_rec(trans);
    }
    return SHR_E_NONE;
}
