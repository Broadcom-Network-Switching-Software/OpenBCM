/*! \file bcm56996_a0_bd_port_intr.c
 *
 * The port interrupts are issued per port macro (PM) and different PM
 * types support a different set of interrupts sources, i.e. the
 * CDPORT interrupts sources are different than the XLPORT interrupt
 * sources.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <sal/sal_spinlock.h>
#include <sal/sal_sem.h>
#include <sal/sal_time.h>

#include <shr/shr_debug.h>
#include <shr/shr_thread.h>
#include <shr/shr_bitop.h>

#include <bcmbd/bcmbd_port_intr.h>
#include <bcmbd/bcmbd_cmicx_intr.h>
#include <bcmbd/bcmbd_port_intr_internal.h>
#include <bcmbd/chip/bcm56996_a0_acc.h>
#include <bcmbd/chip/bcm56996_a0_lp_intr.h>
#include <bcmbd/chip/bcm56996_a0_port_intr.h>

#include "bcm56996_a0_drv.h"

#define BSL_LOG_MODULE  BSL_LS_BCMBD_INTR

/*******************************************************************************
 * Local definitions
 */

#define MAX_UNITS       BCMDRD_CONFIG_MAX_UNITS
#define MAX_INTRS       MAX_PORT_INTR
#define CDMAC_INST      4       /* number of physical ports per PM */
#define XLMAC_INST      4
#define CDPORT_INST     64
#define XLPORT_INST     1
#define NUM_PM_INST     (CDPORT_INST + XLPORT_INST)

/*! Get LP interrupt number from PM instance */
#define LP_INTR_NUM(_pm_inst)           \
    ((_pm_inst) + PM0_INTR)

#define CD_INST_TO_PORT(_pm_inst)       \
    (((_pm_inst) * CDMAC_INST) + 1)

#define PORT_TO_CD_INST(_port)          \
    (((_port) - 1) / CDMAC_INST)

typedef struct bcmbd_intr_vect_s {
    bcmbd_intr_f func;
    uint32_t param;
} bcmbd_intr_vect_t;

typedef struct bcmbd_intr_vect_info_s {
    bcmbd_intr_vect_t intr_vects[MAX_INTRS];
} bcmbd_intr_vect_info_t;

typedef struct bcmbd_intr_state_s {
    bool new_intr[NUM_PM_INST];
} bcmbd_intr_state_t;

#define INTR_MASK_LOCK(_u) sal_spinlock_lock(port_intr_lock[_u]);
#define INTR_MASK_UNLOCK(_u) sal_spinlock_unlock(port_intr_lock[_u]);

/*******************************************************************************
 * Local data
 */

