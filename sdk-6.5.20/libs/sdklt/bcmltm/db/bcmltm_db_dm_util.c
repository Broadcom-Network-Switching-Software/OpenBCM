/*! \file bcmltm_db_dm_util.c
 *
 * Logical Table Manager - Direct Mapped utility functions.
 *
 * This file contains utility routines used in the construction
 * of information for direct map tables.
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

#include <bcmdrd/bcmdrd_pt.h>

#include <bcmltd/bcmltd_types.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/chip/bcmlrd_limits.h>

#include <bcmltm/bcmltm_types.h>
#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_wb_lt_internal.h>
#include <bcmltm/bcmltm_wb_lt_pvt_internal.h>
#include <bcmltm/bcmltm_wb_ptm_internal.h>
#include <bcmltm/bcmltm_lta_intf_internal.h>
#include <bcmltm/bcmltm_db_core_internal.h>

#include "bcmltm_db_map.h"
#include "bcmltm_db_md_util.h"
#include "bcmltm_db_dm_util.h"

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/* Maximum number of field indexes in a field */
#define MAX_FIELD_IDXS             50


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Allocate a field map object.
 *
 * Allocate a field map object from the given field map list.
 * This function does not fill field map information.
 *
 * If the field is already present in the list, memory is allocated for
 * the field map and appended to the link list of that existing field.
 * If the field is new, the element in the field map array is used.
 *
 * As a result, the new field map is allocated and placed such that
 * maps with the same field ID and indexes are across the link list of
 * the array element.  This organization is used for creating the
 * corresponding node cookie.
 *
 * Note that other than maps with matching (fid, idx) being placed
 * across the link list of an array element, the elements in the list array
 * are not ordered.
 *
 * \param [in] unit Unit number.
 * \param [in] lfid Logical field ID.
 * \param [in] idx Index of field elements.
 * \param [in] flist Field list to add new field map to.
 * \param [in] field_map_ptr Returns pointer to new field map.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_map_alloc(int unit,
                uint32_t lfid,
                uint32_t idx,
                bcmltm_map_field_mapped_list_t *flist,
                bcmltm_map_field_mapped_t **field_map_ptr)
{
    bcmltm_map_field_mapped_t *field_map = NULL;
    bcmltm_map_field_mapped_t *new_field_map = NULL;
    uint32_t map_idx = 0;
    uint32_t found = 0;
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    /* Check if (field_id, index) is new to the list */
    for (i = 0; i < flist->num; i++) {
        field_map = &flist->field_maps[i];
        if ((field_map->src_field_id == lfid) &&
            (field_map->src_field_idx == idx)) {
            map_idx = i;
            found = 1;
            break;
        }
    }

    if (!found) {
        /* New field, return array element */
        *field_map_ptr = &flist->field_maps[flist->num++];
        SHR_EXIT();
    }

    /* Field is not new, add map into corresponding element link list */
    field_map = &flist->field_maps[map_idx];

    /* Allocate */
    SHR_ALLOC(new_field_map, sizeof(*new_field_map), "bcmltmDmMapField");
    SHR_NULL_CHECK(new_field_map, SHR_E_MEMORY);
    sal_memset(new_field_map, 0, sizeof(*new_field_map));

    /* Insert it first in the link list (order does not matter) */
    new_field_map->next = field_map->next;
    field_map->next = new_field_map;

    *field_map_ptr = new_field_map;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the fixed field metadata for given LRD field entry map.
 *
 * Get the fixed field metadata for given LRD field entry map.
 *
 * \param [in] unit Unit number.
 * \param [in] attr Table attributes.
 * \param [in] ptm_wb_block PTM working buffer block corresponding to the PT.
 * \param [in] map_entry LRD field entry map.
 * \param [out] field_map Field structure to fill.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
fixed_field_get(int unit,
                const bcmltm_table_attr_t *attr,
                const bcmltm_wb_block_t *ptm_wb_block,
                const bcmlrd_map_entry_t *map_entry,
                bcmltm_fixed_field_t *field_map)
{
    bcmltm_table_type_t table_type;
    bcmltm_field_type_t field_type;

    SHR_FUNC_ENTER(unit);

    table_type = attr->type;

    if (map_entry->entry_type == BCMLRD_MAP_ENTRY_FIXED_KEY) {
        field_type = BCMLTM_FIELD_TYPE_KEY;
    } else if (map_entry->entry_type == BCMLRD_MAP_ENTRY_FIXED_VALUE) {
        field_type = BCMLTM_FIELD_TYPE_VALUE;
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Incorrect map entry type for "
                              "fixed field map: entry_type=%d\n"),
                   map_entry->entry_type));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Set working buffer offset */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_ptm_field_lt_offset_get(ptm_wb_block,
                                           map_entry->desc.field_id,
                                           field_type,
                                           BCMLTM_FIELD_DIR_IN,
                                           map_entry->desc.entry_idx,
                                           &(field_map->wbc.buffer_offset)));

    /* Set min and max bit information */
    field_map->wbc.maxbit = map_entry->desc.maxbit;
    field_map->wbc.minbit = map_entry->desc.minbit;

    /* Set fixed value (entry in use) */
    field_map->field_value = map_entry->u.fixed.value;

    /* Set delete value (entry not in use)*/
    field_map->delete_value = map_entry->u.fixed.default_value;

    /* Set PT changed offset */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_ptm_pt_changed_offset_get(table_type,
                                             field_type,
                                             ptm_wb_block,
                                             map_entry->desc.entry_idx,
                                             &field_map->pt_changed_offset));

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the field map type for given LRD direct field map entry.
 *
 * This routine gets the field map type, Key or Value,
 * based on the LRD entry information.
 *
 * This is only applicable for a direct PT field map entry.
 *
 * A logical field can be mapped to more than one PT field.  It is
 * the mapped PT field that ultimately determines whether a field map
 * type is a Key or Value.
 *
 * \param [in] unit Unit number.
 * \param [in] table_type Table type.
 * \param [in] fid LT field ID.
 * \param [in] map LRD map.
 * \param [in] entry LRD map entry.
 * \param [out] field_type Returning field map type BCMLTM_FIELD_TYPE_xxx.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
map_entry_mapped_field_type_get(int unit,
                                bcmltm_table_type_t table_type,
                                uint32_t fid,
                                const bcmlrd_map_t *map,
                                const bcmlrd_map_entry_t *entry,
                                bcmltm_field_type_t *field_type)
{
    SHR_FUNC_ENTER(unit);

    /* Sanity check for map entry type */
    if ((entry->entry_type != BCMLRD_MAP_ENTRY_MAPPED_KEY) &&
        (entry->entry_type != BCMLRD_MAP_ENTRY_MAPPED_VALUE)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Incorrect map entry type for "
                              "direct PT field map: entry_type=%d\n"),
                   entry->entry_type));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    

    /* Get field map type from map entry type */
    if (BCMLTM_TABLE_TYPE_IS_INDEX(table_type)) {
        if (entry->entry_type == BCMLRD_MAP_ENTRY_MAPPED_KEY) {
            *field_type = BCMLTM_FIELD_TYPE_KEY;
        } else {
            *field_type = BCMLTM_FIELD_TYPE_VALUE;
        }

    } else {
        const bcmlrd_field_data_t *field_data = NULL;

        /* Get field map type from LT field definition */
        field_data = &map->field_data->field[fid];

        /*
         * Check for special virtual fields that are not flagged
         * in the LRD as Keys but are treated as Keys by the PTM interface
         * and therefore needs to be placed in the LTM Keys field list.
         *
         * Key fields for PTM/LTM are those fields that are necessary to
         * identify a resource in an operation.
         *
         * There are different reasons why a PT Key field may not be declared
         * as Key in the LT map file.  One of the reasons, for example,
         * is that for FLTG produces 2 LT fields per Key field
         * defined in a TCAM table, which does not apply to a virtual field
         * (i.e. INSTANCE).
         */
        if (entry->desc.field_id == BCMLRD_FIELD_INSTANCE) {
            *field_type = BCMLTM_FIELD_TYPE_KEY;
        } else {
            if (field_data->flags & BCMLRD_FIELD_F_KEY) {
                *field_type = BCMLTM_FIELD_TYPE_KEY;
            } else {
                *field_type = BCMLTM_FIELD_TYPE_VALUE;
            }
        }
    }

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse LRD map entry for a direct PT field map.
 *
 * This routine parses the LRD entry of types:
 *     BCMLRD_MAP_ENTRY_MAPPED_KEY
 *     BCMLRD_MAP_ENTRY_MAPPED_VALUE
 *
 * It also gets any other information needed to construct the
 * metadata expected in bcmltm_field_map_t.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in] ptm_wb_block PTM working buffer block corresponding to the PT.
 * \param [in] map LRD map.
 * \param [in] entry LRD map entry to parse.
 * \param [out] lt_map LT map structure to fill.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
