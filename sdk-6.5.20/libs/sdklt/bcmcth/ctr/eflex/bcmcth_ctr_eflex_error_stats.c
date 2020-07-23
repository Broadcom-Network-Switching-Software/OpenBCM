/*! \file bcmcth_ctr_eflex_error_stats.c
 *
 * Custom tabler handler routines for the error stats LT for enhanced flex
 * counter.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <bcmltd/chip/bcmltd_id.h>

#include <bcmptm/bcmptm.h>
#include <bcmcth/bcmcth_ctr_eflex_drv.h>
#include <bcmcth/bcmcth_state_eflex_drv.h>
#include <bcmcth/bcmcth_ctr_eflex_internal.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_CTREFLEX

/*! Generate CTR_xxx_EFLEX_ERROR_STATS LT field ids. */
#define CTR_ING_EGR_ERROR_STATS_FIDS(x) \
    CTR_ING_EFLEX_ERROR_STATSt_##x, \
    CTR_EGR_EFLEX_ERROR_STATSt_##x

/*! Generate STATE_xxx_EFLEX_ERROR_STATS LT field ids. */
#define STATE_ING_EGR_ERROR_STATS_FIDS(x) \
    FLEX_STATE_ING_ERROR_STATSt_##x, \
    FLEX_STATE_EGR_ERROR_STATSt_##x

/*******************************************************************************
 * Local definitions
 */
static const bcmlrd_fid_t pipe_fid[][2] = {
        {CTR_ING_EGR_ERROR_STATS_FIDS(PIPEf)},
        {STATE_ING_EGR_ERROR_STATS_FIDS(PIPEf)}
};

static const bcmlrd_fid_t instance_fid[][2] = {
        {0, 0},
        {FLEX_STATE_ING_ERROR_STATSt_INSTANCEf, 0},
};

static const bcmlrd_fid_t egr_instance_fid[][2] = {
        {0, 0},
        {0, FLEX_STATE_EGR_ERROR_STATSt_INSTANCEf },
};

static const bcmlrd_fid_t action_misconfig_fid[][2] = {
        {CTR_ING_EGR_ERROR_STATS_FIDS(ACTION_MISCONFIGf)},
        {STATE_ING_EGR_ERROR_STATS_FIDS(ACTION_MISCONFIGf)},
};

static const bcmlrd_fid_t too_many_actions_fid[][2] = {
        {CTR_ING_EGR_ERROR_STATS_FIDS(TOO_MANY_ACTIONSf)},
        {STATE_ING_EGR_ERROR_STATS_FIDS(TOO_MANY_ACTIONSf)},
};

/**********************************************************************
* Private functions
 */
/*! Function to parse input for key fields */
static int
bcmcth_ctr_eflex_error_stats_input_parse(int unit,
                                         const bcmltd_fields_t *in,
                                         ctr_eflex_error_stats_data_t *entry)
{
    int select;
    uint32_t i;
    bcmltd_field_t *data_field = NULL;
    eflex_comp_t comp;

    SHR_FUNC_ENTER(unit);

    select = entry->ingress ? 0 : 1;
    comp = entry->comp;

    /* Get all the key fields */
    for (i = 0; i < in->count; i++) {
        data_field = in->field[i];
        if (data_field->id == pipe_fid[comp][select]) {
            entry->pipe_idx = data_field->data;
        } else if (data_field->id == instance_fid[comp][select]) {
            entry->inst = data_field->data;
        } else if (data_field->id == egr_instance_fid[comp][select]) {
            entry->egr_inst = data_field->data;
        }
    }

    SHR_FUNC_EXIT();
}

