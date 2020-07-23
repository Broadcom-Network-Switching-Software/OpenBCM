/* 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


/*
 * The below CINT demonstrates IP-GRE L3VPN which show BCM886XX being PE-based MVPN.
 * Available for 88660_A0 and above.
 * MVPN model consists of three kind of devices:
 * 1. Customer Edge device (CE): Resides on customer network and connected with one or more interfaces to provide edge device.
 * The CE is not familiar with the existance of VPN and not with the IP core application.
 * 2. Provide Edge device (PE): Resides on service provider network and connects to one or more CEs.
 * PE is responsible on receiving and sending traffic from / to the core network between the CE and P.
 * PE is in charge of all the VPN processing.
 * 3. Provider device (P): Backbone router on a service provider network. Only in charge of the IP routing in the backbone and not familiar with the VPN processing.
 * It may be that PE device will fucntional as P for other VPN processing of not directly connected CE to backbone.
 *
 *
 * We are configuring PE.

 *
 *
 * Traffic:
 *   - MC packets:
 *     - Packet Incoming from IP-GRE network P1, to P2, P3, CE1, CE2 (Bud node)
 *     - Packet Incoming from IP-GRE network, to PE2 (Continue)
 *     - Packet Incoming from IP-GRE network, to CE1, CE2 (Leaf node)
 *     - Packet Incoming from access side, to CE2, P1
 *
 * Packet Incoming from provider network P1, to P2, P3, CE1, CE2 (Bud node: provider network MC and access MC)
 * Purpose: - check MC is done at provider metwork for IP compatible MC at the IP of the tunnel
 *          - check tunnel termination (of the IP of the overlay) is done, to perform MC at the access network
 *          - check MC is done at access network for IP compatible MC at the inner IP
 *    Send:
 *             ---------------------------------------------------------
 *        eth: |    DA             |   SA              | VLAN           |
 *             ----------------------------------------------------------
 *             | provider_MC       | routerP1_mac      | routerP1_vlan |
 *             | 01:00:5E:05:05:05 | 00:00:00:00:00:12 | 20            |
 *             ----------------------------------------------------------
 *                 ------------------------------
 *             ip: |   SIP        | DIP         |
 *                 ------------------------------
 *                 | routerCE9_ip | provider_MC |
 *                 | 0.0.233.233 | 224.5.5.5    |
 *                 ------------------------------
 *
 *                GRE 4B
 *
 *                       --------------------------
 *                   ip: |   SIP       | DIP       |
 *                       --------------------------
 *                       | host90_ip   | access_MC |
 *                       | 90.90.90.90 | 224.6.6.6 |
 *                       --------------------------
 *   Receive:
 *
 *    - receive 2 packets to P2 and P3
 *             ---------------------------------------------------------
 *        eth: |    DA             |   SA              | VLAN           |
 *             ----------------------------------------------------------
 *             | provider_MC       | routerPE1_mac     | routerP23_vlan |
 *             | 01:00:5E:05:05:05 | 00:0C:00:02:00:00 | 25             |
 *             ----------------------------------------------------------
 *                 ------------------------------
 *             ip: |   SIP        | DIP         |
 *                 ------------------------------
 *                 | routerCE9_ip | provider_MC |
 *                 | 0.0.233.233 | 224.5.5.5    |
 *                 ------------------------------
 *
 *                GRE 4B
 *
 *                       --------------------------
 *                   ip: |   SIP       | DIP       |
 *                       --------------------------
 *                       | host90_ip   | access_MC |
 *                       | 90.90.90.90 | 224.6.6.6 |
 *                       --------------------------
 *
 *    - receive 2 packets to CE1 and CE2
 *             -----------------------------------------------------------
 *        eth: |    DA             |   SA              | VLAN            |
 *             -----------------------------------------------------------
 *             | access_MC         | routerPE1_mac     | routerCE_vlan |
 *             | 01:00:5E:06:06:06 | 00:0C:00:02:00:00 | 30              |
 *             -----------------------------------------------------------
 *
 *                  --------------------------
 *              ip: |   SIP     | DIP         |
 *                  --------------------------
 *                  | host90_ip |  access_MC  |
 *                  | 90.90.90.90 | 224.6.6.6 |
 *                  --------------------------
 *
 *
 *
 *
 *
 * Packet Incoming from provider network, to PE2 (Continue: provider to provider)
 * Purpose: - check MC is done at provider metwork for IP compatible MC at the IP of the tunnel
 *
 *
 *    Send:
 *             ---------------------------------------------------------
 *        eth: |    DA             |   SA              | VLAN           |
 *             ----------------------------------------------------------
 *             | provider_MC       | routerP1_mac      | routerP1_vlan |
 *             | 01:00:5E:07:07:07 | 00:00:00:00:00:12 | 20            |
 *             ----------------------------------------------------------
 *                 ------------------------------
 *             ip: |   SIP        | DIP         |
 *                 ------------------------------
 *                 | routerCE9_ip | provider_MC |
 *                 | 0.0.233.233 | 224.7.7.7    |
 *                 ------------------------------
 *
 *                GRE 4B
 *
 *                       --------------------------
 *                   ip: |   SIP       | DIP       |
 *                       --------------------------
 *                       | host90_ip   | access_MC |
 *                       | 90.90.90.90 | 224.6.6.6 |
 *                       --------------------------
 *
 *  Receive:
 *    receive 2 packets to P2 and P3
 *             ---------------------------------------------------------
 *        eth: |    DA             |   SA              | VLAN           |
 *             ----------------------------------------------------------
 *             | provider_MC       | routerPE1_mac     | routerP23_vlan |
 *             | 01:00:5E:07:07:07 | 00:0C:00:02:00:00 | 25             |
 *             ----------------------------------------------------------
 *                 ------------------------------
 *             ip: |   SIP        | DIP         |
 *                 ------------------------------
 *                 | routerCE9_ip | provider_MC |
 *                 | 0.0.233.233 | 224.7.7.7    |
 *                 ------------------------------
 *
 *                GRE 4B
 *
 *                       --------------------------
 *                   ip: |   SIP       | DIP       |
 *                       --------------------------
 *                       | host90_ip   | access_MC |
 *                       | 90.90.90.90 | 224.6.6.6 |
 *                       --------------------------
 *
 *
 *
 *
 *
 *
 *
 * Packet Incoming from provider network, to CE2, CE3 (leaf node: provider to access):
 *  Purpose: - check tunnel termination (of the IP of the overlay) is done, to perform MC at the access network
 *           - check MC is done at access network for IP compatible MC at the inner IP
 *           - check no replication is done for provider network
 *
 *    Send:
 *             ---------------------------------------------------------
 *        eth: |    DA             |   SA              | VLAN           |
 *             ----------------------------------------------------------
 *             | provider_MC       | routerP1_mac      | routerP1_vlan |
 *             | 01:00:5E:08:08:08 | 00:00:00:00:00:12 | 20            |
 *             ----------------------------------------------------------
 *                 ------------------------------
 *             ip: |   SIP        | DIP         |
 *                 ------------------------------
 *                 | routerCE9_ip | provider_MC |
 *                 | 0.0.233.233 | 224.8.8.8    |
 *                 ------------------------------
 *
 *                GRE 4B
 *
 *                       --------------------------
 *                   ip: |   SIP       | DIP       |
 *                       --------------------------
 *                       | host91_ip   | access_MC |
 *                       | 91.91.91.91 | 224.6.6.6 |
 *                       --------------------------
 *
 *  Receive:
 *    - receive 2 packets to CE1 and CE2
 *             ---------------------------------------------------------
 *        eth: |    DA             |   SA              | VLAN          |
 *             ---------------------------------------------------------
 *             | access_MC         | routerPE1_mac     | routerCE_vlan |
 *             | 01:00:5E:09:09:09 | 00:00:00:00:CE:01 | 30            |
 *             ---------------------------------------------------------
 *
 *                  ----------------------------
 *              ip: |   SIP       | DIP         |
 *                  ----------------------------
 *                  | host90_ip   |  access_MC  |
 *                  | 91.91.91.91 | 224.6.6.6   |
 *                  ----------------------------
 *
 *
 *
 *
 * Packet Incoming from access network, to CE2, P1 (leaf node: access to access and provider):
 *  Purpose: - check MC is done at access network for IP compatible MC at the IP
 *           - check repalication at the access
 *           - check replication at the provider with overlay layers: GREoIPoE
 *
 *
 *    Send:
 *             ---------------------------------------------------------
 *        eth: |    DA             |   SA              | VLAN          |
 *             ---------------------------------------------------------
 *             | access_MC         | routerCE1_mac     | routerCE_vlan |
 *             | 01:00:5E:09:09:09 | 00:00:00:00:CE:01 | 30            |
 *             ---------------------------------------------------------
 *
 *                  ----------------------------
 *              ip: |   SIP       | DIP         |
 *                  ----------------------------
 *                  | host11_ip   |  access_MC  |
 *                  | 11.11.11.11 | 224.9.9.9   |
 *                  ----------------------------
 *
 *
 *   Receive:
 *      receive 1 packets to CE4
 *             ---------------------------------------------------------
 *        eth: |    DA             |   SA              | VLAN           |
 *             ---------------------------------------------------------
 *             | access_MC         | routerPE1_mac     | routerCE4_vlan |
 *             | 01:00:5E:09:09:09 | 00:0C:00:02:00:00 | 25             |
 *             ---------------------------------------------------------
 *
 *                  ----------------------------
 *              ip: |   SIP       | DIP         |
 *                  ----------------------------
 *                  | host11_ip   |  access_MC  |
 *                  | 11.11.11.11 | 224.9.9.9   |
 *                  ----------------------------
 *
 *  Receive:
 *    receive 1 packets to P1
 *             ---------------------------------------------------------
 *        eth: |    DA             |   SA              | VLAN           |
 *             ----------------------------------------------------------
 *             | provider_MC       | routerPE1_mac     | routerP1_vlan |
 *             | 01:00:5E:09:09:09 | 00:0C:00:02:00:00 | 20            |
 *             ----------------------------------------------------------
 *                 ------------------------------
 *             ip: |   SIP        | DIP         |
 *                 ------------------------------
 *                 | routerCE9_ip | provider_MC |
 *                 | 0.0.233.233 | 224.7.7.7    |
 *                 ------------------------------
 *
 *                GRE 4B
 *
 *                       --------------------------
 *                   ip: |   SIP       | DIP       |
 *                       --------------------------
 *                       | host90_ip   | access_MC |
 *                       | 90.90.90.90 | 224.6.6.6 |
 *                       --------------------------
 *
 *
 * How to run:
 *  soc properties:
 * tm_port_header_type_in_40.BCM88650=ETH
 * tm_port_header_type_out_40.BCM88650=ETH
 * ucode_port_40.BCM88650=RCY.0
 * default_logical_interface_ip_tunnel_overlay_mc=16484  /* for Jer1 only*/
 *
 *
 * Test Scenario
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_vlan.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/dnx/utility/cint_dnx_util_rch.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_sand_mvpn.c
 * cint
 * mvpn_run_defaults_with_ports(0,200,201,202,203);
 * exit;
 *
 * /* mvpn test 1 - packet from P1 to P2, P3 and CE1 CE2 (bud node: provider to provider and access) */
 * tx 1 psrc=202 data=0x01005e0505050000000000128100001408004500002c00000000402fabaf0000e9e9e00505050000080045000014000000004000e0295a5a5a5ae0060606
 *
 * /* Received packets on unit 0 should be: */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e060606000c000200008100001e080045000014000000003f00e1295a5a5a5ae0060606 */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e060606000c000200008100001e080045000014000000003f00e1295a5a5a5ae0060606 */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e050505000c000200008100001908004500002c000000003f2facaf0000e9e9e00505050000080045000014000000004000e0295a5a5a5ae0060606 */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e050505000c000200008100001908004500002c000000003f2facaf0000e9e9e00505050000080045000014000000004000e0295a5a5a5ae0060606 */
 *
 * /* mvpn test 2 - packet from P1 to P2 and P3 (continue: provider to provider) */
 * tx 1 psrc=202 data=0x01005e0707070000000000128100001408004500002c00000000402fa9ab0000e9e9e00707070000080045000014000000004000de275b5b5b5be0060606
 *
 * /* Received packets on unit 0 should be: */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e070707000c000200008100001908004500002c000000003f2faaab0000e9e9e00707070000080045000014000000004000de275b5b5b5be0060606 */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e070707000c000200008100001908004500002c000000003f2faaab0000e9e9e00707070000080045000014000000004000de275b5b5b5be0060606 */
 *
 * /* mvpn test 3 - packet from P1 to CE1, CE2 (drop) */
 * tx 1 psrc=202 data=0x01005e0808080000000000128100001408004500002c00000000402fa8a90000e9e9e00808080000080045000014000000004000dc255c5c5c5ce0060606
 *
 * /* Received packets on unit 0 should be: */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e060606000c000200008100001e080045000014000000003f00dd255c5c5c5ce0060606 */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e060606000c000200008100001e080045000014000000003f00dd255c5c5c5ce0060606 */
 *
 * /* mvpn test 4 - packet from CE1 to P1 and CE2 (access to access and provider) */
 * tx 1 psrc=200 data=0x01005e09090900000000ce018100001e0800450000140000000080003bc20b0b0b0be0090909
 *
 * /* Received packets on unit 0 should be: */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e090909000c0002000081000019080045000014000000007f003cc20b0b0b0be0090909000000000000000000000000000000000000000000000000 */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e090909000c000200008100001408004500004600000000002f3166a0000011e00909090000080045000014000000007f003cc20b0b0b0be0090909000000000000000000000000000000000000000000000000 */
 */



