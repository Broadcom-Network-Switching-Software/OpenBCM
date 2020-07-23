/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/* 
 * how to run:
 * cd ../../../src/examples/sand
 * cint ../sand/utility/cint_sand_utils_global.c
 * cint utility/cint_sand_utils_l3.c
 * cint utility/cint_sand_utils_global.c
 * cint cint_ip_route_basic.c
 * cint cint_sand_ip_tunnel.c
 * cint cint_sand_ipv6_tunnel_encap.c
 * cint
 * print ipv6_tunnel_encap_tunnels_example(<unit>, <access_port>, <provider_port>, <nof_tunnels>); 
 *  
 * creating ipv6 tunnel no. 1 (host) - from <access_port> run packet: 
 *      ethernet header with DA 00:0c:00:02:00:00, SA 00:00:07:00:01:00 and Vlan tag id 2
 *      ipv4 header with DIP 127.255.255.3 and SIP 1.2.3.4
 * the packet will arrive at <provider_port> with: 
 *      ethernet header with DA 00:00:00:00:CD:1D, SA 00:0C:00:02:00:00 and Vlan tag id 100
 *      ipv6 header with DIP 2000:FEFD:FCFB:FAF9:F8F7:F6F5:F4F3:F2F1, SIP 2000:0000:0000:0000:0200:77FF:FEFD:FCFB and hop limit 60
 *      ipv4 header with DIP 127.255.255.3 and SIP 1.2.3.4, ttl decremented
 *
 * creating ipv6 tunnel no. 2 (route) - from <access_port> run packet: 
 *      ethernet header with DA 00:0c:00:02:00:00, SA 00:00:07:00:01:00 and Vlan tag id 2
 *      ipv4 header with DIP 127.255.255.0 and SIP 1.2.3.4
 * the packet will arrive at <provider_port> with: 
 *      ethernet header with DA 20:00:00:00:CD:1D, SA 00:0C:00:02:00:00 and Vlan tag id 100
 *      ipv6 header with DIP 2000:EEED:ECEB:EAE9:E8E7:E6E5:E4E3:E2E1, SIP 2000:0000:0000:0000:0300:77FF:EEED:ECEB and hop limit 50
 *      ipv4 header with DIP 127.255.255.0 and SIP 1.2.3.4, ttl decremented
*/ 

/*
 * Function that  add an IPv6/IPv4 Route entry
 * Inputs: unit - relevant unit;
 * Inputs: tunnel_idx - tunnel index;
 * If it is tunnel index 0, Add IPv4/IPv6 host for this tunnel.
 * else, Add IPv4 or IPv6 route for this tunnel.
 */