/*! Function to lookup error stats IMM LT entry. */
static int
bcmcth_ctr_eflex_error_stats_lookup(int unit,
                                    bool ingress,
                                    eflex_comp_t comp,
                                    bcmimm_lookup_type_t lkup_type,
                                    const bcmltd_fields_t *in,
                                    bcmltd_fields_t *out)
{
    ctr_eflex_error_stats_data_t entry = {0};
    const bcmltd_fields_t *key_fields = NULL;
    uint32_t i = 0;
    ctr_eflex_control_t *ctrl = NULL;
    int select;

    SHR_FUNC_ENTER(unit);

    select = ingress ? 0 : 1;

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    entry.ingress = ingress;
    entry.comp = comp;
    entry.pipe_idx = CTR_EFLEX_INVALID_VAL;
    entry.inst = CTR_EFLEX_INVALID_VAL;

    /* key fields are in the "out" parameter for traverse operations */
    key_fields = (lkup_type == BCMIMM_TRAVERSE) ? out : in;

    /* Parse key fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_error_stats_input_parse(unit,
                                                  key_fields,
                                                  &entry));

    /* Determine first and last pipe number */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_num_pipes_get(unit,
                                        entry.ingress,
                                        entry.comp,
                                        entry.pipe_idx,
                                        &entry.start_pipe_idx,
                                        &entry.end_pipe_idx));

    if (entry.comp == CTR_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_error_stats_read(unit, &entry));
    } else if (entry.comp == STATE_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_state_eflex_error_stats_read(unit, &entry));
    }

    for (i = 0; i < out->count; i++) {
        out->field[i]->idx = 0;
        out->field[i]->flags = 0;
        out->field[i]->next = NULL;
        if (out->field[i]->id == action_misconfig_fid[comp][select]) {
            out->field[i]->data = entry.action_misconfig;
        } else if (out->field[i]->id == too_many_actions_fid[comp][select]) {
            out->field[i]->data = entry.too_many_actions;
        } else {
            /* Do nothing. */
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Stage function to insert,update, delete IMM LT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] ingress Ingress or Egress direction.
 * \param [in] comp    Enhanced Flex component.
 * \param [in] event_reason Reason for the entry event.
 * \param [in] key_fields Linked list of the key fields identifying
 *                        the entry.
 * \param [in] data_fields Linked list of the data fields in the
 *                         modified entry.
 * \param [out] output_fields Linked list of the output fields in the
 *                            modified entry.
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_ctr_eflex_error_stats_lt_stage(int unit,
                                      bool ingress,
                                      eflex_comp_t comp,
                                      bcmimm_entry_event_t event_reason,
                                      const bcmltd_field_t *key_fields,
                                      const bcmltd_field_t *data_fields,
                                      bcmltd_fields_t *output_fields)
{
    ctr_eflex_error_stats_data_t entry = {0};
    ctr_eflex_control_t *ctrl = NULL;
    int select;

    SHR_FUNC_ENTER(unit);
    select = ingress ? 0 : 1;

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    entry.ingress = ingress;
    entry.comp = comp;
    entry.pipe_idx = CTR_EFLEX_INVALID_VAL;
    entry.inst = CTR_EFLEX_INVALID_VAL;

    /* Parse key fields */
    while (key_fields) {
        if (key_fields->id == pipe_fid[comp][select]) {
            entry.pipe_idx = key_fields->data;
        } else if (key_fields->id == instance_fid[comp][select]) {
            entry.inst = key_fields->data;
        } else if (key_fields->id == egr_instance_fid[comp][select]) {
            entry.egr_inst = key_fields->data;
        }
        key_fields = key_fields->next;
    }

    /* Determine first and last pipe number */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_num_pipes_get(unit,
                                        entry.ingress,
                                        entry.comp,
                                        entry.pipe_idx,
                                        &entry.start_pipe_idx,
                                        &entry.end_pipe_idx));

    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            entry.ingress,
                                            entry.comp,
                                            &entry.pipe_unique));

    if (event_reason == BCMIMM_ENTRY_INSERT ||
        event_reason == BCMIMM_ENTRY_UPDATE) {
        /* Parse the data fields */
        while (data_fields) {
            if (data_fields->id == action_misconfig_fid[comp][select]) {
                entry.action_misconfig = data_fields->data;
                entry.upd_action_misconfig = true;
            } else if (data_fields->id == too_many_actions_fid[comp][select]) {
                entry.too_many_actions = data_fields->data;
                entry.upd_too_many_actions = true;
            } else {
                /* Do nothing */
            }
            data_fields = data_fields->next;
        }

        if (event_reason == BCMIMM_ENTRY_INSERT) {
            entry.upd_action_misconfig = true;
            entry.upd_too_many_actions = true;
        }

        if (entry.comp == CTR_EFLEX) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_error_stats_write(unit, &entry));
        } else if (entry.comp == STATE_EFLEX) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_state_eflex_error_stats_write(unit, &entry));
        }
    }

    if (output_fields) {
        output_fields->count = 0;
    }

