/*! \file bcm56780_a0_tm_thd.c
 *
 * File containing mmu threshold init sequence for bcm56780_a0
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_sleep.h>

#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmbd/chip/bcm56780_a0_acc.h>

#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmtm/generated/bcmtm_ha.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_device_info.h>
#include <bcmtm/chip/bcm56780_a0_tm_thd_internal.h>
#include <bcmtm/chip/bcm56780_a0_tm.h>

#define BSL_LOG_MODULE  BSL_LS_BCMTM_CHIP
/*******************************************************************************
 * Private functions
 */
static int
ceiling(int a, int b)
{
    return ((a-1)/b) + 1;

}

static int
itm_ipipe_map(int unit, int itm)
{
    uint32_t pipe_map = 0;


    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(bcmdrd_dev_valid_pipes_get(unit, &pipe_map));
    if (itm == 0)
    {
        /* If itm 0 is input, return the enabled pipe bitmap in itm 0 */
        return 0xc3 & pipe_map;
    }
    else {
        /* If itm 1 is input, return the enabled pipe bitmap in itm 1 */
        return 0x3c & pipe_map;
    }
exit:
    SHR_FUNC_EXIT();
}


static int
pt_index_max(int unit, bcmdrd_sid_t sid)
{
    bcmdrd_sym_info_t sinfo;

    SHR_FUNC_ENTER(unit);
    if (SHR_FAILURE(bcmdrd_pt_info_get(unit, sid, &sinfo))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    return sinfo.index_max;

exit:
    SHR_FUNC_EXIT();

}

uint32_t
td4_x9_piped_mem_index(int unit, int port, bcmdrd_sid_t mem, int arr_off)
{
    int mmu_chip_port, mmu_local_port, index;
    bcmtm_port_map_info_t *port_map;

    bcmtm_port_map_get(unit, &port_map);

    mmu_local_port = port_map->lport_map[port].mlocal_port;
    mmu_chip_port = port_map->lport_map[port].mchip_port;

    switch (mem) {
    case MMU_THDI_PORT_PG_MIN_CONFIGm:
    case MMU_THDI_PORT_PG_SHARED_CONFIGm:
    case MMU_THDI_PORT_PG_RESUME_CONFIGm:
    case MMU_THDI_PORT_PG_HDRM_CONFIGm:
    case MMU_THDI_PORTSP_CONFIGm:
    case MMU_THDI_PORT_BST_CONFIGm:
         /* Index is local mmu port 0-19 */
        index = mmu_local_port;
        break;
    case MMU_THDO_CONFIG_PORT_UC0m:
    case MMU_THDO_CONFIG_PORT_UC1m:
    case MMU_THDO_CONFIG_PORT_UC2m:
    case MMU_THDO_PORT_QUEUE_SERVICE_POOLm:
        index = mmu_chip_port;
        break;
    case MMU_THDO_CONFIG_PORTSP_MCm:
    case MMU_THDO_RESUME_PORT_UC0m:
    case MMU_THDO_RESUME_PORT_UC1m:
    case MMU_THDO_RESUME_PORT_UC2m:
    case MMU_THDO_RESUME_PORT_MC0m:
    case MMU_THDO_RESUME_PORT_MC1m:
    case MMU_THDO_RESUME_PORT_MC2m:
         /* Index is port,  spid */
        index = (mmu_chip_port << 2) + arr_off;
        break;
    default:
        return arr_off;
    }

    return index;
}


void
td4_x9_mmu_init_dev_config(int unit, soc_mmu_device_info_t *devcfg, int lossless)
{
    sal_memset(devcfg, 0, sizeof(soc_mmu_device_info_t));

    devcfg->max_pkt_byte = TD4_X9_MMU_MAX_PACKET_BYTES;
    devcfg->mmu_hdr_byte = TD4_X9_MMU_PACKET_HEADER_BYTES;
    devcfg->jumbo_pkt_size = TD4_X9_MMU_JUMBO_FRAME_BYTES;
    devcfg->default_mtu_size = TD4_X9_MMU_DEFAULT_MTU_BYTES;
    devcfg->mmu_cell_size = TD4_X9_MMU_BYTES_PER_CELL;
    devcfg->mmu_total_cell = TD4_X9_MMU_TOTAL_CELLS_PER_ITM;
    devcfg->num_pg = TD4_X9_MMU_NUM_PG;
    devcfg->num_service_pool = TD4_X9_MMU_NUM_POOL;
    devcfg->flags = SOC_MMU_CFG_F_PORT_MIN | SOC_MMU_CFG_F_PORT_POOL_MIN |
                    SOC_MMU_CFG_F_RQE | SOC_MMU_CFG_F_EGR_MCQ_ENTRY;
    devcfg->total_mcq_entry = TD4_X9_MMU_TOTAL_MCQ_ENTRY(unit);
    devcfg->rqe_queue_num = TD4_X9_MMU_NUM_RQE_QUEUES;
}

int
td4_x9_default_pg_headroom(int unit, int port)
{
    int speed = 10000, hdrm = 0;

    bcmtm_drv_info_t *drv_info;
    bcmtm_port_map_info_t *port_map;
    bcmtm_drv_info_get(unit, &drv_info);
    bcmtm_port_map_get(unit, &port_map);

    if (bcmtm_lport_is_cpu(unit, port)) {
        return 0;
    } else if (bcmtm_lport_is_mgmt(unit, port)) {
        return 0;
    } else if (bcmtm_lport_is_lb(unit, port)) {
        return 0;
    } else if (!drv_info->lossless) {
        return 0;
    }

    speed = port_map->lport_map[port].cur_speed;

    if (speed >= 400000) {
        hdrm = TD4_X9_PG_HEADROOM_OVERSUB_400G;
    } else if (speed >= 200000) {
        hdrm = TD4_X9_PG_HEADROOM_OVERSUB_200G;
    } else if (speed >= 100000) {
        hdrm = TD4_X9_PG_HEADROOM_OVERSUB_100G;
    } else if (speed >= 40000) {
        hdrm = TD4_X9_PG_HEADROOM_OVERSUB_50G;
    } else if (speed >= 25000) {
        hdrm = TD4_X9_PG_HEADROOM_OVERSUB_25G;
    } else if (speed >= 1000) {
        hdrm = TD4_X9_PG_HEADROOM_OVERSUB_10G;
    } else {
        hdrm = TD4_X9_PG_HEADROOM_OVERSUB_10G;
        LOG_WARN(BSL_LS_BCMTM_INIT,
                 (BSL_META_U(unit,
                             "PG headroom (port: %d) Illegal speed %d\n"),
                  port, speed));
    }
    LOG_VERBOSE(BSL_LS_BCMTM_INIT,
                (BSL_META_U(unit,
                            "PG headroom (port:%d, lossless:%d) hdrm= %d\n"),
                 port, drv_info->lossless, hdrm));
    return hdrm;
}



static int
td4_x9_get_shared_limit(int unit, soc_mmu_cfg_buf_t *buf, int pool_id)
{
    soc_mmu_cfg_buf_pool_t *buf_pool;
    int limit_itm;

    buf_pool = &buf->pools_itm[0][pool_id];
    if (buf_pool->total != 0) {
        limit_itm = buf_pool->total
            - buf_pool->prigroup_headroom
            - buf_pool->prigroup_guarantee
            - buf_pool->port_guarantee
            - buf_pool->queue_group_guarantee
            - buf_pool->queue_guarantee
            - buf_pool->mcq_entry_reserved;
        LOG_VERBOSE(BSL_LS_BCMTM_INIT,
                (BSL_META_U(unit,
                            "Shared limit(pool_id:%d):\n"
                            "Total cell memory = %d\n"
                            "Total PG headroom = %d\n"
                            "Total PGMin guarantee = %d\n"
                            "Total Port guarantee = %d\n"
                            "Total QGrpMin guarantee = %d\n"
                            "Total QMin guarantee = %d\n"
                            "Total MCQ Entry reserved = %d\n"
                            "Shared Limit = %d\n"),
                 pool_id, buf_pool->total,
                 buf_pool->prigroup_headroom,
                 buf_pool->prigroup_guarantee,
                 buf_pool->port_guarantee,
                 buf_pool->queue_group_guarantee,
                 buf_pool->queue_guarantee,
                 buf_pool->mcq_entry_reserved,
                 limit_itm));
    } else {
        limit_itm = 0;
    }
    return limit_itm;
}


static int
td4_x9_get_hdrm_limit(int unit, soc_mmu_cfg_buf_t *buf, int pool_id)
{
    soc_mmu_cfg_buf_pool_t *buf_pool;
    int hdrm;

    buf_pool = &buf->pools_itm[0][pool_id];
    hdrm = buf_pool->prigroup_headroom;
    LOG_VERBOSE(BSL_LS_BCMTM_INIT,
            (BSL_META_U(unit,
                        "Headroom limit (pool:%d) = %d\n"), pool_id, hdrm));
    return hdrm;
}

static void
td4_x9_mmu_config_mapping_profiles_set(int unit, soc_mmu_cfg_buf_t *buf,
                                    soc_mmu_device_info_t *devcfg, int lossless)
{
    soc_mmu_cfg_buf_mapping_profile_t *buf_map_profiles;
    int idx, profile_idx, inpri2pg;


    for (profile_idx = 0; profile_idx < SOC_MMU_CFG_NUM_PROFILES_MAX;
            profile_idx++) {
        buf_map_profiles = &buf->mapprofiles[profile_idx];
        for (idx = 0; idx < SOC_MMU_CFG_INT_PRI_MAX; idx++) {
            if (lossless == 2) {
                /* Lossy+Lossless mode */
                if (idx == 0) {
                    inpri2pg = 7;
                } else if (idx >= 7) {
                    inpri2pg = 0;
                } else {
                    inpri2pg = idx;
                }
            } else if (lossless == 1) {
                /* Lossless mode */
                inpri2pg = 7;
            } else {
                /* Lossy mode */
                if (idx < 7) {
                    inpri2pg = idx;
                } else {
                    inpri2pg = 7;
                }
            }
            buf_map_profiles->inpri_to_prigroup_uc[idx] = inpri2pg;
            buf_map_profiles->inpri_to_prigroup_mc[idx] = inpri2pg;
        }

        if (profile_idx == 0) {
            /* By default only one profile is valid */
            buf_map_profiles->valid = 1;
        } else {
            buf_map_profiles->valid = 0;
        }
        for (idx = 0; idx < SOC_MMU_CFG_PRI_GROUP_MAX; idx++) {
            buf_map_profiles->prigroup_to_servicepool[idx] = 0;
            buf_map_profiles->prigroup_to_headroompool[idx] = 0;
            buf_map_profiles->pfcpri_to_prigroup[idx] = 0;
        }
    }
}



static int
td4_x9_mmu_config_mapping_profiles_set_hw(int unit, soc_mmu_cfg_buf_t *buf,
                                       soc_mmu_device_info_t *devcfg, int lossless)
{
    soc_mmu_cfg_buf_mapping_profile_t *buf_map_profiles;
    int idx, loop;
    uint32_t ltmbuf[BCMTM_MAX_ENTRY_WSIZE];
    uint32_t fval = 0;
    /* Requesting LT ID not available */
    bcmltd_sid_t ltid = -1;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t field_pri[SOC_MMU_CFG_INT_PRI_MAX] = {
            INPPRI0_PGf, INPPRI1_PGf, INPPRI2_PGf,
            INPPRI3_PGf, INPPRI4_PGf, INPPRI5_PGf,
            INPPRI6_PGf, INPPRI7_PGf, INPPRI8_PGf,
            INPPRI9_PGf, INPPRI10_PGf, INPPRI11_PGf,
            INPPRI12_PGf, INPPRI13_PGf, INPPRI14_PGf,
            INPPRI15_PGf};
    bcmdrd_fid_t field_hpid[SOC_MMU_CFG_PRI_GROUP_MAX] = {
            PG0_HPIDf, PG1_HPIDf, PG2_HPIDf,
            PG3_HPIDf, PG4_HPIDf, PG5_HPIDf,
            PG6_HPIDf, PG7_HPIDf};
    bcmdrd_fid_t field_spid[SOC_MMU_CFG_PRI_GROUP_MAX] = {
            PG0_SPIDf, PG1_SPIDf, PG2_SPIDf,
            PG3_SPIDf, PG4_SPIDf, PG5_SPIDf,
            PG6_SPIDf, PG7_SPIDf};
    bcmdrd_fid_t field_pfcpri[SOC_MMU_CFG_PRI_GROUP_MAX] = {
            PFCPRI0_PGf, PFCPRI1_PGf, PFCPRI2_PGf,
            PFCPRI3_PGf, PFCPRI4_PGf, PFCPRI5_PGf,
            PFCPRI6_PGf, PFCPRI7_PGf};
    bcmdrd_sid_t sid;
    SHR_FUNC_ENTER(unit);

    /* Per-profile programming. */
    sid = MMU_THDI_UC_INPPRI_PG_PROFILEr;
    for (idx = 0; idx <= pt_index_max(unit, sid); idx++) {
        buf_map_profiles = &buf->mapprofiles[idx];
        if (buf_map_profiles->valid  != 1) {
            continue;
        }
        sal_memset(ltmbuf, 0, sizeof(ltmbuf));
        for (loop = 0; loop < SOC_MMU_CFG_INT_PRI_MAX; loop++) {
            fval = buf_map_profiles->inpri_to_prigroup_uc[loop];
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, sid, field_pri[loop], ltmbuf, &fval));
        }
        BCMTM_PT_DYN_INFO(pt_info, idx, 0);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));
    }

    sid = MMU_THDI_NONUC_INPPRI_PG_PROFILEr;
    for (idx = 0; idx <= pt_index_max(unit, sid); idx++) {
        buf_map_profiles = &buf->mapprofiles[idx];
        if (buf_map_profiles->valid  != 1) {
            continue;
        }
        sal_memset(ltmbuf, 0, sizeof(ltmbuf));
        for (loop = 0; loop < SOC_MMU_CFG_INT_PRI_MAX; loop++) {
            fval = buf_map_profiles->inpri_to_prigroup_mc[loop];
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, sid, field_pri[loop], ltmbuf, &fval));
        }
        BCMTM_PT_DYN_INFO(pt_info, idx, 0);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));
    }

    sid = MMU_THDI_PG_PROFILEr;
    for (idx = 0; idx <= pt_index_max(unit, sid); idx++) {
        buf_map_profiles = &buf->mapprofiles[idx];
        if (buf_map_profiles->valid  != 1) {
            continue;
        }
        sal_memset(ltmbuf, 0, sizeof(ltmbuf));
        for (loop = 0; loop < SOC_MMU_CFG_PRI_GROUP_MAX; loop++) {
            fval = buf_map_profiles->prigroup_to_headroompool[loop];
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, sid, field_hpid[loop], ltmbuf, &fval));
            fval = buf_map_profiles->prigroup_to_servicepool[loop];
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, sid, field_spid[loop], ltmbuf, &fval));
        }
        BCMTM_PT_DYN_INFO(pt_info, idx, 0);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));
    }

    sid = MMU_THDI_PFCPRI_PG_PROFILEr;
    for (idx = 0; idx <= pt_index_max(unit, sid); idx++) {
        buf_map_profiles = &buf->mapprofiles[idx];
        if (buf_map_profiles->valid  != 1) {
            continue;
        }
        sal_memset(ltmbuf, 0, sizeof(ltmbuf));
        for (loop = 0; loop < SOC_MMU_CFG_PRI_GROUP_MAX; loop++) {
            fval = buf_map_profiles->pfcpri_to_prigroup[loop];
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, sid, field_pfcpri[loop], ltmbuf, &fval));
        }
        BCMTM_PT_DYN_INFO(pt_info, idx, 0);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, sid, ltid,&pt_info, ltmbuf));
    }

