/*
 * Feature  : Flex counter to demonstrate quantization in pipe local mode
 *
 * Usage    : BCM.0> cint h4_hsdk_flexctr_quantization.c
 *
 * Config   : th4_hsdk_flexctr_quantization_64x400.config.yml
 *
 * Log file : th4_hsdk_flexctr_quantization_log.txt
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
 * cint script which shows configuring different quantization ranges for ingress
 * Vlan and counting on ingress port in pipe local mode
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup):
 *   =============================================
 *     a) Select ingress port and egress port configure them in loopback mode
 *     b) Necessary flex counter configurations are done.
 *        1. The ingress vlan quantization range is selected as the object
 *        2. Packets are counted on ingress port [source]
 *
 *     Note: IFP configuration on egress port is meant for a verification
 *           purpose and is not part of the feature configuration.
 *
 *   2) Step2 - Verification (Done in test_verify):
 *   ===============================================
 *     a) Send the packet that matches the description [Different vlan range
 *        and selected ingress port]
 *
 *     b) Expected Result:
 *     ===================
 *     The number of sent packets on different vlan range should match the flex
 *     ctr stat get output
 */

/* Reset C interpreter*/

cint_reset();

const int PIPE_MAX = 4; /* bcm_info_get can be used to get this information */
bcm_port_t egress_port;
bcm_port_t ingress_port;
uint32_t stat_counter_id[PIPE_MAX];

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

/* Provides ports for the specific pipe */

