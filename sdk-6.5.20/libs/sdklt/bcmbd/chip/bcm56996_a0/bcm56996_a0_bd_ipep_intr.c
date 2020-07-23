/*! \file bcm56996_a0_bd_ipep_intr.c
 *
 * The IPEP interrupts are daisy-chained on a single iProc hardware
 * interrupt, which means that the hardware interrupt handler must
 * inspect additional registers to determine the actual source(s) of
 * the interrupt.
 *
 * Note that both the IPEP interrupt status registers and the top-level
 * register are SOC registers, which should not be read directly from
 * interrupt context. Instead, the primary interrupt handler will mask
 * the iProc interrupt and start a thread that executes the
 * second-level interrupt handler. When the second-level interrupt
 * handler has no more work to do, the iProc interrupt is re-enabled.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <sal/sal_assert.h>
#include <sal/sal_spinlock.h>
#include <sal/sal_sem.h>
#include <sal/sal_time.h>

#include <shr/shr_debug.h>
#include <shr/shr_thread.h>
#include <shr/shr_bitop.h>

#include <bcmbd/bcmbd_ipep_intr_internal.h>
#include <bcmbd/bcmbd_cmicx.h>
#include <bcmbd/bcmbd_cmicx_intr.h>
#include <bcmbd/chip/bcm56996_a0_acc.h>
#include <bcmbd/chip/bcm56996_a0_lp_intr.h>
#include <bcmbd/chip/bcm56996_a0_ipep_intr.h>
#include <bcmdrd/chip/bcm56996_a0_defs.h>
#include "bcm56996_a0_drv.h"

#define  BSL_LOG_MODULE     BSL_LS_BCMBD_INTR

/*******************************************************************************
 * Local definitions
 */

#define MAX_UNITS           BCMDRD_CONFIG_MAX_UNITS
#define MAX_INTRS           MAX_IPEP_INTR
#define IPEP_INST_NUM       4
/*
 * IPEP interrupts in LP:
 *   IP_TO_CMIC (IPEP_INST_NUM instances)
 *   EP_TO_CMIC (IPEP_INST_NUM instances)
 *   CE_TO_CMIC_INTR
 *   DLB_CREDIT_TO_CMIC_INTR
 */
#define IPEP_INTR_NUM       (IPEP_INST_NUM + IPEP_INST_NUM + 1 + 1)

typedef struct bcmbd_intr_vect_s {
    bcmbd_intr_f func;
    uint32_t param;
} bcmbd_intr_vect_t;

typedef struct bcmbd_intr_vect_info_s {
    bcmbd_intr_vect_t intr_vects[MAX_INTRS];
} bcmbd_intr_vect_info_t;

typedef struct bcmbd_intr_map_s {
    SHR_BITDCLNAME(bmp, MAX_INTRS);
} bcmbd_intr_map_t;

typedef struct bcmbd_intr_state_s {
    bool new_intr[IPEP_INTR_NUM];
} bcmbd_intr_state_t;

#define INTR_MASK_LOCK(_u) sal_spinlock_lock(ipep_intr_lock[_u]);
#define INTR_MASK_UNLOCK(_u) sal_spinlock_unlock(ipep_intr_lock[_u]);

/*******************************************************************************
 * Local data
 */
