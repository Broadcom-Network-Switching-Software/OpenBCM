/*! \file bcmbd_cmicx_mcs.c
 *
 * CMICx MCS routines
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
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmbd/bcmbd_sw_intr.h>
#include <bcmbd/bcmbd_cmicx_sw_intr.h>
#include <bcmbd/bcmbd_cmicx_acc.h>

#include <bcmbd/bcmbd_mcs.h>
#include <bcmbd/bcmbd_mcs_internal.h>
#include <bcmbd/bcmbd_cmicx_mcs.h>

#define BSL_LOG_MODULE BSL_LS_BCMBD_DEV

/*******************************************************************************
 * Private functions
 */

static uint32_t
cmicx_mcs_to_pci_addr(int unit, int uc, uint32_t mcs_addr)
{
    return mcs_addr;
}

static int
cmicx_mcs_uc_reset(int unit, int uc)
{
    return SHR_E_NONE;
}

static int
cmicx_mcs_uc_cfg_set(int unit, int uc, bcmbd_ukernel_config_t *cfg)
{
    return SHR_E_NONE;
}

static int
cmicx_mcs_uc_cfg_get(int unit, int uc, bcmbd_ukernel_config_t *cfg)
{
    return SHR_E_NONE;
}

static int
cmicx_mcs_uc_dbg_get(int unit, int uc, mcs_debug_extn_t *dbg)
{
    return SHR_E_NONE;
}

static int
cmicx_mcs_uc_start(int unit, int uc)
{
    return SHR_E_NONE;
}

static int
cmicx_mcs_uc_is_reset(int unit, int uc, int *isrst)
{
    *isrst = 0;
    return SHR_E_NONE;
}

static int
cmicx_mcs_intr_enable(int unit, int uc)
{
    return SHR_E_NONE;
}

static int
cmicx_mcs_intr_disable(int unit, int uc)
{
    return SHR_E_NONE;
}

static int
cmicx_mcs_intr_set(int unit, int uc)
{
    return SHR_E_NONE;
}

static const mcs_drv_t cmicx_mcs_drv = {
    .mcs_to_pci_addr = cmicx_mcs_to_pci_addr,
    .mcs_uc_reset = cmicx_mcs_uc_reset,
    .mcs_uc_cfg_set = cmicx_mcs_uc_cfg_set,
    .mcs_uc_cfg_get = cmicx_mcs_uc_cfg_get,
    .mcs_uc_dbg_get = cmicx_mcs_uc_dbg_get,
    .mcs_uc_start = cmicx_mcs_uc_start,
    .mcs_uc_is_reset = cmicx_mcs_uc_is_reset,
    .mcs_intr_enable = cmicx_mcs_intr_enable,
    .mcs_intr_disable = cmicx_mcs_intr_disable,
    .mcs_intr_set = cmicx_mcs_intr_set,
};

/*******************************************************************************
 * Public functions
 */

const mcs_drv_t *
bcmbd_cmicx_mcs_drv_get(int unit)
{
    return &cmicx_mcs_drv;
}

int
bcmbd_cmicx_mcs_init(int unit)
{
    return SHR_E_NONE;
}
