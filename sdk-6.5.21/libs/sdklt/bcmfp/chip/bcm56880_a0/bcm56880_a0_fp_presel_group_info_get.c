/*! \file bcm56880_a0_fp_presel_group_info_get.c
 *
 * Chip Specific Stage presel group info get function
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
bcm56880_a0_fp_presel_group_info_get(int unit,
                                     bcmfp_stage_id_t stage_id,
                                     bcmfp_group_id_t group_id,
                                     uint32_t **presel_group_info)
{
    SHR_FUNC_ENTER(unit);

    if (stage_id == BCMFP_STAGE_ID_LOOKUP ||
        stage_id == BCMFP_STAGE_ID_EGRESS ||
        stage_id == BCMFP_STAGE_ID_INGRESS ||
        stage_id == BCMFP_STAGE_ID_EXACT_MATCH ||
        stage_id == BCMFP_STAGE_ID_FLOW_TRACKER) {
        SHR_IF_ERR_VERBOSE_EXIT
                (bcm56880_a0_fp_common_presel_group_info_get(unit,
                                                             stage_id,
                                                             group_id,
                                                             presel_group_info));
    }
exit:
    SHR_FUNC_EXIT();

}