static bool port_intr_connected[MAX_UNITS];
static sal_spinlock_t port_intr_lock[MAX_UNITS];
static bcmbd_intr_vect_info_t port_intr_vect_info[MAX_UNITS];
static bcmbd_intr_state_t port_intr_state[MAX_UNITS];
static shr_thread_t port_thread_ctrl[MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

static void
bit_set(uint32_t *bit_mask, int bit_num, int bit_val)
{
    *bit_mask &= ~LSHIFT32(1, bit_num);
    if (bit_val) {
        *bit_mask |= LSHIFT32(1, bit_num);
    }
}

static int
cdport_intr_update(int unit, unsigned int intr_num, int port, int enable)
{
    int ioerr = 0;
    CDPORT_INTR_MASKr_t cdp_mask;
    CDMAC_INTR_ENABLEr_t cdm_en;
    uint32_t pm_inst, inst;
    uint32_t intr_bits;
    uint32_t regval;
    int pm_port;
    int mask;

    pm_inst = PORT_TO_CD_INST(port);
    pm_port = CD_INST_TO_PORT(pm_inst);
    inst = port - pm_port;

    /* mask bit value for CDPORT_INTR_MASK */
    if (enable) {
        mask = 0;
    } else {
        mask = 1;
    }

    ioerr += READ_CDPORT_INTR_MASKr(unit, &cdp_mask, pm_port);

    if (intr_num >= CDMAC_INTR_BASE) {
        /* Use enable bits directly for per-MAC register */
        ioerr += READ_CDMAC_INTR_ENABLEr(unit, port, &cdm_en);
        regval = CDMAC_INTR_ENABLEr_GET(cdm_en);
        if (enable) {
            regval |= LSHIFT32(1, intr_num - CDMAC_INTR_BASE);
        } else {
            regval &= ~LSHIFT32(1, intr_num - CDMAC_INTR_BASE);
        }
        CDMAC_INTR_ENABLEr_SET(cdm_en, regval);
        ioerr += WRITE_CDMAC_INTR_ENABLEr(unit, port, cdm_en);

        /* Enable MAC_INTR interrupts accordingly in parent register */
        intr_bits = CDPORT_INTR_MASKr_CDMAC_INTRf_GET(cdp_mask);
        if (mask == 1 && regval == 0) {
            bit_set(&intr_bits, inst, 1);
        } else {
            bit_set(&intr_bits, inst, 0);
        }
        CDPORT_INTR_MASKr_CDMAC_INTRf_SET(cdp_mask, intr_bits);
    } else if (intr_num == CDPORT_INTR_LINK_DOWN) {
        intr_bits = CDPORT_INTR_MASKr_LINK_DOWNf_GET(cdp_mask);
        bit_set(&intr_bits, inst, mask);
        CDPORT_INTR_MASKr_LINK_DOWNf_SET(cdp_mask, intr_bits);
    } else if (intr_num == CDPORT_INTR_LINK_UP) {
        intr_bits = CDPORT_INTR_MASKr_LINK_UPf_GET(cdp_mask);
        bit_set(&intr_bits, inst, mask);
        CDPORT_INTR_MASKr_LINK_UPf_SET(cdp_mask, intr_bits);
    } else if (intr_num == CDPORT_INTR_FLOWCONTROL_REQ_FULL) {
        intr_bits = CDPORT_INTR_MASKr_FLOWCONTROL_REQ_FULLf_GET(cdp_mask);
        bit_set(&intr_bits, inst, mask);
        CDPORT_INTR_MASKr_FLOWCONTROL_REQ_FULLf_SET(cdp_mask, intr_bits);
    } else if (intr_num == CDPORT_INTR_TSTS_INTERRUPT_STATUS) {
        intr_bits = CDPORT_INTR_MASKr_TSTS_INTERRUPT_STATUSf_GET(cdp_mask);
        bit_set(&intr_bits, inst, mask);
        CDPORT_INTR_MASKr_TSTS_INTERRUPT_STATUSf_SET(cdp_mask, intr_bits);
    } else if (intr_num == CDPORT_INTR_FDR_INTERRUPT) {
        intr_bits = CDPORT_INTR_MASKr_FDR_INTERRUPTf_GET(cdp_mask);
        bit_set(&intr_bits, inst, mask);
        CDPORT_INTR_MASKr_FDR_INTERRUPTf_SET(cdp_mask, intr_bits);
    } else if (intr_num == CDPORT_INTR_LP_PAGE_RDY_SW_AN_INTERRUPT) {
        intr_bits = CDPORT_INTR_MASKr_LP_PAGE_RDY_SW_AN_INTERRUPTf_GET(cdp_mask);
        bit_set(&intr_bits, inst, mask);
        CDPORT_INTR_MASKr_LP_PAGE_RDY_SW_AN_INTERRUPTf_SET(cdp_mask, intr_bits);
    } else if (intr_num == CDPORT_INTR_AN_COMPLETED_SW_AN_INTERRUPT) {
        intr_bits = CDPORT_INTR_MASKr_AN_COMPLETED_SW_AN_INTERRUPTf_GET(cdp_mask);
        bit_set(&intr_bits, inst, mask);
        CDPORT_INTR_MASKr_AN_COMPLETED_SW_AN_INTERRUPTf_SET(cdp_mask, intr_bits);
    } else if (intr_num == CDPORT_INTR_AN_GOOD_CHK_AN_INTERRUPT) {
        intr_bits = CDPORT_INTR_MASKr_AN_GOOD_CHK_AN_INTERRUPTf_GET(cdp_mask);
        bit_set(&intr_bits, inst, mask);
        CDPORT_INTR_MASKr_AN_GOOD_CHK_AN_INTERRUPTf_SET(cdp_mask, intr_bits);
    } else if (intr_num == CDPORT_INTR_TSC_ECC_1B_ERR) {
        /* The 11 interrupt events are enabled and cleared in TSC PCS */
        if (mask) {
            mask = 0x7ff;
        }
        CDPORT_INTR_MASKr_TSC_ECC_1B_ERRf_SET(cdp_mask, mask);
    } else if (intr_num == CDPORT_INTR_TSC_ECC_2B_ERR) {
        /* The 11 interrupt events are enabled and cleared in TSC PCS */
        if (mask) {
            mask = 0x7ff;
        }
        CDPORT_INTR_MASKr_TSC_ECC_2B_ERRf_SET(cdp_mask, mask);
    } else if (intr_num == CDPORT_INTR_PMD_INTR) {
        CDPORT_INTR_MASKr_PMD_INTRf_SET(cdp_mask, mask);
    }

    ioerr += WRITE_CDPORT_INTR_MASKr(unit, cdp_mask, pm_port);

    return ioerr;
}

static int
xlport_intr_update(int unit, unsigned int intr_num, int port, int enable)
{
    int ioerr = 0;
    XLPORT_INTR_ENABLEr_t xlp_en;
    XLMAC_INTR_ENABLEr_t xlm_en;
    MGMT_OBM_INTR_ENABLEr_t obm_en;
    uint32_t pm_inst, inst;
    uint32_t intr_bits;
    uint32_t regval;
    int pm_port;

    pm_inst = PORT_TO_CD_INST(port);
    pm_port = CD_INST_TO_PORT(pm_inst);
    inst = port - pm_port;
    /* Second XLPORT uses lane 2 */
    if (inst == 1) {
        inst = 2;
    }

    ioerr += READ_XLPORT_INTR_ENABLEr(unit, &xlp_en, pm_port);
    if (intr_num >= XLMAC_INTR_BASE) {
        /* Use enable bits directly for per-MAC register */
        ioerr += READ_XLMAC_INTR_ENABLEr(unit, port, &xlm_en);
        regval = XLMAC_INTR_ENABLEr_GET(xlm_en, 0);
        if (enable) {
            regval |= LSHIFT32(1, intr_num - XLMAC_INTR_BASE);
        } else {
            regval &= ~LSHIFT32(1, intr_num - XLMAC_INTR_BASE);
        }
        XLMAC_INTR_ENABLEr_SET(xlm_en, 0, regval);
        ioerr += WRITE_XLMAC_INTR_ENABLEr(unit, port, xlm_en);

        /* Enable MAC_ERR interrupts accordingly in parent register */
        intr_bits = XLPORT_INTR_ENABLEr_MAC_ERRf_GET(xlp_en);
        if (enable == 0 && regval == 0) {
            bit_set(&intr_bits, inst, 0);
        } else {
            bit_set(&intr_bits, inst, 1);
        }
        XLPORT_INTR_ENABLEr_MAC_ERRf_SET(xlp_en, intr_bits);
    } else if (intr_num == XLPORT_INTR_RX_FLOWCONTROL_REQ_FULL) {
        intr_bits = XLPORT_INTR_ENABLEr_RX_FLOWCONTROL_REQ_FULLf_GET(xlp_en);
        bit_set(&intr_bits, inst, enable);
        XLPORT_INTR_ENABLEr_RX_FLOWCONTROL_REQ_FULLf_SET(xlp_en, intr_bits);
    } else if (intr_num == XLPORT_INTR_LINK_DOWN) {
        intr_bits = XLPORT_INTR_ENABLEr_LINK_DOWNf_GET(xlp_en);
        bit_set(&intr_bits, inst, enable);
        XLPORT_INTR_ENABLEr_LINK_DOWNf_SET(xlp_en, intr_bits);
    } else if (intr_num == XLPORT_INTR_MIB_TX_MEM_ERR) {
        XLPORT_INTR_ENABLEr_MIB_TX_MEM_ERRf_SET(xlp_en, enable);
    } else if (intr_num == XLPORT_INTR_MIB_RX_MEM_ERR) {
        XLPORT_INTR_ENABLEr_MIB_RX_MEM_ERRf_SET(xlp_en, enable);
    } else if (intr_num == XLPORT_INTR_TSC_ERR) {
        XLPORT_INTR_ENABLEr_TSC_ERRf_SET(xlp_en, enable);
    } else if (intr_num == XLPORT_INTR_OBM_ECC_ERR) {
        ioerr += READ_MGMT_OBM_INTR_ENABLEr(unit, &obm_en);
        MGMT_OBM_INTR_ENABLEr_OBM_ECC_ERRf_SET(obm_en, enable);
        ioerr += WRITE_MGMT_OBM_INTR_ENABLEr(unit, obm_en);
    }
    ioerr += WRITE_XLPORT_INTR_ENABLEr(unit, xlp_en, pm_port);

    return ioerr;
}

static void
port_intr_update(int unit, unsigned int intr_num, int port, int enable)
{
    int ioerr = 0;

    /* Ensure enable is 0 or 1 when used for field values */
    if (enable) {
        enable = 1;
    }

    if (intr_num >= XLPORT_INTR_BASE) {
        ioerr = xlport_intr_update(unit, intr_num, port, enable);
    } else {
        ioerr = cdport_intr_update(unit, intr_num, port, enable);
    }

    if (ioerr) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Port interrupt %u I/O error\n"),
                   intr_num));
    }
}

