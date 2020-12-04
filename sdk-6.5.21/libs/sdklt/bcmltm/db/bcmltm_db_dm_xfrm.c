/*! \file bcmltm_db_dm_xfrm.c
 *
 * Logical Table Manager - Information for Field Transforms.
 *
 * This file contains routines to construct information for
 * field transforms with selection in direct mapped logical tables.
 *
 * The metadata created is used for the following LTM driver flow:
 * (field select opcode driver version)
 *     API <-> API Cache <-> [VALIDATE, XFRM] <-> PTM
 *
 * This driver flow can be applied to tables with or without field selections.
 *
 * The information is derived from the LRD Field Transform map entries
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
 *                  - INPUT for Forward and Reverse Value transform.
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
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>

#include <bcmltm/bcmltm_lta_intf_internal.h>
#include <bcmltm/bcmltm_types_internal.h>
#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm_db_core_internal.h>
#include <bcmltm/bcmltm_wb_lta_internal.h>
#include <bcmltm/bcmltm_wb_ptm_internal.h>
#include <bcmltm/bcmltm_wb_lt_internal.h>

#include "bcmltm_db_util.h"
#include "bcmltm_db_map.h"
#include "bcmltm_db_field_select.h"
#include "bcmltm_db_md_util.h"
#include "bcmltm_db_dm_util.h"
#include "bcmltm_db_dm_xfrm.h"

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/* Maximum number of field indexes (wide fields) in a field */
#define MAX_FIELD_IDXS           50

/*!
 * \brief Transform Field List Types.
 *
 * This indicates the type of field list in a transform.
 *
 * A field transform has the following field lists associated with it:
 *     Input fields
 *     Input key fields (valid only on value transforms)
 *     Output fields
 */
typedef enum bcmltm_xfrm_field_list_type_e {
    /*! Input field list. */
    BCMLTM_XFRM_FIELD_LIST_TYPE_IN,

    /*! Input key field list. */
    BCMLTM_XFRM_FIELD_LIST_TYPE_IN_KEY,

    /*! Output field list. */
    BCMLTM_XFRM_FIELD_LIST_TYPE_OUT,
} bcmltm_xfrm_field_list_type_t;

/*!
 * \brief Field Transform Arguments.
 *
 * This structure contains context information for
 * field transform and can be used to pass various data
 * and help reduce the number of input function arguments.
 */
typedef struct bcmltm_db_xfrm_arg_s {
    /*! Field transform type to process. */
    bcmltm_field_xfrm_type_t xfrm_type;

    /*! LRD map. */
    const bcmlrd_map_t *lrd_map;

    /*! Field select map index working buffer offset. */
    uint32_t select_map_index_offset;

    /*! Selector map index required to execute this transform. */
    uint32_t select_map_index;

    /*! Direct mapped table arguments. */
    const bcmltm_db_dm_arg_t *dm_arg;
} bcmltm_db_xfrm_arg_t;

/*!
 * \brief Field Transform Map Entry Arguments.
 *
 * This structure contains context information for a
 * field transform map entry and can be used to pass various data
 * and help reduce the number of input function arguments.
 */
typedef struct bcmltm_db_xfrm_me_arg_s {
    /*! LRD map group. */
    const bcmlrd_map_group_t *group;

    /*! LRD map entry. */
    const bcmlrd_map_entry_t *entry;

    /*! Field transform handler. */
    const bcmltd_xfrm_handler_t *lta_handler;

    /*! Field transform LTA working buffer block. */
    const bcmltm_wb_block_t *lta_wb_block;

    /*! Field transform PTM working buffer block. */
    const bcmltm_wb_block_t *ptm_wb_block;

    /*! Field transform arguments. */
    const bcmltm_db_xfrm_arg_t *xfrm_arg;
} bcmltm_db_xfrm_me_arg_t;

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Check if map entry is to be processed based on input criteria.
 *
 * This helper routine checks whether the given map entry is to be
 * processed or skipped based on the specified criteria.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] map LRD map.
 * \param [in] map_entry Map entry to check.
 * \param [in] xfrm_type Desired field transform type.
 *
 * \retval TRUE if map entry is to be processed based on criteria.
 * \retval FALSE if map entry is to be skipped, or failure.
 */
static bool
xfrm_map_entry_process(int unit,
                       bcmlrd_sid_t sid,
                       const bcmlrd_map_t *map,
                       const bcmltm_db_map_entry_t *map_entry,
                       bcmltm_field_xfrm_type_t xfrm_type)
{
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    bcmlrd_map_entry_type_t entry_type;

    /* Skip non physical map groups (non-DM) */
    group = bcmltm_db_map_lrd_group_get(map, map_entry);
    if (group->dest.kind != BCMLRD_MAP_PHYSICAL) {
        return FALSE;
    }

    /* Check transform type */
    entry = bcmltm_db_map_lrd_entry_get(map, map_entry);
    entry_type = entry->entry_type;

    /* Forward key transform */
    if ((xfrm_type == BCMLTM_FIELD_XFRM_TYPE_FWD_KEY) &&
        (entry_type == BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER)) {
        return TRUE;
    }

    /* Forward value transform */
    if ((xfrm_type == BCMLTM_FIELD_XFRM_TYPE_FWD_VALUE) &&
        (entry_type == BCMLRD_MAP_ENTRY_FWD_VALUE_FIELD_XFRM_HANDLER)) {
        return TRUE;
    }

    /* Reverse key transform */
    if ((xfrm_type == BCMLTM_FIELD_XFRM_TYPE_REV_KEY) &&
        (entry_type == BCMLRD_MAP_ENTRY_REV_KEY_FIELD_XFRM_HANDLER)) {
        return TRUE;
    }

    /* Reverse value transform */
    if ((xfrm_type == BCMLTM_FIELD_XFRM_TYPE_REV_VALUE) &&
        ((entry_type ==
          BCMLRD_MAP_ENTRY_REV_VALUE_FIELD_XFRM_HANDLER) ||
         (entry_type ==
          BCMLRD_MAP_ENTRY_ALWAYS_REV_VALUE_FIELD_XFRM_HANDLER))) {
        return TRUE;
    }

    /* No match */
    return FALSE;
}

/*!
 * \brief Get WB LTA field type for given transform field list type.
 *
 * This routine returns the corresponding working buffer
 * LTA field type for the specified transform field type.
 *
 * \param [in] list_type Transform field list type.
 *
 * \retval WB LTA field type for given transform field list type.
 */
static inline int
xfrm_wb_lta_field_type_get(bcmltm_xfrm_field_list_type_t list_type)
{
    int lta_field_type;

    /* Get field type for LTA wb */
    if (list_type == BCMLTM_XFRM_FIELD_LIST_TYPE_IN) {
        lta_field_type = BCMLTM_WB_LTA_INPUT_FIELDS;
    } else if (list_type == BCMLTM_XFRM_FIELD_LIST_TYPE_IN_KEY) {
        lta_field_type = BCMLTM_WB_LTA_INPUT_KEY_FIELDS;
    } else {
        lta_field_type = BCMLTM_WB_LTA_OUTPUT_FIELDS;
    }

    return lta_field_type;
}

/*!
 * \brief Get number of field id and idx count for source field list.
 *
 * This routine returns the number of field ids and total
 * number of field elements (fid,idx) for the requested transform
 * source field list.
 *
 * The source field list contains the API logical fields.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] field_list Transform source field list.
 * \param [out] num_fields Number of field IDs (fid).
 * \param [out] num_field_elems Number of field elements (fid,idx).
 */
