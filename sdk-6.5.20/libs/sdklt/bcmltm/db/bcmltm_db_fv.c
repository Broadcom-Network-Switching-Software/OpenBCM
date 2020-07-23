/*! \file bcmltm_db_fv.c
 *
 * Logical Table Manager - Information for Field Validations.
 *
 * This file contains routines to construct information for
 * field validations in logical tables.
 *
 * The metadata created is used for the following LTM driver flow:
 * (non field select opcode driver version)
 *     API <-> [VALIDATE, XFRM] <-> PTM
 *
 * This driver flow can be applied only to tables without field selections.
 *
 * This information is derived from the LRD Field Validation map groups.
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
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_wb_lt_internal.h>
#include <bcmltm/bcmltm_wb_lta_internal.h>
#include <bcmltm/bcmltm_lta_intf_internal.h>

#include "bcmltm_db_md_util.h"
#include "bcmltm_db_fv.h"


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/*!
 * \brief Field Validation map list.
 *
 * Contains mapping entries of type:
 *     BCMLRD_MAP_ENTRY_KEY/VALUE_FIELD_VALIDATION.
 */
typedef struct fv_map_list_s {
    /*! Size of array. */
    uint32_t max;

    /*! Number of valid field validation in array. */
    uint32_t num;

    /*! Array of field validation handlers. */
    bcmltd_field_val_handler_t *fv_handler;

} fv_map_list_t;


/*!
 * \brief Map entries information for Field Validation groups.
 *
 * This structure is used to hold information parsed from the LRD
 * map entries that are relevant for the Field Validation Handler groups.
 */
typedef struct fv_map_entries_s {
    /*! Field Validation maps for Key fields. */
    fv_map_list_t validate_keys;

    /*! Field Validation maps for Value fields. */
    fv_map_list_t validate_values;

} fv_map_entries_t;


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Count number of Field Validation entries.
 *
 * Count number of Field Validation entries.
 *
 * \param [in] unit Unit number.
 * \param [in] map LRD map.
 * \param [out] key_fv_count Number of Key Field Validation entries.
 * \param [out] value_fv_count Number of Value Field Validation entries.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
fv_map_entries_count_get(int unit,
                         const bcmlrd_map_t *map,
                         uint32_t *key_fv_count,
                         uint32_t *value_fv_count)
{
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    uint32_t group_idx;
    uint32_t entry_idx;

    SHR_FUNC_ENTER(unit);

    *key_fv_count = *value_fv_count = 0;

    /* Process map group */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];

        /* Process Field Validation kind */
        if (group->dest.kind != BCMLRD_MAP_VALIDATION) {
            continue;
        }

        /* Process each map entry */
        for (entry_idx = 0; entry_idx < group->entries; entry_idx++) {
            entry = &group->entry[entry_idx];
            if (entry->entry_type ==
                BCMLRD_MAP_ENTRY_KEY_FIELD_VALIDATION) {
                (*key_fv_count)++;
            } else if (entry->entry_type ==
                       BCMLRD_MAP_ENTRY_VALUE_FIELD_VALIDATION) {
                (*value_fv_count)++;
            }
        }
    }

    SHR_EXIT();

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize Field Validation map list.
 *
 * This routine initializes the Field Validation map list.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] fv_count Number of Field Validation entries.
 * \param [out] fv_map_list LT mapping for Field Validation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
