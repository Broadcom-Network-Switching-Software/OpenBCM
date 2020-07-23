/*! \file bcmcth_ctr_eflex_trigger.c
 *
 * BCMCTH CTR_EFLEX_TRIGGERt IMM handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmbd/bcmbd_ts.h>

#include <bcmptm/bcmptm.h>
#include <bcmcth/bcmcth_ctr_eflex_drv.h>
#include <bcmcth/bcmcth_state_eflex_drv.h>
#include <bcmcth/bcmcth_ctr_eflex_internal.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_CTREFLEX

/*! Generate CTR_xxx_EFLEX_TRIGGER LT field ids. */
#define CTR_ING_EGR_TRIGGER_FIDS(x) \
    CTR_ING_EFLEX_TRIGGERt_##x, \
    CTR_EGR_EFLEX_TRIGGERt_##x

/*! Generate STATE_xxx_EFLEX_TRIGGER LT field ids. */
#define STATE_ING_EGR_TRIGGER_FIDS(x) \
    FLEX_STATE_ING_TRIGGERt_##x, \
    FLEX_STATE_EGR_TRIGGERt_##x

/*******************************************************************************
 * Local definitions
 */

/* TRIGGER LT field info */
static const bcmlrd_sid_t trigger_table_sid[][2] = {
        {CTR_ING_EGR_LT_IDS(EFLEX_TRIGGERt)},
        {STATE_ING_EGR_LT_IDS(TRIGGERt)},
};

static const bcmlrd_fid_t trigger_action_profile_id_fid[][2] = {
       {CTR_ING_EFLEX_TRIGGERt_CTR_ING_EFLEX_ACTION_PROFILE_IDf,
        CTR_EGR_EFLEX_TRIGGERt_CTR_EGR_EFLEX_ACTION_PROFILE_IDf},
       {FLEX_STATE_ING_TRIGGERt_FLEX_STATE_ING_ACTION_PROFILE_IDf,
        FLEX_STATE_EGR_TRIGGERt_FLEX_STATE_EGR_ACTION_PROFILE_IDf},
};

static const bcmlrd_fid_t trigger_fid[][2] = {
        {CTR_ING_EGR_TRIGGER_FIDS(TRIGGERf)},
        {STATE_ING_EGR_TRIGGER_FIDS(TRIGGERf)},
};

static const bcmlrd_fid_t start_fid[][2] = {
        {CTR_ING_EGR_TRIGGER_FIDS(STARTf)},
        {STATE_ING_EGR_TRIGGER_FIDS(STARTf)},
};

static const bcmlrd_fid_t stop_fid[][2] = {
        {CTR_ING_EGR_TRIGGER_FIDS(STOPf)},
        {STATE_ING_EGR_TRIGGER_FIDS(STOPf)},
};

static const bcmlrd_fid_t start_value_fid[][2] = {
        {CTR_ING_EGR_TRIGGER_FIDS(START_VALUEf)},
        {STATE_ING_EGR_TRIGGER_FIDS(START_VALUEf)},
};

static const bcmlrd_fid_t stop_value_fid[][2] = {
        {CTR_ING_EGR_TRIGGER_FIDS(STOP_VALUEf)},
        {STATE_ING_EGR_TRIGGER_FIDS(STOP_VALUEf)},
};

static const bcmlrd_fid_t cond_mask_fid[][2] = {
        {CTR_ING_EGR_TRIGGER_FIDS(COND_MASKf)},
        {STATE_ING_EGR_TRIGGER_FIDS(COND_MASKf)},
};

static const bcmlrd_fid_t start_time_fid[][2] = {
        {CTR_ING_EGR_TRIGGER_FIDS(START_TIME_OFFSETf)},
        {STATE_ING_EGR_TRIGGER_FIDS(START_TIME_OFFSETf)},
};

static const bcmlrd_fid_t num_period_fid[][2] = {
        {CTR_ING_EGR_TRIGGER_FIDS(NUM_PERIODf)},
        {STATE_ING_EGR_TRIGGER_FIDS(NUM_PERIODf)},
};

static const bcmlrd_fid_t num_actions_fid[][2] = {
        {CTR_ING_EGR_TRIGGER_FIDS(NUM_ACTIONSf)},
        {STATE_ING_EGR_TRIGGER_FIDS(NUM_ACTIONSf)},
};

static const bcmlrd_fid_t compare_stop_fid[][2] = {
        {CTR_ING_EGR_TRIGGER_FIDS(COMPARE_STOPf)},
        {STATE_ING_EGR_TRIGGER_FIDS(COMPARE_STOPf)},
};

static const bcmlrd_fid_t compare_start_fid[][2] = {
        {CTR_ING_EGR_TRIGGER_FIDS(COMPARE_STARTf)},
        {STATE_ING_EGR_TRIGGER_FIDS(COMPARE_STARTf)},
};

static const bcmlrd_fid_t scale_fid[][2] = {
        {CTR_ING_EGR_TRIGGER_FIDS(SCALEf)},
        {STATE_ING_EGR_TRIGGER_FIDS(SCALEf)},
};

static const bcmlrd_fid_t interval_shift_fid[][2] = {
        {CTR_ING_EGR_TRIGGER_FIDS(INTERVAL_SHIFTf)},
        {0, 0},
};

static const bcmlrd_fid_t interval_size_fid[][2] = {
        {CTR_ING_EGR_TRIGGER_FIDS(INTERVAL_SIZEf)},
        {0, 0},
};

static const bcmlrd_fid_t oper_state_fid[][2] = {
        {CTR_ING_EGR_TRIGGER_FIDS(OPERATIONAL_STATEf)},
        {STATE_ING_EGR_TRIGGER_FIDS(OPERATIONAL_STATEf)},
};

/**********************************************************************
* Private functions
 */
/* Function to initialize TRIGGER entry with invalid values */
static int
bcmcth_ctr_eflex_trigger_entry_init(
    int unit,
    ctr_eflex_trigger_data_t *entry)
{
    SHR_FUNC_ENTER(unit);

    entry->pipe_idx = CTR_EFLEX_INVALID_VAL;
    entry->inst = CTR_EFLEX_INVALID_VAL;
    entry->trigger = CTR_EFLEX_INVALID_VAL;
    entry->start = CTR_EFLEX_INVALID_VAL;
    entry->stop = CTR_EFLEX_INVALID_VAL;
    entry->start_value = CTR_EFLEX_INVALID_VAL;
    entry->stop_value = CTR_EFLEX_INVALID_VAL;
    entry->cond_mask = CTR_EFLEX_INVALID_VAL;
    entry->start_time = CTR_EFLEX_INVALID_VAL;
    entry->num_periods = CTR_EFLEX_INVALID_VAL;
    entry->num_actions = CTR_EFLEX_INVALID_VAL;
    entry->compare_stop = 0;
    entry->compare_start = 0;
    entry->scale = CTR_EFLEX_INVALID_VAL;
    entry->interval_shift = CTR_EFLEX_INVALID_VAL;
    entry->interval_size = CTR_EFLEX_INVALID_VAL;

    SHR_FUNC_EXIT();
}