int
ipv6_tunnel_encap_add_route_basic(
    int unit,
    int tunnel_idx)
{
    int rv;

    if ((ip_tunnel_basic_info.tunnel_type[tunnel_idx] == bcmTunnelTypeIp4In6) || 
        (ip_tunnel_basic_info.tunnel_type[tunnel_idx] == bcmTunnelTypeIpAnyIn6) ||
        (ip_tunnel_basic_info.tunnel_type[tunnel_idx] == bcmTunnelTypeGre4In6) ||
        (ip_tunnel_basic_info.tunnel_type[tunnel_idx] == bcmTunnelTypeGreAnyIn6)) 
    {
        if (tunnel_idx == 0) {
            
            /** Add IPv4 hosts to IPv6 tunnel*/
            rv = add_host_ipv4(unit, ip_tunnel_basic_info.encap_host_dip,
                                ip_tunnel_basic_info.access_eth_vrf, ip_tunnel_basic_info.encap_fec_id[tunnel_idx], 0, 0);
            if (rv != BCM_E_NONE)
            {
                printf("Error, add_host_ipv4 \n");
                return rv;
            }            
         } else {
            /** Add IPv4 routes to IPv6 tunnel*/
            rv = add_route_ipv4(unit, ip_tunnel_basic_info.encap_host_dip,
                                ip_tunnel_basic_info.encap_route_dip_mask,
                                ip_tunnel_basic_info.access_eth_vrf, ip_tunnel_basic_info.encap_fec_id[tunnel_idx]);
            if (rv != BCM_E_NONE)
            {
                printf("Error, add_route_ipv4 \n");
                return rv;
            }
         }
    }     

    if ((ip_tunnel_basic_info.tunnel_type[tunnel_idx] == bcmTunnelTypeIp6In6) || 
        (ip_tunnel_basic_info.tunnel_type[tunnel_idx] == bcmTunnelTypeIpAnyIn6) ||
        (ip_tunnel_basic_info.tunnel_type[tunnel_idx] == bcmTunnelTypeGre6In6) ||
        (ip_tunnel_basic_info.tunnel_type[tunnel_idx] == bcmTunnelTypeGreAnyIn6)) 
    {
        if (tunnel_idx == 0) {
            /** Add IPv6 routes to IPv6 tunnel*/
            rv = add_host_ipv6(unit, ip_tunnel_basic_info.encap_host_dip6,
                                ip_tunnel_basic_info.access_eth_vrf, ip_tunnel_basic_info.encap_fec_id[tunnel_idx]);
            if (rv != BCM_E_NONE)
            {
                printf("Error, add_host_ipv6 \n");
                return rv;
            }
        } else {
            /** Add IPv6 routes to IPv6 tunnel*/
            rv = add_route_ipv6(unit, ip_tunnel_basic_info.encap_host_dip6,
                                ip_tunnel_basic_info.encap_route_dip6_mask,
                                ip_tunnel_basic_info.access_eth_vrf, ip_tunnel_basic_info.encap_fec_id[tunnel_idx]);
            if (rv != BCM_E_NONE)
            {
                printf("Error, add_route_ipv6 \n");
                return rv;
            }
        }
    } 

    return rv;
}


/*
 * Function that  add an IPv6/IPv4 Route entry
 * Inputs: unit - relevant unit;
 * Inputs: tunnel_idx - tunnel index;
 * If it is tunnel index 0, Add IPv4 host and IPv4 route for this tunnel.
 * else, Add IPv6 host and IPv6 route for this tunnel.
 */