struct cint_mvpn_info_s {
    int access_port;               /* access port */
    int access_port2;              /* another access port */
    int provider_port;             /* provider port */
    int provider_port2;            /* another provider port */
    int recycle_port;              /* recycle port */
    int provider_vlan;             /* vlan at the l2gre header at provider side */
    int provider_vlan2;            /* another vlan at the l2gre header at provider side */
    int access_vlan;               /* vlan at the access side */
    int access_vlan2;              /* another vlan at the access side */
    bcm_mac_t my_mac;              /* my mac */
    int internal_vlan;             /* this vlan is internal: used to build overlay vlan for packet that goes to 2nd pass.
                            * from vlan at the 2nd pass: get vsi, get inRif. This inRif at frwrd.   */
    int ipmc_index;                /* ipmc index base. (Each MC group creation increment this index) */
    int recycle_entry_encap_id;    /* relevant to JR2 */
};

cint_mvpn_info_s cint_mvpn_info =
/* ports :
   access port | another access port | provider port | another provider port | recycle port */
{200,            201,                  202 ,           203,                    40,
/* vlans:
   provider vlan | another provider vlan | access vlan | another access vlan */
    20,            25,                     30,           35,
/* my mac */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00},
/* internal vlan */
11,
/* ipmc index base */
5000,
/* recycle_entry_encap_id */
0
};

