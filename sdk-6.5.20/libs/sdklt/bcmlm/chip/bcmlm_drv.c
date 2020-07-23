/*! \file bcmlm_drv.c
 *
 * Link Manager driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>

#include <sal/sal_libc.h>
#include <sal/sal_types.h>
#include <sal/sal_alloc.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmpc/bcmpc_pmgr.h>
#include <bcmlm/bcmlm_drv_internal.h>
#include <bcmlm/bcmlm_chip_internal.h>

/******************************************************************************
* Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLM_DRV

/* Linkscan driver */
static bcmlm_drv_t *bcmlm_drv[BCMDRD_CONFIG_MAX_UNITS];

/*! Device specific attach function type. */
typedef int (*bcmlm_drv_attach_f)(bcmlm_drv_t *drv);

/*! Array of device specific attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
        { _bc##_lm_drv_attach, _bc##_lm_drv_detach },
static struct {
        bcmlm_drv_attach_f attach;
        bcmlm_drv_attach_f detach;
} list_dev_attach[] = {
        { NULL, NULL }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
        { NULL, NULL } /* end-of-list */
};

#define LM_CALL(_ld, _lf, _la) \
        ((_ld) == 0 ? SHR_E_INIT: \
              ((_ld)->_lf == 0 ? SHR_E_NONE: (_ld)->_lf _la))

#define LINKSCAN_HW_CONFIG(_u, _hw) \
        LM_CALL(bcmlm_drv[_u], hw_config, ((_u), (_hw)))

#define LINKSCAN_HW_LINK_GET(_u, _hw_link) \
        LM_CALL(bcmlm_drv[_u], hw_link_get, ((_u), (_hw_link)))

#define LINKSCAN_HW_INTR_CLEAR(_u) \
        LM_CALL(bcmlm_drv[_u], hw_intr_clear, (_u))

#define LINKSCAN_HW_INTR_CB_SET(_u, _intr_func) \
        LM_CALL(bcmlm_drv[_u], hw_intr_cb_set, ((_u), (_intr_func)))

#define LINKSCAN_HW_SCAN_STOP(_u)    \
        LM_CALL(bcmlm_drv[_u], hw_scan_stop, (_u))

#define LINKSCAN_HW_SCAN_START(_u)    \
        LM_CALL(bcmlm_drv[_u], hw_scan_start, (_u))

#define LINKSCAN_PORT_VALIDATE(_u, _port) \
        LM_CALL(bcmlm_drv[_u], port_validate, ((_u), (_port)))

#define LINKSCAN_FAULT_CHK_ENABLED(_u, _enabled) \
        LM_CALL(bcmlm_drv[_u], fault_check_enabled, ((_u), (_enabled)))

/******************************************************************************
* Private functions
 */

/******************************************************************************
* Public functions
 */
int
bcmlm_drv_attach(int unit)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    if (bcmlm_drv[unit] != NULL) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_ALLOC(bcmlm_drv[unit], sizeof(bcmlm_drv_t), "bcmlmDrv");
    SHR_NULL_CHECK(bcmlm_drv[unit], SHR_E_MEMORY);
    sal_memset(bcmlm_drv[unit], 0, sizeof(bcmlm_drv_t));
    bcmlm_drv[unit]->unit = unit;
    SHR_IF_ERR_EXIT
        (list_dev_attach[dev_type].attach(bcmlm_drv[unit]));

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(bcmlm_drv[unit]);
    }

    SHR_FUNC_EXIT();
}

