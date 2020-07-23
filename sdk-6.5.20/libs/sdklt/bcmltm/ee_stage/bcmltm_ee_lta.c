/*! \file bcmltm_ee_lta.c
 *
 * Logical Table Manager Execution Engine
 *
 * Logical Table Adaptors
 *
 * This file contains the LTM EE stage functions for LTA custom table
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
#include <bcmltm/bcmltm_tree.h>
#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm_ee_tree_nodes_internal.h>
#include <bcmltm/bcmltm_fa_util_internal.h>
#include <bcmltm/bcmltm_stats_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_EXECUTIONENGINE


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Initialize the LTA table handler operation argument.
 *
 * This routine initializes the LTA table handler operation argument
 * with the corresponding API entry information.
 *
 * \param [out] op_arg Table handler operation argument to initialize.
 * \param [in] lt_entry API entry for this operation.
 */
static void
lta_table_handler_op_arg_init(bcmltd_op_arg_t *op_arg,
                              const bcmltm_entry_t *lt_entry)
{
    sal_memset(op_arg, 0, sizeof(*op_arg));

    op_arg->trans_id = lt_entry->trans_id;
    op_arg->attrib = lt_entry->attrib;

    return;
}

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
 * \brief Determine the error statistic when a CTH validate
 * function returns error.
 *
 * When a CTH implementation opts for the expedited path, no value
 * checks are performed.  The CTH validate function verifies any
 * error conditions.  The presumption is that debugged code will
 * not have parameter errors in general.
 *
 * If an error _is_ returned from CTH validate, the LTM must determine
 * the correct error statistic.  The penalty for extra work is
 * reserved to the error case.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] in_fields Start of input API field list.
 * \param [in] lta_field_list Pointer to the context information for this
 *             operation.
 *
 * \retval TRUE if parameter error found, FALSE otherwise.
 */
static bool
bcmltm_node_cth_validate_stat(int unit,
                              bcmltm_lt_md_t *lt_md,
                              bcmltm_field_list_t *in_fields,
                              const bcmltm_lta_field_list_t *lta_field_list)
{
    uint32_t map_count;
    bcmltm_lta_field_map_t *lta_field_maps, *cur_lta_map,
        *error_lta_map = NULL;
    bcmltm_field_spec_t match_spec;
    bcmltm_field_list_t *error_field_data = NULL, *api_field_data = in_fields;

    lta_field_maps = lta_field_list->field_maps;
    map_count = lta_field_list->num_maps;

    while (api_field_data != NULL) {
        
        if ((api_field_data->flags & SHR_FMM_FIELD_DEL) == 0) {

            match_spec.field_id = api_field_data->id;
            match_spec.field_idx = api_field_data->idx;

            /* Binary search of metadata, which is ordered by (fid, fidx) */
            cur_lta_map = sal_bsearch(&match_spec,
                                      lta_field_maps, map_count,
                                      sizeof(*cur_lta_map),
                                      lta_field_spec_compare);

            if (cur_lta_map != NULL) {

                if ((cur_lta_map->flags & BCMLTM_FIELD_READ_ONLY) != 0) {
                    /*
                     * Read-only value fields should not be in API input.
                     * This error takes precedence over OOB value errors below.
                     */
                    BCMLTM_LOG_VERBOSE_FIELD_RO_WRITE(unit,
                                                      lt_md->params->ltid,
                                                      cur_lta_map->api_field_id,
                                                      cur_lta_map->field_idx,
                                                      TRUE);
                    bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                                           BCMLRD_FIELD_FIELD_LIST_ERROR_COUNT);
                    return TRUE;
                }

                /* Check that the supplied field value is legal. */
                if ((api_field_data->data < cur_lta_map->minimum_value) ||
                    (api_field_data->data > cur_lta_map->maximum_value)) {
                    if (error_lta_map == NULL) {
                        /* Only record first out of bounds error. */
                        error_lta_map = cur_lta_map;
                        error_field_data = api_field_data;
                    }
                }
            }
        }
        api_field_data = api_field_data->next;
    }

    /* Out of bounds field value */
    if ((error_lta_map != NULL) && (error_field_data != NULL)) {
        BCMLTM_LOG_VERBOSE_FIELD_OOB(unit,
                                     lt_md->params->ltid,
                                     error_lta_map->api_field_id,
                                     error_lta_map->field_idx,
                                     TRUE,
                                     error_field_data->data,
                                     error_lta_map->minimum_value,
                                     error_lta_map->maximum_value);
        bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                               BCMLRD_FIELD_FIELD_SIZE_ERROR_COUNT);
        return TRUE;
    }

    return FALSE;
}


