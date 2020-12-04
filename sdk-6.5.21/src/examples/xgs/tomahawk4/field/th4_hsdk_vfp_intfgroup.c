/*
 * Feature  : VLAN/VFI assignment for interface/port groups using VFP rule
 *
 * Usage    : BCM.0> c th4_hsdk_vfp_intfgroup.c
 *
 * config   : bcm56990_a0-generic-64x400.config.yml
 *
 * Log file : th4_hsdk_vfp_intfgroup_log.txt
 *
 * Test Topology :
 *                              CPU
 *                                ^
 *                                |  Rx reason : CPU_SFLOW*
 *                                |
 *                      +---------+---------+
 *                      |        TH4        |
 *   ingress port       |  +----+   +-----+ |   egress port in LB
 *   in LoopBack        |  |VFP |   | L2  | |   with ing-sampler
 *           ---------->+  |RULE|   |entry| + ------>
 *     ingress VFI xx   |  +----+   +-----+ |   VFI 200
 *                      |                   |   CFI 1
 *                      +-------------------+   PRI 7
 *
 *
 * Summary:
 * ========
 * cint script to demonstrate VLAN/VFI assignment for a specific portgroup using interface
 * class id as a qualifier in the VFP rule.
 *
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in testSetup()):
 *   =============================================
 *     a) Selects two random ports and configure them in Loopback mode. Out of these
 *        two ports, one port is used as ingress_port and the other as
 *        egress_port.
 *     b) Start packet watcher and register additional custom Rx callback for the
 *        packet verfication.
 *     c) In function l2Setup(), the ports are configured in a ingress VLAN/VFI and
 *        L2 static entry is setup for unicast forwarding with known VLAN/VFI value.
 *     d) Enable ingress sampling on the egress port with maximum rate with CPU as
 *        destination
       e) Assign the InterfaceClass id to selected ingress port
 *
 *   2) Step2 - VFP Configuration (Done in vfpSetup()):
 *   =========================================================
 *     a) A VFP group is created with InterfaceClass as a qualifier
 *     b) Create a VFP group entry to filter on InterfaceClass of the port with new
 *        outer VLAN/VFI, CFI, PRI assignments as actions
 *     c) In flexCounterSetup(), Add flex counters to the VFP entry solely for
 *        verfication purpose
 *     d) install the VFP entry in hardware
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the outputs of 'fp show' 'show c' 'l2 show'
 *     b) on the egress port SFLOW counters are incremented and static l2 entry is HIT
 *     c) Expected Result:
 *     ===================
 *     The CPU packet capture shows the Egress packet copied by egress port sampler with
 *     newly assigned outer VLAN, CFI and priority value
 *
 *   Egress Packet:
 *   [bcmPWN.0]
 *   [bcmPWN.0]Packet[1]: Total x
 *   [bcmPWN.0]Packet[1]: data[0000]: {0000000000dd} {000000000202} 8100 f0c8
 *   [bcmPWN.0]Packet[1]: data[0010]: 0800 4500 002e 0000 0000 4006 3e8c 0a0a
 *   [bcmPWN.0]Packet[1]: data[0020]: 0a0a 0a0a 0a0a 227d 0050 0000 0000 0000
 *   [bcmPWN.0]Packet[1]: data[0030]: 0000 5000 0000 4aca 0000 0001 0203 0405
 *   [bcmPWN.0]Packet[1]: data[0040]: b0f9 eb4f
 *   [bcmPWN.0]
 *   [bcmPWN.0][RX metadata information]
 *   	PKT_LENGTH=0x48
 *   	SRC_PORT_NUM=0xdf
 *   	OUTER_VID=0xc8
 *   	OUTER_CFI=1
 *   	OUTER_PRI=7
 *    ...
 *   [RX reasons]
 *   	CPU_SFLOW
 *   	CPU_SFLOW_CPU_SFLOW_SRC
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t egress_port;
bcm_port_t ingress_port;
bcm_gport_t gport;
uint32 stat_counter_id;
uint32 class = 511;
bcm_field_entry_t entry;
bcm_vlan_t ingress_vlan = 2, egress_vlan;
bcm_field_group_config_t group_config;
bcm_field_flexctr_config_t flexctr_cfg;
bcm_mac_t dst_mac = "00:00:00:00:00:dd";
int test_failed = 0; /* Final result will be stored here */
int rx_count = 0; /* Total test packet received by CPU for verification */

