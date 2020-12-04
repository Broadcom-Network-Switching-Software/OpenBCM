/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : EBST threshold profile configuration on a per uc queue per port basis
 *
 * Usage    : BCM.0> cint td4x9_hsdk_ebst_ucq.c
 *
 * config   : bcm56780_a0-generic-48x100-8x400_ebst.config.yml
 *
 * Log file : td4x9_hsdk_ebst_ucq_log.txt
 *
 * Test Topology :
 *
 *                  +---------------------+
 *                  |                     |
 *                  |                     |        (loopback; VLAN=1)
 *                  |                     |  ce0         +---+
 *                  |                     +--------------+   |
 *                  |         DUT         |              +---+
 *                  |                     |
 *                  |                     |        (loopback; VLAN=1)
 *                  |  +---------------+  |  ce1         +---+
 *                  |  | UCQ EBST_DATA |  +--------------+   |
 *                  |  +---------------+  |              +---+
 *                  |                     |        egress port (egress rate limiting applied)
 *                  +---------------------+
 *
 * Summary:
 * ========
 * CINT example to demonstrate Even Buffer Statistics Tracking (EBST) capability (UCQ case)
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Configure DUT for EBST test with egress rate limiting applied on the observation port
 *     b) CB function registration
 *
 *   2) Step2 - Configuration (Done in ebst_setup()):
 *   =========================================================
 *     a) EBST control configuration
 *     b) Set BST tracking mode to instantaneous mode (instead of high watermark mode)
 *        - Disable BST snapshot
 *        - Enable BST counters
 *     c) Set threshold for UC Queue 0 (as well as MC Queue 8 for the ease of presentation) of the egress port selected 
 *     d) Configure Monitoring on (Port,Queue)
 *     e) Set ebst monitor on egress port UC Queue 0 (as well as MC Queue 8 for the ease of presentation)
 *     f) Configure the SCAN_ROUND and SCAN_MODE
 *     g) Start EBST scan
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) CPU sends 30 1532-byte unicast packets to loopback port ingress port
 *     b) Show port status and packet counts of ingress port and egress port on console by applying "ps" and "show c"
 *     c) Read the status of EBST_DATA buffer of the unicast queue with respect to the egress port
 *     d) Expected Result:
 *     ===================
 *     Dump of EBST data buffer stat_array shows timestamp of the scan and the buffer count in cells.  
 *     When EBST is enabled, a background hardware process scans the BST counters periodically.
 *     If BST count crosses the start threshold of a particular BST counter (queue / PortSP),
 *     it copies the occupancy level information to EBST Data buffer per ITM.
 *     This information includes the timestamp of the scan and the buffer count in cells.
 *     The above-mentioned information can be retrieved by reading EBST_DATA buffer of the unicast queue.
 */

/* Reset C interpreter*/
cint_reset();

int unit = 0;
bcm_gport_t observation_gport; 
bcm_cos_queue_t uc_cosq = 0;
bcm_cos_queue_t mc_cosq = 8;
bcm_cosq_object_id_t object_id;
bcm_cosq_ebst_monitor_t monitor;
bcm_cosq_ebst_monitor_t_init(&monitor);
bcm_port_t egress_port;
bcm_port_t ingress_port;
bcm_gport_t egress_phy_gport;
bcm_gport_t ingress_phy_gport;

/*
 * This function is written so that hardcoding of port
 * numbers in Cint scripts is removed. This function gives
 * required number of ports
 */