exit:
    SHR_FUNC_EXIT();
}

/*! Function to get ERROR_STATS IMM LT entry limit value. */
static int
bcmcth_ctr_eflex_error_stats_lt_ent_limit_get(int unit,
                                              bool ingress,
                                              eflex_comp_t comp,
                                              uint32_t *count)
{
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;
    uint8_t pipe_unique;
    uint32_t num_pipes, num_instances = 1;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));
    device_info = ctrl->ctr_eflex_device_info;

    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            ingress,
                                            comp,
                                            &pipe_unique));

    num_pipes = pipe_unique ? device_info->num_pipes : 1;

    if (comp == STATE_EFLEX && ingress == INGRESS) {
        num_instances = device_info->state_ext.num_ingress_instance;
    } else if (comp == STATE_EFLEX && ingress == EGRESS) {
        num_instances = device_info->state_ext.num_egress_instance;
    }

    /* Calculate the entry limit */
    *count = num_pipes * num_instances;

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit,"comp %d ingress %d entry_limit %d\n"),
        comp, ingress, *count));

exit:
    SHR_FUNC_EXIT();
}

/***********************
 * INGRESS IMM Functions
 */

/*!
 * \brief Lookup callback function of CTR_ING_EFLEX_ERROR_STATS IMM event
 *        handler.
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] trans_id Transaction ID associated with this operation.
 * \param [in] context Pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 * \param [in] lkup_type Indicates the type of lookup that is being performed.
 * \param [in] in Structure which contains the key fields but may include
 *                also other fields. The component should only focus on the
 *                key fields.
 * \param [in,out] out Structure contains all the fields of the table, so that
 *                     the component should not delete or add any field to this
 *                     structure. The data values of the field were set by the
 *                     IMM so the component may only overwrite the data section
 *                     of the fields.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_ctr_eflex_ing_error_stats_lt_lookup_cb(int unit,
                                              bcmltd_sid_t sid,
                                              uint32_t trans_id,
                                              void *context,
                                              bcmimm_lookup_type_t lkup_type,
                                              const bcmltd_fields_t *in,
                                              bcmltd_fields_t *out)
{
    uint32_t t;
    ctr_eflex_control_t *ctrl;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);
    if (lkup_type == BCMIMM_LOOKUP) {
        SHR_NULL_CHECK(in, SHR_E_PARAM);
    }
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, CTR_EFLEX, &ctrl));

    for (t = 0; t < ctrl->lt_info_num; t++) {
        if (ctrl->ctr_eflex_field_info[t].lt_sid ==
            CTR_ING_EFLEX_ERROR_STATSt) {
            break;
        }
    }

    if (out) {
        if (t == ctrl->lt_info_num) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        } else if (out->count <
                   ctrl->ctr_eflex_field_info[t].num_data_fields) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "out fields supplied to lookup is too "
                                  "small to hold the desired fields.\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
               (BSL_META_U(unit,
                           "out not supplied to lookup.\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_error_stats_lookup(unit,
                                             INGRESS,
                                             CTR_EFLEX,
                                             lkup_type,
                                             in,
                                             out));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Stage callback function of CTR_ING_EFLEX_ERROR_STATS to insert,
 *        update, delete IMM LT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] trans_id Transaction ID associated with this operation.
 * \param [in] event_reason Reason for the entry event.
 * \param [in] key_fields Linked list of the key fields identifying
 *                        the entry.
 * \param [in] data_fields Linked list of the data fields in the
 *                         modified entry.
 * \param [in] context Pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 * \param [out] output_fields Linked list of the output fields in the
 *                            modified entry.
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_ctr_eflex_ing_error_stats_lt_stage_cb(int unit,
                                             bcmltd_sid_t sid,
                                             uint32_t trans_id,
                                             bcmimm_entry_event_t event_reason,
                                             const bcmltd_field_t *key_fields,
                                             const bcmltd_field_t *data_fields,
                                             void *context,
                                             bcmltd_fields_t *output_fields)
{
    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);
    if (event_reason != BCMIMM_ENTRY_DELETE) {
        SHR_NULL_CHECK(output_fields, SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_error_stats_lt_stage(unit,
                                               INGRESS,
                                               CTR_EFLEX,
                                               event_reason,
                                               key_fields,
                                               data_fields,
                                               output_fields));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Entry limit get callback function of CTR_ING_EFLEX_ERROR_STATS
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] count Entry limit value.
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_ctr_eflex_ing_error_stats_lt_ent_limit_get_cb(int unit,
                                                     uint32_t trans_id,
                                                     bcmltd_sid_t sid,
                                                     uint32_t *count)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_error_stats_lt_ent_limit_get(unit,
                                                       INGRESS,
                                                       CTR_EFLEX,
                                                       count));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Lookup callback function of FLEX_STATE_ING_ERROR_STATS IMM event
 *        handler.
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] trans_id Transaction ID associated with this operation.
 * \param [in] context Pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 * \param [in] lkup_type Indicates the type of lookup that is being performed.
 * \param [in] in Structure which contains the key fields but may include
 *                also other fields. The component should only focus on the
 *                key fields.
 * \param [in,out] out Structure contains all the fields of the table, so that
 *                     the component should not delete or add any field to this
 *                     structure. The data values of the field were set by the
 *                     IMM so the component may only overwrite the data section
 *                     of the fields.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_state_eflex_ing_error_stats_lt_lookup_cb(int unit,
                                                bcmltd_sid_t sid,
                                                uint32_t trans_id,
                                                void *context,
                                                bcmimm_lookup_type_t lkup_type,
                                                const bcmltd_fields_t *in,
                                                bcmltd_fields_t *out)
{
    uint32_t t;
    ctr_eflex_control_t *ctrl;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);
    if (lkup_type == BCMIMM_LOOKUP) {
        SHR_NULL_CHECK(in, SHR_E_PARAM);
    }
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, STATE_EFLEX, &ctrl));

    for (t = 0; t < ctrl->lt_info_num; t++) {
        if (ctrl->ctr_eflex_field_info[t].lt_sid ==
            FLEX_STATE_ING_ERROR_STATSt) {
            break;
        }
    }

    if (out) {
        if (t == ctrl->lt_info_num) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        } else if (out->count <
                   ctrl->ctr_eflex_field_info[t].num_data_fields) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "out fields supplied to lookup is too "
                                  "small to hold the desired fields.\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
               (BSL_META_U(unit,
                           "out not supplied to lookup.\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_error_stats_lookup(unit,
                                             INGRESS,
                                             STATE_EFLEX,
                                             lkup_type,
                                             in,
                                             out));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Stage callback function of FLEX_STATE_ING_ERROR_STATS to insert,
 *        update, delete IMM LT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] trans_id Transaction ID associated with this operation.
 * \param [in] event_reason Reason for the entry event.
 * \param [in] key_fields Linked list of the key fields identifying
 *                        the entry.
 * \param [in] data_fields Linked list of the data fields in the
 *                         modified entry.
 * \param [in] context Pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 * \param [out] output_fields Linked list of the output fields in the
 *                            modified entry.
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_state_eflex_ing_error_stats_lt_stage_cb(
    int unit,
    bcmltd_sid_t sid,
    uint32_t trans_id,
    bcmimm_entry_event_t event_reason,
    const bcmltd_field_t *key_fields,
    const bcmltd_field_t *data_fields,
    void *context,
    bcmltd_fields_t *output_fields)
{
    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);
    if (event_reason != BCMIMM_ENTRY_DELETE) {
        SHR_NULL_CHECK(output_fields, SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_error_stats_lt_stage(unit,
                                               INGRESS,
                                               STATE_EFLEX,
                                               event_reason,
                                               key_fields,
                                               data_fields,
                                               output_fields));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Entry limit get callback function of FLEX_STATE_ING_ERROR_STATS
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] count Entry limit value.
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_state_eflex_ing_error_stats_lt_ent_limit_get_cb(int unit,
                                                       uint32_t trans_id,
                                                       bcmltd_sid_t sid,
                                                       uint32_t *count)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_error_stats_lt_ent_limit_get(unit,
                                                       INGRESS,
                                                       STATE_EFLEX,
                                                       count));

exit:
    SHR_FUNC_EXIT();
}

/***********************
 * EGRESS IMM Functions
 */

