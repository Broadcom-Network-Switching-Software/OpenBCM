/*! \file bcmltm_ee_model_index.c
 *
 * Logical Table Manager Execution Engine
 *
 * Logical table simple index basic implementation
 *
 * This file contains the LTM EE stage functions for LT operations
 * on simple index tables.
 *
 * ALLOC  - not supported for simple index LTs
 * INSERT
 * LOOKUP
 * DELETE
 * UPDATE
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmptm/bcmptm.h>
#include <bcmltm/bcmltm_table_op_pt_info.h>
#include <bcmltm/bcmltm_table_op_dop_info.h>
#include <bcmltm/bcmltm_tree.h>
#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_wb_ptm_internal.h>
#include <bcmltm/bcmltm_ee_tree_nodes_internal.h>
#include <bcmltm/bcmltm_stats_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_EXECUTIONENGINE


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Common logic for LTM-PTM interface of modeled index PT table ops.
 *
 * This function implements the common logic for the LTM-PTM interface
 * to dispatch WRITE or READ ops for modeled index physical tables.
 * The PTM interface for each op is common, so the preparatory logic
 * is very similar.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 * \param [in] pt_op The PTM op type to be executed.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmltm_ee_node_model_index_opcode(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie,
                                  bcmptm_op_type_t pt_op)
{
    uint32_t ptix, opix;
    uint32_t pt_flags;
    uint32_t pt_count, pt_sbus_count;
    uint8_t word_size;
    uint32_t *cur_entry;
    bcmltm_pt_list_t *pt_list;
    bcmltm_pt_mem_args_t *cur_mem_args;
    bcmltm_pt_op_list_t *pt_op_list;
    bcmltm_wb_ptm_param_offsets_t param_offsets;
    bcmltm_wb_ptm_entry_index_offsets_t entry_offsets;
    bcmbd_pt_dyn_info_t cur_pt_dyn_info;
    uint32_t rsp_ltid;
    uint32_t rsp_flags;
    uint64_t req_flags;
    uint32_t ltid, ptid;
    uint32_t pt_suppress_offset;

    SHR_FUNC_ENTER(unit);

    ltid = lt_md->params->ltid;

    pt_list = BCMLTM_PT_LIST(node_cookie);
    /*
     * Check if an LTA Transform has declared that PT
     * operations should be omitted during this LT op.
     */
    pt_suppress_offset = pt_list->pt_suppress_offset;
    if ((pt_suppress_offset != BCMLTM_WB_OFFSET_INVALID) &&
        (ltm_buffer[pt_suppress_offset] != 0)) {
        SHR_EXIT();
    }

    pt_count = BCMLTM_PT_LIST_NUM(node_cookie);

    for (ptix = 0; ptix < pt_count; ptix++) {
        cur_mem_args = BCMLTM_PT_LIST_MEM_ARGS(node_cookie, ptix);
        pt_op_list = BCMLTM_PT_LIST_OPS_LIST(node_cookie, ptix);
        pt_sbus_count = pt_op_list->num_pt_ops;
        word_size = pt_op_list->word_size;
        pt_flags = pt_op_list->flags;

        /*
         * Clear the PT changed flag if PTM operation is not a write.
         *
         * The PT changed logic only applies to a PTM write operation.
         * Currently the PT op list node cookie is shared among
         * several opcodes, so it is necessary to clear this flag
         * when the operation is not a PTM write.
         * If in the future the PT op list node cookie is created
         * per opcode, this explicit clearing of this flag can be removed.
         */
        if (pt_op != BCMPTM_OP_WRITE) {
            pt_flags &= ~BCMLTM_PT_OP_FLAGS_PT_CHANGED;
        }

        /* Prepare PTM request flags */
        req_flags = cur_mem_args->flags;
        req_flags |= bcmptm_lt_entry_attrib_to_ptm_req_flags(lt_entry->attrib);

        for (opix = 0; opix < pt_sbus_count; opix++) {
            /* Get PTM parameter offsets */
            (void)bcmltm_wb_ptm_param_offsets_get(
                pt_op_list->param_offsets[opix],
                &param_offsets);

            /* Check PT entry changed flag */
            if (pt_flags & BCMLTM_PT_OP_FLAGS_PT_CHANGED) {
                if (ltm_buffer[param_offsets.pt_changed] !=
                    BCMLTM_PT_ENTRY_CHANGED_VALUE) {
                    continue;
                }
            }

            /* Fresh copy for any adjustments with PT_OP_FLAGS */
            sal_memcpy(&cur_pt_dyn_info, cur_mem_args->pt_dyn_info,
                       sizeof(bcmbd_pt_dyn_info_t));

            bcmltm_pt_dyn_info_get(pt_flags, &param_offsets,
                                   ltm_buffer, &cur_pt_dyn_info);

            if (pt_flags & BCMLTM_PT_OP_FLAGS_ACCTYPE) {
                /*
                 * Implementation pending
                 */
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
            }

            /* Check if a LTA Transform has modified the SID sent
             * to the PTM. Use that instead of PTID if so. */
            if (pt_flags & BCMLTM_PT_OP_FLAGS_PT_SID) {
                ptid = ltm_buffer[param_offsets.pt_sid_override];
            } else {
                ptid = cur_mem_args->pt;
            }

            /* Get PTM entry offsets */
            (void)bcmltm_wb_ptm_entry_index_offsets_get(
                pt_op_list->buffer_offsets[opix],
                &entry_offsets);

            cur_entry = ltm_buffer + entry_offsets.data;

            /*
             * Perform the PTM operation.
             */
            bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                                   BCMLRD_FIELD_PHYSICAL_TABLE_OP_COUNT);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_mreq_indexed_lt(unit,
                                        req_flags,
                                        ptid,
                                        &cur_pt_dyn_info,
                                        cur_mem_args->pt_static_info,
                                        NULL, /* misc_info */
                                        pt_op,
                                        cur_entry,
                                        word_size,
                                        ltid,
                                        lt_entry->trans_id, /* cseq_id */
                                        /* Notify only used for commit/abort */
                                        NULL, /* notify ptr */
                                        NULL, /* notify info (cookie) */
                                        cur_entry,
                                        &rsp_ltid,
                                        &rsp_flags));

            /* Process the response values - LTID and flags */
            if (lt_state->table_op_pt_info_enable) {
                /*
                 * PT op info enable is checked inside this function
                 * as well due to use by the PTM callbacks for CTH.
                 * But the common case is disabled, so LTM internals will
                 * be more efficient to skip this function entirely.
                 */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmltm_pt_status_mreq_indexed_lt(unit, ltid,
                                                      lt_entry->trans_id, ptid,
                                                      &cur_pt_dyn_info));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmltm_dop_status_mreq_indexed_lt(unit, ltid, ptid,
                                                       &cur_pt_dyn_info));
            }
        }
    }

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                               BCMLRD_FIELD_PHYSICAL_TABLE_OP_ERROR_COUNT);
    }
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

