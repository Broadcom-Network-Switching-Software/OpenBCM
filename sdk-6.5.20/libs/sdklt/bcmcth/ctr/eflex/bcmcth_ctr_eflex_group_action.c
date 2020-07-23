/*! \file bcmcth_ctr_eflex_group_action.c
 *
 * BCMCTH CTR_EFLEX_GROUP_ACTION_PROFILEt IMM handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>

#include <bcmptm/bcmptm.h>
#include <bcmcth/bcmcth_ctr_eflex_drv.h>
#include <bcmcth/bcmcth_state_eflex_drv.h>
#include <bcmcth/bcmcth_ctr_eflex_internal.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_CTREFLEX

/*! Generate CTR_xxx_GROUP_ACTION_PROFILE LT field ids. */
#define CTR_ING_EGR_GROUP_ACTION_PROFILE_FIDS(x) \
    CTR_ING_EFLEX_GROUP_ACTION_PROFILEt_##x, \
    CTR_EGR_EFLEX_GROUP_ACTION_PROFILEt_##x

/*! Generate STATE_xxx_GROUP_ACTION_PROFILE LT field ids. */
#define STATE_ING_EGR_GROUP_ACTION_PROFILE_FIDS(x) \
    FLEX_STATE_ING_GROUP_ACTION_PROFILEt_##x, \
    FLEX_STATE_EGR_GROUP_ACTION_PROFILEt_##x

/*! Invalid index for mapped action profile id */
#define INVALID_MAP_INDEX 0xFFFF

/*******************************************************************************
 * Local definitions
 */

/* GROUP_ACTION_PROFILE LT field info */
static const bcmlrd_sid_t group_action_profile_table_sid[][2] = {
        {CTR_ING_EGR_LT_IDS(EFLEX_GROUP_ACTION_PROFILEt)},
        {STATE_ING_EGR_LT_IDS(GROUP_ACTION_PROFILEt)},
};

static const bcmlrd_fid_t group_profile_id_fid[][2] = {
{CTR_ING_EFLEX_GROUP_ACTION_PROFILEt_CTR_ING_EFLEX_GROUP_ACTION_PROFILE_IDf,
CTR_EGR_EFLEX_GROUP_ACTION_PROFILEt_CTR_EGR_EFLEX_GROUP_ACTION_PROFILE_IDf},
{FLEX_STATE_ING_GROUP_ACTION_PROFILEt_FLEX_STATE_ING_GROUP_ACTION_PROFILE_IDf,
FLEX_STATE_EGR_GROUP_ACTION_PROFILEt_FLEX_STATE_EGR_GROUP_ACTION_PROFILE_IDf},
};

static const bcmlrd_fid_t group_fid[][2] = {
        {CTR_ING_EGR_GROUP_ACTION_PROFILE_FIDS(GROUPf)},
        {STATE_ING_EGR_GROUP_ACTION_PROFILE_FIDS(GROUPf)},
};

static const bcmlrd_fid_t pipe_idx_fid[][2] = {
        {CTR_ING_EGR_GROUP_ACTION_PROFILE_FIDS(PIPEf)},
        {STATE_ING_EGR_GROUP_ACTION_PROFILE_FIDS(PIPEf)},
};

static const bcmlrd_fid_t instance_fid[][2] = {
        {0, 0},
        {FLEX_STATE_ING_GROUP_ACTION_PROFILEt_INSTANCEf, 0},
};

static const bcmlrd_fid_t egr_instance_fid[][2] = {
        {0, 0},
        {0, FLEX_STATE_EGR_GROUP_ACTION_PROFILEt_INSTANCEf},
};

static const bcmlrd_fid_t group_action_profile_id_fid[][2] = {
{CTR_ING_EFLEX_GROUP_ACTION_PROFILEt_CTR_ING_EFLEX_ACTION_PROFILE_IDf,
CTR_EGR_EFLEX_GROUP_ACTION_PROFILEt_CTR_EGR_EFLEX_ACTION_PROFILE_IDf},
{FLEX_STATE_ING_GROUP_ACTION_PROFILEt_FLEX_STATE_ING_ACTION_PROFILE_IDf,
FLEX_STATE_EGR_GROUP_ACTION_PROFILEt_FLEX_STATE_EGR_ACTION_PROFILE_IDf},
};

static const bcmlrd_fid_t group_map_fid[][2] = {
        {CTR_ING_EGR_GROUP_ACTION_PROFILE_FIDS(GROUP_MAPf)},
        {STATE_ING_EGR_GROUP_ACTION_PROFILE_FIDS(GROUP_MAPf)},
};

static const bcmlrd_fid_t oper_state_fid[][2] = {
        {CTR_ING_EGR_GROUP_ACTION_PROFILE_FIDS(OPERATIONAL_STATEf)},
        {STATE_ING_EGR_GROUP_ACTION_PROFILE_FIDS(OPERATIONAL_STATEf)},
};

/**********************************************************************
* Private functions
 */

/* Function to initialize GROUP_ACTION_PROFILE entry with invalid values */
static int
bcmcth_ctr_eflex_group_action_entry_init(
    int unit,
    ctr_eflex_group_action_data_t *entry)
{
    int i, num_actions;
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;
    num_actions = entry->ingress ? device_info->num_ing_actions :
                                   device_info->num_egr_actions;

    entry->group_profile_id = CTR_EFLEX_INVALID_VAL;
    entry->group_id = CTR_EFLEX_INVALID_VAL;
    entry->action_profile_id = CTR_EFLEX_INVALID_VAL;
    entry->action_id = CTR_EFLEX_INVALID_VAL;
    entry->pipe_idx = CTR_EFLEX_INVALID_VAL;
    entry->inst = CTR_EFLEX_INVALID_VAL;
    entry->egr_inst = CTR_EFLEX_INVALID_VAL;

    for (i = 0; i < num_actions; i++) {
        entry->action_profile_id_map[i] = CTR_EFLEX_INVALID_VAL;
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to get default values for GROUP_ACTION_PROFILE entry */
static int
bcmcth_ctr_eflex_group_action_default_get(
    int unit,
    ctr_eflex_group_action_data_t *entry)
{
    bcmlrd_client_field_info_t *f_info = NULL;
    const bcmlrd_table_rep_t *lt_info = NULL;
    size_t num_fid = 0, i, num_actions, idx;
    int select;
    uint8_t pipe_unique;
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;
    eflex_comp_t comp;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;
    num_actions = entry->ingress ? device_info->num_ing_actions :
                                   device_info->num_egr_actions;

    select  = entry->ingress ? 0 : 1;
    comp = entry->comp;

    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            entry->ingress,
                                            entry->comp,
                                            &pipe_unique));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit,
                                group_action_profile_table_sid[comp][select],
                                &num_fid));

    lt_info = bcmlrd_table_get(group_action_profile_table_sid[comp][select]);
    SHR_NULL_CHECK(lt_info, SHR_E_BADID);

    SHR_ALLOC(f_info, sizeof(bcmlrd_client_field_info_t) * num_fid,
              "bcmcthCtrEflexGroupAllocFields");
    SHR_NULL_CHECK(f_info, SHR_E_MEMORY);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_table_fields_def_get(unit,
                                     lt_info->name,
                                     num_fid,
                                     f_info,
                                     &num_fid));

    for (i = 0; i < num_fid; i++) {
        if (f_info[i].id == group_fid[comp][select]) {
            entry->group_id = f_info[i].def.u16;
        } else if (pipe_unique &&
                   f_info[i].id == pipe_idx_fid[comp][select]) {
            entry->pipe_idx = f_info[i].def.u8;
        } else if (f_info[i].id == group_action_profile_id_fid[comp][select]) {
            entry->action_profile_id = f_info[i].def.u16;
        } else if (f_info[i].id == group_map_fid[comp][select]) {
            for (idx = 0; idx < num_actions; idx++) {
                entry->action_profile_id_map[idx] = f_info[i].def.u16;
            }
        } else if (entry->comp == STATE_EFLEX) {
            /* Flex state data */
            if (entry->ingress == INGRESS &&
                f_info[i].id == instance_fid[comp][select]) {
                entry->inst = (state_eflex_inst_t)f_info[i].def.u32;
            }
            if (entry->ingress == EGRESS &&
                f_info[i].id == egr_instance_fid[comp][select]) {
                entry->egr_inst = (state_eflex_inst_t)f_info[i].def.u32;
            }
        }
    }

