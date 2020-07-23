/*! \file bcmcth_ctr_eflex_imm.c
 *
 * BCMCTH CTR_EFLEX_ACTION_PROFILEt IMM handler.
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

/*******************************************************************************
 * Private Functions
 */

/* Function to initialize ACTION_PROFILE entry with invalid values */
static int
bcmcth_ctr_eflex_action_profile_entry_init(
    int unit,
    ctr_eflex_action_profile_data_t *entry)
{
    int i, num_objs;
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;
    num_objs = device_info->num_objs;

    entry->action_profile_id = CTR_EFLEX_INVALID_VAL;
    entry->action_id = CTR_EFLEX_INVALID_VAL;
    entry->pool_idx = CTR_EFLEX_INVALID_VAL;
    entry->pipe_idx = CTR_EFLEX_INVALID_VAL;
    entry->base_idx = CTR_EFLEX_INVALID_VAL;
    entry->base_idx_auto = CTR_EFLEX_INVALID_VAL;
    entry->shadow = CTR_EFLEX_INVALID_VAL;
    entry->num_ctrs = CTR_EFLEX_INVALID_VAL;
    entry->ctr_mode = CTR_EFLEX_INVALID_VAL;
    entry->ctr_update_mode[UPDATE_MODE_A_IDX] = CTR_EFLEX_INVALID_VAL;
    entry->ctr_update_mode[UPDATE_MODE_B_IDX] = CTR_EFLEX_INVALID_VAL;

    for (i = 0; i < num_objs; i++) {
        entry->obj[i] = CTR_EFLEX_INVALID_VAL;
    }
    entry->op_profile_id_ctr_index = CTR_EFLEX_INVALID_VAL;
    entry->count_on_rule_drop = CTR_EFLEX_INVALID_VAL;
    entry->ctr_val_data = CTR_EFLEX_INVALID_VAL;
    entry->count_on_hw_drop = CTR_EFLEX_INVALID_VAL;
    entry->count_on_mirror_egress = CTR_EFLEX_INVALID_VAL;

    entry->zone = CTR_EFLEX_INVALID_VAL;
    entry->op_profile_id_ctr_a = CTR_EFLEX_INVALID_VAL;
    entry->op_profile_id_ctr_b = CTR_EFLEX_INVALID_VAL;
    entry->ctr_src_a = CTR_EFLEX_INVALID_VAL;
    entry->ctr_src_b = CTR_EFLEX_INVALID_VAL;
    entry->evict_comp = CTR_EFLEX_INVALID_VAL;
    entry->evict_reset = CTR_EFLEX_INVALID_VAL;

    entry->state_ext.inst = CTR_EFLEX_INVALID_VAL;
    entry->state_ext.egr_inst = CTR_EFLEX_INVALID_VAL;
    entry->state_ext.upd_compare_op[UPDATE_MODE_A_IDX] =
                                                    CTR_EFLEX_INVALID_VAL;
    entry->state_ext.upd_compare_op[UPDATE_MODE_B_IDX] =
                                                    CTR_EFLEX_INVALID_VAL;
    entry->state_ext.upd_true_mode[UPDATE_MODE_A_IDX] =
                                                    CTR_EFLEX_INVALID_VAL;
    entry->state_ext.upd_true_mode[UPDATE_MODE_B_IDX] =
                                                    CTR_EFLEX_INVALID_VAL;
    entry->state_ext.upd_false_mode[UPDATE_MODE_A_IDX] =
                                                    CTR_EFLEX_INVALID_VAL;
    entry->state_ext.upd_false_mode[UPDATE_MODE_B_IDX] =
                                                    CTR_EFLEX_INVALID_VAL;
    entry->state_ext.op_profile_id_comp[UPDATE_MODE_A_IDX] =
                                                    CTR_EFLEX_INVALID_VAL;
    entry->state_ext.op_profile_id_comp[UPDATE_MODE_B_IDX] =
                                                    CTR_EFLEX_INVALID_VAL;
    entry->state_ext.op_profile_id_upd_true[UPDATE_MODE_A_IDX] =
                                                    CTR_EFLEX_INVALID_VAL;
    entry->state_ext.op_profile_id_upd_true[UPDATE_MODE_B_IDX] =
                                                    CTR_EFLEX_INVALID_VAL;
    entry->state_ext.op_profile_id_upd_false[UPDATE_MODE_A_IDX] =
                                                    CTR_EFLEX_INVALID_VAL;
    entry->state_ext.op_profile_id_upd_false[UPDATE_MODE_B_IDX] =
                                                    CTR_EFLEX_INVALID_VAL;
    entry->state_ext.ctr_src_comp[UPDATE_MODE_A_IDX] =
                                                    CTR_EFLEX_INVALID_VAL;
    entry->state_ext.ctr_src_comp[UPDATE_MODE_B_IDX] =
                                                    CTR_EFLEX_INVALID_VAL;
    entry->state_ext.ctr_src_upd_true[UPDATE_MODE_A_IDX] =
                                                    CTR_EFLEX_INVALID_VAL;
    entry->state_ext.ctr_src_upd_true[UPDATE_MODE_B_IDX] =
                                                    CTR_EFLEX_INVALID_VAL;
    entry->state_ext.ctr_src_upd_false[UPDATE_MODE_A_IDX] =
                                                    CTR_EFLEX_INVALID_VAL;
    entry->state_ext.ctr_src_upd_false[UPDATE_MODE_B_IDX] =
                                                    CTR_EFLEX_INVALID_VAL;

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function to replace ACTION_PROFILE entry with values from another
 * entry if is invalid
 * entry -- LT entry with updated values provided by application
 * cur_entry -- current LT entry present in the table.
 */
static int
bcmcth_ctr_eflex_action_profile_fill(int unit,
                                     ctr_eflex_action_profile_data_t *entry,
                                     ctr_eflex_action_profile_data_t *cur_entry)
{
    int i, num_objs;
    uint8_t pipe_unique;
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;
    num_objs = device_info->num_objs;

    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            entry->ingress,
                                            entry->comp,
                                            &pipe_unique));

    if (entry->action_id == CTR_EFLEX_INVALID_VAL) {
        entry->action_id = cur_entry->action_id;
    }

    if (entry->pool_idx == CTR_EFLEX_INVALID_VAL) {
        entry->pool_idx = cur_entry->pool_idx;
    }

    if (pipe_unique && entry->pipe_idx == CTR_EFLEX_INVALID_VAL) {
        entry->pipe_idx = cur_entry->pipe_idx;
    }

    if (entry->base_idx == CTR_EFLEX_INVALID_VAL) {
        entry->base_idx = cur_entry->base_idx;
    }

    if (entry->base_idx_auto == CTR_EFLEX_INVALID_VAL) {
        entry->base_idx_auto = cur_entry->base_idx_auto;
    }

    if (entry->base_idx_oper == CTR_EFLEX_INVALID_VAL) {
        entry->base_idx_oper = cur_entry->base_idx_oper;
    }

    if (entry->shadow == CTR_EFLEX_INVALID_VAL) {
        entry->shadow = cur_entry->shadow;
    }

    if (entry->num_ctrs == CTR_EFLEX_INVALID_VAL) {
        entry->num_ctrs = cur_entry->num_ctrs;
    }

    if (entry->ctr_mode == CTR_EFLEX_INVALID_VAL) {
        entry->ctr_mode = cur_entry->ctr_mode;
    }

    for (i = 0; i < num_objs; i++) {
        if (entry->obj[i] == CTR_EFLEX_INVALID_VAL) {
            entry->obj[i] = cur_entry->obj[i];
        }
    }

    if (entry->op_profile_id_ctr_index == CTR_EFLEX_INVALID_VAL) {
        entry->op_profile_id_ctr_index = cur_entry->op_profile_id_ctr_index;
    }

    if (entry->count_on_rule_drop == CTR_EFLEX_INVALID_VAL) {
        entry->count_on_rule_drop = cur_entry->count_on_rule_drop;
    }

    if (entry->ctr_val_data == CTR_EFLEX_INVALID_VAL) {
        entry->ctr_val_data = cur_entry->ctr_val_data;
    }

    if (entry->count_on_hw_drop == CTR_EFLEX_INVALID_VAL) {
        entry->count_on_hw_drop = cur_entry->count_on_hw_drop;
    }

    if (entry->ingress == EGRESS && cur_entry->ingress == EGRESS &&
        entry->count_on_mirror_egress == CTR_EFLEX_INVALID_VAL) {
        entry->count_on_mirror_egress = cur_entry->count_on_mirror_egress;
    }

    /* Flex counter data */
    if (entry->comp == CTR_EFLEX) {
        if (entry->zone == CTR_EFLEX_INVALID_VAL) {
            entry->zone = cur_entry->zone;
        }

        if (entry->ctr_update_mode[UPDATE_MODE_A_IDX] ==
                                                        CTR_EFLEX_INVALID_VAL) {
            entry->ctr_update_mode[UPDATE_MODE_A_IDX] =
                cur_entry->ctr_update_mode[UPDATE_MODE_A_IDX];
        }

        if (entry->ctr_update_mode[UPDATE_MODE_B_IDX] ==
                                                        CTR_EFLEX_INVALID_VAL) {
            entry->ctr_update_mode[UPDATE_MODE_B_IDX] =
                cur_entry->ctr_update_mode[UPDATE_MODE_B_IDX];
        }

        if (entry->op_profile_id_ctr_a == CTR_EFLEX_INVALID_VAL) {
            entry->op_profile_id_ctr_a = cur_entry->op_profile_id_ctr_a;
        }

        if (entry->op_profile_id_ctr_b == CTR_EFLEX_INVALID_VAL) {
            entry->op_profile_id_ctr_b = cur_entry->op_profile_id_ctr_b;
        }
        if (entry->ctr_src_a == CTR_EFLEX_INVALID_VAL) {
            entry->ctr_src_a = cur_entry->ctr_src_a;
        }

        if (entry->ctr_src_b == CTR_EFLEX_INVALID_VAL) {
            entry->ctr_src_b = cur_entry->ctr_src_b;
        }
        if (entry->evict_comp == CTR_EFLEX_INVALID_VAL) {
            entry->evict_comp = cur_entry->evict_comp;
        }

        if (entry->evict_reset == CTR_EFLEX_INVALID_VAL) {
            entry->evict_reset = cur_entry->evict_reset;
        }
    }

    /* Flex state data */
    if (entry->comp == STATE_EFLEX) {
        if (entry->state_ext.inst == CTR_EFLEX_INVALID_VAL) {
            entry->state_ext.inst = cur_entry->state_ext.inst;
        }

        if (entry->state_ext.inst == CTR_EFLEX_INVALID_VAL) {
            entry->state_ext.egr_inst = cur_entry->state_ext.egr_inst;
        }

        if (entry->state_ext.upd_compare_op[UPDATE_MODE_A_IDX] ==
                                                        CTR_EFLEX_INVALID_VAL) {
            entry->state_ext.upd_compare_op[UPDATE_MODE_A_IDX] =
                         cur_entry->state_ext.upd_compare_op[UPDATE_MODE_A_IDX];
        }

        if (entry->state_ext.upd_compare_op[UPDATE_MODE_B_IDX] ==
                                                        CTR_EFLEX_INVALID_VAL) {
            entry->state_ext.upd_compare_op[UPDATE_MODE_B_IDX] =
                         cur_entry->state_ext.upd_compare_op[UPDATE_MODE_B_IDX];
        }

        if (entry->state_ext.upd_true_mode[UPDATE_MODE_A_IDX] ==
                                                        CTR_EFLEX_INVALID_VAL) {
            entry->state_ext.upd_true_mode[UPDATE_MODE_A_IDX] =
                         cur_entry->state_ext.upd_true_mode[UPDATE_MODE_A_IDX];
        }

        if (entry->state_ext.upd_true_mode[UPDATE_MODE_B_IDX] ==
                                                        CTR_EFLEX_INVALID_VAL) {
            entry->state_ext.upd_true_mode[UPDATE_MODE_B_IDX] =
                         cur_entry->state_ext.upd_true_mode[UPDATE_MODE_B_IDX];
        }

        if (entry->state_ext.upd_false_mode[UPDATE_MODE_A_IDX] ==
                                                        CTR_EFLEX_INVALID_VAL) {
            entry->state_ext.upd_false_mode[UPDATE_MODE_A_IDX] =
                         cur_entry->state_ext.upd_false_mode[UPDATE_MODE_A_IDX];
        }

        if (entry->state_ext.upd_false_mode[UPDATE_MODE_B_IDX] ==
                                                        CTR_EFLEX_INVALID_VAL) {
            entry->state_ext.upd_false_mode[UPDATE_MODE_B_IDX] =
                         cur_entry->state_ext.upd_false_mode[UPDATE_MODE_B_IDX];
        }

        if (entry->state_ext.op_profile_id_comp[UPDATE_MODE_A_IDX] ==
                                                        CTR_EFLEX_INVALID_VAL) {
            entry->state_ext.op_profile_id_comp[UPDATE_MODE_A_IDX] =
                    cur_entry->state_ext.op_profile_id_comp[UPDATE_MODE_A_IDX];
        }

        if (entry->state_ext.op_profile_id_comp[UPDATE_MODE_B_IDX] ==
                                                        CTR_EFLEX_INVALID_VAL) {
            entry->state_ext.op_profile_id_comp[UPDATE_MODE_B_IDX] =
                    cur_entry->state_ext.op_profile_id_comp[UPDATE_MODE_B_IDX];
        }

        if (entry->state_ext.op_profile_id_upd_true[UPDATE_MODE_A_IDX] ==
                                                        CTR_EFLEX_INVALID_VAL) {
            entry->state_ext.op_profile_id_upd_true[UPDATE_MODE_A_IDX] =
                cur_entry->state_ext.op_profile_id_upd_true[UPDATE_MODE_A_IDX];
        }

        if (entry->state_ext.op_profile_id_upd_true[UPDATE_MODE_B_IDX] ==
                                                        CTR_EFLEX_INVALID_VAL) {
            entry->state_ext.op_profile_id_upd_true[UPDATE_MODE_B_IDX] =
                cur_entry->state_ext.op_profile_id_upd_true[UPDATE_MODE_B_IDX];
        }

        if (entry->state_ext.op_profile_id_upd_false[UPDATE_MODE_A_IDX] ==
                                                        CTR_EFLEX_INVALID_VAL) {
            entry->state_ext.op_profile_id_upd_false[UPDATE_MODE_A_IDX] =
                cur_entry->state_ext.op_profile_id_upd_false[UPDATE_MODE_A_IDX];
        }

        if (entry->state_ext.op_profile_id_upd_false[UPDATE_MODE_B_IDX] ==
                                                        CTR_EFLEX_INVALID_VAL) {
            entry->state_ext.op_profile_id_upd_false[UPDATE_MODE_B_IDX] =
                cur_entry->state_ext.op_profile_id_upd_false[UPDATE_MODE_B_IDX];
        }

        if (entry->state_ext.ctr_src_comp[UPDATE_MODE_A_IDX] ==
                                                        CTR_EFLEX_INVALID_VAL) {
            entry->state_ext.ctr_src_comp[UPDATE_MODE_A_IDX] =
                        cur_entry->state_ext.ctr_src_comp[UPDATE_MODE_A_IDX];
        }

        if (entry->state_ext.ctr_src_comp[UPDATE_MODE_B_IDX] ==
                                                        CTR_EFLEX_INVALID_VAL) {
            entry->state_ext.ctr_src_comp[UPDATE_MODE_B_IDX] =
                        cur_entry->state_ext.ctr_src_comp[UPDATE_MODE_B_IDX];
        }

        if (entry->state_ext.ctr_src_upd_true[UPDATE_MODE_A_IDX] ==
                                                        CTR_EFLEX_INVALID_VAL) {
            entry->state_ext.ctr_src_upd_true[UPDATE_MODE_A_IDX] =
                    cur_entry->state_ext.ctr_src_upd_true[UPDATE_MODE_A_IDX];
        }

        if (entry->state_ext.ctr_src_upd_true[UPDATE_MODE_B_IDX] ==
                                                        CTR_EFLEX_INVALID_VAL) {
            entry->state_ext.ctr_src_upd_true[UPDATE_MODE_B_IDX] =
                    cur_entry->state_ext.ctr_src_upd_true[UPDATE_MODE_B_IDX];
        }

        if (entry->state_ext.ctr_src_upd_false[UPDATE_MODE_A_IDX] ==
                                                        CTR_EFLEX_INVALID_VAL) {
            entry->state_ext.ctr_src_upd_false[UPDATE_MODE_A_IDX] =
                    cur_entry->state_ext.ctr_src_upd_false[UPDATE_MODE_A_IDX];
        }

        if (entry->state_ext.ctr_src_upd_false[UPDATE_MODE_B_IDX] ==
                                                        CTR_EFLEX_INVALID_VAL) {
            entry->state_ext.ctr_src_upd_false[UPDATE_MODE_B_IDX] =
                    cur_entry->state_ext.ctr_src_upd_false[UPDATE_MODE_B_IDX];
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_ctr_eflex_action_profile_lt_fields_check(
    int unit,
    ctr_eflex_action_profile_data_t *entry,
    const bcmltd_field_t *data_fields)
{
    int select;
    uint8_t pipe_unique;
    eflex_comp_t comp;
    ctr_eflex_control_t *ctrl = NULL;
    ctr_eflex_action_profile_lt_info_t *lt_info = NULL;

    SHR_FUNC_ENTER(unit);

    select = entry->ingress ? 0 : 1;
    comp = entry->comp;

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            entry->ingress,
                                            entry->comp,
                                            &pipe_unique));

    /* Get LT fid info */
    lt_info = entry->ingress ? &ctrl->ing_action_lt_info :
                               &ctrl->egr_action_lt_info;

    /* data_fields contains the key field for get_first/next IMM API */
    if (data_fields->id == lt_info->action_profile_id_fid) {
        entry->action_profile_id = data_fields->data;
    } else if (data_fields->id == lt_info->action_fid) {
        entry->action_id = data_fields->data;
    } else if (pipe_unique &&
               data_fields->id == lt_info->pipe_idx_fid) {
        entry->pipe_idx = data_fields->data;
    } else if (data_fields->id == lt_info->pool_fid) {
        entry->pool_idx = data_fields->data;
    } else if (data_fields->id == lt_info->base_idx_auto_fid) {
        entry->base_idx_auto = data_fields->data;
    } else if (data_fields->id == lt_info->base_idx_fid) {
        entry->base_idx = data_fields->data;
    } else if (data_fields->id == lt_info->base_idx_oper_fid) {
        entry->base_idx_oper = data_fields->data;
    } else if (data_fields->id == lt_info->num_ctrs_fid) {
        entry->num_ctrs = data_fields->data;
    } else if (data_fields->id == lt_info->shadow_fid) {
        entry->shadow = data_fields->data;
    }  else if (data_fields->id == lt_info->ctr_mode_fid) {
        entry->ctr_mode = (bcmptm_cci_ctr_mode_t)data_fields->data;
    } else if (data_fields->id == lt_info->op_profile_id_index_fid) {
        entry->op_profile_id_ctr_index = data_fields->data;
    } else if (data_fields->id == lt_info->count_rule_drop_fid) {
        entry->count_on_rule_drop =
                    (ctr_eflex_drop_count_mode_t)data_fields->data;
    } else if (data_fields->id == lt_info->ctr_val_data_fid) {
        entry->ctr_val_data = data_fields->data;
    } else if (data_fields->id == lt_info->count_hw_drop_fid) {
        entry->count_on_hw_drop = data_fields->data;
    } else if (entry->ingress == EGRESS &&
               data_fields->id == lt_info->count_mirror_egress_fid) {
        entry->count_on_mirror_egress = data_fields->data;
    } else if (entry->comp == CTR_EFLEX) {
        /* Flex counter data */
        if (data_fields->id == lt_info->zone_fid) {
            entry->zone = (ctr_eflex_action_zone_t)data_fields->data;
        } else if (data_fields->id == lt_info->ctr_update_mode_a_fid) {
            entry->ctr_update_mode[UPDATE_MODE_A_IDX] =
                        (bcmptm_cci_update_mode_t)data_fields->data;
        } else if (data_fields->id == lt_info->ctr_update_mode_b_fid) {
            entry->ctr_update_mode[UPDATE_MODE_B_IDX] =
                        (bcmptm_cci_update_mode_t)data_fields->data;
        } else if (data_fields->id == lt_info->obj_fid) {
            entry->obj[data_fields->idx] = \
                bcmcth_ctr_eflex_obj_id_get(unit, select, data_fields->data);
        } else if (data_fields->id == lt_info->op_profile_id_a_fid) {
            entry->op_profile_id_ctr_a = data_fields->data;
        } else if (data_fields->id == lt_info->op_profile_id_b_fid) {
            entry->op_profile_id_ctr_b = data_fields->data;
        } else if (data_fields->id == lt_info->ctr_src_a_fid) {
            entry->ctr_src_a = (ctr_eflex_src_t)data_fields->data;
        } else if (data_fields->id == lt_info->ctr_src_b_fid) {
            entry->ctr_src_b = (ctr_eflex_src_t)data_fields->data;
        } else if (data_fields->id == lt_info->evict_comp_fid) {
            entry->evict_comp = (ctr_eflex_evict_comp_t)data_fields->data;
        } else if (data_fields->id == lt_info->evict_reset_fid) {
            entry->evict_reset = data_fields->data;
        }
    } else if (entry->comp == STATE_EFLEX) {
        /* Flex state data */
        if (entry->ingress == INGRESS &&
            data_fields->id == lt_info->state_ext.instance_fid) {
            entry->state_ext.inst = (state_eflex_inst_t)data_fields->data;
        } else if (entry->ingress == EGRESS &&
            lt_info->state_ext.egr_instance_fid_valid &&
            data_fields->id == lt_info->state_ext.egr_instance_fid) {
            entry->state_ext.egr_inst =
                                (state_eflex_egr_inst_t)data_fields->data;
        } else if (data_fields->id == lt_info->state_ext.update_comp_a_fid) {
            entry->state_ext.upd_compare_op[UPDATE_MODE_A_IDX] =
                                (state_eflex_compare_t)data_fields->data;
        } else if (data_fields->id == lt_info->state_ext.update_comp_b_fid) {
            entry->state_ext.upd_compare_op[UPDATE_MODE_B_IDX] =
                                (state_eflex_compare_t)data_fields->data;
        } else if (data_fields->id == lt_info->state_ext.upd_true_mode_a_fid) {
            entry->state_ext.upd_true_mode[UPDATE_MODE_A_IDX] =
                                (bcmptm_cci_update_mode_t)data_fields->data;
        } else if (data_fields->id == lt_info->state_ext.upd_true_mode_b_fid) {
            entry->state_ext.upd_true_mode[UPDATE_MODE_B_IDX] =
                                (bcmptm_cci_update_mode_t)data_fields->data;
        } else if (data_fields->id == lt_info->state_ext.upd_false_mode_a_fid) {
            entry->state_ext.upd_false_mode[UPDATE_MODE_A_IDX] =
                                (bcmptm_cci_update_mode_t)data_fields->data;
        } else if (data_fields->id == lt_info->state_ext.upd_false_mode_b_fid) {
            entry->state_ext.upd_false_mode[UPDATE_MODE_B_IDX] =
                                (bcmptm_cci_update_mode_t)data_fields->data;
        } else if (data_fields->id == lt_info->state_ext.op_id_comp_a_fid) {
            entry->state_ext.op_profile_id_comp[UPDATE_MODE_A_IDX] =
                                                        data_fields->data;
        } else if (data_fields->id == lt_info->state_ext.op_id_comp_b_fid) {
            entry->state_ext.op_profile_id_comp[UPDATE_MODE_B_IDX] =
                                                        data_fields->data;
        } else if (data_fields->id == lt_info->state_ext.op_id_upd_true_a_fid) {
            entry->state_ext.op_profile_id_upd_true[UPDATE_MODE_A_IDX] =
                                                        data_fields->data;
        } else if (data_fields->id == lt_info->state_ext.op_id_upd_true_b_fid) {
            entry->state_ext.op_profile_id_upd_true[UPDATE_MODE_B_IDX] =
                                                        data_fields->data;
        } else if (data_fields->id ==
                                    lt_info->state_ext.op_id_upd_false_a_fid) {
            entry->state_ext.op_profile_id_upd_false[UPDATE_MODE_A_IDX] =
                                                        data_fields->data;
        } else if (data_fields->id ==
                                    lt_info->state_ext.op_id_upd_false_b_fid) {
            entry->state_ext.op_profile_id_upd_false[UPDATE_MODE_B_IDX] =
                                                        data_fields->data;
        } else if (data_fields->id == lt_info->state_ext.ctr_src_comp_a_fid) {
            entry->state_ext.ctr_src_comp[UPDATE_MODE_A_IDX] =
                                        (ctr_eflex_src_t)data_fields->data;
        } else if (data_fields->id == lt_info->state_ext.ctr_src_comp_b_fid) {
            entry->state_ext.ctr_src_comp[UPDATE_MODE_B_IDX] =
                                        (ctr_eflex_src_t)data_fields->data;
        } else if (data_fields->id ==
                                lt_info->state_ext.ctr_src_upd_true_a_fid) {
            entry->state_ext.ctr_src_upd_true[UPDATE_MODE_A_IDX] =
                                        (ctr_eflex_src_t)data_fields->data;
        } else if (data_fields->id ==
                                lt_info->state_ext.ctr_src_upd_true_b_fid) {
            entry->state_ext.ctr_src_upd_true[UPDATE_MODE_B_IDX] =
                                        (ctr_eflex_src_t)data_fields->data;
        } else if (data_fields->id ==
                                lt_info->state_ext.ctr_src_upd_false_a_fid) {
            entry->state_ext.ctr_src_upd_false[UPDATE_MODE_A_IDX] =
                                        (ctr_eflex_src_t)data_fields->data;
        } else if (data_fields->id ==
                                lt_info->state_ext.ctr_src_upd_false_b_fid) {
            entry->state_ext.ctr_src_upd_false[UPDATE_MODE_B_IDX] =
                                        (ctr_eflex_src_t)data_fields->data;
        } else if (data_fields->id == lt_info->state_ext.a_mem_update_fid) {
            entry->state_ext.a_mem_update =
                                    (flex_state_update_src_t)data_fields->data;
        } else if (data_fields->id == lt_info->state_ext.b_mem_update_fid) {
            entry->state_ext.b_mem_update =
                                    (flex_state_update_src_t)data_fields->data;
        } else if (data_fields->id == lt_info->state_ext.a_bus_update_fid) {
            entry->state_ext.a_bus_update =
                                    (flex_state_update_src_t)data_fields->data;
        } else if (data_fields->id == lt_info->state_ext.b_bus_update_fid) {
            entry->state_ext.b_bus_update =
                                    (flex_state_update_src_t)data_fields->data;
        } else if (data_fields->id == lt_info->state_ext.truth_table_en_fid) {
            entry->state_ext.truth_table_en = (bool)data_fields->data;
        } else if (data_fields->id ==
                                lt_info->state_ext.truth_table_instance_fid) {
            entry->state_ext.truth_table_instance = (uint8_t)data_fields->data;
        } else if (data_fields->id == lt_info->obj_fid) {
            entry->obj[data_fields->idx] = \
                bcmcth_state_eflex_obj_id_get(unit,
                                              select,
                                              data_fields->data);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief ACTION_PROFILE IMM input LT fields parsing.
 *
 * Parse the input LT fields & save the data to entry provided as input.
 *
 * \param [in] unit Unit number.
 * \param [in] key_flds IMM input key field array.
 * \param [in] data_flds IMM input data field array.
 * \param [out] entry CTR control data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c in to \c pdata.
 */
static int
bcmcth_ctr_eflex_action_profile_lt_fields_parse(
    int unit,
    const bcmltd_field_t *key_fields,
    const bcmltd_field_t *data_fields,
    ctr_eflex_action_profile_data_t *entry)
{
    eflex_comp_t comp;
    ctr_eflex_control_t *ctrl = NULL;
    ctr_eflex_action_profile_lt_info_t *lt_info = NULL;

    SHR_FUNC_ENTER(unit);

    comp = entry->comp;

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    /* Get LT fid info */
    lt_info = entry->ingress ? &ctrl->ing_action_lt_info :
                               &ctrl->egr_action_lt_info;

    /* Parse key fields */
    while (key_fields) {
        if (key_fields->id == lt_info->action_profile_id_fid) {
            entry->action_profile_id = key_fields->data;
            break;
        }

        key_fields = key_fields->next;
    }

    /* Parse data fields */
    while (data_fields) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_action_profile_lt_fields_check(unit,
                                                             entry,
                                                             data_fields));

        data_fields = data_fields->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to parse ACTION_PROFILE IMM entry */
static int
bcmcth_ctr_eflex_action_profile_entry_parse(
    int unit,
    ctr_eflex_action_profile_data_t *entry,
    bcmltd_fields_t *flist)
{
    uint32_t i;
    bcmltd_field_t *data_fields = NULL;

    SHR_FUNC_ENTER(unit);

    /* extract action_id and other fields */
    for (i = 0; i < flist->count; i++) {
        data_fields = flist->field[i];

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_action_profile_lt_fields_check(unit,
                                                            entry,
                                                            data_fields));
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to get default values for ACTION_PROFILE entry */
static int
bcmcth_ctr_eflex_action_profile_default_get(
    int unit,
    ctr_eflex_action_profile_data_t *entry)
{
    bcmlrd_client_field_info_t *f_info = NULL;
    const bcmlrd_table_rep_t *t_info = NULL;
    size_t num_fid = 0, i;
    int select, num_objs, idx;
    uint8_t pipe_unique;
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;
    ctr_eflex_action_profile_lt_info_t *lt_info = NULL;

    SHR_FUNC_ENTER(unit);
    select  = entry->ingress ? 0 : 1;

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;
    num_objs = device_info->num_objs;

    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            entry->ingress,
                                            entry->comp,
                                            &pipe_unique));

    /* Get LT sid and fid info */
    lt_info = entry->ingress ? &ctrl->ing_action_lt_info :
                               &ctrl->egr_action_lt_info;

    SHR_IF_ERR_VERBOSE_EXIT(bcmlrd_field_count_get(unit,
                                                   lt_info->lt_sid,
                                                   &num_fid));

    t_info = bcmlrd_table_get(lt_info->lt_sid);
    SHR_NULL_CHECK(lt_info, SHR_E_BADID);

    SHR_ALLOC(f_info, sizeof(bcmlrd_client_field_info_t) * num_fid,
              "bcmcthCtrEflexActionAllocFields");
    SHR_NULL_CHECK(f_info, SHR_E_MEMORY);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_table_fields_def_get(unit,
                                     t_info->name,
                                     num_fid,
                                     f_info,
                                     &num_fid));

    for (i = 0; i < num_fid; i++) {
        if (f_info[i].id == lt_info->action_fid) {
            entry->action_id = f_info[i].def.u64;
        } else if (pipe_unique &&
                   f_info[i].id == lt_info->pipe_idx_fid) {
            entry->pipe_idx = f_info[i].def.u8;
        } else if (f_info[i].id == lt_info->pool_fid) {
            entry->pool_idx = f_info[i].def.u8;
        } else if (f_info[i].id == lt_info->base_idx_auto_fid) {
            entry->base_idx_auto = f_info[i].def.u8;
        } else if (f_info[i].id == lt_info->base_idx_fid) {
            entry->base_idx = f_info[i].def.u8;
        } else if (f_info[i].id == lt_info->base_idx_oper_fid) {
            entry->base_idx_oper = f_info[i].def.u8;
        } else if (f_info[i].id == lt_info->num_ctrs_fid) {
            entry->num_ctrs = f_info[i].def.u32;
        } else if (f_info[i].id == lt_info->shadow_fid) {
            entry->shadow = f_info[i].def.u8;
        }  else if (f_info[i].id == lt_info->ctr_mode_fid) {
            entry->ctr_mode = (bcmptm_cci_ctr_mode_t)f_info[i].def.u32;
        } else if (f_info[i].id == lt_info->op_profile_id_index_fid) {
            entry->op_profile_id_ctr_index = f_info[i].def.u16;
        } else if (f_info[i].id == lt_info->count_rule_drop_fid) {
            entry->count_on_rule_drop =
                                (ctr_eflex_drop_count_mode_t)f_info[i].def.u32;
        } else if (f_info[i].id == lt_info->ctr_val_data_fid) {
            entry->ctr_val_data = f_info[i].def.u8;
        } else if (f_info[i].id == lt_info->count_hw_drop_fid) {
            entry->count_on_hw_drop = f_info[i].def.u8;
        } else if (entry->ingress == EGRESS &&
                   f_info[i].id == lt_info->count_mirror_egress_fid) {
            entry->count_on_mirror_egress = f_info[i].def.u8;
        } else if (entry->comp == CTR_EFLEX) {
            /* Flex counter data */
            if (f_info[i].id == lt_info->zone_fid) {
                entry->zone = f_info[i].def.u64;
            } else if (f_info[i].id == lt_info->ctr_update_mode_a_fid) {
                entry->ctr_update_mode[UPDATE_MODE_A_IDX] =
                                    (bcmptm_cci_update_mode_t)f_info[i].def.u32;
            } else if (f_info[i].id == lt_info->ctr_update_mode_b_fid) {
                entry->ctr_update_mode[UPDATE_MODE_B_IDX] =
                                    (bcmptm_cci_update_mode_t)f_info[i].def.u32;
            } else if (f_info[i].id == lt_info->op_profile_id_a_fid) {
                entry->op_profile_id_ctr_a = f_info[i].def.u16;
            } else if (f_info[i].id == lt_info->op_profile_id_b_fid) {
                entry->op_profile_id_ctr_b = f_info[i].def.u16;
            } else if (f_info[i].id == lt_info->ctr_src_a_fid) {
                entry->ctr_src_a = (ctr_eflex_src_t)f_info[i].def.u32;
            } else if (f_info[i].id == lt_info->ctr_src_b_fid) {
                entry->ctr_src_b = (ctr_eflex_src_t)f_info[i].def.u32;
            } else if (f_info[i].id == lt_info->evict_comp_fid) {
                entry->evict_comp = (ctr_eflex_evict_comp_t)f_info[i].def.u32;
            } else if (f_info[i].id == lt_info->evict_reset_fid) {
                entry->evict_reset = f_info[i].def.u8;
            } else if (f_info[i].id == lt_info->obj_fid) {
                for (idx = 0; idx < num_objs; idx++) {
                    entry->obj[idx] = \
                        bcmcth_ctr_eflex_obj_id_get(unit,
                                                    select,
                                                    f_info[i].def.u32);
                }
            }
        } else if (entry->comp == STATE_EFLEX) {
            /* Flex state data */
            if (entry->ingress == INGRESS &&
                f_info[i].id == lt_info->state_ext.instance_fid) {
                entry->state_ext.inst = (state_eflex_inst_t)f_info[i].def.u32;
            } else if (!entry->ingress &&
                lt_info->state_ext.egr_instance_fid_valid &&
                f_info[i].id == lt_info->state_ext.egr_instance_fid) {
                entry->state_ext.egr_inst =
                                    (state_eflex_egr_inst_t)f_info[i].def.u32;
            } else if (f_info[i].id == lt_info->state_ext.update_comp_a_fid) {
                entry->state_ext.upd_compare_op[UPDATE_MODE_A_IDX] =
                                    (state_eflex_compare_t)f_info[i].def.u32;
            } else if (f_info[i].id == lt_info->state_ext.update_comp_b_fid) {
                entry->state_ext.upd_compare_op[UPDATE_MODE_B_IDX] =
                                    (state_eflex_compare_t)f_info[i].def.u32;
            } else if (f_info[i].id == lt_info->state_ext.upd_true_mode_a_fid) {
                entry->state_ext.upd_true_mode[UPDATE_MODE_A_IDX] =
                                    (bcmptm_cci_update_mode_t)f_info[i].def.u32;
            } else if (f_info[i].id == lt_info->state_ext.upd_true_mode_b_fid) {
                entry->state_ext.upd_true_mode[UPDATE_MODE_B_IDX] =
                                    (bcmptm_cci_update_mode_t)f_info[i].def.u32;
            } else if (f_info[i].id ==
                                    lt_info->state_ext.upd_false_mode_a_fid) {
                entry->state_ext.upd_false_mode[UPDATE_MODE_A_IDX] =
                                    (bcmptm_cci_update_mode_t)f_info[i].def.u32;
            } else if (f_info[i].id ==
                                    lt_info->state_ext.upd_false_mode_b_fid) {
                entry->state_ext.upd_false_mode[UPDATE_MODE_B_IDX] =
                                    (bcmptm_cci_update_mode_t)f_info[i].def.u32;
            } else if (f_info[i].id == lt_info->state_ext.op_id_comp_a_fid) {
                entry->state_ext.op_profile_id_comp[UPDATE_MODE_A_IDX] =
                                                            f_info[i].def.u16;
            } else if (f_info[i].id == lt_info->state_ext.op_id_comp_b_fid) {
                entry->state_ext.op_profile_id_comp[UPDATE_MODE_B_IDX] =
                                                            f_info[i].def.u16;
            } else if (f_info[i].id ==
                                    lt_info->state_ext.op_id_upd_true_a_fid) {
                entry->state_ext.op_profile_id_upd_true[UPDATE_MODE_A_IDX] =
                                                            f_info[i].def.u16;
            } else if (f_info[i].id ==
                                    lt_info->state_ext.op_id_upd_true_b_fid) {
                entry->state_ext.op_profile_id_upd_true[UPDATE_MODE_B_IDX] =
                                                            f_info[i].def.u16;
            } else if (f_info[i].id ==
                                    lt_info->state_ext.op_id_upd_false_a_fid) {
                entry->state_ext.op_profile_id_upd_false[UPDATE_MODE_A_IDX] =
                                                            f_info[i].def.u16;
            } else if (f_info[i].id ==
                                    lt_info->state_ext.op_id_upd_false_b_fid) {
                entry->state_ext.op_profile_id_upd_false[UPDATE_MODE_B_IDX] =
                                                            f_info[i].def.u16;
            } else if (f_info[i].id == lt_info->state_ext.ctr_src_comp_a_fid) {
                entry->state_ext.ctr_src_comp[UPDATE_MODE_A_IDX] =
                                            (ctr_eflex_src_t)f_info[i].def.u32;
            } else if (f_info[i].id == lt_info->state_ext.ctr_src_comp_b_fid) {
                entry->state_ext.ctr_src_comp[UPDATE_MODE_B_IDX] =
                                            (ctr_eflex_src_t)f_info[i].def.u32;
            } else if (f_info[i].id ==
                                    lt_info->state_ext.ctr_src_upd_true_a_fid) {
                entry->state_ext.ctr_src_upd_true[UPDATE_MODE_A_IDX] =
                                            (ctr_eflex_src_t)f_info[i].def.u32;
            } else if (f_info[i].id ==
                                    lt_info->state_ext.ctr_src_upd_true_b_fid) {
                entry->state_ext.ctr_src_upd_true[UPDATE_MODE_B_IDX] =
                                            (ctr_eflex_src_t)f_info[i].def.u32;
            } else if (f_info[i].id ==
                                lt_info->state_ext.ctr_src_upd_false_a_fid) {
                entry->state_ext.ctr_src_upd_false[UPDATE_MODE_A_IDX] =
                                            (ctr_eflex_src_t)f_info[i].def.u32;
            } else if (f_info[i].id ==
                                lt_info->state_ext.ctr_src_upd_false_b_fid) {
                entry->state_ext.ctr_src_upd_false[UPDATE_MODE_B_IDX] =
                                            (ctr_eflex_src_t)f_info[i].def.u32;
            } else if (f_info[i].id == lt_info->state_ext.a_mem_update_fid) {
                entry->state_ext.a_mem_update =
                                (flex_state_update_src_t)f_info[i].def.u32;
            } else if (f_info[i].id == lt_info->state_ext.b_mem_update_fid) {
                entry->state_ext.b_mem_update =
                                (flex_state_update_src_t)f_info[i].def.u32;
            } else if (f_info[i].id == lt_info->state_ext.a_bus_update_fid) {
                entry->state_ext.a_bus_update =
                                (flex_state_update_src_t)f_info[i].def.u32;
            } else if (f_info[i].id == lt_info->state_ext.b_bus_update_fid) {
                entry->state_ext.b_bus_update =
                                (flex_state_update_src_t)f_info[i].def.u32;
            } else if (f_info[i].id == lt_info->state_ext.truth_table_en_fid) {
                entry->state_ext.truth_table_en = (bool)f_info[i].def.u32;
            } else if (f_info[i].id ==
                                lt_info->state_ext.truth_table_instance_fid) {
                entry->state_ext.truth_table_instance =
                                            (uint8_t)f_info[i].def.u32;
            } else if (f_info[i].id == lt_info->obj_fid) {
                for (idx = 0; idx < num_objs; idx++) {
                    entry->obj[idx] = \
                        bcmcth_state_eflex_obj_id_get(unit,
                                                      select,
                                                      f_info[i].def.u32);
                }
            }
        } else {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,"Unmatched field id %d\n"),f_info[i].id));
        }
    }