/*
 * This function examines the index of a simple index LT table for
 * existence, and permits an LT entry insertion if it does not.
 */
int
bcmltm_ee_node_model_index_insert(int unit,
                                  bcmltm_lt_md_t *ltm_md,
                                  bcmltm_lt_state_t *ltm_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie)
{
    uint32_t cur_index;
    bcmltm_ee_index_info_t *idx_info;
    SHR_BITDCL *inuse_bitmap;

    SHR_FUNC_ENTER(unit);

    idx_info = BCMLTM_EE_INDEX_INFO(node_cookie);

    if (idx_info == NULL) {
        /* "No-key" LTs do not require an API field to determine the index.
         * They may only have one entry. */
        cur_index = 0;
    } else {
        cur_index = ltm_buffer[idx_info->index_offset];
    }

    if (cur_index >= ltm_state->bitmap_size) {
        /* Track index beyond the bounds of the LT. */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_EXIT
        (bcmltm_state_data_get(unit, ltm_state,
                               BCMLTM_STATE_TYPE_INUSE_BITMAP,
                               (void *) &inuse_bitmap));

    if (SHR_BITGET(inuse_bitmap, cur_index)) {
        /* Entry already exists.  Insert not permitted */
        SHR_ERR_EXIT(SHR_E_EXISTS);
    } else {
        /* On INSERT, check for max entries. */
        if (ltm_state->entry_count >= ltm_state->max_entry_count) {
            SHR_ERR_EXIT(SHR_E_FULL);
        }
    }

    /* Else, mark entry as now existing.
     * Note that this is the working state which is discarded
     * on a subsequent failure any time during the transaction.
     */
    SHR_BITSET(inuse_bitmap, cur_index);

 exit:
    SHR_FUNC_EXIT();
}

/*
 * This function examines the index of a simple index LT table for
 * existence, and removes the in-use marker if so.
 */
int
bcmltm_ee_node_model_index_delete(int unit,
                                  bcmltm_lt_md_t *ltm_md,
                                  bcmltm_lt_state_t *ltm_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie)
{
    uint32_t cur_index;
    bcmltm_ee_index_info_t *idx_info;
    SHR_BITDCL *inuse_bitmap;

    SHR_FUNC_ENTER(unit);

    idx_info = BCMLTM_EE_INDEX_INFO(node_cookie);

    if (idx_info == NULL) {
        /* "No-key" LTs do not require an API field to determine the index.
         * They may only have one entry. */
        cur_index = 0;
    } else {
        cur_index = ltm_buffer[idx_info->index_offset];
    }

    if (cur_index >= ltm_state->bitmap_size) {
        /* Track index beyond the bounds of the LT. */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_EXIT
        (bcmltm_state_data_get(unit, ltm_state,
                               BCMLTM_STATE_TYPE_INUSE_BITMAP,
                               (void *) &inuse_bitmap));

    if (!SHR_BITGET(inuse_bitmap, cur_index)) {
        /* Entry does not exist.  Delete not permitted */
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Else, mark entry as now free.
     * Note that this is the working state which is discarded
     * on a subsequent failure any time during the transaction.
     */
    SHR_BITCLR(inuse_bitmap, cur_index);

 exit:
    SHR_FUNC_EXIT();
}

/*
 * This function tests the index of a simple index LT table for
 * existence.
 */
int
bcmltm_ee_node_model_index_exists(int unit,
                                  bcmltm_lt_md_t *ltm_md,
                                  bcmltm_lt_state_t *ltm_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie)
{
    uint32_t cur_index;
    bcmltm_ee_index_info_t *idx_info;
    SHR_BITDCL *inuse_bitmap;

    SHR_FUNC_ENTER(unit);

    idx_info = BCMLTM_EE_INDEX_INFO(node_cookie);

    if (idx_info == NULL) {
        /* "No-key" LTs do not require an API field to determine the index.
         * They may only have one entry. */
        cur_index = 0;
    } else {
        cur_index = ltm_buffer[idx_info->index_offset];
    }

    if (cur_index >= ltm_state->bitmap_size) {
        /* Track index beyond the bounds of the LT. */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_EXIT
        (bcmltm_state_data_get(unit, ltm_state,
                               BCMLTM_STATE_TYPE_INUSE_BITMAP,
                               (void *) &inuse_bitmap));

    if (!SHR_BITGET(inuse_bitmap, cur_index)) {
        /* Entry does not exist. */
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

 exit:
    SHR_FUNC_EXIT();
}

/*
 * This function tests the index of a simple index LT table for
 * availability.
 */
int
bcmltm_ee_node_model_index_free(int unit,
                                bcmltm_lt_md_t *ltm_md,
                                bcmltm_lt_state_t *ltm_state,
                                bcmltm_entry_t *lt_entry,
                                uint32_t *ltm_buffer,
                                void *node_cookie)
{
    uint32_t cur_index;
    bcmltm_ee_index_info_t *idx_info;
    SHR_BITDCL *inuse_bitmap;

    SHR_FUNC_ENTER(unit);

    idx_info = BCMLTM_EE_INDEX_INFO(node_cookie);

    if (idx_info == NULL) {
        /* "No-key" LTs do not require an API field to determine the index.
         * They may only have one entry. */
        cur_index = 0;
    } else {
        cur_index = ltm_buffer[idx_info->index_offset];
    }

    if (cur_index >= ltm_state->bitmap_size) {
        /* Track index beyond the bounds of the LT. */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_EXIT
        (bcmltm_state_data_get(unit, ltm_state,
                               BCMLTM_STATE_TYPE_INUSE_BITMAP,
                               (void *) &inuse_bitmap));

    if (SHR_BITGET(inuse_bitmap, cur_index)) {
        /* Entry does not exist. */
        SHR_ERR_EXIT(SHR_E_EXISTS);
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_ee_node_model_index_write(int unit,
                                 bcmltm_lt_md_t *lt_md,
                                 bcmltm_lt_state_t *lt_state,
                                 bcmltm_entry_t *lt_entry,
                                 uint32_t *ltm_buffer,
                                 void *node_cookie)
{
    return bcmltm_ee_node_model_index_opcode(unit, lt_md,
                                             lt_state, lt_entry,
                                             ltm_buffer, node_cookie,
                                             BCMPTM_OP_WRITE);
}

int
bcmltm_ee_node_model_index_read(int unit,
                                bcmltm_lt_md_t *lt_md,
                                bcmltm_lt_state_t *lt_state,
                                bcmltm_entry_t *lt_entry,
                                uint32_t *ltm_buffer,
                                void *node_cookie)
{
    return bcmltm_ee_node_model_index_opcode(unit, lt_md,
                                             lt_state, lt_entry,
                                             ltm_buffer, node_cookie,
                                             BCMPTM_OP_READ);
}

/*
 * This function examines the index of a shared resource simple index
 * LT table for existence, and permits an LT entry insertion
 * if it does not.
 */
int
bcmltm_ee_node_global_index_insert(int unit,
                                  bcmltm_lt_md_t *ltm_md,
                                  bcmltm_lt_state_t *ltm_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie)
{
    uint32_t cur_index;
    bcmltm_ee_index_info_t *idx_info;
    SHR_BITDCL *global_bitmap;
    uint32_t global_ltid;
    bcmltm_lt_state_t *global_lt_state;

    SHR_FUNC_ENTER(unit);

    idx_info = BCMLTM_EE_INDEX_INFO(node_cookie);

    if (idx_info == NULL) {
        /* "No-key" LTs do not require an API field to determine the index.
         * They may only have one entry. */
        cur_index = 0;
    } else {
        cur_index = ltm_buffer[idx_info->index_offset];
    }

    global_ltid = ltm_md->params->global_inuse_ltid;

    /* Global LT state */
    SHR_IF_ERR_EXIT
        (bcmltm_state_lt_get(unit, global_ltid, &global_lt_state));

    if (cur_index >= global_lt_state->bitmap_size) {
        /* Track index beyond the bounds of the global LT. */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_EXIT
        (bcmltm_state_data_get(unit, global_lt_state,
                               BCMLTM_STATE_TYPE_GLOBAL_BITMAP,
                               (void *) &global_bitmap));

    if (SHR_BITGET(global_bitmap, cur_index)) {
        /* Entry already used by shared LT.  Insert not permitted */
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    } else {
        /* On INSERT, check for max entries. */
        if (ltm_state->entry_count >= ltm_state->max_entry_count) {
            SHR_ERR_EXIT(SHR_E_FULL);
        }
    }

    /* Else, mark entry as now existing.
     * Note that this is the working state which is discarded
     * on a subsequent failure any time during the transaction.
     */
    SHR_BITSET(global_bitmap, cur_index);

 exit:
    SHR_FUNC_EXIT();
}

/*
 * This function examines the index of a shared resource simple index
 * LT table for existence, and removes the in-use marker if so.
 */
int
bcmltm_ee_node_global_index_delete(int unit,
                                  bcmltm_lt_md_t *ltm_md,
                                  bcmltm_lt_state_t *ltm_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie)
{
    uint32_t cur_index;
    bcmltm_ee_index_info_t *idx_info;
    SHR_BITDCL *global_bitmap;
    uint32_t global_ltid;
    bcmltm_lt_state_t *global_lt_state;

    SHR_FUNC_ENTER(unit);

    idx_info = BCMLTM_EE_INDEX_INFO(node_cookie);

    if (idx_info == NULL) {
        /* "No-key" LTs do not require an API field to determine the index.
         * They may only have one entry. */
        cur_index = 0;
    } else {
        cur_index = ltm_buffer[idx_info->index_offset];
    }

    global_ltid = ltm_md->params->global_inuse_ltid;

    /* Global LT state */
    SHR_IF_ERR_EXIT
        (bcmltm_state_lt_get(unit, global_ltid, &global_lt_state));

    if (cur_index >= global_lt_state->bitmap_size) {
        /* Track index beyond the bounds of the global LT. */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_EXIT
        (bcmltm_state_data_get(unit, global_lt_state,
                               BCMLTM_STATE_TYPE_GLOBAL_BITMAP,
                               (void *) &global_bitmap));

    if (!SHR_BITGET(global_bitmap, cur_index)) {
        /* Entry does not exist.  Delete not permitted */
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Else, mark entry as now free.
     * Note that this is the working state which is discarded
     * on a subsequent failure any time during the transaction.
     */
    SHR_BITCLR(global_bitmap, cur_index);

 exit:
    SHR_FUNC_EXIT();
}

