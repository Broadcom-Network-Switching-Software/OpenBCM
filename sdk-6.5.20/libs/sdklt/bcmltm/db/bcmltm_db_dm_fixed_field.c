/*! \file bcmltm_db_dm_fixed_field.c
 *
 * Logical Table Manager - Information for Fixed Field Maps.
 *
 * This file contains interfaces to construct information for
 * fixed field maps with selections in direct mapped logical tables.
 *
 * Fixed field maps are used to copy specific values into
 * specified destination locations (PTM working buffer).
 *
 * The metadata created is used for the following LTM driver flow:
 * (field select opcode driver version)
 *     API <-> API Cache <-> [VALIDATE, XFRM] <-> PTM
 *
 * This driver flow can be applied to tables with or without field selections.
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

#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm_lta_intf_internal.h>
#include <bcmltm/bcmltm_types_internal.h>
#include <bcmltm/bcmltm_db_core_internal.h>
#include <bcmltm/bcmltm_wb_ptm_internal.h>
#include <bcmltm/bcmltm_wb_lt_internal.h>

#include "bcmltm_db_map.h"
#include "bcmltm_db_field_select.h"
#include "bcmltm_db_md_util.h"
#include "bcmltm_db_dm_util.h"
#include "bcmltm_db_dm_fixed_field.h"


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Check if map entry is to be processed for fixed fields.
 *
 * This helper routine checks whether the given map entry is to be
 * processed or skipped for fixed field maps.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] map LRD map.
 * \param [in] map_entry Map entry to check.
 * \param [in] fixed_type Desired fixed field type.
 *
 * \retval TRUE if map entry is to be processed for fixed fields.
 * \retval FALSE if map entry is to be skipped, or failure.
 */
static bool
fixed_field_map_entry_process(int unit,
                              bcmlrd_sid_t sid,
                              const bcmlrd_map_t *map,
                              const bcmltm_db_map_entry_t *map_entry,
                              bcmltm_fixed_field_type_t fixed_type)
{
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    bcmlrd_map_entry_type_t entry_type;

    /* Skip non physical map groups (non-DM) */
    group = bcmltm_db_map_lrd_group_get(map, map_entry);
    if (group->dest.kind != BCMLRD_MAP_PHYSICAL) {
        return FALSE;
    }

    /* Check fixed field type */
    entry = bcmltm_db_map_lrd_entry_get(map, map_entry);
    entry_type = entry->entry_type;

    /* Fixed key field */
    if ((fixed_type == BCMLTM_FIXED_FIELD_TYPE_KEY) &&
        (entry_type == BCMLRD_MAP_ENTRY_FIXED_KEY)) {
        return TRUE;
    }

    /* Fixed value field */
    if ((fixed_type == BCMLTM_FIXED_FIELD_TYPE_VALUE) &&
        (entry_type == BCMLRD_MAP_ENTRY_FIXED_VALUE)) {
        return TRUE;
    }

    /* No match */
    return FALSE;
}