int
bcmlm_drv_detach(int unit)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    SHR_NULL_CHECK(bcmlm_drv[unit], SHR_E_INTERNAL);

    SHR_IF_ERR_EXIT
        (list_dev_attach[dev_type].detach(bcmlm_drv[unit]));

    SHR_FREE(bcmlm_drv[unit]);

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_dev_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (bcmlm_drv[unit] && bcmlm_drv[unit]->hw_init) {
        SHR_IF_ERR_EXIT
            (bcmlm_drv[unit]->hw_init(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_dev_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (bcmlm_drv[unit] && bcmlm_drv[unit]->hw_cleanup) {
        SHR_IF_ERR_EXIT
            (bcmlm_drv[unit]->hw_cleanup(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_hw_linkscan_config(int unit, bcmdrd_pbmp_t hw)
{
    shr_port_t lport, pport;
    bcmdrd_pbmp_t ppbm;

    SHR_FUNC_ENTER(unit);

    BCMDRD_PBMP_CLEAR(ppbm);
    BCMDRD_PBMP_ITER(hw, lport) {
        pport = bcmpc_lport_to_pport(unit, lport);
        if (pport != BCMPC_INVALID_PPORT) {
            BCMDRD_PBMP_PORT_ADD(ppbm, pport);
        }
    }

    SHR_IF_ERR_EXIT
        (LINKSCAN_HW_SCAN_STOP(unit));

    SHR_IF_ERR_EXIT
        (LINKSCAN_HW_CONFIG(unit, ppbm));

    if (BCMDRD_PBMP_NOT_NULL(ppbm)) {
        SHR_IF_ERR_EXIT
            (LINKSCAN_HW_SCAN_START(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_hw_linkscan_link_get(int unit, bcmdrd_pbmp_t *hw)
{
    shr_port_t lport, pport;
    bcmdrd_pbmp_t ppbm;

    SHR_FUNC_ENTER(unit);

    BCMDRD_PBMP_CLEAR(*hw);
    BCMDRD_PBMP_CLEAR(ppbm);
    SHR_IF_ERR_EXIT
        (LINKSCAN_HW_LINK_GET(unit, &ppbm));

    BCMDRD_PBMP_ITER(ppbm, pport) {
        lport = bcmpc_pport_to_lport(unit, pport);
        if (lport != BCMPC_INVALID_LPORT) {
            BCMDRD_PBMP_PORT_ADD(*hw, lport);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_hw_linkscan_intr_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (LINKSCAN_HW_INTR_CLEAR(unit));
exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_hw_linkscan_intr_cb_set(int unit, bcmbd_intr_f cb)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (LINKSCAN_HW_INTR_CB_SET(unit, cb));

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_linkscan_fault_check_enabled(int unit, int *enabled)
{
    SHR_FUNC_ENTER(unit);

    if (SHR_FAILURE(LINKSCAN_FAULT_CHK_ENABLED(unit, enabled))) {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_linkscan_port_validate(int unit, shr_port_t port)
{
    int pipe;

    SHR_FUNC_ENTER(unit);

    pipe = bcmdrd_dev_logic_port_pipe(unit, port);
    if (pipe < 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Invalid input port %d.\n"), port));
        SHR_ERR_EXIT(SHR_E_PORT);
    }

    if (SHR_FAILURE(LINKSCAN_PORT_VALIDATE(unit, port))) {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

exit:
    SHR_FUNC_EXIT();
}


int
bcmlm_sw_linkscan_link_get(int unit, shr_port_t port, int *link)
{
    shr_port_t pport;

    SHR_FUNC_ENTER(unit);

    pport = bcmpc_lport_to_pport(unit, port);
    if (pport == BCMPC_INVALID_PPORT) {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_link_state_get(unit, pport, link));

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_linkscan_fault_get(int unit, shr_port_t port, int *fault)
{
    shr_port_t pport;

    SHR_FUNC_ENTER(unit);

    pport = bcmpc_lport_to_pport(unit, port);
    if (pport == BCMPC_INVALID_PPORT) {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_fault_state_get(unit, pport, fault));

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_port_update(int unit, shr_port_t port, int link)
{
    shr_port_t pport;

    SHR_FUNC_ENTER(unit);

    pport = bcmpc_lport_to_pport(unit, port);
    if (pport == BCMPC_INVALID_PPORT) {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_port_link_change(unit, pport, link));
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_UP(unit, port,
                           "Notify port %d link change to PC link %d\n"),
               port,link));

exit:
    SHR_FUNC_EXIT();
}
