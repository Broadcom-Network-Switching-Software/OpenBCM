/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * Example of a simple VXLAN test scenario
 * Test Scenario
 *
 * ./bcm.user
 *
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_vlan.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_multicast.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_vxlan.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/dpp/cint_port_tpid.c
 * cint ../../src/examples/dpp/cint_advanced_vlan_translation_mode.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_sand_ip_tunnel.c
 * cint ../../src/examples/sand/cint_sand_vxlan.c
 * cint
 * vxlan_example(0,200,203, 0);
 * exit;
 *
 * Terminate VXLAN tunnel by DIP, SIP, VRF, tunnel_type
 * tx 1 psrc=203 data=0x80c8000c0002000000000700010081000014080045000057000000008011ef83a0000001ab000011555555550043000008000000001388000000000000f0000c000200018100001effff
 * Received packets on unit 0 should be:
 * 0x0000000000F0000C000200018100001EFFFF
 *
 *  *  Traffic:
 *
 *  Send the following traffic:
 *
 *  ###############################################################################################################
 *  Sending packet from port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *      0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |  |                                 DA  (0C:00:02:00:00)                              |
 *   |  |-----------------------------------------------------------------------------------|    LL Ethernet
 *   |  |               DA                   |              SA  (00:07:00:01:00)            |
 *   |  |-----------------------------------------------------------------------------------|       Header
 *   |  |                                 SA                                                |
 *   |  |-----------------------------------------------------------------------------------|     ( 18 bytes)
 *   |  |                                VLAN (0x20)                                        |
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |       Ethertype = 0x0800(IPv4)     |                                              |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |     Version  |  IHL   |   ToS      |          Total Lenght                        |
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |        Identification              |   Flags   |  Fragment Offset                 |   Tunnel   IPv4
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |         TTL       |Protocol=17(UDP)|             Header Checksum                  |       Header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Source address ( 160.0.0.1 )                         |      (20 bytes)
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Destination address (171.0.0.17)                     |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |    Source port (0x5555)            |    Destination port (0x5555)                 |   UDP  Header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |    UDP lenght                      |    UDP checksum                              |    (8 bytes)
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  | | R | R |  Ver  | I | P | R | O |  |    Reserved      | Next Protocol             |  VXLAN  header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |    VXLAN Network Identifier (VNI) = 0x5000            | Reserved                  |    (8 bytes)
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |                               DA  (00:00:00:00:00:f0)                             |
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |               DA                   |              SA (00:0C:00:02:00:01)          |    Native
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                                 SA                                                |   Ethernet
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                                VLAN (0x30)                                        |    Header
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
 *   |  |                               DA   ( 00:00:00:00:00:f0)                           |    Native
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |               DA                    |             SA   (00:0C:00:02:00:01)        |
 *   |  |-----------------------------------------------------------------------------------|   Ethernet
 *   |  |                                 SA                                                |
 *   |  |-----------------------------------------------------------------------------------|    Header
 *   |  |                                VLAN  (0x30)                                       |  ( 18 bytes )
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
struct vxlan_s {
    int vpn_id;                 /* VPN ID */
    int vni;                    /* VNI of the VXLAN */
    int vxlan_network_group_id; /* ingress and egress orientation for VXLAN */
    int vm_vlan;                /* Virtual machine VID */
    int core_vlan;              /* VID of the outer ETH header */
    bcm_mac_t vm1_da;           /* Remote VM DA */
    bcm_mac_t vm2_da;           /* Remote VM DA */
    int eth_rif_intf_core;      /* Core RIF */
    int eth_rif_core_vrf;       /* VRF - a router instance for the tunnels's DIP */
    int eth_rif_vm_vrf;     /* VRF after termination,resolved as Tunnel-InLif property */
    bcm_mac_t intf_core_mac_addr; /* mac for next hop */
    bcm_mac_t tunnel_next_hop_mac; /* tunnel next hop MAC DA */
    bcm_ip_t tunnel_dip;        /* tunnel DIP */
    bcm_ip_t tunnel_dip_mask;   /* tunnel DIP mask */
    bcm_ip_t tunnel_sip;        /* tunnel SIP */
    bcm_ip_t tunnel_sip_mask;   /* tunnel SIP mask*/
    int tunnel_arp_id;          /* Id for ARP entry */
    int tunnel_fec_id;          /* FEC id */
    bcm_gport_t tunnel_id_init;      /* Tunnel init ID */
    bcm_ip_t route_dip;         /* Route DIP */
    uint32 route_dip_mask;      /* mask for route DIP */
    bcm_gport_t tunnel_id_term; /* Tunnel term ID */ 
};

