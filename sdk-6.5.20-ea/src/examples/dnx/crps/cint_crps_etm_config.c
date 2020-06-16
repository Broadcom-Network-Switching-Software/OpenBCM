/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_crps_etm_config.c
 * related files: cint_crps_db_utils.c
 * Purpose:     Example of configuring a statistic database from one engine of egress tm (etm) using the counter processor.
 *                  
 * *
 * The cint include:
 *  - configure a statistic data base for egress voq.
 *  -
 *  Note: etm meta_data is smaller than 10 bits, therfrore, there is no expansion selection for it.
 */

/**
* \brief  
*   configure an etm (egress tm) data base from one engine and enable it's counting and eviction
*   example assume that the engine already cleared and not in used.
* \par DIRECT INPUT: 
*   \param [in] unit                  -  
*   \param [in] core                  -  
*   \param [in] database_id   -  id for the database that will may be used when reading a counter
*   \param [in] counter_format        -  
*   \param [in] start_object_stat_id  -  
*   \param [in] engine_id             -  
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
crps_etm_data_base_create(
    int unit,
    int core,
    int database_id,
    bcm_stat_counter_format_type_t counter_format,
    int start_object_stat_id,
    int engine_id)
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
    bcm_stat_expansion_data_mapping_t data_mapping_array[8];
    uint32 *engine_size = dnxc_data_1d_get(unit, "crps", "engine", "engines_info", "nof_counters", engine_id);
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

    /** configure database */
    database.database_id = database_id;
    database.core_id = core;
    rv = bcm_stat_counter_database_create(unit, BCM_STAT_DATABASE_CREATE_WITH_ID, core, database_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_database_create\n");
        return rv;
    }

    /** fill the engine structure */
    engine.core_id = core;
    engine.engine_id = engine_id;
    /** attach one engine to the database */
    rv = bcm_stat_counter_engine_attach(unit, 0, &database, engine_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_engine_attach\n");
        return rv;
    }

     /** configure the structure for API bcm_stat_counter_eviction_set */
    eviction.dma_fifo_select = 1;
    eviction.record_format = bcmStatEvictionRecordFormatPhysical;
    eviction.eviction_event_id = database_id;
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
    rv = bcm_stat_counter_eviction_set(unit, eviction_flags, &engine, &eviction);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_eviction_set\n");
        return rv;
    }

    cnt_interface.source = bcmStatCounterInterfaceEgressReceiveQueue;
    cnt_interface.command_id = 0;
    cnt_interface.type_config[0].valid = TRUE;
    cnt_interface.type_config[0].object_type_offset = 0;
    cnt_interface.type_config[0].start = start_object_stat_id;
    cnt_interface.format_type = counter_format;
    /** 8 offsets. each offset represent different tc value */
    cnt_interface.counter_set_size = 8;
    cnt_interface.type_config[0].end = start_object_stat_id + ((*engine_size)/cnt_interface.counter_set_size) - 1;
    
    rv = bcm_stat_counter_interface_set(unit, interface_flags, &database, &cnt_interface);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_interface_set\n");
        return rv;
    }

    ctr_set_map.nof_entries = 8;
    for (i = 0; i < ctr_set_map.nof_entries; i++)
    {
        data_mapping_array[i].nof_key_conditions = 1;
        data_mapping_array[i].key[0].type = bcmStatExpansionTypeTrafficClass;
        data_mapping_array[i].key[0].value = i;
        data_mapping_array[i].value.counter_set_offset = i;
        data_mapping_array[i].value.valid = TRUE;
    }
    ctr_set_map.expansion_data_mapping = data_mapping_array;    
    rv = bcm_stat_counter_set_mapping_set(unit, mapping_flags, &database, &ctr_set_map);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_set_mapping_set\n");
        return rv;
    }

   /** enable the database */
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
 *   example of crps database with selected core_id, connected
 *   to etm and print the database to screen. database
 *   description: (parameters value are arbitrary example.)
 *       For each core, one engine connected to src etm, command_id=0.
 * \param [in] unit -
 *     unit id
 *  \param [in] engine_id - one engine is created , connected to
 *         database with the same id
 *  \param [in] core_id - core_id
 * \return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
int
crps_etm_database_example(
    int unit,
    int engine_id,
    int core_id)
{
    int rv;
    int database_id = engine_id;
    int start_object_stat_id = 0;
    int command_id = 0;

        
    rv = crps_etm_data_base_create(unit, core_id, database_id,
            bcmStatCounterFormatPacketsAndBytes, start_object_stat_id, engine_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in crps_etm_data_base_create \n");
        return rv;
    }     

    return BCM_E_NONE;      
}

