/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_crps_itm_config.c
 * related files: cint_crps_db_utils.c 
 * Purpose:     Example of configuring a statistic database of ingress voq (itm) using the counter processor.
 * 
 * *
 * The cint include:
 *  - configure a statistic data base for ingress voq, one configuration for all engines that belong to the data base.
 *  - configure specific parameters per engine (engine id,
 *    engine counters range etc) and enable the database
 *  - add new range to the database
 *  - remove a range of a database
 */

enum crps_itm_config_database_t {
    CRPS_ITM_ADD_NEW_RANGE = 0, 
    CRPS_ITM_REMOVE_RANGE = 1,
    CRPS_ITM_NORMAL_DATABASE =2
};

/**
* \brief  
*   configure an itm (ingress VOQ) data base and enable it's counting and eviction
* \par DIRECT INPUT: 
*   \param [in] unit                  -  
*   \param [in] core                  -  
*   \param [in] database_id   -  id for the database that will may be used when reading a counter
*   \param [in] counter_format        -  
*   \param [in] start_object_stat_id  -  
*   \param [in] end_object_stat_id    -  
*   \param [in] nof_engines           -  
*   \param [in] engine_id_arr         -  
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
int
crps_itm_voq_data_base_create(
    int unit,
    int core,
    int database_id,
    bcm_stat_counter_format_type_t counter_format,
    int start_object_stat_id,
    int end_object_stat_id,
    int nof_engines,
    int *engine_id_arr)
{
    int rv, i;
    int interface_flags = 0;
    int mapping_flags = 0;
    int eviction_flags = 0;
    int enable_flags = 0;
    bcm_stat_counter_database_t database;
    bcm_stat_engine_t engine;
    bcm_stat_counter_enable_t enable_config;
    bcm_stat_counter_interface_t cnt_interface;
    bcm_stat_counter_set_map_t ctr_set_map;
    bcm_stat_eviction_t eviction;
    bcm_stat_counter_engine_control_t control = bcmStatCounterClearAll;
    bcm_color_t colors[4] = { bcmColorGreen, bcmColorYellow, bcmColorRed, bcmColorBlack };
    bcm_stat_expansion_data_mapping_t data_mapping_array[8];
    uint32 feature_conditional_eviction_support = 
        *dnxc_data_get(unit, "crps", "eviction", "conditional_action_is_supported", NULL);
    uint32 *conditional_no_eviction_support;
    int temp = 0, action_idx = -1;
    /** calc no eviction flag bit index */    
    while ((BCM_STAT_EVICTION_CONDITIONAL_ACTION_NO_EVICTION & temp) == 0)
    {
        action_idx++;
        temp = 1 << action_idx;        
    }    
    conditional_no_eviction_support = 
            dnxc_data_1d_get(unit, "crps", "eviction", "condional_action_valid", "valid", action_idx); 


    bcm_stat_counter_database_t_init(&database);
    bcm_stat_engine_t_init(&engine);
    bcm_stat_eviction_t_init(&eviction);
    bcm_stat_counter_set_map_t_init(&ctr_set_map);
    bcm_stat_counter_interface_t_init(&cnt_interface);
    bcm_stat_counter_enable_t_init(&enable_config);

    printf("*********** Create database: database_id[%d], core=%d **************\n\n", database_id, core);

    /** configure the structure for API bcm_stat_counter_eviction_set */
    eviction.dma_fifo_select = 0;
    eviction.record_format = bcmStatEvictionRecordFormatPhysical;
    eviction.eviction_event_id = 0;
    eviction.type = bcmStatEvictionDestinationLocalHost;
    /** optimization - do not evict a counter which its value is zero */
    /** HW compare 34 bits of the counter */
    if ((*conditional_no_eviction_support == TRUE) && (feature_conditional_eviction_support == TRUE))
    {
        eviction_flags |= BCM_STAT_EVICTION_CONDITIONAL_ENABLE;
        eviction.cond.action_flags = BCM_STAT_EVICTION_CONDITIONAL_ACTION_NO_EVICTION;
        eviction.cond.condition_source_select = bcmStatEvictionConditionalSourceUserData;
        COMPILER_64_SET(eviction.cond.condition_user_data, 0x0, 0x0);
        eviction.cond.qualifier = bcmStatEvictionConditionalQualOrEqualZero;
    }

    /** step 1: configure and create the database */
    database.database_id = database_id;
    database.core_id = core;
    rv = bcm_stat_counter_database_create(unit, BCM_STAT_DATABASE_CREATE_WITH_ID, core, &database_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_database_create\n");
        return rv;
    }

    /** configure all eninges and enable them */
    for (i = 0; i < nof_engines; i++)
    {
        engine.core_id = core;
        engine.engine_id = engine_id_arr[i];

        printf("*********** set API: engine_id[%d], core=%d **************\n\n", engine.engine_id, engine.core_id);

        /** step 2: attach the engines to the database */
        rv = bcm_stat_counter_engine_attach(unit, 0, &database, engine_id_arr[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_stat_counter_engine_attach\n");
            return rv;
        }

        /** step 3: configure the counter eviction per engine */
        rv = bcm_stat_counter_eviction_set(unit, eviction_flags, &engine, &eviction);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_stat_counter_eviction_set\n");
            return rv;
        }
    }

    /** configure interface and data mapping structure  */
    cnt_interface.source = bcmStatCounterInterfaceIngressVoq;
    cnt_interface.command_id = 0;       /* can be 0 or 1 */
    /** numof types */
    cnt_interface.type_config[0].valid = TRUE;
    cnt_interface.type_config[0].object_type_offset = 0;
    cnt_interface.format_type = counter_format;

    /** in this example, for max voq, we don't need any expansion. the counter set will be 1. */
    if (counter_format == bcmStatCounterFormatMaxSize)
    {
        cnt_interface.counter_set_size = 1;
        ctr_set_map.nof_entries = 1;
        /*
         * create a key that contain all valid fields from all values. total 9 bits, all options -meaning it will
         * create 512 entries
         */
        data_mapping_array[0].nof_key_conditions = 0;
        data_mapping_array[0].value.counter_set_offset = 0;
        data_mapping_array[0].value.valid = TRUE;
        ctr_set_map.expansion_data_mapping = data_mapping_array;
    }
    /** for queues counting, use expansion of DPxDISPOSITION */
    else
    {
        cnt_interface.counter_set_size = 4;
        ctr_set_map.nof_entries = 4;
        /** configure each offset in the set to count different value of DPxDISPOSITION */
        /** offset 0 - Green fwd; offset 1- Green drop; offset 2 -
         *  Yellow fwd; offset 3 - yellow drop */
        for (i = 0; i < ctr_set_map.nof_entries; i++)
        {
            data_mapping_array[i].nof_key_conditions = 2;
            data_mapping_array[i].key[0].type = bcmStatExpansionTypeDropPrecedenceMeterResolved;
            data_mapping_array[i].key[0].value = colors[i / 2];
            data_mapping_array[i].key[1].type = bcmStatExpansionTypeDispositionIsDrop;
            data_mapping_array[i].key[1].value = i % 2;
            data_mapping_array[i].value.counter_set_offset = i;
            data_mapping_array[i].value.valid = TRUE;
        }
        ctr_set_map.expansion_data_mapping = data_mapping_array;
    }

    /** step 4: configure the counter interface per database */
    cnt_interface.type_config[0].start = start_object_stat_id;
    cnt_interface.type_config[0].end = end_object_stat_id;
    rv = bcm_stat_counter_interface_set(unit, interface_flags, &database, &cnt_interface);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_interface_set\n");
        return rv;
    }

    /** step 5: configure the counter set mapping per database */
    rv = bcm_stat_counter_set_mapping_set(unit, mapping_flags, &database, &ctr_set_map);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_set_mapping_set\n");
        return rv;
    }

    /** step 6: enable the database */
    enable_config.enable_counting = TRUE;
    enable_config.enable_eviction = TRUE;
    rv = bcm_stat_counter_database_enable_set(unit, 0, &database, &enable_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_database_enable_set\n");
        return rv;
    }

    return BCM_E_NONE;
}