exit:
    SHR_FREE(f_info);
    SHR_FUNC_EXIT();
}

/*
 * Function to lookup ACTION_PROFILE table with given action id.
 * It populates the LT entry with all fields except the key field
 * which is provided within the entry as input.
 */
int
bcmcth_ctr_eflex_action_profile_lookup(int unit,
                                       ctr_eflex_action_profile_data_t *entry)
{
    bcmltd_fields_t key_fields = {0};
    bcmltd_fields_t imm_fields = {0};
    size_t num_fields = 0, num_key_fields;
    int num_objs;
    uint32_t t;
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;
    eflex_comp_t comp;
    ctr_eflex_action_profile_lt_info_t *lt_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    comp = entry->comp;

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;
    num_objs = device_info->num_objs;

    /* Get LT fid info */
    lt_info = entry->ingress ? &ctrl->ing_action_lt_info :
                               &ctrl->egr_action_lt_info;

    /* Assign default values */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_default_get(unit, entry));

    for (t = 0; t < ctrl->lt_info_num; t++) {
        if (ctrl->ctr_eflex_field_info[t].lt_sid == lt_info->lt_sid) {
            break;
        }
    }

    if (t == ctrl->lt_info_num) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "ACTION_PROFILE LT info not found.\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    num_key_fields = ctrl->ctr_eflex_field_info[t].num_key_fields;

    /* Allocate key field */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_fields_alloc(unit,
                                       &key_fields,
                                       num_key_fields));

    key_fields.count = num_key_fields;
    key_fields.field[0]->id   = lt_info->action_profile_id_fid;
    key_fields.field[0]->data = entry->action_profile_id;

    /* Allocate imm fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit, lt_info->lt_sid, &num_fields));

    /*
     * Adjust num_fields for array size
     * since LRD API counts field array size as 1
     */
    num_fields += num_objs - 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_fields_alloc(unit, &imm_fields, num_fields));

    /* lookup IMM table; error if entry not found */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit,
                             lt_info->lt_sid,
                             &key_fields,
                             &imm_fields));

    /* Parse IMM entry fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_entry_parse(unit, entry, &imm_fields));

exit:
    bcmcth_ctr_eflex_fields_free(unit, &key_fields);
    /* imm_fields.count may be modified by bcmimm_entry_lookup */
    imm_fields.count = num_fields;
    bcmcth_ctr_eflex_fields_free(unit, &imm_fields);

    SHR_FUNC_EXIT();
}

