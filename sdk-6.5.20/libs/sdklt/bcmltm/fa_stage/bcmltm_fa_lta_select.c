/*! \file bcmltm_fa_lta_select.c
 *
 * Logical Table Manager Field Adaptation
 *
 * Logical Table Adaptors
 *
 * This file contains the LTM FA stage functions for LTA validate and
 * transform operations when the Logical Table employs field selection.
 * Custom table handlers (CTH) do not apply here because CTH
 * implementations must manage all LT steps beyond index LT key bounds
 * and table entry limits.
 *
 * All of the function nodes in this file will employ the field
 * select map index conditional execution model.  If the field
 * select map does not match the specified value, the function
 * will not operate.
 *
 * Relevant functions will also check for IwA INSERT index absent
 * and generic PT suppress conditions.
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
 * \brief Log Verbose for API-facing LTA input list with field selection.
 *
 * This routine performs a log verbose and displays
 * the API facing fields for the specified LTA field list, when
 * the Logical Table supports field selection.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] lsf_list LTA select field list metadata.
 */
static void
api_lta_select_field_list_log_verbose(int unit,
                                      uint32_t sid,
                                      const uint32_t *ltm_buffer,
                       const bcmltm_lta_select_field_list_t *lsf_list)
{
    const uint32_t *ptr_math;
    const bcmltd_fields_t *api_fields;
    const bcmltd_field_t *field;
    const char *field_name;
    uint32_t field_count;
    uint32_t fix;

    /* If API-facing field list is not available, just return */
    if ((lsf_list == NULL) ||
        (lsf_list->api_fields_parameter_offset ==
         BCMLTM_WB_OFFSET_INVALID)) {
        return;
    }

    ptr_math = ltm_buffer + lsf_list->api_fields_parameter_offset;
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
 * \brief Initialize a LTA field array within the Working Buffer based
 *        on a LTA field select structrue.
 *
 * This function prepares the space assigned withing the Working Buffer
 * to manage LTA input or output field lists, using the LTA select-aware
 * mapping structure.
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
bcmltm_fa_node_lta_select_field_list_init(int unit,
                                          bool output,
                                          uint32_t *ltm_buffer,
                                          void *node_cookie)
{
    uint32_t fix, field_count, max_fields;
    bcmltm_lta_select_field_list_t *lta_field_list;
    bcmltd_field_t **lta_fields_ptrs, *cur_lta_field;
    bcmltd_fields_t *lta_fields_parameter;
    bcmltm_lta_select_field_map_t *lsf_maps, *cur_lta_map;
    uint32_t *ptr_math;
    uint32_t smi_offset, smi;

    SHR_FUNC_ENTER(unit);

    lta_field_list = BCMLTM_FA_LTA_SELECT_FIELD_LIST(node_cookie);

    smi_offset = lta_field_list->select_map_index_offset;
    smi = lta_field_list->select_map_index;
    if ((smi_offset != BCMLTM_WB_OFFSET_INVALID) &&
        (ltm_buffer[smi_offset] != smi)) {
        /* Field select choice is not active, do nothing. */
        SHR_EXIT();
    }

    ptr_math = ltm_buffer + lta_field_list->fields_parameter_offset;
    lta_fields_parameter = (bcmltd_fields_t *)ptr_math;
    ptr_math = ltm_buffer + lta_field_list->fields_ptrs_offset;
    lta_fields_ptrs = (bcmltd_field_t **)ptr_math;
    lsf_maps = lta_field_list->lta_select_field_maps;

    max_fields = lta_field_list->max_fields;
    lta_fields_parameter->count = 0;
    lta_fields_parameter->field = lta_fields_ptrs;

    if (output) {
        /* Initialize field structures for output field values. */
        field_count = 0;
        for (fix = 0; fix < max_fields; fix++) {
            cur_lta_map = &(lsf_maps[fix]);
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

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Conditionally copy field structures from LTA field
 *        array pointers to the API cache.
 *
 * The central implementation of FA tree nodes which scan
 * an LTA field pointer array for fields which should be copied
 * to the API cache in field select LTs.
 *
 * The "alloc" argument provide context for the conditional decision
 * to copy the LTA output fields.
 * If TRUE, the LTA suppress flag in the WB is mapped to the index absent
 * flag.  The fields should be copied if the flag is _set_, because
 * this is the case of index with allocation returning the allocated
 * values to the API cache.
 * If FALSE, the LTA suppress flag should be used in its standard
 * meaning, to skip copying fields when set because the matching
 * transform was suppressed.
 *
 * \param [in] unit Unit number.
 * \param [in] alloc TRUE - index allocation key copy.
 *                   FALSE - honor LTA suppress.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmltm_fa_node_select_lta_to_api_wb_core(int unit,
                                         bool alloc,
                                         bcmltm_lt_md_t *lt_md,
                                         bcmltm_lt_state_t *lt_state,
                                         bcmltm_entry_t *lt_entry,
                                         uint32_t *ltm_buffer,
                                         void *node_cookie)
{
    uint32_t mix, map_count;
    bcmltd_fields_t *lta_fields_parameter;
    bcmltm_field_list_t *api_field_data, *cur_lta_field;
    uint32_t *ptr_math;
    bcmltm_lta_select_field_map_t *lsf_maps, *cur_lta_map;
    bcmltm_lta_select_field_list_t *lsf_list;
    uint32_t smi_offset, smi;

    SHR_FUNC_ENTER(unit);

    lsf_list = BCMLTM_FA_LTA_SELECT_FIELD_LIST(node_cookie);
    SHR_NULL_CHECK(lsf_list, SHR_E_INTERNAL);

    smi_offset = lsf_list->select_map_index_offset;
    smi = lsf_list->select_map_index;
    if ((smi_offset != BCMLTM_WB_OFFSET_INVALID) &&
        (ltm_buffer[smi_offset] != smi)) {
        /* Field select choice is not active, do nothing. */
        SHR_EXIT();
    }

    if (lsf_list->lta_suppress_offset != BCMLTM_WB_OFFSET_INVALID) {
        if (alloc) {
            if (ltm_buffer[lsf_list->lta_suppress_offset] == 0) {
                /*
                 * No allocation occurred, no fields to copy.
                 */
                SHR_EXIT();
            }
        } else {
            if (ltm_buffer[lsf_list->lta_suppress_offset] != 0) {
                /*
                 * LTA op suppressed, no fields to copy.
                 */
                SHR_EXIT();
            }
        }
    }

    ptr_math = ltm_buffer + lsf_list->fields_parameter_offset;
    lta_fields_parameter = (bcmltd_fields_t *)ptr_math;

    lsf_maps = lsf_list->lta_select_field_maps;
    map_count = lsf_list->num_maps;

    for (mix = 0; mix < map_count; mix++) {
        cur_lta_map = &(lsf_maps[mix]);

        cur_lta_field = bcmltm_lta_find_field(lta_fields_parameter,
                                              cur_lta_map->field_id,
                                              cur_lta_map->field_idx);
        if (cur_lta_field == NULL) { /* Field omitted from LTA output */
            continue;
        }

        ptr_math = ltm_buffer + cur_lta_map->apic_offset;
        api_field_data = (bcmltm_field_list_t *)ptr_math;

        sal_memcpy(api_field_data, cur_lta_field,
                   sizeof(bcmltd_field_t));
    }

 exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

extern int
bcmltm_fa_node_lta_select_input_init(int unit,
                                     bcmltm_lt_md_t *lt_md,
                                     bcmltm_lt_state_t *lt_state,
                                     bcmltm_entry_t *lt_entry,
                                     uint32_t *ltm_buffer,
                                     void *node_cookie)
{
    return bcmltm_fa_node_lta_select_field_list_init(unit, FALSE,
                                                     ltm_buffer, node_cookie);
}

extern int
bcmltm_fa_node_lta_select_output_init(int unit,
                                      bcmltm_lt_md_t *lt_md,
                                      bcmltm_lt_state_t *lt_state,
                                      bcmltm_entry_t *lt_entry,
                                      uint32_t *ltm_buffer,
                                      void *node_cookie)
{
    return bcmltm_fa_node_lta_select_field_list_init(unit, TRUE,
                                                     ltm_buffer, node_cookie);
}

/*!
 * \brief An FA tree node to append field structures from API cache
 *        to LTA field array pointers.
 *
 * An FA tree node which fills the LTA field pointer array with the
 * elements of the API cache employed for field select LTs.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmltm_fa_node_select_api_wb_to_lta(int unit,
                                    bcmltm_lt_md_t *lt_md,
                                    bcmltm_lt_state_t *lt_state,
                                    bcmltm_entry_t *lt_entry,
                                    uint32_t *ltm_buffer,
                                    void *node_cookie)
{
    uint32_t fix, field_count, map_count;
    bcmltd_field_t **lta_fields_ptrs, *cur_lta_field;
    bcmltd_fields_t *lta_fields_parameter;
    bcmltm_lta_select_field_map_t *lsf_maps, *cur_lta_map;
    uint32_t *ptr_math;
    bcmltm_lta_select_field_list_t *lsf_list;
    uint32_t smi_offset, smi;

    SHR_FUNC_ENTER(unit);

    lsf_list = BCMLTM_FA_LTA_SELECT_FIELD_LIST(node_cookie);
    SHR_NULL_CHECK(lsf_list, SHR_E_INTERNAL);

    smi_offset = lsf_list->select_map_index_offset;
    smi = lsf_list->select_map_index;
    if ((smi_offset != BCMLTM_WB_OFFSET_INVALID) &&
        (ltm_buffer[smi_offset] != smi)) {
        /* Field select choice is not active, do nothing. */
        SHR_EXIT();
    }

    ptr_math = ltm_buffer + lsf_list->fields_parameter_offset;
    lta_fields_parameter = (bcmltd_fields_t *)ptr_math;
    lta_fields_ptrs = lta_fields_parameter->field;
    lsf_maps = lsf_list->lta_select_field_maps;

    map_count = lsf_list->num_maps;

    field_count = lta_fields_parameter->count;

    for (fix = 0; fix < map_count; fix++) {
        cur_lta_map = &(lsf_maps[fix]);
        ptr_math = ltm_buffer + cur_lta_map->apic_offset;
        cur_lta_field = (bcmltd_field_t *)ptr_math;
        /*
         * cur_lta_field should now point to the API field location in
         * the API cache.
         */

        lta_fields_ptrs[field_count] = cur_lta_field;
        field_count++;
    }

    lta_fields_parameter->count = field_count;

 exit:
    SHR_FUNC_EXIT();
}

extern int
bcmltm_fa_node_select_lta_to_api_wb(int unit,
                                    bcmltm_lt_md_t *lt_md,
                                    bcmltm_lt_state_t *lt_state,
                                    bcmltm_entry_t *lt_entry,
                                    uint32_t *ltm_buffer,
                                    void *node_cookie)
{
    return bcmltm_fa_node_select_lta_to_api_wb_core(unit, FALSE,
                                                    lt_md, lt_state,
                                                    lt_entry,
                                                    ltm_buffer,
                                                    node_cookie);
}

extern int
bcmltm_fa_node_select_alloc_lta_to_api_wb(int unit,
                                          bcmltm_lt_md_t *lt_md,
                                          bcmltm_lt_state_t *lt_state,
                                          bcmltm_entry_t *lt_entry,
                                          uint32_t *ltm_buffer,
                                          void *node_cookie)
{
    return bcmltm_fa_node_select_lta_to_api_wb_core(unit, TRUE,
                                                    lt_md, lt_state,
                                                    lt_entry,
                                                    ltm_buffer,
                                                    node_cookie);
}

extern int
bcmltm_fa_node_select_lta_to_wbc(int unit,
                                 bcmltm_lt_md_t *lt_md,
                                 bcmltm_lt_state_t *lt_state,
                                 bcmltm_entry_t *lt_entry,
                                 uint32_t *ltm_buffer,
                                 void *node_cookie)
{
    uint32_t ltid, fix, map_count;
    bcmltd_fields_t *lta_fields_parameter;
    bcmltm_field_list_t *lta_field;
    uint32_t *ptr_math;
    uint32_t field_val[BCMDRD_MAX_PT_WSIZE];
    bcmltm_lta_select_field_map_t *lsf_maps, *cur_lta_map = NULL;
    bcmltm_lta_select_field_list_t *lsf_list = NULL;
    uint32_t smi_offset, smi;

    SHR_FUNC_ENTER(unit);

    ltid = lt_md->params->ltid;

    lsf_list = BCMLTM_FA_LTA_SELECT_FIELD_LIST(node_cookie);
    SHR_NULL_CHECK(lsf_list, SHR_E_INTERNAL);

    smi_offset = lsf_list->select_map_index_offset;
    smi = lsf_list->select_map_index;
    if ((smi_offset != BCMLTM_WB_OFFSET_INVALID) &&
        (ltm_buffer[smi_offset] != smi)) {
        /* Field select choice is not active, do nothing. */
        SHR_EXIT();
    }

    if (lsf_list->lta_suppress_offset != BCMLTM_WB_OFFSET_INVALID) {
        if (ltm_buffer[lsf_list->lta_suppress_offset] != 0) {
            /*
             * LTA op suppressed, no fields to copy.
             */
            SHR_EXIT();
        }
    }

    ptr_math = ltm_buffer +
        lsf_list->fields_parameter_offset;
    lta_fields_parameter = (bcmltd_fields_t *)ptr_math;

    lsf_maps = lsf_list->lta_select_field_maps;
    map_count = lsf_list->num_maps;

    for (fix = 0; fix < map_count; fix++) {
        cur_lta_map = &(lsf_maps[fix]);

        lta_field = bcmltm_lta_find_field(lta_fields_parameter,
                                          cur_lta_map->field_id,
                                          cur_lta_map->field_idx);
        if (lta_field == NULL) { /* Field omitted from output */
            continue;
        }

        sal_memset(field_val, 0, sizeof(field_val));
        shr_uint64_to_uint32_array(lta_field->data, field_val);

        /* Field validation check */
        if (!bcmltm_field_fit_check(&(cur_lta_map->wbc), field_val)) {
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

        if (cur_lta_map != NULL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "LTM %s(ltid=%d): "
                                    "LTA output field"
                                    " value 0x%" PRIx64 " invalid fit"
                                    "(minbit=%d maxbit=%d).\n"),
                         table_name, ltid,
                         lta_field->data,
                         cur_lta_map->wbc.minbit, cur_lta_map->wbc.maxbit));
        }

        /* Display API-facing LTA input list */
        api_lta_select_field_list_log_verbose(unit, ltid,
                                              ltm_buffer, lsf_list);

        bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                               BCMLRD_FIELD_FIELD_SIZE_ERROR_COUNT);
    }
    SHR_FUNC_EXIT();
}

