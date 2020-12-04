/*! \file bcmbd_cmicd_mcs.c
 *
 * CMICd MCS routines
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
#include <bcmbd/bcmbd_cmicd_intr.h>
#include <bcmbd/bcmbd_cmicd_acc.h>

#include <bcmbd/bcmbd_mcs.h>
#include <bcmbd/bcmbd_mcs_internal.h>
#include <bcmbd/bcmbd_cmicd_mcs.h>

#define BSL_LOG_MODULE BSL_LS_BCMBD_DEV

/* Generic Definitions for MCS Regs */

#define CMICD_SW_INTR_NUM_HOSTS (4)
#define CMICD_MCS_INTR(_host_num) (CMICD_IRQ_SW_INTR0 + (_host_num))

/* CMC0 is used by PCI, CMC1 is used by uC0 and CMC2 by uC1 */
#define CMICD_CMC_UC(_uc) ((_uc) + 1)

/*******************************************************************************
 * Private functions
 */

static void
cmicd_mcs_isr(int unit, uint32_t hostnum)
{
    CMIC_CMC_SW_INTR_CONFIGr_t swi;
    int cmc = 0;

    /* Clear the interrupt source */
    CMIC_CMC_SW_INTR_CONFIGr_SET(swi, hostnum);
    WRITE_CMIC_CMC_SW_INTR_CONFIGr(unit, cmc, swi);

    bcmbd_mcs_isr(unit, hostnum);
}

static uint32_t
cmicd_mcs_to_pci_addr(int unit, int uc, uint32_t mcs_addr)
{
    return mcs_addr;
}

static int
cmicd_mcs_uc_reset(int unit, int uc)
{
    return SHR_E_NONE;
}

static int
cmicd_mcs_uc_cfg_set(int unit, int uc, bcmbd_ukernel_config_t *cfg)
{
    return SHR_E_NONE;
}

static int
cmicd_mcs_uc_cfg_get(int unit, int uc, bcmbd_ukernel_config_t *cfg)
{
    return SHR_E_NONE;
}

static int
cmicd_mcs_uc_dbg_get(int unit, int uc, mcs_debug_extn_t *dbg)
{
    return SHR_E_NONE;
}

static int
cmicd_mcs_uc_start(int unit, int uc)
{
    return SHR_E_NONE;
}

static int
cmicd_mcs_uc_is_reset(int unit, int uc, int *isrst)
{
    *isrst = 0;
    return SHR_E_NONE;
}

static int
cmicd_mcs_intr_enable(int unit, int hostnum)
{
    bcmbd_cmicd_intr_enable(unit, -1, CMICD_MCS_INTR(hostnum));

    return SHR_E_NONE;
}

static int
cmicd_mcs_intr_disable(int unit, int hostnum)
{
    bcmbd_cmicd_intr_disable(unit, -1, CMICD_MCS_INTR(hostnum));

    return SHR_E_NONE;
}

static int
cmicd_mcs_intr_set(int unit, int uc)
{
    CMIC_CMC_SW_INTR_CONFIGr_t cfg;

    CMIC_CMC_SW_INTR_CONFIGr_CLR(cfg);
    CMIC_CMC_SW_INTR_CONFIGr_SW_INTR_STAT_BIT_VALUEf_SET(cfg,1);

    WRITE_CMIC_CMC_SW_INTR_CONFIGr(unit, CMICD_CMC_UC(uc), cfg);

    return SHR_E_NONE;
}

static const mcs_drv_t cmicd_mcs_drv = {
    .mcs_to_pci_addr = cmicd_mcs_to_pci_addr,
    .mcs_uc_reset = cmicd_mcs_uc_reset,
    .mcs_uc_cfg_set = cmicd_mcs_uc_cfg_set,
    .mcs_uc_cfg_get = cmicd_mcs_uc_cfg_get,
    .mcs_uc_dbg_get = cmicd_mcs_uc_dbg_get,
    .mcs_uc_start = cmicd_mcs_uc_start,
    .mcs_uc_is_reset = cmicd_mcs_uc_is_reset,
    .mcs_intr_enable = cmicd_mcs_intr_enable,
    .mcs_intr_disable = cmicd_mcs_intr_disable,
    .mcs_intr_set = cmicd_mcs_intr_set,
};

/*******************************************************************************
 * Public functions
 */

const mcs_drv_t *
bcmbd_cmicd_mcs_drv_get(int unit)
{
    return &cmicd_mcs_drv;
}

int
bcmbd_cmicd_mcs_init(int unit)
{
    int hostnum;

    SHR_FUNC_ENTER(unit);

    for (hostnum = 0; hostnum < CMICD_SW_INTR_NUM_HOSTS; hostnum++) {
        bcmbd_cmicd_intr_func_set(unit, -1, CMICD_MCS_INTR(hostnum),
                                  cmicd_mcs_isr, hostnum);
    }

    SHR_FUNC_EXIT();
}
