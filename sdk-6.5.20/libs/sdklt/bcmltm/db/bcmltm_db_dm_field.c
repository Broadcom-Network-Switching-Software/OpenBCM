/*! \file bcmltm_db_dm_field.c
 *
 * Logical Table Manager - Information for Direct Field Maps.
 *
 * This file contains interfaces to construct information for
 * logical fields with selections that are directly mapped to
 * physical fields in direct mapped logical tables.
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

#include "bcmltm_db_util.h"
#include "bcmltm_db_map.h"
#include "bcmltm_db_field_select.h"
#include "bcmltm_db_md_util.h"
#include "bcmltm_db_dm_util.h"
#include "bcmltm_db_dm_field.h"


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/* Maximum number of field indexes (wide fields) in a field */
#define MAX_FIELD_IDXS             50

/*******************************************************************************
 * Private functions
 */

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
 * \param [in] fid Logical field ID.
 * \param [in] map LRD map.
 * \param [in] entry LRD map entry.
 * \param [out] field_type Returning field map type BCMLTM_FIELD_TYPE_xxx.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
map_entry_field_type_get(int unit,
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
 * \brief Check if map entry is to be processed based on input criteria.
 *
 * This helper routine checks whether the given map entry is to be
 * processed or skipped based on the specified criteria.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] attr Table attributes.
 * \param [in] map LRD map.
 * \param [in] map_entry Map entry to check.
 * \param [in] field_type Desired field type, key or value.
 * \param [in] key_type Desired key type (for IwA).
 *
 * \retval TRUE if map entry is to be processed based on criteria.
 * \retval FALSE if map entry is to be skipped, or failure.
 */
static bool
field_map_entry_process(int unit,
                        bcmlrd_sid_t sid,
                        const bcmltm_table_attr_t *attr,
                        const bcmlrd_map_t *map,
                        const bcmltm_db_map_entry_t *map_entry,
                        bcmltm_field_type_t field_type,
                        bcmltm_field_key_type_t key_type)
{
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    int rv;
    uint32_t fid;
    bcmltm_field_type_t entry_field_type;

    /* Skip non physical map groups (non-DM) */
    group = bcmltm_db_map_lrd_group_get(map, map_entry);
    if (group->dest.kind != BCMLRD_MAP_PHYSICAL) {
        return FALSE;
    }

    /* Skip non direct mapped field entries */
    entry = bcmltm_db_map_lrd_entry_get(map, map_entry);
    if ((entry->entry_type != BCMLRD_MAP_ENTRY_MAPPED_KEY) &&
        (entry->entry_type != BCMLRD_MAP_ENTRY_MAPPED_VALUE)) {
        return FALSE;
    }

    /* Logical source field ID */
    fid = entry->u.mapped.src.field_id;

    /* Check if field type matches */
    rv = map_entry_field_type_get(unit, attr->type, fid,
                                  map, entry, &entry_field_type);
    if (SHR_FAILURE(rv)) {
        return FALSE;
    }
    if (entry_field_type != field_type) {
        return FALSE;
    }

    /* Do further checks on key field type */
    if (field_type == BCMLTM_FIELD_TYPE_KEY) {
        /*
         * Skip key field if:
         *   - Request is required field AND key field is allocatable
         *   - Request is allocatable (opt) field AND key field is required
         */
        if (((key_type == BCMLTM_FIELD_KEY_TYPE_IWA_REQ) &&
             bcmltm_db_field_key_is_alloc(unit, sid, fid)) ||
            ((key_type == BCMLTM_FIELD_KEY_TYPE_IWA_ALLOC) &&
             !bcmltm_db_field_key_is_alloc(unit, sid, fid))) {
            return FALSE;
        }
    }

    /* Map entry matches criteria */
    return TRUE;
}