/* Function to get default values for TRIGGER entry */
static int
bcmcth_ctr_eflex_trigger_default_get(
    int unit,
    ctr_eflex_trigger_data_t *entry)
{
    bcmlrd_client_field_info_t *f_info = NULL;
    const bcmlrd_table_rep_t *lt_info = NULL;
    size_t num_fid = 0, i;
    int select;
    eflex_comp_t comp;

    SHR_FUNC_ENTER(unit);

    select  = entry->ingress ? 0 : 1;
    comp = entry->comp;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit,
                                trigger_table_sid[comp][select],
                                &num_fid));

    lt_info = bcmlrd_table_get(trigger_table_sid[comp][select]);
    SHR_NULL_CHECK(lt_info, SHR_E_BADID);

    SHR_ALLOC(f_info, sizeof(bcmlrd_client_field_info_t) * num_fid,
              "bcmcthCtrEflexTriggerAllocFields");
    SHR_NULL_CHECK(f_info, SHR_E_MEMORY);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_table_fields_def_get(unit,
                                     lt_info->name,
                                     num_fid,
                                     f_info,
                                     &num_fid));

    for (i = 0; i < num_fid; i++) {
        if (f_info[i].id == trigger_fid[comp][select]) {
            entry->trigger = f_info[i].def.u8;
        } else if (f_info[i].id == start_fid[comp][select]) {
            entry->start = (ctr_eflex_start_trigger_t)f_info[i].def.u32;
        } else if (f_info[i].id == stop_fid[comp][select]) {
            entry->stop = (ctr_eflex_stop_trigger_t)f_info[i].def.u32;
        } else if (f_info[i].id == start_value_fid[comp][select]) {
            entry->start_value = f_info[i].def.u32;
        } else if (f_info[i].id == stop_value_fid[comp][select]) {
            entry->stop_value = f_info[i].def.u32;
        } else if (f_info[i].id == cond_mask_fid[comp][select]) {
            entry->cond_mask = f_info[i].def.u32;
        } else if (f_info[i].id == start_time_fid[comp][select]) {
            entry->start_time = f_info[i].def.u64;
        } else if (f_info[i].id == num_period_fid[comp][select]) {
            entry->num_periods = f_info[i].def.u16;
        } else if (f_info[i].id == scale_fid[comp][select]) {
            entry->scale = (ctr_eflex_scale_t)f_info[i].def.u32;
        } else if (f_info[i].id == interval_shift_fid[comp][select]) {
            entry->interval_shift = f_info[i].def.u16;
        } else if (f_info[i].id == interval_size_fid[comp][select]) {
            entry->interval_size = f_info[i].def.u16;
        } else if (f_info[i].id == num_actions_fid[comp][select]) {
            entry->num_actions = f_info[i].def.u16;
        } else if (f_info[i].id == compare_stop_fid[comp][select]) {
            entry->compare_stop = f_info[i].def.is_true;
        } else if (f_info[i].id == compare_start_fid[comp][select]) {
            entry->compare_start = f_info[i].def.is_true;
        }
    }

exit:
    SHR_FREE(f_info);
    SHR_FUNC_EXIT();
}

/*
 * Function to replace TRIGGER entry with values from another
 * entry if is invalid
 */
static int
bcmcth_ctr_eflex_trigger_fill(int unit,
                              ctr_eflex_trigger_data_t *entry,
                              ctr_eflex_trigger_data_t *cur_entry)
{
    SHR_FUNC_ENTER(unit);

    if (entry->trigger == CTR_EFLEX_INVALID_VAL) {
        entry->trigger = cur_entry->trigger;
    }

    if (entry->start == CTR_EFLEX_INVALID_VAL) {
        entry->start = cur_entry->start;
    }

    if (entry->stop == CTR_EFLEX_INVALID_VAL) {
        entry->stop = cur_entry->stop;
    }

    if (entry->start_value == CTR_EFLEX_INVALID_VAL) {
        entry->start_value = cur_entry->start_value;
    }

    if (entry->stop_value == CTR_EFLEX_INVALID_VAL) {
        entry->stop_value = cur_entry->stop_value;
    }

    if (entry->cond_mask == CTR_EFLEX_INVALID_VAL) {
        entry->cond_mask = cur_entry->cond_mask;
    }

    if (entry->start_time == CTR_EFLEX_INVALID_VAL) {
        entry->start_time = cur_entry->start_time;
    }

    if (entry->num_periods == CTR_EFLEX_INVALID_VAL) {
        entry->num_periods = cur_entry->num_periods;
    }

    if (entry->num_actions == CTR_EFLEX_INVALID_VAL) {
        entry->num_actions = cur_entry->num_actions;
    }

    if (entry->compare_start == 0) {
        entry->compare_start = cur_entry->compare_start;
    }

    if (entry->compare_stop == 0) {
        entry->compare_stop = cur_entry->compare_stop;
    }

    if (entry->scale == CTR_EFLEX_INVALID_VAL) {
        entry->scale = cur_entry->scale;
    }

    if (entry->interval_shift == CTR_EFLEX_INVALID_VAL) {
        entry->interval_shift = cur_entry->interval_shift;
    }

    if (entry->interval_size == CTR_EFLEX_INVALID_VAL) {
        entry->interval_size = cur_entry->interval_size;
    }

    SHR_FUNC_EXIT();
}

/* Function to parse data fields in given TRIGGER entry */
static int
bcmcth_ctr_eflex_trigger_lt_fields_check(
    int unit,
    ctr_eflex_trigger_data_t *entry,
    const bcmltd_field_t *data_fields)
{
    int select;
    eflex_comp_t comp;

    SHR_FUNC_ENTER(unit);

    select = entry->ingress ? 0 : 1;
    comp = entry->comp;

    if (data_fields->id == trigger_fid[comp][select]) {
        entry->trigger = data_fields->data;
    } else if (data_fields->id == start_fid[comp][select]) {
        entry->start = (ctr_eflex_start_trigger_t)data_fields->data;
    } else if (data_fields->id == stop_fid[comp][select]) {
        entry->stop = (ctr_eflex_stop_trigger_t)data_fields->data;
    } else if (data_fields->id == start_value_fid[comp][select]) {
        entry->start_value = data_fields->data;
    } else if (data_fields->id == stop_value_fid[comp][select]) {
        entry->stop_value = data_fields->data;
    } else if (data_fields->id == cond_mask_fid[comp][select]) {
        entry->cond_mask = data_fields->data;
    } else if (data_fields->id == start_time_fid[comp][select]) {
        entry->start_time = data_fields->data;
    } else if (data_fields->id == num_period_fid[comp][select]) {
        entry->num_periods = data_fields->data;
    } else if (data_fields->id == scale_fid[comp][select]) {
        entry->scale = (ctr_eflex_scale_t)data_fields->data;
    } else if (data_fields->id == interval_shift_fid[comp][select]) {
        entry->interval_shift = data_fields->data;
     } else if (data_fields->id == interval_size_fid[comp][select]) {
        entry->interval_size = data_fields->data;
    } else if (data_fields->id == oper_state_fid[comp][select]) {
        entry->oper_state = data_fields->data;
    } else if (data_fields->id == num_actions_fid[comp][select]) {
        entry->num_actions = data_fields->data;
    } else if (data_fields->id == compare_start_fid[comp][select]) {
        entry->compare_start = data_fields->data;
    } else if (data_fields->id == compare_stop_fid[comp][select]) {
        entry->compare_stop = data_fields->data;
    }

    SHR_FUNC_EXIT();
}

