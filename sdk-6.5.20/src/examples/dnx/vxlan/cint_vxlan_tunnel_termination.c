/*
 *  : cint_vxlan_tunnel_term_basic.c,v 1.0 2017/07/28 10:54:52 sk019289
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * Example of basic VXLAN configuration
 *
 * cint ../../src/examples/sand/cint_sand_utils_global.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_multicast.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_vxlan.c
 * cint ../../src/examples/dnx/cint_vxlan_tunnel_termination.c
 * cint
 * int unit = 0;
 * int rv = 0;
 * int core_port = 200;
 * int virtual_machine_port = 201;
 * rv = vxlan_gpe_tunnel_termination(unit, core_port, virtual_machine_port);
 * print rv;
 *
 * Traffic:
 *
 *  Send the following traffic:
 *
 *  ###############################################################################################################
 *  Sending packet from port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *      0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
 *   |  |                                VLAN (30)                                          |    Header
 *   |  |-----------------------------------------------------------------------------------|  (18 bytes)
 *   |  |       Ethertype = 0xffff(Eth)      |                                              |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |                              Original                                             |    Payload
 *   |  |                              Payload                                              |
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Receiving packet on port == 203:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                               DA   ( 00:00:00:00:00:f1)                           |    Native
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |               DA                    |             SA   (00:0C:00:02:00:01)        |
 *   |  |-----------------------------------------------------------------------------------|   Ethernet
 *   |  |                                 SA                                                |
 *   |  |-----------------------------------------------------------------------------------|    Header
 *   |  |                                VLAN  (20)                                         |  ( 18 bytes )
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |       Ethertype = 0xffff(Eth)       |                                             |
 *   |  |-----------------------------------------------------------------------------------|------------------------
 *   |  |                              Original                                             |    Payload
 *   |  |                              Payload                                              |
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ##################################################################################################################
 */

/*
 * Example of IPv6 VXLAN GPE L3 Tunnel Termination
 *
 * cint ../../src/examples/sand/cint_sand_utils_global.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_multicast.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_vxlan.c
 * cint ../../src/examples/dnx/cint_vxlan_tunnel_termination.c
 * cint
 * int unit = 0;
 * int rv = 0;
 * int core_port = 200;
 * int virtual_machine_port = 201;
 * rv = vxlan6_gpe_tunnel_termination_ipv4(unit, core_port, virtual_machine_port);
 * print rv;
 *
 * Traffic:
 *
 *  Send the following traffic:
 *
 *  ###############################################################################################################
 *  Sending packet from port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *      0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |  |                                 DA  (00:0C:00:02:00:00)                           |
 *   |  |-----------------------------------------------------------------------------------|    LL Ethernet
 *   |  |               DA                   |              SA  (00:00:07:00:01:00)         |
 *   |  |-----------------------------------------------------------------------------------|       Header
 *   |  |                                 SA                                                |
 *   |  |-----------------------------------------------------------------------------------|     ( 18 bytes)
 *   |  |       TPID  = 0x8100               |       VID = 15                               |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |       Ethertype = 0x86DD(IPv6)     |                                              |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |     Version  |  IHL   |   ToS      |          Total Lenght                        |
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |        Identification              |   Flags   |  Fragment Offset                 |   Tunnel   IPv4
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |         TTL       |Protocol=17(UDP)|             Header Checksum                  |       Header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |     IPv6 SRC address (ECEF:EEED:ECEB:EAE9:E8E7:E6E5:E4E3:E2E1)                    |      (40 bytes)
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |     IPv6 DST address (2001:0DB8:0000:0000:0211:22FF:FE33:4455)                    |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |    Source port (0x5555)            |    Destination port (0x12b6)                 |   UDP  Header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |    UDP lenght                      |    UDP checksum                              |    (8 bytes)
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  | | R | R |  Ver  | I | P=1 | R | O  |    Reserved      | Next Protocol = 1(IPv4)   |  VXLAN  header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |    VXLAN Network Identifier (VNI) = 0x5000            | Reserved                  |    (8 bytes)
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |     Version  |  IHL   |   ToS      |          Total Lenght                        |
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |        Identification              |   Flags   |  Fragment Offset                 |   Tunnel   IPv4
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |         TTL = 128       |Protocol=17(UDP)|             Header Checksum            |       Header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Source address ( 168.0.0.1 )                         |      (20 bytes)
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Destination address (169.0.0.1)                      |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |    Source port                     |    Destination port                          |   UDP  Header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |    UDP lenght                      |    UDP checksum                              |    (8 bytes)
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |                              Original                                             |    Payload
 *   |  |                              Payload                                              |
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                               DA   ( 00:0C:00:02:00:01)                           |    Native
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |               DA                    |             SA   (00:0C:00:02:00:0C)        |
 *   |  |-----------------------------------------------------------------------------------|   Ethernet
 *   |  |                                 SA                                                |
 *   |  |-----------------------------------------------------------------------------------|    Header
 *   |  |                                VLAN  (0x11)                                         |  ( 18 bytes )
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |       Ethertype = 0x0800(Eth)       |                                             |
 *   |  |-----------------------------------------------------------------------------------|------------------------
 *   |  |     Version  |  IHL   |   ToS      |          Total Lenght                        |
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |        Identification              |   Flags   |  Fragment Offset                 |   Tunnel   IPv4
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |         TTL = 127       |Protocol=17(UDP)|             Header Checksum            |       Header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Source address ( 168.0.0.2 )                         |      (20 bytes)
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Destination address (169.0.0.1)                      |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |    Source port                     |    Destination port                          |   UDP  Header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |    UDP lenght                      |    UDP checksum                              |    (8 bytes)
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |                              Original                                             |    Payload
 *   |  |                              Payload                                              |
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ##################################################################################################################
 *
 * 
 * Example of VXLAN configuration when no VSI are resolved from VNI.
 * define trap when vni->vsi is not resolved. 
 * Consequently, the tunnel is still terminated. 
 * A default VSI (defined in tunnel in lif), is the new vsi instead. 
 *  
 * The default vsi (16b) is: 2b defined in tunnel in lif. 
 * The remaining bits are resolved by arr prefix (1024) 
 * Default vsi value is 1024
 * 
 * 
 * cint ../../src/examples/sand/cint_sand_utils_global.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_multicast.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_vxlan.c
 * cint ../../src/examples/dnx/cint_vxlan_tunnel_termination.c
 * cint
 * int unit = 0;
 * int rv = 0;
 * int core_port = 200;
 * int virtual_machine_port = 201;
 * rv = vxlan_gpe_tunnel_termination_sem(unit, core_port, virtual_machine_port);
 * print rv;
 * int trap_default; 
 * bcm_switch_control_get(unit, bcmSwitchForwardLookupNotFoundTrap, &trap_default}
 * bcm_gport_t action_gport;
 * BCM_GPORT_TRAP_SET(action_gport, trap_default,7,0);
 * cint_tcam_1q_1a_main(0,bcmFieldStageIngressPMF1, BCM_FIELD_CONTEXT_ID_DEFAULT);
 * bcm_field_entry_t entry_id;
 * int default_vsi = 1024;
 * cint_tcam_1q_1a_entry_add(0,default_vsi,action_gport,&entry_id);
 */ 

