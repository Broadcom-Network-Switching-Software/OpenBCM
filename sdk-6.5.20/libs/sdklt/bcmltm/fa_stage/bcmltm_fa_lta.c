/*! \file bcmltm_fa_lta.c
 *
 * Logical Table Manager Field Adaptation
 *
 * Logical Table Adaptors
 *
 * This file contains the LTM FA stage functions for LTA custom table
 * handlers.  The Logical Table Adaptor interface for custom tables
 * allows the implementation of a LT to be controlled solely through
 * custom logic.  This is intended for cases where the standard LTM
 * processing is too generic for the special case of this LT.
 *
 * The LTA interface provides a standardized method of passing the
 * LT fields and other necessary data to the custom logic.  The custom
 * table implementation is responsible for analyzing the fields,
 * producing the necessary PTM operations, and extracting any
 * return fields.
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
 * \brief Comparison function for LTA metadata binary search.
 *
 * The LTA metadata arrays, ordered by (fid, fidx), are searched
 * in the bcmltm_fa_node_api_fields_cth_to_lta_expedited function.
 * The binary search algorithm requires this comparison function.
 *
 * \param [in] match_spec_p Pointer to field spec to match (fid, fidx).
 * \param [in] lta_map_p Pointer to LTA field map metadata to compare.
 */
static int
lta_field_spec_compare(const void *match_spec_p,
                       const void *lta_map_p)
{
    const bcmltm_field_spec_t *match_spec = match_spec_p;
    const bcmltm_lta_field_map_t *lta_map = lta_map_p;

    if (match_spec->field_id < lta_map->api_field_id) {
        return -1;
    } else if (match_spec->field_id > lta_map->api_field_id) {
        return 1;
    } else if (match_spec->field_idx < lta_map->field_idx) {
        return -1;
    } else if (match_spec->field_idx > lta_map->field_idx) {
        return 1;
    } else {
        return 0;
    }
}

/*!
 * \brief Log Verbose for API-facing LTA input list.
 *
 * This routine performs a log verbose and displays
 * the API facing fields for the specified LTA field list.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] lta_field_list LTA field list metadata.
 */
static void
api_lta_field_list_log_verbose(int unit,
                               uint32_t sid,
                               const uint32_t *ltm_buffer,
                               const bcmltm_lta_field_list_t *lta_field_list)
{
    const uint32_t *ptr_math;
    const bcmltd_fields_t *api_fields;
    const bcmltd_field_t *field;
    const char *field_name;
    uint32_t field_count;
    uint32_t fix;

    /* If API-facing field list is not available, just return */
    if (lta_field_list->api_fields_parameter_offset ==
        BCMLTM_WB_OFFSET_INVALID) {
        return;
    }

    ptr_math = ltm_buffer + lta_field_list->api_fields_parameter_offset;
    api_fields = (bcmltd_fields_t *)ptr_math;
    field_count = api_fields->count;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "API LTA Input Fields: count=%d\n"),
                 field_count));

    for (fix = 0; fix < field_count; fix++) {
        field = api_fields->field[fix];

        field_name = bcmltm_lt_field_fid_to_name(unit, sid, field->id);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "  %s[%d](fid=%d) "
                                "data=0x%" PRIx64 " flags=0x%x\n"),
                     field_name,
                     field->idx,
                     field->id,
                     field->data,
                     field->flags));
    }

    return;
}

/*!
 * \brief Initialize a LTA field arrays within the Working Buffer.
 *
 * This function prepares the space assigned withing the Working Buffer
 * to manage LTA input or output field lists.
 * When the output argument is true, the field structures are cleared.
 * Input arguments expect the field structures to be initialized
 * by some other operation.
 *
 * \param [in] unit Unit number.
 * \param [in] output When TRUE, clear the field structures for a LTA
 *             output lists.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmltm_fa_node_lta_field_list_init(int unit,
                                   bool output,
                                   uint32_t *ltm_buffer,
                                   void *node_cookie)
{
    int rv = SHR_E_NONE;
    uint32_t fix, field_count, max_fields;
    bcmltm_lta_field_list_t *lta_field_list;
    bcmltd_field_t **lta_fields_ptrs, *cur_lta_field;
    bcmltd_fields_t *lta_fields_parameter;
    bcmltm_lta_field_map_t *lta_field_maps, *cur_lta_map;
    uint32_t *ptr_math;

    lta_field_list = BCMLTM_FA_LTA_FIELD_LIST(node_cookie);

    ptr_math = ltm_buffer + lta_field_list->fields_parameter_offset;
    lta_fields_parameter = (bcmltd_fields_t *)ptr_math;
    ptr_math = ltm_buffer + lta_field_list->fields_ptrs_offset;
    lta_fields_ptrs = (bcmltd_field_t **)ptr_math;
    lta_field_maps = lta_field_list->field_maps;

    lta_fields_parameter->count = 0;
    lta_fields_parameter->field = lta_fields_ptrs;

    if (output) {
        /* Initialize field structures for output field values. */
        field_count = 0;
        max_fields = lta_field_list->max_fields;
        for (fix = 0; fix < max_fields; fix++) {
            cur_lta_map = &(lta_field_maps[fix]);
            ptr_math = ltm_buffer + cur_lta_map->field_offset;
            cur_lta_field = (bcmltd_field_t *)ptr_math;

            /*
             * Initialize the field structure
             */
            sal_memset(cur_lta_field, 0,
                       sizeof(bcmltd_field_t));

            lta_fields_ptrs[field_count] = cur_lta_field;
            field_count++;
        }
        lta_fields_parameter->count = field_count;
    }

    return rv;
}

