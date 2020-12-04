/*
 * Feature  : Range checkers group with IFP rule in default global mode
 *
 * Usage    : BCM.0> c th4_hsdk_ifp_rangegroup.c
 *
 * config   : bcm56990_a0-generic-64x400.config.yml
 *
 * Log file : th4_hsdk_ifp_rangegroup_log.txt
 *
 * Test Topology :
 *                              CPU
 *                                ^
 *                                |  Rx reason : CPU_FPP
 *                                |
 *                      +---------+---------+
 *                      |        TH4        |
 *    ingress port[0]   |                   |
 *       in loopback    |   +-----------+   | egress port [1]
 *           ---------->+   | Range IFP |   +-------->
 *     vlan x           |   +-----------+   |
 *                      |    pkt +L4src     |
 *                      |  length           |
 *                      +-------------------+
 *
 *
 * Summary:
 * ========
 * TH4 supports range check group to combine different range checkers into a single
 * group to reduces number of rules and improves rule density. This cint script
 * demonstrates L4 src port and packet length range checkers group in IFP.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in testSetup()):
 *   =============================================
 *     a) Select two random test ports and configure them in Loopback mode.
 *     b) Start packet watcher and register additional custom Rx callback for the
 *        packet verfication.
 *
 *   2) Step2 - IFP Configuration (Done in ifpRangeSetup()):
 *   =========================================================
 *     a) Range checker with ID 1 is setup for L4 source port
 *     b) Range checker with ID 2 is setup for packet length, add both the
 *        range checkers in range checker group's bitmap.
 *     c) An IFP group is created with outer vlan, Range check group as the qualifiers
 *     d) Create an IFP entry to filter on packet length [64-80] bytes and
 *        L4 src port range [5000-6000] with COPY_TO_CPU action.
 *     e) Add flex counters to the IFP entry solely for verfication purpose
 *     d) install the IFP entry in hardware
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the outputs of 'fp show' 'show c'
 *     b) IFP-Range ineligible packets are dropped on the ingress test port and are counted
 *        by RDBGC0
 *     c) Expected Result:
 *     ===================
 *     The CPU packet capture shows the ingress packet copied by IFP rule as below
 *     which has packet length within range of 64 to 80 bytes also L4 src port in
 *     5000 to 6000 range as configured in IFP
 *
 *   [bcmPWN.0]Packet[1]: Total x
 *   [bcmPWN.0]Packet[1]: data[0000]: {000000000202} {000000000001} 8100 0005
 *   [bcmPWN.0]Packet[1]: data[0010]: 0800 4500 002e 0000 0000 4006 a4f0 0a0a
 *   [bcmPWN.0]Packet[1]: data[0020]: 0a14 c0a8 0114 138c 0e69 0000 0000 0000
 *   [bcmPWN.0]Packet[1]: data[0030]: 0000 5000 0000 b206 0000 0001 0203 0405
 *   [bcmPWN.0]Packet[1]: data[0040]: 225e b0b6
 *   [bcmPWN.0]
 *   [bcmPWN.0][RX metadata information]
 *    ...
 *   [RX reasons]
 *   	CPU_FFP
 *   [bcmPWN.0]Packet[1]:
 *   Packet[1]:   Ethernet: dst<00:00:00:00:02:02> src<00:00:00:00:00:01> Tagged Packet ProtID<0x8100> Ctrl<0x0005> Internet Protocol (IP)
 *   Packet[1]:   ...
 *   Packet[1]:   TCP: sPort<5004> dPort<3689> Seq<0> Ack<0> Off<5> Flags<0x00=> Csum<0xb206> Urp<0x0000>
 */


/* Reset C interpreter*/
cint_reset();

bcm_port_t egress_port;
bcm_port_t ingress_port;
uint32 stat_counter_id;
bcm_vlan_t ingress_vlan = 5;
bcm_range_t range_id[] = {1, 2};
bcm_range_group_config_t range_group_config;
bcm_field_entry_t entry;
bcm_field_group_config_t group_config;
bcm_field_flexctr_config_t flexctr_cfg;
int test_failed = 0; /* Final result will be stored here */
int rx_count = 0; /* Total test packet received by CPU for verification */

/* Expected Egress packet */
unsigned char expected_packet[68] = {
0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x81, 0x00, 0x00, 0x05,
0x08, 0x00, 0x45, 0x00, 0x00, 0x2E, 0x00, 0x00, 0x00, 0x00, 0x40, 0x06, 0xA4, 0xF0, 0x0A, 0x0A,
0x0A, 0x14, 0xC0, 0xA8, 0x01, 0x14, 0x13, 0x8C, 0x0E, 0x69, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0xB2, 0x06, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
0x22, 0x5E, 0xB0, 0xB6};

/* RANGE checker group IFP rule */

