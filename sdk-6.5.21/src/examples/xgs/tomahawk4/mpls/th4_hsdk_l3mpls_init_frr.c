/*  Feature  : L3 MPLS encap
 *
 *  Usage    : BCM.0> cint th4_hsdk_l3mpls_init_frr.c
 *
 *  config   : bcm56990-generic-l3mpls.config.yml
 *
 *  Log file : th4_hsdk_l3mpls_init_frr_log.txt
 *
 *  Test Topology :
 *                   +------------------------------+
 *                   |                              |
 *                   |                              | egress_primary_port
 *                   |                              +--------------------+
 *  ingress_port     |                              |
 * +-----------------+          SWITCH              |
 *                   |                              |
 *                   |                              | egress_failover_port
 *                   |                              +--------------------+
 *                   |                              |
 *                   |                              |
 *                   +------------------------------+
 *
 *  This script can verify below L3 APIs:
 *  ========
 *   bcm_l3_route_add()
 *   bcm_mpls_tunnel_initiator_set()
 *   bcm_failover_egress_set()
 *   bcm_failover_egress_status_set()
 *
 *  Summary:
 *  ========
 *  This cint example demonstrate L3 MPLS encap traffic configuration.
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Create vlan and add port to vlan. Enable L3 feature per port.
 *
 *    2) Step2 - Configuration (Done in config_l3_mpls_init_frr())
 *    ======================================================
 *      a) Create egress objects used for unicast route destionation.
 *      b) Create a route.
 *
 *    3) Step3 - Verification (Done in test_verify())
 *    ==========================================
 *      a) Verify the result after send a packet.
 *      b) Expected Result:
 *         Packet raw data (86):
 *         {00000000fe02} {000000000002} 8100 000c
 *         8847 0123 42ff 0567 81fe 4500 003c 6762
 *         0000 fe01 be76 0a3a 4001 0a3a 4273 0000
 *         550a 0001 0051 6162 6364 6566 6768 696a
 *         6b6c 6d6e 6f70 7172 7374 7576 7761 6263
 *         6465 6667 6869
 *      c) Switch over from primary path to failover patch, resend a packet.
 *      d) Expected Result:
 *         Packet raw data (86):
 *         {000000bbfe02} {00000000bb02} 8100 0016
 *         8847 0555 52ff 0666 61fe 4500 003c 6762
 *         0000 fe01 be76 0a3a 4001 0a3a 4273 0000
 *         550a 0001 0051 6162 6364 6566 6768 696a
 *         6b6c 6d6e 6f70 7172 7374 7576 7761 6263
 *         6465 6667 6869
 *
 *    4) Step4 - The l3 mpls init teardown configuration(Done in test_cleanup())
 *    ==========================================
 *      a) Mostly reverse the configuration, done in clear_l3_mpls_init_frr()
 */
cint_reset();

int unit = 0;
int rv;
bcm_port_t ingress_port, egress_ports[2];
bcm_port_t egress_primary_port, egress_failover_port;
bcm_vlan_t in_vlan = 11, out_vlan = 12, out_vlan_1 = 22;
bcm_vrf_t vrf = 1;
int my_station_id;
bcm_mac_t router_mac_in = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
bcm_mac_t router_mac_out = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
bcm_mac_t dst_mac = {0x00, 0x00, 0x00, 0x00, 0xfe, 0x02};
bcm_ip_t dip = 0x0a3a4273;
bcm_ip_t mask = 0xffffff00;
uint32_t stat_counter_id, stat_counter_id_1;
bcm_field_group_t ifp_group;
bcm_field_entry_t ifp_entry[2];
bcm_if_t primary_egr_obj;
int fixed_nh_offset = 0x3000;

/*
 * This function is written so that hardcoding of port
 * numbers in Cint scripts is removed. This function gives
 * required number of ports
 */
bcm_error_t
portNumbersGet(int unit, int *port_list, int num_ports)
{
	bcm_error_t rv = BCM_E_NONE;
    int i = 0, port = 0;
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
        return BCM_E_PORT;
    }

    return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode
 */
bcm_error_t
ingress_port_setup(int unit, bcm_port_t port)
{
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_NONE));

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
egress_ports_setup(int unit, int port_count, int *port)
{
	int i=0;
    bcm_field_entry_t entry;
    bcm_field_group_config_t group_config;

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDrop);
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));
	ifp_group = group_config.group;
	print ifp_group;

    for(i=0;i<port_count; i++){
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));
	ifp_entry[i] = entry;
	print ifp_entry[i];
    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, *port, BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));
    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, *port, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, *port, BCM_PORT_DISCARD_ALL));
	port++;
	}

    return BCM_E_NONE;
}