/*!
 * \brief An FA tree node to convert API fields to LTA field arrays,
 *        with an option to append default field values.
 *
 * An FA tree node which converts the API fields to an array of
 * Logical Table Adapter fields.  Expected fields are required when
 * corresponding to key fields.  Otherwise, when the fields_default
 * parameter is true and the API field is not present in the input list,
 * the default value is added to the LTA list.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 * \param [in] fields_required Indicates the mapped fields existence
 *             requirement choice.  One of BCMLTM_FIELDS_REQUIRED_*.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmltm_fa_node_api_fields_to_lta_core(int unit,
                                      bcmltm_lt_md_t *lt_md,
                                      bcmltm_lt_state_t *lt_state,
                                      bcmltm_entry_t *lt_entry,
                                      uint32_t *ltm_buffer,
                                      void *node_cookie,
                                      bcmltm_fields_required_t fields_required)
{
    uint32_t fix, field_count, found_count, map_count, ltid;
    bcmltm_lta_field_list_t *lta_field_list;
    bcmltd_field_t **lta_fields_ptrs, *cur_lta_field;
    bcmltd_fields_t *lta_fields_parameter;
    bcmltm_lta_field_map_t *lta_field_maps, *cur_lta_map;
    uint32_t *ptr_math;
    bcmltm_field_list_t *api_field_data;

    SHR_FUNC_ENTER(unit);

    ltid = lt_md->params->ltid;

    lta_field_list = BCMLTM_FA_LTA_FIELD_LIST(node_cookie);

    ptr_math = ltm_buffer + lta_field_list->fields_parameter_offset;
    lta_fields_parameter = (bcmltd_fields_t *)ptr_math;
    lta_fields_ptrs = lta_fields_parameter->field;
    lta_field_maps = lta_field_list->field_maps;

    map_count = lta_field_list->num_maps;
    /* sal_assert(map_count <= lta_field_list->max_fields); */

    field_count = lta_fields_parameter->count;
    found_count = 0;

    /*
     * Do not check for the PT suppress status.
     *
     * The intention is to always process the API input fields
     * to detect for input errors, even if the forward
     * transform may be later skipped.
     */

    for (fix = 0; fix < map_count; fix++) {
        cur_lta_map = &(lta_field_maps[fix]);
        ptr_math = ltm_buffer + cur_lta_map->field_offset;
        cur_lta_field = (bcmltd_field_t *)ptr_math;

        api_field_data = bcmltm_api_find_field(lt_entry->in_fields,
                                               cur_lta_map->api_field_id,
                                               cur_lta_map->field_idx);
        if (api_field_data == NULL) {
            if (fields_required == BCMLTM_FIELDS_REQUIRED_ALL) {
                /* LTA fields are required */
                BCMLTM_LOG_VERBOSE_FIELD_MISSING(unit,
                                                 ltid,
                                                 cur_lta_map->api_field_id,
                                                 cur_lta_map->field_idx,
                                                 TRUE);

                bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                                       BCMLRD_FIELD_FIELD_LIST_ERROR_COUNT);
                SHR_ERR_EXIT(SHR_E_PARAM);
            } else if (fields_required == BCMLTM_FIELDS_REQUIRED_DEFAULT) {
                /* Missing LTA fields are supplied */
                sal_memset(cur_lta_field, 0, sizeof(bcmltd_field_t));
                cur_lta_field->id = cur_lta_map->api_field_id;
                cur_lta_field->idx = cur_lta_map->field_idx;
                cur_lta_field->data = cur_lta_map->default_value;
            } else {
                /* LTA fields are optional */
                continue;
            }
        } else {
            /*
             * LTA field format is the same as the LTM field format.
             * We just need to copy from the API input list to
             * the WB location for the input field list.
             */
            sal_memcpy(cur_lta_field, api_field_data,
                       sizeof(bcmltd_field_t));

            if ((cur_lta_field->flags & SHR_FMM_FIELD_DEL) != 0) {
                /* Field unset, supply default for CTH but retain flag */
                cur_lta_field->data = cur_lta_map->default_value;
            } else {
                /*
                 * If field not marked for unset,
                 * check that the supplied field value is legal.
                 */
                if ((cur_lta_field->data < cur_lta_map->minimum_value) ||
                    (cur_lta_field->data > cur_lta_map->maximum_value)) {
                    /* Out of bounds field value */
                    BCMLTM_LOG_VERBOSE_FIELD_OOB(unit,
                                                 ltid,
                                                 cur_lta_map->api_field_id,
                                                 cur_lta_map->field_idx,
                                                 TRUE,
                                                 cur_lta_field->data,
                                                 cur_lta_map->minimum_value,
                                                 cur_lta_map->maximum_value);

                    bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                                           BCMLRD_FIELD_FIELD_SIZE_ERROR_COUNT);
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
            }
        }

        lta_fields_ptrs[field_count] = cur_lta_field;
        field_count++;
        found_count++;
    }

    if (fields_required == BCMLTM_FIELDS_REQUIRED_ALL_OR_NONE) {
        if (found_count == 0) {
            /* Record absence of fields in Working Buffer. */
            ltm_buffer[lta_field_list->index_absent_offset] =
                BCMLTM_INDEX_ABSENT;
        } else if (found_count != map_count) {
            const char *table_name;

            table_name = bcmltm_lt_table_sid_to_name(unit, ltid);
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "LTM %s(ltid=%d): "
                                    "Incorrect number of key fields\n"),
                         table_name, ltid));

            bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                                   BCMLRD_FIELD_FIELD_LIST_ERROR_COUNT);
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        /* Else, standard all key fields present */
    }
    lta_fields_parameter->count = field_count;

 exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

int
bcmltm_fa_node_default_fields_to_lta(int unit,
                                     bcmltm_lt_md_t *lt_md,
                                     bcmltm_lt_state_t *lt_state,
                                     bcmltm_entry_t *lt_entry,
                                     uint32_t *ltm_buffer,
                                     void *node_cookie)
{
    int rv = SHR_E_NONE;
    uint32_t fix, field_count, map_count;
    bcmltm_lta_field_list_t *lta_field_list;
    bcmltd_field_t **lta_fields_ptrs, *cur_lta_field;
    bcmltd_fields_t *lta_fields_parameter;
    bcmltm_lta_field_map_t *lta_field_maps, *cur_lta_map;
    uint32_t *ptr_math;

    lta_field_list = BCMLTM_FA_LTA_FIELD_LIST(node_cookie);

    ptr_math = ltm_buffer + lta_field_list->fields_parameter_offset;
    lta_fields_parameter = (bcmltd_fields_t *)ptr_math;
    ptr_math = ltm_buffer + lta_field_list->fields_ptrs_offset;
    lta_fields_ptrs = (bcmltd_field_t **)ptr_math;
    lta_field_maps = lta_field_list->field_maps;

    map_count = lta_field_list->num_maps;
    /* sal_assert(map_count <= lta_field_list->max_fields); */
    field_count = lta_fields_parameter->count;

    for (fix = 0; fix < map_count; fix++) {
        cur_lta_map = &(lta_field_maps[fix]);
        ptr_math = ltm_buffer + cur_lta_map->field_offset;
        cur_lta_field = (bcmltd_field_t *)ptr_math;

        /* LTA fields defaults */
        sal_memset(cur_lta_field, 0, sizeof(bcmltd_field_t));
        cur_lta_field->id = cur_lta_map->api_field_id;
        cur_lta_field->idx = cur_lta_map->field_idx;
        cur_lta_field->data = cur_lta_map->default_value;

        lta_fields_ptrs[field_count] = cur_lta_field;
        field_count++;
    }

    lta_fields_parameter->count = field_count;
    lta_fields_parameter->field = lta_fields_ptrs;

    return rv;
}


