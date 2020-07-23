/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * Flow structure initializers
 */

#if defined(INCLUDE_L3)

#include <sal/core/libc.h>
 
#include <bcm/flow.h>

/*
 * Function:
 *      bcm_flow_logical_field_t_init
 * Purpose:
 *      Initialize bcm_flow_logical_field_t struct
 * Parameters:
 *      info - Pointer to the struct to be init'ed
 */
void
bcm_flow_logical_field_t_init(bcm_flow_logical_field_t *info)
{
    if (info != NULL) {
        sal_memset(info, 0, sizeof(*info));
    }
    return;
}

/* 
 * Function:
 *      bcm_flow_match_config_t_init
 * Purpose:
 *      Initialize flow match config struct
 * Parameters: 
 *      flow_match - Pointer to the struct to be init'ed
 */
void 
bcm_flow_match_config_t_init(bcm_flow_match_config_t *flow_match)
{
    if (flow_match != NULL) {
        sal_memset(flow_match, 0, sizeof(*flow_match));
    }
    return;
}

/* 
 * Function:
 *      bcm_flow_port_encap_t_init
 * Purpose:
 *      Initialize flow egress encap struct
 * Parameters: 
 *      info - Pointer to the struct to be init'ed
 */
void
bcm_flow_port_encap_t_init(bcm_flow_port_encap_t *info)
{
    if (info != NULL) {
        sal_memset(info, 0, sizeof(*info));
    }
    return;
}

/*
 * Function:
 *      bcm_flow_encap_config_t_init
 * Purpose:
 *      Initialize flow encap config struct
 * Parameters:
 *      info - Pointer to the struct to be init'ed
 */
void
bcm_flow_encap_config_t_init(bcm_flow_encap_config_t *info)
{
    if (info != NULL) {
        sal_memset(info, 0, sizeof(*info));
    }
    return;
}

/*
 * Function:
 *      bcm_flow_port_t_init
 * Purpose:
 *      Initialize FLOW  port struct. 
 *      To used be before creating the VP.
 *   Parameters:
 *      flow_port - Pointer to the struct to be init'ed
 */
void
bcm_flow_port_t_init(bcm_flow_port_t *flow_port)
{
    if (flow_port != NULL) {
        sal_memset(flow_port, 0, sizeof(*flow_port));
    }   
     return;
}

/* 
 * Function:
 *      bcm_flow_vpn_config_t_init
 * Purpose:
 *      Initialize FLOW  vpn config struct
 * Parameters: 
 *      L2 VPN - Pointer to the struct to be init'ed
 */
void
bcm_flow_vpn_config_t_init(bcm_flow_vpn_config_t *l2_vpn)
{
    if (l2_vpn != NULL) {
        sal_memset(l2_vpn, 0, sizeof(*l2_vpn));
    }
    return;
}

/*
 * Function:
 *      bcm_flow_tunnel_initiator_t_init
 * Purpose:
 *      Initialize bcm_flow_tunnel_initiator_t struct
 * Parameters:
 *      info - Pointer to the struct to be init'ed
 */
void
bcm_flow_tunnel_initiator_t_init(bcm_flow_tunnel_initiator_t *info)
{
    if (info != NULL) {
        sal_memset(info, 0, sizeof(*info));
    }
    return;
}
/*
 * Function:
 *      bcm_flow_tunnel_terminator_t_init
 * Purpose:
 *      Initialize bcm_flow_tunnel_terminator_t struct
 * Parameters:
 *      info - Pointer to the struct to be init'ed
 */
void
bcm_flow_tunnel_terminator_t_init(bcm_flow_tunnel_terminator_t *info)
{
    if (info != NULL) {
        sal_memset(info, 0, sizeof(*info));
    }
    return;
}

/*
 * Function:
 *      bcm_flow_stat_info_t_init
 * Purpose:
 *      Initialize flow stat info struct
 * Parameters:
 *      flow_match - Pointer to the struct to be init'ed
 */
void
bcm_flow_stat_info_t_init(bcm_flow_stat_info_t *flow_stat)
{
    if (flow_stat != NULL) {
        sal_memset(flow_stat, 0, sizeof(*flow_stat));
    }
    return;
}

/*
 * Function:
 *      bcm_flow_handle_info_t_init
 * Purpose:
 *      Initialize flow handle info struct
 * Parameters:
 *      flow_handle_info - Pointer to the struct to be init'ed
 */
void bcm_flow_handle_info_t_init(bcm_flow_handle_info_t *flow_handle_info)
{
    if (flow_handle_info != NULL) {
        sal_memset(flow_handle_info, 0, sizeof(*flow_handle_info));
    }
    return;
}

/*
 * Function:
 *      bcm_flow_l3_ingress_info_t_init
 * Purpose:
 *      Initialize flow l3 ingress info struct
 * Parameters:
 *      flow_l3_ingress_info - Pointer to the struct to be init'ed
 */
void bcm_flow_l3_ingress_info_t_init(bcm_flow_l3_ingress_info_t *flow_l3_ingress_info)
{
    if (flow_l3_ingress_info != NULL) {
        sal_memset(flow_l3_ingress_info, 0, sizeof(*flow_l3_ingress_info));
    }
    return;
}
#else
typedef int _bcm_flow_not_empty; /* Make ISO compilers happy. */
#endif  /* INCLUDE_L3 */


