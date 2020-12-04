/*! \file bcmsec_attach.c
 *
 * BCMSEC attach APIs
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

#include <bcmsec/bcmsec_types.h>
#include <bcmsec/generated/bcmsec_ha.h>
#include <bcmsec/bcmsec_drv.h>
#include <bcmsec/bcmsec_sysm.h>
#include <bcmsec/bcmsec_event.h>
#include <bcmsec/imm/bcmsec_imm_defines_internal.h>


/*******************************************************************************
* Local definitions
 */
#define BSL_LOG_MODULE  BSL_LS_BCMSEC_INIT

/*! Device specific attach function type. */
typedef int (*bcmsec_drv_attach_f)(int unit);


/*! Array of device specific attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { bcmsec_##_bd##_drv_attach },
static struct {
    bcmsec_drv_attach_f attach;
} list_dev_attach[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

/*******************************************************************************
* Public functions
 */

int
bcmsec_attach(int unit)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

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
bcmsec_detach(int unit, bool warm)
{
    bcmsec_drv_t *sec_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmsec_drv_get(unit, &sec_drv));

    if (sec_drv && sec_drv->dev_cleanup) {
        SHR_IF_ERR_EXIT
            (sec_drv->dev_cleanup(unit, warm));
    }

    SHR_IF_ERR_EXIT
        (bcmsec_event_cleanup(unit));
    SHR_IF_ERR_EXIT
        (bcmsec_drv_set(unit, NULL));

exit:
    SHR_FUNC_EXIT();
}

int
bcmsec_stop(int unit, bool warm)
{
    bcmsec_drv_t *sec_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmsec_drv_get(unit, &sec_drv));

    if (sec_drv && sec_drv->dev_stop) {
        SHR_IF_ERR_EXIT
            (sec_drv->dev_stop(unit, warm));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmsec_run(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    SHR_FUNC_EXIT();
}

int bcmsec_imm_reg(int unit)
{
    bcmsec_drv_t *sec_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmsec_drv_get(unit, &sec_drv));

    if (sec_drv && sec_drv->imm_init) {
            sec_drv->imm_init(unit);
    }
exit:
    SHR_FUNC_EXIT();
}
