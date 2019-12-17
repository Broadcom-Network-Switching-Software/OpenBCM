/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        port.c
 * Purpose:     TH3 SOC Port driver.
 */
#include <shared/bsl.h>
#include <soc/error.h>
#include <soc/defs.h>
#include <soc/tdm/core/tdm_top.h>

#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <soc/init/tomahawk3_tdm.h>
#include <soc/tomahawk3.h>
#include <soc/scache.h>

#ifdef BCM_WARM_BOOT_SUPPORT

#define SOC_FLEXPORT_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define SOC_FLEXPORT_WB_DEFAULT_VERSION            SOC_FLEXPORT_WB_VERSION_1_0

extern int
_soc_th3_port_speed_cap[SOC_MAX_NUM_DEVICES][_TH3_DEV_PORTS_PER_DEV];
#endif

/*
 * Function:
 *      soc_th3_port_schedule_tdm_init
 * Purpose:
 *      Initialize TDM information in port_schedule_state that will be passed
 *      to DV FlexPort API
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      port_schedule_state  - (OUT) Port resource state.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      If called during Scheduler Initialization, only below values need to be
 *      properly set:
 *        soc_tdm_schedule_pipe_t::num_slices
 *        soc_tdm_schedule_t:: cal_len
 *        soc_tdm_schedule_t:: num_ovs_groups
 *        soc_tdm_schedule_t:: ovs_group_len
 *        soc_tdm_schedule_t:: num_pkt_sch_or_ovs_space
 *        soc_tdm_schedule_t:: pkt_sch_or_ovs_space_len 
 */
