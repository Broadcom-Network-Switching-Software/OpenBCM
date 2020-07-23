/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        trident3_tdm.c
 * Purpose:
 * Requires:
 */


#include <shared/bsl.h>
#include <soc/tdm/core/tdm_top.h>

#include <soc/defs.h>
#include <soc/mem.h>

#if defined(BCM_TRIDENT3_SUPPORT)
#include <soc/trident3.h>
#include <soc/trident3_tdm.h>
#include <soc/tdm/trident3/tdm_td3_defines.h>


/*** START SDK API COMMON CODE ***/

/*! @fn void _soc_td3_tdm_print_port_map(
        int unit,
        soc_port_map_type_t *port_map)
    @param unit Chip unit number.
    @param port_map Pointer to a soc_port_map_type_t struct variable.
    @brief Debug API to print soc_port_map_type_t
 */
void
_soc_td3_tdm_print_port_map(int unit, soc_port_map_type_t *port_map)
{
    int port;
    int is_hg2, is_ovs, is_mgm;
    int speed, phy_port, idb_port, mmu_port, num_lanes, pblk;
    int phy_bmap, p2l_map, m2p_map;

    for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
        is_mgm = SOC_PBMP_MEMBER(port_map->management_pbm, port) ? 1 : 0;
        if ((is_mgm == 1) || (port_map->log_port_speed[port] > 0)) {
            speed     = is_mgm ? 10 : port_map->log_port_speed[port]/1000;
            phy_port  = port_map->port_l2p_mapping[port];
            idb_port  = port_map->port_l2i_mapping[port];
            mmu_port  = port_map->port_p2m_mapping[phy_port];
            num_lanes = port_map->port_num_lanes[port];
            pblk      = port_map->port_p2PBLK_inst_mapping[phy_port];
            phy_bmap  = SOC_PBMP_MEMBER(port_map->physical_pbm, phy_port) ? 1 : 0;
            is_hg2    = SOC_PBMP_MEMBER(port_map->hg2_pbm, port) ? 1 : 0;
            is_ovs    = SOC_PBMP_MEMBER(port_map->oversub_pbm, port) ? 1 : 0;
            p2l_map   = port_map->port_p2l_mapping[phy_port];
            m2p_map   = port_map->port_m2p_mapping[mmu_port];
            LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                                "log_port=%3d phy_port=%3d speed=%3dG "
                                "%s %s idb_port=%3d mmu_port=%3d "
                                "num_lanes=%1d pblk=%3d phy_bmap=%1d "
                                "p2l_map=%3d "),
                                port, phy_port, speed,
                                (is_hg2==1) ? "HG2" : "ETH",
                                (is_ovs==1) ? "OVS" : " LR",
                                idb_port, mmu_port, num_lanes, pblk,
                                phy_bmap, p2l_map));
            LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "m2p_map=%3d %s\n"),
                                m2p_map, (is_mgm==1) ? "MGM" : "" ));
        }
    }

}


/*! @fn void _soc_td3_tdm_print_tdm_info(
        int unit,
        soc_tdm_schedule_t *tdm_info,
        int hpipe,
        int is_ing)
    @param unit Chip unit number.
    @param tdm_info Pointer to a soc_tdm_schedule_t struct variable.
    @param hpipe Half Pipe.
    @param is_ing distinguishes between IP and EP schedulers
    @brief Debug API to print soc_tdm_schedule_t
 */
void
_soc_td3_tdm_print_tdm_info(int unit, soc_tdm_schedule_t *tdm_info,
                            int hpipe, int is_ing)
{
    int cal_len, j, m;

    if (hpipe == 0) {
        for (cal_len = tdm_info->cal_len; cal_len > 0; cal_len--) {
            if (tdm_info->linerate_schedule[cal_len - 1] != TD3_NUM_EXT_PORTS) {
                break;
            }
        }

        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                            "\tMAIN CALENDAR: cal_len=%3d \n"),
                            cal_len ));
        for (j=0; j < cal_len; j++) {
            if (j%16==0) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                                    "\n %3d : %3d\t"),
                                    j, j+15));
            }
            LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                                "%5d"),
                                tdm_info->linerate_schedule[j]));
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                        "\n\n\tOVS TABLES: HPIPE= %3d \n"),
                        hpipe ));
    for (j=0; j< tdm_info->num_ovs_groups; j++) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                            "\nOVS_CAL[%d]:\t"),
                            j ));
        for (m=0; m< tdm_info->ovs_group_len; m++) {
            if (tdm_info->oversub_schedule[j][m] != TD3_NUM_EXT_PORTS) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                                    "%d\t"),
                                    tdm_info->oversub_schedule[j][m] ));
            } else {
                LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                                    "---\t")));
            }
        }
    }

    if (is_ing == 1) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                            "\n\n\tPKT_SCHEDULER: HPIPE= %3d \n"),
                            hpipe ));
        for (j=0; j<tdm_info->pkt_sch_or_ovs_space_len; j++) {
            if (j%16==0) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                                    "\n %3d : %3d\t"),
                                    j, j+15 ));
            }
            if (tdm_info->pkt_sch_or_ovs_space[0][j] != TD3_NUM_EXT_PORTS) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                                    "\t%d"),
                                    tdm_info->pkt_sch_or_ovs_space[0][j] ));
            } else {
                LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                                    "\t---")));
            }
        }
    }
    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,"\n")));
}


/*! @fn void _soc_td3_tdm_print_port_resource(
        int unit,
        soc_port_resource_t *port_info,
        int entry_num)
    @param unit Chip unit number.
    @param port_info Pointer to a soc_port_resource_t struct variable.
    @param entry_num entry number.
    @brief Debug API to print soc_port_resource_t
 */
void
_soc_td3_tdm_print_port_resource(int unit,
                                 soc_port_resource_t *port_info,
                                 int entry_num)
{
    int is_hg2, lane;

    is_hg2 = 0;
    if (port_info->encap == _SHR_PORT_ENCAP_HIGIG2) {
        is_hg2=1;
    }

    if (port_info->physical_port == -1) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                            "nport=%3d DOWN: "
                            "logical_port=%3d physical_port=%3d\n"),
                            entry_num, port_info->logical_port,
                            port_info->physical_port));
    } else {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                            "nport=%3d  UP : "
                            "logical_port=%3d physical_port=%3d mmu_port=%3d "),
                            entry_num, port_info->logical_port,
                            port_info->physical_port,
                            port_info->mmu_port ));
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                            "idb_port=%3d pipe=%3d speed=%3d num_lanes=%1d "
                            "%s  %s\n"),
                            port_info->idb_port,
                            port_info->pipe, port_info->speed/1000,
                            port_info->num_lanes,
                            (is_hg2==1) ? "HG2": "ETH",
                            (port_info->oversub==1) ? "OVS": " LR" ));
        for (lane = 0; lane < SOC_PORT_RESOURCE_LANES_MAX; lane++) {
            if (port_info->lane_info[lane] != NULL) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                                    "nport=%3d lane_info[%d]: pgw=%3d"
                                    "xlp=%3d port_index=%3d\n"),
                                    entry_num, lane,
                                    port_info->lane_info[lane]->pgw,
                                    port_info->lane_info[lane]->xlp,
                                    port_info->lane_info[lane]->port_index));
            }
        }
    }
}


/*! @fn void _soc_td3_tdm_print_schedule_state(
        int unit,
        soc_port_schedule_state_t *sch_info)
    @param unit Chip unit number.
    @param sch_info Pointer to a soc_port_schedule_state_t struct variable.
    @brief Debug API to print sch_info
 */
void
_soc_td3_tdm_print_schedule_state(int unit,
                                  soc_port_schedule_state_t *sch_info)
{
    int i, pipe, hpipe, print_tdm;
    soc_tdm_schedule_t *tdm_schedule;

    print_tdm = 1;

    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                        "\n\nPRINTING soc_port_schedule_state_t \n")));
    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                        "nport= %3d\n"),
                        sch_info->nport));
    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                        "frequency= %4dMHz\n"),
                        sch_info->frequency));
    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                        "bandwidth= %4d\n"),
                        sch_info->bandwidth));
    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                        "io_bandwidth= %4d\n"),
                        sch_info->io_bandwidth));
    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                        "lossless= %1d\n"),
                        sch_info->lossless));
    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                        "is_flexport= %1d\n"),
                        sch_info->is_flexport));

    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                        "\nPRINTING soc_port_schedule_state_t"
                        "::[in_port_map]\n")));
    _soc_td3_tdm_print_port_map(unit, &sch_info->in_port_map);

    if (sch_info->is_flexport == 1) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                            "\nPRINTING soc_port_schedule_state_t"
                            "::[out_port_map]\n")));
        _soc_td3_tdm_print_port_map(unit, &sch_info->out_port_map);
    }

    if (print_tdm == 1) {
        for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
             LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                                 "\n====== TDM Tables PIPE=%1d INGRESS\n"),
                                 pipe));
            for (hpipe = 0; hpipe< MAX_SCH_SLICES; hpipe++) {
                tdm_schedule = &(sch_info->tdm_ingress_schedule_pipe[
											pipe].tdm_schedule_slice[hpipe]);
                _soc_td3_tdm_print_tdm_info(unit, tdm_schedule, hpipe, 1);
            }
            LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                                "\n====== TDM Tables PIPE=%1d  EGRESS\n"),
                                pipe));
            for (hpipe = 0; hpipe< MAX_SCH_SLICES; hpipe++) {
                tdm_schedule = &(sch_info->tdm_egress_schedule_pipe[
											pipe].tdm_schedule_slice[hpipe]);
                _soc_td3_tdm_print_tdm_info(unit, tdm_schedule, hpipe, 0);
            }
        }
    }

    if ((sch_info->is_flexport == 1) && (sch_info->nport > 0)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                            "\n====== FLEXPORT RESOURCES ===== "
                            "num entries= %d = \n"),
                            sch_info->nport));
        for (i = 0; i < sch_info->nport; i++) {
            _soc_td3_tdm_print_port_resource(unit, &sch_info->resource[i], i);
        }
    }
}


/*! @fn void soc_td3_tdm_get_port_ratio(
        int unit,
        soc_port_schedule_state_t *sch_info,
        int pm_num,
        int *pm_mode,
        int prev_or_new)
    @param unit   Chip unit number.
    @param sch_info Pointer to a soc_port_schedule_state_t struct variable.
    @param pm_num PM block_id (0 to 31)
    @param *pm_mode mode to be returned.
    @param prev_or_new function looks in in_port_map or out_port_map.
    @brief Help function to get port ratio.
 */

/*
 * #1   single: 100  -   x  -   x  -   x  SOC_TD3_PORT_RATIO_SINGLE
 * #2   single:  40  -   x  -   x  -   x  SOC_TD3_PORT_RATIO_TRI_023_2_1_1
 * #3   dual:    50  -   x  -  50  -   x  SOC_TD3_PORT_RATIO_DUAL_1_1
 * #4   dual:    40  -   x  -  40  -   x  SOC_TD3_PORT_RATIO_DUAL_1_1
 * #5   dual:    20  -   x  -  20  -   x  SOC_TD3_PORT_RATIO_DUAL_1_1
 * #6   dual:    40  -   x  -  20  -   x  SOC_TD3_PORT_RATIO_DUAL_2_1
 * #7   dual:    20  -   x  -  40  -   x  SOC_TD3_PORT_RATIO_DUAL_1_2
 * #8   tri:     50  -   x  - 25/x - 25/x SOC_TD3_PORT_RATIO_TRI_023_2_1_1
 * #9   tri:     20  -   x  - 10/x - 10/x SOC_TD3_PORT_RATIO_TRI_023_2_1_1
 * #10  tri:     40  -   x  - 10/x - 10/x SOC_TD3_PORT_RATIO_TRI_023_4_1_1
 * #11  tri:    25/x - 25/x -  50  -   x  SOC_TD3_PORT_RATIO_TRI_012_1_1_2
 * #12  tri:    10/x - 10/x -  20  -   x  SOC_TD3_PORT_RATIO_TRI_012_1_1_2
 * #13  tri:    10/x - 10/x -  40  -   x  SOC_TD3_PORT_RATIO_TRI_012_1_1_4
 * #14  quad:   25/x - 25/x - 25/x - 25/x SOC_TD3_PORT_RATIO_QUAD
 * #15  quad:   10/x - 10/x - 10/x - 10/x SOC_TD3_PORT_RATIO_QUAD
 */
