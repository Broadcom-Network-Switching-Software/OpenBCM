/*! \file bcm56990_a0_tm_init.c
 *
 * File containing device level and port level init sequence for bcm56990_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmbd/chip/bcm56990_a0_acc.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmpc/bcmpc_types.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmissu/issu_api.h>

#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_ha_internal.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmtm/bcmtm_device_info.h>
#include <bcmtm/bst/bcmtm_bst_internal.h>
#include <bcmtm/chip/bcm56990_a0_tm.h>
#include "bcm56990_a0_tm_ct.h"
#include "bcm56990_a0_tm_wred.h"
#include "bcm56990_a0_tm_obm.h"
#include "bcm56990_a0_tm_ebst.h"
#include "bcm56990_a0_tm_multicast_internal.h"
#include "bcm56990_a0_tm_scheduler_shaper.h"
#include <bcmtm/chip/bcm56990_a0_tm_thd_internal.h>
#include "bcm56990_a0_tm_idb_flexport.h"
#include "bcm56990_a0_tm_edb_flexport.h"
#include "bcm56990_a0_tm_mmu_tdm.h"
#include "bcm56990_a0_tm_mmu_flexport.h"
#include "bcm56990_a0_tm_pfc.h"
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>
#include <bcmlrd/chip/bcm56990_a0/bcm56990_a0_lrd_enum_ctype.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE  BSL_LS_BCMTM_CHIP

static bcmtm_port_info_t cpu_ports[TH4_NUM_CPU_PORTS] = {
    /* log_port, phy_port, pipe, idb_port, mmu_port */
    { 0, 0, 0, 16, 16 }
};

static bcmtm_port_info_t lb_ports[TH4_NUM_LB_PORTS] = {
    { 33,  259, 1,  16, 48 },     /* loopback port 0 */
    { 67,  260, 3,  16, 112 },    /* loopback port 1 */
    { 101, 261, 5,  16, 176 },    /* loopback port 2 */
    { 135, 262, 7,  16, 240 },    /* loopback port 3 */
    { 169, 263, 9,  16, 304 },    /* loopback port 4 */
    { 203, 264, 11, 16, 368 },    /* loopback port 5 */
    { 237, 265, 13, 16, 432 },    /* loopback port 6 */
    { 271, 266, 15, 16, 496 },    /* loopback port 7 */
};

static bcmtm_port_info_t mgmt_ports[TH4_NUM_MGMT_PORTS] = {
    { 152, 257, 8,  16, 272},     /* management port 0 */
    { 50,  258, 2,  16, 80 },     /* management port 1 */
};

#define TH4_MC_Q_MODE_DEFAULT 2
#define TH4_NUM_CPU_QUEUES 48
#define TH4_NUM_GP_QUEUES  12

#define BCMTM_TH4_AUX_PORT_SPEED        10000
#define BCMTM_TH4_10G_MGMT_PORT_SPEED   10000
#define BCMTM_TH4_40G_MGMT_PORT_SPEED   40000
#define CLK(mhz)      (BCM56990_A0_LRD_DEVICE_CLK_FREQ_T_T_CLK_##mhz)

/* From Device_CLK_FREQ_T.type.ltl. */
static
unsigned int th4_freq_retrieve_from_enum(int unit, int freq_enum) {
    uint32_t frequency;

    switch (freq_enum) {
        case CLK(1325MHZ):
            frequency = 1325;
            break;
        case CLK(950MHZ):
            frequency = 950;
            break;
        case CLK(1025MHZ):
            frequency = 1025;
            break;
        case CLK(1100MHZ):
            frequency = 1100;
            break;
        case CLK(1175MHZ):
            frequency = 1175;
            break;
        case CLK(1250MHZ):
            frequency = 1250;
            break;
        default:
            frequency = 1325;
            LOG_WARN(BSL_LS_BCMBD_DEV,
                     (BSL_META_U(unit,
                                 "Invalid frequency %d. Set with default value.\n"),
                      frequency));
            break;
    }
    return frequency;
}

/*******************************************************************************
* Private functions
 */
static int
port_pipe_get(int unit, int phys_port)
{
    int pp, pipe;

    if ((phys_port < 0) || (phys_port >= TH4_NUM_PHYS_PORTS)) {
        return -1;
    }

    /* Fixed mappings */
    /* CPU ports */
    for (pp = 0; pp < COUNTOF(cpu_ports); pp++) {
        if (phys_port == cpu_ports[pp].phy_port) {
            return cpu_ports[pp].pipe;
        }
    }
    /* LB ports */
    for (pp = 0; pp < COUNTOF(lb_ports); pp++) {
        if (phys_port == lb_ports[pp].phy_port) {
            return lb_ports[pp].pipe;
        }
    }
    /* Mgmt ports */
    for (pp = 0; pp < COUNTOF(mgmt_ports); pp++) {
        if (phys_port == mgmt_ports[pp].phy_port) {
            return mgmt_ports[pp].pipe;
        }
    }

   /* Regular ports */
   pipe = (phys_port - 1) / TH4_PHYS_PORTS_PER_PIPE;

   return pipe;
}

static int
port_idbport_get(int unit, int phys_port, int dev_port)
{
    int pp, idb_port;
    int lcl_phys_port, pipe_pm, subp;

    if ((dev_port < 0) || (dev_port >= TH4_DEV_PORTS_PER_DEV)) {
        return -1;
    }

    /* Fixed mappings */
    /* CPU ports */
    for (pp = 0; pp < COUNTOF(cpu_ports); pp++) {
        if (dev_port == cpu_ports[pp].log_port) {
            return cpu_ports[pp].idb_port;
        }
    }
    /* LB ports */
    for (pp = 0; pp < COUNTOF(lb_ports); pp++) {
        if (dev_port == lb_ports[pp].log_port) {
            return lb_ports[pp].idb_port;
        }
    }
    /* Mgmt ports */
    for (pp = 0; pp < COUNTOF(mgmt_ports); pp++) {
        if (dev_port == mgmt_ports[pp].log_port) {
            return mgmt_ports[pp].idb_port;
        }
    }

    /* Front panel ports */
    /* Assign idb/local_mmu_port in a round-robin fashion across PMs*/
    lcl_phys_port = ((phys_port - 1) % TH4_PHYS_PORTS_PER_PIPE);
    pipe_pm = lcl_phys_port >> 2;
    subp = lcl_phys_port % TH4_PORTS_PER_PBLK;

    idb_port = (subp * TH4_PORTS_PER_PBLK) + pipe_pm;

    return idb_port;
}



