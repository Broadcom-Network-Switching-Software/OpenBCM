/*! \file bcmbd_attach.c
 *
 * Attach chip-specific software drivers.
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

#include <bcmlrd/bcmlrd_conf.h>

#include <bcmbd/bcmbd.h>

/*******************************************************************************
* Local definitions
 */

/* Log source for this component */
#define BSL_LOG_MODULE BSL_LS_BCMBD_DEV

typedef int (*bcmbd_drv_attach_f)(int unit);

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_bd_attach },
static struct {
    bcmbd_drv_attach_f attach;
} dev_attach[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

typedef int (*bcmbd_drv_detach_f)(int unit);

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_bd_detach },
static struct {
    bcmbd_drv_detach_f detach;
} dev_detach[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

typedef int (*bcmbd_var_attach_f)(int unit);

/* Define stub functions for base variant. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
int _bc##_bd_var_attach(int unit) {return SHR_E_NONE;}
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>

#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    { _bd##_vu##_va##_##bd_var_attach},
static struct {
    bcmbd_var_attach_f attach;
} variant_attach[] = {
    { 0 },
#include <bcmlrd/chip/bcmlrd_variant.h>
    { 0 }
};

int
bcmbd_attach(int unit)
{
    bcmdrd_dev_type_t dev_type;
    bcmbd_drv_t *bd;
    bcmlrd_variant_t variant;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    bd = bcmbd_drv_get(unit);
    assert(bd);

    if (bd->dev_type != BCMDRD_DEV_T_NONE) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Base driver already attached\n")));
        /* Warn without error */
        SHR_IF_ERR_EXIT(SHR_E_NONE);
    }

    bd->dev_type = dev_type;

    /* Perform device-specific software setup */
    SHR_IF_ERR_EXIT
        (dev_attach[dev_type].attach(unit));

    variant = bcmlrd_variant_get(unit);
    if (variant != BCMLRD_VARIANT_T_NONE) {
        SHR_IF_ERR_EXIT
            (variant_attach[variant].attach(unit));
    }

exit:
    if (SHR_FUNC_ERR()) {
        /* Detach driver to clean up if attach fails */
        bcmbd_detach(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmbd_detach(int unit)
{
    bcmdrd_dev_type_t dev_type;
    bcmbd_drv_t *bd;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    bd = bcmbd_drv_get(unit);
    assert(bd);

    if (bd->dev_type == BCMDRD_DEV_T_NONE) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Base driver not attached\n")));
        /* Warn without error */
        SHR_IF_ERR_EXIT(SHR_E_NONE);
    }

    if (bcmbd_dev_running(unit)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Stopping device before detaching\n")));
        SHR_IF_ERR_EXIT
            (bcmbd_dev_stop(unit));
    }

    /* Perform device-specific software tear-down */
    SHR_IF_ERR_EXIT
        (dev_detach[dev_type].detach(unit));

    /* Mark as detached */
    sal_memset(bd, 0, sizeof(*bd));

  exit:
    SHR_FUNC_EXIT();
}

bool
bcmbd_attached(int unit)
{
    bcmdrd_dev_type_t dev_type;
    bcmbd_drv_t *bd;

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        return false;
    }

    bd = bcmbd_drv_get(unit);
    assert(bd);

    if (bd->dev_type == BCMDRD_DEV_T_NONE) {
        return false;
    }

    return true;
}