int
soc_th3_port_schedule_tdm_init(int unit,
                            soc_port_schedule_state_t *port_schedule_state)
{
#if 1
    
    soc_control_t *soc = SOC_CONTROL(unit);
    _soc_tomahawk3_tdm_t *tdm = soc->tdm_info;
    soc_tdm_schedule_pipe_t *tdm_ischd, *tdm_eschd;
    soc_tdm_schedule_t *sched;
    int pipe, hpipe, is_flexport;

    if (tdm == NULL) {
        /*XGSSIM will skip tdm configuration on TH3*/
        if (SAL_BOOT_SIMULATION) {
            return SOC_E_NONE;
        }
        return SOC_E_INIT;
    }

    is_flexport = port_schedule_state->is_flexport;

    /* Construct tdm_ingress_schedule_pipe and tdm_egress_schedule_pipe */
    for (pipe = 0; pipe <  _TH3_PIPES_PER_DEV; pipe++) {
        tdm_ischd = &port_schedule_state->tdm_ingress_schedule_pipe[pipe];
        tdm_eschd = &port_schedule_state->tdm_egress_schedule_pipe[pipe];

        tdm_ischd->num_slices = _TH3_OVS_HPIPE_COUNT_PER_PIPE;
        tdm_eschd->num_slices = _TH3_OVS_HPIPE_COUNT_PER_PIPE;

        if (is_flexport) {
            /* TDM Calendar is always in slice 0 */
            sal_memcpy(tdm_ischd->tdm_schedule_slice[0].linerate_schedule,
                        tdm->tdm_pipe[pipe].idb_tdm,
                        sizeof(int)*_TH3_TDM_LENGTH);
            sal_memcpy(tdm_eschd->tdm_schedule_slice[0].linerate_schedule,
                        tdm->tdm_pipe[pipe].mmu_tdm,
                        sizeof(int)*_TH3_TDM_LENGTH);
        }

        /* OVS */
        for (hpipe = 0; hpipe < _TH3_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            /* IDB OVERSUB*/
            sched = &tdm_ischd->tdm_schedule_slice[hpipe];
            sched->cal_len = _TH3_TDM_LENGTH;

/* TDM says everything related to hpipe and pblk can be removed except
 * sched->cal_len = _TH3_TDM_LENGTH; */
#if 0
            sched->num_ovs_groups = _TH3_OVS_GROUP_COUNT_PER_HPIPE;
            sched->ovs_group_len = _TH3_OVS_GROUP_TDM_LENGTH;
            sched->num_pkt_sch_or_ovs_space = 1;
            sched->pkt_sch_or_ovs_space_len = _TH3_PKT_SCH_LENGTH;

            if (is_flexport) {
                for (group = 0; group < _TH3_OVS_GROUP_COUNT_PER_HPIPE; group++) {
                    for (slot = 0; slot < _TH3_OVS_GROUP_TDM_LENGTH; slot++) {
                        sched->oversub_schedule[group][slot] =
                            tdm->tdm_pipe[pipe].ovs_tdm[hpipe][group][slot];
                    }
                }
                for (slot = 0; slot < _TH3_PKT_SCH_LENGTH; slot++) {
                    sched->pkt_sch_or_ovs_space[0][slot] = 
                        tdm->tdm_pipe[pipe].pkt_shaper_tdm[hpipe][slot];
                }
            }

            /* MMU OVERSUB */
            sched = &tdm_eschd->tdm_schedule_slice[hpipe];
            sched->cal_len = _TH3_TDM_LENGTH;
            sched->num_ovs_groups = _TH3_OVS_GROUP_COUNT_PER_HPIPE;
            sched->ovs_group_len = _TH3_OVS_GROUP_TDM_LENGTH;
            sched->num_pkt_sch_or_ovs_space = 1;
            sched->pkt_sch_or_ovs_space_len = _TH3_PKT_SCH_LENGTH;

            if (is_flexport) {
                for (group = 0; group < _TH3_OVS_GROUP_COUNT_PER_HPIPE; group++) {
                    for (slot = 0; slot < _TH3_OVS_GROUP_TDM_LENGTH; slot++) {
                        sched->oversub_schedule[group][slot] =
                            tdm->tdm_pipe[pipe].ovs_tdm[hpipe][group][slot];
                        }
                    }
                for (slot = 0; slot < _TH3_PKT_SCH_LENGTH; slot++) {
                    sched->pkt_sch_or_ovs_space[0][slot] = 
                        tdm->tdm_pipe[pipe].pkt_shaper_tdm[hpipe][slot];
                }
            }
#endif
        }
    }

/* TDM says everything related to hpipe and pblk can be removed except
 * sched->cal_len = _TH3_TDM_LENGTH; */
#if 0
    if (is_flexport) {
        port_p2PBLK_inst_mapping =
            port_schedule_state->in_port_map.port_p2PBLK_inst_mapping;
        /* pblk info for phy_port */
        for (phy_port = 1; phy_port < TH3_NUM_EXT_PORTS; phy_port++) {
            if (tdm->pblk_info[phy_port].pblk_cal_idx == -1) {
                continue;
            }
            port_p2PBLK_inst_mapping[phy_port] =
                                        tdm->pblk_info[phy_port].pblk_cal_idx;
        }
    }
#endif
#endif
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_th3_soc_tdm_update
 * Purpose:
 *      Update TDM information in SOC with TDM state return from DV FlexPort API
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      port_schedule_state  - (OUT) Port resource state.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
soc_th3_soc_tdm_update(int unit,
                            soc_port_schedule_state_t *port_schedule_state)
{
#if 1
    
    soc_control_t *soc = SOC_CONTROL(unit);
    _soc_tomahawk3_tdm_t *tdm = soc->tdm_info;
    soc_tdm_schedule_pipe_t *tdm_ischd, *tdm_eschd;
    int pipe, cdport;

    if (tdm == NULL) {
        /*XGSSIM will skip tdm configuration on TH3*/
        if (SAL_BOOT_XGSSIM) {
            return SOC_E_NONE;
        }
        return SOC_E_INIT;
    }

    /* Copy info from soc_port_schedule_state_t to _soc_tomahawk3_tdm_t */
    for (pipe = 0; pipe <  _TH3_PIPES_PER_DEV; pipe++) {
        tdm_ischd = &port_schedule_state->tdm_ingress_schedule_pipe[pipe];
        tdm_eschd = &port_schedule_state->tdm_egress_schedule_pipe[pipe];

        /* TDM Calendar is always in slice 0 */
        sal_memcpy(tdm->tdm_pipe[pipe].idb_tdm,
                    tdm_ischd->tdm_schedule_slice[0].linerate_schedule,
                    sizeof(int)*_TH3_TDM_LENGTH);
        sal_memcpy(tdm->tdm_pipe[pipe].mmu_tdm,
                    tdm_eschd->tdm_schedule_slice[0].linerate_schedule,
                    sizeof(int)*_TH3_TDM_LENGTH);
/* TDM says can be removed */
#if 0
        for (hpipe = 0; hpipe < _TH3_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            sched = &tdm_ischd->tdm_schedule_slice[hpipe];
            for (group = 0; group < _TH3_OVS_GROUP_COUNT_PER_HPIPE; group++) {
                for (slot = 0; slot < _TH3_OVS_GROUP_TDM_LENGTH; slot++) {
                    tdm->tdm_pipe[pipe].ovs_tdm[hpipe][group][slot] =
                        sched->oversub_schedule[group][slot];
                }
            }
            for (slot = 0; slot < _TH3_PKT_SCH_LENGTH; slot++) {
                tdm->tdm_pipe[pipe].pkt_shaper_tdm[hpipe][slot] =
                    sched->pkt_sch_or_ovs_space[0][slot];

            }
        }
#endif
    }
/* TDM says pblk stuff can be removed */
#if 0
    /* pblk info init  */
    for (phy_port = 1; phy_port < TH3_NUM_EXT_PORTS; phy_port++) {
        tdm->pblk_info[phy_port].pblk_cal_idx = -1;
        tdm->pblk_info[phy_port].pblk_hpipe_num = -1;
    }

    /* pblk info for phy_port */
    for (pipe = 0; pipe < _TH3_PIPES_PER_DEV; pipe++) {
        tdm_ischd = &port_schedule_state->tdm_ingress_schedule_pipe[pipe];
        for (hpipe = 0; hpipe < _TH3_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            for (group = 0; group < _TH3_OVS_GROUP_COUNT_PER_HPIPE; group++) {
                for (index = 0; index < _TH3_OVS_GROUP_TDM_LENGTH; index++) {
                    phy_port =
                        tdm_ischd->tdm_schedule_slice[hpipe].oversub_schedule[group][index];
                    if (phy_port < TH3_NUM_EXT_PORTS) {
                        tdm->pblk_info[phy_port].pblk_hpipe_num = hpipe;
                        tdm->pblk_info[phy_port].pblk_cal_idx =
                            port_schedule_state->out_port_map.port_p2PBLK_inst_mapping[phy_port];
                    }
                }
            }
        }
    }
#endif

    /* Port Macros in TH3 have 2 MACs.  Mac 0 is in bits 0-3, Mac 1 is in bits 4-7 */
    /* CDPORT port ratio */
    for (cdport = 0; cdport < _TH3_PBLKS_PER_DEV; cdport++) {
        soc_tomahawk3_port_ratio_get_schedule_state(unit, port_schedule_state, cdport,
            &tdm->port_ratio[cdport], 1);
    }
/* TDM says should be removed or completely redefined for TH3 */
#if 0
    /* Calculate the oversub ratio */
    for (pipe = 0; pipe < _TH3_PIPES_PER_DEV; pipe++) {
        /* At MAX Frequency OVS is always 3:2 */
        if (si->frequency == 1700) {
            for (hpipe = 0; hpipe < _TH3_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
                tdm->ovs_ratio_x1000[pipe][hpipe] = 1500;
            }
            continue;
        }

        tdm_ischd = &port_schedule_state->tdm_ingress_schedule_pipe[pipe];
        /* Count number of OVSB_TOKEN assigned by the TDM code */
        for (length = _TH3_TDM_LENGTH; length > 0; length--) {
            if (tdm_ischd->tdm_schedule_slice[0].linerate_schedule[length - 1]
                    != TH3_NUM_EXT_PORTS) {
                break;
            }
        }
        ovs_core_slot_count = 0;
        for (index = 0; index < length; index++) {
            if (tdm_ischd->tdm_schedule_slice[0].linerate_schedule[index]
                    == TH3_OVSB_TOKEN) {
                ovs_core_slot_count++;
            }
        }

        /* At line-rate with less than max frequency, OVS picks the 2:1 */
        if (ovs_core_slot_count == 0) {
            for (hpipe = 0; hpipe < _TH3_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
                tdm->ovs_ratio_x1000[pipe][hpipe] = 2000;
            }
        }

        /* Count number of slot needed for half-pipe's oversub I/O bandwidth */
        for (hpipe = 0; hpipe < _TH3_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            ovs_io_slot_count = 0;
            for (group = 0; group < _TH3_OVS_GROUP_COUNT_PER_HPIPE; group++) {
                for (index = 0; index < _TH3_OVS_GROUP_TDM_LENGTH; index++) {
                    phy_port = tdm_ischd->tdm_schedule_slice[hpipe].oversub_schedule[group][index];
                    if (phy_port == TH3_NUM_EXT_PORTS) {
                        continue;
                    }
                    port = si->port_p2l_mapping[phy_port];
                    if ((port == -1) || IS_CPU_PORT(unit, port) ||
                        IS_LB_PORT(unit, port) ||
                        IS_MANAGEMENT_PORT(unit, port)) {
                        LOG_ERROR(BSL_LS_SOC_PORT,
                                  (BSL_META_U(unit,
                                              "Flexport: Invalid physical "
                                              "port %d in OverSub table.\n"),
                                   phy_port));
                        continue;
                    }
                    port_slot_count = si->port_init_speed[port] / 2500;
                    ovs_io_slot_count += port_slot_count;
                }
            }
            if (ovs_core_slot_count != 0) {
                tdm->ovs_ratio_x1000[pipe][hpipe] =
                    ovs_io_slot_count * 1000 / (ovs_core_slot_count / 2);
            }
        }
    }
#endif
#endif
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_th3_support_speeds
 * Purpose:
 *      Get the supported speed of port for specified lanes
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      lanes                - (IN)  Number of Lanes for the port.
 *      speed_mask           - (IN)  Bitmap for supported speed.
 * Returns:
 *      BCM_E_xxx
 * Notes: This is NOT supported because PM8x50 is not supported in
 *        bcm_port_ability_t
 */
int
soc_th3_support_speeds(int unit, int lanes, uint32 *speed_mask)
{
    *speed_mask = 0;

    return SOC_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
int soc_th3_flexport_scache_allocate(int unit)
{
    int rv = SOC_E_NONE;
    uint8 *flexport_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32 alloc_get = 0;
    uint32 alloc_size = 0;
    int stable_size;
    int default_ver = SOC_FLEXPORT_WB_DEFAULT_VERSION;

    alloc_size =  (sizeof(int) * _TH3_PHY_PORTS_PER_DEV)  + /* phy to logical*/
                   (sizeof(int) * _TH3_DEV_PORTS_PER_DEV) + /* logical to phy */
                   (sizeof(int) * _TH3_DEV_PORTS_PER_DEV) + /* max port speed */
                   (sizeof(int) * _TH3_DEV_PORTS_PER_DEV) + /* init port speed */
                   (sizeof(int) * _TH3_DEV_PORTS_PER_DEV) + /* num of lanes */
                   (sizeof(int) * _TH3_DEV_PORTS_PER_DEV) + /* port fec*/
                   (sizeof(int) * _TH3_DEV_PORTS_PER_DEV) + /* port phy lane config */
                   (sizeof(int) * _TH3_DEV_PORTS_PER_DEV) + /* port link training */
                   (sizeof(pbmp_t));                        /* Disabled bitmap */
    alloc_get = alloc_size;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_FLEXIO_HANDLE, 0);
    /* check to see if an scache table has been configured */
    rv = soc_stable_size_get(unit, &stable_size);
    if (SOC_FAILURE(rv) || stable_size <= 0) {
        return rv;
    }

    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                                          TRUE, &alloc_get,
                                          &flexport_scache_ptr,
                                          default_ver,
                                          NULL);

    if (rv  == SOC_E_CONFIG) {
        /* Probably Level1 */
        return SOC_E_NONE;
    }
    /* NotRequired but just to confirm Get the pointer for the Level 2 cache */
    if (alloc_get != alloc_size) {
        /* Expected size doesn't match retrieved size */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "ERROR: scache memory for flexport size mismatch"
                 "%s()[LINE:%d] DONE \n"),FUNCTION_NAME(),  __LINE__));
        return SOC_E_INTERNAL;
    }

    if (NULL == flexport_scache_ptr) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "ERROR: scache memory not allocated for flexport"
                 "%s()[LINE:%d] DONE \n"),FUNCTION_NAME(),  __LINE__));
        return SOC_E_MEMORY;
    }
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                "%s()[LINE:%d] DONE \n"),FUNCTION_NAME(),  __LINE__));
    return SOC_E_NONE;

}

