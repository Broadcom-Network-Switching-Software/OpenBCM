/*! \file bcm56880_a0_bd_dev_reset.c
 *
 * Issue device reset and pull all chip blocks out of reset.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <shr/shr_debug.h>
#include <shr/shr_pb.h>

#include <sal/sal_sleep.h>

#include <bcmdrd/bcmdrd_feature.h>
#include <bcmbd/bcmbd_config.h>
#include <bcmbd/chip/bcm56880_a0_cmic_bcast.h>
#include <bcmbd/chip/bcm56880_a0_acc.h>
#include <bcmbd/chip/bcm56880_a0_dev.h>
#include <bcmlrd/chip/bcm56880_a0/bcm56880_a0_lrd_enum_ctype.h>
#include "bcm56880_a0_drv.h"

/* Log source for this component */
#define BSL_LOG_MODULE                  BSL_LS_BCMBD_DEV

/*******************************************************************************
* Local definitions
 */

#define PLL_LOCK_USEC                   50

#define TD4_PIPES_PER_DEV               8
#define PORT_RESET_SLEEP_USEC           2
#define PORT_RESET_EMUL_SLEEP_USEC      10100

#define WAIT_MSEC                       10
#define RESET_WAIT_MSEC                 1
#define EMUL_WAIT_MSEC                  250

#define PIPE_RESET_HW_TIMEOUT_MSEC      50
#define PIPE_RESET_EMUL_TIMEOUT_MSEC    10000

#define CLK(mhz)      (BCM56880_A0_LRD_DEVICE_CLK_FREQ_T_T_CLK_##mhz)

#define TS_CLK(mhz)      (BCM56880_A0_LRD_DEVICE_TS_PLL_REF_CLK_T_T_CLK_##mhz)
#define BS_CLK(mhz)      (BCM56880_A0_LRD_DEVICE_BS_PLL_REF_CLK_T_T_CLK_##mhz)


typedef enum {
    TD4_TS_PLL,
    TD4_BS_PLL0,
    TD4_BS_PLL1,
    TD4_PLL_INVALID
} td4_plls_e;

/*******************************************************************************
* Private functions
 */

static int
block_port_get(int unit, int port, int blktype)
{
    const bcmdrd_chip_info_t *cinfo;
    bcmdrd_pblk_t pblk;

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return -1;
    }

    if (bcmdrd_chip_port_block(cinfo, port, blktype, &pblk)) {
        return -1;
    }

    return pblk.lane;
}

static int
xlport_pbmp_get(int unit, bcmdrd_pbmp_t *pbmp)
{
    return bcmdrd_dev_blktype_pbmp(unit, BLKTYPE_XLPORT, pbmp);
}

static int
cdport_pbmp_get(int unit, bcmdrd_pbmp_t *pbmp)
{
    return bcmdrd_dev_blktype_pbmp(unit, BLKTYPE_CDPORT, pbmp);
}

static int
xlport_reset(int unit, int port, bool emul)
{
    int ioerr = 0;
    XLPORT_MAC_CONTROLr_t xmac_ctrl;
    XLPORT_XGXS0_CTRL_REGr_t xgxs_ctrl;
    int reset_sleep_us = emul ? PORT_RESET_EMUL_SLEEP_USEC : PORT_RESET_SLEEP_USEC;

    ioerr += READ_XLPORT_MAC_CONTROLr(unit, &xmac_ctrl, port);
    XLPORT_MAC_CONTROLr_XMAC0_RESETf_SET(xmac_ctrl, 1);
    ioerr += WRITE_XLPORT_MAC_CONTROLr(unit, xmac_ctrl, port);
    sal_usleep(reset_sleep_us);
    XLPORT_MAC_CONTROLr_XMAC0_RESETf_SET(xmac_ctrl, 0);
    ioerr += WRITE_XLPORT_MAC_CONTROLr(unit, xmac_ctrl, port);

    /* Configure clock source */
    ioerr += READ_XLPORT_XGXS0_CTRL_REGr(unit, &xgxs_ctrl, port);
    XLPORT_XGXS0_CTRL_REGr_REFIN_ENf_SET(xgxs_ctrl, emul ? 0 : 1);
    ioerr += WRITE_XLPORT_XGXS0_CTRL_REGr(unit, xgxs_ctrl, port);

    /* Deassert power down */
    ioerr += READ_XLPORT_XGXS0_CTRL_REGr(unit, &xgxs_ctrl, port);
    XLPORT_XGXS0_CTRL_REGr_PWRDWNf_SET(xgxs_ctrl, 0);
    XLPORT_XGXS0_CTRL_REGr_IDDQf_SET(xgxs_ctrl, 0);
    ioerr += WRITE_XLPORT_XGXS0_CTRL_REGr(unit, xgxs_ctrl, port);
    sal_usleep(reset_sleep_us);

    /* Reset XGXS */
    ioerr += READ_XLPORT_XGXS0_CTRL_REGr(unit, &xgxs_ctrl, port);
    XLPORT_XGXS0_CTRL_REGr_RSTB_HWf_SET(xgxs_ctrl, 0);
    ioerr += WRITE_XLPORT_XGXS0_CTRL_REGr(unit, xgxs_ctrl, port);
    sal_usleep(reset_sleep_us);

    /* Bring XGXS out of reset */
    XLPORT_XGXS0_CTRL_REGr_RSTB_HWf_SET(xgxs_ctrl, 1);
    ioerr += WRITE_XLPORT_XGXS0_CTRL_REGr(unit, xgxs_ctrl, port);
    sal_usleep(reset_sleep_us);

    return ioerr;
}

static int
cdport_bcast_enable(int unit, int num_bcast, int *last_bcast_port)
{
    int ioerr = 0;
    int port, idx;
    int bcast_domain;
    int blk_id;
    bcmdrd_pbmp_t pbmp;
    CDPORT_SBUS_CONTROLr_t sbus_ctrl;

    for (idx = 0; idx < num_bcast; idx++) {
        last_bcast_port[idx] = -1;
    }

    cdport_pbmp_get(unit, &pbmp);
    BCMDRD_PBMP_ITER(pbmp, port) {
        /* ring number that port belong to */
        bcast_domain = (port < CDPORT_BCST1_1ST_PORT) ? 0 : 1;
        /* block id */
        blk_id = CDPORT_BCAST_BLK(bcast_domain);
        if (bcast_domain == 0) {
            /* Set last port number in the chain 0 */
            last_bcast_port[bcast_domain] = port;
        } else if (last_bcast_port[bcast_domain] < 0) {
            /* Set last port number in the chain 1 */
            last_bcast_port[bcast_domain] = port;
        }
        CDPORT_SBUS_CONTROLr_CLR(sbus_ctrl);
        CDPORT_SBUS_CONTROLr_SBUS_BCAST_BLOCK_IDf_SET(sbus_ctrl, blk_id);
        ioerr += WRITE_CDPORT_SBUS_CONTROLr(unit, sbus_ctrl, port);
    }

    for (idx = 0; idx < num_bcast; idx++) {
        if (last_bcast_port[idx] > 0 ) {
            port = last_bcast_port[idx];
            ioerr += READ_CDPORT_SBUS_CONTROLr(unit, &sbus_ctrl, port);
            CDPORT_SBUS_CONTROLr_SBUS_CHAIN_LASTf_SET(sbus_ctrl, 1);
            ioerr += WRITE_CDPORT_SBUS_CONTROLr(unit, sbus_ctrl, port);
        }
    }

    return ioerr;
}

