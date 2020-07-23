/*! \file bcm56990_b0_fp_ing_src_class_mode.c
 *
 *  APIs to update SRC_CLASS_MODE for Tomahawk-4 B0 device(56990_B0)
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/chip/bcm56990_b0_enum.h>
#include <bcmfp/chip/bcm56990_b0_fp.h>
#include <bcmdrd/chip/bcm56990_b0_enum.h>
#include <bcmfp/bcmfp_ptm_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcmfp_bcm56990_b0_ingress_src_class_mode_set(int unit,
                                             uint32_t trans_id,
                                             bcmltd_sid_t sid,
                                             int pipe_id,
                                             uint32_t mode)
{
    int index = 0;
    uint32_t profile_data = 0;
    bcmfp_stage_oper_mode_t oper_mode;
    bcmdrd_sid_t profile_sid = IFP_LOGICAL_TABLE_SELECT_CONFIGr;

    SHR_FUNC_ENTER(unit);

    bcmdrd_pt_field_set(unit,
                        IFP_LOGICAL_TABLE_SELECT_CONFIGr,
                        &profile_data,
                        SOURCE_CLASS_MODEf,
                        &mode);

    /* Get stage operational mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_mode_get(unit,
                                   BCMFP_STAGE_ID_INGRESS,
                                   &oper_mode));

    /* For both GLOBAL and GLOBAL_PIPE_AWARE modes */
    if (oper_mode != BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE) {
        pipe_id = -1;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ptm_index_insert(unit,
                                trans_id,
                                pipe_id,
                                sid,
                                profile_sid,
                                &profile_data,
                                index));

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_bcm56990_b0_em_src_class_mode_set(int unit,
                                        uint32_t trans_id,
                                        bcmltd_sid_t sid,
                                        int pipe_id,
                                        uint32_t mode)
{
    int index = 0;
    uint32_t profile_data = 0;
    bcmdrd_sid_t profile_sid = EXACT_MATCH_LOGICAL_TABLE_SELECT_CONFIGr;

    SHR_FUNC_ENTER(unit);

    bcmdrd_pt_field_set(unit,
                        EXACT_MATCH_LOGICAL_TABLE_SELECT_CONFIGr,
                        &profile_data,
                        SOURCE_CLASS_MODEf,
                        &mode);
    /* Only global mode is supported in 56990_b0 */
    pipe_id = -1;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ptm_index_insert(unit,
                                trans_id,
                                pipe_id,
                                sid,
                                profile_sid,
                                &profile_data,
                                index));

exit:
    SHR_FUNC_EXIT();
}
