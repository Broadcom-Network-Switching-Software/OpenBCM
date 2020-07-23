/*! \file bcm56990_a0_ptm_uft_be.c
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
#include <bcmdrd/chip/bcm56990_a0_enum.h>
#include <bcmdrd/chip/bcm56990_a0_defs.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltd/chip/bcmltd_id.h>

#include "bcm56990_a0_ptm_uft_be.h"


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_UFT


/*******************************************************************************
 * Typedefs
 */

static bcmdrd_sid_t grp_ptsid[7] = {
    INVALIDm,
    L2_ENTRY_SINGLEm,
    MPLS_ENTRY_DOUBLEm,
    EXACT_MATCH_4m,
    L3_TUNNEL_QUADm,
    L3_DEFIP_ALPM_LEVEL2m,
    L3_DEFIP_ALPM_LEVEL3m,
};

/*******************************************************************************
 * Private Functions
 */

/*!
 * \brief Initialize device specific info
 *
 * \param [in] unit Device u.
 * \param [out] dev Device specific info.
 *
 * \return SHR_E_XXX.
 */
static int
bcm56990_a0_uft_be_device_info_init(int unit,
                                    const bcmptm_uft_var_drv_t *var_info,
                                    uft_be_dev_info_t *dev)
{
    SHR_FUNC_ENTER(unit);

    dev->pt_cnt = 7;
    dev->ptsid = grp_ptsid;

    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */

int
bcm56990_a0_uft_be_driver_register(int unit, uft_be_driver_t *drv)
{
    SHR_FUNC_ENTER(unit);

    if (drv) {
        drv->device_info_init  =  bcm56990_a0_uft_be_device_info_init;
    } else {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

exit:
    SHR_FUNC_EXIT();
}