exit:
    SHR_FREE(f_info);
    SHR_FUNC_EXIT();
}

/*
 * Function to replace GROUP_ACTION_PROFILE entry with values from another
 * entry if it is invalid
 * entry -- LT entry with updated values provided by application
 * cur_entry -- current LT entry present in the table.
 */
static int
bcmcth_ctr_eflex_group_action_fill(int unit,
                                   ctr_eflex_group_action_data_t *entry,
                                   ctr_eflex_group_action_data_t *cur_entry)
{
    int i, num_actions;
    uint8_t pipe_unique;
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;
    num_actions = entry->ingress ? device_info->num_ing_actions :
                                   device_info->num_egr_actions;

    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            entry->ingress,
                                            entry->comp,
                                            &pipe_unique));

    if (pipe_unique && entry->pipe_idx == CTR_EFLEX_INVALID_VAL) {
        entry->pipe_idx = cur_entry->pipe_idx;
    }

    if (entry->group_id == CTR_EFLEX_INVALID_VAL) {
        entry->group_id = cur_entry->group_id;
    }

    if (entry->action_profile_id == CTR_EFLEX_INVALID_VAL) {
        entry->action_profile_id = cur_entry->action_profile_id;
    }

    if (entry->comp == STATE_EFLEX && entry->ingress == INGRESS &&
        entry->inst == CTR_EFLEX_INVALID_VAL) {
        entry->inst = cur_entry->inst;
    }
    if (entry->comp == STATE_EFLEX && entry->ingress == EGRESS &&
        entry->egr_inst == CTR_EFLEX_INVALID_VAL) {
        entry->egr_inst = cur_entry->inst;
    }

    for (i = 0; i < num_actions; i++) {
        if (entry->action_profile_id_map[i] == CTR_EFLEX_INVALID_VAL) {
            entry->action_profile_id_map[i] =
                cur_entry->action_profile_id_map[i];
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to parse data fields in given GROUP_ACTION_PROFILE entry */
static int
bcmcth_ctr_eflex_group_action_lt_fields_check(
    int unit,
    ctr_eflex_group_action_data_t *entry,
    const bcmltd_field_t *data_fields)
{
    int select;
    uint8_t pipe_unique;
    eflex_comp_t comp;

    SHR_FUNC_ENTER(unit);

    select = entry->ingress ? 0 : 1;
    comp = entry->comp;

    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            entry->ingress,
                                            entry->comp,
                                            &pipe_unique));

    /* data_fields contains the key field for get_first/next IMM API */
    if (data_fields->id == group_profile_id_fid[comp][select]) {
        entry->group_profile_id = data_fields->data;
    } else if (data_fields->id == group_fid[comp][select]) {
        entry->group_id = data_fields->data;
    } else if (pipe_unique &&
               data_fields->id == pipe_idx_fid[comp][select]) {
        entry->pipe_idx = data_fields->data;
    } else if (entry->ingress == INGRESS &&
               entry->comp == STATE_EFLEX &&
               data_fields->id == instance_fid[comp][select]) {
        entry->inst = data_fields->data;
    } else if (entry->ingress == EGRESS &&
               entry->comp == STATE_EFLEX &&
               data_fields->id == egr_instance_fid[comp][select]) {
        entry->egr_inst = data_fields->data;
    } else if (data_fields->id == group_action_profile_id_fid[comp][select]) {
        entry->action_profile_id = data_fields->data;
    } else if (data_fields->id == group_map_fid[comp][select]) {
        entry->action_profile_id_map[data_fields->idx] = data_fields->data;
    } else if (data_fields->id == oper_state_fid[comp][select]) {
        entry->oper_state = data_fields->data;
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to parse given GROUP_ACTION_PROFILE entry */
static int
bcmcth_ctr_eflex_group_action_lt_fields_parse(
    int unit,
    const bcmltd_field_t *data_fields,
    ctr_eflex_group_action_data_t *entry)
{
    SHR_FUNC_ENTER(unit);

    /* Parse data fields */
    while (data_fields) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_group_action_lt_fields_check(unit,
                                                           entry,
                                                           data_fields));

        data_fields = data_fields->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to parse GROUP_ACTION_PROFILE IMM entry */
static int
bcmcth_ctr_eflex_group_action_entry_parse(
    int unit,
    ctr_eflex_group_action_data_t *entry,
    const bcmltd_fields_t *flist)
{
    uint32_t i;
    bcmltd_field_t *data_fields = NULL;

    SHR_FUNC_ENTER(unit);

    /* extract data fields */
    for (i = 0; i < flist->count; i++) {
        data_fields = flist->field[i];

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_group_action_lt_fields_check(unit,
                                                           entry,
                                                           data_fields));
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to update OPERATIONAL_STATE in GROUP_ACTION_PROFILE IMM LT */
static int
bcmcth_ctr_eflex_group_action_oper_state_update(
    int unit,
    ctr_eflex_group_action_data_t *entry)
{
    bcmltd_fields_t key_fields = {0};
    size_t num_key_fields, num_readonly_fields;
    int select;
    uint32_t t;
    ctr_eflex_control_t *ctrl = NULL;
    eflex_comp_t comp;

    SHR_FUNC_ENTER(unit);
    select = entry->ingress ? 0 : 1;
    comp = entry->comp;

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));

    for (t = 0; t < ctrl->lt_info_num; t++) {
        if (ctrl->ctr_eflex_field_info[t].lt_sid ==
            group_action_profile_table_sid[comp][select]) {
            break;
        }
    }

    if (t == ctrl->lt_info_num) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "GROUP_ACTION_PROFILE LT info not found.\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    num_key_fields = ctrl->ctr_eflex_field_info[t].num_key_fields;
    num_readonly_fields = ctrl->ctr_eflex_field_info[t].num_read_only_fields;

    /* Allocate key field */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_fields_alloc(unit,
                                       &key_fields,
                                       (num_key_fields + num_readonly_fields)));

    /* Set the fields.count to include OPERATIONAL_STATE */
    key_fields.count = num_key_fields + num_readonly_fields;
    key_fields.field[0]->id = group_profile_id_fid[comp][select];
    key_fields.field[0]->data = entry->group_profile_id;
    key_fields.field[1]->id = oper_state_fid[comp][select];
    key_fields.field[1]->data = entry->oper_state;

    /* Update IMM entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_update(unit,
                             0,
                             group_action_profile_table_sid[comp][select],
                             &key_fields));

exit:
    bcmcth_ctr_eflex_fields_free(unit, &key_fields);
    SHR_FUNC_EXIT();
}

/*
 * Function to lookup GROUP_ACTION_PROFILE table with given group id.
 * It populates the LT entry with all fields except the key field
 * which is provided within the entry as input.
 */
int
bcmcth_ctr_eflex_group_action_lookup(int unit,
                                     ctr_eflex_group_action_data_t *entry)
{
    bcmltd_fields_t key_fields = {0};
    bcmltd_field_t fld;
    bcmltd_field_t *fld_ptr_arr[1];
    bcmltd_fields_t imm_fields = {0};
    size_t num_fields = 0, num_actions;
    int select;
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;
    eflex_comp_t comp;

    SHR_FUNC_ENTER(unit);

    select = entry->ingress ? 0 : 1;
    comp = entry->comp;

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;
    num_actions = entry->ingress ? device_info->num_ing_actions :
                                   device_info->num_egr_actions;

    /* Assign default values */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_group_action_default_get(unit, entry));

    /* Configure key field */
    key_fields.count = 1;
    key_fields.field = fld_ptr_arr;
    fld_ptr_arr[0] = &fld;
    fld.id = group_profile_id_fid[comp][select];
    fld.data = entry->group_profile_id;

    /* Allocate imm fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit,
                                group_action_profile_table_sid[comp][select],
                                &num_fields));

    /*
     * Adjust num_fields for array size
     * since LRD API counts field array size as 1
     */
    num_fields += num_actions - 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_fields_alloc(unit, &imm_fields, num_fields));

    /* lookup IMM table; error if entry not found */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit,
                             group_action_profile_table_sid[comp][select],
                             &key_fields,
                             &imm_fields));

    /* Parse IMM entry fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_group_action_entry_parse(unit, entry, &imm_fields));

exit:
    /* imm_fields.count may be modified by bcmimm_entry_lookup */
    imm_fields.count = num_fields;
    bcmcth_ctr_eflex_fields_free(unit, &imm_fields);

    SHR_FUNC_EXIT();
}

