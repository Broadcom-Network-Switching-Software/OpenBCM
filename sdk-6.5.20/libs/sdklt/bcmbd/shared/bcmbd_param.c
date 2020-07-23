/*! \file bcmbd_param.c
 *
 * APIs for obtaining PT parameters.
 *
 * Since the PT parameters are also used internally within the BD, the
 * implementation reasides in the 'shared' sub-component instead of
 * 'main'.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <shr/shr_error.h>

#include <bcmbd/bcmbd.h>

int
bcmbd_pt_param_get(int unit, bcmdrd_sid_t sid,
                   bcmbd_pt_param_t ptype, uint32_t *pval)
{
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);

    if (bd == NULL) {
        return SHR_E_UNIT;
    }

    if (bd->pt_param_get == NULL) {
        return SHR_E_UNAVAIL;
    }

    return bd->pt_param_get(unit, sid, ptype, pval);
}

uint32_t
bcmbd_mor_clks_read_get(int unit, bcmdrd_sid_t sid)
{
    int rv;
    uint32_t mor_clks;

    rv = bcmbd_pt_param_get(unit, sid,
                            BCMBD_PT_PARAM_MOR_CLKS_READ, &mor_clks);
    if (SHR_FAILURE(rv)) {
        mor_clks = 0;
    }
    return mor_clks;
}

uint32_t
bcmbd_mor_clks_write_get(int unit, bcmdrd_sid_t sid)
{
    int rv;
    uint32_t mor_clks;

    rv = bcmbd_pt_param_get(unit, sid,
                            BCMBD_PT_PARAM_MOR_CLKS_WRITE, &mor_clks);
    if (SHR_FAILURE(rv)) {
        mor_clks = 0;
    }
    return mor_clks;
}

