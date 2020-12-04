/*! \file bcm56780_a0_bd_sec_intr.c
 *
 * The top level register is:
 *
 *  - MACSEC_INTR_STATUS
 *
 * This register contains primary (leaf) interrupt sources as well as
 * secondary (daisy-chained) interrupt sources. The secondary sources
 * are
 *
 *  - SEC_EBCFG_CPU_INT_STAT (8 register instances)
 *  - SEC_ITMCFG_CPU_INT_STAT (2 register instances)
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

#include <bcmbd/bcmbd_sec_intr_internal.h>
#include <bcmbd/bcmbd_cmicx_intr.h>
#include <bcmbd/chip/bcm56780_a0_acc.h>
#include <bcmbd/chip/bcm56780_a0_lp_intr.h>
#include <bcmbd/chip/bcm56780_a0_sec_intr.h>

#include "bcm56780_a0_drv.h"

#define  BSL_LOG_MODULE  BSL_LS_BCMBD_INTR

/*******************************************************************************
 * Local definitions
 */

#define MAX_UNITS        BCMDRD_CONFIG_MAX_UNITS
#define MAX_INTRS        SEC_INTR_MAX

#define CMIC_SEC_INTR0   MACSEC0_TO_CMIC_PERR_INTR
#define CMIC_SEC_INTR1   MACSEC1_TO_CMIC_PERR_INTR
#define CMIC_SEC_INTR2   MACSEC2_TO_CMIC_PERR_INTR
#define CMIC_SEC_INTR3   MACSEC3_TO_CMIC_PERR_INTR
#define CMIC_SEC_INTR4   MACSEC4_TO_CMIC_PERR_INTR
#define CMIC_SEC_INTR5   MACSEC5_TO_CMIC_PERR_INTR
#define CMIC_SEC_COUNT   6

#define MAX_SEC_BLK      6

typedef struct bcmbd_intr_vect_s {
    bcmbd_intr_f func;
    uint32_t param;
} bcmbd_intr_vect_t;

typedef struct bcmbd_intr_vect_info_s {
    bcmbd_intr_vect_t intr_vects[MAX_INTRS];
} bcmbd_intr_vect_info_t;

#define INTR_MASK_LOCK(_u) sal_spinlock_lock(sec_intr_lock[_u]);
#define INTR_MASK_UNLOCK(_u) sal_spinlock_unlock(sec_intr_lock[_u]);

/*******************************************************************************
 * Local data
 */

static bool sec_intr_connected[MAX_UNITS];
static sal_spinlock_t sec_intr_lock[MAX_UNITS];
static bcmbd_intr_vect_info_t sec_intr_vect_info[MAX_UNITS];
static shr_thread_t sec_thread_ctrl[MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

static void
sec_intr_update(int unit, unsigned int intr_num, int enable)
{
    int i;
    MACSEC_INTR_ENABLEr_t macsec_en;
    INTR_MASK_LOCK(unit);

    for (i = 0; i < MAX_SEC_BLK; i++) {
        READ_MACSEC_INTR_ENABLEr(unit, &macsec_en, i);
        if (intr_num == SEC_INTR_SA_EXPIRY_ENCRYPT) {
            MACSEC_INTR_ENABLEr_ESEC_SA_EXP_FIFO_NON_EMPTYf_SET(macsec_en, enable);
        }
        if (intr_num == SEC_INTR_SA_EXPIRY_DECRYPT) {
            MACSEC_INTR_ENABLEr_ISEC_SA_EXP_FIFO_NON_EMPTYf_SET(macsec_en, enable);
        }
        WRITE_MACSEC_INTR_ENABLEr(unit, macsec_en, i);
    }

    INTR_MASK_UNLOCK(unit);

    /* Not required for SER event types. */
    return;
}

static void
sec_intr_enable(int unit, unsigned int intr_num)
{
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                          "Enable SEC intr %u\n"),
                 intr_num));
    if (intr_num >= MAX_INTRS) {
        return;
    }
    sec_intr_update(unit, intr_num, 1);
}

static void
sec_intr_disable(int unit, unsigned int intr_num)
{
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                          "Disable SEC intr %u\n"),
                 intr_num));
    if (intr_num >= MAX_INTRS) {
        return;
    }
    sec_intr_update(unit, intr_num, 0);
}

static void
sec_intr_clear(int unit, unsigned int intr_num)
{
    /* Not required. Interrupts are clear on read. */
    return;
}

static void
sec_intr(int unit)
{
    int i;
    MACSEC_INTR_STATUSr_t status;
    SHR_BITDCLNAME(bmp, MAX_INTRS);
    unsigned int intr_num;
    bcmbd_intr_vect_t *iv;

    INTR_MASK_LOCK(unit);

    sal_memset(&bmp, 0, sizeof(bmp));
    for (i = 0; i < MAX_SEC_BLK; i++) {
        READ_MACSEC_INTR_STATUSr(unit, &status, i);
        if (MACSEC_INTR_STATUSr_ESEC_SA_EXP_FIFO_NON_EMPTYf_GET(status)) {
            SHR_BITSET(bmp, SEC_INTR_SA_EXPIRY_ENCRYPT);
        }
        if (MACSEC_INTR_STATUSr_ISEC_SA_EXP_FIFO_NON_EMPTYf_GET(status)) {
            SHR_BITSET(bmp, SEC_INTR_SA_EXPIRY_DECRYPT);
        }
        SHR_BIT_ITER(bmp, MAX_INTRS, intr_num) {
            if (SHR_BITGET(bmp, intr_num) == 0) {
                continue;
            }
            LOG_DEBUG(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                        "SEC interrupt %u, blk_id %u\n"),
                        intr_num, i));
            iv = &sec_intr_vect_info[unit].intr_vects[intr_num];
            if (iv->func) {
                uint32_t param = intr_num;
                SEC_INTR_INST_SET(param, i);
                iv->func(unit, param);
            } else {
                LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                            "No handler for SEC interrupt %u, blk_id %u\n"),
                            intr_num, i));
                sec_intr_disable(unit, intr_num);
            }
        }
    }

    INTR_MASK_UNLOCK(unit);

    return;
}

