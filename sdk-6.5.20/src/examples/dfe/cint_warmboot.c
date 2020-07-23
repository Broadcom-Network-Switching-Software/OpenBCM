/* 
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 *
 *Des:
 *  This test cfg sw info before warmboot and expecting to get the same sw info after warm boot.
 *
 *  first run bcm after cfg in config-bcm:
 *      "##warm boot
 *      stable_location=3
 *      stable_size=200000
 *      scache_filename=warm_boot.mem"
 *  run cint_dfe_cold_boot_set
 *  exit;
 *
 *  now run bcm again after:
 *  export SOC_BOOT_FLAGS=0x200000
 *  and run cint_dfe_warm_boot_check
 */

 /*global vars*/
    /*groups*/
    bcm_module_t group = BCM_FABRIC_GROUP_MODID_SET(5);
    int arr_set[2];
    arr_set[0] = 1;
    arr_set[1] = 2;

    /*locals*/
    bcm_module_t local = BCM_FABRIC_LOCAL_MODID_SET(5);
    bcm_module_t modid_set = 3;

    /*fifos*/
    bcm_fabric_link_threshold_type_t types[17]; 
    int values_set[17];

    /*set thresholds for fifo_type_rx_prim*/
    types[0] = bcmFabricLinkRxFifoLLFC;         values_set[0] = 28;
    types[1] = bcmFabricLinkRxGciLvl1FC;        values_set[1] = 28;
    types[2] = bcmFabricLinkRxGciLvl2FC;        values_set[2] = 28;
    types[3] = bcmFabricLinkRxGciLvl3FC;        values_set[3] = 28;
    types[4] = bcmFabricLinkRxPrio0Drop;        values_set[4] = 28;
    types[5] = bcmFabricLinkRxPrio1Drop;        values_set[5] = 28;
    types[6] = bcmFabricLinkRxPrio2Drop;        values_set[6] = 28;
    types[7] = bcmFabricLinkRxPrio3Drop;        values_set[7] = 28;

    /*mc*/
    bcm_multicast_t mc = 64;


