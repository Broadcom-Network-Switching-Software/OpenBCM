/*! \file bcmltm_ee_passthru.c
 *
 * Logical Table Manager Execution Engine
 *
 * Physical table bypass implementation
 *
 * This file contains the LTM EE stage functions for PT PassThru
 * That is, the set of PTM operations which permit access to the
 * underlying physical device resources on which LTs are built.
 *
 * Under normal operations, these tables are not used to reach the
 * device.  These are provided for debug or unanticipated exception cases,
 * and perhaps as building blocks for hierarchical LTs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Working buffer layout for PT passthru
 */

/*
 * | uint32_t index |
 * | uint32_t entry_data[${SBUS_DATA_MAX_WORDS}] |
 */

#include <shr/shr_debug.h>
#include <bcmptm/bcmptm.h>
#include <bcmltm/bcmltm_table_op_pt_info.h>
#include <bcmltm/bcmltm_table_op_dop_info.h>
#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_tree.h>
#include <bcmltm/bcmltm_wb_ptm_internal.h>
#include <bcmltm/bcmltm_ee_tree_nodes_internal.h>
#include <bcmltm/bcmltm_stats_internal.h>
#include <bcmltm/bcmltm_device_op_pt_info.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_EXECUTIONENGINE


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Convert parameter offset list in WB to PTM dynamic hash info
 *
 * This function extracts the PTM parameters from the Working Buffer
 * based on the parsed parameter offsets and returns the information
 * in the dynamic hash info structure to be used by the PTM interactive
 * hash operation.
 *
 * \param [in] pt_flags BCMLTM_PT_OP_FLAGS_* to parse parameters.
 * \param [in] offsets The processed set of LTM buffer offsets that
 *                    indicate the location of each memory parameter.
 * \param [in] ltm_buffer Pointer to the LTM Working Buffer for this op.
 * \param [out] dyn_info The PTM dynamic information structure to be
 *                     filled out with the provided memory parameters.
 *
 * \retval None.
 */
static void
bcmltm_pt_dyn_hash_info_get(uint32_t pt_flags,
                            bcmltm_wb_ptm_param_offsets_t *offsets,
                            uint32_t *ltm_buffer,
                            bcmbd_pt_dyn_hash_info_t *dyn_hash_info)
{
    if (pt_flags & BCMLTM_PT_OP_FLAGS_TABLE_INST) {
        /*
         * Add the table instance to the PT dynamic info
         * Note, only one of port or table instance should
         * be specified for a given PT operation.
         */
        dyn_hash_info->tbl_inst = ltm_buffer[offsets->table_inst];
    }

    if (pt_flags & BCMLTM_PT_OP_FLAGS_BANK) {
        /*
         * Add the cur_index to the memen_spec.
         */
        dyn_hash_info->bank = ltm_buffer[offsets->bank];
    }
}

