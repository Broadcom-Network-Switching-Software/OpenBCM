/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_sand_ip_tunnel_term.c Purpose: Various examples for IPv6 Tunnels.
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
 * cint ../../../../src/examples/sand/cint_sand_ip_tunnel_term.c
 * cint
 * int unit = 0;
 * int rv = 0;
 * int provider_port = 200;
 * int access_port   = 201;
 * int nof_tunnels = 2;
 * rv = ip_tunnel_term_sip_dip_ip4_ip6_ipany_in4($unit,$provider_port,$access_port);
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
 *   -  sip  = 160.15.16.17
 *   -  dip  = 161.15.16.17
 *   -  type  = bcmTunnelTypeIp4In4/bcmTunnelTypeGre4In4;
 *      sip   = 192.128.1.4
 *      dip   = 1.0.0.17
 *   -  Vlan  = 10
 *   -  Dmac  = 00:0C:00:02:00:00
 *   -  Smac  = 00:00:07:00:01:00
 *
 * Tunnnel 2:
 *   -  sip  = 170.15.16.17
 *   -  dip  = 171.15.16.17
 *   -  type  = bcmTunnelTypeIp6In4/bcmTunnelTypeGre6In4;
 *      sip   = 2000:0000:0000:0000:0200:77FF:EEED:ECEB
 *      dip   = 2000:1111:2222:3333:4444:5555:6666:8888
 *   -  Vlan  = 10
 *   -  Dmac  = 00:0C:00:02:00:00
 *   -  Smac  = 00:00:07:00:01:00
 *
 * Tunnnel 3:
 *   -  sip  = 180.15.16.17
 *   -  dip  = 181.15.16.17
 *   -  type  = bcmTunnelTypeIpAnyIn4/bcmTunnelTypeGreAnyIn4;
 *      sip   = 192.128.1.6
 *      dip   = 5.0.0.17
 *   -  Vlan  = 10
 *   -  Dmac  = 00:0C:00:02:00:00
 *   -  Smac  = 00:00:07:00:01:00
 */

/*
 * Service init for IP tunnel termination scenarios:
 * Inputs:   unit              - relevant unit;
 *              provider_port - provider port;
 *              access_port   - access port ;
 *              nof_tunnels   - number of tunnels; 
 *              is_subnet      - Indicates subnet;
 *              tunnel_type1 - tunnel type for tunnel 1;
 *              tunnel_type2 - tunnel type for tunnel 2;
 *              tunnel_type3 - tunnel type for tunnel 3;
 */
int ip_tunnel_term_sip_dip_init (int unit, int provider_port, int access_port, 
    int nof_tunnels, int is_subnet,
    int tunnel_type1, int tunnel_type2, int tunnel_type3) 
 {
    int rv;
    
    ip_tunnel_basic_info.nof_tunnels = nof_tunnels;
    ip_tunnel_basic_info.access_port = access_port;
    ip_tunnel_basic_info.provider_port = provider_port;
    
    ip_tunnel_basic_info.tunnel_type[0] = tunnel_type1;
    ip_tunnel_basic_info.tunnel_type[1] = tunnel_type2;
    ip_tunnel_basic_info.tunnel_type[2] = tunnel_type3;    

    if (is_subnet) {
        ip_tunnel_basic_info.sip_mask[0] = ip_tunnel_basic_info.subnet_mask[0];
        ip_tunnel_basic_info.dip_mask[0] = ip_tunnel_basic_info.subnet_mask[0];
        ip_tunnel_basic_info.sip_mask[1] = ip_tunnel_basic_info.subnet_mask[1];
        ip_tunnel_basic_info.dip_mask[1] = ip_tunnel_basic_info.subnet_mask[1];
        ip_tunnel_basic_info.sip_mask[2] = ip_tunnel_basic_info.subnet_mask[2];
        ip_tunnel_basic_info.dip_mask[2] = ip_tunnel_basic_info.subnet_mask[2]; 
    }

    if(soc_property_get(unit , "bcm886xx_ip4_tunnel_termination_mode",3) == 4 ||
       soc_property_get(unit , "bcm886xx_ip4_tunnel_termination_mode",3) == 5 ||
       soc_property_get(unit , "bcm886xx_ip4_tunnel_termination_mode",3) == 6) {
        rv = bcm_port_class_set(unit, provider_port, bcmPortClassFieldIngressTunnelTerminated, ip_tunnel_basic_info.port_property); 
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_class_set, in_port=%d, \n", provider_port);
        }
    }

    return rv;
}