/*!
 * \brief Add fixed field map metadata for given map entry.
 *
 * This routine adds the metadata for the given fixed field map entry
 * into the fixed field array.
 *
 * It assumes that the given map entry is the desired map entry type
 * to add to the field map array metadata.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] dm_arg DM arguments.
 * \param [in,out] num_fixed_fields Number of fields in array list.
 * \param [in,out] fixed_fields Fixed field list to add map metadata to.
 * \param [in] map LRD map.
 * \param [in] map_entry Map entry identifier of map to add metadata for.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
fixed_field_map_entry_add(int unit,
                          bcmlrd_sid_t sid,
                          const bcmltm_db_dm_arg_t *dm_arg,
                          uint32_t *num_fixed_fields,
                          bcmltm_fixed_field_t *fixed_fields,
                          const bcmlrd_map_t *map,
                          const bcmltm_db_map_entry_t *map_entry)
{
    bcmltm_fixed_field_t *field;
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    const bcmltm_wb_block_t *ptm_wb_block = NULL;
    bcmdrd_sid_t ptid;  /* PT ID */
    uint32_t pt_fid;    /* PT (destination) field ID */
    uint16_t pt_entry_idx;  /* PT entry index */
    uint32_t pt_field_offset = BCMLTM_WB_OFFSET_INVALID;
    uint32_t pt_changed_offset = BCMLTM_WB_OFFSET_INVALID;
    bcmltm_table_type_t table_type;
    bcmltm_field_type_t field_type;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    group = bcmltm_db_map_lrd_group_get(map, map_entry);
    entry = bcmltm_db_map_lrd_entry_get(map, map_entry);

    /* Get relevant field information */
    ptid = group->dest.id;
    pt_fid = entry->desc.field_id;
    pt_entry_idx = entry->desc.entry_idx;

    /* Find PTM working buffer block */
    ptm_wb_block = bcmltm_db_dm_ptm_wb_block_find(unit, sid,
                                                  dm_arg->pt_info, ptid);
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

    table_type = dm_arg->attr->type;

    /* Destination field type is based on the fixed field map type */
    if (entry->entry_type == BCMLRD_MAP_ENTRY_FIXED_KEY) {
        field_type = BCMLTM_FIELD_TYPE_KEY;
    } else if (entry->entry_type == BCMLRD_MAP_ENTRY_FIXED_VALUE) {
        field_type = BCMLTM_FIELD_TYPE_VALUE;
    } else {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Incorrect map entry type for "
                              "fixed field map: %s(ltid=%d) entry_type=%d\n"),
                   table_name, sid, entry->entry_type));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Get destination field offset */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_ptm_field_lt_offset_get(ptm_wb_block,
                                           pt_fid,
                                           field_type,
                                           BCMLTM_FIELD_DIR_IN,
                                           pt_entry_idx,
                                           &pt_field_offset));

    /* Get PT changed offset */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_ptm_pt_changed_offset_get(table_type,
                                             field_type,
                                             ptm_wb_block,
                                             pt_entry_idx,
                                             &pt_changed_offset));

    /* Get next field in array */
    idx = *num_fixed_fields;
    field = &fixed_fields[idx];
    (*num_fixed_fields)++;

    /* Fill data */
    field->wbc.buffer_offset = pt_field_offset;
    field->wbc.maxbit = entry->desc.maxbit;
    field->wbc.minbit = entry->desc.minbit;
    field->pt_changed_offset = pt_changed_offset;
    field->field_value = entry->u.fixed.value;
    field->delete_value = entry->u.fixed.default_value;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Resize fields array.
 *
 * This utility routine resizes the given fields array
 * to a smaller size, if necessary, in order to free unused memory.
 *
 * If a resize takes place, the previous map array is freed.
 *
 * \param [in] unit Unit number.
 * \param [in] max_fields Max fields allocated in array.
 * \param [in] num_fields Actual number of fields.
 * \param [in,out] fixed_fields_ptr Returning pointer to resized array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
fixed_fields_resize(int unit,
                    uint32_t max_fields,
                    uint32_t num_fields,
                    bcmltm_fixed_field_t **fixed_fields_ptr)
{
    bcmltm_fixed_field_t *fixed_fields = NULL;
    bcmltm_fixed_field_t *new_fixed_fields = NULL;
    unsigned int alloc_size;

    SHR_FUNC_ENTER(unit);

    fixed_fields = *fixed_fields_ptr;

    /* Sanity check */
    if (num_fields > max_fields) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Nothing to do if number of fields matches allocated max fields */
    if (num_fields == max_fields) {
        SHR_EXIT();
    }

    /* Free array if there are no field maps */
    if (num_fields == 0) {
        SHR_FREE(fixed_fields);
        *fixed_fields_ptr = NULL;
        SHR_EXIT();
    }

    /*
     * Reallocate smaller array and copy array elements.
     */
    alloc_size = sizeof(*new_fixed_fields) * num_fields;
    SHR_ALLOC(new_fixed_fields, alloc_size, "bcmltmDmFixedFields");
    SHR_NULL_CHECK(new_fixed_fields, SHR_E_MEMORY);
    sal_memcpy(new_fixed_fields, fixed_fields, alloc_size);

    /* Free old array and return new array */
    SHR_FREE(fixed_fields);
    *fixed_fields_ptr = new_fixed_fields;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the fixed field array metadata for given map entry list.
 *
 * This routine creates the fixed field array metadata for
 * the given map entry list.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] dm_arg DM arguments.
 * \param [in] map_entry_list Map entry list to process.
 * \param [in] fixed_type Fixed field type.
 * \param [out] fixed_fields_ptr Returning pointer to fixed field array.
 * \param [out] num_fixed_fields Number of fields returned in array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
