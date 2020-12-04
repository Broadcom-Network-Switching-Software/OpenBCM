/*
 * Feature  : Ingress field processor group with field compression.
 *
 * Usage    : BCM.0> c th4_hsdk_ifp_compression_ttl.c
 *
 * config   : bcm56990_a0-generic-64x400.config.yml
 *
 * Log file : th4_hsdk_ifp_compression_ttl_log.txt
 *
 * Test Topology :
 *                              CPU
 *                                ^
 *                                |  Rx reason : CPU_FFP
 *                                |
 *                      +---------+---------+
 *                      |        TH4        |
 *   ingress port       |      +-----+      |
 *   in LoopBack        |      | IFP |      |
 *           ---------->+      | RULE|      |
 *     ingress VFI 5    |      +-----+      |
 *                      |                   |
 *                      +-------------------+
 *
 *
 * Summary:
 * ========
 * Tomahawk4 IFP supports IP TTL, IP TOS, TCP flags, IP_PROTOCOL and ETHERTYPE field
 * value compressions with new set of HSDK APIs in which Ethertype is of index
 * mapped type and rest of the techniques/fields are direct map type. The legacy
 * pseudo class stage qualifiers are deprecated for TH4 HSDK.
 * This cint example exercises ETHERTYPE and IP TTL FP compressions in global mode
 * to improve rule density resulting in better utilisation of IFP TCAM resources.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in testSetup()):
 *   =============================================
 *     a) Selects a random test port and configure it in Loopback mode.
 *     b) Start packet watcher and register additional custom Rx callback for the
 *        packet verfication.
 *
 *   2) Step2 - IFP Configuration (Done in ifpCompressionSetup()):
 *   =========================================================
 *     a) An IFP stage field value compression map is created for IPv6 ethertype
          to remap to EtherTypeClass value 0xA. Similarly non consecutive IP TTL
          value range is remapped to TtlClassZero value 0xA.
 *     b) Create an IFP group in auto mode to filter on outer VLAN and
 *        TTL, Ethertype class qualifiers with COPY_TO_CPU action.
 *     c) In flexCounterSetup(), Add flex counters to the IFP entry solely for
 *        verfication purpose.
 *     d) install the IFP entry in hardware.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the outputs of 'fp show' 'show c'
 *     b) Expected Result:
 *     ===================
 *     The CPU packet capture shows the ingress IPv6 packet copied by IFP rule as below
 *
 *   Ingress Packet:
 *   [bcmPWN.0]Packet[8]: Total x
     [bcmPWN.0]Packet[8]: data[0000]: {001111111111} {000000012222} 8100 0005
 *   [bcmPWN.0]Packet[8]: data[0010]: 86dd 6335 5555 0026 0605 3ffe 0000 0000
 *   [bcmPWN.0]Packet[8]: data[0020]: 0000 0000 0000 0000 0001 3ffe 0000 0000
 *   [bcmPWN.0]Packet[8]: data[0030]: 0000 0000 0000 0000 0002 3030 3131 0000
 *   [bcmPWN.0]Packet[8]: data[0040]: 3232 0000 3333 5010 7788 57db 0000 1111
 *   [bcmPWN.0]Packet[8]: data[0050]: 1111 1111 1111 1111 1111 1111 1111 1111
 *   [bcmPWN.0]
 *   [bcmPWN.0][RX metadata information]
 *   ...
 *   ING_TAG_TYPE=1
 *   [RX reasons]
 *   CPU_FFP
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t ingress_port;
uint32 stat_counter_id;
bcm_vlan_t ing_vlan = 5;
bcm_field_entry_t entry;
bcm_field_group_config_t group_config;
bcm_field_flexctr_config_t flexctr_cfg;
int test_failed = 0; /* Final result will be stored here */
int rx_count = 0; /* Total test packet received by CPU for verification */

/* Expected Egress packet */
unsigned char expected_packet[96] = {
    0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x01, 0x22, 0x22, 0x81, 0x00, 0x00, 0x05,
    0x86, 0xDD, 0x63, 0x35, 0x55, 0x55, 0x00, 0x26, 0x06, 0x05, 0x3F, 0xFE, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x3F, 0xFE, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x30, 0x30, 0x31, 0x31, 0x00, 0x00,
    0x32, 0x32, 0x00, 0x00, 0x33, 0x33, 0x50, 0x10, 0x77, 0x88, 0x57, 0xDB, 0x00, 0x00, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11
 };