int cint_dfe_cold_boot_set(int unit) {
    int rv, i;
    bcm_switch_event_control_t event;
    bcm_switch_event_control_t type;


    /*groups*/
    rv = bcm_fabric_modid_group_set(unit, group, 2, arr_set);
    if (rv != BCM_E_NONE) {
        printf("ERROR: in bcm_fabric_modid_group_set, failed to set a group, rv = %d.\n", rv);
        return rv;
    }
    int arr_get[5];
    int count;
    rv = bcm_fabric_modid_group_get(unit, group, 5, arr_get, &count);
    if (rv != BCM_E_NONE) {
        printf("ERROR: in bcm_fabric_modid_group_get, failed to get a group, rv = %d.\n", rv);
        return rv;
    }
    if (count != 2) {
        printf("ERROR: in bcm_fabric_modid_group: set != get.\n");
        return BCM_E_FAIL;
    }
    for (i=0; i < count; i++) {
         if (arr_get[i] != arr_set[i]) {
             printf("ERROR: in bcm_fabric_modid_group: set != get.\n");
             return BCM_E_FAIL;
         }
    }

    /*locals*/
    bcm_module_t modid_get;

    rv = bcm_fabric_modid_local_mapping_set(unit, local, modid_set);
    if (rv != BCM_E_NONE) {
        printf("ERROR: in bcm_fabric_modid_local_mapping_set, failed, rv = %d.\n", rv);
        return rv;
    }
    rv = bcm_fabric_modid_local_mapping_get(unit, local, &modid_get);
    if (rv != BCM_E_NONE) {
        printf("ERROR: in bcm_fabric_modid_local_mapping_get, failed, rv = %d.\n", rv);
        return rv;
    }
    if (modid_get != modid_set) {
        printf("ERROR: in bcm_fabric_modid_local_mapping: set != get.\n");
        return BCM_E_FAIL;
    }

    /*fifos*/
    int fifo_type_rx_prim;
    rv = bcm_fabric_link_threshold_add(unit, BCM_FABRIC_LINK_THRESHOLD_RX_FIFO_ONLY | BCM_FABRIC_LINK_TH_PRIM_ONLY, &fifo_type_rx_prim);
    if (rv != BCM_E_NONE) {
          printf("Error, in bcm_fabric_link_threshold_add BCM_FABRIC_LINK_THRESHOLD_RX_FIFO_ONLY | BCM_FABRIC_LINK_TH_PRIM_ONLY, rv=%d, \n", rv);
          return rv;
      }
    printf("Input for cint_dfe_warm_boot_check fuction: fifo_type_rx_prim = %d\n", fifo_type_rx_prim);
     
    int values_get[17];

    rv = bcm_fabric_link_thresholds_set(unit, fifo_type_rx_prim, 8, types, values_set);
        if (rv != BCM_E_NONE) {
          printf("Error, in bcm_fabric_link_thresholds_set for fifo_type_rx_prim, rv=%d, \n", rv);
          return rv;
      }
    rv = bcm_fabric_link_thresholds_get(unit, fifo_type_rx_prim, 8, types, values_get);
    if (rv != BCM_E_NONE) {
          printf("Error, in bcm_fabric_link_thresholds_get for fifo_type_rx_prim, rv=%d, \n", rv);
          return rv;
      }
    for (i=0; i<8; i++) {
        if (values_set[i] != values_get[i]) {
            printf("Error, in bcm_fabric_link_thresholds, get != set \n", rv);
            return BCM_E_FAIL;
        }
    }

    /*multicast*/
    rv = bcm_multicast_create(unit, BCM_MULTICAST_WITH_ID, &mc);
    if (rv != BCM_E_NONE) {
        printf("ERROR: in bcm_multicast_create, failed to create mc group, mc_id = %d, rv= %d.\n", mc, rv);
        return rv;
    }
    rv = bcm_multicast_create(unit, BCM_MULTICAST_WITH_ID, &mc); 
    if (rv != BCM_E_PARAM) {
        printf("ERROR: in bcm_multicast_create, expecting BCM_E_PARAM while trying to create existing mc group, mc_id = %d, rv= %d.\n", mc, rv);
        return rv;
    }

    /* print */
    event.action = bcmSwitchEventLog;

    /* set print enable */
    event.event_id = 70;
    event.index = 0;
    rv = bcm_switch_event_control_set(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT,event,1);
    if (rv != BCM_E_NONE) {
        printf("ERROR: in bcm_switch_event_control_set for enable switch event log rv = %d\n", rv);
        return rv;
    }
   
    /* set print disable */
    event.event_id = 14;
    event.index = 0;
    rv = bcm_switch_event_control_set(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT,event,0);
    if (rv != BCM_E_NONE) {
        printf("ERROR: in bcm_switch_event_control_set for disable switch event log rv = %d\n", rv);
        return rv;
    }
   
    /* override */

    /* override enable */
    event.event_id = 20;
    event.index = 0;
    event.action = bcmSwitchEventCorrActOverride;
    rv = bcm_switch_event_control_set(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT,event,6);
    if (rv != BCM_E_NONE) {
        printf("ERROR: in bcm_switch_event_control_set for enable  switch event override rv = %d\n", rv);
        return rv;
    }
 

    /* override disable */
    event.event_id = 22;
    event.index = 0;
    event.action = bcmSwitchEventCorrActOverride;
    rv = bcm_switch_event_control_set(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT,event,0);
    if (rv != BCM_E_NONE) {
        printf("ERROR: in bcm_switch_event_control_set for disable  switch event override rv = %d\n", rv);
        return rv;
    }


    /* priority */

    event.action = bcmSwitchEventPriority;
    event.index = 0;
    event.event_id = 99;
    rv = bcm_switch_event_control_set(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT,event,6);
    if (rv != BCM_E_NONE) {
        printf("ERROR: in bcm_switch_event_control_set for set priority  for id 99 rv = %d\n", rv);
        return rv;
    }
    
    event.event_id = 107;
    event.index = 0;
    rv = bcm_switch_event_control_set(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT,event,13);
    if (rv != BCM_E_NONE) {
        printf("ERROR: in bcm_switch_event_control_set for set priority  for id 107 rv = %d\n", rv);
        return rv;
    }
   

    event.event_id = 118;
    event.index = 0;
    rv = bcm_switch_event_control_set(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT,event,7);
    if (rv != BCM_E_NONE) {
        printf("ERROR: in bcm_switch_event_control_set for set priority  for id 118 rv = %d\n", rv);
        return rv;
    }

    /* storm nominal */

    event.action = bcmSwitchEventStormNominal;
    event.event_id = BCM_SWITCH_EVENT_CONTROL_ALL;
    event.index = 0;
    rv = bcm_switch_event_control_set(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT,event,351);
    if (rv != BCM_E_NONE) {
        printf("ERROR: in bcm_switch_event_control_set for set priority rv = %d\n", rv);
        return rv;
    }
   

    /* storm time count */
    
    event.action = bcmSwitchEventStormTimedCount;
    event.event_id = 98;
    event.index = 0;
    rv = bcm_switch_event_control_set(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT,event,9);
    if (rv != BCM_E_NONE) {
        printf("ERROR: in bcm_switch_event_control_set for set event storm time count for id 98 block instance = %d rv = %d\n", j, rv);
        return rv;
    }
    
    /* storm time period */

    event.action = bcmSwitchEventStormTimedPeriod;
    event.event_id = 252;
    event.index = 0;
    rv = bcm_switch_event_control_set(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT,event,12);
    if (rv != BCM_E_NONE) {
        printf("ERROR: in bcm_switch_event_control_set for set event storm time period for id 252 block instance = %d rv = %d\n", j, rv);
        return rv;
    }
    
    /*sync*/
    rv = bcm_switch_control_set(unit, bcmSwitchControlSync, 1);
    if (rv != BCM_E_NONE) {
        printf("ERROR: in bcm_switch_control_set, failed to sync, rv= %d.\n", rv);
        return rv;
    }

    printf("cint_dfe_cold_boot_set: PASS\n");
    return BCM_E_NONE;
}