static int
port_mmuport_get(int unit, int phys_port, int log_port)
{
    int pipe, mmu_lcl_port, mmu_port;

    mmu_lcl_port = port_idbport_get(unit, phys_port, log_port);
    pipe = port_pipe_get(unit, phys_port);
    mmu_port = (pipe * TH4_MMU_PORTS_PER_PIPE) + mmu_lcl_port;
    return mmu_port;
}

static int
th4_uc_cosq_base_get(int unit, int pport, int lport)
{
    int pipe, pipe_queue_base[TH4_PIPES_PER_DEV], pipe_index;
    int local_mmu_port;
    /* Number of queues per pipe is
     * pipe 0: 16 FP, 1 CPU: 16*12 + 48 = 240
     * pipe 1,2,3,5,7,8,9,11,13,15: 16 FP, 1 LB/1 MGMT: 16*12 + 12 = 204
     * pipe 12,14: 16 FP, 12 unused queue: 16*12 + 12 = 204
     * pipe 4,6,10: 16 FP: 16*12 = 192
     */
    int num_queues_pipe[] = {240, 204, 204, 204, 192, 204, 192, 204,
                             204, 204, 192, 204, 204, 204, 204, 204};

    pipe_queue_base[0] = 0;
    for (pipe_index = 1; pipe_index < TH4_PIPES_PER_DEV; pipe_index++) {
        pipe_queue_base[pipe_index] = num_queues_pipe[pipe_index - 1] +
                                        pipe_queue_base[pipe_index - 1];
    }

    pipe = port_pipe_get(unit, pport);
    if (pipe < 0){
        return -1;
    }
    local_mmu_port = port_idbport_get(unit, pport, lport);
    return (pipe_queue_base[pipe] + local_mmu_port * TH4_NUM_GP_QUEUES);
}

static int
th4_mc_cosq_base_get(int unit, int pport, int lport)
{
    int uc_q_base, num_mcq;
    uc_q_base = th4_uc_cosq_base_get(unit, pport, lport);

    if (uc_q_base < 0) {
        return -1;
    }

    if (bcmtm_lport_is_cpu(unit, lport)) {
        /* MCQ mode is not applicable for CPU port. */
        return uc_q_base;
    }

    num_mcq = bcm56990_a0_tm_num_mc_q_non_cpu_port_get(unit);
    return (uc_q_base + TH4_NUM_GP_QUEUES - num_mcq);
}