extern int
bcmltm_fa_node_api_key_fields_to_lta(int unit,
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

    return bcmltm_fa_node_api_fields_to_lta_core(unit,
                                                 lt_md,
                                                 lt_state,
                                                 lt_entry,
                                                 ltm_buffer,
                                                 node_cookie,
                                                 keys_required);
}

extern int
bcmltm_fa_node_api_alloc_key_fields_to_lta(int unit,
                                           bcmltm_lt_md_t *lt_md,
                                           bcmltm_lt_state_t *lt_state,
                                           bcmltm_entry_t *lt_entry,
                                           uint32_t *ltm_buffer,
                                           void *node_cookie)
{
    return bcmltm_fa_node_api_fields_to_lta_core(unit,
                                                 lt_md,
                                                 lt_state,
                                                 lt_entry,
                                                 ltm_buffer,
                                                 node_cookie,
                                      BCMLTM_FIELDS_REQUIRED_ALL_OR_NONE);
}

extern int
bcmltm_fa_node_api_value_fields_to_lta(int unit,
                                       bcmltm_lt_md_t *lt_md,
                                       bcmltm_lt_state_t *lt_state,
                                       bcmltm_entry_t *lt_entry,
                                       uint32_t *ltm_buffer,
                                       void *node_cookie)
{
    return bcmltm_fa_node_api_fields_to_lta_core(unit,
                                                 lt_md,
                                                 lt_state,
                                                 lt_entry,
                                                 ltm_buffer,
                                                 node_cookie,
                                                 BCMLTM_FIELDS_REQUIRED_NONE);
}

extern int
bcmltm_fa_node_api_fields_default_to_lta(int unit,
                                         bcmltm_lt_md_t *lt_md,
                                         bcmltm_lt_state_t *lt_state,
                                         bcmltm_entry_t *lt_entry,
                                         uint32_t *ltm_buffer,
                                         void *node_cookie)
{
    return bcmltm_fa_node_api_fields_to_lta_core(unit,
                                                 lt_md,
                                                 lt_state,
                                                 lt_entry,
                                                 ltm_buffer,
                                                 node_cookie,
                                      BCMLTM_FIELDS_REQUIRED_DEFAULT);
}

