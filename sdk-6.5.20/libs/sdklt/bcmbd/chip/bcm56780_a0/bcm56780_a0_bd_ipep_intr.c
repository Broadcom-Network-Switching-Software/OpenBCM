/*! \file bcm56780_a0_bd_ipep_intr.c
 *
 * The IPEP interrupts are daisy-chained on a single iProc hardware
 * interrupt, which means that the hardware interrupt handler must
 * inspect additional registers to determine the actual source(s) of
 * the interrupt.
 *
 * The top level registers are:
 *
 *  - EP_TO_CMIC_INTR_STATUS
 *  - IP_TO_CMIC_INTR_STATUS
 *  - DLB_ECMP_INTR_STATUS
 *  - CENTRAL_CTR_EVICTION_INTR_STATUS
 *
 * These registers contain primary (leaf) interrupt sources as well as
 * secondary (daisy-chained) interrupt sources. The secondary sources
 * span 7 registers, which are:
 *
 *  - EPOST_EGR_INTR_STATUS (EP)
 *  - EGR_DOI_INTR_STATUS (EP)
 *  - EGR_DII_INTR_STATUS (EP)
 *  - RDB0_ENABLE_STATUS (EP)
 *  - RDB1_ENABLE_STATUS (EP)
 *
 *  - IPOST_SER_STATUS_BLK_ING_INTR_STATUS (IP)
 *  - LEARN_CACHE_CTRL (IP)
 *  - ING_DOI_INTR_STATUS (IP)
 *  - ING_DII_INTR_STATUS (IP)
 *  - FT_LEARN_INTR_STATUS (IP)
 *
 * These second level registers also contain both primary (leaf) and
 * secondary (daisy-chained) interrupt sources. The secondary
 * interrupt sources require that one of the following registers is
 * read to obtain the primary interrupt source:
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
#include <sal/sal_thread.h>
#include <sal/sal_spinlock.h>
#include <sal/sal_sem.h>
#include <sal/sal_time.h>

#include <shr/shr_debug.h>
#include <shr/shr_thread.h>
#include <shr/shr_bitop.h>

#include <bcmbd/bcmbd_ipep_intr_internal.h>
#include <bcmbd/bcmbd_cmicx.h>
#include <bcmbd/bcmbd_cmicx_intr.h>
#include <bcmbd/chip/bcm56780_a0_acc.h>
#include <bcmbd/chip/bcm56780_a0_lp_intr.h>
#include <bcmbd/chip/bcm56780_a0_ipep_intr.h>

#include "bcm56780_a0_drv.h"

#define  BSL_LOG_MODULE  BSL_LS_BCMBD_INTR

/*******************************************************************************
 * Local definitions
 */

#define MAX_UNITS       BCMDRD_CONFIG_MAX_UNITS
#define MAX_INTRS       MAX_IPEP_INTR
#define MAX_INST        2

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

#define INTR_MASK_LOCK(_u) sal_spinlock_lock(ipep_intr_lock[_u]);
#define INTR_MASK_UNLOCK(_u) sal_spinlock_unlock(ipep_intr_lock[_u]);

/*******************************************************************************
 * Local data
 */

