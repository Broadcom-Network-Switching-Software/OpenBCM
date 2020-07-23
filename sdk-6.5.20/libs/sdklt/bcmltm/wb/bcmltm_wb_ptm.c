/*! \file bcmltm_wb_ptm.c
 *
 * Logical Table Manager Working Buffer for PTM.
 *
 * This file contains routines to manage the working buffer blocks
 * used for operations that require to assemble data for the PTM
 * interface, such as PTM format entry buffers, PTM memory parameters.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_types.h>
#include <shr/shr_debug.h>

#include <sal/sal_libc.h>

#include <bsl/bsl.h>

#include <bcmlrd/bcmlrd_types.h>

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm_wb_types_internal.h>
#include <bcmltm/bcmltm_wb_internal.h>
#include <bcmltm/bcmltm_wb_ptm_internal.h>
#include <bcmltm/bcmltm_pt.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE       BSL_LS_BCMLTM_METADATA


/* Size (in words) of one single PTM parameter (uint32_t) */
#define  PARAM_WSIZE         1

/* Size (in words) of one PT entry (fixed for all PTs) */
#define  PT_ENTRY_WSIZE      BCMDRD_MAX_PT_WSIZE

/*
 * The PTM entry section for Keyed LT has 3 sets of Key and Data
 * and are used as follows for the different PTM operations:
 *
 *                 LOOKUP/TRAVERSE    INSERT/DELETE
 *    Input Key          K0                 K0
 *    Input Data         D0                 D1
 *    Output Key         K1                 K2
 *    Output Data        D1                 D2
 *
 * The third set of PTM entries K2/D2 (used for INSERT/DELETE),
 * are not relevant to the LTM operation and are not expected to be used
 * (other than for analysis and avoid overwrite of other entries).
 * Similarly, the first set of Data D0 is not relevant
 * since input data is not expected on LOOKUP/TRAVERSE.
 *
 * The PTM opcodes for TRAVERSE is GET_FIRST and GET_NEXT.
 *
 * There is not an explicit PTM UPDATE operation.  Instead,
 * the LT UPDATE results in 2 PTM operations: LOOKUP followed by INSERT.
 *
 * Based on this arrangement, the field offsets for the Keyed PTM
 * entries can be summarized as follows for any LTM operation:
 *    Key Input         -> K0
 *    Key Output        -> K1
 *    Data Input/Output -> D1
 *
 * If the above set assignments are changed, the corresponding
 * routine need to adjust to return the appropriate offset.
 *
 * Each set contains space for BCMLTM_PT_ENTRY_LIST_MAX(4) PT entries.
 */
#define KEY_INPUT_SET    BCMLTM_WB_PTM_KEYED_ENTRY_SET_0
#define KEY_OUTPUT_SET   BCMLTM_WB_PTM_KEYED_ENTRY_SET_1
#define DATA_SET         BCMLTM_WB_PTM_KEYED_ENTRY_SET_1


/* Checks that type is a valid PTM block type */
#define BLOCK_PTM_TYPE_VALID(_type)                        \
    ((_type == BCMLTM_WB_BLOCK_TYPE_PTM_INDEX) ||          \
     (_type == BCMLTM_WB_BLOCK_TYPE_PTM_KEYED_PTHRU) ||    \
     (_type == BCMLTM_WB_BLOCK_TYPE_PTM_KEYED_LT))


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Get working buffer size for the parameter section of 1 PTM operation.
 *
 * This function gets the working buffer size of the PTM parameter
 * section for 1 PTM operation.
 *
 * \retval Size, in words, of PTM parameter section for 1 PTM operation.
 */
static int
param_wsize_get(void)
{
    bcmltm_wb_ptm_param_offsets_t param_offsets;

    return bcmltm_wb_ptm_param_offsets_get(0, &param_offsets);
}


/*!
 * \brief Get working buffer size for the entry section of 1 PTM operation.
 *
 * This function gets the working buffer size of the PTM entry
 * section for 1 PTM operation.  The content of the entry section
 * varies depending on the block type.  Some type contain only
 * a data entry, where as others contain key and data entries.
 *
 * The only value block types are:
 *     BCMLTM_WB_BLOCK_TYPE_PTM_INDEX
 *     BCMLTM_WB_BLOCK_TYPE_PTM_KEYED_PTHRU
 *     BCMLTM_WB_BLOCK_TYPE_PTM_KEYED_LT
 *
 * \param [in] type Working buffer block type BCMLTM_WB_BLOCK_TYPE_PTM_xxx
 *
 * \retval Size, in words, of PTM parameter section for 1 PTM operation.
 */
