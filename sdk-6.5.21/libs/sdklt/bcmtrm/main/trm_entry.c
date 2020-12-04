/*! \file trm_entry.c
 *
 * This module implements the entry handling backend
 * of the transaction manager (TRM)
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_msgq.h>
#include <sal/sal_mutex.h>
#include <sal/sal_sem.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_lmem_mgr.h>
#include <bsl/bsl.h>
#include <bcmltm/bcmltm.h>
#include <bcmtrm/trm_api.h>
#include "trm_internal.h"

#define BSL_LOG_MODULE BSL_LS_BCMTRM_ENTRY

/*
 * Validate that the output arrays are sorted linked list based on their field
 * index.
 */
static int validate_input_field_order(shr_fmm_t *out_field)
{
    shr_fmm_t *field = out_field;
    uint32_t idx = 0;    /* Keeps the index of previous field */
    uint32_t fid = 0;    /* Keeps the ID of previous field */

    while (field) {
        if (field->id == fid) { /* Failure can only occur on match field IDs */
            if (field->idx < idx) {  /* Field ID lower then previous field? */
                return SHR_E_FAIL;
            }
        }
        idx = field->idx;
        fid = field->id;
        field = field->next;
    }
    return SHR_E_NONE;
}

/*
 * This function merges the output fields into the entry for entries
 * associated with LT only. These entries as field array associated
 * with the entry.
 * The function goes through the entire set of input fields and tries
 * to determine: if the field already present (i.e. matching fid and index).
 *
 * The above operation is simple for non array, simply check the field at
 * the field array using the FID as index into the array. If the entry is NULL
 * extract the field from the output list (out_field) onto the entry. The
 * field is inserted into the begining of the entry's field list
 * (entry->l_field), as well as into the field array using the FID for the
 * field location.
 *
 * Arrays are a bit more tricky as they should be sorted based on their idx.
 * The sorting means that the fields will be ordered in the field linked list.
 *
 * For array the process starts by identifying that this is an array. The
 * field is part of an array if its idx > 0. Once we determine that the
 * specific FID is of an array, the program starts to try and insert the field
 * at the right location. If a field with matching idx is already there just
 * update the data portion.
 * The field in_field points into various locations of the array. Initially,
 * it will point to the field array element (using FID as the array index).
 * At this point it is clear that the in_field is not NULL.
 * When the function detects that this is an array it doesn't move in_field
 * to the begining of the array, unless the field out_field (the one we are
 * trying to insert) has index smaller then the in_field index. When this
 * happens, the field is being inserted prior to the in_field and the in_field
 * being reset to point to the first element of the array. Otherwise, in_field
 * is only increment up the array. The reason for this is only for
 * optimization as otherwise, every new field will require a complete search
 * of its location.
 * For this scheme to work it is required that all the field arrays in the input
 * list (ltm_entry->out_fields) are sorted.
 */
