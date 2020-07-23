/*! \file bcm56780_a0_tm_init.c
 *
 * File containing device level and port level init sequence for bcm56780_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_timeout.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmbd/chip/bcm56780_a0_acc.h>
#include <bcmbd/chip/bcm56780_a0_ipep_intr.h>
#include <bcmbd/bcmbd_ipep_intr.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_port.h>
#include <bcmlrd/chip/bcm56780_a0/bcm56780_a0_lrd_enum_ctype.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmpc/bcmpc_types.h>
#include <bcmissu/issu_api.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bst/bcmtm_bst_internal.h>
#include <bcmtm/chip/bcm56780_a0_tm_thd_internal.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>
#include <bcmlrd/chip/bcm56780_a0/bcm56780_a0_lrd_enum_ctype.h>
#include <bcmlrd/bcmlrd_conf.h>
#include <bcmcth/bcmcth_tm_drv.h>
#include <bcmtm/bcmtm_device_info.h>
#include <bcmtm/chip/bcm56780_a0_tm_thd_internal.h>
#include <bcmtm/chip/bcm56780_a0_tm.h>
#include <bcmtm/bcmtm_ha_internal.h>
#include "bcm56780_a0_tm_idb_flexport.h"
#include "bcm56780_a0_tm_wred.h"
#include "bcm56780_a0_tm_ct.h"
#include "bcm56780_a0_tm_scheduler_shaper.h"
#include "bcm56780_a0_tm_imm_internal.h"
#include "bcm56780_a0_tm_ebst.h"
#include "bcm56780_a0_tm_obm.h"
#include "bcm56780_a0_tm_multicast.h"

/*******************************************************************************
 * Local definitions
 */
#define TD4_X9_CPU_LB_PORT_SPEED 10000
#define RDB_USEC_MAX 1000
#define RDB_EMUL_USEC_MAX (10000 * RDB_USEC_MAX)
#define RDB_MIN_POLLS 1
#define NUM_PP_PIPES  2


/*! CPU port configuration. */
static bcmtm_port_info_t
cpu_ports[TD4_X9_NUM_CPU_PORTS] = {
    /*! Logical Physical Pipe IDB TM. */
    { 0, 0, 0, 19, 19 }
};

/*! Loopback port configuration. */
static bcmtm_port_info_t
lb_ports[TD4_X9_NUM_LB_PORTS] = {
    /*! Logical Physical Pipe IDB TM. */
    { 39,  163, 1, 18, 50  },    /* loopback port 0 */
    { 79,  164, 3, 18, 114 },    /* loopback port 1 */
};

/* RDB port configuration. */
static bcmtm_port_info_t
rdb_ports[TD4_X9_NUM_RDB_PORTS] = {
    /*! Logical Physical Pipe IDB TM. */
    { 19,  165, 0, 18, 18  },    /* RDB port 0. */
    { 59,  166, 2, 18, 82  },    /* RDB port 1. */
};

/*! Management port configuration. */
static bcmtm_port_info_t
mgmt_ports[TD4_X9_NUM_MGMT_PORTS] = {
    /*! Logical Physical Pipe IDB TM. */
    { 38,  161, 1, 19, 51 },     /* management port 0 */
    { 78,  162, 3, 19, 115 },    /* management port 1 */
};

typedef struct bcmtm_status_chk_s {
    /*! Register name. */
    bcmdrd_sid_t ref_reg;
    /*! Done status field. */
    bcmdrd_fid_t done_field;
} bcmtm_status_chk_t;

#define BSL_LOG_MODULE  BSL_LS_BCMTM_CHIP

/*******************************************************************************
 * Private functions.
 */
/*!
 * \brief RDB system init.
 *
 * This enables RDB block for the system. This is the reset sequence for the
 * RDB port and the Intialization sequence performed.
 *
 * \param [in] unit Logical unit number.
 * \retval SHR_E_NONE No error.
 * \rtval SHR_E_IO Access errors.
 */
static int
bcm56780_a0_tm_rdb_init(int unit)
{
    int ioerr = 0;
    RDB_SBS_CONTROLr_t rdb_sbs_ctrl;
    RDB_ENABLEr_t rdb_enable;
    RDB_STATUSr_t rdb_status;
    RDB_DMA_NIH_0r_t rdb_nih0;
    RDB_DMA_NIH_1r_t rdb_nih1;
    shr_timeout_t to;
    int idx, rv = 0;
    uint32_t epipe_map = 0;
    bool emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);
    bool sim = bcmdrd_feature_is_sim(unit);
    uint32_t a[2] = {0x0, 0x00012800};
    uint32_t a1[2] = {0x0, 0x81000000};
    /* This enables sbs return for the epipe direclty to cmic. */
    ioerr+= READ_RDB_SBS_CONTROLr(unit, &rdb_sbs_ctrl);
    RDB_SBS_CONTROLr_BYPASS_RDBf_SET(rdb_sbs_ctrl, 0);
    ioerr+= WRITE_RDB_SBS_CONTROLr(unit, rdb_sbs_ctrl);

    /* Initialization seqeunce to initialize RDB memories. */
    rv = bcmdrd_dev_valid_blk_pipes_get(unit, 0, BLKTYPE_EPIPE, &epipe_map);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    for (idx = 0; idx < NUM_PP_PIPES; idx++) {
        if (epipe_map & (1 << idx)) {
            RDB_DMA_NIH_0r_NIHf_SET(rdb_nih0, a);
            ioerr += WRITE_RDB_DMA_NIH_0r(unit, idx, rdb_nih0);

            RDB_DMA_NIH_1r_NIHf_SET(rdb_nih1, a1);
            ioerr += WRITE_RDB_DMA_NIH_1r(unit, idx, rdb_nih1);
        }
    }

    for (idx = 0; idx < NUM_PP_PIPES; idx++) {
        if (epipe_map & (1 << idx)) {
            ioerr += READ_RDB_ENABLEr(unit, idx, &rdb_enable);
            RDB_ENABLEr_CTRL_INIT_0f_SET(rdb_enable, 1);
            RDB_ENABLEr_CTRL_INIT_1f_SET(rdb_enable, 1);
            ioerr += WRITE_RDB_ENABLEr(unit, idx, rdb_enable);
        }
    }
    /* Polling for RDB status register. */
    if (sim) {
        return SHR_E_NONE;
    }
    for (idx = 0; idx < NUM_PP_PIPES; idx++) {
        if (epipe_map & (1 << idx)) {
            if (emul) {
                shr_timeout_init(&to, RDB_EMUL_USEC_MAX, RDB_MIN_POLLS);
            } else {
                shr_timeout_init(&to, RDB_USEC_MAX, RDB_MIN_POLLS);
            }
            do {
                ioerr += READ_RDB_STATUSr(unit, idx, &rdb_status);
                if (shr_timeout_check(&to)) {
                    /* Time out. */
                    LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "RDB status time out for pipe: %0d\n"),
                                    idx));
                    break;
                }
                if ((RDB_STATUSr_CTRL_DONE_0f_GET(rdb_status) == 1) &&
                    (RDB_STATUSr_CTRL_DONE_1f_GET(rdb_status) == 1)) {
                    break;
                }
            } while (true);
        }
    }
    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