map_entry_mapped_parse(int unit, bcmlrd_sid_t sid,
                       const bcmltm_table_attr_t *attr,
                       const bcmltm_wb_block_t *ptm_wb_block,
                       const bcmlrd_map_t *map,
                       const bcmlrd_map_entry_t *entry,
                       bcmltm_db_dm_map_entries_t *lt_map)
{
    bcmltm_map_field_mapped_t *field_map = NULL;
    bcmltm_map_field_mapped_list_t *flist;
    uint32_t lfid, idx_count, idx;
    bcmltm_field_type_t field_type = BCMLRD_FIELD_F_KEY;
    uint16_t fidx_minbit[MAX_FIELD_IDXS];
    uint16_t fidx_maxbit[MAX_FIELD_IDXS];
    uint32_t num_fidx;
    uint64_t *default_value = NULL;
    uint32_t default_count;
    uint64_t *minimum_value = NULL;
    uint32_t minimum_count;
    uint64_t *maximum_value = NULL;
    uint32_t maximum_count;

    SHR_FUNC_ENTER(unit);

    /* Logical source field ID */
    lfid = entry->u.mapped.src.field_id;

    /* Key or Value Field Map */
    SHR_IF_ERR_EXIT
        (map_entry_mapped_field_type_get(unit, attr->type, lfid,
                                         map, entry, &field_type));
    if (field_type == BCMLTM_FIELD_TYPE_KEY) {
        flist = &lt_map->keys;
    } else {
        flist = &lt_map->values;
    }

    /* Get field min and max bits per index */
    SHR_IF_ERR_EXIT
        (bcmltm_db_field_to_fidx_min_max(unit, entry->desc.field_id,
                                         entry->desc.minbit,
                                         entry->desc.maxbit,
                                         MAX_FIELD_IDXS,
                                         fidx_minbit, fidx_maxbit,
                                         &num_fidx));
    /*
     * An LRD direct map entry is expected only for a mapped field.
     * An unmapped field at this point is considered an error.
     */

    /* Get number of field indices */
    idx_count = bcmlrd_field_idx_count_get(unit, sid, lfid);

    /* All fields has at least 1 index count for a mapped entry */
    if (idx_count < 1) {
        const char *table_name;
        const char *field_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        field_name = bcmltm_lt_field_fid_to_name(unit, sid, lfid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Invalid field index count of zero "
                              "for mapped field: "
                              "%s(ltid=%d) %s(fid=%d)\n"),
                   table_name, sid, field_name, lfid));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Get SW default value for field */
    SHR_ALLOC(default_value, sizeof(*default_value) * idx_count,
              "bcmltmDmFieldDefVals");
    SHR_NULL_CHECK(default_value, SHR_E_MEMORY);
    sal_memset(default_value, 0, sizeof(*default_value) * idx_count);
    SHR_IF_ERR_EXIT
        (bcmlrd_field_default_get(unit, sid, lfid,
                                  idx_count,
                                  default_value,
                                  &default_count));
    if (idx_count != default_count) {
        const char *table_name;
        const char *field_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        field_name = bcmltm_lt_field_fid_to_name(unit, sid, lfid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Invalid index count for field defaults: "
                              "%s(ltid=%d) %s(fid=%d) "
                              "expected=%d actual=%d\n"),
                   table_name, sid, field_name, lfid,
                   idx_count, default_count));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Get SW minimum value for field */
    SHR_ALLOC(minimum_value, sizeof(*minimum_value) * idx_count,
              "bcmltmDmFieldMinVals");
    SHR_NULL_CHECK(minimum_value, SHR_E_MEMORY);
    sal_memset(minimum_value, 0, sizeof(*minimum_value) * idx_count);
    SHR_IF_ERR_EXIT
        (bcmlrd_field_min_get(unit, sid, lfid,
                              idx_count,
                              minimum_value,
                              &minimum_count));
    if (idx_count != minimum_count) {
        const char *table_name;
        const char *field_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        field_name = bcmltm_lt_field_fid_to_name(unit, sid, lfid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Invalid index count for field minimums: "
                              "%s(ltid=%d) %s(fid=%d) "
                              "expected=%d actual=%d\n"),
                   table_name, sid, field_name, lfid,
                   idx_count, minimum_count));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Get SW maximum value for field */
    SHR_ALLOC(maximum_value, sizeof(*maximum_value) * idx_count,
              "bcmltmDmFieldMaxVals");
    SHR_NULL_CHECK(maximum_value, SHR_E_MEMORY);
    sal_memset(maximum_value, 0, sizeof(*maximum_value) * idx_count);
    SHR_IF_ERR_EXIT
        (bcmlrd_field_max_get(unit, sid, lfid,
                              idx_count,
                              maximum_value,
                              &maximum_count));
    if (idx_count != maximum_count) {
        const char *table_name;
        const char *field_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        field_name = bcmltm_lt_field_fid_to_name(unit, sid, lfid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Invalid index count for field maximums: "
                              "%s(ltid=%d) %s(fid=%d) "
                              "expected=%d actual=%d\n"),
                   table_name, sid, field_name, lfid,
                   idx_count, maximum_count));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    for (idx = 0; idx < idx_count; idx++) {
        /* Allocate new map object in list */
        SHR_IF_ERR_EXIT
            (field_map_alloc(unit, lfid, idx, flist, &field_map));

        field_map->src_field_idx = idx;
        field_map->src_field_id = lfid;
        field_map->default_value = default_value[idx];
        field_map->minimum_value = minimum_value[idx];
        field_map->maximum_value = maximum_value[idx];
        field_map->dst_field_id = entry->desc.field_id;
        field_map->ptm_wb_block = ptm_wb_block;
        field_map->pt_entry_idx = entry->desc.entry_idx;
        /* Set WBC min and max bit information */
        if (idx < num_fidx) {
            field_map->wbc_minbit = fidx_minbit[idx];
            field_map->wbc_maxbit = fidx_maxbit[idx];
        } else {
            /*
             * Corresponding PTM min, max range doesn't exist.
             * To avoid PTM write, set BCMLTM_FIELD_BIT_POSITION_INVALID
             * for both minbit and maxbit.
             */
            field_map->wbc_minbit = BCMLTM_FIELD_BIT_POSITION_INVALID;
            field_map->wbc_maxbit = BCMLTM_FIELD_BIT_POSITION_INVALID;
        }
    }

 exit:
    SHR_FREE(maximum_value);
    SHR_FREE(minimum_value);
    SHR_FREE(default_value);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse LRD map entry for a fixed field map.
 *
 * This routine parses the LRD entry of types:
 *     BCMLRD_MAP_ENTRY_FIXED_KEY
 *     BCMLRD_MAP_ENTRY_FIXED_VALUE
 *
 * It also gets any other information needed to construct the
 * metadata expected in bcmltm_fixed_field_t.
 *
 * \param [in] unit Unit number.
 * \param [in] attr Table attributes.
 * \param [in] ptm_wb_block PTM working buffer block corresponding to the PT.
 * \param [in] entry LRD map entry to parse.
 * \param [out] lt_map LT map structure to fill.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
