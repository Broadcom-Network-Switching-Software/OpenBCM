/*! \file bcmbd_mcs.c
 *
 * MCS driver APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <sal_config.h>
#include <sal/sal_types.h>
#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <sal/sal_sem.h>
#include <sal/sal_sleep.h>
#include <sal/sal_spinlock.h>
#include <sal/sal_thread.h>
#include <sal/sal_time.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_devlist.h>

#include <bcmbd/bcmbd_mcs.h>
#include <bcmbd/bcmbd_mcs_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMBD_DEV

/*******************************************************************************
 * Local data
 */

static const mcs_drv_t *mcs_drv[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Public functions
 */

int
bcmbd_mcs_drv_init(int unit, const mcs_drv_t *drv)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    mcs_drv[unit] = drv;

    return SHR_E_NONE;
}

uint32_t
bcmbd_mcs_to_pci_addr(int unit, int uc, uint32_t mcs_addr)
{
    const mcs_drv_t *drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }
    drv = mcs_drv[unit];

    if (drv && drv->mcs_to_pci_addr) {
        return drv->mcs_to_pci_addr(unit, uc, mcs_addr);
    }

    return SHR_E_UNAVAIL;
}

int
bcmbd_mcs_uc_reset(int unit, int uc)
{
    const mcs_drv_t *drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }
    drv = mcs_drv[unit];

    if (drv && drv->mcs_uc_reset) {
        return drv->mcs_uc_reset(unit, uc);
    }

    return SHR_E_UNAVAIL;
}

int
bcmbd_mcs_uc_cfg_set(int unit, int uc, bcmbd_ukernel_config_t *cfg)
{
    const mcs_drv_t *drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }
    drv = mcs_drv[unit];

    if (drv && drv->mcs_uc_cfg_set) {
        return drv->mcs_uc_cfg_set(unit, uc, cfg);
    }

    return SHR_E_UNAVAIL;
}

int
bcmbd_mcs_uc_cfg_get(int unit, int uc, bcmbd_ukernel_config_t *cfg)
{
    const mcs_drv_t *drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }
    drv = mcs_drv[unit];

    if (drv && drv->mcs_uc_cfg_get) {
        return drv->mcs_uc_cfg_get(unit, uc, cfg);
    }

    return SHR_E_UNAVAIL;
}

int
bcmbd_mcs_uc_dbg_get(int unit, int uc, mcs_debug_extn_t *dbg)
{
    const mcs_drv_t *drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }
    drv = mcs_drv[unit];

    if (drv && drv->mcs_uc_dbg_get) {
        return drv->mcs_uc_dbg_get(unit, uc, dbg);
    }

    return SHR_E_UNAVAIL;
}

int
bcmbd_mcs_uc_start(int unit, int uc)
{
    const mcs_drv_t *drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }
    drv = mcs_drv[unit];

    if (drv && drv->mcs_uc_start) {
        return drv->mcs_uc_start(unit, uc);
    }

    return SHR_E_UNAVAIL;
}

int
bcmbd_mcs_uc_is_reset(int unit, int uc, int *isrst)
{
    const mcs_drv_t *drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }
    drv = mcs_drv[unit];

    if (drv && drv->mcs_uc_is_reset) {
        return drv->mcs_uc_is_reset(unit, uc, isrst);
    }

    return SHR_E_UNAVAIL;
}

int
bcmbd_mcs_intr_enable(int unit, int uc)
{
    const mcs_drv_t *drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }
    drv = mcs_drv[unit];

    if (drv && drv->mcs_intr_enable) {
        return drv->mcs_intr_enable(unit, uc);
    }

    return SHR_E_UNAVAIL;
}

int
bcmbd_mcs_intr_disable(int unit, int uc)
{
    const mcs_drv_t *drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }
    drv = mcs_drv[unit];

    if (drv && drv->mcs_intr_disable) {
        return drv->mcs_intr_disable(unit, uc);
    }

    return SHR_E_UNAVAIL;
}

int
bcmbd_mcs_intr_set(int unit, int uc)
{
    const mcs_drv_t *drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }
    drv = mcs_drv[unit];

    if (drv && drv->mcs_intr_set) {
        return drv->mcs_intr_set(unit, uc);
    }

    return SHR_E_UNAVAIL;
}

int
bcmbd_mcs_rm_reserve(int unit, int uc, bcmbd_mcs_res_req_t *req,
                     bcmbd_ukernel_config_t *cfg, uint32_t flags)
{
    const mcs_drv_t *drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }
    drv = mcs_drv[unit];

    if (drv && drv->mcs_rm_reserve) {
        return drv->mcs_rm_reserve(unit, uc, req, cfg, flags);
    }

    return SHR_E_UNAVAIL;
}

int
bcmbd_mcs_rm_release(int unit, int uc)
{
    const mcs_drv_t *drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }
    drv = mcs_drv[unit];

    if (drv && drv->mcs_rm_release) {
        return drv->mcs_rm_release(unit, uc);
    }

    return SHR_E_UNAVAIL;
}

int
bcmbd_mcs_rm_get_resources(int unit, int uc, int type, bcmbd_ukernel_config_t *cfg,
                      uint32_t flags)
{
    const mcs_drv_t *drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }
    drv = mcs_drv[unit];

    if (drv && drv->mcs_rm_get_resources) {
        return drv->mcs_rm_get_resources(unit, uc, type, cfg, flags);
    }

    return SHR_E_UNAVAIL;
}