static int proc_lt_entry_results(bcmtrm_entry_t *entry,
                                 bcmltm_entry_t *ltm_entry)
{
    shr_fmm_t *out_field = (shr_fmm_t *)ltm_entry->out_fields;
    shr_fmm_t *in_field = NULL;
    shr_fmm_t *prev_out_field = NULL;
    shr_fmm_t *prev_in_field;
    bool is_array = false;
    shr_fmm_t *field;

    while (out_field) {
        uint32_t idx = out_field->idx;
        uint32_t id = out_field->id;

        /* Sanity */
        if (id > entry->max_fid) {
            LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(entry->info.unit, "Invalid field ID=%u max %u\n"),
                              id, entry->max_fid));

            return SHR_E_INTERNAL;
        }
        /* Check if field already there, just update the data */
        if (entry->fld_arr[id]) {
            if (!is_array || idx == 0) {
                in_field = entry->fld_arr[id];
                is_array = false;
            }
            /*
             * in_field can't be NULL since if the field is not part of an
             * array then the block above assigns non NULL value to in_field.
             * Otherwise, the in_field simply runs through the array. When
             * the output field's (out_field) index is lower than the in_field
             * then the out_field will be placed (in the linked list) prior to
             * in_field and the in_field will be reset to point at the first
             * element of the array.
             */
            if (in_field->idx == idx) {
                in_field->data = out_field->data;
                prev_out_field = out_field;
                out_field = out_field->next;
                shr_fmm_free(prev_out_field);
            } else { /* Array! Insert the element at the right place */
                is_array = true;
                if (in_field->idx < idx) {
                    /*
                     * The list of fields pointed by in_field is sorted based
                     * on the field index. The following section find the last
                     * field that its index is smaller then the index of the
                     * field we want to insert (out_field). There are two
                     * cases:
                     * 1. We find a place to insert the new field (out_field)
                     *    into the list (following in_field) if:
                     *    a. The element in_field is the last in the field
                     *       list or
                     *    b. The element in_field points to element not in the
                     *       array (i.e. different field ID) or
                     *    c. The element in_field points to element that has
                     *       higher index than the index of the new field.
                     *
                     * 2. We found an element with matching index (and ID) so
                     *    we simply updating the field data.
                     */
                    while (true) {
                        /* Case (1) above */
                        if (!in_field->next ||
                            in_field->next->id != id ||
                            in_field->next->idx > idx) {
                            prev_out_field = out_field;
                            out_field = out_field->next;
                            if (entry->fld_mem_hdl) {
                                field = entry->free_fld;
                                if (!field) {
                                    sal_mutex_take(entry->entry_free_sync, SAL_MUTEX_FOREVER);
                                    field = shr_lmm_alloc(entry->fld_mem_hdl);
                                    sal_mutex_give(entry->entry_free_sync);
                                    if (!field) {
                                        LOG_ERROR(BSL_LOG_MODULE,
                                           (BSL_META_U(entry->info.unit,
                                             "Insufficient fields for output\n")));
                                        return SHR_E_RESOURCE;
                                    }
                                } else {
                                    entry->free_fld = field->next;
                                }
                                sal_memcpy(field, prev_out_field,
                                           sizeof(*field));
                                shr_fmm_free(prev_out_field);
                            } else {
                                field = prev_out_field;
                            }
                            field->next = in_field->next;
                            in_field->next = field;
                            break;
                        }
                        /* Case (2) above */
                        if (in_field->next->idx == idx) {
                            /* Element exist */
                            in_field->next->data = out_field->data;
                            prev_out_field = out_field;
                            out_field = out_field->next;
                            shr_fmm_free(prev_out_field);
                            break;
                        }
                        in_field = in_field->next;
                    }
                } else { /* Output field is first in the array */
                    prev_out_field = out_field;
                    out_field = out_field->next;
                    if (entry->fld_mem_hdl) {
                        field = entry->free_fld;
                        if (!field) {
                            sal_mutex_take(entry->entry_free_sync, SAL_MUTEX_FOREVER);
                            field = shr_lmm_alloc(entry->fld_mem_hdl);
                            sal_mutex_give(entry->entry_free_sync);
                            if (!field) {
                                LOG_ERROR(BSL_LOG_MODULE,
                                       (BSL_META_U(entry->info.unit,
                                         "Insufficient fields for output\n")));
                                return SHR_E_RESOURCE;
                            }
                        } else {
                            entry->free_fld = field->next;
                        }
                        sal_memcpy(field, prev_out_field, sizeof(*field));
                        shr_fmm_free(prev_out_field);
                    } else {
                        field = prev_out_field;
                    }
                    entry->fld_arr[id] = field;
                    /*
                     * Need to find previous field to insert the new
                     * element to the linked list.
                     */
                    if (entry->l_field == in_field) {
                        entry->l_field = field;
                        entry->l_field->next = in_field;
                    } else {
                        prev_in_field = entry->l_field;
                        while (prev_in_field->next != in_field) {
                            prev_in_field = prev_in_field->next;
                        }
                        prev_in_field->next = field;
                        prev_in_field->next->next = in_field;
                    }
                    /*
                     * Update the input field to start at the beginning of the
                     * array
                     */
                    in_field = entry->fld_arr[id];
                }
            }  /* End of handling array */
        } else {  /* New element */
            prev_out_field = out_field;
            out_field = out_field->next;
            if (entry->fld_mem_hdl) {
                field = entry->free_fld;
                if (!field) {
                    sal_mutex_take(entry->entry_free_sync, SAL_MUTEX_FOREVER);
                    field = shr_lmm_alloc(entry->fld_mem_hdl);
                    sal_mutex_give(entry->entry_free_sync);
                    if (!field) {
                        LOG_ERROR(BSL_LOG_MODULE,
                           (BSL_META_U(entry->info.unit,
                             "Insufficient fields for output\n")));
                        return SHR_E_RESOURCE;
                    }
                } else {
                    entry->free_fld = field->next;
                }
                sal_memcpy(field, prev_out_field, sizeof(*field));
                shr_fmm_free(prev_out_field);
            } else {
                field = prev_out_field;
            }
            entry->fld_arr[id] = field;
            /* Add the field to the begining of the list */
            field->next = entry->l_field;
            entry->l_field = field;
            is_array = false;
        }
    }
    return SHR_E_NONE;
}

