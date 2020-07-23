/*! \file bcm56880_a0_ptm_uft_be.c
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
#include <bcmdrd/chip/bcm56880_a0_enum.h>
#include <bcmdrd/chip/bcm56880_a0_defs.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmptm/bcmptm_cth_uft_be_internal.h>
#include <bcmptm/bcmptm_uft_internal.h>

#include "bcm56880_a0_ptm_uft_be.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_UFT


/*******************************************************************************
 * Typedefs
 */


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
bcm56880_a0_uft_be_device_info_init(int unit,
                                    const bcmptm_uft_var_drv_t *var_info,
                                    uft_be_dev_info_t *dev)
{
    SHR_FUNC_ENTER(unit);

    dev->ptsid      = NULL;
    dev->bank_ptsid = var_info->bank_ptsid;
    dev->alpm_ptsid = var_info->alpm_ptsid;
    dev->pt_cnt     = var_info->em_bank_cnt;
    dev->var        = var_info;

    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */

int
bcm56880_a0_uft_be_driver_register(int unit, uft_be_driver_t *drv)
{
    SHR_FUNC_ENTER(unit);

    if (drv) {
        drv->device_info_init     = bcm56880_a0_uft_be_device_info_init;
    } else {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

exit:
    SHR_FUNC_EXIT();
}

