/*
 * Feature  : PFC Deadlock Detection and Recovery
 *
 * Usage    : BCM.0> cint th4_hsdk_mmu_fc_pfc_dd_recovery.c
 *
 * Config   : th4_hsdk_mmu_config.yml
 *
 * Log file : th4_hsdk_mmu_fc_pfc_dd_recovery_log.txt
 *
 * Test Topology  :
 *  
 *       DMAC=0x1            +------------+         DMAC=0x1
 *   I   SMAC=0x2   +-------->   56980    +-------> SMAC=0x2         I
 *   X   VLAN=2, Pri=2 cd0(1)|            |cd1(2)   VLAN=2, Pri=2    X
 *   I                       | l2:mac=0x1 |                          I
 *   A                       |    vlan=2  |                          A
 *            <-------+      |    port=2  |     <-------+
 *         TX PFC FRAMES     |            |  RX PFC FRAMES
 *                           +------------+ 
 *
 *  Summary:
 *  ========
 *  This cint example demonstrates PFC deadlock detection and recovery APIs
 *
 * This script can verify below cosq APIs:
 * ========
 *     bcm_cosq_pfc_deadlock_recovery_event_register
 *     bcm_cosq_pfc_deadlock_control_set
 *     bcm_cosq_pfc_dd_recovery_mode_set
 *     bcm_cosq_pfc_deadlock_info_get
 *     bcm_cosq_pfc_deadlock_queue_status_get
 *
 *  Detailed steps done in the CINT script:
 *  =======================================
 *  1) Step1 - Test Setup (Done in test_setup())
 *  ============================================
 *    a) Select one ingress and one egress port
 *    b) Setup a L2 forwarding path from ingress to egress port
 *
 *  2) Step2 - Configuration (Done in pfc_config())
 *  =================================================
 *    a) Set the PFC class and scheduler profile to egress port
 *    b) Set the PFC tx priotiry to PG mapping
 *    c) Disable generic Tx and Rx Pause
 *    d) Enable PFC Tx and Rx Pause
 *    e) Configure PFC deadlock detection and recovery (done in pfc_deadlock_detection_config())
 *
 *  3) Step3 - Verification (Done in test_verify())
 *  ==========================================
 *    a) Transmit L2 packet (dmac=0x1 smac=0x2 vlan=2 prio=0)
 *       at line rate from IXIA to ingress port cd0
 *
 *    c) Expected Result:
 *       ================
 *       For test case mentioned in a)
 *       On cd0 (ingress port): verify that PFC frames should be transmitted at regular intervals as
 *       egress port is congested.
 *       In 'show c' output, PFC frame Tx should be seen on cd0 (ingress port)
 *
 *       If there is PFC deadlock, the callback should print the port number and PFC priority
 */
 
cint_reset();

int unit = 0;
bcm_port_t ing_port; /* Ingress port */
bcm_port_t egr_port; /* Egress port */
int cos = 2;     /* Priority of incoming packet on which testing is done */

int MAX_QUEUES = 12;
int MAX_PORTS = 272;
int CPU_PORT = 0;
int prev_port = -1;
int free_ucast_index = 0;
int free_mcast_index = 0;
int free_sched_index = 0;
bcm_gport_t ucast_q_gport[MAX_PORTS][MAX_QUEUES];

static bcm_error_t
cosq_gport_traverse_callback(int unit, bcm_gport_t port,
                             int numq, uint32 flags,
                             bcm_gport_t gport, void *user_data)
{
     int local_port = 0;

     switch (flags) {
         case BCM_COSQ_GPORT_UCAST_QUEUE_GROUP:
             local_port = BCM_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_GET(gport);
             if (prev_port != local_port) {
                 free_ucast_index = 0;
                 free_mcast_index = 0;
                 free_sched_index = 0;
                 prev_port = local_port;
             }

             if (CPU_PORT == local_port) {
                 return BCM_E_PARAM;
             }
             ucast_q_gport[local_port][free_ucast_index] = gport;
             free_ucast_index++;
             break;

         case BCM_COSQ_GPORT_MCAST_QUEUE_GROUP:
         case BCM_COSQ_GPORT_SCHEDULER:
             break;
         default:
             return BCM_E_PARAM;
     }

    return 1;
 }

/*
 * Build the UC queue gport list
 */
