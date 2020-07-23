/*! \file bcm56880_a0_bd_mcs.c
 *
 * MCS routines for BCM56880_A0
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_sleep.h>
#include <bcmbd/bcmbd_mcs.h>
#include <bcmbd/bcmbd_mcs_internal.h>
#include <bcmbd/bcmbd_cmicx_mcs.h>
#include <bcmbd/bcmbd_sw_intr.h>
#include <bcmbd/bcmbd_cmicx2_sw_intr.h>
#include <bcmdrd/chip/bcm56880_a0_cmic_defs.h>
#include <bcmbd/chip/bcm56880_a0_cmic_acc.h>

#include "bcm56880_a0_drv.h"

/*******************************************************************************
 * Local definitions
 */

/* Number of embedded micro controllers */
#define NUM_UC          4

/* Shared memory area for exchanging CPU messages */
#define MSG_AREAS       {0x50820000, 0x50880000, 0x51820000, 0x51880000}

/* PCI addresses of MCS memories */

/* ATCM and BTCM are reserved 256K each in mHost
 * but as only 128K of ATCM is implemented, only
 * 384K is mapped in the PCI
 */
#define MHOST_ATCM_SIZE     (256 * 1024)
#define MHOST_BTCM_SIZE     (256 * 1024)
#define MHOST_TCM_SIZE      (MHOST_ATCM_SIZE + MHOST_BTCM_SIZE)

#define PCI_ATCM_SIZE       (128 * 1024)
#define PCI_BTCM_SIZE       (256 * 1024)
#define PCI_TCM_SIZE        (PCI_ATCM_SIZE + PCI_BTCM_SIZE)

#define RTS0_PCI_TCM_BASE   (0x50800000)
#define RTS1_PCI_TCM_BASE   (0x51800000)

/* 2MB of SRAM per RTS. */
#define MHOST_SRAM_BASE     (0x900000)
#define MHOST_SRAM_SIZE     (2 * 1024 * 1024)

#define RTS0_PCI_SRAM_BASE  (0x50900000)
#define RTS1_PCI_SRAM_BASE  (0x51900000)

#define NUM_SBUSDMA_CHAN    8

#define NUM_PKTDMA_CHAN     8

#define NUM_SCHANFIFO_CHAN  4

#define FLOW_DB_ENTRY_SIZE          20 /* Size of flow table in Ukernel */
#define FLOW_START_TIME_STAMP       8  /* Size of flow start timestamp in Ukernel */
#define EFLOW_DB_ENTRY_SIZE         16 /* Size of elephant flow table in Ukernel */
#define MAX_NUM_FLOWS_HW_LEARN      128 * 1024
#define MAX_NUM_FLOWS_SW_LEARN      512 * 1024
#define BUFFER_MEMORY               32 * 1024  

#define APP_RESOURCES { \
    {MCS_APP_UNKNOWN, 0, (1024 * 1024), 0, 2, 2, 0}, \
    {MCS_APP_NOAPP, 0, (1024 * 1024), 0, 2, 2, 0}, \
    {MCS_APP_TXBCN, 0, (1024 * 1024), 0, 2, 2, 0}, \
    {MCS_APP_BFD, 0, (1024 * 1024), 0, 2, 2, 0}, \
    {MCS_APP_FT, (((FLOW_DB_ENTRY_SIZE + FLOW_START_TIME_STAMP /*+ EFLOW_DB_ENTRY_SIZE*/) * MAX_NUM_FLOWS_SW_LEARN) + BUFFER_MEMORY),  \
                  (1024 * 1024), 0, 2, 2, 1}, \
    {MCS_APP_BS, 0, (1024 * 1024), 0, 2, 2, 0}, \
    {MCS_APP_ST, 0, (1024 * 1024), 0, 2, 2, 0}, \
    {MCS_APP_IFA, 0, (1024 * 1024), 0, 2, 2, 0}, \
    {MCS_APP_IFA2, 0, (1024 * 1024), (512 * 1024), 2, 2, 0},  \
    {MCS_APP_QCM, 0, (1024 * 1024), 0, 2, 2, 0}, \
    {MCS_APP_INT, 0, (1024 * 1024), 0, 2, 2, 0}, \
    {MCS_APP_SUM, 0, (1024 * 1024), 0, 2, 2, 0}, \
    {MCS_APP_IMS, 0, (1024 * 1024), 0, 2, 2, 0}, \
    {MCS_APP_TDPLL, 0, (1024 * 1024), 0, 2, 2, 0}, \
    {MCS_APP_PTPPP, 0, (1024 * 1024), 0, 2, 2, 0}, \
    {MCS_APP_PTPM, 0, (1024 * 1024), 0, 2, 2, 0}, \
    {MCS_APP_PTPFULL, 0, (1024 * 1024), 0, 2, 2, 0}, \
    {MCS_APP_KNETSYNC, 0, (1024 * 1024), 0, 2, 2, 0}, \
    {MCS_APP_MOD, 0, (1024 * 1024), 0, 2, 2, 0} \
}

