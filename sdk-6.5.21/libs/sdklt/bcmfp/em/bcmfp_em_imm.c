/*! \file bcmfp_em_imm.c
 *
 * IMM call back registration for different LTs present in
 * Ingress Field Processor stage.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_em_internal.h>
#include <bcmfp/bcmfp_trans_internal.h>
#include <bcmfp/bcmfp_cth_common.h>
#include <bcmfp/bcmfp_types_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

/*!
 * \brief FP group IMM table change callback function for staging.
 *
 * Handle FP group IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] LT operation arg
 * \param [in] event_reason Call back reason.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
bcmfp_em_grp_config_stage(int unit,
                               bcmltd_sid_t sid,
                               const bcmltd_op_arg_t *op_arg,
                               bcmimm_entry_event_t event_reason,
                               const bcmltd_field_t *key,
                               const bcmltd_field_t *data,
                               void *context,
                               bcmltd_fields_t *output_fields)
{
    bcmfp_stage_id_t stage_id = BCMFP_STAGE_ID_EXACT_MATCH;

    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }
    if ((event_reason != BCMIMM_ENTRY_DELETE) &&
            (data == NULL)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ltd_buffers_clear(unit, stage_id));

    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_group_config_insert(unit,
                                           op_arg,
                                           sid,
                                           stage_id,
                                           key,
                                           data,
                                           output_fields));
            break;
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_group_config_update(unit,
                                           op_arg,
                                           sid,
                                           stage_id,
                                           key,
                                           data,
                                           output_fields));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_group_config_delete(unit,
                                           op_arg,
                                           sid,
                                           stage_id,
                                           key,
                                           data,
                                           output_fields));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief FP rule IMM table change callback function for staging.
 *
 * Handle FP rule IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] LT operation arg
 * \param [in] event_reason Call back reason.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
bcmfp_em_rule_config_stage(int unit,
                                bcmltd_sid_t sid,
                                const bcmltd_op_arg_t *op_arg,
                                bcmimm_entry_event_t event_reason,
                                const bcmltd_field_t *key,
                                const bcmltd_field_t *data,
                                void *context,
                                bcmltd_fields_t *output_fields)
{
    bcmfp_stage_id_t stage_id = BCMFP_STAGE_ID_EXACT_MATCH;

    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ltd_buffers_clear(unit, stage_id));

    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_rule_config_insert(unit,
                                          op_arg,
                                          sid,
                                          stage_id,
                                          key,
                                          data,
                                          output_fields));
            break;
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_rule_config_update(unit,
                                          op_arg,
                                          sid,
                                          stage_id,
                                          key,
                                          data,
                                          output_fields));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_rule_config_delete(unit,
                                          op_arg,
                                          sid,
                                          stage_id,
                                          key,
                                          data,
                                          output_fields));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief FP policy IMM table change callback function for staging.
 *
 * Handle FP policy IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] LT operation arg
 * \param [in] event_reason Call back reason.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
bcmfp_em_policy_config_stage(int unit,
                                  bcmltd_sid_t sid,
                                  const bcmltd_op_arg_t *op_arg,
                                  bcmimm_entry_event_t event_reason,
                                  const bcmltd_field_t *key,
                                  const bcmltd_field_t *data,
                                  void *context,
                                  bcmltd_fields_t *output_fields)
{
    bcmfp_stage_id_t stage_id = BCMFP_STAGE_ID_EXACT_MATCH;

    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ltd_buffers_clear(unit, stage_id));

    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_policy_config_insert(unit,
                                          op_arg,
                                          sid,
                                          stage_id,
                                          key,
                                          data,
                                          output_fields));
            break;
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_policy_config_update(unit,
                                          op_arg,
                                          sid,
                                          stage_id,
                                          key,
                                          data,
                                          output_fields));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_policy_config_delete(unit,
                                          op_arg,
                                          sid,
                                          stage_id,
                                          key,
                                          data,
                                          output_fields));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief FP entry IMM table change callback function for staging.
 *
 * Handle FP entry IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] LT operation arg
 * \param [in] event_reason Call back reason.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
bcmfp_em_entry_config_stage(int unit,
                                 bcmltd_sid_t sid,
                                 const bcmltd_op_arg_t *op_arg,
                                 bcmimm_entry_event_t event_reason,
                                 const bcmltd_field_t *key,
                                 const bcmltd_field_t *data,
                                 void *context,
                                 bcmltd_fields_t *output_fields)
{
    bcmfp_stage_id_t stage_id = BCMFP_STAGE_ID_EXACT_MATCH;

    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ltd_buffers_clear(unit, stage_id));

    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_entry_config_insert(unit,
                                          op_arg,
                                          sid,
                                          stage_id,
                                          key,
                                          data,
                                          output_fields));
            break;
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_entry_config_update(unit,
                                          op_arg,
                                          sid,
                                          stage_id,
                                          key,
                                          data,
                                          output_fields));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_entry_config_delete(unit,
                                          op_arg,
                                          sid,
                                          stage_id,
                                          key,
                                          data,
                                          output_fields));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief FP entry IMM table change callback function for lookup.
 *
 * Handle FP entry IMM table lookup event.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] LT operation arg
 * \param [in] context Is a pointer that was given during registration.
 * \param [in] lkup_type Indicates the type of lookup that is being performed.
 * \param [in] in This is a linked list of the input fields identifying
 * the entry.
 * \param [in] out This is a linked list of the data fields in the
 * entry.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
bcmfp_em_entry_lookup(int unit,
                           bcmltd_sid_t sid,
                           const bcmltd_op_arg_t *op_arg,
                           void *context,
                           bcmimm_lookup_type_t lkup_type,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ltd_buffers_clear(unit, BCMFP_STAGE_ID_EXACT_MATCH));

    if (op_arg->attrib & BCMLT_ENT_ATTR_GET_FROM_HW ||
        lkup_type == BCMIMM_LOOKUP) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_config_lookup(unit,
                                       op_arg,
                                       sid,
                                       BCMFP_STAGE_ID_EXACT_MATCH,
                                       lkup_type,
                                       in,
                                       out));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief FP presel entry IMM table change callback function for lookup.
 *
 * Handle FP presel entry IMM table lookup event.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] LT operation arg
 * \param [in] context Is a pointer that was given during registration.
 * \param [in] lkup_type Indicates the type of lookup that is being performed.
 * \param [in] in This is a linked list of the input fields identifying
 * the entry.
 * \param [in] out This is a linked list of the data fields in the
 * entry.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
bcmfp_em_pse_lookup(int unit,
                    bcmltd_sid_t sid,
                    const bcmltd_op_arg_t *op_arg,
                    void *context,
                    bcmimm_lookup_type_t lkup_type,
                    const bcmltd_fields_t *in,
                    bcmltd_fields_t *out)
{
    bcmfp_stage_id_t stage_id = BCMFP_STAGE_ID_EXACT_MATCH;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ltd_buffers_clear(unit, stage_id));

    if (op_arg->attrib & BCMLT_ENT_ATTR_GET_FROM_HW ||
        lkup_type == BCMIMM_LOOKUP) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_pse_config_lookup(unit,
                                     op_arg,
                                     sid,
                                     stage_id,
                                     lkup_type,
                                     in,
                                     out));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief FP presel entry IMM table change callback function for staging.
 *
 * Handle FP presel entry IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] LT operation arg
 * \param [in] event_reason Call back reason.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
bcmfp_em_pse_config_stage(int unit,
                                  bcmltd_sid_t sid,
                                  const bcmltd_op_arg_t *op_arg,
                                  bcmimm_entry_event_t event_reason,
                                  const bcmltd_field_t *key,
                                  const bcmltd_field_t *data,
                                  void *context,
                                  bcmltd_fields_t *output_fields)
{
    bcmfp_stage_id_t stage_id = BCMFP_STAGE_ID_EXACT_MATCH;

    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ltd_buffers_clear(unit, stage_id));

    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_pse_config_insert(unit,
                                                  op_arg,
                                                  sid,
                                                  stage_id,
                                                  key,
                                                  data,
                                                  output_fields));
            break;
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_pse_config_update(unit,
                                                  op_arg,
                                                  sid,
                                                  stage_id,
                                                  key,
                                                  data,
                                                  output_fields));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_pse_config_delete(unit,
                                                  op_arg,
                                                  sid,
                                                  stage_id,
                                                  key,
                                                  data,
                                                  output_fields));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief FP presel group IMM table change callback function for staging.
 *
 * Handle FP presel group IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] LT operation arg
 * \param [in] event_reason Call back reason.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
bcmfp_em_psg_config_stage(int unit,
                                  bcmltd_sid_t sid,
                                  const bcmltd_op_arg_t *op_arg,
                                  bcmimm_entry_event_t event_reason,
                                  const bcmltd_field_t *key,
                                  const bcmltd_field_t *data,
                                  void *context,
                                  bcmltd_fields_t *output_fields)
{
    bcmfp_stage_id_t stage_id = BCMFP_STAGE_ID_EXACT_MATCH;

    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ltd_buffers_clear(unit, stage_id));

    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_psg_config_insert(unit,
                                                  op_arg,
                                                  sid,
                                                  stage_id,
                                                  key,
                                                  data,
                                                  output_fields));
            break;
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_psg_config_update(unit,
                                                  op_arg,
                                                  sid,
                                                  stage_id,
                                                  key,
                                                  data,
                                                  output_fields));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_psg_config_delete(unit,
                                                  op_arg,
                                                  sid,
                                                  stage_id,
                                                  key,
                                                  data,
                                                  output_fields));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief FP pdd IMM table change callback function for staging.
 *
 * Handle FP pdd IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] LT operation arg
 * \param [in] event_reason Call back reason.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
bcmfp_em_pdd_config_stage(int unit,
                               bcmltd_sid_t sid,
                               const bcmltd_op_arg_t *op_arg,
                               bcmimm_entry_event_t event_reason,
                               const bcmltd_field_t *key,
                               const bcmltd_field_t *data,
                               void *context,
                               bcmltd_fields_t *output_fields)
{
    bcmfp_stage_id_t stage_id = BCMFP_STAGE_ID_EXACT_MATCH;

    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ltd_buffers_clear(unit, stage_id));

    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_pdd_config_insert(unit,
                                         op_arg,
                                         sid,
                                         stage_id,
                                         key,
                                         data,
                                         output_fields));
            break;
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_pdd_config_update(unit,
                                         op_arg,
                                         sid,
                                         stage_id,
                                         key,
                                         data,
                                         output_fields));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_pdd_config_delete(unit,
                                         op_arg,
                                         sid,
                                         stage_id,
                                         key,
                                         data,
                                         output_fields));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief FP sbr IMM table change callback function for staging.
 *
 * Handle FP sbr IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] LT operation arg
 * \param [in] event_reason Call back reason.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
bcmfp_em_sbr_config_stage(int unit,
                               bcmltd_sid_t sid,
                               const bcmltd_op_arg_t *op_arg,
                               bcmimm_entry_event_t event_reason,
                               const bcmltd_field_t *key,
                               const bcmltd_field_t *data,
                               void *context,
                               bcmltd_fields_t *output_fields)
{
    bcmfp_stage_id_t stage_id = BCMFP_STAGE_ID_EXACT_MATCH;

    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ltd_buffers_clear(unit, stage_id));

    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_sbr_config_insert(unit,
                                         op_arg,
                                         sid,
                                         stage_id,
                                         key,
                                         data,
                                         output_fields));
            break;
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_sbr_config_update(unit,
                                         op_arg,
                                         sid,
                                         stage_id,
                                         key,
                                         data,
                                         output_fields));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_sbr_config_delete(unit,
                                         op_arg,
                                         sid,
                                         stage_id,
                                         key,
                                         data,
                                         output_fields));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief FP source class mode IMM table change callback function for staging.
 *
 * Handle FP source class mode IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] LT operation arg
 * \param [in] event_reason Call back reason.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
bcmfp_em_src_class_stage(int unit,
                         bcmltd_sid_t sid,
                         const bcmltd_op_arg_t *op_arg,
                         bcmimm_entry_event_t event_reason,
                         const bcmltd_field_t *key,
                         const bcmltd_field_t *data,
                         void *context,
                         bcmltd_fields_t *output_fields)
{
    bcmfp_stage_id_t stage_id = BCMFP_STAGE_ID_EXACT_MATCH;

    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ltd_buffers_clear(unit, stage_id));

    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_src_class_insert(unit,
                                        op_arg,
                                        sid,
                                        stage_id,
                                        key,
                                        data,
                                        output_fields));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_src_class_delete(unit,
                                        op_arg,
                                        sid,
                                        stage_id,
                                        key,
                                        data,
                                        output_fields));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief FP range check group IMM table change callback function for staging.
 *
 * Handle FP range check group IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] LT operation arg
 * \param [in] event_reason Call back reason.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
bcmfp_em_range_check_group_stage(int unit,
                                 bcmltd_sid_t sid,
                                 const bcmltd_op_arg_t *op_arg,
                                 bcmimm_entry_event_t event_reason,
                                 const bcmltd_field_t *key,
                                 const bcmltd_field_t *data,
                                 void *context,
                                 bcmltd_fields_t *output_fields)
{
    bcmfp_stage_id_t stage_id = BCMFP_STAGE_ID_EXACT_MATCH;

    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ltd_buffers_clear(unit, stage_id));

    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_range_check_group_insert(unit,
                                                op_arg,
                                                sid,
                                                stage_id,
                                                key,
                                                data,
                                                output_fields));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_range_check_group_delete(unit,
                                                op_arg,
                                                sid,
                                                stage_id,
                                                key,
                                                data,
                                                output_fields));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}
/*!
 * \brief FP em group LT entry IMM event callback structure.
 *
 * This structure contains callback functions that will be conresponding
 * to FP em group LT entry commit stages.
 */
