/*! \file bcm56990_a0_bd_pvt_intr.c
 *
 * PVTMON thermal monitor interrupt driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_time.h>
#include <sal/sal_sleep.h>
#include <shr/shr_debug.h>
#include <shr/shr_thread.h>
#include <sal/sal_spinlock.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmbd/bcmbd_cmicx.h>
#include <bcmbd/bcmbd_cmicx_intr.h>
#include <bcmbd/bcmbd_pvt_intr_internal.h>
#include <bcmbd/bcmbd_pvt_internal.h>
#include <bcmbd/chip/bcm56990_a0_acc.h>
#include <bcmbd/chip/bcm56990_a0_lp_intr.h>
#include <bcmbd/chip/bcm56990_a0_pvt_intr.h>

#include "bcm56990_a0_drv.h"

#define  BSL_LOG_MODULE  BSL_LS_BCMBD_INTR

/*******************************************************************************
 * Local definitions
 */
#define MAX_UNITS                   BCMDRD_CONFIG_MAX_UNITS
#define MAX_INTRS                   MAX_PVT_INTR
#define INTR_NUM_TO_MON_IDX(_num)   ((_num)>>1)

#define INTR_MASK_LOCK(_u) sal_spinlock_lock(pvt_intr_lock[_u]);
#define INTR_MASK_UNLOCK(_u) sal_spinlock_unlock(pvt_intr_lock[_u]);

typedef struct bcmbd_intr_vect_s {
    bcmbd_intr_f func;
    uint32_t param;
} bcmbd_intr_vect_t;

typedef struct bcmbd_intr_vect_info_s {
    bcmbd_intr_vect_t intr_vects[MAX_INTRS];
} bcmbd_intr_vect_info_t;

/*******************************************************************************
 * Local data
 */

static bool pvt_intr_connected[MAX_UNITS];
static shr_thread_t pvt_thread_ctrl[MAX_UNITS];
static sal_spinlock_t pvt_intr_lock[MAX_UNITS];
static bcmbd_intr_vect_info_t pvt_intr_vect_info[MAX_UNITS];

/*******************************************************************************
 * Private functions
 */
static void
pvt_intr_update(int unit, unsigned int intr_num, int enable)
{
    TOP_PVTMON_INTR_MASK_0r_t intr_mask;
    TOP_TMON_CHANNELS_CTRL_1r_t chan_ctrl;
    uint32_t mask;
    uint32_t sw;
    int mon_idx;
    int ioerr = 0;

    mon_idx = INTR_NUM_TO_MON_IDX(intr_num);

    ioerr += READ_TOP_TMON_CHANNELS_CTRL_1r(unit, &chan_ctrl);
    sw = TOP_TMON_CHANNELS_CTRL_1r_TMON_INTR_HIGHTEMP_CTRL_ENf_GET(chan_ctrl);
    if (enable) {
        sw |= LSHIFT32(1, mon_idx);
    } else {
        sw &= ~LSHIFT32(1, mon_idx);
    }
    TOP_TMON_CHANNELS_CTRL_1r_TMON_INTR_HIGHTEMP_CTRL_ENf_SET(chan_ctrl, sw);
    ioerr += WRITE_TOP_TMON_CHANNELS_CTRL_1r(unit, chan_ctrl);

    ioerr += READ_TOP_PVTMON_INTR_MASK_0r(unit, &intr_mask);
    mask = TOP_PVTMON_INTR_MASK_0r_GET(intr_mask);
    if (enable) {
        mask |= LSHIFT32(1, intr_num);
    } else {
        mask &= ~LSHIFT32(1, intr_num);
    }
    TOP_PVTMON_INTR_MASK_0r_SET(intr_mask, mask);
    ioerr += WRITE_TOP_PVTMON_INTR_MASK_0r(unit, intr_mask);

    if (ioerr) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "PVTMON I/O error\n")));
    }
}

static void
pvt_intr_enable(int unit, unsigned int intr_num)
{
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Enable PVTMON intr %u\n"),
                 intr_num));
    if (intr_num >= MAX_INTRS) {
        return;
    }

    /* We may be called by multiple application threads */
    INTR_MASK_LOCK(unit);
    pvt_intr_update(unit, intr_num, 1);
    INTR_MASK_UNLOCK(unit);
}

static void
pvt_intr_disable(int unit, unsigned int intr_num)
{
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Disable PVTMON intr %u\n"),
                 intr_num));
    if (intr_num >= MAX_INTRS) {
        return;
    }

    /* We may be called by multiple application threads */
    INTR_MASK_LOCK(unit);
    pvt_intr_update(unit, intr_num, 0);
    INTR_MASK_UNLOCK(unit);
}

static void
pvt_intr(int unit)
{
    uint32_t intr_stat = 0;
    uint32_t intr_num;
    TOP_PVTMON_INTR_STATUS_0r_t reg;

    if (READ_TOP_PVTMON_INTR_STATUS_0r(unit, &reg) != 0) {
        return;
    }
    intr_stat = TOP_PVTMON_INTR_STATUS_0r_GET(reg);
    if (intr_stat == 0) {
        /* No active interrupts */
        return;
    }

    /* Now call interrupt handlers according to the interrupt source matrix */
    for (intr_num = 0; intr_num < MAX_INTRS; intr_num++) {
        if ((intr_stat & LSHIFT32(1, intr_num))) {
            bcmbd_intr_vect_t *iv;
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "PVT interrupt %u\n"),
                       intr_num));
            iv = &pvt_intr_vect_info[unit].intr_vects[intr_num];
            if (iv->func) {
                uint32_t param = iv->param;
                iv->func(unit, param);
            } else {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "No handler for PVT interrupt %u\n"),
                           intr_num));
                pvt_intr_disable(unit, intr_num);
            }
        }
    }
}



