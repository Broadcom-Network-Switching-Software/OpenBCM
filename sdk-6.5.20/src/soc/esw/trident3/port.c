/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        port.c
 * Purpose:     TD3 SOC Port driver.
 */

#include <shared/bsl.h>
#include <soc/error.h>
#include <soc/defs.h>
#include <soc/tdm/core/tdm_top.h>

#if defined(BCM_TRIDENT3_SUPPORT)
#include <soc/tdm/trident3/tdm_td3_defines.h>
#include <soc/flexport/trident3/trident3_flexport_defines.h>
#include <soc/trident3.h>
#include <soc/scache.h>
#include <soc/types.h>

#define TD3_PORTS_PER_TSC 4

#ifdef BCM_WARM_BOOT_SUPPORT

#define SOC_FLEXPORT_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define SOC_FLEXPORT_WB_DEFAULT_VERSION            SOC_FLEXPORT_WB_VERSION_1_0

extern int
_soc_td3_port_speed_cap[SOC_MAX_NUM_DEVICES][132];
#endif
/*
 * Function:
 *      soc_td3_max_lr_bandwidth
 * Purpose:
 *      Get the max line rate bandwidth per pipeline
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      max_lr_bw            - (OUT) Max line rate bandwidth (Gbps).
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
soc_td3_max_lr_bandwidth(int unit, uint32 *max_lr_bw)
{
    soc_info_t *si = &SOC_INFO(unit);
    int second_mgmt_enable = 0;
    int rv = SOC_E_NONE;

    second_mgmt_enable = SOC_PBMP_MEMBER(PBMP_MANAGEMENT(unit), 128) ? 1 : 0;

    switch (si->frequency) {
    case 1700:
        *max_lr_bw =  1112;
        break;
    case 1625:
        *max_lr_bw =  1062;
        break;
    case 1525:
        *max_lr_bw =  994;
        break;
    case 1425:
        *max_lr_bw =  927;
        break;
    case 1325:
        *max_lr_bw =  860;
        break;
    case 1275:
        *max_lr_bw =  826;
        break;
    case 1012:
        *max_lr_bw =  650;
        break;
    case 850:
        *max_lr_bw =  541;
        break;
    default:
        *max_lr_bw = 0;
        rv = SOC_E_INTERNAL;
        break;
    }

    if (second_mgmt_enable && *max_lr_bw > 0) {
        *max_lr_bw =  *max_lr_bw - 10;
    }
    return rv;
}

/*
 * Function:
 *      soc_td3_max_lr_cal_len
 * Purpose:
 *      Get the max cal length per pipe
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      eth                  - (IN)  ethernet mode
 *      max_lr_cal           - (OUT) Max line rate cal (slots).
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
soc_td3_max_lr_cal_len(int unit, uint32 *max_lr_cal, int eth)
{
    soc_info_t *si = &SOC_INFO(unit);
    int second_mgmt_enable = 0;
    int rv = SOC_E_NONE;

    second_mgmt_enable = SOC_PBMP_MEMBER(PBMP_MANAGEMENT(unit), 128) ? 1 : 0;

    switch (si->frequency) {
    case 1700:
        *max_lr_cal =  eth ? 456 : 430;
        break;
    case 1625:
        *max_lr_cal =  eth ? 436 : 411;
        break;
    case 1525:
        *max_lr_cal =  eth ? 409 : 386;
        break;
    case 1425:
        *max_lr_cal =  eth ? 383 : 361;
        break;
    case 1325:
        *max_lr_cal =  eth ? 356 : 335;
        break;
    case 1275:
        *max_lr_cal =  eth ? 342 : 323;
        break;
    case 1012:
        *max_lr_cal =  eth ? 272 : 256;
        break;
    case 850:
        *max_lr_cal =  eth ? 228 : 215;
        break;
    default:
        *max_lr_cal = 0;
        rv = SOC_E_INTERNAL;
        break;
    }
    if (*max_lr_cal > 0) {
        *max_lr_cal = *max_lr_cal - 12; /* ANCL */
    }
    if (second_mgmt_enable && *max_lr_cal > 0) {
        *max_lr_cal =  *max_lr_cal - 4;
    }
    return rv;
}