void
mvpn_info_init (
    int unit,
    int access_port,
    int access_port2,
    int provider_port,
    int provider_port2)
{
    cint_mvpn_info.access_port = access_port;
    cint_mvpn_info.access_port2 = access_port2;
    cint_mvpn_info.provider_port = provider_port;
    cint_mvpn_info.provider_port2 = provider_port2;
}

/* Adding ports to MC group*/
int
mvpn_multicast_ingress_set (
    int unit,
    int ipmc_index,
    int *ports,
    int nof_ports,
    int *encaps)
{
    int rv = BCM_E_NONE;
    int i;
    bcm_multicast_replication_t replications[8];
    int flags = 0;

    for (i=0; i < nof_ports; i++) {
        bcm_multicast_replication_t_init(&replications[i]);
        BCM_GPORT_LOCAL_SET(replications[i].port, ports[i]);
        replications[i].encap1 = encaps[i];
    }

    flags = is_device_or_above(unit, JERICHO2) ? BCM_MULTICAST_INGRESS_GROUP : BCM_MULTICAST_INGRESS;
    rv = bcm_multicast_set(unit, ipmc_index, flags, nof_ports, replications);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_set\n");
        return rv;
    }

    return rv;
}

int
mvpn_ip_tunnel_terminator_create(
    int unit,
    bcm_ip_t dip,
    int *term_tunnel_id,
    int flags)
{
    int rv;
    bcm_tunnel_terminator_t tunnel_term;

    /*
     * Create IP tunnel terminator for SIP,DIP, VRF lookup in VTT3 SEM
     */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.dip      = dip;
    tunnel_term.dip_mask = 0xFFFFFFFF;
    tunnel_term.vrf      = 0;
    tunnel_term.type     = bcmTunnelTypeGreAnyIn4;
    tunnel_term.flags    = flags;

    rv = bcm_tunnel_terminator_create(unit, &tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_terminator_create \n");
        return rv;
    }

    *term_tunnel_id = tunnel_term.tunnel_id;

    return rv;
}