void
soc_td3_tdm_get_port_ratio(int unit,
                            soc_port_schedule_state_t *sch_info,
                            int pm_num, int *pm_mode, int prev_or_new)
{
    int port, phy_port_base, lane, mode = 0;
    int num_lanes[TRIDENT3_TDM_PORTS_PER_PBLK];
    int speed_max[TRIDENT3_TDM_PORTS_PER_PBLK];
    soc_port_map_type_t *port_map;
    int l_speed = 0, l_num_lanes = 0;

    sal_memset(num_lanes, 0, TRIDENT3_TDM_PORTS_PER_PBLK*sizeof(int));
    sal_memset(speed_max, 0, TRIDENT3_TDM_PORTS_PER_PBLK*sizeof(int));

    if (prev_or_new == 1) {
        port_map = &(sch_info->in_port_map);
    } else {
        port_map = &(sch_info->out_port_map);
    }

    phy_port_base = 1 + (pm_num * TRIDENT3_TDM_PORTS_PER_PBLK);
    for (lane = 0; lane < TRIDENT3_TDM_PORTS_PER_PBLK; lane += 2) {
        port = port_map->port_p2l_mapping[phy_port_base + lane];
        if ( (port == -1) || (port_map->log_port_speed[port]==0) ) {
            num_lanes[lane] = -1;
            speed_max[lane] = -1;
        } else {
            soc_td3_tdm_get_log_port_speed(unit, sch_info, port,
                prev_or_new, &l_speed, &l_num_lanes);
            num_lanes[lane] = l_num_lanes;
            speed_max[lane] = l_speed;
        }
    }

    if (num_lanes[0] == 4) {
        mode = SOC_TD3_PORT_RATIO_SINGLE;
    } else if (num_lanes[0] == 2 && num_lanes[2] == 2) {
        if (speed_max[0] == speed_max[2]) {
            mode = SOC_TD3_PORT_RATIO_DUAL_1_1;
        } else if (speed_max[0] > speed_max[2]) {
            mode = SOC_TD3_PORT_RATIO_DUAL_2_1;
        } else {
            mode = SOC_TD3_PORT_RATIO_DUAL_1_2;
        }
    } else if (num_lanes[0] == 2) {
        if (num_lanes[2] == -1) {
            mode = SOC_TD3_PORT_RATIO_DUAL_1_1;
        } else {
            mode = (speed_max[0] == 20000 || speed_max[0] == 21000 ||
                    speed_max[0] == 50000 || speed_max[0] == 53000) ?
                   SOC_TD3_PORT_RATIO_TRI_023_2_1_1 :
                   SOC_TD3_PORT_RATIO_TRI_023_4_1_1;
        }
    } else if (num_lanes[2] == 2) {
        if (num_lanes[0] == -1) {
            mode = SOC_TD3_PORT_RATIO_DUAL_1_1;
        } else {
            mode = (speed_max[2] == 20000 || speed_max[2] == 21000 ||
                    speed_max[2] == 50000 || speed_max[2] == 53000) ?
                   SOC_TD3_PORT_RATIO_TRI_012_1_1_2 :
                   SOC_TD3_PORT_RATIO_TRI_012_1_1_4;
        }
    } else {
        mode = SOC_TD3_PORT_RATIO_QUAD;
    }

    *pm_mode = mode;
}


/*! @fn void _soc_td3_tdm_get_pipe_map(
        int unit,
        soc_port_schedule_state_t *sch_info,
        uint32 *pipe_map)
    @param unit Chip unit number.
    @param sch_info Pointer to a soc_port_schedule_state_t struct variable.
    @param pipe_map Returned pipe map.
    @brief Returns pipe bitmap.
 * Description:
        For init, a pipe gets 1 if exists active port within that pipe.
        For flexport, a pipe gets 1 if exists flexed port within that pipe.
 */
void
_soc_td3_tdm_get_pipe_map(int unit,
                          soc_port_schedule_state_t *sch_info,
                          uint32 *pipe_map)
{
    uint32 port, pipe, i, pipe_bmap = 0;

    if (sch_info->is_flexport == 0) { /* Init */
        for (port = 0; port < TRIDENT3_TDM_PHY_PORTS_PER_DEV; port++) {
            if (sch_info->out_port_map.log_port_speed[port] > 0 ) {
                pipe = port / TRIDENT3_TDM_PHY_PORTS_PER_PIPE;
                pipe_bmap |= 1 << pipe;
            }
        }
    } else { /* FlexPort */
        for (i = 0; i < sch_info->nport; i++) {
            port = sch_info->resource[i].logical_port;
            pipe = port / TRIDENT3_TDM_PHY_PORTS_PER_PIPE;
            pipe_bmap |= 1 << pipe;
        }
    }
    *pipe_map = pipe_bmap;
}


/*! @fn void _soc_td3_tdm_get_ovs_pipe_map(
        int unit,
        soc_port_schedule_state_t *sch_info,
        uint32 *ovs_pipe_map)
    @param unit Chip unit number.
    @param sch_info Pointer to a soc_port_schedule_state_t struct variable.
    @param ovs_pipe_map Returned pipe map.
    @brief Returns OVS pipe bitmap.
 * Description:
        For init, a pipe gets 1 if exists oversub port within that pipe.
 */
void
_soc_td3_tdm_get_ovs_pipe_map(int unit,
                              soc_port_schedule_state_t *sch_info,
                              uint32 *ovs_pipe_map)
{
    uint32 port, pipe, pipe_bmap = 0;
    int is_ovs_map;

    if (sch_info->is_flexport == 0) { /* Init */
        for (port = 0; port < TRIDENT3_TDM_PHY_PORTS_PER_DEV; port++) {
            is_ovs_map = SOC_PBMP_MEMBER(sch_info->out_port_map.oversub_pbm,
															port) ? 1 : 0;
            if ((sch_info->out_port_map.log_port_speed[port] > 0) &&
                (1 == is_ovs_map) ) {
                pipe = port / TRIDENT3_TDM_PHY_PORTS_PER_PIPE;
                pipe_bmap |= 1 << pipe;
            }
        }
    }
    *ovs_pipe_map = pipe_bmap;
}


/*! @fn int _soc_td3_tdm_get_speed_ovs_class(
        int unit,
        int speed)
    @param unit   Chip unit number.
    @param speed Speed
    @return port_speed_class to be returned
    @brief Help function to determine ovs class
 * Description:
    Oversubscription speed group class encoding
        0 - 0
        1 - 2 (1G/10G)
        2 - 4 (20G)
        3 - 5 (25G)
        4 - 8 (40G)
        5 - 10 (50G)
        6 - 20 (100G)
 */
int
_soc_td3_tdm_get_speed_ovs_class(int unit, int speed)
{
    int speed_class = 0;

    if (speed >= 40000) {
        if (speed >= 100000) {
            speed_class = 6;
        } else if (speed >= 50000) {
            speed_class = 5;
        } else {
            speed_class = 4;
        }
    } else {
        if (speed >= 25000) {
            speed_class = 3;
        } else if (speed >= 20000) {
            speed_class = 2;
        } else if (speed >= 10000) {
            speed_class = 1;
        } else if (speed >= 1000) {
            speed_class = 1;
        }
    }

    /* 1G and 10G are always treated as 25G. */
    if (speed_class == 1) {
        speed_class = 3;
    }

    return (speed_class);
}


/*! @fn int _soc_td3_tdm_check_is_hsp_port_e(
        int unit,
        int speed)
    @param unit   Chip unit number.
    @param speed Speed
    @return 1->high speed port; 0->not high speed port;
    @brief To determine the same port spacing for egress pipe
 * Description:
    Oversubscription speed group class encoding
        0 - 0
        1 - 2 (10G)
        2 - 4 (20G)
        3 - 5 (25G)
        4 - 8 (40G)
        5 - 10 (50G)
        6 - 20 (100G)
 */
STATIC int
_soc_td3_tdm_check_is_hsp_port_e(int unit, int speed)
{
    int speed_class, port_is_hsp = 0;

    speed_class = _soc_td3_tdm_get_speed_ovs_class(unit, speed);
    switch (speed_class) {
        case 2: /* 20G */
        case 4: /* 40G */
        case 5: /* 50G */
        case 6: /* 100G */
            port_is_hsp = 1;
            break;
        default:
            port_is_hsp = 0;
            break;
    }

    return (port_is_hsp);
}


/*! @fn int _soc_td3_tdm_check_2nd_mgmt_enable(
        int unit,
        soc_port_schedule_state_t *sch_info)
    @param unit   Chip unit number.
    @param sch_info Pointer to a soc_port_schedule_state_t struct variable.
    @return 1-> 2nd management port enabled; 0 -> otherwise
    @brief Help function to determine if 2nd management port is enabled.
 */
STATIC int
_soc_td3_tdm_check_2nd_mgmt_enable(int unit,
					soc_port_schedule_state_t *sch_info)
{
    int port;
    int mgmt_port_cnt = 0;
    int mgmt_2nd_port_en = 0;

    SOC_PBMP_ITER(sch_info->in_port_map.management_pbm, port) {
        mgmt_port_cnt++;
    }
    if (mgmt_port_cnt>1) {
        mgmt_2nd_port_en = 1;
    }

    return (mgmt_2nd_port_en);
}


/*! @fn int _soc_td3_tdm_get_same_port_spacing_e(
        int unit,
        int speed)
    @param unit   Chip unit number.
    @param speed Speed
    @return same port spacing of the given speed port
    @brief To determine the same port spacing for egress pipe
           - 8 for high speed port
           - 14 for not high speed port
 */
int
_soc_td3_tdm_get_same_port_spacing_e(int unit, int speed)
{
    int port_same_spacing = 0;

    if (_soc_td3_tdm_check_is_hsp_port_e(unit, speed) == 1) {
        port_same_spacing = 8;
    } else {
        port_same_spacing = 14;
    }

    return (port_same_spacing);
}


/*! @fn int _soc_td3_tdm_calculation(
        int unit,
        soc_port_schedule_state_t *sch_info)
    @param unit Chip unit number.
    @param sch_info Pointer to a soc_port_schedule_state_t struct variable.
    @brief API to calculate TDM tables.
 */
