/*! \file bcmltm_fa_field.c
 *
 * Logical Table Manager Field Adaptation
 *
 * API field to/from Working Buffer conversion
 *
 * This file contains the LTM FA stage functions which translate
 * API field values to/from Working Buffer locations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_util.h>

#include <bcmlrd/bcmlrd_table.h>

#include <bcmltm/bcmltm_tree.h>
#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm_fa_tree_nodes_internal.h>
#include <bcmltm/bcmltm_fa_util_internal.h>
#include <bcmltm/bcmltm_wb_types_internal.h>
#include <bcmltm/bcmltm_stats_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_FIELDADAPTATION


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Copy API fields into the Working Buffer.
 *
 * The internal implementation of FA nodes which copy API fields into
 * the Working Buffer.  May permit missing fields or not based on
 * the all_required parameter.
 *
 * \param [in] unit Unit number.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] fs_map Pointer to the conditional field list selection info.
 * \param [out] map_index_p Pointer field list select map index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmltm_fa_select_map_index(int unit,
                           uint32_t *ltm_buffer,
                           bcmltm_field_select_mapping_t *fs_map,
                           uint32_t *map_index_p)
{
    uint32_t map_index;

    if (fs_map->map_index_offset != BCMLTM_WB_OFFSET_INVALID) {
        map_index = ltm_buffer[fs_map->map_index_offset];
        /* The map index of a select field is +1 to reserve 0 for invalid */
        if (map_index > fs_map->num_maps) {
            return SHR_E_INTERNAL;
        }
    } else {
        map_index = 0;
        if (fs_map->num_maps != 1) {
            return SHR_E_INTERNAL;
        }
    }

    *map_index_p = map_index;
    return SHR_E_NONE;
}