/*!
 * \brief Common logic for LTM-PTM interface of PT PassThru hash table ops.
 *
 * This function implements the common logic for the LTM-PTM interface
 * to dispatch INSERT, LOOKUP, and DELETE ops for PT PassThru hash
 * physical tables.  The PTM interface for each op is common, so the
 * preparatory logic is very similar.
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
bcmltm_pt_hash_opcode(int unit,
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
    bcmltm_pt_mem_args_t *cur_mem_args;
    bcmltm_pt_op_list_t *pt_op_list;
    bcmltm_wb_ptm_param_offsets_t param_offsets;
    bcmltm_wb_ptm_entry_keyed_pthru_offsets_t entry_offsets;
    bcmbd_pt_dyn_hash_info_t cur_pt_dyn_hash_info;
    uint32_t rsp_ltid;
    uint64_t req_flags;
    uint32_t rsp_flags;
    uint32_t ltid;
    uint32_t *entry_key;
    uint32_t *entry_data;
    uint32_t entry_index;
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    ltid = lt_md->params->ltid;

    pt_count = BCMLTM_PT_LIST_NUM(node_cookie);

    for (ptix = 0; ptix < pt_count; ptix++) {
        cur_mem_args = BCMLTM_PT_LIST_MEM_ARGS(node_cookie, ptix);
        pt_op_list = BCMLTM_PT_LIST_OPS_LIST(node_cookie, ptix);
        pt_sbus_count = pt_op_list->num_pt_ops;
        word_size = pt_op_list->word_size;
        pt_flags = pt_op_list->flags;

        /* Sanity check (since pt_dyn_hash_info is only created on Hashed PT) */
        if (cur_mem_args->pt_dyn_hash_info == NULL) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        /* Request flags */
        req_flags = cur_mem_args->flags;

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
            sal_memcpy(&cur_pt_dyn_hash_info, cur_mem_args->pt_dyn_hash_info,
                       sizeof(bcmbd_pt_dyn_hash_info_t));

            bcmltm_pt_dyn_hash_info_get(pt_flags, &param_offsets,
                                        ltm_buffer, &cur_pt_dyn_hash_info);

            if (pt_flags & BCMLTM_PT_OP_FLAGS_ACCTYPE) {
                /*
                 * Implementation pending
                 */
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
            }

            /* Prepare the entry arrays */
            (void)bcmltm_wb_ptm_entry_keyed_pthru_offsets_get(
                pt_op_list->buffer_offsets[opix],
                &entry_offsets);
            entry_key = ltm_buffer + entry_offsets.key;
            entry_data = ltm_buffer + entry_offsets.data;

            /*
             * Perform the PTM write operation.
             */
            bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                                   BCMLRD_FIELD_PHYSICAL_TABLE_OP_COUNT);
            rv =  bcmptm_ireq_hash_lt(unit,
                                      req_flags,
                                      cur_mem_args->pt,
                                      &cur_pt_dyn_hash_info,
                                      cur_mem_args->pt_static_info,
                                      entry_key,
                                      entry_data,
                                      pt_op,
                                      word_size,
                                      ltid,
                                      entry_key,
                                      entry_data,
                                      &entry_index,
                                      &rsp_ltid,
                                      &rsp_flags);

            /* Record PT passthrough operation */
            if (lt_md->params->lt_flags & BCMLTM_LT_FLAGS_PASSTHRU) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmltm_device_op(unit, cur_mem_args->pt,
                                      lt_entry->opcode.pt_opcode,
                                      cur_pt_dyn_hash_info.tbl_inst,
                                      entry_index, rv));
            }

            SHR_IF_ERR_VERBOSE_EXIT(rv);

            if (!(lt_md->params->lt_flags & BCMLTM_LT_FLAGS_PASSTHRU)) {
                /* Process the response values - index, LTID, and flags */
                if (lt_state->table_op_pt_info_enable) {
                    /*
                     * PT op info enable is checked inside this function
                     * as well due to use by the PTM callbacks for CTH.
                     * But the common case is disabled, so LTM internals will
                     * be more efficient to skip this function entirely.
                     */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmltm_pt_status_ireq_hash_lt(unit, ltid,
                                                       lt_entry->trans_id,
                                                       cur_mem_args->pt,
                                                       entry_index,
                                                       &cur_pt_dyn_hash_info));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmltm_dop_status_ireq_hash_lt(unit, ltid,
                                                        cur_mem_args->pt,
                                                        entry_index,
                                                        &cur_pt_dyn_hash_info));
                }
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

void
bcmltm_pt_dyn_info_get(uint32_t pt_flags,
                       bcmltm_wb_ptm_param_offsets_t *offsets,
                       uint32_t *ltm_buffer,
                       bcmbd_pt_dyn_info_t *dyn_info)
{
    if (pt_flags & BCMLTM_PT_OP_FLAGS_INDEX) {
        /*
         * Add the cur_index to the memen_spec.
         */
        dyn_info->index = ltm_buffer[offsets->index];
    }

    if (pt_flags & BCMLTM_PT_OP_FLAGS_PORT) {
        /*
         * Add the port number to the PT dynamic info.
         */
        dyn_info->tbl_inst = ltm_buffer[offsets->port];
    }

    if (pt_flags & BCMLTM_PT_OP_FLAGS_TABLE_INST) {
        /*
         * Add the table instance to the PT dynamic info
         * Note, only one of port or table instance should
         * be specified for a given PT operation.
         */
        dyn_info->tbl_inst = ltm_buffer[offsets->table_inst];
    }
}


