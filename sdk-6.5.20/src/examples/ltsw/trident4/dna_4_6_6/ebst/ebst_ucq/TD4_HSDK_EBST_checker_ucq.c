/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
void
ebst_checker_ucq(int unit)
{
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
	print bcm_cosq_ebst_monitor_set(unit, &object_id, bcmBstStatIdUcast, &monitor);

	/* STEP 2. Read EBST Status */
	bcm_cosq_ebst_data_entry_t stat_array[100];
	int array_count = 0;
	object_id.cosq = uc_cosq;
	object_id.buffer = 0;
	print bcm_cosq_ebst_data_get(unit, &object_id, bcmBstStatIdUcast, 100, stat_array, &array_count);
	printf("\nEBST_DATA Count ...\n");
	print array_count;
	bshell(unit, "sleep quiet 5");
	printf("\nEBST_DATA Status ...\n");
	print stat_array;

	/* STEP 3. Stop system level EBST scan */
	printf("\nStop system level EBST scan\n");
	print bcm_cosq_ebst_control_set(0, bcmCosqEbstControlScanEnable, 0);
}

ebst_checker_ucq(0);