static int
entry_wsize_get(bcmltm_wb_block_type_t type)
{
    uint32_t wsize = 0;
    bcmltm_wb_ptm_entry_index_offsets_t index;
    bcmltm_wb_ptm_entry_keyed_lt_offsets_t keyed_lt;
    bcmltm_wb_ptm_entry_keyed_pthru_offsets_t keyed_pthru;

    if (type == BCMLTM_WB_BLOCK_TYPE_PTM_INDEX) {
        wsize = bcmltm_wb_ptm_entry_index_offsets_get(0, &index);
    } else if (type == BCMLTM_WB_BLOCK_TYPE_PTM_KEYED_PTHRU) {
        wsize = bcmltm_wb_ptm_entry_keyed_pthru_offsets_get(0, &keyed_pthru);
    } else if (type == BCMLTM_WB_BLOCK_TYPE_PTM_KEYED_LT) {
        wsize = bcmltm_wb_ptm_entry_keyed_lt_offsets_get(0, &keyed_lt);
    }

    return wsize;
}

/*!
 * \brief Get the PTM operation index for given map PT entry index.
 *
 * This function gets the PTM operation index for the
 * field map PT entry index pt_entry_idx in the given
 * PTM working buffer block.
 *
 * A table may require access to more than 1 PT buffer per PT ID.
 * The pt_entry_idx (from the field map) indicates which PT buffer to access.
 *
 * The arrangement of the PT entry buffers are different in Index
 * and Keyed tables as follows:
 * - Index type
 *   Each PT entry has an associated PT operation.
 *   The pt_entry_idx is used to indicate the PT op.
 * - Keyed type
 *   There is only 1 PT operation.
 *   Note: Keyed LTs have a set of 4 PT buffers, but these buffers
 *   are part the single PT op.  In this case, the pt_entry_idx
 *   indicates the index into the array of buffers (4) within the single PT op.
 *
 * \param [in] wb_block Working buffer block.
 * \param [in] pt_entry_idx This is the index of the desired PT entry buffer
 *                          (Note: this is not the HW PT entry INDEX).
 * \param [out] op_idx PTM operation index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pt_op_idx_get(const bcmltm_wb_block_t *wb_block, uint16_t pt_entry_idx,
              uint32_t *op_idx)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if ((wb_block->type == BCMLTM_WB_BLOCK_TYPE_PTM_KEYED_PTHRU) ||
        (wb_block->type == BCMLTM_WB_BLOCK_TYPE_PTM_KEYED_LT)) {
        /*
         * Keyed block
         *
         * Keyed PThru or Keyed LT only has 1 PT op.
         */
        *op_idx = 0;

    } else if (wb_block->type == BCMLTM_WB_BLOCK_TYPE_PTM_INDEX) {
        /*
         * Index block
         *
         * The pt_entry_idx numbering is compact starting at zero.
         * As such, the pt_entry_idx is used as the index into the array
         * of PT ops.
         */
        *op_idx = pt_entry_idx;

    } else {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Invalid buffer block block_type=%d\n"),
                   wb_block->type));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Check valid PTM op index */
    if (*op_idx >= wb_block->u.ptm.num_ops) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Invalid PTM op index: op_idx=%d num_ops=%d\n"),
                   *op_idx, wb_block->u.ptm.num_ops));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Add a PTM block type to the working buffer handle.
 *
 * This function adds a PTM working buffer block required for PTM operation.
 *
 * Valid PTM block types are:
 *     BCMLTM_WB_BLOCK_TYPE_PTM_INDEX
 *     BCMLTM_WB_BLOCK_TYPE_PTM_KEYED_PTHRU
 *     BCMLTM_WB_BLOCK_TYPE_PTM_KEYED_LT
 *
 * \param [in] unit Unit number.
 * \param [in] type Working buffer block type BCMLTM_WB_BLOCK_TYPE_PTM_xxx
 * \param [in] ptid Physical table ID.
 * \param [in] num_ops Number of PTM operations required for this table.
 * \param [in,out] handle Working buffer handle.
 * \param [out] block_id Returning ID for new working buffer block.
 * \param [out] block_ptr Optional return of pointer to working buffer block.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
