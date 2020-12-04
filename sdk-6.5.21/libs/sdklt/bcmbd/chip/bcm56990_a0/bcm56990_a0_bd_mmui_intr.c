/*! \file bcm56990_a0_bd_mmui_intr.c
 *
 * The MMU instrument interrupts are daisy-chained on a single iProc hardware
 * interrupt, which means that the hardware interrupt handler must
 * inspect additional registers to determine the actual source(s) of
 * the interrupt.
 *
 * The top level register is:
 *
 *  - MMU_GLBCFG_CPU_INT_INST_STAT
 *
 * This register contains primary (leaf) interrupt sources as well as
 * secondary (daisy-chained) interrupt sources. The secondary interrupt
 * sources require that one of the following registers is read to obtain
 * the primary interrupt source:
 *
 *  - MMU_CFAP_INT2_STAT
 *  - MMU_THDI_CPU_INT_STAT_INST
 *  - MMU_THDO_BST_CPU_INT_STAT
 *  - MMU_THDR_QE_CPU_INT_STAT_INST
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
#include <bcmbd/chip/bcm56990_a0_acc.h>
#include <bcmbd/chip/bcm56990_a0_lp_intr.h>
#include <bcmbd/chip/bcm56990_a0_mmui_intr.h>

#include "bcm56990_a0_drv.h"

#define  BSL_LOG_MODULE  BSL_LS_BCMBD_INTR

/*******************************************************************************
 * Local definitions
 */

#define MAX_UNITS          BCMDRD_CONFIG_MAX_UNITS
#define MAX_INTRS          MMUI_INTR_MAX
#define NUM_ITM_INST       2
#define TH4_PIPES_PER_DEV  16

#define CMIC_MMUI_INTR   MMU_TO_CMIC_BST_INTR

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

#define INTR_MASK_LOCK(_u) sal_spinlock_lock(mmui_intr_lock[_u]);
#define INTR_MASK_UNLOCK(_u) sal_spinlock_unlock(mmui_intr_lock[_u]);

/*******************************************************************************
 * Local data
 */

