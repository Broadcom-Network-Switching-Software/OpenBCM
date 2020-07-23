/*! \file bcmpc_attach.c
 *
 * BCMPC attach APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_types.h>

#include <bsl/bsl.h>
#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlrd/bcmlrd_conf.h>

#include <bcmpc/bcmpc_types_internal.h>
#include <bcmpc/bcmpc_util_internal.h>
#include <bcmpc/bcmpc_sysm.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_INIT

/*! Device specific attach function type. */
typedef int (*bcmpc_drv_attach_f)(int unit);

/*! Array of device specific attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bd##_pc_drv_attach },
static struct {
    bcmpc_drv_attach_f attach;
} list_dev_attach[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

/*! Array of all device variants */
typedef int (*bcmpc_var_drv_attach_f)(int unit);
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    { _bd##_vu##_va##_pc_var_drv_attach},
static struct {
    bcmpc_var_drv_attach_f attach;
} list_variant_attach[] = {
    { 0 },
#include <bcmlrd/chip/bcmlrd_variant.h>
    { 0 }
};


/*******************************************************************************
 * Internal public functions
 */

int
bcmpc_attach(int unit)
{
    bcmdrd_dev_type_t dev_type;
    bcmlrd_variant_t variant;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    SHR_IF_ERR_EXIT
        (list_dev_attach[dev_type].attach(unit));

    variant = bcmlrd_variant_get(unit);
    if (variant != BCMLRD_VARIANT_T_NONE) {
        SHR_IF_ERR_EXIT
            (list_variant_attach[variant].attach(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_detach(int unit)
{
    bcmpc_drv_t *pc_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    if (pc_drv && pc_drv->dev_cleanup) {
        SHR_IF_ERR_EXIT
            (pc_drv->dev_cleanup(unit));
    }

    bcmpc_drv_set(unit, NULL);

exit:
    SHR_FUNC_EXIT();
}
