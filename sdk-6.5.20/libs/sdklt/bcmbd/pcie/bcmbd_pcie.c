/*! \file bcmbd_pcie.c
 *
 * BD PCIE driver API.
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

#include <bcmbd/bcmbd_pcie.h>
#include <bcmbd/bcmbd_pcie_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* PCIe driver for each unit. */
static bcmbd_pcie_drv_t *bd_pcie_drv[BCMDRD_CONFIG_MAX_UNITS];


/*******************************************************************************
 * Public functions
 */

int
bcmbd_pcie_drv_init(int unit, bcmbd_pcie_drv_t *drv)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    bd_pcie_drv[unit] = drv;

    return SHR_E_NONE;
}

int
bcmbd_pcie_core_reg_read(int unit, uint32_t addr, uint32_t *data)
{
    bcmbd_pcie_drv_t *drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    drv = bd_pcie_drv[unit];
    if (drv && drv->read) {
        return drv->read(unit, addr, data);
    }

    return SHR_E_NONE;
}

int
bcmbd_pcie_core_reg_write(int unit, uint32_t addr, uint32_t data)
{
    bcmbd_pcie_drv_t *drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    drv = bd_pcie_drv[unit];
    if (drv && drv->write) {
        return drv->write(unit, addr, data);
    }

    return SHR_E_NONE;
}

int
bcmbd_pcie_fw_prog_done(int unit)
{
    bcmbd_pcie_drv_t *drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    drv = bd_pcie_drv[unit];
    if (drv && drv->fw_prog_done) {
        return drv->fw_prog_done(unit);
    }

    return SHR_E_NONE;
}