/*
 * Function:
 *      soc_th3_flexport_scache_sync
 * Purpose:
 *      Record Port info that maybe changed during flexport for Level 2 WB
 *
 * Warm Boot Version Map:
 *
 *  BCM_WB_VERSION_1_0
 *    port_p2l_mapping
 *    port_l2p_mapping
 *    port_speed_max
 *    port_init_speed
 *    port_num_lanes
 *    disabled_bitmap
 *
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      SOC_E_XXX
 */
int soc_th3_flexport_scache_sync(int unit)
{
    uint8 *flexport_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32 alloc_get = 0;
    uint32 alloc_size = 0;
    uint32 var_size = 0;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 scache_offset=0;
    int rv = 0;

    alloc_size =  (sizeof(int) * _TH3_PHY_PORTS_PER_DEV)  + /* phy to logical*/
                   (sizeof(int) * _TH3_DEV_PORTS_PER_DEV) + /* logical to phy */
                   (sizeof(int) * _TH3_DEV_PORTS_PER_DEV) + /* max port speed */
                   (sizeof(int) * _TH3_DEV_PORTS_PER_DEV) + /* init port speed */
                   (sizeof(int) * _TH3_DEV_PORTS_PER_DEV) + /* num of lanes */
                   (sizeof(int) * _TH3_DEV_PORTS_PER_DEV) + /* port fec*/
                   (sizeof(int) * _TH3_DEV_PORTS_PER_DEV) + /* port phy lane config */
                   (sizeof(int) * _TH3_DEV_PORTS_PER_DEV) + /* port link training */
                   (sizeof(pbmp_t));                        /* Disabled bitmap */

    alloc_get = alloc_size;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_FLEXIO_HANDLE, 0);
    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                        FALSE, &alloc_get,
                        &flexport_scache_ptr,
                        SOC_FLEXPORT_WB_DEFAULT_VERSION,
                        NULL);
    if (rv == SOC_E_NOT_FOUND) {
        /* Probably Level1 */
        return SOC_E_NONE;
    }
    if (alloc_get != alloc_size) {
        /* Expected size doesn't match retrieved size */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "ERROR: scache memory for flexport size mismatch"
                 "%s()[LINE:%d] DONE \n"),FUNCTION_NAME(),  __LINE__));
        return SOC_E_INTERNAL;
    }
    if (NULL == flexport_scache_ptr) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "ERROR: scache memory not allocated for flexport"
                 "%s()[LINE:%d] DONE \n"),FUNCTION_NAME(),  __LINE__));
        return SOC_E_MEMORY;
    }

    /* Physical to logical port mapping */
    var_size = sizeof(int) * _TH3_PHY_PORTS_PER_DEV;
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_p2l_mapping, var_size);
    scache_offset += var_size;

    var_size = sizeof(int) * _TH3_DEV_PORTS_PER_DEV;

    /* Logical to Physical port mapping */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_l2p_mapping, var_size);
    scache_offset += var_size;

    /* Max port speed */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_speed_max, var_size);
    scache_offset += var_size;

    /* Init port speed */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_init_speed, var_size);
    scache_offset += var_size;

    /* Num of lanes */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_num_lanes, var_size);
    scache_offset += var_size;

    /* Fec */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_fec_type, var_size);
    scache_offset += var_size;

    /* Phy Lane Config */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_phy_lane_config, var_size);
    scache_offset += var_size;

    /* Link Training */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_link_training, var_size);
    scache_offset += var_size;

    /* Disabled Port Bitmap */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               &si->all.disabled_bitmap,
               sizeof(pbmp_t));
    scache_offset += sizeof(pbmp_t);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "%s()[LINE:%d] \n"),FUNCTION_NAME(),  __LINE__));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_th3_flexport_scache_recovery
 * Purpose:
 *      Recover Port info that maybe changed during flexport for Level 2 WB
 *
 * Warm Boot Version Map:
 *
 *  BCM_WB_VERSION_1_0
 *    port_p2l_mapping
 *    port_l2p_mapping
 *    port_speed_max
 *    port_init_speed
 *    port_num_lanes
 *    disabled_bitmap
 *
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      SOC_E_XXX
 */