/*!
 * \brief Lookup callback function of CTR_EGR_EFLEX_ERROR_STATS IMM event
 *        handler.
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] trans_id Transaction ID associated with this operation.
 * \param [in] context Pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 * \param [in] lkup_type Indicates the type of lookup that is being performed.
 * \param [in] in Structure which contains the key fields but may include
 *                also other fields. The component should only focus on the
 *                key fields.
 * \param [in,out] out Structure contains all the fields of the table, so that
 *                     the component should not delete or add any field to this
 *                     structure. The data values of the field were set by the
 *                     IMM so the component may only overwrite the data section
 *                     of the fields.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_ctr_eflex_egr_error_stats_lt_lookup_cb(int unit,
                                              bcmltd_sid_t sid,
                                              uint32_t trans_id,
                                              void *context,
                                              bcmimm_lookup_type_t lkup_type,
                                              const bcmltd_fields_t *in,
                                              bcmltd_fields_t *out)
{
    uint32_t t;
    ctr_eflex_control_t *ctrl;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);
    if (lkup_type == BCMIMM_LOOKUP) {
        SHR_NULL_CHECK(in, SHR_E_PARAM);
    }
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, CTR_EFLEX, &ctrl));

    for (t = 0; t < ctrl->lt_info_num; t++) {
        if (ctrl->ctr_eflex_field_info[t].lt_sid ==
            CTR_EGR_EFLEX_ERROR_STATSt) {
            break;
        }
    }

    if (out) {
        if (t == ctrl->lt_info_num) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        } else if (out->count <
                   ctrl->ctr_eflex_field_info[t].num_data_fields) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "out fields supplied to lookup is too "
                                  "small to hold the desired fields.\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
               (BSL_META_U(unit,
                           "out not supplied to lookup.\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_error_stats_lookup(unit,
                                             EGRESS,
                                             CTR_EFLEX,
                                             lkup_type,
                                             in,
                                             out));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Stage callback function of CTR_EGR_EFLEX_ERROR_STATS to insert,
 *        update, delete IMM LT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] trans_id Transaction ID associated with this operation.
 * \param [in] event_reason Reason for the entry event.
 * \param [in] key_fields Linked list of the key fields identifying
 *                        the entry.
 * \param [in] data_fields Linked list of the data fields in the
 *                         modified entry.
 * \param [in] context Pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 * \param [out] output_fields Linked list of the output fields in the
 *                            modified entry.
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_ctr_eflex_egr_error_stats_lt_stage_cb(int unit,
                                             bcmltd_sid_t sid,
                                             uint32_t trans_id,
                                             bcmimm_entry_event_t event_reason,
                                             const bcmltd_field_t *key_fields,
                                             const bcmltd_field_t *data_fields,
                                             void *context,
                                             bcmltd_fields_t *output_fields)
{
    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);
    if (event_reason != BCMIMM_ENTRY_DELETE) {
        SHR_NULL_CHECK(output_fields, SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_error_stats_lt_stage(unit,
                                               EGRESS,
                                               CTR_EFLEX,
                                               event_reason,
                                               key_fields,
                                               data_fields,
                                               output_fields));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Entry limit get callback function of CTR_EGR_EFLEX_ERROR_STATS
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] count Entry limit value.
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_ctr_eflex_egr_error_stats_lt_ent_limit_get_cb(int unit,
                                                     uint32_t trans_id,
                                                     bcmltd_sid_t sid,
                                                     uint32_t *count)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_error_stats_lt_ent_limit_get(unit,
                                                       EGRESS,
                                                       CTR_EFLEX,
                                                       count));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Lookup callback function of FLEX_STATE_EGR_ERROR_STATS IMM event
 *        handler.
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] trans_id Transaction ID associated with this operation.
 * \param [in] context Pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 * \param [in] lkup_type Indicates the type of lookup that is being performed.
 * \param [in] in Structure which contains the key fields but may include
 *                also other fields. The component should only focus on the
 *                key fields.
 * \param [in,out] out Structure contains all the fields of the table, so that
 *                     the component should not delete or add any field to this
 *                     structure. The data values of the field were set by the
 *                     IMM so the component may only overwrite the data section
 *                     of the fields.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_state_eflex_egr_error_stats_lt_lookup_cb(int unit,
                                                bcmltd_sid_t sid,
                                                uint32_t trans_id,
                                                void *context,
                                                bcmimm_lookup_type_t lkup_type,
                                                const bcmltd_fields_t *in,
                                                bcmltd_fields_t *out)
{
    uint32_t t;
    ctr_eflex_control_t *ctrl;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);
    if (lkup_type == BCMIMM_LOOKUP) {
        SHR_NULL_CHECK(in, SHR_E_PARAM);
    }
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, STATE_EFLEX, &ctrl));

    for (t = 0; t < ctrl->lt_info_num; t++) {
        if (ctrl->ctr_eflex_field_info[t].lt_sid ==
            FLEX_STATE_EGR_ERROR_STATSt) {
            break;
        }
    }

    if (out) {
        if (t == ctrl->lt_info_num) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        } else if (out->count <
                   ctrl->ctr_eflex_field_info[t].num_data_fields) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "out fields supplied to lookup is too "
                                  "small to hold the desired fields.\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
               (BSL_META_U(unit,
                           "out not supplied to lookup.\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_error_stats_lookup(unit,
                                             EGRESS,
                                             STATE_EFLEX,
                                             lkup_type,
                                             in,
                                             out));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Stage callback function of FLEX_STATE_EGR_ERROR_STATS to insert,
 *        update, delete IMM LT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] trans_id Transaction ID associated with this operation.
 * \param [in] event_reason Reason for the entry event.
 * \param [in] key_fields Linked list of the key fields identifying
 *                        the entry.
 * \param [in] data_fields Linked list of the data fields in the
 *                         modified entry.
 * \param [in] context Pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 * \param [out] output_fields Linked list of the output fields in the
 *                            modified entry.
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_state_eflex_egr_error_stats_lt_stage_cb(
    int unit,
    bcmltd_sid_t sid,
    uint32_t trans_id,
    bcmimm_entry_event_t event_reason,
    const bcmltd_field_t *key_fields,
    const bcmltd_field_t *data_fields,
    void *context,
    bcmltd_fields_t *output_fields)
{
    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);
    if (event_reason != BCMIMM_ENTRY_DELETE) {
        SHR_NULL_CHECK(output_fields, SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_error_stats_lt_stage(unit,
                                               EGRESS,
                                               STATE_EFLEX,
                                               event_reason,
                                               key_fields,
                                               data_fields,
                                               output_fields));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Entry limit get callback function of FLEX_STATE_EGR_ERROR_STATS
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] count Entry limit value.
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_state_eflex_egr_error_stats_lt_ent_limit_get_cb(int unit,
                                                       uint32_t trans_id,
                                                       bcmltd_sid_t sid,
                                                       uint32_t *count)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_error_stats_lt_ent_limit_get(unit,
                                                       EGRESS,
                                                       STATE_EFLEX,
                                                       count));

exit:
    SHR_FUNC_EXIT();
}

/**********************************************************************
* Public functions
 */

