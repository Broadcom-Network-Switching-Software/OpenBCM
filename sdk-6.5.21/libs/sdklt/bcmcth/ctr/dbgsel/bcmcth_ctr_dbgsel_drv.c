/*! \file bcmcth_ctr_dbgsel_drv.c
 *
 * BCMCTH ING/EGR Debug Counter Trigger Selection Driver APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <sal/sal_libc.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcmcth/bcmcth_ctr_dbgsel_drv.h>

/*******************************************************************************
 * Local definitions
 */

/*! Device specific attach function type. */
typedef bcmcth_ctr_dbgsel_drv_t *(*bcmcth_ctr_dbgsel_drv_get_f)(int unit);

/*! Array of device specific attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,\
                             _cn,_pf,_pd,_r0,_r1) \
    { _bc##_cth_ctr_dbgsel_drv_get },
static struct {
    bcmcth_ctr_dbgsel_drv_get_f drv_get;
} ctr_dbgsel_drv_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

static bcmcth_ctr_dbgsel_drv_t *ctr_dbgsel_drv[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
* Public functions
 */

int
bcmcth_ctr_dbgsel_drv_init(int unit)
{
    bcmdrd_dev_type_t dev_type;

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        return SHR_E_UNIT;
    }

    /* Perform device-specific software setup */
    ctr_dbgsel_drv[unit] = ctr_dbgsel_drv_get[dev_type].drv_get(unit);

    return SHR_E_NONE;
}

int bcmcth_ctr_dbgsel_set(int unit, bool ingress, int op,
                          uint8_t idx, ctr_dbgsel_trigger_t *trigger_array)
{
    if (ctr_dbgsel_drv[unit]  && ctr_dbgsel_drv[unit]->dbgsel_set) {
        return ctr_dbgsel_drv[unit]->dbgsel_set(unit, ingress, op, idx,
                                                trigger_array);
    }
    return SHR_E_NONE;
}

