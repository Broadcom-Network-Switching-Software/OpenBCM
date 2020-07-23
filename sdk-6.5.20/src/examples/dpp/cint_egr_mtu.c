/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_egr_mtu.c
 */

/* This funciton is the main function. It configures the HW for egress MTU violation and for IPv4 packet routing. Then, it creates the bcmRxTrapEgMtuFilter
 * trap and uses the bcm_field_qualify_RxTrapCode in order to check the PMF handling of MTU violation. If the trap is caught, then the destinations will be
 * updated per core according to out_port_1 and out_port_2. (out_port_2 will be discarded in single core devices) 
 * if use_pmf == 0 then the configuration of the action profile will be done using the traps mechanism. Otherwise, the configuration will be 
 * done using the PMF*/ 
int egr_mtu_run(int unit, int in_port, int out_port_1, int out_port_2,int use_pmf){
    int rv;
    int mtu_value;
    int gport;
    int num_cores;
    bcm_gport_t destinations[2];
    bcm_qos_map_t map;
    int qos_map_id;
    int flags = 0;
    char *proc_name;

    proc_name = "egr_mtu_run";
    num_cores = SOC_DPP_DEFS_GET_NOF_CORES(unit);


    /* This function will enable the packet to be routed from in_port to in-port */
    rv = basic_example(&unit, 1, in_port,in_port) ;
    if (rv != BCM_E_NONE)
    {
	printf("%s(): Error in basic_example\n",proc_name);
	return rv;
    }

    /* This function set the MTU violation profile to "catch" IPv4 packets */
    rv = mtu_check_header_code_enable(unit, bcmForwardingTypeIp4Ucast);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error in mtu_check_header_code_enable\n");
        return rv;
    }


    bcm_qos_map_t_init(&map);
    map.int_pri = 2;
    map.color = bcmColorYellow;

    /* creating an entry in the cos profile table which will update the TC and DP in ETPP*/
    
    if (is_device_or_above(unit, JERICHO))
    {
        rv = egr_mtu_create_cos_profile(unit ,flags, &map, &qos_map_id);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error in egr_mtu_create_cos_profile\n", proc_name);
            return rv;
        }
    }
    /* the packet is supposed to get routed through in_port and we want to redirect it to out_port per core*/

    destinations[0] = out_port_1;
    destinations[1] = out_port_2;
    printf("%s(): Cos map id is %d\n",proc_name, qos_map_id);
    rv = egr_mtu_trap(unit, destinations, num_cores, in_port,qos_map_id);
    if (rv != BCM_E_NONE)
    {
       printf("%s(): Error in egr_mtu_trap\n",proc_name);
       return rv;
    }
    /* The use for drop*/
    /* rv = egr_mtu_trap(unit,BCM_GPORT_BLACK_HOLE, in_port) ; */

    /* Testing the rx_trap_code_qualifier */
    if (use_pmf)
    {
        rv = egr_mtu_init_group(unit);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error in egr_mtu_init_group\n", proc_name);
            return rv;
        }
        rv = egr_mtu_add_filter(unit,0, qos_map_id);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error in egr_mtu_add_filter\n", proc_name);
            return rv;
        } 
    }

    /* ***  The use for deletion the cos profile entry *** */
    /*
    egr_mtu_delete_cos_profile(unit, flags, qos_map_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error in egr_mtu_delete_cos_profile\n", proc_name);
        return rv;
    }
    */
    printf("%s(): EXIT. rv %d\n",proc_name, rv);
    return rv;
}

