/*! \file bcmltm_db_dm.c
 *
 * Logical Table Manager - Information for Direct Mapped Tables.
 *
 * This file contains routines to construct information for
 * direct map logical tables.
 *
 * This information is derived from the LRD Physical (direct map)
 * map groups.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_types.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>
#include <sal/sal_libc.h>
#include <bsl/bsl.h>

#include <bcmdrd/bcmdrd_types.h>

#include <bcmltd/bcmltd_types.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_map.h>

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_types_internal.h>
#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_wb_lt_internal.h>
#include <bcmltm/bcmltm_wb_lt_pvt_internal.h>
#include <bcmltm/bcmltm_wb_ptm_internal.h>
#include <bcmltm/bcmltm_db_core_internal.h>

#include "bcmltm_db_md_util.h"
#include "bcmltm_db_dm_util.h"
#include "bcmltm_db_dm_track.h"
#include "bcmltm_db_dm_fs.h"
#include "bcmltm_db_dm.h"
#include "bcmltm_db_read_only_field.h"
#include "bcmltm_db_wide_field.h"


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA


/* PT Key Index and Memory Parameter WBC min and max bit positions  */
#define PT_KEY_MINBIT        0
#define PT_KEY_MAXBIT       31

/*! Get mask for specified number of bits. */
#define BIT_MASK(_bits)    ((1 << (_bits)) - 1)

/*!
 * \brief Keys allocation variants.
 *
 * This information is used for selecting a subset of fields
 * and building the desired PTM field list.
 */
typedef enum keys_alloc_e {
    /*!
     * No allocation involved, use all fields as listed in map.
     */
    KEYS_ALLOC_NONE,

    /*!
     * Select only keys fields that are required during an INSERT
     * operation on Index with Allocation LTs.
     */
    KEYS_ALLOC_REQUIRED,

    /*!
     * Select only keys fields that are allocatable (optional) during an INSERT
     * operation on Index with Allocation LTs.  If keys are not present,
     * they are allocated during the operation.
     */
    KEYS_ALLOC_OPTIONAL
} keys_alloc_t;

/*!
 * \brief Track Index Field Types.
 *
 * This type indicates the track index field type.
 */
typedef enum track_index_field_type_e {
    /*! Track index field is PT Index. */
    TRACK_INDEX_FIELD_PT_INDEX = 1,

    /*! Track index field is PT Memory Parameter: Port or Table Instance. */
    TRACK_INDEX_FIELD_PT_MEM_PARAM = 2,

    /*! Track index field is PT SID Selector. */
    TRACK_INDEX_FIELD_PT_SID_SEL = 3
} track_index_field_type_t;


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Destroy the given PT param_offsets metadata.
 *
 * Destroy the given PT param_offsets metadata.
 *
 * \param [in] param_offsets Pointer to PT param_offsets metadata to destroy.
 */
static void
pt_param_offsets_destroy(uint32_t *param_offsets)
{
    if (param_offsets == NULL) {
        return;
    }

    SHR_FREE(param_offsets);

    return;
}

/*!
 * \brief Create the PT param_offsets metadata.
 *
 * Create the PT parameter indexes metadata for the given PT view.
 *
 * Assumes arguments are valid (i.e. unit, etc.)
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Table PT information.
 * \param [in] wb_block Working buffer block.
 * \param [out] param_offsets_ptr Returning pointer to PT param_offsets.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pt_param_offsets_create(int unit,
                        const bcmltm_db_pt_t *pt_info,
                        const bcmltm_wb_block_t *wb_block,
                        uint32_t **param_offsets_ptr)
{
    uint32_t *param_offsets = NULL;
    unsigned int size;
    uint32_t op_idx;
    uint32_t num_ops = pt_info->num_ops;

    SHR_FUNC_ENTER(unit);

    if (num_ops > 0) {
        /* Allocate for all PT ops needed for this PT view */
        size = sizeof(uint32_t) * num_ops;
        SHR_ALLOC(param_offsets, size, "bcmltmDmPtParamOffsets");
        SHR_NULL_CHECK(param_offsets, SHR_E_MEMORY);
        sal_memset(param_offsets, 0, size);

        for (op_idx = 0; op_idx < num_ops; op_idx++) {
            SHR_IF_ERR_EXIT
                (bcmltm_wb_ptm_param_base_offset_get(wb_block,
                                                     op_idx,
                                                     &param_offsets[op_idx]));
        }
    }

    *param_offsets_ptr = param_offsets;

exit:
    if (SHR_FUNC_ERR()) {
        pt_param_offsets_destroy(param_offsets);
        *param_offsets_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the given PT buffer offsets metadata.
 *
 * Destroy the given PT buffer offsets metadata.
 *
 * \param [in] buffer_offsets Pointer to PT buffer offsets to destroy
 */
static void
pt_buffer_offsets_destroy(uint32_t *buffer_offsets)
{
    if (buffer_offsets == NULL) {
        return;
    }

    SHR_FREE(buffer_offsets);

    return;
}

/*!
 * \brief Create the PT buffer offsets metadata.
 *
 * Create the PT buffer offsets metadata for the given PT view.
 *
 * Assumes arguments are valid (i.e. unit, etc.)
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Table PT information.
 * \param [in] wb_block Working buffer block.
 * \param [out] buffer_offsets_ptr Returning pointer to PT buffer offsets md.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pt_buffer_offsets_create(int unit,
                         const bcmltm_db_pt_t *pt_info,
                         const bcmltm_wb_block_t *wb_block,
                         uint32_t **buffer_offsets_ptr)
{
    uint32_t *buffer_offsets = NULL;
    unsigned int size;
    uint32_t op_idx;
    uint32_t num_ops = pt_info->num_ops;

    SHR_FUNC_ENTER(unit);

    if (num_ops > 0) {
        /* Allocate for all PT (ops) needed for this PT view */
        size = sizeof(uint32_t) * num_ops;
        SHR_ALLOC(buffer_offsets, size, "bcmltmDmPtBufferOffsets");
        SHR_NULL_CHECK(buffer_offsets, SHR_E_MEMORY);
        sal_memset(buffer_offsets, 0, size);

        for (op_idx = 0; op_idx < num_ops; op_idx++) {
            SHR_IF_ERR_EXIT
                (bcmltm_wb_ptm_entry_base_offset_get(wb_block,
                                                     op_idx,
                                                     &buffer_offsets[op_idx]));
        }
    }

    *buffer_offsets_ptr = buffer_offsets;

