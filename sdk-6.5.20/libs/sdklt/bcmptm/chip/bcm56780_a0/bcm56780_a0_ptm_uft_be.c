/*! \file bcm56780_a0_ptm_uft_be.c
 *
 * Chip specific functions for PTM UFT_BE
 *
 * This file contains the chip specific functions for PTM UFT_BE
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


/*******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_symbols.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmdrd/chip/bcm56780_a0_defs.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmptm/bcmptm_cth_uft_be_internal.h>
#include <bcmptm/bcmptm_uft_internal.h>

#include "bcm56780_a0_ptm_uft_be.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_UFT


/*******************************************************************************
 * Typedefs
 */

static uft_tile_hit_profile_info_t hit_prof[] = {
    {"EFTA10_I1T_00", EFTA10_I1T_00_HIT_INDEX_PROFILE_0m, HIT_CONTEXTf},
    {"EFTA10_I1T_01", EFTA10_I1T_01_HIT_INDEX_PROFILE_0m, HIT_CONTEXTf},
    {"EFTA10_I1T_02", EFTA10_I1T_02_HIT_INDEX_PROFILE_0m, HIT_CONTEXTf},
    {"EFTA10_I1T_03", EFTA10_I1T_03_HIT_INDEX_PROFILE_0m, HIT_CONTEXTf},
    {"EFTA20_I1T_00", EFTA20_I1T_00_HIT_INDEX_PROFILE_0m, HIT_CONTEXTf},
    {"EFTA20_I1T_01", EFTA20_I1T_01_HIT_INDEX_PROFILE_0m, HIT_CONTEXTf},
    {"EFTA20_I1T_02", EFTA20_I1T_02_HIT_INDEX_PROFILE_0m, HIT_CONTEXTf},
    {"EFTA20_I1T_03", EFTA20_I1T_03_HIT_INDEX_PROFILE_0m, HIT_CONTEXTf},
    {"EFTA20_I1T_04", EFTA20_I1T_04_HIT_INDEX_PROFILE_0m, HIT_CONTEXTf},
    {"EFTA20_I1T_05", EFTA20_I1T_05_HIT_INDEX_PROFILE_0m, HIT_CONTEXTf},
    {"EFTA20_I1T_06", EFTA20_I1T_06_HIT_INDEX_PROFILE_0m, HIT_CONTEXTf},
    {"IFTA10_I1T_00", IFTA10_I1T_00_HIT_INDEX_PROFILE_0m, HIT_CONTEXTf},
    {"IFTA130_I1T_00", IFTA130_I1T_00_HIT_INDEX_PROFILE_0m, HIT_CONTEXTf},
    {"IFTA130_I1T_01", IFTA130_I1T_01_HIT_INDEX_PROFILE_0m, HIT_CONTEXTf},
    {"IFTA130_I1T_02", IFTA130_I1T_02_HIT_INDEX_PROFILE_0m, HIT_CONTEXTf},
    {"IFTA130_I1T_03", IFTA130_I1T_03_HIT_INDEX_PROFILE_0m, HIT_CONTEXTf},
    {"IFTA140_I1T_00", IFTA140_I1T_00_HIT_INDEX_PROFILE_0m, HIT_CONTEXTf},
    {"IFTA20_I1T_00", IFTA20_I1T_00_HIT_INDEX_PROFILE_0m, HIT_CONTEXTf},
    {"IFTA50_I1T_00", IFTA50_I1T_00_HIT_INDEX_PROFILE_0m, HIT_CONTEXTf},
    {"IFTA50_I1T_01", IFTA50_I1T_01_HIT_INDEX_PROFILE_0m, HIT_CONTEXTf},
    {"IFTA60_I1T_00", IFTA60_I1T_00_HIT_INDEX_PROFILE_0m, HIT_CONTEXTf},
    {"IFTA60_I1T_01", IFTA60_I1T_01_HIT_INDEX_PROFILE_0m, HIT_CONTEXTf},
    {"IFTA60_I1T_02", IFTA60_I1T_02_HIT_INDEX_PROFILE_0m, HIT_CONTEXTf},
    {"IFTA70_I1T_00", IFTA70_I1T_00_HIT_INDEX_PROFILE_0m, HIT_CONTEXTf},
    {"IFTA70_I1T_01", IFTA70_I1T_01_HIT_INDEX_PROFILE_0m, HIT_CONTEXTf},
    {"EFTA30_E2T_00", EFTA30_E2T_00_HIT_INDEX_PROFILEm, HIT_CONTEXTf},
    {"IFTA30_E2T_00", IFTA30_E2T_00_HIT_INDEX_PROFILEm, HIT_CONTEXTf},
    {"IFTA40_E2T_00", IFTA40_E2T_00_HIT_INDEX_PROFILEm, HIT_CONTEXTf},
    {"IFTA40_E2T_01", IFTA40_E2T_01_HIT_INDEX_PROFILEm, HIT_CONTEXTf},
    {"IFTA40_E2T_02", IFTA40_E2T_02_HIT_INDEX_PROFILEm, HIT_CONTEXTf},
    {"IFTA80_E2T_00", IFTA80_E2T_00_HIT_INDEX_PROFILEm, HIT_CONTEXTf},
    {"IFTA80_E2T_01", IFTA80_E2T_01_HIT_INDEX_PROFILEm, HIT_CONTEXTf},
    {"IFTA80_E2T_02", IFTA80_E2T_02_HIT_INDEX_PROFILEm, HIT_CONTEXTf},
    {"IFTA80_E2T_03", IFTA80_E2T_03_HIT_INDEX_PROFILEm, HIT_CONTEXTf},
    {"IFTA90_E2T_00", IFTA90_E2T_00_HIT_INDEX_PROFILEm, HIT_CONTEXTf},
    {"IFTA90_E2T_01", IFTA90_E2T_01_HIT_INDEX_PROFILEm, HIT_CONTEXTf},
    {"IFTA90_E2T_02", IFTA90_E2T_02_HIT_INDEX_PROFILEm, HIT_CONTEXTf},
    {"IFTA90_E2T_03", IFTA90_E2T_03_HIT_INDEX_PROFILEm, HIT_CONTEXTf},
};

/*******************************************************************************
 * External Variables
 */


/*******************************************************************************
 * Private Functions
 */

/*!
 * \brief Initialize device specific info.
 *
 * \param [in] unit Device u.
 * \param [out] dev Device specific info.
 *
 * \return SHR_E_XXX.
 */
static int
bcm56780_a0_uft_be_device_info_init(int unit,
                                    const bcmptm_uft_var_drv_t *var_info,
                                    uft_be_dev_info_t *dev)
{
    SHR_FUNC_ENTER(unit);

    dev->ptsid        = NULL;
    dev->bank_ptsid   = var_info->bank_ptsid;
    dev->alpm_ptsid   = var_info->alpm_ptsid;
    dev->pt_cnt       = var_info->em_bank_cnt;
    dev->var          = var_info;
    dev->hit_prof     = hit_prof;
    dev->hit_prof_cnt = sizeof(hit_prof)/sizeof(uft_tile_hit_profile_info_t);

    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */

int
bcm56780_a0_uft_be_driver_register(int unit, uft_be_driver_t *drv)
{
    SHR_FUNC_ENTER(unit);

    if (drv) {
        drv->device_info_init     = bcm56780_a0_uft_be_device_info_init;
    } else {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

exit:
    SHR_FUNC_EXIT();
}

