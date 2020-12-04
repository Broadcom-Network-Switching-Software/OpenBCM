/*
 * Feature  : IFP autoexpansion with ingress vlan metering and without flex counters.
 *
 * Usage    : BCM.0> c th4_hsdk_ifp_policer_autoexpansion.c
 *
 * config   : bcm56990_a0-generic-64x400.config.yml
 *
 * Log file : th4_hsdk_ifp_policer_autoexpansion_log.txt
 *
 * Test Topology :
 *                              CPU
 *                                ^
 *                                |  Rx reason : CPU_SFLOW *
 *                                |
 *                      +---------+---------+
 *                      |        TH4        |
 *   ingress port[0]    |                   |egress port[1]
 *    in loopback       |   +-----------+   |in loopback
 *           ---------->+   | IFP Rules |   +------>
 *      vlan  x         |   +-----------+   |
 *                      |      policer      |
 *                      |                   |
 *                      +-------------------+
 *
 *
 * Summary:
 * ========
 * Tomahawk4 A0 device's ifp (flex)counters support is tightly coupled with Physical
 * TCAM so these ifp counter are not available for autoexpandable group entries. The
 * non-expandable IFP groups can still use ifp (flex)counters. Note Tomahawk4B/G device ifp
 * does support autoexpansion with flex counters as it has global IFP flex counters.
 * This cint example creates ifp group entries in multiple ifp slices without IFP
 * counters, qualifing traffic on outer vlan and also metering some specific vlan traffic.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in testSetup()):
 *   =============================================
 *     a) Select a pair of test ports and add them in ingress test vlan pool 2-1023
 *        then configure them in Loopback mode.
 *     b) Start packet watcher and register additional custom Rx callback for the
 *        packet verification.
 *     c) Enable ingress sampling on egress test port with rate to capture sufficient
 *        remarked red test packets.
 *   2) Step2 - IFP Configuration (Done in ifpPolicerSetup()):
 *   =========================================================
 *     a) An IFP group is created with Outer vlan as one of the qualifier
 *        in Tripple wide mode with several color specific actions.
 *     b) Create an ifp entry then add a outer vlan qualifier value from vlan [2-1023]
 *        pool and if vlan or entry id is in multiple/modulo of 8 then Create and Attach
 *        a two rate three color policer, further the IFP policy will drop Green and
 *        Yellow packets to allow Red Packets with remarked outer priority of 4.
 *     c) Install the ifp entry in hardware. Create ~1024 such IFP entries with same
 *        ingress port but with different outer vlan. The spillover entries will occupy
 *        further available IFP TCAMs.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the outputs of 'fp show brief/entry' 'show c'
 *     b) Expected Result:
 *     ===================
 *     The CPU packet capture shows the remarked sampled packets as below
 *
 *   Egress Packet:
 *   [bcmPWN.0]Packet[y]: Total y
 *   [bcmPWN.0]Packet[y]: data[0000]: {000000000202} {000000000001} 8100 8xxx
 *
 *   [RX metadata information]
 *   ...
 *   [RX reasons]
 *   CPU_SFLOW
 */

/* Reset C interpreter*/
cint_reset();

int num_ports = 2;
int port_list[num_ports];
bcm_vlan_t vlan = -1;
bcm_field_entry_t entry;
bcm_field_group_config_t group_config;
bcm_policer_config_t policer_config;
bcm_policer_t pid;
int test_failed = 0; /* Final result will be stored here */
int rx_count = 0; /* Total test packet received by CPU for verification */

/* Qualifier set for IFP TCAM */
bcm_field_qualify_t ifp_qset[] = {
    bcmFieldQualifyStageIngress,
    bcmFieldQualifyIpProtocol,
    bcmFieldQualifySrcIp6,
    bcmFieldQualifyDstIp6,
    bcmFieldQualifyL4SrcPort,
    bcmFieldQualifyL4DstPort,
    bcmFieldQualifyOuterVlanId,
    bcmFieldQualifyIp6TrafficClass,
    bcmFieldQualifyIp6HopLimit,
    bcmFieldQualifyIp6NextHeader,
    bcmFieldQualifyTcpControl,
    bcmFieldQualifyExtensionHeaderType,
    bcmFieldQualifyRangeCheck,
    bcmFieldQualifyExtensionHeaderType,
    -1};

