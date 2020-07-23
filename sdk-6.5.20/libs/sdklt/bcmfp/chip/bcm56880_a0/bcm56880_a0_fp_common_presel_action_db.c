/*! \file bcm56880_a0_fp_common_presel_action_db.c
 *
 * IFP/EFP/VFP presel action configuration DB initialization
 * function for Trident4(56880_A0).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/chip/bcm56880_a0_fp.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcm56880_a0_fp_common_presel_action_db_init(int unit,
                                         bcmfp_stage_t *stage)
{
    uint32_t flags = 0;
    BCMFP_PRESEL_ACTION_CFG_DECL;
    bcmltd_fid_t fid = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    /* KEY TYPE */
    fid = stage->tbls.pse_tbl->key_type_fid;
    BCMFP_PRESEL_ACTION_CFG_ADD_ONE(unit,
                                    stage,
                                    fid,
                                    flags,
                                    5,
                                    4,
                                    -1);
exit:
    SHR_FUNC_EXIT();
}