/**
* \brief  
*   crps_itm_voq_data_base_range_add function handle scenario that the user want to extend his counting VOQs range
*    (for example: when adding new card to the sytem) 
*   this function feets to scenario that the user wants to concatanate the new range to the original range.
*   For example: If the user want to add 8K to the currecnt range, and the current range was: object-stat-id 0-4K,
*   the new range will be 0-12K.
*   user wants to use the free space (if exist) of the last engine in the data base and concatenate it another engines.
*   as showed in the following  sketch:
*
*    
*  DB range before.                DB range after.
*  it contains only half   ==>   it contains two engines
*  engine 
*   ___                              ___    ___
*  |***|                            |***|  |***|
*  |***|                            |***|  |***|
*  |   |                            |***|  |***|
*  |   |                            |***|  |   |

* \par DIRECT INPUT: 
*   \param [in] unit                  -  
*   \param [in] core                  -
*   \param [in] database_id - database id
*   \param [in] db_last_engine_id     -  the last engine id of the current data base.
*   \param [in] additonal_range_size  - the additional range that need to add to the data base
*   \param [in] nof_engines           -  
*   \param [in] engine_id_arr         -  
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
int
crps_itm_voq_data_base_range_add(
    int unit,
    int core,
    int database_id,
    int db_last_engine_id,
    int additonal_range_size,
    int nof_engines,
    int *engine_id_arr)
{
    int rv, i, new_end_data_base_range;
    int flags;
    bcm_stat_counter_database_t database;
    bcm_stat_engine_t engine;
    bcm_stat_counter_interface_t cnt_interface;
    bcm_stat_eviction_t eviction;

    bcm_stat_counter_database_t_init(&database);
    bcm_stat_engine_t_init(&engine);
    bcm_stat_eviction_t_init(&eviction);
    bcm_stat_counter_interface_t_init(&cnt_interface);

    printf("*********** Add new range to database: database_id[%d], core=%d **************\n\n", database_id, core);

    /** configura database stucture */
    database.database_id = database_id;
    database.core_id = core;

    /** get the eviction structure of the last engine */
    engine.engine_id = db_last_engine_id;
    engine.core_id = core;
    rv = bcm_stat_counter_eviction_get(unit, 0, &engine, &eviction);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_eviction_get\n");
        return rv;
    }

    /** step 1: attach the new engines to the database and select
     *  eviction mode */
    for (i = 0; i < nof_engines; i++)
    {
        engine.core_id = core;
        engine.engine_id = engine_id_arr[i];

        /** attach engines to the database */
        rv = bcm_stat_counter_engine_attach(unit, 0, database, engine_id_arr[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_stat_counter_engine_attach\n");
            return rv;
        }
        /** configure the counter eviction per engine */
        rv = bcm_stat_counter_eviction_set(unit, 0, &engine, &eviction);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_stat_counter_eviction_set\n");
            return rv;
        }
    }

    /** step 2: get the database configuration of interface */
    rv = bcm_stat_counter_interface_get(unit, 0, &database, &cnt_interface);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_interface_get\n");
        return rv;
    }

    /** hold the new end point of the data base range */
    new_end_data_base_range = (cnt_interface.type_config[0].end + additonal_range_size);

    /** step 3: extend the range of the last engine of the current
     *  data base and connect it to new engine if needed */
    cnt_interface.type_config[0].end = new_end_data_base_range;
    /** this flag will allow the user to extend the range while the engine is enabled */
    flags = BCM_STAT_COUNTER_MODIFY_RANGE_ONLY;
    rv = bcm_stat_counter_interface_set(unit, flags, &database, &cnt_interface);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_interface_set\n");
        return rv;
    }

    return BCM_E_NONE;
}

