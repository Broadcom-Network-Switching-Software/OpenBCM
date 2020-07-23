/*! \file bcmltm_db_xfrm.c
 *
 * Logical Table Manager - Information for Field Transforms.
 *
 * This file contains routines to construct information for
 * field transforms in logical tables.
 *
 * The metadata created is used for the following LTM driver flow:
 * (non field select opcode driver version)
 *     API <-> [VALIDATE, XFRM] <-> PTM
 *
 * This driver flow can be applied only to tables without field selections.
 *
 * This information is derived from the LRD Field Transform map entries
 * in the Physical (direct map) map groups.
 *
 * Transform fields are declared in the LRD map file along with its
 * type, whether SOURCE or DESTINATION:
 * - SOURCE fields:
 *   These fields are those declared as "transform_src" in the LT map file.
 *   For the most part, these are the logical table fields.
 *   These fields are used as INPUT fields for FORWARD transform, and
 *   used as OUTPUT for REVERSE transform.
 *
 * - SOURCE KEY fields:
 *   These fields are those declared as "transform_src_key" in the LT map file.
 *   These are the logical table fields, used as INPUT fields for both FORWARD
 *   and REVERSE Value transform.
 *
 * - DESTINATION fields:
 *   These fields are those declared as "transform_dst" in the LT map file.
 *   For the most part, these are the physical table fields.
 *
 * Each LT transform consists of a set of transforms:
 * - FORWARD Transform:
 *   This takes information from a set of LT logical fields
 *   and, for the most part, produces the Physical fields and
 *   corresponding information to be written to PTM.
 * - REVERSE Transform:
 *   This takes a set of Physical fields and corresponding information
 *   obtained from PTM and produces a set of LT logical fields.
 *
 * The source and destination fields are used as input or output depending
 * on the transform function:
 * - FORWARD Transform:  INPUT  = SOURCE fields, SOURCE_KEY fields
 *                       OUTPUT = DESTINATION fields
 * - REVERSE Transform:  INPUT  = DESTINATION fields, SOURCE_KEY fields.
 *                       OUTPUT = SOURCE fields
 *
 *   transform_src: Logical field
 *                  - INPUT  for Forward transform.
 *                  - OUTPUT for Reverse transform.
 *   transform_key_src: Logical field
 *                  - INPUT for Forward and Reverse transform.
 *
 *   transform_dst: Physical field
 *                  - OUTPUT for Forward transform.
 *                  - INPUT  for Reverse transform.
 *
 *   +---------+----------------------+----------------------+
 *   |         | Forward              | Reverse              |
 *   +---------+----------------------+----------------------+
 *   | INPUT   | arg->field(Logical)  | arg->field(Physical) |
 *   |         | arg->kfield(Logical) | arg->kfield(Logical) |
 *   +---------+----------------------+----------------------+
 *   | OUTPUT  | arg->rfield(Physical)| arg->rfield(Logical) |
 *   +---------+----------------------+----------------------+
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

#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_table.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_wb_lt_internal.h>
#include <bcmltm/bcmltm_wb_lta_internal.h>
#include <bcmltm/bcmltm_wb_ptm_internal.h>
#include <bcmltm/bcmltm_db_types_internal.h>
#include <bcmltm/bcmltm_db_core_internal.h>

#include "bcmltm_db_md_util.h"
#include "bcmltm_db_xfrm.h"

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/* Maximum number of field indexes in a field */
#define MAX_FIELD_IDXS           50


/*******************************************************************************
 * Private functions
 */

#define IS_XTYPE_FWD(xtype) \
    ((xtype == BCMLTM_DB_XFRM_TYPE_FWD_KEY_TRANSFORM) ||   \
     (xtype == BCMLTM_DB_XFRM_TYPE_FWD_VALUE_TRANSFORM))

#define IS_FTYPE_IN(ftype)      (ftype == BCMLTM_WB_LTA_INPUT_FIELDS)
#define IS_FTYPE_IN_KEY(ftype)  (ftype == BCMLTM_WB_LTA_INPUT_KEY_FIELDS)
#define IS_FTYPE_OUT(ftype)     (ftype == BCMLTM_WB_LTA_OUTPUT_FIELDS)

#define XFRM_SRC_FCOUNT(ptr, type) \
    (IS_XTYPE_FWD(type)) ? ptr->fields : ptr->rfields
#define XFRM_DST_FCOUNT(ptr, type) \
    (IS_XTYPE_FWD(type)) ? ptr->rfields : ptr->fields
#define XFRM_SRC_KEY_FCOUNT(ptr)   (ptr->kfields)

#define XFRM_SRC_FID(ptr, type, idx) \
    (IS_XTYPE_FWD(type)) ? ptr->field[idx] : ptr->rfield[idx]
#define XFRM_SRC_KEY_FID(ptr, idx) (ptr->kfield[idx])

/*!
 * \brief Get the number of source fields for the given LT transform.
 *
 * For given logical table transform, it returns the number of
 * source fields count as follows:
 *   fid_count      - This is the number of logical fields IDs
 *                    and does not include the indexes.
 *   num_fields     - This is the number of the unique field
 *                    elements.  Each element is identified by
 *                    its field ID and field index (fid,idx).
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] xfrm_type Transform type.
 * \param [in] xfrm_arg Transform handler argument.
 * \param [in] is_key Indicate if it is input key or not.
 * \param [out] fid_count Returning number of src field IDs (fid).
 * \param [out] num_fields Returning number of src field elements (fid,idx).
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
src_fields_count_get(int unit, bcmlrd_sid_t sid, int xfrm_type,
                     const bcmltd_transform_arg_t *xfrm_arg,
                     int field_type,
                     uint32_t *fid_count, uint32_t *num_fields)
{
    uint32_t i;
    uint32_t idx_count;
    int fid;

    *fid_count = *num_fields = 0;
    *fid_count = (IS_FTYPE_IN_KEY(field_type)) ?
                 XFRM_SRC_KEY_FCOUNT(xfrm_arg) :
                 XFRM_SRC_FCOUNT(xfrm_arg, xfrm_type);

    for (i = 0; i < *fid_count; i++) {
        fid = (IS_FTYPE_IN_KEY(field_type)) ?
              XFRM_SRC_KEY_FID(xfrm_arg, i) :
              XFRM_SRC_FID(xfrm_arg, xfrm_type, i);
        idx_count = bcmlrd_field_idx_count_get(unit, sid, fid);
        *num_fields += idx_count;
    }

    return SHR_E_NONE;
}

/*!
 * \brief Get the number of destination fields for the given LT transform.
 *
 * For given logical table transform, it returns the number of
 * destination fields count as follows:
 *   fid_count      - This is the number of physical fields IDs
 *                    and does not include the indexes.
 *   num_fields     - This is the number of the unique field
 *                    elements.  Each element is identified by
 *                    its field ID and field index (fid,idx).
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] xfrm_type Transform type.
 * \param [in] xfrm_arg Transform handler argument.
 * \param [in] xfrm_dst Transform destination fields.
 * \param [out] fid_count Returning number of dst field IDs (fid).
 * \param [out] num_fields Returning number of dst field elements (fid,idx).
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
dst_fields_count_get(int unit, bcmlrd_sid_t sid, int xfrm_type,
                     const bcmltd_transform_arg_t *xfrm_arg,
                     const bcmltm_xfrm_dst_fields_t *xfrm_dst,
                     uint32_t *fid_count, uint32_t *num_fields)
{
    uint32_t i;
    uint32_t idx_count;
    uint32_t width;

    SHR_FUNC_ENTER(unit);

    *fid_count = *num_fields = 0;
    *fid_count = XFRM_DST_FCOUNT(xfrm_arg, xfrm_type);

    /* Sanity check */
    if (*fid_count != xfrm_dst->num) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM Transform destination fields: "
                              "%s(ltid=%d) field count mismatch "
                              "lrd_xfrm=%d ltm_xfrm=%d\n"),
                   table_name, sid, *fid_count, xfrm_dst->num));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    for (i = 0; i < *fid_count; i++) {
        width = xfrm_dst->field[i].wbc.maxbit -
                xfrm_dst->field[i].wbc.minbit + 1;
        idx_count = bcmltd_field_idx_count_get(width);
        *num_fields += idx_count;
    }

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the entry index for given destination field ID.
 *
 * This routine gets the map entry index for the given destination
 * field ID in the specified transform.
 *
 * \param [in] xfrm Transform handler.
 * \param [in] xfrm_type Transform or Reverse Transform.
 * \param [in] fid Destination field ID.
 * \param [out] entry_idx Entry index for specified field ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