extern int
bcmltm_fa_node_api_fields_update_to_lta(int unit,
                                        bcmltm_lt_md_t *lt_md,
                                        bcmltm_lt_state_t *lt_state,
                                        bcmltm_entry_t *lt_entry,
                                        uint32_t *ltm_buffer,
                                        void *node_cookie)
{
    uint32_t fix, field_count, map_count, ltid;
    bcmltm_lta_field_list_t *lta_field_list;
    bcmltd_field_t **lta_fields_ptrs, *cur_lta_field;
    bcmltd_fields_t *lta_fields_parameter;
    bcmltm_lta_field_map_t *lta_field_maps, *cur_lta_map;
    uint32_t *ptr_math;
    bcmltm_field_list_t *api_field_data;

    SHR_FUNC_ENTER(unit);

    lta_field_list = BCMLTM_FA_LTA_FIELD_LIST(node_cookie);

    ltid = lt_md->params->ltid;

    ptr_math = ltm_buffer + lta_field_list->fields_parameter_offset;
    lta_fields_parameter = (bcmltd_fields_t *)ptr_math;
    lta_fields_ptrs = lta_fields_parameter->field;
    lta_field_maps = lta_field_list->field_maps;

    map_count = lta_field_list->num_maps;
    /* sal_assert(map_count <= lta_field_list->max_fields); */

    field_count = lta_fields_parameter->count;

    for (fix = 0; fix < map_count; fix++) {
        cur_lta_map = &(lta_field_maps[fix]);

        api_field_data = bcmltm_api_find_field(lt_entry->in_fields,
                                               cur_lta_map->api_field_id,
                                               cur_lta_map->field_idx);
        if (api_field_data == NULL) {
            /* LTA fields are optional */
            continue;
        }

        cur_lta_field = bcmltm_lta_find_field(lta_fields_parameter,
                                              cur_lta_map->api_field_id,
                                              cur_lta_map->field_idx);
        if (cur_lta_field == NULL) { /* Missing expected field */
            
            ptr_math = ltm_buffer + cur_lta_map->field_offset;
            cur_lta_field = (bcmltd_field_t *)ptr_math;

            lta_fields_ptrs[field_count] = cur_lta_field;
            field_count++;
        }

        /*
         * LTA field format is the same as the LTM field format.
         * We just need to copy from the API input to the allocated
         * input field list.
         */
        sal_memcpy(cur_lta_field, api_field_data,
                   sizeof(bcmltd_field_t));

        /*
         * Field in API input list, and in the LTA list produced
         * as output of the read during the first step of UPDATE.
         * Modify values per API request.
         */
        if ((api_field_data->flags & SHR_FMM_FIELD_DEL) == 0) {
            /* First, check that the supplied field value is legal. */
            if ((api_field_data->data < cur_lta_map->minimum_value) ||
                (api_field_data->data > cur_lta_map->maximum_value)) {
                /* Out of bounds field value */
                BCMLTM_LOG_VERBOSE_FIELD_OOB(unit,
                                             ltid,
                                             cur_lta_map->api_field_id,
                                             cur_lta_map->field_idx,
                                             TRUE,
                                             api_field_data->data,
                                             cur_lta_map->minimum_value,
                                             cur_lta_map->maximum_value);

                bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                                       BCMLRD_FIELD_FIELD_SIZE_ERROR_COUNT);
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        } else {
            /* Unset request: revert LTA field to field default value. */
            cur_lta_field->data = cur_lta_map->default_value;
        }

        /* No update to the field pointers in the LTA list because
         * this field was already present in the list when found above. */
    }

    lta_fields_parameter->count = field_count;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_api_fields_cth_to_lta_expedited(int unit,
                                               bcmltm_lt_md_t *lt_md,
                                               bcmltm_lt_state_t *lt_state,
                                               bcmltm_entry_t *lt_entry,
                                               uint32_t *ltm_buffer,
                                               void *node_cookie)
{
    uint32_t field_count, map_count;
    bcmltm_lta_field_list_t *lta_field_list;
    bcmltd_field_t **lta_fields_ptrs, *cur_lta_field;
    bcmltd_fields_t *lta_fields_parameter;
    bcmltm_lta_field_map_t *lta_field_maps, *cur_lta_map;
    bcmltm_field_spec_t match_spec;
    uint32_t *ptr_math;
    bcmltm_field_list_t *api_field_data;

    SHR_FUNC_ENTER(unit);

    lta_field_list = BCMLTM_FA_LTA_FIELD_LIST(node_cookie);

    ptr_math = ltm_buffer + lta_field_list->fields_parameter_offset;
    lta_fields_parameter = (bcmltd_fields_t *)ptr_math;
    lta_fields_ptrs = lta_fields_parameter->field;

    lta_field_maps = lta_field_list->field_maps;
    map_count = lta_field_list->num_maps;

    field_count = lta_fields_parameter->count;

    api_field_data = lt_entry->in_fields;

    while (api_field_data != NULL) {
        
        if ((api_field_data->flags & SHR_FMM_FIELD_DEL) != 0) {
            match_spec.field_id = api_field_data->id;
            match_spec.field_idx = api_field_data->idx;

            /* Binary search of metadata, which is ordered by (fid, fidx) */
            cur_lta_map = sal_bsearch(&match_spec,
                                      lta_field_maps, map_count,
                                      sizeof(*cur_lta_map),
                                      lta_field_spec_compare);

            if (cur_lta_map == NULL) {
                /*
                 * API field not expected.  May be an error some day,
                 * but now for compatibility with other LTs skip.
                 */
                continue;
            }

            /* Use WB space for holding this field's information */
            ptr_math = ltm_buffer + cur_lta_map->field_offset;
            cur_lta_field = (bcmltd_field_t *)ptr_math;

            sal_memcpy(cur_lta_field, api_field_data,
                       sizeof(bcmltd_field_t));

            /* Unset request: revert LTA field to field default value. */
            cur_lta_field->data = cur_lta_map->default_value;
            lta_fields_ptrs[field_count] = cur_lta_field;
        } else {
            /* Use API field structure without copy. */
            lta_fields_ptrs[field_count] = api_field_data;
        }

        field_count++;
        if (field_count > lta_field_list->max_fields) {
            /* Out of LTA list space */
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        api_field_data = api_field_data->next;
    }

    lta_fields_parameter->count = field_count;

 exit:
    SHR_FUNC_EXIT();
}

extern int
bcmltm_fa_node_api_fields_cth_to_lta(int unit,
                                     bcmltm_lt_md_t *lt_md,
                                     bcmltm_lt_state_t *lt_state,
                                     bcmltm_entry_t *lt_entry,
                                     uint32_t *ltm_buffer,
                                     void *node_cookie)
{
    uint32_t fix, field_count, map_count, ltid;
    bcmltm_lta_field_list_t *lta_field_list;
    bcmltd_field_t **lta_fields_ptrs, *cur_lta_field;
    bcmltd_fields_t *lta_fields_parameter;
    bcmltm_lta_field_map_t *lta_field_maps, *cur_lta_map,
        *error_lta_map = NULL;
    uint32_t *ptr_math;
    bcmltm_field_list_t *api_field_data, *error_field_data = NULL;
    bool in_list;

    SHR_FUNC_ENTER(unit);

    lta_field_list = BCMLTM_FA_LTA_FIELD_LIST(node_cookie);

    ltid = lt_md->params->ltid;

    ptr_math = ltm_buffer + lta_field_list->fields_parameter_offset;
    lta_fields_parameter = (bcmltd_fields_t *)ptr_math;
    lta_fields_ptrs = lta_fields_parameter->field;
    lta_field_maps = lta_field_list->field_maps;

    map_count = lta_field_list->num_maps;
    /* sal_assert(map_count <= lta_field_list->max_fields); */

    field_count = lta_fields_parameter->count;

    for (fix = 0; fix < map_count; fix++) {
        cur_lta_map = &(lta_field_maps[fix]);

        api_field_data = bcmltm_api_find_field(lt_entry->in_fields,
                                               cur_lta_map->api_field_id,
                                               cur_lta_map->field_idx);
        if (api_field_data == NULL) {
            /* LTA fields are optional */
            continue;
        }

        if ((cur_lta_map->flags & BCMLTM_FIELD_READ_ONLY) != 0) {
            /*
             * Read-only value fields should not be in API input.
             * This error take precedence over OOB value errors below.
             */
            error_lta_map = cur_lta_map;
            SHR_ERR_EXIT(SHR_E_ACCESS);
        }

        in_list = FALSE;
        cur_lta_field = bcmltm_lta_find_field(lta_fields_parameter,
                                              cur_lta_map->api_field_id,
                                              cur_lta_map->field_idx);
        if (cur_lta_field != NULL) {
            
            in_list = TRUE;
            /* Field already in list. */
            if (cur_lta_field->data == api_field_data->data) {
                /* No change, do not update */
                continue;
            }
        }

        ptr_math = ltm_buffer + cur_lta_map->field_offset;
        cur_lta_field = (bcmltd_field_t *)ptr_math;

        sal_memcpy(cur_lta_field, api_field_data,
                   sizeof(bcmltd_field_t));

        if ((api_field_data->flags & SHR_FMM_FIELD_DEL) == 0) {
            /* Check that the supplied field value is legal. */
            if ((api_field_data->data < cur_lta_map->minimum_value) ||
                (api_field_data->data > cur_lta_map->maximum_value)) {
                if (error_lta_map == NULL) {
                    /* Only record first out of bounds error. */
                    error_lta_map = cur_lta_map;
                    error_field_data = api_field_data;
                    SHR_IF_ERR_CONT(SHR_E_PARAM);
                }
                continue;
            }
        } else {
            /* Unset request: revert LTA field to field default value. */
            cur_lta_field->data = cur_lta_map->default_value;
        }

        if (!in_list) {
            /* Field not in API input list, so append it. */
            lta_fields_ptrs[field_count] = cur_lta_field;
            field_count++;
        }
    }

    lta_fields_parameter->count = field_count;

 exit:
    if (SHR_FUNC_VAL_IS(SHR_E_ACCESS)){
        /* Supplied read-only write value */
        if (error_lta_map != NULL) {
            BCMLTM_LOG_VERBOSE_FIELD_RO_WRITE(unit,
                                              ltid,
                                              error_lta_map->api_field_id,
                                              error_lta_map->field_idx,
                                              TRUE);
        }
        bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                               BCMLRD_FIELD_FIELD_LIST_ERROR_COUNT);
    } else if (SHR_FUNC_VAL_IS(SHR_E_PARAM)){
        /* Out of bounds field value */
        if ((error_lta_map != NULL) && (error_field_data != NULL)) {
            BCMLTM_LOG_VERBOSE_FIELD_OOB(unit,
                                         ltid,
                                         error_lta_map->api_field_id,
                                         error_lta_map->field_idx,
                                         TRUE,
                                         error_field_data->data,
                                         error_lta_map->minimum_value,
                                         error_lta_map->maximum_value);
        }
        bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                               BCMLRD_FIELD_FIELD_SIZE_ERROR_COUNT);
    }
    SHR_FUNC_EXIT();
}

