/*
 Feature        : EBST - Service pool and UC Queue Based Events

 Usage          : BCM.0> cint th4_hsdk_mmu_ebst_service_pool.c

 config         : th4_hsdk_mmu_config.yml
                  th4_hsdk_mmu_config_4g.yml

 Log file       : th4_hsdk_mmu_ebst_service_pool_log.txt

 Test Topology  :

                      +------------+
                      |            |
 DMAC=0x000000000011  |            |        DMAC=0x000000000011
 SMAC=0x2      ------->   56990    +------> SMAC=0x2
 VLAN=3,Pri=0  cd0(1) |            |cd1(2)  VLAN=3,Pri=0
                      |            |
                      +------------+

  Summary:
  ========
  This example illustrates configuration of EBST based on unicast queue & service pool events

    Detailed steps done in the CINT script:
    =======================================
    1) Step1 - Test Setup (Done in test_setup())
    ============================================
     - Select two port: ingress and egress ports
     - Create VLAN with the selected ports
     - Set up L2 switching to egress port
     - Set ingress and egress ports to loopback
     - Discard looped back packet on egress port
     - Configure rate on egress port to create congestion
    ===========================================
    2) Step2 - EBST Configuration ebst_config()
      a. Configure threshold for egress port and unicast queue 0
      b. Configure monitor Profile and number of entries used in
         data buffer for selected port/queue
      c. Configure EBST scan mode, scan interval, scan threshold
      d, Enable EBST Scan and EBST tracking
    ====================================================
    3) Step3 - Verification (Done in test_verify())
      a. Send packets to ingress port, packet is L2 switched to egress port with UC Queue 0
      b. These packets are expected to pass the BST threshold, and generate events
      c. Remove UC Queue from monitor profile before read EBST event data
      d. Read FIFO for event data
      e. Stop EBST
    ==================================
      e) Expected Result:
         ================
      Array count from bcm_cosq_ebst_data_get should be NON ZERO in case we see data in FIFO
 */

cint_reset();

int unit = 0;
bcm_port_t ing_port; /* Ingress port */
bcm_port_t egr_port; /* Egress port */
bcm_cos_t uc_cos = 0; /* UC COS Queue */
bcm_vlan_t vlan_in = 3;
uint8 dst_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x11};
int check = 0;  /* set to 1 to dump tables that are configured by each API */
int test_failed = 0;

/*
 * This function is written so that hardcoding of port
 * numbers in Cint scripts is removed. This function gives
 * required number of ports
 */
bcm_error_t portNumbersGet(int unit, int *port_list, int num_ports)
{
    int i=0, port=0, rv=0;
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

/* Create a vlan and add ports to vlan */
bcm_error_t data_path_setup(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_pbmp_t pbmp, ubmp;
    bcm_l2_addr_t addr;

    /* Create ingress vlan and add port to vlan */
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ing_port);
    BCM_PBMP_PORT_ADD(pbmp, egr_port);
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vlan_in));
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vlan_in, pbmp, ubmp));

    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, ing_port, vlan_in));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egr_port, vlan_in));

    /* Add a L2 address on front panel port. */
    bcm_l2_addr_t_init(&addr, dst_mac, vlan_in);
    addr.port = egr_port;
    addr.flags |= BCM_L2_STATIC;
    BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &addr));

    return rv;
}

