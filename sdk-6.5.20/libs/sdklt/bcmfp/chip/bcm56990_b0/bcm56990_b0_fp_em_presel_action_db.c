/*! \file bcm56990_b0_fp_em_presel_action_db.c
 *
 *  EM preselection action configuration DB
 *  initialization function for Tomahawk-4 B0(56990_B0).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/chip/bcm56990_b0_fp.h>
#include <bcmltd/chip/bcmltd_id.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcmfp_bcm56990_b0_em_presel_action_cfg_db_init(int unit,
                                               bcmfp_stage_t *stage)
{
    uint32_t fid = 0;
    uint32_t offset = 178;
    BCMFP_PRESEL_ACTION_CFG_DECL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_PRESEL_CLASSf;
    BCMFP_PRESEL_ACTION_CFG_ADD_ONE
        (unit, stage, fid, 0, offset + 31, 2, -1);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_NORMALIZE_L2f;
    BCMFP_PRESEL_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, offset + 37, 1, -1);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_NORMALIZE_L3_L4f;
    BCMFP_PRESEL_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, offset + 38, 1, -1);
exit:
    SHR_FUNC_EXIT();
}