/* Expected Egress packet attributes */
uint32 expected_packet_attr[3] = { 7,  /* new priority */
                                   1,  /* new CFI */
                                   200 /* new outer vlan*/
                                 };

egress_vlan = expected_packet_attr[2];

/* Interface class grouping VFP rule */
bcm_error_t
vfpSetup(int unit, int classid) {

    int rv = 0;

    /* Configure IFP group in Auto mode with per port interface class
    as one of the qualifiers */
    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageLookup);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInterfaceClassPort);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionOuterVlanNew);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionOuterVlanPrioNew);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionOuterVlanCfiNew);
    group_config.priority = BCM_FIELD_GROUP_PRIO_ANY;
    group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_MODE;
    group_config.mode = bcmFieldGroupModeAuto;

    rv =  bcm_field_group_config_create(unit, &group_config);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_group_config_create() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv =  bcm_field_entry_create(unit, group_config.group, &entry);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_create() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_qualify_InterfaceClassPort(unit, entry, classid, 0x1ff);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_qualify_InterfaceClassPort() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Peplace outer tag attributes for the test packets hitting this rule */
    rv = bcm_field_action_add(unit,entry,bcmFieldActionOuterVlanNew, expected_packet_attr[2], 0);
    rv |= bcm_field_action_add(unit,entry,bcmFieldActionOuterVlanPrioNew, expected_packet_attr[0], 0);
    rv |= bcm_field_action_add(unit,entry,bcmFieldActionOuterVlanCfiNew,expected_packet_attr[1],0);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_action_add() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }


    /* Setup Flex Counters to verify VFP rule hits */
    rv = flexCounterSetup(unit, entry, group_config.group);
    if (BCM_FAILURE(rv)) {
        printf("flexCounterSetup() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Write VFP entry in hardware */
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

    print bcm_flexctr_action_t_init(&action);
    action.flags = 0;
    /* Group ID passed as hint */
    action.hint = group;
    action.hint_type = bcmFlexctrHintFieldGroupId;
    action.source = bcmFlexctrSourceVfp;
    action.mode = bcmFlexctrCounterModeNormal;
    action.index_num = 2;

    /* Counter index is PKT_ATTR_OBJ0. */
    index_op = &action.index_operation;
    index_op->object[0] = bcmFlexctrObjectStaticIngFieldStageLookup;
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

/* Configures the port in loopback mode */
bcm_error_t
port_lb_setup(int unit, bcm_pbmp_t pbm) {

    bcm_port_t     port;
    BCM_PBMP_ITER(pbm, port) {
        printf("port_lb_setup port: %d setup\n", port);
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
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

/* L2 switch configuration function */
bcm_error_t
l2Setup(int unit, bcm_vlan_t ing_vlan, bcm_vlan_t egr_vlan) {

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

    /* Create the egress vlan */
    rv = bcm_vlan_create(unit, egr_vlan);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_vlan_create: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Add egr port to egress vlan*/
    rv = vlan_add_port(unit, egr_vlan, egress_port, 0);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_add_port: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Add ing port to egress vlan*/
    rv = vlan_add_port(unit, egr_vlan, ingress_port, 0);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_add_port: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Setup identity mapping of VLAN_ID to VFI at ingress port */
    /* it is enabled by default in the switch */
    rv = bcm_port_control_set(unit, ingress_port, bcmPortControlTrustIncomingVlan,1);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_port_control_set: %s\n", bcm_errmsg(rv));
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

    /* Configure static l2 entry for forwarding */
    rv = add_l2_static_entry(unit,0,dst_mac, egr_vlan,egress_port);
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
    int result_ovid_check = -1;
    int result_cfi_check = -1;
    int result_pri_check = -1;
    uint32  ovid, cfi, pri;
    void  *buffer;
    uint32  length;

    (*count)++; /* Bump received packet count */

    /* Get outer vlan metadata */
    BCM_IF_ERROR_RETURN(bcm_pktio_pmd_field_get
                  (unit, packet, bcmPktioPmdTypeRx,
                   BCM_PKTIO_RXPMD_OUTER_VID, &ovid));

    if(ovid == expected_packet_attr[2]) {
        result_ovid_check = 0;
    }

    /* Get vlan tag CFI value */
    BCM_IF_ERROR_RETURN(bcm_pktio_pmd_field_get
                  (unit, packet, bcmPktioPmdTypeRx,
                   BCM_PKTIO_RXPMD_OUTER_CFI, &cfi));

    if(cfi == expected_packet_attr[1]) {
        result_cfi_check = 0;
    }

    /* Get vlan tag priority value */
    BCM_IF_ERROR_RETURN(bcm_pktio_pmd_field_get
                  (unit, packet, bcmPktioPmdTypeRx,
                   BCM_PKTIO_RXPMD_OUTER_PRI, &pri));

    if(pri == expected_packet_attr[0]) {
        result_pri_check = 0;
    }

    test_failed = test_failed || (result_ovid_check != 0) || (result_cfi_check != 0) || (result_pri_check != 0);

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
 * This functions gets the port numbers and sets up ingress and
 * egress ports and neccessary testsetup.
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

    /* Layer2 test setup */
    rv = l2Setup(unit, ingress_vlan, egress_vlan);
    if (BCM_FAILURE(rv)) {
        printf("l2Setup() failed. %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Ingress sampling to CPU with maximum rate */
    rv = bcm_port_sample_rate_set (unit, egress_port, 1, 0);
    if (BCM_FAILURE(rv)) {
        printf("bcm_port_sample_rate_set() failed. %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* 9-bit interface class id will mark ingress traffic eligible for the VFP rule */
    rv = bcm_port_class_set(unit, ingress_port, bcmPortClassFieldLookup, class);
    if (BCM_FAILURE(rv)) {
        printf("bcm_port_class_set() failed. %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Setup a VFP rule with class qualifier and Flex Counters, action to replace outer vlan */
    rv = vfpSetup(unit, class);
    if (BCM_FAILURE(rv)) {
        printf("vfpSetup() FAILED: %s\n", bcm_errmsg(rv));
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
    /* select a random packet number from 0-10 */
    int count = sal_rand() % 10;
    if (count == 0) {
      count = 1;
    }

    bshell(unit, "vlan show");
    bshell(unit, "fp show");
    bshell(unit, "sleep quiet 1");
    printf("\nSending %d tagged test packets to ingress port:%d\n", count, ingress_port);
    /* Send test packets with the DMAC to ingress_port */
    snprintf(str, 512, "tx %d pbm=%d data=0x0000000000DD0000000002028100000208004500002E0000000040063E8C0A0A0A0A0A0A0A0A227D00500000000000000000500000004ACA0000000102030405B0F9EB4F", count, ingress_port);
    bshell(unit, str);
    bshell(unit, "sleep quiet 3");
    bshell(unit, "l2 show");
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

    /* print flex counter value. */
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


/* Cleanup test setup */
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

    /* Destroy flex counter action. */
    rv = bcm_flexctr_action_destroy(unit, stat_counter_id);
    if (BCM_FAILURE(rv)) {
       printf("bcm_flexctr_action_destroy %s\n", bcm_errmsg(rv));
       return rv;
    }

    return BCM_E_NONE;
}


/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual FP configuration (Done in vfpSetup())
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