bcm_error_t clear_field_resource(int unit, bcm_field_group_t group, bcm_field_entry_t *entry, int entry_num)
{
	bcm_error_t rv = BCM_E_NONE;
	int i;

	for(i=0; i<entry_num; i++){
    /* Remove field entry from hardware table */
    rv = bcm_field_entry_remove(unit, entry[i]);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_remove() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Destroy the field entry */
    rv = bcm_field_entry_destroy(unit, entry[i]);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_destroy() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }
	}

    /* Destroy the field group */
    rv = bcm_field_group_destroy(unit, group);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_group_destroy() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/*
 * Test Setup:
 * This functions gets the port numbers and sets up ingress and
 * egress ports. Check ingress_port_setup() and egress_ports_setup().
 */
bcm_error_t test_setup(int unit)
{
	bcm_error_t rv = BCM_E_NONE;
    int port_list[3];

    rv = portNumbersGet(unit, port_list, 3);
    if (BCM_FAILURE(rv)) {
        printf("portNumbersGet() failed\n");
        return rv;
    }

    ingress_port = port_list[0];
	print ingress_port;
    egress_ports[0] = egress_primary_port = port_list[1];
    egress_ports[1] = egress_failover_port = port_list[2];
    print egress_primary_port;
	print egress_failover_port;

    printf("setting ingress port:%d MAC loopback\n", ingress_port);
    rv = ingress_port_setup(unit, ingress_port);
    if (BCM_FAILURE(rv)) {
        printf("ingress_port_setup() failed for port %d\n", ingress_port);
        return rv;
    }

    printf("setting egress port:%d %d MAC loopback\n", egress_ports[0], egress_ports[1]);
    rv = egress_ports_setup(unit, 2, egress_ports);
    if (BCM_FAILURE(rv)) {
        printf("egress_ports_setup() failed for ports %d %d\n", egress_port[0], egress_port[1]);
        return rv;
    }

    /* Enable IPv4 on ingress port */
    print bcm_port_control_set(unit, ingress_port, bcmPortControlIP4, TRUE);

    print bcm_switch_control_set(unit, bcmSwitchMplsPortIndependentLowerRange2, 0x600);
    print bcm_switch_control_set(unit, bcmSwitchMplsPortIndependentLowerRange1, 0x600);

    /*bshell(unit, "pw start report +raw +decode");*/

	return BCM_E_NONE;
}

/*Expected Egress packets */
unsigned char expected_egress_packet1[86] = {
0x00, 0x00, 0x00, 0x00, 0xfe, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x81, 0x00, 0x00, 0x0c,
0x88, 0x47, 0x01, 0x23, 0x42, 0xff, 0x05, 0x67, 0x81, 0xfe, 0x45, 0x00, 0x00, 0x3c, 0x67, 0x62,
0x00, 0x00, 0xfe, 0x01, 0xbe, 0x76, 0x0a, 0x3a, 0x40, 0x01, 0x0a, 0x3a, 0x42, 0x73, 0x00, 0x00,
0x55, 0x0a, 0x00, 0x01, 0x00, 0x51, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a,
0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x61, 0x62, 0x63,
0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
};

unsigned char expected_egress_packet2[86] = {
0x00, 0x00, 0x00, 0xbb, 0xfe, 0x02, 0x00, 0x00, 0x00, 0x00, 0xbb, 0x02, 0x81, 0x00, 0x00, 0x16,
0x88, 0x47, 0x05, 0x55, 0x52, 0xff, 0x06, 0x66, 0x61, 0xfe, 0x45, 0x00, 0x00, 0x3c, 0x67, 0x62,
0x00, 0x00, 0xfe, 0x01, 0xbe, 0x76, 0x0a, 0x3a, 0x40, 0x01, 0x0a, 0x3a, 0x42, 0x73, 0x00, 0x00,
0x55, 0x0a, 0x00, 0x01, 0x00, 0x51, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a,
0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x61, 0x62, 0x63,
0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
};

int test_failed = 0; /*Final result will be stored here */
int rx_count;   /* Global received packet count */
uint32 test_pkt_num = 2;
unsigned char *expected_packets[test_pkt_num];
const uint8 priority = 100;