int
ipv6_tunnel_encap_add_route_per_tunnel(
    int unit,
    int tunnel_idx)
{
    int rv;
    int device_is_jericho2;
    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, is_device_jericho2, device_is_jericho2\n");
        return rv;
    }

    if ((ip_tunnel_basic_info.tunnel_type[tunnel_idx] == bcmTunnelTypeIp4In6) || 
        (ip_tunnel_basic_info.tunnel_type[tunnel_idx] == bcmTunnelTypeIpAnyIn6) ||
        (ip_tunnel_basic_info.tunnel_type[tunnel_idx] == bcmTunnelTypeGre4In6) ||
        (ip_tunnel_basic_info.tunnel_type[tunnel_idx] == bcmTunnelTypeGreAnyIn6)) 
    {
        if (tunnel_idx == 0) {
            /** Add IPv4 hosts to IPv6 tunnel*/
            rv = add_host_ipv4(unit, ip_tunnel_basic_info.encap_host_dip,
                                ip_tunnel_basic_info.access_eth_vrf, ip_tunnel_basic_info.encap_fec_id[tunnel_idx], 0, 0);
            if (rv != BCM_E_NONE)
            {
                printf("Error, add_host_ipv4 \n");
                return rv;
            }            
            /** Add IPv4 routes to IPv6 tunnel*/
            rv = add_route_ipv4(unit, ip_tunnel_basic_info.encap_host_dip,
                                ip_tunnel_basic_info.encap_route_dip_mask,
                                ip_tunnel_basic_info.access_eth_vrf, ip_tunnel_basic_info.encap_fec_id[tunnel_idx]);
            if (rv != BCM_E_NONE)
            {
                printf("Error, add_route_ipv4 \n");
                return rv;
            }
        }
    }     

    if ((ip_tunnel_basic_info.tunnel_type[tunnel_idx] == bcmTunnelTypeIp6In6) || 
        (ip_tunnel_basic_info.tunnel_type[tunnel_idx] == bcmTunnelTypeIpAnyIn6) ||
        (ip_tunnel_basic_info.tunnel_type[tunnel_idx] == bcmTunnelTypeGre6In6) ||
        (ip_tunnel_basic_info.tunnel_type[tunnel_idx] == bcmTunnelTypeGreAnyIn6)) 
    {
        if (tunnel_idx == 1) {
            /** Add IPv6 routes to IPv6 tunnel*/
            if (!device_is_jericho2) {
                rv = add_ipv6_host(unit, ip_tunnel_basic_info.encap_host_dip6, ip_tunnel_basic_info.access_eth_vrf, 
                                    ip_tunnel_basic_info.encap_fec_id[tunnel_idx], ip_tunnel_basic_info.encap_arp_id[tunnel_idx]);
                
                if (rv != BCM_E_NONE)
                {
                    printf("Error, add_host_ipv6 \n");
                    return rv;
                }
            } else {
                rv = add_host_ipv6(unit, ip_tunnel_basic_info.encap_host_dip6,
                                    ip_tunnel_basic_info.access_eth_vrf, ip_tunnel_basic_info.encap_fec_id[tunnel_idx]);
                if (rv != BCM_E_NONE)
                {
                    printf("Error, add_host_ipv6 \n");
                    return rv;
                }
            }
            /** Add IPv6 routes to IPv6 tunnel*/
            rv = add_route_ipv6(unit, ip_tunnel_basic_info.encap_host_dip6,
                                ip_tunnel_basic_info.encap_route_dip6_mask,
                                ip_tunnel_basic_info.access_eth_vrf, ip_tunnel_basic_info.encap_fec_id[tunnel_idx]);
            if (rv != BCM_E_NONE)
            {
                printf("Error, add_route_ipv6 \n");
                return rv;
            }
        }
    } 

    return rv;
}

/*
 * Function that  add an IPv6/IPv4 Route entry
 * Inputs: unit - relevant unit;
 * Inputs: tunnel_idx - tunnel index;
 */
int
ipv6_tunnel_encap_add_route(
    int unit,
    int tunnel_idx)
{
    if (!ip_tunnel_basic_info.ipv6_host_per_tunnel) {
        return ipv6_tunnel_encap_add_route_basic(unit, tunnel_idx);
    } else {
        return ipv6_tunnel_encap_add_route_per_tunnel(unit, tunnel_idx);
    }
}

/*
 * Function that do the IPv6 tunnel encapsulation according to the tunnel type.
 * Inputs: unit - relevant unit;
 * Inputs: access_port - access port;
 * Inputs: provider_port - provider port;
 * Inputs: nof_tunnels - number of tunnels;
 */
int
ipv6_tunnel_encap_tunnels_example(
    int unit,
    int access_port,    
    int provider_port,
    int nof_tunnels)
{
    int rv = BCM_E_NONE;
    int tunnel_idx;

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
    rv = ip_tunnel_encap_open_route_interfaces(unit, access_port, provider_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, ip_tunnel_encap_open_route_interfaces \n");
        return rv;
    }   
    
    for (tunnel_idx = 0; tunnel_idx < ip_tunnel_basic_info.nof_tunnels; tunnel_idx++)
    {
        rv = ip_tunnel_encap_create_arp(unit, tunnel_idx);
        if (rv != BCM_E_NONE)
        {
            printf("Error, ipv6_tunnel_encap_create_arp\n");
            return rv;
        }

        rv = ip_tunnel_encap_create(unit, tunnel_idx);
        if (rv != BCM_E_NONE)
        {
            printf("Error, ipv6_tunnel_encap_create, tunnel #1 \n");
            return rv;
        }

        rv = ip_tunnel_encap_create_fec(unit, tunnel_idx);
        if (rv != BCM_E_NONE)
        {
            printf("Error, ipv6_tunnel_encap_create_fec\n");
            return rv;
        }
        
        rv = ipv6_tunnel_encap_add_route(unit, tunnel_idx);
        if (rv != BCM_E_NONE)
        {
            printf("Error, ipv6_tunnel_terminate_add_route \n");
            return rv;
        }
    }

    return rv;
}

