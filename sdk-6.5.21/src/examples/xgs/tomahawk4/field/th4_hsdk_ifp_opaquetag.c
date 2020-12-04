/*
 * Feature  : IFP rule with Opaque tag qualifier
 *
 * Usage    : BCM.0> c th4_hsdk_ifp_opaquetag.c
 *
 * config   : bcm56990_a0-generic-64x400.config.yml
 *
 * Log file : th4_hsdk_ifp_opaquetag_log.txt
 *
 * Test Topology :
 *                              CPU
 *                                ^
 *                                |  Rx reason : CPU_FFP
 *                                |
 *                      +---------+---------+
 *                      |        TH4        |
 *   ingress port[0]    |                   |egress ports [1]
 *    in loopback       |   +-----------+   |in loopback
 *           ---------->+   | IFP Rule  |   +------>
 *      vlan  5         |   +-----------+   |
 *                      |    OPAQUE tag     |
 *                      |                   |
 *                      +-------------------+
 *
 *
 * Summary:
 * ========
 * TH4 supports only single outer vlan tag but if a double tagged
 * packets enters the pipeline it treats everything beyond 1st outer tag
 * as data so opaque tag setting would be necessary to parse L3 field after
 * 2nd vlan tag. This cint exercises opaque tag settings and also qualify
 * packets on L3 fields for double tagged packets.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in testSetup()):
 *   =============================================
 *     a) Select two test ports and configure them in Loopback mode.
 *     b) Start packet watcher and register additional custom Rx callback for the
 *        packet verfication.
 *     c) Setup Opaque tag with ethertype 0x9100 in function opaqueTagSetup().
 *
 *   2) Step2 - IFP Configuration (Done in ifpOtagSetup()):
 *   =========================================================
 *     a) An IFP group is created with Outer vlan, DstIp and Opaque tag as qualifiers
 *        in auto mode with COPY_TO_CPU as action.
 *     b) Create an IFP group entry to filter on dest IP range 192.168.1.x of inner
 *        VLAN 10 or opaque tagged IPv4 packets and redirect qualified packets to CPU.
 *     c) In flexCounterSetup(), Add flex counters to the IFP entry solely for
 *        verfication purpose
 *     d) install the IFP entry in hardware
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the outputs of 'fp show' 'show c'
 *     b) Expected Result:
 *     ===================
 *     The CPU packet capture shows the ingress IPv6 packet copied by IFP rule as below
 *
 *   Egress Packet:
 *   [bcmPWN.0]Packet[x]: Total x
 *   [bcmPWN.0]Packet[x]: data[0000]: {000000000202} {000000000001} 8100 0005
 *   [bcmPWN.0]Packet[x]: data[0010]: 9100 000a 0800 4500 002e 0000 0000 4006
 *   [bcmPWN.0]Packet[x]: data[0020]: a4f0 0a0a 0a14 c0a8 0114 138c 0569 0000
 *   [bcmPWN.0]Packet[x]: data[0030]: 0000 0000 0000 5000 0000 b206 0000 0001
 *   [bcmPWN.0]Packet[x]: data[0040]: 0203 0405 225e b0b6
 *   [bcmPWN.0]
 *   [RX metadata information]
 *   ...
 *   [RX reasons]
 *   CPU_FFP
 */

/* Reset C interpreter*/
cint_reset();

int num_ports = 2;
int port_list[num_ports];
bcm_vlan_t ingress_vlan = 5, inner_vlan = 10;
bcm_ip_t route = 0xC0A80100;
bcm_ip_t route_mask = 0xffffff00;
uint32 stat_counter_id;
bcm_field_entry_t entry;
bcm_field_group_config_t group_config;
bcm_field_flexctr_config_t flexctr_cfg;
int test_failed = 0; /* Final result will be stored here */
int rx_count = 0; /* Total test packet received by CPU for verification */

/* Expected Egress packet L2 header */
unsigned char expected_packet[96] = {
    0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x01, 0x22, 0x22,
    0x81, 0x00, 0x00, 0x05, 0x91, 0x00, 0x00, 0x0A };

/* Opaque tag setup */
bcm_error_t
opaqueTagSetup(int unit) {

      int index = 1;
      int rv = 0;
      bcm_switch_opaque_tag_params_t opaque_tag_params;
      bcm_switch_opaque_tag_config_t_init(&opaque_tag_params);
      opaque_tag_params.ethertype = 0x9100; /* Opaque tag ethertype */
      opaque_tag_params.tag_size = BCM_SWITCH_OPAQUE_TAG_SIZE_4_BYTES;
      opaque_tag_params.tag_type = 0x1;
      opaque_tag_params.valid = 0x1;
      rv = bcm_switch_opaque_tag_config_set(unit, index, &opaque_tag_params);
      if (BCM_FAILURE(rv)) {
          printf("bcm_switch_opaque_tag_config_set() FAILED: %s\n", bcm_errmsg(rv));
          return rv;
      }

    return BCM_E_NONE;
}


