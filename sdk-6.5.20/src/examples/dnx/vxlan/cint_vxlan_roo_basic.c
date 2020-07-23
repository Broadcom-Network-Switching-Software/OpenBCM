/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * Example of basic VXLAN configuration
 *
 * cint ../../src/examples/sand/cint_sand_utils_global.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_multicast.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_vxlan.c
 * cint ../../src/examples/sand/cint_vxlan_roo_basic.c
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

/*
 * VXLAN global structure
 */
struct cint_vxlan_roo_basic_info_s 
{
    int vxlan_network_group_id;             /* ingress and egress orientation for VXLAN */
    bcm_ip_t tunnel_dip;                    /* tunnel DIP */
    bcm_ip_t tunnel_dip_mask;               /* tunnel DIP mask */
    bcm_ip_t tunnel_sip;                    /* tunnel SIP */
    bcm_ip_t tunnel_sip_mask;               /* tunnel SIP mask*/    
    bcm_ip6_t tunnel_dip6;                  /* tunnel DIP6 */
    bcm_ip6_t tunnel_dip6_mask;             /* tunnel DIP6 mask */
    bcm_ip6_t tunnel_sip6;                  /* tunnel SIP6 */
    bcm_ip6_t tunnel_sip6_mask;             /* tunnel SIP6 mask*/
    bcm_gport_t tunnel_id;                  /* Tunnel ID */
    int tunnel_if;                          /* Tunnel Interface */
    int vpn_id;                             /* VPN ID */
    int vni;                                /* VNI of the VXLAN */
    int access_port;                        /* incoming port from access side */
    int core_port;                          /* incoming port from core side */
    int core_native_eth_rif;                /* core native RIF */
    int core_overlay_eth_rif;               /* core overlay RIF */
    int core_overlay_eth_vrf;               /* core overlay VRF */
    int access_eth_rif;                     /* access RIF */
    int access_eth_vrf;                     /* access VRF */
    bcm_vlan_t access_vid;					/* access vid for AC */
    bcm_gport_t outer_ac_vlan_port_id;      /* vlan port id of the ac_port, used for ingress */
    int core_native_arp_id;                 /* Id for core native ARP entry */
    int core_overlay_arp_id;                /* Id for core overlay ARP entry */
    int access_arp_id;                      /* Id for access ARP entry */
    int core_native_fec_id;                 /* core native fec id for native outrif and native arp */
    int core_overlay_fec_id;                /* core overlay fec id for overlay */
    int access_fec_id;                      /* access fec id for access outrif and access arp */
    bcm_mac_t access_eth_fwd_mac;           /* mac for bridge fwd */
    bcm_mac_t core_native_eth_rif_mac;      /* mac for core native RIF */
    bcm_mac_t core_overlay_eth_rif_mac;     /* mac for core overlay RIF */
    bcm_mac_t access_eth_rif_mac;           /* mac for access RIF */
    bcm_mac_t core_native_next_hop_mac;     /* mac for next core native hop */
    bcm_mac_t core_overlay_next_hop_mac;    /* mac for next core overlay hop */
    bcm_mac_t access_next_hop_mac;          /* mac for next access hop */
    bcm_ip_t route_dip_into_overlay;        /* first dip */
    bcm_ip_t route_dip_out_of_overlay;      /* second dip */
    bcm_ip6_t routev6_dip_out_of_overlay;
    bcm_ip6_t maskv6_dip_out_of_overlay;
    int outer_tpid;                         /* Outer tpid for application */
    int inner_tpid;                         /* Inner tpid for application */
    bcm_gport_t egress_native_ac_vlan_port_id;  /* vlan port id of the inner ac_port, used for egress */
    uint32 egress_vlan_edit_profile, ingress_vlan_edit_profile;
    int tag_format;
    int tunnel_termination_vrf;
    bcm_if_t core_overlay_encap_tunnel_intf;
    int native_vlan; /* native vlan. Set by native vlan editing. */
};


