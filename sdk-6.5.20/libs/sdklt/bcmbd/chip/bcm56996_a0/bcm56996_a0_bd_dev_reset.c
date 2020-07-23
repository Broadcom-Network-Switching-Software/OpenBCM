/*! \file bcm56996_a0_bd_dev_reset.c
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
#include <bcmbd/chip/bcm56996_a0_cmic_bcast.h>
#include <bcmbd/chip/bcm56996_a0_acc.h>
#include <bcmdrd/chip/bcm56996_a0_defs.h>
#include <bcmbd/chip/bcm56996_a0_cmic_acc.h>
#include <bcmdrd/chip/bcm56996_a0_cmic_defs.h>
#include <bcmbd/chip/bcm56996_a0_dev.h>
#include <bcmlrd/chip/bcm56996_a0/bcm56996_a0_lrd_enum_ctype.h>

#include "bcm56996_a0_drv.h"

/* Log source for this component */
#define BSL_LOG_MODULE  BSL_LS_BCMBD_DEV

/*******************************************************************************
* Local definitions
 */
/* 50ms.  Unit is 40ns (clock). */
#define SEQRST_SW_TIME          1250000
/* 60ms.
 * Should be no less than SEQRST_SW_TIME + hardware time space * steps.
 * The hardware time space is 100 us and 21 steps.
 */
#define SEQRST_TIME             1500000
/* Emulate software sequential reset disabled by setting 1.*/
#define SEQRST_SW_TIME_HW       1
/* 5ms.
 * Should be no less than SEQRST_SW_TIME + hardware time space * steps.
 * The hardware time space is 100 us and 21 steps.
 */
#define SEQRST_TIME_HW          125000
/* Sequential reset mode. */
#define SEQ_RESET_T(_t)    (BCM56996_A0_LRD_DEVICE_SEQ_RESET_T_T_SEQ_RESET_##_t)

/* PLL post divider ratio. Refer to CH6_MDIV in regsfile. */
#define PLL_CH6_MDIV            2
/* PLL fix divider ratio. Refer to CH6_MDIV in regsfile. */
#define PLL_FIX_DIV             2
/* PLL input reference clock pre-divider ratio. */
#define PLL_PDIV                1
/* Reference clock. */
#define PLL_BASE                50
/* Calculation:
 * VCO = PLL_BASE * NDIV
 * frequency = VCO / (PLL_CH6_MDIV * PLL_FIX_DIV * PLL_PDIV)
 * NDIV = (frequency * PLL_CH6_MDIV * PLL_FIX_DIV * PLL_PDIV) / PLL_BASE
 */
#define PLL_NDIV(_f)     (_f * PLL_CH6_MDIV * PLL_FIX_DIV * PLL_PDIV / PLL_BASE)

/* PLL configuration delays. */
#define PLL_BYPASS_USEC         1
#define PLL_PP_HOLD_USEC        10
#define PLL_PP_CONFIG_USEC      25
#define PLL_LOCK_USEC           25
#define PLL_LOCK_MAX_USEC       1000
#define PLL_TS_BS_LOCK_USEC     50

/* Reset delays. */
#define TSC_PWRUP_USEC          5000
#define PM_RESET_USEC           50
#define MAC_RESET_USEC          10
#define WAIT_MSEC               5
#define EMUL_WAIT_MSEC          250
#define CLOCK_GATING_WAIT_USEC  1

/* Clock frequency enum is used for clk_freq_enum table index. */
#define CLK(mhz)                (BCM56996_A0_LRD_DEVICE_CLK_FREQ_T_T_CLK_##mhz)
/* Default frequency. */
#define PLL_FREQ_DEF            1325
/* Frequency identifier. */
#define PLL_FREQ_DEF_ID         CLK(1325MHZ)

/* FGFS frequencies list. */
static struct {
    uint32_t id;
    uint32_t freq;
} clk_freqs[] = {
    {CLK(1325MHZ), 1325},
    {CLK(1250MHZ), 1250},
    {CLK(1175MHZ), 1175},
    {CLK(1100MHZ), 1100},
    {CLK(1025MHZ), 1025},
    {CLK(950MHZ), 950}
};

/* TS/BS PLL frequence info */
typedef struct pll_7_param_s {
    unsigned int ref_freq;
    unsigned int ref_freq_val;
    unsigned int ref_freq_info;
    unsigned int ndiv;
} pll_7_param_t;

/* Clock frequency enum is used for ts_pll_params table index. */
#define TS_CLK(mhz)      (BCM56996_A0_LRD_DEVICE_TS_PLL_REF_CLK_T_T_CLK_##mhz)

/*
 * TimeSync PLL reference clock info
 * output 1000 Mhz, pdiv = 1, ch6_mdiv = 2.
 */
static const pll_7_param_t ts_pll_params[] = {
    /* ref_feq,        val,      ref_freq_info, ndiv */
    {TS_CLK(EXT_50MHZ), 50000000,   50,         80 },  /* 50MHz external */
    {TS_CLK(INT_50MHZ),        0,   50,         80 },  /* 50MHz internal */
    {                0,        0,   50,         80 },  /* Auto mode logic */
};

/* Clock frequency enum is used for bs_pll_params table index. */
#define BS_CLK(mhz)      (BCM56996_A0_LRD_DEVICE_BS_PLL_REF_CLK_T_T_CLK_##mhz)

/*
 * BroadSync PLL reference clock info
 * output 20 Mhz, pdiv = 1, ch6_mdiv = 100.
 */
static const pll_7_param_t bs_pll_params[] = {
    /* ref_feq,        val,      ref_freq_info,  ndiv */
    {BS_CLK(EXT_12_8MHZ), 12800000,   13,         312 }, /* 25MHz external */
    {BS_CLK(EXT_20MHZ),   20000000,   20,         200 }, /* 50MHz external */
    {BS_CLK(EXT_25MHZ),   25000000,   25,         160 }, /* 50MHz internal */
    {BS_CLK(EXT_32MHZ),   32000000,   32,         125 }, /* 50MHz internal */
    {BS_CLK(EXT_50MHZ),   50000000,   50,         80  }, /* 50MHz internal */
    {BS_CLK(INT_50MHZ),          0,   50,         80  }, /* 50MHz internal */
    {                0,          0,   50,         80  }, /* Auto mode logic */
};