fixed_fields_create(int unit,
                    bcmlrd_sid_t sid,
                    const bcmltm_db_dm_arg_t *dm_arg,
                    const bcmltm_db_map_entry_list_t *map_entry_list,
                    bcmltm_fixed_field_type_t fixed_type,
                    bcmltm_fixed_field_t **fixed_fields_ptr,
                    uint32_t *num_fixed_fields)

{
    bcmltm_fixed_field_t *fixed_fields = NULL;
    uint32_t fixed_field_count = 0;
    const bcmlrd_map_t *lrd_map = map_entry_list->map;
    const bcmltm_db_map_entry_t *map_entry;
    uint32_t max_entries;
    unsigned int alloc_size;
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    *num_fixed_fields = 0;
    *fixed_fields_ptr = NULL;

    /* Get max count of map entries in table */
    SHR_IF_ERR_EXIT
        (bcmltm_db_map_entries_count_get(unit, sid,
                                         &max_entries));
    if (max_entries == 0) {
        SHR_EXIT();
    }

    /* Allocate fixed fields array */
    alloc_size = sizeof(*fixed_fields) * max_entries;
    SHR_ALLOC(fixed_fields, alloc_size, "bcmltmDmFixedFields");
    SHR_NULL_CHECK(fixed_fields, SHR_E_MEMORY);
    sal_memset(fixed_fields, 0, alloc_size);

    /* Parse each map entry in list */
    for (i = 0; i < map_entry_list->num_entries; i++) {
        map_entry = &map_entry_list->entries[i];

        /* Check if map entry qualifies, if not skip */
        if (!fixed_field_map_entry_process(unit, sid,
                                           lrd_map, map_entry, fixed_type)) {
            continue;
        }

        /* Add map entry to list */
        SHR_IF_ERR_EXIT
            (fixed_field_map_entry_add(unit, sid, dm_arg,
                                       &fixed_field_count, fixed_fields,
                                       lrd_map, map_entry));
    }

    /* Resize fields array */
    SHR_IF_ERR_EXIT
        (fixed_fields_resize(unit, max_entries, fixed_field_count,
                             &fixed_fields));

    *num_fixed_fields = fixed_field_count;
    *fixed_fields_ptr = fixed_fields;

 exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(fixed_fields);
        *num_fixed_fields = 0;
        *fixed_fields_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the fixed field list metadata.
 *
 * This routine creates the fixed field list metadata for
 * the given map entry list.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] dm_arg DM arguments.
 * \param [in] map_entry_list Map entry list to process.
 * \param [in] fixed_type Fixed field type.
 * \param [out] list_ptr Returning pointer to fixed field list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
fixed_field_list_create(int unit,
                        bcmlrd_sid_t sid,
                        const bcmltm_db_dm_arg_t *dm_arg,
                        const bcmltm_db_map_entry_list_t *map_entry_list,
                        bcmltm_fixed_field_type_t fixed_type,
                        bcmltm_fixed_field_list_t **list_ptr)
{
    bcmltm_fixed_field_list_t *list = NULL;

    SHR_FUNC_ENTER(unit);

    /*
     * This metadata object should always be created even if
     * the field count is zero.  This is expected by the corresponding
     * FA node function logic.
     */

    /* Allocate */
    SHR_ALLOC(list, sizeof(*list), "bcmltmDmFixedFieldList");
    SHR_NULL_CHECK(list, SHR_E_MEMORY);
    sal_memset(list, 0, sizeof(*list));

    /* Create fixed fields array and fill data */
    SHR_IF_ERR_EXIT
        (fixed_fields_create(unit, sid, dm_arg,
                             map_entry_list,
                             fixed_type,
                             &list->fixed_fields,
                             &list->num_fixed_fields));

    *list_ptr = list;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_fixed_field_list_destroy(list);
        *list_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the fixed field mapping metadata.
 *
 * This routine creates the fixed field mapping metadata for
 * given field selection group and fixed field type.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] dm_arg DM arguments.
 * \param [in] selection Field selection group.
 * \param [in] fixed_type Fixed field type.
 * \param [out] info_ptr Returning pointer to fixed field mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