/*!
 * \brief  RDB port initialization.
 *
 * This populates the drv_info for the rdb ports. This sets up IDB configuration
 * for the RDB ports as well. This enables interrupts for the RDB.
 *
 * \param [in] unit Logical unit number.
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Error in interrups param configurations.
 */
static int
bcm56780_a0_tm_rdb_port_init(int unit)
{
    bcmtm_lport_t lport;
    bcmtm_pport_t pport;
    bcmtm_port_map_info_t *port_map;
    int idx;
    bcmdrd_sid_t sid;
    bcmtm_pt_info_t pt_info = {0};
    uint32_t epipe_map = 0, ltmbuf, fval;
    int chip_port[] = {18, 58};
    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    /* populate drv_info. */

    for (idx = 0; idx < COUNTOF(rdb_ports); idx++) {
        lport = rdb_ports[idx].log_port;
        pport = rdb_ports[idx].phy_port;
        port_map->lport_map[lport].pport = pport;
        port_map->pport_map[pport].lport = lport;
        port_map->lport_map[lport].mport = rdb_ports[idx].mmu_port;
        port_map->pport_map[pport].mport = rdb_ports[idx].mmu_port;
        port_map->lport_map[lport].pipe = rdb_ports[idx].pipe;
        port_map->pport_map[pport].mlocal_port = rdb_ports[idx].idb_port;
        port_map->lport_map[lport].mlocal_port = rdb_ports[idx].idb_port;
        port_map->lport_map[lport].mchip_port = chip_port[idx];
        port_map->lport_map[lport].base_ucq = 0;
        port_map->lport_map[lport].base_mcq = 0;
        port_map->lport_map[lport].num_ucq = port_map->lport_map[lport].num_mcq = 0;
        port_map->mport_map[rdb_ports[idx].mmu_port].lport = lport;
        /* Add the port to IDB */
        SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_port_idb_add(unit, pport));
        SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_port_mmu_add(unit, pport));
        /* Reset RDB ports. */
        SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_port_idb_up(unit, pport));
        SHR_IF_ERR_EXIT
                (bcm56780_a0_tm_imm_update(unit, pport, 1));
    }

    /* Enable RX and TX for RDB port. */
    sid = RDB_ENABLEr;
    SHR_IF_ERR_EXIT
        (bcmdrd_dev_valid_blk_pipes_get(unit, 0, BLKTYPE_EPIPE, &epipe_map));

    for (idx = 0; idx < TD4_X9_PP_PIPE_PER_DEV; idx++) {
        if (epipe_map & (1 << idx)) {
            BCMTM_PT_DYN_INFO(pt_info, 0, idx);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_read(unit, sid, -1, &pt_info, &ltmbuf));

            fval = 1;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, sid, RX_ENABLE_0f, &ltmbuf, &fval));
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, sid, RX_ENABLE_1f, &ltmbuf, &fval));
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, sid, TX_ENABLEf, &ltmbuf, &fval));
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, sid, -1, &pt_info, &ltmbuf));
            /* mapping interrupt function callback handler */
            SHR_IF_ERR_EXIT
                (bcmbd_ipep_intr_func_set(unit, EP_TO_CMIC_RDB,
                                          bcm56780_a0_rdb_intr_hdlr, idx));
        }
    }
    /* Clear the interrupts */
    SHR_IF_ERR_EXIT
        (bcmbd_ipep_intr_clear(unit, EP_TO_CMIC_RDB));

    /* Enable interrupts for RDB */
    SHR_IF_ERR_EXIT
        (bcmbd_ipep_intr_enable(unit, EP_TO_CMIC_RDB));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the clock frequency.
 * Reads the DEVICE_CONFIG.tbl.ltl to get the frequency from the config
 * table.
 *
 * \param [in] unit Logical unit number.
 * \param [in] freq_enum Freqeuency value from the enumerations.
 *
 * \return frequency value.
 */
static uint32_t
bcm56780_a0_freq_retrieve_from_enum(int unit, int freq_enum) {

    /*
     * This should be in sync with bcm56780_a0_lrd_enum_ctype.h
     * for enumeration definitions.
     */
    uint32_t frequency[] = {
                1350, /* BCM56780_A0_LRD_DEVICE_CLK_FREQ_T_T_CLK_1350MHZ */
                1500, /* BCM56780_A0_LRD_DEVICE_CLK_FREQ_T_T_CLK_1500MHZ */
                 950, /* BCM56780_A0_LRD_DEVICE_CLK_FREQ_T_T_CLK_950MHZ */
                1025, /* BCM56780_A0_LRD_DEVICE_CLK_FREQ_T_T_CLK_1025MHZ */
                1100, /* BCM56780_A0_LRD_DEVICE_CLK_FREQ_T_T_CLK_1100MHZ */
                1175, /* BCM56780_A0_LRD_DEVICE_CLK_FREQ_T_T_CLK_1175MHZ */
                1250, /* BCM56780_A0_LRD_DEVICE_CLK_FREQ_T_T_CLK_1250MHZ */
                };
    return frequency[freq_enum];
}

/*!
 * \brief Get the pipe number from physical port.
 *
 * \param [in] unit Logical unit number.
 * \param [in] lport Logical port number.
 *
 * \return Pipe number if success else returns -1 in case of failure.
 */
static int
bcm56780_a0_tm_lport_pipe_get(int unit, int lport)
{
    int pipe;

    if (lport < 0 || lport >= TD4_X9_DEV_PORTS_PER_DEV) {
        return -1;
    }
    pipe = lport / TD4_X9_DEV_PORTS_PER_PIPE;
    return pipe;
}

/*!
 * \brief Get the IDB port number.
 *
 * \param [in] unit Logical unit number.
 * \param [in] lport Logical port number.
 * \param [in] pipe Pipe number for the logical port.
 *
 * \return Return the IDB port number. Returns -1 in case of failure.
 */
static int
bcm56780_a0_tm_lport_idbport_get(int unit,
                                 int lport,
                                 int pipe)
{
    int pp, idb_port;

    if ((lport < 0) || (lport >= TD4_X9_DEV_PORTS_PER_DEV)) {
        return -1;
    }

    /* Fixed mappings */
    /* CPU ports */
    for (pp = 0; pp < COUNTOF(cpu_ports); pp++) {
        if (lport == cpu_ports[pp].log_port) {
            return cpu_ports[pp].idb_port;
        }
    }
    /* LB ports */
    for (pp = 0; pp < COUNTOF(lb_ports); pp++) {
        if (lport == lb_ports[pp].log_port) {
            return lb_ports[pp].idb_port;
        }
    }
    /* Mgmt ports */
    for (pp = 0; pp < COUNTOF(mgmt_ports); pp++) {
        if (lport == mgmt_ports[pp].log_port) {
            return mgmt_ports[pp].idb_port;
        }
    }
    if (pipe == 0) {
        idb_port = (lport - 1) % TD4_X9_DEV_PORTS_PER_PIPE;
    } else {
        idb_port = lport % TD4_X9_DEV_PORTS_PER_PIPE;
    }
    return idb_port;
}

/*!
 * \brief Get MMU port number from logical port.
 *
 * \param [in] unit Logical unit number.
 * \param [in] lport Logical port number.
 *
 * \return Returns mmu port number. Returns -1 in case of errors.
 */
