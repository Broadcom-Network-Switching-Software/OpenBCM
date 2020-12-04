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
 * cint ../../src/examples/sand/cint_vxlan_tunnel_termination_basic.c
 * cint
 * int unit = 0;
 * int rv = 0;
 * int core_port = 200;
 * int virtual_machine_port = 201;
 * rv = vxlan_tunnel_termination_basic(unit, core_port, virtual_machine_port);
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
 * VXLAN global structure
 */
struct cint_vxlan_tunnel_basic_info_s
{
    int vxlan_network_group_id; /* ingress and egress orientation for VXLAN */
    int inner_vlan;             /* Virtual machine VID */
    int eth_rif_intf_core;      /* Core RIF */
    int eth_rif_intf_virtual_machine;   /* Virtual Machine RIF */
    int eth_rif_virtual_machine_vrf;    /* router instance VRF resolved at VTT1 */
    int eth_rif_core_vrf;       /* VRF after termination,resolved as Tunnel-InLif property */
    bcm_mac_t core_mac_address; /* mac for next hop */
    bcm_mac_t virtual_machine_mac_address;      /* tunnel next hop MAC DA */
    bcm_mac_t virtual_machine_da;       /* Remote VM DA */
    bcm_ip_t tunnel_dip;        /* tunnel DIP */
    bcm_ip_t tunnel_dip_mask;   /* tunnel DIP mask */
    bcm_ip_t tunnel_sip;        /* tunnel SIP */
    bcm_ip_t tunnel_sip_mask;   /* tunnel SIP mask */
    int arp_id;                 /* Id for ARP entry */
    int fec_id;                 /* FEC id */
    bcm_gport_t tunnel_id;      /* Tunnel ID */
    int tunnel_if;              /* Tunnel Interface */
    int vpn_id;                 /* VPN ID */
    int vni;                    /* VNI of the VXLAN */
    bcm_gport_t vxlan_port_id;  /* vxlan gport */
};

cint_vxlan_tunnel_basic_info_s cint_vxlan_tunnel_basic_info = {
    /*
     * VXLAN Orientation, Inner VLAN
     */
    1, 510,
    /*
     * eth_rif_intf_core | eth_rif_intf_virtual_machine
     */
    15, 100,
    /*
     * virtual_machine VRF, Core VRF
     */
    5, 1,
    /*
     * core_mac_address | virtual_machine_mac_address | Virtual Machine DA
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}, {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01}, {0x00, 0x00, 0x00, 0x00, 0x00, 0xf1},
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
     * arp_id
     */
    0,
    /*
     * fec id | tunnel id | tunnel_if
     */
    5000, 20, 10,
    /*
     * VPN ID
     */
    15,
    /*
     * VNI
     */
    5000,
    /* vxlan port id */
    0
};
/*
 * The basic VXLAN example with parameters:
 * Unit - relevant unit
 * core_port - port where core routers and aggregation switches are connected.
 * vm_port - port where virtual machines are connected.
 */
