/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenSDK/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : L3 MPLS Tunnel Termination
 *
 * Usage    : BCM.0> cint l3_mpls_term.c
 *
 * config   : mpls_config.bcm
 *
 * Log file : l3_mpls_term_log.txt
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
 *   This Cint example to show configuration of L3 MPLS tunnel termination using
 *   BCM APIs.
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
 *   2) Step2 - Configuration (Done in config_l3_mpls_term()):
 *   =========================================================
 *     a) Configure a basic L3 MPLS Tunnel termination functional scenario and
 *        does the necessary configurations of vlan, interface, next hop and
 *        Tunnel + VC labels with POP action.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l3 intf show',
 *        'l3 egress show' and 'l3 defip show'
 *
 *     b) Transmit the MPLS packet with IP unicast payload. The contents of the packet
 *        are printed on screen. The packet used matches the route configured
 *        through script.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see that dmac, smac and vlan are all changed as the packet is
 *       routed through the egress port after Tunnel + VC labels are POPed
 *       as part of tunnel termination on ingress port. Also run the
 *       'l3 defip show' to check the HIT bit status (y i.e Yes) and 'show c'
 *       to check the Tx/Rx packet stats/counters.
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
 *  (egress_port) Access port <<<<<  XGS <<<<< Provider port (ingress_port)
 *   (IP packet)                               (MPLS packet with 2 labels:
 *                                              tunnel label + VC label)
 *  In Packet:
 *  ----------
 *  00 00 00 00 13 03 00 00 00 00 00 AA 81 00 00 0C  VID 12
 *  88 47 00 30 10 40 00 30 21 40 45 00 00 40 00 00  MPLS Labels: 769, 770
 *  00 00 40 FF 9B 0B 0A 0A 0A 01 C0 A8 0A 01 00 01  DIP: 192.168.10.1 SIP 10.10.10.1
 *  02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11
 *  12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F 20 21
 *  22 23 24 25 26 27 28 29 2A 2B E3 12 8E DB
 *
 *  Out Packet:
 *  -----------
 *  0000 0013 0202 0000 0013 0201 8100 000b  VID 11
 *  0800 4500 0040 0000 0000 3fff 9c0b 0a0a  DIP: 192.168.10.1 SIP 10.10.10.1
 *  0a01 c0a8 0a01 0001 0203 0405 0607 0809
 *  0a0b 0c0d 0e0f 1011 1213 1415 1617 1819
 *  1a1b 1c1d 1e1f 2021 2223 2425 2627 2829
 *  2a2b 8b60 d787
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
    printf("\nSending MPLS terminated packet to ingress_port:%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000013030000000000AA8100000C88470030104000302140450000400000000040FF9B0B0A0A0A01C0A80A01000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2BE3128EDB; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "l3 defip show");
    bshell(unit, "show c");

    return;
}

