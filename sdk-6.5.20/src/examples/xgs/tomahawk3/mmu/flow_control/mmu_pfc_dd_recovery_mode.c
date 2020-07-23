/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 Test Setup: TH3 in 32x400G (NRZ) mode
 external loopback to connect cd0(1) <-> cd1(2) (prilrack 3.9)
 */


cint_reset();
int rx_port=1;
int tx_port = 2;
int profile_id = 0;

/* Setup MMU in lossless mode */
bshell (0, "config add mmu_lossless=1");

/*Modify flex_enable and MTU size to aloow more number of PFC-optimized
 * classes*/
bshell (0, "debug bcm cosq warn");
bshell(0, "rc");

/* Program Pipeline for traffic flooding to different queues */
bshell(0, "mod egr_vlan 1 1 ut_port_bitmap=0x0");
bshell(0, "mod port 0 160 port_bridge=1");
bshell(0, "insert l2x mac_addr=0x2 vlan_id=1 tgid=1 base_valid=1");

/* Enable PFC receive and transmit on the PFC partners */
print bcm_port_control_set(0, tx_port, bcmPortControlPFCTransmit, 1);
print bcm_port_control_set(0, rx_port, bcmPortControlPFCReceive, 1);

/* Map priorities 0 to PG7 */
int pg_array[8] = {7, 6, 5, 4, 3, 2, 1, 0};
print bcm_cosq_priority_group_pfc_priority_mapping_profile_set(0, 0, 8, pg_array);

/* Scheduler mapping for profile profile_id */
/* Default mode with 8 UC and 4 MC */
/* Cos0-3 has 1-UC+1-MC pair, cos4-7 has resp.UC queue */
bcm_cosq_mapping_t cosq_mapping_arr[12];
cosq_mapping_arr[0].cosq=0;
cosq_mapping_arr[0].num_ucq=1;
cosq_mapping_arr[0].num_mcq=1;
cosq_mapping_arr[0].strict_priority=0;
cosq_mapping_arr[0].fc_is_uc_only=1;
cosq_mapping_arr[1].cosq=1;
cosq_mapping_arr[1].num_ucq=1;
cosq_mapping_arr[1].num_mcq=1;
cosq_mapping_arr[1].strict_priority=0;
cosq_mapping_arr[1].fc_is_uc_only=0;
cosq_mapping_arr[2].cosq=2;
cosq_mapping_arr[2].num_ucq=1;
cosq_mapping_arr[2].num_mcq=1;
cosq_mapping_arr[2].strict_priority=0;
cosq_mapping_arr[2].fc_is_uc_only=0;
cosq_mapping_arr[3].cosq=3;
cosq_mapping_arr[3].num_ucq=1;
cosq_mapping_arr[3].num_mcq=1;
cosq_mapping_arr[3].strict_priority=0;
cosq_mapping_arr[3].fc_is_uc_only=0;
cosq_mapping_arr[4].cosq=4;
cosq_mapping_arr[4].num_ucq=1;
cosq_mapping_arr[4].num_mcq=0;
cosq_mapping_arr[4].strict_priority=0;
cosq_mapping_arr[4].fc_is_uc_only=0;
cosq_mapping_arr[5].cosq=5;
cosq_mapping_arr[5].num_ucq=1;
cosq_mapping_arr[5].num_mcq=0; 
cosq_mapping_arr[5].strict_priority=0;
cosq_mapping_arr[5].fc_is_uc_only=0;
cosq_mapping_arr[6].cosq=6; 
cosq_mapping_arr[6].num_ucq=1; 
cosq_mapping_arr[6].num_mcq=0; 
cosq_mapping_arr[6].strict_priority=0;
cosq_mapping_arr[6].fc_is_uc_only=0;
cosq_mapping_arr[7].cosq=7;
cosq_mapping_arr[7].num_ucq=1;
cosq_mapping_arr[7].num_mcq=0;
cosq_mapping_arr[7].strict_priority=0;
cosq_mapping_arr[7].fc_is_uc_only=0;
cosq_mapping_arr[8].cosq=8;
cosq_mapping_arr[8].num_ucq=0;
cosq_mapping_arr[8].num_mcq=0;
cosq_mapping_arr[8].strict_priority=0;
cosq_mapping_arr[8].fc_is_uc_only=0;
cosq_mapping_arr[9].cosq=9;
cosq_mapping_arr[9].num_ucq=0;
cosq_mapping_arr[9].num_mcq=0;
cosq_mapping_arr[9].strict_priority=0;
cosq_mapping_arr[9].fc_is_uc_only=0;
cosq_mapping_arr[10].cosq=10;
cosq_mapping_arr[10].num_ucq=0;
cosq_mapping_arr[10].num_mcq=0;
cosq_mapping_arr[10].strict_priority=0;
cosq_mapping_arr[10].fc_is_uc_only=0;
cosq_mapping_arr[11].cosq=11;
cosq_mapping_arr[11].num_ucq=0;
cosq_mapping_arr[11].num_mcq=0;
cosq_mapping_arr[11].strict_priority=0;
cosq_mapping_arr[11].fc_is_uc_only=0;
print bcm_cosq_schedq_mapping_set(0, profile_id, 12, cosq_mapping_arr);