static int
cdport_reset(int unit, bcmdrd_pbmp_t pbmp, bool emul, bool sim)
{
    int ioerr = 0;
    int port, idx;
    int last_bcast_port[CDPORT_NUM_BCAST];
    CDPORT_MAC_CONTROLr_t mac_ctrl;
    CDPORT_XGXS0_CTRL_REGr_t xgxs_ctrl;
    int reset_sleep_us = emul ? PORT_RESET_EMUL_SLEEP_USEC : PORT_RESET_SLEEP_USEC;

    if (sim) {
        CDPORT_XGXS0_CTRL_REGr_CLR(xgxs_ctrl);
        CDPORT_XGXS0_CTRL_REGr_TSC_RSTBf_SET(xgxs_ctrl, 1);
        /* Simulate that TSC is powered up and out of reset. */
        BCMDRD_PBMP_ITER(pbmp, port) {
            ioerr += WRITE_CDPORT_XGXS0_CTRL_REGr(unit, xgxs_ctrl, port);
        }
    } else {
        /* Do reset through broadcast command. */
        ioerr += cdport_bcast_enable(unit, CDPORT_NUM_BCAST, last_bcast_port);
        CDPORT_XGXS0_CTRL_REGr_CLR(xgxs_ctrl);
        for (idx = 0; idx < CDPORT_NUM_BCAST; idx++) {
            if (last_bcast_port[idx] > 0) {
                port = CDPORT_BCAST_PORT(idx);
                /* Power up TSC by clearing TSC_PWRDWN. */
                ioerr += WRITE_CDPORT_XGXS0_CTRL_REGr(unit, xgxs_ctrl, port);
            }
        }

        sal_usleep(reset_sleep_us);

        CDPORT_XGXS0_CTRL_REGr_TSC_RSTBf_SET(xgxs_ctrl, 1);
        for (idx = 0; idx < CDPORT_NUM_BCAST; idx++) {
            if (last_bcast_port[idx] > 0) {
                port = CDPORT_BCAST_PORT(idx);
                /* Bring TSC out of reset */
                ioerr += WRITE_CDPORT_XGXS0_CTRL_REGr(unit, xgxs_ctrl, port);
            }
        }
    }

    /* Only 1 field, no need to READ_, use _CLR instead */
    CDPORT_MAC_CONTROLr_CLR(mac_ctrl);
    CDPORT_MAC_CONTROLr_CDMAC0_RESETf_SET(mac_ctrl, 1);
    BCMDRD_PBMP_ITER(pbmp, port) {
        ioerr += WRITE_CDPORT_MAC_CONTROLr(unit, mac_ctrl, port);
    }
    sal_usleep(reset_sleep_us);
    CDPORT_MAC_CONTROLr_CDMAC0_RESETf_SET(mac_ctrl, 0);
    BCMDRD_PBMP_ITER(pbmp, port) {
        ioerr += WRITE_CDPORT_MAC_CONTROLr(unit, mac_ctrl, port);
    }

    return ioerr;
}

static int
wait_for_pll_lock(int unit)
{
    int ioerr = 0;
    int msec;
    TOP_PP_PLL_STATUSr_t pp_pll_status;
    TOP_CORE_PLL_STATUSr_t core_pll_status;

    /* Check PP lock status */
    msec = 0;
    while (msec < PLL_LOCK_USEC) {
        ioerr += READ_TOP_PP_PLL_STATUSr(unit, &pp_pll_status);
        if (TOP_PP_PLL_STATUSr_LOCKf_GET(pp_pll_status) == 1) {
            break;
        }
        sal_usleep(25);
        msec += 25;
    }
    if (msec >= PLL_LOCK_USEC) {
        LOG_WARN(BSL_LS_BCMBD_DEV,
                 (BSL_META_U(unit,
                             "PP PLL did not lock, status = 0x%08"PRIx32"\n"),
                  TOP_PP_PLL_STATUSr_GET(pp_pll_status)));
    }

    /* Check core PLL lock status */
    msec = 0;
    while (msec < PLL_LOCK_USEC) {
        ioerr += READ_TOP_CORE_PLL_STATUSr(unit, &core_pll_status);
        if (TOP_CORE_PLL_STATUSr_LOCKf_GET(core_pll_status) == 1) {
            break;
        }
        sal_usleep(25);
        msec += 25;
    }
    if (msec >= PLL_LOCK_USEC) {
        LOG_WARN(BSL_LS_BCMBD_DEV,
                 (BSL_META_U(unit,
                             "Core PLL did not lock, status = 0x%08"PRIx32"\n"),
                  TOP_CORE_PLL_STATUSr_GET(core_pll_status)));
    }

    return ioerr;
}

/*!
 * Performs steps 1-3 of the CORE_PLL Programming sequence, and
 * is done when TD4 is operating at a different core clock frequency
 */
static int
core_pll_bypass_prog_pre(int unit)
{
    int ioerr = 0;
    TOP_CORE_PLL_CTRL_2r_t top_core_pll_ctrl_2;

    /* 1 */
    ioerr += READ_TOP_CORE_PLL_CTRL_2r(unit, &top_core_pll_ctrl_2);
    TOP_CORE_PLL_CTRL_2r_CH6_BYP_ENf_SET(top_core_pll_ctrl_2, 1);
    ioerr += WRITE_TOP_CORE_PLL_CTRL_2r(unit, top_core_pll_ctrl_2);

    /* 2: wait 1 us */
    sal_usleep(1);

    /* To 3: Programming of PLL config parameters follows this in calling
     * function
     */

    return ioerr;
}

/*!
 * Performs steps 5-9 of the CORE_PLL Programming sequence, and
 * is done when TD4 is operating at a different core clock frequency
 */
static int
core_pll_bypass_prog_post(int unit)
{
    int ioerr = 0;
    TOP_CORE_PLL_CTRL_2r_t top_core_pll_ctrl_2;

    /*
     * From 3: This follows the programming of the PLL config parameters from
     * the calling function
     */

    /* 4 */
    ioerr += READ_TOP_CORE_PLL_CTRL_2r(unit, &top_core_pll_ctrl_2);
    TOP_CORE_PLL_CTRL_2r_SW_CFG_SELf_SET(top_core_pll_ctrl_2, 1);
    ioerr += WRITE_TOP_CORE_PLL_CTRL_2r(unit, top_core_pll_ctrl_2);

    /* 5: wait 1 us */
    sal_usleep(1);

    /* 6 */
    ioerr += READ_TOP_CORE_PLL_CTRL_2r(unit, &top_core_pll_ctrl_2);
    TOP_CORE_PLL_CTRL_2r_CH6_BYP_ENf_SET(top_core_pll_ctrl_2, 0);
    ioerr += WRITE_TOP_CORE_PLL_CTRL_2r(unit, top_core_pll_ctrl_2);

    return ioerr;
}

/*!
 * Performs necessary steps to configure the chip for different
 * core frequencies.
 */
