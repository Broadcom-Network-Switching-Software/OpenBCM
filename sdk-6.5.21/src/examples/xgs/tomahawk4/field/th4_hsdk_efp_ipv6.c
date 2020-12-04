/*
 * Feature  : Egress field processor group with new key types.
 *
 * Usage    : BCM.0> c th4_hsdk_efp_ipv6.c
 *
 * config   : bcm56990_a0-generic-64x400.config.yml
 *
 * Log file : th4_hsdk_efp_ipv6_log.txt
 *
 * Test Topology :
 *                              CPU
 *                                ^
 *                                |  Rx reason : CPU_SFLOW
 *                                |
 *                      +---------+---------+
 *                      |        TH4        |
 *      ingress port    |      +-----+      |  egress port
 *       in LoopBack    |      | EFP |      |  in LoopBack
 *           ---------->+      | RULE|      +------>
 *     ingress VLAN 5   |      +-----+      |  egress VLAN 10
 *                      |                   |
 *                      +-------------------+
 *
 *
 * Summary:
 * ========
 * Tomahawk4 EFP design uses enhanced bcm_field_group_config_t structure to determine
 * EFP entry key at the group creation to be compatible with legacy SDK behaviour and
 * provides flexibility of choosing the different packet types for groups in a TCAM slice,
 * in other words for TH4 EFP three main parameters group_mode, mode_type and packet_type
 * will determine the EFP entry key. This cint example exercises simple EFP rule to
 * filter IPv6 packets in EFP stage.
 *
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in testSetup()):
 *   =============================================
 *     a) Select a pair of test ports and add them in ingress vlan then
 *        configure them in Loopback mode.
 *     b) Start packet watcher and register additional custom Rx callback for the
 *        packet verification.
 *     c) Enable ingress sampling on the egress port with maximum rate with CPU as
 *        destination.
 *
 *   2) Step2 - EFP Configuration (Done in efpIpv6Setup()):
 *   =========================================================
 *     a) Select a group mode type and packet type which supports required qsets
 *        and check if all the qualifiers are supported in the selected mode.
 *     b) Create an EFP group in Auto mode, selected packet and group mode type to filter
 *        on IPv6 header fields and egress port.
 *     c) Create an EFP entry and add qualifier values with action as to assign a new
 *        OuterVlan.
 *     d) In flexCounterSetup(), Add flex counters to the EFP entry solely for
 *        verification purpose.
 *     e) install the EFP entry in hardware.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the outputs of 'fp show' 'show c'
 *     b) Expected Result:
 *     ===================
 *     The CPU packet capture shows the ingress IPv6 packet copied by EFP rule as below
 *
 *   Ingress Packet:
 *   [bcmPWN.0]Packet[x]: Total x
 *   [bcmPWN.0]Packet[x]: data[0000]: {001111111111} {000000012222} 8100 000A
 *   [bcmPWN.0]Packet[x]: data[0010]: 86dd 6335 5555 0026 06ff 3ffe 0000 0000
 *   [bcmPWN.0]Packet[x]: data[0020]: 0000 0000 0000 0000 0001 3ffe 0000 0000
 *   [bcmPWN.0]Packet[x]: data[0030]: 0000 0000 0000 0000 0002 3030 3131 0000
 *   [bcmPWN.0]Packet[x]: data[0040]: 3232 0000 3333 5010 7788 57db 0000 1111
 *   [bcmPWN.0]Packet[x]: data[0050]: 1111 1111 1111 1111 1111 1111 1111 1111
 *   [bcmPWN.0]
 *   [bcmPWN.0][RX metadata information]
 *   ...
 *   ING_TAG_TYPE=1
 *   [RX reasons]
 *   CPU_SFLOW
 *   CPU_SFLOW_CPU_SFLOW_SRC
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t ingress_port;
bcm_port_t egress_port;
bcm_field_hintid_t hint_id;
uint32 stat_counter_id;
bcm_vlan_t ing_vlan = 5;
bcm_field_entry_t entry;
bcm_field_group_config_t group_config;
bcm_field_flexctr_config_t flexctr_cfg;
int test_failed = 0; /* Final result will be stored here */
int rx_count = 0; /* Total test packet received by CPU for verification */


  /* Qualifier set for EFP TCAM */