/* Function to help insert ACTION_PROFILE entry based on cold or warm boot  */
static int
bcmcth_ctr_eflex_action_profile_insert_helper(
    int unit,
    bool warm,
    uint32_t *base_idx,
    ctr_eflex_action_profile_data_t *entry)
{
    ctr_eflex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));

    /* Determine first and last pipe number */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_num_pipes_get(unit,
                                        entry->ingress,
                                        entry->comp,
                                        entry->pipe_idx,
                                        &entry->start_pipe_idx,
                                        &entry->end_pipe_idx));

    if (!warm) {
        /* Validate action id on all applicable pipes */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_action_id_validate(unit, entry));

        /* Check for valid update modes for given counter mode */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_update_mode_validate(unit, entry));

        /* Validate entry if slim mode is configured */
        if (entry->ctr_mode == SLIM_MODE) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_slim_mode_validate(unit, entry, NULL));
        }
    }

    /* Number of counter fields */
    entry->num_update_mode = entry->ingress ? ctrl->ing_ctr_field_num :
                                              ctrl->egr_ctr_field_num;

    /* Adjust num_ctrs if flex counter hitbit or SLIM mode is configured */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_num_ctrs_adjust(unit, entry));

    /* Debug print ACTION_PROFILE entry info */
    SHR_IF_ERR_VERBOSE_EXIT
          (bcmcth_eflex_action_profile_debug_print(unit, true, entry));

    /*
     * During warmboot, always reserve the counters
     * If base_idx_auto is enabled, use base_idx_oper set during cold boot
     * else use provided base_idx value
     */
    if (warm) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_pool_mgr_ctr_reserve(unit, warm, entry));
    } else {
        /*
         * During coldboot, if base_idx_auto is enabled,
         * calculate the base index value & allocate counters
         */
        if (entry->base_idx_auto) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_pool_mgr_ctr_alloc(unit,
                                                     warm,
                                                     entry,
                                                     base_idx));
        } else {
            /* Use the provided base index value to reserve counters */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_pool_mgr_ctr_reserve(unit, warm, entry));
        }
    }

    if (entry->base_idx_auto) {
        /* Update entry with calculated base index */
        entry->base_idx = *base_idx;
    }

    /* Allocate shadow pools */
    if (entry->shadow) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_pool_mgr_shadow_ctr_alloc(unit, warm, entry));
    }

    /* Write to PT and ACTION_PROFILE_INFO LT only during coldboot */
    if (!warm) {
        /* Get OPERAND_PROFILE profile */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_action_profile_op_profile_get(unit, entry));

        if (entry->comp == CTR_EFLEX) {
            /* Write to counter action tables */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_action_table_write(unit, entry));

            /* Set corresponding action id bit in action enable reg */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_action_reg_set(unit, ENABLE, entry));

            /* Set shadow pools registers */
            if (entry->shadow) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmcth_ctr_eflex_shadow_pool_set(unit, ENABLE, entry));
            }
        } else if (entry->comp == STATE_EFLEX) {
            /* Write to counter action tables */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_state_eflex_action_table_write(unit, entry));

            /* Set corresponding action id bit in action enable reg */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_state_eflex_action_reg_set(unit, ENABLE, entry));

            /* Set shadow pools registers */
            if (entry->shadow) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmcth_state_eflex_shadow_pool_set(unit, ENABLE, entry));
            }
        }

        /* Insert action profile info LT entry */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_action_profile_info_program(unit, true, entry));
    }

    /* Set action id bit in resource list */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_id_set(unit, ENABLE, entry));