/* configure l2gre ip tunnel, ready to be added to MC group using encap_id.
 * DA: 01:00:5E:09:09:09
 * IP: 224.9.9.9
 */
int
mvpn_ip_tunnel_initiator_create (
    int unit,
    int *tunnel_intf)
{
    int rv = BCM_E_NONE;
    int tunnel_arp_id;
    bcm_mac_t tunnel_next_hop_mac = {0x01, 0x00, 0x5E, 0x09, 0x09, 0x09};
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;

    rv = l3__egress_only_encap__create(unit, 0, &tunnel_arp_id,
                                       tunnel_next_hop_mac,
                                       cint_mvpn_info.provider_vlan);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object ARP only\n");
        return rv;
    }

    /*
     * Create the tunnel initiator
     * and set next-lif-pointer to the ARP entry
     */
    bcm_l3_intf_t_init(&l3_intf);

    /* Create IP tunnel initiator for encapsulating GRE4oIPv4 tunnel header*/
    bcm_tunnel_initiator_t_init(&tunnel_init);
    tunnel_init.dip        = 0xE0090909;
    tunnel_init.sip        = 0xA0000011;
    tunnel_init.flags      = 0;
    tunnel_init.type       = bcmTunnelTypeGreAnyIn4;
    tunnel_init.l3_intf_id = tunnel_arp_id;
    if (!is_device_or_above(unit, JERICHO2))
    {
        /* For Jr2, vlan is an ARP property */
        tunnel_init.vlan = cint_mvpn_info.provider_vlan;
    }

    rv = bcm_tunnel_initiator_create(unit,&l3_intf, tunnel_init);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_initiator_create \n");
    }

    *tunnel_intf = l3_intf.l3a_intf_id;

    return rv;
}

int
mvpn_ipmc_add (
    int unit,
    bcm_ip_t mc_ip,
    bcm_ip_t src_ip,
    int ipmc_index,
    int vlan,
    uint32 flags)
{
    int rv = BCM_E_NONE;
    bcm_ipmc_addr_t data;

    /* Configurations relevant for LEM<FID,DIP> lookup*/
    bcm_ipmc_addr_t_init(&data);
    data.mc_ip_addr = mc_ip;
    data.mc_ip_mask = 0xffffffff;
    if (src_ip != -1 && src_ip != 0) {
        data.s_ip_addr = src_ip;
        data.s_ip_mask = 0xffffffff;
        data.vid = vlan;
    }

    data.flags = flags;
    data.group = ipmc_index;
    /* Creates the entry */
    rv = bcm_ipmc_add(unit,&data);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_ipmc_add \n");
        return rv;
    }

    return rv;
}