fv_map_list_init(int unit, bcmlrd_sid_t sid,
                 uint32_t fv_count,
                 fv_map_list_t *fv_map_list)
{
    uint32_t size;

    SHR_FUNC_ENTER(unit);

    if (fv_count == 0) {
        SHR_EXIT();
    }

    fv_map_list->max = fv_count;
    fv_map_list->num = 0;
    size = sizeof(bcmltd_field_val_handler_t) * fv_count;

    /* Allocate field map array */
    SHR_ALLOC(fv_map_list->fv_handler, size, "bcmltmFieldValHandlers");
    SHR_NULL_CHECK(fv_map_list->fv_handler, SHR_E_MEMORY);
    sal_memset(fv_map_list->fv_handler, 0, size);

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse LRD Field Validation map entry.
 *
 * This routine parses the LRD field entry of types:
 *     BCMLRD_MAP_ENTRY_KEY_FIELD_VALIDATION
 *     BCMLRD_MAP_ENTRY_VALUE_FIELD_VALIDATION
 *
 * It also gets any other information needed to
 * construct the metadata expected for the validation node cookies.
 *
 * \param [in] unit Unit number.
 * \param [in] entry LRD map entry to parse.
 * \param [out] lt_map LT map structure to fill.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
fv_map_entry_validation_parse(int unit,
                              const bcmlrd_map_entry_t *entry,
                              fv_map_entries_t *lt_map)
{
    fv_map_list_t *fv_map_list;
    const bcmltd_field_val_handler_t *fv_handler = NULL;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    /* Field Validation Handler */
    fv_handler = bcmltm_lta_intf_val_handler_get(unit, entry->u.handler_id);

    /* Sanity check */
    if ((fv_handler == NULL) ||
        (fv_handler->arg == NULL) ||
        (fv_handler->arg->fields <= 0)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid Field Validation handler\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (entry->entry_type == BCMLRD_MAP_ENTRY_KEY_FIELD_VALIDATION) {
        fv_map_list = &lt_map->validate_keys;
    } else if (entry->entry_type == BCMLRD_MAP_ENTRY_VALUE_FIELD_VALIDATION) {
        fv_map_list = &lt_map->validate_values;
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Unsupported entry type\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    idx = fv_map_list->num;
    if (idx >= fv_map_list->max) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Field Validation index is out of bound\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    sal_memcpy(&fv_map_list->fv_handler[idx], fv_handler, sizeof(*fv_handler));

    fv_map_list->num = ++idx;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup Field Validation map list.
 *
 * Cleanup given Field Validation map list.
 *
 * \param [out] list List to cleanup.
 */
static void
fv_map_list_cleanup(fv_map_list_t *list)
{
    if (list == NULL) {
        return;
    }

    if (list->fv_handler != NULL) {
        SHR_FREE(list->fv_handler);
    }

    return;
}

/*!
 * \brief Cleanup LT Field Validation map entries information.
 *
 * Free any internal memory allocated during the parsing of the
 * Field Validation map entries.
 *
 * \param [in] lt_map LT mapping information to clean.
 */
static void
fv_map_entries_cleanup(fv_map_entries_t *lt_map)
{
    fv_map_list_cleanup(&lt_map->validate_keys);
    fv_map_list_cleanup(&lt_map->validate_values);

    return;
}

/*!
 * \brief Get Field Validation mapping information for a given LT.
 *
 * Get the Field Validation mapping information for a given table ID.
 *
 * This routine parses the LRD entry map information for:
 *     Map Group Kind: BCMLRD_MAP_VALIDATION
 *     Map Entry Type: BCMLRD_MAP_ENTRY_KEY/VALUE_FIELD_VALIDATION
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
fv_map_entries_parse(int unit, bcmlrd_sid_t sid,
                     fv_map_entries_t *lt_map)
{
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    uint32_t group_idx;
    uint32_t entry_idx;
    uint32_t num_key_fv = 0;
    uint32_t num_value_fv = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(lt_map, 0, sizeof(*lt_map));

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));

    /* Count number of Field Validation entries */
    SHR_IF_ERR_EXIT
        (fv_map_entries_count_get(unit, map,
                                  &num_key_fv, &num_value_fv));

    /* Init Validate Keys field maps */
    SHR_IF_ERR_EXIT
        (fv_map_list_init(unit, sid, num_key_fv, &lt_map->validate_keys));

    /* Init Validate Values field maps */
    SHR_IF_ERR_EXIT
        (fv_map_list_init(unit, sid, num_value_fv, &lt_map->validate_values));

    /* For each map group */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];

        /* Process Field Validation kind */
        if (group->dest.kind != BCMLRD_MAP_VALIDATION) {
            continue;
        }

        /* Process each map entry */
        for (entry_idx = 0; entry_idx < group->entries; entry_idx++) {
            entry = &group->entry[entry_idx];

            switch(entry->entry_type) {
            case BCMLRD_MAP_ENTRY_KEY_FIELD_VALIDATION:
            case BCMLRD_MAP_ENTRY_VALUE_FIELD_VALIDATION:
                SHR_IF_ERR_EXIT
                    (fv_map_entry_validation_parse(unit, entry, lt_map));
                break;

            default:
                break;
            }
        }
    }

 exit:
    if (SHR_FUNC_ERR()) {
        fv_map_entries_cleanup(lt_map);
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the given LTA Field Validation params metadata.
 *
 * Destroy the given LTA Field Validation params metadata.
 *
 * \param [in] params LTA Field Validation params to destroy.
 */
static void
validate_params_destroy(bcmltm_lta_validate_params_t *params)
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
 * \brief Create the LTA Field Validation params metadata for given table.
 *
 * Create the LTA Field Validation params metadata for given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] fv_handler Field validation handler.
 * \param [in] wb_block Working buffer block for LTA field validation.
 * \param [out] params_ptr Returning pointer of Field params metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
validate_params_create(int unit, bcmlrd_sid_t sid,
                       bcmltd_field_val_handler_t *fv_handler,
                       const bcmltm_wb_block_t *wb_block,
                       bcmltm_lta_validate_params_t **params_ptr)
{
    bcmltm_lta_validate_params_t *params = NULL;
    uint32_t size;
    bcmltm_wb_lta_fields_offsets_t offsets;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(params, sizeof(bcmltm_lta_validate_params_t),
              "bcmltmLtaValidateParams");
    SHR_NULL_CHECK(params, SHR_E_MEMORY);
    sal_memset(params, 0, sizeof(*params));

    size = sizeof(bcmltd_field_val_arg_t);
    SHR_ALLOC(params->lta_args, size, "bcmltmLtaFieldValArgs");
    SHR_NULL_CHECK(params->lta_args, SHR_E_MEMORY);
    sal_memset(params->lta_args, 0, size);

    /* Get offsets */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lta_fields_offsets_get(wb_block,
                                          BCMLTM_WB_LTA_INPUT_FIELDS,
                                          &offsets));

    /* Fill data */
    params->lta_validate = fv_handler->validate;
    params->fields_input_offset = offsets.param;
    sal_memcpy(params->lta_args, fv_handler->arg, sizeof(*params->lta_args));

    *params_ptr = params;

 exit:
    if (SHR_FUNC_ERR()) {
        validate_params_destroy(params);
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
 * \brief Create the list of Field Validation source field maps metadata.
 *
 * Create the list of field maps metadata for the LTA Field Validation
 * SOURCE fields for given LT.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] total_indices Total field indices in field Validation.
 * \param [in] fv_handler Field validation handler.
 * \param [in] wb_block Working buffer block for LTA field validation.
 * \param [out] field_maps_ptr Returning pointer to list of field maps.
 * \param [out] num_fields Number of field maps (fid,idx) returned in list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_maps_create(int unit,
                  bcmlrd_sid_t sid,
                  uint32_t total_indices,
                  const bcmltd_field_val_handler_t *fv_handler,
                  const bcmltm_wb_block_t *wb_block,
                  bcmltm_lta_field_map_t **field_maps_ptr,
                  uint32_t *num_fields)
{
    bcmltm_lta_field_map_t *lta_field_maps = NULL;
    uint32_t lfid, idx, fidx, lidx, idx_count;
    unsigned int size;
    uint32_t offset;
    uint64_t *minimum_value = NULL;
    uint32_t minimum_count;
    uint64_t *maximum_value = NULL;
    uint32_t maximum_count;
    uint32_t field_flags;

    SHR_FUNC_ENTER(unit);

    *num_fields = 0;
    *field_maps_ptr = NULL;

    if (total_indices == 0) {
        SHR_EXIT();
    }

    /* Allocate field_map */
    size = sizeof(*lta_field_maps) * total_indices;
    SHR_ALLOC(lta_field_maps, size, "bcmltmFvLtaFieldMaps");
    SHR_NULL_CHECK(lta_field_maps, SHR_E_MEMORY);
    sal_memset(lta_field_maps, 0, size);

    for (lidx = 0, fidx = 0; fidx < fv_handler->arg->fields; fidx++) {
        lfid = fv_handler->arg->field[fidx];

        idx_count = bcmlrd_field_idx_count_get(unit, sid, lfid);
        if (idx_count == 0) {
            continue;    /* Skip unmapped fields */
        }

        /* Get SW minimum value for field */
        SHR_ALLOC(minimum_value, sizeof(*minimum_value) * idx_count,
                  "bcmltmFvFieldMinVals");
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
                  "bcmltmFvFieldMaxVals");
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

        /* Get field properties */
        SHR_IF_ERR_EXIT
            (bcmltm_db_field_properties_get(unit, sid, lfid, &field_flags));

        for (idx = 0; idx < idx_count; idx++) {
            /* Get field offset */
            SHR_IF_ERR_EXIT
                (bcmltm_wb_lta_field_map_offset_get(wb_block,
                                                    BCMLTM_WB_LTA_INPUT_FIELDS,
                                                    lidx, &offset));
            lta_field_maps[lidx].api_field_id = lfid;
            lta_field_maps[lidx].field_idx = idx;
            lta_field_maps[lidx].field_offset = offset;
            lta_field_maps[lidx].minimum_value = minimum_value[idx];
            lta_field_maps[lidx].maximum_value = maximum_value[idx];
            lta_field_maps[lidx].flags = field_flags;
            lidx++;
            (*num_fields)++;
        }

        SHR_FREE(maximum_value);
        SHR_FREE(minimum_value);
    }

    *field_maps_ptr = lta_field_maps;

exit:
    SHR_FREE(maximum_value);
    SHR_FREE(minimum_value);

    if (SHR_FUNC_ERR()) {
        field_maps_destroy(lta_field_maps);
        *num_fields = 0;
        *field_maps_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the given LTA Field list metadata.
 *
 * Destroy the given LTA Field list metadata.
 *
 * \param [in] flist LTA Field list to destroy.
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
 * \brief Create the LTA Field Validate list metadata for given table.
 *
 * Create the LTA Validate Field list metadata for given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] total_indices Total number of field elements.
 * \param [in] fv_handler Field validation handler.
 * \param [in] wb_block Working buffer block for LTA field validation.
 * \param [out] flist_ptr Returning pointer to list of field maps.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_list_create(int unit,
                  bcmlrd_sid_t sid,
                  uint32_t total_indices,
                  bcmltd_field_val_handler_t *fv_handler,
                  const bcmltm_wb_block_t *wb_block,
                  bcmltm_lta_field_list_t **flist_ptr)
{
    bcmltm_lta_field_list_t *flist = NULL;
    bcmltm_wb_lta_fields_offsets_t offsets;
    uint32_t num_fields;
    uint32_t api_fields_offset = BCMLTM_WB_OFFSET_INVALID;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(flist, sizeof(*flist), "bcmltmFvLtaFieldList");
    SHR_NULL_CHECK(flist, SHR_E_MEMORY);
    sal_memset(flist, 0, sizeof(*flist));

    /* Get fields offset */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lta_fields_offsets_get(wb_block,
                                          BCMLTM_WB_LTA_INPUT_FIELDS,
                                          &offsets));

    /* API input list is always the LTA input for a field validation */
    api_fields_offset = offsets.param;

    /* Fill data */
    flist->fields_parameter_offset = offsets.param;
    flist->fields_ptrs_offset = offsets.ptrs;
    flist->api_fields_parameter_offset = api_fields_offset;

    /* Get field maps */
    SHR_IF_ERR_EXIT
        (field_maps_create(unit, sid, total_indices,
                           fv_handler, wb_block,
                           &flist->field_maps, &num_fields));
    flist->max_fields = num_fields;
    flist->num_maps = num_fields;

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

/*!
 * \brief Destroy Field Validation information list.
 *
 * Destroy given Field Validation information list.
 *
 * \param [in] list Field Validation information list to destroy.
 */
static void
fv_list_destroy(bcmltm_db_fv_list_t *list)
{
    bcmltm_db_fv_t *info = NULL;
    uint32_t idx;

    if (list == NULL) {
        return;
    }

    if (list->fvs != NULL) {
        for (idx = 0; idx < list->num_fvs; ++idx) {
            info = &list->fvs[idx];

            /* Destroy Field Validation lists */
            field_list_destroy(info->fsrc);

            /* Destroy Field Validation params */
            validate_params_destroy(info->validate_params);
        }

        SHR_FREE(list->fvs);
    }

    SHR_FREE(list);
    list = NULL;

    return;
}

/*!
 * \brief Create Field Validation information list.
 *
 * Create the information for the Field Validation list on
 * given logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] lt_map LT map information.
 * \param [in] field_type Indicates key or value field.
 * \param [out] list_ptr Returning pointer to Validate Field info list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
fv_list_create(int unit, bcmlrd_sid_t sid,
               fv_map_entries_t *lt_map,
               bcmltm_field_type_t field_type,
               bcmltm_db_fv_list_t **list_ptr)
{
    bcmltm_db_fv_list_t *list = NULL;
    fv_map_list_t *map_list = NULL;
    bcmltd_field_val_handler_t *fv_handler = NULL;
    bcmltm_db_fv_t info;
    bcmltm_wb_block_id_t wb_block_id;
    bcmltm_wb_block_t *wb_block = NULL;
    bcmltm_wb_handle_t *wb_handle = NULL;
    uint32_t total_indices;
    uint32_t num_fvs = 0;
    uint32_t size = 0;
    uint32_t idx = 0;
    uint32_t fidx = 0;

    SHR_FUNC_ENTER(unit);

    if (field_type == BCMLTM_FIELD_TYPE_KEY) {
        map_list = &lt_map->validate_keys;
    } else {
        map_list = &lt_map->validate_values;
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

    num_fvs = map_list->num;

    /* Allocate */
    SHR_ALLOC(list , sizeof(*list), "bcmltmDbFvList");
    SHR_NULL_CHECK(list, SHR_E_MEMORY);
    sal_memset(list, 0, sizeof(*list));

    if (num_fvs == 0) {
        list->num_fvs = 0;
        *list_ptr = list;
        SHR_FUNC_EXIT();
    }

    size = sizeof(bcmltm_db_fv_t) * num_fvs;
    SHR_ALLOC(list->fvs , size, "bcmltmDbFvs");
    SHR_NULL_CHECK(list->fvs, SHR_E_MEMORY);
    sal_memset(list->fvs, 0, size);

    for (idx = 0; idx < num_fvs; idx++) {
        uint32_t lfid = 0;
        uint32_t count = 0;
        uint32_t num_fields = 0;

        sal_memset(&info, 0, sizeof(info));

        fv_handler = &map_list->fv_handler[idx];
        for (fidx = 0; fidx < fv_handler->arg->fields; fidx++) {
            lfid = fv_handler->arg->field[fidx];
            count = bcmlrd_field_idx_count_get(unit, sid, lfid);
            num_fields += count;
        }
        total_indices = num_fields;

        /* Add Working Buffer block */
        SHR_IF_ERR_EXIT
            (bcmltm_wb_block_lta_val_add(unit,
                                         total_indices,
                                         wb_handle, &wb_block_id, &wb_block));
        info.wb_block_id = wb_block_id;

        /* Create Field Validation params */
        SHR_IF_ERR_EXIT
            (validate_params_create(unit, sid, fv_handler, wb_block,
                                    &info.validate_params));

        /* Create Field Validation List Source */
        SHR_IF_ERR_EXIT
            (field_list_create(unit, sid, total_indices,
                               fv_handler, wb_block, &info.fsrc));

        sal_memcpy(&list->fvs[idx], &info, sizeof(info));
    }

    list->num_fvs = num_fvs;
    *list_ptr = list;

 exit:
    if (SHR_FUNC_ERR()) {
        fv_list_destroy(list);
    }

    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_db_fv_info_create(int unit, bcmlrd_sid_t sid,
                         bcmltm_db_fv_info_t **info_ptr)
{
    bcmltm_db_fv_info_t *info = NULL;
    fv_map_entries_t map_entries;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(info, sizeof(*info), "bcmltmDbFvInfo");
    SHR_NULL_CHECK(info, SHR_E_MEMORY);
    sal_memset(info, 0, sizeof(*info));

    /* Obtain LT map entries information */
    SHR_IF_ERR_EXIT
        (fv_map_entries_parse(unit, sid, &map_entries));

    /* Create Field Validation lists for Keys and Values */
    SHR_IF_ERR_EXIT
        (fv_list_create(unit, sid, &map_entries,
                        BCMLTM_FIELD_TYPE_KEY,
                        &info->validate_keys));
    SHR_IF_ERR_EXIT
        (fv_list_create(unit, sid, &map_entries,
                        BCMLTM_FIELD_TYPE_VALUE,
                        &info->validate_values));

    *info_ptr = info;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_fv_info_destroy(info);
        *info_ptr = NULL;
    }

    fv_map_entries_cleanup(&map_entries);

    SHR_FUNC_EXIT();
}

int
bcmltm_db_fv_info_copy_list_create(int unit, bcmlrd_sid_t sid,
                                   bcmltm_db_xfrm_list_t *from_list,
                                   bcmltm_db_fv_list_t *to_list,
                                   bcmltm_db_fv_list_t **list_ptr)
{
    bcmltm_db_fv_list_t *list = NULL;
    uint32_t to_num_fvs = 0;
    uint32_t from_num_xfrms = 0;
    uint32_t from_xfrm_idx;
    uint32_t to_validate_idx;
    uint32_t size = 0;
    bcmltm_db_fv_t info;

    SHR_FUNC_ENTER(unit);

    to_num_fvs = to_list->num_fvs;
    from_num_xfrms = from_list->num_xfrms;

    /* Allocate */
    SHR_ALLOC(list, sizeof(*list), "bcmltmDbFvList");
    SHR_NULL_CHECK(list, SHR_E_MEMORY);
    sal_memset(list, 0, sizeof(*list));
    if ((to_num_fvs * from_num_xfrms) == 0) {
        list->num_fvs = 0;
        *list_ptr = list;
        SHR_FUNC_EXIT();
    }

    size = sizeof(bcmltm_db_fv_t) * to_num_fvs * from_num_xfrms;
    SHR_ALLOC(list->fvs , size, "bcmltmDbFvs");
    SHR_NULL_CHECK(list->fvs, SHR_E_MEMORY);
    sal_memset(list->fvs, 0, size);

    sal_memset(&info, 0, sizeof(info));

    /* Allocate */
    for (to_validate_idx = 0;
         to_validate_idx < to_num_fvs;
         to_validate_idx++) {
        for (from_xfrm_idx = 0; from_xfrm_idx < from_num_xfrms;
             from_xfrm_idx++) {
            SHR_IF_ERR_EXIT
                (bcmltm_db_copy_lta_field_list_create(unit, sid,
                           from_list->xfrms[from_xfrm_idx].fout,
                           to_list->fvs[to_validate_idx].fsrc,
                           &(info.fsrc)));

            sal_memcpy(&(list->fvs[(to_validate_idx * from_num_xfrms) +
                                   from_xfrm_idx]),
                       &info, sizeof(info));
        }
    }

    list->num_fvs = to_num_fvs * from_num_xfrms;
    *list_ptr = list;

 exit:
    if (SHR_FUNC_ERR()) {
        fv_list_destroy(list);
    }

    SHR_FUNC_EXIT();
}

void
bcmltm_db_fv_info_destroy(bcmltm_db_fv_info_t *info)
{
    if (info == NULL) {
        return;
    }

    /* Destroy Validate Copy LTA Fields */
    fv_list_destroy(info->rvalue_to_value);

    /* Destroy Validate Fields for Keys and Values */
    fv_list_destroy(info->validate_values);
    fv_list_destroy(info->validate_keys);

    SHR_FREE(info);

    return;
}

