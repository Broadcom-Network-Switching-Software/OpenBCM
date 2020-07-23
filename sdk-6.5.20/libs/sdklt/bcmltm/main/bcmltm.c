/*! \file bcmltm.c
 *
 * Logical Table Manager
 *
 * This module contains the control implementation of Logical Table
 * Management functions.
 *
 * LTM northbound APIs
 * Transaction tracking, including commit and abort
 * Invocation of LTM abstract function node trees
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>

#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm.h>

#include <bcmlrd/bcmlrd_table.h>

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm.h>
#include <bcmltm/bcmltm_tree.h>
#include <bcmltm/bcmltm_fa_util_internal.h>
#include <bcmltm/bcmltm_fa_tree_nodes_internal.h>
#include <bcmltm/bcmltm_ee_tree_nodes_internal.h>
#include <bcmltm/bcmltm_ha_internal.h>
#include <bcmltm/bcmltm_md.h>
#include <bcmltm/bcmltm_stats_internal.h>
#include <bcmltm/bcmltm_md_pthru_internal.h>
#include <bcmltm/bcmltm_md_logical_internal.h>
#include <bcmltm/bcmltm_db_internal.h>
#include <bcmltm/bcmltm_db_table_internal.h>
#include <bcmltm/bcmltm_lta_cth_internal.h>
#include <bcmltm/bcmltm_stats_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_ENTRY

static bcmltm_transaction_status_t *bcmltm_trans_status[BCMDRD_CONFIG_MAX_UNITS];

static uint32_t *working_buffer_modeled[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t *working_buffer_interactive[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t wb_max_size[BCMDRD_CONFIG_MAX_UNITS];

/*!
 * \brief Table Commit Handler Interfaces.
 *
 * This enumeration contains the types of interfaces in a
 * Table Commit handler that can be called.
 */
typedef enum table_commit_intf_e {
    /*! Prepare the transaction. */
    TABLE_COMMIT_INTF_PREPARE = 0,

    /*! Commit the transaction. */
    TABLE_COMMIT_INTF_COMMIT = 1,

    /*! Abort the transaction. */
    TABLE_COMMIT_INTF_ABORT = 2,
} table_commit_intf_t;


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Provide a string pointer to the LT opcode name.
 *
 * This function returns a string pointer to the LT opcode name.
 *
 * \param [in] opcode LT opcode.
 *
 * \retval String pointer to the LT opcode name.
 */
static const char *
ltm_lt_opcode_str(bcmlt_opcode_t opcode)
{
    switch (opcode) {
    case BCMLT_OPCODE_NOP:
        return "NOP";
    case BCMLT_OPCODE_INSERT:
        return "INSERT";
    case BCMLT_OPCODE_LOOKUP:
        return "LOOKUP";
    case BCMLT_OPCODE_DELETE:
        return "DELETE";
    case BCMLT_OPCODE_UPDATE:
        return "UPDATE";
    case BCMLT_OPCODE_TRAVERSE:
        return "TRAVERSE";
    default:
        return "UNKNOWN";
    }
}

/*!
 * \brief Provide a string pointer to the PT opcode name.
 *
 * This function returns a string pointer to the PT opcode name.
 *
 * \param [in] opcode PT opcode.
 *
 * \retval String pointer to the PT opcode name.
 */
static const char *
ltm_pt_opcode_str(bcmlt_pt_opcode_t opcode)
{
    switch (opcode) {
    case BCMLT_PT_OPCODE_NOP:
        return "NOP";
    case BCMLT_PT_OPCODE_FIFO_POP:
        return "FIFO_POP";
    case BCMLT_PT_OPCODE_FIFO_PUSH:
        return "FIFO_PUSH";
    case BCMLT_PT_OPCODE_SET:
        return "SET";
    case BCMLT_PT_OPCODE_MODIFY:
        return "MODIFY";
    case BCMLT_PT_OPCODE_GET:
        return "GET";
    case BCMLT_PT_OPCODE_CLEAR:
        return "CLEAR";
    case BCMLT_PT_OPCODE_INSERT:
        return "INSERT";
    case BCMLT_PT_OPCODE_DELETE:
        return "DELETE";
    case BCMLT_PT_OPCODE_LOOKUP:
        return "LOOKUP";
    default:
        return "UNKNOWN";
    }
}

/*!
 *
 * \brief Allocate Working Buffers for a unit
 *
 * The Working Buffer space for a given unit, both modeled and
 * interactive modes, is allocated during initialization rather than
 * at the start of every operation.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors
 */
