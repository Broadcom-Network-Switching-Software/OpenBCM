/*! \file bcm56780_a0_bd_attach.c
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

#include <sal/sal_libc.h>

#include <bcmbd/bcmbd_config.h>
#include <bcmbd/bcmbd_cmicx2.h>
#include <bcmbd/bcmbd_cmicx_intr.h>
#include <bcmbd/bcmbd_cmicx_hotswap.h>
#include <bcmbd/bcmbd_internal.h>
#include <bcmbd/bcmbd_i2c_internal.h>
#include <bcmbd/bcmbd_led_internal.h>
#include <bcmbd/bcmbd_i2c.h>
#include <bcmbd/bcmbd_cmicx_qspi.h>
#include <bcmbd/bcmbd_cmicx_pcie.h>
#include <bcmbd/bcmbd_cmicx_miim.h>
#include <bcmbd/bcmbd_cmicx_gpio.h>
#include <bcmbd/bcmbd_cmicx_gpio_intr.h>
#include <bcmbd/bcmbd_cmicx_schan_error.h>
#include <bcmbd/bcmbd_cmicx2_sw_intr.h>
#include <bcmbd/bcmbd_cmicx2_ts_intr.h>
#include <bcmbd/bcmbd_cmicx2_tsfifo.h>
#include <bcmbd/bcmbd_m0ssq_internal.h>
#include <bcmbd/bcmbd_ts_internal.h>
#include <bcmbd/bcmbd_tsfifo.h>
#include <bcmbd/bcmbd_fwm.h>
#include <bcmbd/bcmbd_pvt_intr.h>
#include <bcmbd/chip/bcm56780_a0_dev.h>

#include <bcmdrd/bcmdrd_hal.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmdrd/bcmdrd_tile.h>
#include <bcmdrd/bcmdrd_port.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/chip/bcm56780_a0_tile_hcf.h>
#include <bcmdrd/chip/bcm56780_a0_defs.h>

#include "bcm56780_a0_drv.h"

/* Log source for this component */
#define BSL_LOG_MODULE BSL_LS_BCMBD_DEV

/*******************************************************************************
* Local definitions
 */

/* Physical block numbers */
#define IPIPE 1
#define EPIPE 2

static bcmdrd_port_info_t bcm56780_a0_port_info[] = {
    /* Phys  Port type             Logic  MMU */
    {     0, BCMDRD_PORT_TYPE_CPU,     0,  19 },
    {   161, BCMDRD_PORT_TYPE_MGMT,   38,  51 },
    {   162, BCMDRD_PORT_TYPE_MGMT,   78, 115 },
    {   163, BCMDRD_PORT_TYPE_LB,     39,  50 },
    {   164, BCMDRD_PORT_TYPE_LB,     79, 114 },
    {   165, BCMDRD_PORT_TYPE_RDB,    19,  18 },
    {   166, BCMDRD_PORT_TYPE_RDB,    59,  82 },
    {    -1, BCMDRD_PORT_TYPE_RSVD,   58,  83 },
    {    -1  }
};

/*******************************************************************************
* Private functions
 */

