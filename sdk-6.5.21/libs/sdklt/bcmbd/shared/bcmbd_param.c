/*! \file bcmbd_param.c
 *
 * APIs for obtaining device and PT parameters.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <shr/shr_error.h>

#include <bcmbd/bcmbd.h>

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

uint32_t
bcmbd_hmi_cmdbuf_size_get(int unit)
{
    int rv;
    uint32_t cmdbuf_size;

    rv = bcmbd_dev_param_get(unit,
                             BCMBD_DEV_PRM_HMI_CMDBUF_SIZE, &cmdbuf_size);
    if (SHR_FAILURE(rv)) {
        cmdbuf_size = 0;
    }
    return cmdbuf_size;
}