exit:
    SHR_FUNC_EXIT();
}


void
td4_x9_mmu_config_thdi_set(int unit, int port, soc_mmu_cfg_buf_t *buf,
                        soc_mmu_device_info_t *devcfg,  int lossless)
{
    soc_mmu_cfg_buf_port_t *buf_port;
    soc_mmu_cfg_buf_port_pool_t *buf_port_pool;
    soc_mmu_cfg_buf_prigroup_t *buf_prigroup;
    bcmtm_drv_info_t *drv_info;
    bcmtm_port_map_info_t *port_map;
    int default_mtu_cells;
    int idx;
    int total_pool_size = 0;

    bcmtm_drv_info_get(unit, &drv_info);
    bcmtm_port_map_get(unit, &port_map);

    default_mtu_cells = TD4_X9_MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->default_mtu_size +
                                                       devcfg->mmu_hdr_byte,
                                                       devcfg->mmu_cell_size);
    total_pool_size = devcfg->mmu_total_cell;

    buf_port = &buf->ports[port];

    buf_port->pri_to_prigroup_profile_idx = 0;
    buf_port->prigroup_profile_idx = 0;

    /*
     * Per-Port Per PG settings
     * port_guarantee_enable = 1 implies use PGMin
     * port_guarantee_enable = 0 implies use PortSPMin
     */
    for (idx = 0; idx < TD4_X9_MMU_NUM_PG; idx++) {
        buf_prigroup = &buf_port->prigroups[idx];
        buf_prigroup->user_delay = -1;
        buf_prigroup->switch_delay = -1;
        buf_prigroup->pool_floor = 0;
        buf_prigroup->pool_resume = 0;
        buf_prigroup->port_guarantee_enable = 1;
        if (bcmtm_lport_is_rdb(unit, port)) {
            buf_prigroup->port_guarantee_enable = 0;
        }
        /* Use PGMIN by default */
        buf_prigroup->guarantee = 0;
        buf_prigroup->headroom = 0;
        buf_prigroup->flow_control_enable = 0;
        buf_prigroup->lossless = 0;
        buf_prigroup->pool_scale = -1;
        if ((lossless == 1) || (lossless == 2)) {
            buf_prigroup->pool_scale = 8;
        }
        /* PG7 is lossless PG */
        if (idx == TD4_X9_MMU_NUM_PG - 1) {
            buf_prigroup->guarantee = (lossless == 0) ? 0 : default_mtu_cells;
            buf_prigroup->lossless = (lossless == 0) ? 0 : 1;
            buf_prigroup->headroom = (lossless == 0) ? 0 :
                ((port_map->lport_map[port].cur_speed != -1) ?
                  td4_x9_default_pg_headroom(unit, port) : 0);
            if (bcmtm_lport_is_rdb(unit, port)) {
                buf_prigroup->guarantee =  0;
                buf_prigroup->headroom =  0;
            }
        }
    }

    /* Per-Port Per Pool settings */
    for (idx = 0; idx < TD4_X9_MMU_NUM_POOL; idx++) {
        buf_port_pool = &buf_port->pools[idx];
        buf_port_pool->guarantee = 0;
        buf_port_pool->pool_limit = 0;
        buf_port_pool->pool_resume = 0;
        if (idx == 0) {
            buf_port_pool->pool_limit = total_pool_size;
            buf_port_pool->pool_resume = total_pool_size;

        }
    }
}