static bool
bcm56780_a0_addr_bypass(int unit, uint32_t adext, uint32_t addr)
{
    int block;

    if (bcmdrd_dev_bypass_mode_get(unit) == BYPASS_MODE_EPIP) {
        block = (adext & 0xff);

        /* TD4-X9 Enable RDB block in bypass mode. */
        if ((block == EPIPE) &&
            ((adext == 0x2) || (adext == 0x102)) &&
            (((addr >= 0xC8000) && (addr <= 0xCDA00)) ||
             ((addr >= 0xCE400) && (addr <= 0xED400)) ||
             ((addr >= 0xED800) && (addr <= 0x10C800)))) {
            return false;
        }

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
bcm56780_a0_feature_init(int unit)
{
    int bypass_mode = bcmdrd_dev_bypass_mode_get(unit);

    bcmdrd_feature_enable(unit, BCMDRD_FT_IPROC);
    bcmdrd_feature_enable(unit, BCMDRD_FT_KNET);
    if (bypass_mode == BYPASS_MODE_EPIP ||
        bypass_mode == BYPASS_MODE_FLEXCODE) {
        bcmdrd_feature_disable(unit, BCMDRD_FT_FLEXCODE);
    } else {
        bcmdrd_feature_enable(unit, BCMDRD_FT_FLEXCODE);
    }

    return SHR_E_NONE;
}

static int
bcm56780_a0_bd_dev_start(int unit)
{
    bool emul;
    bool ts_en = false;

    SHR_FUNC_ENTER(unit);

    emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);

    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_intr_connect(unit));

    SHR_IF_ERR_EXIT
        (bcmbd_cmicx2_lp_intr_start(unit));

    SHR_IF_ERR_EXIT
        (bcmbd_cmicx2_sw_intr_start(unit));

    SHR_IF_ERR_EXIT
        (bcmbd_cmicx2_ts_intr_start(unit));

    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_gpio_intr_start(unit));

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

    if (!emul) {
        SHR_IF_ERR_EXIT
            (bcmbd_pvt_intr_start(unit, 0, (bcmbd_intr_f)NULL, 0));

        SHR_IF_ERR_EXIT
            (bcmbd_m0ssq_init(unit));

        SHR_IF_ERR_EXIT
            (bcmbd_led_init(unit));

        SHR_IF_ERR_EXIT
            (bcmbd_i2c_init(unit));

        SHR_IF_ERR_EXIT
            (bcmbd_gpio_init(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_bd_dev_stop(int unit)
{
    bool emul;
    SHR_FUNC_ENTER(unit);

    emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);

    if (!emul) {
        SHR_IF_ERR_CONT
            (bcmbd_gpio_cleanup(unit));

        SHR_IF_ERR_CONT
            (bcmbd_i2c_cleanup(unit));

        SHR_IF_ERR_CONT
            (bcmbd_led_cleanup(unit));

        SHR_IF_ERR_CONT
            (bcmbd_m0ssq_cleanup(unit));

        SHR_IF_ERR_EXIT
            (bcmbd_pvt_intr_stop(unit, 0));
    }

    SHR_IF_ERR_CONT
        (bcmbd_tsfifo_cleanup(unit));

    SHR_IF_ERR_CONT
        (bcmbd_ts_cleanup(unit));
    SHR_IF_ERR_CONT
        (bcmbd_cmicx_gpio_intr_stop(unit));

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
bcm56780_a0_pt_param_get(int unit, bcmdrd_sid_t sid,
                         bcmbd_pt_param_t ptype, uint32_t *pval)
{
    switch (ptype) {
    case BCMBD_PT_PARAM_MOR_CLKS_READ:
        *pval = bcm56780_a0_bd_mor_clks_read_get(unit, sid);
        return SHR_E_NONE;
    default:
        break;
    }
    return SHR_E_UNAVAIL;
}

static int
bcm56780_a0_otp_param_get(int unit, bcmbd_otp_param_t otp_param, uint32_t *val)
{
    switch (otp_param) {
    case BCMBD_OTP_PRM_AVS_VOLTAGE:
        return bcm56780_a0_bd_avs_voltage_get(unit, val);
    default:
        break;
    }
    return SHR_E_UNAVAIL;
}

static int
bcm56780_a0_drv_init(int unit)
{
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);

    if (bd == NULL) {
        return SHR_E_FAIL;
    }

    bd->dev_reset = bcm56780_a0_bd_dev_reset;
    bd->dev_init = bcm56780_a0_bd_dev_init;
    bd->dev_start = bcm56780_a0_bd_dev_start;
    bd->dev_stop = bcm56780_a0_bd_dev_stop;
    bd->knet_enable_set = bcmbd_cmicx_intr_knet_enable_set;
    bd->knet_enable_get = bcmbd_cmicx_intr_knet_enable_get;
    bd->addr_bypass = bcm56780_a0_addr_bypass;
    bd->pt_param_get = bcm56780_a0_pt_param_get;
    bd->otp_param_get = bcm56780_a0_otp_param_get;

    return SHR_E_NONE;
}

static int
bcm56780_a0_iproc_init(int unit, unsigned int iomem_idx, size_t size)
{
    bcmdrd_hal_iproc_cfg_t iproc_cfg, *icfg = &iproc_cfg;

    sal_memset(icfg, 0, sizeof(*icfg));

    /* iProc configuration for BCM56780 family. */
    icfg->iproc_ver = 15;

    bcmdrd_hal_iproc_init(unit, iomem_idx, size, icfg);

    return SHR_E_NONE;
}

static int
bcm56780_a0_emul_valid_pbmps_set(int unit, uint32_t valid_pipes)
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
            /* Keep the management ports. */
            if (pnd == BCMDRD_PND_PHYS && (port == 161 || port == 162)) {
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
bcm56780_a0_hc_subpipe_error(int unit, int ch, uint32_t *schan_msg, uint32_t reg_val)
{
    /* Dont want to check schan ERR in half chip */
    return SHR_E_NONE;
}

/*
 * NOTE: Below function needed for 5.6T SKUs (0x784 and 0x786)
static int
bcm_cflag_5_6_t_pt_index_valid(int unit, bcmdrd_sid_t sid,
                               int tbl_inst, int tbl_idx, bool *valid)
{
    int idx = -1;
    uint32_t check_bit;
    uint32_t tsc_disable = 0x80601;  Disabled ports: 0, 9, 10, 19.
    const char *sname;
    char n;

    if (valid == NULL) {
        return SHR_E_PARAM;
    }

    if (bcmdrd_pt_blktype_get(unit, sid, 0) != BLKTYPE_PORT_IF) {
        return SHR_E_UNAVAIL;
    }

    sname = bcmdrd_pt_sid_to_name(unit, sid);
    if (sname == NULL) {
        return SHR_E_PARAM;
    }

    if (sal_strncmp(sname, "IDB_", 4) == 0) {
        sname += 4;
        if (sal_strncmp(sname, "OBM", 3) == 0) {
            n = *(sname + 3);
            if (n >= '0' && n <= '4') {
                idx = n - '0';
            }
        } else if (sal_strncmp(sname, "CA", 2) == 0) {
            n = *(sname + 2);
            if (n >= '0' && n <= '4') {
                idx = n - '0';
            }
        }
    }

    if (idx < 0) {
        return SHR_E_UNAVAIL;
    }

    if (tbl_idx >= 0 && (tbl_idx < bcmdrd_pt_index_min(unit, sid) ||
                         tbl_idx > bcmdrd_pt_index_max(unit, sid))) {
        * Invalid index *
        *valid = false;
    } else if (tbl_inst < 0) {
        * Ignore instance *
        *valid = true;
    } else if (tbl_inst >= 8) {
        * Invalid instance *
        *valid = false;
    } else {
        *valid = true;
        check_bit = tbl_inst * 5 + idx;
        if ((tsc_disable & (1 << check_bit)) != 0) {
            *valid = false;
        }
    }
    return SHR_E_NONE;
}
*/

/*******************************************************************************
* Public functions
 */

int
bcm56780_a0_bd_attach(int unit)
{
    uint64_t chip_debug;
    uint32_t pipe_map;
    const bcmdrd_tile_mux_info_db_t **mux_info_list;

    SHR_FUNC_ENTER(unit);

    /* Enable symbolic debug messages */
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_sym_addr_enable(unit));

    SHR_IF_ERR_EXIT
        (bcmbd_cmicx2_feature_init(unit));
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_drv_init(unit));
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_drv_tbl_ops_disable(unit));
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_schan_init(unit));

    SHR_IF_ERR_EXIT
        (bcm56780_a0_feature_init(unit));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_drv_init(unit));

    SHR_IF_ERR_EXIT
        (bcmdrd_port_info_init(unit, bcm56780_a0_port_info));

    /* Update tile information. */
    mux_info_list = bcm56780_a0_mux_info_db_list_get();
    bcmdrd_tile_info_update(unit, mux_info_list);

    /* DEVICE_CONFIG.CHIP_DEBUG indicates the pipe-disable bitmap */
    chip_debug = bcmbd_config64_get(unit, 0,
                                    "DEVICE_CONFIG",
                                    "CHIP_DEBUG");
    if (chip_debug != 0) {
        SHR_IF_ERR_EXIT
            (bcmbd_cmicx_schan_error_func_set(unit, bcm56780_a0_hc_subpipe_error));
    }
    SHR_IF_ERR_EXIT
        (bcmdrd_dev_valid_pipes_get(unit, &pipe_map));
    pipe_map &= ~chip_debug;
    SHR_IF_ERR_EXIT
        (bcm56780_a0_emul_valid_pbmps_set(unit, pipe_map));

    SHR_IF_ERR_EXIT
        (bcmdrd_hal_reg32_init(unit, 0, BCMBD_CMICX_IOMEM_SIZE));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_iproc_init(unit, 1, 0));
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_hotswap_recover(unit));
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_intr_init(unit));
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx2_lp_intr_init(unit));
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_gpio_intr_drv_init(unit));

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
        (bcmbd_cmicx_i2c_drv_init(unit, 1));
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_qspi_drv_init(unit));
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_pcie_drv_init(unit));
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_miim_drv_init(unit));
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_gpio_drv_init(unit));

    SHR_IF_ERR_EXIT
        (bcm56780_a0_bd_port_intr_drv_init(unit));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_bd_ipep_intr_drv_init(unit));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_bd_mmu_intr_drv_init(unit));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_bd_pvt_intr_drv_init(unit));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_bd_sec_intr_drv_init(unit));

    SHR_IF_ERR_EXIT
        (bcmbd_fwm_dev_create(unit));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_bd_mcs_drv_init(unit));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_bd_m0ssq_drv_init(unit));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_bd_led_drv_init(unit));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_bd_ts_drv_init(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcm56780_a0_bd_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_CONT
        (bcm56780_a0_bd_led_drv_cleanup(unit));
    SHR_IF_ERR_CONT
        (bcm56780_a0_bd_m0ssq_drv_cleanup(unit));
    SHR_IF_ERR_CONT
        (bcm56780_a0_bd_mcs_drv_cleanup(unit));
    SHR_IF_ERR_CONT
        (bcmbd_fwm_dev_destroy(unit));
    SHR_IF_ERR_CONT
        (bcm56780_a0_bd_ts_drv_cleanup(unit));

    SHR_IF_ERR_CONT
        (bcm56780_a0_bd_pvt_intr_drv_cleanup(unit));
    SHR_IF_ERR_CONT
        (bcm56780_a0_bd_mmu_intr_drv_cleanup(unit));
    SHR_IF_ERR_CONT
        (bcm56780_a0_bd_ipep_intr_drv_cleanup(unit));
    SHR_IF_ERR_CONT
        (bcm56780_a0_bd_port_intr_drv_cleanup(unit));
    SHR_IF_ERR_CONT
        (bcm56780_a0_bd_sec_intr_drv_cleanup(unit));

    SHR_IF_ERR_CONT
        (bcmbd_miim_cleanup(unit));
    SHR_IF_ERR_CONT
        (bcmbd_cmicx_gpio_drv_cleanup(unit));
    SHR_IF_ERR_CONT
        (bcmbd_cmicx_i2c_drv_cleanup(unit, 1));

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
        (bcmbd_cmicx_gpio_intr_drv_cleanup(unit));

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

    /* Clear tile information. */
    bcmdrd_tile_info_update(unit, NULL);

    SHR_FUNC_EXIT();
}
