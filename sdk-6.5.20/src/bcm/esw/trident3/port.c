/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        port.c
 * Purpose:     Port Management for Trident 3
 *
 *              The purpose is to set port management implementation specified
 *              to Trident 3 in this file.
 */

#include <soc/defs.h>

#if defined(BCM_TRIDENT3_SUPPORT)
#include <sal/core/libc.h>
#include <soc/drv.h>
#include <soc/hash.h>
#include <soc/l2x.h>
#include <soc/trident3.h>
#include <soc/tomahawk.h>
#include <soc/esw/cancun.h>
#include <bcm/port.h>
#include <bcm/error.h>

#include <bcm_int/esw/port.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/mirror.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trident3.h>
#if defined(BCM_HELIX5_SUPPORT)
#include <bcm_int/esw/helix5.h>
#endif
#if defined(BCM_HURRICANE4_SUPPORT)
#include <bcm_int/esw/hurricane4.h>
#endif
#include <bcm_int/esw/trident.h>
#include <bcm_int/esw/tomahawk.h>

#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/portctrl.h>
#include <soc/flexport/trident3/trident3_flexport.h>
#include <soc/flexport/trident3/trident3_flexport_defines.h>
#include <include/bcm_int/esw/triumph3.h>
#include <bcm_int/esw/trident2plus.h>
/*
 * Function Vector
 */
static bcm_esw_port_drv_t bcm_td3_port_drv = {
    /* fn_drv_init                   */ bcmi_td3_port_fn_drv_init,
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
static bcmi_xgs5_port_func_t bcmi_td3_port_calls = {
    /* reconfigure_ports        */ soc_trident3_reconfigure_ports,
    /* soc_resource_init        */ bcmi_td3_port_soc_resource_init,
    /* port_resource_validate   */ bcmi_td3_port_resource_validate,
    /* pre_flexport_tdm         */ bcmi_td3_pre_flexport_tdm,
    /* post_flexport_tdm        */ bcmi_td3_post_flexport_tdm,
    /* port_macro_update        */ bcmi_td3_clport_update,
    /* port_enable              */ bcmi_td3_port_enable,
    /* port_disable             */ bcmi_td3_port_disable,
    /* no_tdm_speed_update      */ bcmi_td3_port_speed_1g_update,
    /* speed_ability_get        */ bcmi_td3_speed_ability_get,
    /* speed_mix_validate       */ bcmi_td3_speed_mix_validate,
    /* cos_map_update           */ bcmi_td3_port_cosmap_update,
};

static bcmi_xgs5_dev_info_t  *bcmi_td3_dev_info[SOC_MAX_NUM_DEVICES];

/* HW Definitions */
static bcmi_xgs5_port_drv_t    bcmi_td3_port_drv;

static
int (*td3_port_attach_exec[PORT_ATTACH_EXEC__MAX])(int unit, bcm_port_t port) = {
    /* MMU config init */
    &bcmi_td3_port_attach_mmu,

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
int (*td3_port_detach_exec[PORT_DETACH_EXEC__MAX])(int unit, bcm_port_t port) = {
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
    &bcmi_td3_port_detach_asf,
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
 *      bcmi_td3_init_condition_set
 * Purpose:
 *      Set the info of skiped modules during initialization in dev_info.
 * Parameters:
 *      unit - (IN) Unit number.
 *      dev_info - (OUT) Device info used during flexport
 * Returns:
 *      None
 * Notes:
 *      Must follow the _bcm_modules_init()
 */
void
_bcmi_td3_init_condition_set(int unit, bcmi_xgs5_dev_info_t *dev_info)
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
        (soc_property_get(unit, "skip_stack_init", 0) ? FALSE : TRUE)) {
        SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_STK);
    }
    if (init_all ||
        (soc_property_get(unit, "skip_mirror_init", 0) ? FALSE : TRUE)) {
        SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_MIRROR);
    }
    if (init_all) {
        SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_TRUNK);
        SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_MCAST);
        SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_RATE);

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
            soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
            soc_feature(unit, soc_feature_channelized_switching)) {
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
    if (soc_property_get(unit, "skip_ipmc_init", 0)) {
        SHR_BITCLR(dev_info->init_cond, SHR_BPROF_BCM_IPMC);
    }

    if (soc_property_get(unit, "skip_mpls_init", 0)) {
        SHR_BITCLR(dev_info->init_cond, SHR_BPROF_BCM_MPLS);
    }

    if (soc_property_get(unit, "skip_mim_init", 0)) {
        SHR_BITCLR(dev_info->init_cond, SHR_BPROF_BCM_MIM);
    }
    if (soc_property_get(unit, "skip_trunk_init", 0)) {
        SHR_BITCLR(dev_info->init_cond, SHR_BPROF_BCM_TRUNK);
    }
    if (soc_property_get(unit, "skip_mirror_init", 0)) {
        SHR_BITCLR(dev_info->init_cond, SHR_BPROF_BCM_MIRROR);
    }
    return;

}


/*
 * Function:
 *      bcmi_td3_port_dev_info_init
 * Purpose:
 *      Initialize the device info.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcmi_td3_port_dev_info_init(int unit)
{
    bcmi_xgs5_dev_info_t *dev_info;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 pipe_lr_bw;
    int alloc_size;
    int pipe, phy_port, blk, bindex;
    int i, cntmaxsize;

    /* Free bcmi_td3_dev_info[unit] if already allocated */
    if (bcmi_td3_dev_info[unit]) {
        sal_free(bcmi_td3_dev_info[unit]);
        bcmi_td3_dev_info[unit] = NULL;
    }

    /* Allocate memory for main SW data structure */
    alloc_size = sizeof(bcmi_xgs5_dev_info_t) +
                 (sizeof(bcmi_xgs5_phy_port_t) * TRIDENT3_PHY_PORTS_PER_DEV);
    dev_info = sal_alloc(alloc_size, "bcmi_td3_dev_info");
    if (dev_info == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(dev_info, 0, alloc_size);
    dev_info->phy_port_info = (bcmi_xgs5_phy_port_t *)&dev_info[1];
    bcmi_td3_dev_info[unit] = dev_info;

    /* Clear data structure */

    /* Device information init */
    dev_info = bcmi_td3_dev_info[unit];
    dev_info->num_pipe = _TD3_PIPES_PER_DEV;
    dev_info->num_tsc = _TD3_PBLKS_PER_DEV(unit);
    dev_info->phy_ports_max = TRIDENT3_PHY_PORTS_PER_DEV;
    dev_info->mmu_lossless = 0;
    dev_info->asf_prof_default = _SOC_ASF_MEM_PROFILE_SIMILAR;
    dev_info->pipe_bound = 1;
    for (pipe = 0; pipe < _TD3_PIPES_PER_DEV; pipe++) {
        /* First logical port in pipe   */
        dev_info->pipe_log_port_base[pipe] = TRIDENT3_PHY_PORTS_PER_PIPE * pipe;

        /* First physical port in pipe  */
        dev_info->pipe_phy_port_base[pipe] = TRIDENT3_GPHY_PORTS_PER_PIPE * pipe +
                                             (pipe ? 1 : 0);

        dev_info->ports_pipe_max[pipe] = TRIDENT3_PHY_PORTS_PER_PIPE;
    }
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

    dev_info->restriction_mask = BCMI_XGS5_FLEXPORT_RESTRICTION_PIPE_BANDWIDTH |
                            BCMI_XGS5_FLEXPORT_RESTRICTION_MIX_LR_OVS_DEV |
                            BCMI_XGS5_FLEXPORT_RESTRICTION_SPEED_CLASS |
                            BCMI_XGS5_FLEXPORT_RESTRICTION_PM_OVS_MIX_SISTER |
                            BCMI_XGS5_FLEXPORT_RESTRICTION_PM_MIX_ETH_HIGIG |
                            BCMI_XGS5_FLEXPORT_RESTRICTION_PM_LR_MIX_SISTER |
                            BCMI_XGS5_FLEXPORT_RESTRICTION_PM_HG_MIX_SPEED |
                            BCMI_XGS5_FLEXPORT_RESTRICTION_PM_SINGLE_PLL;

    dev_info->encap_mask = BCM_PA_ENCAP_IEEE | BCM_PA_ENCAP_HIGIG2;

    /*
     * Restriction 13:No port configurations with more than 4 port speed
     *                classes are supported in LR.
     * Restriction 15:All port configurations with 1-3 port speed classes are
     *                supported, except configurations that contain both 20G
     *                and 25G port speeds.
     * OS speed class exception: 5 speed classes (10/25/40/50/100) are supported
     *                           in oversub mode only.
     */
    dev_info->speed_class.speed_class_num = si->oversub_mode ?
                        _TD3_TDM_OS_SPEED_CLASS_MAX : _TD3_TDM_LR_SPEED_CLASS_MAX;
    
    dev_info->speed_class.no_mix_speed_mask = SOC_PA_SPEED_20GB
                                                | SOC_PA_SPEED_25GB;

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
    BCM_IF_ERROR_RETURN
        (soc_td3_max_lr_bandwidth(unit, &pipe_lr_bw));
    dev_info->pipe_lr_bw = pipe_lr_bw * 1000;

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
    for (phy_port = 0; phy_port < TRIDENT3_PHY_PORTS_PER_DEV; phy_port++) {
        /* Set information to invalid */
        dev_info->phy_port_info[phy_port].pipe = -1;
    }

    /* Flexport is not supported for CPU/LB/MNG ports, so skip CPU/LB/MNG */
    for (phy_port = 1; phy_port <= TRIDENT3_PHY_PORTS_PER_DEV; phy_port++) {
        /* Check for end of port list */
        if (SOC_PORT_BLOCK(unit, phy_port) < 0 &&
            SOC_PORT_BINDEX(unit, phy_port) < 0) {
            break;
        }
        /* Set pipe information */
        dev_info->phy_port_info[phy_port].pipe =
                                    (phy_port - 1) / TRIDENT3_GPHY_PORTS_PER_PIPE;

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
    }

    /* For TD3 encap type change (i.e calendar type from Ethernet type to
     * HiGig type, or vice versa)  should also be detected as a
     * flexport trigger condition
     */
    dev_info->encap_change_flex = 1;

    /* Init condition */
    _bcmi_td3_init_condition_set(unit, dev_info);

    return BCM_E_NONE;
}