/*
 * VXLAN global structure
 */
struct cint_vxlan_tunnel_basic_info_s
{
    int vxlan_network_group_id;             /* ingress and egress orientation for VXLAN */
    int inner_vlan;                         /* Virtual machine VID */
    int virtual_machine_vlan;               /* Out VLAN*/
    int virtual_machine_vlan_2;               /* Out VLAN*/
    int eth_rif_intf_core;                  /* Core RIF */
    int eth_rif_intf_virtual_machine;       /* out RIF */
    int eth_rif_core_vrf;                   /* VRF for termination */
    int access_vrf;                         /* VRF after termination,resolved as Tunnel-InLif property */
    bcm_mac_t core_mac_address;             /* mac for next hop */
    bcm_mac_t intf_access_mac_address;      /* mac for out RIF */
    bcm_mac_t virtual_machine_mac_address;  /* tunnel next hop MAC DA */
    bcm_mac_t virtual_machine_da;           /* Remote VM DA */
    bcm_mac_t virtual_machine_da_encap;     /* Remote VM DA */
    bcm_ip_t tunnel_dip;                    /* tunnel DIP */
    bcm_ip_t tunnel_dip_mask;               /* tunnel DIP mask */
    bcm_ip_t tunnel_sip;                    /* tunnel SIP */
    bcm_ip_t tunnel_sip_mask;               /* tunnel SIP mask */
    bcm_ip_t tunnel_dip_encap;              /* tunnel DIP */
    bcm_ip_t tunnel_sip_encap;              /* tunnel DIP */
    bcm_ip_t dip_route;                     /* route DIP */
    bcm_ip_t dip_route_encap;               /* route DIP for encapsulation*/
    bcm_ip_t dip_route_mask;                /* route DIP mask*/
    int arp_id;                             /* Id for ARP entry */
    int arp_id_2;                           /* Id for ARP entry */
    int fec_id;                             /* FEC id */
    int fec_id_2;                           /* FEC id */
    bcm_gport_t tunnel_id;                  /* Tunnel ID */
    int tunnel_if;                          /* Tunnel Interface */
    int vpn_id;                             /* VPN ID */
    int vpn_id_2;                           /* VPN ID */
    int vni;                                /* VNI of the VXLAN */
    int vxlan_tunnel_type;                  /* VxLAN or VxLAN-GPE */
    int tunnel_term_id;                     /* LIF Gport of the tunnel*/
    int tunnel_encapsulation_id;
    int network_domain;
    int network_domain_2;
    bcm_ip6_t tunnel_ipv6_dip;              /* tunnel IPv6 DIP */
    bcm_ip6_t tunnel_ipv6_dip_mask;         /* tunnel DIP mask*/
    bcm_ip6_t tunnel_ipv6_sip;              /* tunnel SIP */
    bcm_ip6_t tunnel_ipv6_sip_mask;         /* tunnel SIP mask */
    int flags_ing_intf;                             /*flags, used for cs_profile of the tunnel LIF*/
};

cint_vxlan_tunnel_basic_info_s cint_vxlan_tunnel_basic_info = {
    /*
     * VXLAN Orientation, Inner VLAN
     */
    1, 20, 30, 31,
    /*
     * eth_rif_intf_core | eth_rif_intf_virtual_machine
     */
    15, 17,
    /*
     * Core VRF | eth_rif_core_vrf
     */
    1, 5,
    /*
     * core_mac_address | intf_access_mac_address | virtual_machine_mac_address | Virtual Machine DA  |   virtual_machine_da_encap
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}, {0x00, 0x0c, 0x00, 0x02, 0x00, 0x0c}, {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01}, 
    {0x00, 0x00, 0x00, 0x00, 0x00, 0xf1}, {0x00, 0x00, 0x00, 0x00, 0x00, 0xf2},
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
    0xAC000001 /* 172.0.0.1 */ ,
    /*
     * tunnel SIP mask
     */
    0xffffffff,
    /*
     * tunnel DIP encap
     */
    0xAB000002, /* 171.0.0.2 */
    /*
     * tunnel SIP encap
     */
    0xAC000002, /* 171.0.0.2 */
    /*
     * route DIP
     */
    0xA9000001 /* 169.0.0.1 */ ,
    /*
     * route DIP
     */
    0xA9000002 /* 169.0.0.1 */ ,
    /*
     * route DIP mask
     */
    0xffffffff,
    /*
     * arp_id | arp_id_2
     */
    0, 0,
    /*
     * fec id | fec id 2 | tunnel id | tunnel_if
     */
    40980, 40981, 20, 10,
    /*
     * VPN ID
     */
    15, 16,
    /*
     * VNI
     */
    5000,
    /*
     * vxlan_tunnel_type
     */
     bcmTunnelTypeVxlanGpe,
     /*
      * tunnel_term_id | tunnel_encapsulation_id | network_domain | network_domain_2
      */
     0, 0, 1, 2,
    /*
     * tunnel IPv6 DIP
     */
    { 0x20, 0x01, 0x0D, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, 0x22, 0xFF, 0xFE, 0x33, 0x44, 0x55 },
    /*
     * tunnel DIP mask
     */
    { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
    /*
     * tunnel IPv6 SIP
     */
    { 0xEC, 0xEF, 0xEE, 0xED, 0xEC, 0xEB, 0xEA, 0xE9, 0xE8, 0xE7, 0xE6, 0xE5, 0xE4, 0xE3, 0xE2, 0xE1 },
    /*
     * tunnel SIP mask
     */
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }, 0
};

/*
 * Reusable VXLAN-GPE L2 tunnel termination example config with parameters:
 *  unit                 - relevant unit
 *  core_port            - port where core routers and aggregation switches are connected.
 *  virtual_machine_port - port where virtual machines are connected.
 */