typedef enum {
    TIME_SYNC_PLL,
    BROAD_SYNC_PLL0,
    BROAD_SYNC_PLL1,
    INVALID_PLL
} pll_type_e;

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
    XLPORT_XGXS0_CTRL_REGr_t xgxs_ctrl;

    /* Configure clock source */
    ioerr += READ_XLPORT_XGXS0_CTRL_REGr(unit, &xgxs_ctrl, port);
    XLPORT_XGXS0_CTRL_REGr_REFIN_ENf_SET(xgxs_ctrl, emul ? 0 : 1);
    ioerr += WRITE_XLPORT_XGXS0_CTRL_REGr(unit, xgxs_ctrl, port);

    /* Deassert power down */
    ioerr += READ_XLPORT_XGXS0_CTRL_REGr(unit, &xgxs_ctrl, port);
    XLPORT_XGXS0_CTRL_REGr_PWRDWNf_SET(xgxs_ctrl, 0);
    XLPORT_XGXS0_CTRL_REGr_IDDQf_SET(xgxs_ctrl, 0);
    ioerr += WRITE_XLPORT_XGXS0_CTRL_REGr(unit, xgxs_ctrl, port);
    sal_usleep(TSC_PWRUP_USEC);

    /* Bring XGXS out of reset */
    XLPORT_XGXS0_CTRL_REGr_RSTB_HWf_SET(xgxs_ctrl, 1);
    ioerr += WRITE_XLPORT_XGXS0_CTRL_REGr(unit, xgxs_ctrl, port);

    return ioerr;
}

static int
cdport_bcast_enable(int unit,
                    int num_bcast,
                    int *last_bcast_port_u0,
                    int *last_bcast_port_u1)
{
    int ioerr = 0;
    int idx;
    int port;
    bcmdrd_pbmp_t pbmp;
    CDPORT_SBUS_CONTROLr_t sbus_ctrl;
    CDPORT_TSC_CLOCK_CONTROLr_t tsc_clock_control;

    /* on TH4G, CDPORT_TSC_CLOCK_CONTROL is added
     * to gate off tsc clock and the chip default is enable
     * so the first thing to do is to relase tsc clock gating
     */
    CDPORT_TSC_CLOCK_CONTROLr_SET(tsc_clock_control, 0);

    cdport_pbmp_get(unit, &pbmp);
    BCMDRD_PBMP_ITER(pbmp, port) {
        if (block_port_get(unit, port, BLKTYPE_CDPORT) == 0) {
            /* this CDPORT_TSC_CLOCK_CONTROL is one copy per PML */
            if ((port - 1) % 4 == 0) {
                ioerr += WRITE_CDPORT_TSC_CLOCK_CONTROLr(unit,
                                                         tsc_clock_control,
                                                         port);
            }
        }
    }

    for (idx = 0; idx < num_bcast; idx++) {
        last_bcast_port_u0[idx] = -1;
        last_bcast_port_u1[idx] = -1;
    }
    cdport_pbmp_get(unit, &pbmp);
    BCMDRD_PBMP_ITER(pbmp, port) {
        if (block_port_get(unit, port, BLKTYPE_CDPORT) == 0) {
            int bcast_domain = CDPORT_BCAST_DOMAIN(port);
            int blk_u0 = CDPORT_BCAST_BLK_U0(bcast_domain);
            int blk_u1 = CDPORT_BCAST_BLK_U1(bcast_domain);
            int blk = 0;
            if (PORT_IS_U0(port)) {
                blk = blk_u0;
                if (bcast_domain & 0x1) {
                    last_bcast_port_u0[bcast_domain] = port;
                } else if (last_bcast_port_u0[bcast_domain] < 0) {
                    /* Domain 0 & 2 are reverse sequence in the ring. */
                    last_bcast_port_u0[bcast_domain] = port;
                }
            } else {
                blk = blk_u1;
                if (bcast_domain & 0x1) {
                    last_bcast_port_u1[bcast_domain] = port;
                } else if (last_bcast_port_u1[bcast_domain] < 0) {
                    /* Domain 0 & 2 are reverse sequence in the ring. */
                    last_bcast_port_u1[bcast_domain] = port;
                }
            }
            CDPORT_SBUS_CONTROLr_CLR(sbus_ctrl);
            CDPORT_SBUS_CONTROLr_SBUS_BCAST_BLOCK_IDf_SET(sbus_ctrl, blk);
            ioerr += WRITE_CDPORT_SBUS_CONTROLr(unit, sbus_ctrl, port);
        }
    }
    /* first update U0 last port */
    for (idx = 0; idx < num_bcast; idx++) {
        if (last_bcast_port_u0[idx] > 0) {
            port = last_bcast_port_u0[idx];
            ioerr += READ_CDPORT_SBUS_CONTROLr(unit, &sbus_ctrl, port);
            CDPORT_SBUS_CONTROLr_SBUS_CHAIN_LASTf_SET(sbus_ctrl, 1);
            ioerr += WRITE_CDPORT_SBUS_CONTROLr(unit, sbus_ctrl, port);
        }
    }
    /* first update U1 last port */
    for (idx = 0; idx < num_bcast; idx++) {
        if (last_bcast_port_u1[idx] > 0) {
            port = last_bcast_port_u1[idx];
            ioerr += READ_CDPORT_SBUS_CONTROLr(unit, &sbus_ctrl, port);
            CDPORT_SBUS_CONTROLr_SBUS_CHAIN_LASTf_SET(sbus_ctrl, 1);
            ioerr += WRITE_CDPORT_SBUS_CONTROLr(unit, sbus_ctrl, port);
        }
    }
    return ioerr;
}