/*
 * Service init for IP tunnel termination scenarios:
 * Inputs:   unit              - relevant unit;
 *              provider_port - provider port;
 *              access_port   - access port ;
 *              nof_tunnels   - number of tunnels; 
 *              is_subnet      - Indicates subnet;
 *              tunnel_type1 - tunnel type for tunnel 1;
 *              tunnel_type2 - tunnel type for tunnel 2;
 *              tunnel_type3 - tunnel type for tunnel 3;
 */
int ip_tunnel_term_sip_dip_priority_init (int unit, int provider_port, int access_port, 
    int nof_tunnels, 
    int tunnel_type1, int tunnel_type2, int tunnel_type3) 
 {
    int rv;
	
    ip_tunnel_basic_info.nof_tunnels = nof_tunnels;
    ip_tunnel_basic_info.access_port = access_port;
    ip_tunnel_basic_info.provider_port = provider_port;
    
    ip_tunnel_basic_info.tunnel_type[0] = tunnel_type1;
    ip_tunnel_basic_info.tunnel_type[1] = tunnel_type2;
    ip_tunnel_basic_info.tunnel_type[2] = tunnel_type3;

    /* tunnel 1: DIP only */
    ip_tunnel_basic_info.sip_mask[0] = 0;
    /* tunnel 2: DIP, SIP */
    /* tunnel 3: DIP, SIP with mask */
    ip_tunnel_basic_info.dip_mask[2] = 0xFFFFFF00;
    ip_tunnel_basic_info.sip_mask[2] = 0xFFFFFF00;
	
    if(soc_property_get(unit , "bcm886xx_ip4_tunnel_termination_mode",3) == 4 ||
       soc_property_get(unit , "bcm886xx_ip4_tunnel_termination_mode",3) == 5 ||
       soc_property_get(unit , "bcm886xx_ip4_tunnel_termination_mode",3) == 6) {
        rv = bcm_port_class_set(unit, provider_port, bcmPortClassFieldIngressTunnelTerminated, ip_tunnel_basic_info.port_property); 
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_class_set, in_port=%d, \n", provider_port);
        }
    }
	
    return rv;
}

/*
 * Function that  add an IPv6/IPv4 Route entry for termination entry.
 * Inputs: unit - relevant unit;
 */
int
ip_tunnel_term_add_route(
    int unit,
    int tunnel_idx)
{
    int rv;

    rv = add_route_ipv4(unit, ip_tunnel_basic_info.term_host_dip[tunnel_idx],
                        ip_tunnel_basic_info.term_route_dip_mask,
                        ip_tunnel_basic_info.tunnel_vrf[tunnel_idx], ip_tunnel_basic_info.term_fec_id[tunnel_idx]);
    if (rv != BCM_E_NONE)
    {
        printf("Error, add_route_ipv4 \n");
        return rv;
    }

    rv = add_route_ipv6(unit, ip_tunnel_basic_info.term_host_dip6[tunnel_idx],
                        ip_tunnel_basic_info.term_route_dip6_mask,
                        ip_tunnel_basic_info.tunnel_vrf[tunnel_idx], ip_tunnel_basic_info.term_fec_id[tunnel_idx]);
    if (rv != BCM_E_NONE)
    {
        printf("Error, add_route_ipv6 \n");
        return rv;
    }

    return rv;
}

/*
 * Function that  add an IPv6/IPv4 Route entry for non termination entry.
 * Inputs: unit - relevant unit;
 */
