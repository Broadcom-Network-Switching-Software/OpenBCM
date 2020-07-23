/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * Example of basic VXLAN configuration
 *
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_vlan.c 
 * cint ../../src/examples/sand/utility/cint_sand_utils_multicast.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_vxlan.c 
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c 
 * cint ../../src/examples/dpp/cint_port_tpid.c
 * cint ../../src/examples/dpp/cint_advanced_vlan_translation_mode.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_sand_ip_tunnel.c
 * cint ../../src/examples/sand/cint_sand_field_fec_id_align_wa.c
 * cint ../../src/examples/sand/cint_sand_vxlan.c
 * cint ../../src/examples/sand/cint_sand_ipv6_vxlan.c
 * cint ../../src/examples/sand/cint_field_ingress_large_termination.c
 *
 * cint
 * int unit = 0;
 * int rv = 0;
 * int access_port = 200;
 * int core_port = 203;
 * rv = vxlan_roo_basic(unit, access_port, core_port);
 * print rv;
 *
 * Traffic:
 *
 *  Scenarios configured in this cint:
 *  Routing out of overlay:
 *
 *  ###############################################################################################################
 *  Sending packet from port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *      0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
 *   |  ----------------------------------------------------
 *   |  |                                 DA  (00:0C:00:02:00:00)                           |
 *   |  |-----------------------------------------------------------------------------------|    LL Ethernet
 *   |  |               DA                   |              SA  (00:00:07:00:01:00)         |
 *   |  |-----------------------------------------------------------------------------------|       Header
 *   |  |                                 SA                                                |
 *   |  |-----------------------------------------------------------------------------------|     ( 14 bytes)
 *   |  |       Ethertype = 0x0800(IPv4)     |                                              |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |     Version  |  IHL   |   ToS      |          Total Lenght                        |
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |        Identification              |   Flags   |  Fragment Offset                 |   Tunnel   IPv4
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |         TTL       |Protocol=17(UDP)|             Header Checksum                  |       Header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Source address ( 172.0.0.1 )                         |      (20 bytes)
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Destination address (171.0.0.1)                      |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |    Source port (0x5555)            |    Destination port (0x5555)                 |   UDP  Header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |    UDP lenght                      |    UDP checksum                              |    (8 bytes)
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  | | R | R |  Ver  | I | P | R | O |  |    Reserved      | Next Protocol             |  VXLAN  header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |    VXLAN Network Identifier (VNI) = 0x5000            | Reserved                  |    (8 bytes)
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |                               DA  (00:00:00:00:00:f1)                             |
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |               DA                   |              SA (00:0C:00:02:00:01)          |    Native
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                                 SA                                                |   Ethernet
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |          TPID = 0x8100(VLAN)       |         |           VID=30                   |    Header
 *   |  |-----------------------------------------------------------------------------------|  (18 bytes)
 *   |  |       Ethertype = 0x0800(IPv4)     |                                              |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |     Version  |  IHL   |   ToS      |          Total Lenght                        |
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |        Identification              |   Flags   |  Fragment Offset                 |        IPv4
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |         TTL       |Protocol=17(UDP)|             Header Checksum                  |       Header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Source address ( 127.255.255.01 )                    |      (20 bytes)
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Destination address (127.255.255.03)                 |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  ------------------------------------------------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Receiving packet on port == 203:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                               DA   ( 00:00:00:00:CD:1F)                           | 
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |               DA                    |             SA   (00:0F:00:02:0A:44)        |
 *   |  |-----------------------------------------------------------------------------------|   Ethernet
 *   |  |                                 SA                                                |
 *   |  |-----------------------------------------------------------------------------------|    Header
 *   |  |          TPID = 0x8100(VLAN)        |         |           VID=20                  |  ( 18 bytes )
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |       Ethertype = 0x0800(IPv4)       |                                            |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |     Version  |  IHL   |   ToS      |          Total Lenght                        |
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |        Identification              |   Flags   |  Fragment Offset                 |        IPv4
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |         TTL       |Protocol=17(UDP)|             Header Checksum                  |       Header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Source address ( 127.255.255.01 )                    |      (20 bytes)
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Destination address (127.255.255.03)                 |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *
 *  Routing into Overlay:
 *
 *  Sending packet on port == 200: ------------>
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                               DA   ( 00:0F:00:02:0A:44)                           | 
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |               DA                    |             SA   (00:00:00:00:CD:1F)        |
 *   |  |-----------------------------------------------------------------------------------|   Ethernet
 *   |  |                                 SA                                                |
 *   |  |-----------------------------------------------------------------------------------|    Header
 *   |  |          TPID = 0x8100(VLAN)        |         |           VID=20                  |  ( 18 bytes )
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |       Ethertype = 0x0800(IPv4)      |                                             |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |     Version  |  IHL   |   ToS      |          Total Lenght                        |
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |        Identification              |   Flags   |  Fragment Offset                 |        IPv4
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |         TTL       |Protocol=17(UDP)|             Header Checksum                  |       Header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Source address ( 127.255.255.01 )                    |      (20 bytes)
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Destination address (127.255.255.03)                 |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Receiving packet on port = 203 <----------------
 *   |  ----------------------------------------------------
 *   |  |                                 DA  (00:0C:00:02:00:00)                           |
 *   |  |-----------------------------------------------------------------------------------|    LL Ethernet
 *   |  |               DA                   |              SA  (00:00:07:00:01:00)         |
 *   |  |-----------------------------------------------------------------------------------|       Header
 *   |  |                                 SA                                                |
 *   |  |-----------------------------------------------------------------------------------|     ( 14 bytes)
 *   |  |       Ethertype = 0x0800(IPv4)     |                                              |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |     Version  |  IHL   |   ToS      |          Total Lenght                        |
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |        Identification              |   Flags   |  Fragment Offset                 |   Tunnel   IPv4
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |         TTL       |Protocol=17(UDP)|             Header Checksum                  |       Header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Source address ( 172.0.0.1 )                         |      (20 bytes)
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Destination address (171.0.0.1)                      |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |    Source port (0x5555)            |    Destination port (0x5555)                 |   UDP  Header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |    UDP lenght                      |    UDP checksum                              |    (8 bytes)
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  | | R | R |  Ver  | I | P | R | O |  |    Reserved      | Next Protocol             |  VXLAN  header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |    VXLAN Network Identifier (VNI) = 0x5000            | Reserved                  |    (8 bytes)
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |                               DA  (00:00:00:00:00:f1)                             |
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |               DA                   |              SA (00:0C:00:02:00:01)          |    Native
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                                 SA                                                |   Ethernet
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |          TPID = 0x8100(VLAN)       |         |           VID=30                   |    Header
 *   |  |-----------------------------------------------------------------------------------|  (18 bytes)
 *   |  |       Ethertype = 0x0800(IPv4)     |                                              |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |     Version  |  IHL   |   ToS      |          Total Lenght                        |
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |        Identification              |   Flags   |  Fragment Offset                 |        IPv4
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |         TTL       |Protocol=17(UDP)|             Header Checksum                  |       Header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Source address ( 127.255.255.01 )                    |      (20 bytes)
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Destination address (127.255.255.03)                 |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  ------------------------------------------------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Receiving packet on port == 203:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                               DA   ( 00:00:00:00:CD:1E)                           | 
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |               DA                    |             SA   (00:0F:00:02:0A:33)        |
 *   |  |-----------------------------------------------------------------------------------|   Ethernet
 *   |  |                                 SA                                                |
 *   |  |-----------------------------------------------------------------------------------|    Header
 *   |  |          TPID = 0x8100(VLAN)        |         |           VID=20                  |  ( 18 bytes )
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |       Ethertype = 0x0800(IPv4)       |                                            |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |     Version  |  IHL   |   ToS      |          Total Lenght                        |
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |        Identification              |   Flags   |  Fragment Offset                 |   Tunnel   IPv4
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |         TTL       |Protocol=17(UDP)|             Header Checksum                  |       Header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Source address ( 172.0.0.1 )                         |      (20 bytes)
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Destination address (171.0.0.1)                      |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |    Source port (0x5555)            |    Destination port (0x5555)                 |   UDP  Header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |    UDP lenght                      |    UDP checksum                              |    (8 bytes)
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  | | R | R |  Ver  | I | P | R | O |  |    Reserved      | Next Protocol             |  VXLAN  header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |    VXLAN Network Identifier (VNI) = 0x5000            | Reserved                  |    (8 bytes)
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |                               DA  (00:00:00:00:00:f1)                             |
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |               DA                   |              SA (00:0C:00:02:00:01)          |    Native
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                                 SA                                                |   Ethernet
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |          TPID = 0x8100(VLAN)       |         |           VID=30                   |    Header
 *   |  |-----------------------------------------------------------------------------------|  (18 bytes)
 *   |  |       Ethertype = 0x0800(IPv4)     |                                              |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |     Version  |  IHL   |   ToS      |          Total Lenght                        |
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |        Identification              |   Flags   |  Fragment Offset                 |        IPv4
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |         TTL       |Protocol=17(UDP)|             Header Checksum                  |       Header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Source address ( 127.255.255.01 )                    |      (20 bytes)
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Destination address (127.255.255.03)                 |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 * ##################################################################################################################
 */

