/*! \file bcmtm_attach.c
 *
 * BCMTM attach APIs
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

#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_sysm.h>
#include <bcmtm/ebst/bcmtm_ebst_internal.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>


/*******************************************************************************
* Local definitions
 */
#define BSL_LOG_MODULE  BSL_LS_BCMTM_INIT

/*! Device specific attach function type. */
typedef int (*bcmtm_drv_attach_f)(int unit);


/*! Array of device specific attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { bcmtm_##_bd##_drv_attach },
static struct {
    bcmtm_drv_attach_f attach;
} list_dev_attach[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

typedef int (*bcmtm_var_drv_attach_f)(int unit);
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    { _bd##_vu##_va##_tm_var_drv_attach},
static struct {
    bcmtm_var_drv_attach_f attach;
} list_variant_attach[] = {
    { 0 },
#include <bcmlrd/chip/bcmlrd_variant.h>
    { 0 }
};

/*******************************************************************************
* Public functions
 */

int
bcmtm_attach(int unit)
{
    bcmdrd_dev_type_t dev_type;
    bcmlrd_variant_t variant;

    SHR_FUNC_ENTER(unit);
    variant = bcmlrd_variant_get(unit);
    if (variant != BCMLRD_VARIANT_T_NONE) {
        if (SHR_SUCCESS(list_variant_attach[variant].attach(unit))) {
            SHR_EXIT();
        }
    }

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    SHR_IF_ERR_EXIT
        (list_dev_attach[dev_type].attach(unit));
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_detach(int unit, bool warm)
{
    bcmtm_drv_t *tm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_drv_get(unit, &tm_drv));

    SHR_IF_ERR_EXIT
        (bcmtm_ebst_cleanup(unit));

    SHR_IF_ERR_EXIT
        (bcmtm_pfc_deadlock_monitor_cleanup(unit));

    if (tm_drv && tm_drv->dev_cleanup) {
        SHR_IF_ERR_EXIT
            (tm_drv->dev_cleanup(unit, warm));
    }
    bcmtm_drv_set(unit, NULL);
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_stop(int unit, bool warm)
{
    bcmtm_drv_t *tm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_drv_get(unit, &tm_drv));

    if (tm_drv && tm_drv->dev_stop) {
        SHR_IF_ERR_EXIT
            (tm_drv->dev_stop(unit, warm));
    }

    SHR_IF_ERR_EXIT
        (bcmtm_pfc_deadlock_monitor_stop(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_run(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_pfc_deadlock_monitor_run(unit, warm));
exit:
    SHR_FUNC_EXIT();
}

int bcmtm_imm_reg(int unit)
{
    bcmtm_drv_t *tm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_drv_get(unit, &tm_drv));

    if (tm_drv && tm_drv->imm_init) {
            tm_drv->imm_init(unit);
    }
exit:
    SHR_FUNC_EXIT();
}
