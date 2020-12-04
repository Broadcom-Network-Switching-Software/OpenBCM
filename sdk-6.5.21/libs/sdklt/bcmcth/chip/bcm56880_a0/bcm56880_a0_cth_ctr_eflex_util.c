/*! \file bcm56880_a0_cth_ctr_eflex_util.c
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
#include <bcmcth/chip/bcm56880_a0/bcm56880_a0_ctr_eflex.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_CTREFLEX

/*
 * Function to lookup ACTION_PROFILE LT entry given as input
 * and save pool id, base index, pipe index as part of hitbit
 * configuration.
 *
 * For hit bit only, UFT and non-UFT banks have overlapped table ids
 * and different sizes in terms of hitbits supported (64K vs 16K)
 * Both are configured in 2 pools such that UFT banks are allocated
 * complete 1st pool while non-UFT banks start within the 1st pool
 * but the non-overlapped table ids are placed at beginning of 2nd
 * pool by adjusting its base index in the respective ACTION_PROFILE
 * LT entry.
 * Hence,the base index for UFT banks is configured as 0 while it is
 * non-zero for non-UFT banks.
 */
static int
bcm56880_a0_cth_ctr_eflex_hitbit_ctrl_add_helper(
    int unit,
    uint32_t lkup_idx,
    ctr_eflex_control_t *ctrl,
    ctr_eflex_hitbit_lt_info_t *hitbit_lt_info,
    ctr_eflex_action_profile_data_t *action_entry)
{
    uint32_t num_hitbit_ctr, num_ctr_per_section, num_slim_ctrs;
    uint32_t uft_idx;

    SHR_FUNC_ENTER(unit);

    /* Number of hitbits per counter */
    num_hitbit_ctr = ctrl->ctr_eflex_device_info->num_hitbit_ctr;

    /* Number of slim counters */
    num_slim_ctrs = ctrl->ctr_eflex_device_info->num_slim_ctrs;

    /* Number of counters per section */
    num_ctr_per_section = ctrl->ctr_eflex_device_info->num_ctr_section;

    /* Lookup ACTION_PROFILE LT with action profile id key */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_lookup(unit,
                                                action_entry));

    /*
     * If base index is 0,
     * then save ACTION_PROFILE LT entry info as UFT index,
     * else save ACTION_PROFILE LT entry info as non-UFT index
     */
    uft_idx = (action_entry->base_idx == 0) ? UFT_IDX : NON_UFT_IDX;

    hitbit_lt_info->enable[lkup_idx] = true;
    hitbit_lt_info->pool_idx[lkup_idx] = action_entry->pool_idx;
    hitbit_lt_info->base_idx[uft_idx] = action_entry->base_idx;
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

/*
 * Function to parse GROUP_ACTION_PROFILE LT entry and save hitbit
 * config for each valid action profile id.
 *
 * For hit bit only, 2 ACTION_PROFILE LT entries need to be configured
 * for UFT and non-UFT banks for which PTRM Hash will report hitbit entry
 * moves. These 2 entries are then configured as part of GROUP_ACTION_PROFILE
 * LT entry. One is configured as the counter action profile id representing
 * group action and other is configured as part of the group_map.
 */
static int
bcm56880_a0_cth_ctr_eflex_hitbit_ctrl_add_wrapper(
    int unit,
    uint32_t lkup_idx,
    ctr_eflex_control_t *ctrl,
    ctr_eflex_hitbit_lt_info_t *hitbit_lt_info,
    ctr_eflex_group_action_data_t *entry)
{
    uint32_t num_actions, i;
    ctr_eflex_action_profile_data_t action_entry;

    SHR_FUNC_ENTER(unit);

    num_actions = entry->ingress ?
                  ctrl->ctr_eflex_device_info->num_ing_actions :
                  ctrl->ctr_eflex_device_info->num_egr_actions;

    /* Group action profile id is a valid action id */
    sal_memset(&action_entry, 0,
               sizeof(ctr_eflex_action_profile_data_t));
    action_entry.ingress = entry->ingress;
    action_entry.comp = entry->comp;
    action_entry.action_profile_id = entry->action_profile_id;

    /* Add hitbit control info */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56880_a0_cth_ctr_eflex_hitbit_ctrl_add_helper(unit,
                                                          lkup_idx,
                                                          ctrl,
                                                          hitbit_lt_info,
                                                          &action_entry));

    /* Parse group_map in group action entry to get first valid action id */
    for (i = 0; i < num_actions; i++) {
        if (entry->action_profile_id_map[i] !=
                CTR_EFLEX_INVALID_ACTION_PROFILE_ID) {
            sal_memset(&action_entry, 0,
                       sizeof(ctr_eflex_action_profile_data_t));
            action_entry.ingress = entry->ingress;
            action_entry.comp = entry->comp;
            action_entry.action_profile_id = entry->action_profile_id_map[i];

            /* Add hitbit control info */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56880_a0_cth_ctr_eflex_hitbit_ctrl_add_helper(
                    unit,
                    lkup_idx,
                    ctrl,
                    hitbit_lt_info,
                    &action_entry));
            break;
        }
    } /* end FOR */

