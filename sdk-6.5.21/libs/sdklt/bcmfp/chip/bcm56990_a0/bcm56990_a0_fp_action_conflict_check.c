/*! \file bcm56990_a0_fp_action_conflict_check.c
 *
 * Chip Specific Stage Action Conflict Check function
 * for (56990_A0).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_chip_internal.h>
#include <bcmfp/chip/bcm56990_a0_fp.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_INIT

int
bcm56990_a0_fp_action_conflict_check(int unit,
                                     bcmfp_stage_t *stage,
                                     bcmfp_action_t action1,
                                     bcmfp_action_t action2)
{

    bcmfp_stage_id_t stage_id;

    SHR_FUNC_ENTER(unit);

    stage_id = stage->stage_id;

    switch (stage_id) {
        case BCMFP_STAGE_ID_INGRESS:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_bcm56990_a0_ingress_action_cfg_conflict_check(unit,
                                                                     action1,
                                                                     action2));
            break;
        case BCMFP_STAGE_ID_LOOKUP:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_bcm56990_a0_vlan_action_cfg_conflict_check(unit,
                                                                  action1,
                                                                  action2));
            break;
        case BCMFP_STAGE_ID_EGRESS:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_bcm56990_a0_egress_action_cfg_conflict_check(unit,
                                                                    action1,
                                                                    action2));
            break;
        case BCMFP_STAGE_ID_EXACT_MATCH:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_bcm56990_a0_em_action_cfg_conflict_check(unit,
                                                                action1,
                                                                action2));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}