/* Configure PFC receive for profile profile_id */
/* Priority 0 &4 are PFC-optimized */
bcm_cosq_pfc_class_map_config_t config_array1[8];
bcm_cosq_pfc_class_map_config_t_init(&config_array1[0]);

config_array1[0].pfc_enable = 1;
config_array1[0].cos_list_bmp |= 1 << 3;
config_array1[0].pfc_optimized = 0;
config_array1[1].pfc_enable = 1;
config_array1[1].cos_list_bmp |= 1 << 6;
config_array1[1].pfc_optimized = 0;
config_array1[2].pfc_enable = 1;
config_array1[2].cos_list_bmp |= 1 << 5;
config_array1[2].pfc_optimized = 0;
config_array1[3].pfc_enable = 1;
config_array1[3].cos_list_bmp |= 1 << 4;
config_array1[3].pfc_optimized = 0;
config_array1[4].pfc_enable = 1;
config_array1[4].cos_list_bmp |= 1 << 0;
config_array1[4].pfc_optimized = 1;
config_array1[5].pfc_enable = 1;
config_array1[5].cos_list_bmp |= 1 << 2;
config_array1[5].pfc_optimized = 0;
config_array1[6].pfc_enable = 1;
config_array1[6].cos_list_bmp |= 1 << 1;
config_array1[6].pfc_optimized = 0;
config_array1[7].pfc_enable = 1;
config_array1[7].cos_list_bmp |= 1 << 7;
config_array1[7].pfc_optimized = 0;

print bcm_cosq_pfc_class_config_profile_set(0, profile_id, 8, config_array1);

/* Map rx_port to use profile 1*/
print bcm_cosq_port_profile_set(0, rx_port, bcmCosqProfilePFCAndQueueHierarchy,
        profile_id);


/* Flood traffic for 8 UC and 4 MC queues */
bshell(0, "tx 1000 length=512 dm=0x2 vlanprio=0 vlantag=1");
bshell (0, "sleep 1");

bshell(0, "sleep 1");

bshell (0, "echo 'Show c with traffic on all queues'");
bshell(0, "show c cd0");

/*Create congestion by metering on PFC generate port */
print bcm_port_rate_egress_set(0,tx_port,64,64);
/* Modify PG limits to generate early PFC */
print bcm_cosq_control_set (0, tx_port, 7,
        bcmCosqControlIngressPortPGSharedDynamicEnable, 0);

bshell(0, "clear c");
bshell(0, "sleep 1");
bshell(0, "echo 'Show c with PFC frames reception and paused Queues'");
bshell(0, "show c cd0");

/* Enable PFC deadlock detection for priority 0 */
print bcm_cosq_pfc_deadlock_recovery_event_register(0, NULL, NULL);
print bcm_cosq_pfc_deadlock_control_set(0, rx_port, 0,
        bcmCosqPFCDeadlockDetectionTimer, 10);
print bcm_cosq_pfc_deadlock_control_set(0, rx_port, 0,
        bcmCosqPFCDeadlockTimerGranularity, bcmCosqPFCDeadlockTimerInterval1MiliSecond);
bshell (0,"echo 'Enable debug to show PFC deadlock interrupt triggered'");
bshell (0, "debug bcm cosq");
print bcm_cosq_pfc_deadlock_control_set(0, rx_port, 0, bcmCosqPFCDeadlockDetectionAndRecoveryEnable, 1);

int i;
for (i = 0; i < 2; i++) {
    bshell(0, "echo 'Change mode to use PFCState'");
    print bcm_cosq_pfc_dd_recovery_mode_set(0, rx_port, bcmCosqPfcDdRecoveryModePFCState);
    bshell(0, "sleep 1");
    bshell(0, "echo 'Change mode to use Timer'");
    print bcm_cosq_pfc_dd_recovery_mode_set(0, rx_port, bcmCosqPfcDdRecoveryModeTimer);
    bshell(0, "sleep 1");
}
bshell(0, "show c cd0");

/*Below are the two ways to end recovery */

/* 1. remap pg to pfc_pri mapping to unmap PG7 to priority 0 so, priority 0 doesnt
 * receive any more flow control*/
/*pg_array[0] = 6;
print bcm_cosq_priority_group_pfc_priority_mapping_profile_set(0, 0, 8,
pg_array);*/

/* 2. Disable deadlock detection and recovery */
/*print bcm_cosq_pfc_deadlock_control_set(0, rx_port, 0,
 * bcmCosqPFCDeadlockDetectionAndRecoveryEnable, 0);*/


/*Warm boot testing */
/* Exit after sync */
/*bshell (0, "sc controlsync=1");
bshell(0, "exit clean");*/

/* reload in warmboot */

/*Use either of above 2 ways to end deadlock situation */
