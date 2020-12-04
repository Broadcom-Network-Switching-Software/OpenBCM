/*  Feature  : L3 IPv4 route to trunk group with rtag7 load balance
 *
 *  Usage    : BCM.0> cint th4_hsdk_l3_rtag7_trunk.c
 *
 *  config   : bcm56990-generic-l3.config.yml
 *
 *  Log file : th4_hsdk_l3_rtag7_trunk_log.txt
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
 *  This script can verify below L3 APIs:
 *  ========
 *   bcm_l3_route_add()
 *   bcm_trunk_create()/bcm_trunk_set()
 *
 *
 *  Summary:
 *  ========
 *  This cint example demonstrate L3 route to trunk with random load balance
 *  configuration.
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Create vlan and add port to vlan. Enable L3 feature per port.
 *
 *    2) Step2 - Configuration (Done in config_l3_route_rtag7_trunk())
 *    ======================================================
 *      a) Create egress objects used for unicast route destionation.
 *      b) Create a route.
 *
 *    3) Step3 - Verification (Done in test_verify())
 *    ==========================================
 *      a) Verify the result after send 100 packet, SIP increased by step 1.
 *         {000000000001} {002a10777700} 8100 000b
 *         0800 4500 003c 6762 0000 ff01 bd76 0a3a
 *         4001 0a3a 4273 0000 550a 0001 0051 6162
 *         6364 6566 6768 696a 6b6c 6d6e 6f70 7172
 *         7374 7576 7761 6263 6465 6667 6869
 *         ...
 *
 *         {000000000001} {002a10777700} 8100 000b
 *         0800 4500 003c 6762 0000 ff01 bd13 0a3a
 *         4064 0a3a 4273 0000 550a 0001 0051 6162
 *         6364 6566 6768 696a 6b6c 6d6e 6f70 7172
 *         7374 7576 7761 6263 6465 6667 6869
 *
 *      b) Expected Result:
 *         From both trunk member ports, received packets evenly within
 *         tolerance range
 *         Packet raw data (78):
 *         {00000000fe02} {000000000002} 8100 000c
 *         0800 4500 003c 6762 0000 fe01 be76 0a3a
 *         4001 0a3a 4273 0000 550a 0001 0051 6162
 *         6364 6566 6768 696a 6b6c 6d6e 6f70 7172
 *         7374 7576 7761 6263 6465 6667 6869
 *         ...
 *
 *         {00000000fe02} {000000000002} 8100 000c
 *         0800 4500 003c 6762 0000 fe01 be13 0a3a
 *         4064 0a3a 4273 0000 550a 0001 0051 6162
 *         6364 6566 6768 696a 6b6c 6d6e 6f70 7172
 *         7374 7576 7761 6263 6465 6667 6869
 *
 *    4) Step4 - The l3 IPv4 route to trunk group with rtag7 load balance teardown
 *       configuration(Done in test_cleanup())
 *    ==========================================
 *      a) Mostly reverse the configuration, done in clear_l3_route_rtag7_trunk()
 */
cint_reset();

int unit = 0;
bcm_error_t rv = BCM_E_NONE;
bcm_port_t ingress_port;
bcm_port_t egress_port[2];
bcm_vlan_t in_vlan = 11, out_vlan = 12;
bcm_vrf_t vrf = 1;
bcm_trunk_t trunk_id = 1;
int my_station_id;
bcm_mac_t router_mac_in = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
bcm_mac_t router_mac_out = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
bcm_mac_t dst_mac = {0x00, 0x00, 0x00, 0x00, 0xfe, 0x02};
bcm_ip_t dip = 0x0a3a4273;
bcm_ip_t mask = 0xffffff00;
bcm_field_group_t ifp_group;
bcm_field_entry_t ifp_entry[2];
uint32_t stat_counter_id;
uint32 testpktnum = 100;


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

int test_failed = 0; /*Final result will be stored here */

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