int soc_th3_flexport_scache_recovery(int unit)
{
    uint32 alloc_get = 0;
    uint32 alloc_size = 0;
    int rv = SOC_E_NONE;
    uint8 *flexport_scache_ptr = NULL;
    soc_scache_handle_t scache_handle;
    uint32 scache_offset=0;
    uint32 var_size = 0;
    uint16 recovered_ver = 0;
    soc_info_t *si = &SOC_INFO(unit);

    alloc_size =  (sizeof(int) * _TH3_PHY_PORTS_PER_DEV)  + /* phy to logical*/
                   (sizeof(int) * _TH3_DEV_PORTS_PER_DEV) + /* logical to phy */
                   (sizeof(int) * _TH3_DEV_PORTS_PER_DEV) + /* max port speed */
                   (sizeof(int) * _TH3_DEV_PORTS_PER_DEV) + /* init port speed */
                   (sizeof(int) * _TH3_DEV_PORTS_PER_DEV) + /* num of lanes */
                   (sizeof(int) * _TH3_DEV_PORTS_PER_DEV) + /* port fec*/
                   (sizeof(int) * _TH3_DEV_PORTS_PER_DEV) + /* port phy lane config */
                   (sizeof(int) * _TH3_DEV_PORTS_PER_DEV) + /* port link training */
                   (sizeof(pbmp_t));                        /* Disabled bitmap */

    alloc_get = alloc_size;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_FLEXIO_HANDLE, 0);
    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                        FALSE, &alloc_get,
                        &flexport_scache_ptr,
                        SOC_FLEXPORT_WB_DEFAULT_VERSION,
                        &recovered_ver);
    if (SOC_FAILURE(rv)) {
        if ((rv == SOC_E_CONFIG) ||
            (rv == SOC_E_NOT_FOUND)) {
            /* warmboot file does not contain this
            * module, or the warmboot state does not exist.
            * in this case return SOC_E_NOT_FOUND
            */
            return SOC_E_NOT_FOUND;
        } else {
            /* Only Level2 - flexport treat this as a error */
            LOG_ERROR(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                     "Failed to recover scache data - %s\n"),soc_errmsg(rv)));
            return rv;
        }
    }
    if (alloc_get != alloc_size) {
        /* Expected size doesn't match retrieved size */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "ERROR: scache recovery for flexport"
                 "%s()[LINE:%d] DONE \n"),
                  FUNCTION_NAME(),  __LINE__));
        return SOC_E_INTERNAL;
    }

    if (NULL == flexport_scache_ptr) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "ERROR: scache recovery for flexport"
                 "%s()[LINE:%d] DONE \n"),
                  FUNCTION_NAME(),  __LINE__));
        return SOC_E_MEMORY;
    }

    /*Physical to logical port mapping */
    var_size = (sizeof(int) * _TH3_PHY_PORTS_PER_DEV);
    sal_memcpy(si->port_p2l_mapping,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    var_size = (sizeof(int) * _TH3_DEV_PORTS_PER_DEV);

    /*Logical to Physical port mapping*/
    sal_memcpy(si->port_l2p_mapping,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* Max port speed */
    sal_memcpy(_soc_th3_port_speed_cap[unit],
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* Init port speed */
    sal_memcpy(si->port_speed_max,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* Num of lanes */
    sal_memcpy(si->port_num_lanes,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* Fec */
    sal_memcpy(si->port_fec_type,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* Phy Lane Config */
    sal_memcpy(si->port_phy_lane_config,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* Link Training */
    sal_memcpy(si->port_link_training,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* Disabled Port Bitmap */
    sal_memcpy(&si->all.disabled_bitmap,
           &flexport_scache_ptr[scache_offset],
           sizeof(pbmp_t));
    scache_offset += sizeof(pbmp_t);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "%s()[LINE:%d] \n"),FUNCTION_NAME(),  __LINE__));

    return SOC_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

void
soc_th3_flexport_sw_dump(int unit)
{
    int port, phy_port, mmu_port, pipe, pm, cosq, numq, uc_cosq, uc_numq;
    int max_speed, init_speed, num_lanes, fec, link_training, phy_lane_config;
    char  pfmt[SOC_PBMP_FMT_LEN];

    LOG_CLI((BSL_META_U(unit,
                            "  port(log/phy/mmu)  pipe  pm  lanes    "
                            "speed(Max)    fec  link train  lane config  "
                            "uc_Qbase/Numq mc_Qbase/Numq\n")));
    PBMP_ALL_ITER(unit, port) {
        pipe = SOC_INFO(unit).port_pipe[port];
        phy_port = SOC_INFO(unit).port_l2p_mapping[port];
        mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];
        num_lanes = SOC_INFO(unit).port_num_lanes[port];
        pm = SOC_INFO(unit).port_serdes[port];
        max_speed = SOC_INFO(unit).port_speed_max[port];
        init_speed = SOC_INFO(unit).port_init_speed[port];
        cosq = SOC_INFO(unit).port_cosq_base[port];
        numq = SOC_INFO(unit).port_num_cosq[port];
        uc_cosq = SOC_INFO(unit).port_uc_cosq_base[port];
        uc_numq = SOC_INFO(unit).port_num_uc_cosq[port];
        fec = SOC_INFO(unit).port_fec_type[port];
        link_training = SOC_INFO(unit).port_link_training[port];
        phy_lane_config = SOC_INFO(unit).port_phy_lane_config[port];

        LOG_CLI((BSL_META_U(unit,
                            "  %4s(%3d/%3d/%3d)  %4d  %2d  %5d %7d(%7d) %3d"
                            "%10d %11d %6d/%-6d  %6d/%-6d\n"),
                SOC_INFO(unit).port_name[port], port, phy_port, mmu_port,
                pipe, pm, num_lanes, init_speed, max_speed, fec, link_training,
                phy_lane_config, uc_cosq, uc_numq, cosq, numq));
    }

    LOG_CLI((BSL_META_U(unit,
                        "\n    Oversub Bitmap: %s"),
       SOC_PBMP_FMT(SOC_INFO(unit).oversub_pbm, pfmt)));
    LOG_CLI((BSL_META_U(unit,
                        "\n    Disabled Bitmap: %s \n"),
       SOC_PBMP_FMT(SOC_INFO(unit).all.disabled_bitmap, pfmt)));
}

/*
 * Function:
 *      soc_th3_port_mode_get
 * Description:
 *      Get port mode by giving first logical port of TSC4
 * Parameters:
 *      unit          - Device number
 *      port          - Logical port
 *      int*          - Port Mode
 *
 * Each TSC4 can be configured into following 5 mode:
 *   Lane number    0    1    2    3
 *   ------------  ---  ---  ---  ---
 *      quad port  25G  25G  25G  25G
 *   tri_012 port  25G  25G  50G   x
 *   tri_023 port  50G   x   25G  25G
 *      dual port  50G   x   50G   x
 *    single port  100G  x    x    x
 */
int
soc_th3_port_mode_get(int unit, int logical_port, int *mode)
{
#if 0
    
    soc_info_t *si;
    int lane;
    int port, first_phyport, phy_port;
    int num_lanes[_TH2_PORTS_PER_PBLK];

    si = &SOC_INFO(unit);
    first_phyport = si->port_l2p_mapping[logical_port];

    for (lane = 0; lane < _TH2_PORTS_PER_PBLK; lane++) {
        phy_port = first_phyport + lane;
        port = si->port_p2l_mapping[phy_port];
        if (port == -1 || SOC_PBMP_MEMBER(si->all.disabled_bitmap, port)) {
            num_lanes[lane] = -1;
        } else {
            num_lanes[lane] = si->port_num_lanes[port];
        }
    }

    if (num_lanes[0] == 4) {
        *mode = SOC_TH2_PORT_MODE_QUAD;
    } else if (num_lanes[0] == 2 && num_lanes[2] == 2) {
        *mode = SOC_TH2_PORT_MODE_DUAL;
    } else if (num_lanes[0] == 2 && num_lanes[2] == 1 && num_lanes[3] == 1) {
        *mode = SOC_TH2_PORT_MODE_TRI_023;
    } else if (num_lanes[0] == 1 && num_lanes[1] == 1 && num_lanes[2] == 2) {
        *mode = SOC_TH2_PORT_MODE_TRI_012;
    } else {
        *mode = SOC_TH2_PORT_MODE_SINGLE;
    }
#endif
    return SOC_E_NONE;
}

#endif /* BCM_TOMAHAWK3_SUPPORT */