vxlan_s g_vxlan = {
/* vpn_id     | vni */
     15,           5000,
/* vxlan orientation */
      1,
/* vm VID   | core VID*/
    510,           100,
/* VM1 DA */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0xf0},
/* VM2 DA */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0xf1},
/* In RIF */
    15,
/* VRF        |  VRF after termination */
    1,           5,
/* intf_core_mac_address */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00},
/* tunnel next hop MAC DA */
    {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d},
/* tunnel DIP - 171.0.0.17    |  tunnel DIP mask */
    0xAB000011,                      0xffffffff,
/* tunnel SIP - 160.0.0.1     |  tunnel SIP mask */
    0xA0000001,                      0xffffffff,
/* tunnel_arp_id | tunnel_fec_id   | tunnel_id_init */
    0,             5000,              0,
/* route_dip */
    0x7fffff03 /* 127.255.255.03 */,
/* route_dip_mask */
    0xfffffff0, 
/* tunnel_id_term */
    0
};


void vxlan_init(int unit, vxlan_s *param) {
    if (param != NULL) {
        sal_memcpy(&g_vxlan, param, sizeof(g_vxlan));
    }
}

void vxlan_struct_get(vxlan_s *param) {
    sal_memcpy( param, &g_vxlan, sizeof(g_vxlan));
}

/* Create and configure an VPN. */
int vxlan_open_vpn(int unit, int vpn, int vni){
    int rv = BCM_E_NONE;
    int device_is_jericho2;
    char *proc_name;
    int flags;

    proc_name = "vxlan_open_vpn";
    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, is_device_jericho2 \n", proc_name);
        return rv;
    }

    if(!device_is_jericho2) {

        rv = multicast__open_mc_group(unit, &vpn, BCM_MULTICAST_TYPE_L2);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error, multicast__open_mc_group\n", proc_name);
            return rv;
        }
    }

    bcm_vxlan_vpn_config_t vpn_config;
    bcm_vxlan_vpn_config_t_init(&vpn_config);
    vpn_config.flags = BCM_VXLAN_VPN_WITH_ID|BCM_VXLAN_VPN_WITH_VPNID;
    vpn_config.vpn = vpn;
    vpn_config.broadcast_group = vpn;
    vpn_config.unknown_unicast_group = vpn;
    vpn_config.unknown_multicast_group = vpn;
    vpn_config.vnid = vni;
    rv = bcm_vxlan_vpn_create(unit,&vpn_config);
    if(rv != BCM_E_NONE) {
        printf("%s(): error in bcm_vxlan_vpn_create \n", proc_name);
        return rv;
    }

    return rv;
}

int vlan_port_add(int unit,  bcm_gport_t in_port, bcm_gport_t *port_id){
    int rv;
    bcm_vlan_port_t vp1;
    bcm_vlan_port_t_init(&vp1);

    /* the match criteria is port:1, out-vlan:510   */
    vp1.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp1.port = in_port;
    vp1.match_vlan = g_vxlan.vm_vlan;
    vp1.egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : g_vxlan.vm_vlan;
    vp1.flags = 0;
    vp1.vsi = 0;
    rv = bcm_vlan_port_create(unit,&vp1);

    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }

    /* handle of the created gport */
    *port_id = vp1.vlan_port_id;

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
    return BCM_E_NONE;
}



int tunnel_terminator_create(int unit, int is_vxlan_gpe)
{
    int rv;
    bcm_tunnel_terminator_t tunnel_term;
    int device_is_jericho2;

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, tunnel_terminator_create device_is_jericho2 \n");
        return rv;
    }

    /* Create IP tunnel terminator for SIP,DIP, VRF lookup */
    bcm_tunnel_terminator_t_init(&tunnel_term);

    tunnel_term.dip      = g_vxlan.tunnel_dip;         /* default: 171.0.0.17 */
    tunnel_term.dip_mask = g_vxlan.tunnel_dip_mask;    /* default: 255.255.255.255 */
    tunnel_term.sip      = g_vxlan.tunnel_sip;         /* default: 160.0.0.1 */
    tunnel_term.sip_mask = g_vxlan.tunnel_sip_mask;    /* default: 255.255.255.255 */
    tunnel_term.type     = is_vxlan_gpe ? bcmTunnelTypeVxlanGpe: bcmTunnelTypeVxlan;
    tunnel_term.vrf      = g_vxlan.eth_rif_core_vrf;

    if(!device_is_jericho2) {
        tunnel_term.tunnel_if = 10;
    }

    rv = bcm_tunnel_terminator_create(unit,&tunnel_term);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_terminator_create \n");
        return rv;
    }
    g_vxlan.tunnel_id_term = tunnel_term.tunnel_id;

    return rv;
}



/*
 * The basic VXLAN example with parameters:
 * Unit - relevant unit
 * core_port - port where core routers and aggregation switches are connected.
 * vm_port - port where virtual machines are connected.
 * is_vxlan_gpe - indicates vxlan or vxlan_gpe
 */