/* Function to help insert PT entry for GROUP_ACTION_PROFILE LT */
static int
bcmcth_ctr_eflex_group_action_insert_helper(
    int unit,
    size_t action_profile_id_skip_index,
    ctr_eflex_group_action_data_t *entry)
{
    size_t num_actions, i;
    uint8_t pipe_unique;
    ctr_eflex_action_profile_data_t action_entry;
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;
    num_actions = entry->ingress ? device_info->num_ing_actions :
                                   device_info->num_egr_actions;

    /*
     * The number of simultaneous actions is reduced by 1 since one of the
     * associated counter actions is configured as the group counter
     * action
     */
    entry->num_actions = num_actions;

    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            entry->ingress,
                                            entry->comp,
                                            &pipe_unique));

    /* Determine first and last pipe number */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_num_pipes_get(unit,
                                        entry->ingress,
                                        entry->comp,
                                        entry->pipe_idx,
                                        &entry->start_pipe_idx,
                                        &entry->end_pipe_idx));

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "Insert %s Entry:\n"
            "grp_profile_id %d, grp_id %d, grp_action_profile_id %d\n"
            "grp_action_id %d, pipe_idx %d, inst %d egr_inst %d\n"),
            entry->ingress ? "Ingress" : "Egress",
            entry->group_profile_id, entry->group_id, entry->action_profile_id,
            entry->action_id, entry->pipe_idx, entry->inst, entry->egr_inst));

    /*
     * Check if each associated action profile id exists in ACTION_PROFILE LT.
     * If yes, save corresponding action id in counter actions map.
     * If no, return error and set OPERATIONAL_STATE as INACTIVE.
     */
    for (i = 0; i < entry->num_actions; i++) {
        /*
         * If action_profile_id_map[i] is in the middle of being inserted,
         * then corresponding action_id_map[i] is already populated and
         * the lookup in ACTION_PROFILE LT must be skipped, else it will fail.
         */
        if (action_profile_id_skip_index == i) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Skipping action profile id %d \n"),
                entry->action_profile_id_map[i]));
            continue;
        }

        sal_memset(&action_entry, 0, sizeof(ctr_eflex_action_profile_data_t));
        action_entry.ingress = entry->ingress;
        action_entry.comp = entry->comp;

        if (entry->action_profile_id_map[i] !=
                CTR_EFLEX_INVALID_ACTION_PROFILE_ID) {
            action_entry.action_profile_id = entry->action_profile_id_map[i];

            /* Lookup ACTION_PROFILE LT with given action profile id key */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_action_profile_lookup(unit,
                                                        &action_entry));

            /*
             * In pipe unique mode, check if group action id and associated
             * counter action id are configured for the same pipe.
             */
            if (pipe_unique && entry->pipe_idx != action_entry.pipe_idx) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                        "Action id %d pipe idx %d NOT present in"
                        "group action id %d pipe idx %d\n"),
                    action_entry.action_id, action_entry.pipe_idx,
                    entry->action_id, entry->pipe_idx));
                SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
            }

            /*
             * For ingress flex state, check if group action id and associated
             * counter action id are configured for the same ingress instance.
             */
            if (entry->comp == STATE_EFLEX && entry->ingress == INGRESS &&
                entry->inst != action_entry.state_ext.inst) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                        "Flex state action id %d ingress inst %d NOT present in"
                        "group action id %d ingress inst %d\n"),
                    action_entry.action_id, action_entry.state_ext.inst,
                    entry->action_id, entry->inst));
                SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
            }
            if (entry->comp == STATE_EFLEX && entry->ingress == EGRESS &&
                entry->egr_inst != action_entry.state_ext.egr_inst) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                        "Flex state action id %d egress inst %d NOT present in"
                        "group action id %d egress inst %d\n"),
                    action_entry.action_id, action_entry.state_ext.egr_inst,
                    entry->action_id, entry->egr_inst));
                SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
            }

            /* If found, then save associated counter action. */
            entry->action_id_map[i] = action_entry.action_id;

            /* If found, then save associated counter action. */
            entry->action_id_map[i] = action_entry.action_id;

            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Action id %d is part of group id %d\n"),
                action_entry.action_id, entry->group_id));
        } else {
            /* Set invalid action id for the associated action */
             entry->action_id_map[i] = 0;
        }
    }

    /* Write to group action registers */
    if (entry->comp == CTR_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_group_action_write(unit, entry));
    } else if (entry->comp == STATE_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_state_eflex_group_action_write(unit, entry));
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to help delete PT entry for GROUP_ACTION_PROFILE LT */
static int
bcmcth_ctr_eflex_group_action_delete_helper(
    int unit,
    ctr_eflex_group_action_data_t *entry)
{
    size_t i, num_actions;
    uint8_t pipe_unique;
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;
    num_actions = entry->ingress ? device_info->num_ing_actions :
                                   device_info->num_egr_actions;
    entry->num_actions = num_actions;

    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            entry->ingress,
                                            entry->comp,
                                            &pipe_unique));

    /* Determine first and last pipe number */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_num_pipes_get(unit,
                                        entry->ingress,
                                        entry->comp,
                                        entry->pipe_idx,
                                        &entry->start_pipe_idx,
                                        &entry->end_pipe_idx));

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "Delete %s Entry:\n"
            "grp_profile_id %d, grp_id %d, grp_action_profile_id %d"
            "grp_action_id %d, pipe_unique %d, pipe_idx %d, inst %d egr_inst %d\n"),
            entry->ingress ? "Ingress" : "Egress",
            entry->group_profile_id, entry->group_id, entry->action_profile_id,
            entry->action_id, pipe_unique, entry->pipe_idx, entry->inst, entry->egr_inst));

    /* Set invalid action id for group */
    entry->action_id = 0;

    /* Set invalid action id for all associated actions in group */
    for (i = 0; i < entry->num_actions; i++) {
        entry->action_id_map[i] = 0;
    }

    /* Write to group action registers */
    if (entry->comp == CTR_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_group_action_write(unit, entry));
    } else if (entry->comp == STATE_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_state_eflex_group_action_write(unit, entry));
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to populate GROUP_ACTION_PROFILE entry during insert notification */
static int
bcmcth_ctr_eflex_group_action_insert_notify(
    int unit,
    bool group_action_profile_id_match,
    bool map_action_profile_id_match,
    size_t skip_index,
    ctr_eflex_action_profile_data_t *action_entry,
    ctr_eflex_group_action_data_t *entry)
{
    ctr_eflex_action_profile_data_t temp_action_entry;

    SHR_FUNC_ENTER(unit);

    /* Incoming ACTION_PROFILE LT matches group action profile id */
    if (group_action_profile_id_match) {
        /* Get group action id from action profile entry */
        entry->action_id = action_entry->action_id;

        /* Insert entry in PT */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_group_action_insert_helper(
                unit, skip_index, entry));
    }

    /*
     * Incoming ACTION_PROFILE LT matches one of associated
     * group action profile id and does not match group action profile id
     */
    if (map_action_profile_id_match && !group_action_profile_id_match) {
        sal_memset(&temp_action_entry, 0,
                   sizeof(ctr_eflex_action_profile_data_t));
        temp_action_entry.ingress = entry->ingress;
        temp_action_entry.comp = entry->comp;
        temp_action_entry.action_profile_id =
                                    entry->action_profile_id;

        /* Lookup ACTION_PROFILE entry to get group action id */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_action_profile_lookup(
                unit, &temp_action_entry));

        /*
         * For ingress flex state, check if group action id and associated
         * counter action id are configured for the same ingress instance.
         */
        if (entry->comp == STATE_EFLEX && entry->ingress == INGRESS &&
            entry->inst != temp_action_entry.state_ext.inst) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                    "Flex state action id %d ingress inst %d NOT present in"
                    "group action id %d ingress inst %d\n"),
                temp_action_entry.action_id, temp_action_entry.state_ext.inst,
                entry->action_id, entry->inst));
            SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }
        if (entry->comp == STATE_EFLEX && entry->ingress == EGRESS &&
            entry->egr_inst != temp_action_entry.state_ext.egr_inst) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                    "Flex state action id %d egress inst %d NOT present in"
                    "group action id %d egress inst %d \n"),
                temp_action_entry.action_id, temp_action_entry.state_ext.egr_inst,
                entry->action_id, entry->egr_inst));
            SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }

        /* Get action_id from action profile entry */
        entry->action_id = temp_action_entry.action_id;

        /* Get action_id from action profile entry */
        entry->action_id = temp_action_entry.action_id;

        /* Copy incoming action id to associated action id */
        entry->action_id_map[skip_index] = action_entry->action_id;

        /* Insert entry in PT */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_group_action_insert_helper(
                unit, skip_index, entry));
    }

    /*
     * Update GROUP_ACTION_PROFILE LT entry
     * OPERATIONAL_STATE to ACTIVE
     */
    entry->oper_state = CTR_EFLEX_OPER_STATE_ACTIVE;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_group_action_oper_state_update(
            unit, entry));

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function to check if GROUP_ACTION_PROFILE entry
 * should be updated based on ACTION_PROFILE LT entry insert/delete operation.
 *
 * GROUP_ACTION_PROFILE entry includes
 *   - Group action profile id and corresponding action id.
 *   - Associated action profile id array and corresponding action id array.
 *
 * Action ids are populated by looking up ACTION_PROFILE LT with the
 * corresponding action profile ids.
 *
 * This function may be called after ACTION_PROFILE entry insert/delete
 * operation for either one.
 *
 * This function iterates over each GROUP_ACTION_PROFILE entry searching
 * for a match with action profile id in input ACTION_PROFILE entry.
 *
 * For INSERT notification, the current OPERATIONAL_STATE must be INACTIVE.
 * There are 2 cases to consider:-
 *     Case I: ACTION_PROFILE entry matches group action profile id
 *             - Copy input action id as GROUP action id.
 *             - Call insert_helper() with invalid skip index.
 *
 *     Case II: ACTION_PROFILE entry matches one of associated action profile id
 *             - Lookup ACTION_PROFILE LT to get GROUP action id.
 *             - Copy input action id to matched associated action id.
 *             - Save the array index of matched action profile id.
 *               This index needs to be skipped in insert_helper() since the
 *               lookup in ACTION_PROFILE LT will fail
 *             - Call insert_helper() with skip index.
 *
 *  In both cases, if insert_helper() returns SUCESS, OPERATIONAL_STATE is
 *  updated to ACTIVE.
 *
 * For DELETE notification, the current OPERATIONAL_STATE must be ACTIVE.
 *     For matches on either of the cases above
 *             - Write default values to PT.
 *             - Set OPERATIONAL_STATE to INACTIVE.
 */
