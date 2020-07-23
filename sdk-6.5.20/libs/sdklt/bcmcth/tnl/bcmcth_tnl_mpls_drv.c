/*! \file bcmcth_tnl_mpls_drv.c
 *
 * BCMCTH TNL MPLS driver APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmcth/bcmcth_tnl_mpls_drv.h>

/*******************************************************************************
 * Local definitions
 */
/*! Device specific attach function type. */
typedef bcmcth_tnl_mpls_drv_t *(*bcmcth_tnl_mpls_drv_get_f)(int unit);

/*! Array of device specific attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_cth_tnl_mpls_drv_get },
static struct {
    bcmcth_tnl_mpls_drv_get_f drv_get;
} mpls_drv_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

static bcmcth_tnl_mpls_drv_t *mpls_drv[BCMDRD_CONFIG_MAX_UNITS];
/*******************************************************************************
 * Public Functions
 */
int
bcmcth_tnl_mpls_drv_init(int unit)
{
    bcmdrd_dev_type_t dev_type;

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        return SHR_E_UNIT;
    }

    /* Perform device-specific software setup */
    mpls_drv[unit] = mpls_drv_get[dev_type].drv_get(unit);

    return SHR_E_NONE;
}

int
bcmcth_tnl_mpls_exp_qos_ctrl_set(int unit,
                                 bcmcth_tnl_mpls_exp_qos_ctrl_t *lt_entry)
{
    if (mpls_drv[unit] && mpls_drv[unit]->qos_ctrl_set) {
        return mpls_drv[unit]->qos_ctrl_set(unit, lt_entry);
    }

    return SHR_E_NONE;
}

int
bcmcth_tnl_mpls_exp_remark_ctrl_set(int unit,
                                    bcmcth_tnl_mpls_exp_remark_ctrl_t *lt_entry)
{
    if (mpls_drv[unit] && mpls_drv[unit]->remark_ctrl_set) {
        return mpls_drv[unit]->remark_ctrl_set(unit, lt_entry);
    }

    return SHR_E_NONE;
}

