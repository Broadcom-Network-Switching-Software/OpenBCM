/*! \file bcmltm_fa_field_select.c
 *
 * Logical Table Manager Field Adaptation
 *
 * API field with overlays to/from Working Buffer conversion
 *
 * This file contains the LTM FA stage functions which translate
 * API field values to/from Working Buffer locations when field overlays
 * are present.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

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
 * \brief Determine field map index based on field select.
 *
 * Determine the correct field map list index based on a selector field
 * value.
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
        map_index = 1;
        if (fs_map->num_maps != 1) {
            return SHR_E_INTERNAL;
        }
    }

    *map_index_p = map_index;
    return SHR_E_NONE;
}

/*!
 * \brief Determine fixed field map index based on field select.
 *
 * Determine the correct fixed field map list index based on a selector field
 * value.
 *
 * \param [in] unit Unit number.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] ff_map Pointer to the conditional field list selection info.
 * \param [out] map_index_p Pointer field list select map index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmltm_fa_select_fixed_map_index(int unit,
                                 uint32_t *ltm_buffer,
                                 bcmltm_fixed_field_mapping_t *ff_map,
                                 uint32_t *map_index_p)
{
    uint32_t map_index;

    if (ff_map->map_index_offset != BCMLTM_WB_OFFSET_INVALID) {
        map_index = ltm_buffer[ff_map->map_index_offset];
        /* The map index of a select field is +1 to reserve 0 for invalid */
        if (map_index > ff_map->num_maps) {
            return SHR_E_INTERNAL;
        }
    } else {
        map_index = 1;
        if (ff_map->num_maps != 1) {
            return SHR_E_INTERNAL;
        }
    }

    *map_index_p = map_index;
    return SHR_E_NONE;
}