int
mvpn_l3_intf_configuration (
    int unit)
{
    int rv;
    char* proc_name = "mvpn_l3_intf_configuration";
    int vrf = 0;
    sand_utils_l3_eth_rif_s eth_rif_structure;

    /* add provider port to provider vlan. */
    rv = in_port_intf_set(unit, cint_mvpn_info.provider_port, cint_mvpn_info.provider_vlan); 
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set \n");
        return rv;
    }

    /* routing interface for IP of the overlay*/

    /* 1. create RIF for IP routing for IP of the overlay : inRif allow the packet to be routed */
    sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_mvpn_info.provider_vlan, 0, BCM_L3_INGRESS_GLOBAL_ROUTE, cint_mvpn_info.my_mac, vrf);
    rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, sand_utils_l3_eth_rif_create provider_vlan\n");
        return rv;
    }
    printf("%s:\t rif configured for provider_vlan(%d)\n", proc_name, cint_mvpn_info.provider_vlan);

    /* 2. create RIF for IP routing for IP of the overlay: outRif build the LL: SA and VLAN */
    sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_mvpn_info.provider_vlan2, 0, 0, cint_mvpn_info.my_mac, vrf);
    rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, sand_utils_l3_eth_rif_create provider_vlan2\n");
        return rv;
    }
    printf("%s:\t rif configured for provider_vlan2(%d)\n", proc_name, cint_mvpn_info.provider_vlan2);

    /* routing interface for IP of the overlay for 2nd pass */

    /* 3. create RIF for IP routing at the IP of the overlay, for the 2nd pass:
     * outRif builds the LL: SA and VLAN (at the 1st pass)
     * inRif allows packet to be routed (at the 2nd pass) */
    sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_mvpn_info.internal_vlan, 0, BCM_L3_INGRESS_GLOBAL_ROUTE, cint_mvpn_info.my_mac, vrf);
    rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, sand_utils_l3_eth_rif_create internal_vlan\n");
        return rv;
    }
    printf("%s:\t rif configured for internal_vlan(%d)\n", proc_name, cint_mvpn_info.internal_vlan);

    /* add access port to access vlan. */
    rv = in_port_intf_set(unit, cint_mvpn_info.access_port, cint_mvpn_info.access_vlan); 
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set \n");
        return rv;
    }
    /* 4. create RIF for IP routing at the IP of the access, for the 2nd pass:
     * outRif is used to build the LL: SA and VLAN
     * inRif allows packet to be routed */
    sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_mvpn_info.access_vlan, 0, BCM_L3_INGRESS_GLOBAL_ROUTE, cint_mvpn_info.my_mac, vrf);
    rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, sand_utils_l3_eth_rif_create access_vlan\n");
        return rv;
    }
    printf("%s:\t rif configured for access_vlan(%d)\n", proc_name, cint_mvpn_info.access_vlan);

    return rv;
}

int
mvpn_recycle_configuration (
    int unit)
{
    int rv;
    char* proc_name = "mvpn_recycle_configuration";
    int vrf = 0;
    sand_utils_l3_eth_rif_s eth_rif_structure;

    /* 1. ports configuration: add a recycle port */
    rv = bcm_port_control_set(unit, cint_mvpn_info.recycle_port, bcmPortControlOverlayRecycle, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set\n");
        return rv;
    }
    printf("%s:\t recycle port(%d) configured\n", proc_name, cint_mvpn_info.recycle_port);

    if (is_device_or_above(unit, JERICHO2)) {
        /** Create entry, which will be built the recycle header for the copy going on the 2nd pass */
        rv = create_recycle_entry(unit, &cint_mvpn_info.recycle_entry_encap_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create_recycle_entry \n");
            return rv;
        }
        printf("%s:\t recycle entry(0x%08X) created \n", proc_name, cint_mvpn_info.recycle_entry_encap_id);
    }

    if (!is_device_or_above(unit, JERICHO2)) {
        /* On JR2, we have default lif. No need to create AC lif */
        /* JR1 AC configuration:
         * - at the 2nd pass, we got the VSI from port.default_vlan. (don't allocate lif)
         *
         * - the rif resolved at AC is the rif used for the inner IP.
         * (no special configuration here, at the 2nd pass, dummy lif for IP tunnel termination
         *  will use the AC rif instead of resolve his own)
         */

        /* add AC lif for the overlay at the 2nd pass */
        bcm_vlan_port_t vp1;
        bcm_vlan_port_t_init(&vp1);
        /* the match criteria is port */
        vp1.criteria = BCM_VLAN_PORT_MATCH_PORT;
        vp1.port = cint_mvpn_info.recycle_port;
        vp1.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_VSI_BASE_VID;
        rv = bcm_vlan_port_create(unit,&vp1);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_create\n");
            return rv;
        }
        printf("%s:\t AC lif for the overlay at the 2nd pass configured: port(%d) lif(%d)\n ", proc_name, vp1.port,vp1.vlan_port_id);
    }

    return rv;
}



/*  multicast configuration:
 *  - add MC for the IP of the overlay
 *  - add ports to MC: provider ports and recycle
 *  - add routing entry to the MC (224.5.5.5 -> MC)
 */