void
td4_x9_mmu_config_thdo_set(int unit, int port, soc_mmu_cfg_buf_t *buf,
                        soc_mmu_device_info_t *devcfg, int lossless)
{
    bcmtm_drv_info_t *drv_info;
    soc_mmu_cfg_buf_port_t *buf_port;
    soc_mmu_cfg_buf_port_pool_t *buf_port_pool;
    soc_mmu_cfg_buf_queue_t *buf_queue;
    soc_mmu_cfg_buf_qgroup_t *queue_grp;
    int default_mtu_cells;
    int idx, num_cosq;
    int total_pool_size = 0;


    bcmtm_drv_info_get(unit, &drv_info);

    default_mtu_cells = TD4_X9_MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->default_mtu_size +
                                                       devcfg->mmu_hdr_byte,
                                                       devcfg->mmu_cell_size);

    /* per ITM limit */
    total_pool_size = devcfg->mmu_total_cell;


    queue_grp = &buf->qgroups[port];
    if (!bcmtm_lport_is_cpu(unit, port)) {
        /* CPU is Qmin */
        queue_grp->guarantee = 2 * default_mtu_cells;
        queue_grp->guarantee_mc = 2 * default_mtu_cells;
    }

    /* Per-port settings */
    buf_port = &buf->ports[port];
    for (idx = 0; idx < TD4_X9_MMU_NUM_POOL; idx++) {
        buf_port_pool = &buf_port->pools[idx];
        buf_port_pool->pool_limit = 0;
        buf_port_pool->pool_resume = 0;
        if (idx == 0) {
            buf_port_pool->pool_limit = total_pool_size;
            buf_port_pool->pool_resume = total_pool_size;
        }
    }

    buf_port = &buf->ports[port];
    if (bcmtm_lport_is_cpu(unit, port)) {
        num_cosq = TD4_X9_NUM_QUEUES_CPU_PORT;
    } else {
        num_cosq = TD4_X9_NUM_QUEUES_NON_CPU_PORT;
    }

    for (idx = 0; idx < num_cosq; idx++) {
        buf_queue = &buf_port->queues[idx];
        if (bcmtm_lport_is_cpu(unit, port)) {
            /* CPU PORT */
            buf_queue->qgroup_min_enable = 0;
            buf_queue->guarantee = default_mtu_cells;
        } else {
            buf_queue->qgroup_min_enable = 1;
            buf_queue->guarantee = 0;
        }
        buf_queue->pool_scale = 8;
        buf_queue->discard_enable = 1;
        buf_queue->color_discard_enable = 0;
        /* Resume limits - accounted for 8 cell granularity */
        buf_queue->yellow_limit = 0;
        buf_queue->red_limit = 0;
        buf_queue->pool_resume = 2;
        buf_queue->pool_limit = 0;
        buf_queue->pool_idx = 0;
        buf_queue->use_dyn_limit = 1;
        buf_queue->color_limit_dyn = 1;
    }

    /* Queue to pool mapping */
    for (idx = 0; idx < num_cosq; idx++) {
        buf_port->queues[idx].pool_idx = 0;
    }
}