exit:
    SHR_FUNC_EXIT();
}

/* Function to recover internal state from ACTION_PROFILE LT during warmboot */
int
bcmcth_ctr_eflex_action_profile_state_recover(int unit,
                                              bool ingress,
                                              eflex_comp_t comp)
{
    bcmltd_fields_t imm_fields = {0};
    bcmltd_fields_t key_fields = {0};
    uint32_t base_idx, t;
    size_t num_fields = 0, num_key_fields;
    int rv, num_objs;
    ctr_eflex_action_profile_data_t entry;
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;
    ctr_eflex_action_profile_lt_info_t *lt_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;
    num_objs = device_info->num_objs;

    /* Get LT sid and fid info */
    lt_info = ingress ? &ctrl->ing_action_lt_info :
                        &ctrl->egr_action_lt_info;

    for (t = 0; t < ctrl->lt_info_num; t++) {
        if (ctrl->ctr_eflex_field_info[t].lt_sid == lt_info->lt_sid) {
            break;
        }
    }

    if (t == ctrl->lt_info_num) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "ACTION_PROFILE LT info not found.\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    num_key_fields = ctrl->ctr_eflex_field_info[t].num_key_fields;

    /* Allocate key field */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_fields_alloc(unit,
                                       &key_fields,
                                       num_key_fields));

    key_fields.count = num_key_fields;
    key_fields.field[0]->id = lt_info->action_profile_id_fid;

    /* Allocate imm fields */
    SHR_IF_ERR_VERBOSE_EXIT(bcmlrd_field_count_get(unit,
                                                   lt_info->lt_sid,
                                                   &num_fields));

    /*
     * Adjust num_fields for array size
     * since LRD API counts field array size as 1
     */
    num_fields += num_objs - 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_fields_alloc(unit, &imm_fields, num_fields));

    /* Get first entry from ACTION_PROFILE IMM LT */
    rv = bcmimm_entry_get_first(unit, lt_info->lt_sid, &imm_fields);

    while (rv == SHR_E_NONE) {
        sal_memset(&entry, 0, sizeof(ctr_eflex_action_profile_data_t));
        entry.ingress = ingress;
        entry.comp = comp;

        /* Assign default values */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_action_profile_default_get(unit, &entry));

        /* Parse IMM entry fields */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_action_profile_entry_parse(unit,
                                                         &entry,
                                                         &imm_fields));

        /* Insert entry in PT */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_action_profile_insert_helper(unit,
                                                           true,
                                                           &base_idx,
                                                           &entry));

        /* Setup key for next entry */
        key_fields.field[0]->data = entry.action_profile_id;

        /* imm_fields.count may be modified. */
        imm_fields.count = num_fields;

        /* Get next entry from ACTION_PROFILE IMM LT */
        rv = bcmimm_entry_get_next(unit,
                                   lt_info->lt_sid,
                                   &key_fields,
                                   &imm_fields);
    } /* end WHILE */

    if (rv == SHR_E_NOT_FOUND) {
        rv = SHR_E_NONE;
    }