int
vxlan_gpe_tunnel_termination(
    int unit,
    int core_port,
    int virtual_machine_port)
{

    int rv;
    char *proc_name;
    bcm_if_t encap_tunnel_intf;
    int term_tunnel_id;
    int encap_tunnel_id;

    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);

    bcm_gport_t vlan_port_id;
    bcm_gport_t vxlan_port_id;

    proc_name = "vxlan_gpe_tunnel_termination";

    /*
     * Build L2 VPN
     */
    rv = vxlan_open_vpn(unit, cint_vxlan_tunnel_basic_info.vpn_id, cint_vxlan_tunnel_basic_info.vni);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_open_vpn, vpn=%d, \n", proc_name, cint_vxlan_tunnel_basic_info.vpn_id);
        return rv;
    }

    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, core_port, cint_vxlan_tunnel_basic_info.eth_rif_intf_core);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set - core\n", proc_name);
        return rv;
    }

    /*
     * Set Out-Port default properties
     */
    rv = out_port_set(unit, virtual_machine_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set vm_port \n", proc_name);
        return rv;
    }

    /*
     * Create Routing interface for the tunnel's DIP. We will use it as ingress ETH-RIF to perform
     * LL termination (my-mac procedure), to enable the IPv4 routing for this ETH-RIF and to set the VRF.
     */
    rv = intf_eth_rif_create(unit, cint_vxlan_tunnel_basic_info.eth_rif_intf_core,
                             cint_vxlan_tunnel_basic_info.core_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create eth_rif_intf_core \n", proc_name);
        return rv;
    }

    /*
     * Set Incoming ETH-RIF properties, VRF is updated for the rif.
     * This VRF value is used for IP-Tunnel-Term.VRF lookup key.
     */
    ingr_itf.intf_id = cint_vxlan_tunnel_basic_info.eth_rif_intf_core;
    ingr_itf.vrf = cint_vxlan_tunnel_basic_info.eth_rif_core_vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_ingress_rif_set intf_out \n", proc_name);
        return rv;
    }

    /*
     * Create the tunnel termination lookup and the tunnel termination inlif
     */
    rv = vxlan_tunnel_terminator_create(unit, &term_tunnel_id, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function vxlan_tunnel_terminator_create \n", proc_name);
        return rv;
    }

    /*
     * Create the tunnel initiator
     */
    rv = vxlan_tunnel_initiator_create(unit, cint_vxlan_tunnel_basic_info.arp_id, &encap_tunnel_intf, &encap_tunnel_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function tunnel_initiator_configure \n", proc_name);
        return rv;
    }

    /*
     * Add VXLAN port properties and configure the tunnel terminator In-Lif and tunnel initiatior Out-Lif
     */
    rv = vxlan_port_add(unit, core_port, term_tunnel_id, encap_tunnel_id, &vxlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_port_add \n", proc_name);
    }

    /*
     * Create an MACT entry for VXLAN tunnel, mapped to the VPN
     */
    rv = mact_entry_add(unit, virtual_machine_port, cint_vxlan_tunnel_basic_info.virtual_machine_da);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, mact_entry_add\n", proc_name);
        return rv;
    }

    return rv;
}


/*
 * VxLAN-GPE L2 tunnel termination example:
 * Tunnel Termination for ETHoVXLAN-GPEoUDPoIPv4oE packet format.
 * VTT3 TCAM: DIP, SIP, VRF, tunnel type, next protocol.
 * VRF resolution covered - Tunnel LIF FoDo property.
 * FoDo is resolved by VNI lookup at VTT4.
 */
int
vxlan_gpe_tunnel_termination_tcam(
    int unit,
    int core_port,
    int virtual_machine_port)
{
    int rv;
    char *proc_name;

    proc_name = "vxlan_gpe_tunnel_termination_tcam";

    /* Configure masks for VTT3 TCAM lookup */
    cint_vxlan_tunnel_basic_info.tunnel_dip_mask = 0xffffff00;
    cint_vxlan_tunnel_basic_info.tunnel_sip_mask = 0x00000000;

    rv = vxlan_gpe_tunnel_termination(unit, core_port, virtual_machine_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_gpe_tunnel_termination, vpn=%d, \n", proc_name);
        return rv;
    }

    return rv;
}

/*
 * VxLAN-GPE L2 tunnel termination example:
 * Tunnel Termination for ETHoVXLAN-GPEoUDPoIPv4oETH packet format.
 * VTT3 SEM: DIP, VRF, tunnel type
 * FoDo is resolved by VNI lookup at VTT4.
 */
int
vxlan_gpe_tunnel_termination_sem(
    int unit,
    int core_port,
    int virtual_machine_port)
{
    int rv;
    char *proc_name;

    proc_name = "vxlan_gpe_tunnel_termination_sem";

    /* Configure masks for VTT3 SEM lookup */
    cint_vxlan_tunnel_basic_info.tunnel_dip_mask = 0xffffffff;
    cint_vxlan_tunnel_basic_info.tunnel_sip_mask = 0x00000000;

    rv = vxlan_gpe_tunnel_termination(unit, core_port, virtual_machine_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_gpe_tunnel_termination, vpn=%d, \n", proc_name);
        return rv;
    }

    return rv;
}


int
mact_entry_add(
    int unit,
    int vxlan_port_id,
    bcm_mac_t virtual_machine_da)
{
    int rv;
    bcm_l2_addr_t l2addr;

    bcm_l2_addr_t_init(&l2addr, virtual_machine_da, cint_vxlan_tunnel_basic_info.vpn_id);
    l2addr.port = vxlan_port_id;
    l2addr.mac = virtual_machine_da;
    l2addr.vid = cint_vxlan_tunnel_basic_info.vpn_id;
    l2addr.l2mc_group = cint_vxlan_tunnel_basic_info.vpn_id;
    l2addr.flags = BCM_L2_STATIC;

    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_l2_addr_add \n", proc_name);
        return rv;
    }
    return rv;
}



int
vxlan_port_add(
    int unit,
    int core_port,
    int tunnel_term_id,
    int tunnel_encapsulation_id,
    bcm_gport_t * vxlan_port_id)
{
    bcm_if_t egress_if;
    bcm_vxlan_port_t vxlan_port;
    int rv = BCM_E_NONE;

    BCM_L3_ITF_SET(egress_if, BCM_L3_ITF_TYPE_FEC, cint_vxlan_tunnel_basic_info.fec_id);
    bcm_vxlan_port_t_init(&vxlan_port);
    vxlan_port.criteria = BCM_VXLAN_PORT_MATCH_VN_ID;
    vxlan_port.match_port = core_port;
    vxlan_port.match_tunnel_id = tunnel_term_id;
    vxlan_port.flags = BCM_VXLAN_PORT_EGRESS_TUNNEL;
    vxlan_port.egress_if = egress_if;
    vxlan_port.egress_tunnel_id = tunnel_encapsulation_id;
    vxlan_port.default_vpn = 1023;

    rv = bcm_vxlan_port_add(unit, cint_vxlan_tunnel_basic_info.vpn_id, &vxlan_port);
    if(rv != BCM_E_NONE) {
        printf("error bcm_vxlan_port_add \n");
        return rv;
    }

    *vxlan_port_id = vxlan_port.vxlan_port_id;
    return rv;
}

int
vxlan_tunnel_initiator_create(
    int unit,
    bcm_if_t arp_itf,
    bcm_if_t * tunnel_intf,
    int *encap_tunnel_id)
{
    int rv;
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;

    bcm_l3_intf_t_init(&l3_intf);

    /*
     * Create IP tunnel initiator for encapsulating Vxlan tunnel header
     */
    bcm_tunnel_initiator_t_init(&tunnel_init);
    tunnel_init.dip = cint_vxlan_tunnel_basic_info.tunnel_dip_encap;  /* default: 171.0.0.17 */
    tunnel_init.sip = cint_vxlan_tunnel_basic_info.tunnel_sip_encap;  /* default: 160.0.0.1 */
    tunnel_init.flags = 0;
    tunnel_init.dscp = 10;      /* default: 10 */
    tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    tunnel_init.type = cint_vxlan_tunnel_basic_info.vxlan_tunnel_type;
    tunnel_init.l3_intf_id = arp_itf;
    tunnel_init.ttl = 64;       /* default: 64 */
    tunnel_init.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE;   /* default: -1 */
    rv = bcm_tunnel_initiator_create(unit, &l3_intf, tunnel_init);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_initiator_create \n");
    }
    *tunnel_intf = l3_intf.l3a_intf_id;
    *encap_tunnel_id = tunnel_init.tunnel_id;
    cint_vxlan_tunnel_basic_info.tunnel_encapsulation_id = tunnel_init.tunnel_id;
    return rv;
}