static bool mmui_intr_connected[MAX_UNITS];
static bcmbd_intr_map_t mmui_intr_mask[MAX_UNITS];
static sal_spinlock_t mmui_intr_lock[MAX_UNITS];
static bcmbd_intr_vect_info_t mmui_intr_vect_info[MAX_UNITS];
static shr_thread_t mmui_thread_ctrl[MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

static void
mmui_intr_update(int unit, unsigned int intr_num, int enable)
{
    int ioerr = 0;
    MMU_GLBCFG_CPU_INT_INST_ENr_t glb_en;
    MMU_THDR_QE_CPU_INT_EN_INSTr_t thdr_inst_en;
    MMU_CFAP_INT2_ENr_t cfap2_en;
    MMU_THDO_BST_CPU_INT_ENr_t thdo_bst_en;
    MMU_THDI_CPU_INT_EN_INSTr_t thdi_inst_en;
    bool glb_thdo_upd = false;

    /* We may be called by multiple application threads */
    INTR_MASK_LOCK(unit);

    ioerr += READ_MMU_GLBCFG_CPU_INT_INST_ENr(unit, &glb_en);
    /* Start with leaf interrupts and then do daisy-chained interrupts */
    if (intr_num == MMUI_INTR_THDI_BST) {
        /* Enable/disable leaf interrupt */
        ioerr += READ_MMU_THDI_CPU_INT_EN_INSTr(unit, &thdi_inst_en);
        MMU_THDI_CPU_INT_EN_INSTr_BST_TRIGf_SET(thdi_inst_en, enable);
        ioerr += WRITE_MMU_THDI_CPU_INT_EN_INSTr(unit, thdi_inst_en);
        MMU_GLBCFG_CPU_INT_INST_ENr_THDI0_INT_INST_ENf_SET(glb_en, enable);
        MMU_GLBCFG_CPU_INT_INST_ENr_THDI1_INT_INST_ENf_SET(glb_en, enable);
    } else if (intr_num == MMUI_INTR_THDR_BST) {
        /* Enable/disable leaf interrupt */
        ioerr += READ_MMU_THDR_QE_CPU_INT_EN_INSTr(unit, &thdr_inst_en);
        MMU_THDR_QE_CPU_INT_EN_INSTr_BSTf_SET(thdr_inst_en, enable);
        ioerr += WRITE_MMU_THDR_QE_CPU_INT_EN_INSTr(unit, thdr_inst_en);
        MMU_GLBCFG_CPU_INT_INST_ENr_THDR0_INT_INST_ENf_SET(glb_en, enable);
        MMU_GLBCFG_CPU_INT_INST_ENr_THDR1_INT_INST_ENf_SET(glb_en, enable);
    } else if (intr_num == MMUI_INTR_CFAP_BST) {
        /* Enable/disable leaf interrupt */
        ioerr += READ_MMU_CFAP_INT2_ENr(unit, &cfap2_en);
        MMU_CFAP_INT2_ENr_BST_TRIGf_SET(cfap2_en, enable);
        ioerr += WRITE_MMU_CFAP_INT2_ENr(unit, cfap2_en);
        MMU_GLBCFG_CPU_INT_INST_ENr_CFAP0_INT_INST_ENf_SET(glb_en, enable);
        MMU_GLBCFG_CPU_INT_INST_ENr_CFAP1_INT_INST_ENf_SET(glb_en, enable);
    } else if (intr_num == MMUI_INTR_THDO_BST) {
        /* Enable/disable leaf interrupt */
        ioerr += READ_MMU_THDO_BST_CPU_INT_ENr(unit, &thdo_bst_en);
        MMU_THDO_BST_CPU_INT_ENr_BSTf_SET(thdo_bst_en, enable);
        ioerr += WRITE_MMU_THDO_BST_CPU_INT_ENr(unit, thdo_bst_en);
        glb_thdo_upd = true;
    } else if (intr_num == MMUI_INTR_THDO_EBST_FIFO_FULL) {
        /* Enable/disable leaf interrupt */
        ioerr += READ_MMU_THDO_BST_CPU_INT_ENr(unit, &thdo_bst_en);
        MMU_THDO_BST_CPU_INT_ENr_EBST_FIFO_FULLf_SET(thdo_bst_en, enable);
        ioerr += WRITE_MMU_THDO_BST_CPU_INT_ENr(unit, thdo_bst_en);
        glb_thdo_upd = true;
    }

    /* Start with leaf interrupts and then do daisy-chained interrupts */
    if (glb_thdo_upd) {
        ioerr += READ_MMU_THDO_BST_CPU_INT_ENr(unit, &thdo_bst_en);
        enable = MMU_THDO_BST_CPU_INT_ENr_GET(thdo_bst_en) ? 1 : 0;
        MMU_GLBCFG_CPU_INT_INST_ENr_THDO0_INT_INST_ENf_SET(glb_en, enable);
        MMU_GLBCFG_CPU_INT_INST_ENr_THDO1_INT_INST_ENf_SET(glb_en, enable);
    }
    ioerr += WRITE_MMU_GLBCFG_CPU_INT_INST_ENr(unit, glb_en);

    if (ioerr) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "MMU interupt %u I/O error\n"),
                   intr_num));
    }

    INTR_MASK_UNLOCK(unit);
}

static void
mmui_intr_enable(int unit, unsigned int intr_num)
{
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                          "Enable MMU intr %u\n"),
                 intr_num));
    if (intr_num >= MAX_INTRS) {
        return;
    }
    SHR_BITSET(mmui_intr_mask[unit].bmp, intr_num);
    mmui_intr_update(unit, intr_num, 1);
}

static void
mmui_intr_disable(int unit, unsigned int intr_num)
{
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                          "Disable MMU intr %u\n"),
                 intr_num));
    if (intr_num >= MAX_INTRS) {
        return;
    }
    SHR_BITCLR(mmui_intr_mask[unit].bmp, intr_num);
    mmui_intr_update(unit, intr_num, 0);
}