/*!
 * \brief Copy API fields into the Working Buffer.
 *
 * The internal implementation of FA nodes which copy API fields into
 * the Working Buffer.  May permit missing fields or not based on
 * the all_required parameter.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] field_list Pointer to the list of fields to process.
 * \param [in] fields_required Indicates the mapped fields existence
 *             requirement choice.  One of BCMLTM_API_FIELDS_REQUIRED_*.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmltm_fa_node_api_fields_to_wb(int unit,
                                bcmltm_lt_md_t *lt_md,
                                bcmltm_lt_state_t *lt_state,
                                bcmltm_entry_t *lt_entry,
                                uint32_t *ltm_buffer,
                                bcmltm_field_map_list_t *field_list,
                                bcmltm_fields_required_t fields_required)
{
    bool field_range_valid = TRUE;
    bool logical;
    uint32_t fix, field_count, found_count, ltid;
    bcmltm_field_map_t *field_maps_list, *cur_field_map = NULL;
    uint32_t field_val[BCMDRD_MAX_PT_WSIZE];
    bcmltm_field_list_t *api_field_data;
    bcmltm_field_stats_t field_err_stat;

    SHR_FUNC_ENTER(unit);

    /* Initialize in case of error */
    field_val[0] = field_val[1] = 0;

    ltid = lt_md->params->ltid;
    logical =
        (lt_md->params->lt_flags & BCMLTM_LT_FLAGS_PASSTHRU) ? FALSE : TRUE;

    field_count = field_list->num_fields;
    field_maps_list = field_list->field_maps;
    found_count = 0;
    for (fix = 0; fix < field_count; fix++) {
        cur_field_map = &(field_maps_list[fix]);

        api_field_data = bcmltm_api_find_field(lt_entry->in_fields,
                                               cur_field_map->field_id,
                                               cur_field_map->field_idx);
        if (api_field_data == NULL) {  /* Value fields are optional */
            if (fields_required == BCMLTM_FIELDS_REQUIRED_ALL ) {
                /* Field missing */
                BCMLTM_LOG_VERBOSE_FIELD_MISSING(unit,
                                                 ltid,
                                                 cur_field_map->field_id,
                                                 cur_field_map->field_idx,
                                                 logical);
                SHR_ERR_EXIT(SHR_E_PARAM);
            } else {
                /* The WB schan entries should be set to default */
                continue;
            }
        }

        sal_memset(field_val, 0, sizeof(field_val));
        if ((fields_required == BCMLTM_FIELDS_REQUIRED_UNSET) &&
            ((api_field_data->flags & SHR_FMM_FIELD_DEL) != 0)) {
            shr_uint64_to_uint32_array(cur_field_map->default_value,
                                       field_val);
        } else {
            /* Check field range for Logical Tables, no PT PassThru. */
            if (logical &&
                ((api_field_data->data < cur_field_map->minimum_value) ||
                 (api_field_data->data > cur_field_map->maximum_value))) {
                /* Out of bounds field value */
                field_range_valid = FALSE;

                BCMLTM_LOG_VERBOSE_FIELD_OOB(unit,
                                             ltid,
                                             cur_field_map->field_id,
                                             cur_field_map->field_idx,
                                             logical,
                                             api_field_data->data,
                                             cur_field_map->minimum_value,
                                             cur_field_map->maximum_value);

                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            shr_uint64_to_uint32_array(api_field_data->data, field_val);
        }

        while (cur_field_map != NULL) {
            
            /* Field validation check */
            field_range_valid = FALSE;
            field_range_valid = bcmltm_field_fit_check(&(cur_field_map->wbc),
                                                 field_val);

            if (field_range_valid != TRUE) {
                BCMLTM_LOG_VERBOSE_FIELD_UNFIT(unit,
                                               ltid,
                                               cur_field_map->field_id,
                                               cur_field_map->field_idx,
                                               logical,
                                               api_field_data->data,
                                               &(cur_field_map->wbc));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            bcmltm_field_value_into_buffer(cur_field_map,
                                           field_val, ltm_buffer);

            /* Update PT entry changed flag if appropriate. */
            if (cur_field_map->pt_changed_offset != BCMLTM_WB_OFFSET_INVALID) {
                ltm_buffer[cur_field_map->pt_changed_offset] =
                    BCMLTM_PT_ENTRY_CHANGED_VALUE;
            }

            /* Next link in map list for this API field */
            cur_field_map = cur_field_map->next_map;
        }

        found_count++;
    }

    if (fields_required == BCMLTM_FIELDS_REQUIRED_ALL_OR_NONE) {
        if (found_count == 0) {
            /* Record absence of fields in Working Buffer. */
            ltm_buffer[field_list->index_absent_offset] =
                BCMLTM_INDEX_ABSENT;
        } else if (found_count != field_count) {
            const char *table_name;
            const char *table_sid_type;

            table_name = bcmltm_table_sid_to_name(unit, ltid, logical);
            table_sid_type = bcmltm_table_sid_type_str(logical);
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "LTM %s(%s=%d): "
                                    "Incorrect number of key fields\n"),
                         table_name, table_sid_type, ltid));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        /* Else, standard all key fields present */
    }

 exit:
    if (SHR_FUNC_ERR()) {
        field_err_stat = field_range_valid ?
            BCMLRD_FIELD_FIELD_LIST_ERROR_COUNT :
            BCMLRD_FIELD_FIELD_SIZE_ERROR_COUNT;

        bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                               field_err_stat);
    }
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_fa_node_api_key_fields_to_wb(int unit,
                                    bcmltm_lt_md_t *lt_md,
                                    bcmltm_lt_state_t *lt_state,
                                    bcmltm_entry_t *lt_entry,
                                    uint32_t *ltm_buffer,
                                    void *node_cookie)
{
    bcmltm_fields_required_t keys_required;

    keys_required =
        ((lt_entry->opcode.lt_opcode == BCMLT_OPCODE_TRAVERSE) &&
         (lt_entry->in_fields == NULL)) ?
        BCMLTM_FIELDS_REQUIRED_NONE :
        BCMLTM_FIELDS_REQUIRED_ALL;

    return bcmltm_fa_node_api_fields_to_wb(unit, lt_md, lt_state,
                                           lt_entry, ltm_buffer,
                BCMLTM_FA_SELECT_FIELD_MAP_LIST(node_cookie, 0),
                                           keys_required);
}