bcm_field_qualify_t efp_qset[] = {
    bcmFieldQualifyStageEgress,
    bcmFieldQualifySrcIp6High,
    bcmFieldQualifyDstIp6High,
    bcmFieldQualifyOutPort,
    bcmFieldQualifyIp6NextHeader,
    bcmFieldQualifyIp6TrafficClass,
    bcmFieldQualifyL4Ports,
    bcmFieldQualifyIpType,
    bcmFieldQualifyTcpControl,
    bcmFieldQualifyIpFrag,
    bcmFieldQualifyInnerVlanId,
    -1
};

/* EFP configuration structure */
struct fp_cfg_t{
    bcm_ip6_t src_ip;           /* Src Ipv6 to qualify in EFP */
    bcm_ip6_t src_ip_mask;      /* Ipv6 Mask to qualify in EFP */
    bcm_ip6_t dst_ip;           /* Dst Ipv6 to qualify in EFP */
    bcm_ip6_t dst_ip_mask;      /* Ipv6 Mask to qualify in EFP */
    bcm_port_t outport;         /* Egress port of the packet */
    bcm_port_t outport_mask;    /* Egress port mask of the packet */
    uint8 next_header;          /* L4 header */
    uint8 next_header_mask;     /* L4 header mask */
    uint8 traffic_class;        /* Traffic Class */
    uint8 traffic_class_mask;   /* Traffic Class mask */
    uint8 l4_valid;             /* Valid L4 src port and dst port bit */
    uint8 l4_valid_mask;        /* Valid L4 src port and dst port bit mask */
    bcm_field_IpType_t ip_type; /* Ip type */
};

/* Set the reguired EFP Configuration here */
fp_cfg_t fp_cfg = {
    /* Upper 64 bits of SrcIpv6 3FFE::1 */
    {0x3F, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01},
    /* Upper 64 bits of SrcIpv6 FFFF:FFFF:FFFF:FFFF:0:0:0:0 */
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /* Upper 64 bits of DstIpv6 3FFE::2 */
    {0x3F, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02},
    /* Upper 64 bits of DstIpv6 FFFF:FFFF:FFFF:FFFF:0:0:0:0 */
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    -1 ,                    /* OutPort placeholder - egress_port */
    0xFF,                   /* OutPort mask - 0xFF */
    0x6,                    /* L4 header - 0x6 [TCP]*/
    0xFF,                   /* L4 header mask - 0xFF */
    0x33,                   /* Traffic Class  - 0x33 */
    0xFF,                   /* Traffic Class mask - 0xFF */
    0x1,                    /* L4 valid bit - 0x1 */
    0xFF,                   /* L4 valid bit mask - 0xFF */
    bcmFieldIpTypeIpv6      /* TpType - Ipv6 */
};

/* Expected Egress packet */
unsigned char expected_packet[96] = {
    0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x01, 0x22, 0x22, 0x81, 0x00, 0x00, 0x0A,
    0x86, 0xDD, 0x63, 0x35, 0x55, 0x55, 0x00, 0x26, 0x06, 0xFF, 0x3F, 0xFE, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x3F, 0xFE, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x30, 0x30, 0x31, 0x31, 0x00, 0x00,
    0x32, 0x32, 0x00, 0x00, 0x33, 0x33, 0x50, 0x10, 0x77, 0x88, 0x57, 0xDB, 0x00, 0x00, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11
 };

