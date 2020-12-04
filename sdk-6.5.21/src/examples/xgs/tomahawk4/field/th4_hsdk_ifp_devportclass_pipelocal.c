/*
 * Feature  :  IFP rule with DevicePort class as a qualifier in per pipe mode.
 *
 * Usage    : BCM.0> c th4_hsdk_ifp_devportclass_pipelocal.c
 *
 * config   : th4_hsdk_ifp_devportclass_pipelocal_config.yml
 *            with FP_ING_OPERMODE: PIPE_UNIQUE
 *
 * Log file : th4_hsdk_ifp_devportclass_pipelocal_log.txt
 *
 * Test Topology :
 *                              CPU
 *                                ^
 *     ingress ports              |  Rx reason : CPU_FPP
 *     in loopback                |
 *                      +---------+----------+
 *        port[0] ----> |         TH4        |   portgroup0 = port[0]+port[1]+port[2]
 *                      |                    |   portgroup1 = port[3]+port[1]+port[2]
 *        port[1] ----> |   +-----------+    |
 *                      |   | IFP Rules |    |
 *        port[2] ----> |   +-----------+    |
 *                      |   DevicePort class |
 *        port[3] ----> |                    |
 *                      +--------------------+
 *         VFI 5
 *
 * Summary:
 * ========
 * InPorts qualifier is not available in TH4 because of hardware limitation so
 * per port SystemPort or DevicePort class ids can be used as workaround qualifiers
 * this script demonstrates use of DevicePort class as a IFP qualifier in per pipe/
 * pipe local IFP mode.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in testSetup()):
 *   =============================================
 *     a) Select list of random ingress ports from a pp_pipe and configure them in Loopback mode.
 *     b) Assign DevicePort classids to the selected test ports.
 *     c) Start packet watcher and register additional custom Rx callback for the
 *        packet verification.
 *
 *   2) Step2 - IFP Configuration (Done in ifpClassSetup()):
 *   =========================================================
 *     a) An IFP group is created with Outer vlan and DevicePort class as qualifiers in pipe mode.
 *     b) Create a IFP entry[0] to filter on portgroup0 i.e. with class[0] and class[1] ports
 *        with drop action
 *     c) Add flex counters to the IFP entry[0] solely for verification purpose
 *     d) Create another IFP entry[1] to filter on portgroup1 i.e. with class[2] and class[1]
 *        ports with COPY_TO_CPU action
 *     e) Add flex counters to the IFP entry[1] solely for verification purpose
 *     f) install the IFP entries in hardware
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the outputs of 'fp show' 'show c'
 *     b) Change in the entry priority of the IFP entry[1] results in COPY_TO_CPU action
 *        for common class[1] ports
 *     c) Expected Result:
 *     ===================
 *     The CPU packet capture shows the ingress packet copied by entry[1] IFP rule as below
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

int num_ports = 4;
int port_list[num_ports];
int pipe_list[1];
int class[] = { 31,  /* port_list[0] class id of group0 only ports */
                32,  /* port_list[1-2]  class id of overlapping ports
                        of group1 & group0 */
                33   /* port_list[3] class id of group1 only ports */ };
uint32 stat_counter_id;
int max_ports_per_pipe = 17;
int num_pipe_instances = 16;
int inst, min_port, max_port, ct;
bcm_vlan_t ing_vlan = 5;
bcm_field_entry_t entry[2];
bcm_field_group_config_t group_config;
bcm_field_flexctr_config_t flexctr_cfg;
int test_failed = 0; /* Final result will be stored here */
int rx_count = 0; /* Total test packet received by CPU for verification */