/* Function to parse given TRIGGER entry */
static int
bcmcth_ctr_eflex_trigger_lt_fields_parse(
    int unit,
    const bcmltd_field_t *data_fields,
    ctr_eflex_trigger_data_t *entry)
{
    SHR_FUNC_ENTER(unit);

    /* Parse data fields */
    while (data_fields) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_trigger_lt_fields_check(unit,
                                                      entry,
                                                      data_fields));

        data_fields = data_fields->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to parse TRIGGER IMM entry */
static int
bcmcth_ctr_eflex_trigger_entry_parse(
    int unit,
    ctr_eflex_trigger_data_t *entry,
    bcmltd_fields_t *flist)
{
    uint32_t i;
    bcmltd_field_t *data_fields = NULL;

    SHR_FUNC_ENTER(unit);

    /* extract data fields */
    for (i = 0; i < flist->count; i++) {
        data_fields = flist->field[i];

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_trigger_lt_fields_check(unit,
                                                      entry,
                                                      data_fields));
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to update OPERATIONAL_STATE in TRIGGER IMM LT */
static int
bcmcth_ctr_eflex_trigger_oper_state_update(
    int unit,
    ctr_eflex_trigger_data_t *entry)
{
    bcmltd_fields_t key_fields = {0};
    bcmltd_fields_t imm_fields = {0};
    size_t num_fields = 0, num_key_fields, num_readonly_fields;
    int select;
    uint32_t t;
    ctr_eflex_control_t *ctrl = NULL;
    eflex_comp_t comp;

    SHR_FUNC_ENTER(unit);
    select = entry->ingress ? 0 : 1;
    comp = entry->comp;

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    for (t = 0; t < ctrl->lt_info_num; t++) {
        if (ctrl->ctr_eflex_field_info[t].lt_sid ==
                trigger_table_sid[comp][select]) {
            break;
        }
    }

    if (t == ctrl->lt_info_num) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "TRIGGER LT info not found.\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    num_key_fields = ctrl->ctr_eflex_field_info[t].num_key_fields;
    num_readonly_fields = ctrl->ctr_eflex_field_info[t].num_read_only_fields;

    /* Allocate key field */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_fields_alloc(unit,
                                       &key_fields,
                                       (num_key_fields + num_readonly_fields)));

    key_fields.count = num_key_fields;
    key_fields.field[0]->id = trigger_action_profile_id_fid[comp][select];
    key_fields.field[0]->data = entry->action_profile_id;

    /* Allocate imm fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit,
                                trigger_table_sid[comp][select],
                                &num_fields));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_fields_alloc(unit, &imm_fields, num_fields));

    /* lookup IMM table; error if entry not found */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit,
                             trigger_table_sid[comp][select],
                             &key_fields,
                             &imm_fields));

    /* Set the fields.count to include OPERATIONAL_STATE */
    key_fields.count = num_key_fields + num_readonly_fields;
    key_fields.field[1]->id = oper_state_fid[comp][select];
    key_fields.field[1]->data = entry->oper_state;

    /* Update IMM entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_update(unit,
                             0,
                             trigger_table_sid[comp][select],
                             &key_fields));

exit:
    bcmcth_ctr_eflex_fields_free(unit, &key_fields);
    /* imm_fields.count may be modified by bcmimm_entry_lookup */
    imm_fields.count = num_fields;
    bcmcth_ctr_eflex_fields_free(unit, &imm_fields);

    SHR_FUNC_EXIT();
}

/*
 * Function to lookup TRIGGER table with given trigger id.
 * It populates the LT entry with all fields except the key field
 * which is provided within the entry as input.
 */
static int
bcmcth_ctr_eflex_trigger_lookup(int unit,
                                ctr_eflex_trigger_data_t *entry)
{
    bcmltd_fields_t key_fields = {0};
    bcmltd_fields_t imm_fields = {0};
    size_t num_fields = 0, num_key_fields;
    int select;
    uint32_t t;
    ctr_eflex_control_t *ctrl = NULL;
    eflex_comp_t comp;

    SHR_FUNC_ENTER(unit);

    select = entry->ingress ? 0 : 1;
    comp = entry->comp;

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    /* Assign default values */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_trigger_default_get(unit, entry));

    for (t = 0; t < ctrl->lt_info_num; t++) {
        if (ctrl->ctr_eflex_field_info[t].lt_sid ==
                trigger_table_sid[comp][select]) {
            break;
        }
    }

    if (t == ctrl->lt_info_num) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "TRIGGER LT info not found.\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    num_key_fields = ctrl->ctr_eflex_field_info[t].num_key_fields;

    /* Allocate key field */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_fields_alloc(unit,
                                       &key_fields,
                                       num_key_fields));

    key_fields.count = num_key_fields;
    key_fields.field[0]->id = trigger_action_profile_id_fid[comp][select];
    key_fields.field[0]->data = entry->action_profile_id;

    /* Allocate imm fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit,
                                trigger_table_sid[comp][select],
                                &num_fields));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_fields_alloc(unit, &imm_fields, num_fields));

    /* lookup IMM table; error if entry not found */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit,
                             trigger_table_sid[comp][select],
                             &key_fields,
                             &imm_fields));

    /* Parse IMM entry fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_trigger_entry_parse(unit, entry, &imm_fields));

exit:
    bcmcth_ctr_eflex_fields_free(unit, &key_fields);
    /* imm_fields.count may be modified by bcmimm_entry_lookup */
    imm_fields.count = num_fields;
    bcmcth_ctr_eflex_fields_free(unit, &imm_fields);

    SHR_FUNC_EXIT();
}