static bcmimm_lt_cb_t bcmfp_em_grp_config_imm_callback = {

  /*! Validate function. */
  .validate = NULL,

  /*! Staging function. */
  .op_stage = bcmfp_em_grp_config_stage,

  /*! Commit function. */
  .commit = bcmfp_em_trans_commit,

  /*! Abort function. */
  .abort = bcmfp_em_trans_abort
};

/*!
 * \brief FP em rule LT IMM event callback structure.
 *
 * This structure contains callback functions that will be conresponding
 * to FP em rule LT entry commit stages.
 */
static bcmimm_lt_cb_t bcmfp_em_rule_config_imm_callback = {

  /*! Validate function. */
  .validate = NULL,

  /*! Staging function. */
  .op_stage = bcmfp_em_rule_config_stage,

  /*! Commit function. */
  .commit = bcmfp_em_trans_commit,

  /*! Abort function. */
  .abort = bcmfp_em_trans_abort
};

/*!
 * \brief FP em policy LT IMM event callback structure.
 *
 * This structure contains callback functions that will be conresponding
 * to FP em policy LT entry commit stages.
 */
static bcmimm_lt_cb_t bcmfp_em_policy_config_imm_callback = {

  /*! Validate function. */
  .validate = NULL,

  /*! Staging function. */
  .op_stage = bcmfp_em_policy_config_stage,

  /*! Commit function. */
  .commit = bcmfp_em_trans_commit,

  /*! Abort function. */
  .abort = bcmfp_em_trans_abort
};