extern int
bcmltm_fa_node_lta_output_init(int unit,
                               bcmltm_lt_md_t *lt_md,
                               bcmltm_lt_state_t *lt_state,
                               bcmltm_entry_t *lt_entry,
                               uint32_t *ltm_buffer,
                               void *node_cookie)
{
    return bcmltm_fa_node_lta_field_list_init(unit, TRUE,
                                              ltm_buffer, node_cookie);
}

extern int
bcmltm_fa_node_lta_input_init(int unit,
                               bcmltm_lt_md_t *lt_md,
                               bcmltm_lt_state_t *lt_state,
                               bcmltm_entry_t *lt_entry,
                               uint32_t *ltm_buffer,
                               void *node_cookie)
{
    return bcmltm_fa_node_lta_field_list_init(unit, FALSE,
                                              ltm_buffer, node_cookie);
}

extern int
bcmltm_fa_node_lta_to_api_fields(int unit,
                                 bcmltm_lt_md_t *lt_md,
                                 bcmltm_lt_state_t *lt_state,
                                 bcmltm_entry_t *lt_entry,
                                 uint32_t *ltm_buffer,
                                 void *node_cookie)
{
    uint32_t fix, field_count, mix, map_count;
    bcmltm_lta_field_list_t *lta_field_list;
    bcmltd_field_t **lta_fields_ptrs;
    bcmltd_fields_t *lta_fields_parameter;
    bcmltm_field_list_t *api_field_data, *last_out_field;
    uint32_t *ptr_math;
    bcmltm_lta_field_map_t *lta_field_maps, *cur_lta_map;
    uint32_t pt_suppress_offset;
    bool pt_suppress = FALSE;

    SHR_FUNC_ENTER(unit);

    lta_field_list = BCMLTM_FA_LTA_FIELD_LIST(node_cookie);

    pt_suppress_offset = lta_field_list->pt_suppress_offset;
    if (pt_suppress_offset != BCMLTM_WB_OFFSET_INVALID) {
        if (ltm_buffer[pt_suppress_offset] != 0) {
            pt_suppress = TRUE;
        }
    }

    ptr_math = ltm_buffer + lta_field_list->fields_parameter_offset;
    lta_fields_parameter = (bcmltd_fields_t *)ptr_math;

    lta_field_maps = lta_field_list->field_maps;
    map_count = lta_field_list->num_maps;

    lta_fields_ptrs = lta_fields_parameter->field;
    field_count = lta_fields_parameter->count;

    /* Scan for existing output fields */
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

    /* Return default API values since transform was suppressed */
    if (pt_suppress) {
        if (lt_entry->attrib & BCMLT_ENT_ATTR_FILTER_DEFAULTS) {
            /* Nothing to do, exit without error */
            SHR_EXIT();
        }

        /* Copy field defaults into output */
        for (mix = 0; mix < map_count; mix++) {
            cur_lta_map = &(lta_field_maps[mix]);

            /* Make output slot */
            api_field_data = (lt_entry->field_alloc_cb)();
            SHR_NULL_CHECK(api_field_data, SHR_E_MEMORY);

            /* Copy field info from metadata */
            api_field_data->data = cur_lta_map->default_value;
            api_field_data->id = cur_lta_map->api_field_id;
            api_field_data->idx = cur_lta_map->field_idx;
            api_field_data->flags = 0; /* Flags only set by APP layer */
            api_field_data->next = NULL;

            /* Add retrieve field value to entry */
            if (last_out_field == NULL) {
                lt_entry->out_fields = api_field_data;
            } else {
                last_out_field->next = api_field_data;
            }
            last_out_field = api_field_data;
        }

        /* Done, no further processing */
        SHR_EXIT();
    }

    for (fix = 0; fix < field_count; fix++) {
        if (lt_entry->attrib & BCMLT_ENT_ATTR_FILTER_DEFAULTS) {
            cur_lta_map = NULL;

            
            for (mix = 0; mix < map_count; mix++) {
                if ((lta_field_maps[mix].api_field_id ==
                    lta_fields_ptrs[fix]->id) &&
                    (lta_field_maps[mix].field_idx ==
                     lta_fields_ptrs[fix]->idx)) {
                    cur_lta_map = &(lta_field_maps[mix]);
                    break;
                }
            }

            if (cur_lta_map == NULL) {
                /* Unknown field. */
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            if (((cur_lta_map->flags & BCMLTM_FIELD_KEY) == 0) &&
                (lta_fields_ptrs[fix]->data ==
                cur_lta_map->default_value)) {
                /* Do not add default value fields to output list */
                continue;
            }
        }

        api_field_data = (lt_entry->field_alloc_cb)();
        SHR_NULL_CHECK(api_field_data, SHR_E_MEMORY);

        /*
         * LTA field format is the same as the LTM field format.
         * We just need to copy from the WB location to the allocated
         * output field list.
         */
        sal_memcpy(api_field_data, lta_fields_ptrs[fix],
                   sizeof(bcmltd_field_t));

        /* Add retrieve field value to entry */
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

extern int
bcmltm_fa_node_lta_to_api_alloc_fields(int unit,
                                       bcmltm_lt_md_t *lt_md,
                                       bcmltm_lt_state_t *lt_state,
                                       bcmltm_entry_t *lt_entry,
                                       uint32_t *ltm_buffer,
                                       void *node_cookie)
{
    uint32_t fix, field_count;
    bcmltm_lta_field_list_t *lta_field_list;
    bcmltd_field_t **lta_fields_ptrs;
    bcmltd_fields_t *lta_fields_parameter;
    bcmltm_field_list_t *api_field_data, *last_out_field;
    uint32_t *ptr_math;

    SHR_FUNC_ENTER(unit);

    lta_field_list = BCMLTM_FA_LTA_FIELD_LIST(node_cookie);

    if (ltm_buffer[lta_field_list->index_absent_offset] !=
        BCMLTM_INDEX_ABSENT) {
        /*
         * No allocation took place, so no need to return
         * key fields mapped to index.
         */
        SHR_EXIT();
    }

    ptr_math = ltm_buffer + lta_field_list->fields_parameter_offset;
    lta_fields_parameter = (bcmltd_fields_t *)ptr_math;

    lta_fields_ptrs = lta_fields_parameter->field;
    field_count = lta_fields_parameter->count;

    /* Scan for existing output fields */
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
        api_field_data = (lt_entry->field_alloc_cb)();
        SHR_NULL_CHECK(api_field_data, SHR_E_MEMORY);

        /*
         * LTA field format is the same as the LTM field format.
         * We just need to copy from the WB location to the allocated
         * output field list.
         */
        sal_memcpy(api_field_data, lta_fields_ptrs[fix],
                   sizeof(bcmltd_field_t));

        /* Add retrieve field value to entry */
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

extern int
bcmltm_fa_node_lta_to_lta_fields(int unit,
                                 bcmltm_lt_md_t *lt_md,
                                 bcmltm_lt_state_t *lt_state,
                                 bcmltm_entry_t *lt_entry,
                                 uint32_t *ltm_buffer,
                                 void *node_cookie)
{
    uint32_t fix, field_count, map_count, max_fields;
    bcmltm_lta_field_list_t *lta_field_list;
    bcmltd_field_t **lta_fields_ptrs;
    bcmltd_fields_t *src_lta_fields_parameter, *lta_fields_parameter;
    bcmltm_field_list_t *lta_field;
    bcmltm_lta_field_map_t *lta_field_maps, *cur_lta_map;
    uint32_t *ptr_math;

    SHR_FUNC_ENTER(unit);

    lta_field_list = BCMLTM_FA_LTA_FIELD_LIST(node_cookie);

    ptr_math = ltm_buffer + lta_field_list->fields_parameter_offset;
    src_lta_fields_parameter = (bcmltd_fields_t *)ptr_math;

    ptr_math = ltm_buffer +
        lta_field_list->target_fields_parameter_offset;
    lta_fields_parameter = (bcmltd_fields_t *)ptr_math;
    lta_fields_ptrs = lta_fields_parameter->field;

    lta_field_maps = lta_field_list->field_maps;

    field_count = lta_fields_parameter->count;
    max_fields = lta_field_list->max_fields;

    map_count = lta_field_list->num_maps;

    for (fix = 0; fix < map_count; fix++) {
        cur_lta_map = &(lta_field_maps[fix]);

        lta_field = bcmltm_lta_find_field(src_lta_fields_parameter,
                                          cur_lta_map->api_field_id,
                                          cur_lta_map->field_idx);
        if (lta_field == NULL) { /* Is this optional? */
            continue;
        }

        if (field_count >= max_fields) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        lta_fields_ptrs[field_count] = lta_field;
        field_count++;
    }

    lta_fields_parameter->count = field_count;

 exit:
    SHR_FUNC_EXIT();
}

extern int
bcmltm_fa_node_lta_fields_to_wbc(int unit,
                                 bcmltm_lt_md_t *lt_md,
                                 bcmltm_lt_state_t *lt_state,
                                 bcmltm_entry_t *lt_entry,
                                 uint32_t *ltm_buffer,
                                 void *node_cookie)
{
    uint32_t ltid, fix, map_count;
    bcmltm_lta_field_list_t *lta_field_list;
    bcmltd_fields_t *lta_fields_parameter;
    bcmltm_field_list_t *lta_field;
    bcmltm_lta_field_map_t *lta_field_maps, *cur_lta_map = NULL;
    uint32_t *ptr_math;
    uint32_t field_val[BCMDRD_MAX_PT_WSIZE];
    uint32_t pt_suppress_offset;

    SHR_FUNC_ENTER(unit);

    ltid = lt_md->params->ltid;

    lta_field_list = BCMLTM_FA_LTA_FIELD_LIST(node_cookie);

    pt_suppress_offset = lta_field_list->pt_suppress_offset;
    if (pt_suppress_offset != BCMLTM_WB_OFFSET_INVALID) {
        if (ltm_buffer[pt_suppress_offset] != 0) {
            /*
             * Nothing to do since transform was skipped.
             * Also, PT operations will later be suppressed.
             */
            SHR_EXIT();
        }
    }

    ptr_math = ltm_buffer +
        lta_field_list->fields_parameter_offset;
    lta_fields_parameter = (bcmltd_fields_t *)ptr_math;

    lta_field_maps = lta_field_list->field_maps;
    map_count = lta_field_list->num_maps;

    for (fix = 0; fix < map_count; fix++) {
        cur_lta_map = &(lta_field_maps[fix]);

        lta_field = bcmltm_lta_find_field(lta_fields_parameter,
                                          cur_lta_map->api_field_id,
                                          cur_lta_map->field_idx);
        if (lta_field == NULL) { /* Is this optional? */
            continue;
        }

        sal_memset(field_val, 0, sizeof(field_val));
        shr_uint64_to_uint32_array(lta_field->data, field_val);

        /* Field validation check */
        if (!bcmltm_field_fit_check(&(cur_lta_map->wbc), field_val)) {
            BCMLTM_LOG_VERBOSE_FIELD_UNFIT(unit,
                                           ltid,
                                           cur_lta_map->api_field_id,
                                           cur_lta_map->field_idx,
                                           TRUE,
                                           lta_field->data,
                                           &(cur_lta_map->wbc));
            bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                                   BCMLRD_FIELD_FIELD_SIZE_ERROR_COUNT);
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        bcmltm_field_value_into_wbc(&(cur_lta_map->wbc),
                                    field_val, ltm_buffer);

        /* Update PT entry changed flag if appropriate. */
        if (cur_lta_map->pt_changed_offset != BCMLTM_WB_OFFSET_INVALID) {
            ltm_buffer[cur_lta_map->pt_changed_offset] =
                BCMLTM_PT_ENTRY_CHANGED_VALUE;
        }
    }

 exit:
    if (SHR_FUNC_ERR()) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, ltid);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "LTM %s(ltid=%d): "
                                "LTA output field"
                                " value 0x%" PRIx64 " invalid fit"
                                "(minbit=%d maxbit=%d).\n"),
                     table_name, ltid,
                     lta_field->data,
                     cur_lta_map->wbc.minbit, cur_lta_map->wbc.maxbit));

        /* Display API-facing LTA input list */
        api_lta_field_list_log_verbose(unit, ltid,
                                       ltm_buffer, lta_field_list);

        bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                               BCMLRD_FIELD_FIELD_SIZE_ERROR_COUNT);
    }
    SHR_FUNC_EXIT();
}

