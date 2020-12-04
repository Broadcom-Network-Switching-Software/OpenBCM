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

#if defined(BCM_TOMAHAWK2_SUPPORT)
#include <soc/tomahawk.h>
#include <soc/tomahawk2.h>
#include <soc/tomahawk2_tdm.h>
#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/portctrl.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/tomahawk2.h>
#include <soc/flexport/tomahawk2_flexport.h>

/*
 * Function Vector
 */
static bcm_esw_port_drv_t bcm_th2_port_drv = {
    /* fn_drv_init                */ bcmi_th2_port_fn_drv_init,
    /* resource_set               */ bcmi_xgs5_port_resource_set,
    /* resource_get               */ bcmi_xgs5_port_resource_get,
    /* resource_multi_set         */ bcmi_xgs5_port_resource_multi_set,
    /* resource_speed_set         */ bcmi_xgs5_port_speed_set,
    /* resource_traverse          */ bcmi_xgs5_port_resource_traverse,
    /* port_redirect_set          */ NULL,
    /* port_redirect_get          */ NULL,
    /* port_enable_set            */ bcmi_xgs5_port_enable_set,
    /* encap_speed_check          */ bcmi_xgs5_port_encap_speed_check,
    /* force_lb_set               */ bcmi_xgs5_port_force_lb_set,
    /* force_lb_check             */ bcmi_xgs5_port_force_lb_check,
    /* resource_status_update     */ bcmi_xgs5_port_resource_status_update,
    /* port_resource_speed_multi_set */ bcmi_xgs5_port_resource_speed_multi_set,
    /* device_speed_check            */ NULL
};

/* Device programming routines */
static bcmi_xgs5_port_func_t bcmi_th2_port_calls = {
    /* reconfigure_ports        */ soc_tomahawk2_reconfigure_ports,
    /* soc_resource_init        */ bcmi_th2_port_soc_resource_init,
    /* port_resource_validate   */ bcmi_th2_port_resource_validate,
    /* pre_flexport_tdm         */ bcmi_th2_pre_flexport_tdm,
    /* post_flexport_tdm        */ bcmi_th2_post_flexport_tdm,
    /* port_macro_update        */ bcmi_th2_clport_update,
    /* port_enable              */ bcmi_th2_port_enable,
    /* port_disable             */ bcmi_th2_port_disable,
    /* no_tdm_speed_update      */ bcmi_th2_port_speed_1g_update,
    /* speed_ability_get        */ bcmi_th2_speed_ability_get,
    /* speed_mix_validate       */ bcmi_th2_speed_mix_validate,
};

static int              bcmi_th2_port_drv_internal_initialized = 0;

static bcmi_xgs5_dev_info_t  *bcmi_th2_dev_info[SOC_MAX_NUM_DEVICES];

/* HW Definitions */
static bcmi_xgs5_port_drv_t    bcmi_th2_port_drv;

static 
int (*th2_port_attach_exec[PORT_ATTACH_EXEC__MAX])(int unit, bcm_port_t port) = {
    /* MMU config init */
    &bcmi_th2_port_attach_mmu,

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
    &bcm_th_cosq_port_attach,
    &bcmi_xgs5_port_attach_linkscan,
    &bcmi_xgs5_port_attach_stat,
    &bcmi_xgs5_port_attach_stack,
    &bcmi_xgs5_port_attach_rate,
    &bcmi_xgs5_port_attach_field,
    &bcmi_xgs5_port_attach_mirror,
    &bcmi_xgs5_port_attach_l3,
    &bcmi_xgs5_port_attach_ipmc,
    &bcm_th2_ipmc_repl_port_attach,
    &bcmi_xgs5_port_attach_mpls,
    &bcmi_xgs5_port_attach_mim
};

