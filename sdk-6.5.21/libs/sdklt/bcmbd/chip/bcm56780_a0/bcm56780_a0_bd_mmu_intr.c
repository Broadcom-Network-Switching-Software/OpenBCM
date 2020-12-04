/*! \file bcm56780_a0_bd_mmu_intr.c
 *
 * The MMU interrupts are daisy-chained on a single iProc hardware
 * interrupt, which means that the hardware interrupt handler must
 * inspect additional registers to determine the actual source(s) of
 * the interrupt.
 *
 * The top level register is:
 *
 *  - MMU_GLBCFG_CPU_INT_STAT
 *
 * This register contains primary (leaf) interrupt sources as well as
 * secondary (daisy-chained) interrupt sources. The secondary sources
 * span 10 registers, which are:
 *
 *  - MMU_EBCFG_CPU_INT_STAT (8 register instances)
 *  - MMU_ITMCFG_CPU_INT_STAT (2 register instances)
 *
 * These second level registers also contain both primary (leaf) and
 * secondary (daisy-chained) interrupt sources. The secondary
 * interrupt sources require that one of the following registers is
 * read to obtain the primary interrupt source:
 *
 *  - MMU_EBTOQ_CPU_INT_STAT (EB)
 *  - MMU_EBQS_CPU_INT_STAT (EB)
 *  - MMU_EBCTM_CPU_INT_STAT (EB)
 *  - MMU_EBCFP_CPU_INT_STAT (EB)
 *  - MMU_EBPCC_CPU_INT_STAT (EB)
 *  - MMU_MTRO_CPU_INT_STAT (EB)
 *  - MMU_INTFI_CPU_INT_STAT (EB)
 *  - MMU_CRB_CPU_INT_STAT (ITM)
 *  - MMU_TOQ_CPU_INT_STAT (ITM)
 *  - MMU_THDI_CPU_INT_STAT (ITM)
 *  - MMU_THDR_QE_CPU_INT_STAT (ITM)
 *  - MMU_CFAP_INT_STAT (ITM)
 *  - MMU_SCB_CPU_INT_STAT (ITM)
 *  - MMU_OQS_CPU_INT_STAT (ITM)
 *  - MMU_RQE_INT_STAT (ITM)
 *  - MMU_THDO_BST_CPU_INT_STAT (ITM)
 *  - MMU_THDO_CPU_INT_STAT (ITM)
 *  - MMU_RL_CPU_INT_STAT (ITM)
 *
 * Note that both the MMU interrupt status registers and the top-level
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

#include <bcmbd/bcmbd_mmu_intr_internal.h>
#include <bcmbd/bcmbd_cmicx_intr.h>
#include <bcmbd/chip/bcm56780_a0_acc.h>
#include <bcmbd/chip/bcm56780_a0_lp_intr.h>
#include <bcmbd/chip/bcm56780_a0_mmu_intr.h>

#include "bcm56780_a0_drv.h"

#define  BSL_LOG_MODULE  BSL_LS_BCMBD_INTR

/*******************************************************************************
 * Local definitions
 */

#define MAX_UNITS       BCMDRD_CONFIG_MAX_UNITS
#define MAX_INTRS       MAX_MMU_INTR
#define MAX_INST        4
#define NUM_EB_INST     4
#define NUM_ITM_INST    1
#define TD4_X9_PIPES_PER_DEV 4

#define CMIC_MMU_INTR   MMU_TO_CMIC_INTR

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

#define INTR_MASK_LOCK(_u) sal_spinlock_lock(mmu_intr_lock[_u]);
#define INTR_MASK_UNLOCK(_u) sal_spinlock_unlock(mmu_intr_lock[_u]);

/*******************************************************************************
 * Local data
 */

