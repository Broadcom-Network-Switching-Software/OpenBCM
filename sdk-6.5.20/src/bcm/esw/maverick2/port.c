/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        port.c
 * Purpose:     Port Management for MAVERICK 2
 *
 *              The purpose is to set port management implementation specified
 *              to MAVERICK 2 in this file.
 */

#include <soc/defs.h>

#if defined(BCM_MAVERICK2_SUPPORT)
#include <soc/trident3.h>
#include <soc/maverick2.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/trident3.h>
#include <bcm_int/esw/maverick2.h>

#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/portctrl.h>

#include <soc/flexport/maverick2/maverick2_flexport.h>
#include <soc/flexport/maverick2/maverick2_flexport_defines.h>

static bcmi_xgs5_dev_info_t  *bcmi_mv2_dev_info[SOC_MAX_NUM_DEVICES];

/* HW Definitions */
static bcmi_xgs5_port_drv_t    bcmi_mv2_port_drv;

/*
 * Function:
 *      bcmi_mv2_port_dev_info_init
 * Purpose:
 *      Initialize the device info.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
STATIC int
bcmi_mv2_port_dev_info_init(int unit)
{
    bcmi_xgs5_dev_info_t *dev_info;
    soc_info_t *si = &SOC_INFO(unit);
    int alloc_size;
    int pipe = 0, phy_port, blk, bindex;
    int i, cntmaxsize;

    /* Free bcmi_td3_dev_info[unit] if already allocated */
    if (bcmi_mv2_dev_info[unit]) {
        sal_free(bcmi_mv2_dev_info[unit]);
        bcmi_mv2_dev_info[unit] = NULL;
    }

    /* Allocate memory for main SW data structure */
    alloc_size = sizeof(bcmi_xgs5_dev_info_t) +
                 (sizeof(bcmi_xgs5_phy_port_t) * MAVERICK2_PHY_PORTS_PER_DEV);
    dev_info = sal_alloc(alloc_size, "bcmi_mv2_dev_info");
    if (dev_info == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(dev_info, 0, alloc_size);
    dev_info->phy_port_info = (bcmi_xgs5_phy_port_t *)&dev_info[1];
    bcmi_mv2_dev_info[unit] = dev_info;

    /* Clear data structure */

    /* Device information init */
    dev_info = bcmi_mv2_dev_info[unit];
    dev_info->phy_ports_max = MAVERICK2_PHY_PORTS_PER_DEV + 1;
    dev_info->mmu_lossless = 0;
    dev_info->asf_prof_default = _SOC_ASF_MEM_PROFILE_SIMILAR;
    dev_info->pipe_bound = 1;
    /* First logical port in pipe   */
    dev_info->pipe_log_port_base[pipe] = 0;

    /* First physical port in pipe  */
    dev_info->pipe_phy_port_base[pipe] = 0;

    dev_info->ports_pipe_max[pipe] = MAVERICK2_PHY_PORTS_PER_PIPE;

    /* 10G is the min speed in TDM calculations */
    dev_info->tdm_pipe_lr_bw_limit_checker_speed_min = SPEED_10G;

    /* 10G is the min speed in TDM calendar program.
     * TDM eventually sets minimum speed to 10G in TDM calendar.
     */
    dev_info->tdm_speed_min = SPEED_1G;

    cntmaxsize = soc_property_get(unit, spn_BCM_STAT_JUMBO, 1518);
    if (cntmaxsize > si->max_mtu) {
        cntmaxsize = si->max_mtu;
    }
    dev_info->cntmaxsize_xl = cntmaxsize;
    dev_info->cntmaxsize_cl = cntmaxsize;

    dev_info->restriction_mask = BCMI_XGS5_FLEXPORT_RESTRICTION_MIX_LR_OVS_DEV |
                            BCMI_XGS5_FLEXPORT_RESTRICTION_SPEED_CLASS |
                            BCMI_XGS5_FLEXPORT_RESTRICTION_PM_OVS_MIX_SISTER |
                            BCMI_XGS5_FLEXPORT_RESTRICTION_PM_SINGLE_PLL;

    dev_info->encap_mask = BCM_PA_ENCAP_IEEE | BCM_PA_ENCAP_HIGIG2;

    /*
     * Restriction 13:No port configurations with more than 5 port speed
     *                classes are supported.
     */
    dev_info->speed_class.speed_class_num = _MV2_TDM_SPEED_CLASS_MAX;

    dev_info->port_ratio_mask = BCMI_XGS5_PORT_RATIO_SINGLE |
                                    BCMI_XGS5_PORT_RATIO_DUAL_1_1 |
                                    BCMI_XGS5_PORT_RATIO_DUAL_2_1 |
                                    BCMI_XGS5_PORT_RATIO_DUAL_1_2 |
                                    BCMI_XGS5_PORT_RATIO_TRI_023_2_1_1 |
                                    BCMI_XGS5_PORT_RATIO_TRI_023_4_1_1 |
                                    BCMI_XGS5_PORT_RATIO_TRI_012_1_1_2 |
                                    BCMI_XGS5_PORT_RATIO_TRI_012_1_1_4 |
                                    BCMI_XGS5_PORT_RATIO_QUAD;

    dev_info->pipe_lr_bw = 2000000;

    /* Set valid speeds for 1/2/4 Lanes */
    BCM_IF_ERROR_RETURN
        (soc_td3_support_speeds(unit, 1, &dev_info->speed_valid[1]));
    BCM_IF_ERROR_RETURN
        (soc_td3_support_speeds(unit, 2, &dev_info->speed_valid[2]));
    BCM_IF_ERROR_RETURN
        (soc_td3_support_speeds(unit, 4, &dev_info->speed_valid[4]));

    /*
     * Set physical port info:
     *  - Port lane capability
     *  - Port is enabled for flexport
     *  - pipe
     */
    for (phy_port = 0; phy_port < MAVERICK2_PHY_PORTS_PER_DEV; phy_port++) {
        /* Set information to invalid */
        dev_info->phy_port_info[phy_port].pipe = -1;
    }

    /* Flexport is not supported for CPU/LB/MNG ports, so skip CPU/LB/MNG */
    for (phy_port = 1; phy_port < MAVERICK2_PHY_PORTS_PER_DEV; phy_port++) {
        int lanes;

        /* Check for end of port list */
        if (SOC_PORT_BLOCK(unit, phy_port) < 0 &&
            SOC_PORT_BINDEX(unit, phy_port) < 0) {
            break;
        }
        /* Set pipe information */
        dev_info->phy_port_info[phy_port].pipe = 0;

        for (i = 0; i < SOC_DRIVER(unit)->port_num_blktype; i++) {
            blk = SOC_PORT_IDX_BLOCK(unit, phy_port, i);
            bindex = SOC_PORT_IDX_BINDEX(unit, phy_port, i);

            if ((blk < 0) || (bindex < 0)) {
                continue;
            }

            if (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_CLPORT) {
                /* Set port lane capabilites */
                dev_info->phy_port_info[phy_port].lanes_valid =
                                                BCMI_XGS5_PORT_LANES_1;
                if (bindex == 0) {
                    dev_info->phy_port_info[phy_port].lanes_valid |=
                        BCMI_XGS5_PORT_LANES_2 | BCMI_XGS5_PORT_LANES_4;
                } else if (bindex == 2) {
                    dev_info->phy_port_info[phy_port].lanes_valid |=
                                                BCMI_XGS5_PORT_LANES_2;
                }

                if (SHR_BITGET(si->flexible_pm_bitmap,blk)) {
                    dev_info->phy_port_info[phy_port].flex = 1;
                }
            }
        } /* for port_num_blktype */
        dev_info->phy_port_info[phy_port].max_lane_speed = 27000;

        (void)soc_mv2_port_lanes_set(unit, phy_port, &lanes,
                            &dev_info->phy_port_info[phy_port].max_lane_speed);
        if (dev_info->phy_port_info[phy_port].max_lane_speed == 11000) {
            dev_info->phy_port_info[phy_port].lanes_valid = BCMI_XGS5_PORT_LANES_1;
        }
    }

    /* Init condition */
    _bcmi_td3_init_condition_set(unit, dev_info);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_mv2_port_soc_resource_init
 * Purpose:
 *      Initialize SOC Port Structure.
 *      This includes:
 *          Copy Port information from BCM port resource array.
 *          Set correct pipe, MMU port, IDB port for new logical port.
 * Parameters:
 *      unit         - (IN)  Unit number.
 *      nport        - (IN)  Number of elements in array resource.
 *      resource     - (IN)  BCM Port resource configuration array.
 *      soc_resource - (OUT) SOC Port resource array.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
STATIC int
bcmi_mv2_port_soc_resource_init(int unit, int nport,
                                bcm_port_resource_t *resource,
                                soc_port_resource_t *soc_resource)
{
    int i;
    int logic_port, idb_port, phy_port;

    /*
     * SOC Port Structure initialization
     */

    /* Clear data structure */
    sal_memset(soc_resource, 0, sizeof(soc_port_resource_t) * nport);

    /* Copy initial information */
    for (i = 0; i < nport; i++) {
        logic_port = resource[i].port;
        phy_port = resource[i].physical_port;
        idb_port = logic_port - 1;

        if (logic_port > MAVERICK2_LOG_PORT_MNG0) {
            /* Last valid logical port number */
            LOG_CLI((BSL_META_U(unit,
                                "Logical port %d: is not valid\n"),
                                logic_port));
            return BCM_E_CONFIG;
        }

        if ((phy_port > 1) && (phy_port <= 40) && (logic_port >= 33)) {
            /* Half Pipe 0 */
            LOG_CLI((BSL_META_U(unit,
                                "Physical port %d: mapped to Half Pipe 1\n"),
                                phy_port));
            return BCM_E_CONFIG;
        } else if ((phy_port >= 41) && (phy_port <= 80) && (logic_port <= 32)) {
            /* Half Pipe 1 */
            LOG_CLI((BSL_META_U(unit,
                                "Physical port %d: mapped to Half Pipe 0\n"),
                                phy_port));
            return BCM_E_CONFIG;
        }

        soc_resource[i].flags = resource[i].flags;
        soc_resource[i].logical_port = logic_port;
        soc_resource[i].physical_port = resource[i].physical_port;
        soc_resource[i].speed = resource[i].speed;
        soc_resource[i].num_lanes = resource[i].lanes;
        soc_resource[i].encap = resource[i].encap;
        soc_resource[i].idb_port = idb_port;
        soc_resource[i].mmu_port = idb_port;
        /* MV2 does NOT support port configuration with a mix of line-rate ports
         * and oversubscribed ports. */
        soc_resource[i].oversub = 1;

        if (resource[i].speed >= 40000) {
            soc_resource[i].hsp = 1;
	    }
    }

    return BCM_E_NONE;
}

/* Function:
 *      bcmi_mv2_port_resource_validate
 * Purpose:
 *      Validate that the given FlexPort operations is valid.
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      port_schedule_state  - (OUT) Port resource state.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
STATIC int
bcmi_mv2_port_resource_validate(int unit, soc_port_schedule_state_t
                                *port_schedule_state)
{
    int phy_port, logic_port;
    int hpipe_bw[2] = {0};
    soc_port_map_type_t *post_pi = &port_schedule_state->out_port_map;

    for (phy_port = 1; phy_port <= MAVERICK2_GPHY_PORTS_PER_PIPE; phy_port++) {
        if (post_pi->port_p2l_mapping[phy_port] == -1) {
            continue;
        }
        logic_port = post_pi->port_p2l_mapping[phy_port];
        hpipe_bw[(phy_port - 1) / 40] += MV2_TDM_SPEED_ADJUST(post_pi->log_port_speed[logic_port]);
    }
    if (hpipe_bw[0] > SOC_INFO(unit).io_bandwidth / 2) {
        LOG_CLI((BSL_META_U(unit,
                 "Hpipe0 Bandwidth %dGb exceeds Max Hpipe Bandwidth %dGb\n"),
                 hpipe_bw[0] / 1000, (SOC_INFO(unit).io_bandwidth / 2) / 1000));
        return BCM_E_PARAM;
    }

    if (hpipe_bw[1] > SOC_INFO(unit).io_bandwidth / 2) {
        LOG_CLI((BSL_META_U(unit,
                 "Hpipe1 Bandwidth %dGb exceeds Max Hpipe Bandwidth %dGb\n"),
                 hpipe_bw[1] / 1000, (SOC_INFO(unit).io_bandwidth / 2) / 1000));
        return BCM_E_PARAM;
    }

    for (phy_port = 1; phy_port <= MAVERICK2_GPHY_PORTS_PER_PIPE; phy_port += MAVERICK2_PORTS_PER_PBLK) {
        if (post_pi->port_p2l_mapping[phy_port] == -1 &&
            (post_pi->port_p2l_mapping[phy_port + 1] != -1 ||
             post_pi->port_p2l_mapping[phy_port + 2] != -1 ||
             post_pi->port_p2l_mapping[phy_port + 3] != -1)) {
            return BCM_E_PARAM;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_mv2_pre_flexport_tdm
 * Purpose:
 *      Pass TDM information pre FlexPort to DV FlexPort API
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      port_schedule_state  - (OUT) Port resource state.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
STATIC int
bcmi_mv2_pre_flexport_tdm(int unit,
                            soc_port_schedule_state_t *port_schedule_state)
{
    port_schedule_state->is_flexport = 1;

    BCM_IF_ERROR_RETURN
        (soc_mv2_port_schedule_tdm_init(unit, port_schedule_state));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_mv2_post_flexport_tdm
 * Purpose:
 *      Update TDM information in SOC with post FlexPort TDM state return from
 *      DV FlexPort API.
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      port_schedule_state  - (IN) Port resource state.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
STATIC int
bcmi_mv2_post_flexport_tdm(int unit,
                            soc_port_schedule_state_t *port_schedule_state)
{
    BCM_IF_ERROR_RETURN
        (soc_mv2_soc_tdm_update(unit, port_schedule_state));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_mv2_port_enable
 * Purpose:
 *      Enable port.
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      port_schedule_state  - (IN) Port resource state.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      This function should only be called with PORTMOD enabled
 */
STATIC int
bcmi_mv2_port_enable(int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int i, nport, rv = BCM_E_NONE;
    bcm_port_t *port;

    nport = port_schedule_state->nport;

    port = sal_alloc(nport * sizeof(bcm_port_t), "port array");
    if (port == NULL) {
        return BCM_E_MEMORY;
    }
    for (i = 0; i < nport; i++) {
        port[i] = port_schedule_state->resource[i].logical_port;
    }

    rv = soc_maverick2_flex_mmu_port_up(unit, port_schedule_state);
    if (BCM_SUCCESS(rv)) {
        rv = soc_maverick2_flex_ep_port_up(unit, port_schedule_state);
    }
    if (BCM_SUCCESS(rv)) {
        rv = soc_maverick2_flex_idb_port_up(unit, port_schedule_state);
    }
    if (BCM_SUCCESS(rv)) {
        rv = bcmi_esw_portctrl_mac_up(unit, nport, port);
    }
    sal_free(port);
    return rv;
}

/*
 * Function:
 *      bcmi_mv2_port_disable
 * Purpose:
 *      Disable port.
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      port_schedule_state  - (IN) Port resource state.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      This function should only be called with PORTMOD enabled
 */
STATIC int
bcmi_mv2_port_disable(int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int i, nport, rv = BCM_E_NONE;
    bcm_port_t *port;

    nport = port_schedule_state->nport;

    port = sal_alloc(nport * sizeof(bcm_port_t), "port array");
    if (port == NULL) {
        return BCM_E_MEMORY;
    }
    for (i = 0; i < nport; i++) {
        port[i] = port_schedule_state->resource[i].logical_port;
    }
    rv = bcmi_esw_portctrl_mac_rx_down(unit, nport, port);
    if (BCM_SUCCESS(rv)) {
        rv = soc_maverick2_flex_idb_port_down(unit, port_schedule_state);
    }
    if (BCM_SUCCESS(rv)) {
        rv = soc_maverick2_flex_mmu_port_down(unit, port_schedule_state);
    }
    if (BCM_SUCCESS(rv)) {
        rv = soc_maverick2_flex_ep_port_down(unit, port_schedule_state);
    }
    if (BCM_SUCCESS(rv)) {
        rv = bcmi_esw_portctrl_mac_tx_down(unit, nport, port);
    }

    sal_free(port);

    return rv;
}

/*
 * Function:
 *      bcmi_mv2_port_speed_1g_update
 * Purpose:
 *      Special operations for 1G speed.
 * Parameters:
 *      unit                 - (IN) Unit number.
 *      port                 - (IN) Logical port number.
 *      speed                - (IN) port speed (Mbps).
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      In the PRD, nothing below 10G is officially supported.
 *      We enable customers with limited 1G support in Maverick 2.
 *      1G is not a support speed class in TDM calculation, thus treated same as
 *      10G in TDM calculation. 1G port does NOT support cut through, flow
 *      control.
 */
int
bcmi_mv2_port_speed_1g_update(int unit, bcm_port_t port, int speed)
{
    if (speed != SPEED_1G) {
        return BCM_E_NONE;
    }

    /* Update port speed info in soc_info */
    SOC_INFO(unit).port_init_speed[port] = speed;

    /* Set asf mode to SAF */
    if (soc_feature(unit, soc_feature_asf_multimode)) {
        BCM_IF_ERROR_RETURN
            (soc_mv2_port_asf_mode_set(unit, port, speed,
                                    _SOC_TD3_ASF_MODE_SAF));
    }

    /* Disable Pause */
    BCM_IF_ERROR_RETURN
        (bcm_esw_port_pause_set(unit, port, 0, 0));

    /* Disable PFC */
    if (soc_feature(unit, soc_feature_priority_flow_control)) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_control_set(unit, port,
                                      bcmPortControlPFCReceive, 0));
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_control_set(unit, port,
                                      bcmPortControlPFCTransmit, 0));
    }

    return BCM_E_NONE;
}