exit:
    if (SHR_FUNC_ERR()) {
        pt_buffer_offsets_destroy(buffer_offsets);
        *buffer_offsets_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the given PT op list metadata.
 *
 * Destroy the given PT op list metadata.
 *
 * \param [in] op_list Pointer to PT op list metadata to destroy.
 */
static void
pt_op_list_destroy(bcmltm_pt_op_list_t *op_list)
{
    if (op_list == NULL) {
        return;
    }

    pt_buffer_offsets_destroy(op_list->buffer_offsets);
    pt_param_offsets_destroy(op_list->param_offsets);

    SHR_FREE(op_list);
    return;
}

/*!
 * \brief Create the PT op list metadata.
 *
 * Create the PT op list metadata for the given PT view.
 * This routine populates the required metadata information.
 *
 * Assumes:
 *   - unit, pt_info are valid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] pt_info Table PT information.
 * \param [in] lt_map LT mapping information.
 * \param [out] op_list_ptr Returning pointer to PT op list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pt_op_list_create(int unit,
                  bcmlrd_sid_t sid,
                  const bcmltm_db_pt_t *pt_info,
                  const bcmltm_db_dm_map_entries_t *lt_map,
                  bcmltm_pt_op_list_t **op_list_ptr)
{
    bcmltm_pt_op_list_t *op_list = NULL;
    bcmdrd_sid_t ptid;
    const bcmltm_wb_block_t *wb_block = NULL;

    SHR_FUNC_ENTER(unit);

    ptid = pt_info->sid;

    /* Get working buffer block for PT */
    wb_block = bcmltm_wb_lt_block_get(unit, sid, pt_info->wb_block_id);
    if (wb_block == NULL) {
        const char *table_name;
        const char *pt_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        pt_name = bcmltm_pt_table_sid_to_name(unit, ptid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM could not find working buffer block: "
                              "%s(ltid=%d) %s(ptid=%d) wb_block_id=%d\n"),
                   table_name, sid, pt_name, ptid, pt_info->wb_block_id));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Allocate */
    SHR_ALLOC(op_list, sizeof(*op_list), "bcmltmDmPtOpList");
    SHR_NULL_CHECK(op_list, SHR_E_MEMORY);
    sal_memset(op_list, 0, sizeof(*op_list));

    op_list->flags = pt_info->flags;
    op_list->num_pt_ops = pt_info->num_ops;
    
    op_list->word_size = bcmltm_wb_ptm_pt_entry_wsize();

    SHR_IF_ERR_EXIT
        (pt_param_offsets_create(unit, pt_info, wb_block,
                                 &op_list->param_offsets));
    SHR_IF_ERR_EXIT
        (pt_buffer_offsets_create(unit, pt_info, wb_block,
                                  &op_list->buffer_offsets));

    *op_list_ptr = op_list;

 exit:
    if (SHR_FUNC_ERR()) {
        pt_op_list_destroy(op_list);
        *op_list_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the given list of pointers to PT op list metadata.
 *
 * Destroy the given list of pointers to PT op list metadata.
 *
 * \param [in] num_pt_views Number of entries in list.
 * \param [in] op_list_ptrs List of pointers to PT op list.
 */
static void
pt_op_list_ptrs_destroy(uint32_t num_pt_views,
                        bcmltm_pt_op_list_t **op_list_ptrs)
{
    uint32_t pt_view;

    if (op_list_ptrs == NULL) {
        return;
    }

    for (pt_view = 0; pt_view < num_pt_views; pt_view++) {
        if (op_list_ptrs[pt_view] == NULL) {
            continue;
        }
        pt_op_list_destroy(op_list_ptrs[pt_view]);
    }

    SHR_FREE(op_list_ptrs);
    return;
}

/*!
 * \brief Create the list of pointers to PT op list metadata.
 *
 * Create the list of pointers to PT op list metadata for the given
 * physical mapping information.
 *
 * This routine populates the required metadata information.
 *
 * Assumes:
 *   - unit and list of table IDs are valid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] pt_info Table PT information.
 * \param [in] lt_map LT mapping information.
 * \param [out] op_list_ptrs_ptr Returning pointer to list of PT op list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pt_op_list_ptrs_create(int unit,
                       bcmlrd_sid_t sid,
                       const bcmltm_db_dm_pt_info_t *pt_info,
                       const bcmltm_db_dm_map_entries_t *lt_map,
                       bcmltm_pt_op_list_t ***op_list_ptrs_ptr)
{
    unsigned int size;
    uint32_t pt_view;
    uint32_t num_pt_views = pt_info->num_pts;
    bcmltm_pt_op_list_t **op_list_ptrs = NULL;

    SHR_FUNC_ENTER(unit);

    if (num_pt_views > 0) {
        /* Allocate */
        size = sizeof(bcmltm_pt_op_list_t *) * num_pt_views;
        SHR_ALLOC(op_list_ptrs, size, "bcmltmDmPtOpListPtrs");
        SHR_NULL_CHECK(op_list_ptrs, SHR_E_MEMORY);
        sal_memset(op_list_ptrs, 0, size);

        for (pt_view = 0; pt_view < num_pt_views; pt_view++) {
            pt_op_list_create(unit, sid, &pt_info->pts[pt_view], lt_map,
                              &op_list_ptrs[pt_view]);
        }
    }

    *op_list_ptrs_ptr = op_list_ptrs;

 exit:
    if (SHR_FUNC_ERR()) {
        pt_op_list_ptrs_destroy(num_pt_views, op_list_ptrs);
        *op_list_ptrs_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the given PT dynamic info metadata.
 *
 * Destroy the given PT dynamic info metadata.
 *
 * \param [in] pt_dyn_info Pointer to PT dynamic info to destroy.
 */
static void
pt_dyn_info_destroy(bcmbd_pt_dyn_info_t *pt_dyn_info)
{
    SHR_FREE(pt_dyn_info);
    return;
}

/*!
 * \brief Create the PT dynamic info metadata.
 *
 * Create the PT op list metadata for the given table.
 * This routine populates the required metadata information.
 *
 * Assumes:
 *   - unit is valid.
 *
 * \param [in] unit Unit number.
 * \param [out] pt_dyn_info_ptr Returning pointer to PT dynamic info metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pt_dyn_info_create(int unit, bcmbd_pt_dyn_info_t **pt_dyn_info_ptr)
{
    bcmbd_pt_dyn_info_t *pt_dyn_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(pt_dyn_info, sizeof(*pt_dyn_info), "bcmltmDmPtDynInfo");
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_MEMORY);
    sal_memset(pt_dyn_info, 0, sizeof(*pt_dyn_info));

    /* Fill data */
    pt_dyn_info->index = 0;
    pt_dyn_info->tbl_inst = -1;

    *pt_dyn_info_ptr = pt_dyn_info;

 exit:
    if (SHR_FUNC_ERR()) {
        pt_dyn_info_destroy(pt_dyn_info);
        *pt_dyn_info_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the given PT mem args metadata.
 *
 * Destroy the given PT mem args metadata.
 *
 * \param [in] mem_args Pointer to PT mem args metadata to destroy.
 */
static void
pt_mem_args_destroy(bcmltm_pt_mem_args_t *mem_args)
{
    if (mem_args == NULL) {
        return;
    }

    pt_dyn_info_destroy(mem_args->pt_dyn_info);
    SHR_FREE(mem_args);

    return;
}

/*!
 * \brief Create the PT mem args metadata.
 *
 * Create the PT mem args metadata for the given table.
 * This routine populates the required metadata information.
 *
 * Assumes:
 *   - unit, sid are valid.
 *
 * \param [in] unit Unit number.
 * \param [in] ptid Physical table ID.
 * \param [out] mem_args_ptr Returning pointer to PT mem args metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pt_mem_args_create(int unit, bcmdrd_sid_t ptid,
                   bcmltm_pt_mem_args_t **mem_args_ptr)
{
    bcmltm_pt_mem_args_t *mem_args = NULL;
    uint64_t flags = 0;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(mem_args, sizeof(*mem_args), "bcmltmDmPtMemArgs");
    SHR_NULL_CHECK(mem_args, SHR_E_MEMORY);
    sal_memset(mem_args, 0, sizeof(*mem_args));

    /* Fill data */
    mem_args->flags = flags;
    mem_args->pt = ptid;
    mem_args->pt_static_info = NULL;
    mem_args->pt_dyn_hash_info = NULL;  /* Not used */
    SHR_IF_ERR_EXIT
        (pt_dyn_info_create(unit, &mem_args->pt_dyn_info));

    *mem_args_ptr = mem_args;

 exit:
    if (SHR_FUNC_ERR()) {
        pt_mem_args_destroy(mem_args);
        *mem_args_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the given list of pointers to PT mem args metadata.
 *
 * Destroy the given list of pointers to PT mem args metadata.
 *
 * \param [in] num_pt_views Number of entries in list.
 * \param [in] op_list_ptrs List of pointers to PT mem args list.
 */
static void
pt_mem_args_ptrs_destroy(uint32_t num_pt_views,
                         bcmltm_pt_mem_args_t **mem_args_ptrs)
{
    uint32_t pt_view;

    if (mem_args_ptrs == NULL) {
        return;
    }

    for (pt_view = 0; pt_view < num_pt_views; pt_view++) {
        if (mem_args_ptrs[pt_view] == NULL) {
            continue;
        }
        pt_mem_args_destroy(mem_args_ptrs[pt_view]);
    }

    SHR_FREE(mem_args_ptrs);

    return;
}

/*!
 * \brief Create the list of pointers to PT mem args metadata.
 *
 * Create the list of pointers to PT mem args metadata for the given
 * physical mapping information.
 *
 * This routine populates the required metadata information.
 *
 * Assumes:
 *   - unit and list of table IDs are valid.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Table PT information.
 * \param [out] mem_args_ptrs_ptr Returning pointer to list of PT mem args list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pt_mem_args_ptrs_create(int unit,
                        const bcmltm_db_dm_pt_info_t *pt_info,
                        bcmltm_pt_mem_args_t ***mem_args_ptrs_ptr)
{
    unsigned int size;
    uint32_t pt_view;
    uint32_t num_pt_views = pt_info->num_pts;
    bcmltm_pt_mem_args_t **mem_args_ptrs = NULL;

    SHR_FUNC_ENTER(unit);

    if (num_pt_views > 0) {
        /* Allocate */
        size = sizeof(bcmltm_pt_mem_args_t *) * num_pt_views;
        SHR_ALLOC(mem_args_ptrs, size, "bcmltmDmPtMemArgsPtrs");
        SHR_NULL_CHECK(mem_args_ptrs, SHR_E_MEMORY);
        sal_memset(mem_args_ptrs, 0, size);

        for (pt_view = 0; pt_view < num_pt_views; pt_view++) {
            pt_mem_args_create(unit, pt_info->pts[pt_view].sid,
                               &mem_args_ptrs[pt_view]);
        }
    }

    *mem_args_ptrs_ptr = mem_args_ptrs;

 exit:
    if (SHR_FUNC_ERR()) {
        pt_mem_args_ptrs_destroy(num_pt_views, mem_args_ptrs);
        *mem_args_ptrs_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the given PT list metadata.
 *
 * Destroy the given PT list metadata.
 *
 * \param [in] pt_list List of PT metadata.
 */
static void
pt_list_destroy(bcmltm_pt_list_t *pt_list)
{
    if (pt_list == NULL) {
        return;
    }

    pt_op_list_ptrs_destroy(pt_list->num_pt_view, pt_list->memop_lists);
    pt_mem_args_ptrs_destroy(pt_list->num_pt_view, pt_list->mem_args);

    SHR_FREE(pt_list);

    return;
}

/*!
 * \brief Create the PT list metadata.
 *
 * Create the PT list metadata for the given physical mapping information.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] dm_arg DM arguments.
 * \param [in] lt_map LT mapping information.
 * \param [out] pt_list_ptr Returning pointer PT list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pt_list_create(int unit,
               bcmlrd_sid_t sid,
               const bcmltm_db_dm_arg_t *dm_arg,
               const bcmltm_db_dm_map_entries_t *lt_map,
               bcmltm_pt_list_t **pt_list_ptr)
{
    bcmltm_pt_list_t *pt_list = NULL;
    const bcmltm_db_dm_pt_info_t *pt_info = dm_arg->pt_info;
    uint32_t pt_suppress_offset;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(pt_list, sizeof(*pt_list), "bcmltmDmPtList");
    SHR_NULL_CHECK(pt_list, SHR_E_MEMORY);
    sal_memset(pt_list, 0, sizeof(*pt_list));

    /* Get PT suppress status offset */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_pt_suppress_offset_get(unit,
                                             sid,
                                             dm_arg->pt_suppress,
                                             &pt_suppress_offset));

    /* Fill data */
    pt_list->pt_suppress_offset = pt_suppress_offset;
    pt_list->num_pt_view = pt_info->num_pts;
    SHR_IF_ERR_EXIT
        (pt_mem_args_ptrs_create(unit, pt_info,
                                 &pt_list->mem_args));
    SHR_IF_ERR_EXIT
        (pt_op_list_ptrs_create(unit, sid, pt_info, lt_map,
                                &pt_list->memop_lists));

    *pt_list_ptr = pt_list;

 exit:
    if (SHR_FUNC_ERR()) {
        pt_list_destroy(pt_list);
        *pt_list_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the given PT keyed operations information metadata.
 *
 * Destroy the given PT keyed operations information metadata.
 *
 * \param [in] info PT keyed operations information to destroy.
 */
static void
pt_keyed_op_info_destroy(bcmltm_pt_keyed_op_info_t *info)
{
    if (info == NULL) {
        return;
    }

    pt_mem_args_destroy(info->mem_args);

    SHR_FREE(info);

    return;
}

/*!
 * \brief Create the PT keyed operations information metadata.
 *
 * Create the PT keyed operations information metadata for
 * the given logical table and PTM operation type.
 *
 * This routine populates the required metadata information.
 * This metadata is used for Keyed LTs of type Hash and TCAMs.
 *
 * The PT maps for Keyed LTs should be defined as follows:
 * - Hash                   : 1 PT (which maps to Key and Value fields).
 * - TCAM Aggregate View    : 1 PT (which maps to Key and Value fields).
 * - TCAM Non-Aggregate View: 2 PTs (one maps to Key fields and the second
 *                            maps to Value fields).
 *
 * It is assumed that the FLTG will check to verify that the tables
 * are defined correctly to meet these PT map rules.
 *
 * The PTM interface for Keyed tables expects the Key and Value fields to
 * to be passed in the same PTM operation.  The Key fields are placed in
 * the PT key entry section whereas the Value fields are placed in
 * the PT data entry section.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] ptm_op PTM operation.
 * \param [in] dm_arg DM arguments.
 * \param [in] lt_map LT mapping information.
 * \param [out] info_ptr Returning pointer for PT keyed op info metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pt_keyed_op_info_create(int unit,
                        bcmlrd_sid_t sid,
                        bcmptm_op_type_t ptm_op,
                        const bcmltm_db_dm_arg_t *dm_arg,
                        const bcmltm_db_dm_map_entries_t *lt_map,
                        bcmltm_pt_keyed_op_info_t **info_ptr)
{
    bcmltm_pt_keyed_op_info_t *info = NULL;
    const bcmltm_db_dm_pt_info_t *pt_info = dm_arg->pt_info;
    bcmltm_wb_block_id_t wb_block_id;
    const bcmltm_wb_block_t *wb_block = NULL;
    uint32_t pt_view;
    bcmdrd_sid_t ptid;
    uint32_t pt_wsize;
    uint32_t entry_base_offset;
    bcmltm_wb_ptm_entry_keyed_lt_offsets_t entry_offsets;
    uint32_t pt_idx;
    uint32_t set_ki_idx, set_ko_idx, set_di_idx, set_do_idx;
    uint32_t op_idx = 0; /* Always 0 since keyed tables have only 1 PTM op */
    uint32_t pt_suppress_offset;

    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    if (pt_info->num_pts < 1) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Keyed LT should contain at least 1 PT: "
                              "%s(ltid=%d) num_pt=%d\n"),
                   table_name, sid, pt_info->num_pts));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /*
     * Get working buffer block
     * Keyed LTs only have 1 PTM working buffer block.
     * Use block ID of any view (all views should have same block ID).
     */
    wb_block_id = pt_info->pts[0].wb_block_id;
    wb_block = bcmltm_wb_lt_block_get(unit, sid, wb_block_id);
    if (wb_block == NULL) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM could not find working buffer block: "
                              "%s(ltid=%d) wb_block_id=%d\n"),
                   table_name, sid, wb_block_id));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Allocate */
    SHR_ALLOC(info, sizeof(*info), "bcmltmDmPtKeyedOpInfo");
    SHR_NULL_CHECK(info, SHR_E_MEMORY);
    sal_memset(info, 0, sizeof(*info));

    /* Get PT suppress status offset */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_pt_suppress_offset_get(unit,
                                             sid,
                                             dm_arg->pt_suppress,
                                             &pt_suppress_offset));

    /* Fill data */
    info->pt_suppress_offset = pt_suppress_offset;

    /* Assign PT key and data entry sizes to allocated PT entry size in WB */
    pt_wsize = bcmltm_wb_ptm_pt_entry_wsize();
    info->key_word_size  = pt_wsize;
    info->data_word_size = pt_wsize;

    /* Combine flags of all PTs */
    for (pt_view = 0; pt_view < pt_info->num_pts; pt_view++) {
        info->flags |=  pt_info->pts[pt_view].flags;
    }

    /* Use first PT ID for mem_args (this is a don't care, PTM uses LTID) */
    ptid = pt_info->pts[0].sid;
    SHR_IF_ERR_EXIT
        (pt_mem_args_create(unit, ptid, &info->mem_args));

    /* Get PTM param working buffer offset */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_ptm_param_base_offset_get(wb_block, op_idx,
                                             &info->param_offset));

    /* Get PTM entry key/data working buffer offsets */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_ptm_entry_base_offset_get(wb_block, op_idx,
                                             &entry_base_offset));
    SHR_IF_ERR_EXIT
        (bcmltm_wb_ptm_entry_keyed_lt_offsets_get(entry_base_offset,
                                                  &entry_offsets));
    SHR_IF_ERR_EXIT
        (bcmltm_wb_ptm_entry_keyed_idx_get(ptm_op,
                                           BCMLTM_FIELD_TYPE_KEY,
                                           BCMLTM_FIELD_DIR_IN,
                                           &set_ki_idx));
    SHR_IF_ERR_EXIT
        (bcmltm_wb_ptm_entry_keyed_idx_get(ptm_op,
                                           BCMLTM_FIELD_TYPE_VALUE,
                                           BCMLTM_FIELD_DIR_IN,
                                           &set_di_idx));
    SHR_IF_ERR_EXIT
        (bcmltm_wb_ptm_entry_keyed_idx_get(ptm_op,
                                           BCMLTM_FIELD_TYPE_KEY,
                                           BCMLTM_FIELD_DIR_OUT,
                                           &set_ko_idx));
    SHR_IF_ERR_EXIT
        (bcmltm_wb_ptm_entry_keyed_idx_get(ptm_op,
                                           BCMLTM_FIELD_TYPE_VALUE,
                                           BCMLTM_FIELD_DIR_OUT,
                                           &set_do_idx));
    for (pt_idx = 0; pt_idx < BCMLTM_PT_ENTRY_LIST_MAX; pt_idx++) {
        info->input_key_buffer_offset[pt_idx] =
            entry_offsets.key[set_ki_idx][pt_idx];
        info->input_data_buffer_offset[pt_idx] =
            entry_offsets.data[set_di_idx][pt_idx];
        info->output_key_buffer_offset[pt_idx] =
            entry_offsets.key[set_ko_idx][pt_idx];
        info->output_data_buffer_offset[pt_idx] =
            entry_offsets.data[set_do_idx][pt_idx];
    }

    *info_ptr = info;

 exit:
    if (SHR_FUNC_ERR()) {
        pt_keyed_op_info_destroy(info);
        *info_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the given list of PT keyed info metadata.
 *
 * Destroy the given list of PT keyed info metadata.
 *
 * \param [in] info_list List of PT keyed info to destroy.
 */
static void
pt_keyed_list_destroy(bcmltm_pt_keyed_op_info_t **info_list)
{
    uint32_t idx;

    if (info_list == NULL) {
        return;
    }

    for (idx = 0; idx < BCMLTM_DB_DM_PT_KEYED_NUM; idx++) {
        pt_keyed_op_info_destroy(info_list[idx]);
    }

    return;
}

/*!
 * \brief Create the list of PT keyed operations information metadata.
 *
 * Create the list of PT keyed operations information metadata for
 * all the PTM operations on the given logical table.
 * Some PTM operations share the same information, so only necessary
 * distinct node cookie are created.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] dm_arg DM arguments.
 * \param [in] lt_map LT mapping information.
 * \param [out] info_list Returning list of PT keyed op info metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pt_keyed_list_create(int unit,
                     bcmlrd_sid_t sid,
                     const bcmltm_db_dm_arg_t *dm_arg,
                     const bcmltm_db_dm_map_entries_t *lt_map,
                     bcmltm_pt_keyed_op_info_t **info_list)
{
    SHR_FUNC_ENTER(unit);

    /* Create information for INSERT/DELETE (share same information) */
    SHR_IF_ERR_EXIT
        (pt_keyed_op_info_create(unit, sid,
                                 BCMPTM_OP_INSERT,
                                 dm_arg,
                                 lt_map,
                                &info_list[BCMLTM_DB_DM_PT_KEYED_INS_DEL_IDX]));

    /* Create information for LOOKUP/TRAVERSE (share same information) */
    SHR_IF_ERR_EXIT
        (pt_keyed_op_info_create(unit, sid,
                                 BCMPTM_OP_LOOKUP,
                                 dm_arg,
                                 lt_map,
                                 &info_list[BCMLTM_DB_DM_PT_KEYED_LK_TRV_IDX]));

 exit:
    if (SHR_FUNC_ERR()) {
        pt_keyed_list_destroy(info_list);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the given EE Index Info metadata.
 *
 * Destroy the given EE Index Info metadata.
 *
 * \param [in] index_info Index info.
 */
static void
ee_lt_index_destroy(bcmltm_ee_index_info_t *index_info)
{
    if (index_info == NULL) {
        return;
    }

    SHR_FREE(index_info);

    return;
}

/*!
 * \brief Create the EE LT Index Info metadata.
 *
 * Create the EE LT Index Info metadata for the given table.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] lt_pvt_wb_offsets LT Private working buffer block offsets.
 * \param [out] index_info_ptr Returning pointer EE LT Index metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ee_lt_index_create(int unit, bcmlrd_sid_t sid,
                   const bcmltm_wb_lt_pvt_offsets_t *lt_pvt_wb_offsets,
                   bcmltm_ee_index_info_t **index_info_ptr)
{
    bcmltm_ee_index_info_t *index_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(index_info, sizeof(*index_info), "bcmltmDmEeIndexInfo");
    SHR_NULL_CHECK(index_info, SHR_E_MEMORY);
    sal_memset(index_info, 0, sizeof(*index_info));

    /* Fill data */
    index_info->index_offset = lt_pvt_wb_offsets->track_index;

    *index_info_ptr = index_info;

 exit:
    if (SHR_FUNC_ERR()) {
        ee_lt_index_destroy(index_info);
        *index_info_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Fill an array of WB copy data.
 *
 * This routine fills an array of WB to copy from a source
 * offset to a list of destination offsets.
 *
 * \param [in] from_offset Source WBC to copy from.
 * \param [in] num_offset Number of offsets to copy to.
 * \param [in] to_offset Destination WBCs to copy to.
 * \param [out] wb_copy Array of WB copy.
 *
 * \retval Number of WB elements copied.
 */
static int
pt_key_wb_copy_fill(uint32_t from_offset,
                    uint32_t num_offset, uint32_t *to_offset,
                    bcmltm_wb_copy_t *wb_copy)
{
    uint32_t i;

    for (i = 0; i < num_offset; i++) {
        wb_copy[i].wbc_source.buffer_offset = from_offset;
        wb_copy[i].wbc_source.minbit = PT_KEY_MINBIT;
        wb_copy[i].wbc_source.maxbit = PT_KEY_MAXBIT;
        wb_copy[i].wbc_destination.buffer_offset = to_offset[i];
        wb_copy[i].wbc_destination.minbit = PT_KEY_MINBIT;
        wb_copy[i].wbc_destination.maxbit = PT_KEY_MAXBIT;
    }

    return i;
}

/*!
 * \brief Find matching track index field offset.
 *
 * This routine finds a track index field offset that matches
 * an offset in the provided offset list.
 *
 * \param [in] track Track index fields to search.
 * \param [in] num_offset Size of offset list.
 * \param [in] offset Offset list to search.
 * \param [out] track_field_offset Matching track field offset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
track_index_field_offset_find(const bcmltm_track_index_t *track,
                              uint32_t num_offset,
                              uint32_t *offset,
                              uint32_t *track_field_offset)
{
    uint32_t offset_idx;
    uint32_t track_idx;
    uint32_t num_track_fields = track->track_index_field_num;
    const bcmltm_track_index_field_t *track_field;

    *track_field_offset = BCMLTM_WB_OFFSET_INVALID;

    for (offset_idx = 0; offset_idx < num_offset; offset_idx++) {
        for (track_idx = 0; track_idx < num_track_fields; track_idx++) {

            track_field = &track->track_index_fields[track_idx];
            if (track_field->field_offset == offset[offset_idx]) {
                /* Found */
                *track_field_offset = track_field->field_offset;
                return SHR_E_NONE;
            }
        }
    }

    return SHR_E_NOT_FOUND;
}

