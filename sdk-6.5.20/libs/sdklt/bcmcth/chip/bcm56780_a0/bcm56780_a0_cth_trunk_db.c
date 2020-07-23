/*! \file bcm56780_a0_cth_trunk.c
 *
 * Chip driver for BCMCTH TRUNK.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <sal/sal_assert.h>
#include <sal/sal_libc.h>
#include <sal/sal_types.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmcth/bcmcth_trunk_drv.h>
#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmdrd/chip/bcm56780_a0_defs.h>
#include <bcmcth/chip/bcm56780_a0/bcm56780_a0_trunk_vp.ptrm.data.h>

#define BSL_LOG_MODULE BSL_LS_BCMCTH_TRUNK

static bcmcth_trunk_drv_var_ids_t
    bcm56780_a0_cth_trunk_vp_drv_var_ids = {
   .trunk_vp_weighted_ltid = TRUNK_VP_WEIGHTEDt,
   .trunk_vp_weighted_trunk_vp_id = TRUNK_VP_WEIGHTEDt_TRUNK_VP_IDf,
   .trunk_vp_weighted_wt_size_id = TRUNK_VP_WEIGHTEDt_WEIGHTED_SIZEf,
   .trunk_vp_weighted_member_cnt_id  = TRUNK_VP_WEIGHTEDt_MEMBER_CNTf,
   .trunk_vp_ltid = TRUNK_VPt,
   .trunk_vp_trunk_vp_id = TRUNK_VPt_TRUNK_VP_IDf,
   .trunk_vp_lb_mode_id = TRUNK_VPt_LB_MODEf,
   .trunk_vp_max_members_id = TRUNK_VPt_MAX_MEMBERSf,
   .trunk_vp_member_cnt_id= TRUNK_VPt_MEMBER_CNTf,
};
static bcmcth_trunk_vp_drv_var_t  bcm56780_a0_cth_trunk_vp_drv_var =  {
    .flds_mem0 = bcm56780_a0_trunk_vp_flds_mem0,
    .wflds_mem0 = bcm56780_a0_trunk_vp_wflds_mem0,
    .member0_info = &bcm56780_a0_trunk_vp_member0_info,
    .wmember0_info = &bcm56780_a0_trunk_vp_wmember0_info,
    .mem0 = bcm56780_a0_trunk_vp_mem0,
    .ids = &bcm56780_a0_cth_trunk_vp_drv_var_ids,
};
static bcmcth_trunk_drv_var_t bcm56780_a0_cth_trunk_drv_var =  {
    .trunk_vp_var = &bcm56780_a0_cth_trunk_vp_drv_var,
};
bcmcth_trunk_drv_var_t *
bcm56780_a0_cth_trunk_drv_var_attach(int unit)
{
    return &bcm56780_a0_cth_trunk_drv_var;
}
