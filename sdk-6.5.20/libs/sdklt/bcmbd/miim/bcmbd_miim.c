/*! \file bcmbd_miim.c
 *
 * BD TimeSync driver API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <sal/sal_libc.h>

#include <bcmdrd/bcmdrd_chip.h>

#include <bcmbd/bcmbd_miim_internal.h>

#define BSL_LOG_MODULE  BSL_LS_BCMBD_MIIM

/*******************************************************************************
 * Local definitions
 */

static const bcmbd_miim_drv_t *bd_miim_drv[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Public functions
 */

int
bcmbd_miim_drv_init(int unit, const bcmbd_miim_drv_t *miim_drv)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    bd_miim_drv[unit] = miim_drv;

    return SHR_E_NONE;
}

int
bcmbd_miim_init(int unit)
{
    const bcmbd_miim_drv_t *miim_drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    miim_drv = bd_miim_drv[unit];
    if (miim_drv && miim_drv->init) {
        return miim_drv->init(unit);
    }
    return SHR_E_NONE;
}

int
bcmbd_miim_cleanup(int unit)
{
    const bcmbd_miim_drv_t *miim_drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    miim_drv = bd_miim_drv[unit];
    if (miim_drv && miim_drv->cleanup) {
        return miim_drv->cleanup(unit);
    }
    return SHR_E_NONE;
}

int
bcmbd_miim_op(int unit, bcmbd_miim_op_t *op, uint32_t *data)
{
    const bcmbd_miim_drv_t *miim_drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    miim_drv = bd_miim_drv[unit];
    if (miim_drv && miim_drv->miim_op) {
        return miim_drv->miim_op(unit, op, data);
    }
    return SHR_E_UNAVAIL;
}

int
bcmbd_miim_rate_config_set(int unit, bool internal, int busno,
                          bcmbd_miim_rate_config_t *ratecfg)
{
    const bcmbd_miim_drv_t *miim_drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    miim_drv = bd_miim_drv[unit];
    if (miim_drv && miim_drv->rate_config_set) {
        return miim_drv->rate_config_set(unit, internal, busno, ratecfg);
    }
    return SHR_E_NONE;
}

int
bcmbd_miim_read(int unit, uint32_t phy, uint32_t reg, uint32_t *val)
{
    bcmbd_miim_op_t miim_op, *op = &miim_op;

    sal_memset(op, 0, sizeof(*op));

    if (BCMBD_MIIM_PHY_C45_GET(phy)) {
        op->opcode = BCMBD_MIIM_OPC_CL45_READ;
    } else {
        op->opcode = BCMBD_MIIM_OPC_CL22_READ;
    }
    op->internal = BCMBD_MIIM_PHY_INT_GET(phy);
    op->busno = BCMBD_MIIM_PHY_BUS_GET(phy);
    op->phyad = BCMBD_MIIM_PHY_ID_GET(phy);

    op->regad = BCMBD_MIIM_REGAD_GET(reg);
    op->devad = BCMBD_MIIM_DEVAD_GET(reg);

    return bcmbd_miim_op(unit, op, val);
}

int
bcmbd_miim_write(int unit, uint32_t phy, uint32_t reg, uint32_t val)
{
    bcmbd_miim_op_t miim_op, *op = &miim_op;

    sal_memset(op, 0, sizeof(*op));

    if (BCMBD_MIIM_PHY_C45_GET(phy)) {
        op->opcode = BCMBD_MIIM_OPC_CL45_WRITE;
    } else {
        op->opcode = BCMBD_MIIM_OPC_CL22_WRITE;
    }
    op->internal = BCMBD_MIIM_PHY_INT_GET(phy);
    op->busno = BCMBD_MIIM_PHY_BUS_GET(phy);
    op->phyad = BCMBD_MIIM_PHY_ID_GET(phy);

    op->regad = BCMBD_MIIM_REGAD_GET(reg);
    op->devad = BCMBD_MIIM_DEVAD_GET(reg);

    return bcmbd_miim_op(unit, op, &val);
}