cint_vxlan_roo_basic_info_s cint_vxlan_roo_basic_info =
{
    /*
     * VXLAN Orientation,
     */
    1,
    /*
     * tunnel DIP
     */
    0xAB000001, /* 171.0.0.1 */
    /*
     * tunnel DIP mask
     */
    0xffffffff,
    /*
     * tunnel SIP
     */
    0xAC000001, /* 172.0.0.1 */
    /*
     * tunnel SIP mask
     */
    0xffffffff,
    /*
     * tunnel DIP6
     */
    { 0x20, 0x01, 0x0D, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, 0x22, 0xFF, 0xFE, 0x33, 0x44, 0x55 },
    /*
     * tunnel DIP6 mask
     */
    { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
    /*
     * tunnel DIP6
     */
    { 0x20, 0x01, 0x0D, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, 0x22, 0xFF, 0xFE, 0x77, 0x88, 0x99 },
    /*
     * tunnel SIP6 mask
     */
    { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
    /*
     * tunnel id | tunnel_if
     */
    20, 10, 
    /*
     * VPN ID
     */
    15,
    /*
     * VNI
     */
    5000,
    /*
     * ports : access_port | core_port 
     */
    200, 201,
    /*
     * core_native_eth_rif
     */
    15,
    /*
     * core_overlay_eth_rif, core_overlay_eth_vrf
     */
    20, 5,
    /*
     * access_eth_rif, access_eth_vrf
     */
    10, 1,
    /*
     *  access_vid | outer_ac_vlan_port_id
     */
     0x5a5, 8888,
    /*
     * core_native_arp_id | core_overlay_arp_id | access_arp_id
     */
    0, 5050, 0,
    /*
     * core_native_fec_id | core_overlay_fec_id | access_fec_id | mpls_fec_id
     */
	40961, 65537, 40963,
    /*
     * access_eth_fwd_mac | core_native_eth_rif_mac | core_overlay_eth_rif_mac | access_eth_rif_mac
     */
	{0x00, 0x00, 0xA5, 0x5A, 0xA5, 0x5A},
	{0x00, 0x0F, 0x00, 0x02, 0x0A, 0x22},
	{0x00, 0x0F, 0x00, 0x02, 0x0A, 0x33},
	{0x00, 0x0F, 0x00, 0x02, 0x0A, 0x44},
    /*
     * core_native_next_hop_mac |  core_overlay_next_hop_mac | access_next_hop_mac
     */
	{0x00, 0x00, 0x00, 0x00, 0xCD, 0x1D},
	{0x00, 0x00, 0x00, 0x00, 0xCD, 0x1E},
	{0x00, 0x00, 0x00, 0x00, 0xCD, 0x1F},
    /*
     *route_dip_into_overlay | route_dip_out_of_overlay
     127.255.255.02 , 127.255.255.03
     */
    0x7fffff02, 0x7fffff03,
	/*
	 * routev6_dip_out_of_overlay | maskv6_dip_out_of_overlay
	 */
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x10 },
	{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0 },
    /*
     * outer_tpid | inner_tpid 
     */
    0x8100, 0x88a8,
    /*
     * egress_native_ac_vlan_port_id
     */
    0,
    /*
     * egress_vlan_edit_profile, ingress_vlan_edit_profile 
     */
    4, 5,
    /*
     * tag_format | tunnel_termination_vrf | core_overlay_encap_tunnel_intf
     */
    0, 5, 0, 
    /*
 */
    0xC, 

};

void
vxlan_roo_basic_init(
    int access_port,
    int core_port)
{
    cint_vxlan_roo_basic_info.access_port = access_port;
    cint_vxlan_roo_basic_info.core_port = core_port;
}

/*
 * The basic VXLAN example with parameters:
 * Unit - relevant unit
 * access_port - port where core host is connected to.
 * core_port - port where DC Fabric router is connected to.
 */
int vxlan_roo_basic(
    int unit,
    int is_ipv6_vxlan,
    int access_port,
    int core_port)
{

    int rv;
    int device_is_jericho2;
    char *proc_name;
    bcm_if_t encap_tunnel_intf;
    int term_tunnel_id;
    int encap_tunnel_id;
    bcm_gport_t vlan_port_id;
    bcm_gport_t vxlan_port_id;
    l3_ingress_intf ingr_itf;
    bcm_gport_t tunnel_id;
    
    proc_name = "vxlan_roo_basic";
   

    l3_ingress_intf_init(&ingr_itf);
    
    vxlan_roo_basic_init(access_port, core_port);

    /* 
     * Build L2 VPN
     */
    rv = vxlan_roo_basic_vxlan_open_vpn(unit, cint_vxlan_roo_basic_info.vpn_id, cint_vxlan_roo_basic_info.vni);
    if (rv != BCM_E_NONE) 
    {
        printf("%s(): Error, vxlan_roo_basic_vxlan_open_vpn, vpn=%d, \n", proc_name, cint_vxlan_roo_basic_info.vpn_id);
        return rv;
    }
    
    /* configure entry: VNI+AC
       At ESEM for vsi, netork_domain -> VNI, AC information
       create a virtual AC which represent the AC information in ESEM entry.
       The virtual AC save the key to access the ESEM entry. 
       In this case: vsi, network domain
       */
    bcm_vlan_port_t vlan_port; 
    bcm_vlan_port_t_init(&vlan_port); 
    vlan_port.vsi = cint_vxlan_roo_basic_info.vpn_id; /* vsi, part of the key in ESEM lookup */
    vlan_port.match_class_id = 0; /* network domain, part of the key in ESEM lookup */
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NAMESPACE_VSI; 
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION; 
    rv = bcm_vlan_port_create(unit, vlan_port); 
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_vlan_port_create \n", proc_name);
        return rv;
    }

    /* 
       vlan port translation set
       */
    int vlan_edit_profile = 3; 
    bcm_vlan_port_translation_t vlan_port_translation; 
    bcm_vlan_port_translation_t_init(&vlan_port_translation); 
    vlan_port_translation.flags = BCM_VLAN_ACTION_SET_EGRESS; 
    vlan_port_translation.gport = vlan_port.vlan_port_id; 
    vlan_port_translation.new_outer_vlan = cint_vxlan_roo_basic_info.native_vlan; 
    vlan_port_translation.new_inner_vlan = 0; 
    vlan_port_translation.vlan_edit_class_id = vlan_edit_profile; 
    rv = bcm_vlan_port_translation_set(unit, &vlan_port_translation); 
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_vlan_port_translation_set \n", proc_name);
        return rv;
    }
    /*
     * Create action ID
     */
    int action_id; 
    rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS, &action_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_id_create\n");
        return rv;
    }
    /*
     * Set translation action
     */
    bcm_vlan_action_set_t action;
    bcm_vlan_action_set_t_init(&action);
    action.outer_tpid = 0x8100;
    action.inner_tpid = 0x8100;
    action.dt_outer = bcmVlanActionAdd;
    action.dt_inner = bcmVlanActionNone;
    rv = bcm_vlan_translate_action_id_set(unit,
                                          BCM_VLAN_ACTION_SET_EGRESS,
                                          action_id, &action);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_id_set\n");
        return rv;
    }
    /*
     * Set translation action class (map edit_profile & tag_format to action_id)
     */
    int tag_format_untag = 0;
    bcm_vlan_translate_action_class_t action_class;
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = vlan_edit_profile;
    action_class.tag_format_class_id = tag_format_untag;
    action_class.vlan_translation_action_id = action_id;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_translate_action_class_set(unit, &action_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }

    rv = vxlan_roo_basic_configure_port_properties(unit, cint_vxlan_roo_basic_info.access_port,
                                                  cint_vxlan_roo_basic_info.core_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vxlan_roo_basic_configure_port_properties\n", rv);
        return rv;
    }
    
    /*
     * Configure L3 interfaces 
     */
    rv = vxlan_roo_basic_create_l3_interfaces(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_basic_create_l3_interfaces\n", rv);
        return rv;
    }
    
    /*
     * Create the tunnel termination lookup and the tunnel termination inlif
     */
    rv = vxlan_tunnel_terminator_create(unit, is_ipv6_vxlan, &term_tunnel_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function vxlan_tunnel_terminator_create \n", proc_name);
        return rv;
    }

    /*
     *Configure ARP entries
     */
    rv = vxlan_roo_basic_create_arp_entries(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_basic_create_arp_entries\n", rv);
        return rv;
    }
    /*
     * Create the tunnel initiator
     */
    rv = vxlan_tunnel_initiator_create(unit, is_ipv6_vxlan, cint_vxlan_roo_basic_info.core_overlay_arp_id, &(cint_vxlan_roo_basic_info.core_overlay_encap_tunnel_intf), &encap_tunnel_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function tunnel_initiator_configure \n", proc_name);
        return rv;
    }


    /*
     * Configure fec entry
     */
    rv = vxlan_roo_basic_create_fec_entries(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_basic_create_fec_entries\n", rv);
        return rv;
    }
    /* 
     * Add VXLAN port properties and configure the tunnel terminator In-Lif and tunnel initiatior Out-Lif
     */
    rv = vxlan_port_add(unit, core_port, term_tunnel_id, encap_tunnel_id, &vxlan_port_id);
    if (rv != BCM_E_NONE) 
    {
        printf("%s(): Error, vxlan_port_add \n", proc_name);
        return rv;
    }
    
    rv = vxlan_roo_basic_l3_forwarding(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_basic_l3_forwarding\n", rv);
        return rv;
    }
    
    rv = vxlan_roo_basic_eve_swap(unit, cint_vxlan_roo_basic_info.core_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_basic_eve_swap\n", rv);
        return rv;
    }
    rv = vlan_port_add(unit, access_port, cint_vxlan_roo_basic_info.access_vid, &vlan_port_id);
    if (rv != BCM_E_NONE) 
    {
        printf("%s(): Error, vlan_port_add \n", proc_name);
        return rv;
    }

    rv = vswitch_add_port(unit, cint_vxlan_roo_basic_info.vpn_id, vlan_port_id);
    if (rv != BCM_E_NONE) 
    {
        printf("%s(): Error, vswitch_add_port\n", proc_name);
        return rv;
    }

    /* 
     * Create an MACT entry to VXLAN tunnel, mapped to the VPN
     */
    rv = mact_entry_add (unit, vxlan_port_id);
    if (rv != BCM_E_NONE) 
    {
        printf("%s(): Error, mact_entry_add\n", proc_name);
        return rv;
    }


    return rv;
}

