/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_sand_ipv6_tunnel_term.c Purpose: Various examples for IPv6 Tunnels.
 */

/*
 * Configuration:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../../../src/examples/sand/cint_sand_ip_tunnel.c
 * cint ../../../../src/examples/sand/cint_sand_ipv6_tunnel_term.c
 * cint
 * int unit = 0;
 * int rv = 0;
 * int provider_port = 200;
 * int access_port   = 201;
 * int nof_tunnels = 2;
 * rv = ipv6_tunnel_terminate_tunnels_example($unit,$provider_port,$access_port,$nof_tunnels);
 * print rv;
 *
 * Scenarios configured in this cint:
 *  ~~~~~~~~~~~~~~~~~~~~~~~~
 *  IPv4oIPv6oETH:
 * - terminate IP tunnels.
 * - do routing after termination
 *  ~~~~~~~~~~~~~~~~~~~~~~~~
 *  An IPv6 tunnel header is being terminated.
 *
 * buid two IP tunnels terminations
 * Tunnnel 1:
 *   -  sip6  = 2000:0000:0000:0000:0200:77FF:FEFD:FCFB
 *   -  dip6  = 2000:FEFD:FCFB:FAF9:F8F7:F6F5:F4F3:F2F1
 *   -  type  = bcmTunnelTypeIp4In6/bcmTunnelTypeGre4In6;
 *      sip   = 192.128.1.4
 *      dip   = 1.0.0.17
 *   -  Vlan  = 10
 *   -  Dmac  = 00:0C:00:02:00:00
 *   -  Smac  = 00:00:07:00:01:00
 *
 * Tunnnel 2:
 *   -  sip6  = 2000:0000:0000:0000:0300:77FF:EEED:ECEB
 *   -  dip6  = 2000:EEED:ECEB:EAE9:E8E7:E6E5:E4E3:E2E1
 *   -  type  = bcmTunnelTypeIp6In6/bcmTunnelTypeGre6In6;
 *      sip   = 2000:0000:0000:0000:0200:77FF:EEED:ECEB
 *      dip   = 2000:1111:2222:3333:4444:5555:6666:8888
 *   -  Vlan  = 10
 *   -  Dmac  = 00:0C:00:02:00:00
 *   -  Smac  = 00:00:07:00:01:00
 *
 * Tunnnel 3:
 *   -  sip6  = 2000:0000:0000:0000:0400:77FF:DEDD:DCDB
 *   -  dip6  = 2000:DEDD:DCDB:DAD9:D8D7:D6D5:D4D3:D2D1
 *   -  type  = bcmTunnelTypeIpAnyIn6/bcmTunnelTypeGreAnyIn6;
 *      sip   = 192.128.1.6
 *      dip   = 1.0.0.49
 *   -  Vlan  = 10
 *   -  Dmac  = 00:0C:00:02:00:00
 *   -  Smac  = 00:00:07:00:01:00
 */

/*
 * Service init for IPv6 tunnel termination scenarios in Jericho 1 and 2.
 * Inputs:   unit              - relevant unit;
 *              core_port      - core port;
 *              access_port   - access port ;
 *              nof_tunnels   - number of tunnels;
 *              tunnel_type1 - tunnel type for tunnel 1;
 *              tunnel_type2 - tunnel type for tunnel 2;
 *              tunnel_type3 - tunnel type for tunnel 3;
 *              is_cascade    - Indicates cascade termination lookup;
 *              is_subnet      - Indicates subnet;
 */