dst_field_entry_idx_get(const bcmltd_xfrm_handler_t *xfrm,
                        int xfrm_type, uint32_t fid,
                        uint16_t *entry_idx)
{
    const bcmltd_field_list_t *field_list;
    uint32_t idx;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Get the right list for destination PT fields */
    if ((xfrm_type == BCMLTM_DB_XFRM_TYPE_FWD_KEY_TRANSFORM) ||
        (xfrm_type == BCMLTM_DB_XFRM_TYPE_FWD_VALUE_TRANSFORM)) {
        field_list = xfrm->arg->rfield_list;
    } else {
        field_list = xfrm->arg->field_list;
    }

    /* Search transform list */
    for (idx = 0; idx < field_list->field_num; idx++) {
        if (field_list->field_array[idx].field_id == fid) {
            *entry_idx = field_list->field_array[idx].entry_idx;
            break;
        }
    }

    if (idx >= field_list->field_num) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

 exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Check if transform is suppressable by PT suppress.
 *
 * This routine checks if given transform is a candidate to
 * be suppressed when a PT suppress status is supplied.
 *
 * Note that this is only a indication that the transform can be suppressed.
 * To determine whether a transform is skipped on PT suppress,
 * the LT needs to have a transform that supplies a PT suppress status.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] entry_type LRD map entry type.
 * \param [out] is_suppressable TRUE, if transform is affected by PT suppress.
 *                              FALSE, if PT suppress does not affect transform
 *                              (i.e. transform is called regardless of
 *                               PT suppress status).
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_is_pt_suppressable(int unit,
                        bcmlrd_sid_t sid,
                        bcmlrd_map_entry_type_t entry_type,
                        bool *is_suppressable)
{
    const char *table_name = "";

    SHR_FUNC_ENTER(unit);

    switch (entry_type) {
    case BCMLRD_MAP_ENTRY_FWD_VALUE_FIELD_XFRM_HANDLER:
    case BCMLRD_MAP_ENTRY_REV_VALUE_FIELD_XFRM_HANDLER:
        *is_suppressable = TRUE;
        break;

    case BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER:
    case BCMLRD_MAP_ENTRY_REV_KEY_FIELD_XFRM_HANDLER:
    case BCMLRD_MAP_ENTRY_ALWAYS_REV_VALUE_FIELD_XFRM_HANDLER:
        *is_suppressable = FALSE;
        break;

    default:
        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Incorrect map entry type for transform: "
                            "%s(ltid=%d) entry_type=%d\n"),
                   table_name, sid, entry_type));
        SHR_ERR_EXIT(SHR_E_PARAM);
        break;
    }

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the PT suppress working buffer offset for transform.
 *
 * This function gets the PT suppress working buffer offset
 * for given transform, if applicable.
 *
 * If PT suppress does not apply to transform, an invalid offset is
 * returned.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] entry_type LRD map entry type.
 * \param [in] pt_suppress Indicates if PT is supplied in LT map.
 * \param [out] offset Working buffer offset for PT suppress, if applicable.
 *                     Invalid offset, if PT suppress is not applicable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_pt_suppress_offset_get(int unit,
                            bcmlrd_sid_t sid,
                            bcmlrd_map_entry_type_t entry_type,
                            bool pt_suppress,
                            uint32_t *offset)
{
    bool is_suppressable = FALSE;

    SHR_FUNC_ENTER(unit);

    *offset = BCMLTM_WB_OFFSET_INVALID;

    /*
     * PT Suppress
     *
     * The PT suppress status is applicable when all these are true:
     *   - Transform is a PT-suppressable value transform.
     *   - PT suppress is supplied by any transform.
     *
     * Notes:
     * - PT suppress is a per-LT status.
     * - Transforms may or may not be PT-suppressable.  If a transform
     *   is not PT-suppressable, these should not be skipped even if
     *   a PT-suppress is supplied.
     */

    SHR_IF_ERR_EXIT
        (xfrm_is_pt_suppressable(unit,
                                 sid,
                                 entry_type,
                                 &is_suppressable));

    if ((is_suppressable) && (pt_suppress)) {
        SHR_IF_ERR_EXIT
            (bcmltm_wb_lt_pt_suppress_offset_get_by_sid(unit, sid, offset));
    }

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the transform suppress working buffer offset.
 *
 * This function gets the working buffer offset for the transform
 * suppress status, if applicable.
 *
 * The transform suppress status, when valid, indicates whether to
 * skip or call the field transform during an operation.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] attr Table attributes.
 * \param [in] lt_map LT Map information.
 * \param [in] xfrm Transform map information.
 * \param [in] xfrm_type Transform type.
 * \param [out] offset Working buffer offset for transform suppress indicator.
 *                     Invalid offset, if PT suppress is not applicable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