static struct {
    unsigned int intrs[IPEP_INTR_NUM];
    uint32_t ip_base;
    uint32_t ep_base;
} ipep_lp;
static bool ipep_intr_connected[MAX_UNITS];
static bcmbd_intr_map_t ipep_intr_mask[MAX_UNITS];
static sal_spinlock_t ipep_intr_lock[MAX_UNITS];
static bcmbd_intr_vect_info_t ipep_intr_vect_info[MAX_UNITS];
static shr_thread_t ipep_thread_ctrl[MAX_UNITS];
/* Save valid IPEP pipes. */
static uint32_t valid_ipep_pipes[MAX_UNITS];
static bcmbd_intr_state_t ipep_intr_state[MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

static uint32_t
ipep_intr_mask32_get(bcmbd_intr_map_t *intr_map,
                     int base, int num_bits)
{
    int idx;
    uint32_t mask = 0;

    for (idx = 0; idx < num_bits; idx++) {
        if (SHR_BITGET(intr_map->bmp, base + idx)) {
            mask |= (1 << idx);
        }
    }
    return mask;
}

static void
ipep_intr_update(int unit, unsigned int intr_num, int enable)
{
    int ioerr = 0;
    DLB_ECMP_INTR_ENABLEr_t dlb_en;
    FLEX_CTR_EGR_INTR_ENABLEr_t egr_ctr_en;
    FLEX_CTR_ING_INTR_ENABLEr_t ing_ctr_en;
    CENTRAL_CTR_EVICTION_INTR_ENABLEr_t cev_en;
    EP_TO_CMIC_INTR_ENABLEr_t ep2c_en;
    IP_TO_CMIC_INTR_ENABLEr_t ip2c_en;
    IP4_INTR_ENABLEr_t ip4_en;
    ING_DII_INTR_ENABLEr_t idii_en;
    ING_DOI_INTR_ENABLEr_t idoi_en;
    EGR_DII_INTR_ENABLEr_t edii_en;
    EGR_DOI_INTR_ENABLEr_t edoi_en;
    EGR_INTR_ENABLE_2r_t egr_en;
    uint32_t intr_mask;

    /* We may be called by multiple application threads */
    INTR_MASK_LOCK(unit);

    if (intr_num == DLB_ECMP_SER_FIFO_NON_EMPTY) {
        /* Enable/disable interrupt */
        ioerr += READ_DLB_ECMP_INTR_ENABLEr(unit, &dlb_en);
        DLB_ECMP_INTR_ENABLEr_SER_FIFO_NON_EMPTYf_SET(dlb_en, enable);
        ioerr += WRITE_DLB_ECMP_INTR_ENABLEr(unit, dlb_en);
    } else if (intr_num == FLEX_CTR_EGR_SER_FIFO_NON_EMPTY) {
        /* Wait for DE change to duplicate */
        ioerr += READ_FLEX_CTR_EGR_INTR_ENABLEr(unit, &egr_ctr_en);
        FLEX_CTR_EGR_INTR_ENABLEr_SER_FIFO_NON_EMPTYf_SET(egr_ctr_en, enable);
        ioerr += WRITE_FLEX_CTR_EGR_INTR_ENABLEr(unit, egr_ctr_en);
        /* Enable/disable parent interrupt */
        ioerr += READ_EP_TO_CMIC_INTR_ENABLEr(unit, &ep2c_en);
        EP_TO_CMIC_INTR_ENABLEr_FLEX_COUNTERf_SET(ep2c_en, enable);
        ioerr += WRITE_EP_TO_CMIC_INTR_ENABLEr(unit, ep2c_en);
    } else if (intr_num == EP2C_EFPMOD) {
        ioerr += READ_EGR_INTR_ENABLE_2r(unit, &egr_en);
        EGR_INTR_ENABLE_2r_SER_FIFO_NON_EMPTYf_SET(egr_en, enable);
        ioerr += WRITE_EGR_INTR_ENABLE_2r(unit, egr_en);

        ioerr += READ_EP_TO_CMIC_INTR_ENABLEr(unit, &ep2c_en);
        EP_TO_CMIC_INTR_ENABLEr_EFPMODf_SET(ep2c_en, enable);
        ioerr += WRITE_EP_TO_CMIC_INTR_ENABLEr(unit, ep2c_en);
    } else if (intr_num == FLEX_CTR_ING_SER_FIFO_NON_EMPTY) {
        /* wait for DE change to duplicate */
        ioerr += READ_FLEX_CTR_ING_INTR_ENABLEr(unit, &ing_ctr_en);
        FLEX_CTR_ING_INTR_ENABLEr_SER_FIFO_NON_EMPTYf_SET(ing_ctr_en, enable);
        ioerr += WRITE_FLEX_CTR_ING_INTR_ENABLEr(unit, ing_ctr_en);
        /* Enable/disable parent interrupt */
        ioerr += READ_IP4_INTR_ENABLEr(unit, &ip4_en);
        IP4_INTR_ENABLEr_FLEX_CTRf_SET(ip4_en, enable);
        ioerr += WRITE_IP4_INTR_ENABLEr(unit, ip4_en);
        /* Enable/disable parent's parent interrupt */
        enable = enable ||
                 ipep_intr_mask32_get(&ipep_intr_mask[unit],
                                      ING_DOI_BIT_BASE,
                                      ING_DOI_NUM_BITS) ||
                 ipep_intr_mask32_get(&ipep_intr_mask[unit],
                                      IP4_ISW_BIT_BASE,
                                      IP4_ISW_NUM_BITS);
        ioerr += READ_IP_TO_CMIC_INTR_ENABLEr(unit, &ip2c_en);
        IP_TO_CMIC_INTR_ENABLEr_IP4f_SET(ip2c_en, enable);
        ioerr += WRITE_IP_TO_CMIC_INTR_ENABLEr(unit, ip2c_en);
    } else if (intr_num == IP4_ISW) {
        ioerr += READ_IP4_INTR_ENABLEr(unit, &ip4_en);
        IP4_INTR_ENABLEr_ISWf_SET(ip4_en, enable);
        ioerr += WRITE_IP4_INTR_ENABLEr(unit, ip4_en);
        /* Enable/disable parent interrupt */
        enable = enable ||
                 ipep_intr_mask32_get(&ipep_intr_mask[unit],
                                      ING_DOI_BIT_BASE,
                                      ING_DOI_NUM_BITS) ||
                 ipep_intr_mask32_get(&ipep_intr_mask[unit],
                                      FLEX_CTR_ING_BIT_BASE,
                                      FLEX_CTR_ING_NUM_BITS);
        ioerr += READ_IP_TO_CMIC_INTR_ENABLEr(unit, &ip2c_en);
        IP_TO_CMIC_INTR_ENABLEr_IP4f_SET(ip2c_en, enable);
        ioerr += WRITE_IP_TO_CMIC_INTR_ENABLEr(unit, ip2c_en);
    } else if (intr_num >= CENTRAL_CTR_EVICTION_BIT_BASE) {
        intr_mask = ipep_intr_mask32_get(&ipep_intr_mask[unit],
                                         CENTRAL_CTR_EVICTION_BIT_BASE,
                                         CENTRAL_CTR_EVICTION_NUM_BITS);
        CENTRAL_CTR_EVICTION_INTR_ENABLEr_SET(cev_en, intr_mask);
        ioerr += WRITE_CENTRAL_CTR_EVICTION_INTR_ENABLEr(unit, cev_en);
    } else if (intr_num >= IP_TO_CMIC_BIT_BASE) {
        ioerr += READ_IP_TO_CMIC_INTR_ENABLEr(unit, &ip2c_en);
        if (intr_num == IP_TO_CMIC_IP1) {
            IP_TO_CMIC_INTR_ENABLEr_IP1f_SET(ip2c_en, enable);
        } else if (intr_num == IP_TO_CMIC_IP2) {
            IP_TO_CMIC_INTR_ENABLEr_IP2f_SET(ip2c_en, enable);
        } else {
            IP_TO_CMIC_INTR_ENABLEr_IP3f_SET(ip2c_en, enable);
        }
        ioerr += WRITE_IP_TO_CMIC_INTR_ENABLEr(unit, ip2c_en);
    } else if (intr_num >= ING_DOI_BIT_BASE) {
        /* Enable/disable leaf interrupt */
        ioerr += READ_ING_DOI_INTR_ENABLEr(unit, &idoi_en);
        if (intr_num == ING_DOI_SER_FIFO_NON_EMPTY) {
            ING_DOI_INTR_ENABLEr_SER_FIFO_NON_EMPTYf_SET(idoi_en, enable);
        } else if (intr_num == ING_DOI_EVENT_FIFO_0_READ_FAIL ||
                   intr_num == ING_DOI_EVENT_FIFO_1_READ_FAIL ||
                   intr_num == ING_DOI_EVENT_FIFO_2_READ_FAIL ||
                   intr_num == ING_DOI_EVENT_FIFO_3_READ_FAIL) {
            ING_DOI_INTR_ENABLEr_EVENT_FIFO_READ_FAILf_SET(idoi_en, enable);
        } else if (intr_num == ING_DOI_EVENT_FIFO_0_WRITE_FAIL ||
                   intr_num == ING_DOI_EVENT_FIFO_1_WRITE_FAIL ||
                   intr_num == ING_DOI_EVENT_FIFO_2_WRITE_FAIL ||
                   intr_num == ING_DOI_EVENT_FIFO_3_WRITE_FAIL) {
            ING_DOI_INTR_ENABLEr_EVENT_FIFO_WRITE_FAILf_SET(idoi_en, enable);
        } else if (intr_num >= ING_DOI_EVENT_FIFO_0_UNCORRECTED_ECC_ERR &&
                   intr_num <= ING_DOI_EVENT_FIFO_3_UNCORRECTED_ECC_ERR) {
            ING_DOI_INTR_ENABLEr_EVENT_FIFO_UNCORRECTED_ECC_ERRf_SET(idoi_en, enable);
        } else if (intr_num >= ING_DOI_CELL_QUEUE_0_UNCORRECTED_ECC_ERR &&
                   intr_num <= ING_DOI_CELL_QUEUE_3_UNCORRECTED_ECC_ERR) {
            ING_DOI_INTR_ENABLEr_CELL_QUEUE_UNCORRECTED_ECC_ERRf_SET(idoi_en, enable);
        } else if (intr_num >= ING_DOI_SLOT_PIPELINE_0_UNCORRECTED_ECC_ERR &&
                   intr_num <= ING_DOI_SLOT_PIPELINE_3_UNCORRECTED_ECC_ERR) {
            ING_DOI_INTR_ENABLEr_SLOT_PIPELINE_UNCORRECTED_ECC_ERRf_SET(idoi_en, enable);
        } else if (intr_num >= ING_DOI_PKT_BUFFER_0_UNCORRECTED_ECC_ERR &&
                   intr_num <= ING_DOI_PKT_BUFFER_3_UNCORRECTED_ECC_ERR) {
            ING_DOI_INTR_ENABLEr_PKT_BUFFER_UNCORRECTED_ECC_ERRf_SET(idoi_en, enable);
        }
        ioerr += WRITE_ING_DOI_INTR_ENABLEr(unit, idoi_en);

        /* Enable/disable parent interrupt */
        intr_mask = ipep_intr_mask32_get(&ipep_intr_mask[unit],
                                         ING_DOI_BIT_BASE, ING_DOI_NUM_BITS);
        enable = (intr_mask != 0);
        ioerr += READ_IP4_INTR_ENABLEr(unit, &ip4_en);
        IP4_INTR_ENABLEr_IPOSTf_SET(ip4_en, enable);
        ioerr += WRITE_IP4_INTR_ENABLEr(unit, ip4_en);

        /* Enable/disable parent's parent interrupt */
        enable = enable ||
                 ipep_intr_mask32_get(&ipep_intr_mask[unit],
                                      FLEX_CTR_ING_BIT_BASE,
                                      FLEX_CTR_ING_NUM_BITS) ||
                 ipep_intr_mask32_get(&ipep_intr_mask[unit],
                                      IP4_ISW_BIT_BASE,
                                      IP4_ISW_NUM_BITS);
        ioerr += READ_IP_TO_CMIC_INTR_ENABLEr(unit, &ip2c_en);
        IP_TO_CMIC_INTR_ENABLEr_IP4f_SET(ip2c_en, enable);
        ioerr += WRITE_IP_TO_CMIC_INTR_ENABLEr(unit, ip2c_en);

    } else if (intr_num >= ING_DII_BIT_BASE) {
        /* Enable/disable leaf interrupt */
        ioerr += READ_ING_DII_INTR_ENABLEr(unit, &idii_en);
        if (intr_num == ING_DII_MEM_RESET_COMPLETE) {
            ING_DII_INTR_ENABLEr_MEM_RESET_COMPLETEf_SET(idii_en, enable);
        } else if (intr_num >= ING_DII_EVENT_FIFO_0_ERR &&
                   intr_num <= ING_DII_EVENT_FIFO_3_ERR) {
            ING_DII_INTR_ENABLEr_EVENT_FIFO_ERRf_SET(idii_en, enable);
        } else if (intr_num == ING_DII_REFRESH_ERR) {
            ING_DII_INTR_ENABLEr_REFRESH_ERRf_SET(idii_en, enable);
        } else if (intr_num >= ING_DII_EVENT_FIFO0_SER_ERR &&
                   intr_num <= ING_DII_EVENT_FIFO3_SER_ERR) {
            ING_DII_INTR_ENABLEr_EVENT_FIFO_SER_ERRf_SET(idii_en, enable);
        }
        ioerr += WRITE_ING_DII_INTR_ENABLEr(unit, idii_en);

        /* Enable/disable parent interrupt */
        intr_mask = ipep_intr_mask32_get(&ipep_intr_mask[unit],
                                         ING_DII_BIT_BASE, ING_DII_NUM_BITS);
        enable = (intr_mask != 0);
        ioerr += READ_IP_TO_CMIC_INTR_ENABLEr(unit, &ip2c_en);
        IP_TO_CMIC_INTR_ENABLEr_IP0f_SET(ip2c_en, enable);
        ioerr += WRITE_IP_TO_CMIC_INTR_ENABLEr(unit, ip2c_en);
    } else if (intr_num >= EGR_DOI_BIT_BASE) {
        /* Enable/disable leaf interrupt */

        ioerr += READ_EGR_DOI_INTR_ENABLEr(unit, &edoi_en);
        if (intr_num == EGR_DOI_SER_FIFO_NON_EMPTY) {
            EGR_DOI_INTR_ENABLEr_SER_FIFO_NON_EMPTYf_SET(edoi_en, enable);
        } else if (intr_num == EGR_DOI_EVENT_FIFO_0_READ_FAIL ||
                   intr_num == EGR_DOI_EVENT_FIFO_1_READ_FAIL ||
                   intr_num == EGR_DOI_EVENT_FIFO_2_READ_FAIL ||
                   intr_num == EGR_DOI_EVENT_FIFO_3_READ_FAIL) {
            EGR_DOI_INTR_ENABLEr_EVENT_FIFO_READ_FAILf_SET(edoi_en, enable);
        } else if (intr_num == EGR_DOI_EVENT_FIFO_0_WRITE_FAIL ||
                   intr_num == EGR_DOI_EVENT_FIFO_1_WRITE_FAIL ||
                   intr_num == EGR_DOI_EVENT_FIFO_2_WRITE_FAIL ||
                   intr_num == EGR_DOI_EVENT_FIFO_3_WRITE_FAIL) {
            EGR_DOI_INTR_ENABLEr_EVENT_FIFO_WRITE_FAILf_SET(edoi_en, enable);
        } else if (intr_num >= EGR_DOI_EVENT_FIFO_CTRL_0_UNCORRECTED_ECC_ERR &&
                   intr_num <= EGR_DOI_EVENT_FIFO_CTRL_3_UNCORRECTED_ECC_ERR) {
            EGR_DOI_INTR_ENABLEr_EVENT_FIFO_CTRL_UNCORRECTED_ECC_ERRf_SET(edoi_en, enable);
        } else if (intr_num >= EGR_DOI_CELL_QUEUE_0_UNCORRECTED_ECC_ERR &&
                   intr_num <= EGR_DOI_CELL_QUEUE_3_UNCORRECTED_ECC_ERR) {
            EGR_DOI_INTR_ENABLEr_CELL_QUEUE_UNCORRECTED_ECC_ERRf_SET(edoi_en, enable);
        } else if (intr_num >= EGR_DOI_SLOT_PIPELINE_0_UNCORRECTED_ECC_ERR &&
                   intr_num <= EGR_DOI_SLOT_PIPELINE_3_UNCORRECTED_ECC_ERR) {
            EGR_DOI_INTR_ENABLEr_SLOT_PIPELINE_UNCORRECTED_ECC_ERRf_SET(edoi_en, enable);
        } else if (intr_num >= EGR_DOI_PKT_BUFFER_0_UNCORRECTED_ECC_ERR &&
                   intr_num <= EGR_DOI_PKT_BUFFER_3_UNCORRECTED_ECC_ERR) {
            EGR_DOI_INTR_ENABLEr_PKT_BUFFER_UNCORRECTED_ECC_ERRf_SET(edoi_en, enable);
        } else if (intr_num >= EGR_DOI_EVENT_FIFO_DATA_0_UNCORRECTED_ECC_ERR &&
                   intr_num <= EGR_DOI_EVENT_FIFO_DATA_3_UNCORRECTED_ECC_ERR) {
            EGR_DOI_INTR_ENABLEr_EVENT_FIFO_DATA_UNCORRECTED_ECC_ERRf_SET(edoi_en, enable);
        }
        ioerr += WRITE_EGR_DOI_INTR_ENABLEr(unit, edoi_en);

        /* Enable/disable parent interrupt */
        intr_mask = ipep_intr_mask32_get(&ipep_intr_mask[unit],
                                         EGR_DOI_BIT_BASE, EGR_DOI_NUM_BITS);
        enable = (intr_mask != 0);
        ioerr += READ_EP_TO_CMIC_INTR_ENABLEr(unit, &ep2c_en);
        EP_TO_CMIC_INTR_ENABLEr_EPOSTf_SET(ep2c_en, enable);
        ioerr += WRITE_EP_TO_CMIC_INTR_ENABLEr(unit, ep2c_en);
    } else {
        /* Enable/disable leaf interrupt */
        ioerr += READ_EGR_DII_INTR_ENABLEr(unit, &edii_en);
        if (intr_num == EGR_DII_MEM_RESET_COMPLETE) {
            EGR_DII_INTR_ENABLEr_MEM_RESET_COMPLETEf_SET(edii_en, enable);
        } else if (intr_num >= EGR_DII_EVENT_FIFO_0_ERR &&
                   intr_num <= EGR_DII_EVENT_FIFO_3_ERR) {
            EGR_DII_INTR_ENABLEr_EVENT_FIFO_ERRf_SET(edii_en, enable);
        } else if (intr_num == EGR_DII_REFRESH_ERR) {
            EGR_DII_INTR_ENABLEr_REFRESH_ERRf_SET(edii_en, enable);
        } else if (intr_num >= EGR_DII_EVENT_FIFO0_SER_ERR &&
                   intr_num <= EGR_DII_EVENT_FIFO3_SER_ERR) {
            EGR_DII_INTR_ENABLEr_EVENT_FIFO_SER_ERRf_SET(edii_en, enable);
        }
        ioerr += WRITE_EGR_DII_INTR_ENABLEr(unit, edii_en);

        /* Enable/disable parent interrupt */
        intr_mask = ipep_intr_mask32_get(&ipep_intr_mask[unit],
                                         EGR_DII_BIT_BASE, EGR_DII_NUM_BITS);
        enable = (intr_mask != 0);
        ioerr += READ_EP_TO_CMIC_INTR_ENABLEr(unit, &ep2c_en);
        EP_TO_CMIC_INTR_ENABLEr_EARBf_SET(ep2c_en, enable);
        ioerr += WRITE_EP_TO_CMIC_INTR_ENABLEr(unit, ep2c_en);
    }

    if (ioerr) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "IPEP interupt %u I/O error\n"),
                   intr_num));
    }

    INTR_MASK_UNLOCK(unit);
}

