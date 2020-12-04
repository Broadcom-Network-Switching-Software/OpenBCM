/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        port.c
 * Purpose:     MV2 SOC Port driver.
 */

#include <shared/bsl.h>
#include <soc/error.h>
#include <soc/defs.h>
#include <soc/tdm/core/tdm_top.h>

#if defined(BCM_FIREBOLT6_SUPPORT)
#include <soc/firebolt6.h>
#include <soc/maverick2.h>
#include <soc/scache.h>

/*
 * Function:
 *      soc_fb6_max_lr_bandwidth
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
soc_fb6_max_lr_bandwidth(int unit, uint32 *max_lr_bw)
{
    soc_info_t *si = &SOC_INFO(unit);
    int cal_universal;

    cal_universal = si->fabric_port_enable ? 1 : 0;

    switch (si->frequency) {
    case 1700:
        *max_lr_bw = cal_universal ? 1050 : 1095;
        break;
    case 1625:
        *max_lr_bw = cal_universal ? 1002 : 1047;
        break;
    case 1525:
        *max_lr_bw = cal_universal ? 940 : 980;
        break;
    case 1425:
        *max_lr_bw = cal_universal ? 877 : 915;
        break;
    case 1325:
        *max_lr_bw = cal_universal ? 812 : 847;
        break;
    case 1275:
        *max_lr_bw = cal_universal ? 782 : 815;
        break;
    case 1225:
        *max_lr_bw = cal_universal ? 750 : 782;
        break;
    case 1125:
        *max_lr_bw = cal_universal ? 687 : 717;
        break;
    case 1050:
        *max_lr_bw = cal_universal ? 640 : 667;
        break;
    case 950:
        *max_lr_bw = cal_universal ? 575 : 600;
        break;
    case 850:
        *max_lr_bw = cal_universal ? 512 : 535;
        break;
    default:
        *max_lr_bw = 0;
        break;
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_firebolt6_port_mode_get
 * Description:
 *      Get port mode by giving first logical port of TSC3
 * Parameters:
 *      unit          - Device number
 *      port          - Logical port
 *      int*          - Port Mode
 *
 * Each TSC3 can be configured into following 5 mode:
 *   Lane number    0    1    2    3
 *   ------------  ---  ---  ---  ---
 *      quad port  25G  25G  25G  25G
 *   tri_012 port  25G  25G  50G   x
 *   tri_023 port  50G   x   25G  25G
 *      dual port  50G   x   50G   x
 *    single port  100G   x    x    x
 */
int
soc_firebolt6_port_mode_get(int unit, int logical_port, int *mode)
{
    soc_info_t *si;
    int lane;
    int port, first_phyport, phy_port;
    int num_lanes[FB6_PORTS_PER_TSC];

    si = &SOC_INFO(unit);
    /* Each core is 4 lane core to get the first divide by 4 then multiply by 4
     * physical port is 1 => ((2-1)/4)*4+1 = 1
     * physical port is 7 => ((7-1)/4)*4+1 = 5
     */
	first_phyport = (((si->port_l2p_mapping[logical_port]-1)/4)*4)+1;

    for (lane = 0; lane < FB6_PORTS_PER_TSC; lane++) {
        phy_port = first_phyport + lane;
        port = si->port_p2l_mapping[phy_port];
        if (port == -1 || SOC_PBMP_MEMBER(si->all.disabled_bitmap, port)) {
            num_lanes[lane] = -1;
        } else {
            num_lanes[lane] = si->port_num_lanes[port];
        }
    }

    if (num_lanes[0] == 4) {
        *mode = SOC_FB6_PORT_MODE_QUAD;
    } else if (num_lanes[0] == 2 && num_lanes[2] == 2) {
        *mode = SOC_FB6_PORT_MODE_DUAL;
    } else if (num_lanes[0] == 2 && num_lanes[2] == 1 && num_lanes[3] == 1) {
        *mode = SOC_FB6_PORT_MODE_TRI_023;
    } else if (num_lanes[0] == 1 && num_lanes[1] == 1 && num_lanes[2] == 2) {
        *mode = SOC_FB6_PORT_MODE_TRI_012;
    } else {
        *mode = SOC_FB6_PORT_MODE_SINGLE;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_fb6_support_speeds
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
soc_fb6_support_speeds(int unit, int lanes, uint32 *speed_mask)
{
    soc_info_t *si = &SOC_INFO(unit);
    uint32 speed_valid;

    switch(lanes) {
    case 1:
        /* Single Lane Port Speeds */
        speed_valid =   SOC_PA_SPEED_1000MB | SOC_PA_SPEED_2500MB |
                        SOC_PA_SPEED_5000MB | SOC_PA_SPEED_10GB   |
                        SOC_PA_SPEED_11GB   | SOC_PA_SPEED_25GB   |
                        SOC_PA_SPEED_27GB;
        break;
    case 2:
        /* Dual Lane Port Speeds */
        speed_valid =   SOC_PA_SPEED_20GB | SOC_PA_SPEED_21GB |
                        SOC_PA_SPEED_40GB | SOC_PA_SPEED_42GB |
                        SOC_PA_SPEED_50GB | SOC_PA_SPEED_53GB;
        break;
    case 4:
        /* Quad Lane Port Speeds */
        speed_valid =   SOC_PA_SPEED_40GB  | SOC_PA_SPEED_42GB |
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
    *speed_mask = speed_valid;

    return SOC_E_NONE;
}

#endif /* BCM_FIREBOLT6_SUPPORT */