int
bcmcth_ctr_eflex_group_action_notify(
    int unit,
    bool insert,
    ctr_eflex_action_profile_data_t *action_entry)
{
    bcmltd_fields_t key_fields = {0};
    bcmltd_field_t fld;
    bcmltd_field_t *fld_ptr_arr[1];
    bcmltd_fields_t imm_fields = {0};
    ctr_eflex_group_action_data_t entry = {0};
    size_t i, num_fields = 0, num_actions, skip_index;
    int select, rv;
    bool group_action_profile_id_match, map_action_profile_id_match;
    bool cur_oper_state;
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;
    eflex_comp_t comp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(action_entry, SHR_E_PARAM);

    select = action_entry->ingress ? 0 : 1;
    comp = action_entry->comp;

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;
    num_actions = action_entry->ingress ? device_info->num_ing_actions :
                                          device_info->num_egr_actions;

    /*
     * If insert, update GROUP_ACTION_PROFILE entry if it is INACTIVE
     * If delete, update GROUP_ACTION_PROFILE entry if it is ACTIVE
     */
    cur_oper_state = insert ? CTR_EFLEX_OPER_STATE_INACTIVE :
                              CTR_EFLEX_OPER_STATE_ACTIVE;

    /* Configure key field */
    key_fields.count = 1;
    key_fields.field = fld_ptr_arr;
    fld_ptr_arr[0] = &fld;
    fld.id = group_profile_id_fid[comp][select];
    fld.data = entry.group_profile_id;

    /* Allocate imm fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit,
                                group_action_profile_table_sid[comp][select],
                                &num_fields));

    /*
     * Adjust num_fields for array size
     * since LRD API counts field array size as 1
     */
    num_fields += num_actions - 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_fields_alloc(unit, &imm_fields, num_fields));

    /* Get first entry from GROUP_ACTION_PROFILE IMM LT */
    rv = bcmimm_entry_get_first(unit,
                                group_action_profile_table_sid[comp][select],
                                &imm_fields);

    /* Iterate GROUP_ACTION_PROFILE LT searching for input action profile id */
    while (rv == SHR_E_NONE) {
        sal_memset(&entry, 0, sizeof(ctr_eflex_group_action_data_t));
        group_action_profile_id_match = false;
        map_action_profile_id_match = false;
        skip_index = INVALID_MAP_INDEX;
        entry.ingress = action_entry->ingress;
        entry.comp = action_entry->comp;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_group_action_entry_init(unit, &entry));

        /* Get default values */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_group_action_default_get(unit, &entry));

        /* Parse IMM entry fields */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_group_action_entry_parse(unit,
                                                       &entry,
                                                       &imm_fields));

        /* GROUP_ACTION_PROFILE LT entry OPERATIONAL_STATE is expected value */
        if (entry.oper_state == cur_oper_state) {
            if (entry.ingress == INGRESS && entry.comp == STATE_EFLEX) {
                /*
                 * For ingress flex state,
                 * instance and action profile id match.
                 */
                if (entry.inst == action_entry->state_ext.inst &&
                    entry.action_profile_id ==
                    action_entry->action_profile_id) {
                    group_action_profile_id_match = true;
                }
            }  else if (entry.ingress == EGRESS && entry.comp == STATE_EFLEX) {
                /*
                 * For egress flex state,
                 * instance and action profile id match.
                 */
                if (entry.egr_inst == action_entry->state_ext.egr_inst &&
                    entry.action_profile_id ==
                    action_entry->action_profile_id) {
                    group_action_profile_id_match = true;
                }
            } else if (entry.action_profile_id ==
                       action_entry->action_profile_id) {
                /* Action profile id matches group action profile id */
                group_action_profile_id_match = true;
            }

            /*
             * Action profile id matches any one of
             * associated action profile ids
             */
            for (i = 0; i < num_actions; i++) {
                if (entry.action_profile_id_map[i] ==
                    action_entry->action_profile_id) {
                    map_action_profile_id_match = true;
                    skip_index = i;
                    break;
                }
            }

            /* Insert notification */
            if (insert) {
                /* Insert entry in PT */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmcth_ctr_eflex_group_action_insert_notify(
                            unit,
                            group_action_profile_id_match,
                            map_action_profile_id_match,
                            skip_index,
                            action_entry,
                            &entry));

                /*
                 * Update GROUP_ACTION_PROFILE LT entry
                 * OPERATIONAL_STATE to ACTIVE
                 */
                entry.oper_state = CTR_EFLEX_OPER_STATE_ACTIVE;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmcth_ctr_eflex_group_action_oper_state_update(
                        unit, &entry));
            } else {
                /* Delete notification */
                /*
                 * Incoming action profile id matches either group action
                 * profile id or one of associated action profile ids.
                 * In either case, write defaults to PT and set
                 * OPERATIONAL_STATE to INACTIVE.
                 */
                if (group_action_profile_id_match ||
                    map_action_profile_id_match) {

                    /* Delete entry in PT */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmcth_ctr_eflex_group_action_delete_helper(unit,
                                                                     &entry));

                    /*
                     * Update GROUP_ACTION_PROFILE LT entry
                     * OPERATIONAL_STATE to INACTIVE
                     */
                    entry.oper_state = CTR_EFLEX_OPER_STATE_INACTIVE;
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmcth_ctr_eflex_group_action_oper_state_update(
                            unit, &entry));
                }
            }
        }

        /* Setup key for next entry */
        key_fields.field[0]->data = entry.group_profile_id;

        /* imm_fields.count may be modified. */
        imm_fields.count = num_fields;

        /* Get next entry from GROUP_ACTION_PROFILE IMM LT */
        rv = bcmimm_entry_get_next(unit,
                                   group_action_profile_table_sid[comp][select],
                                   &key_fields,
                                   &imm_fields);
    } /* end WHILE */

    if (rv == SHR_E_NOT_FOUND) {
        rv = SHR_E_NONE;
    }

