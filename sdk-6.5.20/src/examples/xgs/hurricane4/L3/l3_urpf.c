/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenSDK/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : L3 URPF with strict/loose mode
 *
 * Usage    : BCM.0> cint l3_urpf.c
 *
 * config   : l3_config.bcm
 *
 * Log file : l3_urpf_log.txt
 *
 * Test Topology :
 *
 *                  +------------------------------+
 *                  |                              |
 *                  |                              |
 *                  |                              |
 *     ingress_port |                              |  egress_port
 * +----------------+          SWITCH              +-----------------+
 *                  |                              |
 *                  |                              |
 *                  |                              |
 *                  |                              |
 *                  |                              |
 *                  +------------------------------+
 * Summary:
 * ========
 *   This Cint example to show configuration of the L3 IPv4 entry with URPF mode
 *   (strict/loose) using BCM APIs. In strict uRPF check mode, checks are
 *   performed to ensure that the SIP address is present in the routing table
 *   and the incoming L3 interface matches the SIP's L3 interface in the routing table.
 *   In loose uRPF check mode, it merely verifies that the source IP address is
 *   present in the routing table.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects two ports and configure them in Loopback mode. Out of these
 *        two ports, one port is used as ingress_port and the other as
 *        egress_port.
 *
 *     b) Install an IFP rule to copy incoming packets to CPU and start
 *        packet watcher.
 *
 *     Note: IFP rule is meant for a testing purpose only (Internal) and it is
 *           nothing to do with an actual functional test.
 *
 *   2) Step2 - Configuration (Done in config_l3_urpf()):
 *   =========================================================
 *     a) Configure a basic IPv4 route/entry with URPF mode (strict/loose)
 *        functional scenario. This adds the route in l3 table[route] and
 *        does the necessary configurations of vlan, interface and next hop.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l3 intf show',
 *        'l3 egress show' and 'l3 defip show'
 *
 *     b) Transmit the known IPv4 unicast packet with SIP hit and miss use-cases.
 *        The contents of the packet are printed on screen.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see that dmac, smac and vlan are all changed as the packet is
 *       routed through the egress port. For SIP hit case, packet egresses out
 *       on egress_port and for SIP miss case, packet drops. And then Turn on
 *       URPF_MISS_TOCPU field and re-send SIP miss packet, so the packet
 *       failing the URPF is forwarded to CPU with reason code 'L3SourceMiss'.
 *       Also run the 'l3 defip show' to check the HIT bit status (y i.e Yes)
 *       and 'show c' to check the Tx/Rx packet stats/counters.
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t ingress_port;
bcm_port_t egress_port;

/*
 * This function is written so that hardcoding of port
 * numbers in Cint scripts is removed. This function gives
 * required number of ports
 */
bcm_error_t
portNumbersGet(int unit, int *port_list, int num_ports)
{
    int i = 0, port = 0, rv = 0;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;

    rv = bcm_port_config_get(unit, &configP);
    if (BCM_FAILURE(rv)) {
        printf("\nError in retrieving port configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    ports_pbmp = configP.e;
    for (i= 1; i < BCM_PBMP_PORT_MAX; i++) {
        if (BCM_PBMP_MEMBER(&ports_pbmp, i) && (port < num_ports)) {
            port_list[port] = i;
            port++;
        }
    }

    if ((0 == port) || (port != num_ports)) {
        printf("portNumbersGet() failed \n");
        return -1;
    }

    return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified port to CPU.
 */
bcm_error_t
ingress_port_setup(int unit, bcm_port_t port)
{
    bcm_field_qset_t qset;
    bcm_field_group_t group;
    bcm_field_entry_t entry;

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_NONE));

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);

    BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, &group));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified port to CPU. Port is also configured
 * to discard all packets. This is to avoid continuous
 * loopback of the packet.
 */
bcm_error_t
egress_port_setup(int unit, bcm_port_t port)
{
    bcm_field_qset_t qset;
    bcm_field_group_t group;
    bcm_field_entry_t entry;

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_ALL));

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);

    BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, &group));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    return BCM_E_NONE;
}

