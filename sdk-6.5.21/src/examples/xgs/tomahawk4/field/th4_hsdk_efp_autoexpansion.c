/*
 * Feature  : Egress field processor group autoexpansion with flex counters.
 *
 * Usage    : BCM.0> c th4_hsdk_efp_autoexpansion.c
 *
 * config   : bcm56990_a0-generic-64x400.config.yml
 *
 * Log file : th4_hsdk_efp_autoexpansion_log.txt
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
 *     ingress VLAN x   |      +-----+      |  egress VLAN (x + 0x200)
 *                      |                   |
 *                      +-------------------+
 *
 *
 * Summary:
 * ========
 * Tomahawk4 A0/B0/G0 device's EFP stage has per slice EFP flex counters support so
 * EFP (flex)counters are tightly coupled with TCAM Physical slices and for autoexpansion
 * usecase it needs additional configutations with flex counter hint to create and share
 * a flex counter pool by all the EFP tcam entries. This cint example exercises
 * EFP group entries to create EFP rules in all four EFP slices.
 *
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in testSetup()):
 *   =============================================
 *     a) Select a pair of test ports and add them in ingress test vlan pool 2-511
 *        then configure them in Loopback mode.
 *     b) Start packet watcher and register additional custom Rx callback for the
 *        packet verification.
 *     c) Enable ingress sampling on the egress port with maximum rate with CPU as
 *        destination.
 *
 *   2) Step2 - EFP Configuration (Done in efpIpv6Setup()):
 *   =========================================================
 *     a) Create four flex counter actions (one for each EFP slice) with a pool of
 *        2048 counters, this shared counter pool will be updated by EFP entries.
 *     b) Create a FP hint of type GroupExpandFlexCtrAction for autoexpandable fp group.
 *     b) Select a group mode type and packet type which supports required qsets
 *        and check if all the qualifiers are supported in the selected mode.
 *     c) Create an EFP group with above FP hint in Auto mode, selected packet
 *        and group mode type to filter on ingress vlan and egress port.
 *     d) Create an EFP entry and add qualifier values with action as to assign a new
 *        OuterVlan (0x200 + ingress vlan).
 *     e) In flexCounterSetup(), Add flex counters to the EFP entry solely for
 *        verification purpose.
 *     f) install the EFP entry in hardware.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the outputs of 'fp show brief' 'show c'
 *     b) Expected Result:
 *     ===================
 *     The CPU packet capture shows the egressed packet sampled by ingress sampler
 *     of test port with modified outer vlan by the EFP rule (0x200 + ingress vlan)
 *     as below
 *
 *   Ingress Packet:
 *   [bcmPWN.0]
 *   [bcmPWN.0]Packet[x]: Total x
 *   [bcmPWN.0]Packet[x]: data[0000]: {001111111111} {000000012222} 8100 (0x200+yy)
 *   [bcmPWN.0]Packet[x]: data[0010]: 86dd 6335 5555 0026 06ff 3ffe 0000 0000
 *   [bcmPWN.0]Packet[x]: data[0020]: 0000 0000 0000 0000 0001 3ffe 0000 0000
 *   [bcmPWN.0]Packet[x]: data[0030]: 0000 0000 0000 0000 0002 3030 3131 0000
 *   [bcmPWN.0]Packet[x]: data[0040]: 3232 0000 3333 5010 7788 57db 0000 1111
 *   [bcmPWN.0]Packet[x]: data[0050]: 1111 1111 1111 1111 1111 1111 1111 1111
 *   [bcmPWN.0]
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
uint32 num_actions = 4;
bcm_field_hintid_t hint_id;
uint32 stat_counter_id[num_actions];
bcm_vlan_t vlan;
bcm_field_entry_t entry;
bcm_field_entry_t eid[512];
bcm_field_group_config_t group_config;
bcm_field_flexctr_config_t flexctr_cfg;
bcm_flexctr_action_t action[num_actions];
int test_failed = 0; /* Final result will be stored here */
int rx_count = 0; /* Total test packet received by CPU for verification */

  /* Qualifier set for EFP TCAM */
bcm_field_qualify_t efp_qset[] = {
    bcmFieldQualifyStageEgress,
    bcmFieldQualifyOutPort,
    bcmFieldQualifyOuterVlanId,
    -1
};