/*!
 * \brief FP em entry LT IMM event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to FP em entry LT entry commit stages.
 */
static bcmimm_lt_cb_t bcmfp_em_entry_config_imm_callback = {

  /*! Validate function. */
  .validate = NULL,

  /*! Staging function. */
  .op_stage = bcmfp_em_entry_config_stage,

  /*! Entry Limit function. */
  .ent_limit_get = bcmfp_ptm_stage_entry_limit_get,

  /*! Commit function. */
  .commit = bcmfp_em_trans_commit,

  /*! Abort function. */
  .abort = bcmfp_em_trans_abort,

  /*! Lookup function. */
  .op_lookup = bcmfp_em_entry_lookup
};

/*!
 * \brief FP em presel entry LT IMM event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to FP em presel entry LT entry commit stages.
 */
static bcmimm_lt_cb_t bcmfp_em_pse_imm_callback = {

  /*! Validate function. */
  .validate = NULL,

  /*! Staging function. */
  .op_stage = bcmfp_em_pse_config_stage,

  /*! Commit function. */
  .commit = bcmfp_em_trans_commit,

  /*! Abort function. */
  .abort = bcmfp_em_trans_abort,

   /*! Lookup function. */
  .op_lookup = bcmfp_em_pse_lookup
};

/*!
 * \brief FP em presel group LT IMM event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to FP em presel group LT entry commit stages.
 */