bcm_error_t
portNumbersGet(int unit, int *port_list, int num_ports)
{
    int i = 0, port = 0, rv = 0;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;

    rv = bcm_port_config_get(unit, &configP);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_port_config_get: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    ports_pbmp = configP.e;
    for (i= 1; i < BCM_PBMP_PORT_MAX; i++) {
        if (BCM_PBMP_MEMBER(&ports_pbmp, i) && (port < num_ports)) {
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
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified port to CPU.
 */
bcm_error_t
ingress_port_multi_setup(int unit, bcm_pbmp_t pbm)
{
  bcm_port_t     port;
  BCM_PBMP_ITER(pbm, port) {
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
  }
    return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified port to CPU. Port is also configured
 * to discard all packets. This is to avoid continuous
 * loopback of the packet from the egress port.
 */
bcm_error_t
egress_port_multi_setup(int unit, bcm_pbmp_t pbm)
{
  bcm_port_t     port;
  BCM_PBMP_ITER(pbm, port) {
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
  }
  return BCM_E_NONE;
}

/* Register CB function for EBST FIFO full event */
void bst_callback(int unit, bcm_switch_event_t event, uint32 arg1,
                  uint32 arg2, uint32 arg3, void *userdata)
{
  if (event == BCM_SWITCH_EVENT_MMU_EBST_FIFO_FULL)
  {
    printf("HW FIFO getting full in buffer %d.    
           Please increase EBST scan round.\n", arg1);

    /* Increse the SCAN_ROUND to a relatively larger value to recover from
       FIFO full situation. Start the EBST operation again */
    BCM_IF_ERROR_RETURN(bcm_cosq_ebst_control_set(unit, bcmCosqEbstControlScanInterval, 10000));
    BCM_IF_ERROR_RETURN(bcm_cosq_ebst_control_set(unit, bcmCosqEbstControlScanEnable , 1));

  }
  return BCM_E_NONE;
}

/*
 * Test Setup:
 * This functions gets the port numbers and sets up ingress and
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */

bcm_error_t
test_setup(int unit)
{
    int num_ports = 2;
    int port_list[num_ports];

    if (BCM_E_NONE != portNumbersGet(unit, port_list, num_ports)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    ingress_port = port_list[0];
    egress_port = port_list[1];

    if (BCM_E_NONE !=bcm_port_gport_get(unit, ingress_port, &(ingress_phy_gport))){
        printf("bcm_port_gport_get() failed to get gport for port %d\n", ingress_port);
        return -1;
    }

    if (BCM_E_NONE != bcm_port_gport_get(unit, egress_port, &(egress_phy_gport))){
        printf("bcm_port_gport_get() failed to get gport for port %d\n", egress_port);
        return -1;
    }
    bcm_pbmp_t  pbmp;
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);
    if (BCM_E_NONE != ingress_port_multi_setup(unit, pbmp)) {
        printf("ingress_port_multi_setup() failed for port %d\n", ingress_port);
        return -1;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, egress_port);
    if (BCM_E_NONE != egress_port_multi_setup(unit, pbmp)) {
        printf("egress_port_multi_setup() failed for port %d\n", egress_port);
        return -1;
    }

    /* Apply egress rate limiting on the observation port xe0 */ 
    BCM_IF_ERROR_RETURN(bcm_port_rate_egress_set(unit, egress_port, 512, 256));
     
    /* CB function registration */
    BCM_IF_ERROR_RETURN(bcm_switch_event_register(unit, bst_callback, NULL));

    return BCM_E_NONE;
}

bcm_error_t ebst_setup(int unit)
{

  /* EBST control configuration */
  /* 
    STEP 1: Set BST tracking mode to instantaneous mode (instead of high watermark mode)
    disable BST snapshot
    enable BST counters
  */
   BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchBstTrackingMode, 0)); 
   BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchBstSnapshotEnable, 0));
   BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchBstEnable, TRUE));

  /* STEP 2: Configure start and stop threshold for the profile. Associate the UC and MC queue with the profile id.*/
  object_id.port = egress_port;
  object_id.cosq = uc_cosq;
  object_id.buffer = -1;
  int rv;
  bcm_cosq_ebst_threshold_profile_t profile;
  bcm_cosq_ebst_threshold_profile_t_init(&profile);
  profile.start_bytes = 1000;
  profile.stop_bytes = 800;

  /* Assume TM_SCHEDULER_CONFIG.NUM_MC_Q=4 has been set in config.yml 
     There are 12 queues per port. 
  */

  /* Set threshold for egress port queue 0 */
  BCM_IF_ERROR_RETURN(bcm_cosq_ebst_threshold_profile_set(unit, &object_id, bcmBstStatIdUcast, &profile));
 
  /* Set threshold for egress port mc queue 0 */
  /* 
  mc_cosq=8 is the first mc cosq for the port.
  TM_THD_MC_Q table entry key: port = 1, TM_MC_Q_ID=0
  */
  object_id.cosq = mc_cosq; 

  BCM_IF_ERROR_RETURN(bcm_cosq_ebst_threshold_profile_set(unit, &object_id, bcmBstStatIdMcast, &profile));

  /* STEP 3: Configure Monitoring on (Port,Queue) */
  monitor.flags |= (BCM_COSQ_EBST_MONITOR_ENABLE_VALID | BCM_COSQ_EBST_MONITOR_ENTRY_NUM_VALID);
  monitor.enable = 1;      /* enable a port/queue should be performed when the EBST is not in EBST_START state */
  monitor.entry_num = 100;
  object_id.cosq = uc_cosq;

  /* Set ebst monitor on egress port uc queue 0 */
  BCM_IF_ERROR_RETURN(bcm_cosq_ebst_monitor_set(unit, &object_id, bcmBstStatIdUcast, &monitor));

  object_id.cosq = mc_cosq;
  BCM_IF_ERROR_RETURN(bcm_cosq_ebst_monitor_set(unit, &object_id, bcmBstStatIdMcast, &monitor));
 
  /* STEP 4: Configure the SCAN_ROUND and SCAN_MODE */
  /* Target LT: TM_EBST_CONTROL */
  BCM_IF_ERROR_RETURN(bcm_cosq_ebst_control_set(unit, bcmCosqEbstControlScanInterval, 50));
  BCM_IF_ERROR_RETURN(bcm_cosq_ebst_control_set(unit, bcmCosqEbstControlScanMode, bcmCosqEbstScanModeQueue));
  BCM_IF_ERROR_RETURN(bcm_cosq_ebst_control_set(unit, bcmCosqEbstControlScanEnable , 1));

  /* STEP 5: Start EBST scan */
  BCM_IF_ERROR_RETURN(bcm_cosq_ebst_enable_set(unit, 1));  /* EBST_START state */
	
  return BCM_E_NONE;
}