static int
core_pll_config(int unit, unsigned int frequency)
{
  int ioerr = 0;
    int ndiv_int = -1;
    TOP_CORE_PLL_CTRL_0r_t top_core_pll_ctrl_0;

    switch (frequency) {
    case CLK(1350MHZ):
        /* Don't do anything if core clock is default */
        break;
    case CLK(950MHZ):
        ndiv_int = 76;
        break;
    case CLK(1025MHZ):
        ndiv_int = 82;
        break;
    case CLK(1100MHZ):
        ndiv_int = 88;
        break;
    case CLK(1175MHZ):
        ndiv_int = 94;
        break;
    case CLK(1250MHZ):
        ndiv_int = 100;
        break;
    case CLK(850MHZ):
        ndiv_int = 68;
        break;
    default:
        LOG_WARN(BSL_LS_BCMBD_DEV,
                 (BSL_META_U(unit,
                             "Invalid frequency %d. Set with default value.\n"),
                  ndiv_int));
        break;
    }

    if (ndiv_int != -1) {
        LOG_INFO(BSL_LS_BCMBD_DEV,
                 (BSL_META_U(unit,
                             "*** change NDIV_INT to %d\n"),
                  ndiv_int));

        /* Call this to prepare for changing the core pll frequency */
        ioerr += core_pll_bypass_prog_pre(unit);

        ioerr += READ_TOP_CORE_PLL_CTRL_0r(unit, &top_core_pll_ctrl_0);
        TOP_CORE_PLL_CTRL_0r_NDIV_INTf_SET(top_core_pll_ctrl_0, ndiv_int);
        ioerr += WRITE_TOP_CORE_PLL_CTRL_0r(unit, top_core_pll_ctrl_0);
        /*
         * Call this after changing the core pll frequency to bring the
         * core pll bypass out of reset
         */
        ioerr += core_pll_bypass_prog_post(unit);
    }

    return ioerr;
}

/*!
 * Program the PP_PLL.
 */
static int
pp_pll_config(int unit, unsigned int frequency)
{

    int ioerr = 0;
    int ndiv_int = -1;
    TOP_PP_PLL_CTRL_0r_t top_pp_pll_0;
    TOP_PP_PLL_CTRL_2r_t top_pp_pll_2;

    switch (frequency) {
    case CLK(1350MHZ):
        /* Don't do anything if core clock is default */
        break;
    case CLK(950MHZ):
        ndiv_int = 76;
        break;
    case CLK(1025MHZ):
        ndiv_int = 82;
        break;
    case CLK(1100MHZ):
        ndiv_int = 88;
        break;
    case CLK(1175MHZ):
        ndiv_int = 94;
        break;
    case CLK(1250MHZ):
        ndiv_int = 100;
        break;
    case CLK(850MHZ):
        ndiv_int = 68;
        break;
    default:
        LOG_WARN(BSL_LS_BCMBD_DEV,
                 (BSL_META_U(unit,
                             "Invalid frequency %d. Set with default value.\n"),
                  ndiv_int));
        break;
    }

    if (ndiv_int != -1) {
        /* Hold all post div channels */
        ioerr += READ_TOP_PP_PLL_CTRL_2r(unit, &top_pp_pll_2);
        TOP_PP_PLL_CTRL_2r_POST_HOLD_ALLf_SET(top_pp_pll_2, 1);
        ioerr += WRITE_TOP_PP_PLL_CTRL_2r(unit, top_pp_pll_2);

        sal_usleep(10);

        /* Set ndiv_int and ndiv for target freq */
        ioerr += READ_TOP_PP_PLL_CTRL_0r(unit, &top_pp_pll_0);
        TOP_PP_PLL_CTRL_0r_NDIV_INTf_SET(top_pp_pll_0, ndiv_int);
        ioerr += WRITE_TOP_PP_PLL_CTRL_0r(unit, top_pp_pll_0);

        /* Wait for 25us */
        sal_usleep(25);

        /* Release all post div channels to normal operation */
        ioerr += READ_TOP_PP_PLL_CTRL_2r(unit, &top_pp_pll_2);
        TOP_PP_PLL_CTRL_2r_POST_HOLD_ALLf_SET(top_pp_pll_2, 0);
        ioerr += WRITE_TOP_PP_PLL_CTRL_2r(unit, top_pp_pll_2);
    }

    return ioerr;
}

/* Determine a PLL's current DCO value.  The PLL should be checked to ensure
   that it is locked prior to calling this function */
static int32_t
pll_dco_code_get(int32_t unit, td4_plls_e pll, uint32_t *dco_code)
{
    int ioerr = 0;
    TOP_TS_PLL_CTRL_4r_t ts_ctrl;
    TOP_BS_PLL_CTRL_4r_t bs0_ctrl;
    TOP_BS_PLL_CTRL_4r_t bs1_ctrl;
    TOP_TS_PLL_STATUSr_t ts_status;
    TOP_BS_PLL_STATUSr_t bs_status;

    if (pll == TD4_TS_PLL) {
        /* Configure the stat logic to read the DCO value */
        ioerr += READ_TOP_TS_PLL_CTRL_4r(unit, &ts_ctrl);
        TOP_TS_PLL_CTRL_4r_STAT_SELECTf_SET(ts_ctrl, 4); /* DCO */
        TOP_TS_PLL_CTRL_4r_STAT_MODEf_SET(ts_ctrl, 4);   /* avg */
        TOP_TS_PLL_CTRL_4r_STAT_RESETf_SET(ts_ctrl, 1);
        TOP_TS_PLL_CTRL_4r_STAT_UPDATEf_SET(ts_ctrl, 0);
        ioerr += WRITE_TOP_TS_PLL_CTRL_4r(unit, ts_ctrl);

        /* Take out of reset */
        ioerr += READ_TOP_TS_PLL_CTRL_4r(unit, &ts_ctrl);
        TOP_TS_PLL_CTRL_4r_STAT_RESETf_SET(ts_ctrl, 0);
        ioerr += WRITE_TOP_TS_PLL_CTRL_4r(unit, ts_ctrl);

        /* Initiate a capture */
        ioerr += READ_TOP_TS_PLL_CTRL_4r(unit, &ts_ctrl);
        TOP_TS_PLL_CTRL_4r_STAT_UPDATEf_SET(ts_ctrl, 1);
        ioerr += WRITE_TOP_TS_PLL_CTRL_4r(unit, ts_ctrl);

        sal_usleep(10 * 1000);

        /* Get the DCO code */
        ioerr += READ_TOP_TS_PLL_STATUSr(unit, &ts_status);
        *dco_code = TOP_TS_PLL_STATUSr_STATf_GET(ts_status);

        /* Clear the capture state */
        ioerr += READ_TOP_TS_PLL_CTRL_4r(unit, &ts_ctrl);
        TOP_TS_PLL_CTRL_4r_STAT_UPDATEf_SET(ts_ctrl, 0);
        ioerr += WRITE_TOP_TS_PLL_CTRL_4r(unit, ts_ctrl);
    } else if (pll == TD4_BS_PLL0) {
        /* Configure the stat logic to read the DCO value */
        ioerr += READ_TOP_BS_PLL_CTRL_4r(unit, 0, &bs0_ctrl);
        TOP_BS_PLL_CTRL_4r_STAT_SELECTf_SET(bs0_ctrl, 4); /* DCO */
        TOP_BS_PLL_CTRL_4r_STAT_MODEf_SET(bs0_ctrl, 4);   /* avg */
        TOP_BS_PLL_CTRL_4r_STAT_RESETf_SET(bs0_ctrl, 1);
        TOP_BS_PLL_CTRL_4r_STAT_UPDATEf_SET(bs0_ctrl, 0);
        ioerr += WRITE_TOP_BS_PLL_CTRL_4r(unit, 0, bs0_ctrl);

        /* Take out of reset */
        ioerr += READ_TOP_BS_PLL_CTRL_4r(unit, 0, &bs0_ctrl);
        TOP_BS_PLL_CTRL_4r_STAT_RESETf_SET(bs0_ctrl, 0);
        ioerr += WRITE_TOP_BS_PLL_CTRL_4r(unit, 0, bs0_ctrl);

        /* Initiate a capture */
        ioerr += READ_TOP_BS_PLL_CTRL_4r(unit, 0,  &bs0_ctrl);
        TOP_BS_PLL_CTRL_4r_STAT_UPDATEf_SET(bs0_ctrl, 1);
        ioerr += WRITE_TOP_BS_PLL_CTRL_4r(unit, 0, bs0_ctrl);

        sal_usleep(10 * 1000);

        /* Get the DCO code */
        ioerr += READ_TOP_BS_PLL_STATUSr(unit, 0, &bs_status);
        *dco_code = TOP_BS_PLL_STATUSr_STATf_GET(bs_status);

        /* Clear the capture state */
        ioerr += READ_TOP_BS_PLL_CTRL_4r(unit, 0, &bs0_ctrl);
        TOP_BS_PLL_CTRL_4r_STAT_UPDATEf_SET(bs0_ctrl, 0);
        ioerr += WRITE_TOP_BS_PLL_CTRL_4r(unit, 0,  bs0_ctrl);
    } else if (pll == TD4_BS_PLL1) {

        /* Configure the stat logic to read the DCO value */
        ioerr += READ_TOP_BS_PLL_CTRL_4r(unit, 1, &bs1_ctrl);
        TOP_BS_PLL_CTRL_4r_STAT_SELECTf_SET(bs1_ctrl, 4); /* DCO */
        TOP_BS_PLL_CTRL_4r_STAT_MODEf_SET(bs1_ctrl, 4);   /* avg */
        TOP_BS_PLL_CTRL_4r_STAT_RESETf_SET(bs1_ctrl, 1);
        TOP_BS_PLL_CTRL_4r_STAT_UPDATEf_SET(bs1_ctrl, 0);
        ioerr += WRITE_TOP_BS_PLL_CTRL_4r(unit, 1, bs1_ctrl);

        /* Take out of reset */
        ioerr += READ_TOP_BS_PLL_CTRL_4r(unit, 1, &bs1_ctrl);
        TOP_BS_PLL_CTRL_4r_STAT_RESETf_SET(bs1_ctrl, 0);
        ioerr += WRITE_TOP_BS_PLL_CTRL_4r(unit, 1, bs1_ctrl);

        /* Initiate a capture */
        ioerr += READ_TOP_BS_PLL_CTRL_4r(unit, 1, &bs1_ctrl);
        TOP_BS_PLL_CTRL_4r_STAT_UPDATEf_SET(bs1_ctrl, 1);
        ioerr += WRITE_TOP_BS_PLL_CTRL_4r(unit, 1, bs1_ctrl);

        sal_usleep(10 * 1000);

        /* Get the DCO code */
        ioerr += READ_TOP_BS_PLL_STATUSr(unit, 1, &bs_status);
        *dco_code = TOP_BS_PLL_STATUSr_STATf_GET(bs_status);

        /* Clear the capture state */
        ioerr += READ_TOP_BS_PLL_CTRL_4r(unit, 1, &bs1_ctrl);
        TOP_BS_PLL_CTRL_4r_STAT_UPDATEf_SET(bs1_ctrl, 0);
        ioerr += WRITE_TOP_BS_PLL_CTRL_4r(unit, 1, bs1_ctrl);
    } else {
        return SHR_E_CONFIG;
    }
    return ioerr;
}

