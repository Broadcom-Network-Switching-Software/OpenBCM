/*  Feature  : L2 switch to trunk group with udf rtag7 load balance
 *
 *  Usage    : BCM.0> cint th4_hsdk_l2_rtag7_udf_trunk.c
 *
 *  config   : bcm56990-generic-l2.config.yml
 *
 *  Log file : th4_hsdk_l2_rtag7_udf_trunk_log.txt
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
 *  This cint example demonstrate L2 switch to trunk with UDF RTAG7 load balance
 *  configuration.
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Create vlan and add port to vlan, add test packets' DMAC and VLAN to l2_entry.
 *
 *    2) Step2 - Configuration (Done in config_l2_rtag7_udf_trunk())
 *    ======================================================
 *      a) Create egress objects used for unicast route destionation.
 *      b) Create a route.
 *
 *    3) Step3 - Verification (Done in test_verify())
 *    ==========================================
 *      a) Verify the result after send 10 packets,
 *      which increase the L3 DST/SRC port, and L3 SRC/DST address.
 *
 *      b) Expected Result:
 *         From trunk member port 0 received 5 packets
 *         From trunk member port 1 received 5 packets
 *
 *    4) Step4 - The l2 switch to trunk group with udf rtag7 load balance teardown
 *       configuration(Done in test_cleanup())
 *    ==========================================
 *      a) Mostly reverse the configuration, done in clear_l2_rtag7_udf_trunk()
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

bcm_error_t
check_flexctr(int unit, uint32 stat_counter_id, int *flexctr_check_failed)
{
    bcm_error_t rv = BCM_E_NONE;
    uint32 num_entries = 20;
    uint32 counter_index[20];
    bcm_flexctr_counter_value_t counter_value[20];
    int expected_packet_length = 103*5;
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

    if (COMPILER_64_LO(counter_value[egress_port[i]].value[0]) != 5) {
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

/*
 * The UDF chunks UDF_1 chunks 6 and 7 are overlaid on hash bins 2 and 3
 * for RTAG7 calculation.
 */
bcm_error_t udf_hash_init(int unit)
{
    int arg = 0;
    bcm_error_t rv = BCM_E_NONE;

    /* Enable UDF hashing */
    rv = bcm_switch_control_set(unit, bcmSwitchUdfHashEnable, BCM_HASH_FIELD0_ENABLE_UDFHASH |
            BCM_HASH_FIELD1_ENABLE_UDFHASH);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_switch_control_set for UdfHashEnable %s.\n", bcm_errmsg (rv));
        return rv;
    }

    /* Enable Bin 2 and 3 for UDF HASH */
    arg = BCM_HASH_FIELD_SRCMOD | BCM_HASH_FIELD_SRCPORT;
    rv = bcm_switch_control_set(unit, bcmSwitchHashIP4Field0, arg);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_switch_control_set for HashIP4Field0 %s.\n", bcm_errmsg (rv));
        return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchHashIP4Field1, arg);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_switch_control_set for HashIP4Field1 %s.\n", bcm_errmsg (rv));
        return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpField0, arg);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_switch_control_set for HashIP4TcpUdpField0 %s.\n",
                 bcm_errmsg (rv));
        return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpField1, arg);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_switch_control_set for HashIP4TcpUdpField1 %s.\n",
                 bcm_errmsg (rv));
        return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpPortsEqualField0, arg);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_switch_control_set for HashIP4TcpUdpPortsEqualField0  %s.\n",
                 bcm_errmsg (rv));
        return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpPortsEqualField1, arg);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_switch_control_set for HashIP4TcpUdpPortsEqualField1  %s.\n",
                 bcm_errmsg (rv));
        return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchHashField0Config,
                                BCM_HASH_FIELD_CONFIG_CRC32LO);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_switch_control_set for HashField0Config %s.\n", bcm_errmsg (rv));
        return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchHashField1Config,
                                BCM_HASH_FIELD_CONFIG_CRC32LO);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_switch_control_set for HashField1Config %s.\n", bcm_errmsg (rv));
        return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchHashSeed0, 0x33333333);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_switch_control_set for HashSeed0 %s.\n", bcm_errmsg (rv));
        return rv;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchHashSeed1, 0x55555555);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_switch_control_set for HashSeed1 %s.\n", bcm_errmsg (rv));
        return rv;
    }

    rv = bcm_switch_control_get(0, bcmSwitchHashControl, &arg);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_switch_control_get for HashControl %s.\n", bcm_errmsg (rv));
        return rv;
    }
    arg |= BCM_HASH_CONTROL_TRUNK_NUC_ENHANCE;
    rv = bcm_switch_control_set(0, bcmSwitchHashControl, arg);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_switch_control_set for HashControl %s.\n", bcm_errmsg (rv));
        return rv;
    }

    return rv;
}

