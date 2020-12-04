/*! \file bcm56990_a0_bd_db_intr.c
 *
 * The IDE-EDB interrupts are daisy-chained on a single iProc hardware
 * interrupt, which means that the hardware interrupt handler must
 * inspect additional registers to determine the actual source(s) of
 * the interrupt.
 *
 * Note that both the IDB-EDB interrupt status registers and the top-level
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
#include <sal/sal_spinlock.h>
#include <sal/sal_sem.h>
#include <sal/sal_time.h>

#include <shr/shr_debug.h>
#include <shr/shr_thread.h>
#include <shr/shr_bitop.h>

#include <bcmbd/bcmbd_db_intr_internal.h>
#include <bcmbd/bcmbd_cmicx.h>
#include <bcmbd/bcmbd_cmicx_intr.h>
#include <bcmbd/chip/bcm56990_a0_acc.h>
#include <bcmbd/chip/bcm56990_a0_lp_intr.h>
#include <bcmbd/chip/bcm56990_a0_db_intr.h>
#include <bcmdrd/chip/bcm56990_a0_defs.h>
#include "bcm56990_a0_drv.h"

#define  BSL_LOG_MODULE     BSL_LS_BCMBD_INTR

/*******************************************************************************
 * Local definitions
 */

#define MAX_UNITS           BCMDRD_CONFIG_MAX_UNITS
#define MAX_INTRS           MAX_DB_INTR
#define PORTIF_INST_NUM     16
/*! Get LP interrupt number from IDB-EDB instance */
#define LP_INTR_NUM(_db_inst)           \
    ((_db_inst) + PIPE0_IDB_EDB_TO_CMIC_PERR_INTR)

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

#define INTR_MASK_LOCK(_u) sal_spinlock_lock(db_intr_lock[_u]);
#define INTR_MASK_UNLOCK(_u) sal_spinlock_unlock(db_intr_lock[_u]);

typedef struct bcmbd_intr_state_s {
    bool new_intr[PORTIF_INST_NUM];
} bcmbd_intr_state_t;

/*******************************************************************************
 * Local data
 */

static bool db_intr_connected[MAX_UNITS];
static bcmbd_intr_map_t db_intr_mask[MAX_UNITS];
static sal_spinlock_t db_intr_lock[MAX_UNITS];
static bcmbd_intr_vect_info_t db_intr_vect_info[MAX_UNITS];
static shr_thread_t db_thread_ctrl[MAX_UNITS];
/* Save valid PORT_IF pipes. */
static uint32_t valid_portif_pipes[MAX_UNITS];
static bcmbd_intr_state_t db_intr_state[MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

static uint32_t
db_intr_mask32_get(bcmbd_intr_map_t *intr_map, int base, int num_bits)
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
db_intr_update(int unit, unsigned int intr_num, int enable)
{
    int ioerr = 0;
    uint32_t intr_mask;

    /* We may be called by multiple application threads */
    INTR_MASK_LOCK(unit);

    if (intr_num == EDB_SER_FIFO_NON_EMPTY) {
        EDB_INTR_ENABLEr_t edb_en;
        /* Enable/disable interrupt */
        ioerr += READ_EDB_INTR_ENABLEr(unit, &edb_en);
        EDB_INTR_ENABLEr_SER_FIFO_NON_EMPTYf_SET(edb_en, enable);
        ioerr += WRITE_EDB_INTR_ENABLEr(unit, edb_en);
    } else {
        IDB_INTR_ENABLEr_t idb_en;
        /* Enable/disable leaf interrupt */
        intr_mask = db_intr_mask32_get(&db_intr_mask[unit],
                                       IDB_BIT_BASE, IDB_NUM_BITS);
        IDB_INTR_ENABLEr_SET(idb_en, intr_mask);
        ioerr += WRITE_IDB_INTR_ENABLEr(unit, idb_en);
    }

    if (ioerr) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "IDB-EDB interupt %u I/O error\n"),
                   intr_num));
    }

    INTR_MASK_UNLOCK(unit);
}

static void
db_intr_enable(int unit, unsigned int intr_num)
{
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Enable IDB-EDB intr %u\n"),
                 intr_num));
    if (intr_num >= MAX_INTRS) {
        return;
    }
    SHR_BITSET(db_intr_mask[unit].bmp, intr_num);
    db_intr_update(unit, intr_num, 1);
}