/**
* \brief  
*   User can reduce a database range using
*   bcm_stat_counter_interface_set with MODIFY_ONLY flag. If
*   there is a need to remove engines they should be detached
*   from the database and this will detach and reset the engine.
* \par DIRECT INPUT: 
*   \param [in] unit           -  
*   \param [in] core           -
*   \param [in] database_id -
*   \param [in] remove_range_size -
*   \param [in] nof_engines    -  
*   \param [in] engine_id_arr  -  
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
int
crps_itm_crps_data_base_range_remove(
    int unit,
    int core,
    int database_id,
    int remove_range_size,
    int nof_engines,
    int *engine_id_arr)
{
    int rv, i, new_end_data_base_range;
    int flags;
    bcm_stat_counter_database_t database;
    bcm_stat_engine_t engine;
    bcm_stat_counter_interface_t cnt_interface;

    bcm_stat_counter_database_t_init(&database);
    bcm_stat_engine_t_init(&engine);
    bcm_stat_counter_interface_t_init(&cnt_interface);

    printf("*********** Remove range from database: database_id[%d], core=%d **************\n\n", database_id, core);

    /** configura database stucture */
    database.database_id = database_id;
    database.core_id = core;

     /** if needed - detach the unused engines */
    for (i = 0; i < nof_engines; i++)
    {
        /** detach engines to the database */
        rv = bcm_stat_counter_engine_detach(unit, 0, &database, engine_id_arr[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_stat_counter_engine_attach\n");
            return rv;
        }
    }

    /** get interface configuration for this database */
    rv = bcm_stat_counter_interface_get(unit, 0, &database, &cnt_interface);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_interface_get\n");
        return rv;
    }
    /** hold the new end point of the data base range */
    new_end_data_base_range = (cnt_interface.type_config[0].end - remove_range_size);

    /** reduce the range of the last engine of the
     *  current data base  */
    cnt_interface.type_config[0].end = new_end_data_base_range;
    /** this flag will allow the user to extend the range while the engine is enabled */
    flags = BCM_STAT_COUNTER_MODIFY_RANGE_ONLY;
    rv = bcm_stat_counter_interface_set(unit, flags, &database, &cnt_interface);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_interface_set\n");
        return rv;
    }

    return BCM_E_NONE;
}