/*******************************************************************************
 * PVT Interrupt driver
 */

static void
pvt_thread(shr_thread_t tc, void *arg)
{
    int unit = (uintptr_t)arg;

    while (1) {
        shr_thread_sleep(tc, SHR_THREAD_FOREVER);
        if (shr_thread_stopping(tc)) {
            break;
        }

        pvt_intr(unit);
        bcmbd_cmicx_lp_intr_enable(unit, PVTMON_INTR);
    }
}

static void
pvt_isr(int unit, uint32_t param)
{
    /* Disable interrupt and wake up thread */
    bcmbd_cmicx_lp_intr_disable_nosync(unit, PVTMON_INTR);

    shr_thread_wake(pvt_thread_ctrl[unit]);
}

static void
pvt_intr_clear(int unit, unsigned int intr_num)
{
    int ioerr = 0;
    uint32_t intr_bits;
    TOP_PVTMON_INTR_STATUS_0r_t status;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                          "Clear PVTMON intr %u\n"),
                 intr_num));
    if (intr_num >= MAX_INTRS) {
        return;
    }

    intr_bits = LSHIFT32(1, intr_num);

    /* Write 1 to clear status bits to dessert interrupt */
    TOP_PVTMON_INTR_STATUS_0r_CLR(status);
    TOP_PVTMON_INTR_STATUS_0r_SET(status, intr_bits);
    ioerr += WRITE_TOP_PVTMON_INTR_STATUS_0r(unit, status);
    if (ioerr) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "PVTMON I/O error\n")));
    }
}


static int
pvt_intr_init(int unit)
{
    shr_thread_t tc;
    void *arg;

    if (pvt_intr_lock[unit] != NULL) {
        return SHR_E_INTERNAL;
    }

    pvt_intr_lock[unit] = sal_spinlock_create("bcmbdPvtIntr");
    if (pvt_intr_lock[unit] == NULL) {
        return SHR_E_FAIL;
    }

    if (pvt_thread_ctrl[unit] != NULL) {
        return SHR_E_INTERNAL;
    }

    /* Pass in unit number as context */
    arg = (void *)(uintptr_t)unit;

    tc = shr_thread_start("bcmbdPvtIntr", -1, pvt_thread, arg);
    if (tc == NULL) {
        return SHR_E_FAIL;
    }

    pvt_thread_ctrl[unit] = tc;

    return SHR_E_NONE;
}

static int
pvt_intr_cleanup(int unit)
{
    int rv = SHR_E_NONE;

    bcmbd_cmicx_lp_intr_disable(unit, PVTMON_INTR);
    pvt_intr_connected[unit] = false;

    if (pvt_thread_ctrl[unit] != NULL) {
        rv = shr_thread_stop(pvt_thread_ctrl[unit], 2 * SECOND_USEC);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        pvt_thread_ctrl[unit] = NULL;
    }

    if (pvt_intr_lock[unit] != NULL) {
        rv = sal_spinlock_destroy(pvt_intr_lock[unit]);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        pvt_intr_lock[unit] = NULL;
    }

    return rv;
}

static int
pvt_intr_func_set(int unit, unsigned int intr_num,
                  bcmbd_intr_f intr_func, uint32_t intr_param)
{
    bcmbd_intr_vect_t *iv;

    if (!pvt_intr_connected[unit]) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Add handler for PVTMON intr %u\n"),
                   intr_num));
        bcmbd_cmicx_lp_intr_func_set(unit, PVTMON_INTR, pvt_isr, 0);
        bcmbd_cmicx_lp_intr_enable(unit, PVTMON_INTR);
        pvt_intr_connected[unit] = true;
    }

    if (intr_num >= MAX_INTRS) {
        return SHR_E_PARAM;
    }

    iv = &pvt_intr_vect_info[unit].intr_vects[intr_num];
    iv->func = intr_func;
    iv->param = intr_param;

    return SHR_E_NONE;
}


static bcmbd_intr_drv_t pvt_intr_drv = {
    .intr_func_set = pvt_intr_func_set,
    .intr_enable = pvt_intr_enable,
    .intr_disable = pvt_intr_disable,
    .intr_clear = pvt_intr_clear,
};

/*******************************************************************************
 * Public functions
 */

int
bcm56990_a0_bd_pvt_intr_drv_init(int unit)
{
    int rv = SHR_E_NONE;
    bool emul;

    emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);
    if (emul) {
        return SHR_E_NONE;
    }

    rv = bcmbd_pvt_intr_drv_init(unit, &pvt_intr_drv);
    if (SHR_SUCCESS(rv)) {
        rv = pvt_intr_init(unit);
        if (SHR_FAILURE(rv)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "PVT intr init error\n")));
            (void)pvt_intr_cleanup(unit);
        }
    }

    return rv;
}

int
bcm56990_a0_bd_pvt_intr_drv_cleanup(int unit)
{
    bool emul;

    emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);

    if (emul) {
        return SHR_E_NONE;
    } else {
        (void)bcmbd_pvt_intr_drv_init(unit, NULL);
        return pvt_intr_cleanup(unit);
    }
}