static void proc_pt_entry_results(bcmtrm_entry_t *entry,
                                  bcmltm_entry_t *ltm_entry)
{
    shr_fmm_t *out_field = (shr_fmm_t *)ltm_entry->out_fields;
    shr_fmm_t *in_field = NULL;
    shr_fmm_t *prev_out_field = NULL;
    shr_fmm_t *prev_in_field;

    while (out_field) {
        in_field = entry->l_field;
        prev_in_field = NULL;
        /* Try to find the output field also in the input list */
        while (in_field && (in_field->id != out_field->id)) {
            prev_in_field = in_field;
            in_field = in_field->next;
        }

        /* If we have exact match copy the data and free the output field */
        if (in_field) {
            shr_fmm_t *tmp_field;

            if (in_field->idx == out_field->idx) {
                /* Copy the output value */
                in_field->data = out_field->data;
            }
            /* Delete from the link list */
            if (prev_out_field) {
                prev_out_field->next = out_field->next;
            } else {
                ltm_entry->out_fields = out_field->next;
            }
            tmp_field = out_field;
            out_field = out_field->next;
            if (in_field->idx == tmp_field->idx) {
                /* Data was copied, free the field */
                shr_fmm_free(tmp_field);
            } else {
                /*
                 * Need to insert the field at the proper location in
                 * the array. We know that the id matches but not the idx.
                 */
                while (in_field && (in_field->id == tmp_field->id) &&
                       (in_field->idx < tmp_field->idx)) {
                    prev_in_field = in_field;
                    in_field = in_field->next;
                }
                /* First check if the index matched */
                if (in_field && (in_field->id == tmp_field->id) &&
                    (in_field->idx == tmp_field->idx)) {
                    /* If match, copy the data and free the field */
                    in_field->data = tmp_field->data;
                    shr_fmm_free(tmp_field);
                    continue;
                }
                if (prev_in_field == NULL) {   /* First entry */
                    tmp_field->next = entry->l_field;
                    entry->l_field = tmp_field;
                } else {
                    tmp_field->next = in_field;
                    prev_in_field->next = tmp_field;
                }
            }
        } else { /* No match of field ID */
            prev_out_field = out_field;
            out_field = out_field->next;
        }
    }
    if (ltm_entry->out_fields) {
        /*
         * Attach the input fields to the end of the output
         * list. Remember that prev_out_field is the last
         * field unless there was only one output field.
         */
        if (prev_out_field) {
            prev_out_field->next = entry->l_field;
        } else {   /* Only one field in the output list */
            ltm_entry->out_fields->next = entry->l_field;
        }
        /* Place the begining of the output field list into the list of fields*/
        entry->l_field = (shr_fmm_t *)ltm_entry->out_fields;
    }
}

const char* bcmtrm_ltopcode_to_str(bcmlt_opcode_t opcode)
{
    static const char *operation_str[] = {
        "",
        "insert",
        "lookup",
        "delete",
        "update",
        "traverse"
    };
    return operation_str[opcode];
}