void verify(int unit)
{
  char str[512];

  /* CPU start to send unicast traffic */
  printf("\nCPU start sending known unicast packet\n");
  bshell(unit, "vlan remove 1 pbm=cpu");
  snprintf(str, 512, "l2 add port=%d mac=0x2 vlanid=0x1 static=true", egress_port);
  bshell(unit, str);
  snprintf(str, 512, "tx 30 pbm=%d length=1532 vlantag=0x1 sm=0x1 dm=0x2; sleep quiet 5", ingress_port);
  bshell(unit, str);
  snprintf(str, 512, "ps %d; ps %d", ingress_port, egress_port);
  bshell(unit, str);
  bshell(unit, "show c");
	
  /* STEP 1. Remove the queue from the EBST scan to read the status of EBST_DATA of the unicast queue.
             Disable ebst monitor on uc queue 0 of egress port */
  monitor.flags &= ~BCM_COSQ_EBST_MONITOR_ENTRY_NUM_VALID;
  monitor.enable = 0;
  object_id.cosq = uc_cosq;
  BCM_IF_ERROR_RETURN(bcm_cosq_ebst_monitor_set(unit, &object_id, bcmBstStatIdUcast, &monitor));

  /* STEP 2. Read EBST Status */
  bcm_cosq_ebst_data_entry_t stat_array[100];
  int array_count = 0;
  object_id.cosq = uc_cosq;
  object_id.buffer = 0;
  BCM_IF_ERROR_RETURN(bcm_cosq_ebst_data_get(unit, &object_id, bcmBstStatIdUcast, 100, stat_array, &array_count));
  printf("\nEBST_DATA Count ...\n");
  print array_count;
  bshell(unit, "sleep quiet 5");
  printf("\nEBST_DATA Status ...\n");
  print stat_array;

  /* STEP 3. Stop system level EBST scan */
  printf("\nStop system level EBST scan\n");
  BCM_IF_ERROR_RETURN(bcm_cosq_ebst_control_set(0, bcmCosqEbstControlScanEnable, 0));
	
  return BCM_E_NONE;	
}

bcm_error_t execute()
{
  int rrv;
    
  if((rrv = test_setup(0)) != BCM_E_NONE)
  {
      printf("Creating the test setup failed %d\n", rrv);
      return rrv;
  }
    
  if( (rrv = ebst_setup(0)) != BCM_E_NONE )
  {
      printf("Test run ebst failed with %d\n", rrv);
      return rrv;
  }
    
  if( (rrv = verify(0)) != BCM_E_NONE )
  {
      printf("EBST verify ucq failed with %d\n", rrv);
      return rrv;
  }
    
  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}
