/*! \file bcmltm_db_validate.c
 *
 * Logical Table Manager - Information for Field Validations.
 *
 * This file contains interfaces to construct information for
 * field validations in logical tables.
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

#include <bcmltm/bcmltm_lta_intf_internal.h>
#include <bcmltm/bcmltm_types_internal.h>
#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm_db_core_internal.h>
#include <bcmltm/bcmltm_wb_lta_internal.h>
#include <bcmltm/bcmltm_wb_lt_internal.h>

#include "bcmltm_db_util.h"
#include "bcmltm_db_map.h"
#include "bcmltm_db_field_select.h"
#include "bcmltm_db_md_util.h"
#include "bcmltm_db_dm_util.h"
#include "bcmltm_db_validate.h"

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/*!
 * \brief Field Validation Arguments.
 *
 * This structure contains context information for
 * field validation and can be used to pass various data
 * and help reduce the number of input function arguments.
 */
typedef struct bcmltm_db_validate_arg_s {
    /*! Field validation type to process. */
    bcmltm_field_validate_type_t validate_type;

    /*! LRD map. */
    const bcmlrd_map_t *lrd_map;

    /*! Field select map index working buffer offset. */
    uint32_t select_map_index_offset;

    /*! Selector map index required to execute this validation. */
    uint32_t select_map_index;
} bcmltm_db_validate_arg_t;

/*!
 * \brief Field Validation Map Entry Arguments.
 *
 * This structure contains context information for a
 * field validation map entry and can be used to pass various data
 * and help reduce the number of input function arguments.
 */
typedef struct bcmltm_db_validate_me_arg_s {
    /*! Field validation handler. */
    const bcmltd_field_val_handler_t *lta_handler;

    /*! Field validation working buffer block. */
    const bcmltm_wb_block_t *lta_wb_block;

    /*! Number of field elements (fid,idx). */
    uint32_t num_field_elems;

    /*! Field validation arguments. */
    const bcmltm_db_validate_arg_t *val_arg;
} bcmltm_db_validate_me_arg_t;

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
 * \param [in] val_type Desired field validation type, key or value.
 *
 * \retval TRUE if map entry is to be processed based on criteria.
 * \retval FALSE if map entry is to be skipped, or failure.
 */
static bool
validate_map_entry_process(int unit,
                           bcmlrd_sid_t sid,
                           const bcmlrd_map_t *map,
                           const bcmltm_db_map_entry_t *map_entry,
                           bcmltm_field_validate_type_t val_type)
{
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;

    /* Skip non validation map groups */
    group = bcmltm_db_map_lrd_group_get(map, map_entry);
    if (group->dest.kind != BCMLRD_MAP_VALIDATION) {
        return FALSE;
    }

    /* Check validation type */
    entry = bcmltm_db_map_lrd_entry_get(map, map_entry);

    /* Key validation */
    if ((val_type == BCMLTM_FIELD_VALIDATE_TYPE_KEY) &&
        (entry->entry_type == BCMLRD_MAP_ENTRY_KEY_FIELD_VALIDATION)) {
        return TRUE;
    }

    /* Value validation */
    if ((val_type == BCMLTM_FIELD_VALIDATE_TYPE_VALUE) &&
        (entry->entry_type == BCMLRD_MAP_ENTRY_VALUE_FIELD_VALIDATION)) {
        return TRUE;
    }

    return FALSE;
}

/*!
 * \brief Get number of field id and idx count for a field validation.
 *
 * This core routine returns the count of logical fields IDs and
 * the total number of field elements (fid,idx) for the requested
 * field validation map entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] lta_arg Field validation handler.
 * \param [out] num_fields Number of field IDs (fid).
 * \param [out] num_field_elems Number of field elements (fid,idx).
 */
static void
validate_field_count_get(int unit,
                         bcmlrd_sid_t sid,
                         const bcmltd_field_val_arg_t *lta_arg,
                         uint32_t *num_fields,
                         uint32_t *num_field_elems)
{
    uint32_t fid;
    uint32_t idx_count;
    uint32_t i;

    *num_fields = lta_arg->fields;
    *num_field_elems = 0;

    for (i = 0; i < *num_fields; i++) {
        fid = lta_arg->field[i];
        idx_count = bcmlrd_field_idx_count_get(unit, sid, fid);
        *num_field_elems += idx_count;
    }

    return;
}