map_entry_fixed_parse(int unit,
                      const bcmltm_table_attr_t *attr,
                      const bcmltm_wb_block_t *ptm_wb_block,
                      const bcmlrd_map_entry_t *entry,
                      bcmltm_db_dm_map_entries_t *lt_map)
{
    bcmltm_map_field_fixed_list_t *fixed_fields;
    bcmltm_fixed_field_t *field;

    SHR_FUNC_ENTER(unit);

    if (entry->entry_type == BCMLRD_MAP_ENTRY_FIXED_KEY) {
        fixed_fields = &lt_map->fixed_keys;
    } else if (entry->entry_type == BCMLRD_MAP_ENTRY_FIXED_VALUE) {
        fixed_fields = &lt_map->fixed_values;
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid fixed field map entry type\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Check array size */
    if (fixed_fields->num >= fixed_fields->max) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Fill information */
    field = &fixed_fields->field_maps[fixed_fields->num];
    SHR_IF_ERR_EXIT
        (fixed_field_get(unit, attr, ptm_wb_block, entry, field));
    fixed_fields->num++;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse LRD map entry for field transforms.
 *
 * This routine parses the LRD entry of types:
 *     BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER
 *     BCMLRD_MAP_ENTRY_FWD_VALUE_FIELD_XFRM_HANDLER
 *     BCMLRD_MAP_ENTRY_REV_KEY_FIELD_XFRM_HANDLER
 *     BCMLRD_MAP_ENTRY_REV_VALUE_FIELD_XFRM_HANDLER
 *     BCMLRD_MAP_ENTRY_ALWAYS_REV_VALUE_FIELD_XFRM_HANDLER
 *
 * It also gets any other information needed to construct the
 * metadata expected for the field transform node cookies.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] ptid Physical table ID corresponding to the PT map group.
 * \param [in] ptm_wb_block PTM working buffer block corresponding to the PT.
 * \param [in] entry LRD map entry to parse.
 * \param [out] lt_map LT map structure to fill.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
map_entry_xfrm_parse(int unit,
                     bcmlrd_sid_t sid,
                     bcmdrd_sid_t ptid,
                     const bcmltm_wb_block_t *ptm_wb_block,
                     const bcmlrd_map_entry_t *entry,
                     bcmltm_db_dm_map_entries_t *lt_map)
{
    bcmltm_map_field_xfrm_list_t *xfrm_list;
    bcmltm_map_xfrm_t *xfrm;
    const bcmltd_xfrm_handler_t *xfrm_handler = NULL;
    bcmltm_xfrm_dst_fields_t *xfrm_dst = NULL;
    uint32_t idx;
    uint32_t num_fields;
    uint32_t findex;
    bcmltm_field_dir_t field_dir;

    SHR_FUNC_ENTER(unit);

    /* Sanity check and get handler */
    SHR_IF_ERR_EXIT
        (bcmltm_db_xfrm_check(unit, sid, entry, &xfrm_handler));

    if (entry->entry_type == BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER) {
        xfrm_list = &lt_map->fwd_key_xfrms;
        field_dir = BCMLTM_FIELD_DIR_IN;
    } else if (entry->entry_type ==
               BCMLRD_MAP_ENTRY_FWD_VALUE_FIELD_XFRM_HANDLER) {
        xfrm_list = &lt_map->fwd_value_xfrms;
        field_dir = BCMLTM_FIELD_DIR_IN;
    } else if (entry->entry_type ==
               BCMLRD_MAP_ENTRY_REV_KEY_FIELD_XFRM_HANDLER) {
        xfrm_list = &lt_map->rev_key_xfrms;
        field_dir = BCMLTM_FIELD_DIR_OUT;
    } else if ((entry->entry_type ==
                BCMLRD_MAP_ENTRY_REV_VALUE_FIELD_XFRM_HANDLER) ||
               (entry->entry_type ==
                BCMLRD_MAP_ENTRY_ALWAYS_REV_VALUE_FIELD_XFRM_HANDLER)) {
        xfrm_list = &lt_map->rev_value_xfrms;
        field_dir = BCMLTM_FIELD_DIR_OUT;
    } else {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "%s(ltid=%d): Unsupported entry type\n"),
                       table_name, sid));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    idx = xfrm_list->num;
    if (idx >= xfrm_list->max) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "%s(ltid=%d): "
                              "Transform index is out of bound "
                              "idx=%d max=%d\n"),
                   table_name, sid,
                   idx, xfrm_list->max));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    xfrm = &xfrm_list->xfrms[idx];

    /* Set transform type and handler */
    xfrm->entry_type = entry->entry_type;
    sal_memcpy(&xfrm->handler, xfrm_handler, sizeof(bcmltd_xfrm_handler_t));

    /* Process transform destination fields */
    if ((entry->entry_type == BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER) ||
        (entry->entry_type == BCMLRD_MAP_ENTRY_FWD_VALUE_FIELD_XFRM_HANDLER)) {
        num_fields = xfrm_handler->arg->rfields;
    } else {
        num_fields = xfrm_handler->arg->fields;
    }
    xfrm_dst = &xfrm->dst;
    xfrm_dst->ptid = ptid;
    xfrm_dst->num = num_fields;

    if (num_fields == 0) {
        xfrm_list->num = ++idx;
        SHR_EXIT();
    }

    SHR_ALLOC(xfrm_dst->field,
              sizeof(bcmltm_xfrm_dst_field_t) * num_fields,
              "bcmltmDbMapXfrmDstFieldArr");
    SHR_NULL_CHECK(xfrm_dst->field, SHR_E_MEMORY);
    sal_memset(xfrm_dst->field, 0,
               sizeof(bcmltm_xfrm_dst_field_t) * num_fields);

    /* field list in xfrm */
    for (findex = 0; findex < num_fields ; findex++) {
        const bcmltd_field_desc_t *dst_field_desc;
        bcmltm_xfrm_dst_field_t dst_field;
        uint32_t field_id;  /* PT field ID */
        bcmltm_field_type_t field_type;
        uint16_t pt_entry_idx;

        if ((entry->entry_type ==
             BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER) ||
            (entry->entry_type ==
             BCMLRD_MAP_ENTRY_FWD_VALUE_FIELD_XFRM_HANDLER)) {
            dst_field_desc =
                &xfrm_handler->arg->rfield_list->field_array[findex];
        } else {
            dst_field_desc =
                &xfrm_handler->arg->field_list->field_array[findex];
        }

        field_id = dst_field_desc->field_id;
        pt_entry_idx = dst_field_desc->entry_idx;

        /*
         * If entry_type is KEY transform handler,
         * destination physical field would be key field.
         */
        if ((entry->entry_type ==
             BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER) ||
            (entry->entry_type ==
             BCMLRD_MAP_ENTRY_REV_KEY_FIELD_XFRM_HANDLER)) {
            field_type = BCMLTM_FIELD_TYPE_KEY;
        } else {
            field_type = BCMLTM_FIELD_TYPE_VALUE;
        }

        /*
         * Get offset from corresponding WB block:
         * - Track index field resides in the LT Private block.
         * - PT suppress field resides in the LT Private block.
         * - Other destination fields reside in the PTM block.
         */
        if (field_id == BCMLRD_FIELD_TRACK_INDEX) {
            SHR_IF_ERR_EXIT
                (bcmltm_wb_lt_track_index_offset_get_by_sid(unit, sid,
                                      &dst_field.wbc.buffer_offset));
        } else if (field_id == BCMLRD_FIELD_PT_SUPPRESS) {
            SHR_IF_ERR_EXIT
                (bcmltm_wb_lt_pt_suppress_offset_get_by_sid(unit, sid,
                                      &dst_field.wbc.buffer_offset));
        } else {
            SHR_IF_ERR_EXIT
                (bcmltm_wb_ptm_field_lt_offset_get(ptm_wb_block,
                                                   field_id, field_type,
                                                   field_dir,
                                                   pt_entry_idx,
                                             &(dst_field.wbc.buffer_offset)));
        }

        dst_field.wbc.maxbit = dst_field_desc->maxbit;
        dst_field.wbc.minbit = dst_field_desc->minbit;
        dst_field.field_id = field_id;
        dst_field.pt_entry_idx = pt_entry_idx;

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Table %d (%d), field (%d), "
                                "wb offset, min, max = (%d, %d, %d)\n"),
                     sid, ptid, field_id,
                     dst_field.wbc.buffer_offset,
                     dst_field.wbc.minbit,
                     dst_field.wbc.maxbit));

        sal_memcpy(&xfrm_dst->field[findex], &dst_field,
                   sizeof(bcmltm_xfrm_dst_field_t));

        /* Set special indicators based on field IDs */
        if (field_id == BCMLRD_FIELD_TRACK_INDEX) {
            /* TRACK_INDEX indicates transform provides the track index */
            if (entry->entry_type ==
                BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER) {
                lt_map->track_index_map = TRUE;
            }
        } else if (field_id == BCMLRD_FIELD_PT_SUPPRESS) {
            /* PT_SUPPRESS indicates PTM operations can be suppressed */
            if (entry->entry_type ==
                BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER) {
                lt_map->pt_suppress = TRUE;
            }
        }
    }

    /* Update list count */
    xfrm_list->num = ++idx;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize LT field list map information.
 *
 * This routine initializes the LT field list map information.
 *
 * \param [in] unit Unit number.
 * \param [in] flist_count Array size.
 * \param [out] flist LT field list map to initialize.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
