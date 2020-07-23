/*! \file bcm56780_a0_fp_presel_edw_get.c
 *
 * Chip Specific Stage presel edw get function
 * for Trident4-X9(56780_A0).
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
#include <bcmfp/chip/bcm56780_a0_fp.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_INIT

int
bcm56780_a0_fp_presel_edw_get(int unit,
                              bcmfp_stage_id_t stage_id,
                              bcmfp_group_id_t group_id,
                              bcmfp_group_oper_info_t *opinfo,
                              uint32_t **edw)
{
    SHR_FUNC_ENTER(unit);

    switch (stage_id) {
        case BCMFP_STAGE_ID_INGRESS:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56780_a0_fp_ing_presel_edw_get(unit,
                                                   stage_id,
                                                   group_id,
                                                   opinfo,
                                                   edw));

            break;
        case BCMFP_STAGE_ID_LOOKUP:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56780_a0_fp_vlan_presel_edw_get(unit,
                                                    stage_id,
                                                    group_id,
                                                    opinfo,
                                                    edw));

            break;
        case BCMFP_STAGE_ID_EGRESS:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56780_a0_fp_egr_presel_edw_get(unit,
                                                   stage_id,
                                                   group_id,
                                                   opinfo,
                                                   edw));

            break;
        case BCMFP_STAGE_ID_EXACT_MATCH:
        case BCMFP_STAGE_ID_FLOW_TRACKER:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56780_a0_fp_em_presel_edw_get(unit,
                                                  stage_id,
                                                  group_id,
                                                  opinfo,
                                                  edw));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();

}
