/*! \file bcm56780_a0_fp_common_ctr.c
 *
 * APIs to update counter related info in entry policy
 * words for Trident4-X9 device(56780_A0).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/chip/bcm56780_a0_fp.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmfp/bcmfp_oper_internal.h>
#include <bcmfp/bcmfp_cth_ctr.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcm56780_a0_fp_common_counter_actions_set(int unit,
                                    bcmfp_stage_id_t stage_id,
                                    bcmfp_group_id_t group_id,
                                    bcmfp_pdd_id_t pdd_id,
                                    bcmfp_group_oper_info_t *opinfo,
                                    bcmfp_ctr_params_t *ctr_params,
                                    uint32_t **edw)
{
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);
    SHR_NULL_CHECK(ctr_params, SHR_E_PARAM);
    SHR_NULL_CHECK(edw, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_action_set(unit,
                              stage_id,
                              pdd_id,
                              group_id,
                              opinfo->pdd_type,
                              opinfo,
                              stage->tbls.pdd_tbl->aux_flex_ctr_valid_fid,
                              ctr_params->flex_ctr_action,
                              edw));

exit:
    SHR_FUNC_EXIT();
}