/*!
 * \brief Destroy given LTA field validation params metadata.
 *
 * This routine destroys the given LTA field validation params metadata.
 *
 * \param [in] params LTA field validation params metadata to destroy.
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
 * \brief Create LTA field validation parameters metadata.
 *
 * This routine creates the LTA field validation parameters metadata
 * for the given field validation map entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] entry Map entry.
 * \param [in] me_arg Map entry arguments.
 * \param [out] params_ptr Returning pointer to validation params metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
validate_params_create(int unit, bcmlrd_sid_t sid,
                       const bcmlrd_map_entry_t *entry,
                       const bcmltm_db_validate_me_arg_t *me_arg,
                       bcmltm_lta_validate_params_t **params_ptr)
{
    bcmltm_lta_validate_params_t *params = NULL;
    const bcmltd_field_val_handler_t *lta_handler = me_arg->lta_handler;
    uint32_t alloc_size;
    bcmltm_wb_lta_fields_offsets_t offsets;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(params, sizeof(*params), "bcmltmLtaValidateParams");
    SHR_NULL_CHECK(params, SHR_E_MEMORY);
    sal_memset(params, 0, sizeof(*params));

    alloc_size = sizeof(bcmltd_field_val_arg_t);
    SHR_ALLOC(params->lta_args, alloc_size, "bcmltmLtaFieldValArg");
    SHR_NULL_CHECK(params->lta_args, SHR_E_MEMORY);
    sal_memset(params->lta_args, 0, alloc_size);

    /* Get offsets */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lta_fields_offsets_get(me_arg->lta_wb_block,
                                          BCMLTM_WB_LTA_INPUT_FIELDS,
                                          &offsets));

    /* Fill data */
    params->lta_validate = lta_handler->validate;
    sal_memcpy(params->lta_args, lta_handler->arg, sizeof(*params->lta_args));
    params->fields_input_offset = offsets.param;
    params->select_map_index_offset = me_arg->val_arg->select_map_index_offset;
    params->select_map_index = me_arg->val_arg->select_map_index ;

    *params_ptr = params;

 exit:
    if (SHR_FUNC_ERR()) {
        validate_params_destroy(params);
        *params_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create validation field maps metadata.
 *
 * This routine creates the array of field maps metadata for the
 * given field validation map entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in,out] list LTA field list to add created field maps to.
 * \param [in] entry Map entry.
 * \param [in] me_arg Map entry arguments.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