/* Linearly scale the DCO code as a function of the VCO's frequency */
static int
pll_dco_normalize(uint32_t *dco_code, uint32_t ref_freq, uint32_t ndiv)
{
    static const int32_t int_ref_freq = 50000000;

    *dco_code <<= 20;
    if (ref_freq <= 0) {
        *dco_code /=  (int_ref_freq / 100000 * ndiv);
    } else {
        *dco_code /= (ref_freq / 100000 * ndiv);
    }

    return SHR_E_NONE;
}

typedef struct pll_7_param_s {
    unsigned int ref_freq;
    unsigned int ref_freq_val;
    unsigned int ref_freq_info;
    unsigned int ndiv;
} pll_7_param_t;

/*!
 * Program the TS_PLL.
 */
static int
ts_pll_config(int unit)
{
    int ioerr = 0;
    TOP_SOFT_RESET_REG_2r_t top_sreset_2;
    TOP_TS_PLL_CTRL_0r_t pll_ctrl_0;
    TOP_TS_PLL_CTRL_1r_t pll_ctrl_1;
    TOP_TS_PLL_STATUSr_t ts_pll_status;
    uint32_t ref_clk;
    int index;
    static const uint32_t auto_ref = TS_CLK(AUTO);
    static const uint32_t def_src = TS_CLK(INT_50MHZ);
    uint32_t dco_code, dco_code_target;
    static const uint32_t dco_poison = 0xffff;
    int rv;

    /* output 1000 Mhz */
    /* pdiv = 1, ch6_mdiv = 2; */
    static const pll_7_param_t ts_pll_params[] = {
        /* ref_feq,        val,      ref_freq_info, ndiv */
        {TS_CLK(EXT_50MHZ), 50000000,   50,         80 },  /* 50MHz external reference clock */
        {TS_CLK(INT_50MHZ),        0,   50,         80 },  /* 50MHz internal reference clock */
        {                0,        0,   50,         80 },  /* Auto mode logic */
    };

    ref_clk = bcmbd_config_get(unit, def_src,
                               "DEVICE_CONFIG",
                               "TS_PLL_CLK_SEL");

    index = (ref_clk == auto_ref) ? (COUNTOF(ts_pll_params) - 1) : 0;
    dco_code_target = dco_poison;

    for (; index < COUNTOF(ts_pll_params); index++) {
        if (ref_clk != auto_ref &&
                ts_pll_params[index].ref_freq != ref_clk) {
            continue;
        }

        /* Place in reset */
        ioerr += READ_TOP_SOFT_RESET_REG_2r(unit, &top_sreset_2);
        TOP_SOFT_RESET_REG_2r_TOP_TS_PLL_RST_Lf_SET(top_sreset_2, 0);
        ioerr += WRITE_TOP_SOFT_RESET_REG_2r(unit, top_sreset_2);

        /* Set N-divider value */
        ioerr += READ_TOP_TS_PLL_CTRL_0r(unit, &pll_ctrl_0);
        TOP_TS_PLL_CTRL_0r_NDIV_INTf_SET(pll_ctrl_0, ts_pll_params[index].ndiv);
        ioerr += WRITE_TOP_TS_PLL_CTRL_0r(unit, pll_ctrl_0);

        /* Select internal or external clock source */
        ioerr += READ_TOP_TS_PLL_CTRL_1r(unit, &pll_ctrl_1);
        TOP_TS_PLL_CTRL_1r_REFCLK_SOURCE_SELf_SET(pll_ctrl_1,
                            ((ts_pll_params[index].ref_freq == 0) ||
                            (ts_pll_params[index].ref_freq == TS_CLK(INT_50MHZ))) ? 0 : 1);
        ioerr += WRITE_TOP_TS_PLL_CTRL_1r(unit, pll_ctrl_1);

        /* Set new reference frequency info */
        ioerr += READ_TOP_TS_PLL_CTRL_1r(unit, &pll_ctrl_1);
        TOP_TS_PLL_CTRL_1r_FREFEFF_INFOf_SET(pll_ctrl_1,
                            ts_pll_params[index].ref_freq_info);

        /* Take out of reset */
        ioerr += READ_TOP_SOFT_RESET_REG_2r(unit, &top_sreset_2);
        TOP_SOFT_RESET_REG_2r_TOP_TS_PLL_RST_Lf_SET(top_sreset_2, 1);
        ioerr += WRITE_TOP_SOFT_RESET_REG_2r(unit, top_sreset_2);

        /* Give time to lock */
        /*
         * Per HW spec, 25 usec is sufficient to get PLL locked so wait 50
         * usec to be safe.
         */
        sal_usleep(50);

        /* Check PLL lock status */
        ioerr += READ_TOP_TS_PLL_STATUSr(unit, &ts_pll_status);
        if (TOP_TS_PLL_STATUSr_LOCKf_GET(ts_pll_status) == 1) {

            if (ref_clk == auto_ref && dco_code_target == dco_poison) {
                /* If auto freq, get DCO value for later comparison */
                rv = pll_dco_code_get(unit, TD4_TS_PLL,
                                      &dco_code_target);
                if (SHR_FAILURE(rv)) {
                    return rv;
                }

                pll_dco_normalize(&dco_code_target,
                                  ts_pll_params[index].ref_freq_val,
                                  ts_pll_params[index].ndiv);

                /* Start iterating over all of the available frequencies */
                index = -1;
            } else if (ref_clk == auto_ref) {
                /* Make sure the current DCO code is close to the target */

                rv = pll_dco_code_get(unit, TD4_TS_PLL,
                                      &dco_code);
                if (SHR_FAILURE(rv)) {
                    return rv;
                }
                pll_dco_normalize(&dco_code,
                                  ts_pll_params[index].ref_freq_val,
                                  ts_pll_params[index].ndiv);

                if (dco_code >= (dco_code_target - 400) &&
                        dco_code <= (dco_code_target + 400)) {
                    if (ts_pll_params[index].ref_freq_val) {
                        LOG_WARN(BSL_LS_BCMBD_DEV,
                            (BSL_META_U(unit,
                                "TS_PLL locked on unit %d using "
                                "%d.%dMHz external reference clock\n"
                                "Recommended to add \"%s=%u\" "
                                "config lt\n"),
                                unit,
                                ts_pll_params[index].ref_freq_val / 1000000,
                                ts_pll_params[index].ref_freq_val / 100000 % 10,
                                "DEVICE_CONFIG.TS_PLL_CLK_SEL",
                                ts_pll_params[index].ref_freq_val));
                    } else {
                        LOG_WARN(BSL_LS_BCMBD_DEV,
                            (BSL_META_U(unit, "TS_PLL locked on unit %d using "
                                "internal reference clock\n"), unit));
                    }
                    break;
                } else {
                    continue;
                }
            } else {
                /* PLL locked at desired frequency */
                break;
            }
        } else if (ref_clk != auto_ref) {
            /* Not able to lock at desired frequency */
            ioerr++;
            LOG_ERROR(BSL_LS_BCMBD_DEV,
                (BSL_META_U(unit,
                    "TS_PLL failed to lock on unit %d \n"),
                     unit));
            return ioerr;
        }
    }

    /* De-assert LCPLL's post reset */
    ioerr += READ_TOP_SOFT_RESET_REG_2r(unit, &top_sreset_2);
    TOP_SOFT_RESET_REG_2r_TOP_TS_PLL_POST_RST_Lf_SET(top_sreset_2, 1);
    ioerr += WRITE_TOP_SOFT_RESET_REG_2r(unit, top_sreset_2);
    return ioerr;
}

