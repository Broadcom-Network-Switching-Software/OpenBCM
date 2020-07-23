/*! \file bcm56990_b0_bd_attach.c
 *
 * Initialize base driver resources.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <shr/shr_debug.h>

#include <bcmbd/bcmbd_config.h>
#include <bcmbd/bcmbd_cmicx2.h>
#include <bcmbd/bcmbd_cmicx_intr.h>
#include <bcmbd/bcmbd_cmicx_hotswap.h>
#include <bcmbd/bcmbd_i2c.h>
#include <bcmbd/bcmbd_cmicx_qspi.h>
#include <bcmbd/bcmbd_cmicx_pcie.h>
#include <bcmbd/bcmbd_cmicx_miim.h>
#include <bcmbd/bcmbd_cmicx_schan_error.h>
#include <bcmbd/bcmbd_internal.h>
#include <bcmbd/bcmbd_cmicx2_sw_intr.h>
#include <bcmbd/bcmbd_cmicx2_ts_intr.h>
#include <bcmbd/bcmbd_cmicx2_tsfifo.h>
#include <bcmbd/bcmbd_led_internal.h>
#include <bcmbd/bcmbd_ts_internal.h>
#include <bcmbd/bcmbd_tsfifo.h>
#include <bcmbd/bcmbd_m0ssq_internal.h>
#include <bcmbd/bcmbd_fwm.h>
#include <bcmbd/bcmbd_otp.h>
#include <bcmbd/chip/bcm56990_b0_dev.h>

#include <bcmdrd/bcmdrd_hal.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmdrd/bcmdrd_port.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/chip/bcm56990_b0_defs.h>

#include "bcm56990_b0_drv.h"

/* Log source for this component */
#define BSL_LOG_MODULE BSL_LS_BCMBD_DEV

/*******************************************************************************
* Local definitions
 */

/* Physical block numbers */
#define IPIPE               1
#define EPIPE               2
#define AT_UNIQUE           31

static bcmdrd_port_info_t bcm56990_b0_port_info[] = {
    /* Phys  Port type             Logic  MMU */
    {     0, BCMDRD_PORT_TYPE_CPU,     0,  16 },
    {   259, BCMDRD_PORT_TYPE_LB,     33,  48 },
    {   260, BCMDRD_PORT_TYPE_LB,     67, 112 },
    {   261, BCMDRD_PORT_TYPE_LB,    101, 176 },
    {   262, BCMDRD_PORT_TYPE_LB,    135, 240 },
    {   263, BCMDRD_PORT_TYPE_LB,    169, 304 },
    {   264, BCMDRD_PORT_TYPE_LB,    203, 368 },
    {   265, BCMDRD_PORT_TYPE_LB,    237, 432 },
    {   266, BCMDRD_PORT_TYPE_LB,    271, 496 },
    {   258, BCMDRD_PORT_TYPE_MGMT,   50,  80 },
    {   257, BCMDRD_PORT_TYPE_MGMT,  152, 272 },
    {   267, BCMDRD_PORT_TYPE_RSVD,   84, 144 },
    {   268, BCMDRD_PORT_TYPE_RSVD,  118, 208 },
    {   269, BCMDRD_PORT_TYPE_RSVD,  186, 336 },
    {   270, BCMDRD_PORT_TYPE_RSVD,  220, 400 },
    {   271, BCMDRD_PORT_TYPE_RSVD,  254, 464 },
    {    -1  }
};

/*******************************************************************************
* Private functions
 */

static bool
bcm56990_b0_addr_bypass(int unit, uint32_t adext, uint32_t addr)
{
    int block;

    if (bcmdrd_dev_bypass_mode_get(unit) == BYPASS_MODE_EPIP) {
        block = (adext & 0xff);
        if (block == IPIPE || block == EPIPE) {
            LOG_VERBOSE(BSL_LS_BCMBD_BYPASS,
                        (BSL_META_U(unit,
                                    "Address bypass: "
                                    "0x%04"PRIx32"_%08"PRIx32"\n"),
                         adext, addr));
            return true;
        }
    }

    return false;
}

static int
bcm56990_b0_feature_init(int unit)
{
    bcmdrd_feature_enable(unit, BCMDRD_FT_IPROC);
    bcmdrd_feature_enable(unit, BCMDRD_FT_KNET);

    return SHR_E_NONE;
}