/* Function to help insert PT entry for TRIGGER LT */
static int
bcmcth_ctr_eflex_trigger_insert_helper(int unit,
                                       ctr_eflex_trigger_data_t *entry)
{
    uint64_t timestamp = 0;

    SHR_FUNC_ENTER(unit);

    /* Check for invalid combination of START and STOP type */
    if (entry->start == TIME && (entry->stop == STOP_CONDITION ||
                                   entry->stop == ACTION_COUNT)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Invalid START and STOP combination \n")));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Determine first and last pipe number */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_num_pipes_get(unit,
                                        entry->ingress,
                                        entry->comp,
                                        entry->pipe_idx,
                                        &entry->start_pipe_idx,
                                        &entry->end_pipe_idx));

    /* Check if trigger is enabled and start type is TIME */
    if (entry->start == TIME && entry->trigger == 1) {
        /* Get current timestamp */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmbd_ts_timestamp_nanosec_get(unit,
                                            BCMEFLEX_TS_INST,
                                            &timestamp));

        /* Add timestamp offset to current time */
        entry->start_time += timestamp;
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "Insert %s Entry:\n"
            "action_profile_id %d, action_id %d, pipe_idx %d, inst %d\n"
            "trigger %d, start %d, stop %d\n"
            "start_value %d, stop_value %d\n"
            "cond_mask %d, start_time 0x%" PRIx64 "\n"
            "num_actions %d, compare_start %d\n"
            "compare_stop %d\n"
            "num_periods %d, scale %d\n"),
            entry->ingress ? "Ingress" : "Egress",
            entry->action_profile_id, entry->action_id, entry->pipe_idx,
            entry->inst, entry->trigger, entry->start, entry->stop,
            entry->start_value, entry->stop_value,
            entry->cond_mask, entry->start_time,
            entry->num_actions, entry->compare_start,
            entry->compare_stop,
            entry->num_periods, entry->scale));

    /* Write to trigger table */
    if (entry->comp == CTR_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_trigger_write(unit, entry));
    } else if (entry->comp == STATE_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_state_eflex_trigger_write(unit, entry));
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to help insert PT entry for TRIGGER LT */
static int
bcmcth_ctr_eflex_trigger_delete_helper(int unit,
                                       ctr_eflex_trigger_data_t *entry)
{
    SHR_FUNC_ENTER(unit);

    /* Determine first and last pipe number */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_num_pipes_get(unit,
                                        entry->ingress,
                                        entry->comp,
                                        entry->pipe_idx,
                                        &entry->start_pipe_idx,
                                        &entry->end_pipe_idx));

    /* Get default values */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_trigger_default_get(unit, entry));

    /* Write to trigger table */
    if (entry->comp == CTR_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_trigger_write(unit, entry));
    } else if (entry->comp == STATE_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_state_eflex_trigger_write(unit, entry));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function to check if corresponding TRIGGER LT entry,
 * should be updated based on ACTION_PROFILE LT entry insert/delete operation.
 */
int
bcmcth_ctr_eflex_trigger_notify(
    int unit,
    bool insert,
    ctr_eflex_action_profile_data_t *action_entry)
{
    ctr_eflex_trigger_data_t entry = {0};
    bool oper_state;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(action_entry, SHR_E_PARAM);

    /*
     * If insert, update TRIGGER entry if it is INACTIVE
     * If delete, update TRIGGER entry if it is ACTIVE
     */
    oper_state = insert ? CTR_EFLEX_OPER_STATE_INACTIVE :
                          CTR_EFLEX_OPER_STATE_ACTIVE;

    entry.ingress = action_entry->ingress;
    entry.comp = action_entry->comp;
    entry.action_profile_id = action_entry->action_profile_id;
    entry.action_id = action_entry->action_id;
    entry.pipe_idx = action_entry->pipe_idx;
    entry.inst = action_entry->state_ext.inst;
    entry.egr_inst = action_entry->state_ext.egr_inst;

    /* Lookup entry with given key fields */
     SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_trigger_lookup(unit, &entry));

    /* Check if current OPERATIONAL_STATE matches expected value */
    if (entry.oper_state == oper_state) {
        if (insert) {
            /* Insert entry in PT */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_trigger_insert_helper(unit, &entry));

            /* Update TRIGGER LT entry OPERATIONAL_STATE to ACTIVE */
            entry.oper_state = CTR_EFLEX_OPER_STATE_ACTIVE;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_trigger_oper_state_update(unit, &entry));
        } else {
            /* Delete entry in PT */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_trigger_delete_helper(unit, &entry));

            /* Update TRIGGER LT entry OPERATIONAL_STATE to INACTIVE */
            entry.oper_state = CTR_EFLEX_OPER_STATE_INACTIVE;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_trigger_oper_state_update(unit, &entry));
        }
    }

exit:
    if (SHR_FUNC_VAL_IS(SHR_E_NOT_FOUND)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "Action profile Id %d entry not found in TRIGGER LT.\n"),
                entry.action_profile_id));
        return SHR_E_NONE;
    }
    SHR_FUNC_EXIT();
}