/*!
 * Program the BS_PLLs.
 */
static int
bs_pll_config(int unit)
{
    int ioerr = 0;
    TOP_SOFT_RESET_REG_2r_t top_sreset_2;
    TOP_BS_PLL_CTRL_0r_t pll_ctrl_0;
    TOP_BS_PLL_CTRL_1r_t pll_ctrl_1;
    TOP_BS_PLL_STATUSr_t pll_status;
    uint32_t ref_clk;
    int index;
    static const uint32_t auto_ref = BS_CLK(AUTO);
    static const uint32_t def_src = BS_CLK(INT_50MHZ);
    uint32_t dco_code, dco_code_target;
    static const uint32_t dco_poison = 0xffff;
    int rv;

    /* output 20 Mhz */
    /* pdiv = 1, ch6_mdiv = 100; */
    static const pll_7_param_t bs_pll_params[] = {
        /* ref_feq,        val,      ref_freq_info,  ndiv */
        {BS_CLK(EXT_12_8MHZ), 12800000,   13,         312 }, /* 25MHz external reference clock */
        {BS_CLK(EXT_20MHZ),   20000000,   20,         200 }, /* 50MHz external reference clock */
        {BS_CLK(EXT_25MHZ),   25000000,   25,         160 }, /* 50MHz internal reference clock */
        {BS_CLK(EXT_32MHZ),   32000000,   32,         125 }, /* 50MHz internal reference clock */
        {BS_CLK(EXT_50MHZ),   50000000,   50,         80  }, /* 50MHz internal reference clock */
        {BS_CLK(INT_50MHZ),          0,   50,         80  }, /* 50MHz internal reference clock */
        {                0,          0,   50,         80  }, /* Auto mode logic */
    };

    ref_clk = bcmbd_config_get(unit, def_src,
                                "DEVICE_CONFIG",
                                "BS_PLL_0_CLK_SEL");

    index = (ref_clk == auto_ref) ? (COUNTOF(bs_pll_params) - 1) : 0;
    dco_code_target = dco_poison;

    for (; index < COUNTOF(bs_pll_params); index++) {
        if (ref_clk != auto_ref &&
                bs_pll_params[index].ref_freq != ref_clk) {
            continue;
        }

        /* Place in reset */
        ioerr += READ_TOP_SOFT_RESET_REG_2r(unit, &top_sreset_2);
        TOP_SOFT_RESET_REG_2r_TOP_BS_PLL0_RST_Lf_SET(top_sreset_2, 0);
        ioerr += WRITE_TOP_SOFT_RESET_REG_2r(unit, top_sreset_2);

        /* Set N-divider value */
        ioerr += READ_TOP_BS_PLL_CTRL_0r(unit, 0, &pll_ctrl_0);
        TOP_BS_PLL_CTRL_0r_NDIV_INTf_SET(pll_ctrl_0, bs_pll_params[index].ndiv);
        ioerr += WRITE_TOP_BS_PLL_CTRL_0r(unit, 0, pll_ctrl_0);

        /* Select internal or external clock source */
        ioerr += READ_TOP_BS_PLL_CTRL_1r(unit, 0,  &pll_ctrl_1);
        TOP_BS_PLL_CTRL_1r_REFCLK_SOURCE_SELf_SET(pll_ctrl_1,
                            ((bs_pll_params[index].ref_freq == 0) ||
                            (bs_pll_params[index].ref_freq == BS_CLK(INT_50MHZ))) ? 0 : 1);

        /* Set new reference frequency info */
        TOP_BS_PLL_CTRL_1r_FREFEFF_INFOf_SET(pll_ctrl_1,
                            bs_pll_params[index].ref_freq_info);

        ioerr += WRITE_TOP_BS_PLL_CTRL_1r(unit, 0, pll_ctrl_1);

        /* Take out of reset */
        ioerr += READ_TOP_SOFT_RESET_REG_2r(unit, &top_sreset_2);
        TOP_SOFT_RESET_REG_2r_TOP_BS_PLL0_RST_Lf_SET(top_sreset_2, 1);
        ioerr += WRITE_TOP_SOFT_RESET_REG_2r(unit, top_sreset_2);

        /* Give time to lock */
        /*
         * Per HW spec, 25 usec is sufficient to get PLL locked so wait 50
         * usec to be safe.
         */
        sal_usleep(50);

        /* Check PLL lock status */
        ioerr += READ_TOP_BS_PLL_STATUSr(unit, 0, &pll_status);
        if (TOP_BS_PLL_STATUSr_LOCKf_GET(pll_status) == 1) {

            if (ref_clk == auto_ref && dco_code_target == dco_poison) {
                /* If auto freq, get DCO value for later comparison */
                rv = pll_dco_code_get(unit, TD4_BS_PLL0,
                                      &dco_code_target);
                if (SHR_FAILURE(rv)) {
                    return rv;
                }

                pll_dco_normalize(&dco_code_target,
                                  bs_pll_params[index].ref_freq_val,
                                  bs_pll_params[index].ndiv);

                /* Start iterating over all of the available frequencies */
                index = -1;
            } else if (ref_clk == auto_ref) {
                /* Make sure the current DCO code is close to the target */

                rv = pll_dco_code_get(unit, TD4_BS_PLL0,
                                      &dco_code);
                if (SHR_FAILURE(rv)) {
                    return rv;
                }
                pll_dco_normalize(&dco_code,
                                  bs_pll_params[index].ref_freq_val,
                                  bs_pll_params[index].ndiv);

                if (dco_code >= (dco_code_target - 400) &&
                        dco_code <= (dco_code_target + 400)) {
                    if (bs_pll_params[index].ref_freq_val) {
                        LOG_WARN(BSL_LS_BCMBD_DEV,
                            (BSL_META_U(unit,
                                "BS_PLL0 locked on unit %d using "
                                "%d.%dMHz external reference clock\n"
                                "Recommended to add \"%s=%u\" "
                                "config lt\n"),
                                unit,
                                bs_pll_params[index].ref_freq_val / 1000000,
                                bs_pll_params[index].ref_freq_val / 100000 % 10,
                                "DEVICE_CONFIG.BS_PLL_0_CLK_SEL",
                                bs_pll_params[index].ref_freq_val));
                    } else {
                        LOG_WARN(BSL_LS_BCMBD_DEV,
                            (BSL_META_U(unit, "BS_PLL 0 locked on unit %d using "
                                "internal reference clock\n"), unit));
                    }
                    break;
                } else {
                    continue;
                }
            } else {
                /* PLL locked at desired frequency */
                break;
            }
        } else if (ref_clk != auto_ref) {
            /* Not able to lock at desired frequency */
            ioerr++;
            LOG_ERROR(BSL_LS_BCMBD_DEV,
                (BSL_META_U(unit,
                    "BS_PLL0 failed to lock on unit %d \n"),
                     unit));
            return ioerr;
        }
    }

    ref_clk = bcmbd_config_get(unit, def_src,
                                "DEVICE_CONFIG",
                                "BS_PLL_1_CLK_SEL");

    index = (ref_clk == auto_ref) ? (COUNTOF(bs_pll_params) - 1) : 0;
    dco_code_target = dco_poison;

    for (; index < COUNTOF(bs_pll_params); index++) {
        if (ref_clk != auto_ref &&
                bs_pll_params[index].ref_freq != ref_clk) {
            continue;
        }

        /* Place in reset */
        ioerr += READ_TOP_SOFT_RESET_REG_2r(unit, &top_sreset_2);
        TOP_SOFT_RESET_REG_2r_TOP_BS_PLL1_RST_Lf_SET(top_sreset_2, 0);
        ioerr += WRITE_TOP_SOFT_RESET_REG_2r(unit, top_sreset_2);

        /* Set N-divider value */
        ioerr += READ_TOP_BS_PLL_CTRL_0r(unit, 1, &pll_ctrl_0);
        TOP_BS_PLL_CTRL_0r_NDIV_INTf_SET(pll_ctrl_0, bs_pll_params[index].ndiv);
        ioerr += WRITE_TOP_BS_PLL_CTRL_0r(unit, 1, pll_ctrl_0);

        /* Select internal or external clock source */
        ioerr += READ_TOP_BS_PLL_CTRL_1r(unit, 1,  &pll_ctrl_1);
        TOP_BS_PLL_CTRL_1r_REFCLK_SOURCE_SELf_SET(pll_ctrl_1,
                            ((bs_pll_params[index].ref_freq == 0) ||
                            (bs_pll_params[index].ref_freq == BS_CLK(INT_50MHZ))) ? 0 : 1);
        TOP_BS_PLL_CTRL_1r_FREFEFF_INFOf_SET(pll_ctrl_1,
                            bs_pll_params[index].ref_freq_info);

        ioerr += WRITE_TOP_BS_PLL_CTRL_1r(unit, 1, pll_ctrl_1);
        /* Take out of reset */
        ioerr += READ_TOP_SOFT_RESET_REG_2r(unit, &top_sreset_2);
        TOP_SOFT_RESET_REG_2r_TOP_BS_PLL1_RST_Lf_SET(top_sreset_2, 1);
        ioerr += WRITE_TOP_SOFT_RESET_REG_2r(unit, top_sreset_2);

        /* Give time to lock */
        /*
         * Per HW spec, 25 usec is sufficient to get PLL locked so wait 50
         * usec to be safe.
         */
        sal_usleep(50);

        /* Check PLL lock status */
        ioerr += READ_TOP_BS_PLL_STATUSr(unit, 1, &pll_status);
        if (TOP_BS_PLL_STATUSr_LOCKf_GET(pll_status) == 1) {

            if (ref_clk == auto_ref && dco_code_target == dco_poison) {
                /* If auto freq, get DCO value for later comparison */
                rv = pll_dco_code_get(unit, TD4_BS_PLL1,
                                      &dco_code_target);
                if (SHR_FAILURE(rv)) {
                    return rv;
                }

                pll_dco_normalize(&dco_code_target,
                                  bs_pll_params[index].ref_freq_val,
                                  bs_pll_params[index].ndiv);

                /* Start iterating over all of the available frequencies */
                index = -1;
            } else if (ref_clk == auto_ref) {
                /* Make sure the current DCO code is close to the target */

                rv = pll_dco_code_get(unit, TD4_BS_PLL1,
                                      &dco_code);
                if (SHR_FAILURE(rv)) {
                    return rv;
                }
                pll_dco_normalize(&dco_code,
                                  bs_pll_params[index].ref_freq_val,
                                  bs_pll_params[index].ndiv);

                if (dco_code >= (dco_code_target - 400) &&
                        dco_code <= (dco_code_target + 400)) {
                    if (bs_pll_params[index].ref_freq_val) {
                        LOG_WARN(BSL_LS_BCMBD_DEV,
                            (BSL_META_U(unit,
                                "BS_PLL0 locked on unit %d using "
                                "%d.%dMHz external reference clock\n"
                                "Recommended to add \"%s=%u\" "
                                "config lt\n"),
                                unit,
                                bs_pll_params[index].ref_freq_val / 1000000,
                                bs_pll_params[index].ref_freq_val / 100000 % 10,
                                "DEVICE_CONFIG.BS_PLL_1_CLK_SEL",
                                bs_pll_params[index].ref_freq_val));
                    } else {
                        LOG_WARN(BSL_LS_BCMBD_DEV,
                            (BSL_META_U(unit, "BS_PLL 1 locked on unit %d using "
                                "internal reference clock\n"), unit));
                    }
                    break;
                } else {
                    continue;
                }
            } else {
                /* PLL locked at desired frequency */
                break;
            }
        } else if (ref_clk != auto_ref) {
            /* Not able to lock at desired frequency */
            ioerr++;
            LOG_ERROR(BSL_LS_BCMBD_DEV,
                (BSL_META_U(unit,
                    "BS_PLL1 failed to lock on unit %d \n"),
                     unit));
            return ioerr;
        }
    }

    /* De-assert LCPLL's post reset */
    ioerr += READ_TOP_SOFT_RESET_REG_2r(unit, &top_sreset_2);
    TOP_SOFT_RESET_REG_2r_TOP_BS_PLL0_POST_RST_Lf_SET(top_sreset_2, 1);
    TOP_SOFT_RESET_REG_2r_TOP_BS_PLL1_POST_RST_Lf_SET(top_sreset_2, 1);
    ioerr += WRITE_TOP_SOFT_RESET_REG_2r(unit, top_sreset_2);
    return ioerr;
}