int vxlan_example(int unit, int core_port, int vm_port, int is_vxlan_gpe){

    bcm_gport_t vlan_port_id;
    bcm_gport_t vxlan_port_id;
    int rv;
    int device_is_jericho2;
    bcm_if_t tunnel_out_rif_intf;
    char *proc_name;
    int dummy_arp = 0;
    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);

    proc_name = "vxlan_example";

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, is_device_jericho2 \n", proc_name);
        return rv;
    }

    /* init L2 VXLAN module */
    /* Needed only for JER1 */
    if(!device_is_jericho2){
        rv = bcm_vxlan_init(unit);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error, bcm_switch_control_set \n", proc_name);
            return rv;
        }
    }

    /* build L2 VPN */
    rv = vxlan_open_vpn(unit,g_vxlan.vpn_id,g_vxlan.vni);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, vxlan_open_vpn, vpn=%d, \n", proc_name, g_vxlan.vpn_id);
        return rv;
    }

    /*
     *  Set In-Port to In ETh-RIF
     *  for incoming port, you configure default VSI.
     */
    rv = in_port_intf_set(unit, core_port, g_vxlan.eth_rif_intf_core);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n", proc_name);
        return rv;
    }

    /* Set Out-Port default properties */
    rv = out_port_set(unit, vm_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set vm_port \n", proc_name);
        return rv;
    }

    /*** build tunnel terminations and router interfaces ***/
    /*
     * Create Routing interface for the tunnel's DIP. We will use it as ingress ETH-RIF to perform
     * LL termination (my-mac procedure), to enable the IPv4 routing for this ETH-RIF and to set the VRF.
     */
    rv = intf_eth_rif_create(unit, g_vxlan.eth_rif_intf_core, g_vxlan.intf_core_mac_addr);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create eth_rif_intf_core \n", proc_name);
        return rv;
    }

    /* Set Incoming ETH-RIF properties for the tunnel's DIP.*/
    ingr_itf.intf_id = g_vxlan.eth_rif_intf_core;
    ingr_itf.vrf = g_vxlan.eth_rif_core_vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_ingress_rif_set intf_out \n", proc_name);
        return rv;
    }

    /* Needed only for JER1 */
    if(!device_is_jericho2)
    {

        int tunnel_if = 20; /* The RIF property of the InLif used only for JR1 */
        /* Create ingress Tunnel interface. */
        rv = intf_eth_rif_create(unit,tunnel_if, g_vxlan.intf_core_mac_addr);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, intf_eth_rif_create tunnel_if 2 \n", proc_name);
            return rv;
        }

        /* Set ingress Tunnel interface properties. */
        ingr_itf.intf_id = tunnel_if;
        ingr_itf.vrf = g_vxlan.eth_rif_vm_vrf;
        rv = intf_ingress_rif_set(unit, &ingr_itf);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, intf_ingress_rif_set intf_out \n", proc_name);
            return rv;
        }
    }


    /* Create the tunnel termination lookup and the tunnel termination In-Lif */
    rv = tunnel_terminator_create(unit, is_vxlan_gpe);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, tunnel_terminator_create \n", proc_name);
        return rv;
    }
    if (device_is_jericho2)
    {
        g_vxlan.tunnel_fec_id = 35001;
    }
    /*
     * Create ARP entry for VXLAN tunnel and set ARP properties. 
     */
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid;

    bcm_l3_egress_t_init(&l3eg);

    l3eg.mac_addr = g_vxlan.tunnel_next_hop_mac;
    l3eg.encap_id = g_vxlan.tunnel_arp_id;
    l3eg.vlan = g_vxlan.core_vlan;
    l3eg.intf = tunnel_out_rif_intf;
    rv = bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg, &l3egid);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object, \n", proc_name);
        return rv;
    }

    /*
     * Create the tunnel initiator
     */
    rv = tunnel_initiator_create(unit, l3eg.encap_id, &tunnel_out_rif_intf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function tunnel_initiator_configure \n", proc_name);
        return rv;
    }

    /*
     * Create FEC and configure its: Outlif, Destination port, Global Out-EthRIF
     */
    rv = l3__egress_only_fec__create(unit, g_vxlan.tunnel_fec_id, tunnel_out_rif_intf, 0, core_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC only\n", proc_name);
    }

    printf("vxlan tunnel fec_id: 0x%x\n", g_vxlan.tunnel_fec_id);


    /* Create ARP entry and set its properties. */
    
    rv = l3__egress_only_encap__create(unit, 0,&dummy_arp, g_vxlan.vm1_da, 0);
    if(rv != BCM_E_NONE) {
        printf("%s(): Error, l3_egress_only_encap__create \n", proc_name);
        return rv;
    }

    int vxlan_port_flags = 0;
    /* Add VXLAN port properties and configure the tunnel terminator In-Lif and tunnel initiatior Out-Lif*/
    vxlan_port_add_s vxlan_port_add;
    vxlan_port_s_clear(&vxlan_port_add);
    vxlan_port_add.vpn = g_vxlan.vpn_id;
    vxlan_port_add.in_port = core_port;
    vxlan_port_add.in_tunnel = g_vxlan.tunnel_id_term;
    vxlan_port_add.out_tunnel = g_vxlan.tunnel_id_init;
    BCM_L3_ITF_SET(vxlan_port_add.egress_if, BCM_L3_ITF_TYPE_FEC, g_vxlan.tunnel_fec_id); 
    vxlan_port_add.flags = vxlan_port_flags;
    vxlan_port_add.network_group_id = g_vxlan.vxlan_network_group_id;

    printf("Before vxlan port add: update vxlan inlif and outlif \n");

    
    vxlan__vxlan_port_add(unit, vxlan_port_add);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, vxlan__vxlan_port_add, in_gport=0x%08x \n", proc_name, g_vxlan.tunnel_id_term);
        return rv;
    }

    vxlan_port_id = vxlan_port_add.vxlan_port_id;

    if (!device_is_jericho2) {

        /* Configure the Outgoing port TPID properties. */
        port_tpid_init(vm_port, 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error, port_tpid_set \n", proc_name);
            return rv;
        }

        /* Configure the Incomming port TPID properties. */
        port_tpid_init(core_port, 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error, port_tpid_set \n", proc_name);
            return rv;
        }
    }

    rv = vlan_port_add(unit, vm_port, &vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, vswitch_metro_add_port_1 2, in_intf=0x%08x \n",proc_name, vlan_port_id);
    }

    rv = vswitch_add_port(unit, g_vxlan.vpn_id, vm_port, vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, vswitch_add_port\n", proc_name);
        return rv;
    }

    /* Create an MACT entry with the Native DA, mapped to the VPN */
    bcm_l2_addr_t l2addr1;
    bcm_l2_addr_t_init(&l2addr1, g_vxlan.vm1_da, g_vxlan.vpn_id);
    l2addr1.port = vm_port;
    l2addr1.mac = g_vxlan.vm1_da;
    l2addr1.vid = g_vxlan.vpn_id;
    l2addr1.flags = BCM_L2_STATIC;

    rv = bcm_l2_addr_add(unit, &l2addr1);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, bcm_l2_addr_add \n", proc_name);
        return rv;
    }

    /* Create an MACT entry for VXLAN tunnel, mapped to the VPN */
    bcm_l2_addr_t l2addr2;
    bcm_l2_addr_t_init(&l2addr2, g_vxlan.vm2_da, g_vxlan.vpn_id);
    l2addr2.port = vm_port;
    l2addr2.mac = g_vxlan.vm2_da;
    l2addr2.vid = g_vxlan.vpn_id;
    l2addr2.flags = BCM_L2_STATIC;

    rv = bcm_l2_addr_add(unit, &l2addr2);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, bcm_l2_addr_add \n", proc_name);
        return rv;
    }
    return rv;
}