static bcm_error_t
cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb, void *user_data)
{
    int rv = 0;

    rv = bcm_cosq_gport_traverse(unit, cb, NULL);
    if (rv  < BCM_E_NONE) {
        printf("ERR(%d): bcm_cosq_gport_traverse function unit = %d\n", rv, unit);
        return -1;
    }

    free_ucast_index = 0;
    free_mcast_index = 0;
    free_sched_index = 0;
    prev_port = -1;

    return 1;
}

/*
 * This function is written so that hardcoding of port
 * numbers in Cint scripts is removed. This function gives
 * required number of ports
 */
bcm_error_t portNumbersGet(int unit, int *port_list, int num_ports)
{
    int i=0, port=0, rv = 0;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;

    rv = bcm_port_config_get(unit, &configP);
    if(BCM_FAILURE(rv)) {
       printf("\nError in retrieving port configuration: %s.\n", bcm_errmsg(rv));
       return rv;
    }

    ports_pbmp = configP.e;
    for (i= 1; i < BCM_PBMP_PORT_MAX; i++) {
        if (BCM_PBMP_MEMBER(&ports_pbmp, i)&& (port < num_ports)) {
            port_list[port]=i;
            port++;
        }
    }

    if (( port == 0 ) || ( port != num_ports )) {
         printf("portNumbersGet() failed \n");
         return -1;
    }

    return BCM_E_NONE;
}

