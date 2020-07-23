/*! \file bcm56990_b0_fp_variant_attach.c
 *
 * BCMFP variant attach function for device bcm56990_b0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_types_internal.h>
#include <bcmfp/chip/bcm56990_b0_fp.h>
#include <shr/shr_util.h>
#include <bcmfp/bcmfp_chip_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_INIT

int
bcm56990_b0_di_fp_variant_attach(int unit)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_stage_id_t stage_id = 0;

    SHR_FUNC_ENTER(unit);

    stage_id = BCMFP_STAGE_ID_INGRESS;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_bcm56990_b0_ing_tbls_init(unit, stage));

    stage_id = BCMFP_STAGE_ID_EGRESS;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_bcm56990_b0_egress_tbls_init(unit, stage));

    stage_id = BCMFP_STAGE_ID_LOOKUP;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_bcm56990_b0_vlan_tbls_init(unit, stage));

    stage_id = BCMFP_STAGE_ID_EXACT_MATCH;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_bcm56990_b0_em_tbls_init(unit, stage));
exit:
    SHR_FUNC_EXIT();
}
