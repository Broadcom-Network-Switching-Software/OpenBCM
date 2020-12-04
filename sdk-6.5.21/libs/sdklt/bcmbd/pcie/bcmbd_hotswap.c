/*! \file bcmbd_pcie_hotswap.c
 *
 * BD PCIE hotswap driver API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <sal/sal_libc.h>

#include <bcmdrd/bcmdrd_chip.h>
#include <bcmdrd/bcmdrd_dev.h>

#include <bcmbd/bcmbd_hotswap.h>
#include <bcmbd/bcmbd_hotswap_internal.h>

#define BSL_LOG_MODULE  BSL_LS_BCMBD_DEV

/*******************************************************************************
 * Local definitions
 */

/* PCIe driver for each unit. */
static bcmbd_hotswap_drv_t *bd_hotswap_drv[BCMDRD_CONFIG_MAX_UNITS];


/*******************************************************************************
 * Public functions
 */

int
bcmbd_hotswap_drv_init(int unit, bcmbd_hotswap_drv_t *drv)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    bd_hotswap_drv[unit] = drv;

    return SHR_E_NONE;
}

int
bcmbd_hotswap_enable_set(int unit, bool enable)
{
    bcmbd_hotswap_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    drv = bd_hotswap_drv[unit];
    if (drv && drv->hotswap_enable_set) {
        SHR_IF_ERR_EXIT(drv->hotswap_enable_set(unit, enable));
    }

exit:
    SHR_FUNC_EXIT();
}


int
bcmbd_hotswap_enable_get(int unit, bool *enable)
{
    bcmbd_hotswap_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    drv = bd_hotswap_drv[unit];
    if (drv && drv->hotswap_enable_get) {
        SHR_IF_ERR_EXIT(drv->hotswap_enable_get(unit, enable));
    }

exit:
    SHR_FUNC_EXIT();
}
