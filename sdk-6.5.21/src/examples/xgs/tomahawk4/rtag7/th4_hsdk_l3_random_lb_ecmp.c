/*  Feature  : RTAG7 of L3 IPv4 Route to ECMP group with random load balance
 *
 *  Usage    : BCM.0> cint th4_hsdk_l3_random_lb_ecmp.c
 *
 *  config   : bcm56990-generic-l3.config.yml
 *
 *  Log file : th4_hsdk_l3_random_lb_ecmp_log.txt
 *
 *  Test Topology :
 *                  +------------------------------+
 *                  |                              |
 *                  |                              | ecmp object 0
 *                  |                              +---------------------+
 *     ingress_port |                              |
 * +----------------+          SWITCH              |
 *                  |                              |
 *                  |                              | ecmp object 1
 *                  |                              +---------------------+
 *                  |                              |
 *                  |                              |
 *                  +------------------------------+
 *
 *  This script can verify below L3 APIs:
 *  ========
 *   bcm_l3_route_add()
 *   bcm_l3_ecmp_create()
 *
 *
 *  Summary:
 *  ========
 *  This cint example demonstrate L3 route to ecmp group
 *  with random load balance configuration.
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Create vlan and add port to vlan. Enable L3 feature per port.
 *
 *    2) Step2 - Configuration (Done in config_l3_random_lb_ecmp())
 *    ======================================================
 *      a) Create egress objects used for unicast route destionation.
 *      b) Create an ecmp grpup.
 *      b) Create a l3 route to that ecmp group.
 *
 *    3) Step3 - Verification (Done in test_verify())
 *    ==========================================
 *      a) Verify the result after send 100 packets.
 *         ingress_port:
 *         {000000000001} {002a10777700} 8100 000b
 *         0800 4500 003c 6762 0000 ff01 bd76 0a3a
 *         4001 0a3a 4273 0000 550a 0001 0051 6162
 *         6364 6566 6768 696a 6b6c 6d6e 6f70 7172
 *         7374 7576 7761 6263 6465 6667 6869
 *
 *      b) Expected Result:
 *         egress_port[0]:
 *         Packet raw data (78):
 *         {00000000fe02} {000000000002} 8100 000c
 *         0800 4500 003c 6762 0000 fe01 be76 0a3a
 *         4001 0a3a 4273 0000 550a 0001 0051 6162
 *         6364 6566 6768 696a 6b6c 6d6e 6f70 7172
 *         7374 7576 7761 6263 6465 6667 6869
 *
 *         egress_port[1]:
 *         Packet raw data (78):
 *         {00000002fe02} {000000000202} 8100 0016
 *         0800 4500 003c 6762 0000 fe01 be76 0a3a
 *         4001 0a3a 4273 0000 550a 0001 0051 6162
 *         6364 6566 6768 696a 6b6c 6d6e 6f70 7172
 *         7374 7576 7761 6263 6465 6667 6869
 *
 *    4) Step4 - The l3 route teardown configuration(Done in test_cleanup())
 *    ==========================================
 *      a) Mostly reverse the configuration, done in clear_l3_random_lb_ecmp()
 */
cint_reset();