/* Function to insert entry into TRIGGER IMM LT */
static int
bcmcth_ctr_eflex_trigger_lt_entry_insert(
    int unit,
    const bcmltd_field_t *data_fields,
    ctr_eflex_trigger_data_t *entry)
{
    ctr_eflex_action_profile_data_t action_entry = {0};

    SHR_FUNC_ENTER(unit);

    action_entry.ingress = entry->ingress;
    action_entry.comp = entry->comp;
    action_entry.action_profile_id = entry->action_profile_id;

    /* Lookup entry with given action profile id key in ACTION_PROFILE LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_lookup(unit, &action_entry));

    /* Get pipe idx and action id from action profile entry */
    entry->pipe_idx = action_entry.pipe_idx;
    entry->action_id = action_entry.action_id;
    entry->inst = action_entry.state_ext.inst;
    entry->egr_inst = action_entry.state_ext.egr_inst;

    /* Get default values */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_trigger_default_get(unit, entry));

    /*
     * Parse user values
     * default values will be over-written by user values, if present
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_trigger_lt_fields_parse(unit,
                                                  data_fields,
                                                  entry));

    /* Insert entry in PT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_trigger_insert_helper(unit, entry));

exit:
    SHR_FUNC_EXIT();
}

/* Function to update entry into TRIGGER IMM LT */
static int
bcmcth_ctr_eflex_trigger_lt_entry_update(
    int unit,
    const bcmltd_field_t *data_fields,
    ctr_eflex_trigger_data_t *entry,
    ctr_eflex_trigger_data_t *cur_entry)
{
    uint64_t timestamp = 0;
    ctr_eflex_action_profile_data_t action_entry = {0};

    SHR_FUNC_ENTER(unit);

    action_entry.ingress = entry->ingress;
    action_entry.comp = entry->comp;
    action_entry.action_profile_id = entry->action_profile_id;

    /* Lookup entry with given action profile id key in ACTION_PROFILE LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_lookup(unit, &action_entry));

    /* Get pipe idx and action id from action profile entry */
    entry->pipe_idx = action_entry.pipe_idx;
    entry->action_id = action_entry.action_id;
    entry->inst = action_entry.state_ext.inst;
    entry->egr_inst = action_entry.state_ext.egr_inst;

    /* Parse user values */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_trigger_lt_fields_parse(unit,
                                                  data_fields,
                                                  entry));

    /* Determine first and last pipe number */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_num_pipes_get(unit,
                                        entry->ingress,
                                        entry->comp,
                                        entry->pipe_idx,
                                        &entry->start_pipe_idx,
                                        &entry->end_pipe_idx));

    cur_entry->action_profile_id = entry->action_profile_id;
    cur_entry->pipe_idx = entry->pipe_idx;
    cur_entry->inst = entry->inst;

    /* Lookup entry with given key fields */
     SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_trigger_lookup(unit, cur_entry));

    /* Fill missing fields in new entry with current entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_trigger_fill(unit, entry, cur_entry));

    /* Determine first and last pipe number */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_num_pipes_get(unit,
                                        cur_entry->ingress,
                                        cur_entry->comp,
                                        cur_entry->pipe_idx,
                                        &cur_entry->start_pipe_idx,
                                        &cur_entry->end_pipe_idx));

    /* Check for invalid combination of START and STOP type */
    if (entry->start == TIME &&  (entry->stop == STOP_CONDITION ||
                         entry->stop == ACTION_COUNT)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Invalid START and STOP combination \n")));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Check if trigger is enabled and start type is TIME */
    if (entry->start == TIME && entry->trigger == 1) {
        /* Get current timestamp */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmbd_ts_timestamp_nanosec_get(unit,
                                            BCMEFLEX_TS_INST,
                                            &timestamp));

        /* Add timestamp offset to current time */
        entry->start_time += timestamp;
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "Update %s Entry:\n"
            "action_profile_id %d, action_id %d, pipe_idx %d, inst %d\n"
            "trigger %d, start %d, stop %d\n"
            "start_value %d, stop_value %d\n"
            "cond_mask %d, start_time 0x%" PRIx64 "\n"
            "num_actions %d, compare_start %d\n"
            "compare_stop %d\n"
            "num_periods %d, scale %d\n"),
            entry->ingress ? "Ingress" : "Egress",
            entry->action_profile_id, entry->action_id, entry->pipe_idx,
            entry->inst, entry->trigger, entry->start, entry->stop,
            entry->start_value, entry->stop_value,
            entry->cond_mask, entry->start_time,
            entry->num_actions, entry->compare_start,
            entry->compare_stop,
            entry->num_periods, entry->scale));

    /* Write to trigger table */
    if (entry->comp == CTR_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_trigger_write(unit, entry));
    } else if (entry->comp == STATE_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_state_eflex_trigger_write(unit, entry));
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to delete entry from TRIGGER IMM LT */
static int
bcmcth_ctr_eflex_trigger_lt_entry_delete(
    int unit,
    const bcmltd_field_t *data_fields,
    ctr_eflex_trigger_data_t *entry)
{
    ctr_eflex_action_profile_data_t action_entry = {0};

    SHR_FUNC_ENTER(unit);

    /* Lookup TRIGGER entry to get operational state */
     SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_trigger_lookup(unit, entry));

    /* If entry is INACTIVE, no further action is needed */
    if (entry->oper_state == CTR_EFLEX_OPER_STATE_INACTIVE) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Action profile id %d entry is INACTIVE.\n"),
            entry->action_profile_id));
        SHR_EXIT();
    }

    action_entry.ingress = entry->ingress;
    action_entry.comp = entry->comp;
    action_entry.action_profile_id = entry->action_profile_id;

    /* Lookup entry with given action profile id key in ACTION_PROFILE LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_lookup(unit, &action_entry));

    /* Get pipe idx and action id from action profile entry */
    entry->pipe_idx = action_entry.pipe_idx;
    entry->action_id = action_entry.action_id;
    entry->inst = action_entry.state_ext.inst;
    entry->egr_inst = action_entry.state_ext.egr_inst;

    /* Delete entry in PT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_trigger_delete_helper(unit, entry));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_ctr_eflex_trigger_lt_stage(int unit,
                                  bool ingress,
                                  eflex_comp_t comp,
                                  uint64_t req_flags,
                                  bcmimm_entry_event_t event_reason,
                                  const bcmltd_field_t *key_fields,
                                  const bcmltd_field_t *data_fields,
                                  bcmltd_fields_t *out)
{
    int select;
    ctr_eflex_trigger_data_t entry = {0};
    ctr_eflex_trigger_data_t cur_entry = {0};
    ctr_eflex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);
    select = ingress ? 0 : 1;

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    entry.ingress = ingress;
    entry.comp = comp;
    entry.req_flags = req_flags;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_trigger_entry_init(unit, &entry));

    /* Parse the key fields to get action profile id */
    while (key_fields) {
        if (key_fields->id == trigger_action_profile_id_fid[comp][select]) {
            entry.action_profile_id = key_fields->data;
            break;
        }
        key_fields = key_fields->next;
    }

    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_trigger_lt_entry_insert(unit,
                                                          data_fields,
                                                          &entry));
            break;

        case BCMIMM_ENTRY_UPDATE:
            cur_entry.ingress = ingress;
            cur_entry.comp = comp;
            cur_entry.req_flags = req_flags;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_trigger_entry_init(unit, &cur_entry));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_trigger_lt_entry_update(unit,
                                                          data_fields,
                                                          &entry,
                                                          &cur_entry));
            break;

        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_trigger_lt_entry_delete(unit,
                                                          data_fields,
                                                          &entry));
            break;

        default:
            break;
    }

exit:
    if (out) {
        out->count = 0;
        out->field[out->count]->id = oper_state_fid[comp][select];
        out->field[out->count]->flags = 0;
        out->field[out->count]->next = 0;
        out->field[out->count]->idx = 0;
        if (SHR_FUNC_VAL_IS(SHR_E_NOT_FOUND)) {
            /* Set OPERATIONAL_STATE to INACTIVE . */
            out->field[out->count++]->data =
                                    ctrl->ctr_eflex_enum.oper_state_inactive;
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                    "Action Id %d entry not found in ACTION_PROFILE LT, "
                    "Setting operational state to INACTIVE.\n"),
                    entry.action_id));
        } else {
            /* Set OPERATIONAL_STATE to ACTIVE . */
            out->field[out->count++]->data =
                                    ctrl->ctr_eflex_enum.oper_state_active;
        }
    }

    if (SHR_FUNC_VAL_IS(SHR_E_NOT_FOUND)) {
        return SHR_E_NONE;
    }

    SHR_FUNC_EXIT();
}