transform_suppress_offset_get(int unit,
                              bcmlrd_sid_t sid,
                              const bcmltm_table_attr_t *attr,
                              const bcmltm_db_dm_map_entries_t *lt_map,
                              const bcmltm_map_xfrm_t *xfrm,
                              int xfrm_type,
                              uint32_t *offset)
{
    const bcmltd_xfrm_handler_t *xfrm_handler = &xfrm->handler;
    uint32_t index_absent_offset = BCMLTM_WB_OFFSET_INVALID;
    uint32_t pt_suppress_offset = BCMLTM_WB_OFFSET_INVALID;

    SHR_FUNC_ENTER(unit);

    *offset = BCMLTM_WB_OFFSET_INVALID;

    /*
     * A transform can be skipped under only one of the following cases:
     * - Index Absent
     * - PT suppress
     */

    /*
     * Index Absent
     *
     * The index absent is applicable when all these are true:
     *   - Transform is a forward key transform.
     *   - Transform contains allocatable keys (IwA LT)
     *
     * Note that this not applicable on reverse key transform.
     * Reverse transform needs to be called when the keys
     * are allocated (index absent flag is set to true),
     * since allocated keys need to be copied into the API output.
     */
    if (xfrm_type == BCMLTM_DB_XFRM_TYPE_FWD_KEY_TRANSFORM) {
        bool alloc = bcmltm_db_xfrm_key_is_alloc(attr, xfrm_type,
                                                 xfrm_handler->arg);
        if (alloc) {
            SHR_IF_ERR_EXIT
                (bcmltm_wb_lt_index_absent_offset_get_by_sid(unit, sid,
                                                      &index_absent_offset));
        }
    }

    /*
     * PT Suppress
     *
     * The PT suppress status is applicable when all these are true:
     *   - Transform is a PT-suppressable value transform.
     *   - PT suppress is supplied by any transform.
     *
     * Notes:
     * - PT suppress is a per-LT status.
     * - Transforms may or may not be PT-suppressable.  If a transform
     *   is not PT-suppressable, these should not be skipped even if
     *   a PT-suppress is supplied.
     */
    SHR_IF_ERR_EXIT
        (xfrm_pt_suppress_offset_get(unit, sid,
                                     xfrm->entry_type,
                                     lt_map->pt_suppress,
                                     &pt_suppress_offset));

    /* Check that only one of the offsets is valid */
    if ((index_absent_offset != BCMLTM_WB_OFFSET_INVALID) &&
        (pt_suppress_offset != BCMLTM_WB_OFFSET_INVALID)) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Unexpected transform suppress condition: "
                              "Index absent and PT suppress are valid  "
                              "%s(ltid=%d)\n"),
                   table_name, sid));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (index_absent_offset != BCMLTM_WB_OFFSET_INVALID) {
        *offset = index_absent_offset;
    } else if (pt_suppress_offset != BCMLTM_WB_OFFSET_INVALID) {
        *offset = pt_suppress_offset;
    }

 exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Destroy the given LTA XFRM table params metadata.
 *
 * Destroy the given LTA XFRM table params metadata.
 *
 * \param [in] params LTA table params to destroy.
 */
static void
transform_params_destroy(bcmltm_lta_transform_params_t *params)
{
    if (params == NULL) {
        return;
    }

    if (params->lta_args != NULL) {
        SHR_FREE(params->lta_args);
    }

    SHR_FREE(params);

    return;
}

