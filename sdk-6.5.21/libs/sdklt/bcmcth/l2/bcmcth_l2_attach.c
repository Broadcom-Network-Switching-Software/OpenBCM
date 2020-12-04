/*! \file bcmcth_l2_attach.c
 *
 * BCMCTH L2 driver attach/detach
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_conf.h>
#include <bcmcth/bcmcth_l2.h>
#include <bcmcth/bcmcth_l2_util.h>

#define BSL_LOG_MODULE BSL_LS_BCMCTH_L2

/*! Device specific attach function type. */
typedef int (*bcmcth_l2_attach_f)(int unit);

/*! Array of device specific attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_cth_l2_attach },
static struct {
    bcmcth_l2_attach_f attach;
} dev_attach[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};


/*! Variant specific attach function type. */
typedef int (*bcmcth_l2_var_attach_f)(int unit);

/* Define stub functions for base variant. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
int _bc##_cth_l2_var_attach(int unit) {return SHR_E_NONE;}
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>

/*! Array of variant specific attach functions */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    { _bd##_vu##_va##_cth_l2_var_attach },
static struct {
    bcmcth_l2_var_attach_f attach;
} variant_attach[] = {
    { 0 },
#include <bcmlrd/chip/bcmlrd_variant.h>
    { 0 }
};


/*******************************************************************************
 * Public Functions
 */
int
bcmcth_l2_attach(int unit, bool warm)
{
    bcmdrd_dev_type_t dev_type;
    bcmlrd_variant_t variant;
    bcmcth_l2_drv_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    /* Clear driver block, device attach may be null operation. */
    drv = bcmcth_l2_drv_get(unit);
    sal_memset(drv, 0, sizeof(bcmcth_l2_drv_t));
    drv->cfg_sid = BCMLTD_SID_INVALID;
    drv->state_sid = BCMLTD_SID_INVALID;
    drv->state_ptsid = BCMDRD_INVALID_ID;

    /* Perform device-specific software setup. */
    SHR_IF_ERR_EXIT
        (dev_attach[dev_type].attach(unit));

    variant = bcmlrd_variant_get(unit);
    if (variant != BCMLRD_VARIANT_T_NONE) {
        SHR_IF_ERR_EXIT
            (variant_attach[variant].attach(unit));
    }

    /* Initialize hardware pipe info. */
    SHR_IF_ERR_EXIT
        (bcmcth_l2_pipe_info_get(unit, drv->state_ptsid, &(drv->pipe_info)));

    /* Initialize device specific software. */
    SHR_IF_ERR_EXIT
        (bcmcth_l2_drv_init(unit, warm));

    /* Initialize L2 software */
    SHR_IF_ERR_EXIT
        (bcmcth_l2_learn_init(unit, warm));

exit:
    if (SHR_FUNC_ERR()) {
        /* Detach driver to clean up if attach fails. */
        bcmcth_l2_detach(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmcth_l2_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_l2_learn_cleanup(unit));

exit:
    SHR_FUNC_EXIT();
}
