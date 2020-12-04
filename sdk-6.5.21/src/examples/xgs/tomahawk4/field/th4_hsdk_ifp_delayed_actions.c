/*
 * Feature  : IFP delayed action for Layer2 switched packets
 *
 * Usage    : BCM.0> c th4_hsdk_ifp_delayed_actions.c
 *
 * config   : bcm56990_a0-generic-64x400.config.yml
 *
 * Log file : th4_hsdk_ifp_delayed_actions_log.txt
 *
 * Test Topology :
 *                              CPU
 *                                ^
 *                                |
 *                                |
 *                      +---------+---------+
 *                      |        TH4        | egress port[1]
 *   ingress port[0]    |  +-----+  +-----+ + ------>
 *   in LoopBack        |  | L2  |  | IFP | |
 *           ---------->+  |entry|  | Rule| |
 *     ingress VFI      |  +-----+  +-----+ + ------>
 *                      |   delayed action  | egress port[3]
 *                      +-------------------+ with ing-sampler
 *
 *
 * Summary:
 * ========
 * cint script to demonstrate delayed IFP action for the Dest Mac ranges
 * of L2 switched packets. In Tomahawk4, Destination port resolution happens after
 * IFP stage, so no qualifier is available to match on DGLP in IFP stage,
 * however the delayed actions bcmFieldActionMatchPbmpDrop/Redirect are available
 * to override egress port resolution.
 *
 *
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in testSetup()):
 *   =============================================
 *     a) Selects four random ports and configure them in Loopback mode. Out of these
 *        four ports, one port[0] is used as ingress_port and the other as
 *        egress ports[1-3].
 *     b) Start packet watcher and register additional custom Rx callback for the
 *        packet verfication.
 *     c) In function l2Setup(), the test ports are configured in a ingress VLAN/VFI and
 *        L2 static entries are setup for unicast forwarding to egress port [1] with
 *        known VLAN/VFI value.
 *     d) Enable ingress sampling on the egress port[3] with maximum rate with CPU as
 *        destination
 *
 *   2) Step2 - IFP Configuration (Done in ifpDelayedActionSetup()):
 *   =========================================================
 *     a) A IFP group is created with InPort, DMAC as qualifiers and delayed action
 *        bcmFieldActionMatchPbmpDrop/Redirect as IFP actions
 *     b) Create an IFP group entry_id[0] to filter on ingress port[0], DMAC aa:bb:bb:[00-ff]
 *        and to match on dest port[1] to drop the egress port[1] destined L2 packets.
 *     c) Create another IFP group entry_id[1] to filter on ingress port[0], DMAC aa:bb:[00-ff]:bb
 *        and to match on dest port[1], port[2] bitmap to redirect port[1], port[2] destined L2
 *        packets to port[3].
 *     d) In flexCounterSetup(), Add flex counters to the IFP entries solely for
 *        verfication purpose
 *     e) install the IFP entries in hardware
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the outputs of 'fp show' 'show c' 'l2 show'
 *     b) on the egress port[3] SFLOW counters are incremented and static l2 entry is HIT
 *     c) Expected Result:
 *     ===================
 *     The CPU packet capture shows the egress packets copied by the port[3] sampler and
 *     entry_id[0] bcmFieldActionCopyToCpu IFP action have DMAC falling in either
 *     00:00:aa:bb:bb:[00-ff] or 00:00:aa:bb:[00-ff]:bb Dest MAC ranges.
 */

/* Reset C interpreter*/
cint_reset();

int num_ports = 4;
int port_list[num_ports];
bcm_vlan_t ingress_vlan = 2;
bcm_field_entry_t entry_id[2];
bcm_field_group_config_t group_config;
bcm_field_flexctr_config_t flexctr_cfg;
uint32 stat_counter_id;
bcm_pbmp_t redirect_pbm;
bcm_mac_t mac1 = {0x00,0x00,0xaa,0xbb,0xbb,0x01};
bcm_mac_t mask1 = {0xff,0xff,0xff,0xff,0xff,0x00};
bcm_mac_t mac2 = {0x00,0x00,0xaa,0xbb,0x01,0xbb};
bcm_mac_t mask2 = {0xff,0xff,0xff,0xff,0x00,0xff};
int test_failed = 0; /* Final result will be stored here */
int rx_count = 0; /* Total test packet received by CPU for verification */

