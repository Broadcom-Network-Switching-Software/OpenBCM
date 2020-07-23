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
 * cint cint_sand_ip_tunnel_encap.c
 * cint
 * print ip_tunnel_encap_ipany_greany(<unit>, <access_port>, <provider_port>); 
 
 *  
 * creating ipv6 tunnel no. 1 (host) - from <access_port> run packet: 
 *      ethernet header with DA 00:0c:00:02:00:00, SA 00:00:07:00:01:00 and Vlan tag id 2
 *      ipv4 header with DIP 127.255.255.3 and SIP 1.2.3.4
 * the packet will arrive at <provider_port> with: 
 *      ethernet header with DA 00:00:00:00:CD:1D, SA 00:0C:00:02:00:00 and Vlan tag id 100
 *      ipv6 header with DIP 161.15.16.17 and SIP 160.15.16.17 and hop limit 60
 *      ipv4 header with DIP 127.255.255.3 and SIP 1.2.3.4, ttl decremented
 *
 * creating ipv6 tunnel no. 2 (route) - from <access_port> run packet: 
 *      ethernet header with DA 00:0c:00:02:00:00, SA 00:00:07:00:01:00 and Vlan tag id 2
 *      ipv4 header with DIP 127.255.255.0 and SIP 1.2.3.4
 * the packet will arrive at <provider_port> with: 
 *      ethernet header with DA 20:00:00:00:CD:1D, SA 00:0C:00:02:00:00 and Vlan tag id 100
 *      ipv6 header with DIP 171.15.16.17 and SIP 170.15.16.17 and hop limit 60 and hop limit 50
 *      ipv4 header with DIP 127.255.255.5 and SIP 1.2.3.4, ttl decremented
 */

/*
 * Function that  add an IPv6/IPv4 Route entry
 * The IPv4/IPv6 host entry will be encapsulated as tunnel-1
 * The IPv4/IPv6 route entry will be encapsulated as tunnel-2
 * Inputs: unit - relevant unit;
 */
int
ip_tunnel_encap_add_route(
    int unit)
{
    int rv;
    bcm_ip6_t ipv6_host_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    
    /** Add IPv4/IPv6 hosts to IPv6 tunnel encap-1 */
    rv = add_host_ipv4(unit, ip_tunnel_basic_info.encap_host_dip,
                        ip_tunnel_basic_info.access_eth_vrf, ip_tunnel_basic_info.encap_fec_id[0], 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, add_host_ipv4 \n");
        return rv;
    }     

    /* Use the IPv6 route API to add the IPv6 host */
    rv = add_route_ipv6(unit, ip_tunnel_basic_info.encap_host_dip6, ipv6_host_mask,
                        ip_tunnel_basic_info.access_eth_vrf, ip_tunnel_basic_info.encap_fec_id[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error, add_host_ipv6 \n");
        return rv;
    }

    /** Add IPv4/IPv6 routes to IPv6 tunnel encap-0 */
    rv = add_route_ipv4(unit, ip_tunnel_basic_info.encap_host_dip,
                        ip_tunnel_basic_info.encap_route_dip_mask,
                        ip_tunnel_basic_info.access_eth_vrf, ip_tunnel_basic_info.encap_fec_id[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error, add_route_ipv4 \n");
        return rv;
    }


    rv = add_route_ipv6(unit, ip_tunnel_basic_info.encap_host_dip6,
                        ip_tunnel_basic_info.encap_route_dip6_mask,
                        ip_tunnel_basic_info.access_eth_vrf, ip_tunnel_basic_info.encap_fec_id[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error, add_route_ipv6 \n");
        return rv;
    }

    return rv;
}

/*
 * Function that do the IPv4 tunnel encapsulation according to the tunnel type.
 * Inputs: unit - relevant unit;
 * Inputs: access_port - access port;
 * Inputs: core_port - core port;
 */
int
ip_tunnel_encap_tunnels_example(
    int unit,
    int access_port,    
    int provider_port)
{
    int rv = BCM_E_NONE;
    int tunnel_idx;

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
            printf("Error, ip_tunnel_encap_create_arp\n");
            return rv;
        }

        rv = ip_tunnel_encap_create(unit, tunnel_idx);
        if (rv != BCM_E_NONE)
        {
            printf("Error, ip_tunnel_encap_create, tunnel #1 \n");
            return rv;
        }

        rv = ip_tunnel_encap_create_fec(unit, tunnel_idx);
        if (rv != BCM_E_NONE)
        {
            printf("Error, ip_tunnel_encap_create_fec\n");
            return rv;
        }        
    }
    
    rv = ip_tunnel_encap_add_route(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, ip_tunnel_encap_add_route \n");
        return rv;
    }

    return rv;
}

int ip_tunnel_encap_ipany_greany(int unit, int access_port, int provider_port){
    int rv;

    ip_tunnel_basic_info.tunnel_type[0] = bcmTunnelTypeIpAnyIn4; 
    ip_tunnel_basic_info.dscp[0] = 20;
    ip_tunnel_basic_info.ttl[0] = 64;

    ip_tunnel_basic_info.tunnel_type[1] = bcmTunnelTypeGreAnyIn4;
    ip_tunnel_basic_info.dscp[1] = 30;
    ip_tunnel_basic_info.ttl[1] =  40;

    rv = ip_tunnel_encap_tunnels_example(unit, access_port, provider_port);
    if (rv != BCM_E_NONE) {
        printf("Error, ip_tunnel_encap_tunnels_example \n");
    }

    return rv;
}

int ip_tunnel_encap_dscp_ttl_ipany_greany(int unit, int access_port, int provider_port){
    int rv;

    ip_tunnel_basic_info.nof_tunnels = 2;
    ip_tunnel_basic_info.tunnel_type[0] = bcmTunnelTypeIpAnyIn4;
    ip_tunnel_basic_info.dscp[0] = 5;
    ip_tunnel_basic_info.ttl[0] = 64;
    
    ip_tunnel_basic_info.tunnel_type[1] = bcmTunnelTypeGreAnyIn4;	
    ip_tunnel_basic_info.dscp[1] = 0;
    ip_tunnel_basic_info.dscp_sel[1] = bcmTunnelDscpPacket;
    ip_tunnel_basic_info.tunnel_flags[1] = BCM_TUNNEL_TERM_USE_OUTER_TTL;	
    ip_tunnel_basic_info.ttl[1] = 0;

    rv = ip_tunnel_encap_tunnels_example(unit, access_port, provider_port);
    if (rv != BCM_E_NONE) {
        printf("Error, ip_tunnel_encap_tunnels_example \n");
    }

    return rv;
}