static int
bcm56780_a0_tm_lport_mport_get(int unit, int lport)
{
    int pipe, idb_port, mport;

    pipe = bcm56780_a0_tm_lport_pipe_get(unit, lport);
    if (pipe < 0) {
        return -1;
    }
    idb_port = bcm56780_a0_tm_lport_idbport_get(unit, lport, pipe);
    if (idb_port < 0) {
        return -1;
    }
    mport = (pipe * TD4_X9_MMU_PORTS_PER_PIPE) + idb_port;
    return mport;
}

/*!
 * \brief Get unicast base queue number for the logical port.
 *
 * \param [in] unit Logical unit number.
 * \param [in] lport Logical port number.
 *
 * \return Returns unicast base queue number. Returns -1 in case of errors.
 */
static int
bcm56780_a0_tm_uc_cosq_base_get(int unit, int lport)
{
    int pipe, pipe_q_base[TD4_X9_PIPES_PER_DEV], pipe_idx;
    int local_mport;
    /* Number of queues per pipe is
     * pipe 0: 18 FP, 1 Spare, 1 CPU: 18*12 + 12 + 48 = 276
     * pipe 1,37: 18FP, 1 LB, 1 MGMT: 18*12 + 12 + 12 = 240
     * pipe 2: 18 FP: 18*12 = 216
     */
    int num_q_pipe[] = {276, 240, 216, 240};

    pipe_q_base[0] = 0;
    for (pipe_idx = 1; pipe_idx < TD4_X9_PIPES_PER_DEV; pipe_idx++) {
        pipe_q_base[pipe_idx] =
                   num_q_pipe[pipe_idx - 1] + pipe_q_base[pipe_idx - 1];
    }
    pipe = bcm56780_a0_tm_lport_pipe_get(unit, lport);
    if (pipe < 0) {
        return -1;
    }
    local_mport = bcm56780_a0_tm_lport_idbport_get(unit, lport, pipe);
    if (local_mport < 0) {
        return -1;
    }
    return (pipe_q_base[pipe] + local_mport * TD4_X9_NUM_QUEUES_NON_CPU_PORT);
}

/*!
 * \brief Get multicast base queue number for the logical port.
 *
 * \param [in] unit Logical unit number.
 * \param [in] lport Logical port number.
 *
 * \return Returns multicast base queue number. Returns -1 in case of errors.
 */
static int
bcm56780_a0_tm_mc_cosq_base_get(int unit, int lport)
{
    int num_mcq;
    int uc_q_base = bcm56780_a0_tm_uc_cosq_base_get(unit, lport);

    if (uc_q_base < 0) {
        return -1;
    }
    if (bcmtm_lport_is_cpu(unit, lport)) {
        /* MCQ mode is not applicable for CPU port. */
        return uc_q_base;
    }
    num_mcq = bcm56780_a0_tm_num_mc_q_non_cpu_port_get(unit);
    return (uc_q_base + TD4_X9_NUM_QUEUES_NON_CPU_PORT - num_mcq);
}

/*!
 * \brief Device related settings.
 *
 * \param [in] unit Logical unit number.
 */
static void
bcm56780_a0_tm_dev_info_set(int unit)
{
    struct bcmtm_device_info bcm56780_a0_tm_dev_info = {
        .max_pkt_sz   = TD4_X9_MMU_MAX_PACKET_BYTES,
        .pkt_hdr_sz   = TD4_X9_MMU_PACKET_HEADER_BYTES,
        .jumbo_pkt_sz = TD4_X9_MMU_JUMBO_FRAME_BYTES,
        .default_mtu  = TD4_X9_MMU_DEFAULT_MTU_BYTES,
        .cell_sz      = TD4_X9_MMU_BYTES_PER_CELL,
        .num_cells    = TD4_X9_MMU_TOTAL_CELLS,
        .num_port_pg  = TD4_X9_MMU_PRI_GROUP,
        .num_sp       = TD4_X9_MMU_SERVICE_POOL,
        .mcq_entry    = TD4_X9_MMU_MCQ_ENTRY,
        .num_q        = TD4_X9_NUM_QUEUES_NON_CPU_PORT,
        .num_cpu_q    = TD4_X9_NUM_QUEUES_CPU_PORT,
        .num_pipe     = TD4_X9_PIPES_PER_DEV,
        .num_nhop_sparse = TD4_X9_NHOPS_PER_REPL_INDEX_SPARSE_MODE,
        .num_nhop_dense = TD4_X9_NHOPS_PER_REPL_INDEX_DENSE_MODE,
        .max_num_repl = TD4_X9_MAX_MC_PKT_REPL,
        .num_buffer_pool  = TD4_X9_ITMS_PER_DEV,
    };
    bcmtm_device_info[unit] = bcm56780_a0_tm_dev_info;
}

/*
 * \brief Check if dpr frequency is in valid range.
 *
 * The check is performed based on the core clock freuency.
 *
 * \param [in] unit Device unit number.
 * \param [in] dpr_freq DPR frequency value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid DPR frequency.
 */
static int
bcm56780_a0_tm_dpr_freq_validate(int unit, int dpr_freq)
{
    int dpr_freq_max, dpr_freq_min;
    bcmtm_drv_info_t *drv_info;

    bcmtm_drv_info_get(unit, &drv_info);
    dpr_freq_max = drv_info->frequency;
    dpr_freq_min = (drv_info->frequency + 1) / 2;
    if (dpr_freq < dpr_freq_min || dpr_freq > dpr_freq_max) {
        LOG_ERROR(BSL_LS_BCMTM_INIT,
                  (BSL_META_U(unit,
                              "DPR clock frequency %dMHz is not supported. "
                              "Valid range %d-%d MHz\n"),
                   dpr_freq, dpr_freq_max, dpr_freq_min));
        return SHR_E_PARAM;
    }
    return SHR_E_NONE;
}

/*!
 * \brief TM driver_info populate function
 *
 * Write all available chip specific info to the TM drv_info structure.
 * The chip specific information will be retrieved during chip init and config.
 *
 * \param [in] unit Logical unit number.
 * \param [in] warm Warm boot.
 *
 * \retval SHR_E_NONE No error.
 * \retval Returns error code for error.
 */
