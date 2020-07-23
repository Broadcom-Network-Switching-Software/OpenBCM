/*! \file bcm56880_a0_tm_init.c
 *
 * File containing device level and port level init sequence for bcm56880_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmbd/chip/bcm56880_a0_acc.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmpc/bcmpc_types.h>
#include <bcmmgmt/bcmmgmt_sysm.h>

#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_ha_internal.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmtm/bcmtm_device_info.h>
#include <bcmtm/bst/bcmtm_bst_internal.h>
#include <bcmtm/chip/bcm56880_a0_tm.h>
#include <bcmtm/chip/bcm56880_a0_tm_thd_internal.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>
#include <bcmissu/issu_api.h>
#include <bcmlrd/chip/bcm56880_a0/bcm56880_a0_lrd_enum_ctype.h>
#include <bcmlrd/bcmlrd_conf.h>
#include <bcmcth/bcmcth_tm_drv.h>

#include "bcm56880_a0_tm_obm.h"
#include "bcm56880_a0_tm_wred.h"
#include "bcm56880_a0_tm_ct.h"
#include "bcm56880_a0_tm_scheduler_shaper.h"
#include "bcm56880_a0_tm_ebst.h"
#include "bcm56880_a0_tm_oobfc.h"
#include "bcm56880_a0_tm_multicast_internal.h"
#include "bcm56880_a0_tm_idb_flexport.h"
#include "bcm56880_a0_tm_pfc.h"
#include "bcm56880_a0_tm_mirror_on_drop.h"

/*******************************************************************************
 * Local definitions
 */

static bcmtm_port_info_t cpu_ports[TD4_NUM_CPU_PORTS] = {
    { 0, 0, 0, 19, 19 }
};

static bcmtm_port_info_t lb_ports[TD4_NUM_LB_PORTS] = {
    { 39,  261, 1, 18,  50 },   /* loopback port 0 (ITM 0) */
    { 159, 264, 7, 18, 242 },   /* loopback port 3 (ITM 0) */
    { 79,  262, 3, 18, 114 },   /* loopback port 1 (ITM 1) */
    { 119, 263, 5, 18, 178 },   /* loopback port 2 (ITM 1) */
};

static bcmtm_port_info_t mgmt_ports[TD4_NUM_MGMT_PORTS] = {
    { 38,  257, 1, 19,  51 },   /* management port 0 (ITM 0) */
    { 158, 260, 7, 19, 243 },   /* management port 3 (ITM 0) */
    { 78,  258, 3, 19, 115 },   /* management port 1 (ITM 1) */
    { 118, 259, 5, 19, 179 },   /* management port 2 (ITM 1) */
};

#define TD4_NUM_CPU_QUEUES 48
#define BCMTM_TD4_CPU_LB_PORT_SPEED 10000
#define BSL_LOG_MODULE  BSL_LS_BCMTM_CHIP
#define CLK(mhz)      (BCM56880_A0_LRD_DEVICE_CLK_FREQ_T_T_CLK_##mhz)