/*
 * Create UDF to get IPv4 UDP packet src & dest ports
 */
bcm_error_t udf_hash_udp_ports_create_udf(int unit)
{
    bcm_udf_alloc_hints_t hints;
    bcm_udf_t udf_info;
    bcm_udf_pkt_format_info_t pkt_format;
    bcm_udf_pkt_format_id_t pkt_format_id = 0;
    bcm_udf_hash_config_t config;
    bcm_udf_id_t udf_ids[2];
    int udf_offset[2] = {0, 16};   /* Source and destination UDP port */
    bcm_error_t rv = BCM_E_NONE;
    int i = 0;

    /* Gnerate packet format to match IP/UDP packet */
    bcm_udf_pkt_format_info_t_init(&pkt_format);
    pkt_format.l2                 = BCM_PKT_FORMAT_L2_ANY;
    pkt_format.vlan_tag           = BCM_PKT_FORMAT_VLAN_TAG_ANY;
    pkt_format.ip_protocol        = 0x11;  /* UDP */
    pkt_format.ip_protocol_mask   = 0xff;
    pkt_format.outer_ip           = BCM_PKT_FORMAT_IP_ANY;
    pkt_format.inner_ip           = BCM_PKT_FORMAT_IP_NONE;
    pkt_format.tunnel             = BCM_PKT_FORMAT_TUNNEL_NONE;
    rv = bcm_udf_pkt_format_create(unit, FALSE /* IPv4 */, &pkt_format, &pkt_format_id);
    if (BCM_FAILURE(rv)) {
        printf ("Failed to create pkt format, error code: %s\n", bcm_errmsg(rv));
        return rv;
    }

    bcm_udf_alloc_hints_t_init(&hints);
    hints.flags    = BCM_UDF_CREATE_O_UDFHASH;

    /* Create UDF to match on UDP ports */
    for (i = 0; i < 2; i++) {
        bcm_udf_t_init(&udf_info);
        udf_info.flags = 0;
        udf_info.layer = bcmUdfLayerL4OuterHeader;
        udf_info.start = udf_offset[i];
        udf_info.width = 16;
        rv = bcm_udf_create(unit, &hints, &udf_info, &udf_ids[i]);
        if (BCM_FAILURE(rv)) {
            printf ("failed to create UDF for dest port, error code: %s\n", bcm_errmsg(rv));
            return rv;
        }

        /* Attach both UDF id to ip UDP packet */

        rv  = bcm_udf_pkt_format_add(unit, udf_ids[i], pkt_format_id);
        if (BCM_FAILURE(rv)) {
            printf ("Failed to attach udf id %d to pkt format %d\n", udf_ids[i], pkt_format_id);
            return rv;
        }

        /* Config hash mask for UDF_1 chunks 6 and 7 overlaid on hash bins 2 and 3 */
        bcm_udf_hash_config_t_init(&config);
        config.udf_id = udf_ids[i];
        config.mask_length = 2;
        config.hash_mask[0] = 0xff;
        config.hash_mask[1] = 0xff;
        rv = bcm_udf_hash_config_add(unit, 0, &config);
        if (BCM_FAILURE(rv)) {
            printf("failed, error code: %s\n", bcm_errmsg(BCM_FAILURE(rv)));
            return rv;
        }
    }

    return rv;
}

bcm_error_t
config_l2_rtag7_udf_trunk(int unit)
{
	bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t trunk_gp_id;
    bcm_port_t trunk_member_ports[2];
    bcm_l2_addr_t l2addr;

    rv = bcm_udf_init(unit);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_udf_init(): %s.\n", bcm_errmsg (rv));
        return rv;
    }

    /* UDF hash configurations */
    rv = udf_hash_init(unit);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing udf_hash_init(): %s.\n", bcm_errmsg (rv));
        return rv;
    }

    rv = udf_hash_udp_ports_create_udf(unit);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing udf_hash_udp_ports_create(): %s.\n", bcm_errmsg (rv));
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


	/* create flex ctr and attach to egress port */
	rv = config_egr_port_flexctr(unit, 2, egress_port, &stat_counter_id);
    if (BCM_FAILURE(rv)) {
       printf("ERROR: config_egr_port_flexctr() : %s.\n", bcm_errmsg(rv));
       return rv;
    }

	return rv;
}

