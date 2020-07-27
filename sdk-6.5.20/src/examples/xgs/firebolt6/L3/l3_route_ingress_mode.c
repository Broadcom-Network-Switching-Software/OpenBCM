/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenSDK/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : L3 IPv4 route/ LPM entry with ingress mode
 *
 * Usage    : BCM.0> cint ipv4_route_ingress_mode.c
 *
 * config   : ipv4_route_ingress_mode_config.bcm
 *
 * Log file : ipv4_route_ingress_mode_log.txt
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
 *   This Cint example to show configuration of the IPv4 route entry with
 *   ingress mode using BCM APIs.
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
 *   2) Step2 - Configuration (Done in config_l3_route_ingress_mode()):
 *   ==================================================================
 *     a) Configure a basic IPv4 route/entry with ingress mode functional
 *        scenario. This adds the route in l3 table[route] and does the
 *        necessary configurations of vlan, interface and next hop.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l3 intf show',
 *        'l3 egress show' and 'l3 defip show'
 *
 *     b) Transmit the known IPv4 unicast packet. The contents of the packet
 *        are printed on screen. The packet used matches the route configured
 *        through script.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see that dmac, smac and vlan are all changed as the packet is
 *       routed through the egress port. Also run the 'l3 defip show' to
 *       check the HIT bit status (y i.e Yes) and 'show c' to check the Tx/Rx
 *       packet stats/counters.
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
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));

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

/*
 * Verification:
 *   In Packet:
 *   ----------
 *   00000000aaaa 000000002222 8100 000c
 *   0800 4500 0032 0000 0000 40ff 9b19 0a0a
 *   0a01 c0a8 0a01 0001 0203 0405 0607 0809
 *   0a0b 0c0d 0e0f 1011 1213 1415 1617 1819
 *   1a1b 1c1d 907e 5579 0000 0000
 *
 *   Out Packet:
 *   -----------
 *   000000001111 00000000bbbb 8100 000d
 *   0800 4500 0032 0000 0000 3fff 9c19 0a0a
 *   0a01 c0a8 0a01 0001 0203 0405 0607 0809
 *   0a0b 0c0d 0e0f 1011 1213 1415 1617 1819
 *   1a1b 1c1d 907e 5579 0000 0000
 */
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
    printf("\nSending IPv4 unicast packet to ingress_port:%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000AAAA0000000022228100000C0800450000320000000040FF9B190A0A0A01C0A80A01000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D907E5579; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "l3 defip show");
    bshell(unit, "show c");

    return;
}

bcm_error_t
config_l3_route_ingress_mode(int unit)
{
    bcm_vlan_t vid_in = 12;
    bcm_vlan_t vid_egr = 13;
    bcm_l3_intf_t l3_intf;
    bcm_if_t l3_egr_if_egr;
    int vrf = 0;
    bcm_l3_ingress_t l3_ingress;
    bcm_if_t ingress_if;
    bcm_l3_egress_t l3_egress;
    bcm_if_t egr_obj_egr;
    bcm_vlan_control_vlan_t vlan_control;
    bcm_pbmp_t pbmp, upbmp;
    bcm_l3_route_t route_info;
    bcm_l2_addr_t l2addr;

    bcm_mac_t router_mac_egr  = {0x00,0x00,0x00,0x00,0xBB,0xBB};
    bcm_mac_t router_mac_in  = {0x00,0x00,0x00,0x00,0xAA,0xAA};
    bcm_mac_t nxt_hop_egr  = {0x00,0x00,0x00,0x00,0x11,0x11};
    bcm_mac_t nxt_hop_in  = {0x00,0x00,0x00,0x00,0x22,0x22};

    bcm_ip_t dip = 0xC0A80A01;  /* 192.168.10.1 */
    bcm_ip_t mask = 0xffffff00;  /* 255.255.255.0 */

    /* Use Egress Object mode */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, TRUE));

    /* Allow explicaitly configure ingress intf */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3IngressMode, TRUE));

    /* Enable mapping between VLAN and L3 IIF */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3IngressInterfaceMapSet, TRUE));

    /* Create vlan and add port to vlan */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid_in));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vid_in, pbmp, upbmp));

    /* Egress side VLAN */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid_egr));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, egress_port);
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vid_egr, pbmp, upbmp));

    /* Adding router mac, enable ingress L3 lookup */
    bcm_l2_addr_t l2_addr;

    bcm_l2_addr_t_init(&l2_addr, router_mac_in, vid_in);
    l2_addr.flags |= (BCM_L2_L3LOOKUP | BCM_L2_STATIC);
    BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2_addr));

    /* Create egress L3 interface */
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags |= BCM_L3_ADD_TO_ARL;
    sal_memcpy(l3_intf.l3a_mac_addr, router_mac_egr, sizeof(router_mac_egr));
    l3_intf.l3a_vid = vid_egr;
    l3_intf.l3a_mtu = 1500;
    BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf));
    l3_egr_if_egr = l3_intf.l3a_intf_id;

    /*
     * Use the same ID to allocate the ingress interface (L3_IIF)
     * (This is really not needed for the routing to work, we are dealing with
     *  one direction)
     */
    ingress_if = l3_egr_if_egr;
    bcm_l3_ingress_t_init(&l3_ingress);
    l3_ingress.flags = BCM_L3_INGRESS_REPLACE;
    l3_ingress.vrf = vrf;
    l3_ingress.ipmc_intf_id  = l3_egr_if_egr;
    BCM_IF_ERROR_RETURN(bcm_l3_ingress_create(unit, &l3_ingress, &ingress_if));

    /* Egress side next hop object */
    bcm_l3_egress_t_init(&l3_egress);
    sal_memcpy(l3_egress.mac_addr, nxt_hop_egr, sizeof(nxt_hop_egr));
    l3_egress.vlan = l3_egr_if_egr;
    l3_egress.port = egress_port;
    l3_egress.intf = l3_egr_if_egr;
    BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj_egr));

    /* configure VLAN_ID to L3_IIF mapping */
    bcm_vlan_control_vlan_t_init(&vlan_control);
    BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_get(unit, vid_egr, &vlan_control));
    vlan_control.ingress_if = ingress_if;
    vlan_control.flags |= BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK;
    BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_set(unit, vid_egr, vlan_control));

    /* Install the route for DIP, note this is routed out on the egress_port */
    bcm_l3_route_t_init(&route_info);
    route_info.l3a_subnet = dip;
    route_info.l3a_ip_mask = mask;
    route_info.l3a_intf = egr_obj_egr;
    BCM_IF_ERROR_RETURN(bcm_l3_route_add(unit, &route_info));

    return BCM_E_NONE;
}

/*
 * execute:
 *  This function does the following
 *  a)test setup
 *  b)actual configuration (Done in config_l3_route_ingress_mode())
 *  c)demonstrates the functionality(done in verify()).
 */
bcm_error_t
execute()
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

    print "~~~ #2) config_l3_route_ingress_mode(): ** start **";
    if (BCM_FAILURE((rv = config_l3_route_ingress_mode(unit)))) {
        printf("config_l3_route_ingress_mode() failed.\n");
        return -1;
    }
    print "~~~ #2) config_l3_route_ingress_mode(): ** end **";

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
