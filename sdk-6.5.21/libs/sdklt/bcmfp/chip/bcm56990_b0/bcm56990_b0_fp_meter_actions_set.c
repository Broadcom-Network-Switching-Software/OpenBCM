/*! \file bcm56990_b0_fp_meter_actions_set.c
 *
 * Chip Specific Stage meter actions set function
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
bcm56990_b0_fp_meter_actions_set(int unit,
                                 bcmfp_stage_t *stage,
                                 bcmfp_group_id_t group_id,
                                 bcmfp_pdd_id_t pdd_id,
                                 bcmfp_group_oper_info_t *opinfo,
                                 bcmcth_meter_fp_policy_fields_t *meter_entry,
                                 uint32_t **edw)
{
    bcmfp_stage_id_t stage_id;

    SHR_FUNC_ENTER(unit);

    stage_id = stage->stage_id;

    switch (stage_id) {
        case BCMFP_STAGE_ID_INGRESS:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_bcm56990_b0_ingress_meter_actions_set(unit,
                                                             group_id,
                                                             pdd_id,
                                                             opinfo,
                                                             meter_entry,
                                                             edw));
            break;
        case BCMFP_STAGE_ID_EXACT_MATCH:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_bcm56990_b0_em_meter_actions_set(unit,
                                                        group_id,
                                                        pdd_id,
                                                        opinfo,
                                                        meter_entry,
                                                        edw));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();

}