typedef union MHOST_x_CR5_RST_CTRLr_s {
    RTS0_MHOST_0_CR5_RST_CTRLr_t ctl0;
    RTS0_MHOST_1_CR5_RST_CTRLr_t ctl1;
    RTS1_MHOST_0_CR5_RST_CTRLr_t ctl2;
    RTS1_MHOST_1_CR5_RST_CTRLr_t ctl3;
} MHOST_x_CR5_RST_CTRLr_t;

typedef union MHOST_x_SW_PROG_INTR_SETr_s {
    RTS0_MHOST_0_SW_PROG_INTR_SETr_t mh0;
    RTS0_MHOST_1_SW_PROG_INTR_SETr_t mh1;
    RTS1_MHOST_0_SW_PROG_INTR_SETr_t mh2;
    RTS1_MHOST_1_SW_PROG_INTR_SETr_t mh3;
} MHOST_x_SW_PROG_INTR_SETr_t;

#define READ_RTSX_MHOST_X_CR5_RST_CTRLr(u, r, uc) { \
        if (uc == 0) { \
            READ_RTS0_MHOST_0_CR5_RST_CTRLr(u, r.ctl0); \
        } else if (uc == 1) { \
            READ_RTS0_MHOST_1_CR5_RST_CTRLr(u, r.ctl1); \
        } else if (uc == 2) { \
            READ_RTS1_MHOST_0_CR5_RST_CTRLr(u, r.ctl2); \
        } else { \
            READ_RTS1_MHOST_1_CR5_RST_CTRLr(u, r.ctl3); \
        } \
}

#define WRITE_RTSX_MHOST_X_CR5_RST_CTRLr(u, r, uc) { \
        if (uc == 0) { \
            WRITE_RTS0_MHOST_0_CR5_RST_CTRLr(u, r.ctl0); \
        } else if (uc == 1) { \
            WRITE_RTS0_MHOST_1_CR5_RST_CTRLr(u, r.ctl1); \
        } else if (uc == 2) { \
            WRITE_RTS1_MHOST_0_CR5_RST_CTRLr(u, r.ctl2); \
        } else { \
            WRITE_RTS1_MHOST_1_CR5_RST_CTRLr(u, r.ctl3); \
        } \
}

/*******************************************************************************
 * Local data
 */

/* Device-specific MCS driver */
static mcs_drv_t mcs_drv[BCMDRD_CONFIG_MAX_UNITS];

static const bcmbd_mcs_res_req_t
    bcm56880_a0_mcs_app_resources[MCS_APP_COUNT] = APP_RESOURCES;

static bcmbd_ukernel_config_t
    bcm56880_a0_mcs_resources_used[BCMDRD_CONFIG_MAX_UNITS][NUM_UC];