int unit = 0;
bcm_error_t rv = BCM_E_NONE;
bcm_port_t ingress_port;
bcm_port_t egress_port[2];
bcm_vlan_t in_vlan = 11, out_vlan = 12, out_vlan2 = 22;
bcm_vrf_t vrf = 1;
int egr_obj_num = 2;
bcm_if_t egr_obj_array[2];
bcm_if_t ecmp_group_id;
int my_station_id;
bcm_mac_t router_mac_in = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
bcm_mac_t router_mac_out = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
bcm_mac_t router_mac_out2 = {0x00, 0x00, 0x00, 0x00, 0x02, 0x02};
bcm_mac_t dst_mac = {0x00, 0x00, 0x00, 0x00, 0xfe, 0x02};
bcm_mac_t dst_mac2 = {0x00, 0x00, 0x00, 0x02, 0xfe, 0x02};
bcm_ip_t dip = 0x0a3a4273;
bcm_ip_t mask = 0xffffff00;
bcm_field_group_t ifp_group;
bcm_field_entry_t ifp_entry[2];
uint32_t stat_counter_id;
int test_pkts_num = 100;


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
egress_port_setup(int unit, int port_count, bcm_port_t *port)
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
 * egress ports. Check ingress_port_setup() and egress_port_setup().
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
    egress_port[0] = port_list[1];
    print egress_port[0];
    egress_port[1] = port_list[2];
    print egress_port[1];

    printf("setting ingress port:%d MAC loopback\n", ingress_port);
    rv = ingress_port_setup(unit, ingress_port);
    if (BCM_FAILURE(rv)) {
        printf("ingress_port_setup() failed for port %d\n", ingress_port);
        return rv;
    }

    printf("setting egress ports:%d, %d MAC loopback\n", egress_port[0], egress_port[1]);
    rv = egress_port_setup(unit, 2, egress_port);
    if (BCM_FAILURE(rv)) {
        printf("egress_port_setup() failed for port %d, %d\n", egress_port[0], egress_port[1]);
        return rv;
    }

    /* Enable IPv4/6 on ingress port */
    print bcm_port_control_set(unit, ingress_port, bcmPortControlIP4, TRUE);


    /*bshell(unit, "pw start report +raw +decode");*/

	return BCM_E_NONE;
}

/*Expected Egress packets */
unsigned char expected_egress_packet1[78] = {
0x00, 0x00, 0x00, 0x00, 0xfe, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x81, 0x00, 0x00, 0x0c,
0x08, 0x00, 0x45, 0x00, 0x00, 0x3c, 0x67, 0x62, 0x00, 0x00, 0xfe, 0x01, 0xbe, 0x76, 0x0a, 0x3a,
0x40, 0x01, 0x0a, 0x3a, 0x42, 0x73, 0x00, 0x00, 0x55, 0x0a, 0x00, 0x01, 0x00, 0x51, 0x61, 0x62,
0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72,
0x73, 0x74, 0x75, 0x76, 0x77, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
};

unsigned char expected_egress_packet2[78] = {
0x00, 0x00, 0x00, 0x02, 0xfe, 0x02, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x81, 0x00, 0x00, 0x16,
0x08, 0x00, 0x45, 0x00, 0x00, 0x3c, 0x67, 0x62, 0x00, 0x00, 0xfe, 0x01, 0xbe, 0x76, 0x0a, 0x3a,
0x40, 0x01, 0x0a, 0x3a, 0x42, 0x73, 0x00, 0x00, 0x55, 0x0a, 0x00, 0x01, 0x00, 0x51, 0x61, 0x62,
0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72,
0x73, 0x74, 0x75, 0x76, 0x77, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
};

int test_failed = 0; /*Final result will be stored here */
int rx_count;   /* Global received packet count */
unsigned char *expected_packets[2];
const uint8 priority = 100;

