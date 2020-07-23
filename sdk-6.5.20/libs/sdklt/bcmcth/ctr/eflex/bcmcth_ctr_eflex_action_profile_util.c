/*! \file bcmcth_ctr_eflex_action_profile_util.c
 *
 * BCMCTH CTR_EFLEX_ACTION_PROFILEt IMM handler
 * internal utility functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <bcmltd/chip/bcmltd_id.h>

#include <bcmcth/bcmcth_ctr_eflex_internal.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_CTREFLEX

/* Function to get action id resource list */
static int
bcmcth_ctr_eflex_action_id_list_get(int unit,
                                    uint32_t pipe_idx,
                                    ctr_eflex_action_profile_data_t *entry,
                                    ctr_eflex_pool_list_t **list)
{
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;
    int egr_num_instance = 0;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));
    device_info = ctrl->ctr_eflex_device_info;
    egr_num_instance = device_info->state_ext.num_egress_instance;

    switch (entry->comp) {
        case CTR_EFLEX:
            *list = entry->ingress ? &ctrl->ing_action_id_list[pipe_idx] :
                                     &ctrl->egr_action_id_list[pipe_idx];
            break;
        case STATE_EFLEX:
            if (entry->ingress) {
                switch (entry->state_ext.inst) {
                    case ING_POST_LKUP_INST:
                        *list = &ctrl->ing_action_id_list[pipe_idx];
                        break;
                    case ING_POST_FWD_INST:
                        *list =
                        &ctrl->state_ext.ing_action_id_list_b[pipe_idx];
                        break;
                    default:
                        SHR_ERR_EXIT(SHR_E_PARAM);
                } /* end inner SWITCH */
            } else {
                if (egr_num_instance == 1 ) {
                    *list = &ctrl->egr_action_id_list[pipe_idx];
                } else {
                    switch (entry->state_ext.egr_inst) {
                        case EGR_POST_LKUP_INST:
                            *list = &ctrl->egr_action_id_list[pipe_idx];
                             break;
                        case EGR_POST_FWD_INST:
                            *list =
                            &ctrl->state_ext.egr_action_id_list_b[pipe_idx];
                            break;
                        default:
                            SHR_ERR_EXIT(SHR_E_PARAM);
                   } /* end inner SWITCH */
                }
            }
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    } /* end outer SWITCH */

exit:
    SHR_FUNC_EXIT();
}