/*!
 * \brief Get the PT offsets and corresponding track field offset.
 *
 * This routine gets all the PT offsets and the corresponding track
 * field offset for the specified field type in a given logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] pt_list PT list.
 * \param [in] track_index Track index.
 * \param [in] type Track index field type TRACK_INDEX_FIELD_PT_xxx.
 * \param [in] offset_max Array size of offset.
 * \param [out] offset Returning offsets.
 * \param [out] num_offset Actual number of offsets returned.
 * \param [out] track_field_offset Corresponding track field offset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
track_field_pt_offsets_get(int unit, bcmlrd_sid_t sid,
                           const bcmltm_pt_list_t *pt_list,
                           const bcmltm_track_index_t *track_index,
                           track_index_field_type_t type,
                           uint32_t offset_max,
                           uint32_t *offset,
                           uint32_t *num_offset,
                           uint32_t *track_field_offset)
{
    SHR_FUNC_ENTER(unit);

    *track_field_offset = BCMLTM_WB_OFFSET_INVALID;

    /* Get offsets */
    if (type == TRACK_INDEX_FIELD_PT_INDEX) {
        SHR_IF_ERR_EXIT
            (bcmltm_db_dm_pt_index_offsets_get(unit, sid, pt_list,
                                               offset_max,
                                               offset, num_offset));
    } else if (type == TRACK_INDEX_FIELD_PT_MEM_PARAM) {
        SHR_IF_ERR_EXIT
            (bcmltm_db_dm_pt_mem_param_offsets_get(unit, sid, pt_list,
                                                   offset_max,
                                                   offset, num_offset));
    }  else if (type == TRACK_INDEX_FIELD_PT_SID_SEL) {
        SHR_IF_ERR_EXIT
            (bcmltm_db_dm_pt_sid_sel_offsets_get(unit, sid, pt_list,
                                                 offset_max, offset,
                                                 num_offset));
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* If no offset, just return */
    if (*num_offset == 0) {
        SHR_EXIT();
    }

    /* Find track index offset for PT field component */
    SHR_IF_ERR_EXIT
        (track_index_field_offset_find(track_index,
                                       *num_offset, offset,
                                       track_field_offset));
 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the given WB copy list metadata.
 *
 * Destroy the given WB copy list metadata.
 *
 * \param [in] index WB copy list.
 */
static void
wb_copy_list_destroy(bcmltm_wb_copy_list_t *list)
{
    if (list == NULL) {
        return;
    }

    if (list->wb_copy != NULL) {
        SHR_FREE(list->wb_copy);
    }
    SHR_FREE(list);

    return;
}

/*!
 * \brief Create the Track Index fields to PT WB copy list metadata.
 *
 * This routine creates WB copy list metadata for the given table
 * use for copying the track index field components (those that
 * uses the PT parameters) into the corresponding PT WB locations.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] pt_list PT list.
 * \param [in] track_index Track Index.
 * \param [out] list_ptr Returning pointer WB copy list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
track_to_pt_copy_create(int unit, bcmlrd_sid_t sid,
                        const bcmltm_pt_list_t *pt_list,
                        const bcmltm_track_index_t *track_index,
                        bcmltm_wb_copy_list_t **list_ptr)
{
    bcmltm_wb_copy_list_t *list = NULL;
    int num_pt = pt_list->num_pt_view;
    int num_pt_track_fields;
    uint32_t *offset = NULL;
    uint32_t *index_offset = NULL;
    uint32_t *mem_param_offset = NULL;
    uint32_t *sid_sel_offset = NULL;
    uint32_t num_index_offset;
    uint32_t num_mem_param_offset;
    uint32_t num_sid_sel_offset;
    uint32_t track_index_offset;
    uint32_t track_mem_param_offset;
    uint32_t track_sid_sel_offset;
    uint32_t max_num_wb_copy = 0;
    uint32_t num_wb_copy = 0;
    bcmltm_wb_copy_t *wb_copy = NULL;
    unsigned int size;

    SHR_FUNC_ENTER(unit);

    /*
     * If track index has no track field elements there is nothing to
     * copy to the PT WB params, just return.
     */
    if (track_index->track_index_field_num == 0) {
        *list_ptr = NULL;
        SHR_EXIT();
    }

    

    /* Max number of possible track PT field components */
    num_pt_track_fields = 3;

    /* Allocate */
    size = sizeof(*offset) * num_pt * num_pt_track_fields;
    SHR_ALLOC(offset, size, "bcmltmDmOffsets");
    SHR_NULL_CHECK(offset, SHR_E_MEMORY);
    sal_memset(offset, 0, sizeof(*offset));

    index_offset = offset;
    mem_param_offset = &offset[num_pt];
    sid_sel_offset = &offset[num_pt * 2];

    /* Get the PT offsets and corresponding Track field offset */
    /* PT Index */
    SHR_IF_ERR_EXIT
        (track_field_pt_offsets_get(unit, sid, pt_list, track_index,
                                    TRACK_INDEX_FIELD_PT_INDEX,
                                    num_pt,
                                    index_offset, &num_index_offset,
                                    &track_index_offset));
    max_num_wb_copy += num_index_offset;

    /* PT Memory Parameter */
    SHR_IF_ERR_EXIT
        (track_field_pt_offsets_get(unit, sid, pt_list, track_index,
                                    TRACK_INDEX_FIELD_PT_MEM_PARAM,
                                    num_pt,
                                    mem_param_offset, &num_mem_param_offset,
                                    &track_mem_param_offset));
    max_num_wb_copy += num_mem_param_offset;

    /* PT SID Selector */
    SHR_IF_ERR_EXIT
        (track_field_pt_offsets_get(unit, sid, pt_list, track_index,
                                    TRACK_INDEX_FIELD_PT_SID_SEL,
                                    num_pt,
                                    sid_sel_offset, &num_sid_sel_offset,
                                    &track_sid_sel_offset));
    max_num_wb_copy += num_sid_sel_offset;

    /* Allocate */
    SHR_ALLOC(list, sizeof(*list),
              "bcmltmDmTrackToPtWbCopyList");
    SHR_NULL_CHECK(list, SHR_E_MEMORY);
    sal_memset(list, 0, sizeof(*list));

    if (max_num_wb_copy > 0) {
        size = sizeof(*wb_copy) * max_num_wb_copy;
        SHR_ALLOC(wb_copy, size,
                  "bcmltmDmTrackToPtWbCopyArr");
        SHR_NULL_CHECK(wb_copy, SHR_E_MEMORY);
        sal_memset(wb_copy, 0, sizeof(*wb_copy));
    }

    list->num_wb_copy = 0;
    list->wb_copy = wb_copy;
    list->index_absent_offset = track_index->index_absent_offset;

    *list_ptr = list;

    if (max_num_wb_copy == 0) {
        SHR_EXIT();
    }

    /* Fill WBC copy list */
    num_wb_copy += pt_key_wb_copy_fill(track_index_offset,
                                       num_index_offset, index_offset,
                                       &wb_copy[num_wb_copy]);

    num_wb_copy += pt_key_wb_copy_fill(track_mem_param_offset,
                                       num_mem_param_offset, mem_param_offset,
                                       &wb_copy[num_wb_copy]);

    num_wb_copy += pt_key_wb_copy_fill(track_sid_sel_offset,
                                       num_sid_sel_offset, sid_sel_offset,
                                       &wb_copy[num_wb_copy]);

    list->num_wb_copy = num_wb_copy;

 exit:
    if (SHR_FUNC_ERR()) {
        wb_copy_list_destroy(list);
        *list_ptr = NULL;
    }

    SHR_FREE(offset);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate a field map object and insert to link list.
 *
 * This function allocates a field map object and inserts it
 * to the field map link list.
 *
 * This function does not fill field map information.
 *
 * \param [in] field_list Field list to add new field map to.
 * \param [in] field_map_ptr Returns pointer to new field map.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_map_alloc(bcmltm_field_map_t *field_list,
                bcmltm_field_map_t **field_map_ptr)
{
    bcmltm_field_map_t *new_field_map = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Allocate */
    SHR_ALLOC(new_field_map, sizeof(*new_field_map), "bcmltmDmFieldMap");
    SHR_NULL_CHECK(new_field_map, SHR_E_MEMORY);
    sal_memset(new_field_map, 0, sizeof(*new_field_map));

    /* Insert it first in the link list (order does not matter) */
    new_field_map->next_map = field_list->next_map;
    field_list->next_map = new_field_map;

    *field_map_ptr = new_field_map;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the field map metadata for given LRD field entry map.
 *
 * Get the field map metadata for given LRD field entry map.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in] map_info Field map information from LRD map entry.
 * \param [in] field_type Indicates key or value field map type.
 * \param [in] field_dir Indicates input or output field direction,
 *                       (relevant only for Keyed LTs on Key fields).
 * \param [out] field_map PTM field map to fill.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_map_get(int unit,
              bcmlrd_sid_t sid,
              const bcmltm_table_attr_t *attr,
              const bcmltm_map_field_mapped_t *map_info,
              bcmltm_field_type_t field_type,
              bcmltm_field_dir_t field_dir,
              bcmltm_field_map_t *field_map)
{
    SHR_FUNC_ENTER(unit);

    field_map->field_id = map_info->src_field_id;
    field_map->field_idx = map_info->src_field_idx;

    /* Set working buffer offset */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_ptm_field_lt_offset_get(map_info->ptm_wb_block,
                                           map_info->dst_field_id,
                                           field_type,
                                           field_dir,
                                           map_info->pt_entry_idx,
                                           &(field_map->wbc.buffer_offset)));
    field_map->wbc.maxbit = map_info->wbc_maxbit;
    field_map->wbc.minbit = map_info->wbc_minbit;

    /* Set PT changed offset */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_ptm_pt_changed_offset_get(attr->type,
                                             field_type,
                                             map_info->ptm_wb_block,
                                             map_info->pt_entry_idx,
                                             &field_map->pt_changed_offset));

    /* Default value */
    field_map->default_value = map_info->default_value;
    /* Minimum value */
    field_map->minimum_value = map_info->minimum_value;
    /* Maximum value */
    field_map->maximum_value = map_info->maximum_value;

    /* Field properties */
    SHR_IF_ERR_EXIT
        (bcmltm_db_field_properties_get(unit, sid,
                                        map_info->src_field_id,
                                        &field_map->flags));

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the list of field maps metadata.
 *
 * Create the list of field maps metadata.
 *
 * This routine copies the maps for valid fields from the source list which
 * was perhaps larger than needed and formats it to what is expected
 * for its final use (FA cookie).
 *
 * NOTE:  The node functions that use this list expects a certain
 * placement of the field maps in order to function properly.
 * The following pattern must be met for the returned field map list:
 * - Maps that correspond to the same API logical field ID and index (fid, idx)
 *   are placed across the link list of the array element.
 * - Logical fields with distinct (fid, idx) are placed along the array.
 *   (there is no other additional ordering, i.e. elements in list
 *    array are not ordered).
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in] map_info_list Source field mapping information list.
 * \param [in] field_type Indicates key or value field.
 * \param [in] field_dir Indicates input or output field direction,
 *                       (relevant only for Keyed LTs on Key fields).
 * \param [in] keys_alloc Indicates what keys to include in returning list
 *                        (relevant only for key fields on IwA LTs).
 * \param [out] field_maps_ptr Returning pointer to list of field maps.
 * \param [out] num_fields Number of field maps returned in list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_maps_create(int unit,
                  bcmlrd_sid_t sid,
                  const bcmltm_table_attr_t *attr,
                  const bcmltm_map_field_mapped_list_t *map_info_list,
                  bcmltm_field_type_t field_type,
                  bcmltm_field_dir_t field_dir,
                  keys_alloc_t keys_alloc,
                  bcmltm_field_map_t **field_maps_ptr,
                  uint32_t *num_fields)
{
    bcmltm_field_map_t *field_maps = NULL;
    bcmltm_field_map_t *field_map = NULL;
    bcmltm_field_map_t *new_field_map = NULL;
    const bcmltm_map_field_mapped_t *map_info;
    unsigned int size;
    uint32_t field_max_count;
    uint32_t info_idx;
    uint32_t field_idx;
    uint32_t lt_fid;

    SHR_FUNC_ENTER(unit);

    *num_fields = 0;
    *field_maps_ptr = NULL;

    /* Get total number of fields maps */
    field_max_count = map_info_list->num;
    if (field_max_count == 0) {
        SHR_EXIT();
    }

    /* Allocate field_map */
    size = sizeof(*field_maps) * field_max_count;
    SHR_ALLOC(field_maps, size, "bcmltmDmFieldMaps");
    SHR_NULL_CHECK(field_maps, SHR_E_MEMORY);
    sal_memset(field_maps, 0, size);

    /*
     * Process field map information list
     *
     * The map information list is already organized in the
     * the way the FA node cookie field map is expected:
     * - All maps with the same field ID and index (fid, idx)
     *   are placed across the link list of the array element.
     * - Fields with distinct (fid, idx) are placed along the array.
     * - Note that there is no other additional ordering
     *   (i.e. elements in list array are not ordered).
     *
     * As as result, the PTM field map can be constructed following
     * the same map information list organization.
     */
    for (info_idx = 0, field_idx = 0;
         info_idx < map_info_list->max; info_idx++) {

        map_info = &map_info_list->field_maps[info_idx];

        /* Skip invalid field */
        if (map_info->src_field_idx == BCMLTM_FIELD_INDEX_UNDEFINED) {
            continue;
        }

        /*
         * Skip key field if:
         *   - Request is required field AND key field is allocatable
         *   - Request is optional (alloc) field AND key field is required
         */
        lt_fid = map_info->src_field_id;
        if (field_type == BCMLTM_FIELD_TYPE_KEY) {
            if (((keys_alloc == KEYS_ALLOC_REQUIRED) &&
                 bcmltm_db_field_key_is_alloc(unit, sid, lt_fid)) ||
                ((keys_alloc == KEYS_ALLOC_OPTIONAL) &&
                 !bcmltm_db_field_key_is_alloc(unit, sid, lt_fid))) {
                continue;
            }
        }

        /* Process field map */
        field_map = &field_maps[field_idx];

        /* Get field map information */
        SHR_IF_ERR_EXIT
            (field_map_get(unit, sid, attr, map_info, field_type, field_dir,
                           field_map));

        /* Process maps across link list */
        map_info = map_info->next;
        while (map_info != NULL) {
            /* Allocate map and insert to list */
            SHR_IF_ERR_EXIT
                (field_map_alloc(field_map, &new_field_map));

            /* Get field map information */
            SHR_IF_ERR_EXIT
                (field_map_get(unit, sid, attr, map_info, field_type, field_dir,
                               new_field_map));

            map_info = map_info->next;
        }

        field_idx++;
    }

    /* Sanity check */
    if (keys_alloc == KEYS_ALLOC_NONE) {
        if (field_idx != map_info_list->num) {
            const char *table_name;

            table_name = bcmltm_lt_table_sid_to_name(unit, sid);
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "LTM Number of fields mismatch: "
                                  "%s(ltid=%d) expected=%d actual=%d\n"),
                       table_name, sid, map_info_list->num, field_idx));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    *num_fields = field_idx;
    *field_maps_ptr = field_maps;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_field_maps_destroy(field_max_count, field_maps);
        *num_fields = 0;
        *field_maps_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the field list metadata.
 *
 * Create the field list metadata for given table ID.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] dm_arg DM arguments.
 * \param [in] lt_map LT mapping information.
 * \param [in] field_type Indicates key or value field.
 * \param [in] field_dir Indicates input or output field direction,
 *                       (relevant only for Keyed LTs on Key fields).
 * \param [in] keys_alloc Indicates what keys to include in returning list
 *                        (relevant only for key fields on IwA LTs).
 * \param [out] field_list_ptr Returning pointer to field list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_map_list_create(int unit,
                      bcmlrd_sid_t sid,
                      const bcmltm_db_dm_arg_t *dm_arg,
                      const bcmltm_db_dm_map_entries_t *lt_map,
                      bcmltm_field_type_t field_type,
                      bcmltm_field_dir_t field_dir,
                      keys_alloc_t keys_alloc,
                      bcmltm_field_map_list_t **field_list_ptr)
{
    const bcmltm_map_field_mapped_list_t *map_info_list;
    const bcmltm_table_attr_t *attr = dm_arg->attr;
    const bcmltm_wb_lt_pvt_offsets_t *lt_pvt_wb_offsets;
    bcmltm_field_map_list_t *field_list = NULL;
    uint32_t num_fields;

    SHR_FUNC_ENTER(unit);

    lt_pvt_wb_offsets = dm_arg->lt_pvt_wb_offsets;

    /* Allocate */
    SHR_ALLOC(field_list, sizeof(*field_list), "bcmltmDmFieldList");
    SHR_NULL_CHECK(field_list, SHR_E_MEMORY);
    sal_memset(field_list, 0, sizeof(*field_list));

    /* Key or Value */
    if (field_type == BCMLTM_FIELD_TYPE_KEY) {
        map_info_list = &lt_map->keys;
    } else {
        map_info_list = &lt_map->values;
    }

    /* Get field map */
    SHR_IF_ERR_EXIT
        (field_maps_create(unit, sid, attr, map_info_list,
                           field_type, field_dir, keys_alloc,
                           &field_list->field_maps,
                           &num_fields));
    field_list->num_fields = num_fields;

    /* Valid for Index with Allocation LT and on optional key fields */
    if (BCMLTM_TABLE_TYPE_IS_ALLOC_INDEX(attr->type) &&
        (field_type == BCMLTM_FIELD_TYPE_KEY) &&
        (keys_alloc == KEYS_ALLOC_OPTIONAL)) {
        /* Sanity check */
        if (lt_pvt_wb_offsets == NULL) {
            const char *table_name;

            table_name = bcmltm_lt_table_sid_to_name(unit, sid);
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Index with Allocation LT must have "
                                  "a LT Private working buffer block: "
                                  "%s(ltid=%d)\n"),
                       table_name, sid));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        field_list->index_absent_offset = lt_pvt_wb_offsets->index_absent;
    } else {
        field_list->index_absent_offset = BCMLTM_WB_OFFSET_INVALID;
    }

    *field_list_ptr = field_list;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_field_map_list_destroy(field_list);
        *field_list_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the field mapping metadata.
 *
 * Create the field mapping metadata for given table ID.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] dm_arg DM arguments.
 * \param [in] lt_map LT mapping information.
 * \param [in] field_type Indicates key or value field.
 * \param [in] field_dir Indicates input or output field direction,
 *                       (relevant only for Keyed LTs on Key fields).
 * \param [in] keys_alloc Indicates what keys to include in returning list
 *                        (relevant only for key fields on IwA LTs).
 * \param [out] field_mapping_ptr Returning pointer to field mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_mapping_create(int unit,
                     bcmlrd_sid_t sid,
                     const bcmltm_db_dm_arg_t *dm_arg,
                     const bcmltm_db_dm_map_entries_t *lt_map,
                     bcmltm_field_type_t field_type,
                     bcmltm_field_dir_t field_dir,
                     keys_alloc_t keys_alloc,
                     bcmltm_field_select_mapping_t **field_mapping_ptr)
{
    bcmltm_field_select_mapping_t *field_mapping = NULL;
    uint32_t alloc_size;
    uint32_t pt_suppress_offset;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    alloc_size = sizeof(*field_mapping) + sizeof(bcmltm_field_map_list_t *);
    SHR_ALLOC(field_mapping, alloc_size,
              "bcmltmDmFieldSelectMapping");
    SHR_NULL_CHECK(field_mapping, SHR_E_MEMORY);
    sal_memset(field_mapping, 0, alloc_size);

    /* Get PT suppress status offset */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_pt_suppress_offset_get(unit,
                                             sid,
                                             dm_arg->pt_suppress,
                                             &pt_suppress_offset));

    /* Fill data */
    field_mapping->pt_suppress_offset = pt_suppress_offset;
    field_mapping->map_index_offset = BCMLTM_WB_OFFSET_INVALID;
    field_mapping->num_maps = 1;

    SHR_IF_ERR_EXIT
        (field_map_list_create(unit, sid, dm_arg,
                               lt_map, field_type, field_dir, keys_alloc,
                               &(field_mapping->field_map_list[0])));

    *field_mapping_ptr = field_mapping;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_field_mapping_destroy(field_mapping);
        *field_mapping_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the fixed field list metadata.
 *
 * Destroy the fixed field list metadata.
 *
 * \param [in] list Fixed field list metadata to destroy.
 */