/* Test setup along with rate on egress port to create congesiton */
bcm_error_t test_setup(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    int port_list[2], i;
    uint32 stages;
    uint32 buf[2];
    bcm_time_tod_t tod, new_tod;

    if (BCM_FAILURE(portNumbersGet(unit, port_list, 2))) {
        printf("portNumbersGet() failed\n");
        return rv;
    }

    ing_port = port_list[0];
    egr_port = port_list[1];

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ing_port, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egr_port, BCM_PORT_LOOPBACK_MAC));

    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egr_port, BCM_PORT_DISCARD_ALL));

    if (BCM_FAILURE((rv = data_path_setup(unit)))) {
        printf("data_path_setup failed. : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    if (BCM_FAILURE((rv = bcm_port_rate_egress_set(unit , egr_port, 64, 64)))) {
        printf("bcm_port_rate_egress_set : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* BCM_TIME_STAGE_MMU. */
    stages = BCM_TIME_STAGE_MMU;
    buf[0] = 4;
    sal_memcpy(&tod.ts.seconds, buf, 2);
    tod.ts.nanoseconds = 5;
    buf[0] = 6;
    sal_memcpy(&tod.ts_adjustment_counter_ns, buf, 2);
    BCM_IF_ERROR_RETURN(bcm_time_tod_set(unit, stages, &tod));

    return rv;
}

/*
 * Set start and stop threshold to capture micro burts
 */
bcm_error_t set_ebst_threshold_profile(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_object_id_t object_id;
    int CELL_SIZE = 254;

    /* Set EBST monitor on egress port and UC queue 0 (queue #0)  */
    object_id.port = egr_port;
    object_id.cosq = uc_cos;
    object_id.buffer = -1;

    bcm_cosq_ebst_threshold_profile_t profile;

    bcm_cosq_ebst_threshold_profile_t_init(&profile);
    profile.start_bytes = 6 * CELL_SIZE;
    profile.stop_bytes = 3 * CELL_SIZE;

    /* Set threshold for egress port UC queue 0 */
    rv = bcm_cosq_ebst_threshold_profile_set(unit, &object_id, bcmBstStatIdUcast, &profile);
    if (BCM_FAILURE(rv)) {
        printf("bcm_cosq_ebst_threshold_profile_set : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    if(check){
        bshell(0, "bsh -c 'lt TM_EBST_PROFILE traverse -l'");
        bshell(0, "bsh -c 'lt TM_SCHEDULER_CONFIG traverse -l'");
    }

    return rv;
}

/*
 * Configure EBST Monitor on (egress port, UC queue) with 1000 entries
 * TH4 supports a total of 12 queues per port, the default configuration is
 * 8 unicast queues (0..7) and 4 multicast queues (8..11)
 */
bcm_error_t set_ebst_monitor(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_object_id_t object_id;
    bcm_cosq_ebst_monitor_t monitor;

    /* Set EBST monitor on egress port and UC queue 0 (queue #0)  */
    object_id.port = egr_port;
    object_id.cosq = uc_cos;
    object_id.buffer = -1;

    bcm_cosq_ebst_monitor_t_init(&monitor);
    monitor.enable = 1;
    monitor.entry_num = 1000;
    monitor.flags = BCM_COSQ_EBST_MONITOR_ENTRY_NUM_VALID |
                    BCM_COSQ_EBST_MONITOR_ENABLE_VALID;

    rv = bcm_cosq_ebst_monitor_set(unit, &object_id, bcmBstStatIdUcast, &monitor);
    if (BCM_FAILURE(rv)) {
        printf("bcm_cosq_ebst_monitor_set : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Set ebst monitor on shared service pool 0 */
    rv = bcm_cosq_ebst_monitor_set(unit, &object_id, bcmBstStatIdEgrPool, &monitor);
    if (BCM_FAILURE(rv)) {
        printf("bcm_cosq_ebst_monitor_set : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    if(check){
        bshell(0, "bsh -c 'lt TM_EBST_UC_Q traverse -l'");
    }

    return rv;
}

/* Configure EBST Scan mode, threshold, Enable EBST Scan and EBST Tracking */
bcm_error_t set_ebst_scan_mode_threshold(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    int threshold;

    /* Scan round */
    rv =  bcm_cosq_ebst_control_set(unit, bcmCosqEbstControlScanInterval, 2000);
    if (BCM_FAILURE(rv)) {
        printf("bcm_cosq_ebst_control_set : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Scan mode */
    rv =  bcm_cosq_ebst_control_set(unit, bcmCosqEbstControlScanMode, bcmCosqEbstScanModeQueue);
    if (BCM_FAILURE(rv)) {
        printf("bcm_cosq_ebst_control_set : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Scan threshold */
    threshold = 100;
    rv = bcm_cosq_ebst_control_set(unit, bcmCosqEbstControlScanThreshold, threshold);
    if (BCM_FAILURE(rv)) {
        printf("bcm_cosq_ebst_control_set : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Enable system level EBST Scan */
    rv = bcm_cosq_ebst_control_set(unit, bcmCosqEbstControlScanEnable , 1);
    if (BCM_FAILURE(rv)) {
        printf("bcm_cosq_ebst_control_set : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_cosq_ebst_enable_set(unit, 1);
    if (BCM_FAILURE(rv)) {
        printf("bcm_cosq_ebst_enable_set : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    if(check){
        bshell(0, "bsh -c 'lt TM_EBST_CONTROL traverse -l'");
    }

    return rv;
}

bcm_error_t ebst_config(int unit)
{
    bcm_error_t rv = BCM_E_NONE;

    rv = set_ebst_threshold_profile(unit);
    if (BCM_FAILURE(rv)) {
        printf("set_ebst_threshold_profile() failed. : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = set_ebst_monitor(unit);
    if (BCM_FAILURE(rv)) {
        printf("set_ebst_monitor() failed. : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = set_ebst_scan_mode_threshold(unit);
    if (BCM_FAILURE(rv)) {
        printf("set_ebst_scan_mode_threshold() failed. : %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/*
 * Remove (egress port, UC Queue) from EBST monitor before FIFO Read
 */
bcm_error_t remove_object_ebst_monitor(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_object_id_t object_id;
    bcm_cosq_ebst_monitor_t monitor;

    object_id.port = egr_port;
    object_id.cosq = uc_cos;
    object_id.buffer = -1;

    bcm_cosq_ebst_monitor_t_init(&monitor);
    monitor.enable = 0;
    monitor.entry_num = -1;
    monitor.flags = BCM_COSQ_EBST_MONITOR_ENABLE_VALID;

    /* Set EBST monitor on egress port UC queue */
    rv = bcm_cosq_ebst_monitor_set(unit, &object_id, bcmBstStatIdUcast, &monitor);
    if (BCM_FAILURE(rv)) {
        printf("bcm_cosq_ebst_monitor_set : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Set EBST monitor on service pool 0 */
    rv = bcm_cosq_ebst_monitor_set(unit, &object_id, bcmBstStatIdEgrPool, &monitor);
    if (BCM_FAILURE(rv)) {
        printf("bcm_cosq_ebst_monitor_set : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

bcm_error_t stop_ebst_monitor(int unit)
{
    bcm_error_t rv = BCM_E_NONE;

    /* Stop system level EBST Scan */
    rv = bcm_cosq_ebst_control_set(unit, bcmCosqEbstControlScanEnable , 0);
    if (BCM_FAILURE(rv)) {
        printf("bcm_cosq_ebst_control_set : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Release internal resources */
    rv = bcm_cosq_ebst_enable_set(unit, 0);
    if (BCM_FAILURE(rv)) {
        printf("bcm_cosq_ebst_enable_set : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/*
 * Read EBST stat from table TM_EBST_DATA
 */
bcm_error_t read_ebst_stat(int unit, bcm_bst_stat_id_t sid)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_object_id_t object_id;
    bcm_cosq_ebst_data_entry_t stat_array[1000];
    int array_count[2] = {0};

    /*
     * Egress port are available on both itm0 and itm1.
     * and If BST count crosses the start threshold of a
     * particular BST counter (queue / PortSP), it copies
     * the occupancy level information to EBST Data buffer per ITM
     */
    int itm;
    for (itm = 0; itm<2 ; itm++) {
        object_id.port = egr_port;
        object_id.cosq = uc_cos;
        object_id.buffer = itm;
        rv = bcm_cosq_ebst_data_get(unit, &object_id, sid,
                                    1000, stat_array, &array_count[itm]);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_ebst_data_get() failed. : %s\n", bcm_errmsg(rv));
            return rv;
        }
        printf("\nTotal Array Count from ITM %d is %d \n", itm, array_count[itm]);
        if (array_count[itm] !=0) {
            break;
        }
    }

    if (!array_count[0] && !array_count[1]) {
        test_failed = 1;
        printf("No EBST event data\n");
        return BCM_E_FAIL;
    }

    int index = sal_rand() % array_count[itm];
    if (index == 0) {
        index = 1;
    }

    printf("\nReading a Random EBST DATA Entry : %d  \n", index);
    printf("\n Bytes Value from Entry %d is %d \n", index, stat_array[index].bytes);
    if (stat_array[index].green_drop_state == bcmCosqColorPktDrop) {
       printf("\n Green Drop State from Entry %d is in DROP State\n", index);
    } else {
       printf("\n Green Drop State from Entry %d is in ALLOW State\n", index);
    }
    if (stat_array[index].yellow_drop_state == bcmCosqColorPktDrop) {
       printf("\n Yellow Drop State from Entry %d is in DROP State\n", index);
    } else {
       printf("\n Yellow Drop State from Entry %d is in ALLOW State\n", index);
    }
    if (stat_array[index].red_drop_state == bcmCosqColorPktDrop) {
       printf("\n RED Drop State from Entry %d is in DROP State\n", index);
    } else {
       printf("\n RED Drop State from Entry %d is in ALLOW State\n", index);
    }

    return BCM_E_NONE;
}

/* Verify function call */
bcm_error_t
test_verify(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    char str[512];

    bshell(unit, "clear c");
    printf("\nSending 100 unicast packet(length 2500) to ingress port : %d\n", ing_port);
    printf("packet is L2 switched to egress port : %d\n", egr_port);
    snprintf(str, 512, "tx 100 pbm=%d DestMac=0x11 vlantag=3 length=2500; sleep quiet 1", ing_port);
    bshell(unit, str);
    bshell(unit, "sleep quiet 10");

    rv = remove_object_ebst_monitor(unit);
    if (rv != BCM_E_NONE) {
        printf("remove_object_ebst_monitor() failed. : %s\n", bcm_errmsg(rv));
        return rv;
    }

    if(check){
        bshell(0, "bsh -c 'lt TM_EBST_STATUS traverse -l'");
        bshell(0, "bsh -c 'lt TM_EBST_CONTROL traverse -l'");
    }

    bshell(unit, "sleep quiet 5");

    printf("\n==> Read EBST entry for Service Pool\n");
    rv = read_ebst_stat(unit, bcmBstStatIdEgrPool);
    if (rv != BCM_E_NONE) {
        printf("read_ebst_stat() failed. : %s\n", bcm_errmsg(rv));
    }

    printf("\n==> Read EBST entry for Unicast queue\n");
    rv = read_ebst_stat(unit, bcmBstStatIdUcast);
    if (rv != BCM_E_NONE) {
        printf("read_ebst_stat() failed. : %s\n", bcm_errmsg(rv));
    }

    printf("\n------------------------------------- \n");
    printf("Test = [%s]", (test_failed == 0) ? "PASS" : "FAIL");
    printf("\n------------------------------------- \n");

    rv = stop_ebst_monitor(unit);
    if (rv != BCM_E_NONE) {
        printf("stop_ebst_monitor() failed. : %s\n", bcm_errmsg(rv));
    }

    if(test_failed != 0) {
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}


bcm_error_t execute()
{
    bcm_error_t rv;
    int unit =0;
    bshell(unit, "config show; a ; version");

    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return -1;
    }

    if (BCM_FAILURE((rv = ebst_config(unit)))) {
        printf("ebst_config() failed.\n");
        return -1;
    }

    if (BCM_FAILURE((rv = test_verify(unit)))) {
        printf("test_verify() failed.\n");
        return -1;
    }

    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
    print execute();
}