static int
bcmltm_wb_init(int unit)
{
    uint32_t wb_size_max, pt_wb_size_max;
    SHR_FUNC_ENTER(unit);

    if ((working_buffer_modeled[unit] != NULL) ||
        (working_buffer_interactive[unit] != NULL)) {
        /* This shouldn't happen? */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Create Modeled path Working Buffer space for this unit. */
    wb_size_max = 0;
    SHR_IF_ERR_EXIT
        (bcmltm_md_logical_wb_max_get(unit, BCMLTM_TABLE_MODE_MODELED,
                                      &wb_size_max));
    if (wb_size_max == 0) {
        /* No working buffer! */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(working_buffer_modeled[unit], wb_size_max,
              "bcmltmModeledWb");
    if (working_buffer_modeled[unit] == NULL) {
        LOG_ERROR(BSL_LS_BCMLTM_ENTRY,
                  (BSL_META_U(unit,
                              "Insufficient memory for modeled path"
                              "working buffer\n")));
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    /* Save max WB size for valid bitmap calculations */
    wb_max_size[unit] = wb_size_max;

    /* Create Interactive path Working Buffer space for this unit. */
    wb_size_max = 0;
    SHR_IF_ERR_EXIT
        (bcmltm_md_logical_wb_max_get(unit, BCMLTM_TABLE_MODE_INTERACTIVE,
                                      &wb_size_max));

    pt_wb_size_max = 0;
    SHR_IF_ERR_EXIT
        (bcmltm_md_pthru_wb_max_get(unit, &pt_wb_size_max));

    if (pt_wb_size_max > wb_size_max) {
        wb_size_max = pt_wb_size_max;
    }

    if (wb_size_max == 0) {
        /* No working buffer! */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(working_buffer_interactive[unit], wb_size_max,
               "bcmltmInteractiveWb");
    if (working_buffer_interactive[unit] == NULL) {
        LOG_ERROR(BSL_LS_BCMLTM_ENTRY,
                  (BSL_META_U(unit,
                              "Insufficient memory for interactive path"
                              "working buffer\n")));
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    if (wb_size_max > wb_max_size[unit]) {
        wb_max_size[unit] = wb_size_max;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 *
 * \brief Cleanup Working Buffers for a unit
 *
 * The Working Buffer space for a given unit, both modeled and
 * interactive modes, is freed during cleanup.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors
 */
static void
bcmltm_wb_cleanup(int unit)
{
        SHR_FREE(working_buffer_modeled[unit]);
        SHR_FREE(working_buffer_interactive[unit]);
}

/*!
 *
 * \brief Reconnect the HA state rollback copies of a transaction.
 *
 * During HA recovery, determine the correct state rollback copies
 * for the transaction in flight.
 *
 * \param [in] unit Unit number.
 * \param [in] state_mgmt LTM parameters for state tracking.
 *
 * \retval SHR_E_NONE No errors
 */
static int
bcmltm_rollback_states_recover(int unit,
                               bcmltm_state_mgmt_t *state_mgmt)
{
    bcmltm_ha_ptr_t state_rollback_hap, data_rollback_hap;
    void *state_rollback, *data_rollback;
    uint8_t lt_num;

    SHR_FUNC_ENTER(unit);

    state_rollback_hap =
       bcmltm_ha_blk_get(unit, BCMLTM_HA_BLOCK_TYPE_ROLLBACK_STATE);
    data_rollback_hap =
       bcmltm_ha_blk_get(unit, BCMLTM_HA_BLOCK_TYPE_ROLLBACK_STATE_DATA);

    lt_num = 0;
    while (lt_num < state_mgmt->lt_trans_max) {
        state_rollback = bcmltm_ha_ptr_to_mem_ptr(unit, state_rollback_hap);
        SHR_NULL_CHECK(state_rollback, SHR_E_MEMORY);
        state_mgmt->lt_state_rollback_chunk_hap[lt_num] = state_rollback_hap;

        data_rollback = bcmltm_ha_ptr_to_mem_ptr(unit, data_rollback_hap);
        SHR_NULL_CHECK(data_rollback, SHR_E_MEMORY);
        state_mgmt->lt_state_data_rollback_chunk_hap[lt_num] =
            data_rollback_hap;

        state_rollback_hap =
            bcmltm_ha_ptr_increment(unit, state_rollback_hap,
                                    state_mgmt->maximum_state_size);
        data_rollback_hap =
            bcmltm_ha_ptr_increment(unit, data_rollback_hap,
                                    state_mgmt->maximum_state_data_size);
        lt_num++;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check for second LTID in LT op
 *
 * Handle the cases where other table's state may be affected
 * 1) TABLE_CONTROL, which can change the state of other LTs.
 * 2) Shared index LTs, where one table manages the global
 *    in-use bitmap.
 *
 * For these cases the second LTID's state may be subject to
 * rollback, or commit/abort notification of CTH and transform LTA.
 *
 * This function is only called for logical (non PT PassThru) LTs.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Pointer to LTM entry specification.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [out] extra_ltid_valid Boolean TRUE if extra LTID for this LT op.
 * \param [out] extra_ltid Pointer for extra LTID value.
 *
 * \retval SHR_E_NONE No errors
 */
static int
bcmltm_extra_ltid_get(int unit,
                      bcmltm_entry_t *ltm_entry,
                      bcmltm_lt_md_t *lt_md,
                      bool *extra_ltid_valid,
                      uint32_t *extra_ltid)
{
    bcmltm_field_list_t *api_field_data;

    SHR_FUNC_ENTER(unit);

    *extra_ltid_valid = FALSE;
    if ((ltm_entry->table_id == TABLE_CONTROLt) &&
        (ltm_entry->opcode.lt_opcode == BCMLT_OPCODE_UPDATE)) {
        /* Need to capture the LT state of the target LT */
        api_field_data =
            bcmltm_api_find_field(ltm_entry->in_fields,
                                  TABLE_CONTROLt_TABLE_IDf,
                                  0);
        if (api_field_data == NULL) {
            LOG_VERBOSE(BSL_LS_BCMLTM_ENTRY,
                        (BSL_META_U(unit,
                                    "Missing TABLE_CONTROL.TABLE_ID field\n")));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        *extra_ltid = api_field_data->data;
        *extra_ltid_valid = TRUE;
    } else if (lt_md->params->lt_flags & BCMLTM_LT_FLAGS_OVERLAY) {
        *extra_ltid = lt_md->params->global_inuse_ltid;
        if (*extra_ltid != lt_md->params->ltid) {
            *extra_ltid_valid = TRUE;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 *
 * \brief Erase transaction status for a unit
 *
 * Erase the transaction record of a given unit upon termination of
 * the transaction (commit or abort).  Also clear the state rollback
 * memory areas.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors
 */
static void
bcmltm_transaction_clear(int unit)
{
    bcmltm_trans_status[unit]->lt_num = 0;
    return;
}

/*!
 *
 * \brief Initialize transaction management of LTM
 *
 * Create the transaction record memory organization within the
 * LTM HA transaction memory block.  Also prepare the state rollback
 * memory areas.
 *
 * The HA memory regions are not set to any initialized state
 * in this function.  The LTM HA module handles this properly
 * whether Cold Boot or HA recovery.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates if this is cold or warm boot.
 *
 * \retval SHR_E_NONE No errors
 */
static int
bcmltm_transaction_init(int unit, bool warm)
{
    uint8_t trans_lt_max, lt_num;
    bcmltm_ha_ptr_t trans_status_hap;
    uint32_t trans_status_size, state_size, state_data_size;
    bcmltm_state_mgmt_t *state_mgmt;
    bcmltm_ha_ptr_t state_rollback_hap, data_rollback_hap;
    void *state_rollback, *data_rollback;

    SHR_FUNC_ENTER(unit);

    trans_lt_max = bcmltm_ha_transaction_max(unit);
    if (trans_lt_max == 0) {
        /* Atomic transactions disabled. */
        bcmltm_trans_status[unit] = NULL;
        SHR_EXIT();
    }

    state_mgmt = bcmltm_state_mgmt_get(unit);
    SHR_NULL_CHECK(state_mgmt, SHR_E_INIT);

    if (warm) {
        trans_status_hap =
           bcmltm_ha_blk_get(unit, BCMLTM_HA_BLOCK_TYPE_TRANS_STATUS);
    } else {
        trans_status_size = sizeof(bcmltm_transaction_status_t) +
            (trans_lt_max * sizeof(uint32_t));

        trans_status_hap =
            bcmltm_ha_alloc(unit,
                            BCMLTM_HA_BLOCK_TYPE_TRANS_STATUS,
                            trans_status_size);
    }

    bcmltm_trans_status[unit] =
        bcmltm_ha_ptr_to_mem_ptr(unit, trans_status_hap);
    SHR_NULL_CHECK(bcmltm_trans_status[unit], SHR_E_MEMORY);

    if (warm) {
        if (trans_lt_max > bcmltm_trans_status[unit]->max_lt_num) {
            /* Need to resize HA block, or throw error. */
        }
    } else {
        bcmltm_trans_status[unit]->ltid_list_hap =
            BCMLTM_HA_PTR(BCMLTM_HA_BLOCK_TYPE_TRANS_STATUS,
                          sizeof(bcmltm_ha_block_header_t) +
                          sizeof(bcmltm_transaction_status_t));
    }

    /*
     * Allocation note
     * Number of transaction LTs is non-zero by check above
     */
    SHR_ALLOC(state_mgmt->lt_state_rollback_chunk_hap,
              trans_lt_max * sizeof(bcmltm_ha_ptr_t),
              "bcmltmStateRollbackHaPtrs");
    SHR_NULL_CHECK(state_mgmt->lt_state_rollback_chunk_hap, SHR_E_MEMORY);
    sal_memset(state_mgmt->lt_state_rollback_chunk_hap, 0,
               trans_lt_max * sizeof(bcmltm_ha_ptr_t));

    SHR_ALLOC(state_mgmt->lt_state_data_rollback_chunk_hap,
              trans_lt_max * sizeof(bcmltm_ha_ptr_t),
              "bcmltmStateDataRollbackHaPtrs");
    SHR_NULL_CHECK(state_mgmt->lt_state_data_rollback_chunk_hap,
                   SHR_E_MEMORY);
    sal_memset(state_mgmt->lt_state_data_rollback_chunk_hap, 0,
               trans_lt_max * sizeof(bcmltm_ha_ptr_t));

    state_size = state_mgmt->maximum_state_size;
    state_data_size = state_mgmt->maximum_state_data_size;

    if (!warm) {
        for (lt_num = 0; lt_num < trans_lt_max; lt_num++) {
            state_rollback_hap =
                bcmltm_ha_alloc(unit,
                                BCMLTM_HA_BLOCK_TYPE_ROLLBACK_STATE,
                                state_size);
            state_rollback =
                bcmltm_ha_ptr_to_mem_ptr(unit, state_rollback_hap);
            SHR_NULL_CHECK(state_rollback, SHR_E_MEMORY);
            state_mgmt->lt_state_rollback_chunk_hap[lt_num] =
                state_rollback_hap;

            data_rollback_hap =
                bcmltm_ha_alloc(unit,
                                BCMLTM_HA_BLOCK_TYPE_ROLLBACK_STATE_DATA,
                                state_data_size);
            data_rollback = bcmltm_ha_ptr_to_mem_ptr(unit, data_rollback_hap);
            SHR_NULL_CHECK(data_rollback, SHR_E_MEMORY);
            state_mgmt->lt_state_data_rollback_chunk_hap[lt_num] =
                data_rollback_hap;
        }
    } else {
        bcmltm_rollback_states_recover(unit,
                                       state_mgmt);
    }

    
    bcmltm_transaction_clear(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 *
 * \brief Check transaction status of LT entry operation
 *
 * Verify that a LT entry operation is valid within a transaction.
 * Validate the transaction ID is in proper sequence.
 * Note if this LTID has already been referenced by this transaction.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] ltid Logical Table ID.
 * \param [out] new_trans_ltid
 *        TRUE This LTID is not already include in in this transaction.
 *        FALSE This LTID is already in this transaction
 *
 * \retval SHR_E_NONE No errors
 */
static int
ltm_transaction_update(int unit,
                       uint32_t trans_id,
                       uint32_t ltid,
                       bool *new_trans_ltid)
{
    bcmltm_transaction_status_t *trans_status;
    uint32_t *ltid_list;
    uint32_t ltix, lt_num;

    trans_status = bcmltm_trans_status[unit];
    lt_num = bcmltm_trans_status[unit]->lt_num;
    ltid_list =
        bcmltm_ha_ptr_to_mem_ptr(unit, trans_status->ltid_list_hap);

    if (lt_num == 0) {
        /* Start of transaction after previous commit */
        trans_status->trans_id = trans_id;
    } else if (trans_status->trans_id != trans_id) {
        /* Mismatched transaction ID during incomplete transaction.
         * This should be caught at TRM layer.
         */
        return SHR_E_INTERNAL;
    } /* Else, new entry in this transaction */

    for (ltix = 0; ltix < lt_num; ltix++) {
        if (ltid_list[ltix] == ltid) {
            /* Found this entry's LTID in the transaction list */
            *new_trans_ltid = FALSE;
            return SHR_E_NONE;
        }
    }

    /* Didn't find this LTID in the existing transaction list, so add
     * it if there is room. */
    if (lt_num >= BCMLTM_TRANSACTION_LT_MAX) {
        /* No room to add LTID to transaction list */
        return SHR_E_RESOURCE;
    }

    *new_trans_ltid = TRUE;
    ltid_list[lt_num] = ltid;
    (bcmltm_trans_status[unit]->lt_num)++;

    return SHR_E_NONE;
}

/*!
 * \brief Cleanup LTM transaction and state management information.
 *
 * This function frees any physical memory allocated
 * for the LTM transaction and state management information.
 *
 * HA memory is left as is in case of future Warm Boot.
 *
 * \param [in] unit Unit number.
 */
static void
bcmltm_transaction_cleanup(int unit)
{
    uint8_t trans_lt_max;
    uint32_t trans_lt_list_size;
    bcmltm_transaction_status_t *trans_status;
    bcmltm_state_mgmt_t *state_mgmt;
    uint32_t *ltid_list;

    /* Cleanup Transaction Status */
    trans_status = bcmltm_trans_status[unit];
    if (trans_status != NULL) {
        bcmltm_transaction_clear(unit);
        trans_lt_max = trans_status->max_lt_num;
        ltid_list =
            bcmltm_ha_ptr_to_mem_ptr(unit, trans_status->ltid_list_hap);
        trans_lt_list_size =
            (trans_lt_max * sizeof(uint32_t));
        sal_memset(ltid_list, 0, trans_lt_list_size);
    }

    state_mgmt = bcmltm_state_mgmt_get(unit);
    if (state_mgmt != NULL) {
        SHR_FREE(state_mgmt->lt_state_rollback_chunk_hap);
        SHR_FREE(state_mgmt->lt_state_data_rollback_chunk_hap);
    }
}

/*!
 *
 * \brief Initialize LTM state data.
 *
 * This routine initializes the state data information
 * for all logical tables.
 *
 * PT Pass Thru tables do not have any state data information.
 *
 * Note: Cleanup is not necessary since the state data
 * cleanup happens during the LT state cleanup (part of the LT metadata
 * destroy).
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates if this is cold or warm boot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltm_state_data_init(int unit, bool warm)
{
    bcmltm_md_t *ltm_md;
    uint32_t sid;
    bcmltm_lt_md_t *lt_md;
    bcmltm_lt_state_t *lt_state = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmltm_md_logical_retrieve(unit, &ltm_md));

    for (sid = 0; sid < ltm_md->lt_max; sid++) {
        lt_md = ltm_md->lt_md[sid];

        /* Skip undefined tables */
        if (lt_md == NULL) {
            continue;
        }

        if (!warm) {
            /* Table ID is the index of the LT metadata */
            SHR_IF_ERR_EXIT
                (bcmltm_md_logical_state_data_reset(unit, sid));
        } else {
            bcmltm_table_info_t table_info;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmltm_db_table_info_get(unit, sid, &table_info));

            if (((table_info.opcodes & BCMLTM_OPCODE_FLAG_INSERT) == 0) &&
                ((table_info.opcodes & BCMLTM_OPCODE_FLAG_DELETE) == 0)) {
                if ((lt_md->table_entry_usage != NULL) &&
                    (lt_md->table_entry_limit->entry_limit_get != NULL)) {
                    /* Table size is coordinated with some other module */
                    bcmltd_table_entry_limit_arg_t table_arg;
                    bcmltd_table_entry_limit_t table_data;
                    int rv;

                    /*
                     * LTM does not change this table size.
                     * Trust the value from outside module.
                     */
                    sal_memset(&table_arg, 0, sizeof(table_arg));
                    table_arg.entry_maximum = table_info.entry_maximum;

                    rv = lt_md->table_entry_limit->entry_limit_get(unit,
                                            BCMPTM_DIRECT_TRANS_ID,
                                            sid,
                                            &table_arg,
                                            &table_data,
                                            lt_md->table_entry_limit->arg);
                    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
                    if (rv != SHR_E_UNAVAIL) {
                        SHR_IF_ERR_EXIT
                            (bcmltm_state_lt_get(unit, sid, &lt_state));
                        lt_state->max_entry_count = table_data.entry_limit;
                    }
                }
            }
        }

        if ((lt_md->table_entry_usage != NULL) &&
            (lt_md->table_entry_usage->max_entries_set != NULL)) {
            bcmltd_table_max_entries_arg_t table_arg;
            bcmltd_table_max_entries_t table_data;
            /*
             * For cold or warm, notify CTH implementations of the max_entry
             * setting stored in LTM HA state.
             */

            sal_memset(&table_arg, 0, sizeof(table_arg));
            sal_memset(&table_data, 0, sizeof(table_data));

            SHR_IF_ERR_EXIT
                (bcmltm_state_lt_get(unit, sid, &lt_state));
            table_data.max_entries = lt_state->max_entry_count;

            SHR_IF_ERR_VERBOSE_EXIT
                (lt_md->table_entry_usage->max_entries_set(
                    unit,
                    0,
                    sid,
                    &table_arg,
                    &table_data,
                    lt_md->table_entry_usage->arg));
        }
    }

    if (!warm) {
        uint32_t rsp_flags = 0;
        /* Clear PTM WAL status after entry limit queries above. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_mreq_indexed_lt(unit, BCMPTM_REQ_FLAGS_COMMIT,
                                    0, NULL, NULL, NULL, BCMPTM_OP_NOP,
                                    NULL, 0, 0, BCMPTM_DIRECT_TRANS_ID,
                                    NULL, NULL, NULL, NULL, &rsp_flags));
    }

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Call the specified Table Commit handler interface.
 *
 * This routine calls the Table Commit handler for the specified
 * interface, prepare, commit, or abort.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] tc_list Table Commit handler list.
 * \param [in] intf Handler interface type to call TABLE_COMMIT_INTF_.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
table_commit_process(int unit,
                     uint32_t trans_id,
                     const bcmltm_table_commit_list_t *tc_list,
                     table_commit_intf_t intf)
{
    const bcmltd_table_commit_handler_t *handler;
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    if (tc_list == NULL) {
        SHR_EXIT();
    }

    for (i = 0; i < tc_list->num_handlers; i++) {
        handler = tc_list->handlers[i];
        if (handler == NULL) {
            continue;
        }

        if (intf == TABLE_COMMIT_INTF_PREPARE) {
            SHR_IF_ERR_VERBOSE_EXIT
                (handler->prepare(unit, trans_id, handler->arg));
        } else if (intf == TABLE_COMMIT_INTF_COMMIT) {
            SHR_IF_ERR_VERBOSE_EXIT
                (handler->commit(unit, trans_id, handler->arg));
        } else if (intf == TABLE_COMMIT_INTF_ABORT) {
            SHR_IF_ERR_VERBOSE_EXIT
                (handler->abort(unit, trans_id, handler->arg));
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

 exit:
    SHR_FUNC_EXIT();
}

/*!
 *
 * \brief Process one LT entry operation.
 *
 * The LTM implementation of a single LT entry operation.
 *
 * Receive a single logical table entry update from the LT operation handler.
 * Perform a sequence of Field Adaptation and Execution Engine operations
 * for the relevant API op requested.
 * Provide appropriate responses via API entry and/or notification
 * callbacks.
 *
 * \param [in] entry Pointer to LTM entry specification.
 * \param [in] passthru TRUE if PT PassThru table
 *                      FALSE if Logical table
 *
 * \retval SHR_E_NONE No errors
 */
static int
ltm_entry_operation(bcmltm_entry_t *ltm_entry, bool passthru)
{
    /*
     * These memory entries should be pointers, implemented as a bank of
     * pre-allocated buffers from which they may be drawn.
     * This will be similar to the RX buffer methodology.
     */
    uint32_t unit = ltm_entry->unit;
    uint32_t *working_buffer = NULL;
    bcmltm_lt_md_t *lt_md = NULL;
    bcmltm_lt_state_t *lt_state = NULL;
    bcmltm_ha_ptr_t lt_state_hap;
    bcmltm_lt_op_md_t *op_md;
    uint32_t trix, opix;
    bcmltm_table_catg_t table_catg;
    bcmltm_field_stats_t op_stat, op_err_stat;
    bool new_trans_ltid;
    bool extra_trans_ltid;
    uint32_t extra_ltid;
    bcmltm_lt_md_t *extra_lt_md = NULL;
    bcmltm_ha_ptr_t extra_lt_state_hap;
    uint32_t table_id;
    uint32_t *lt_stats;
    const char *table_catg_str = "";
    const char *table_name = "";
    const char *table_sid_type = "";
    const char *opcode_str = "";
    const char *extra_table_name = "";

    SHR_FUNC_ENTER(unit);

    /*
     * Determine metadata relevant to the selected table
     */
    table_id = ltm_entry->table_id;
    if (passthru) {
        opix = (uint32_t) ltm_entry->opcode.pt_opcode;
        table_catg = BCMLTM_TABLE_CATG_PTHRU;
        table_catg_str = "Physical";
        opcode_str = ltm_pt_opcode_str(opix);
        table_name = bcmltm_pt_table_sid_to_name(unit, table_id);
        table_sid_type = bcmltm_table_sid_type_str(FALSE);
    } else {
        opix = (uint32_t) ltm_entry->opcode.lt_opcode;
        table_catg = BCMLTM_TABLE_CATG_LOGICAL;
        table_catg_str = "Logical";
        opcode_str = ltm_lt_opcode_str(opix);
        table_name = bcmltm_lt_table_sid_to_name(unit, table_id);
        table_sid_type = bcmltm_table_sid_type_str(TRUE);
    }

    LOG_VERBOSE(BSL_LS_BCMLTM_ENTRY,
                (BSL_META_U(unit,
                            "%s Table Entry Operation %s "
                            "%s (%s=%d trans_id=%d)\n"),
                 table_catg_str, opcode_str,
                 table_name, table_sid_type, table_id, ltm_entry->trans_id));

    /* Dynamic init metadata */
    SHR_IF_ERR_EXIT(bcmltm_md_lt_retrieve(unit,
                                          table_catg,
                                          table_id, &lt_md));

    if (lt_md == NULL) {
        LOG_VERBOSE(BSL_LS_BCMLTM_ENTRY,
                    (BSL_META_U(unit,
                                "Unrecognized %s Table %s (%s=%d)\n"),
                     table_catg_str, table_name, table_sid_type, table_id));
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    /* Working LT state */
    lt_state_hap = lt_md->params->lt_state_hap;
    if (BCMLTM_HA_PTR_IS_INVALID(lt_state_hap)) {
        /* This LT has no state, so this is not an error. */
        lt_state = NULL;
    } else {
        /* This LT should have state, so the pointer must be valid. */
        lt_state = bcmltm_ha_ptr_to_mem_ptr(unit, lt_state_hap);
        SHR_NULL_CHECK(lt_state, SHR_E_INTERNAL);
    }

    if (lt_md->params->lt_flags & BCMLTM_LT_FLAGS_MODELED) {
        if (bcmltm_trans_status[unit] != NULL) {
            SHR_IF_ERR_EXIT
                (ltm_transaction_update(unit, ltm_entry->trans_id,
                                        table_id,
                                        &new_trans_ltid));

            if (new_trans_ltid) {
                /*
                 * First reference to this modeled path LT in this transaction.
                 * Make a rollback copy of the LT state in case of abort.
                 */
                SHR_IF_ERR_EXIT
                    (bcmltm_state_clone(unit,
                                        (bcmltm_trans_status[unit]->lt_num - 1),
                                        lt_state_hap,
                                  &(lt_md->params->lt_rollback_state_hap)));

                /* Call Table Commit handler to indicate start of transaction */
                SHR_IF_ERR_VERBOSE_EXIT
                    (table_commit_process(unit, ltm_entry->trans_id,
                                          lt_md->tc_list,
                                          TABLE_COMMIT_INTF_PREPARE));
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmltm_extra_ltid_get(unit, ltm_entry, lt_md,
                                       &extra_trans_ltid, &extra_ltid));

            if (extra_trans_ltid) {
                /*
                 * Check metadata exists to indicate valid LTID before
                 * adding to transaction list.
                 */
                SHR_IF_ERR_EXIT
                    (bcmltm_md_lt_retrieve(unit,
                                           table_catg,
                                           extra_ltid, &extra_lt_md));

                if (extra_lt_md == NULL) {
                    extra_table_name = bcmltm_lt_table_sid_to_name(unit,
                                                                   extra_ltid);
                    LOG_VERBOSE(BSL_LS_BCMLTM_ENTRY,
                                (BSL_META_U(unit,
                                            "Invalid Table %s(ltid=%d)\n"),
                                 extra_table_name, extra_ltid));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }

                SHR_IF_ERR_EXIT
                    (ltm_transaction_update(unit, ltm_entry->trans_id,
                                            extra_ltid,
                                            &new_trans_ltid));

                if (new_trans_ltid) {
                    /* First reference to this modeled path LT in this
                      * transaction.
                     * Make a rollback copy of the LT state in case of abort.
                     */
                    extra_lt_state_hap = extra_lt_md->params->lt_state_hap;
                    if (BCMLTM_HA_PTR_IS_INVALID(extra_lt_state_hap)) {
                        extra_table_name =
                            bcmltm_lt_table_sid_to_name(unit, extra_ltid);
                        /* No valid LT state! */
                        LOG_VERBOSE(BSL_LS_BCMLTM_ENTRY,
                                    (BSL_META_U(unit,
                                                "Missing LT state for %s "
                                                "Table %s(ltid=%d)\n"),
                                     table_catg_str,
                                     extra_table_name, extra_ltid));
                        return SHR_E_INTERNAL;
                    }

                    SHR_IF_ERR_EXIT
                        (bcmltm_state_clone(unit,
                                      (bcmltm_trans_status[unit]->lt_num - 1),
                                            extra_lt_state_hap,
                                &(extra_lt_md->params->lt_rollback_state_hap)));

                    /*
                     * Call Table Commit handler to indicate start of
                     * transaction.
                     */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (table_commit_process(unit, ltm_entry->trans_id,
                                              extra_lt_md->tc_list,
                                              TABLE_COMMIT_INTF_PREPARE));
                }
            }
        } else {
            /*
             * Atomic transactions are turned off, but CTH/transforms
             * must still be notified.
             */

            /* Call Table Commit handler to indicate start of operation */
            SHR_IF_ERR_VERBOSE_EXIT
                (table_commit_process(unit, ltm_entry->trans_id,
                                      lt_md->tc_list,
                                      TABLE_COMMIT_INTF_PREPARE));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmltm_extra_ltid_get(unit, ltm_entry, lt_md,
                                       &extra_trans_ltid, &extra_ltid));

            if (extra_trans_ltid) {
                /*
                 * Check metadata exists to indicate valid LTID before
                 * proceeding.
                 */
                SHR_IF_ERR_EXIT
                    (bcmltm_md_lt_retrieve(unit,
                                           table_catg,
                                           extra_ltid, &extra_lt_md));

                if (extra_lt_md == NULL) {
                    extra_table_name =
                        bcmltm_lt_table_sid_to_name(unit, extra_ltid);
                    LOG_VERBOSE(BSL_LS_BCMLTM_ENTRY,
                                (BSL_META_U(unit,
                                            "Invalid Table %s(ltid=%d)\n"),
                                 extra_table_name, extra_ltid));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }

                /*
                 * Call Table Commit handler to indicate start of
                 * operation.
                 */
                SHR_IF_ERR_VERBOSE_EXIT
                    (table_commit_process(unit, ltm_entry->trans_id,
                                          extra_lt_md->tc_list,
                                          TABLE_COMMIT_INTF_PREPARE));
            }
        }
        working_buffer = working_buffer_modeled[unit];
    } else {
        working_buffer = working_buffer_interactive[unit];
    }

    op_md = lt_md->op[opix];
    if (op_md == NULL) {
        LOG_VERBOSE(BSL_LS_BCMLTM_ENTRY,
                    (BSL_META_U(unit,
                                "Operation %s not available on this table "
                                "%s (%s=%d)\n"),
                     opcode_str, table_name, table_sid_type, table_id));
        SHR_ERR_EXIT(SHR_E_NO_HANDLER);
    }

    /* Clear Working Buffer */
    sal_memset(working_buffer, 0, op_md->working_buffer_size);

    for (trix = 0; trix < op_md->num_nodes.active_node_count; trix++) {
        /* Execute node functions */
        if ((op_md->nodes_array[trix] != NULL) &&
            (op_md->nodes_array[trix]->node_function != NULL)){
            SHR_IF_ERR_VERBOSE_EXIT
                ((op_md->nodes_array[trix]->node_function)(unit,
                                                           lt_md,
                                                           lt_state,
                                                           ltm_entry,
                                                           working_buffer,
                              op_md->nodes_array[trix]->node_cookie));
        }
    }

 exit:
    if (lt_md != NULL) {
        lt_stats = BCMLTM_STATS_ARRAY(lt_md);
        if (table_catg == BCMLTM_TABLE_CATG_LOGICAL) {
            op_stat = op_err_stat = BCMLRD_FIELD_STATS_NUM;
            switch (opix) {
            case BCMLT_OPCODE_INSERT:
                op_stat = BCMLRD_FIELD_LT_INSERT_COUNT;
                op_err_stat = BCMLRD_FIELD_LT_INSERT_ERROR_COUNT;
                if (!SHR_FUNC_ERR() && (lt_state != NULL) &&
                    (lt_md->table_entry_usage == NULL)) {
                    /* Increase inuse entries on successful INSERT */
                    (lt_state->entry_count)++;
                }
                break;
            case BCMLT_OPCODE_LOOKUP:
                op_stat = BCMLRD_FIELD_LT_LOOKUP_COUNT;
                op_err_stat = BCMLRD_FIELD_LT_LOOKUP_ERROR_COUNT;
                break;
            case BCMLT_OPCODE_DELETE:
                op_stat = BCMLRD_FIELD_LT_DELETE_COUNT;
                op_err_stat = BCMLRD_FIELD_LT_DELETE_ERROR_COUNT;
                if (!SHR_FUNC_ERR() && (lt_state != NULL) &&
                    (lt_md->table_entry_usage == NULL)) {
                    /* Decrease inuse entries on successful DELETE */
                    (lt_state->entry_count)--;
                }
                break;
            case BCMLT_OPCODE_UPDATE:
                op_stat = BCMLRD_FIELD_LT_UPDATE_COUNT;
                op_err_stat = BCMLRD_FIELD_LT_UPDATE_ERROR_COUNT;
                break;
            case BCMLT_OPCODE_TRAVERSE:
                op_stat = BCMLRD_FIELD_LT_TRAVERSE_COUNT;
                op_err_stat = BCMLRD_FIELD_LT_TRAVERSE_ERROR_COUNT;
                break;
            }
            if (SHR_FUNC_ERR()) {
                bcmltm_stats_increment(lt_stats, op_err_stat);
            }
            bcmltm_stats_increment(lt_stats, op_stat);

            if ((bcmltm_trans_status[unit] == NULL) &&
                (lt_md->params->lt_flags & BCMLTM_LT_FLAGS_MODELED)) {
                const bcmltd_table_handler_t *cth_handler = NULL;
                table_commit_intf_t intf;
                /*
                 * Atomic transactions disabled.
                 * Do LTM internal commit/abort now.
                 */

                intf = SHR_FUNC_ERR() ?
                    TABLE_COMMIT_INTF_ABORT :
                    TABLE_COMMIT_INTF_COMMIT;

                /* Call Table Commit handler for commit */
                SHR_IF_ERR_VERBOSE_EXIT
                    (table_commit_process(unit,
                                          ltm_entry->trans_id,
                                          lt_md->tc_list,
                                          intf));

                /* If LT is CTH, call handler commit */
                cth_handler = lt_md->cth;
                if (cth_handler != NULL) {
                    if (cth_handler->ha != NULL) {
                        if (SHR_FUNC_ERR()) {
                            SHR_IF_ERR_VERBOSE_EXIT
                                (cth_handler->ha->abort(unit,
                                                        ltm_entry->trans_id,
                                                        cth_handler->arg));
                        } else {
                            SHR_IF_ERR_VERBOSE_EXIT
                                (cth_handler->ha->commit(unit,
                                                         ltm_entry->trans_id,
                                                         cth_handler->arg));
                        }
                    }
                }
            }
        }

        if (SHR_FUNC_ERR()) {
            bcmltm_stats_increment(lt_stats,
                                   BCMLRD_FIELD_LT_ERROR_COUNT);
        }
    }

    /*
     * Log verbose message only on SUCCESS.
     * Failure cases should have already logged corresponding error earlier.
     */
    if (!SHR_FUNC_ERR()) {
        LOG_VERBOSE(BSL_LS_BCMLTM_ENTRY,
                    (BSL_META_U(unit,
                                "%s %s successful (%s=%d trans_id=%d)\n"),
                     opcode_str, table_name, table_sid_type, table_id,
                     ltm_entry->trans_id));
    }

    SHR_FUNC_EXIT();
}


/*!
 *
 * \brief Prepare one LT entry operation for common processing
 *
 * Common implementation to ready a Logical Table operation for
 * opcode processing.
 *
 * The TRM-LTM interface uses separate functions for each opcode.
 * The LTM implementation combines them into a single LTM opcode
 * processing step.  This happens in two stages
 * 1) Combine LT (or PT PassThru) opcodes together
 * 2) Merge PT PassThru and LT opcodes processing at the common
 * implementation.
 *
 * \param [in] lt_op The LT opcode determined by the TRM function call.
 * \param [in] entry Pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors
 */
static int
bcmltm_entry_process(bcmlt_opcode_t lt_op,
                     bcmltm_entry_t *entry)
{
    if (entry == NULL) {
        LOG_ERROR(BSL_LS_BCMLTM_ENTRY,
                  ("No entry structure provided to %s op\n",
                   ltm_lt_opcode_str(lt_op)));
        return SHR_E_PARAM;
    }

    entry->opcode.lt_opcode = lt_op;
    return ltm_entry_operation(entry, FALSE);
}

/*!
 *
 * \brief Prepare one PT entry operation for common processing
 *
 * Common implementation to ready a PT PassThru operation for
 * opcode processing.
 *
 * The TRM-LTM interface uses separate functions for each opcode.
 * The LTM implementation combines them into a single LTM opcode
 * processing step.  This happens in two stages
 * 1) Combine PT PassThru (or LT) opcodes together
 * 2) Merge PT PassThru and LT opcodes processing at the common
 * implementation.
 *
 * \param [in] ptpt_op The PT PassThru opcode determined by the
 *                     TRM function call.
 * \param [in] entry Pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors
 */
static int
bcmltm_entry_pt_process(bcmlt_pt_opcode_t ptpt_op,
                        bcmltm_entry_t *entry)
{
    if (entry == NULL) {
        LOG_ERROR(BSL_LS_BCMLTM_ENTRY,
                  ("No entry structure provided to %s op\n",
                   ltm_pt_opcode_str(ptpt_op)));
        return SHR_E_PARAM;
    }

    entry->opcode.pt_opcode = ptpt_op;

    return ltm_entry_operation(entry, TRUE);
}

/*!
 * \brief Retrieve statistics array of a LT.
 *
 * Fetch the statistics array of a particular Logical Table.
 * This function is the high-level implementation which is registered
 * via callback to provide access to lower-level LTM submodules.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical Table ID.
 * \param [out] lt_stats_p Pointer to the statistics array for this LT.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM LTID outside the range of valid LTIDs.
 * \retval SHR_E_NOT_FOUND No state for this LTID.
 */
static int
bcmltm_internal_stats_lt_get(int unit,
                             uint32_t ltid,
                             uint32_t **lt_stats_p)
{
    int rv;
    bcmltm_lt_md_t *lt_md = NULL;

    rv = bcmltm_md_lt_retrieve(unit,
                               BCMLTM_TABLE_CATG_LOGICAL,
                               ltid, &lt_md);

    if (SHR_FAILURE(rv)) {
        return rv;
    }

    if (lt_md == NULL) {
        return SHR_E_INTERNAL;
    }

    *lt_stats_p = lt_md->lt_stats;

    return SHR_E_NONE;
}

/*!
 * \brief Retrieve total state of a LT.
 *
 * Fetch the total working state of a particular Logical Table.
 * This function is the high-level implementation which is registered
 * via callback to provide access to lower-level LTM submodules.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical Table ID.
 * \param [out] lt_state Pointer to the state for this LT.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM LTID outside the range of valid LTIDs.
 * \retval SHR_E_NOT_FOUND No state for this LTID.
 */
static int
bcmltm_internal_state_lt_get(int unit,
                             uint32_t ltid,
                             bcmltm_lt_state_t **lt_state_p)
{
    int rv;
    bcmltm_lt_md_t *lt_md = NULL;
    bcmltm_ha_ptr_t lt_state_hap;

    rv = bcmltm_md_lt_retrieve(unit,
                               BCMLTM_TABLE_CATG_LOGICAL,
                               ltid, &lt_md);

    if (SHR_FAILURE(rv)) {
        return rv;
    }

    if (lt_md == NULL) {
        return SHR_E_INTERNAL;
    }

    lt_state_hap = lt_md->params->lt_state_hap;
    *lt_state_p = bcmltm_ha_ptr_to_mem_ptr(unit, lt_state_hap);

    if (*lt_state_p == NULL) {
        return SHR_E_INTERNAL;
    }

    return SHR_E_NONE;
}

/*!
 * \brief Retrieve total state of a LT.
 *
 * Fetch the total working state of a particular Logical Table.
 * This function is the high-level implementation which is registered
 * via callback to provide access to lower-level LTM submodules.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical Table ID.
 * \param [out] lt_state Pointer to the state for this LT.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM LTID outside the range of valid LTIDs.
 * \retval SHR_E_NOT_FOUND No state for this LTID.
 */
static int
bcmltm_internal_info_lt_get(int unit,
                            uint32_t table_id,
                            uint32_t field_id,
                            uint32_t field_idx,
                            uint64_t *data_value)
{
    int rv;
    bcmltm_lt_md_t *lt_md = NULL;

    rv = bcmltm_md_lt_retrieve(unit,
                               BCMLTM_TABLE_CATG_LOGICAL,
                               table_id, &lt_md);

    if (SHR_FAILURE(rv)) {
        return rv;
    }

    if (lt_md == NULL) {
        return SHR_E_INTERNAL;
    }

    return SHR_E_NONE;
}


/*!
 * \brief Update the valid bitmap of a LT.
 *
 * Recalculate the valid bitmap of an Index w/Allocation Logical Table.
 * Using the currently configured index key field bounds, iterate
 * through every valid key field combination to create the valid
 * bitmap of tracking indexes.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical Table ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Insufficient system memory for structure allocation.
 * \retval SHR_E_INTERNAL Inconsistent LTM state.
 */
static int
bcmltm_internal_state_lt_valid_bitmap_update(int unit,
                                             uint32_t ltid)
{
    int rv;
    bcmltm_lt_state_t *lt_state = NULL;
    uint32_t alloc_size;
    SHR_BITDCL *inuse_bitmap, *valid_bitmap;
    SHR_BITDCL *valid_ti_bitmap = NULL;
    SHR_BITDCL  *compare_bitmap = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get LT state */
    SHR_IF_ERR_EXIT
        (bcmltm_state_lt_get(unit, ltid, &lt_state));
    SHR_NULL_CHECK(lt_state, SHR_E_INTERNAL);

    /* Get in-use bitmap */
    SHR_IF_ERR_EXIT
        (bcmltm_state_data_get(unit, lt_state,
                               BCMLTM_STATE_TYPE_INUSE_BITMAP,
                               (void *) &inuse_bitmap));
    SHR_NULL_CHECK(inuse_bitmap, SHR_E_INTERNAL);

    /* Get current valid bitmap, if any */
    rv = bcmltm_state_data_get(unit, lt_state,
                               BCMLTM_STATE_TYPE_VALID_BITMAP,
                               (void *) &valid_bitmap);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
    /*
     * No NULL check, because we need to do this even if valid_bitmap
     * doesn't exist for Simple Index LTs.  Not found will set the
     * pointer to NULL.
     */

    /* Create extra bitmaps (note all bitmaps for a LT are same size) */
    SHR_IF_ERR_EXIT
        (bcmltm_state_data_size_get(unit, lt_state,
                                    BCMLTM_STATE_TYPE_INUSE_BITMAP,
                                    &alloc_size));
    if (alloc_size == 0) {
        /* No entries */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(compare_bitmap, alloc_size, "bcmltmCompareBitmap");
    SHR_NULL_CHECK(compare_bitmap, SHR_E_MEMORY);
    sal_memset(compare_bitmap, 0, alloc_size);
    SHR_ALLOC(valid_ti_bitmap, alloc_size,
              "bcmltmValidTrackIndexBitmap");
    SHR_NULL_CHECK(valid_ti_bitmap, SHR_E_MEMORY);
    sal_memset(valid_ti_bitmap, 0, alloc_size);

    /* Get the valid bitmap of track indexes */
    SHR_IF_ERR_EXIT
        (bcmltm_md_logical_valid_track_index_bitmap_get(unit, ltid,
                                                        valid_ti_bitmap));

    /* Check that valid bitmap does not abandon any existing entries. */
    SHR_BITREMOVE_RANGE(inuse_bitmap, valid_ti_bitmap,
                        0, lt_state->bitmap_size,
                        compare_bitmap);

    if (SHR_BITNULL_RANGE(compare_bitmap,
                          0, lt_state->bitmap_size)) {
        /* No orphaned entries, proceed with update. */
        if (valid_bitmap != NULL) {
            /* Copy new bitmap over existing valid bitmap in LT state. */
            SHR_BITCOPY_RANGE(valid_bitmap, 0, valid_ti_bitmap,
                              0, lt_state->bitmap_size);
        }
        /*
         * Else, no valid bitmap tracked, new valid bitmap
         * was only to check for orphaned entries.
         */
    } else {
        /* Orphaned entries must be removed before changing key bounds. */
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

 exit:
    /* Clean up all allocated memory */
    SHR_FREE(compare_bitmap);
    SHR_FREE(valid_ti_bitmap);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve LTA entry limit handler of a LT.
 *
 * Fetch the LTA entry limit handler of a particular Logical Table.
 * This function is the high-level implementation which is registered
 * via callback to provide access to lower-level LTM submodules.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical Table ID.
 * \param [out] table_entry_limit Pointer to the entry limit handler
 *                                for this LT
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM LTID outside the range of valid LTIDs.
 * \retval SHR_E_NOT_FOUND No state for this LTID.
 */
static int
bcmltm_internal_entry_limit_get(int unit,
                                uint32_t table_id,
                                const bcmltd_table_entry_limit_handler_t
                                **table_entry_limit)
{
    int rv;
    bcmltm_lt_md_t *lt_md = NULL;

    rv = bcmltm_md_lt_retrieve(unit,
                               BCMLTM_TABLE_CATG_LOGICAL,
                               table_id, &lt_md);

    if (SHR_FAILURE(rv)) {
        return rv;
    }

    if (lt_md == NULL) {
        return SHR_E_INTERNAL;
    }

    *table_entry_limit = lt_md->table_entry_limit;

    return SHR_E_NONE;
}

/*!
 * \brief Retrieve LTA entry usage handler of a LT.
 *
 * Fetch the LTA entry usage handler of a particular Logical Table.
 * This function is the high-level implementation which is registered
 * via callback to provide access to lower-level LTM submodules.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical Table ID.
 * \param [out] table_entry_usage Pointer to the entry usage handler
 *                                for this LT
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM LTID outside the range of valid LTIDs.
 * \retval SHR_E_NOT_FOUND No state for this LTID.
 */
static int
bcmltm_internal_entry_usage_get(int unit,
                                uint32_t table_id,
                                const bcmltd_table_entry_usage_handler_t
                                **table_entry_usage)
{
    int rv;
    bcmltm_lt_md_t *lt_md = NULL;

    rv = bcmltm_md_lt_retrieve(unit,
                               BCMLTM_TABLE_CATG_LOGICAL,
                               table_id, &lt_md);

    if (SHR_FAILURE(rv)) {
        return rv;
    }

    if (lt_md == NULL) {
        return SHR_E_INTERNAL;
    }

    *table_entry_usage = lt_md->table_entry_usage;

    return SHR_E_NONE;
}

/*******************************************************************************
 * Public functions
 */

/*
 * bcmltm_init
 *
 * Initialize the metadata for managing the LTs on a specific unit
 */
int
bcmltm_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    /*
     * Incorporate the regsfile products, PEMLA/CANCUN output, and
     * application configuration to construct the metadata
     * for managing LTs and mapping them to the PTs on which they
     * are implemented.
     */

    LOG_VERBOSE(BSL_LS_BCMLTM_INIT,
                (BSL_META_U(unit,
                            "Logical Table Resource Manager initialization\n")));

    /* Initialize LTM metadata */
    SHR_IF_ERR_EXIT(bcmltm_md_init(unit, warm));

    /* Initialize LTM custom table handlers for this unit */
    SHR_IF_ERR_EXIT(bcmltm_lta_cth_init(unit));

    /* Initialize LTM Working Buffers */
    SHR_IF_ERR_EXIT(bcmltm_wb_init(unit));

    /* Initialize transaction management and state rollback buffers */
    SHR_IF_ERR_EXIT(bcmltm_transaction_init(unit, warm));

    /* Register LTM internal callbacks. */
    bcmltm_stats_lt_get_register(unit, bcmltm_internal_stats_lt_get);
    bcmltm_state_lt_get_register(unit, bcmltm_internal_state_lt_get);
    bcmltm_info_lt_get_register(unit, bcmltm_internal_info_lt_get);
    bcmltm_state_lt_valid_bitmap_update_register(unit,
                    bcmltm_internal_state_lt_valid_bitmap_update);
    bcmltm_entry_limit_get_register(unit, bcmltm_internal_entry_limit_get);
    bcmltm_entry_usage_get_register(unit, bcmltm_internal_entry_usage_get);

    /* Initialize state data */
    SHR_IF_ERR_EXIT(ltm_state_data_init(unit, warm));

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Cleanup of state data is not needed */

    /* Cleanup transaction management and state rollback buffers */
    bcmltm_transaction_cleanup(unit);

    /* Cleanup LTM Working Buffers */
    bcmltm_wb_cleanup(unit);

    /* Cleanup LTM custom table handlers for this unit */
    SHR_IF_ERR_EXIT(bcmltm_lta_cth_cleanup(unit));

    /* Cleanup LTM metadata */
    SHR_IF_ERR_EXIT(bcmltm_md_cleanup(unit));

 exit:
    SHR_FUNC_EXIT();
}

/*****
 *
 * Standard LT TRM interface functions
 */

int
bcmltm_entry_insert(bcmltm_entry_t *entry)
{
    return bcmltm_entry_process(BCMLT_OPCODE_INSERT,
                                entry);
}

int
bcmltm_entry_lookup(bcmltm_entry_t *entry)
{
    return bcmltm_entry_process(BCMLT_OPCODE_LOOKUP,
                                entry);
}

int
bcmltm_entry_delete(bcmltm_entry_t *entry)
{
    return bcmltm_entry_process(BCMLT_OPCODE_DELETE,
                                entry);
}

int
bcmltm_entry_update(bcmltm_entry_t *entry)
{
    return bcmltm_entry_process(BCMLT_OPCODE_UPDATE,
                                entry);
}

int
bcmltm_entry_traverse_first(bcmltm_entry_t *entry)
{
    if (entry->in_fields != NULL) {
        return SHR_E_PARAM;
    }

    return bcmltm_entry_process(BCMLT_OPCODE_TRAVERSE,
                                entry);
}

int
bcmltm_entry_traverse_next(bcmltm_entry_t *entry)
{
    if (entry->in_fields == NULL) {
        int rv;
        bcmltm_lt_md_t *lt_md = NULL;
        /* Dynamic init metadata */
        rv = bcmltm_md_lt_retrieve(entry->unit,
                                   BCMLTM_TABLE_CATG_LOGICAL,
                                   entry->table_id, &lt_md);
        if (SHR_FAILURE(rv)) {
            return rv;
        }

        if (lt_md->params->lt_flags & BCMLTM_LT_FLAGS_NO_KEY) {
            /* No key LTs have only one entry */
            return SHR_E_NOT_FOUND;
        } else {
            /* Should have the previous key fields input */
            return SHR_E_PARAM;
        }
    }

    return bcmltm_entry_process(BCMLT_OPCODE_TRAVERSE,
                                entry);
}

/*****
 *
 * PT PassThru TRM interface functions
 */

int
bcmltm_entry_pt_fifo_pop(bcmltm_entry_t *entry)
{
    return bcmltm_entry_pt_process(BCMLT_PT_OPCODE_FIFO_POP,
                                   entry);
}

int
bcmltm_entry_pt_fifo_push(bcmltm_entry_t *entry)
{
    return bcmltm_entry_pt_process(BCMLT_PT_OPCODE_FIFO_PUSH,
                                   entry);
}

int
bcmltm_entry_pt_set(bcmltm_entry_t *entry)
{
    return bcmltm_entry_pt_process(BCMLT_PT_OPCODE_SET,
                                   entry);
}

int
bcmltm_entry_pt_modify(bcmltm_entry_t *entry)
{
    return bcmltm_entry_pt_process(BCMLT_PT_OPCODE_MODIFY,
                                   entry);
}

int
bcmltm_entry_pt_get(bcmltm_entry_t *entry)
{
    return bcmltm_entry_pt_process(BCMLT_PT_OPCODE_GET,
                                   entry);
}

int
bcmltm_entry_pt_clear(bcmltm_entry_t *entry)
{
    return bcmltm_entry_pt_process(BCMLT_PT_OPCODE_CLEAR,
                                   entry);
}

int
bcmltm_entry_pt_insert(bcmltm_entry_t *entry)
{
    return bcmltm_entry_pt_process(BCMLT_PT_OPCODE_INSERT,
                                   entry);
}

int
bcmltm_entry_pt_delete(bcmltm_entry_t *entry)
{
    return bcmltm_entry_pt_process(BCMLT_PT_OPCODE_DELETE,
                                   entry);
}

int
bcmltm_entry_pt_lookup(bcmltm_entry_t *entry)
{
    return bcmltm_entry_pt_process(BCMLT_PT_OPCODE_LOOKUP,
                                   entry);
}

int
bcmltm_commit(int unit,
              uint32_t trans_id,
              bcmptm_trans_cb_f notif_fn,
              void *context)
{
    bcmltm_transaction_status_t *trans_status;
    uint32_t *ltid_list;
    bcmltm_lt_md_t *lt_md = NULL;
    uint32_t ltid, ltix;
    uint32_t rsp_flags;
    const bcmltd_table_handler_t *cth_handler = NULL;

    SHR_FUNC_ENTER(unit);

    if ((bcmltm_trans_status[unit] != NULL) &&
        (trans_id == bcmltm_trans_status[unit]->trans_id)) {
        trans_status = bcmltm_trans_status[unit];
        /* In process transaction matches transaction ID, clean up state */
        ltid_list =
            bcmltm_ha_ptr_to_mem_ptr(unit, trans_status->ltid_list_hap);

        for (ltix = 0; ltix < trans_status->lt_num; ltix++) {
            ltid = ltid_list[ltix];

            /* Fetch metadata - only Logical Tables will commit */
            SHR_IF_ERR_EXIT(bcmltm_md_lt_retrieve(unit,
                                                  BCMLTM_TABLE_CATG_LOGICAL,
                                                  ltid, &lt_md));

            if (lt_md == NULL) {
                const char *table_name =
                    bcmltm_lt_table_sid_to_name(unit, ltid);

                LOG_VERBOSE(BSL_LS_BCMLTM_ENTRY,
                            (BSL_META_U(unit,
                                        "Unrecognized Logical Table "
                                        "%s (ltid=%d trans_id=%d)\n"),
                             table_name, ltid, trans_id));
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
            }

            /* Call Table Commit handler for commit */
            SHR_IF_ERR_VERBOSE_EXIT
                (table_commit_process(unit, trans_id,
                                      lt_md->tc_list,
                                      TABLE_COMMIT_INTF_COMMIT));

            /* If LT is CTH, call handler commit */
            cth_handler = lt_md->cth;
            if (cth_handler != NULL) {
                if (cth_handler->ha != NULL) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (cth_handler->ha->commit(unit, trans_id,
                                                 cth_handler->arg));
                }
            }

            /* Discard rollback LT state */
            BCMLTM_HA_PTR_INVALID_SET(lt_md->params->lt_rollback_state_hap);
        }

        /* Clear transaction record */
        bcmltm_transaction_clear(unit);
    }

    /* Whether we found the record or not, we must pass this
     * notification on to the PTM. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_mreq_indexed_lt(unit,
                                BCMPTM_REQ_FLAGS_COMMIT,
                                0,
                                NULL,
                                NULL,
                                NULL,      /* misc_info */
                                BCMPTM_OP_NOP,
                                NULL,
                                0,
                                0,
                                trans_id,  /* cseq_id */
                                notif_fn,  /* notify_fn_ptr */
                                context,   /* notify info (cookie) */
                                NULL,
                                NULL,
                                &rsp_flags));

exit:

    /*
     * Log verbose message only on SUCCESS.
     * Failure cases should have already logged corresponding error earlier.
     */
    if (!SHR_FUNC_ERR()) {
        LOG_VERBOSE(BSL_LS_BCMLTM_ENTRY,
                    (BSL_META_U(unit,
                                "LTM commit successful trans_id=%d\n"),
                     trans_id));
    }

    SHR_FUNC_EXIT();
}

int
bcmltm_abort(int unit,
             uint32_t trans_id)
{
    bcmltm_transaction_status_t *trans_status;
    uint32_t *ltid_list;
    bcmltm_lt_md_t *lt_md = NULL;
    uint32_t ltid, ltix;
    uint32_t rsp_flags;
    const bcmltd_table_handler_t *cth_handler = NULL;

    SHR_FUNC_ENTER(unit);

    if ((bcmltm_trans_status[unit] != NULL) &&
        (trans_id == bcmltm_trans_status[unit]->trans_id)) {
        trans_status = bcmltm_trans_status[unit];
        /* In process transaction matches transaction ID, clean up state */
        ltid_list =
            bcmltm_ha_ptr_to_mem_ptr(unit, trans_status->ltid_list_hap);

        for (ltix = 0; ltix < trans_status->lt_num; ltix++) {
            ltid = ltid_list[ltix];

            /* Fetch metadata - only Logical Tables will commit */
            SHR_IF_ERR_EXIT(bcmltm_md_lt_retrieve(unit,
                                                  BCMLTM_TABLE_CATG_LOGICAL,
                                                  ltid, &lt_md));

            if (lt_md == NULL) {
                const char *table_name =
                    bcmltm_lt_table_sid_to_name(unit, ltid);

                LOG_VERBOSE(BSL_LS_BCMLTM_ENTRY,
                            (BSL_META_U(unit,
                                        "Unrecognized Logical Table "
                                        "%s (ltid=%d trans_id=%d)\n"),
                             table_name, ltid, trans_id));
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
            }

            /* Call Table Commit handler for abort */
            SHR_IF_ERR_VERBOSE_EXIT
                (table_commit_process(unit, trans_id,
                                      lt_md->tc_list,
                                      TABLE_COMMIT_INTF_ABORT));

            /* If LT is CTH, call handler abort */
            cth_handler = lt_md->cth;
            if (cth_handler != NULL) {
                if (cth_handler->ha != NULL) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (cth_handler->ha->abort(unit, trans_id,
                                                cth_handler->arg));
                }
            }

            if (!BCMLTM_HA_PTR_IS_INVALID(lt_md->params->lt_rollback_state_hap)) {
                /* Copy rollback LT state to working copy */
                SHR_IF_ERR_EXIT
                    (bcmltm_state_copy(unit,
                                       lt_md->params->lt_rollback_state_hap,
                                       lt_md->params->lt_state_hap));

                BCMLTM_HA_PTR_INVALID_SET(lt_md->params->lt_rollback_state_hap);
            }
        }

        /* Clear transaction record */
        bcmltm_transaction_clear(unit);
    }

    /* Whether we found the record or not, we must pass this
     * notification on to the PTM. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_mreq_indexed_lt(unit,
                                BCMPTM_REQ_FLAGS_ABORT,
                                0,
                                NULL,
                                NULL,
                                NULL, /* misc_info */
                                BCMPTM_OP_NOP,
                                NULL,
                                0,
                                0,
                                trans_id, /* cseq_id */
                                NULL, /* notify ptr */
                                NULL, /* notify info (cookie) */
                                NULL,
                                NULL,
                                &rsp_flags));

exit:
    /*
     * Log verbose message only on SUCCESS.
     * Failure cases should have already logged corresponding error earlier.
     */
    if (!SHR_FUNC_ERR()) {
        LOG_VERBOSE(BSL_LS_BCMLTM_ENTRY,
                    (BSL_META_U(unit,
                                "LTM abort successful trans_id=%d\n"),
                     trans_id));
    }

    SHR_FUNC_EXIT();
}

void
bcmltm_stats_increment_lt(int unit,
                          uint32_t ltid,
                          uint32_t stat_field)
{
    int rv;
    bcmltm_lt_md_t *lt_md = NULL;

    /* This is a CTH interface, so it should only be LTs,
     * not PT PassThru. */
    rv = bcmltm_md_lt_retrieve(unit,
                               BCMLTM_TABLE_CATG_LOGICAL,
                               ltid, &lt_md);

    if (SHR_FAILURE(rv) || (lt_md == NULL)) {
        /* No way to update stats */
        return;
    }

    bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md), stat_field);
}

int
bcmltm_inuse_count_get(int unit,
                       uint32_t trans_id,
                       bcmltd_sid_t ltid,
                       uint32_t *inuse_count)
{
    int rv;
    bcmltm_lt_md_t *lt_md = NULL;
    bcmltm_lt_state_t *lt_state = NULL;
    const bcmltd_table_entry_usage_handler_t *table_entry_usage = NULL;

    SHR_FUNC_ENTER(unit);

    rv = bcmltm_md_lt_retrieve(unit,
                               BCMLTM_TABLE_CATG_LOGICAL,
                               ltid, &lt_md);

    if (SHR_FAILURE(rv) || (lt_md == NULL)) {
        /*
         * Unknown or diabled LT.
         * Return 0 without error for other modules.
         */
        *inuse_count = 0;
        return SHR_E_NONE;
    }

    SHR_IF_ERR_EXIT
        (bcmltm_state_lt_get(unit, ltid, &lt_state));
    SHR_NULL_CHECK(lt_state, SHR_E_INTERNAL);

    *inuse_count = lt_state->entry_count;

    SHR_IF_ERR_EXIT
        (bcmltm_entry_usage_get(unit, ltid, &table_entry_usage));
    if ((table_entry_usage != NULL) &&
        (table_entry_usage->entry_inuse_get != NULL)) {
        bcmltd_table_entry_inuse_arg_t table_arg;
        bcmltd_table_entry_inuse_t table_data;

        sal_memset(&table_arg, 0, sizeof(table_arg));
        rv = table_entry_usage->entry_inuse_get(unit,
                                                trans_id,
                                                ltid,
                                                &table_arg,
                                                &table_data,
                                                table_entry_usage->arg);
        if (rv != SHR_E_UNAVAIL) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        } /* Else discard UNAVAIL error */
        *inuse_count = table_data.entry_inuse;
    }
exit:
    SHR_FUNC_EXIT();
}