static void
bcm56990_a0_bcmtm_dev_info_set(int unit)
{
    uint32_t dev_flags;
    struct bcmtm_device_info bcm56990_a0_tm_dev_info = {
        .max_pkt_sz   = TH4_MMU_MAX_PACKET_BYTES,
        .pkt_hdr_sz   = TH4_MMU_PACKET_HEADER_BYTES,
        .jumbo_pkt_sz = TH4_MMU_JUMBO_FRAME_BYTES,
        .default_mtu  = TH4_MMU_DEFAULT_MTU_BYTES,
        .cell_sz      = TH4_MMU_BYTES_PER_CELL,
        .num_cells    = TH4_MMU_TOTAL_CELLS_PER_ITM,
        .num_port_pg  = SOC_MMU_CFG_PRI_GROUP_MAX,
        .num_sp       = SOC_MMU_CFG_SERVICE_POOL_MAX,
        .mcq_entry    = SOC_TH4_MMU_MCQ_ENTRY_PER_ITM,
        .num_q        = TH4_NUM_GP_QUEUES,
        .num_cpu_q    = TH4_NUM_CPU_QUEUES,
        .num_pipe     = TH4_PIPES_PER_DEV,
        .num_nhop_sparse = TH4_NHOPS_PER_REPL_INDEX_SPARSE_MODE,
        .num_nhop_dense = TH4_NHOPS_PER_REPL_INDEX_DENSE_MODE,
        .max_num_repl = TH4_MAX_MC_PKT_REPL,
        .num_buffer_pool  = TH4_ITMS_PER_DEV,};
    bcmtm_device_info[unit] = bcm56990_a0_tm_dev_info;

    dev_flags = bcmdrd_dev_flags_get(unit);
    if (dev_flags & CHIP_FLAG_PIPE_F00F) {
        bcmtm_device_info[unit].num_buffer_pool = TH4_ITMS_PER_DEV / 2;
        bcmtm_device_info[unit].num_pipe = TH4_PIPES_PER_DEV / 2;
    }
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
th4_dpr_frequency_range_check(int unit, int dpr_freq)
{
    int dpr_freq_max, dpr_freq_min;
    bcmtm_drv_info_t *drv_info;

    bcmtm_drv_info_get(unit, &drv_info);

    dpr_freq_max = drv_info->frequency;
    dpr_freq_min = (drv_info->frequency + 1) / 2;
    if ((dpr_freq < dpr_freq_min) || (dpr_freq > dpr_freq_max) ||
            dpr_freq == -1) {
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
 * Write all available chip specific info to the TM port_map structure.
 *
 * The chip specific information will be retrieved during chip init and config.
 */
static int
th4_tm_port_map_populate(int unit, bool warm)
{
    uint64_t config_value;
    bcmtm_drv_info_t *drv_info;
    int pm_id;
    uint64_t flex_port[TH4_PBLKS_PER_DEV] = {0};
    size_t actual;

    SHR_FUNC_ENTER(unit);

    if (warm) {
        SHR_EXIT();
    }
    bcmtm_drv_info_get(unit, &drv_info);

    /* MMU lossless mode. default to 0(lossy)*/
    SHR_IF_ERR_EXIT
        (bcmcfg_field_get(unit, TM_THD_CONFIGt, TM_THD_CONFIGt_THRESHOLD_MODEf,
                          &config_value));
    drv_info->lossless = config_value;

    SHR_IF_ERR_EXIT
        (bcmcfg_field_get(unit, DEVICE_CONFIGt, DEVICE_CONFIGt_CORE_CLK_FREQf,
                          &config_value));
    drv_info->frequency = th4_freq_retrieve_from_enum(unit, config_value);
    SHR_IF_ERR_EXIT
        (bcmcfg_field_get(unit, DEVICE_CONFIGt, DEVICE_CONFIGt_PP_CLK_FREQf,
                          &config_value));
    drv_info->dpr_clock_frequency = th4_freq_retrieve_from_enum(unit, config_value);
    drv_info->fabric_port_enable = 0;
    drv_info->oobfc_clock_ns = 8;

    SHR_IF_ERR_EXIT
        (bcmcfg_field_array_get(unit, TM_PM_FLEX_CONFIGt,
                                TM_PM_FLEX_CONFIGt_MAX_SUB_PORTf,
                                0, TH4_PBLKS_PER_DEV, flex_port, &actual));
    for (pm_id = 0; pm_id < TH4_PBLKS_PER_DEV; pm_id++) {
        drv_info->flex_eligible[pm_id] = flex_port[pm_id];
    }

    SHR_IF_ERR_EXIT
        (th4_dpr_frequency_range_check(unit, drv_info->dpr_clock_frequency));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Function to fill port_sched_state_resource structure
 *
 * Retrieve all information contained in bcmpc_mmu_port_cfg_t and fill it into
 * the port_schedule_state field in tm_port_map. The bcmpc_mmu_port_cfg_t is
 * generated by BCMPC and contains all the necessary port config info.
 */
void
bcm56990_a0_tm_drv_info_portmap_fill(int unit,
                      size_t num_ports,
                      const bcmpc_mmu_port_cfg_t *old_cfg,
                      const bcmpc_mmu_port_cfg_t *new_cfg)
{
#define BCMTM_TH4_CPU_LB_PORT_SPEED 10000
    int pport, lport, mmu_port, idb_port;
    int chip_port;

    bcmtm_port_map_info_t *port_map;

    bcmpc_mmu_port_cfg_t port_cfg_data;
    bcmpc_mmu_port_cfg_t *port_cfg;
    int idx;

    bcmtm_port_map_get(unit, &port_map);

    /* Suppose that the config array is indexed by physical port number. */
    for (idx = 0; idx < (int)num_ports; idx++) {
        port_cfg_data = new_cfg[idx];
        port_cfg = &port_cfg_data;
        pport =  port_cfg->pport;

        /* If the pport is mapped to an invalid lport, ignore it. */
        if (port_cfg->lport == BCMPC_INVALID_LPORT ||
                      pport == BCMPC_INVALID_LPORT) {
            continue;
        }
        lport = port_cfg->lport;
        port_map->pport_map[pport].lport = lport;
        port_map->lport_map[lport].pport = pport;

        port_map->lport_map[lport].num_lanes = port_cfg->num_lanes;
        port_map->lport_map[lport].max_speed = port_cfg->speed_max;
        port_map->lport_map[lport].cur_speed = port_cfg->speed_cur;

        if (!bcmtm_lport_is_fp(unit, lport)) {
            if (port_map->lport_map[lport].cur_speed < BCMTM_TH4_CPU_LB_PORT_SPEED) {
                /* set speed to minimum supported internal to TM */
                port_map->lport_map[lport].cur_speed = BCMTM_TH4_CPU_LB_PORT_SPEED;
            }
        }

        port_map->lport_map[lport].pipe = port_pipe_get(unit, pport);
        idb_port = port_idbport_get(unit, pport, lport);
        if (idb_port < 0) {
            continue;
        }
        port_map->lport_map[lport].mlocal_port = idb_port;
        port_map->pport_map[pport].mlocal_port = idb_port;
        mmu_port = port_mmuport_get(unit, pport, lport);
        port_map->pport_map[pport].mport = mmu_port;
        port_map->lport_map[lport].mport = mmu_port;
        port_map->mport_map[mmu_port].lport = lport;
        port_map->pport_map[pport].mlocal_port = mmu_port & 0x1f;

        chip_port =
            (port_map->lport_map[lport].pipe * TH4_DEV_PORTS_PER_PIPE) +
            port_map->lport_map[lport].mlocal_port;
        port_map->lport_map[lport].mchip_port = chip_port;

        if (bcmtm_lport_is_cpu(unit, lport)) {
            /* CPU port */
            port_map->lport_map[lport].num_mcq = 48;
            port_map->lport_map[lport].num_ucq = 0;
            /* Update lport speed for CPU. */
            port_map->lport_map[lport].max_speed = BCMTM_TH4_CPU_LB_PORT_SPEED;
            port_map->lport_map[lport].cur_speed = BCMTM_TH4_CPU_LB_PORT_SPEED;
        } else {
            port_map->lport_map[lport].num_mcq =
                        bcm56990_a0_tm_num_mc_q_non_cpu_port_get(unit);
            port_map->lport_map[lport].num_ucq =
                        bcm56990_a0_tm_num_uc_q_non_cpu_port_get(unit);
        }

        /* Overwrite loopback port speed (only support 10G). */
        if (bcmtm_lport_is_lb(unit, lport)) {
            port_map->lport_map[lport].max_speed = BCMTM_TH4_CPU_LB_PORT_SPEED;
            port_map->lport_map[lport].cur_speed = BCMTM_TH4_CPU_LB_PORT_SPEED;
        }

        port_map->lport_map[lport].base_ucq = th4_uc_cosq_base_get(unit, pport,
                port_cfg->lport);
        port_map->lport_map[lport].base_mcq = th4_mc_cosq_base_get(unit, pport,
                port_cfg->lport);

        port_map->lport_map[lport].flags = 0;

        if (port_cfg->flags & BCMPC_MMU_F_HG2) {
            port_map->lport_map[lport].flags |= BCMTM_PORT_IS_HG2;
        }
        if (port_cfg->flags & BCMPC_MMU_F_MGMT) {
            port_map->lport_map[lport].flags |= BCMTM_PORT_IS_MGMT;
        }
        if (port_cfg->flags & BCMPC_MMU_F_OVSUB) {
            port_map->lport_map[lport].flags |= BCMTM_PORT_IS_OVSB;
        }

    }

#undef BCMTM_TH4_CPU_LB_PORT_SPEED
}

static int
th4_mmu_thdo_pipe_itm_config(int unit) {
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_sid_t sid ;
    bcmdrd_fid_t fid ;
    uint32_t fval ;
    int pipe, itm;
    uint32_t ltmbuf = 0;
    uint32_t valid_pipe_bmp;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmdrd_dev_valid_pipes_get(unit, &valid_pipe_bmp));
    for (pipe = 0; pipe < TH4_PIPES_PER_DEV; pipe++) {
        if ((1 << pipe) & valid_pipe_bmp) {
            sid = MMU_THDO_CONFIG_PORTr;
            fid = OUTPUT_PORT_RX_ENABLEf;
            fval = 0x1ffff; /* Enable all 17 ports in the pipe */
            ltmbuf = 0;

            BCMTM_PT_DYN_INFO(pt_dyn_info, pipe, 0);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_dyn_info, &ltmbuf));
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_dyn_info, &ltmbuf));
        }
    }

    sid = MMU_THDO_ENGINE_ENABLES_CFGr;
    for (itm = 0; itm < TH4_ITMS_PER_DEV; itm++) {
        if (bcm56990_a0_tm_check_valid_itm(unit, itm) == SHR_E_NONE) {
            ltmbuf = 0;
            fval = 1;
            BCMTM_PT_DYN_INFO(pt_dyn_info, 0, itm);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_dyn_info, &ltmbuf));
            fid = SRC_PORT_DROP_COUNT_ENABLEf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
            fid = DLB_INTERFACE_ENABLEf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_dyn_info, &ltmbuf));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