static int
bcm56780_a0_tm_drv_info_populate(int unit, bool warm)
{
    uint64_t config_value;
    bcmtm_drv_info_t *drv_info;
    int pm_id;
    bcmltd_fid_t fid;
    bcmltd_sid_t ltid;
    uint64_t flex_port[TD4_X9_PBLKS_PER_DEV] = {0};
    size_t actual;

    SHR_FUNC_ENTER(unit);

    if (warm) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmtm_drv_info_get(unit, &drv_info));

    /* MMU lossless mode. default to 0(lossy)*/
    ltid = TM_THD_CONFIGt;
    fid = TM_THD_CONFIGt_THRESHOLD_MODEf;
    SHR_IF_ERR_EXIT
        (bcmcfg_field_get(unit, ltid, fid, &config_value));
    drv_info->lossless = config_value;

    /*! Device core clock freqeuncy. */
    ltid = DEVICE_CONFIGt;
    fid = DEVICE_CONFIGt_CORE_CLK_FREQf;
    SHR_IF_ERR_EXIT
        (bcmcfg_field_get(unit, ltid, fid, &config_value));
    drv_info->frequency =
            bcm56780_a0_freq_retrieve_from_enum(unit, config_value);

    /*! Packet processing clock freqeuncy. */
    fid = DEVICE_CONFIGt_PP_CLK_FREQf;
    SHR_IF_ERR_EXIT
        (bcmcfg_field_get(unit, ltid, fid, &config_value));
    drv_info->dpr_clock_frequency =
            bcm56780_a0_freq_retrieve_from_enum(unit, config_value);

    drv_info->fabric_port_enable = 0;
    drv_info->oobfc_clock_ns = 8;

    ltid = TM_PM_FLEX_CONFIGt;
    fid = TM_PM_FLEX_CONFIGt_MAX_SUB_PORTf;
    SHR_IF_ERR_EXIT
        (bcmcfg_field_array_get(unit, ltid, fid, 0, TD4_X9_PBLKS_PER_DEV,
                                flex_port, &actual));

    for (pm_id = 0; pm_id < TD4_X9_PBLKS_PER_DEV; pm_id++) {
        /* Maximum number of subports in the port block.*/
        drv_info->flex_eligible[pm_id] = flex_port[pm_id];
    }

    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_dpr_freq_validate(unit, drv_info->dpr_clock_frequency));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Enabled THDO port configuration.
 *
 * 1. Enable THDO for packet egress.
 * 2. Enable the source port drop count.
 * 3. Configures multicast counter overflow detection value.
 *
 * \param [in] unit Logical unit number.
 *
 * \retval SHR_E_NONE No error.
 * \retval Returns error code in case of error.
 */
static int
bcm56780_a0_tm_thdo_port_config(int unit)
{
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t fval, ltmbuf = 0;
    int pipe;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */

    SHR_FUNC_ENTER(unit);

    /* Enabling THDO for packet egress. */
    for (pipe = 0; pipe < TD4_X9_PIPES_PER_DEV; pipe++) {
        sid = MMU_THDO_CONFIG_PORTr;
        fid = OUTPUT_PORT_RX_ENABLEf;
        fval = 0xfffff; /* Enable all 20 ports in the pipe */
        ltmbuf = 0;

        BCMTM_PT_DYN_INFO(pt_info, pipe, 0);
        SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &ltmbuf));
    }

    /* Enable the source port drop count once THDO is enabled. */
    sid = MMU_THDO_ENGINE_ENABLES_CFGr;
    fid = SRC_PORT_DROP_COUNT_ENABLEf;
    fval = 1;
    BCMTM_PT_DYN_INFO(pt_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, &ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &ltmbuf));

    /* Configuration to detect multicast counter overflow. mmu_Setting v1.0 */
    sid = MMU_THDO_COUNTER_MC_OVERFLOW_SIZEr;
    fid = MC_OVERFLOW_SIZEf;
    fval = 0x45dd0;
    BCMTM_PT_DYN_INFO(pt_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure OQS ager limit.
 *
 * \param [in] unit Logical unit number.
 *
 * \retval SHR_E_NONE No error.
 * \retval Returns error code in case of error.
 */
static int
bcm56780_a0_tm_oqs_ager_limit_encode(int unit)
{
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_sid_t sid = MMU_OQS_AGER_LIMITr;
    bcmdrd_fid_t fid = AGER_LIMITf;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t fval, ltmbuf = 0;
    int encode = 0, index;
    /* Recommended values of ager_limit based on speed encoding. */
    uint32_t ager_limit[] = {4, 4, 3, 2};

    SHR_FUNC_ENTER(unit);
    /*!
     * Configure OQS ager limit based on port speed encoding.
     * OQS port_speed encoding - 0:<=50GE, 1:100GE, 2:200GE, 3:400GE
     */
    for ( ; encode < COUNTOF(ager_limit); encode++) {
        /*
         * Program same ager limit for 2 indices mapped to the same speed encode
         * indexed by {port_speed[1:0], HP_MMUQ[0]}
         */
        index = encode * 2;
        for ( ; index < ((encode + 1) * 2); index++) {
            ltmbuf = 0;
            fval = ager_limit[encode];
            BCMTM_PT_DYN_INFO(pt_info, index, 0);
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &ltmbuf));
        }
    }

    /* Ager limit config for CPU port. */
    index = 8;
    ltmbuf = 0;
    fval = 4; /* CPU ager limit. */
    BCMTM_PT_DYN_INFO(pt_info, index, 0);
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Checks for TM initialization.
 * 1. Checks for TOQs status register.
 * 2. Checks for WRED status register.
 * 3. CFP initialization status check.
 *
 * \param [in] unit Logical unit number.
 *
 * \retval SHR_E_NONE No error.
 * \retval Returns error code in case of error.
 */