int
vxlan_roo_basic_eve_swap(
    int unit,
    int port)
{
    int action_id_1;
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t action;
    bcm_port_tpid_class_t port_tpid_class;
    bcm_vlan_translate_action_class_t action_class;
    /*
     * set tag format for untagged packets
     */
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port;
    port_tpid_class.tpid1 = cint_vxlan_roo_basic_info.outer_tpid;
    port_tpid_class.tpid2 = cint_vxlan_roo_basic_info.inner_tpid;
    port_tpid_class.tag_format_class_id = cint_vxlan_roo_basic_info.tag_format;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY;
    port_tpid_class.vlan_translation_action_id = 0;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_tpid_class_set, port=%d, \n", port);
        return rv;
    }

    /*
     * Create action ID
     */
    rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS, &action_id_1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_id_create\n");
        return rv;
    }

    /*
     * Set translation action
     */
    bcm_vlan_action_set_t_init(&action);
    action.outer_tpid = cint_vxlan_roo_basic_info.outer_tpid;
    action.inner_tpid = cint_vxlan_roo_basic_info.inner_tpid;
    action.dt_outer = bcmVlanActionDelete;
    action.dt_inner = bcmVlanActionDelete;
    rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS, action_id_1, &action);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_id_set\n");
        return rv;
    }

    /*
     * Set translation action class (map edit_profile & tag_format to action_id)
     */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = cint_vxlan_roo_basic_info.egress_vlan_edit_profile;
    action_class.tag_format_class_id = cint_vxlan_roo_basic_info.tag_format;
    action_class.vlan_translation_action_id = action_id_1;
    action_class.flags = BCM_VLAN_ACTION_SET_INGRESS;
    rv = bcm_vlan_translate_action_class_set(unit, &action_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }

    return rv;

}