fixed_field_mapping_create(int unit,
                           bcmlrd_sid_t sid,
                           const bcmltm_db_dm_arg_t *dm_arg,
                           const bcmltm_db_field_selection_t *selection,
                           bcmltm_fixed_field_type_t fixed_type,
                           bcmltm_fixed_field_mapping_t **info_ptr)
{
    bcmltm_fixed_field_mapping_t *field_mapping = NULL;
    const bcmltm_db_field_selection_map_t *selection_map = NULL;
    const bcmltm_db_map_entry_list_t *map_entry_list;
    uint32_t num_maps;
    uint32_t alloc_size;
    uint32_t map_index_offset = BCMLTM_WB_OFFSET_INVALID;
    uint32_t map_index;
    uint32_t fm_idx;
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    *info_ptr = NULL;

    num_maps = selection->num_maps;

    /* Allocate */
    alloc_size = sizeof(*field_mapping) +
        sizeof(bcmltm_fixed_field_list_t *) * num_maps;
    SHR_ALLOC(field_mapping, alloc_size, "bcmltmDmFixedFieldMapping");
    SHR_NULL_CHECK(field_mapping, SHR_E_MEMORY);
    sal_memset(field_mapping, 0, alloc_size);

    /* Get selection map index offset */
    if (selection->selection_id != BCMLTM_FIELD_SELECTION_ID_UNCOND) {
        SHR_IF_ERR_EXIT
            (bcmltm_wb_lt_fs_map_index_offset_get_by_sid(unit, sid,
                                              selection->selection_id,
                                              &map_index_offset));
    }

    /* Fill data */
    field_mapping->map_index_offset = map_index_offset;
    field_mapping->num_maps = num_maps;

    for (i = 0; i < num_maps; i++) {
        selection_map = selection->maps[i];

        map_index = selection_map->map_index;
        if (map_index > num_maps) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        fm_idx = map_index - 1;
        map_entry_list = selection_map->map_entry_list;
        SHR_IF_ERR_EXIT
            (fixed_field_list_create(unit, sid, dm_arg,
                                     map_entry_list,
                                     fixed_type,
                                   &(field_mapping->fixed_field_list[fm_idx])));
    }

    *info_ptr = field_mapping;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_fixed_field_mapping_destroy(field_mapping);
        *info_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_db_dm_fixed_field_info_create(int unit,
                              bcmlrd_sid_t sid,
                              const bcmltm_db_dm_arg_t *dm_arg,
                              const bcmltm_db_field_selection_t *selection,
                              bcmltm_db_dm_fixed_field_info_t **info_ptr)
{
    bcmltm_db_dm_fixed_field_info_t *info = NULL;

    SHR_FUNC_ENTER(unit);

    *info_ptr = NULL;

    /* Allocate info struct */
    SHR_ALLOC(info, sizeof(*info), "bcmltmDbDmFixedFieldInfo");
    SHR_NULL_CHECK(info, SHR_E_MEMORY);
    sal_memset(info, 0, sizeof(*info));

    /* Create Fixed Key Fields */
    SHR_IF_ERR_EXIT
        (fixed_field_mapping_create(unit, sid, dm_arg,
                                    selection,
                                    BCMLTM_FIXED_FIELD_TYPE_KEY,
                                    &info->keys));

    /* Create Fixed Value Fields */
    SHR_IF_ERR_EXIT
        (fixed_field_mapping_create(unit, sid, dm_arg,
                                    selection,
                                    BCMLTM_FIXED_FIELD_TYPE_VALUE,
                                    &info->values));

    *info_ptr = info;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_dm_fixed_field_info_destroy(info);
        *info_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

void
bcmltm_db_dm_fixed_field_info_destroy(bcmltm_db_dm_fixed_field_info_t *info)
{
    if (info == NULL) {
        return;
    }

    /* Destroy Field Mappings */
    bcmltm_db_fixed_field_mapping_destroy(info->values);
    bcmltm_db_fixed_field_mapping_destroy(info->keys);

    SHR_FREE(info);

    return;
}