block_ptm_add(int unit,
              bcmltm_wb_block_type_t type,
              bcmdrd_sid_t ptid, uint32_t num_ops,
              bcmltm_wb_handle_t *handle,
              bcmltm_wb_block_id_t *block_id,
              bcmltm_wb_block_t **block_ptr)
{
    bcmltm_wb_block_t *block = NULL;
    uint32_t param_wsize;
    uint32_t entry_wsize;
    uint32_t op_wsize;
    uint32_t block_wsize;
    uint32_t param_base_offset;
    uint32_t entry_base_offset;

    SHR_FUNC_ENTER(unit);

    /*
     * Working Buffer PTM Block Layout
     *
     *    +----------------------+
     *    | Param  op_0          |
     *    | Entry  op_0          |
     *    |----------------------|
     *    | Param  op_1          |
     *    | Entry  op_1          |
     *    |----------------------|
     *    | ....                 |
     *    |----------------------|
     *    | Param  op_<n-1>      |
     *    | Entry  op_<n-1>      |
     *    +----------------------+
     *
     * Where "n" is the number of PTM operations.
     */

    /* Check for handle */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_handle_validate(handle));

    /* Check for valid block type */
    if (!BLOCK_PTM_TYPE_VALID(type)) {
        const char *table_name;
        const char *table_sid_type;
        const char *pt_name;

        table_name = bcmltm_table_sid_to_name(unit,
                                              handle->sid, handle->logical);
        table_sid_type = bcmltm_table_sid_type_str(handle->logical);
        pt_name = bcmltm_pt_table_sid_to_name(unit, ptid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM invalid working buffer block type: "
                              "%s(%s=%d) %s(ptid=%d) block_type=%d\n"),
                   table_name, table_sid_type, handle->sid,
                   pt_name, ptid, type));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Sections sizes */
    param_wsize = param_wsize_get();
    entry_wsize = entry_wsize_get(type);
    op_wsize = param_wsize + entry_wsize;
    block_wsize = op_wsize * num_ops;

    /* Add block */
    block = bcmltm_wb_block_add(block_wsize, type, handle);
    if (block == NULL) {
        const char *table_name;
        const char *table_sid_type;
        const char *pt_name;

        table_name = bcmltm_table_sid_to_name(unit,
                                              handle->sid, handle->logical);
        table_sid_type = bcmltm_table_sid_type_str(handle->logical);
        pt_name = bcmltm_pt_table_sid_to_name(unit, ptid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM could not allocate PTM "
                              "working buffer block: "
                              "%s(%s=%d) %s(ptid=%d)\n"),
                   table_name, table_sid_type, handle->sid,
                   pt_name, ptid));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Base offsets for sections */
    param_base_offset = block->base_offset;
    entry_base_offset = param_base_offset + param_wsize;

    /* Fill PTM block information */
    block->u.ptm.ptid = ptid;
    block->u.ptm.num_ops = num_ops;
    block->u.ptm.param_wsize = param_wsize;
    block->u.ptm.entry_wsize = entry_wsize;
    block->u.ptm.op_wsize = op_wsize;
    block->u.ptm.offsets.param = param_base_offset;
    block->u.ptm.offsets.entry = entry_base_offset;

    *block_id = block->id;
    if (block_ptr != NULL) {
        *block_ptr = block;
    }


 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the PTM operation flags BCMLTM_PT_OP_FLAGS_ for given PT.
 *
 * This routine returns the PTM operation flags BCMLTM_PT_OP_FLAGS_
 * corresponding to given PT.  These flags indicate the
 * parameters and entries required for PTM operations.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol (Physical Table) ID.
 * \param [in] pthru Indicates if this is for PT Pass Thru usage.
 *                   If TRUE, this is for a PT Pass Thru case.
 *                   If FALSE, this is for a LT case.
 *
 * \retval Working Buffer flags BCMLTM_PT_OP_FLAGS_
 */
static uint32_t
ptm_op_flags_get(int unit, bcmdrd_sid_t sid, bool pthru)
{
    uint32_t flags = 0;

    /*
     * Parameters Section
     */

    /*
     * Parameter Index:  Memory or register array
     *
     * - PT PassThru: PT INDEX is always accepted and passed to PTM.
     * - LT         : PT INDEX is only present if this is applicable
     *                to the PT (the index to the HW memory or register
     *                can be non-zero).
     */
    if (pthru) {
        /* PT Pass Thru */
        flags |= BCMLTM_PT_OP_FLAGS_INDEX;
    } else {
        /* LT */
        if (bcmdrd_pt_index_max(unit, sid) > 0) {
            flags |= BCMLTM_PT_OP_FLAGS_INDEX;
        }
    }

    /* Parameter Port:  Port based register */
    if (bcmdrd_pt_is_port_reg(unit, sid) ||
        bcmdrd_pt_is_soft_port_reg(unit, sid)) {
        flags |= BCMLTM_PT_OP_FLAGS_PORT;
    }

    /*
     * Parameter Table Instance:
     * Symbols with number of table instance > 1,
     * and, not a port-based register.
     */
    if (!(flags & BCMLTM_PT_OP_FLAGS_PORT) &&
        (bcmdrd_pt_num_tbl_inst(unit, sid) > 1)) {
        flags |= BCMLTM_PT_OP_FLAGS_TABLE_INST;
    }

    /* Parameter Priority: TCAM */
    if (bcmdrd_pt_attr_is_cam(unit, sid)) {
        flags |= BCMLTM_PT_OP_FLAGS_PRIORITY;
    }

    /*
     * Entry Data Section
     */
    /* Entry Data Key:  TCAM or HASH */
    if (bcmdrd_pt_attr_is_hashed(unit, sid) ||
        bcmdrd_pt_attr_is_cam(unit, sid)) {
        flags |= BCMLTM_PT_OP_FLAGS_KEY;
        flags |= BCMLTM_PT_OP_FLAGS_DATA;
    }

    return flags;
}