/*!
 * \brief Check for missing keys when a CTH validate
 * function returns error.
 *
 * When a CTH implementation opts for the expedited path, no key
 * checks are performed.  The CTH validate function verifies any
 * error conditions.  The presumption is that debugged code will
 * not have parameter errors in general.
 *
 * If an error _is_ returned from CTH validate, the LTM must determine
 * the correct error statistic.  The penalty for extra work is
 * reserved to the error case.
 *
 * NB: This check only works for index tables at present.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] in_fields Start of input API field list.
 * \param [in] lta_field_list Pointer to the context information for this
 *             operation.
 *
 * \retval TRUE if parameter error found, FALSE otherwise.
 */
static bool
bcmltm_node_cth_validate_keys(int unit,
                              bcmltm_lt_md_t *lt_md,
                              bcmltm_field_list_t *in_fields,
                              const bcmltm_lta_field_list_t *lta_field_list)
{
    uint32_t map_count, key_count = 0;
    bcmltm_lta_field_map_t *lta_field_maps, *cur_lta_map,
        *error_lta_map = NULL;
    bcmltm_field_spec_t match_spec;
    bcmltm_field_list_t *error_field_data = NULL, *api_field_data = in_fields;

    lta_field_maps = lta_field_list->field_maps;
    map_count = lta_field_list->num_maps;

    while (api_field_data != NULL) {
        match_spec.field_id = api_field_data->id;
        match_spec.field_idx = api_field_data->idx;

        /* Binary search of metadata, which is ordered by (fid, fidx) */
        cur_lta_map = sal_bsearch(&match_spec,
                                  lta_field_maps, map_count,
                                  sizeof(*cur_lta_map),
                                  lta_field_spec_compare);

        if (cur_lta_map != NULL) {
            /* Check that the supplied field value is legal. */
            if ((api_field_data->data < cur_lta_map->minimum_value) ||
                (api_field_data->data > cur_lta_map->maximum_value)) {
                if (error_lta_map == NULL) {
                    /* Only record first out of bounds error. */
                    error_lta_map = cur_lta_map;
                    error_field_data = api_field_data;
                }
            }
            key_count++;
        }
        api_field_data = api_field_data->next;
    }

    if (key_count != map_count) {
        const char *table_name;

        /* Missing key field(s) */
        table_name = bcmltm_lt_table_sid_to_name(unit, lt_md->params->ltid);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "LTM %s(ltid=%d): "
                                "Incorrect number of key fields\n"),
                     table_name, lt_md->params->ltid));

        bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                               BCMLRD_FIELD_FIELD_LIST_ERROR_COUNT);
        return TRUE;
    }

    /* Out of bounds field value */
    if ((error_lta_map != NULL) && (error_field_data != NULL)) {
        BCMLTM_LOG_VERBOSE_FIELD_OOB(unit,
                                     lt_md->params->ltid,
                                     error_lta_map->api_field_id,
                                     error_lta_map->field_idx,
                                     TRUE,
                                     error_field_data->data,
                                     error_lta_map->minimum_value,
                                     error_lta_map->maximum_value);
        bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                               BCMLRD_FIELD_FIELD_SIZE_ERROR_COUNT);
        return TRUE;
    }

    return FALSE;
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_ee_node_lta_insert(int unit,
                          bcmltm_lt_md_t *lt_md,
                          bcmltm_lt_state_t *lt_state,
                          bcmltm_entry_t *lt_entry,
                          uint32_t *ltm_buffer,
                          void *node_cookie)
{
    int rv = SHR_E_EXISTS;
    bcmltm_lta_table_params_t *lta_table_params;
    const bcmltd_table_handler_t *lta_handler;
    bcmltd_fields_t *in_fields;
    uint32_t *ptr_math;
    bool validate_error = FALSE;

    SHR_FUNC_ENTER(unit);

    lta_table_params = BCMLTM_LTA_TABLE_PARAMS(node_cookie);
    lta_handler = lta_table_params->handler;
    ptr_math = ltm_buffer + lta_table_params->fields_input_offset;
    in_fields = (bcmltd_fields_t *)ptr_math;

    if (lta_handler->validate != NULL) {
        rv = lta_handler->validate(unit,
                                   BCMLT_OPCODE_INSERT,
                                   in_fields,
                                   lta_handler->arg);

        if (rv == SHR_E_BADID) {
            /* Key fields are invalid.  Convert error code. */
            rv = SHR_E_PARAM;
        } else if (rv != SHR_E_EXISTS) {
            /* Entry exists error takes priority over table full */
            if (lt_state->entry_count >= lt_state->max_entry_count) {
                SHR_ERR_EXIT(SHR_E_FULL);
            }
            if (SHR_FAILURE(rv)) {
                /* Check for missing key error */
                if (lta_table_params->lta_key_list != NULL) {
                    validate_error =
                        bcmltm_node_cth_validate_keys(unit, lt_md,
                                                      lt_entry->in_fields,
                                        lta_table_params->lta_key_list);
                }
                if (!validate_error &&
                    (lta_table_params->lta_field_list != NULL)) {
                    validate_error =
                        bcmltm_node_cth_validate_stat(unit, lt_md,
                                                      lt_entry->in_fields,
                                        lta_table_params->lta_field_list);
                }
            }
        }
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    if (lta_handler->op_insert != NULL) {
        bcmltd_op_arg_t op_arg;

        lta_table_handler_op_arg_init(&op_arg, lt_entry);
        SHR_IF_ERR_VERBOSE_EXIT
            (lta_handler->op_insert(unit,
                                    &op_arg,
                                    in_fields,
                                    NULL,
                                    lta_handler->arg));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (lta_handler->insert(unit,
                                 lt_entry->trans_id,
                                 in_fields,
                                 lta_handler->arg));
    }

 exit:
    if (SHR_FUNC_ERR() && !validate_error) {
        bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                               BCMLRD_FIELD_TABLE_HANDLER_ERROR_COUNT);
    }
    SHR_FUNC_EXIT();
}