int
_soc_td3_tdm_calculation(int unit, soc_port_schedule_state_t *sch_info)
{
    int i, j, pm_num, phy_port, lgc_port, phy_port_tmp;
    int pipe, hpipe;
    int lane, group, index;
    int mgmt_pm_hg = 0, mgmt_mode = 0;
    int tdm_idb_cal_len, tdm_mmu_cal_len, tdm_pipe_grp_num, tdm_pipe_grp_len;
    int tdm_chk_en = 0;
    int *tdm_idb_pipe_main, *tdm_mmu_pipe_main;
    int **tdm_pipe_ovs_group;
    tdm_soc_t _chip_pkg;
    tdm_mod_t *_tdm_pkg;
    uint32 port_speeds[TRIDENT3_TDM_PHY_PORTS_PER_DEV];
    uint32 port_states[TRIDENT3_TDM_PHY_PORTS_PER_DEV];
    int l_speed = 0, l_num_lanes = 0, prev_or_new = 1;

    sal_memset(port_speeds, 0, TRIDENT3_TDM_PHY_PORTS_PER_DEV*sizeof(uint32));
    sal_memset(port_states, 0, TRIDENT3_TDM_PHY_PORTS_PER_DEV*sizeof(uint32));

    /* Remap input speeds to ETH bitrates */
    soc_trident3_port_schedule_speed_remap(unit, sch_info);

    /* Set port states and speeds */
    for (lgc_port = 0; lgc_port < TRIDENT3_TDM_PHY_PORTS_PER_DEV; lgc_port++) {
        phy_port = sch_info->in_port_map.port_l2p_mapping[lgc_port];
        if (phy_port < TRIDENT3_TDM_PHY_PORTS_PER_DEV) {
            if (sch_info->in_port_map.log_port_speed[lgc_port] > 0 ) {
                if (SOC_PBMP_MEMBER(sch_info->in_port_map.hg2_pbm,lgc_port)) {
                    port_states[phy_port] =
                        SOC_PBMP_MEMBER(sch_info->in_port_map.oversub_pbm,
							lgc_port) ? PORT_STATE__OVERSUB_HG :
										PORT_STATE__LINERATE_HG;
                } else {
                    port_states[phy_port] =
                        SOC_PBMP_MEMBER(sch_info->in_port_map.oversub_pbm,
							lgc_port) ? PORT_STATE__OVERSUB :
										PORT_STATE__LINERATE;
                }

                soc_td3_tdm_get_log_port_speed(unit, sch_info, lgc_port,
                    prev_or_new, &l_speed, &l_num_lanes);
                for (lane = 1; lane < l_num_lanes; lane++) {
                    phy_port_tmp = phy_port + lane;
                    if (phy_port_tmp < TRIDENT3_TDM_PHY_PORTS_PER_DEV) {
                        port_states[phy_port_tmp] = PORT_STATE__COMBINE;
                    }
                }
                port_speeds[phy_port] = l_speed;
            }
            if (SOC_PBMP_MEMBER(sch_info->in_port_map.management_pbm, lgc_port)) {
                    if (SOC_PBMP_MEMBER(sch_info->in_port_map.hg2_pbm, lgc_port)) {
                        mgmt_pm_hg = 1; /* HIGIG2 : ETHERNET */
                    }
                    port_speeds[phy_port] = 0;
                    port_states[phy_port] = 0;
            }
        }
    }
    /* Print input config */
    if (LOG_CHECK(BSL_LS_SOC_TDM | BSL_INFO)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                            "frequency: %dMHz\n"),
                            sch_info->frequency));
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                            "%8s%8s%8s\n"),
                            "port", "speed", "state"));
        for (phy_port = 0; phy_port < TRIDENT3_TDM_PHY_PORTS_PER_DEV;
									phy_port++) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                                "%8d%8d%8d\n"),
                                phy_port, port_speeds[phy_port]/1000,
                                port_states[phy_port]));
        }
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "\n")));
    }
    /* Initialize input variables for SW TDM Generation:
       -- unit
       -- num_ext_ports
       -- state
       -- speed
       -- clk_freq
       -- flex_port_en: 0->init, 1->flex
       -- mgmt_pm_hg: 0->Ethernet mgmt, 1->HiGig2 mgmt
       -- mgmt_mode: 0->two Ethernet mgmt ports
                     1->one Ethernet mgmt port
                     2->two Higig mgmt ports
                     3->one Higig mgmt port
    */
    sal_memset(&_chip_pkg, 0, sizeof(tdm_soc_t));
    _chip_pkg.unit          = unit;
    _chip_pkg.num_ext_ports = TD3_NUM_EXT_PORTS;
    _chip_pkg.state = sal_alloc((_chip_pkg.num_ext_ports)*sizeof(int),
                                "port state list");
    if (_chip_pkg.state == NULL) {
        return SOC_E_MEMORY;
    }
    _chip_pkg.speed = sal_alloc((_chip_pkg.num_ext_ports)*sizeof(int),
                                "port speed list");
    if (_chip_pkg.speed == NULL) {
        sal_free(_chip_pkg.state);
        return SOC_E_MEMORY;
    }
    for (index = 0; index < _chip_pkg.num_ext_ports; index++) {
        _chip_pkg.state[index] = 0;
        _chip_pkg.speed[index] = 0;
    }
    for (index = 0; index < _chip_pkg.num_ext_ports &&
                    index < TRIDENT3_TDM_PHY_PORTS_PER_DEV; index++) {
        _chip_pkg.state[index] = port_states[index];
        _chip_pkg.speed[index] = port_speeds[index];
    }
    /* Shift port state to left one position; required by C_TDM */
    for (index = 1; index < _chip_pkg.num_ext_ports; index++) {
        _chip_pkg.state[index - 1] = _chip_pkg.state[index];
    }
    _chip_pkg.clk_freq               = sch_info->frequency;
    _chip_pkg.flex_port_en           = sch_info->is_flexport;
    _chip_pkg.soc_vars.td3.tdm_chk_en= tdm_chk_en;
    _chip_pkg.soc_vars.td3.mgmt_pm_hg= mgmt_pm_hg;
    mgmt_mode = mgmt_pm_hg == 1 ? 2 : 0;
    _chip_pkg.soc_vars.td3.mgmt_mode =
        (_soc_td3_tdm_check_2nd_mgmt_enable(unit, sch_info) == 1) ?
        (mgmt_mode + 0) : (mgmt_mode + 1);

    /* Populate TDM tables by invoking SW TDM API */
    /* coverity[dont_call : FALSE] */
    _tdm_pkg = _soc_set_tdm_tbl(SOC_SEL_TDM(&_chip_pkg));

    /* Free the memory allocated to port states and speeds */
    sal_free(_chip_pkg.state);
    sal_free(_chip_pkg.speed);

    /* Copy TDM result into output structure */
    if (_tdm_pkg == NULL) {
        LOG_CLI((BSL_META_U(unit,
                 "Unsupported config for TDM calendar generation\n")));
        return SOC_E_FAIL;
    } else {
        /* IDB calendar (0-3), MMU calendar (4-7) */
        for (pipe = 0; pipe < TRIDENT3_TDM_PIPES_PER_DEV; pipe++) {
            switch (pipe) {
                /* IDB_PIPE_0, MMU_PIPE_0 */
                case 0:
                    tdm_pipe_ovs_group = _tdm_pkg->_chip_data.cal_0.cal_grp;
                    tdm_pipe_grp_num   = _tdm_pkg->_chip_data.cal_0.grp_num;
                    tdm_pipe_grp_len   = _tdm_pkg->_chip_data.cal_0.grp_len;
                    tdm_idb_pipe_main  = _tdm_pkg->_chip_data.cal_0.cal_main;
                    tdm_idb_cal_len    = _tdm_pkg->_chip_data.cal_0.cal_len;
                    tdm_mmu_pipe_main  = _tdm_pkg->_chip_data.cal_4.cal_main;
                    tdm_mmu_cal_len    = _tdm_pkg->_chip_data.cal_4.cal_len;
                    break;
                /* IDB_PIPE_1, MMU_PIPE_1 */
                default:
                    tdm_pipe_ovs_group = _tdm_pkg->_chip_data.cal_1.cal_grp;
                    tdm_pipe_grp_num   = _tdm_pkg->_chip_data.cal_1.grp_num;
                    tdm_pipe_grp_len   = _tdm_pkg->_chip_data.cal_1.grp_len;
                    tdm_idb_pipe_main  = _tdm_pkg->_chip_data.cal_1.cal_main;
                    tdm_idb_cal_len    = _tdm_pkg->_chip_data.cal_1.cal_len;
                    tdm_mmu_pipe_main  = _tdm_pkg->_chip_data.cal_5.cal_main;
                    tdm_mmu_cal_len    = _tdm_pkg->_chip_data.cal_5.cal_len;
                    break;
            }
            if (tdm_pipe_ovs_group == NULL ||
                tdm_idb_pipe_main == NULL ||
                tdm_mmu_pipe_main == NULL) {
                LOG_CLI((BSL_META_U(unit,
                         "Unsupported config for TDM calendar generation\n")));
                tdm_td3_main_free(_tdm_pkg);
                sal_free(_tdm_pkg);
                return SOC_E_FAIL;
            } else {
                /* IDB TDM main calendar */
                for (index = 0; index < tdm_idb_cal_len; index++) {
                    sch_info->tdm_ingress_schedule_pipe[pipe].tdm_schedule_slice[
						0].linerate_schedule[index] = tdm_idb_pipe_main[index];
                }
                /* MMU TDM main calendar */
                for (index = 0; index < tdm_mmu_cal_len; index++) {
                    sch_info->tdm_egress_schedule_pipe[pipe].tdm_schedule_slice[
						0].linerate_schedule[index] = tdm_mmu_pipe_main[index];
                }

                /* IDB and MMU OverSub Groups */
                for (i = 0; i < tdm_pipe_grp_num; i++) {
                    /* Group 0:5 -> hpipe 0, Group 6:11 -> hpipe1 */
                    hpipe = i / TRIDENT3_TDM_OVS_GROUPS_PER_HPIPE;
                    group = i % TRIDENT3_TDM_OVS_GROUPS_PER_HPIPE;
                    for (j = 0; j < tdm_pipe_grp_len; j++) {
                        sch_info->tdm_ingress_schedule_pipe[
								pipe].tdm_schedule_slice[hpipe].oversub_schedule[
								group][j] = tdm_pipe_ovs_group[i][j];
                        sch_info->tdm_egress_schedule_pipe[
								pipe].tdm_schedule_slice[hpipe].oversub_schedule[
								group][j] = tdm_pipe_ovs_group[i][j];
                    }
                }

                /* Packet shaper */
                for (hpipe = 0; hpipe < 2; hpipe++) {
                    for (j = 0; j < TD3_SHAPING_GRP_LEN; j++) {
                        sch_info->tdm_ingress_schedule_pipe[
							pipe].tdm_schedule_slice[hpipe].pkt_sch_or_ovs_space[
							0][j] = tdm_pipe_ovs_group[hpipe+
										TD3_SHAPING_GRP_IDX_0][j];
                    }
                }
            }
        }
        /* Re-evaluate this at FlexPort;
         * Before calling FlexPort sch_info->in_port_map.port_p2PBLK_inst_mapping should be copied in pm_num_to_pblk
         * After FlexPort pm_num_to_pblk should be copied back to sch_info->out_port_map.port_p2PBLK_inst_mapping
         */
        sal_memset(sch_info->out_port_map.port_p2PBLK_inst_mapping,
                   0, sizeof(int) * SOC_MAX_NUM_PORTS);
        for (pm_num = 0; pm_num < TD3_NUM_PHY_PM; pm_num++) {
            for (lane = 0; lane < TRIDENT3_TDM_PORTS_PER_PBLK; lane++) {
                sch_info->out_port_map.port_p2PBLK_inst_mapping[
				(pm_num * TRIDENT3_TDM_PORTS_PER_PBLK) + 1 + lane] =
                _tdm_pkg->_chip_data.soc_pkg.soc_vars.td3.pm_num_to_pblk[pm_num];
            }
        }
        /* Free the memory allocated in TDM algorithm code */
        tdm_td3_main_free(_tdm_pkg); /* This function frees all pointers allocated during _soc_set_tdm_tbl call */
        sal_free(_tdm_pkg);
    }

    /* Print TDM result */
    if (LOG_CHECK(BSL_LS_SOC_TDM | BSL_INFO)) {
        for (pipe = 0; pipe < TRIDENT3_TDM_PIPES_PER_DEV; pipe++) {
            /* IDB TDM main calendar */
            LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                                "Pipe %d [IDB TDM main calendar]\n"),
                                pipe));
            for (index = 0; index < MAX_CAL_LEN; index++) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                                    " %3d"),
                                    sch_info->tdm_ingress_schedule_pipe[
						pipe].tdm_schedule_slice[0].linerate_schedule[index]));
                if (index % 16 == 0 && index > 0) {
                    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "\n")));
                }
            }
            LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "\n")));
            /* Oversub groups */
            for (hpipe = 0; hpipe < TRIDENT3_TDM_HPIPES_PER_PIPE; hpipe++) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                              "Pipe %d Half-Pipe %d [IDB TDM Oversub Groups]\n"),
                              pipe, hpipe));
                for (group = 0; group < TRIDENT3_TDM_OVS_GROUPS_PER_HPIPE;
																group++) {
                    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                                        "oversub group %d: "),
                                        group));
                    for (index = 0; index < TRIDENT3_TDM_OVS_GROUP_LENGTH;
																index++) {
                        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                                            " %3d"),
                sch_info->tdm_ingress_schedule_pipe[pipe].tdm_schedule_slice[
				hpipe].oversub_schedule[group][index]));
                    }
                    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "\n")));
                }
                LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "\n")));
            }
            /* Packet scheduler */
            for (hpipe = 0; hpipe < TRIDENT3_TDM_HPIPES_PER_PIPE; hpipe++) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                            "Pipe %d Half-Pipe %d [IDB TDM PKT SCHEDULER]\n"),
                                    pipe, hpipe));
                for (index = 0; index < TD3_SHAPING_GRP_LEN; index++) {
                    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                                        " %3d"),
                    sch_info->tdm_ingress_schedule_pipe[
					pipe].tdm_schedule_slice[hpipe].pkt_sch_or_ovs_space[0][index]));
                    if (index % 16 == 0 && index > 0) {
                        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "\n")));
                    }
                }
                LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "\n")));
            }
            /* MMU TDM main calendar */
            LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                        "Pipe %d [MMU TDM main calendar]:\n"), pipe));
            for (index = 0; index < MAX_CAL_LEN; index++) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, " %3d"),
                            sch_info->tdm_egress_schedule_pipe[pipe].
                            tdm_schedule_slice[0].linerate_schedule[index]));
                if (index % 16 == 0 && index > 0) {
                    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "\n")));
                }
            }
            LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "\n")));
        }
    }

    /* If init then just copy in_port_map to out_port_map*/
    if (sch_info->is_flexport == 0) {
        for (i= 0; i< SOC_MAX_NUM_PORTS;i++) {
            sch_info->out_port_map.log_port_speed[i]   =
					sch_info->in_port_map.log_port_speed[i];
            sch_info->out_port_map.port_p2l_mapping[i] =
					sch_info->in_port_map.port_p2l_mapping[i];
            sch_info->out_port_map.port_l2p_mapping[i] =
					sch_info->in_port_map.port_l2p_mapping[i];
            sch_info->out_port_map.port_p2m_mapping[i] =
					sch_info->in_port_map.port_p2m_mapping[i];
            sch_info->out_port_map.port_num_lanes[i]   =
					sch_info->in_port_map.port_num_lanes[i];
            sch_info->out_port_map.port_l2i_mapping[i] =
					sch_info->in_port_map.port_l2i_mapping[i];
        }
        for (i = 0; i < SOC_MAX_NUM_MMU_PORTS; i++) {
            sch_info->out_port_map.port_m2p_mapping[i] =
					sch_info->in_port_map.port_m2p_mapping[i];
        }
        for (i = 0; i < _SHR_PBMP_WORD_MAX; i++) {
            sch_info->out_port_map.physical_pbm.pbits[i]   =
					sch_info->in_port_map.physical_pbm.pbits[i];
            sch_info->out_port_map.hg2_pbm.pbits[i]        =
					sch_info->in_port_map.hg2_pbm.pbits[i];
            sch_info->out_port_map.management_pbm.pbits[i] =
					sch_info->in_port_map.management_pbm.pbits[i];
            sch_info->out_port_map.oversub_pbm.pbits[i]    =
					sch_info->in_port_map.oversub_pbm.pbits[i];
        }
    }
    /*
     * else {
     *    HERE future handling of FlexPort between in_port_map to out_port_map
     * }
    */

    _soc_td3_tdm_print_schedule_state(unit, sch_info);
    return SOC_E_NONE;
}