int ipv6_tunnel_term_service_init (int unit, int core_port,int access_port, 
    int nof_tunnels, int tunnel_type1, int tunnel_type2, int tunnel_type3,
    int is_cascade, int is_subnet) {
    int rv = 0;
    int ipv6_tunnel_term_mode;

    ip_tunnel_basic_info.nof_tunnels = nof_tunnels;
    
    ip_tunnel_basic_info.tunnel_type[0] = tunnel_type1;
    ip_tunnel_basic_info.tunnel_type[1] = tunnel_type2;
    ip_tunnel_basic_info.tunnel_type[2] = tunnel_type3;
    
    ip_tunnel_basic_info.v6_cascade_lookups[0] = is_cascade;
    ip_tunnel_basic_info.v6_cascade_lookups[1] = is_cascade;
    ip_tunnel_basic_info.v6_cascade_lookups[2] = is_cascade;

    if (is_subnet) {
        /* For Jericho:
                *   SIP subnet support for cascade lookup mode. 
                *   DIP subnet support for non-cascade lookup mode.
                * For Jercho2:
                *   Only DIP subnet support for non-cascade lookup mode.
                */
        if (!is_cascade) {            
            sal_memcpy(ip_tunnel_basic_info.dip6_mask[0], ip_tunnel_basic_info.v6_subnet_mask[0], 16);
            sal_memcpy(ip_tunnel_basic_info.dip6_mask[1], ip_tunnel_basic_info.v6_subnet_mask[1], 16);
            sal_memcpy(ip_tunnel_basic_info.dip6_mask[2], ip_tunnel_basic_info.v6_subnet_mask[2], 16);
        } else {
            if (!is_device_or_above(unit, JERICHO2)) {
                sal_memcpy(ip_tunnel_basic_info.sip6_mask[0], ip_tunnel_basic_info.v6_subnet_mask[0], 16);
                sal_memcpy(ip_tunnel_basic_info.sip6_mask[1], ip_tunnel_basic_info.v6_subnet_mask[1], 16);
                sal_memcpy(ip_tunnel_basic_info.sip6_mask[2], ip_tunnel_basic_info.v6_subnet_mask[2], 16);
            }            
        } 
    }

    if (!is_device_or_above(unit, JERICHO2)) {
        ipv6_tunnel_term_mode = soc_property_get(unit, "bcm886xx_ip6_tunnel_termination_mode", 1);

        if (ipv6_tunnel_term_mode == 2) { 
            /* Configure the port to support the IPv6 tunnel cascade lookup: DIP+SIP */
            rv = bcm_port_control_set(unit, core_port, bcmPortControlIPv6TerminationCascadedModeEnable, 1); 
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_port_control_set, in_port=%d, \n", in_port);
            }
            rv = bcm_port_class_set(unit, core_port, bcmPortClassFieldIngressTunnelTerminated, ip_tunnel_basic_info.port_property); 
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_port_class_set, in_port=%d, \n", in_port);
            }
        }
    }

    return rv;
}

/*
 * Service init for IPv6 tunnel termination scenarios
 * Inputs:   unit              - relevant unit;
 *              core_port      - core port;
 *              access_port   - access port ;
 *              nof_tunnels   - number of tunnels;
 */
int ipv6_tunnel_term_same_dip_vrf_service_init (int unit, int core_port,int access_port, int nof_tunnels) {
    int rv = 0;
    int ipv6_tunnel_term_mode;

    ip_tunnel_basic_info.nof_tunnels = nof_tunnels;  
    ip_tunnel_basic_info.tunnel_type[0] = bcmTunnelTypeIpAnyIn6;
    ip_tunnel_basic_info.tunnel_type[1] = bcmTunnelTypeIpAnyIn6;
    
    ip_tunnel_basic_info.v6_cascade_lookups[0] = 0;
    ip_tunnel_basic_info.v6_cascade_lookups[1] = 1;
    if (is_device_or_above(unit, JERICHO2)) {
        ip_tunnel_basic_info.v6_cascade_lookups[1] = 2;
    }

    /* Set the DIP1 use the same value as DIP0 */
    sal_memcpy(ip_tunnel_basic_info.dip6[1], ip_tunnel_basic_info.dip6[0], 16);

    if (!is_device_or_above(unit, JERICHO2)) {
        ipv6_tunnel_term_mode = soc_property_get(unit, "bcm886xx_ip6_tunnel_termination_mode", 1);

        if (ipv6_tunnel_term_mode == 2) { 
            /* Configure the port to support the IPv6 tunnel cascade lookup: DIP+SIP */
            rv = bcm_port_control_set(unit, core_port, bcmPortControlIPv6TerminationCascadedModeEnable, 1); 
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_port_control_set, in_port=%d, \n", in_port);
            }
            rv = bcm_port_class_set(unit, core_port, bcmPortClassFieldIngressTunnelTerminated, ip_tunnel_basic_info.port_property); 
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_port_class_set, in_port=%d, \n", in_port);
            }
        }
    }

    return rv;
}

/*
 * Function that  add an IPv6/IPv4 Route entry
 * Inputs: unit - relevant unit;
 * Inputs: tunnel_idx - tunnel index;
 * If it is bcmTunnelTypeIp4In6 and bcmTunnelTypeGre4In6, Add IPv4 route for this tunnel.
 * If it is bcmTunnelTypeIp6In6 and bcmTunnelTypeGre6In6, Add IPv6 route for this tunnel.
 * else, Add IPv4/IPv6 route for this tunnel.
 */