int
ip_tunnel_term_add_no_term_route(
    int unit)
{
    int rv;

    rv = add_route_ipv4(unit, ip_tunnel_basic_info.no_term_route_dip,
                        ip_tunnel_basic_info.no_term_route_dip_mask,
                        ip_tunnel_basic_info.provider_eth_vrf, ip_tunnel_basic_info.term_fec_id[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error, add_route_ipv4 \n");
        return rv;
    }
    
    return rv;
}


/*
 * Function that do the IP tunnel termination according to the tunnel type.
 * Inputs: unit - relevant unit;
 * Inputs: access_port - access port;
 * Inputs: core_port - core port;
 */
int
ip_tunnel_term_tunnels_example(
    int unit,   
    int provider_port,
    int access_port)
{
    int rv = BCM_E_NONE;
    int tunnel_idx;
    char *proc_name;
    sand_utils_l3_fec_s fec_structure;

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
            printf("Error, ip_tunnel_term_create, tunnel:%d \n", tunnel_idx);
            return rv;
        }
        
        rv = ip_tunnel_term_create_arp(unit, tunnel_idx);
        if (rv != BCM_E_NONE)
        {
            printf("Error, ip_tunnel_term_create_arp:%d \n", tunnel_idx);
            return rv;
        }

        rv = ip_tunnel_term_create_fec(unit, tunnel_idx);
        if (rv != BCM_E_NONE)
        {
            printf("Error, ip_tunnel_term_create_fec:%d \n", tunnel_idx);
            return rv;
        }        

        rv = ip_tunnel_term_add_route(unit, tunnel_idx);
        if (rv != BCM_E_NONE)
        {
            printf("Error, ip_tunnel_term_add_route:%d \n", tunnel_idx);
            return rv;
        }
    }

    rv = ip_tunnel_term_add_no_term_route(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, ip_tunnel_term_add_route:%d \n");
        return rv;
    }

    return rv;
}

int ip_tunnel_term_sip_dip_ip4_ip6_ipany_in4 (int unit, int provider_port, int access_port) {
    ip_tunnel_term_sip_dip_init(unit, provider_port, access_port, 3, 0, bcmTunnelTypeIp4In4, bcmTunnelTypeIp6In4, bcmTunnelTypeIpAnyIn4);
    return ip_tunnel_term_tunnels_example(unit, provider_port, access_port);
}

int ip_tunnel_term_sip_dip_gre4_gre6_greany_in4 (int unit, int provider_port, int access_port) {
    ip_tunnel_term_sip_dip_init(unit, provider_port, access_port, 3, 0, bcmTunnelTypeGre4In4, bcmTunnelTypeGre6In4, bcmTunnelTypeGreAnyIn4);

    return ip_tunnel_term_tunnels_example(unit, provider_port, access_port);
}

int ip_tunnel_term_sip_dip_ip4_ip6_ipany_in4_subnet (int unit, int provider_port, int access_port) {
    ip_tunnel_term_sip_dip_init(unit, provider_port, access_port, 3, 1, bcmTunnelTypeIp4In4, bcmTunnelTypeIp6In4, bcmTunnelTypeIpAnyIn4);

    return ip_tunnel_term_tunnels_example(unit, provider_port, access_port);
}

int ip_tunnel_term_sip_dip_gre4_gre6_greany_in4_subnet (int unit, int provider_port, int access_port) {    
    ip_tunnel_term_sip_dip_init(unit, provider_port, access_port, 3, 1, bcmTunnelTypeGre4In4, bcmTunnelTypeGre6In4, bcmTunnelTypeGreAnyIn4);

    return ip_tunnel_term_tunnels_example(unit, provider_port, access_port);
}

int ip_tunnel_term_sip_dip_ip4_ip6_ipany_in4_priority (int unit, int provider_port, int access_port) {
    ip_tunnel_term_sip_dip_priority_init(unit, provider_port, access_port, 3, 
        bcmTunnelTypeIp4In4, bcmTunnelTypeIp6In4, bcmTunnelTypeIpAnyIn4);

    return ip_tunnel_term_tunnels_example(unit, provider_port, access_port);
}

int ip_tunnel_term_sip_dip_gre4_gre6_greany_in4_priority (int unit, int provider_port, int access_port) {
    ip_tunnel_term_sip_dip_priority_init(unit, provider_port, access_port, 3, 
        bcmTunnelTypeGre4In4, bcmTunnelTypeGre6In4, bcmTunnelTypeGreAnyIn4);

    return ip_tunnel_term_tunnels_example(unit, provider_port, access_port);
}