/* 
 * Main function for configuring the VXLAN VPN
 */
int
vxlan_open_vpn(
    int unit,
    int vpn_id,
    int vni)
{
    int rv = BCM_E_NONE;
    char *proc_name;

    proc_name = "vxlan_open_vpn";
    bcm_vxlan_vpn_config_t vpn_config;
    bcm_vxlan_vpn_config_t_init(&vpn_config);
    vpn_config.flags = BCM_VXLAN_VPN_WITH_ID | BCM_VXLAN_VPN_WITH_VPNID;
    vpn_config.broadcast_group = vpn_id;
    vpn_config.vpn = vpn_id;
    vpn_config.vnid = vni;
    vpn_config.unknown_unicast_group = vpn_id;
    vpn_config.unknown_multicast_group = vpn_id;
    rv = bcm_vxlan_vpn_create(unit, &vpn_config);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error in bcm_vxlan_vpn_create \n", proc_name);
        return rv;
    }
    return rv;
}

int
vxlan_tunnel_terminator_create(
    int unit,
    int *term_tunnel_id,
    int is_l3_case)
{
    int rv;
    bcm_tunnel_terminator_t tunnel_term;

    /*
     * Create IP tunnel terminator for SIP,DIP, VRF lookup in TCAM
     */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.dip      = cint_vxlan_tunnel_basic_info.tunnel_dip;
    tunnel_term.dip_mask = cint_vxlan_tunnel_basic_info.tunnel_dip_mask;
    tunnel_term.sip      = cint_vxlan_tunnel_basic_info.tunnel_sip;
    tunnel_term.sip_mask = cint_vxlan_tunnel_basic_info.tunnel_sip_mask;
    tunnel_term.vrf      = cint_vxlan_tunnel_basic_info.eth_rif_core_vrf;
    tunnel_term.type     = cint_vxlan_tunnel_basic_info.vxlan_tunnel_type;

    rv = bcm_tunnel_terminator_create(unit, &tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_terminator_create \n");
        return rv;
    }

    if (is_l3_case)
    {
        /** In JR2 VRF is a tunnel-LIF property set by bcm_l3_ingress_create API */
        bcm_l3_ingress_t ing_intf;
        bcm_l3_ingress_t_init(&ing_intf);
        ing_intf.flags = cint_vxlan_tunnel_basic_info.flags_ing_intf; /* must, as we update exist RIF */
        ing_intf.vrf   = cint_vxlan_tunnel_basic_info.access_vrf+3;

        /** Convert tunnel's GPort ID to intf ID */
        bcm_if_t intf_id;
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(intf_id, tunnel_term.tunnel_id);

        rv = bcm_l3_ingress_create(unit, ing_intf, intf_id);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l3_ingress_create\n");
            return rv;
        }
    }
    *term_tunnel_id = tunnel_term.tunnel_id;
    cint_vxlan_tunnel_basic_info.tunnel_term_id = tunnel_term.tunnel_id;
    return rv;
}

int
vxlan_network_domain_update(
    int unit,
    int update_value)
{
    int rv;
    rv = bcm_port_class_set (unit, cint_vxlan_tunnel_basic_info.tunnel_term_id, bcmPortClassIngress, update_value);
    return rv;
}

/* for cases where we don't want to fwd according to vsi */
int 
vxlan_vni_to_invalid_vsi(
    int unit, 
    int network_domain) 
{
    int rv;


    bcm_vxlan_network_domain_config_t config; 
    bcm_vxlan_network_domain_config_t_init(&config);
    config.vni = cint_vxlan_tunnel_basic_info.vni; 
    config.network_domain = network_domain; 
    config.vsi = 0;          
    config.flags = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_REPLACE;
    rv = bcm_vxlan_network_domain_config_add(unit, config); 
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vxlan_network_domain_config_add \n");
        return rv;
    }
    return rv;
}

int
vxlan_network_domain_update_egress(
    int unit,
    int update_value)
{
    int rv;
    rv = bcm_port_class_set (unit, cint_vxlan_tunnel_basic_info.tunnel_encapsulation_id, bcmPortClassEgress, update_value);
    return rv;
}

/* replace vni*network domain -> invalid vsi with valid vsi. */
int
vxlan_network_domain_management_update(
    int unit,
    int out_port1,
    int out_port2)
{
    int rv;
    int flags;
    bcm_multicast_replication_t rep_array;
    bcm_l2_addr_t l2addr2;

    bcm_vxlan_vpn_config_t vpn_config;
    bcm_vxlan_vpn_config_t_init(&vpn_config);
    vpn_config.flags = BCM_VXLAN_VPN_WITH_ID | BCM_VXLAN_VPN_WITH_VPNID;
    vpn_config.broadcast_group = cint_vxlan_tunnel_basic_info.vpn_id_2;
    vpn_config.vpn = cint_vxlan_tunnel_basic_info.vpn_id_2;
    vpn_config.vnid = BCM_VXLAN_VNI_INVALID;
    vpn_config.unknown_unicast_group = cint_vxlan_tunnel_basic_info.vpn_id_2;
    vpn_config.unknown_multicast_group = cint_vxlan_tunnel_basic_info.vpn_id_2;
    rv = bcm_vxlan_vpn_create(unit, &vpn_config);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error in bcm_vxlan_vpn_create \n", proc_name);
        return rv;
    }

    bcm_vxlan_network_domain_config_t config;
    bcm_vxlan_network_domain_config_t_init(&config);
    /*
     * Update network domain for the created VSI->VNI
     */
    config.flags            = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING;
    config.vsi              = cint_vxlan_tunnel_basic_info.vpn_id_2;
    config.vni              = cint_vxlan_tunnel_basic_info.vni;
    config.network_domain   = cint_vxlan_tunnel_basic_info.network_domain; /** VNI2VSI mapping created with vxlan_vpn_create has always network_domain 0*/

    rv = bcm_vxlan_network_domain_config_add(unit, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vxlan_network_domain_config_add \n");
        return rv;
    }

    flags = (BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP);
    rv = bcm_multicast_create(unit, flags, cint_vxlan_tunnel_basic_info.vpn_id_2);

    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_multicast_create\n");
        return rv;
    }

    flags = BCM_MULTICAST_INGRESS_GROUP;  
    bcm_multicast_replication_t_init(&rep_array);
    rep_array.port = out_port1;
    rv = bcm_multicast_add(unit, cint_vxlan_tunnel_basic_info.vpn_id_2, flags, 1, &rep_array);

    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_multicast_add\n");
        return rv;
    }

    rep_array.port = out_port2;
    rv = bcm_multicast_add(unit, cint_vxlan_tunnel_basic_info.vpn_id_2, flags, 1, &rep_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_multicast_add in Port= Port_2\n");
        return rv;
    }

    bcm_l2_addr_t_init(&l2addr2, cint_vxlan_tunnel_basic_info.virtual_machine_da, cint_vxlan_tunnel_basic_info.vpn_id_2);
    l2addr2.mac        = cint_vxlan_tunnel_basic_info.virtual_machine_da;
    l2addr2.vid        = cint_vxlan_tunnel_basic_info.vpn_id_2;
    l2addr2.l2mc_group = cint_vxlan_tunnel_basic_info.vpn_id_2;
    l2addr2.flags      = BCM_L2_MCAST;
    rv = bcm_l2_addr_add(unit, &l2addr2);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, bcm_l2_addr_add\n", proc_name);
        return rv;
    }
    return rv;
}

