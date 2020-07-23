/*! \file bcm56880_a0_fp_profiles_copy.c
 *
 * Chip Specific Stage profiles copy function
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
bcm56880_a0_fp_profiles_copy(int unit, uint32_t trans_id, bcmfp_stage_t *stage)
{
    bcmfp_stage_id_t stage_id;

    SHR_FUNC_ENTER(unit);

    stage_id = stage->stage_id;

    switch (stage_id) {
        case BCMFP_STAGE_ID_EXACT_MATCH:
        case BCMFP_STAGE_ID_FLOW_TRACKER:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56880_a0_fp_em_profiles_copy(unit, trans_id, stage));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();

}
