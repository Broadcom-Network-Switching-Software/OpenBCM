/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenSDK/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : L3 IPv4 Route DSCP queuing remarking
 *
 * Usage    : BCM.0> cint ipv4_route_dscp_queuing_remarking.c
 *
 * config   : l3_config.bcm
 *
 * Log file : ipv4_route_dscp_queuing_remarking_log.txt
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
 *   This Cint example to show configuration of the IPv4 route with DSCP queuing
 *   and remarking using BCM APIs.
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
 *   2) Step2 - Configuration (Done in config_ipv4_route_dscp_queuing_remarking()):
 *   ==============================================================================
 *     a) Configure a basic IPv4 DSCP Queuing and remark QoS mapping functional
 *        scenario. This adds the route in l3 table[route] and does the
 *        necessary configurations of vlan, interface and next hopi.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l3 intf show',
 *        'l3 egress show' and 'l3 defip show'
 *
 *     b) Transmit the known IPv4 unicast packet with DSCP. The contents of the
 *        packet are printed on screen. The packet used matches the route
 *        configured through script.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see that dmac, smac and vlan are all changed as the packet is
 *       routed through the egress port and priority as 3. Also run the
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
 *  In Packet:
 *  ----------
 *   0000 0000 0011 0000 0000 00aa 8100 000A
 *   0800 45FC 002E 0000 0000 40FF BF32 5050
 *   5050 1801 0102 0001 0203 0405 0607 0809
 *   0A0B 0C0D 0E0F 1011 1213 1415 1617 1819
 *   A20E 2B04
 *
 *  Out Packet:
 *  -----------
 *   0000 0000 00bb 0000 0000 0022 8100 6014
 *   0800 4514 002e 0000 0000 3fff c11a 5050
 *   5050 1801 0102 0001 0203 0405 0607 0809
 *   0a0b 0c0d 0e0f 1011 1213 1415 1617 1819
 *   6f5c 56b0
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
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000000110000000000aa8100000A080045FC002E0000000040FFBF325050505018010102000102030405060708090A0B0C0D0E0F10111213141516171819A20E2B04; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "l3 defip show");
    bshell(unit, "show c");

    return;
}

/* Create vlan and add port to vlan */
int
create_vlan_add_port(int unit, bcm_vlan_t vlan, bcm_port_t port)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_pbmp_t pbmp, ubmp;

    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);

    rv = bcm_vlan_create(unit, vlan);
    if ((BCM_FAILURE(rv)) & (rv != BCM_E_EXISTS)) {
        printf("Error vlan: %d port:%d\n", vlan, port);
        return rv;
    }

    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vlan, pbmp, ubmp));

    return BCM_E_NONE;

}

/* Create L3 interface */
int
create_l3_intf(int unit, bcm_mac_t mac, bcm_vlan_t vlan, bcm_if_t *l3_intf_id)
{
    bcm_l3_intf_t l3_intf;

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_vid = vlan;
    sal_memcpy(l3_intf.l3a_mac_addr, mac, sizeof(mac));

    BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf));
    *l3_intf_id = l3_intf.l3a_intf_id;

    return BCM_E_NONE;
}

/* Create L3 egress object */
int
create_egr_obj(int unit, bcm_if_t l3_if, bcm_mac_t mac, bcm_gport_t gport,
               bcm_if_t *egr_if)
{
    bcm_error_t rv;
    bcm_l3_egress_t l3_egr;

    bcm_l3_egress_t_init(&l3_egr);
    l3_egr.intf = l3_if;
    l3_egr.port = gport;
    sal_memcpy(l3_egr.mac_addr, mac, sizeof(mac));
    BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egr, egr_if));

    return BCM_E_NONE;
}

/* Adding subnet address to defip table */
int
add_route(int unit, bcm_ip_t subnet_addr, bcm_ip_t subnet_mask, bcm_if_t egr_if)
{
    bcm_l3_route_t route;

    bcm_l3_route_t_init(&route);
    route.l3a_intf = egr_if;
    route.l3a_subnet = subnet_addr;
    route.l3a_ip_mask = subnet_mask;
    BCM_IF_ERROR_RETURN(bcm_l3_route_add(unit, &route));

    return BCM_E_NONE;
}

/* Adding entry into the L2 station table */
int
add_l2_station_entry(int unit, bcm_mac_t mac, bcm_mac_t mac_mask, bcm_vlan_t vlan, int station_id)
{
    bcm_l2_station_t station;

    bcm_l2_station_t_init(&station);
    station.flags = BCM_L2_STATION_IPV4;
    sal_memcpy(station.dst_mac, mac, sizeof(mac));
    sal_memcpy(station.dst_mac_mask, mac_mask, sizeof(mac_mask));
    station.vlan = vlan;
    station.vlan_mask = vlan ? 0xfff: 0x0;
    BCM_IF_ERROR_RETURN(bcm_l2_station_add(unit, &station_id, &station));

    return BCM_E_NONE;
}


/* Mapping packet dscp to PHB (internal pri and color) */
int
map_dscp_to_phb(int unit, bcm_gport_t gport, int srccp, int mapcp, int int_pri, bcm_color_t cng)
{
    /* Enable port to trust incoming DSCP */
    BCM_IF_ERROR_RETURN(bcm_port_dscp_map_mode_set(unit, gport, BCM_PORT_DSCP_MAP_ALL));

    /* Map packet DSCP to PHB */
    BCM_IF_ERROR_RETURN(bcm_port_dscp_map_set(unit, gport, srccp, mapcp, int_pri | cng ));

    return BCM_E_NONE;
}

