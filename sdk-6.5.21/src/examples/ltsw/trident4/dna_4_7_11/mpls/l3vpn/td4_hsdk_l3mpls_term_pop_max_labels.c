/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : L3 MPLS Tunnel Termination - Pop Max labels
 *
 * Usage    : BCM.0> cint td4_hsdk_l3mpls_term_pop_max_labels.c
 *
 * config   : bcm56880_a0-generic-32x400.config.yml
 *
 * Log file : td4_hsdk_l3mpls_term_pop_max_labels_log.txt
 *
 * Script Verified Version: HSDK 6.5.19 & DNA_4_5_6
 *
 * RELEASE_NOTE: MPLS feature supports from SDK 6.5.19 onwards
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
 *   This Cint example to show configuration of L3 MPLS tunnel termination pop
 *   upto 3 labels using BCM APIs.
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
 *        pop upto 2 Tunnel + 1 VC labels with POP action.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l3 intf show' and
 *        'l3 egress show'
 *
 *     b) Transmit the MPLS packet with IP unicast payload. The contents of the packet
 *        are printed on screen. The packet used matches the route configured
 *        through script.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see that dmac, smac and vlan are all changed as the packet is
 *       routed through the egress port after 2 Tunnel labels + 1 VC label are POPed
 *       as part of tunnel termination on ingress port. Also run the 'show c'
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
    bcm_field_entry_t        entry;
    bcm_field_group_config_t group_config;

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    group_config.priority = 1;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    printf("ingress_port_setup configured for ingress_port : %d\n", port);

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
    bcm_field_entry_t        entry;
    bcm_field_group_config_t group_config;

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDrop);
    group_config.priority = 2;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    printf("egress_port_setup configured for egress_port : %d\n", port);

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
 *   (IP packet)                               (MPLS packet with 3 labels:
 *                                              2 tunnel labels + 1 VC label)
 *  In Packet:
 *  ----------
 *  00 00 00 00 13 03 00 00 00 00 00 AA 81 00 00 0C  VID 12
 *  88 47 00 30 10 40 00 30 20 40 00 30 21 40 45 00 00 40 00 00  MPLS Labels: 769, 770, 771
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

    bshell(unit, "vlan show");
    bshell(unit, "l3 intf show");
    bshell(unit, "l3 egress show");
    bshell(unit, "clear c");
    printf("\nSending MPLS terminated packet to ingress_port:%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000013030000000000AA8100000C8847003010400030204000303140450000400000000040FF9B0B0A0A0A01C0A80A01000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2BE3128EDB; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "sleep 2");
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
    uint32 label_term = 0x301;
    bcm_mpls_tunnel_switch_t tunnel_switch;
    bcm_l3_intf_t l3_intf;
    bcm_l3_egress_t l3_egress;
    bcm_if_t egr_obj;
    bcm_l3_route_t  l3_route;
    bcm_l3_ingress_t l3_ingress;
    bcm_if_t ingress_if;

    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, c_port, &c_gport));
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, p_port, &p_gport));
    printf("customer_gport=0x%x, provider_gport=0x%x\n", c_gport, p_gport);

    /* Note: L3 VPN ID is not needed for L3 MPLS VPN in TD4.*/

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

    /* Create L3 interface  */
    bcm_l3_intf_t_init(&l3_intf);
    sal_memcpy(l3_intf.l3a_mac_addr, local_mac_c, 6);
    l3_intf.l3a_vid = c_vlan;
    l3_intf.l3a_vrf = vrf_id;
    l3_intf.l3a_mtu = 1500;
    BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf));
    printf("l3 intf = %d\n", l3_intf.l3a_intf_id);

    /* Use the same ID to allocate the ingress interface (L3_IIF) */
    ingress_if = l3_intf_id;
    bcm_l3_ingress_t_init(&l3_ingress);
    l3_ingress.flags = BCM_L3_INGRESS_REPLACE;
    l3_ingress.vrf = vrf_id;
    l3_ingress.ipmc_intf_id  = l3_intf_id;
    BCM_IF_ERROR_RETURN(bcm_l3_ingress_create(unit, &l3_ingress, &ingress_if));

    /* Create Egress object to customer side */
    bcm_l3_egress_t_init(&l3_egress);
    sal_memcpy(l3_egress.mac_addr, nh_mac_c, 6);
    l3_egress.intf     = l3_intf.l3a_intf_id;
    l3_egress.vlan     = c_vlan;
    l3_egress.port     = c_gport;
    BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj));

    /* Adding IP address to host table */
    bcm_l3_host_t host;
    bcm_l3_host_t_init(&host);
    host.l3a_intf = egr_obj;
    host.l3a_ip_addr = dip;
    host.l3a_vrf = vrf_id;
    BCM_IF_ERROR_RETURN(bcm_l3_host_add(unit, &host));

    /* Add MPLS table entry for incoming label matching
     * and Pop upto 3 labels */
    bcm_mpls_tunnel_switch_t_init (&tunnel_switch);
    int i = 0;
    for (i = 0; i < 3; i++) {
        tunnel_switch.label   = label_term + i;
        tunnel_switch.port    = BCM_GPORT_INVALID;
        tunnel_switch.action  = BCM_MPLS_SWITCH_ACTION_POP;
        tunnel_switch.ingress_if = ingress_if;
        BCM_IF_ERROR_RETURN(bcm_mpls_tunnel_switch_add(unit, &tunnel_switch));
    }

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