static int
bcm56990_b0_bd_dev_start(int unit)
{
    bool ts_en = false;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_intr_connect(unit));

    SHR_IF_ERR_EXIT
        (bcmbd_cmicx2_lp_intr_start(unit));

    SHR_IF_ERR_EXIT
        (bcmbd_cmicx2_sw_intr_start(unit));

    SHR_IF_ERR_EXIT
        (bcmbd_cmicx2_ts_intr_start(unit));

    SHR_IF_ERR_EXIT
        (bcmbd_ts_init(unit));
    /* Enable TS1 timestamp counter */
    SHR_IF_ERR_EXIT
        (bcmbd_ts_timestamp_enable_get(unit, 1, &ts_en));
    if (!ts_en) {
        SHR_IF_ERR_EXIT
            (bcmbd_ts_timestamp_enable_set(unit, 1, true));
    }

    SHR_IF_ERR_EXIT
        (bcmbd_tsfifo_init(unit));

    SHR_IF_ERR_EXIT
        (bcmbd_m0ssq_init(unit));

    SHR_IF_ERR_EXIT
        (bcmbd_led_init(unit));

    SHR_IF_ERR_EXIT
        (bcmbd_i2c_init(unit));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_bd_dev_stop(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_CONT
        (bcmbd_i2c_cleanup(unit));

    SHR_IF_ERR_CONT
        (bcmbd_led_cleanup(unit));

    SHR_IF_ERR_CONT
        (bcmbd_m0ssq_cleanup(unit));

    SHR_IF_ERR_CONT
        (bcmbd_tsfifo_cleanup(unit));

    SHR_IF_ERR_CONT
        (bcmbd_ts_cleanup(unit));

    SHR_IF_ERR_CONT
        (bcmbd_cmicx2_ts_intr_stop(unit));

    SHR_IF_ERR_CONT
        (bcmbd_cmicx2_sw_intr_stop(unit));

    SHR_IF_ERR_CONT
        (bcmbd_cmicx2_lp_intr_stop(unit));

    SHR_IF_ERR_CONT
        (bcmbd_cmicx_intr_disconnect(unit));

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_pt_param_get(int unit, bcmdrd_sid_t sid,
                         bcmbd_pt_param_t ptype, uint32_t *pval)
{
    switch (ptype) {
    case BCMBD_PT_PARAM_MOR_CLKS_READ:
        *pval = bcm56990_b0_bd_mor_clks_read_get(unit, sid);
        return SHR_E_NONE;
    default:
        break;
    }
    return SHR_E_UNAVAIL;
}

static int
bcm56990_b0_otp_param_get(int unit, bcmbd_otp_param_t otp_param, uint32_t *val)
{
    switch (otp_param) {
    case BCMBD_OTP_PRM_AVS_VOLTAGE:
        return bcm56990_b0_bd_avs_voltage_get(unit, val);
    default:
        break;
    }
    return SHR_E_UNAVAIL;
}

static int
bcm56990_b0_drv_init(int unit)
{
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);

    if (bd == NULL) {
        return SHR_E_FAIL;
    }

    bd->dev_reset = bcm56990_b0_bd_dev_reset;
    bd->dev_init = bcm56990_b0_bd_dev_init;
    bd->dev_start = bcm56990_b0_bd_dev_start;
    bd->dev_stop = bcm56990_b0_bd_dev_stop;
    bd->knet_enable_set = bcmbd_cmicx_intr_knet_enable_set;
    bd->knet_enable_get = bcmbd_cmicx_intr_knet_enable_get;
    bd->addr_bypass = bcm56990_b0_addr_bypass;
    bd->pt_param_get = bcm56990_b0_pt_param_get;
    bd->otp_param_get = bcm56990_b0_otp_param_get;

    return SHR_E_NONE;
}

static int
bcm56990_b0_iproc_init(int unit, unsigned int iomem_idx, size_t size)
{
    bcmdrd_hal_iproc_cfg_t iproc_cfg, *icfg = &iproc_cfg;

    sal_memset(icfg, 0, sizeof(*icfg));

    /* iProc configuration for BCM56990 family. */
    icfg->iproc_ver = 17;

    bcmdrd_hal_iproc_init(unit, iomem_idx, size, icfg);

    return SHR_E_NONE;
}