static void
td4_x9_mmu_config_thresholds_pool_set(int unit, soc_mmu_cfg_buf_t *buf,
                                   soc_mmu_device_info_t *devcfg, int lossless)
{
    bcmtm_drv_info_t *drv_info;
    soc_mmu_cfg_buf_pool_t *buf_pool;
    int default_mtu_cells;
    int itm, pipe, idx;
    int pm;
    int total_headroom_itm[TD4_X9_ITMS_PER_DEV] = {0};
    int total_pgmin_itm[TD4_X9_ITMS_PER_DEV] = {0};
    int total_port_min = 0;
    int total_qgrpmin_itm[TD4_X9_ITMS_PER_DEV] = {0};
    int total_qmin_itm[TD4_X9_ITMS_PER_DEV] = {0};
    int total_num_ports = 0;
    int ports_in_pipe[TD4_X9_PIPES_PER_DEV] = {0};
    int ports_in_pm[TD4_X9_PBLKS_PER_DEV] = {0};
    int itm_pipe_map;

    bcmtm_drv_info_get(unit, &drv_info);

    default_mtu_cells = TD4_X9_MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->default_mtu_size +
                                                   devcfg->mmu_hdr_byte,
                                                   devcfg->mmu_cell_size);

    for (itm = 0; itm < TD4_X9_ITMS_PER_DEV; itm ++) {
       for (idx = 0; idx < TD4_X9_MMU_NUM_POOL; idx++) {
            buf_pool = &buf->pools[idx];
            if (idx == 0) {
                /* 100% scale up by 100 */
                buf_pool->total_mcq_entry = SOC_TD4_X9_MMU_MCQ_ENTRY_PER_ITM;
                buf_pool->total = TD4_X9_MMU_TOTAL_CELLS_PER_ITM;
                /* 100 Percent */
                buf_pool->size = 10000 | MMU_CFG_BUF_PERCENT_FLAG;
            } else {
                buf_pool->total_mcq_entry = 0;
                buf_pool->total = 0;
                buf_pool->size = 0;
            }

            sal_memcpy(&buf->pools_itm[itm][idx], buf_pool,
                sizeof(soc_mmu_cfg_buf_pool_t));
        }

        /*
         * THDI - Calculating the total PGMin and total PG Headroom
         * per ITM
         * if (not flex port)
         * Total PGMIN : pgmin * number of ports in ITM
         * Total Headroom: Sum of (headroom(speed) * number of ports(speed)
         *                 * num_active_pg) for all possible port speeds
         * if (flex port)
         * Total PGMIN : MIN(max ports per PM, Max ports per ITM) * pgmin
         * Total Headroom :
         */
        total_num_ports = 0;
        for (pipe = 0; pipe < TD4_X9_PIPES_PER_DEV; pipe++) {
            ports_in_pipe[pipe] = 0;
        }
        for (pm = 0; pm < TD4_X9_PBLKS_PER_DEV; pm++) {
            ports_in_pm[pm] = 0;
        }
        itm_pipe_map = itm_ipipe_map(unit, itm);
        if ((lossless == 1) || (lossless == 2)) {

            /* pg min for fp, cpu, lb, mgmt, mmu_pg_min = 8 */
            /* All PMs will be considered Flex eligible PMs. */
            /* Calculate total headroom. */
            for (pipe = 0; pipe < TD4_X9_PIPES_PER_DEV; pipe++) {
                if (itm_pipe_map & (1 <<pipe)) {
                    for (pm = pipe * TD4_X9_PBLKS_PER_PIPE;
                            pm < (pipe + 1) * TD4_X9_PBLKS_PER_PIPE;
                            pm++) {
                        ports_in_pm[pm] = drv_info->flex_eligible[pm];
                        if (ports_in_pm[pm] > 4) {
                            /* Treat as 8x50G PM. */
                            total_headroom_itm[itm] += TD4_X9_PG_HEADROOM_OVERSUB_50G * 8;
                        } else if (ports_in_pm[pm] > 2) {
                            /* Treat as 4x100G PM. */
                            total_headroom_itm[itm] += TD4_X9_PG_HEADROOM_OVERSUB_100G * 4;
                        } else if (ports_in_pm[pm] == 2) {
                            /* Treat as 2x200G PM. */
                            total_headroom_itm[itm] += TD4_X9_PG_HEADROOM_OVERSUB_200G * 2;
                        } else if (ports_in_pm[pm] == 1) {
                            /* Treat as 1x400G PM. */
                            total_headroom_itm[itm] += TD4_X9_PG_HEADROOM_OVERSUB_400G;
                        }
                        ports_in_pipe[pipe] += ports_in_pm[pm];
                    }
                    /* Each pipe cannot have more than 18 ports. */
                    ports_in_pipe[pipe] = MIN(ports_in_pipe[pipe], 18);
                    total_num_ports += ports_in_pipe[pipe];
                }
            }
            /* Add management and LB ports for total ports using PG min */
            total_num_ports += TD4_X9_NUM_CPU_PORTS +
                (TD4_X9_NUM_LB_PORTS + TD4_X9_NUM_MGMT_PORTS) / TD4_X9_ITMS_PER_DEV;

            total_pgmin_itm[itm] = total_num_ports * default_mtu_cells ;
            total_port_min = 120;

        } else {
            total_headroom_itm[itm]= 0;
            total_pgmin_itm[itm] = 0;
        }
    }
    /*
     * total_headroom = MAX(total_headroom_itm[0], total_headroom_itm[1]);
     * total_pgmin = MIN(total_pgmin_itm[0], total_pgmin_itm[1]);
     */
    for (itm = 0; itm < TD4_X9_ITMS_PER_DEV; itm ++) {
        buf_pool = &buf->pools_itm[itm][0];
        buf_pool->prigroup_headroom = total_headroom_itm[itm];
        buf_pool->prigroup_guarantee = total_pgmin_itm[itm];
        buf_pool->port_guarantee = total_port_min;
    }

    for (itm = 0; itm < TD4_X9_ITMS_PER_DEV; itm ++) {
        total_num_ports = 0;
        for (pipe = 0; pipe < TD4_X9_PIPES_PER_DEV; pipe++) {
            ports_in_pipe[pipe] = 0;
        }
        for (pm = 0; pm < TD4_X9_PBLKS_PER_DEV; pm++) {
            ports_in_pm[pm] = 0;
        }
        /*
         * THD0 - Calculating the total QGRPMin
         * if (not flex port)
         * Total QGrpMin : qgrpmin * number of ports
         * if (flex port)
         * Total QGrpMin : MIN(max ports per PM, Max ports per ITM) * qgrpmin
         */
        for (pipe = 0; pipe < TD4_X9_PIPES_PER_DEV; pipe++) {
            for (pm = pipe * TD4_X9_PBLKS_PER_PIPE; pm < (pipe + 1) * TD4_X9_PBLKS_PER_PIPE; pm++) {
                ports_in_pm[pm] = drv_info->flex_eligible[pm];
                ports_in_pipe[pipe] += ports_in_pm[pm];
            }
            /* Each pipe cannot have more than 18 ports. */
            ports_in_pipe[pipe] = MIN(ports_in_pipe[pipe], 18);
        }
        for (pipe = 0; pipe < TD4_X9_PIPES_PER_DEV; pipe++) {
            total_num_ports += ports_in_pipe[pipe];
        }
        total_qgrpmin_itm[itm] = (total_num_ports + TD4_X9_NUM_LB_PORTS +
                                  TD4_X9_NUM_MGMT_PORTS) * (2 * default_mtu_cells);

        /* qmin for cpu * number of cpu queues */
        total_qmin_itm[itm] = default_mtu_cells * SOC_TD4_X9_NUM_CPU_QUEUES;
    }


    for (itm = 0; itm < TD4_X9_ITMS_PER_DEV; itm ++) {
        buf_pool = &buf->pools_itm[itm][0];
        buf_pool->queue_group_guarantee = total_qgrpmin_itm[itm];
        buf_pool->queue_guarantee = total_qmin_itm[itm];
        /* Total qmin reserved for MC_CQE is 0 */
        buf_pool->mcq_entry_reserved = total_qgrpmin_itm[itm];
    } /* itm */

}


static void
td4_x9_mmu_config_thresholds_set(int unit, soc_mmu_cfg_buf_t *buf,
                              soc_mmu_device_info_t *devcfg, int lossless)
{
    int port;

    for (port = 0; port < SOC_MMU_CFG_PORT_MAX; port++) {
        td4_x9_mmu_config_thdi_set(unit, port, buf, devcfg, lossless);
        td4_x9_mmu_config_thdo_set(unit, port, buf, devcfg, lossless);
    }

    td4_x9_mmu_config_thresholds_pool_set(unit, buf, devcfg, lossless);
}