/**
 * \brief
 *   example of crps database with selected core_id, connected
 *   to itm.
 *   If want to test add/remove range - follow this sequence:
 *   (this sequence is specifically for this test case)
 *   1. create normal database.
 *   2. add range to the database.
 *   3. remove range from the database.
 * \param [in] unit -
 *     unit id
 * \param [in] database_id - database_id 
 * \param [in] core_id - core_id 
 * \param [in] create_db - create normal database, add range to 
 *        this database, remove range to this database
 * \return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
int
crps_itm_database_example(
    int unit,
    int database_id,
    int core_id,
    crps_itm_config_database_t create_db)
{
    uint32 nof_all_engines = *(dnxc_data_get(unit, "crps", "engine", "nof_engines", NULL));
    int rv,i;
    int nof_engines = 6;
    int engine_id[nof_engines] = {database_id, database_id+1, database_id+2, database_id+3, database_id+4, database_id+5};
    int engine_id_remove[3] = {nof_all_engines -1,database_id+5, database_id+4}; /** as removing is made from last, we assume that add range was called before remove range*/
    int engine_id_add[1] = {nof_all_engines-1};
    uint32* engine_size;
    int start_object_stat_id = 0;
    int end_object_stat_id = 0;
    int command_id = 0;

         
    end_object_stat_id = 0;
    /** normal data_base size  */
    for(i = 0; i < nof_engines; i++)
    {
        engine_size = dnxc_data_1d_get(unit, "crps", "engine", "engines_info", "nof_counters", engine_id[i]);
        end_object_stat_id += (*engine_size);
    }
    /* just for the test, we want that end will be in the middle of the last engine */
    engine_size = dnxc_data_1d_get(unit, "crps", "engine", "engines_info", "nof_counters", engine_id[i-1]);        
    end_object_stat_id = end_object_stat_id - ((*engine_size)/2);
    /** using counter_set_size=4, need to devide the range in this number */
    end_object_stat_id = (end_object_stat_id - 1)/4;
    if (create_db == CRPS_ITM_NORMAL_DATABASE )
    {
        rv = crps_itm_voq_data_base_create(unit, core_id, database_id, bcmStatCounterFormatPackets, start_object_stat_id, end_object_stat_id, nof_engines, engine_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error in crps_itm_data_base_create \n");
            return rv;
        }
    }
    else if (create_db == CRPS_ITM_ADD_NEW_RANGE)
    {
        /** add another range of 128 stat_ids, in new engine to the database */
        /** add engine id 21 */
        engine_size = dnxc_data_1d_get(unit, "crps", "engine", "engines_info", "nof_counters", engine_id_add[0]);
        rv = crps_itm_voq_data_base_range_add(unit, core_id, database_id, engine_id[nof_engines-1], ((*engine_size)/4), 1, engine_id_add);
        if (rv != BCM_E_NONE)
        {
            printf("Error in crps_itm_voq_data_base_range_add \n");
            return rv;
        }
    }
    else /** create_db = CRPS_ITM_REMOVE_RANGE */
    {
        /** reduce range */
        /** normal db size 6*8K, add range 16K => 64K/4, remove last 3
         *  engines - 16K+2*8K=32K/4, meaning reduce to half */
        rv = crps_itm_crps_data_base_range_remove(unit, core_id, database_id, 8*1024, 3, engine_id_remove);
        if (rv != BCM_E_NONE)
        {
            printf("Error in crps_itm_crps_data_base_range_remove \n");
            return rv;
        }
    }

    return BCM_E_NONE;      
}