static bool mmu_intr_connected[MAX_UNITS];
static bcmbd_intr_map_t mmu_intr_mask[MAX_UNITS];
static sal_spinlock_t mmu_intr_lock[MAX_UNITS];
static bcmbd_intr_vect_info_t mmu_intr_vect_info[MAX_UNITS];
static shr_thread_t mmu_thread_ctrl[MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

static void
mmu_intr_mask32_set(bcmbd_intr_map_t *intr_map, uint32_t mask,
                    int base, int num_bits)
{
    int idx;

    for (idx = 0; idx < num_bits; idx++) {
        if (mask & (1 << idx)) {
            SHR_BITSET(intr_map->bmp, base + idx);
        }
    }
}

static uint32_t
mmu_intr_mask32_get(bcmbd_intr_map_t *intr_map,
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
mmu_intr_update(int unit, unsigned int intr_num, int enable)
{
    int ioerr = 0;
    MMU_GLBCFG_CPU_INT_ENr_t glb_en;
    MMU_EBCFG_CPU_INT_ENr_t eb_en;
    MMU_ITMCFG_CPU_INT_ENr_t itm_en;
    MMU_EBTOQ_CPU_INT_ENr_t ebtoq_en;
    MMU_THDR_QE_CPU_INT_ENr_t thdr_en;
    MMU_CFAP_INT_ENr_t cfap_en;
    MMU_THDO_BST_CPU_INT_ENr_t thdo_bst_en;
    MMU_THDO_CPU_INT_ENr_t thdo_en;
    MMU_THDI_CPU_INT_ENr_t thdi_en;
    bool glb_eb_upd = false;
    bool glb_itm_upd = false;
    uint32_t intr_mask;

    /* We may be called by multiple application threads */
    INTR_MASK_LOCK(unit);

    /* Start with leaf interrupts and then do daisy-chained interrupts */
    if (intr_num == MMU_INTR_EB_MEM_PAR_ERR) {
        /* Enable/disable leaf interrupt */
        ioerr += READ_MMU_EBCFG_CPU_INT_ENr(unit, &eb_en);
        MMU_EBCFG_CPU_INT_ENr_MEM_PAR_ERR_INT_ENf_SET(eb_en, enable);
        ioerr += WRITE_MMU_EBCFG_CPU_INT_ENr(unit, eb_en);
        glb_eb_upd = true;
    } else if (intr_num == MMU_INTR_ITM_MEM_PAR_ERR) {
        /* Enable/disable leaf interrupt */
        ioerr += READ_MMU_ITMCFG_CPU_INT_ENr(unit, &itm_en);
        MMU_ITMCFG_CPU_INT_ENr_MEM_PAR_ERR_INT_ENf_SET(itm_en, enable);
        ioerr += WRITE_MMU_ITMCFG_CPU_INT_ENr(unit, itm_en);
        glb_itm_upd = true;
    } else if (intr_num >= THDI_BIT_BASE) {
        /* Enable/disable leaf interrupt */
        intr_mask = mmu_intr_mask32_get(&mmu_intr_mask[unit],
                                        THDI_BIT_BASE, THDI_NUM_BITS);
        MMU_THDI_CPU_INT_ENr_SET(thdi_en, intr_mask);
        ioerr += WRITE_MMU_THDI_CPU_INT_ENr(unit, thdi_en);
        /* Enable/disable parent interrupt if needed */
        if (enable || intr_mask == 0) {
            ioerr += READ_MMU_ITMCFG_CPU_INT_ENr(unit, &itm_en);
            MMU_ITMCFG_CPU_INT_ENr_THDI_INT_ENf_SET(itm_en, enable);
            ioerr += WRITE_MMU_ITMCFG_CPU_INT_ENr(unit, itm_en);
            glb_itm_upd = true;
        }
    } else if (intr_num >= THDO_BIT_BASE) {
        /* Enable/disable leaf interrupt */
        intr_mask = mmu_intr_mask32_get(&mmu_intr_mask[unit],
                                        THDO_BIT_BASE, THDO_NUM_BITS);
        MMU_THDO_CPU_INT_ENr_SET(thdo_en, intr_mask);
        ioerr += WRITE_MMU_THDO_CPU_INT_ENr(unit, thdo_en);
        /* Enable/disable parent interrupt if needed */
        if (enable || intr_mask == 0) {
            ioerr += READ_MMU_ITMCFG_CPU_INT_ENr(unit, &itm_en);
            MMU_ITMCFG_CPU_INT_ENr_THDO_INT_ENf_SET(itm_en, enable);
            ioerr += WRITE_MMU_ITMCFG_CPU_INT_ENr(unit, itm_en);
            glb_itm_upd = true;
        }
    } else if (intr_num >= THDO_BST_BIT_BASE) {
        /* Enable/disable leaf interrupt */
        intr_mask = mmu_intr_mask32_get(&mmu_intr_mask[unit],
                                        THDO_BST_BIT_BASE, THDO_BST_NUM_BITS);
        MMU_THDO_BST_CPU_INT_ENr_SET(thdo_bst_en, intr_mask);
        ioerr += WRITE_MMU_THDO_BST_CPU_INT_ENr(unit, thdo_bst_en);
        /* Enable/disable parent interrupt if needed */
        if (enable || intr_mask == 0) {
            /* This register shares the THDO bit in the parent */
            ioerr += READ_MMU_ITMCFG_CPU_INT_ENr(unit, &itm_en);
            MMU_ITMCFG_CPU_INT_ENr_THDO_INT_ENf_SET(itm_en, enable);
            ioerr += WRITE_MMU_ITMCFG_CPU_INT_ENr(unit, itm_en);
            glb_itm_upd = true;
        }
    } else if (intr_num >= CFAP_BIT_BASE) {
        /* Enable/disable leaf interrupt */
        intr_mask = mmu_intr_mask32_get(&mmu_intr_mask[unit],
                                        CFAP_BIT_BASE, CFAP_NUM_BITS);
        /* This is a 64-bit register using only the lower 32 bits */
        MMU_CFAP_INT_ENr_CLR(cfap_en);
        MMU_CFAP_INT_ENr_SET(cfap_en, 0, intr_mask);
        ioerr += WRITE_MMU_CFAP_INT_ENr(unit, cfap_en);
        /* Enable/disable parent interrupt if needed */
        if (enable || intr_mask == 0) {
            ioerr += READ_MMU_ITMCFG_CPU_INT_ENr(unit, &itm_en);
            MMU_ITMCFG_CPU_INT_ENr_CFAP_INT_ENf_SET(itm_en, enable);
            ioerr += WRITE_MMU_ITMCFG_CPU_INT_ENr(unit, itm_en);
            glb_itm_upd = true;
        }
    } else if (intr_num >= THDR_BIT_BASE) {
        /* Enable/disable leaf interrupt */
        intr_mask = mmu_intr_mask32_get(&mmu_intr_mask[unit],
                                        THDR_BIT_BASE, THDR_NUM_BITS);
        MMU_THDR_QE_CPU_INT_ENr_SET(thdr_en, intr_mask);
        ioerr += WRITE_MMU_THDR_QE_CPU_INT_ENr(unit, thdr_en);
        /* Enable/disable parent interrupt if needed */
        if (enable || intr_mask == 0) {
            ioerr += READ_MMU_ITMCFG_CPU_INT_ENr(unit, &itm_en);
            MMU_ITMCFG_CPU_INT_ENr_THDR_INT_ENf_SET(itm_en, enable);
            ioerr += WRITE_MMU_ITMCFG_CPU_INT_ENr(unit, itm_en);
            glb_itm_upd = true;
        }
    } else if (intr_num >= EBTOQ_BIT_BASE) {
        /* Enable/disable leaf interrupt */
        intr_mask = mmu_intr_mask32_get(&mmu_intr_mask[unit],
                                        EBTOQ_BIT_BASE, EBTOQ_NUM_BITS);
        MMU_EBTOQ_CPU_INT_ENr_SET(ebtoq_en, intr_mask);
        ioerr += WRITE_MMU_EBTOQ_CPU_INT_ENr(unit, ebtoq_en);
        /* Enable/disable parent interrupt if needed */
        if (enable || intr_mask == 0) {
            ioerr += READ_MMU_EBCFG_CPU_INT_ENr(unit, &eb_en);
            MMU_EBCFG_CPU_INT_ENr_EBTOQ_INT_ENf_SET(eb_en, enable);
            ioerr += WRITE_MMU_EBCFG_CPU_INT_ENr(unit, eb_en);
            glb_eb_upd = true;
        }
    }

    /* Start with leaf interrupts and then do daisy-chained interrupts */
    ioerr += READ_MMU_GLBCFG_CPU_INT_ENr(unit, &glb_en);
    if (intr_num == MMU_INTR_GLB_MEM_PAR_ERR) {
        MMU_GLBCFG_CPU_INT_ENr_MEM_PAR_ERR_INT_ENf_SET(glb_en, enable);
    } else if (intr_num == MMU_INTR_GLB_PPSCH) {
        MMU_GLBCFG_CPU_INT_ENr_PPSCH_INT_ENf_SET(glb_en, enable);
    } else if (intr_num == MMU_INTR_GLB_INTFO) {
        MMU_GLBCFG_CPU_INT_ENr_INTFO_INT_ENf_SET(glb_en, enable);
    } else if (intr_num == MMU_INTR_GLB_TS_OVERFLOW) {
        MMU_GLBCFG_CPU_INT_ENr_TS_OVERFLOW_INT_ENf_SET(glb_en, enable);
    } else if (intr_num == MMU_INTR_GLB_TS_UNDERFLOW) {
        MMU_GLBCFG_CPU_INT_ENr_TS_UNDERFLOW_INT_ENf_SET(glb_en, enable);
    } else if (glb_eb_upd) {
        /*
         * If any of the EBCFG interrupts are enabled, then we need to
         * enable all EBCFG instances in the top-level register.
         *
         * Note that the EBCFG interrupt enable register is NOT
         * instantiated, i.e. either all or none of the interrupt
         * instances are enabled.
         */
        ioerr += READ_MMU_EBCFG_CPU_INT_ENr(unit, &eb_en);
        enable = MMU_EBCFG_CPU_INT_ENr_GET(eb_en) ? 1 : 0;
        MMU_GLBCFG_CPU_INT_ENr_EBCFG0_INT_ENf_SET(glb_en, enable);
        MMU_GLBCFG_CPU_INT_ENr_EBCFG1_INT_ENf_SET(glb_en, enable);
        MMU_GLBCFG_CPU_INT_ENr_EBCFG2_INT_ENf_SET(glb_en, enable);
        MMU_GLBCFG_CPU_INT_ENr_EBCFG3_INT_ENf_SET(glb_en, enable);
        MMU_GLBCFG_CPU_INT_ENr_EBCFG4_INT_ENf_SET(glb_en, enable);
        MMU_GLBCFG_CPU_INT_ENr_EBCFG5_INT_ENf_SET(glb_en, enable);
        MMU_GLBCFG_CPU_INT_ENr_EBCFG6_INT_ENf_SET(glb_en, enable);
        MMU_GLBCFG_CPU_INT_ENr_EBCFG7_INT_ENf_SET(glb_en, enable);
    } else if (glb_itm_upd) {
        /*
         * If any of the ITMCFG interrupts are enabled, then we need
         * to enable all ITMCFG instances in the top-level register.
         *
         * Note that the ITMCFG interrupt enable register is NOT
         * instantiated, i.e. either all or none of the interrupt
         * instances are enabled.
         */
        ioerr += READ_MMU_ITMCFG_CPU_INT_ENr(unit, &itm_en);
        enable = MMU_ITMCFG_CPU_INT_ENr_GET(itm_en) ? 1 : 0;
        MMU_GLBCFG_CPU_INT_ENr_ITMCFG0_INT_ENf_SET(glb_en, enable);
        MMU_GLBCFG_CPU_INT_ENr_ITMCFG1_INT_ENf_SET(glb_en, enable);
    }
    ioerr += WRITE_MMU_GLBCFG_CPU_INT_ENr(unit, glb_en);

    if (ioerr) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "MMU interupt %u I/O error\n"),
                   intr_num));
    }

    INTR_MASK_UNLOCK(unit);
}