static int
pll_init(int unit)
{
    int ioerr = 0;
    TOP_SOFT_RESET_REG_2r_t top_sreset_2;
    TOP_PP_PLL_CTRL_0r_t pll_ctrl_0;
    uint32_t clk_freq;
    uint32_t default_core_clk_freq = CLK(1350MHZ);

    clk_freq = bcmbd_config_get(unit, default_core_clk_freq,
                                "DEVICE_CONFIG",
                                "CORE_CLK_FREQ");
    ioerr += core_pll_config(unit, clk_freq);

    /* If not configured, use core frequency. */
    clk_freq = bcmbd_config_get(unit, clk_freq,
                                "DEVICE_CONFIG",
                                "PP_CLK_FREQ");
    /*
     * If default core clock frequency is being used, then adjust PP
     * clock frequency according to device capabilities.
     */
    if (clk_freq == default_core_clk_freq &&
        (bcmdrd_dev_flags_get(unit) & CHIP_FLAG_BW8T)) {
        clk_freq = CLK(950MHZ);
    }
    ioerr += pp_pll_config(unit, clk_freq);

    /* Set NDIV_P to 0x0 */
    ioerr += READ_TOP_PP_PLL_CTRL_0r(unit, &pll_ctrl_0);
    TOP_PP_PLL_CTRL_0r_NDIV_Pf_SET(pll_ctrl_0, 0);
    ioerr += WRITE_TOP_PP_PLL_CTRL_0r(unit, pll_ctrl_0);

    /*
     * Bring LCPLL, iProc, PP, core PLLs and AVS
     * out of reset.
     */
    ioerr += READ_TOP_SOFT_RESET_REG_2r(unit, &top_sreset_2);
    TOP_SOFT_RESET_REG_2r_TOP_AVS_RST_Lf_SET(top_sreset_2, 1);
    TOP_SOFT_RESET_REG_2r_TOP_IPROC_PLL_RST_Lf_SET(top_sreset_2, 1);
    TOP_SOFT_RESET_REG_2r_TOP_PP_PLL_RST_Lf_SET(top_sreset_2, 1);
    TOP_SOFT_RESET_REG_2r_TOP_CORE_PLL_RST_Lf_SET(top_sreset_2, 1);

    ioerr += WRITE_TOP_SOFT_RESET_REG_2r(unit, top_sreset_2);

    /*
     * Per HW spec, 25 usec is sufficient to get PLL locked so wait 50
     * usec to be safe.
     */
    sal_usleep(50);

    /* Initialize LCPLLs */
    ioerr += wait_for_pll_lock(unit);

    /* De-assert LCPLL's post reset */
    ioerr += READ_TOP_SOFT_RESET_REG_2r(unit, &top_sreset_2);
    TOP_SOFT_RESET_REG_2r_TOP_IPROC_PLL_POST_RST_Lf_SET(top_sreset_2, 1);
    TOP_SOFT_RESET_REG_2r_TOP_PP_PLL_POST_RST_Lf_SET(top_sreset_2, 1);
    TOP_SOFT_RESET_REG_2r_TOP_CORE_PLL_POST_RST_Lf_SET(top_sreset_2, 1);
    ioerr += WRITE_TOP_SOFT_RESET_REG_2r(unit, top_sreset_2);

    /* Initialize, configure, check lock status of TS, BS PLLs */
    ioerr += ts_pll_config(unit);
    ioerr += bs_pll_config(unit);
    return ioerr;
}