map_field_mapped_init(int unit,
                      uint32_t flist_count,
                      bcmltm_map_field_mapped_list_t *flist)
{
    bcmltm_map_field_mapped_t *field_maps = NULL;
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    flist->max = flist_count;
    flist->num = 0;
    flist->field_maps = NULL;

    if (flist_count == 0) {
        SHR_EXIT();
    }

    /* Allocate field map array */
    SHR_ALLOC(field_maps, sizeof(*field_maps) * flist_count,
              "bcmltmDmMapFields");
    SHR_NULL_CHECK(field_maps, SHR_E_MEMORY);
    sal_memset(field_maps, 0, sizeof(*field_maps) * flist_count);
    flist->field_maps = field_maps;

    /* Initialize field index to indicate all fields are invalid */
    for (i = 0; i < flist_count; i++) {
        field_maps[i].src_field_idx = BCMLTM_FIELD_INDEX_UNDEFINED;
    }

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup LT field list map information.
 *
 * Cleanup LT field list map information.
 *
 * \param [out] flist - Field list to cleanup.
 */
static void
map_field_mapped_cleanup(bcmltm_map_field_mapped_list_t *flist)
{
    uint32_t idx;
    bcmltm_map_field_mapped_t *field_map = NULL;
    bcmltm_map_field_mapped_t *next_map = NULL;

    if (flist->field_maps != NULL) {
        /* Free elements in link list */
        for (idx = 0; idx < flist->max; idx++) {
            field_map = flist->field_maps[idx].next;
            while (field_map != NULL) {
                next_map = field_map->next;
                SHR_FREE(field_map);
                field_map = next_map;
            }
        }

        SHR_FREE(flist->field_maps);
    }

    return;
}

/*!
 * \brief Count number of fixed field map entries.
 *
 * This routine counts the number of fixed field map entries.
 *
 * \param [in] unit Unit number.
 * \param [in] map LRD map.
 * \param [out] count Number of fixed field map entries.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
map_field_fixed_count_get(int unit,
                          const bcmlrd_map_t *map,
                          uint32_t *count)
{
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    uint32_t group_idx;
    uint32_t entry_idx;

    *count = 0;

    /* Count number of fixed field map entries */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];

        /* Skip groups that are not of Physical table kind */
        if (group->dest.kind != BCMLRD_MAP_PHYSICAL) {
            continue;
        }

        /* Process each map entry */
        for (entry_idx = 0; entry_idx < group->entries; entry_idx++) {
            entry = &group->entry[entry_idx];

            if ((entry->entry_type == BCMLRD_MAP_ENTRY_FIXED_KEY) ||
                (entry->entry_type == BCMLRD_MAP_ENTRY_FIXED_VALUE)) {
                (*count)++;
            }
        }
    }

    return SHR_E_NONE;
}

/*!
 * \brief Initialize LT fixed field list map information.
 *
 * This routine initializes the LT fixed field list map information.
 *
 * \param [in] unit Unit number.
 * \param [in] count Number of fixed field map entries.
 * \param [out] flist LT field list map to initialize.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
map_field_fixed_init(int unit,
                     uint32_t count,
                     bcmltm_map_field_fixed_list_t *flist)
{
    bcmltm_fixed_field_t *field_maps = NULL;

    SHR_FUNC_ENTER(unit);

    flist->max = count;
    flist->num = 0;
    flist->field_maps = NULL;

    /* Allocate field map array */
    if (count > 0) {
        SHR_ALLOC(field_maps, sizeof(*field_maps) * count,
                  "bcmltmDmMapFixedFields");
        SHR_NULL_CHECK(field_maps, SHR_E_MEMORY);
        sal_memset(field_maps, 0, sizeof(*field_maps) * count);
    }

    flist->field_maps = field_maps;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup LT fixed field list map information.
 *
 * Cleanup LT fixed field list map information.
 *
 * \param [out] flist - Field list to cleanup.
 */
static void
map_field_fixed_cleanup(bcmltm_map_field_fixed_list_t *flist)
{
    SHR_FREE(flist->field_maps);

    return;
}

/*!
 * \brief Count number of Transform entry.
 *
 * Count number of Transform entry.
 *
 * \param [in] unit Unit number.
 * \param [in] map LRD map.
 * \param [out] fwd_key_xfrm_count Number of forward Key Transform entry.
 * \param [out] fwd_value_xfrm_count Number of forward Value Transform entry.
 * \param [out] rev_key_xfrm_count Number of reverse Key Transform entry.
 * \param [out] rev_value_xfrm_count Number of reverse Value Transform entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
map_field_xfrm_count_get(int unit,
                         const bcmlrd_map_t *map,
                         uint32_t *fwd_key_xfrm_count,
                         uint32_t *fwd_value_xfrm_count,
                         uint32_t *rev_key_xfrm_count,
                         uint32_t *rev_value_xfrm_count)
{
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    uint32_t group_idx;
    uint32_t entry_idx;

    SHR_FUNC_ENTER(unit);

    /* Count number of transform entry */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];
        /* Process Physical table kind */
        if (group->dest.kind != BCMLRD_MAP_PHYSICAL) {
            continue;
        }
        /* Process each map entry */
        for (entry_idx = 0; entry_idx < group->entries; entry_idx++) {
            entry = &group->entry[entry_idx];
            if (entry->entry_type ==
                BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER) {
                (*fwd_key_xfrm_count)++;
            } else if (entry->entry_type ==
                       BCMLRD_MAP_ENTRY_FWD_VALUE_FIELD_XFRM_HANDLER) {
                (*fwd_value_xfrm_count)++;
            } else if (entry->entry_type ==
                       BCMLRD_MAP_ENTRY_REV_KEY_FIELD_XFRM_HANDLER) {
                (*rev_key_xfrm_count)++;
            } else if ((entry->entry_type ==
                        BCMLRD_MAP_ENTRY_REV_VALUE_FIELD_XFRM_HANDLER) ||
                       (entry->entry_type ==
                        BCMLRD_MAP_ENTRY_ALWAYS_REV_VALUE_FIELD_XFRM_HANDLER)) {
                (*rev_value_xfrm_count)++;
            }
        }
    }

    SHR_EXIT();

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize LT xfrm list map information.
 *
 * This routine initializes the LT xfrm list map information.
 *
 * \param [in] unit Unit number.
 * \param [in] xfrm_count Number of Field Transform.
 * \param [out] xfrm_list LT field list map to initialize.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
