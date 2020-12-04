/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        port.c
 * Purpose:     Port driver.
 */

#include <soc/defs.h>

#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <soc/tomahawk.h>
#include <soc/tomahawk3.h>
#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/portctrl.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/trident.h>
#include <bcm_int/esw/tomahawk3.h>
#include <soc/flexport/tomahawk3_flexport.h>

/*
 * Function Vector
 */
static bcm_esw_port_drv_t bcm_th3_port_drv = {
    /* fn_drv_init                   */ bcmi_th3_port_fn_drv_init,
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
    /* device_speed_check            */ bcmi_th3_device_speed_check,
};

/* Device programming routines */
static bcmi_xgs5_port_func_t bcmi_th3_port_calls = {
    /* reconfigure_ports        */ bcmi_tomahawk3_reconfigure_ports,
    /* soc_resource_init        */ bcmi_th3_port_soc_resource_init,
    /* port_resource_validate   */ bcmi_th3_port_resource_validate,
    /* pre_flexport_tdm         */ bcmi_th3_pre_flexport_tdm,
    /* post_flexport_tdm        */ NULL, 
    /* port_macro_update        */ bcmi_th3_cdport_update,
    /* port_enable              */ bcmi_th3_port_enable,
    /* port_disable             */ bcmi_th3_port_disable,
    /* no_tdm_speed_update      */ bcmi_th3_port_speed_1g_update,
    /* speed_ability_get        */ bcmi_th3_speed_ability_get,
};

static bcmi_xgs5_dev_info_t  *bcmi_th3_dev_info[SOC_MAX_NUM_DEVICES];

/* HW Definitions */
static bcmi_xgs5_port_drv_t    bcmi_th3_port_drv;

static 
int (*th3_port_attach_exec[PORT_ATTACH_EXEC__MAX])(int unit, bcm_port_t port) = {
    /* MMU config init */
    &bcmi_th3_port_attach_mmu,

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
    &bcm_th3_cosq_port_attach,
    &bcmi_xgs5_port_attach_linkscan,
    &bcmi_xgs5_port_attach_stat,
    &bcmi_xgs5_port_attach_stack,
    &bcmi_xgs5_port_attach_rate,
    &bcmi_xgs5_port_attach_field,
    &bcmi_xgs5_port_attach_mirror,
    &bcmi_xgs5_port_attach_l3,
    &bcmi_xgs5_port_attach_ipmc,
#if defined(INCLUDE_L3)
    &bcm_th3_ipmc_repl_port_attach,
#else
    NULL,
#endif
    &bcmi_xgs5_port_attach_mpls,
    &bcmi_xgs5_port_attach_mim,
};

static 
int (*th3_port_detach_exec[PORT_DETACH_EXEC__MAX])(int unit, bcm_port_t port) = {
    /* Other modules related to port */
#if defined(INCLUDE_L3)
    &bcm_th3_ipmc_repl_port_detach,
#else
    NULL,
#endif
    &bcmi_xgs5_port_detach_ipmc,
    &bcmi_xgs5_port_detach_mpls,
    &bcmi_xgs5_port_detach_mim,
    &bcmi_xgs5_port_detach_l3,
    &bcmi_xgs5_port_detach_mirror,
    &bcmi_xgs5_port_detach_field,
    &bcmi_xgs5_port_detach_rate,
    &bcmi_xgs5_port_detach_stack,
    &bcm_th3_cosq_port_detach,
    &bcmi_xgs5_port_detach_trunk,
    &bcmi_xgs5_port_detach_vlan,
    &bcmi_xgs5_port_detach_stg,

    /* PORT module (_bcm_esw_port_deinit) */
    &bcmi_th3_port_detach_asf,
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

static pbmp_t fast_reboot_epc_link_mask[SOC_MAX_NUM_DEVICES];
/*
 * Function:
 *      bcmi_th3_init_condition_set
 * Purpose:
 *      Set the info of initialized modules in dev_info.
 * Parameters:
 *      unit - (IN) Unit number.
 *      dev_info - (OUT) Device info used during flexport
 * Returns:
 *      None
 * Notes:
 *      Must follow the _bcm_modules_init()
 */
STATIC void
_bcmi_th3_init_condition_set(int unit, bcmi_xgs5_dev_info_t *dev_info)
{
    int init_all;

    init_all = soc_property_get(unit, "init_all_modules",
                                (SAL_BOOT_BCMSIM) || (!SAL_BOOT_SIMULATION));

    if (soc_feature(unit, soc_feature_virtual_switching) ||
        soc_feature(unit, soc_feature_gport_service_counters) ||
        soc_feature(unit, soc_feature_fast_init) ||
        soc_feature(unit, soc_feature_int_common_init)){
        SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_COMMON);
    }

    SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_PORT);

    if (!(SAL_BOOT_SIMULATION &&
          soc_property_get(unit, spn_SKIP_L2_VLAN_INIT, 0))) {
        SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_L2);
        SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_VLAN);
    }

    SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_STG);

    if (init_all ||
        (soc_property_get(unit, "skip_cosq_init", 0) ? FALSE : TRUE)) {
        SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_COSQ);
    }
    if (init_all ||
        (soc_property_get(unit, "skip_mirror_init", 0) ? FALSE : TRUE)) {
        SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_MIRROR);
    }
    if (init_all ||
        (soc_property_get(unit, "skip_ipmc_init", 0) ? FALSE : TRUE)) {
        SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_IPMC);
    }
    if (init_all) {
        SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_TRUNK);
        SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_MCAST);

        if (SOC_MEM_IS_VALID(unit, FP_UDF_TCAMm)) {
            SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_UDF);
        }