th4_mmu_rqe_queue_number_init(int unit)
{
    bcmtm_pt_info_t pt_dyn_info = {0};
    uint32_t ltmbuf;
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t fval;

    SHR_FUNC_ENTER(unit);

    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    fid = RQE_Q_NUMf;

    /*RQE queue number for CPU hi packets*/
    sid = CPU_HI_RQE_Q_NUMr;
    fval = TH4_CPU_HI_RQE_Q_NUM;
    ltmbuf = 0;
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_read(unit, sid, ltid,
                            (void*)&pt_dyn_info, &ltmbuf));
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid,(void*)&pt_dyn_info, &ltmbuf));

    /*RQE queue number for CPU lo packets*/
    sid = CPU_LO_RQE_Q_NUMr;
    fval = TH4_CPU_LO_RQE_Q_NUM;
    ltmbuf = 0;
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_read(unit, sid, ltid,
                            (void*)&pt_dyn_info, &ltmbuf));
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid,(void*)&pt_dyn_info, &ltmbuf));

    /*RQE queue number for Mirror packets*/
    sid = MIRROR_RQE_Q_NUMr;
    fval = TH4_MIRR_RQE_Q_NUM;
    ltmbuf = 0;
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_read(unit, sid, ltid,
                            (void*)&pt_dyn_info, &ltmbuf));
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid,(void*)&pt_dyn_info, &ltmbuf));

exit:
    SHR_FUNC_EXIT();
}