/* IFP rule */
bcm_error_t
ifpClassSetup(int unit) {

    int rv = 0;
    int pp_pipe = pipe_list[0];
    bcm_pbmp_t in_pbmp;
    bcm_pbmp_t in_pbmp_mask;
    bcm_port_config_t pc;
    bcm_field_group_oper_mode_t fp_mode;

    /* This test requires ifp module in pipe local/unique mode */
    bcm_field_group_oper_mode_get(unit, bcmFieldQualifyStageIngress, &fp_mode);
    print fp_mode;
    if ( bcmFieldGroupOperModePipeLocal != fp_mode ) {
        printf("Invalid FieldGroupOperMode for this test; please check YML config\n");
        return BCM_E_PARAM;
    }
    bcm_port_config_t_init(&pc);
    bcm_port_config_get(unit, &pc);

    /* IFP group configuration with Device port bitmap qualifier */
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyOuterVlanId);
    BCM_FIELD_QSET_ADD(group_config.qset,bcmFieldQualifyDevicePortBitmap);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDrop);
    group_config.priority = BCM_FIELD_GROUP_PRIO_ANY;
    group_config.ports = pc.per_pp_pipe[pp_pipe]; /* FP PIPE = pp_pipe */
    group_config.flags= BCM_FIELD_GROUP_CREATE_WITH_MODE |
                        BCM_FIELD_GROUP_CREATE_WITH_PORT;
    group_config.mode= bcmFieldGroupModeAuto;
    rv = bcm_field_group_config_create(unit, &group_config);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_group_config_create() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create FP entry to match on class[0] and class[1] DevicePorts */
    rv = bcm_field_entry_create(unit, group_config.group, &entry[0]);
    if (BCM_FAILURE(rv)) {
        printf("entry[0]:bcm_field_entry_create() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Entry priority determines which TCAM entry will hit for group0 + group1 common ports */
    rv = bcm_field_entry_prio_set(unit, entry[0], 20);
    if (BCM_FAILURE(rv)) {
        printf("entry[0]:bcm_field_entry_prio_set() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* TH4 has all ports mapped to 4 pp_pipes; each pp_pipe is mapped to
       four port pipes for example pp_pipe[3] processes packets from physical port_pipe[12-15]
       FP processing pipe is mapped to corresponding pp_pipe */
    for (inst = pp_pipe*4 ; inst < (pp_pipe*4 + 4); inst++) {
        min_port = (max_ports_per_pipe * inst);
        max_port = ((max_ports_per_pipe * inst) + max_ports_per_pipe);
        BCM_PBMP_PORT_ADD(in_pbmp, class[0] + min_port);
        BCM_PBMP_PORT_ADD(in_pbmp, class[1] + min_port);
            for (ct = min_port; ct < max_port; ct++) {
                BCM_PBMP_PORT_ADD(in_pbmp_mask, ct);
            }
    }
    print in_pbmp;
    print in_pbmp_mask;
    rv = bcm_field_qualify_OuterVlanId(unit, entry[0], ing_vlan, 0xFFF);
    rv |= bcm_field_qualify_DevicePortBitmap(unit, entry[0], in_pbmp, in_pbmp_mask);
    if (BCM_FAILURE(rv)) {
        printf("entry[0]:bcm_field_qualify_*() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_action_add(unit, entry[0], bcmFieldActionDrop, 0, 0);
    if (BCM_FAILURE(rv)) {
        printf("entry[0]:bcm_field_action_add() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Setup Flex Counter action to verify IFP rule hits */
    rv = flexCounterSetup(unit, group_config.group, &stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("flexCounterSetup() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Attach index 1 flex counter to fp entry[0] */
    flexctr_cfg.flexctr_action_id = stat_counter_id;
    flexctr_cfg.counter_index = 1;
    rv =  bcm_field_entry_flexctr_attach(unit, entry[0], &flexctr_cfg);
    if (BCM_FAILURE(rv)) {
        printf("entry[0]:bcm_field_entry_flexctr_attach() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Write IFP entry[0] in hardware */
    rv = bcm_field_entry_install(unit, entry[0]);
    if (BCM_FAILURE(rv)) {
        printf("entry[0]:bcm_field_entry_install() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_PBMP_CLEAR(in_pbmp);
    BCM_PBMP_CLEAR(in_pbmp_mask);

    /* Create FP entry to match on class[2] and class[1] DevicePorts */
    rv = bcm_field_entry_create(unit, group_config.group, entry[1]);
    if (BCM_FAILURE(rv)) {
        printf("entry[1]:bcm_field_entry_create() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_entry_prio_set(unit, entry[1], 2);
    if (BCM_FAILURE(rv)) {
        printf("entry[1]:bcm_field_entry_prio_set() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    for (inst = pp_pipe*4 ; inst < (pp_pipe*4 + 4); inst++) {
        min_port = (max_ports_per_pipe * inst);
        max_port = ((max_ports_per_pipe * inst) + max_ports_per_pipe);
        BCM_PBMP_PORT_ADD(in_pbmp, class[1] + min_port);
        BCM_PBMP_PORT_ADD(in_pbmp, class[2] + min_port);
            for (ct = min_port; ct < max_port; ct++) {
                BCM_PBMP_PORT_ADD(in_pbmp_mask, ct);
            }
    }

    rv = bcm_field_qualify_OuterVlanId(unit, entry[1], ing_vlan, 0xFFF);
    rv |= bcm_field_qualify_DevicePortBitmap(unit, entry[1], in_pbmp, in_pbmp_mask);
    if (BCM_FAILURE(rv)) {
        printf("entry[1]:bcm_field_qualify_*() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_action_add(unit, entry[1], bcmFieldActionCopyToCpu, 0, 0);
    if (BCM_FAILURE(rv)) {
        printf("entry[1]:bcm_field_action_add() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    flexctr_cfg.flexctr_action_id = stat_counter_id;
    flexctr_cfg.counter_index = 2;
    rv = bcm_field_entry_flexctr_attach(unit, entry[1], &flexctr_cfg);
    if (BCM_FAILURE(rv)) {
        printf("entry[1]:bcm_field_entry_flexctr_attach() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }
    /* Write IFP entry[1] in hardware */
    rv = bcm_field_entry_install(unit, entry[1]);
    if (BCM_FAILURE(rv)) {
        printf("entry[1]:bcm_field_entry_install() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;

}

/* create IFP flex counter action with 32 counters */
bcm_error_t
flexCounterSetup(int unit, int group, uint32 * counter_id)  {

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
    rv = bcm_flexctr_action_create(unit, options, &action, counter_id);
    if (BCM_FAILURE(rv)) {
       printf("bcm_flexctr_action_create() FAILED: %s\n", bcm_errmsg(rv));
       return rv;
    }
    printf("stat_counter_id == %d \r\n", *counter_id);

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
    bcm_pbmp_t ports_pbmp[4];
    bcm_pbmp_t tpbmp;
    int no;

    rv = bcm_port_config_get(unit, &configP);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_port_config_get: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    sal_srand(sal_time_usecs());
    no = sal_rand() % 4;
    /* Select one of the pp_pipe[0-3] */
    pipe_list[0] = no;

    /* Read port bitmap of all 4 port_pipes of the pp_pipe[no] */
    for (i = 0; 4 > i; i++) {
         BCM_IF_ERROR_RETURN(bcm_port_pipe_pbmp_get(0, (no*4 + i), &ports_pbmp[i]));
        }

    /* Select one port each from port_pipes of the pp_pipe[no] */
    for (i = 0; num_ports > i; i++) {
        tpbmp = ports_pbmp[i];
        for (j = 1; j < BCM_PBMP_PORT_MAX; j++) {
            if (BCM_PBMP_MEMBER(&tpbmp, j) && (port < num_ports)) {
                printf("pp_pipe%d:pipe%d:port%d is selected\n", no, (no*4 + i), j);
                port_list[port] = j;
                port++;
                break;
            }
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

/* Configures the port group DevicePort classids */
bcm_error_t
port_ipbm_setup(int unit) {
    int rv = 0;
    /* Set port Device port bitmap for all test ports port_list[0-3] */
    rv = bcm_port_class_set(unit, port_list[0], bcmPortClassFieldIngressDevicePort, class[0]);

    rv |= bcm_port_class_set(unit, port_list[1], bcmPortClassFieldIngressDevicePort, class[1]);
    rv |= bcm_port_class_set(unit, port_list[2], bcmPortClassFieldIngressDevicePort, class[1]);

    rv |= bcm_port_class_set(unit, port_list[3], bcmPortClassFieldIngressDevicePort, class[2]);

    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_port_class_set: %s.\n",bcm_errmsg(rv));
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

    if (port_list[3] == src_port || port_list[1] == src_port) {
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

    if (BCM_E_NONE != portNumbersGet(unit, port_list, pipe_list, num_ports)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    bcm_pbmp_t  pbmp;
    int i = 0;
    BCM_PBMP_CLEAR(pbmp);
    for (; i<num_ports; i++) {
       BCM_PBMP_PORT_ADD(pbmp, port_list[i]);
    }
    if (BCM_E_NONE != port_lb_setup(unit, pbmp)) {
        printf("port_lb_setup() failed \n");
        return -1;
    }

    if (BCM_E_NONE != port_ipbm_setup(unit)) {
        printf("port_ipbm_setup() failed \n");
        return -1;
    }

    bshell(unit, "pw start report all +raw +decode");

    rv = registerPktioRxCallback(unit);
    if (BCM_FAILURE(rv)) {
        printf("registerPktioRxCallback() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Setup a IFP rule with Flex Counters verfication */
    rv = ifpClassSetup(unit);
    if (BCM_FAILURE(rv)) {
        printf("ifpClassSetup() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Verify function call */
bcm_error_t
testVerify(int unit) {

    char str[512];
    int rv = 0;
    bcm_flexctr_counter_value_t counter_value;
    uint32 num_entries = 1;
    uint32 counter_index = 1;
    int count = 1;
    int e0_count =(2*count)+2, e1_count = (2*count)+1;

    bshell(unit, "fp show");
    bshell(unit, "sleep quiet 1");
    printf("\nSending test packets to test ports\n");
    /* Send test packets to class[0] port of portgroup0 */
    snprintf(str, 512, "tx %d pbm=%d data=0x0000000002020000000000018100000508004500002E000000004006A4F00A0A0A14C0A80114138C0569000000000000000050000000B2060000000102030405225EB0B6", count, port_list[0]);
    bshell(unit, str);
    bshell(unit, "sleep 1");
    /* Send test packets to class[1] shared port of port of portgroup0 */
    snprintf(str, 512, "tx %d pbm=%d data=0x0000000002020000000000018100000508004500002E000000004006A4F00A0A0A14C0A80165138C0569000000000000000050000000B2060000000102030405225EB0B6", count+2, port_list[2]);
    bshell(unit, str);
    bshell(unit, "sleep 1");
    /* Send test packets to class[2] port of portgroup1 */
    snprintf(str, 512, "tx %d pbm=%d data=0x0000000002020000000000018100000508004500002E000000004006A4F00A0A0A14C0A80214138C0569000000000000000050000000B2060000000102030405225EB0B6", count+1, port_list[3]);
    bshell(unit, str);
    bshell(unit, "sleep 1");

    /* Set (entry[1] priority > entry[0] priority) so that IFP entry[1] will hit for group0 + group1 common ports */
    rv = bcm_field_entry_prio_set(unit, entry[1], 22);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_prio_set() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Send test packets to class[1] overlapping port of port of portgroup0 */
    snprintf(str, 512, "tx %d pbm=%d data=0x0000000002020000000000018100000508004500002E000000004006A4F00A0A0A14C0A80165138C0569000000000000000050000000B2060000000102030405225EB0B6", count, port_list[1]);
    bshell(unit, str);
    bshell(unit, "sleep quiet 3");
    bshell(unit, "show c");

    printf("\n------------------------------------- \n");
    printf("    RESULT OF RX PACKET VERIFICATION  ");
    printf("\n------------------------------------- \n");
    printf("Test = [%s]",((test_failed == 0 ) && (e1_count == rx_count))?"PASS":"FAIL");
    printf("\n------------------------------------- \n");
    /* test_failed is asserted by pktioRxCallback().
       sleep command in needed to give pktioRxCallback() chance to run. */
    if (test_failed == 1) {

       return BCM_E_FAIL;
    }

    /* Get flex counter value of entry[0] */
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
    printf("FlexCtr Get entry[0]-%d : %d packets / %d bytes\n", entry[0], COMPILER_64_LO(counter_value.value[0])
                                                , COMPILER_64_LO(counter_value.value[1]));
    if (COMPILER_64_LO(counter_value.value[0]) != e0_count) {
      printf("Packet verify failed. Expected packet stat %d but get %d\n", e0_count, COMPILER_64_LO(counter_value.value[0]));
      test_failed = 1;
    };

    counter_index = 2;
    /* Get flex counter value of entry[1] */
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
    printf("FlexCtr Get entry[1]-%d : %d packets / %d bytes\n", entry[1], COMPILER_64_LO(counter_value.value[0])
                                                , COMPILER_64_LO(counter_value.value[1]));
    if (COMPILER_64_LO(counter_value.value[0]) != e1_count) {
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
 *  b) actual configuration (Done in ifpClassSetup())
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