#ifdef BCM_FIELD_SUPPORT
        if (SOC_MEM_IS_VALID(unit, IFP_RANGE_CHECKm)) {
            SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_RANGE);
        }
        if (soc_feature(unit, soc_feature_field)) {
            SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_FIELD);
            SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_AUTH);
        }
#endif

#ifdef INCLUDE_L3
        if (soc_feature(unit, soc_feature_failover)) {
            SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_FAILOVER);
        }

        if (soc_feature(unit, soc_feature_l3) &&
            soc_property_get(unit, spn_L3_ENABLE, 1)) {
            SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_L3);
        }
        if (soc_feature(unit, soc_feature_ip_mcast)) {
            SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_IPMC);
        }

#ifdef BCM_MPLS_SUPPORT
        if (soc_feature(unit, soc_feature_mpls)) {
            SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_MPLS);
        }
#endif
        if (soc_feature(unit, soc_feature_mim)) {
            SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_MIM);
        }
        if (soc_feature(unit, soc_feature_wlan)) {
            SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_WLAN);
        }
        SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_PROXY);

        if (soc_feature(unit, soc_feature_subport) ||
            soc_feature(unit, soc_feature_linkphy_coe) ||
            soc_feature(unit, soc_feature_subtag_coe) ||
            soc_feature(unit, soc_feature_channelized_switching) ||
            soc_feature(unit, soc_feature_hgproxy_subtag_coe)) {
            SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_SUBPORT);
        }
        if (soc_feature(unit, soc_feature_trill)) {
            SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_TRILL);
        }
        if (soc_feature(unit, soc_feature_niv)) {
            SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_NIV);
        }
        if (soc_feature(unit, soc_feature_l2gre)) {
            SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_L2GRE);
        }
        if (soc_feature(unit, soc_feature_vxlan)) {
            SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_VXLAN);
        }
        if (soc_feature(unit, soc_feature_port_extension)) {
            SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_EXTENDER);
        }
        SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_MULTICAST);
#endif
        if (soc_feature(unit, soc_feature_qos_profile)) {
            SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_QOS);
        }
        if (soc_feature(unit, soc_feature_oam) ||
            soc_feature(unit, soc_feature_fp_based_oam)) {
            SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_OAM);
        }
#ifdef INCLUDE_PTP
        if (soc_feature(unit, soc_feature_ptp)) {
            SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_PTP);
        }
#endif
#ifdef INCLUDE_BFD
        if (soc_feature(unit, soc_feature_bfd)) {
            SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_BFD);
        }
#endif
        if (soc_feature(unit, soc_feature_fcoe)) {
            SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_FCOE);
        }
    }

    SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_LINKSCAN);
    SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_STAT);
#ifdef INCLUDE_KNET
    SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_KNET);
#endif
    SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_TX);
    SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_RX);
#ifdef INCLUDE_REGEX
    if (soc_feature(unit, soc_feature_regex)) {
        SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_REGEX);
    }