int
vxlan_network_domain_management_update_egress(
    int unit)
{
    int rv;
    int flags;
    bcm_vxlan_network_domain_config_t config;
    bcm_vxlan_network_domain_config_t_init(&config);
    /*
     * Update network domain for the created VSI->VNI
     */
    config.flags            = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING;
    config.vsi              = cint_vxlan_tunnel_basic_info.vpn_id;
    config.vni              = cint_vxlan_tunnel_basic_info.vni;
    config.network_domain   = cint_vxlan_tunnel_basic_info.network_domain_2; /** VNI2VSI mapping created with vxlan_vpn_create has always network_domain 0*/

    rv = bcm_vxlan_network_domain_config_add(unit, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vxlan_network_domain_config_add \n");
        return rv;
    }
    return rv;
}

int
vxlan_gpe_tunnel_l2_network_domain_management(
    int unit,
    int core_port,
    int virtual_machine_port)
{

    int rv;
    char *proc_name;
    bcm_if_t encap_tunnel_intf;
    int term_tunnel_id;
    int encap_tunnel_id;

    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);

    bcm_gport_t vlan_port_id;
    bcm_gport_t vxlan_port_id;

    proc_name = "vxlan_gpe_tunnel_l2_network_domain_management";


    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_vxlan_tunnel_basic_info.fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    bcm_vxlan_vpn_config_t vpn_config;
    bcm_vxlan_vpn_config_t_init(&vpn_config);
    vpn_config.flags = BCM_VXLAN_VPN_WITH_ID | BCM_VXLAN_VPN_WITH_VPNID;
    vpn_config.broadcast_group = cint_vxlan_tunnel_basic_info.vpn_id;
    vpn_config.vpn = cint_vxlan_tunnel_basic_info.vpn_id;
    vpn_config.vnid = BCM_VXLAN_VNI_INVALID;
    vpn_config.unknown_unicast_group = cint_vxlan_tunnel_basic_info.vpn_id;
    vpn_config.unknown_multicast_group = cint_vxlan_tunnel_basic_info.vpn_id;
    rv = bcm_vxlan_vpn_create(unit, &vpn_config);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error in bcm_vxlan_vpn_create \n", proc_name);
        return rv;
    }
    bcm_vxlan_network_domain_config_t config;
    bcm_vxlan_network_domain_config_t_init(&config);
    /*
     * Update network domain for the created VNI->VSI
     */
    config.flags            = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING ;
    config.vsi              = cint_vxlan_tunnel_basic_info.vpn_id;
    config.vni              = cint_vxlan_tunnel_basic_info.vni;
    config.network_domain   = 0;
    rv = bcm_vxlan_network_domain_config_add(unit, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vxlan_network_domain_config_add \n");
        return rv;
    }

    /*
     * Update network domain for the created VSI->VNI
     */
    config.flags            = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING;
    config.vsi              = cint_vxlan_tunnel_basic_info.vpn_id;
    config.vni              = cint_vxlan_tunnel_basic_info.vni;
    config.network_domain   = 0;
    rv = bcm_vxlan_network_domain_config_add(unit, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vxlan_network_domain_config_add \n");
        return rv;
    }
    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, core_port, cint_vxlan_tunnel_basic_info.eth_rif_intf_core);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set - core\n", proc_name);
        return rv;
    }

    /*
     * Set Out-Port default properties
     */
    rv = out_port_set(unit, virtual_machine_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set vm_port \n", proc_name);
        return rv;
    }

    /*
     * Create Routing interface for the tunnel's DIP. We will use it as ingress ETH-RIF to perform
     * LL termination (my-mac procedure), to enable the IPv4 routing for this ETH-RIF and to set the VRF.
     */
    rv = intf_eth_rif_create(unit, cint_vxlan_tunnel_basic_info.eth_rif_intf_core,
                             cint_vxlan_tunnel_basic_info.core_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create eth_rif_intf_core \n", proc_name);
        return rv;
    }

    /*
     * Set Incoming ETH-RIF properties, VRF is updated for the rif.
     * This VRF value is used for IP-Tunnel-Term.VRF lookup key.
     */
    ingr_itf.intf_id = cint_vxlan_tunnel_basic_info.eth_rif_intf_core;
    ingr_itf.vrf = cint_vxlan_tunnel_basic_info.eth_rif_core_vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_ingress_rif_set intf_out \n", proc_name);
        return rv;
    }

    /*
     * Create the tunnel termination lookup and the tunnel termination inlif
     */
    rv = vxlan_tunnel_terminator_create(unit, &term_tunnel_id, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function vxlan_tunnel_terminator_create \n", proc_name);
        return rv;
    }

    /** Create ARP entry for VXLAN tunnel and set ARP properties. */
    rv = l3__egress_only_encap__create(unit, BCM_L3_EGRESS_ONLY, &(cint_vxlan_tunnel_basic_info.arp_id), 
                                        cint_vxlan_tunnel_basic_info.virtual_machine_mac_address, cint_vxlan_tunnel_basic_info.virtual_machine_vlan);
    if(rv != BCM_E_NONE)
    {
        printf("%s(): Error, l3_egress_only_encap__create\n", proc_name);
        return rv;
    }

    /*
     * Create the tunnel initiator
     */
    rv = vxlan_tunnel_initiator_create(unit, cint_vxlan_tunnel_basic_info.arp_id, &encap_tunnel_intf, &encap_tunnel_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function tunnel_initiator_configure \n", proc_name);
        return rv;
    }

    /** Create FEC and configure its: Outlif, Destination port, Global Out-EthRIF */
    rv = l3__egress_only_fec__create(unit, cint_vxlan_tunnel_basic_info.fec_id, encap_tunnel_intf, 0, virtual_machine_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, l3__egress_only_fec__create fec_id=0x%x\n", proc_name,
               cint_vxlan_tunnel_basic_info.tunnel_fec_id);
        return rv;
    }

    /*
     * Add VXLAN port properties and configure the tunnel terminator In-Lif and tunnel initiatior Out-Lif
     */
    rv = vxlan_port_add(unit, core_port, term_tunnel_id, encap_tunnel_id, &vxlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_port_add \n", proc_name);
    }

    /*
     * Create an MACT entry for VXLAN tunnel, mapped to the VPN
     */
    rv = mact_entry_add(unit, virtual_machine_port, cint_vxlan_tunnel_basic_info.virtual_machine_da);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, mact_entry_add\n", proc_name);
        return rv;
    }

    /*
     * Create an MACT entry for VXLAN tunnel, mapped to the VPN
     */
    rv = mact_entry_add(unit, vxlan_port_id, cint_vxlan_tunnel_basic_info.virtual_machine_da_encap);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, mact_entry_add\n", proc_name);
        return rv;
    }

    return rv;
}

