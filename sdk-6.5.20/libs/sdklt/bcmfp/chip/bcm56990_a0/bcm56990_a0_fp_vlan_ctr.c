/*! \file bcm56990_a0_fp_vlan_ctr.c
 *
 * APIs to update counter related info
 * in entry policy words for
 * TH4 device(56990_A0).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmfp/chip/bcm56990_a0_fp.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcmfp_bcm56990_a0_vlan_counter_actions_set(int unit,
                                   bcmfp_stage_id_t stage_id,
                                   bcmfp_group_id_t group_id,
                                   bcmfp_pdd_id_t pdd_id,
                                   bcmfp_group_oper_info_t *opinfo,
                                   bcmfp_ctr_params_t *ctr_params,
                                   uint32_t **edw)
{
    uint32_t value = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);
    SHR_NULL_CHECK(ctr_params, SHR_E_PARAM);
    SHR_NULL_CHECK(edw, SHR_E_PARAM);

    /* FLEX_CTR_ACTION */
    value = ctr_params->flex_ctr_action;
    bcmdrd_field_set(edw[0], 38, 42, &(value));

    /* FLEX_CTR_OBJECT */
    value = ctr_params->flex_ctr_object;
    bcmdrd_field_set(edw[0], 43, 52, &(value));
exit:
    SHR_FUNC_EXIT();
}
