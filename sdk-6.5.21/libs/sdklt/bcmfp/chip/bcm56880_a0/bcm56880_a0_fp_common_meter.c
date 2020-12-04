/*! \file bcm56880_a0_fp_common_meter.c
 *
 * APIs to update meter related info in entry policy
 * words for Trident4 device(56880_A0).
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
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/chip/bcm56880_a0_enum.h>
#include <bcmfp/bcmfp_oper_internal.h>
#include <bcmcth/bcmcth_meter_util.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcm56880_a0_fp_common_meter_actions_set(int unit,
                                        bcmfp_group_id_t group_id,
                                        bcmfp_pdd_id_t pdd_id,
                                        bcmfp_group_oper_info_t *opinfo,
                                        bcmcth_meter_fp_policy_fields_t *meter_entry,
                                        uint32_t **edw)
{
    SHR_FUNC_ENTER(unit);

    SHR_FUNC_EXIT();
}
