/*! \file bcmecmp_attach.c
 *
 * Attach chip-specific ECMP software drivers.
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
#include <bcmecmp/bcmecmp.h>
#include <bcmlrd/bcmlrd_conf.h>

/*******************************************************************************
* Local definitions
 */

/* Log source for this component */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_ECMP

typedef int (*bcmecmp_drv_attach_f)(int unit);

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_ecmp_attach },
static struct {
    bcmecmp_drv_attach_f attach;
} dev_attach[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

typedef int (*bcmecmp_drv_detach_f)(int unit);

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_ecmp_detach },
static struct {
    bcmecmp_drv_detach_f detach;
} dev_detach[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

typedef int (*bcmecmp_drv_var_attach_f)(int unit, bcmecmp_drv_var_t *drv_var);

#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    { _bd##_vu##_va##_ecmp_drv_var_attach },
static struct {
    bcmecmp_drv_var_attach_f attach;
} dev_var_attach[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmlrd/chip/bcmlrd_variant.h>
    { 0 } /* end-of-list */
};

/*!
 * \brief Initialize device driver.
 *
 * Initialize device features and install base driver functions.
 *
 * \param [in] unit Unit number.
 */
int
bcmecmp_attach(int unit)
{
    bcmdrd_dev_type_t dev_type;
    bcmecmp_drv_t *ecmpd;
    bcmlrd_variant_t variant;
    bcmecmp_drv_var_t *ecmpd_var;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    ecmpd = bcmecmp_drv_get(unit);
    SHR_NULL_CHECK(ecmpd, SHR_E_FAIL);

    if (ecmpd->dev_type != BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_EXISTS);
    }

    ecmpd->dev_type = dev_type;

    /* Perform device-specific software setup */
    SHR_IF_ERR_EXIT
        (dev_attach[dev_type].attach(unit));

    /* Perform ECMP variant-specific driver attach. */
    variant = bcmlrd_variant_get(unit);
    if (variant == BCMLRD_VARIANT_T_NONE) {
        return SHR_E_UNIT;
    }

    ecmpd_var = bcmecmp_drv_var_get(unit);
    SHR_NULL_CHECK(ecmpd_var, SHR_E_FAIL);
    sal_memset(ecmpd_var, 0, sizeof(*ecmpd_var));
    SHR_IF_ERR_EXIT
        (dev_var_attach[variant].attach(unit, ecmpd_var));
  exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clean up device driver.
 *
 * Release any resources allocated by \ref bcmecmp_attach.
 *
 * \param [in] unit Unit number.
 */
int
bcmecmp_detach(int unit)
{
    bcmdrd_dev_type_t dev_type;
    bcmecmp_drv_t *ecmpd;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    ecmpd = bcmecmp_drv_get(unit);
    assert(ecmpd);

    if (ecmpd->dev_type == BCMDRD_DEV_T_NONE) {
        /* Not attached */
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    /* Perform device-specific software tear-down */
    SHR_IF_ERR_EXIT
        (dev_detach[dev_type].detach(unit));

    /* Mark as deattached */
    ecmpd->dev_type = BCMDRD_DEV_T_NONE;

  exit:
    SHR_FUNC_EXIT();
}