int
ipv6_tunnel_term_add_route(
    int unit,
    int tunnel_idx)
{
    int rv;

    if ((ip_tunnel_basic_info.tunnel_type[tunnel_idx] == bcmTunnelTypeIp4In6) || 
        (ip_tunnel_basic_info.tunnel_type[tunnel_idx] == bcmTunnelTypeIpAnyIn6) ||
        (ip_tunnel_basic_info.tunnel_type[tunnel_idx] == bcmTunnelTypeGre4In6) ||
        (ip_tunnel_basic_info.tunnel_type[tunnel_idx] == bcmTunnelTypeGreAnyIn6)) 
    {
        /** Add IPv4 routes to IPv6 tunnel*/
        rv = add_route_ipv4(unit, ip_tunnel_basic_info.term_host_dip[tunnel_idx],
                            ip_tunnel_basic_info.term_route_dip_mask,
                            ip_tunnel_basic_info.tunnel_vrf[tunnel_idx], ip_tunnel_basic_info.term_fec_id[tunnel_idx]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, add_route_ipv4 \n");
            return rv;
        }
    }     

    if ((ip_tunnel_basic_info.tunnel_type[tunnel_idx] == bcmTunnelTypeIp6In6) || 
        (ip_tunnel_basic_info.tunnel_type[tunnel_idx] == bcmTunnelTypeIpAnyIn6) ||
        (ip_tunnel_basic_info.tunnel_type[tunnel_idx] == bcmTunnelTypeGre6In6) ||
        (ip_tunnel_basic_info.tunnel_type[tunnel_idx] == bcmTunnelTypeGreAnyIn6)) 
    {
        /** Add IPv6 routes to IPv6 tunnel*/
        rv = add_route_ipv6(unit, ip_tunnel_basic_info.term_host_dip6[tunnel_idx],
                            ip_tunnel_basic_info.term_route_dip6_mask,
                            ip_tunnel_basic_info.tunnel_vrf[tunnel_idx], ip_tunnel_basic_info.term_fec_id[tunnel_idx]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, add_route_ipv6 \n");
            return rv;
        }
    } 

    return rv;
}

/*
 * Function that do the IPv6 tunnel termination according to the tunnel type.
 * Inputs: unit - relevant unit;
 * Inputs: access_port - access port;
 * Inputs: core_port - core port;
 * Inputs: nof_tunnels - number of tunnels;
 */
int
ipv6_tunnel_term_tunnels_example(
    int unit,
    int provider_port,
    int access_port,
    int nof_tunnels)
{
    int rv = BCM_E_NONE;
    int device_is_jericho2;
    int tunnel_idx;

    if (is_device_or_above(unit,JERICHO2)) {
        ip_tunnel_basic_info.term_arp_id[0] = *dnxc_data_get(unit, "l3", "rif", "nof_rifs", NULL) + 0xb8a;
        ip_tunnel_basic_info.term_arp_id[1] = *dnxc_data_get(unit, "l3", "rif", "nof_rifs", NULL) + 0xb94;
        ip_tunnel_basic_info.term_arp_id[2] = *dnxc_data_get(unit, "l3", "rif", "nof_rifs", NULL) + 0xb9E;
    }

    ip_tunnel_basic_info.nof_tunnels = nof_tunnels;
    ip_tunnel_basic_info.access_port = access_port;
    ip_tunnel_basic_info.provider_port = provider_port;

    rv = ip_tunnel_fec_arp_map(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, ip_tunnel_fec_arp_map \n");
        return rv;
    }
    
    /* Open route interfaces */
    rv = ip_tunnel_term_open_route_interfaces(unit, access_port, provider_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, ip_tunnel_term_open_route_interfaces \n");
        return rv;
    }    
    
    for (tunnel_idx = 0; tunnel_idx < ip_tunnel_basic_info.nof_tunnels; tunnel_idx++)
    {
        /** Create two tunnel termination lookups and the tunnel termination inlifs*/
        rv = ip_tunnel_term_create(unit, tunnel_idx);
        if (rv != BCM_E_NONE)
        {
            printf("Error, ipv6_tunnel_term_create, tunnel #1 \n");
            return rv;
        }
        
        rv = ip_tunnel_term_create_arp(unit, tunnel_idx);
        if (rv != BCM_E_NONE)
        {
            printf("Error, ipv6_tunnel_term_create_arp\n");
            return rv;
        }

        rv = ip_tunnel_term_create_fec(unit, tunnel_idx);
        if (rv != BCM_E_NONE)
        {
            printf("Error, ipv6_tunnel_term_create_fec\n");
            return rv;
        }        

        rv = ipv6_tunnel_term_add_route(unit, tunnel_idx);
        if (rv != BCM_E_NONE)
        {
            printf("Error, ipv6_tunnel_terminate_add_route \n");
            return rv;
        }
    }

    return rv;
}


int
ipv6_tunnel_term_ipany_greany(
    int unit,
    int provider_port,
    int access_port)
{
    int rv;

    ip_tunnel_basic_info.tunnel_type[0] = bcmTunnelTypeIpAnyIn6;
    ip_tunnel_basic_info.tunnel_type[1] = bcmTunnelTypeGreAnyIn6;
    rv = ipv6_tunnel_term_tunnels_example(unit, provider_port, access_port, 2);
    if (rv != BCM_E_NONE)
    {
        printf("ipv6_tunnel_term_tunnels_example \n");
        return rv;
    }    

    return rv;
}

