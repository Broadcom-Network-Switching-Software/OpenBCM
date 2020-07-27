/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenSDK/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : L3 MPLS Tunnel Termination
 *
 * Usage    : BCM.0> cint l3_mpls_vpn_pop_qos.c
 *
 * config   : mpls_config.bcm
 *
 * Log file : l3_mpls_vpn_pop_qos_log.txt
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
 *   This Cint example to show configuration of L3 MPLS tunnel termination
 *   with QoS using BCM APIs.
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
 *   2) Step2 - Configuration (Done in config_l3_mpls_vpn_pop_qos()):
 *   =========================================================
 *     a) Configure a basic L3 MPLS Tunnel termination functional scenario and
 *        does the necessary configurations of vlan, interface, next hop and
 *        Tunnel + VC labels with POP action.
 *     b) COnfigure a QoS remapping from incoming packet EXP to internal priority.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l3 intf show',
 *        'l3 egress show' and 'l3 defip show'
 *
 *     b) Transmit the MPLS packet with TWO labels & EXP as 3. The contents
 *        of the packet are printed on screen.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see that dmac, smac and vlan are all changed as the packet is
 *       routed through the egress port after Tunnel + VC labels are POPed
 *       as part of tunnel termination on ingress port. We can observe that
 *       exp is mapped to CoS 3 and int_proi as 6 on egress side. Also run the
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
 *   0000 0000 2222 0000 0000 0002 8100 0014
 *   8847 1234 5640 6789 a540 4500 0026 0000
 *   0000 40ff 3db8 1414 1402 0a0a 0a02 0001
 *   0203 0405 0607 0809 0a0b 0c0d 0e0f 1011
 *   9dad 84e9
 *
 * Out Packet:
 * -----------
 *  0000 0000 0002 0000 0000 1111 8100 c00a
 *  0800 4500 0026 0000 0000 3fff 3eb8 1414
 *  1402 0a0a 0a02 0001 0203 0405 0607 0809
 *  0a0b 0c0d 0e0f 1011 9dad 84e9 4ccd a19a
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
    snprintf(str, 512, "tx 1 pbm=%d data=0x000000002222000000000002810000148847123456406789a540450000260000000040ff3db8141414020a0a0a02000102030405060708090a0b0c0d0e0f10119dad84e9; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "l3 defip show");
    bshell(unit, "show c");

    return;
}

/* Create vlan and add port to vlan */
int
vlan_create_add_port(int unit, int vid, int port)
{
    bcm_pbmp_t pbmp, upbmp;

    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    return bcm_vlan_port_add(unit, vid, pbmp, upbmp);
}

/* Create vlan and add array of ports to vlan */
int
vlan_create_add_ports(int unit, int vid, int count, bcm_port_t *ports)
{
    bcm_pbmp_t pbmp, upbmp;
    int i;

    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);

    for (i = 0; i < count; i++) {
        BCM_PBMP_PORT_ADD(pbmp, ports[i]);
    }

    return bcm_vlan_port_add(unit, vid, pbmp, upbmp);
}

int
create_l3_interface(int unit, int flags, bcm_mac_t local_mac, int vid, bcm_if_t *intf_id)
{
    bcm_l3_intf_t l3_intf;
    bcm_error_t rv = BCM_E_NONE;

    /* L3 interface and egress object for access */
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags |= flags;
    sal_memcpy(l3_intf.l3a_mac_addr, local_mac, sizeof(local_mac));
    l3_intf.l3a_vid = vid;
    l3_intf.l3a_intf_id = *intf_id;
    rv = bcm_l3_intf_create(unit, &l3_intf);
    *intf_id = l3_intf.l3a_intf_id;

    return rv;
}

/* Create L3 egress object */
int
create_egr_obj(int unit, int l3_if, bcm_mac_t nh_mac, bcm_gport_t gport,
                   bcm_if_t *egr_obj_id)
{
    bcm_l3_egress_t l3_egress;

    bcm_l3_egress_t_init(&l3_egress);
    sal_memcpy(l3_egress.mac_addr,  nh_mac, sizeof(nh_mac));
    /* l3_egress.vlan = l3_if; vid; */
    l3_egress.intf  = l3_if;
    l3_egress.port = gport;
    return bcm_l3_egress_create(unit, 0, &l3_egress, egr_obj_id);
}

