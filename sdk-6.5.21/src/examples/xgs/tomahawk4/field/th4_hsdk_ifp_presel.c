/*
 * Feature  : Preselector class assignment in ingress field processor
 *
 * Usage    : BCM.0> c th4_hsdk_ifp_presel.c
 *
 * config   : bcm56990_a0-generic-64x400.config.yml
 *
 * Log file : th4_hsdk_ifp_presel_log.txt
 *
 * Test Topology :
 *                              CPU
 *                                ^
 *                                |  Rx reason : CPU_FFP
 *                                |
 *                      +---------+---------+
 *                      |        TH4        |
 *   ingress port       |  +----+   +-----+ |
 *   in LoopBack        |  |PRE |   | IFP | |
 *           ---------->+  |SEL |   |RULE | |
 *     ingress VFI 5    |  +----+   +-----+ |
 *                      |      GroupClass   |
 *                      +-------------------+
 *
 *
 * Summary:
 * ========
 * cint script to demonstrate preselector GroupClass assigment in presel stage
 * further an IFP entry qualifying the GroupClass traffic
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in testSetup()):
 *   =============================================
 *     a) Selects two random test ports and configure them in Loopback mode.
 *     b) Start packet watcher and register additional custom Rx callback for the
 *        packet verfication.
 *
 *   2) Step2 - Presel+IFP Configuration (Done in ifpPreselSetup()):
 *   =========================================================
 *     a) An IFP stage presel is created with id 1 to identify IP packets dropped
 *        due to invalid VFI also add presel class assignment as action to the entry
 *     b) Create a IFP group entry to filter on GroupClass of the port with
 *        COPY_TO_CPU actions
 *     c) In flexCounterSetup(), Add flex counters to the IFP entry solely for
 *        verfication purpose
 *     d) install the IFP entry in hardware
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the outputs of 'fp show' 'show c'
 *     b) Expected Result:
 *     ===================
 *     The CPU packet capture shows the ingress packet copied by presel IFP rule as below
 *
 *   Ingress Packet:
 *   [bcmPWN.0]
 *   [bcmPWN.0]Packet[1]: Total x
 *   [bcmPWN.0]Packet[1]: data[0000]: {01005e010101} {000006000300} 8100 0005
 *   [bcmPWN.0]Packet[1]: data[0010]: 0800 4500 002e 0000 0000 40ff 91cd 0201
 *   [bcmPWN.0]Packet[1]: data[0020]: 0101 e401 0101 0001 0203 0405 0607 0809
 *   [bcmPWN.0]Packet[1]: data[0030]: 0a0b 0c0d 0e0f 1011 1213 1415 1617 1819
 *   [bcmPWN.0]Packet[1]: data[0040]: bf2e 44c3
 *   [bcmPWN.0]
 *   [bcmPWN.0][RX metadata information]
 *   	PKT_LENGTH=0x48
 *    ...
 *   [RX reasons]
 *   	CPU_FFP
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t egress_port;
bcm_port_t ingress_port;
bcm_gport_t gport;
uint32 stat_counter_id;
uint32 class = 3;
bcm_vlan_t ing_vlan = 5;
bcm_field_entry_t entry;
bcm_field_group_config_t group_config;
bcm_field_presel_t presel_id = 1;
bcm_field_presel_config_t presel_config;
bcm_field_flexctr_config_t flexctr_cfg;
int test_failed = 0; /* Final result will be stored here */
int rx_count = 0; /* Total test packet received by CPU for verification */

/* Expected Egress packet */
unsigned char expected_packet[68] = {
0x01, 0x00, 0x5e, 0x01, 0x01, 0x01, 0x00, 0x00, 0x06, 0x00, 0x03, 0x00, 0x81, 0x00, 0x00, 0x05,
0x08, 0x00, 0x45, 0x00, 0x00, 0x2e, 0x00, 0x00, 0x00, 0x00, 0x40, 0xff, 0x91, 0xcd, 0x02, 0x01,
0x01, 0x01, 0xe4, 0x01, 0x01, 0x01, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
0xbf, 0x2e, 0x44, 0xc3 };

bcm_error_t
ifpPreselSetup(int unit, int classid) {

    int rv = 0;

    bcm_field_qset_t presel_qual;
    bcm_field_presel_config_t_init(&presel_config);
    presel_config.stage = bcmFieldStageIngress;
    presel_config.flags = BCM_FIELD_PRESEL_CREATE_WITH_ID;
    presel_config.presel_id = presel_id;
    rv = bcm_field_presel_config_create(unit, &presel_config);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_presel_config_create() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }
    /* Preselector to identify IP packets dropped due to invalid VLAN/VFI */
    rv = bcm_field_qualify_Drop(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, 1, 1);
    rv |= bcm_field_qualify_ForwardingVlanValid(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, 0, 1);
    rv |= bcm_field_qualify_IpType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL , bcmFieldIpTypeIp);
    if (BCM_FAILURE(rv)) {
        printf("Presel:bcm_field_qualify_*() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Assign GroupClass to the identified presel flow */
    rv = bcm_field_action_add(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldActionGroupClassSet, class, 0);
    if (BCM_FAILURE(rv)) {
        printf("Presel:bcm_field_action_add() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* IFP group configuration with GroupClass as a qualifier */
    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyGroupClass);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    group_config.mode = bcmFieldGroupModeSingle;
    group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_PRESELSET |
                         BCM_FIELD_GROUP_CREATE_WITH_MODE;
    BCM_FIELD_PRESEL_ADD(group_config.preselset, presel_id);
    rv = bcm_field_group_config_create(unit, &group_config);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_group_config_create() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create FP entry to match on GroupClass assigned by preselector */
    rv = bcm_field_entry_create(unit, group_config.group, &entry);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_create() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_qualify_GroupClass(unit, entry, class, 0x3);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_qualify_*() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_action_add() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Setup Flex Counters to verify presel-IFP rule hits */
    rv = flexCounterSetup(unit, entry, group_config.group);
    if (BCM_FAILURE(rv)) {
        printf("flexCounterSetup() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Commit the IFP entry */
    rv = bcm_field_entry_install(unit, entry);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_install() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;

}

/* Create IFP flex counter action */
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

/*Setup learning on port*/
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

    /* Setup a IFP rule with presel and class qualifier with Flex Counters verfication */
    rv = ifpPreselSetup(unit, class);
    if (BCM_FAILURE(rv)) {
        printf("ifpPreselSetup() FAILED: %s\n", bcm_errmsg(rv));
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
    int count = sal_rand() % 10;
    if (count == 0) {
      count = 1;
    }

    bshell(unit, "fp show");
    bshell(unit, "fp show presel 1");
    bshell(unit, "sleep quiet 1");
    printf("\nSending %d tagged test packets to ingress port:%d\n", count, ingress_port);
    snprintf(str, 512, "tx %d pbm=%d data=0x01005E0101010000060003008100000508004500002E0000000040FF91CD02010101E4010101000102030405060708090A0B0C0D0E0F10111213141516171819BF2E44C3", count, ingress_port);
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

    /* Destroy counter action. */
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

    /* Destroy the presel entry */
    rv = bcm_field_presel_destroy(unit, presel_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_presel_destroy() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}


/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in ifpPreselSetup())
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