static void
xfrm_src_field_count_get(int unit,
                         bcmlrd_sid_t sid,
                         const bcmltd_field_list_t *field_list,
                         uint32_t *num_fields,
                         uint32_t *num_field_elems)
{
    uint32_t fid;
    uint32_t i;
    uint32_t idx_count;

    *num_fields = *num_field_elems = 0;

    if (field_list == NULL) {
        return;
    }

    *num_fields = field_list->field_num;
    for (i = 0; i < *num_fields; i++) {
        fid = field_list->field_array[i].field_id;
        idx_count = bcmlrd_field_idx_count_get(unit, sid, fid);
        *num_field_elems += idx_count;
    }

    return;
}

/*!
 * \brief Get number of field id and idx count for destination field list.
 *
 * This routine returns the number of field ids and total
 * number of field elements (fid,idx) for the requested transform
 * destination field list.
 *
 * The destination field list contains the physical fields or any
 * other internal non-HW destination.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] field_list Transform destination field list.
 * \param [out] num_fields Number of field IDs (fid).
 * \param [out] num_field_elems Number of field elements (fid,idx).
 */
static void
xfrm_dst_field_count_get(int unit,
                         bcmlrd_sid_t sid,
                         const bcmltd_field_list_t *field_list,
                         uint32_t *num_fields,
                         uint32_t *num_field_elems)
{
    uint32_t i;
    uint32_t idx_count;
    uint32_t width;

    *num_fields = *num_field_elems = 0;

    if (field_list == NULL) {
        return;
    }

    *num_fields = field_list->field_num;
    for (i = 0; i < *num_fields; i++) {
        width = field_list->field_array[i].maxbit -
                field_list->field_array[i].minbit + 1;
        idx_count = bcmltd_field_idx_count_get(width);
        *num_field_elems += idx_count;
    }

    return;
}

/*!
 * \brief Get number of field id and idx count for input field list.
 *
 * This routine returns the number of field ids and total
 * number of field elements (fid,idx) for the input field list of
 * the requested transform.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] lta_arg Transform handler arguments.
 * \param [in] entry_type Map entry type.
 * \param [out] num_fields Number of field IDs (fid).
 * \param [out] num_field_elems Number of field elements (fid,idx).
 */
static void
xfrm_in_field_count_get(int unit,
                        bcmlrd_sid_t sid,
                        const bcmltd_transform_arg_t *lta_arg,
                        bcmlrd_map_entry_type_t entry_type,
                        uint32_t *num_fields,
                        uint32_t *num_field_elems)
{
    /*
     * Field counts are calculated differently based on type of fields,
     * whether logical or physical:
     *   Forward transform - input fields are source logical fields.
     *   Reverse transform - input fields are destination physical fields.
     */
    if (BCMLTM_DB_MAP_ENTRY_TYPE_IS_FWD_XFRM(entry_type)) {
        xfrm_src_field_count_get(unit, sid,
                                 lta_arg->field_list,
                                 num_fields,
                                 num_field_elems);
    } else {
        xfrm_dst_field_count_get(unit, sid,
                                 lta_arg->field_list,
                                 num_fields,
                                 num_field_elems);
    }

    return;
}

/*!
 * \brief Get number of field id and idx count for input key field list.
 *
 * This routine returns the number of field ids and total
 * number of field elements (fid,idx) for the input key field list of
 * the requested transform.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] lta_arg Transform handler arguments.
 * \param [out] num_fields Number of field IDs (fid).
 * \param [out] num_field_elems Number of field elements (fid,idx).
 */
static void
xfrm_in_key_field_count_get(int unit,
                            bcmlrd_sid_t sid,
                            const bcmltd_transform_arg_t *lta_arg,
                            uint32_t *num_fields,
                            uint32_t *num_field_elems)
{
    /* Input key fields are always source logical fields */
    xfrm_src_field_count_get(unit, sid,
                             lta_arg->kfield_list,
                             num_fields,
                             num_field_elems);
    return;
}

/*!
 * \brief Get number of field id and idx count for output field list.
 *
 * This routine returns the number of field ids and total
 * number of field elements (fid,idx) for the output field list of
 * the requested transform.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] lta_arg Transform handler arguments.
 * \param [in] entry_type Map entry type.
 * \param [out] num_fields Number of field IDs (fid).
 * \param [out] num_field_elems Number of field elements (fid,idx).
 */
static void
xfrm_out_field_count_get(int unit,
                         bcmlrd_sid_t sid,
                         const bcmltd_transform_arg_t *lta_arg,
                         bcmlrd_map_entry_type_t entry_type,
                         uint32_t *num_fields,
                         uint32_t *num_field_elems)
{
    /*
     * Field counts are calculated differently based on type of fields,
     * whether logical or physical:
     *   Forward transform - output fields are destination physical fields.
     *   Reverse transform - output fields are source logical fields.
     */
    if (BCMLTM_DB_MAP_ENTRY_TYPE_IS_FWD_XFRM(entry_type)) {
        xfrm_dst_field_count_get(unit, sid,
                                 lta_arg->rfield_list,
                                 num_fields,
                                 num_field_elems);
    } else {
        xfrm_src_field_count_get(unit, sid,
                                 lta_arg->rfield_list,
                                 num_fields,
                                 num_field_elems);
    }

    return;
}