static void
port_intr_enable(int unit, unsigned int intr_num)
{
    int port = PORT_INTR_INST_GET(intr_num);
    intr_num = PORT_INTR_NUM_GET(intr_num);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                          "Enable port %d intr %u\n"),
                 port, intr_num));
    if (intr_num >= MAX_INTRS) {
        return;
    }

    /* We may be called by multiple application threads */
    INTR_MASK_LOCK(unit);

    port_intr_update(unit, intr_num, port, 1);

    INTR_MASK_UNLOCK(unit);
}

static void
port_intr_disable(int unit, unsigned int intr_num)
{
    int port = PORT_INTR_INST_GET(intr_num);
    intr_num = PORT_INTR_NUM_GET(intr_num);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                          "Disable port %d intr %u\n"),
                 port, intr_num));
    if (intr_num >= MAX_INTRS) {
        return;
    }

    /* We may be called by multiple application threads */
    INTR_MASK_LOCK(unit);

    port_intr_update(unit, intr_num, port, 0);

    INTR_MASK_UNLOCK(unit);
}

static int
cdport_intr_clear(int unit, unsigned int intr_num, int port)
{
    int ioerr = 0;
    CDPORT_PORT_INTR_STATUSr_t intr_stat;
    CDPORT_PORT_STATUSr_t port_stat;
    uint32_t pm_inst, inst;
    uint32_t intr_bits;
    int pm_port;

    pm_inst = PORT_TO_CD_INST(port);
    pm_port = CD_INST_TO_PORT(pm_inst);
    inst = port - pm_port;
    intr_bits = LSHIFT32(1, inst);

    /* The following interrupts are W1TC (write one to clear) */
    if (intr_num == CDPORT_INTR_LINK_DOWN) {
        CDPORT_PORT_STATUSr_CLR(port_stat);
        CDPORT_PORT_STATUSr_SET(port_stat, intr_bits);
        ioerr += WRITE_CDPORT_PORT_STATUSr(unit, port_stat, pm_port);
    } else if (intr_num == CDPORT_INTR_LINK_UP) {
        CDPORT_PORT_STATUSr_CLR(port_stat);
        CDPORT_PORT_STATUSr_SET(port_stat, (intr_bits<<4));
        ioerr += WRITE_CDPORT_PORT_STATUSr(unit, port_stat, pm_port);
    } else if (intr_num == CDPORT_INTR_FLOWCONTROL_REQ_FULL) {
        CDPORT_PORT_INTR_STATUSr_CLR(intr_stat);
        CDPORT_PORT_INTR_STATUSr_FLOWCONTROL_REQ_FULLf_SET(intr_stat, intr_bits);
        ioerr += WRITE_CDPORT_PORT_INTR_STATUSr(unit, intr_stat, pm_port);
    }

    return ioerr;
}