/* From DEVICE_CLK_FREQ_T.type.ltl. */
static
unsigned int td4_freq_retrieve_from_enum(int unit, int freq_enum) {
    uint32_t frequency;

    switch (freq_enum) {
        case CLK(1350MHZ):
            frequency = 1350;
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
        case CLK(850MHZ):
            frequency = 850;
            break;
        default:
            frequency = 1350;
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

    if (phys_port < 0 || phys_port >= TD4_NUM_PHYS_PORTS) {
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
   pipe = (phys_port - 1) / TD4_PHYS_PORTS_PER_PIPE;
   return pipe;
}

static int
port_idbport_get(int unit, int phys_port, int dev_port)
{
    int pp, idb_port, pipe;

    if ((dev_port < 0) || (dev_port >= TD4_DEV_PORTS_PER_DEV)) {
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

    pipe = port_pipe_get(unit, phys_port);
    if (pipe == 0) {
        idb_port = (dev_port - 1) % TD4_DEV_PORTS_PER_PIPE;
    } else {
        idb_port = dev_port % TD4_DEV_PORTS_PER_PIPE;
    }

    return idb_port;
}

static int
port_mmuport_get(int unit, int phys_port, int log_port)
{
    int pipe, idb_port, mmu_port;

    idb_port = port_idbport_get(unit, phys_port, log_port);
    pipe = port_pipe_get(unit, phys_port);
    if (pipe < 0 || idb_port < 0) {
        return -1;
    }
    mmu_port = (pipe * TD4_MMU_PORTS_PER_PIPE) + idb_port;
    return mmu_port;
}

static int
td4_uc_cosq_base_get(int unit, int pport, int lport)
{
    int pipe, pipe_queue_base[TD4_PIPES_PER_DEV], pipe_index;
    int local_mmu_port;
    /* Number of queues per pipe is
     * pipe 0: 18 FP, 1 Spare, 1 CPU: 18*12 + 12 + 48 = 276
     * pipe 1,3,5,7: 18FP, 1 LB, 1 MGMT: 18*12 + 12 + 12 = 240
     * pipe 2: 18 FP: 18*12 = 216
     * pipe 4: 228
     * pipe 6: 240
     */
    int num_queues_pipe[] = {276, 240, 216, 240, 228, 240, 240, 240};

    pipe_queue_base[0] = 0;
    for (pipe_index = 1; pipe_index < TD4_PIPES_PER_DEV; pipe_index++) {
        pipe_queue_base[pipe_index] = num_queues_pipe[pipe_index - 1] +
                                        pipe_queue_base[pipe_index - 1];
    }

    pipe = port_pipe_get(unit, pport);
    if (pipe < 0) {
        return -1;
    }
    local_mmu_port = port_idbport_get(unit, pport, lport);
    if (local_mmu_port < 0) {
        return -1;
    }
    return (pipe_queue_base[pipe] + local_mmu_port * TD4_NUM_GP_QUEUES);
}


static int
td4_mc_cosq_base_get(int unit, int pport, int lport)
{
    int num_mcq;
    int uc_q_base = td4_uc_cosq_base_get(unit, pport, lport);

    if (uc_q_base < 0) {
        return -1;
    }
    if (bcmtm_lport_is_cpu(unit, lport)) {
        /* MCQ mode is not applicable for CPU port. */
        return uc_q_base;
    }
    num_mcq = bcm56880_a0_tm_num_mc_q_non_cpu_port_get(unit);
    return (uc_q_base + TD4_NUM_GP_QUEUES - num_mcq);
}

static void
bcm56880_a0_bcmtm_dev_info_set(int unit)
{
    struct bcmtm_device_info bcm56880_a0_tm_dev_info = {
        .max_pkt_sz   = TD4_MMU_MAX_PACKET_BYTES,
        .pkt_hdr_sz   = TD4_MMU_PACKET_HEADER_BYTES,
        .jumbo_pkt_sz = TD4_MMU_JUMBO_FRAME_BYTES,
        .default_mtu  = TD4_MMU_DEFAULT_MTU_BYTES,
        .cell_sz      = TD4_MMU_BYTES_PER_CELL,
        .num_cells    = BCM56880_A0_MMU_TOTAL_CELLS_PER_ITM,
        .num_port_pg  = SOC_MMU_CFG_PRI_GROUP_MAX,
        .num_sp       = SOC_MMU_CFG_SERVICE_POOL_MAX,
        .mcq_entry    = SOC_TD4_MMU_MCQ_ENTRY_PER_ITM,
        .num_q        = TD4_NUM_GP_QUEUES,
        .num_cpu_q    = TD4_NUM_CPU_QUEUES,
        .num_pipe     = TD4_PIPES_PER_DEV,
        .num_nhop_sparse = TD4_NHOPS_PER_REPL_INDEX_SPARSE_MODE,
        .num_nhop_dense = TD4_NHOPS_PER_REPL_INDEX_DENSE_MODE,
        .max_num_repl = TD4_MAX_MC_PKT_REPL,
        .num_buffer_pool  = TD4_ITMS_PER_DEV,
        };
    uint32_t flags;
    bcmtm_device_info[unit] = bcm56880_a0_tm_dev_info;

    flags = bcmdrd_dev_flags_get(unit);
    if ((flags & CHIP_FLAG_BW8T) == CHIP_FLAG_BW8T) {
        bcmtm_device_info[unit].num_cells = BCM56883_A0_MMU_TOTAL_CELLS_PER_ITM;
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
td4_dpr_frequency_range_check(int unit, int dpr_freq)
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
 *
 * The chip specific information will be retrieved during chip init and config.
 */
static int
td4_tm_drv_info_populate(int unit, bool warm)
{
    uint64_t config_value;
    uint64_t flex_port[TD4_PBLKS_PER_DEV] = {0};
    bcmtm_drv_info_t *drv_info;
    int pm_id;
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
    drv_info->frequency = td4_freq_retrieve_from_enum(unit, config_value);
    SHR_IF_ERR_EXIT
        (bcmcfg_field_get(unit, DEVICE_CONFIGt, DEVICE_CONFIGt_PP_CLK_FREQf,
                          &config_value));
    drv_info->dpr_clock_frequency = td4_freq_retrieve_from_enum(unit, config_value);
    drv_info->fabric_port_enable = 0;
    drv_info->oobfc_clock_ns = 8;

    SHR_IF_ERR_EXIT
        (bcmcfg_field_array_get(unit, TM_PM_FLEX_CONFIGt,
                                TM_PM_FLEX_CONFIGt_MAX_SUB_PORTf,
                                0, TD4_PBLKS_PER_DEV,
                                flex_port, &actual));
    for (pm_id = 0; pm_id < TD4_PBLKS_PER_DEV; pm_id++) {
        drv_info->flex_eligible[pm_id] = flex_port[pm_id];
    }

    SHR_IF_ERR_EXIT
        (td4_dpr_frequency_range_check(unit, drv_info->dpr_clock_frequency));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Function to fill port_sched_state_resource structure
 *
 * Retrieve all information contained in bcmpc_mmu_port_cfg_t and fill it into
 * the port_schedule_state field in tm_drv_info. The bcmpc_mmu_port_cfg_t is
 * generated by BCMPC and contains all the necessary port config info.
 *
 * \param [in] unit Logical unit number.
 * \param [in] num_ports Number of ports
 * \param [in] old_cfg Old port configuration indexed by physical ports.
 * \param [in] new_cfg New port configuration indexed by physical ports.
 */
void
bcm56880_a0_tm_drv_info_portmap_fill(int unit,
                                     size_t num_ports,
                                     const bcmpc_mmu_port_cfg_t *old_cfg,
                                     const bcmpc_mmu_port_cfg_t *new_cfg)
{
    int pport, lport, mmu_port, idb_port;
    int chip_port, pipe;
    bcmtm_port_map_info_t *port_map;
    int idx;

    bcmpc_mmu_port_cfg_t port_cfg_data;
    bcmpc_mmu_port_cfg_t *port_cfg;

    bcmtm_port_map_get(unit, &port_map);

    /* Config array is indexed by physical port number. */
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
            if (port_map->lport_map[lport].cur_speed < BCMTM_TD4_CPU_LB_PORT_SPEED) {
                /* set speed to minimum supported internal to TM */
                port_map->lport_map[lport].cur_speed = BCMTM_TD4_CPU_LB_PORT_SPEED;
            }
        }

        pipe = port_pipe_get(unit, pport);
        port_map->lport_map[lport].pipe = pipe;
        idb_port = port_idbport_get(unit, pport, lport);
        if (idb_port < 0 ) {
            continue;
        }
        port_map->lport_map[lport].mlocal_port = idb_port;
        port_map->pport_map[pport].mlocal_port = idb_port;
        mmu_port = port_mmuport_get(unit, pport, lport);
        port_map->pport_map[pport].mport = mmu_port;
        port_map->lport_map[lport].mport = mmu_port;
        port_map->mport_map[mmu_port].lport = lport;
        chip_port=
            (port_map->lport_map[lport].pipe * TD4_DEV_PORTS_PER_PIPE) +
             port_map->lport_map[lport].mlocal_port;
        port_map->lport_map[lport].mchip_port = chip_port;
        if (bcmtm_lport_is_cpu(unit, lport)) {
            /* CPU port */
            port_map->lport_map[lport].num_mcq = 48;
            port_map->lport_map[lport].num_ucq = 0;
            /* Update lport speed for CPU. */
            port_map->lport_map[lport].max_speed = BCMTM_TD4_CPU_LB_PORT_SPEED;
            port_map->lport_map[lport].cur_speed = BCMTM_TD4_CPU_LB_PORT_SPEED;
        } else {
            port_map->lport_map[lport].num_mcq =
                bcm56880_a0_tm_num_mc_q_non_cpu_port_get(unit);
            port_map->lport_map[lport].num_ucq =
                bcm56880_a0_tm_num_uc_q_non_cpu_port_get(unit);
        }

        /* Overwrite loopback port speed (only support 10G). */
        if (bcmtm_lport_is_lb(unit, lport)) {
            port_map->lport_map[lport].max_speed = BCMTM_TD4_CPU_LB_PORT_SPEED;
            port_map->lport_map[lport].cur_speed = BCMTM_TD4_CPU_LB_PORT_SPEED;
        }

        port_map->lport_map[lport].base_ucq = td4_uc_cosq_base_get(unit, pport,
                port_cfg->lport);
        port_map->lport_map[lport].base_mcq = td4_mc_cosq_base_get(unit, pport,
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
        if (port_cfg->flags & BCMPC_MMU_F_HG3) {
            port_map->lport_map[lport].flags |= BCMTM_PORT_IS_HG3;
        } else {
            port_map->lport_map[lport].flags &= (0xffffffff ^ BCMTM_PORT_IS_HG3);
        }
    }
}

static int
td4_mmu_thdo_pipe_itm_config(int unit) {
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_sid_t sid ;
    bcmdrd_fid_t fid ;
    uint32_t fval ;
    int pipe, itm;
    uint32_t ltmbuf = 0;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */

    SHR_FUNC_ENTER(unit);
    for (pipe = 0; pipe < TD4_PIPES_PER_DEV; pipe++) {
        sid = MMU_THDO_CONFIG_PORTr;
        fid = OUTPUT_PORT_RX_ENABLEf;
        fval = 0xfffff; /* Enable all 20 ports in the pipe */
        ltmbuf = 0;

        BCMTM_PT_DYN_INFO(pt_dyn_info, pipe, 0);
        SHR_IF_ERR_EXIT(bcmtm_pt_indexed_read(unit, sid, ltid,
                            (void*)&pt_dyn_info, &ltmbuf));
        SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, sid, ltid,
                    (void*)&pt_dyn_info, &ltmbuf));
    }

    sid = MMU_THDO_ENGINE_ENABLES_CFGr;
    fid = SRC_PORT_DROP_COUNT_ENABLEf;
    for (itm = 0; itm < TD4_ITMS_PER_DEV; itm++) {
        if (bcm56880_a0_tm_check_valid_itm(unit, itm) == SHR_E_NONE) {
            ltmbuf = 0;
            fval = 1;
            BCMTM_PT_DYN_INFO(pt_dyn_info, 0, itm);
            SHR_IF_ERR_EXIT(bcmtm_pt_indexed_read(unit, sid, ltid,
                                (void*)&pt_dyn_info, &ltmbuf));
            SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
            SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, DLB_INTERFACE_ENABLEf,
                                            &ltmbuf, &fval));
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, sid, ltid,
                        (void*)&pt_dyn_info, &ltmbuf));
        }
    }

    sid = MMU_THDO_COUNTER_MC_OVERFLOW_SIZEr;
    fid = MC_OVERFLOW_SIZEf;
    fval = 0x45dd0; /* Sky high value according to MMU settings v3.2 */
    ltmbuf = 0;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_dyn_info, &ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_dyn_info, &ltmbuf));