static void
ipep_intr_enable(int unit, unsigned int intr_num)
{
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Enable IPEP intr %u\n"),
                 intr_num));
    if (intr_num >= MAX_INTRS) {
        return;
    }
    SHR_BITSET(ipep_intr_mask[unit].bmp, intr_num);
    ipep_intr_update(unit, intr_num, 1);
}

static void
ipep_intr_disable(int unit, unsigned int intr_num)
{
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Disable IPEP intr %u\n"),
                 intr_num));
    if (intr_num >= MAX_INTRS) {
        return;
    }
    SHR_BITCLR(ipep_intr_mask[unit].bmp, intr_num);
    ipep_intr_update(unit, intr_num, 0);
}

static int
ipep_fifo_intr_status_get(int unit, int inst, unsigned int intr_num)
{
    int intr_status = 0;
    FLEX_CTR_EGR_INTR_STATUSr_t egr_ctr_st;
    EGR_DOI_INTR_STATUSr_t edoi_st;
    EP_TO_CMIC_INTR_STATUSr_t ep2c_st;
    FLEX_CTR_ING_INTR_STATUSr_t ing_ctr_st;
    ING_DOI_INTR_STATUSr_t idoi_st;
    IP4_INTR_STATUSr_t ip4_st;
    DLB_ECMP_INTR_STATUSr_t dlb_st;
    CENTRAL_CTR_EVICTION_INTR_STATUSr_t cev_st;

    switch (intr_num) {
    case FLEX_CTR_EGR_SER_FIFO_NON_EMPTY:
        if (READ_FLEX_CTR_EGR_INTR_STATUSr(unit, inst, &egr_ctr_st) == 0) {
            if (FLEX_CTR_EGR_INTR_STATUSr_SER_FIFO_NON_EMPTYf_GET(egr_ctr_st)) {
                intr_status = 1;
            }
        }
        break;
    case EGR_DOI_SER_FIFO_NON_EMPTY:{
        if (READ_EGR_DOI_INTR_STATUSr(unit, inst, &edoi_st) == 0) {
            if (EGR_DOI_INTR_STATUSr_SER_FIFO_NON_EMPTYf_GET(edoi_st)) {
                intr_status = 1;
            }
        }
        break;
    case ING_DOI_SER_FIFO_NON_EMPTY:
        if (READ_ING_DOI_INTR_STATUSr(unit, inst, &idoi_st) == 0) {
            if (ING_DOI_INTR_STATUSr_SER_FIFO_NON_EMPTYf_GET(idoi_st)) {
                intr_status = 1;
            }
        }
        break;
    case EP2C_EFPMOD:
        if (READ_EP_TO_CMIC_INTR_STATUSr(unit, inst, &ep2c_st) == 0) {
            if (EP_TO_CMIC_INTR_STATUSr_EFPMODf_GET(ep2c_st)) {
                intr_status = 1;
            }
        }
        break;
    case FLEX_CTR_ING_SER_FIFO_NON_EMPTY:
        if (READ_FLEX_CTR_ING_INTR_STATUSr(unit, inst, &ing_ctr_st) == 0) {
            if (FLEX_CTR_ING_INTR_STATUSr_SER_FIFO_NON_EMPTYf_GET(ing_ctr_st)) {
                intr_status = 1;
            }
        }
        }
        break;
    case IP4_ISW:
        if (READ_IP4_INTR_STATUSr(unit, inst, &ip4_st) == 0) {
            if (IP4_INTR_STATUSr_ISWf_GET(ip4_st)) {
                intr_status = 1;
            }
        }
        break;
    case DLB_ECMP_SER_FIFO_NON_EMPTY:
        if (READ_DLB_ECMP_INTR_STATUSr(unit, &dlb_st) == 0) {
            if (DLB_ECMP_INTR_STATUSr_SER_FIFO_NON_EMPTYf_GET(dlb_st)) {
                intr_status = 1;
            }
        }
        break;
    case CENTRAL_CTR_EVICTION_FIFO_NOT_EMPTY:
        if (READ_CENTRAL_CTR_EVICTION_INTR_STATUSr(unit, &cev_st) == 0) {
            if (CENTRAL_CTR_EVICTION_INTR_STATUSr_FIFO_NOT_EMPTYf_GET(cev_st)) {
                intr_status = 1;
            }
        }
        break;
    default:
        break;
    }

    return intr_status;
}