exit:
    SHR_FUNC_EXIT();
}

int
bcm56880_a0_cth_ctr_eflex_hitbit_ctrl_add_internal(
    int unit,
    ctr_eflex_hitbit_ctrl_data_t *entry,
    ctr_eflex_control_t *ctrl,
    ctr_eflex_group_action_data_t *dst_grp_action_entry,
    ctr_eflex_group_action_data_t *src_grp_action_entry,
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
    if (entry->dst_grp_action_profile_id !=
                                    CTR_EFLEX_INVALID_ACTION_PROFILE_ID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56880_a0_cth_ctr_eflex_hitbit_ctrl_add_wrapper(
                unit,
                DST_LKUP_IDX,
                ctrl,
                &hitbit_lt_info[idx],
                dst_grp_action_entry));
    }

    /* Save source hitbit info */
    if (entry->src_grp_action_profile_id !=
                                    CTR_EFLEX_INVALID_ACTION_PROFILE_ID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56880_a0_cth_ctr_eflex_hitbit_ctrl_add_wrapper(
                unit,
                SRC_LKUP_IDX,
                ctrl,
                &hitbit_lt_info[idx],
                src_grp_action_entry));
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "Add %s Entry:\n"
            "lt_id %d, idx %d\n"
            "DST: enable %d, pool_idx %d, pipe_idx %d\n"
            "     is_hitbit %d, is_slim %d\n"
            "     base_idx UFT %d, NON UFT %d\n"
            "     pool_sid Base %d, Next %d\n"
            "SRC: enable %d, pool_idx %d, pipe_idx %d\n"
            "     is_hitbit %d, is_slim %d\n"
            "     base_idx UFT %d, NON UFT %d\n"
            "     pool_sid Base %d, Next %d\n"),
            entry->ingress ? "Ingress" : "Egress",
            entry->lt_id, idx,
            hitbit_lt_info[idx].enable[DST_LKUP_IDX],
            hitbit_lt_info[idx].pool_idx[DST_LKUP_IDX],
            hitbit_lt_info[idx].pipe_idx,
            hitbit_lt_info[idx].is_hitbit[DST_LKUP_IDX],
            hitbit_lt_info[idx].is_slim[DST_LKUP_IDX],
            hitbit_lt_info[idx].base_idx[UFT_IDX],
            hitbit_lt_info[idx].base_idx[NON_UFT_IDX],
            hitbit_lt_info[idx].pool_sid[DST_LKUP_IDX][0],
            hitbit_lt_info[idx].pool_sid[DST_LKUP_IDX][1],
            hitbit_lt_info[idx].enable[SRC_LKUP_IDX],
            hitbit_lt_info[idx].pool_idx[SRC_LKUP_IDX],
            hitbit_lt_info[idx].pipe_idx,
            hitbit_lt_info[idx].is_hitbit[SRC_LKUP_IDX],
            hitbit_lt_info[idx].is_slim[SRC_LKUP_IDX],
            hitbit_lt_info[idx].base_idx[UFT_IDX],
            hitbit_lt_info[idx].base_idx[NON_UFT_IDX],
            hitbit_lt_info[idx].pool_sid[SRC_LKUP_IDX][0],
            hitbit_lt_info[idx].pool_sid[SRC_LKUP_IDX][1]));

    /* Return hitbit lt info */
    *hitbit_info = &hitbit_lt_info[idx];

exit:
    SHR_FUNC_EXIT();
}