STATIC int
bcmi_mv2_port_attach_mmu(int unit, int port)
{
    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port) ||
        IS_MANAGEMENT_PORT(unit, port)) {
        return BCM_E_PARAM;
    }

    /* Assign the base queues and number of queues for the specified port. */
    BCM_IF_ERROR_RETURN
        (soc_maverick2_num_cosq_init_port(unit, port));

    /* Enable enqueing packets to the port */
    SOC_IF_ERROR_RETURN(
        soc_td3_port_mmu_tx_enable_set(unit, port, 1));
    SOC_IF_ERROR_RETURN(
        soc_td3_port_mmu_rx_enable_set(unit, port, 1));

    /* Reconfigure THDI, THDO settings */
    BCM_IF_ERROR_RETURN
        (soc_mv2_mmu_config_init_port(unit, port));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_mv2_port_detach_asf
 * Purpose:
 *      Update ASF when port deleted during Flexport operations.
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      port                 - (IN)  Logical port number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
STATIC int
bcmi_mv2_port_detach_asf(int unit, bcm_port_t port)
{
    BCM_IF_ERROR_RETURN
        (soc_mv2_port_asf_detach(unit, port));

    return BCM_E_NONE;
}


/*
 * Function Vectors
 */

static bcm_esw_port_drv_t bcm_mv2_port_drv = {
    /* fn_drv_init                   */ bcmi_mv2_port_fn_drv_init,
    /* resource_set                  */ bcmi_xgs5_port_resource_set,
    /* resource_get                  */ bcmi_xgs5_port_resource_get,
    /* resource_multi_set            */ bcmi_xgs5_port_resource_multi_set,
    /* resource_speed_set            */ bcmi_xgs5_port_speed_set,
    /* resource_traverse             */ bcmi_xgs5_port_resource_traverse,
    /* port_redirect_set             */ NULL,
    /* port_redirect_get             */ NULL,
    /* port_enable_set               */ bcmi_xgs5_port_enable_set,
    /* encap_speed_check             */ bcmi_xgs5_port_encap_speed_check,
    /* force_lb_set                  */ NULL,
    /* force_lb_check                */ NULL,
    /* resource_status_update        */ bcmi_xgs5_port_resource_status_update,
    /* port_resource_speed_multi_set */ bcmi_xgs5_port_resource_speed_multi_set,
    /* device_speed_check            */ NULL,
};

