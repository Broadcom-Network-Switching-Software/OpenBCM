/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        port.c
 * Purpose:     Port Management for HELIX 5
 *
 *              The purpose is to set port management implementation specified
 *              to HELIX 5 in this file.
 */

#include <soc/defs.h>

#if defined(BCM_HELIX5_SUPPORT)
#include <soc/trident3.h>
#include <soc/helix5.h>

#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/trident3.h>
#include <bcm_int/esw/helix5.h>

#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/portctrl.h>

#include <soc/flexport/helix5/helix5_flexport.h>
#include <soc/flexport/helix5/helix5_flexport_defines.h>

static bcmi_xgs5_dev_info_t  *bcmi_hx5_dev_info[SOC_MAX_NUM_DEVICES];

/* HW Definitions */
static bcmi_xgs5_port_drv_t    bcmi_hx5_port_drv;


/*
 * Function:
 *      bcmi_hx5_port_dev_info_init
 * Purpose:
 *      Initialize the device info.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcmi_hx5_port_dev_info_init(int unit)
{
    bcmi_xgs5_dev_info_t *dev_info;
    soc_info_t *si = &SOC_INFO(unit);
    int alloc_size;
    int pipe = 0, phy_port, blk, bindex;
    int i, cntmaxsize;

    /* Free bcmi_hx5_dev_info[unit] if already allocated */
    if (bcmi_hx5_dev_info[unit]) {
        sal_free(bcmi_hx5_dev_info[unit]);
        bcmi_hx5_dev_info[unit] = NULL;
    }

    /* Allocate memory for main SW data structure */
    alloc_size = sizeof(bcmi_xgs5_dev_info_t) +
                 (sizeof(bcmi_xgs5_phy_port_t) * (HELIX5_PHY_PORTS_PER_DEV));
    dev_info = sal_alloc(alloc_size, "bcmi_hx5_dev_info");
    if (dev_info == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(dev_info, 0, alloc_size);
    dev_info->phy_port_info = (bcmi_xgs5_phy_port_t *)&dev_info[1];
    bcmi_hx5_dev_info[unit] = dev_info;

    /* Clear data structure */

    /* Device information init */
    dev_info = bcmi_hx5_dev_info[unit];
    dev_info->phy_ports_max = HELIX5_PHY_PORTS_PER_DEV;
    dev_info->mmu_lossless = 0;
    dev_info->asf_prof_default = _SOC_ASF_MEM_PROFILE_EXTREME;
    dev_info->pipe_bound = 1;
    /* First logical port in pipe   */
    dev_info->pipe_log_port_base[pipe] = 0;

    /* First physical port in pipe  */
    dev_info->pipe_phy_port_base[pipe] = 0;

    dev_info->ports_pipe_max[pipe] = HELIX5_PHY_PORTS_PER_PIPE;

    /* 10G is the min speed in TDM calendar program.
     * TDM eventually sets minimum speed to 10G in TDM calendar.
     */
    dev_info->tdm_speed_min = SPEED_1G;

    /* 10G is the min speed in TDM calculations */
    dev_info->tdm_pipe_lr_bw_limit_checker_speed_min = SPEED_10G;

    cntmaxsize = soc_property_get(unit, spn_BCM_STAT_JUMBO, 1518);
    if (cntmaxsize > si->max_mtu) {
        cntmaxsize = si->max_mtu;
    }
    dev_info->cntmaxsize_xl = cntmaxsize;
    dev_info->cntmaxsize_cl = cntmaxsize;

    dev_info->restriction_mask = BCMI_XGS5_FLEXPORT_RESTRICTION_SPEED_CLASS |
                            BCMI_XGS5_FLEXPORT_RESTRICTION_PM_OVS_MIX_SISTER |
                            BCMI_XGS5_FLEXPORT_RESTRICTION_PM_SINGLE_PLL;

    dev_info->encap_mask = BCM_PA_ENCAP_IEEE | BCM_PA_ENCAP_HIGIG2;

    /*
     * Restriction 13:No port configurations with more than 5 port speed
     *                classes are supported.
     */
    dev_info->speed_class.speed_class_num = _HX5_TDM_SPEED_CLASS_MAX;
    dev_info->speed_class.no_mix_speed_mask = 0;

    dev_info->port_ratio_mask = BCMI_XGS5_PORT_RATIO_SINGLE |
                                    BCMI_XGS5_PORT_RATIO_DUAL_1_1 |
                                    BCMI_XGS5_PORT_RATIO_DUAL_2_1 |
                                    BCMI_XGS5_PORT_RATIO_DUAL_1_2 |
                                    BCMI_XGS5_PORT_RATIO_TRI_023_2_1_1 |
                                    BCMI_XGS5_PORT_RATIO_TRI_023_4_1_1 |
                                    BCMI_XGS5_PORT_RATIO_TRI_012_1_1_2 |
                                    BCMI_XGS5_PORT_RATIO_TRI_012_1_1_4 |
                                    BCMI_XGS5_PORT_RATIO_QUAD;

    /* Set max line rate bandwidth per pipeline */
    dev_info->pipe_lr_bw = 560000;

    /* Set valid speeds for 1/2/4 Lanes */
    dev_info->speed_valid[1] = SOC_PA_SPEED_10MB | SOC_PA_SPEED_100MB |
                               SOC_PA_SPEED_1000MB | SOC_PA_SPEED_2500MB |
                               SOC_PA_SPEED_5000MB | SOC_PA_SPEED_10GB |
                               SOC_PA_SPEED_11GB | SOC_PA_SPEED_25GB | SOC_PA_SPEED_27GB;

    dev_info->speed_valid[2] = SOC_PA_SPEED_10GB | SOC_PA_SPEED_11GB |
                               SOC_PA_SPEED_20GB | SOC_PA_SPEED_21GB |
                               SOC_PA_SPEED_40GB | SOC_PA_SPEED_42GB |
                               SOC_PA_SPEED_50GB | SOC_PA_SPEED_53GB;

    dev_info->speed_valid[4] = SOC_PA_SPEED_40GB | SOC_PA_SPEED_42GB | SOC_PA_SPEED_100GB;

    /*
     * Set physical port info:
     *  - Port lane capability
     *  - Port is enabled for flexport
     *  - pipe
     */
    for (phy_port = 0; phy_port < HELIX5_PHY_PORTS_PER_DEV; phy_port++) {
        /* Set information to invalid */
        dev_info->phy_port_info[phy_port].pipe = -1;
    }

    /* Flexport is not supported for CPU/LB/MNG ports, so skip CPU/LB/MNG */
    for (phy_port = 1; phy_port < HELIX5_PHY_PORTS_PER_DEV; phy_port++) {
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

            if ((SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_CLPORT) ||
                (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_XLPORT) ||
                (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_GXPORT)) {
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
    }

    /* Init condition */
    _bcmi_td3_init_condition_set(unit, dev_info);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_hx5_port_soc_resource_init
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
bcmi_hx5_port_soc_resource_init(int unit, int nport,
                                bcm_port_resource_t *resource,
                                soc_port_resource_t *soc_resource)
{
    int i, oversub;
    int phy_port, logic_port, idb_port, mmu_port, pipe;
    int free_mmu_port[HELIX5_GDEV_PORTS_PER_PIPE] = {0};
    soc_info_t *si = &SOC_INFO(unit);

    /*
     * SOC Port Structure initialization
     */

    /* Clear data structure */
    sal_memset(soc_resource, 0, sizeof(soc_port_resource_t) * nport);
    mmu_port = 16;

    /* Copy initial information */
    for (i = 0; i < nport; i++) {
        logic_port = resource[i].port;
        phy_port = resource[i].physical_port;
        idb_port = si->port_l2i_mapping[logic_port];
        oversub = soc_hx5_port_oversub_get(unit, phy_port, logic_port);

        pipe = 0;
        if (phy_port <= 60 && !oversub &&
            !(resource[i].flags & SOC_PORT_RESOURCE_SPEED)) {
            if (phy_port < 0 && idb_port >= 16) {
                /* Delete mapping - Add assigned idb/mmu port in free list */
                free_mmu_port[idb_port] = 1;
            }

            if (phy_port > 0) {
                /* Search for first unused mmu_port */
                for (; mmu_port < HELIX5_GDEV_PORTS_PER_PIPE; mmu_port++) {
                    if ((si->port_m2p_mapping[mmu_port] == -1) ||
                        (free_mmu_port[mmu_port] == 1)){
                        idb_port = mmu_port;
                        break;
                    }
                }
                if (mmu_port >= HELIX5_GDEV_PORTS_PER_PIPE) {
                    LOG_ERROR(BSL_LS_BCM_PORT, (BSL_META_U
                              (unit, "Cannot find free MMU port\n")));

                    return BCM_E_RESOURCE;
                }
                mmu_port++;
            }
        } else if (phy_port > 0) {
            /* Fixed mapping */
            idb_port = si->port_p2m_mapping[phy_port];
        }

        soc_resource[i].flags = resource[i].flags;
        soc_resource[i].logical_port = logic_port;
        soc_resource[i].physical_port = phy_port;
        /* Pass less than 1g speeds as 1g */
        soc_resource[i].speed = resource[i].speed < 1000  ? 1000 : resource[i].speed;
        soc_resource[i].num_lanes = resource[i].lanes;
        soc_resource[i].encap = resource[i].encap;
        soc_resource[i].pipe = pipe;
        soc_resource[i].idb_port = idb_port;
        soc_resource[i].mmu_port = idb_port;
        soc_resource[i].oversub = oversub;

        if (resource[i].speed >= 40000) {
            soc_resource[i].hsp = 1;
        }
    }

    return BCM_E_NONE;
}

/* Function:
 *      bcmi_hx5_port_resource_validate
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
bcmi_hx5_port_resource_validate(int unit,
                                soc_port_schedule_state_t *port_schedule_state)
{
    int phy_port;
    soc_port_map_type_t *post_pi = &port_schedule_state->out_port_map;

    for (phy_port = 1; phy_port <= HELIX5_GPHY_PORTS_PER_PIPE; phy_port++) {
        /* Front Panel Ports */
        if (post_pi->port_p2l_mapping[phy_port] > 64) {
            LOG_CLI((BSL_META_U(unit, "Invalid logical port number %d\n"),
                                post_pi->port_p2l_mapping[phy_port]));
            return BCM_E_PARAM;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_hx5_pre_flexport_tdm
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
bcmi_hx5_pre_flexport_tdm(int unit,
                            soc_port_schedule_state_t *port_schedule_state)
{
    port_schedule_state->is_flexport = 1;

    BCM_IF_ERROR_RETURN
        (soc_hx5_port_schedule_tdm_init(unit, port_schedule_state));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_hx5_post_flexport_tdm
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
bcmi_hx5_post_flexport_tdm(int unit,
                           soc_port_schedule_state_t *port_schedule_state)
{
    int mmu_port, port;
    soc_reg_t reg;
    uint64 rval;

    BCM_IF_ERROR_RETURN
        (soc_hx5_soc_tdm_update(unit, port_schedule_state));

    /* Map unused logical port numbers to unused mmu port number after flex
     * This is needed to clear the internal mapping of logical port to mmu port
     */
    reg = MMU_PORT_TO_DEVICE_PORT_MAPPINGr;
    COMPILER_64_ZERO(rval);
    for (mmu_port = 0; mmu_port < HELIX5_GDEV_PORTS_PER_PIPE; mmu_port++) {
        SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, reg, mmu_port, 0, &rval));
        if (soc_reg64_field32_get(unit, reg, rval, DEVICE_PORTf) == 0xff) {
            /* Unused mmu port */
            for (port = 1; port <= HELIX5_GDEV_PORTS_PER_PIPE; port++) {
                if (SOC_INFO(unit).port_l2p_mapping[port] == -1) {
                    /* Unused logical port */
                    soc_reg64_field32_set(unit, reg, &rval, DEVICE_PORTf, port);
                    SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, mmu_port, 0, rval));
                }
            }
            /* Reset unused MMU Port to 0xFF */
            soc_reg64_field32_set(unit, reg, &rval, DEVICE_PORTf, 0xff);
            SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, mmu_port, 0, rval));
            break;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_hx5_port_enable
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
bcmi_hx5_port_enable(int unit, soc_port_schedule_state_t *port_schedule_state)
{
    return soc_helix5_flex_top_port_up(unit, port_schedule_state);
}

/*
 * Function:
 *      bcmi_hx5_port_disable
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
bcmi_hx5_port_disable(int unit, soc_port_schedule_state_t *port_schedule_state)
{
    return soc_helix5_flex_top_port_down(unit, port_schedule_state);
}

/*
 * Function:
 *      bcmi_hx5_speed_ability_get
 * Purpose:
 *      Retrieve the local port abilities.
 * Parameters:
 *      unit - Unit.
 *      port - Logical port num.
 *      mask - (OUT) Mask of BCM_PORT_ABILITY_ values indicating the
 *              supported speeds of the port.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
STATIC int
bcmi_hx5_speed_ability_get(int unit, bcm_port_t port, bcm_port_abil_t *mask)
{
    BCM_IF_ERROR_RETURN(soc_hx5_support_speeds(unit, port, mask));
    return BCM_E_NONE;
}

STATIC int
bcmi_hx5_port_attach_mmu(int unit, int port)
{
    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port) ||
        IS_FAE_PORT(unit, port)) {
        return BCM_E_PARAM;
    }

    /* Assign the base queues and number of queues for the specified port. */
    BCM_IF_ERROR_RETURN
        (soc_helix5_num_cosq_init_port(unit, port));

    /* Enable enqueing packets to the port */
    SOC_IF_ERROR_RETURN(
        soc_td3_port_mmu_tx_enable_set(unit, port, 1));
    SOC_IF_ERROR_RETURN(
        soc_td3_port_mmu_rx_enable_set(unit, port, 1));

    /* Reconfigure THDI, THDO settings */
    BCM_IF_ERROR_RETURN
        (soc_hx5_mmu_config_init_port(unit, port));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_hx5_port_detach_asf
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
bcmi_hx5_port_detach_asf(int unit, bcm_port_t port)
{
    return BCM_E_NONE;
}


/*
 * Function Vectors
 */

static bcm_esw_port_drv_t bcm_hx5_port_drv = {
    /* fn_drv_init                   */ bcmi_hx5_port_fn_drv_init,
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
    /* device_speed_check            */ NULL
};

/* Device programming routines */
static bcmi_xgs5_port_func_t bcmi_hx5_port_calls = {
    /* reconfigure_ports        */ soc_helix5_reconfigure_ports,
    /* soc_resource_init        */ bcmi_hx5_port_soc_resource_init,
    /* port_resource_validate   */ bcmi_hx5_port_resource_validate,
    /* pre_flexport_tdm         */ bcmi_hx5_pre_flexport_tdm,
    /* post_flexport_tdm        */ bcmi_hx5_post_flexport_tdm,
    /* port_macro_update        */ NULL,
    /* port_enable              */ bcmi_hx5_port_enable,
    /* port_disable             */ bcmi_hx5_port_disable,
    /* no_tdm_speed_update      */ bcmi_td3_port_speed_1g_update,
    /* speed_ability_get        */ bcmi_hx5_speed_ability_get,
    /* speed_mix_validate       */ NULL,
    /* cos_map_update           */ bcmi_hx5_port_cosmap_update,
};

static
int (*hx5_port_attach_exec[PORT_ATTACH_EXEC__MAX])(int unit, bcm_port_t port) = {
    /* MMU config init */
    &bcmi_hx5_port_attach_mmu,

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
    &bcm_hx5_cosq_port_attach,
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
int (*hx5_port_detach_exec[PORT_DETACH_EXEC__MAX])(int unit, bcm_port_t port) = {
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
    &bcm_hx5_cosq_port_detach,
    &bcmi_xgs5_port_detach_trunk,
    &bcmi_xgs5_port_detach_vlan,
    &bcmi_xgs5_port_detach_stg,

    /* PORT module (_bcm_esw_port_deinit) */
    &bcmi_hx5_port_detach_asf,
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
 *      bcmi_hx5_port_fn_drv_init
 * Purpose:
 *      Initialize the Port function driver.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcmi_hx5_port_fn_drv_init(int unit)
{
    int phase_pos, present;

    BCM_IF_ERROR_RETURN
        (bcmi_hx5_port_dev_info_init(unit));

    /* BCMI Driver init */
    bcmi_hx5_port_drv.port_calls = &bcmi_hx5_port_calls;
    bcmi_hx5_port_drv.dev_info[unit] = bcmi_hx5_dev_info[unit];


    sal_memcpy(bcmi_hx5_port_drv.port_calls->port_attach_exec,
               hx5_port_attach_exec,
               sizeof(bcmi_hx5_port_drv.port_calls->port_attach_exec));

    sal_memcpy(bcmi_hx5_port_drv.port_calls->port_detach_exec,
               hx5_port_detach_exec,
               sizeof(bcmi_hx5_port_drv.port_calls->port_detach_exec));

    /* Initialize Common XGS5 Port module */
    BCM_IF_ERROR_RETURN
        (bcmi_xgs5_port_fn_drv_init(unit,
                                    &bcm_hx5_port_drv,
                                    &bcmi_hx5_port_drv,
                                    NULL));

    BCM_IF_ERROR_RETURN(soc_check_flex_phase(unit, hx5_flexport_phases,
                                             soc_helix5_flex_top_port_down,
                                             &present));
    /* Detach port enable/disable in DV Flexport */
    if (present) {
        BCM_IF_ERROR_RETURN
            (soc_detach_flex_phase(unit, hx5_flexport_phases,
                                   soc_helix5_flex_top_port_down,
                                   &phase_pos));
    }
    BCM_IF_ERROR_RETURN(soc_check_flex_phase(unit, hx5_flexport_phases,
                                             soc_helix5_flex_top_port_up,
                                             &present));
    if (present) {
        BCM_IF_ERROR_RETURN
            (soc_detach_flex_phase(unit, hx5_flexport_phases,
                                   soc_helix5_flex_top_port_up,
                                   &phase_pos));
    }

    return BCM_E_NONE;
}

#endif  /* BCM_HELIX5_SUPPORT */