static int
sec_cmic_lp_intr_enable(int unit, int enable)
{
    int i;
    int sec_cmic_intr[] = {
        CMIC_SEC_INTR0, CMIC_SEC_INTR1, CMIC_SEC_INTR2,
        CMIC_SEC_INTR3, CMIC_SEC_INTR4, CMIC_SEC_INTR5
    };
    for (i = 0; i < CMIC_SEC_COUNT; i++) {
        if (enable) {
            bcmbd_cmicx_lp_intr_enable(unit, sec_cmic_intr[i]);
        } else {
            bcmbd_cmicx_lp_intr_disable(unit, sec_cmic_intr[i]);
        }
    }
    return 0;
}

static void
sec_thread(shr_thread_t tc, void *arg)
{
    int unit = (uintptr_t)arg;

    while (1) {
        shr_thread_sleep(tc, SHR_THREAD_FOREVER);
        if (shr_thread_stopping(tc)) {
            break;
        }
        sec_intr(unit);
        sec_cmic_lp_intr_enable(unit, 1);
    }
}

static void
sec_isr(int unit, uint32_t param)
{
    /* Disable interrupt and wake up SEC thread */
    bcmbd_cmicx_lp_intr_disable_nosync(unit, param);
    shr_thread_wake(sec_thread_ctrl[unit]);
}

static int
sec_intr_init(int unit)
{
    shr_thread_t tc;
    void *arg;

    if (sec_intr_lock[unit] != NULL) {
        return SHR_E_INTERNAL;
    }

    sec_intr_lock[unit] = sal_spinlock_create("bcmbdSecIntr");
    if (sec_intr_lock[unit] == NULL) {
        return SHR_E_FAIL;
    }

    if (sec_thread_ctrl[unit] != NULL) {
        return SHR_E_INTERNAL;
    }

    /* Pass in unit number as context */
    arg = (void *)(uintptr_t)unit;

    tc = shr_thread_start("bcmbdSecIntr", -1, sec_thread, arg);
    if (tc == NULL) {
        return SHR_E_FAIL;
    }

    sec_thread_ctrl[unit] = tc;

    return SHR_E_NONE;
}

static int
sec_intr_cleanup(int unit)
{
    int rv = SHR_E_NONE;

    sec_cmic_lp_intr_enable(unit, 0);
    sec_intr_connected[unit] = false;

    if (sec_thread_ctrl[unit] != NULL) {
        rv = shr_thread_stop(sec_thread_ctrl[unit], 2 * SECOND_USEC);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        sec_thread_ctrl[unit] = NULL;
    }

    if (sec_intr_lock[unit] != NULL) {
        rv = sal_spinlock_destroy(sec_intr_lock[unit]);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        sec_intr_lock[unit] = NULL;
    }

    return rv;
}

static int
sec_intr_func_set(int unit, unsigned int intr_num,
                  bcmbd_intr_f intr_func, uint32_t intr_param)
{
    int i;
    int sec_cmic_intr[] = {
        CMIC_SEC_INTR0, CMIC_SEC_INTR1, CMIC_SEC_INTR2,
        CMIC_SEC_INTR3, CMIC_SEC_INTR4, CMIC_SEC_INTR5
    };
    bcmbd_intr_vect_t *iv;

    if (!sec_intr_connected[unit]) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Add handler for SEC interrupt %u\n"),
                   intr_num));
        for (i = 0; i < CMIC_SEC_COUNT; i++) {
            bcmbd_cmicx_lp_intr_func_set(unit, sec_cmic_intr[i],
                                         sec_isr, sec_cmic_intr[i]);
        }
        sec_cmic_lp_intr_enable(unit, 1);
        sec_intr_connected[unit] = true;
    }

    if (intr_num >= MAX_INTRS) {
        return SHR_E_PARAM;
    }

    iv = &sec_intr_vect_info[unit].intr_vects[intr_num];
    iv->func = intr_func;
    iv->param = intr_param;

    return SHR_E_NONE;
}

static bcmbd_intr_drv_t sec_intr_drv = {
    .intr_func_set = sec_intr_func_set,
    .intr_enable = sec_intr_enable,
    .intr_disable = sec_intr_disable,
    .intr_clear = sec_intr_clear,
};

/*******************************************************************************
 * Public functions
 */

int
bcm56780_a0_bd_sec_intr_drv_init(int unit)
{
    int rv;

    rv = bcmbd_sec_intr_drv_init(unit, &sec_intr_drv);
    if (SHR_SUCCESS(rv)) {
        rv = sec_intr_init(unit);
        if (SHR_FAILURE(rv)) {
            (void)sec_intr_cleanup(unit);
        }
    }

    return rv;
}

int
bcm56780_a0_bd_sec_intr_drv_cleanup(int unit)
{
    (void)bcmbd_sec_intr_drv_init(unit, NULL);
    return sec_intr_cleanup(unit);
}