static bcmimm_lt_cb_t bcmfp_em_psg_imm_callback = {

  /*! Validate function. */
  .validate = NULL,

  /*! Staging function. */
  .op_stage = bcmfp_em_psg_config_stage,

  /*! Commit function. */
  .commit = bcmfp_em_trans_commit,

  /*! Abort function. */
  .abort = bcmfp_em_trans_abort
};

/*!
 * \brief FP em PDD LT IMM event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to FP em PDD LT entry commit stages.
 */
static bcmimm_lt_cb_t bcmfp_em_pdd_imm_callback = {

  /*! Validate function. */
  .validate = NULL,

  /*! Staging function. */
  .op_stage = bcmfp_em_pdd_config_stage,

  /*! Commit function. */
  .commit = bcmfp_em_trans_commit,

  /*! Abort function. */
  .abort = bcmfp_em_trans_abort
};

/*!
 * \brief FP em SBR LT IMM event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to FP em SBR LT entry commit stages.
 */
static bcmimm_lt_cb_t bcmfp_em_sbr_imm_callback = {

  /*! Validate function. */
  .validate = NULL,

  /*! Staging function. */
  .op_stage = bcmfp_em_sbr_config_stage,

  /*! Commit function. */
  .commit = bcmfp_em_trans_commit,

  /*! Abort function. */
  .abort = bcmfp_em_trans_abort
};