/*
 * Configure device valid pipe bitmaps for restrict hardware resource access.
 * This feature is normally used for EMULation.
 */
static int
bcm56990_b0_valid_pbmps_set(int unit, uint32_t valid_pipes)
{
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);
    bcmdrd_pbmp_t *valid_pbmp, pbmp;
    bcmdrd_pipe_info_t pipe_info, *pi = &pipe_info;
    int port, pipe;
    bcmdrd_port_num_domain_t pnd;

    if (cinfo == NULL) {
        return SHR_E_FAIL;
    }

    sal_memset(pi, 0, sizeof(*pi));

    /* Update valid bitmaps in chip_info */
    for (pnd = 0; pnd < BCMDRD_PND_COUNT; pnd++) {
        valid_pbmp = (bcmdrd_pbmp_t *)(&cinfo->valid_pbmps[pnd]);
        BCMDRD_PBMP_CLEAR(pbmp);
        pi->pnd = pnd;
        BCMDRD_PBMP_ITER(*valid_pbmp, port) {
            /* Keep the management port. */
            if (pnd == BCMDRD_PND_PHYS && (port == 257 || port == 258)) {
                BCMDRD_PBMP_PORT_ADD(pbmp, port);
                continue;
            }
            pi->port = port;
            pipe = bcmdrd_chip_pipe_info(cinfo, pi,
                                         BCMDRD_PIPE_INFO_TYPE_PIPE_INDEX_FROM_PORT);
            if ((1 << pipe) & valid_pipes) {
                BCMDRD_PBMP_PORT_ADD(pbmp, port);
            }
        }
        /* Update the valid port bitmap */
        BCMDRD_PBMP_AND(*valid_pbmp, pbmp);

        /* Update the device valid ports */
        if (pnd == BCMDRD_PND_PHYS) {
            bcmdrd_dev_valid_ports_set(unit, valid_pbmp);
        }
    }

    bcmdrd_dev_valid_pipes_set(unit, valid_pipes);

    return SHR_E_NONE;
}

static int
bcm56990_b0_pipe_init(int unit)
{
    uint32_t disabled_pipes;
    uint32_t valid_pipes = 0xFFFF;
    uint32_t dev_flags;

    dev_flags = bcmdrd_dev_flags_get(unit);
    if (dev_flags & CHIP_FLAG_PIPE_F00F) {
        valid_pipes = 0xF00F;
    }

    /* DEVICE_CONFIG.CHIP_DEBUG indicates the pipe-disable bitmap */
    disabled_pipes = bcmbd_config_get(unit, 0, "DEVICE_CONFIG", "CHIP_DEBUG");
    valid_pipes &= ~disabled_pipes;
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "DEVICE_CONFIG.CHIP_DEBUG: 0x%08x.\n"),
                 disabled_pipes));

    return bcm56990_b0_valid_pbmps_set(unit, valid_pipes);
}