void
vxlan6_basic_init(
    int unit,
    int access_port,
    int provider_port,
    int vdc_enable)
{
    int rv = BCM_E_NONE;
    int ipv6_tunnel_term_mode;

    rv = vxlan_basic_init(unit, access_port, provider_port, vdc_enable);
    if (rv != BCM_E_NONE) {
        printf("Error, vxlan_basic_init \n");
		return rv;
    }

    ip_tunnel_basic_info.tunnel_type[0] = bcmTunnelTypeVxlan6;

    if (!is_device_or_above(unit, JERICHO2)) {
        /* init the core port to support IPv6 tunnel cascade lookup and port property lookup, needed only for JER1*/
        ipv6_tunnel_term_mode = soc_property_get(unit, "bcm886xx_ip6_tunnel_termination_mode", 1);

        if (ipv6_tunnel_term_mode == 2) { 
            /* Configure the port to support the IPv6 tunnel cascade lookup: DIP+SIP */
            rv = bcm_port_control_set(unit, provider_port, bcmPortControlIPv6TerminationCascadedModeEnable, 1); 
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_port_control_set, in_port=%d, \n", provider_port);
            }
            rv = bcm_port_class_set(unit, provider_port, bcmPortClassFieldIngressTunnelTerminated, ip_tunnel_basic_info.port_property); 
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_port_class_set, in_port=%d, \n", provider_port);
            }
        }

        /* Use the larget termination to terminate the IPv6 VXLAN in ingress side. */
        rv = large_termination_example(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, large_termination_example\n");
            return rv;
        }
    }

    return rv;
}