static int
td4_x9_mmu_config_thdi_per_itm_set_hw(int unit, soc_mmu_cfg_buf_t *buf,
                                   soc_mmu_device_info_t *devcfg, int lossless)
{
    uint32_t fval;
    bcmtm_pt_info_t pt_info = {0};
    int idx, itm;
    int jumbo_frame_cells;
    int limit, headroom;
    int pool_resume = 0;
    soc_mmu_cfg_buf_pool_t *buf_pool;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;

    SHR_FUNC_ENTER(unit);

    jumbo_frame_cells = TD4_X9_MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->jumbo_pkt_size +
                                                       devcfg->mmu_hdr_byte,
                                                       devcfg->mmu_cell_size);

    /* Per-ITM programming */
    for (itm = 0; itm < TD4_X9_ITMS_PER_DEV; itm ++) {
        /* Loop to write all instances. */
        fval = TD4_X9_MMU_PHYSICAL_CELLS_PER_ITM - TD4_X9_MMU_RSVD_CELLS_CFAP_PER_ITM;
        BCMTM_PT_DYN_INFO(pt_info, 0, itm);
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, MMU_CFAP_FULLTHRESHOLDSETr, CFAPFULLSETPOINTf,
                             &ltmbuf, &fval));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, MMU_CFAP_FULLTHRESHOLDSETr, ltid,
                                    (void*)&pt_info, &ltmbuf));

        ltmbuf = 0;
        fval = 2 * jumbo_frame_cells;
        BCMTM_PT_DYN_INFO(pt_info, 0, itm);
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, MMU_CFAP_FULL_RESUME_OFFSETr, OFFSETf,
                             &ltmbuf,
                             &fval));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, MMU_CFAP_FULL_RESUME_OFFSETr, ltid,
                                    (void*)&pt_info, &ltmbuf));

        ltmbuf = 0;

        BCMTM_PT_DYN_INFO(pt_info, 0, itm);
        fval = TD4_X9_CFAP_BANK_FULL_LIMIT;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, MMU_CFAP_BANKFULLr, LIMITf, &ltmbuf,
                             &fval));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, MMU_CFAP_BANKFULLr, ltid,
                                    (void*)&pt_info, &ltmbuf));

        /* Input thresholds */
        ltmbuf = 0;
        fval = TD4_X9_LAST_PKT_ACCEPT_MODE_DEFAULT;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, MMU_THDI_POOL_CONFIGr,
                             LAST_PKT_ACCEPT_MODEf, &ltmbuf, &fval));
        fval = TD4_X9_COLOR_AWARE_DEFAULT;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, MMU_THDI_POOL_CONFIGr, COLOR_AWAREf,
                             &ltmbuf, &fval));
        BCMTM_PT_DYN_INFO(pt_info, 0, itm);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, MMU_THDI_POOL_CONFIGr, ltid,
                                    (void*)&pt_info, &ltmbuf));
    }

    /* BACC_TYPE = CHIP, ACC_TYPE = DUPLICATE. */
    ltmbuf = 0;
    fval = TD4_X9_SPID_OVERRIDE_ENABLE_DEFAULT;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, MMU_THDI_CPU_SPID_OVERRIDE_CTRLr, ENABLEf,
                         &ltmbuf, &fval));
    BCMTM_PT_DYN_INFO(pt_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, MMU_THDI_CPU_SPID_OVERRIDE_CTRLr, ltid,
                       (void*)&pt_info, &ltmbuf));

    ltmbuf = 0;
    fval = TD4_X9_SPID_OVERRIDE_ENABLE_DEFAULT;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, MMU_THDI_MIRROR_SPID_OVERRIDE_CTRLr, ENABLEf,
                         &ltmbuf, &fval));
    BCMTM_PT_DYN_INFO(pt_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, MMU_THDI_MIRROR_SPID_OVERRIDE_CTRLr, ltid,
                       (void*)&pt_info, &ltmbuf));

    ltmbuf = 0;
    fval = TD4_X9_SPID_OVERRIDE_ENABLE_DEFAULT;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, MMU_THDI_MC_SPID_OVERRIDE_CTRLr, ENABLEf,
                         &ltmbuf, &fval));
    BCMTM_PT_DYN_INFO(pt_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, MMU_THDI_MC_SPID_OVERRIDE_CTRLr, ltid,
                       (void*)&pt_info, &ltmbuf));


    /* Per-ITM Per-Pool programming */
    for (itm = 0; itm < TD4_X9_ITMS_PER_DEV; itm ++) {
        /* Loop to write all instances. */
        for (idx = 0; idx < TD4_X9_MMU_NUM_POOL; idx++) {
            buf_pool = &buf->pools_itm[0][idx];
            limit = td4_x9_get_shared_limit(unit, buf, idx);
            headroom = td4_x9_get_hdrm_limit(unit, buf, idx);

            if (buf_pool->total != 0) {
                pool_resume = 2 * jumbo_frame_cells;
            } else {
                pool_resume = 0;
            }

            ltmbuf = 0;
            fval = headroom;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, MMU_THDI_HDRM_BUFFER_CELL_LIMIT_HPr,
                                 LIMITf, &ltmbuf, &fval));
            BCMTM_PT_DYN_INFO(pt_info, idx, itm);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit,
                                        MMU_THDI_HDRM_BUFFER_CELL_LIMIT_HPr, ltid,
                                        (void*)&pt_info, &ltmbuf));

            ltmbuf = 0;
            fval = limit;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, MMU_THDI_BUFFER_CELL_LIMIT_SPr,
                                 LIMITf, &ltmbuf, &fval));
            BCMTM_PT_DYN_INFO(pt_info, idx, itm);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, MMU_THDI_BUFFER_CELL_LIMIT_SPr,
                                        ltid,
                                        (void*)&pt_info, &ltmbuf));

            ltmbuf = 0;
            fval = pool_resume;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, MMU_THDI_CELL_RESET_LIMIT_OFFSET_SPr,
                                 OFFSETf, &ltmbuf, &fval));
            BCMTM_PT_DYN_INFO(pt_info, idx, itm);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit,
                                        MMU_THDI_CELL_RESET_LIMIT_OFFSET_SPr, ltid,
                                        (void*)&pt_info, &ltmbuf));

            ltmbuf = 0;
            fval = TD4_X9_SPAP_YELLOW_OFFSET_DEFAULT;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, MMU_THDI_CELL_SPAP_YELLOW_OFFSET_SPr,
                                 OFFSETf, &ltmbuf, &fval));
            BCMTM_PT_DYN_INFO(pt_info, idx, itm);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, MMU_THDI_CELL_SPAP_YELLOW_OFFSET_SPr,
                                        ltid,
                                        (void*)&pt_info, &ltmbuf));


            ltmbuf = 0;
            fval = TD4_X9_SPAP_RED_OFFSET_DEFAULT;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, MMU_THDI_CELL_SPAP_RED_OFFSET_SPr,
                                 OFFSETf, &ltmbuf, &fval));
            BCMTM_PT_DYN_INFO(pt_info, idx, itm);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, MMU_THDI_CELL_SPAP_RED_OFFSET_SPr,
                                        ltid,
                                        (void*)&pt_info, &ltmbuf));
        }
    }

exit:
    SHR_FUNC_EXIT();
}