int
vxlan_network_domain_management_l3(
    int unit,
    int network_domain)
{
    int rv;
    bcm_vxlan_network_domain_config_t config;
    bcm_vxlan_network_domain_config_t_init(&config);
    /*
     * Update network domain for the created VNI->VRF
     */
    config.flags            = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L3_MAPPING;
    config.vrf              = cint_vxlan_tunnel_basic_info.access_vrf;
    config.vni              = cint_vxlan_tunnel_basic_info.vni;
    config.network_domain   = network_domain;

    rv = bcm_vxlan_network_domain_config_add(unit, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vxlan_network_domain_config_add \n");
        return rv;
    }
    return rv;
}

int
vxlan_network_domain_management_l3_egress(
    int unit,
    int network_domain)
{
    int rv;
    bcm_vxlan_network_domain_config_t config;
    bcm_vxlan_network_domain_config_t_init(&config);
    /*
     * Update network domain for the created VRF->VNI
     */
    config.flags            = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L3_MAPPING;
    config.vrf              = cint_vxlan_tunnel_basic_info.access_vrf;
    config.vni              = cint_vxlan_tunnel_basic_info.vni;
    config.network_domain   = network_domain;

    rv = bcm_vxlan_network_domain_config_add(unit, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vxlan_network_domain_config_add \n");
        return rv;
    }
    return rv;
}

int
vxlan_gpe_tunnel_l3_network_domain_management(
    int unit,
    int core_port,
    int virtual_machine_port)
{

    int rv;
    char *proc_name;
    bcm_if_t encap_tunnel_intf;
    int term_tunnel_id;
    int encap_tunnel_id;

    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);
    bcm_gport_t vxlan_port_id;

    proc_name = "vxlan_gpe_tunnel_l3_network_domain_management";
    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, core_port, cint_vxlan_tunnel_basic_info.eth_rif_intf_core);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set - core\n", proc_name);
        return rv;
    }

    /*
     * Set Out-Port default properties
     */
    rv = out_port_set(unit, virtual_machine_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set vm_port \n", proc_name);
        return rv;
    }

    /*
     * Create Routing interface for the tunnel's DIP. We will use it as ingress ETH-RIF to perform
     * LL termination (my-mac procedure), to enable the IPv4 routing for this ETH-RIF and to set the VRF.
     */
    rv = intf_eth_rif_create(unit, cint_vxlan_tunnel_basic_info.eth_rif_intf_core,
                             cint_vxlan_tunnel_basic_info.core_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create eth_rif_intf_core \n", proc_name);
        return rv;
    }

    /*
     * Create egress routing interface used for routing after the tunnel termination.
     * We are using it as egress ETH-RIF, providing the link layer SA.
     */
    rv = intf_eth_rif_create(unit, cint_vxlan_tunnel_basic_info.eth_rif_intf_virtual_machine, cint_vxlan_tunnel_basic_info.intf_access_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create, eth_rif_intf_virtual_machine\n", proc_name);
        return rv;
    }

    /** Create ARP entry for VXLAN tunnel and set ARP properties. */
    rv = l3__egress_only_encap__create(unit, BCM_L3_EGRESS_ONLY, &(cint_vxlan_tunnel_basic_info.arp_id), 
                                        cint_vxlan_tunnel_basic_info.virtual_machine_mac_address, cint_vxlan_tunnel_basic_info.eth_rif_intf_virtual_machine);
    if(rv != BCM_E_NONE)
    {
        printf("%s(): Error, l3_egress_only_encap__create\n", proc_name);
        return rv;
    }

    /*
     * Set Incoming ETH-RIF properties, VRF is updated for the rif.
     * This VRF value is used for IP-Tunnel-Term.VRF lookup key.
     */
    ingr_itf.intf_id = cint_vxlan_tunnel_basic_info.eth_rif_intf_core;
    ingr_itf.vrf = cint_vxlan_tunnel_basic_info.eth_rif_core_vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_ingress_rif_set intf_out \n", proc_name);
        return rv;
    }

    /*
     * Create the tunnel termination lookup and the tunnel termination inlif
     */
    rv = vxlan_tunnel_terminator_create(unit, &term_tunnel_id, 1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function vxlan_tunnel_terminator_create \n", proc_name);
        return rv;
    }

    /** Create ARP entry for VXLAN tunnel and set ARP properties. */
    rv = l3__egress_only_encap__create(unit, BCM_L3_EGRESS_ONLY, &(cint_vxlan_tunnel_basic_info.arp_id_2), 
                                        cint_vxlan_tunnel_basic_info.virtual_machine_da_encap, cint_vxlan_tunnel_basic_info.virtual_machine_vlan_2);
    if(rv != BCM_E_NONE)
    {
        printf("%s(): Error, l3_egress_only_encap__create\n", proc_name);
        return rv;
    }

    /*
     * Create the tunnel initiator
     */
    rv = vxlan_tunnel_initiator_create(unit, cint_vxlan_tunnel_basic_info.arp_id_2, &encap_tunnel_intf, &encap_tunnel_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function tunnel_initiator_configure \n", proc_name);
        return rv;
    }

    /** Create FEC and configure its: Outlif, Destination port, Global Out-EthRIF */
    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_vxlan_tunnel_basic_info.fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    rv = l3__egress_only_fec__create(unit, cint_vxlan_tunnel_basic_info.fec_id, cint_vxlan_tunnel_basic_info.eth_rif_intf_virtual_machine,
                                     cint_vxlan_tunnel_basic_info.arp_id, virtual_machine_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, l3__egress_only_fec__create fec_id=0x%x\n", proc_name,
               cint_vxlan_tunnel_basic_info.tunnel_fec_id);
        return rv;
    }

    /** Create FEC and configure its: Outlif, Destination port, Global Out-EthRIF */
    cint_vxlan_tunnel_basic_info.fec_id_2 = cint_vxlan_tunnel_basic_info.fec_id + 1;
    rv = l3__egress_only_fec__create(unit, cint_vxlan_tunnel_basic_info.fec_id_2, encap_tunnel_id,
                                     0, virtual_machine_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, l3__egress_only_fec__create fec_id=0x%x\n", proc_name,
               cint_vxlan_tunnel_basic_info.tunnel_fec_id);
        return rv;
    }

    rv = add_route_ipv4(unit, cint_vxlan_tunnel_basic_info.dip_route, cint_vxlan_tunnel_basic_info.dip_route_mask,
                        cint_vxlan_tunnel_basic_info.access_vrf, cint_vxlan_tunnel_basic_info.fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, add_route_ipv4", proc_name);
        return rv;
    }

    rv = add_route_ipv4(unit, cint_vxlan_tunnel_basic_info.dip_route_encap, cint_vxlan_tunnel_basic_info.dip_route_mask,
                        cint_vxlan_tunnel_basic_info.access_vrf, cint_vxlan_tunnel_basic_info.fec_id_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, add_route_ipv4", proc_name);
        return rv;
    }

    rv = vxlan_network_domain_management_l3 (unit, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_network_domain_management_l3", proc_name);
        return rv;
    }
    return rv;
}