int tunnel_initiator_create(
    int unit,
    bcm_if_t arp_itf,
    bcm_if_t *itf)
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

    /* Create IP tunnel initiator for encapsulating GRE4oIPv4 tunnel header*/
    bcm_tunnel_initiator_t_init(&tunnel_init);
    tunnel_init.dip = g_vxlan.tunnel_dip; /* default: 171.0.0.17 */
    tunnel_init.sip = g_vxlan.tunnel_sip; /* default: 160.0.0.1 */
    tunnel_init.flags = 0;
    tunnel_init.dscp = 10; /* default: 10 */
    tunnel_init.type = bcmTunnelTypeVxlan;
    tunnel_init.l3_intf_id = arp_itf;
    if (device_is_jericho2)
    {
        tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    }
    else
    {
        tunnel_init.dscp_sel = bcmTunnelDscpAssign;
        tunnel_init.vlan = g_vxlan.core_vlan; /* For JR2 the vlan is set at LL*/
    }
    tunnel_init.ttl = 64; /* default: 64 */
    tunnel_init.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE; /* default: -1 */
    rv = bcm_tunnel_initiator_create(unit,&l3_intf, tunnel_init);

    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_initiator_create \n");
    }

    *itf = l3_intf.l3a_intf_id;
    g_vxlan.tunnel_id_init = tunnel_init.tunnel_id;

    printf("tunnel encap tunnel id: 0x%x \n", g_vxlan.tunnel_id_init);
    printf("tunnel encap l3 itf: 0x%x \n", *itf);

    return rv;
}