static void
mmui_intr_clear(int unit, unsigned int intr_num)
{
    int ioerr = 0;
    int idx;
    MMU_GLBCFG_CPU_INT_INST_STATr_t glb_stat;
    MMU_THDI_CPU_INT_STAT_INSTr_t thdi_inst_stat;
    MMU_THDR_QE_CPU_INT_STAT_INSTr_t thdr_inst_stat;
    MMU_THDR_QE_CPU_INT_SET_INSTr_t thdr_inst_set;
    MMU_CFAP_INT2_STATr_t cfap2_stat;
    MMU_CFAP_INT2_SETr_t cfap2_set;
    MMU_THDO_BST_CPU_INT_STATr_t thdo_bst_stat;
    MMU_THDO_BST_CPU_INT_SETr_t thdo_bst_set;
    MMU_THDO_BST_STATr_t thdo_bst_data;
    MMU_THDI_BST_TRIGGER_STATUS_TYPEr_t thdi_bst_stat;

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
    if (intr_num == MMUI_INTR_THDI_BST) {
        /* clear bst interrupt status for THDI */
        MMU_THDI_BST_TRIGGER_STATUS_TYPEr_CLR(thdi_bst_stat);
        for (idx = 0; idx < TH4_PIPES_PER_DEV; idx++) {
            if (pipe_map & (1 << idx)) {
                ioerr +=
                    WRITE_MMU_THDI_BST_TRIGGER_STATUS_TYPEr(unit, idx,
                                        thdi_bst_stat);
            }
        }
        MMU_THDI_CPU_INT_STAT_INSTr_CLR(thdi_inst_stat);
        MMU_THDI_CPU_INT_STAT_INSTr_BST_TRIG_STATf_SET(thdi_inst_stat, 1);
        ioerr += WRITE_MMU_THDI_CPU_INT_STAT_INSTr(unit, intr_inst, thdi_inst_stat);
        if (intr_inst == 0) {
            MMU_GLBCFG_CPU_INT_INST_STATr_THDI0_INT_INST_STATf_SET(glb_stat, 1);
        } else {
            MMU_GLBCFG_CPU_INT_INST_STATr_THDI1_INT_INST_STATf_SET(glb_stat, 1);
        }
    } else if (intr_num == MMUI_INTR_THDR_BST) {
        MMU_THDR_QE_CPU_INT_SET_INSTr_CLR(thdr_inst_set);
        MMU_THDR_QE_CPU_INT_SET_INSTr_BSTf_SET(thdr_inst_set, 0);
        ioerr += WRITE_MMU_THDR_QE_CPU_INT_SET_INSTr(unit, intr_inst, thdr_inst_set);
        MMU_THDR_QE_CPU_INT_STAT_INSTr_CLR(thdr_inst_stat);
        MMU_THDR_QE_CPU_INT_STAT_INSTr_BSTf_SET(thdr_inst_stat, 1);
        ioerr += WRITE_MMU_THDR_QE_CPU_INT_STAT_INSTr(unit, intr_inst, thdr_inst_stat);
        if (intr_inst == 0) {
            MMU_GLBCFG_CPU_INT_INST_STATr_THDR0_INT_INST_STATf_SET(glb_stat, 1);
        } else {
            MMU_GLBCFG_CPU_INT_INST_STATr_THDR1_INT_INST_STATf_SET(glb_stat, 1);
        }
    } else if (intr_num == MMUI_INTR_CFAP_BST) {
        MMU_CFAP_INT2_SETr_CLR(cfap2_set);
        MMU_CFAP_INT2_SETr_BST_TRIGf_SET(cfap2_set, 0);
        ioerr += WRITE_MMU_CFAP_INT2_SETr(unit, intr_inst, cfap2_set);
        MMU_CFAP_INT2_STATr_CLR(cfap2_stat);
        MMU_CFAP_INT2_STATr_BST_TRIG_STATf_SET(cfap2_stat, 1);
        ioerr += WRITE_MMU_CFAP_INT2_STATr(unit, intr_inst, cfap2_stat);
        if (intr_inst == 0) {
            MMU_GLBCFG_CPU_INT_INST_STATr_THDR0_INT_INST_STATf_SET(glb_stat, 1);
        } else {
            MMU_GLBCFG_CPU_INT_INST_STATr_THDR1_INT_INST_STATf_SET(glb_stat, 1);
        }
    } else if (intr_num == MMUI_INTR_THDO_BST) {
        ioerr += READ_MMU_THDO_BST_CPU_INT_SETr(unit, intr_inst, &thdo_bst_set);
        MMU_THDO_BST_CPU_INT_SETr_BSTf_SET(thdo_bst_set, 0);
        ioerr += WRITE_MMU_THDO_BST_CPU_INT_SETr(unit, intr_inst, thdo_bst_set);
        MMU_THDO_BST_STATr_CLR(thdo_bst_data);
        ioerr += WRITE_MMU_THDO_BST_STATr(unit, intr_inst, thdo_bst_data);

        MMU_THDO_BST_CPU_INT_STATr_CLR(thdo_bst_stat);
        MMU_THDO_BST_CPU_INT_STATr_BSTf_SET(thdo_bst_stat, 1);
        ioerr += WRITE_MMU_THDO_BST_CPU_INT_STATr(unit, intr_inst, thdo_bst_stat);
        if (intr_inst == 0) {
            MMU_GLBCFG_CPU_INT_INST_STATr_THDO0_INT_INST_STATf_SET(glb_stat, 1);
        } else {
            MMU_GLBCFG_CPU_INT_INST_STATr_THDO1_INT_INST_STATf_SET(glb_stat, 1);
        }
    } else if (intr_num == MMUI_INTR_THDO_EBST_FIFO_FULL) {
        ioerr += READ_MMU_THDO_BST_CPU_INT_SETr(unit, intr_inst, &thdo_bst_set);
        MMU_THDO_BST_CPU_INT_SETr_EBST_FIFO_FULLf_SET(thdo_bst_set, 0);
        ioerr += WRITE_MMU_THDO_BST_CPU_INT_SETr(unit, intr_inst, thdo_bst_set);
        MMU_THDO_BST_CPU_INT_STATr_CLR(thdo_bst_stat);
        MMU_THDO_BST_CPU_INT_STATr_EBST_FIFO_FULLf_SET(thdo_bst_stat, 1);
        ioerr += WRITE_MMU_THDO_BST_CPU_INT_STATr(unit, intr_inst, thdo_bst_stat);
        if (intr_inst == 0) {
            MMU_GLBCFG_CPU_INT_INST_STATr_THDO0_INT_INST_STATf_SET(glb_stat, 1);
        } else {
            MMU_GLBCFG_CPU_INT_INST_STATr_THDO1_INT_INST_STATf_SET(glb_stat, 1);
        }
    }
    ioerr += WRITE_MMU_GLBCFG_CPU_INT_INST_STATr(unit, glb_stat);
}

