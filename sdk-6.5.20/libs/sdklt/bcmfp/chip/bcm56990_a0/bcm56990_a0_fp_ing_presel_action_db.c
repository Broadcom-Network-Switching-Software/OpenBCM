/*! \file bcm56990_a0_fp_ing_presel_action_db.c
 *
 *  IFP preselection action configuration DB
 *  initialization function for Tomahawk-4(56990_A0).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/chip/bcm56990_a0_fp.h>
#include <bcmltd/chip/bcmltd_id.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcmfp_bcm56990_a0_ingress_presel_action_cfg_db_init(int unit,
                                                    bcmfp_stage_t *stage)
{
    uint32_t fid = 0;
    uint32_t offset = 210;
    BCMFP_PRESEL_ACTION_CFG_DECL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_PRESEL_CLASSf;
    BCMFP_PRESEL_ACTION_CFG_ADD_ONE
        (unit, stage, fid, 0, offset + 40, 2, -1);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_NORMALIZE_L2f;
    BCMFP_PRESEL_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, offset + 49, 1, -1);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_NORMALIZE_L3_L4f;
    BCMFP_PRESEL_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, offset + 50, 1, -1);
exit:
    SHR_FUNC_EXIT();
}

