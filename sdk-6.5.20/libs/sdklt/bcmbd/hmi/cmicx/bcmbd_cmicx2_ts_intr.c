/*! \file bcmbd_cmicx2_ts_intr.c
 *
 * CMICx timesync interrupt handler framework.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <sal/sal_spinlock.h>
#include <sal/sal_sem.h>
#include <shr/shr_error.h>

#include <bcmdrd/bcmdrd_types.h>

#include <bcmbd/bcmbd_cmicx.h>
#include <bcmbd/bcmbd_cmicx2_acc.h>
#include <bcmbd/bcmbd_cmicx_intr.h>
#include <bcmbd/bcmbd_cmicx2_ts_intr.h>
#include <bcmbd/bcmbd_ts_intr_internal.h>


#define BSL_LOG_MODULE  BSL_LS_BCMBD_DEV

/*******************************************************************************
 * Local definitions
 */

#define MAX_INTRS       CMICX_MAX_TS_INTRS
#define MAX_UNITS       BCMDRD_CONFIG_MAX_UNITS

#define INTR_VALID(_i) ((unsigned int)(_i) < MAX_INTRS)

typedef struct intr_vect_s {
    bcmbd_intr_f func;
    uint32_t param;
    bool enabled;
} intr_vect_t;

typedef struct intr_vect_info_s {
    intr_vect_t intr_vects[MAX_INTRS];
} intr_vect_info_t;

#define INTR_MASK_LOCK(_u) sal_spinlock_lock(ts_intr_lock[_u]);
#define INTR_MASK_UNLOCK(_u) sal_spinlock_unlock(ts_intr_lock[_u]);

/*******************************************************************************
 * Local data
 */