validate_field_maps_create(int unit,
                           bcmlrd_sid_t sid,
                           bcmltm_lta_select_field_list_t *list,
                           const bcmlrd_map_entry_t *entry,
                           const bcmltm_db_validate_me_arg_t *me_arg)
{
    bcmltm_lta_select_field_map_t *field_maps = NULL;
    bcmltm_lta_select_field_map_t *field_map;
    const bcmltd_field_val_handler_t *lta_handler = me_arg->lta_handler;
    uint32_t max_field_elems;
    unsigned int alloc_size;
    uint32_t fid;
    uint32_t idx;
    uint32_t idx_count;
    uint32_t fm_idx;
    uint32_t lta_fidx;
    uint32_t apic_offset = BCMLTM_WB_OFFSET_INVALID;
    uint32_t lta_offset = BCMLTM_WB_OFFSET_INVALID;
    uint32_t field_flags = 0x0;

    SHR_FUNC_ENTER(unit);

    list->num_maps = 0;
    list->max_fields = 0;
    list->lta_select_field_maps = NULL;

    max_field_elems = me_arg->num_field_elems;

    if (max_field_elems == 0) {
        SHR_EXIT();
    }

    /* Allocate field map array */
    alloc_size = sizeof(*field_maps) * max_field_elems;
    SHR_ALLOC(field_maps, alloc_size, "bcmltmValidateLtaSelectFieldMaps");
    SHR_NULL_CHECK(field_maps, SHR_E_MEMORY);
    sal_memset(field_maps, 0, alloc_size);

    /* Process each field in validation field list */
    for (fm_idx = 0, lta_fidx = 0;
         lta_fidx < lta_handler->arg->fields; lta_fidx++) {
        fid = lta_handler->arg->field[lta_fidx];

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
                                      "LTM Validation Field Map: current "
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
                                                    BCMLTM_WB_LTA_INPUT_FIELDS,
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
        SHR_FREE(field_maps);
        list->num_maps = 0;
        list->max_fields = 0;
        list->lta_select_field_maps = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create LTA validation field list metadata.
 *
 * This routine creates the LTA field list metadata
 * for the given field validation map entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] entry Map entry.
 * \param [in] me_arg Map entry arguments.
 * \param [out] list_ptr Returning pointer to LTA field list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
validate_field_list_create(int unit,
                           bcmlrd_sid_t sid,
                           const bcmlrd_map_entry_t *entry,
                           const bcmltm_db_validate_me_arg_t *me_arg,
                           bcmltm_lta_select_field_list_t **list_ptr)
{
    bcmltm_lta_select_field_list_t *list = NULL;
    bcmltm_wb_lta_fields_offsets_t offsets;
    uint32_t api_fields_offset = BCMLTM_WB_OFFSET_INVALID;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(list, sizeof(*list), "bcmltmValidateLtaSelectFieldList");
    SHR_NULL_CHECK(list, SHR_E_MEMORY);
    sal_memset(list, 0, sizeof(*list));

    /* Get fields offset */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lta_fields_offsets_get(me_arg->lta_wb_block,
                                          BCMLTM_WB_LTA_INPUT_FIELDS,
                                          &offsets));

    /* API input list is always the LTA input for a field validation */
    api_fields_offset = offsets.param;

    /* Fill data */
    list->fields_parameter_offset = offsets.param;
    list->api_fields_parameter_offset = api_fields_offset;
    list->fields_ptrs_offset = offsets.ptrs;
    list->lta_suppress_offset = BCMLTM_WB_OFFSET_INVALID;
    list->select_map_index_offset = me_arg->val_arg->select_map_index_offset;
    list->select_map_index = me_arg->val_arg->select_map_index;

    /* Create field maps */
    SHR_IF_ERR_EXIT
        (validate_field_maps_create(unit, sid, list,
                                    entry, me_arg));

    *list_ptr = list;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_lta_select_field_list_destroy(list);
        *list_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Add field validation metadata for given map entry.
 *
 * This routine adds the metadata for the given field validation
 * map entry into the validation list.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in,out] list List to add new validation to.
 * \param [in] map_entry Map entry identifier of map to add metadata for.
 * \param [in] val_arg Validate arguments.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
validate_map_entry_add(int unit,
                       bcmlrd_sid_t sid,
                       bcmltm_db_validate_list_t *list,
                       const bcmltm_db_map_entry_t *map_entry,
                       const bcmltm_db_validate_arg_t *val_arg)
{
    const bcmlrd_map_t *lrd_map = val_arg->lrd_map;
    const bcmlrd_map_entry_t *entry = NULL;
    const bcmltd_field_val_handler_t *lta_handler = NULL;
    bcmltm_db_validate_t *validate;
    bcmltm_db_validate_me_arg_t me_arg;
    uint32_t num_fields = 0;
    uint32_t num_field_elems = 0;
    bcmltm_wb_block_id_t lta_wb_block_id;
    bcmltm_wb_block_t *lta_wb_block = NULL;
    bcmltm_wb_handle_t *wb_handle = NULL;

    SHR_FUNC_ENTER(unit);

    entry = bcmltm_db_map_lrd_entry_get(lrd_map, map_entry);

    /* Get handler information */
    lta_handler = bcmltm_lta_intf_val_handler_get(unit, entry->u.handler_id);

    /* Sanity check */
    if ((lta_handler == NULL) ||
        (lta_handler->arg == NULL) ||
        (lta_handler->arg->fields <= 0)) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "%s(ltid=%d): "
                              "Invalid Field Validation handler\n"),
                   table_name, sid));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Get count of fields(fid) and field elements(fid,idx)  */
    validate_field_count_get(unit, sid, lta_handler->arg,
                             &num_fields, &num_field_elems);

    /* Add LTA Working Buffer block */
    wb_handle = bcmltm_wb_lt_handle_get(unit, sid);
    if (wb_handle == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_IF_ERR_EXIT
        (bcmltm_wb_block_lta_val_add(unit, num_field_elems, wb_handle,
                                     &lta_wb_block_id, &lta_wb_block));

    /* Populate validation arguments */
    sal_memset(&me_arg, 0, sizeof(me_arg));
    me_arg.lta_handler = lta_handler;
    me_arg.lta_wb_block = lta_wb_block;
    me_arg.num_field_elems = num_field_elems;
    me_arg.val_arg = val_arg;

    /* Add new field validation to list */
    validate = &list->validations[list->num_validations];
    list->num_validations++;

    /* Set field validation information and metadata */
    validate->wb_block_id = lta_wb_block_id;

    /* Create field validation params */
    SHR_IF_ERR_EXIT
        (validate_params_create(unit, sid,
                                entry, &me_arg,
                                &validate->params));

    /* Create field validation input field list */
    SHR_IF_ERR_EXIT
        (validate_field_list_create(unit, sid,
                                    entry, &me_arg,
                                    &validate->in_fields));

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add field validation metadata for a selection map.
 *
 * This routine creates and adds the field validation metadata
 * for the map entries specified in the field selection map entry list.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in,out] list List to add validations to.
 * \param [in] sm Field selection map.
 * \param [in] select_map_index_offset Selector map index offset.
 * \param [in] val_type Specifies validation type to create metadata for.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
