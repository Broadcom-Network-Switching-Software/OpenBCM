/*! \file bcmlt_ltable_entry.c
 *
 * Handles entry operations.
 * This file handles all operations that are related to entries including
 * entry allocation and free, entry commit and get entry info.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_mutex.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_lmem_mgr.h>
#include <bcmdrd_config.h>
#include <bcmlt/bcmlt.h>
#include <bcmtrm/trm_api.h>
#include "bcmlt_internal.h"

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMLT_ENTRY

/*
 * These synchronization objects will be used for the high performance (HP)
 * API. They will only be used during entry allocation and free.
 */
static sal_mutex_t hp_api_sync;

/*
 * These fields will be allocated in a bulk upon entry creation. Therefore we
 * can use our own field memory handler that is not thread safe. The thread
 * safety will be held by the entry allocation and free functions.
 */
static shr_lmm_hdl_t fields_hdl;

/*******************************************************************************
 * Public functions local to this component
 */
int bcmlt_entry_module_init()
{
#define FIELD_CHUNK_SIZE 20
    int rv;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if (hp_api_sync) {
        SHR_EXIT();
    }
    hp_api_sync = sal_mutex_create("bcmltHpEntry");
    SHR_NULL_CHECK(hp_api_sync, SHR_E_MEMORY);

    LMEM_MGR_INIT(shr_fmm_t, fields_hdl, FIELD_CHUNK_SIZE, false, rv);
    SHR_IF_ERR_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_module_shutdown()
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if (!hp_api_sync) {
        SHR_EXIT();
    }

    sal_mutex_destroy(hp_api_sync);
    hp_api_sync = NULL;

    shr_lmm_delete(fields_hdl);
    fields_hdl = NULL;

exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_clean_data_fields(bcmtrm_entry_t *entry)
{
#define MAX_KEY_FIELDS  128
    uint32_t key_ids[MAX_KEY_FIELDS];
    uint32_t key_count = MAX_KEY_FIELDS;
    uint32_t j;
    shr_fmm_t *field = entry->l_field;
    shr_fmm_t *prev_field = NULL;

    if (!entry->l_field) {
        return SHR_E_NONE;
    }

    sal_memset(key_ids, 0, sizeof(key_ids));
    bcmlt_db_table_keys_get(entry->info.unit, entry->db_hdl,
                            MAX_KEY_FIELDS, key_ids, &key_count);
    if (key_count > MAX_KEY_FIELDS) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(entry->info.unit,
                              "Entry has too many fields=%u max allowed %d\n"),
                   key_count, MAX_KEY_FIELDS));
        return SHR_E_UNAVAIL;
    }
    while (field) {
        for (j = 0; j < key_count; j++) {
            if (field->id == key_ids[j]) {
                break;
            }
        }
        if (j < key_count) {   /* Field is a key field */
            prev_field = field;
            field = field->next;
            continue;
        }

        /* Field is not key, delete it */
        if (entry->fld_arr) {
            entry->fld_arr[field->id] = NULL;
        }
        if (prev_field) {
            prev_field->next = field->next;
            if (entry->fld_mem_hdl) {
                field->next = entry->free_fld;
                entry->free_fld = field;
            } else {
                shr_fmm_free(field);
            }
            field = prev_field->next;
        } else {    /* First element in the list */
            entry->l_field = field->next;
            if (entry->fld_mem_hdl) {
                field->next = entry->free_fld;
                entry->free_fld = field;
            } else {
                shr_fmm_free(field);
            }
            field = entry->l_field;
        }
    }
    return SHR_E_NONE;
}

sal_mutex_t bcmlt_entry_sync_obj_get()
{
    return hp_api_sync;
}

/*******************************************************************************
 * Public functions
 */
int bcmlt_entry_allocate(int unit,
                         const char *table_name,
                         bcmlt_entry_handle_t *entry_hdl)
{
    bcmtrm_entry_t *entry;
    bcmlt_table_attrib_t *table_attr;
    void *hdl;
    shr_lmm_hdl_t fld_array_hdl;
    bool free_mutex = false;

    SHR_FUNC_ENTER(unit);
    if (!bcmlt_is_initialized()) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "BCMLT was not initialized yet\n")));
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    UNIT_VALIDATION(unit);
    if (!table_name || !entry_hdl) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Table name and entry handle shouln't"\
                                " be NULL\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    sal_mutex_take(hp_api_sync, SAL_MUTEX_FOREVER);
    free_mutex = true;
    /* Find the table ID associated with the name */
    SHR_IF_ERR_EXIT(bcmlt_db_table_info_get(unit,
                                            table_name,
                                            &table_attr,
                                            &fld_array_hdl,
                                            &hdl));

    entry = bcmtrm_entry_alloc(unit,
                               table_attr->table_id,
                               table_attr->interactive,
                               table_attr->pt,
                               fld_array_hdl,
                               table_attr->name);
    SHR_NULL_CHECK(entry, SHR_E_RESOURCE);
    entry->db_hdl = hdl;
    entry->entry_free_sync = hp_api_sync;
    /* Clean the fields array */
    if (fld_array_hdl) {
        sal_memset(entry->fld_arr, 0, sizeof(void *) * (table_attr->max_fid+1));
        entry->max_fid = table_attr->max_fid;
    } else {
        entry->max_fid = 0;
    }

    SHR_IF_ERR_EXIT(bcmlt_hdl_alloc(entry, entry_hdl));

    entry->info.entry_hdl = *entry_hdl;
