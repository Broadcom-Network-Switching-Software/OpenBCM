/*! \file bcm56780_a0_fp_bank_sids_get.c
 *
 * Chip Specific bank sids get function
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
#include <bcmdrd/chip/bcm56780_a0_enum.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcm56780_a0_fp_bank_sids_get(int unit,
                             bcmfp_stage_t *stage,
                             bcmdrd_sid_t bank_sid,
                             uint32_t *ptid)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ptid, SHR_E_PARAM);

    switch (stage->stage_id) {
        case BCMFP_STAGE_ID_LOOKUP:
        case BCMFP_STAGE_ID_EGRESS:
        case BCMFP_STAGE_ID_INGRESS:
            break;
        case BCMFP_STAGE_ID_EXACT_MATCH:
        case BCMFP_STAGE_ID_FLOW_TRACKER:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56780_a0_fp_em_bank_sids_get(unit,
                                                 bank_sid,
                                                 ptid));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}