/*!
 * \brief Get working buffer size in words for a PT Pass Thru table type.
 *
 * This function returns the working buffer size, in words,
 * for a PT Pass Thru table with the specified block type and
 * number of PTM operations.
 *
 * \param [in] unit Unit number.
 * \param [in] type PT Pass Thru block type.
 * \param [in] num_ops Number of PTM operations.
 *
 * \retval Working buffer size in words.
 */
static uint32_t
ptm_pthru_wsize_get(int unit,
                    bcmltm_wb_block_type_t type,
                    uint32_t num_ops)
{
    int rv;
    bcmltm_wb_handle_t handle;
    bcmltm_wb_block_id_t block_id;
    bcmdrd_sid_t sid = 0;
    uint32_t wb_wsize = 0;

    rv = bcmltm_wb_handle_init(sid, FALSE, &handle);
    if (SHR_FAILURE(rv)) {
        return 0;
    }

    rv = block_ptm_add(unit, type,
                       sid, num_ops, &handle, &block_id, NULL);
    if (SHR_FAILURE(rv)) {
        return 0;
    }

    wb_wsize = bcmltm_wb_wsize_get(&handle);

    bcmltm_wb_handle_cleanup(&handle);

    return wb_wsize;
}

/*******************************************************************************
 * Public functions
 */

uint32_t
bcmltm_wb_ptm_op_flags_pthru_get(int unit, bcmdrd_sid_t sid)
{
    return ptm_op_flags_get(unit, sid, TRUE);
}

uint32_t
bcmltm_wb_ptm_op_flags_lt_get(int unit, bcmdrd_sid_t sid)
{
    return ptm_op_flags_get(unit, sid, FALSE);
}

int
bcmltm_wb_block_ptm_pthru_add(int unit,
                              bcmdrd_sid_t ptid,
                              uint32_t flags, uint32_t num_ops,
                              bcmltm_wb_handle_t *handle,
                              bcmltm_wb_block_id_t *block_id,
                              bcmltm_wb_block_t **block_ptr)
{
    bcmltm_wb_block_type_t block_type;

    if (flags & BCMLTM_PT_OP_FLAGS_KEY) {
        block_type = BCMLTM_WB_BLOCK_TYPE_PTM_KEYED_PTHRU;
    } else {
        block_type = BCMLTM_WB_BLOCK_TYPE_PTM_INDEX;
    }

    return block_ptm_add(unit, block_type,
                         ptid, num_ops, handle, block_id, block_ptr);
}


int
bcmltm_wb_block_ptm_lt_add(int unit,
                           bcmdrd_sid_t ptid,
                           bcmltm_table_type_t table_type,
                           uint32_t num_ops,
                           bcmltm_wb_handle_t *handle,
                           bcmltm_wb_block_id_t *block_id,
                           bcmltm_wb_block_t **block_ptr)
{
    bcmltm_wb_block_type_t block_type;

    if (BCMLTM_TABLE_TYPE_IS_KEYED(table_type)) {
        block_type = BCMLTM_WB_BLOCK_TYPE_PTM_KEYED_LT;
    } else {
        block_type = BCMLTM_WB_BLOCK_TYPE_PTM_INDEX;
    }

    return block_ptm_add(unit, block_type,
                         ptid, num_ops, handle, block_id, block_ptr);
}

uint32_t
bcmltm_wb_ptm_pthru_wsize_max_get(int unit,
                                  uint32_t num_ops)
{
    uint32_t max_wsize = 0;
    uint32_t index_wsize;
    uint32_t keyed_wsize;

    index_wsize = ptm_pthru_wsize_get(unit,
                                      BCMLTM_WB_BLOCK_TYPE_PTM_INDEX,
                                      num_ops);
    max_wsize = index_wsize;

    keyed_wsize = ptm_pthru_wsize_get(unit,
                                      BCMLTM_WB_BLOCK_TYPE_PTM_KEYED_PTHRU,
                                      num_ops);
    if (keyed_wsize > max_wsize) {
        max_wsize = keyed_wsize;
    }

    return max_wsize;
}

uint32_t
bcmltm_wb_ptm_pt_entry_wsize(void)
{
    return PT_ENTRY_WSIZE;
}


int
bcmltm_wb_ptm_param_base_offset_get(const bcmltm_wb_block_t *wb_block,
                                    uint32_t op_idx,
                                    uint32_t *offset)
{
    uint32_t op_wsize;

    /* Check for valid PTM block type */
    if (!BLOCK_PTM_TYPE_VALID(wb_block->type)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("LTM invalid working buffer PTM block type: "
                            "type=%d\n"),
                   wb_block->type));
        return SHR_E_INTERNAL;
    }

    /* Check valid PTM op index */
    if (op_idx >= wb_block->u.ptm.num_ops) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("LTM invalid PTM op index: idx=%d num_ops=%d\n"),
                   op_idx, wb_block->u.ptm.num_ops));
        return SHR_E_INTERNAL;
    }

    op_wsize = wb_block->u.ptm.op_wsize;
    *offset = wb_block->u.ptm.offsets.param + (op_idx * op_wsize);

    return SHR_E_NONE;
}