/*! Function to lookup TRIGGER IMM LT entry. */
static int
bcmcth_ctr_eflex_trigger_lt_lookup(int unit,
                                   bool ingress,
                                   eflex_comp_t comp,
                                   uint64_t req_flags,
                                   bcmimm_lookup_type_t lkup_type,
                                   const bcmltd_fields_t *in,
                                   bcmltd_fields_t *out)
{
    int select;
    size_t i;
    ctr_eflex_action_profile_data_t action_entry = {0};
    ctr_eflex_trigger_data_t entry = {0};
    ctr_eflex_control_t *ctrl = NULL;
    const bcmltd_fields_t *key_fields = NULL;
    bool unmapped = true;

    SHR_FUNC_ENTER(unit);
    select = ingress ? 0 : 1;

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    /* key fields are in the "out" parameter for traverse operations */
    key_fields = (lkup_type == BCMIMM_TRAVERSE) ? out : in;

    action_entry.ingress = entry.ingress = ingress;
    action_entry.comp = entry.comp = comp;
    entry.req_flags = req_flags;

    /* Parse the key fields to get action profile id */
    for (i = 0; i < key_fields->count; i++) {
        if (key_fields->field[i]->id ==
                trigger_action_profile_id_fid[comp][select]) {
            action_entry.action_profile_id = key_fields->field[i]->data;
            break;
        }
    }

    /* Lookup entry with given action profile id key in ACTION_PROFILE LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_lookup(unit, &action_entry));

    /* Get pipe idx and action id from action profile entry */
    entry.pipe_idx = action_entry.pipe_idx;
    entry.action_id = action_entry.action_id;
    entry.inst = action_entry.state_ext.inst;
    entry.egr_inst = action_entry.state_ext.egr_inst;

    /* Read from trigger table */
    if (entry.comp == CTR_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_trigger_read(unit, &entry));
    } else if (entry.comp == STATE_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_state_eflex_trigger_read(unit, &entry));
    }

    out->count = 0;

    /* Populate out fields */
    out->field[out->count]->id = trigger_action_profile_id_fid[comp][select];
    out->field[out->count++]->data = action_entry.action_profile_id;

    out->field[out->count]->id = trigger_fid[comp][select];
    out->field[out->count++]->data = entry.trigger;

    out->field[out->count]->id = start_fid[comp][select];
    out->field[out->count++]->data = (uint64_t)entry.start;

    out->field[out->count]->id = stop_fid[comp][select];
    out->field[out->count++]->data = (uint64_t)entry.stop;

    if (entry.start == TIME) {
        out->field[out->count]->id = start_time_fid[comp][select];
        out->field[out->count++]->data = entry.start_time;
    } else if (entry.start == START_CONDITION) {
        out->field[out->count]->id = start_value_fid[comp][select];
        out->field[out->count++]->data = entry.start_value;

        out->field[out->count]->id = cond_mask_fid[comp][select];
        out->field[out->count++]->data = entry.cond_mask;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_field_is_unmapped(unit,
                                      trigger_table_sid[comp][select],
                                      compare_start_fid[comp][select],
                                      &unmapped));
        if (!unmapped) {
            out->field[out->count]->id = compare_start_fid[comp][select];
            out->field[out->count++]->data = (uint64_t)entry.compare_start;
        }
    }

    if (entry.stop == PERIOD) {
        out->field[out->count]->id = num_period_fid[comp][select];
        out->field[out->count++]->data = entry.num_periods;

        out->field[out->count]->id = scale_fid[comp][select];
        out->field[out->count++]->data = entry.scale;
    } else if (entry.stop == STOP_CONDITION) {
        out->field[out->count]->id = stop_value_fid[comp][select];
        out->field[out->count++]->data = entry.stop_value;

        out->field[out->count]->id = cond_mask_fid[comp][select];
        out->field[out->count++]->data = entry.cond_mask;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_field_is_unmapped(unit,
                                      trigger_table_sid[comp][select],
                                      compare_stop_fid[comp][select],
                                      &unmapped));
        if (!unmapped) {
            out->field[out->count]->id = compare_stop_fid[comp][select];
            out->field[out->count++]->data = (uint64_t)entry.compare_stop;
        }
    } else if (entry.stop == ACTION_COUNT) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_field_is_unmapped(unit,
                                      trigger_table_sid[comp][select],
                                      num_actions_fid[comp][select],
                                      &unmapped));

        if (!unmapped) {
            out->field[out->count]->id = num_actions_fid[comp][select];
            out->field[out->count++]->data = entry.num_actions;
        }
    }

    if (comp == CTR_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_field_is_unmapped(unit,
                                      trigger_table_sid[comp][select],
                                      interval_shift_fid[comp][select],
                                      &unmapped));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_field_is_unmapped(unit,
                                      trigger_table_sid[comp][select],
                                      interval_size_fid[comp][select],
                                      &unmapped));
        if (!unmapped) {
            out->field[out->count]->id = interval_shift_fid[comp][select];
            out->field[out->count++]->data = entry.interval_shift;
            out->field[out->count]->id = interval_size_fid[comp][select];
            out->field[out->count++]->data = entry.interval_size;
        }
    }

exit:
    if (out) {
        if (SHR_FUNC_VAL_IS(SHR_E_NOT_FOUND)) {
            /* Set OPERATIONAL_STATE to INACTIVE . */
            (void)bcmcth_ctr_eflex_oper_state_set(
                    unit,
                    oper_state_fid[comp][select],
                    ctrl->ctr_eflex_enum.oper_state_inactive,
                    out);

            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                    "Action Id %d entry not found in ACTION_PROFILE LT, "
                    "Setting operational state to INACTIVE.\n"),
                    entry.action_id));
        } else {
            /* Set OPERATIONAL_STATE to ACTIVE . */
            out->field[out->count]->id = oper_state_fid[comp][select];
            out->field[out->count++]->data =
                                        ctrl->ctr_eflex_enum.oper_state_active;
        }
    }

    if (SHR_FUNC_VAL_IS(SHR_E_NOT_FOUND)) {
        return SHR_E_NONE;
    }

    SHR_FUNC_EXIT();
}

/***********************
 * INGRESS IMM Functions
 */