int
vxlan_tunnel_termination_basic(
    int unit,
    int core_port,
    int virtual_machine_port)
{

    int rv;
    int device_is_jericho2;
    char *proc_name;
    bcm_if_t encap_tunnel_intf;
    int term_tunnel_id;
    int encap_tunnel_id;

    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);

    bcm_gport_t vlan_port_id;

    proc_name = "vxlan_tunnel_termination_basic";

    rv = is_device_jericho2(unit, &device_is_jericho2);
    /*
     * Initialize VXLAN module - used for J1 only
     */
    if (!device_is_jericho2)
    {
        rv = bcm_vxlan_init(unit);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, bcm_vxlan_init \n", proc_name);
            return rv;
        }
    }
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
    if (device_is_jericho2)
    {
        rv = in_port_intf_set(unit, core_port, cint_vxlan_tunnel_basic_info.eth_rif_intf_core);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in_port_intf_set - core\n", proc_name);
            return rv;
        }
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
    rv = intf_eth_rif_create(unit, cint_vxlan_tunnel_basic_info.eth_rif_intf_virtual_machine,
                             cint_vxlan_tunnel_basic_info.virtual_machine_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create eth_rif_intf_virtual_machine\n", proc_name);
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
     * In DPP (JR1) architecture IP-Tunnel LIF retrieve its RIF properties according to RIF-ID object (from tunnel_if).
     * While in DNX architecture (JR2) RIF properties are retrieved directly from IP-Tunnel LIF.
     */
    if (!device_is_jericho2)
    {
        /*
         * Create ingress Tunnel interface.
         * The function is used for Jericho 1 only.
         */
        rv = intf_eth_rif_create(unit, cint_vxlan_tunnel_basic_info.tunnel_if,
                                 cint_vxlan_tunnel_basic_info.core_mac_address);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, intf_eth_rif_create tunnel_if\n", proc_name);
            return rv;
        }
        /*
         * Set Incoming Tunnel-RIF properties
         */
        ingr_itf.intf_id = cint_vxlan_tunnel_basic_info.tunnel_if;
        ingr_itf.vrf = cint_vxlan_tunnel_basic_info.eth_rif_virtual_machine_vrf;
        rv = intf_ingress_rif_set(unit, &ingr_itf);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, intf_ingress_rif_set \n", proc_name);
            return rv;
        }
    }
    /*
     * Create the tunnel termination lookup and the tunnel termination inlif
     */
    rv = vxlan_tunnel_terminator_create(unit, &term_tunnel_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function vxlan_tunnel_terminator_create \n", proc_name);
        return rv;
    }

    if (device_is_jericho2)
    {
        cint_vxlan_tunnel_basic_info.fec_id = 40981;
    }
    /*
     * Create ARP entry and set its properties
     */
    rv = l3__egress_only_encap__create(unit, 0, &(cint_vxlan_tunnel_basic_info.arp_id),
                                       cint_vxlan_tunnel_basic_info.virtual_machine_mac_address,
                                       cint_vxlan_tunnel_basic_info.eth_rif_intf_virtual_machine);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only\n", proc_name);
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
     * Create FEC and configure its: Outlif (ARP), Destination port
     */
    if (is_device_or_above(unit, JERICHO2))
    {
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_vxlan_tunnel_basic_info.fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
    }
    rv = l3__egress_only_fec__create(unit, cint_vxlan_tunnel_basic_info.fec_id, encap_tunnel_intf,
                                     0, virtual_machine_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC only\n", proc_name);
        return rv;
    }

    /*
     * Add VXLAN port properties and configure the tunnel terminator In-Lif and tunnel initiatior Out-Lif
     */
    rv = vxlan_port_add(unit, core_port, term_tunnel_id, encap_tunnel_id, &cint_vxlan_tunnel_basic_info.vxlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_port_add \n", proc_name);
    }

    if (!device_is_jericho2)
    {
        /*
         * Configure the Outgoing port TPID properties.
         */
        port_tpid_init(virtual_machine_port, 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, port_tpid_set - virtual_machine\n", proc_name);
            return rv;
        }
        /*
         * Configure the Incomming port TPID properties.
         */
        port_tpid_init(core_port, 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, port_tpid_set - core\n", proc_name);
            return rv;
        }
    }

    rv = vlan_port_add(unit, virtual_machine_port, &vlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vlan_port_add \n", proc_name);
    }

    rv = vswitch_add_port(unit, cint_vxlan_tunnel_basic_info.vpn_id, virtual_machine_port, vlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vswitch_add_port\n", proc_name);
        return rv;
    }

    /*
     * Create an MACT entry for VXLAN tunnel, mapped to the VPN
     */
    rv = mact_entry_add(unit, virtual_machine_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, mact_entry_add\n", proc_name);
        return rv;
    }

    return rv;
}