/*
 * The basic VXLAN example with parameters:
 * Unit - relevant unit
 * access_port - port where core host is connected to.
 * provider_port - port where DC Fabric router is connected to.
 */
int vxlan6_basic(
    int unit,
    int access_port,
    int provider_port,
    int vdc_enable)
{
    int rv;
    int tunnel_idx = 0;
    
    vxlan6_basic_init(unit, access_port, provider_port, vdc_enable);
    
    /* 
     * Build L2 VPN
     */
    rv = sand_utils_vxlan_open_vpn(unit, cint_vxlan_basic_info.vpn_id, cint_vxlan_basic_info.vxlan_vdc, cint_vxlan_basic_info.vni);
    if (rv != BCM_E_NONE) 
    {
        printf("Error(%d), in sand_utils_vxlan_open_vpn, vpn=%d, vdc=%d \n", cint_vxlan_basic_info.vpn_id, cint_vxlan_basic_info.vxlan_vdc);
        return rv;
    }

    rv = ip_tunnel_fec_arp_map(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), ip_tunnel_fec_arp_map \n", rv);
        return rv;
    }
    
    /* Open route interfaces */
    rv = ip_tunnel_open_route_interfaces(unit, access_port, provider_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), ip_tunnel_open_route_interfaces \n", rv);
        return rv;
    }   

    /*
     * Set the split horizon 
     */
    rv = vxlan_split_horizon_set(unit, cint_vxlan_basic_info.vpn_id, cint_vxlan_basic_info.vxlan_vdc, cint_vxlan_basic_info.vxlan_network_group_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vxlan_split_horizon_set\n", rv);
        return rv;
    }
    
    /*
     * Create the tunnel termination lookup and the tunnel termination inlif
     */
    rv = ip_tunnel_term_create(unit, tunnel_idx);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in function ipv6_tunnel_term_create \n", rv);
        return rv;
    }

    /*
     *Configure ARP entries 
     */
    rv = ip_tunnel_create_arp(unit, tunnel_idx);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in ipv6_tunnel_create_arp\n", rv);
        return rv;
    }
    /*
     * Create the tunnel initiator
     */
    rv = ip_tunnel_encap_create(unit, tunnel_idx);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in ipv6_tunnel_encap_create \n", rv);
        return rv;
    }

    /*
     * Configure fec entry
     */
    rv = ip_tunnel_create_fec(unit, tunnel_idx);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in ipv6_tunnel_create_fec\n", rv);
        return rv;
    }

    /*
     * configure fec for learning purpose 
     */
    rv = vxlan_encap_fec_learn(unit, tunnel_idx);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vxlan_encap_fec_learn\n", rv);
        return rv;
    }
    
    /* 
     * Add VXLAN port properties and configure the tunnel terminator In-Lif and tunnel initiatior Out-Lif
     */
    rv = vxlan_vxlan_port_add(unit, provider_port, tunnel_idx, FALSE);
    if (rv != BCM_E_NONE) 
    {
        printf("Error(%d), vxlan_port_add \n", rv);
        return rv;
    }

    rv = vxlan_network_domain_set(unit, access_port, provider_port, tunnel_idx);
    if (rv != BCM_E_NONE) 
    {
        printf("Error(%d), vxlan_network_domain_set \n", rv);
        return rv;
    }

    rv = vxlan_vlan_port_add(unit, access_port, cint_vxlan_basic_info.access_vid, &cint_vxlan_basic_info.vlan_port_id);
    if (rv != BCM_E_NONE) 
    {
        printf("Error(%d), vxlan_vlan_port_add \n", rv);
        return rv;
    }
 
    rv = vswitch_add_port(unit, cint_vxlan_basic_info.vpn_id, access_port, cint_vxlan_basic_info.vlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), vswitch_add_port\n", rv);
        return rv;
    }

    rv = vxlan_fwd_l2(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), vxlan_fwd_l2 \n", rv);
        return rv;
    }  

    return rv;
}



