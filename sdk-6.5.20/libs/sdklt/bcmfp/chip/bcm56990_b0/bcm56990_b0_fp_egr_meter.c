/*! \file bcm56990_b0_fp_egr_meter.c
 *
 * APIs to update meter related info
 * in entry policy words for
 * TH4 device(56990_B0).
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

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcmfp_bcm56990_b0_egress_meter_actions_set(int unit,
                                bcmfp_group_id_t group_id,
                                bcmfp_pdd_id_t pdd_id,
                                bcmfp_group_oper_info_t *opinfo,
                                bcmcth_meter_fp_policy_fields_t *meter_entry,
                                uint32_t **edw)
{
    SHR_FUNC_ENTER(unit);

    SHR_FUNC_EXIT();
}