/* Function to validate action id in ACTION_PROFILE entry */
int
bcmcth_ctr_eflex_action_id_validate(int unit,
                                    ctr_eflex_action_profile_data_t *entry)
{
    uint32_t pipe_idx;
    ctr_eflex_pool_list_t *list = NULL;

    SHR_FUNC_ENTER(unit);

    /* Check if given action id is already in use in all applicable pipes */
    for (pipe_idx = entry->start_pipe_idx;
         pipe_idx < entry->end_pipe_idx;
         pipe_idx++) {

        /* Get action id resource list */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_action_id_list_get(unit,
                                                 pipe_idx,
                                                 entry,
                                                 &list));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_id_validate(unit, entry->action_id, list));
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to set/clear action id in resource list */
int
bcmcth_ctr_eflex_action_id_set(int unit,
                               bool val,
                               ctr_eflex_action_profile_data_t *entry)
{
    uint32_t pipe_idx;
    ctr_eflex_pool_list_t *list = NULL;

    SHR_FUNC_ENTER(unit);

    /* Set/Clear given action id in all applicable pipes */
    for (pipe_idx = entry->start_pipe_idx;
         pipe_idx < entry->end_pipe_idx;
         pipe_idx++) {

        /* Get action id resource list */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_action_id_list_get(unit,
                                                 pipe_idx,
                                                 entry,
                                                 &list));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_id_set(unit, val, entry->action_id, list));
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to validate the update modes for WIDE counter mode */
static int
bcmcth_ctr_eflex_wide_update_mode_validate_helper(
    int unit,
    bcmptm_cci_update_mode_t ctr_update_mode)
{
    SHR_FUNC_ENTER(unit);

    switch (ctr_update_mode) {
        case NOOP:
        case SET_TO_CVALUE:
        case INC_BY_CVALUE:
        case SETBIT:
            break;
        default:
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,"Invalid update mode %d for WIDE mode\n"),
                ctr_update_mode));
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to validate the update modes for given counter mode */
int
bcmcth_ctr_eflex_update_mode_validate(int unit,
                                      ctr_eflex_action_profile_data_t *entry)
{
    SHR_FUNC_ENTER(unit);

    /* Check for valid update modes for given counter mode
     * NORMAL_MODE - all update modes are valid
     *               except SETBIT for flex counter
     * WIDE_MODE - for ctr A only -- set, increment and setbit modes are valid
     * SLIM_MODE - for ctr A only -- set and increment are valid
     */
    if (entry->ctr_mode == WIDE_MODE) {
        if (entry->comp == CTR_EFLEX) {
            /* Validate ctr A update mode */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_wide_update_mode_validate_helper(
                    unit,
                    entry->ctr_update_mode[UPDATE_MODE_A_IDX]));
        } else if (entry->comp == STATE_EFLEX) {
            /* Validate ctr A update true mode */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_wide_update_mode_validate_helper(
                    unit,
                    entry->state_ext.upd_true_mode[UPDATE_MODE_A_IDX]));

            /* Validate ctr A update false mode */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_wide_update_mode_validate_helper(
                    unit,
                    entry->state_ext.upd_false_mode[UPDATE_MODE_A_IDX]));
        }
    } else if (entry->ctr_mode == SLIM_MODE) {
        switch (entry->ctr_update_mode[UPDATE_MODE_A_IDX]) {
            case NOOP:
            case SET_TO_CVALUE:
            case INC_BY_CVALUE:
                break;
            default:
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,"Invalid update mode %d for SLIM mode\n"),
                    entry->ctr_update_mode[UPDATE_MODE_A_IDX]));
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } if (entry->comp == CTR_EFLEX && entry->ctr_mode == NORMAL_MODE &&
          (entry->ctr_update_mode[UPDATE_MODE_A_IDX] == SETBIT ||
           entry->ctr_update_mode[UPDATE_MODE_B_IDX] == SETBIT)) {
        /* SETBIT update mode is invalid for flex counter in NORMAL mode */
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit,"Invalid SETBIT counter update mode "
                             "for NORMAL mode\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
   if (entry->comp == CTR_EFLEX  &&
      (entry->ctr_update_mode[UPDATE_MODE_A_IDX] == CLRBIT ||
       entry->ctr_update_mode[UPDATE_MODE_B_IDX] == XORBIT)) {
       /* CLR_BIT ANDXOR_BIT are valid only for flex state */
       LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit,"Invalid CLR_BIT/XOR_BIT counter update mode "
                             "for flex counter \n")));
       SHR_ERR_EXIT(SHR_E_PARAM);
   }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function to validate ACTION_PROFILE LT entry
 * if slim mode is configured
 */
int
bcmcth_ctr_eflex_slim_mode_validate(int unit,
                                    ctr_eflex_action_profile_data_t *entry,
                                    ctr_eflex_action_profile_data_t *cur_entry)
{
    SHR_FUNC_ENTER(unit);

    /*
     * For SLIM mode, if base index auto is disabled,
     * then base index must always be 0
     */
    if (!entry->base_idx_auto && entry->base_idx) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Base idx %d must be 0 for SLIM mode\n"),
            entry->base_idx));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /*
     * If entry is updated and same pool id is used,
     * then skip pool empty check and return success.
     */
    if (cur_entry && cur_entry->pool_idx == entry->pool_idx) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Updated entry requests same pool idx\n")));
        SHR_EXIT();
    }

    /*
     * For SLIM mode, the requested pool must be empty
     * since same pool cannot be used with another counter mode
     */
    if (!bcmcth_ctr_eflex_pool_mgr_is_pool_free(unit, entry)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Pool idx %d must be empty for SLIM mode\n"),
            entry->pool_idx));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}
/*
 * Function to adjust number of counters for given counter mode
 * and update mode
 */