static void
mmui_intr(int unit)
{
    MMU_GLBCFG_CPU_INT_INST_STATr_t glb_stat;
    MMU_THDR_QE_CPU_INT_STAT_INSTr_t thdr_stat;
    MMU_CFAP_INT2_STATr_t cfap2_stat;
    MMU_THDI_CPU_INT_STAT_INSTr_t thdi_stat;
    MMU_THDO_BST_CPU_INT_STATr_t thdo_stat;
    bcmbd_intr_map_t mmui_stat[NUM_ITM_INST];
    bcmbd_intr_vect_t *iv;
    unsigned int inst;
    unsigned int intr_num;

    /* Start with an empty interrupt source matrix */
    sal_memset(mmui_stat, 0, sizeof(mmui_stat));

    /* Read top-level register to figure out what to do next */
    if (READ_MMU_GLBCFG_CPU_INT_INST_STATr(unit, &glb_stat) != 0) {
        return;
    }

    /* Check top-level leaf interrupts (one instance only) */
    if (MMU_GLBCFG_CPU_INT_INST_STATr_THDI0_INT_INST_STATf_GET(glb_stat)) {
        if (READ_MMU_THDI_CPU_INT_STAT_INSTr(unit, 0, &thdi_stat) != 0) {
            return;
        }
        if (MMU_THDI_CPU_INT_STAT_INSTr_BST_TRIG_STATf_GET(thdi_stat)) {
            SHR_BITSET(mmui_stat[0].bmp, MMUI_INTR_THDI_BST);
        }
    }
    if (MMU_GLBCFG_CPU_INT_INST_STATr_THDI1_INT_INST_STATf_GET(glb_stat)) {
        if (READ_MMU_THDI_CPU_INT_STAT_INSTr(unit, 1, &thdi_stat) != 0) {
            return;
        }
        if (MMU_THDI_CPU_INT_STAT_INSTr_BST_TRIG_STATf_GET(thdi_stat)) {
            SHR_BITSET(mmui_stat[1].bmp, MMUI_INTR_THDI_BST);
        }
    }
    if (MMU_GLBCFG_CPU_INT_INST_STATr_THDR0_INT_INST_STATf_GET(glb_stat)) {
        if (READ_MMU_THDR_QE_CPU_INT_STAT_INSTr(unit, 0, &thdr_stat) != 0) {
            return;
        }
        if (MMU_THDR_QE_CPU_INT_STAT_INSTr_BSTf_GET(thdr_stat)) {
            SHR_BITSET(mmui_stat[0].bmp, MMUI_INTR_THDR_BST);
        }
    }
    if (MMU_GLBCFG_CPU_INT_INST_STATr_THDR1_INT_INST_STATf_GET(glb_stat)) {
        if (READ_MMU_THDR_QE_CPU_INT_STAT_INSTr(unit, 1, &thdr_stat) != 0) {
            return;
        }
        if (MMU_THDR_QE_CPU_INT_STAT_INSTr_BSTf_GET(thdr_stat)) {
            SHR_BITSET(mmui_stat[1].bmp, MMUI_INTR_THDR_BST);
        }
    }
    if (MMU_GLBCFG_CPU_INT_INST_STATr_CFAP0_INT_INST_STATf_GET(glb_stat)) {
        if (READ_MMU_CFAP_INT2_STATr(unit, 0, &cfap2_stat) != 0) {
            return;
        }
        if (MMU_CFAP_INT2_STATr_BST_TRIG_STATf_GET(cfap2_stat)) {
            SHR_BITSET(mmui_stat[0].bmp, MMUI_INTR_CFAP_BST);
        }
    }
    if (MMU_GLBCFG_CPU_INT_INST_STATr_CFAP1_INT_INST_STATf_GET(glb_stat)) {
        if (READ_MMU_CFAP_INT2_STATr(unit, 1, &cfap2_stat) != 0) {
            return;
        }
        if (MMU_CFAP_INT2_STATr_BST_TRIG_STATf_GET(cfap2_stat)) {
            SHR_BITSET(mmui_stat[1].bmp, MMUI_INTR_CFAP_BST);
        }
    }
    if (MMU_GLBCFG_CPU_INT_INST_STATr_THDO0_INT_INST_STATf_GET(glb_stat)) {
        if (READ_MMU_THDO_BST_CPU_INT_STATr(unit, 0, &thdo_stat) != 0) {
            return;
        }
        if (MMU_THDO_BST_CPU_INT_STATr_BSTf_GET(thdo_stat)) {
            SHR_BITSET(mmui_stat[0].bmp, MMUI_INTR_THDO_BST);
        }
        if (MMU_THDO_BST_CPU_INT_STATr_EBST_FIFO_FULLf_GET(thdo_stat)) {

            SHR_BITSET(mmui_stat[0].bmp, MMUI_INTR_THDO_EBST_FIFO_FULL);
        }
    }
    if (MMU_GLBCFG_CPU_INT_INST_STATr_THDO1_INT_INST_STATf_GET(glb_stat)) {
        if (READ_MMU_THDO_BST_CPU_INT_STATr(unit, 1, &thdo_stat) != 0) {
            return;
        }
        if (MMU_THDO_BST_CPU_INT_STATr_BSTf_GET(thdo_stat)) {
            SHR_BITSET(mmui_stat[1].bmp, MMUI_INTR_THDO_BST);
        }
        if (MMU_THDO_BST_CPU_INT_STATr_EBST_FIFO_FULLf_GET(thdo_stat)) {

            SHR_BITSET(mmui_stat[1].bmp, MMUI_INTR_THDO_EBST_FIFO_FULL);
        }
    }

    /* Now call interrupt handlers according to the interrupt source matrix */
    for (inst = 0; inst < NUM_ITM_INST; inst++) {
        SHR_BIT_ITER(mmui_stat[inst].bmp, MMUI_INTR_MAX, intr_num) {
            if (SHR_BITGET(mmui_stat[inst].bmp, intr_num) == 0) {
                /* No interrupt */
                continue;
            }
            if (SHR_BITGET(mmui_intr_mask[unit].bmp, intr_num) == 0) {
                /* Interrupt disabled */
                continue;
            }
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "MMU interrupt %u, instance %u\n"),
                       intr_num, inst));
            iv = &mmui_intr_vect_info[unit].intr_vects[intr_num];
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
                mmui_intr_disable(unit, intr_num);
            }
        }
    }
}