int
vxlan_roo_basic_l3_forwarding(
    int unit)
{
    int rv = BCM_E_NONE;
    int opportunistic_learning_always_transplant = 0;

    opportunistic_learning_always_transplant = *(dnxc_data_get(unit, "l2", "feature", "opportunistic_learning_always_transplant", NULL));

    if (opportunistic_learning_always_transplant)
    {
        /*
         * Add route entry with mask as 0xffffffff for access to core traffic
         */
        rv = add_route_ipv4(unit, cint_vxlan_roo_basic_info.route_dip_into_overlay, 0xffffffff, cint_vxlan_roo_basic_info.access_eth_vrf, cint_vxlan_roo_basic_info.core_native_fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in add_host_ipv41, \n");
            return rv;
        }
        /*
         * Add route entry with mask as 0xffffffff for core to access traffic
         */
        rv = add_route_ipv4(unit, cint_vxlan_roo_basic_info.route_dip_out_of_overlay, 0xffffffff, cint_vxlan_roo_basic_info.tunnel_termination_vrf, cint_vxlan_roo_basic_info.access_fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in add_host_ipv42, \n");
            return rv;
        }

    }
    else
    {
        /*
         * Add Host entry for access to core traffic
         */
        rv = add_host_ipv4(unit, cint_vxlan_roo_basic_info.route_dip_into_overlay, cint_vxlan_roo_basic_info.access_eth_vrf, cint_vxlan_roo_basic_info.core_native_fec_id, 0, 0);
    	if (rv != BCM_E_NONE)
        {
            printf("Error, in add_host_ipv41, \n");
            return rv;
        }
        /*
         * Add Host entry for core to access traffic
         */
        rv = add_host_ipv4(unit, cint_vxlan_roo_basic_info.route_dip_out_of_overlay, cint_vxlan_roo_basic_info.tunnel_termination_vrf, cint_vxlan_roo_basic_info.access_fec_id, 0, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in add_host_ipv42, \n");
            return rv;
        }
    }

    rv = add_route_ipv6(unit, cint_vxlan_roo_basic_info.routev6_dip_out_of_overlay, cint_vxlan_roo_basic_info.maskv6_dip_out_of_overlay, cint_vxlan_roo_basic_info.tunnel_termination_vrf, cint_vxlan_roo_basic_info.access_fec_id);
    if (rv != BCM_E_NONE)
    {
      printf("%s(): Error, in function add_route_ipv6, err_id = %d \n", proc_name, rv);
      return rv;
    }

    return rv;
}

