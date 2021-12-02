/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

 /* Explanation:
 *  This cint includes functions that are required to activate ingress latency measurements between the IPP and ITP.
 *
 * Calling Sequence - ingress latency per queue:
 *  1. create 4 consecutive ingress latency profiles.
 *  2. set the properties of the profiles.
 *  3. map base 4 queues to the latency profile
 *
 * Calling Sequence - ingress latency per pp flow:
 *  1. create ingress latency profile.
 *  2. set the properties of the profile.
 *  3. map pp flow by PMF to the latency profile.
 *
 */

/** holds if system header mode is legacy (jericho-1) */
int jericho1_system_header_mode;
/** holds the latency profile gport (for latency per queue, it holds the base profile)*/
bcm_gport_t latency_gport;

/**
* \brief - function allocate 4 consecutive ingress latency profile and map a base queue quartet to the base profile
*/
int ingress_latency_per_queue_example(
    int unit,
    bcm_gport_t gport_q,
    int in_port,
    bcm_field_context_t pmf_context)
{
    int bin_idx, quartet = 4, profile_idx;
    bcm_gport_t gport, phy_port;
    int base_profile = 16;
    bcm_switch_profile_mapping_t profile_mapping;
    int num_cos_levels;
    uint32 flags;
    bcm_gport_t gport_qual;
    int nof_profiles = 1;

    /** check if the qid is base of 4 of base of 8 */
    BCM_IF_ERROR_RETURN(bcm_cosq_gport_get(unit, gport_q, &phy_port, &num_cos_levels, &flags));

    /** create 4 consecutive profiles */
    /** profile id % 4 must be zero. therefore, we are using WITH_ID flag. As example we use base_profile=16 */
    /** For queue mapping, we are using the extension range, because only queues can use the extansion range. */
    /** This give better usage of the profiles range (but it is not madatory) */
    for(profile_idx = 0; profile_idx < quartet; profile_idx++)
    {
        BCM_GPORT_INGRESS_LATENCY_PROFILE_SET(gport, (base_profile + profile_idx));
        BCM_IF_ERROR_RETURN(bcm_cosq_latency_profile_create(unit, (BCM_COSQ_LATENCY_PROFILE_ID_EXTENSION | BCM_COSQ_LATENCY_PROFILE_WITH_ID), 
            &gport));
        /** set the properties of the profile. by default all actions related to latency are disabled. */
        /** Example activate some of them */
        BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit, gport, 0, bcmCosqControlLatencyTrack, TRUE));
        BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit, gport, 0, bcmCosqControlLatencyCounterEnable, TRUE));
        BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit,gport,0,bcmCosqControlLatencyTrackDropPacket, TRUE));
         /** set bin threshold in jump of 1000 nano-second each, just for the example */
        for(bin_idx = 0; bin_idx < 7; bin_idx++)
        {
            BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit, gport, bin_idx, bcmCosqControlLatencyBinThreshold, (1000 + bin_idx*1000)));
        }
        
        BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit,gport,0,bcmCosqControlLatencyDropEnable, TRUE));
        BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit,gport,0,bcmCosqControlLatencyEcnEnable, FALSE));
        /** set 10000 nano-second drop threshold */
        BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit,gport, 0, bcmCosqControlLatencyDropThreshold, 100000));
             
       printf (" ingress latency profile %d was created \n", gport);
        if (profile_idx == 0)
        {
            latency_gport = gport;
        }
        
    }

    /** map only the base quartet queue to base quartet profile */
    profile_mapping.profile_type = bcmCosqIngressQueuesToLatencyProfile;
    profile_mapping.mapped_profile = latency_gport;
    BCM_IF_ERROR_RETURN(bcm_cosq_profile_mapping_set(unit, gport_q, 0, 0, &profile_mapping));
    /** if 8 queues bundle, map also the second quartet of the bundle */
    if(num_cos_levels == 8)
    {
        BCM_IF_ERROR_RETURN(bcm_cosq_profile_mapping_set(unit, gport_q, 4, 0, &profile_mapping));        
    }

    /** in order to have latency measurements need to add flow id
     *  extension present - this can be made with ACL flow id
     *  rule - creating flow id on the default profile 0 only to
     *  get the extension, even  if later this profile is used
     *  there is no interference - if it is used for flow the
     *  flag for flow will set the valid bit/ if it is used for
     *  VOQ, extension will be present - no issue with the flow
     *  as the valid bit will not be set */
    BCM_GPORT_LOCAL_SET(gport_qual, in_port);
    profile_idx = 0;
    BCM_IF_ERROR_RETURN(ingress_latency_flow_acl_rule_set(unit, pmf_context, &profile_idx, &profile_idx, bcmFieldQualifyInPort, 0x1FF, &gport_qual, nof_profiles));

    return BCM_E_NONE;
}