static void
mmu_intr_enable(int unit, unsigned int intr_num)
{
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                          "Enable MMU intr %u\n"),
                 intr_num));
    if (intr_num >= MAX_INTRS) {
        return;
    }
    SHR_BITSET(mmu_intr_mask[unit].bmp, intr_num);
    mmu_intr_update(unit, intr_num, 1);
}

static void
mmu_intr_disable(int unit, unsigned int intr_num)
{
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                          "Disable MMU intr %u\n"),
                 intr_num));
    if (intr_num >= MAX_INTRS) {
        return;
    }
    SHR_BITCLR(mmu_intr_mask[unit].bmp, intr_num);
    mmu_intr_update(unit, intr_num, 0);
}

static void
mmu_intr_clear(int unit, unsigned int intr_num)
{
    int ioerr = 0;
    int idx;
    MMU_GLBCFG_CPU_INT_STATr_t glb_stat;
    MMU_EBCFG_CPU_INT_STATr_t eb_stat;
    MMU_ITMCFG_CPU_INT_STATr_t itm_stat;
    MMU_EBTOQ_CPU_INT_STATr_t ebtoq_stat;
    MMU_THDR_QE_CPU_INT_STATr_t thdr_stat;
    MMU_CFAP_INT_STATr_t cfap_stat;
    MMU_THDO_BST_CPU_INT_STATr_t thdo_bst_stat;
    MMU_THDO_CPU_INT_STATr_t thdo_stat;
    MMU_THDI_CPU_INT_STATr_t thdi_stat;
    MMU_THDO_BST_STATr_t thdo_bst_data;
    MMU_THDI_BST_TRIGGER_STATUS_TYPEr_t thdi_bst_stat;

    bool glb_eb_upd = false;
    bool glb_itm_upd = false;
    uint32_t intr_mask;
    uint32_t intr_inst;
    uint32_t pipe_map;
    int rv;

    intr_inst = MMU_INTR_INST_GET(intr_num);
    intr_num = MMU_INTR_NUM_GET(intr_num);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                          "Clear MMU intr %u (%u)\n"),
                 intr_num, intr_inst));
    if (intr_num >= MAX_INTRS) {
        return;
    }

    rv = bcmdrd_dev_valid_pipes_get(unit, &pipe_map);
    if (SHR_FAILURE(rv)) {
        return;
    }

    /* Start with leaf interrupts and then do daisy-chained interrupts */
    if (intr_num == MMU_INTR_EB_MEM_PAR_ERR) {
        /* Clear EBCFG interrupt instance */
        MMU_EBCFG_CPU_INT_STATr_CLR(eb_stat);
        MMU_EBCFG_CPU_INT_STATr_MEM_PAR_ERR_INT_STATf_SET(eb_stat, 1);
        ioerr += WRITE_MMU_EBCFG_CPU_INT_STATr(unit, intr_inst, eb_stat);
        /* Request top-level interrupt to be cleared */
        glb_eb_upd = true;
    } else if (intr_num == MMU_INTR_ITM_MEM_PAR_ERR) {
        /* Clear ITMCFG interrupt instance */
        MMU_ITMCFG_CPU_INT_STATr_CLR(itm_stat);
        MMU_ITMCFG_CPU_INT_STATr_MEM_PAR_ERR_INT_STATf_SET(itm_stat, 1);
        ioerr += WRITE_MMU_ITMCFG_CPU_INT_STATr(unit, itm_stat);
        /* Request top-level interrupt to be cleared */
        glb_itm_upd = true;
    } else if (intr_num >= THDI_BIT_BASE) {
        /* clear bst interrupt status for THDI */
        if (intr_num == MMU_INTR_THDI_BST) {
            MMU_THDI_BST_TRIGGER_STATUS_TYPEr_CLR(thdi_bst_stat);
            for (idx = 0; idx < TD4_X9_PIPES_PER_DEV; idx++) {
                if (pipe_map & (1 << idx)) {
                    ioerr +=
                        WRITE_MMU_THDI_BST_TRIGGER_STATUS_TYPEr(unit, idx,
                                            thdi_bst_stat);
                }
            }
        }
        /* Clear leaf interrupt instance */
        intr_mask = 1 << (intr_num - THDI_BIT_BASE);
        MMU_THDI_CPU_INT_STATr_SET(thdi_stat, intr_mask);
        ioerr += WRITE_MMU_THDI_CPU_INT_STATr(unit, thdi_stat);
        /* Then attempt to clear interrupt in parent register */
        MMU_ITMCFG_CPU_INT_STATr_CLR(itm_stat);
        MMU_ITMCFG_CPU_INT_STATr_THDI_INT_STATf_SET(itm_stat, 1);
        ioerr += WRITE_MMU_ITMCFG_CPU_INT_STATr(unit, itm_stat);
        /* Request top-level interrupt to be cleared */
        glb_itm_upd = true;
    } else if (intr_num >= THDO_BIT_BASE) {
        /* Clear leaf interrupt instance */
        intr_mask = 1 << (intr_num - THDO_BIT_BASE);
        MMU_THDO_CPU_INT_STATr_SET(thdo_stat, intr_mask);
        ioerr += WRITE_MMU_THDO_CPU_INT_STATr(unit, thdo_stat);
        /* Then attempt to clear interrupt in parent register */
        MMU_ITMCFG_CPU_INT_STATr_CLR(itm_stat);
        MMU_ITMCFG_CPU_INT_STATr_THDO_INT_STATf_SET(itm_stat, 1);
        ioerr += WRITE_MMU_ITMCFG_CPU_INT_STATr(unit,itm_stat);
        /* Request top-level interrupt to be cleared */
        glb_itm_upd = true;
    } else if (intr_num >= THDO_BST_BIT_BASE) {
        if ( intr_num == MMU_INTR_THDO_BST) {
            MMU_THDO_BST_STATr_CLR(thdo_bst_data);
            ioerr += WRITE_MMU_THDO_BST_STATr(unit, thdo_bst_data);
        }
        /* Clear leaf interrupt instance */
        intr_mask = 1 << (intr_num - THDO_BST_BIT_BASE);
        MMU_THDO_BST_CPU_INT_STATr_SET(thdo_bst_stat, intr_mask);
        ioerr += WRITE_MMU_THDO_BST_CPU_INT_STATr(unit, thdo_bst_stat);
        /* Then attempt to clear interrupt in parent register */
        MMU_ITMCFG_CPU_INT_STATr_CLR(itm_stat);
        MMU_ITMCFG_CPU_INT_STATr_THDO_INT_STATf_SET(itm_stat, 1);
        ioerr += WRITE_MMU_ITMCFG_CPU_INT_STATr(unit, itm_stat);
        /* Request top-level interrupt to be cleared */
        glb_itm_upd = true;
    } else if (intr_num >= CFAP_BIT_BASE) {
        /* Clear leaf interrupt instance */
        intr_mask = 1 << (intr_num - CFAP_BIT_BASE);
        /* This is a 64-bit register using only the lower 32 bits */
        MMU_CFAP_INT_STATr_CLR(cfap_stat);
        MMU_CFAP_INT_STATr_SET(cfap_stat, 0, intr_mask);
        ioerr += WRITE_MMU_CFAP_INT_STATr(unit, cfap_stat);
        /* Then attempt to clear interrupt in parent register */
        MMU_ITMCFG_CPU_INT_STATr_CLR(itm_stat);
        MMU_ITMCFG_CPU_INT_STATr_CFAP_INT_STATf_SET(itm_stat, 1);
        ioerr += WRITE_MMU_ITMCFG_CPU_INT_STATr(unit, itm_stat);
        /* Request top-level interrupt to be cleared */
        glb_itm_upd = true;
    } else if (intr_num >= THDR_BIT_BASE) {
        /* Clear leaf interrupt instance */
        intr_mask = 1 << (intr_num - THDR_BIT_BASE);
        MMU_THDR_QE_CPU_INT_STATr_SET(thdr_stat, intr_mask);
        ioerr += WRITE_MMU_THDR_QE_CPU_INT_STATr(unit,  thdr_stat);
        /* Then attempt to clear interrupt in parent register */
        MMU_ITMCFG_CPU_INT_STATr_CLR(itm_stat);
        MMU_ITMCFG_CPU_INT_STATr_THDR_INT_STATf_SET(itm_stat, 1);
        ioerr += WRITE_MMU_ITMCFG_CPU_INT_STATr(unit,  itm_stat);
        /* Request top-level interrupt to be cleared */
        glb_itm_upd = true;
    } else if (intr_num >= EBTOQ_BIT_BASE) {
        /* Clear leaf interrupt instance */
        intr_mask = 1 << (intr_num - EBTOQ_BIT_BASE);
        MMU_EBTOQ_CPU_INT_STATr_SET(ebtoq_stat, intr_mask);
        ioerr += WRITE_MMU_EBTOQ_CPU_INT_STATr(unit, intr_inst, ebtoq_stat);
        /* Then attempt to clear interrupt in parent register */
        MMU_EBCFG_CPU_INT_STATr_CLR(eb_stat);
        MMU_EBCFG_CPU_INT_STATr_EBTOQ_INT_STATf_SET(eb_stat, 1);
        ioerr += WRITE_MMU_EBCFG_CPU_INT_STATr(unit, intr_inst, eb_stat);
        /* Request top-level interrupt to be cleared */
        glb_eb_upd = true;
    }

    /* Start with leaf interrupts and then do daisy-chained interrupts */
    MMU_GLBCFG_CPU_INT_STATr_CLR(glb_stat);
    if (intr_num == MMU_INTR_GLB_MEM_PAR_ERR) {
        MMU_GLBCFG_CPU_INT_STATr_MEM_PAR_ERR_INT_STATf_SET(glb_stat, 1);
    } else if (intr_num == MMU_INTR_GLB_PPSCH) {
        MMU_GLBCFG_CPU_INT_STATr_PPSCH_INT_STATf_SET(glb_stat, 1);
    } else if (intr_num == MMU_INTR_GLB_INTFO) {
        MMU_GLBCFG_CPU_INT_STATr_INTFO_INT_STATf_SET(glb_stat, 1);
    } else if (intr_num == MMU_INTR_GLB_TS_OVERFLOW) {
        MMU_GLBCFG_CPU_INT_STATr_TS_OVERFLOW_INT_STATf_SET(glb_stat, 1);
    } else if (intr_num == MMU_INTR_GLB_TS_UNDERFLOW) {
        MMU_GLBCFG_CPU_INT_STATr_TS_UNDERFLOW_INT_STATf_SET(glb_stat, 1);
    } else if (glb_eb_upd) {
        if (intr_inst == 0) {
            MMU_GLBCFG_CPU_INT_STATr_EBCFG0_INT_STATf_SET(glb_stat, 1);
        } else if (intr_inst == 1) {
            MMU_GLBCFG_CPU_INT_STATr_EBCFG1_INT_STATf_SET(glb_stat, 1);
        } else if (intr_inst == 2) {
            MMU_GLBCFG_CPU_INT_STATr_EBCFG2_INT_STATf_SET(glb_stat, 1);
        } else if (intr_inst == 3) {
            MMU_GLBCFG_CPU_INT_STATr_EBCFG3_INT_STATf_SET(glb_stat, 1);
        } else if (intr_inst == 4) {
            MMU_GLBCFG_CPU_INT_STATr_EBCFG4_INT_STATf_SET(glb_stat, 1);
        } else if (intr_inst == 5) {
            MMU_GLBCFG_CPU_INT_STATr_EBCFG5_INT_STATf_SET(glb_stat, 1);
        } else if (intr_inst == 6) {
            MMU_GLBCFG_CPU_INT_STATr_EBCFG6_INT_STATf_SET(glb_stat, 1);
        } else if (intr_inst == 7) {
            MMU_GLBCFG_CPU_INT_STATr_EBCFG7_INT_STATf_SET(glb_stat, 1);
        }
    } else if (glb_itm_upd) {
        if (intr_inst == 0) {
            MMU_GLBCFG_CPU_INT_STATr_ITMCFG0_INT_STATf_SET(glb_stat, 1);
        } else if (intr_inst == 1) {
            MMU_GLBCFG_CPU_INT_STATr_ITMCFG1_INT_STATf_SET(glb_stat, 1);
        }
    }
    ioerr += WRITE_MMU_GLBCFG_CPU_INT_STATr(unit, glb_stat);
}