int
vxlan_roo_basic_create_fec_entries(
    int unit)
{
    int rv;
    int temp_out_rif;
    int flags = 0;
    int fec_id;
    bcm_gport_t gport;

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    cint_vxlan_roo_basic_info.core_native_fec_id = fec_id++;
    cint_vxlan_roo_basic_info.access_fec_id = fec_id++;

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 1, &fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    cint_vxlan_roo_basic_info.core_overlay_fec_id = fec_id ;


    /*
     * Create native FEC for routing into overlay:
     *  1) Give a fec id. 
     *  2) Native out rif.
     *  3) Native arp id. 
     *  4) Point to overlay fec.    
     */
    BCM_GPORT_FORWARD_PORT_SET(gport, cint_vxlan_roo_basic_info.core_overlay_fec_id);
    rv = l3__egress_only_fec__create(unit, cint_vxlan_roo_basic_info.core_native_fec_id,
                                     cint_vxlan_roo_basic_info.core_native_eth_rif,
                                     cint_vxlan_roo_basic_info.core_native_arp_id, gport, flags);
    printf("For native FEC: fec id 0x%x, \n eth rif 0x%x, \n arp 0x%x, \n port (fec): 0x%x \n", cint_vxlan_roo_basic_info.core_native_fec_id, 
                               cint_vxlan_roo_basic_info.core_native_eth_rif,
                               cint_vxlan_roo_basic_info.core_native_arp_id, 
                               gport);
    
    if (rv != BCM_E_NONE)
    {
        printf("Error in creating native FEC for routing into overlay\n");
        return rv;
    }

    /*
     * Create overlay FEC for routing into overlay: 
     * 1) Give a fec id. 
     * 2) Tunnel id of the overlay VXLAN Tunnel. 
     * 3) Tunnel id of the overlay Eth.
     * 4) Out port.
     */
    gport = 0;
    flags |= BCM_L3_CASCADED;
    BCM_GPORT_LOCAL_SET(gport, cint_vxlan_roo_basic_info.core_port);
    rv = l3__egress_only_fec__create(unit, cint_vxlan_roo_basic_info.core_overlay_fec_id,
                                     cint_vxlan_roo_basic_info.core_overlay_encap_tunnel_intf,
                                     0, gport, flags);
    if (rv != BCM_E_NONE)
    {
        printf("Error in creating overlay FEC for routing into overlay\n");
        return rv;
    }

    /*
     * Create FEC for routing out of overlay:
     * 1) Give a fec id. 
     * 2) Access out rif.
     * 3) Access arp id.
     * 4) Out port.
     */
    flags = 0;
    rv = l3__egress_only_fec__create(unit, cint_vxlan_roo_basic_info.access_fec_id,
                                     cint_vxlan_roo_basic_info.access_eth_rif, cint_vxlan_roo_basic_info.access_arp_id,
                                     cint_vxlan_roo_basic_info.access_port, flags);
    if (rv != BCM_E_NONE)
    {
        printf("Error in creating FEC routing out of overlay\n");
        return rv;
    }
	return rv;
}