static bool ipep_intr_connected[MAX_UNITS];
static bcmbd_intr_map_t ipep_intr_mask[MAX_UNITS];
static sal_spinlock_t ipep_intr_lock[MAX_UNITS];
static bcmbd_intr_vect_info_t ipep_intr_vect_info[MAX_UNITS];
static shr_thread_t ipep_thread_ctrl[MAX_UNITS];

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
    int ioerr = 0, rv = 0;
    DLB_ECMP_INTR_ENABLEr_t dlb_en;
    CENTRAL_CTR_EVICTION_INTR_ENABLEr_t cev_en;
    EP_TO_CMIC_INTR_ENABLEr_t ep2c_en;
    IP_TO_CMIC_INTR_ENABLEr_t ip2c_en;
    ING_DII_INTR_ENABLEr_t idii_en;
    ING_DOI_INTR_ENABLEr_t idoi_en;
    EGR_DII_INTR_ENABLEr_t edii_en;
    EGR_DOI_INTR_ENABLEr_t edoi_en;
    RDB0_INTR_ENABLEr_t rdb0_en;
    RDB1_INTR_ENABLEr_t rdb1_en;
    uint32_t intr_mask = 0;
    uint32_t epipe_map = 0;
    int pipe = 0;

    /* We may be called by multiple application threads */
    INTR_MASK_LOCK(unit);

    if (intr_num == DLB_ECMP_INTR) {
        /* Enable/disable interrupt */
        ioerr += READ_DLB_ECMP_INTR_ENABLEr(unit, &dlb_en);
        DLB_ECMP_INTR_ENABLEr_SER_FIFO_NON_EMPTYf_SET(dlb_en, enable);
        ioerr += WRITE_DLB_ECMP_INTR_ENABLEr(unit, dlb_en);
    } else if (intr_num == LEARN_CACHE_INTR) {
        /* LEARN_CACHE interrupt is controlled by IP interrupt ctrl field IP7 */
        ioerr += READ_IP_TO_CMIC_INTR_ENABLEr(unit, &ip2c_en);
        IP_TO_CMIC_INTR_ENABLEr_IP7f_SET(ip2c_en, enable);
        ioerr += WRITE_IP_TO_CMIC_INTR_ENABLEr(unit, ip2c_en);
    } else if (intr_num == IPOST_SER_STATUS_BLK_ING_SER_FIFO_NON_EMPTY) {
        /* IPOST_SER interrupt is controlled by IP interrupt ctrl field IP9 */
        ioerr += READ_IP_TO_CMIC_INTR_ENABLEr(unit, &ip2c_en);
        IP_TO_CMIC_INTR_ENABLEr_IP9f_SET(ip2c_en, enable);
        ioerr += WRITE_IP_TO_CMIC_INTR_ENABLEr(unit, ip2c_en);
    } else if (intr_num == EPOST_EGR_SER_FIFO_NON_EMPTY) {
        /* IPOST_SER interrupt is controlled by EP interrupt ctrl field EP3 */
        ioerr += READ_EP_TO_CMIC_INTR_ENABLEr(unit, &ep2c_en);
        EP_TO_CMIC_INTR_ENABLEr_EP3f_SET(ep2c_en, enable);
        ioerr += WRITE_EP_TO_CMIC_INTR_ENABLEr(unit, ep2c_en);
    } else if (intr_num == FT_LEARN_INTR) {
        /* FT_LEARN interrupt is controlled by IP interrupt ctrl field IP4 */
        ioerr += READ_IP_TO_CMIC_INTR_ENABLEr(unit, &ip2c_en);
        IP_TO_CMIC_INTR_ENABLEr_IP4f_SET(ip2c_en, enable);
        ioerr += WRITE_IP_TO_CMIC_INTR_ENABLEr(unit, ip2c_en);
    } else if (intr_num >= CENTRAL_CTR_EVICTION_BIT_BASE) {
        /* Enable/disable interrupt */
        ioerr += READ_CENTRAL_CTR_EVICTION_INTR_ENABLEr(unit, &cev_en);
        switch (intr_num) {
            case CENTRAL_CTR_EVICTION_FIFO_NOT_EMPTY:
                CENTRAL_CTR_EVICTION_INTR_ENABLEr_FIFO_NOT_EMPTYf_SET(cev_en, enable);
                break;
            case CENTRAL_CTR_EVICTION_FIFO_OVERFLOW:
                CENTRAL_CTR_EVICTION_INTR_ENABLEr_FIFO_OVERFLOWf_SET(cev_en, enable);
                break;
            case CENTRAL_CTR_EVICTION_FIFO_PARITY_ERROR:
                CENTRAL_CTR_EVICTION_INTR_ENABLEr_FIFO_PARITY_ERRORf_SET(cev_en, enable);
                break;
            default:
                break;
        }
        ioerr += WRITE_CENTRAL_CTR_EVICTION_INTR_ENABLEr(unit, cev_en);
    } else if (intr_num >= ING_DII_BIT_BASE) {
        /* Enable/disable leaf interrupt */
        intr_mask = ipep_intr_mask32_get(&ipep_intr_mask[unit],
                                        ING_DII_BIT_BASE, ING_DII_NUM_BITS);
        ING_DII_INTR_ENABLEr_SET(idii_en, intr_mask);
        ioerr += WRITE_ING_DII_INTR_ENABLEr(unit, idii_en);
        /* Enable/disable parent interrupt if needed */
        if (enable || intr_mask == 0) {
            /* This register shares the THDO bit in the parent */
            ioerr += READ_IP_TO_CMIC_INTR_ENABLEr(unit, &ip2c_en);
            IP_TO_CMIC_INTR_ENABLEr_DIIf_SET(ip2c_en, enable);
            ioerr += WRITE_IP_TO_CMIC_INTR_ENABLEr(unit, ip2c_en);
        }
    } else if (intr_num >= ING_DOI_BIT_BASE) {
        /* Enable/disable leaf interrupt */
        intr_mask = ipep_intr_mask32_get(&ipep_intr_mask[unit],
                                        ING_DOI_BIT_BASE, ING_DOI_NUM_BITS);
        ING_DOI_INTR_ENABLEr_SET(idoi_en, intr_mask);
        ioerr += WRITE_ING_DOI_INTR_ENABLEr(unit, idoi_en);
        /* Enable/disable parent interrupt if needed */
        if (enable || intr_mask == 0) {
            /* This register shares the THDO bit in the parent */
            ioerr += READ_IP_TO_CMIC_INTR_ENABLEr(unit, &ip2c_en);
            IP_TO_CMIC_INTR_ENABLEr_DOIf_SET(ip2c_en, enable);
            ioerr += WRITE_IP_TO_CMIC_INTR_ENABLEr(unit, ip2c_en);
        }
    } else if (intr_num >= EGR_DII_BIT_BASE) {
        /* Enable/disable leaf interrupt */
        intr_mask = ipep_intr_mask32_get(&ipep_intr_mask[unit],
                                        EGR_DII_BIT_BASE, EGR_DII_NUM_BITS);
        EGR_DII_INTR_ENABLEr_SET(edii_en, intr_mask);
        ioerr += WRITE_EGR_DII_INTR_ENABLEr(unit, edii_en);
        /* Enable/disable parent interrupt if needed */
        if (enable || intr_mask == 0) {
            /* This register shares the THDO bit in the parent */
            ioerr += READ_EP_TO_CMIC_INTR_ENABLEr(unit, &ep2c_en);
            EP_TO_CMIC_INTR_ENABLEr_DIIf_SET(ep2c_en, enable);
            ioerr += WRITE_EP_TO_CMIC_INTR_ENABLEr(unit, ep2c_en);
        }
    } else if (intr_num >= EGR_DOI_BIT_BASE) {
        /* Enable/disable leaf interrupt */
        intr_mask = ipep_intr_mask32_get(&ipep_intr_mask[unit],
                                        EGR_DOI_BIT_BASE, EGR_DOI_NUM_BITS);
        EGR_DOI_INTR_ENABLEr_SET(edoi_en, intr_mask);
        ioerr += WRITE_EGR_DOI_INTR_ENABLEr(unit, edoi_en);
        /* Enable/disable parent interrupt if needed */
        if (enable || intr_mask == 0) {
            /* This register shares the THDO bit in the parent */
            ioerr += READ_EP_TO_CMIC_INTR_ENABLEr(unit, &ep2c_en);
            EP_TO_CMIC_INTR_ENABLEr_DOIf_SET(ep2c_en, enable);
            ioerr += WRITE_EP_TO_CMIC_INTR_ENABLEr(unit, ep2c_en);
        }
    } else if (intr_num == EP_TO_CMIC_RDB) {
        rv = bcmdrd_dev_valid_blk_pipes_get(unit, 0, BLKTYPE_EPIPE, &epipe_map);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "IPEP interupt %u valid pipe number get error\n"),
                     intr_num));
        }
        /*! Enable/disable leaf interrupt */
        while (epipe_map) {
            if (epipe_map & 0x1) {
                intr_mask = 0xfff; /* Enabling RDB interrupts. */
                RDB0_INTR_ENABLEr_SET(rdb0_en, intr_mask);
                ioerr += WRITE_RDB0_INTR_ENABLEr(unit, pipe, rdb0_en);
                RDB1_INTR_ENABLEr_SET(rdb1_en, intr_mask);
                ioerr += WRITE_RDB1_INTR_ENABLEr(unit, pipe, rdb1_en);
            }
            epipe_map >>= 1;
            pipe++;
        }
        /* Enable/disable parent interrupt if needed. */
        if (enable || intr_mask == 0) {
            ioerr += READ_EP_TO_CMIC_INTR_ENABLEr(unit, &ep2c_en);
            EP_TO_CMIC_INTR_ENABLEr_RDBf_SET(ep2c_en, enable);
            ioerr += WRITE_EP_TO_CMIC_INTR_ENABLEr(unit, ep2c_en);
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
ipep_intr_status_get(int unit, int inst, unsigned int intr_num)
{
    int intr_status = 0;

    switch (intr_num) {
    case EPOST_EGR_SER_FIFO_NON_EMPTY:
    {
        EPOST_EGR_INTR_STATUSr_t eei_st;
        if (READ_EPOST_EGR_INTR_STATUSr(unit, inst, &eei_st) == 0) {
            if (EPOST_EGR_INTR_STATUSr_SER_FIFO_NON_EMPTYf_GET(eei_st)) {
                intr_status = 1;
            }
        }
    }
    break;
    case EGR_DOI_SER_FIFO_NON_EMPTY:
    {
        EGR_DOI_INTR_STATUSr_t edoi_st;
        if (READ_EGR_DOI_INTR_STATUSr(unit, inst, &edoi_st) == 0) {
            if (EGR_DOI_INTR_STATUSr_SER_FIFO_NON_EMPTYf_GET(edoi_st)) {
                intr_status = 1;
            }
        }
    }
    break;
    case IPOST_SER_STATUS_BLK_ING_SER_FIFO_NON_EMPTY:
    {
        IPOST_SER_STATUS_BLK_ING_INTR_STATUSr_t iss_st;
        if (READ_IPOST_SER_STATUS_BLK_ING_INTR_STATUSr(unit, inst, &iss_st) == 0) {
            if (IPOST_SER_STATUS_BLK_ING_INTR_STATUSr_SER_FIFO_NON_EMPTYf_GET(iss_st)) {
                intr_status = 1;
            }
        }
    }
    break;
    case LEARN_CACHE_INTR:
    {
        IP_TO_CMIC_INTR_STATUSr_t ip2c_st;
        if (READ_IP_TO_CMIC_INTR_STATUSr(unit, inst, &ip2c_st) == 0) {
            if (IP_TO_CMIC_INTR_STATUSr_IP7f_GET(ip2c_st)) {
                intr_status = 1;
            }
        }
    }
    break;
    case ING_DOI_SER_FIFO_NON_EMPTY:
    {
        ING_DOI_INTR_STATUSr_t idoi_st;
        if (READ_ING_DOI_INTR_STATUSr(unit, inst, &idoi_st) == 0) {
            if (ING_DOI_INTR_STATUSr_SER_FIFO_NON_EMPTYf_GET(idoi_st)) {
                intr_status = 1;
            }
        }
    }
    break;
    case DLB_ECMP_INTR:
    {
        DLB_ECMP_INTR_STATUSr_t dlb_st;
        if (READ_DLB_ECMP_INTR_STATUSr(unit, &dlb_st) == 0) {
            if (DLB_ECMP_INTR_STATUSr_SER_FIFO_NON_EMPTYf_GET(dlb_st)) {
                intr_status = 1;
            }
        }
    }
    break;
    case CENTRAL_CTR_EVICTION_FIFO_NOT_EMPTY:
    {
        CENTRAL_CTR_EVICTION_INTR_STATUSr_t cev_st;
        if (READ_CENTRAL_CTR_EVICTION_INTR_STATUSr(unit, &cev_st) == 0) {
            if (CENTRAL_CTR_EVICTION_INTR_STATUSr_FIFO_NOT_EMPTYf_GET(cev_st)) {
                intr_status = 1;
            }
        }
    }
    break;
    case FT_LEARN_INTR:
    {
        IP_TO_CMIC_INTR_STATUSr_t ip2c_st;
        if (READ_IP_TO_CMIC_INTR_STATUSr(unit, inst, &ip2c_st) == 0) {
            if (IP_TO_CMIC_INTR_STATUSr_IP4f_GET(ip2c_st)) {
                intr_status = 1;
            }
        }
    }
    break;
    default:
        break;
    }

    return intr_status;
}
static void
ipep_intr_fifo_pop(int unit, uint32_t intr_inst,
                   unsigned int intr_num, bcmdrd_sid_t fifo_sid)
{
    uint32_t entry_data[BCMDRD_MAX_PT_WSIZE];
    bcmbd_pt_dyn_info_t dyn_info;

    while (ipep_intr_status_get(unit, intr_inst, intr_num)) {
        dyn_info.tbl_inst = intr_inst;
        dyn_info.index = 0;
        (void)bcmbd_cmicx_pop(unit, fifo_sid, &dyn_info, NULL,
                              entry_data, BCMDRD_MAX_PT_WSIZE);
    }

    return;
}

static void
ipep_intr_clear(int unit, unsigned int intr_num)
{
    uint32_t intr_inst;
    bcmdrd_sid_t fifo_sid = INVALIDm;
    RDB0_INTR_STATUSr_t rdb0_status;
    RDB1_INTR_STATUSr_t rdb1_status;
    EP_TO_CMIC_INTR_STATUSr_t ep2c_status;

    intr_inst = IPEP_INTR_INST_GET(intr_num);
    intr_num = IPEP_INTR_NUM_GET(intr_num);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Clear IPEP intr %u (%u)\n"),
                 intr_num, intr_inst));
    if (intr_num >= MAX_INTRS) {
        return;
    }

    /* Needs to do sbus pop from below fifos to clear
     * - EPOST_EGR_SER_FIFO
     * - EGR_DIO_SER_FIFO
     * - IPOST_SER_STATUS_BLK_ING_SER_FIFO
     * - LEARN_CACHE_CACHE
     * - ING_DOI_SER_FIFO
     * - DLB_ECMP_SER_FIFO
     * - CENTRAL_CTR_EVICTION_FIFO
     * - FT_LEARN_INTR_FIFO
     */
    switch (intr_num) {
    case EPOST_EGR_SER_FIFO_NON_EMPTY:
        fifo_sid = EPOST_EGR_SER_FIFOm;
        break;
    case EGR_DOI_SER_FIFO_NON_EMPTY:
        fifo_sid = EGR_DOI_SER_FIFOm;
        break;
    case IPOST_SER_STATUS_BLK_ING_SER_FIFO_NON_EMPTY:
        fifo_sid = IPOST_SER_STATUS_BLK_ING_SER_FIFOm;
        break;
    case LEARN_CACHE_INTR:
        fifo_sid = LEARN_CACHE_CACHEm;
        break;
    case ING_DOI_SER_FIFO_NON_EMPTY:
        fifo_sid = ING_DOI_SER_FIFOm;
        break;
    case DLB_ECMP_INTR:
        fifo_sid = DLB_ECMP_SER_FIFOm;
        break;
    case CENTRAL_CTR_EVICTION_FIFO_NOT_EMPTY:
        fifo_sid = CENTRAL_CTR_EVICTION_FIFOm;
        break;
    case EP_TO_CMIC_RDB:
        /* clear leaf interrupt instance */
        RDB0_INTR_STATUSr_CLR(rdb0_status);
        WRITE_RDB0_INTR_STATUSr(unit, intr_inst, rdb0_status);
        RDB1_INTR_STATUSr_CLR(rdb1_status);
        WRITE_RDB1_INTR_STATUSr(unit, intr_inst, rdb1_status);
        /* Attempt to clear parent interrupt instance. */
        READ_EP_TO_CMIC_INTR_STATUSr(unit, intr_inst, &ep2c_status);
        EP_TO_CMIC_INTR_STATUSr_RDBf_SET(ep2c_status, 0);
        WRITE_EP_TO_CMIC_INTR_STATUSr(unit, intr_inst, ep2c_status);
        break;
    case FT_LEARN_INTR:
        fifo_sid = FT_LEARN_NOTIFY_FIFOm;
        break;
    default:
        break;
    }

    if (fifo_sid == INVALIDm) {
        if ( intr_num != EP_TO_CMIC_RDB) {
             LOG_WARN(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                             "Not able to do clear on IPEP intr %u (%u)\n"),
                  intr_num, intr_inst));
        }
    } else {
        ipep_intr_fifo_pop(unit, intr_inst, intr_num, fifo_sid);
    }

    return;
}

