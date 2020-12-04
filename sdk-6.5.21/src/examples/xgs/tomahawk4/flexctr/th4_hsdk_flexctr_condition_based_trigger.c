/*
 * Feature  : To demonstrate usage of condition based trigger flex counters
 *
 * Usage    : BCM.0> cint th4_hsdk_flexctr_condition_based_trigger.c
 *
 * Config   : th4_hsdk_flexctr_64x400.config.yml
 *
 * Log file : th4_hsdk_flexctr_condition_based_trigger_log.txt
 *
 * Test Topology :
 *                      +----------------------+
 *                      |                      |
 *                      |        TH4           |
 *        ingress_port  |                      |  egress_port
 *       ------------->-+  Flexctr             +---------->----
 *                      |   Src: IngVlan       |
 *                      |   Obj: Trigger based |
 *                      |        ingress port  |
 *                      |                      |
 *                      +----------------------+
 *
 *
 * Summary:
 * ========
 * cint script which shows counting based on conditional trigger. In this
 * example, range of ingress ports are set as the trigger to count on ingress
 * vlan. The packets sent with the configured ingress vlan between the set start
 * trigger port and end trigger port are counted.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup):
 *   =============================================
 *     a) Ingress ports and egress port are configured in loopback mode
 *     b) Vlan on which the counters are configured is created and ports are
 *        added to the vlan
 *     c) Necessary flex counter configurations are done.
 *        1. The ingress port range is selected as the trigger object
 *        2. Packets are counted on ingress vlan [source]
 *
 *     Note: IFP configuration on egress port is meant for a verification
 *           purpose and is not part of the feature configuration.
 *
 *   2) Step2 - Verification (Done in test_verify):
 *   ===============================================
 *     a) Send the packet that matches the description [vlan and ports]
 *
 *     b) Expected Result:
 *     ===================
 *     The number of packets sent on ports between configured start port and end
 *     port should match the flex ctr stat get output
 */

/* Reset C interpreter*/

cint_reset();
bcm_port_t start_port = 2;
bcm_port_t end_port = 4;
bcm_port_t egress_port = 17;
bcm_vlan_t vlan = 30;
uint32_t stat_counter_id;
bcm_mac_t dmac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02}; /* testing purpose and not a needed step */

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
 *    rule copies the packets ingressing on the specified port to CPU. It is
 *    also configured to drop the packets on that port. This is to avoid
 *    continuous loopback of the packet from the egress port.
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