/* Device programming routines */
static bcmi_xgs5_port_func_t bcmi_mv2_port_calls = {
    /* reconfigure_ports        */ soc_maverick2_reconfigure_ports,
    /* soc_resource_init        */ bcmi_mv2_port_soc_resource_init,
    /* port_resource_validate   */ bcmi_mv2_port_resource_validate,
    /* pre_flexport_tdm         */ bcmi_mv2_pre_flexport_tdm,
    /* post_flexport_tdm        */ bcmi_mv2_post_flexport_tdm,
    /* port_macro_update        */ bcmi_td3_clport_update,
    /* port_enable              */ bcmi_mv2_port_enable,
    /* port_disable             */ bcmi_mv2_port_disable,
    /* no_tdm_speed_update      */ bcmi_mv2_port_speed_1g_update,
    /* speed_ability_get        */ bcmi_td3_speed_ability_get,
    /* speed_mix_validate       */ NULL,
    /* cos_map_update           */ bcmi_td3_port_cosmap_update,
};

static
int (*mv2_port_attach_exec[PORT_ATTACH_EXEC__MAX])(int unit, bcm_port_t port) = {
    /* MMU config init */
    &bcmi_mv2_port_attach_mmu,

    /* PORT module (bcm_esw_port_init) */
    &bcmi_xgs5_port_attach_dscp,
    &bcmi_xgs5_port_attach_software,
    &bcmi_xgs5_port_attach_egr_vlan_action,
    &bcmi_xgs5_port_cfg_init,
    &bcmi_xgs5_port_attach_port_probe,
    &bcmi_xgs5_port_attach_frame_length_check,
    &bcmi_xgs5_port_attach_rcpu_mtu,
    &bcmi_xgs5_port_attach_outer_tpid,
    &bcmi_xgs5_port_egr_block_profile_init,
    &bcmi_xgs5_port_attach_vlan_protocol,
    &bcmi_xgs5_port_attach_eee,
    &bcmi_xgs5_port_attach_higig,
    &bcmi_xgs5_port_attach_port_info_cfg,
    &bcmi_xgs5_port_attach_egr_link_delay,

    /* Other modules related to port */
    &bcmi_xgs5_port_attach_stg,
    &bcmi_xgs5_port_attach_vlan,
    &bcmi_xgs5_port_attach_trunk,
    &bcm_td3_cosq_port_attach,
    &bcmi_xgs5_port_attach_linkscan,
    &bcmi_xgs5_port_attach_stat,
    &bcmi_td3_port_attach_stack,
    &bcmi_xgs5_port_attach_rate,
    &bcmi_xgs5_port_attach_field,
    &bcmi_xgs5_port_attach_mirror,
    &bcmi_td3_port_attach_l3,
    &bcmi_xgs5_port_attach_ipmc,
#if defined(INCLUDE_L3)
    &bcm_th_ipmc_repl_port_attach,
#else
    NULL,
#endif
    &bcmi_xgs5_port_attach_mpls,
    &bcmi_xgs5_port_attach_mim,
};