/*
 * Per IPEP DE engineers confirmed, parent interrupt source is cleared
 * automaticatlly, if all son sources be cleared.
 */
static void
ipep_intr_write_clear(int unit, int inst, unsigned int intr_num)
{
    uint32_t ioerr = 0;
    uint32_t val;
    ING_DOI_HW_STATUSr_t idoi_hw_st;
    ING_DII_HW_STATUSr_t idii_hw_st;
    ING_DII_HW_RESET_CONTROL_0r_t idii_hw_rc;
    EGR_DOI_HW_STATUSr_t edoi_hw_st;
    EGR_DII_HW_STATUSr_t edii_hw_st;
    EGR_DII_HW_RESET_CONTROL_0r_t edii_hw_rc;

    INTR_MASK_LOCK(unit);

    /* The order should not change, interrupts exlude non_empty bits. */
    if (intr_num >= CENTRAL_CTR_EVICTION_BIT_BASE) {
        CENTRAL_CTR_EVICTION_INTR_STATUSr_t cev_st;
        READ_CENTRAL_CTR_EVICTION_INTR_STATUSr(unit, &cev_st);
        if (intr_num == CENTRAL_CTR_EVICTION_FIFO_OVERFLOW) {
            CENTRAL_CTR_EVICTION_INTR_STATUSr_FIFO_OVERFLOWf_SET(cev_st, 0);
        } else if (intr_num == CENTRAL_CTR_EVICTION_FIFO_PARITY_ERROR) {
            CENTRAL_CTR_EVICTION_INTR_STATUSr_FIFO_PARITY_ERRORf_SET(cev_st, 0);
        }
        ioerr += WRITE_CENTRAL_CTR_EVICTION_INTR_STATUSr(unit, cev_st);
    } else if (intr_num >= IP_TO_CMIC_BIT_BASE) {
        IP_TO_CMIC_INTR_STATUSr_t ip2c_st;
        ioerr += READ_IP_TO_CMIC_INTR_STATUSr(unit, inst, &ip2c_st);
        if (intr_num == IP_TO_CMIC_IP1) {
            IP_TO_CMIC_INTR_STATUSr_IP1f_SET(ip2c_st, 0);
        } else if (intr_num == IP_TO_CMIC_IP2) {
            IP_TO_CMIC_INTR_STATUSr_IP2f_SET(ip2c_st, 0);
        } else if (intr_num == IP_TO_CMIC_IP3) {
            IP_TO_CMIC_INTR_STATUSr_IP3f_SET(ip2c_st, 0);
        }
        ioerr += WRITE_IP_TO_CMIC_INTR_STATUSr(unit, inst, ip2c_st);
    } else if (intr_num >= ING_DOI_BIT_BASE) {
        /* Set ING_DOI_HW_STATUS to clear */
        if (intr_num >= ING_DOI_EVENT_FIFO_0_READ_FAIL &&
            intr_num <= ING_DOI_EVENT_FIFO_3_WRITE_FAIL) {
            READ_ING_DOI_HW_STATUSr(unit, inst, &idoi_hw_st);
            val = ING_DOI_HW_STATUSr_GET(idoi_hw_st);
            val &= ~(0x1 << (intr_num - ING_DOI_EVENT_FIFO_0_READ_FAIL));
            ING_DOI_HW_STATUSr_SET(idoi_hw_st, val);
            ioerr += WRITE_ING_DOI_HW_STATUSr(unit, inst, idoi_hw_st);
        }
    } else if (intr_num >= ING_DII_BIT_BASE) {
        if (intr_num == ING_DII_MEM_RESET_COMPLETE) {
            READ_ING_DII_HW_RESET_CONTROL_0r(unit, inst, &idii_hw_rc);
            ING_DII_HW_RESET_CONTROL_0r_DONEf_SET(idii_hw_rc, 0);
            ioerr += WRITE_ING_DII_HW_RESET_CONTROL_0r(unit, inst, idii_hw_rc);
        } else {
            /* Set ING_DII_HW_STATUS to clear */
            READ_ING_DII_HW_STATUSr(unit, inst, &idii_hw_st);
            val = ING_DII_HW_STATUSr_GET(idii_hw_st);
            if (intr_num >= ING_DII_EVENT_FIFO_0_ERR &&
                intr_num <= ING_DII_EVENT_FIFO_3_ERR) {
                val &= ~(0x1 << (intr_num - ING_DII_EVENT_FIFO_0_ERR));
            } else if (intr_num >= ING_DII_EVENT_FIFO0_SER_ERR &&
                       intr_num <= ING_DII_EVENT_FIFO3_SER_ERR) {
                val &= ~(0x1 << (intr_num - ING_DII_EVENT_FIFO0_SER_ERR + 5));
            } else if (intr_num == ING_DII_REFRESH_ERR) {
                val &= ~(0x1 << 4);
            }
            ING_DII_HW_STATUSr_SET(idii_hw_st, val);
            ioerr += WRITE_ING_DII_HW_STATUSr(unit, inst, idii_hw_st);
        }
    } else if (intr_num >= EGR_DOI_BIT_BASE) {
        /* Set EGR_DOI_HW_STATUS to clear */
        if (intr_num >= EGR_DOI_EVENT_FIFO_0_READ_FAIL &&
            intr_num <= EGR_DOI_EVENT_FIFO_3_WRITE_FAIL) {
            READ_EGR_DOI_HW_STATUSr(unit, inst, &edoi_hw_st);
            val = EGR_DOI_HW_STATUSr_GET(edoi_hw_st);
            val &= ~(0x1 << (intr_num - EGR_DOI_EVENT_FIFO_0_READ_FAIL));
            EGR_DOI_HW_STATUSr_SET(edoi_hw_st, val);
            ioerr += WRITE_EGR_DOI_HW_STATUSr(unit, inst, edoi_hw_st);
        }
    } else {
        if (intr_num == EGR_DII_MEM_RESET_COMPLETE) {
            READ_EGR_DII_HW_RESET_CONTROL_0r(unit, inst, &edii_hw_rc);
            EGR_DII_HW_RESET_CONTROL_0r_DONEf_SET(edii_hw_rc, 0);
            ioerr += WRITE_EGR_DII_HW_RESET_CONTROL_0r(unit, inst, edii_hw_rc);
        } else {
            /* Set EGR_DII_HW_STATUS to clear */
            READ_EGR_DII_HW_STATUSr(unit, inst, &edii_hw_st);
            val = EGR_DII_HW_STATUSr_GET(edii_hw_st);
            if (intr_num >= EGR_DII_EVENT_FIFO_0_ERR &&
                intr_num <= EGR_DII_EVENT_FIFO_3_ERR) {
                val &= ~(0x1 << (intr_num - EGR_DII_EVENT_FIFO_0_ERR));
            } else if (intr_num >= EGR_DII_EVENT_FIFO0_SER_ERR &&
                       intr_num <= EGR_DII_EVENT_FIFO3_SER_ERR) {
                val &= ~(0x1 << (intr_num - EGR_DII_EVENT_FIFO0_SER_ERR + 5));
            } else if (intr_num == EGR_DII_REFRESH_ERR) {
                val &= ~(0x1 << 4);
            }
            EGR_DII_HW_STATUSr_SET(edii_hw_st, val);
            ioerr += WRITE_EGR_DII_HW_STATUSr(unit, inst, edii_hw_st);
        }
    }

    if (ioerr) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "IPEP interupt %u I/O error\n"),
                   intr_num));
    }

    INTR_MASK_UNLOCK(unit);
}