static void
db_intr_disable(int unit, unsigned int intr_num)
{
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Disable IDB-EDB intr %u\n"),
                 intr_num));
    if (intr_num >= MAX_INTRS) {
        return;
    }
    SHR_BITCLR(db_intr_mask[unit].bmp, intr_num);
    db_intr_update(unit, intr_num, 0);
}

static void
db_intr_clear(int unit, unsigned int intr_num)
{
    uint32_t intr_inst;
    bcmdrd_sid_t fifo_sid = INVALIDm;

    intr_inst = DB_INTR_INST_GET(intr_num);
    intr_num = DB_INTR_NUM_GET(intr_num);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Clear IDB-EDB intr %u (%u)\n"),
                 intr_num, intr_inst));
    if (intr_num >= MAX_INTRS) {
        return;
    }

    /*
     * For non_empty interrupts, pop out all entries to clear.
     * - FLEX_CTR_EGR_SER_FIFOm
     * This should not be done for normal application to avoid events' missing.
     */
    if (intr_num == EDB_SER_FIFO_NON_EMPTY) {
        EDB_INTR_STATUSr_t edb_st;
        uint32_t entry_data[BCMDRD_MAX_PT_WSIZE];
        bcmbd_pt_dyn_info_t dyn_info;
        fifo_sid = EDB_SER_FIFOm;

        while (1) {
            READ_EDB_INTR_STATUSr(unit, intr_inst, &edb_st);
            if (!EDB_INTR_STATUSr_SER_FIFO_NON_EMPTYf_GET(edb_st)) {
                break;
            }
            dyn_info.tbl_inst = intr_inst;
            dyn_info.index = 0;
            (void)bcmbd_cmicx_pop(unit, fifo_sid, &dyn_info, NULL,
                                  entry_data, BCMDRD_MAX_PT_WSIZE);
        }

    } else {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Clear IDB-EDB intr %u (%u)\n"),
                   intr_num, intr_inst));
    }
}

static void
db_intr(int unit, uint32_t inst)
{
    EDB_INTR_STATUSr_t edb_st;
    IDB_INTR_STATUSr_t idb_st;
    bcmbd_intr_vect_t *iv;
    unsigned int intr_num;
    int i;
    uint32_t val;

    bcmbd_intr_map_t db_stat[PORTIF_INST_NUM];

    /* Start with an empty interrupt source matrix */
    sal_memset(db_stat, 0, sizeof(db_stat));

    /* Processing EDB_INTR_STATUS */
    if (READ_EDB_INTR_STATUSr(unit, inst, &edb_st) != 0) {
        return;
    }
    if (EDB_INTR_STATUSr_SER_FIFO_NON_EMPTYf_GET(edb_st)) {
        SHR_BITSET(db_stat[inst].bmp, EDB_SER_FIFO_NON_EMPTY);
    }

    /* Processing IDB_INTR_STATUS */
    if (READ_IDB_INTR_STATUSr(unit, inst, &idb_st) != 0) {
        return;
    }
    val = IDB_INTR_STATUSr_GET(idb_st);
    for (i = 0; i < IDB_NUM_BITS; i++) {
        if (val & (0x1 << i)) {
            SHR_BITSET(db_stat[inst].bmp, IDB_BIT_BASE + i);
        }
    }

    /* Now call interrupt handlers according to the interrupt source matrix */
    SHR_BIT_ITER(db_stat[inst].bmp, MAX_INTRS, intr_num) {
        if (SHR_BITGET(db_stat[inst].bmp, intr_num) == 0) {
            /* No interrupt */
            continue;
        }
        if (SHR_BITGET(db_intr_mask[unit].bmp, intr_num) == 0) {
            /* Interrupt disabled */
            continue;
        }
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "IDB-EDB interrupt %u, instance %u\n"),
                   intr_num, inst));
        iv = &db_intr_vect_info[unit].intr_vects[intr_num];
        if (iv->func) {
            uint32_t param = iv->param;
            /* Add interrupt instance to callback parameter */
            DB_INTR_INST_SET(param, inst);
            iv->func(unit, param);
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "No handler for IDB-EDB interrupt %u\n"),
                       intr_num));
            db_intr_disable(unit, intr_num);
        }
    }
}