exit:
    bcmcth_ctr_eflex_fields_free(unit, &key_fields);
    /* imm_fields.count may be modified. */
    imm_fields.count = num_fields;
    bcmcth_ctr_eflex_fields_free(unit, &imm_fields);

    SHR_FUNC_EXIT();
}

/* Function to insert entry into ACTION_PROFILE IMM LT */
static int
bcmcth_ctr_eflex_action_profile_lt_entry_insert(
    int unit,
    const bcmltd_field_t *key_fields,
    const bcmltd_field_t *data_fields,
    ctr_eflex_action_profile_data_t *entry,
    uint32_t *base_idx)
{
    SHR_FUNC_ENTER(unit);

    /* Get default values */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_default_get(unit, entry));

    /*
     * Parse user values
     * default values will be over-written by user values, if present
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_lt_fields_parse(unit,
                                                         key_fields,
                                                         data_fields,
                                                         entry));

    /*
     * For flex state, since CCI does not know comparison operation result
     * always use SET update mode for both counters
     */
    if (entry->comp == STATE_EFLEX) {
        entry->ctr_update_mode[UPDATE_MODE_A_IDX] = SET_TO_CVALUE;
        entry->ctr_update_mode[UPDATE_MODE_B_IDX] = SET_TO_CVALUE;
    } else if (entry->comp == CTR_EFLEX &&
               (entry->ctr_mode == WIDE_MODE || entry->ctr_mode == SLIM_MODE)) {
        /*
         * For flex counter in WIDE or SLIM mode, CCI expects
         * both update modes should be same as UPDATE_MODE_A
         */
        entry->ctr_update_mode[UPDATE_MODE_B_IDX] =
                                    entry->ctr_update_mode[UPDATE_MODE_A_IDX];
    }

    /* Insert entry in PT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_insert_helper(unit,
                                                       false,
                                                       base_idx,
                                                       entry));

    /* Notify GROUP_ACTION_PROFILE LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_group_action_notify(unit, true, entry));

    /* Notify TRIGGER LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_trigger_notify(unit, true, entry));

    /* Notify HITBIT_CONTROL LT */
    if (entry->comp == CTR_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_hitbit_ctrl_notify(
                unit,
                true, /* insert operation */
                false, /* action profile entry */
                (void *)entry));
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to delete entry from ACTION_PROFILE IMM LT */
static int
bcmcth_ctr_eflex_action_profile_lt_entry_delete(
    int unit,
    bool hw_only,
    ctr_eflex_action_profile_data_t *entry)
{
    state_eflex_inst_t inst;
    state_eflex_egr_inst_t egr_inst;
    uint32_t action_id, shadow;
    ctr_eflex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));

    /* Lookup entry with given key fields */
     SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_lookup(unit, entry));

    /* Get action id and flex state instance for current entry */
    inst = entry->state_ext.inst;
    egr_inst = entry->state_ext.egr_inst;
    action_id = entry->action_id;
    shadow = entry->shadow;

    /* Number of counter fields */
    entry->num_update_mode = entry->ingress ? ctrl->ing_ctr_field_num :
                                              ctrl->egr_ctr_field_num;

    /* Determine first and last pipe number */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_num_pipes_get(unit,
                                        entry->ingress,
                                        entry->comp,
                                        entry->pipe_idx,
                                        &entry->start_pipe_idx,
                                        &entry->end_pipe_idx));

    if (!hw_only) {
        /* Adjust num_ctrs if flex counter hitbit or SLIM mode is configured */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_action_profile_num_ctrs_adjust(unit, entry));

        /* Free the counters */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_pool_mgr_ctr_free(unit, entry));

        /* Free shadow pool counters */
        if (entry->shadow) {
            /*
             * Lookup ACTION_PROFILE_INFO LT entry to get
             * primary & shadow pool info
             */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_action_profile_info_lookup(unit, entry));

            /* Free the shadow counters */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_pool_mgr_shadow_ctr_free(unit, false, entry));
        }

        /* Delete action profile info LT entry */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_action_profile_info_program(unit, false, entry));
    }

    /* Clear corresponding action id bit in action enable reg */
    if (entry->comp == CTR_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_action_reg_set(unit, DISABLE, entry));
    } else if (entry->comp == STATE_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_state_eflex_action_reg_set(unit, DISABLE, entry));
    }

    /* Clear action id bit in resource list */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_id_set(unit, DISABLE, entry));

    /* Get default values */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_default_get(unit, entry));

    /* Restore action id, flex state instance for current entry */
    entry->state_ext.inst = inst;
    entry->state_ext.egr_inst = egr_inst;
    entry->action_id = action_id;
    entry->shadow = shadow;

    /* Write to counter action tables */
    if (entry->comp == CTR_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_action_table_write(unit, entry));

        /* Clear shadow pools registers */
        if (entry->shadow) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_shadow_pool_set(unit, DISABLE, entry));
        }
    } else if (entry->comp == STATE_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_state_eflex_action_table_write(unit, entry));

        /* Clear shadow pools registers */
        if (entry->shadow) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_state_eflex_shadow_pool_set(unit, DISABLE, entry));
        }
    }

    if (!hw_only) {
        /* Notify GROUP_ACTION_PROFILE LT */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_group_action_notify(unit, false, entry));

        /* Notify TRIGGER LT */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_trigger_notify(unit, false, entry));

        /* Notify HITBIT_CONTROL LT */
        if (entry->comp == CTR_EFLEX) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_hitbit_ctrl_notify(
                    unit,
                    false, /* delete operation */
                    false, /* action profile entry */
                    (void *)entry));
        }

    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to update shadow pools in ACTION_PROFILE IMM LT */