int config_trunk_flexctr(int unit, bcm_trunk_t tid, uint32 *stat_counter_id)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_flexctr_action_t action = {0};
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    bcm_flexctr_value_operation_t *value_b_op = NULL;
    int options = 0;
    bcm_gport_t ingress_gport;

    action.index_num = 1;
    action.source = bcmFlexctrSourceTrunk;
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

    rv = bcm_trunk_stat_attach(unit, tid, *stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("ERROR: bcm_trunk_stat_attach() -  %s.\n", bcm_errmsg(rv));
    }
    printf("Flexctr stat_counter_id:0x%x attached to trunk %d\n", *stat_counter_id, tid);

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
    uint32 num_entries = 20;
    uint32 counter_index[20];
    bcm_flexctr_counter_value_t counter_value[20];
	int tolerance = testpktnum/10;
	int receivedpktnum = 0;
    int expected_packet_length = 82*(testpktnum/2-tolerance); /* including CRC 4 bytes */
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

	receivedpktnum = receivedpktnum + COMPILER_64_LO(counter_value[egress_port[i]].value[0]);

    if (COMPILER_64_LO(counter_value[egress_port[i]].value[0]) < (testpktnum/2-tolerance)) {
        printf("Flexctr packets verify failed, get %d packet\n", COMPILER_64_LO(counter_value[egress_port[i]].value[0]));
        flexctr_check_failed = 1;
        return BCM_E_FAIL;
    };

    if (COMPILER_64_LO(counter_value[egress_port[i]].value[1]) < expected_packet_length) {
        printf("Flexctr bytes verify failed, get %d bytes\n", COMPILER_64_LO(counter_value[egress_port[i]].value[1]));
        flexctr_check_failed = 1;
        return BCM_E_FAIL;
    };
	}

	if (receivedpktnum != testpktnum){
        printf("Flexctr packets verify failed, total get %d packet, expected total %d\n", receivedpktnum, testpktnum);
        flexctr_check_failed = 1;
        rv = BCM_E_FAIL;
	}

    return BCM_E_NONE;
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
    /* Use rtag7 load balancing */
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

  /*
     bcm_switch_control_get(unit,bcmSwitchHashSelectControl, &flags);
     flags |=(BCM_HASH_FIELD0_DISABLE_IP4|
     BCM_HASH_FIELD1_DISABLE_IP4|
     BCM_HASH_FIELD0_DISABLE_IP6|
     BCM_HASH_FIELD1_DISABLE_IP6);
     print bcm_switch_control_set(unit, bcmSwitchHashSelectControl, flags);
     */

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

  /* Block A - L2 packet field selection (RTAG7_HASH_FIELD_BMAP3) */
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

  flags = BCM_HASH_FIELD_SRCMOD |
    BCM_HASH_FIELD_SRCPORT |
    BCM_HASH_FIELD_PROTOCOL |
    BCM_HASH_FIELD_DSTL4 |
    BCM_HASH_FIELD_SRCL4 |
    BCM_HASH_FIELD_IP4DST_LO |
    BCM_HASH_FIELD_IP4DST_HI |
    BCM_HASH_FIELD_IP4SRC_LO |
    BCM_HASH_FIELD_IP4SRC_HI;
  /* Block A - L3 packet field selection (RTAG7_HASH_FIELD_BMAP_1) */
  rv = bcm_switch_control_set(unit, bcmSwitchHashIP4Field0, flags);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpField0, flags);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpPortsEqualField0, flags);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* Block B - L3 packet field selection */
  rv = bcm_switch_control_set(unit, bcmSwitchHashIP4Field1, flags);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpField1, flags);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpPortsEqualField1, flags);
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

  return rv;
}

