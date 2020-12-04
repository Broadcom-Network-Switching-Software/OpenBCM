/*! \file bcma_testcase_swi_cmicx2.c
 *
 * Broadcom test case functions for CMICx2 interrupts
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_types.h>
#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <sal/sal_sleep.h>

#include <bcmbd/bcmbd_cmicx_intr.h>
#include <bcmbd/bcmbd_cmicx2_acc.h>

#include "bcma_testcase_cmicx_intr_internal.h"

static void
swi_intr_pcie(int unit, uint32_t intr_param)
{
    ICFG_PCIE_SW_PROG_INTRr_t pcie_spi;

    bcma_testcase_cmicx_intr_cnt++;
    bcma_testcase_cmicx_intr_num = intr_param;

    /* Clear interrupt condition */
    ICFG_PCIE_SW_PROG_INTRr_CLR(pcie_spi);
    ICFG_PCIE_SW_PROG_INTRr_PCIEf_SET(pcie_spi, 0);
    WRITE_ICFG_PCIE_SW_PROG_INTRr(unit, pcie_spi);
}

static void
swi_trig_pcie(int unit)
{
    ICFG_PCIE_SW_PROG_INTRr_t pcie_spi;

    /* Set software interrupt */
    ICFG_PCIE_SW_PROG_INTRr_CLR(pcie_spi);
    ICFG_PCIE_SW_PROG_INTRr_PCIEf_SET(pcie_spi, 1);
    WRITE_ICFG_PCIE_SW_PROG_INTRr(unit, pcie_spi);
}

static void
swi_intr_rts0mhost0(int unit, uint32_t intr_param)
{
    ICFG_RTS0_MHOST0_SW_PROG_INTRr_t mh0_spi;

    bcma_testcase_cmicx_intr_cnt++;
    bcma_testcase_cmicx_intr_num = intr_param;

    /* Clear interrupt condition */
    ICFG_RTS0_MHOST0_SW_PROG_INTRr_CLR(mh0_spi);
    ICFG_RTS0_MHOST0_SW_PROG_INTRr_PCIEf_SET(mh0_spi, 0);
    WRITE_ICFG_RTS0_MHOST0_SW_PROG_INTRr(unit, mh0_spi);
}

static void
swi_trig_rts0mhost0(int unit)
{
    ICFG_RTS0_MHOST0_SW_PROG_INTRr_t mh0_spi;

    /* Set software interrupt */
    ICFG_RTS0_MHOST0_SW_PROG_INTRr_CLR(mh0_spi);
    ICFG_RTS0_MHOST0_SW_PROG_INTRr_PCIEf_SET(mh0_spi, 1);
    WRITE_ICFG_RTS0_MHOST0_SW_PROG_INTRr(unit, mh0_spi);
}

static void
swi_intr_rts0mhost1(int unit, uint32_t intr_param)
{
    ICFG_RTS0_MHOST1_SW_PROG_INTRr_t mh1_spi;

    bcma_testcase_cmicx_intr_cnt++;
    bcma_testcase_cmicx_intr_num = intr_param;

    /* Clear interrupt condition */
    ICFG_RTS0_MHOST1_SW_PROG_INTRr_CLR(mh1_spi);
    ICFG_RTS0_MHOST1_SW_PROG_INTRr_PCIEf_SET(mh1_spi, 0);
    WRITE_ICFG_RTS0_MHOST1_SW_PROG_INTRr(unit, mh1_spi);
}

static void
swi_trig_rts0mhost1(int unit)
{
    ICFG_RTS0_MHOST1_SW_PROG_INTRr_t mh1_spi;

    /* Set software interrupt */
    ICFG_RTS0_MHOST1_SW_PROG_INTRr_CLR(mh1_spi);
    ICFG_RTS0_MHOST1_SW_PROG_INTRr_PCIEf_SET(mh1_spi, 1);
    WRITE_ICFG_RTS0_MHOST1_SW_PROG_INTRr(unit, mh1_spi);
}

static void
swi_intr_rts1mhost0(int unit, uint32_t intr_param)
{
    ICFG_RTS1_MHOST0_SW_PROG_INTRr_t mh0_spi;

    bcma_testcase_cmicx_intr_cnt++;
    bcma_testcase_cmicx_intr_num = intr_param;

    /* Clear interrupt condition */
    ICFG_RTS1_MHOST0_SW_PROG_INTRr_CLR(mh0_spi);
    ICFG_RTS1_MHOST0_SW_PROG_INTRr_PCIEf_SET(mh0_spi, 0);
    WRITE_ICFG_RTS1_MHOST0_SW_PROG_INTRr(unit, mh0_spi);
}