static void
ipep_intr(int unit)
{
    DLB_ECMP_INTR_STATUSr_t dlb_st;
    CENTRAL_CTR_EVICTION_INTR_STATUSr_t cev_st;
    EP_TO_CMIC_INTR_STATUSr_t ep2c_st;
    IP_TO_CMIC_INTR_STATUSr_t ip2c_st;
    IPOST_SER_STATUS_BLK_ING_INTR_STATUSr_t iss_st;
    EPOST_EGR_INTR_STATUSr_t eei_st;
    ING_DII_INTR_STATUSr_t idii_st;
    ING_DOI_INTR_STATUSr_t idoi_st;
    EGR_DII_INTR_STATUSr_t edii_st;
    EGR_DOI_INTR_STATUSr_t edoi_st;
    bcmbd_intr_vect_t *iv;
    unsigned int inst;
    unsigned int intr_num;

    bcmbd_intr_map_t ipep_stat[MAX_INST];

    /* Start with an empty interrupt source matrix */
    sal_memset(ipep_stat, 0, sizeof(ipep_stat));

    for (inst = 0; inst < MAX_INST; inst++) {
        /* Processing TOP level IP_TO_CMIC_INTR_STATUS */
        if (READ_IP_TO_CMIC_INTR_STATUSr(unit, inst, &ip2c_st) != 0) {
            return;
        }

        if (IP_TO_CMIC_INTR_STATUSr_DIIf_GET(ip2c_st)) {
            if (READ_ING_DII_INTR_STATUSr(unit, inst, &idii_st) != 0) {
                return;
            }
            if (ING_DII_INTR_STATUSr_MEM_RESET_COMPLETEf_GET(idii_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, ING_DII_MEM_RESET_COMPLETE);
            }
            if (ING_DII_INTR_STATUSr_EVENT_FIFO_ERRf_GET(idii_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, ING_DII_EVENT_FIFO_ERR);
            }
            if (ING_DII_INTR_STATUSr_REFRESH_ERRf_GET(idii_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, ING_DII_REFRESH_ERR);
            }
        }

        if (IP_TO_CMIC_INTR_STATUSr_DOIf_GET(ip2c_st)) {
            if (READ_ING_DOI_INTR_STATUSr(unit, inst, &idoi_st) != 0) {
                return;
            }
            if (ING_DOI_INTR_STATUSr_SER_FIFO_NON_EMPTYf_GET(idoi_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, ING_DOI_SER_FIFO_NON_EMPTY);
            }
            if (ING_DOI_INTR_STATUSr_EVENT_FIFO_0_READ_FAILf_GET(idoi_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, ING_DOI_EVENT_FIFO_0_READ_FAIL);
            }
            if (ING_DOI_INTR_STATUSr_EVENT_FIFO_0_WRITE_FAILf_GET(idoi_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, ING_DOI_EVENT_FIFO_0_WRITE_FAIL);
            }
            if (ING_DOI_INTR_STATUSr_CELL_QUEUE_0_UNCORRECTED_ECC_ERRf_GET(idoi_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, ING_DOI_CELL_QUEUE_0_UNCORRECTED_ECC_ERR);
            }
            if (ING_DOI_INTR_STATUSr_SLOT_PIPELINE_0_UNCORRECTED_ECC_ERRf_GET(idoi_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, ING_DOI_SLOT_PIPELINE_0_UNCORRECTED_ECC_ERR);
            }
            if (ING_DOI_INTR_STATUSr_EVENT_FIFO_1_READ_FAILf_GET(idoi_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, ING_DOI_EVENT_FIFO_1_READ_FAIL);
            }
            if (ING_DOI_INTR_STATUSr_EVENT_FIFO_1_WRITE_FAILf_GET(idoi_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, ING_DOI_EVENT_FIFO_1_WRITE_FAIL);
            }
            if (ING_DOI_INTR_STATUSr_CELL_QUEUE_1_UNCORRECTED_ECC_ERRf_GET(idoi_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, ING_DOI_CELL_QUEUE_1_UNCORRECTED_ECC_ERR);
            }
            if (ING_DOI_INTR_STATUSr_SLOT_PIPELINE_1_UNCORRECTED_ECC_ERRf_GET(idoi_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, ING_DOI_SLOT_PIPELINE_1_UNCORRECTED_ECC_ERR);
            }
        }

        if (IP_TO_CMIC_INTR_STATUSr_IP7f_GET(ip2c_st)) {
            SHR_BITSET(ipep_stat[inst].bmp, LEARN_CACHE_INTR);
        }

        if (IP_TO_CMIC_INTR_STATUSr_IP4f_GET(ip2c_st)) {
            SHR_BITSET(ipep_stat[inst].bmp, FT_LEARN_INTR);
        }

        if (IP_TO_CMIC_INTR_STATUSr_IP9f_GET(ip2c_st)) {
            if (READ_IPOST_SER_STATUS_BLK_ING_INTR_STATUSr(unit, inst, &iss_st) != 0) {
                return;
            }
            if (IPOST_SER_STATUS_BLK_ING_INTR_STATUSr_SER_FIFO_NON_EMPTYf_GET(iss_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, IPOST_SER_STATUS_BLK_ING_SER_FIFO_NON_EMPTY);
            }
        }

        /* Processing TOP level EP_TO_CMIC_INTR_STATUS */
        if (READ_EP_TO_CMIC_INTR_STATUSr(unit, inst, &ep2c_st) != 0) {
            return;
        }

        if (EP_TO_CMIC_INTR_STATUSr_DIIf_GET(ep2c_st)) {
            if (READ_EGR_DII_INTR_STATUSr(unit, inst, &edii_st) != 0) {
                return;
            }
            if (EGR_DII_INTR_STATUSr_MEM_RESET_COMPLETEf_GET(edii_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, EGR_DII_MEM_RESET_COMPLETE);
            }
            if (EGR_DII_INTR_STATUSr_EVENT_FIFO_ERRf_GET(edii_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, EGR_DII_EVENT_FIFO_ERR);
            }
            if (EGR_DII_INTR_STATUSr_REFRESH_ERRf_GET(edii_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, EGR_DII_REFRESH_ERR);
            }
        }

        if (EP_TO_CMIC_INTR_STATUSr_DOIf_GET(ep2c_st)) {
            if (READ_EGR_DOI_INTR_STATUSr(unit, inst, &edoi_st) != 0) {
                return;
            }
            if (EGR_DOI_INTR_STATUSr_SER_FIFO_NON_EMPTYf_GET(edoi_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, EGR_DOI_SER_FIFO_NON_EMPTY);
            }
            if (EGR_DOI_INTR_STATUSr_EVENT_FIFO_0_READ_FAILf_GET(edoi_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, EGR_DOI_EVENT_FIFO_0_READ_FAIL);
            }
            if (EGR_DOI_INTR_STATUSr_EVENT_FIFO_0_WRITE_FAILf_GET(edoi_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, EGR_DOI_EVENT_FIFO_0_WRITE_FAIL);
            }
            if (EGR_DOI_INTR_STATUSr_CELL_QUEUE_0_UNCORRECTED_ECC_ERRf_GET(edoi_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, EGR_DOI_CELL_QUEUE_0_UNCORRECTED_ECC_ERR);
            }
            if (EGR_DOI_INTR_STATUSr_SLOT_PIPELINE_0_UNCORRECTED_ECC_ERRf_GET(edoi_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, ING_DOI_SLOT_PIPELINE_0_UNCORRECTED_ECC_ERR);
            }
            if (EGR_DOI_INTR_STATUSr_EVENT_FIFO_1_READ_FAILf_GET(edoi_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, EGR_DOI_EVENT_FIFO_1_READ_FAIL);
            }
            if (EGR_DOI_INTR_STATUSr_EVENT_FIFO_1_WRITE_FAILf_GET(edoi_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, EGR_DOI_EVENT_FIFO_1_WRITE_FAIL);
            }
            if (EGR_DOI_INTR_STATUSr_CELL_QUEUE_1_UNCORRECTED_ECC_ERRf_GET(edoi_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, EGR_DOI_CELL_QUEUE_1_UNCORRECTED_ECC_ERR);
            }
            if (EGR_DOI_INTR_STATUSr_SLOT_PIPELINE_1_UNCORRECTED_ECC_ERRf_GET(edoi_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, EGR_DOI_SLOT_PIPELINE_1_UNCORRECTED_ECC_ERR);
            }
        }

        if (EP_TO_CMIC_INTR_STATUSr_EP3f_GET(ep2c_st)) {
            if (READ_EPOST_EGR_INTR_STATUSr(unit, inst, &eei_st) != 0) {
                return;
            }
            if (EPOST_EGR_INTR_STATUSr_SER_FIFO_NON_EMPTYf_GET(eei_st)) {
                SHR_BITSET(ipep_stat[inst].bmp, EPOST_EGR_SER_FIFO_NON_EMPTY);
            }
        }
        if (EP_TO_CMIC_INTR_STATUSr_RDBf_GET(ep2c_st)) {
            SHR_BITSET(ipep_stat[inst].bmp, EP_TO_CMIC_RDB);
        }
    }

    inst = 0;
    /* Processing TOP level DLB_ECMP_INTR_STATUS */
    if (READ_DLB_ECMP_INTR_STATUSr(unit, &dlb_st) != 0) {
        return;
    }

    if (DLB_ECMP_INTR_STATUSr_SER_FIFO_NON_EMPTYf_GET(dlb_st)) {
        SHR_BITSET(ipep_stat[inst].bmp, DLB_ECMP_INTR);
    }

    /* Processing TOP level CENTRAL_CTR_EVICTION_INTR_STATUS */
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

    /* Now call interrupt handlers according to the interrupt source matrix */
    for (inst = 0; inst < MAX_INST; inst++) {
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
}

static void
ipep_thread(shr_thread_t tc, void *arg)
{
    int unit = (uintptr_t)arg;

    while (1) {
        shr_thread_sleep(tc, SHR_THREAD_FOREVER);
        if (shr_thread_stopping(tc)) {
            break;
        }
        ipep_intr(unit);
        bcmbd_cmicx_lp_intr_enable(unit, CE_TO_CMIC_INTR);
        bcmbd_cmicx_lp_intr_enable(unit, DLB_CREDIT_TO_CMIC_INTR);
        bcmbd_cmicx_lp_intr_enable(unit, PIPE0_EP_TO_CMIC_INTR);
        bcmbd_cmicx_lp_intr_enable(unit, PIPE0_IP_TO_CMIC_INTR);
        bcmbd_cmicx_lp_intr_enable(unit, PIPE1_EP_TO_CMIC_INTR);
        bcmbd_cmicx_lp_intr_enable(unit, PIPE1_IP_TO_CMIC_INTR);
    }
}

static void
ipep_isr(int unit, uint32_t param)
{
    /* Disable interrupt and wake up IPEP thread */
    bcmbd_cmicx_lp_intr_disable_nosync(unit, CE_TO_CMIC_INTR);
    bcmbd_cmicx_lp_intr_disable_nosync(unit, DLB_CREDIT_TO_CMIC_INTR);
    bcmbd_cmicx_lp_intr_disable_nosync(unit, PIPE0_EP_TO_CMIC_INTR);
    bcmbd_cmicx_lp_intr_disable_nosync(unit, PIPE0_IP_TO_CMIC_INTR);
    bcmbd_cmicx_lp_intr_disable_nosync(unit, PIPE1_EP_TO_CMIC_INTR);
    bcmbd_cmicx_lp_intr_disable_nosync(unit, PIPE1_IP_TO_CMIC_INTR);
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
    int rv = SHR_E_NONE;

    bcmbd_cmicx_lp_intr_disable(unit, CE_TO_CMIC_INTR);
    bcmbd_cmicx_lp_intr_disable(unit, DLB_CREDIT_TO_CMIC_INTR);
    bcmbd_cmicx_lp_intr_disable(unit, PIPE0_EP_TO_CMIC_INTR);
    bcmbd_cmicx_lp_intr_disable(unit, PIPE0_IP_TO_CMIC_INTR);
    bcmbd_cmicx_lp_intr_disable(unit, PIPE1_EP_TO_CMIC_INTR);
    bcmbd_cmicx_lp_intr_disable(unit, PIPE1_IP_TO_CMIC_INTR);

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
    bcmbd_intr_vect_t *iv;

    if (!ipep_intr_connected[unit]) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Add handler for IPEP interrupt %u\n"),
                   intr_num));
        bcmbd_cmicx_lp_intr_func_set(unit, CE_TO_CMIC_INTR, ipep_isr, 0);
        bcmbd_cmicx_lp_intr_func_set(unit, DLB_CREDIT_TO_CMIC_INTR, ipep_isr, 0);
        bcmbd_cmicx_lp_intr_func_set(unit, PIPE0_EP_TO_CMIC_INTR, ipep_isr, 0);
        bcmbd_cmicx_lp_intr_func_set(unit, PIPE0_IP_TO_CMIC_INTR, ipep_isr, 0);
        bcmbd_cmicx_lp_intr_func_set(unit, PIPE1_EP_TO_CMIC_INTR, ipep_isr, 0);
        bcmbd_cmicx_lp_intr_func_set(unit, PIPE1_IP_TO_CMIC_INTR, ipep_isr, 0);

        bcmbd_cmicx_lp_intr_enable(unit, CE_TO_CMIC_INTR);
        bcmbd_cmicx_lp_intr_enable(unit, DLB_CREDIT_TO_CMIC_INTR);
        bcmbd_cmicx_lp_intr_enable(unit, PIPE0_EP_TO_CMIC_INTR);
        bcmbd_cmicx_lp_intr_enable(unit, PIPE0_IP_TO_CMIC_INTR);
        bcmbd_cmicx_lp_intr_enable(unit, PIPE1_EP_TO_CMIC_INTR);
        bcmbd_cmicx_lp_intr_enable(unit, PIPE1_IP_TO_CMIC_INTR);
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

/*******************************************************************************
 * Public functions
 */
int
bcm56780_a0_bd_ipep_intr_drv_init(int unit)
{
    int rv;

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
bcm56780_a0_bd_ipep_intr_drv_cleanup(int unit)
{
    (void)bcmbd_ipep_intr_drv_init(unit, NULL);
    return ipep_intr_cleanup(unit);
}