int
bcmltm_wb_ptm_entry_base_offset_get(const bcmltm_wb_block_t *wb_block,
                                    uint32_t op_idx,
                                    uint32_t *offset)
{
    uint32_t op_wsize;

    /* Check for valid PTM block type */
    if (!BLOCK_PTM_TYPE_VALID(wb_block->type)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("LTM invalid working buffer PTM block type: "
                            "type=%d\n"),
                   wb_block->type));
        return SHR_E_INTERNAL;
    }

    /* Check valid PTM op index */
    if (op_idx >= wb_block->u.ptm.num_ops) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("LTM invalid PTM op index: idx=%d num_ops=%d\n"),
                   op_idx, wb_block->u.ptm.num_ops));
        return SHR_E_INTERNAL;
    }

    op_wsize = wb_block->u.ptm.op_wsize;
    *offset = wb_block->u.ptm.offsets.entry + (op_idx * op_wsize);

    return SHR_E_NONE;
}


uint32_t
bcmltm_wb_ptm_param_offsets_get(uint32_t base_offset,
                                bcmltm_wb_ptm_param_offsets_t *offsets)
{
    uint32_t wsize = 0;

    /*
     * WB PTM parameter layout (for 1 PT operation)
     *
     * Memory is allocated for all possible PTM parameters.
     * Each parameter size is 1 word, defined as PARAM_WSIZE.
     *
     * Parameters:
     *     +-------------------------+
     *     | uint32_t index          |
     *     | uint32_t subentry       |
     *     | uint32_t port           |
     *     | uint32_t table_inst     |
     *     | ... other parameters... |
     *     +-------------------------+
     */

    sal_memset(offsets, 0, sizeof(*offsets));

    offsets->index = base_offset;
    offsets->subentry = offsets->index + PARAM_WSIZE;
    offsets->port = offsets->subentry + PARAM_WSIZE;
    offsets->table_inst = offsets->port + PARAM_WSIZE;
    offsets->priority = offsets->table_inst + PARAM_WSIZE;
    offsets->bank = offsets->priority + PARAM_WSIZE;
    offsets->acctype = offsets->bank + PARAM_WSIZE;
    offsets->pt_sid_override = offsets->acctype + PARAM_WSIZE;
    offsets->pt_sid_selector = offsets->pt_sid_override + PARAM_WSIZE;
    offsets->pt_changed = offsets->pt_sid_selector + PARAM_WSIZE;
    offsets->pdd_ldtype[0] = offsets->pt_changed + PARAM_WSIZE;
    offsets->pdd_ldtype[1] = offsets->pdd_ldtype[0] + PARAM_WSIZE;
    offsets->pdd_ldtype[2] = offsets->pdd_ldtype[1] + PARAM_WSIZE;
    offsets->pdd_ldtype[3] = offsets->pdd_ldtype[2] + PARAM_WSIZE;

    wsize = offsets->pdd_ldtype[3] + PARAM_WSIZE - base_offset;

    return wsize;
}


uint32_t
bcmltm_wb_ptm_entry_index_offsets_get(uint32_t base_offset,
                                      bcmltm_wb_ptm_entry_index_offsets_t
                                      *offsets)
{
    uint32_t wsize = 0;

    /*
     * WB for PTM entry for Index PT
     *
     * Memory is allocated for 1 PT entry.
     *
     * Entry:
     *     +------------------------------+
     *     | Data                         |
     *     +------------------------------+
     *
     * Currently, the size of a PT entry is fixed and set
     * to the maximum physical PT entry size.
     */

    sal_memset(offsets, 0, sizeof(*offsets));

    offsets->data = base_offset;

    wsize = PT_ENTRY_WSIZE;

    return wsize;
}


uint32_t
bcmltm_wb_ptm_entry_keyed_pthru_offsets_get(uint32_t base_offset,
                          bcmltm_wb_ptm_entry_keyed_pthru_offsets_t *offsets)
{
    uint32_t wsize = 0;
    uint32_t pt_entry_wsize = PT_ENTRY_WSIZE;  /* Same for Key and Data */

    /*
     * WB for PTM entry for Keyed PT Pass Thru
     *
     * PT Pass Thru only requires 1 Key and 1 Data entry buffer.
     * Each Key/Data entry buffer only needs 1 PTM entry.
     *
     * Entry:
     *     +------------------------------+
     *     | Key                          |
     *     | Data                         |
     *     +------------------------------+
     *
     * Currently, the size of a PTM entry is fixed and set
     * to the maximum physical PT entry size.
     */

    sal_memset(offsets, 0, sizeof(*offsets));

    offsets->key = base_offset;
    offsets->data = offsets->key + pt_entry_wsize;

    wsize = offsets->data + pt_entry_wsize - base_offset;

    return wsize;
}