static void
mmu_intr(int unit)
{
    MMU_GLBCFG_CPU_INT_STATr_t glb_stat;
    MMU_EBCFG_CPU_INT_STATr_t eb_stat;
    MMU_ITMCFG_CPU_INT_STATr_t itm_stat;
    MMU_EBTOQ_CPU_INT_STATr_t ebtoq_stat;
    MMU_THDR_QE_CPU_INT_STATr_t thdr_stat;
    MMU_CFAP_INT_STATr_t cfap_stat;
    MMU_THDO_BST_CPU_INT_STATr_t thdo_bst_stat;
    MMU_THDO_CPU_INT_STATr_t thdo_stat;
    MMU_THDI_CPU_INT_STATr_t thdi_stat;
    bcmbd_intr_map_t mmu_stat[MAX_INST];
    bcmbd_intr_vect_t *iv;
    uint32_t inst_mask;
    uint32_t intr_stat;
    unsigned int inst;
    unsigned int intr_num;

    /* Start with an empty interrupt source matrix */
    sal_memset(mmu_stat, 0, sizeof(mmu_stat));

    /* Read top-level register to figure out what to do next */
    if (READ_MMU_GLBCFG_CPU_INT_STATr(unit, &glb_stat) != 0) {
        return;
    }

    /* Check top-level leaf interrupts (one instance only) */
    inst = 0;
    if (MMU_GLBCFG_CPU_INT_STATr_MEM_PAR_ERR_INT_STATf_GET(glb_stat)) {
        SHR_BITSET(mmu_stat[inst].bmp, MMU_INTR_GLB_MEM_PAR_ERR);
    } else if (MMU_GLBCFG_CPU_INT_STATr_PPSCH_INT_STATf_GET(glb_stat)) {
        SHR_BITSET(mmu_stat[inst].bmp, MMU_INTR_GLB_PPSCH);
    } else if (MMU_GLBCFG_CPU_INT_STATr_INTFO_INT_STATf_GET(glb_stat)) {
        SHR_BITSET(mmu_stat[inst].bmp, MMU_INTR_GLB_INTFO);
    } else if (MMU_GLBCFG_CPU_INT_STATr_TS_OVERFLOW_INT_STATf_GET(glb_stat)) {
        SHR_BITSET(mmu_stat[inst].bmp, MMU_INTR_GLB_TS_OVERFLOW);
    } else if (MMU_GLBCFG_CPU_INT_STATr_TS_UNDERFLOW_INT_STATf_GET(glb_stat)) {
        SHR_BITSET(mmu_stat[inst].bmp, MMU_INTR_GLB_TS_UNDERFLOW);
    }

    /* Determine which EB register instances have pending interrupts */
    inst_mask = 0;
    if (MMU_GLBCFG_CPU_INT_STATr_EBCFG0_INT_STATf_GET(glb_stat)) {
        inst_mask |= (1 << 0);
    }
    if (MMU_GLBCFG_CPU_INT_STATr_EBCFG1_INT_STATf_GET(glb_stat)) {
        inst_mask |= (1 << 1);
    }
    if (MMU_GLBCFG_CPU_INT_STATr_EBCFG2_INT_STATf_GET(glb_stat)) {
        inst_mask |= (1 << 2);
    }
    if (MMU_GLBCFG_CPU_INT_STATr_EBCFG3_INT_STATf_GET(glb_stat)) {
        inst_mask |= (1 << 3);
    }
    if (MMU_GLBCFG_CPU_INT_STATr_EBCFG4_INT_STATf_GET(glb_stat)) {
        inst_mask |= (1 << 4);
    }
    if (MMU_GLBCFG_CPU_INT_STATr_EBCFG5_INT_STATf_GET(glb_stat)) {
        inst_mask |= (1 << 5);
    }
    if (MMU_GLBCFG_CPU_INT_STATr_EBCFG6_INT_STATf_GET(glb_stat)) {
        inst_mask |= (1 << 6);
    }
    if (MMU_GLBCFG_CPU_INT_STATr_EBCFG7_INT_STATf_GET(glb_stat)) {
        inst_mask |= (1 << 7);
    }
    /* Read EB register instances with pending interrupts */
    for (inst = 0; inst_mask && inst < NUM_EB_INST; inst++) {
        if ((inst_mask & (1 << inst)) == 0) {
            /* This instance has no pending interrupts */
            continue;
        }
        if (READ_MMU_EBCFG_CPU_INT_STATr(unit, inst, &eb_stat) != 0) {
            return;
        }
        /* Add EB interrupt status to interrupt source matrix */
        if (MMU_EBCFG_CPU_INT_STATr_MEM_PAR_ERR_INT_STATf_GET(eb_stat)) {
            SHR_BITSET(mmu_stat[inst].bmp, MMU_INTR_EB_MEM_PAR_ERR);
        }
        if (MMU_EBCFG_CPU_INT_STATr_EBTOQ_INT_STATf_GET(eb_stat)) {
            if (READ_MMU_EBTOQ_CPU_INT_STATr(unit, inst, &ebtoq_stat) != 0) {
                return;
            }
            intr_stat = MMU_EBTOQ_CPU_INT_STATr_GET(ebtoq_stat);
            mmu_intr_mask32_set(&mmu_stat[inst], intr_stat,
                                EBTOQ_BIT_BASE, EBTOQ_NUM_BITS);
        }
    }

    /* Determine which ITM register instances have pending interrupts */
    inst_mask = 0;
    if (MMU_GLBCFG_CPU_INT_STATr_ITMCFG0_INT_STATf_GET(glb_stat)) {
        inst_mask |= (1 << 0);
    }
    if (MMU_GLBCFG_CPU_INT_STATr_ITMCFG1_INT_STATf_GET(glb_stat)) {
        inst_mask |= (1 << 1);
    }

    /* Read ITM register instances with pending interrupts */
    for (inst = 0; inst_mask && inst < NUM_ITM_INST; inst++) {
        if ((inst_mask & (1 << inst)) == 0) {
            /* This instance has no pending interrupts */
            continue;
        }
        if (READ_MMU_ITMCFG_CPU_INT_STATr(unit, &itm_stat) != 0) {
            return;
        }
        /* Add ITM interrupt status to interrupt source matrix */
        if (MMU_ITMCFG_CPU_INT_STATr_MEM_PAR_ERR_INT_STATf_GET(itm_stat)) {
            SHR_BITSET(mmu_stat[inst].bmp, MMU_INTR_ITM_MEM_PAR_ERR);
        }
        if (MMU_ITMCFG_CPU_INT_STATr_THDI_INT_STATf_GET(itm_stat)) {
            if (READ_MMU_THDI_CPU_INT_STATr(unit, &thdi_stat) != 0) {
                return;
            }
            intr_stat = MMU_THDI_CPU_INT_STATr_GET(thdi_stat);
            mmu_intr_mask32_set(&mmu_stat[inst], intr_stat,
                                THDI_BIT_BASE, THDI_NUM_BITS);
        }
        if (MMU_ITMCFG_CPU_INT_STATr_THDO_INT_STATf_GET(itm_stat)) {
            if (READ_MMU_THDO_CPU_INT_STATr(unit, &thdo_stat) != 0) {
                return;
            }
            intr_stat = MMU_THDO_CPU_INT_STATr_GET(thdo_stat);
            mmu_intr_mask32_set(&mmu_stat[inst], intr_stat,
                                THDO_BIT_BASE, THDO_NUM_BITS);
            if (READ_MMU_THDO_BST_CPU_INT_STATr(unit, &thdo_bst_stat) != 0) {
                return;
            }
            intr_stat = MMU_THDO_BST_CPU_INT_STATr_GET(thdo_bst_stat);
            mmu_intr_mask32_set(&mmu_stat[inst], intr_stat,
                                THDO_BST_BIT_BASE, THDO_BST_NUM_BITS);
        }
        if (MMU_ITMCFG_CPU_INT_STATr_CFAP_INT_STATf_GET(itm_stat)) {
            if (READ_MMU_CFAP_INT_STATr(unit, &cfap_stat) != 0) {
                return;
            }
            /* This is a 64-bit register using only the lower 32 bits */
            intr_stat = MMU_CFAP_INT_STATr_GET(cfap_stat, 0);
            mmu_intr_mask32_set(&mmu_stat[inst], intr_stat,
                                CFAP_BIT_BASE, CFAP_NUM_BITS);
        }
        if (MMU_ITMCFG_CPU_INT_STATr_THDR_INT_STATf_GET(itm_stat)) {
            if (READ_MMU_THDR_QE_CPU_INT_STATr(unit, &thdr_stat) != 0) {
                return;
            }
            intr_stat = MMU_THDR_QE_CPU_INT_STATr_GET(thdr_stat);
            mmu_intr_mask32_set(&mmu_stat[inst], intr_stat,
                                THDR_BIT_BASE, THDR_NUM_BITS);
        }
    }

    /* Now call interrupt handlers according to the interrupt source matrix */
    for (inst = 0; inst < MAX_INST; inst++) {
        SHR_BIT_ITER(mmu_stat[inst].bmp, MAX_INTRS, intr_num) {
            if (SHR_BITGET(mmu_stat[inst].bmp, intr_num) == 0) {
                /* No interrupt */
                continue;
            }
            if (SHR_BITGET(mmu_intr_mask[unit].bmp, intr_num) == 0) {
                /* Interrupt disabled */
                continue;
            }
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "MMU interrupt %u, instance %u\n"),
                       intr_num, inst));
            iv = &mmu_intr_vect_info[unit].intr_vects[intr_num];
            if (iv->func) {
                uint32_t param = iv->param;
                /* Add interrupt instance to callback parameter */
                MMU_INTR_INST_SET(param, inst);
                iv->func(unit, param);
            } else {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "No handler for MMU interrupt %u\n"),
                           intr_num));
                mmu_intr_disable(unit, intr_num);
            }
        }
    }
}