/*
 * Function:
 *      bcmi_td3_port_fn_drv_init
 * Purpose:
 *      Initialize the Port function driver.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcmi_td3_port_fn_drv_init(int unit)
{
   int phase_pos, present;

    BCM_IF_ERROR_RETURN
        (bcmi_td3_port_dev_info_init(unit));

    /* BCMI Driver init */
    bcmi_td3_port_drv.port_calls = &bcmi_td3_port_calls;
    bcmi_td3_port_drv.dev_info[unit] = bcmi_td3_dev_info[unit];


    sal_memcpy(bcmi_td3_port_drv.port_calls->port_attach_exec,
               td3_port_attach_exec,
               sizeof(bcmi_td3_port_drv.port_calls->port_attach_exec));

    sal_memcpy(bcmi_td3_port_drv.port_calls->port_detach_exec,
               td3_port_detach_exec,
               sizeof(bcmi_td3_port_drv.port_calls->port_detach_exec));

    /* Initialize Common XGS5 Port module */
    BCM_IF_ERROR_RETURN
        (bcmi_xgs5_port_fn_drv_init(unit,
                                    &bcm_td3_port_drv,
                                    &bcmi_td3_port_drv,
                                    NULL));

    BCM_IF_ERROR_RETURN(soc_check_flex_phase(unit, td3_flexport_phases,
                                             soc_trident3_flex_top_port_down,
                                             &present));
    /* Detach port enable/disable in DV Flexport */
    if (present) {
        BCM_IF_ERROR_RETURN
            (soc_detach_flex_phase(unit, td3_flexport_phases,
                                   soc_trident3_flex_top_port_down,
                                   &phase_pos));
    }
    BCM_IF_ERROR_RETURN(soc_check_flex_phase(unit, td3_flexport_phases,
                                             soc_trident3_flex_top_port_up,
                                             &present));
    if (present) {
        BCM_IF_ERROR_RETURN
            (soc_detach_flex_phase(unit, td3_flexport_phases,
                                   soc_trident3_flex_top_port_up,
                                   &phase_pos));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_td3_port_soc_resource_init
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
bcmi_td3_port_soc_resource_init(int unit, int nport,
                                 bcm_port_resource_t *resource,
                                 soc_port_resource_t *soc_resource)
{
    int i;
    int logic_port, idb_port, mmu_port, pipe;
    soc_info_t *si = &SOC_INFO(unit);

    /*
     * SOC Port Structure initialization
     */

    /* Clear data structure */
    sal_memset(soc_resource, 0, sizeof(soc_port_resource_t) * nport);

    /* Copy initial information */
    for (i = 0; i < nport; i++) {
        logic_port = resource[i].port;

        pipe = logic_port / TRIDENT3_PHY_PORTS_PER_PIPE;
        idb_port = (logic_port % TRIDENT3_PHY_PORTS_PER_PIPE) - 1;
        mmu_port = (pipe << 7) | idb_port;

        soc_resource[i].flags = resource[i].flags;
        soc_resource[i].logical_port = logic_port;
        soc_resource[i].physical_port = resource[i].physical_port;
        soc_resource[i].speed = resource[i].speed;
        soc_resource[i].num_lanes = resource[i].lanes;
        soc_resource[i].encap = resource[i].encap;
        soc_resource[i].pipe = pipe;
        soc_resource[i].idb_port = idb_port;
        soc_resource[i].mmu_port = mmu_port;
        /* TD3 does NOT support port configuration with a mix of line-rate ports
         * and oversubscribed ports. */
        soc_resource[i].oversub = SOC_PBMP_IS_NULL(si->oversub_pbm) ? 0 : 1;

        if (resource[i].speed >= 40000) {
            soc_resource[i].hsp = 1;
	    }
    }

    return BCM_E_NONE;
}

/* Function:
 *      bcmi_td3_port_resource_validate
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
bcmi_td3_port_resource_validate(int unit,
                            soc_port_schedule_state_t *port_schedule_state)
{
    soc_port_resource_t *pr;
    int nport = port_schedule_state->nport;
    int i, pmap_err = 0;
    int port;       /* Logical port associated to physical port */
    int phy_port;   /* Physical port associated logical port */

    /* Allow only correct logical port to physical port association. */
    for (i = 0, pr = port_schedule_state->resource; i < nport; i++, pr++) {
        if (pr->physical_port == -1) {
            continue;
        }
        port = pr->logical_port;
        phy_port = pr->physical_port;
        if ((port >= 0) && (port <= 64)) {
            if (port != phy_port) {
                pmap_err = 1;
                break;
            }
        } else if((port == 65) && (phy_port != 130)) {
            pmap_err = 1;
            break;
        } else if ((port == 66) && (phy_port != 129)) {
            pmap_err = 1;
            break;
        } else if((port >= 67) && (port <= 130)) {
            if (port != phy_port + 2) {
                pmap_err = 1;
                break;
           }
        } else if((port == 131) && (phy_port != 131)) {
            pmap_err = 1;
            break;
        }
    }
    if (pmap_err) {
        LOG_CLI((BSL_META_U(unit,
                            "Port %d: Invalid logical port to physical port mapping.\n"),
                            port));
        return BCM_E_PORT;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_td3_pre_flexport_tdm
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
bcmi_td3_pre_flexport_tdm(int unit,
                            soc_port_schedule_state_t *port_schedule_state)
{
    port_schedule_state->is_flexport = 1;

    BCM_IF_ERROR_RETURN
        (soc_td3_port_schedule_tdm_init(unit, port_schedule_state));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_td3_post_flexport_tdm
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
bcmi_td3_post_flexport_tdm(int unit,
                            soc_port_schedule_state_t *port_schedule_state)
{
    BCM_IF_ERROR_RETURN
        (soc_td3_soc_tdm_update(unit, port_schedule_state));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_td3_clport_update
 * Purpose:
 *      Update CLPORT.
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      port_schedule_state  - (IN) Port resource state.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcmi_td3_clport_update(int unit,
                            soc_port_schedule_state_t *port_schedule_state)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_resource_t *pr;
    int i, nport;
    int phy_port, logic_port, phy_port_base;
    int blk;
    int clport, lane, mode;
    SHR_BITDCL  pm_bmp[_SHR_BITDCLSIZE(SOC_MAX_NUM_BLKS)];
    soc_port_map_type_t *in_port_map;
    uint32 rval;
    static soc_field_t port_fields[TRIDENT3_PORTS_PER_PBLK] = {
        PORT0f, PORT1f, PORT2f, PORT3f
    };
    static const int mode_encodings[SOC_TD3_PORT_RATIO_COUNT] = {
        4, 3, 3, 3, 2, 2, 1, 1, 0
    };

    SHR_BITCLR_RANGE(pm_bmp, 0, SOC_MAX_NUM_BLKS);

    nport = port_schedule_state->nport;
    in_port_map = &port_schedule_state->in_port_map;

    for (i = 0, pr = port_schedule_state->resource; i < nport; i++, pr++) {
        if (!(pr->op & ~(BCMI_XGS5_PORT_RESOURCE_OP_SPEED|
                      BCMI_XGS5_PORT_RESOURCE_OP_ENCAP))) {
            continue;
        }

        logic_port = pr->logical_port;
        phy_port = pr->physical_port == -1 ?
                    in_port_map->port_l2p_mapping[logic_port] :
                    pr->physical_port;
        blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
        SHR_BITSET(pm_bmp, blk);
    }

    SHR_BIT_ITER(pm_bmp, SOC_MAX_NUM_BLKS, blk) {
        if (!SOC_INFO(unit).block_valid[blk]) {
            continue;
        }
        logic_port = SOC_BLOCK_PORT(unit, blk);
        clport = SOC_BLOCK_NUMBER(unit, blk);
        phy_port_base = PORT_BLOCK_BASE_PORT(logic_port);

        /* Assert CLPORT soft reset */
        rval = 0;
        for (lane = 0; lane < TRIDENT3_PORTS_PER_PBLK; lane++) {
            if (si->port_p2l_mapping[phy_port_base + lane] != -1) {
                soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &rval,
                                  port_fields[lane], 1);
            }
        }
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_SOFT_RESETr(unit, logic_port, rval));

        /* Set port mode */
        soc_td3_tdm_get_port_ratio(unit, port_schedule_state, clport,
                                         &mode,
                                         0);
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, CLPORT_MODE_REGr,
                                                  logic_port, 0, &rval));
        soc_reg_field_set(unit, CLPORT_MODE_REGr, &rval, XPORT0_CORE_PORT_MODEf,
                          mode_encodings[mode]);
        soc_reg_field_set(unit, CLPORT_MODE_REGr, &rval, XPORT0_PHY_PORT_MODEf,
                          mode_encodings[mode]);
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_MODE_REGr(unit, logic_port, rval));

        /* De-assert XLPORT soft reset */
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_SOFT_RESETr(unit, logic_port, 0));
    }
    return BCM_E_NONE;
}

int
bcmi_td3_port_attach_mmu(int unit, int port)
{
    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port) ||
        IS_MANAGEMENT_PORT(unit, port)) {
        return BCM_E_PARAM;
    }

    /* Assign the base queues and number of queues for the specified port. */
    BCM_IF_ERROR_RETURN
        (soc_trident3_num_cosq_init_port(unit, port));

    /* Enable enqueing packets to the port */
    SOC_IF_ERROR_RETURN(
        soc_td3_port_mmu_tx_enable_set(unit, port, 1));
    SOC_IF_ERROR_RETURN(
        soc_td3_port_mmu_rx_enable_set(unit, port, 1));

    /* Reconfigure THDI, THDO settings */
    BCM_IF_ERROR_RETURN
        (soc_td3_mmu_config_init_port(unit, port));

    return BCM_E_NONE;
}

int
bcmi_td3_port_attach_stack(int unit, int port)
{
    modport_map_subport_entry_t subport_entry;
    modport_map_subport_mirror_entry_t subport_m_entry;

    /* Configure logical subport numbers */
    sal_memset(&subport_entry, 0, sizeof(subport_entry));
    soc_MODPORT_MAP_SUBPORTm_field32_set(unit, &subport_entry, ENABLEf, 1);
    soc_MODPORT_MAP_SUBPORTm_field32_set(unit, &subport_entry, DESTf, port);
    SOC_IF_ERROR_RETURN
        (WRITE_MODPORT_MAP_SUBPORTm(unit, MEM_BLOCK_ALL, port, &subport_entry));

    /* Configure logical subport mirror numbers */
    sal_memset(&subport_m_entry, 0, sizeof(subport_m_entry));
    soc_MODPORT_MAP_SUBPORT_M0m_field32_set(unit, &subport_m_entry, ENABLEf, 1);
    soc_MODPORT_MAP_SUBPORT_M0m_field32_set(unit,
                                            &subport_m_entry, DESTf, port);
    SOC_IF_ERROR_RETURN
        (WRITE_MODPORT_MAP_SUBPORT_MIRRORm(
            unit, MEM_BLOCK_ALL, port, &subport_m_entry));

    BCM_IF_ERROR_RETURN
            (bcmi_xgs5_port_attach_stack(unit, port));

    return BCM_E_NONE;
}

