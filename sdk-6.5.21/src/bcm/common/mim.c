/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * Mac-in-Mac initializers
 */
#if defined(INCLUDE_L3)
#include <sal/core/libc.h>
 
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/l2u.h>
#include <soc/util.h>
#include <soc/debug.h>

#include <bcm/port.h>
#include <bcm/error.h> 
#include <bcm/mim.h>

/* 
 * Function:
 *      bcm_mim_vpn_config_t_init
 * Purpose:
 *      Initialize the MiM vpn config struct
 * Parameters: 
 *      mim_vpn_config - Pointer to the struct to be init'ed
 */
void
bcm_mim_vpn_config_t_init(bcm_mim_vpn_config_t *mim_vpn_config)
{
    bcm_vlan_protocol_packet_ctrl_t *protocol_pkt_def; /* default action */
    if (mim_vpn_config != NULL) {
        sal_memset(mim_vpn_config, 0, sizeof(*mim_vpn_config));
        protocol_pkt_def = &mim_vpn_config->protocol_pkt;
        mim_vpn_config->inlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE;
        mim_vpn_config->outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE;
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_REG_FIELD_VALID(0, PROTOCOL_PKT_CONTROLr, MMRP_FWD_ACTIONf))
#endif
        {
            protocol_pkt_def->mmrp_action = BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
        }
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_REG_FIELD_VALID(0, PROTOCOL_PKT_CONTROLr, SRP_FWD_ACTIONf))
#endif
        {
            protocol_pkt_def->srp_action = BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
        }
        protocol_pkt_def->igmp_report_leave_action= BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
        protocol_pkt_def->igmp_query_action= BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
        protocol_pkt_def->igmp_unknown_msg_action= BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
        protocol_pkt_def->mld_report_done_action= BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
        protocol_pkt_def->mld_query_action= BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
        protocol_pkt_def->ip4_mc_router_adv_action= BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
        protocol_pkt_def->ip4_rsvd_mc_action= BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
        protocol_pkt_def->ip6_mc_router_adv_action= BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
        protocol_pkt_def->ip6_rsvd_mc_action= BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
    }
    return;
}

/* 
 * Function:
 *      bcm_mim_port_t_init
 * Purpose:
 *      Initialize the MiM port struct
 * Parameters: 
 *      port_info - Pointer to the struct to be init'ed
 */
void
bcm_mim_port_t_init(bcm_mim_port_t *port_info)
{   
    if (port_info != NULL) {
        sal_memset(port_info, 0, sizeof(*port_info));
    }
    return;
}

#else
typedef int _bcm_mim_not_empty; /* Make ISO compilers happy. */
#endif  /* INCLUDE_L3 */