/**
 * \brief
 *  A wrapper function for the function "crps_database_stat_counter_get". 
 *  function gets the gport (voq) to read it's counter and translate it into object_stat_id.
 * \param [in] unit -unit id
 * \param [in] gport -gport represent ingress voq. (ucast/mcast)
 * \param [in] command_id -command_id (2 commands for itm towards crps)
 * \return
 *   object_stat_id that match to the voq for the command_id. ( -1 if not found)
 * \remark
 *   NONE
 * \see
 *   NONE
 */
int
crps_itm_gport_to_object_stat_id_translate(
    int unit,    
    bcm_gport_t gport,
    int command_id)
{
    int object_stat_id;
    int rv;
    bcm_cosq_stat_info_key_t info_key;
    bcm_cosq_stat_info_t config;
    bcm_cosq_obj_map_key_t map_key;

    info_key.command_id = command_id;
    rv = bcm_cosq_stat_info_get(unit, 0, &info_key, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in API bcm_cosq_stat_info_get \n");
        return -1;
    }        
       
    if(config.enable_mapping == TRUE)
    {                   
        map_key.gport = gport;
        rv = bcm_cosq_stat_obj_map_get(unit, 0, &map_key, &object_stat_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error in API bcm_cosq_stat_obj_map_get \n");
            return -1;
        }           
    }        
    else
    {
        if(BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) == TRUE)
        {
            object_stat_id = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(gport);
        }
        else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) == TRUE)
        {
            object_stat_id = BCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(gport);
        }
        else
        {
            printf("invalid gport (=0x%X) \n", gport);
            return -1;
        }
    }
    object_stat_id = object_stat_id / config.num_queues_per_entry;       

    return object_stat_id;
}
    