static int
td4_x9_mmu_config_thdo_per_itm_set_hw(int unit, soc_mmu_cfg_buf_t *buf,
                                   soc_mmu_device_info_t *devcfg, int lossless)
{
    soc_mmu_cfg_buf_pool_t *buf_pool;
    uint32_t fval;
    /* Requesting LT ID not available */
    bcmltd_sid_t ltid = -1;
    uint32_t ltmbuf = 0;
    int idx, itm;
    int jumbo_frame_cells;
    bcmtm_pt_info_t pt_info = {0};
    int limit, resume_limit, mcq_resume_limit;
    int pool_resume = 0;
    bcmtm_drv_info_t *drv_info;

    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);

    jumbo_frame_cells = TD4_X9_MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->jumbo_pkt_size +
                                                    devcfg->mmu_hdr_byte,
                                                    devcfg->mmu_cell_size);
    pool_resume = 2 * jumbo_frame_cells;

    /* Per-ITM programming */

    /* Output thresholds */
    for (itm = 0; itm < SOC_MMU_CFG_ITM_MAX; itm ++) {
        /* Loop to write all instances. */
        ltmbuf = 0;
        fval = TD4_X9_LAST_PKT_ACCEPT_MODE_POOL_DEFAULT;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, MMU_THDO_SHARED_DB_POOL_CONFIGr,
                             LAST_PKT_ACCEPT_MODEf, &ltmbuf, &fval));
        fval = TD4_X9_COLOR_AWARE_DEFAULT;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, MMU_THDO_SHARED_DB_POOL_CONFIGr,
                             POOL_COLOR_LIMIT_ENABLEf, &ltmbuf, &fval));
        BCMTM_PT_DYN_INFO(pt_info, 0, itm);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, MMU_THDO_SHARED_DB_POOL_CONFIGr, ltid,
                                    (void*)&pt_info, &ltmbuf));

        ltmbuf = 0;
        fval = TD4_X9_LAST_PKT_ACCEPT_MODE_POOL_DEFAULT;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, MMU_THDO_MC_SHARED_CQE_POOL_CONFIGr,
                             LAST_PKT_ACCEPT_MODEf, &ltmbuf, &fval));
        fval = TD4_X9_COLOR_AWARE_DEFAULT;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, MMU_THDO_MC_SHARED_CQE_POOL_CONFIGr,
                             POOL_COLOR_LIMIT_ENABLEf, &ltmbuf, &fval));
        BCMTM_PT_DYN_INFO(pt_info, 0, itm);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, MMU_THDO_MC_SHARED_CQE_POOL_CONFIGr,
                                    ltid,
                                    (void*)&pt_info, &ltmbuf));


        /* Per-ITM Per-Pool programming */
        for (idx = 0; idx < TD4_X9_MMU_NUM_POOL; idx++) {
            limit = td4_x9_get_shared_limit(unit, buf, idx);
            buf_pool = &buf->pools[idx];

            if (buf_pool->total != 0) {
                resume_limit = limit - pool_resume;
            } else {
                resume_limit = 0;
            }
            if (buf_pool->total_mcq_entry != 0) {
                mcq_resume_limit = buf_pool->total_mcq_entry - pool_resume;
            } else {
                mcq_resume_limit = 0;
            }

            ltmbuf = 0;
            fval = limit;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, MMU_THDO_SHARED_DB_POOL_SHARED_LIMITr,
                                 SHARED_LIMITf, &ltmbuf, &fval));
            BCMTM_PT_DYN_INFO(pt_info, idx, itm);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, MMU_THDO_SHARED_DB_POOL_SHARED_LIMITr,
                                        ltid, (void*)&pt_info, &ltmbuf));

            ltmbuf = 0;
            fval = limit/8;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, MMU_THDO_SHARED_DB_POOL_YELLOW_SHARED_LIMITr,
                                 YELLOW_SHARED_LIMITf, &ltmbuf, &fval));
            BCMTM_PT_DYN_INFO(pt_info, idx, itm);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, MMU_THDO_SHARED_DB_POOL_YELLOW_SHARED_LIMITr,
                                        ltid, (void*)&pt_info, &ltmbuf));

            ltmbuf = 0;
            fval = limit/8;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, MMU_THDO_SHARED_DB_POOL_RED_SHARED_LIMITr,
                                 RED_SHARED_LIMITf, &ltmbuf, &fval));
            BCMTM_PT_DYN_INFO(pt_info, idx, itm);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, MMU_THDO_SHARED_DB_POOL_RED_SHARED_LIMITr,
                                        ltid,
                                        (void*)&pt_info, &ltmbuf));

            ltmbuf = 0;
            fval = resume_limit/8;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, MMU_THDO_SHARED_DB_POOL_RESUME_LIMITr,
                                 RESUME_LIMITf, &ltmbuf, &fval));
            BCMTM_PT_DYN_INFO(pt_info, idx, itm);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, MMU_THDO_SHARED_DB_POOL_RESUME_LIMITr,
                                        ltid,
                                        (void*)&pt_info, &ltmbuf));

            ltmbuf = 0;
            fval = resume_limit/8;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, MMU_THDO_SHARED_DB_POOL_YELLOW_RESUME_LIMITr,
                                 YELLOW_RESUME_LIMITf, &ltmbuf, &fval));
            BCMTM_PT_DYN_INFO(pt_info, idx, itm);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit,
                                        MMU_THDO_SHARED_DB_POOL_YELLOW_RESUME_LIMITr, ltid,
                                        (void*)&pt_info, &ltmbuf));

            ltmbuf = 0;
            fval = resume_limit/8;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, MMU_THDO_SHARED_DB_POOL_RED_RESUME_LIMITr,
                                 RED_RESUME_LIMITf, &ltmbuf, &fval));
            BCMTM_PT_DYN_INFO(pt_info, idx, itm);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit,
                                        MMU_THDO_SHARED_DB_POOL_RED_RESUME_LIMITr, ltid,
                                        (void*)&pt_info, &ltmbuf));


            ltmbuf = 0;
            fval = buf_pool->total_mcq_entry;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, MMU_THDO_MC_SHARED_CQE_POOL_SHARED_LIMITr,
                                 SHARED_LIMITf, &ltmbuf, &fval));
            BCMTM_PT_DYN_INFO(pt_info, idx, itm);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit,
                                        MMU_THDO_MC_SHARED_CQE_POOL_SHARED_LIMITr, ltid,
                                        (void*)&pt_info, &ltmbuf));

            ltmbuf = 0;
            fval = buf_pool->total_mcq_entry/8;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, MMU_THDO_MC_SHARED_CQE_POOL_YELLOW_SHARED_LIMITr,
                                 YELLOW_SHARED_LIMITf, &ltmbuf, &fval));
            BCMTM_PT_DYN_INFO(pt_info, idx, itm);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit,
                                        MMU_THDO_MC_SHARED_CQE_POOL_YELLOW_SHARED_LIMITr, ltid,
                                        (void*)&pt_info, &ltmbuf));

            ltmbuf = 0;
            fval = buf_pool->total_mcq_entry/8;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, MMU_THDO_MC_SHARED_CQE_POOL_RED_SHARED_LIMITr,
                                 RED_SHARED_LIMITf, &ltmbuf, &fval));
            BCMTM_PT_DYN_INFO(pt_info, idx, itm);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit,
                                        MMU_THDO_MC_SHARED_CQE_POOL_RED_SHARED_LIMITr, ltid,
                                        (void*)&pt_info, &ltmbuf));

            ltmbuf = 0;
            fval = mcq_resume_limit/8;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, MMU_THDO_MC_SHARED_CQE_POOL_RESUME_LIMITr,
                                 RESUME_LIMITf, &ltmbuf, &fval));
            BCMTM_PT_DYN_INFO(pt_info, idx, itm);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit,
                                        MMU_THDO_MC_SHARED_CQE_POOL_RESUME_LIMITr, ltid,
                                        (void*)&pt_info, &ltmbuf));

            ltmbuf = 0;
            fval = mcq_resume_limit/8;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit,
                                 MMU_THDO_MC_SHARED_CQE_POOL_YELLOW_RESUME_LIMITr,
                                 YELLOW_RESUME_LIMITf, &ltmbuf, &fval));
            BCMTM_PT_DYN_INFO(pt_info, idx, itm);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit,
                                        MMU_THDO_MC_SHARED_CQE_POOL_YELLOW_RESUME_LIMITr, ltid,
                                        (void*)&pt_info, &ltmbuf));

            ltmbuf = 0;
            fval = mcq_resume_limit/8;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, MMU_THDO_MC_SHARED_CQE_POOL_RED_RESUME_LIMITr,
                                 RED_RESUME_LIMITf, &ltmbuf, &fval));
            BCMTM_PT_DYN_INFO(pt_info, idx, itm);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit,
                                        MMU_THDO_MC_SHARED_CQE_POOL_RED_RESUME_LIMITr, ltid,
                                        (void*)&pt_info, &ltmbuf));
        }
    }


exit:
    SHR_FUNC_EXIT();
}

static void
soc_mmu_cfg_free(int unit, soc_mmu_cfg_buf_t *cfg)
{
    sal_free(cfg);
}


soc_mmu_cfg_buf_t*
bcm56780_a0_mmu_buf_cfg_alloc(int unit)
{
    soc_mmu_cfg_buf_t *buf;
    soc_mmu_cfg_buf_queue_t *buf_queue;
    int num_cosq, alloc_size=0;
    int port;

    alloc_size = sizeof(soc_mmu_cfg_buf_t);
    for (port = 0; port < SOC_MMU_CFG_PORT_MAX; port++) {
        if (bcmtm_lport_is_cpu(unit, port)) {
            num_cosq = TD4_X9_NUM_QUEUES_CPU_PORT;
        } else {
            num_cosq = TD4_X9_NUM_QUEUES_NON_CPU_PORT;
        }
        alloc_size += sizeof(soc_mmu_cfg_buf_queue_t) * num_cosq;
    }
    buf = sal_alloc(alloc_size, "bcmtmThdBufCfg");
    if (buf == NULL) {
        return NULL;
    }
    sal_memset(buf, 0, alloc_size);
    buf_queue = (soc_mmu_cfg_buf_queue_t *)&buf[1];
    for (port = 0; port < SOC_MMU_CFG_PORT_MAX; port++) {
        if (bcmtm_lport_is_cpu(unit, port)) {
            num_cosq = TD4_X9_NUM_QUEUES_CPU_PORT;
        } else {
            num_cosq = TD4_X9_NUM_QUEUES_NON_CPU_PORT;
        }
        if (num_cosq) {
            buf->ports[port].queues = buf_queue;
            buf_queue += num_cosq;
        }
    }
    return buf;
}

static int
buf_size_tracking_init(int unit, soc_mmu_cfg_buf_t *buf)
{
    int itm, pool, pool_buf_size, buf_size;
    soc_mmu_cfg_buf_pool_t *buf_pool;
    bcmtm_thd_info_t *thd_info;

    bcmtm_thd_info_get(unit, &thd_info);

    for (itm = 0; itm < TD4_X9_ITMS_PER_DEV; itm ++) {
        buf_size = 0;
        for (pool = 0; pool < SOC_MMU_CFG_SERVICE_POOL_MAX; pool++) {
            buf_pool = &buf->pools_itm[itm][pool];
            if (buf_pool->total != 0) {
                pool_buf_size = buf_pool->total
                              - buf_pool->prigroup_headroom
                              - buf_pool->prigroup_guarantee
                              - buf_pool->port_guarantee
                              - buf_pool->queue_group_guarantee
                              - buf_pool->queue_guarantee
                              - buf_pool->mcq_entry_reserved;
            } else {
                pool_buf_size = 0;
            }
            buf_size += pool_buf_size;
        }
        thd_info->itm_buf_size[itm] = buf_size;
    }

    return SHR_E_NONE;
}

