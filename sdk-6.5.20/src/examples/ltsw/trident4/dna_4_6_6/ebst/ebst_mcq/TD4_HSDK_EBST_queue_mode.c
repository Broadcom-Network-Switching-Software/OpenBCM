/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : EBST threshold profile configuration on a per uc/mc queue per port basis
 * Usage    : BCM.0> cint TD4_HSDK_EBST_queue_mode.c
 * config   : bcm56880_a0-generic-32x400_vxlan.config.yml
 * Log file : TD4_HSDK_EBST_queue_mode_mcq_test.log
 *
 * Purpose: Demonstrate EBST threshold profile configuration for UC and MC queues on a selected port
 *
 * Description: This example demonstrates the sequence to configure EBST to monitor 
 *              unicast queue 0 and multicast queue 0 on xe0 (logical port 38.) 
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
 * Test Method (MC Case): Apply the following scripts one by one in sequence:
 *	 1. TD4_HSDK_EBST_queue_mode.c
 *   2. TD4_HSDK_EBST_test_run_ebst_dlf.c
 *   3. TD4_HSDK_EBST_checker_mcq.c 
 *   and read the status of EBST_DATA buffer of the unicast queue.
 */
/* Reset C interpreter*/
cint_reset();

/* STEP 1: Top level variables initialization and BST control configuration */
/* In HSDK, logical port number is used always for port parameter */
int unit = 0;
bcm_gport_t  observation_gport = 38;   /* logical port number of xe0 with hsdk-all-6.5.18-EA6 */
bcm_cos_queue_t uc_cosq = 0;
bcm_cos_queue_t mc_cosq = 8;

/* 
   Set BST tracking mode to instantaneous mode (instead of high watermark mode)
   disable BST snapshot
   enable BST counters
*/
BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchBstTrackingMode, 0)); 
BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchBstSnapshotEnable, 0));
BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchBstEnable, TRUE));

/* STEP 2: Register CB function for EBST FIFO full event */
void func1(int unit, bcm_switch_event_t event, uint32 arg1, 
uint32 arg2, uint32 arg3, void *userdata)
{
    if(event == BCM_SWITCH_EVENT_MMU_EBST_FIFO_FULL)
    {
        printf("HW FIFO getting full in buffer %d. 
Please increase EBST scan round.\n", arg1);
    
	/* Increse the SCAN_ROUND to a relatively larger value to recover from 
	   FIFO full situation. Start the EBST operation again */
        print bcm_cosq_ebst_control_set(unit, bcmCosqEbstControlScanInterval, 6000);
        print bcm_cosq_ebst_control_set(unit, bcmCosqEbstControlScanEnable , 1);

    }
    return;
}

print bcm_switch_event_register(unit, func1, NULL);

/* STEP 3: Configure start and stop threshold for the profile. Associate the UC and MC queue with the profile id.*/
bcm_cosq_object_id_t object_id;
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
print bcm_cosq_ebst_threshold_profile_set(unit, &object_id, bcmBstStatIdUcast, &profile);
 
/* Set threshold for port 38 mc queue 0 */
/* 
mc_cosq=8 is the first mc cosq for the port.
TM_THD_MC_Q table entry key: port = 1, TM_MC_Q_ID=0
*/
object_id.cosq = mc_cosq; 

print bcm_cosq_ebst_threshold_profile_set(unit, &object_id, bcmBstStatIdMcast, &profile);

/* STEP 4: Configure Monitoring on (Port,Queue) */
bcm_cosq_ebst_monitor_t monitor;
bcm_cosq_ebst_monitor_t_init(&monitor);

monitor.flags |= (BCM_COSQ_EBST_MONITOR_ENABLE_VALID | BCM_COSQ_EBST_MONITOR_ENTRY_NUM_VALID);
monitor.enable = 1;      /* enable a port/queue should be performed when the EBST is not in EBST_START state */
monitor.entry_num = 100;
object_id.cosq = uc_cosq;

/* Set ebst monitor on port 38 uc queue 0 */
print bcm_cosq_ebst_monitor_set(unit, &object_id, bcmBstStatIdUcast, &monitor);

object_id.cosq = mc_cosq;
print bcm_cosq_ebst_monitor_set(unit, &object_id, bcmBstStatIdMcast, &monitor);
 
/* STEP 5: Configure the SCAN_ROUND and SCAN_MODE */
/* Target LT: TM_EBST_CONTROL */
print bcm_cosq_ebst_control_set(unit, bcmCosqEbstControlScanInterval, 50);
print bcm_cosq_ebst_control_set(unit, bcmCosqEbstControlScanMode, bcmCosqEbstScanModeQueue);

/* STEP 6:  Start EBST scan */
print bcm_cosq_ebst_enable_set(unit, 1);  /* EBST_START state */