map_field_xfrm_init(int unit,
                    uint32_t xfrm_count,
                    bcmltm_map_field_xfrm_list_t *xfrm_list)
{
    uint32_t size;
    bcmltm_map_xfrm_t *xfrms = NULL;

    SHR_FUNC_ENTER(unit);

    xfrm_list->max = xfrm_count;
    xfrm_list->num = 0;
    xfrm_list->xfrms = NULL;

    if (xfrm_count == 0) {
        SHR_EXIT();
    }

    /* Allocate transform map array */
    size = sizeof(*xfrms) * xfrm_count;
    SHR_ALLOC(xfrms, size,
              "bcmltmDmMapXfrms");
    SHR_NULL_CHECK(xfrms, SHR_E_MEMORY);
    sal_memset(xfrms, 0, size);

    xfrm_list->xfrms = xfrms;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup LT field xfrm list map information.
 *
 * Cleanup LT field xfrm list map information.
 *
 * \param [out] xfrm_list xfrm list map to cleanup.
 */
static void
map_field_xfrm_cleanup(bcmltm_map_field_xfrm_list_t *xfrm_list)
{
    uint32_t idx;
    bcmltm_xfrm_dst_field_t *fields;

    if (xfrm_list == NULL) {
        return;
    }

    if (xfrm_list->xfrms != NULL) {
        /* Free fields list */
        for (idx = 0; idx < xfrm_list->num ; idx++) {
            fields = xfrm_list->xfrms[idx].dst.field;
            if (fields != NULL) {
                SHR_FREE(fields);
            }
        }

        SHR_FREE(xfrm_list->xfrms);
    }

    return;
}

/*!
 * \brief Get the maximum entry index for given map transform.
 *
 * This routine gets the maximum entry index for the given
 * map entry transform.
 *
 * \param [in] unit Unit number.
 * \param [in] entry LRD map entry.
 * \param [out] max_pt_entry_idx Maximum entry index in transform.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_dst_field_max_entry_idx_get(int unit,
                                 const bcmlrd_map_entry_t *entry,
                                 uint16_t *max_pt_entry_idx)
{
    const bcmltd_xfrm_handler_t *xfrm = NULL;
    const bcmltd_field_list_t *field_list;
    uint32_t idx;
    uint16_t pt_entry_idx;

    SHR_FUNC_ENTER(unit);

    /* Transform Handler */
    xfrm = bcmltm_lta_intf_xfrm_handler_get(unit,
                                            entry->u.xfrm.desc->handler_id);

    /* Sanity check */
    if (xfrm == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Transform Handler is NULL\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Get the right list for destination PT fields */
    if ((entry->entry_type == BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER) ||
        (entry->entry_type == BCMLRD_MAP_ENTRY_FWD_VALUE_FIELD_XFRM_HANDLER)) {
        field_list = xfrm->arg->rfield_list;
    } else {
        field_list = xfrm->arg->field_list;
    }

    *max_pt_entry_idx = 0;
    for (idx = 0; idx < field_list->field_num; idx++) {
        pt_entry_idx = field_list->field_array[idx].entry_idx;

        if (pt_entry_idx > *max_pt_entry_idx) {
            *max_pt_entry_idx = pt_entry_idx;
        }
    }

 exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Get the number of PT operations for given PT group.
 *
 * This routine gets the number of PT operations for the given
 * PT map group (i.e. PT ID).
 *
 * An LRD physical map group contains all the map entries associated
 * with a PT ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in] map_group LRD map group (type expected is BCMLRD_MAP_PHYSICAL).
 * \param [out] pt_op_count Number of PT operations required.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pt_op_count_get(int unit,
                bcmlrd_sid_t sid,
                const bcmltm_table_attr_t *attr,
                const bcmlrd_map_group_t *map_group,
                uint32_t *pt_op_count)
{
    bcmltm_table_type_t table_type;
    const bcmlrd_map_entry_t *map_entry;
    uint32_t idx;
    uint16_t pt_entry_idx;
    uint16_t max_pt_entry_idx = 0;

    SHR_FUNC_ENTER(unit);

    if (map_group->dest.kind != BCMLRD_MAP_PHYSICAL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    table_type = attr->type;

    if (BCMLTM_TABLE_TYPE_IS_KEYED(table_type)) {
        /*
         * Keyed tables only have 1 PTM operation per LT.
         *
         * The PTM WB for keyed tables have 4 buffers.
         * The 'entry_idx' is used to select one these
         * 4 keyed PTM buffers.
         *
         * The 'entry_idx' for keyed tables is [0..3].
         */
        *pt_op_count = 1;

    } else {
        /*
         * Index tables can have multiple PT operations per PT ID.
         * The number of PT operations for a PT is derived from the
         * maximum entry index specified (by 'entry_idx') in the
         * map entries within a LRD map group.
         *
         * The 'entry_idx' is used to select the PT operation
         * within a PT ID that corresponds to the map entry.
         */

        /* Process each map entry */
        pt_entry_idx = 0;
        for (idx = 0; idx < map_group->entries; idx++) {
            map_entry = &map_group->entry[idx];

            switch (map_entry->entry_type) {
            case BCMLRD_MAP_ENTRY_MAPPED_KEY:
            case BCMLRD_MAP_ENTRY_MAPPED_VALUE:
            case BCMLRD_MAP_ENTRY_FIXED_KEY:
            case BCMLRD_MAP_ENTRY_FIXED_VALUE:
                pt_entry_idx = map_entry->desc.entry_idx;
                break;

            case BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER:
            case BCMLRD_MAP_ENTRY_FWD_VALUE_FIELD_XFRM_HANDLER:
            case BCMLRD_MAP_ENTRY_REV_KEY_FIELD_XFRM_HANDLER:
            case BCMLRD_MAP_ENTRY_REV_VALUE_FIELD_XFRM_HANDLER:
            case BCMLRD_MAP_ENTRY_ALWAYS_REV_VALUE_FIELD_XFRM_HANDLER:
                SHR_IF_ERR_EXIT
                    (xfrm_dst_field_max_entry_idx_get(unit, map_entry,
                                                      &pt_entry_idx));
                break;

            default:
                break;
            }

            if (pt_entry_idx > max_pt_entry_idx) {
                max_pt_entry_idx = pt_entry_idx;
            }
        }

        /*
         * The 'entry_idx' numbering should be compact starting at zero
         * [0..max_idx], so the count of PT ops is the
         * max 'entry_idx' + 1.
         */
        *pt_op_count = max_pt_entry_idx + 1;
    }

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get table instance access for a mapped PT.
 *
 * This routine gets the table instance access for
 * the given mapped PT in the specified logical table.
 *
 * The presence of a map to a virtual PT table instance field
 * indicates that the PT operates in unique access mode.  Otherwise,
 * the PT access mode is global.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] ptid Physical table ID.
 * \param [out] table_inst TRUE, PT has table instance (unique).
 *                         FALSE, PT does not have table instance (global).
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pt_table_inst_access_get(int unit,
                         bcmlrd_sid_t sid,
                         bcmdrd_sid_t ptid,
                         bool *table_inst)
{
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    uint32_t group_idx;
    uint32_t entry_idx;
    bool found = FALSE;

    SHR_FUNC_ENTER(unit);

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT
        (bcmlrd_map_get(unit, sid, &map));

    /* Check group map */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];
        /* Skip non-PT groups */
        if (group->dest.kind != BCMLRD_MAP_PHYSICAL) {
            continue;
        }

        if (group->dest.id != ptid) {
            continue;
        }

        /*
         * Check map entries for virtual destination PT table instance.
         *
         * It is only necessary to find one per group
         * since the table instance access is per PT.
         */
        for (entry_idx = 0; (entry_idx < group->entries) && !found;
             entry_idx++) {
            entry = &group->entry[entry_idx];

            switch (entry->entry_type) {
            case BCMLRD_MAP_ENTRY_MAPPED_KEY: /* Direct mapped field */
            case BCMLRD_MAP_ENTRY_FIXED_KEY:  /* Fixed field */
                if (entry->desc.field_id == BCMLRD_FIELD_INSTANCE) {
                    found = TRUE;
                }
                break;

            case BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER: /* Transform */
                if (bcmltm_db_xfrm_field_find(unit, entry, 0,
                                              BCMLRD_FIELD_INSTANCE)) {
                    found = TRUE;
                }
                break;

            default:
                break;
            }
        }
    }

    *table_inst = found;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check for PT SID selection in LRD map physical group.
 *
 * This routine checks if given PT ID in LRD map physical group
 * is subject to selection in logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] group LRD physical group.
 * \param [out] pt_sid_sel TRUE, if PT SID selection applies.
 *                         FALSE, if there is no PT SID selection.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pt_sid_sel_get(int unit,
               const bcmlrd_map_group_t *group,
               bool *pt_sid_sel)
{
    const bcmlrd_map_entry_t *entry = NULL;
    const bcmltd_xfrm_handler_t *xfrm = NULL;
    uint32_t entry_idx;
    bool found = FALSE;

    SHR_FUNC_ENTER(unit);

    *pt_sid_sel = FALSE;

    /*
     * Check map entry
     *
     * It is only necessary to find one entry per group
     * since PT SID selector is one per PT group.
     */
    for (entry_idx = 0; (entry_idx < group->entries) && !found;
         entry_idx++) {
        entry = &group->entry[entry_idx];

        switch (entry->entry_type) {
        case BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER:
            xfrm = bcmltm_lta_intf_xfrm_handler_get(unit,
                                             entry->u.xfrm.desc->handler_id);
            if (xfrm != NULL) {
                if (xfrm->arg->tables > 0) {
                    found = TRUE;
                }
            }
            break;

        default:
            break;
        }
    }

    *pt_sid_sel = found;

    SHR_FUNC_EXIT();
}