/* Create L3 MPLS VPN */
int
create_l3_mpls_vpn(int unit, int vrf, bcm_vpn_t *vpn_id)
{
    bcm_mpls_vpn_config_t vpn_info;

    bcm_mpls_vpn_config_t_init(&vpn_info);
    vpn_info.flags = BCM_MPLS_VPN_L3;
    vpn_info.lookup_id = vrf;
    BCM_IF_ERROR_RETURN(bcm_mpls_vpn_id_create(unit, &vpn_info));
    *vpn_id = vpn_info.vpn;

    return BCM_E_NONE;
}

/* Add subnet address to defip table */
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

/* Create L3 ingress interface */
int
create_l3_ingress_intf(int unit, bcm_vrf_t vrf, bcm_if_t *ing_l3_if)
{
    bcm_l3_ingress_t l3_ingress_intf;

    bcm_l3_ingress_t_init(&l3_ingress_intf);
    l3_ingress_intf.vrf = vrf;
    BCM_IF_ERROR_RETURN(bcm_l3_ingress_create(unit, &l3_ingress_intf, ing_l3_if));

    return BCM_E_NONE;
}

/* Add MPLS label entry with action POP */
int
add_mpls_label_entry(int unit, bcm_gport_t gport, bcm_mpls_label_t mpls_label_term,
                     bcm_if_t ing_l3_if, bcm_vpn_t vpn_id, int exp_map_id)
{
    bcm_mpls_tunnel_switch_t tunnel_switch;

    /* MPLS_ENTRY:EXP_MAPPING_PTR --> ING_MPLS_EXP_MAPPING */
    bcm_mpls_tunnel_switch_t_init(&tunnel_switch);
    tunnel_switch.flags = BCM_MPLS_SWITCH_INT_PRI_MAP;
    tunnel_switch.label = mpls_label_term;
    tunnel_switch.ingress_if = ing_l3_if;
    tunnel_switch.port = BCM_GPORT_INVALID;
    tunnel_switch.action = BCM_MPLS_SWITCH_ACTION_POP;
    tunnel_switch.vpn = vpn_id;
    tunnel_switch.exp_map = exp_map_id;
    BCM_IF_ERROR_RETURN(bcm_mpls_tunnel_switch_add(unit, tunnel_switch));

    return BCM_E_NONE;
}

/* Add entry to the L2 station table */
int
add_l2_station_entry(int unit, bcm_mac_t mac, bcm_mac_t mac_mask, bcm_vlan_t vlan, int station_id)
{
    bcm_l2_station_t station;

    bcm_l2_station_t_init(&station);
    station.flags = BCM_L2_STATION_MPLS;
    sal_memcpy(station.dst_mac, mac, sizeof(mac));
    sal_memcpy(station.dst_mac_mask, mac_mask, sizeof(mac_mask));
    station.vlan = vlan;
    station.vlan_mask = vlan ? 0xfff: 0x0;
    BCM_IF_ERROR_RETURN(bcm_l2_station_add(unit, &station_id, &station));

    return BCM_E_NONE;
}