exit:
    if (free_mutex) {
        sal_mutex_give(hp_api_sync);
    }
    SHR_FUNC_EXIT();
}

int bcmlt_entry_allocate_by_id(int unit,
                               uint32_t tid,
                               uint32_t field_cnt,
                               bcmlt_entry_handle_t *entry_hdl)
{
    bcmtrm_entry_t *entry = NULL;
    shr_lmm_hdl_t fld_array_hdl = NULL;
    bcmlt_table_attrib_t *table_attr;
    void *hdl;

    SHR_FUNC_ENTER(unit);
    if (!bcmlt_is_initialized()) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "BCMLT was not initialized yet\n")));
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    UNIT_VALIDATION(unit);
    if (!entry_hdl) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Table name and entry handle shouln't"\
                                " be NULL\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    sal_mutex_take(hp_api_sync, SAL_MUTEX_FOREVER);
    /* Find the table ID associated with the name */
    SHR_IF_ERR_EXIT(bcmlt_db_id_table_info_get(unit,
                                               tid,
                                               &table_attr,
                                               &fld_array_hdl,
                                               &hdl));

    entry = bcmtrm_entry_alloc(unit,
                               tid,
                               table_attr->interactive,
                               table_attr->pt,
                               fld_array_hdl,
                               table_attr->name);
    SHR_NULL_CHECK(entry, SHR_E_RESOURCE);
    entry->db_hdl = hdl;
    entry->entry_free_sync = hp_api_sync;
    /* Clean the fields array */
    if (fld_array_hdl) {
        sal_memset(entry->fld_arr, 0, sizeof(void *) * (table_attr->max_fid+1));
        entry->max_fid = table_attr->max_fid;
    } else {
        entry->max_fid = 0;
    }

    /* Prepare the fields free list */
    if (field_cnt > 0) {
        uint32_t max_fld_cnt = bcmlt_db_lt_max_fields(unit);
        /*
         * Restrict the number of allocated fields with the maximal any LT
         * might need.
         */
        if (field_cnt > max_fld_cnt) {
            field_cnt = max_fld_cnt;
        }
        entry->free_fld = shr_lmm_alloc_bulk(fields_hdl, field_cnt);
        SHR_NULL_CHECK(entry->free_fld, SHR_E_MEMORY);
    }
    entry->fld_mem_hdl = fields_hdl;

    SHR_IF_ERR_EXIT(bcmlt_hdl_alloc(entry, entry_hdl));
    entry->info.entry_hdl = *entry_hdl;

exit:
    if (SHR_FUNC_ERR()) {
        if (entry) {
            if (entry->free_fld) {
                shr_lmm_free_bulk(fields_hdl, entry->free_fld);
            }
            bcmtrm_entry_free(entry);
        }
    }
    if (fld_array_hdl) {
        sal_mutex_give(hp_api_sync);
    }
    SHR_FUNC_EXIT();
}

int bcmlt_entry_clear(bcmlt_entry_handle_t entry_hdl)
{
    shr_fmm_t *field;
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);

    /* Sanity check. Entry state must be E_ACTIVE */
    ENTRY_VALIDATE(entry);

    /* Free all the fields or place them in the field free list */
    while (entry->l_field) {
        field = entry->l_field;
        entry->l_field = field->next;
        if (entry->fld_arr) {
            entry->fld_arr[field->id] = NULL;
        }
        if (entry->fld_mem_hdl) {
            field->next = entry->free_fld;
            entry->free_fld = field;
        } else {
            shr_fmm_free(field);
        }
    }

    entry->attrib = 0;   /* Clear attributes */
    entry->usr_data = NULL;  /* Clear user data */
    entry->asynch = false;
    entry->delete = false;
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_free(bcmlt_entry_handle_t entry_hdl)
{
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);
    bool mutex_taken = false;

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);

    /*
     * Sanity check, don't allow deletion of entry in the following
     * cases:
     * 1. entry in idle state
     * 2. entry is part of a transaction
     */
    if (!entry || (entry->state > E_COMMITTED) ||
        entry->state < E_IDLE) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META("Invalid entry handle\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (entry->state == E_IDLE) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META("Stale entry handle\n")));
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    if (entry->p_trans) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                                "Entry is part of a transaction\n")));
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    sal_mutex_take(hp_api_sync, SAL_MUTEX_FOREVER);
    mutex_taken = true;
    SHR_IF_ERR_EXIT(bcmtrm_entry_free(entry));

    SHR_IF_ERR_EXIT(bcmlt_hdl_free(entry_hdl));