/*!
 * \brief Copy input LTM entry fields into the Working Buffer API field cache.
 *
 * The internal implementation of FA nodes which copy LTM entry input
 * fields into the Working Buffer's API field cache area.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] field_map_list Pointer to the list of field maps to process.
 * \param [in] fields_required Indicates the mapped fields existence
 *             requirement choice.  One of BCMLTM_API_FIELDS_REQUIRED_*.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmltm_fa_node_in_fields_to_api_wb(int unit,
                                   bcmltm_lt_md_t *lt_md,
                                   bcmltm_lt_state_t *lt_state,
                                   bcmltm_entry_t *lt_entry,
                                   uint32_t *ltm_buffer,
                                   bcmltm_field_map_list_t *field_map_list,
                                   bcmltm_fields_required_t fields_required)
{
    bool field_range_valid = TRUE;
    bool logical;
    uint32_t fix, field_count, found_count, ltid;
    bcmltm_field_map_t *field_maps, *cur_field_map = NULL,
        *error_field_map = NULL;
    uint32_t field_val[BCMDRD_MAX_PT_WSIZE];
    uint32_t *ptr_math;
    bcmltm_field_list_t *api_field_data, *wb_field_dest,
        *error_field_data = NULL;
    bcmltm_field_list_t default_field;

    SHR_FUNC_ENTER(unit);

    /* Initialize in case of error */
    field_val[0] = field_val[1] = 0;

    ltid = lt_md->params->ltid;
    logical =
        (lt_md->params->lt_flags & BCMLTM_LT_FLAGS_PASSTHRU) ? FALSE : TRUE;

    field_count = field_map_list->num_fields;
    field_maps = field_map_list->field_maps;
    found_count = 0;
    for (fix = 0; fix < field_count; fix++) {
        cur_field_map = &(field_maps[fix]);

        ptr_math = ltm_buffer + cur_field_map->apic_offset;
        wb_field_dest = (bcmltm_field_list_t *)ptr_math;

        api_field_data = bcmltm_api_find_field(lt_entry->in_fields,
                                               cur_field_map->field_id,
                                               cur_field_map->field_idx);
        if (api_field_data == NULL) {  /* Value fields are optional */
            if (fields_required == BCMLTM_FIELDS_REQUIRED_ALL) {
                /* Only record first missing required field error. */
                if (error_field_map == NULL) {
                    error_field_map = cur_field_map;
                    SHR_IF_ERR_CONT(SHR_E_PARAM);
                }
                continue;
            } else if (fields_required == BCMLTM_FIELDS_REQUIRED_UNSET) {
                /*
                 * UNSET indicates UPDATE values overwrite, so
                 * API default values were already copied into API cache.
                 */
                continue;
            } else {
                /* Supply API default value */
                sal_memset(&default_field, 0, sizeof(default_field));
                default_field.id = cur_field_map->field_id;
                default_field.idx = cur_field_map->field_idx;
                default_field.data = cur_field_map->default_value;
                api_field_data = &default_field;
            }
        } else {
            found_count++;
        }

        if ((fields_required == BCMLTM_FIELDS_REQUIRED_UNSET) &&
            ((api_field_data->flags & SHR_FMM_FIELD_DEL) != 0)) {
            /*
             * UNSET indicates UPDATE overwrite, so API cache already
             * has field info.  Just set value to default in case the
             * PT read retrieved a different state.
             */
            wb_field_dest->data = cur_field_map->default_value;
        } else {
            /* Check field range for Logical Tables, not PT PassThru. */
            if (logical &&
                ((api_field_data->data < cur_field_map->minimum_value) ||
                 (api_field_data->data > cur_field_map->maximum_value))) {
                /* Only record first out of bounds field value. */
                if (error_field_map == NULL) {
                    field_range_valid = FALSE;
                    error_field_map = cur_field_map;
                    SHR_IF_ERR_CONT(SHR_E_PARAM);
                }
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

            sal_memcpy(wb_field_dest, api_field_data,
                       sizeof(bcmltm_field_list_t));
        }
    }

    if (fields_required == BCMLTM_FIELDS_REQUIRED_ALL_OR_NONE) {
        if (found_count == 0) {
            /* Record absence of fields in Working Buffer. */
            ltm_buffer[field_map_list->index_absent_offset] =
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
    if (SHR_FUNC_VAL_IS(SHR_E_PARAM)){
        if (field_range_valid) {
            if (error_field_map != NULL) {
                BCMLTM_LOG_VERBOSE_FIELD_MISSING(unit,
                                                 ltid,
                                                 error_field_map->field_id,
                                                 error_field_map->field_idx,
                                                 logical);
            }
            bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                                   BCMLRD_FIELD_FIELD_LIST_ERROR_COUNT);
        } else {
            if ((error_field_map != NULL) && (error_field_data != NULL)) {
                BCMLTM_LOG_VERBOSE_FIELD_OOB(unit,
                                             ltid,
                                             error_field_map->field_id,
                                             error_field_map->field_idx,
                                             logical,
                                             error_field_data->data,
                                             error_field_map->minimum_value,
                                             error_field_map->maximum_value);
            }
            bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                                   BCMLRD_FIELD_FIELD_SIZE_ERROR_COUNT);
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Combined copy cached API values within the Working Buffer.
 *
 * The central implementaion of several tree function nodes which copy
 * Working Buffer API field cache values within the Working Buffer
 * to some other location - PT entry, IMM entry, memory parameter,
 * or other WB internal location.
 *
 * \param [in] unit Unit number.
 * \param [in] field_map_list Pointer to the list of API field maps
 *                            to process.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcmltm_fa_node_api_wb_to_wbc_core(int unit,
                                  bcmltm_field_map_list_t *field_map_list,
                                  uint32_t *ltm_buffer)
{
    uint32_t fix, field_count;
    bcmltm_field_map_t *field_maps, *cur_field_map;

    field_count = field_map_list->num_fields;
    field_maps = field_map_list->field_maps;
    for (fix = 0; fix < field_count; fix++) {
        cur_field_map = &(field_maps[fix]);

        while (cur_field_map != NULL) {
            /* Copy non read-only fields */
            if (!(cur_field_map->flags & BCMLTM_FIELD_READ_ONLY)) {
                bcmltm_field_map_into_buffer(cur_field_map, ltm_buffer);

                /* Update PT entry changed flag if appropriate. */
                if (cur_field_map->pt_changed_offset !=
                    BCMLTM_WB_OFFSET_INVALID) {
                    ltm_buffer[cur_field_map->pt_changed_offset] =
                        BCMLTM_PT_ENTRY_CHANGED_VALUE;
                }
            }

            /* Next link in map list for this API field */
            cur_field_map = cur_field_map->next_map;
        }
    }

    return SHR_E_NONE;
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_fa_node_in_key_fields_to_api_wb(int unit,
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

    return bcmltm_fa_node_in_fields_to_api_wb(unit, lt_md, lt_state,
                                              lt_entry, ltm_buffer,
                BCMLTM_FA_SELECT_FIELD_MAP_LIST(node_cookie, 0),
                                              keys_required);
}

int
bcmltm_fa_node_in_alloc_key_fields_to_api_wb(int unit,
                                             bcmltm_lt_md_t *lt_md,
                                             bcmltm_lt_state_t *lt_state,
                                             bcmltm_entry_t *lt_entry,
                                             uint32_t *ltm_buffer,
                                             void *node_cookie)
{
    return bcmltm_fa_node_in_fields_to_api_wb(unit, lt_md, lt_state,
                                              lt_entry, ltm_buffer,
                BCMLTM_FA_SELECT_FIELD_MAP_LIST(node_cookie, 0),
                              BCMLTM_FIELDS_REQUIRED_ALL_OR_NONE);
}

int
bcmltm_fa_node_in_value_fields_to_api_wb(int unit,
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

    if (map_index == 0) {
        /* This field list is not selected. */
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmltm_fa_node_in_fields_to_api_wb(unit, lt_md, lt_state,
                                            lt_entry, ltm_buffer,
                BCMLTM_FA_SELECT_FIELD_MAP_LIST(node_cookie, map_index - 1),
                                            BCMLTM_FIELDS_REQUIRED_NONE));
 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_in_fields_unset_to_api_wb(int unit,
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

    if (map_index == 0) {
        /* This field list is not selected. */
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmltm_fa_node_in_fields_to_api_wb(unit, lt_md, lt_state,
                                            lt_entry, ltm_buffer,
                BCMLTM_FA_SELECT_FIELD_MAP_LIST(node_cookie, map_index - 1),
                                            BCMLTM_FIELDS_REQUIRED_UNSET));
 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_api_wb_to_out_fields(int unit,
                                    bcmltm_lt_md_t *lt_md,
                                    bcmltm_lt_state_t *lt_state,
                                    bcmltm_entry_t *lt_entry,
                                    uint32_t *ltm_buffer,
                                    void *node_cookie)
{
    uint32_t fix, field_count;
    bcmltm_field_map_list_t *field_map_list;
    bcmltm_field_map_t *field_maps, *cur_field_map;
    uint32_t *ptr_math;
    bcmltm_field_list_t *api_field_data, *last_out_field, *wb_field_dest;
    uint32_t map_index;
    bcmltm_field_select_mapping_t *fs_map;
    bool exclude_default = FALSE;

    SHR_FUNC_ENTER(unit);

    fs_map = BCMLTM_FA_FS_MAPPING(node_cookie);

    SHR_IF_ERR_EXIT
        (bcmltm_fa_select_map_index(unit, ltm_buffer,
                                    fs_map, &map_index));

    if (map_index == 0) {
        /* This field list is not selected. */
        SHR_EXIT();
    }

    field_map_list =
        BCMLTM_FA_SELECT_FIELD_MAP_LIST(node_cookie, map_index - 1);

    field_count = field_map_list->num_fields;
    field_maps = field_map_list->field_maps;
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

    if (lt_entry->attrib & BCMLT_ENT_ATTR_FILTER_DEFAULTS) {
        exclude_default = TRUE;
    }

    for (fix = 0; fix < field_count; fix++) {
        cur_field_map = &(field_maps[fix]);

        ptr_math = ltm_buffer + cur_field_map->apic_offset;
        wb_field_dest = (bcmltm_field_list_t *)ptr_math;

        if (exclude_default &&
            (wb_field_dest->data == cur_field_map->default_value)) {
            /* Do not add default value fields to output list */
            continue;
        }

        api_field_data = (lt_entry->field_alloc_cb)();
        SHR_NULL_CHECK(api_field_data, SHR_E_MEMORY);

        /* Add previously constructed API field value to entry */
        sal_memcpy(api_field_data, wb_field_dest,
                   sizeof(bcmltm_field_list_t));

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
bcmltm_fa_node_api_wb_to_out_alloc_fields(int unit,
                                          bcmltm_lt_md_t *lt_md,
                                          bcmltm_lt_state_t *lt_state,
                                          bcmltm_entry_t *lt_entry,
                                          uint32_t *ltm_buffer,
                                          void *node_cookie)
{
    uint32_t fix, field_count;
    bcmltm_field_map_list_t *field_map_list;
    bcmltm_field_map_t *field_maps, *cur_field_map;
    uint32_t *ptr_math;
    bcmltm_field_list_t *api_field_data, *last_out_field, *wb_field_dest;
    uint32_t map_index;
    bcmltm_field_select_mapping_t *fs_map;

    SHR_FUNC_ENTER(unit);

    fs_map = BCMLTM_FA_FS_MAPPING(node_cookie);

    SHR_IF_ERR_EXIT
        (bcmltm_fa_select_map_index(unit, ltm_buffer,
                                    fs_map, &map_index));

    if (map_index == 0) {
        /* This field list is not selected. */
        SHR_EXIT();
    }

    field_map_list =
        BCMLTM_FA_SELECT_FIELD_MAP_LIST(node_cookie, map_index - 1);
    if ((field_map_list->index_absent_offset != BCMLTM_WB_OFFSET_INVALID) &&
        (ltm_buffer[field_map_list->index_absent_offset] !=
         BCMLTM_INDEX_ABSENT)) {
        /*
         * No allocation took place, so no need to return
         * key fields mapped to index.
         */
        SHR_EXIT();
    }

    field_count = field_map_list->num_fields;
    field_maps = field_map_list->field_maps;
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
        cur_field_map = &(field_maps[fix]);

        ptr_math = ltm_buffer + cur_field_map->apic_offset;
        wb_field_dest = (bcmltm_field_list_t *)ptr_math;

        api_field_data = (lt_entry->field_alloc_cb)();
        SHR_NULL_CHECK(api_field_data, SHR_E_MEMORY);

        /* Add previously constructed API field value to entry */
        sal_memcpy(api_field_data, wb_field_dest,
                   sizeof(bcmltm_field_list_t));

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
bcmltm_fa_node_api_wb_to_out_key_fields(int unit,
                                        bcmltm_lt_md_t *lt_md,
                                        bcmltm_lt_state_t *lt_state,
                                        bcmltm_entry_t *lt_entry,
                                        uint32_t *ltm_buffer,
                                        void *node_cookie)
{
    uint32_t fix, field_count;
    bcmltm_field_map_list_t *field_map_list;
    bcmltm_field_map_t *field_maps, *cur_field_map;
    uint32_t *ptr_math;
    bcmltm_field_list_t *api_field_data, *last_out_field, *wb_field_dest;
    uint32_t map_index;
    bcmltm_field_select_mapping_t *fs_map;

    SHR_FUNC_ENTER(unit);

    fs_map = BCMLTM_FA_FS_MAPPING(node_cookie);

    SHR_IF_ERR_EXIT
        (bcmltm_fa_select_map_index(unit, ltm_buffer,
                                    fs_map, &map_index));

    if (map_index == 0) {
        /* This field list is not selected. */
        SHR_EXIT();
    }

    field_map_list =
        BCMLTM_FA_SELECT_FIELD_MAP_LIST(node_cookie, map_index - 1);

    field_count = field_map_list->num_fields;
    field_maps = field_map_list->field_maps;
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
        cur_field_map = &(field_maps[fix]);

        ptr_math = ltm_buffer + cur_field_map->apic_offset;
        wb_field_dest = (bcmltm_field_list_t *)ptr_math;

        api_field_data = (lt_entry->field_alloc_cb)();
        SHR_NULL_CHECK(api_field_data, SHR_E_MEMORY);

        /* Add previously constructed API field value to entry */
        sal_memcpy(api_field_data, wb_field_dest,
                   sizeof(bcmltm_field_list_t));

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
bcmltm_fa_node_select_fixed_fields_to_wb(int unit,
                                         bcmltm_lt_md_t *lt_md,
                                         bcmltm_lt_state_t *lt_state,
                                         bcmltm_entry_t *lt_entry,
                                         uint32_t *ltm_buffer,
                                         void *node_cookie)
{
    uint32_t fix, field_count;
    bcmltm_fixed_field_t *cur_fixed, *field_list;
    bcmltm_fixed_field_list_t *fixed_field_data;
    bcmltm_fixed_field_mapping_t *fixed_map;
    uint32_t map_index;

    SHR_FUNC_ENTER(unit);

    fixed_map = BCMLTM_FA_FIXED_FIELD_MAPPING(node_cookie);

    SHR_IF_ERR_EXIT
        (bcmltm_fa_select_fixed_map_index(unit, ltm_buffer,
                                          fixed_map, &map_index));

    if (map_index == 0) {
        /* This field list is not selected. */
        SHR_EXIT();
    }

    fixed_field_data =
        BCMLTM_FA_FIXED_FIELD_MAP_LIST(node_cookie, map_index - 1);
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

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_delete_select_fixed_fields_to_wb(int unit,
                                                bcmltm_lt_md_t *lt_md,
                                                bcmltm_lt_state_t *lt_state,
                                                bcmltm_entry_t *lt_entry,
                                                uint32_t *ltm_buffer,
                                                void *node_cookie)
{
    uint32_t fix, field_count;
    bcmltm_fixed_field_t *cur_fixed, *field_list;
    bcmltm_fixed_field_list_t *fixed_field_data;
    bcmltm_fixed_field_mapping_t *fixed_map;
    uint32_t map_index;

    SHR_FUNC_ENTER(unit);

    fixed_map = BCMLTM_FA_FIXED_FIELD_MAPPING(node_cookie);

    SHR_IF_ERR_EXIT
        (bcmltm_fa_select_fixed_map_index(unit, ltm_buffer,
                                          fixed_map, &map_index));

    if (map_index == 0) {
        /* This field list is not selected. */
        SHR_EXIT();
    }

    fixed_field_data =
        BCMLTM_FA_FIXED_FIELD_MAP_LIST(node_cookie, map_index - 1);
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

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_api_wb_to_wbc(int unit,
                             bcmltm_lt_md_t *lt_md,
                             bcmltm_lt_state_t *lt_state,
                             bcmltm_entry_t *lt_entry,
                             uint32_t *ltm_buffer,
                             void *node_cookie)
{
    bcmltm_field_map_list_t *field_map_list;
    uint32_t map_index;
    bcmltm_field_select_mapping_t *fs_map;

    SHR_FUNC_ENTER(unit);

    fs_map = BCMLTM_FA_FS_MAPPING(node_cookie);

    SHR_IF_ERR_EXIT
        (bcmltm_fa_select_map_index(unit, ltm_buffer,
                                    fs_map, &map_index));

    if (map_index == 0) {
        /* This field list is not selected. */
        SHR_EXIT();
    }

    field_map_list =
        BCMLTM_FA_SELECT_FIELD_MAP_LIST(node_cookie, map_index - 1);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmltm_fa_node_api_wb_to_wbc_core(unit, field_map_list,
                                           ltm_buffer));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_alloc_first_api_wb_to_wbc(int unit,
                                         bcmltm_lt_md_t *lt_md,
                                         bcmltm_lt_state_t *lt_state,
                                         bcmltm_entry_t *lt_entry,
                                         uint32_t *ltm_buffer,
                                         void *node_cookie)
{
    bcmltm_field_map_list_t *field_map_list;
    uint32_t map_index;
    bcmltm_field_select_mapping_t *fs_map;

    SHR_FUNC_ENTER(unit);

    fs_map = BCMLTM_FA_FS_MAPPING(node_cookie);

    SHR_IF_ERR_EXIT
        (bcmltm_fa_select_map_index(unit, ltm_buffer,
                                    fs_map, &map_index));

    if (map_index == 0) {
        /* This field list is not selected. */
        SHR_EXIT();
    }

    field_map_list =
        BCMLTM_FA_SELECT_FIELD_MAP_LIST(node_cookie, map_index - 1);

    if ((field_map_list->index_absent_offset != BCMLTM_WB_OFFSET_INVALID) &&
        (ltm_buffer[field_map_list->index_absent_offset] ==
         BCMLTM_INDEX_ABSENT)) {
        /* Allocation required, do not copy missing key fields. */
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmltm_fa_node_api_wb_to_wbc_core(unit, field_map_list,
                                           ltm_buffer));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_alloc_second_api_wb_to_wbc(int unit,
                                          bcmltm_lt_md_t *lt_md,
                                          bcmltm_lt_state_t *lt_state,
                                          bcmltm_entry_t *lt_entry,
                                          uint32_t *ltm_buffer,
                                          void *node_cookie)
{
    bcmltm_field_map_list_t *field_map_list;
    uint32_t map_index;
    bcmltm_field_select_mapping_t *fs_map;

    SHR_FUNC_ENTER(unit);

    fs_map = BCMLTM_FA_FS_MAPPING(node_cookie);

    SHR_IF_ERR_EXIT
        (bcmltm_fa_select_map_index(unit, ltm_buffer,
                                    fs_map, &map_index));

    if (map_index == 0) {
        /* This field list is not selected. */
        SHR_EXIT();
    }

    field_map_list =
        BCMLTM_FA_SELECT_FIELD_MAP_LIST(node_cookie, map_index - 1);

    if ((field_map_list->index_absent_offset != BCMLTM_WB_OFFSET_INVALID) &&
        (ltm_buffer[field_map_list->index_absent_offset] !=
         BCMLTM_INDEX_ABSENT)) {
        /* No allocation occured, do not copy key fields
         * already copied on first pass. */
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmltm_fa_node_api_wb_to_wbc_core(unit, field_map_list,
                                           ltm_buffer));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_key_traverse_api_wb_to_wbc(int unit,
                                          bcmltm_lt_md_t *lt_md,
                                          bcmltm_lt_state_t *lt_state,
                                          bcmltm_entry_t *lt_entry,
                                          uint32_t *ltm_buffer,
                                          void *node_cookie)
{
    bcmltm_field_map_list_t *field_map_list;
    uint32_t map_index;
    bcmltm_field_select_mapping_t *fs_map;

    SHR_FUNC_ENTER(unit);

    if (lt_entry->in_fields == NULL) { /* TRAVERSE start */
        /* These key fields are not supplied. */
        SHR_EXIT();
    }

    fs_map = BCMLTM_FA_FS_MAPPING(node_cookie);

    SHR_IF_ERR_EXIT
        (bcmltm_fa_select_map_index(unit, ltm_buffer,
                                    fs_map, &map_index));

    if (map_index == 0) {
        /* This field list is not selected. */
        SHR_EXIT();
    }

    field_map_list =
        BCMLTM_FA_SELECT_FIELD_MAP_LIST(node_cookie, map_index - 1);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmltm_fa_node_api_wb_to_wbc_core(unit, field_map_list,
                                           ltm_buffer));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_wbc_to_api_wb(int unit,
                             bcmltm_lt_md_t *lt_md,
                             bcmltm_lt_state_t *lt_state,
                             bcmltm_entry_t *lt_entry,
                             uint32_t *ltm_buffer,
                             void *node_cookie)
{
    uint32_t fix, field_count;
    bcmltm_field_map_list_t *field_map_list;
    bcmltm_field_map_t *field_maps, *cur_field_map;
    uint32_t map_index;
    bcmltm_field_select_mapping_t *fs_map;
    uint32_t pts_offset;

    SHR_FUNC_ENTER(unit);

    fs_map = BCMLTM_FA_FS_MAPPING(node_cookie);

    pts_offset = fs_map->pt_suppress_offset;
    if (pts_offset != BCMLTM_WB_OFFSET_INVALID) {
        if (ltm_buffer[pts_offset] != 0) {
            /*
             * PT ops suppressed, no fields to copy.
             */
            SHR_EXIT();
        }
    }

    SHR_IF_ERR_EXIT
        (bcmltm_fa_select_map_index(unit, ltm_buffer,
                                    fs_map, &map_index));

    if (map_index == 0) {
        /* This field list is not selected. */
        SHR_EXIT();
    }

    field_map_list =
        BCMLTM_FA_SELECT_FIELD_MAP_LIST(node_cookie, map_index - 1);
    if ((field_map_list->index_absent_offset != BCMLTM_WB_OFFSET_INVALID) &&
        (ltm_buffer[field_map_list->index_absent_offset] !=
         BCMLTM_INDEX_ABSENT)) {
        /*
         * No allocation took place, so no need to copy allocated
         * key fields.
         */
        SHR_EXIT();
    }
    field_count = field_map_list->num_fields;
    field_maps = field_map_list->field_maps;
    for (fix = 0; fix < field_count; fix++) {
        cur_field_map = &(field_maps[fix]);

        /*
         * Unlike API WB to WBC, only the first map for this
         * specific API field is examined.
         */
        
        if (cur_field_map != NULL) {
            bcmltm_field_map_from_buffer(cur_field_map, ltm_buffer);
        }
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_select_field_map(int unit,
                                bcmltm_lt_md_t *lt_md,
                                bcmltm_lt_state_t *lt_state,
                                bcmltm_entry_t *lt_entry,
                                uint32_t *ltm_buffer,
                                void *node_cookie)
{
    bcmltm_field_select_maps_t *fsm;
    bcmltm_select_map_t *cur_map;
    uint32_t map_index, sfv;
    uint32_t mix;
    uint32_t *ptr_math;
    bcmltm_field_list_t *api_field_data;

    SHR_FUNC_ENTER(unit);

    fsm = BCMLTM_FA_SELECT_FIELD_MAP(node_cookie);

    if (fsm->parent_select_offset != BCMLTM_WB_OFFSET_INVALID) {
        if (ltm_buffer[fsm->parent_select_offset] !=
            fsm->parent_select_map_index) {
            /* This selection is not active.  Return without error. */
            /* Clear this map index in case UPDATE read set it. */
            ltm_buffer[fsm->map_index_offset] = 0;
            SHR_EXIT();
        }
    } /* Else top level selector field, unconditional */

    if (fsm->select_field_offset != BCMLTM_WB_OFFSET_INVALID) {
        ptr_math = ltm_buffer + fsm->select_field_offset;
        api_field_data = (bcmltm_field_list_t *)ptr_math;
        sfv = api_field_data->data;
    } else if (fsm->pt_subentry_offset != BCMLTM_WB_OFFSET_INVALID) {
        sfv = ltm_buffer[fsm->pt_subentry_offset];
    } else {
        /* One of the offsets must be valid. */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    for (mix = 0; mix < fsm->num_maps; mix++) {
        cur_map = &(fsm->select_maps[mix]);

        if (sfv == cur_map->selector_field_value) {
            map_index = cur_map->map_index;
            break;
        }
    }

    if (mix >= fsm->num_maps) {
        /* Select field value not mapped, no field selection */
        map_index = 0;
    }

    ltm_buffer[fsm->map_index_offset] = map_index;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_select_field_unmap(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie)
{
    bcmltm_field_select_maps_t *fsm;
    bcmltm_select_map_t *cur_map;
    uint32_t map_index, sfv;
    uint32_t mix;
    uint32_t *ptr_math;
    bcmltm_field_list_t *api_field_data;

    SHR_FUNC_ENTER(unit);

    fsm = BCMLTM_FA_SELECT_FIELD_MAP(node_cookie);

    if (fsm->parent_select_offset != BCMLTM_WB_OFFSET_INVALID) {
        /* This must be a top-level key map. */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    map_index = ltm_buffer[fsm->map_index_offset];

    for (mix = 0; mix < fsm->num_maps; mix++) {
        cur_map = &(fsm->select_maps[mix]);

        if (map_index == cur_map->map_index) {
            sfv = cur_map->selector_field_value;
            break;
        }
    }

    if (mix >= fsm->num_maps) {
        /*
         * Select map_index not valid.
         * Must be valid because this is a key map.
         */
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (fsm->select_field_offset != BCMLTM_WB_OFFSET_INVALID) {
        ptr_math = ltm_buffer + fsm->select_field_offset;
        api_field_data = (bcmltm_field_list_t *)ptr_math;
        api_field_data->data = sfv;
    } else if (fsm->pt_subentry_offset != BCMLTM_WB_OFFSET_INVALID) {
        ltm_buffer[fsm->pt_subentry_offset] = sfv;
    } else {
        /* One of the offsets must be valid. */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_api_wb_defaults(int unit,
                               bcmltm_lt_md_t *lt_md,
                               bcmltm_lt_state_t *lt_state,
                               bcmltm_entry_t *lt_entry,
                               uint32_t *ltm_buffer,
                               void *node_cookie)
{
    uint32_t fix, field_count;
    bcmltm_field_map_list_t *field_map_list;
    bcmltm_field_map_t *field_maps, *cur_field_map;
    uint32_t *ptr_math;
    bcmltm_field_list_t *wb_field_dest;
    uint32_t map_index;
    bcmltm_field_select_mapping_t *fs_map;

    SHR_FUNC_ENTER(unit);

    fs_map = BCMLTM_FA_FS_MAPPING(node_cookie);

    SHR_IF_ERR_EXIT
        (bcmltm_fa_select_map_index(unit, ltm_buffer,
                                    fs_map, &map_index));

    if (map_index == 0) {
        /* This field list is not selected. */
        SHR_EXIT();
    }

    field_map_list =
        BCMLTM_FA_SELECT_FIELD_MAP_LIST(node_cookie, map_index - 1);

    field_count = field_map_list->num_fields;
    field_maps = field_map_list->field_maps;

    for (fix = 0; fix < field_count; fix++) {
        cur_field_map = &(field_maps[fix]);

        ptr_math = ltm_buffer + cur_field_map->apic_offset;
        wb_field_dest = (bcmltm_field_list_t *)ptr_math;

        wb_field_dest->id = cur_field_map->field_id;
        wb_field_dest->idx = cur_field_map->field_idx;
        wb_field_dest->data = cur_field_map->default_value;
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_pt_clear(int unit,
                        bcmltm_lt_md_t *lt_md,
                        bcmltm_lt_state_t *lt_state,
                        bcmltm_entry_t *lt_entry,
                        uint32_t *ltm_buffer,
                        void *node_cookie)
{
    
    return SHR_E_NONE;
}