exit:
    SHR_FUNC_EXIT();
}

static int
td4_oqs_ager_limit_encode(int unit)
{
    bcmtm_drv_info_t *drv_info;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_sid_t sid ;
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t fval;
    uint32_t ltmbuf = 0;
    int speed_encode;
    /* Recommended values of ager_limit based on speed encoding. */
    uint32_t ager_limit[4] = {4, 4, 3, 2};
    int index;
    uint32_t cpu_ager_limit;


    SHR_FUNC_ENTER(unit);
    bcmtm_drv_info_get(unit, &drv_info);
    /* Configure OQS ager limit based on port speed encoding. */
    sid = MMU_OQS_AGER_LIMITr;
    fid = AGER_LIMITf;
    /* OQS port_speed encoding - 0:<=50GE, 1:100GE, 2:200GE, 3:400GE*/
    for (speed_encode = 0; speed_encode < 4; speed_encode++) {
        /*
         * Program same ager limit for 2 indices mapped to the same speed encode
         * indexed by {port_speed[1:0], HP_MMUQ[0]}
         */
        for (index = (speed_encode * 2); index < ((speed_encode + 1) * 2);
                index++) {
            ltmbuf = 0;
            fval = ager_limit[speed_encode];
            BCMTM_PT_DYN_INFO(pt_dyn_info, index, 0);
            SHR_IF_ERR_EXIT(bcmtm_pt_indexed_read(unit, sid, ltid,
                            (void*)&pt_dyn_info, &ltmbuf));
            SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, sid, ltid,
                        (void*)&pt_dyn_info, &ltmbuf));
        }
    }
    /* Ager limit config for CPU port. */
    index = 8;
    ltmbuf = 0;
    cpu_ager_limit = 4;
    BCMTM_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_read(unit, sid, ltid,
                    (void*)&pt_dyn_info, &ltmbuf));
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &cpu_ager_limit));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid,
                (void*)&pt_dyn_info, &ltmbuf));