/*
 * Function:
 *      soc_td3_support_speeds
 * Purpose:
 *      Get the supported speed of port for specified lanes
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      lanes                - (IN)  Number of Lanes for the port.
 *      speed_mask           - (IN)  Bitmap for supported speed.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
soc_td3_support_speeds(int unit, int lanes, uint32 *speed_mask)
{
    soc_info_t *si = &SOC_INFO(unit);
    uint32 speed_valid;

    switch(lanes) {
    case 1:
        /* Single Lane Port Speeds */
        speed_valid = SOC_PA_SPEED_1000MB | SOC_PA_SPEED_10GB |
                        SOC_PA_SPEED_11GB | SOC_PA_SPEED_25GB |
                        SOC_PA_SPEED_27GB;
        break;
    case 2:
        /* Dual Lane Port Speeds */
        speed_valid = SOC_PA_SPEED_20GB | SOC_PA_SPEED_21GB |
                        SOC_PA_SPEED_40GB | SOC_PA_SPEED_42GB |
                        SOC_PA_SPEED_50GB | SOC_PA_SPEED_53GB;
        break;
    case 4:
        /* Quad Lane Port Speeds */
        speed_valid = SOC_PA_SPEED_40GB | SOC_PA_SPEED_42GB |
                        SOC_PA_SPEED_100GB | SOC_PA_SPEED_106GB;
        break;
    default:
        return SOC_E_PARAM;
    }

    if (FALSE == si->fabric_port_enable) {
        /* Ethernet-optimized Calendar doesn't support any HG speed */
        speed_valid &= ~(SOC_PA_SPEED_11GB | SOC_PA_SPEED_21GB |
                            SOC_PA_SPEED_27GB | SOC_PA_SPEED_42GB |
                            SOC_PA_SPEED_53GB | SOC_PA_SPEED_106GB);
    }

    if (si->frequency < 1300) {
        speed_valid &= ~(SOC_PA_SPEED_100GB | SOC_PA_SPEED_106GB);
    }

    if (si->frequency < 1012) {
        speed_valid &= ~(SOC_PA_SPEED_50GB | SOC_PA_SPEED_53GB);
        speed_valid &= ~(SOC_PA_SPEED_25GB | SOC_PA_SPEED_27GB);
    }

    if (si->frequency < 850) {
        speed_valid &= ~(SOC_PA_SPEED_40GB | SOC_PA_SPEED_42GB);
        speed_valid &= ~(SOC_PA_SPEED_20GB | SOC_PA_SPEED_21GB);
        speed_valid &= ~(SOC_PA_SPEED_10GB | SOC_PA_SPEED_11GB);
    }

    *speed_mask = speed_valid;

    return SOC_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