/*
 * Test Setup:
 * This functions gets the port numbers and sets up ingress and
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t
test_setup(int unit)
{
    int port_list[2];

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 2)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    ingress_port = port_list[0];
    egress_port = port_list[1];

    if (BCM_E_NONE != ingress_port_setup(unit, ingress_port)) {
        printf("ingress_port_setup() failed for port %d\n", ingress_port);
        return -1;
    }

    if (BCM_E_NONE != egress_port_setup(unit, egress_port)) {
        printf("egress_port_setup() failed for port %d\n", egress_port);
        return -1;
    }

    bshell(unit, "pw start report +raw +decode");

    return BCM_E_NONE;
}

void
verify(int unit)
{
    char str[512];

    bshell(unit, "hm ieee");
    bshell(unit, "vlan show");
    bshell(unit, "l3 intf show");
    bshell(unit, "l3 egress show");
    bshell(unit, "l3 defip show");
    bshell(unit, "clear c");

    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, ingress_port, BCM_PORT_DISCARD_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egress_port, BCM_PORT_DISCARD_ALL));
    printf("\na) Sending IPv4 unicast packet (SIP=10.10.10.1) to ingress_port:%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000AAAA0000000022228100000C0800450000320000000040FF9B190A0A0A01C0A80A01000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D907E5579; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "l3 defip show");
    bshell(unit, "show c");

    printf("\nb) Sending IPv4 unicast packet (SIP=10.10.20.1), SIP miss (so URPF fails) to ingress_port:%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000AAAA0000000022228100000C0800450000320000000040FF91190A0A1401C0A80A01000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D2EF9FFDE; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "l3 defip show");
    bshell(unit, "show c");

    printf("\nc) Turn on URPF_MISS_TOCPU, so the packet failing the URPF is forwarded to CPU, re-send SIP miss packet to ingress_port:%d\n", ingress_port);
    print "modreg CPU_CONTROL_1  URPF_MISS_TOCPU=1";
    bshell(unit, "modreg CPU_CONTROL_1  URPF_MISS_TOCPU=1");
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000AAAA0000000022228100000C0800450000320000000040FF91190A0A1401C0A80A01000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D2EF9FFDE; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "l3 defip show");
    bshell(unit, "show c");

    return;
}


bcm_error_t
config_l3_urpf(int unit)
{
    bcm_vlan_t vid_in = 12;
    bcm_vlan_t vid_egr = 13;
    int port_in = ingress_port;
    int port_egr = egress_port;
    bcm_l3_intf_t l3_intf;
    int vrf = 0;
    bcm_if_t l3_egr_if_egr;
    bcm_if_t l3_egr_if_in;
    bcm_l3_ingress_t l3_ingress;
    bcm_if_t ingress_if_egr;
    bcm_if_t ingress_if_in;
    bcm_l3_egress_t l3_egress;
    bcm_if_t egr_obj_egr;
    bcm_if_t egr_obj_in;
    bcm_vlan_control_vlan_t vlan_control;
    bcm_pbmp_t pbmp, upbmp;
    bcm_l3_route_t route_info;

    bcm_mac_t router_mac_egr  = {0x00,0x00,0x00,0x00,0xBB,0xBB};
    bcm_mac_t router_mac_in  = {0x00,0x00,0x00,0x00,0xAA,0xAA};
    bcm_mac_t nxt_hop_egr  = {0x00,0x00,0x00,0x00,0x11,0x11};
    bcm_mac_t nxt_hop_in  = {0x00,0x00,0x00,0x00,0x22,0x22};

    bcm_ip_t dip = 0xC0A80A01;  /* 192.168.10.1 */
    bcm_ip_t sip = 0x0A0A0A01;  /* 10.10.10.1 */
    bcm_ip_t mask = 0xffffff00;  /* 255.255.255.0 */

    /* Use Egress Object mode */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, TRUE));

    /* Allow explicaitly configure ingress intf */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3IngressMode, TRUE));

    /* Enable mapping between VLAN and L3 IIF */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3IngressInterfaceMapSet, TRUE));

    /* Enable URPF */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3UrpfRouteEnable, TRUE));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3InterfaceUrpfEnable, TRUE));

    /* Egress side VLAN */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid_egr));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, port_egr);
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vid_egr, pbmp, upbmp));

    /* Create egress L3 interface - VID 13 */
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags |= BCM_L3_ADD_TO_ARL;
    sal_memcpy(l3_intf.l3a_mac_addr, router_mac_egr, sizeof(router_mac_egr));
    l3_intf.l3a_vid = vid_egr;
    l3_intf.l3a_mtu = 1400;
    BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf));

    l3_egr_if_egr = l3_intf.l3a_intf_id;
    ingress_if_egr = l3_egr_if_egr;

    /* Use the same ID to allocate the ingress interface (L3_IIF) */
    bcm_l3_ingress_t_init(&l3_ingress);
    l3_ingress.flags = BCM_L3_INGRESS_REPLACE;
    l3_ingress.vrf = vrf;
    l3_ingress.ipmc_intf_id  = l3_egr_if_egr;
    l3_ingress.urpf_mode = bcmL3IngressUrpfLoose;  /* Specify the URPF mode */
    /* l3_ingress.urpf_mode = bcmL3IngressUrpfStrict; */
    /* l3_ingress.urpf_mode = bcmL3IngressUrpfDisable; */
    BCM_IF_ERROR_RETURN(bcm_l3_ingress_create(unit, &l3_ingress, &ingress_if_egr));

    /* Egress side next hop object */
    bcm_l3_egress_t_init(&l3_egress);
    sal_memcpy(l3_egress.mac_addr, nxt_hop_egr, sizeof(nxt_hop_egr));
    l3_egress.vlan = l3_egr_if_egr;   /* Note: not vid_egr! */
    l3_egress.port = port_egr;
    l3_egress.intf = l3_egr_if_egr;
    BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj_egr));

    /* configure VLAN_ID to L3_IIF mapping */
    bcm_vlan_control_vlan_t_init(&vlan_control);
    BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_get(unit, vid_egr, &vlan_control));
    vlan_control.ingress_if = ingress_if_egr;
    vlan_control.flags |= BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK;
    BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_set(unit, vid_egr, vlan_control));

    /* Install the route for DIP, note this is routed out on the port_out */
    bcm_l3_route_t_init(&route_info);
    route_info.l3a_subnet = dip;
    route_info.l3a_ip_mask = mask;
    route_info.l3a_intf = egr_obj_egr;
    BCM_IF_ERROR_RETURN(bcm_l3_route_add(unit, &route_info));

    /* Ingress side VLAN */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid_in));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, port_in);
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vid_in, pbmp, upbmp));
    /* Create vlan and add array of ports to vlan */


    /*
     * Create ingress side egress L3 interface - VID 12
     */
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags |= BCM_L3_ADD_TO_ARL;
    sal_memcpy(l3_intf.l3a_mac_addr, router_mac_in, sizeof(router_mac_egr));
    l3_intf.l3a_vid = vid_in;
    l3_intf.l3a_mtu = 1400;
    BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf));
    l3_egr_if_in = l3_intf.l3a_intf_id;
    ingress_if_in = l3_egr_if_in;

    /* Use the same ID to allocate the ingress interface (L3_IIF) */
    bcm_l3_ingress_t_init(&l3_ingress);
    l3_ingress.flags = BCM_L3_INGRESS_WITH_ID;
    l3_ingress.vrf = vrf;
    l3_ingress.ipmc_intf_id  = l3_egr_if_in;
    l3_ingress.urpf_mode = bcmL3IngressUrpfStrict;  /* Specify the URPF mode */
    /* l3_ingress.urpf_mode = bcmL3IngressUrpfLoose; */
    /* l3_ingress.urpf_mode = bcmL3IngressUrpfDisable; */
    BCM_IF_ERROR_RETURN(bcm_l3_ingress_create(unit, &l3_ingress, &ingress_if_in));

    /* Ingress side next hop object */
    bcm_l3_egress_t_init(&l3_egress);
    sal_memcpy(l3_egress.mac_addr, nxt_hop_in, sizeof(nxt_hop_in));
    l3_egress.vlan = ingress_if_in;   /* Note: not vid_in! */
    l3_egress.port = port_in;
    l3_egress.intf = l3_egr_if_in;
    BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj_in));

    /* configure VLAN_ID to L3_IIF mapping */
    bcm_vlan_control_vlan_t_init(&vlan_control);
    BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_get(unit, vid_in, &vlan_control));
    vlan_control.ingress_if = ingress_if_in;
    vlan_control.flags |= BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK;
    BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_set(unit, vid_in, vlan_control));

    /* Install the route for SIP, note this points to port_in */
    bcm_l3_route_t_init(&route_info);
    route_info.l3a_subnet = sip;
    route_info.l3a_ip_mask = mask;
    route_info.l3a_intf = egr_obj_in;
    BCM_IF_ERROR_RETURN(bcm_l3_route_add(unit, &route_info));

    return BCM_E_NONE;
}

/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in config_l3_urpf())
 *  c) demonstrates the functionality(done in verify()).
 */
bcm_error_t
execute(void)
{
    bcm_error_t rv;
    int unit = 0;
    print "config show; attach; cancun stat; version";
    bshell(unit, "config show; a ; cancun stat; version");

    print "~~~ #1) test_setup(): ** start **";
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return -1;
    }
    print "~~~ #1) test_setup(): ** end **";

    print "~~~ #2) config_l3_urpf(): ** start **";
    if (BCM_FAILURE((rv = config_l3_urpf(unit)))) {
        printf("config_l3_urpf() failed.\n");
        return -1;
    }
    print "~~~ #2) config_l3_urpf(): ** end **";

    print "~~~ #3) verify(): ** start **";
    verify(unit);
    print "~~~ #3) verify(): ** end **";

    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
    print "execute(): Start";
    print execute();
    print "execute(): End";
}
