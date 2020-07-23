/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
cint_reset();

int unit = 0;

bcm_cosq_object_id_t object_id;
object_id.port = 1;
object_id.cosq = 0;
object_id.buffer = -1;
int rv;

/*** Configure start and stop threshold for the profile. Associate the UC and MC queue with the profile id. ***/
bcm_cosq_ebst_threshold_profile_t profile;
bcm_cosq_ebst_threshold_profile_t_init(&profile);
profile.start_bytes = 1000;
profile.stop_bytes = 800;

/*
 * Assume TM_SCHEDULER_CONFIG NUM_MC_Q=4 has been set in config.yml
 */

 /* Set threshold for port 1 uc queue 0 */
print bcm_cosq_ebst_threshold_profile_set(unit, &object_id, bcmBstStatIdUcast, &profile);
 /* Set threshold for port 1 mc queue 0 */
object_id.cosq = 8;
print bcm_cosq_ebst_threshold_profile_set(unit, &object_id, bcmBstStatIdMcast, &profile);


/*** Partition the TM_EBST_DATA table among the EBST resource LTs ***/
bcm_cosq_ebst_monitor_t monitor;
bcm_cosq_ebst_monitor_t_init(&monitor);

monitor.enable = 1;
monitor.entry_num = 1000;
monitor.flags = BCM_COSQ_EBST_MONITOR_ENTRY_NUM_VALID | BCM_COSQ_EBST_MONITOR_ENABLE_VALID;
object_id.cosq = 0;
/* Set ebst monitor on port 1 uc queue 0 */
print bcm_cosq_ebst_monitor_set(unit, &object_id, bcmBstStatIdUcast, &monitor);

/* Set ebst monitor on shared service pool 0 */
print bcm_cosq_ebst_monitor_set(unit, &object_id, bcmBstStatIdEgrPool, &monitor);

object_id.cosq = 8;
print bcm_cosq_ebst_monitor_set(unit, &object_id, bcmBstStatIdMcast, &monitor);

/*** Configure the SCAN_ROUND, SCAN_MODE and SCAN_THD using the TM_EBST_CONTROL LT. ***/
print bcm_cosq_ebst_control_set(unit, bcmCosqEbstControlScanInterval, 2000);

print bcm_cosq_ebst_control_set(unit, bcmCosqEbstControlScanMode, bcmCosqEbstScanModeQueue);

int threshold;
threshold = 100;
print bcm_cosq_ebst_control_set(unit, bcmCosqEbstControlScanThreshold, threshold);
threshold = -1;
print bcm_cosq_ebst_control_get(unit, bcmCosqEbstControlScanThreshold, &threshold);
print threshold;

print bcm_cosq_ebst_enable_set(unit, 1);

/*** Remove the EBST objects from the EBST scan ***/
monitor.enable = 0;
monitor.entry_num = -1;
object_id.cosq = 0;
monitor.flags = BCM_COSQ_EBST_MONITOR_ENABLE_VALID;
/* Set ebst monitor on port 1 uc queue 0 */
print bcm_cosq_ebst_monitor_set(unit, &object_id, bcmBstStatIdUcast, &monitor);

/* Set ebst monitor on service pool 0 */
print bcm_cosq_ebst_monitor_set(unit, &object_id, bcmBstStatIdEgrPool, &monitor);


/*** Read EBST stat from table TM_EBST_DATA ***/
bcm_cosq_ebst_data_entry_t stat_array[1000];
object_id.buffer = 0;
int array_count = 0;
object_id.cosq = 0;
print bcm_cosq_ebst_data_get(unit, &object_id, bcmBstStatIdUcast, 1000, stat_array, &array_count);
print array_count;

array_count = 0;
object_id.cosq = 0;
print bcm_cosq_ebst_data_get(unit, &object_id, bcmBstStatIdEgrPool, 1000, stat_array, &array_count);
print array_count;


/*** Stop the system level EBST scan ***/
print bcm_cosq_ebst_control_set(unit, bcmCosqEbstControlScanEnable, 0);

/*** Release internal resources ***/
print bcm_cosq_ebst_enable_set(unit, 0);

