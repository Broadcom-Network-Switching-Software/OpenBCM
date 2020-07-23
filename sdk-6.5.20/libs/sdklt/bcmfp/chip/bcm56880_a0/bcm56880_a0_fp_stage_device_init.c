/*! \file bcm56880_a0_fp_stage_device_init.c
 *
 * Chip Specific Stage Initialization function
 * for Trident4(56880_A0).
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
#include <bcmfp/chip/bcm56880_a0_fp.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_INIT

int
bcm56880_a0_fp_stage_device_init(int unit, bcmfp_stage_t *stage, bool warm)
{
    bcmfp_stage_id_t stage_id;

    SHR_FUNC_ENTER(unit);

    stage_id = stage->stage_id;

    switch (stage_id) {
        case BCMFP_STAGE_ID_INGRESS:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56880_a0_fp_ing_device_consts_init(unit, stage));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56880_a0_fp_common_qual_db_init(unit, stage));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56880_a0_fp_common_action_db_init(unit, stage));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56880_a0_fp_common_ext_db_init(unit, stage));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56880_a0_fp_ing_alpm_fids_init(unit, stage));
            break;
        case BCMFP_STAGE_ID_LOOKUP:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56880_a0_fp_vlan_device_consts_init(unit, stage));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56880_a0_fp_common_qual_db_init(unit, stage));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56880_a0_fp_common_action_db_init(unit, stage));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56880_a0_fp_common_ext_db_init(unit, stage));
            break;
        case BCMFP_STAGE_ID_EGRESS:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56880_a0_fp_egr_device_consts_init(unit, stage));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56880_a0_fp_common_qual_db_init(unit, stage));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56880_a0_fp_common_action_db_init(unit, stage));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56880_a0_fp_common_ext_db_init(unit, stage));
            break;
        case BCMFP_STAGE_ID_EXACT_MATCH:
        case BCMFP_STAGE_ID_FLOW_TRACKER:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56880_a0_fp_em_device_consts_init(unit, stage));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56880_a0_fp_em_ext_db_init(unit, stage));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56880_a0_fp_em_qual_db_init(unit, stage));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56880_a0_fp_common_action_db_init(unit, stage));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}