bcm_error_t
efpIpv6Setup(int unit, fp_cfg_t *efp_cfg, bcm_field_qualify_t * qset) {

    int rv = 0;
    bcm_field_group_mode_type_t gmode = bcmFieldGroupModeTypeL3Double;
    bcm_field_group_packet_type_t ptype = bcmFieldGroupPacketTypeIp6;
    bcm_field_qset_t tqset;

    /* Read qualifiers available in the selected gmode and ptype. */
    rv = bcm_field_group_mode_qset_get(unit, gmode, ptype, &tqset);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_group_mode_qset_get() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);

    /* Add only supported qualifiers in group_config */
    while (*qset != -1) {
        if (BCM_FIELD_QSET_TEST(tqset, *qset)) {
            BCM_FIELD_QSET_ADD(group_config.qset, *qset);
        }else {
            printf("Unsupported qualifier:"); print *qset;
        }
        qset++;
    }

    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionOuterVlanNew);
    group_config.mode = bcmFieldGroupModeAuto;
    group_config.priority = 1;
    /* EFP Groups with flex counter support and doesn't need autoexpansion then
       user need to add flag BCM_FIELD_GROUP_CREATE_AUTO_EXPANSION_DISABLE in group
       configuration */
    group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_MODE |
                          BCM_FIELD_GROUP_CREATE_AUTO_EXPANSION_DISABLE;
    group_config.mode_type = gmode;
    group_config.packet_type = ptype;

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

    rv = bcm_field_qualify_OutPort(unit, entry, egress_port, efp_cfg->outport_mask);
    rv |= bcm_field_qualify_IpType(unit, entry, efp_cfg->ip_type);
    rv |= bcm_field_qualify_DstIp6High(unit, entry, efp_cfg->dst_ip,
                                                    efp_cfg->dst_ip_mask);
    rv |= bcm_field_qualify_SrcIp6High(unit, entry, efp_cfg->src_ip,
                                                    efp_cfg->src_ip_mask);
    rv |= bcm_field_qualify_L4Ports(unit, entry, efp_cfg->l4_valid, efp_cfg->l4_valid_mask);
    rv |= bcm_field_qualify_Ip6NextHeader(unit, entry, efp_cfg->next_header,
                                                       efp_cfg->next_header_mask);
    rv |= bcm_field_qualify_Ip6TrafficClass(unit, entry, efp_cfg->traffic_class,
                                                      efp_cfg->traffic_class_mask);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_qualify_*() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_action_add(unit, entry, bcmFieldActionOuterVlanNew, 10, 0xFFF);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_action_add() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Setup Flex Counters to verify EFP rule hits */
    rv = flexCounterSetup(unit, entry, group_config.group);
    if (BCM_FAILURE(rv)) {
        printf("flexCounterSetup() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Commit the EFP entry */
    rv = bcm_field_entry_install(unit, entry);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_install() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;

}

/* Create EFP flex counter action */
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
    /* Group ID passed as hint and EFP as source */
    action.hint = group;
    action.hint_type = bcmFlexctrHintFieldGroupId;
    action.source = bcmFlexctrSourceEfp;
    action.mode = bcmFlexctrCounterModeNormal;
    action.index_num = 2;
    action.drop_count_mode=bcmFlexctrDropCountModeAll;

    /* Counter index is EFP object. */
    index_op = &action.index_operation;
    index_op->object[0] = bcmFlexctrObjectStaticEgrFieldStageEgress;
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

    /* Create an egress action. */
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

/* port configuration function */
bcm_error_t
portSetup(int unit) {

    bcm_error_t rv = BCM_E_NONE;

    /* Create the ingress vlan */
    rv = bcm_vlan_create(unit, ing_vlan);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_vlan_create: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Add ing port to ingress vlan*/
    rv = vlan_add_port(unit, ing_vlan, ingress_port, 0);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_add_port: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Add egr port to ingress vlan*/
    rv = vlan_add_port(unit, ing_vlan, egress_port, 0);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_add_port: %s\n", bcm_errmsg(rv));
        return rv;
    }

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

    /* Ingress sampling to CPU with maximum rate */
    rv = bcm_port_sample_rate_set (unit, egress_port, 1, 0);
    if (BCM_FAILURE(rv)) {
        printf("bcm_port_sample_rate_set() failed. %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Setup a EFP rule with field extraction hint with Flex Counters verfication */
    rv = efpIpv6Setup(unit, &fp_cfg, efp_qset);
    if (BCM_FAILURE(rv)) {
        printf("efpIpv6Setup() FAILED: %s\n", bcm_errmsg(rv));
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
    printf("\nSending %d tagged test packets to ingress port:%d\n", count, ingress_port);
    snprintf(str, 512, "tx %d pbm=%d data=0x0011111111110000000122228100000586DD63355555002606FF3FFE00000000000000000000000000013FFE00000000000000000000000000023030313100003232000033335010778857DB0000111111111111111111111111111111111111", count, ingress_port);
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
 *  b) actual configuration (Done in efpIpv6Setup())
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