/*!
 * \brief Create the LTA XFRM table params metadata for given table.
 *
 * Create the LTA Field Table Handler table params metadata for
 * given transform.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in] lt_map LT Map information.
 * \param [in] xfrm_type Transform type.
 * \param [in] xfrm Transform map information.
 * \param [in] lta_wb_block LTA transform working buffer block.
 * \param [in] ptm_wb_block PTM working buffer block.
 * \param [out] params_ptr Returning pointer table params metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
transform_params_create(int unit,
                        bcmlrd_sid_t sid,
                        const bcmltm_table_attr_t *attr,
                        const bcmltm_db_dm_map_entries_t *lt_map,
                        int xfrm_type,
                        const bcmltm_map_xfrm_t *xfrm,
                        const bcmltm_wb_block_t *lta_wb_block,
                        const bcmltm_wb_block_t *ptm_wb_block,
                        bcmltm_lta_transform_params_t **params_ptr)
{
    bcmltm_lta_transform_params_t *params = NULL;
    const bcmltd_xfrm_handler_t *xfrm_handler = &xfrm->handler;
    bcmltm_wb_lta_fields_offsets_t in_offsets;
    bcmltm_wb_lta_fields_offsets_t in_key_offsets;
    bcmltm_wb_lta_fields_offsets_t out_offsets;
    uint32_t pt_sid_override_offset = BCMLTM_WB_OFFSET_INVALID;
    uint32_t pt_sid_selector_offset = BCMLTM_WB_OFFSET_INVALID;
    uint32_t transform_suppress_offset = BCMLTM_WB_OFFSET_INVALID;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(params, sizeof(bcmltm_lta_transform_params_t),
              "bcmltmLtaTransformParams");
    SHR_NULL_CHECK(params, SHR_E_MEMORY);
    sal_memset(params, 0, sizeof(bcmltm_lta_transform_params_t));

    
    SHR_ALLOC(params->lta_args, sizeof(bcmltd_transform_arg_t),
              "bcmltmLtaTransformArg");
    SHR_NULL_CHECK(params->lta_args, SHR_E_MEMORY);
    sal_memset(params->lta_args, 0, sizeof(bcmltd_transform_arg_t));

    /* Get LTA input/output offsets */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lta_fields_offsets_get(lta_wb_block,
                                          BCMLTM_WB_LTA_INPUT_FIELDS,
                                          &in_offsets));
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lta_fields_offsets_get(lta_wb_block,
                                          BCMLTM_WB_LTA_INPUT_KEY_FIELDS,
                                          &in_key_offsets));
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lta_fields_offsets_get(lta_wb_block,
                                          BCMLTM_WB_LTA_OUTPUT_FIELDS,
                                          &out_offsets));

    /* Get PT table select offsets from PTM WB */
    if (xfrm_handler->arg->tables != 0) {
        /* TABLE_SEL are expected only on key transforms */
        if ((xfrm_type == BCMLTM_DB_XFRM_TYPE_FWD_KEY_TRANSFORM) ||
            (xfrm_type == BCMLTM_DB_XFRM_TYPE_REV_KEY_TRANSFORM)) {
            uint16_t pt_entry_idx;

            SHR_IF_ERR_EXIT
                (dst_field_entry_idx_get(xfrm_handler, xfrm_type,
                                         BCMLRD_FIELD_TABLE_SEL,
                                         &pt_entry_idx));
            SHR_IF_ERR_EXIT
                (bcmltm_wb_ptm_pt_sid_override_offset_get(ptm_wb_block,
                                                          pt_entry_idx,
                                                      &pt_sid_override_offset));
            SHR_IF_ERR_EXIT
                (bcmltm_wb_ptm_field_lt_offset_get(ptm_wb_block,
                                                   BCMLRD_FIELD_TABLE_SEL,
                                                   BCMLTM_FIELD_TYPE_KEY,
                                                   BCMLTM_FIELD_DIR_IN,
                                                   pt_entry_idx,
                                                   &pt_sid_selector_offset));
        }
    }

    /* Get transform suppress offset */
    SHR_IF_ERR_EXIT
        (transform_suppress_offset_get(unit,
                                       sid,
                                       attr,
                                       lt_map,
                                       xfrm,
                                       xfrm_type,
                                       &transform_suppress_offset));

    /* Fill data */
    params->lta_transform = xfrm_handler->transform;
    params->lta_ext_transform = xfrm_handler->ext_transform;
    params->fields_input_offset = in_offsets.param;
    params->fields_input_key_offset = in_key_offsets.param;
    params->fields_output_offset = out_offsets.param;
    params->pt_sid_selector_offset = pt_sid_selector_offset;
    params->pt_sid_override_offset = pt_sid_override_offset;
    params->transform_suppress_offset = transform_suppress_offset;
    params->select_map_index_offset = BCMLTM_WB_OFFSET_INVALID;
    params->select_map_index = 0;
    sal_memcpy(params->lta_args, xfrm_handler->arg, sizeof(*params->lta_args));

    *params_ptr = params;

 exit:
    if (SHR_FUNC_ERR()) {
        transform_params_destroy(params);
        *params_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the list of field maps metadata.
 *
 * Destroy the list of field maps metadata.
 *
 * \param [in] field_maps Field maps metadata to destroy.
 */
static void
field_maps_destroy(bcmltm_lta_field_map_t *field_maps)
{
    SHR_FREE(field_maps);

    return;
}

/*!
 * \brief Create the list of transform source field maps metadata.
 *
 * Create the list of field maps metadata for the LTA Transform
 * SOURCE fields for given LT and transform.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] xfrm_type Transform type.
 * \param [in] field_type LTA field type.
 * \param [in] xfrm Transform map information.
 * \param [in] lta_wb_block LTA transform working buffer block.
 * \param [out] field_maps_ptr Returning pointer to list of field maps.
 * \param [out] num_fields Number of field maps (fid,idx) returned in list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_src_field_maps_create(int unit,
                           bcmlrd_sid_t sid, int xfrm_type,
                           int field_type,
                           const bcmltm_map_xfrm_t *xfrm,
                           const bcmltm_wb_block_t *lta_wb_block,
                           bcmltm_lta_field_map_t **field_maps_ptr,
                           uint32_t *num_fields)
{
    bcmltm_lta_field_map_t *field_maps = NULL;
    unsigned int size;
    uint32_t map_idx;
    uint32_t xfrm_idx;
    uint32_t fid;           /* Source (logical) field ID */
    uint32_t fid_count;     /* Number of source field IDs */
    uint32_t total_fields;  /* Total source fields elements (fid, idx) */
    const bcmltd_transform_arg_t *xfrm_arg = xfrm->handler.arg;
    uint32_t idx;
    uint32_t idx_count;
    uint32_t offset;
    uint64_t *default_value = NULL;
    uint32_t default_count;
    uint64_t *minimum_value = NULL;
    uint32_t minimum_count;
    uint64_t *maximum_value = NULL;
    uint32_t maximum_count;
    uint32_t field_flags;

    SHR_FUNC_ENTER(unit);

    *num_fields = 0;
    *field_maps_ptr = NULL;

    SHR_IF_ERR_EXIT
        (src_fields_count_get(unit, sid, xfrm_type, xfrm_arg, field_type,
                              &fid_count, &total_fields));

    if (total_fields == 0) {
        *num_fields = total_fields;
        *field_maps_ptr = field_maps;
        SHR_EXIT();
    }

    /* Allocate field_map */
    size = sizeof(*field_maps) * total_fields;
    SHR_ALLOC(field_maps, size, "bcmltmXfrmLtaFieldMaps");
    SHR_NULL_CHECK(field_maps, SHR_E_MEMORY);
    sal_memset(field_maps, 0, size);

    /* Fill data */
    for (map_idx = 0, xfrm_idx = 0; xfrm_idx < fid_count; xfrm_idx++) {
        fid = (IS_FTYPE_IN_KEY(field_type)) ?
              XFRM_SRC_KEY_FID(xfrm_arg, xfrm_idx) :
              XFRM_SRC_FID(xfrm_arg, xfrm_type, xfrm_idx);

        /* Get number of field indices */
        idx_count = bcmlrd_field_idx_count_get(unit, sid, fid);
        if (idx_count == 0) {
            continue;    /* Skip unmapped fields */
        }

        /* Get SW default value for field */
        SHR_ALLOC(default_value, sizeof(*default_value) * idx_count,
                  "bcmltmXfrmFieldDefVals");
        SHR_NULL_CHECK(default_value, SHR_E_MEMORY);
        sal_memset(default_value, 0, sizeof(*default_value) * idx_count);
        SHR_IF_ERR_EXIT
            (bcmlrd_field_default_get(unit, sid, fid,
                                      idx_count,
                                      default_value,
                                      &default_count));
        if (idx_count != default_count) {
            const char *table_name;
            const char *field_name;

            table_name = bcmltm_lt_table_sid_to_name(unit, sid);
            field_name = bcmltm_lt_field_fid_to_name(unit, sid, fid);
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Invalid index count for field defaults: "
                                  "%s(ltid=%d) %s(fid=%d) "
                                  "expected=%d actual=%d\n"),
                       table_name, sid, field_name, fid,
                       idx_count, default_count));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        /* Get SW minimum value for field */
        SHR_ALLOC(minimum_value, sizeof(*minimum_value) * idx_count,
                  "bcmltmXfrmFieldMinVals");
        SHR_NULL_CHECK(minimum_value, SHR_E_MEMORY);
        sal_memset(minimum_value, 0, sizeof(*minimum_value) * idx_count);
        SHR_IF_ERR_EXIT
            (bcmlrd_field_min_get(unit, sid, fid,
                                  idx_count,
                                  minimum_value,
                                  &minimum_count));
        if (idx_count != minimum_count) {
            const char *table_name;
            const char *field_name;

            table_name = bcmltm_lt_table_sid_to_name(unit, sid);
            field_name = bcmltm_lt_field_fid_to_name(unit, sid, fid);
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Invalid index count for field minimums: "
                                  "%s(ltid=%d) %s(fid=%d) "
                                  "expected=%d actual=%d\n"),
                       table_name, sid, field_name, fid,
                       idx_count, minimum_count));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        /* Get SW maximum value for field */
        SHR_ALLOC(maximum_value, sizeof(*maximum_value) * idx_count,
                  "bcmltmXfrmFieldMaxVals");
        SHR_NULL_CHECK(maximum_value, SHR_E_MEMORY);
        sal_memset(maximum_value, 0, sizeof(*maximum_value) * idx_count);
        SHR_IF_ERR_EXIT
            (bcmlrd_field_max_get(unit, sid, fid,
                                  idx_count,
                                  maximum_value,
                                  &maximum_count));
        if (idx_count != maximum_count) {
            const char *table_name;
            const char *field_name;

            table_name = bcmltm_lt_table_sid_to_name(unit, sid);
            field_name = bcmltm_lt_field_fid_to_name(unit, sid, fid);
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Invalid index count for field maximums: "
                                  "%s(ltid=%d) %s(fid=%d) "
                                  "expected=%d actual=%d\n"),
                       table_name, sid, field_name, fid,
                       idx_count, maximum_count));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        /* Get field properties */
        SHR_IF_ERR_EXIT
            (bcmltm_db_field_properties_get(unit, sid, fid, &field_flags));

        for (idx = 0; idx < idx_count; idx++) {
            if (map_idx >= total_fields) {
                const char *table_name;

                table_name = bcmltm_lt_table_sid_to_name(unit, sid);
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "LTM Transform Field Map: current "
                                      "src field count exceeds max count "
                                      "%s(ltid=%d) "
                                      "count=%d max_count=%d\n"),
                           table_name, sid,
                           (map_idx+1), total_fields));
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }

            /* Get field offset */
            SHR_IF_ERR_EXIT
                (bcmltm_wb_lta_field_map_offset_get(lta_wb_block, field_type,
                                                    map_idx, &offset));

            field_maps[map_idx].api_field_id = fid;
            field_maps[map_idx].field_idx = idx;
            field_maps[map_idx].field_offset = offset;
            field_maps[map_idx].default_value = default_value[idx];
            field_maps[map_idx].minimum_value = minimum_value[idx];
            field_maps[map_idx].maximum_value = maximum_value[idx];
            field_maps[map_idx].flags = field_flags;

            map_idx++;
        }

        SHR_FREE(maximum_value);
        SHR_FREE(minimum_value);
        SHR_FREE(default_value);
    }

    *num_fields = total_fields;
    *field_maps_ptr = field_maps;

 exit:
    if (SHR_FUNC_ERR()) {
        field_maps_destroy(field_maps);
        *num_fields = 0;
        *field_maps_ptr = NULL;
    }

    SHR_FREE(maximum_value);
    SHR_FREE(minimum_value);
    SHR_FREE(default_value);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the list of transform destination field maps metadata.
 *
 * Create the list of field maps metadata for the LTA Transform
 * DESTINATION fields for given LT and transform.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in] xfrm_type Transform type.
 * \param [in] field_type LTA field type.
 * \param [in] xfrm Transform map information.
 * \param [in] lta_wb_block LTA transform working buffer block.
 * \param [in] ptm_wb_block PTM working buffer block.
 * \param [out] field_maps_ptr Returning pointer to list of field maps.
 * \param [out] num_fields Number of field maps (fid,idx) returned in list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_dst_field_maps_create(int unit,
                           bcmlrd_sid_t sid,
                           const bcmltm_table_attr_t *attr,
                           int xfrm_type,
                           int field_type,
                           const bcmltm_map_xfrm_t *xfrm,
                           const bcmltm_wb_block_t *lta_wb_block,
                           const bcmltm_wb_block_t *ptm_wb_block,
                           bcmltm_lta_field_map_t **field_maps_ptr,
                           uint32_t *num_fields)
{
    bcmltm_lta_field_map_t *field_maps = NULL;
    const bcmltm_xfrm_dst_fields_t *xfrm_dst = &xfrm->dst;
    unsigned int size;
    bcmdrd_sid_t ptid;
    bcmltm_xfrm_dst_field_t *dst_field;
    uint32_t map_idx;
    uint32_t dst_idx;
    uint32_t fid;           /* Destination (physical) field ID */
    uint32_t fid_count;     /* Number of dst field IDs */
    uint32_t total_fields;  /* Total dst fields elements (fid, idx) */
    const bcmltd_transform_arg_t *xfrm_arg = xfrm->handler.arg;
    const bcmlrd_map_t *map = NULL;
    uint32_t idx;
    uint32_t idx_count;
    uint16_t fidx_minbit[MAX_FIELD_IDXS];
    uint16_t fidx_maxbit[MAX_FIELD_IDXS];
    uint32_t offset;
    uint32_t pt_changed_offset;
    bcmltm_field_type_t lt_field_type;
    uint32_t field_flags = 0x0;

    SHR_FUNC_ENTER(unit);

    *num_fields = 0;
    *field_maps_ptr = NULL;

    /*
     * Get field properties.
     *
     * Destination field is PT field.  The only
     * indicator for the PT field properties comes from the transform type.
     */
    if ((xfrm_type == BCMLTM_DB_XFRM_TYPE_FWD_KEY_TRANSFORM) ||
        (xfrm_type == BCMLTM_DB_XFRM_TYPE_REV_KEY_TRANSFORM)) {
        field_flags |= BCMLTM_FIELD_KEY;
        lt_field_type = BCMLTM_FIELD_TYPE_KEY;
    } else {
        lt_field_type = BCMLTM_FIELD_TYPE_VALUE;
    }

    SHR_IF_ERR_EXIT
        (dst_fields_count_get(unit, sid, xfrm_type, xfrm_arg, xfrm_dst,
                              &fid_count, &total_fields));

    if (total_fields == 0) {
        SHR_EXIT();
    }

    /* Allocate field_map */
    size = sizeof(*field_maps) * total_fields;
    SHR_ALLOC(field_maps, size, "bcmltmXfrmLtaFieldMaps");
    SHR_NULL_CHECK(field_maps, SHR_E_MEMORY);
    sal_memset(field_maps, 0, size);

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));

    /* Fill data */
    for (map_idx = 0, dst_idx = 0; dst_idx < xfrm_dst->num; dst_idx++) {
        ptid = xfrm_dst->ptid;
        dst_field = &xfrm_dst->field[dst_idx];
        fid = dst_field->field_id;

        /* Get PT changed offset */
        SHR_IF_ERR_EXIT
            (bcmltm_wb_ptm_pt_changed_offset_get(attr->type,
                                                 lt_field_type,
                                                 ptm_wb_block,
                                                 dst_field->pt_entry_idx,
                                                 &pt_changed_offset));
        SHR_IF_ERR_EXIT
            (bcmltm_db_field_to_fidx_min_max(unit, fid,
                                             dst_field->wbc.minbit,
                                             dst_field->wbc.maxbit,
                                             MAX_FIELD_IDXS,
                                             fidx_minbit, fidx_maxbit,
                                             &idx_count));

        /* All fields has at least 1 index count */
        if (idx_count < 1) {
            const char *table_name;
            const char *pt_name;
            const char *pt_field_name;

            table_name = bcmltm_lt_table_sid_to_name(unit, sid);
            pt_name = bcmltm_pt_table_sid_to_name(unit, ptid);
            pt_field_name = bcmltm_pt_field_fid_to_name(unit, ptid, fid);
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "LTM Transform Destination Field Map: "
                                  "invalid field index count (should be > 0) "
                                  "%s(ltid=%d) %s(ptid=%d) "
                                  "%s(fid=%d) count=%d\n"),
                       table_name, sid, pt_name, ptid,
                       pt_field_name, fid, idx_count));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        /* Fill data for each field index element */
        for (idx = 0; idx < idx_count; idx++) {
            if (map_idx >= total_fields) {
                const char *table_name;

                table_name = bcmltm_lt_table_sid_to_name(unit, sid);
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "LTM Transform Destination Field Map: "
                                      "current field count exceeds max count "
                                      "%s(ltid=%d) "
                                      "count=%d max_count=%d\n"),
                           table_name, sid, (map_idx+1), total_fields));
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }

            /* Get field offset */
            SHR_IF_ERR_EXIT
                (bcmltm_wb_lta_field_map_offset_get(lta_wb_block,
                                                    field_type,
                                                    map_idx, &offset));

            field_maps[map_idx].api_field_id = fid;
            field_maps[map_idx].field_idx = idx;
            field_maps[map_idx].field_offset = offset;

            field_maps[map_idx].pt_changed_offset = pt_changed_offset;
            field_maps[map_idx].flags = field_flags;

            field_maps[map_idx].wbc.buffer_offset =
                dst_field->wbc.buffer_offset;
            field_maps[map_idx].wbc.maxbit = fidx_maxbit[idx];
            field_maps[map_idx].wbc.minbit = fidx_minbit[idx];

            map_idx++;
        }
    }

    *num_fields = total_fields;
    *field_maps_ptr = field_maps;

 exit:
    if (SHR_FUNC_ERR()) {
        field_maps_destroy(field_maps);
        *num_fields = 0;
        *field_maps_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the list of field maps metadata.
 *
 * Create the list of field maps metadata for either the
 * LTA Source or Destination field list for given LT and transform.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in] xfrm_type Transform type.
 * \param [in] xfrm Transform map information.
 * \param [in] lta_wb_block LTA transform working buffer block.
 * \param [in] ptm_wb_block PTM working buffer block.
 * \param [in] field_type Indicates input or output field list.
 * \param [out] field_maps_ptr Returning pointer to list of field maps.
 * \param [out] num_fields Number of field maps (fid,idx) returned in list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_field_maps_create(int unit,
                       bcmlrd_sid_t sid,
                       const bcmltm_table_attr_t *attr,
                       int xfrm_type,
                       const bcmltm_map_xfrm_t *xfrm,
                       const bcmltm_wb_block_t *lta_wb_block,
                       const bcmltm_wb_block_t *ptm_wb_block,
                       int field_type,
                       bcmltm_lta_field_map_t **field_maps_ptr,
                       uint32_t *num_fields)
{
    /*
     *  +--------------------------------+------------------+------------------+
     *  | field_type                     | Forward          | Reverse          |
     *  +--------------------------------+------------------+------------------+
     *  | BCMLTM_WB_LTA_INPUT_FIELDS     | arg->field (src) | arg->field(dst)  |
     *  | BCMLTM_WB_LTA_INPUT_KEY_FIELDS | arg->kfield(src) | arg->kfield(src) |
     *  | BCMLTM_WB_LTA_OUTPUT_FIELDS    | arg->rfield(dst) | arg->rfield(src) |
     *  +--------------------------------+------------------+------------------+
     */
    int rv = SHR_E_NONE;
    int is_src = 0;

    if ((IS_FTYPE_IN_KEY(field_type)) ||
        (IS_FTYPE_IN(field_type) && IS_XTYPE_FWD(xfrm_type)) ||
        (IS_FTYPE_OUT(field_type) && !IS_XTYPE_FWD(xfrm_type))) {
        is_src = 1;
    }

    if (is_src) {
        rv = xfrm_src_field_maps_create(unit, sid, xfrm_type, field_type, xfrm,
                                        lta_wb_block, field_maps_ptr,
                                        num_fields);
    } else {
        rv = xfrm_dst_field_maps_create(unit, sid, attr,
                                        xfrm_type, field_type, xfrm,
                                        lta_wb_block, ptm_wb_block,
                                        field_maps_ptr,
                                        num_fields);
    }

    return rv;
}