bcm_error_t
ifpCompressionSetup(int unit) {

    int rv = 0;
    bcm_class_t data;
    bcm_class_t mask;
    bcm_pbmp_t pbmp;
    bcm_field_group_oper_mode_t fp_mode;
    bcm_port_config_t pc;
    bcm_port_config_t_init(&pc);
    bcm_port_config_get(unit, &pc);

    /* This test requires ifp module in global mode */
    bcm_field_group_oper_mode_get(unit, bcmFieldQualifyStageIngress, &fp_mode);
    print fp_mode;
    if ( bcmFieldGroupOperModeGlobal != fp_mode ) {
        printf("Invalid FieldGroupOperMode for this test; please check YML config\n");
        return BCM_E_PARAM;
    }

    /* Only IPv6 ETHERTYPE is remapped to a new value of EtherTypeClass 0xA
    Ethertypes are larger in size but in practice only few values are used */

    uint16 eth = 0x86DD;
    uint16 eth_remap = 0xA;
    /* Global mode configuration */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_ASSIGN(pbmp, pc.all);
    rv = bcm_field_compression_index_mapped_set(unit, bcmFieldCompressionIndexMapTypeEthertype,
                                                 1, pbmp, eth, eth_remap);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_compression_index_mapped_set() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* non consecutive IP TTL values are mapped to TTL class 0xA */
    uint16 ttl[] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x11, 0x12, 0x13, 0x14, 0x15};
    uint16 ttl_remap[] = {0xA, 0xA, 0xA, 0xA, 0xA, 0xA, 0xA, 0xA, 0xA, 0xA};
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_ASSIGN(pbmp, pc.all);
    rv = bcm_field_compression_direct_mapped_set(unit, bcmFieldCompressionDirectMapTypeTtlZero,
                                                 pbmp, 10, ttl, ttl_remap);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_compression_direct_mapped_set() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    COMPILER_64_SET(data, 0, 0xA);
    COMPILER_64_SET(mask, 0, 0xFF);

    /* IFP group configuration with SMAC and SRC IPv6 qualifiers */
    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyTtlClassZero);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyOuterVlanId);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyEtherTypeClass);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDrop);
    group_config.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE;
    group_config.mode = bcmFieldGroupModeAuto;

    rv = bcm_field_group_config_create(unit, &group_config);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_group_config_create() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create FP entry to match on ingress vlan and extracted packet fields */
    rv = bcm_field_entry_create(unit, group_config.group, &entry);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_create() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_qualify_OuterVlanId(unit, entry, ing_vlan, 0xFFF);
    rv |= bcm_field_qualify_EtherTypeClass(unit, entry, data, mask);
    rv |= bcm_field_qualify_TtlClassZero(unit, entry, data, mask);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_qualify_*() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0);
    rv |= bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_action_add() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Setup Flex Counters to verify IFP rule hits */
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

    int num_ports = 1;
    int port_list[num_ports];
    bcm_error_t         rv;

    if (BCM_E_NONE != portNumbersGet(unit, port_list, num_ports)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    ingress_port = port_list[0];

    bcm_pbmp_t  pbmp;
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);
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

    /* Setup a IFP rule with field compression with Flex Counters verfication */
    rv = ifpCompressionSetup(unit);
    if (BCM_FAILURE(rv)) {
        printf("ifpCompressionSetup() FAILED: %s\n", bcm_errmsg(rv));
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
    /* select a random packet number from 0-5 */
    sal_srand(sal_time_usecs());
    int count = sal_rand() % 5;
    if (count == 0) {
      count = 1;
    }

    bshell(unit, "fp show");
    bshell(unit, "sleep quiet 1");
    printf("\nSending %d tagged test packets to ingress port:%d\n", (2*count)+1, ingress_port);
    /* Send test packets with IPv6 ether type 0x86DD and TTL value 255 */
    snprintf(str, 512, "tx %d pbm=%d data=0x0011111111110000000122228100000586DD63355555002606FF3FFE00000000000000000000000000013FFE00000000000000000000000000023030313100003232000033335010778857DB0000111111111111111111111111111111111111", count+1, ingress_port);
    bshell(unit, str);
    bshell(unit, "sleep quiet 1");
    /* Send test packets with IPv6 ether type 0x86DD and TTL value 05 */
    snprintf(str, 512, "tx %d pbm=%d data=0x0011111111110000000122228100000586DD63355555002606053FFE00000000000000000000000000013FFE00000000000000000000000000023030313100003232000033335010778857DB0000111111111111111111111111111111111111", count, ingress_port);
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

    return BCM_E_NONE;
}


/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in ifpCompressionSetup())
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