/* Opaque tag IFP rule */
bcm_error_t
ifpOtagSetup(int unit) {

    int rv = 0;

    /* IFP group configuration with Outervlan, DST IPv4, lower opaque tag
       (vlan) as qualifiers in Auto mode */
    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyOuterVlanId);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyDstIp);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyVlanFormat);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyOpaqueTagLow);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    group_config.mode = bcmFieldGroupModeAuto;
    group_config.priority = 1;
    group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_MODE;

    rv = bcm_field_group_config_create(unit, &group_config);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_group_config_create() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create FP entry to match on Opaque tag attributes. */
    rv = bcm_field_entry_create(unit, group_config.group, &entry);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_create() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Filter INNER tagged that is opaque tagged packets. */
    uint8 vformat_data = BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED |
                         BCM_FIELD_VLAN_FORMAT_INNER_TAGGED;
    uint8 vformat_mask = BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED |
                         BCM_FIELD_VLAN_FORMAT_INNER_TAGGED;

    rv = bcm_field_qualify_VlanFormat(unit, entry, vformat_data, vformat_mask);
    /* Opaque tag configurations would allow to qualify on dest IP. */
    rv |= bcm_field_qualify_DstIp(unit, entry, route, route_mask);
    /* Qualify on INNER/OPAQUE vlan id .*/
    rv |= bcm_field_qualify_OpaqueTagLow(unit, entry, inner_vlan, 0xFFFF);
    rv |= bcm_field_qualify_OuterVlanId(unit, entry, ingress_vlan, 0xFFF);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_qualify_*() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_action_add() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Setup Flex Counters to verify IFP rule hits. */
    rv = flexCounterSetup(unit, entry, group_config.group);
    if (BCM_FAILURE(rv)) {
        printf("flexCounterSetup() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Commit the IFP entry. */
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

    /* Add ingress test port to ingress vlan */
    rv = vlan_add_port(unit, ing_vlan, port_list[0], 0);
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

    return BCM_E_NONE;
}

/* Rx callback function for applications using the HSDK PKTIO */
bcm_pktio_rx_t
pktioRxCallback(int unit, bcm_pktio_pkt_t * packet, void *cookie) {

    int  *count = (auto) cookie;
    int result_port_check = -1;
    int result_pkt_check = -1;
    void  *buffer;
    uint32  length, src_port;

    (*count)++; /* Bump received packet count */

    /* Get source port metadata */
    BCM_IF_ERROR_RETURN(bcm_pktio_pmd_field_get
                        (unit, packet, bcmPktioPmdTypeRx,
                         BCM_PKTIO_RXPMD_SRC_PORT_NUM, &src_port));

    if(port_list[0] == src_port) {
        result_port_check = 0;
    }

    /* Get basic packet info */
    if (BCM_FAILURE
       (bcm_pktio_pkt_data_get(unit, packet, (void *) &buffer, &length))) {
       return BCM_PKTIO_RX_NOT_HANDLED;
    }

    if (0 == sal_memcmp(buffer, expected_packet, 20)) {
        result_pkt_check = 0;
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

    rv = opaqueTagSetup(unit);
    if (BCM_FAILURE(rv)) {
        printf("opaqueTagSetup() failed. %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Setup a IFP rule with Flex Counters verfication */
    rv = ifpOtagSetup(unit);
    if (BCM_FAILURE(rv)) {
        printf("ifpOtagSetup() FAILED: %s\n", bcm_errmsg(rv));
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
    /* select a random packet number from 0-5 */
    sal_srand(sal_time_usecs());
    int count = sal_rand() % 5;
    if (count == 0) {
      count = 1;
    }

    bshell(unit, "fp show");
    bshell(unit, "sleep quiet 1");
    printf("\nSending %d tagged test packets to ingress port:%d\n", (2*count)+1, port_list[0]);

    /* Send test packets with opaque tag 0x9100000A */
    snprintf(str, 512, "tx %d pbm=%d data=0x000000000202000000000001810000059100000A08004500002E000000004006A4F00A0A0A14C0A80114138C0569000000000000000050000000B2060000000102030405225EB0B6", count, port_list[0]);
    bshell(unit, str);

    bshell(unit, "sleep 1");

    /* Send test packets without opaque tag */
    snprintf(str, 512, "tx %d pbm=%d data=0x0000000002020000000000018100000508004500002e12344000ff0693ca0a0a0a14c0a80105138c0569000000640000006450000fa0b0b60000aaaaaaaaaaaaaaaaaaaa", count+1, port_list[0]);
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

    return BCM_E_NONE;
}


/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in ifpOtagSetup())
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