static void
mmui_thread(shr_thread_t tc, void *arg)
{
    int unit = (uintptr_t)arg;

    while (1) {
        shr_thread_sleep(tc, SHR_THREAD_FOREVER);
        if (shr_thread_stopping(tc)) {
            break;
        }
        mmui_intr(unit);
        bcmbd_cmicx_lp_intr_enable(unit, CMIC_MMUI_INTR);
    }
}

static void
mmui_isr(int unit, uint32_t param)
{
    /* Disable interrupt and wake up MMU instrument thread */
    bcmbd_cmicx_lp_intr_disable_nosync(unit, CMIC_MMUI_INTR);
    shr_thread_wake(mmui_thread_ctrl[unit]);
}

static int
mmui_intr_init(int unit)
{
    shr_thread_t tc;
    void *arg;

    if (mmui_intr_lock[unit] != NULL) {
        return SHR_E_INTERNAL;
    }

    mmui_intr_lock[unit] = sal_spinlock_create("bcmbdMmuiIntr");
    if (mmui_intr_lock[unit] == NULL) {
        return SHR_E_FAIL;
    }

    if (mmui_thread_ctrl[unit] != NULL) {
        return SHR_E_INTERNAL;
    }

    /* Pass in unit number as context */
    arg = (void *)(uintptr_t)unit;

    tc = shr_thread_start("bcmbdMmuiIntr", -1, mmui_thread, arg);
    if (tc == NULL) {
        return SHR_E_FAIL;
    }

    mmui_thread_ctrl[unit] = tc;

    return SHR_E_NONE;
}