int
bcmltm_ee_node_lta_alloc_insert(int unit,
                                bcmltm_lt_md_t *lt_md,
                                bcmltm_lt_state_t *lt_state,
                                bcmltm_entry_t *lt_entry,
                                uint32_t *ltm_buffer,
                                void *node_cookie)
{
    int rv = SHR_E_EXISTS;
    bcmltm_lta_table_params_t *lta_table_params;
    const bcmltd_table_handler_t *lta_handler;
    bcmltd_fields_t *in_fields, *out_fields;
    uint32_t *ptr_math;

    SHR_FUNC_ENTER(unit);

    lta_table_params = BCMLTM_LTA_TABLE_PARAMS(node_cookie);
    lta_handler = lta_table_params->handler;
    ptr_math = ltm_buffer + lta_table_params->fields_input_offset;
    in_fields = (bcmltd_fields_t *)ptr_math;
    ptr_math = ltm_buffer + lta_table_params->fields_output_offset;
    out_fields = (bcmltd_fields_t *)ptr_math;

    if (lta_handler->validate != NULL) {
        rv = lta_handler->validate(unit,
                                   BCMLT_OPCODE_INSERT,
                                   in_fields,
                                   lta_handler->arg);

        if (rv == SHR_E_BADID) {
            /* Key fields are invalid.  Convert error code. */
            rv = SHR_E_PARAM;
        } else if (rv != SHR_E_EXISTS) {
            /* Entry exists error takes priority over table full */
            if (lt_state->entry_count >= lt_state->max_entry_count) {
                SHR_ERR_EXIT(SHR_E_FULL);
            }
        }
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    if (lta_handler->op_insert_alloc != NULL) {
        bcmltd_op_arg_t op_arg;

        lta_table_handler_op_arg_init(&op_arg, lt_entry);
        SHR_IF_ERR_VERBOSE_EXIT
            (lta_handler->op_insert_alloc(unit,
                                          &op_arg,
                                          in_fields,
                                          out_fields,
                                          lta_handler->arg));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (lta_handler->insert_alloc(unit,
                                       lt_entry->trans_id,
                                       in_fields,
                                       out_fields,
                                       lta_handler->arg));
    }

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                               BCMLRD_FIELD_TABLE_HANDLER_ERROR_COUNT);
    }
    SHR_FUNC_EXIT();
}