/*! @fn int _soc_td3_tdm_set_idb_calendar(
        int unit,
        soc_port_schedule_state_t *sch_info)
    @param unit Chip unit number.
    @param sch_info Pointer to a soc_port_schedule_state_t struct variable.
    @brief API to initialize the TD3 IDB main calendar.
 */
int
_soc_td3_tdm_set_idb_calendar(int unit, soc_port_schedule_state_t *sch_info)
{
    uint32 pipe_map;
    uint32 cal_len;
    soc_mem_t mem;
    soc_reg_t reg;
    soc_field_t cal_end_field;
    int pipe, slot, id, length, calendar_id;
    int phy_port, idb_port;
    int port;
    uint32 rval;
    uint32 memfld;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 mem_indx;
    static const soc_mem_t idb_tdm_cal_mem[TRIDENT3_TDM_PIPES_PER_DEV][2] = {
        {IS_TDM_CALENDAR0_PIPE0m, IS_TDM_CALENDAR1_PIPE0m},
        {IS_TDM_CALENDAR0_PIPE1m, IS_TDM_CALENDAR1_PIPE1m}
    };
    static const soc_reg_t idb_tdm_cfg_reg[TRIDENT3_TDM_PIPES_PER_DEV] = {
        IS_TDM_CONFIG_PIPE0r, IS_TDM_CONFIG_PIPE1r
    };
    static const soc_field_t calendar_end_fields[] = {
        CAL0_ENDf, CAL1_ENDf
    };

    _soc_td3_tdm_get_pipe_map(unit, sch_info, &pipe_map);
    sal_memset(entry, 0, sizeof(uint32) * soc_mem_entry_words(unit,
								IS_TDM_CALENDAR0_PIPE0m));

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        /* coverity[overrun-local : FALSE] */
        reg = idb_tdm_cfg_reg[pipe];
        /* Pipe is empty */
        if ((pipe_map & (1 << pipe)) == 0) {
            /* At init, even if the pipe doesn't have active ports,
             * enable scheduler for sbus accesses to PP IPIPE
             */
            if (sch_info->is_flexport == 0) {
                rval = 0;
                soc_reg_field_set(unit, reg, &rval, ENABLEf, 1);
                SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY,
											0, rval));
            }
            continue;
        }
        /* Determine calendar_id : 0 or 1 */
        if (sch_info->is_flexport == 1) { /* choose "the other calendar" */
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY,
											0, &rval));
            calendar_id = soc_reg_field_get(unit, reg, rval, CURR_CALf) ^ 1;
        } else {
            /* At init use calendar_id=0 */
            calendar_id = 0;
            /* Also, before writing the TDM calendar entries, make sure
             * calendar is disabled again;
             * IDB scheduler may be already enabled at this point to allow
             * sbus access to IPIPE for reset purpose (Sec. 3.1.2 SW_init doc)
             */
            rval = 0;
            soc_reg_field_set(unit, reg, &rval, ENABLEf, 0);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
        }
        /* coverity[overrun-local : FALSE] */
        mem = idb_tdm_cal_mem[pipe][calendar_id];
        cal_end_field = calendar_end_fields[calendar_id];

        /* TDM Calendar is always taken from slice 0 */
        cal_len = sch_info->tdm_ingress_schedule_pipe[pipe].tdm_schedule_slice[
																0].cal_len;
        for (length = cal_len; length > 0; length--) {
            if (sch_info->tdm_ingress_schedule_pipe[pipe].tdm_schedule_slice[
					0].linerate_schedule[length - 1] != TD3_NUM_EXT_PORTS) {
                break;
            }
        }
        for (slot = 0; slot < length; slot += 2) {
            /* EVEN */
            phy_port = sch_info->tdm_ingress_schedule_pipe[
				pipe].tdm_schedule_slice[0].linerate_schedule[slot];
            if (phy_port == TD3_OVSB_TOKEN) {
                idb_port = TRIDENT3_TDM_IDB_OVSB_TOKEN;
            } else if (phy_port == TD3_IDL1_TOKEN) {
                idb_port = TRIDENT3_TDM_IDB_SBUS_TOKEN;
            } else if (phy_port == TD3_IDL2_TOKEN) {
                idb_port = TRIDENT3_TDM_IDB_UNUSED_TOKEN;
            } else if (phy_port == TD3_NULL_TOKEN) {
                idb_port = TRIDENT3_TDM_IDB_NULL_TOKEN;
            } else if (phy_port >= TD3_NUM_EXT_PORTS) {
                idb_port = TRIDENT3_TDM_IDB_UNUSED_TOKEN;
            } else if (phy_port == TD3_MGM1_TOKEN) {
                /* TDM code always allocates slots for management port.
                 * If mgmt ports are omitted from config, this mapping
                 * is not set up.
                 */
                idb_port = TRIDENT3_TDM_IDB_MGMT1_TOKEN;
            } else {
                port = sch_info->out_port_map.port_p2l_mapping[phy_port];
                idb_port = sch_info->out_port_map.port_l2i_mapping[port] & 0x7f;
            }
            if (idb_port > 63) { /* Not a general front panel port */
                id = 0xf;
            } else {
                id = (phy_port - 1) / TRIDENT3_TDM_PORTS_PER_PBLK;
            }
            memfld = idb_port & 0x7f; /* [0,69] && 127 */
            soc_mem_field_set(unit, mem, entry, PORT_NUM_EVENf, &memfld);
            memfld = id & 0xf;
            soc_mem_field_set(unit, mem, entry, PHY_PORT_ID_EVENf, &memfld);
            /* ODD */
            phy_port = sch_info->tdm_ingress_schedule_pipe[pipe].tdm_schedule_slice[0].linerate_schedule[slot + 1];
            if (phy_port == TD3_OVSB_TOKEN) {
                idb_port = TRIDENT3_TDM_IDB_OVSB_TOKEN;
            } else if (phy_port == TD3_IDL1_TOKEN) {
                idb_port = TRIDENT3_TDM_IDB_SBUS_TOKEN;
            } else if (phy_port == TD3_IDL2_TOKEN) {
                idb_port = TRIDENT3_TDM_IDB_UNUSED_TOKEN;
            } else if (phy_port == TD3_NULL_TOKEN) {
                idb_port = TRIDENT3_TDM_IDB_NULL_TOKEN;
            } else if (phy_port >= TD3_NUM_EXT_PORTS) {
                idb_port = TRIDENT3_TDM_IDB_UNUSED_TOKEN;
            } else if (phy_port == TD3_MGM1_TOKEN) {
                /* TDM code always allocates slots for management port.
                 * If mgmt ports are omitted from config, this mapping
                 * is not set up.
                 */
                idb_port = TRIDENT3_TDM_IDB_MGMT1_TOKEN;
            } else {
                port = sch_info->out_port_map.port_p2l_mapping[phy_port];
                idb_port = sch_info->out_port_map.port_l2i_mapping[port] & 0x7f;
            }
            if (idb_port > 63) { /* Not a general front panel port */
                id = 0xf;
            } else {
                id = (phy_port - 1) / TRIDENT3_TDM_PORTS_PER_PBLK;
            }
            memfld = idb_port & 0x7f; /* [0,69] && 127 */
            soc_mem_field_set(unit, mem, entry, PORT_NUM_ODDf, &memfld);
            memfld = id & 0xf;
            soc_mem_field_set(unit, mem, entry, PHY_PORT_ID_ODDf, &memfld);
            /* Memory entry */
            mem_indx = slot / 2;
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL, mem_indx, entry));
        }

        /* set for 2nd management port */
        if (_soc_td3_tdm_check_2nd_mgmt_enable(unit, sch_info) == 1 &&
            pipe == (TRIDENT3_TDM_PIPES_PER_DEV - 1)) {
            /* reg: IS_TDM_CONFIG_PIPExr */
            soc_reg_field_set(unit, reg, &rval, MGMT_2ND_PORT_ENABLEf, 1);
        }

        soc_reg_field_set(unit, reg, &rval, cal_end_field, length-1);
        soc_reg_field_set(unit, reg, &rval, CURR_CALf, calendar_id);
        soc_reg_field_set(unit, reg, &rval, ENABLEf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }

    return SOC_E_NONE;
}


/*! @fn int _soc_td3_tdm_set_idb_hsp(
        int unit,
        soc_port_schedule_state_t *sch_info)
    @param unit Chip unit number.
    @param sch_info Pointer to a soc_port_schedule_state_t struct variable.
    @brief API to initialize the TD3 IDB HSP.
 */