static int
cdport_reset(int unit, bool sim)
{
    int ioerr = 0;
    int port;
    int idx;
    int last_bcast_port_u0[CDPORT_NUM_BCAST];
    int last_bcast_port_u1[CDPORT_NUM_BCAST];
    CDPORT_XGXS0_CTRL_REGr_t xgxs_ctrl;

    /* Do reset through broadcast command. */
    ioerr += cdport_bcast_enable(unit,
                                 CDPORT_NUM_BCAST,
                                 last_bcast_port_u0,
                                 last_bcast_port_u1);

    for (idx = 0; idx < CDPORT_NUM_BCAST; idx++) {
        if (last_bcast_port_u0[idx] > 0) {
            port = CDPORT_BCAST_PORT_U0(idx);
            /* Power up TSC by clearing TSC_PWRDWN. */
            CDPORT_XGXS0_CTRL_REGr_CLR(xgxs_ctrl);
            ioerr += WRITE_CDPORT_XGXS0_CTRL_REGr(unit, xgxs_ctrl, port);
        }
    }

    for (idx = 0; idx < CDPORT_NUM_BCAST; idx++) {
        if (last_bcast_port_u1[idx] > 0) {
            port = CDPORT_BCAST_PORT_U1(idx);
            /* Power up TSC by clearing TSC_PWRDWN. */
            CDPORT_XGXS0_CTRL_REGr_CLR(xgxs_ctrl);
            ioerr += WRITE_CDPORT_XGXS0_CTRL_REGr(unit, xgxs_ctrl, port);
        }
    }

    sal_usleep(TSC_PWRUP_USEC);

    for (idx = 0; idx < CDPORT_NUM_BCAST; idx++) {
        if (last_bcast_port_u0[idx] > 0) {
            port = CDPORT_BCAST_PORT_U0(idx);
            /* Bring TSC out of reset. */
            CDPORT_XGXS0_CTRL_REGr_TSC_RSTBf_SET(xgxs_ctrl, 1);
            ioerr += WRITE_CDPORT_XGXS0_CTRL_REGr(unit, xgxs_ctrl, port);
        }
    }

    for (idx = 0; idx < CDPORT_NUM_BCAST; idx++) {
        if (last_bcast_port_u1[idx] > 0) {
            port = CDPORT_BCAST_PORT_U1(idx);
            /* Bring TSC out of reset. */
            CDPORT_XGXS0_CTRL_REGr_TSC_RSTBf_SET(xgxs_ctrl, 1);
            ioerr += WRITE_CDPORT_XGXS0_CTRL_REGr(unit, xgxs_ctrl, port);
        }
    }

    /*
     * The simulation environments do not support SBUS broadcast, so
     * in these cases we still need to iterate over all blocks.
     *
     * If this is not done, then the port macro driver will report an
     * error because it checks if the CDPORT blocks are powered up and
     * out of reset.
     */
    if (sim) {
        bcmdrd_pbmp_t pbmp;
        cdport_pbmp_get(unit, &pbmp);
        BCMDRD_PBMP_ITER(pbmp, port) {
            if (block_port_get(unit, port, BLKTYPE_CDPORT) == 0) {
                /* Simulate that TSC is powered up and out of reset. */
                CDPORT_XGXS0_CTRL_REGr_CLR(xgxs_ctrl);
                CDPORT_XGXS0_CTRL_REGr_TSC_RSTBf_SET(xgxs_ctrl, 1);
                ioerr += WRITE_CDPORT_XGXS0_CTRL_REGr(unit, xgxs_ctrl, port);
            }
        }
    }

    return ioerr;
}

static int
wait_for_pll_lock(int unit)
{
    int ioerr = 0;
    int usec;
    TOP_PP_PLL_STATUSr_t pp_pll_status;
    TOP_CORE_PLL_STATUSr_t core_pll_status;

    /* Check PP lock status. */
    usec = 0;
    while (usec < PLL_LOCK_MAX_USEC) {
        ioerr += READ_TOP_PP_PLL_STATUSr(unit, &pp_pll_status);
        if (TOP_PP_PLL_STATUSr_LOCKf_GET(pp_pll_status) == 1) {
            break;
        }
        sal_usleep(PLL_LOCK_USEC);
        usec += PLL_LOCK_USEC;
    }
    if (usec >= PLL_LOCK_MAX_USEC) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "PP PLL did not lock, status = 0x%08"PRIx32"\n"),
                  TOP_PP_PLL_STATUSr_GET(pp_pll_status)));
    }

    /* Check core PLL lock status */
    usec = 0;
    while (usec < PLL_LOCK_USEC) {
        ioerr += READ_TOP_CORE_PLL_STATUSr(unit, &core_pll_status);
        if (TOP_CORE_PLL_STATUSr_LOCKf_GET(core_pll_status) == 1) {
            break;
        }
        sal_usleep(PLL_LOCK_USEC);
        usec += PLL_LOCK_USEC;
    }
    if (usec >= PLL_LOCK_MAX_USEC) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Core PLL did not lock, status = 0x%08"PRIx32"\n"),
                  TOP_CORE_PLL_STATUSr_GET(core_pll_status)));
    }

    return ioerr;
}

/*!
 * Performs steps 1-3 of the CORE_PLL Programming sequence, and
 * is done when TH4G is operating at a different core clock frequency
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
    sal_usleep(PLL_BYPASS_USEC);

    /* To 3: Programming of PLL config parameters follows this in calling
     * function
     */

    return ioerr;
}

/*!
 * Performs steps 5-9 of the CORE_PLL Programming sequence, and
 * is done when TH4G is operating at a different core clock frequency
 */
static int
core_pll_bypass_prog_post(int unit)
{
    int ioerr = 0;
    int usec;
    TOP_CORE_PLL_CTRL_2r_t top_core_pll_ctrl_2;
    TOP_CORE_PLL_STATUSr_t core_pll_status;

    /*
     * From 3: This follows the programming of the PLL config parameters from
     * the calling function
     */

    /* 4 */
    ioerr += READ_TOP_CORE_PLL_CTRL_2r(unit, &top_core_pll_ctrl_2);
    TOP_CORE_PLL_CTRL_2r_SW_CFG_SELf_SET(top_core_pll_ctrl_2, 1);
    ioerr += WRITE_TOP_CORE_PLL_CTRL_2r(unit, top_core_pll_ctrl_2);

    /* 5: wait to core clock lock */
    usec = 0;
    while (usec < PLL_LOCK_MAX_USEC) {
        sal_usleep(PLL_LOCK_USEC);
        ioerr += READ_TOP_CORE_PLL_STATUSr(unit, &core_pll_status);
        if (TOP_CORE_PLL_STATUSr_LOCKf_GET(core_pll_status) == 1) {
            break;
        }
        usec += PLL_LOCK_USEC;
    }

    if (usec < PLL_LOCK_MAX_USEC) {
        /* 6 */
        ioerr += READ_TOP_CORE_PLL_CTRL_2r(unit, &top_core_pll_ctrl_2);
        TOP_CORE_PLL_CTRL_2r_CH6_BYP_ENf_SET(top_core_pll_ctrl_2, 0);
        ioerr += WRITE_TOP_CORE_PLL_CTRL_2r(unit, top_core_pll_ctrl_2);
    } else {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Core PLL did not lock, status = 0x%08"PRIx32"\n"),
                  TOP_CORE_PLL_STATUSr_GET(core_pll_status)));
    }

    return ioerr;
}

