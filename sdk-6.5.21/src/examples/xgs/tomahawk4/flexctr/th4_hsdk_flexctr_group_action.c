/*
 * Feature  : Flex counter to demonstrate group action
 *
 * Usage    : BCM.0> cint h4_hsdk_flexctr_group_action.c
 *
 * Config   : th4_hsdk_flexctr_64x400.config.yml
 *
 * Log file : th4_hsdk_flexctr_group_action_log.txt
 *
 * Test Topology :
 *                      +----------------------+
 *                      |                      |
 *                      |        TH4           |
 *        ingress_port  |                      |  egress_port
 *       ------------->-+  Flexctr             +---------->----
 *                      |   Src: IngPort       |
 *                      |   Obj: Quantization  |
 *                      |                      |
 *                      |                      |
 *                      +----------------------+
 *
 *
 * Summary:
 * ========
 * In flex counter group action, single source trigger multiple (group of)
 * actions each updating different counter pool. Counter processor supports upto
 * four group actions.
 * This cint script shows configuring different quantization ranges for IP
 * packet length and counting on ingress port. This configures flex counter
 * group action in which one of the actions counts packets and bytes. The other
 * action shows the maximum packet length in the counting range of IP packet
 * length and the length of the last packet sent in the range.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup):
 *   =============================================
 *     a) Select ingress port and egress port configure them in loopback mode
 *     b) Necessary flex counter configurations are done.
 *        1. The IP packet length quantization range is selected as the object
 *        2. Packets are counted on ingress port [source]
 *        3. Flex counter group action is configured to count packets, bytes,
 *           max packet length in the range and the last packet length
 *
 *     Note: IFP configuration on egress port is meant for a verification
 *           purpose and is not part of the feature configuration.
 *
 *   2) Step2 - Verification (Done in test_verify):
 *   ===============================================
 *     a) Send the packets that matches the description [Different IP packet
 *        length and selected ingress port]
 *
 *     b) Expected Result:
 *     ===================
 *     The number of sent packets should match the flex ctr stat get output
 */

/* Reset C interpreter*/

cint_reset();

bcm_port_t egress_port;
bcm_port_t ingress_port;
uint32_t stat_counter_id;

/*
 *    Checks if given port/index is already present
 *    in the list so that same port number is not generated again
 */

int
check_assigned_port(int *port_index_list,int no_entries, int index)
{
    int i=0;

    for (i=0; i < no_entries; i++) {
        if (port_index_list[i] == index)
        return 1;
    }

    /* no entry found */
    return 0;
}

/*
 *    Provides required number of ports
 */

int
port_numbers_get(int unit, int *port_list, int num_ports)
{
    int i = 0, port = 0, index = 0;
    bcm_port_config_t port_config;
    bcm_pbmp_t ports_pbmp;
    int temp_ports[BCM_PBMP_PORT_MAX];
    int port_index_list[num_ports];

    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &port_config));

    ports_pbmp = port_config.e;
    for (i = 1; i < BCM_PBMP_PORT_MAX; i++) {
        if (BCM_PBMP_MEMBER(&ports_pbmp, i)) {
            temp_ports[port] = i;
            port++;
        }
    }

    if (( port == 0 ) || ( port < num_ports )) {
        printf("Eror in portNumbersGet \n");
        return -1;
    }

    /* generating random ports */
    for (i= 0; i < num_ports; i++) {
        index = sal_rand() % port;
        if (check_assigned_port(port_index_list, i, index) == 0) {
            port_list[i] = temp_ports[index];
            port_index_list[i] = index;
        } else {
            i = i - 1;
        }
    }

    return BCM_E_NONE;
}

/*
 *    Configures the port in loopback mode
 */

int
ingress_port_multi_setup(int unit, bcm_pbmp_t pbm)
{
    bcm_port_t port;

    BCM_PBMP_ITER(pbm, port) {
        printf("ingress_port_multi_setup port: %d setup\n", port);
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
    }

    return BCM_E_NONE;
}

/*
 *    Configures the port in loopback mode and installs an IFP rule. This IFP
 *    rule drops the packets ingressing on the specified port to CPU. This is to
 *    avoid continuous loopback of the packet from the egress port.
 */

int
egress_port_multi_setup(int unit, bcm_pbmp_t pbm)
{
    bcm_field_group_t group;
    bcm_field_entry_t entry;
    bcm_port_t port;

    bcm_field_group_config_t group_config;

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDrop);
    group_config.priority = 2;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    BCM_PBMP_ITER(pbm, port) {
        printf("egress_port_multi_setup port: %d setup\n", port);

        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));
        BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));
        BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
    }

    return BCM_E_NONE;
}

/*    Configures the flex counters to count on a range of IP packet length on a
 *    given ingress port to demonstrate flex counter group action feature
 */