int
bcmi_td3_port_detach_stack(int unit, int port)
{
    modport_map_subport_entry_t subport_entry;
    modport_map_subport_mirror_entry_t subport_m_entry;

    /* Clear logical subport numbers */
    sal_memset(&subport_entry, 0, sizeof(subport_entry));
    SOC_IF_ERROR_RETURN
        (WRITE_MODPORT_MAP_SUBPORTm(unit, MEM_BLOCK_ALL, port, &subport_entry));

    /* Clear logical subport mirror numbers */
    sal_memset(&subport_m_entry, 0, sizeof(subport_m_entry));
    SOC_IF_ERROR_RETURN
        (WRITE_MODPORT_MAP_SUBPORT_MIRRORm(
            unit, MEM_BLOCK_ALL, port, &subport_m_entry));

    BCM_IF_ERROR_RETURN
            (bcmi_xgs5_port_detach_stack(unit, port));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_td3_port_attach_l3
 * Purpose:
 *      Enable IPv4, IPv6 on port, add to default vfi membership profile
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_td3_port_attach_l3(int unit, bcm_port_t port)
{
#ifdef INCLUDE_L3
    int rv = BCM_E_NONE;
    /* L3 */
    if (soc_feature(unit, soc_feature_l3) &&
        soc_property_get(unit, spn_L3_ENABLE, 1) &&
        !IS_ST_PORT(unit, port)) {

        BCM_IF_ERROR_RETURN
            (bcm_esw_port_control_set(unit, port, bcmPortControlIP4, 1));
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_control_set(unit, port, bcmPortControlIP6, 1));
    }
    if (soc_feature(unit, soc_feature_l3) &&
        soc_property_get(unit, spn_L3_ENABLE, 1)) {
   /* VFI */
        rv = _bcm_td2p_vfi_membership_port_attach(unit, port);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                     (BSL_META_U(unit,
                                 "Unable to add port to default VFI membership "
                                 "unit=%d port=%d rv=%d\n"),
                      unit, port, rv));
            return rv;
        }
    }
#endif /* INCLUDE_L3 */
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_td3_port_detach_l3
 * Purpose:
 *      Disable IPv4, IPv6 on port,
 *      remove port from default  vfi membership profile
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_td3_port_detach_l3(int unit, bcm_port_t port)
{
#ifdef INCLUDE_L3
    int rv = BCM_E_NONE;
    /* L3 */
    if (soc_feature(unit, soc_feature_l3) &&
        soc_property_get(unit, spn_L3_ENABLE, 1) &&
        !IS_ST_PORT(unit, port)) {

        BCM_IF_ERROR_RETURN
            (bcm_esw_port_control_set(unit, port, bcmPortControlIP4, 0));
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_control_set(unit, port, bcmPortControlIP6, 0));
    }
    if (soc_feature(unit, soc_feature_l3) &&
        soc_property_get(unit, spn_L3_ENABLE, 1)) {
    /* VFI */
       rv = _bcm_td2p_vfi_membership_port_detach(unit, port);
       if (BCM_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_BCM_PORT,
                     (BSL_META_U(unit,
                                 "Unable to clear port in default VFI membership "
                                 "unit=%d port=%d rv=%d\n"),
                      unit, port, rv));
           return rv;
       }
    }
#endif /* INCLUDE_L3 */
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_td3_port_detach_asf
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
bcmi_td3_port_detach_asf(int unit, bcm_port_t port)
{
    BCM_IF_ERROR_RETURN
        (soc_td3_port_asf_detach(unit, port));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_td3_port_enable
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
bcmi_td3_port_enable(int unit,
                             soc_port_schedule_state_t *port_schedule_state)
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

    if (BCM_SUCCESS(rv) && (!(SAL_BOOT_XGSSIM || SAL_BOOT_BCMSIM))) {
        rv = soc_trident3_flex_mmu_port_up(unit, port_schedule_state);
    }
    if (BCM_SUCCESS(rv)) {
        rv = soc_trident3_flex_ep_port_up(unit, port_schedule_state);
    }
    if (BCM_SUCCESS(rv)) {
        rv = soc_trident3_flex_idb_port_up(unit, port_schedule_state);
    }
    if (BCM_SUCCESS(rv)) {
        rv = bcmi_esw_portctrl_mac_up(unit, nport, port);
    }
    sal_free(port);
    return rv;
}

/*
 * Function:
 *      bcmi_td3_port_disable
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
bcmi_td3_port_disable(int unit,
                                soc_port_schedule_state_t *port_schedule_state)
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
        rv = soc_trident3_flex_idb_port_down(unit, port_schedule_state);
    }
    if (BCM_SUCCESS(rv) && (!(SAL_BOOT_XGSSIM || SAL_BOOT_BCMSIM))) {
        rv = soc_trident3_flex_mmu_port_down(unit, port_schedule_state);
    }
    if (BCM_SUCCESS(rv)) {
        rv = soc_trident3_flex_ep_port_down(unit, port_schedule_state);
    }
    if (BCM_SUCCESS(rv)) {
        rv = bcmi_esw_portctrl_mac_tx_down(unit, nport, port);
    }

    sal_free(port);

    return rv;
}

/*
 * Function:
 *      bcmi_td3_port_speed_1g_update
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
 *      We enable customers with limited 1G support in Trident 3.
 *      1G is not a support speed class in TDM calculation, thus treated same as
 *      10G in TDM calculation. 1G port does NOT support cut through, flow
 *      control.
 */
int
bcmi_td3_port_speed_1g_update(int unit, bcm_port_t port, int speed)
{
    if (speed != SPEED_1G) {
        return BCM_E_NONE;
    }

    /* Update port speed info in soc_info */
    SOC_INFO(unit).port_init_speed[port] = speed;

    /* Set asf mode to SAF */
    if (soc_feature(unit, soc_feature_asf_multimode)) {
        BCM_IF_ERROR_RETURN
            (soc_td3_port_asf_mode_set(unit, port, speed,
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
/*
 * Function:
 *      bcmi_td3_speed_ability_get
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
bcmi_td3_speed_ability_get(int unit, bcm_port_t port, bcm_port_abil_t *mask)
{
    if (IS_MANAGEMENT_PORT(unit, port)) {
        /* TD3 Port Rates for Management ports:
         * 10M 100M, 1G, 2.5G, 10G, HG[11G] */
        *mask = BCM_PORT_ABILITY_10MB | BCM_PORT_ABILITY_100MB |
                BCM_PORT_ABILITY_1000MB | BCM_PORT_ABILITY_2500MB |
                BCM_PORT_ABILITY_10GB | BCM_PORT_ABILITY_11GB;
    } else {
        BCM_IF_ERROR_RETURN
            (soc_td3_support_speeds(unit,
                                    SOC_INFO(unit).port_num_lanes[port],
                                    mask));
    }
    return BCM_E_NONE;
}

int _bcm_td3_port_phb_map_tab1_default_entry_add(int unit,
                                               soc_profile_mem_t *prof)
{
    phb_mapping_tbl_1_entry_t pri_map[_BCM_TD3_PHB_MAP_TAB1_ENTRIES_PER_PROFILE];
    ing_untagged_phb_entry_t phb;
    int index, pkt_pri, cfi, base = 0;
    int phb_map_tab1_entry;
    void *entries[2];

    sal_memset(pri_map, 0, sizeof(phb_mapping_tbl_1_entry_t) *
                                     _BCM_TD3_PHB_MAP_TAB1_ENTRIES_PER_PROFILE);
    sal_memset(&phb, 0, sizeof(phb));

    for (cfi = 0; cfi <= 1; cfi++) {
        for (pkt_pri = 0; pkt_pri <= 7; pkt_pri++) {
            index = (pkt_pri << 1) | cfi;
            soc_mem_field32_set(unit, PHB_MAPPING_TBL_1m,  &pri_map[index],
                                INT_PRIf, pkt_pri);
            soc_mem_field32_set(unit, PHB_MAPPING_TBL_1m,  &pri_map[index],
                                CNGf, cfi);
        }
    }
    entries[0] = &pri_map;
    entries[1] = &phb;

#if defined(BCM_HURRICANE4_SUPPORT)
    if (SOC_IS_HURRICANE4(unit)) {
        phb_map_tab1_entry = _BCM_HR4_PHB_MAP_TAB1_ENTRIES_PER_PROFILE;
    } else
#endif
#if defined(BCM_HELIX5_SUPPORT)
    if (SOC_IS_HELIX5(unit)) {
        phb_map_tab1_entry = _BCM_HX5_PHB_MAP_TAB1_ENTRIES_PER_PROFILE;
    } else
#endif
    {
        phb_map_tab1_entry = _BCM_TD3_PHB_MAP_TAB1_ENTRIES_PER_PROFILE;
    }

    BCM_IF_ERROR_RETURN
        (soc_profile_mem_add(unit, prof, entries, phb_map_tab1_entry,
                             (uint32 *)&base));
    if (base != soc_mem_index_min(unit, PHB_MAPPING_TBL_1m)) {
        return BCM_E_INTERNAL;
    }
    return BCM_E_NONE;
}

#ifdef BCM_HURRICANE4_SUPPORT
/*
 * Function:
 *      _bcm_hr4_port_cfg_init
 * Purpose:
 *      Initialize port configuration according to Initial System
 *      Configuration (see init.c)
 * Parameters:
 *      unit       - (IN)StrataSwitch unit number.
 *      port       - (IN)Port number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_hr4_port_cfg_init(int unit, bcm_port_t port)
{
#ifdef  INCLUDE_XFLOW_MACSEC
    int rv = 0;
    uint32  subport_id_to_sgpp_map[SOC_MAX_MEM_FIELD_WORDS]; /* HW entry buffer.*/
#endif
    bcm_module_t my_modid;
    int egr_gpp_index = 0;
    egr_gpp_attributes_entry_t egr_gpp_entry;

    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));

    BCM_IF_ERROR_RETURN
        (_bcm_esw_src_mod_port_table_index_get(unit, my_modid, port,
                                               &egr_gpp_index));
    BCM_IF_ERROR_RETURN
        (READ_EGR_GPP_ATTRIBUTESm(unit, MEM_BLOCK_ANY,
                                  egr_gpp_index, &egr_gpp_entry));
    soc_mem_field32_set(unit, EGR_GPP_ATTRIBUTESm, &egr_gpp_entry,
                        EGR_PP_PORT_SELECTf,
                        1);
    soc_mem_field32_set(unit, EGR_GPP_ATTRIBUTESm, &egr_gpp_entry,
                        EGR_PP_PORTf,
                        port);
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (soc_feature(unit, soc_feature_channelized_switching)) {
        soc_mem_field32_set(unit, EGR_GPP_ATTRIBUTESm, &egr_gpp_entry,
                            SUBPORT_NUMf,
                            port);
    }
#endif
    soc_mem_field32_set(unit, EGR_GPP_ATTRIBUTESm, &egr_gpp_entry,
                        LPORT_PROFILE_SELECTf,
                        0);
    BCM_IF_ERROR_RETURN
        (WRITE_EGR_GPP_ATTRIBUTESm(unit, MEM_BLOCK_ALL,
                                   egr_gpp_index, &egr_gpp_entry));
    BCM_IF_ERROR_RETURN
        (soc_mem_field32_modify(unit, EGR_PORTm, port,
                                EGR_PP_PORT_SELECTf,
                                0));
    BCM_IF_ERROR_RETURN
        (soc_mem_field32_modify(unit, EGR_PORTm, port, EGR_PP_PORTf,
                                port));
    BCM_IF_ERROR_RETURN
        (soc_mem_field32_modify(unit, EGR_PORTm, port,
                                LPORT_PROFILE_SELECTf,
                                0));