exit:
    /* imm_fields.count may be modified. */
    imm_fields.count = num_fields;
    bcmcth_ctr_eflex_fields_free(unit, &imm_fields);
    SHR_FUNC_EXIT();
}

/* Function to insert entry into GROUP_ACTION_PROFILE IMM LT */
static int
bcmcth_ctr_eflex_group_action_lt_entry_insert(
    int unit,
    const bcmltd_field_t *data_fields,
    ctr_eflex_group_action_data_t *entry)
{
    ctr_eflex_action_profile_data_t action_entry;

    SHR_FUNC_ENTER(unit);

    /* Get default values */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_group_action_default_get(unit, entry));

    /*
     * Parse user values
     * default values will be over-written by user values, if present
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_group_action_lt_fields_parse(unit,
                                                       data_fields,
                                                       entry));

    sal_memset(&action_entry, 0, sizeof(ctr_eflex_action_profile_data_t));
    action_entry.ingress = entry->ingress;
    action_entry.comp = entry->comp;
    action_entry.action_profile_id = entry->action_profile_id;

    /* Lookup entry with given action profile id in ACTION_PROFILE LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_lookup(unit, &action_entry));

    /*
     * For ingress flex state, check if group action id and associated
     * counter action id are configured for the same ingress instance.
     */
    if (entry->comp == STATE_EFLEX && entry->ingress == INGRESS &&
        entry->inst != action_entry.state_ext.inst) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "Flex state action id %d ingress inst %d NOT present in"
                "group action id %d ingress inst %d\n"),
            action_entry.action_id, action_entry.state_ext.inst,
            entry->action_id, entry->inst ));
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
    if (entry->comp == STATE_EFLEX && entry->ingress == EGRESS &&
        entry->egr_inst != action_entry.state_ext.egr_inst) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "Flex state action id %d egress inst %d NOT present in"
                "group action id %d egress inst %d \n"),
            action_entry.action_id, action_entry.state_ext.egr_inst,
            entry->action_id, entry->egr_inst ));
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    /* Get action_id from action profile entry */
    entry->action_id = action_entry.action_id;

    /* Get action_id from action profile entry */
    entry->action_id = action_entry.action_id;

    /* Insert entry in PT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_group_action_insert_helper(unit,
                                                     INVALID_MAP_INDEX,
                                                     entry));

    /* Notify HITBIT_CONTROL LT */
    if (entry->comp == CTR_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_hitbit_ctrl_notify(unit,
                                                 true, /* insert operation */
                                                 true, /* group action entry */
                                                 (void *)entry));
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to update entry in GROUP_ACTION_PROFILE IMM LT */
static int
bcmcth_ctr_eflex_group_action_lt_entry_update(
    int unit,
    const bcmltd_field_t *data_fields,
    ctr_eflex_group_action_data_t *entry,
    ctr_eflex_group_action_data_t *cur_entry)
{
    size_t i, num_actions;
    uint8_t pipe_unique;
    ctr_eflex_action_profile_data_t action_entry = {0};
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;
    num_actions = entry->ingress ? device_info->num_ing_actions :
                                   device_info->num_egr_actions;
    entry->num_actions = cur_entry->num_actions = num_actions;

    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            entry->ingress,
                                            entry->comp,
                                            &pipe_unique));

    /* Parse user values */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_group_action_lt_fields_parse(unit,
                                                       data_fields,
                                                       entry));

    cur_entry->group_profile_id = entry->group_profile_id;

    /* Lookup entry with given key fields */
     SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_group_action_lookup(unit, cur_entry));

    /* Fill missing fields in new entry with current entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_group_action_fill(unit, entry, cur_entry));

    action_entry.ingress = entry->ingress;
    action_entry.comp = entry->comp;
    action_entry.action_profile_id = entry->action_profile_id;

    /* Lookup entry with given action profile id key in ACTION_PROFILE LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_lookup(unit, &action_entry));

    /*
     * For ingress flex state, check if group action id and associated
     * counter action id are configured for the same ingress instance.
     */
    if (entry->comp == STATE_EFLEX && entry->ingress == INGRESS &&
        entry->inst != action_entry.state_ext.inst) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "Flex state action id %d ingress inst %d NOT present in"
                "group action id %d ingress inst %d \n"),
            action_entry.action_id, action_entry.state_ext.inst,
            entry->action_id, entry->inst));
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
    if (entry->comp == STATE_EFLEX && entry->ingress == EGRESS &&
        entry->egr_inst != action_entry.state_ext.egr_inst) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "Flex state action id %d egress inst %d NOT present in"
                "group action id %d egress inst %d \n"),
            action_entry.action_id, action_entry.state_ext.egr_inst,
            entry->action_id, entry->egr_inst));
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    /* Get pipe idx and action id from action profile entry */
    entry->action_id = action_entry.action_id;

    /* Get pipe idx and action id from action profile entry */
    entry->action_id = action_entry.action_id;

    /* Determine first and last pipe number */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_num_pipes_get(unit,
                                        entry->ingress,
                                        entry->comp,
                                        entry->pipe_idx,
                                        &entry->start_pipe_idx,
                                        &entry->end_pipe_idx));

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "Update %s Entry:\n"
            "grp_profile_id %d, grp_id %d, grp_action_profile_id %d\n"
            "grp_action_id %d, pipe_idx %d, inst %d egr_inst %d\n"),
            entry->ingress ? "Ingress" : "Egress",
            entry->group_profile_id, entry->group_id, entry->action_profile_id,
            entry->action_id, entry->pipe_idx, entry->inst, entry->egr_inst));

    /*
     * Check if each associated action profile id exists in ACTION_PROFILE LT.
     * If yes, save corresponding action id in counter actions map.
     * If no, return error and set OPERATIONAL_STATE as INACTIVE.
     */
    for (i = 0; i < entry->num_actions; i++) {
        sal_memset(&action_entry, 0, sizeof(ctr_eflex_action_profile_data_t));
        action_entry.ingress = entry->ingress;
        action_entry.comp = entry->comp;

        if (entry->action_profile_id_map[i] !=
                CTR_EFLEX_INVALID_ACTION_PROFILE_ID) {
            action_entry.action_profile_id = entry->action_profile_id_map[i];

            /*
             * Lookup entry with given action profile id key
             * in ACTION_PROFILE LT
             */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_action_profile_lookup(unit, &action_entry));

            /*
             * Check if group action id and associated counter action id are
             * configured for the same pipe.
             */
            if (pipe_unique && entry->pipe_idx != action_entry.pipe_idx) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                        "Action id %d pipe idx %d NOT present in"
                        "group action id %d pipe idx %d\n"),
                    action_entry.action_id, action_entry.pipe_idx,
                    entry->action_id, entry->pipe_idx));
                SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
            }

            /*
             * For ingress flex state, check if group action id and associated
             * counter action id are configured for the same ingress instance.
             */
            if (entry->comp == STATE_EFLEX && entry->ingress == INGRESS &&
                entry->inst != action_entry.state_ext.inst) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                        "Flex state action id %d ingress inst %d NOT present in"
                        "group action id %d ingress inst %d\n"),
                    action_entry.action_id, action_entry.state_ext.inst,
                    entry->action_id, entry->inst));
                SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
            }
            if (entry->comp == STATE_EFLEX && entry->ingress == EGRESS &&
                entry->egr_inst != action_entry.state_ext.egr_inst) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                        "Flex state action id %d egress inst %d NOT present in"
                        "group action id %d egress inst %d\n"),
                    action_entry.action_id, action_entry.state_ext.egr_inst,
                    entry->action_id, entry->egr_inst));
                SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
            }

            /* If found, then save associated counter action. */
            entry->action_id_map[i] = action_entry.action_id;

            /* If found, then save associated counter action. */
            entry->action_id_map[i] = action_entry.action_id;

            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Action id %d is part of group id %d\n"),
                action_entry.action_id, entry->group_id));
        } else {
            /* Set invalid action id for the associated action */
            entry->action_id_map[i] = 0;
        }
    }

    /* Delete current entry in PT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_group_action_delete_helper(unit, cur_entry));

    /* Write to group action registers */
    if (entry->comp == CTR_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_group_action_write(unit, entry));
    } else if (entry->comp == STATE_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_state_eflex_group_action_write(unit, entry));
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to delete entry from GROUP_ACTION_PROFILE IMM LT */
static int
bcmcth_ctr_eflex_group_action_lt_entry_delete(
    int unit,
    const bcmltd_field_t *data_fields,
    ctr_eflex_group_action_data_t *entry)
{
    ctr_eflex_action_profile_data_t action_entry = {0};

    SHR_FUNC_ENTER(unit);

    /* Lookup GROUP_ACTION_PROFILE entry to get group action profile id */
     SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_group_action_lookup(unit, entry));

    /* If entry is INACTIVE, no further action is needed */
    if (entry->oper_state == CTR_EFLEX_OPER_STATE_INACTIVE) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "GROUP_ACTION_PROFILE entry is INACTIVE.\n")));
        SHR_EXIT();
    }

    /* Lookup entry with given action profile id key in ACTION_PROFILE LT */
    action_entry.ingress = entry->ingress;
    action_entry.comp = entry->comp;
    action_entry.action_profile_id = entry->action_profile_id;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_lookup(unit, &action_entry));

    /*
     * For ingress flex state,
     * check if both are configured for same instance.
     */
    if (entry->ingress == INGRESS && entry->comp == STATE_EFLEX &&
        entry->inst != action_entry.state_ext.inst) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "Flex state action id %d ingress inst %d NOT present in"
                "group action id %d ingress inst %d\n"),
            action_entry.action_id, action_entry.state_ext.inst,
            entry->action_id, entry->inst));
        SHR_EXIT();
    }
    if (entry->ingress == EGRESS && entry->comp == STATE_EFLEX &&
        entry->egr_inst != action_entry.state_ext.egr_inst) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "Flex state action id %d egress inst %d NOT present in"
                "group action id %d egress inst %d\n"),
            action_entry.action_id, action_entry.state_ext.egr_inst,
            entry->action_id, entry->egr_inst));
        SHR_EXIT();
    }


    /* Delete entry in PT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_group_action_delete_helper(unit, entry));

    /* Notify HITBIT_CONTROL LT */
    if (entry->comp == CTR_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_hitbit_ctrl_notify(unit,
                                                 false, /* delete operation */
                                                 true,  /* group action entry */
                                                 (void *)entry));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_ctr_eflex_group_action_lt_stage(int unit,
                                       bool ingress,
                                       eflex_comp_t comp,
                                       uint64_t req_flags,
                                       bcmimm_entry_event_t event_reason,
                                       const bcmltd_field_t *key_fields,
                                       const bcmltd_field_t *data_fields,
                                       bcmltd_fields_t *out)
{
    int select;
    ctr_eflex_group_action_data_t entry = {0};
    ctr_eflex_group_action_data_t cur_entry = {0};
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
        (bcmcth_ctr_eflex_group_action_entry_init(unit, &entry));

    /* Parse the key fields to get group profile id */
    while (key_fields) {
        if (key_fields->id == group_profile_id_fid[comp][select]) {
            entry.group_profile_id = key_fields->data;
        }
        key_fields = key_fields->next;
    }

    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_group_action_lt_entry_insert(unit,
                                                               data_fields,
                                                               &entry));
            break;

        case BCMIMM_ENTRY_UPDATE:
            cur_entry.ingress = ingress;
            cur_entry.comp = comp;
            cur_entry.req_flags = req_flags;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_group_action_entry_init(unit, &cur_entry));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_group_action_lt_entry_update(unit,
                                                               data_fields,
                                                               &entry,
                                                               &cur_entry));
            break;

        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_group_action_lt_entry_delete(unit,
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
            /* Set OPERATIONAL_STATE to INACTIVE */
            out->field[out->count++]->data =
                                    ctrl->ctr_eflex_enum.oper_state_inactive;
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                    "Action Id entry not found in ACTION_PROFILE LT, "
                    "Setting operational state to INACTIVE.\n")));
        } else {
            /* Set OPERATIONAL_STATE to ACTIVE */
            out->field[out->count++]->data =
                                    ctrl->ctr_eflex_enum.oper_state_active;
        }
    }

    if (SHR_FUNC_VAL_IS(SHR_E_NOT_FOUND)) {
        return SHR_E_NONE;
    }

    SHR_FUNC_EXIT();
}