/*!
 * Performs necessary steps to configure the chip for different
 * core frequencies.
 */
static
int core_pll_config(int unit, uint32_t frequency)
{
    int ioerr = 0;
    int ndiv_int;
    TOP_CORE_PLL_CTRL_0r_t top_core_pll_ctrl_0;

    if (frequency != PLL_FREQ_DEF) {
        ndiv_int = PLL_NDIV(frequency);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Change core ndiv to %d\n"),
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
static
int pp_pll_config(int unit, uint32_t frequency)
{
    int ioerr = 0;
    int ndiv_int = -1;
    TOP_PP_PLL_CTRL_0r_t top_pp_pll_0;
    TOP_PP_PLL_CTRL_2r_t top_pp_pll_2;

    /* For default frequency, do nothing. */
    if (frequency != PLL_FREQ_DEF) {
        ndiv_int = PLL_NDIV(frequency);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit , "Change PP ndiv to %d\n"),
                     ndiv_int));

        /* hold all post div channels */
        ioerr += READ_TOP_PP_PLL_CTRL_2r(unit, &top_pp_pll_2);
        TOP_PP_PLL_CTRL_2r_POST_HOLD_ALLf_SET(top_pp_pll_2, 1);
        ioerr += WRITE_TOP_PP_PLL_CTRL_2r(unit, top_pp_pll_2);

        sal_usleep(PLL_PP_HOLD_USEC);

        /* Set ndiv_int and mdiv for target freq */
        ioerr += READ_TOP_PP_PLL_CTRL_0r(unit, &top_pp_pll_0);
        TOP_PP_PLL_CTRL_0r_NDIV_INTf_SET(top_pp_pll_0, ndiv_int);
        ioerr += WRITE_TOP_PP_PLL_CTRL_0r(unit, top_pp_pll_0);

        /* Wait for 25us */
        sal_usleep(PLL_PP_CONFIG_USEC);

        /* Release all post div channels to normal operation */
        ioerr += READ_TOP_PP_PLL_CTRL_2r(unit, &top_pp_pll_2);
        TOP_PP_PLL_CTRL_2r_POST_HOLD_ALLf_SET(top_pp_pll_2, 0);
        ioerr += WRITE_TOP_PP_PLL_CTRL_2r(unit, top_pp_pll_2);
    }

    return ioerr;
}

/* Lookup frequency by ID. */
static int
get_clock(uint32_t id, uint32_t *freq)
{
    int i = 0;
    int count = COUNTOF(clk_freqs);

    for (i = 0; i < count; i++) {
        if (clk_freqs[i].id == id) {
            *freq = clk_freqs[i].freq;
            return SHR_E_NONE;
        }
    }

    return SHR_E_NOT_FOUND;
}

/*!
 * Determine a PLL's current DCO value. The PLL should be checked to ensure
 * that it is locked prior to calling this function.
 */