static int
xlport_intr_clear(int unit, unsigned int intr_num, int port)
{
    int ioerr = 0;
    XLPORT_INTR_STATUSr_t xlp_stat;
    XLPORT_LINKSTATUS_DOWN_CLEARr_t xlp_linkstat;
    uint32_t pm_inst, inst;
    uint32_t intr_bits;
    int pm_port;

    pm_inst = PORT_TO_CD_INST(port);
    pm_port = CD_INST_TO_PORT(pm_inst);
    inst = port - pm_port;
    /* Second XLPORT uses lane 2 */
    if (inst == 1) {
        inst = 2;
    }
    intr_bits = LSHIFT32(1, inst);

    /* The following interrupts are W1TC (write one to clear) */
    if (intr_num == XLPORT_INTR_LINK_DOWN) {
        XLPORT_LINKSTATUS_DOWN_CLEARr_CLR(xlp_linkstat);
        XLPORT_LINKSTATUS_DOWN_CLEARr_SET(xlp_linkstat, intr_bits);
        ioerr += WRITE_XLPORT_LINKSTATUS_DOWN_CLEARr(unit, xlp_linkstat, pm_port);
    } else if (intr_num == XLPORT_INTR_RX_FLOWCONTROL_REQ_FULL) {
        XLPORT_INTR_STATUSr_CLR(xlp_stat);
        XLPORT_INTR_STATUSr_RX_FLOWCONTROL_REQ_FULLf_SET(xlp_stat, intr_bits);
        ioerr += WRITE_XLPORT_INTR_STATUSr(unit, xlp_stat, pm_port);
    } else if (intr_num == XLPORT_INTR_MIB_RX_MEM_ERR) {
        XLPORT_INTR_STATUSr_CLR(xlp_stat);
        XLPORT_INTR_STATUSr_MIB_RX_MEM_ERRf_SET(xlp_stat, 1);
        ioerr += WRITE_XLPORT_INTR_STATUSr(unit, xlp_stat, pm_port);
    } else if (intr_num == XLPORT_INTR_MIB_TX_MEM_ERR) {
        XLPORT_INTR_STATUSr_CLR(xlp_stat);
        XLPORT_INTR_STATUSr_MIB_TX_MEM_ERRf_SET(xlp_stat, 1);
        ioerr += WRITE_XLPORT_INTR_STATUSr(unit, xlp_stat, pm_port);
    }
    return ioerr;
}

static void
port_intr_clear(int unit, unsigned int intr_num)
{
    int ioerr = 0;
    int port;

    port = PORT_INTR_INST_GET(intr_num);
    intr_num = PORT_INTR_NUM_GET(intr_num);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                          "Clear port %d intr %u\n"),
                 port, intr_num));
    if (intr_num >= MAX_INTRS) {
        return;
    }

    if (intr_num >= XLPORT_INTR_BASE) {
        ioerr = xlport_intr_clear(unit, intr_num, port);
    } else {
        ioerr = cdport_intr_clear(unit, intr_num, port);
    }

    if (ioerr) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Port interrupt %u I/O error\n"),
                   intr_num));
    }
}

