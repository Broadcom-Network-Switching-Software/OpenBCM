/*
 * Feature  : Range checkers on User Defined Field (UDF) using IFP rule
 *
 * Usage    : BCM.0> c th4_hsdk_ifp_udfrange.c
 *
 * config   : bcm56990_a0-generic-64x400.config.yml
 *
 * Log file : th4_hsdk_ifp_udfrange_log.txt
 *
 * Test Topology :
 *                              CPU
 *                                ^
 *                                |  Rx reason : CPU_FPP
 *                                |
 *                      +---------+---------+
 *                      |        TH4        |
 *   ingress port       |                   |
 *    in loopback       |   +-----------+   |
 *           ---------->+   |  IFP UDF  |   |
 *     unknown vfi xx   |   +-----------+   |
 *                      | xx.xx.01.[10-100] |
 *                      |                   |
 *                      +-------------------+
 *
 *
 * Summary:
 * ========
 * cint script to demonstrate range checker application on IFP UDF field for Destination
 * IP of L4 packets
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in testSetup()):
 *   =============================================
 *     a) Select a random ingress port and configure it in Loopback mode.
 *
 *     b) Start packet watcher and register additional custom Rx callback for the
 *        packet verfication.
 *
 *   2) Step2 - IFP Configuration (Done in udfRangeSetup()):
 *   =========================================================
 *     a) A 16 bit UDF is created with IPv4 ethertype and range checker hint
 *     b) Range checker is setup for the UDF with dest IP Range on fourth octet
 *     c) An IFP group is created with IP type, Range checker as qualifiers
 *     d) Create a IFP entry to filter on dest IP range xx.xx.01.[10-100] with
 *        COPY_TO_CPU CPU action
 *     e) Add flex counters to the IFP entry solely for verfication purpose
 *     d) install the IFP entry in hardware
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the outputs of 'fp show' 'show c'
 *     b) UDF-Range ineligible packets are dropped on the test port and are counted
 *        by RDBGC0
 *     c) Expected Result:
 *     ===================
 *     The CPU packet capture shows the ingress packet copied by IFP rule as below
 *
 *   Ingress Packet:
 *   [bcmPWN.0]
 *   [bcmPWN.0]Packet[1]: Total x
 *   [bcmPWN.0]Packet[1]: data[0000]: {000000000202} {000000000001} 8100 0005
 *   [bcmPWN.0]Packet[1]: data[0010]: 0800 4500 002e 0000 0000 4006 a4f0 0a0a
 *   [bcmPWN.0]Packet[1]: data[0020]: 0a14 c0a8 0114 138c 0569 0000 0000 0000
 *   [bcmPWN.0]Packet[1]: data[0030]: 0000 5000 0000 b206 0000 0001 0203 0405
 *   [bcmPWN.0]Packet[1]: data[0040]: 225e b0b6
 *   [bcmPWN.0]
 *   [bcmPWN.0][RX metadata information]
 *    ...
 *   [RX reasons]
 *   	CPU_FFP
 */

/* Reset C interpreter*/
cint_reset();

int num_ports = 1;
int port_list[num_ports];
uint32 dstip_range[] = {0x010A, 0x0164};
bcm_udf_t udf_info;
bcm_udf_id_t udf_id;
bcm_range_config_t range_config;
bcm_udf_pkt_format_id_t pkt_format_id;
uint32 stat_counter_id;
bcm_field_entry_t entry;
bcm_field_group_config_t group_config;
bcm_field_flexctr_config_t flexctr_cfg;
int test_failed = 0; /* Final result will be stored here */
int rx_count = 0; /* Total test packet received by CPU for verification */