static int
bcm56780_a0_tm_init_check (int unit)
{
    bcmtm_status_chk_t reg_global_list[] = {
        { MMU_TOQ_STATUSr, INIT_DONEf },
        { MMU_WRED_MEM_INIT_STATUSr, DONEf }};

    bcmtm_status_chk_t reg_pipe_list[] = {
        { MMU_EBCFP_INIT_DONEr, DONEf },
        { EBTOQ_STATUSr, INIT_DONEf }};

    int idx, pipe;
    uint32_t done, ltmbuf;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t pipe_map;

    SHR_FUNC_ENTER(unit);

    /*! Global register status check. */
    for (idx = 0; idx < COUNTOF(reg_global_list); idx++) {
        sid = reg_global_list[idx].ref_reg;
        fid = reg_global_list[idx].done_field;
        BCMTM_PT_DYN_INFO(pt_info, 0, 0);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, &ltmbuf));
        SHR_IF_ERR_EXIT
            (bcmtm_field_get(unit, sid, fid, &ltmbuf, &done));
        if (!done) {
            LOG_ERROR(BSL_LS_BCMTM_INIT,
                    (BSL_META_U(unit,
                                "MMU Init done not set after \n")));
            return SHR_E_INTERNAL;
        }
    }

    /*! Pipe based register status check. */
    SHR_IF_ERR_EXIT(bcmdrd_dev_valid_pipes_get(unit, &pipe_map));
    for (idx = 0; idx < COUNTOF(reg_pipe_list); idx++) {
        sid = reg_pipe_list[idx].ref_reg;
        fid = reg_pipe_list[idx].done_field;

        /*! Pipe validation for the chip. */
        for (pipe = 0; pipe < TD4_X9_PIPES_PER_DEV; pipe++) {
            if (pipe_map & (1 << pipe)) {
                BCMTM_PT_DYN_INFO(pt_info, 0, pipe);
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_read(unit, sid, ltid,
                                           &pt_info, &ltmbuf));
                SHR_IF_ERR_EXIT
                    (bcmtm_field_get(unit, sid, fid, &ltmbuf, &done));
                if (!done) {
                    LOG_ERROR(BSL_LS_BCMTM_INIT,
                            (BSL_META_U(unit,
                                        "MMU Init done not set after\n")));
                    return SHR_E_INTERNAL;
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Perform BCMTM memeory initialization.
 *
 * \param [in] unit Logical unit number.
 *
 * \retval SHR_E_NONE No error.
 * \retval Returns error code in case of error.
 */
static int
bcm56780_a0_tm_memory_init(int unit)
{
    bool emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);
    uint64_t parity_en = 0;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_sid_t sid = MMU_GLBCFG_MISCCONFIGr;
    bcmdrd_fid_t fid = ECCP_ENf;
    bcmltd_sid_t ltid;
    bcmltd_sid_t lfid;
    uint32_t fval;
    uint32_t ltmbuf = 0;
    int pipe;
    uint32_t pipe_map;

    SHR_FUNC_ENTER(unit);

    /* MMU memories are initialized in base driver (BCMBD) */
    if (emul) {
        SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_init_check(unit));
    }

    /* Program parity enable field based on the config LT. */
    ltid = SER_CONFIGt;
    lfid = SER_CONFIGt_SER_ENABLEf;
    SHR_IF_ERR_EXIT
        (bcmcfg_field_get(unit, ltid, lfid, &parity_en));
    fval = parity_en;
    BCMTM_PT_DYN_INFO(pt_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, &ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &ltmbuf));

    /* Write 1 to entry 0 of MMU_EBCFP_MXM_TAG_MEM after mem init */
    sid = MMU_EBCFP_MXM_TAG_MEMm;
    fid = DATAf;
    fval = 1;
    SHR_IF_ERR_EXIT(bcmdrd_dev_valid_pipes_get(unit, &pipe_map));
    for (pipe = 0; pipe < TD4_X9_PIPES_PER_DEV; pipe++) {
        if (pipe_map & (1 << pipe)) {
            BCMTM_PT_DYN_INFO(pt_info, 0, pipe);
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, sid, -1, &pt_info, &ltmbuf));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Cell pointer initialization.
 * This control CFAP Bank initialization.
 *
 * \param [in] unit Logical unit number.
 *
 * \retval SHR_E_NONE No error.
 * \retval Returns error code in case of error.
 */
static int
bcm56780_a0_tm_cell_pointer_init(int unit)
{
    bcmdrd_sid_t sid = MMU_CFAP_INIT_64r;
    uint32_t fval[2] = {0xffffffff, 0x3};
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf[2] = {0};
    bcmdrd_fid_t fid = START_CFAP_INITf;

    SHR_FUNC_ENTER(unit);

    BCMTM_PT_DYN_INFO(pt_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf, fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_cos_map_init(int unit)
{
    bcmdrd_sid_t sid = IPOST_QUEUE_ASSIGNMENT_COS_MAP_INDEX_OFFSET_MASK_0r;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf[2] = {0};
    uint32_t fval = 0x1ff;

    SHR_FUNC_ENTER(unit);

    /* IPOST_QUEUE_ASSIGNMENT_COS_MAP_INDEX_OFFSET_MASK_0r */
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, MASK_0f, ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, MASK_1f, ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, MASK_2f, ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_dyn_info, ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_intfi_enable(int unit)
{
    bcmdrd_sid_t sid = MMU_INTFI_ENABLEr;
    uint32_t fval;
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    bcmdrd_fid_t fid = ENABLEf;

    SHR_FUNC_ENTER(unit);

    fval = 1;
    BCMTM_PT_DYN_INFO(pt_info, 0, 0);
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &ltmbuf));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_info_table_init(int unit, bool warm)
{
    bcmcth_info_table_t data;
    bcmcth_tm_info_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    /* TM_Q_ASSIGNMENT_INFO table setup. */
    if (SHR_FAILURE(bcmtm_q_assignment_info_drv_get(unit, &drv))) {
        SHR_EXIT();
    }

    if (drv && drv->get_data) {
        SHR_IF_ERR_EXIT
            (drv->get_data(unit, &data));
        SHR_IF_ERR_EXIT
            (info_tbl_init(unit, &data, warm));
    }

    /* TM_COS_Q_CPU_MAP_INFO table setup */
    if (SHR_FAILURE(bcmtm_cos_q_cpu_map_info_drv_get(unit, &drv))) {
        SHR_EXIT();
    }

    if (drv && drv->get_data) {
        SHR_IF_ERR_EXIT
            (drv->get_data(unit, &data));
        SHR_IF_ERR_EXIT
            (info_tbl_init(unit, &data, warm));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_pfc_ddr_info_alloc(int unit, bool warm)
{
    bcmtm_pfc_ddr_info_t *pfc_ddr_info;
    uint32_t data_sz, alloc_sz;

    SHR_FUNC_ENTER(unit);

    data_sz = sizeof(bcmtm_pfc_ddr_info_t);
    alloc_sz = data_sz * TD4_X9_DEV_PORTS_PER_DEV;
    pfc_ddr_info = shr_ha_mem_alloc(unit,
                                    BCMMGMT_TM_COMP_ID,
                                    BCMTM_HA_DEV_PFC_DEADLOCK_INFO,
                                    "bcmtmPfcDeadlockInfo",
                                    &alloc_sz);
    SHR_NULL_CHECK(pfc_ddr_info, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT
        ((alloc_sz < (data_sz * TD4_X9_DEV_PORTS_PER_DEV)) ? SHR_E_MEMORY: SHR_E_NONE);
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "BCMTM: HA memory for pfc ddr port map %0d\n"),
            alloc_sz));
    if (!warm) {
        sal_memset(pfc_ddr_info, 0, alloc_sz);
        bcmissu_struct_info_report(unit, BCMMGMT_TM_COMP_ID,
                                   BCMTM_HA_DEV_PFC_DEADLOCK_INFO, 0,
                                   data_sz, TD4_X9_DEV_PORTS_PER_DEV,
                                   BCMTM_PFC_DDR_INFO_T_ID);
    }
    SHR_IF_ERR_EXIT(bcmtm_pfc_ddr_info_set(unit, pfc_ddr_info));
exit:
    SHR_FUNC_EXIT();

}

static int
bcm56780_a0_tm_port_map_alloc(int unit, bool warm)
{
    bcmtm_port_map_info_t port_map;
    uint32_t data_sz, alloc_sz;
    size_t num_inst;

    SHR_FUNC_ENTER(unit);

    data_sz = sizeof(bcmtm_lport_map_t);
    num_inst = TD4_X9_DEV_PORTS_PER_DEV;
    alloc_sz = data_sz * num_inst;
    port_map.lport_map = shr_ha_mem_alloc(unit,
                                          BCMMGMT_TM_COMP_ID,
                                          BCMTM_HA_LPORT_DEV_INFO,
                                          "bcmtmLportMap",
                                          &alloc_sz);
    SHR_NULL_CHECK(port_map.lport_map, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT
        ((alloc_sz < (data_sz * num_inst)) ? SHR_E_MEMORY : SHR_E_NONE);
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "BCMTM: HA memory for logical port map %0d\n"),
            alloc_sz));
    if (!warm) {
        /* memset the allocated memory */
        sal_memset(port_map.lport_map, -1, alloc_sz);
        bcmissu_struct_info_report(unit, BCMMGMT_TM_COMP_ID,
                                   BCMTM_HA_LPORT_DEV_INFO, 0,
                                   data_sz, num_inst,
                                   BCMTM_LPORT_MAP_T_ID);
    }

    data_sz = sizeof(bcmtm_pport_map_t);
    num_inst = TD4_X9_NUM_PHYS_PORTS;
    alloc_sz = data_sz * num_inst;
    port_map.pport_map = shr_ha_mem_alloc(unit,
                                          BCMMGMT_TM_COMP_ID,
                                          BCMTM_HA_PPORT_DEV_INFO,
                                          "bcmtmPportMap",
                                          &alloc_sz);
    SHR_NULL_CHECK(port_map.pport_map, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT
        ((alloc_sz < (data_sz * num_inst)) ? SHR_E_MEMORY : SHR_E_NONE);
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "BCMTM: HA memory for Physical port map %0d\n"),
            alloc_sz));
    if (!warm) {
        /* memset the allocated memory */
        sal_memset(port_map.pport_map, -1, alloc_sz);
        bcmissu_struct_info_report(unit, BCMMGMT_TM_COMP_ID,
                                   BCMTM_HA_PPORT_DEV_INFO, 0,
                                   data_sz, num_inst,
                                   BCMTM_PPORT_MAP_T_ID);
    }

    data_sz = sizeof(bcmtm_mport_map_t);
    num_inst = TD4_X9_MMU_PORTS_PER_PIPE * TD4_X9_PIPES_PER_DEV;
    alloc_sz = data_sz * num_inst;
    port_map.mport_map = shr_ha_mem_alloc(unit,
                                          BCMMGMT_TM_COMP_ID,
                                          BCMTM_HA_MPORT_DEV_INFO,
                                          "bcmtmMportMap",
                                          &alloc_sz);
    SHR_NULL_CHECK(port_map.pport_map, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT
        ((alloc_sz < (data_sz * num_inst)) ? SHR_E_MEMORY : SHR_E_NONE);
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "BCMTM: HA memory for MMU port map %0d\n"),
            alloc_sz));
    if (!warm) {
        /* memset the allocated memory */
        sal_memset(port_map.pport_map, -1, alloc_sz);
        bcmissu_struct_info_report(unit, BCMMGMT_TM_COMP_ID,
                                   BCMTM_HA_MPORT_DEV_INFO, 0,
                                   data_sz, num_inst,
                                   BCMTM_MPORT_MAP_T_ID);
    }

    SHR_IF_ERR_EXIT
        (bcmtm_port_map_set(unit, &port_map));
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
void
bcm56780_a0_tm_drv_info_portmap_fill(int unit,
                                     size_t num_ports,
                                     const bcmpc_mmu_port_cfg_t *old_cfg,
                                     const bcmpc_mmu_port_cfg_t *new_cfg)
{
    int pport, lport, mmu_port, idb_port;
    int chip_port, pipe;
    int idx;
    bcmtm_port_map_info_t *port_map;

    bcmpc_mmu_port_cfg_t port_cfg_data;
    bcmpc_mmu_port_cfg_t *port_cfg;

    bcmtm_port_map_get(unit, &port_map);

    /* Suppose that the config array is indexed by physical port number. */
    for (idx = 0; idx < (int)num_ports; idx++) {
        port_cfg_data = new_cfg[idx];
        port_cfg = &port_cfg_data;
        pport = port_cfg->pport;
        lport = port_cfg->lport;

        /* If the pport is mapped to an invalid lport, ignore it. */
        if (lport == BCMPC_INVALID_LPORT ||
            pport == BCMPC_INVALID_LPORT) {
            continue;
        }
        port_map->pport_map[pport].lport = lport;
        port_map->lport_map[lport].pport = pport;
        port_map->lport_map[lport].num_lanes = port_cfg->num_lanes;
        port_map->lport_map[lport].max_speed = port_cfg->speed_max;
        port_map->lport_map[lport].cur_speed = port_cfg->speed_cur;
        if (!bcmtm_lport_is_fp(unit, lport)) {
            if (port_map->lport_map[lport].cur_speed < TD4_X9_CPU_LB_PORT_SPEED) {
                /* set speed to minimum supported internal to TM */
                port_map->lport_map[lport].cur_speed = TD4_X9_CPU_LB_PORT_SPEED;
                port_map->lport_map[lport].max_speed = TD4_X9_CPU_LB_PORT_SPEED;
            }
        }

        pipe = bcm56780_a0_tm_lport_pipe_get(unit, lport);
        port_map->lport_map[lport].pipe = pipe;
        idb_port = bcm56780_a0_tm_lport_idbport_get(unit, lport, pipe);
        if (idb_port < 0 ) {
            continue;
        }
        port_map->lport_map[lport].mlocal_port = idb_port;
        port_map->pport_map[pport].mlocal_port = idb_port;

        mmu_port = bcm56780_a0_tm_lport_mport_get(unit, lport);
        port_map->pport_map[pport].mport = mmu_port;
        port_map->lport_map[lport].mport = mmu_port;
        port_map->mport_map[mmu_port].lport = lport;
        chip_port=
            (port_map->lport_map[lport].pipe * TD4_X9_DEV_PORTS_PER_PIPE) +
            port_map->lport_map[lport].mlocal_port;
        port_map->lport_map[lport].mchip_port = chip_port;

        if (bcmtm_lport_is_cpu(unit, lport)) {
            /* CPU port */
            port_map->lport_map[lport].num_mcq = 48;
            port_map->lport_map[lport].num_ucq = 0;
            port_map->lport_map[lport].max_speed = TD4_X9_CPU_LB_PORT_SPEED;
            port_map->lport_map[lport].cur_speed = TD4_X9_CPU_LB_PORT_SPEED;
        } else {
            /* Non CPU port. */
            port_map->lport_map[lport].num_mcq =
                bcm56780_a0_tm_num_mc_q_non_cpu_port_get(unit);
            port_map->lport_map[lport].num_ucq =
                bcm56780_a0_tm_num_uc_q_non_cpu_port_get(unit);
        }
        /* Overwrite loopback port speed (only support 10G). */
        if (bcmtm_lport_is_lb(unit, lport)) {
            port_map->lport_map[lport].max_speed = TD4_X9_CPU_LB_PORT_SPEED;
            port_map->lport_map[lport].cur_speed = TD4_X9_CPU_LB_PORT_SPEED;
        }
        if (bcmtm_lport_is_cpu(unit, lport)) {
            port_map->lport_map[lport].base_ucq = 0;
        } else {
        port_map->lport_map[lport].base_ucq =
                    bcm56780_a0_tm_uc_cosq_base_get(unit, lport);
        }
        port_map->lport_map[lport].base_mcq =
                    bcm56780_a0_tm_mc_cosq_base_get(unit, lport);
        port_map->lport_map[lport].flags = 0;

        if (port_cfg->flags & BCMPC_MMU_F_HG3) {
            port_map->lport_map[lport].flags |= BCMTM_PORT_IS_HG3;
        } else {
            port_map->lport_map[lport].flags &= (0xffffffff ^ BCMTM_PORT_IS_HG3);
        }
    }
}