static void
fixed_fields_destroy(bcmltm_fixed_field_list_t *list)
{
    if (list == NULL) {
        return;
    }

    SHR_FREE(list->fixed_fields);
    SHR_FREE(list);

    return;
}

/*!
 * \brief Create the fixed field list metadata.
 *
 * Create the fixed field list metadata for given table ID.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] lt_map LT mapping information.
 * \param [in] fixed_type Indicates key or value field.
 * \param [out] list_ptr Returning pointer to field mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
fixed_fields_create(int unit,
                    bcmlrd_sid_t sid,
                    const bcmltm_db_dm_map_entries_t *lt_map,
                    bcmltm_fixed_field_type_t fixed_type,
                    bcmltm_fixed_field_list_t **list_ptr)
{
    const bcmltm_map_field_fixed_list_t *fixed_field_info;
    bcmltm_fixed_field_list_t *list = NULL;
    bcmltm_fixed_field_t *fields = NULL;
    uint32_t count = 0;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(list, sizeof(*list), "bcmltmDmFixedFieldList");
    SHR_NULL_CHECK(list, SHR_E_MEMORY);
    sal_memset(list, 0, sizeof(*list));

    /* Key or Value */
    if (fixed_type == BCMLTM_FIXED_FIELD_TYPE_KEY) {
        fixed_field_info = &lt_map->fixed_keys;
    } else {
        fixed_field_info = &lt_map->fixed_values;
    }

    /* Allocate for the fields array and copy */
    count = fixed_field_info->num;
    if (count > 0) {
        SHR_ALLOC(fields, sizeof(*fields) * count,
                  "bcmltmDmFixedFields");
        SHR_NULL_CHECK(fields, SHR_E_MEMORY);
        sal_memcpy(fields, fixed_field_info->field_maps,
                   sizeof(*fields) * count);
    }

    /* Fill data */
    list->num_fixed_fields = count;
    list->fixed_fields = fields;

    *list_ptr = list;

 exit:
    if (SHR_FUNC_ERR()) {
        fixed_fields_destroy(list);
        *list_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the field mapping metadata for all value fields.
 *
 * This routine creates the API cache field mapping metadata for all
 * the value fields in a given table using the provided
 * information created for all selection groups.
 *
 * The created list contains all value fields, regardless of whether
 * a field is selected or not, and is not subject to any field selection,
 * i.e. it is unconditional.
 *
 * The primary use of this metadata is to set the defaults for all
 * value fields in the API cache during a DELETE or UPDATE operation.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] fs_info Field selection information.
 * \param [out] mapping_ptr Returning pointer to field mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