static
int (*mv2_port_detach_exec[PORT_DETACH_EXEC__MAX])(int unit, bcm_port_t port) = {
    /* Other modules related to port */
#if defined(INCLUDE_L3)
    &bcm_th_ipmc_repl_port_detach,
#else
    NULL,
#endif
    &bcmi_xgs5_port_detach_ipmc,
    &bcmi_xgs5_port_detach_mpls,
    &bcmi_xgs5_port_detach_mim,
    &bcmi_td3_port_detach_l3,
    &bcmi_xgs5_port_detach_mirror,
    &bcmi_xgs5_port_detach_field,
    &bcmi_xgs5_port_detach_rate,
    &bcmi_td3_port_detach_stack,
    &bcm_td3_cosq_port_detach,
    &bcmi_xgs5_port_detach_trunk,
    &bcmi_xgs5_port_detach_vlan,
    &bcmi_xgs5_port_detach_stg,

    /* PORT module (_bcm_esw_port_deinit) */
    &bcmi_mv2_port_detach_asf,
    &bcmi_xgs5_port_detach_egr_link_delay,
    &bcmi_xgs5_port_detach_higig,
    &bcmi_xgs5_port_detach_eee,
    &bcmi_xgs5_port_detach_vlan_protocol,
    &bcmi_xgs5_port_egr_block_profile_init,
    &bcmi_xgs5_port_detach_phy,
    &bcmi_xgs5_port_detach_dscp,
    &bcmi_xgs5_port_cfg_init,
    &bcmi_xgs5_port_detach_egr_vlan_action,
    &bcmi_xgs5_port_detach_software,
};