/*!
 * \brief Find field mapping to PDD LDTYPE.
 *
 * This routine checks if the given logical table contains
 * a field mapping to the virtual destination field for
 * PDD logical data type LDTYPE.
 *
 * The search includes the following types of field map entries:
 *   - Direct field maps (key and value)
 *   - Fixed field maps (key and value)
 *   - Transform field maps (key and value)
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] ptid Physical table ID.
 *                  If an INVALID PT ID is provided, search is
 *                  done in all PT groups (any PT ID).
 *
 * \retval TRUE Map to PDD LDTYPE field is found.
 * \retval FALSE Map to PDD LDTYPE field is not found, or failure.
 */
static inline bool
map_dst_field_pdd_ldtype_find(int unit,
                              bcmlrd_sid_t sid,
                              bcmdrd_sid_t ptid)
{
    return bcmltm_db_map_dst_field_find(unit, sid, ptid,
                                        BCMLRD_FIELD_LDTYPE);
}

/*******************************************************************************
 * Public functions
 */

const bcmltm_wb_block_t *
bcmltm_db_dm_ptm_wb_block_find(int unit, bcmlrd_sid_t sid,
                               const bcmltm_db_dm_pt_info_t *pt_info,
                               bcmdrd_sid_t ptid)
{
    uint32_t i;
    const bcmltm_wb_block_t *wb_block = NULL;

    /* Find PT ID */
    for (i =  0; i < pt_info->num_pts; i++) {
        if (pt_info->pts[i].sid == ptid) {
            wb_block = bcmltm_wb_lt_block_get(unit, sid,
                                              pt_info->pts[i].wb_block_id);
            break;
        }
    }

    return wb_block;
}

int
bcmltm_db_dm_pt_info_create(int unit,
                            bcmlrd_sid_t sid,
                            const bcmltm_table_attr_t *attr,
                            bcmltm_db_dm_pt_info_t **pt_info_ptr)
{
    unsigned int size;
    bcmltm_db_dm_pt_info_t *pt_info = NULL;
    bcmltm_db_map_group_count_t group_count;
    bcmlrd_map_t const *map = NULL;
    const bcmlrd_map_group_t *group = NULL;
    bcmdrd_sid_t ptid;
    uint32_t num_ops = 0;
    uint32_t num_pt = 0;
    uint32_t group_idx;
    uint32_t pt_idx;
    uint32_t flags;
    bcmltm_wb_handle_t *wb_handle = NULL;
    bcmltm_wb_block_id_t wb_block_id;
    bcmltm_table_type_t table_type;
    bool pt_sid_sel;
    bool pdd_ldtype;

    SHR_FUNC_ENTER(unit);

    *pt_info_ptr = NULL;

    /* Get PT group count */
    SHR_IF_ERR_EXIT
        (bcmltm_db_map_group_count_get(unit, sid, &group_count));
    num_pt = group_count.dm;
    if (num_pt == 0) {
        SHR_EXIT();
    }

    size = sizeof(*pt_info) + (sizeof(bcmltm_db_pt_t) * num_pt);
    SHR_ALLOC(pt_info, size, "bcmltmDbDmPtInfo");
    SHR_NULL_CHECK(pt_info, SHR_E_MEMORY);
    sal_memset(pt_info, 0, size);

    /* Get table type */
    table_type = attr->type;

    /* Get Working Buffer handle */
    wb_handle = bcmltm_wb_lt_handle_get(unit, sid);

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));

    for (group_idx = 0, pt_idx = 0; group_idx < map->groups; group_idx++) {

        group = &map->group[group_idx];

        if (group->dest.kind != BCMLRD_MAP_PHYSICAL) {
            continue;
        }

        /* Sanity check, this should not occur unless above logic is wrong */
        if (pt_idx >= num_pt) {
            const char *table_name;

            table_name = bcmltm_lt_table_sid_to_name(unit, sid);
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "LTM metadata PT mapping: "
                                  "invalid pt index for %s(ltid=%d) "
                                  "pt_idx=%d num_pt=%d\n"),
                       table_name, sid, (int) pt_idx, (int) num_pt));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        ptid = group->dest.id;

        /* Get PT OP flags based on HW symbol properties */
        flags = bcmltm_wb_ptm_op_flags_lt_get(unit, ptid);

        /*
         * Override table instance flag if required.
         *
         * If table instance flag is set, this indicates PT
         * has UNIQUE access mode (i.e. PT has multiple instances).
         * This property can be overridden by the LT map.
         *
         * A PT in UNIQUE access mode is changed to GLOBAL access mode
         * if the LT has no maps to a destination table instance field for
         * the given PT.
         */
        if (flags & BCMLTM_PT_OP_FLAGS_TABLE_INST) {
            bool table_inst = false;

            SHR_IF_ERR_EXIT
                (pt_table_inst_access_get(unit, sid, ptid, &table_inst));
            if (!table_inst) {
                /* Set global mode */
                flags &= ~BCMLTM_PT_OP_FLAGS_TABLE_INST;
            }
        }

        /* Set PT SID selection flag */
        SHR_IF_ERR_EXIT
            (pt_sid_sel_get(unit, group, &pt_sid_sel));
        if (pt_sid_sel) {
            flags |= BCMLTM_PT_OP_FLAGS_PT_SID;
        }

        /* Set PT Entry Changed flag if applicable */
        if (bcmltm_wb_ptm_pt_changed_flag_valid(table_type)) {
            flags |= BCMLTM_PT_OP_FLAGS_PT_CHANGED;
        }

        /* Set PDD LDTYPE flag */
        pdd_ldtype = map_dst_field_pdd_ldtype_find(unit, sid, ptid);
        if (pdd_ldtype) {
            flags |= BCMLTM_PT_OP_FLAGS_PDD_LDTYPE;
        }

        /*
         * PTM working buffer block
         * - Non-Keyed : One block per PT.
         * - Keyed     : One block per LT regardless of number of PTs.
         *
         * Keyed LTs only have 1 PTM working buffer block because
         * there is only 1 PTM operation.
         */
        if ((pt_idx == 0) || !BCMLTM_TABLE_TYPE_IS_KEYED(table_type)) {
            /* Get number of PT ops */
            SHR_IF_ERR_EXIT
                (pt_op_count_get(unit, sid, attr, group, &num_ops));

            /* Add Working Buffer block */
            SHR_IF_ERR_EXIT
                (bcmltm_wb_block_ptm_lt_add(unit, ptid, table_type, num_ops,
                                            wb_handle, &wb_block_id, NULL));
        }

        pt_info->pts[pt_idx].sid = ptid;
        pt_info->pts[pt_idx].flags = flags;
        pt_info->pts[pt_idx].num_ops = num_ops;
        pt_info->pts[pt_idx].wb_block_id = wb_block_id;
        pt_info->num_pts++;
        pt_idx++;
    }

    *pt_info_ptr = pt_info;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_dm_pt_info_destroy(pt_info);
        *pt_info_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