/* Policer IFP rule */
bcm_error_t
ifpPolicerSetup(int unit, bcm_field_qualify_t * qset) {

    int rv = 0, i;
    /* Choose non-overlapping bit rates for test validation */
    const int   base_rate = 10;
    const int   cir = base_rate * 8;
    const int   cbs = (base_rate + 12) * 8;
    const int   pir = (base_rate + 36) * 8;
    const int   pbs = (base_rate + 40) * 8;

    bcm_policer_config_t_init(&policer_config);
    policer_config.mode = bcmPolicerModeTrTcm;
    /* Two rate, three color policer ignoring previous color marking */
    policer_config.flags |= (BCM_POLICER_WITH_ID | BCM_POLICER_COLOR_BLIND);
    /* Committed Rate, below this rate, packets are green */
    policer_config.ckbits_sec = cir;
    policer_config.ckbits_burst = cbs;
    /* Peak Rate, below rate is yellow, above rate is red */
    policer_config.pkbits_sec = pir;
    policer_config.pkbits_burst = pbs;


    bcm_field_group_config_t_init(group_config);
    /* Add all qualifiers in group_config */
    while (*qset != -1) {
        BCM_FIELD_QSET_ADD(group_config.qset, *qset);
        qset++;
    }
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionGpDrop);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionYpDrop);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionRpPrioPktNew);

    group_config.mode = bcmFieldGroupModeTriple;
    group_config.group = (BCM_FIELD_IFP_ID_BASE | 10);
    group_config.flags |=  BCM_FIELD_GROUP_CREATE_WITH_MODE |
                           BCM_FIELD_GROUP_CREATE_WITH_ID |
                           BCM_FIELD_GROUP_CREATE_WITH_ASET;
    group_config.priority = 1;
    rv = bcm_field_group_config_create(unit, &group_config);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_group_config_create() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    for ( i = 2; i < 1024; i++) {

        entry = (BCM_FIELD_IFP_ID_BASE | i);
        rv = bcm_field_entry_create_id(unit, group_config.group, entry);
        if (BCM_FAILURE(rv)) {
            printf("entry%X:bcm_field_entry_create_id() FAILED: %s\n", entry, bcm_errmsg(rv));
            return rv;
        }

        /* Qualify packets based on ingress outer vlan. */
        rv = bcm_field_qualify_OuterVlanId(unit, entry, i, 0xFFF);
        if (BCM_FAILURE(rv)) {
            printf("entry%X:bcm_field_qualify_*() FAILED: %s\n", entry, bcm_errmsg(rv));
            return rv;
        }

        /* Attach policer only to the ifp rules with vlan or entry id in multiple of 8 */
        if (i%8 == 0) {
            pid = i/8;
            rv = bcm_policer_create(unit, &policer_config, &pid);
            if (BCM_FAILURE(rv)) {
                printf("entry%X:bcm_policer_create() FAILED: %s\n", entry, bcm_errmsg(rv));
                return rv;
            }

            /* Attach policer to the group entry. */
            rv = bcm_field_entry_policer_attach(unit, entry, 0, pid);
            if (BCM_FAILURE(rv)) {
                printf("entry%X:entry%X:bcm_field_entry_policer_attach() FAILED: %s\n", entry, bcm_errmsg(rv));
                return rv;
            }

            /* Drop Green and Yellow marked packets */
            rv = bcm_field_action_add(unit, entry, bcmFieldActionGpDrop, 0, 0);
            rv |= bcm_field_action_add(unit, entry, bcmFieldActionYpDrop, 0, 0);
            /* Mark red packets with new outer packet priority */
            rv |= bcm_field_action_add(unit, entry, bcmFieldActionRpPrioPktNew, 4, 0xFF);
            if (BCM_FAILURE(rv)) {
                printf("entry%X:bcm_field_action_add() FAILED: %s\n", entry, bcm_errmsg(rv));
                return rv;
            }

        }

        rv = bcm_field_entry_install(unit, entry);
        if (BCM_FAILURE(rv)) {
            printf("entry%X:bcm_field_entry_install() FAILED: %s\n", entry, bcm_errmsg(rv));
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
l2Setup(int unit) {

    bcm_error_t rv = BCM_E_NONE;
    int index = 0,i;

    /* select a test vlan with policer from vlan range 2-1023 */
    i = sal_rand() % 128;
    if (i <= 0) {
        i = 1;
    }
    /* only fp entries with id in multiple of 8 has policer */
    vlan = i * 8;
    /* Create the ingress vlans */
    for ( i = 2; i < 1024; i++ ) {
        rv = bcm_vlan_create(unit, i);
        if (BCM_FAILURE(rv)) {
            printf("Error in bcm_vlan_create: %s\n", bcm_errmsg(rv));
            return rv;
        }

        /* Add ing port to ingress vlans */
        rv = vlan_add_port(unit, i, port_list[0], 0);
        if (BCM_FAILURE(rv)) {
            printf("\nError in vlan_add_port: %s\n", bcm_errmsg(rv));
            return rv;
        }

        /* Add egr port to ingress vlans */
        rv = vlan_add_port(unit, i, port_list[1], 0);
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

    /* Enable port_list[1] sampler to verify egressing Red packets with priority 4 */
    rv = bcm_port_sample_rate_set (unit, port_list[1], 0xFFF, 0);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_port_sample_rate_set: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Rx callback function for applications using the HSDK PKTIO */
bcm_pktio_rx_t
pktioRxCallback(int unit, bcm_pktio_pkt_t * packet, void *cookie) {

    int  *count = (auto) cookie;
    int result_pri_check = -1;
    uint32  pri;

    (*count)++; /* Bump received packet count */

    /* Get outer priority metadata */
    BCM_IF_ERROR_RETURN(bcm_pktio_pmd_field_get
                  (unit, packet, bcmPktioPmdTypeRx,
                   BCM_PKTIO_RXPMD_OUTER_PRI, &pri));

    if(pri == 4) {
        result_pri_check = 0;
    }

    test_failed = test_failed || (result_pri_check != 0);

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

    rv = l2Setup(unit);
    if (BCM_FAILURE(rv)) {
        printf("l2Setup() failed. %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Setup a IFP rule without Flex Counters */
    rv = ifpPolicerSetup(unit, ifp_qset);
    if (BCM_FAILURE(rv)) {
        printf("ifpPolicerSetup() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Verify function call */
bcm_error_t
testVerify(int unit) {

    char str[512];

    bshell(unit, "fp show brief");
    bshell(unit, "sleep quiet 1");
    /* Print the entry to be tested with traffic */
    snprintf(str, 512, "fp show entry %d ", (BCM_FIELD_IFP_ID_BASE | vlan));
    bshell(unit, str);
    printf("\nSending vlan %d tagged test packets to ingress port:%d\n", vlan, port_list[0]);
    /* Send 100 test packet burst with selected outer vlan and packet size 512 bytes */
    snprintf(str, 512, "tx 100 pbm=%d DM=0x000000000202 SM=0x000000000001 vlan=%d length=512", port_list[0], vlan);
    bshell(unit, str);

    bshell(unit, "sleep quiet 3");
    bshell(unit, "show c");

    printf("\n------------------------------------- \n");
    printf("    RESULT OF RX PACKET VERIFICATION  ");
    printf("\n------------------------------------- \n");
    printf("Test = [%s]",((test_failed == 0 ) && (0 != rx_count))?"PASS":"FAIL");
    printf("\n------------------------------------- \n");
    /* test_failed is asserted by pktioRxCallback().
       sleep command in needed to give pktioRxCallback() chance to run. */
    if (test_failed == 1 || 0 == rx_count) {

       return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}


/*
 *   Cleanup test setup sequentially
 */
bcm_error_t testCleanup(int unit) {

    int rv, i=0;

    for ( i = 2; i < 1024; i++ ) {

        entry = (BCM_FIELD_IFP_ID_BASE | i);
        /* Remove FP entry from hardware */
        rv = bcm_field_entry_remove(unit, entry);
        if (BCM_FAILURE(rv)) {
            printf("entry%X:bcm_field_entry_remove() FAILED: %s\n", entry, bcm_errmsg(rv));
            return rv;
        }

        if (i % 8 == 0) {
            /* Detach the Policer of the FP entry */
            rv = bcm_field_entry_policer_detach(unit, entry, 0);
            if (BCM_FAILURE(rv)) {
                printf("entry%X:bcm_field_entry_policer_detach() FAILED: %s\n", entry, bcm_errmsg(rv));
                return rv;
            }

            /* Destroy the Policer entry */
            rv = bcm_policer_destroy(unit, (i/8));
            if (BCM_FAILURE(rv)) {
                printf("entry%X:bcm_policer_destroy() FAILED: %s\n", entry, bcm_errmsg(rv));
                return rv;
            }
        }

        /* Destroy the test FP entry */
        rv = bcm_field_entry_destroy(unit, entry);
        if (BCM_FAILURE(rv)) {
        printf("entry%X:bcm_field_entry_destroy() FAILED: %s\n", entry, bcm_errmsg(rv));
            return rv;
        }
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
 *  b) actual configuration (Done in ifpPolicerSetup())
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