int
bcmltm_ee_node_lta_lookup(int unit,
                          bcmltm_lt_md_t *lt_md,
                          bcmltm_lt_state_t *lt_state,
                          bcmltm_entry_t *lt_entry,
                          uint32_t *ltm_buffer,
                          void *node_cookie)
{
    bcmltm_lta_table_params_t *lta_table_params;
    const bcmltd_table_handler_t *lta_handler;
    bcmltd_fields_t *in_fields, *out_fields;
    uint32_t *ptr_math;

    SHR_FUNC_ENTER(unit);

    lta_table_params = BCMLTM_LTA_TABLE_PARAMS(node_cookie);
    lta_handler = lta_table_params->handler;
    ptr_math = ltm_buffer + lta_table_params->fields_input_offset;
    in_fields = (bcmltd_fields_t *)ptr_math;
    ptr_math = ltm_buffer + lta_table_params->fields_output_offset;
    out_fields = (bcmltd_fields_t *)ptr_math;

    if (lta_handler->validate != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (lta_handler->validate(unit,
                                   BCMLT_OPCODE_LOOKUP,
                                   in_fields,
                                   lta_handler->arg));
    }

    if (lta_handler->op_lookup != NULL) {
        bcmltd_op_arg_t op_arg;

        lta_table_handler_op_arg_init(&op_arg, lt_entry);
        SHR_IF_ERR_VERBOSE_EXIT
            (lta_handler->op_lookup(unit,
                                    &op_arg,
                                    in_fields,
                                    out_fields,
                                    lta_handler->arg));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (lta_handler->lookup(unit,
                                 lt_entry->trans_id,
                                 in_fields,
                                 out_fields,
                                 lta_handler->arg));
    }

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                               BCMLRD_FIELD_TABLE_HANDLER_ERROR_COUNT);
    }
    SHR_FUNC_EXIT();
}

int
bcmltm_ee_node_lta_delete(int unit,
                          bcmltm_lt_md_t *lt_md,
                          bcmltm_lt_state_t *lt_state,
                          bcmltm_entry_t *lt_entry,
                          uint32_t *ltm_buffer,
                          void *node_cookie)
{
    bcmltm_lta_table_params_t *lta_table_params;
    const bcmltd_table_handler_t *lta_handler;
    bcmltd_fields_t *in_fields;
    uint32_t *ptr_math;

    SHR_FUNC_ENTER(unit);

    lta_table_params = BCMLTM_LTA_TABLE_PARAMS(node_cookie);
    lta_handler = lta_table_params->handler;
    ptr_math = ltm_buffer + lta_table_params->fields_input_offset;
    in_fields = (bcmltd_fields_t *)ptr_math;

    if (lta_handler->validate != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (lta_handler->validate(unit,
                                   BCMLT_OPCODE_DELETE,
                                   in_fields,
                                   lta_handler->arg));
    }

    if (lta_handler->op_delete != NULL) {
        bcmltd_op_arg_t op_arg;

        lta_table_handler_op_arg_init(&op_arg, lt_entry);
        SHR_IF_ERR_VERBOSE_EXIT
            (lta_handler->op_delete(unit,
                                    &op_arg,
                                    in_fields,
                                    NULL,
                                    lta_handler->arg));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (lta_handler->delete(unit,
                                 lt_entry->trans_id,
                                 in_fields,
                                 lta_handler->arg));
    }

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                               BCMLRD_FIELD_TABLE_HANDLER_ERROR_COUNT);
    }
    SHR_FUNC_EXIT();
}