static 
int (*th2_port_detach_exec[PORT_DETACH_EXEC__MAX])(int unit, bcm_port_t port) = {
    /* Other modules related to port */
    &bcm_th2_ipmc_repl_port_detach,
    &bcmi_xgs5_port_detach_ipmc,
    &bcmi_xgs5_port_detach_mpls,
    &bcmi_xgs5_port_detach_mim,
    &bcmi_xgs5_port_detach_l3,
    &bcmi_xgs5_port_detach_mirror,
    &bcmi_xgs5_port_detach_field,
    &bcmi_xgs5_port_detach_rate,
    &bcmi_xgs5_port_detach_stack,
    &bcm_th_cosq_port_detach,
    &bcmi_xgs5_port_detach_trunk,
    &bcmi_xgs5_port_detach_vlan,
    &bcmi_xgs5_port_detach_stg,

    /* PORT module (_bcm_esw_port_deinit) */
    &bcmi_th2_port_detach_asf,
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
 *      bcmi_th2_init_condition_set
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
_bcmi_th2_init_condition_set(int unit, bcmi_xgs5_dev_info_t *dev_info)
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

    if (!(SAL_BOOT_SIMULATION &&
          soc_property_get(unit, "skip_trunk_init", 0))) {
        SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_TRUNK);
    }

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
        if (!soc_property_get(unit, "skip_mcast_init", 0)) {
            SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_MCAST);
        }
        SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_RATE);

        if (SOC_MEM_IS_VALID(unit, FP_UDF_TCAMm)) {
            SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_UDF);
        }

#ifdef BCM_FIELD_SUPPORT
        if (SOC_MEM_IS_VALID(unit, IFP_RANGE_CHECKm)) {
            SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_RANGE);
        }
        if (soc_feature(unit, soc_feature_field)) {
            if (!soc_property_get(unit, "skip_field_init", 0)) {
                SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_FIELD);
            }
            if (!soc_property_get(unit, "skip_auth_init", 0)) {
                SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_AUTH);
            }
        }
#endif

#ifdef INCLUDE_L3
        if (soc_feature(unit, soc_feature_failover)) {
            SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_FAILOVER);
        }

        if (soc_feature(unit, soc_feature_l3) &&
            soc_property_get(unit, spn_L3_ENABLE, 1) &&
            (!soc_property_get(unit, "skip_l3_init", 0))) {
            SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_L3);
        }
        if (soc_feature(unit, soc_feature_ip_mcast) &&
            (!soc_property_get(unit, "skip_ipmc_init", 0))) {
            SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_IPMC);
        }

#ifdef BCM_MPLS_SUPPORT
        if (soc_feature(unit, soc_feature_mpls) &&
            (!soc_property_get(unit, "skip_mpls_init", 0))) {
            SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_MPLS);
        }
#endif
        if (soc_feature(unit, soc_feature_mim) &&
            (!soc_property_get(unit, "skip_mim_init", 0))) {
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
        if (soc_feature(unit, soc_feature_flex_flow)) {
            SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_FLOW);
        }
        if (soc_feature(unit, soc_feature_port_extension)) {
            SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_EXTENDER);
        }
        SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_MULTICAST);
#endif
        if (soc_feature(unit, soc_feature_qos_profile) &&
            (!soc_property_get(unit, "skip_qos_init", 0))) {
            SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_QOS);
        }
        if (soc_feature(unit, soc_feature_time_support)) {
            SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_TIME);
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
        /* Service meter */
        if (soc_feature(unit, soc_feature_global_meter)) {
            SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_GLB_METER);
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
    if (soc_feature(unit, soc_feature_oam) ||
        soc_feature(unit, soc_feature_fp_based_oam)) {
        SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_OAM);
    }
#ifdef SW_AUTONEG_SUPPORT
    if (soc_feature(unit, soc_feature_sw_autoneg)) {
        SHR_BITSET(dev_info->init_cond, SHR_BPROF_BCM_SW_AN);
    }
#endif

    return;
}

