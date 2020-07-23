/*! \file bcmltm_db_cth.c
 *
 * Logical Table Manager - Information for Custom Table Handlers.
 *
 * This file contains routines to construct information for
 * custom table handlers.
 *
 * This information is derived from the LRD Custom Table Handler map groups.
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

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm_types_internal.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_wb_lt_internal.h>
#include <bcmltm/bcmltm_wb_lta_internal.h>
#include <bcmltm/bcmltm_lta_intf_internal.h>
#include <bcmltm/bcmltm_db_core_internal.h>

#include "bcmltm_db_cth.h"
#include "bcmltm_db_md_util.h"

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/*!
 * \brief Field Map.
 *
 * Field map information.
 */
typedef struct cth_map_field_s {
    /*! Field ID. */
    uint32_t fid;

    /*! The index of the field in an array. */
    uint32_t idx;

    /*! The minimum value for this field. */
    uint64_t minimum_value;

    /*! The maximum value for this field. */
    uint64_t maximum_value;

    /*! The default value for this field. */
    uint64_t default_value;
} cth_map_field_t;

/*!
 * \brief LT map entries information for Custom Table Handler groups.
 *
 * This structure is used to hold information parsed from the LRD
 * map entries that are relevant for the Custom Table Handler groups.
 */
typedef struct cth_map_entries_s {
    /*! Custom Table Handler. */
    const bcmltd_table_handler_t *handler;

    /*! Number of total key field elements (fid, idx). */
    uint32_t num_key_fields;

    /*! Array of fields. */
    cth_map_field_t *key_field;

    /*! Number of total field elements (fid, idx). */
    uint32_t num_fields;

    /*! Array of fields. */
    cth_map_field_t *field;
} cth_map_entries_t;


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Cleanup LT Custom Table Handler map entries information.
 *
 * Free any internal memory allocated during the parsing of the
 * Custom Table Handler map entries.
 *
 * \param [in] lt_map LT mapping information to clean.
 */
static void
cth_map_entries_cleanup(cth_map_entries_t *lt_map)
{
    SHR_FREE(lt_map->key_field);
    SHR_FREE(lt_map->field);
    lt_map->key_field = NULL;
    lt_map->field = NULL;

    return;
}

