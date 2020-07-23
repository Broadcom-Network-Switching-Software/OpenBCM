/** \file tunnel_init_ipv4.c
 *  IPV4 TUNNEL encapsulation functionality for DNX
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif /* */
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TUNNEL
/*
 * Include files.
 * {
 */

#include <bcm/types.h>
#include <bcm/tunnel.h>

/*
 * }
 */
/*
 * MACROs and ENUMs
 * {
 */
/*
 * End of MACROs
 * }
 */
/*
 * Verify functions
 * {
 */

/* Indicate weather tunnel type is ipv4 */
void
dnx_tunnel_init_ipv4_is_ipv4_tunnel(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    uint32 *is_ipv4)
{
    switch (tunnel_type)
    {

        /** This is the list of all supported tunnel types */
        
        case bcmTunnelTypeGreAnyIn4:
        case bcmTunnelTypeIpAnyIn4:
        case bcmTunnelTypeUdp:
        case bcmTunnelTypeVxlan:
        case bcmTunnelTypeVxlanGpe:
        {
            *is_ipv4 = TRUE;
            break;
        }
        default:
        {
            *is_ipv4 = FALSE;
        }
    }
}
