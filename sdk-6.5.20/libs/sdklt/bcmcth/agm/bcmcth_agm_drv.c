/*! \file bcmcth_agm_drv.c
 *
 * BCMCTH Aggregation Group Monitor Driver APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <sal/sal_libc.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcmbd/bcmbd_ts.h>

#include <bcmcth/bcmcth_agm_drv.h>

/*******************************************************************************
 * Local definitions
 */

/*! Device specific attach function type. */
typedef bcmcth_agm_drv_t *(*bcmcth_agm_drv_get_f)(int unit);

/*! Array of device specific attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_cth_agm_drv_get },
static struct {
    bcmcth_agm_drv_get_f drv_get;
} agm_drv_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

static bcmcth_agm_drv_t *agm_drv[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
* Public functions
 */

int
bcmcth_agm_drv_init(int unit)
{
    bcmdrd_dev_type_t dev_type;

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        return SHR_E_UNIT;
    }

    /* Perform device-specific software setup */
    agm_drv[unit] = agm_drv_get[dev_type].drv_get(unit);

    return SHR_E_NONE;
}

int
bcmcth_agm_init(int unit)
{
    int rv;
    bool ts_en = false;

    if (!agm_drv[unit]) {
        return SHR_E_NONE;
    }

    /* Check if TimeSync timestamp counter is enabled. */
    rv = bcmbd_ts_timestamp_enable_get(unit, BCMAGM_TS_INST, &ts_en);
    if (SHR_SUCCESS(rv) && !ts_en) {
        rv = SHR_E_DISABLED;
    }
    return rv;
}

int
bcmcth_agm_cleanup(int unit)
{
    return SHR_E_NONE;
}

int
bcmcth_agm_control_set(int unit,
                       bcmcth_agm_control_t *agm_ctrl)
{
    if (agm_drv[unit] && agm_drv[unit]->agm_control_set) {
        return agm_drv[unit]->agm_control_set(unit, agm_ctrl);
    }

    return SHR_E_NONE;
}