/* Create vlan and add port to vlan */
int
create_vlan_add_ports(int unit, bcm_vlan_t vlan, bcm_pbmp_t pbmp)
{
    bcm_error_t rv;
    bcm_pbmp_t ubmp;

    BCM_PBMP_CLEAR(ubmp);

    rv = bcm_vlan_create(unit, vlan);
    if ((BCM_FAILURE(rv)) & (rv != BCM_E_EXISTS)) {
        printf("Error in creating vlan : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vlan, pbmp, pbmp));

    return BCM_E_NONE;
}

/*    Configures the flex counters to count on vlan with the conditional trigger
 *    of ingress port range
 */

int
configure_flex_counter_condition_based_trigger(int unit)
{
    int options = 0;
    bcm_error_t rv;
    bcm_flexctr_action_t action;
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    bcm_flexctr_value_operation_t *value_b_op = NULL;
    bcm_flexctr_trigger_t *trigger = NULL;

    bcm_flexctr_action_t_init(&action);
    action.flags |= BCM_FLEXCTR_ACTION_FLAGS_TRIGGER;
    action.source = bcmFlexctrSourceIngVlan;
    action.hint = 0;
    action.mode = bcmFlexctrCounterModeNormal;
    action.index_num = 1;

    /* Counter trigger. */
    trigger = &action.trigger;
    trigger->trigger_type = bcmFlexctrTriggerTypeCondition;
    trigger->object = bcmFlexctrObjectStaticIngIngressPort;
    trigger->object_value_start = start_port;
    trigger->object_value_stop = end_port;
    trigger->object_value_mask = 0xffff;

    /* Per ingress port count. */
    index_op = &action.index_operation;
    index_op->object[0] = bcmFlexctrObjectConstZero;
    index_op->mask_size[0] = 15;
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

    /* Create an ingress action */
    rv = bcm_flexctr_action_create(unit, options, &action, &stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_action_create %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf("stat_counter_id == %d \r\n", stat_counter_id);

    /* Attach counter action to vlan */
    rv = bcm_vlan_stat_attach(unit, vlan, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_vlan_stat_attach %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Enable flex counter trigger */
    rv = bcm_flexctr_trigger_enable_set(unit, stat_counter_id, TRUE);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_trigger_enable_set %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/*
 *    Configures the flex counter feature and the required set up to verify it
 */

int test_setup(int unit)
{
    bcm_error_t rv;
    bcm_pbmp_t pbmp;
    bcm_port_t port;
    bcm_l2_addr_t addr;

    /*    Ports in the range of conditional trigger are put in loopback for test
     *    set up
     */

    BCM_PBMP_CLEAR(pbmp);
    for(port=start_port; port<=end_port; port++)
        BCM_PBMP_PORT_ADD(pbmp, port);

    rv = ingress_port_multi_setup(unit, pbmp);
    if (BCM_FAILURE(rv)) {
        printf("Error in ingress_port_multi_setup %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Egress port is set up for testing purpose */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, egress_port);
    rv = egress_port_multi_setup(unit, pbmp);
    if (BCM_FAILURE(rv)) {
        printf("Error in egress_port_multi_setup %s\n", bcm_errmsg(rv));
        return rv;
    }

    /*    Add ports in the range of conditional trigger to the vlan on which
     *    counters are set up
     */
    BCM_PBMP_CLEAR(pbmp);
    for(port=start_port; port<=end_port; port++)
        BCM_PBMP_PORT_ADD(pbmp, port);

    BCM_PBMP_PORT_ADD(pbmp, egress_port);

    rv = create_vlan_add_ports(unit, vlan, pbmp);
    if (BCM_FAILURE(rv)) {
        printf("Error in create_vlan_add_port for ing port %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* L2 add is done for testing purpose and not a needed step */
    bcm_l2_addr_t_init(&addr, dmac, vlan);
    addr.port = egress_port;
    rv = bcm_l2_addr_add(unit, &addr);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_l2_addr_add %s\n", bcm_errmsg(rv));
        return rv;
    }

    /*    Configure flex counters to count on vlan with conditional trigger of
     *    provided ingress port range
     */
    rv = configure_flex_counter_condition_based_trigger(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in configure_flex_counter_egr_pkt_attr %s\n", bcm_errmsg(rv));
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
    int count = 3;

    print "vlan show";
    bshell(unit, "vlan show");

    /* Put ports out of trigger range to loopback mode and add them to vlan for testing purposes */
    bshell(unit, "port cd0,cd5 lb=mac");
    snprintf(str, 512, "vlan add %d pbm=cd0,cd5", vlan);
    bshell(unit, str);

    printf("\nSending %d packets to %d ingress port \n", count-1, 1);
    snprintf(str, 512, "tx %d pbm=%d vlan=%d dm=0x2 sm=0x8", count-1, 1, vlan);
    bshell(unit, str);
    sal_sleep(1);

    printf("\nSending %d packets to %d ingress port \n", count, start_port);
    snprintf(str, 512, "tx %d pbm=%d vlan=%d dm=0x2 sm=0x3", count, start_port, vlan);
    bshell(unit, str);
    sal_sleep(1);

    printf("\nSending %d packets to %d ingress port \n", count-1, 1);
    snprintf(str, 512, "tx %d pbm=%d vlan=%d dm=0x2 sm=0x8", count-1, 1, vlan);
    bshell(unit, str);
    sal_sleep(1);

    printf("\nSending %d packets to %d ingress port \n", count-1, 3);
    snprintf(str, 512, "tx %d pbm=%d vlan=%d dm=0x2 sm=0x9", count-1, 3, vlan);
    bshell(unit, str);
    sal_sleep(1);

    printf("\nSending %d packets to %d ingress port \n", count-2, 18);
    snprintf(str, 512, "tx %d pbm=%d vlan=%d dm=0x2 sm=0x9", count-2, 18, vlan);
    bshell(unit, str);
    sal_sleep(1);

    printf("\nSending %d packets to %d ingress port \n", count-2, 3);
    snprintf(str, 512, "tx %d pbm=%d vlan=%d dm=0x2 sm=0x7", count-2, 3, vlan);
    bshell(unit, str);
    sal_sleep(1);

    printf("\nSending %d packets to %d ingress port \n", count+1, end_port);
    snprintf(str, 512, "tx %d pbm=%d vlan=%d dm=0x2 sm=0x5", count+1, end_port, vlan);
    bshell(unit, str);
    sal_sleep(4);

    printf("\nSending %d packets to %d ingress port \n", count, 3);
    snprintf(str, 512, "tx %d pbm=%d vlan=%d dm=0x2 sm=0x9", count, 3, vlan);
    bshell(unit, str);
    sal_sleep(4);


    /* Get counter value. */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    rv = bcm_flexctr_stat_sync_get(unit, stat_counter_id, num_entries, &counter_index, &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_flexctr_stat_sync_get %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* print counter value. */
    printf("\nStat - %d packets / %d bytes\n", COMPILER_64_LO(counter_value.value[0]),
                                             COMPILER_64_LO(counter_value.value[1]));

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
