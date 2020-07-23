/*! \file bcmbd_cmicx_sw_intr.c
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
#include <bcmbd/bcmbd_cmicx_acc.h>
#include <bcmbd/bcmbd_cmicx_intr.h>
#include <bcmbd/bcmbd_cmicx_sw_intr.h>
#include <bcmbd/bcmbd_sw_intr_internal.h>
#include <bcmbd/bcmbd_m0ssq.h>

#define BSL_LOG_MODULE  BSL_LS_BCMBD_DEV

/*******************************************************************************
 * Local definitions
 */

#define MAX_INTRS       CMICX_MAX_SWI_INTRS
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

/*! \brief Host CPU interrupt bit.
 *
 * This bit corresponds to the host CPU bit in the SW_PROG registers.
 *
 * Bit 0: PCI
 * Bit 1: MHOST0
 * Bit 2: MHOST1
 *
 * By default we assume to be connected via PCI.
 */
static uint32_t host_cpu_intr = 0x1;

/*******************************************************************************
 * Private functions
 */

static void
cmicx_sw_intr(int unit, uint32_t param)
{
    intr_vect_info_t *ivi = &sw_intr_vect_info[unit];
    intr_vect_t *iv;
    ICFG_MHOST0_SW_PROG_INTRr_t mhost0;
    ICFG_MHOST1_SW_PROG_INTRr_t mhost1;
    ICFG_CORTEXM0_SW_PROG_INTRr_t m0_intr;
    uint32_t intr, uc, uc_max;

    iv = &ivi->intr_vects[CMICX_SWI_MHOST0];
    READ_ICFG_MHOST0_SW_PROG_INTRr(unit, &mhost0);
    intr = ICFG_MHOST0_SW_PROG_INTRr_GET(mhost0);
    if (intr & host_cpu_intr) {
        ICFG_MHOST0_SW_PROG_INTRr_CLR(mhost0);
        WRITE_ICFG_MHOST0_SW_PROG_INTRr(unit, mhost0);
        if (iv->func && iv->enabled) {
            iv->func(unit, iv->param);
        }
    }

    iv = &ivi->intr_vects[CMICX_SWI_MHOST1];
    READ_ICFG_MHOST1_SW_PROG_INTRr(unit, &mhost1);
    intr = ICFG_MHOST1_SW_PROG_INTRr_GET(mhost1);
    if (intr & host_cpu_intr) {
        ICFG_MHOST1_SW_PROG_INTRr_CLR(mhost1);
        WRITE_ICFG_MHOST1_SW_PROG_INTRr(unit, mhost1);
        if (iv->func && iv->enabled) {
            iv->func(unit, iv->param);
        }
    }

    uc_max = bcmbd_m0ssq_uc_num_get(unit);
    for (uc = 0; uc < uc_max; uc++) {

        /* Check M0 to PCIe software interrupt. */
        READ_ICFG_CORTEXM0_SW_PROG_INTRr(unit, uc, &m0_intr);
        intr = ICFG_CORTEXM0_SW_PROG_INTRr_PCIEf_GET(m0_intr);

        if (intr) {

            /* Clear M0 to PCIe interrupt. */
            ICFG_CORTEXM0_SW_PROG_INTRr_PCIEf_SET(m0_intr, 0);
            WRITE_ICFG_CORTEXM0_SW_PROG_INTRr(unit, uc, m0_intr);

            /* Invoke handle function. */
            iv = &ivi->intr_vects[CMICX_SWI_CORTEXM0_U0 + uc];
            if (iv->func && iv->enabled) {
                iv->func(unit, iv->param);
            }
        }
    }
}

static int
cmicx_sw_intr_func_set(int unit, unsigned int intr_num,
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
cmicx_sw_intr_enable(int unit, unsigned int intr_num)
{
    if (INTR_VALID(intr_num)) {
        sw_intr_vect_info[unit].intr_vects[intr_num].enabled = true;
    }
}

static void
cmicx_sw_intr_disable(int unit, unsigned int intr_num)
{
    if (INTR_VALID(intr_num)) {
        sw_intr_vect_info[unit].intr_vects[intr_num].enabled = false;
    }
}

/* HMI-specific software interrupt driver. */
static bcmbd_intr_drv_t cmicx_sw_intr_drv = {
    .intr_func_set = cmicx_sw_intr_func_set,
    .intr_enable = cmicx_sw_intr_enable,
    .intr_disable = cmicx_sw_intr_disable
};

/*******************************************************************************
 * Public functions
 */

int
bcmbd_cmicx_sw_intr_drv_init(int unit)
{
    sal_memset((void *)&sw_intr_vect_info[unit], 0, sizeof(intr_vect_info_t));
    return bcmbd_sw_intr_drv_init(unit, &cmicx_sw_intr_drv);
}

int
bcmbd_cmicx_sw_intr_stop(int unit)
{
    /* Disable software interrupt */
    bcmbd_cmicx_intr_disable(unit, IPROC_IRQ_ICFG_SW_PROG_INTR);

    return SHR_E_NONE;
}

int
bcmbd_cmicx_sw_intr_start(int unit)
{
    int rv;

    /* Install software interrupt handler */
    rv = bcmbd_cmicx_intr_func_set(unit, IPROC_IRQ_ICFG_SW_PROG_INTR,
                                   cmicx_sw_intr, 0);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Enable software interrupt */
    bcmbd_cmicx_intr_enable(unit, IPROC_IRQ_ICFG_SW_PROG_INTR);

    return SHR_E_NONE;
}