int
bcmcth_ctr_eflex_action_profile_num_ctrs_adjust(
    int unit,
    ctr_eflex_action_profile_data_t *entry)
{
    uint32_t max_ctr_num, num_slim_ctrs, num_pools;
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;

    if (entry->ctr_mode == SLIM_MODE) {
        /*
         * If SLIM mode is configured,
         * then num_ctrs must always be in multiples of complete pools
         */

        /* Get number of slim counters per pool entry */
        num_slim_ctrs = device_info->num_slim_ctrs;

        /* Get max number of counters per pool */
        max_ctr_num = entry->ingress ?
                      ctrl->ing_pool_ctr_num[entry->pool_idx] :
                      ctrl->egr_pool_ctr_num[entry->pool_idx];
        if (device_info->slim_ctr_row == TRUE)  {
        /* Slim counters are calculated row wise and we dont need to reserve full pool */
            entry->num_ctrs =  entry->num_ctrs % device_info->num_slim_ctrs ?
                           entry->num_ctrs / device_info->num_slim_ctrs + 1 :
                           entry->num_ctrs / device_info->num_slim_ctrs;

        } else {
        /* Calculate number of complete pools required */
           num_pools = entry->num_ctrs % (max_ctr_num * num_slim_ctrs) ?
                   (entry->num_ctrs / (max_ctr_num * num_slim_ctrs)) + 1 :
                   entry->num_ctrs / (max_ctr_num * num_slim_ctrs);

           entry->num_ctrs = num_pools * max_ctr_num;
        }
    }
    if (entry->ctr_mode == NORMAL_DOUBLE_MODE) {

        entry->num_ctrs = entry->num_ctrs%2 ?
                             entry->num_ctrs/2 + 1:
                             entry->num_ctrs/2;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function to get OP_PROFILE profile for corresponding
 * OPERAND_PROFILE LT index. If not found, set it to 0.
 */
int
bcmcth_ctr_eflex_action_profile_op_update(
    int unit,
    uint32_t op_profile_idx,
    bool *found,
    ctr_eflex_op_profile_data_t *op_entry,
    ctr_eflex_action_profile_data_t *entry)
{

    SHR_FUNC_ENTER(unit);

    /*
     * The same op profile id maybe configured for multiple operand profile ids
     * in given ACTION_PROFILE entry. Hence check against all operand profile
     * ids in the ACTION_PROFILE entry.
     */

    /* Check OP_PROFILE profile id to calculate counter index */
    if (op_entry->op_profile_id == entry->op_profile_id_ctr_index) {
        *found = true;
        entry->op_profile_ctr_index = op_profile_idx;
    }

    if (entry->comp == CTR_EFLEX) {
        /* Check OP_PROFILE profile id to calculate counter A value */
        if (op_entry->op_profile_id == entry->op_profile_id_ctr_a) {
            *found = true;
            entry->op_profile_ctr_a = op_profile_idx;
        }

        /* Check OP_PROFILE profile id to calculate counter B value */
        if (op_entry->op_profile_id == entry->op_profile_id_ctr_b) {
            *found = true;
            entry->op_profile_ctr_b = op_profile_idx;
        }
    } else if (entry->comp == STATE_EFLEX) {
        /* Check OP_PROFILE profile to calculate counter value for comparison */
        if (op_entry->op_profile_id ==
            entry->state_ext.op_profile_id_comp[UPDATE_MODE_A_IDX]) {
            *found = true;
            entry->state_ext.op_profile_comp[UPDATE_MODE_A_IDX] =
                                                                op_profile_idx;
        }

        if (op_entry->op_profile_id ==
            entry->state_ext.op_profile_id_comp[UPDATE_MODE_B_IDX]) {
            *found = true;
            entry->state_ext.op_profile_comp[UPDATE_MODE_B_IDX] =
                                                                op_profile_idx;
        }

        /*
         * Get OP_PROFILE profile to calculate counter value
         * if comparison operation returns true
         */
        if (op_entry->op_profile_id ==
            entry->state_ext.op_profile_id_upd_true[UPDATE_MODE_A_IDX]) {
            *found = true;
            entry->state_ext.op_profile_upd_true[UPDATE_MODE_A_IDX] =
                                                                op_profile_idx;
        }

        if (op_entry->op_profile_id ==
            entry->state_ext.op_profile_id_upd_true[UPDATE_MODE_B_IDX]) {
            *found = true;
            entry->state_ext.op_profile_upd_true[UPDATE_MODE_B_IDX] =
                                                                op_profile_idx;
        }

        /*
         * Get OP_PROFILE profile to calculate counter value
         * if comparison operation returns false
         */
        if (op_entry->op_profile_id ==
            entry->state_ext.op_profile_id_upd_false[UPDATE_MODE_A_IDX]) {
            *found = true;
            entry->state_ext.op_profile_upd_false[UPDATE_MODE_A_IDX] =
                                                                op_profile_idx;
        }

        if (op_entry->op_profile_id ==
            entry->state_ext.op_profile_id_upd_false[UPDATE_MODE_B_IDX]) {
            *found = true;
            entry->state_ext.op_profile_upd_false[UPDATE_MODE_B_IDX] =
                                                                op_profile_idx;
        }
    }

    SHR_FUNC_EXIT();
}

/* Helper function to get OPERAND_PROFILE profile */
static int
bcmcth_ctr_eflex_action_profile_op_profile_get_helper(int unit,
                                                      bool ingress,
                                                      eflex_comp_t comp,
                                                      uint32_t pipe_idx,
                                                      state_eflex_inst_t inst,
                                                      uint32_t op_profile_id,
                                                      uint32_t *op_profile)
{
    int rv;
    uint8_t pipe_unique;
    ctr_eflex_op_profile_data_t op_entry = {0};
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;
    int egr_num_instance = 0;

    SHR_FUNC_ENTER(unit);
    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));
    device_info = ctrl->ctr_eflex_device_info;
    egr_num_instance = device_info->state_ext.num_egress_instance;

    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            ingress,
                                            comp,
                                            &pipe_unique));

    op_entry.ingress = ingress;
    op_entry.comp = comp;
    op_entry.op_profile_id = op_profile_id;

    /* Lookup OPERAND_PROFILE IMM LT to get op profile index */
    rv = bcmcth_ctr_eflex_op_profile_lookup(unit, &op_entry);

    /*
     * In pipe unique mode, if ACTION_PROFILE and OPERAND_PROFILE LT
     * entries are not configured for the same pipe, then
     * return OPERAND_PROFILE LT entry as not found.
     */
    if (pipe_unique && op_entry.pipe_idx != pipe_idx) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "Op profile id %d pipe idx %d NOT same as "
                "action profile pipe idx %d\n"),
            op_entry.op_profile_id, op_entry.pipe_idx, pipe_idx));
        rv = SHR_E_NOT_FOUND;
    }

    /*
     * For ingress flex state, if ACTION_PROFILE and OPERAND_PROFILE LT
     * entries are not configured for the same instance, then
     * return OPERAND_PROFILE LT entry as not found.
     */
    if (comp == STATE_EFLEX && ingress == INGRESS && op_entry.inst != inst) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "Flex state op profile id %d ingress inst %d NOT same as "
                "action profile ingress inst %d\n"),
            op_entry.op_profile_id, op_entry.inst, inst));
        rv = SHR_E_NOT_FOUND;
    }
    if (egr_num_instance > 1) {
         if (comp == STATE_EFLEX && ingress == EGRESS &&
             op_entry.egr_inst != (state_eflex_egr_inst_t) inst) {
             LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                    "Flex state op profile id %d egress inst %d NOT same as "
                    "action profile egress inst %d\n"),
                 op_entry.op_profile_id, op_entry.egr_inst, (state_eflex_egr_inst_t)inst));
             rv = SHR_E_NOT_FOUND;
         }
    }

    *op_profile = SHR_SUCCESS(rv) ? op_entry.op_profile_index : 0;

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function to get OPERAND_PROFILE profile for corresponding
 * OPERAND_PROFILE LT index. If not found, set it to 0.
 */