/*!
 * \brief Get the entry index for given destination field ID.
 *
 * This routine gets the map entry index for the given destination
 * field ID in the specified transform.
 *
 * \param [in] lta_arg Transform handler arguments.
 * \param [in] entry_type Map entry type.
 * \param [in] fid Destination field ID.
 * \param [out] entry_idx Entry index for specified field ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_dst_field_entry_idx_get(const bcmltd_transform_arg_t *lta_arg,
                             bcmlrd_map_entry_type_t entry_type,
                             uint32_t fid,
                             uint16_t *entry_idx)
{
    const bcmltd_field_list_t *field_list;
    uint32_t idx;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Get the right list for destination PT fields */
    if (BCMLTM_DB_MAP_ENTRY_TYPE_IS_FWD_XFRM(entry_type)) {
        field_list = lta_arg->rfield_list;
    } else {
        field_list = lta_arg->field_list;
    }

    /* Search transform field list */
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
 * \brief Get destination field working buffer offset.
 *
 * This function gets the working buffer offset for a
 * transform destination field.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] fid Destination field.
 * \param [in] pt_entry_idx Entry index of destination field.
 * \param [in] me_arg Transform map entry arguments.
 * \param [out] offset Working buffer offset for destination field.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_dst_field_offset_get(int unit,
                          bcmlrd_sid_t sid,
                          uint32_t fid,
                          uint16_t pt_entry_idx,
                          const bcmltm_db_xfrm_me_arg_t *me_arg,
                          uint32_t *offset)
{
    SHR_FUNC_ENTER(unit);

    *offset = BCMLTM_WB_OFFSET_INVALID;

    /*
     * Get offset from corresponding WB block:
     * - Track index field resides in the LT Private block.
     * - PT suppress field resides in the LT Private block.
     * - Other destination fields reside in the PTM block.
     */
    if (fid == BCMLRD_FIELD_TRACK_INDEX) {
        SHR_IF_ERR_EXIT
            (bcmltm_wb_lt_track_index_offset_get_by_sid(unit, sid,
                                                        offset));
    } else if (fid == BCMLRD_FIELD_PT_SUPPRESS) {
        SHR_IF_ERR_EXIT
            (bcmltm_wb_lt_pt_suppress_offset_get_by_sid(unit, sid,
                                                        offset));
    } else {
        const bcmltm_wb_block_t *ptm_wb_block = me_arg->ptm_wb_block;
        bcmlrd_map_entry_type_t entry_type = me_arg->entry->entry_type;
        bcmltm_field_type_t field_type;
        bcmltm_field_dir_t field_dir;

        if (BCMLTM_DB_MAP_ENTRY_TYPE_IS_KEY_XFRM(entry_type)) {
            field_type = BCMLTM_FIELD_TYPE_KEY;
        } else {
            field_type = BCMLTM_FIELD_TYPE_VALUE;
        }

        if (BCMLTM_DB_MAP_ENTRY_TYPE_IS_FWD_XFRM(entry_type)) {
            field_dir = BCMLTM_FIELD_DIR_IN;
        } else {
            field_dir = BCMLTM_FIELD_DIR_OUT;
        }

        SHR_IF_ERR_EXIT
            (bcmltm_wb_ptm_field_lt_offset_get(ptm_wb_block,
                                               fid, field_type, field_dir,
                                               pt_entry_idx,
                                               offset));
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
    const char *table_name = NULL;

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
        (xfrm_is_pt_suppressable(unit, sid, entry_type,
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
 * \param [in] me_arg Transform map entry arguments.
 * \param [out] offset Working buffer offset for transform suppress indicator.
 *                     Invalid offset, if PT suppress is not applicable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_lta_suppress_offset_get(int unit,
                             bcmlrd_sid_t sid,
                             const bcmltm_db_xfrm_me_arg_t *me_arg,
                             uint32_t *offset)
{
    const bcmltm_table_attr_t *attr = me_arg->xfrm_arg->dm_arg->attr;
    bcmlrd_map_entry_type_t entry_type = me_arg->entry->entry_type;
    bcmltm_field_xfrm_type_t xfrm_type = me_arg->xfrm_arg->xfrm_type;
    bool pt_suppress = me_arg->xfrm_arg->dm_arg->pt_suppress;
    uint32_t index_absent_offset = BCMLTM_WB_OFFSET_INVALID;
    uint32_t pt_suppress_offset = BCMLTM_WB_OFFSET_INVALID;

    SHR_FUNC_ENTER(unit);

    *offset = BCMLTM_WB_OFFSET_INVALID;

    /*
     * A transform can be skipped under only one of the following cases:
     * - Index Absent
     * - PT suppress
     */

    
    if (xfrm_type == BCMLTM_DB_XFRM_TYPE_FWD_KEY_TRANSFORM) {
        bool alloc = bcmltm_db_xfrm_key_is_alloc(attr, xfrm_type,
                                                 me_arg->lta_handler->arg);
        if (alloc) {
            SHR_IF_ERR_EXIT
                (bcmltm_wb_lt_index_absent_offset_get_by_sid(unit, sid,
                                                      &index_absent_offset));
        }
    }

    /* PT Suppress */
    SHR_IF_ERR_EXIT
        (xfrm_pt_suppress_offset_get(unit, sid,
                                     entry_type, pt_suppress,
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
 * \brief Destroy given LTA transform params metadata.
 *
 * This routine destroys the given LTA transform params metadata.
 *
 * \param [in] params LTA transform params metadata to destroy.
 */
static void
xfrm_params_destroy(bcmltm_lta_transform_params_t *params)
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
 * \brief Create LTA transform params metadata.
 *
 * This routine creates the LTA transform params metadata
 * for the given field transform map entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] entry LRD transform map entry.
 * \param [in] me_arg Transform map entry arguments.
 * \param [out] params_ptr Returning pointer to transform params metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_params_create(int unit, bcmlrd_sid_t sid,
                   const bcmlrd_map_entry_t *entry,
                   const bcmltm_db_xfrm_me_arg_t *me_arg,
                   bcmltm_lta_transform_params_t **params_ptr)
{
    bcmltm_lta_transform_params_t *params = NULL;
    bcmlrd_map_entry_type_t entry_type = entry->entry_type;
    const bcmltd_xfrm_handler_t *lta_handler = me_arg->lta_handler;
    const bcmltm_wb_block_t *lta_wb_block = me_arg->lta_wb_block;
    const bcmltm_wb_block_t *ptm_wb_block = me_arg->ptm_wb_block;
    bcmltm_wb_lta_fields_offsets_t in_offsets;
    bcmltm_wb_lta_fields_offsets_t in_key_offsets;
    bcmltm_wb_lta_fields_offsets_t out_offsets;
    uint32_t pt_sid_override_offset = BCMLTM_WB_OFFSET_INVALID;
    uint32_t pt_sid_selector_offset = BCMLTM_WB_OFFSET_INVALID;
    uint32_t lta_suppress_offset = BCMLTM_WB_OFFSET_INVALID;
    uint32_t smi_offset = me_arg->xfrm_arg->select_map_index_offset;
    uint32_t smi = me_arg->xfrm_arg->select_map_index;
    uint32_t alloc_size;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(params, sizeof(*params), "bcmltmLtaTransformParams");
    SHR_NULL_CHECK(params, SHR_E_MEMORY);
    sal_memset(params, 0, sizeof(*params));

    
    alloc_size = sizeof(bcmltd_transform_arg_t);
    SHR_ALLOC(params->lta_args, alloc_size, "bcmltmLtaTransformArg");
    SHR_NULL_CHECK(params->lta_args, SHR_E_MEMORY);
    sal_memset(params->lta_args, 0, alloc_size);

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

    /* Get PT table select offset */
    if (lta_handler->arg->tables != 0) {
        /* TABLE_SEL are expected only on key transforms */
        if (BCMLTM_DB_MAP_ENTRY_TYPE_IS_KEY_XFRM(entry_type)) {
            uint16_t pt_entry_idx;

            SHR_IF_ERR_EXIT
                (xfrm_dst_field_entry_idx_get(lta_handler->arg,
                                              entry_type,
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
        (xfrm_lta_suppress_offset_get(unit, sid, me_arg,
                                      &lta_suppress_offset));

    /* Fill data */
    params->lta_transform = lta_handler->transform;
    params->lta_ext_transform = lta_handler->ext_transform;
    params->fields_input_offset = in_offsets.param;
    params->fields_input_key_offset = in_key_offsets.param;
    params->fields_output_offset = out_offsets.param;
    params->pt_sid_selector_offset = pt_sid_selector_offset;
    params->pt_sid_override_offset = pt_sid_override_offset;
    params->transform_suppress_offset = lta_suppress_offset;
    params->select_map_index_offset = smi_offset;
    params->select_map_index = smi;
    sal_memcpy(params->lta_args, lta_handler->arg, sizeof(*params->lta_args));

    *params_ptr = params;

 exit:
    if (SHR_FUNC_ERR()) {
        xfrm_params_destroy(params);
        *params_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy field maps metadata.
 *
 * This routine destroys the given field maps metadata array.
 *
 * \param [in] field_maps Field maps metadata to destroy.
 */
static inline void
field_maps_destroy(bcmltm_lta_select_field_map_t *field_maps)
{
    SHR_FREE(field_maps);

    return;
}

/*!
 * \brief Create transform field maps metadata for source fields.
 *
 * This routine creates the array of field maps metadata for
 * for the given source field list.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in,out] list LTA field list to add created field maps to.
 * \param [in] lrd_field_list LRD field list to process.
 * \param [in] list_type Transform field list type (in, in-key, out).
 * \param [in] me_arg Transform map entry arguments.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_src_field_maps_create(int unit,
                           bcmlrd_sid_t sid,
                           bcmltm_lta_select_field_list_t *list,
                           const bcmltd_field_list_t *lrd_field_list,
                           bcmltm_xfrm_field_list_type_t list_type,
                           const bcmltm_db_xfrm_me_arg_t *me_arg)
{
    bcmltm_lta_select_field_map_t *field_maps = NULL;
    bcmltm_lta_select_field_map_t *field_map;
    const bcmltd_field_desc_t *field_desc;
    int wb_lta_field_type;
    uint32_t num_fields;
    uint32_t max_field_elems;
    unsigned int alloc_size;
    uint32_t fid;
    uint32_t idx;
    uint32_t idx_count;
    uint32_t fm_idx;
    uint32_t lta_fidx;
    uint32_t apic_offset = BCMLTM_WB_OFFSET_INVALID;
    uint32_t lta_offset = BCMLTM_WB_OFFSET_INVALID;
    uint32_t field_flags;

    SHR_FUNC_ENTER(unit);

    list->num_maps = 0;
    list->max_fields = 0;
    list->lta_select_field_maps = NULL;

    xfrm_src_field_count_get(unit, sid, lrd_field_list,
                             &num_fields, &max_field_elems);
    if (max_field_elems == 0) {
        SHR_EXIT();
    }

    /* Allocate field map array */
    alloc_size = sizeof(*field_maps) * max_field_elems;
    SHR_ALLOC(field_maps, alloc_size, "bcmltmXfrmLtaSelectFieldMaps");
    SHR_NULL_CHECK(field_maps, SHR_E_MEMORY);
    sal_memset(field_maps, 0, alloc_size);

    wb_lta_field_type = xfrm_wb_lta_field_type_get(list_type);

    /*
     * Process each field in LRD field list.
     *
     * Rule:
     * - The order of the input fields for a transform must
     *   correspond to the order in which the fields are specified
     *   in the LRD field list.
     * - Indexes for a given field must be in order.
     *
     * The following list creation follows this rule.
     */
    for (fm_idx = 0, lta_fidx = 0; lta_fidx < num_fields; lta_fidx++) {
        field_desc = &lrd_field_list->field_array[lta_fidx];

        fid = field_desc->field_id;

        idx_count = bcmlrd_field_idx_count_get(unit, sid, fid);
        if (idx_count == 0) {
            continue;    /* Skip unmapped fields */
        }

        /* Get field properties */
        SHR_IF_ERR_EXIT
            (bcmltm_db_field_properties_get(unit, sid, fid, &field_flags));

        for (idx = 0; idx < idx_count; idx++) {
            /* Sanity check */
            if (fm_idx >= max_field_elems) {
                const char *table_name;

                table_name = bcmltm_lt_table_sid_to_name(unit, sid);
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "LTM Transform Field Map: current "
                                      "field count exceeds max count "
                                      "%s(ltid=%d) "
                                      "count=%d max_count=%d\n"),
                           table_name, sid, (fm_idx+1), max_field_elems));
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }

            /* Get field offsets */
            SHR_IF_ERR_EXIT
                (bcmltm_wb_lt_apic_field_offset_get_by_sid(unit, sid,
                                                           fid, idx,
                                                           &apic_offset));


            SHR_IF_ERR_EXIT
                (bcmltm_wb_lta_field_map_offset_get(me_arg->lta_wb_block,
                                                    wb_lta_field_type,
                                                    fm_idx,
                                                    &lta_offset));

            /* Get next field map to populate */
            field_map = &field_maps[fm_idx];
            fm_idx++;

            /* Fill data */
            field_map->field_id = fid;
            field_map->field_idx = idx;
            field_map->apic_offset = apic_offset;
            field_map->field_offset = lta_offset;
            field_map->flags = field_flags;

            /* Not applicable */
            field_map->wbc.buffer_offset = BCMLTM_WB_OFFSET_INVALID;
            field_map->wbc.maxbit = 0;
            field_map->wbc.minbit = 0;
            field_map->pt_changed_offset = BCMLTM_WB_OFFSET_INVALID;
        }
    }

    list->num_maps = fm_idx;
    list->max_fields = max_field_elems;
    list->lta_select_field_maps = field_maps;

 exit:
    if (SHR_FUNC_ERR()) {
        field_maps_destroy(field_maps);
        list->num_maps = 0;
        list->max_fields = 0;
        list->lta_select_field_maps = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create transform field maps metadata for destination fields.
 *
 * This routine creates the array of field maps metadata for
 * for the given destination field list.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in,out] list LTA field list to add created field maps to.
 * \param [in] lrd_field_list LRD field list to process.
 * \param [in] list_type Transform field list type (in, in-key, out).
 * \param [in] me_arg Transform map entry arguments.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_dst_field_maps_create(int unit,
                           bcmlrd_sid_t sid,
                           bcmltm_lta_select_field_list_t *list,
                           const bcmltd_field_list_t *lrd_field_list,
                           bcmltm_xfrm_field_list_type_t list_type,
                           const bcmltm_db_xfrm_me_arg_t *me_arg)
{
    bcmltm_lta_select_field_map_t *field_maps = NULL;
    bcmltm_lta_select_field_map_t *field_map;
    const bcmltm_table_attr_t *attr = me_arg->xfrm_arg->dm_arg->attr;
    const bcmltd_field_desc_t *field_desc;
    bcmltm_field_type_t field_type;
    int wb_lta_field_type;
    uint32_t num_fields;
    uint32_t max_field_elems;
    unsigned int alloc_size;
    uint32_t fid;
    uint32_t idx;
    uint32_t idx_count;
    uint16_t pt_entry_idx;
    uint32_t fm_idx;
    uint32_t lta_fidx;
    uint32_t lta_offset = BCMLTM_WB_OFFSET_INVALID;
    uint32_t pt_changed_offset = BCMLTM_WB_OFFSET_INVALID;
    uint32_t dst_field_offset = BCMLTM_WB_OFFSET_INVALID;
    uint16_t dst_minbit[MAX_FIELD_IDXS];
    uint16_t dst_maxbit[MAX_FIELD_IDXS];
    uint32_t field_flags = 0x0;

    SHR_FUNC_ENTER(unit);

    list->num_maps = 0;
    list->max_fields = 0;
    list->lta_select_field_maps = NULL;

    xfrm_dst_field_count_get(unit, sid, lrd_field_list,
                             &num_fields, &max_field_elems);
    if (max_field_elems == 0) {
        SHR_EXIT();
    }

    /* Allocate field map array */
    alloc_size = sizeof(*field_maps) * max_field_elems;
    SHR_ALLOC(field_maps, alloc_size, "bcmltmXfrmLtaSelectFieldMaps");
    SHR_NULL_CHECK(field_maps, SHR_E_MEMORY);
    sal_memset(field_maps, 0, alloc_size);

    /*
     * Get field properties.
     *
     * Destination field is PT field.  The only
     * indicator for the PT field properties comes from the transform type.
     */
    if (BCMLTM_DB_MAP_ENTRY_TYPE_IS_KEY_XFRM(me_arg->entry->entry_type)) {
        field_flags |= BCMLTM_FIELD_KEY;
        field_type = BCMLTM_FIELD_TYPE_KEY;
    } else {
        field_type = BCMLTM_FIELD_TYPE_VALUE;
    }

    wb_lta_field_type = xfrm_wb_lta_field_type_get(list_type);

    /*
     * Process each field in LRD field list.
     *
     * Rule: The order of the input fields for a transform must
     * correspond to the order in which the fields are specified
     * in the LRD field list.
     *
     * The following list creation follows this rule.
     */
    for (fm_idx = 0, lta_fidx = 0; lta_fidx < num_fields; lta_fidx++) {
        field_desc = &lrd_field_list->field_array[lta_fidx];

        fid = field_desc->field_id;
        pt_entry_idx = field_desc->entry_idx;

        /* Get PT changed offset */
        SHR_IF_ERR_EXIT
            (bcmltm_wb_ptm_pt_changed_offset_get(attr->type,
                                                 field_type,
                                                 me_arg->ptm_wb_block,
                                                 pt_entry_idx,
                                                 &pt_changed_offset));

        /* Get destination field offset */
        SHR_IF_ERR_EXIT
            (xfrm_dst_field_offset_get(unit, sid,
                                       fid, pt_entry_idx, me_arg,
                                       &dst_field_offset));

        /* Get destination field min and max bits per index */
        SHR_IF_ERR_EXIT
            (bcmltm_db_field_to_fidx_min_max(unit, fid,
                                             field_desc->minbit,
                                             field_desc->maxbit,
                                             MAX_FIELD_IDXS,
                                             dst_minbit, dst_maxbit,
                                             &idx_count));

        /* All fields has at least 1 index count */
        if (idx_count < 1) {
            const char *table_name;
            const char *field_name;

            table_name = bcmltm_lt_table_sid_to_name(unit, sid);
            field_name = bcmltm_lt_field_fid_to_name(unit, sid, fid);
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "LTM Transform Destination Field Map: "
                                  "invalid field index count (should be > 0) "
                                  "%s(ltid=%d) %s(fid=%d) count=%d\n"),
                       table_name, sid, field_name, fid, idx_count));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        /* Fill data for each field index element */
        for (idx = 0; idx < idx_count; idx++) {
            /* Sanity check */
            if (fm_idx >= max_field_elems) {
                const char *table_name;

                table_name = bcmltm_lt_table_sid_to_name(unit, sid);
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "LTM Transform Field Map: current "
                                      "field count exceeds max count "
                                      "%s(ltid=%d) "
                                      "count=%d max_count=%d\n"),
                           table_name, sid, (fm_idx+1), max_field_elems));
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }

            /* Get field offset */
            SHR_IF_ERR_EXIT
                (bcmltm_wb_lta_field_map_offset_get(me_arg->lta_wb_block,
                                                    wb_lta_field_type,
                                                    fm_idx,
                                                    &lta_offset));

            /* Get next field map to populate */
            field_map = &field_maps[fm_idx];
            fm_idx++;


            /* Fill data */
            field_map->field_id = fid;
            field_map->field_idx = idx;
            field_map->wbc.buffer_offset = dst_field_offset;
            field_map->wbc.maxbit = dst_maxbit[idx];
            field_map->wbc.minbit = dst_minbit[idx];
            field_map->field_offset = lta_offset;
            field_map->pt_changed_offset = pt_changed_offset;
            field_map->flags = field_flags;

            /* Not applicable */
            field_map->apic_offset = BCMLTM_WB_OFFSET_INVALID;
        }
    }

    list->num_maps = fm_idx;
    list->max_fields = max_field_elems;
    list->lta_select_field_maps = field_maps;

 exit:
    if (SHR_FUNC_ERR()) {
        field_maps_destroy(field_maps);
        list->num_maps = 0;
        list->max_fields = 0;
        list->lta_select_field_maps = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create transform field maps metadata.
 *
 * This routine creates the array of field maps metadata
 * for the specified transform field list type on given transform.
 *
 * The field maps metadata is populated differently based
 * on the target field list type and transform type.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in,out] list LTA field list to add created field maps to.
 * \param [in] list_type Transform field list type (in, in-key, out).
 * \param [in] me_arg Transform map entry arguments.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_field_maps_create(int unit,
                       bcmlrd_sid_t sid,
                       bcmltm_lta_select_field_list_t *list,
                       bcmltm_xfrm_field_list_type_t list_type,
                       const bcmltm_db_xfrm_me_arg_t *me_arg)
{
    const bcmltd_transform_arg_t *lta_arg = me_arg->lta_handler->arg;
    bcmlrd_map_entry_type_t entry_type = me_arg->entry->entry_type;
    const bcmltd_field_list_t *field_list;
    bool is_src = FALSE;

    SHR_FUNC_ENTER(unit);

    /*
     * Select corresponding transform field list to process
     * and function to use for creating the metadata.
     */
    if (list_type == BCMLTM_XFRM_FIELD_LIST_TYPE_IN) {
        /*
         * Input field list
         *
         * Forward transform: fields are source logical fields.
         * Reverse transform: fields are destination physical fields.
         */
        field_list = lta_arg->field_list;
        if (BCMLTM_DB_MAP_ENTRY_TYPE_IS_FWD_XFRM(entry_type)) {
            is_src = TRUE;
        } else {
            is_src = FALSE;
        }

    } else if (list_type == BCMLTM_XFRM_FIELD_LIST_TYPE_IN_KEY) {
        /*
         * Input key field list
         *
         * Fields are always source logical fields.
         */
        field_list = lta_arg->kfield_list;
        is_src = TRUE;

    } else {
        /*
         * Output fields
         *
         * Forward transform: fields are destination physical fields.
         * Reverse transform: fields are source logical fields.
         */
        field_list = lta_arg->rfield_list;
        if (BCMLTM_DB_MAP_ENTRY_TYPE_IS_FWD_XFRM(entry_type)) {
            is_src = FALSE;
        } else {
            is_src = TRUE;
        }
    }

    if (is_src) {
        SHR_IF_ERR_EXIT
            (xfrm_src_field_maps_create(unit, sid,
                                        list, field_list,
                                        list_type, me_arg));
    } else {
        SHR_IF_ERR_EXIT
            (xfrm_dst_field_maps_create(unit, sid,
                                        list, field_list,
                                        list_type, me_arg));
    }

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create LTA transform field list metadata.
 *
 * This routine creates the LTA field list metadata
 * for the given transform field list type (input, input-key, or
 * output) in transform map entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] list_type Transform field list type (in, in-key, out).
 * \param [in] entry Map entry.
 * \param [in] me_arg Transform map entry arguments.
 * \param [out] list_ptr Returning pointer to LTA field list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_field_list_create(int unit,
                       bcmlrd_sid_t sid,
                       bcmltm_xfrm_field_list_type_t list_type,
                       const bcmlrd_map_entry_t *entry,
                       const bcmltm_db_xfrm_me_arg_t *me_arg,
                       bcmltm_lta_select_field_list_t **list_ptr)
{
    bcmltm_lta_select_field_list_t *list = NULL;
    bcmlrd_map_entry_type_t entry_type = me_arg->entry->entry_type;
    const bcmltm_wb_block_t *lta_wb_block = me_arg->lta_wb_block;
    int wb_lta_field_type;
    bcmltm_wb_lta_fields_offsets_t offsets;
    uint32_t api_fields_offset = BCMLTM_WB_OFFSET_INVALID;
    uint32_t lta_suppress_offset = BCMLTM_WB_OFFSET_INVALID;
    uint32_t smi_offset = me_arg->xfrm_arg->select_map_index_offset;
    uint32_t smi = me_arg->xfrm_arg->select_map_index;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(list, sizeof(*list), "bcmltmXfrmLtaSelectFieldList");
    SHR_NULL_CHECK(list, SHR_E_MEMORY);
    sal_memset(list, 0, sizeof(*list));

    /* Get fields offset */
    wb_lta_field_type = xfrm_wb_lta_field_type_get(list_type);
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lta_fields_offsets_get(lta_wb_block, wb_lta_field_type,
                                          &offsets));

    /*
     * For forward transform, the API input list is always the LTA input.
     * For reverse transform, there is no API input list.
     */
    if (BCMLTM_DB_MAP_ENTRY_TYPE_IS_FWD_XFRM(entry_type)) {
        if (list_type == BCMLTM_XFRM_FIELD_LIST_TYPE_OUT) {
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

    /* Get transform suppress offset */
    SHR_IF_ERR_EXIT
        (xfrm_lta_suppress_offset_get(unit, sid, me_arg,
                                      &lta_suppress_offset));

    /* Fill data */
    list->fields_parameter_offset = offsets.param;
    list->api_fields_parameter_offset = api_fields_offset;
    list->fields_ptrs_offset = offsets.ptrs;
    list->lta_suppress_offset = lta_suppress_offset;
    list->select_map_index_offset = smi_offset;
    list->select_map_index = smi;

    /* Create field maps */
    SHR_IF_ERR_EXIT
        (xfrm_field_maps_create(unit, sid,
                                list, list_type, me_arg));

    *list_ptr = list;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_lta_select_field_list_destroy(list);
        *list_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Add field transform metadata for given map entry.
 *
 * This routine adds the metadata for the given field transform
 * map entry into the transform list.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in,out] list List to add new transform to.
 * \param [in] map_entry Map entry identifier of map to add metadata for.
 * \param [in] xfrm_arg Transform arguments.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_map_entry_add(int unit,
                   bcmlrd_sid_t sid,
                   bcmltm_db_dm_xfrm_list_t *list,
                   const bcmltm_db_map_entry_t *map_entry,
                   const bcmltm_db_xfrm_arg_t *xfrm_arg)
{
    const bcmlrd_map_t *lrd_map = xfrm_arg->lrd_map;
    const bcmltm_db_dm_pt_info_t *pt_info = xfrm_arg->dm_arg->pt_info;
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    bcmlrd_map_entry_type_t entry_type;
    bcmdrd_sid_t ptid;
    const bcmltd_xfrm_handler_t *lta_handler = NULL;
    bcmltm_db_dm_xfrm_t *xfrm = NULL;
    bcmltm_db_xfrm_me_arg_t me_arg;
    uint32_t num_in_fields = 0;
    uint32_t num_in_field_elems = 0;
    uint32_t num_in_key_fields = 0;
    uint32_t num_in_key_field_elems = 0;
    uint32_t num_out_fields = 0;
    uint32_t num_out_field_elems = 0;
    bcmltm_wb_handle_t *wb_handle = NULL;
    const bcmltm_wb_block_t *ptm_wb_block = NULL;
    bcmltm_wb_block_id_t lta_wb_block_id;
    bcmltm_wb_block_t *lta_wb_block = NULL;
    unsigned int alloc_size;
    bool is_alloc_keys;

    SHR_FUNC_ENTER(unit);

    group = bcmltm_db_map_lrd_group_get(lrd_map, map_entry);
    entry = bcmltm_db_map_lrd_entry_get(lrd_map, map_entry);
    entry_type = entry->entry_type;
    ptid = group->dest.id;

    /* Sanity check and get handler */
    SHR_IF_ERR_EXIT
        (bcmltm_db_xfrm_check(unit, sid, entry, &lta_handler));

    /* Get count of fields(fid) and field elements(fid,idx)  */
    xfrm_in_field_count_get(unit, sid, lta_handler->arg, entry_type,
                            &num_in_fields, &num_in_field_elems);
    xfrm_in_key_field_count_get(unit, sid, lta_handler->arg,
                                &num_in_key_fields, &num_in_key_field_elems);
    xfrm_out_field_count_get(unit, sid, lta_handler->arg, entry_type,
                             &num_out_fields, &num_out_field_elems);

    /* Find PTM working buffer block */
    ptm_wb_block = bcmltm_db_dm_ptm_wb_block_find(unit, sid,
                                                  pt_info, ptid);
    if (ptm_wb_block == NULL) {
        const char *table_name;
        const char *pt_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        pt_name = bcmltm_pt_table_sid_to_name(unit, ptid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM PTM working buffer block "
                              "not found: %s(ltid=%d) %s(ptid=%d)\n"),
                   table_name, sid, pt_name, ptid));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Add LTA Working Buffer block */
    wb_handle = bcmltm_wb_lt_handle_get(unit, sid);
    if (wb_handle == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_IF_ERR_EXIT
        (bcmltm_wb_block_lta_xfrm_add(unit,
                                      num_in_field_elems,
                                      num_in_key_field_elems,
                                      num_out_field_elems,
                                      wb_handle,
                                      &lta_wb_block_id, &lta_wb_block));

    /* Check if transform contains allocatable keys (IwA) */
    is_alloc_keys = bcmltm_db_xfrm_key_is_alloc(xfrm_arg->dm_arg->attr,
                                                xfrm_arg->xfrm_type,
                                                lta_handler->arg);

    /* Populate transform arguments */
    sal_memset(&me_arg, 0, sizeof(me_arg));
    me_arg.group = group;
    me_arg.entry = entry;
    me_arg.lta_handler = lta_handler;
    me_arg.lta_wb_block = lta_wb_block;
    me_arg.ptm_wb_block = ptm_wb_block;
    me_arg.xfrm_arg = xfrm_arg;

    /* Allocate transform struct */
    alloc_size = sizeof(*xfrm);
    SHR_ALLOC(xfrm, alloc_size, "bcmltmDbXfrm");
    SHR_NULL_CHECK(xfrm, SHR_E_MEMORY);
    sal_memset(xfrm, 0, alloc_size);

    /* Add new field transform to list */
    list->xfrms[list->num_xfrms] = xfrm;
    list->num_xfrms++;

    /* Set field transform information and metadata */
    sal_memcpy(&xfrm->map_entry, map_entry, sizeof(*map_entry));
    xfrm->lta_wb_block_id = lta_wb_block_id;
    xfrm->is_alloc_keys = is_alloc_keys;

    /* Create field transform params */
    SHR_IF_ERR_EXIT
        (xfrm_params_create(unit, sid,
                            entry, &me_arg,
                            &xfrm->params));

    /* Create field transform field lists */
    SHR_IF_ERR_EXIT
        (xfrm_field_list_create(unit, sid,
                                BCMLTM_XFRM_FIELD_LIST_TYPE_IN,
                                entry, &me_arg,
                                &xfrm->in_fields));
    SHR_IF_ERR_EXIT
        (xfrm_field_list_create(unit, sid,
                                BCMLTM_XFRM_FIELD_LIST_TYPE_IN_KEY,
                                entry, &me_arg,
                                &xfrm->in_key_fields));
    SHR_IF_ERR_EXIT
        (xfrm_field_list_create(unit, sid,
                                BCMLTM_XFRM_FIELD_LIST_TYPE_OUT,
                                entry, &me_arg,
                                &xfrm->out_fields));

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add field transform metadata for a selection map.
 *
 * This routine creates and adds the field transform metadata
 * for the map entries specified in the field selection map entry list.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] dm_arg DM arguments.
 * \param [in,out] list List to add transforms to.
 * \param [in] sm Field selection map.
 * \param [in] select_map_index_offset Selector map index offset.
 * \param [in] xfrm_type Specifies transform type to create metadata for.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_selection_map_add(int unit,
                       bcmlrd_sid_t sid,
                       const bcmltm_db_dm_arg_t *dm_arg,
                       bcmltm_db_dm_xfrm_list_t *list,
                       const bcmltm_db_field_selection_map_t *sm,
                       uint32_t select_map_index_offset,
                       bcmltm_field_xfrm_type_t xfrm_type)
{
    const bcmlrd_map_t *lrd_map = sm->map_entry_list->map;
    const bcmltm_db_map_entry_list_t *map_entry_list = sm->map_entry_list;
    const bcmltm_db_map_entry_t *map_entry;
    bcmltm_db_xfrm_arg_t xfrm_arg;
    uint32_t me_idx;

    SHR_FUNC_ENTER(unit);

    /* Populate transform arguments */
    sal_memset(&xfrm_arg, 0, sizeof(xfrm_arg));
    xfrm_arg.xfrm_type = xfrm_type;
    xfrm_arg.lrd_map = lrd_map;
    xfrm_arg.select_map_index_offset = select_map_index_offset;
    xfrm_arg.select_map_index = sm->map_index;
    xfrm_arg.dm_arg = dm_arg;

    /* Process each entry in map entry list */
    for (me_idx = 0; me_idx < map_entry_list->num_entries; me_idx++) {
        map_entry = &map_entry_list->entries[me_idx];

        /* Check if map entry qualifies, if not skip */
        if (!xfrm_map_entry_process(unit, sid,
                                    lrd_map, map_entry, xfrm_type)) {
            continue;
        }

        /* Add map entry to list */
        SHR_IF_ERR_EXIT
            (xfrm_map_entry_add(unit, sid,
                                list, map_entry, &xfrm_arg));
    }

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Resize transforms array.
 *
 * This utility routine resizes the field transforms array in the
 * given list to a smaller size, if necessary,
 * in order to free unused memory.
 *
 * If a resize takes place, the previous array is freed.
 *
 * \param [in] unit Unit number.
 * \param [in] max_array Max transforms allocated in array.
 * \param [in,out] list List of transforms.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_list_resize(int unit,
                 uint32_t max_array,
                 bcmltm_db_dm_xfrm_list_t *list)
{
    bcmltm_db_dm_xfrm_t **xfrms = NULL;
    bcmltm_db_dm_xfrm_t **new_xfrms = NULL;
    unsigned int alloc_size;

    SHR_FUNC_ENTER(unit);

    xfrms = list->xfrms;

    /* Sanity check */
    if (list->num_xfrms > max_array) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Nothing to do if number of transforms matches allocated max */
    if (list->num_xfrms == max_array) {
        SHR_EXIT();
    }

    /* Free array if there are no field transforms */
    if (list->num_xfrms == 0) {
        SHR_FREE(xfrms);
        list->xfrms = NULL;
        SHR_EXIT();
    }

    /* Reallocate smaller array and copy array elements */
    alloc_size = sizeof(bcmltm_db_dm_xfrm_t *) * list->num_xfrms;
    SHR_ALLOC(new_xfrms, alloc_size, "bcmltmDbXfrms");
    SHR_NULL_CHECK(new_xfrms, SHR_E_MEMORY);
    sal_memcpy(new_xfrms, xfrms, alloc_size);

    /* Free old array and return new array */
    SHR_FREE(xfrms);
    list->xfrms = new_xfrms;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy field transform list.
 *
 * This routine destroys the given field transform list.
 *
 * \param [in] list Field transform list to destroy.
 */
static void
xfrm_list_destroy(bcmltm_db_dm_xfrm_list_t *list)
{
    bcmltm_db_dm_xfrm_t *xfrm = NULL;
    uint32_t idx;

    if (list == NULL) {
        return;
    }

    if (list->xfrms != NULL) {
        for (idx = 0; idx < list->num_xfrms; idx++) {
            xfrm = list->xfrms[idx];

            /* Destroy field lists */
            bcmltm_db_lta_select_field_list_destroy(xfrm->out_fields);
            bcmltm_db_lta_select_field_list_destroy(xfrm->in_key_fields);
            bcmltm_db_lta_select_field_list_destroy(xfrm->in_fields);

            /* Destroy transform params */
            xfrm_params_destroy(xfrm->params);

            SHR_FREE(xfrm);
        }

        SHR_FREE(list->xfrms);
    }

    SHR_FREE(list);

    return;
}

/*!
 * \brief Create field transform list for a selection group.
 *
 * This routine creates the list of field transforms
 * for the given field selection group.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] dm_arg DM arguments.
 * \param [in] selection Field selection group.
 * \param [in] xfrm_type Specifies transform type to create metadata for.
 * \param [out] list_ptr Returning pointer to field transform info list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_list_create(int unit,
                 bcmlrd_sid_t sid,
                 const bcmltm_db_dm_arg_t *dm_arg,
                 const bcmltm_db_field_selection_t *selection,
                 bcmltm_field_xfrm_type_t xfrm_type,
                 bcmltm_db_dm_xfrm_list_t **list_ptr)
{
    bcmltm_db_dm_xfrm_list_t *list = NULL;
    const bcmltm_db_field_selection_map_t *selection_map = NULL;
    uint32_t select_map_index_offset = BCMLTM_WB_OFFSET_INVALID;
    uint32_t max_entries;
    unsigned int alloc_size;
    uint32_t sm_idx;

    SHR_FUNC_ENTER(unit);

    *list_ptr = NULL;

    /* Use max map entries count for max transforms in table */
    SHR_IF_ERR_EXIT
        (bcmltm_db_map_entries_count_get(unit, sid,
                                         &max_entries));
    if (max_entries == 0) {
        SHR_EXIT();
    }

    /* Allocate list */
    SHR_ALLOC(list , sizeof(*list), "bcmltmDbXfrmList");
    SHR_NULL_CHECK(list, SHR_E_MEMORY);
    sal_memset(list, 0, sizeof(*list));

    /* Allocate field transform array */
    alloc_size = sizeof(bcmltm_db_dm_xfrm_t *) * max_entries;
    SHR_ALLOC(list->xfrms , alloc_size, "bcmltmDbXfrms");
    SHR_NULL_CHECK(list->xfrms, SHR_E_MEMORY);
    sal_memset(list->xfrms, 0, alloc_size);

    list->num_xfrms = 0;

    /* Get field selection map index offset */
    if (selection->selection_id != BCMLTM_FIELD_SELECTION_ID_UNCOND) {
        SHR_IF_ERR_EXIT
            (bcmltm_wb_lt_fs_map_index_offset_get_by_sid(unit, sid,
                                              selection->selection_id,
                                              &select_map_index_offset));
    }

    /* Process each selection map */
    for (sm_idx = 0; sm_idx < selection->num_maps; sm_idx++) {
        selection_map = selection->maps[sm_idx];
        SHR_IF_ERR_EXIT
            (xfrm_selection_map_add(unit, sid, dm_arg,
                                    list,
                                    selection_map,
                                    select_map_index_offset,
                                    xfrm_type));
    }

    /* Resize array */
    SHR_IF_ERR_EXIT
        (xfrm_list_resize(unit, max_entries, list));

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
bcmltm_db_dm_xfrm_info_create(int unit,
                              bcmlrd_sid_t sid,
                              const bcmltm_db_dm_arg_t *dm_arg,
                              const bcmltm_db_field_selection_t *selection,
                              bcmltm_db_dm_xfrm_info_t **info_ptr)
{
    bcmltm_db_dm_xfrm_info_t *info = NULL;

    SHR_FUNC_ENTER(unit);

    *info_ptr = NULL;

    /* Allocate */
    SHR_ALLOC(info, sizeof(*info), "bcmltmDbDmXfrmInfo");
    SHR_NULL_CHECK(info, SHR_E_MEMORY);
    sal_memset(info, 0, sizeof(*info));

    /* Create Forward Key Field Transform Information */
    SHR_IF_ERR_EXIT
        (xfrm_list_create(unit, sid, dm_arg,
                          selection,
                          BCMLTM_FIELD_XFRM_TYPE_FWD_KEY,
                          &info->fwd_keys));

    /* Create Forward Value Field Transform Information */
    SHR_IF_ERR_EXIT
        (xfrm_list_create(unit, sid, dm_arg,
                          selection,
                          BCMLTM_FIELD_XFRM_TYPE_FWD_VALUE,
                          &info->fwd_values));

    /* Create Reverse Key Field Transform Information */
    SHR_IF_ERR_EXIT
        (xfrm_list_create(unit, sid, dm_arg,
                          selection,
                          BCMLTM_FIELD_XFRM_TYPE_REV_KEY,
                          &info->rev_keys));

    /* Create Reverse Value Field Transform Information */
    SHR_IF_ERR_EXIT
        (xfrm_list_create(unit, sid, dm_arg,
                          selection,
                          BCMLTM_FIELD_XFRM_TYPE_REV_VALUE,
                          &info->rev_values));

    *info_ptr = info;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_dm_xfrm_info_destroy(info);
        *info_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

void
bcmltm_db_dm_xfrm_info_destroy(bcmltm_db_dm_xfrm_info_t *info)
{
    if (info == NULL) {
        return;
    }

    xfrm_list_destroy(info->rev_values);
    xfrm_list_destroy(info->rev_keys);
    xfrm_list_destroy(info->fwd_values);
    xfrm_list_destroy(info->fwd_keys);

    SHR_FREE(info);

    return;
}

int
bcmltm_db_dm_xfrm_list_ref_create(int unit,
                                  bcmlrd_sid_t sid,
                                  const bcmltm_db_map_entry_list_t *map_list,
                                  const bcmltm_db_dm_xfrm_list_t *in_list,
                                  bcmltm_field_xfrm_type_t xfrm_type,
                                  bcmltm_db_dm_xfrm_list_t **list_ptr)
{
    bcmltm_db_dm_xfrm_list_t *list = NULL;
    bcmltm_db_dm_xfrm_t *xfrm = NULL;
    const bcmltm_db_map_entry_t *map_entry = NULL;
    const bcmlrd_map_t *lrd_map;
    uint32_t max_entries;
    unsigned int alloc_size;
    uint32_t me_idx;

    SHR_FUNC_ENTER(unit);

    *list_ptr = NULL;

    if (map_list == 0) {
        /* Nothing to do */
        SHR_EXIT();
    }

    /* Use map entries count for max transforms in table */
    max_entries = map_list->num_entries;
    if (max_entries == 0) {
        SHR_EXIT();
    }

    /* Allocate list */
    SHR_ALLOC(list , sizeof(*list), "bcmltmDbXfrmList");
    SHR_NULL_CHECK(list, SHR_E_MEMORY);
    sal_memset(list, 0, sizeof(*list));

    /* Allocate field transform array */
    alloc_size = sizeof(bcmltm_db_dm_xfrm_t *) * max_entries;
    SHR_ALLOC(list->xfrms , alloc_size, "bcmltmDbXfrms");
    SHR_NULL_CHECK(list->xfrms, SHR_E_MEMORY);
    sal_memset(list->xfrms, 0, alloc_size);

    list->num_xfrms = 0;

    /* Process each entry in map entry list */
    lrd_map = map_list->map;
    for (me_idx = 0; me_idx < map_list->num_entries; me_idx++) {
        map_entry = &map_list->entries[me_idx];

        /* Check if map entry qualifies, if not skip */
        if (!xfrm_map_entry_process(unit, sid,
                                    lrd_map, map_entry, xfrm_type)) {
            continue;
        }

        /* Find transform */
        xfrm = bcmltm_db_dm_xfrm_find(in_list, map_entry);
        if (xfrm == NULL) {
            const char *table_name;

            /* Map entry expected to be found in list */
            table_name = bcmltm_lt_table_sid_to_name(unit, sid);
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Transform map entry not found: "
                                  "%s(ltid=%d) group_idx=%d entry_idx=%d\n"),
                       table_name, sid,
                       map_entry->group_idx, map_entry->entry_idx));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        /* Add transform reference to list */
        list->xfrms[list->num_xfrms] = xfrm;
        list->num_xfrms++;
    }

    /* Resize array */
    SHR_IF_ERR_EXIT
        (xfrm_list_resize(unit, max_entries, list));

    /* If there are no transforms list, destroy list too */
    if (list->xfrms == NULL) {
        SHR_FREE(list);
        list = NULL;
    }

    *list_ptr = list;

 exit:
    if (SHR_FUNC_ERR()) {
        xfrm_list_destroy(list);
    }

    SHR_FUNC_EXIT();
}

void
bcmltm_db_dm_xfrm_list_ref_destroy(bcmltm_db_dm_xfrm_list_t *list)
{
    if (list == NULL) {
        return;
    }

    if (list->xfrms != NULL) {
        SHR_FREE(list->xfrms);
    }

    SHR_FREE(list);

    return;

}

bcmltm_db_dm_xfrm_t *
bcmltm_db_dm_xfrm_find(const bcmltm_db_dm_xfrm_list_t *list,
                       const bcmltm_db_map_entry_t *map_entry)
{
    bcmltm_db_dm_xfrm_t *xfrm = NULL;
    uint32_t idx;

    if ((list == NULL) || (list->num_xfrms == 0)) {
        return NULL;
    }

    for (idx = 0; idx < list->num_xfrms; idx++) {
        xfrm = list->xfrms[idx];

        if ((xfrm->map_entry.group_idx == map_entry->group_idx) &&
            (xfrm->map_entry.entry_idx == map_entry->entry_idx)) {
            return xfrm;
        }
    }

    return NULL;
}