void bcmtrm_proc_entry_results(bcmtrm_entry_t *entry,
                               bcmltm_entry_t *ltm_entry)
{
    int rv;

    if (ltm_entry && (ltm_entry->out_fields != NULL)) {
        if (validate_input_field_order(ltm_entry->out_fields) == SHR_E_NONE) {
            /* Link the output fields to the list of fields */
            if (entry->fld_arr) {
                rv = proc_lt_entry_results(entry, ltm_entry);
                if (entry->info.status == SHR_E_NONE) {
                    entry->info.status = rv;
                }
            } else {
                proc_pt_entry_results(entry, ltm_entry);
            }
        } else {
            entry->info.status = SHR_E_INTERNAL;
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(entry->info.unit,
                                  "Array fields not in ordered table=%s\n"),
                       entry->info.table_name));
        }
    }

    if (ltm_entry) {
        shr_lmm_free(bcmtrm_ltm_entry_hdl, (void *)ltm_entry);
    }
    /* Free the ltm entry */
    entry->ltm_entry = NULL;
}

/*
 * This function is called by the ptm for hardware operation completion.
 * This callback function is only for entry commit operations.
 * It should try to avoid blocking as much as possible so the only processing
 * is to post the event into the notify thread for further processing. Note
 * that posting the event might be blocking
*/
static void trm_entry_cb(uint32_t trans_id,
                         shr_error_t status,
                         void *user_data)
{
    bcmtrm_entry_t *entry = (bcmtrm_entry_t *)user_data;
    bcmtrm_hw_notif_struct_t hw_notif;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(entry->info.unit, "status=%d\n"), status));
    /*
     * the entry must be in committing state. Make sure the callback
     * is not happening for stale entries
     */
    if ((entry->state != E_COMMITTING) &&
         (entry->state != E_COMMITTED)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(entry->info.unit, "Entry in wrong state %d\n"),
                  entry->state));
        return;
    }

    bcmtrm_trans_ha_state.done_f(entry->info.unit,
                                 trans_id,
                                 entry->ha_trn_hdl);
    hw_notif.type = ENTRY_CB;
    hw_notif.ltm_entry = entry;
    hw_notif.status = status;
    /* Post the event to the notification thread */
    sal_msgq_post(bcmtrm_hw_notif_q, &hw_notif, SAL_MSGQ_NORMAL_PRIORITY,
                  SAL_MSGQ_FOREVER);
}

static void sync_entry_cb(uint32_t trans_id,
                     shr_error_t status,
                     void *user_data)
{
    bcmtrm_entry_t *entry = (bcmtrm_entry_t *)user_data;

    bcmtrm_trans_ha_state.done_f(entry->info.unit, trans_id, entry->ha_trn_hdl);

    entry->info.status = status;
    if (entry->cb.entry_cb) {
        entry->cb.entry_cb(BCMLT_NOTIF_OPTION_HW,
                           entry->opcode.lt_opcode,
                           &entry->info,
                           entry);
    }
}

/*
 * Record the entry information into interactive HA memory so it
 * can be replayed later. Note that the last element to set is the
 * number of fields.
 */
static void interactive_ha_record(bcmtrm_entry_t *entry,
                                  uint32_t op_id,
                                  trn_interact_info_t *ha_mem)
{
    shr_fmm_t *fld;
    uint32_t fld_cnt = 0;
    trn_interact_field_t *inter_fld;

    if (!ha_mem) { /* Nowhere to record */
        return;
    }
    /* For PT passthrough ignore get and lookup operations */
    if (entry->pt && (entry->opcode.pt_opcode == BCMLT_PT_OPCODE_GET ||
        entry->opcode.pt_opcode == BCMLT_PT_OPCODE_LOOKUP)) {
        return;
    }
    /* For PT operations ignore lookup and traverse operations */
    if (!entry->pt && (entry->opcode.lt_opcode == BCMLT_OPCODE_LOOKUP ||
                       entry->opcode.lt_opcode == BCMLT_OPCODE_TRAVERSE)) {
        return;
    }

    inter_fld = ha_mem->field_arry;
    fld = entry->l_field;

    ha_mem->field_count = 0;
    while (fld && fld_cnt < ha_mem->max_fields) {
        inter_fld->data = fld->data;
        inter_fld->fid = fld->id;
        inter_fld->idx = fld->idx;
        inter_fld++;
        fld = fld->next;
        fld_cnt++;
    }
    if (fld && fld_cnt == ha_mem->max_fields) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(entry->info.unit,
                              "Insufficient space for interactive HA\n")));
    } else {
        ha_mem->opcode = entry->pt ? entry->opcode.pt_opcode :
                                     entry->opcode.lt_opcode;
        ha_mem->pt = entry->pt;
        ha_mem->tbl_id = entry->table_id;
        ha_mem->trn_id = op_id;
        ha_mem->field_count = (uint16_t)(fld_cnt & 0xFFFF);
    }
}

