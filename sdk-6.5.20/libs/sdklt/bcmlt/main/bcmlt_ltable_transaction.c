/*! \file bcmlt_ltable_transaction.c
 *
 *  Handles logical tables transactions.
 *  This file handles all the functionality of the API that is transaction
 *  based.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmlt/bcmlt.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_lmem_mgr.h>
#include "bcmlt_internal.h"

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMLT_TRANS

/* Sanity check macro */
#define TRANS_VALIDATE(trans) \
    if (!trans || (trans->state != T_ACTIVE)) {                             \
        if (!trans) {                                                       \
            LOG_VERBOSE(BSL_LOG_MODULE,                                     \
                        (BSL_META("Invalid transaction handle\n")));        \
        } else {                                                            \
            LOG_VERBOSE(BSL_LOG_MODULE,                                     \
                        (BSL_META("Transaction was committed\n")));         \
        }                                                                   \
        SHR_ERR_EXIT(SHR_E_PARAM);                                   \
    }

int bcmlt_transaction_allocate(bcmlt_trans_type_t type,
                               bcmlt_transaction_hdl_t *trans_hdl)
{
    bcmtrm_trans_t *trans;
    sal_mutex_t mtx = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if (!bcmlt_is_initialized()) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    if (!trans_hdl || (type < BCMLT_TRANS_TYPE_ATOMIC) ||
        (type >= BCMLT_TRANS_TYPE_NUM)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    mtx = bcmlt_entry_sync_obj_get();
    if (mtx) {
        sal_mutex_take(mtx, SAL_MUTEX_FOREVER);
    }
    trans = bcmtrm_trans_alloc(type);
    if (!trans) {
        if (type == BCMLT_TRANS_TYPE_ATOMIC && !bcmlt_is_atomic_ena()) {
            SHR_ERR_EXIT(SHR_E_DISABLED);
        } else {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }
    }

    SHR_IF_ERR_EXIT(bcmlt_hdl_alloc(trans, trans_hdl));
    trans->info.trans_hdl = *trans_hdl;
    trans->trans_free_sync = mtx;
exit:
    if (mtx) {
        sal_mutex_give(mtx);
    }
    SHR_FUNC_EXIT();
}

int bcmlt_transaction_free(bcmlt_transaction_hdl_t trans_hdl)
{
    bcmtrm_trans_t *trans = bcmlt_hdl_data_get(trans_hdl);
    bcmtrm_entry_t *entry;
    sal_mutex_t mtx = NULL;

    SHR_FUNC_ENTER(trans ? trans->unit : BSL_UNIT_UNKNOWN);
    if (!trans || (trans->state == T_IDLE)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    mtx = trans->trans_free_sync;
    if (mtx) {
        sal_mutex_take(mtx, SAL_MUTEX_FOREVER);
    }
    /* Clean all the entries handles as they were allocated in this layer */
    entry = trans->l_entries;
    while (entry) {
        SHR_IF_ERR_EXIT(bcmlt_hdl_free(entry->info.entry_hdl));
        entry->info.entry_hdl = 0;
        entry = entry->next;
    }
    SHR_IF_ERR_EXIT(bcmtrm_trans_free(trans));

    SHR_IF_ERR_EXIT(bcmlt_hdl_free(trans_hdl));
exit:
    if (mtx) {
        sal_mutex_give(mtx);
    }
    SHR_FUNC_EXIT();
}

/*
 * Entry should be linked according to the order they were inserted.
 * This will enable the function bcmlt_transaction_entry_num_get() to make
 * sense.
*/
int bcmlt_transaction_entry_add(bcmlt_transaction_hdl_t trans_hdl,
                                bcmlt_opcode_t opcode,
                                bcmlt_entry_handle_t entry_hdl)
{
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);
    bcmtrm_trans_t *trans = bcmlt_hdl_data_get(trans_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    TRANS_VALIDATE(trans);
    ENTRY_VALIDATE(entry);
    LT_OPCODE_VALIDATE(opcode, entry->info.unit);
    if (entry->pt) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                                "Use the PT functions for PT entry\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (entry->p_trans) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                                "Entry already belong to a transaction\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Don't allow interactive entry in atomic transaction */
    if ((trans->info.type == BCMLT_TRANS_TYPE_ATOMIC) &&
        (entry->interactive)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                                "Can't use atomic transaction for "\
                                "interactive tables\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (trans->l_entries) {  /* Not the first entry */
        if (trans->pt_trans) { /* Don't allow mixed transactions */
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(entry->info.unit,
                                    "Can't mix LT and PT entries in the "\
                                    "same transaction\n")));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        /* For now do not allow transaction which spans multiple units */
        if (trans->unit != entry->info.unit) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                      (BSL_META_U(entry->info.unit,
                                   "No support for multi-unit transaction\n")));
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
    } else {   /* First entry */
        trans->pt_trans = false;
        trans->unit = entry->info.unit;
    }

    /* Clear get from hw attribute if opcode is not lookup */
    if (opcode == BCMLT_OPCODE_LOOKUP) {
        /* Clean out all fields other than key fields. */
        SHR_IF_ERR_EXIT(bcmlt_entry_clean_data_fields(entry));
    } else if (opcode == BCMLT_OPCODE_TRAVERSE) {
        /* Clean out all fields other than key fields. */
        if (entry->l_field) {
            SHR_IF_ERR_EXIT(bcmlt_entry_clean_data_fields(entry));
            if (!entry->l_field) {   /* If no key fields don't search again */
                LOG_VERBOSE(BSL_LOG_MODULE,
                          (BSL_META_U(entry->info.unit,
                                      "Only the first traverse operation can" \
                                      "have no key fields\n")));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }
        /* Make sure that there are no conflicting snapshot flags */
        if ((entry->attrib & BCMLT_ENT_ATTR_TRAVERSE_SNAPSHOT) &&
            (entry->attrib & BCMLT_ENT_ATTR_TRAVERSE_DONE)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        entry->attrib &= ~BCMLT_ENT_ATTR_GET_FROM_HW;
    }

    entry->opcode.lt_opcode = opcode;
    /* Add entry to the end of the list */
    entry->next = NULL;
    if (!trans->l_entries) {
        trans->l_entries = entry;
    } else {
        trans->last_entry->next = entry;
    }
    trans->last_entry = entry;
    entry->p_trans = trans;
    trans->info.num_entries++;
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_transaction_pt_entry_add(bcmlt_transaction_hdl_t trans_hdl,
                                   bcmlt_pt_opcode_t opcode,
                                   bcmlt_entry_handle_t entry_hdl)
{
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);
    bcmtrm_trans_t *trans = bcmlt_hdl_data_get(trans_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    TRANS_VALIDATE(trans);
    ENTRY_VALIDATE(entry);
    PT_OPCODE_VALIDATE(opcode, entry->info.unit);
    if (!entry->pt) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                                "Use the PT functions only for PT entry\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (entry->p_trans) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                                "Entry already belongs to a transaction\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (trans->info.type == BCMLT_TRANS_TYPE_ATOMIC) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                              "Can't use atomic transaction for PT tables\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (trans->l_entries) {
        if (!trans->pt_trans) { /* Don't allow mixed transactions */
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(entry->info.unit,
                                    "Can't mix LT and PT entries in the "\
                                    "same transaction\n")));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        /* For now do not allow transaction which spans multiple units */
        if (trans->unit != entry->info.unit) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(entry->info.unit,
                                   "No support for multi-unit transaction\n")));
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
    } else {
        trans->pt_trans = true;
        trans->unit = entry->info.unit;
    }
    entry->opcode.pt_opcode = opcode;

    /* Add entry to the end of the list */
    entry->next = NULL;
    if (!trans->l_entries) {
        trans->l_entries = entry;
    } else {
        trans->last_entry->next = entry;
    }
    trans->last_entry = entry;
    entry->p_trans = trans;
    trans->info.num_entries++;
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_transaction_entry_delete(bcmlt_transaction_hdl_t  trans_hdl,
                                   bcmlt_entry_handle_t  entry_hdl)
{
    bcmtrm_entry_t *entry;
    bcmtrm_entry_t *entry_prev = NULL;
    bcmtrm_trans_t *trans = bcmlt_hdl_data_get(trans_hdl);

    SHR_FUNC_ENTER(trans ? trans->unit : BSL_UNIT_UNKNOWN);
    /* Can only delete from non-committed transactions */
    TRANS_VALIDATE(trans);
    if ((trans->info.num_entries == 0) || !entry_hdl) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    for (entry = trans->l_entries; entry; entry = entry->next) {
        if (entry->info.entry_hdl == entry_hdl) {
            if (entry_prev) {
                entry_prev->next = entry->next;
            } else {
                trans->l_entries = entry->next;
            }
            if (entry == trans->last_entry) {
                trans->last_entry = entry_prev;
            }
            break;
        }
        entry_prev = entry;
    }
    SHR_NULL_CHECK(entry, SHR_E_PARAM); /* Didn't find the entry */
    entry->p_trans = NULL;

    trans->info.num_entries--;
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_transaction_info_get(bcmlt_transaction_hdl_t trans_hdl,
                               bcmlt_transaction_info_t  *trans_info)
{
    bcmtrm_trans_t *trans = bcmlt_hdl_data_get(trans_hdl);

    SHR_FUNC_ENTER(trans ? trans->unit : BSL_UNIT_UNKNOWN);
    if (!trans || (trans->state == T_IDLE) || !trans_info) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    *trans_info = trans->info;
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_transaction_commit_async(bcmlt_transaction_hdl_t trans_hdl,
        bcmlt_notif_option_t notif_opt,
        void *user_data,
        bcmlt_trans_cb notif_fn,
        bcmlt_priority_level_t priority)
{
    bcmtrm_trans_t *trans = bcmlt_hdl_data_get(trans_hdl);
    bcmtrm_entry_t *entry;

    SHR_FUNC_ENTER(trans ? trans->unit : BSL_UNIT_UNKNOWN);
    TRANS_VALIDATE(trans);
    PRIORITY_VALIDATE(priority, trans->unit);
    NOTIFICATION_VALIDATE(notif_opt, notif_fn, trans->unit);
    if (trans->info.num_entries == 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    entry = trans->l_entries;
    while (entry) {
        if (entry->fld_mem_hdl) {
            SHR_ERR_EXIT(SHR_E_DISABLED);
        }
        entry = entry->next;
    }
    trans->cb = notif_fn;
    trans->info.notif_opt = notif_opt;
    trans->usr_data = user_data;
    trans->priority = priority;

    if (bsl_fast_check(BSL_LS_BCMLT_DETAILS | BSL_VERBOSE)) {
        bcmlt_dump_trans(trans);
    }
    SHR_IF_ERR_EXIT(bcmtrm_asynch_trans_req(trans));
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_transaction_commit(bcmlt_transaction_hdl_t trans_hdl,
                             bcmlt_priority_level_t priority)
{
    bcmtrm_trans_t *trans = bcmlt_hdl_data_get(trans_hdl);

    SHR_FUNC_ENTER(trans ? trans->unit : BSL_UNIT_UNKNOWN);
    TRANS_VALIDATE(trans);
    PRIORITY_VALIDATE(priority, trans->unit);

    if (trans->info.num_entries == 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    trans->priority = BCMLT_PRIORITY_NORMAL;
    trans->info.notif_opt = BCMLT_NOTIF_OPTION_NO_NOTIF;

    if (bsl_fast_check(BSL_LS_BCMLT_DETAILS | BSL_VERBOSE)) {
        bcmlt_dump_trans(trans);
    }
    SHR_IF_ERR_EXIT(bcmtrm_trans_req(trans));
    SHR_IF_ERR_EXIT(trans->info.status);
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_transaction_entry_num_get(bcmlt_transaction_hdl_t trans_hdl,
                                    uint32_t entry_num,
                                    bcmlt_entry_info_t  *entry_info)
{
    bcmtrm_entry_t *entry;
    uint32_t j;
    bcmtrm_trans_t *trans = bcmlt_hdl_data_get(trans_hdl);

    SHR_FUNC_ENTER(trans ? trans->unit : BSL_UNIT_UNKNOWN);
    if (!trans || !entry_info ||
        (trans->state == T_IDLE) ||
        (entry_num >= trans->info.num_entries)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    for (j = 0, entry = trans->l_entries;
          j < entry_num;
          j++, entry = entry->next);

    *entry_info = entry->info;
exit:
    SHR_FUNC_EXIT();
}