static int
th4_check_mmu_mem_init_done(int unit)
{
    struct {
        bcmdrd_sid_t ref_reg;
        bcmdrd_fid_t done_field;
    } itm_rf_list[] = {
        {MMU_TOQ_STATUSr, INIT_DONEf},
        {MMU_WRED_MEM_INIT_STATUSr, DONEf},
    };
    struct {
        bcmdrd_sid_t ref_reg;
        bcmdrd_fid_t done_field;
    } pipe_rf_list[] = {
        {MMU_EBCFP_INIT_DONEr, DONEf},
        {EBTOQ_STATUSr, INIT_DONEf},
    };
    int idx, itm, pipe;
    uint32_t done;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t pipe_map;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < COUNTOF(itm_rf_list); idx++) {
        sid = itm_rf_list[idx].ref_reg;
        fid = itm_rf_list[idx].done_field;
        for (itm = 0; itm < TH4_ITMS_PER_DEV; itm++) {
            if (bcm56990_a0_tm_check_valid_itm(unit, itm) == SHR_E_NONE) {
                BCMTM_PT_DYN_INFO(pt_dyn_info, 0, itm);
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_read(unit, sid, ltid,
                                (void*)&pt_dyn_info, &ltmbuf));
                SHR_IF_ERR_EXIT
                    (bcmtm_field_get(unit, sid, fid, &ltmbuf, &done));
                if (!done) {
                    LOG_ERROR(BSL_LS_BCMTM_INIT,
                              (BSL_META_U(unit,
                                          "MMU Memory Init done not set after "
                                          "toggling\n")));
                    return SHR_E_INTERNAL;
                }
            }
        }
    }

    SHR_IF_ERR_EXIT(bcmdrd_dev_valid_pipes_get(unit, &pipe_map));
    for (idx = 0; idx < COUNTOF(pipe_rf_list); idx++) {
        sid = pipe_rf_list[idx].ref_reg;
        fid = pipe_rf_list[idx].done_field;
        for (pipe = 0; pipe < TH4_PIPES_PER_DEV; pipe++) {
            if (pipe_map & (1 << pipe)) {
                BCMTM_PT_DYN_INFO(pt_dyn_info, 0, pipe);
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_read(unit, sid, ltid,
                                (void*)&pt_dyn_info, &ltmbuf));
                SHR_IF_ERR_EXIT
                    (bcmtm_field_get(unit, sid, fid, &ltmbuf, &done));
                if (!done) {
                    LOG_ERROR(BSL_LS_BCMTM_INIT,
                              (BSL_META_U(unit,
                                          "MMU Memory Init done not set after "
                                          "toggling\n")));
                    return SHR_E_INTERNAL;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
th4_init_tm_memory(int unit)
{
    bool emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);
    uint64_t parity_en = 0;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_sid_t sid = MMU_GLBCFG_MISCCONFIGr;
    bcmdrd_fid_t fid = ECCP_ENf;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t fval;
    uint32_t ltmbuf = 0;
    int pipe;
    uint32_t pipe_map;


    SHR_FUNC_ENTER(unit);

    /* MMU memories are initialized in base driver (BCMBD) */

    if (emul) {
        SHR_IF_ERR_EXIT(th4_check_mmu_mem_init_done(unit));
    }

    /* Program parity enable field based on the config LT. */
    SHR_IF_ERR_EXIT
        (bcmcfg_field_get(unit, SER_CONFIGt, SER_CONFIGt_SER_ENABLEf, &parity_en));
    fval = parity_en;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_dyn_info, &ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_dyn_info, &ltmbuf));

    /* SDKLT-9648 Write 1 to entry 0 of MMU_EBCFP_MXM_TAG_MEM after mem init */
    sid = MMU_EBCFP_MXM_TAG_MEMm;
    fid = DATAf;
    fval = 1;
    SHR_IF_ERR_EXIT(bcmdrd_dev_valid_pipes_get(unit, &pipe_map));
    for (pipe = 0; pipe < TH4_PIPES_PER_DEV; pipe++) {
        if (pipe_map & (1 << pipe)) {
            BCMTM_PT_DYN_INFO(pt_dyn_info, 0, pipe);
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_dyn_info, &ltmbuf));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
th4_en_mmu_refresh(int unit)
{
    int sid_index;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_sid_t sid ;
    bcmdrd_sid_t sid_list[2] = {MMU_MTRO_CONFIGr, MMU_WRED_REFRESH_CONTROLr};
    bcmdrd_fid_t fid = REFRESH_DISABLEf;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t fval ;
    uint32_t ltmbuf = 0;

    SHR_FUNC_ENTER(unit);

    for (sid_index = 0; sid_index < COUNTOF(sid_list); sid_index++) {
        sid = sid_list[sid_index];
        fval = 0;
        BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
        SHR_IF_ERR_EXIT(bcmtm_pt_indexed_read(unit, sid, ltid,
                            (void*)&pt_dyn_info, &ltmbuf));
        SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, sid, ltid,(void*)&pt_dyn_info,
                &ltmbuf));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
th4_cell_pointer_init(int unit)
{
    int itm;
    bcmdrd_sid_t sid = MMU_CFAP_INIT_64r;
    uint32_t fval[2] = {0xffffffff, 0x3};
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf[2] = {0};
    bcmdrd_fid_t fid = START_CFAP_INITf;


    SHR_FUNC_ENTER(unit);

    for (itm = 0; itm < TH4_ITMS_PER_DEV; itm++) {
        if (bcm56990_a0_tm_check_valid_itm(unit, itm) == SHR_E_NONE) {
            BCMTM_PT_DYN_INFO(pt_dyn_info, 0, itm);
            SHR_IF_ERR_EXIT(bcmtm_pt_indexed_read(unit, sid, ltid,
                                (void*)&pt_dyn_info, ltmbuf));
            SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, fval));
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, sid, ltid,
                        (void*)&pt_dyn_info, ltmbuf));

        }
    }

exit:
    SHR_FUNC_EXIT();
}


static int
th4_mmu_intfi_enable(int unit)
{
    bcmdrd_sid_t sid = MMU_INTFI_ENABLEr;
    uint32_t fval;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    bcmdrd_fid_t fid = ENABLEf;

    SHR_FUNC_ENTER(unit);

    fval = 1;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, (void*)&pt_dyn_info, &ltmbuf));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_tm_pfc_ddr_info_alloc(int unit, bool warm)
{
    bcmtm_pfc_ddr_info_t *pfc_ddr_info;
    uint32_t data_sz, alloc_sz;

    SHR_FUNC_ENTER(unit);

    data_sz = sizeof(bcmtm_pfc_ddr_info_t);
    alloc_sz = data_sz * TH4_DEV_PORTS_PER_DEV;
    pfc_ddr_info = shr_ha_mem_alloc(unit,
                                    BCMMGMT_TM_COMP_ID,
                                    BCMTM_HA_DEV_PFC_DEADLOCK_INFO,
                                    "bcmtmPfcDeadlockInfo",
                                    &alloc_sz);
    SHR_NULL_CHECK(pfc_ddr_info, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT
        ((alloc_sz < (data_sz * TH4_DEV_PORTS_PER_DEV)) ? SHR_E_MEMORY: SHR_E_NONE);
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "BCMTM: HA memory for pfc ddr port map %0d\n"),
            alloc_sz));
    if (!warm) {
        sal_memset(pfc_ddr_info, 0, alloc_sz);
        bcmissu_struct_info_report(unit, BCMMGMT_TM_COMP_ID,
                                   BCMTM_HA_DEV_PFC_DEADLOCK_INFO, 0,
                                   data_sz, TH4_DEV_PORTS_PER_DEV,
                                   BCMTM_PFC_DDR_INFO_T_ID);
    }
    SHR_IF_ERR_EXIT(bcmtm_pfc_ddr_info_set(unit, pfc_ddr_info));
exit:
    SHR_FUNC_EXIT();
}


static int
bcm56990_a0_tm_port_map_alloc(int unit, bool warm)
{
    bcmtm_port_map_info_t port_map;
    uint32_t data_sz, alloc_sz;
    size_t num_inst;

    SHR_FUNC_ENTER(unit);

    data_sz = sizeof(bcmtm_lport_map_t);
    num_inst = TH4_DEV_PORTS_PER_DEV;
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
                                   data_sz, num_inst, BCMTM_LPORT_MAP_T_ID);
    }

    data_sz = sizeof(bcmtm_pport_map_t);
    num_inst = TH4_NUM_PHYS_PORTS;
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
                                   data_sz, num_inst, BCMTM_PPORT_MAP_T_ID);
    }

    data_sz = sizeof(bcmtm_mport_map_t);
    num_inst = TH4_MMU_PORTS_PER_PIPE * TH4_PIPES_PER_DEV;
    alloc_sz = data_sz * num_inst;
    port_map.mport_map = shr_ha_mem_alloc(unit,
                                          BCMMGMT_TM_COMP_ID,
                                          BCMTM_HA_MPORT_DEV_INFO,
                                          "bcmtmMportMap",
                                          &alloc_sz);
    SHR_NULL_CHECK(port_map.pport_map, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT
        ((alloc_sz < (data_sz * num_inst))? SHR_E_MEMORY : SHR_E_NONE);
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "BCMTM: HA memory for MMU port map %0d\n"),
            alloc_sz));
    if (!warm) {
        /* memset the allocated memory */
        sal_memset(port_map.pport_map, -1, alloc_sz);
        bcmissu_struct_info_report(unit, BCMMGMT_TM_COMP_ID,
                                   BCMTM_HA_MPORT_DEV_INFO, 0,
                                   data_sz, num_inst, BCMTM_MPORT_MAP_T_ID);
    }

    SHR_IF_ERR_EXIT
        (bcmtm_port_map_set(unit, &port_map));