void bcmtrm_appl_entry_inform(bcmtrm_entry_t *entry,
                              shr_error_t status,
                              bcmlt_notif_option_t notif_opt)
{
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(entry->info.unit, "notif_opt=%d status=%d\n"),
                 notif_opt, status));

    /*
     * Don't make a callback if the caller doesn't want it or if the entry
     * has been marked for deletion.
     */
    if (entry->info.notif_opt == BCMLT_NOTIF_OPTION_NO_NOTIF ||
        entry->delete) {
        return;
    }

    /* In case of a failure, make sure that the callback will occur. */
    if (status != SHR_E_NONE) {
        if ((entry->info.notif_opt != BCMLT_NOTIF_OPTION_ALL) &&
            (entry->info.notif_opt != notif_opt)) {
            notif_opt = entry->info.notif_opt;
        }
        /* No more notifications after this one. */
        entry->info.notif_opt = BCMLT_NOTIF_OPTION_NO_NOTIF;
    } else if ((entry->info.notif_opt != notif_opt) &&
               (entry->info.notif_opt != BCMLT_NOTIF_OPTION_ALL)) {
        return;
    }
    /*
     * This function should only be called for single entry and not an
     * entry that is part of a transaction.
     */
    assert(entry->p_trans == NULL);

    /*
     * Pass the point of the callback we can't access the entry as it is
     * possible that the OS switch thread during the callback and the
     * application uses this thread to reporpose the entry for another commit
     * or simply freed the entry.
     */
    if (entry->pt) {
        if (entry->cb.entry_pt_cb) {
            entry->cb.entry_pt_cb(notif_opt,
                                  entry->opcode.pt_opcode,
                                  &entry->info,
                                  entry->usr_data);
        }
    } else if (entry->cb.entry_cb) {
        entry->cb.entry_cb(notif_opt,
                           entry->opcode.lt_opcode,
                           &entry->info,
                           entry->usr_data);
    }
}

int bcmtrm_stage_entry (bcmtrm_entry_t *entry,
                        uint32_t op_id,
                        uint32_t trans_id)
{
    int rv;
    bcmltm_entry_t *ltm_entry;

    ltm_entry = shr_lmm_alloc(bcmtrm_ltm_entry_hdl);
    if (!ltm_entry) {
        return SHR_E_MEMORY;
    }
    ltm_entry->next = NULL;
    ltm_entry->entry_id = op_id;
    ltm_entry->trans_id = trans_id;
    ltm_entry->in_fields = (bcmltm_field_list_t *)entry->l_field;
    ltm_entry->attrib = entry->attrib;
    ltm_entry->opcode = entry->opcode;
    ltm_entry->table_id = entry->table_id;
    ltm_entry->out_fields = NULL;
    ltm_entry->unit = entry->info.unit;
    ltm_entry->field_alloc_cb = bcmtrm_local_field_alloc;
    entry->ltm_entry = ltm_entry;

    if (entry->pt) {
        switch (entry->opcode.pt_opcode) {
        case BCMLT_PT_OPCODE_FIFO_POP:
            rv = bcmltm_entry_pt_fifo_pop(ltm_entry);
            break;
        case BCMLT_PT_OPCODE_FIFO_PUSH:
            rv = bcmltm_entry_pt_fifo_push(ltm_entry);
            break;
        case BCMLT_PT_OPCODE_SET:
            rv = bcmltm_entry_pt_set(ltm_entry);
            break;
        case BCMLT_PT_OPCODE_MODIFY:
            rv = bcmltm_entry_pt_modify(ltm_entry);
            break;
        case BCMLT_PT_OPCODE_GET:
            rv = bcmltm_entry_pt_get(ltm_entry);
            break;
        case BCMLT_PT_OPCODE_CLEAR:
            rv = bcmltm_entry_pt_clear(ltm_entry);
            break;
        case BCMLT_PT_OPCODE_INSERT:
            rv = bcmltm_entry_pt_insert(ltm_entry);
            break;
        case BCMLT_PT_OPCODE_DELETE:
            rv = bcmltm_entry_pt_delete(ltm_entry);
            break;
        case BCMLT_PT_OPCODE_LOOKUP:
            rv = bcmltm_entry_pt_lookup(ltm_entry);
            break;
        default:
            return SHR_E_INTERNAL;
        }
    } else {
        switch (entry->opcode.lt_opcode) {
        case BCMLT_OPCODE_DELETE:
            rv = bcmltm_entry_delete(ltm_entry);
            break;
        case BCMLT_OPCODE_INSERT:
            rv = bcmltm_entry_insert(ltm_entry);
            break;
        case BCMLT_OPCODE_UPDATE:
            rv = bcmltm_entry_update(ltm_entry);
            break;
        case BCMLT_OPCODE_LOOKUP:
            rv = bcmltm_entry_lookup(ltm_entry);
            break;
        case BCMLT_OPCODE_TRAVERSE:
            if (!entry->l_field) {
                rv = bcmltm_entry_traverse_first(ltm_entry);
            } else {
                rv = bcmltm_entry_traverse_next(ltm_entry);
            }
            break;
        default:
            return SHR_E_INTERNAL;
        }
    }
    /* If failed, clear the ltm entry */
    if (rv != SHR_E_NONE) {
        entry->ltm_entry = NULL;
        if (!entry->pt) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(entry->info.unit,
                                "Table %s LT operation %s failed error = %s\n"),
                         entry->info.table_name,
                         bcmtrm_ltopcode_to_str(entry->opcode.lt_opcode),
                         shr_errmsg(rv)));
        }
        shr_lmm_free(bcmtrm_ltm_entry_hdl, (void *)ltm_entry);
    }
    return rv;
}