static void
ipep_intr_clear(int unit, unsigned int intr_num)
{
    uint32_t intr_inst;
    bcmdrd_sid_t fifo_sid = INVALIDm;

    intr_inst = IPEP_INTR_INST_GET(intr_num);
    intr_num = IPEP_INTR_NUM_GET(intr_num);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Clear IPEP intr %u (%u)\n"),
                 intr_num, intr_inst));
    if (intr_num >= MAX_INTRS) {
        return;
    }

    /*
     * For non_empty interrupts, pop out all entries to clear.
     * - FLEX_CTR_EGR_SER_FIFOm
     * - EGR_DIO_SER_FIFOm
     * - EGR_SER_FIFO_2m
     * - FLEX_CTR_ING_SER_FIFOm
     * - ING_DOI_SER_FIFOm
     * - ING_SER_FIFOm
     * - DLB_ECMP_SER_FIFOm
     * - CENTRAL_CTR_EVICTION_FIFOm
     * This should not be done for normal application to avoid events' missing.
     */
    switch (intr_num) {
    case FLEX_CTR_EGR_SER_FIFO_NON_EMPTY:
        fifo_sid = FLEX_CTR_EGR_SER_FIFOm;
        break;
    case EGR_DOI_SER_FIFO_NON_EMPTY:
        fifo_sid = EGR_DOI_SER_FIFOm;
        break;
    case ING_DOI_SER_FIFO_NON_EMPTY:
        fifo_sid = ING_DOI_SER_FIFOm;
        break;
    case EP2C_EFPMOD:
        fifo_sid = EGR_SER_FIFO_2m;
        break;
    case FLEX_CTR_ING_SER_FIFO_NON_EMPTY:
        fifo_sid = FLEX_CTR_ING_SER_FIFOm;
        break;
    case IP4_ISW:
        fifo_sid = ING_SER_FIFOm;
        break;
    case DLB_ECMP_SER_FIFO_NON_EMPTY:
        fifo_sid = DLB_ECMP_SER_FIFOm;
        break;
    case CENTRAL_CTR_EVICTION_FIFO_NOT_EMPTY:
        fifo_sid = CENTRAL_CTR_EVICTION_FIFOm;
        break;
    default:
        break;
    }

    if (fifo_sid == INVALIDm) {
        ipep_intr_write_clear(unit, intr_inst, intr_num);
    } else {
        uint32_t entry_data[BCMDRD_MAX_PT_WSIZE];
        bcmbd_pt_dyn_info_t dyn_info;

        while (ipep_fifo_intr_status_get(unit, intr_inst, intr_num)) {
            dyn_info.tbl_inst = intr_inst;
            dyn_info.index = 0;
            (void)bcmbd_cmicx_pop(unit, fifo_sid, &dyn_info, NULL,
                                  entry_data, BCMDRD_MAX_PT_WSIZE);
        }
    }
}