static void
swi_trig_rts1mhost0(int unit)
{
    ICFG_RTS1_MHOST0_SW_PROG_INTRr_t mh0_spi;

    /* Set software interrupt */
    ICFG_RTS1_MHOST0_SW_PROG_INTRr_CLR(mh0_spi);
    ICFG_RTS1_MHOST0_SW_PROG_INTRr_PCIEf_SET(mh0_spi, 1);
    WRITE_ICFG_RTS1_MHOST0_SW_PROG_INTRr(unit, mh0_spi);
}

static void
swi_intr_rts1mhost1(int unit, uint32_t intr_param)
{
    ICFG_RTS1_MHOST1_SW_PROG_INTRr_t mh1_spi;

    bcma_testcase_cmicx_intr_cnt++;
    bcma_testcase_cmicx_intr_num = intr_param;

    /* Clear interrupt condition */
    ICFG_RTS1_MHOST1_SW_PROG_INTRr_CLR(mh1_spi);
    ICFG_RTS1_MHOST1_SW_PROG_INTRr_PCIEf_SET(mh1_spi, 0);
    WRITE_ICFG_RTS1_MHOST1_SW_PROG_INTRr(unit, mh1_spi);
}

static void
swi_trig_rts1mhost1(int unit)
{
    ICFG_RTS1_MHOST1_SW_PROG_INTRr_t mh1_spi;

    /* Set software interrupt */
    ICFG_RTS1_MHOST1_SW_PROG_INTRr_CLR(mh1_spi);
    ICFG_RTS1_MHOST1_SW_PROG_INTRr_PCIEf_SET(mh1_spi, 1);
    WRITE_ICFG_RTS1_MHOST1_SW_PROG_INTRr(unit, mh1_spi);
}


static void
swi_intr_rcpu(int unit, uint32_t intr_param)
{
    ICFG_CMIC_RCPU_SW_PROG_INTRr_t rcpu_spi;

    bcma_testcase_cmicx_intr_cnt++;
    bcma_testcase_cmicx_intr_num = intr_param;

    /* Clear interrupt condition */
    ICFG_CMIC_RCPU_SW_PROG_INTRr_CLR(rcpu_spi);
    ICFG_CMIC_RCPU_SW_PROG_INTRr_PCIEf_SET(rcpu_spi, 0);
    WRITE_ICFG_CMIC_RCPU_SW_PROG_INTRr(unit, rcpu_spi);
}

static void
swi_trig_rcpu(int unit)
{
    ICFG_CMIC_RCPU_SW_PROG_INTRr_t rcpu_spi;

    /* Set software interrupt */
    ICFG_CMIC_RCPU_SW_PROG_INTRr_CLR(rcpu_spi);
    ICFG_CMIC_RCPU_SW_PROG_INTRr_PCIEf_SET(rcpu_spi, 1);
    WRITE_ICFG_CMIC_RCPU_SW_PROG_INTRr(unit, rcpu_spi);
}

static bcma_testcase_swi_test_t cmicx2_swi_test[] = {
    {"PCIE",       swi_intr_pcie,   swi_trig_pcie},
    {"rts0mHost0", swi_intr_rts0mhost0, swi_trig_rts0mhost0},
    {"rts0mHost1", swi_intr_rts0mhost1, swi_trig_rts0mhost1},
    {"rts1mHost0", swi_intr_rts1mhost0, swi_trig_rts1mhost0},
    {"rts1mHost1", swi_intr_rts1mhost1, swi_trig_rts1mhost1},
    {"RCPU",       swi_intr_rcpu,   swi_trig_rcpu},
};

int
bcma_testcase_cmicx2_intr_func_get(int unit, bcma_testcase_swi_test_t **swi_test,
                                   int *count, char **def_host)
{
    if (swi_test != NULL) {
        *swi_test = cmicx2_swi_test;
    }

    if (count != NULL) {
        *count    = COUNTOF(cmicx2_swi_test);
    }

    if (def_host != NULL) {
        *def_host = cmicx2_swi_test[1].host;
    }

    return SHR_E_NONE;
}