exit:
    if (mutex_taken) {
        sal_mutex_give(hp_api_sync);
    }
    SHR_FUNC_EXIT();
}

int bcmlt_entry_commit(bcmlt_entry_handle_t entry_hdl,
                       bcmlt_opcode_t opcode,
                       bcmlt_priority_level_t priority)
{
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    /* Input parameters checking */
    ENTRY_VALIDATE(entry);

    if (entry->pt) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                                "Use the PT functions for PT entry\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (entry->p_trans) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                                "Entry is part of a transaction\n")));
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    LT_OPCODE_VALIDATE(opcode, entry->info.unit);
    PRIORITY_VALIDATE(priority, entry->info.unit);

    entry->opcode.lt_opcode = opcode;
    entry->priority = BCMLT_PRIORITY_NORMAL; /* No priorities for sync oper */

    /* Clear get from hw attribute if opcode is not lookup */
    if (opcode == BCMLT_OPCODE_LOOKUP) {
        /* Clean out all fields other than key fields. */
        SHR_IF_ERR_EXIT(bcmlt_entry_clean_data_fields(entry));
    } else if (opcode == BCMLT_OPCODE_TRAVERSE) {
        /* Clean out all fields other than key fields. */
        if (entry->l_field) {
            SHR_IF_ERR_EXIT(bcmlt_entry_clean_data_fields(entry));
            if (!entry->l_field) {   /* If no key fields don't search again */
                entry->info.status = SHR_E_NOT_FOUND;
                SHR_ERR_EXIT(SHR_E_NOT_FOUND);
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

    entry->usr_data = NULL;
    entry->info.notif_opt = BCMLT_NOTIF_OPTION_NO_NOTIF;

    if (bsl_fast_check(BSL_LS_BCMLT_DETAILS | BSL_VERBOSE)) {
        bcmlt_dump_entry(entry, NULL);
    }
    SHR_IF_ERR_EXIT(bcmtrm_entry_req(entry));
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(entry->info.unit, "exit status=%d\n"),
                 entry->info.status));
    SHR_IF_ERR_EXIT(entry->info.status);
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_commit_async(bcmlt_entry_handle_t entry_hdl,
                             bcmlt_opcode_t opcode,
                             void *user_data,
                             bcmlt_notif_option_t notif_opt,
                             bcmlt_entry_cb notif_fn,
                             bcmlt_priority_level_t priority)
{
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    /* Input parameters checking */
    ENTRY_VALIDATE(entry);
    if (entry->pt) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                                "Use the PT functions for PT entry\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (entry->p_trans) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                                "Entry is part of a transaction\n")));
        SHR_ERR_EXIT(SHR_E_BUSY);
    }
    if (entry->fld_mem_hdl) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                                "Entry type supports only sync commit\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    LT_OPCODE_VALIDATE(opcode, entry->info.unit);
    PRIORITY_VALIDATE(priority, entry->info.unit);
    NOTIFICATION_VALIDATE(notif_opt, notif_fn, entry->info.unit);

    entry->opcode.lt_opcode = opcode;
    entry->priority = priority;

    /* Clear get from hw attribute if opcode is not lookup */
    if (opcode == BCMLT_OPCODE_LOOKUP) {
        /* Clean out all fields other than key fields. */
        SHR_IF_ERR_EXIT(bcmlt_entry_clean_data_fields(entry));
    } else if (opcode == BCMLT_OPCODE_TRAVERSE) {
        /* Clean out all fields other than key fields. */
        if (entry->l_field) {
            SHR_IF_ERR_EXIT(bcmlt_entry_clean_data_fields(entry));
            if (!entry->l_field) {   /* If no key fields don't search again */
                SHR_ERR_EXIT(SHR_E_NOT_FOUND);
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

    entry->cb.entry_cb = notif_fn;
    entry->info.notif_opt = notif_opt;
    entry->usr_data = user_data;
    if (bsl_fast_check(BSL_LS_BCMLT_DETAILS | BSL_VERBOSE)) {
        bcmlt_dump_entry(entry, NULL);
    }
    SHR_IF_ERR_EXIT(bcmtrm_asynch_entry_req(entry));
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_pt_entry_commit(bcmlt_entry_handle_t entry_hdl,
                          bcmlt_pt_opcode_t opcode,
                          bcmlt_priority_level_t priority)
{
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    /* Input parameters checking */
    ENTRY_VALIDATE(entry);
    if (!entry->pt) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                                "Use the PT functions only for PT entry\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (entry->p_trans) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                                "Entry is part of a transaction\n")));
        SHR_ERR_EXIT(SHR_E_BUSY);
    }
    PT_OPCODE_VALIDATE(opcode, entry->info.unit);
    PRIORITY_VALIDATE(priority, entry->info.unit);

    entry->opcode.pt_opcode = opcode;
    entry->priority = BCMLT_PRIORITY_NORMAL; /* No priorities for sync oper */

    /* Clear get from hw attribute if opcode is not get/lookup */
    if ((opcode != BCMLT_PT_OPCODE_GET) &&
        (opcode != BCMLT_PT_OPCODE_LOOKUP)) {
        entry->attrib &= ~BCMLT_ENT_ATTR_GET_FROM_HW;
    } else {
        /* Make sure not to pass this flag for PT operations */
        entry->attrib &= ~BCMLT_ENT_ATTR_FILTER_DEFAULTS;
    }

    entry->state = E_COMMITTING;
    entry->usr_data = NULL;
    SHR_IF_ERR_EXIT(bcmtrm_entry_req(entry));
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(entry->info.unit, "exit status=%d\n"),
                 entry->info.status));
    SHR_IF_ERR_EXIT(entry->info.status);
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_pt_entry_commit_async(bcmlt_entry_handle_t entry_hdl,
                                bcmlt_pt_opcode_t opcode,
                                void *user_data,
                                bcmlt_notif_option_t notif_opt,
                                bcmlt_entry_pt_cb notif_fn,
                                bcmlt_priority_level_t priority)
{
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_VALIDATE(entry);
    if (!entry->pt) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                                "Use the PT functions only for PT entry\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (entry->p_trans) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                                "Entry is part of a transaction\n")));
        SHR_ERR_EXIT(SHR_E_BUSY);
    }
    if (entry->fld_mem_hdl) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                                "Entry type supports only sync commit\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    PT_OPCODE_VALIDATE(opcode, entry->info.unit);
    PRIORITY_VALIDATE(priority, entry->info.unit);
    PT_NOTIFICATION_VALIDATE(notif_opt, notif_fn, entry->info.unit);

    entry->opcode.pt_opcode = opcode;
    entry->priority = priority;

    /* Clear get from hw attribute if opcode is not get/lookup */
    if ((opcode != BCMLT_PT_OPCODE_GET) &&
        (opcode != BCMLT_PT_OPCODE_LOOKUP)) {
        entry->attrib &= ~BCMLT_ENT_ATTR_GET_FROM_HW;
    } else {
        /* Make sure not to pass this flag for PT operations */
        entry->attrib &= ~BCMLT_ENT_ATTR_FILTER_DEFAULTS;
    }

    entry->state = E_COMMITTING;
    entry->cb.entry_pt_cb = notif_fn;
    entry->info.notif_opt = notif_opt;
    entry->usr_data = user_data;
    SHR_IF_ERR_EXIT(bcmtrm_asynch_entry_req(entry));
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_info_get(bcmlt_entry_handle_t entry_hdl,
                         bcmlt_entry_info_t  *entry_info)
{
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    if (!entry || !entry_info || (entry->state == E_IDLE)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    *entry_info = entry->info;
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_attrib_set(bcmlt_entry_handle_t entry_hdl,
                           uint32_t attrib)
{
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_VALIDATE(entry);

    entry->attrib |= attrib;

exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_attrib_clear(bcmlt_entry_handle_t entry_hdl,
                             uint32_t attrib)
{
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_VALIDATE(entry);

    entry->attrib &= ~attrib;

exit:
    SHR_FUNC_EXIT();
}


int bcmlt_entry_attrib_get(bcmlt_entry_handle_t entry_hdl,
                           uint32_t *attrib)
{
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_GET_VALIDATE(entry);
    if (!attrib) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    *attrib = entry->attrib;

exit:
    SHR_FUNC_EXIT();
}


int bcmlt_entry_oper_get(bcmlt_entry_handle_t entry_hdl,
                         bcmlt_unified_opcode_t *uni_opcode)
{
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_GET_VALIDATE(entry);
    if (!uni_opcode) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    uni_opcode->pt = entry->pt;
    if (entry->pt) {
        uni_opcode->opcode.pt = entry->opcode.pt_opcode;
    } else {
        uni_opcode->opcode.lt = entry->opcode.lt_opcode;
    }

exit:
    SHR_FUNC_EXIT();
}