static intr_vect_info_t ts_intr_vect_info[MAX_UNITS];
static sal_spinlock_t ts_intr_lock[MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

static void
cmicx2_ts_intr(int unit, uint32_t param)
{
    intr_vect_info_t *ivi = &ts_intr_vect_info[unit];
    intr_vect_t *iv;
    uint32_t stat, intr, ts;
    NS_TS_INT_STATUSr_t ts_stat;

    /* Read the Masked Interrupt Status Register */
    READ_NS_TS_INT_STATUSr(unit, &ts_stat);
    stat = NS_TS_INT_STATUSr_GET(ts_stat);

    /* Handle all received TS interrupts */
    for (ts = 0; ts < MAX_INTRS; ts++) {
        intr = (1 << ts);
        if (stat & intr) {
            /* Received an interrupt. Invoke handler function. */
            iv = &ivi->intr_vects[ts];
            if (iv->func && iv->enabled) {
                iv->func(unit, iv->param);
            }
        }
    }
}

static int
cmicx2_ts_intr_func_set(int unit, unsigned int intr_num,
                       bcmbd_intr_f intr_func, uint32_t intr_param)
{
    intr_vect_t *vi;

    if (!INTR_VALID(intr_num)) {
        return SHR_E_PARAM;
    }

    vi = &ts_intr_vect_info[unit].intr_vects[intr_num];
    vi->func = intr_func;
    vi->param= intr_param;

    return SHR_E_NONE;
}

static void
cmicx2_ts_intr_enable(int unit, unsigned int intr_num)
{
    NS_TS_INT_ENABLEr_t ts_en;
    uint32_t en, mask;

    /* We may be called by multiple application threads */
    INTR_MASK_LOCK(unit);
    if (INTR_VALID(intr_num)) {
        mask = (1 << intr_num);

        READ_NS_TS_INT_ENABLEr(unit, &ts_en);
        en = NS_TS_INT_ENABLEr_GET(ts_en);
        en |= mask;
        NS_TS_INT_ENABLEr_SET(ts_en, en);
        WRITE_NS_TS_INT_ENABLEr(unit, ts_en);

        ts_intr_vect_info[unit].intr_vects[intr_num].enabled = true;
    }
    INTR_MASK_UNLOCK(unit);
}

static void
cmicx2_ts_intr_disable(int unit, unsigned int intr_num)
{
    NS_TS_INT_ENABLEr_t ts_en;
    uint32_t en, mask;

    /* We may be called by multiple application threads */
    INTR_MASK_LOCK(unit);
    if (INTR_VALID(intr_num)) {
        mask = ~(1 << intr_num);

        READ_NS_TS_INT_ENABLEr(unit, &ts_en);
        en = NS_TS_INT_ENABLEr_GET(ts_en);
        en &= mask;
        NS_TS_INT_ENABLEr_SET(ts_en, en);
        WRITE_NS_TS_INT_ENABLEr(unit, ts_en);

        ts_intr_vect_info[unit].intr_vects[intr_num].enabled = false;
    }
    INTR_MASK_UNLOCK(unit);
}

static void
cmicx2_ts_intr_clear(int unit, unsigned int intr_num)
{
    NS_TS_INT_STATUSr_t ts_stat;
    uint32_t mask;

    if (INTR_VALID(intr_num)) {
        mask = (1 << intr_num);
        NS_TS_INT_STATUSr_SET(ts_stat, mask);
        WRITE_NS_TS_INT_STATUSr(unit, ts_stat);
    }

}
static int
cmicx2_ts_intr_init(int unit)
{
    if (ts_intr_lock[unit] != NULL) {
        return SHR_E_INTERNAL;
    }
    ts_intr_lock[unit] = sal_spinlock_create("bcmbdCmicxTsIntr");
    if (ts_intr_lock[unit] == NULL) {
        return SHR_E_FAIL;
    }
    return SHR_E_NONE;
}

static int
cmicx2_ts_intr_cleanup(int unit)
{
    int rv = SHR_E_NONE;

    if (ts_intr_lock[unit] != NULL) {
        rv = sal_spinlock_destroy(ts_intr_lock[unit]);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        ts_intr_lock[unit] = NULL;
    }

    return rv;
}

/* HMI-specific timesync interrupt driver. */
static bcmbd_intr_drv_t cmicx2_ts_intr_drv = {
    .intr_func_set = cmicx2_ts_intr_func_set,
    .intr_enable = cmicx2_ts_intr_enable,
    .intr_disable = cmicx2_ts_intr_disable,
    .intr_clear = cmicx2_ts_intr_clear
};

/*******************************************************************************
 * Public functions
 */

int
bcmbd_cmicx2_ts_intr_drv_init(int unit)
{
    int rv;

    sal_memset((void *)&ts_intr_vect_info[unit], 0, sizeof(intr_vect_info_t));
    rv = bcmbd_ts_intr_drv_init(unit, &cmicx2_ts_intr_drv);
    if (SHR_SUCCESS(rv)) {
        rv = cmicx2_ts_intr_init(unit);
        if (SHR_FAILURE(rv)) {
            (void)cmicx2_ts_intr_cleanup(unit);
        }
    }
    return rv;
}

int
bcmbd_cmicx2_ts_intr_stop(int unit)
{
    /* Disable timesync interrupt */
    bcmbd_cmicx_intr_disable(unit, IPROC_IRQ_TS_INTR);

    return SHR_E_NONE;
}

int
bcmbd_cmicx2_ts_intr_start(int unit)
{
    int rv;

    /* Install timesync interrupt handler */
    rv = bcmbd_cmicx_intr_func_set(unit, IPROC_IRQ_TS_INTR,
                                   cmicx2_ts_intr, 0);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Enable timesync interrupt */
    bcmbd_cmicx_intr_enable(unit, IPROC_IRQ_TS_INTR);

    return SHR_E_NONE;
}

int
bcmbd_cmicx2_ts_intr_drv_cleanup(int unit)
{
    (void)bcmbd_ts_intr_drv_init(unit, NULL);
    return cmicx2_ts_intr_cleanup(unit);
}