static int
mmui_intr_cleanup(int unit)
{
    int rv = SHR_E_NONE;

    bcmbd_cmicx_lp_intr_disable(unit, CMIC_MMUI_INTR);
    mmui_intr_connected[unit] = false;

    if (mmui_thread_ctrl[unit] != NULL) {
        rv = shr_thread_stop(mmui_thread_ctrl[unit], 2 * SECOND_USEC);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        mmui_thread_ctrl[unit] = NULL;
    }

    if (mmui_intr_lock[unit] != NULL) {
        rv = sal_spinlock_destroy(mmui_intr_lock[unit]);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        mmui_intr_lock[unit] = NULL;
    }

    return rv;
}

static int
mmui_intr_func_set(int unit, unsigned int intr_num,
                   bcmbd_intr_f intr_func, uint32_t intr_param)
{
    bcmbd_intr_vect_t *iv;

    if (!mmui_intr_connected[unit]) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Add handler for MMU instrument interrupt %u\n"),
                   intr_num));
        bcmbd_cmicx_lp_intr_func_set(unit, CMIC_MMUI_INTR,
                                     mmui_isr, 0);
        bcmbd_cmicx_lp_intr_enable(unit, CMIC_MMUI_INTR);
        mmui_intr_connected[unit] = true;
    }

    if (intr_num >= MAX_INTRS) {
        return SHR_E_PARAM;
    }

    iv = &mmui_intr_vect_info[unit].intr_vects[intr_num];
    iv->func = intr_func;
    iv->param = intr_param;

    return SHR_E_NONE;
}

static bcmbd_intr_drv_t mmui_intr_drv = {
    .intr_func_set = mmui_intr_func_set,
    .intr_enable = mmui_intr_enable,
    .intr_disable = mmui_intr_disable,
    .intr_clear = mmui_intr_clear,
};

/*******************************************************************************
 * Public functions
 */

int
bcm56990_a0_bd_mmui_intr_drv_init(int unit)
{
    int rv;

    rv = bcmbd_mmui_intr_drv_init(unit, &mmui_intr_drv);
    if (SHR_SUCCESS(rv)) {
        rv = mmui_intr_init(unit);
        if (SHR_FAILURE(rv)) {
            (void)mmui_intr_cleanup(unit);
        }
    }

    return rv;
}

int
bcm56990_a0_bd_mmui_intr_drv_cleanup(int unit)
{
    (void)bcmbd_mmui_intr_drv_init(unit, NULL);
    return mmui_intr_cleanup(unit);
}