uint32_t
bcmltm_wb_ptm_entry_keyed_lt_offsets_get(uint32_t base_offset,
                          bcmltm_wb_ptm_entry_keyed_lt_offsets_t *offsets)
{
    uint32_t wsize = 0;
    uint32_t pt_entry_wsize = PT_ENTRY_WSIZE;  /* Same for Key and Data */
    uint32_t entry_offset;
    uint32_t set_idx;
    uint32_t pt_idx;

    /*
     * WB for PTM entry for Keyed LT
     *
     * A Keyed LT requires 3 sets of Key and Data entry buffers.
     * Each Key and Data entry buffer set have space for 4 PTM entries.
     *
     * Entry:
     *     +------------------------------+
     *     | Key  Set 0: PT entries 0..3  |
     *     | Data Set 0: PT entries 0..3  |
     *     |------------------------------|
     *     | Key  Set 1: PT entries 0..3  |
     *     | Data Set 1: PT entries 0..3  |
     *     |------------------------------|
     *     | Key  Set 2: PT entries 0..3  |
     *     | Data Set 2: PT entries 0..3  |
     *     +------------------------------+
     *
     * Currently, the size of one PTM entry is fixed and set
     * to the maximum physical PT entry size.
     */

    sal_memset(offsets, 0, sizeof(*offsets));

    entry_offset = base_offset;
    for (set_idx = 0; set_idx < BCMLTM_WB_PTM_KEYED_ENTRY_SET_NUM; set_idx++) {
        for (pt_idx = 0; pt_idx < BCMLTM_PT_ENTRY_LIST_MAX; pt_idx++) {
            offsets->key[set_idx][pt_idx] = entry_offset;
            entry_offset += pt_entry_wsize;
        }
        for (pt_idx = 0; pt_idx < BCMLTM_PT_ENTRY_LIST_MAX; pt_idx++) {
            offsets->data[set_idx][pt_idx] = entry_offset;
            entry_offset += pt_entry_wsize;
        }
    }

    wsize = entry_offset - base_offset;

    return wsize;
}