/*!
 * \brief Lookup callback function of CTR_ING_EFLEX_TRIGGER IMM event handler.
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] op_arg The operation arguments.
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
bcmcth_ctr_eflex_ing_trigger_lt_lookup_cb(int unit,
                                          bcmltd_sid_t sid,
                                          const bcmltd_op_arg_t *op_arg,
                                          void *context,
                                          bcmimm_lookup_type_t lkup_type,
                                          const bcmltd_fields_t *in,
                                          bcmltd_fields_t *out)
{
    uint32_t t;
    uint64_t req_flags;
    ctr_eflex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);
    if (lkup_type == BCMIMM_LOOKUP) {
        SHR_NULL_CHECK(in, SHR_E_PARAM);
    }
    SHR_NULL_CHECK(out, SHR_E_PARAM);
    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, CTR_EFLEX, &ctrl));

    for (t = 0; t < ctrl->lt_info_num; t++) {
        if (ctrl->ctr_eflex_field_info[t].lt_sid == CTR_ING_EFLEX_TRIGGERt) {
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

    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_trigger_lt_lookup(unit,
                                            INGRESS,
                                            CTR_EFLEX,
                                            req_flags,
                                            lkup_type,
                                            in,
                                            out));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Stage callback function of CTR_ING_EFLEX_TRIGGER to insert, update,
 *        delete IMM LT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] op_arg The operation arguments.
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
bcmcth_ctr_eflex_ing_trigger_lt_stage_cb(int unit,
                                         bcmltd_sid_t sid,
                                         const bcmltd_op_arg_t *op_arg,
                                         bcmimm_entry_event_t event_reason,
                                         const bcmltd_field_t *key_fields,
                                         const bcmltd_field_t *data_fields,
                                         void *context,
                                         bcmltd_fields_t *output_fields)
{
    uint64_t req_flags;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(key_fields, SHR_E_PARAM);
    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);
    if (event_reason != BCMIMM_ENTRY_DELETE) {
        SHR_NULL_CHECK(output_fields, SHR_E_PARAM);
    }

    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_trigger_lt_stage(unit,
                                           INGRESS,
                                           CTR_EFLEX,
                                           req_flags,
                                           event_reason,
                                           key_fields,
                                           data_fields,
                                           output_fields));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Entry limit get callback function of CTR_ING_EFLEX_TRIGGER
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] count Entry limit value.
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_ctr_eflex_ing_trigger_lt_ent_limit_get_cb(int unit,
                                                 uint32_t trans_id,
                                                 bcmltd_sid_t sid,
                                                 uint32_t *count)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_lt_ent_limit_get(unit,
                                                          INGRESS,
                                                          CTR_EFLEX,
                                                          count));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Lookup callback function of FLEX_STATE_ING_TRIGGER IMM event handler.
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] op_arg The operation arguments.
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
bcmcth_state_eflex_ing_trigger_lt_lookup_cb(int unit,
                                            bcmltd_sid_t sid,
                                            const bcmltd_op_arg_t *op_arg,
                                            void *context,
                                            bcmimm_lookup_type_t lkup_type,
                                            const bcmltd_fields_t *in,
                                            bcmltd_fields_t *out)
{
    uint32_t t;
    uint64_t req_flags;
    ctr_eflex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);
    if (lkup_type == BCMIMM_LOOKUP) {
        SHR_NULL_CHECK(in, SHR_E_PARAM);
    }
    SHR_NULL_CHECK(out, SHR_E_PARAM);
    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, STATE_EFLEX, &ctrl));

    for (t = 0; t < ctrl->lt_info_num; t++) {
        if (ctrl->ctr_eflex_field_info[t].lt_sid == FLEX_STATE_ING_TRIGGERt) {
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

    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_trigger_lt_lookup(unit,
                                            INGRESS,
                                            STATE_EFLEX,
                                            req_flags,
                                            lkup_type,
                                            in,
                                            out));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Stage callback function of FLEX_STATE_ING_TRIGGER to insert, update,
 *        delete IMM LT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] op_arg The operation arguments.
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
bcmcth_state_eflex_ing_trigger_lt_stage_cb(int unit,
                                           bcmltd_sid_t sid,
                                           const bcmltd_op_arg_t *op_arg,
                                           bcmimm_entry_event_t event_reason,
                                           const bcmltd_field_t *key_fields,
                                           const bcmltd_field_t *data_fields,
                                           void *context,
                                           bcmltd_fields_t *output_fields)
{
    uint64_t req_flags;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(key_fields, SHR_E_PARAM);
    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);
    if (event_reason != BCMIMM_ENTRY_DELETE) {
        SHR_NULL_CHECK(output_fields, SHR_E_PARAM);
    }

    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_trigger_lt_stage(unit,
                                           INGRESS,
                                           STATE_EFLEX,
                                           req_flags,
                                           event_reason,
                                           key_fields,
                                           data_fields,
                                           output_fields));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Entry limit get callback function of FLEX_STATE_ING_TRIGGER
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] count Entry limit value.
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_state_eflex_ing_trigger_lt_ent_limit_get_cb(int unit,
                                                   uint32_t trans_id,
                                                   bcmltd_sid_t sid,
                                                   uint32_t *count)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_lt_ent_limit_get(unit,
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
 * \brief Lookup callback function of CTR_EGR_EFLEX_TRIGGER IMM event handler.
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] op_arg The operation arguments.
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
bcmcth_ctr_eflex_egr_trigger_lt_lookup_cb(int unit,
                                          bcmltd_sid_t sid,
                                          const bcmltd_op_arg_t *op_arg,
                                          void *context,
                                          bcmimm_lookup_type_t lkup_type,
                                          const bcmltd_fields_t *in,
                                          bcmltd_fields_t *out)
{
    uint32_t t;
    uint64_t req_flags;
    ctr_eflex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);
    if (lkup_type == BCMIMM_LOOKUP) {
        SHR_NULL_CHECK(in, SHR_E_PARAM);
    }
    SHR_NULL_CHECK(out, SHR_E_PARAM);
    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, CTR_EFLEX, &ctrl));

    for (t = 0; t < ctrl->lt_info_num; t++) {
        if (ctrl->ctr_eflex_field_info[t].lt_sid == CTR_EGR_EFLEX_TRIGGERt) {
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

    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_trigger_lt_lookup(unit,
                                            EGRESS,
                                            CTR_EFLEX,
                                            req_flags,
                                            lkup_type,
                                            in,
                                            out));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Stage callback function of CTR_EGR_EFLEX_TRIGGER to insert, update,
 *        delete IMM LT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] op_arg The operation arguments.
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
bcmcth_ctr_eflex_egr_trigger_lt_stage_cb(int unit,
                                         bcmltd_sid_t sid,
                                         const bcmltd_op_arg_t *op_arg,
                                         bcmimm_entry_event_t event_reason,
                                         const bcmltd_field_t *key_fields,
                                         const bcmltd_field_t *data_fields,
                                         void *context,
                                         bcmltd_fields_t *output_fields)
{
    uint64_t req_flags;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(key_fields, SHR_E_PARAM);
    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);
    if (event_reason != BCMIMM_ENTRY_DELETE) {
        SHR_NULL_CHECK(output_fields, SHR_E_PARAM);
    }

    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_trigger_lt_stage(unit,
                                           EGRESS,
                                           CTR_EFLEX,
                                           req_flags,
                                           event_reason,
                                           key_fields,
                                           data_fields,
                                           output_fields));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Entry limit get callback function of CTR_EGR_EFLEX_TRIGGER
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] count Entry limit value.
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_ctr_eflex_egr_trigger_lt_ent_limit_get_cb(int unit,
                                                 uint32_t trans_id,
                                                 bcmltd_sid_t sid,
                                                 uint32_t *count)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_lt_ent_limit_get(unit,
                                                          EGRESS,
                                                          CTR_EFLEX,
                                                          count));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Lookup callback function of FLEX_STATE_EGR_TRIGGER IMM event handler.
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] op_arg The operation arguments.
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
bcmcth_state_eflex_egr_trigger_lt_lookup_cb(int unit,
                                            bcmltd_sid_t sid,
                                            const bcmltd_op_arg_t *op_arg,
                                            void *context,
                                            bcmimm_lookup_type_t lkup_type,
                                            const bcmltd_fields_t *in,
                                            bcmltd_fields_t *out)
{
    uint32_t t;
    uint64_t req_flags;
    ctr_eflex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);
    if (lkup_type == BCMIMM_LOOKUP) {
        SHR_NULL_CHECK(in, SHR_E_PARAM);
    }
    SHR_NULL_CHECK(out, SHR_E_PARAM);
    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, STATE_EFLEX, &ctrl));

    for (t = 0; t < ctrl->lt_info_num; t++) {
        if (ctrl->ctr_eflex_field_info[t].lt_sid == FLEX_STATE_EGR_TRIGGERt) {
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

    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_trigger_lt_lookup(unit,
                                            EGRESS,
                                            STATE_EFLEX,
                                            req_flags,
                                            lkup_type,
                                            in,
                                            out));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Stage callback function of FLEX_STATE_EGR_TRIGGER to insert, update,
 *        delete IMM LT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] op_arg The operation arguments.
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
bcmcth_state_eflex_egr_trigger_lt_stage_cb(int unit,
                                           bcmltd_sid_t sid,
                                           const bcmltd_op_arg_t *op_arg,
                                           bcmimm_entry_event_t event_reason,
                                           const bcmltd_field_t *key_fields,
                                           const bcmltd_field_t *data_fields,
                                           void *context,
                                           bcmltd_fields_t *output_fields)
{
    uint64_t req_flags;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(key_fields, SHR_E_PARAM);
    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);
    if (event_reason != BCMIMM_ENTRY_DELETE) {
        SHR_NULL_CHECK(output_fields, SHR_E_PARAM);
    }

    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_trigger_lt_stage(unit,
                                           EGRESS,
                                           STATE_EFLEX,
                                           req_flags,
                                           event_reason,
                                           key_fields,
                                           data_fields,
                                           output_fields));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Entry limit get callback function of FLEX_STATE_EGR_TRIGGER
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] count Entry limit value.
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_state_eflex_egr_trigger_lt_ent_limit_get_cb(int unit,
                                                   uint32_t trans_id,
                                                   bcmltd_sid_t sid,
                                                   uint32_t *count)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_lt_ent_limit_get(unit,
                                                          EGRESS,
                                                          STATE_EFLEX,
                                                          count));

exit:
    SHR_FUNC_EXIT();
}

/**********************************************************************
* Public functions
 */