int
vxlan_network_domain_testing(
    int unit)
{

    bcm_vxlan_vpn_config_t vxlan_vpn_create, vxlan_vpn_get;
    bcm_vxlan_network_domain_config_t vxlan_vpn_config, vxlan_vpn_config_get;

    int flags=BCM_VXLAN_VPN_WITH_VPNID | BCM_VXLAN_VPN_WITH_ID;

    int vnid=500;
    int vpn=4000;

    int bc_group = vpn;
    int uuc_group= vpn;
    int umc_group =vpn;
    int rv =0;

    bcm_vxlan_vpn_config_t_init (vxlan_vpn_create);
    bcm_vxlan_vpn_config_t_init (vxlan_vpn_get);
    vxlan_vpn_create.flags  = flags;
    vxlan_vpn_create.vpn = vpn;
    vxlan_vpn_create.vnid = BCM_VXLAN_VNI_INVALID;
    vxlan_vpn_create.unknown_unicast_group = uuc_group;
    vxlan_vpn_create.unknown_multicast_group = umc_group;
    vxlan_vpn_create.broadcast_group = bc_group;
    rv = bcm_vxlan_vpn_create(unit, &vxlan_vpn_create);

    bcm_vxlan_network_domain_config_t_init (vxlan_vpn_config);
    vxlan_vpn_config.network_domain  = 0;
    vxlan_vpn_config.vni = vnid;
    vxlan_vpn_config.vsi = vpn;
    vxlan_vpn_config.flags = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING;

    printf("Step 1: Create VNI2VSI mapping with bcm_vxlan_network_domain_config_add API\n");
    rv = bcm_vxlan_network_domain_config_add(unit, &vxlan_vpn_config);

    bcm_vxlan_network_domain_config_t_init (vxlan_vpn_config_get);
    vxlan_vpn_config_get.flags  = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING;
    vxlan_vpn_config_get.network_domain = 0;
    vxlan_vpn_config_get.vni = vnid;

    printf("Step 2: Get the VNI2VSI mapping with bcm_vxlan_network_domain_config_get API\n");
    rv = bcm_vxlan_network_domain_config_get (unit, &vxlan_vpn_config_get);

    printf("Step 3: Compare the returned VSI value\n");
    if (vxlan_vpn_config_get.vsi != vxlan_vpn_config.vsi)
    {
        printf(" Mismatch in configured vsi %d, and get VSI %d \n", vxlan_vpn_config.vsi , vxlan_vpn_config_get.vsi);
        return BCM_E_PARAM;
    }

    printf("Step 5: Try to create the same mapping with bcm_vxlan_vpn_create API\n");
    rv = bcm_vxlan_vpn_create(unit, &vxlan_vpn_create);
    if (rv != BCM_E_EXISTS)
    {
        printf("Error in bcm_vxlan_vpn_create, entry should exist already");
        return BCM_E_EXISTS;
    }

    printf("Step 6: Try to get the VNI2VSI mapping with bcm_vxlan_vpn_get API\n");
    rv = bcm_vxlan_vpn_get(unit, vpn, &vxlan_vpn_get);
    if (rv != BCM_E_PARAM)
    {
        printf("Error in bcm_vxlan_vpn_get, entry should be created from bcm_vxlan_network_domain_config_add API");
        return BCM_E_PARAM;
    }

    printf("Step 7: Try to remove the VNI2VSI mapping with bcm_vxlan_vpn_destroy API\n");
    rv = bcm_vxlan_vpn_destroy(unit, vpn);
    if (rv != BCM_E_NONE)
    {
        printf("The entry should not be removed from vpn_destory, only the mapping for the mc group");
        return BCM_E_NONE;
    }

    vxlan_vpn_config.vsi = vpn+1;
    vxlan_vpn_config.flags = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY |
            BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_REPLACE;

    printf("Step 8: Replace VNI2VSI mapping with bcm_vxlan_network_domain_config_add API\n");
    rv = bcm_vxlan_network_domain_config_add(unit, &vxlan_vpn_config);

    printf("Step 9: Remove VNI2VSI mapping with bcm_vxlan_network_domain_config_add API\n");
    rv = bcm_vxlan_network_domain_config_remove(unit, &vxlan_vpn_config);

    printf("Step 10: Try to create the mapping with bcm_vxlan_vpn_create API\n");
    vxlan_vpn_create.vpn = vpn+1;
    vxlan_vpn_create.vnid = vnid;
    rv = bcm_vxlan_vpn_create(unit, &vxlan_vpn_create);

    vxlan_vpn_config.vsi = vpn+1;
        vxlan_vpn_config.flags = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY |
                BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_REPLACE;
    printf("Step 11: Try to replace VNI2VSI mapping with bcm_vxlan_network_domain_config_add API\n");
    rv = bcm_vxlan_network_domain_config_add(unit, &vxlan_vpn_config);
    if (rv != BCM_E_PARAM)
    {
        printf("Error in bcm_vxlan_network_domain_config_add, entry should be created from bcm_vxlan_vpn_create API");
        return BCM_E_PARAM;
    }

    printf("Step 12: Try to delete VNI2VSI mapping with bcm_vxlan_network_domain_config_add API\n");
    rv = bcm_vxlan_network_domain_config_remove(unit, &vxlan_vpn_config);
    if (rv != BCM_E_PARAM)
    {
        printf("Error in bcm_vxlan_network_domain_config_remove, entry should be created from bcm_vxlan_vpn_create API");
        return BCM_E_PARAM;
    }

    printf("Step 13: Try to remove the mapping with bcm_vxlan_vpn_create API\n");
    rv = bcm_vxlan_vpn_destroy(unit, vpn+1);

    vxlan_vpn_config.network_domain  = 5;
    vxlan_vpn_config.vni = vnid+10;
    vxlan_vpn_config.vrf = vpn;
    vxlan_vpn_config.vsi = 0;
    vxlan_vpn_config.flags = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L3_MAPPING;

    printf("Step 14: Create VNI2VRF mapping with bcm_vxlan_network_domain_config_add API\n");
    rv = bcm_vxlan_network_domain_config_add(unit, &vxlan_vpn_config);

    vxlan_vpn_config.vrf = vpn+6;
    vxlan_vpn_config.flags = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY |
            BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L3_MAPPING | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_REPLACE;
    printf("Step 14: Replace VNI2VRF mapping with bcm_vxlan_network_domain_config_add API\n");
    rv = bcm_vxlan_network_domain_config_add(unit, &vxlan_vpn_config);

    vxlan_vpn_config_get.flags  = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L3_MAPPING;
    vxlan_vpn_config_get.network_domain = 5;
    vxlan_vpn_config_get.vni = vnid+10;
    printf("Step 15: Get the VNI2VRF mapping with bcm_vxlan_network_domain_config_get API\n");
    rv = bcm_vxlan_network_domain_config_get (unit, &vxlan_vpn_config_get);

    printf("Step 16: Compare the returned VSI value\n");
    if (vxlan_vpn_config_get.vrf != vxlan_vpn_config.vrf)
    {
        printf(" Mismatch in configured vrf %d, and get vrf %d \n", vxlan_vpn_config.vrf , vxlan_vpn_config_get.vrf);
        return BCM_E_PARAM;
    }
    printf("Step 17: Create VNI2VRF mapping with bcm_vxlan_network_domain_config_add API\n");
    rv = bcm_vxlan_network_domain_config_add(unit, &vxlan_vpn_config);

    printf("Step 18: Remove VNI2VRF mapping with bcm_vxlan_network_domain_config_add API\n");
    rv = bcm_vxlan_network_domain_config_remove(unit, &vxlan_vpn_config);
    return rv;
}