int
_soc_td3_tdm_set_idb_hsp(int unit, soc_port_schedule_state_t *sch_info)
{
    soc_reg_t reg;
    uint32 pipe_map;
    int pipe;
    uint32 pmap;
    uint32 rval = 0;

    static const soc_reg_t idb_tdm_hsp[TRIDENT3_TDM_PIPES_PER_DEV][2] = {
        {IS_TDM_HSP_0_PIPE0r, IS_TDM_HSP_0_PIPE1r},
        {IS_TDM_HSP_1_PIPE0r, IS_TDM_HSP_1_PIPE1r}
    };

    /* IDB always configures PORT_BMPf field for idb_tdm_hsp regs
     * to 0xFFFF_FFFF_FFFF_FFFF, independent of the port's speed.
     */
    pmap = 0xffffffff; /* up to 32 ports */

    _soc_td3_tdm_get_pipe_map(unit, sch_info, &pipe_map);
    for (pipe = 0; pipe < TRIDENT3_TDM_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }
        reg = idb_tdm_hsp[pipe][0];
        soc_reg_field_set(unit, reg, &rval, PORT_BMPf, pmap);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

        reg = idb_tdm_hsp[pipe][1];
        soc_reg_field_set(unit, reg, &rval, PORT_BMPf, pmap);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }

    return SOC_E_NONE;
}


/*! @fn int _soc_td3_tdm_set_idb_ppe_credit(
        int unit,
        soc_port_schedule_state_t
        *sch_info,
        int full_credit_threshold_0,
        int opp_credit_threshold_0,
        int full_credit_threshold_1,
        int opp_credit_threshold_1 )
    @param unit Chip unit number.
    @param sch_info Pointer to a soc_port_schedule_state_t struct variable.
    @param full_credit_threshold_0
    @param opp_credit_threshold_0
    @param full_credit_threshold_1
    @param opp_credit_threshold_1
    @brief API to initialize the TD3 IDB PPE Credit Config register.
           Regs/Mems configured:PPE_CREDIT_CONFIG_PIPE<pipe>
 */
int
_soc_td3_tdm_set_idb_ppe_credit(int unit,
                                soc_port_schedule_state_t *sch_info,
                                int full_credit_threshold_0,
                                int opp_credit_threshold_0,
                                int full_credit_threshold_1,
                                int opp_credit_threshold_1 )
{
    soc_reg_t reg;
    uint32 pipe_map;
    int pipe;
    uint32 rval;
    static const soc_reg_t idb_ppe_credit[TRIDENT3_TDM_PIPES_PER_DEV] = {
        PPE_CREDIT_CONFIG_PIPE0r, PPE_CREDIT_CONFIG_PIPE1r
    };

    _soc_td3_tdm_get_pipe_map(unit, sch_info, &pipe_map);

    for (pipe = 0; pipe < TRIDENT3_TDM_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }
        reg = idb_ppe_credit[pipe];
        rval = 0;
        /*SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));*/
        soc_reg_field_set(unit, reg, &rval, FULL_CREDIT_THRESHOLD_0f,
											full_credit_threshold_0);
        soc_reg_field_set(unit, reg, &rval, OPP_CREDIT_THRESHOLD_0f,
											opp_credit_threshold_0);
        soc_reg_field_set(unit, reg, &rval, FULL_CREDIT_THRESHOLD_1f,
											full_credit_threshold_1);
        soc_reg_field_set(unit, reg, &rval, OPP_CREDIT_THRESHOLD_1f,
											opp_credit_threshold_1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0,
											rval));
    }

    return SOC_E_NONE;
}


/*! @fn int _soc_td3_tdm_set_idb_opportunistic(
        int unit,
        soc_port_schedule_state_t *sch_info,
        int cpu_op_en,
        int lb_opp_en,
        int opp1_port_en,
        int opp2_port_en,
        int opp_ovr_sub_en)
    @param unit Chip unit number.
    @param sch_info Pointer to a soc_port_schedule_state_t struct variable.
    @param cpu_op_en
    @param lb_opp_en
    @param opp1_port_en
    @param opp2_port_en
    @param opp_ovr_sub_en
    @brief API to initialize the TD3 IDB Opportunistic related registers
           Regs/Mems configured: IS_CPU_LB_OPP_CFG_PIPE<pipe>
                                 IS_OPP_SCHED_CFG_PIPE<pipe>
 */
int
_soc_td3_tdm_set_idb_opportunistic(int unit,
                                   soc_port_schedule_state_t *sch_info,
                                   int cpu_op_en,
                                   int lb_opp_en,
                                   int opp1_port_en,
                                   int opp2_port_en,
                                   int opp_ovr_sub_en)
{
    soc_reg_t reg;
    uint32 pipe_map;
    int pipe;
    uint32 rval;
    static const soc_reg_t idb_cpu_lb_opp_cfg[TRIDENT3_TDM_PIPES_PER_DEV] = {
        IS_CPU_LB_OPP_CFG_PIPE0r, IS_CPU_LB_OPP_CFG_PIPE1r
    };
    static const soc_reg_t idb_opp_sched_cfg[TRIDENT3_TDM_PIPES_PER_DEV] = {
        IS_OPP_SCHED_CFG_PIPE0r, IS_OPP_SCHED_CFG_PIPE1r
    };

    _soc_td3_tdm_get_pipe_map(unit, sch_info, &pipe_map);

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        if ((pipe_map & (1 << pipe)) == 0) {
            /* Enable OPP1 to enable sbus access to the rest of IPIPE*/
            reg = idb_opp_sched_cfg[pipe];
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
            soc_reg_field_set(unit, reg, &rval, OPP1_PORT_ENf,
												(opp1_port_en == 1) ? 1 : 0);
            soc_reg_field_set(unit, reg, &rval, OPP1_SPACINGf, 8);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
            continue;
        }

        reg = idb_cpu_lb_opp_cfg[pipe];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, CPU_OPP_ENf,
												(cpu_op_en == 1) ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, LB_OPP_ENf,
												(lb_opp_en == 1) ? 1 : 0);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

        reg = idb_opp_sched_cfg[pipe];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, OPP1_PORT_ENf,
												(opp1_port_en == 1) ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, OPP1_SPACINGf, 8);
        soc_reg_field_set(unit, reg, &rval, OPP2_PORT_ENf,
												(opp2_port_en == 1) ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, OPP2_SPACINGf, 0);
        soc_reg_field_set(unit, reg, &rval, OPP_OVR_SUB_ENf,
												(opp_ovr_sub_en == 1) ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, OPP_STRICT_PRIf, 1);
        soc_reg_field_set(unit, reg, &rval, DISABLE_PORT_NUMf, 67); /* NULL port */
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }

    return SOC_E_NONE;
}


/*! @fn int _soc_td3_tdm_set_idb_pkt_calendar(
        int unit,
        soc_port_schedule_state_t *sch_info)
    @param unit Chip unit number.
    @param sch_info Pointer to a soc_port_schedule_state_t struct variable.
    @brief API to initialize the TD3 IDB Packet Shaper calendar.
           Regs/Mems configured: IDB PKT_SCH_CALENDAR
 */
int
_soc_td3_tdm_set_idb_pkt_calendar(int unit,
                                  soc_port_schedule_state_t *sch_info)
{
    uint32 pipe_map;
    soc_mem_t mem;
    int pipe, slot, hpipe;
    int port, phy_port, idb_port;
    uint32 memfld;
    uint32 entry[SOC_MAX_MEM_WORDS];
    static const soc_mem_t idb_pkt_shaper_mems[TRIDENT3_TDM_PIPES_PER_DEV][2] = {
        {PKT_SCH_CALENDAR0_PIPE0m, PKT_SCH_CALENDAR1_PIPE0m},
        {PKT_SCH_CALENDAR0_PIPE1m, PKT_SCH_CALENDAR1_PIPE1m}
    };

    _soc_td3_tdm_get_pipe_map(unit, sch_info, &pipe_map);

    for (pipe = 0; pipe < TRIDENT3_TDM_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }
        /* Configure packet shaper calendar */
        for (hpipe = 0; hpipe < TRIDENT3_TDM_HPIPES_PER_PIPE; hpipe++) {
            mem = idb_pkt_shaper_mems[pipe][hpipe];
            for (slot = 0; slot < TD3_SHAPING_GRP_LEN; slot ++) {
                phy_port = sch_info->tdm_ingress_schedule_pipe[
				pipe].tdm_schedule_slice[hpipe].pkt_sch_or_ovs_space[0][slot];
                if ((phy_port >= TD3_NUM_EXT_PORTS) || (phy_port <= 0)) {
                    idb_port = TRIDENT3_TDM_IDB_UNUSED_TOKEN;
                } else {
                    port = sch_info->in_port_map.port_p2l_mapping[phy_port];
                    idb_port = sch_info->in_port_map.port_l2i_mapping[port] & 0x1f;
                }
                sal_memset(entry, 0, sizeof(uint32) * soc_mem_entry_words(unit,
											PKT_SCH_CALENDAR0_PIPE0m));
                memfld = idb_port & 0x3f;
                soc_mem_field_set(unit, mem, entry, PORT_NUMf, &memfld);
                SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
										slot, entry));
            }
        }
    }

    return SOC_E_NONE;
}


/*! @fn int _soc_td3_tdm_set_ovs_group_sel(
        int unit,
        soc_port_schedule_state_t *sch_info,
        int set_idb,
        int set_mmu)
    @param unit Chip unit number.
    @param sch_info : Pointer to a soc_port_schedule_state_t struct variable.
    @param set_idb if 1 then IDB OVS tables are configured
    @param set_mmu if 1 then MMU OVS tables are configured
    @brief API to initialize the TD3 MMU &/or IDB OVS tables.
            Regs/Mems configured: IDB HPIPE0/1_OVR_SUB_GRP_CFG,
                                  HPIPE0/1_OVR_SUB_GRP_CFG
                                  PBLK<n>_CALENDAR
 */