int
bcmltm_fa_node_api_alloc_key_fields_to_wb(int unit,
                                          bcmltm_lt_md_t *lt_md,
                                          bcmltm_lt_state_t *lt_state,
                                          bcmltm_entry_t *lt_entry,
                                          uint32_t *ltm_buffer,
                                          void *node_cookie)
{
    return bcmltm_fa_node_api_fields_to_wb(unit, lt_md, lt_state,
                                           lt_entry, ltm_buffer,
                BCMLTM_FA_SELECT_FIELD_MAP_LIST(node_cookie, 0),
                              BCMLTM_FIELDS_REQUIRED_ALL_OR_NONE);
}

int
bcmltm_fa_node_api_value_fields_to_wb(int unit,
                                      bcmltm_lt_md_t *lt_md,
                                      bcmltm_lt_state_t *lt_state,
                                      bcmltm_entry_t *lt_entry,
                                      uint32_t *ltm_buffer,
                                      void *node_cookie)
{
    uint32_t map_index;
    bcmltm_field_select_mapping_t *fs_map;

    SHR_FUNC_ENTER(unit);

    fs_map = BCMLTM_FA_FS_MAPPING(node_cookie);

    SHR_IF_ERR_EXIT
        (bcmltm_fa_select_map_index(unit, ltm_buffer,
                                    fs_map, &map_index));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmltm_fa_node_api_fields_to_wb(unit, lt_md, lt_state,
                                         lt_entry, ltm_buffer,
                BCMLTM_FA_SELECT_FIELD_MAP_LIST(node_cookie, map_index),
                                         BCMLTM_FIELDS_REQUIRED_NONE));
 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_api_fields_unset_to_wb(int unit,
                                      bcmltm_lt_md_t *lt_md,
                                      bcmltm_lt_state_t *lt_state,
                                      bcmltm_entry_t *lt_entry,
                                      uint32_t *ltm_buffer,
                                      void *node_cookie)
{
    uint32_t map_index;
    bcmltm_field_select_mapping_t *fs_map;

    SHR_FUNC_ENTER(unit);

    fs_map = BCMLTM_FA_FS_MAPPING(node_cookie);

    SHR_IF_ERR_EXIT
        (bcmltm_fa_select_map_index(unit, ltm_buffer,
                                    fs_map, &map_index));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmltm_fa_node_api_fields_to_wb(unit, lt_md, lt_state,
                                         lt_entry, ltm_buffer,
                BCMLTM_FA_SELECT_FIELD_MAP_LIST(node_cookie, map_index),
                                         BCMLTM_FIELDS_REQUIRED_UNSET));
 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_wb_to_api_fields(int unit,
                                bcmltm_lt_md_t *lt_md,
                                bcmltm_lt_state_t *lt_state,
                                bcmltm_entry_t *lt_entry,
                                uint32_t *ltm_buffer,
                                void *node_cookie)
{
    uint32_t fix, field_count;
    bcmltm_field_map_list_t *field_list;
    bcmltm_field_map_t *field_maps_list, *cur_field_map;
    uint32_t field_val[BCMDRD_MAX_PT_WSIZE];
    bcmltm_field_list_t *api_field_data, *last_out_field;
    uint64_t field_val64;
    uint32_t map_index;
    bcmltm_field_select_mapping_t *fs_map;
    uint32_t pts_offset;
    bool pt_suppress = FALSE;

    SHR_FUNC_ENTER(unit);

    fs_map = BCMLTM_FA_FS_MAPPING(node_cookie);

    pts_offset = fs_map->pt_suppress_offset;
    if (pts_offset != BCMLTM_WB_OFFSET_INVALID) {
        if (ltm_buffer[pts_offset] != 0) {
            pt_suppress = TRUE;
        }
    }

    SHR_IF_ERR_EXIT
        (bcmltm_fa_select_map_index(unit, ltm_buffer,
                                    fs_map, &map_index));

    field_list = BCMLTM_FA_SELECT_FIELD_MAP_LIST(node_cookie, map_index);
    field_count = field_list->num_fields;
    field_maps_list = field_list->field_maps;
    last_out_field = lt_entry->out_fields;
    if (last_out_field != NULL) {
        while (last_out_field->next != NULL) {
            last_out_field = last_out_field->next;
        }
        /* Traverse out fields to end of list for returning data
         * While the entry structure provided to the LTM
         * should have no output fields in the list, this permits
         * multiple data return nodes in a read op tree.
         */
    }

    for (fix = 0; fix < field_count; fix++) {
        cur_field_map = &(field_maps_list[fix]);

        if (pt_suppress) {
            /*
             * PT op was skipped due to LTA Transform option.
             * Supply default values to output.
             */
            if (lt_entry->attrib & BCMLT_ENT_ATTR_FILTER_DEFAULTS) {
                /* Do not add default value fields to output list */
                continue;
            } else {
                field_val64 = cur_field_map->default_value;
            }
        } else {
            sal_memset(field_val, 0, sizeof(field_val));
            bcmltm_field_value_from_buffer(cur_field_map,
                                           field_val, ltm_buffer);

            shr_uint32_array_to_uint64(field_val, &field_val64);

            if (lt_entry->attrib & BCMLT_ENT_ATTR_FILTER_DEFAULTS) {
                if (field_val64 == cur_field_map->default_value) {
                    /* Do not add default value fields to output list */
                    continue;
                }
            }
        }

        api_field_data = (lt_entry->field_alloc_cb)();
        SHR_NULL_CHECK(api_field_data, SHR_E_MEMORY);

        /* Add retrieve field value to entry */
        api_field_data->data = field_val64;
        api_field_data->id = cur_field_map->field_id;
        api_field_data->idx = cur_field_map->field_idx;
        api_field_data->flags = 0; /* Flags only set by APP layer */
        api_field_data->next = NULL;
        if (last_out_field == NULL) {
            lt_entry->out_fields = api_field_data;
        } else {
            last_out_field->next = api_field_data;
        }
        last_out_field = api_field_data;
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_wb_to_api_alloc_fields(int unit,
                                      bcmltm_lt_md_t *lt_md,
                                      bcmltm_lt_state_t *lt_state,
                                      bcmltm_entry_t *lt_entry,
                                      uint32_t *ltm_buffer,
                                      void *node_cookie)
{
    uint32_t fix, field_count;
    bcmltm_field_map_list_t *field_list;
    bcmltm_field_map_t *field_maps_list, *cur_field_map;
    uint32_t field_val[BCMDRD_MAX_PT_WSIZE];
    bcmltm_field_list_t *api_field_data, *last_out_field;
    uint64_t field_val64;

    SHR_FUNC_ENTER(unit);

    field_list = BCMLTM_FA_SELECT_FIELD_MAP_LIST(node_cookie, 0);
    if (ltm_buffer[field_list->index_absent_offset] !=
        BCMLTM_INDEX_ABSENT) {
        /*
         * No allocation took place, so no need to return
         * key fields mapped to index.
         */
        SHR_EXIT();
    }

    field_count = field_list->num_fields;
    field_maps_list = field_list->field_maps;
    last_out_field = lt_entry->out_fields;
    if (last_out_field != NULL) {
        while (last_out_field->next != NULL) {
            last_out_field = last_out_field->next;
        }
        /* Traverse out fields to end of list for returning data
         * While the entry structure provided to the LTM
         * should have no output fields in the list, this permits
         * multiple data return nodes in a read op tree.
         */
    }

    for (fix = 0; fix < field_count; fix++) {
        cur_field_map = &(field_maps_list[fix]);

        sal_memset(field_val, 0, sizeof(field_val));
        bcmltm_field_value_from_buffer(cur_field_map,
                                       field_val, ltm_buffer);

        shr_uint32_array_to_uint64(field_val, &field_val64);

        api_field_data = (lt_entry->field_alloc_cb)();
        SHR_NULL_CHECK(api_field_data, SHR_E_MEMORY);

        /* Add retrieve field value to entry */
        api_field_data->data = field_val64;
        api_field_data->id = cur_field_map->field_id;
        api_field_data->idx = cur_field_map->field_idx;
        api_field_data->flags = 0; /* Flags only set by APP layer */
        api_field_data->next = NULL;
        if (last_out_field == NULL) {
            lt_entry->out_fields = api_field_data;
        } else {
            last_out_field->next = api_field_data;
        }
        last_out_field = api_field_data;
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_wb_to_api_key_fields(int unit,
                                    bcmltm_lt_md_t *lt_md,
                                    bcmltm_lt_state_t *lt_state,
                                    bcmltm_entry_t *lt_entry,
                                    uint32_t *ltm_buffer,
                                    void *node_cookie)
{
    uint32_t fix, field_count;
    bcmltm_field_map_list_t *field_list;
    bcmltm_field_map_t *field_maps_list, *cur_field_map;
    uint32_t field_val[BCMDRD_MAX_PT_WSIZE];
    bcmltm_field_list_t *api_field_data, *last_out_field;
    uint64_t field_val64;
    uint32_t map_index;
    bcmltm_field_select_mapping_t *fs_map;

    SHR_FUNC_ENTER(unit);

    fs_map = BCMLTM_FA_FS_MAPPING(node_cookie);

    /* No selection in keys, this should be the trivial map */
    SHR_IF_ERR_EXIT
        (bcmltm_fa_select_map_index(unit, ltm_buffer,
                                    fs_map, &map_index));

    field_list = BCMLTM_FA_SELECT_FIELD_MAP_LIST(node_cookie, map_index);
    field_count = field_list->num_fields;
    field_maps_list = field_list->field_maps;
    last_out_field = lt_entry->out_fields;
    if (last_out_field != NULL) {
        while (last_out_field->next != NULL) {
            last_out_field = last_out_field->next;
        }
        /* Traverse out fields to end of list for returning data
         * While the entry structure provided to the LTM
         * should have no output fields in the list, this permits
         * multiple data return nodes in a read op tree.
         */
    }

    for (fix = 0; fix < field_count; fix++) {
        cur_field_map = &(field_maps_list[fix]);

        sal_memset(field_val, 0, sizeof(field_val));
        bcmltm_field_value_from_buffer(cur_field_map,
                                       field_val, ltm_buffer);

        shr_uint32_array_to_uint64(field_val, &field_val64);

        api_field_data = (lt_entry->field_alloc_cb)();
        SHR_NULL_CHECK(api_field_data, SHR_E_MEMORY);

        /* Add retrieve field value to entry */
        api_field_data->data = field_val64;
        api_field_data->id = cur_field_map->field_id;
        api_field_data->idx = cur_field_map->field_idx;
        api_field_data->flags = 0; /* Flags only set by APP layer */
        api_field_data->next = NULL;
        if (last_out_field == NULL) {
            lt_entry->out_fields = api_field_data;
        } else {
            last_out_field->next = api_field_data;
        }
        last_out_field = api_field_data;
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_fixed_fields_to_wb(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie)
{
    uint32_t fix, field_count;
    bcmltm_fixed_field_t *cur_fixed, *field_list;
    bcmltm_fixed_field_list_t *fixed_field_data;

    fixed_field_data = BCMLTM_FA_FIXED_FIELD_LIST(node_cookie);
    field_count = fixed_field_data->num_fixed_fields;
    field_list = fixed_field_data->fixed_fields;

    for (fix = 0; fix < field_count; fix++) {
        cur_fixed = &(field_list[fix]);

        /* Should we add an assert that (maxbit - minbit +1) <= 32? */
        bcmltm_field_value_into_wbc(&(cur_fixed->wbc),
                                    &(cur_fixed->field_value),
                                    ltm_buffer);

        /* Update PT entry changed flag if appropriate */
        if (cur_fixed->pt_changed_offset != BCMLTM_WB_OFFSET_INVALID) {
            ltm_buffer[cur_fixed->pt_changed_offset] =
                BCMLTM_PT_ENTRY_CHANGED_VALUE;
        }
    }

    return SHR_E_NONE;
}

int
bcmltm_fa_node_delete_fixed_fields_to_wb(int unit,
                                         bcmltm_lt_md_t *lt_md,
                                         bcmltm_lt_state_t *lt_state,
                                         bcmltm_entry_t *lt_entry,
                                         uint32_t *ltm_buffer,
                                         void *node_cookie)
{
    uint32_t fix, field_count;
    bcmltm_fixed_field_t *cur_fixed, *field_list;
    bcmltm_fixed_field_list_t *fixed_field_data;

    fixed_field_data = BCMLTM_FA_FIXED_FIELD_LIST(node_cookie);
    field_count = fixed_field_data->num_fixed_fields;
    field_list = fixed_field_data->fixed_fields;

    for (fix = 0; fix < field_count; fix++) {
        cur_fixed = &(field_list[fix]);

        /* Should we add an assert that (maxbit - minbit +1) <= 32? */
        bcmltm_field_value_into_wbc(&(cur_fixed->wbc),
                                    &(cur_fixed->delete_value),
                                    ltm_buffer);

        /* Update PT entry changed flag if appropriate */
        if (cur_fixed->pt_changed_offset != BCMLTM_WB_OFFSET_INVALID) {
            ltm_buffer[cur_fixed->pt_changed_offset] =
                BCMLTM_PT_ENTRY_CHANGED_VALUE;
        }
    }

    return SHR_E_NONE;
}

int
bcmltm_fa_node_default_fields_to_wb(int unit,
                                    bcmltm_lt_md_t *lt_md,
                                    bcmltm_lt_state_t *lt_state,
                                    bcmltm_entry_t *lt_entry,
                                    uint32_t *ltm_buffer,
                                    void *node_cookie)
{
    uint32_t fix, field_count;
    bcmltm_field_map_list_t *field_list;
    bcmltm_field_map_t *field_maps_list, *cur_field_map;
    uint32_t field_val[BCMDRD_MAX_PT_WSIZE];
    uint32_t map_index;
    bcmltm_field_select_mapping_t *fs_map;

    SHR_FUNC_ENTER(unit);

    fs_map = BCMLTM_FA_FS_MAPPING(node_cookie);

    SHR_IF_ERR_EXIT
        (bcmltm_fa_select_map_index(unit, ltm_buffer,
                                    fs_map, &map_index));

    field_list = BCMLTM_FA_SELECT_FIELD_MAP_LIST(node_cookie, map_index);
    field_count = field_list->num_fields;
    field_maps_list = field_list->field_maps;
    for (fix = 0; fix < field_count; fix++) {
        cur_field_map = &(field_maps_list[fix]);

        /*
         * Default value must be contained in the first field map of
         * of the field map chain.
         */
        field_val[0] = cur_field_map->default_value & 0xffffffff;
        field_val[1] = (cur_field_map->default_value >> 32) & 0xffffffff;

        while (cur_field_map != NULL) {
            bcmltm_field_value_into_buffer(cur_field_map,
                                           field_val, ltm_buffer);

            /* Update PT entry changed flag if appropriate. */
            if (cur_field_map->pt_changed_offset != BCMLTM_WB_OFFSET_INVALID) {
                ltm_buffer[cur_field_map->pt_changed_offset] =
                    BCMLTM_PT_ENTRY_CHANGED_VALUE;
            }

            /* Next link in map list for this API field */
            cur_field_map = cur_field_map->next_map;
        }
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_wbc_to_wbc(int unit,
                          bcmltm_lt_md_t *lt_md,
                          bcmltm_lt_state_t *lt_state,
                          bcmltm_entry_t *lt_entry,
                          uint32_t *ltm_buffer,
                          void *node_cookie)
{
    uint32_t wix, wbc_count;
    bcmltm_wb_copy_list_t *wbc_list;
    bcmltm_wb_copy_t *wb_copy_list, *cur_wb_copy;
    uint32_t wbc_val[BCMDRD_MAX_PT_WSIZE];

    wbc_list = BCMLTM_FA_WB_COPY_LIST(node_cookie);
    wbc_count = wbc_list->num_wb_copy;
    wb_copy_list = wbc_list->wb_copy;
    for (wix = 0; wix < wbc_count; wix++) {
        cur_wb_copy = &(wb_copy_list[wix]);

        /* Retrieve value from source WBC */
        bcmltm_field_value_from_wbc(&(cur_wb_copy->wbc_source),
                                    wbc_val, ltm_buffer);

        /* Set value at destination WBC */
        bcmltm_field_value_into_wbc(&(cur_wb_copy->wbc_destination),
                                    wbc_val, ltm_buffer);
    }

    return SHR_E_NONE;
}

int
bcmltm_fa_node_alloc_wbc_to_wbc(int unit,
                                bcmltm_lt_md_t *lt_md,
                                bcmltm_lt_state_t *lt_state,
                                bcmltm_entry_t *lt_entry,
                                uint32_t *ltm_buffer,
                                void *node_cookie)
{
    bcmltm_wb_copy_list_t *wbc_list;

    wbc_list = BCMLTM_FA_WB_COPY_LIST(node_cookie);
    if (ltm_buffer[wbc_list->index_absent_offset] != BCMLTM_INDEX_ABSENT) {
        /* Nothing to do */
        return SHR_E_NONE;
    }

    /* Otherwise, copy allocatable key fields to necessary destinations. */
    return bcmltm_fa_node_wbc_to_wbc(unit, lt_md, lt_state,
                                     lt_entry, ltm_buffer, node_cookie);
}

int
bcmltm_fa_node_read_only_input_check(int unit,
                                     bcmltm_lt_md_t *lt_md,
                                     bcmltm_lt_state_t *lt_state,
                                     bcmltm_entry_t *lt_entry,
                                     uint32_t *ltm_buffer,
                                     void *node_cookie)
{
    uint32_t fix, field_count;
    bcmltm_ro_field_list_t *field_list;
    bcmltm_field_spec_t *field_spec_list, *cur_field_spec;
    bcmltm_field_list_t *api_field_data;

    SHR_FUNC_ENTER(unit);

    field_list = BCMLTM_FA_RO_FIELD_LIST(node_cookie);
    field_count = field_list->num_field_specs;
    field_spec_list = field_list->field_specs;
    for (fix = 0; fix < field_count; fix++) {
        cur_field_spec = &(field_spec_list[fix]);
        api_field_data = bcmltm_api_find_field(lt_entry->in_fields,
                                               cur_field_spec->field_id,
                                               cur_field_spec->field_idx);
        if (api_field_data != NULL) {
            /* Read only fields may not be in input */
            BCMLTM_LOG_VERBOSE_FIELD_RO_WRITE(unit,
                                              lt_md->params->ltid,
                                              cur_field_spec->field_id,
                                              cur_field_spec->field_idx,
                                              TRUE);
            bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                                   BCMLRD_FIELD_FIELD_LIST_ERROR_COUNT);
            SHR_ERR_EXIT(SHR_E_ACCESS);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_wide_field_check(int unit,
                                bcmltm_lt_md_t *lt_md,
                                bcmltm_lt_state_t *lt_state,
                                bcmltm_entry_t *lt_entry,
                                uint32_t *ltm_buffer,
                                void *node_cookie)
{
    uint32_t fix, field_count, aix, array_count, found_count;
    bcmltm_wide_field_list_t *field_list;
    bcmltm_wide_field_t *wide_field_list, *cur_wide_field;
    bcmltm_field_list_t *api_field_data;

    SHR_FUNC_ENTER(unit);

    field_list = BCMLTM_FA_WIDE_FIELD_LIST(node_cookie);
    field_count = field_list->num_wide_fields;
    wide_field_list = field_list->wide_field;
    for (fix = 0; fix < field_count; fix++) {
        cur_wide_field = &(wide_field_list[fix]);

        array_count = cur_wide_field->num_field_idx;
        found_count = 0;
        for (aix = 0; aix < array_count; aix++) {
            api_field_data = bcmltm_api_find_field(lt_entry->in_fields,
                                                   cur_wide_field->field_id,
                                                   aix);
            if (api_field_data != NULL) {
                found_count++;
            }
        }

        /*
         * Wide fields must have all or none of the array elements
         * present in the API input list.
         */
        if ((found_count != 0) && (found_count != array_count)) {
            BCMLTM_LOG_VERBOSE_WIDE_FRAGMENT(unit,
                                             lt_md->params->ltid,
                                             cur_wide_field->field_id,
                                             TRUE);
            bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                                   BCMLRD_FIELD_FIELD_LIST_ERROR_COUNT);
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}