exit:
    SHR_FUNC_EXIT();
}
static int
td4_check_mmu_mem_init_done(int unit)
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
        for (itm = 0; itm < TD4_ITMS_PER_DEV; itm++) {
            if (bcm56880_a0_tm_check_valid_itm(unit, itm) == SHR_E_NONE) {
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
        for (pipe = 0; pipe < TD4_PIPES_PER_DEV; pipe++) {
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
td4_init_tm_memory(int unit)
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
        SHR_IF_ERR_EXIT(td4_check_mmu_mem_init_done(unit));
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
    for (pipe = 0; pipe < TD4_PIPES_PER_DEV; pipe++) {
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
td4_en_mmu_refresh(int unit)
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
td4_cell_pointer_init(int unit)
{
    int itm;
    bcmdrd_sid_t sid = MMU_CFAP_INIT_64r;
    uint32_t fval[2] = {0xffffffff, 0x3};
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf[2] = {0};
    bcmdrd_fid_t fid = START_CFAP_INITf;


    SHR_FUNC_ENTER(unit);

    for (itm = 0; itm < TD4_ITMS_PER_DEV; itm++) {
        if (bcm56880_a0_tm_check_valid_itm(unit, itm) == SHR_E_NONE) {
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
bcm56880_a0_tm_cos_map_init(int unit)
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
td4_mmu_intfi_enable(int unit)
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
bcm56880_a0_tm_info_table_init(int unit, bool warm)
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
bcm56880_a0_tm_pfc_ddr_info_alloc(int unit, bool warm)
{
    bcmtm_pfc_ddr_info_t *pfc_ddr_info;
    uint32_t data_sz, alloc_sz;

    SHR_FUNC_ENTER(unit);

    data_sz = sizeof(bcmtm_pfc_ddr_info_t);
    alloc_sz = data_sz * TD4_DEV_PORTS_PER_DEV;
    pfc_ddr_info = shr_ha_mem_alloc(unit,
                                    BCMMGMT_TM_COMP_ID,
                                    BCMTM_HA_DEV_PFC_DEADLOCK_INFO,
                                    "bcmtmPfcDeadlockInfo",
                                    &alloc_sz);
    SHR_NULL_CHECK(pfc_ddr_info, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT
        ((alloc_sz < (data_sz * TD4_DEV_PORTS_PER_DEV)) ? SHR_E_MEMORY: SHR_E_NONE);
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "BCMTM: HA memory for pfc ddr port map %0d\n"),
            alloc_sz));
    if (!warm) {
        sal_memset(pfc_ddr_info, 0, alloc_sz);
        bcmissu_struct_info_report(unit, BCMMGMT_TM_COMP_ID,
                                   BCMTM_HA_DEV_PFC_DEADLOCK_INFO, 0,
                                   data_sz, TD4_DEV_PORTS_PER_DEV,
                                   BCMTM_PFC_DDR_INFO_T_ID);
    }
    SHR_IF_ERR_EXIT(bcmtm_pfc_ddr_info_set(unit, pfc_ddr_info));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_tm_port_map_alloc(int unit, bool warm)
{
    bcmtm_port_map_info_t port_map;
    uint32_t data_sz, alloc_sz;
    size_t num_inst;

    SHR_FUNC_ENTER(unit);

    data_sz = sizeof(bcmtm_lport_map_t);
    num_inst = TD4_DEV_PORTS_PER_DEV;
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
    num_inst = TD4_NUM_PHYS_PORTS;
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
    num_inst = TD4_MMU_PORTS_PER_PIPE * TD4_PIPES_PER_DEV;
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
bcm56880_a0_tm_chip_q_get(int unit, uint32_t lport,
                          uint32_t mmu_q, uint32_t *chip_q)
{
    uint32_t num_uc_q;
    int q_base;
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
    if (q_base == -1) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
    if (chip_q) {
        *chip_q = q_base + mmu_q;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm56880_a0_tm_mmu_q_from_chip_q_get(int unit, int chip_q,
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
        *mmu_q -= num_uc_q;
        *q_type = MC_Q; /* Multicast queues. */
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm56880_a0_tm_check_valid_itm (int unit, int itm)
{
    uint32_t valid_pipe_bmp;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmdrd_dev_valid_pipes_get(unit, &valid_pipe_bmp));
    if ((itm >= TD4_ITMS_PER_DEV) || (itm < 0)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (itm == 0) {
        /* Check if pipes 0,1,6,7 are part of valid pipes */
        if ((valid_pipe_bmp & 0xc3) != 0) {
            SHR_EXIT();
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    } else if (itm == 1) {
        /* Check if pipes 2,3,4,5 are part of valid pipes */
        if ((valid_pipe_bmp & 0x3c) != 0) {
            SHR_EXIT();
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm56880_a0_tm_num_uc_q_non_cpu_port_get(int unit)
{
    int num_mcq;
    num_mcq = bcm56880_a0_tm_num_mc_q_non_cpu_port_get(unit);
    return (TD4_NUM_GP_QUEUES - num_mcq);
}


int
bcm56880_a0_tm_num_mc_q_non_cpu_port_get(int unit)
{
    static int mc_q_mode_num_mc_queues[4] = {0, 2, 4, 6};
    bcmltd_sid_t ltid = TM_SCHEDULER_CONFIGt;
    bcmltd_fid_t fid = TM_SCHEDULER_CONFIGt_NUM_MC_Qf;
    uint64_t mc_q_mode;

    (void)bcmcfg_field_get(unit, ltid, fid, &mc_q_mode);
    return mc_q_mode_num_mc_queues[mc_q_mode];
}

int
bcm56880_a0_ip_port_forwarding_bitmap_enable (int unit, bcmpc_pport_t pport) {
    int ioerr = 0;
    uint32_t bmp[5];
    IPOST_MPB_CCBI_FIXED_ING_DEST_PORT_ENABLEm_t ccbi_ing_dst_port;
    bcmtm_port_map_info_t *port_map;
    int lport;

    bcmtm_port_map_get(unit, &port_map);
    lport = port_map->pport_map[pport].lport;
    ioerr += READ_IPOST_MPB_CCBI_FIXED_ING_DEST_PORT_ENABLEm
                                            (unit, 0, &ccbi_ing_dst_port);
    IPOST_MPB_CCBI_FIXED_ING_DEST_PORT_ENABLEm_PORT_BITMAPf_GET
                                                (ccbi_ing_dst_port, bmp);


    SHR_BITSET(bmp, lport);
    IPOST_MPB_CCBI_FIXED_ING_DEST_PORT_ENABLEm_PORT_BITMAPf_SET
                                                (ccbi_ing_dst_port, bmp);
    ioerr += WRITE_IPOST_MPB_CCBI_FIXED_ING_DEST_PORT_ENABLEm
                                            (unit, 0, ccbi_ing_dst_port);

    return ioerr;

}

int
bcm56880_a0_ip_port_forwarding_bitmap_disable (int unit, bcmpc_pport_t pport) {
    int ioerr = 0;
    uint32_t bmp[5];
    IPOST_MPB_CCBI_FIXED_ING_DEST_PORT_ENABLEm_t ccbi_ing_dst_port;
    bcmtm_port_map_info_t *port_map;
    int lport;

    bcmtm_port_map_get(unit, &port_map);
    lport = port_map->pport_map[pport].lport;
    ioerr += READ_IPOST_MPB_CCBI_FIXED_ING_DEST_PORT_ENABLEm
                                        (unit, 0, &ccbi_ing_dst_port);
    IPOST_MPB_CCBI_FIXED_ING_DEST_PORT_ENABLEm_PORT_BITMAPf_GET
                                             (ccbi_ing_dst_port, bmp);


    SHR_BITCLR(bmp, lport);
    IPOST_MPB_CCBI_FIXED_ING_DEST_PORT_ENABLEm_PORT_BITMAPf_SET
                                            (ccbi_ing_dst_port, bmp);
    ioerr += WRITE_IPOST_MPB_CCBI_FIXED_ING_DEST_PORT_ENABLEm
                                        (unit, 0, ccbi_ing_dst_port);

    return ioerr;

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
bcm56880_a0_tm_validate (int unit,
                         size_t num_ports,
                         const bcmpc_mmu_port_cfg_t *old_cfg,
                         const bcmpc_mmu_port_cfg_t *new_cfg)
{
    int pport, lport, idx;
    int pipe, pm;
    uint32_t num_fp_ports = 0, num_400g_ports = 0;
    int num_fp_ports_pipe[TD4_PIPES_PER_DEV] = {0};
    uint64_t config_value;
    bcmtm_drv_info_t *drv_info;
    uint32_t flags;
    uint32_t max_fp_port = TD4_MAX_NUM_FP_PORTS;
    uint32_t flex_port[TD4_PBLKS_PER_DEV] = {0};

    SHR_FUNC_ENTER(unit);
    bcmtm_drv_info_get(unit, &drv_info);
    SHR_IF_ERR_EXIT
        (bcmcfg_field_get(unit, DEVICE_CONFIGt, DEVICE_CONFIGt_CORE_CLK_FREQf,
                          &config_value));

    for (idx = 0; idx < (int)num_ports; idx++) {
        pport = new_cfg[idx].pport;
        lport = new_cfg[idx].lport;
        if (lport == BCMPC_INVALID_LPORT || pport == BCMPC_INVALID_LPORT) {
            /*Invalid lport*/
            continue;
        }
        if (bcmtm_lport_is_cpu(unit, lport) || bcmtm_lport_is_lb(unit, lport)) {
            /* Auxillary port */
            if ((new_cfg[idx].num_lanes != TD4_NUM_LANES_AUX_PORT) ||
                (new_cfg[idx].speed_cur != BCMTM_TD4_CPU_LB_PORT_SPEED)) {
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
        pm = ((pport - 1) >> 3);
        flex_port[pm]++;

        if (new_cfg[idx].speed_cur == 400000) {
            num_400g_ports++;
        }

    }

    flags = bcmdrd_dev_flags_get(unit);

    if ((flags & CHIP_FLAG_BW8T) == CHIP_FLAG_BW8T) {
        max_fp_port = 72;
        /* BCM56883 each packet pipe supports total of 18 ports.*/
        for (pipe = 0; pipe < TD4_PIPES_PER_DEV; pipe += 2) {
            if (num_fp_ports_pipe[pipe] + num_fp_ports_pipe[pipe + 1] >
                                                 TD4_MAX_NUM_FP_PORTS_PIPE) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }
    } else {
        for (pipe = 0; pipe < TD4_PIPES_PER_DEV; pipe++) {
            if (num_fp_ports_pipe[pipe] > TD4_MAX_NUM_FP_PORTS_PIPE) {
                /* Check total number of front panel ports per pipe */
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }
        for (pm = 0; pm < TD4_PBLKS_PER_DEV; pm++) {
            if (flex_port[pm] > drv_info->flex_eligible[pm]) {
                /* port is port macros are more than number of port configured. */
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }
    }

    /* Check total number of front panel ports */
    if (num_fp_ports > max_fp_port) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (num_400g_ports != 0) {
        /* Check frequency */
        if (td4_freq_retrieve_from_enum(unit, config_value) < 1325) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }
exit:
    SHR_FUNC_EXIT();
}


int
bcm56880_a0_tm_chip_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    /* Generate drv_info which is equavalent to soc_info in SDK6 */
    SHR_IF_ERR_EXIT
        (bcmtm_drv_info_alloc(unit, warm));

    /* Port mapping for the device. */
    SHR_IF_ERR_EXIT
        (bcm56880_a0_tm_port_map_alloc(unit, warm));

    /* Populate TM driver info. */
    SHR_IF_ERR_EXIT
        (td4_tm_drv_info_populate(unit, warm));

    /* Allocate feature-specific SW states in HA. */
    SHR_IF_ERR_EXIT
        (bcmtm_thd_info_alloc(unit, warm));

    /* Allocate for pfc ddr info. */
    SHR_IF_ERR_EXIT
        (bcm56880_a0_tm_pfc_ddr_info_alloc(unit, warm));

    /* Multicast device info ha memory allocation. */
    SHR_IF_ERR_EXIT
        (bcmtm_mc_dev_info_alloc(unit, warm));


    /*! BCMTM device info set */
    bcm56880_a0_bcmtm_dev_info_set(unit);

    /* Device level init sequence. Need to skip in the case of warm boot. */
    if (!warm) {
        SHR_IF_ERR_EXIT
            (td4_init_tm_memory(unit));
        SHR_IF_ERR_EXIT
            (td4_en_mmu_refresh(unit));
        SHR_IF_ERR_EXIT
            (td4_cell_pointer_init(unit));
        SHR_IF_ERR_EXIT
            (td4_mmu_intfi_enable(unit));
        SHR_IF_ERR_EXIT
            (td4_mmu_thdo_pipe_itm_config(unit));
        SHR_IF_ERR_EXIT
            (td4_oqs_ager_limit_encode(unit));
        SHR_IF_ERR_EXIT
            (bcm56880_a0_tm_idb_general_init(unit));
        SHR_IF_ERR_EXIT
            (bcm56880_a0_tm_cos_map_init(unit));
        SHR_IF_ERR_EXIT
            (bcm56880_a0_tm_tdm_mmu_init(unit));
        SHR_IF_ERR_EXIT
            (bcm56880_a0_tm_rqe_threshold_init(unit));
        SHR_IF_ERR_EXIT
            (bcm56880_a0_tm_mmu_config_init_thresholds(unit));
        /* To be enabled after THDO. */
        SHR_IF_ERR_EXIT
            (bcm56880_a0_tm_wred_enable(unit));
        /* Cut Through initialization */
        SHR_IF_ERR_EXIT
            (bcm56880_a0_tm_ct_init(unit));
    }

    /* scheduler init. */
    SHR_IF_ERR_EXIT
        (bcm56880_a0_tm_scheduler_chip_init(unit, warm));

    /* Shaper init. */
    SHR_IF_ERR_EXIT
        (bcm56880_a0_tm_shaper_chip_init(unit, warm));

    /* OBM threshold init. */
    SHR_IF_ERR_EXIT
        (bcm56880_a0_tm_obm_init(unit, warm));

    /* WRED init. */
    SHR_IF_ERR_EXIT
        (bcm56880_a0_tm_wred_chip_init(unit, warm));

    /* Buffer statistics tracking init. */
    SHR_IF_ERR_EXIT
        (bcmtm_bst_init(unit, warm));

    SHR_IF_ERR_EXIT
        (bcm56880_a0_tm_ebst_init(unit, warm));
    /* IMM populate. */
    if (!warm) {
        SHR_IF_ERR_EXIT
            (bcm56880_a0_tm_imm_populate(unit));
    }

    /* Info table initialization. */
    SHR_IF_ERR_EXIT
        (bcm56880_a0_tm_info_table_init(unit, warm));
exit:
    SHR_FUNC_EXIT();
}
int
bcm56880_a0_tm_port_delete(int unit, bcmtm_pport_t pport)
{

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcm56880_a0_tm_port_idb_delete(unit, pport));
    SHR_IF_ERR_EXIT
        (bcm56880_a0_tm_port_mmu_delete(unit, pport));
    SHR_IF_ERR_EXIT
        (bcm56880_a0_tm_port_edb_delete(unit, pport));
    SHR_IF_ERR_EXIT
        (bcm56880_a0_tm_imm_update(unit, pport, 0));
exit:
    SHR_FUNC_EXIT();
}

int
bcm56880_a0_tm_port_add(int unit, bcmtm_pport_t pport)
{

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcm56880_a0_tm_port_idb_add(unit, pport));
    SHR_IF_ERR_EXIT
        (bcm56880_a0_tm_port_mmu_add(unit, pport));
    SHR_IF_ERR_EXIT
        (bcm56880_a0_tm_port_edb_add(unit, pport));
    SHR_IF_ERR_EXIT
        (bcm56880_a0_tm_imm_update(unit, pport, 1));
exit:
    SHR_FUNC_EXIT();

}

/*!
 * \breif  Update unicast drop config in IP.

 * @param [in] unit Device number
 * @param [in] pport Physical port number
 * @param [in] lport Logical port number
 * @param [in] add_flag Flag indicating port add or delete sequence.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm56880_a0_tm_update_unicast_drop_config(int unit,
                                          bcmtm_pport_t pport,
                                          int lport,
                                          bool flag)
{
    int inst = 0;
    int index = 0;
    uint32_t ltmbuf = 0;
    uint32_t fval;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_sid_t sid = IPOST_MPB_CCBI_FIXED_UNICAST_DROP_CONFIGr;
    bcmdrd_fid_t fid = DST_PORT_NUMf;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    bcmtm_pport_t t_pport = BCMTM_INVALID_LPORT;
    int port = BCMTM_INVALID_LPORT;
    bcmtm_port_map_info_t *port_map;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);

    /*
     * Get the valid logical port in the device
     * which is not cpu port or the deleted port.
     */
    for (port = 0; port < TD4_DEV_PORTS_PER_DEV; port++) {
        if (flag) {
            if (port == 0) {
                continue;
            }
        } else {
            if ((port == lport) ||
                (port == 0)) {
                continue;
            }
        }
        /* get physical port from logical port. */
        t_pport = port_map->lport_map[port].pport;
        if (pport == t_pport){
            continue;
        } else if (t_pport == (uint16_t) BCMTM_INVALID_LPORT) {
            continue;
        }
        fval = port;
       break;
    }
    BCMTM_PT_DYN_INFO(pt_dyn_info, index, inst);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_dyn_info, &ltmbuf));
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_dyn_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

int
bcm56880_a0_tm_check_ep_mmu_credit(int unit, int lport)
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
        LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "FAIL: Port: %d EP to MMU credit mismatch"
                                " EP requests: %d, MMU credit: %d "
                                "lport : %d, mmu_port: %d\n"),
                                lport, ep_mmu_requests, mmu_credit, lport,
                                mmu_port));
        err_count += 1;
    } else if (ep_mmu_requests == 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "FAIL: Port: %d EP to MMU credit "
                                " 0 request/credit between EP and MMU \n"),
                                lport));
        err_count += 1;
    } else {
        LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                        "PASS: Port: %d EP to MMU credit "
                        "EP requests: %d, MMU credit: %d \n"),
                        lport, ep_mmu_requests, mmu_credit));
    }
    if (err_count != 0) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm56880_a0_tm_check_port_ep_credit(int unit, int lport)
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

    }
    if (ep_credit != mac_requests) {
        LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "FAIL: Port: %d Port to EP credit mismatch"
                                " Port requests: %d, EP credit: %d \n"),
                                lport, mac_requests, ep_credit));
        err_count += 1;
    } else if (mac_requests == 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "FAIL: Port: %d Port to EP credit "
                                " 0 request/credit between Port and EP \n"),
                                lport));
        err_count += 1;
    } else {
        LOG_INFO(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "PASS: Port: %d Port to EP credit "
                            " Port requests: %d, EP credit: %d \n"),
                            lport, mac_requests, ep_credit));
    }
    if (err_count != 0) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm56880_a0_tm_mchip_port_lport_get(int unit, int mchip_port, int *lport)
{
    int mlocal_port = 0, mport = 0;
    int pipe;

    SHR_FUNC_ENTER(unit);

    mlocal_port = mchip_port % TD4_DEV_PORTS_PER_PIPE;
    pipe = mchip_port / TD4_DEV_PORTS_PER_PIPE;

    mport =  pipe * TD4_MMU_PORTS_PER_PIPE + mlocal_port;
    SHR_IF_ERR_EXIT(bcmtm_mport_lport_get(unit, mport, lport));
exit:
    SHR_FUNC_EXIT();

}