/* Rx callback function for applications using the HSDK PKTIO */
bcm_pktio_rx_t
pktioRxCallback(int unit, bcm_pktio_pkt_t * packet, void *cookie)
{
    int                 i, *count = (auto) cookie;
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

	if ((egress_port[0] != port) && (egress_port[1] != port)){
   	    test_failed = 1;
        printf("pktioRxCallback: packet received from wrong port %d, expected to received from port %d or %d.\n", port, egress_port[0], egress_port[1]);
	} else if ((egress_port[0] == port)&&(sal_memcmp(buffer, expected_packets[0], length) != 0)){
   	    test_failed = 1;
        printf("pktioRxCallback: packet received from port %d as expected, but packet data not match\n", port);
	} else if ((egress_port[1] == port)&&(sal_memcmp(buffer, expected_packets[1], length) != 0)){
   	    test_failed = 1;
        printf("pktioRxCallback: packet received from port %d as expected, but packet data not match\n", port);
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

/* Destroy vlan and remove ports from vlan */
bcm_error_t
vlan_destroy_remove_ports(int unit, int vid, int count, int *ports)
{
    bcm_pbmp_t pbmp;
    int i;

    BCM_PBMP_CLEAR(pbmp);
    for (i = 0; i < count; i++) {
        BCM_PBMP_PORT_ADD(pbmp, ports[i]);
	}

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
create_egr_obj(int unit, int l3_if, bcm_mac_t nh_mac, bcm_port_t port,
               bcm_if_t *egr_obj_id)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_l3_egress_t l3_egress;

    bcm_l3_egress_t_init(&l3_egress);
    sal_memcpy(l3_egress.mac_addr,  nh_mac, sizeof(nh_mac));
    l3_egress.intf  = l3_if;
    l3_egress.port = port;
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

int config_l3_egress_flexctr(int unit, int egr_obj_num, bcm_if_t *egr_obj_id, uint32 *stat_counter_id)
{
    int rv;
	int i = 0;
    int options = 0;
    bcm_flexctr_counter_value_t counter_value;
    bcm_flexctr_action_t action = {0};
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    bcm_flexctr_value_operation_t *value_b_op = NULL;

    action.source = bcmFlexctrSourceL3Egress;
    action.hint = 0;
    action.mode = bcmFlexctrCounterModeNormal;
    action.index_num = 16;

    index_op = &action.index_operation;
    index_op->object[0] = bcmFlexctrObjectStaticEgrL3NextHop;
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

    /* First action in group. */
    rv = bcm_flexctr_action_create(unit, options, &action, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_action_create %s\n", bcm_errmsg(rv));
        return rv;
    }
	print *stat_counter_id;

	for(i = 0; i < egr_obj_num; i++){
        rv = bcm_l3_egress_stat_attach(unit, egr_obj_id[i], *stat_counter_id);
        if (BCM_FAILURE(rv)) {
            printf("ERROR: bcm_l3_egress_stat_attach(%d) - %d : %s.\n", egr_obj_id[i], rv, bcm_errmsg(rv));
            return rv;
        }
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

bcm_error_t
check_flexctr(int unit, uint32 stat_counter_id, int *flexctr_check_failed)
{
    bcm_error_t rv = BCM_E_NONE;
    uint32 num_entries = 3;
    uint32 counter_index[3] = {0, 1, 2};
    bcm_flexctr_counter_value_t counter_value[3];
	int tolerance = test_pkts_num / 10;
    int expected_packet_length = 82*(test_pkts_num/2-tolerance);

    /* Get counter value. */
    sal_memset(counter_value, 0, sizeof(counter_value));
    rv = bcm_flexctr_stat_get(unit,
                              stat_counter_id,
                              num_entries,
                              counter_index,
                              counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_get %s\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Flexctr Get : %d packets / %d bytes from egr l3 obj\n",
           COMPILER_64_LO(counter_value[1].value[0]),
           COMPILER_64_LO(counter_value[1].value[1]));

    printf("Flexctr Get : %d packets / %d bytes from egr l3 obj2\n",
           COMPILER_64_LO(counter_value[2].value[0]),
           COMPILER_64_LO(counter_value[2].value[1]));

	/* expected to receive packets in tolerace range from each of egr l3 egress obj */
    if (COMPILER_64_LO(counter_value[1].value[0]) < (test_pkts_num/2-tolerance)) {
        printf("Flexctr packets verify failed. flexctr attached to egr obj 1 got %d packets.\n",
				COMPILER_64_LO(counter_value[1].value[0]));
        *flexctr_check_failed = 1;
        rv = BCM_E_FAIL;
    }else if(COMPILER_64_LO(counter_value[2].value[0]) < (test_pkts_num/2-tolerance)) {
        printf("Flexctr packets verify failed. flexctr attached to egr obj 2 got %d packets.\n",
				COMPILER_64_LO(counter_value[2].value[0]));
        *flexctr_check_failed = 1;
        rv = BCM_E_FAIL;
	}

    if (COMPILER_64_LO(counter_value[1].value[1]) < expected_packet_length) {
        printf("Flexctr bytes verify failed. flexctr attached to egr obj 1 expected bytes %d but got %d\n",
				expected_packet_length, COMPILER_64_LO(counter_value[1].value[1]));
        *flexctr_check_failed = 1;
        rv = BCM_E_FAIL;
    }else if (COMPILER_64_LO(counter_value[2].value[1]) < expected_packet_length) {
        printf("Flexctr bytes verify failed. flexctr attached to egr obj 2 expected bytes %d but got %d\n",
				expected_packet_length, COMPILER_64_LO(counter_value[2].value[1]));
        *flexctr_check_failed = 1;
        rv = BCM_E_FAIL;
	}

    return rv;
}

bcm_error_t
config_ecmp_spray(int unit, bcm_port_t port)
{
    bcm_error_t rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchECMPLevel1RandomSeed, 0xff));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchECMPLevel2RandomSeed, 0xff88));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, port,
                bcmPortControlECMPLevel1LoadBalancingRandomizer, 6));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, port,
                bcmPortControlECMPLevel2LoadBalancingRandomizer, 8));

    return rv;
}