static void
ipep_intr(int unit, uint32_t intr_idx)
{
    DLB_ECMP_INTR_STATUSr_t dlb_st;
    CENTRAL_CTR_EVICTION_INTR_STATUSr_t cev_st;
    EP_TO_CMIC_INTR_STATUSr_t ep2c_st;
    IP_TO_CMIC_INTR_STATUSr_t ip2c_st;
    IP4_INTR_STATUSr_t ip4_st;
    ING_DII_INTR_STATUSr_t idii_st;
    ING_DII_HW_STATUSr_t idii_hw_st;
    ING_DOI_INTR_STATUSr_t idoi_st;
    ING_DOI_HW_STATUSr_t idoi_hw_st;
    EGR_DII_INTR_STATUSr_t edii_st;
    EGR_DII_HW_STATUSr_t edii_hw_st;
    EGR_DOI_INTR_STATUSr_t edoi_st;
    EGR_DOI_HW_STATUSr_t edoi_hw_st;
    bcmbd_intr_vect_t *iv;
    unsigned int inst = 0;
    unsigned int intr_num;
    bcmbd_intr_map_t ipep_stat[IPEP_INST_NUM];

    /* Start with an empty interrupt source matrix */
    sal_memset(ipep_stat, 0, sizeof(ipep_stat));

    if (intr_idx < ipep_lp.ip_base) {
        if (ipep_lp.intrs[intr_idx] == DLB_CREDIT_TO_CMIC_INTR) {
            if (READ_DLB_ECMP_INTR_STATUSr(unit, &dlb_st) != 0) {
                return;
            }

            if (DLB_ECMP_INTR_STATUSr_SER_FIFO_NON_EMPTYf_GET(dlb_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, DLB_ECMP_SER_FIFO_NON_EMPTY);
            }
        } else {
            if (READ_CENTRAL_CTR_EVICTION_INTR_STATUSr(unit, &cev_st) != 0) {
                return;
            }
            if (CENTRAL_CTR_EVICTION_INTR_STATUSr_FIFO_NOT_EMPTYf_GET(cev_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, CENTRAL_CTR_EVICTION_FIFO_NOT_EMPTY);
            }
            if (CENTRAL_CTR_EVICTION_INTR_STATUSr_FIFO_OVERFLOWf_GET(cev_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, CENTRAL_CTR_EVICTION_FIFO_OVERFLOW);
            }
            if (CENTRAL_CTR_EVICTION_INTR_STATUSr_FIFO_PARITY_ERRORf_GET(cev_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, CENTRAL_CTR_EVICTION_FIFO_PARITY_ERROR);
            }
        }
    } else if (intr_idx < ipep_lp.ep_base) {
        inst = (intr_idx - ipep_lp.ip_base);

        if (READ_IP_TO_CMIC_INTR_STATUSr(unit, inst, &ip2c_st) != 0) {
            return;
        }

        if (IP_TO_CMIC_INTR_STATUSr_IP0f_GET(ip2c_st)) {
            if (READ_ING_DII_INTR_STATUSr(unit, inst, &idii_st) != 0) {
                return;
            }
            if (ING_DII_INTR_STATUSr_MEM_RESET_COMPLETEf_GET(idii_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, ING_DII_MEM_RESET_COMPLETE);
            }
            if (ING_DII_INTR_STATUSr_EVENT_FIFO_ERRf_GET(idii_st)) {
                if (READ_ING_DII_HW_STATUSr(unit, inst, &idii_hw_st) != 0) {
                    return;
                }
                if (ING_DII_HW_STATUSr_EVENT_FIFO_0_WRITE_FAILf_GET(idii_hw_st)) {
                    SHR_BITSET(ipep_stat[inst].bmp, ING_DII_EVENT_FIFO_0_ERR);
                }
                if (ING_DII_HW_STATUSr_EVENT_FIFO_1_WRITE_FAILf_GET(idii_hw_st)) {
                    SHR_BITSET(ipep_stat[inst].bmp, ING_DII_EVENT_FIFO_1_ERR);
                }
                if (ING_DII_HW_STATUSr_EVENT_FIFO_2_WRITE_FAILf_GET(idii_hw_st)) {
                    SHR_BITSET(ipep_stat[inst].bmp, ING_DII_EVENT_FIFO_2_ERR);
                }
                if (ING_DII_HW_STATUSr_EVENT_FIFO_3_WRITE_FAILf_GET(idii_hw_st)) {
                    SHR_BITSET(ipep_stat[inst].bmp, ING_DII_EVENT_FIFO_3_ERR);
                }
            }
            if (ING_DII_INTR_STATUSr_REFRESH_ERRf_GET(idii_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, ING_DII_REFRESH_ERR);
            }
            if (ING_DII_INTR_STATUSr_EVENT_FIFO_SER_ERRf_GET(idii_st)) {
                if (READ_ING_DII_HW_STATUSr(unit, inst, &idii_hw_st) != 0) {
                    return;
                }
                if (ING_DII_HW_STATUSr_EVENT_FIFO_0_SER_ERRORf_GET(idii_hw_st)) {
                    SHR_BITSET(ipep_stat[inst].bmp, ING_DII_EVENT_FIFO0_SER_ERR);
                }
                if (ING_DII_HW_STATUSr_EVENT_FIFO_1_SER_ERRORf_GET(idii_hw_st)) {
                    SHR_BITSET(ipep_stat[inst].bmp, ING_DII_EVENT_FIFO1_SER_ERR);
                }
                if (ING_DII_HW_STATUSr_EVENT_FIFO_2_SER_ERRORf_GET(idii_hw_st)) {
                    SHR_BITSET(ipep_stat[inst].bmp, ING_DII_EVENT_FIFO2_SER_ERR);
                }
                if (ING_DII_HW_STATUSr_EVENT_FIFO_3_SER_ERRORf_GET(idii_hw_st)) {
                    SHR_BITSET(ipep_stat[inst].bmp, ING_DII_EVENT_FIFO3_SER_ERR);
                }
            }
        }

        if (IP_TO_CMIC_INTR_STATUSr_IP4f_GET(ip2c_st)) {
            if (READ_IP4_INTR_STATUSr(unit, inst, &ip4_st) != 0) {
                return;
            }
            if (IP4_INTR_STATUSr_FLEX_CTRf_GET(ip4_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, FLEX_CTR_ING_SER_FIFO_NON_EMPTY);
            }
            if (IP4_INTR_STATUSr_IPOSTf_GET(ip4_st)) {
                if (READ_ING_DOI_INTR_STATUSr(unit, inst, &idoi_st) != 0) {
                    return;
                }
                if (ING_DOI_INTR_STATUSr_SER_FIFO_NON_EMPTYf_GET(idoi_st)) {
                    SHR_BITSET(ipep_stat[inst].bmp, ING_DOI_SER_FIFO_NON_EMPTY);
                }
                if (ING_DOI_INTR_STATUSr_EVENT_FIFO_READ_FAILf_GET(idoi_st)) {
                    if (READ_ING_DOI_HW_STATUSr(unit, inst, &idoi_hw_st) != 0) {
                        return;
                    }
                    if (ING_DOI_HW_STATUSr_EVENT_FIFO_0_READ_FAILf_GET(idoi_hw_st)) {
                        SHR_BITSET(ipep_stat[inst].bmp, ING_DOI_EVENT_FIFO_0_READ_FAIL);
                    }
                    if (ING_DOI_HW_STATUSr_EVENT_FIFO_1_READ_FAILf_GET(idoi_hw_st)) {
                        SHR_BITSET(ipep_stat[inst].bmp, ING_DOI_EVENT_FIFO_1_READ_FAIL);
                    }
                    if (ING_DOI_HW_STATUSr_EVENT_FIFO_2_READ_FAILf_GET(idoi_hw_st)) {
                        SHR_BITSET(ipep_stat[inst].bmp, ING_DOI_EVENT_FIFO_2_READ_FAIL);
                    }
                    if (ING_DOI_HW_STATUSr_EVENT_FIFO_3_READ_FAILf_GET(idoi_hw_st)) {
                        SHR_BITSET(ipep_stat[inst].bmp, ING_DOI_EVENT_FIFO_3_READ_FAIL);
                    }
                }
                if (ING_DOI_INTR_STATUSr_EVENT_FIFO_WRITE_FAILf_GET(idoi_st)) {
                    if (READ_ING_DOI_HW_STATUSr(unit, inst, &idoi_hw_st) != 0) {
                        return;
                    }
                    if (ING_DOI_HW_STATUSr_EVENT_FIFO_0_WRITE_FAILf_GET(idoi_hw_st)) {
                        SHR_BITSET(ipep_stat[inst].bmp, ING_DOI_EVENT_FIFO_0_WRITE_FAIL);
                    }
                    if (ING_DOI_HW_STATUSr_EVENT_FIFO_1_WRITE_FAILf_GET(idoi_hw_st)) {
                        SHR_BITSET(ipep_stat[inst].bmp, ING_DOI_EVENT_FIFO_1_WRITE_FAIL);
                    }
                    if (ING_DOI_HW_STATUSr_EVENT_FIFO_2_WRITE_FAILf_GET(idoi_hw_st)) {
                        SHR_BITSET(ipep_stat[inst].bmp, ING_DOI_EVENT_FIFO_2_WRITE_FAIL);
                    }
                    if (ING_DOI_HW_STATUSr_EVENT_FIFO_3_WRITE_FAILf_GET(idoi_hw_st)) {
                        SHR_BITSET(ipep_stat[inst].bmp, ING_DOI_EVENT_FIFO_3_WRITE_FAIL);
                    }
                }
                if (ING_DOI_INTR_STATUSr_EVENT_FIFO_UNCORRECTED_ECC_ERRf_GET(idoi_st)) {
                    SHR_BITSET(ipep_stat[inst].bmp, ING_DOI_EVENT_FIFO_0_UNCORRECTED_ECC_ERR);
                }
                if (ING_DOI_INTR_STATUSr_CELL_QUEUE_UNCORRECTED_ECC_ERRf_GET(idoi_st)) {
                    SHR_BITSET(ipep_stat[inst].bmp, ING_DOI_CELL_QUEUE_0_UNCORRECTED_ECC_ERR);
                }
                if (ING_DOI_INTR_STATUSr_SLOT_PIPELINE_UNCORRECTED_ECC_ERRf_GET(idoi_st)) {
                    SHR_BITSET(ipep_stat[inst].bmp, ING_DOI_SLOT_PIPELINE_0_UNCORRECTED_ECC_ERR);
                }
                if (ING_DOI_INTR_STATUSr_PKT_BUFFER_UNCORRECTED_ECC_ERRf_GET(idoi_st)) {
                    SHR_BITSET(ipep_stat[inst].bmp, ING_DOI_PKT_BUFFER_0_UNCORRECTED_ECC_ERR);
                }
            }
            if (IP4_INTR_STATUSr_ISWf_GET(ip4_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, IP4_ISW);
            }
        }

        if (IP_TO_CMIC_INTR_STATUSr_IP1f_GET(ip2c_st)) {
            SHR_BITSET(ipep_stat[inst].bmp, IP_TO_CMIC_IP1);
        }
        if (IP_TO_CMIC_INTR_STATUSr_IP2f_GET(ip2c_st)) {
            SHR_BITSET(ipep_stat[inst].bmp, IP_TO_CMIC_IP2);
        }
        if (IP_TO_CMIC_INTR_STATUSr_IP3f_GET(ip2c_st)) {
            SHR_BITSET(ipep_stat[inst].bmp, IP_TO_CMIC_IP3);
        }
    } else {
        inst = (intr_idx - ipep_lp.ep_base);

        if (READ_EP_TO_CMIC_INTR_STATUSr(unit, inst, &ep2c_st) != 0) {
            return;
        }

        if (EP_TO_CMIC_INTR_STATUSr_EARBf_GET(ep2c_st)) {
            if (READ_EGR_DII_INTR_STATUSr(unit, inst, &edii_st) != 0) {
                return;
            }
            if (EGR_DII_INTR_STATUSr_MEM_RESET_COMPLETEf_GET(edii_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, EGR_DII_MEM_RESET_COMPLETE);
            }
            if (EGR_DII_INTR_STATUSr_EVENT_FIFO_ERRf_GET(edii_st)) {
                if (READ_EGR_DII_HW_STATUSr(unit, inst, &edii_hw_st) != 0) {
                    return;
                }
                if (EGR_DII_HW_STATUSr_EVENT_FIFO_0_WRITE_FAILf_GET(edii_hw_st)) {
                    SHR_BITSET(ipep_stat[inst].bmp, EGR_DII_EVENT_FIFO_0_ERR);
                }
                if (EGR_DII_HW_STATUSr_EVENT_FIFO_1_WRITE_FAILf_GET(edii_hw_st)) {
                    SHR_BITSET(ipep_stat[inst].bmp, EGR_DII_EVENT_FIFO_1_ERR);
                }
                if (EGR_DII_HW_STATUSr_EVENT_FIFO_2_WRITE_FAILf_GET(edii_hw_st)) {
                    SHR_BITSET(ipep_stat[inst].bmp, EGR_DII_EVENT_FIFO_2_ERR);
                }
                if (EGR_DII_HW_STATUSr_EVENT_FIFO_3_WRITE_FAILf_GET(edii_hw_st)) {
                    SHR_BITSET(ipep_stat[inst].bmp, EGR_DII_EVENT_FIFO_3_ERR);
                }
            }
            if (EGR_DII_INTR_STATUSr_REFRESH_ERRf_GET(edii_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, EGR_DII_REFRESH_ERR);
            }
            if (EGR_DII_INTR_STATUSr_EVENT_FIFO_SER_ERRf_GET(edii_st)) {
                if (READ_EGR_DII_HW_STATUSr(unit, inst, &edii_hw_st) != 0) {
                    return;
                }
                if (EGR_DII_HW_STATUSr_EVENT_FIFO_0_SER_ERRORf_GET(edii_hw_st)) {
                    SHR_BITSET(ipep_stat[inst].bmp, EGR_DII_EVENT_FIFO0_SER_ERR);
                }
                if (EGR_DII_HW_STATUSr_EVENT_FIFO_1_SER_ERRORf_GET(edii_hw_st)) {
                    SHR_BITSET(ipep_stat[inst].bmp, EGR_DII_EVENT_FIFO1_SER_ERR);
                }
                if (EGR_DII_HW_STATUSr_EVENT_FIFO_2_SER_ERRORf_GET(edii_hw_st)) {
                    SHR_BITSET(ipep_stat[inst].bmp, EGR_DII_EVENT_FIFO2_SER_ERR);
                }
                if (EGR_DII_HW_STATUSr_EVENT_FIFO_3_SER_ERRORf_GET(edii_hw_st)) {
                    SHR_BITSET(ipep_stat[inst].bmp, EGR_DII_EVENT_FIFO3_SER_ERR);
                }
            }
        }

        if (EP_TO_CMIC_INTR_STATUSr_FLEX_COUNTERf_GET(ep2c_st)) {
            SHR_BITSET(ipep_stat[inst].bmp, FLEX_CTR_EGR_SER_FIFO_NON_EMPTY);
        }

        if (EP_TO_CMIC_INTR_STATUSr_EPOSTf_GET(ep2c_st)) {
            if (READ_EGR_DOI_INTR_STATUSr(unit, inst, &edoi_st) != 0) {
                return;
            }
            if (EGR_DOI_INTR_STATUSr_SER_FIFO_NON_EMPTYf_GET(edoi_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, EGR_DOI_SER_FIFO_NON_EMPTY);
            }
            if (EGR_DOI_INTR_STATUSr_EVENT_FIFO_READ_FAILf_GET(edoi_st)) {
                if (READ_EGR_DOI_HW_STATUSr(unit, inst, &edoi_hw_st) != 0) {
                    return;
                }
                if (EGR_DOI_HW_STATUSr_EVENT_FIFO_0_READ_FAILf_GET(edoi_hw_st)) {
                    SHR_BITSET(ipep_stat[inst].bmp, EGR_DOI_EVENT_FIFO_0_READ_FAIL);
                }
                if (EGR_DOI_HW_STATUSr_EVENT_FIFO_1_READ_FAILf_GET(edoi_hw_st)) {
                    SHR_BITSET(ipep_stat[inst].bmp, EGR_DOI_EVENT_FIFO_1_READ_FAIL);
                }
                if (EGR_DOI_HW_STATUSr_EVENT_FIFO_2_READ_FAILf_GET(edoi_hw_st)) {
                    SHR_BITSET(ipep_stat[inst].bmp, EGR_DOI_EVENT_FIFO_2_READ_FAIL);
                }
                if (EGR_DOI_HW_STATUSr_EVENT_FIFO_3_READ_FAILf_GET(edoi_hw_st)) {
                    SHR_BITSET(ipep_stat[inst].bmp, EGR_DOI_EVENT_FIFO_3_READ_FAIL);
                }
            }
            if (EGR_DOI_INTR_STATUSr_EVENT_FIFO_WRITE_FAILf_GET(edoi_st)) {
                if (READ_EGR_DOI_HW_STATUSr(unit, inst, &edoi_hw_st) != 0) {
                    return;
                }
                if (EGR_DOI_HW_STATUSr_EVENT_FIFO_0_WRITE_FAILf_GET(edoi_hw_st)) {
                    SHR_BITSET(ipep_stat[inst].bmp, EGR_DOI_EVENT_FIFO_0_WRITE_FAIL);
                }
                if (EGR_DOI_HW_STATUSr_EVENT_FIFO_1_WRITE_FAILf_GET(edoi_hw_st)) {
                    SHR_BITSET(ipep_stat[inst].bmp, EGR_DOI_EVENT_FIFO_1_WRITE_FAIL);
                }
                if (EGR_DOI_HW_STATUSr_EVENT_FIFO_2_WRITE_FAILf_GET(edoi_hw_st)) {
                    SHR_BITSET(ipep_stat[inst].bmp, EGR_DOI_EVENT_FIFO_2_WRITE_FAIL);
                }
                if (EGR_DOI_HW_STATUSr_EVENT_FIFO_3_WRITE_FAILf_GET(edoi_hw_st)) {
                    SHR_BITSET(ipep_stat[inst].bmp, EGR_DOI_EVENT_FIFO_3_WRITE_FAIL);
                }
            }
            if (EGR_DOI_INTR_STATUSr_EVENT_FIFO_CTRL_UNCORRECTED_ECC_ERRf_GET(edoi_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, EGR_DOI_EVENT_FIFO_CTRL_0_UNCORRECTED_ECC_ERR);
            }
            if (EGR_DOI_INTR_STATUSr_CELL_QUEUE_UNCORRECTED_ECC_ERRf_GET(edoi_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, EGR_DOI_CELL_QUEUE_0_UNCORRECTED_ECC_ERR);
            }
            if (EGR_DOI_INTR_STATUSr_SLOT_PIPELINE_UNCORRECTED_ECC_ERRf_GET(edoi_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, EGR_DOI_SLOT_PIPELINE_0_UNCORRECTED_ECC_ERR);
            }
            if (EGR_DOI_INTR_STATUSr_PKT_BUFFER_UNCORRECTED_ECC_ERRf_GET(edoi_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, EGR_DOI_PKT_BUFFER_0_UNCORRECTED_ECC_ERR);
            }
        }

        if (EP_TO_CMIC_INTR_STATUSr_EFPMODf_GET(ep2c_st)) {
            SHR_BITSET(ipep_stat[inst].bmp, EP2C_EFPMOD);
        }
    }

    /* Now call interrupt handlers according to the interrupt source matrix */
    SHR_BIT_ITER(ipep_stat[inst].bmp, MAX_INTRS, intr_num) {
        if (SHR_BITGET(ipep_stat[inst].bmp, intr_num) == 0) {
            /* No interrupt */
            continue;
        }
        if (SHR_BITGET(ipep_intr_mask[unit].bmp, intr_num) == 0) {
            /* Interrupt disabled */
            continue;
        }
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "IPEP interrupt %u, instance %u\n"),
                   intr_num, inst));
        iv = &ipep_intr_vect_info[unit].intr_vects[intr_num];
        if (iv->func) {
            uint32_t param = iv->param;
            /* Add interrupt instance to callback parameter */
            IPEP_INTR_INST_SET(param, inst);
            iv->func(unit, param);
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "No handler for IPEP interrupt %u\n"),
                       intr_num));
            ipep_intr_disable(unit, intr_num);
        }
    }
}