#ifdef  INCLUDE_XFLOW_MACSEC
     /* SUBPORT_ID_TO_SGPP_MAP config */
    if (soc_feature(unit, soc_feature_xflow_macsec_svtag)) {
        if (!(IS_MACSEC_PORT(unit, port) || IS_CPU_PORT(unit, port) ||
              IS_LB_PORT(unit,port) || IS_FAE_PORT(unit, port))) {
            sal_memset(&subport_id_to_sgpp_map, 0,
                       sizeof(subport_id_to_sgpp_map));
            soc_mem_field32_set(unit, SUBPORT_ID_TO_SGPP_MAPm,
                                &subport_id_to_sgpp_map, BASE_VALIDf, 0x1);
            soc_mem_field32_set(unit, SUBPORT_ID_TO_SGPP_MAPm,
                                &subport_id_to_sgpp_map, SUBPORT_IDf, port);
            soc_mem_field32_set(unit, SUBPORT_ID_TO_SGPP_MAPm,
                                &subport_id_to_sgpp_map, SGPPf,
                                ((my_modid << 0x8) | port));
            rv = soc_mem_insert(unit, SUBPORT_ID_TO_SGPP_MAPm,
                                MEM_BLOCK_ALL, &subport_id_to_sgpp_map);
            if ((rv != BCM_E_NONE) && (rv != BCM_E_EXISTS)) {
                return rv;
            }
        }
    }
#endif
    return (BCM_E_NONE);
}
#endif

/*
 * Function:
 *      _bcm_td3_port_cfg_init
 * Purpose:
 *      Initialize port configuration according to Initial System
 *      Configuration (see init.c)
 * Parameters:
 *      unit       - (IN)StrataSwitch unit number.
 *      port       - (IN)Port number.
 *      vlan_data  - (IN)Initial VLAN data information
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td3_port_cfg_init(int unit, bcm_port_t port, bcm_vlan_data_t *vlan_data)
{
    soc_mem_t egr_port_mem = EGR_LPORT_PROFILEm;
    soc_field_t field;
    soc_field_t egr_port_field_ids[SOC_MAX_MEM_FIELD_WORDS];
    uint32 egt_port_field_values[SOC_MAX_MEM_FIELD_WORDS];
    int egr_port_field_count = 0;
    soc_field_t port_field_ids[SOC_MAX_MEM_FIELD_WORDS];
    uint32 port_field_values[SOC_MAX_MEM_FIELD_WORDS];
    int port_field_count = 0;

    uint32 port_type;
    uint32 egr_port_type = 0;
    int dual_modid = 0;
    int key_type_value;
    int cpu_hg_index = -1;
    uint32 max_gid, gid;
    uint32 cancun_ver;
    
    SOC_IF_ERROR_RETURN(soc_cancun_version_get(unit, &cancun_ver));

    if ((SOC_IS_TR_VL(unit)) && (NUM_MODID(unit) >= 2)) {
        dual_modid = 1;
    }

    if (IS_CPU_PORT(unit,port)) {
        cpu_hg_index = SOC_INFO(unit).cpu_hg_index;
        port_type = 0; /* Same as Ethernet port */
        egr_port_type = 1; /* CPU port needs to be HG port in EGR_PORT table */
    } else if (IS_ST_PORT(unit, port)) {
        port_type = 1;
        egr_port_type = 1;
    } else if (IS_LB_PORT(unit, port)) {
        port_type = 2;
        egr_port_type = 2;
    } else {
        port_type = 0;
    }

    /* EGR_LPORT_TABLE config init */
#ifdef BCM_SHADOW_SUPPORT
    /* EGR_PORT fields are Unsupported in shadow */
    if (!SOC_IS_SHADOW(unit))
#endif
    {
        /* CPU port type is HG for CANCUN version >= B870.5.2.3 on TD3 */
        if ((SOC_IS_TRIDENT3(unit)) && (cancun_ver <= SOC_CANCUN_VERSION_DEF_5_2_3)) {
            soc_mem_set_field_value_array(egr_port_field_ids, PORT_TYPEf,
                                      egt_port_field_values, port_type,
                                      egr_port_field_count);
        } else {
            soc_mem_set_field_value_array(egr_port_field_ids, PORT_TYPEf,
                                      egt_port_field_values, egr_port_type,
                                      egr_port_field_count);
        }
        soc_mem_set_field_value_array(egr_port_field_ids, EN_EFILTERf,
                                      egt_port_field_values, 1,
                                      egr_port_field_count);
        if (SOC_IS_TR_VL(unit)) {
            soc_mem_set_field_value_array(egr_port_field_ids, DUAL_MODID_ENABLEf,
                                          egt_port_field_values, dual_modid,
                                          egr_port_field_count);
        }

        field = VT_PORT_GROUP_IDf;
        BCM_IF_ERROR_RETURN
            (_bcm_td3_egr_port_tab_conv(unit, &field, &egr_port_mem));

        max_gid = soc_mem_field_length(unit, egr_port_mem, field);
        if(max_gid > 31) {
            return BCM_E_INTERNAL;
        }
        max_gid = (1 << max_gid) - 1;
        gid = (port < max_gid) ? port : max_gid;
        soc_mem_set_field_value_array(egr_port_field_ids, field,
                                      egt_port_field_values, gid,
                                      egr_port_field_count);

        if (soc_feature(unit, soc_feature_directed_mirror_only)) {
            soc_mem_set_field_value_array(egr_port_field_ids, EM_SRCMOD_CHANGEf,
                                          egt_port_field_values, 1,
                                          egr_port_field_count);
        }

        _bcm_esw_egr_port_tab_multi_set(unit, port, egr_port_field_count,
                                        egr_port_field_ids,
                                        egt_port_field_values);

#if defined(CANCUN_SUPPORT)
        if (soc_feature(unit, soc_feature_cancun)) {
            /* initialize the Cancun tag profile entry setup
             * for VT_MISS_UNTAG action. Should be done in Cancun
             */
            if (soc_cancun_cmh_check(unit, EGR_VLAN_CONTROL_1m,
                VT_MISS_UNTAGf) == SOC_E_NONE) {
                int rv = SOC_E_NONE;
                rv = soc_cancun_cmh_mem_set(unit,
                        EGR_VLAN_CONTROL_1m, port,
                                VT_MISS_UNTAGf, 1);
                if (BCM_FAILURE(rv) && rv != SOC_E_PARAM) {
                    return rv;
                }
            }
        }
#endif

        /* Copy EGR port information to CPU Higig port if applied */
        if (cpu_hg_index != -1 &&
            soc_mem_index_max(unit, EGR_PORTm) == cpu_hg_index) {
            BCM_IF_ERROR_RETURN(_bcm_esw_egr_port_tab_multi_set(unit, cpu_hg_index,
                                    egr_port_field_count, egr_port_field_ids,
                                    egt_port_field_values));
        }

#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
            BCM_IF_ERROR_RETURN(_bcm_hr4_port_cfg_init(unit, port));
        }
#endif

    }

    /* PORT_TABLE config init */
    soc_mem_set_field_value_array(port_field_ids, PORT_VIDf,
                                  port_field_values, vlan_data->vlan_tag,
                                  port_field_count);
#if 0
    soc_mem_set_field_value_array(port_field_ids, OUTER_TPIDf,
                                  port_field_values, 0x8100,
                                  port_field_count);
#endif
    soc_mem_set_field_value_array(port_field_ids, MAC_BASED_VID_ENABLEf,
                                  port_field_values, 1,
                                  port_field_count);
    soc_mem_set_field_value_array(port_field_ids, SUBNET_BASED_VID_ENABLEf,
                                  port_field_values, 1,
                                  port_field_count);
#if 0
    soc_mem_set_field_value_array(port_field_ids, TRUST_INCOMING_VIDf,
                                  port_field_values, 1,
                                  port_field_count);
#endif
    soc_mem_set_field_value_array(port_field_ids, PRI_MAPPINGf,
                                  port_field_values, 0xfac688,
                                  port_field_count);
    soc_mem_set_field_value_array(port_field_ids, CFI_0_MAPPINGf,
                                  port_field_values, 0,
                                  port_field_count);
    soc_mem_set_field_value_array(port_field_ids, CFI_1_MAPPINGf,
                                  port_field_values, 1,
                                  port_field_count);
    soc_mem_set_field_value_array(port_field_ids, IPRI_MAPPINGf,
                                  port_field_values, 0xfac688,
                                  port_field_count);
    soc_mem_set_field_value_array(port_field_ids, ICFI_0_MAPPINGf,
                                  port_field_values, 0,
                                  port_field_count);
    soc_mem_set_field_value_array(port_field_ids, ICFI_1_MAPPINGf,
                                  port_field_values, 1,
                                  port_field_count);
    soc_mem_set_field_value_array(port_field_ids, CML_FLAGS_NEWf,
                                  port_field_values, 0x8,
                                  port_field_count);
    soc_mem_set_field_value_array(port_field_ids, CML_FLAGS_MOVEf,
                                  port_field_values, 0x8,
                                  port_field_count);
    BCM_IF_ERROR_RETURN
        (_bcm_esw_pt_vtkey_type_value_get(unit, VLXLT_HASH_KEY_TYPE_OVID,
                                          &key_type_value));
    soc_mem_set_field_value_array(port_field_ids, VT_KEY_TYPEf,
                                  port_field_values, key_type_value,
                                  port_field_count);
    soc_mem_set_field_value_array(port_field_ids, VT_PORT_TYPE_SELECT_1f,
                                  port_field_values, 1,
                                  port_field_count);
    BCM_IF_ERROR_RETURN
        (_bcm_esw_pt_vtkey_type_value_get(unit, VLXLT_HASH_KEY_TYPE_IVID,
                                          &key_type_value));
    soc_mem_set_field_value_array(port_field_ids, VT_KEY_TYPE_2f,
                                  port_field_values, key_type_value,
                                  port_field_count);
    soc_mem_set_field_value_array(port_field_ids, VT_PORT_TYPE_SELECT_2f,
                                  port_field_values, 1,
                                  port_field_count);
    soc_mem_set_field_value_array(port_field_ids, PORT_TYPEf,
                                  port_field_values, port_type,
                                  port_field_count);
    soc_mem_set_field_value_array(port_field_ids, SRC_SYS_PORT_IDf,
                                  port_field_values, port,
                                  port_field_count);
    /* TD3TBD SYS_PORT_ID and PP_PORT_NUM should be covered by CIH, 
     * will remove it after CIH is ready. */
    soc_mem_set_field_value_array(port_field_ids, SYS_PORT_IDf,
                                  port_field_values, port,
                                  port_field_count);
    soc_mem_set_field_value_array(port_field_ids, PP_PORT_NUMf,
                                  port_field_values, port,
                                  port_field_count);
    soc_mem_set_field_value_array(port_field_ids, DUAL_MODID_ENABLEf,
                                  port_field_values, dual_modid,
                                  port_field_count);
    soc_mem_set_field_value_array(port_field_ids, TAG_ACTION_PROFILE_PTRf,
                                  port_field_values, 1,
                                  port_field_count);

    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_multi_set(unit, port, _BCM_CPU_TABS_BOTH,
                            port_field_count, port_field_ids,
                            port_field_values));

    if (cpu_hg_index != -1) {
        soc_cancun_cmh_mem_set(unit, PORT_TABm, cpu_hg_index, PORT_TYPEf, 1);
        /* TD3TBD should be covered by CMH, will remove it after CMH
         * is ready. */
        BCM_IF_ERROR_RETURN(soc_mem_field32_modify(unit, ING_DEVICE_PORTm,
                            cpu_hg_index, PORT_TYPEf, 1));
    }