/* Delayed action IFP rules */
bcm_error_t
ifpDelayedActionSetup(int unit) {

    int rv = 0;
    bcm_field_action_params_t param1, param2;
    bcm_field_action_match_config_t match1, match2;

    /* IFP group configuration with PbmpDrop/Redirect action */
    bcm_field_group_config_t_init(group_config);
    BCM_FIELD_QSET_ADD(group_config.qset,bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config.qset,bcmFieldQualifyDstMac);
    BCM_FIELD_QSET_ADD(group_config.qset,bcmFieldQualifyInPort);
    BCM_FIELD_ASET_ADD(group_config.aset,bcmFieldActionMatchPbmpDrop);
    BCM_FIELD_ASET_ADD(group_config.aset,bcmFieldActionMatchPbmpRedirect);
    BCM_FIELD_ASET_ADD(group_config.aset,bcmFieldActionCopyToCpu);
    group_config.flags= BCM_FIELD_GROUP_CREATE_WITH_MODE;
    group_config.priority=2;
    group_config.mode= bcmFieldGroupModeAuto;
    rv = bcm_field_group_config_create(unit, &group_config);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_group_config_create() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_entry_create(unit, group_config.group, &entry_id[0]);
    if (BCM_FAILURE(rv)) {
        printf("entry_id[0]:bcm_field_entry_create() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_qualify_InPort(unit, entry_id[0], port_list[0], 0xFFF);
    rv |= bcm_field_qualify_DstMac(unit, entry_id[0], mac1, mask1);
    if (BCM_FAILURE(rv)) {
        printf("entry_id[0]:bcm_field_qualify_*() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Drop port[1] destined L2 packets */
    BCM_PBMP_PORT_ADD(redirect_pbm, port_list[1]);
    match1.egr_pbmp = redirect_pbm;
    rv = bcm_field_action_config_info_add(unit, entry_id[0], bcmFieldActionMatchPbmpDrop, param1, match1);
    if (BCM_FAILURE(rv)) {
        printf("entry_id[0]:bcm_field_action_config_info_add() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_action_add(unit, entry_id[0], bcmFieldActionCopyToCpu, 0,0);
    if (BCM_FAILURE(rv)) {
        printf("entry_id[0]:bcm_field_action_add() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Setup Flex Counter action to verify IFP rule hits */
    rv = flexCounterSetup(unit, group_config.group, &stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("flexCounterSetup() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Attach index 1 flex counter to fp entry_id[0] */
    flexctr_cfg.flexctr_action_id = stat_counter_id;
    flexctr_cfg.counter_index = 1;
    rv =  bcm_field_entry_flexctr_attach(unit, entry_id[0], &flexctr_cfg);
    if (BCM_FAILURE(rv)) {
        printf("entry_id[0]:bcm_field_entry_flexctr_attach() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Set entry priority */
    rv = bcm_field_entry_prio_set(unit, entry_id[0], 2);
    if (BCM_FAILURE(rv)) {
        printf("entry_id[0]:bcm_field_entry_prio_set() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Write IFP entry_id[0] in hardware */
    rv = bcm_field_entry_install(unit, entry_id[0]);
    if (BCM_FAILURE(rv)) {
        printf("entry_id[0]:bcm_field_entry_install() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_PBMP_CLEAR(redirect_pbm);
    rv = bcm_field_entry_create(unit, group_config.group, &entry_id[1]);
    if (BCM_FAILURE(rv)) {
        printf("entry_id[1]:bcm_field_entry_create() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_qualify_InPort(unit, entry_id[1], port_list[0], 0xFFF);
    rv |= bcm_field_qualify_DstMac(unit, entry_id[1], mac2, mask2);
    if (BCM_FAILURE(rv)) {
        printf("entry_id[1]:bcm_field_qualify_*() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* redirect port[1], port[2] destined L2 packets to port[3]*/
    BCM_PBMP_PORT_ADD(redirect_pbm, port_list[1]);
    BCM_PBMP_PORT_ADD(redirect_pbm, port_list[2]);
    match2.egr_pbmp = redirect_pbm;
    param2.param0 = port_list[3];
    rv = bcm_field_action_config_info_add(unit, entry_id[1], bcmFieldActionMatchPbmpRedirect, param2, match2);
    if (BCM_FAILURE(rv)) {
        printf("entry_id[1]:bcm_field_action_config_info_add() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    flexctr_cfg.flexctr_action_id = stat_counter_id;
    flexctr_cfg.counter_index = 2;
    rv = bcm_field_entry_flexctr_attach(unit, entry_id[1], &flexctr_cfg);
    if (BCM_FAILURE(rv)) {
        printf("entry_id[1]:bcm_field_entry_flexctr_attach() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Set entry priority */
    rv = bcm_field_entry_prio_set(unit, entry_id[1], 20);
    if (BCM_FAILURE(rv)) {
        printf("entry_id[1]:bcm_field_entry_prio_set() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Write IFP entry_id[1] in hardware */
    rv = bcm_field_entry_install(unit, entry_id[1]);
    if (BCM_FAILURE(rv)) {
        printf("entry_id[1]:bcm_field_entry_install() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;

}

/* create IFP flex counter action with 32 counters */
bcm_error_t
flexCounterSetup(int unit, int group, uint32 *stat_id)  {

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
    action.index_num = 32;
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
    rv = bcm_flexctr_action_create(unit, options, &action, stat_id);
    if (BCM_FAILURE(rv)) {
       printf("bcm_flexctr_action_create() FAILED: %s\n", bcm_errmsg(rv));
       return rv;
    }
    printf("stat_counter_id == %d \r\n", *stat_id);

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

/* Static L2 entry for forwarding */
bcm_error_t
add_l2_static_entry(int unit,uint32 flag,bcm_mac_t mac,bcm_vpn_t vpn_id,bcm_port_t port) {

    bcm_l2_addr_t l2_addr;
    bcm_l2_addr_t_init(&l2_addr, mac, vpn_id);
    l2_addr.flags = BCM_L2_STATIC | flag;
    l2_addr.port = port;
    return bcm_l2_addr_add(unit, &l2_addr);
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

/* L2 switch configuration function */
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

    /* Add test ports port_list[0-3] to ingress vlan */
    for (index = 0; index < num_ports; index++) {
        rv |= vlan_add_port(unit, ing_vlan, port_list[index], 0);
        if (BCM_FAILURE(rv)) {
            printf("\nError in vlan_add_port: %s\n", bcm_errmsg(rv));
            return rv;
        }
    }

    /* Learn mode port config for ingress port port_list[0] */
    rv = port_learn_mode_set(unit, port_list[0], BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD);
    if (BCM_FAILURE(rv)) {
        printf("\nError in port_learn_mode_set: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Drop egress test ports port_list[1-3] received packet to avoid looping back */
    for (index = 1; index < num_ports; index++) {
        rv |= port_learn_mode_set(unit, port_list[index], 0);
        if (BCM_FAILURE(rv)) {
            printf("\nError in port_learn_mode_set: %s\n", bcm_errmsg(rv));
            return rv;
        }
    }

    /* Configure static l2 entries for forwarding */
    rv = add_l2_static_entry(unit,0, mac1, ing_vlan, port_list[1]);
    if (BCM_FAILURE(rv)) {
        printf("\nError in add_l2_static_entry: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv |= add_l2_static_entry(unit,0, mac2, ing_vlan, port_list[1]);
    if (BCM_FAILURE(rv)) {
        printf("\nError in add_l2_static_entry: %s\n", bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;
}

/* Rx callback function for applications using the HSDK PKTIO */
bcm_pktio_rx_t
pktioRxCallback(int unit, bcm_pktio_pkt_t * packet, void *cookie) {

    int  *count = (auto) cookie;
    int result_port_check = -1;
    int result_mac_check = -1;
    void  *buffer;
    uint32  length, src_port;

    (*count)++; /* Bump received packet count */

    /* Get source port metadata */
    BCM_IF_ERROR_RETURN(bcm_pktio_pmd_field_get
                        (unit, packet, bcmPktioPmdTypeRx,
                         BCM_PKTIO_RXPMD_SRC_PORT_NUM, &src_port));

    if(port_list[0] == src_port ||
       port_list[3] == src_port   ) {
        result_port_check = 0;
    }

    /* Get basic packet info */
    if (BCM_FAILURE
       (bcm_pktio_pkt_data_get(unit, packet, (void *) &buffer, &length))) {
       return BCM_PKTIO_RX_NOT_HANDLED;
    }

    if ((0 == sal_memcmp(buffer, mac1, 6)) ||
        (0 == sal_memcmp(buffer, mac2, 6))   ) {
        result_mac_check = 0;
    }

    test_failed = test_failed || (result_port_check != 0) || (result_mac_check != 0);

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
    int index = 0;

    if (BCM_E_NONE != portNumbersGet(unit, port_list, num_ports)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    bcm_pbmp_t  pbmp;
    BCM_PBMP_CLEAR(pbmp);
    for (index = 0; index < num_ports; index++) {
        BCM_PBMP_PORT_ADD(pbmp, port_list[index]);
    }
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

    /* Ingress sampling to CPU with maximum rate */
    rv = bcm_port_sample_rate_set (unit, port_list[3], 1, 0);
    if (BCM_FAILURE(rv)) {
        printf("bcm_port_sample_rate_set() failed. %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Setup two IFP rules with Flex Counters verfication */
    rv = ifpDelayedActionSetup(unit);
    if (BCM_FAILURE(rv)) {
        printf("ifpDelayedActionSetup() FAILED: %s\n", bcm_errmsg(rv));
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
    int count = 1;

    bshell(unit, "fp show");
    bshell(unit, "sleep quiet 1");
    printf("\nSending %d tagged test packets to ingress port:%d\n", (2*count)+1, port_list[0]);
    /* Send test packets with dest MAC 00:00:AA:BB:BB:01 and VLAN id 2 */
    snprintf(str, 512, "tx %d pbm=%d data=0x0000AABBBB010000000000018100000208004500002E000000004006A4F00A0A0A14C0A80114138C0569000000000000000050000000B2060000000102030405225EB0B6", count, port_list[0]);
    bshell(unit, str);
    bshell(unit, "sleep quiet 1");
    /* Send test packets with dest MAC 00:00:AA:BB:01:BB and VLAN id 2 */
    snprintf(str, 512, "tx %d pbm=%d data=0x0000AABB01BB0000000000028100000208004500002E000000004006A4F00A0A0A14C0A80114138C0569000000000000000050000000B2060000000102030405225EB0B6", count+1, port_list[0]);
    bshell(unit, str);
    bshell(unit, "sleep quiet 3");
    bshell(unit, "l2 show");
    bshell(unit, "show c");


    printf("\n------------------------------------- \n");
    printf("    RESULT OF RX PACKET VERIFICATION  ");
    printf("\n------------------------------------- \n");
    printf("Test = [%s]",((test_failed == 0 ) && ((2*count)+1 == rx_count))?"PASS":"FAIL");
    printf("\n------------------------------------- \n");
    /* test_failed is asserted by pktioRxCallback().
       sleep command in needed to give pktioRxCallback() chance to run. */
    if (test_failed == 1) {

       return BCM_E_FAIL;
    }

    /* Get flex counter value of entry_id[0] */
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
    printf("FlexCtr Get entry_id[0]-%d : %d packets / %d bytes\n", entry_id[0], COMPILER_64_LO(counter_value.value[0])
                                                , COMPILER_64_LO(counter_value.value[1]));
    if (COMPILER_64_LO(counter_value.value[0]) != count) {
      printf("Packet verify failed. Expected packet stat %d but get %d\n", e0_count, COMPILER_64_LO(counter_value.value[0]));
      test_failed = 1;
    };

    counter_index = 2;
    /* Get flex counter value of entry_id[1] */
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
    printf("FlexCtr Get entry_id[1]-%d : %d packets / %d bytes\n", entry_id[1], COMPILER_64_LO(counter_value.value[0])
                                                , COMPILER_64_LO(counter_value.value[1]));
    if (COMPILER_64_LO(counter_value.value[0]) != (count+1)) {
      printf("Packet verify failed. Expected packet stat %d but get %d\n", e1_count, COMPILER_64_LO(counter_value.value[0]));
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
bcm_error_t testCleanup(int unit, int * eid) {

    int rv;
    /* Remove FP entries from hardware */
    rv = bcm_field_entry_remove(unit, eid[0]);
    rv |= bcm_field_entry_remove(unit, eid[1]);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_remove() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Detach the flex counters of the FP entries */
    flexctr_cfg.flexctr_action_id = stat_counter_id;
    flexctr_cfg.counter_index = 1;
    rv = bcm_field_entry_flexctr_detach(unit, eid[0], &flexctr_cfg);
    flexctr_cfg.flexctr_action_id = stat_counter_id;
    flexctr_cfg.counter_index = 2;
    rv |= bcm_field_entry_flexctr_detach(unit, eid[1], &flexctr_cfg);
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

    /* Destroy the test FP entries */
    rv = bcm_field_entry_destroy(unit, eid[0]);
    rv |= bcm_field_entry_destroy(unit, eid[1]);
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
 *  b) actual configuration (Done in ifpDelayedActionSetup())
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

    rv = testCleanup(unit, entry_id);
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