/*!
 * \brief Allocate a field map object.
 *
 * This routine allocates a field map object in the given field map array.
 * This function does not fill field map information.
 * It assumes the array is large enough to hold the required number
 * of distinct field elements.
 *
 * If the field element is already present in the list, memory
 * is allocated for the field map and appended to the link list
 * of that existing field.
 *
 * If the field element is new, the next available element in the
 * field map array list is used.
 *
 * As a result, the new field map is allocated and placed such that
 * maps with the same field ID and index are across the link list of
 * an array element.  This organization is expected by the FA node
 * using the corresponding metadata.
 *
 * \param [in] unit Unit number.
 * \param [in] fid Logical field ID.
 * \param [in] idx Logical field index.
 * \param [in,out] num_field_maps Number of field maps in array list.
 * \param [in,out] field_maps Field map array list to add new field map to.
 * \param [out] field_map_ptr Returns pointer to new field map.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_map_alloc(int unit,
                uint32_t fid,
                uint32_t idx,
                uint32_t *num_field_maps,
                bcmltm_field_map_t *field_maps,
                bcmltm_field_map_t **field_map_ptr)
{
    bcmltm_field_map_t *field_map = NULL;
    bcmltm_field_map_t *new_field_map = NULL;
    bool found = FALSE;
    uint32_t count;
    uint32_t map_idx = 0;
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    /* Check if field (id, idx) is already in the list */
    count = *num_field_maps;
    for (i = 0; i < count; i++) {
        field_map = &field_maps[i];
        if ((field_map->field_id == fid) && (field_map->field_idx == idx)) {
            map_idx = i;
            found = TRUE;
            break;
        }
    }

    /* If not found, return next element in field map array and adjust count */
    if (!found) {
        *field_map_ptr = &field_maps[count++];
        *num_field_maps = count;
        SHR_EXIT();
    }

    /* Field is not new, add map to link list of matched element in array */
    field_map = &field_maps[map_idx];

    /* Allocate */
    SHR_ALLOC(new_field_map, sizeof(*new_field_map), "bcmltmDmFieldMap");
    SHR_NULL_CHECK(new_field_map, SHR_E_MEMORY);
    sal_memset(new_field_map, 0, sizeof(*new_field_map));

    /* Insert it first in the link list (order does not matter) */
    new_field_map->next_map = field_map->next_map;
    field_map->next_map = new_field_map;

    *field_map_ptr = new_field_map;

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Add direct field map metadata for given map entry.
 *
 * This routine adds the metadata for the given direct field map entry
 * into the field map array.
 *
 * It assumes that the given map entry is the desired map entry type
 * to add to the field map array metadata.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] dm_arg DM arguments.
 * \param [in,out] num_field_maps Number of field maps in array list.
 * \param [in,out] field_maps Field map array list to add new field map to.
 * \param [in] map LRD map.
 * \param [in] map_entry Map entry identifier of map to add metadata for.
 * \param [in] field_type Indicates key or value field.
 * \param [in] field_dir Indicates input or output field direction,
 *                       (relevant only for key fields on Keyed LTs).
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_map_entry_add(int unit,
                    bcmlrd_sid_t sid,
                    const bcmltm_db_dm_arg_t *dm_arg,
                    uint32_t *num_field_maps,
                    bcmltm_field_map_t *field_maps,
                    const bcmlrd_map_t *map,
                    const bcmltm_db_map_entry_t *map_entry,
                    bcmltm_field_type_t field_type,
                    bcmltm_field_dir_t field_dir)
{
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    bcmltm_field_map_t *field_map;
    uint32_t fid;  /* Logical (source) field ID */
    bcmdrd_sid_t ptid;  /* PT ID */
    uint32_t pt_fid;    /* PT (destination) field ID */
    uint16_t pt_entry_idx;  /* PT entry index */
    uint32_t idx_count;
    uint32_t idx;
    uint16_t pt_minbit[MAX_FIELD_IDXS];
    uint16_t pt_maxbit[MAX_FIELD_IDXS];
    uint32_t num_pt_idx;
    const bcmltm_wb_block_t *ptm_wb_block = NULL;
    uint64_t *default_values = NULL;
    uint64_t *minimum_values = NULL;
    uint64_t *maximum_values = NULL;
    uint32_t apic_offset = BCMLTM_WB_OFFSET_INVALID;
    uint32_t pt_changed_offset = BCMLTM_WB_OFFSET_INVALID;
    uint32_t pt_field_offset = BCMLTM_WB_OFFSET_INVALID;
    uint16_t wbc_maxbit;
    uint16_t wbc_minbit;
    uint32_t field_flags = 0x0;

    SHR_FUNC_ENTER(unit);

    group = bcmltm_db_map_lrd_group_get(map, map_entry);
    entry = bcmltm_db_map_lrd_entry_get(map, map_entry);

    /* Logical (source) and PT (destination) IDs */
    fid = entry->u.mapped.src.field_id;
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

    /* Get destination field min and max bits per index */
    SHR_IF_ERR_EXIT
        (bcmltm_db_field_to_fidx_min_max(unit,
                                         pt_fid,
                                         entry->desc.minbit,
                                         entry->desc.maxbit,
                                         MAX_FIELD_IDXS,
                                         pt_minbit,
                                         pt_maxbit,
                                         &num_pt_idx));

    /*
     * An LRD direct map entry is expected only for a mapped field.
     * An unmapped field at this point is considered an error.
     */

    /* Get number of logical field indices */
    idx_count = bcmlrd_field_idx_count_get(unit, sid, fid);

    /* All fields has at least 1 index count for a mapped entry */
    if (idx_count < 1) {
        const char *table_name;
        const char *field_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        field_name = bcmltm_lt_field_fid_to_name(unit, sid, fid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Invalid field index count of zero "
                              "for mapped field: "
                              "%s(ltid=%d) %s(fid=%d)\n"),
                   table_name, sid, field_name, fid));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Get default, min, max value arrays */
    SHR_IF_ERR_EXIT
        (bcmltm_db_array_uint64_alloc(idx_count, &default_values));
    SHR_IF_ERR_EXIT
        (bcmltm_db_array_uint64_alloc(idx_count, &minimum_values));
    SHR_IF_ERR_EXIT
        (bcmltm_db_array_uint64_alloc(idx_count, &maximum_values));
    SHR_IF_ERR_EXIT
        (bcmltm_db_field_def_min_max_get(unit, sid, fid,
                                         idx_count,
                                         default_values,
                                         minimum_values,
                                         maximum_values));

    /* Get PT changed offset */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_ptm_pt_changed_offset_get(dm_arg->attr->type,
                                             field_type,
                                             ptm_wb_block,
                                             pt_entry_idx,
                                             &pt_changed_offset));

    /* Get destination field offset */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_ptm_field_lt_offset_get(ptm_wb_block,
                                           pt_fid,
                                           field_type,
                                           field_dir,
                                           pt_entry_idx,
                                           &pt_field_offset));

    /* Get field properties */
    SHR_IF_ERR_EXIT
        (bcmltm_db_field_properties_get(unit, sid, fid, &field_flags));

    /*
     * Add field map for each logical field element (id, idx).
     *
     * Indexes (idx) for a given field must be in order.
     */
    for (idx = 0; idx < idx_count; idx++) {
        /* Allocate new map object in list */
        SHR_IF_ERR_EXIT
            (field_map_alloc(unit, fid, idx,
                             num_field_maps, field_maps,
                             &field_map));

        /* Get API Cache field wb offset */
        SHR_IF_ERR_EXIT
            (bcmltm_wb_lt_apic_field_offset_get_by_sid(unit, sid,
                                                       fid, idx,
                                                       &apic_offset));

        if (idx < num_pt_idx) {
            wbc_minbit = pt_minbit[idx];
            wbc_maxbit = pt_maxbit[idx];
        } else {
            /*
             * Corresponding PTM min, max range doesn't exist.
             * To avoid PTM write, set BCMLTM_FIELD_BIT_POSITION_INVALID
             * for both minbit and maxbit.
             */
            wbc_minbit = BCMLTM_FIELD_BIT_POSITION_INVALID;
            wbc_maxbit = BCMLTM_FIELD_BIT_POSITION_INVALID;
        }

        /* Fill data */
        field_map->field_id = fid;
        field_map->field_idx = idx;
        field_map->apic_offset = apic_offset;
        field_map->wbc.buffer_offset = pt_field_offset;
        field_map->wbc.maxbit = wbc_maxbit;
        field_map->wbc.minbit = wbc_minbit;
        field_map->pt_changed_offset = pt_changed_offset;
        field_map->default_value = default_values[idx];
        field_map->minimum_value = minimum_values[idx];
        field_map->maximum_value = maximum_values[idx];
        field_map->flags = field_flags;
    }

 exit:
    SHR_FREE(maximum_values);
    SHR_FREE(minimum_values);
    SHR_FREE(default_values);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Resize field maps array.
 *
 * This utility routine resizes the given field maps array
 * to a smaller size, if necessary, in order to free unused memory.
 *
 * If a resize takes place, the previous map array is freed.
 *
 * \param [in] unit Unit number.
 * \param [in] max_maps Max field maps allocated in array.
 * \param [in] num_maps Actual number of field maps.
 * \param [in,out] field_maps_ptr Returning pointer to resized array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_maps_resize(int unit,
                  uint32_t max_maps,
                  uint32_t num_maps,
                  bcmltm_field_map_t **field_maps_ptr)
{
    bcmltm_field_map_t *field_maps = NULL;
    bcmltm_field_map_t *new_field_maps = NULL;
    unsigned int alloc_size;

    SHR_FUNC_ENTER(unit);

    field_maps = *field_maps_ptr;

    /* Sanity check */
    if (num_maps > max_maps) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Nothing to do if number of maps matches allocated max maps */
    if (num_maps == max_maps) {
        SHR_EXIT();
    }

    /* Free array if there are no maps */
    if (num_maps == 0) {
        SHR_FREE(field_maps);
        *field_maps_ptr = NULL;
        SHR_EXIT();
    }

    /*
     * Reallocate smaller array and copy array elements.
     *
     * There is no need to copy linked items since the references
     * are automatically transferred during the element copy.
     */
    alloc_size = sizeof(*new_field_maps) * num_maps;
    SHR_ALLOC(new_field_maps, alloc_size, "bcmltmDmFieldMaps");
    SHR_NULL_CHECK(new_field_maps, SHR_E_MEMORY);
    sal_memcpy(new_field_maps, field_maps, alloc_size);

    /* Free old array and return new array */
    SHR_FREE(field_maps);
    *field_maps_ptr = new_field_maps;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the field map array metadata for given map entry list.
 *
 * This routine creates the direct field map array metadata for
 * the given map entry list.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] dm_arg DM arguments.
 * \param [in] map_entry_list Map entry list to process.
 * \param [in] fspec Specifies type of fields to create metadata for.
 * \param [out] field_maps_ptr Returning pointer to field map array.
 * \param [out] num_field_maps Number of field maps returned in array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_maps_create(int unit,
                  bcmlrd_sid_t sid,
                  const bcmltm_db_dm_arg_t *dm_arg,
                  const bcmltm_db_map_entry_list_t *map_entry_list,
                  const bcmltm_db_dm_field_map_spec_t *fspec,
                  bcmltm_field_map_t **field_maps_ptr,
                  uint32_t *num_field_maps)
{
    bcmltm_field_map_t *field_maps = NULL;
    const bcmlrd_map_t *lrd_map = map_entry_list->map;
    const bcmltm_db_map_entry_t *map_entry;
    bcmltm_field_type_t field_type = fspec->field_type;
    bcmltm_field_dir_t field_dir = fspec->field_dir;
    bcmltm_field_key_type_t key_type = fspec->key_type;
    uint32_t max_fields;
    uint32_t max_field_elems;
    unsigned int alloc_size;
    uint32_t field_map_count = 0;
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    *num_field_maps = 0;
    *field_maps_ptr = NULL;

    /* Get max count of field IDs and field elements (fid,idx) */
    SHR_IF_ERR_EXIT
        (bcmltm_db_field_count_get(unit, sid,
                                   &max_fields, &max_field_elems));
    if (max_field_elems == 0) {
        SHR_EXIT();
    }

    /* Allocate field maps array */
    alloc_size = sizeof(*field_maps) * max_field_elems;
    SHR_ALLOC(field_maps, alloc_size, "bcmltmDmFieldMaps");
    SHR_NULL_CHECK(field_maps, SHR_E_MEMORY);
    sal_memset(field_maps, 0, alloc_size);

    /* Parse each map entry in list */
    for (i = 0; i < map_entry_list->num_entries; i++) {
        map_entry = &map_entry_list->entries[i];

        /* Check if map entry qualifies, if not skip */
        if (!field_map_entry_process(unit, sid, dm_arg->attr,
                                     lrd_map, map_entry,
                                     field_type, key_type)) {
            continue;
        }

        SHR_IF_ERR_EXIT
            (field_map_entry_add(unit, sid, dm_arg,
                                 &field_map_count, field_maps,
                                 lrd_map, map_entry,
                                 field_type, field_dir));
    }

    /* Resize field map array */
    SHR_IF_ERR_EXIT
        (field_maps_resize(unit, max_field_elems, field_map_count,
                           &field_maps));

    *num_field_maps = field_map_count;
    *field_maps_ptr = field_maps;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_field_maps_destroy(max_field_elems, field_maps);
        *num_field_maps = 0;
        *field_maps_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the field map list metadata.
 *
 * This routine creates the direct field map list metadata for
 * the given map entry list.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] dm_arg DM arguments.
 * \param [in] map_entry_list Map entry list to process.
 * \param [in] fspec Specifies type of fields to create metadata for.
 * \param [out] field_list_ptr Returning pointer to field map list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_map_list_create(int unit,
                      bcmlrd_sid_t sid,
                      const bcmltm_db_dm_arg_t *dm_arg,
                      const bcmltm_db_map_entry_list_t *map_entry_list,
                      const bcmltm_db_dm_field_map_spec_t *fspec,
                      bcmltm_field_map_list_t **field_list_ptr)
{
    bcmltm_field_map_list_t *field_list = NULL;
    const bcmltm_table_attr_t *attr = dm_arg->attr;
    bcmltm_field_type_t field_type = fspec->field_type;
    bcmltm_field_key_type_t key_type = fspec->key_type;
    uint32_t num_fields;
    uint32_t index_absent_offset = BCMLTM_WB_OFFSET_INVALID;

    SHR_FUNC_ENTER(unit);

    /*
     * This metadata object should always be created even if
     * the field count is zero.  This is expected by the corresponding
     * FA node function logic.
     */

    /* Allocate */
    SHR_ALLOC(field_list, sizeof(*field_list), "bcmltmDmFieldMapList");
    SHR_NULL_CHECK(field_list, SHR_E_MEMORY);
    sal_memset(field_list, 0, sizeof(*field_list));

    /*
     * Get index absent offset.
     * Valid only for Index with Allocation LT and
     * on allocatable key fields.
     */
    if (BCMLTM_TABLE_TYPE_IS_ALLOC_INDEX(attr->type) &&
        (field_type == BCMLTM_FIELD_TYPE_KEY) &&
        (key_type == BCMLTM_FIELD_KEY_TYPE_IWA_ALLOC)) {
        index_absent_offset = dm_arg->lt_pvt_wb_offsets->index_absent;
    }

    /* Create field maps array and fill data */
    SHR_IF_ERR_EXIT
        (field_maps_create(unit, sid, dm_arg,
                           map_entry_list, fspec,
                           &field_list->field_maps,
                           &num_fields));

    field_list->num_fields = num_fields;
    field_list->index_absent_offset = index_absent_offset;

    *field_list_ptr = field_list;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_field_map_list_destroy(field_list);
        *field_list_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_db_dm_field_mapping_create(int unit,
                                  bcmlrd_sid_t sid,
                                  const bcmltm_db_dm_arg_t *dm_arg,
                                  const bcmltm_db_field_selection_t *selection,
                                  const bcmltm_db_dm_field_map_spec_t *fspec,
                            bcmltm_field_select_mapping_t **field_mapping_ptr)
{
    bcmltm_field_select_mapping_t *field_mapping = NULL;
    const bcmltm_db_field_selection_map_t *selection_map = NULL;
    const bcmltm_db_map_entry_list_t *map_entry_list;
    bcmltm_field_type_t field_type = fspec->field_type;
    uint32_t num_maps;
    uint32_t alloc_size;
    uint32_t pt_suppress_offset = BCMLTM_WB_OFFSET_INVALID;
    uint32_t map_index_offset = BCMLTM_WB_OFFSET_INVALID;
    uint32_t map_index;
    uint32_t fm_idx;
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    *field_mapping_ptr = NULL;

    if (selection == NULL) {
        SHR_EXIT();
    }

    num_maps = selection->num_maps;

    /* Allocate */
    alloc_size = sizeof(*field_mapping) +
        sizeof(bcmltm_field_map_list_t *) * num_maps;
    SHR_ALLOC(field_mapping, alloc_size, "bcmltmDmFieldSelectMapping");
    SHR_NULL_CHECK(field_mapping, SHR_E_MEMORY);
    sal_memset(field_mapping, 0, alloc_size);

    /*
     * Get PT suppress status offset.
     * Valid only when PT suppress is provided by map and
     * on values fields.
     */
    if (dm_arg->pt_suppress &&
        (field_type == BCMLTM_FIELD_TYPE_VALUE)) {
        SHR_IF_ERR_EXIT
            (bcmltm_wb_lt_pt_suppress_offset_get_by_sid(unit, sid,
                                                        &pt_suppress_offset));
    }

    /* Get selection map index offset */
    if (selection->selection_id != BCMLTM_FIELD_SELECTION_ID_UNCOND) {
        SHR_IF_ERR_EXIT
            (bcmltm_wb_lt_fs_map_index_offset_get_by_sid(unit, sid,
                                              selection->selection_id,
                                              &map_index_offset));
    }

    /* Fill data */
    field_mapping->pt_suppress_offset = pt_suppress_offset;
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
            (field_map_list_create(unit, sid, dm_arg,
                                   map_entry_list, fspec,
                                   &(field_mapping->field_map_list[fm_idx])));
    }

    *field_mapping_ptr = field_mapping;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_field_mapping_destroy(field_mapping);
        *field_mapping_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_field_map_info_create(int unit,
                                   bcmlrd_sid_t sid,
                                   const bcmltm_db_dm_arg_t *dm_arg,
                                   const bcmltm_db_field_selection_t *selection,
                                   bcmltm_db_dm_field_map_info_t **info_ptr)
{
    bcmltm_db_dm_field_map_info_t *info = NULL;
    bcmltm_table_type_t table_type;
    bcmltm_db_dm_field_map_spec_t fspec;

    SHR_FUNC_ENTER(unit);

    *info_ptr = NULL;

    /* Allocate info struct */
    SHR_ALLOC(info, sizeof(*info), "bcmltmDbDmFieldInfo");
    SHR_NULL_CHECK(info, SHR_E_MEMORY);
    sal_memset(info, 0, sizeof(*info));

    /* Get table type */
    table_type = dm_arg->attr->type;

    /* Create Input Key Field Mappings (FA node cookie) */
    bcmltm_db_dm_field_map_spec_set(&fspec,
                                    BCMLTM_FIELD_TYPE_KEY,
                                    BCMLTM_FIELD_DIR_IN,
                                    BCMLTM_FIELD_KEY_TYPE_ALL);
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_field_mapping_create(unit, sid, dm_arg,
                                           selection, &fspec,
                                           &info->in_keys));
    /*
     * Create Output Key Field Mappings (FA node cookie).
     * Input and output field mappings are different on Keyed LTs,
     * but they are the same on Index LTs.
     */
    if (BCMLTM_TABLE_TYPE_IS_KEYED(table_type)) {
        bcmltm_db_dm_field_map_spec_set(&fspec,
                                        BCMLTM_FIELD_TYPE_KEY,
                                        BCMLTM_FIELD_DIR_OUT,
                                        BCMLTM_FIELD_KEY_TYPE_ALL);
        SHR_IF_ERR_EXIT
            (bcmltm_db_dm_field_mapping_create(unit, sid, dm_arg,
                                               selection, &fspec,
                                               &info->out_keys));
    } else {
        info->out_keys = info->in_keys;
    }

    /* Create Required and Allocatable Key Field Mappings for IwA LTs */
    if (BCMLTM_TABLE_TYPE_IS_ALLOC_INDEX(table_type)) {
        bcmltm_db_dm_field_map_spec_set(&fspec,
                                        BCMLTM_FIELD_TYPE_KEY,
                                        BCMLTM_FIELD_DIR_IN,
                                        BCMLTM_FIELD_KEY_TYPE_IWA_REQ);
        SHR_IF_ERR_EXIT
            (bcmltm_db_dm_field_mapping_create(unit, sid, dm_arg,
                                               selection, &fspec,
                                               &info->req_keys));

        bcmltm_db_dm_field_map_spec_set(&fspec,
                                        BCMLTM_FIELD_TYPE_KEY,
                                        BCMLTM_FIELD_DIR_IN,
                                        BCMLTM_FIELD_KEY_TYPE_IWA_ALLOC);
        SHR_IF_ERR_EXIT
            (bcmltm_db_dm_field_mapping_create(unit, sid, dm_arg,
                                               selection, &fspec,
                                               &info->alloc_keys));
    }

    /*
     * Create Value Field Mappings (FA node cookie).
     * Input and output field mappings are the same for all LTs.
     */
    bcmltm_db_dm_field_map_spec_set(&fspec,
                                    BCMLTM_FIELD_TYPE_VALUE,
                                    BCMLTM_FIELD_DIR_IN,
                                    0); /* N/A */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_field_mapping_create(unit, sid, dm_arg,
                                           selection, &fspec,
                                           &info->values));

    *info_ptr = info;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_dm_field_map_info_destroy(info);
        *info_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

void
bcmltm_db_dm_field_map_info_destroy(bcmltm_db_dm_field_map_info_t *info)
{
    if (info == NULL) {
        return;
    }

    /* Destroy Field Mappings */
    bcmltm_db_field_mapping_destroy(info->values);
    bcmltm_db_field_mapping_destroy(info->alloc_keys);
    bcmltm_db_field_mapping_destroy(info->req_keys);
    if (info->out_keys != info->in_keys) {
        bcmltm_db_field_mapping_destroy(info->out_keys);
    }
    bcmltm_db_field_mapping_destroy(info->in_keys);

    SHR_FREE(info);

    return;
}