/*
 * IPv6 VXLAN-GPE L3 tunnel termination config with parameters:
 *  unit                 - relevant unit
 *  core_port            - port where core routers and aggregation switches are connected.
 *  virtual_machine_port - port where virtual machines are connected.
 */
int
vxlan6_tunnel_terminator_create(
    int unit,
    int *term_tunnel_id,
    int is_l3_case)
{
    int rv;
    bcm_tunnel_terminator_t tunnel_term;

    /*
     * Create IP tunnel terminator for SIP,DIP, VRF lookup in TCAM
     */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.dip6      = cint_vxlan_tunnel_basic_info.tunnel_ipv6_dip;
    tunnel_term.dip6_mask = cint_vxlan_tunnel_basic_info.tunnel_ipv6_dip_mask;
    tunnel_term.sip6      = cint_vxlan_tunnel_basic_info.tunnel_ipv6_sip;
    tunnel_term.sip6_mask = cint_vxlan_tunnel_basic_info.tunnel_ipv6_sip_mask;
    tunnel_term.vrf       = cint_vxlan_tunnel_basic_info.eth_rif_core_vrf;
    tunnel_term.type      = cint_vxlan_tunnel_basic_info.vxlan_tunnel_type;

    rv = bcm_tunnel_terminator_create(unit, &tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_terminator_create \n");
        return rv;
    }

    if (is_l3_case)
    {
        /** In JR2 VRF is a tunnel-LIF property set by bcm_l3_ingress_create API */
        bcm_l3_ingress_t ing_intf;
        bcm_l3_ingress_t_init(&ing_intf);
        ing_intf.flags = BCM_L3_INGRESS_WITH_ID; /* must, as we update exist RIF */
        ing_intf.vrf   = cint_vxlan_tunnel_basic_info.access_vrf+3;

        /** Convert tunnel's GPort ID to intf ID */
        bcm_if_t intf_id;
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(intf_id, tunnel_term.tunnel_id);

        rv = bcm_l3_ingress_create(unit, ing_intf, intf_id);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l3_ingress_create\n");
            return rv;
        }
    }
    *term_tunnel_id = tunnel_term.tunnel_id;
    cint_vxlan_tunnel_basic_info.tunnel_term_id = tunnel_term.tunnel_id;
    return rv;
}

/*
 * IPv6 VxLAN-GPE L3 tunnel termination example:
 * Tunnel Termination for IPv4oVXLAN-GPEoUDPoIPv6oE packet format.
 */
int
vxlan6_gpe_tunnel_termination_ipv4 (
    int unit,
    int core_port,
    int virtual_machine_port)
{

    int rv;
    char *proc_name;
    bcm_if_t encap_tunnel_intf;
    int term_tunnel_id;
    int encap_tunnel_id;

    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);
    bcm_gport_t vxlan_port_id;

    proc_name = "vxlan6_gpe_tunnel_termination_l3_network";
    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, core_port, cint_vxlan_tunnel_basic_info.eth_rif_intf_core);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set - core\n", proc_name);
        return rv;
    }

    /*
     * Set Out-Port default properties
     */
    rv = out_port_set(unit, virtual_machine_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set vm_port \n", proc_name);
        return rv;
    }

    /*
     * Create Routing interface for the tunnel's DIP. We will use it as ingress ETH-RIF to perform
     * LL termination (my-mac procedure), to enable the IPv4 routing for this ETH-RIF and to set the VRF.
     */
    rv = intf_eth_rif_create(unit, cint_vxlan_tunnel_basic_info.eth_rif_intf_core,
                             cint_vxlan_tunnel_basic_info.core_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create eth_rif_intf_core \n", proc_name);
        return rv;
    }

    /*
     * Create egress routing interface used for routing after the tunnel termination.
     * We are using it as egress ETH-RIF, providing the link layer SA.
     */
    rv = intf_eth_rif_create(unit, cint_vxlan_tunnel_basic_info.eth_rif_intf_virtual_machine, cint_vxlan_tunnel_basic_info.intf_access_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create, eth_rif_intf_virtual_machine\n", proc_name);
        return rv;
    }

    /** Create ARP entry for VXLAN tunnel and set ARP properties. */
    rv = l3__egress_only_encap__create(unit, BCM_L3_EGRESS_ONLY, &(cint_vxlan_tunnel_basic_info.arp_id),
                                        cint_vxlan_tunnel_basic_info.virtual_machine_mac_address, cint_vxlan_tunnel_basic_info.eth_rif_intf_virtual_machine);
    if(rv != BCM_E_NONE)
    {
        printf("%s(): Error, l3_egress_only_encap__create\n", proc_name);
        return rv;
    }

    /*
     * Set Incoming ETH-RIF properties, VRF is updated for the rif.
     * This VRF value is used for IP-Tunnel-Term.VRF lookup key.
     */
    ingr_itf.intf_id = cint_vxlan_tunnel_basic_info.eth_rif_intf_core;
    ingr_itf.vrf = cint_vxlan_tunnel_basic_info.eth_rif_core_vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_ingress_rif_set intf_out \n", proc_name);
        return rv;
    }

    /*
     * Create the tunnel termination lookup and the tunnel termination inlif
     */
    cint_vxlan_tunnel_basic_info.vxlan_tunnel_type = bcmTunnelTypeVxlan6Gpe;
    rv = vxlan_tunnel_terminator_create(unit, &term_tunnel_id, 1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function vxlan_tunnel_terminator_create \n", proc_name);
        return rv;
    }

    /** Create FEC and configure its: Outlif, Destination port, Global Out-EthRIF */
    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_vxlan_tunnel_basic_info.fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    rv = l3__egress_only_fec__create(unit, cint_vxlan_tunnel_basic_info.fec_id, cint_vxlan_tunnel_basic_info.eth_rif_intf_virtual_machine,
                                     cint_vxlan_tunnel_basic_info.arp_id, virtual_machine_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, l3__egress_only_fec__create fec_id=0x%x\n", proc_name,
               cint_vxlan_tunnel_basic_info.tunnel_fec_id);
        return rv;
    }
    /** only BCM_L3_INGRESS_WITH_ID is set*/
    if (cint_vxlan_tunnel_basic_info.flags_ing_intf == 1)
    {
        rv = add_route_ipv4(unit, cint_vxlan_tunnel_basic_info.dip_route, cint_vxlan_tunnel_basic_info.dip_route_mask,
                    cint_vxlan_tunnel_basic_info.access_vrf, cint_vxlan_tunnel_basic_info.fec_id);
    }
    else
    {
        rv = add_route_ipv4(unit, cint_vxlan_tunnel_basic_info.dip_route, cint_vxlan_tunnel_basic_info.dip_route_mask,
                        0, cint_vxlan_tunnel_basic_info.fec_id);
    }
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, add_route_ipv4", proc_name);
        return rv;
    }

    rv = vxlan_network_domain_management_l3 (unit, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_network_domain_management_l3", proc_name);
        return rv;
    }
    return rv;
}