/* Function to register callback handlers for CTR_xxx_EFLEX_ERROR_STATS LTs */
int
bcmcth_ctr_eflex_imm_error_stats_register(int unit)
{
    bcmimm_lt_cb_t ctr_eflex_lt_cb;

    SHR_FUNC_ENTER(unit);

    /* Register CTR_ING_EFLEX_ERROR_STATS handler */
    sal_memset(&ctr_eflex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
    ctr_eflex_lt_cb.stage  = bcmcth_ctr_eflex_ing_error_stats_lt_stage_cb;
    ctr_eflex_lt_cb.lookup = bcmcth_ctr_eflex_ing_error_stats_lt_lookup_cb;
    ctr_eflex_lt_cb.ent_limit_get  =
        bcmcth_ctr_eflex_ing_error_stats_lt_ent_limit_get_cb;

    SHR_IF_ERR_EXIT_EXCEPT_IF(
        bcmimm_lt_event_reg(unit,
                            CTR_ING_EFLEX_ERROR_STATSt,
                            &ctr_eflex_lt_cb,
                            NULL), SHR_E_PARAM);

    /* Register CTR_EGR_EFLEX_ERROR_STATS handler */
    sal_memset(&ctr_eflex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
    ctr_eflex_lt_cb.stage  = bcmcth_ctr_eflex_egr_error_stats_lt_stage_cb;
    ctr_eflex_lt_cb.lookup = bcmcth_ctr_eflex_egr_error_stats_lt_lookup_cb;
    ctr_eflex_lt_cb.ent_limit_get  =
        bcmcth_ctr_eflex_egr_error_stats_lt_ent_limit_get_cb;

    SHR_IF_ERR_EXIT_EXCEPT_IF(
        bcmimm_lt_event_reg(unit,
                            CTR_EGR_EFLEX_ERROR_STATSt,
                            &ctr_eflex_lt_cb,
                            NULL), SHR_E_PARAM);

exit:
    SHR_FUNC_EXIT();
}

/* Function to register callback handlers for STATE_xxx_EFLEX_ERROR_STATS LTs */
int
bcmcth_state_eflex_imm_error_stats_register(int unit)
{
    bcmimm_lt_cb_t state_eflex_lt_cb;

    SHR_FUNC_ENTER(unit);

    /* Register FLEX_STATE_ING_ERROR_STATS handler */
    sal_memset(&state_eflex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
    state_eflex_lt_cb.stage  = bcmcth_state_eflex_ing_error_stats_lt_stage_cb;
    state_eflex_lt_cb.lookup = bcmcth_state_eflex_ing_error_stats_lt_lookup_cb;
    state_eflex_lt_cb.ent_limit_get  =
        bcmcth_state_eflex_ing_error_stats_lt_ent_limit_get_cb;

    SHR_IF_ERR_EXIT_EXCEPT_IF(
        bcmimm_lt_event_reg(unit,
                            FLEX_STATE_ING_ERROR_STATSt,
                            &state_eflex_lt_cb,
                            NULL), SHR_E_PARAM);

    /* Register FLEX_STATE_EGR_ERROR_STATS handler */
    sal_memset(&state_eflex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
    state_eflex_lt_cb.stage  = bcmcth_state_eflex_egr_error_stats_lt_stage_cb;
    state_eflex_lt_cb.lookup = bcmcth_state_eflex_egr_error_stats_lt_lookup_cb;
    state_eflex_lt_cb.ent_limit_get  =
        bcmcth_state_eflex_egr_error_stats_lt_ent_limit_get_cb;

    SHR_IF_ERR_EXIT_EXCEPT_IF(
        bcmimm_lt_event_reg(unit,
                            FLEX_STATE_EGR_ERROR_STATSt,
                            &state_eflex_lt_cb,
                            NULL), SHR_E_PARAM);

exit:
    SHR_FUNC_EXIT();
}