void
bcmltm_db_dm_pt_info_destroy(bcmltm_db_dm_pt_info_t *pt_info)
{
    SHR_FREE(pt_info);

    return;
}

int
bcmltm_db_dm_map_entries_parse(int unit, bcmlrd_sid_t sid,
                               const bcmltm_table_attr_t *attr,
                               const bcmltm_db_dm_pt_info_t *pt_info,
                               bcmltm_db_dm_map_entries_t *lt_map)
{
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    uint32_t group_idx;
    uint32_t entry_idx;
    uint32_t num_fid;  /* Number of API facing fields (fid) */
    uint32_t num_fid_idx;  /* Total fields elements (fid, idx) */
    uint32_t num_fixed_fields;
    uint32_t num_fwd_key_xfrm = 0, num_fwd_value_xfrm = 0;
    uint32_t num_rev_key_xfrm = 0, num_rev_value_xfrm = 0;
    bcmdrd_sid_t ptid;
    const bcmltm_wb_block_t *ptm_wb_block = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(lt_map, 0, sizeof(*lt_map));

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));

    lt_map->pt_suppress = FALSE;
    lt_map->track_index_map = FALSE;

    /* Get number of mapped LT field IDs and field elements (fid,idx) */
    SHR_IF_ERR_EXIT
        (bcmltm_db_field_count_get(unit, sid,
                                   &num_fid, &num_fid_idx));

    /* Init Keys field maps */
    SHR_IF_ERR_EXIT
        (map_field_mapped_init(unit, num_fid_idx, &lt_map->keys));

    /* Init Values field maps */
    SHR_IF_ERR_EXIT
        (map_field_mapped_init(unit, num_fid_idx, &lt_map->values));

    /* Count number of fixed field maps */
    SHR_IF_ERR_EXIT
        (map_field_fixed_count_get(unit, map,
                                   &num_fixed_fields));

    /* Init Fixed Key field maps */
    SHR_IF_ERR_EXIT
        (map_field_fixed_init(unit, num_fixed_fields, &lt_map->fixed_keys));

    /* Init Fixed Value field maps */
    SHR_IF_ERR_EXIT
        (map_field_fixed_init(unit, num_fixed_fields, &lt_map->fixed_values));

    /* Count number of forward/reverse Field Transform */
    SHR_IF_ERR_EXIT
        (map_field_xfrm_count_get(unit, map,
                                  &num_fwd_key_xfrm, &num_fwd_value_xfrm,
                                  &num_rev_key_xfrm, &num_rev_value_xfrm));

    /* Init Forward Key Transform field maps */
    SHR_IF_ERR_EXIT
        (map_field_xfrm_init(unit, num_fwd_key_xfrm,
                             &lt_map->fwd_key_xfrms));

    /* Init Forward Value Transform field maps */
    SHR_IF_ERR_EXIT
        (map_field_xfrm_init(unit, num_fwd_value_xfrm,
                             &lt_map->fwd_value_xfrms));

    /* Init Reverse Key Transform field maps */
    SHR_IF_ERR_EXIT
        (map_field_xfrm_init(unit, num_rev_key_xfrm,
                             &lt_map->rev_key_xfrms));

    /* Init Reverse Value Transform field maps */
    SHR_IF_ERR_EXIT
        (map_field_xfrm_init(unit, num_rev_value_xfrm,
                             &lt_map->rev_value_xfrms));

    /* For each map group */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];

        /* Process Physical table kind */
        if (group->dest.kind != BCMLRD_MAP_PHYSICAL) {
            continue;
        }

        /* Find PTM working buffer block */
        ptid = group->dest.id;
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

        /* Process each map entry */
        for (entry_idx = 0; entry_idx < group->entries; entry_idx++) {
            entry = &group->entry[entry_idx];

            switch (entry->entry_type) {
            case BCMLRD_MAP_ENTRY_MAPPED_KEY:
            case BCMLRD_MAP_ENTRY_MAPPED_VALUE:
                SHR_IF_ERR_EXIT
                    (map_entry_mapped_parse(unit, sid, attr, ptm_wb_block,
                                            map, entry,
                                            lt_map));
                break;

            case BCMLRD_MAP_ENTRY_FIXED_KEY:
            case BCMLRD_MAP_ENTRY_FIXED_VALUE:
                SHR_IF_ERR_EXIT
                    (map_entry_fixed_parse(unit, attr, ptm_wb_block, entry,
                                           lt_map));
                break;

            case BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER:
            case BCMLRD_MAP_ENTRY_FWD_VALUE_FIELD_XFRM_HANDLER:
            case BCMLRD_MAP_ENTRY_REV_KEY_FIELD_XFRM_HANDLER:
            case BCMLRD_MAP_ENTRY_REV_VALUE_FIELD_XFRM_HANDLER:
            case BCMLRD_MAP_ENTRY_ALWAYS_REV_VALUE_FIELD_XFRM_HANDLER:
                SHR_IF_ERR_EXIT
                    (map_entry_xfrm_parse(unit, sid, ptid,
                                          ptm_wb_block, entry,
                                          lt_map));
                break;

            default:
                break;
            }
        }
    }

 exit:
    SHR_FUNC_EXIT();
}

void
bcmltm_db_dm_map_entries_cleanup(bcmltm_db_dm_map_entries_t *lt_map)
{
    map_field_mapped_cleanup(&lt_map->keys);
    map_field_mapped_cleanup(&lt_map->values);
    map_field_fixed_cleanup(&lt_map->fixed_keys);
    map_field_fixed_cleanup(&lt_map->fixed_values);
    map_field_xfrm_cleanup(&lt_map->fwd_key_xfrms);
    map_field_xfrm_cleanup(&lt_map->fwd_value_xfrms);
    map_field_xfrm_cleanup(&lt_map->rev_key_xfrms);
    map_field_xfrm_cleanup(&lt_map->rev_value_xfrms);

    return;
}