bcm_error_t clear_trunk_rtag7(int unit)
{
  int arg = 0;
  bcm_error_t rv = BCM_E_NONE;

  /* Block A - L2 packet field selection (RTAG7_HASH_FIELD_BMAP3) */
  rv= bcm_switch_control_set(unit, bcmSwitchHashL2Field0, arg);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* Block B - L2 packet field selection */
  rv= bcm_switch_control_set(unit, bcmSwitchHashL2Field1, arg);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* Block A - L3 packet field selection (RTAG7_HASH_FIELD_BMAP_1) */
  rv = bcm_switch_control_set(unit, bcmSwitchHashIP4Field0, arg);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpField0, arg);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpPortsEqualField0, arg);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* Block B - L3 packet field selection */
  rv = bcm_switch_control_set(unit, bcmSwitchHashIP4Field1, arg);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpField1, arg);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpPortsEqualField1, arg);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_switch_control_set(unit, bcmSwitchHashSeed0, arg);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_switch_control_set(unit, bcmSwitchHashSeed1, arg);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_switch_control_set(unit, bcmSwitchHashField0PreProcessEnable, FALSE);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_switch_control_set(unit, bcmSwitchHashField1PreProcessEnable, FALSE);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* Configure HASH A and HASH B functions */
  rv = bcm_switch_control_set(unit, bcmSwitchHashField0Config, arg);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_switch_control_set(unit, bcmSwitchHashField0Config1, arg);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_switch_control_set(unit, bcmSwitchHashField1Config, arg);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_switch_control_set(unit, bcmSwitchHashField1Config1, arg);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_switch_control_set(unit, bcmSwitchTrunkHashSet0UnicastOffset, 0 + 0);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  return rv;
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
config_l3_route_rtag7_trunk(int unit)
{
	bcm_error_t rv = BCM_E_NONE;
    bcm_if_t ingress_if = 0x1b2;
	int flags = 0;
    bcm_if_t l3_intf_id;
    bcm_if_t egr_obj_id;
    bcm_gport_t trunk_gp_id;
    bcm_port_t trunk_member_ports[2];


    /* Create and add ingress port to ingress VLAN */
    rv = vlan_create_add_port(unit, in_vlan, ingress_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_create_add_port(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create and add egress ports to egress VLAN */
    rv = vlan_create_add_ports(unit, out_vlan, 2, egress_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_create_add_ports(): %s.\n",bcm_errmsg(rv));
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

    /* Create L3 egress object */
    rv = create_egr_obj(unit, l3_intf_id, dst_mac, trunk_gp_id, &egr_obj_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in create_egr_obj(): %s.\n", bcm_errmsg(rv));
        return rv;
    }
    printf("l3 egr obj = %d\n", egr_obj_id);

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

	/* create flex ctr and attach to egress port */
	rv = config_egr_port_flexctr(unit, 2, egress_port, &stat_counter_id);
    if (BCM_FAILURE(rv)) {
       printf("ERROR: config_egr_port_flexctr() : %s.\n", bcm_errmsg(rv));
       return rv;
    }

	return rv;
}

bcm_error_t clear_l3_route_rtag7_trunk(int unit)
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

	rv = bcm_trunk_destroy(unit, trunk_id);
    if (BCM_FAILURE(rv)) {
       printf("Error in bcm_trunk_destroy(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

    rv = clear_trunk_rtag7(unit);
    if (BCM_FAILURE(rv)) {
       printf("Error in clear_trunk_rtag7(): %s.\n", bcm_errmsg(rv));
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

	BCM_IF_ERROR_RETURN(vlan_destroy_remove_ports(unit, out_vlan, 2, egress_port));
	BCM_IF_ERROR_RETURN(vlan_destroy_remove_port(unit, in_vlan, ingress_port));

    return rv;
}

void test_verify(int unit)
{
	bcm_error_t rv = BCM_E_NONE;
    char   str[512];
	int i=0;
	uint16 ipchksum = 0xBD76;
    uint16 siplo = 0x4001;

    printf("Transmiting l3 packet into ingress_port:%d\n", ingress_port);
	for(i=0;i<testpktnum;i++){
    snprintf(str, 512, "tx 1 pbm=%d data=0x000000000001002a107777008100000B08004500003c67620000ff01%x0a3a%x0a3a42730000550a000100516162636465666768696a6b6c6d6e6f7071727374757677616263646566676869; sleep quiet 1", ingress_port, ipchksum-i, siplo+i);
    bshell(unit, str);
    }

    bshell(unit, "sleep quiet 2");
    /*bshell(unit, "show c");*/

    rv = check_flexctr(unit, stat_counter_id, &test_failed);
    if (BCM_FAILURE(rv)) {
        printf("check_flexctr() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    printf("\n-------------------------------------------------------------------- \n");
    printf("    RESULT OF L3 ROUTE TO TRUNK WITH RTAG7 LOAD BALANCE VERIFICATION  ");
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

    /* Remove the flex counter from egress port */
    rv = clear_egr_port_flexctr(unit, 2, egress_port, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("clear_egr_port_flexctr() failed.\n");
        return rv;
    }

    /* Remove the l3 IPv4 route to trunk group related configuration */
	rv = clear_l3_route_rtag7_trunk(unit);
    if (BCM_FAILURE(rv)) {
        printf("clear_l3_route_rtag7_trunk() failed.\n");
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
 *  b) demonstrates the deployment for l3 IPv4 route to trunk group configure sequence
 *     (Done in config_l3_route_rtag7_trunk())
 *  c) demonstrates the functionality(done in test_verify()).
 *  d) demonstrates the l3 IPv4 route to trunk group teardown configure sequence
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

    print "~~~ #2) config_l3_route_rtag7_trunk(): ** start **";
    if (BCM_FAILURE((rv = config_l3_route_rtag7_trunk(unit)))) {
        printf("config_l3_route_rtag7_trunk() failed.\n");
        return rv;
    }
    print "~~~ #2) config_l3_route_rtag7_trunk(): ** end **";

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