#ifdef BCM_RCPU_SUPPORT
    if (SOC_IS_RCPU_ONLY(unit)) {
        soc_cancun_cmh_mem_set(unit, PORT_TABm, port, OUTER_TPID_ENABLEf, 0x1);
    }
#endif /* BCM_RCPU_SUPPORT */

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_td3_port_cfg_init
 * Purpose:
 *      Initialize the port configuration according to Initial System
 *      Configuration (see init.c)
 * Parameters:
 *      unit       - (IN)StrataSwitch unit number.
 *      port       - (IN)Port number.
 *      vlan_data  - (IN)Initial VLAN data information
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td3_port_cfg_init(int unit, bcm_port_t port, bcm_vlan_data_t *vlan_data)
{
    if (SOC_IS_TRIDENT3X(unit)) {
        return _bcm_td3_port_cfg_init(unit, port, vlan_data);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_td3_port_table_read
 * Purpose:
 *      Read port table entry and parse it to a common
 *      (bcm_port_cfg_t) structure.
 * Parameters:
 *      unit       - (IN)SOC unit number.
 *      port       - (IN)Port number.
 *      port_cfg   - (OUT)API port information structure.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The following port configuration settings
 *      do not apply to FB/ER ports:
 *
 *      port_cfg->pc_frame_type
 *      port_cfg->pc_ether_type
 *      port_cfg->pc_stp_state
 *      port_cfg->pc_cpu
 *      port_cfg->pc_ptype
 *      port_cfg->pc_pbm
 *      port_cfg->pc_ut_pbm
 *      port_cfg->pc_trunk
 *      port_cfg->pc_pfm
 *    The following port configuration do not apply to ER
 *       port_cfg->pc_nni_port
 */
STATIC int
_bcm_td3_port_table_read(int unit, bcm_port_t port,
                         bcm_port_cfg_t *port_cfg)
{
#define LPORT_FIELDS    22
    int discard_tag = 0;            /* Discard tagged packets bit.   */
    int discard_untag = 0;          /* Discard untagged packets bit. */
    uint32 values[LPORT_FIELDS];
    int i, value = 0;

    soc_field_t fields[LPORT_FIELDS] = {
        PORT_DIS_TAGf,
        PORT_DIS_UNTAGf,
        DROP_BPDUf,
        MIRRORf,
        PORT_VIDf,
        IVIDf,
        TAG_ACTION_PROFILE_PTRf,
        V4L3_ENABLEf,
        V6L3_ENABLEf,
        OPRIf,
        OCFIf,
        IPRIf,
        ICFIf,
        TRUST_DSCP_V4f,
        TRUST_DSCP_V6f,
        EN_IFILTERf,
        PORT_BRIDGEf,
        URPF_MODEf,
        URPF_DEFAULTROUTECHECKf,
        PVLAN_ENABLEf,
        CML_FLAGS_NEWf,
        CML_FLAGS_MOVEf
    };

#define _GET_FIELD_VALUE(_f, _v)                \
    do {                                        \
        for (i = 0; i < LPORT_FIELDS; i++) {    \
            if (fields[i] == (_f)) {            \
                (_v) = values[i];               \
                break;                          \
            }                                   \
        }                                       \
    } while (0)

    BCM_IF_ERROR_RETURN(
        bcm_esw_port_lport_fields_get(
            unit, port, LPORT_PROFILE_LPORT_TAB, LPORT_FIELDS, 
            fields, values));

    /* Get drop all tagged packets flag. */
    _GET_FIELD_VALUE(PORT_DIS_TAGf, discard_tag);

    /* Get drop all untagged packets flag. */
    _GET_FIELD_VALUE(PORT_DIS_UNTAGf, discard_untag);

    if (discard_tag && discard_untag) {
        port_cfg->pc_disc = BCM_PORT_DISCARD_ALL;
    } else if (discard_tag) {
        port_cfg->pc_disc = BCM_PORT_DISCARD_TAG;
    } else if (discard_untag) {
        port_cfg->pc_disc = BCM_PORT_DISCARD_UNTAG;
    } else {
        port_cfg->pc_disc = BCM_PORT_DISCARD_NONE;
    }

    /* Get drop bpdu's on ingress flag. */
    _GET_FIELD_VALUE(DROP_BPDUf, port_cfg->pc_bpdu_disable);

    /* Get enable mirroring flag. */
    _GET_FIELD_VALUE(MIRRORf, value);
    if (soc_feature(unit, soc_feature_mirror_flexible)) {
        /* Multi-bit field */
        port_cfg->pc_mirror_ing = value;
    } else if (!soc_feature(unit, soc_feature_no_mirror) && value) {
        port_cfg->pc_mirror_ing |= BCM_MIRROR_MTP_ONE;
    }

    /* Get port default vlan id (pvid). */
    _GET_FIELD_VALUE(PORT_VIDf, port_cfg->pc_vlan);

    _GET_FIELD_VALUE(IVIDf, port_cfg->pc_ivlan);
    _GET_FIELD_VALUE(TAG_ACTION_PROFILE_PTRf, port_cfg->pc_vlan_action);

    /* Get L3 IPv4 forwarding enable bit. */
    _GET_FIELD_VALUE(V4L3_ENABLEf, value);
    if (value) {
        port_cfg->pc_l3_flags |= BCM_PORT_L3_V4_ENABLE;
    }

    /* Get L3 IPv6 forwarding enable bit. */
    _GET_FIELD_VALUE(V6L3_ENABLEf, value);
    if (value) {
        port_cfg->pc_l3_flags |= BCM_PORT_L3_V6_ENABLE;
    }

    /* Get port default priority.*/
    _GET_FIELD_VALUE(OPRIf, port_cfg->pc_new_opri);
    _GET_FIELD_VALUE(OCFIf, port_cfg->pc_new_ocfi);
    _GET_FIELD_VALUE(IPRIf, port_cfg->pc_new_ipri);
    _GET_FIELD_VALUE(ICFIf, port_cfg->pc_new_icfi);

    /* Get ingress port is trusted port, trust incoming IPv4 DSCP bit. */
    _GET_FIELD_VALUE(TRUST_DSCP_V4f, port_cfg->pc_dse_mode);

    /* Get ingress port is trusted port, trust incoming IPv6 DSCP bit. */
    _GET_FIELD_VALUE(TRUST_DSCP_V6f, port_cfg->pc_dse_mode_ipv6);
    port_cfg->pc_dscp_prio = port_cfg->pc_dse_mode;
    port_cfg->pc_dscp = -1;

    /* Get enable ingress filtering bit. */
    _GET_FIELD_VALUE(EN_IFILTERf, port_cfg->pc_en_ifilter);

    /* Get enable L2 bridging on the incoming port. */
    _GET_FIELD_VALUE(PORT_BRIDGEf, port_cfg->pc_bridge_port);

    /* Unicast rpf mode. */
    _GET_FIELD_VALUE(URPF_MODEf, port_cfg->pc_urpf_mode);

    /* Unicast rpf default gateway check. */
    _GET_FIELD_VALUE(URPF_DEFAULTROUTECHECKf, port_cfg->pc_urpf_def_gw_check);

    /* private VALN enable */
    _GET_FIELD_VALUE(PVLAN_ENABLEf, port_cfg->pc_pvlan_enable);

    /* If port cpu managed learning is not frozen read it from port. */
    if (!soc_feature(unit, soc_feature_no_learning) && 
        soc_l2x_frozen_cml_get(unit, port, &port_cfg->pc_cml, 
                               &port_cfg->pc_cml_move) < 0) {
        _GET_FIELD_VALUE(CML_FLAGS_NEWf, port_cfg->pc_cml);
        _GET_FIELD_VALUE(CML_FLAGS_MOVEf, port_cfg->pc_cml_move);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_td3_port_table_write
 * Purpose:
 *      Prepare & write port table entry 
 * Parameters:
 *      unit       - (IN)SOC unit number.
 *      port       - (IN)Port number.
 *      port_cfg   - (IN)API port information structure.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td3_port_table_write(int unit, bcm_port_t port,
                          bcm_port_cfg_t *port_cfg)
{
    soc_field_t port_field_ids[SOC_MAX_MEM_FIELD_WORDS];
    uint32 port_field_values[SOC_MAX_MEM_FIELD_WORDS];
    int port_field_count = 0;
    int repl_cml, repl_cml_move;
    int cml, cml_move;

    /* Set drop all tagged/untagged packets flag. */
    switch (port_cfg->pc_disc) {
        case BCM_PORT_DISCARD_NONE:
            soc_mem_set_field_value_array(port_field_ids, PORT_DIS_TAGf,
                                          port_field_values, 0,
                                          port_field_count);
            soc_mem_set_field_value_array(port_field_ids, PORT_DIS_UNTAGf,
                                          port_field_values, 0,
                                          port_field_count);
            break;
        case BCM_PORT_DISCARD_ALL:
            soc_mem_set_field_value_array(port_field_ids, PORT_DIS_TAGf,
                                          port_field_values, 1,
                                          port_field_count);
            soc_mem_set_field_value_array(port_field_ids, PORT_DIS_UNTAGf,
                                          port_field_values, 1,
                                          port_field_count);
            break;
        case BCM_PORT_DISCARD_UNTAG:
            soc_mem_set_field_value_array(port_field_ids, PORT_DIS_TAGf,
                                          port_field_values, 0,
                                          port_field_count);
            soc_mem_set_field_value_array(port_field_ids, PORT_DIS_UNTAGf,
                                          port_field_values, 1,
                                          port_field_count);
            break;
        case BCM_PORT_DISCARD_TAG:
            soc_mem_set_field_value_array(port_field_ids, PORT_DIS_TAGf,
                                          port_field_values, 1,
                                          port_field_count);
            soc_mem_set_field_value_array(port_field_ids, PORT_DIS_UNTAGf,
                                          port_field_values, 0,
                                          port_field_count);
            break;
    }

    /* Set drop bpdu's on ingress flag. */
    soc_mem_set_field_value_array(port_field_ids, DROP_BPDUf,
                                  port_field_values, port_cfg->pc_bpdu_disable,
                                  port_field_count);

    /* Set ingress mirroring flag. */
    if (soc_feature(unit, soc_feature_mirror_flexible)) {
        soc_mem_set_field_value_array(port_field_ids, MIRRORf,
            port_field_values, (port_cfg->pc_mirror_ing & BCM_TR2_MIRROR_MTP_MASK),
            port_field_count);
    } else if (!soc_feature(unit, soc_feature_no_mirror)) {
        soc_mem_set_field_value_array(port_field_ids, MIRRORf,
            port_field_values, (port_cfg->pc_mirror_ing & BCM_MIRROR_MTP_ONE),
            port_field_count);
    }

    /*
     * Set cpu managed learning (cml).
     * if unit is not frozen set the same value to port table.
     * else set port table (cml) to PVP_CML_FORWARD.
     */
    if (soc_l2x_frozen_cml_set(unit, port, port_cfg->pc_cml, 
                               port_cfg->pc_cml_move, &repl_cml, 
                               &repl_cml_move) < 0) {
        cml = port_cfg->pc_cml;
        cml_move = port_cfg->pc_cml_move;
    } else {
        cml = repl_cml;
        cml_move = repl_cml_move;
    }

    if (SOC_IS_TRX(unit)) {
        soc_mem_set_field_value_array(port_field_ids, CML_FLAGS_MOVEf,
            port_field_values, cml_move, port_field_count);
        soc_mem_set_field_value_array(port_field_ids, CML_FLAGS_NEWf,
            port_field_values, cml, port_field_count);

        soc_mem_set_field_value_array(port_field_ids, IVIDf,
            port_field_values, port_cfg->pc_ivlan, port_field_count);
        soc_mem_set_field_value_array(port_field_ids, TAG_ACTION_PROFILE_PTRf,
            port_field_values, port_cfg->pc_vlan_action, port_field_count);
    }

    /* Set port default vlan id (pvid). */
    soc_mem_set_field_value_array(port_field_ids, PORT_VIDf,
        port_field_values, port_cfg->pc_vlan, port_field_count);

    /* Set L3 IPv4 forwarding enable bit. */ 
    soc_mem_set_field_value_array(port_field_ids, V4L3_ENABLEf,
        port_field_values, 
        (port_cfg->pc_l3_flags & BCM_PORT_L3_V4_ENABLE) ? 1 : 0,
        port_field_count);

    /* Set L3 IPv6 forwarding enable bit. */ 
    soc_mem_set_field_value_array(port_field_ids, V6L3_ENABLEf,
        port_field_values, 
        (port_cfg->pc_l3_flags & BCM_PORT_L3_V6_ENABLE) ? 1 : 0,
        port_field_count);

    /* Set new port default priority.*/
    soc_mem_set_field_value_array(port_field_ids, PORT_PRIf,
        port_field_values, port_cfg->pc_new_opri, port_field_count);
    soc_mem_set_field_value_array(port_field_ids, OCFIf,
        port_field_values, port_cfg->pc_new_ocfi, port_field_count);
    soc_mem_set_field_value_array(port_field_ids, IPRIf,
        port_field_values, port_cfg->pc_new_ipri, port_field_count);
    soc_mem_set_field_value_array(port_field_ids, ICFIf,
        port_field_values, port_cfg->pc_new_icfi, port_field_count);

    /* Set ingress port is trusted port, trust incoming IPv4 DSCP bit. */ 
    soc_mem_set_field_value_array(port_field_ids, TRUST_DSCP_V4f,
        port_field_values, port_cfg->pc_dse_mode ? 1 : 0, port_field_count);

    /* Set ingress port is trusted port, trust incoming IPv6 DSCP bit. */
    soc_mem_set_field_value_array(port_field_ids, TRUST_DSCP_V6f,
        port_field_values, port_cfg->pc_dse_mode_ipv6 ? 1 : 0, port_field_count);

    /* Set enable ingress filtering. */
    soc_mem_set_field_value_array(port_field_ids, EN_IFILTERf,
        port_field_values, port_cfg->pc_en_ifilter, port_field_count);

    /* Set enable ingress filtering. */
    soc_mem_set_field_value_array(port_field_ids, PORT_BRIDGEf,
        port_field_values, port_cfg->pc_bridge_port, port_field_count);

    /* Unicast rpf mode. */
    soc_mem_set_field_value_array(port_field_ids, URPF_MODEf,
        port_field_values, port_cfg->pc_urpf_mode, port_field_count);

    /* Unicast rpf default gateway check. */
    soc_mem_set_field_value_array(port_field_ids, URPF_DEFAULTROUTECHECKf,
        port_field_values, port_cfg->pc_urpf_def_gw_check, port_field_count);

    /* private VALN enable */
    soc_mem_set_field_value_array(port_field_ids, PVLAN_ENABLEf,
        port_field_values, port_cfg->pc_pvlan_enable, port_field_count);

    _bcm_esw_port_tab_multi_set(unit, port, _BCM_CPU_TABS_BOTH,
                                port_field_count, port_field_ids,
                                port_field_values);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td3_port_cfg_get
 * Purpose:
 *      Get port configuration.
 * Parameters:
 *      unit       - (IN)SOC unit number.
 *      port       - (IN)Port number.
 *      port_cfg   - (OUT)API port information structure.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td3_port_cfg_get(int unit, bcm_port_t port, bcm_port_cfg_t *port_cfg)
{
    /* Input parameters check.*/
    if (NULL == port_cfg) {
        return (BCM_E_PARAM);
    }

    sal_memset(port_cfg, 0, sizeof(bcm_port_cfg_t));

    /* Read port table entry. */
    BCM_IF_ERROR_RETURN(_bcm_td3_port_table_read(unit, port, port_cfg));

    return(BCM_E_NONE);
}

/*
 * Function:
 *      bcm_td3_port_cfg_set
 * Purpose:
 *      Set port configuration.
 * Parameters:
 *      unit       - (IN)SOC unit number.
 *      port       - (IN)Port number.
 *      port_cfg   - (OUT)API port information structure.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td3_port_cfg_set(int unit, bcm_port_t port, bcm_port_cfg_t *port_cfg)
{
    /* Input parameters check.*/
    if (NULL == port_cfg) {
        return (BCM_E_PARAM);
    }

    /* Write port table entry. */
    BCM_IF_ERROR_RETURN(_bcm_td3_port_table_write(unit, port, port_cfg));

    return (BCM_E_NONE);
}

int
_bcm_td3_egr_port_tab_conv(int unit, soc_field_t *field, soc_mem_t* mem) {

    soc_mem_t m = EGR_LPORT_PROFILEm;
    soc_field_t f = *field;

    switch(*field) {
        case QOS_CTRL_IDf:
        case PORT_TYPEf:
        case MY_MODIDf:
        case EGR_PORT_CTRL_IDf:
        case EGR_LPORT_PROFILE_IDXf:
            m = EGR_PORTm;
            break;
        case VT_PORT_GROUP_IDf:
            m = EGR_GPP_ATTRIBUTESm;
            break;
        default:
            break;
    }

    if (SOC_MEM_FIELD_VALID(unit, m, f)) {
        *mem = m;
        *field = f;
        return BCM_E_NONE;
    }

    return BCM_E_NOT_FOUND;
}

int
_bcm_td3_port_tab_conv(int unit, soc_field_t *field, soc_mem_t *mem) {

    soc_mem_t m = LPORT_TABm;
    soc_field_t f = *field;

    switch (*field) {
        case ECCPf:
        case PARITYf:
        case ECCf:
        case ECC_DATAf:
        case SRC_SYS_PORT_IDf:
        case FROM_REMOTE_CPU_ENf:
        case REMOTE_CPU_ENf:
        case DUAL_MODID_ENABLEf:
        case HIGIG_TRUNK_IDf:
        case HIGIG_TRUNKf:
        case MY_MODIDf:
        case POINTERf:
        case HG_TYPEf:
        case HDR_TYPE_0f:
        /*case HDR_TYPE_NAMESPACE_0f:*/
        case INITIAL_SHIFT_AMOUNT_0f:
        case PARSE_CONTEXT_ID_0f:
        case PARSE_CTRL_ID_0f:
        case PKT_FLOW_SELECT_CTRL_ID_0f:
        case SUBPORT_ID_NAMESPACEf:
        case PP_PORT_NUMf:
        case SYS_PORT_IDf:
        case LPORT_PROFILE_IDXf:
        case PORT_TYPEf:
        case PORT_IPBM_INDEXf:
        case INSERT_RX_TIMESTAMPf:
        case DISABLE_TIMESTAMPINGf:
            m = ING_DEVICE_PORTm;
            break;
        case VT_PORT_TYPE_SELECTf:
            f = VT_PORT_TYPE_SELECT_1f;
            break;
        case SUBPORT_TAG_NAMESPACEf:
            m = ING_DEVICE_PORTm;
            f = SUBPORT_ID_NAMESPACEf;
            break;
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
        case SUBPORT_NUM_MASKf:
           m = ING_DEVICE_PORTm;
           f = SUBPORT_NUM_MASKf;
           break;
#endif
        case OUTER_TPID_ENABLEf:
            m = SOURCE_TRUNK_MAP_TABLEm;
            break;
        default:
            break;
    }

    if (SOC_MEM_FIELD_VALID(unit, m, f)) {
        *mem = m;
        *field = f;
        return BCM_E_NONE;
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *      bcm_td3_port_ing_pri_cng_set
 * Description:
 *      Set packet priority and cfi to internal priority and color mapping
 * Parameters:
 *      unit         - (IN) Device number
 *      port         - (IN) Port number
 *      untagged     - (IN) For untagged packet (ignore pkt_pri, cfi argumnet)
 *      pkt_pri      - (IN) Packet priority (802.1p cos)
 *      cfi          - (IN) Packet CFI
 *      int_pri      - (IN) Internal priority
 *      color        - (IN) Color
 * Return Value:
 *      BCM_E_XXX
 * Note:
 *      When both pkt_pri and cfi are -1, the setting is for untagged packet
 */
#define MAP_SIZE    64
int
bcm_td3_port_ing_pri_cng_set(int unit, bcm_port_t port, int untagged,
                             int pkt_pri, int cfi,
                             int int_pri, bcm_color_t color)
{
    ing_pri_cng_map_entry_t  map[MAP_SIZE];
    void                     *entries[1];
    uint32                   prof_index, old_prof_index;
    int                      index;
    int                      pkt_pri_cur, pkt_pri_min, pkt_pri_max;
    int                      cfi_cur, cfi_min, cfi_max;
    int                      rv = BCM_E_NONE;
    soc_mem_t                mem = ING_DEVICE_PORTm;

    if (pkt_pri < 0) {
        pkt_pri_min = 0;
        pkt_pri_max = 7;
    } else {
        pkt_pri_min = pkt_pri;
        pkt_pri_max = pkt_pri;
    }

    if (cfi < 0) {
        cfi_min = 0;
        cfi_max = 1;
    } else {
        cfi_min = cfi;
        cfi_max = cfi;
    }

    /* Lock the port table */
    soc_mem_lock(unit, mem);

    rv = _bcm_esw_port_tab_get(unit, port, TRUST_DOT1P_PTRf,
                               (void *)&old_prof_index);
    if (BCM_FAILURE(rv)) {
        goto __cleanup;
    }

    old_prof_index = old_prof_index * MAP_SIZE;

    entries[0] = map;

    rv = _bcm_ing_pri_cng_map_entry_get(unit, old_prof_index, MAP_SIZE, entries);
    if (BCM_FAILURE(rv)) {
        goto __cleanup;
    }

    for (pkt_pri_cur = pkt_pri_min; pkt_pri_cur <= pkt_pri_max;
         pkt_pri_cur++) {
        for (cfi_cur = cfi_min; cfi_cur <= cfi_max; cfi_cur++) {
            index = ((untagged != 0) << 4) |
                    ((pkt_pri_cur << 1) & 0xe) |
                    (cfi_cur & 0x1);
            if (int_pri >= 0) {
                soc_mem_field32_set(unit, PHB_MAPPING_TBL_1m, &map[index],
                                    INT_PRIf, int_pri);
            }
            if (color >= 0) {
                soc_mem_field32_set(unit, PHB_MAPPING_TBL_1m, &map[index],
                                    CNGf,
                                    _BCM_COLOR_ENCODING(unit, color));
            }
        }
    }

    rv = _bcm_ing_pri_cng_map_entry_add(unit, entries, MAP_SIZE, &prof_index);
    if (BCM_FAILURE(rv)) {
        goto __cleanup;
    }

    if (prof_index != old_prof_index) {
        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                   TRUST_DOT1P_PTRf, prof_index / MAP_SIZE);
        if (BCM_FAILURE(rv)) {
            goto __cleanup;
        }
    }

    if (old_prof_index != 0) {
        rv = _bcm_ing_pri_cng_map_entry_delete(unit, old_prof_index);
        if (BCM_FAILURE(rv)) {
            goto __cleanup;
        }
    }
__cleanup:
    /* Release port table lock */
    soc_mem_unlock(unit, mem);
    return rv;
}

/*
 * Function:
 *      bcm_td3_port_ing_pri_cng_get
 * Description:
 *      Get packet priority and cfi to internal priority and color mapping
 * Parameters:
 *      unit         - (IN) Device number
 *      port         - (IN) Port number
 *      untagged     - (IN) For untagged packet (ignore pkt_pri, cfi argumnet)
 *      pkt_pri      - (IN) Packet priority (802.1p cos)
 *      cfi          - (IN) Packet CFI
 *      int_pri      - (OUT) Internal priority
 *      color        - (OUT) Color
 * Return Value:
 *      BCM_E_XXX
 * Note:
 *      When both pkt_pri and cfi are -1, the setting is for untagged packet
 */
int
bcm_td3_port_ing_pri_cng_get(int unit, bcm_port_t port, int untagged,
                             int pkt_pri, int cfi,
                             int *int_pri, bcm_color_t *color)
{
    ing_pri_cng_map_entry_t  map[MAP_SIZE];
    void                     *entries[1];
    uint32                   profile_index;
    int                      index;
    int                      hw_color;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_get(unit, port, TRUST_DOT1P_PTRf,
                               (void *)&profile_index));
    profile_index = profile_index * MAP_SIZE;

    entries[0] = map;

    BCM_IF_ERROR_RETURN
        (_bcm_ing_pri_cng_map_entry_get(unit, profile_index, MAP_SIZE, entries));

    index = ((untagged != 0) << 4) | ((pkt_pri << 1) & 0xe) | (cfi & 0x1);

    if (int_pri != NULL) {
        *int_pri = soc_mem_field32_get(unit, PHB_MAPPING_TBL_1m, &map[index],
                                       INT_PRIf);
    }
    if (color != NULL) {
        hw_color = soc_mem_field32_get(unit, PHB_MAPPING_TBL_1m, &map[index],
                                       CNGf);
        *color = _BCM_COLOR_DECODING(unit, hw_color);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td3_port_rate_egress_set
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
bcm_td3_port_rate_egress_set(int unit, bcm_port_t port, uint32 bandwidth,
                             uint32 burst, uint32 mode)
{
    int phy_port, mmu_port, index;
    soc_mem_t mem = MMU_MTRO_EGRMETERINGCONFIG_MEMm;
    uint32 rval, itu_mode_sel;
    mmu_mtro_egrmeteringconfig_mem_0_entry_t entry;
    uint32 refresh_rate, bucketsize, granularity, flags;
    int refresh_bitsize, bucket_bitsize;
#if defined BCM_HURRICANE4_SUPPORT
    int time_sel = 0;
#endif

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];

    sal_memset(&entry, 0, sizeof(entry));

    index = mmu_port % SOC_TD3_MMU_PORT_PER_PIPE;
    mem = SOC_TD3_PMEM(unit, port, MMU_MTRO_EGRMETERINGCONFIG_MEM_PIPE0m,
            MMU_MTRO_EGRMETERINGCONFIG_MEM_PIPE1m);


    /* If metering is disabled on this ingress port we are done. */
    if (!bandwidth || !burst) {
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ANY, index, &entry));
        return BCM_E_NONE;
    }

    if((bandwidth < _BCM_PORT_RATE_MIN_KILOBITS ||
        bandwidth > _BCM_PORT_RATE_MAX_KILOBITS) ||
        (burst < _BCM_PORT_BURST_MIN_KILOBITS ||
        burst > _BCM_PORT_BURST_MAX_KILOBITS)) {
        return BCM_E_PARAM;
    }

    flags = mode == _BCM_PORT_RATE_PPS_MODE ?
                        _BCM_TD_METER_FLAG_PACKET_MODE : 0;

    BCM_IF_ERROR_RETURN(READ_MMU_SEDCFG_MISCCONFIGr(unit, &rval));
    itu_mode_sel = soc_reg_field_get(unit, MMU_SEDCFG_MISCCONFIGr, rval,
                                     ITU_MODE_SELf);

    if (itu_mode_sel) {
        flags |= _BCM_TD_METER_FLAG_NON_LINEAR;
    }


    refresh_bitsize = soc_mem_field_length(unit, mem, REFRESHf);
    bucket_bitsize = soc_mem_field_length(unit, mem, THD_SELf);
#if defined BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        SOC_IF_ERROR_RETURN(READ_MTRO_REFRESH_CONFIGr(unit, &rval));
        time_sel = soc_reg_field_get(unit, MTRO_REFRESH_CONFIGr, rval, TICK_MODE_SELf);
        refresh_bitsize = (time_sel) ? refresh_bitsize - 1: refresh_bitsize;
    }
#endif
    BCM_IF_ERROR_RETURN
        (_bcm_td_rate_to_bucket_encoding(unit, bandwidth, burst, flags,
                                         refresh_bitsize, bucket_bitsize,
                                         &refresh_rate, &bucketsize,
                                         &granularity));
    soc_mem_field32_set(unit, mem, &entry, MODEf,
                        mode == _BCM_PORT_RATE_PPS_MODE ? 1 : 0);
#if defined BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
       refresh_rate = (time_sel) ? refresh_rate * 2 : refresh_rate;
    }
#endif
    soc_mem_field32_set(unit, mem, &entry, METER_GRANf, granularity);
    soc_mem_field32_set(unit, mem, &entry, REFRESHf, refresh_rate);
    soc_mem_field32_set(unit, mem, &entry, THD_SELf, bucketsize);

    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, mem, MEM_BLOCK_ANY, index, &entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td3_port_ipbm_opaque_set
 * Purpose:
 *      Set egress metering information
 * Parameters:
 *      unit       - (IN) Unit number
 *      stm_idx    - (IN) SOURCE_TRUNK_MAP table index
 *      class      - (IN) port class value(range 0-131)
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td3_port_ipbm_opaque_set(int unit, int stm_idx, uint32 class)
{
    int rv;
    int temp_p;
    uint32 pipe_port;
    int max_port = 0;
    uint32 ipbm_val = 0;
    uint32 opq_obj_val = 0;
    uint8 opaque_3 = _BCM_TD3_OPAQUE3_ID;
    uint8 opaque_4 = _BCM_TD3_OPAQUE4_ID;
    soc_mem_t mem = SOURCE_TRUNK_MAP_TABLEm;
    soc_field_t ipbm_fld = IPBM_INDEXf;
    soc_field_t opq_obj_fld = OPAQUE_OBJECTf;
    source_trunk_map_table_entry_t stm_entry;

    if (!(SOC_MEM_FIELD_VALID(unit, mem, ipbm_fld) &&
          SOC_MEM_FIELD_VALID(unit, mem, opq_obj_fld))) {
        return BCM_E_UNAVAIL;
    }

    max_port = _BCM_TD3_NUM_PIPE_PORTS;

    if(class >= max_port) {
        return BCM_E_PARAM;
    }

   /* Read source trunk map table. */
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, stm_idx, &stm_entry);
    BCM_IF_ERROR_RETURN(rv);

    pipe_port = class;

    /* Set IPBM_INDEX */
    ipbm_val = (pipe_port < _FP_TD3_TCAM_IPBMP_SIZE) ? pipe_port : _FP_TD3_TCAM_IPBMP_SIZE+1;
    soc_mem_field32_set(unit, mem, &stm_entry, ipbm_fld, ipbm_val);

    /* Set OPAQUE_OBJECT */
    if (pipe_port >= _FP_TD3_TCAM_IPBMP_SIZE) {
        if (pipe_port >= (_FP_TD3_TCAM_IPBMP_SIZE + _FP_FLEX_PBMP_SIZE)) {
            opq_obj_val = opaque_4;
            temp_p = pipe_port - (_FP_TD3_TCAM_IPBMP_SIZE + _FP_FLEX_PBMP_SIZE);
        } else {
            opq_obj_val = opaque_3;
            temp_p = pipe_port - _FP_TD3_TCAM_IPBMP_SIZE;
        }
        opq_obj_val = ((1 << temp_p) << 3) | opq_obj_val;
        soc_mem_field32_set(unit, mem, &stm_entry, opq_obj_fld, opq_obj_val);
    }

    /* Write source trunk map table */
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, stm_idx, &stm_entry);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td3_port_ipbm_opaque_get
 * Purpose:
 *      Set egress metering information
 * Parameters:
 *      unit       - (IN) Unit number
 *      stm_idx    - (IN) SOURCE_TRUNK_MAP table index
 *      class      - (OUT) pointer of class
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td3_port_ipbm_opaque_get(int unit, int stm_idx, uint32* class)
{
    int rv;
    uint32 temp_p;
    uint32 ipbm_val = 0;
    uint32 opq_obj_val = 0;
    uint8 opaque_3 = _BCM_TD3_OPAQUE3_ID;
    uint8 opaque_4 = _BCM_TD3_OPAQUE4_ID;
    soc_mem_t mem = SOURCE_TRUNK_MAP_TABLEm;
    soc_field_t ipbm_fld = IPBM_INDEXf;
    soc_field_t opq_obj_fld = OPAQUE_OBJECTf;
    source_trunk_map_table_entry_t stm_entry;
    uint32 temp_cnt = 0;

    if (!(SOC_MEM_FIELD_VALID(unit, mem, ipbm_fld) &&
          SOC_MEM_FIELD_VALID(unit, mem, opq_obj_fld))) {
        return BCM_E_UNAVAIL;
    }

   /* Read source trunk map table. */
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, stm_idx, &stm_entry);
    BCM_IF_ERROR_RETURN(rv);

    ipbm_val = soc_mem_field32_get(unit, mem, &stm_entry, ipbm_fld);

    if (ipbm_val < _FP_TD3_TCAM_IPBMP_SIZE) {
        *class = ipbm_val;
        return BCM_E_NONE;
    } else {
        temp_p = soc_mem_field32_get(unit, mem, &stm_entry, opq_obj_fld);
        opq_obj_val = temp_p & _BCM_TD3_OPAQUE_ID_MASK;
        temp_p = temp_p >> 3;
        while ((temp_p = temp_p >> 1) > 0) {
            temp_cnt++;
        }
        if (opq_obj_val == opaque_3) {
            *class = temp_cnt + _FP_TD3_TCAM_IPBMP_SIZE;
        } else if (opq_obj_val == opaque_4) {
            *class = temp_cnt + _FP_TD3_TCAM_IPBMP_SIZE
                                                 + _FP_FLEX_PBMP_SIZE;
        } else {
            return BCM_E_INTERNAL;
        }

        return BCM_E_NONE;
    }
}