int
bcmltm_wb_ptm_field_pthru_offset_get(const bcmltm_wb_block_t *wb_block,
                                     bcmdrd_fid_t fid,
                                     uint32_t *offset)
{
    uint32_t op_idx = 0;      /* For now, assume pt op is 0 */
    uint32_t param_base;
    uint32_t entry_base;
    bcmltm_wb_ptm_param_offsets_t param;
    bcmltm_wb_ptm_entry_index_offsets_t entry_index;
    bcmltm_wb_ptm_entry_keyed_pthru_offsets_t entry_keyed;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Determine Parameter or Entry section of Working Buffer */
    if ((fid == BCMLTM_PT_FIELD_KEY_INDEX) ||
        (fid == BCMLTM_PT_FIELD_KEY_PORT) ||
        (fid == BCMLTM_PT_FIELD_KEY_INSTANCE)) {
        /* Parameter section of Working Buffer */
        SHR_IF_ERR_EXIT
            (bcmltm_wb_ptm_param_base_offset_get(wb_block, op_idx,
                                                 &param_base));
        (void)bcmltm_wb_ptm_param_offsets_get(param_base,
                                              &param);
        if (fid == BCMLTM_PT_FIELD_KEY_INDEX) {
            *offset = param.index;
        } else if (fid == BCMLTM_PT_FIELD_KEY_PORT) {
            *offset = param.port;
        } else if (fid == BCMLTM_PT_FIELD_KEY_INSTANCE) {
            *offset = param.table_inst;
        }

    } else {
        /* Entry section of Working Buffer */
        SHR_IF_ERR_EXIT
            (bcmltm_wb_ptm_entry_base_offset_get(wb_block, op_idx,
                                                 &entry_base));

        /* Index or Keyed PT */
        if (wb_block->type == BCMLTM_WB_BLOCK_TYPE_PTM_INDEX) {
            /* Index PT */
            (void)bcmltm_wb_ptm_entry_index_offsets_get(entry_base,
                                                        &entry_index);
            *offset = entry_index.data;
        } else {
            /* Keyed PT */
            
            (void)bcmltm_wb_ptm_entry_keyed_pthru_offsets_get(entry_base,
                                                              &entry_keyed);
            *offset = entry_keyed.key;
        }
    }

    /* Sanity check */
    if (*offset == BCMLTM_WB_OFFSET_INVALID) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("LTM invalid working buffer offset of zero: "
                            "block_type=%d ptid=%d fid=%d\n"),
                   wb_block->type, wb_block->u.ptm.ptid, fid));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_wb_ptm_field_lt_offset_get(const bcmltm_wb_block_t *wb_block,
                                  uint32_t fid,
                                  bcmltm_field_type_t field_type,
                                  bcmltm_field_dir_t field_dir,
                                  uint16_t pt_entry_idx,
                                  uint32_t *offset)
{
    uint32_t op_idx = 0;
    uint32_t param_base;
    uint32_t entry_base;
    bcmltm_wb_ptm_param_offsets_t param;
    bcmltm_wb_ptm_entry_index_offsets_t entry_index;
    bcmltm_wb_ptm_entry_keyed_lt_offsets_t entry_keyed;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Sanity check */
    if (fid == BCMLRD_FIELD_TRACK_INDEX) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("LTM invalid field TRACK_INDEX for PTM block: "
                            "block_type=%d ptid=%d fid=%d\n"),
                   wb_block->type, wb_block->u.ptm.ptid, fid));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Validate PT entry index on Keyed LT */
    if (wb_block->type == BCMLTM_WB_BLOCK_TYPE_PTM_KEYED_LT) {
        if (pt_entry_idx >= BCMLTM_PT_ENTRY_LIST_MAX) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("LTM invalid Keyed PT entry index: "
                                "block_type=%d ptid=%d fid=%d pt_idx=%d\n"),
                       wb_block->type, wb_block->u.ptm.ptid, fid,
                       pt_entry_idx));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    /* Get PT op index */
    SHR_IF_ERR_EXIT
        (pt_op_idx_get(wb_block, pt_entry_idx, &op_idx));

    /* Determine Parameter or Entry section of Working Buffer */
    if ((fid == BCMLRD_FIELD_INDEX) ||
        (fid == BCMLRD_FIELD_PORT) ||
        (fid == BCMLRD_FIELD_ENTRY_PRIORITY) ||
        (fid == BCMLRD_FIELD_INSTANCE) ||
        (fid == BCMLRD_FIELD_TABLE_SEL) ||
        (fid == BCMLRD_FIELD_SELECTOR) ||
        (fid == BCMLRD_FIELD_LDTYPE)) {
        /* Parameter section of Working Buffer */
        SHR_IF_ERR_EXIT
            (bcmltm_wb_ptm_param_base_offset_get(wb_block, op_idx,
                                                 &param_base));
        (void)bcmltm_wb_ptm_param_offsets_get(param_base, &param);

        if (fid == BCMLRD_FIELD_INDEX) {
            *offset = param.index;
        } else if (fid == BCMLRD_FIELD_PORT) {
            *offset = param.port;
        } else if (fid == BCMLRD_FIELD_ENTRY_PRIORITY) {
            *offset = param.priority;
        } else if (fid == BCMLRD_FIELD_INSTANCE) {
            *offset = param.table_inst;
        } else if (fid == BCMLRD_FIELD_TABLE_SEL) {
            *offset = param.pt_sid_selector;
        } else if (fid == BCMLRD_FIELD_SELECTOR) {
            *offset = param.subentry;
        } else if (fid == BCMLRD_FIELD_LDTYPE) {
            *offset = param.pdd_ldtype[pt_entry_idx];
        }

    } else {
        /* Entry section of Working Buffer */
        SHR_IF_ERR_EXIT
            (bcmltm_wb_ptm_entry_base_offset_get(wb_block, op_idx,
                                                 &entry_base));

        /* Index or Keyed */
        if (wb_block->type == BCMLTM_WB_BLOCK_TYPE_PTM_INDEX) {
            /* Index */
            (void)bcmltm_wb_ptm_entry_index_offsets_get(entry_base,
                                                        &entry_index);
            *offset = entry_index.data;
        } else {
            /* Keyed */

            /* Get base offset */
            (void)bcmltm_wb_ptm_entry_keyed_lt_offsets_get(entry_base,
                                                           &entry_keyed);
            /*
             * For Keyed tables, determine whether field is part
             * of the Key or the Data entry.
             */
            if (field_type == BCMLTM_FIELD_TYPE_KEY) {
                if (field_dir == BCMLTM_FIELD_DIR_IN) {
                    *offset = entry_keyed.key[KEY_INPUT_SET][pt_entry_idx];
                } else {
                    *offset = entry_keyed.key[KEY_OUTPUT_SET][pt_entry_idx];
                }
            } else {
                *offset = entry_keyed.data[DATA_SET][pt_entry_idx];
            }
        }
    }

    /* Sanity check */
    if (*offset == BCMLTM_WB_OFFSET_INVALID) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("LTM invalid working buffer offset of zero: "
                            "block_type=%d ptid=%d fid=%d\n"),
                   wb_block->type, wb_block->u.ptm.ptid, fid));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_wb_ptm_pt_sid_override_offset_get(const bcmltm_wb_block_t *wb_block,
                                         uint16_t pt_entry_idx,
                                         uint32_t *offset)
{
    uint32_t op_idx = 0;
    uint32_t param_base;
    bcmltm_wb_ptm_param_offsets_t param;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Get PT op index */
    SHR_IF_ERR_EXIT
        (pt_op_idx_get(wb_block, pt_entry_idx, &op_idx));

    /* Parameter section of Working Buffer */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_ptm_param_base_offset_get(wb_block, op_idx,
                                             &param_base));
    (void)bcmltm_wb_ptm_param_offsets_get(param_base, &param);

    *offset = param.pt_sid_override;

    /* Sanity check */
    if (*offset == BCMLTM_WB_OFFSET_INVALID) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("LTM invalid working buffer offset of zero "
                            "for PT SID override: block_type=%d\n"),
                   wb_block->type));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_wb_ptm_pt_changed_offset_get(bcmltm_table_type_t table_type,
                                    bcmltm_field_type_t field_type,
                                    const bcmltm_wb_block_t *wb_block,
                                    uint16_t pt_entry_idx,
                                    uint32_t *offset)
{
    uint32_t op_idx = 0;
    uint32_t param_base;
    bcmltm_wb_ptm_param_offsets_t param;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /*
     * The PT changed status flag is used to indicate if there is a
     * change in the PT entry.  This allows LTM to skip
     * unnecessary PTM write ops if there is no change in HW.
     *
     * PT changed is currently valid only on value fields of Index LTs.
     * Value fields on Index LTs are normally placed in the
     * PT entry buffer which can modify the HW.
     * Key fields on Index LTs, however, do not trigger a PT changed
     * since they correspond to PT key parameters (i.e. index,
     * port, instance, etc.) and they do not map to the HW entry.
 */

    /*
     * Set offset to 'invalid' to indicate PT changed status is not
     * applicable.
     */
    if (!bcmltm_wb_ptm_pt_changed_flag_valid(table_type) ||
        (field_type == BCMLTM_FIELD_TYPE_KEY)) {
        *offset = BCMLTM_WB_OFFSET_INVALID;
        SHR_EXIT();
    }

    /* Get PT op index */
    SHR_IF_ERR_EXIT
        (pt_op_idx_get(wb_block, pt_entry_idx, &op_idx));

    /* Parameter section of Working Buffer */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_ptm_param_base_offset_get(wb_block, op_idx,
                                             &param_base));
    (void)bcmltm_wb_ptm_param_offsets_get(param_base, &param);

    *offset = param.pt_changed;

    /* Sanity check */
    if (*offset == BCMLTM_WB_OFFSET_INVALID) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("LTM invalid working buffer offset of zero "
                            "for PT changed status: block_type=%d\n"),
                   wb_block->type));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FUNC_EXIT();
}