static void
ipep_thread(shr_thread_t tc, void *arg)
{
    int i;
    int unit = (uintptr_t)arg;

    while (1) {
        shr_thread_sleep(tc, SHR_THREAD_FOREVER);
        if (shr_thread_stopping(tc)) {
            break;
        }
        for (i = 0; i < IPEP_INTR_NUM; i++) {
            if (ipep_intr_state[unit].new_intr[i]) {
                ipep_intr_state[unit].new_intr[i] = false;
                ipep_intr(unit, i);
                bcmbd_cmicx_lp_intr_enable(unit, ipep_lp.intrs[i]);
            }
        }
    }
}

static void
ipep_isr(int unit, uint32_t param)
{
    bcmbd_cmicx_lp_intr_disable_nosync(unit, ipep_lp.intrs[param]);
    ipep_intr_state[unit].new_intr[param] = true;
    shr_thread_wake(ipep_thread_ctrl[unit]);
}

static int
ipep_intr_init(int unit)
{
    shr_thread_t tc;
    void *arg;

    if (ipep_intr_lock[unit] != NULL) {
        return SHR_E_INTERNAL;
    }

    ipep_intr_lock[unit] = sal_spinlock_create("bcmbdIPEPIntr");
    if (ipep_intr_lock[unit] == NULL) {
        return SHR_E_FAIL;
    }

    if (ipep_thread_ctrl[unit] != NULL) {
        return SHR_E_INTERNAL;
    }

    sal_memset(&ipep_intr_state[unit], 0, sizeof(bcmbd_intr_state_t));

    /* Pass in unit number as context */
    arg = (void *)(uintptr_t)unit;

    tc = shr_thread_start("bcmbdIPEPIntr", -1, ipep_thread, arg);
    if (tc == NULL) {
        return SHR_E_FAIL;
    }

    ipep_thread_ctrl[unit] = tc;

    return SHR_E_NONE;
}