bcm_error_t
config_l3_mpls_term(int unit)
{
    bcm_error_t rv;
    bcm_vlan_t c_vlan = 11;
    bcm_vlan_t p_vlan = 12;
    bcm_port_t c_port = egress_port;
    bcm_port_t p_port = ingress_port;
    bcm_gport_t c_gport, p_gport;
    bcm_pbmp_t pbmp, ubmp;
    bcm_mpls_vpn_config_t vpn_info;
    bcm_if_t l3_intf_id = 5;
    bcm_vrf_t vrf_id = 33;
    bcm_vpn_t vpn_id;
    bcm_mac_t local_mac_p = {0x00, 0x00, 0x00, 0x00, 0x13, 0x03};
    bcm_mac_t local_mac_c = {0x00, 0x00, 0x00, 0x13, 0x02, 0x01};
    bcm_mac_t nh_mac_c    = {0x00, 0x00, 0x00, 0x13, 0x02, 0x02};
    bcm_mac_t nh_mac_p    = {0x00, 0x00, 0x00, 0x00, 0x00, 0xAA};
    bcm_ip_t dip = 0xC0A80A01;  /* 192.168.10.1 */
    bcm_ip_t mask = 0xffffff00;
    uint32 tunnel_label_term = 0x301;
    uint32 vc_label_term = 0x302;
    bcm_mpls_tunnel_switch_t tunnel_switch;
    bcm_l3_intf_t l3_intf;
    bcm_l3_egress_t l3_egress;
    bcm_if_t egr_obj;
    bcm_l3_route_t  l3_route;
    bcm_l3_ingress_t l3_ingress;
    bcm_if_t ingress_if;
    bcm_vlan_control_vlan_t vlan_control;

    /* Use Egress Object mode */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1));

    /* allow explicaitly configure ingress intf */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3IngressMode, 1));
    /* bcm_switch_control_set(unit, bcmSwitchL3IngressInterfaceMapSet, TRUE); */

    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, c_port, &c_gport));
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, p_port, &p_gport));
    printf("customer_gport=0x%x, provider_gport=0x%x\n", c_gport, p_gport);

    /* Create L3 MPLS VPN */
    bcm_mpls_vpn_config_t_init(&vpn_info);
    vpn_info.flags = BCM_MPLS_VPN_L3;
    vpn_info.lookup_id = vrf_id;
    BCM_IF_ERROR_RETURN(bcm_mpls_vpn_id_create(unit, &vpn_info));
    vpn_id = vpn_info.vpn;
    printf("vpn 0x%x created \n", vpn_id);

    /* Create Service provider VLAN */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, p_vlan));
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, p_port);
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, p_vlan, pbmp, ubmp));

    /* Create Customer VLAN */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, c_vlan));
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, c_port);
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, c_vlan, pbmp, ubmp));

    /* Create egress L3 interface to customer side */
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags |= BCM_L3_ADD_TO_ARL | BCM_L3_WITH_ID;
    l3_intf.l3a_intf_id = l3_intf_id;
    sal_memcpy(l3_intf.l3a_mac_addr, local_mac_c, 6);
    l3_intf.l3a_vid = c_vlan;
    l3_intf.l3a_vrf = vrf_id;
    BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf));
    printf("l3 intf = %d\n", l3_intf.l3a_intf_id);

    /* Use the same ID to allocate the ingress interface (L3_IIF) */
    ingress_if = l3_intf_id;

    bcm_l3_ingress_t_init(&l3_ingress);
    l3_ingress.flags = BCM_L3_INGRESS_REPLACE;
    l3_ingress.vrf = vrf_id;
    l3_ingress.ipmc_intf_id  = l3_intf_id;
    BCM_IF_ERROR_RETURN(bcm_l3_ingress_create(unit, &l3_ingress, &ingress_if));

    /*
     * configure VLAN_ID to L3_IIF mapping
     bcm_vlan_control_vlan_t_init(&vlan_control);
     print bcm_vlan_control_vlan_get(unit, c_vlan, &vlan_control);
     vlan_control.ingress_if = ingress_if;
     print bcm_vlan_control_vlan_set(unit, c_vlan, vlan_control);
     */

    /* Create Egress object to customer side */
    bcm_l3_egress_t_init(&l3_egress);
    sal_memcpy(l3_egress.mac_addr, nh_mac_c, 6);
    l3_egress.intf     = l3_intf_id;
    l3_egress.vlan     = c_vlan;
    l3_egress.port     = c_gport;
    BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj));

    /* Add route from provider to Customer
     * (so IP portion will be routed after label popping) */
    bcm_l3_route_t_init(&l3_route);
    l3_route.l3a_subnet = dip;
    l3_route.l3a_ip_mask = mask;
    l3_route.l3a_intf = egr_obj;
    l3_route.l3a_vrf = vrf_id;
    BCM_IF_ERROR_RETURN(bcm_l3_route_add(unit, &l3_route));

    /* Add MPLS table entry for incoming label matching */
    bcm_mpls_tunnel_switch_t_init (&tunnel_switch);
    tunnel_switch.label   = tunnel_label_term;
    tunnel_switch.port    = BCM_GPORT_INVALID;
    tunnel_switch.action  = BCM_MPLS_SWITCH_ACTION_POP;
    tunnel_switch.vpn     = vpn_id;
    tunnel_switch.ingress_if = ingress_if;
    BCM_IF_ERROR_RETURN(bcm_mpls_tunnel_switch_add(unit, &tunnel_switch));

    /* Add MPLS table entry for incoming label matching */
    bcm_mpls_tunnel_switch_t_init (&tunnel_switch);
    tunnel_switch.label   = vc_label_term;
    tunnel_switch.port    = BCM_GPORT_INVALID;
    tunnel_switch.action  = BCM_MPLS_SWITCH_ACTION_POP;
    tunnel_switch.vpn     = vpn_id;
    tunnel_switch.ingress_if = ingress_if;
    BCM_IF_ERROR_RETURN(bcm_mpls_tunnel_switch_add(unit, &tunnel_switch));

    /* Enable MPLS Lookup for incoming packet */
    BCM_IF_ERROR_RETURN(bcm_l2_tunnel_add(unit, local_mac_p, p_vlan));
    return BCM_E_NONE;
}

/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in config_l3_mpls_term())
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

    print "~~~ #2) config_l3_mpls_term(): ** start **";
    if (BCM_FAILURE((rv = config_l3_mpls_term(unit)))) {
        printf("config_l3_mpls_term() failed.\n");
        return -1;
    }
    print "~~~ #2) config_l3_mpls_term(): ** end **";

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