static void
mmu_thread(shr_thread_t tc, void *arg)
{
    int unit = (uintptr_t)arg;

    while (1) {
        shr_thread_sleep(tc, SHR_THREAD_FOREVER);
        if (shr_thread_stopping(tc)) {
            break;
        }
        mmu_intr(unit);
        bcmbd_cmicx_lp_intr_enable(unit, CMIC_MMU_INTR);
    }
}

static void
mmu_isr(int unit, uint32_t param)
{
    /* Disable interrupt and wake up MMU thread */
    bcmbd_cmicx_lp_intr_disable_nosync(unit, CMIC_MMU_INTR);
    shr_thread_wake(mmu_thread_ctrl[unit]);
}

static int
mmu_intr_init(int unit)
{
    shr_thread_t tc;
    void *arg;

    if (mmu_intr_lock[unit] != NULL) {
        return SHR_E_INTERNAL;
    }

    mmu_intr_lock[unit] = sal_spinlock_create("bcmbdMmuIntr");
    if (mmu_intr_lock[unit] == NULL) {
        return SHR_E_FAIL;
    }

    if (mmu_thread_ctrl[unit] != NULL) {
        return SHR_E_INTERNAL;
    }

    /* Pass in unit number as context */
    arg = (void *)(uintptr_t)unit;

    tc = shr_thread_start("bcmbdMmuIntr", -1, mmu_thread, arg);
    if (tc == NULL) {
        return SHR_E_FAIL;
    }

    mmu_thread_ctrl[unit] = tc;

    return SHR_E_NONE;
}