static uint32_t
calculate_fraction(uint32_t num, uint32_t nominator, uint32_t denominator)
{
    double result;

    result = num;
    result *= nominator;
    result /= denominator;

    return result < 0 ? result - 0.5 : result + 0.5;
}

/*******************************************************************************
 * Public functions
 */
/*
 * \Initialize mmu thresholds for THDI/THDO
 *
 * Setup mmu threshold related mapping profile
 * Setup mmu ITM threshold for THDI/THDO
 *
 * \param [in] unit Unit number.
 * \param [in] test_only Test only.
 * \param [in] lossless Chip lossless mode
 *
 * \retval SHR_E_* all the possible shr error.
 */
int
bcm56780_a0_tm_mmu_config_init_thresholds(int unit)
{
    soc_mmu_cfg_buf_t *buf;
    soc_mmu_device_info_t devcfg;
    bcmtm_drv_info_t *drv_info;

    SHR_FUNC_ENTER(unit);
    buf = bcm56780_a0_mmu_buf_cfg_alloc(unit);
    if (!buf) {
        return SHR_E_MEMORY;
    }
    bcmtm_drv_info_get(unit, &drv_info);

    LOG_VERBOSE(BSL_LS_BCMTM_INIT,
                (BSL_META_U(unit,
                            "MMU config init: lossless = %d \n"), drv_info->lossless));

    td4_x9_mmu_init_dev_config(unit, &devcfg, drv_info->lossless);

    /* Default config */
    td4_x9_mmu_config_mapping_profiles_set(unit, buf, &devcfg, drv_info->lossless);
    td4_x9_mmu_config_thresholds_set(unit, buf, &devcfg, drv_info->lossless);
    SHR_IF_ERR_EXIT(
            td4_x9_mmu_config_mapping_profiles_set_hw(unit, buf, &devcfg,
                drv_info->lossless));
    SHR_IF_ERR_EXIT
        (td4_x9_mmu_config_thdi_per_itm_set_hw(unit, buf, &devcfg, drv_info->lossless));
    SHR_IF_ERR_EXIT
        (td4_x9_mmu_config_thdo_per_itm_set_hw(unit, buf, &devcfg, drv_info->lossless));

    buf_size_tracking_init(unit, buf);


    LOG_VERBOSE(BSL_LS_BCMTM_INIT,
                (BSL_META_U(unit,
                            "MMU THDI/THDO init done\n")));


exit:
    soc_mmu_cfg_free(unit, buf);
    SHR_FUNC_EXIT();
}

/*
 * \Initialize mmu thresholds for THDR
 *
 * Setup mmu threshold for THDR
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_* all the possible shr error.
 */

int
bcm56780_a0_tm_rqe_threshold_init(int unit) {

    uint32_t fval,idx;
    /* Requesting LT ID not available */
    bcmltd_sid_t ltid = -1;
    uint32_t ltmbuf = 0;
    bcmdrd_sid_t reg = INVALIDr;
    bcmtm_pt_info_t pt_info = {0};

    SHR_FUNC_ENTER(unit);
    for(idx = 0; idx < TD4_X9_MMU_RQE_QUEUE_NUM; idx++) {
        reg = MMU_THDR_QE_CONFIG1_PRIQr;
        ltmbuf = TD4_X9_THDR_QE_CONFIG1_VALUE;
        BCMTM_PT_DYN_INFO(pt_info, idx, -1);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, reg, ltid,
                       (void*)&pt_info, &ltmbuf));

        reg = MMU_THDR_QE_LIMIT_MIN_PRIQr;
        ltmbuf = 0;
        BCMTM_PT_DYN_INFO(pt_info, idx, 0);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, reg, ltid,
                       (void*)&pt_info, &ltmbuf));
        fval = TD4_X9_THDR_QE_LIMIT_MIN;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, MIN_LIMITf, &ltmbuf, &fval));
        BCMTM_PT_DYN_INFO(pt_info, idx, -1);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, reg, ltid,
                       (void*)&pt_info, &ltmbuf));

        reg = MMU_THDR_QE_RESET_OFFSET_COLOR_PRIQr;
        ltmbuf = 0;
        BCMTM_PT_DYN_INFO(pt_info, idx, 0);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, reg, ltid,
                       (void*)&pt_info, &ltmbuf));
        fval = TD4_X9_MMU_THDR_QE_RESET_OFFSET;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, RESET_OFFSET_YELLOWf, &ltmbuf, &fval));
        fval = TD4_X9_MMU_THDR_QE_RESET_OFFSET;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, RESET_OFFSET_REDf, &ltmbuf, &fval));
        BCMTM_PT_DYN_INFO(pt_info, idx, -1);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, reg, ltid,
                       (void*)&pt_info, &ltmbuf));

        reg = MMU_THDR_QE_CONFIG_PRIQr;
        ltmbuf = 0;
        BCMTM_PT_DYN_INFO(pt_info, idx, 0);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, reg, ltid,
                       (void*)&pt_info, &ltmbuf));
        fval = TD4_X9_MMU_THDR_QE_RESET_OFFSET;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, RESET_OFFSETf, &ltmbuf, &fval));
        fval = TD4_X9_THDR_QE_CONIFG_PRI_LIMIT;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, SHARED_LIMITf, &ltmbuf, &fval));
        fval = TD4_X9_THDR_QE_CONIFG_PRI_LIMIT;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, SHARED_ALPHAf, &ltmbuf, &fval));
        BCMTM_PT_DYN_INFO(pt_info, idx, -1);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, reg, ltid,
                       (void*)&pt_info, &ltmbuf));


    }

    /* BASE_TYPE = CHIP, ACC_TYPE = DUPLICATE. */
    idx = 0;
    reg = MMU_THDR_QE_CONFIGr;
    ltmbuf = 0;
    BCMTM_PT_DYN_INFO(pt_info, idx, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid,
                      (void*)&pt_info, &ltmbuf));
    fval = 1;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, LAST_PKT_ACCEPT_MODEf, &ltmbuf, &fval));
    BCMTM_PT_DYN_INFO(pt_info, idx, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid,
                      (void*)&pt_info, &ltmbuf));

    reg = MMU_THDR_QE_CONFIG_SPr;
    ltmbuf = 0;
    BCMTM_PT_DYN_INFO(pt_info, idx, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid,
                      (void*)&pt_info, &ltmbuf));
    fval = TD4_X9_THDR_QE_SP_SHARED_LIMIT;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, SHARED_LIMITf, &ltmbuf, &fval));
    fval = TD4_X9_THDR_QE_SP_RESUME_LIMIT;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, RESUME_LIMITf, &ltmbuf, &fval));
    BCMTM_PT_DYN_INFO(pt_info, idx, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid,
                      (void*)&pt_info, &ltmbuf));

    reg = MMU_THDR_QE_SHARED_COLOR_LIMIT_SPr;
    ltmbuf = 0;
    BCMTM_PT_DYN_INFO(pt_info, idx, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid,
                      (void*)&pt_info, &ltmbuf));
    fval = TD4_X9_THDR_QE_SP_SHARED_LIMIT;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, SHARED_LIMIT_YELLOWf, &ltmbuf, &fval));
    fval = TD4_X9_THDR_QE_SP_SHARED_LIMIT;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, SHARED_LIMIT_REDf, &ltmbuf, &fval));
    BCMTM_PT_DYN_INFO(pt_info, idx, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid,
                      (void*)&pt_info, &ltmbuf));

    reg = MMU_THDR_QE_RESUME_COLOR_LIMIT_SPr;
    ltmbuf = 0;
    fval = TD4_X9_THDR_QE_SP_RESUME_LIMIT;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, RESUME_LIMIT_YELLOWf, &ltmbuf, &fval));
    fval = TD4_X9_THDR_QE_SP_RESUME_LIMIT;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, RESUME_LIMIT_REDf, &ltmbuf, &fval));
    BCMTM_PT_DYN_INFO(pt_info, idx, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid,
                      (void*)&pt_info, &ltmbuf));

exit:
    SHR_FUNC_EXIT();
}

