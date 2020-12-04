/*
 * Feature  : Range checkers with IFP rule in per pipe/local mode
 *
 * Usage    : BCM.0> c th4_hsdk_ifp_range_pipelocal.c
 *
 * config   : th4_hsdk_ifp_range_pipelocal_config.yml
 *            with FP_ING_OPERMODE: PIPE_UNIQUE
 *            and FP_ING_RANGE_CHECK_OPERMODE_PIPEUNIQUE: 1
 *
 * Log file : th4_hsdk_ifp_range_pipelocal_log.txt
 *
 * Test Topology :
 *                              CPU
 *                                ^
 *                                |  Rx reason : CPU_FPP
 *   2 ingress ports              |
 *    in loopback       +---------+---------+
 *                      |        TH4        |
 *           ---------->+                   |
 *  pp_pipe(n) port     |   +-----------+   |
 *                      |   | Range IFP |   |
 *                      |   +-----------+   |
 *           ---------->+    OVLAN+L4dst    |
 *  pp_pipe(n+1) port   |                   |
 *                      +-------------------+
 *
 *
 * Summary:
 * ========
 * cint script to demonstrate perpipe range checkers application in IFP on L4 dst
 * port and outer VLAN/VFI id of IPv4 packets
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in testSetup()):
 *   =============================================
 *     a) Select two random packet processing pipe and corresponding ingress port
 *        and configure it in Loopback mode.
 *     b) Start packet watcher and register additional custom Rx callback for the
 *        packet verfication.
 *
 *   2) Step2 - IFP Configuration (Done in ifpRangeSetup()):
 *   =========================================================
 *     a) Range checker with ID 1 is setup for L4 destination port
 *     b) Range checker with ID 2 is setup for Outer VLAN ID
 *     c) An IFP group is created with IP type, Range checkers as the qualifiers
 *     d) Create a IFP entry to filter on outer VLAN range [0x10-0xBF] and
 *        inverse L4 dest port range [1000-2000] with COPY_TO_CPU CPU action
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
 *     which has outer VLAN id within range of 0x10 to 0xBf also L4 dst port out of
 *     1000 to 2000 range as configured in IFP
 *
 *   [bcmPWN.0]Packet[1]: Total 1
 *   [bcmPWN.0]Packet[1]: data[0000]: {000000000202} {000000000001} 8100 001a
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
 *   Packet[1]:   Ethernet: dst<00:00:00:00:02:02> src<00:00:00:00:00:01> Tagged Packet ProtID<0x8100> Ctrl<0x001a> Internet Protocol (IP)
 *   Packet[1]:   ...
 *   Packet[1]:   TCP: sPort<5004> dPort<3689> Seq<0> Ack<0> Off<5> Flags<0x00=> Csum<0xb206> Urp<0x0000>
 */

/* Reset C interpreter*/
cint_reset();

int num_ports = 2;
int port_list[num_ports];
int pipe_list[num_ports];
bcm_range_t range_id[] = {1, 2};
uint32 L4dport_range[] = {1000, 2000};
uint32 ovlan_range[] = {0x010, 0x0BF};
uint32 stat_counter_id;
bcm_field_entry_t entry;
bcm_field_group_config_t group_config;
bcm_field_flexctr_config_t flexctr_cfg;
int test_failed = 0; /* Final result will be stored here */
int rx_count = 0; /* Total test packet received by CPU for verification */