/*!
 * \brief Create the LTA XFRM field list metadata for given table.
 *
 * Create the LTA Custom Table Handler field list metadata for given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in] lt_map LT Map information.
 * \param [in] xfrm_type Transform type.
 * \param [in] xfrm Transform map information.
 * \param [in] lta_wb_block LTA transform working buffer block.
 * \param [in] ptm_wb_block PTM working buffer block.
 * \param [in] field_type Indicates input or output field list.
 * \param [out] flist Returning pointer for field list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_field_list_create(int unit,
                       bcmlrd_sid_t sid,
                       const bcmltm_table_attr_t *attr,
                       const bcmltm_db_dm_map_entries_t *lt_map,
                       int xfrm_type,
                       const bcmltm_map_xfrm_t *xfrm,
                       const bcmltm_wb_block_t *lta_wb_block,
                       const bcmltm_wb_block_t *ptm_wb_block,
                       int field_type,
                       bcmltm_lta_field_list_t **flist_ptr)
{
    bcmltm_lta_field_list_t *flist = NULL;
    bcmltm_wb_lta_fields_offsets_t offsets;
    uint32_t num_fields;
    uint32_t api_fields_offset = BCMLTM_WB_OFFSET_INVALID;
    uint32_t index_absent_offset = BCMLTM_WB_OFFSET_INVALID;
    uint32_t pt_suppress_offset = BCMLTM_WB_OFFSET_INVALID;
    bool alloc;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(flist, sizeof(*flist), "bcmltmXfrmLtaFieldList");
    SHR_NULL_CHECK(flist, SHR_E_MEMORY);
    sal_memset(flist, 0, sizeof(*flist));

    /* Get fields offset */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lta_fields_offsets_get(lta_wb_block, field_type, &offsets));

    /*
     * For forward transform, the API input list is always the LTA input.
     * For reverse transform, there is no API input list.
     */
    if ((xfrm_type == BCMLTM_DB_XFRM_TYPE_FWD_KEY_TRANSFORM) ||
        (xfrm_type == BCMLTM_DB_XFRM_TYPE_FWD_VALUE_TRANSFORM)) {
        if (field_type == BCMLTM_WB_LTA_OUTPUT_FIELDS) {
            bcmltm_wb_lta_fields_offsets_t in_offsets;

            SHR_IF_ERR_EXIT
                (bcmltm_wb_lta_fields_offsets_get(lta_wb_block,
                                                  BCMLTM_WB_LTA_INPUT_FIELDS,
                                                  &in_offsets));
            api_fields_offset = in_offsets.param;
        } else {
            api_fields_offset = offsets.param;
        }
    }

    /* Get index absent offset */
    alloc = bcmltm_db_xfrm_key_is_alloc(attr, xfrm_type, xfrm->handler.arg);
    if (alloc) {
        SHR_IF_ERR_EXIT
            (bcmltm_wb_lt_index_absent_offset_get_by_sid(unit, sid,
                                                         &index_absent_offset));
    }

    /* Get PT suppress offset */
    SHR_IF_ERR_EXIT
        (xfrm_pt_suppress_offset_get(unit, sid,
                                     xfrm->entry_type,
                                     lt_map->pt_suppress,
                                     &pt_suppress_offset));

    /* Fill data */
    flist->fields_parameter_offset = offsets.param;
    flist->fields_ptrs_offset = offsets.ptrs;
    flist->api_fields_parameter_offset = api_fields_offset;
    flist->target_fields_parameter_offset = BCMLTM_WB_OFFSET_INVALID;
    flist->index_absent_offset = index_absent_offset;
    flist->pt_suppress_offset = pt_suppress_offset;

    /* Get field maps */
    SHR_IF_ERR_EXIT
        (xfrm_field_maps_create(unit, sid, attr,
                                xfrm_type, xfrm,
                                lta_wb_block, ptm_wb_block, field_type,
                                &flist->field_maps, &num_fields));
    flist->max_fields = num_fields;
    flist->num_maps = num_fields;

    *flist_ptr = flist;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_lta_field_list_destroy(flist);
        *flist_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy information for Field Transform list.
 *
 * Destroy given Field Transform information list.
 *
 * \param [in] info Pointer to info list to destroy.
 */
