/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : L3 MPLS Tunnel Initiation - Pushing max labels (8 Tunnel + 1 VC)
 *
 * Usage    : BCM.0> cint td4_hsdk_l3mpls_init_push_max_labels.c
 *
 * config   : bcm56880_a0-generic-32x400.config.yml
 *
 * Log file : td4_hsdk_l3mpls_init_push_max_labels_log.txt
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
 *   This Cint example to show configuration of L3 MPLS tunnel push max labels using
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
 *   2) Step2 - Configuration (Done in config_l3_mpls_init()):
 *   =========================================================
 *     a) Configure a basic L3 MPLS Tunnel Initiation functional scenario and
 *        does the necessary configurations of vlan, interface, next hop and
 *        8 Tunnel labels + 1 VC label
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l3 intf show',
 *        and 'l3 egress show'
 *
 *     b) Transmit the known IPv4 unicast packet. The contents of the packet
 *        are printed on screen. The packet used matches the route configured
 *        through script.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see that dmac, smac and vlan are all changed as the packet is
 *       routed through the egress port and also 8 Tunnel labels + 1 VC label
 *       are pushed as part of tunnel initiation on egress port. And 'show c'
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
 *   Access port >>>>>  XGS >>>>> Provider port
 *   (IP packet)                (MPLS packet with 2 labels: tunnel label + VC label)
 *  In Packet:
 *  ----------
 *      00 00 00 00 00 01 00 00 00 AA BB CC 81 00 00 0B  VID=11
 *      08 00 45 00 00 2E 00 00 00 00 40 FF 9B 1D C0 A8  DIP=10.10.10.1 SIP=192.168.10.1
 *      0A 01 0A 0A 0A 01 00 01 02 03 04 05 06 07 08 09
 *      0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19
 *      02 58 A9 32
 *
 *  Out Packet:
 *  -----------
 *      00 00 00 00 00 03 00 00 00 00 00 02 81 00 00 0C  VID=12
 *      88 47 00 06 C0 40 00 06 B0 40 00 06 A0 40 00 06  MPLS Labels 101 to 108, VC- 201
 *      90 40 00 06 80 40 00 06 70 40 00 06 60 40 00 06
 *      50 40 00 0C 91 3F 45 00 00 2E 00 00 00 00 3F FF
 *      9C 1D C0 A8 0A 01 0A 0A 0A 01 00 01 02 03 04 05
 *      06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13 14 15
 *      16 17 18 19 02 58 A9 32
 */