int
_soc_td3_tdm_set_ovs_group_sel(int unit,
                               soc_port_schedule_state_t *sch_info,
                               int set_idb, int set_mmu)
{
    uint32 pipe_map, ovs_pipe_map;
    soc_reg_t reg, reg1;
    int pipe, hpipe, group, lane, slot, id = 0, mode;
    int port, phy_port, phy_port_base, idb_port, mmu_port, inst;
    int speed_max;
    uint32 rval, rval1;
    int pblks[TRIDENT3_TDM_PIPES_PER_DEV][2][TRIDENT3_TDM_PBLKS_PER_HPIPE];
    int pm_num, pblk_cal_idx;
    int pblk_valid, pblk_spacing;
    int port_speed_class, port_same_spacing, port_sister_spacing;
    int l_speed = 0, l_num_lanes = 0, prev_or_new = 0;

    static const soc_reg_t idb_grp_cfg_regs[TRIDENT3_TDM_PIPES_PER_DEV][2] = {
        {IS_HPIPE0_OVR_SUB_GRP_CFG_PIPE0r, IS_HPIPE1_OVR_SUB_GRP_CFG_PIPE0r},
        {IS_HPIPE0_OVR_SUB_GRP_CFG_PIPE1r, IS_HPIPE1_OVR_SUB_GRP_CFG_PIPE1r}
    };

    static const soc_reg_t mmu_grp_cfg_regs[2] = {
        HPIPE0_OVR_SUB_GRP_CFGr, HPIPE1_OVR_SUB_GRP_CFGr
    };

    static const soc_reg_t idb_grp_ovs_regs[TRIDENT3_TDM_PIPES_PER_DEV][2][6] = {
        {
            {IS_HPIPE0_OVR_SUB_GRP0_TBL_PIPE0r, IS_HPIPE0_OVR_SUB_GRP1_TBL_PIPE0r,
             IS_HPIPE0_OVR_SUB_GRP2_TBL_PIPE0r, IS_HPIPE0_OVR_SUB_GRP3_TBL_PIPE0r,
             IS_HPIPE0_OVR_SUB_GRP4_TBL_PIPE0r, IS_HPIPE0_OVR_SUB_GRP5_TBL_PIPE0r },
            {IS_HPIPE1_OVR_SUB_GRP0_TBL_PIPE0r, IS_HPIPE1_OVR_SUB_GRP1_TBL_PIPE0r,
             IS_HPIPE1_OVR_SUB_GRP2_TBL_PIPE0r, IS_HPIPE1_OVR_SUB_GRP3_TBL_PIPE0r,
             IS_HPIPE1_OVR_SUB_GRP4_TBL_PIPE0r, IS_HPIPE1_OVR_SUB_GRP5_TBL_PIPE0r }
        },
        {
            {IS_HPIPE0_OVR_SUB_GRP0_TBL_PIPE1r, IS_HPIPE0_OVR_SUB_GRP1_TBL_PIPE1r,
             IS_HPIPE0_OVR_SUB_GRP2_TBL_PIPE1r, IS_HPIPE0_OVR_SUB_GRP3_TBL_PIPE1r,
             IS_HPIPE0_OVR_SUB_GRP4_TBL_PIPE1r, IS_HPIPE0_OVR_SUB_GRP5_TBL_PIPE1r },
            {IS_HPIPE1_OVR_SUB_GRP0_TBL_PIPE1r, IS_HPIPE1_OVR_SUB_GRP1_TBL_PIPE1r,
             IS_HPIPE1_OVR_SUB_GRP2_TBL_PIPE1r, IS_HPIPE1_OVR_SUB_GRP3_TBL_PIPE1r,
             IS_HPIPE1_OVR_SUB_GRP4_TBL_PIPE1r, IS_HPIPE1_OVR_SUB_GRP5_TBL_PIPE1r }
        }
    };

    static const soc_reg_t mmu_grp_ovs_regs[2][6] = {
        {HPIPE0_OVR_SUB_GRP0_TBLr, HPIPE0_OVR_SUB_GRP1_TBLr,
         HPIPE0_OVR_SUB_GRP2_TBLr, HPIPE0_OVR_SUB_GRP3_TBLr,
         HPIPE0_OVR_SUB_GRP4_TBLr, HPIPE0_OVR_SUB_GRP5_TBLr },
        {HPIPE1_OVR_SUB_GRP0_TBLr, HPIPE1_OVR_SUB_GRP1_TBLr,
         HPIPE1_OVR_SUB_GRP2_TBLr, HPIPE1_OVR_SUB_GRP3_TBLr,
         HPIPE1_OVR_SUB_GRP4_TBLr, HPIPE1_OVR_SUB_GRP5_TBLr }
    };

    static const soc_reg_t idb_pblk_cal_regs[TRIDENT3_TDM_PIPES_PER_DEV][2][8] = {
        {
            {IS_HPIPE0_PBLK0_CALENDAR_PIPE0r, IS_HPIPE0_PBLK1_CALENDAR_PIPE0r,
             IS_HPIPE0_PBLK2_CALENDAR_PIPE0r, IS_HPIPE0_PBLK3_CALENDAR_PIPE0r,
             IS_HPIPE0_PBLK4_CALENDAR_PIPE0r, IS_HPIPE0_PBLK5_CALENDAR_PIPE0r,
             IS_HPIPE0_PBLK6_CALENDAR_PIPE0r, IS_HPIPE0_PBLK7_CALENDAR_PIPE0r },
            {IS_HPIPE1_PBLK0_CALENDAR_PIPE0r, IS_HPIPE1_PBLK1_CALENDAR_PIPE0r,
             IS_HPIPE1_PBLK2_CALENDAR_PIPE0r, IS_HPIPE1_PBLK3_CALENDAR_PIPE0r,
             IS_HPIPE1_PBLK4_CALENDAR_PIPE0r, IS_HPIPE1_PBLK5_CALENDAR_PIPE0r,
             IS_HPIPE1_PBLK6_CALENDAR_PIPE0r, IS_HPIPE1_PBLK7_CALENDAR_PIPE0r }
        },
        {
            {IS_HPIPE0_PBLK0_CALENDAR_PIPE1r, IS_HPIPE0_PBLK1_CALENDAR_PIPE1r,
             IS_HPIPE0_PBLK2_CALENDAR_PIPE1r, IS_HPIPE0_PBLK3_CALENDAR_PIPE1r,
             IS_HPIPE0_PBLK4_CALENDAR_PIPE1r, IS_HPIPE0_PBLK5_CALENDAR_PIPE1r,
             IS_HPIPE0_PBLK6_CALENDAR_PIPE1r, IS_HPIPE0_PBLK7_CALENDAR_PIPE1r },
            {IS_HPIPE1_PBLK0_CALENDAR_PIPE1r, IS_HPIPE1_PBLK1_CALENDAR_PIPE1r,
             IS_HPIPE1_PBLK2_CALENDAR_PIPE1r, IS_HPIPE1_PBLK3_CALENDAR_PIPE1r,
             IS_HPIPE1_PBLK4_CALENDAR_PIPE1r, IS_HPIPE1_PBLK5_CALENDAR_PIPE1r,
             IS_HPIPE1_PBLK6_CALENDAR_PIPE1r, IS_HPIPE1_PBLK7_CALENDAR_PIPE1r }
        }
    };

    static const soc_reg_t mmu_pblk_cal_regs[2][8] = {
        {HPIPE0_PBLK0_CALENDARr, HPIPE0_PBLK1_CALENDARr,
         HPIPE0_PBLK2_CALENDARr, HPIPE0_PBLK3_CALENDARr,
         HPIPE0_PBLK4_CALENDARr, HPIPE0_PBLK5_CALENDARr,
         HPIPE0_PBLK6_CALENDARr, HPIPE0_PBLK7_CALENDARr },
        {HPIPE1_PBLK0_CALENDARr, HPIPE1_PBLK1_CALENDARr,
         HPIPE1_PBLK2_CALENDARr, HPIPE1_PBLK3_CALENDARr,
         HPIPE1_PBLK4_CALENDARr, HPIPE1_PBLK5_CALENDARr,
         HPIPE1_PBLK6_CALENDARr, HPIPE1_PBLK7_CALENDARr }
    };

    static int pblk_slots[SOC_TD3_PORT_RATIO_COUNT][7] = {
        { 0, -1,  0,  0, -1,  0, -1 }, /* SOC_TD3_PORT_RATIO_SINGLE */
        { 0, -1,  2,  0, -1,  2, -1 }, /* SOC_TD3_PORT_RATIO_DUAL_1_1 */
        { 0,  0,  2,  0,  0,  2, -1 }, /* SOC_TD3_PORT_RATIO_DUAL_2_1 */
        { 0,  2,  2,  0,  2,  2, -1 }, /* SOC_TD3_PORT_RATIO_DUAL_1_2 */
        { 0, -1,  2,  0, -1,  3, -1 }, /* SOC_TD3_PORT_RATIO_TRI_023_2_1_1 */
        { 0,  0,  2,  0,  0,  3, -1 }, /* SOC_TD3_PORT_RATIO_TRI_023_4_1_1 */
        { 0, -1,  2,  1, -1,  2, -1 }, /* SOC_TD3_PORT_RATIO_TRI_012_1_1_2 */
        { 0,  2,  2,  1,  2,  2, -1 }, /* SOC_TD3_PORT_RATIO_TRI_012_1_1_4 */
        { 0, -1,  2,  1, -1,  3, -1 }  /* SOC_TD3_PORT_RATIO_QUAD */
    };

    _soc_td3_tdm_get_pipe_map(unit, sch_info, &pipe_map);
    _soc_td3_tdm_get_ovs_pipe_map(unit, sch_info, &ovs_pipe_map);

    /* OVS tables  */
    for (pipe = 0; pipe < TRIDENT3_TDM_PIPES_PER_DEV; pipe++) {
        if (!(ovs_pipe_map & (1 << pipe))) {
            continue;
        }
        inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;
        for (hpipe = 0; hpipe < TRIDENT3_TDM_HPIPES_PER_PIPE; hpipe++) {
            for (group = 0; group < TRIDENT3_TDM_OVS_GROUPS_PER_HPIPE; group++) {
                for (slot = 0; slot < TRIDENT3_TDM_OVS_GROUP_LENGTH; slot++) {
                    phy_port = sch_info->tdm_ingress_schedule_pipe[
					pipe].tdm_schedule_slice[hpipe].oversub_schedule[group][slot];
                    /* PORT_NUMf restricts port numbering within [0-31] & 0x3f
                     *      HPIPE0 port 0-31 maps to mmu/idb port 0-31
                     *      HPIPE1 port 0-31 maps to mmu/idb port 32-63
                     * PHY_PORT_IDf restricts port block id within [0-7]
                     */
                    if (phy_port > TD3_NUM_PHY_PORTS) {
                        idb_port = 0x3f;
                        mmu_port = 0x3f;
                        id = 0x7;
                    } else {
                        port = sch_info->in_port_map.port_p2l_mapping[phy_port];
                        idb_port = sch_info->in_port_map.port_l2i_mapping[
															port] & 0x1f;
                        mmu_port = sch_info->in_port_map.port_p2m_mapping[
															phy_port] & 0x1f;
                        id = sch_info->out_port_map.port_p2PBLK_inst_mapping[
															phy_port] & 0x7;
                    }
                    /* IDB OverSub Groups */
                    if (set_idb == 1) {
                        rval = 0;
                        reg = idb_grp_ovs_regs[pipe][hpipe][group];
                        soc_reg_field_set(unit, reg, &rval, PHY_PORT_IDf, id);
                        soc_reg_field_set(unit, reg, &rval, PORT_NUMf, idb_port);
                        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY,
															slot, rval));
                    }
                    /* MMU OverSub Groups */
                    if (set_mmu == 1) {
                        rval = 0;
                        reg = mmu_grp_ovs_regs[hpipe][group];
                        soc_reg_field_set(unit, reg, &rval, PHY_PORT_IDf, id);
                        soc_reg_field_set(unit, reg, &rval, PORT_NUMf, mmu_port);
                        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst,
																slot, rval));
                    }
                }

                /* Handle the case where the first element in the group
                 * is invalid but other valid ports exist in the group.
                 */
                for (slot = 0; slot < TRIDENT3_TDM_OVS_GROUP_LENGTH; slot++) {
                    phy_port = sch_info->tdm_ingress_schedule_pipe[
					pipe].tdm_schedule_slice[hpipe].oversub_schedule[group][slot];
                    if (phy_port < TD3_NUM_EXT_PORTS) {
                        break;
                    }
                }

                /* No valid ports found, skip group */
                if (TRIDENT3_TDM_OVS_GROUP_LENGTH == slot) {
                    port_speed_class = 0;
                    port_same_spacing = 0;
                    port_sister_spacing = 0;
                } else {
                    port = sch_info->in_port_map.port_p2l_mapping[phy_port];
                    prev_or_new = 1;
                    soc_td3_tdm_get_log_port_speed(unit, sch_info, port,
                        prev_or_new, &l_speed, &l_num_lanes);
                    speed_max = 25000 * l_num_lanes;
                    if (speed_max > l_speed) speed_max = l_speed;

                    port_speed_class =
                        _soc_td3_tdm_get_speed_ovs_class(unit, speed_max);
                    port_same_spacing =
                        _soc_td3_tdm_get_same_port_spacing_e(unit, speed_max);
                    port_sister_spacing = 4;
                }
                if (set_idb == 1) {
                    rval = 0;
                    reg = idb_grp_cfg_regs[pipe][hpipe];
                    soc_reg_field_set(unit, reg, &rval, SAME_SPACINGf, 8); /* on ingress, all ports have same spacing */
                    soc_reg_field_set(unit, reg, &rval, SISTER_SPACINGf,
														port_sister_spacing);
                    soc_reg_field_set(unit, reg, &rval, SPEEDf,
														port_speed_class);
                    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY,
														group, rval));
                }
                if (set_mmu == 1) {
                    rval = 0;
                    reg = mmu_grp_cfg_regs[hpipe];
                    soc_reg_field_set(unit, reg, &rval, SAME_SPACINGf,
														port_same_spacing);
                    soc_reg_field_set(unit, reg, &rval, SISTER_SPACINGf,
														port_sister_spacing);
                    soc_reg_field_set(unit, reg, &rval, SPEEDf,
														port_speed_class);
                    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst,
														group, rval));
                }
            }
        }
    }

    /* 16 Port-blocks are supported, with each instance of Oversubscription
     * Cell Scheduler supports up to 8 Portblocks. */

    /* First get pblk_cal_idx to pm_num mapping from
     * pm_num to pblk_cal_idx
     * and **oversub_schedule
     */
    /* sal_memset(pblks, -1, sizeof(pblks)); */
    sal_memset(pblks, -1, sizeof(int) * TRIDENT3_TDM_PIPES_PER_DEV * 2 *
               TRIDENT3_TDM_PBLKS_PER_HPIPE);
    for (pipe = 0; pipe < TRIDENT3_TDM_PIPES_PER_DEV; pipe++) {
        if (!(ovs_pipe_map & (1 << pipe))) {
            continue;
        }
        for (hpipe = 0; hpipe < TRIDENT3_TDM_HPIPES_PER_PIPE; hpipe++) {
            for (group = 0; group < TRIDENT3_TDM_OVS_GROUPS_PER_HPIPE; group++) {
                for (slot = 0; slot < TRIDENT3_TDM_OVS_GROUP_LENGTH; slot++) {
                    phy_port = sch_info->tdm_ingress_schedule_pipe[
					pipe].tdm_schedule_slice[hpipe].oversub_schedule[group][slot];
                    if (phy_port < TD3_NUM_EXT_PORTS) {
                        pm_num = (phy_port - 1) / TRIDENT3_TDM_PORTS_PER_PBLK;
                        pblk_cal_idx =
						sch_info->out_port_map.port_p2PBLK_inst_mapping[phy_port];
                        if (pblk_cal_idx >= 0 &&
                            pblk_cal_idx < TRIDENT3_TDM_PBLKS_PER_HPIPE) {
                            pblks[pipe][hpipe][pblk_cal_idx] = pm_num;
                        } else {
                            return SOC_E_FAIL; /* invalid mapping */
                        }
                    }
                }
            }
        }
    }

    /* Configure PBLKs */
    for (pipe = 0; pipe < TRIDENT3_TDM_PIPES_PER_DEV; pipe++) {
        if (!(ovs_pipe_map & (1 << pipe))) {
            continue;
        }

        inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;
        for (hpipe = 0; hpipe < TRIDENT3_TDM_HPIPES_PER_PIPE; hpipe++) {
            for (pblk_cal_idx = 0; pblk_cal_idx < TRIDENT3_TDM_PBLKS_PER_HPIPE;
                    pblk_cal_idx++) {
                pm_num = pblks[pipe][hpipe][pblk_cal_idx];
                phy_port_base = 1 + (pm_num * TRIDENT3_TDM_PORTS_PER_PBLK);
                /* Get port ratio for this Port Macro */
                soc_td3_tdm_get_port_ratio(unit, sch_info, pm_num, &mode, 1);
                reg  = idb_pblk_cal_regs[pipe][hpipe][pblk_cal_idx];
                reg1 = mmu_pblk_cal_regs[hpipe][pblk_cal_idx];
                /* Write PBLK regs
                 *      Only first 6 out of 7 slots could be valid
                 */
                if ((pm_num < 0) || (pm_num > TRIDENT3_TDM_PBLKS_PER_DEV)) {
                    /* To speed-up init this branch could be skipped */
                    for (slot = 0; slot < 6; slot++) {
                        rval = 0;
                        rval1 = 0;
                        if (set_idb == 1) {
                            soc_reg_field_set(unit, reg,  &rval,  PORT_NUMf,
																	0x3f);
                            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg,
											REG_PORT_ANY, slot, rval));
                        }
                        if (set_mmu == 1) {
                            soc_reg_field_set(unit, reg1,  &rval1,  PORT_NUMf,
																	 0x3f);
                            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg1,
													inst, slot, rval1));
                        }
                    }
                } else {
                    for (slot = 0; slot < 6; slot++) {
                        rval = 0;
                        rval1 = 0;
                        lane = pblk_slots[mode][slot];
                        if (lane == -1) {
                            pblk_valid = 0;
                            idb_port = 0x3f;
                            mmu_port = 0x3f;
                            pblk_spacing = 0; /* same spacing */
                        } else {
                            pblk_valid = 1;
                            port = sch_info->in_port_map.port_p2l_mapping[
												phy_port_base + lane];
                            idb_port = sch_info->in_port_map.port_l2i_mapping[
												port] & 0x1f;
                            mmu_port = sch_info->in_port_map.port_p2m_mapping[
												phy_port_base + lane] & 0x1f;

                            prev_or_new = 1;
                            soc_td3_tdm_get_log_port_speed(unit, sch_info,
                                port, prev_or_new, &l_speed, &l_num_lanes);
                            speed_max = 25000 * l_num_lanes;
                            if (speed_max > l_speed) speed_max = l_speed;

                            port_speed_class =
                                _soc_td3_tdm_get_speed_ovs_class(unit,
											speed_max);
                            pblk_spacing =
                                _soc_td3_tdm_get_same_port_spacing_e(unit,
											speed_max);
                        }

                        soc_reg_field_set(unit, reg,  &rval,  VALIDf,
																pblk_valid);
                        soc_reg_field_set(unit, reg,  &rval,  SPACINGf,
																8);
                        soc_reg_field_set(unit, reg,  &rval,  PORT_NUMf,
																idb_port);

                        soc_reg_field_set(unit, reg1, &rval1, VALIDf,
																pblk_valid);
                        soc_reg_field_set(unit, reg1, &rval1, SPACINGf,
																pblk_spacing);
                        soc_reg_field_set(unit, reg1, &rval1, PORT_NUMf,
																mmu_port);
                        if (set_idb == 1) {
                            SOC_IF_ERROR_RETURN
                                (soc_reg32_set(unit, reg, REG_PORT_ANY,
															slot, rval));
                        }
                        if (set_mmu == 1) {
                            SOC_IF_ERROR_RETURN
                                (soc_reg32_set(unit, reg1, inst,
														slot, rval1));
                        }
                    }
                }
            }
        }
    }

    return SOC_E_NONE;
}