int cint_dfe_warm_boot_check(int unit, int fifo_type_rx_prim) {
    int rv, i;
    uint32 value_get;
    bcm_switch_event_control_t event;

    /*groups*/
    int arr_get[5];
    int count;
    rv = bcm_fabric_modid_group_get(unit, group, 5, arr_get, &count);
    if (rv != BCM_E_NONE) {
        printf("ERROR: in bcm_fabric_modid_group_get, failed to get a group, rv = %d.\n", rv);
        return rv;
    }
    if (count != 2) {
        printf("ERROR: in bcm_fabric_modid_group: set != get.\n");
        return BCM_E_FAIL;
    }
    for (i=0; i < count; i++) {
         if (arr_get[i] != arr_set[i]) {
             printf("ERROR: in bcm_fabric_modid_group: set != get.\n");
             return BCM_E_FAIL;
         }
    }

    /*locals*/
    bcm_module_t modid_get;

    rv = bcm_fabric_modid_local_mapping_get(unit, local, &modid_get);
    if (rv != BCM_E_NONE) {
        printf("ERROR: in bcm_fabric_modid_local_mapping_get, failed, rv = %d.\n", rv);
        return rv;
    }
    if (modid_get != modid_set) {
        printf("ERROR: in bcm_fabric_modid_local_mapping: set != get.\n");
        return BCM_E_FAIL;
    }

    /*fifos*/
    int values_get[17];
    rv = bcm_fabric_link_thresholds_get(unit, fifo_type_rx_prim, 8, types, values_get);
    if (rv != BCM_E_NONE) {
          printf("Error, in bcm_fabric_link_thresholds_get for fifo_type_rx_prim, rv=%d, \n", rv);
          return rv;
      }
    for (i=0; i<8; i++) {
        if (values_set[i] != values_get[i]) {
            printf("Error, in bcm_fabric_link_thresholds, get != set \n", rv);
            return BCM_E_FAIL;
        }
    }

    /*multicast*/
    rv = bcm_multicast_create(unit, BCM_MULTICAST_WITH_ID, &mc); 
    if (rv != BCM_E_PARAM) {
        printf("ERROR: in bcm_multicast_create, expecting BCM_E_PARAM while trying to create existing mc group, mc_id = %d, rv= %d.\n", mc, rv);
        return rv;
    }
   
    /*interrupts*/

    /* print */
    event.action = bcmSwitchEventLog;

    /* set print enable */
    event.event_id = 70;
    event.index = 0;
    rv = bcm_switch_event_control_get(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT,event,&value_get);
    if (rv != BCM_E_NONE) {
        printf("ERROR: in bcm_switch_event_get for print event enable rv= %d.\n", rv);
        return rv;
    }

    if(value_get!=0x1) {
        printf("Error, in bcm_switch_event_get for print enable get != set \n", rv);
        return BCM_E_FAIL;
    }
    

    /* set print disable */
    event.event_id = 14;
    event.index = 0;
    rv = bcm_switch_event_control_get(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT,event,&value_get);
    if (rv != BCM_E_NONE) {
        printf("ERROR: in bcm_switch_event_get, for print event disable rv= %d.\n", rv);
        return rv;
    }

    if(value_get!=0) {
        printf("Error, in bcm_switch_event_get for print disable get != set \n", rv);
        return BCM_E_FAIL;
    }

    
    /* override */
    event.action = bcmSwitchEventCorrActOverride;
   

    /* override enable */
    event.event_id = 20;
    event.index = 0;
    event.action = bcmSwitchEventCorrActOverride;
    rv = bcm_switch_event_control_get(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT,event,&value_get);
    if (rv != BCM_E_NONE) {
        printf("ERROR: in bcm_switch_event_get, for event override enable rv= %d.\n", rv);
        return rv;
    }

    if(value_get!=0x1) {
        printf("Error, in bcm_switch_event_get for event override enable != set \n", rv);
        return BCM_E_FAIL;
    }


    /* override disable */
    event.event_id = 22;
    event.index = 0;
    event.action = bcmSwitchEventCorrActOverride;
    rv = bcm_switch_event_control_get(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT,event,&value_get);
    if (rv != BCM_E_NONE) {
        printf("ERROR: in bcm_switch_event_get, for event override disable rv= %d.\n", rv);
        return rv;
    }

    if(value_get!=0) {
        printf("Error, in bcm_switch_event_get for event override disable != set \n", rv);
        return BCM_E_FAIL;
    }


    /* priority */

    event.action = bcmSwitchEventPriority;

    event.event_id = 99;
    event.index = 0;
    rv = bcm_switch_event_control_get(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT,event, &value_get);
    if (rv != BCM_E_NONE) {
        printf("ERROR: in bcm_switch_event_get, for event priority for id 99 rv= %d.\n", rv);
        return rv;
    }
 
    if(value_get!=6) {
        printf("Error, in bcm_switch_event_get for event priority get!= set \n", rv);
        return BCM_E_FAIL;
    }

    event.event_id = 107;
    event.index = 0;
    rv = bcm_switch_event_control_get(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT,event,&value_get);
    if (rv != BCM_E_NONE) {
        printf("ERROR: in bcm_switch_event_get, for event priority for id 107 rv= %d.\n", &value_get);
        return rv;
    }

    if(value_get!=13) {
        printf("Error, in bcm_switch_event_get for event priority get!= set \n", rv);
        return BCM_E_FAIL;
    }
   

    event.event_id = 118;
    event.index = 0;
    rv = bcm_switch_event_control_get(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT,event,&value_get);
    if (rv != BCM_E_NONE) {
        printf("ERROR: in bcm_switch_event_get, for event priority for id 118 rv= %d.\n", rv);
        return rv;
    }

    if(value_get!=7) {
        printf("Error, in bcm_switch_event_get for event priority get!= set \n", rv);
        return BCM_E_FAIL;
    }

    /* storm nominal */

    event.index = 0;
    event.action = bcmSwitchEventStormNominal;
    event.event_id = BCM_SWITCH_EVENT_CONTROL_ALL;
    rv = bcm_switch_event_control_get(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT,event, &value_get);
    if (rv != BCM_E_NONE) {
        printf("ERROR: in bcm_switch_event_get, for storm nominal for id 351 rv= %d.\n", rv);
        return rv;
    }

    if(value_get!=351) {
        printf("Error, in bcm_switch_event_get for storm nominal get!= set \n", rv);
        return BCM_E_FAIL;
    }

    /* storm time count */
    
    event.index = 0;
    event.action = bcmSwitchEventStormTimedCount;
    event.event_id = 98;
    event.index = 0;
    rv = bcm_switch_event_control_get(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event, &value_get);
    if (rv != BCM_E_NONE) {
        printf("ERROR: in bcm_switch_event_get, storm time count for id 98 for index = %d rv= %d.\n", j, rv);
        return BCM_E_FAIL;
    }
    if(value_get!= 9) {
        printf("Error, in bcm_switch_event_get for event storm time count get %u != set \n", value_get);
        return BCM_E_FAIL;
    }
  
    /* storm time period */

    event.action = bcmSwitchEventStormTimedPeriod;
    event.event_id = 252;    
    event.index = 0;
    rv = bcm_switch_event_control_get(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event, &value_get);
    if (rv != BCM_E_NONE) {
        printf("ERROR: in bcm_switch_event_get, for event storm time period for id 252 index = %d rv= %d.\n" , rv);
        return rv;
    }

    if(value_get!=12) {
        printf("Error, in bcm_switch_event_get for event storm time period get %u != set \n", value_get);
        return BCM_E_FAIL;
    }
 
    printf("cint_dfe_warm_boot_check: PASS\n");
    return BCM_E_NONE;

}


