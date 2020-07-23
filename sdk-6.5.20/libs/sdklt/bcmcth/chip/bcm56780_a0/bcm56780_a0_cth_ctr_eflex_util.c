/*! \file bcm56780_a0_cth_ctr_eflex_util.c
 *
 * Chip stub for BCMCTH CTR EFLEX utility functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmcth/bcmcth_ctr_eflex_internal.h>
#include <bcmcth/chip/bcm56780_a0/bcm56780_a0_ctr_eflex.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_CTREFLEX

/*
 * Function to use ACTION_PROFILE LT entry given as input
 * and save pool id, base index, pipe index as part of hitbit
 * configuration.
 */
static int
bcm56780_a0_cth_ctr_eflex_hitbit_ctrl_add_helper(
    int unit,
    uint32_t lkup_idx,
    ctr_eflex_control_t *ctrl,
    ctr_eflex_hitbit_lt_info_t *hitbit_lt_info,
    ctr_eflex_action_profile_data_t *action_entry)
{
    uint32_t num_hitbit_ctr, num_ctr_per_section, num_slim_ctrs;

    SHR_FUNC_ENTER(unit);

    /* Number of hitbits per counter */
    num_hitbit_ctr = ctrl->ctr_eflex_device_info->num_hitbit_ctr;

    /* Number of slim counters */
    num_slim_ctrs = ctrl->ctr_eflex_device_info->num_slim_ctrs;

    /* Number of counters per section */
    num_ctr_per_section = ctrl->ctr_eflex_device_info->num_ctr_section;

    hitbit_lt_info->enable[lkup_idx] = true;
    hitbit_lt_info->pool_idx[lkup_idx] = action_entry->pool_idx;
    hitbit_lt_info->base_idx[lkup_idx] = action_entry->base_idx;
    hitbit_lt_info->is_hitbit[lkup_idx] =
            bcmcth_ctr_eflex_is_hitbit(unit, action_entry);
    hitbit_lt_info->is_slim[lkup_idx] =
            (action_entry->ctr_mode == SLIM_MODE) ? true : false;

    /*
     * For hitbit, base index is multiplied by factor of 64.
     * For slim counters, base index is multiplied by factor of 3.
     */
    if (hitbit_lt_info->is_hitbit[lkup_idx]) {
        hitbit_lt_info->factor[lkup_idx] = num_hitbit_ctr * num_ctr_per_section;
    } else if (hitbit_lt_info->is_slim[lkup_idx]) {
        hitbit_lt_info->factor[lkup_idx] = num_slim_ctrs * num_ctr_per_section;
    } else {
        hitbit_lt_info->factor[lkup_idx] = 1;
    }

    /* Get absolute base index */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_abs_idx_get(unit,
                                      0,
                                      action_entry,
                                      &hitbit_lt_info->abs_base_idx[lkup_idx]));

    /*
     * Assuming only 2 counter pools are needed for hitbit,
     * get counter pool sid of base pool idx and next pool
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_pool_sid_get(
            unit,
            action_entry->ingress,
            action_entry->comp,
            0,
            0,
            action_entry->pool_idx,
            &hitbit_lt_info->pool_sid[lkup_idx][0]));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_pool_sid_get(
            unit,
            action_entry->ingress,
            action_entry->comp,
            0,
            0,
            (action_entry->pool_idx + 1),
            &hitbit_lt_info->pool_sid[lkup_idx][1]));

exit:
    SHR_FUNC_EXIT();
}

int
bcm56780_a0_cth_ctr_eflex_hitbit_ctrl_add_internal(
    int unit,
    ctr_eflex_hitbit_ctrl_data_t *entry,
    ctr_eflex_control_t *ctrl,
    ctr_eflex_action_profile_data_t *dst_action_entry,
    ctr_eflex_action_profile_data_t *src_action_entry,
    ctr_eflex_hitbit_lt_info_t **hitbit_info)
{
    int idx;
    ctr_eflex_hitbit_lt_info_t *hitbit_lt_info = NULL;

    SHR_FUNC_ENTER(unit);

    hitbit_lt_info = entry->ingress ? ctrl->ing_hitbit_lt_info :
                                      ctrl->egr_hitbit_lt_info;

    /* Find first empty entry */
    for (idx = 0; idx < HITBIT_LT_NUM; idx++) {
        if (hitbit_lt_info[idx].lt_id ==
                ctrl->ctr_eflex_enum.invalid_lt_id) {
            break;
        }
    }

    if (idx == HITBIT_LT_NUM) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Hitbit LT info structure is full.\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Set hitbit lt info */
    hitbit_lt_info[idx].ingress = entry->ingress;
    hitbit_lt_info[idx].lt_id = entry->lt_id;
    hitbit_lt_info[idx].pipe_idx = entry->pipe_unique ? entry->pipe_idx :
                                                        ALL_PIPES;

    /* Save destination hitbit info */
    if (entry->dst_action_profile_id != CTR_EFLEX_INVALID_ACTION_PROFILE_ID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_cth_ctr_eflex_hitbit_ctrl_add_helper(
                unit,
                DST_LKUP_IDX,
                ctrl,
                &hitbit_lt_info[idx],
                dst_action_entry));
    }

    /* Save source hitbit info */
    if (entry->src_action_profile_id != CTR_EFLEX_INVALID_ACTION_PROFILE_ID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_cth_ctr_eflex_hitbit_ctrl_add_helper(
                unit,
                SRC_LKUP_IDX,
                ctrl,
                &hitbit_lt_info[idx],
                src_action_entry));
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "Add %s Entry:\n"
            "lt_id %d, idx %d\n"
            "DST: enable %d, pool_idx %d, pipe_idx %d\n"
            "     is_hitbit %d, is_slim %d\n"
            "     base_idx %d\n"
            "     pool_sid Base %d, Next %d\n"
            "SRC: enable %d, pool_idx %d, pipe_idx %d\n"
            "     is_hitbit %d, is_slim %d\n"
            "     base_idx %d\n"
            "     pool_sid Base %d, Next %d\n"),
            entry->ingress ? "Ingress" : "Egress",
            entry->lt_id, idx,
            hitbit_lt_info[idx].enable[DST_LKUP_IDX],
            hitbit_lt_info[idx].pool_idx[DST_LKUP_IDX],
            hitbit_lt_info[idx].pipe_idx,
            hitbit_lt_info[idx].is_hitbit[DST_LKUP_IDX],
            hitbit_lt_info[idx].is_slim[DST_LKUP_IDX],
            hitbit_lt_info[idx].base_idx[DST_LKUP_IDX],
            hitbit_lt_info[idx].pool_sid[DST_LKUP_IDX][0],
            hitbit_lt_info[idx].pool_sid[DST_LKUP_IDX][1],
            hitbit_lt_info[idx].enable[SRC_LKUP_IDX],
            hitbit_lt_info[idx].pool_idx[SRC_LKUP_IDX],
            hitbit_lt_info[idx].pipe_idx,
            hitbit_lt_info[idx].is_hitbit[SRC_LKUP_IDX],
            hitbit_lt_info[idx].is_slim[SRC_LKUP_IDX],
            hitbit_lt_info[idx].base_idx[SRC_LKUP_IDX],
            hitbit_lt_info[idx].pool_sid[SRC_LKUP_IDX][0],
            hitbit_lt_info[idx].pool_sid[SRC_LKUP_IDX][1]));

    /* Return hitbit lt info */
    *hitbit_info = &hitbit_lt_info[idx];

exit:
    SHR_FUNC_EXIT();
}