int
mvpn_bud_provider_mc (
    int unit )
{
    int rv = BCM_E_NONE;
    char* proc_name = "mvpn_bud_provider_mc";
    bcm_ip_t mc_ip = 0xe0050505;
    int term_tunnel_id;
    bcm_if_t l3_intf_id;
    bcm_l3_ingress_t l3_ing_if;
    int ipmc_index = cint_mvpn_info.ipmc_index++;

    if (is_device_or_above(unit, JERICHO2))
    {
        /* On Jer1, for BUD node tunnels, a dedicate database exists */
        rv = mvpn_ip_tunnel_terminator_create(unit, mc_ip, &term_tunnel_id, BCM_TUNNEL_TERM_BUD);
        if (rv != BCM_E_NONE) {
            printf("Error, mvpn_ip_tunnel_terminator_create\n");
            return rv;
        }
        printf("%s:\t ip_tunnel_terminator_create term_tunnel_id(0x%08X)\n", proc_name, term_tunnel_id);

        /* Set Incoming Tunnel-RIF properties */
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(l3_intf_id, term_tunnel_id);

        bcm_l3_ingress_t_init(&l3_ing_if);
        l3_ing_if.flags = BCM_L3_INGRESS_GLOBAL_ROUTE | BCM_L3_INGRESS_WITH_ID;
        l3_ing_if.vrf = 0;
        rv = bcm_l3_ingress_create(unit, l3_ing_if, l3_intf_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_l3_ingress_create\n");
            return rv;
        }
    }

    /* MC for the IP of the overlay: multicast to provider and recycle. - For 1st pass */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L3) | BCM_MULTICAST_WITH_ID, ipmc_index);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }
    printf("%s:\t MC configured ipmc_index(0x%08X)\n", proc_name, ipmc_index);

    /* add members to MC group: add provider ports, recycle port
     * On JR2, one copy goes to recycle header encap
     */
    int encap_for_recycle = is_device_or_above(unit, JERICHO2) ? BCM_L3_ITF_VAL_GET(cint_mvpn_info.recycle_entry_encap_id) : cint_mvpn_info.internal_vlan;
    int members_of_mc[3]  = {cint_mvpn_info.provider_port,  cint_mvpn_info.provider_port2, cint_mvpn_info.recycle_port};
    int encaps[3]         = {cint_mvpn_info.provider_vlan2, cint_mvpn_info.provider_vlan2, encap_for_recycle};
    int nof_member_of_mc  = 3;
    rv = mvpn_multicast_ingress_set(unit,ipmc_index, members_of_mc, nof_member_of_mc, encaps);
    if (rv != BCM_E_NONE) {
        printf("Error, mvpn_multicast_ingress_set\n");
        return rv;
    }
    printf("%s:\t %d members are added to MC\n", proc_name, nof_member_of_mc);

    /* add routing table entry, point to MC */

    rv = mvpn_ipmc_add(unit, mc_ip, 0/*src ip*/, ipmc_index, cint_mvpn_info.provider_vlan/* inRif */, 0/*flags*/);
    if (rv != BCM_E_NONE) {
        printf("Error, mvpn_ipmc_add\n");
        return rv;
    }
    printf("%s:\t mc group entry(0x%08X) added to routing table (0x%08X)\n", proc_name, ipmc_index, mc_ip);

    return rv;
}



/*  multicast configuration:
 *  - add MC for the IP of the overlay
 *  - add ports to MC: provider ports
 *  - add routing entry to the MC (224.7.7.7 -> MC)
 */
int
mvpn_continue_mc (
    int unit)
{
    int rv = BCM_E_NONE;
    char* proc_name = "mvpn_continue_mc";
    int ipmc_index = cint_mvpn_info.ipmc_index++;

    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L3) | BCM_MULTICAST_WITH_ID, ipmc_index);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }
    printf("%s:\t MC configured ipmc_index(0x%08X)\n", proc_name, ipmc_index);

    /* add members to MC group: add provider ports  */
    int members_of_mc[2] = {cint_mvpn_info.provider_port,  cint_mvpn_info.provider_port2};
    int encaps[2]        = {cint_mvpn_info.provider_vlan2, cint_mvpn_info.provider_vlan2};
    int nof_member_of_mc = 2;
    rv = mvpn_multicast_ingress_set(unit, ipmc_index, members_of_mc, nof_member_of_mc, encaps);
    if (rv != BCM_E_NONE) {
        printf("Error, mvpn_multicast_ingress_set\n");
        return rv;
    }
    printf("%s:\t %d members are added to MC\n", proc_name, nof_member_of_mc);

    /* add routing table entry, point to MC */
    bcm_ip_t mc_ip = 0xe0070707;
    rv = mvpn_ipmc_add(unit, mc_ip, 0/*src ip*/, ipmc_index, cint_mvpn_info.provider_vlan/* inRif */, 0/*flags*/);
    if (rv != BCM_E_NONE) {
        printf("Error, mvpn_ipmc_add\n");
        return rv;
    }
    printf("%s:\t mc group entry(0x%08X) added to routing table (0x%08X)\n", proc_name, ipmc_index, mc_ip);

    return rv;
}

