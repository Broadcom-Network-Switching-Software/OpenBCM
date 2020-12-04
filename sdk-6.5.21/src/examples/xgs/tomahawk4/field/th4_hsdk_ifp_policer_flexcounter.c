/*
 * Feature  : IFP rule with a ingress vlan Policer.
 *
 * Usage    : BCM.0> c th4_hsdk_ifp_Policer_flexcounter.c
 *
 * config   : bcm56990_a0-generic-64x400.config.yml
 *
 * Log file : th4_hsdk_ifp_Policer_flexcounter_log.txt
 *
 * Test Topology :
 *                              CPU
 *                                ^
 *                                |  Rx reason : CPU_SFLOW *
 *                                |
 *                      +---------+---------+
 *                      |        TH4        |
 *   ingress port[0]    |                   |egress ports [1]
 *    in loopback       |   +-----------+   |in loopback
 *           ---------->+   | IFP Rule  |   +------>
 *      vlan  5         |   +-----------+   |
 *                      |      policer      |
 *                      |                   |
 *                      +-------------------+
 *
 *
 * Summary:
 * ========
 * TH4 supports only Ingress Policers in either IFP and exact match stages.
 * This cint script exercises an IFP Policer configured to shape vlan traffic with two
 * bit rates and three color marking which results in limiting vlan flood traffic
 * between member ports.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in testSetup()):
 *   =============================================
 *     a) Select two test ports and configure them in Loopback mode.
 *     b) Start packet watcher and register additional custom Rx callback for the
 *        packet verification.
 *     c) Enable ingress sampling on test port with rate to capture sufficient
 *        remarked red test packets.
 *
 *   2) Step2 - IFP Configuration (Done in ifpPolicerSetup()):
 *   =========================================================
 *     a) An IFP group is created with Outer vlan as qualifier
 *        in auto mode with several color specific actions.
 *     b) Create an IFP group entry to filter on outer VLAN 5 and shape the
 *        traffic with policer id 1, further the IFP policy will drop Green and
 *        Yellow packets to allow Red Packets with remarked outer priority of 4.
 *     c) In flexCounterSetup(), Add flex counters to the IFP entry solely for
 *        verification purpose and have IFP policy to increment these flex counters
 *        for each color. On TH4A0 devices, Please make sure IFP group will not
 *        auto expand to multiple TCAM slice else ifp flex counters will not
 *        work properly.
 *     d) install the IFP entry in hardware
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the outputs of 'fp show' 'show c'
 *     b) Expected Result:
 *     ===================
 *     The CPU packet capture shows the remarked sampled packets as below
 *
 *   Egress Packet:
 *   [bcmPWN.0]Packet[x]: Total x
 *   [bcmPWN.0]Packet[x]: data[0000]: {000000000202} {000000000001} 8100 8005
 *
 *   [RX metadata information]
 *   ...
 *   [RX reasons]
 *   CPU_SFLOW
 */

/* Reset C interpreter*/
cint_reset();

int num_ports = 2;
int port_list[num_ports];
bcm_vlan_t ingress_vlan = 5;
uint32 stat_counter_id;
uint32 counter_offset[3] = {0, 1, 2};
bcm_field_entry_t entry;
bcm_field_group_config_t group_config;
bcm_policer_config_t policer_config;
bcm_policer_t pid = 1; /* Non-zero policer Id */
bcm_field_flexctr_config_t flexctr_cfg;
int test_failed = 0; /* Final result will be stored here */
int rx_count = 0; /* Total test packet received by CPU for verification */

/* Expected remarked packet L2 header */
unsigned char expected_packet[16] = {
    0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x81, 0x00, 0x80, 0x05 };