int
bcm56780_a0_tm_thdi_pg_to_pool_mapping_get(int unit, int lport, int pg,
                                           int *itm, int *pool, bool is_hdrm)
{
    bcmdrd_sid_t ptid;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    bcmdrd_fid_t fid;
    bcmtm_pt_info_t pt_info = {0};
    uint32_t ltmbuf[2] = {0};
    uint32_t fval, profile;
    int mmu_port;
    bcmdrd_fid_t field_hpid[SOC_MMU_CFG_PRI_GROUP_MAX] = {
            PG0_HPIDf, PG1_HPIDf, PG2_HPIDf,
            PG3_HPIDf, PG4_HPIDf, PG5_HPIDf,
            PG6_HPIDf, PG7_HPIDf};
    bcmdrd_fid_t field_spid[SOC_MMU_CFG_PRI_GROUP_MAX] = {
            PG0_SPIDf, PG1_SPIDf, PG2_SPIDf,
            PG3_SPIDf, PG4_SPIDf, PG5_SPIDf,
            PG6_SPIDf, PG7_SPIDf};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_lport_mport_get(unit, lport, &mmu_port));
    *itm = 0;
    /* Get PG profile for the port. */
    ptid = MMU_THDI_ING_PORT_CONFIGr;
    BCMTM_PT_DYN_INFO(pt_info, 0, mmu_port);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, ltid, &pt_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, ptid, PG_PROFILE_SELf, ltmbuf, &fval));
    profile = fval;

    /* Find service pool ID for the PG in the profile. */
    ptid = MMU_THDI_PG_PROFILEr;
    BCMTM_PT_DYN_INFO(pt_info, profile, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, ltid, &pt_info, ltmbuf));
    if (is_hdrm) {
        fid = field_hpid[pg];
    } else {
        fid = field_spid[pg];
    }
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, ptid, fid, ltmbuf, &fval));
    *pool = fval;

exit:
    SHR_FUNC_EXIT();
}

int
bcm56780_a0_tm_shared_buffer_update(int unit, int itm, int pool, long int delta,
                                    shared_buffer_update_reason_t reason,
                                    bool check_only)
{
    bcmdrd_sid_t ptid;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    bcmtm_pt_info_t pt_info = {0};
    uint32_t ltmbuf = 0;
    uint32_t fval, new_limit, unused_buf, denominator;
    long int remaining;
    int pool_idx, pool_start, pool_end;
    uint32_t pool_shared_limit[SOC_MMU_CFG_SERVICE_POOL_MAX] = {0};
    uint32_t new_limit_array[SOC_MMU_CFG_SERVICE_POOL_MAX] = {0};
    bcmtm_thd_info_t *thd_info;

    SHR_FUNC_ENTER(unit);

    bcmtm_thd_info_get(unit, &thd_info);

    if (delta == 0) {
        return SHR_E_NONE;
    }
    /* Get shared buffer pool allocation. */
    unused_buf = thd_info->itm_buf_size[itm];
    ptid = MMU_THDI_BUFFER_CELL_LIMIT_SPr;
    for (pool_idx = 0; pool_idx < SOC_MMU_CFG_SERVICE_POOL_MAX; pool_idx++) {
        BCMTM_PT_DYN_INFO(pt_info, pool_idx, itm);
        ltmbuf = 0;
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, ptid, ltid, &pt_info, &ltmbuf));
        SHR_IF_ERR_EXIT
            (bcmtm_field_get(unit, ptid, LIMITf, &ltmbuf, &fval));
        pool_shared_limit[pool_idx] = fval;
        unused_buf -= fval;
    }

    denominator = thd_info->itm_buf_size[itm] - unused_buf;

    /* Check if the delta can be achieved in the buffer. */
    if (reason == RESERVED_BUFFER_UPDATE) {
        /* Update is triggered by change made to reserved resource. */
        if (pool == -1) {
            /*
             * Reserved resource is not associated to a pool. Adjust all service
             * pools proportionally.
             */
            if ((long int) thd_info->itm_buf_size[itm] + delta >= 0) {
                if (check_only) {
                    return SHR_E_NONE;
                }
                thd_info->itm_buf_size[itm] += delta;
                if (delta < 0 && (long int) unused_buf + delta >= 0) {
                    /* Allocate unused buffer space. No need to update pool limits. */
                    return SHR_E_NONE;
                } else {
                    if (delta < 0) {
                        delta += unused_buf;
                    }
                    /* Calculate per pool new limit. */
                    new_limit = denominator + delta;
                    remaining = new_limit;
                    for (pool_idx = SOC_MMU_CFG_SERVICE_POOL_MAX - 1;
                         pool_idx > 0;
                         pool_idx--) {
                        new_limit_array[pool_idx]
                            = calculate_fraction(new_limit,
                                                 pool_shared_limit[pool_idx],
                                                 denominator);
                        remaining -= new_limit_array[pool_idx];
                        /* Cumulated rounding error may cause negative remaining. */
                        if (remaining < 0) {
                            new_limit_array[pool_idx] += remaining;
                            remaining = 0;
                            break;
                        }
                    }
                    new_limit_array[pool_idx] = remaining;
                }
            } else {
                return SHR_E_PARAM;
            }
        } else {
            if (delta > 0) {
                if (check_only) {
                    return SHR_E_NONE;
                }
                thd_info->itm_buf_size[itm] += delta;
                new_limit = pool_shared_limit[pool] + delta;
            } else if ((long int) pool_shared_limit[pool] + unused_buf + delta >= 0) {
                if (check_only) {
                    return SHR_E_NONE;
                }
                thd_info->itm_buf_size[itm] += delta;
                new_limit = pool_shared_limit[pool] + unused_buf + delta;
                if (new_limit >= pool_shared_limit[pool]) {
                    /* User has already made the space for the reserved resources. */
                    return SHR_E_NONE;
                }
            } else {
                return SHR_E_PARAM;
            }
        }
    } else {
        /*
         * Update context is pool shared limit adjustment.
         * No check needed if pool limit is going to decrease.
         * Check against unused buffer size if pool limit is going to increase.
         */
        if (delta > (long long int)unused_buf) {
            /* Do not have enough space to allocate the delta. */
            return SHR_E_PARAM;
        }
        if (check_only) {
            return SHR_E_NONE;
        }
        new_limit = pool_shared_limit[pool] + delta;
    }

    if (pool == -1) {
        /* Will loop through all pools. */
        pool_start = 0;
        pool_end = SOC_MMU_CFG_SERVICE_POOL_MAX - 1;
    } else {
        pool_start = pool_end = pool;
        new_limit_array[pool] = new_limit;
    }

    for (; pool_start <= pool_end; pool_start++) {
        BCMTM_PT_DYN_INFO(pt_info, pool_start, itm);
        new_limit = new_limit_array[pool_start];

        /* Ingress pool shared settings. */
        ptid = MMU_THDI_BUFFER_CELL_LIMIT_SPr;
        ltmbuf = 0;
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, ptid, ltid, &pt_info, &ltmbuf));
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, ptid, LIMITf, &ltmbuf, &new_limit));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, ptid, ltid, &pt_info, &ltmbuf));

        /* Update and notify the corresponding LT. */
        if (reason != ING_SERVICE_POOL_UPDATE) {
            /*
             * If the caller is ingress service pool LT IMM handler, the handler
             * will adjust all the limits in ingress service pool, so no need to
             * call the following function (It is supposed to be called by other
             * LT handler to update ingress service pool).
             */
            SHR_IF_ERR_EXIT
                (bcm56780_a0_tm_ing_pool_shared_limit_lt_update(unit, itm, pool_start,
                                                                new_limit));
        }

        /* Egress Pool shared settings. */
        ptid = MMU_THDO_SHARED_DB_POOL_SHARED_LIMITr;
        ltmbuf = 0;
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, ptid, ltid, &pt_info, &ltmbuf));
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, ptid, SHARED_LIMITf, &ltmbuf, &new_limit));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, ptid, ltid, &pt_info, &ltmbuf));

        /* Update and notify the corresponding LT. */
        if (reason != EGR_SERVICE_POOL_UPDATE) {
            /*
             * If the caller is egress service pool LT IMM handler, the handler
             * will adjust all the limits in egress service pool, so no need to
             * call the following function (It is supposed to be called by other
             * LT handler to update egress service pool).
             */
            SHR_IF_ERR_EXIT
                (bcm56780_a0_tm_egr_pool_shared_limit_lt_update(unit, itm, pool_start,
                                                                new_limit));
        }
    }

exit:
    SHR_FUNC_EXIT();
}


