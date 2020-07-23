/*! \file bcm56996_a0_fp_device_attach.c
 *
 * BCMFP driver attach function for device bcm56996_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_hal.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/chip/bcm56996_a0_fp.h>
#include <bcmlrd/bcmlrd_map.h>
#include <shr/shr_util.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmfp/bcmfp_chip_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_INIT

int
bcm56996_a0_fp_device_attach(int unit)
{
    bcmfp_stage_id_t stage_id = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    stage_id = BCMFP_STAGE_ID_INGRESS;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    if (stage != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56996_a0_fp_ingress_info_tbls_init(unit, stage));
    }

    stage_id = BCMFP_STAGE_ID_EGRESS;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    if (stage != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56996_a0_fp_egress_info_tbls_init(unit, stage));
    }

    stage_id = BCMFP_STAGE_ID_LOOKUP;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    if (stage != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56996_a0_fp_lookup_info_tbls_init(unit, stage));
    }

    stage_id = BCMFP_STAGE_ID_EXACT_MATCH;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    if (stage != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56996_a0_fp_em_info_tbls_init(unit, stage));
    }

exit:
    SHR_FUNC_EXIT();
}