/*! Function to lookup GROUP_ACTION_PROFILE IMM LT entry. */
static int
bcmcth_ctr_eflex_group_action_lt_lookup(int unit,
                                        bool ingress,
                                        eflex_comp_t comp,
                                        uint64_t req_flags,
                                        bcmimm_lookup_type_t lkup_type,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out)
{
    int select;
    size_t num_actions, i;
    uint8_t pipe_unique;
    ctr_eflex_action_profile_data_t action_entry = {0};
    ctr_eflex_group_action_data_t entry = {0};
    const bcmltd_fields_t *key_fields = NULL;
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;

    SHR_FUNC_ENTER(unit);
    select = ingress ? 0 : 1;
    action_entry.ingress = entry.ingress = ingress;
    action_entry.comp = entry.comp = comp;
    entry.req_flags = req_flags;

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            entry.ingress,
                                            entry.comp,
                                            &pipe_unique));

    device_info = ctrl->ctr_eflex_device_info;
    num_actions = entry.ingress ? device_info->num_ing_actions :
                                  device_info->num_egr_actions;

    /* key fields are in the "out" parameter for traverse operations */
    key_fields = (lkup_type == BCMIMM_TRAVERSE) ? out : in;

    /* Parse the key fields to get group id */
    for (i = 0; i < key_fields->count; i++) {
        if (key_fields->field[i]->id == group_profile_id_fid[comp][select]) {
            entry.group_profile_id = key_fields->field[i]->data;
        }
    }

    /* Parse IMM fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_group_action_entry_parse(unit, &entry, out));

    action_entry.action_profile_id = entry.action_profile_id;

    /* Lookup entry with given action profile id key in ACTION_PROFILE LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_lookup(unit, &action_entry));

    /*
     * For ingress flex state, check if group action id and associated
     * counter action id are configured for the same ingress instance.
     */
    if (entry.comp == STATE_EFLEX && entry.ingress == INGRESS &&
        entry.inst != action_entry.state_ext.inst) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "Flex state action id %d ingress inst %d NOT present in"
                "group action id %d ingress inst %d egr_inst %d\n"),
            action_entry.action_id, action_entry.state_ext.inst,
            entry.action_id, entry.inst, entry.egr_inst));
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
    if (entry.comp == STATE_EFLEX && entry.ingress == EGRESS &&
        entry.egr_inst != action_entry.state_ext.egr_inst) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "Flex state action id %d ingress inst %d NOT present in"
                "group action id %d egress inst %d egr_inst %d\n"),
            action_entry.action_id, action_entry.state_ext.egr_inst,
            entry.action_id, entry.inst, entry.egr_inst));
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }


    /*
     * Check if each associated action profile id exists in ACTION_PROFILE LT.
     * If no, return error and set OPERATIONAL_STATE as INACTIVE.
     */
    for (i = 0; i < num_actions; i++) {
        sal_memset(&action_entry, 0, sizeof(ctr_eflex_action_profile_data_t));
        action_entry.ingress = entry.ingress;
        action_entry.comp = entry.comp;

        if (entry.action_profile_id_map[i] !=
                CTR_EFLEX_INVALID_ACTION_PROFILE_ID) {
            action_entry.action_profile_id = entry.action_profile_id_map[i];

            /*
             * Lookup entry with given action profile id key
             * in ACTION_PROFILE LT
             */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_action_profile_lookup(unit, &action_entry));

            /*
             * In pipe unique mode, check if group action id and associated
             * counter action id are configured for the same pipe.
             */
            if (pipe_unique && entry.pipe_idx != action_entry.pipe_idx) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                        "Action id %d pipe idx %d NOT present in"
                        "group action id %d pipe idx %d\n"),
                    action_entry.action_id, action_entry.pipe_idx,
                    entry.action_id, entry.pipe_idx));
                SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
            }

            /*
             * For ingress flex state, check if group action id and associated
             * counter action id are configured for the same ingress instance.
             */
            if (entry.comp == STATE_EFLEX && entry.ingress == INGRESS &&
                entry.inst != action_entry.state_ext.inst) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                        "Flex state action id %d ingress inst %d NOT present in"
                        "group action id %d ingress inst %d egr_inst %d\n"),
                    action_entry.action_id, action_entry.state_ext.inst,
                    entry.action_id, entry.inst, entry.egr_inst));
                SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
            }
            if (entry.comp == STATE_EFLEX && entry.ingress == EGRESS &&
                entry.egr_inst != action_entry.state_ext.egr_inst) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                        "Flex state action id %d ingress inst %d NOT present in"
                        "group action id %d egress inst %d egr_inst %d\n"),
                    action_entry.action_id, action_entry.state_ext.egr_inst,
                    entry.action_id, entry.inst, entry.egr_inst));
                SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
            }

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
            (void)bcmcth_ctr_eflex_oper_state_set(
                    unit,
                    oper_state_fid[comp][select],
                    ctrl->ctr_eflex_enum.oper_state_active,
                    out);
        }
    }

    if (SHR_FUNC_VAL_IS(SHR_E_NOT_FOUND)) {
        return SHR_E_NONE;
    }

    SHR_FUNC_EXIT();
}