static int
bcmcth_ctr_eflex_action_profile_shadow_update(
    int unit,
    ctr_eflex_shadow_config_t shadow,
    ctr_eflex_action_profile_data_t *cur_entry,
    ctr_eflex_action_profile_data_t *entry)
{
    SHR_FUNC_ENTER(unit);

    if (shadow == SHADOW_ENABLE) {
        /* Allocate shadow pools */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_pool_mgr_shadow_ctr_alloc(unit, false, entry));

        /* Set shadow pools registers */
        if (entry->comp == CTR_EFLEX) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_shadow_pool_set(unit, ENABLE, entry));
        } else if (entry->comp == STATE_EFLEX) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_state_eflex_shadow_pool_set(unit, ENABLE, entry));
        }
    } else if (shadow == SHADOW_DISABLE) {
        /*
         * Lookup ACTION_PROFILE_INFO LT entry to get
         * primary & shadow pool info
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_action_profile_info_lookup(unit, cur_entry));

        entry->start_shdw_pool_idx = cur_entry->start_shdw_pool_idx;
        entry->end_shdw_pool_idx = cur_entry->end_shdw_pool_idx;

        /* Free the shadow pool and enable primary pools */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_pool_mgr_shadow_ctr_free(unit, true, entry));

        /* Clear shadow pools registers */
        if (entry->comp == CTR_EFLEX) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_shadow_pool_set(unit, DISABLE, entry));
        } else if (entry->comp == STATE_EFLEX) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_state_eflex_shadow_pool_set(unit, DISABLE, entry));
        }

         /* Reset shadow pool ids to 0 since they are now disabled */
         entry->start_shdw_pool_idx =  entry->end_shdw_pool_idx = 0;
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to update entry in ACTION_PROFILE IMM LT */
static int
bcmcth_ctr_eflex_action_profile_lt_entry_update(
    int unit,
    const bcmltd_field_t *key_fields,
    const bcmltd_field_t *data_fields,
    ctr_eflex_action_profile_data_t *entry,
    ctr_eflex_action_profile_data_t *cur_entry,
    uint32_t *base_idx)
{
    bool ctrs_freed = false;
    ctr_eflex_shadow_config_t shadow = SHADOW_NOOP;
    ctr_eflex_control_t *ctrl = NULL;
    uint8_t pipe_unique;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));

    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            entry->ingress,
                                            entry->comp,
                                            &pipe_unique));


    /* Parse user values */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_lt_fields_parse(unit,
                                                        key_fields,
                                                        data_fields,
                                                        entry));

    cur_entry->action_profile_id = entry->action_profile_id;

    /* Lookup current entry with given key fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_lookup(unit, cur_entry));

    /* If action id is valid and updated, validate it on all applicable pipes */
    if (entry->action_id != cur_entry->action_id &&
        entry->action_id != CTR_EFLEX_INVALID_VAL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_action_id_validate(unit, entry));
    }

    /* Check if shadow pool is being updated. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_shadow_update_get(unit,
                                                           entry,
                                                           cur_entry,
                                                           &shadow));

    /* Fill missing fields in new entry with current entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_fill(unit, entry, cur_entry));

    /* Determine first and last pipe number for new entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_num_pipes_get(unit,
                                        entry->ingress,
                                        entry->comp,
                                        entry->pipe_idx,
                                        &entry->start_pipe_idx,
                                        &entry->end_pipe_idx));

    /* Determine first and last pipe number for current entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_num_pipes_get(unit,
                                        cur_entry->ingress,
                                        cur_entry->comp,
                                        cur_entry->pipe_idx,
                                        &cur_entry->start_pipe_idx,
                                        &cur_entry->end_pipe_idx));

    /* Check for valid update modes for given counter mode */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_ctr_eflex_update_mode_validate(unit, entry));

    /* Validate entry if slim mode is configured */
    if (entry->ctr_mode == SLIM_MODE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_slim_mode_validate(unit, entry, cur_entry));
    }

    /* Number of counter fields */
    cur_entry->num_update_mode = cur_entry->ingress ? ctrl->ing_ctr_field_num :
                                                      ctrl->egr_ctr_field_num;

    /* Adjust num_ctrs if flex counter hitbit or SLIM mode is configured */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_num_ctrs_adjust(unit, cur_entry));

    /* Free the counters in current entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pool_mgr_ctr_free(unit, cur_entry));

    ctrs_freed = true;

    /* Number of counter fields */
    entry->num_update_mode = cur_entry->num_update_mode;

    /* Adjust num_ctrs if flex counter hitbit or SLIM mode is configured */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_num_ctrs_adjust(unit, entry));

    /* Debug print ACTION_PROFILE entry info */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_action_profile_debug_print(unit, false, entry));

    /*
     * For flex state, since CCI does not know comparison operation result
     * always use SET update mode for both counters
     */
    if (entry->comp == STATE_EFLEX) {
        entry->ctr_update_mode[UPDATE_MODE_A_IDX] = SET_TO_CVALUE;
        entry->ctr_update_mode[UPDATE_MODE_B_IDX] = SET_TO_CVALUE;
    }

    if (entry->base_idx_auto) {
        /* Calculate the base index value & allocate counters */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_pool_mgr_ctr_alloc(unit, false, entry, base_idx));
    } else {
        /* Use the provided base index value to reserve counters */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_pool_mgr_ctr_reserve(unit, false, entry));
    }

    /* Get OPERAND_PROFILE profile */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_op_profile_get(unit, entry));

    /* Write to counter action tables */
    if (entry->comp == CTR_EFLEX) {
        /* Write to counter action tables */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_action_table_write(unit, entry));
    } else if (entry->comp == STATE_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_state_eflex_action_table_write(unit, entry));
    }

    /* Update shadow pools */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_shadow_update(unit,
                                                       shadow,
                                                       cur_entry,
                                                       entry));
    /* Delete current ACTION_PROFILE_INFO LT entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_info_program(unit, false, cur_entry));

    /* Insert new ACTION_PROFILE_INFO LT entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_info_program(unit, true, entry));

    /*
     * Update internal resource list and delete current entry from HW table
     * only if action id or instance or pipe index in pipe unique
     * mode has changed
     */
    if (entry->action_id != cur_entry->action_id ||
        (entry->state_ext.inst != cur_entry->state_ext.inst) ||
        (entry->state_ext.egr_inst != cur_entry->state_ext.egr_inst) ||
        (pipe_unique && entry->pipe_idx != cur_entry->pipe_idx)) {
        /* Delete current entry from HW table */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_action_profile_lt_entry_delete(unit,
                                                             true,
                                                             cur_entry));

        /* Set new action id bit in resource list */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_action_id_set(unit, ENABLE, entry));
    }

    if (entry->base_idx_auto) {
        /* Update entry with calculated base index */
        entry->base_idx = *base_idx;
    }

exit:
    /* Restore deleted current entry if updated entry cannot be inserted */
    if (SHR_FUNC_VAL_IS(SHR_E_RESOURCE) && ctrs_freed) {
        if (cur_entry->base_idx_auto) {
            /* Calculate the base index value & allocate counters */
            (void)bcmcth_ctr_eflex_pool_mgr_ctr_alloc(unit,
                                                      false,
                                                      cur_entry,
                                                      base_idx);
        } else {
            /* Use the provided base index value to reserve counters */
            (void)bcmcth_ctr_eflex_pool_mgr_ctr_reserve(unit, false, cur_entry);
        }
    }

    SHR_FUNC_EXIT();
}

/*
 * Function to check if corresponding ACTION_PROFILE LT entry,
 * should be updated based on OPERAND_PROFILE LT entry insert/delete operation.
 *
 *
 * This function iterates over each ACTION_PROFILE entry searching
 * for a match with operand profile id in input OPERAND_PROFILE entry.
 *
 * The incoming operand profile id is checked against all ACTION_PROFILE LT
 * entries and within each entry, it is matched with all possible operand
 * profile ids in the entry.
 *
 * If match is found, then the op profile index (PT index) is updated in the
 * ACTION_PROFILE LT entry.
 *
 * For INSERT notification, op profile index in the OPERAND_PROFILE LT entry is
 * used.
 * For DELETE notification, op profile index used is 0.
 */