bool
bcmltm_wb_ptm_pt_changed_flag_valid(bcmltm_table_type_t table_type)
{
    bool pt_changed_flag = FALSE;

    /*
     * The PT changed status flag is used to indicate if there is a
     * change in the PT entry.  This allows LTM to skip
     * unnecessary PTM write ops if there is no change in HW.
     *
     * The PT change flag is currently only applicable for Index LTs.
     * LTM does not apply this on Keyed LTs since operations for
     * Keyed LTs are primarily managed by the PTM resource managers.
     */
    if (BCMLTM_TABLE_TYPE_IS_INDEX(table_type)) {
        pt_changed_flag = TRUE;
    }

    return pt_changed_flag;
}


int
bcmltm_wb_ptm_entry_keyed_idx_get(bcmptm_op_type_t ptm_op,
                                  bcmltm_field_type_t field_type,
                                  bcmltm_field_dir_t field_dir,
                                  uint32_t *set_idx)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /*
     * Set is assigned based on PTM operation and field information,
     * such as Key or Data, Input or Output.
     *
     * For details on the sets assignment, refer to the description
     * for the PTM entry section for Keyed tables.
     */
    switch (ptm_op) {
    case BCMPTM_OP_INSERT:
    case BCMPTM_OP_DELETE:
        if (field_type == BCMLTM_FIELD_TYPE_KEY) {  /* Key */
            if (field_dir == BCMLTM_FIELD_DIR_IN) {
                *set_idx = BCMLTM_WB_PTM_KEYED_ENTRY_SET_0;
            } else {
                *set_idx = BCMLTM_WB_PTM_KEYED_ENTRY_SET_2;
            }
        } else {  /* Data */
            if (field_dir == BCMLTM_FIELD_DIR_IN) {
                *set_idx = BCMLTM_WB_PTM_KEYED_ENTRY_SET_1;
            } else {
                *set_idx = BCMLTM_WB_PTM_KEYED_ENTRY_SET_2;
            }
        }
        SHR_EXIT();
        break;

    case BCMPTM_OP_LOOKUP:
    case BCMPTM_OP_GET_NEXT:
    case BCMPTM_OP_GET_FIRST:
        /* Key or Data */
        if (field_dir == BCMLTM_FIELD_DIR_IN) {
            *set_idx = BCMLTM_WB_PTM_KEYED_ENTRY_SET_0;
        } else {
            *set_idx = BCMLTM_WB_PTM_KEYED_ENTRY_SET_1;
        }
        SHR_EXIT();
        break;

    default:
        SHR_ERR_EXIT(SHR_E_INTERNAL);
        break;
    }

 exit:
    SHR_FUNC_EXIT();
}