void
verify(int unit)
{
    char str[512];

    bshell(unit, "vlan show");
    bshell(unit, "l3 intf show");
    bshell(unit, "l3 egress show");
    bshell(unit, "clear c");
    printf("\nSending IPv4 unicast packet to ingress_port:%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x000000000001000000AABBCC8100000B08004500002E0000000040FF9B1DC0A80A010A0A0A01000102030405060708090A0B0C0D0E0F101112131415161718190258A932; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "show c");

    return;
}

bcm_error_t
config_l3_mpls_init(int unit)
{
    bcm_error_t rv;
    bcm_vlan_t c_vlan = 11;
    bcm_vlan_t p_vlan = 12;
    bcm_port_t c_port = ingress_port;
    bcm_port_t p_port = egress_port;
    bcm_gport_t p_gport;
    bcm_pbmp_t pbmp, ubmp;
    bcm_vlan_control_vlan_t vlan_control;
    bcm_l2_addr_t l2addr;
    bcm_mac_t mac_c = {0x00,0x00,0x00,0x00,0x00,0x01};  /* intf mac */
    bcm_mac_t mac_p = {0x00,0x00,0x00,0x00,0x00,0x02};  /* intf mac */
    bcm_mac_t mac_pn = {0x00,0x00,0x00,0x00,0x00,0x03};  /* next hop mac */
    bcm_l3_intf_t l3_intf;
    bcm_mpls_egress_label_t tunnel_label;
    bcm_mpls_tunnel_encap_t tunnel_encap;
    bcm_l3_egress_t  l3_egress;
    bcm_l3_ingress_t l3_ingress;
    bcm_l3_route_t  l3_route;
    int vrf_id = 33;
    bcm_if_t intfid = 5;
    bcm_if_t ingress_if_egr;
    bcm_mpls_label_t tun_label = 101;
    bcm_mpls_label_t vc_label = 201;
    bcm_if_t egr_obj;
    bcm_vlan_action_set_t vla;
    bcm_if_t  tun_intf_id;
    bcm_ip_t dip = 0x0a0a0a01;  /* 10.10.10.1 */
    bcm_ip_t mask = 0xffffff00;
    int i = 0;

    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, p_port, &p_gport));

    /* create customer vlan */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, c_vlan));
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, c_port);
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, c_vlan, pbmp, ubmp));

    /* create provider vlan */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, p_vlan));
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, p_port);
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, p_vlan, pbmp, ubmp));

    /* Enable L3 lookup on customer side : add intf mac to l2 */
    bcm_l2_addr_t_init(&l2addr, mac_c, c_vlan);
    l2addr.flags |= (BCM_L2_STATIC | BCM_L2_L3LOOKUP);
    BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2addr));

    /* MPLS tunnel encap configuration */
    bcm_mpls_tunnel_encap_t_init(&tunnel_encap);
    for (i = 0; i < 8; i++) {
        tunnel_encap.label_array[i].label = tun_label + i;
        tunnel_encap.label_array[i].ttl = 64;
        tunnel_encap.label_array[i].flags = BCM_MPLS_EGRESS_LABEL_TTL_SET;
    }
    tunnel_encap.num_labels = i;
    BCM_IF_ERROR_RETURN(bcm_mpls_tunnel_encap_create(unit, 0, &tunnel_encap));
    print tunnel_encap.tunnel_id;

    /* create L3 intf for provider side */
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_vid = p_vlan;
    l3_intf.l3a_mtu = 1500;
    l3_intf.l3a_tunnel_idx = tunnel_encap.tunnel_id;
    l3_intf.l3a_mpls_label = vc_label;
    //l3_intf.l3a_mpls_exp = 5;
    l3_intf.l3a_mpls_flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf));
    printf("l3 intf %x created \n", intfid);
    print l3_intf.l3a_intf_id;

    ingress_if_egr = intfid;
    bcm_l3_ingress_t_init(&l3_ingress);
    l3_ingress.flags = BCM_L3_INGRESS_REPLACE;
    l3_ingress.vrf  = vrf_id;
    l3_ingress.ipmc_intf_id  = intfid;
    BCM_IF_ERROR_RETURN(bcm_l3_ingress_create(unit, &l3_ingress, &ingress_if_egr));

    /*
     * configure VLAN_ID to L3_IIF mapping
     */
    bcm_vlan_control_vlan_t_init(&vlan_control);
    BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_get(unit, c_vlan, &vlan_control));
    vlan_control.ingress_if = ingress_if_egr;
    BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_set(unit, c_vlan, vlan_control));

    /* create  overlay L3 egress object */
    bcm_l3_egress_t_init(&l3_egress);
    l3_egress.intf = l3_intf.l3a_intf_id;
    l3_egress.vlan = p_vlan;
    l3_egress.port = p_port;  /* p_gport */
    BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj));

    /* new APIs to add underlay L3 Interface */
    bcm_l3_intf_t ul_l3_intf;
    bcm_l3_intf_t_init(&ul_l3_intf);
    ul_l3_intf.l3a_flags = BCM_L3_WITH_ID;
    ul_l3_intf.l3a_intf_flags = BCM_L3_INTF_UNDERLAY;
    ul_l3_intf.l3a_intf_id = 1 + 0x4000; /* Underlay interface ID offset.*/
    sal_memcpy(ul_l3_intf.l3a_mac_addr, mac_p, sizeof(mac_c));
    ul_l3_intf.l3a_vrf = vrf_id;
    BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &ul_l3_intf));

    /* Create underlay L3 egress object */
    bcm_l3_egress_t ul_l3_egress;
    bcm_if_t ul_egr_obj_id;
    bcm_l3_egress_t_init(&ul_l3_egress);
    sal_memcpy(ul_l3_egress.mac_addr, mac_pn, sizeof(mac_pn));
    //new assignment
    ul_l3_egress.intf = ul_l3_intf.l3a_intf_id;
    ul_l3_egress.port = p_port;
    ul_l3_egress.flags2 = BCM_L3_FLAGS2_UNDERLAY;
    BCM_IF_ERROR_RETURN(bcm_l3_egress_create(0, 0, &ul_l3_egress, &ul_egr_obj_id));

    /* Adding IP address to host table */
    bcm_l3_host_t host;
    bcm_l3_host_t_init(&host);
    host.l3a_intf = egr_obj;
    host.l3a_ul_intf = ul_egr_obj_id;
    host.l3a_ip_addr = dip;
    host.l3a_vrf = vrf_id;
    BCM_IF_ERROR_RETURN(bcm_l3_host_add(unit, &host));

    return BCM_E_NONE;
}

/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in config_l3_mpls_init())
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

    print "~~~ #2) config_l3_mpls_init(): ** start **";
    if (BCM_FAILURE((rv = config_l3_mpls_init(unit)))) {
        printf("config_l3_mpls_init() failed.\n");
        return -1;
    }
    print "~~~ #2) config_l3_mpls_init(): ** end **";

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