int
bcmcth_ctr_eflex_action_profile_notify(int unit,
                                       bool insert,
                                       ctr_eflex_op_profile_data_t *op_entry)
{
    bcmltd_fields_t key_fields = {0};
    bcmltd_field_t fld;
    bcmltd_field_t *fld_ptr_arr[1];
    bcmltd_fields_t imm_fields = {0};
    size_t num_fields = 0;
    int num_objs, rv;
    uint32_t op_profile_idx;
    uint8_t pipe_unique;
    bool found, check_op_id;
    eflex_comp_t comp;
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;
    ctr_eflex_action_profile_data_t entry = {0};
    ctr_eflex_action_profile_lt_info_t *lt_info = NULL;

    SHR_FUNC_ENTER(unit);

    comp = op_entry->comp;

    op_profile_idx = insert ? op_entry->op_profile_index : 0;

    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            op_entry->ingress,
                                            op_entry->comp,
                                            &pipe_unique));

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;
    num_objs = device_info->num_objs;

    /* Get LT sid and fid info */
    lt_info = op_entry->ingress ? &ctrl->ing_action_lt_info :
                                  &ctrl->egr_action_lt_info;

    /* Configure key field */
    key_fields.count = 1;
    key_fields.field = fld_ptr_arr;
    fld_ptr_arr[0] = &fld;
    fld.id = lt_info->action_profile_id_fid;

    /* Allocate imm fields */
    SHR_IF_ERR_VERBOSE_EXIT(bcmlrd_field_count_get(unit,
                                                   lt_info->lt_sid,
                                                   &num_fields));

    /*
     * Adjust num_fields for array size
     * since LRD API counts field array size as 1
     */
    num_fields += num_objs - 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_fields_alloc(unit, &imm_fields, num_fields));

    /* Get first entry from ACTION_PROFILE IMM LT */
    rv = bcmimm_entry_get_first(unit, lt_info->lt_sid, &imm_fields);

    /* Iterate ACTION_PROFILE LT searching for input operand profile id */
    while (rv == SHR_E_NONE) {
        sal_memset(&entry, 0, sizeof(ctr_eflex_action_profile_data_t));
        found = false;
        check_op_id = true;
        entry.ingress = op_entry->ingress;
        entry.comp = op_entry->comp;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_action_profile_entry_init(unit, &entry));

        /* Get default values */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_action_profile_default_get(unit, &entry));

        /* Parse IMM entry fields */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_action_profile_entry_parse(unit,
                                                         &entry,
                                                         &imm_fields));

        /*
         * In pipe unique mode, if ACTION_PROFILE and OPERAND_PROFILE LT
         * entries are not configured for the same pipe, then do not check
         * for operand profile id match
         */
        if (pipe_unique && op_entry->pipe_idx != entry.pipe_idx) {
            check_op_id = false;
        }

        /*
         * For ingress flex state, if ACTION_PROFILE and OPERAND_PROFILE LT
         * entries are not configured for the same instance, then do not check
         * for operand profile id match
         */
        if (op_entry->comp == STATE_EFLEX && op_entry->ingress == INGRESS &&
            op_entry->inst != entry.state_ext.inst) {
            check_op_id = false;
        }
        if (op_entry->comp == STATE_EFLEX && op_entry->ingress == EGRESS &&
            op_entry->egr_inst != entry.state_ext.egr_inst) {
            check_op_id = false;
        }

        if (check_op_id) {
            /*
             * Get OPERAND_PROFILE profiles for operand profile ids which
             * are not part of this notification
             */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_action_profile_op_profile_get(unit, &entry));

            /* Check if operand profile id is found in this entry */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_action_profile_op_update(unit,
                                                           op_profile_idx,
                                                           &found,
                                                           op_entry,
                                                           &entry));

            /* Determine first and last pipe number */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_num_pipes_get(unit,
                                                entry.ingress,
                                                entry.comp,
                                                entry.pipe_idx,
                                                &entry.start_pipe_idx,
                                                &entry.end_pipe_idx));

            /* If found, update counter action table */
            if (found) {
                if (entry.comp == CTR_EFLEX) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmcth_ctr_eflex_action_table_write(unit, &entry));
                } else if (entry.comp == STATE_EFLEX) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmcth_state_eflex_action_table_write(unit, &entry));
                }
            }
        }

        /* Setup key for next entry */
        key_fields.field[0]->data = entry.action_profile_id;

        /* imm_fields.count may be modified. */
        imm_fields.count = num_fields;

        /* Get next entry from ACTION_PROFILE IMM LT */
        rv = bcmimm_entry_get_next(unit,
                                   lt_info->lt_sid,
                                   &key_fields,
                                   &imm_fields);
    } /* end WHILE */

    if (rv == SHR_E_NOT_FOUND) {
        rv = SHR_E_NONE;
    }

exit:
    /* imm_fields.count may be modified by bcmimm_entry_lookup */
    imm_fields.count = num_fields;
    bcmcth_ctr_eflex_fields_free(unit, &imm_fields);

    SHR_FUNC_EXIT();
}

/* Function to get first or next ACTION_PROFILE LT entry */
int
bcmcth_ctr_eflex_action_profile_entry_get(
    int unit,
    bool first,
    ctr_eflex_action_profile_data_t *entry)
{
    bcmltd_fields_t imm_fields = {0};
    bcmltd_field_t fld;
    bcmltd_field_t *fld_ptr_arr[1];
    bcmltd_fields_t key_fields = {0};
    size_t num_fields = 0;
    int num_objs;
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;
    ctr_eflex_action_profile_lt_info_t *lt_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;
    num_objs = device_info->num_objs;

    /* Get LT sid and fid info */
    lt_info = entry->ingress ? &ctrl->ing_action_lt_info :
                               &ctrl->egr_action_lt_info;


    /* Setup key value for get_next operation */
    if (!first) {
        /* Configure key field */
        key_fields.count = 1;
        key_fields.field = fld_ptr_arr;
        fld_ptr_arr[0] = &fld;
        fld.id = lt_info->action_profile_id_fid;
        fld.data = entry->action_profile_id;
    }

    /* Allocate imm fields */
    SHR_IF_ERR_VERBOSE_EXIT(bcmlrd_field_count_get(unit,
                                                   lt_info->lt_sid,
                                                   &num_fields));

    /*
     * Adjust num_fields for array size
     * since LRD API counts field array size as 1
     */
    num_fields += num_objs - 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_fields_alloc(unit, &imm_fields, num_fields));

    if (first) {
        /* Get first entry from ACTION_PROFILE IMM LT */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmimm_entry_get_first(unit, lt_info->lt_sid, &imm_fields));
    } else {
        /* Get next entry from ACTION_PROFILE IMM LT */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmimm_entry_get_next(unit,
                                   lt_info->lt_sid,
                                   &key_fields,
                                   &imm_fields));
    }

    /* Assign default values */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_default_get(unit, entry));

    /* Parse IMM entry fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_entry_parse(unit,
                                                     entry,
                                                     &imm_fields));

exit:
    /* imm_fields.count may be modified. */
    imm_fields.count = num_fields;
    bcmcth_ctr_eflex_fields_free(unit, &imm_fields);

    SHR_FUNC_EXIT();
}

static int
bcmcth_ctr_eflex_action_profile_lt_stage(int unit,
                                         bool ingress,
                                         eflex_comp_t comp,
                                         uint64_t req_flags,
                                         bcmimm_entry_event_t event_reason,
                                         const bcmltd_field_t *key_fields,
                                         const bcmltd_field_t *data_fields,
                                         bcmltd_fields_t *out)
{
    uint32_t base_idx = 0;
    int select;
    ctr_eflex_action_profile_data_t entry = {0};
    ctr_eflex_action_profile_data_t cur_entry = {0};
    bool unmapped;
    int rv = SHR_E_NONE;
    ctr_eflex_action_profile_lt_info_t *lt_info = NULL;
    ctr_eflex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    select = ingress ? 0 : 1;
    entry.ingress = ingress;
    entry.comp = comp;

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    /* Get LT sid and fid info */
    lt_info = ingress ? &ctrl->ing_action_lt_info :
                        &ctrl->egr_action_lt_info;

    if (out) {
        out->count = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_entry_init(unit, &entry));

    entry.req_flags = req_flags;

    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_action_profile_lt_entry_insert(unit,
                                                                 key_fields,
                                                                 data_fields,
                                                                 &entry,
                                                                 &base_idx));
            break;

        case BCMIMM_ENTRY_UPDATE:
            cur_entry.ingress = ingress;
            cur_entry.comp = comp;
            cur_entry.req_flags = req_flags;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_action_profile_entry_init(unit, &cur_entry));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_action_profile_lt_entry_update(unit,
                                                                 key_fields,
                                                                 data_fields,
                                                                 &entry,
                                                                 &cur_entry,
                                                                 &base_idx));
            break;

        case BCMIMM_ENTRY_DELETE:
            /* Parse user values */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_action_profile_lt_fields_parse(unit,
                                                                key_fields,
                                                                data_fields,
                                                                &entry));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_action_profile_lt_entry_delete(unit,
                                                                 false,
                                                                 &entry));
            break;

        default:
            break;
    }

    if (out) {
        if (entry.base_idx_auto) {
            out->field[out->count]->id = lt_info->base_idx_oper_fid;
            out->field[out->count]->data = base_idx;
            out->field[out->count++]->idx = 0;
        }

        if (comp == CTR_EFLEX && lt_info->zone_fid) {
            rv = bcmlrd_field_is_unmapped(unit,
                                          lt_info->lt_sid,
                                          lt_info->zone_fid,
                                          &unmapped);

            if (rv == SHR_E_NONE && !unmapped) {
                out->field[out->count]->id = lt_info->zone_fid;
                out->field[out->count]->data = \
                    bcmcth_ctr_eflex_action_zone_get(unit, select,
                                                     entry.action_id) ? 0 : 1;
                out->field[out->count++]->idx = 0;
            }
        }

        if (lt_info->num_ctrs_oper_fid) {
            out->field[out->count]->id = lt_info->num_ctrs_oper_fid;
            out->field[out->count]->data = entry.num_ctrs;
            out->field[out->count++]->idx = 0;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*! Function to get ACTION_PROFILE / INFO IMM LT entry limit value. */
int
bcmcth_ctr_eflex_action_profile_lt_ent_limit_get(int unit,
                                                 bool ingress,
                                                 eflex_comp_t comp,
                                                 uint32_t *count)
{
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;
    uint8_t pipe_unique;
    uint32_t num_pipes, num_action_id, num_instances = 1, num_rsvd_action_id;
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

    num_action_id = ingress ? device_info->num_ingress_action_id :
                              device_info->num_egress_action_id;
    num_rsvd_action_id = device_info->num_rsvd_action_id;
    num_pipes = pipe_unique ? device_info->num_pipes : 1;

    if (comp == STATE_EFLEX && ingress == INGRESS) {
        num_instances = device_info->state_ext.num_ingress_instance;
    } else if (comp == STATE_EFLEX && ingress == EGRESS) {
        num_instances = device_info->state_ext.num_egress_instance;
    }

    /* Calculate the entry limit */
    *count = (num_action_id - num_rsvd_action_id) * num_pipes * num_instances;

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit,"comp %d ingress %d entry_limit %d\n"),
        comp, ingress, *count));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * IMM event handler
 */