int
bcmltm_ee_node_schan_write(int unit,
                           bcmltm_lt_md_t *lt_md,
                           bcmltm_lt_state_t *lt_state,
                           bcmltm_entry_t *lt_entry,
                           uint32_t *ltm_buffer,
                           void *node_cookie)
{
    uint32_t ptix, opix;
    uint32_t pt_flags;
    uint32_t pt_count, pt_sbus_count;
    uint32_t *cur_entry;
    bcmltm_pt_list_t *pt_list;
    bcmltm_pt_mem_args_t *cur_mem_args;
    bcmltm_pt_op_list_t *pt_op_list;
    bcmltm_wb_ptm_param_offsets_t param_offsets;
    bcmltm_wb_ptm_entry_index_offsets_t entry_offsets;
    bcmbd_pt_dyn_info_t cur_pt_dyn_info;
    uint32_t rsp_ltid;
    uint64_t req_flags;
    uint32_t rsp_flags;
    uint32_t ltid, ptid;
    uint32_t pt_suppress_offset;
    int rv = 0;

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
        pt_flags = pt_op_list->flags;

        /* Request flags */
        req_flags = cur_mem_args->flags;

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

            /* Get entry section */
            (void)bcmltm_wb_ptm_entry_index_offsets_get(
                pt_op_list->buffer_offsets[opix],
                &entry_offsets);
            cur_entry = ltm_buffer + entry_offsets.data;

            /*
             * Perform the PTM write operation.
             */
            bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                                   BCMLRD_FIELD_PHYSICAL_TABLE_OP_COUNT);
            rv = bcmptm_ireq_write(unit,
                                   req_flags,
                                   ptid,
                                   &cur_pt_dyn_info,
                                   cur_mem_args->pt_static_info,
                                   cur_entry,
                                   ltid,
                                   &rsp_ltid,
                                   &rsp_flags);

            /* Record PT passthrough operation */
            if (lt_md->params->lt_flags & BCMLTM_LT_FLAGS_PASSTHRU) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmltm_device_op(unit, ptid, lt_entry->opcode.pt_opcode,
                                      cur_pt_dyn_info.tbl_inst,
                                      cur_pt_dyn_info.index, rv));
            }

            SHR_IF_ERR_VERBOSE_EXIT(rv);

            if (!(lt_md->params->lt_flags & BCMLTM_LT_FLAGS_PASSTHRU)) {
                /* Process the response values - index, LTID, and flags */
                if (lt_state->table_op_pt_info_enable) {
                    /*
                     * PT op info enable is checked inside this function
                     * as well due to use by the PTM callbacks for CTH.
                     * But the common case is disabled, so LTM internals will
                     * be more efficient to skip this function entirely.
                     */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmltm_pt_status_ireq_op(unit, ltid,
                                                  lt_entry->trans_id, ptid,
                                                  &cur_pt_dyn_info));
                }
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

int
bcmltm_ee_node_schan_read(int unit,
                          bcmltm_lt_md_t *lt_md,
                          bcmltm_lt_state_t *lt_state,
                          bcmltm_entry_t *lt_entry,
                          uint32_t *ltm_buffer,
                          void *node_cookie)
{
    uint32_t ptix, opix;
    uint32_t pt_flags;
    uint32_t pt_count, pt_sbus_count, pt_word_size;
    uint32_t *cur_entry;
    bcmltm_pt_list_t *pt_list;
    bcmltm_pt_mem_args_t *cur_mem_args;
    bcmltm_pt_op_list_t *pt_op_list;
    bcmltm_wb_ptm_param_offsets_t param_offsets;
    bcmltm_wb_ptm_entry_index_offsets_t entry_offsets;
    bcmbd_pt_dyn_info_t cur_pt_dyn_info;
    uint32_t rsp_ltid;
    uint64_t req_flags;
    uint32_t rsp_flags;
    uint32_t ltid, ptid;
    uint32_t pt_suppress_offset;
    int rv = 0;

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
        pt_word_size = pt_op_list->word_size;
        pt_flags = pt_op_list->flags;

        /* Prepare PTM request flags */
        req_flags = cur_mem_args->flags;
        req_flags |= bcmptm_lt_entry_attrib_to_ptm_req_flags(lt_entry->attrib);

        for (opix = 0; opix < pt_sbus_count; opix++) {
            /* Fresh copy for any adjustments with PT_OP_FLAGS */
            sal_memcpy(&cur_pt_dyn_info, cur_mem_args->pt_dyn_info,
                       sizeof(bcmbd_pt_dyn_info_t));

            /* Get PTM parameter offsets */
            (void)bcmltm_wb_ptm_param_offsets_get(
                pt_op_list->param_offsets[opix],
                &param_offsets);

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

            /* Get entry section */
            (void)bcmltm_wb_ptm_entry_index_offsets_get(
                pt_op_list->buffer_offsets[opix],
                &entry_offsets);
            cur_entry = ltm_buffer + entry_offsets.data;

            /*
             * Perform the PTM write operation.
             */
            bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                                   BCMLRD_FIELD_PHYSICAL_TABLE_OP_COUNT);
            rv = bcmptm_ireq_read(unit,
                                  req_flags,
                                  ptid,
                                  &cur_pt_dyn_info,
                                  cur_mem_args->pt_static_info,
                                  pt_word_size,
                                  ltid,
                                  cur_entry,
                                  &rsp_ltid,
                                  &rsp_flags);

            /* Record PT passthrough operation */
            if (lt_md->params->lt_flags & BCMLTM_LT_FLAGS_PASSTHRU) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmltm_device_op(unit, ptid, lt_entry->opcode.pt_opcode,
                                      cur_pt_dyn_info.tbl_inst,
                                      cur_pt_dyn_info.index, rv));
            }

            SHR_IF_ERR_VERBOSE_EXIT(rv);

            if (!(lt_md->params->lt_flags & BCMLTM_LT_FLAGS_PASSTHRU)) {
                /* Process the response values - index, LTID, and flags */
                if (lt_state->table_op_pt_info_enable) {
                    /*
                     * PT op info enable is checked inside this function
                     * as well due to use by the PTM callbacks for CTH.
                     * But the common case is disabled, so LTM internals will
                     * be more efficient to skip this function entirely.
                     */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmltm_pt_status_ireq_op(unit, ltid,
                                                  lt_entry->trans_id, ptid,
                                                  &cur_pt_dyn_info));
                }
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