exit:
    SHR_FUNC_EXIT();

}

/*******************************************************************************
 * Public functions
 */
int
bcm56990_a0_tm_chip_q_get(int unit, uint32_t lport,
                          uint32_t mmu_q, uint32_t *chip_q)
{
    uint32_t num_uc_q, q_base;
    bcmtm_port_map_info_t *port_map;
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(chip_q, SHR_E_PARAM);

    bcmtm_port_map_get(unit, &port_map);
    q_base = port_map->lport_map[lport].base_ucq;
    num_uc_q = port_map->lport_map[lport].num_ucq;
    if (mmu_q >= num_uc_q) {
        mmu_q -= num_uc_q;
        q_base = port_map->lport_map[lport].base_mcq;
    }
    if (chip_q) {
        *chip_q = q_base + mmu_q;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_a0_tm_mmu_q_from_chip_q_get(int unit, int chip_q,
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
    *q_type = UC_Q;
    num_uc_q = port_map->lport_map[*lport].num_ucq;
    if ((num_uc_q > 0) && (*mmu_q >= (uint32_t)num_uc_q)) {
        *mmu_q -=  num_uc_q;
        *q_type = MC_Q; /* Multicast queues. */
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_a0_tm_check_valid_itm(int unit, int itm)
{
    int rv;
    uint32_t itm_map;

    SHR_FUNC_ENTER(unit);

    if ((itm >= TH4_NUM_ITM) || (itm < 0)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Get MMU_ITM pipe mask. */
    rv = bcmdrd_dev_valid_blk_pipes_get(unit, 0, BLKTYPE_MMU_ITM, &itm_map);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    if (!(itm_map & (1 << itm))) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_a0_tm_num_uc_q_non_cpu_port_get(int unit)
{
    int num_mcq;
    num_mcq = bcm56990_a0_tm_num_mc_q_non_cpu_port_get(unit);
    return (TH4_NUM_GP_QUEUES - num_mcq);
}

int
bcm56990_a0_tm_num_mc_q_non_cpu_port_get(int unit)
{
#define TH4_MC_Q_MODE_DEFAULT 2
    static int mc_q_mode_num_mc_queues[4] = {0, 2, 4, 6};
    bcmltd_sid_t ltid = TM_SCHEDULER_CONFIGt;
    bcmltd_fid_t fid = TM_SCHEDULER_CONFIGt_NUM_MC_Qf;
    uint64_t mc_q_mode;
    (void) (bcmcfg_field_get(unit, ltid, fid, &mc_q_mode));
    return mc_q_mode_num_mc_queues[mc_q_mode];
}

int
bcm56990_a0_ip_port_forwarding_bitmap_enable(int unit, bcmpc_pport_t pport) {
    uint32_t bmp[9] = {0};
    int lport;
    bcmdrd_sid_t sid;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf[BCMTM_MAX_ENTRY_WSIZE] = {0};
    bcmtm_pt_info_t pt_dyn_info = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_pport_lport_get(unit, pport, &lport));

    sid = ING_DEST_PORT_ENABLEm;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, PORT_BITMAPf, ltmbuf, bmp));

    SHR_BITSET(bmp, lport);

    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, PORT_BITMAPf, ltmbuf, bmp));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_dyn_info, ltmbuf));

    sid = EPC_LINK_BMAPm;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, PORT_BITMAPf, ltmbuf, bmp));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_dyn_info, ltmbuf));

exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_a0_ip_port_forwarding_bitmap_disable(int unit, bcmpc_pport_t pport) {
    uint32_t bmp[9] = {0};
    int lport;
    bcmdrd_sid_t sid;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf[BCMTM_MAX_ENTRY_WSIZE] = {0};
    bcmtm_pt_info_t pt_dyn_info = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_pport_lport_get(unit, pport, &lport));

    sid = ING_DEST_PORT_ENABLEm;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, PORT_BITMAPf, ltmbuf, bmp));

    SHR_BITCLR(bmp, lport);

    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, PORT_BITMAPf, ltmbuf, bmp));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_dyn_info, ltmbuf));

    sid = EPC_LINK_BMAPm;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, PORT_BITMAPf, ltmbuf, bmp));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_dyn_info, ltmbuf));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM port level init/config function
 *
 * Perform all port configuration dependent TM configurations.
 *
 * This function is bound to PC drv and will only be invoked by PC during init
 * or after port update.
 */
