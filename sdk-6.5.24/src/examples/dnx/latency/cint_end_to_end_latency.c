/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

 /* Explanation:
 *  This cint includes functions that are required to activate end to end latency measurements.
 *
 *
 * Calling Sequence - end to end latency per port:
 *  1. Set end to end latency per port. global configuration.
 *  2. create egress latency profile.
 *  2. set the properties of the profiles.
 *  3. map port to the latency profile.
 *
 * Calling Sequence - end to end latency per pp flow:
 *  1. create egress latency profile.
 *  2. set the properties of the profile.
 *  3. map pp flow by PMF to the latency profile.
 */

/** globals for acl rule, which hlod the field group id and entry id. */
/** use it when you want to destroy the group and/or delete the entry */
bcm_field_group_t e2e_lat_fg_id;
bcm_field_entry_t e2e_lat_ent_id;
/** latency profile gport */
bcm_gport_t latency_gport;

int
end_to_end_latency_flow_acl_rule_set(
    int unit,
    bcm_field_context_t pmf_context,
    int flow_id,
    int flow_profile,
    bcm_field_qualify_t qual_type,
    int qual_mask,
    int qual_data)
{
    int rv;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    bcm_gport_t gport;
    void *dest_char;
            
    /** Create and attach first group */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /** Set quals */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = qual_type;
    /** Set actions */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionLatencyFlowId;
    rv = bcm_field_group_add(unit, 0, &fg_info, &e2e_lat_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }        

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_types[1] = fg_info.action_types[1];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

    rv = bcm_field_group_context_attach(unit, 0, e2e_lat_fg_id, pmf_context, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    
    /* Add entry */
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;

    ent_info.nof_entry_quals = 1;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = qual_data;
    ent_info.entry_qual[0].mask[0] = qual_mask;

    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    /**     { latency_flow_profile(4), latency_flow_id(19), latency_flow_id_valid(1) } */
    ent_info.entry_action[0].value[0] = 1;
    ent_info.entry_action[0].value[1] = flow_id;
    ent_info.entry_action[0].value[2] = flow_profile;

    rv = bcm_field_entry_add(unit, 0, e2e_lat_fg_id, &ent_info, &e2e_lat_ent_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }
    printf("Entry add: id:(0x%x) ingress latency: flow decode (=%d)\n", e2e_lat_fg_id ,ent_info.entry_action[0].value[0]);

    return rv;
}

/**
* \brief - function allocate 1 end to end latency profile and map a flow (by PMF) to the profile.
*   In addition, it generate a command toward the counter processor.
*/
int end_to_end_latency_per_flow_example(
    int unit,
    int flow_id,
    int in_port,
    bcm_field_context_t pmf_context)
{
    int rv = BCM_E_NONE;
    int bin_idx, profile_idx;
    bcm_gport_t gport_qual;
    bcm_stat_pp_profile_info_t stat_pp_profile_info;
    int stat_pp_profile;
    int feature_end_to_end_aqm_support = *dnxc_data_get(unit, "latency", "features", "valid_end_to_end_aqm_profile", NULL);
    int feature_valid_flow_profile_flag = *dnxc_data_get(unit, "latency", "features", "valid_flow_profile_flag", NULL);
    int feature_egress_latency_track_support = *dnxc_data_get(unit, "latency", "features", "egress_latency_track_support", NULL);
    uint32 flags = 0;

    flags = feature_valid_flow_profile_flag ? (BCM_COSQ_LATENCY_PROFILE_BY_FLOW | BCM_COSQ_LATENCY_END_TO_END) : BCM_COSQ_LATENCY_END_TO_END;
    /** create a latency profile */
    rv = bcm_cosq_latency_profile_create(unit, flags, &latency_gport);
    if (rv != BCM_E_NONE)
    {   
        printf("bcm_cosq_latency_profile_create failed \n");
        return rv;    
    }
    /** configure device to work in mode: end to end latency per flow */
    rv = bcm_switch_control_set(unit, bcmSwitchEndToEndLatencyPerDestinationPort, FALSE);
    if (rv != BCM_E_NONE)
    {   
        printf("bcm_switch_control_set failed \n");
        return rv;    
    }

    /** generate latency profile command */
    bcm_stat_pp_profile_info_t_init(&stat_pp_profile_info);
    stat_pp_profile_info.counter_command_id = 5; /** must be 5 for egress latency measurments */    
    rv = bcm_stat_pp_profile_create(unit, 0, bcmStatCounterInterfaceEgressTransmitPp, &stat_pp_profile, &stat_pp_profile_info);
    if (rv != BCM_E_NONE)
    {   
        printf("bcm_stat_pp_profile_create failed \n");
        return rv;    
    }       
    /** set the properties of the profile. by default all actions related to latency are disabled. */
    /** Example activate some of them */
    rv = bcm_cosq_control_set(unit, latency_gport, 0, bcmCosqControlLatencyEgressCounterCommand, stat_pp_profile);
    if (rv != BCM_E_NONE)
    {   
        printf("bcm_cosq_latency_profile_create failed \n");
        return rv;    
    }
    if (!feature_end_to_end_aqm_support)
    {
        if (feature_egress_latency_track_support)
        {
            rv = bcm_cosq_control_set(unit, latency_gport, 0, bcmCosqControlLatencyTrack, TRUE);
            if (rv != BCM_E_NONE)
            {
                printf("bcm_cosq_latency_profile_create failed \n");
                return rv;
            }
        }
        rv = bcm_cosq_control_set(unit,latency_gport,0,bcmCosqControlLatencyDropEnable, TRUE);
        if (rv != BCM_E_NONE)
        {   
            printf("bcm_cosq_latency_profile_create failed \n");
            return rv;    
        }  
        rv = bcm_cosq_control_set(unit,latency_gport,0,bcmCosqControlLatencyEcnEnable, FALSE);
        if (rv != BCM_E_NONE)
        {   
            printf("bcm_cosq_latency_profile_create failed \n");
            return rv;    
        } 
        /** set 10000 nano-second drop threshold */
        rv = bcm_cosq_control_set(unit,latency_gport, 0, bcmCosqControlLatencyDropThreshold, 10000);
        if (rv != BCM_E_NONE)
        {   
            printf("bcm_cosq_latency_profile_create failed \n");
            return rv;    
        } 
    }
     
    rv = bcm_cosq_control_set(unit , latency_gport, 0, bcmCosqControlLatencyCounterEnable, TRUE);
    if (rv != BCM_E_NONE)
    {   
        printf("bcm_cosq_latency_profile_create failed \n");
        return rv;    
    }    

    /** set bin threshold in jump of 1000 nano-second each, just for the example */
    for(bin_idx = 0; bin_idx < 7; bin_idx++)
    {
        rv = bcm_cosq_control_set(unit,latency_gport, bin_idx, bcmCosqControlLatencyBinThreshold, (1000 + bin_idx*1000));
        if (rv != BCM_E_NONE)
        {   
            printf("bcm_cosq_latency_profile_create failed \n");
            return rv;    
        }            
    }
    printf (" end to end latency profile %d was created \n", latency_gport);

    /** map a flow to the profile */
    BCM_GPORT_LOCAL_SET(gport_qual, in_port);
    rv = end_to_end_latency_flow_acl_rule_set(unit, pmf_context, flow_id, BCM_GPORT_END_TO_END_LATENCY_PROFILE_GET(latency_gport),
        bcmFieldQualifyInPort, 0x1FF, gport_qual);
    if (rv != BCM_E_NONE)
    {   
        printf("end_to_end_latency_flow_acl_rule_set failed \n");
        return rv;    
    }      
    return rv;
}

/**
* \brief - function allocate 1 end to end latency profile and map a destination port to the profile.
*   In addition, it generate a command toward the counter processor.
*/
int end_to_end_latency_per_port_example(
    int unit,
    int out_port)
{
    int rv = BCM_E_NONE;
    int bin_idx, profile_idx;
    bcm_switch_profile_mapping_t profile_mapping;
    bcm_stat_pp_profile_info_t stat_pp_profile_info;
    int stat_pp_profile;
    int feature_end_to_end_aqm_support = *dnxc_data_get(unit, "latency", "features", "valid_end_to_end_aqm_profile", NULL);
    int feature_egress_latency_track_support = *dnxc_data_get(unit, "latency", "features", "egress_latency_track_support", NULL);
    /** create a latency profile */
    rv = bcm_cosq_latency_profile_create(unit, BCM_COSQ_LATENCY_END_TO_END, &latency_gport);
    if (rv != BCM_E_NONE)
    {   
        printf("bcm_cosq_latency_profile_create failed \n");
        return rv;    
    }

    /** configure device to work in mode: end to end latency per port - Not mandatory, since this is the default */
    rv = bcm_switch_control_set(unit, bcmSwitchEndToEndLatencyPerDestinationPort, TRUE);
    if (rv != BCM_E_NONE)
    {   
        printf("bcm_switch_control_set failed \n");
        return rv;    
    }
    
    /** generate latency profile command */
    bcm_stat_pp_profile_info_t_init(&stat_pp_profile_info);
    stat_pp_profile_info.counter_command_id = 5; /** must be 5 for egress latency measurments */    
    rv = bcm_stat_pp_profile_create(unit, 0, bcmStatCounterInterfaceEgressTransmitPp, &stat_pp_profile, &stat_pp_profile_info);
    if (rv != BCM_E_NONE)
    {   
        printf("bcm_stat_pp_profile_create failed \n");
        return rv;    
    }       
    /** set the properties of the profile. by default all actions related to latency are disabled. */
    /** Example activate some of them */
    rv = bcm_cosq_control_set(unit, latency_gport, 0, bcmCosqControlLatencyEgressCounterCommand, stat_pp_profile);
    if (rv != BCM_E_NONE)
    {   
        printf("bcm_cosq_latency_profile_create failed \n");
        return rv;    
    }
    rv = bcm_cosq_control_set(unit , latency_gport, 0, bcmCosqControlLatencyCounterEnable, TRUE);
    if (rv != BCM_E_NONE)
    {   
        printf("bcm_cosq_latency_profile_create failed \n");
        return rv;    
    }    
     if (!feature_end_to_end_aqm_support)
    {   
        if (feature_egress_latency_track_support)
        {
            rv = bcm_cosq_control_set(unit, latency_gport, 0, bcmCosqControlLatencyTrack, TRUE);
            if (rv != BCM_E_NONE)
            {
                printf("bcm_cosq_latency_profile_create failed \n");
                return rv;
            }
        }
        rv = bcm_cosq_control_set(unit,latency_gport,0,bcmCosqControlLatencyDropEnable, TRUE);
        if (rv != BCM_E_NONE)
        {   
            printf("bcm_cosq_latency_profile_create failed \n");
            return rv;    
        }  
        rv = bcm_cosq_control_set(unit,latency_gport,0,bcmCosqControlLatencyEcnEnable, FALSE);
        if (rv != BCM_E_NONE)
        {   
            printf("bcm_cosq_latency_profile_create failed \n");
            return rv;    
        } 
        /** set 10000 nano-second drop threshold */
        rv = bcm_cosq_control_set(unit,latency_gport, 0, bcmCosqControlLatencyDropThreshold, 10000);
        if (rv != BCM_E_NONE)
        {   
            printf("bcm_cosq_latency_profile_create failed \n");
            return rv;    
        } 
     }
    /** set bin threshold in jump of 1000 nano-second each, just for the example */
    for(bin_idx = 0; bin_idx < 7; bin_idx++)
    {
        rv = bcm_cosq_control_set(unit,latency_gport, bin_idx, bcmCosqControlLatencyBinThreshold, (1000 + bin_idx*1000));
        if (rv != BCM_E_NONE)
        {   
            printf("bcm_cosq_latency_profile_create failed \n");
            return rv;    
        }            
    }
    printf (" end to end latency profile %d was created \n", latency_gport);

    /** map an ouput port to the profile */
    bcm_switch_profile_mapping_t_init(&profile_mapping);
    profile_mapping.profile_type = bcmCosqPortToEndToEndLatencyProfile;
    profile_mapping.mapped_profile = latency_gport;
    rv = bcm_cosq_profile_mapping_set(unit, out_port, 0, 0, &profile_mapping);
    if (rv != BCM_E_NONE)
    {   
        printf("bcm_cosq_profile_mapping_set failed \n");
        return rv;    
    }
    return rv;
}

/** hold the max latency value */
uint32 e2e_max_latency;

/**
* \brief - function get from HW the max 8 latencies per core
*/
int end_to_end_latency_max_table_get(
    int unit)
{
    int rv = BCM_E_NONE;
    int actual_count, i;
    int core_idx;
    bcm_gport_t gport;
    uint32 nof_cores = *(dnxc_data_get(unit, "device", "general", "nof_cores", NULL));
    bcm_cosq_max_latency_pkts_t max_latency_pkts[8];
    int memory_actual_count;

    /**
     * Reset the variable
     */
    e2e_max_latency=0;

    for (core_idx; core_idx < nof_cores; core_idx++)
    {
        BCM_COSQ_GPORT_CORE_SET(gport, core_idx);
        rv = bcm_cosq_max_latency_pkts_get(unit, gport, BCM_COSQ_LATENCY_END_TO_END, 8, max_latency_pkts, &memory_actual_count);
        if (BCM_E_NONE != rv) {
              printf("bcm_cosq_icgm_max_latency_pkts_get failed\n");
              return rv;
        }
        printf("------- MAX LATENCY TABLE PRINT: (size=%d, core=%d) --------\n", memory_actual_count, core_idx);  
        for(i=0;i<memory_actual_count; i++)
        {
            printf("max_latency_pkt[%d]:\n",i);
            print("\tlatency=\n");
            print("\t%x\n",max_latency_pkts[i].latency);
            printf("\tlatency_flow=0x%x\n",max_latency_pkts[i].latency_flow);
            printf("\tdest_gport=0x%x\n",max_latency_pkts[i].dest_gport);
            printf("\tcosq (tc)=0x%x\n",max_latency_pkts[i].cosq);
            /** save the max latency in global variable */
            if(COMPILER_64_HI(max_latency_pkts[i].latency) !=  0)
            {
                printf("invalid latency value (>32 bits)\n");
                return BCM_E_FAIL; 
            }
            if(COMPILER_64_LO(max_latency_pkts[i].latency) >  e2e_max_latency)
            {
                e2e_max_latency = COMPILER_64_LO(max_latency_pkts[i].latency);
            }               
        }               
    }
    
    return rv;
}

/** hold the max latency value for specific flow */
int e2e_flow_latency_max;
/** hold the max latency table actual entries count */
int e2e_flow_latency_max_actual_count;
/** hold the max latency table contents */
bcm_cosq_max_latency_pkts_t max_latency_pkts[8];
/**
* \brief - function get from HW the max latency of specific flow or per core
*/
int end_to_end_latency_flow_max_get(
    int unit,
    int core,
    int flow_id,
    int is_per_flow)
{
    int rv = BCM_E_NONE;
    int actual_count, i;
    bcm_gport_t gport;
    uint32 nof_cores = *(dnxc_data_get(unit, "device", "general", "nof_cores", NULL));

    e2e_flow_latency_max_actual_count=0;
    e2e_flow_latency_max = 0;

    BCM_COSQ_GPORT_CORE_SET(gport, core);
    rv = bcm_cosq_max_latency_pkts_get(unit, gport, BCM_COSQ_LATENCY_END_TO_END, 8, max_latency_pkts, &e2e_flow_latency_max_actual_count);
    if (BCM_E_NONE != rv) {
          printf("bcm_cosq_icgm_max_latency_pkts_get failed\n");
          return rv;
    }
    printf("------- MAX LATENCY TABLE ENTRY PRINT: (size=%d, core=%d, flow_id=%d) --------\n", 
            e2e_flow_latency_max_actual_count, core, flow_id);
    for(i=0;i<e2e_flow_latency_max_actual_count; i++)
    {
        printf("max_latency_pkt[%d]:\n",i);
        print("\tlatency=\n");
        print("\t%x\n",max_latency_pkts[i].latency);
        printf("\tlatency_flow=0x%x\n",max_latency_pkts[i].latency_flow);
        printf("\tdest_gport=0x%x\n",max_latency_pkts[i].dest_gport);
        printf("\tcosq (tc)=0x%x\n",max_latency_pkts[i].cosq);
        if (is_per_flow == TRUE)
        {
            /** Check if the current flow mathches the requested */
            if(max_latency_pkts[i].latency_flow == flow_id)
            {
                /** Check for overflow of variable */
                if(COMPILER_64_HI(max_latency_pkts[i].latency) !=  0)
                {
                    printf("invalid latency value (>32 bits)\n");
                    return BCM_E_FAIL; 
                }

                /** Check if current latecny value is bigger than the previous */
                if(COMPILER_64_LO(max_latency_pkts[i].latency) >  e2e_flow_latency_max)
                {
                    COMPILER_64_TO_32_LO(e2e_flow_latency_max, max_latency_pkts[i].latency);
                }
            }
        }
    }
    
    return rv;
}

/**
* \brief - create counter processor database in order to hold the max end to end latency value per flow/port.
*/
int end_to_end_latency_counter_database_set(
    int unit, 
    int engineId,
    int max_flow_id,
    int count_packets_per_bin)
{
    int rv = BCM_E_NONE;
    int database_id;
    uint32 flags;
    bcm_stat_counter_database_t database;
    bcm_stat_engine_t engine;
    bcm_stat_counter_enable_t enable_config;
    bcm_stat_counter_interface_t cnt_interface;
    bcm_stat_counter_set_map_t ctr_set_map;
    bcm_stat_eviction_t eviction;
    bcm_stat_counter_interface_key_t interface_key;
    bcm_stat_expansion_select_t expansion_select;
    
    bcm_stat_counter_database_t_init(&database);
    bcm_stat_engine_t_init(&engine);
    bcm_stat_eviction_t_init(&eviction);
    bcm_stat_counter_set_map_t_init(&ctr_set_map);
    bcm_stat_counter_interface_key_t_init(&interface_key);
    bcm_stat_expansion_select_t_init(&expansion_select);
    bcm_stat_counter_interface_t_init(&cnt_interface);
    bcm_stat_counter_enable_t_init(&enable_config);

    /** expansion per bins */
    interface_key.command_id = 5;
    interface_key.core_id = BCM_CORE_ALL;
    interface_key.interface_source = bcmStatCounterInterfaceEgressTransmitPp;
    interface_key.type_id = 0;
    /* counts how many packets per bin - need to do expansion per bin */
    if(count_packets_per_bin == TRUE)
    {
        expansion_select.nof_elements = 1;
        /** possible value is only bcmStatExpansionTypeLatencyBin */
        expansion_select.expansion_elements[0].type = bcmStatExpansionTypeMetaData; 
        expansion_select.expansion_elements[0].bitmap = 0x7; /** expansion based on 3 bits latency bin */    
    }
    else
    {
        expansion_select.nof_elements = 0;
    }

    rv = bcm_stat_counter_expansion_select_set(unit, 0, &interface_key, &expansion_select);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_expansion_select_set\n");
        return rv;
    }
    /** configure and create database */
    database.database_id = engineId;
    database.core_id = BCM_CORE_ALL;
    rv = bcm_stat_counter_database_create(unit, BCM_STAT_DATABASE_CREATE_WITH_ID, BCM_CORE_ALL, &database.database_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_database_create\n");
        return rv;
    }
    
    /** attach the engines to the database */
    rv = bcm_stat_counter_engine_attach(unit, 0, &database, engineId);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_engine_attach\n");
        return rv;
    }
   /** configure the counter eviction per engine */
    engine.engine_id = engineId;
    engine.core_id = BCM_CORE_ALL;
    eviction.dma_fifo_select = 0;
    eviction.record_format = bcmStatEvictionRecordFormatPhysical;
    eviction.eviction_event_id = database_id;
    eviction.type = bcmStatEvictionDestinationLocalHost;
    rv = bcm_stat_counter_eviction_set(unit, 0, &engine, &eviction);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_eviction_set\n");
        return rv;
    }    

     /** configure the counter interface per database */
    cnt_interface.source = bcmStatCounterInterfaceEgressTransmitPp;
    cnt_interface.command_id = 5; /** for latency measurements, command_id must be 5 */
    /** if count_packets_per_bin=1, the format is count packets (dual counter entry mode or single entry) */
    if(count_packets_per_bin == TRUE)
    {
        cnt_interface.format_type = bcmStatCounterFormatDoublePackets;
        cnt_interface.counter_set_size = 8;
        flags = 0;
    }
    /** hold the max value per flow-id, counter_set=1 */
    else
    {
        cnt_interface.format_type = bcmStatCounterFormatMaxSize;
        cnt_interface.counter_set_size = 1;
        flags = BCM_STAT_COUNTER_END_TO_END_LATENCY;
    }
    cnt_interface.type_config[0].valid = 1;
    cnt_interface.type_config[0].object_type_offset = 0;
    cnt_interface.type_config[0].start = 0;
    cnt_interface.type_config[0].end = max_flow_id;
    rv = bcm_stat_counter_interface_set(unit, flags, &database, &cnt_interface);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_interface_set\n");
        return rv;
    }

    /**  configure the counter set mapping per database */
    rv = bcm_stat_counter_set_mapping_set(unit, BCM_STAT_COUNTER_MAPPING_FULL_SPREAD, &database, &ctr_set_map);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_set_mapping_set\n");
        return rv;
    }

    /**  enable the engine */
    enable_config.enable_counting = TRUE;
    enable_config.enable_eviction = TRUE;
    rv = bcm_stat_counter_database_enable_set(unit, 0, &database, &enable_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_database_enable_set\n");
        return rv;
    }
    return rv;
}