/*
 * Function:
 *      bcm_td3_port_rate_egress_get
 * Purpose:
 *      Get egress metering information
 * Parameters:
 *      unit       - (IN) Unit number
 *      port       - (IN) Port number
 *      bandwidth  - (OUT) Kilobits per second or packets per second
 *                        zero if rate limiting is disabled
 *      burst      - (OUT) Maximum burst size in kilobits or packets
 *      mode       - (OUT) _BCM_PORT_RATE_BYTE_MODE or _BCM_PORT_RATE_PPS_MODE
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td3_port_rate_egress_get(int unit, bcm_port_t port, uint32 *bandwidth,
                             uint32 *burst, uint32 *mode)
{
    int phy_port, mmu_port, index;
    soc_mem_t mem = MMU_MTRO_EGRMETERINGCONFIG_MEMm;
    uint32 rval, itu_mode_sel;
    mmu_mtri_bkpmeteringconfig_mem_0_entry_t entry;
    uint32 refresh_rate, bucketsize, granularity, flags;
#if defined BCM_HURRICANE4_SUPPORT
    int time_sel = 0;
#endif

    if (bandwidth == NULL || burst == NULL) {
        return BCM_E_PARAM;
    }

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];

    index = mmu_port % SOC_TD3_MMU_PORT_PER_PIPE;
    mem = SOC_TD3_PMEM(unit, port, MMU_MTRO_EGRMETERINGCONFIG_MEM_PIPE0m,
            MMU_MTRO_EGRMETERINGCONFIG_MEM_PIPE1m);

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, &entry));

    refresh_rate = soc_mem_field32_get(unit, mem, &entry, REFRESHf);
    bucketsize = soc_mem_field32_get(unit, mem, &entry, THD_SELf);
    granularity = soc_mem_field32_get(unit, mem, &entry, METER_GRANf);

#if defined BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        SOC_IF_ERROR_RETURN(READ_MTRO_REFRESH_CONFIGr(unit, &rval));
        time_sel = soc_reg_field_get(unit, MTRO_REFRESH_CONFIGr, rval, TICK_MODE_SELf);
        refresh_rate = (time_sel) ? refresh_rate / 2 : refresh_rate;
    }
#endif
    flags = soc_mem_field32_get(unit, mem, &entry, MODEf) ?
                _BCM_TD_METER_FLAG_PACKET_MODE : 0;

    BCM_IF_ERROR_RETURN(READ_MMU_SEDCFG_MISCCONFIGr(unit, &rval));
    itu_mode_sel = soc_reg_field_get(unit, MMU_SEDCFG_MISCCONFIGr, rval,
                                     ITU_MODE_SELf);

    if (itu_mode_sel) {
        flags |= _BCM_TD_METER_FLAG_NON_LINEAR;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td_bucket_encoding_to_rate(unit, refresh_rate, bucketsize,
                                         granularity, flags, bandwidth, burst));

    *mode = flags & _BCM_TD_METER_FLAG_PACKET_MODE ?
                _BCM_PORT_RATE_PPS_MODE : _BCM_PORT_RATE_BYTE_MODE;

    return BCM_E_NONE;

}

int bcm_td3_port_drain_cells(int unit, int port)
{
    mac_driver_t *macd;
    int rv = BCM_E_NONE;

    if (IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    }

    /* Call Port Control module */
    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        return bcmi_esw_portctrl_egress_queue_drain(unit, port);
    }

    PORT_LOCK(unit);
    rv = soc_mac_probe(unit, port, &macd);

    if (BCM_SUCCESS(rv)) {
        rv = MAC_CONTROL_SET(macd, unit, port, SOC_MAC_CONTROL_EGRESS_DRAIN, 1);

    }
    PORT_UNLOCK(unit);
    return rv;
}