apic_all_value_field_mapping_create(int unit,
                                    bcmlrd_sid_t sid,
                                    const bcmltm_db_dm_fs_info_t *fs_info,
                                    bcmltm_field_select_mapping_t **mapping_ptr)
{
    bcmltm_db_dm_apic_field_info_list_t apic_info_list;
    uint32_t num_apics;
    uint32_t alloc_size;
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    sal_memset(&apic_info_list, 0, sizeof(apic_info_list));

    if ((fs_info == NULL) || (fs_info->num_selections == 0)) {
        /* No API cache field information */
        SHR_EXIT();
    }

    num_apics = fs_info->num_selections;

    /* Allocate for API cache info array */
    alloc_size = sizeof(bcmltm_db_dm_apic_field_info_t *) * num_apics;
    SHR_ALLOC(apic_info_list.apics, alloc_size, "bcmltmDbDmApicInfoList");
    SHR_NULL_CHECK(apic_info_list.apics, SHR_E_MEMORY);
    sal_memset(apic_info_list.apics, 0, alloc_size);
    apic_info_list.num_apics = num_apics;

    /* Copy references to API cache field info for each selection group */
    for (i = 0; i < num_apics; i++) {
        apic_info_list.apics[i] = fs_info->selections[i].apic_fields;
    }

    /* Create API cache field mapping for all value fields */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_apic_all_value_field_mapping_create(unit, sid,
                                                          &apic_info_list,
                                                          mapping_ptr));

 exit:
    SHR_FREE(apic_info_list.apics);
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_db_dm_info_create(int unit, bcmlrd_sid_t sid,
                         const bcmltm_table_attr_t *attr,
                         bcmltm_db_dm_info_t **info_ptr)
{
    bcmltm_db_dm_info_t *info = NULL;
    bcmltm_db_dm_arg_t dm_arg;
    const bcmltm_db_dm_pt_info_t *pt_info;
    bcmltm_db_dm_map_entries_t map_entries;
    bcmltm_table_type_t table_type;
    bcmltm_wb_handle_t *wb_handle = NULL;
    bcmltm_wb_block_t *lt_pvt_wb_block = NULL;
    bcmltm_wb_lt_pvt_offsets_t lt_pvt_wb_offsets;
    bcmltm_db_map_entry_list_t *track_map_entry_list = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(info, sizeof(*info), "bcmltmDbDmInfo");
    SHR_NULL_CHECK(info, SHR_E_MEMORY);
    sal_memset(info, 0, sizeof(*info));

    /* Get table type */
    table_type = attr->type;

    /* Create core PT information */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_pt_info_create(unit, sid, attr, &info->pt_info));
    pt_info = info->pt_info;

    /* Add LT Private working buffer block */
    wb_handle = bcmltm_wb_lt_handle_get(unit, sid);
    info->lt_pvt_wb_block_id = BCMLTM_WB_BLOCK_ID_INVALID;
    SHR_IF_ERR_EXIT
        (bcmltm_wb_block_lt_pvt_add(unit, wb_handle,
                                    &info->lt_pvt_wb_block_id,
                                    &lt_pvt_wb_block));
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lt_pvt_offsets_get(lt_pvt_wb_block,
                                      &lt_pvt_wb_offsets));

    /* Set PT suppress availability */
    info->pt_suppress = bcmltm_db_dm_is_pt_suppress(unit, sid);

    /* Track index map supplied */
    info->track_index_map = bcmltm_db_dm_is_track_index_map(unit, sid);

    /* Prepare direct map arguments */
    sal_memset(&dm_arg, 0, sizeof(dm_arg));
    dm_arg.attr = attr;
    dm_arg.pt_info = info->pt_info;
    dm_arg.pt_suppress = info->pt_suppress;
    dm_arg.track_index_map = info->track_index_map;
    dm_arg.lt_pvt_wb_offsets = &lt_pvt_wb_offsets;

    /* Obtain LT map entries information */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_map_entries_parse(unit, sid, attr, pt_info,
                                        &map_entries));

    /* Create PT Information list (used as EE node cookie for PTM operations) */
    
    if (BCMLTM_TABLE_TYPE_IS_INDEX(table_type)) {
        SHR_IF_ERR_EXIT
            (pt_list_create(unit, sid, &dm_arg, &map_entries, &info->pt_list));
    } else if (BCMLTM_TABLE_TYPE_IS_KEYED(table_type)) {
        SHR_IF_ERR_EXIT
            (pt_keyed_list_create(unit, sid, &dm_arg, &map_entries,
                                  info->pt_keyed_list));
    }

    /* Create Field Selection Information */
    
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_fs_info_create(unit, sid, &dm_arg, &info->fs));

    /* Create API Cache field mapping for all value fields */
    SHR_IF_ERR_EXIT
        (apic_all_value_field_mapping_create(unit, sid,
                                             info->fs,
                                             &info->apic_all_value_fields));

    /* Create Input Key PTM Field Mappings (FA node cookie) */
    SHR_IF_ERR_EXIT
        (field_mapping_create(unit, sid,
                              &dm_arg,
                              &map_entries,
                              BCMLTM_FIELD_TYPE_KEY,
                              BCMLTM_FIELD_DIR_IN,
                              KEYS_ALLOC_NONE,
                              &info->in_key_fields));
    /*
     * Create Output Key PTM Field Mappings (FA node cookie).
     * Input and output field mappings are different on Keyed LTs,
     * but they are the same on Index LTs.
     */
    if (BCMLTM_TABLE_TYPE_IS_KEYED(table_type)) {
        (field_mapping_create(unit, sid,
                              &dm_arg,
                              &map_entries,
                              BCMLTM_FIELD_TYPE_KEY,
                              BCMLTM_FIELD_DIR_OUT,
                              KEYS_ALLOC_NONE,
                              &info->out_key_fields));
    } else {
        info->out_key_fields = info->in_key_fields;
    }

    /* Create Required and Optional Key PTM Field Mappings for IwA LTs */
    if (BCMLTM_TABLE_TYPE_IS_ALLOC_INDEX(table_type)) {
        SHR_IF_ERR_EXIT
            (field_mapping_create(unit, sid,
                                  &dm_arg,
                                  &map_entries,
                                  BCMLTM_FIELD_TYPE_KEY,
                                  BCMLTM_FIELD_DIR_IN,
                                  KEYS_ALLOC_REQUIRED,
                                  &info->req_key_fields));
        SHR_IF_ERR_EXIT
            (field_mapping_create(unit, sid,
                                  &dm_arg,
                                  &map_entries,
                                  BCMLTM_FIELD_TYPE_KEY,
                                  BCMLTM_FIELD_DIR_IN,
                                  KEYS_ALLOC_OPTIONAL,
                                  &info->alloc_key_fields));
    }

    /*
     * Create Value PTM field mappings (FA node cookie).
     * Input and output field mappings are the same  for all LTs.
     */
    SHR_IF_ERR_EXIT
        (field_mapping_create(unit, sid,
                              &dm_arg,
                              &map_entries,
                              BCMLTM_FIELD_TYPE_VALUE,
                              BCMLTM_FIELD_DIR_IN,
                              KEYS_ALLOC_NONE,
                              &info->value_fields));

    /* Create Fixed Field Mappings (used as FA node cookies) */
    SHR_IF_ERR_EXIT
        (fixed_fields_create(unit, sid, &map_entries,
                             BCMLTM_FIXED_FIELD_TYPE_KEY,
                             &info->fixed_key_fields));
    SHR_IF_ERR_EXIT
        (fixed_fields_create(unit, sid, &map_entries,
                             BCMLTM_FIXED_FIELD_TYPE_VALUE,
                             &info->fixed_value_fields));

    /* Create Read-Only Field Lists (used as FA node cookies) */
    SHR_IF_ERR_EXIT
        (bcmltm_db_read_only_field_info_create(unit, sid,
                                               &info->read_only_fields));

    /* Create Wide Field Lists (used as FA node cookies) */
    SHR_IF_ERR_EXIT
        (bcmltm_db_wide_field_info_create(unit, sid,
                                          &info->wide_fields));

    
    if (BCMLTM_TABLE_TYPE_IS_INDEX(table_type)) {
        /* FA Track Index (used as FA node cookie on Track Index stage) */
        SHR_IF_ERR_EXIT
            (bcmltm_db_dm_track_index_create(unit, sid, &dm_arg,
                                             info->pt_list,
                                             &info->track_index,
                                             &track_map_entry_list));

        /* EE LT Index (used as EE node cookie on LT Index operations) */
        SHR_IF_ERR_EXIT
            (ee_lt_index_create(unit, sid, &lt_pvt_wb_offsets,
                                &info->ee_lt_index));

        /* Track to PT Index WBC copy (FA node cookie on LT Index traverse) */
        SHR_IF_ERR_EXIT
            (track_to_pt_copy_create(unit, sid,
                                     info->pt_list, info->track_index,
                                     &info->track_to_pt_copy));

        
        if ((info->fs != NULL) &&
            (info->fs->num_selections > 0)) {
            const bcmltm_db_dm_fs_t *fs = NULL;
            const bcmltm_db_dm_xfrm_list_t *rev_key_xfrms = NULL;

            /*
             * Get key transforms.
             *
             * Key information is in the unconditional selection group.
             */
            fs = bcmltm_db_dm_fs_find(info->fs,
                                      BCMLTM_FIELD_SELECTION_ID_UNCOND);

            if (fs->xfrms != NULL) {
                rev_key_xfrms = fs->xfrms->rev_keys;
            }

            /* Create Track Index Information */
            SHR_IF_ERR_EXIT
                (bcmltm_db_dm_track_info_create(unit, sid, &dm_arg,
                                                track_map_entry_list,
                                                rev_key_xfrms,
                                                &info->track_info));
        }
    }

    /* Create Field Transforms Information */
    SHR_IF_ERR_EXIT
        (bcmltm_db_xfrm_info_create(unit, sid, attr,
                                    &map_entries, pt_info,
                                    &info->xfrm));

    *info_ptr = info;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_dm_info_destroy(info);
        *info_ptr = NULL;
    }

    bcmltm_db_map_entry_list_destroy(track_map_entry_list);
    bcmltm_db_dm_map_entries_cleanup(&map_entries);

    SHR_FUNC_EXIT();
}