bcm_error_t clear_l2_rtag7_udf_trunk(int unit)
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

bcm_error_t
clear_trunk_flexctr(int unit,  bcm_trunk_t tid, uint32 stat_counter_id)
{
    bcm_error_t rv = BCM_E_NONE;

    /* Detach flexctr from trunk */
	rv = bcm_trunk_stat_detach(unit, tid, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_trunk_stat_detach: %s\n", bcm_errmsg(rv));
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

    printf("Transmiting test packets into ingress_port:%d\n", ingress_port);

    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000FE020000000022228100000B08004500005100010000401163880A0A01000A0A020013880050003DCE402820DB200020002045200020002054200020002040200020402000205E20A520CA200B2028209E20C020A8202820B22008200020FF", ingress_port);
    bshell(unit, str);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000FE020000000022228100000B08004500005100010000401163860A0A01010A0A020113890051003DCE3C2820DB200020002045200020002054200020002040200020402000205E20A520CA200B2028209E20C020A8202820B22008200020FF", ingress_port);
    bshell(unit, str);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000FE020000000022228100000B08004500005100010000401163840A0A01020A0A0202138A0052003DCE382820DB200020002045200020002054200020002040200020402000205E20A520CA200B2028209E20C020A8202820B22008200020FF", ingress_port);
    bshell(unit, str);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000FE020000000022228100000B08004500005100010000401163820A0A01030A0A0203138B0053003DCE342820DB200020002045200020002054200020002040200020402000205E20A520CA200B2028209E20C020A8202820B22008200020FF", ingress_port);
    bshell(unit, str);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000FE020000000022228100000B08004500005100010000401163800A0A01040A0A0204138C0054003DCE302820DB200020002045200020002054200020002040200020402000205E20A520CA200B2028209E20C020A8202820B22008200020FF", ingress_port);
    bshell(unit, str);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000FE020000000022228100000B080045000051000100004011637E0A0A01050A0A0205138D0055003DCE2C2820DB200020002045200020002054200020002040200020402000205E20A520CA200B2028209E20C020A8202820B22008200020FF", ingress_port);
    bshell(unit, str);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000FE020000000022228100000B080045000051000100004011637C0A0A01060A0A0206138E0056003DCE282820DB200020002045200020002054200020002040200020402000205E20A520CA200B2028209E20C020A8202820B22008200020FF", ingress_port);
    bshell(unit, str);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000FE020000000022228100000B080045000051000100004011637A0A0A01070A0A0207138F0057003DCE242820DB200020002045200020002054200020002040200020402000205E20A520CA200B2028209E20C020A8202820B22008200020FF", ingress_port);
    bshell(unit, str);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000FE020000000022228100000B08004500005100010000401163780A0A01080A0A020813900058003DCE202820DB200020002045200020002054200020002040200020402000205E20A520CA200B2028209E20C020A8202820B22008200020FF", ingress_port);
    bshell(unit, str);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000FE020000000022228100000B08004500005100010000401163760A0A01090A0A020913910059003DCE1C2820DB200020002045200020002054200020002040200020402000205E20A520CA200B2028209E20C020A8202820B22008200020FF", ingress_port);
    bshell(unit, str);

    bshell(unit, "sleep 2");
    /*bshell(unit, "show c");*/

    rv = check_flexctr(unit, stat_counter_id, &test_failed);
    if (BCM_FAILURE(rv)) {
        printf("check_flexctr() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    printf("\n-------------------------------------------------------------------- \n");
    printf("    RESULT OF L2 SWITCH TO TRUNK WITH UDF RTAG7 LOAD BALANCE VERIFICATION  ");
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

    /* Remove the l2 switch to trunk group related configuration */
	rv = clear_l2_rtag7_udf_trunk(unit);
    if (BCM_FAILURE(rv)) {
        printf("clear_l2_rtag7_udf_trunk() failed.\n");
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
 *     (Done in config_l2_rtag7_udf_trunk())
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

    print "~~~ #2) config_l2_rtag7_udf_trunk(): ** start **";
    if (BCM_FAILURE((rv = config_l2_rtag7_udf_trunk(unit)))) {
        printf("config_l2_rtag7_udf_trunk() failed.\n");
        return rv;
    }
    print "~~~ #2) config_l2_rtag7_udf_trunk(): ** end **";

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