static void
cdport_intr(int unit, uint32_t pm_inst)
{
    CDPORT_INTR_STATUSr_t cdp_stat;
    CDPORT_INTR_MASKr_t cdp_mask;
    CDMAC_INTR_STATUSr_t cdm_stat;
    CDMAC_INTR_ENABLEr_t cdm_en;
    uint64_t intr_stat[CDMAC_INST];
    uint32_t intr_bits, intr_num;
    uint32_t regval;
    uint32_t inst;
    int pm_port, port;

    /* Start with an empty interrupt source matrix */
    sal_memset(intr_stat, 0, sizeof(intr_stat));

    /* Read CDPORT register to figure out what to do next */
    pm_port = CD_INST_TO_PORT(pm_inst);
    if (READ_CDPORT_INTR_STATUSr(unit, &cdp_stat, pm_port) != 0) {
        return;
    }
    if (READ_CDPORT_INTR_MASKr(unit, &cdp_mask, pm_port) != 0) {
        return;
    }

    /* CDPORT interrupts that are instantiated per MAC (port) */
    intr_bits = CDPORT_INTR_STATUSr_LINK_DOWNf_GET(cdp_stat);
    intr_bits &= ~CDPORT_INTR_MASKr_LINK_DOWNf_GET(cdp_mask);
    for (inst = 0; inst < CDMAC_INST; inst++) {
        if (intr_bits & LSHIFT32(1, inst)) {
            intr_stat[inst] |= LSHIFT64(1, CDPORT_INTR_LINK_DOWN);
        }
    }
    intr_bits = CDPORT_INTR_STATUSr_LINK_UPf_GET(cdp_stat);
    intr_bits &= ~CDPORT_INTR_MASKr_LINK_UPf_GET(cdp_mask);
    for (inst = 0; inst < CDMAC_INST; inst++) {
        if (intr_bits & LSHIFT32(1, inst)) {
            intr_stat[inst] |= LSHIFT64(1, CDPORT_INTR_LINK_UP);
        }
    }
    intr_bits = CDPORT_INTR_STATUSr_FLOWCONTROL_REQ_FULLf_GET(cdp_stat);
    intr_bits &= ~CDPORT_INTR_MASKr_FLOWCONTROL_REQ_FULLf_GET(cdp_mask);
    for (inst = 0; inst < CDMAC_INST; inst++) {
        if (intr_bits & LSHIFT32(1, inst)) {
            intr_stat[inst] |= LSHIFT64(1, CDPORT_INTR_FLOWCONTROL_REQ_FULL);
        }
    }
    intr_bits = CDPORT_INTR_STATUSr_TSTS_INTERRUPT_STATUSf_GET(cdp_stat);
    intr_bits &= ~CDPORT_INTR_MASKr_TSTS_INTERRUPT_STATUSf_GET(cdp_mask);
    for (inst = 0; inst < CDMAC_INST; inst++) {
        if (intr_bits & LSHIFT32(1, inst)) {
            intr_stat[inst] |= LSHIFT64(1, CDPORT_INTR_TSTS_INTERRUPT_STATUS);
        }
    }
    intr_bits = CDPORT_INTR_STATUSr_FDR_INTERRUPTf_GET(cdp_stat);
    intr_bits &= ~CDPORT_INTR_MASKr_FDR_INTERRUPTf_GET(cdp_mask);
    for (inst = 0; inst < CDMAC_INST; inst++) {
        if (intr_bits & LSHIFT32(1, inst)) {
            intr_stat[inst] |= LSHIFT64(1, CDPORT_INTR_FDR_INTERRUPT);
        }
    }
    intr_bits = CDPORT_INTR_STATUSr_LP_PAGE_RDY_SW_AN_INTERRUPTf_GET(cdp_stat);
    intr_bits &= ~CDPORT_INTR_MASKr_LP_PAGE_RDY_SW_AN_INTERRUPTf_GET(cdp_mask);
    for (inst = 0; inst < CDMAC_INST; inst++) {
        if (intr_bits & LSHIFT32(1, inst)) {
            intr_stat[inst] |= LSHIFT64(1, CDPORT_INTR_LP_PAGE_RDY_SW_AN_INTERRUPT);
        }
    }
    intr_bits = CDPORT_INTR_STATUSr_AN_COMPLETED_SW_AN_INTERRUPTf_GET(cdp_stat);
    intr_bits &= ~CDPORT_INTR_MASKr_AN_COMPLETED_SW_AN_INTERRUPTf_GET(cdp_mask);
    for (inst = 0; inst < CDMAC_INST; inst++) {
        if (intr_bits & LSHIFT32(1, inst)) {
            intr_stat[inst] |= LSHIFT64(1, CDPORT_INTR_AN_COMPLETED_SW_AN_INTERRUPT);
        }
    }
    intr_bits = CDPORT_INTR_STATUSr_AN_GOOD_CHK_AN_INTERRUPTf_GET(cdp_stat);
    intr_bits &= ~CDPORT_INTR_MASKr_AN_GOOD_CHK_AN_INTERRUPTf_GET(cdp_mask);
    for (inst = 0; inst < CDMAC_INST; inst++) {
        if (intr_bits & LSHIFT32(1, inst)) {
            intr_stat[inst] |= LSHIFT64(1, CDPORT_INTR_AN_GOOD_CHK_AN_INTERRUPT);
        }
    }

    /* CDMAC interrupts status */
    intr_bits = CDPORT_INTR_STATUSr_CDMAC_INTRf_GET(cdp_stat);
    intr_bits &= ~CDPORT_INTR_MASKr_CDMAC_INTRf_GET(cdp_mask);
    for (inst = 0; inst < CDMAC_INST; inst++) {
        port = pm_port + inst;
        if (intr_bits & LSHIFT32(1, inst)) {
            /* Actual MAC interrupt sources are in separate register */
            if (READ_CDMAC_INTR_STATUSr(unit, port, &cdm_stat) != 0) {
                continue;
            }
            if (READ_CDMAC_INTR_ENABLEr(unit, port, &cdm_en) != 0) {
                continue;
            }

            /* MAC interrupt bits match hardware (shifted) */
            regval = CDMAC_INTR_STATUSr_GET(cdm_stat) &
                     CDMAC_INTR_ENABLEr_GET(cdm_en);
            intr_stat[inst] |= LSHIFT64(regval, CDMAC_INTR_BASE);
        }
    }

    /* CDPORT interrupts that are instantiated per PM */
    if (CDPORT_INTR_STATUSr_TSC_ECC_1B_ERRf_GET(cdp_stat) &
        (~CDPORT_INTR_MASKr_TSC_ECC_1B_ERRf_GET(cdp_mask))) {
        intr_stat[0] |= LSHIFT64(1, CDPORT_INTR_TSC_ECC_1B_ERR);
    }
    if (CDPORT_INTR_STATUSr_TSC_ECC_2B_ERRf_GET(cdp_stat) &
        (~CDPORT_INTR_MASKr_TSC_ECC_2B_ERRf_GET(cdp_mask))) {
        intr_stat[0] |= LSHIFT64(1, CDPORT_INTR_TSC_ECC_2B_ERR);
    }
    if (CDPORT_INTR_STATUSr_PMD_INTRf_GET(cdp_stat) &
        (~CDPORT_INTR_MASKr_PMD_INTRf_GET(cdp_mask))) {
        intr_stat[0] |= LSHIFT64(1, CDPORT_INTR_PMD_INTR);
    }

    /* Now call interrupt handlers according to the interrupt source matrix */
    for (inst = 0; inst < CDMAC_INST; inst++) {
        if (intr_stat[inst] == 0) {
            /* No active interrupts for this instance */
            continue;
        }
        port = pm_port + inst;
        for (intr_num = 0; intr_num < MAX_INTRS; intr_num++) {
            if ((intr_stat[inst] & LSHIFT64(1, intr_num))) {
                bcmbd_intr_vect_t *iv;
                LOG_DEBUG(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Port interrupt %u, instance %u\n"),
                           intr_num, inst));
                iv = &port_intr_vect_info[unit].intr_vects[intr_num];
                if (iv->func) {
                    uint32_t param = iv->param;
                    /* Add interrupt instance to callback parameter */
                    PORT_INTR_INST_SET(param, port);
                    iv->func(unit, param);
                } else {
                    uint32_t intr_num_inst = intr_num;
                    PORT_INTR_INST_SET(intr_num_inst, port);
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          "No handler for port interrupt %u\n"),
                               intr_num));
                    port_intr_disable(unit, intr_num_inst);
                }
            }
        }
    }
}