/* multicast configuration:
 * - add MC for the IP of the access
 * - add ports to MC: access ports
 * - add routing entry to the MC (224.6.6.6 -> MC)
 */
int
mvpn_bud_leaf_access_mc (
    int unit)
{
    int rv = BCM_E_NONE;
    char* proc_name = "mvpn_bud_leaf_access_mc";
    int ipmc_index = cint_mvpn_info.ipmc_index++;

    /* MC for the IP of the access */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L3) | BCM_MULTICAST_WITH_ID, ipmc_index);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }
    printf("%s:\t MC configured ipmc_index(0x%08X)\n", proc_name, ipmc_index);

    /* add members to MC group: access ports */
    int members_of_mc[2] = {cint_mvpn_info.access_port,  cint_mvpn_info.access_port2};
    int encaps[2]        = {cint_mvpn_info.access_vlan, cint_mvpn_info.access_vlan};
    int nof_member_of_mc = 2;
    rv = mvpn_multicast_ingress_set(unit, ipmc_index, members_of_mc, nof_member_of_mc, encaps);
    if (rv != BCM_E_NONE) {
        printf("Error, mvpn_multicast_ingress_set\n");
        return rv;
    }
    printf("%s:\t %d members are added to MC\n", proc_name, nof_member_of_mc);

    /* add routing table entry, point to MC */
    bcm_ip_t mc_ip = 0xe0060606;
    rv = mvpn_ipmc_add(unit, mc_ip, 0/*src ip*/, ipmc_index, cint_mvpn_info.internal_vlan/* inRif */, 0/*flags*/);
    if (rv != BCM_E_NONE) {
        printf("Error, mvpn_ipmc_add\n");
        return rv;
    }
    printf("%s:\t mc group entry(0x%08X) added to routing table (0x%08X)\n", proc_name, ipmc_index, mc_ip);

    return rv;
}

/* Leaf configuration
 * On JR2, for leaf node, one-pass is enough.
 *   outer ip header is terminated in VT, and inner ip header is used as a key in FLP. .
 * On JR1, for leaf node, two-pass is needed
 *   1st pass, packet is replicated to recycle port. 2cd pass, tunnel termination and replicated to access port.
 *   multicast configuration:
 *   - add MC for the IP of the overlay
 *   - add port to MC: recycle port
 *   - add routing entry to the MC (224.8.8.8 -> MC)
 */
int
mvpn_leaf_configuration (
    int unit)
{
    int rv = BCM_E_NONE;
    char* proc_name = "mvpn_leaf_configuration";
    bcm_ip_t mc_ip = 0xe0080808;
    int term_tunnel_id;
    bcm_if_t l3_intf_id;
    bcm_l3_ingress_t l3_ing_if;

    int ipmc_index = cint_mvpn_info.ipmc_index++;

    if (is_device_or_above(unit, JERICHO2))
    {
        /* For Jer1, Ip dummy lif is created during SDK init in case soc default_logical_interface_ip_tunnel_overlay_mc is configured */
        rv = mvpn_ip_tunnel_terminator_create(unit, mc_ip, &term_tunnel_id, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, mvpn_ip_tunnel_terminator_create\n");
            return rv;
        }
        printf("%s:\t ip_tunnel_terminator_create term_tunnel_id(0x%08X)\n", proc_name, term_tunnel_id);

        /* Set Incoming Tunnel-RIF properties */
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(l3_intf_id, term_tunnel_id);

        bcm_l3_ingress_t_init(&l3_ing_if);
        l3_ing_if.flags = BCM_L3_INGRESS_GLOBAL_ROUTE | BCM_L3_INGRESS_WITH_ID;
        l3_ing_if.vrf = 0;
        rv = bcm_l3_ingress_create(unit, l3_ing_if, l3_intf_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_l3_ingress_create\n");
            return rv;
        }
    }
    else
    {
        /* MC for the IP of the overlay:
         * multicast to recycle
           For 1st pass */

        /* create MC group with id=40960:  */
        rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L3) | BCM_MULTICAST_WITH_ID, ipmc_index);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_create \n");
            return rv;
        }
        printf("%s:\t MC configured ipmc_index(0x%08X)\n", proc_name, ipmc_index);

        /* add members to MC group: add recycle port  */
        int encap_for_recycle = cint_mvpn_info.internal_vlan;
        int members_of_mc[1]  = {cint_mvpn_info.recycle_port};
        int encaps[1]         = {encap_for_recycle};
        int nof_member_of_mc  = 1;
        rv = mvpn_multicast_ingress_set(unit,ipmc_index, members_of_mc, nof_member_of_mc, encaps);
        if (rv != BCM_E_NONE) {
            printf("Error, mvpn_multicast_ingress_set\n");
            return rv;
        }
        printf("%s:\t %d members are added to MC\n", proc_name, nof_member_of_mc);

        /* add routing table entry, point to MC */
        rv = mvpn_ipmc_add(unit, mc_ip, 0/*src ip*/, ipmc_index, cint_mvpn_info.provider_vlan/* inRif */, 0/*flags*/);
        if (rv != BCM_E_NONE) {
            printf("Error, mvpn_ipmc_add\n");
            return rv;
        }
        printf("%s:\t mc group entry(0x%08X) added to routing table (0x%08X)\n", proc_name, ipmc_index, mc_ip);
    }

    return rv;
}