int
bcmcth_ctr_eflex_action_profile_op_profile_get(
    int unit,
    ctr_eflex_action_profile_data_t *entry)
{

    SHR_FUNC_ENTER(unit);

    /* Get OP_PROFILE profile to calculate counter index */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_op_profile_get_helper(
            unit,
            entry->ingress,
            entry->comp,
            entry->pipe_idx,
            entry->ingress ?  entry->state_ext.inst :
            entry->state_ext.egr_inst,
            entry->op_profile_id_ctr_index,
            &entry->op_profile_ctr_index));

    if (entry->comp == CTR_EFLEX) {
        /* Get OP_PROFILE profile to calculate counter A value */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_action_profile_op_profile_get_helper(
                unit,
                entry->ingress,
                entry->comp,
                entry->pipe_idx,
                entry->state_ext.inst,
                entry->op_profile_id_ctr_a,
                &entry->op_profile_ctr_a));

        /* Get OP_PROFILE profile to calculate counter B value */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_action_profile_op_profile_get_helper(
                unit,
                entry->ingress,
                entry->comp,
                entry->pipe_idx,
                entry->state_ext.inst,
                entry->op_profile_id_ctr_b,
                &entry->op_profile_ctr_b));
    } else if (entry->comp == STATE_EFLEX) {
        /* Get OP_PROFILE profile to calculate counter value for comparison */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_action_profile_op_profile_get_helper(
                unit,
                entry->ingress,
                entry->comp,
                entry->pipe_idx,
                entry->ingress ?  entry->state_ext.inst :
                entry->state_ext.egr_inst,
                entry->state_ext.op_profile_id_comp[UPDATE_MODE_A_IDX],
                &entry->state_ext.op_profile_comp[UPDATE_MODE_A_IDX]));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_action_profile_op_profile_get_helper(
                unit,
                entry->ingress,
                entry->comp,
                entry->pipe_idx,
                entry->ingress ?  entry->state_ext.inst :
                entry->state_ext.egr_inst,
                entry->state_ext.op_profile_id_comp[UPDATE_MODE_B_IDX],
                &entry->state_ext.op_profile_comp[UPDATE_MODE_B_IDX]));

        /*
         * Get OP_PROFILE profile to calculate counter value
         * if comparison operation returns true
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_action_profile_op_profile_get_helper(
                unit,
                entry->ingress,
                entry->comp,
                entry->pipe_idx,
                entry->ingress ?  entry->state_ext.inst :
                entry->state_ext.egr_inst,
                entry->state_ext.op_profile_id_upd_true[UPDATE_MODE_A_IDX],
                &entry->state_ext.op_profile_upd_true[UPDATE_MODE_A_IDX]));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_action_profile_op_profile_get_helper(
                unit,
                entry->ingress,
                entry->comp,
                entry->pipe_idx,
                entry->ingress ?  entry->state_ext.inst :
                entry->state_ext.egr_inst,
                entry->state_ext.op_profile_id_upd_true[UPDATE_MODE_B_IDX],
                &entry->state_ext.op_profile_upd_true[UPDATE_MODE_B_IDX]));

        /*
         * Get OP_PROFILE profile to calculate counter value
         * if comparison operation returns false
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_action_profile_op_profile_get_helper(
                unit,
                entry->ingress,
                entry->comp,
                entry->pipe_idx,
                entry->ingress ?  entry->state_ext.inst :
                entry->state_ext.egr_inst,
                entry->state_ext.op_profile_id_upd_false[UPDATE_MODE_A_IDX],
                &entry->state_ext.op_profile_upd_false[UPDATE_MODE_A_IDX]));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_action_profile_op_profile_get_helper(
                unit,
                entry->ingress,
                entry->comp,
                entry->pipe_idx,
                entry->ingress ?  entry->state_ext.inst :
                entry->state_ext.egr_inst,
                entry->state_ext.op_profile_id_upd_false[UPDATE_MODE_B_IDX],
                &entry->state_ext.op_profile_upd_false[UPDATE_MODE_B_IDX]));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function to determine if shadow config is enabled or disabled
 * during ACTION_PROFILE LT update operation.
 */
int
bcmcth_ctr_eflex_action_profile_shadow_update_get(
    int unit,
    ctr_eflex_action_profile_data_t *entry,
    ctr_eflex_action_profile_data_t *cur_entry,
    ctr_eflex_shadow_config_t *shadow)
{
    SHR_FUNC_ENTER(unit);

    /* Shadow config is not updated */
    *shadow = SHADOW_NOOP;

    /* Check if shadow is valid and being updated */
    if (entry->shadow != CTR_EFLEX_INVALID_VAL &&
        entry->shadow != cur_entry->shadow) {
        *shadow = (entry->shadow) ? SHADOW_ENABLE:  SHADOW_DISABLE;
    }

    SHR_FUNC_EXIT();
}