int
bcm_td3_port_init(int unit)
{
    int rv = BCM_E_NONE;
    int port;
    /* VLAN control TBA in FlexPP (SDK-66866) */
    /* Add default entries for SYSTEM_CONFIG_TABLE profile */
#if 0
    /* TD3TBD */
    int port, index;
    uint16 tpid;

    tpid = _bcm_fb2_outer_tpid_default_get(unit);
    BCM_IF_ERROR_RETURN(_bcm_fb2_outer_tpid_lkup(unit, tpid, &index));

    for (port = 0; port <= SOC_PORT_ADDR_MAX(unit); port++) {
        BCM_IF_ERROR_RETURN(
            _bcm_trx_source_trunk_map_set(unit, port,
            OUTER_TPID_ENABLEf, 1 << index));
    }
    BCM_IF_ERROR_RETURN(_bcm_fb2_outer_tpid_tab_ref_count_add(unit, index, 1));
#endif

    /* Move from _soc_trident3_misc_init */
    if (soc_feature(unit, soc_feature_egr_all_profile)) {
        PBMP_ALL_ITER(unit, port) {
            soc_field_t flds[3] = {
                VT_MISS_UNTAGf,
                REMARK_OUTER_DOT1Pf,
                OUTER_TPID_VALIDf
            };
            uint32 vals[3] = {0, 1, 1};
            BCM_IF_ERROR_RETURN(bcm_esw_port_egr_lport_fields_set(unit,
                port, EGR_VLAN_CONTROL_1m, 3, flds, vals));
        }
    }

    return rv;
}

