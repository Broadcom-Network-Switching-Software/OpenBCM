/*  Feature  : L2 switch to trunk group with rtag7 load balance
 *
 *  Usage    : BCM.0> cint th4_hsdk_l2_rtag7_trunk.c
 *
 *  config   : bcm56990-generic-l2.config.yml
 *
 *  Log file : th4_hsdk_l2_rtag7_trunk_log.txt
 *
 *  Test Topology :
 *                  +------------------------------+
 *                  |                              |
 *                  |                              | trunk member port 0
 *                  |                              +---------------------+
 *     ingress_port |                              |
 * +----------------+          SWITCH              |
 *                  |                              |
 *                  |                              | trunk member port 1
 *                  |                              +---------------------+
 *                  |                              |
 *                  |                              |
 *                  +------------------------------+
 *
 *  This script can verify below APIs:
 *  ========
 *   bcm_vlan_create()/bcm_vlan_add()
 *   bcm_trunk_create()/bcm_trunk_set()
 *   bcm_l2_addr_add()
 *
 *
 *  Summary:
 *  ========
 *  This cint example demonstrate L2 switch to trunk with RTAG7 load balance
 *  configuration.
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Create vlan and add port to vlan, add test packets' DMAC and VLAN to l2_entry.
 *
 *    2) Step2 - Configuration (Done in config_l2_rtag7_trunk())
 *    ======================================================
 *      a) Create egress objects used for unicast route destionation.
 *      b) Create a route.
 *
 *    3) Step3 - Verification (Done in test_verify())
 *    ==========================================
 *      a) Verify the result after send 2 packets.
 *         {00000000fe02} {002a10777700} 8100 000b
 *         0800 4500 003c 6762 0000 ff11 bd65 0a3a
 *         4002 0a3a 4273 0000 550a 0028 6907 6162
 *         6364 6566 6768 696a 6b6c 6d6e 6f70 7172
 *         7374 7576 7761 6263 6465 6667 6869
 *
 *         {00000000fe02} {002a10777701} 8100 000b  SMAC increased 1
 *         0800 4500 003c 6762 0000 ff11 bd65 0a3a
 *         4002 0a3a 4273 0000 550a 0028 6907 6162
 *         6364 6566 6768 696a 6b6c 6d6e 6f70 7172
 *         7374 7576 7761 6263 6465 6667 6869
 *
 *      b) Expected Result:
 *         From trunk member port 0,
 *         Packet raw data (78):
 *         {00000000fe02} {002a10777700} 8100 000b
 *         0800 4500 003c 6762 0000 ff11 bd65 0a3a
 *         4002 0a3a 4273 0000 550a 0028 6907 6162
 *         6364 6566 6768 696a 6b6c 6d6e 6f70 7172
 *         7374 7576 7761 6263 6465 6667 6869
 *
 *         From trunk member port 1,
 *         Packet raw data (78):
 *         {00000000fe02} {002a10777701} 8100 000b
 *         0800 4500 003c 6762 0000 ff11 bd65 0a3a
 *         4002 0a3a 4273 0000 550a 0028 6907 6162
 *         6364 6566 6768 696a 6b6c 6d6e 6f70 7172
 *         7374 7576 7761 6263 6465 6667 6869
 *
 *    4) Step4 - The l2 switch to trunk group with rtag7 load balance teardown
 *       configuration(Done in test_cleanup())
 *    ==========================================
 *      a) Mostly reverse the configuration, done in clear_l2_rtag7_trunk()
 */
cint_reset();

int unit = 0;
bcm_error_t rv = BCM_E_NONE;
bcm_port_t ingress_port;
bcm_port_t egress_port[2];
bcm_vlan_t in_vlan = 11;
bcm_trunk_t trunk_id = 1;
bcm_mac_t dst_mac = {0x00, 0x00, 0x00, 0x00, 0xfe, 0x02};
bcm_field_group_t ifp_group;
bcm_field_entry_t ifp_entry[2];
uint32_t stat_counter_id;


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

    /* Create and add all test ports to VLAN */
    rv = vlan_create_add_ports(unit, in_vlan, 3, port_list);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_create_add_ports(): %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /*bshell(unit, "pw start report +raw +decode");*/

	return BCM_E_NONE;
}