static int
bcm56990_b0_pt_index_valid(int unit, bcmdrd_sid_t sid,
                           int tbl_inst, int tbl_idx, bool *valid)
{
    uint32_t valid_pipes = 0;
    int rv;

    /*
     * The index valid check for AVS registers is only applied to
     * BCM56990 half chip mode or BCM56991.
     */
    rv = bcmdrd_dev_valid_pipes_get(unit, &valid_pipes);
    if (SHR_FAILURE(rv) || valid_pipes != 0xF00F) {
        return SHR_E_UNAVAIL;
    }

    if (valid == NULL) {
        return SHR_E_PARAM;
    }

    switch (sid) {
    case AVS_PMB_SLAVE_AVS_ROSC_LVT11_THRESHOLDr:
    case AVS_PMB_SLAVE_AVS_ROSC_LVT8_THRESHOLDr:
    case AVS_PMB_SLAVE_AVS_ROSC_SVT11_THRESHOLDr:
    case AVS_PMB_SLAVE_AVS_ROSC_SVT8_THRESHOLDr:
    case AVS_PMB_SLAVE_AVS_ROSC_ULVT11_THRESHOLDr:
    case AVS_PMB_SLAVE_AVS_ROSC_ULVT8_THRESHOLDr:
        if (tbl_inst > 0) {
            *valid = false;
        } else if (tbl_idx < 0) {
            /* Ignore the index check if negative */
            *valid = true;
        } else if (tbl_idx < bcmdrd_pt_index_min(unit, sid) ||
                   tbl_idx > bcmdrd_pt_index_max(unit, sid)) {
            *valid = false;
        } else {
            if ((tbl_idx >= 0 && tbl_idx <= 2) ||
                (tbl_idx >= 7 && tbl_idx <= 16) ||
                (tbl_idx >= 33 && tbl_idx <= 45) ||
                (tbl_idx >= 56 && tbl_idx <= 61) ||
                (tbl_idx >= 64 && tbl_idx <= 65) ||
                (tbl_idx >= 68 && tbl_idx <= 70) ||
                (tbl_idx >= 73 && tbl_idx <= 75) ||
                (tbl_idx >= 77 && tbl_idx <= 85 && (tbl_idx & 1)) ||
                (tbl_idx >= 87 && tbl_idx <= 92) ||
                (tbl_idx >= 97 && tbl_idx <= 98)) {
                *valid = true;
            } else {
                *valid = false;
            }
        }
        break;
    default:
        return SHR_E_UNAVAIL;
    }

    return SHR_E_NONE;
}

static int
bcm56990_b0_hc_schan_error(int unit, int ch, uint32_t *schan_msg, uint32_t reg_val)
{
    CMIC_SCHAN_CTRLr_t ctrl;
    int rv = SHR_E_NONE;

    CMIC_SCHAN_CTRLr_SET(ctrl, reg_val);

    if (CMIC_SCHAN_CTRLr_NACKf_GET(ctrl)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "S-channel NAK\n")));
        rv = SHR_E_FAIL;
    }

    if (CMIC_SCHAN_CTRLr_SER_CHECK_FAILf_GET(ctrl)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "S-channel SER error\n")));
        rv = SHR_E_FAIL;
    }

    if (CMIC_SCHAN_CTRLr_TIMEOUTf_GET(ctrl)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "S-channel TO error\n")));
        rv = SHR_E_FAIL;
    }

    return rv;
}