extern int
bcmltm_fa_node_select_wbc_to_lta(int unit,
                                 bcmltm_lt_md_t *lt_md,
                                 bcmltm_lt_state_t *lt_state,
                                 bcmltm_entry_t *lt_entry,
                                 uint32_t *ltm_buffer,
                                 void *node_cookie)
{
    uint32_t fix, field_count, map_count, max_fields;
    bcmltd_field_t **lta_fields_ptrs, *cur_lta_field;
    bcmltd_fields_t *lta_fields_parameter;
    uint32_t *ptr_math;
    uint32_t field_val[BCMDRD_MAX_PT_WSIZE];
    bcmltm_lta_select_field_map_t *lsf_maps, *cur_lta_map;
    bcmltm_lta_select_field_list_t *lsf_list;
    uint32_t smi_offset, smi;

    SHR_FUNC_ENTER(unit);

    lsf_list = BCMLTM_FA_LTA_SELECT_FIELD_LIST(node_cookie);
    SHR_NULL_CHECK(lsf_list, SHR_E_INTERNAL);

    smi_offset = lsf_list->select_map_index_offset;
    smi = lsf_list->select_map_index;
    if ((smi_offset != BCMLTM_WB_OFFSET_INVALID) &&
        (ltm_buffer[smi_offset] != smi)) {
        /* Field select choice is not active, do nothing. */
        SHR_EXIT();
    }

    if (lsf_list->lta_suppress_offset != BCMLTM_WB_OFFSET_INVALID) {
        if (ltm_buffer[lsf_list->lta_suppress_offset] != 0) {
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
        lsf_list->fields_parameter_offset;
    lta_fields_parameter = (bcmltd_fields_t *)ptr_math;

    lsf_maps = lsf_list->lta_select_field_maps;
    map_count = lsf_list->num_maps;

    field_count = lta_fields_parameter->count;
    max_fields = lsf_list->max_fields;
    lta_fields_ptrs = lta_fields_parameter->field;

    for (fix = 0; fix < map_count; fix++) {
        cur_lta_map = &(lsf_maps[fix]);
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

        cur_lta_field->id = cur_lta_map->field_id;
        cur_lta_field->idx = cur_lta_map->field_idx;

        lta_fields_ptrs[field_count] = cur_lta_field;
        field_count++;
    }

    lta_fields_parameter->count = field_count;

 exit:
    SHR_FUNC_EXIT();
}