validate_selection_map_add(int unit,
                           bcmlrd_sid_t sid,
                           bcmltm_db_validate_list_t *list,
                           const bcmltm_db_field_selection_map_t *sm,
                           uint32_t select_map_index_offset,
                           bcmltm_field_validate_type_t val_type)
{
    const bcmlrd_map_t *lrd_map = sm->map_entry_list->map;
    const bcmltm_db_map_entry_list_t *map_entry_list = sm->map_entry_list;
    const bcmltm_db_map_entry_t *map_entry;
    bcmltm_db_validate_arg_t val_arg;
    uint32_t me_idx;

    SHR_FUNC_ENTER(unit);

    /* Populate validate arguments */
    sal_memset(&val_arg, 0, sizeof(val_arg));
    val_arg.validate_type = val_type;
    val_arg.lrd_map = lrd_map;
    val_arg.select_map_index_offset = select_map_index_offset;
    val_arg.select_map_index = sm->map_index;

    /* Process each entry in map entry list */
    for (me_idx = 0; me_idx < map_entry_list->num_entries; me_idx++) {
        map_entry = &map_entry_list->entries[me_idx];

        /* Check if map entry qualifies, if not skip */
        if (!validate_map_entry_process(unit, sid,
                                        lrd_map, map_entry, val_type)) {
            continue;
        }

        /* Add map entry to list */
        SHR_IF_ERR_EXIT
            (validate_map_entry_add(unit, sid,
                                    list, map_entry, &val_arg));
    }

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Resize validations array.
 *
 * This utility routine resizes the field validations array in the
 * given list to a smaller size, if necessary, in order to free unused memory.
 *
 * If a resize takes place, the previous array is freed.
 *
 * \param [in] unit Unit number.
 * \param [in] max_array Max validations allocated in array.
 * \param [in,out] list List of validations.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
validate_list_resize(int unit,
                     uint32_t max_array,
                     bcmltm_db_validate_list_t *list)
{
    bcmltm_db_validate_t *validations = NULL;
    bcmltm_db_validate_t *new_validations = NULL;
    unsigned int alloc_size;

    SHR_FUNC_ENTER(unit);

    validations = list->validations;

    /* Sanity check */
    if (list->num_validations > max_array) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Nothing to do if number of validations matches allocated max */
    if (list->num_validations == max_array) {
        SHR_EXIT();
    }

    /* Free array if there are no field validations */
    if (list->num_validations == 0) {
        SHR_FREE(validations);
        list->validations = NULL;
        SHR_EXIT();
    }

    /* Reallocate smaller array and copy array elements */
    alloc_size = sizeof(*validations) * list->num_validations;
    SHR_ALLOC(new_validations, alloc_size, "bcmltmDbValidates");
    SHR_NULL_CHECK(new_validations, SHR_E_MEMORY);
    sal_memcpy(new_validations, validations, alloc_size);

    /* Free old array and return new array */
    SHR_FREE(validations);
    list->validations = new_validations;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy field validation list.
 *
 * This routine destroys the given field validation list.
 *
 * \param [in] list Field validation list to destroy.
 */
static void
validate_list_destroy(bcmltm_db_validate_list_t *list)
{
    bcmltm_db_validate_t *validate = NULL;
    uint32_t idx;

    if (list == NULL) {
        return;
    }

    if (list->validations != NULL) {
        for (idx = 0; idx < list->num_validations; idx++) {
            validate = &list->validations[idx];

            /* Destroy input field list */
            bcmltm_db_lta_select_field_list_destroy(validate->in_fields);

            /* Destroy validation params */
            validate_params_destroy(validate->params);
        }

        SHR_FREE(list->validations);
    }

    SHR_FREE(list);

    return;
}

/*!
 * \brief Create field validation list for a selection group.
 *
 * This routine creates the list of field validations
 * for the given field selection group.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] selection Field selection group.
 * \param [in] val_type Specifies validation type to create metadata for.
 * \param [out] list_ptr Returning pointer to field validation info list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
validate_list_create(int unit,
                     bcmlrd_sid_t sid,
                     const bcmltm_db_field_selection_t *selection,
                     bcmltm_field_validate_type_t val_type,
                     bcmltm_db_validate_list_t **list_ptr)
{
    bcmltm_db_validate_list_t *list = NULL;
    const bcmltm_db_field_selection_map_t *selection_map = NULL;
    uint32_t select_map_index_offset = BCMLTM_WB_OFFSET_INVALID;
    uint32_t max_entries;
    unsigned int alloc_size;
    uint32_t sm_idx;

    SHR_FUNC_ENTER(unit);

    *list_ptr = NULL;

    /* Use max map entries count for max validations in table */
    SHR_IF_ERR_EXIT
        (bcmltm_db_map_entries_count_get(unit, sid,
                                         &max_entries));
    if (max_entries == 0) {
        SHR_EXIT();
    }

    /* Allocate list */
    SHR_ALLOC(list , sizeof(*list), "bcmltmDbValidateList");
    SHR_NULL_CHECK(list, SHR_E_MEMORY);
    sal_memset(list, 0, sizeof(*list));

    /* Allocate field validation array */
    alloc_size = sizeof(bcmltm_db_validate_t) * max_entries;
    SHR_ALLOC(list->validations , alloc_size, "bcmltmDbValidates");
    SHR_NULL_CHECK(list->validations, SHR_E_MEMORY);
    sal_memset(list->validations, 0, alloc_size);

    list->num_validations = 0;

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
            (validate_selection_map_add(unit, sid,
                                        list,
                                        selection_map,
                                        select_map_index_offset,
                                        val_type));
    }

    /* Resize array */
    SHR_IF_ERR_EXIT
        (validate_list_resize(unit, max_entries, list));

    *list_ptr = list;

 exit:
    if (SHR_FUNC_ERR()) {
        validate_list_destroy(list);
    }

    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_db_validate_info_create(int unit,
                               bcmlrd_sid_t sid,
                               const bcmltm_db_field_selection_t *selection,
                               bcmltm_db_validate_info_t **info_ptr)
{
    bcmltm_db_validate_info_t *info = NULL;

    SHR_FUNC_ENTER(unit);

    *info_ptr = NULL;

    /* Allocate info struct */
    SHR_ALLOC(info, sizeof(*info), "bcmltmDbValidateInfo");
    SHR_NULL_CHECK(info, SHR_E_MEMORY);
    sal_memset(info, 0, sizeof(*info));

    /* Create Field Validation lists for Keys and Values */
    SHR_IF_ERR_EXIT
        (validate_list_create(unit, sid,
                              selection,
                              BCMLTM_FIELD_VALIDATE_TYPE_KEY,
                              &info->keys));
    SHR_IF_ERR_EXIT
        (validate_list_create(unit, sid,
                              selection,
                              BCMLTM_FIELD_VALIDATE_TYPE_VALUE,
                              &info->values));

    *info_ptr = info;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_validate_info_destroy(info);
        *info_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

void
bcmltm_db_validate_info_destroy(bcmltm_db_validate_info_t *info)
{
    if (info == NULL) {
        return;
    }

    /* Destroy field validations for Keys and Values */
    validate_list_destroy(info->values);
    validate_list_destroy(info->keys);

    SHR_FREE(info);

    return;
}