bcm_error_t
ifpRangeSetup(int unit) {

    int rv = 0;
    bcm_port_config_t pc;
    bcm_range_oper_mode_t range_mode;
    bcm_field_group_oper_mode_t fp_mode;
    bcm_range_config_t range_config[2];

    /* This test requires range module in global mode */
    bcm_range_oper_mode_get(unit, range_mode);
    print range_mode;
    if ( bcmRangeOperModeGlobal != range_mode ) {
        printf("Invalid RangeOperMode for this test; please check YML config\n");
        return BCM_E_PARAM;
    }
    /* This test requires ifp module in global mode */
    bcm_field_group_oper_mode_get(unit, bcmFieldQualifyStageIngress, &fp_mode);
    print fp_mode;
    if ( bcmFieldGroupOperModeGlobal != fp_mode ) {
        printf("Invalid FieldGroupOperMode for this test; please check YML config\n");
        return BCM_E_PARAM;
    }

    bcm_port_config_t_init(&pc);
    bcm_port_config_get(unit, &pc);

    /* Create a range checker with L4 src port range 5000-6000 */
    bcm_range_config_t_init(&range_config[0]);
    range_config[0].rid = range_id[0];
    range_config[0].rtype = bcmRangeTypeL4SrcPort;
    range_config[0].min = 5000;
    range_config[0].max = 6000;
    range_config[0].offset = 0;
    range_config[0].width = 16;
    range_config[0].ports = pc.all;
    rv = bcm_range_create (unit, BCM_RANGE_CREATE_WITH_ID, &range_config[0]);
    if (BCM_FAILURE(rv)) {
        printf("range1:bcm_range_create() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create a range checker with packet length range 64 to 80 bytes */
    bcm_range_config_t_init(&range_config[1]);
    range_config[1].rid = range_id[1];
    range_config[1].rtype = bcmRangeTypePacketLength;
    range_config[1].min = 64;
    range_config[1].max = 80;
    range_config[1].offset = 0;
    range_config[1].width = 16;
    range_config[1].ports = pc.all;
    rv = bcm_range_create (unit, BCM_RANGE_CREATE_WITH_ID, &range_config[1]);
    if (BCM_FAILURE(rv)) {
        printf("range2:bcm_range_create() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    range_group_config.ports =  pc.all;
    BCM_RANGE_ID_BMP_ADD(range_group_config.range_bmp, range_config[0].rid);
    BCM_RANGE_ID_BMP_ADD(range_group_config.range_bmp, range_config[1].rid);
    rv = bcm_range_group_create (unit, &range_group_config);
    if (BCM_FAILURE(rv)) {
        printf("bcm_range_group_create() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyRangeCheckGroup);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyOuterVlanId);
    group_config.mode = bcmFieldGroupModeAuto;
    group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_MODE;

    rv = bcm_field_group_config_create(unit, &group_config);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_group_config_create() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_entry_create(unit, group_config.group, &entry);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_create() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_qualify_RangeCheckGroup(unit, entry, range_group_config.range_group_id , 0xF);
    rv |= bcm_field_qualify_OuterVlanId(unit, entry, ingress_vlan , 0xFFF);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_qualify_*() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Add COPY_TO_CPU action for the test packets hitting this rule */
    rv = bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_action_add() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Setup Flex Counters to verify IFP rule hits */
    rv = flexCounterSetup(unit, entry, group_config.group);
    if (BCM_FAILURE(rv)) {
        printf("flexCounterSetup() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Write IFP entry in hardware */
    rv = bcm_field_entry_install(unit, entry);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_install() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;

}


bcm_error_t
flexCounterSetup(int unit, int eid, int group)  {

    int options = 0;
    bcm_error_t  rv;
    bcm_flexctr_action_t action;
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    bcm_flexctr_value_operation_t *value_b_op = NULL;

    bcm_flexctr_action_t_init(&action);
    action.flags = 0;
    /* Group ID passed as hint */
    action.hint = group;
    action.hint_type = bcmFlexctrHintFieldGroupId;
    action.source = bcmFlexctrSourceIfp;
    action.mode = bcmFlexctrCounterModeNormal;
    action.index_num = 2;
    action.drop_count_mode=bcmFlexctrDropCountModeAll;

    index_op = &action.index_operation;
    index_op->object[0] = bcmFlexctrObjectStaticIngFieldStageIngress;
    index_op->object_id[0] = 0;
    index_op->mask_size[0] = 8;
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

    /* Create an ingress action */
    rv = bcm_flexctr_action_create(unit, options, &action, &stat_counter_id);
    if (BCM_FAILURE(rv)) {
       printf("bcm_flexctr_action_create() FAILED: %s\n", bcm_errmsg(rv));
       return rv;
    }
    printf("stat_counter_id == %d \r\n", stat_counter_id);

    flexctr_cfg.flexctr_action_id = stat_counter_id;
    flexctr_cfg.counter_index = 1;
    rv = bcm_field_entry_flexctr_attach(unit, eid, &flexctr_cfg);
    if (BCM_FAILURE(rv)) {
       printf("bcm_field_entry_flexctr_attach() FAILED: %s\n", bcm_errmsg(rv));
       return rv;
    }

    return BCM_E_NONE;
}

/*
 * This function is written so that hardcoding of port
 * numbers in Cint scripts is removed. This function gives
 * required number of ports
 */
bcm_error_t
portNumbersGet(int unit, int *port_list, int num_ports) {

    int i = 0, port = 0;
    bcm_error_t rv = 0;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;
    int no;

    rv = bcm_port_config_get(unit, &configP);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_port_config_get: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    ports_pbmp = configP.e;
    sal_srand(sal_time_usecs());
    no = sal_rand() % 250;
    for (i = no; i < BCM_PBMP_PORT_MAX; i++) {
        if (BCM_PBMP_MEMBER(&ports_pbmp, i) && (port < num_ports)) {
            printf("port %d is selected\n", i);
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
 * Configures the port in loopback mode
 */
bcm_error_t
port_lb_setup(int unit, bcm_pbmp_t pbm) {

    bcm_port_t     port;
    BCM_PBMP_ITER(pbm, port) {
        printf("port_lb_setup port: %d setup\n", port);
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
    }

    return BCM_E_NONE;
}

/* Setup learning on port */
bcm_error_t
port_learn_mode_set(int unit, bcm_port_t port, uint32 flag) {

    bcm_error_t rv = BCM_E_NONE;

    rv = bcm_port_learn_set(unit, port, flag);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_port_learn_set: %s\n", bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;
}

/* port configuration function */
bcm_error_t
portSetup(int unit) {

    bcm_error_t rv = BCM_E_NONE;

    /* Learn mode port config */
    rv = port_learn_mode_set(unit, ingress_port, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD);
    if (BCM_FAILURE(rv)) {
        printf("\nError in port_learn_mode_set: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Drop egress_port received packet to avoid looping back */
    rv = port_learn_mode_set(unit, egress_port, 0);
    if (BCM_FAILURE(rv)) {
        printf("\nError in port_learn_mode_set: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}


/* Rx callback function for applications using the HSDK PKTIO */
bcm_pktio_rx_t
pktioRxCallback(int unit, bcm_pktio_pkt_t * packet, void *cookie) {

    int  *count = (auto) cookie;
    int result_pkt_check = -1;
    void  *buffer;
    uint32  length;

    (*count)++; /* Bump received packet count */

    /* Get basic packet info */
    if (BCM_FAILURE
       (bcm_pktio_pkt_data_get(unit, packet, (void *) &buffer, &length))) {
       return BCM_PKTIO_RX_NOT_HANDLED;
    }
    result_pkt_check = sal_memcmp(buffer,expected_packet,length);

    test_failed = test_failed || (result_pkt_check != 0);

    return BCM_PKTIO_RX_NOT_HANDLED;
}

/* Register callback function for received packets */
bcm_error_t
registerPktioRxCallback(int unit) {

    const uint8         priority = 101;
    const uint32        flags = 0;      /* Flags only used on match (for now) */

    return bcm_pktio_rx_register
      (unit, "rx_cb", pktioRxCallback, priority, &rx_count, flags);
}

/*
 * Test Setup:
 * This functions gets the port numbers and sets up test configurations.
 */

bcm_error_t
testSetup(int unit) {

    int num_ports = 2;
    int port_list[num_ports];
    bcm_error_t         rv;

    if (BCM_E_NONE != portNumbersGet(unit, port_list, num_ports)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    ingress_port = port_list[0];
    egress_port = port_list[1];

    bcm_pbmp_t  pbmp;
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);
    BCM_PBMP_PORT_ADD(pbmp, egress_port);
    if (BCM_E_NONE != port_lb_setup(unit, pbmp)) {
        printf("port_lb_setup() failed \n");
        return -1;
    }

    bshell(unit, "pw start report all +raw +decode");

    rv = registerPktioRxCallback(unit);
    if (BCM_FAILURE(rv)) {
        printf("registerPktioRxCallback() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = portSetup(unit);
    if (BCM_FAILURE(rv)) {
        printf("portSetup() failed. %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Setup a IFP rule with range group qualifier with Flex Counters verfication */
    rv = ifpRangeSetup(unit);
    if (BCM_FAILURE(rv)) {
        printf("ifpRangeSetup() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*Verify function call*/
bcm_error_t
testVerify(int unit) {

    char str[512];
    int rv;
    bcm_flexctr_counter_value_t counter_value;
    uint32 num_entries = 1;
    uint32 counter_index = 1;
    sal_srand(sal_time_usecs());
    int count = sal_rand() % 5;
    if (count == 0) {
      count = 1;
    }

    bshell(unit, "fp show");
    bshell(unit, "sleep quiet 1");
    printf("\nSending %d tagged test packets to ingress port:%d\n", count, ingress_port);
    /* Send test packets of size 68 bytes and L4 src port 5004 */
    snprintf(str, 512, "tx %d pbm=%d data=0x0000000002020000000000018100000508004500002E000000004006A4F00A0A0A14C0A80114138C0E69000000000000000050000000B2060000000102030405225EB0B6", count, ingress_port);
    bshell(unit, str);
    bshell(unit, "sleep quiet 1");
    /* Send test packets of size 76 bytes and L4 src port 908 */
    snprintf(str, 512, "tx %d pbm=%d data=0x0000000002020000000000018100000508004500002E000000004006A4F00A0A0A14C0A80114038C0E69000000000000000050000000B20600000001020304050607080A0B000B00225EB0B6", count+1, ingress_port);
    bshell(unit, str);
    bshell(unit, "sleep quiet 3");
    bshell(unit, "show c");

    printf("\n------------------------------------- \n");
    printf("    RESULT OF RX PACKET VERIFICATION  ");
    printf("\n------------------------------------- \n");
    printf("Test = [%s]",((test_failed == 0 ) && (count == rx_count))?"PASS":"FAIL");
    printf("\n------------------------------------- \n");
    /* test_failed is asserted by pktioRxCallback().
       sleep command in needed to give pktioRxCallback() chance to run. */
    if (test_failed == 1) {

       return BCM_E_FAIL;
    }

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

    /* print counter value. */
    printf("FlexCtr Get : %d packets / %d bytes\n", COMPILER_64_LO(counter_value.value[0])
                                                , COMPILER_64_LO(counter_value.value[1]));
    if (COMPILER_64_LO(counter_value.value[0]) != count) {
      printf("Packet verify failed. Expected packet stat %d but get %d\n", count, COMPILER_64_LO(counter_value.value[0]));
      test_failed = 1;
    };

    printf("\n------------------------------------- \n");
    printf("    RESULT OF FLEX COUNTER VERIFICATION  ");
    printf("\n------------------------------------- \n");
    printf("Test = [%s]",(test_failed == 0)?"PASS":"FAIL");
    printf("\n------------------------------------- \n");

    if(test_failed ==0) {
       return BCM_E_NONE;
    }
    else {
       return BCM_E_FAIL;
    }
}


/*
 *   Cleanup test setup
 */
bcm_error_t testCleanup(int unit, int eid) {

    int rv;
    rv = bcm_field_entry_remove(unit, eid);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_remove() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_entry_flexctr_detach(unit, eid, &flexctr_cfg);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_flexctr_detach() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_entry_destroy(unit, eid);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_destroy() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_group_destroy(unit, group_config.group);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_group_destroy() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Destroy counter action. */
    rv = bcm_flexctr_action_destroy(unit, stat_counter_id);
    if (BCM_FAILURE(rv)) {
       printf("bcm_flexctr_action_destroy %s\n", bcm_errmsg(rv));
       return rv;
    }

    /* Delete range group */
    rv = bcm_range_group_delete(unit, &range_group_config);
    if (BCM_FAILURE(rv)) {
        printf("bcm_range_destroy() FAILED for range %d: %s\n", range_id[0], bcm_errmsg(rv));
        return rv;
    }

    /* Destroy the Range */
    rv = bcm_range_destroy(unit, range_id[0]);
    if (BCM_FAILURE(rv)) {
        printf("bcm_range_destroy() FAILED for range %d: %s\n", range_id[0], bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_range_destroy(unit, range_id[1]);
    if (BCM_FAILURE(rv)) {
        printf("bcm_range_destroy() FAILED for range %d: %s\n", range_id[1], bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}


/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in ifpRangeSetup())
 *  c) demonstrates the functionality(done in verify()).
 */
bcm_error_t
execute(void)
{
    bcm_error_t rv;
    int unit = 0;
    print "config show; attach; version";
    bshell(unit, "config show; attach ; version");

    rv = testSetup(unit);
    if (BCM_FAILURE(rv)) {
        printf("testSetup() failed %s \n", bcm_errmsg(rv));
        return rv;
    }
    printf("Completed test setup successfully.\n");

    rv = testVerify(unit);
    if (BCM_FAILURE(rv)) {
        printf("testVerify() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf("Completed test verify successfully.\n");

    rv = testCleanup(unit, entry);
    if (BCM_FAILURE(rv)) {
        printf("testCleanup() failed %s \n", bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
    print "execute(): Start";
    print execute();
    print "execute(): End";
}