static int32_t
pll_dco_code_get(int32_t unit, pll_type_e pll, uint32_t *dco_code)
{
    int ioerr = 0;
    TOP_TS_PLL_CTRL_4r_t ts_ctrl;
    TOP_BS_PLL_CTRL_4r_t bs0_ctrl;
    TOP_BS_PLL_CTRL_4r_t bs1_ctrl;
    TOP_TS_PLL_STATUSr_t ts_status;
    TOP_BS_PLL_STATUSr_t bs_status;

    if (pll == TIME_SYNC_PLL) {
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

        sal_usleep(10 * MSEC_USEC);

        /* Get the DCO code */
        ioerr += READ_TOP_TS_PLL_STATUSr(unit, &ts_status);
        *dco_code = TOP_TS_PLL_STATUSr_STATf_GET(ts_status);

        /* Clear the capture state */
        ioerr += READ_TOP_TS_PLL_CTRL_4r(unit, &ts_ctrl);
        TOP_TS_PLL_CTRL_4r_STAT_UPDATEf_SET(ts_ctrl, 0);
        ioerr += WRITE_TOP_TS_PLL_CTRL_4r(unit, ts_ctrl);
    } else if (pll == BROAD_SYNC_PLL0) {
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

        sal_usleep(10 * MSEC_USEC);

        /* Get the DCO code */
        ioerr += READ_TOP_BS_PLL_STATUSr(unit, 0, &bs_status);
        *dco_code = TOP_BS_PLL_STATUSr_STATf_GET(bs_status);

        /* Clear the capture state */
        ioerr += READ_TOP_BS_PLL_CTRL_4r(unit, 0, &bs0_ctrl);
        TOP_BS_PLL_CTRL_4r_STAT_UPDATEf_SET(bs0_ctrl, 0);
        ioerr += WRITE_TOP_BS_PLL_CTRL_4r(unit, 0,  bs0_ctrl);
    } else if (pll == BROAD_SYNC_PLL1) {
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

        sal_usleep(10 * MSEC_USEC);

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

/* Linearly scale the DCO code as a function of the VCO's frequency. */
static int
pll_dco_normalize(uint32_t *dco_code, uint32_t ref_freq, uint32_t ndiv)
{
    static const uint32_t int_ref_freq = 50000000;

    *dco_code <<= 20;
    if (ref_freq == 0) {
        *dco_code /=  (int_ref_freq / 100000 * ndiv);
    } else {
        *dco_code /= (ref_freq / 100000 * ndiv);
    }

    return SHR_E_NONE;
}

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
    uint8_t src_sel;
    int index;
    static const uint32_t auto_ref = TS_CLK(AUTO);
    static const uint32_t def_src = TS_CLK(INT_50MHZ);
    uint32_t dco_code, dco_code_target;
    static const uint32_t dco_poison = 0xffff;
    int rv;

    ref_clk = bcmbd_config_get(unit, def_src,
                               "DEVICE_CONFIG",
                               "TS_PLL_CLK_SEL");

    index = (ref_clk == auto_ref) ? (COUNTOF(ts_pll_params) - 1) : 0;
    dco_code_target = dco_poison;

    for (; index < COUNTOF(ts_pll_params); index++) {
        if (ref_clk != auto_ref && ts_pll_params[index].ref_freq != ref_clk) {
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
        switch (ts_pll_params[index].ref_freq) {
            case 0:
            case TS_CLK(INT_50MHZ):
                src_sel = 0;
                break;
            default:
                src_sel = 1;
                break;
        }
        ioerr += READ_TOP_TS_PLL_CTRL_1r(unit, &pll_ctrl_1);
        TOP_TS_PLL_CTRL_1r_REFCLK_SOURCE_SELf_SET(pll_ctrl_1, src_sel);
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
        sal_usleep(PLL_TS_BS_LOCK_USEC);

        /* Check PLL lock status */
        ioerr += READ_TOP_TS_PLL_STATUSr(unit, &ts_pll_status);
        if (TOP_TS_PLL_STATUSr_LOCKf_GET(ts_pll_status) == 1) {
            if (ref_clk == auto_ref && dco_code_target == dco_poison) {
                /* If auto freq, get DCO value for later comparison */
                rv = pll_dco_code_get(unit, TIME_SYNC_PLL, &dco_code_target);
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
                rv = pll_dco_code_get(unit, TIME_SYNC_PLL, &dco_code);
                if (SHR_FAILURE(rv)) {
                    return rv;
                }
                pll_dco_normalize(&dco_code,
                                  ts_pll_params[index].ref_freq_val,
                                  ts_pll_params[index].ndiv);

                if (dco_code >= (dco_code_target - 400) &&
                    dco_code <= (dco_code_target + 400)) {
                    if (ts_pll_params[index].ref_freq_val) {
                        LOG_WARN(BSL_LOG_MODULE,
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
                        LOG_WARN(BSL_LOG_MODULE,
                                 (BSL_META_U(unit,
                                             "TS_PLL locked on unit %d using "
                                             "internal reference clock\n"),
                                  unit));
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
            LOG_ERROR(BSL_LOG_MODULE,
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
        sal_usleep(PLL_TS_BS_LOCK_USEC);

        /* Check PLL lock status */
        ioerr += READ_TOP_BS_PLL_STATUSr(unit, 0, &pll_status);
        if (TOP_BS_PLL_STATUSr_LOCKf_GET(pll_status) == 1) {
            if (ref_clk == auto_ref && dco_code_target == dco_poison) {
                /* If auto freq, get DCO value for later comparison */
                rv = pll_dco_code_get(unit, BROAD_SYNC_PLL0, &dco_code_target);
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
                rv = pll_dco_code_get(unit, BROAD_SYNC_PLL0, &dco_code);
                if (SHR_FAILURE(rv)) {
                    return rv;
                }
                pll_dco_normalize(&dco_code,
                                  bs_pll_params[index].ref_freq_val,
                                  bs_pll_params[index].ndiv);

                if (dco_code >= (dco_code_target - 400) &&
                        dco_code <= (dco_code_target + 400)) {
                    if (bs_pll_params[index].ref_freq_val) {
                        LOG_WARN(BSL_LOG_MODULE,
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
                        LOG_WARN(BSL_LOG_MODULE,
                                 (BSL_META_U(unit,
                                             "BS_PLL 0 locked on unit %d using "
                                             "internal reference clock\n"),
                                  unit));
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
            LOG_ERROR(BSL_LOG_MODULE,
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
        if (ref_clk != auto_ref && bs_pll_params[index].ref_freq != ref_clk) {
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
        sal_usleep(PLL_TS_BS_LOCK_USEC);

        /* Check PLL lock status */
        ioerr += READ_TOP_BS_PLL_STATUSr(unit, 1, &pll_status);
        if (TOP_BS_PLL_STATUSr_LOCKf_GET(pll_status) == 1) {

            if (ref_clk == auto_ref && dco_code_target == dco_poison) {
                /* If auto freq, get DCO value for later comparison */
                rv = pll_dco_code_get(unit, BROAD_SYNC_PLL1, &dco_code_target);
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
                rv = pll_dco_code_get(unit, BROAD_SYNC_PLL1, &dco_code);
                if (SHR_FAILURE(rv)) {
                    return rv;
                }
                pll_dco_normalize(&dco_code,
                                  bs_pll_params[index].ref_freq_val,
                                  bs_pll_params[index].ndiv);

                if (dco_code >= (dco_code_target - 400) &&
                        dco_code <= (dco_code_target + 400)) {
                    if (bs_pll_params[index].ref_freq_val) {
                        LOG_WARN(BSL_LOG_MODULE,
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
                        LOG_WARN(BSL_LOG_MODULE,
                                 (BSL_META_U(unit,
                                             "BS_PLL 1 locked on unit %d using "
                                             "internal reference clock\n"),
                                  unit));
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
            LOG_ERROR(BSL_LOG_MODULE,
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

static
int pll_init(int unit, int wait_usec)
{
    int ioerr = 0;
    TOP_SOFT_RESET_REG_2r_t top_sreset_2;
    uint32_t id, core_clock, pp_clock;

    /* Configure core clock. */
    id = bcmbd_config_get(unit, PLL_FREQ_DEF_ID,
                          "DEVICE_CONFIG", "CORE_CLK_FREQ");
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "DEVICE_CONFIG.CORE_CLK_FREQ: %d.\n"), id));
    if (SHR_FAILURE(get_clock(id, &core_clock))) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Invalid CORE_CLK_FREQ %d."
                             "Using default value.\n"), id));
        id = PLL_FREQ_DEF_ID;
        core_clock = PLL_FREQ_DEF;
     }
    ioerr += core_pll_config(unit, core_clock);

    /* Configure PP clock. */
    id = bcmbd_config_get(unit, id,
                          "DEVICE_CONFIG", "PP_CLK_FREQ");
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "DEVICE_CONFIG.PP_CLK_FREQ: %d.\n"), id));
    if (SHR_FAILURE(get_clock(id, &pp_clock)) || (pp_clock > core_clock)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Invalid PP_CLK_FREQ %d. Using core frequency.\n"),
                  id));
        pp_clock = core_clock;
    }
    ioerr += pp_pll_config(unit, pp_clock);

    /*
     * Bring LCPLL, IPROC, PP, CORE PLLs and AVS out of reset.
     */
    ioerr += READ_TOP_SOFT_RESET_REG_2r(unit, &top_sreset_2);
    TOP_SOFT_RESET_REG_2r_TOP_AVS_RST_Lf_SET(top_sreset_2, 1);
    TOP_SOFT_RESET_REG_2r_TOP_IPROC_PLL_RST_Lf_SET(top_sreset_2, 1);
    TOP_SOFT_RESET_REG_2r_TOP_PP_PLL_RST_Lf_SET(top_sreset_2, 1);
    TOP_SOFT_RESET_REG_2r_TOP_CORE_PLL_RST_Lf_SET(top_sreset_2, 1);
    ioerr += WRITE_TOP_SOFT_RESET_REG_2r(unit, top_sreset_2);

    sal_usleep(PLL_LOCK_USEC);

    /* Wait for PLLs lock. */
    ioerr += wait_for_pll_lock(unit);

    /* De-assert LCPLL's post reset. */
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

static int
half_chip_mode_init(int unit)
{
    int rv = 0;
    int ioerr = 0;
    TOP_HALF_MODE_CTRLr_t top_half_mode_ctrl;
    uint32_t pipe_map;
    int itm_idx=0, itm_num;
    int idx, itm_vld_num = 0;
    bcmdrd_pipe_info_t pipe_info, *pi = &pipe_info;

    /* Get full chip ITM number. */
    sal_memset(pi, 0, sizeof(*pi));
    pi->blktype = BLKTYPE_MMU_ITM;
    itm_num = bcmdrd_dev_pipe_info(unit, pi,
                                   BCMDRD_PIPE_INFO_TYPE_NUM_PIPE_INST);

    /* Get ITM valid mask. */
    rv = bcmdrd_dev_valid_blk_pipes_get(unit, 0, BLKTYPE_MMU_ITM, &pipe_map);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    for (idx = 0; idx < itm_num; idx++) {
        if (pipe_map & (1 << idx)) {
            itm_idx = idx;
            itm_vld_num++;
        }
    }
    if (itm_vld_num == 0) {
        return SHR_E_INTERNAL;
    }

    if (itm_vld_num == 1) {
        ioerr += READ_TOP_HALF_MODE_CTRLr(unit, &top_half_mode_ctrl);
        TOP_HALF_MODE_CTRLr_TOP_HALF_CHIP_MODEf_SET(top_half_mode_ctrl, 1);
        TOP_HALF_MODE_CTRLr_TOP_HALF_CHIP_MODE_NUMf_SET(top_half_mode_ctrl,
                                                        itm_idx);
        ioerr += WRITE_TOP_HALF_MODE_CTRLr(unit, top_half_mode_ctrl);
    }

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

/*
 * Sequential reset configuration:
 *  Hardware sequential reset TSC number: use default - each step reset 1/16.
 *  Hardware sequential reset time space: use default - 100 us.
 *  Sequential reset time out should be not les than software timeout +
 *  hardware time space * steps.
 *  In software mode, sequential reset is processed in PCIe firmware loader.
 */
static int
sequence_reset_config(int unit)
{
    int ioerr = 0;
    uint32_t srst_t;
    TOP_SEQ_RST_0r_t srst0;
    SEQ_RST_CONFIGr_t srst_cfg;

    srst_t = bcmbd_config_get(unit, SEQ_RESET_T(SW),
                              "DEVICE_CONFIG", "SEQ_RESET");
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "DEVICE_CONFIG.SEQ_RESET: %d.\n"),
                 srst_t));

    if (srst_t == SEQ_RESET_T(SW)) {
        /* Configuring software timeout. */
        ioerr += READ_TOP_SEQ_RST_0r(unit, &srst0);
        TOP_SEQ_RST_0r_SW_SEQ_RST_TIMEOUT_DURATIONf_SET(srst0, SEQRST_SW_TIME);
        ioerr += WRITE_TOP_SEQ_RST_0r(unit, srst0);
        /* Configure Sequential reset timeout. */
        ioerr += READ_SEQ_RST_CONFIGr(unit, &srst_cfg);
        SEQ_RST_CONFIGr_TOPMGMT_RSTSEQ_TIMEOUTf_SET(srst_cfg, SEQRST_TIME);
        /* Enable Sequential reset. */
        SEQ_RST_CONFIGr_TOPMGMT_RSTSEQ_ENf_SET(srst_cfg, 1);
        ioerr += WRITE_SEQ_RST_CONFIGr(unit, srst_cfg);
    } else if (srst_t == SEQ_RESET_T(HW)) {
        /* Emulate hardware mode by configuring software timeout to 1. */
        ioerr += READ_TOP_SEQ_RST_0r(unit, &srst0);
        TOP_SEQ_RST_0r_SW_SEQ_RST_TIMEOUT_DURATIONf_SET(srst0, SEQRST_SW_TIME_HW);
        ioerr += WRITE_TOP_SEQ_RST_0r(unit, srst0);
        /* Configure Sequential reset timeout. */
        ioerr += READ_SEQ_RST_CONFIGr(unit, &srst_cfg);
        SEQ_RST_CONFIGr_TOPMGMT_RSTSEQ_TIMEOUTf_SET(srst_cfg, SEQRST_TIME_HW);
        /* Enable Sequential reset. */
        SEQ_RST_CONFIGr_TOPMGMT_RSTSEQ_ENf_SET(srst_cfg, 1);
        ioerr += WRITE_SEQ_RST_CONFIGr(unit, srst_cfg);
    } else {
        ioerr += READ_SEQ_RST_CONFIGr(unit, &srst_cfg);
        SEQ_RST_CONFIGr_TOPMGMT_RSTSEQ_ENf_SET(srst_cfg, 0);
        ioerr += WRITE_SEQ_RST_CONFIGr(unit, srst_cfg);
    }

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

int
bcm56996_a0_bd_dev_reset(int unit)
{
    int rv;
    int ioerr = 0;
    int wait_usec;
    int idx;
    int port;
    bcmdrd_pbmp_t pbmp;
    uint32_t val;
    CMIC_TOP_SBUS_RING_MAPr_t sbus_ring_map;
    CMIC_TOP_SBUS_TIMEOUTr_t sbus_to;
    DMU_CRU_RESETr_t cru_reset;
    TOP_SOFT_RESET_REGr_t top_sreset;
    TOP_SOFT_RESET_REG_3r_t top_sreset_3;
    TOP_SOFT_RESET_REG_4r_t top_sreset_4;
    TOP_IP_CLOCK_GATING_DISABLE_REGr_t ip_clk_disable;
    TOP_EP_CLOCK_GATING_DISABLE_REGr_t ep_clk_disable;
    SEQ_RST_CONFIGr_t srst_cfg;
    CDPORT_MAC_CONTROLr_t cmac_ctrl;
    XLPORT_MAC_CONTROLr_t xmac_ctrl;

    bool emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL) ;
    bool sim = bcmdrd_feature_is_sim(unit);

    /* The index is block type number, each block type holds 4 bits which are
     * for ring number.
     * SBUS ring and block number:
     * ring 0: OTPC(6), AVS(87), TOP(8)
     * ring 1: IP(1)
     * ring 2: EP(2)
     * ring 3: MMU_ITM(3), MMU_EB(4), MMU_GLB(5)
     * ring 4: CEV(88)
     * ring 5: PORT_IF(89)
     * ring 6: PFC(90)
     * ring 7: PM0(16)-15(31), PBST0(102/103)
     * ring 8: Pm31(47)-16(32), PBST1(104/105)
     * ring 9: PM32(48)-47(63), PBST0(106/107)
     * ring 10: PM63(79)-48(64), PBST0(108/109)
     * ring 11: PM4x10(12), MGMT_OBM(101)
     */
    uint32_t ring_map[] = { 0x00333210, 0x000B0000, 0x77777777, 0x77777777,
                            0x88888888, 0x88888888, 0x99999999, 0x99999999,
                            0xAAAAAAAA, 0xAAAAAAAA, 0x00000000, 0x00000654,
                            0x77B00000, 0x00AA9988, 0x00000000, 0x00000000 };

    wait_usec = (emul ? EMUL_WAIT_MSEC : WAIT_MSEC) * 1000;

    /* Initialize endian mode for correct reset access */
    rv = bcmbd_cmicx_init(unit);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Reset Port Macros. */
    READ_TOP_SOFT_RESET_REG_3r(unit, &top_sreset_3);
    val = TOP_SOFT_RESET_REG_3r_GET(top_sreset_3);
    if (val) {
        TOP_SOFT_RESET_REG_3r_SET(top_sreset_3, val & 0xffffff00);
        ioerr += WRITE_TOP_SOFT_RESET_REG_3r(unit, top_sreset_3);
        sal_usleep(PM_RESET_USEC);
        TOP_SOFT_RESET_REG_3r_SET(top_sreset_3, val & 0xffff0000);
        ioerr += WRITE_TOP_SOFT_RESET_REG_3r(unit, top_sreset_3);
        sal_usleep(PM_RESET_USEC);
        TOP_SOFT_RESET_REG_3r_SET(top_sreset_3, val & 0xff000000);
        ioerr += WRITE_TOP_SOFT_RESET_REG_3r(unit, top_sreset_3);
        sal_usleep(PM_RESET_USEC);
        TOP_SOFT_RESET_REG_3r_SET(top_sreset_3, 0x0);
        ioerr += WRITE_TOP_SOFT_RESET_REG_3r(unit, top_sreset_3);
        sal_usleep(PM_RESET_USEC);
    }
    READ_TOP_SOFT_RESET_REG_4r(unit, &top_sreset_4);
    val = TOP_SOFT_RESET_REG_4r_GET(top_sreset_4);
    if (val) {
        TOP_SOFT_RESET_REG_4r_SET(top_sreset_4, val & 0xffffff00);
        ioerr += WRITE_TOP_SOFT_RESET_REG_4r(unit, top_sreset_4);
        sal_usleep(PM_RESET_USEC);
        TOP_SOFT_RESET_REG_4r_SET(top_sreset_4, val & 0xffff0000);
        ioerr += WRITE_TOP_SOFT_RESET_REG_4r(unit, top_sreset_4);
        sal_usleep(PM_RESET_USEC);
        TOP_SOFT_RESET_REG_4r_SET(top_sreset_4, val & 0xff000000);
        ioerr += WRITE_TOP_SOFT_RESET_REG_4r(unit, top_sreset_4);
        sal_usleep(PM_RESET_USEC);
        TOP_SOFT_RESET_REG_4r_SET(top_sreset_4, 0x0);
        ioerr += WRITE_TOP_SOFT_RESET_REG_4r(unit, top_sreset_4);
        sal_usleep(PM_RESET_USEC);
    }

    /* Reset Switch/CMIC */
    READ_TOP_SOFT_RESET_REGr(unit, &top_sreset);
    val = TOP_SOFT_RESET_REGr_GET(top_sreset);
    if (sim) {
        TOP_SOFT_RESET_REGr_CLR(top_sreset);
        ioerr += WRITE_TOP_SOFT_RESET_REGr(unit, top_sreset);
    } else if (val) {
        /* Sequential reset. */
        TOP_SOFT_RESET_REGr_TOP_IP_RST_Lf_SET(top_sreset, 0);
        ioerr += WRITE_TOP_SOFT_RESET_REGr(unit, top_sreset);
        sal_usleep(PM_RESET_USEC);
        TOP_SOFT_RESET_REGr_TOP_EP_RST_Lf_SET(top_sreset, 0);
        ioerr += WRITE_TOP_SOFT_RESET_REGr(unit, top_sreset);
        sal_usleep(PM_RESET_USEC);
        TOP_SOFT_RESET_REGr_TOP_MMU_RST_Lf_SET(top_sreset, 0);
        ioerr += WRITE_TOP_SOFT_RESET_REGr(unit, top_sreset);
        sal_usleep(PM_RESET_USEC);
        /* Disable sequential reset before do software reset. */
        ioerr += READ_SEQ_RST_CONFIGr(unit, &srst_cfg);
        SEQ_RST_CONFIGr_TOPMGMT_RSTSEQ_ENf_SET(srst_cfg, 0);
        ioerr += WRITE_SEQ_RST_CONFIGr(unit, srst_cfg);
        /* Reset others. */
        DMU_CRU_RESETr_SET(cru_reset, (0x1 << 1));
        ioerr += WRITE_DMU_CRU_RESETr(unit, cru_reset);
        sal_usleep(wait_usec);
    }

    /* Re-initialize endian mode after reset */
    ioerr += bcmbd_cmicx_init(unit);

    for (idx = 0; idx < COUNTOF(ring_map); idx++) {
        CMIC_TOP_SBUS_RING_MAPr_SET(sbus_ring_map, ring_map[idx]);
        ioerr += WRITE_CMIC_TOP_SBUS_RING_MAPr(unit, idx, sbus_ring_map);
    }
    CMIC_TOP_SBUS_TIMEOUTr_SET(sbus_to, 0x2700);
    ioerr += WRITE_CMIC_TOP_SBUS_TIMEOUTr(unit, sbus_to);

    /* PLLs are not required in simulation or emulation environments */
    if (!(sim || emul)) {
        ioerr += pll_init(unit, wait_usec);

        /* Configure sequential reset. */
        rv = sequence_reset_config(unit);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
    }

    rv = half_chip_mode_init(unit);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Bring port blocks out of reset. Port should be disabled in hardware
     * default status, otherwise releasing TOP_SOFT_RESET_REG_3/4 should not
     * be done all in one.
     */
    TOP_SOFT_RESET_REGr_CLR(top_sreset);
    TOP_SOFT_RESET_REGr_TOP_PM_MGMT_RST_Lf_SET(top_sreset, 1);
    TOP_SOFT_RESET_REGr_TOP_TS_RST_Lf_SET(top_sreset, 1);
    ioerr += WRITE_TOP_SOFT_RESET_REGr(unit, top_sreset);
    TOP_SOFT_RESET_REG_3r_SET(top_sreset_3, 0xffffffff);
    ioerr += WRITE_TOP_SOFT_RESET_REG_3r(unit, top_sreset_3);
    TOP_SOFT_RESET_REG_4r_SET(top_sreset_4, 0xffffffff);
    ioerr += WRITE_TOP_SOFT_RESET_REG_4r(unit, top_sreset_4);

    /* Disable clock gating before de-assert IP/EP/MMU SOFT reset. */
    TOP_IP_CLOCK_GATING_DISABLE_REGr_SET(ip_clk_disable, 0xffffffff);
    ioerr += WRITE_TOP_IP_CLOCK_GATING_DISABLE_REGr(unit, ip_clk_disable);
    TOP_EP_CLOCK_GATING_DISABLE_REGr_SET(ep_clk_disable, 0xffffffff);
    ioerr += WRITE_TOP_EP_CLOCK_GATING_DISABLE_REGr(unit, ep_clk_disable);
    sal_usleep(CLOCK_GATING_WAIT_USEC);

    /* Bring IP, EP, and MMU blocks out of reset. */
    ioerr += READ_TOP_SOFT_RESET_REGr(unit, &top_sreset);
    TOP_SOFT_RESET_REGr_TOP_EP_RST_Lf_SET(top_sreset, 1);
    ioerr += WRITE_TOP_SOFT_RESET_REGr(unit, top_sreset);
    TOP_SOFT_RESET_REGr_TOP_MMU_RST_Lf_SET(top_sreset, 1);
    ioerr += WRITE_TOP_SOFT_RESET_REGr(unit, top_sreset);
    TOP_SOFT_RESET_REGr_TOP_IP_RST_Lf_SET(top_sreset, 1);
    ioerr += WRITE_TOP_SOFT_RESET_REGr(unit, top_sreset);
    sal_usleep(wait_usec);

    /* Enable clock gating after de-assert IP/EP/MMU SOFT reset. */
    TOP_IP_CLOCK_GATING_DISABLE_REGr_CLR(ip_clk_disable);
    ioerr += WRITE_TOP_IP_CLOCK_GATING_DISABLE_REGr(unit, ip_clk_disable);
    TOP_EP_CLOCK_GATING_DISABLE_REGr_CLR(ep_clk_disable);
    ioerr += WRITE_TOP_EP_CLOCK_GATING_DISABLE_REGr(unit, ep_clk_disable);

    rv = bypass_mode_init(unit);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Reset CDPORT blocks. */
    ioerr += cdport_reset(unit, sim);

    /* Reset XLPORT blocks */
    xlport_pbmp_get(unit, &pbmp);
    BCMDRD_PBMP_ITER(pbmp, port) {
        ioerr += xlport_reset(unit, port, emul);
        /* Only one single XLPORT block */
        break;
    }

    /* Reset CDMACs */
    cdport_pbmp_get(unit, &pbmp);
    BCMDRD_PBMP_ITER(pbmp, port) {
        if (block_port_get(unit, port, BLKTYPE_CDPORT) == 0) {
            ioerr += READ_CDPORT_MAC_CONTROLr(unit, &cmac_ctrl, port);
            CDPORT_MAC_CONTROLr_CDMAC_RESETf_SET(cmac_ctrl, 1);
            ioerr += WRITE_CDPORT_MAC_CONTROLr(unit, cmac_ctrl, port);
        }
    }
    sal_usleep(MAC_RESET_USEC);
    BCMDRD_PBMP_ITER(pbmp, port) {
        if (block_port_get(unit, port, BLKTYPE_CDPORT) == 0) {
            ioerr += READ_CDPORT_MAC_CONTROLr(unit, &cmac_ctrl, port);
            CDPORT_MAC_CONTROLr_CDMAC_RESETf_SET(cmac_ctrl, 0);
            ioerr += WRITE_CDPORT_MAC_CONTROLr(unit, cmac_ctrl, port);
        }
    }

    /* Reset XLMACs */
    xlport_pbmp_get(unit, &pbmp);
    BCMDRD_PBMP_ITER(pbmp, port) {
        ioerr += READ_XLPORT_MAC_CONTROLr(unit, &xmac_ctrl, port);
        XLPORT_MAC_CONTROLr_XMAC0_RESETf_SET(xmac_ctrl, 1);
        ioerr += WRITE_XLPORT_MAC_CONTROLr(unit, xmac_ctrl, port);
        sal_usleep(MAC_RESET_USEC);
        XLPORT_MAC_CONTROLr_XMAC0_RESETf_SET(xmac_ctrl, 0);
        ioerr += WRITE_XLPORT_MAC_CONTROLr(unit, xmac_ctrl, port);
    }

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}