/* Mapping internal priority to cosq */
int
map_int_pri_to_cosq(int unit, int int_pri, int cosq)
{
    /* Map internal priority to cosq */
    BCM_IF_ERROR_RETURN(bcm_cosq_mapping_set(unit, int_pri, cosq));

    return BCM_E_NONE;
}

/* Mapping PHB to egress packet DSCP */
int
map_phb_to_dscp(int unit, bcm_gport_t gport, int int_pri, bcm_color_t cng, int egr_pkt_dscp)
{
    /* Enable dscp remark for egress port */
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, gport, bcmPortControlEgressModifyDscp, TRUE));

    /* Map packet PHB to new DSCP */
    BCM_IF_ERROR_RETURN(bcm_port_dscp_unmap_set(unit, gport, int_pri, cng, egr_pkt_dscp));

    return BCM_E_NONE;
}

int
ip_route_dscp_queuing_remark(int unit, bcm_port_t ing_port, bcm_port_t egr_port,
                             bcm_vlan_t ing_vlan, bcm_vlan_t egr_vlan)
{
    bcm_error_t rv;
    bcm_gport_t ing_gport, egr_gport;
    bcm_if_t egr_if, egr_l3_if;

    bcm_mac_t ing_if_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x11};
    bcm_mac_t egr_if_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x22};
    bcm_mac_t dst_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0xbb};
    bcm_mac_t ing_if_mac_mask = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    bcm_ip_t subnet;
    bcm_ip_t mask;

    int station_id = 0;
    int srccp = 63, mapcp = 10, int_pri = 3; /* pkt_dscp, new_dscp, internal priority */
    int cosq = 2;
    int egr_pkt_dscp = 5;  /* dscp remark at egress if needed, else pkt egress with mapcp value */
    bcm_color_t cng = BCM_PRIO_YELLOW, color = bcmColorYellow;

    /* Enable egress mode */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set (unit, bcmSwitchL3EgressMode, 1));

    /* Get gport for ingress and egress ports */
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, ing_port, &ing_gport));
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, egr_port, &egr_gport));

    /* VLAN Configuration */

    /* Create vlan and add port to vlan */
    rv = create_vlan_add_port(unit, ing_vlan, ing_port);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring vlan : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_vlan_add_port(unit, egr_vlan, egr_port);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring vlan : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* IP route configuration */

    /* Create L3 interface */
    rv = create_l3_intf(unit, egr_if_mac, egr_vlan, &egr_l3_if);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring l3 interface : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create L3 egress object */
    rv = create_egr_obj(unit, egr_l3_if, dst_mac, egr_gport, egr_if);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring l3 egress object : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Adding default route to defip table */
    subnet = 0x0;
    mask = 0x0;
    rv = add_route(unit, subnet, mask, egr_if);
    if(BCM_FAILURE(rv)) {
        printf("\nError in adding route: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Adding route to defip table */
    subnet = 0x18010102;
    mask = 0xffffff00;
    rv = add_route(unit, subnet, mask, egr_if);
    if(BCM_FAILURE(rv)) {
        printf("\nError in adding route: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Adding entry into the L2 station table */
    rv = add_l2_station_entry(unit, ing_if_mac, ing_if_mac_mask, ing_vlan, station_id);
    if(BCM_FAILURE(rv)) {
        printf("\nError in adding l2 station entry\n",bcm_errmsg(rv));
        return rv;
    }

    /* L3 QoS configuration */

    /* Mapping packet dscp to PHB (internal pri and color) */
    rv = map_dscp_to_phb(unit, ing_gport, srccp, mapcp, int_pri, cng);
    if(BCM_FAILURE(rv)) {
        printf("\nError in mapping dscp to phb: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Mapping internal priority to cosq */
    rv = map_int_pri_to_cosq(unit, int_pri, cosq);
    if(BCM_FAILURE(rv)) {
        printf("\nError in mapping internal priority to cosq: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Mapping PHB to egress packet DSCP */
    rv = map_phb_to_dscp(unit, egr_gport, int_pri, color, egr_pkt_dscp);
    if(BCM_FAILURE(rv)) {
        printf("\nError in mapping phb to dscp: %s.\n",bcm_errmsg(rv));
        return rv;
    }
}

bcm_error_t
config_ipv4_route_dscp_queuing_remarking(int unit)
{
    bcm_error_t rv;
    ip_route_dscp_queuing_remark(unit, ingress_port, egress_port, 10, 20);

    return BCM_E_NONE;
}

/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in config_ipv4_route_dscp_queuing_remarking())
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

    print "~~~ #2) config_ipv4_route_dscp_queuing_remarking(): ** start **";
    if (BCM_FAILURE((rv = config_ipv4_route_dscp_queuing_remarking(unit)))) {
        printf("config_ipv4_route_dscp_queuing_remarking() failed.\n");
        return -1;
    }
    print "~~~ #2) config_ipv4_route_dscp_queuing_remarking(): ** end **";

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