/**
* \brief - 4 consecutive ingress latency profiles are already 
*        configured - configure the probabilistic drop
*        mechanism */
int ingress_latency_per_queue_configure_probabilistic_mechanism_drop(
    int unit,
    bcm_gport_t gport_q)
{
    int rv = BCM_E_NONE;
    int bin_idx, quartet = 4, profile_idx;
    bcm_gport_t gport;
    int base_profile = 16;
    int feature_probibalistic_mechanism_support = *dnxc_data_get(unit, "latency", "features", "drop_probabilistic_mechanism_support", NULL);

    /** first verify the probabilistic feature is supported */
    if (!feature_probibalistic_mechanism_support)
    {
        printf("Error in ingress_latency_per_queue_configure_probabilistic_mechanism_drop configuration - probabilistic mechanism is not supported\n", rv);
        return BCM_E_FAIL;
    }
    for(profile_idx = 0; profile_idx < quartet; profile_idx++)
    {
        BCM_GPORT_INGRESS_LATENCY_PROFILE_SET(gport, (base_profile + profile_idx));
         /** set bin threshold in jump of 1000 nano-second each, just for the example */
        for(bin_idx = 0; bin_idx < 7; bin_idx++)
        {
            BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit, gport, bin_idx, bcmCosqControlLatencyBinThreshold, (1000 + bin_idx*1000)));
        }
        /** configure probibalistic mechanism for drop (analogical
         *  configuration is required if probabilistic mechanism is
         *  needed for mark cni */
        BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit,gport,0,bcmCosqControlLatencyDropEnable, TRUE));
        BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit,gport,0,bcmCosqControlLatencyDropProbEnable, TRUE));
        BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit,gport, 0, bcmCosqControlLatencyDropProbBaseThreshold, 0));
        BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit,gport, 0, bcmCosqControlLatencyDropThreshold, 8000));

        if(*dnxc_data_get(unit, "latency", "features", "ingress_ecn_probabilistic_mechanism_support", NULL))
        {
            BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit,gport,0,bcmCosqControlLatencyEcnProbEnable, FALSE));
        }

    }
    return rv;
}

/** globals for acl rule, which hlod the field group id and entry id. */
/** use it when you want to destroy the group and/or delete the entry */
bcm_field_group_t ing_lat_fg_id;
bcm_field_entry_t ing_lat_ent_id;


int
ingress_latency_flow_acl_rule_set(
    int unit,
    bcm_field_context_t context,
    int *flow_ids,
    int *flow_profiles,
    bcm_field_qualify_t qual_type,
    int qual_mask,
    int *qual_datas,
    int nof_profiles)
{
    int rv  = BCM_E_NONE;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    bcm_gport_t gport;
    void *dest_char;
    int flow_id, flow_profile, qual_data, i;
            
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
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "latency_tsts", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, 0, &fg_info, &ing_lat_fg_id);
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

    rv = bcm_field_group_context_attach(unit, 0, ing_lat_fg_id, context, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    for (i = 0; i < nof_profiles; i++)
    {
        /* Add entry */
        bcm_field_entry_info_t_init(&ent_info);
        ent_info.priority = 1;
        ent_info.nof_entry_quals = 1;
        ent_info.nof_entry_actions = 1;


        flow_id = flow_ids[i];
        flow_profile = flow_profiles[i];
        qual_data = qual_datas[i];

        ent_info.entry_qual[0].type = fg_info.qual_types[0];
        ent_info.entry_qual[0].value[0] = qual_data;
        ent_info.entry_qual[0].mask[0] = qual_mask;
    

        ent_info.entry_action[0].type = fg_info.action_types[0];
        /** for jericho-1 system header mode, flow-id holds the id and the profile as follow: */
        /** Flow-ID [18:3] represents the flow id, Flow-ID [2:0] represents Latency-Flow-Profile */
        if(jericho1_system_header_mode == TRUE)
        {
            flow_id = flow_id << 3 | flow_profile;
        }
        /**  { latency_flow_profile(4), latency_flow_id(19), latency_flow_id_valid(1) } */
        ent_info.entry_action[0].value[0] = 1;
        ent_info.entry_action[0].value[1] = flow_id;
        ent_info.entry_action[0].value[2] = flow_profile;

        rv = bcm_field_entry_add(unit, 0, ing_lat_fg_id, &ent_info, &ing_lat_ent_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_entry_add\n", rv);
            return rv;
        }
    }

    printf("Entry add: id:(0x%x) ingress latency: flow decode (=%d)\n", ing_lat_fg_id ,ent_info.entry_action[0].value[0]);

    return rv;
}