static uint32_t bcm56880_a0_mcs_cores_used[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */
static uint32_t
bcm56880_a0_mcs_to_pci_addr(int unit, int uc, uint32_t mcs_addr)
{
    uint32_t tcm_base, sram_base;

    if (uc < 2) { /* RTS0 */
        tcm_base = RTS0_PCI_TCM_BASE + (PCI_TCM_SIZE * uc);
        sram_base = RTS0_PCI_SRAM_BASE;
    } else { /* RTS1 */
        tcm_base = RTS1_PCI_TCM_BASE + (PCI_TCM_SIZE * (uc - 2));
        sram_base = RTS1_PCI_SRAM_BASE;
    }

    if (mcs_addr < MHOST_ATCM_SIZE) { /* ATCM */
        return tcm_base + mcs_addr;
    } else if (mcs_addr < MHOST_TCM_SIZE) { /* BTCM */
        return tcm_base + mcs_addr - MHOST_ATCM_SIZE + PCI_ATCM_SIZE;
    } else if ((mcs_addr >= MHOST_SRAM_BASE) &&
               (mcs_addr < (MHOST_SRAM_BASE + MHOST_SRAM_SIZE))) { /* SRAM */
        return sram_base + mcs_addr - MHOST_SRAM_BASE;
    }

    return mcs_addr;
}

static uint32_t
mcs_rd(int unit, int uc, uint32_t mcs_addr)
{
    uint32_t pci_addr, val;

    pci_addr = bcm56880_a0_mcs_to_pci_addr(unit, uc, mcs_addr);
    (void)bcmdrd_hal_iproc_read(unit, pci_addr, &val);
    return val;
}

static void
mcs_wr(int unit, int uc, uint32_t mcs_addr, uint32_t val)
{
    uint32_t pci_addr;

    pci_addr = bcm56880_a0_mcs_to_pci_addr(unit, uc, mcs_addr);
    (void)bcmdrd_hal_iproc_write(unit, pci_addr, val);
}

static void bcm56880_mcs_rm_get_all(bcmbd_ukernel_config_t *cfg, int uc)
{
    cfg->uc = -1;
    cfg->hostram_addr = NULL;
    cfg->hostram_paddr = 0U;
    cfg->hostram_size = 0;
    cfg->sram_base = MHOST_SRAM_BASE;
    cfg->sram_size = MHOST_SRAM_SIZE;
    cfg->sbusdma_bmp = ((1 << NUM_SBUSDMA_CHAN) - 1);
    cfg->pktdma_bmp = ((1 << NUM_PKTDMA_CHAN) - 1);
    cfg->schanfifo_bmp = ((1 << NUM_SCHANFIFO_CHAN) - 1);
}

static int
bcm56880_mcs_rm_get_free(int unit, int uc, bcmbd_ukernel_config_t *cfg,
                         uint32_t flags)
{
    int i;
    bcmbd_ukernel_config_t cfree;
    bcmbd_ukernel_config_t *used, *this, *other;
    uint32_t addr1, addr2, size1, size2;

    /* For restricted resources, only consider the other uC in the same RTS */
    if (uc == 0) {
        other = &(bcm56880_a0_mcs_resources_used[unit][1]);
    } else if (uc == 1) {
        other = &(bcm56880_a0_mcs_resources_used[unit][0]);
    } else if (uc == 2) {
        other = &(bcm56880_a0_mcs_resources_used[unit][3]);
    } else if (uc == 3) {
        other = &(bcm56880_a0_mcs_resources_used[unit][2]);
    } else {
        return SHR_E_PARAM;
    }

    bcm56880_mcs_rm_get_all(&cfree, uc);

    if (flags & BCMBD_MCS_RM_FLAG_INCLUDE_SELF) {
        /* Consider the resources used by the current uC as free */

        /* SRAM */
        if (other->sram_size == 0) {
            addr1 = MHOST_SRAM_BASE;
            size1 = MHOST_SRAM_SIZE;
            addr2 = MHOST_SRAM_BASE;
            size2 = MHOST_SRAM_SIZE;
         } else {
            /* Calculate the frags in beginning and end */
            addr1 = MHOST_SRAM_BASE;
            size1 = other->sram_base - MHOST_SRAM_BASE;
            addr2 = other->sram_base + other->sram_size;
            size2 = MHOST_SRAM_BASE + MHOST_SRAM_SIZE - addr2;
        }

         /* Common resources */
         for (i = 0; i < NUM_UC; i++) {
             if (i != uc) {
                 used = &(bcm56880_a0_mcs_resources_used[unit][i]);
                 cfree.sbusdma_bmp &= ~(used->sbusdma_bmp);
                 cfree.pktdma_bmp &= ~(used->pktdma_bmp);
                 cfree.schanfifo_bmp &= ~(used->schanfifo_bmp);
             }
         }
    } else {
        /* Don't consider the resources used by the current uC as free */

        /* Free SRAM calculation:
         *
         * If both uC haven't reserved any SRAM, then all of SRAM is free.
         * If both uC have reserved SRAM, No SRAM is available.
         *     Any unused fragments are ignored
         * If only one uC has reserved SRAM, get the biggest fragment
         *     in the beginning or end of the SRAM.
         */

        this = &(bcm56880_a0_mcs_resources_used[unit][uc]);

        if ((this->sram_size == 0) && (other->sram_size == 0)) {
            /* All free */
            addr1 = MHOST_SRAM_BASE;
            size1 = MHOST_SRAM_SIZE;
            addr2 = MHOST_SRAM_BASE;
            size2 = MHOST_SRAM_SIZE;
        } else if ((this->sram_size > 0) && (other->sram_size > 0)) {
            /* Both are used. Ignore any fragments */
            addr1 = MHOST_SRAM_BASE;
            size1 = 0;
            addr2 = MHOST_SRAM_BASE;
            size2 = 0;
        } else if (this->sram_size == 0) {
            addr1 = MHOST_SRAM_BASE;
            size1 = other->sram_base - MHOST_SRAM_BASE;
            addr2 = other->sram_base + other->sram_size;
            size2 = MHOST_SRAM_BASE + MHOST_SRAM_SIZE - addr2;
        } else if (other->sram_size == 0) {
            addr1 = MHOST_SRAM_BASE;
            size1 = this->sram_base - MHOST_SRAM_BASE;
            addr2 = this->sram_base + this->sram_size;
            size2 = MHOST_SRAM_BASE + MHOST_SRAM_SIZE - addr2;
        }
        /* Common resources */
        for (i = 0; i < NUM_UC; i++) {
            used = &(bcm56880_a0_mcs_resources_used[unit][i]);
            cfree.sbusdma_bmp &= ~(used->sbusdma_bmp);
            cfree.pktdma_bmp &= ~(used->pktdma_bmp);
            cfree.schanfifo_bmp &= ~(used->schanfifo_bmp);
        }
    }

    if (size1 > size2) {
        cfg->sram_base = addr1;
        cfg->sram_size = size1;
    } else {
        cfg->sram_base = addr2;
        cfg->sram_size = size2;
    }
    cfg->sbusdma_bmp = cfree.sbusdma_bmp;
    cfg->pktdma_bmp = cfree.pktdma_bmp;
    cfg->schanfifo_bmp = cfree.schanfifo_bmp;

    return SHR_E_NONE;
}

static int
bcm56880_mcs_find_resources(int unit, int uc,
                            bcmbd_mcs_res_req_t *res,
                            bcmbd_ukernel_config_t *cfg, uint32_t flags)
{
    uint32_t i, j;
    int rv = SHR_E_NONE;
    bcmbd_ukernel_config_t cfree;

    (void)bcm56880_mcs_rm_get_free(unit, uc, &cfree, flags);

    cfg->uc = uc;

    /* Reserve SRAM */
    if (res->sram_size <= cfree.sram_size) {
        cfg->sram_base = cfree.sram_base;
        cfg->sram_size = res->sram_size;
    } else {
        cfg->sram_base = 0;
        cfg->sram_size = 0;
        rv = SHR_E_RESOURCE;
    }

    /* Reserve SBUSDMA */
    j = 0;
    cfg->sbusdma_bmp = 0;
    for (i = 0; i < NUM_SBUSDMA_CHAN; i++) {
        if (j == res->num_sbusdma) {
            break;
        }
        if (cfree.sbusdma_bmp & (1 << i)) {
            cfg->sbusdma_bmp |= (1 << i);
            j++;
        }
    }
    if (j < res->num_sbusdma) {
        rv = SHR_E_RESOURCE;
    }

    /* Reserve PKTDMA */
    j = 0;
    cfg->pktdma_bmp = 0;
    for (i = 0; i < NUM_SBUSDMA_CHAN; i++) {
        if (j == res->num_pktdma) {
            break;
        }
        if (cfree.pktdma_bmp & (1 << i)) {
            cfg->pktdma_bmp |= (1 << i);
            j++;
        }
    }
    if (j < res->num_pktdma) {
        rv = SHR_E_RESOURCE;
    }

    /* Reserve SCHANFIFO */
    j = 0;
    cfg->schanfifo_bmp = 0;
    for (i = 0; i < NUM_SCHANFIFO_CHAN; i++) {
        if (j == res->num_schanfifo) {
            break;
        }
        if (cfree.schanfifo_bmp & (1 << i)) {
            cfg->schanfifo_bmp |= (1 << i);
            j++;
        }
    }
    if (j < res->num_schanfifo) {
        rv = SHR_E_RESOURCE;
    }

    return rv;
}

static int
bcm56880_a0_mcs_uc_reset(int unit, int uc)
{
    MHOST_x_CR5_RST_CTRLr_t ctl;
    RTS0_MHOST_0_MHOST_STRAP_STATUSr_t strap;

    if ((uc < 0) || (uc >= NUM_UC)) {
        return SHR_E_UNAVAIL;
    }

    READ_RTSX_MHOST_X_CR5_RST_CTRLr(unit, &ctl, uc);

    /* Assert reset */
    RTS0_MHOST_0_CR5_RST_CTRLr_SYS_PORESET_Nf_SET(ctl.ctl0,0);
    RTS0_MHOST_0_CR5_RST_CTRLr_RESET_Nf_SET(ctl.ctl0,0);
    RTS0_MHOST_0_CR5_RST_CTRLr_DBG_RESET_Nf_SET(ctl.ctl0,0);
    RTS0_MHOST_0_CR5_RST_CTRLr_PRESET_DBG_Nf_SET(ctl.ctl0,0);
    RTS0_MHOST_0_CR5_RST_CTRLr_CPU_HALT_Nf_SET(ctl.ctl0,0);

    WRITE_RTSX_MHOST_X_CR5_RST_CTRLr(unit, ctl, uc);
    sal_usleep(20000); /* Wait for reset Assertion */

    /* DeAssert reset but keep halted */
    RTS0_MHOST_0_CR5_RST_CTRLr_SYS_PORESET_Nf_SET(ctl.ctl0,1);
    RTS0_MHOST_0_CR5_RST_CTRLr_RESET_Nf_SET(ctl.ctl0,1);
    RTS0_MHOST_0_CR5_RST_CTRLr_DBG_RESET_Nf_SET(ctl.ctl0,1);
    RTS0_MHOST_0_CR5_RST_CTRLr_PRESET_DBG_Nf_SET(ctl.ctl0,1);

    if (uc == 0) {
        /* Check if the processor boots into Flash */
        READ_RTS0_MHOST_0_MHOST_STRAP_STATUSr(unit,&strap);
        if (strap._rts0_mhost_0_mhost_strap_status != 0) {
            /* Unhalt the uC for a while, until the Flash FW
             * finishes its work and gets into the penholder loop
             */
            RTS0_MHOST_0_CR5_RST_CTRLr_CPU_HALT_Nf_SET(ctl.ctl0,1);
            WRITE_RTSX_MHOST_X_CR5_RST_CTRLr(unit, ctl, uc);
            sal_usleep(100000);
            RTS0_MHOST_0_CR5_RST_CTRLr_CPU_HALT_Nf_SET(ctl.ctl0,0);
        }
    }

    WRITE_RTSX_MHOST_X_CR5_RST_CTRLr(unit, ctl, uc);

    return SHR_E_NONE;
}

static int
bcm56880_a0_mcs_uc_cfg_set(int unit, int uc, bcmbd_ukernel_config_t *cfg)
{
    uint32_t val = 0, ver = 0;

    if ((uc < 0) || (uc >= NUM_UC)) {
        return SHR_E_UNAVAIL;
    }

    /* Check if the Firmware has "Configurable" feature */
    val = mcs_rd(unit, uc, MCS_CFG_FEAT_ADDR);
    if (val != MCS_CONFIG_SUPPORTED) {
        return SHR_E_UNAVAIL;
    }

    /* Only configure items that are supported by this FW */
    ver = mcs_rd(unit, uc, MCS_CFG_VER_ADDR);
    if (ver >= 1) {
        /* Configure uC num */
        val = mcs_rd(unit, uc, MCS_CFG_APP_UC_NUM_ADDR);
        val = (val & 0xffff0000) | uc;
        mcs_wr(unit, uc, MCS_CFG_APP_UC_NUM_ADDR, val);

        /* Configure SRAM */
        mcs_wr(unit, uc, MCS_CFG_SRAM_BASE_ADDR,
               cfg->sram_base);
        mcs_wr(unit, uc, MCS_CFG_SRAM_SIZE_ADDR,
               cfg->sram_size);
        mcs_wr(unit, uc, MCS_CFG_UNCACHED_SRAM_SIZE_ADDR,
               cfg->sram_unc_size);

        /* Configure HostRAM */
        mcs_wr(unit, uc, MCS_CFG_HOSTRAM_BASE_MSW_ADDR,
               (uint32_t)(cfg->hostram_paddr >> 32));
        mcs_wr(unit, uc, MCS_CFG_HOSTRAM_BASE_LSW_ADDR,
            (uint32_t)(cfg->hostram_paddr & 0xffffffff));
        mcs_wr(unit, uc, MCS_CFG_HOSTRAM_SIZE_ADDR,
               cfg->hostram_size);

        /* Configure SBUSDMA channels */
        mcs_wr(unit, uc, MCS_CFG_SBUSDMA_BMP_ADDR,
               cfg->sbusdma_bmp);

        /* Configure PKTDMA channels */
        mcs_wr(unit, uc, MCS_CFG_PKTDMA_BMP_ADDR,
               cfg->pktdma_bmp);

        /* Configure SCHANFIFO channels */
        mcs_wr(unit, uc, MCS_CFG_SCHANFIFO_BMP_ADDR,
               cfg->schanfifo_bmp);
    }

    return SHR_E_NONE;
}

static int
bcm56880_a0_mcs_uc_cfg_get(int unit, int uc, bcmbd_ukernel_config_t *cfg)
{
    uint32_t val = 0, ver = 0;

    if ((uc < 0) || (uc >= NUM_UC)) {
        return SHR_E_UNAVAIL;
    }

    /* Check if the Firmware has "Configurable" feature */
    val = mcs_rd(unit, uc, MCS_CFG_FEAT_ADDR);
    if (val != MCS_CONFIG_SUPPORTED) {
        return SHR_E_UNAVAIL;
    }

    /* Only get items that are supported by this FW */
    ver = mcs_rd(unit, uc, MCS_CFG_VER_ADDR);
    if (ver >= 1) {
        /* Get HostRAM Config */
        val = mcs_rd(unit, uc, MCS_CFG_HOSTRAM_BASE_MSW_ADDR);
        cfg->hostram_paddr = ((uint64_t)(val)) << 32;
        cfg->hostram_paddr |=
                mcs_rd(unit, uc, MCS_CFG_HOSTRAM_BASE_LSW_ADDR);
        cfg->hostram_size =
                mcs_rd(unit, uc, MCS_CFG_HOSTRAM_SIZE_ADDR);

        /* Get SRAM Config */
        cfg->sram_base =
                mcs_rd(unit, uc, MCS_CFG_SRAM_BASE_ADDR);
        cfg->sram_size =
                mcs_rd(unit, uc, MCS_CFG_SRAM_SIZE_ADDR);
        cfg->sram_unc_size =
                mcs_rd(unit, uc, MCS_CFG_UNCACHED_SRAM_SIZE_ADDR);

        /* Get SBUSDMA channels Config */
        cfg->sbusdma_bmp =
                mcs_rd(unit, uc, MCS_CFG_SBUSDMA_BMP_ADDR);

        /* Get PKTDMA channels Config */
        cfg->pktdma_bmp =
                mcs_rd(unit, uc, MCS_CFG_PKTDMA_BMP_ADDR);

        /* Get SCHANFIFO channels Config */
        cfg->schanfifo_bmp =
                mcs_rd(unit, uc, MCS_CFG_SCHANFIFO_BMP_ADDR);
    }

    return SHR_E_NONE;
}

static int
bcm56880_a0_mcs_uc_dbg_get(int unit, int uc, mcs_debug_extn_t *dbg)
{
    unsigned int i;
    uint32_t pci_addr = 0;
    uint32_t dbg_base;
    uint32_t *dbg_ptr = (uint32_t *) dbg;

    dbg_base = mcs_rd(unit, uc, MCS_DBG_PTR_ADDR);
    pci_addr = bcm56880_a0_mcs_to_pci_addr(unit, uc, dbg_base);

    for (i = 0; i < sizeof(mcs_debug_extn_t); i += 4, dbg_ptr++) {
        (void)bcmdrd_hal_iproc_read(unit, (pci_addr + i), dbg_ptr);
    }

    return SHR_E_NONE;
}

static int
bcm56880_a0_mcs_uc_start(int unit, int uc)
{
    MHOST_x_CR5_RST_CTRLr_t ctl;

    if ((uc < 0) || (uc >= NUM_UC)) {
        return SHR_E_UNAVAIL;
    }

    /* Bring ARM core out of halt */
    READ_RTSX_MHOST_X_CR5_RST_CTRLr(unit, &ctl, uc);
    RTS0_MHOST_0_CR5_RST_CTRLr_CPU_HALT_Nf_SET(ctl.ctl0,1);
    WRITE_RTSX_MHOST_X_CR5_RST_CTRLr(unit, ctl, uc);

    if (uc == 0) {
        /* Write ATCM address to the PenHolder Location */
        bcmdrd_hal_iproc_write(unit, 0xffff0fe8, 0);
    }
    sal_usleep(10000);

    return SHR_E_NONE;
}

static int
bcm56880_a0_mcs_uc_is_reset(int unit, int uc, int *isrst)
{
    MHOST_x_CR5_RST_CTRLr_t ctl;

    if ((uc < 0) || (uc >= NUM_UC)) {
        return SHR_E_UNAVAIL;
    }

    if (!isrst) {
        return SHR_E_PARAM;
    }

    READ_RTSX_MHOST_X_CR5_RST_CTRLr(unit, &ctl, uc);
    *isrst = RTS0_MHOST_0_CR5_RST_CTRLr_CPU_HALT_Nf_GET(ctl.ctl0) ? 0 : 1;

    return SHR_E_NONE;
}


static int
bcm56880_a0_mcs_intr_enable(int unit, int uc)
{
    return bcmbd_sw_intr_enable(unit, (CMICX2_SWI_MHOST0 + uc));
}


static int
bcm56880_a0_mcs_intr_disable(int unit, int uc)
{
    return bcmbd_sw_intr_disable(unit, (CMICX2_SWI_MHOST0 + uc));
}

static int
bcm56880_a0_mcs_intr_set(int unit, int uc)
{
    MHOST_x_SW_PROG_INTR_SETr_t mh_set;
    uint32_t intr = (1 << CMICX2_SWI_PCIE);
    int rv = SHR_E_NONE;

    switch (uc) {
        case 0:
            RTS0_MHOST_0_SW_PROG_INTR_SETr_VALf_SET(mh_set.mh0, intr);
            WRITE_RTS0_MHOST_0_SW_PROG_INTR_SETr(unit, mh_set.mh0);
            break;
        case 1:
            RTS0_MHOST_1_SW_PROG_INTR_SETr_VALf_SET(mh_set.mh1, intr);
            WRITE_RTS0_MHOST_1_SW_PROG_INTR_SETr(unit, mh_set.mh1);
            break;
        case 2:
            RTS1_MHOST_0_SW_PROG_INTR_SETr_VALf_SET(mh_set.mh2, intr);
            WRITE_RTS1_MHOST_0_SW_PROG_INTR_SETr(unit, mh_set.mh2);
            break;
        case 3:
            RTS1_MHOST_1_SW_PROG_INTR_SETr_VALf_SET(mh_set.mh3, intr);
            WRITE_RTS1_MHOST_1_SW_PROG_INTR_SETr(unit, mh_set.mh3);
            break;
        default:
            rv = SHR_E_UNAVAIL;
            break;
    }

    return rv;
}

static int
bcm56880_a0_mcs_rm_get_resources(int unit, int uc, int type, bcmbd_ukernel_config_t *cfg,
                            uint32_t flags)
{
    bcmbd_ukernel_config_t *used;

    if (type == BCMBD_MCS_RM_RESOURCES_FREE) {
        return bcm56880_mcs_rm_get_free(unit, uc, cfg, flags);
    } else if (type == BCMBD_MCS_RM_RESOURCES_USED) {
        used = &(bcm56880_a0_mcs_resources_used[unit][uc]);
        sal_memcpy(cfg, used, sizeof(bcmbd_ukernel_config_t));
        return SHR_E_NONE;
    } else if (type == BCMBD_MCS_RM_RESOURCES_ALL) {
        bcm56880_mcs_rm_get_all(cfg, uc);
    }
    return SHR_E_NONE;
}

static int
bcm56880_a0_mcs_rm_reserve(int unit, int uc, bcmbd_mcs_res_req_t *req,
                           bcmbd_ukernel_config_t *cfg, uint32_t flags)
{
    int i, rv = SHR_E_RESOURCE;
    bcmbd_mcs_res_req_t res;
    bcmbd_ukernel_config_t *used;
    void *addr1, *addr2;
    uint64_t paddr1, paddr2, eaddr;

    if (flags & BCMBD_MCS_RM_FLAG_AUTO_CONFIG) {
        if (req->app < MCS_APP_COUNT) {
            sal_memcpy(&res, &bcm56880_a0_mcs_app_resources[req->app],
                       sizeof(bcmbd_mcs_res_req_t));
        } else {
            sal_memcpy(&res, &bcm56880_a0_mcs_app_resources[MCS_APP_UNKNOWN],
                       sizeof(bcmbd_mcs_res_req_t));
        }
    } else {
        sal_memcpy(&res, req, sizeof(bcmbd_mcs_res_req_t));
    }

    if (flags & BCMBD_MCS_RM_FLAG_AUTO_CORE) {
        for (i = 0; i < NUM_UC; i++) {
            if ((bcm56880_a0_mcs_cores_used[unit] & (1 << i)) == 0) {
                /* uC is free. Check for resource availability */
                rv = bcm56880_mcs_find_resources(unit, i, &res, cfg, flags);
                if (rv == SHR_E_NONE) {
                    /* Success */
                    break;
                }
            }
        }
    } else {
        rv = bcm56880_mcs_find_resources(unit, uc, &res, cfg, flags);
    }

    if (rv != SHR_E_NONE) {
        cfg->uc = -1;
        return rv;
    }

    cfg->sram_unc_size = res.sram_unc_size;
    /* Uncached SRAM size must not exceed SRAM size */
    if (cfg->sram_unc_size > cfg->sram_size) {
        cfg->sram_unc_size = cfg->sram_size;
    }

    /* Reserve HostRAM */
    cfg->hostram_addr = NULL;
    cfg->hostram_paddr = 0;
    cfg->hostram_size = 0;

    used = &(bcm56880_a0_mcs_resources_used[unit][cfg->uc]);

    if (res.hostram_size == used->hostram_size) {
        /* Just reuse the already allocated HostRAM */
        cfg->hostram_addr = used->hostram_addr;
        cfg->hostram_paddr = used->hostram_paddr;
        cfg->hostram_size = used->hostram_size;
    } else {
        /* De-Allocate Already used HostRAM */
        if (used->hostram_size != 0) {
            (void)bcmdrd_hal_dma_free(unit, used->hostram_size,
                                      used->hostram_addr, used->hostram_paddr);
            used->hostram_size = 0;
            used->hostram_addr = NULL;
            used->hostram_paddr = 0;
        }
        /* Allocate if requested */
        if (res.hostram_size > 0) {
            addr1 = bcmdrd_hal_dma_alloc(unit, res.hostram_size,
                                        "bcmbdMcsHostRamDma1", &paddr1);
            if (addr1 != NULL) {
                eaddr = paddr1 + res.hostram_size;
                if ((eaddr >> 24) == (paddr1 >> 24)) {
                    /* Falls in the same 16M page */
                    cfg->hostram_addr = addr1;
                    cfg->hostram_paddr = paddr1;
                } else {
                    /* The buffer did not fall into the same 16M page
                     * Release it, and reallocate a smaller buffer, so
                     * that the next allocation will start near a 16MB
                     * boundary
                     */
                    eaddr = (paddr1 & 0xff000000) + 0x01000000;
                    i = eaddr - paddr1; /* smaller buffer size*/
                    (void)bcmdrd_hal_dma_free(unit, res.hostram_size,
                                              addr1, paddr1);
                    addr1 = bcmdrd_hal_dma_alloc(unit, i,
                                                "bcmbdMcsHostRamDma1", &paddr1);
                    addr2 = bcmdrd_hal_dma_alloc(unit, res.hostram_size,
                                                "bcmbdMcsHostRamDma2", &paddr2);
                    if (addr1 != NULL) {
                        (void)bcmdrd_hal_dma_free(unit, i, addr1, paddr1);
                    }
                    if (addr2 != NULL) {
                        eaddr = paddr2 + res.hostram_size;
                        if ((eaddr >> 24) == (paddr2 >> 24)) {
                            /* Falls in the same 16M page */
                            cfg->hostram_addr = addr2;
                            cfg->hostram_paddr = paddr2;
                        } else {
                            /* Not usable. Give up. */
                            (void)bcmdrd_hal_dma_free(unit, res.hostram_size,
                                                      addr2, paddr2);
                        }
                    }
                }
            }
            if (cfg->hostram_addr == NULL) {
                cfg->uc = -1;
                return SHR_E_RESOURCE;
            }
            cfg->hostram_size = res.hostram_size;
        }
    }
    /* Commit the reserve */
    bcm56880_a0_mcs_cores_used[unit] |= (1 << cfg->uc);
    sal_memcpy(&bcm56880_a0_mcs_resources_used[unit][cfg->uc],
               cfg, sizeof(bcmbd_ukernel_config_t));

    return rv;
}

static int
bcm56880_a0_mcs_rm_release(int unit, int uc)
{
    int rv = SHR_E_NONE;
    bcmbd_ukernel_config_t *used;

    used = &(bcm56880_a0_mcs_resources_used[unit][uc]);

    /* Free up any hostRAM */
    if (used->hostram_size != 0) {
         (void)bcmdrd_hal_dma_free(unit, used->hostram_size,
                                   used->hostram_addr, used->hostram_paddr);
     }

    sal_memset(used, 0, sizeof(bcmbd_ukernel_config_t));
    bcm56880_a0_mcs_cores_used[unit] &= ~(1 << uc);

    return rv;
}

/*******************************************************************************
 * Public functions
 */

int
bcm56880_a0_bd_mcs_drv_init(int unit)
{
    int rv;
    const mcs_drv_t *cmicx_drv;
    mcs_drv_t *drv;
    uint32_t msg_area[NUM_UC] = MSG_AREAS;
    uint32_t uc;

    /* Start with default CMICx driver */
    drv = &mcs_drv[unit];
    cmicx_drv = bcmbd_cmicx_mcs_drv_get(unit);
    sal_memcpy(drv, cmicx_drv, sizeof(*drv));

    /* Install device-specific overrides */
    drv->mcs_to_pci_addr = bcm56880_a0_mcs_to_pci_addr;
    drv->mcs_uc_reset = bcm56880_a0_mcs_uc_reset;
    drv->mcs_uc_cfg_set = bcm56880_a0_mcs_uc_cfg_set;
    drv->mcs_uc_cfg_get = bcm56880_a0_mcs_uc_cfg_get;
    drv->mcs_uc_dbg_get = bcm56880_a0_mcs_uc_dbg_get;
    drv->mcs_uc_start = bcm56880_a0_mcs_uc_start;
    drv->mcs_uc_is_reset = bcm56880_a0_mcs_uc_is_reset;
    drv->mcs_intr_enable = bcm56880_a0_mcs_intr_enable;
    drv->mcs_intr_disable = bcm56880_a0_mcs_intr_disable;
    drv->mcs_intr_set = bcm56880_a0_mcs_intr_set;
    drv->mcs_rm_reserve = bcm56880_a0_mcs_rm_reserve;
    drv->mcs_rm_release = bcm56880_a0_mcs_rm_release;
    drv->mcs_rm_get_resources= bcm56880_a0_mcs_rm_get_resources;

    rv = bcmbd_mcs_drv_init(unit, drv);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Initialize Resource manager */
    bcm56880_a0_mcs_cores_used[unit] = 0;
    for (uc = 0; uc < NUM_UC; uc++) {
        sal_memset(&(bcm56880_a0_mcs_resources_used[unit][uc]), 0,
                   sizeof(bcmbd_ukernel_config_t));
        
    }

    rv = bcmbd_mcs_init(unit, NUM_UC, &msg_area[0]);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    for (uc = 0; uc < NUM_UC; uc++) {
        rv = bcmbd_sw_intr_func_set(unit, (CMICX2_SWI_MHOST0 + uc),
                                    bcmbd_mcs_isr, uc);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
    }

    return SHR_E_NONE;
}

int
bcm56880_a0_bd_mcs_drv_cleanup(int unit)
{
    
    /* If necessary, implement bcmbd_mcs_drv_cleanup() */
    return bcmbd_mcs_cleanup(unit);
}
