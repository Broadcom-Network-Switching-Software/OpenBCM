/*! \file bcmbd_cmicx2_sw_intr.c
 *
 * CMICx software interrupt handler framework.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <shr/shr_error.h>

#include <bcmdrd/bcmdrd_types.h>

#include <bcmbd/bcmbd_cmicx.h>
#include <bcmbd/bcmbd_cmicx2_acc.h>
#include <bcmbd/bcmbd_cmicx_intr.h>
#include <bcmbd/bcmbd_cmicx2_sw_intr.h>
#include <bcmbd/bcmbd_sw_intr_internal.h>
#include <bcmbd/bcmbd_m0ssq.h>
#include <bcmbd/bcmbd_mcs.h>

#define BSL_LOG_MODULE  BSL_LS_BCMBD_DEV

/*******************************************************************************
 * Local definitions
 */

#define MAX_INTRS       CMICX2_MAX_SWI_INTRS
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

/*******************************************************************************
 * Local data
 */

static intr_vect_info_t sw_intr_vect_info[MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

static void
cmicx2_sw_intr(int unit, uint32_t param)
{
    intr_vect_info_t *ivi = &sw_intr_vect_info[unit];
    intr_vect_t *iv;
    uint32_t stat, intr, uc;
    PAXB_0_SW_PROG_INTR_STATUSr_t pb_stat;
    PAXB_0_SW_PROG_INTR_CLRr_t pb_clr;

    /* Read the Masked Interrupt Status Register */
    READ_PAXB_0_SW_PROG_INTR_STATUSr(unit, &pb_stat);
    stat = PAXB_0_SW_PROG_INTR_STATUSr_VALf_GET(pb_stat);

    /* Handle all received SW interrupts */
    for (uc = 0; uc < MAX_INTRS; uc++) {
        intr = (1 << uc);
        if (stat & intr) {
            /* Received an interrupt. Clear it */
            PAXB_0_SW_PROG_INTR_CLRr_VALf_SET(pb_clr, intr);
            WRITE_PAXB_0_SW_PROG_INTR_CLRr(unit, pb_clr);
            /* Invoke handler function. */
            iv = &ivi->intr_vects[uc];
            if (iv->func && iv->enabled) {
                iv->func(unit, iv->param);
            }
        }
    }
}

static int
cmicx2_sw_intr_func_set(int unit, unsigned int intr_num,
                       bcmbd_intr_f intr_func, uint32_t intr_param)
{
    intr_vect_t *vi;

    if (!INTR_VALID(intr_num)) {
        return SHR_E_PARAM;
    }

    vi = &sw_intr_vect_info[unit].intr_vects[intr_num];
    vi->func = intr_func;
    vi->param= intr_param;

    return SHR_E_NONE;
}

static void
cmicx2_sw_intr_enable(int unit, unsigned int intr_num)
{
    PAXB_0_SW_PROG_INTR_ENABLEr_t pb_en;
    uint32_t en, mask;

    if (INTR_VALID(intr_num)) {
        mask = (1 << intr_num);

        READ_PAXB_0_SW_PROG_INTR_ENABLEr(unit, &pb_en);
        en = PAXB_0_SW_PROG_INTR_ENABLEr_VALf_GET(pb_en);
        en |= mask;
        PAXB_0_SW_PROG_INTR_ENABLEr_VALf_SET(pb_en, en);
        WRITE_PAXB_0_SW_PROG_INTR_ENABLEr(unit, pb_en);

        sw_intr_vect_info[unit].intr_vects[intr_num].enabled = true;
    }
}

static void
cmicx2_sw_intr_disable(int unit, unsigned int intr_num)
{
    PAXB_0_SW_PROG_INTR_ENABLEr_t pb_en;
    uint32_t en, mask;

    if (INTR_VALID(intr_num)) {
        mask = ~(1 << intr_num);

        READ_PAXB_0_SW_PROG_INTR_ENABLEr(unit, &pb_en);
        en = PAXB_0_SW_PROG_INTR_ENABLEr_VALf_GET(pb_en);
        en &= mask;
        PAXB_0_SW_PROG_INTR_ENABLEr_VALf_SET(pb_en, en);
        WRITE_PAXB_0_SW_PROG_INTR_ENABLEr(unit, pb_en);

        sw_intr_vect_info[unit].intr_vects[intr_num].enabled = false;
    }
}

/* HMI-specific software interrupt driver. */
static bcmbd_intr_drv_t cmicx2_sw_intr_drv = {
    .intr_func_set = cmicx2_sw_intr_func_set,
    .intr_enable = cmicx2_sw_intr_enable,
    .intr_disable = cmicx2_sw_intr_disable
};

/*******************************************************************************
 * Public functions
 */

int
bcmbd_cmicx2_sw_intr_drv_init(int unit)
{
    sal_memset((void *)&sw_intr_vect_info[unit], 0, sizeof(intr_vect_info_t));
    return bcmbd_sw_intr_drv_init(unit, &cmicx2_sw_intr_drv);
}

int
bcmbd_cmicx2_sw_intr_stop(int unit)
{
    /* Disable software interrupt */
    bcmbd_cmicx_intr_disable(unit, IPROC_IRQ_SW_PROG_INTR);

    return SHR_E_NONE;
}

int
bcmbd_cmicx2_sw_intr_start(int unit)
{
    int rv;

    /* Install software interrupt handler */
    rv = bcmbd_cmicx_intr_func_set(unit, IPROC_IRQ_SW_PROG_INTR,
                                   cmicx2_sw_intr, 0);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Enable software interrupt */
    bcmbd_cmicx_intr_enable(unit, IPROC_IRQ_SW_PROG_INTR);

    return SHR_E_NONE;
}