bcm_error_t
efpAutoExpSetup(int unit, bcm_field_qualify_t * qset ) {

    int rv, i = 0;
    bcm_field_group_mode_type_t gmode = bcmFieldGroupModeTypeL2Single;
    bcm_field_group_packet_type_t ptype = bcmFieldGroupPacketTypeDefault;
    bcm_field_qset_t tqset;
    bcm_field_hint_t hint;
    bcm_field_hintid_t hint_id ;

    /* Setup 4 Flex Counters actions for each virtual slice [0-3] */
    rv = flexCounterSetup(unit, entry, group_config.group);
    if (BCM_FAILURE(rv)) {
        printf("flexCounterSetup() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* EFP flex counter hint for autoexpandable fp group  */
    bcm_field_hint_t_init(&hint);
    rv = bcm_field_hints_create(unit, &hint_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_hints_create() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }
    hint.hint_type = bcmFieldHintTypeGroupExpandFlexCtrAction;
    for (i = 0; i < num_actions ; i++) {
        hint.value = stat_counter_id[i];
        rv = bcm_field_hints_add(unit, hint_id, &hint);
        if (BCM_FAILURE(rv)) {
            printf("bcm_field_hints_add() FAILED: %s\n", bcm_errmsg(rv));
            return rv;
        }
    }

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

    /* Create autoexpandable fp group with action to change outgoing vlan */
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionOuterVlanNew);
    group_config.mode = bcmFieldGroupModeAuto;
    group_config.priority = 1;
    group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_MODE;
    group_config.mode_type = gmode;
    group_config.packet_type = ptype;
    group_config.hintid = hint_id;

    rv = bcm_field_group_config_create(unit, &group_config);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_group_config_create() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create FP entries to utilize almost all EFP tcam entries */
    for ( i = 2; i < 512; i++) {

        rv = bcm_field_entry_create(unit, group_config.group, &entry);
        if (BCM_FAILURE(rv)) {
            printf("entry%X:bcm_field_entry_create() FAILED: %s\n", entry, bcm_errmsg(rv));
            return rv;
        }
        /* Record entry id for further entry management */
        eid[i] = entry;

        /* Qualify on egress vlan range [2-511] and egress port */
        rv = bcm_field_qualify_OutPort(unit, entry, egress_port, egress_port);
        /* One to one mapping between qualifying vlan and entry offset to test
        entries from all the physical slices */
        rv |= bcm_field_qualify_OuterVlanId(unit, entry, i, 0xFFF);
        if (BCM_FAILURE(rv)) {
            printf("entry%X:bcm_field_qualify_*() FAILED: %s\n", entry, bcm_errmsg(rv));
            return rv;
        }

        /* change outer vlan to deterministic value (+200) for verification */
        rv = bcm_field_action_add(unit, entry, bcmFieldActionOuterVlanNew, (i+0x200), 0xFFF);
        if (BCM_FAILURE(rv)) {
            printf("entry%X:bcm_field_action_add() FAILED: %s\n", entry, bcm_errmsg(rv));
            return rv;
        }

        flexctr_cfg.flexctr_action_id = stat_counter_id[0];
        /* Use shared pool counter offset to the same value of qualifying outervlan */
        flexctr_cfg.counter_index = i;
        rv =  bcm_field_entry_flexctr_attach(unit, entry, &flexctr_cfg);
        if (BCM_FAILURE(rv)) {
            printf("entry%X:bcm_field_entry_flexctr_attach() FAILED: %s\n", entry, bcm_errmsg(rv));
            return rv;
        }

        /* Commit the EFP entry */
        rv = bcm_field_entry_install(unit, entry);
        if (BCM_FAILURE(rv)) {
            printf("entry%X:bcm_field_entry_install() FAILED: %s\n", entry, bcm_errmsg(rv));
            return rv;
        }
    }
    return BCM_E_NONE;
}

/* Create EFP flex counter actions */
bcm_error_t
flexCounterSetup(int unit, int eid, int group)  {

    int options, i = 0;
    bcm_error_t  rv;
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    bcm_flexctr_value_operation_t *value_b_op = NULL;
    bcm_flexctr_block_t blk;
    uint32_t blk_id;

    /* Create a block with 2048 counters for all pipes */
    bcm_flexctr_block_t_init(&blk);
    blk.stage = bcmFlexctrDirectionEgress;
    blk.num_ctrs = 2048;
    BCM_PBMP_CLEAR(blk.ports);

    rv = bcm_flexctr_block_create(unit, &blk, &blk_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_flexctr_block_create: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Create one flexctr action each for
    EFP virtual slice 0-3 */
    for (i = 0; i < 4;i++ ) {

        bcm_flexctr_action_t_init(&action[i]);
        action[i].flags = 0;
        /* Use the pre-allocated block */
        action[i].hint_type = bcmFlexctrHintSharedBlock;
        action[i].hint = blk_id;
        action[i].hint_ext = 0;
        action[i].source = bcmFlexctrSourceEfp;
        action[i].mode = bcmFlexctrCounterModeNormal;
        action[i].index_num = 2048;
        /* To count drop packets also */
        action[i].drop_count_mode = bcmFlexctrDropCountModeAll;

        index_op = &action[i].index_operation;
        index_op->object[0] = bcmFlexctrObjectStaticEgrFieldStageEgress;
        index_op->object_id[0] = i; /* EFP vslice 0-3 */
        index_op->mask_size[0] = 15;
        index_op->shift[0] = 0;

        /* Increase counter per packet. */
        value_a_op = &action[i].operation_a;
        value_a_op->select = bcmFlexctrValueSelectCounterValue;
        value_a_op->object[0] = bcmFlexctrObjectConstOne;
        value_a_op->mask_size[0] = 15;
        value_a_op->shift[0] = 0;
        value_a_op->object[1] = bcmFlexctrObjectConstZero;
        value_a_op->mask_size[1] = 1;
        value_a_op->shift[1] = 0;
        value_a_op->type = bcmFlexctrValueOperationTypeInc;

        /* Increase counter per packet bytes. */
        value_b_op = &action[i].operation_b;
        value_b_op->select = bcmFlexctrValueSelectPacketLength;
        value_b_op->type = bcmFlexctrValueOperationTypeInc;

        /* Create an egress action */
         rv = bcm_flexctr_action_create(unit, options, &action[i], &stat_counter_id[i]);
         if (BCM_FAILURE(rv)) {
             printf("\nError in action%d:bcm_flexctr_action_create: %s.\n", i, bcm_errmsg(rv));
             return rv;
         }

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
    bcm_port_t port;
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
    int i;

    /* select a test vlan from vlan range 2-511 */
    vlan = sal_rand() % 511;
    if (vlan <= 1) {
        vlan = 2;
    }

    /* Create the ingress vlans */
    for ( i = 2; i < 512 ; i++ ) {
        rv = bcm_vlan_create(unit, i);
        if (BCM_FAILURE(rv)) {
            printf("Error in bcm_vlan_create: %s\n", bcm_errmsg(rv));
            return rv;
        }

        /* Add ing port to ingress vlans */
        rv = vlan_add_port(unit, i, ingress_port, 0);
        if (BCM_FAILURE(rv)) {
            printf("\nError in vlan_add_port: %s\n", bcm_errmsg(rv));
            return rv;
        }

        /* Add egr port to ingress vlans */
        rv = vlan_add_port(unit, i, egress_port, 0);
        if (BCM_FAILURE(rv)) {
            printf("\nError in vlan_add_port: %s\n", bcm_errmsg(rv));
            return rv;
        }
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
    int result_ovid_check = -1;
    uint32  ovid;
    (*count)++; /* Bump received packet count */

    /* Get outer vlan metadata */
    BCM_IF_ERROR_RETURN(bcm_pktio_pmd_field_get
                  (unit, packet, bcmPktioPmdTypeRx,
                   BCM_PKTIO_RXPMD_OUTER_VID, &ovid));

    if((ovid - 0x200) == vlan) {
        result_ovid_check = 0;
    }

    test_failed = test_failed || (result_ovid_check != 0);

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
    rv = efpAutoExpSetup(unit, efp_qset);
    if (BCM_FAILURE(rv)) {
        printf("efpAutoExpSetup() FAILED: %s\n", bcm_errmsg(rv));
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
    uint32 counter_index = vlan;
    /* select a random packet number from 0-5 */
    sal_srand(sal_time_usecs());
    int count = sal_rand() % 5;
    if (count == 0) {
      count = 1;
    }

    bshell(unit, "fp show brief");
    bshell(unit, "sleep quiet 1");
    printf("\nSending %d test packets with OuterVlan %d to ingress port:%d\n", count, vlan, ingress_port);
    snprintf(str, 512, "tx %d pbm=%d data=0x00111111111100000001222281000%03X86DD63355555002606FF3FFE00000000000000000000000000013FFE00000000000000000000000000023030313100003232000033335010778857DB0000111111111111111111111111111111111111", count, ingress_port, vlan);
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
                stat_counter_id[0],
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
bcm_error_t testCleanup(int unit) {

    int rv, i;

    for ( i = 2; i < 512 ; i++ ) {

        /* Remove FP entry from hardware */
        rv = bcm_field_entry_remove(unit, eid[i]);
        if (BCM_FAILURE(rv)) {
            printf("entry%X:bcm_field_entry_remove() FAILED: %s\n", eid[i], bcm_errmsg(rv));
            return rv;
        }

        /* Detach the flex counters of the FP entry */
        rv = bcm_field_entry_flexctr_detach(unit, eid[i], &flexctr_cfg);
        if (BCM_FAILURE(rv)) {
            printf("entry%X:bcm_field_entry_flexctr_detach() FAILED: %s\n", eid[i], bcm_errmsg(rv));
            return rv;
        }

        /* Destroy the test FP entry */
        rv = bcm_field_entry_destroy(unit, eid[i]);
        if (BCM_FAILURE(rv)) {
            printf("entry%X:bcm_field_entry_destroy() FAILED: %s\n", eid[i], bcm_errmsg(rv));
            return rv;
        }
    }

    for ( i = 0; i < num_actions ; i++ ) {

        /* Destroy counter actions. */
        rv = bcm_flexctr_action_destroy(unit, stat_counter_id[i]);
        if (BCM_FAILURE(rv)) {
           printf("bcm_flexctr_action_destroy %s\n", bcm_errmsg(rv));
           return rv;
        }
    }

    /* Destroy IFP hint. */
    rv = bcm_field_hints_destroy(unit, hint_id);
    if (BCM_FAILURE(rv)) {
       printf("bcm_field_hints_destroy %s\n", bcm_errmsg(rv));
       return rv;
    }

    /* Destroy the EFP group */
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
 *  b) actual configuration (Done in efpAutoExpSetup())
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

    rv = testCleanup(unit);
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