int
vxlan_roo_basic_create_arp_entries(
    int unit)
{
    int rv;

    /*
     * Configure native ARP entry 
     */
    rv = l3__egress_only_encap__create_inner(unit, 0, &(cint_vxlan_roo_basic_info.core_native_arp_id),
                                       cint_vxlan_roo_basic_info.core_native_next_hop_mac,
                                       0, 
                                       BCM_L3_NATIVE_ENCAP, 
                                       0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create AC egress object ARP only\n");
        return rv; 
    }

    /*
     * Configure overlay ARP entry
     */
    rv = l3__egress_only_encap__create(unit, 0, &(cint_vxlan_roo_basic_info.core_overlay_arp_id),
                                       cint_vxlan_roo_basic_info.core_overlay_next_hop_mac,
                                       cint_vxlan_roo_basic_info.core_overlay_eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create AC egress object ARP only\n");
        return rv;
    }

    /*
     * Configure access ARP entry
     */
    rv = l3__egress_only_encap__create(unit, 0, &(cint_vxlan_roo_basic_info.access_arp_id),
                                       cint_vxlan_roo_basic_info.access_next_hop_mac,
                                       cint_vxlan_roo_basic_info.access_eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create AC egress object ARP only\n");
        return rv;
    }

    return rv;

}

int
vxlan_roo_basic_configure_port_properties(
    int unit,
    int access_port,
    int core_port)
{
    int rv = BCM_E_NONE;
    /*
     * set class for both ports
     */
    rv = bcm_port_class_set(unit, access_port, bcmPortClassId, access_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_class_set, port=%d, \n", access_port);
        return rv;
    }

    rv = bcm_port_class_set(unit, core_port, bcmPortClassId, core_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_class_set, port=%d, \n", core_port);
        return rv;
    }

    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, cint_vxlan_roo_basic_info.access_port, cint_vxlan_roo_basic_info.access_eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }

    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, cint_vxlan_roo_basic_info.core_port, cint_vxlan_roo_basic_info.core_overlay_eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }

    /*
     * Set Out-Port default properties
     */
    
    rv = out_port_set(unit, cint_vxlan_roo_basic_info.access_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set intf_out\n");
        return rv;
    }

    /*
     * Set Out-Port default properties
     */
    rv = out_port_set(unit, cint_vxlan_roo_basic_info.core_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set intf_out\n");
        return rv;
    }

    return rv;
}

int
vxlan_roo_basic_create_l3_interfaces(
    int unit)
{
    bcm_l3_ingress_t ingress_l3_interface;
    bcm_l3_ingress_t_init(&ingress_l3_interface);
    l3_ingress_intf ingr_itf_access;
    l3_ingress_intf ingr_itf_overlay;
    l3_ingress_intf ingr_itf_native_overlay;
    int rv;

    rv = intf_eth_rif_create(unit, cint_vxlan_roo_basic_info.core_native_eth_rif,
                             cint_vxlan_roo_basic_info.core_native_eth_rif_mac);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create pwe_intf\n");
        return rv;
    }

    rv = intf_eth_rif_create(unit, cint_vxlan_roo_basic_info.core_overlay_eth_rif,
                             cint_vxlan_roo_basic_info.core_overlay_eth_rif_mac);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create pwe_intf\n");
        return rv;
    }

    rv = intf_eth_rif_create(unit, cint_vxlan_roo_basic_info.access_eth_rif,
                             cint_vxlan_roo_basic_info.access_eth_rif_mac);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create pwe_intf\n");
        return rv;
    }
    /*
     * Set Incoming ETH-RIF properties, VRF is updated for the rif.
     * This VRF value is used for IP-Tunnel-Term.VRF lookup key.
     */
    l3_ingress_intf_init(&ingr_itf_access);
    ingr_itf_access.intf_id = cint_vxlan_roo_basic_info.access_eth_rif;
    ingr_itf_access.vrf = cint_vxlan_roo_basic_info.access_eth_vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf_access);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_ingress_rif_set ingr_itf_access \n");
        return rv;
    }
    
    l3_ingress_intf_init(&ingr_itf_overlay);
    ingr_itf_overlay.intf_id = cint_vxlan_roo_basic_info.core_overlay_eth_rif;
    ingr_itf_overlay.vrf = cint_vxlan_roo_basic_info.core_overlay_eth_vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf_overlay);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_ingress_rif_set ingr_itf_overlay \n");
        return rv;
    }

    l3_ingress_intf_init(&ingr_itf_native_overlay);
    ingr_itf_native_overlay.intf_id = cint_vxlan_roo_basic_info.core_native_eth_rif;
    ingr_itf_native_overlay.vrf = cint_vxlan_roo_basic_info.tunnel_termination_vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf_native_overlay);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_ingress_rif_set ingr_itf_native_overlay \n");
        return rv;
    }
    return rv;

}