/*Expected Egress packets */
unsigned char expected_egress_packet1[78] = {
0x00, 0x00, 0x00, 0x00, 0xfe, 0x02, 0x00, 0x2a, 0x10, 0x77, 0x77, 0x00, 0x81, 0x00, 0x00, 0x0b,
0x08, 0x00, 0x45, 0x00, 0x00, 0x3c, 0x67, 0x62, 0x00, 0x00, 0xff, 0x11, 0xbd, 0x65, 0x0a, 0x3a,
0x40, 0x02, 0x0a, 0x3a, 0x42, 0x73, 0x00, 0x00, 0x55, 0x0a, 0x00, 0x28, 0x69, 0x07, 0x61, 0x62,
0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72,
0x73, 0x74, 0x75, 0x76, 0x77, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
};

unsigned char expected_egress_packet2[78] = {
0x00, 0x00, 0x00, 0x00, 0xfe, 0x02, 0x00, 0x2a, 0x10, 0x77, 0x77, 0x01, 0x81, 0x00, 0x00, 0x0b,
0x08, 0x00, 0x45, 0x00, 0x00, 0x3c, 0x67, 0x62, 0x00, 0x00, 0xff, 0x11, 0xbd, 0x65, 0x0a, 0x3a,
0x40, 0x02, 0x0a, 0x3a, 0x42, 0x73, 0x00, 0x00, 0x55, 0x0a, 0x00, 0x28, 0x69, 0x07, 0x61, 0x62,
0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72,
0x73, 0x74, 0x75, 0x76, 0x77, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
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
	} else {
        printf("pktioRxCallback: packet received from correct expected ports\n");
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

int config_egr_port_flexctr(int unit, int port_num, bcm_gport_t *egr_port, uint32 *stat_counter_id)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_flexctr_action_t action = {0};
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    bcm_flexctr_value_operation_t *value_b_op = NULL;
    int options = 0;
	int i = 0;

    action.index_num = 20;
    action.source = bcmFlexctrSourceEgrPort;
    action.hint = 0;
    action.mode = bcmFlexctrCounterModeNormal;

    /* Counter index is always zero. */
    index_op = &action.index_operation;
    index_op->object[0] = bcmFlexctrObjectStaticEgrEgressPort;
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

	for(i=0;i<port_num;i++){
    rv = bcm_port_stat_attach(unit, egr_port[i], *stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("ERROR: bcm_port_stat_attach() -  %s.\n", bcm_errmsg(rv));
    }
    printf("Flexctr stat_counter_id:0x%x attached to egress port %d\n", *stat_counter_id, egr_port[i]);
	}

    return rv;
}

bcm_error_t
clear_egr_port_flexctr(int unit, int port_num,  bcm_gport_t *egr_port, uint32 stat_counter_id)
{
    bcm_error_t rv = BCM_E_NONE;
	int i = 0;

    /* Detach flexctr from port */

	for(i=0;i<port_num;i++){
	rv = bcm_port_stat_detach_with_id(unit, egr_port[i], stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_port_stat_detach_with_id: %s\n", bcm_errmsg(rv));
        return rv;
    }
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
check_flexctr(int unit, uint32 stat_counter_id, int *flexctr_check_failed)
{
    bcm_error_t rv = BCM_E_NONE;
    uint32 num_entries = 20;
    uint32 counter_index[20];
    bcm_flexctr_counter_value_t counter_value[20];
    int expected_packet_length = 82;
	int i = 0;

	for (i=0;i<20;i++){
		counter_index[i] = i;
	}

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

	for(i=0;i<2;i++){
    printf("Flexctr Get : %d packets / %d bytes from egress port %d\n",
           COMPILER_64_LO(counter_value[egress_port[i]].value[0]),
           COMPILER_64_LO(counter_value[egress_port[i]].value[1]), egress_port[i]);

    if (COMPILER_64_LO(counter_value[egress_port[i]].value[0]) != 1) {
        printf("Flexctr packets verify failed, get %d packet\n", COMPILER_64_LO(counter_value[egress_port[i]].value[0]));
        flexctr_check_failed = 1;
        rv = BCM_E_FAIL;
        print counter_value;
    };

    if (COMPILER_64_LO(counter_value[egress_port[i]].value[1]) != expected_packet_length) {
        printf("Flexctr bytes verify failed, get %d bytes\n", COMPILER_64_LO(counter_value[egress_port[i]].value[1]));
        flexctr_check_failed = 1;
        rv = BCM_E_FAIL;
        print counter_value;
    };
	}

    return rv;
}

/*
 * This function creates a trunk group and adds the member ports
 */
bcm_error_t trunk_create(int unit, bcm_trunk_t *tid, int count, bcm_port_t *member_ports)
{
    bcm_error_t rv = BCM_E_NONE;
    int i = 0;
    bcm_trunk_info_t trunk_info;
    bcm_trunk_member_t *member_info = NULL;
    bcm_trunk_member_t info;

    bcm_trunk_info_t_init(&trunk_info);
    /* Use RTAG77 load balancing */
    trunk_info.psc = BCM_TRUNK_PSC_PORTFLOW;
    trunk_info.dlf_index = BCM_TRUNK_UNSPEC_INDEX;
    trunk_info.mc_index = BCM_TRUNK_UNSPEC_INDEX;
    trunk_info.ipmc_index = BCM_TRUNK_UNSPEC_INDEX;
    rv = bcm_trunk_create(unit, BCM_TRUNK_FLAG_WITH_ID, tid);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_trunk_create %s.\n", bcm_errmsg (rv));
        return rv;
    }else {
        printf ("bcm_trunk_create trunk_id = %d.\n", *tid);
	}

    member_info = sal_alloc((sizeof(info) * count), "trunk members");
    for (i = 0 ; i < count ; i++) {
        bcm_trunk_member_t_init(&member_info[i]);
        BCM_GPORT_MODPORT_SET(member_info[i].gport, 0, member_ports[i]);
    }

    rv = bcm_trunk_set(unit, *tid, &trunk_info, count, member_info);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_trunk_set %s.\n", bcm_errmsg (rv));
        return rv;
    }

    return rv;
}