/*
 * Function:
 *      bcmi_th2_port_dev_info_init
 * Purpose:
 *      Initialize the device info.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcmi_th2_port_dev_info_init(int unit)
{
    bcmi_xgs5_dev_info_t *dev_info;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 pipe_lr_bw;
    int alloc_size;
    int pipe, phy_port, blk, bindex, rsv_port, tsc_id;
    int i, cntmaxsize;
    _soc_tomahawk2_tsc_otp_info_t tsc_info;

    /* Free bcmi_th2_dev_info[unit] if already allocated */
    if (bcmi_th2_dev_info[unit]) {
        sal_free(bcmi_th2_dev_info[unit]);
        bcmi_th2_dev_info[unit] = NULL;
    }

    /* Allocate memory for main SW data structure */
    alloc_size = sizeof(bcmi_xgs5_dev_info_t) +
                 (sizeof(bcmi_xgs5_phy_port_t) * _TH2_PHY_PORTS_PER_DEV);
    dev_info = sal_alloc(alloc_size, "bcmi_th2_dev_info");
    if (dev_info == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(dev_info, 0, alloc_size);
    dev_info->phy_port_info = (bcmi_xgs5_phy_port_t *)&dev_info[1];
    bcmi_th2_dev_info[unit] = dev_info;

    /* Device information init */
    dev_info = bcmi_th2_dev_info[unit];
    dev_info->num_pipe = _TH2_PIPES_PER_DEV;
    dev_info->num_tsc = _TH2_PBLKS_PER_DEV;
    dev_info->phy_ports_max = _TH2_PHY_PORTS_PER_DEV;
    dev_info->mmu_lossless = si->mmu_lossless;
    dev_info->asf_prof_default = _SOC_ASF_MEM_PROFILE_SIMILAR;
    dev_info->pipe_bound = 1;
    for (pipe = 0; pipe < _TH2_PIPES_PER_DEV; pipe++) {
        /* First logical port in pipe   */
        dev_info->pipe_log_port_base[pipe] = _TH2_DEV_PORTS_PER_PIPE * pipe;

        /* First physical port in pipe  */
        dev_info->pipe_phy_port_base[pipe] = _TH2_GPHY_PORTS_PER_PIPE * pipe +
                                             (pipe ? 1 : 0);

        /* Pipe 3 has 1 reserved port (physical port 258) */
        rsv_port = pipe == 3 ? 1 : 0;
        dev_info->ports_pipe_max[pipe] = _TH2_DEV_PORTS_PER_PIPE - rsv_port;
    }
    /* 10G is the min speed in TDM calendar program. */
    dev_info->tdm_speed_min = SPEED_10G;

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
                            BCMI_XGS5_FLEXPORT_RESTRICTION_PM_SINGLE_PLL;

    dev_info->encap_mask = BCM_PA_ENCAP_IEEE;
    if(soc_feature(unit, soc_feature_higig2)) {
        dev_info->encap_mask |= BCM_PA_ENCAP_HIGIG2;
        dev_info->hg_encap_mask |= BCM_PA_ENCAP_HIGIG2;
    }

    /*
     * Restriction 13:No port configurations with more than 4 port speed
     *                classes are supported for LR, no port configurations with
     *                more than 5 port speed classes are supported for OS.
     * Restriction 15:All port configurations with 1-3 port speed classes are
     *                supported, except configurations that contain both 20G
     *                and 25G port speeds.
     */
    dev_info->speed_class.speed_class_num = si->oversub_mode ?
                    _TH2_TDM_OS_SPEED_CLASS_MAX : _TH2_TDM_LR_SPEED_CLASS_MAX;
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

    /* Auxiliary ports */
    SOC_PBMP_PORT_ADD(dev_info->aux_pbm, 0);   /* CPU port */
    SOC_PBMP_PORT_ADD(dev_info->aux_pbm, 33);  /* Loopback port on pipe0 */
    SOC_PBMP_PORT_ADD(dev_info->aux_pbm, 66);  /* Management0 on pipe1   */
    SOC_PBMP_PORT_ADD(dev_info->aux_pbm, 67);  /* Loopback port on pipe1 */
    SOC_PBMP_PORT_ADD(dev_info->aux_pbm, 100); /* Management1 on pipe2   */
    SOC_PBMP_PORT_ADD(dev_info->aux_pbm, 101); /* Loopback port on pipe2 */
    SOC_PBMP_PORT_ADD(dev_info->aux_pbm, 134); /* Reserved port on pipe3 */
    SOC_PBMP_PORT_ADD(dev_info->aux_pbm, 135); /* Loopback port on pipe3 */

    /* Set max line rate bandwidth per pipeline */
    BCM_IF_ERROR_RETURN
        (soc_th2_max_lr_bandwidth(unit, &pipe_lr_bw));
    dev_info->pipe_lr_bw = pipe_lr_bw * 1000;

    /* 10G is the min speed in TDM calculations */
    dev_info->tdm_pipe_lr_bw_limit_checker_speed_min = SPEED_10G;

    /* Set valid speeds for 1/2/4 Lanes */
    BCM_IF_ERROR_RETURN
        (soc_th2_support_speeds(unit, 1, &dev_info->speed_valid[1]));
    BCM_IF_ERROR_RETURN
        (soc_th2_support_speeds(unit, 2, &dev_info->speed_valid[2]));
    BCM_IF_ERROR_RETURN
        (soc_th2_support_speeds(unit, 4, &dev_info->speed_valid[4]));

    /*
     * Set physical port info:
     *  - Port lane capability
     *  - Port is enabled for flexport
     *  - pipe
     */
    for (phy_port = 0; phy_port < _TH2_PHY_PORTS_PER_DEV; phy_port++) {
        /* Set information to invalid */
        dev_info->phy_port_info[phy_port].pipe = -1;
    }

    /* Flexport is not supported for CPU/LB/MNG ports, so skip CPU/LB/MNG */
    for (phy_port = 1; phy_port <= _TH2_GPHY_PORTS_PER_DEV; phy_port++) {
        /* Check for end of port list */
        if (SOC_PORT_BLOCK(unit, phy_port) < 0 &&
            SOC_PORT_BINDEX(unit, phy_port) < 0) {
            break;
        }

        /* Set pipe information */
        dev_info->phy_port_info[phy_port].pipe =
                                    (phy_port - 1) / _TH2_GPHY_PORTS_PER_PIPE;

        /* Set TSC info from OTP */
        memset(&tsc_info, 0, sizeof(tsc_info));
        tsc_id = (phy_port - 1) / _TH2_PORTS_PER_PBLK;
        BCM_IF_ERROR_RETURN
            (soc_tomahawk2_tsc_otp_info_get(unit, tsc_id, &tsc_info));
        dev_info->phy_port_info[phy_port].force_lb = tsc_info.force_loopback;
        dev_info->phy_port_info[phy_port].force_hg = tsc_info.force_hg;
        dev_info->phy_port_info[phy_port].max_lane_speed =
                                                     tsc_info.max_lane_speed;
        if (tsc_info.disable) {
            dev_info->phy_port_info[phy_port].flex = 0;
            continue;
        }

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
    }

    /* Init condition */
    _bcmi_th2_init_condition_set(unit, dev_info);

    return BCM_E_NONE;
}

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
bcmi_th2_port_fn_drv_init(int unit)
{
    int phase_pos, present;

    if (!bcmi_th2_port_drv_internal_initialized) {
        /* BCMI Driver init */
        sal_memset(&bcmi_th2_port_drv, 0, sizeof(bcmi_th2_port_drv));
        bcmi_th2_port_drv.port_calls = &bcmi_th2_port_calls;
        sal_memcpy(bcmi_th2_port_drv.port_calls->port_attach_exec,
                   th2_port_attach_exec,
                   sizeof(bcmi_th2_port_drv.port_calls->port_attach_exec));

        sal_memcpy(bcmi_th2_port_drv.port_calls->port_detach_exec,
                   th2_port_detach_exec,
                   sizeof(bcmi_th2_port_drv.port_calls->port_detach_exec));

        bcmi_th2_port_drv_internal_initialized = 1;
    }

    BCM_IF_ERROR_RETURN
        (bcmi_th2_port_dev_info_init(unit));
    bcmi_th2_port_drv.dev_info[unit] = bcmi_th2_dev_info[unit];

    /* Initialize Common XGS5 Port module */
    BCM_IF_ERROR_RETURN
        (bcmi_xgs5_port_fn_drv_init(unit,
                                    &bcm_th2_port_drv,
                                    &bcmi_th2_port_drv,
                                    NULL));
    BCM_IF_ERROR_RETURN(soc_check_flex_phase(unit, flexport_phases,
                                             soc_tomahawk2_flex_top_port_down,
                                             &present));
    /* Detach port enable/disable in DV Flexport */
    if (present) {
        BCM_IF_ERROR_RETURN
            (soc_detach_flex_phase(unit, flexport_phases,
                                   soc_tomahawk2_flex_top_port_down,
                                   &phase_pos));
    }
    BCM_IF_ERROR_RETURN(soc_check_flex_phase(unit, flexport_phases,
                                             soc_tomahawk2_flex_top_port_up,
                                             &present));
    if (present) {
        BCM_IF_ERROR_RETURN
            (soc_detach_flex_phase(unit, flexport_phases,
                                   soc_tomahawk2_flex_top_port_up,
                                   &phase_pos));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_th2_port_soc_resource_init
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
bcmi_th2_port_soc_resource_init(int unit, int nport,
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
        if (logic_port == _TH2_DEV_RSV_PORT) {
            return BCM_E_PORT;
        }

        pipe = logic_port/_TH2_DEV_PORTS_PER_PIPE;
        idb_port = (logic_port % _TH2_DEV_PORTS_PER_PIPE) - (pipe == 0 ? 1 : 0);
        mmu_port = pipe * _TH2_MMU_PORTS_OFFSET_PER_PIPE + idb_port;

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
        /* TH2 does NOT support port configuration with a mix of line-rate ports
         * and oversubscribed ports. */
        soc_resource[i].oversub = si->oversub_mode;

        if (resource[i].lanes > 1) {
            soc_resource[i].hsp = 1;
	    }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_th2_port_resource_validate
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
bcmi_th2_port_resource_validate(int unit,
                            soc_port_schedule_state_t *port_schedule_state)
{
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_th2_pre_flexport_tdm
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
bcmi_th2_pre_flexport_tdm(int unit,
                            soc_port_schedule_state_t *port_schedule_state)
{
    port_schedule_state->is_flexport = 1;

    BCM_IF_ERROR_RETURN
        (soc_th2_port_schedule_tdm_init(unit, port_schedule_state));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_th2_post_flexport_tdm
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
bcmi_th2_post_flexport_tdm(int unit,
                            soc_port_schedule_state_t *port_schedule_state)
{
    BCM_IF_ERROR_RETURN
        (soc_th2_soc_tdm_update(unit, port_schedule_state));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_th2_clport_update
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
bcmi_th2_clport_update(int unit,
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
    static soc_field_t port_fields[_TH2_PORTS_PER_PBLK] = {
        PORT0f, PORT1f, PORT2f, PORT3f
    };
    static const int mode_encodings[SOC_TH_PORT_RATIO_COUNT] = {
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
        for (lane = 0; lane < _TH2_PORTS_PER_PBLK; lane++) {
            if (si->port_p2l_mapping[phy_port_base + lane] != -1) {
                soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &rval,
                                  port_fields[lane], 1);
            }
        }
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_SOFT_RESETr(unit, logic_port, rval));

        /* Set port mode */
        soc_tomahawk2_port_ratio_get(unit, port_schedule_state, clport,
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
bcmi_th2_port_attach_mmu(int unit, int port)
{
    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port) ||
        IS_MANAGEMENT_PORT(unit, port)) {
        return BCM_E_PARAM;
    }

    /* Assign the base queues and number of queues for the specified port. */
    BCM_IF_ERROR_RETURN
        (soc_tomahawk_num_cosq_init_port(unit, port));

    /* Reconfigure THDI, THDO settings */
    BCM_IF_ERROR_RETURN
        (soc_th2_mmu_config_init_port(unit, port));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_th2_port_detach_asf
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
bcmi_th2_port_detach_asf(int unit, bcm_port_t port)
{
    if (soc_feature(unit, soc_feature_asf_multimode)) {
        BCM_IF_ERROR_RETURN
            (soc_th_port_asf_detach(unit, port));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th2_ipmc_repl_port_attach
 * Purpose:
 *      Initialize replication per-port configuration in the runtime.
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      port                 - (IN)  Logical port number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_th2_ipmc_repl_port_attach(int unit, bcm_port_t port)
{
    int rv = BCM_E_NONE;

#ifdef INCLUDE_L3
    if (soc_feature(unit, soc_feature_ip_mcast) &&
        soc_feature(unit, soc_feature_ip_mcast_repl)) {
        rv = bcm_th_ipmc_repl_port_attach(unit, port);
    }
#endif

    return rv;
}

/*
 * Function:
 *      bcm_th2_ipmc_repl_port_detach
 * Purpose:
 *      Clear replication per-port configuration in the runtime.
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      port                 - (IN)  Logical port number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_th2_ipmc_repl_port_detach(int unit, bcm_port_t port)
{
    int rv = BCM_E_NONE;

#ifdef INCLUDE_L3
    if (soc_feature(unit, soc_feature_ip_mcast) &&
        soc_feature(unit, soc_feature_ip_mcast_repl)) {
        rv = bcm_th_ipmc_repl_port_detach(unit, port);
    }
#endif

    return rv;
}

/*
 * Function:
 *      bcmi_th2_port_enable
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
bcmi_th2_port_enable(int unit,
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

    rv = soc_tomahawk2_flex_mmu_port_up(unit, port_schedule_state);
    if (BCM_SUCCESS(rv)) {
        rv = soc_tomahawk2_flex_ep_port_up(unit, port_schedule_state);
    }
    if (BCM_SUCCESS(rv)) {
        rv = soc_tomahawk2_flex_idb_port_up(unit, port_schedule_state);
    }
    if (BCM_SUCCESS(rv)) {
        rv = bcmi_esw_portctrl_mac_up(unit, nport, port);
    }
    sal_free(port);

    return rv;
}

/*
 * Function:
 *      bcmi_th2_port_disable
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
bcmi_th2_port_disable(int unit,
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
    if (BCM_SUCCESS(rv)) {
        rv = soc_tomahawk2_flex_idb_port_down(unit, port_schedule_state);
    }
    if ((BCM_SUCCESS(rv)) && (!(SAL_BOOT_XGSSIM || SAL_BOOT_BCMSIM))) {
        rv = soc_tomahawk2_flex_mmu_port_down(unit, port_schedule_state);
    }
    if (BCM_SUCCESS(rv)) {
        rv = soc_tomahawk2_flex_ep_port_down(unit, port_schedule_state);
    }
    if (BCM_SUCCESS(rv)) {
        rv = bcmi_esw_portctrl_mac_tx_down(unit, nport, port);
    }

    sal_free(port);

    return rv;
}

/*
 * Function:
 *      bcmi_th2_port_speed_1g_update
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
 *      We enable customers with limited 1G support in Tomahawk2.
 *      1G is not a support speed class in TDM calculation, thus treated same as
 *      10G in TDM calculation. 1G port does NOT support cut through, flow
 *      control.
 */
int
bcmi_th2_port_speed_1g_update(int unit, bcm_port_t port, int speed)
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
 *      bcmi_th2_speed_ability_get
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
bcmi_th2_speed_ability_get(int unit, bcm_port_t port, bcm_port_abil_t *mask)
{
    if (IS_MANAGEMENT_PORT(unit, port)) {
        /* TH2 Port Rates for Management ports:
         * 10M, 100M, 1G, 2.5G, 10G, HG[11G] */
        *mask = BCM_PORT_ABILITY_10MB | BCM_PORT_ABILITY_100MB |
                BCM_PORT_ABILITY_1000MB | BCM_PORT_ABILITY_2500MB |
                BCM_PORT_ABILITY_10GB | BCM_PORT_ABILITY_11GB;
    } else {
        BCM_IF_ERROR_RETURN
            (soc_th2_support_speeds(unit,
                                    SOC_INFO(unit).port_num_lanes[port],
                                    mask));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_th2_tsc_xgxs_reset
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
bcmi_th2_tsc_xgxs_reset(int unit, pbmp_t pbmp)
{
    int rv = BCM_E_NONE;
    soc_info_t *si;
    uint64 tsc_map, reset_tsc_map;
    int tsc, ports[_TH2_PBLKS_PER_DEV];
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

    for (tsc = 0; tsc < _TH2_PBLKS_PER_DEV; tsc++) {
        if (COMPILER_64_BITTEST(reset_tsc_map, tsc)) {
            rv = soc_tsc_xgxs_reset(unit, ports[tsc], 0);
        }
    }

    return rv;
}

int
bcmi_th2_speed_mix_validate(int unit, uint32 speed_mask)
{
    return soc_th2_speed_mix_validate(unit, speed_mask);
}

#endif /* BCM_TOMAHAWK2_SUPPORT */