/**
* \brief - function allocate nof_profiles ingress latency profile and map a flow (by PMF) to the profile
*/
int ingress_latency_per_flow_example(
    int unit,
    int *flow_ids,
    int *in_ports,
    int nof_profiles,
    int latency_drop_enable,
    bcm_field_context_t pmf_context)
{
    int bin_idx, profile_idx, rv, i;
    bcm_gport_t gport_qual;
    bcm_gport_t gport_quals[nof_profiles];
    bcm_gport_t latency_profiles[nof_profiles];
    int feature_valid_flow_profile = *dnxc_data_get(unit, "latency", "features", "valid_flow_profile_flag", NULL);
    uint32 flags = 0;

    for(i =  0; i < nof_profiles; i++)
    {

        flags = feature_valid_flow_profile ? BCM_COSQ_LATENCY_PROFILE_BY_FLOW : 0;
        /** create a latency profile */
        BCM_IF_ERROR_RETURN(bcm_cosq_latency_profile_create(unit, flags, &latency_gport));

        /** set the properties of the profile. by default all actions related to latency are disabled. */
        /** Example activate some of them */
        BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit, latency_gport, 0, bcmCosqControlLatencyTrack, TRUE));
        BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit , latency_gport, 0, bcmCosqControlLatencyCounterEnable, TRUE));
        BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit,latency_gport,0,bcmCosqControlLatencyDropEnable, latency_drop_enable));
        BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit,latency_gport,0,bcmCosqControlLatencyEcnEnable, FALSE));
        /** set 10000 nano-second drop threshold */
        BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit,latency_gport, 0, bcmCosqControlLatencyDropThreshold, 20000));
        BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit,latency_gport,0,bcmCosqControlLatencyTrackDropPacket, TRUE));
        /** set bin threshold in jump of 1000 nano-second each, just for the example */
        for(bin_idx = 0; bin_idx < 7; bin_idx++)
        {
            BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit,latency_gport, bin_idx, bcmCosqControlLatencyBinThreshold, (1000 + bin_idx*1000)));
        }

        latency_profiles[i] = BCM_GPORT_INGRESS_LATENCY_PROFILE_GET(latency_gport);

        printf (" ingress latency profile %d was created \n", latency_gport);

    }

    /** map a flow to the profile */
    for(i =  0; i < nof_profiles; i++)
    {
        BCM_GPORT_LOCAL_SET(gport_quals[i], in_ports[i]);
    }

    rv = ingress_latency_flow_acl_rule_set(unit, pmf_context, flow_ids, latency_profiles,
        bcmFieldQualifyInPort, 0x1FF, gport_quals, nof_profiles);
    if (rv != BCM_E_NONE)
    {   
        printf("ingress_latency_flow_acl_rule_set failed \n");
        return rv;    
    }

    return rv;
}

/** hold the max latency value */
uint32 max_latency;
/** holds the number of profiled measured in the max latency table */
uint32 nof_profiles_in_table;
/** hold the dest_gport that belong to the max latency value */
bcm_gport_t dest_gport;
bcm_gport_t latency_flow;


/**
* \brief - function get from HW the max 8 latencies per core
*/
int ingress_latency_max_table_get(
    int unit)
{
    int rv = BCM_E_NONE;
    int actual_count, i;
    int core_idx;
    bcm_gport_t gport;
    uint32 nof_cores = *(dnxc_data_get(unit, "device", "general", "nof_cores", NULL));
    bcm_cosq_max_latency_pkts_t max_latency_pkts[8];
    int memory_actual_count;

    /** Reset the max_latency and dest_gport variables*/
    max_latency = 0;
    dest_gport = 0;
    latency_flow = 0;

    for (core_idx = 0; core_idx < nof_cores; core_idx++)
    {
        BCM_COSQ_GPORT_CORE_SET(gport, core_idx);
        BCM_IF_ERROR_RETURN(bcm_cosq_max_latency_pkts_get(unit, gport, 0, 8, max_latency_pkts, &memory_actual_count));
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
            if(COMPILER_64_LO(max_latency_pkts[i].latency) >  max_latency)
            {
                max_latency = COMPILER_64_LO(max_latency_pkts[i].latency);
                dest_gport = max_latency_pkts[i].dest_gport;
                latency_flow = max_latency_pkts[i].latency_flow;
            }
        }
    }
    nof_profiles_in_table = memory_actual_count;

    return rv;
}