int soc_td3_flexport_scache_allocate(int unit)
{
    int rv = SOC_E_NONE;
    uint8 *flexport_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32 alloc_get = 0;
    uint32 alloc_size = 0;
    int stable_size;
    int default_ver = SOC_FLEXPORT_WB_DEFAULT_VERSION;

    alloc_size =  (sizeof(int) * TRIDENT3_PHY_PORTS_PER_DEV)  + /* phy to logical*/
                  (sizeof(int) * TRIDENT3_PHY_PORTS_PER_DEV) + /* phy to mmu */
                  (sizeof(int) * TRIDENT3_PHY_PORTS_PER_DEV) + /* logical to phy */
                  (sizeof(int) * TRIDENT3_PHY_PORTS_PER_DEV) + /* max port speed */
                  (sizeof(int) * TRIDENT3_PHY_PORTS_PER_DEV) + /* init port speed */
                  (sizeof(int) * TRIDENT3_PHY_PORTS_PER_DEV) + /* num of lanes */
                  (sizeof(pbmp_t))                       + /* HG bitmap */
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
 *      soc_td3_flexport_scache_sync
 * Purpose:
 *      Record Port info that maybe changed during flexport for Level 2 WB
 *
 * Warm Boot Version Map:
 *
 *  BCM_WB_VERSION_1_0
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
int soc_td3_flexport_scache_sync(int unit)
{
    uint8 *flexport_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32 alloc_get = 0;
    uint32 alloc_size = 0;
    uint32 var_size = 0;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 scache_offset=0;
    int rv = 0;

    alloc_size =  (sizeof(int) * TRIDENT3_PHY_PORTS_PER_DEV)  + /* phy to logical*/
                  (sizeof(int) * TRIDENT3_PHY_PORTS_PER_DEV) + /* phy to mmu */
                  (sizeof(int) * TRIDENT3_PHY_PORTS_PER_DEV) + /* logical to phy */
                  (sizeof(int) * TRIDENT3_PHY_PORTS_PER_DEV) + /* max port speed */
                  (sizeof(int) * TRIDENT3_PHY_PORTS_PER_DEV) + /* init port speed */
                  (sizeof(int) * TRIDENT3_PHY_PORTS_PER_DEV) + /* num of lanes */
                  (sizeof(pbmp_t))                       + /* HG bitmap */
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
    var_size = sizeof(int) * TRIDENT3_PHY_PORTS_PER_DEV;
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_p2l_mapping, var_size);
    scache_offset += var_size;


    /* Physical to MMU port mapping */
    var_size = sizeof(int) * TRIDENT3_PHY_PORTS_PER_DEV;
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_p2m_mapping, var_size);
    scache_offset += var_size;

    /* Logical to Physical port mapping */
    var_size = sizeof(int) * TRIDENT3_PHY_PORTS_PER_DEV;
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_l2p_mapping, var_size);
    scache_offset += var_size;

    /* Max port speed */
    var_size = sizeof(int) * TRIDENT3_PHY_PORTS_PER_DEV;

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

    /* HG Bitmap */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               &si->hg.bitmap,
               sizeof(pbmp_t));
    scache_offset += sizeof(pbmp_t);

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
 *      soc_td3_flexport_scache_recovery
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
int soc_td3_flexport_scache_recovery(int unit)
{
    uint32 alloc_get = 0, phy_port =0;
    uint32 alloc_size = 0;
    int rv = SOC_E_NONE;
    uint8 *flexport_scache_ptr = NULL;
    soc_scache_handle_t scache_handle;
    uint32 scache_offset=0;
    uint32 var_size = 0;
    uint16 recovered_ver = 0, pipe=0;
    soc_info_t *si = &SOC_INFO(unit);

    alloc_size =  (sizeof(int) * TRIDENT3_PHY_PORTS_PER_DEV) + /* phy to logical*/
                  (sizeof(int) * TRIDENT3_PHY_PORTS_PER_DEV) + /* phy to mmu */
                  (sizeof(int) * TRIDENT3_PHY_PORTS_PER_DEV) + /* logical to phy */
                  (sizeof(int) * TRIDENT3_PHY_PORTS_PER_DEV) + /* max port speed */
                  (sizeof(int) * TRIDENT3_PHY_PORTS_PER_DEV) + /* init port speed */
                  (sizeof(int) * TRIDENT3_PHY_PORTS_PER_DEV) + /* num of lanes */
                  (sizeof(pbmp_t))                         + /* HG bitmap */
                  (sizeof(pbmp_t));                          /* Disabled bitmap */

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
    var_size = (sizeof(int) * TRIDENT3_PHY_PORTS_PER_DEV);
    sal_memcpy(si->port_p2l_mapping,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /*Physical to MMU port mapping */
    var_size = (sizeof(int) * TRIDENT3_PHY_PORTS_PER_DEV);
    sal_memcpy(si->port_p2m_mapping,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /*Logical to Physical port mapping*/
    var_size = (sizeof(int) * TRIDENT3_PHY_PORTS_PER_DEV);
    sal_memcpy(si->port_l2p_mapping,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* Max port speed */
    var_size = (sizeof(int) * TRIDENT3_PHY_PORTS_PER_DEV);

    sal_memcpy(_soc_td3_port_speed_cap[unit],
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

    /* HG Bitmap */
    sal_memcpy(&si->hg.bitmap,
           &flexport_scache_ptr[scache_offset],
           sizeof(pbmp_t));
    scache_offset += sizeof(pbmp_t);

    /* Disabled Port Bitmap */
    sal_memcpy(&si->all.disabled_bitmap,
           &flexport_scache_ptr[scache_offset],
           sizeof(pbmp_t));
    scache_offset += sizeof(pbmp_t);


    for (pipe = 0; pipe < TRIDENT3_PIPES_PER_DEV; pipe++) {
        SOC_PBMP_CLEAR(si->pipe_pbm[pipe]);
    }
    for (phy_port = 1; phy_port < TRIDENT3_PHY_PORTS_PER_DEV; phy_port++) {
        if (si->port_l2p_mapping[phy_port] == -1) {
            continue;
        }
        pipe =  phy_port / TRIDENT3_PHY_PORTS_PER_PIPE;
        si->port_pipe[phy_port] = pipe;
        SOC_PBMP_PORT_ADD(si->pipe_pbm[pipe], phy_port);
    }
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "%s()[LINE:%d] \n"),FUNCTION_NAME(),  __LINE__));

    return SOC_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
void
soc_td3_flexport_sw_dump(int unit)
{
    int port, phy_port, mmu_port, pipe, pm, cosq, numq, uc_cosq, uc_numq;
    int max_speed, init_speed, num_lanes;
    char  pfmt[SOC_PBMP_FMT_LEN];

    LOG_CLI((BSL_META_U(unit,
                            "  port(log/phy/mmu)  pipe  pm  lanes    "
                            "speed(Max)    uc_Qbase/Numq mc_Qbase/Numq\n")));
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

        LOG_CLI((BSL_META_U(unit,
                            "  %4s(%3d/%3d/%3d)  %4d  %2d  %5d %7d(%7d) "
                            "%6d/%-6d  %6d/%-6d\n"),
                SOC_INFO(unit).port_name[port], port, phy_port, mmu_port,
                pipe, pm, num_lanes, init_speed, max_speed,
                uc_cosq, uc_numq, cosq, numq));
    }

    LOG_CLI((BSL_META_U(unit,
                        "\n    Oversub Bitmap: %s"),
       SOC_PBMP_FMT(SOC_INFO(unit).oversub_pbm, pfmt)));
    LOG_CLI((BSL_META_U(unit,
                        "\n    Disabled Bitmap: %s \n"),
       SOC_PBMP_FMT(SOC_INFO(unit).all.disabled_bitmap, pfmt)));
}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

/*
 * Function:
 *      soc_td3_port_mode_get
 * Description:
 *      Get port mode by giving first logical port of a core
 * Parameters:
 *      unit          - Device number
 *      port          - Logical port
 *      int*          - Port Mode
 *
 * Each core can be configured into following 5 mode:
 *   Lane number    0    1    2    3
 *   ------------  ---  ---  ---  ---
 *      quad port  10G  10G  10G  10G
 *   tri_012 port  10G  10G  20G   x
 *   tri_023 port  20G   x   10G  10G
 *      dual port  20G   x   20G   x
 *    single port  40G   x    x    x
 */
int
soc_td3_port_mode_get(int unit, int logical_port, int *mode)
{
    soc_info_t *si;
    int lane;
    int port, first_phyport, phy_port;
    int num_lanes[TD3_PORTS_PER_TSC];

    si = &SOC_INFO(unit);
    first_phyport = si->port_l2p_mapping[logical_port];

    for (lane = 0; lane < TD3_PORTS_PER_TSC; lane++) {
        phy_port = first_phyport + lane;
        port = si->port_p2l_mapping[phy_port];
        if (port == -1 || SOC_PBMP_MEMBER(si->all.disabled_bitmap, port)) {
            num_lanes[lane] = -1;
        } else {
            num_lanes[lane] = si->port_num_lanes[port];
        }
    }

    if (num_lanes[0] == 4) {
        *mode = SOC_TD3_PORT_MODE_QUAD;
    } else if (num_lanes[0] == 2 && num_lanes[2] == 2) {
        *mode = SOC_TD3_PORT_MODE_DUAL;
    } else if (num_lanes[0] == 2) {
        *mode = SOC_TD3_PORT_MODE_TRI_023;
    } else if (num_lanes[2] == 2) {
        *mode = SOC_TD3_PORT_MODE_TRI_012;
    } else {
        *mode = SOC_TD3_PORT_MODE_SINGLE;
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_td3_speed_mix_validate
 * Purpose:
 *      Check if the mix of all ports speed is valid
 * Parameters:
 *      unit       - Unit.
 *      speed_mask - Bitmap of speed on all ports
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_CONFIG
 */
int
soc_td3_speed_mix_validate(int unit, uint32 speed_mask)
{
    soc_info_t *si = &SOC_INFO(unit);
    uint32 count;

    count = _shr_popcount(speed_mask);

    if (si->oversub_mode) {
        if ((count >= 4) &&
            (speed_mask & SOC_PA_SPEED_20GB) &&
            ((speed_mask & SOC_PA_SPEED_25GB) || (speed_mask & SOC_PA_SPEED_50GB))) {
            LOG_ERROR(BSL_LS_BCM_COMMON,
                  (BSL_META_U(unit,
                              "Port configurations that contain 4 or more speed class "
                              "in oversub mode,  both 20G and 25G/50G port speed "
                              "are not supported.\n")));
            return SOC_E_PARAM;
        }
    } else {
        if ((count == 4) &&
            ((speed_mask & SOC_PA_SPEED_20GB) || (speed_mask & SOC_PA_SPEED_40GB)) &&
            ((speed_mask & SOC_PA_SPEED_25GB) || (speed_mask & SOC_PA_SPEED_50GB))) {
            LOG_ERROR(BSL_LS_BCM_COMMON,
                  (BSL_META_U(unit,
                              "Port configurations that contain 4 speed class "
                              "and both 20G/40G and 25G/50G port speed "
                              "are not supported.\n")));
            return SOC_E_PARAM;
        }
    }
    if ((count < 4) &&
        ((speed_mask & SOC_PA_SPEED_20GB) && (speed_mask & SOC_PA_SPEED_25GB))) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                  (BSL_META_U(unit,
                              "Port configurations that contain both 20G "
                              "and 25G port speed are not supported.\n")));
        return SOC_E_PARAM;
    }

    return SOC_E_NONE;
}
#endif /* BCM_TRIDENT3_SUPPORT */