int
bcm56780_a0_tm_mmu_q_from_chip_q_get(int unit, int chip_q,
                                     int *lport, uint32_t *mmu_q,
                                     bcmtm_q_type_t *q_type)
{
    int num_uc_q, num_mc_q, q_base, base = 0;
    bcmtm_port_map_info_t *port_map;
    int i;
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lport, SHR_E_PARAM);
    SHR_NULL_CHECK(mmu_q, SHR_E_PARAM);

    bcmtm_port_map_get(unit, &port_map);
    for (i = 0; i < MAX_NUM_PORTS; i++) {
        num_uc_q = port_map->lport_map[i].num_ucq;
        q_base = port_map->lport_map[i].base_ucq;
        if ((q_base >= 0) && (chip_q >= q_base) &&
            (chip_q < (q_base + num_uc_q))) {
            break;
        }
        num_mc_q = port_map->lport_map[i].num_mcq;
        q_base = port_map->lport_map[i].base_mcq;
        if ((q_base >= 0) && (chip_q >= q_base) &&
            (chip_q < (q_base + num_mc_q))) {
            base = num_uc_q;
            break;
        }
    }
    if (i == MAX_NUM_PORTS) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    *lport = i;
    *mmu_q = base + (chip_q - q_base);

    num_uc_q = port_map->lport_map[*lport].num_ucq;
    if ((num_uc_q > 0) && (*mmu_q >= (uint32_t)num_uc_q)) {
        *mmu_q -= num_uc_q;
        *q_type = 1; /* Multicast queues. */
    }