/* Rx callback function for applications using the HSDK PKTIO */
bcm_pktio_rx_t
pktioRxCallback(int unit, bcm_pktio_pkt_t * packet, void *cookie)
{
    int                *count = (auto) cookie;
    void               *buffer;
    uint32              length;
    uint32              port;

	(*count)++; /* Bump received packet count */

    /* Get basic packet info */
    if (BCM_FAILURE(bcm_pktio_pkt_data_get(unit, packet, (void *) &buffer, &length))) {
        return BCM_PKTIO_RX_NOT_HANDLED;
    }
    /* Get source port metadata */
    BCM_IF_ERROR_RETURN(bcm_pktio_pmd_field_get
                        (unit, packet, bcmPktioPmdTypeRx,
                         BCM_PKTIO_RXPMD_SRC_PORT_NUM, &port));

    printf("pktioRxCallback: packet received #%d from port %d, length %d\n", *count, port, length);

	if ((egress_ports[0] == port) && (sal_memcmp(buffer, expected_packets[0], length) != 0)){
   	    test_failed = 1;
        printf("pktioRxCallback: packet received from wrong port %d, expected from port %d, or packet data not match\n", port, egress_ports[0]);
    }else if ((egress_ports[1] == port) && (sal_memcmp(buffer, expected_packets[1], length) != 0)){
   	    test_failed = 1;
        printf("pktioRxCallback: packet received from wrong port %d, expected from port %d, or packet data not match\n", port, egress_ports[1]);
	} else {
        printf("pktioRxCallback: packet received from correct expected port %d\n", port);
        printf("pktioRxCallback: packet received data comparision pass\n");
	}

    return BCM_PKTIO_RX_NOT_HANDLED;
}

/* Register callback function for received packets */
bcm_error_t
registerPktioRxCallback(int unit)
{
    const uint32        flags = 0;      /* Flags only used on match (for now) */

    return bcm_pktio_rx_register
      (unit, "rx_cb", pktioRxCallback, priority, &rx_count, flags);
}

bcm_error_t
unregisterPktioRxCallback(int unit)
{
    return bcm_pktio_rx_unregister(unit, pktioRxCallback, priority);
}

/* Create vlan and add port to vlan */
bcm_error_t
vlan_create_add_port(int unit, int vid, int port)
{
    bcm_pbmp_t pbmp, upbmp;

    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    return bcm_vlan_port_add(unit, vid, pbmp, upbmp);
}

/* Destroy vlan and remove port from vlan */
bcm_error_t
vlan_destroy_remove_port(int unit, int vid, int port)
{
    bcm_pbmp_t pbmp;

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    BCM_IF_ERROR_RETURN(bcm_vlan_port_remove(unit, vid, pbmp));
    return bcm_vlan_destroy(unit, vid);
}

/* Create vlan and add array of ports to vlan */
bcm_error_t
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

bcm_error_t
create_l3_ingress_interface(int unit, bcm_vrf_t vrf, bcm_if_t *ingress_if)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_l3_ingress_t l3_ingress;

    bcm_l3_ingress_t_init(&l3_ingress);
    if(*ingress_if)
        l3_ingress.flags = BCM_L3_INGRESS_WITH_ID;
    l3_ingress.vrf = vrf;
    rv = bcm_l3_ingress_create(unit, &l3_ingress, ingress_if);

    return rv;
}

bcm_error_t
clear_l3_ingress_interface(int unit, bcm_if_t intf_id)
{
    bcm_error_t rv = BCM_E_NONE;

	rv = bcm_l3_ingress_destroy(unit, intf_id);
    return rv;
}

bcm_error_t
create_l3_interface(int unit, int flags, bcm_mac_t local_mac, int vid, bcm_if_t *intf_id)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_l3_intf_t l3_intf;

    /* L3 interface and egress object for access */
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags |= flags;
    sal_memcpy(l3_intf.l3a_mac_addr, local_mac, sizeof(local_mac));
    l3_intf.l3a_vid = vid;
    l3_intf.l3a_intf_id = *intf_id;
    rv = bcm_l3_intf_create(unit, &l3_intf);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l3_intf_create()%s\n", bcm_errmsg(rv));
        return rv;
    }
    *intf_id = l3_intf.l3a_intf_id;

    return rv;
}