static int
mmu_intr_cleanup(int unit)
{
    int rv = SHR_E_NONE;

    bcmbd_cmicx_lp_intr_disable(unit, CMIC_MMU_INTR);
    mmu_intr_connected[unit] = false;

    if (mmu_thread_ctrl[unit] != NULL) {
        rv = shr_thread_stop(mmu_thread_ctrl[unit], 2 * SECOND_USEC);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        mmu_thread_ctrl[unit] = NULL;
    }

    if (mmu_intr_lock[unit] != NULL) {
        rv = sal_spinlock_destroy(mmu_intr_lock[unit]);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        mmu_intr_lock[unit] = NULL;
    }

    return rv;
}

static int
mmu_intr_func_set(int unit, unsigned int intr_num,
                  bcmbd_intr_f intr_func, uint32_t intr_param)
{
    bcmbd_intr_vect_t *iv;

    if (!mmu_intr_connected[unit]) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Add handler for MMU interrupt %u\n"),
                   intr_num));
        bcmbd_cmicx_lp_intr_func_set(unit, CMIC_MMU_INTR, mmu_isr, 0);
        bcmbd_cmicx_lp_intr_enable(unit, CMIC_MMU_INTR);
        mmu_intr_connected[unit] = true;
    }

    if (intr_num >= MAX_INTRS) {
        return SHR_E_PARAM;
    }

    iv = &mmu_intr_vect_info[unit].intr_vects[intr_num];
    iv->func = intr_func;
    iv->param = intr_param;

    return SHR_E_NONE;
}

static bcmbd_intr_drv_t mmu_intr_drv = {
    .intr_func_set = mmu_intr_func_set,
    .intr_enable = mmu_intr_enable,
    .intr_disable = mmu_intr_disable,
    .intr_clear = mmu_intr_clear,
};

/*******************************************************************************
 * Public functions
 */

int
bcm56780_a0_bd_mmu_intr_drv_init(int unit)
{
    int rv;

    rv = bcmbd_mmu_intr_drv_init(unit, &mmu_intr_drv);
    if (SHR_SUCCESS(rv)) {
        rv = mmu_intr_init(unit);
        if (SHR_FAILURE(rv)) {
            (void)mmu_intr_cleanup(unit);
        }
    }

    return rv;
}

int
bcm56780_a0_bd_mmu_intr_drv_cleanup(int unit)
{
    (void)bcmbd_mmu_intr_drv_init(unit, NULL);
    return mmu_intr_cleanup(unit);
}