/*! @fn int _soc_td3_tdm_set_ovs_group(
        int unit,
        soc_port_schedule_state_t *sch_info)
    @param unit Chip unit number.
    @param sch_info Pointer to a soc_port_schedule_state_t struct variable.
    @brief API to initialize the TD3 MMU & IDB OVS tables.
            Regs/Mems configured: IDB HPIPE0/1_OVR_SUB_GRP_CFG
                                  HPIPE0/1_OVR_SUB_GRP_CFG
                                  PBLK<n>_CALENDAR
 */
int
_soc_td3_tdm_set_ovs_group(int unit, soc_port_schedule_state_t *sch_info)
{
    return (_soc_td3_tdm_set_ovs_group_sel(unit, sch_info, 1, 1));
}


/*! @fn int _soc_td3_tdm_set_mmu_calendar(
        int unit,
        soc_port_schedule_state_t *sch_info)
    @param unit Chip unit number.
    @param sch_info Pointer to a soc_port_schedule_state_t struct variable.
    @brief API to initialize the TD3 MMU main calendar.
            Regs/Mems configured: MMU TDM_CALENDAR
                                  TDM_CONFIG
 */
int
_soc_td3_tdm_set_mmu_calendar(int unit, soc_port_schedule_state_t *sch_info)
{
    uint32 pipe_map;
    uint32 cal_len;
    soc_mem_t mem;
    soc_reg_t reg;
    soc_field_t cal_end_field;
    int pipe, slot, id, length, calendar_id;
    int phy_port, mmu_port, inst;
    uint32 rval;
    uint32 memfld;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 mem_indx;
    static const soc_mem_t calendar_mems[] = {
        TDM_CALENDAR0_PIPE0m,
        TDM_CALENDAR0_PIPE1m,
        TDM_CALENDAR1_PIPE0m,
        TDM_CALENDAR1_PIPE1m
    };
    static const soc_field_t calendar_end_fields[] = {
        CAL0_ENDf, CAL1_ENDf
    };

    _soc_td3_tdm_get_pipe_map(unit, sch_info, &pipe_map);
    sal_memset(entry, 0, sizeof(uint32) * soc_mem_entry_words(unit,
								TDM_CALENDAR0_PIPE0m));
    for (pipe = 0; pipe < TRIDENT3_TDM_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }

        /* Determine calendar_id: 0 or 1 */
        reg = TDM_CONFIGr;
        inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;
        if (sch_info->is_flexport == 1) { /* choose "the other one" */
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, inst, 0, &rval));
            calendar_id = soc_reg_field_get(unit, reg, rval, CURR_CALf) ^ 1;
        } else {
            calendar_id = 0; /* At init use calendar_id=0 */
            rval = 0;
        }
        mem = calendar_mems[(calendar_id * TRIDENT3_TDM_PIPES_PER_DEV) + pipe];
        cal_end_field = calendar_end_fields[calendar_id];

        /* TDM Calendar is always taken from slice 0 */
        cal_len = sch_info->tdm_egress_schedule_pipe[
					pipe].tdm_schedule_slice[0].cal_len;
        for (length = cal_len; length > 0; length--) {
            if (sch_info->tdm_egress_schedule_pipe[pipe].tdm_schedule_slice[
					0].linerate_schedule[length - 1] != TD3_NUM_EXT_PORTS) {
                break;
            }
        }
        for (slot = 0; slot < length; slot += 2) {
            /* EVEN */
            phy_port = sch_info->tdm_egress_schedule_pipe[
				pipe].tdm_schedule_slice[0].linerate_schedule[slot];
            if (phy_port == TD3_OVSB_TOKEN) {
                mmu_port = TRIDENT3_TDM_MMU_OVSB_TOKEN;
            } else if (phy_port == TD3_IDL1_TOKEN) {
                mmu_port = TRIDENT3_TDM_MMU_PURG_TOKEN;
            } else if (phy_port == TD3_IDL2_TOKEN) {
                mmu_port = TRIDENT3_TDM_MMU_UNUSED_TOKEN;
            } else if (phy_port == TD3_NULL_TOKEN) {
                mmu_port = TRIDENT3_TDM_MMU_NULL_TOKEN;
            } else if (phy_port >= TD3_NUM_EXT_PORTS) {
                mmu_port = TRIDENT3_TDM_MMU_UNUSED_TOKEN;
            } else if (phy_port == TD3_MGM1_TOKEN) {
                /* TDM code always allocates slots for management port.
                 * If mgmt ports are omitted from config, this mapping
                 * is not set up.
                 */
                mmu_port = TRIDENT3_TDM_MMU_MGMT1_TOKEN;
            } else {
                /* [0,69] && 127 */
                mmu_port = sch_info->out_port_map.port_p2m_mapping[
						phy_port] & 0x7f;
            }
            if (mmu_port > 63) {
                id = 0xf;
            } else {
                id = (phy_port - 1) / TRIDENT3_TDM_PORTS_PER_PBLK;
            }
            memfld = mmu_port & 0x7f;
            soc_mem_field_set(unit, mem, entry, PORT_NUM_EVENf, &memfld);
            memfld = id & 0xf;
            soc_mem_field_set(unit, mem, entry, PHY_PORT_ID_EVENf, &memfld);
            /* ODD */
            phy_port = sch_info->tdm_egress_schedule_pipe[
					pipe].tdm_schedule_slice[0].linerate_schedule[slot + 1];
            if (phy_port == TD3_OVSB_TOKEN) {
                mmu_port = TRIDENT3_TDM_MMU_OVSB_TOKEN;
            } else if (phy_port == TD3_IDL1_TOKEN) {
                mmu_port = TRIDENT3_TDM_MMU_PURG_TOKEN;
            } else if (phy_port == TD3_IDL2_TOKEN) {
                mmu_port = TRIDENT3_TDM_MMU_UNUSED_TOKEN;
            } else if (phy_port == TD3_NULL_TOKEN) {
                mmu_port = TRIDENT3_TDM_MMU_NULL_TOKEN;
            } else if (phy_port >= TD3_NUM_EXT_PORTS) {
                mmu_port = TRIDENT3_TDM_MMU_UNUSED_TOKEN;
            } else if (phy_port == TD3_MGM1_TOKEN) {
                /* TDM code always allocates slots for management port.
                 * If mgmt ports are omitted from config, this mapping
                 * is not set up.
                 */
                mmu_port = TRIDENT3_TDM_MMU_MGMT1_TOKEN;
            } else {
                /* [0,69] && 127 */
                mmu_port = sch_info->out_port_map.port_p2m_mapping[
								phy_port] & 0x7f;
            }
            if (mmu_port > 63) {
                id = 0xf;
            } else {
                id = (phy_port - 1) / TRIDENT3_TDM_PORTS_PER_PBLK;
            }
            memfld = mmu_port & 0x7f;
            soc_mem_field_set(unit, mem, entry, PORT_NUM_ODDf, &memfld);
            memfld = id & 0xf;
            soc_mem_field_set(unit, mem, entry, PHY_PORT_ID_ODDf, &memfld);
            /* Memory entry */
            mem_indx = slot / 2;
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL, mem_indx, entry));
        }

        /* set for 2nd management port */
        if (_soc_td3_tdm_check_2nd_mgmt_enable(unit, sch_info) == 1 &&
            pipe == (TRIDENT3_TDM_PIPES_PER_DEV - 1)) {
            soc_reg_field_set(unit, reg, &rval, MGMT_2ND_PORT_ENABLEf, 1);
        }

        soc_reg_field_set(unit, reg, &rval, cal_end_field, length-1);
        soc_reg_field_set(unit, reg, &rval, CURR_CALf, calendar_id);
        soc_reg_field_set(unit, reg, &rval, ENABLEf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, 0, rval));
    }

    return SOC_E_NONE;
}