int
configure_flex_counter_group_action(int unit)
{
    bcm_error_t  rv;
    int options = 0;
    bcm_flexctr_action_t action;
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    bcm_flexctr_value_operation_t *value_b_op = NULL;

    bcm_flexctr_quantization_t quant;
    uint32 qid=0;

    bcm_flexctr_group_action_t group_action;
    uint32_t stat_counter_id_1, stat_counter_id_2;

    /* 16b ranges are as follows (maximum 8 ranges)
       40 ~ 120   - range 0
    */

    bcm_flexctr_quantization_t_init(&quant);
    quant.flags = 0;
    quant.object = bcmFlexctrObjectStaticIngIpTotalLength;

    quant.range_check_min[0] = 40;
    quant.range_check_max[0] = 120;

    quant.range_num = 1;

    /* Create quantization. */
    rv = bcm_flexctr_quantization_create(unit, options, &quant, &qid);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_quantization_create %s\n", bcm_errmsg(rv));
        return rv;
    }

    bcm_flexctr_action_t_init(&action);
    action.source = bcmFlexctrSourceIngPort;
    action.hint = 0;
    action.mode = bcmFlexctrCounterModeNormal;
    action.index_num = 1; /* 1 counter for match case */

    /* Per ingress port count. */
    index_op = &action.index_operation;
    index_op->object[0] = bcmFlexctrObjectQuant;
    index_op->quant_id[0] = qid;
    index_op->mask_size[0] = 4;
    index_op->shift[0] = 0;
    index_op->object[1] = bcmFlexctrObjectConstZero;
    index_op->mask_size[1] = 1;
    index_op->shift[1] = 0;

    /* Increase counter per packet. */
    value_a_op = &action.operation_a;
    value_a_op->select = bcmFlexctrValueSelectCounterValue;
    value_a_op->object[0] = bcmFlexctrObjectConstOne;
    value_a_op->mask_size[0] = 1;
    value_a_op->shift[0] = 0;
    value_a_op->object[1] = bcmFlexctrObjectConstZero;
    value_a_op->mask_size[1] =1;
    value_a_op->shift[1] = 0;
    value_a_op->type = bcmFlexctrValueOperationTypeInc;

    /* Increase counter per packet bytes. */
    value_b_op = &action.operation_b;
    value_b_op->select = bcmFlexctrValueSelectPacketLength;
    value_b_op->type = bcmFlexctrValueOperationTypeInc;

    /* Create an ingress action for counting packets and bytes */
    rv = bcm_flexctr_action_create(unit, options, &action, &stat_counter_id_1);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_action_create %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf("stat_counter_id == %d \r\n", stat_counter_id_1);

    bcm_flexctr_action_t_init(&action);
    action.source = bcmFlexctrSourceIngPort;
    action.hint = 1;
    action.mode = bcmFlexctrCounterModeNormal;
    action.index_num = 1; /* 1 counter for match case */

    /* Per quantization range port count. */
    index_op = &action.index_operation;
    index_op->object[0] = bcmFlexctrObjectQuant;
    index_op->quant_id[0] = qid;
    index_op->mask_size[0] = 4;
    index_op->shift[0] = 0;
    index_op->object[1] = bcmFlexctrObjectConstZero;
    index_op->mask_size[1] = 1;
    index_op->shift[1] = 0;

    /* Increase counter per packet. */
    value_a_op = &action.operation_a;
    value_a_op->select = bcmFlexctrValueSelectPacketLength;
    value_a_op->object[0] = bcmFlexctrObjectConstOne;
    value_a_op->mask_size[0] = 1;
    value_a_op->shift[0] = 0;
    value_a_op->object[1] = bcmFlexctrObjectConstZero;
    value_a_op->mask_size[1] =1;
    value_a_op->shift[1] = 0;
    value_a_op->type = bcmFlexctrValueOperationTypeMax;

    /* Increase counter per packet bytes. */
    value_b_op = &action.operation_b;
    value_b_op->select = bcmFlexctrValueSelectPacketLength;
    value_b_op->type = bcmFlexctrValueOperationTypeSet;

    /* Create an ingress action for counting maximum packet length in the given
     * range and set the length of the last packet */
    rv = bcm_flexctr_action_create(unit, options, &action, &stat_counter_id_2);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_flexctr_action_create %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf("stat_counter_id == %d \r\n", stat_counter_id_2);

    bcm_flexctr_group_action_t_init(&group_action);
    group_action.stat_counter_id[0] = stat_counter_id_1;
    group_action.stat_counter_id[1] = stat_counter_id_2;
    group_action.action_num = 2;

    rv = bcm_flexctr_group_action_create(unit, options, &group_action,
                                         &stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_flexctr_group_action_create %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf("stat_group_counter_id == %d \r\n", stat_counter_id);

    /* Attach group counter action to a ingress port */
    rv = bcm_port_stat_attach(unit, ingress_port, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_port_stat_attach %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/*
 *    Configures the flex counter feature and the required set up to verify it
 */

int test_setup(int unit)
{
    int num_ports = 2;
    int port_list[num_ports];
    bcm_error_t rv;
    bcm_pbmp_t pbmp;

    rv = port_numbers_get(unit, port_list, num_ports);
    if (BCM_FAILURE(rv)) {
        printf("Error in portNumbersGet %s\n", bcm_errmsg(rv));
        return rv;
    }

    ingress_port = port_list[0];
    egress_port = port_list[1];
    printf("Choosing port %d as ingress port and %d as egress port\n", ingress_port, egress_port);

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);
    rv = ingress_port_multi_setup(unit, pbmp);
    if (BCM_FAILURE(rv)) {
        printf("Error in ingress_port_multi_setup %s\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, egress_port);
    rv = egress_port_multi_setup(unit, pbmp);
    if (BCM_FAILURE(rv)) {
        printf("Error in egress_port_multi_setup %s\n", bcm_errmsg(rv));
        return rv;
    }

    bshell(unit, "pw start report +raw +decode");

    /*    Configures the flex counters to count on  a range of IP packet length
     *    on a given ingress port to show flex counter group action feature
     */
    rv = configure_flex_counter_group_action(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in configure_flex_counteregr_pkt_attr %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Flex counter feature Verification */

int
test_verify(int unit)
{
    char str[512];
    int rv;
    bcm_flexctr_counter_value_t counter_value;
    uint32 num_entries = 1;
    uint32 counter_index = 0;
    int count = sal_rand() % 10;
    if (count == 0) {
      count = 1;
    }
    bcm_vlan_t vlan = 5;
    int i;
    bcm_flexctr_group_action_t group_action_get;

    /*  Vlan creation is just for verification purpose and depends on the packet
     * being sent and is not a needed step
     */

    snprintf(str, 512, "vlan create %d pbm=%d,%d", vlan, ingress_port, egress_port);
    bshell(unit, str);

    printf("\nSending %d packets to %d ingress port \n", count, ingress_port);
    snprintf(str, 512, "tx %d pbm=%d data=0x0000000000020000000000238100000508004500006A00000000400657D6C0A84E040A0A0A020000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000", count, ingress_port);
    bshell(unit, str);
    sal_sleep(2);

    printf("\nSending %d packets to %d ingress port \n", count+1, ingress_port);
    snprintf(str, 512, "tx %d pbm=%d data=0x00000000000200000000002381000005080045000032000000004006580EC0A84E040A0A0A02000000000000000000000000000000000000000000000000000000000000", count+1, ingress_port);
    bshell(unit, str);
    sal_sleep(2);

    printf("\nSending %d packets to %d ingress port \n", count+3, ingress_port);
    snprintf(str, 512, "tx %d pbm=%d data=0x000000000002000000000023810000050800450000300000000040065810C0A84E040A0A0A0200000000000000000000000000000000000000000000000000000000", count+3, ingress_port);
    bshell(unit, str);
    sal_sleep(5);
    printf("\n");

    rv = bcm_flexctr_group_action_get(unit, stat_counter_id, &group_action_get);
    if (BCM_FAILURE (rv)) {
        printf ("pipe_from_port FAILED: %s\n", bcm_errmsg (rv));
        return rv;
    }

    /* Get counter value. */
    for(i=0 ; i<group_action_get.action_num; i++) {
        sal_memset(&counter_value, 0, sizeof(counter_value));
        rv = bcm_flexctr_stat_get(unit, group_action_get.stat_counter_id[i], num_entries, &counter_index, &counter_value);
        if (BCM_FAILURE(rv)) {
          printf("Error in bcm_flexctr_stat_sync_get %s\n", bcm_errmsg(rv));
          return rv;
        }

        /* print counter value. */
        if(i==0)
            printf("Stat action %d : %d packets and %d bytes\n", group_action_get.stat_counter_id[i],
                                            COMPILER_64_LO(counter_value.value[0]),
                                            COMPILER_64_LO(counter_value.value[1]));
        else if (i ==1)
            printf("Stat action %d : %d bytes max pkt length and %d bytes last pkt length\n", group_action_get.stat_counter_id[i],
                                            COMPILER_64_LO(counter_value.value[0]),
                                            COMPILER_64_LO(counter_value.value[1]));
    }

    return BCM_E_NONE;
}

/*
 *    This functions does the following
 *    a)test setup [configures the feature]
 *    b)test verify [verifies the configured feature]
 */

int execute()
{
    bcm_error_t rv;
    int unit = 0;

    bshell(unit, "config show; attach ; version");

    rv = test_setup(unit);
    if (BCM_FAILURE(rv)) {
        printf("test_setup() failed, %s.\n", bcm_errmsg(rv));
        return -1;
    }
    printf("\nCompleted test setup successfully\n");

    rv = test_verify(unit);
    if (BCM_FAILURE(rv)) {
        printf("test_verify() failed\n");
        return -1;
    }
    printf("\nCompleted test verify successfully\n");

    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}