int
bcm56990_a0_tm_validate (int unit,
                         size_t num_ports,
                         const bcmpc_mmu_port_cfg_t *old_cfg,
                         const bcmpc_mmu_port_cfg_t *new_cfg)
{
    int pport, lport, idx;
    int pipe, pm;
    int num_fp_ports = 0, num_400g_ports = 0;
    int num_fp_ports_pipe[TH4_PIPES_PER_DEV] = {0};
    uint64_t config_value;
    uint32_t flex_port[TH4_PBLKS_PER_DEV] = {0};
    bcmtm_drv_info_t *drv_info;

    SHR_FUNC_ENTER(unit);
    bcmtm_drv_info_get(unit, &drv_info);
    SHR_IF_ERR_EXIT
        (bcmcfg_field_get(unit, DEVICE_CONFIGt, DEVICE_CONFIGt_CORE_CLK_FREQf,
                          &config_value));
    for (idx = 0; idx < (int)num_ports; idx++) {
        lport = new_cfg[idx].lport;
        pport = new_cfg[idx].pport;
        if (lport == BCMPC_INVALID_LPORT || pport == BCMPC_INVALID_LPORT) {
            /*Invalid lport*/
            continue;
        }
        if (bcmtm_lport_is_cpu(unit, lport) || (bcmtm_lport_is_lb(unit, lport))) {
            if ((new_cfg[idx].num_lanes != TH4_NUM_LANES_AUX_PORT) ||
                (new_cfg[idx].speed_max != BCMTM_TH4_AUX_PORT_SPEED)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            continue;
        }

        if (bcmtm_lport_is_mgmt(unit, lport)) {
            continue;
        }
        num_fp_ports++;
        pipe = port_pipe_get(unit, pport);
        if (pipe >= 0) {
            num_fp_ports_pipe[pipe]++;
        }

        /* Get the port macro number from the port. */
        pm = ((pport - 1) >> 2);
        flex_port[pm]++;

        if (new_cfg[idx].speed_max == 400000) {
            num_400g_ports++;
        }
    }

    if (num_fp_ports > TH4_MAX_NUM_FP_PORTS) {
        /* Check total number of front panel ports */
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    for (pipe = 0; pipe < TH4_PIPES_PER_DEV; pipe++) {
        if (num_fp_ports_pipe[pipe] > TH4_MAX_NUM_FP_PORTS_PIPE) {
            /* Check total number of front panel ports per pipe */
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    for (pm = 0; pm < TH4_PBLKS_PER_DEV; pm++) {
        if (flex_port[pm] > drv_info->flex_eligible[pm]) {
            /* port is port macros are more than number of port configured. */
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }
    if (num_400g_ports != 0) {
        /* Check frequency */
        if (th4_freq_retrieve_from_enum(unit, config_value) < 1325) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_a0_tm_chip_init(int unit, bool warm)
{
    uint32_t ltmbuf_arr[9] = {0};
    bcmtm_pt_info_t pt_dyn_info = {0};

    SHR_FUNC_ENTER(unit);

    /* Generate port_map which is equavalent to soc_info in SDK6 */
    SHR_IF_ERR_EXIT
        (bcmtm_drv_info_alloc(unit, warm));

    /* Port mapping for the device */
    SHR_IF_ERR_EXIT
        (bcm56990_a0_tm_port_map_alloc(unit, warm));

    SHR_IF_ERR_EXIT
        (th4_tm_port_map_populate(unit, warm));

    SHR_IF_ERR_EXIT
        (bcm56990_a0_tm_pfc_ddr_info_alloc(unit, warm));

    /* Allocate feature-specific SW states in HA. */
    SHR_IF_ERR_EXIT
        (bcmtm_thd_info_alloc(unit, warm));

    /* Multicast device info ha memory allocation. */
    SHR_IF_ERR_EXIT
        (bcmtm_mc_dev_info_alloc(unit, warm));

    /* Programming CPU_PBMm for CPU logical port 0. */
    SHR_BITSET(ltmbuf_arr, TH4_CPU_LPORT);
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, CPU_PBMm, -1, (void *)&pt_dyn_info,
                                ltmbuf_arr));

    bcm56990_a0_bcmtm_dev_info_set(unit);

    /* Device level init sequence. Skipping all in case of warmboot. */
    if (!warm) {
        SHR_IF_ERR_EXIT
            (th4_init_tm_memory(unit));
        SHR_IF_ERR_EXIT
            (th4_en_mmu_refresh(unit));
        SHR_IF_ERR_EXIT
            (th4_cell_pointer_init(unit));
        SHR_IF_ERR_EXIT
            (th4_mmu_intfi_enable(unit));
        SHR_IF_ERR_EXIT
            (th4_mmu_thdo_pipe_itm_config(unit));
        SHR_IF_ERR_EXIT
            (th4_mmu_rqe_queue_number_init(unit));
        SHR_IF_ERR_EXIT
            (bcm56990_a0_tm_idb_general_init(unit));
        SHR_IF_ERR_EXIT
            (bcm56990_a0_tm_tdm_mmu_init(unit));
        SHR_IF_ERR_EXIT
            (bcm56990_a0_tm_rqe_threshold_init(unit));
        SHR_IF_ERR_EXIT
            (bcm56990_a0_tm_mmu_config_init_thresholds(unit));
        SHR_IF_ERR_EXIT
            (bcm56990_a0_tm_wred_enable(unit));
        SHR_IF_ERR_EXIT
            (bcm56990_a0_tm_ct_init(unit));
    }
    /* Scheduler shaper chip init */
    SHR_IF_ERR_EXIT
        (bcm56990_a0_tm_scheduler_shaper_chip_init(unit, warm));
    /* OBM threshold init. */
    SHR_IF_ERR_EXIT
        (bcm56990_a0_tm_obm_init(unit, warm));
    /* BST Init. */
    SHR_IF_ERR_EXIT
        (bcmtm_bst_init(unit, warm));

    SHR_IF_ERR_EXIT
        (bcm56990_a0_tm_ebst_init(unit, warm));

    /* Multicast init. */
    SHR_IF_ERR_EXIT
        (bcm56990_a0_tm_multicast_init(unit, warm));
    /* WRED init. */
    SHR_IF_ERR_EXIT
        (bcm56990_a0_tm_wred_chip_init(unit, warm));

    if (!warm) {
        SHR_IF_ERR_EXIT
            (bcm56990_a0_tm_imm_populate(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_a0_tm_port_delete(int unit, bcmtm_pport_t pport)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcm56990_a0_tm_port_idb_delete(unit, pport));
    SHR_IF_ERR_EXIT
        (bcm56990_a0_tm_port_mmu_delete(unit, pport));
    SHR_IF_ERR_EXIT
        (bcm56990_a0_tm_port_edb_delete(unit, pport));
    SHR_IF_ERR_EXIT
        (bcm56990_a0_tm_imm_update(unit, pport, 0));
exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_a0_tm_port_add(int unit, bcmtm_pport_t pport)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcm56990_a0_tm_port_idb_add(unit, pport));
    SHR_IF_ERR_EXIT
        (bcm56990_a0_tm_port_mmu_add(unit, pport));
    SHR_IF_ERR_EXIT
        (bcm56990_a0_tm_port_edb_add(unit, pport));
    SHR_IF_ERR_EXIT
        (bcm56990_a0_tm_imm_update(unit, pport, 1));
exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_a0_tm_check_ep_mmu_credit(int unit, int lport)
{
    int pport, mmu_port;
    bcmtm_port_map_info_t *port_map;
    uint32_t ep_mmu_requests = 0, mmu_credit = 0;
    int err_count = 0;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;

    SHR_FUNC_ENTER(unit);
    if (lport == BCMTM_INVALID_LPORT) {
        SHR_ERR_EXIT(SHR_E_PORT);
    }
    bcmtm_port_map_get(unit, &port_map);
    pport = port_map->lport_map[lport].pport;
    mmu_port = port_map->lport_map[lport].mport;
    BCMTM_PT_DYN_INFO(pt_dyn_info, pport, 0);
    SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, EGR_MMU_REQUESTSm, ltid,
                                  (void*)&pt_dyn_info, &ltmbuf));
    SHR_IF_ERR_EXIT(bcmtm_field_get(unit, EGR_MMU_REQUESTSm,
                                    OUTSTANDING_MMU_REQUESTSf,
                                    &ltmbuf, &ep_mmu_requests));

    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, mmu_port);
    /* Read directly from HW as PT cache may not reflect HW update */
    pt_dyn_info.flags |= BCMLT_ENT_ATTR_GET_FROM_HW;
    SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, MMU_EBPTS_EDB_CREDIT_COUNTERr, ltid,
                                  (void*)&pt_dyn_info, &ltmbuf));
    SHR_IF_ERR_EXIT(bcmtm_field_get(unit, MMU_EBPTS_EDB_CREDIT_COUNTERr,
                                    NUM_EDB_CREDITSf,
                                    &ltmbuf, &mmu_credit));

    if (ep_mmu_requests != mmu_credit) {
        LOG_ERROR(BSL_LS_BCMTM_INIT,
                    (BSL_META_U(unit,
                                "FAIL: Port: %d EP to MMU credit mismatch"
                                " EP requests: %d, MMU credit: %d \n"),
                                lport, ep_mmu_requests, mmu_credit));
        err_count += 1;
    } else if (ep_mmu_requests == 0) {
        LOG_ERROR(BSL_LS_BCMTM_INIT,
                    (BSL_META_U(unit,
                                "FAIL: Port: %d EP to MMU credit "
                                " 0 request/credit between EP and MMU \n"),
                                lport));
        err_count += 1;
    }
    if (err_count != 0) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_a0_tm_check_port_ep_credit(int unit, int lport)
{
    int pport;
    bcmtm_port_map_info_t *port_map;
    uint32_t ep_credit = 0, mac_requests = 0;
    int err_count = 0;
    uint32_t ltmbuf[BCMTM_MAX_ENTRY_WSIZE] = {0};
    bcmtm_pt_info_t pt_dyn_info = {0};


    SHR_FUNC_ENTER(unit);
    if (lport == BCMTM_INVALID_LPORT) {
        SHR_ERR_EXIT(SHR_E_PORT);
    }
    if (!bcmtm_lport_is_fp(unit, lport) && !bcmtm_lport_is_mgmt(unit, lport)) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }
    bcmtm_port_map_get(unit, &port_map);
    pport = port_map->lport_map[lport].pport;

    BCMTM_PT_DYN_INFO(pt_dyn_info, pport, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, EGR_PORT_REQUESTSm, -1, &pt_dyn_info,
                               ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, EGR_PORT_REQUESTSm, OUTSTANDING_PORT_REQUESTSf,
                         ltmbuf, &ep_credit));

    if (bcmtm_lport_is_fp(unit, lport)) {
        BCMTM_PT_DYN_INFO(pt_dyn_info, 0, pport);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, CDMAC_TXFIFO_STATUSr,-1, &pt_dyn_info,
                                   ltmbuf));
        SHR_IF_ERR_EXIT
            (bcmtm_field_get(unit, CDMAC_TXFIFO_STATUSr, CELL_REQ_CNTf, ltmbuf,
                             &mac_requests));
    } else if (bcmtm_lport_is_mgmt(unit, lport)) {
        BCMTM_PT_DYN_INFO(pt_dyn_info, 0, pport);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, XLMAC_TXFIFO_CELL_REQ_CNTr,-1, &pt_dyn_info,
                                   ltmbuf));
        SHR_IF_ERR_EXIT
            (bcmtm_field_get(unit, XLMAC_TXFIFO_CELL_REQ_CNTr, REQ_CNTf, ltmbuf,
                             &mac_requests));
        /*TH4 management port, PM->EDB interface is different. Every 2 requests
         * from MAC are quivalent to 1 credit in EP. */
        mac_requests /= 2;
    }
    if (ep_credit != mac_requests) {
        LOG_ERROR(BSL_LS_BCMTM_INIT,
                    (BSL_META_U(unit,
                                "FAIL: Port: %d Port to EP credit mismatch"
                                " Port requests: %d, EP credit: %d \n"),
                                lport, mac_requests, ep_credit));
        err_count += 1;
    } else if (mac_requests == 0) {
        LOG_ERROR(BSL_LS_BCMTM_INIT,
                    (BSL_META_U(unit,
                                "FAIL: Port: %d Port to EP credit "
                                " 0 request/credit between Port and EP \n"),
                                lport));
        err_count += 1;
    }
    if (err_count != 0) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_a0_tm_mchip_port_lport_get(int unit, int mchip_port, int *lport)
{
    int mlocal_port = 0, mport = 0;
    int pipe;

    SHR_FUNC_ENTER(unit);

    mlocal_port = mchip_port % TH4_DEV_PORTS_PER_PIPE;
    pipe = mchip_port / TH4_DEV_PORTS_PER_PIPE;

    mport = pipe * TH4_MMU_PORTS_PER_PIPE + mlocal_port;
    SHR_IF_ERR_EXIT(bcmtm_mport_lport_get(unit, mport, lport));

exit:
    SHR_FUNC_EXIT();

}