exit:
    SHR_FUNC_EXIT();
}

void
bcm56780_a0_rdb_intr_hdlr(int unit,
                          uint32_t intr_param)
{
    uint32_t intr_inst;
    shr_timeout_t to;
    RDB_FORCE_EOP_AND_STOPr_t rdb_status;
    RDB_SBS_CONTROLr_t rdb_ctrl;
    TOP_SOFT_RESET_REGr_t top_sreset;
    bool emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);
    int rdb_pport, idx;
    int ioerr = 0;

    intr_inst = IPEP_INTR_INST_GET(intr_param);

    /*
     * Check for RDB_FORCE_EOP_AND_STOP for intr_inst.
     * If RDB_FORCE_EOP_AND_STOP.TERMINATE = 1 uncorrectable error.
     */
    ioerr+= READ_RDB_FORCE_EOP_AND_STOPr(unit, intr_inst, &rdb_status);
    if (emul) {
        shr_timeout_init(&to, RDB_EMUL_USEC_MAX, RDB_MIN_POLLS);
    } else {
        shr_timeout_init(&to, RDB_USEC_MAX, RDB_MIN_POLLS);
    }

    if (RDB_FORCE_EOP_AND_STOPr_TERMINATEf_GET(rdb_status)) {
        /* Recover from the uncorrectable state. */
        do {
            ioerr+= READ_RDB_FORCE_EOP_AND_STOPr(unit, intr_inst, &rdb_status);
            /* Pool for RDB_FORCE_EOP_AND_STOP.SENT set to 1. */
            if (RDB_FORCE_EOP_AND_STOPr_SENTf_GET(rdb_status)) {
                break;
            }
            if (shr_timeout_check(&to)) {
                break;
            }
        } while (true);

        /* Disable SBUS transaction to RDB. */
        ioerr += READ_RDB_SBS_CONTROLr(unit, &rdb_ctrl);
        RDB_SBS_CONTROLr_BYPASS_RDBf_SET(rdb_ctrl, 1);
        ioerr += WRITE_RDB_SBS_CONTROLr(unit, rdb_ctrl);

        /* Reset RDB top register. */
        ioerr += READ_TOP_SOFT_RESET_REGr(unit, &top_sreset);
        TOP_SOFT_RESET_REGr_TOP_RDB_RST_Lf_SET(top_sreset, 0);
        ioerr += WRITE_TOP_SOFT_RESET_REGr(unit, top_sreset);

        /*
         * IDB buffer CA empty check and do IDB Buffer reset. This disables
         * IP from sending furture credits.
         */
        for (idx = 0; idx < COUNTOF(rdb_ports); idx++) {
            rdb_pport = rdb_ports[idx].phy_port;
            if (SHR_FAILURE(bcm56780_a0_tm_port_idb_down(unit, rdb_pport))) {
                LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "RDB: IDB CA port down sequence  failure\n")));
            }
        }
        /* Reinitialize the block. */
        ioerr += READ_TOP_SOFT_RESET_REGr(unit, &top_sreset);
        TOP_SOFT_RESET_REGr_TOP_RDB_RST_Lf_SET(top_sreset, 1);
        ioerr += WRITE_TOP_SOFT_RESET_REGr(unit, top_sreset);

        /* Initialize RDB. */
        ioerr += bcm56780_a0_tm_rdb_init(unit);

        ioerr += bcm56780_a0_tm_rdb_port_init(unit);
        if (ioerr) {
            LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "RDB: flush sequence fail.\n")));
        }
    }
}

/*!
 * \brief Get number of unicast queue for non-CPU port.
 *
 * \param [in] unit Logical unit number.
 *
 * \retval returns number of unicast queue.
 * \return SHR_E_INTERNAL Error.
 */
int
bcm56780_a0_tm_num_uc_q_non_cpu_port_get(int unit)
{
    int num_mcq;
    num_mcq = bcm56780_a0_tm_num_mc_q_non_cpu_port_get(unit);
    if (num_mcq < 0 ) {
        return SHR_E_INTERNAL;
    }
    return (TD4_X9_NUM_QUEUES_NON_CPU_PORT - num_mcq);
}

/*!
 * \brief Get number of multicast queue for non-CPU port.
 *
 * \param [in] unit Logical unit number.
 *
 * \retval returns number of multicast queue.
 * \return SHR_E_INTERNAL Error.
 */
int
bcm56780_a0_tm_num_mc_q_non_cpu_port_get(int unit)
{
    static int mc_q_mode_num_mc_queues[4] = {0, 2, 4, 6};
    bcmltd_sid_t ltid = TM_SCHEDULER_CONFIGt;
    bcmltd_fid_t fid = TM_SCHEDULER_CONFIGt_NUM_MC_Qf;
    uint64_t mc_q_mode;
    int rv;

    rv = bcmcfg_field_get(unit, ltid, fid, &mc_q_mode);
    if (SHR_SUCCESS(rv)) {
        return mc_q_mode_num_mc_queues[mc_q_mode];
    }
    return rv;
}

/*!
 * \brief IP port forwarding enable and disable.
 *
 * \param [in] unit Logical unit number.
 * \param [in] pport Physical port.
 * \param [in] enable True when forwarding is enabled and False when forwarding
 *  is disabled.
 *
 * \retval SHR_E_NONE No error.
 * \retval Error code in case of error.
 */
int
bcm56780_a0_tm_ip_port_forwarding_enable (int unit,
                                          bcmtm_pport_t pport)
{
    uint32_t ltmbuf[3];
    bcmdrd_sid_t sid = IPOST_MPB_CCBI_FIXED_ING_DEST_PORT_ENABLEm;
    bcmtm_port_map_info_t *port_map;
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1;
    int lport, enable = 1;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    lport = port_map->pport_map[pport].lport;

    BCMTM_PT_DYN_INFO(pt_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, ltmbuf));

    if (enable) {
        SHR_BITSET(ltmbuf, lport);
    } else {
        SHR_BITCLR(ltmbuf, lport);
    }
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