/* Policer IFP rule */
bcm_error_t
ifpPolicerSetup(int unit) {

    int rv = 0;
    /* Choose non-overlapping bit rates for test validation */
    const int   base_rate = 10;
    const int   cir = base_rate * 8;
    const int   cbs = (base_rate + 12) * 8;
    const int   pir = (base_rate + 36) * 8;
    const int   pbs = (base_rate + 40) * 8;

    bcm_policer_config_t_init(&policer_config);
    policer_config.mode = bcmPolicerModeTrTcm;
    /* Two rate, three color policer ignoring previous color marking */
    policer_config.flags |= (BCM_POLICER_WITH_ID | BCM_POLICER_COLOR_BLIND);
    /* Committed Rate, below this rate, packets are green */
    policer_config.ckbits_sec = cir;
    policer_config.ckbits_burst = cbs;
    /* Peak Rate, below rate is yellow, above rate is red */
    policer_config.pkbits_sec = pir;
    policer_config.pkbits_burst = pbs;
    rv = bcm_policer_create(unit, &policer_config, &pid);
    if (BCM_FAILURE(rv)) {
        printf("bcm_policer_create() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    bcm_field_group_config_t_init(group_config);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyOuterVlanId);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionGpStatGroup);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionYpStatGroup);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionRpStatGroup);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionGpDrop);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionYpDrop);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionRpPrioPktNew);

    group_config.mode = bcmFieldGroupModeAuto;
    group_config.flags |= (BCM_FIELD_GROUP_CREATE_WITH_MODE |
                           BCM_FIELD_GROUP_CREATE_WITH_ASET);
    group_config.priority = 1;
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

    /* Qualify packets based on ingress outer vlan. */
    rv = bcm_field_qualify_OuterVlanId(unit, entry, ingress_vlan, 0xFFF);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_qualify_*() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Attach policer to the group entry. */
    rv = bcm_field_entry_policer_attach(unit, entry, 0, pid);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_policer_attach() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create flex counter action and attach it to group entry. */
    rv = flexCounterSetup(unit, entry);
    if (BCM_FAILURE(rv)) {
        printf("flexCounterSetup() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Drop Green and Yellow marked packets */
    rv = bcm_field_action_add(unit, entry, bcmFieldActionGpDrop, 0, 0);
    rv |= bcm_field_action_add(unit, entry, bcmFieldActionYpDrop, 0, 0);
    /* Mark red packets with new outer packet priority */
    rv |= bcm_field_action_add(unit, entry, bcmFieldActionRpPrioPktNew, 4, 0xFF);
    /* Count Red/Yellow/Green packets using spefic flex counter indices */
    rv |= bcm_field_action_add(unit, entry, bcmFieldActionGpStatGroup, counter_offset[0], 0);
    rv |= bcm_field_action_add(unit, entry, bcmFieldActionYpStatGroup, counter_offset[1], 0);
    rv |= bcm_field_action_add(unit, entry, bcmFieldActionRpStatGroup, counter_offset[2], 0);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_action_add() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_entry_install(unit, entry);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_install() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;

}


bcm_error_t
flexCounterSetup(int unit, int eid)  {

    int options = 0;
    bcm_error_t  rv;
    bcm_flexctr_action_t action;
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    bcm_flexctr_value_operation_t *value_b_op = NULL;

    bcm_flexctr_action_t_init(&action);
    action.flags = 0;
    /* Pool ID passed as hint and IFP as source */
    action.hint = 1; /* pool id */
    action.hint_type = bcmFlexctrHintPool;
    action.source = bcmFlexctrSourceIfp;
    action.mode = bcmFlexctrCounterModeNormal;
    /* Allocate three counter one for each color */
    action.index_num = 0x3;
    action.drop_count_mode=bcmFlexctrDropCountModeAll;

    /* Counter index is IFP. */
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
    value_a_op->mask_size[0] = 15;
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

/* Configure port to vlan as tagged or untagged */
bcm_error_t
vlan_add_port(int unit, int vid, bcm_port_t t_port, bcm_port_t ut_port) {

    bcm_pbmp_t pbmp, upbmp;
    bcm_error_t rv = BCM_E_NONE;

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, t_port);
    BCM_PBMP_PORT_ADD(upbmp, ut_port);
    rv = bcm_vlan_port_add(unit, vid, pbmp, upbmp);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_vlan_port_add: %s\n", bcm_errmsg(rv));
        return rv;
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

/* L2 and port configuration function */
bcm_error_t
l2Setup(int unit, bcm_vlan_t ing_vlan) {

    bcm_error_t rv = BCM_E_NONE;
    int index = 0;

    /* Create the ingress vlan */
    rv = bcm_vlan_create(unit, ing_vlan);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_vlan_create: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Add both test ports to ingress vlan to cause vlan flooding */
    rv = vlan_add_port(unit, ing_vlan, port_list[0], 0);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_add_port: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = vlan_add_port(unit, ing_vlan, port_list[1], 0);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_add_port: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Learn mode port config for ingress port port_list[0] */
    rv = port_learn_mode_set(unit, port_list[0], BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD);
    if (BCM_FAILURE(rv)) {
        printf("\nError in port_learn_mode_set: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Enable port_list[1] sampler to verify egressing Red packets with priority 4 */
    rv = bcm_port_sample_rate_set (unit, port_list[1], 0xFFF, 0);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_port_sample_rate_set: %s\n", bcm_errmsg(rv));
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

    /* Get packet data */
    if (BCM_FAILURE
       (bcm_pktio_pkt_data_get(unit, packet, (void *) &buffer, &length))) {
       return BCM_PKTIO_RX_NOT_HANDLED;
    }

    if (0 == sal_memcmp(buffer, expected_packet, 16)) {
        result_pkt_check = 0;
    }

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

    bcm_error_t rv;

    if (BCM_E_NONE != portNumbersGet(unit, port_list, num_ports)) {
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

    rv = l2Setup(unit, ingress_vlan);
    if (BCM_FAILURE(rv)) {
        printf("l2Setup() failed. %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Setup a IFP rule with Flex Counters verfication */
    rv = ifpPolicerSetup(unit);
    if (BCM_FAILURE(rv)) {
        printf("ifpPolicerSetup() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Verify function call */
bcm_error_t
testVerify(int unit) {

    char str[512];
    int rv;
    uint32 num_entries = 3;
    bcm_flexctr_counter_value_t counter_value[num_entries];

    bshell(unit, "fp show");
    bshell(unit, "sleep quiet 1");
    printf("\nSending tagged test packets to ingress port:%d\n", port_list[0]);
    /* Send 100 test packet burst with outer vlan tag 5 and packet size 512 bytes */
    snprintf(str, 512, "tx 100 pbm=%d DM=0x000000000202 SM=0x000000000001 vlan=%d length=512", port_list[0], ingress_vlan);
    bshell(unit, str);

    bshell(unit, "sleep quiet 3");
    bshell(unit, "show c");

    printf("\n------------------------------------- \n");
    printf("    RESULT OF RX PACKET VERIFICATION  ");
    printf("\n------------------------------------- \n");
    printf("Test = [%s]",((test_failed == 0 ) && (0 != rx_count))?"PASS":"FAIL");
    printf("\n------------------------------------- \n");
    /* test_failed is asserted by pktioRxCallback().
       sleep command in needed to give pktioRxCallback() chance to run. */
    if (test_failed == 1 || 0 == rx_count) {

       return BCM_E_FAIL;
    }

    /* Get flex counter value. */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    rv = bcm_flexctr_stat_get(unit,
                stat_counter_id,
                num_entries,
                &counter_offset,
                &counter_value);
    if (BCM_FAILURE(rv)) {
      printf("bcm_flexctr_stat_get %s\n", bcm_errmsg(rv));
      return rv;
    }

    /* print counter value. */
    printf("FlexCtr Get Green : %d packets / %d bytes\n", COMPILER_64_LO(counter_value[0].value[0]), COMPILER_64_LO(counter_value[0].value[1]));
    printf("FlexCtr Get Yellow : %d packets / %d bytes\n", COMPILER_64_LO(counter_value[1].value[0]), COMPILER_64_LO(counter_value[1].value[1]));
    printf("FlexCtr Get Red : %d packets / %d bytes\n", COMPILER_64_LO(counter_value[2].value[0]), COMPILER_64_LO(counter_value[2].value[1]));

    if (COMPILER_64_LO(counter_value[0].value[0]) == 0 ||
        COMPILER_64_LO(counter_value[1].value[0]) == 0 ||
        COMPILER_64_LO(counter_value[2].value[0]) == 0) {
        printf("Packet verify failed. Expected non-zero color packet stats \n");
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

    /* Detach the Policer of the FP entry */
    rv = bcm_field_entry_policer_detach(unit, eid, 0);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_policer_detach() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Destroy the Policer entry */
    rv = bcm_policer_destroy(unit, pid);
    if (BCM_FAILURE(rv)) {
        printf("bcm_policer_destroy() FAILED: %s\n", bcm_errmsg(rv));
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

    return BCM_E_NONE;
}


/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in ifpPolicerSetup())
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