static int
bcm56990_b0_cmicx_schan_error_func_set(int unit)
{
    bool emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);
    uint32_t valid_pipes;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmdrd_dev_valid_pipes_get(unit, &valid_pipes));

    /*
     * Accessing MMU DUPLICATE register in emulator half-chip mode result in
     * ERROR_BIT being set, they need to be skipped in error checking.
     */
    if (emul && (valid_pipes == 0xF00F)) {
        SHR_IF_ERR_EXIT
            (bcmbd_cmicx_schan_error_func_set(unit, bcm56990_b0_hc_schan_error));
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
* Public functions
 */

int
bcm56990_b0_bd_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Enable symbolic debug messages */
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_sym_addr_enable(unit));

    SHR_IF_ERR_EXIT
        (bcmbd_cmicx2_feature_init(unit));
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_drv_init(unit));
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_schan_init(unit));

    SHR_IF_ERR_EXIT
        (bcm56990_b0_feature_init(unit));
    SHR_IF_ERR_EXIT
        (bcm56990_b0_drv_init(unit));
    SHR_IF_ERR_EXIT
        (bcmdrd_port_info_init(unit, bcm56990_b0_port_info));
    SHR_IF_ERR_EXIT
        (bcm56990_b0_pipe_init(unit));

    SHR_IF_ERR_EXIT
        (bcm56990_b0_cmicx_schan_error_func_set(unit));
    SHR_IF_ERR_EXIT
        (bcmdrd_dev_pt_index_valid_func_set(unit, bcm56990_b0_pt_index_valid));

    SHR_IF_ERR_EXIT
        (bcmdrd_hal_reg32_init(unit, 0, BCMBD_CMICX_IOMEM_SIZE));
    SHR_IF_ERR_EXIT
        (bcm56990_b0_iproc_init(unit, 1, 0));
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_hotswap_recover(unit));
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_intr_init(unit));
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx2_lp_intr_init(unit));

    SHR_IF_ERR_EXIT
        (bcmbd_cmicx2_sw_intr_drv_init(unit));
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx2_ts_intr_drv_init(unit));
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx2_tsfifo_drv_init(unit));
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_sbusdma_init(unit));
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_fifodma_init(unit));
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx2_ccmdma_init(unit));
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_schanfifo_init(unit));
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_i2c_drv_init(unit, 0));
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_qspi_drv_init(unit));
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_pcie_drv_init(unit));
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_miim_drv_init(unit));

    SHR_IF_ERR_EXIT
        (bcm56990_b0_bd_db_intr_drv_init(unit));
    SHR_IF_ERR_EXIT
        (bcm56990_b0_bd_ipep_intr_drv_init(unit));
    SHR_IF_ERR_EXIT
        (bcm56990_b0_bd_port_intr_drv_init(unit));
    SHR_IF_ERR_EXIT
        (bcm56990_b0_bd_mmu_intr_drv_init(unit));
    SHR_IF_ERR_EXIT
        (bcm56990_b0_bd_mmui_intr_drv_init(unit));
    SHR_IF_ERR_EXIT
        (bcm56990_b0_bd_pvt_intr_drv_init(unit));

    SHR_IF_ERR_EXIT
        (bcmbd_fwm_dev_create(unit));
    SHR_IF_ERR_EXIT
        (bcm56990_b0_bd_m0ssq_drv_init(unit));
    SHR_IF_ERR_EXIT
        (bcm56990_b0_bd_led_drv_init(unit));
    SHR_IF_ERR_EXIT
        (bcm56990_b0_bd_ts_drv_init(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_b0_bd_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_CONT
        (bcm56990_b0_bd_led_drv_cleanup(unit));
    SHR_IF_ERR_CONT
        (bcm56990_b0_bd_m0ssq_drv_cleanup(unit));
    SHR_IF_ERR_CONT
        (bcmbd_fwm_dev_destroy(unit));
    SHR_IF_ERR_CONT
        (bcm56990_b0_bd_ts_drv_cleanup(unit));
    SHR_IF_ERR_CONT
        (bcm56990_b0_bd_pvt_drv_cleanup(unit));

    SHR_IF_ERR_CONT
        (bcm56990_b0_bd_pvt_intr_drv_cleanup(unit));
    SHR_IF_ERR_CONT
        (bcm56990_b0_bd_mmu_intr_drv_cleanup(unit));
    SHR_IF_ERR_CONT
        (bcm56990_b0_bd_port_intr_drv_cleanup(unit));
    SHR_IF_ERR_CONT
        (bcm56990_b0_bd_mmui_intr_drv_cleanup(unit));
    SHR_IF_ERR_CONT
        (bcm56990_b0_bd_ipep_intr_drv_cleanup(unit));
    SHR_IF_ERR_CONT
        (bcm56990_b0_bd_db_intr_drv_cleanup(unit));

    SHR_IF_ERR_CONT
        (bcmbd_miim_cleanup(unit));
    SHR_IF_ERR_CONT
        (bcmbd_cmicx_i2c_drv_cleanup(unit, 0));
    SHR_IF_ERR_CONT
        (bcmbd_cmicx_schanfifo_cleanup(unit));
    SHR_IF_ERR_CONT
        (bcmbd_cmicx_ccmdma_cleanup(unit));
    SHR_IF_ERR_CONT
        (bcmbd_cmicx_fifodma_cleanup(unit));
    SHR_IF_ERR_CONT
        (bcmbd_cmicx_sbusdma_cleanup(unit));
    SHR_IF_ERR_CONT
        (bcmbd_cmicx2_tsfifo_drv_cleanup(unit));
    SHR_IF_ERR_CONT
        (bcmbd_cmicx2_ts_intr_drv_cleanup(unit));
    SHR_IF_ERR_CONT
        (bcmdrd_hal_iproc_cleanup(unit));
    SHR_IF_ERR_CONT
        (bcmdrd_hal_reg32_cleanup(unit));

    SHR_IF_ERR_CONT
        (bcmdrd_dev_pt_index_valid_func_set(unit, NULL));

    SHR_IF_ERR_CONT
        (bcmbd_cmicx_schan_cleanup(unit));
    SHR_IF_ERR_CONT
        (bcmbd_cmicx_drv_cleanup(unit));

    SHR_FUNC_EXIT();
}