int
mact_entry_add(
    int unit,
    int vxlan_port_id)
{
    int rv;
    bcm_l2_addr_t l2addr;

    bcm_l2_addr_t_init(&l2addr, cint_vxlan_tunnel_basic_info.virtual_machine_da, cint_vxlan_tunnel_basic_info.vpn_id);
    l2addr.port = vxlan_port_id;
    l2addr.mac = cint_vxlan_tunnel_basic_info.virtual_machine_da;
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
    int rv;
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;
    int device_is_jericho2;

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, is_device_jericho2 device_is_jericho2 \n");
        return rv;
    }
    vxlan_port_add_s vxlan_port_add;
    vxlan_port_s_clear(&vxlan_port_add);
    vxlan_port_add.vpn = cint_vxlan_tunnel_basic_info.vpn_id;
    vxlan_port_add.in_port = core_port;
    vxlan_port_add.in_tunnel = tunnel_term_id;
    vxlan_port_add.out_tunnel = tunnel_encapsulation_id;
    BCM_L3_ITF_SET(vxlan_port_add.egress_if, BCM_L3_ITF_TYPE_FEC, cint_vxlan_tunnel_basic_info.fec_id);
    vxlan_port_add.flags = 0;
    vxlan_port_add.network_group_id = cint_vxlan_tunnel_basic_info.vxlan_network_group_id;

    
    rv = vxlan__vxlan_port_add(unit, vxlan_port_add);
    *vxlan_port_id = vxlan_port_add.vxlan_port_id;

    return BCM_E_NONE;
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
    int device_is_jericho2;

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, is_device_jericho2 device_is_jericho2 \n");
        return rv;
    }

    bcm_l3_intf_t_init(&l3_intf);

    /*
     * Create IP tunnel initiator for encapsulating Vxlan tunnel header
     */
    bcm_tunnel_initiator_t_init(&tunnel_init);
    tunnel_init.dip = cint_vxlan_tunnel_basic_info.tunnel_dip;  /* default: 171.0.0.17 */
    tunnel_init.sip = cint_vxlan_tunnel_basic_info.tunnel_sip;  /* default: 160.0.0.1 */
    tunnel_init.flags = 0;
    tunnel_init.dscp = 10;      /* default: 10 */
    tunnel_init.type = bcmTunnelTypeVxlan;
    tunnel_init.l3_intf_id = arp_itf;
    if (device_is_jericho2)
    {
        tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    }
    else
    {
        tunnel_init.dscp_sel = bcmTunnelDscpAssign;
        tunnel_init.vlan = cint_vxlan_tunnel_basic_info.inner_vlan;     /* For JR2 the vlan is set at LL */
    }
    tunnel_init.ttl = 64;       /* default: 64 */
    tunnel_init.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE;   /* default: -1 */
    rv = bcm_tunnel_initiator_create(unit, &l3_intf, tunnel_init);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_initiator_create \n");
    }
    *tunnel_intf = l3_intf.l3a_intf_id;
    *encap_tunnel_id = tunnel_init.tunnel_id;
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
    int device_is_jericho2;
    char *proc_name;

    proc_name = "vxlan_open_vpn";
    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, is_device_jericho2 \n", proc_name);
        return rv;
    }

    if (!device_is_jericho2)
    {
        rv = multicast__open_mc_group(unit, &vpn_id, BCM_MULTICAST_TYPE_L2);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, multicast__open_mc_group\n", proc_name);
            return rv;
        }
    }
    bcm_vxlan_vpn_config_t vpn_config;
    bcm_vxlan_vpn_config_t_init(&vpn_config);
    vpn_config.flags = BCM_VXLAN_VPN_WITH_ID | BCM_VXLAN_VPN_WITH_VPNID;
    vpn_config.vpn = vpn_id;
    vpn_config.broadcast_group = vpn_id;
    vpn_config.unknown_unicast_group = vpn_id;
    vpn_config.unknown_multicast_group = vpn_id;
    vpn_config.vnid = vni;
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
    int *term_tunnel_id)
{
    int rv;
    bcm_tunnel_terminator_t tunnel_term;
    int device_is_jericho2;

    rv = is_device_jericho2(unit, &device_is_jericho2);

    /*
     * Create IP tunnel terminator for SIP,DIP, VRF lookup 
     */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.dip      = cint_vxlan_tunnel_basic_info.tunnel_dip;
    tunnel_term.dip_mask = cint_vxlan_tunnel_basic_info.tunnel_dip_mask;
    tunnel_term.sip      = cint_vxlan_tunnel_basic_info.tunnel_sip;
    tunnel_term.sip_mask = cint_vxlan_tunnel_basic_info.tunnel_sip_mask;
    tunnel_term.vrf      = cint_vxlan_tunnel_basic_info.eth_rif_core_vrf;
    tunnel_term.type     = bcmTunnelTypeVxlan;
    if (!device_is_jericho2)
    {
        tunnel_term.tunnel_if = cint_vxlan_tunnel_basic_info.tunnel_if;
    }

    rv = bcm_tunnel_terminator_create(unit, &tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_terminator_create \n");
        return rv;
    }

    *term_tunnel_id = tunnel_term.tunnel_id;

    return rv;
}

int
vlan_port_add(
    int unit,
    bcm_gport_t in_port,
    bcm_gport_t * port_id)
{
    int rv;
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);

    /*
     * the match criteria is port:1, out-vlan:510 
     */
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = in_port;
    vlan_port.match_vlan = cint_vxlan_tunnel_basic_info.inner_vlan;
    vlan_port.egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : cint_vxlan_tunnel_basic_info.inner_vlan;
    vlan_port.flags = 0;
    vlan_port.vsi = 0;
    rv = bcm_vlan_port_create(unit, &vlan_port);

    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }

    /*
     * handle of the created gport 
     */
    *port_id = vlan_port.vlan_port_id;

    return rv;
}

int
vswitch_add_port(
    int unit,
    bcm_vlan_t vsi,
    bcm_port_t phy_port,
    bcm_gport_t gport)
{
    int rv;
    int device_is_jericho2;
    rv = is_device_jericho2(unit, &device_is_jericho2);
    /*
     * add to vswitch 
     */
    rv = bcm_vswitch_port_add(unit, vsi, gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }
    if (!device_is_jericho2)
    {
        /*
         * update Multicast to have the added port 
         */
        rv = multicast__vlan_port_add(unit, vsi, phy_port, gport, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, multicast__vlan_port_add\n");
            return rv;
        }
    }

    return rv;
}