bcm_error_t
clear_l3_interface(int unit, bcm_if_t intf_id)
{
    bcm_l3_intf_t l3_intf;
    bcm_error_t rv = BCM_E_NONE;

    bcm_l3_intf_t_init(&l3_intf);
	l3_intf.l3a_intf_id = intf_id;
	rv = bcm_l3_intf_get(unit, &l3_intf);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l3_intf_get %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_l3_intf_delete(unit, &l3_intf);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l3_intf_delete %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/* Create L3 egress object */
bcm_error_t
create_egr_obj_with_route_label(int unit, int l3_if, bcm_mac_t nh_mac, bcm_port_t port,
               bcm_mpls_label_t route_label, bcm_if_t *egr_obj_id)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_l3_egress_t l3_egress;

    bcm_l3_egress_t_init(&l3_egress);
    sal_memcpy(l3_egress.mac_addr,  nh_mac, sizeof(nh_mac));
    l3_egress.intf  = l3_if;
    l3_egress.port = port;
    l3_egress.flags = BCM_L3_ROUTE_LABEL;
    l3_egress.mpls_label = route_label;
    rv = bcm_l3_egress_create(unit, 0, &l3_egress, egr_obj_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l3_egress_create() %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/* Clear L3 egress object */
bcm_error_t
clear_egr_obj(int unit, bcm_if_t egr_obj_id)
{
    return bcm_l3_egress_destroy(unit, egr_obj_id);
}

/* Config my_station_tcam to enable mpls and l3 lookup */
bcm_error_t enable_mpls_l3_lookup(int unit, bcm_mac_t router_mac_in, bcm_vlan_t in_vlan, int *station_id)
{
	bcm_error_t rv = BCM_E_NONE;
    bcm_l2_station_t l2_station;

    bcm_l2_station_t_init(&l2_station);
	l2_station.vlan = in_vlan;
	l2_station.vlan_mask = BCM_VLAN_MAX;
    sal_memcpy(l2_station.dst_mac, router_mac_in, sizeof(router_mac_in));
    l2_station.dst_mac_mask = "ff:ff:ff:ff:ff:ff";
    l2_station.flags = BCM_L2_STATION_IPV4 | BCM_L2_STATION_MPLS;
    rv = bcm_l2_station_add(unit, station_id, &l2_station);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l2_station_add() %s\n", bcm_errmsg(rv));
        return rv;
    }

	return rv;
}

/* Config my_station_tcam to enable l3 lookup */
bcm_error_t enable_l3_lookup(int unit, bcm_mac_t router_mac_in, bcm_vlan_t in_vlan, int *station_id)
{
	bcm_error_t rv = BCM_E_NONE;
    bcm_l2_station_t l2_station;

    bcm_l2_station_t_init(&l2_station);
	l2_station.vlan = in_vlan;
	l2_station.vlan_mask = BCM_VLAN_MAX;
    sal_memcpy(l2_station.dst_mac, router_mac_in, sizeof(router_mac_in));
    l2_station.dst_mac_mask = "ff:ff:ff:ff:ff:ff";
    l2_station.flags = BCM_L2_STATION_IPV4 ;
    rv = bcm_l2_station_add(unit, station_id, &l2_station);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l2_station_add() %s\n", bcm_errmsg(rv));
        return rv;
    }

	return rv;
}

/* Clear my_station_tcam entry to disable l3 looup*/
bcm_error_t disable_l3_lookup(int unit, int station_id)
{
	bcm_error_t rv = BCM_E_NONE;

    rv = bcm_l2_station_delete(unit, station_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l2_station_delete() %s\n", bcm_errmsg(rv));
        return rv;
    }

	return rv;
}