static int
bypass_mode_init(int unit)
{
    int ioerr = 0;
    TOP_MISC_CONTROLr_t misc_ctrl;

    if (bcmdrd_dev_bypass_mode_get(unit) == BYPASS_MODE_EPIP) {
        ioerr += READ_TOP_MISC_CONTROLr(unit, &misc_ctrl);
        TOP_MISC_CONTROLr_PP_BYPASS_MODEf_SET(misc_ctrl, 1);
        ioerr += WRITE_TOP_MISC_CONTROLr(unit, misc_ctrl);
    }

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

/*
 * 0th entry of MMU_EBMB_CCBE_SLICE_CPU is a special entry
 * and read along with servicing null slot from MMU. This
 * should be initialized before any null slot is served from MMU.
 *
 * EP starts requesting null slot from MMU as soon as it is
 * brought out of reset AND when there is an SBUS request to EP
 *
 * MMU_EBMB_CCBE_SLICE_CPU entry 0 should be written as 0 before
 * any SBUS request is made to EP
 */
static int
mmu_config_after_ep_reset(int unit)
{
    int i, ioerr = 0;
    uint32_t pipe_map;
    MMU_EBMB_CCBE_SLICE_CPUm_t mecsc;

    ioerr = bcmdrd_dev_valid_pipes_get(unit, &pipe_map);
    if (SHR_FAILURE(ioerr)) {
        return ioerr;
    }
    MMU_EBMB_CCBE_SLICE_CPUm_CLR(mecsc);
    for (i = 0; i < TD4_PIPES_PER_DEV; i++) {
        if (pipe_map & (1 << i)) {
            /* Clear index 0 of MMU_EBMB_CCBE_SLICE_CPUm */
            ioerr += WRITE_MMU_EBMB_CCBE_SLICE_CPUm(unit, i, 0, mecsc);
        }
    }

    return ioerr;
}

/*******************************************************************************
* Public functions
 */

int
bcm56880_a0_bd_dev_reset(int unit)
{
    int ioerr = 0;
    int rv;
    int wait_usec;
    int port, idx;
    bcmdrd_pbmp_t pbmp;
    CMIC_TOP_SBUS_RING_MAPr_t sbus_ring_map;
    CMIC_TOP_SBUS_TIMEOUTr_t sbus_to;
    DMU_CRU_RESETr_t cru_reset;
    TOP_SOFT_RESET_REGr_t top_sreset;
    TOP_SOFT_RESET_REG_3r_t top_sreset_3;
    TOP_CLOCKING_ENFORCE_REGr_t top_clk;
    AVS_HW_MNTR_SEQUENCER_MASK_PVT_MNTRr_t avs_mntr;
    RTS0_MHOST_0_CR5_RST_CTRLr_t ctl0;
    RTS0_MHOST_1_CR5_RST_CTRLr_t ctl1;
    RTS1_MHOST_0_CR5_RST_CTRLr_t ctl2;
    RTS1_MHOST_1_CR5_RST_CTRLr_t ctl3;
    bool emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL) ;
    bool sim = bcmdrd_feature_is_sim(unit);
    uint32_t ring_map[] = { 0x00333210, 0x00000000, 0x44444444, 0x44444444,
                            0x55555555, 0x55555555, 0x00000000, 0x00005004,
                            0x00000000, 0x00000000, 0x00000000, 0x00000076,
                            0x00000000, 0x00000000, 0x00000000, 0x00000000 };

    wait_usec = (emul ? EMUL_WAIT_MSEC : RESET_WAIT_MSEC) * 1000;

    /* Initialize endian mode for correct reset access */
    ioerr += bcmbd_cmicx_init(unit);

    /*
     * Both PM_RST_L and TSC_ENABLE would put BH serdes in iddq mode in TD4.
     * Must separate PM_RST_L by quad (8 PM per Quad) for reset assertion
     * and de-assertion. TSC_ENABLE cannot be toggled for 7nm chip,
     * so need to leave TSC_ENABLE default value.
     */
    TOP_SOFT_RESET_REG_3r_SET(top_sreset_3, 0xffffff00);
    ioerr += WRITE_TOP_SOFT_RESET_REG_3r(unit, top_sreset_3);
    sal_usleep(1000);
    TOP_SOFT_RESET_REG_3r_SET(top_sreset_3, 0xffff0000);
    ioerr += WRITE_TOP_SOFT_RESET_REG_3r(unit, top_sreset_3);
    sal_usleep(1000);
    TOP_SOFT_RESET_REG_3r_SET(top_sreset_3, 0xff000000);
    ioerr += WRITE_TOP_SOFT_RESET_REG_3r(unit, top_sreset_3);
    sal_usleep(1000);
    TOP_SOFT_RESET_REG_3r_SET(top_sreset_3, 0x000000);
    ioerr += WRITE_TOP_SOFT_RESET_REG_3r(unit, top_sreset_3);
    sal_usleep(1000);

    /* Reset R5 cores */
    if (bcmdrd_feature_is_real_hw(unit)) {
        RTS0_MHOST_0_CR5_RST_CTRLr_CLR(ctl0);
        ioerr += WRITE_RTS0_MHOST_0_CR5_RST_CTRLr(unit, ctl0);
        RTS0_MHOST_1_CR5_RST_CTRLr_CLR(ctl1);
        ioerr += WRITE_RTS0_MHOST_1_CR5_RST_CTRLr(unit, ctl1);
        RTS1_MHOST_0_CR5_RST_CTRLr_CLR(ctl2);
        ioerr += WRITE_RTS1_MHOST_0_CR5_RST_CTRLr(unit, ctl2);
        RTS1_MHOST_1_CR5_RST_CTRLr_CLR(ctl3);
        ioerr += WRITE_RTS1_MHOST_1_CR5_RST_CTRLr(unit, ctl3);
    }

    /* Pull reset line */
    if (!sim) {
        DMU_CRU_RESETr_SET(cru_reset, 0x2);
        ioerr += WRITE_DMU_CRU_RESETr(unit, cru_reset);
        /* Wait for all tables to initialize */
        sal_usleep(wait_usec);
    }

    /* Re-initialize endian mode after reset */
    ioerr += bcmbd_cmicx_init(unit);

    /*
     * SBUS ring and block number for bcm56880:
     *              block_num   ring
     *  -------------------------------
     *   IP         1           1
     *   EP         2           2
     *   MMU        3,4,5       3       #MMU_ITM, MMU_EB, MMU_GLB
     *   OTPC       6           0
     *   TOP        8           0
     *   PM_MGMT    12          0
     *   PM0-15     16-31       4
     *   PM16-31    32-47       5
     *   AVS        87          0
     *   CEV        88          6
     *   PORT_IF    89          7
     */
    for (idx = 0; idx < COUNTOF(ring_map); idx++) {
        CMIC_TOP_SBUS_RING_MAPr_SET(sbus_ring_map, ring_map[idx]);
        ioerr += WRITE_CMIC_TOP_SBUS_RING_MAPr(unit, idx, sbus_ring_map);
    }
    CMIC_TOP_SBUS_TIMEOUTr_SET(sbus_to, 0x2700);
    ioerr += WRITE_CMIC_TOP_SBUS_TIMEOUTr(unit, sbus_to);

    sal_usleep(wait_usec);

    /*
     * Workaround TD4-6331
     */
    AVS_HW_MNTR_SEQUENCER_MASK_PVT_MNTRr_CLR(avs_mntr);
    AVS_HW_MNTR_SEQUENCER_MASK_PVT_MNTRr_SEQ_MASK_PVT_MNTRf_SET(avs_mntr,
                                                                0xffffe);
    WRITE_AVS_HW_MNTR_SEQUENCER_MASK_PVT_MNTRr(unit, avs_mntr);

    /* Reset all blocks */
    TOP_SOFT_RESET_REGr_CLR(top_sreset);
    ioerr += WRITE_TOP_SOFT_RESET_REGr(unit, top_sreset);
    sal_usleep(wait_usec);

    /*
     * Bring TimeSync out of reset before PLL programming to ensure
     * divided TimeSync clocks inside iProc are phase aligned.
     */
    TOP_SOFT_RESET_REGr_CLR(top_sreset);
    TOP_SOFT_RESET_REGr_TOP_TS_RST_Lf_SET(top_sreset, 1);
    ioerr += WRITE_TOP_SOFT_RESET_REGr(unit, top_sreset);
    sal_usleep(wait_usec);

    /* PLLs are only required on real hardware */
    if (bcmdrd_feature_is_real_hw(unit)) {
        ioerr += pll_init(unit);
    }

    if (sim || emul) {
        /* Bring port blocks out of reset */
        ioerr += READ_TOP_SOFT_RESET_REGr(unit, &top_sreset);
        TOP_SOFT_RESET_REGr_TOP_PM_MGMT_RST_Lf_SET(top_sreset, 1);
        ioerr += WRITE_TOP_SOFT_RESET_REGr(unit, top_sreset);
    }

    TOP_SOFT_RESET_REG_3r_SET(top_sreset_3, 0x000000ff);
    ioerr += WRITE_TOP_SOFT_RESET_REG_3r(unit, top_sreset_3);
    sal_usleep(1000);
    TOP_SOFT_RESET_REG_3r_SET(top_sreset_3, 0x0000ffff);
    ioerr += WRITE_TOP_SOFT_RESET_REG_3r(unit, top_sreset_3);
    sal_usleep(1000);
    TOP_SOFT_RESET_REG_3r_SET(top_sreset_3, 0x00ffffff);
    ioerr += WRITE_TOP_SOFT_RESET_REG_3r(unit, top_sreset_3);
    sal_usleep(1000);
    TOP_SOFT_RESET_REG_3r_SET(top_sreset_3, 0xffffffff);
    ioerr += WRITE_TOP_SOFT_RESET_REG_3r(unit, top_sreset_3);
    sal_usleep(1000);

    /* Bring IP, EP, PORT_IF and MMU blocks out of reset */
    ioerr += READ_TOP_SOFT_RESET_REGr(unit, &top_sreset);
    TOP_SOFT_RESET_REGr_TOP_IP_RST_Lf_SET(top_sreset, 1);
    TOP_SOFT_RESET_REGr_TOP_EP_RST_Lf_SET(top_sreset, 1);
    TOP_SOFT_RESET_REGr_TOP_MMU_RST_Lf_SET(top_sreset, 1);
    ioerr += WRITE_TOP_SOFT_RESET_REGr(unit, top_sreset);
    sal_usleep(wait_usec);

    ioerr += mmu_config_after_ep_reset(unit);

    /* WAR for SOBMH packet drop */
    ioerr += READ_TOP_CLOCKING_ENFORCE_REGr(unit, &top_clk);
    TOP_CLOCKING_ENFORCE_REGr_EP0f_SET(top_clk, 1);
    TOP_CLOCKING_ENFORCE_REGr_EP1f_SET(top_clk, 1);
    TOP_CLOCKING_ENFORCE_REGr_EP2f_SET(top_clk, 1);
    TOP_CLOCKING_ENFORCE_REGr_EP3f_SET(top_clk, 1);
    ioerr += WRITE_TOP_CLOCKING_ENFORCE_REGr(unit, top_clk);

    rv = bypass_mode_init(unit);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Reset port blocks */
    xlport_pbmp_get(unit, &pbmp);
    BCMDRD_PBMP_ITER(pbmp, port) {
        ioerr += xlport_reset(unit, port, emul);
        /* Only one single XLPORT block */
        break;
    }

    cdport_pbmp_get(unit, &pbmp);
    BCMDRD_PBMP_ITER(pbmp, port) {
        if (!block_port_get(unit, port, BLKTYPE_CDPORT) == 0) {
            BCMDRD_PBMP_PORT_REMOVE(pbmp, port);
        }
    }

    ioerr += cdport_reset(unit, pbmp, emul, sim);

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}