static void
xlport_intr(int unit)
{
    XLPORT_INTR_STATUSr_t xlp_stat;
    XLPORT_INTR_ENABLEr_t xlp_en;
    XLMAC_INTR_STATUSr_t xlm_stat;
    XLMAC_INTR_ENABLEr_t xlm_en;
    MGMT_OBM_INTR_STATUSr_t obm_stat;
    MGMT_OBM_INTR_ENABLEr_t obm_en;
    uint64_t intr_stat[XLMAC_INST];
    uint32_t intr_bits, intr_num;
    uint32_t regval;
    uint32_t inst;
    int pm_port, port;

    /* Start with an empty interrupt source matrix */
    sal_memset(intr_stat, 0, sizeof(intr_stat));

    /* Read XLPORT register to figure out what to do next */
    pm_port = 257;
    if (READ_XLPORT_INTR_STATUSr(unit, &xlp_stat, pm_port) != 0) {
        return;
    }
    if (READ_XLPORT_INTR_ENABLEr(unit, &xlp_en, pm_port) != 0) {
        return;
    }

    /* Read MGMT_OBM_INTR_STATUS to check OBM ECC error */
    if (READ_MGMT_OBM_INTR_STATUSr(unit, &obm_stat) != 0) {
        return;
    }
    if (READ_MGMT_OBM_INTR_ENABLEr(unit, &obm_en) != 0) {
        return;
    }

    /* Get interrupts that are instantiated per MAC (port) */
    intr_bits = XLPORT_INTR_STATUSr_RX_FLOWCONTROL_REQ_FULLf_GET(xlp_stat);
    intr_bits &= XLPORT_INTR_ENABLEr_RX_FLOWCONTROL_REQ_FULLf_GET(xlp_en);
    for (inst = 0; inst < XLMAC_INST; inst++) {
        if (intr_bits & LSHIFT32(1, inst)) {
            intr_stat[inst] |= LSHIFT64(1, XLPORT_INTR_RX_FLOWCONTROL_REQ_FULL);
        }
    }
    intr_bits = XLPORT_INTR_STATUSr_LINK_DOWNf_GET(xlp_stat);
    intr_bits &= XLPORT_INTR_ENABLEr_LINK_DOWNf_GET(xlp_en);
    for (inst = 0; inst < XLMAC_INST; inst++) {
        if (intr_bits & LSHIFT32(1, inst)) {
            intr_stat[inst] |= LSHIFT64(1, XLPORT_INTR_LINK_DOWN);
        }
    }
    intr_bits = XLPORT_INTR_STATUSr_MAC_ERRf_GET(xlp_stat);
    intr_bits &= XLPORT_INTR_ENABLEr_MAC_ERRf_GET(xlp_en);
    for (inst = 0; inst < XLMAC_INST; inst++) {
        port = pm_port + inst;
        /* Second XLPORT uses lane 2 */
        if (inst == 2) {
            port = pm_port + 1;
        }
        if (intr_bits & LSHIFT32(1, inst)) {
            /* Actual MAC interrupt sources are in separate register */
            if (READ_XLMAC_INTR_STATUSr(unit, port, &xlm_stat) != 0) {
                continue;
            }
            if (READ_XLMAC_INTR_ENABLEr(unit, port, &xlm_en) != 0) {
                continue;
            }

            /* MAC interrupt bits match hardware (shifted) */
            regval = XLMAC_INTR_STATUSr_GET(xlm_stat, 0) &
                     XLMAC_INTR_ENABLEr_GET(xlm_en, 0);
            intr_stat[inst] |= LSHIFT64(regval, XLMAC_INTR_BASE);
        }
    }

    /* Get interrupts that are instantiated per PM */
    if (XLPORT_INTR_STATUSr_MIB_TX_MEM_ERRf_GET(xlp_stat) &
        XLPORT_INTR_ENABLEr_MIB_TX_MEM_ERRf_GET(xlp_en)) {
        intr_stat[0] |= LSHIFT64(1, XLPORT_INTR_MIB_TX_MEM_ERR);
    }
    if (XLPORT_INTR_STATUSr_MIB_RX_MEM_ERRf_GET(xlp_stat) &
        XLPORT_INTR_ENABLEr_MIB_RX_MEM_ERRf_GET(xlp_en)) {
        intr_stat[0] |= LSHIFT64(1, XLPORT_INTR_MIB_RX_MEM_ERR);
    }
    if (XLPORT_INTR_STATUSr_TSC_ERRf_GET(xlp_stat) &
        XLPORT_INTR_ENABLEr_TSC_ERRf_GET(xlp_en)) {
        intr_stat[0] |= LSHIFT64(1, XLPORT_INTR_TSC_ERR);
    }
    if (MGMT_OBM_INTR_STATUSr_OBM_ECC_ERRf_GET(obm_stat) &
        MGMT_OBM_INTR_ENABLEr_OBM_ECC_ERRf_GET(obm_en)) {
        intr_stat[0] |= LSHIFT64(1, XLPORT_INTR_OBM_ECC_ERR);
    }

    /* Now call interrupt handlers according to the interrupt source matrix */
    for (inst = 0; inst < XLMAC_INST; inst++) {
        if (intr_stat[inst] == 0) {
            /* No active interrupts for this instance */
            continue;
        }
        port = pm_port + inst;
        /* Second XLPORT uses lane 2 */
        if (inst == 2) {
            port = pm_port + 1;
        }
        for (intr_num = 0; intr_num < MAX_INTRS; intr_num++) {
            if ((intr_stat[inst] & LSHIFT64(1, intr_num))) {
                bcmbd_intr_vect_t *iv;
                LOG_DEBUG(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Port interrupt %u, instance %u\n"),
                           intr_num, inst));
                iv = &port_intr_vect_info[unit].intr_vects[intr_num];
                if (iv->func) {
                    uint32_t param = iv->param;
                    /* Add interrupt instance to callback parameter */
                    PORT_INTR_INST_SET(param, port);
                    iv->func(unit, param);
                } else {
                    uint32_t intr_num_inst = intr_num;
                    PORT_INTR_INST_SET(intr_num_inst, port);
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          "No handler for port interrupt %u\n"),
                               intr_num));
                    port_intr_disable(unit, intr_num_inst);
                }
            }
        }
    }
}