static void
db_thread(shr_thread_t tc, void *arg)
{
    uint32_t inst;
    int unit = (uintptr_t)arg;

    while (1) {
        shr_thread_sleep(tc, SHR_THREAD_FOREVER);
        if (shr_thread_stopping(tc)) {
            break;
        }

        for (inst = 0; inst < PORTIF_INST_NUM; inst++) {
            if (db_intr_state[unit].new_intr[inst]) {
                db_intr_state[unit].new_intr[inst] = false;
                db_intr(unit, inst);
                bcmbd_cmicx_lp_intr_enable(unit, LP_INTR_NUM(inst));
            }
        }
    }
}

static void
db_isr(int unit, uint32_t param)
{
    bcmbd_cmicx_lp_intr_disable_nosync(unit, LP_INTR_NUM(param));
    db_intr_state[unit].new_intr[param] = true;
    shr_thread_wake(db_thread_ctrl[unit]);
}

static int
db_intr_init(int unit)
{
    shr_thread_t tc;
    void *arg;

    if (db_intr_lock[unit] != NULL) {
        return SHR_E_INTERNAL;
    }

    db_intr_lock[unit] = sal_spinlock_create("bcmbdDBIntr");
    if (db_intr_lock[unit] == NULL) {
        return SHR_E_FAIL;
    }

    if (db_thread_ctrl[unit] != NULL) {
        return SHR_E_INTERNAL;
    }

    sal_memset(&db_intr_state[unit], 0, sizeof(bcmbd_intr_state_t));

    /* Pass in unit number as context */
    arg = (void *)(uintptr_t)unit;

    tc = shr_thread_start("bcmbdDBIntr", -1, db_thread, arg);
    if (tc == NULL) {
        return SHR_E_FAIL;
    }

    db_thread_ctrl[unit] = tc;

    return SHR_E_NONE;
}

static int
db_intr_cleanup(int unit)
{
    int rv = SHR_E_NONE;
    int inst;

    for (inst = 0; inst < PORTIF_INST_NUM; inst++) {
        bcmbd_cmicx_lp_intr_disable(unit, LP_INTR_NUM(inst));
    }

    db_intr_connected[unit] = false;

    if (db_thread_ctrl[unit] != NULL) {
        rv = shr_thread_stop(db_thread_ctrl[unit], 2 * SECOND_USEC);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        db_thread_ctrl[unit] = NULL;
    }

    if (db_intr_lock[unit] != NULL) {
        rv = sal_spinlock_destroy(db_intr_lock[unit]);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        db_intr_lock[unit] = NULL;
    }

    return rv;
}

static int
db_intr_func_set(int unit, unsigned int intr_num,
                 bcmbd_intr_f intr_func, uint32_t intr_param)
{
    uint32_t inst;
    bcmbd_intr_vect_t *iv;

    if (!db_intr_connected[unit]) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Add handler for IDB-EDB interrupt %u\n"),
                   intr_num));
        for (inst = 0; inst < PORTIF_INST_NUM; inst++) {
            if (valid_portif_pipes[unit] & (0x1 << inst)) {
                bcmbd_cmicx_lp_intr_func_set(unit, LP_INTR_NUM(inst),
                                             db_isr, inst);
                bcmbd_cmicx_lp_intr_enable(unit, LP_INTR_NUM(inst));
            }
        }

        db_intr_connected[unit] = true;
    }

    if (intr_num >= MAX_INTRS) {
        return SHR_E_PARAM;
    }

    iv = &db_intr_vect_info[unit].intr_vects[intr_num];
    iv->func = intr_func;
    iv->param = intr_param;

    return SHR_E_NONE;
}

static bcmbd_intr_drv_t db_intr_drv = {
    .intr_func_set = db_intr_func_set,
    .intr_enable = db_intr_enable,
    .intr_disable = db_intr_disable,
    .intr_clear = db_intr_clear,
};

/*******************************************************************************
 * Public functions
 */
int
bcm56990_a0_bd_db_intr_drv_init(int unit)
{
    int rv;
    uint32_t pipe_map;

    rv = bcmdrd_dev_valid_pipes_get(unit, &pipe_map);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    valid_portif_pipes[unit] = pipe_map;

    rv = bcmbd_db_intr_drv_init(unit, &db_intr_drv);
    if (SHR_SUCCESS(rv)) {
        rv = db_intr_init(unit);
        if (SHR_FAILURE(rv)) {
            (void)db_intr_cleanup(unit);
        }
    }

    return rv;
}

int
bcm56990_a0_bd_db_intr_drv_cleanup(int unit)
{
    (void)bcmbd_db_intr_drv_init(unit, NULL);
    return db_intr_cleanup(unit);
}