void
bcmltm_db_dm_info_destroy(bcmltm_db_dm_info_t *info)
{
    if (info == NULL) {
        return;
    }

    /* Destroy XFRM INFO List */
    bcmltm_db_xfrm_info_destroy(info->xfrm);

    /* Destroy LT Track Index */
    bcmltm_db_dm_track_info_destroy(info->track_info);
    wb_copy_list_destroy(info->track_to_pt_copy);
    ee_lt_index_destroy(info->ee_lt_index);
    bcmltm_db_dm_track_index_destroy(info->track_index);

    /* Destroy Wide Field List */
    bcmltm_db_wide_field_info_destroy(info->wide_fields);

    /* Destroy Read-Only Field List */
    bcmltm_db_read_only_field_info_destroy(info->read_only_fields);

    /* Destroy Fixed Field List Mappings */
    fixed_fields_destroy(info->fixed_value_fields);
    fixed_fields_destroy(info->fixed_key_fields);

    /* Destroy Field Mappings for Keys and Values */
    bcmltm_db_field_mapping_destroy(info->value_fields);
    bcmltm_db_field_mapping_destroy(info->alloc_key_fields);
    bcmltm_db_field_mapping_destroy(info->req_key_fields);
    if (info->out_key_fields != info->in_key_fields) {
        bcmltm_db_field_mapping_destroy(info->out_key_fields);
    }
    bcmltm_db_field_mapping_destroy(info->in_key_fields);

    /* Destroy API Cache field mapping for all value fields */
    bcmltm_db_field_mapping_destroy(info->apic_all_value_fields);

    /* Destroy Field Selection Information */
    bcmltm_db_dm_fs_info_destroy(info->fs);

    /* Destroy PT Keyed list */
    pt_keyed_list_destroy(info->pt_keyed_list);

    /* Destroy PT list */
    pt_list_destroy(info->pt_list);

    /* Destroy core PT information */
    bcmltm_db_dm_pt_info_destroy(info->pt_info);

    SHR_FREE(info);

    return;
}