#endif
    if (soc_feature(unit, soc_feature_time_support)) {
        SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_TIME);
    }
    /* Service meter */
    if (soc_feature(unit, soc_feature_global_meter)) {
        SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_GLB_METER);
    }

    return;
}
#if 1
/*
 * Function:
 *      bcmi_th3_port_dev_info_init
 * Purpose:
 *      Initialize the device info.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcmi_th3_port_dev_info_init(int unit)
{
    bcmi_xgs5_dev_info_t *dev_info;
    soc_info_t *si = &SOC_INFO(unit);
    int alloc_size;
    int pipe, phy_port, blk, bindex, rsv_port;
    int i, cntmaxsize;

    /* Free bcmi_th3_dev_info[unit] if already allocated */
    if (bcmi_th3_dev_info[unit]) {
        sal_free(bcmi_th3_dev_info[unit]);
        bcmi_th3_dev_info[unit] = NULL;
    }

    /* Allocate memory for main SW data structure */
    alloc_size = sizeof(bcmi_xgs5_dev_info_t) +
                 (sizeof(bcmi_xgs5_phy_port_t) * _TH3_PHY_PORTS_PER_DEV);
    dev_info = sal_alloc(alloc_size, "bcmi_th3_dev_info");
    if (dev_info == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(dev_info, 0, alloc_size);
    dev_info->phy_port_info = (bcmi_xgs5_phy_port_t *)&dev_info[1];
    bcmi_th3_dev_info[unit] = dev_info;

    /* Clear data structure */

    /* Device information init */
    dev_info = bcmi_th3_dev_info[unit];
    dev_info->phy_ports_max = _TH3_PHY_PORTS_PER_DEV;
    dev_info->mmu_lossless = si->mmu_lossless;
    dev_info->asf_prof_default = _SOC_ASF_MEM_PROFILE_SIMILAR;
    dev_info->pipe_bound = 1;
    for (pipe = 0; pipe < _TH3_PIPES_PER_DEV; pipe++) {
        /* First logical port in pipe   */
        dev_info->pipe_log_port_base[pipe] = _TH3_DEV_PORTS_PER_PIPE * pipe;

        /* First physical port in pipe  */
        dev_info->pipe_phy_port_base[pipe] = _TH3_GPHY_PORTS_PER_PIPE * pipe +
                                             (pipe ? 1 : 0);
        /* TH3 has no rserved ports*/
        rsv_port = 0;
        dev_info->ports_pipe_max[pipe] = _TH3_DEV_PORTS_PER_PIPE - rsv_port;
    }
    /* 50G is the min speed in TDM calendar program.  However, Tomahawk 3
     * allows speeds 10G and 25G
     */
    dev_info->tdm_speed_min = SPEED_10G;

    /* 10G is the min speed in TDM calculations */
    dev_info->tdm_pipe_lr_bw_limit_checker_speed_min = SPEED_10G;

    /* There is no CDPORT_CNTMAXSIZE, but maintain code consistency for these
     * fields*/
    cntmaxsize = soc_property_get(unit, spn_BCM_STAT_JUMBO, 1518);
    if (cntmaxsize > si->max_mtu) {
        cntmaxsize = si->max_mtu;
    }
    dev_info->cntmaxsize_xl = cntmaxsize;
    dev_info->cntmaxsize_cl = cntmaxsize;

    dev_info->restriction_mask = /*BCMI_XGS5_FLEXPORT_RESTRICTION_PIPE_BANDWIDTH |
                            BCMI_XGS5_FLEXPORT_RESTRICTION_MIX_LR_OVS_DEV | */
                            BCMI_XGS5_FLEXPORT_RESTRICTION_SPEED_CLASS |
                            BCMI_XGS5_FLEXPORT_RESTRICTION_PM_DOUBLE_PLL;
                            /*BCMI_XGS5_FLEXPORT_RESTRICTION_PM_MIX_ETH_HIGIG;*/

    dev_info->encap_mask = BCM_PA_ENCAP_IEEE;

    /*
     * Restriction 13:No port configurations with more than 4 port speed
     *                classes are supported.
     *
     * Restriction 15:All port configurations with 1-3 port speed classes are
     *                supported, except configurations that contain both 20G
     *                and 25G port speeds.
     */
    dev_info->speed_class.speed_class_num = _TH3_TDM_SPEED_CLASS_MAX;

    dev_info->port_ratio_mask = BCMI_XGS5_PORT_RATIO_SINGLE |
                                    BCMI_XGS5_PORT_RATIO_DUAL_1_1 |
                                    BCMI_XGS5_PORT_RATIO_DUAL_2_1 |
                                    BCMI_XGS5_PORT_RATIO_DUAL_1_2 |
                                    BCMI_XGS5_PORT_RATIO_TRI_023_2_1_1 |
                                    BCMI_XGS5_PORT_RATIO_TRI_023_4_1_1 |
                                    BCMI_XGS5_PORT_RATIO_TRI_012_1_1_2 |
                                    BCMI_XGS5_PORT_RATIO_TRI_012_1_1_4 |
                                    BCMI_XGS5_PORT_RATIO_QUAD;

    /*
     * Set physical port info:
     *  - Port lane capability
     *  - Port is enabled for flexport
     *  - pipe
     */
    for (phy_port = 0; phy_port < _TH3_PHY_PORTS_PER_DEV; phy_port++) {
        /* Set information to invalid */
        dev_info->phy_port_info[phy_port].pipe = -1;
    }

    /* Flexport is not supported for CPU/LB/MNG ports, so skip CPU/LB/MNG */
    for (phy_port = 1; phy_port <= _TH3_GPHY_PORTS_PER_DEV; phy_port++) {
        /* Check for end of port list */
        if (SOC_PORT_BLOCK(unit, phy_port) < 0 &&
            SOC_PORT_BINDEX(unit, phy_port) < 0) {
            break;
        }
        /* Set pipe information */
        dev_info->phy_port_info[phy_port].pipe =
                                    (phy_port - 1) / _TH3_GPHY_PORTS_PER_PIPE;


        /* Set TSC info */
        dev_info->phy_port_info[phy_port].force_lb = 0;
        dev_info->phy_port_info[phy_port].max_lane_speed = 53000;

        for (i = 0; i < SOC_DRIVER(unit)->port_num_blktype; i++) {
            blk = SOC_PORT_IDX_BLOCK(unit, phy_port, i);
            bindex = SOC_PORT_IDX_BINDEX(unit, phy_port, i);

            if ((blk < 0) || (bindex < 0)) {
                continue;
            }
            
            if (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_CDPORT) {
                /* Set port lane capabilites */
                dev_info->phy_port_info[phy_port].lanes_valid =
                                                BCMI_XGS5_PORT_LANES_1;
                if (bindex == 0) {
                    dev_info->phy_port_info[phy_port].lanes_valid |=
                        BCMI_XGS5_PORT_LANES_2 | BCMI_XGS5_PORT_LANES_4 |
                        BCMI_XGS5_PORT_LANES_8;
                } else if (bindex == 2 || bindex == 6) {
                    dev_info->phy_port_info[phy_port].lanes_valid |=
                                                BCMI_XGS5_PORT_LANES_2;
                } else if (bindex == 4) {
                    dev_info->phy_port_info[phy_port].lanes_valid |=
                        BCMI_XGS5_PORT_LANES_2 | BCMI_XGS5_PORT_LANES_4;
                }

                if (SHR_BITGET(si->flexible_pm_bitmap,blk)) {
                    dev_info->phy_port_info[phy_port].flex = 1;
                }
            }
        } /* for port_num_blktype */
    }

    /* Init condition */
    _bcmi_th3_init_condition_set(unit, dev_info);

    return BCM_E_NONE;
}
#endif
/*
 * Function:
 *      bcmi_td2p_port_fn_drv_init
 * Purpose:
 *      Initialize the Port function driver.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcmi_th3_port_fn_drv_init(int unit)
{
    int phase_pos, present;

    BCM_IF_ERROR_RETURN
        (bcmi_th3_port_dev_info_init(unit));

    /* BCMI Driver init */
    sal_memset(&bcmi_th3_port_drv, 0, sizeof(bcmi_th3_port_drv));
    bcmi_th3_port_drv.port_calls = &bcmi_th3_port_calls;
    bcmi_th3_port_drv.dev_info[unit] = bcmi_th3_dev_info[unit];


    sal_memcpy(bcmi_th3_port_drv.port_calls->port_attach_exec,
               th3_port_attach_exec,
               sizeof(bcmi_th3_port_drv.port_calls->port_attach_exec));

    sal_memcpy(bcmi_th3_port_drv.port_calls->port_detach_exec,
               th3_port_detach_exec,
               sizeof(bcmi_th3_port_drv.port_calls->port_detach_exec));

    /* Initialize Common XGS5 Port module */
    BCM_IF_ERROR_RETURN
        (bcmi_xgs5_port_fn_drv_init(unit,
                                    &bcm_th3_port_drv,
                                    &bcmi_th3_port_drv,
                                    NULL));

    BCM_IF_ERROR_RETURN(soc_check_flex_phase(unit, th3_flexport_phases,
                                             soc_tomahawk3_flex_top_port_down,
                                             &present));
    /* Detach port enable/disable in DV Flexport */
    if (present) {
        BCM_IF_ERROR_RETURN
            (soc_detach_flex_phase(unit, th3_flexport_phases,
                                   soc_tomahawk3_flex_top_port_down,
                                   &phase_pos));
    }
    BCM_IF_ERROR_RETURN(soc_check_flex_phase(unit, th3_flexport_phases,
                                             soc_tomahawk3_flex_top_port_up,
                                             &present));
    if (present) {
        BCM_IF_ERROR_RETURN
            (soc_detach_flex_phase(unit, th3_flexport_phases,
                                   soc_tomahawk3_flex_top_port_up,
                                   &phase_pos));
    }

    return BCM_E_NONE;
}
/*
 * Function:
 *      bcmi_th3_port_soc_resource_init
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
int
bcmi_th3_port_soc_resource_init(int unit, int nport,
                                 bcm_port_resource_t *resource,
                                 soc_port_resource_t *soc_resource)
{
    int i;
    int logic_port, idb_port, mmu_port, pipe;

    /*
     * SOC Port Structure initialization
     */

    /* Clear data structure */
    sal_memset(soc_resource, 0, sizeof(soc_port_resource_t) * nport);

    /* Copy initial information */
    for (i = 0; i < nport; i++) {
        logic_port = resource[i].port;
#if 0
        
        if (logic_port == _TH3_DEV_RSV_PORT) {
            return BCM_E_PORT;
        }
#endif
        pipe = logic_port/_TH3_DEV_PORTS_PER_PIPE;
        idb_port = (logic_port % _TH3_DEV_PORTS_PER_PIPE) - (pipe == 0 ? 1 : 0);
        mmu_port = pipe * _TH3_MMU_PORTS_OFFSET_PER_PIPE + idb_port;

        soc_resource[i].flags = resource[i].flags;
        soc_resource[i].logical_port = logic_port;
        soc_resource[i].physical_port = resource[i].physical_port;
        soc_resource[i].speed = resource[i].speed < SPEED_10G ?
                                SPEED_10G : resource[i].speed;
        soc_resource[i].num_lanes = resource[i].lanes;
        soc_resource[i].encap = resource[i].encap;
        soc_resource[i].pipe = pipe;
        soc_resource[i].idb_port = idb_port;
        soc_resource[i].mmu_port = mmu_port;
        /* TH3 does NOT support port configuration with a mix of line-rate ports
         * and oversubscribed ports; all ports are oversub */
        soc_resource[i].oversub = 1;

        /* New Speed Config parameters required for FlexPort */
        soc_resource[i].fec_type = resource[i].fec_type;
        soc_resource[i].phy_lane_config = resource[i].phy_lane_config;
        soc_resource[i].link_training = resource[i].link_training;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_th3_port_resource_validate
 * Purpose:
 *      Validate that the given FlexPort operations is valid.
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      port_schedule_state  - (OUT) Port resource state.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcmi_th3_port_resource_validate(int unit,
                            soc_port_schedule_state_t *port_schedule_state)
{
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_th3_pre_flexport_tdm
 * Purpose:
 *      Pass TDM information pre FlexPort to DV FlexPort API
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      port_schedule_state  - (OUT) Port resource state.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcmi_th3_pre_flexport_tdm(int unit,
                            soc_port_schedule_state_t *port_schedule_state)
{
    port_schedule_state->is_flexport = 1;



    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_th3_post_flexport_tdm
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
int
bcmi_th3_post_flexport_tdm(int unit,
                            soc_port_schedule_state_t *port_schedule_state)
{


    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_th3_cdport_update
 * Purpose:
 *      Update CDPORT.
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      port_schedule_state  - (IN) Port resource state.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcmi_th3_cdport_update(int unit,
                            soc_port_schedule_state_t *port_schedule_state)
{
    /* This function is intended to update port_mode in CDPORT_MODE_REG.
     * On TH3, Portmod(PM8x50) will take care of port_mode update.
     * So we just return E_NONE in this function.
     */
    return BCM_E_NONE;
}

int
bcmi_th3_port_attach_mmu(int unit, int port)
{
    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port) ||
        IS_MANAGEMENT_PORT(unit, port)) {
        return BCM_E_PARAM;
    }
# if 0
    
    /* Assign the base queues and number of queues for the specified port. */
    BCM_IF_ERROR_RETURN
        (soc_tomahawk3_num_cosq_init_port(unit, port));

    /* Reconfigure THDI, THDO settings */
    BCM_IF_ERROR_RETURN
        (soc_th3_mmu_config_init_port(unit, port));
#endif
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_th3_port_detach_asf
 * Purpose:
 *      Update ASF when port deleted during Flexport operations.
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      port                 - (IN)  Logical port number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcmi_th3_port_detach_asf(int unit, bcm_port_t port)
{

    if (soc_feature(unit, soc_feature_asf_multimode)) {
        BCM_IF_ERROR_RETURN
            (soc_th3_port_asf_detach(unit, port));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_th3_port_enable
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
int
bcmi_th3_port_enable(int unit,
                             soc_port_schedule_state_t *port_schedule_state)
{
    int i, nport, rv = 0;
    bcm_port_t *port;

    nport = port_schedule_state->nport;

    port = sal_alloc(nport * sizeof(bcm_port_t), "port array");
    if (port == NULL) {
        return BCM_E_MEMORY;
    }
    for (i = 0; i < nport; i++) {
        port[i] = port_schedule_state->resource[i].logical_port;
    }

    if (BCM_SUCCESS(rv)) {
        rv = soc_tomahawk3_ep_port_up(unit, port_schedule_state);
    }

    if (!SAL_BOOT_BCMSIM) {
        if (BCM_SUCCESS(rv)) {
            rv = soc_tomahawk3_mmu_port_up(unit, port_schedule_state);
        }
    }

    if (!SAL_BOOT_BCMSIM) {
        if (BCM_SUCCESS(rv)) {
            rv = soc_tomahawk3_idb_port_up(unit, port_schedule_state);
        }
    }
   
    if (BCM_SUCCESS(rv)) 
    {
        /* Fast Reboot minimizes the mac up procedure by skipping sleeps
         * needed to drain the pipeline found in the normal mac up
         * procedure. This is ok because the pipeline has been previously
         * drained
         */
        if (SOC_FAST_REBOOT(unit)) {
            rv = bcmi_esw_portctrl_fast_reboot_mac_up(unit, nport, port);
        } else {
            rv = bcmi_esw_portctrl_mac_up(unit, nport, port);
        }
    }
    sal_free(port);

    return rv;
}

/*
 * Function:
 *      bcmi_th3_port_disable
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
int
bcmi_th3_port_disable(int unit,
                                soc_port_schedule_state_t *port_schedule_state)
{
    int i, nport, rv;
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
    
    if (!SAL_BOOT_BCMSIM) {
        if (BCM_SUCCESS(rv)) {
            rv = soc_tomahawk3_idb_port_down(unit, port_schedule_state);
        }
    }

    if (!SAL_BOOT_BCMSIM) {
        if (BCM_SUCCESS(rv)) {
            rv = soc_tomahawk3_mmu_port_down(unit, port_schedule_state);
        }
    }

    if (BCM_SUCCESS(rv)) {
        rv = soc_tomahawk3_ep_port_down(unit, port_schedule_state);
    }
    /* Clear EDB_CREDIT_COUNTER */
    if (BCM_SUCCESS(rv)) {
        rv = soc_tomahawk3_tdm_clear_edb_credit_counter(unit,
                                        port_schedule_state);
    }
    if (BCM_SUCCESS(rv)) 
    {
        rv = bcmi_esw_portctrl_mac_tx_down(unit, nport, port);
    }

    sal_free(port);

    return rv;
}

/*
 * Function:
 *      bcmi_th3_port_speed_1g_update
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
 *      We enable customers with limited 1G support in Tomahawk3.
 *      1G is not a support speed class in TDM calculation, thus treated same as
 *      10G in TDM calculation. 1G port does NOT support cut through, flow
 *      control.
 */
int
bcmi_th3_port_speed_1g_update(int unit, bcm_port_t port, int speed)
{
    if (speed != SPEED_1G) {
        return BCM_E_NONE;
    }

    /* Update port speed info in soc_info */
    SOC_INFO(unit).port_init_speed[port] = speed;

    /* Set asf mode to SAF */
    if (soc_feature(unit, soc_feature_asf_multimode)) {
        BCM_IF_ERROR_RETURN
            (soc_th_port_asf_mode_set(unit, port, speed,
                                      _SOC_TH_ASF_MODE_SAF));
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

/*
 * Function:
 *      bcmi_th3_speed_ability_get
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
int
bcmi_th3_speed_ability_get(int unit, bcm_port_t port, bcm_port_abil_t *mask)
{
    if (IS_MANAGEMENT_PORT(unit, port)) {
        /* TH3 Port Rates for Management ports:
         * 100M, 1G, 2.5G, 10G, HG[11G] */
        *mask = BCM_PORT_ABILITY_10MB |  BCM_PORT_ABILITY_100MB |
                BCM_PORT_ABILITY_1000MB | BCM_PORT_ABILITY_2500MB |
                BCM_PORT_ABILITY_10GB | BCM_PORT_ABILITY_11GB;
    } else {
        /* Not supported for PM8x50. */
        *mask = 0;
        return BCM_E_UNAVAIL;
#if 0
        BCM_IF_ERROR_RETURN
            (soc_th3_support_speeds(unit,
                                    SOC_INFO(unit).port_num_lanes[port],
                                    mask));
#endif
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_th3_tsc_xgxs_reset
 * Purpose:
 *      Reset TSC.
 * Parameters:
 *      unit - Unit.
 *      pbmp - Bitmap of ports to reset.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcmi_th3_tsc_xgxs_reset(int unit, pbmp_t pbmp)
{
    int rv = BCM_E_NONE;
    soc_info_t *si;
    uint64 tsc_map, reset_tsc_map;
    int tsc, ports[_TH3_PBLKS_PER_DEV];
    bcm_port_t port;
    _bcm_port_info_t *port_info = NULL;

    si = &SOC_INFO(unit);
    
    /* Construct tsc_map covered by the specified pbmp */
    COMPILER_64_ZERO(tsc_map);    
    PBMP_ITER(pbmp, port) {
        tsc = si->port_serdes[port];
        if (tsc != -1) {
            COMPILER_64_BITSET(tsc_map, tsc);
        }
    }

    COMPILER_64_ZERO(reset_tsc_map);
    PBMP_PORT_ITER(unit, port) {
        tsc = si->port_serdes[port];
        if (COMPILER_64_BITTEST(tsc_map, tsc)) {
            _bcm_port_info_access(unit, port, &port_info);            
            if (port_info->flags & _PORT_INFO_PROBE_RESET_SERDES) {
                port_info->flags &= ~_PORT_INFO_PROBE_RESET_SERDES;
                COMPILER_64_BITSET(reset_tsc_map, tsc);
                ports[tsc] = port;
            }
        }
    }
    
    for (tsc = 0; tsc < _TH3_PBLKS_PER_DEV; tsc++) {
        if (COMPILER_64_BITTEST(reset_tsc_map, tsc)) {
            rv = soc_tsc_xgxs_reset(unit, ports[tsc], 0);
        }
    }
    
    return rv;    
}

/*
 * Function:
 *      bcm_th3_port_rate_egress_set
 * Purpose:
 *      Set egress metering information
 * Parameters:
 *      unit       - (IN) Unit number
 *      port       - (IN) Port number
 *      bandwidth  - (IN) Kilobits per second or packets per second
 *                        zero if rate limiting is disabled
 *      burst      - (IN) Maximum burst size in kilobits or packets
 *      mode       - (IN) _BCM_PORT_RATE_BYTE_MODE or _BCM_PORT_RATE_PPS_MODE
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th3_port_rate_egress_set(int unit, bcm_port_t port, uint32 bandwidth,
                            uint32 burst, uint32 mode)
{
    soc_info_t *si;
    int index, pipe;
    soc_mem_t mem;
    uint32 rval, itu_mode_sel;
    mmu_mtro_egrmeteringconfig_entry_t entry;
    uint32 refresh_rate, bucketsize, granularity, flags;
    int refresh_bitsize, bucket_bitsize;

    si = &SOC_INFO(unit);

    sal_memset(&entry, 0, sizeof(entry));

    index = SOC_TH3_MMU_LOCAL_PORT(unit, port);
    pipe = si->port_pipe[port];

    mem = SOC_MEM_UNIQUE_ACC(unit, MMU_MTRO_EGRMETERINGCONFIGm)[pipe];

    /* If metering is disabled on this ingress port we are done. */
    if (!bandwidth || !burst) {
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ANY, index, &entry));
        return BCM_E_NONE;
    }
    if((bandwidth < _BCM_TH3_PORT_RATE_MIN_KILOBITS ||
        bandwidth > _BCM_TH3_PORT_RATE_MAX_KILOBITS) ||
       (burst < _BCM_PORT_BURST_MIN_KILOBITS ||
        burst > _BCM_PORT_BURST_MAX_KILOBITS)){
        return BCM_E_PARAM;
    }
    flags = mode == _BCM_PORT_RATE_PPS_MODE ?
        _BCM_TD_METER_FLAG_PACKET_MODE : 0;

    BCM_IF_ERROR_RETURN(READ_MMU_MTRO_CONFIGr(unit, &rval));
    itu_mode_sel = soc_reg_field_get(unit, MMU_MTRO_CONFIGr, rval, ITU_MODE_SELf);

    if (itu_mode_sel) {
        flags |= _BCM_TD_METER_FLAG_NON_LINEAR;
    }

    refresh_bitsize = soc_mem_field_length(unit, mem, REFRESHf);
    bucket_bitsize = soc_mem_field_length(unit, mem, THD_SELf);
    BCM_IF_ERROR_RETURN
        (_bcm_th3_rate_to_bucket_encoding(unit, bandwidth, burst, flags,
                                         refresh_bitsize, bucket_bitsize,
                                         &refresh_rate, &bucketsize,
                                         &granularity));
    soc_mem_field32_set(unit, mem, &entry, PACKET_SHAPINGf,
                    mode == _BCM_PORT_RATE_PPS_MODE ? 1 : 0);

    soc_mem_field32_set(unit, mem, &entry, METER_GRANf, granularity);
    soc_mem_field32_set(unit, mem, &entry, REFRESHf, refresh_rate);
    soc_mem_field32_set(unit, mem, &entry, THD_SELf, bucketsize);

    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, mem, MEM_BLOCK_ANY, index, &entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th3_port_rate_egress_get
 * Purpose:
 *      Get egress metering information
 * Parameters:
 *      unit       - (IN) Unit number
 *      port       - (IN) Port number
 *      bandwidth  - (OUT) Kilobits per second or packets per second
 *                         zero if rate limiting is disabled
 *      burst      - (OUT) Maximum burst size in kilobits or packets
 *      mode       - (OUT) _BCM_PORT_RATE_BYTE_MODE or _BCM_PORT_RATE_PPS_MODE
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th3_port_rate_egress_get(int unit, bcm_port_t port, uint32 *bandwidth,
                            uint32 *burst, uint32 *mode)
{
    soc_info_t *si;
    int index, pipe;
    soc_mem_t mem;
    uint32 rval, itu_mode_sel;
    mmu_mtro_egrmeteringconfig_entry_t entry;
    uint32 refresh_rate, bucketsize, granularity, flags;

    if (bandwidth == NULL || burst == NULL) {
        return BCM_E_PARAM;
    }

    si = &SOC_INFO(unit);
    index = SOC_TH3_MMU_LOCAL_PORT(unit, port);
    pipe = si->port_pipe[port];

    mem = SOC_MEM_UNIQUE_ACC(unit, MMU_MTRO_EGRMETERINGCONFIGm)[pipe];

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, &entry));

    refresh_rate = soc_mem_field32_get(unit, mem, &entry, REFRESHf);
    bucketsize = soc_mem_field32_get(unit, mem, &entry, THD_SELf);
    granularity = soc_mem_field32_get(unit, mem, &entry, METER_GRANf);

    flags = soc_mem_field32_get(unit, mem, &entry, PACKET_SHAPINGf) ?
        _BCM_TD_METER_FLAG_PACKET_MODE : 0;

    BCM_IF_ERROR_RETURN(READ_MMU_MTRO_CONFIGr(unit, &rval));
    itu_mode_sel = soc_reg_field_get(unit, MMU_MTRO_CONFIGr, rval, ITU_MODE_SELf);

    if (itu_mode_sel) {
        flags |= _BCM_TD_METER_FLAG_NON_LINEAR;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th3_bucket_encoding_to_rate(unit, refresh_rate, bucketsize,
                                         granularity, flags, bandwidth,
                                         burst));
    *mode = flags & _BCM_TD_METER_FLAG_PACKET_MODE ?
        _BCM_PORT_RATE_PPS_MODE : _BCM_PORT_RATE_BYTE_MODE;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th3_device_speed_check
 * Purpose:
 *      Restriction: Checks supported speeds for TH3
 * Parameters:
 *      unit     - (IN) Unit number
 *      speed    - (IN) speed
 *      lanes    - (IN) lanes (-1 if don't care about lanes)
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_th3_device_speed_check(int unit, int speed, int lanes)
{
    return soc_th3_device_speed_check(unit, speed, lanes);
}

/*
 * Function:
 *      bcmi_tomahawk3_reconfigure_ports
 * Purpose:
 *      To bring the TSC clock for port macros being flexed up for
 *      the flex operation, and back down if they were brought up
 * Parameters:
 *      unit                   - (IN) Unit number
 *      port_schedule_state    - (IN) Flex info
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_tomahawk3_reconfigure_ports(int unit,
            soc_port_schedule_state_t *port_schedule_state)
{
    int i;
    int nport;
    int phy_port;
    int pm_id;
    int portmod_pm_id;
    int is_init;
    int rv = BCM_E_NONE;
    char brought_up[SOC_TH3_PM8X50_COUNT];
    soc_port_resource_t *resource;

    resource = port_schedule_state->resource;
    nport = port_schedule_state->nport;

    sal_memset(brought_up, 0, SOC_TH3_PM8X50_COUNT * sizeof(char));

    /* Find ports that are going to come up after flexing and enable
     * the TSC clock so that the DV Flexport code can access
     * the registers */
    for (i = 0; i < nport; i++) {
        phy_port = resource[i].physical_port;

        /* Skip Port deletes; we only care about ports coming up because
         * they'll require the port macro to be active */
        if (phy_port == -1) {
            continue;
        }

        /* Get the SDK port macro number */
        pm_id = (phy_port - 1) / _TH3_PORTS_PER_PBLK;

        /* Skip if we already brought it up */
        if (brought_up[pm_id] == 1) {
            continue;
        }

        /* Fetch the portmod pm id since indexing is different than sdk */
        rv = portmod_phy_pm_id_get(unit, phy_port,
                    &portmod_pm_id);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }

        /* Check if the core is initialized. If it's NOT initialized,
         * we will need to bring up the clocks so that DV Flexport code can
         * access registers
         */
        rv = portmod_pm_is_initialized(unit, portmod_pm_id, &is_init);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }

        /* If the core is active, skip turning on the PM clocks; they're already on */
        if (PORTMOD_CORE_INIT_FLAG_INITIALZIED_GET(is_init)) {
            continue;
        }

        /* Enable the PM */
        rv = portmod_pm_enable(unit, portmod_pm_id, 0, 1);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }

        /* Mark that we brought up the clock on this PM so we can bring it
         * down after DV flexport
         */
        brought_up[pm_id] = 1;
    }



    /******************************
     ** Perform DV Flexport Code **
 */
    rv = soc_tomahawk3_reconfigure_ports(unit,
                        port_schedule_state);


cleanup:
    /* Disable all of the port macros that we enabled now that DV flex
     * is done */
    for (i = 0; i < SOC_TH3_PM8X50_COUNT; i++) {
        if (brought_up[i] == 1) {
            phy_port = (i * _TH3_PORTS_PER_PBLK) + 1;

            /* These shouldn't return error since they passed in the above code
             * but catch the error anyway so it'll hopefully be easier to find
             * if there is a problem calling these functions */
            BCM_IF_ERROR_RETURN(portmod_phy_pm_id_get(unit, phy_port,
                        &portmod_pm_id));
            /* Disable the PM */
            BCM_IF_ERROR_RETURN(portmod_pm_enable(unit, portmod_pm_id, 0, 0));
        }
    }

    return rv;
}