int
bcmltm_ee_node_pt_hash_insert(int unit,
                              bcmltm_lt_md_t *lt_md,
                              bcmltm_lt_state_t *lt_state,
                              bcmltm_entry_t *lt_entry,
                              uint32_t *ltm_buffer,
                              void *node_cookie)
{
    return bcmltm_pt_hash_opcode(unit, lt_md,
                                 lt_state, lt_entry,
                                 ltm_buffer, node_cookie,
                                 BCMPTM_OP_INSERT);
}

int
bcmltm_ee_node_pt_hash_delete(int unit,
                              bcmltm_lt_md_t *lt_md,
                              bcmltm_lt_state_t *lt_state,
                              bcmltm_entry_t *lt_entry,
                              uint32_t *ltm_buffer,
                              void *node_cookie)
{
    return bcmltm_pt_hash_opcode(unit, lt_md,
                                 lt_state, lt_entry,
                                 ltm_buffer, node_cookie,
                                 BCMPTM_OP_DELETE);
}

int
bcmltm_ee_node_pt_hash_lookup(int unit,
                              bcmltm_lt_md_t *lt_md,
                              bcmltm_lt_state_t *lt_state,
                              bcmltm_entry_t *lt_entry,
                              uint32_t *ltm_buffer,
                              void *node_cookie)
{
    int rv;

    rv = bcmltm_pt_hash_opcode(unit, lt_md,
                                lt_state, lt_entry,
                                ltm_buffer, node_cookie,
                                BCMPTM_OP_LOOKUP);

    return rv;
}


int
bcmltm_ee_node_pt_hash_lookup_not_found(int unit,
                                        bcmltm_lt_md_t *lt_md,
                                        bcmltm_lt_state_t *lt_state,
                                        bcmltm_entry_t *lt_entry,
                                        uint32_t *ltm_buffer,
                                        void *node_cookie)
{
    int rv;

    rv = bcmltm_pt_hash_opcode(unit, lt_md,
                                lt_state, lt_entry,
                                ltm_buffer, node_cookie,
                                BCMPTM_OP_LOOKUP);

    if (rv == SHR_E_NONE) {
        /*
         * Finding a previous entry is a failure.
         * INSERT must reference a new entry key.
         */
        LOG_VERBOSE(BSL_LS_BCMLTM_EXECUTIONENGINE,
                    (BSL_META_U(unit,
                                "Unanticipated existing keyed entry\n")));
        rv = SHR_E_EXISTS;
    } else if (rv == SHR_E_NOT_FOUND) {
        rv = SHR_E_NONE;
    }

    return rv;
}