bcm_error_t data_path_setup(int unit)
{
    bcm_vlan_t vid = 2;
    bcm_mac_t dest_mac = "00:00:00:00:00:01"; /*Destination MAC */
    bcm_pbmp_t pbmp, upbmp;
    bcm_l2_addr_t l2addr;
    int rv = 0;

    rv = bcm_vlan_create(unit, vid);
    if (rv != BCM_E_NONE) {
        printf("[bcm_vlan_create] returned '%s'\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, ing_port);
    BCM_PBMP_PORT_ADD(pbmp, egr_port);
    rv = bcm_vlan_port_add(unit, vid, pbmp, upbmp);
    if (rv != BCM_E_NONE) {
        printf("bcm_vlan_port_add returned :%s.\n", bcm_errmsg(rv));
        return rv;
    }

    bcm_l2_addr_t_init(l2addr, dest_mac, vid);
    l2addr.port = egr_port;
    l2addr.flags |= BCM_L2_STATIC;
    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE) {
        printf("bcm_l2_addr_add returned :'%s'\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

bcm_error_t data_path_cleanup(int unit)
{
    bcm_vlan_t vid = 2;

    /* Delete L2 address */
    BCM_IF_ERROR_RETURN(bcm_l2_addr_delete_by_vlan(unit, vid, 0));

    /* Create a vlan. */
    BCM_IF_ERROR_RETURN(bcm_vlan_destroy(unit, vid));

    return BCM_E_NONE;
}

/*
 * PFC class and scheduler profile goes together. 
 * The cosq_port_profile_set is common for PFC and Scheduler i.e. it setups up the same profile 
 * for the port for both Scheduler and PFC. Hence scheduler profile 1 need to be created if we need to 
 * create and attach pfc profile 1 to port.
 */
bcm_error_t
pfc_cos_and_sched_profile_setup(int unit, int prf_id)
{
    bcm_cosq_mapping_t cosq_mapping_arr[MAX_QUEUES];
    bcm_cosq_pfc_class_map_config_t config_array[8];
    int rv = 0;
    int i;

    /* Configure Scheduler profile */
    cosq_mapping_arr[0].cosq = 0;
    cosq_mapping_arr[0].num_ucq = 1;
    cosq_mapping_arr[0].num_mcq = 1;
    cosq_mapping_arr[0].strict_priority = 0;
    cosq_mapping_arr[0].fc_is_uc_only = 0;

    cosq_mapping_arr[1].cosq = 1;
    cosq_mapping_arr[1].num_ucq = 1;
    cosq_mapping_arr[1].num_mcq = 1;
    cosq_mapping_arr[1].strict_priority = 0;
    cosq_mapping_arr[1].fc_is_uc_only = 0;

    cosq_mapping_arr[2].cosq = 2;
    cosq_mapping_arr[2].num_ucq = 1;
    cosq_mapping_arr[2].num_mcq = 1;
    cosq_mapping_arr[2].strict_priority = 0;
    cosq_mapping_arr[2].fc_is_uc_only = 0;

    cosq_mapping_arr[3].cosq = 3;
    cosq_mapping_arr[3].num_ucq = 1;
    cosq_mapping_arr[3].num_mcq = 1;
    cosq_mapping_arr[3].strict_priority = 0;
    cosq_mapping_arr[3].fc_is_uc_only = 0;

    cosq_mapping_arr[4].cosq = 4;
    cosq_mapping_arr[4].num_ucq = 1;
    cosq_mapping_arr[4].num_mcq = 0;
    cosq_mapping_arr[4].strict_priority = 0;
    cosq_mapping_arr[4].fc_is_uc_only = 0;

    cosq_mapping_arr[5].cosq = 5;
    cosq_mapping_arr[5].num_ucq = 1;
    cosq_mapping_arr[5].num_mcq = 0;
    cosq_mapping_arr[5].strict_priority = 0;
    cosq_mapping_arr[5].fc_is_uc_only = 0;

    cosq_mapping_arr[6].cosq = 6;
    cosq_mapping_arr[6].num_ucq = 1;
    cosq_mapping_arr[6].num_mcq = 0;
    cosq_mapping_arr[6].strict_priority = 0;
    cosq_mapping_arr[6].fc_is_uc_only = 0;

    cosq_mapping_arr[7].cosq = 7;
    cosq_mapping_arr[7].num_ucq = 1;
    cosq_mapping_arr[7].num_mcq = 0;
    cosq_mapping_arr[7].strict_priority = 0;

    cosq_mapping_arr[8].cosq = 8;
    cosq_mapping_arr[8].num_ucq = 0;
    cosq_mapping_arr[8].num_mcq = 0;
    cosq_mapping_arr[8].strict_priority = 0;
    cosq_mapping_arr[8].fc_is_uc_only = 0;

    cosq_mapping_arr[9].cosq = 9;
    cosq_mapping_arr[9].num_ucq = 0;
    cosq_mapping_arr[9].num_mcq = 0;
    cosq_mapping_arr[9].strict_priority = 0;
    cosq_mapping_arr[9].fc_is_uc_only = 0;

    cosq_mapping_arr[10].cosq = 10;
    cosq_mapping_arr[10].num_ucq = 0;
    cosq_mapping_arr[10].num_mcq = 0;
    cosq_mapping_arr[10].strict_priority = 0;
    cosq_mapping_arr[10].fc_is_uc_only = 0;

    cosq_mapping_arr[11].cosq = 11;
    cosq_mapping_arr[11].num_ucq = 0;
    cosq_mapping_arr[11].num_mcq = 0;
    cosq_mapping_arr[11].strict_priority = 0;
    cosq_mapping_arr[11].fc_is_uc_only = 0;

    /* create scheduler profile */
    rv = bcm_cosq_schedq_mapping_set(unit, prf_id, MAX_QUEUES, cosq_mapping_arr);
    if (rv != BCM_E_NONE) {
        printf ("sched_profile_setup failed() :%s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Configure PFC receive for profile prf_id, priority 0&4 are PFC-optimized */
    config_array[0].pfc_enable = 1;
    config_array[0].cos_list_bmp |= 1 << 3;

    config_array[1].pfc_enable = 1;
    config_array[1].cos_list_bmp |= 1 << 6;

    config_array[2].pfc_enable = 1;
    config_array[2].cos_list_bmp |= 1 << 5;

    config_array[3].pfc_enable = 1;
    config_array[3].cos_list_bmp |= 1 << 4;

    config_array[4].pfc_enable = 1;
    config_array[4].cos_list_bmp |= 1 << 0;

    config_array[5].pfc_enable = 1;
    config_array[5].cos_list_bmp |= 1 << 2;

    config_array[6].pfc_enable = 1;
    config_array[6].cos_list_bmp |= 1 << 1;

    config_array[7].pfc_enable = 1;
    config_array[7].cos_list_bmp |= 1 << 7;

    rv = bcm_cosq_pfc_class_config_profile_set(unit, prf_id, 8, config_array);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_pfc_class_config_profile_set() failed: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Set the scheduler profile to port
 */
bcm_error_t
port_pfc_cos_and_sched_profile_config(int unit, int port)
{
    int profile_id = 1;
    int rv = 0;

    /* create scheduler profile */
    if (BCM_FAILURE((rv = pfc_cos_and_sched_profile_setup(unit, profile_id)))) {
         printf("Error in pfc_cos_and_sched_profile_setup() : %s.\n", bcm_errmsg(rv));
         return rv;
    }

    bcm_cosq_profile_type_t profile_type = bcmCosqProfilePFCAndQueueHierarchy;
    bcm_cosq_dynamic_setting_type_t dynamic;
    BCM_IF_ERROR_RETURN(bcm_cosq_profile_property_dynamic_get(unit, profile_type, &dynamic));

    if (dynamic == bcmCosqDynamicTypeFixed) {
        /*
         * Mapping cannot be changed during run time as disable MMU
         * traffic routine cannot guarantee flush based on Source port
         * Example: bcmCosqProfilePriGroupProperties
         */
        printf("The profile %d can not be modified(bcmCosqDynamicTypeFixed)\n", profile_id);
        return -1;
    } else if (dynamic == bcmCosqDynamicTypeConditional) {
        /*
         * Mapping can be changed as long as the port has no traffic in MMU
         * Example: bcmCosqProfileQueueHierarchy, bcmCosqProfilePFCClass,
         *          bcmCosqProfilePFCAndQueueHierarchy
         */
        printf("The profile %d is bcmCosqDynamicTypeConditional\n", profile_id);
        BCM_IF_ERROR_RETURN(bcm_port_control_set (unit, port, bcmPortControlMmuTrafficEnable, 0));

        /* Attach sched profile to specified port */ 
        rv = bcm_cosq_port_profile_set(unit, port, bcmCosqProfilePFCAndQueueHierarchy, profile_id);
        if (rv != BCM_E_NONE) {
             printf("bcm_cosq_port_profile_set failed: %s.\n", bcm_errmsg(rv));  
             return rv;
        }

        BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, port, bcmPortControlMmuTrafficEnable, 1));
    } else if (dynamic == bcmCosqDynamicTypeFlexible) {
        /* Mapping can be changed at run time. Example : bcmCosqProfileIntPriToPGMap */
        printf("The profile %d is bcmCosqDynamicTypeFlexible\n", profile_id);

        /* Attach sched profile to specified port */ 
        rv = bcm_cosq_port_profile_set(unit, port, bcmCosqProfilePFCAndQueueHierarchy, profile_id);
        if (rv != BCM_E_NONE) {
             printf("bcm_cosq_port_profile_set failed: %s.\n", bcm_errmsg(rv));  
             return rv;
        }
    }

    return BCM_E_NONE;
}

/* PFC tx priotiry to PG mapping */
bcm_error_t
pfc_priority_to_pg_setup(int unit)
{
    int pri_to_pg_array[8] = {0, 1, 2, 3, 4, 5, 6, 7}; 
    int profile_id = 1;
    int rv = 0;
  
    /* Set PFC-tx priority to PG mapping 1:1 mapping*/
    rv = bcm_cosq_priority_group_pfc_priority_mapping_profile_set(unit, profile_id, 8, pri_to_pg_array);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_priority_group_pfc_priority_mapping_profile_set returned '%s'\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*************************************************
 * Deadlock Detection and recovery
 */
int
pfc_dd_cb (int unit, bcm_port_t port,
           bcm_cos_queue_t pfc_pri,
           bcm_cosq_pfc_deadlock_recovery_event_t recovery_state, void *userdata)
{
    printf("User callback for port: %d, PFC_pri:%d\n", port, pfc_pri);
    bcm_cosq_pfc_deadlock_recovery_start(unit, port, pfc_pri);

    return BCM_E_NONE;
}

/*
 * PFC DD configuration
 */
bcm_error_t
pfc_deadlock_detection_config(int unit)
{
    int pfc_priority;
    int rv;

    BCM_IF_ERROR_RETURN(bcm_cosq_pfc_deadlock_recovery_event_register(unit, pfc_dd_cb, NULL));

    /* Enable PFC deadlock detection for priority 0 */
    pfc_priority = 0;
    BCM_IF_ERROR_RETURN(bcm_cosq_pfc_deadlock_control_set(unit, egr_port, pfc_priority,
                             bcmCosqPFCDeadlockDetectionTimer, 10));
    BCM_IF_ERROR_RETURN(bcm_cosq_pfc_deadlock_control_set(unit, egr_port, pfc_priority,
        bcmCosqPFCDeadlockTimerGranularity, bcmCosqPFCDeadlockTimerInterval1MiliSecond));

    BCM_IF_ERROR_RETURN(bcm_cosq_pfc_deadlock_control_set(unit, egr_port, pfc_priority,
                             bcmCosqPFCDeadlockDetectionAndRecoveryEnable, 1));

    /* Change PFC deadlock recovery mode */
    printf("Change mode to use PfcDdRecoveryModePFCState\n");
/*    BCM_IF_ERROR_RETURN(
             bcm_cosq_pfc_dd_recovery_mode_set(unit, egr_port, bcmCosqPfcDdRecoveryModePFCState));
    bshell(0, "sleep 1");
*/

    printf("Change mode to use PfcDdRecoveryModeTimer\n");
/*    BCM_IF_ERROR_RETURN(
             bcm_cosq_pfc_dd_recovery_mode_set(unit, egr_port, bcmCosqPfcDdRecoveryModeTimer));
    bshell(0, "sleep 1");
*/

    /* Recovery Action */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit,
                    bcmSwitchPFCDeadlockRecoveryAction, bcmSwitchPFCDeadlockActionTransmit));

    return BCM_E_NONE;
}

/*
 * PFC DD status
 */
bcm_error_t
pfc_deadlock_detection_status(int unit)
{
    bcm_cosq_pfc_deadlock_info_t pfc_deadlock_info;
    uint8 deadlock_status;
    int pfc_priority;
    bcm_gport_t qgport;
    int rv;

    /* Deadlock status for a PFC priority */
    pfc_priority = 0;
    bcm_cosq_pfc_deadlock_info_get(unit, pfc_priority, &pfc_deadlock_info);
    print pfc_deadlock_info;

    /* Deadlock status for a given UC Queue Gport */
    qgport = ucast_q_gport[egr_port][2];
    bcm_cosq_pfc_deadlock_queue_status_get(unit, qgport, &deadlock_status);
    printf("deadlock_status=%d\n", deadlock_status);

    return BCM_E_NONE;
}

bcm_error_t pfc_config(int unit, bcm_port_t ing_port, bcm_port_t egr_port)
{
    bcm_mac_t pause_smac = "00:11:22:33:44:55"; /*Source MAC on all PFC frames transmitted from a port */
    uint32 limit_kbits_sec = 1; /* Bandwidth limit on egress queue - 1 kbps*/
    int enable = 1;
    int disable = 0;
    int rv = 0;

    rv = port_pfc_cos_and_sched_profile_config(unit, egr_port);
    if (rv != BCM_E_NONE) {
        printf("port_pfc_cos_and_sched_profile_config returned '%s'\n", bcm_errmsg(rv));
        return rv;
    }

    /* PFC tx priotiry to PG mapping */
    rv = pfc_priority_to_pg_setup(unit);
    if (rv != BCM_E_NONE) {
        printf("pfc_priority_to_pg_setup returned '%s'\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Set PAUSE frame smac\n");

    /* Set the source MAC for PFC Transmit frames */
    rv = bcm_port_pause_addr_set(unit, ing_port, pause_smac);
    if (rv != BCM_E_NONE) { 
        printf("bcm_port_pause_addr_set returned '%s'\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Disable regular PAUSE\n");

    /* Disable normal PAUSE on ingress port */
    rv = bcm_port_pause_set(unit, ing_port, disable, disable);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_pause_set~ingress returned '%s'\n", bcm_errmsg(rv));
        return rv;
    }

    /* Disable normal PAUSE on egress port */
    rv = bcm_port_pause_set(unit, egr_port, disable, disable);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_pause_set~egress returned '%s'\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Eenabling PFC PAUSE\n");

    /* Enable PFC Tx on ingress port and RX on egress port */
    rv = bcm_port_control_set(unit, ing_port, bcmPortControlPFCTransmit, 1);
    if (rv != BCM_E_NONE) {
         printf("bcm_port_control_set~Tx returned '%s'\n", bcm_errmsg(rv));
         return rv;
    }
    rv = bcm_port_control_set(unit, egr_port, bcmPortControlPFCReceive, 1);
    if (rv != BCM_E_NONE) {
         printf("bcm_port_control_set~Rx returned '%s'\n", bcm_errmsg(rv));
         return rv;
    }

    /*
     * Set the bandwidth limit on particular queue of egr port 
     */
    rv = bcm_cosq_gport_bandwidth_set(unit, egr_port, cos, limit_kbits_sec, limit_kbits_sec, 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_set returned '%s'\n", bcm_errmsg(rv));
        return rv;
    }

    /* Modify PG limits to generate early PFC */
    bcm_cosq_control_set (unit, ing_port, 7,  bcmCosqControlIngressPortPGSharedDynamicEnable, 0);

    rv = pfc_deadlock_detection_config(unit);
    if (rv != BCM_E_NONE) {
        printf("pfc_deadlock_detection_config returned '%s'\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

bcm_error_t test_setup(int unit)
{
    int port_list[2], i;
    int rv = 0;

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 2)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    ing_port = port_list[0];
    egr_port = port_list[1];

    /* common data path setup */
    if (BCM_FAILURE((rv = data_path_setup(unit)))) {
        printf("data_path_setup() failed.: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Build the UC queue GPORT list */
    printf("Build the UC queue GPORT list\n");
    if (BCM_FAILURE((rv = cosq_gport_traverse(0, cosq_gport_traverse_callback, NULL)))) {
        printf("cosq_gport_traverse() failed.\n");
        return -1;
    }

    return BCM_E_NONE;
}

/*
 * Ingress port should send PFC frame towards IXIA
 * egress port should not egress anything as it receives PFC pause frames from Ixia.
 */
void test_verify(int unit)
{
    char str[1280];
    bcm_error_t rv;

    /* Send a packet with priority 0 to ingress port, the IFP assign the packet to
     * new COS queue based on the classifier */
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ing_port, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egr_port, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egr_port, BCM_PORT_DISCARD_ALL));

    /* snprintf(str, 256, "tx 100 pbm=%d length=512 dm=0x11 vlanprio=0 vlantag=2;", ing_port); */

    snprintf(str, 1280, "tx 1 pbm=%d data=000000000001000000000002810000020800450001EA0000000040FF3BF5141414020A0A0A0108004502002E000040000A06EDAFC0A80164C0A8006481815151B1B2B3B4A1A2A3A4500F01F543CE00001112000102030000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000AF9386DB; sleep quiet 1;", ing_port);
    bshell(unit, str);

    bshell(0, "sleep 1");

    printf("Show c with traffic on all queues\n");
    snprintf(str, 256, "show c %d;", ing_port);
    bshell(unit, str);

    /* Check PFC deadlock status */
    rv = pfc_deadlock_detection_status(unit);
    if (rv != BCM_E_NONE) {
        printf("pfc_deadlock_detection_status returned '%s'\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Clean up pre-test setup.
 */
int test_cleanup(int unit)
{
    int pfc_priority = 0;

    printf("==>Cleaning up...\n");

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ing_port, BCM_PORT_LOOPBACK_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egr_port, BCM_PORT_LOOPBACK_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egr_port, BCM_PORT_DISCARD_NONE));

    /* Disable deadlock detection */
    BCM_IF_ERROR_RETURN(bcm_cosq_pfc_deadlock_control_set(unit, egr_port, pfc_priority,
                             bcmCosqPFCDeadlockDetectionAndRecoveryEnable, 0));

    BCM_IF_ERROR_RETURN(bcm_cosq_pfc_deadlock_recovery_event_unregister(unit, pfc_dd_cb, NULL));


    /* Delete L2 forwarding */
    if (BCM_E_NONE != data_path_cleanup(unit)) {
        printf("data_path_cleanup() failed.\n");
        return -1;
    }

    return BCM_E_NONE;
}

bcm_error_t execute()
{
    bcm_error_t rv;

    bshell(unit, "config show; attach; version");

    /* Select ports and set up L2 switching */
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return -1;
    }

    /* Config and enable PFC */
    if (BCM_FAILURE((rv = pfc_config(unit, ing_port, egr_port)))) {
        printf("pfc_config() failed.\n");
        return -1;
    }

    /* verify the result */
    if (BCM_FAILURE((rv = test_verify(unit)))) {
        printf("test_verify() failed.\n");
        return -1;
    }


    if (BCM_FAILURE((rv = test_cleanup(unit)))) {
        printf("test_cleanup() failed.\n");
        return -1;
    }

    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
    print execute();
}