static void
port_intr(int unit, uint32_t pm_inst)
{
    if (pm_inst < CDPORT_INST) {
        cdport_intr(unit, pm_inst);
    } else  {
        xlport_intr(unit);
    }
}

static void
port_thread(shr_thread_t tc, void *arg)
{
    int unit = (uintptr_t)arg;
    uint32_t pm_inst;

    while (1) {
        shr_thread_sleep(tc, SHR_THREAD_FOREVER);
        if (shr_thread_stopping(tc)) {
            break;
        }
        for (pm_inst = 0; pm_inst < NUM_PM_INST; pm_inst++) {
            if (port_intr_state[unit].new_intr[pm_inst]) {
                port_intr_state[unit].new_intr[pm_inst] = false;
                port_intr(unit, pm_inst);
                bcmbd_cmicx_lp_intr_enable(unit, LP_INTR_NUM(pm_inst));
            }
        }
    }
}

static void
port_isr(int unit, uint32_t param)
{
    /* Disable interrupt and wake up port thread */
    bcmbd_cmicx_lp_intr_disable_nosync(unit, LP_INTR_NUM(param));
    port_intr_state[unit].new_intr[param] = true;
    shr_thread_wake(port_thread_ctrl[unit]);
}

static int
port_intr_init(int unit)
{
    shr_thread_t tc;
    void *arg;

    if (port_intr_lock[unit] != NULL) {
        return SHR_E_INTERNAL;
    }

    port_intr_lock[unit] = sal_spinlock_create("bcmbdPortIntr");
    if (port_intr_lock[unit] == NULL) {
        return SHR_E_FAIL;
    }

    if (port_thread_ctrl[unit] != NULL) {
        return SHR_E_INTERNAL;
    }

    /* Pass in unit number as context */
    arg = (void *)(uintptr_t)unit;

    tc = shr_thread_start("bcmbdPortIntr", -1, port_thread, arg);
    if (tc == NULL) {
        return SHR_E_FAIL;
    }

    port_thread_ctrl[unit] = tc;

    return SHR_E_NONE;
}