bcm_error_t config_trunk_rtag7(int unit)
{
    int flags;
    bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t gport_in;

    rv=bcm_switch_control_get(unit, bcmSwitchHashControl, &flags);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_get(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    flags |= (BCM_HASH_CONTROL_TRUNK_NUC_ENHANCE |
        BCM_HASH_CONTROL_TRUNK_NUC_SRC |
        BCM_HASH_CONTROL_TRUNK_NUC_DST |
        BCM_HASH_CONTROL_TRUNK_NUC_MODPORT);

    rv=bcm_switch_control_set(unit, bcmSwitchHashControl, flags);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    rv = bcm_switch_control_get(unit,bcmSwitchHashSelectControl, &flags);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }
    flags |=(BCM_HASH_FIELD0_DISABLE_IP4|
    BCM_HASH_FIELD1_DISABLE_IP4|
    BCM_HASH_FIELD0_DISABLE_IP6|
    BCM_HASH_FIELD1_DISABLE_IP6|
	BCM_HASH_FIELD0_DISABLE_MPLS|
	BCM_HASH_FIELD1_DISABLE_MPLS|
	BCM_HASH_FIELD0_DISABLE_TUNNEL_IP4_IP|
	BCM_HASH_FIELD1_DISABLE_TUNNEL_IP4_IP|
	BCM_HASH_FIELD0_DISABLE_VXLAN|
	BCM_HASH_FIELD1_DISABLE_VXLAN);
    rv = bcm_switch_control_set(unit, bcmSwitchHashSelectControl, flags);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    /* Block A - L2 packet field selection (RTAG7_HASH_FIELD_BMAP3) */
    flags = BCM_HASH_FIELD_SRCMOD |
      BCM_HASH_FIELD_SRCPORT |
      BCM_HASH_FIELD_VLAN |
      BCM_HASH_FIELD_ETHER_TYPE |
      BCM_HASH_FIELD_MACDA_LO |
      BCM_HASH_FIELD_MACDA_MI |
      BCM_HASH_FIELD_MACDA_HI |
      BCM_HASH_FIELD_MACSA_LO |
      BCM_HASH_FIELD_MACSA_MI |
      BCM_HASH_FIELD_MACSA_HI;
    rv= bcm_switch_control_set(unit, bcmSwitchHashL2Field0, flags);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    /* Block B - L2 packet field selection */
    rv= bcm_switch_control_set(unit, bcmSwitchHashL2Field1, flags);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchHashSeed0, 0x33333333);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchHashSeed1, 0x55555555);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchHashField0PreProcessEnable, TRUE);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchHashField1PreProcessEnable, TRUE);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    /* Configure HASH A and HASH B functions */
    rv = bcm_switch_control_set(unit, bcmSwitchHashField0Config,
        BCM_HASH_FIELD_CONFIG_CRC32LO);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchHashField0Config1,
        BCM_HASH_FIELD_CONFIG_CRC32HI);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchHashField1Config,
        BCM_HASH_FIELD_CONFIG_CRC32HI);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchHashField1Config1,
        BCM_HASH_FIELD_CONFIG_CRC32LO);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchTrunkHashSet0UnicastOffset, 0 + 0);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    bcm_switch_control_set(unit, bcmSwitchTrunkHashSet0NonUnicastOffset, 0 + 0);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    return rv;
}