/* Function to register callback handlers for CTR_xxx_EFLEX_TRIGGER LTs */
int
bcmcth_ctr_eflex_imm_trigger_register(int unit)
{
    bcmimm_lt_cb_t ctr_eflex_lt_cb;

    SHR_FUNC_ENTER(unit);

    /* Register CTR_ING_EFLEX_TRIGGER handler */
    sal_memset(&ctr_eflex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
    ctr_eflex_lt_cb.op_stage  = bcmcth_ctr_eflex_ing_trigger_lt_stage_cb;
    ctr_eflex_lt_cb.op_lookup = bcmcth_ctr_eflex_ing_trigger_lt_lookup_cb;
    ctr_eflex_lt_cb.ent_limit_get =
        bcmcth_ctr_eflex_ing_trigger_lt_ent_limit_get_cb;

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmimm_lt_event_reg(unit,
                            CTR_ING_EFLEX_TRIGGERt,
                            &ctr_eflex_lt_cb,
                            NULL));

    /* Register CTR_EGR_EFLEX_TRIGGER handler */
    sal_memset(&ctr_eflex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
    ctr_eflex_lt_cb.op_stage  = bcmcth_ctr_eflex_egr_trigger_lt_stage_cb;
    ctr_eflex_lt_cb.op_lookup = bcmcth_ctr_eflex_egr_trigger_lt_lookup_cb;
    ctr_eflex_lt_cb.ent_limit_get =
        bcmcth_ctr_eflex_egr_trigger_lt_ent_limit_get_cb;

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmimm_lt_event_reg(unit,
                            CTR_EGR_EFLEX_TRIGGERt,
                            &ctr_eflex_lt_cb,
                            NULL));

exit:
    SHR_FUNC_EXIT();
}

/* Function to register callback handlers for STATE_xxx_EFLEX_TRIGGER LTs */
int
bcmcth_state_eflex_imm_trigger_register(int unit)
{
    bcmimm_lt_cb_t state_eflex_lt_cb;

    SHR_FUNC_ENTER(unit);

    /* Register FLEX_STATE_ING_TRIGGER handler */
    sal_memset(&state_eflex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
    state_eflex_lt_cb.op_stage  = bcmcth_state_eflex_ing_trigger_lt_stage_cb;
    state_eflex_lt_cb.op_lookup = bcmcth_state_eflex_ing_trigger_lt_lookup_cb;
    state_eflex_lt_cb.ent_limit_get =
        bcmcth_state_eflex_ing_trigger_lt_ent_limit_get_cb;

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmimm_lt_event_reg(unit,
                            FLEX_STATE_ING_TRIGGERt,
                            &state_eflex_lt_cb,
                            NULL));

    /* Register FLEX_STATE_EGR_TRIGGER handler */
    sal_memset(&state_eflex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
    state_eflex_lt_cb.op_stage  = bcmcth_state_eflex_egr_trigger_lt_stage_cb;
    state_eflex_lt_cb.op_lookup = bcmcth_state_eflex_egr_trigger_lt_lookup_cb;
    state_eflex_lt_cb.ent_limit_get =
        bcmcth_state_eflex_egr_trigger_lt_ent_limit_get_cb;

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmimm_lt_event_reg(unit,
                            FLEX_STATE_EGR_TRIGGERt,
                            &state_eflex_lt_cb,
                            NULL));

exit:
    SHR_FUNC_EXIT();
}
