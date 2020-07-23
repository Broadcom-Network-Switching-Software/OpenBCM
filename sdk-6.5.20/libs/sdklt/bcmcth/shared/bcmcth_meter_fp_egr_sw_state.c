/*! \file bcmcth_meter_fp_egr_sw_state.c
 *
 * FP meter egress software state.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmcth/bcmcth_meter_fp_egr.h>

#define BSL_LOG_MODULE  BSL_LS_BCMCTH_METER

/*! EFP Meter S/W meter table. */
static bcmcth_meter_fp_sw_state_t
meter_fp_egr_sw_state[BCMDRD_CONFIG_MAX_UNITS];

int
bcmcth_meter_fp_egr_sw_state_get(int unit,
                                 bcmcth_meter_fp_sw_state_t **ptr)
{
    SHR_FUNC_ENTER(unit);

    *ptr = &meter_fp_egr_sw_state[unit];

    SHR_FUNC_EXIT();
}