int egr_mtu_trap(int unit,bcm_gport_t *destinations,int destinations_len, int out_port, int qos_map_id)
{
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t config;
    bcm_rx_trap_core_config_t arr_core[2];
    bcm_vlan_port_t vlan_port[2];
    int flags = 0;
    int trap_id;
    int mtu_value;
    int mtu_limit = 200;
    int i;
    char *proc_name;

    proc_name = "egr_mtu_trap";
    printf("%s(): Enter. destinations_len %d, destinations[0] 0x%08X\r\n", proc_name, destinations_len, destinations[0]);
    /* setting the mtu value on the out_port*/
    rv = bcm_port_l3_mtu_set(unit, out_port, mtu_limit);
    if (rv != BCM_E_NONE) {
	printf("Error, bcm_port_l3_mtu_set : 15\n");
	return rv;
    }

    rv = bcm_port_l3_mtu_get(unit, out_port, &mtu_value);     
    printf(" out_port %d  mtu = %d\n",out_port, mtu_value);


    for (i = 0; i< destinations_len; ++i)
    {
      /* setting the Out-lif (just to get an encap id - needs more configuration process in order to change the Vlan id for example) */      
        vlan_port[i].criteria = BCM_VLAN_PORT_MATCH_PORT;
        vlan_port[i].flags = 0;
        vlan_port[i].port = destinations[i] ;
        vlan_port[i].egress_vlan = 500;

        rv = bcm_vlan_port_create(unit, &(vlan_port[i]));
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in bcm_vlan_port_create\n", proc_name);
            print rv;
        }
        else{
            printf("%s(): Created vlan port %d\n", proc_name, vlan_port[i].vlan_port_id);
        }
    }
  
    /*Defining the trap */

 
    if (destinations_len > 1)
    {
        sal_memset(&config, 0, sizeof(config));
        config.flags |= BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_UPDATE_PRIO |BCM_RX_TRAP_UPDATE_ENCAP_ID | BCM_RX_TRAP_UPDATE_QOS_MAP_ID;
        config.trap_strength = 5;
        config.prio=7;
        config.qos_map_id = qos_map_id;

        for (i = 0; i< destinations_len; ++i)
        {
            arr_core[i].dest_port = destinations[i];
            arr_core[i].encap_id = vlan_port[i].encap_id;
        }

        config.core_config_arr = arr_core;
        config.core_config_arr_len = destinations_len;
    }
    else
    {
        sal_memset(&config, 0, sizeof(config));
        config.flags |= BCM_RX_TRAP_UPDATE_DEST |BCM_RX_TRAP_UPDATE_PRIO | BCM_RX_TRAP_UPDATE_ENCAP_ID;
        config.encap_id = vlan_port.encap_id;
        config.trap_strength = 5;
        config.dest_port = destinations[0];  
        config.prio=7;

        /* other options */
        /*
          bcm_gport_t local_gport;
          BCM_GPORT_LOCAL_SET(local_gport, 0);
          config.dest_port = local_gport; 
          config.dest_port= BCM_GPORT_BLACK_HOLE;  
        */ 
        }

    rv = bcm_rx_trap_type_create(unit, flags, bcmRxTrapEgMtuFilter, &trap_id);

    printf("%s(): mtu trap id is %d \n", proc_name, trap_id);
    if (rv != BCM_E_NONE) {
	printf("%s(): Error, in trap create, trap id %d;  \n", proc_name, trap_id);
	return rv;
    }

    printf("%s(): Going to call bcm_rx_trap_set(). config: dest_port 0x%08X core_config_arr_len %d\r\n", \
        proc_name, config.dest_port, config.core_config_arr_len);
    if (config.core_config_arr_len > 1)
    {
        printf("==> config: core_config_arr[0].dest_port 0x%08X core_config_arr[1].dest_port 0x%08X\r\n", \
            config.core_config_arr[0].dest_port, config.core_config_arr[1].dest_port);
    }
    else if (config.core_config_arr_len == 1)
    {
        printf("==> config: core_config_arr[0].dest_port 0x%08X\r\n", \
            config.core_config_arr[0].dest_port);
    }
    rv = bcm_rx_trap_set(unit,trap_id,&config);
    if (rv != BCM_E_NONE) {
	printf("%s(): Error, in trap set \n", proc_name);
	return rv;
    }
    
    printf("%s(): EXIT. rv %d\r\n", proc_name, rv);
    return rv;
}


int egr_mtu_init_group(int unit)
{
    int rc;
    int priority_group = 3;
    bcm_field_qset_t qset;
    bcm_field_group_t grp;
    bcm_field_aset_t aset;

    /* Define the QSET */
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpType);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyRxTrapCode);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcPort);
    /*  Create the Field group */
    rc = bcm_field_group_create(unit, qset, priority_group, &grp);
    if (BCM_E_NONE != rc) {
        printf("Error in bcm_field_group_create\n");
        bcm_field_group_destroy(unit, grp);
        return -1;
    }

      printf("grp is %d",grp);
    BCM_FIELD_ASET_INIT(aset);

    /*  Attach the action set */
    rc = bcm_field_group_action_set(unit, grp, aset);
    if (BCM_E_NONE != rc) {
        printf("Error in bcm_field_group_action_set\n");
        bcm_field_group_destroy(unit, grp);
        return -1;
    }

    rc = bcm_field_group_install(unit, grp);
    if (BCM_E_NONE != rc) {
        printf("Error in bcm_field_group_install\n");
        bcm_field_group_destroy(unit, grp);
        return -1;
    }        /* Define the QSET */

    printf("Init MTU Group successful, group:%d\n", grp);
    return 0;
}

