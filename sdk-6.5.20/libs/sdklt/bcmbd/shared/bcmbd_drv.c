/*! \file bcmbd_drv.c
 *
 * Base driver data structures.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>
#include <sal/sal_assert.h>

#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcmbd/bcmbd.h>

/*******************************************************************************
* Local definitions
 */

/* Log source for this component */
#define BSL_LOG_MODULE BSL_LS_BCMBD_DEV

/* Base driver object */
static bcmbd_drv_t bd_drv[BCMDRD_CONFIG_MAX_UNITS];

bcmbd_drv_t *
bcmbd_drv_get(int unit)
{
    if (!bcmdrd_dev_exists(unit)) {
        return NULL;
    }

    return &bd_drv[unit];
}

uint32_t
bcmbd_block_port_addr(int unit, int block, int lane, uint32_t offset, int idx)
{
    bcmbd_drv_t *bd;

    if (!BCMDRD_UNIT_VALID(unit)) {
        assert(0);
        return 0;
    }
    bd = &bd_drv[unit];
    assert(bd->block_port_addr);

    /* Neutralize idx if unused */
    if (idx < 0) {
        idx = 0;
    }

    return bd->block_port_addr(block, lane, offset, idx);
}

bool
bcmbd_addr_bypass(int unit, uint32_t adext, uint32_t addr)
{
    bcmbd_drv_t *bd;

    if (!BCMDRD_UNIT_VALID(unit)) {
        assert(0);
        return false;
    }

    bd = &bd_drv[unit];
    if (bd->addr_bypass) {
        return bd->addr_bypass(unit, adext, addr);
    }

    return false;
}