bcm_error_t
config_l2_rtag7_trunk(int unit)
{
	bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t trunk_gp_id;
    bcm_port_t trunk_member_ports[2];
    bcm_l2_addr_t l2addr;

    rv = config_trunk_rtag7(unit);
    if (BCM_FAILURE(rv)) {
        printf("\nError in config_trunk_rtag7(): %s.\n",bcm_errmsg(rv));
        return rv;
    }

    trunk_member_ports[0] = egress_port[0];
    trunk_member_ports[1] = egress_port[1];
    rv = trunk_create(unit, &trunk_id, 2, trunk_member_ports);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing create_trunk(): %s.\n", bcm_errmsg (rv));
        return rv;
    }
    printf("\nCreate trunk id:%d with member ports %d and %d\n", trunk_id, trunk_member_ports[0], trunk_member_ports[1]);
    BCM_GPORT_TRUNK_SET (trunk_gp_id, trunk_id);
	print trunk_gp_id;

    bcm_l2_addr_t_init(&l2addr, dst_mac, in_vlan);
    l2addr.port = trunk_gp_id;
    rv = bcm_l2_addr_add(unit, &l2addr);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l2_addr_add(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

	/* create flex ctr and attach to trunk group */
	rv = config_egr_port_flexctr(unit, 2, egress_port, &stat_counter_id);
    if (BCM_FAILURE(rv)) {
       printf("ERROR: config_egr_port_flexctr() : %s.\n", bcm_errmsg(rv));
       return rv;
    }

	return rv;
}

bcm_error_t clear_l2_rtag7_trunk(int unit)
{
	bcm_error_t rv = BCM_E_NONE;

 	rv = bcm_trunk_destroy(unit, trunk_id);
    if (BCM_FAILURE(rv)) {
       printf("Error in bcm_trunk_destroy(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

    rv = bcm_l2_addr_delete(unit, dst_mac, in_vlan);
    if (BCM_FAILURE(rv)) {
       printf("Error in bcm_l2_addr_delete(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

    rv = bcm_l2_clear(unit);
    if (BCM_FAILURE(rv)) {
       printf("Error in bcm_l2_clear(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

	BCM_IF_ERROR_RETURN(vlan_destroy_remove_ports(unit, in_vlan, 2, egress_port));

    return rv;
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

    printf("Transmiting test packets into ingress_port:%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000FE02002A107777008100000B08004500003C67620000FF11BD650A3A40020A3A42730000550A002869076162636465666768696A6B6C6D6E6F7071727374757677616263646566676869; sleep 1", ingress_port);
    bshell(unit, str);

    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000FE02002A107777018100000B08004500003C67620000FF11BD650A3A40020A3A42730000550A002869076162636465666768696A6B6C6D6E6F7071727374757677616263646566676869; sleep 1", ingress_port);
    bshell(unit, str);

    bshell(unit, "sleep 1");
    /*bshell(unit, "show c");*/

    rv = check_flexctr(unit, stat_counter_id, &test_failed);
    if (BCM_FAILURE(rv)) {
        printf("check_flexctr() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    printf("\n-------------------------------------------------------------------- \n");
    printf("    RESULT OF L2 SWITCH TO TRUNK WITH RTAG7 LOAD BALANCE VERIFICATION  ");
    printf("\n-------------------------------------------------------------------- \n");
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
    bcm_l3_route_t route_info;

	/* unregister Pktio callback */
    unregisterPktioRxCallback(unit);

    /* Remove the flex counter from trunk object */
    rv = clear_egr_port_flexctr(unit, 2, egress_port, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("clear_egr_port_flexctr() failed.\n");
        return rv;
    }

    /* Remove the l2 switch to trunk group related configuration */
	rv = clear_l2_rtag7_trunk(unit);
    if (BCM_FAILURE(rv)) {
        printf("clear_l2_rtag7_trunk() failed.\n");
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
 *  b) demonstrates the deployment for l2 switch to trunk group configure sequence
 *     (Done in config_l2_rtag7_trunk())
 *  c) demonstrates the functionality(done in test_verify()).
 *  d) demonstrates the l2 switch to trunk group teardown configure sequence
 *     (Done in test_cleanup())
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

    print "~~~ #2) config_l2_rtag7_trunk(): ** start **";
    if (BCM_FAILURE((rv = config_l2_rtag7_trunk(unit)))) {
        printf("config_l2_rtag7_trunk() failed.\n");
        return rv;
    }
    print "~~~ #2) config_l2_rtag7_trunk(): ** end **";

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