/*!
 * \brief Get the Custom Table Handler mapping information for a given LT.
 *
 * Get the LT Custom Table Handler mapping information for a given table ID.
 *
 * This routine parses the LRD entry map information for:
 *     Map Group Kind: BCMLRD_MAP_CUSTOM
 *     Map Entry Type: BCMLRD_MAP_ENTRY_TABLE_HANDLER
 *
 * This support table definitions with only only 1 custom table handler
 * group and 1 map entry handler in the group per LT.
 *
 * This routine allocates memory so caller must call corresponding routine
 * to free memory when this is no longer required.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] lt_map Returning LT mapping information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cth_map_entries_parse(int unit, bcmlrd_sid_t sid,
                      cth_map_entries_t *lt_map)
{
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    uint32_t group_idx;
    uint32_t entry_idx;
    uint32_t num_fid;      /* Number of API facing fields (fid) */
    uint32_t num_fid_idx;  /* Total fields elements (fid, idx) */
    uint32_t fid_list_count;
    bcmltm_fid_idx_t *fid_list = NULL;
    uint32_t field_idx = 0 ;
    cth_map_field_t *field = NULL;     /* Total field list */
    uint32_t key_field_idx = 0 ;
    cth_map_field_t *key_field = NULL; /* Total Key field list */
    unsigned int size;
    uint32_t i;
    uint32_t fid;
    uint32_t idx;
    uint32_t idx_count;
    uint64_t *default_value = NULL;
    uint32_t default_count;
    uint64_t *minimum_value = NULL;
    uint32_t minimum_count;
    uint64_t *maximum_value = NULL;
    uint32_t maximum_count;
    bcmlrd_field_def_t field_def;

    SHR_FUNC_ENTER(unit);

    sal_memset(lt_map, 0, sizeof(*lt_map));

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));

    /* Get number of API facing LT fields and field elements(fid,idx) */
    SHR_IF_ERR_EXIT
        (bcmltm_db_field_count_get(unit, sid,
                                   &num_fid, &num_fid_idx));

    /* Get list of field IDs and index count */
    size = sizeof(*fid_list) * num_fid;
    if (size == 0) {
        /* No FIDs */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(fid_list, size, "bcmltmCthFidIdxArr");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);
    sal_memset(fid_list, 0, size);
    SHR_IF_ERR_EXIT
        (bcmltm_db_field_list_get(unit, sid,
                                  num_fid, fid_list, &fid_list_count));

    /* Allocate field map array */
    size = sizeof(*field) * num_fid_idx;
    if (size == 0) {
        /* No FIDs */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(field, size, "bcmltmCthMapFields");
    SHR_NULL_CHECK(field, SHR_E_MEMORY);
    sal_memset(field, 0, size);

    /* Allocate field map array for Key fields */
    size = sizeof(*key_field) * num_fid_idx;
    SHR_ALLOC(key_field, size, "bcmltmCthKeyMapFields");
    SHR_NULL_CHECK(key_field, SHR_E_MEMORY);
    sal_memset(key_field, 0, size);

    /* Get field map information */
    for (i = 0; i < fid_list_count; i++) {
        fid = fid_list[i].fid;
        idx_count = fid_list[i].idx_count;

        SHR_IF_ERR_EXIT
            (bcmlrd_table_field_def_get(unit, sid, fid, &field_def));

        /* Get SW default value for field */
        SHR_ALLOC(default_value, sizeof(*default_value) * idx_count,
                  "bcmltmCthFieldDefVals");
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
                  "bcmltmCthFieldMinVals");
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
                  "bcmltmCthFieldMaxVals");
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

        for (idx = 0; idx < idx_count; idx++) {
            if (i >= num_fid_idx) {
                const char *table_name;

                table_name = bcmltm_lt_table_sid_to_name(unit, sid);
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "LTM Custom Table Handler: current "
                                      "field count exceeds max count "
                                      "%s(ltid=%d) count=%d max_count=%d\n"),
                           table_name, sid, (i+1), num_fid_idx));
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            if (field_def.key) {
                key_field[key_field_idx].fid = fid;
                key_field[key_field_idx].idx = idx;
                key_field[key_field_idx].default_value =
                    default_value[idx];
                key_field[key_field_idx].minimum_value =
                    minimum_value[idx];
                key_field[key_field_idx].maximum_value =
                    maximum_value[idx];
                key_field_idx++;
            }
            field[field_idx].fid = fid;
            field[field_idx].idx = idx;
            field[field_idx].default_value = default_value[idx];
            field[field_idx].minimum_value = minimum_value[idx];
            field[field_idx].maximum_value = maximum_value[idx];
            field_idx++;
        }

        SHR_FREE(maximum_value);
        SHR_FREE(minimum_value);
        SHR_FREE(default_value);
    }

    lt_map->key_field      = key_field;
    lt_map->num_key_fields = key_field_idx;
    lt_map->field          = field;
    lt_map->num_fields     = field_idx;

    /* For each map group */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];

        /* Process Custom Table Handler group kind */
        if (group->dest.kind != BCMLRD_MAP_CUSTOM) {
            continue;
        }

        /* Process map entries until table handler entry is found */
        for (entry_idx = 0;
             (entry_idx < group->entries) && (lt_map->handler == NULL);
             entry_idx++) {
            entry = &group->entry[entry_idx];

            switch(entry->entry_type) {
            case BCMLRD_MAP_ENTRY_TABLE_HANDLER:
                lt_map->handler =
                    bcmltm_lta_intf_table_handler_get(unit,
                                                      entry->u.handler_id);
                break;

            default:
                break;
            }
        }

        break;  /* Support 1 custom table handler entry per LT */
    }

 exit:
    SHR_FREE(maximum_value);
    SHR_FREE(minimum_value);
    SHR_FREE(default_value);
    SHR_FREE(fid_list);

    if (SHR_FUNC_ERR()) {
        SHR_FREE(key_field);
        lt_map->key_field = NULL;

        SHR_FREE(field);
        lt_map->field = NULL;

        cth_map_entries_cleanup(lt_map);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the CTH interface type.
 *
 * This routine gets the custom table handler interface type.
 * This information indicates LTM which flow to apply for the specified
 * custom table handler.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] intf Custom table handler interface type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cth_handler_intf_type_get(int unit,
                          bcmlrd_sid_t sid,
                          bcmltm_cth_handler_intf_type_t *intf)
{
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_table_attr_t *tbl_attr = NULL;
    uint32_t num_attrs;
    uint32_t attr_idx;

    SHR_FUNC_ENTER(unit);

    /* Set default */
    *intf = BCMLTM_CTH_HANDLER_INTF_TYPE_STANDARD;

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));

    num_attrs = map->table_attr->attributes;
    for (attr_idx = 0; attr_idx < num_attrs; attr_idx++) {
        tbl_attr = &map->table_attr->attr[attr_idx];

        if (tbl_attr->key != BCMLRD_MAP_TABLE_ATTRIBUTE_HANDLER_INTF) {
            continue;
        }

        if (tbl_attr->value ==
            BCMLRD_MAP_TABLE_ATTRIBUTE_HANDLER_INTF_STANDARD) {
            *intf = BCMLTM_CTH_HANDLER_INTF_TYPE_STANDARD;
        } else if (tbl_attr->value ==
                   BCMLRD_MAP_TABLE_ATTRIBUTE_HANDLER_INTF_EXPEDITED) {
            *intf = BCMLTM_CTH_HANDLER_INTF_TYPE_EXPEDITED;
        } else {
            /* Unknown flow type for LTM */
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        break;
    }

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the given LTA CTH table params metadata.
 *
 * Destroy the given LTA CTH table params metadata.
 *
 * \param [in] params LTA table params to destroy.
 */
static void
table_params_destroy(bcmltm_lta_table_params_t *params)
{
    if (params == NULL) {
        return;
    }

    SHR_FREE(params);

    return;
}

/*!
 * \brief Create the LTA CTH table params metadata for given table.
 *
 * Create the LTA Custom Table Handler table params metadata for given table.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_map Custom table handler mapping information.
 * \param [in] wb_block Working buffer block.
 * \param [in] info Custom table handler info database structure.
 * \param [out] params_ptr Returning pointer table params metadata.
  *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
table_params_create(int unit,
                    const cth_map_entries_t *lt_map,
                    const bcmltm_wb_block_t *wb_block,
                    bcmltm_db_cth_info_t *info,
                    bcmltm_lta_table_params_t **params_ptr)
{
    bcmltm_lta_table_params_t *params = NULL;
    bcmltm_wb_lta_fields_offsets_t in_offsets;
    bcmltm_wb_lta_fields_offsets_t out_offsets;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(params, sizeof(*params), "bcmltmCthLtaTableParams");
    SHR_NULL_CHECK(params, SHR_E_MEMORY);
    sal_memset(params, 0, sizeof(*params));

    /* Get offsets */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lta_fields_offsets_get(wb_block,
                                          BCMLTM_WB_LTA_INPUT_FIELDS,
                                          &in_offsets));
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lta_fields_offsets_get(wb_block,
                                          BCMLTM_WB_LTA_OUTPUT_FIELDS,
                                          &out_offsets));

    /* Fill data */
    params->handler = lt_map->handler;
    params->fields_input_offset = in_offsets.param;
    params->fields_output_offset = out_offsets.param;
    if (info->intf_type == BCMLTM_CTH_HANDLER_INTF_TYPE_EXPEDITED) {
        params->lta_field_list = info->fin;
        params->lta_key_list = info->fin_keys;
    }

    *params_ptr = params;

 exit:
    if (SHR_FUNC_ERR()) {
        table_params_destroy(params);
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
 * \brief Create the list of field maps metadata.
 *
 * Create the list of field maps metadata for either the
 * LTA input field list or the LTA output field list.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] lt_map Custom table handler mapping information.
 * \param [in] wb_block Working buffer block.
 * \param [in] field_type Indicate input or output field list.
 * \param [in] is_key Indicates field list should include key fields only.
 * \param [out] field_maps_ptr Returning pointer to list of field maps.
 * \param [out] num_fields Number of field maps returned in list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_maps_create(int unit,
                  bcmlrd_sid_t sid,
                  const cth_map_entries_t *lt_map,
                  const bcmltm_wb_block_t *wb_block,
                  int field_type,
                  int is_key,
                  bcmltm_lta_field_map_t **field_maps_ptr,
                  uint32_t *num_fields)
{
    bcmltm_lta_field_map_t *field_maps = NULL;
    cth_map_field_t *field = NULL;
    unsigned int size;
    uint32_t i;
    uint32_t field_max_count;
    uint32_t field_flags;
    uint32_t offset;

    SHR_FUNC_ENTER(unit);

    *num_fields = 0;
    *field_maps_ptr = NULL;

    /* Get total number of fields */
    if (is_key) {
        field_max_count = lt_map->num_key_fields;
        field = lt_map->key_field;
    } else {
        field_max_count = lt_map->num_fields;
        field = lt_map->field;
    }
    if (field_max_count == 0) {
        SHR_EXIT();
    }

    /* Allocate field_map */
    size = sizeof(*field_maps) * field_max_count;
    SHR_ALLOC(field_maps, size, "bcmltmCthLtaFieldMaps");
    SHR_NULL_CHECK(field_maps, SHR_E_MEMORY);
    sal_memset(field_maps, 0, size);

    /* Fill data */
    for (i = 0; i < field_max_count ; i++) {
        /* Get field properties */
        SHR_IF_ERR_EXIT
            (bcmltm_db_field_properties_get(unit, sid, field[i].fid,
                                            &field_flags));

        /* Get field offset */
        SHR_IF_ERR_EXIT
            (bcmltm_wb_lta_field_map_offset_get(wb_block, field_type,
                                                i, &offset));

        field_maps[i].api_field_id = field[i].fid;
        field_maps[i].field_idx = field[i].idx;
        field_maps[i].default_value = field[i].default_value;
        field_maps[i].minimum_value = field[i].minimum_value;
        field_maps[i].maximum_value = field[i].maximum_value;
        field_maps[i].flags = field_flags;
        field_maps[i].field_offset = offset;

        /*
         * The following fields are not used by the FA node cookie
         * for LTA Custom Table Handler:
         *   field_maps[i].wbc;
         */
    }

    *num_fields = field_max_count;
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
 * \brief Destroy the given LTA CTH fields list metadata.
 *
 * Destroy the given LTA CTH field list metadata.
 *
 * \param [in] flist LTA field list to destroy.
 */
static void
field_list_destroy(bcmltm_lta_field_list_t *flist)
{
    if (flist == NULL) {
        return;
    }

    field_maps_destroy(flist->field_maps);

    SHR_FREE(flist);

    return;
}


/*!
 * \brief Create the LTA CTH field list metadata for given table.
 *
 * Create the LTA Custom Table Handler field list metadata for given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] lt_map Custom table handler mapping information.
 * \param [in] wb_block Working buffer block.
 * \param [in] field_type Indicates input or output field list.
 * \param [in] is_key Indicates field list should include key fields only.
 * \param [out] flist Returning pointer for field list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_list_create(int unit,
                  bcmlrd_sid_t sid,
                  const cth_map_entries_t *lt_map,
                  const bcmltm_wb_block_t *wb_block,
                  int field_type,
                  int is_key,
                  bcmltm_lta_field_list_t **flist_ptr)
{
    bcmltm_lta_field_list_t *flist = NULL;
    bcmltm_wb_lta_fields_offsets_t offsets;
    uint32_t num_fields;
    uint32_t api_fields_offset = BCMLTM_WB_OFFSET_INVALID;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(flist, sizeof(*flist), "bcmltmCthLtaFieldList");
    SHR_NULL_CHECK(flist, SHR_E_MEMORY);
    sal_memset(flist, 0, sizeof(*flist));

    /* Get fields offset */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lta_fields_offsets_get(wb_block, field_type, &offsets));

    /* API input list is always the LTA input for CTH */
    if (field_type == BCMLTM_WB_LTA_OUTPUT_FIELDS) {
        bcmltm_wb_lta_fields_offsets_t in_offsets;

        SHR_IF_ERR_EXIT
            (bcmltm_wb_lta_fields_offsets_get(wb_block,
                                              BCMLTM_WB_LTA_INPUT_FIELDS,
                                              &in_offsets));
        api_fields_offset = in_offsets.param;
    } else {
        api_fields_offset = offsets.param;
    }

    /* Fill data */
    flist->fields_parameter_offset = offsets.param;
    flist->fields_ptrs_offset = offsets.ptrs;
    flist->api_fields_parameter_offset = api_fields_offset;

    /* Get field maps */
    SHR_IF_ERR_EXIT
        (field_maps_create(unit, sid, lt_map, wb_block, field_type, is_key,
                           &flist->field_maps,
                           &num_fields));

    /* All LTA values are in a single map list for CTH */
    flist->num_maps = num_fields;
    flist->max_fields = num_fields;

    /* Not applicable */
    flist->target_fields_parameter_offset = BCMLTM_WB_OFFSET_INVALID;
    flist->index_absent_offset = BCMLTM_WB_OFFSET_INVALID;

    *flist_ptr = flist;

 exit:
    if (SHR_FUNC_ERR()) {
        field_list_destroy(flist);
        *flist_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_db_cth_info_create(int unit, bcmlrd_sid_t sid,
                          bcmltm_db_cth_info_t **info_ptr)
{
    bcmltm_db_cth_info_t *info = NULL;
    cth_map_entries_t map_entries;
    bcmltm_wb_block_id_t wb_block_id;
    bcmltm_wb_block_t *wb_block = NULL;
    bcmltm_wb_handle_t *wb_handle = NULL;

    SHR_FUNC_ENTER(unit);

    /* Obtain LT map entries information */
    SHR_IF_ERR_EXIT
        (cth_map_entries_parse(unit, sid, &map_entries));

    /* Allocate */
    SHR_ALLOC(info, sizeof(*info), "bcmltmDbCthInfo");
    SHR_NULL_CHECK(info, SHR_E_MEMORY);
    sal_memset(info, 0, sizeof(*info));

    /* Add Working Buffer block */
    wb_handle = bcmltm_wb_lt_handle_get(unit, sid);
    SHR_IF_ERR_EXIT
        (bcmltm_wb_block_lta_cth_add(unit, map_entries.num_fields,
                                     wb_handle, &wb_block_id, &wb_block));
    info->wb_block_id = wb_block_id;

    /* Custom Table Handler interface */
    info->handler = map_entries.handler;

    /* Get handler interface type */
    SHR_IF_ERR_EXIT
        (cth_handler_intf_type_get(unit, sid, &info->intf_type));

    /*
     * Create Custom Table Handler Field List Input/Output
     * (used as LTA FA node cookies)
     */
    SHR_IF_ERR_EXIT
        (field_list_create(unit, sid, &map_entries, wb_block,
                           BCMLTM_WB_LTA_INPUT_FIELDS, 0 /* All fields */,
                           &info->fin));
    SHR_IF_ERR_EXIT
        (field_list_create(unit, sid, &map_entries, wb_block,
                           BCMLTM_WB_LTA_INPUT_FIELDS, 1 /* Key fields only */,
                           &info->fin_keys));
    SHR_IF_ERR_EXIT
        (field_list_create(unit, sid, &map_entries, wb_block,
                           BCMLTM_WB_LTA_OUTPUT_FIELDS, 0 /* All fields */,
                           &info->fout));

    /* Create Custom Table Handler table params (used as LTA EE node cookie) */
    SHR_IF_ERR_EXIT
        (table_params_create(unit, &map_entries, wb_block,
                             info, &info->table_params));

    *info_ptr = info;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_cth_info_destroy(info);
        *info_ptr = NULL;
    }

    cth_map_entries_cleanup(&map_entries);

    SHR_FUNC_EXIT();
}

void
bcmltm_db_cth_info_destroy(bcmltm_db_cth_info_t *info)
{
    if (info == NULL) {
        return;
    }

    /* Destroy CTH Field lists */
    field_list_destroy(info->fout);
    field_list_destroy(info->fin_keys);
    field_list_destroy(info->fin);

    /* Destroy CTH table params */
    table_params_destroy(info->table_params);

    SHR_FREE(info);

    return;
}