bcm_error_t config_l3_egress_flexctr(int unit, bcm_if_t egr_obj, uint32 *stat_counter_id)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_flexctr_action_t action = {0};
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    bcm_flexctr_value_operation_t *value_b_op = NULL;
    int options = 0;

    action.index_num = 1;
    action.source = bcmFlexctrSourceL3Egress;
    action.hint = 0;
    action.mode = bcmFlexctrCounterModeNormal;

    /* Counter index is always zero. */
    index_op = &action.index_operation;
    index_op->object[0] = bcmFlexctrObjectConstZero;
    index_op->mask_size[0] = 16;
    index_op->shift[0] = 0;
    index_op->object[1] = bcmFlexctrObjectConstZero;
    index_op->mask_size[1] = 1;
    index_op->shift[1] = 0;

    /* Increase counter per packet. */
    value_a_op = &action.operation_a;
    value_a_op->select = bcmFlexctrValueSelectCounterValue;
    value_a_op->object[0] = bcmFlexctrObjectConstOne;
    value_a_op->mask_size[0] = 16;
    value_a_op->shift[0] = 0;
    value_a_op->object[1] = bcmFlexctrObjectConstZero;
    value_a_op->mask_size[1] = 1;
    value_a_op->shift[1] = 0;
    value_a_op->type = bcmFlexctrValueOperationTypeInc;

    /* Increase counter per packet bytes. */
    value_b_op = &action.operation_b;
    value_b_op->select = bcmFlexctrValueSelectPacketLength;
    value_b_op->type = bcmFlexctrValueOperationTypeInc;

    rv = bcm_flexctr_action_create(unit, options, &action, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_action_create %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf("Created flexctr stat_counter_id = 0x%x.\n", *stat_counter_id);

    rv = bcm_l3_egress_stat_attach(unit, egr_obj, *stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("ERROR: bcm_l3_egress_stat_attach() -  %s.\n", bcm_errmsg(rv));
        return rv;
    }
    printf("Flexctr stat_counter_id:0x%x attached to l3 egress obj %d\n", *stat_counter_id, egr_obj);

    return rv;
}

bcm_error_t
check_flexctr(int unit, uint32 stat_counter_id, int *flexctr_check_failed)
{
    bcm_error_t rv = BCM_E_NONE;
    uint32 num_entries = 1;
    uint32 counter_index = 0;
    bcm_flexctr_counter_value_t counter_value;
    int expected_packet_length = 90;

    /* Get counter value. */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    rv = bcm_flexctr_stat_get(unit,
                              stat_counter_id,
                              num_entries,
                              &counter_index,
                              &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_get %s\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Flexctr Get : %d packets / %d bytes\n",
           COMPILER_64_LO(counter_value.value[0]),
           COMPILER_64_LO(counter_value.value[1]));

    if (COMPILER_64_LO(counter_value.value[0]) != 1) {
        printf("Flexctr packets verify failed. Expected packet %d but get %d\n", rx_count, COMPILER_64_LO(counter_value.value[0]));
        *flexctr_check_failed = 1;
        return BCM_E_FAIL;
    };

    if (COMPILER_64_LO(counter_value.value[1]) != expected_packet_length) {
        printf("Flexctr bytes verify failed. Expected bytes %d but get %d\n", expected_packet_length, COMPILER_64_LO(counter_value.value[1]));
        *flexctr_check_failed = 1;
        return BCM_E_FAIL;
    };

    return BCM_E_NONE;
}