/* Map EXP to PHB (internal priority and color) */
int
map_exp_to_phb(int unit, int exp, int int_pri, bcm_color_t cng, int* exp_map_id)
{
    bcm_mpls_exp_map_t ing_exp_map;

    /* Create ingress exp map */
    BCM_IF_ERROR_RETURN(bcm_mpls_exp_map_create(unit, BCM_MPLS_EXP_MAP_INGRESS, exp_map_id));

    /* map exp to int_pri and cng */
    bcm_mpls_exp_map_t_init(&ing_exp_map);
    ing_exp_map.priority = int_pri;
    ing_exp_map.color = cng;
    ing_exp_map.exp = exp;
    BCM_IF_ERROR_RETURN(bcm_mpls_exp_map_set(unit, *exp_map_id, &ing_exp_map));

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


int
l3_mpls_vpn_qos(int unit, bcm_port_t customer_port, bcm_port_t provider_port,
                     bcm_vlan_t customer_vlan, bcm_vlan_t provider_vlan)
{
    bcm_error_t rv;
    bcm_if_t ing_l3_if, egr_l3_if, egr_if;
    bcm_port_t customer_gport, provider_gport;
    bcm_vrf_t vrf = 0;
    bcm_vpn_t vpn_id = 0;
    bcm_mpls_label_t tunnel_label = 0x12345, vc_label = 0x6789a;
    int exp_map_id = 0, station_id = 0;
    int int_pri = 6, exp = 3, cosq = 2;
    bcm_color_t cng = bcmColorYellow;

    bcm_mac_t customer_if_mac = {0x00, 0x00, 0x00, 0x00, 0x11, 0x11};
    bcm_mac_t provider_if_mac = {0x00, 0x00, 0x00, 0x00, 0x22, 0x22};
    bcm_mac_t dst_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
    bcm_mac_t provider_if_mac_mask = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    bcm_ip_t subnet = 0x0;
    bcm_ip_t mask = 0x0;

    /* Enable ingress and egress modes */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3IngressMode, 1));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1));

    /* Get gport for customer and provider ports */
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, customer_port, &customer_gport));
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, provider_port, &provider_gport));

    /* VLAN Configuration */

    /* Create vlan and add port to vlan */
    rv = vlan_create_add_port(unit, customer_vlan, customer_port);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring vlan : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = vlan_create_add_port(unit, provider_vlan, provider_port);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring vlan : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create L3 MPLS VPN */
    rv = create_l3_mpls_vpn(unit, vrf, &vpn_id);
    if(BCM_FAILURE(rv)) {
        printf("\nError in configuring : %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* IP route configuration */

    /* Create L3 interface */
    rv = create_l3_interface(unit, 0, customer_if_mac, customer_vlan, &egr_l3_if);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring l3 interface : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create L3 egress object */
    rv = create_egr_obj(unit, egr_l3_if, dst_mac, customer_gport, egr_if);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring l3 egress object : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Adding default route to defip table */
    subnet = 0x0;
    mask = 0x0;
    rv = add_route(unit, subnet, mask, egr_if);
    if(BCM_FAILURE(rv)) {
        printf("\nError in adding default route: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Adding route to defip table */
    subnet = 0x0a0a0a02;
    mask = 0xffffff00;
    rv = add_route(unit, subnet, mask, egr_if);
    if(BCM_FAILURE(rv)) {
        printf("\nError in adding route: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Create L3 ingress interface */
    rv = create_l3_ingress_intf(unit, vrf, &ing_l3_if);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring l3 ingress interface : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* MPLS termination configuration */

    /* L3 MPLS VPN QoS configuration */
    rv = map_exp_to_phb(unit, exp, int_pri, cng, &exp_map_id);
    if (BCM_FAILURE(rv)) {
        printf("Error in mapping exp to phb: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Mapping internal priority to cosq */
    rv = map_int_pri_to_cosq(unit, int_pri, cosq);
    if(BCM_FAILURE(rv)) {
        printf("\nError in mapping internal priority to cosq: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Add mpls label entry -- tunnel label */
    rv = add_mpls_label_entry(unit, provider_gport, tunnel_label, ing_l3_if, vpn_id, exp_map_id);
    if (BCM_FAILURE(rv)) {
        printf("Error in adding mpls label entry (tunnel): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Add mpls label entry -- vc label */
    rv = add_mpls_label_entry(unit, provider_gport, vc_label, ing_l3_if, vpn_id, exp_map_id);
    if (BCM_FAILURE(rv)) {
        printf("Error in adding mpls label entry (vc): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Adding entry into the L2 station table */
    rv = add_l2_station_entry(unit, provider_if_mac, provider_if_mac_mask, provider_vlan, station_id);
    if(BCM_FAILURE(rv)) {
        printf("\nError in adding l2 station entry\n",bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

bcm_error_t
config_l3_mpls_vpn_pop_qos(int unit)
{
    return l3_mpls_vpn_qos(unit, egress_port, ingress_port, 10, 20);
}

/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in config_l3_mpls_vpn_pop_qos())
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

    print "~~~ #2) config_l3_mpls_vpn_pop_qos(): ** start **";
    if (BCM_FAILURE((rv = config_l3_mpls_vpn_pop_qos(unit)))) {
        printf("config_l3_mpls_vpn_pop_qos() failed.\n");
        return -1;
    }
    print "~~~ #2) config_l3_mpls_vpn_pop_qos(): ** end **";

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

