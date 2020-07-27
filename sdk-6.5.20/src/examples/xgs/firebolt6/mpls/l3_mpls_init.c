/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenSDK/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : L3 MPLS Tunnel Initiation
 *
 * Usage    : BCM.0> cint l3_mpls_init.c
 *
 * config   : mpls_config.bcm
 *
 * Log file : l3_mpls_init_log.txt
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
 *   This Cint example to show configuration of L3 MPLS tunnel initiation using
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
 *        Tunnel + VC labels.
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
 *       routed through the egress port and also Tunnel + VC labels are pushed
 *       as part of tunnel initiation on egress port. Also run the
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
 *      0000 0000 0003 0000 0000 0002 8100 000c   VID=12
 *      8847 0006 5040 000c 913f 4500 002e 0000   MPLS labels: 101, 201
 *      0000 3fff 9c1d c0a8 0a01 0a0a 0a01 0001
 *      0203 0405 0607 0809 0a0b 0c0d 0e0f 1011
 *      1213 1415 1617 1819 43e0 6dbb
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
    snprintf(str, 512, "tx 1 pbm=%d data=0x000000000001000000AABBCC8100000B08004500002E0000000040FF9B1DC0A80A010A0A0A01000102030405060708090A0B0C0D0E0F101112131415161718190258A932; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "l3 defip show");
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

    /* Use Egress Object mode */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, TRUE));

    /* Allow explicaitly configure ingress intf */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3IngressMode, TRUE));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3IngressInterfaceMapSet, TRUE));

    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, p_port, &p_gport));

    /* create customer vlan */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, c_vlan));
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, c_port);
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, c_vlan, pbmp, ubmp));

    /* Enable L3 lookup on customer side : add intf mac to l2 */
    bcm_l2_addr_t_init(&l2addr, mac_c, c_vlan);
    l2addr.flags |= (BCM_L2_STATIC | BCM_L2_L3LOOKUP);
    BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2addr));

    /*
       bcm_mac_t  mac_mask = {0xff,0xff,0xff,0xff,0xff,0xff};
       int total_mask_len  = 67;
       bcm_l2_station_t l3_station;
       int station_id;

       bcm_l2_station_t_init(&l3_station);
       sal_memcpy(l3_station.dst_mac, mac_c, sizeof(mac_c));
       sal_memcpy(l3_station.dst_mac_mask, mac_mask, sizeof(mac_mask));
       l3_station.vlan      = c_vlan;
       l3_station.vlan_mask = 0xFFF;
       l3_station.priority  = total_mask_len;
       l3_station.flags     = BCM_L2_STATION_IPV4 | BCM_L2_STATION_IPV6;
       print bcm_l2_station_add(unit, &station_id, &l3_station);
       */

    /* create provider vlan */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, p_vlan));
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, p_port);
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, p_vlan, pbmp, ubmp));

    /* create L3 intf for provider vlan */
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_intf_id = intfid;
    l3_intf.l3a_flags = BCM_L3_WITH_ID | BCM_L3_ADD_TO_ARL;
    l3_intf.l3a_mtu = 1500;
    sal_memcpy(l3_intf.l3a_mac_addr, mac_p, 6);
    l3_intf.l3a_vid = p_vlan;
    l3_intf.l3a_vrf = vrf_id;
    BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf));
    printf("l3 intf %x created \n", intfid);

    /*
     * Use the same ID to allocate the ingress interface (L3_IIF)
     * (This is really not needed for L3MPLS init, since we only need
     *  to use EGR_L3_INTF to create the tunnel)
     */
    ingress_if_egr = intfid;

    bcm_l3_ingress_t_init(&l3_ingress);
    l3_ingress.flags = BCM_L3_INGRESS_REPLACE;
    l3_ingress.vrf  = vrf_id;
    l3_ingress.ipmc_intf_id  = intfid;
    BCM_IF_ERROR_RETURN(bcm_l3_ingress_create(unit, &l3_ingress, &ingress_if_egr));

    /* set port.l3_iif to ingress_if_egr */
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, c_port, bcmPortControlL3Ingress, ingress_if_egr));

    /*
     * configure VLAN_ID to L3_IIF mapping
     */
    bcm_vlan_control_vlan_t_init(&vlan_control);
    BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_get(unit, c_vlan, &vlan_control));
    vlan_control.ingress_if = ingress_if_egr;
    BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_set(unit, c_vlan, vlan_control));

    /* configure mpls tunnle lable for l3 intf */
    bcm_mpls_egress_label_t_init(&tunnel_label);
    tunnel_label.flags = BCM_MPLS_EGRESS_LABEL_TTL_SET;
    tunnel_label.label = tun_label;
    tunnel_label.ttl = 64;
    BCM_IF_ERROR_RETURN(bcm_mpls_tunnel_initiator_set(unit, intfid, 1, &tunnel_label));

    /* create L3 egress object */
    bcm_l3_egress_t_init(&l3_egress);
    sal_memcpy(l3_egress.mac_addr, mac_pn, 6);
    l3_egress.intf = intfid;
    l3_egress.vlan = p_vlan;
    l3_egress.port = p_port;  /* p_gport */
    l3_egress.mpls_label = vc_label;
    BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, BCM_L3_ROUTE_LABEL, &l3_egress, &egr_obj));

    /* If you don't want to add VC label
       BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj)); */

    /* add route to egress obj */
    bcm_l3_route_t_init(&l3_route);
    l3_route.l3a_subnet = dip;
    l3_route.l3a_ip_mask = mask;
    l3_route.l3a_intf = egr_obj;
    l3_route.l3a_vrf = vrf_id;
    BCM_IF_ERROR_RETURN(bcm_l3_route_add(unit, &l3_route));

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