/**
* \brief - create counter processor database in order to hold the max latency value per flow/queue.
*/
int ingress_latency_counter_database_set(
    int unit, 
    int engineId,
    int max_flow_id,
    bcm_stat_expansion_types_t expansion_type)
{
    int rv = BCM_E_NONE;
    int database_id;
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
    interface_key.command_id = 0;
    interface_key.core_id = BCM_CORE_ALL;
    interface_key.interface_source = bcmStatCounterInterfaceIngressTransmitPp;
    interface_key.type_id = 0;
    expansion_select.nof_elements = 1;
    /** possible value: bcmStatExpansionTypeLatencyFlowProfile, bcmStatExpansionTypeTrafficClass, bcmStatExpansionTypeLatencyBin */
    expansion_select.expansion_elements[0].type = expansion_type; 
    BCM_IF_ERROR_RETURN(bcm_stat_counter_expansion_select_set(unit, 0, &interface_key, &expansion_select));
    /** configure and create database */
    database.database_id = engineId;
    database.core_id = BCM_CORE_ALL;
    BCM_IF_ERROR_RETURN(bcm_stat_counter_database_create(unit, BCM_STAT_DATABASE_CREATE_WITH_ID, BCM_CORE_ALL, &database.database_id));   
    /** attach the engines to the database */
    BCM_IF_ERROR_RETURN(bcm_stat_counter_engine_attach(unit, 0, &database, engineId));
   /** configure the counter eviction per engine */
    engine.engine_id = engineId;
    engine.core_id = BCM_CORE_ALL;
    eviction.dma_fifo_select = 0;
    eviction.record_format = bcmStatEvictionRecordFormatPhysical;
    eviction.eviction_event_id = database_id;
    eviction.type = bcmStatEvictionDestinationLocalHost;
    BCM_IF_ERROR_RETURN(bcm_stat_counter_eviction_set(unit, 0, &engine, &eviction)); 

     /** configure the counter interface per database */
    cnt_interface.source = bcmStatCounterInterfaceIngressTransmitPp;
    cnt_interface.command_id = 0;
    /** if the expansion is per bin, it means that we want to count packets per bins and not save the max value. */
    if(expansion_type == bcmStatExpansionTypeLatencyBin)
    {
        cnt_interface.format_type = bcmStatCounterFormatDoublePackets;
    }
    else
    {
        cnt_interface.format_type = bcmStatCounterFormatMaxSize;
    }
    
    if(expansion_type == bcmStatExpansionTypeLatencyFlowProfile)
    {
        cnt_interface.counter_set_size = 32;
    }
    else
    {
        cnt_interface.counter_set_size = 8;
    }
    cnt_interface.type_config[0].valid = 1;
    cnt_interface.type_config[0].object_type_offset = 0;
    cnt_interface.type_config[0].start = 0;
    cnt_interface.type_config[0].end = max_flow_id;
    BCM_IF_ERROR_RETURN(bcm_stat_counter_interface_set(unit, 0, &database, &cnt_interface));
    /**  configure the counter set mapping per database */
    BCM_IF_ERROR_RETURN(bcm_stat_counter_set_mapping_set(unit, BCM_STAT_COUNTER_MAPPING_FULL_SPREAD, &database, &ctr_set_map));

    /**  enable the engine */
    enable_config.enable_counting = TRUE;
    enable_config.enable_eviction = TRUE;
    BCM_IF_ERROR_RETURN(bcm_stat_counter_database_enable_set(unit, 0, &database, &enable_config));

    return rv;
}

/** Stat array holding the 8  bins counter stats */
int stat_array[8];
/**
 * \brief
 *      tcl interface function to set the statistics type in the stat_array and
 *      the expected returned value from API bcm_stat_counter_get/bcm_stat_counter_explicit get.
 *      since tcl is not able to send an array to the cint, or to the API itself, we use this function
 */
int
ingress_latency_crps_stats_array_set(
    int unit,
    int is_bytes,
    int offset)
{
    uint32 high32_data = 0;
    stat_array[offset] = BCM_STAT_COUNTER_STAT_SET(is_bytes, offset);

    return BCM_E_NONE;
}