bcm_error_t
clear_l3_egress_flexctr(int unit,  bcm_if_t intf_id, uint32 stat_counter_id)
{
    bcm_error_t rv = BCM_E_NONE;

    /* Detach flexctr from l3 egress */
    rv = bcm_l3_egress_stat_detach(unit, intf_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l3_egress_stat_detach: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Destroy flexctr action */
    rv = bcm_flexctr_action_destroy(unit, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_action_destroy %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

bcm_error_t
config_l3_mpls_init_frr(int unit)
{
	bcm_error_t rv = BCM_E_NONE;
    bcm_if_t ingress_if = 0x1b2;
	int flags = 0;
    bcm_if_t l3_intf_id;
    bcm_if_t egr_obj_id;
    int num_labels = 1;
    bcm_mpls_egress_label_t label_array[2];
    bcm_mpls_egress_label_t failover_label_array[2];
	bcm_mpls_label_t tunnel_label_init = 0x1234;
	bcm_mpls_label_t vc_label_init = 0x5678;
	uint8 exp = 1;
	uint8 ttl = 255;

    /*****************************
     * Create failover path
 */
    /*
     * The failover path is used for 1:1 protection of the primary path.
     * The packet going out of the failover path will have two different labels
     * by the same mechanism as the primary path
     * bcm_l3_egress_create() and bcm_mpls_tunnel_initiator_set()
     */
    bcm_if_t failover_tunnel_intf_id = 0x1b3;
    bcm_mac_t failover_mac = {0x00,0x00,0x00,0x00,0xbb,0x02};
    bcm_mac_t failover_mac_nh = {0x00,0x00,0x00,0xbb,0xfe,0x02};
    bcm_vlan_t failover_vid = out_vlan_1;
    bcm_port_t failover_port = egress_failover_port;
    bcm_l3_egress_t failover_l3_egress;
    uint32 failover_tunnel_label = 0x5555;
    bcm_mpls_label_t failover_vc_label = 0x6666;

    /* Egress side VLAN */
    rv = vlan_create_add_port(unit, failover_vid, failover_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_create_add_port(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create L3 Interface for failover LSP */
    rv = create_l3_interface(unit, flags, failover_mac, failover_vid, &failover_tunnel_intf_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in create_l3_interface: %s.\n",bcm_errmsg(rv));
        return rv;
    }
    printf("failover_tunnel_intf_id = %d\n", failover_tunnel_intf_id);

    /* Set MPLS tunnel initiator failover */
    bcm_mpls_egress_label_t_init(failover_label_array);
    failover_label_array[0].flags = BCM_MPLS_EGRESS_LABEL_TTL_SET | BCM_MPLS_EGRESS_LABEL_EXP_SET;
    failover_label_array[0].label = failover_tunnel_label;
    failover_label_array[0].exp = exp;
    failover_label_array[0].ttl = ttl;
    print bcm_mpls_tunnel_initiator_set(unit, failover_tunnel_intf_id, 1, failover_label_array);

    /* Create Egr Object for failover LSP */
    bcm_l3_egress_t_init(&failover_l3_egress);
    sal_memcpy(failover_l3_egress.mac_addr, failover_mac_nh, 6);
    failover_l3_egress.port       = failover_port;
    failover_l3_egress.intf       = failover_tunnel_intf_id;
    failover_l3_egress.mpls_label = failover_vc_label;

    /************************************
     * Create primary path
 */
    /* Create and add ingress port to ingress VLAN */
    rv = vlan_create_add_port(unit, in_vlan, ingress_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_create_add_port(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create and add egress port to egress VLAN */
    rv = vlan_create_add_port(unit, out_vlan, egress_ports[0]);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_create_add_port(): %s.\n",bcm_errmsg(rv));
        return rv;
    }

	/* Enable L3 Lookup for incoming l3 packet */
    rv = enable_l3_lookup(unit, router_mac_in, in_vlan, &my_station_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in enable_l3_lookup(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create L3 ingress interface */
    rv = create_l3_ingress_interface(unit, vrf, &ingress_if);
    if (BCM_FAILURE(rv)) {
        printf("\nError in create_l3_ingress_interface(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
	print ingress_if;

    /* Config vlan_id to l3_iif mapping */
    bcm_vlan_control_vlan_t vlan_ctrl;
    bcm_vlan_control_vlan_t_init(&vlan_ctrl);
    bcm_vlan_control_vlan_get(unit, in_vlan, &vlan_ctrl);
    vlan_ctrl.ingress_if = ingress_if;
    print bcm_vlan_control_vlan_set(unit, in_vlan, vlan_ctrl);

    /* Create egress L3 interface */
    rv = create_l3_interface(unit, flags, router_mac_out, out_vlan, &l3_intf_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in create_l3_interface: %s.\n",bcm_errmsg(rv));
        return rv;
    }
    printf("l3 egr intf = %d\n", l3_intf_id);

    /* Create L3 egress object */
    rv = create_egr_obj_with_route_label(unit, l3_intf_id, dst_mac, egress_ports[0], vc_label_init, &egr_obj_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in create_egr_obj_with_route_label(): %s.\n", bcm_errmsg(rv));
        return rv;
    }
    printf("l3 egr obj = %d\n", egr_obj_id);
	primary_egr_obj = egr_obj_id;
    BCM_IF_ERROR_RETURN(bcm_failover_egress_set(unit, primary_egr_obj, &failover_l3_egress));

    bcm_l3_route_t route_info;
    /* Install the route for DIP */
    bcm_l3_route_t_init(&route_info);
    route_info.l3a_intf = egr_obj_id;
    route_info.l3a_subnet = dip;
    route_info.l3a_ip_mask = mask;
    route_info.l3a_vrf = vrf;
    rv = bcm_l3_route_add(unit, &route_info);
    if (BCM_FAILURE(rv)) {
        printf("Error executing bcm_l3_route_add(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    bcm_mpls_egress_label_t_init(label_array);
	label_array[0].flags = BCM_MPLS_EGRESS_LABEL_TTL_SET | BCM_MPLS_EGRESS_LABEL_EXP_SET;
    label_array[0].label = tunnel_label_init;
	label_array[0].exp   = exp;
    label_array[0].ttl   = ttl;
    rv = bcm_mpls_tunnel_initiator_set(unit, l3_intf_id, num_labels, label_array);
    if (BCM_FAILURE(rv)) {
        printf("ERROR: bcm_mpls_tunnel_initiator_set() : %s.\n", bcm_errmsg(rv));
        return rv;
    }
	print tunnel_label_init;
	print exp;
	print ttl;


	return rv;
}

bcm_error_t clear_l3_mpls_init_frr(int unit)
{
	bcm_error_t rv = BCM_E_NONE;

    bcm_l3_route_t route_info;
	bcm_l3_egress_t egr, failover_egr;
    bcm_vlan_control_vlan_t vlan_ctrl;

    bcm_l3_route_t_init(&route_info);
    route_info.l3a_subnet = dip;
    route_info.l3a_ip_mask = mask;
    route_info.l3a_vrf = vrf;
    rv = bcm_l3_route_get(unit, &route_info);
    if (BCM_FAILURE(rv)) {
       printf("Error in bcm_l3_route_get(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

	rv = bcm_l3_egress_get(unit, route_info.l3a_intf, &egr);
    if (BCM_FAILURE(rv)) {
       printf("Error in bcm_l3_egress_get(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

    rv = bcm_mpls_tunnel_initiator_clear(unit, egr.intf);
    if (BCM_FAILURE(rv)) {
       printf("Error in bcm_mpls_tunnel_initiator_clear(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

	rv = bcm_l3_egress_get(unit, route_info.l3a_intf+fixed_nh_offset, &failover_egr);
    if (BCM_FAILURE(rv)) {
       printf("Error in bcm_l3_egress_get(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

    rv = bcm_mpls_tunnel_initiator_clear(unit, failover_egr.intf);
    if (BCM_FAILURE(rv)) {
       printf("Error in bcm_mpls_tunnel_initiator_clear(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

    /* Config vlan_id to l3_iif mapping */
    bcm_vlan_control_vlan_t_init(&vlan_ctrl);
    bcm_vlan_control_vlan_get(unit, in_vlan, &vlan_ctrl);
	rv = clear_l3_ingress_interface(unit, vlan_ctrl.ingress_if);
    if (BCM_FAILURE(rv)) {
       printf("Error in clear_l3_ingress_interface(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

    vlan_ctrl.ingress_if = 0;
    rv = bcm_vlan_control_vlan_set(unit, in_vlan, vlan_ctrl);
    if (BCM_FAILURE(rv)) {
       printf("Error in bcm_vlan_control_vlan_set(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

	rv = clear_l3_interface(unit, egr.intf);
    if (BCM_FAILURE(rv)) {
       printf("Error in clear_l3_interface(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

	rv = clear_l3_interface(unit, failover_egr.intf);
    if (BCM_FAILURE(rv)) {
       printf("Error in clear_l3_interface(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

    rv = bcm_failover_egress_clear(unit, primary_egr_obj);
    if (BCM_FAILURE(rv)) {
       printf("Error in bcm_failover_egress_clear(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

	rv = clear_egr_obj(unit, route_info.l3a_intf);
    if (BCM_FAILURE(rv)) {
       printf("Error in clear_egr_obj(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

	rv = bcm_l3_route_delete(unit, &route_info);
    if (BCM_FAILURE(rv)) {
       printf("Error in bcm_l3_route_delete(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

    rv = disable_l3_lookup(unit, my_station_id);
    if (BCM_FAILURE(rv)) {
       printf("Error in bcm_l3_route_delete(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

	BCM_IF_ERROR_RETURN(vlan_destroy_remove_port(unit, out_vlan, egress_ports[0]));
	BCM_IF_ERROR_RETURN(vlan_destroy_remove_port(unit, out_vlan_1, egress_ports[1]));
	BCM_IF_ERROR_RETURN(vlan_destroy_remove_port(unit, in_vlan, ingress_port));

    return rv;
}

void test_verify(int unit)
{
	bcm_error_t rv = BCM_E_NONE;
    char   str[512];
    expected_packets[0] = expected_egress_packet1;
    expected_packets[1] = expected_egress_packet2;

    int label_count = 0;
    bcm_mpls_egress_label_t label_array[2];

    rv = registerPktioRxCallback(unit);
    if (BCM_FAILURE(rv)) {
        printf("registerPktioRxCallback() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

	/* create flex ctr and attach to l3 egress nexthop */
	rv = config_l3_egress_flexctr(unit, primary_egr_obj, &stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("ERROR: config_l3_egress_flexctr() : %s.\n", bcm_errmsg(rv));
    }

    printf("Transmiting l3 packet into ingress_port:%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x000000000001002a107777008100000B08004500003c67620000ff01bd760a3a40010a3a42730000550a000100516162636465666768696a6b6c6d6e6f7071727374757677616263646566676869; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "sleep quiet 1");

    rv = check_flexctr(unit, stat_counter_id, &test_failed);
    if (BCM_FAILURE(rv)) {
        printf("check_flexctr() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Remove the flex counter from l3 egress object */
    rv = clear_l3_egress_flexctr(unit, primary_egr_obj, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("clear_l3_egress_flexctr() failed.\n");
        return rv;
    }

    /* Switch From Primary to Protection */
    print "Switch-over from primary-path to failover/protection-path";
    BCM_IF_ERROR_RETURN(bcm_failover_egress_status_set(unit, primary_egr_obj, TRUE));

	rv = config_l3_egress_flexctr(unit, primary_egr_obj+fixed_nh_offset, &stat_counter_id_1);
    if (BCM_FAILURE(rv)) {
        printf("ERROR: config_l3_egress_flexctr() : %s.\n", bcm_errmsg(rv));
    }

    printf("ReTransmiting l3 packet into ingress_port:%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x000000000001002a107777008100000B08004500003c67620000ff01bd760a3a40010a3a42730000550a000100516162636465666768696a6b6c6d6e6f7071727374757677616263646566676869; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "sleep quiet 1");

    rv = check_flexctr(unit, stat_counter_id_1, &test_failed);
    if (BCM_FAILURE(rv)) {
        printf("check_flexctr() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Remove the flex counter from l3 egress object */
    rv = clear_l3_egress_flexctr(unit, primary_egr_obj+fixed_nh_offset, stat_counter_id_1);
    if (BCM_FAILURE(rv)) {
        printf("clear_l3_egress_flexctr() failed.\n");
        return rv;
    }
    printf("\n------------------------------------------------------------- \n");
    printf("    RESULT OF L3MPLS TUNNEL INITIATION FAST REROUTE VERIFICATION  ");
    printf("\n------------------------------------------------------------- \n");
    printf("Test = [%s]", !test_failed? "PASS":"FAIL");
    printf("\n------------------------------------- \n");

    if (!test_failed)
        return BCM_E_NONE;
    else
        return BCM_E_FAIL;
}

/*
 * Clean up pre-test setup.
 */
int test_cleanup(int unit)
{
    bcm_error_t rv;

	/* unregister Pktio callback */
    unregisterPktioRxCallback(unit);

    /* Remove the l3 mpls tunnel initiation related configuration */
	rv = clear_l3_mpls_init_frr(unit);
    if (BCM_FAILURE(rv)) {
        printf("clear_l3_mpls_init_frr() failed.\n");
        return rv;
	}

    /* Remove the field configiuration */
	BCM_IF_ERROR_RETURN(clear_field_resource(unit, ifp_group, ifp_entry, 2));

    /* Remove the port loopback configiuration */
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ingress_port, BCM_PORT_LOOPBACK_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, ingress_port, BCM_PORT_DISCARD_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_ports[0], BCM_PORT_LOOPBACK_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_ports[1], BCM_PORT_LOOPBACK_NONE));

    return BCM_E_NONE;
}


/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in config_l3_mpls_init_frr())
 *  c) demonstrates the functionality(done in test_verify()).
 *  d) demonstrates the l3 mpls initiation teardown configure sequence(Done in test_cleanup())
 */
bcm_error_t
execute(void)
{
    int unit = 0;
    bcm_error_t rv;

    print "~~~ #1) test_setup(): ** start **";
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return rv;
    }
    print "~~~ #1) test_setup(): ** end **";

    print "~~~ #2) config_l3_mpls_init_frr(): ** start **";
    if (BCM_FAILURE((rv = config_l3_mpls_init_frr(unit)))) {
        printf("config_l3_mpls_init_frr() failed.\n");
        return rv;
    }
    print "~~~ #2) config_l3_mpls_init_frr(): ** end **";

    print "~~~ #3) test_verify(): ** start **";
    test_verify(unit);
    print "~~~ #3) test_verify(): ** end **";

    print "~~~ #4) test_cleanup(): ** start **";
    if (BCM_FAILURE(rv = test_cleanup(unit))) {
         printf("test_cleanup() failed.\n");
         return rv;
    }
    print "~~~ #4) test_cleanup(): ** end **";

    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print "execute(): Start";
  print execute();
  print "execute(): End";
}