int
vxlan_roo_basic_create_eth_rifs(
    int unit)
{

    int rv;

    /*
     * create vlan based on the vsi (vpn)
     */
    rv = bcm_vlan_create(unit, cint_vxlan_roo_basic_info.core_native_eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Failed (%d) to create VLAN %d\n", rv, cint_vxlan_roo_basic_info.core_native_eth_rif);
        return rv;
    }

    /*
     * create vlan based on the vsi (vpn)
     */
    rv = bcm_vlan_create(unit, cint_vxlan_roo_basic_info.core_overlay_eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Failed (%d) to create VLAN %d\n", rv, cint_vxlan_roo_basic_info.core_overlay_eth_rif);
        return rv;
    }



    return rv;
}

int mact_entry_add(
    int unit,
    int gport)
{
    int rv;
    bcm_l2_addr_t l2addr;

    bcm_l2_addr_t_init(&l2addr, cint_vxlan_roo_basic_info.access_eth_fwd_mac, cint_vxlan_roo_basic_info.vpn_id);
    l2addr.port = gport;
    l2addr.flags = BCM_L2_STATIC;

    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, bcm_l2_addr_add \n");
        return rv;
    }
    return rv;
}

int vxlan_port_add(
    int unit,
    int core_port,
    int tunnel_term_id,
    int tunnel_encapsulation_id,
    bcm_gport_t *vxlan_port_id)
{
    int rv;
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;

    vxlan_port_add_s vxlan_port_add;
    vxlan_port_s_clear(&vxlan_port_add);
    vxlan_port_add.vpn = cint_vxlan_roo_basic_info.vpn_id;
    vxlan_port_add.in_port = core_port;
    vxlan_port_add.in_tunnel = tunnel_term_id;
    vxlan_port_add.out_tunnel = tunnel_encapsulation_id;
    BCM_L3_ITF_SET(vxlan_port_add.egress_if, BCM_L3_ITF_TYPE_FEC, cint_vxlan_roo_basic_info.core_overlay_fec_id); 
    vxlan_port_add.flags = 0;
    vxlan_port_add.network_group_id = cint_vxlan_roo_basic_info.vxlan_network_group_id;
    vxlan_port_add.out_tunnel_if = cint_vxlan_roo_basic_info.core_overlay_encap_tunnel_intf;
    
    rv = vxlan__vxlan_port_add(unit, vxlan_port_add);
    *vxlan_port_id = vxlan_port_add.vxlan_port_id;

    return BCM_E_NONE;
}

int vxlan_tunnel_initiator_create(
    int unit,
    int is_ipv6_vxlan,
    bcm_if_t arp_itf,
    bcm_if_t *tunnel_intf,
    int *encap_tunnel_id)
{
    int rv;
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;

    bcm_l3_intf_t_init(&l3_intf);
    /* Create IP tunnel initiator for encapsulating Vxlan tunnel header*/
    bcm_tunnel_initiator_t_init(&tunnel_init);
    if (is_ipv6_vxlan) {
        tunnel_init.type = bcmTunnelTypeVxlan6;
        tunnel_init.dip6 = cint_vxlan_roo_basic_info.tunnel_dip6; /* default: 2001:0DB8:0000:0000:0211:22FF:FE33:4455 */
        tunnel_init.sip6 = cint_vxlan_roo_basic_info.tunnel_sip6; /* default: 2001:0DB8:0000:0000:0211:22FF:FE77:8899 */
    } else {
        tunnel_init.type = bcmTunnelTypeVxlan;
        tunnel_init.dip = cint_vxlan_roo_basic_info.tunnel_dip; /* default: 171.0.0.1 */
        tunnel_init.sip = cint_vxlan_roo_basic_info.tunnel_sip; /* default: 172.0.0.1 */
    }
    tunnel_init.flags = 0;
    tunnel_init.dscp = 10; /* default: 10 */
    tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;   
    tunnel_init.l3_intf_id = arp_itf;
    tunnel_init.ttl = 64; /* default: 64 */
    tunnel_init.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE; /* default: -1 */
    tunnel_init.encap_access = bcmEncapAccessTunnel2;
    rv = bcm_tunnel_initiator_create(unit,&l3_intf, tunnel_init);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_initiator_create \n");
        return rv;
    }
    *tunnel_intf = l3_intf.l3a_intf_id;
    *encap_tunnel_id = tunnel_init.tunnel_id;
    return rv;
}

/* 
 * Main function for configuring the VXLAN VPN
 */