/*
 * Function:
 *      bcmi_mv2_port_fn_drv_init
 * Purpose:
 *      Initialize the Port function driver.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcmi_mv2_port_fn_drv_init(int unit)
{
    int phase_pos, present;

    BCM_IF_ERROR_RETURN
        (bcmi_mv2_port_dev_info_init(unit));

    /* BCMI Driver init */
    bcmi_mv2_port_drv.port_calls = &bcmi_mv2_port_calls;
    bcmi_mv2_port_drv.dev_info[unit] = bcmi_mv2_dev_info[unit];


    sal_memcpy(bcmi_mv2_port_drv.port_calls->port_attach_exec,
               mv2_port_attach_exec,
               sizeof(bcmi_mv2_port_drv.port_calls->port_attach_exec));

    sal_memcpy(bcmi_mv2_port_drv.port_calls->port_detach_exec,
               mv2_port_detach_exec,
               sizeof(bcmi_mv2_port_drv.port_calls->port_detach_exec));

    /* Initialize Common XGS5 Port module */
    BCM_IF_ERROR_RETURN
        (bcmi_xgs5_port_fn_drv_init(unit,
                                    &bcm_mv2_port_drv,
                                    &bcmi_mv2_port_drv,
                                    NULL));


    BCM_IF_ERROR_RETURN(soc_check_flex_phase(unit, mv2_flexport_phases,
                                             soc_maverick2_flex_top_port_down,
                                             &present));
    /* Detach port enable/disable in DV Flexport */
    if (present) {
        BCM_IF_ERROR_RETURN
            (soc_detach_flex_phase(unit, mv2_flexport_phases,
                                   soc_maverick2_flex_top_port_down,
                                   &phase_pos));
    }
    BCM_IF_ERROR_RETURN(soc_check_flex_phase(unit, mv2_flexport_phases,
                                             soc_maverick2_flex_top_port_up,
                                             &present));
    if (present) {
        BCM_IF_ERROR_RETURN
            (soc_detach_flex_phase(unit, mv2_flexport_phases,
                                   soc_maverick2_flex_top_port_up,
                                   &phase_pos));
    }

    return BCM_E_NONE;
}

#else /* BCM_MAVERICK2_SUPPORT */
typedef int _mv2_port_not_empty; /* Make ISO compilers happy. */

#endif  /* BCM_MAVERICK2_SUPPORT */