static int
port_intr_cleanup(int unit)
{
    int rv = SHR_E_NONE;
    uint32_t pm_inst;

    for (pm_inst = 0; pm_inst < NUM_PM_INST; pm_inst++) {
        bcmbd_cmicx_lp_intr_disable(unit, LP_INTR_NUM(pm_inst));
    }
    port_intr_connected[unit] = false;

    if (port_thread_ctrl[unit] != NULL) {
        rv = shr_thread_stop(port_thread_ctrl[unit], 2 * SECOND_USEC);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        port_thread_ctrl[unit] = NULL;
    }

    if (port_intr_lock[unit] != NULL) {
        rv = sal_spinlock_destroy(port_intr_lock[unit]);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        port_intr_lock[unit] = NULL;
    }

    return rv;
}

static int
port_intr_func_set(int unit, unsigned int intr_num,
                  bcmbd_intr_f intr_func, uint32_t intr_param)
{
    bcmbd_intr_vect_t *iv;
    uint32_t pm_inst;

    if (!port_intr_connected[unit]) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Add handler for port interrupt %u\n"),
                   intr_num));
        for (pm_inst = 0; pm_inst < NUM_PM_INST; pm_inst++) {
            bcmbd_cmicx_lp_intr_func_set(unit, LP_INTR_NUM(pm_inst),
                                         port_isr, pm_inst);
            bcmbd_cmicx_lp_intr_enable(unit, LP_INTR_NUM(pm_inst));
        }
        port_intr_connected[unit] = true;
    }

    if (intr_num >= MAX_INTRS) {
        return SHR_E_PARAM;
    }
    if (PORT_INTR_INST_GET(intr_param)) {
        return SHR_E_PARAM;
    }

    iv = &port_intr_vect_info[unit].intr_vects[intr_num];
    iv->func = intr_func;
    iv->param = intr_param;

    return SHR_E_NONE;
}

static bcmbd_intr_drv_t port_intr_drv = {
    .intr_func_set = port_intr_func_set,
    .intr_enable = port_intr_enable,
    .intr_disable = port_intr_disable,
    .intr_clear = port_intr_clear,
};

/*******************************************************************************
 * Public functions
 */

int
bcm56996_a0_bd_port_intr_drv_init(int unit)
{
    int rv;

    rv = bcmbd_port_intr_drv_init(unit, &port_intr_drv);
    if (SHR_SUCCESS(rv)) {
        rv = port_intr_init(unit);
        if (SHR_FAILURE(rv)) {
            (void)port_intr_cleanup(unit);
        }
    }
    return rv;
}

int
bcm56996_a0_bd_port_intr_drv_cleanup(int unit)
{
    (void)bcmbd_port_intr_drv_init(unit, NULL);
    return port_intr_cleanup(unit);
}