int
bcmltm_ee_node_lta_update(int unit,
                          bcmltm_lt_md_t *lt_md,
                          bcmltm_lt_state_t *lt_state,
                          bcmltm_entry_t *lt_entry,
                          uint32_t *ltm_buffer,
                          void *node_cookie)
{
    int rv = SHR_E_NOT_FOUND;
    bcmltm_lta_table_params_t *lta_table_params;
    const bcmltd_table_handler_t *lta_handler;
    bcmltd_fields_t *in_fields;
    uint32_t *ptr_math;
    bool validate_error = FALSE;

    SHR_FUNC_ENTER(unit);

    lta_table_params = BCMLTM_LTA_TABLE_PARAMS(node_cookie);
    lta_handler = lta_table_params->handler;
    ptr_math = ltm_buffer + lta_table_params->fields_input_offset;
    in_fields = (bcmltd_fields_t *)ptr_math;

    if (lta_handler->validate != NULL) {
        rv = lta_handler->validate(unit,
                                   BCMLT_OPCODE_UPDATE,
                                   in_fields,
                                   lta_handler->arg);
        if (SHR_FAILURE(rv) && rv != SHR_E_NOT_FOUND) {
            /* Check for missing key error */
            if (lta_table_params->lta_key_list != NULL) {
                validate_error =
                    bcmltm_node_cth_validate_keys(unit, lt_md,
                                                  lt_entry->in_fields,
                                    lta_table_params->lta_key_list);
            }
            /* Diagnose value error */
            if (!validate_error &&
                (lta_table_params->lta_field_list != NULL)) {
                validate_error =
                    bcmltm_node_cth_validate_stat(unit, lt_md,
                                                  lt_entry->in_fields,
                                    lta_table_params->lta_field_list);
            }
        }
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    if (lta_handler->op_update != NULL) {
        bcmltd_op_arg_t op_arg;

        lta_table_handler_op_arg_init(&op_arg, lt_entry);
        SHR_IF_ERR_VERBOSE_EXIT
            (lta_handler->op_update(unit,
                                    &op_arg,
                                    in_fields,
                                    NULL,
                                    lta_handler->arg));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (lta_handler->update(unit,
                                 lt_entry->trans_id,
                                 in_fields,
                                 lta_handler->arg));
    }

 exit:
    if (SHR_FUNC_ERR() && !validate_error) {
        bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                               BCMLRD_FIELD_TABLE_HANDLER_ERROR_COUNT);
    }
    SHR_FUNC_EXIT();
}


int
bcmltm_ee_node_lta_traverse(int unit,
                            bcmltm_lt_md_t *lt_md,
                            bcmltm_lt_state_t *lt_state,
                            bcmltm_entry_t *lt_entry,
                            uint32_t *ltm_buffer,
                            void *node_cookie)
{
    bcmltm_lta_table_params_t *lta_table_params;
    const bcmltd_table_handler_t *lta_handler;
    bcmltd_fields_t *in_fields, *out_fields;
    uint32_t *ptr_math;

    SHR_FUNC_ENTER(unit);

    lta_table_params = BCMLTM_LTA_TABLE_PARAMS(node_cookie);
    lta_handler = lta_table_params->handler;
    ptr_math = ltm_buffer + lta_table_params->fields_input_offset;
    in_fields = (bcmltd_fields_t *)ptr_math;
    ptr_math = ltm_buffer + lta_table_params->fields_output_offset;
    out_fields = (bcmltd_fields_t *)ptr_math;

    if (lt_entry->in_fields != NULL) { /* TRAVERSE next */
        /* Continuing traverse */
        if (lta_handler->validate != NULL) {
            SHR_IF_ERR_VERBOSE_EXIT
                (lta_handler->validate(unit,
                                       BCMLT_OPCODE_TRAVERSE,
                                       in_fields,
                                       lta_handler->arg));
        }

        if (lta_handler->op_next != NULL) {
            bcmltd_op_arg_t op_arg;

            lta_table_handler_op_arg_init(&op_arg, lt_entry);
            SHR_IF_ERR_VERBOSE_EXIT
                (lta_handler->op_next(unit,
                                      &op_arg,
                                      in_fields,
                                      out_fields,
                                      lta_handler->arg));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (lta_handler->traverse->next(unit,
                                             lt_entry->trans_id,
                                             in_fields,
                                             out_fields,
                                             lta_handler->arg));
        }
    } else {
        /* Start traverse */
        if (lta_handler->op_first != NULL) {
            bcmltd_op_arg_t op_arg;

            lta_table_handler_op_arg_init(&op_arg, lt_entry);
            SHR_IF_ERR_VERBOSE_EXIT
                (lta_handler->op_first(unit,
                                       &op_arg,
                                       NULL,
                                       out_fields,
                                       lta_handler->arg));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (lta_handler->traverse->first(unit,
                                              lt_entry->trans_id,
                                              out_fields,
                                              lta_handler->arg));
        }
    }

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                               BCMLRD_FIELD_TABLE_HANDLER_ERROR_COUNT);
    }
    SHR_FUNC_EXIT();
}