int
bcmltm_db_dm_pt_index_offsets_get(int unit, bcmlrd_sid_t sid,
                                  const bcmltm_pt_list_t *pt_list,
                                  uint32_t offset_max,
                                  uint32_t *offset, uint32_t *num_offset)
{
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    uint32_t group_idx;
    uint32_t entry_idx;
    bcmdrd_sid_t ptid;
    uint32_t fid = 0;
    uint32_t offset_count = 0;
    bool found = FALSE;

    SHR_FUNC_ENTER(unit);

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT
        (bcmlrd_map_get(unit, sid, &map));

    /*
     * Find the key field(s) mapped to the PT index.
     * Usually, the LT index corresponds to the PT index:
     *   __INDEX
     *
     * There are cases where a field is mapped to both __INDEX and also
     * to a memory parameter (i.e. __PORT, __INSTANCE).  In this case,
     * the offset is part of the 'index' offset (for index copy and
     * tracking purposes).
     *
     * Example of logical key field is mapped to both an index and
     * memory parameter:
     *           F1: PT1.__INDEX
     *           F1: PT2.__PORT
     */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];
        /* Skip non-PT groups */
        if (group->dest.kind != BCMLRD_MAP_PHYSICAL) {
            continue;
        }

        ptid = group->dest.id;
        found = FALSE;

        /*
         * Check map entry
         *
         * It is only necessary to find one entry per group
         * since a PT only has one index.
         */
        for (entry_idx = 0; (entry_idx < group->entries) && !found;
             entry_idx++) {
            entry = &group->entry[entry_idx];

            switch (entry->entry_type) {
            case BCMLRD_MAP_ENTRY_MAPPED_KEY: /* Direct mapped field */
                fid = entry->desc.field_id;
                if (fid == BCMLRD_FIELD_INDEX) {
                    found = TRUE;
                } else if ((fid == BCMLRD_FIELD_PORT) ||
                           (fid == BCMLRD_FIELD_INSTANCE)) {
                    /* Check if source is also mapped to __INDEX */
                    if (bcmltm_db_field_key_pt_index_mapped(unit, sid,
                                             entry->u.mapped.src.field_id)) {
                        found = TRUE;
                    }
                }
                break;

            case BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER: /* Transform */
                if (bcmltm_db_xfrm_field_find(unit, entry, 0,
                                              BCMLRD_FIELD_INDEX)) {
                    found = TRUE;
                    fid = BCMLRD_FIELD_INDEX;
                }
                break;

            default:
                break;
            }

        }

        /* Get offset */
        if (found) {
            if (offset_count >= offset_max) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }

            SHR_IF_ERR_EXIT
                (bcmltm_db_pt_param_offset_get(unit, pt_list, ptid, fid,
                                               &offset[offset_count]));
            offset_count++;
        }
    }

    *num_offset = offset_count;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_pt_mem_param_offsets_get(int unit, bcmlrd_sid_t sid,
                                      const bcmltm_pt_list_t *pt_list,
                                      uint32_t offset_max,
                                      uint32_t *offset, uint32_t *num_offset)
{
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    uint32_t group_idx;
    uint32_t entry_idx;
    bcmdrd_sid_t ptid;
    uint32_t fid = 0;
    uint32_t offset_count = 0;
    bool found = FALSE;

    SHR_FUNC_ENTER(unit);

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT
        (bcmlrd_map_get(unit, sid, &map));

    /*
     * Find the key field(s) mapped to the PT memory parameter.
     * The memory parameters are normally mapped to these virtual
     * destination fields:
     *   __PORT
     *   __INSTANCE
     *
     * If the source field also maps to the PT Index (__INDEX),
     * this should be part of the index offset and not
     * the memory parameter (for index copy and
     * tracking purposes).
     */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];
        /* Skip non-PT groups */
        if (group->dest.kind != BCMLRD_MAP_PHYSICAL) {
            continue;
        }

        ptid = group->dest.id;
        found = FALSE;

        /*
         * Check map entry
         *
         * It is only necessary to find one entry per group
         * since a PT only has one index.
         */
        for (entry_idx = 0; (entry_idx < group->entries) && !found;
             entry_idx++) {
            entry = &group->entry[entry_idx];

            switch (entry->entry_type) {
            case BCMLRD_MAP_ENTRY_MAPPED_KEY: /* Direct mapped field */
                fid = entry->desc.field_id;
                if ((fid == BCMLRD_FIELD_PORT) ||
                    (fid == BCMLRD_FIELD_INSTANCE)) {
                    /* Check if source is also mapped to __INDEX */
                    if (!bcmltm_db_field_key_pt_index_mapped(unit, sid,
                                              entry->u.mapped.src.field_id)) {
                        found = TRUE;
                    }
                }
                break;

            case BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER: /* Transform */
                if (bcmltm_db_xfrm_field_find(unit, entry, 0,
                                              BCMLRD_FIELD_PORT)) {
                    found = TRUE;
                    fid = BCMLRD_FIELD_PORT;
                } else if (bcmltm_db_xfrm_field_find(unit, entry, 0,
                                                     BCMLRD_FIELD_INSTANCE)) {
                    found = TRUE;
                    fid = BCMLRD_FIELD_INSTANCE;
                }
                break;

            default:
                break;
            }

        }

        /* Get offset */
        if (found) {
            if (offset_count >= offset_max) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }

            SHR_IF_ERR_EXIT
                (bcmltm_db_pt_param_offset_get(unit, pt_list, ptid, fid,
                                               &offset[offset_count]));
            offset_count++;
        }
    }

    *num_offset = offset_count;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_pt_sid_sel_offsets_get(int unit, bcmlrd_sid_t sid,
                                    const bcmltm_pt_list_t *pt_list,
                                    uint32_t offset_max,
                                    uint32_t *offset, uint32_t *num_offset)
{
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    const bcmltd_xfrm_handler_t *xfrm = NULL;
    uint32_t group_idx;
    uint32_t entry_idx;
    bcmdrd_sid_t ptid;
    uint32_t offset_count = 0;
    bool found = FALSE;

    SHR_FUNC_ENTER(unit);

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT
        (bcmlrd_map_get(unit, sid, &map));

    /* Find PT SID selector (specified through table transforms) */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];
        /* Skip non-PT groups */
        if (group->dest.kind != BCMLRD_MAP_PHYSICAL) {
            continue;
        }

        ptid = group->dest.id;
        found = FALSE;

        /*
         * Check map entry
         *
         * It is only necessary to find one entry per group
         * since PT SID selector is one per PT group.
         */
        for (entry_idx = 0; (entry_idx < group->entries) && !found;
             entry_idx++) {
            entry = &group->entry[entry_idx];

            switch (entry->entry_type) {
            case BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER:
                xfrm = bcmltm_lta_intf_xfrm_handler_get(unit,
                                    entry->u.xfrm.desc->handler_id);
                if (xfrm != NULL) {
                    if (xfrm->arg->tables > 0) {
                        found = TRUE;
                    }
                }
                break;

            default:
                break;
            }

        }

        /* Get offset */
        if (found) {
            if (offset_count >= offset_max) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }

            SHR_IF_ERR_EXIT
                (bcmltm_db_pt_param_offset_get(unit, pt_list, ptid,
                                               BCMLRD_FIELD_TABLE_SEL,
                                               &offset[offset_count]));
            offset_count++;
        }
    }

    *num_offset = offset_count;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_pt_suppress_offset_get(int unit,
                                    bcmlrd_sid_t sid,
                                    bool pt_suppress,
                                    uint32_t *offset)
{
    SHR_FUNC_ENTER(unit);

    *offset = BCMLTM_WB_OFFSET_INVALID;

    /* If PT suppress is available, get corresponding offset */
    if (pt_suppress) {
        SHR_IF_ERR_EXIT
            (bcmltm_wb_lt_pt_suppress_offset_get_by_sid(unit, sid, offset));
    }

 exit:
    SHR_FUNC_EXIT();
}