int
bcmi_td3_speed_mix_validate(int unit, uint32 speed_mask)
{
    return soc_td3_speed_mix_validate(unit, speed_mask);
}

#define  FIELD_IDS_NUM   4
/*
 * Function:
 *      bcmi_td3_port_internal_loopback_set
 * Purpose:
 *      Put the port in the internal loopback mode 
 *
 * Parameters:
 *      unit       - (IN) Unit number
 *      port       - (IN) Port number
 *      enable     - (IN) TRUE or FALSE
 * Returns:
 *      BCM_E_XXX
 */
int bcmi_td3_port_internal_loopback_set(int unit, bcm_port_t port, int enable)
{
    soc_field_t fld_ids[FIELD_IDS_NUM];
    uint32 fld_values[FIELD_IDS_NUM];

    fld_ids[0]    = PORT_TYPEf;
    fld_ids[1]    = PARSE_CONTEXT_ID_0f;
    if (enable) { /* enable */
        fld_values[0] = 2;
        fld_values[1] = 4;
    } else {
        fld_values[0] = 0;
        fld_values[1] = 6;
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_multi_set(unit, port, 
        _BCM_CPU_TABS_BOTH, 2, fld_ids, fld_values));
    fld_ids[0]    = PORT_TYPEf;
    fld_ids[1]    = EGR_PORT_CTRL_IDf;
    fld_ids[2]    = EDIT_CTRL_IDf;
    fld_ids[3]    = EM_SRCMOD_CHANGEf;
    if (enable) { /* enable */
        fld_values[0] = 2;
        fld_values[1] = 2;
        fld_values[2] = 3;
        fld_values[3] = 0;
    } else {
        fld_values[0] = 0;
        fld_values[1] = 1;
        fld_values[2] = 0;
        fld_values[3] = 1;
    }
    BCM_IF_ERROR_RETURN(_bcm_esw_egr_port_tab_multi_set(unit, port, 
                     FIELD_IDS_NUM, fld_ids, fld_values));
    if (enable) {
        /* enable IPv4 L3 on loopback port. The CPU masquerade packet
         * will reference the LPORT profile from source_trunk_map with
         * port number set in the loopback header. The default LPORT
         * profile index is 0 for all these masquerade ports.
         */
        SOC_IF_ERROR_RETURN(soc_mem_field32_modify(unit, LPORT_TABm, 0,
                        V4L3_ENABLEf, 1));
        SOC_IF_ERROR_RETURN(soc_mem_field32_modify(unit, LPORT_TABm, 0,
                        V6L3_ENABLEf, 1));

    }
    PORT_LOCK(unit);
    if (enable) {
        SOC_PBMP_PORT_ADD(SOC_INFO(unit).lb_pbm, port);
    } else {
        SOC_PBMP_PORT_REMOVE(SOC_INFO(unit).lb_pbm, port);
    }
    PORT_UNLOCK(unit);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_td3_port_pp_num_set
 * Purpose:
 *      Set the packet processing port number for a given port. 
 *      
 * Parameters:
 *      unit       - (IN) Unit number
 *      port       - (IN) Port number
 *      pp_num     - (IN) packet processing port number 
 * Returns:
 *      BCM_E_XXX
 */
int bcmi_td3_port_pp_num_set(int unit, bcm_port_t port, int pp_num)
{
    uint32 idx_new;
    uint32 idx_old;

    if (!SOC_PORT_ADDRESSABLE(unit, pp_num)) {
        return BCM_E_PARAM;
    }
    if (BCM_GPORT_IS_MODPORT(port)) {
        BCM_IF_ERROR_RETURN(_bcm_trx_source_trunk_map_set(unit, port,
                              PP_PORT_NUMf, pp_num));
                
        /* the pp_port should inherit the local port's LPORT profile */
        if (pp_num) {
            BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_get(unit, pp_num,
                            LPORT_PROFILE_IDXf, (int *)&idx_new));
        } else {
            /* reset to default lport profile if pp_port is 0 */
            idx_new = 0;
        }
        /* save the original lport profile */ 
        BCM_IF_ERROR_RETURN(_bcm_trx_source_trunk_map_get(unit, port,
                    LPORT_PROFILE_IDXf, &idx_old));
        
        BCM_IF_ERROR_RETURN(_bcm_trx_source_trunk_map_set(unit, port,
                              LPORT_PROFILE_IDXf, idx_new));

        /* increase the reference for this profile */ 
        BCM_IF_ERROR_RETURN(_bcm_lport_ind_profile_mem_reference(unit,
                              idx_new, 1));

        /* decrease or delete the old profile */
        BCM_IF_ERROR_RETURN(_bcm_lport_ind_profile_entry_delete(unit,
                              idx_old));

    } else if (SOC_PORT_ADDRESSABLE(unit, port)) {
        /* for local port */
        BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                     _BCM_CPU_TABS_ETHER, PP_PORT_NUMf, pp_num));
    } else {
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_td3_port_pp_num_get
 * Purpose:
 *      Get the packet processing port number for a given port. 
 *      
 * Parameters:
 *      unit       - (IN) Unit number
 *      port       - (IN) Port number
 *      pp_num     - (OUT) packet processing port number 
 * Returns:
 *      BCM_E_XXX
 */
int bcmi_td3_port_pp_num_get(int unit, bcm_port_t port, int *pp_num)
{
    if (BCM_GPORT_IS_MODPORT(port)) {
        BCM_IF_ERROR_RETURN(_bcm_trx_source_trunk_map_get(unit, port,
                              PP_PORT_NUMf, (uint32 *)pp_num));
    } else if (SOC_PORT_ADDRESSABLE(unit, port)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_get(unit, port,
                                PP_PORT_NUMf, pp_num));
    } else {
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

#else /* BCM_TRIDENT3_SUPPORT */
typedef int _td3_port_not_empty; /* Make ISO compilers happy. */

#endif  /* BCM_TRIDENT3_SUPPORT */