int
bcm56780_a0_tm_ip_port_forwarding_disable(int unit,
                                          bcmtm_pport_t pport)
{
    uint32_t ltmbuf[3];
    bcmdrd_sid_t sid = IPOST_MPB_CCBI_FIXED_ING_DEST_PORT_ENABLEm;
    bcmtm_port_map_info_t *port_map;
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1;
    int lport, enable = 0;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    lport = port_map->pport_map[pport].lport;

    BCMTM_PT_DYN_INFO(pt_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, ltmbuf));

    if (enable) {
        SHR_BITSET(ltmbuf, lport);
    } else {
        SHR_BITCLR(ltmbuf, lport);
    }
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));
exit:
    SHR_FUNC_EXIT();
}
/*!
 * \brief TM port validation.
 *
 * Validates the port configuration coming from PC.
 *
 * This function is bound to PC drv and will only be invoked by PC during init
 * or after port update.
 *
 * \param [in] unit Logical unit number.
 * \param [in] num_ports Number of ports.
 * \param [in] old_cfg Old port configurations.
 * \param [in] new_cfg New port configurations.
 *
 * \retval SHR_E_NONE No error.
 * \retval Return error code in case of error.
 */
int
bcm56780_a0_tm_validate (int unit,
                         size_t num_ports,
                         const bcmpc_mmu_port_cfg_t *old_cfg,
                         const bcmpc_mmu_port_cfg_t *new_cfg)
{
    int pport, lport;
    int pipe, pm;
    int num_fp_ports = 0, num_400g_ports = 0;
    int num_fp_ports_pipe[TD4_X9_PIPES_PER_DEV] = {0};
    bcmtm_drv_info_t *drv_info;
    uint32_t flex_port[TD4_X9_PBLKS_PER_DEV] = {0};

    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);
    for (pport = 0; pport < (int)num_ports; pport++) {
        lport = new_cfg[pport].lport;
        if (lport == BCMPC_INVALID_LPORT) {
            /*Invalid lport*/
            continue;
        }
        /* Auxillary port : Loopback port and CPU port. */
        if (bcmtm_lport_is_cpu(unit, lport) || (bcmtm_lport_is_lb(unit, lport))) {
            if ((new_cfg[pport].num_lanes != TD4_X9_NUM_LANES_AUX_PORT) ||
                (new_cfg[pport].speed_cur != TD4_X9_CPU_LB_PORT_SPEED)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            continue;
        }
        if (bcmtm_lport_is_mgmt(unit, lport) || bcmtm_lport_is_rdb(unit, lport)) {
            continue;
        }
        num_fp_ports++;
        pipe = bcm56780_a0_tm_lport_pipe_get(unit, lport);
        if (pipe >= 0) {
            num_fp_ports_pipe[pipe]++;
        }

        /* Get the port macro number from the port. */
        pm = ((pport - 1) >> 3);
        flex_port[pm]++;

        if (new_cfg[pport].speed_cur == 400000) {
            num_400g_ports++;
        }
    }

    if (num_fp_ports > TD4_X9_FP_PORTS_PER_DEV) {
        /* Check total number of front panel ports */
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    for (pipe = 0; pipe < TD4_X9_PIPES_PER_DEV; pipe++) {
        if (num_fp_ports_pipe[pipe] > TD4_X9_FP_PORTS_PER_PIPE) {
            /* Check total number of front panel ports per pipe */
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }
    for (pm = 0; pm < TD4_X9_PBLKS_PER_DEV; pm++) {
        if (flex_port[pm] > drv_info->flex_eligible[pm]) {
            /* port is port macros are more than number of port configured. */
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if (num_400g_ports != 0) {
        /* Check frequency */
        if (drv_info->frequency < 1325) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcm56780_a0_tm_chip_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    /* Generate drv_info which is equavalent to soc_info in SDK6 */
    SHR_IF_ERR_EXIT
        (bcmtm_drv_info_alloc(unit, warm));

    /* Port mapping for the device. */
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_port_map_alloc(unit, warm));

    /* drv_info populate. */
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_drv_info_populate(unit, warm));

    /* Allocate feature-specific SW states in HA. */
    SHR_IF_ERR_EXIT
        (bcmtm_thd_info_alloc(unit, warm));

    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_pfc_ddr_info_alloc(unit, warm));

    /* Multicast device info HA memory allocation. */
    SHR_IF_ERR_EXIT
        (bcmtm_mc_dev_info_alloc(unit, warm));

    bcm56780_a0_tm_dev_info_set(unit);

    /* Device level init sequence. Need to skip in the case of warm boot. */
    if (!warm) {
        SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_memory_init(unit));
        SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_cell_pointer_init(unit));
        SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_intfi_enable(unit));
        SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_thdo_port_config(unit));
        SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_oqs_ager_limit_encode(unit));
        SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_idb_init(unit));
        SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_cos_map_init(unit));
        SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_tdm_mmu_init(unit));
        SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_rqe_threshold_init(unit));
        SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_mmu_config_init_thresholds(unit));

        /* To be enabled after THDO. */
        SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_wred_enable(unit));


        /* Cut Through initialization */
        SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_ct_init(unit));
        /*
         * IMM populate
         * This should be called befiore rdb init.
         */
        SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_imm_populate(unit));
        /* Initialize the RDB settings. */
        SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_rdb_init(unit));
        /* RDB port init. This should be called once general IDB init is done.*/
        SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_rdb_port_init(unit));

    }
    /* scheduler init. */
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_scheduler_chip_init(unit, warm));
    /* Shaper init. */
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_shaper_chip_init(unit, warm));

    /* OBM threshold init. */
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_obm_init(unit, warm));

    /* WRED init. */
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_wred_chip_init(unit, warm));

    /* Buffer statistics tracking init. */
    SHR_IF_ERR_EXIT
        (bcmtm_bst_init(unit, warm));

    /* Event buffer statistics tracking init. */
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_ebst_init(unit, warm));

    /* Initialize info tables. */
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_info_table_init(unit, warm));
exit:
    SHR_FUNC_EXIT();
}

int
bcm56780_a0_tm_port_delete(int unit, bcmtm_pport_t pport)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_port_idb_delete(unit, pport));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_port_mmu_delete(unit, pport));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_port_edb_delete(unit, pport));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_imm_update(unit, pport, 0));
exit:
    SHR_FUNC_EXIT();
}

int
bcm56780_a0_tm_port_add(int unit, bcmtm_pport_t pport)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_port_idb_add(unit, pport));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_port_mmu_add(unit, pport));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_port_edb_add(unit, pport));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_imm_update(unit, pport, 1));
exit:
    SHR_FUNC_EXIT();
}

int
bcm56780_a0_tm_check_valid_itm(int unit, int buffer_pool)
{
    SHR_FUNC_ENTER(unit);

    /* only one valid ITM */
    if (buffer_pool >= TD4_X9_ITMS_PER_DEV || buffer_pool < 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcm56780_a0_tm_mchip_port_lport_get(int unit, int mchip_port, int *lport)
{
    int mlocal_port = 0, mport = 0;
    int pipe;

    SHR_FUNC_ENTER(unit);
    mlocal_port = mchip_port % TD4_X9_DEV_PORTS_PER_PIPE;
    pipe = mchip_port / TD4_X9_DEV_PORTS_PER_PIPE;

    mport = pipe * TD4_X9_MMU_PORTS_PER_PIPE + mlocal_port;
    SHR_IF_ERR_EXIT(bcmtm_mport_lport_get(unit, mport, lport));
exit:
    SHR_FUNC_EXIT();
}