int egr_mtu_add_filter(int unit, int grp, int qos_map_id)
{
    int rc;
    bcm_field_entry_t entry;
    bcm_rx_trap_t data;
    int trap_id;
    
    rc = bcm_field_entry_create(unit, grp, &entry);
    if (BCM_E_NONE != rc) {
        printf("Error in bcm_field_entry_create\n");
        return -1;
    }



    /* VSI */
    rc = bcm_field_qualify_RxTrapCode(unit, entry, bcmRxTrapEgMtuFilter);
    if (BCM_E_NONE != rc) {
       printf("Error in bcm_field_qualify_RxTrapCode\n");
      return rc;
    }

    /* checking that RxtrapCode get works */
    rc =  bcm_field_qualify_RxTrapCode_get( unit,  entry, &data);
    if (data != bcmRxTrapEgMtuFilter) {
       printf("Error in bcm_field_qualify_RxTrapCode_get\n");
      return rc;
    } 
   
    /* The use for drop*/
    /* 
    rc = bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0);
    if (BCM_E_NONE != rc) {
        printf("Error in bcm_field_action_add\n");
        return rc;
    } 
    */ 
    
    
    rc = bcm_field_action_add(unit,entry, bcmFieldActionQosMapIdNew, qos_map_id, 0);
    if (BCM_E_NONE != rc) 
    {
        printf("Error in cm_field_action_add\n");
        return rc;
    } 

    rc = bcm_field_entry_install(unit, entry);
    if (BCM_E_NONE != rc)
    {
        printf("Error in bcm_field_entry_install\n");
        return rc;
    }

    printf("Add MTU Filter successful");
    return 0;
}

/* creates an entry in the cos profile table which will update the TC and DP in ETPP*/
int egr_mtu_create_cos_profile(/*in*/ int unit, int flags, bcm_qos_map_t *map,/*out*/ int *qos_map_id)
{
    int rc = BCM_E_NONE;
    int previous_int_pri;
    int previous_color;
    int i;

    rc = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS| BCM_QOS_MAP_ENCAP_INTPRI_COLOR, qos_map_id);
    if (BCM_E_NONE != rc)
    {
        printf("Error in bcm_qos_map_create\n");
        return rc;
    }
    /* just for checking */
    rc = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS| BCM_QOS_MAP_ENCAP_INTPRI_COLOR, qos_map_id);
    if (BCM_E_NONE != rc)
    {
        printf("Error in bcm_qos_map_create\n");
        return rc;
    }

    rc = bcm_qos_map_add(unit, flags, map, *qos_map_id);
    if (BCM_E_NONE != rc) 
    {
        printf("Error in bcm_qos_map_add\n");
        return rc;
    }
    

    /* ** Test of the get function ** */

    /* saving the values in case the get funciton fails*/
    previous_int_pri = map->int_pri; 
    previous_int_pri = map->color; 
    map->int_pri = 0;
    map->color = 0;

    bcm_qos_map_multi_get(unit, flags, *qos_map_id, 1, map, &i); 
    if (BCM_E_NONE != rc || map->int_pri!=previous_int_pri ||  map->color!=previous_color) 
    {
        printf("Error in bcm_qos_map_add\n");
        /* restoring the values*/
        map->int_pri = previous_int_pri;
        map->color = previous_int_pri;
        return rc;
    }

    printf("Created an entry with id: %d\n", *qos_map_id);
    return rc;


}

/* Deletes and destroys the cos profile entry*/
int egr_mtu_delete_cos_profile(/*in*/ int unit, int flags, int cos_id)
{
    /* dummy map struct for the function */
    bcm_qos_map_t map;
    int rc = BCM_E_NONE;
    bcm_qos_map_t_init(&map);

    bcm_qos_map_delete( unit, flags, &map, cos_id); 
    if (BCM_E_NONE != rc)
    {
        printf("Error in bcm_qos_map_delete\n");
        return rc;
    }
    bcm_qos_map_destroy(unit, cos_id);
    if (BCM_E_NONE != rc)
    {
        printf("Error in bcm_qos_map_destroy\n");
        return rc;
    }

    return rc;

}