int vxlan_roo_basic_vxlan_open_vpn(
    int unit, 
    int vpn_id, 
    int vni)
{
    int rv = BCM_E_NONE;
    char *proc_name;

    proc_name = "vxlan_roo_basic_vxlan_open_vpn";

    bcm_vxlan_vpn_config_t vpn_config;
    bcm_vxlan_vpn_config_t_init(&vpn_config);
    vpn_config.flags = BCM_VXLAN_VPN_WITH_ID|BCM_VXLAN_VPN_WITH_VPNID;
    vpn_config.vpn = vpn_id;
    vpn_config.broadcast_group = vpn_id;
    vpn_config.unknown_unicast_group = vpn_id;
    vpn_config.unknown_multicast_group = vpn_id;
    vpn_config.vnid = vni;
    rv = bcm_vxlan_vpn_create(unit, &vpn_config);
    if(rv != BCM_E_NONE) 
    {
        printf("%s(): Error in bcm_vxlan_vpn_create \n", proc_name);
        return rv;
    }
    return rv;
}

int vxlan_tunnel_terminator_create(
    int unit,
    int is_ipv6_vxlan,
    int *term_tunnel_id)
{
    int rv;
    bcm_tunnel_terminator_t tunnel_term;

    /* Create IP tunnel terminator for SIP,DIP, VRF lookup */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    if (is_ipv6_vxlan) {
        tunnel_term.type = bcmTunnelTypeVxlan6;
        tunnel_term.dip6 = cint_vxlan_roo_basic_info.tunnel_dip6;
        tunnel_term.dip6_mask = cint_vxlan_roo_basic_info.tunnel_dip6_mask;
        tunnel_term.sip6 = cint_vxlan_roo_basic_info.tunnel_sip6;
        tunnel_term.sip6_mask = cint_vxlan_roo_basic_info.tunnel_sip6_mask;        
    } else {
        tunnel_term.type = bcmTunnelTypeVxlan;
        tunnel_term.dip = cint_vxlan_roo_basic_info.tunnel_dip;
        tunnel_term.dip_mask = cint_vxlan_roo_basic_info.tunnel_dip_mask;
        tunnel_term.sip = cint_vxlan_roo_basic_info.tunnel_sip;
        tunnel_term.sip_mask = cint_vxlan_roo_basic_info.tunnel_sip_mask;
    }
    tunnel_term.vrf = cint_vxlan_roo_basic_info.core_overlay_eth_vrf;
    BCM_GPORT_TUNNEL_ID_SET(tunnel_term.tunnel_id, 0x3000);
    tunnel_term.flags = BCM_TUNNEL_TERM_TUNNEL_WITH_ID;
    

    rv = bcm_tunnel_terminator_create(unit,&tunnel_term);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_terminator_create \n");
        return rv;
    }
    *term_tunnel_id = tunnel_term.tunnel_id;
    /** In JR2 VRF is a tunnel-LIF property set by bcm_l3_ingress_create API */
     bcm_l3_ingress_t ing_intf;
     bcm_l3_ingress_t_init(&ing_intf);
     bcm_if_t intf_id;

     ing_intf.flags = BCM_L3_INGRESS_WITH_ID; /* must, as we update exist RIF */
     ing_intf.vrf = cint_vxlan_roo_basic_info.tunnel_termination_vrf;

     /* Convert tunnel's GPort ID to intf ID */
      BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(intf_id, tunnel_term.tunnel_id);
      rv = bcm_l3_ingress_create(unit, ing_intf, intf_id);
      if (rv != BCM_E_NONE) 
      {
          printf("Error, bcm_l3_ingress_create\n");
          return rv;
      }
    return rv;
}

int vlan_port_add(int unit, bcm_gport_t in_port, bcm_vlan_t vid, bcm_gport_t *port_id){
    int rv;
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);

    /* the match criteria is port:1, out-vlan:510   */
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = in_port;
    vlan_port.match_vlan = vid;
    vlan_port.flags = 0;
    vlan_port.vsi = 0;
    rv = bcm_vlan_port_create(unit, &vlan_port);

    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }

    /* handle of the created gport */
    *port_id = vlan_port.vlan_port_id;

    return rv;
}

int vswitch_add_port(int unit, bcm_vlan_t vsi, bcm_gport_t gport){
    int rv;
    int device_is_jericho2;

    /* add to vswitch */
    rv = bcm_vswitch_port_add(unit, vsi, gport);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }

    return rv;
}