/* RANGE checker IFP rule */
bcm_error_t
ifpRangeSetup(int unit) {

    int rv = 0;
    int pp_pipe = pipe_list[0];
    bcm_range_config_t range_config[2];
    bcm_port_config_t pc;
    bcm_range_oper_mode_t range_mode;
    bcm_field_group_oper_mode_t fp_mode;

    /* This test requires range module in perpipe mode */
    bcm_range_oper_mode_get(unit, range_mode);
    print range_mode;
    if ( bcmRangeOperModePipeLocal != range_mode ) {
        printf("Invalid RangeOperMode for this test; please check YML config\n");
        return BCM_E_PARAM;
    }
    /* This test requires ifp module in pipe local/unique mode */
    bcm_field_group_oper_mode_get(unit, bcmFieldQualifyStageIngress, &fp_mode);
    print fp_mode;
    if ( bcmFieldGroupOperModePipeLocal != fp_mode ) {
        printf("Invalid FieldGroupOperMode for this test; please check YML config\n");
        return BCM_E_PARAM;
    }

    bcm_port_config_t_init(&pc);
    bcm_port_config_get(unit, &pc);

    /* Create a range checker with L4 dst port range */
    bcm_range_config_t_init(&range_config[0]);
    range_config[0].rid = range_id[0];
    range_config[0].rtype = bcmRangeTypeL4DstPort;
    range_config[0].min = L4dport_range[0];
    range_config[0].max = L4dport_range[1];
    range_config[0].offset = 0;
    range_config[0].width = 16;
    range_config[0].ports = pc.per_pp_pipe[pp_pipe];
    rv = bcm_range_create (unit, BCM_RANGE_CREATE_WITH_ID, &range_config[0]);
    if (BCM_FAILURE(rv)) {
        printf("range1:bcm_range_create() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create a range checker with outer vlanid range */
    bcm_range_config_t_init(&range_config[1]);
    range_config[1].rid = range_id[1];
    range_config[1].rtype = bcmRangeTypeOuterVlan;
    range_config[1].min = ovlan_range[0];
    range_config[1].max = ovlan_range[1];
    range_config[1].offset = 0;
    range_config[1].width = 8;
    range_config[1].ports = pc.per_pp_pipe[pp_pipe];
    rv = bcm_range_create (unit, BCM_RANGE_CREATE_WITH_ID, &range_config[1]);
    if (BCM_FAILURE(rv)) {
        printf("range2:bcm_range_create() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Configure IFP group in pipe local mode with range checker
       as one of the qualifiers */
    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyRangeCheck);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyIpType);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    group_config.mode = bcmFieldGroupModeAuto;
    group_config.ports = pc.per_pp_pipe[pp_pipe];
    group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_MODE |
                          BCM_FIELD_GROUP_CREATE_WITH_PORT ;

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
    /* L4 dst port range applied in inverse mode i.e. out of L4dport_range */
    rv = bcm_field_qualify_RangeCheck(unit, entry, range_config[0].rid, 1);
    rv |= bcm_field_qualify_RangeCheck(unit, entry, range_config[1].rid, 0);
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

    /* Setup Flex Counters to verify IFP-range rule hits */
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

/* Flex counter action with IFP source in global mode */
bcm_error_t
flexCounterSetup(int unit, int eid, int group)  {

    int options = 0;
    bcm_error_t  rv;
    bcm_flexctr_action_t action;
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    bcm_flexctr_value_operation_t *value_b_op = NULL;

    print bcm_flexctr_action_t_init(&action);
    action.flags = 0;
    /* Group ID passed as hint and IFP as source */
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
 * required number of ports from random pipe
 */
bcm_error_t
portNumbersGet(int unit, int *port_list, int *pipe_list ,int num_ports) {

    int i = 0, j = 0, port = 0;
    bcm_error_t rv = 0;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;
    int no;

    rv = bcm_port_config_get(unit, &configP);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_port_config_get: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    sal_srand(sal_time_usecs());
    no = sal_rand() % 4;

    for (i = 0; num_ports > i; i++) {
        ports_pbmp = configP.per_pp_pipe[no];
        for (j = 1; j < BCM_PBMP_PORT_MAX; j++) {
            if (BCM_PBMP_MEMBER(&ports_pbmp, j) && (port < num_ports)) {
                printf("pp_pipe%d:port%d is selected\n", no,j);
                port_list[i] = j;
                pipe_list[i] = no;
                port++;
                break;
            }
        }
        no++;
        no = no % 4;
    }

    if ((0 == port) || (port != num_ports)) {
        printf("portNumbersGet() failed \n");
        return -1;
    }

    return BCM_E_NONE;
}

/* Configures the port in loopback mode */
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

    /*Learn mode port config*/
    rv = port_learn_mode_set(unit, port_list[0], BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD);
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
    int result_ovid_check = -1;
    int result_port_check = -1;
    void  *buffer;
    uint32  length, ovid, src_port;

    (*count)++; /* Bump received packet count */

    /* Get outer vlan metadata */
    BCM_IF_ERROR_RETURN(bcm_pktio_pmd_field_get
                  (unit, packet, bcmPktioPmdTypeRx,
                   BCM_PKTIO_RXPMD_OUTER_VID, &ovid));

    if((ovlan_range[0] <= ovid) &&
        (ovid <= ovlan_range[1])) {
        result_ovid_check = 0;
    }

    /* Get source port metadata */
    BCM_IF_ERROR_RETURN(bcm_pktio_pmd_field_get
                        (unit, packet, bcmPktioPmdTypeRx,
                         BCM_PKTIO_RXPMD_SRC_PORT_NUM, &src_port));

    if(port_list[0] == src_port) {
        result_port_check = 0;
    }

    test_failed = test_failed || (result_ovid_check != 0) || (result_port_check != 0);

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

    if (BCM_E_NONE != portNumbersGet(unit, port_list, pipe_list, num_ports)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    bcm_pbmp_t  pbmp;
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port_list[0]);
    BCM_PBMP_PORT_ADD(pbmp, port_list[1]);
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

    /* Setup a range checker and IpType qualifier IFP rule with Flex Counters verfication */
    rv = ifpRangeSetup(unit);
    if (BCM_FAILURE(rv)) {
        printf("ifpPreselSetup() FAILED: %s\n", bcm_errmsg(rv));
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
    int i = 0;

    bshell(unit, "fp show");
    bshell(unit, "sleep quiet 1");
    printf("\nSending 4 tagged test packets to ingress ports %d & %d\n", port_list[0], port_list[1]);
    for (i;i < num_ports; i++) {
      /* Send test a packet with ovid 5 and L4 dst port 3689 */
      snprintf(str, 512, "tx 1 pbm=%d data=0x0000000002020000000000018100000508004500002E000000004006A4F00A0A0A14C0A80114138C0E69000000000000000050000000B2060000000102030405225EB0B6",  port_list[i]);
      bshell(unit, str);
      bshell(unit, "sleep quiet 1");
      /* Send test a packet with ovid 26 and L4 dst port 3689 */
      snprintf(str, 512, "tx 1 pbm=%d data=0x0000000002020000000000018100001A08004500002E000000004006A4F00A0A0A14C0A80114138C0E69000000000000000050000000B2060000000102030405225EB0B6",  port_list[i]);
      bshell(unit, str);
      bshell(unit, "sleep quiet 1");
      /* Send test a packet with ovid 26 and L4 dst port 1389 */
      snprintf(str, 512, "tx 1 pbm=%d data=0x0000000002020000000000018100001A08004500002E000000004006A4F00A0A0A14C0A80114138C0569000000000000000050000000B2060000000102030405225EB0B6",  port_list[i]);
      bshell(unit, str);
      bshell(unit, "sleep quiet 1");
      /* Send test a packet with ovid 255 and L4 dst port 3689 */
      snprintf(str, 512, "tx 1 pbm=%d data=0x000000000202000000000001810000FF08004500002E000000004006A4F00A0A0A14C0A80114138C0E69000000000000000050000000B2060000000102030405225EB0B6",  port_list[i]);
      bshell(unit, str);
      bshell(unit, "sleep quiet 1");
    }
    bshell(unit, "sleep quiet 3");
    bshell(unit, "show c");

    printf("\n------------------------------------- \n");
    printf("    RESULT OF RX PACKET VERIFICATION  ");
    printf("\n------------------------------------- \n");
    printf("Test = [%s]",(test_failed == 0 && 1 == rx_count) ?"PASS":"FAIL");
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

    /* Print counter value. */
    printf("FlexCtr Get : %d packets / %d bytes\n", COMPILER_64_LO(counter_value.value[0])
                                                , COMPILER_64_LO(counter_value.value[1]));
    if (COMPILER_64_LO(counter_value.value[0]) != rx_count) {
      printf("Packet verify failed. Expected packet stat %d but get %d\n", rx_count, COMPILER_64_LO(counter_value.value[0]));
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