int bcmtrm_process_entry_req(bcmtrm_entry_t *entry,
                             uint32_t *op_id,
                             uint32_t *trans_id)
{
    int rv;
    bcmptm_trans_cb_f  cb;
    int unit = entry->info.unit;
    bcmtrm_hw_notif_struct_t hw_notif;
    uint32_t ha_trn_hdl;

    do {
        bcmtrm_trans_ha_state.set_f(unit, BCMTRM_STATE_STAGING,
                                    *trans_id, &ha_trn_hdl);
        rv = bcmtrm_stage_entry(entry, *op_id, *trans_id);

        if (rv != SHR_E_NONE) {
            LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                                "Stage failed rv=%d e_id=%d pt=%d opcode=%d\n"),
                      rv,*op_id, entry->pt, entry->opcode.pt_opcode));
            bcmltm_abort(unit, *trans_id);
            bcmtrm_trans_ha_state.cancel_f(unit, *trans_id, ha_trn_hdl);
            entry->info.status = rv;
            break;
        } else {
            entry->state = E_COMMITTED;
            if (entry->asynch) {
                bcmtrm_appl_entry_inform(entry,
                                         SHR_E_NONE,
                                         BCMLT_NOTIF_OPTION_COMMIT);
            }
        }

        if (entry->asynch) {
            cb = trm_entry_cb;
        } else {
            cb = sync_entry_cb;
        }

        entry->ha_trn_hdl = ha_trn_hdl;
        bcmtrm_trans_ha_state.update_f(unit,
                                       BCMTRM_STATE_COMMITTED,
                                       *trans_id,
                                       ha_trn_hdl);
        /*
         * Only error handling allowed past the call to commit as the function
         * may return before or after the callbck from the WAL.
         */
        if ((rv = bcmltm_commit(unit,
                                *trans_id, cb, entry)) != SHR_E_NONE) {

            bcmtrm_trans_ha_state.cancel_f(unit, *trans_id, ha_trn_hdl);
        }
    } while (0);

    if (rv != SHR_E_NONE && entry->asynch) {
        hw_notif.status = rv;
        hw_notif.ltm_entry = entry;
        if (entry->p_trans) {
            hw_notif.type = TRANS_CB;
        } else {
            hw_notif.type = ENTRY_CB;
        }
        /* Post the event to the notification thread */
        sal_msgq_post(bcmtrm_hw_notif_q, &hw_notif, SAL_MSGQ_NORMAL_PRIORITY,
                      SAL_MSGQ_FOREVER);
    }

    INCREMENT_OP_ID((*trans_id));

    return rv;
}