extern int
bcmltm_fa_node_wbc_to_lta_fields(int unit,
                                 bcmltm_lt_md_t *lt_md,
                                 bcmltm_lt_state_t *lt_state,
                                 bcmltm_entry_t *lt_entry,
                                 uint32_t *ltm_buffer,
                                 void *node_cookie)
{
    uint32_t fix, field_count, map_count, max_fields;
    bcmltm_lta_field_list_t *lta_field_list;
    bcmltd_field_t **lta_fields_ptrs, *cur_lta_field;
    bcmltd_fields_t *lta_fields_parameter;
    bcmltm_lta_field_map_t *lta_field_maps, *cur_lta_map;
    uint32_t *ptr_math;
    uint32_t field_val[BCMDRD_MAX_PT_WSIZE];
    uint32_t pt_suppress_offset;

    SHR_FUNC_ENTER(unit);

    lta_field_list = BCMLTM_FA_LTA_FIELD_LIST(node_cookie);

    pt_suppress_offset = lta_field_list->pt_suppress_offset;
    if (pt_suppress_offset != BCMLTM_WB_OFFSET_INVALID) {
        if (ltm_buffer[pt_suppress_offset] != 0) {
            /*
             * Nothing to copy from the PTM buffer since PT operations
             * were suppressed.
             *
             * The corresponding reverse transform is suppressed.
             * Later, during the LTA to API step, the API defaults are
             * supplied instead of the transform output fields.
             */
            SHR_EXIT();
        }
    }

    ptr_math = ltm_buffer +
        lta_field_list->fields_parameter_offset;
    lta_fields_parameter = (bcmltd_fields_t *)ptr_math;

    lta_field_maps = lta_field_list->field_maps;
    map_count = lta_field_list->num_maps;

    field_count = lta_fields_parameter->count;
    max_fields = lta_field_list->max_fields;
    lta_fields_ptrs = lta_fields_parameter->field;

    for (fix = 0; fix < map_count; fix++) {
        cur_lta_map = &(lta_field_maps[fix]);
        ptr_math = ltm_buffer + cur_lta_map->field_offset;
        cur_lta_field = (bcmltd_field_t *)ptr_math;
        sal_memset(cur_lta_field, 0, sizeof(bcmltd_field_t));

        /* Is this memset required? */
        sal_memset(field_val, 0, sizeof(field_val));
        bcmltm_field_value_from_wbc(&(cur_lta_map->wbc),
                                    field_val, ltm_buffer);

        shr_uint32_array_to_uint64(field_val, &(cur_lta_field->data));

        if (field_count >= max_fields) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        cur_lta_field->id = cur_lta_map->api_field_id;
        cur_lta_field->idx = cur_lta_map->field_idx;

        lta_fields_ptrs[field_count] = cur_lta_field;
        field_count++;
    }

    lta_fields_parameter->count = field_count;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_lta_transform(int unit,
                             bcmltm_lt_md_t *lt_md,
                             bcmltm_lt_state_t *lt_state,
                             bcmltm_entry_t *lt_entry,
                             uint32_t *ltm_buffer,
                             void *node_cookie)
{
    bcmltm_lta_transform_params_t *lta_transform_params;
    bcmltd_handler_transform_op_f lta_transform;
    bcmltd_transform_arg_t *lta_args;
    bcmltd_fields_t *in_fields, *out_fields;
    uint32_t *ptr_math;
    uint32_t suppress_offset, smi_offset, smi;

    SHR_FUNC_ENTER(unit);

    lta_transform_params = BCMLTM_LTA_TRANSFORM_PARAMS(node_cookie);

    /* Check if transform should be omitted */
    suppress_offset = lta_transform_params->transform_suppress_offset;
    if ((suppress_offset != BCMLTM_WB_OFFSET_INVALID) &&
        (ltm_buffer[suppress_offset] != 0)) {
        SHR_EXIT();
    }

    smi_offset = lta_transform_params->select_map_index_offset;
    smi = lta_transform_params->select_map_index;
    if ((smi_offset != BCMLTM_WB_OFFSET_INVALID) &&
        (ltm_buffer[smi_offset] != smi)) {
        /* Field select choice is not active, do not invoke transform. */
        SHR_EXIT();
    }

    lta_transform = lta_transform_params->lta_transform;
    lta_args = lta_transform_params->lta_args;
    ptr_math = ltm_buffer + lta_transform_params->fields_input_offset;
    in_fields = (bcmltd_fields_t *)ptr_math;
    ptr_math = ltm_buffer + lta_transform_params->fields_output_offset;
    out_fields = (bcmltd_fields_t *)ptr_math;
    SHR_IF_ERR_VERBOSE_EXIT
        ((lta_transform)(unit,
                         in_fields,
                         out_fields,
                         lta_args));

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                               BCMLRD_FIELD_TRANSFORM_ERROR_COUNT);
    }
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_lta_ext_transform(int unit,
                                 bcmltm_lt_md_t *lt_md,
                                 bcmltm_lt_state_t *lt_state,
                                 bcmltm_entry_t *lt_entry,
                                 uint32_t *ltm_buffer,
                                 void *node_cookie)
{
    bcmltm_lta_transform_params_t *lta_transform_params;
    bcmltd_handler_ext_transform_op_f lta_ext_transform;
    bcmltd_transform_arg_t *lta_args;
    bcmltd_fields_t *in_key_fields, *in_value_fields, *out_fields;
    uint32_t *ptr_math;
    uint32_t suppress_offset, smi_offset, smi;

    SHR_FUNC_ENTER(unit);

    lta_transform_params = BCMLTM_LTA_TRANSFORM_PARAMS(node_cookie);

    /* Check if transform should be omitted */
    suppress_offset = lta_transform_params->transform_suppress_offset;
    if ((suppress_offset != BCMLTM_WB_OFFSET_INVALID) &&
        (ltm_buffer[suppress_offset] != 0)) {
        SHR_EXIT();
    }

    smi_offset = lta_transform_params->select_map_index_offset;
    smi = lta_transform_params->select_map_index;
    if ((smi_offset != BCMLTM_WB_OFFSET_INVALID) &&
        (ltm_buffer[smi_offset] != smi)) {
        /* Field select choice is not active, do not invoke transform. */
        SHR_EXIT();
    }

    lta_ext_transform = lta_transform_params->lta_ext_transform;
    lta_args = lta_transform_params->lta_args;
    ptr_math = ltm_buffer + lta_transform_params->fields_input_key_offset;
    in_key_fields = (bcmltd_fields_t *)ptr_math;
    ptr_math = ltm_buffer + lta_transform_params->fields_input_offset;
    in_value_fields = (bcmltd_fields_t *)ptr_math;
    ptr_math = ltm_buffer + lta_transform_params->fields_output_offset;
    out_fields = (bcmltd_fields_t *)ptr_math;

    SHR_IF_ERR_VERBOSE_EXIT
        ((lta_ext_transform)(unit,
                             in_key_fields,
                             in_value_fields,
                             out_fields,
                             lta_args));

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                               BCMLRD_FIELD_TRANSFORM_ERROR_COUNT);
    }
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_lta_traverse_transform(int unit,
                                      bcmltm_lt_md_t *lt_md,
                                      bcmltm_lt_state_t *lt_state,
                                      bcmltm_entry_t *lt_entry,
                                      uint32_t *ltm_buffer,
                                      void *node_cookie)
{
    if (lt_entry->in_fields != NULL) { /* TRAVERSE next */
        return bcmltm_fa_node_lta_transform(unit,
                                            lt_md,
                                            lt_state,
                                            lt_entry,
                                            ltm_buffer,
                                            node_cookie);
    }
    /* Else, do not perform key fields forward transform on traverse start
     * because there are no key fields supplied.
     */

    return SHR_E_NONE;
}