/**
 * \brief
 *      get statisitic counters value base on core, database_id, type
 *      the statistics type is only packets (and not bytes)
 * \param [in] unit -
 *     unit id
 * \param [in] flags - flags
 * \param [in] core_id -core id 
 * \param [in] database_id -database_id
 * \param [in] type_id -type_id
 * \param [in] object_stat_id -this is the counter pointer 
 * \param [in] nstat -number of statistics to read that belong to the same counter_set
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
int
ingress_latency_crps_database_stat_counter_get(
    int unit,
    int flags,
    int core_id,
    int database_id,
    int type_id,
    int object_stat_id,
    int nstat,
    uint64 *counters_get)
{
    int rv, i;
    const uint32 *max_counter_set_size = dnxc_data_get(unit, "crps", "engine", "max_counter_set_size", NULL);
    bcm_stat_counter_input_data_t input_data;
    bcm_stat_counter_output_data_t output_data;
    bcm_stat_counter_value_t counter_value[(*max_counter_set_size) * 2];
    int check_error = FALSE;

    printf("crps_database_stat_counter_get: database_id=%d, type_id=%d,object_stat_id=%d,nstat=%d \n", database_id,
           type_id, object_stat_id, nstat);
    bcm_stat_counter_input_data_t_init(&input_data);
    bcm_stat_counter_output_data_t_init(&output_data);

    /** fill the input structure */
    input_data.core_id = core_id;
    input_data.database_id = database_id;
    input_data.type_id = type_id;
    input_data.counter_source_id = object_stat_id;
    input_data.nstat = nstat;
    input_data.stat_arr = stat_array;
    output_data.value_arr = counter_value;
    /** call the API */
    rv = bcm_stat_counter_get(unit, flags, &input_data, &output_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_get\n");
        return rv;
    }
    /** compare the received values to the input expected values */
    for (i = 0; i < nstat; i++)
    {
        counters_get[i] = output_data.value_arr[i].value;
    }

    return BCM_E_NONE;
    
}

