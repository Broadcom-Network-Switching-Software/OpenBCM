/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : EBST threshold profile configuration on a per uc queue per port basis
 * Usage    : BCM.0> cint TD4_HSDK_EBST_ucq.c
 * config   : bcm56880_a0-generic-32x400_vxlan.config.yml
 * Log file : TD4_HSDK_EBST_ucq_test.log
 *
 * Purpose: Demonstrate EBST threshold profile configuration for UC queue on a selected port
 *
 * Description: This example demonstrates the sequence to configure EBST to monitor 
 *              unicast Queue 0 on xe0 (logical port 38.) 
 *              The start threshold value is 1000 cells and stop threshold value is 800 cells.
 *
 * Background: For EBST, all controls and configurations are per device in LT view.
 * 
 * 1. BST Logical Table (counters related to EBST)
 *   - CTR_EGR_TM_BST_PORT_SERVICE_POOL
 *   - CTR_EGR_TM_BST_UC_Q
 *   - CTR_EGR_TM_BST_MC_Q
 * 
 * 2. EBST Control/Status Tables
 *   - TM_EBST_CONTROL
 *   - TM_EBST_STATUS
 * 
 * 3. MMU egress admission control and EBST profile tables
 *   - TM_THD_UC_Q
 *   - TM_THD_MC_Q
 *   - TM_EGR_THD_UC_PORT_SERVICE_POOL
 *   - TM_EGR_THD_MC_PORT_SERVICE_POOL
 *   - TM_EBST_PROFILE
 * 
 * 4. EBST resource tables
 *   - TM_EBST_PORT
 *   - TM_EBST_PORT_SERVICE_POOL
 *   - TM_EBST_UC_Q
 *   - TM_EBST_MC_Q
 * 
 * 5. EBST data table
 *   - TM_EBST_DATA
 * 
 * Test Topology :
 * CPU sends unicast traffic to cd0; egress rate limiting is applied on the observation port xe0
 * 
 *                  +---------------------+      
 *                  |                     |                             
 *                  |                     |        (loopback; VLAN=1)    
 *                  |                     |  cd0         +---+
 *                  |                     +--------------+   |
 *                  |      Trident4       |              +---+  
 *                  |                     |           
 *                  |                     |        (loopback; VLAN=1) 
 *                  |  +---------------+  |  xe0         +---+         
 *                  |  | UCQ EBST_DATA |  +--------------+   |
 *                  |  +---------------+  |              +---+  
 *                  |                     |        observation port (egress rate limiting applied)
 *                  +---------------------+                         
 *
 * Summary:
 * ========
 * CINT example to demonstrate Even Buffer Statistics Tracking (EBST) capability on Trident4 (UCQ case)
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
 *     c) Set threshold for UC Queue 0 (as well as MC Queue 8 for the ease of presentation) of the observation port xe0 (Port 38)
 *     d) Configure Monitoring on (Port,Queue)
 *     e) Set ebst monitor on Port 38 (the observation port) UC Queue 0 (as well as MC Queue 8 for the ease of presentation)
 *     f) Configure the SCAN_ROUND and SCAN_MODE
 *     g) Start EBST scan
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) CPU sends 30 1532-byte unicast packets to loopback port cd0. and 
 *     b) Show port status and packet counts of cd0 and observation port xe0 on console by applying "ps cd0,xe0" and "show c"
 *     c) Read the status of EBST_DATA buffer of the unicast queue with respect to the observation port
 *     d) Expected Result:
 *     ===================
 * 	   When EBST is enabled, a background hardware process scans the BST counters periodically. 
 *	   If BST count crosses the start threshold of a particular BST counter (queue / PortSP), 
 *	   it copies the occupancy level information to EBST Data buffer per ITM.
 *	   This information includes the timestamp of the scan and the buffer count in cells. 
 *     The above-mentioned information can be retrieved by reading EBST_DATA buffer of the unicast queue.
 */
 
/* Reset C interpreter*/
cint_reset();

/* Top level variables initialization and CB function declaration */
int unit = 0;
bcm_gport_t  observation_gport = 38;   /* logical port number of xe0 with hsdk-all-6.5.20 */
bcm_cos_queue_t uc_cosq = 0;
bcm_cos_queue_t mc_cosq = 8;
bcm_cosq_object_id_t object_id;
bcm_cosq_ebst_monitor_t monitor;
bcm_cosq_ebst_monitor_t_init(&monitor);

/* Register CB function for EBST FIFO full event */
void func1(int unit, bcm_switch_event_t event, uint32 arg1, 
uint32 arg2, uint32 arg3, void *userdata)
{
    if(event == BCM_SWITCH_EVENT_MMU_EBST_FIFO_FULL)
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


bcm_error_t test_setup(int unit)
{
	/* Using CPU tx to send unicast traffic in verifying status of EBST_DATA for the unicast queue */
	/* Test Setup */
	printf("\nTest Setup and Apply egress rate limiting on the observation port xe0\n");
	bshell(unit, "port cd0,xe0 lb=mac");
	bshell(unit, "vlan remove 1 pbm=cpu");
	bshell(unit, "l2 add port=xe0 mac=0x2 vlanid=0x1 static=true");
	bshell(unit, "port cd0,xe0 lb=mac");
	BCM_IF_ERROR_RETURN(bcm_port_rate_egress_set(unit, 38, 512, 256));  /* Apply egress rate limiting on the observation port xe0 */

    /* CB function registration */
    BCM_IF_ERROR_RETURN(bcm_switch_event_register(unit, func1, NULL));	
	
    return BCM_E_NONE;	
	
}	


bcm_error_t ebst_setup(int unit)
{
/* In HSDK, logical port number is used always for port parameter */

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
    object_id.port = observation_gport;
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

 /* Set threshold for port 38 uc queue 0 */
    BCM_IF_ERROR_RETURN(bcm_cosq_ebst_threshold_profile_set(unit, &object_id, bcmBstStatIdUcast, &profile));
 
/* Set threshold for port 38 mc queue 0 */
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

/* Set ebst monitor on port 38 uc queue 0 */
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


void
verify(int unit)
{
	/* CPU start to send unicast traffic */
	printf("\nCPU start sending known unicast packet\n");
    bshell(unit, "tx 30 pbm=cd0 length=1532 vlantag=0x1 sm=0x1 dm=0x2");
	
	bshell(unit, "sleep quiet 5");
	bshell(unit, "ps cd0,xe0");
	bshell(unit, "show c");
	
    /* When EBST is enabled, a background hardware process scans the BST counters periodically. 
	   If BST count crosses the start threshold of a particular BST counter (queue / PortSP), 
	   it copies the occupancy level information to EBST Data buffer per ITM. 
	   This information includes the timestamp of the scan and the buffer count in cells. 
	   To read the records, recommend to stop monitor for a EBST resource or stop EBST scan. 
	   The records will be not keep during warmboot and will be cleared when EBST disable. 
	   Note: to use this api the buffer_id in object_id should be specified. */
	
	/* STEP 1. Remove the queue from the EBST scan to read the status of EBST_DATA of the unicast queue.
	           Disable ebst monitor on uc queue 0 of observation port 38 */
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
    
    if( (rrv = test_setup(0)) != BCM_E_NONE )
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