int
bcmltm_fa_node_lta_alloc_second_transform(int unit,
                                          bcmltm_lt_md_t *lt_md,
                                          bcmltm_lt_state_t *lt_state,
                                          bcmltm_entry_t *lt_entry,
                                          uint32_t *ltm_buffer,
                                          void *node_cookie)
{
    bcmltm_lta_transform_params_t *lta_transform_params;
    bcmltd_handler_transform_op_f lta_transform;
    bcmltd_transform_arg_t *lta_args;
    bcmltd_fields_t *in_fields, *out_fields;
    uint32_t *ptr_math;
    uint32_t suppress_offset, smi_offset, smi;

    SHR_FUNC_ENTER(unit);

    lta_transform_params = BCMLTM_LTA_TRANSFORM_PARAMS(node_cookie);

    /*
     * If allocation occured, suppress_offset should be set.  If it is
     * not, this transform should be skipped.
     */
    suppress_offset = lta_transform_params->transform_suppress_offset;
    if ((suppress_offset != BCMLTM_WB_OFFSET_INVALID) &&
        (ltm_buffer[suppress_offset] == 0)) {
        SHR_EXIT();
    }

    smi_offset = lta_transform_params->select_map_index_offset;
    smi = lta_transform_params->select_map_index;
    if ((smi_offset != BCMLTM_WB_OFFSET_INVALID) &&
        (ltm_buffer[smi_offset] != smi)) {
        /* Field select choice is not active, do not invoke transform. */
        SHR_EXIT();
    }

    lta_transform = lta_transform_params->lta_transform;
    lta_args = lta_transform_params->lta_args;
    ptr_math = ltm_buffer + lta_transform_params->fields_input_offset;
    in_fields = (bcmltd_fields_t *)ptr_math;
    ptr_math = ltm_buffer + lta_transform_params->fields_output_offset;
    out_fields = (bcmltd_fields_t *)ptr_math;
    SHR_IF_ERR_VERBOSE_EXIT
        ((lta_transform)(unit,
                         in_fields,
                         out_fields,
                         lta_args));

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                               BCMLRD_FIELD_TRANSFORM_ERROR_COUNT);
    }
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_lta_pt_sid_map(int unit,
                              bcmltm_lt_md_t *lt_md,
                              bcmltm_lt_state_t *lt_state,
                              bcmltm_entry_t *lt_entry,
                              uint32_t *ltm_buffer,
                              void *node_cookie)
{
    bcmltm_lta_transform_params_t *lta_transform_params;
    bcmltd_transform_arg_t *lta_args;
    bcmltd_sid_t pt_select;

    SHR_FUNC_ENTER(unit);

    lta_transform_params = BCMLTM_LTA_TRANSFORM_PARAMS(node_cookie);
    lta_args = lta_transform_params->lta_args;

    if (lta_transform_params->pt_sid_selector_offset != BCMLTM_WB_OFFSET_INVALID) {
        /* Selector */
        pt_select = ltm_buffer[lta_transform_params->pt_sid_selector_offset];

        if (pt_select >= lta_args->tables) {
            /* Out of range */
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        if (lta_transform_params->pt_sid_override_offset != BCMLTM_WB_OFFSET_INVALID) {
            ltm_buffer[lta_transform_params->pt_sid_override_offset] =
                lta_args->table[pt_select];
        } else {
            /* If a selector is provided, there should be space for
             * the PT SID value. */
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_lta_validate(int unit,
                            bcmltm_lt_md_t *lt_md,
                            bcmltm_lt_state_t *lt_state,
                            bcmltm_entry_t *lt_entry,
                            uint32_t *ltm_buffer,
                            void *node_cookie)
{
    bcmltm_lta_validate_params_t *lta_validate_params;
    bcmltd_handler_field_val_op_f lta_validate;
    bcmltd_field_val_arg_t *lta_args;
    bcmltd_fields_t *in_fields;
    uint32_t *ptr_math;
    uint32_t smi_offset, smi;

    SHR_FUNC_ENTER(unit);

    lta_validate_params = BCMLTM_LTA_VALIDATE_PARAMS(node_cookie);
    lta_validate = lta_validate_params->lta_validate;
    lta_args = lta_validate_params->lta_args;
    ptr_math = ltm_buffer + lta_validate_params->fields_input_offset;
    in_fields = (bcmltd_fields_t *)ptr_math;

    smi_offset = lta_validate_params->select_map_index_offset;
    smi = lta_validate_params->select_map_index;
    if ((smi_offset != BCMLTM_WB_OFFSET_INVALID) &&
        (ltm_buffer[smi_offset] != smi)) {
        /* Field select choice is not active, do not invoke transform. */
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        ((lta_validate)(unit,
                        lt_entry->opcode.lt_opcode,
                        in_fields,
                        lta_args));

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                               BCMLRD_FIELD_VALIDATE_ERROR_COUNT);
    }
    SHR_FUNC_EXIT();
}
