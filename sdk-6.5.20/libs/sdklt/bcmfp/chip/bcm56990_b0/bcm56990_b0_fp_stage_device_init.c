/*! \file bcm56990_b0_fp_stage_device_init.c
 *
 * Chip Specific Stage Initialization function
 * for (56990_B0).
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
#include <bcmfp/chip/bcm56990_b0_fp.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_INIT

int
bcm56990_b0_fp_stage_device_init(int unit,
                                 bcmfp_stage_t *stage,
                                 bool warm)
{
    bcmfp_stage_id_t stage_id;

    SHR_FUNC_ENTER(unit);

    stage_id = stage->stage_id;

    switch (stage_id) {
        case BCMFP_STAGE_ID_INGRESS:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_bcm56990_b0_ingress_device_consts_init(unit,
                                                              stage,
                                                              warm));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_bcm56990_b0_ingress_ext_cfg_db_init(unit, stage));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_bcm56990_b0_ingress_qual_cfg_db_init(unit, stage));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_bcm56990_b0_ingress_action_cfg_db_init(unit, stage));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_b0_fp_ing_alpm_fids_init(unit, stage));
            break;
        case BCMFP_STAGE_ID_LOOKUP:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_bcm56990_b0_vlan_device_consts_init(unit, stage));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_bcm56990_b0_vlan_qual_cfg_db_init(unit, stage));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_bcm56990_b0_vlan_action_cfg_db_init(unit, stage));
            break;
        case BCMFP_STAGE_ID_EGRESS:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_bcm56990_b0_egress_device_consts_init(unit, stage));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_bcm56990_b0_egress_qual_cfg_db_init(unit, stage));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_bcm56990_b0_egress_action_cfg_db_init(unit, stage));
            break;
        case BCMFP_STAGE_ID_EXACT_MATCH:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_bcm56990_b0_exactmatch_device_consts_init(unit, stage));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_bcm56990_b0_em_ext_cfg_db_init(unit, stage));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_bcm56990_b0_em_qual_cfg_db_init(unit, stage));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_bcm56990_b0_em_action_cfg_db_init(unit, stage));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();

}