/*! Function to get GROUP_ACTION_PROFILE IMM LT entry limit value. */
static int
bcmcth_ctr_eflex_group_action_lt_ent_limit_get(int unit,
                                               bool ingress,
                                               eflex_comp_t comp,
                                               uint32_t *count)
{
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;
    const bcmlrd_map_t *map = NULL;
    uint8_t pipe_unique;
    uint32_t num_pipes, k;
    size_t ent_limit;
    int rv, select;

    SHR_FUNC_ENTER(unit);
    select = ingress ? 0 : 1;

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

    num_pipes = pipe_unique ? 1: device_info->num_pipes;

    /* Check if LT is supported for given device */
    rv = bcmlrd_map_get(unit,
                        group_action_profile_table_sid[comp][select],
                        &map);

    if (SHR_SUCCESS(rv) && map) {
        for (k = 0; k < map->table_attr->attributes; k++) {
            if (map->table_attr->attr[k].key ==
                    BCMLRD_MAP_TABLE_ATTRIBUTE_ENTRY_LIMIT) {
                ent_limit = map->table_attr->attr[k].value;
                *count = ent_limit / num_pipes;
                break;
            }
        }
    } else {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit,"GROUP_ACTION_PROFILE LT not supported.\n")));
        *count = 0;

    }

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit,"comp %d ingress %d entry_limit %d\n"),
        comp, ingress, *count));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * IMM event handler
 */

/***********************
 * INGRESS IMM Functions
 */