static void
xfrm_list_destroy(bcmltm_db_xfrm_list_t *list)
{
    bcmltm_db_xfrm_t *xfrm = NULL;
    uint32_t idx;

    if (list == NULL) {
        return;
    }

    for (idx = 0; idx < list->num_xfrms; ++idx) {
        xfrm = &list->xfrms[idx];

        /* Destroy XFRM Field lists */
        bcmltm_db_lta_field_list_destroy(xfrm->fout);
        bcmltm_db_lta_field_list_destroy(xfrm->fin_key);
        bcmltm_db_lta_field_list_destroy(xfrm->fin);

        /* Destroy XFRM table params */
        transform_params_destroy(xfrm->xfrm_params);
    }

    SHR_FREE(list->xfrms);
    SHR_FREE(list);

    return;
}

/*!
 * \brief Create information for list of Field Transforms.
 *
 * Create information for list of Field Transforms for the specified
 * logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in] xfrm_type Field Transform Type : Forward or Reverse Transform.
 * \param [in] map_entries Map information.
 * \param [in] pt_info PT information.
 * \param [out] list_ptr Returning pointer to info list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_list_create(int unit, bcmlrd_sid_t sid,
                 const bcmltm_table_attr_t *attr,
                 int xfrm_type,
                 bcmltm_db_dm_map_entries_t *map_entries,
                 const bcmltm_db_dm_pt_info_t *pt_info,
                 bcmltm_db_xfrm_list_t **list_ptr)
{
    bcmltm_db_xfrm_list_t *list = NULL;
    bcmltm_map_field_xfrm_list_t *map_xfrm_list;
    bcmltm_map_xfrm_t *map_xfrm;
    uint32_t num_xfrms = 0;
    uint32_t size = 0;
    bcmltm_db_xfrm_t xfrm;
    bcmltd_xfrm_handler_t *xfrm_handler = NULL;
    bcmltm_xfrm_dst_fields_t *xfrm_dst = NULL;
    uint32_t idx = 0;
    uint32_t src_fid_count;
    uint32_t src_key_fid_count;
    uint32_t dst_fid_count;
    uint32_t src_total_fields;  /* Total fields elements (fid, idx) */
    uint32_t src_key_total_fields;  /* Total fields elements (fid, idx) */
    uint32_t dst_total_fields;  /* Total fields elements (fid, idx) */
    bcmltm_wb_block_id_t wb_block_id;
    bcmltm_wb_block_t *wb_block = NULL;
    bcmltm_wb_handle_t *wb_handle = NULL;
    uint32_t in_total_fields;  /* Total fields elements (fid, idx) */
    uint32_t in_key_total_fields;  /* Total fields elements (fid, idx) */
    uint32_t out_total_fields;  /* Total fields elements (fid, idx) */
    bcmdrd_sid_t ptid;
    const bcmltm_wb_block_t *ptm_wb_block;

    SHR_FUNC_ENTER(unit);

    if (xfrm_type == BCMLTM_DB_XFRM_TYPE_FWD_KEY_TRANSFORM) {
        map_xfrm_list = &map_entries->fwd_key_xfrms;
    } else if (xfrm_type == BCMLTM_DB_XFRM_TYPE_FWD_VALUE_TRANSFORM) {
        map_xfrm_list = &map_entries->fwd_value_xfrms;
    } else if (xfrm_type == BCMLTM_DB_XFRM_TYPE_REV_KEY_TRANSFORM) {
        map_xfrm_list = &map_entries->rev_key_xfrms;
    } else {
        map_xfrm_list = &map_entries->rev_value_xfrms;
    }

    /* Get Working Buffer handle */
    wb_handle = bcmltm_wb_lt_handle_get(unit, sid);
    if (wb_handle == NULL) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM null working buffer handle: "
                              "%s(ltid=%d)\n"),
                   table_name, sid));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    num_xfrms = map_xfrm_list->num;

    /* Allocate */
    SHR_ALLOC(list , sizeof(*list), "bcmltmDbXfrmList");
    SHR_NULL_CHECK(list, SHR_E_MEMORY);
    sal_memset(list, 0, sizeof(*list));
    if (num_xfrms == 0) {
        list->num_xfrms = num_xfrms;
        *list_ptr = list;
        SHR_FUNC_EXIT();
    }

    if (map_xfrm_list->xfrms == NULL) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Unexpected empty transform map list: "
                              "%s(ltid=%d) expected_num_xfrms=%d\n"),
                   table_name, sid, num_xfrms));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    size = sizeof(bcmltm_db_xfrm_t) * num_xfrms;
    SHR_ALLOC(list->xfrms , size, "bcmltmDbXfrms");
    SHR_NULL_CHECK(list->xfrms, SHR_E_MEMORY);
    sal_memset(list->xfrms, 0, size);

    /* Allocate */
    for (idx = 0; idx < num_xfrms; idx++) {
        map_xfrm = &map_xfrm_list->xfrms[idx];
        /* Obtain transform arg pointer */
        xfrm_handler = &map_xfrm->handler;
        xfrm_dst = &map_xfrm->dst;

        sal_memset(&xfrm, 0, sizeof(xfrm));

        SHR_IF_ERR_EXIT
            (src_fields_count_get(unit, sid, xfrm_type, xfrm_handler->arg,
                                  BCMLTM_WB_LTA_INPUT_FIELDS,
                                  &src_fid_count, &src_total_fields));
        SHR_IF_ERR_EXIT
            (src_fields_count_get(unit, sid, xfrm_type, xfrm_handler->arg,
                                  BCMLTM_WB_LTA_INPUT_KEY_FIELDS,
                                  &src_key_fid_count, &src_key_total_fields));

        SHR_IF_ERR_EXIT
            (dst_fields_count_get(unit, sid, xfrm_type, xfrm_handler->arg,
                                  xfrm_dst,
                                  &dst_fid_count, &dst_total_fields));

        /*
         * Forward Transform:  Input are Source fields
         *                     Output are Destination fields
         *
         * Reverse Transform:  Input are Destination fields
         *                     Output are Source fields
         */
        if ((xfrm_type == BCMLTM_DB_XFRM_TYPE_FWD_KEY_TRANSFORM) ||
            (xfrm_type == BCMLTM_DB_XFRM_TYPE_FWD_VALUE_TRANSFORM)) {
            in_total_fields = src_total_fields;
            in_key_total_fields = src_key_total_fields;
            out_total_fields = dst_total_fields;
        } else {
            in_total_fields = dst_total_fields;
            in_key_total_fields = src_key_total_fields;
            out_total_fields = src_total_fields;
        }
        /* Add Working Buffer block */
        SHR_IF_ERR_EXIT
            (bcmltm_wb_block_lta_xfrm_add(unit,
                                          in_total_fields,
                                          in_key_total_fields,
                                          out_total_fields,
                                          wb_handle, &wb_block_id, &wb_block));
        xfrm.wb_block_id = wb_block_id;

        ptid = xfrm_dst->ptid;

        ptm_wb_block = bcmltm_db_dm_ptm_wb_block_find(unit, sid,
                                                      pt_info, ptid);

        /*
         * Create Field Transform Handler table params
         * (used as LTA FA node cookies for calling transform).
         */
        SHR_IF_ERR_EXIT
            (transform_params_create(unit, sid, attr, map_entries,
                                     xfrm_type, map_xfrm,
                                     wb_block, ptm_wb_block,
                                     &xfrm.xfrm_params));

        /*
         * Create Field Transform Handler Field List Input/Output
         * (used as LTA FA node cookies).
         */
        SHR_IF_ERR_EXIT
            (xfrm_field_list_create(unit, sid, attr,
                                    map_entries,
                                    xfrm_type, map_xfrm,
                                    wb_block, ptm_wb_block,
                                    BCMLTM_WB_LTA_INPUT_FIELDS,
                                    &xfrm.fin));
        SHR_IF_ERR_EXIT
            (xfrm_field_list_create(unit, sid, attr,
                                    map_entries,
                                    xfrm_type, map_xfrm,
                                    wb_block, ptm_wb_block,
                                    BCMLTM_WB_LTA_INPUT_KEY_FIELDS,
                                    &xfrm.fin_key));
        SHR_IF_ERR_EXIT
            (xfrm_field_list_create(unit, sid, attr,
                                    map_entries,
                                    xfrm_type, map_xfrm,
                                    wb_block, ptm_wb_block,
                                    BCMLTM_WB_LTA_OUTPUT_FIELDS,
                                    &xfrm.fout));

        sal_memcpy(&list->xfrms[idx], &xfrm, sizeof(xfrm));
    }

    list->num_xfrms = num_xfrms;
    *list_ptr = list;

 exit:
    if (SHR_FUNC_ERR()) {
        xfrm_list_destroy(list);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create LTA-LTA copy info for Field Transforms for given table.
 *
 * Create the LTA-LTA copy list for Field Transforms needed by the
 * LTM metadata for LTA Field Transform Handler.
 * These are the reverse transform output to forward transform input
 * LTA field copy nodes used by the UPDATE opcode.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] from_list Node cookie list of reverse transforms from
 *                       which to copy field values in output list.
 * \param [in] to_list Node cookie list of forward transforms to
 *                     which to copy field values into input list.
 * \param [in] is_key The flag to indicate the destination is in_key.
 * \param [out] info_ptr Returning pointer to LT info for xfrm copy list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_copy_list_create(int unit, bcmlrd_sid_t sid,
                      bcmltm_db_xfrm_list_t *from_list,
                      bcmltm_db_xfrm_list_t *to_list,
                      bool is_inkey,
                      bcmltm_db_xfrm_list_t **list_ptr)
{
    bcmltm_db_xfrm_list_t *list = NULL;
    bcmltm_lta_field_list_t *to_field = NULL;
    uint32_t to_num_xfrms = 0;
    uint32_t from_num_xfrms = 0;
    uint32_t from_xfrm_idx;
    uint32_t to_xfrm_idx;
    uint32_t size = 0;
    bcmltm_db_xfrm_t xfrm;

    SHR_FUNC_ENTER(unit);

    to_num_xfrms = to_list->num_xfrms;
    from_num_xfrms = from_list->num_xfrms;

    /* Allocate */
    SHR_ALLOC(list, sizeof(*list), "bcmltmDbXfrmList");
    SHR_NULL_CHECK(list, SHR_E_MEMORY);
    sal_memset(list, 0, sizeof(*list));
    if ((to_num_xfrms * from_num_xfrms) == 0) {
        list->num_xfrms = 0;
        *list_ptr = list;
        SHR_FUNC_EXIT();
    }

    size = sizeof(bcmltm_db_xfrm_t) * to_num_xfrms * from_num_xfrms;
    SHR_ALLOC(list->xfrms, size, "bcmltmDbXfrms");
    SHR_NULL_CHECK(list->xfrms, SHR_E_MEMORY);
    sal_memset(list->xfrms, 0, size);

    sal_memset(&xfrm, 0, sizeof(xfrm));

    /* Allocate */
    for (to_xfrm_idx = 0; to_xfrm_idx < to_num_xfrms; to_xfrm_idx++) {
        for (from_xfrm_idx = 0; from_xfrm_idx < from_num_xfrms;
             from_xfrm_idx++) {
            to_field = (is_inkey) ? (to_list->xfrms[to_xfrm_idx].fin_key) :
                                    (to_list->xfrms[to_xfrm_idx].fin);
            if (to_field->num_maps == 0) {
                continue;
            }
            SHR_IF_ERR_EXIT
                (bcmltm_db_copy_lta_field_list_create(unit, sid,
                           from_list->xfrms[from_xfrm_idx].fout,
                           to_field,
                           &(xfrm.fin)));

            sal_memcpy(&(list->xfrms[(to_xfrm_idx * from_num_xfrms) +
                                     from_xfrm_idx]),
                       &xfrm, sizeof(xfrm));
        }
    }

    list->num_xfrms = to_num_xfrms * from_num_xfrms;
    *list_ptr = list;

 exit:
    if (SHR_FUNC_ERR()) {
        xfrm_list_destroy(list);
    }

    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

int
bcmltm_db_xfrm_info_create(int unit, bcmlrd_sid_t sid,
                           const bcmltm_table_attr_t *attr,
                           bcmltm_db_dm_map_entries_t *map_entries,
                           const bcmltm_db_dm_pt_info_t *pt_info,
                           bcmltm_db_xfrm_info_t **info_ptr)
{
    bcmltm_db_xfrm_info_t *info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(info, sizeof(*info), "bcmltmDbXfrmInfo");
    SHR_NULL_CHECK(info, SHR_E_MEMORY);
    sal_memset(info, 0, sizeof(*info));

    /* Create Forward Key Field Transform Information */
    SHR_IF_ERR_EXIT
        (xfrm_list_create(unit, sid, attr,
                          BCMLTM_DB_XFRM_TYPE_FWD_KEY_TRANSFORM,
                          map_entries, pt_info,
                          &info->fwd_key_xfrm_list));

    /* Create Forward Value Field Transform Information */
    SHR_IF_ERR_EXIT
        (xfrm_list_create(unit, sid, attr,
                          BCMLTM_DB_XFRM_TYPE_FWD_VALUE_TRANSFORM,
                          map_entries, pt_info,
                          &info->fwd_value_xfrm_list));

    /* Create Reverse Key Field Transform Information */
    SHR_IF_ERR_EXIT
        (xfrm_list_create(unit, sid, attr,
                          BCMLTM_DB_XFRM_TYPE_REV_KEY_TRANSFORM,
                          map_entries, pt_info,
                          &info->rev_key_xfrm_list));

    /* Create Reverse Value Field Transform Information */
    SHR_IF_ERR_EXIT
        (xfrm_list_create(unit, sid, attr,
                          BCMLTM_DB_XFRM_TYPE_REV_VALUE_TRANSFORM,
                          map_entries, pt_info,
                          &info->rev_value_xfrm_list));

    /* Create LTA Copy Field Information */
    SHR_IF_ERR_EXIT
        (xfrm_copy_list_create(unit, sid,
                               info->rev_key_xfrm_list,
                               info->fwd_key_xfrm_list,
                               0,
                               &info->rkey_to_fkey_xfrm_list));
    SHR_IF_ERR_EXIT
        (xfrm_copy_list_create(unit, sid,
                               info->rev_value_xfrm_list,
                               info->fwd_value_xfrm_list,
                               0,
                               &info->rvalue_to_fvalue_xfrm_list));
    SHR_IF_ERR_EXIT
        (xfrm_copy_list_create(unit, sid,
                               info->rev_key_xfrm_list,
                               info->rev_value_xfrm_list,
                               1,
                               &info->rkey_to_rvalue_xfrm_list));

    *info_ptr = info;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_xfrm_info_destroy(info);
        *info_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

void
bcmltm_db_xfrm_info_destroy(bcmltm_db_xfrm_info_t *info)
{
    if (info == NULL) {
        return;
    }

    xfrm_list_destroy(info->rkey_to_rvalue_xfrm_list);
    xfrm_list_destroy(info->rvalue_to_fvalue_xfrm_list);
    xfrm_list_destroy(info->rkey_to_fkey_xfrm_list);
    xfrm_list_destroy(info->rev_value_xfrm_list);
    xfrm_list_destroy(info->rev_key_xfrm_list);
    xfrm_list_destroy(info->fwd_value_xfrm_list);
    xfrm_list_destroy(info->fwd_key_xfrm_list);

    SHR_FREE(info);

    return;
}