/*!
 * \brief Stage callback function of IMM event handler (bcmimm_lt_cb_t).
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] op_arg The operation arguments.
 * \param [in] event_reason This is the reason for the entry event.
 * \param [in] key_fields This is a linked list of the key fields identifying
 *                        the entry.
 * \param [in] data_fields This is a linked list of the data fields in the
 *                        modified entry.
 * \param [in] context Is a pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_ctr_eflex_ing_action_profile_lt_stage_cb(
    int unit,
    bcmltd_sid_t sid,
    const bcmltd_op_arg_t *op_arg,
    bcmimm_entry_event_t event_reason,
    const bcmltd_field_t *key_fields,
    const bcmltd_field_t *data_fields,
    void *context,
    bcmltd_fields_t *output_fields)
{
    uint32_t t;
    uint64_t req_flags;
    ctr_eflex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(key_fields, SHR_E_PARAM);
    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);
    if (event_reason != BCMIMM_ENTRY_DELETE) {
        SHR_NULL_CHECK(output_fields, SHR_E_PARAM);
    }

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, CTR_EFLEX, &ctrl));

    for (t = 0; t < ctrl->lt_info_num; t++) {
        if (ctrl->ctr_eflex_field_info[t].lt_sid ==
            ctrl->ing_action_lt_info.lt_sid) {
            break;
        }
    }

    if (output_fields) {
        if (t == ctrl->lt_info_num) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        } else if (output_fields->count <
                   ctrl->ctr_eflex_field_info[t].num_read_only_fields) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "output_fields allocated by IMM callback is "
                                  "too small to hold the desired fields\n")));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_lt_stage(unit,
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
 * \brief Entry limit get callback function of CTR_ING_EFLEX_ACTION_PROFILE
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] count Entry limit value.
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_ctr_eflex_ing_action_profile_lt_ent_limit_get_cb(int unit,
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

static int
bcmcth_ctr_eflex_egr_action_profile_lt_stage_cb(
    int unit,
    bcmltd_sid_t sid,
    const bcmltd_op_arg_t *op_arg,
    bcmimm_entry_event_t event_reason,
    const bcmltd_field_t *key_fields,
    const bcmltd_field_t *data_fields,
    void *context,
    bcmltd_fields_t *output_fields)
{
    uint32_t t;
    uint64_t req_flags;
    ctr_eflex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(key_fields, SHR_E_PARAM);
    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);
    if (event_reason != BCMIMM_ENTRY_DELETE) {
        SHR_NULL_CHECK(output_fields, SHR_E_PARAM);
    }

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, CTR_EFLEX, &ctrl));

    for (t = 0; t < ctrl->lt_info_num; t++) {
        if (ctrl->ctr_eflex_field_info[t].lt_sid ==
            ctrl->egr_action_lt_info.lt_sid) {
            break;
        }
    }

    if (output_fields) {
        if (t == ctrl->lt_info_num) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        } else if (output_fields->count <
                   ctrl->ctr_eflex_field_info[t].num_read_only_fields) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "output_fields allocated by IMM callback is "
                                  "too small to hold the desired fields.\n")));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_lt_stage(unit,
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
 * \brief Entry limit get callback function of CTR_EGR_EFLEX_ACTION_PROFILE
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] count Entry limit value.
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_ctr_eflex_egr_action_profile_lt_ent_limit_get_cb(int unit,
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
 * \brief Stage callback function of IMM event handler (bcmimm_lt_cb_t).
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] op_arg The operation arguments.
 * \param [in] event_reason This is the reason for the entry event.
 * \param [in] key_fields This is a linked list of the key fields identifying
 *                        the entry.
 * \param [in] data_fields This is a linked list of the data fields in the
 *                        modified entry.
 * \param [in] context Is a pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_state_eflex_ing_action_profile_lt_stage_cb(
    int unit,
    bcmltd_sid_t sid,
    const bcmltd_op_arg_t *op_arg,
    bcmimm_entry_event_t event_reason,
    const bcmltd_field_t *key_fields,
    const bcmltd_field_t *data_fields,
    void *context,
    bcmltd_fields_t *output_fields)
{
    uint32_t t;
    uint64_t req_flags;
    ctr_eflex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(key_fields, SHR_E_PARAM);
    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);
    if (event_reason != BCMIMM_ENTRY_DELETE) {
        SHR_NULL_CHECK(output_fields, SHR_E_PARAM);
    }

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, STATE_EFLEX, &ctrl));

    for (t = 0; t < ctrl->lt_info_num; t++) {
        if (ctrl->ctr_eflex_field_info[t].lt_sid ==
            ctrl->ing_action_lt_info.lt_sid) {
            break;
        }
    }

    if (output_fields) {
        if (t == ctrl->lt_info_num) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        } else if (output_fields->count <
                   ctrl->ctr_eflex_field_info[t].num_read_only_fields) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "output_fields allocated by IMM callback is "
                                  "too small to hold the desired fields\n")));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_lt_stage(unit,
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
bcmcth_state_eflex_ing_action_profile_lt_ent_limit_get_cb(int unit,
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

static int
bcmcth_state_eflex_egr_action_profile_lt_stage_cb(
    int unit,
    bcmltd_sid_t sid,
    const bcmltd_op_arg_t *op_arg,
    bcmimm_entry_event_t event_reason,
    const bcmltd_field_t *key_fields,
    const bcmltd_field_t *data_fields,
    void *context,
    bcmltd_fields_t *output_fields)
{
    uint32_t t;
    uint64_t req_flags;
    ctr_eflex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(key_fields, SHR_E_PARAM);
    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);
    if (event_reason != BCMIMM_ENTRY_DELETE) {
        SHR_NULL_CHECK(output_fields, SHR_E_PARAM);
    }

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, STATE_EFLEX, &ctrl));

    for (t = 0; t < ctrl->lt_info_num; t++) {
        if (ctrl->ctr_eflex_field_info[t].lt_sid ==
            ctrl->egr_action_lt_info.lt_sid) {
            break;
        }
    }

    if (output_fields) {
        if (t == ctrl->lt_info_num) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        } else if (output_fields->count <
                   ctrl->ctr_eflex_field_info[t].num_read_only_fields) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "output_fields allocated by IMM callback is "
                                  "too small to hold the desired fields.\n")));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }


    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_lt_stage(unit,
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
bcmcth_state_eflex_egr_action_profile_lt_ent_limit_get_cb(int unit,
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

static int
bcmcth_ctr_eflex_imm_action_profile_register(int unit)
{
    bcmimm_lt_cb_t ctr_eflex_lt_cb;
    ctr_eflex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_eflex_ctrl_get(unit, CTR_EFLEX, &ctrl));

    /* Register callback handlers for the in-memory logical tables */

    /* Register CTR_ING_EFLEX_ACTION_PROFILE handler */
    sal_memset(&ctr_eflex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
    ctr_eflex_lt_cb.op_stage = bcmcth_ctr_eflex_ing_action_profile_lt_stage_cb;
    ctr_eflex_lt_cb.ent_limit_get  =
        bcmcth_ctr_eflex_ing_action_profile_lt_ent_limit_get_cb;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_lt_event_reg(unit,
                             ctrl->ing_action_lt_info.lt_sid,
                             &ctr_eflex_lt_cb,
                             NULL));

    /* Register CTR_EGR_EFLEX_ACTION_PROFILE handler */
    sal_memset(&ctr_eflex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
    ctr_eflex_lt_cb.op_stage = bcmcth_ctr_eflex_egr_action_profile_lt_stage_cb;
    ctr_eflex_lt_cb.ent_limit_get  =
        bcmcth_ctr_eflex_egr_action_profile_lt_ent_limit_get_cb;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_lt_event_reg(unit,
                             ctrl->egr_action_lt_info.lt_sid,
                             &ctr_eflex_lt_cb,
                             NULL));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_state_eflex_imm_action_profile_register(int unit)
{
    bcmimm_lt_cb_t state_eflex_lt_cb;
    ctr_eflex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_eflex_ctrl_get(unit, STATE_EFLEX, &ctrl));

    /* Register callback handlers for the in-memory logical tables */

    /* Register FLEX_STATE_ING_ACTION_PROFILE handler */
    sal_memset(&state_eflex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
    state_eflex_lt_cb.op_stage =
        bcmcth_state_eflex_ing_action_profile_lt_stage_cb;
    state_eflex_lt_cb.ent_limit_get  =
        bcmcth_state_eflex_ing_action_profile_lt_ent_limit_get_cb;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_lt_event_reg(unit,
                             ctrl->ing_action_lt_info.lt_sid,
                             &state_eflex_lt_cb,
                             NULL));

    /* Register FLEX_STATE_EGR_ACTION_PROFILE handler */
    sal_memset(&state_eflex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
    state_eflex_lt_cb.op_stage =
        bcmcth_state_eflex_egr_action_profile_lt_stage_cb;
    state_eflex_lt_cb.ent_limit_get  =
        bcmcth_state_eflex_egr_action_profile_lt_ent_limit_get_cb;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_lt_event_reg(unit,
                             ctrl->egr_action_lt_info.lt_sid,
                             &state_eflex_lt_cb,
                             NULL));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */

int
bcmcth_ctr_eflex_imm_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Register CTR_xxx_EFLEX_ACTION_PROFILE handler */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_ctr_eflex_imm_action_profile_register(unit));

    /* Register CTR_xxx_EFLEX_ACTION_PROFILE_INFO handler */
    /* Register CTR_xxx_FLEX_POOL_INFO handler */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_imm_action_profile_info_register(unit));

    /* Register CTR_xxx_EFLEX_OPERAND_PROFILE handler */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_ctr_eflex_imm_op_profile_register(unit));

    /* Register CTR_xxx_EFLEX_RANGE_CHK_PROFILE handler */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_ctr_eflex_imm_range_chk_register(unit));

    /* Register CTR_xxx_EFLEX_TRIGGER handler */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_ctr_eflex_imm_trigger_register(unit));

    /* Register CTR_xxx_EFLEX_GROUP_ACTION_PROFILE handler */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_ctr_eflex_imm_group_action_register(unit));

    /* Register CTR_xxx_EFLEX_ERROR_STATS handler */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_ctr_eflex_imm_error_stats_register(unit));

    /* Register CTR_xxx_EFLEX_HITBIT_CONTROL handler */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_ctr_eflex_imm_hitbit_ctrl_register(unit));

    /* Register CTR_xxx_EFLEX_OBJ_QUANTIZATION handler */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_ctr_eflex_imm_obj_quant_register(unit));

   /* Register CTR_xxx_EFLEX_OBJ_INFO handler */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_ctr_eflex_imm_obj_info_register(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_state_eflex_imm_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Register FLEX_STATE_xxx_ACTION_PROFILE handler */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_state_eflex_imm_action_profile_register(unit));

    /* Register FLEX_STATE_xxx_ACTION_PROFILE_INFO handler */
    /* Register FLEX_STATE_xxx_POOL_INFO handler */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_state_eflex_imm_action_profile_info_register(unit));

    /* Register FLEX_STATE_xxx_OPERAND_PROFILE handler */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_state_eflex_imm_op_profile_register(unit));

    /* Register FLEX_STATE_xxx_RANGE_CHK_PROFILE handler */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_state_eflex_imm_range_chk_register(unit));

    /* Register FLEX_STATE_xxx_TRIGGER handler */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_state_eflex_imm_trigger_register(unit));

    /* Register FLEX_STATE_xxx_GROUP_ACTION_PROFILE handler */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_state_eflex_imm_group_action_register(unit));

    /* Register FLEX_STATE_xxx_ERROR_STATS handler */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_state_eflex_imm_error_stats_register(unit));

    /* Register FLEX_STATE_xxx_OBJ_QUANTIZATION handler */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_state_eflex_imm_obj_quant_register(unit));

    /* Register FLEX_STATE_xxx_OBJ_INFO handler */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_state_eflex_imm_obj_info_register(unit));

    /* Register FLEX_STATE_xxx_OBJ_QUANTIZATION handler */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_state_eflex_imm_truth_table_register(unit));

exit:
    SHR_FUNC_EXIT();
}