int bcmtrm_process_interactive_entry_req(bcmtrm_entry_t *entry,
                                         uint32_t op_id,
                                         trn_interact_info_t *ha_mem)
{
    int rv;
    bcmtrm_hw_notif_struct_t hw_notif;

    /* Keep the entry in HA memory for recovery */
    interactive_ha_record(entry, op_id, ha_mem);

    rv = bcmtrm_stage_entry(entry,
                           op_id,
                           0);

    if (ha_mem) {
        ha_mem->field_count = 0;
    }

    if (rv != SHR_E_NONE) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(entry->info.unit,
                             "Stage failed rv=%d e_id=%d pt_opcode=%d\n"),
                  rv, op_id, entry->opcode.pt_opcode));
    }
    /* Everything is done. Inform the notifier thread to do the rest */
    entry->state = E_COMMITTED;

    hw_notif.ltm_entry = entry;
    hw_notif.status = rv;
    if (entry->p_trans) {
        bcmtrm_trans_t *trans = entry->p_trans;
        hw_notif.type = TRANS_CB;
        sal_mutex_take(trans->lock_obj->mutex, SAL_MUTEX_FOREVER);
        trans->committed_entries++;
        if (rv == SHR_E_NONE) {
            trans->commit_success++;
        }
        sal_mutex_give(trans->lock_obj->mutex);
    } else {
        hw_notif.type = ENTRY_CB;
    }
    /* Post the event to the notification thread */
    sal_msgq_post(bcmtrm_hw_notif_q, &hw_notif, SAL_MSGQ_NORMAL_PRIORITY,
                  SAL_MSGQ_FOREVER);

    return rv;
}

void bcmtrm_entry_complete(bcmtrm_entry_t *entry)
{
    sal_mutex_t mtx;
    bool del_entry = false;

    if (entry->asynch) {
        mtx = bcmtrm_entry_mutex_get();
        if (mtx) {
            sal_mutex_take(mtx, SAL_MUTEX_FOREVER);
            entry->state = E_ACTIVE;   /* The entry is done */
            del_entry = entry->delete;
            sal_mutex_give(mtx);
        }
        if (del_entry) {
            sal_mutex_take(entry->entry_free_sync, SAL_MUTEX_FOREVER);
            bcmtrm_entry_done(entry);
            sal_mutex_give(entry->entry_free_sync);
        }
    }
}

void bcmtrm_recover_interact(int unit, trn_interact_info_t * ha_mem)
{
    bcmtrm_entry_t *entry;
    shr_fmm_t *fld = NULL;
    uint32_t fld_cnt;
    trn_interact_field_t *inter_fld = ha_mem->field_arry;

    if (ha_mem->field_count == 0) {
        return;
    }
    entry = bcmtrm_entry_alloc(unit, ha_mem->tbl_id, true, ha_mem->pt, NULL, "");
    if (!entry) {
        return;
    }
    if (ha_mem->pt) {
        entry->opcode.pt_opcode = (bcmlt_pt_opcode_t)ha_mem->opcode;
    } else {
        entry->opcode.lt_opcode = (bcmlt_opcode_t)ha_mem->opcode;
    }

    for (fld_cnt = 0; fld_cnt < ha_mem->field_count; fld_cnt++, inter_fld++) {
        if (fld) {
            fld->next = shr_fmm_alloc();
            if (!fld->next) {
                return;
            }
            fld = fld->next;
        } else {
            fld = shr_fmm_alloc();
            if (!fld) {
                return;
            }
            entry->l_field = fld;
        }
        fld->data = inter_fld->data;
        fld->id = inter_fld->fid;
        fld->idx = inter_fld->idx;
        fld->next = NULL;
    }

    /* Now execute the operation. */
    bcmtrm_process_interactive_entry_req(entry, ha_mem->trn_id, NULL);
    ha_mem->field_count = 0;

    /* Free the resources */
    bcmtrm_entry_done(entry);
}