/* multicast configuration:
 * - add MC for the IP of the access
 * - add port to MC:
 *   - access port
 *   - configure l2gre ip tunnel
 *   - add provider "l2gre ip tunnel" port
 * - add routing entry to the MC (224.9.9.9 -> MC)
 */
int
mvpn_access_mc (
    int unit)
{
    int rv = BCM_E_NONE;
    char* proc_name = "mvpn_access_mc";
    int ipmc_index = cint_mvpn_info.ipmc_index++;

    /* MC for the IP of the native IP: multicast to provider and access */

    /* create MC group */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L3) | BCM_MULTICAST_WITH_ID, ipmc_index);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }
    printf("%s:\t MC configured ipmc_index(0x%08X)\n", proc_name, ipmc_index);

    /* add members to MC group: add l2gre ip tunnel port  */

    /* configure l2gre IP tunnel: with compatible MC addresses */
    int tunnel_encap_id;
    rv = mvpn_ip_tunnel_initiator_create(unit, &tunnel_encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in mvpn_l2gre_ip_tunnel\n");
        return rv;
    }
    printf("%s:\t mvpn_ip_tunnel_initiator_create tunnel_encap_id(0x%08X)\n", proc_name, tunnel_encap_id);

    /* add members to MC group: provider_port, access port */
    int members_of_mc[2] = {cint_mvpn_info.provider_port,  cint_mvpn_info.access_port2};
    int encaps[2]        = {(tunnel_encap_id&0x3fffff),    cint_mvpn_info.provider_vlan2};
    int nof_member_of_mc = 2;
    rv = mvpn_multicast_ingress_set(unit, ipmc_index, members_of_mc, nof_member_of_mc, encaps);
    if (rv != BCM_E_NONE) {
        printf("Error, mvpn_multicast_ingress_set\n");
        return rv;
    }
    printf("%s:\t %d members are added to MC\n", proc_name, nof_member_of_mc);

    /* add routing table entry, point to MC */
    bcm_ip_t mc_ip = 0xe0090909;
    rv = mvpn_ipmc_add(unit, mc_ip, 0/*src ip*/, ipmc_index, cint_mvpn_info.access_vlan/* inRif */, 0/*flags*/);
    if (rv != BCM_E_NONE) {
        printf("Error, mvpn_ipmc_add\n");
        return rv;
    }
    printf("%s:\t mc group entry(0x%08X) added to routing table (0x%08X)\n", proc_name, ipmc_index, mc_ip);

    return rv;
}

int
mvpn_run (
    int unit)
{
    int rv = BCM_E_NONE;

    /*  recycle configuration */
    rv = mvpn_recycle_configuration(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, mvpn_recycle_configuration\n");
        return rv;
    }
    

    /* create routing interface*/
    rv = mvpn_l3_intf_configuration(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, mvpn_l3_intf_configuration\n");
        return rv;
    }

    /* bud node (drop and continue) MC configuration:
     * - configure MC and routing table entry at provider side
       - configure MC and routing table entry at access side */
    rv = mvpn_bud_provider_mc(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, mvpn_bud_provider_mc\n");
        return rv;
    }

    /* we configure only 1 MC group at access. (used for bud node and leaf node)*/
    rv = mvpn_bud_leaf_access_mc(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, mvpn_bud_leaf_access_mc\n");
        return rv;
    }

    /* "continue" node MC configuration:
     * - configure MC and routing table entry at provider side */
    rv = mvpn_continue_mc(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, mvpn_continue_mc\n");
        return rv;
    }

    /* leaf node configuration:
     * On JR2, for leaf node, one-pass is enough.
     *   outer ip header is terminated in VT, and inner ip header is used as a key in FLP. .
     * On JR1, for leaf node, two-pass is needed
     *   1st pass, packet is replicated to recycle port. 2cd pass, tunnel termination and replicated to access port.
     * configure MC at access side: see mvpn_bud_leaf_access_mc */
    rv = mvpn_leaf_configuration(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, mvpn_leaf_configuration\n");
        return rv;
    }

    /* access node MC configuration:
     * - configure MC and routing table entry at access side: to access and provider
       - for provider: configure l2gre ip tunnel. */
    rv = mvpn_access_mc(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, mvpn_access_mc\n");
        return rv;
    }

    return rv;
}


int
mvpn_run_defaults_with_ports (
    int unit,
    int accessPort,
    int accessPort2,
    int networkPort,
    int networkPort2)
{
    mvpn_info_init(unit,
                   accessPort, /* access port */
                   accessPort2, /* another access port */
                   networkPort,  /* provider port */
                   networkPort2 /* another provider port */
                   );
    return mvpn_run(unit);
}


int
mvpn_run_with_defaults (
    int unit)
{
   return mvpn_run_defaults_with_ports(unit, 200, 201, 202, 203);
}