/*
 * Function that do the IPv6 tunnel encapsulation according to the tunnel type.
 * Inputs: unit - relevant unit;
 * Inputs: access_port - access port;
 * Inputs: core_port - core port;
 * Inputs: nof_tunnels - number of tunnels;
 */
int
ipv6_tunnel_encap_tunnels_ipany_ipany_in6(
    int unit,
    int access_port,    
    int core_port,
    int nof_tunnels)
{
    ip_tunnel_basic_info.tunnel_type[0] = bcmTunnelTypeIpAnyIn6;
    ip_tunnel_basic_info.tunnel_type[1] = bcmTunnelTypeIpAnyIn6;

    return ipv6_tunnel_encap_tunnels_example(unit, access_port, core_port, nof_tunnels);
}


/*
 * Function that do the IPv6 tunnel encapsulation according to the tunnel type.
 * Inputs: unit - relevant unit;
 * Inputs: access_port - access port;
 * Inputs: core_port - core port;
 * Inputs: nof_tunnels - number of tunnels;
 */
int
ipv6_tunnel_encap_tunnels_ip4_ip6_in6(
    int unit,
    int access_port,    
    int core_port,
    int nof_tunnels)
{
    ip_tunnel_basic_info.tunnel_type[0] = bcmTunnelTypeIp4In6;
    ip_tunnel_basic_info.tunnel_type[1] = bcmTunnelTypeIp6In6;
    ip_tunnel_basic_info.ipv6_host_per_tunnel = 1;
    ip_tunnel_basic_info.ttl[0]=64;
    ip_tunnel_basic_info.ttl[1]=40;

    return ipv6_tunnel_encap_tunnels_example(unit, access_port, core_port, nof_tunnels);
}

/*
 * Function that do the IPv6 tunnel encapsulation according to the tunnel type.
 * Inputs: unit - relevant unit;
 * Inputs: access_port - access port;
 * Inputs: core_port - core port;
 * Inputs: nof_tunnels - number of tunnels;
 */
int
ipv6_tunnel_encap_tunnels_gre4_gre6_in6(
    int unit,
    int access_port,    
    int core_port,
    int nof_tunnels)
{
    ip_tunnel_basic_info.tunnel_type[0] = bcmTunnelTypeGreAnyIn6;
    ip_tunnel_basic_info.tunnel_type[1] = bcmTunnelTypeGreAnyIn6;
    ip_tunnel_basic_info.ipv6_host_per_tunnel = 1;
    ip_tunnel_basic_info.ttl[0]=64;
    ip_tunnel_basic_info.ttl[1]=40;

    return ipv6_tunnel_encap_tunnels_example(unit, access_port, core_port, nof_tunnels);
}

/*
 * Function that clean the IPv6 tunnel encapsulation tunnels.
 * Inputs: unit - relevant unit;
 */
int
ipv6_tunnel_encap_tunnels_example_cleanup(
    int unit)
{
    int rv;
    int tunnel_idx;
    bcm_l3_intf_t l3_intf;

    for (tunnel_idx = 0; tunnel_idx < ip_tunnel_basic_info.nof_tunnels; tunnel_idx++) {
        bcm_l3_intf_t_init(&l3_intf);
        l3_intf.l3a_intf_id = ip_tunnel_basic_info.encap_tunnel_intf[tunnel_idx];

        rv = bcm_tunnel_initiator_clear(unit, &l3_intf);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_tunnel_initiator_clear");
        }
    }

    return rv;
}