/* UDF-RANGE checker IFP rule */
bcm_error_t
udfRangeSetup(int unit) {

    int rv = 0, length = 2;
    bcm_udf_alloc_hints_t hints;
    bcm_udf_pkt_format_info_t pkt_format;
    bcm_port_config_t pc;
    bcm_range_oper_mode_t range_mode;

    /* This test requires range module in global mode */
    bcm_range_oper_mode_get(unit, &range_mode);
    print range_mode;
    if ( bcmRangeOperModeGlobal != range_mode ) {
        printf("Invalid RangeOperMode for this test; please check YML config\n");
        return -1;
    }

    bcm_port_config_t_init(&pc);
    bcm_port_config_get(unit, &pc);

    /* Hints For UDF Create */
    bcm_udf_alloc_hints_t_init(&hints);
    hints.flags |= BCM_UDF_CREATE_O_RANGE_CHECK | BCM_UDF_CREATE_O_FIELD_INGRESS;

    /* UDF Offset Allocation */
    bcm_udf_t_init(&udf_info);
    udf_info.layer = bcmUdfLayerL3OuterHeader;
    udf_info.start = 144;
    udf_info.width = 16;
    rv = bcm_udf_create(unit, &hints, &udf_info, &udf_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_udf_create() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* UDF Packet Format with IPv4 ethertype */
    bcm_udf_pkt_format_info_t_init(&pkt_format);
    pkt_format.vlan_tag = BCM_PKT_FORMAT_VLAN_TAG_ANY;
    pkt_format.ethertype=0x0800;
    pkt_format.ethertype_mask=0xffff;
    pkt_format.prio = 2;
    rv = bcm_udf_pkt_format_create(unit, 0, &pkt_format, &pkt_format_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_udf_pkt_format_create() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_udf_pkt_format_add(unit, udf_id, pkt_format_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_udf_pkt_format_add() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create a range checker with dstIP range */
    bcm_range_config_t_init(&range_config);
    range_config.udf_id = udf_id;
    range_config.offset = 0;
    range_config.width = 16;
    range_config.rtype = bcmRangeTypeUdf;
    range_config.min = dstip_range[0];
    range_config.max = dstip_range[1];
    range_config.ports = pc.all;
    rv = bcm_range_create (unit, 0, &range_config);
    if (BCM_FAILURE(rv)) {
        printf("bcm_range_create() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Configure IFP group in Single wide mode with range checker
    as one of the qualifiers */
    int obj_list[] = {udf_id};
    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyRangeCheck);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyIpType);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    rv = bcm_field_qset_id_multi_set(unit, bcmFieldQualifyUdf, 1, &obj_list, &group_config.qset);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_qset_id_multi_set() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }
    group_config.mode = bcmFieldGroupModeSingle;
    group_config.priority = 2;
    group_config.ports = pc.all;
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

    /* Qualify IPv4 packets with range qualifier on destIP LSB */
    rv = bcm_field_qualify_RangeCheck(unit, entry, range_config.rid, 0);
    rv |= bcm_field_qualify_IpType(unit, entry, bcmFieldIpTypeIpv4NoOpts);
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

    /* Setup Flex Counters to verify UDF-RANGE rule hits */
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
    /* Group ID passed as hint and IFP as source */
    action.hint = group;
    action.hint_type = bcmFlexctrHintFieldGroupId;
    action.source = bcmFlexctrSourceIfp;
    action.mode = bcmFlexctrCounterModeNormal;
    action.index_num = 2;
    action.drop_count_mode=bcmFlexctrDropCountModeAll;

    /* Counter index is PKT_ATTR_OBJ0. */
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
    /* select random ports between 0-249 */
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

/* Setup learning on test port */
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
    rv = port_learn_mode_set(unit, port_list[num_ports-1], BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD);
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
    int result_port_check = -1;
    void  *buffer;
    uint32  length, src_port;

    (*count)++; /* Bump received packet count */

    /* Get source port metadata */
    BCM_IF_ERROR_RETURN(bcm_pktio_pmd_field_get
                        (unit, packet, bcmPktioPmdTypeRx,
                         BCM_PKTIO_RXPMD_SRC_PORT_NUM, &src_port));

    if(port_list[num_ports-1] == src_port) {
        result_port_check = 0;
    }

    test_failed = test_failed || (result_port_check != 0);

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

    bcm_error_t rv;

    if (BCM_E_NONE != portNumbersGet(unit, port_list, num_ports)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    bcm_pbmp_t  pbmp;
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port_list[num_ports-1]);
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

    /* Setup a IFP rule with Flex Counters verfication */
    rv = udfRangeSetup(unit);
    if (BCM_FAILURE(rv)) {
        printf("udfRangeSetup() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Verify function call */
bcm_error_t
testVerify(int unit) {

    char str[512];
    int rv;
    bcm_flexctr_counter_value_t counter_value;
    uint32 num_entries = 1;
    uint32 counter_index = 1;
    sal_srand(sal_time_usecs());
    /* select a random packet number from 0-10 */
    int count = sal_rand() % 10;
    if (count == 0) {
      count = 1;
    }

    bshell(unit, "fp show");
    bshell(unit, "sleep quiet 1");
    printf("\nSending %d tagged test packets to ingress port:%d\n", count, port_list[num_ports-1]);
    /* Send test packets with destIP C0:A8:01:14 falling within UDF range checker */
    snprintf(str, 512, "tx %d pbm=%d data=0x0000000002020000000000018100000508004500002E000000004006A4F00A0A0A14C0A80114138C0569000000000000000050000000B2060000000102030405225EB0B6", count, port_list[num_ports-1]);
    bshell(unit, str);
    bshell(unit, "sleep quiet 1");
    /* Send test packets with destIP C0:A8:01:65 out of the UDF range */
    snprintf(str, 512, "tx %d pbm=%d data=0x0000000002020000000000018100000508004500002E000000004006A4F00A0A0A14C0A80165138C0569000000000000000050000000B2060000000102030405225EB0B6", count+1, port_list[num_ports-1]);
    bshell(unit, str);
    bshell(unit, "sleep quiet 1");
    /* Send test packets with destIP C0:A8:02:14 UDF mismatch */
    snprintf(str, 512, "tx %d pbm=%d data=0x0000000002020000000000018100000508004500002E000000004006A4F00A0A0A14C0A80214138C0569000000000000000050000000B2060000000102030405225EB0B6", count+1, port_list[num_ports-1]);
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

    /* Get flex counter value. */
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
    if (COMPILER_64_LO(counter_value.value[0]) != rx_count) {
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
 *   Cleanup test setup sequentially
 */
bcm_error_t testCleanup(int unit, int eid) {

    int rv;
    /* Remove FP entry from hardware */
    rv = bcm_field_entry_remove(unit, eid);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_remove() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Detach the flex counters of the FP entry */
    rv = bcm_field_entry_flexctr_detach(unit, eid, &flexctr_cfg);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_flexctr_detach() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Destroy flex counter action */
    rv = bcm_flexctr_action_destroy(unit, stat_counter_id);
    if (BCM_FAILURE(rv)) {
       printf("bcm_flexctr_action_destroy %s\n", bcm_errmsg(rv));
       return rv;
    }

    /* Destroy the test FP entry */
    rv = bcm_field_entry_destroy(unit, eid);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_destroy() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Destroy the FP group */
    rv = bcm_field_group_destroy(unit, group_config.group);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_group_destroy() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Remove packet format from the UDF */
    rv = bcm_udf_pkt_format_delete(unit, udf_id, pkt_format_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_udf_pkt_format_delete() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Delete the packet format */
    rv = bcm_udf_pkt_format_destroy(unit, pkt_format_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_udf_pkt_format_destroy() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Destroy the UDF */
    rv = bcm_udf_destroy(unit, udf_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_udf_destroy() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Destroy the Range */
    rv = bcm_range_destroy(unit, range_config.rid);
    if (BCM_FAILURE(rv)) {
        printf("bcm_range_destroy() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}


/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in udfRangeSetup())
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