int
flexctr_pipe_pbmp_get(int unit, int pipe, bcm_pbmp_t *pipe_pbmp)
{
    int rv;
    bcm_pbmp_t pbmp;
    bcm_port_t port;
    bcm_port_config_t cfg;

    rv = bcm_port_config_get(unit, &cfg);
    if (BCM_FAILURE(rv)) {
        printf("bcm_port_pipe_pbmp_get FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    if(pipe == 0)
        pbmp = cfg.per_pp_pipe[0];
    else if(pipe == 1)
        pbmp = cfg.per_pp_pipe[1];
    else if(pipe == 2)
        pbmp = cfg.per_pp_pipe[2];
    else if(pipe == 3)
        pbmp = cfg.per_pp_pipe[3];

    BCM_PBMP_ITER(pbmp, port) {
        BCM_PBMP_PORT_ADD(*pipe_pbmp, port);
    }

    return rv;
}

/*    Configures the flex counters to count on different vlan ranges on a given
 *    ingress port to demonstrate quantization functionality
 */

int
configure_flex_counter_quantization(int unit, int pipe)
{
    bcm_error_t  rv;
    int options = 0;
    bcm_flexctr_action_t action;
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    bcm_flexctr_value_operation_t *value_b_op = NULL;

    bcm_flexctr_quantization_t quant;
    uint32 qid=0;

     /* 16b ranges are as follows (maximum 8 ranges)
       2 ~ 511      - range 0
       0512 ~ 1023  - range 1
       1024 ~ 1535  - range 2
       1536 ~ 2047  - range 3
       2048 ~ 2559  - range 4
       2560 ~ 002F  - range 5
       3072 ~ 3583  - range 6
       3584 ~ 4094  - range 7
     */

    bcm_flexctr_quantization_t_init(&quant);
    quant.flags = 0;
    quant.object = bcmFlexctrObjectStaticIngVfi;

    quant.range_check_min[0] = 2;
    quant.range_check_max[0] = 511;
    quant.range_check_min[1] = 512;
    quant.range_check_max[1] = 1023;
    quant.range_check_min[2] = 1024;
    quant.range_check_max[2] = 1535;
    quant.range_check_min[3] = 1536;
    quant.range_check_max[3] = 2047;
    quant.range_check_min[4] = 2048;
    quant.range_check_max[4] = 2559;
    quant.range_check_min[5] = 2560;
    quant.range_check_max[5] = 3071;
    quant.range_check_min[6] = 3072;
    quant.range_check_max[6] = 3583;
    quant.range_check_min[7] = 3584;
    quant.range_check_max[7] = 4094;
    quant.range_num = 8;
    rv = flexctr_pipe_pbmp_get(unit, pipe, &quant.ports);
	if (BCM_FAILURE(rv)) {
        printf("Error in flexctr_pipe_pbmp_get %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create quantization. */
    rv = bcm_flexctr_quantization_create(unit, options, &quant, &qid);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_flexctr_quantization_create %s\n", bcm_errmsg(rv));
        return rv;
    }

    bcm_flexctr_action_t_init(&action);
    action.flags = 0;
    action.source = bcmFlexctrSourceIngPort;
    action.hint = 0;
    action.mode = bcmFlexctrCounterModeNormal;
    action.index_num = 9; /* 0 ~ 7 for match cases and 8 for no match case */
    rv = flexctr_pipe_pbmp_get(unit, pipe, &action.ports);
	if (BCM_FAILURE(rv)) {
        printf("Error in flexctr_pipe_pbmp_get %s\n", bcm_errmsg(rv));
        return rv;
    }

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

    /* Create an ingress action */
    rv = bcm_flexctr_action_create(unit, options, &action, stat_counter_id[pipe]);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_flexctr_action_create %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Provides the pipe to which the port belongs */

int
pipe_from_port(int unit, bcm_port_t port, int *opipe)
{
    int pipe = 0;
    bcm_port_config_t port_config;

    BCM_IF_ERROR_RETURN (bcm_port_config_get(unit, &port_config));

    for (pipe = 0; pipe < PIPE_MAX; pipe++) {
        if (BCM_PBMP_MEMBER(port_config.per_pp_pipe[pipe], port)) {
            break;
        }
    }

    if (pipe >= PIPE_MAX) {
        return BCM_E_PARAM;
    }

    *opipe = pipe;

    return BCM_E_NONE;
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
    int pipe;

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

    /*    Configures the flex counters to count on different vlan ranges on a
     *    given ingress port to demonstrate quantization functionality
     */
    for (pipe = 0; pipe < PIPE_MAX; pipe++) {
        rv = configure_flex_counter_quantization(unit, pipe);
        if (BCM_FAILURE(rv)) {
            printf("Error in configure_flex_counteregr_pkt_attr %s\n", bcm_errmsg(rv));
            return rv;
        }
    }

    rv = pipe_from_port(unit, ingress_port, &pipe);
    if (BCM_FAILURE (rv)) {
        printf ("pipe_from_port FAILED: %s\n", bcm_errmsg (rv));
        return rv;
    }

    /* Attach counter action to a port */
    rv = bcm_port_stat_attach(unit, ingress_port, stat_counter_id[pipe]);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_port_stat_attach %s\n", bcm_errmsg(rv));
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
    uint32 counter_index[9] = {0, 1, 2, 3, 4 ,5 ,6, 7, 8};
    int count = 2;
    int i;
    bcm_vlan_t vlan;
    int pipe;

    /* Sending packets in different vlan ranges */

    /*    Vlan creation is for verification purpose and can create any vlan in
     *    the specified range
     */
    vlan = 5;
    snprintf(str, 512, "vlan create %d pbm=%d,%d", vlan, ingress_port, egress_port);
    bshell(unit, str);

    printf("\nSending %d packet with %d vlan on %d ingress port \n", count, vlan, ingress_port);
    snprintf(str, 512, "tx %d pbm=%d vlan=%d", count, ingress_port, vlan);
    bshell(unit, str);
    sal_sleep(1);

    vlan = 576;
    snprintf(str, 512, "vlan create %d pbm=%d,%d", vlan, ingress_port, egress_port);
    bshell(unit, str);

    printf("\nSending %d packet with %d vlan on %d ingress port \n", count+2, vlan, ingress_port);
    snprintf(str, 512, "tx %d pbm=%d vlan=%d", count+2, ingress_port, vlan);
    bshell(unit, str);
    sal_sleep(1);

    vlan = 1300;
    snprintf(str, 512, "vlan create %d pbm=%d,%d", vlan, ingress_port, egress_port);
    bshell(unit, str);

    printf("\nSending %d packet with %d vlan on %d ingress port \n", count+1, vlan, ingress_port);
    snprintf(str, 512, "tx %d pbm=%d vlan=%d", count+1, ingress_port, vlan);
    bshell(unit, str);
    sal_sleep(1);

    vlan = 2000;
    snprintf(str, 512, "vlan create %d pbm=%d,%d", vlan, ingress_port, egress_port);
    bshell(unit, str);

    printf("\nSending %d packet with %d vlan on %d ingress port \n", count-1, vlan, ingress_port);
    snprintf(str, 512, "tx %d pbm=%d vlan=%d", count-1, ingress_port, vlan);
    bshell(unit, str);
    sal_sleep(1);

    vlan = 2200;
    snprintf(str, 512, "vlan create %d pbm=%d,%d", vlan, ingress_port, egress_port);
    bshell(unit, str);

    printf("\nSending %d packet with %d vlan on %d ingress port \n", count+3, vlan, ingress_port);
    snprintf(str, 512, "tx %d pbm=%d vlan=%d", count+3, ingress_port, vlan);
    bshell(unit, str);
    sal_sleep(1);

    vlan = 3001;
    snprintf(str, 512, "vlan create %d pbm=%d,%d", vlan, ingress_port, egress_port);
    bshell(unit, str);

    printf("\nSending %d packet with %d vlan on %d ingress port \n", count+2, vlan, ingress_port);
    snprintf(str, 512, "tx %d pbm=%d vlan=%d", count+2, ingress_port, vlan);
    bshell(unit, str);
    sal_sleep(1);

    vlan = 3502;
    snprintf(str, 512, "vlan create %d pbm=%d,%d", vlan, ingress_port, egress_port);
    bshell(unit, str);

    printf("\nSending %d packet with %d vlan on %d ingress port \n", count, vlan, ingress_port);
    snprintf(str, 512, "tx %d pbm=%d vlan=%d", count, ingress_port, vlan);
    bshell(unit, str);
    sal_sleep(1);

    vlan = 4006;
    snprintf(str, 512, "vlan create %d pbm=%d,%d", vlan, ingress_port, egress_port);
    bshell(unit, str);

    printf("\nSending %d packet with %d vlan on %d ingress port \n", count+1, vlan, ingress_port);
    snprintf(str, 512, "tx %d pbm=%d vlan=%d", count+1, ingress_port, vlan);
    bshell(unit, str);
    sal_sleep(1);

    vlan = 4095;
    snprintf(str, 512, "vlan create %d pbm=%d,%d", vlan, ingress_port, egress_port);
    bshell(unit, str);

    printf("\nSending %d packet with %d vlan on %d ingress port \n", count+3, vlan, ingress_port);
    snprintf(str, 512, "tx %d pbm=%d vlan=%d", count+3, ingress_port, vlan);
    bshell(unit, str);
    sal_sleep(5);
    printf("\n");

    rv = pipe_from_port(unit, ingress_port, &pipe);
    if (BCM_FAILURE (rv)) {
        printf ("pipe_from_port FAILED: %s\n", bcm_errmsg (rv));
        return rv;
    }

    /* Get counter value. */
    for(i=0 ; i<9; i++) {
        sal_memset(&counter_value, 0, sizeof(counter_value));
        rv = bcm_flexctr_stat_sync_get(unit, stat_counter_id[pipe], num_entries, counter_index[i], &counter_value);
        if (BCM_FAILURE(rv)) {
          printf("Error in bcm_flexctr_stat_sync_get %s\n", bcm_errmsg(rv));
          return rv;
        }

        /* print counter value. */
        printf("Stat in vlan range %d : %d packets / %d bytes\n", i,
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