/**
 * \brief
 *      configure ingress latency profiles mapped to queues and
 *      configure voq admission
 *      Reject bins are configured with thresholds :
 *      1000,  2000,   3000,   2000000,   3000000,  4000000,
 *      5000000
 *      Then the Reject Drop Probabilities are set to 0% -
 *      meaning pass all, except the configuration for bin 3 -
 *      bin 3 has Drop Probability of 100% meaning all latency
 *      with value between 3000 and 2000000 will be dropped.
 * \param [in] unit -
 *     unit id
 * \param [in] unit - unit
 * \param [in] out_port -out_port
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
int
ingress_latency_voq_admission(
    int unit, 
    bcm_port_t in_port,
    bcm_port_t out_port,
    bcm_field_context_t pmf_context)
{
    /** Scenario - reject bin 3 latency, admit all other bins */
    int Latencies[8] = {1000,  2000,   3000,   2000000,   3000000,  4000000,   5000000};
    int Probabilities[4][8]= {
                {0, 0, 0, 100, 0 ,0, 0, 0},
                {0, 0, 0, 100, 0 ,0, 0, 0},
                {0, 0, 0, 100, 0 ,0, 0, 0},
                {0, 0, 0, 100, 0 ,0, 0, 0}
                };

    int LatencyDisabledWhenVOQBelowThisInBytes = 1000, bin_id=0;
    int feature_voq_latency_based_admission_all_profiles_support = *dnxc_data_get(unit, "latency", "based_admission", "cgm_extended_profiles_enable", NULL);
    int nof_profiles = 1;

    bcm_gport_t latency_profile_gport[8],  ingress_voq_bundle_gport[8], gport_qual;

    /** Get port base ingress queue*/
    bcm_gport_t sysport_gport, base_voq_gport, base_voq_rate_class_gport;
    bcm_switch_profile_mapping_t voq_profile_mapping;
    int base_q, key, dp_index, profile_id;
    
    BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, out_port);
    bcm_cosq_sysport_ingress_queue_map_get(unit, 0, sysport_gport, &base_voq_gport);
    base_q = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(base_voq_gport);

    voq_profile_mapping.profile_type=bcmCosqIngressQueueToRateClass;
    BCM_IF_ERROR_RETURN(bcm_cosq_profile_mapping_get(unit, base_voq_gport, 0, 0, &voq_profile_mapping));

    base_voq_rate_class_gport = voq_profile_mapping.mapped_profile;

    /** Create latency profiles for 8 queues and configure bins*/
    for (profile_id=0; profile_id < 8; profile_id++) {
        if (feature_voq_latency_based_admission_all_profiles_support)
        {
            /** for devices which support all latency profiles for voq
             *  latency admission test create profiles with extension -
             *  16 and above */
            BCM_IF_ERROR_RETURN(bcm_cosq_latency_profile_create(unit, BCM_COSQ_LATENCY_PROFILE_ID_EXTENSION, &latency_profile_gport[profile_id]));

        }
        else
        {
            /** for devices which support only first 16 profiles for voq
             *  latency admission test use profiles 4-11 */
            BCM_GPORT_INGRESS_LATENCY_PROFILE_SET(latency_profile_gport[profile_id], (profile_id + 4));
            BCM_IF_ERROR_RETURN(bcm_cosq_latency_profile_create(unit, BCM_COSQ_LATENCY_PROFILE_WITH_ID, &latency_profile_gport[profile_id]));
        }
        /** enable latency measurements */
        BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit, latency_profile_gport[profile_id], 0, bcmCosqControlLatencyCounterEnable, TRUE));
        BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit, latency_profile_gport[profile_id], 0, bcmCosqControlLatencyTrack, TRUE));
        BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit, latency_profile_gport[profile_id], 0, bcmCosqControlLatencyTrackDropPacket, TRUE));
        BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit, latency_profile_gport[profile_id], 0, bcmCosqControlLatencyDropEnable, TRUE));

        for (bin_id=0; bin_id < 7; bin_id++) {
            BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit, latency_profile_gport[profile_id], bin_id, bcmCosqControlLatencyVoqRejectBinThreshold, Latencies[bin_id]));
        }
    }

    /** Attach VOQs to latency profiles */
    voq_profile_mapping.mapped_profile = latency_profile_gport[0];
    voq_profile_mapping.profile_type = bcmCosqIngressQueuesToLatencyProfile;
    BCM_IF_ERROR_RETURN(bcm_cosq_profile_mapping_set(unit, base_voq_gport, 0, 0, &voq_profile_mapping));

    voq_profile_mapping.mapped_profile = latency_profile_gport[4];
    voq_profile_mapping.profile_type = bcmCosqIngressQueuesToLatencyProfile;
    BCM_IF_ERROR_RETURN(bcm_cosq_profile_mapping_set(unit, base_voq_gport, 4, 0, &voq_profile_mapping));


    for (dp_index=0; dp_index < 4; dp_index++) {
        key = BCM_COSQ_GENERIC_KEY_COLOR_SET(key, dp_index);
        BCM_IF_ERROR_RETURN(bcm_cosq_generic_control_set(unit, 0, base_voq_rate_class_gport, key, bcmCosqGenericControlLatencyVoqRejectEnable, TRUE));

    }

    for (dp_index=0; dp_index < 4; dp_index++) {
        for (bin_id=0; bin_id < 8; bin_id++) {
            key = BCM_COSQ_GENERIC_KEY_LAT_BIN_COLOR_SET(key, bin_id, dp_index);
            BCM_IF_ERROR_RETURN(bcm_cosq_generic_control_set(unit, 0, base_voq_rate_class_gport, key, bcmCosqGenericControlLatencyVoqRejectProb, Probabilities[dp_index][bin_id]));
        }
    }

    for (dp_index=0; dp_index < 4; dp_index++) {
        key = BCM_COSQ_GENERIC_KEY_COLOR_SET(key, dp_index);
        BCM_IF_ERROR_RETURN(bcm_cosq_generic_control_set(unit, 0, base_voq_rate_class_gport, key, bcmCosqGenericControlLatencyVoqRejectMinThreshold, LatencyDisabledWhenVOQBelowThisInBytes));
    }

    /** in order to have latency measurements need to add flow id
     *  extension present - this can be made with ACL flow id
     *  rule - creating flow id on the default profile 0 only to
     *  get the extension, even  if later this profile is used
     *  there is no interference - if it is used for flow the
     *  flag for flow will set the valid bit/ if it is used for
     *  VOQ, extension will be present - no issue with the flow
     *  as the valid bit will not be set */
    BCM_GPORT_LOCAL_SET(gport_qual, in_port);
    BCM_IF_ERROR_RETURN(ingress_latency_flow_acl_rule_set(unit, pmf_context, &profile_id, &profile_id, bcmFieldQualifyInPort, 0x1FF, &gport_qual, nof_profiles));

    return BCM_E_NONE;
}





