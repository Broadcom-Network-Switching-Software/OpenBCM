/*! \file bcm56880_a0_fp_counter_actions_set.c
 *
 * Chip Specific Stage counter actions set function
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
bcm56880_a0_fp_counter_actions_set(int unit,
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

    if (stage_id == BCMFP_STAGE_ID_LOOKUP ||
        stage_id == BCMFP_STAGE_ID_EGRESS ||
        stage_id == BCMFP_STAGE_ID_INGRESS ||
        stage_id == BCMFP_STAGE_ID_EXACT_MATCH ||
        stage_id == BCMFP_STAGE_ID_FLOW_TRACKER) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56880_a0_fp_common_counter_actions_set(unit,
                                                       stage->stage_id,
                                                       group_id,
                                                       pdd_id,
                                                       opinfo,
                                                       ctr_params,
                                                       edw));
    }

exit:
    SHR_FUNC_EXIT();

}