/*
 * Function:
 *      bcm_tomahawk3_port_fast_reboot_traffic_enable
 * Purpose:
 *      Enables traffic after Fast Reboot and reconfig has completed:
 * Parameters:
 *      unit     - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tomahawk3_port_fast_reboot_traffic_enable(int unit)
{
    pbmp_t mask;
    int rv = BCM_E_CONFIG;

    if (SOC_FAST_REBOOT(unit)) {

        SOC_PBMP_CLEAR(mask);
        /* Assigns ports from PBMP_ALL, which were assigned in soc_attach */
        SOC_PBMP_ASSIGN(mask, fast_reboot_epc_link_mask[unit]);
        /* Assigns ports saved from cache, which were assigned in linkscan init */
        SOC_PBMP_OR(mask, SOC_CONTROL(unit)->link_mask2);

        /* Clear the Fast Reboot flag so that we can set EPC_LINK_BMAP and
         * ING_DEST_PORT_ENABLE */
        SOC_FAST_REBOOT_DONE(unit);

        /* Set EPC_LINK_BMAP */
        rv = soc_link_mask2_set(unit, mask);
    }

    return rv;
}

/*
 * Function:
 *      bcmi_th3_port_fast_reboot_epc_link_bmap_save
 * Purpose:
 *      Saves the mask for EPC_LINK_BMAP and ING_DEST_PORT_ENABLE
 *      for when the user wants to re-enable traffic after
 *      completion of fast reboot
 * Parameters:
 *      unit        - (IN) Unit number
 *      mask        - (IN) pbmp for epc_link_bmap
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_th3_port_fast_reboot_epc_link_bmap_save(int unit, pbmp_t mask)
{
    SOC_PBMP_CLEAR(fast_reboot_epc_link_mask[unit]);
    SOC_PBMP_ASSIGN(fast_reboot_epc_link_mask[unit], mask);

    return BCM_E_NONE;
}
#endif /* BCM_TOMAHAWK3_SUPPORT */