static int
ipep_intr_cleanup(int unit)
{
    int i;
    int rv = SHR_E_NONE;

    for (i = 0; i < IPEP_INTR_NUM; i++) {
        bcmbd_cmicx_lp_intr_disable(unit, ipep_lp.intrs[i]);
    }

    ipep_intr_connected[unit] = false;

    if (ipep_thread_ctrl[unit] != NULL) {
        rv = shr_thread_stop(ipep_thread_ctrl[unit], 2 * SECOND_USEC);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        ipep_thread_ctrl[unit] = NULL;
    }

    if (ipep_intr_lock[unit] != NULL) {
        rv = sal_spinlock_destroy(ipep_intr_lock[unit]);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        ipep_intr_lock[unit] = NULL;
    }

    return rv;
}

static int
ipep_intr_func_set(int unit, unsigned int intr_num,
                  bcmbd_intr_f intr_func, uint32_t intr_param)
{
    uint32_t i, inst;
    bcmbd_intr_vect_t *iv;

    if (!ipep_intr_connected[unit]) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Add handler for IPEP interrupt %u\n"),
                   intr_num));
        for (i = 0; i < IPEP_INTR_NUM; i++) {
            /* instance valid check for IP/EP */
            if (i >= ipep_lp.ip_base) {
                inst = (i - ipep_lp.ip_base) % IPEP_INST_NUM;
                if (!(valid_ipep_pipes[unit] & (0x1 << inst))) {
                    continue;
                }
            }
            bcmbd_cmicx_lp_intr_func_set(unit, ipep_lp.intrs[i],
                                         ipep_isr, i);
            bcmbd_cmicx_lp_intr_enable(unit, ipep_lp.intrs[i]);
        }
        ipep_intr_connected[unit] = true;
    }

    if (intr_num >= MAX_INTRS) {
        return SHR_E_PARAM;
    }

    iv = &ipep_intr_vect_info[unit].intr_vects[intr_num];
    iv->func = intr_func;
    iv->param = intr_param;

    return SHR_E_NONE;
}

static bcmbd_intr_drv_t ipep_intr_drv = {
    .intr_func_set = ipep_intr_func_set,
    .intr_enable = ipep_intr_enable,
    .intr_disable = ipep_intr_disable,
    .intr_clear = ipep_intr_clear,
};

static void
ipep_lp_init(void)
{
    static bool init = false;
    uint32_t i;

    if (!init) {
        i = 0;
        ipep_lp.intrs[i++] = CE_TO_CMIC_INTR;
        ipep_lp.intrs[i++] = DLB_CREDIT_TO_CMIC_INTR;
        ipep_lp.ip_base = i;
        ipep_lp.intrs[i++] = QUAD0_IP_TO_CMIC_PIPE_INTR;
        ipep_lp.intrs[i++] = QUAD1_IP_TO_CMIC_PIPE_INTR;
        ipep_lp.intrs[i++] = QUAD2_IP_TO_CMIC_PIPE_INTR;
        ipep_lp.intrs[i++] = QUAD3_IP_TO_CMIC_PIPE_INTR;
        ipep_lp.ep_base = i;
        ipep_lp.intrs[i++] = QUAD0_EP_TO_CMIC_INTR;
        ipep_lp.intrs[i++] = QUAD1_EP_TO_CMIC_INTR;
        ipep_lp.intrs[i++] = QUAD2_EP_TO_CMIC_INTR;
        ipep_lp.intrs[i++] = QUAD3_EP_TO_CMIC_INTR;
        assert(i == IPEP_INTR_NUM);
        init = true;
    }
}

/*******************************************************************************
 * Public functions
 */
int
bcm56996_a0_bd_ipep_intr_drv_init(int unit)
{
    int rv;
    uint32_t pipe_map;

    ipep_lp_init();

    rv = bcmdrd_dev_valid_blk_pipes_get(unit, 0, BLKTYPE_IPIPE, &pipe_map);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    valid_ipep_pipes[unit] = pipe_map;

    rv = bcmbd_ipep_intr_drv_init(unit, &ipep_intr_drv);
    if (SHR_SUCCESS(rv)) {
        rv = ipep_intr_init(unit);
        if (SHR_FAILURE(rv)) {
            (void)ipep_intr_cleanup(unit);
        }
    }

    return rv;
}

int
bcm56996_a0_bd_ipep_intr_drv_cleanup(int unit)
{
    (void)bcmbd_ipep_intr_drv_init(unit, NULL);
    return ipep_intr_cleanup(unit);
}