bcm_error_t
config_l3_random_lb_ecmp(int unit)
{
	bcm_error_t rv = BCM_E_NONE;
    bcm_if_t ingress_if = 0x1b2;
	int flags = 0;
    bcm_if_t l3_intf_id, l3_intf_id2;
    bcm_if_t egr_obj_id, egr_obj_id2;
    /* ECMP group */
    bcm_l3_egress_ecmp_t ecmp_info;
    bcm_l3_ecmp_member_t ecmp_member_array[2];


    /* Create and add ingress port to ingress VLAN */
    rv = vlan_create_add_port(unit, in_vlan, ingress_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_create_add_port(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create and add egress port to egress VLAN */
    rv = vlan_create_add_port(unit, out_vlan, egress_port[0]);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_create_add_port(): %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Create and add egress port to egress VLAN */
    rv = vlan_create_add_port(unit, out_vlan2, egress_port[1]);
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

    /* Create egress L3 interface */
    rv = create_l3_interface(unit, flags, router_mac_out2, out_vlan2, &l3_intf_id2);
    if (BCM_FAILURE(rv)) {
        printf("\nError in create_l3_interface: %s.\n",bcm_errmsg(rv));
        return rv;
    }
    printf("l3 egr intf2 = %d\n", l3_intf_id2);

    /* Create L3 egress object */
    rv = create_egr_obj(unit, l3_intf_id, dst_mac, egress_port[0], &egr_obj_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in create_egr_obj(): %s.\n", bcm_errmsg(rv));
        return rv;
    }
    printf("l3 egr obj = %d\n", egr_obj_id);

    rv = create_egr_obj(unit, l3_intf_id2, dst_mac2, egress_port[1], &egr_obj_id2);
    if (BCM_FAILURE(rv)) {
        printf("\nError in create_egr_obj(): %s.\n", bcm_errmsg(rv));
        return rv;
    }
    printf("l3 egr obj2 = %d\n", egr_obj_id2);

    /* Configure ECMP random number generator seed */
    rv = config_ecmp_spray(unit, ingress_port);
    if(BCM_FAILURE(rv)) {
        printf("\nError in config_ecmp_spray(): %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Level 2 ECMP group */
    bcm_l3_egress_ecmp_t_init(&ecmp_info);
    ecmp_info.ecmp_group_flags = BCM_L3_ECMP_UNDERLAY;
    ecmp_info.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RANDOM;
    ecmp_info.dynamic_size = 64;
    ecmp_info.max_paths = 64;

    bcm_l3_ecmp_member_t_init(&ecmp_member_array[0]);
    ecmp_member_array[0].egress_if = egr_obj_id;
    bcm_l3_ecmp_member_t_init(&ecmp_member_array[1]);
    ecmp_member_array[1].egress_if = egr_obj_id2;
    rv = bcm_l3_ecmp_create(unit, 0, &ecmp_info, 2, ecmp_member_array);
    if (BCM_FAILURE(rv)) {
       printf("Error executing bcm_l3_ecmp_create(): %s.\n", bcm_errmsg(rv));
       return rv;
    }
    ecmp_group_id = ecmp_info.ecmp_intf;
    print ecmp_group_id;

    bcm_l3_route_t route_info;
    /* Install the route for DIP */
    bcm_l3_route_t_init(&route_info);
	route_info.l3a_flags |= BCM_L3_MULTIPATH;
    route_info.l3a_intf = ecmp_group_id;
    route_info.l3a_subnet = dip;
    route_info.l3a_ip_mask = mask;
    route_info.l3a_vrf = vrf;
    rv = bcm_l3_route_add(unit, &route_info);
    if (BCM_FAILURE(rv)) {
        printf("Error executing bcm_l3_route_add(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

	/* create flex ctr and attach to l3 egress objs */
	egr_obj_array[0] = egr_obj_id;
	egr_obj_array[1] = egr_obj_id2;
	rv = config_l3_egress_flexctr(unit, 2, egr_obj_array, &stat_counter_id);
    if (BCM_FAILURE(rv)) {
       printf("ERROR: config_l3_egress_flexctr() : %s.\n", bcm_errmsg(rv));
       return rv;
    }

	return rv;
}

bcm_error_t clear_l3_random_lb_ecmp(int unit)
{
	bcm_error_t rv = BCM_E_NONE;

    bcm_l3_route_t route_info;
	bcm_l3_egress_t egr;
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

    /* Config vlan_id to l3_iif mapping */
    bcm_vlan_control_vlan_t_init(&vlan_ctrl);
    bcm_vlan_control_vlan_get(unit, in_vlan, &vlan_ctrl);
    print vlan_ctrl.ingress_if;
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

	rv = bcm_l3_ecmp_destroy(unit, ecmp_group_id);
    if (BCM_FAILURE(rv)) {
       printf("Error in bcm_l3_ecmp_destroy(): %s.\n", bcm_errmsg(rv));
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

	BCM_IF_ERROR_RETURN(vlan_destroy_remove_port(unit, out_vlan, egress_port[0]));
	BCM_IF_ERROR_RETURN(vlan_destroy_remove_port(unit, out_vlan2, egress_port[1]));
	BCM_IF_ERROR_RETURN(vlan_destroy_remove_port(unit, in_vlan, ingress_port));

    return rv;
}

bcm_error_t
clear_l3_ecmp_flexctr(int unit,  bcm_if_t ecmp_group_id, uint32 stat_counter_id)
{
    bcm_error_t rv = BCM_E_NONE;

    /* Detach flexctr from ecmp group */
	rv = bcm_l3_ecmp_stat_detach(unit, ecmp_group_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l3_ecmp_stat_detach: %s\n", bcm_errmsg(rv));
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

void test_verify(int unit)
{
	bcm_error_t rv = BCM_E_NONE;
    char   str[512];
    expected_packets[0] = expected_egress_packet1;
    expected_packets[1] = expected_egress_packet2;

    rv = registerPktioRxCallback(unit);
    if (BCM_FAILURE(rv)) {
        printf("registerPktioRxCallback() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Transmiting l3 packet into ingress_port:%d\n", ingress_port);

    snprintf(str, 512, "tx %d pbm=%d data=0x000000000001002a107777008100000B08004500003c67620000ff01bd760a3a40010a3a42730000550a000100516162636465666768696a6b6c6d6e6f7071727374757677616263646566676869; sleep quiet 1", test_pkts_num, ingress_port);
    bshell(unit, str);

    bshell(unit, "sleep quiet 1");
    /*bshell(unit, "show c");*/

    rv = check_flexctr(unit, stat_counter_id, &test_failed);
    if (BCM_FAILURE(rv)) {
        printf("check_flexctr() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    printf("\n---------------------------------------------------------------------------- \n");
    printf("    RESULT OF RTAG7 of L3 IPv4 ROUTE to ECMP GROUP with Random LB VERIFICATION  ");
    printf("\n---------------------------------------------------------------------------- \n");
    printf("Test = [%s]", !test_failed? "PASS":"FAIL");
    printf("\n------------------------------------- \n");

    if (!test_failed)
        return BCM_E_NONE;
    else
        return BCM_E_FAIL;
}

bcm_error_t
clear_l3_egress_flexctr(int unit,  int egr_obj_num, bcm_if_t *egr_obj_array, uint32 stat_counter_id)
{
    bcm_error_t rv = BCM_E_NONE;
	int i=0;

    /* Detach flexctr from l3 egr objects */
	for(i = 0; i < egr_obj_num; i++){
	    rv = bcm_l3_egress_stat_detach(unit, egr_obj_array[i]);
            if (BCM_FAILURE(rv)) {
                printf("bcm_l3_egress_stat_detach() %s\n", bcm_errmsg(rv));
                return rv;
            }
	}

    rv = bcm_flexctr_action_destroy(unit, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_action_destroy() %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Clean up pre-test setup.
 */
int test_cleanup(int unit)
{
    bcm_error_t rv;
    bcm_l3_route_t route_info;

	/* unregister Pktio callback */
    unregisterPktioRxCallback(unit);

    /* lookup a route given network and netmask */
    bcm_l3_route_t_init(&route_info);
    route_info.l3a_subnet = dip;
    route_info.l3a_ip_mask = mask;
    route_info.l3a_vrf = vrf;
    rv = bcm_l3_route_get(unit, &route_info);
    if (BCM_FAILURE(rv)) {
       printf("Error in bcm_l3_route_add(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

    /* Remove the flex counter from l3 egress object */
    rv = clear_l3_egress_flexctr(unit, egr_obj_num, egr_obj_array, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("clear_l3_egress_flexctr() failed.\n");
        return rv;
    }

    /* Remove the l3 route related configuration */
	rv = clear_l3_random_lb_ecmp(unit);
    if (BCM_FAILURE(rv)) {
        printf("clear_l3_random_lb_ecmp() failed.\n");
        return rv;
	}

    /* Remove the field configiuration */
	BCM_IF_ERROR_RETURN(clear_field_resource(unit, ifp_group, ifp_entry, 2));

    /* Remove the port loopback configiuration */
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ingress_port, BCM_PORT_LOOPBACK_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, ingress_port, BCM_PORT_DISCARD_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port[0], BCM_PORT_LOOPBACK_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port[1], BCM_PORT_LOOPBACK_NONE));

    return BCM_E_NONE;
}


/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) demonstrates the deployment for l3 route configure sequence(Done in config_l3_random_lb_ecmp())
 *  c) demonstrates the functionality(done in test_verify()).
 *  d) demonstrates the l3 route teardown configure sequence(Done in test_cleanup())
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

    print "~~~ #2) config_l3_random_lb_ecmp(): ** start **";
    if (BCM_FAILURE((rv = config_l3_random_lb_ecmp(unit)))) {
        printf("config_l3_random_lb_ecmp() failed.\n");
        return rv;
    }
    print "~~~ #2) config_l3_random_lb_ecmp(): ** end **";

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