/*!
 * \brief Lookup callback function of CTR_ING_EFLEX_GROUP_ACTION_PROFILE
 *  IMM event handler.
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
bcmcth_ctr_eflex_ing_group_action_lt_lookup_cb(int unit,
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
        if (ctrl->ctr_eflex_field_info[t].lt_sid ==
            CTR_ING_EFLEX_GROUP_ACTION_PROFILEt) {
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
        (bcmcth_ctr_eflex_group_action_lt_lookup(unit,
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
 * \brief Stage callback function of CTR_ING_EFLEX_GROUP_ACTION_PROFILE
 *        to insert, update, delete IMM LT entry.
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
bcmcth_ctr_eflex_ing_group_action_lt_stage_cb(int unit,
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
        (bcmcth_ctr_eflex_group_action_lt_stage(unit,
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
 * \brief Entry limit get callback function of
 *        CTR_ING_EFLEX_GROUP_ACTION_PROFILE
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] count Entry limit value.
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_ctr_eflex_ing_group_action_lt_ent_limit_get_cb(int unit,
                                                      uint32_t trans_id,
                                                      bcmltd_sid_t sid,
                                                      uint32_t *count)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_group_action_lt_ent_limit_get(unit,
                                                        INGRESS,
                                                        CTR_EFLEX,
                                                        count));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Lookup callback function of FLEX_STATE_ING_GROUP_ACTION_PROFILE
 *  IMM event handler.
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
bcmcth_state_eflex_ing_group_action_lt_lookup_cb(int unit,
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
        if (ctrl->ctr_eflex_field_info[t].lt_sid ==
            FLEX_STATE_ING_GROUP_ACTION_PROFILEt) {
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
        (bcmcth_ctr_eflex_group_action_lt_lookup(unit,
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
 * \brief Stage callback function of FLEX_STATE_ING_GROUP_ACTION_PROFILE
 *        to insert, update, delete IMM LT entry.
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
bcmcth_state_eflex_ing_group_action_lt_stage_cb(
    int unit,
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
        (bcmcth_ctr_eflex_group_action_lt_stage(unit,
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
 * \brief Entry limit get callback function of FLEX_STATE_ING_ACTION_PROFILE
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] count Entry limit value.
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_state_eflex_ing_group_action_lt_ent_limit_get_cb(int unit,
                                                        uint32_t trans_id,
                                                        bcmltd_sid_t sid,
                                                        uint32_t *count)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_group_action_lt_ent_limit_get(unit,
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
 * \brief Lookup callback function of CTR_EGR_EFLEX_GROUP_ACTION_PROFILE
 *  IMM event handler.
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
bcmcth_ctr_eflex_egr_group_action_lt_lookup_cb(int unit,
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
        if (ctrl->ctr_eflex_field_info[t].lt_sid ==
            CTR_EGR_EFLEX_GROUP_ACTION_PROFILEt) {
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
        (bcmcth_ctr_eflex_group_action_lt_lookup(unit,
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
 * \brief Stage callback function of CTR_EGR_EFLEX_GROUP_ACTION_PROFILE
 *        to insert, update, delete IMM LT entry.
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
bcmcth_ctr_eflex_egr_group_action_lt_stage_cb(int unit,
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
        (bcmcth_ctr_eflex_group_action_lt_stage(unit,
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
 * \brief Entry limit get callback function of
 *        CTR_EGR_EFLEX_GROUP_ACTION_PROFILE
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] count Entry limit value.
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_ctr_eflex_egr_group_action_lt_ent_limit_get_cb(int unit,
                                                      uint32_t trans_id,
                                                      bcmltd_sid_t sid,
                                                      uint32_t *count)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_group_action_lt_ent_limit_get(unit,
                                                        EGRESS,
                                                        CTR_EFLEX,
                                                        count));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Lookup callback function of FLEX_STATE_EGR_GROUP_ACTION_PROFILE
 *  IMM event handler.
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
bcmcth_state_eflex_egr_group_action_lt_lookup_cb(int unit,
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
        if (ctrl->ctr_eflex_field_info[t].lt_sid ==
            FLEX_STATE_EGR_GROUP_ACTION_PROFILEt) {
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
        (bcmcth_ctr_eflex_group_action_lt_lookup(unit,
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
 * \brief Stage callback function of FLEX_STATE_EGR_GROUP_ACTION_PROFILE
 *        to insert, update, delete IMM LT entry.
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
bcmcth_state_eflex_egr_group_action_lt_stage_cb(
    int unit,
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
        (bcmcth_ctr_eflex_group_action_lt_stage(unit,
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
 * \brief Entry limit get callback function of FLEX_STATE_EGR_ACTION_PROFILE
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] count Entry limit value.
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_state_eflex_egr_group_action_lt_ent_limit_get_cb(int unit,
                                                        uint32_t trans_id,
                                                        bcmltd_sid_t sid,
                                                        uint32_t *count)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_group_action_lt_ent_limit_get(unit,
                                                        EGRESS,
                                                        STATE_EFLEX,
                                                        count));

exit:
    SHR_FUNC_EXIT();
}

/**********************************************************************
* Public functions
 */

/*
 * Function to register callback handlers for
 * CTR_xxx_EFLEX_GROUP_ACTION_PROFILE LTs
 */
int
bcmcth_ctr_eflex_imm_group_action_register(int unit)
{
    bcmimm_lt_cb_t ctr_eflex_lt_cb;

    SHR_FUNC_ENTER(unit);

    /* Register CTR_ING_EFLEX_GROUP_ACTION_PROFILE handler */
    sal_memset(&ctr_eflex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
    ctr_eflex_lt_cb.op_stage  = bcmcth_ctr_eflex_ing_group_action_lt_stage_cb;
    ctr_eflex_lt_cb.op_lookup = bcmcth_ctr_eflex_ing_group_action_lt_lookup_cb;
    ctr_eflex_lt_cb.ent_limit_get  =
        bcmcth_ctr_eflex_ing_group_action_lt_ent_limit_get_cb;

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmimm_lt_event_reg(unit,
                            CTR_ING_EFLEX_GROUP_ACTION_PROFILEt,
                            &ctr_eflex_lt_cb,
                            NULL));

    /* Register CTR_EGR_EFLEX_GROUP_ACTION_PROFILE handler */
    sal_memset(&ctr_eflex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
    ctr_eflex_lt_cb.op_stage  = bcmcth_ctr_eflex_egr_group_action_lt_stage_cb;
    ctr_eflex_lt_cb.op_lookup = bcmcth_ctr_eflex_egr_group_action_lt_lookup_cb;
    ctr_eflex_lt_cb.ent_limit_get  =
        bcmcth_ctr_eflex_egr_group_action_lt_ent_limit_get_cb;

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmimm_lt_event_reg(unit,
                            CTR_EGR_EFLEX_GROUP_ACTION_PROFILEt,
                            &ctr_eflex_lt_cb,
                            NULL));

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function to register callback handlers for
 * STATE_xxx_EFLEX_GROUP_ACTION_PROFILE LTs
 */
int
bcmcth_state_eflex_imm_group_action_register(int unit)
{
    bcmimm_lt_cb_t state_eflex_lt_cb;

    SHR_FUNC_ENTER(unit);

    /* Register FLEX_STATE_ING_GROUP_ACTION_PROFILE handler */
    sal_memset(&state_eflex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
    state_eflex_lt_cb.op_stage  =
        bcmcth_state_eflex_ing_group_action_lt_stage_cb;
    state_eflex_lt_cb.op_lookup =
        bcmcth_state_eflex_ing_group_action_lt_lookup_cb;
    state_eflex_lt_cb.ent_limit_get  =
        bcmcth_state_eflex_ing_group_action_lt_ent_limit_get_cb;

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmimm_lt_event_reg(unit,
                            FLEX_STATE_ING_GROUP_ACTION_PROFILEt,
                            &state_eflex_lt_cb,
                            NULL));

    /* Register FLEX_STATE_EGR_GROUP_ACTION_PROFILE handler */
    sal_memset(&state_eflex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
    state_eflex_lt_cb.op_stage  =
        bcmcth_state_eflex_egr_group_action_lt_stage_cb;
    state_eflex_lt_cb.op_lookup =
        bcmcth_state_eflex_egr_group_action_lt_lookup_cb;
    state_eflex_lt_cb.ent_limit_get  =
        bcmcth_state_eflex_egr_group_action_lt_ent_limit_get_cb;

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmimm_lt_event_reg(unit,
                            FLEX_STATE_EGR_GROUP_ACTION_PROFILEt,
                            &state_eflex_lt_cb,
                            NULL));

exit:
    SHR_FUNC_EXIT();
}
