/*! \file bcm56996_a0_fp_counter_actions_set.c
 *
 * Chip Specific Stage counter actions set function
 * for (56996_A0).
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
#include <bcmfp/chip/bcm56996_a0_fp.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_INIT

int
bcm56996_a0_fp_counter_actions_set(int unit,
                                   bcmfp_stage_t *stage,
                                   bcmfp_group_id_t group_id,
                                   bcmfp_pdd_id_t pdd_id,
                                   bcmfp_group_oper_info_t *opinfo,
                                   bcmfp_ctr_params_t *ctr_params,
                                   uint32_t **edw)
{
    bcmfp_stage_id_t stage_id;

    SHR_FUNC_ENTER(unit);

    stage_id = stage->stage_id;

    switch (stage_id) {
        case BCMFP_STAGE_ID_INGRESS:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_bcm56996_a0_ingress_counter_actions_set(unit,
                                                               stage->stage_id,
                                                               group_id,
                                                               pdd_id,
                                                               opinfo,
                                                               ctr_params,
                                                               edw));
            break;
        case BCMFP_STAGE_ID_LOOKUP:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_bcm56996_a0_vlan_counter_actions_set(unit,
                                                            stage->stage_id,
                                                            group_id,
                                                            pdd_id,
                                                            opinfo,
                                                            ctr_params,
                                                            edw));
            break;
        case BCMFP_STAGE_ID_EGRESS:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_bcm56996_a0_egress_counter_actions_set(unit,
                                                              stage->stage_id,
                                                              group_id,
                                                              pdd_id,
                                                              opinfo,
                                                              ctr_params,
                                                              edw));
            break;
        case BCMFP_STAGE_ID_EXACT_MATCH:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_bcm56996_a0_em_counter_actions_set(unit,
                                                          stage->stage_id,
                                                          group_id,
                                                          pdd_id,
                                                          opinfo,
                                                          ctr_params,
                                                          edw));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();

}