/*!
 * \brief FP em source class mode LT IMM event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to FP em source class mode LT entry commit stages.
 */
static bcmimm_lt_cb_t bcmfp_em_src_class_imm_callback = {

  /*! Validate function. */
  .validate = NULL,

  /*! Staging function. */
  .op_stage = bcmfp_em_src_class_stage,

  /*! Commit function. */
  .commit = bcmfp_em_trans_commit,

  /*! Abort function. */
  .abort = bcmfp_em_trans_abort
};


/*!
 * \brief FP em range check group LT IMM event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to FP em range check group LT entry commit stages.
 */
static bcmimm_lt_cb_t bcmfp_em_range_check_group_imm_callback = {

  /*! Validate function. */
  .validate = NULL,

  /*! Staging function. */
  .op_stage = bcmfp_em_range_check_group_stage,

  /*! Commit function. */
  .commit = NULL,

  /*! Abort function. */
  .abort = NULL
};

int
bcmfp_em_imm_register(int unit)
{
    int rv;
    bcmfp_stage_t *stage = NULL;
    const bcmlrd_map_t *map = NULL;
    bcmimm_lt_cb_t *cb = NULL;
    bcmltd_sid_t tbl_id = BCMLTD_SID_INVALID;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, BCMFP_STAGE_ID_EXACT_MATCH, &stage));
    if (stage == NULL) {
        SHR_EXIT();
    }

    /* Register callbacks for em group LT. */
    if (stage->tbls.group_tbl != NULL) {
        tbl_id = stage->tbls.group_tbl->sid;
        rv = bcmlrd_map_get(unit, tbl_id, &map);
        if (SHR_SUCCESS(rv)) {
            cb = &bcmfp_em_grp_config_imm_callback;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmimm_lt_event_reg(unit, tbl_id, cb, NULL));
        }
    }

    /* Register callbacks for em rule LT. */
    if (stage->tbls.rule_tbl != NULL) {
        tbl_id = stage->tbls.rule_tbl->sid;
        rv = bcmlrd_map_get(unit, tbl_id, &map);
        if (SHR_SUCCESS(rv)) {
            cb = &bcmfp_em_rule_config_imm_callback;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmimm_lt_event_reg(unit, tbl_id, cb, NULL));
        }
    }

    /* Register callbacks for em policy LT. */
    if (stage->tbls.policy_tbl != NULL) {
        tbl_id = stage->tbls.policy_tbl->sid;
        rv = bcmlrd_map_get(unit, tbl_id, &map);
        if (SHR_SUCCESS(rv)) {
            cb = &bcmfp_em_policy_config_imm_callback;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmimm_lt_event_reg(unit, tbl_id, cb, NULL));
        }
    }

    /* Register callbacks for em entry LT. */
    if (stage->tbls.entry_tbl != NULL) {
        tbl_id = stage->tbls.entry_tbl->sid;
        rv = bcmlrd_map_get(unit, tbl_id, &map);
        if (SHR_SUCCESS(rv)) {
            cb = &bcmfp_em_entry_config_imm_callback;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmimm_lt_event_reg(unit, tbl_id, cb, NULL));
        }
    }

    /* Register callbacks for em presel entry LT. */
    if (stage->tbls.pse_tbl != NULL) {
        tbl_id = stage->tbls.pse_tbl->sid;
        rv = bcmlrd_map_get(unit, tbl_id, &map);
        if (SHR_SUCCESS(rv)) {
            cb = &bcmfp_em_pse_imm_callback;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmimm_lt_event_reg(unit, tbl_id, cb, NULL));
        }
    }

    /* Register callbacks for em presel group LT. */
    if (stage->tbls.psg_tbl != NULL) {
        tbl_id = stage->tbls.psg_tbl->sid;
        rv = bcmlrd_map_get(unit, tbl_id, &map);
        if (SHR_SUCCESS(rv)) {
            cb = &bcmfp_em_psg_imm_callback;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmimm_lt_event_reg(unit, tbl_id, cb, NULL));
        }
    }

    /* Register callbacks for em pdd LT. */
    if (stage->tbls.pdd_tbl != NULL) {
        tbl_id = stage->tbls.pdd_tbl->sid;
        rv = bcmlrd_map_get(unit, tbl_id, &map);
        if (SHR_SUCCESS(rv)) {
            cb = &bcmfp_em_pdd_imm_callback;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmimm_lt_event_reg(unit, tbl_id, cb, NULL));
        }
    }

    /* Register callbacks for em sbr LT. */
    if (stage->tbls.sbr_tbl != NULL) {
        tbl_id = stage->tbls.sbr_tbl->sid;
        rv = bcmlrd_map_get(unit, tbl_id, &map);
        if (SHR_SUCCESS(rv)) {
            cb = &bcmfp_em_sbr_imm_callback;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmimm_lt_event_reg(unit, tbl_id, cb, NULL));
        }
    }

    /* Register callbacks for em source class mode LT. */
    if (stage->tbls.src_class_tbl != NULL) {
        tbl_id = stage->tbls.src_class_tbl->sid;
        rv = bcmlrd_map_get(unit, tbl_id, &map);
        if (SHR_SUCCESS(rv)) {
            cb = &bcmfp_em_src_class_imm_callback;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmimm_lt_event_reg(unit, tbl_id, cb, NULL));
        }
    }

    /* Register callbacks for em range check group LT. */
    if (stage->tbls.range_check_group_tbl != NULL) {
        tbl_id = stage->tbls.range_check_group_tbl->sid;
        rv = bcmlrd_map_get(unit, tbl_id, &map);
        if (SHR_SUCCESS(rv)) {
            cb = &bcmfp_em_range_check_group_imm_callback;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmimm_lt_event_reg(unit, tbl_id, cb, NULL));
        }
    }

exit:
    SHR_FUNC_EXIT();
}