/*! @fn int _soc_td3_tdm_set_mmu_hsp(
        int unit,
        soc_port_schedule_state_t *sch_info)
    @param unit Chip unit number.
    @param sch_info Pointer to a soc_port_schedule_state_t struct variable.
    @brief API to initialize the TD3 MMU HSP.
            Regs/Mems configured: TDM_HSP_PIPE
 */
int
_soc_td3_tdm_set_mmu_hsp(int unit,
                         soc_port_schedule_state_t *sch_info)
{
    soc_reg_t reg;
    uint32 pipe_map;
    int pipe, hpipe, inst;
    int port, mmu_port, phy_port;
    uint32 port_map[TRIDENT3_TDM_PIPES_PER_DEV][2];
    uint32 rval = 0;
    int l_speed = 0, l_num_lanes = 0, prev_or_new = 0;

    _soc_td3_tdm_get_pipe_map(unit, sch_info, &pipe_map);

    for (pipe = 0; pipe < TRIDENT3_TDM_PIPES_PER_DEV; pipe++) {
        port_map[pipe][0] = 0;
        port_map[pipe][1] = 0;
    }

    for (port=0; port<TRIDENT3_TDM_PHY_PORTS_PER_DEV; port++) {
        if (sch_info->out_port_map.log_port_speed[port] > 0) {
            soc_td3_tdm_get_log_port_speed(unit, sch_info, port,
                prev_or_new, &l_speed, &l_num_lanes);
            if (_soc_td3_tdm_check_is_hsp_port_e(unit, l_speed) == 1) {
                phy_port = sch_info->out_port_map.port_l2p_mapping[port];
                if ((phy_port >= 1) && (phy_port <= TRIDENT3_TDM_GPORTS_PER_DEV)) {
                    pipe = (phy_port - 1) / TRIDENT3_TDM_GPORTS_PER_PIPE;
                    hpipe = ((phy_port - 1) & 0x3f) /
                            TRIDENT3_TDM_GPORTS_PER_HPIPE;
                    mmu_port = sch_info->out_port_map.port_p2m_mapping[
							phy_port] & 0x1f;
                    if (pipe < TRIDENT3_TDM_PIPES_PER_DEV && hpipe < 2) {
                        port_map[pipe][hpipe] |= 1 << mmu_port;
                    }
                }
            }
        }
    }

    for (pipe = 0; pipe < TRIDENT3_TDM_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }
        inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;

        reg = TDM_HSP_0r;
        soc_reg_field_set(unit, reg, &rval, PORT_BMPf, port_map[pipe][0]);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, 0, rval));

        reg = TDM_HSP_1r;
        soc_reg_field_set(unit, reg, &rval, PORT_BMPf, port_map[pipe][1]);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, 0, rval));
    }

    return SOC_E_NONE;
}


/*! @fn int _soc_td3_tdm_set_mmu_opportunistic(
        int unit,
        soc_port_schedule_state_t *sch_info,
        int cpu_op_en,
        int lb_opp_en,
        int opp1_port_en,
        int opp2_port_en,
        int opp_ovr_sub_en)
    @param unit Chip unit number.
    @param sch_info Pointer to a soc_port_schedule_state_t struct variable.
    @param cpu_op_en
    @param lb_opp_en
    @param opp1_port_en
    @param opp2_port_en
    @param opp_ovr_sub_en
    @brief API to initialize the TD3 MMU Opportunistic related registers
            Regs/Mems configured: CPU_LB_OPP_CFG
                                  OPP_SCHED_CFG
 */
int
_soc_td3_tdm_set_mmu_opportunistic(int unit,
                                   soc_port_schedule_state_t *sch_info,
                                   int cpu_op_en,
                                   int lb_opp_en,
                                   int opp1_port_en,
                                   int opp2_port_en,
                                   int opp_ovr_sub_en)
{
    uint32 pipe_map;
    soc_reg_t reg;
    int pipe;
    int inst;
    uint32 rval;

    _soc_td3_tdm_get_pipe_map(unit, sch_info, &pipe_map);

    for (pipe = 0; pipe < TRIDENT3_TDM_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }

        inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;

        reg = CPU_LB_OPP_CFGr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, inst, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, CPU_OPP_ENf,
										(cpu_op_en == 1) ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, LB_OPP_ENf,
										(lb_opp_en == 1) ? 1 : 0);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, 0, rval));

        reg = OPP_SCHED_CFGr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, inst, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, OPP1_PORT_ENf,
											(opp1_port_en == 1) ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, OPP1_SPACINGf, 6);
        soc_reg_field_set(unit, reg, &rval, OPP2_PORT_ENf,
											(opp2_port_en == 1) ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, OPP2_SPACINGf, 0);
        soc_reg_field_set(unit, reg, &rval, OPP_OVR_SUB_ENf,
											(opp_ovr_sub_en == 1) ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, OPP_STRICT_PRIf, 0);
        soc_reg_field_set(unit, reg, &rval, DISABLE_PORT_NUMf, 67);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, 0, rval));
    }

    return SOC_E_NONE;
}


/*! @fn int soc_td3_tdm_init(
        int unit,
        soc_port_schedule_state_t *sch_info)
    @param unit Chip unit number.
    @param sch_info Pointer to a soc_port_schedule_state_t struct variable.
    @brief Main API for TDM init
 */
int soc_td3_tdm_init(int unit, soc_port_schedule_state_t *sch_info)
{
    SOC_IF_ERROR_RETURN(_soc_td3_tdm_calculation(unit, sch_info));

    SOC_IF_ERROR_RETURN(_soc_td3_tdm_set_idb_calendar(unit, sch_info));
    SOC_IF_ERROR_RETURN(_soc_td3_tdm_set_idb_hsp(unit, sch_info));
    SOC_IF_ERROR_RETURN(_soc_td3_tdm_set_idb_pkt_calendar(unit, sch_info));
    SOC_IF_ERROR_RETURN(_soc_td3_tdm_set_idb_ppe_credit(unit, sch_info,
                                                        16, 9, 16, 9));
    /* IDB opp1_port_en should always be 1 to enable sbus access to the rest of IPIPE*/
    SOC_IF_ERROR_RETURN(_soc_td3_tdm_set_idb_opportunistic(unit, sch_info,
                                                           1, 1, 1, 1, 1));

    SOC_IF_ERROR_RETURN(_soc_td3_tdm_set_ovs_group(unit, sch_info));
    SOC_IF_ERROR_RETURN(_soc_td3_tdm_set_mmu_calendar(unit, sch_info));
    SOC_IF_ERROR_RETURN(_soc_td3_tdm_set_mmu_hsp(unit, sch_info));
    SOC_IF_ERROR_RETURN(_soc_td3_tdm_set_mmu_opportunistic(unit, sch_info,
                                                           1, 1, 1, 1, 1));

    return SOC_E_NONE;
}

/*! @fn int soc_trident3_port_speed_higig2eth(int speed)
 *  @param int speed
 *  @brief Map port speed to ETH bitrates
 */
static int
soc_trident3_port_speed_higig2eth(int speed)
{
    switch (speed) {
    case 11000:
        /* 10G */
        return 10000;
    case 21000:
        /* 20G */
        return 20000;
    case 27000:
        /* 25G */
        return 25000;
    case 42000:
        /* 40G */
        return 40000;
    case 53000:
        /* 50G */
        return 50000;
    case 106000:
        /* 100G */
        return 100000;
    default:
        return speed;
    }
}

/*! @fn void soc_trident3_port_schedule_speed_remap(int unit,
 *               soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable.
 *  @brief API to remap speeds to ETH bitrates
 */
void
soc_trident3_port_schedule_speed_remap(
    int  unit,
    soc_port_schedule_state_t *port_schedule_state)
{
    int i, port;

    /* Speed remap for in_port_map*/
    for (port=0; port < SOC_MAX_NUM_PORTS; port++) {
        port_schedule_state->in_port_map.log_port_speed[port] =
            soc_trident3_port_speed_higig2eth(
				port_schedule_state->in_port_map.log_port_speed[port]);
    }

    if (port_schedule_state->is_flexport == 1) {
        /* Speed remap for out_port_map*/
        for (port=0; port < SOC_MAX_NUM_PORTS; port++) {
            port_schedule_state->out_port_map.log_port_speed[port] =
                soc_trident3_port_speed_higig2eth(
				port_schedule_state->out_port_map.log_port_speed[port]);
        }
        /* Speed remap for resource */
        for (i = 0; i < port_schedule_state->nport; i++) {
            if (-1 != port_schedule_state->resource[i].physical_port) { /* that is, port up */
                port_schedule_state->resource[i].speed =
                    soc_trident3_port_speed_higig2eth(
						port_schedule_state->resource[i].speed);
            }
        }
    }
}

/*! @fn int soc_td3_tdm_get_log_port_speed(int unit,
 *   soc_port_schedule_state_t *port_schedule_state,
 *  int log_port,
 *  int prev_or_new,
 *  int *speed,
 *  int *num_lanes)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable.
 *  @param log_port: logical port
 *  @param prev_or_new: if = 1 use in_port_map, otherwise use out_port_map.
 *  int *speed: port speed returned
 *  int *num_lanes: num lanes returned
 *  @brief helper func to get the scheduler speed and num lanes
 */
int
soc_td3_tdm_get_log_port_speed(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int log_port,
    int prev_or_new,
    int *speed,
    int *num_lanes)
{
    soc_port_map_type_t *port_map;

    if (prev_or_new == 1) {
        port_map = &port_schedule_state->in_port_map;
    } else {
        port_map = &port_schedule_state->out_port_map;
    }

    *speed = port_map->log_port_speed[log_port];
    *num_lanes = port_map->port_num_lanes[log_port];

    /* If 25G should act as 50G in the scheduler, change speed and num_lanes */
    if (SOC_PBMP_MEMBER(port_map->physical_pbm, log_port) &&
        (*speed == 25000) ) {
        *speed = 50000;
        *num_lanes = 2;
    }

    return SOC_E_NONE;
}

/*** END SDK API COMMON CODE ***/

#endif /* BCM_TRIDENT3_SUPPORT */
