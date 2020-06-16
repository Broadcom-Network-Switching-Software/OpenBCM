/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_crps_oam_config.c
 * Purpose:     Configuring a statistic database of OAM using the counter processor.
 *              the database is built from one or more engines depending on the parameters the user gave
 * 
 * The cint include:
 *  - configure CRPS OAM database with the given engines and parameters
 *  - destroy the database
 *  - example of configuring 3 databases
 */

/** 
* Choose whenever to set or not eviction for OAMP
* Set to {1} when allocating counters for OAM statistics
* Set to {0} when allocating counters for SLM 
*  
* Should also be set to 1 for alternate marking ingress node 
* (see cint_mpls_deep_stack_alternate_marking.c) 
*  
* All other cases should remain 0. 
*/
int crps_oam_config_enable_eviction = 0;

/**
* \brief  
*   Configure CRPS with IngressOam/EgressOam/Oamp source
*   user gives the source, the command id, array with engines to be configured
*   and the counter base, provide also the database_id
*  
*   database_id value can be between 0 till 21 (as are the
*   engine ids - so you can create maximum 22 databases per
*   core)
*  
*   If BCM_CORE_ALL is selected two databases will be created -
*   one for core 0 and one on core 1 with the same database_ids
*   and the same engines.
*  
*   Once the parameters are given a CRPS database is created
*   that represents the given source+command_id+the engines.
*   This database is represented with database_id.
*  
*   If new database is to be created alongside with the previous
*   - it should have different engines and different
*     database_id.
*  
*   Once finished with the database - please use
*   crps_oam_database_destroy() to destroy the database.
*  
*   Possible sources: bcmStatCounterInterfaceIngressOam;
*   bcmStatCounterInterfaceEgressOam;
*       bcmStatCounterInterfaceOamp Possible command ids: For
*       IngressOam : 7,8,9 For EgressOam : 0,1,2
*       For Oamp: 0
* 
*   Create database on the given core with the given engines,
*   OAM source, command id, database_id
* \par DIRECT INPUT:
*   \param [in] unit            -  UNIT_ID
*   \param [in] core            -  CORE_ID, BCM_CORE_ALL
*          supported
*   \param [in] source          -  OAM source typede
*   \param [in] command_id      -  command_id
*   \param [in] nof_engines     -  number of engines to configure
*   \param [in] engine_id_array -  array with engine ids to configure
*   \param [in] counter_base    -  the start of the counter range
*   \param [in] database_id     -  database_id (0-21)
*   \param [out] actual_nof_counters - actual num of counters in
*          the database
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
crps_oam_database_set(
    int unit,
    int core_id,
    bcm_stat_counter_interface_type_t source,
    int command_id,
    int nof_engines,
    int *engine_id_array,
    int counter_base,
    int database_id,
    uint32 *actual_nof_counters)
{
    int rv, flags = 0, i = 0;
    bcm_stat_counter_database_t database;
    bcm_stat_engine_t engine;
    bcm_stat_counter_enable_t enable_config;
    bcm_stat_counter_interface_t interface;
    bcm_stat_counter_set_map_t ctr_set_map;
    bcm_stat_counter_interface_key_t interface_key;
    bcm_stat_expansion_select_t expansion_select;
    bcm_stat_counter_engine_control_t control = bcmStatCounterClearAll;
    bcm_stat_expansion_data_mapping_t data_mapping_array[1];
    int counter_set_size = 1; /** For OAM sources counter_set_size should be always 1 */
    int type_id = 0; /** Configure only one type */
    uint32 *engine_size;
    uint32 total_nof_counters = 0; /** Calculate the size of the database*/
    bcm_stat_eviction_t eviction;
    uint32 eviction_flags = 0;
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

    
    bcm_stat_eviction_t_init(&eviction);
    bcm_stat_counter_database_t_init(&database);
    bcm_stat_engine_t_init(&engine);
    bcm_stat_counter_set_map_t_init(&ctr_set_map);
    bcm_stat_expansion_data_mapping_t_init(&data_mapping_array);
    bcm_stat_counter_interface_key_t_init(&interface_key);
    bcm_stat_expansion_select_t_init(&expansion_select);
    bcm_stat_counter_interface_t_init(&interface);
    bcm_stat_counter_enable_t_init(&enable_config);
    ctr_set_map.expansion_data_mapping = data_mapping_array;
    /** check if the given source are indeed OAM sources
    * command id will be verified inside the API */
    if (source != bcmStatCounterInterfaceIngressOam && source != bcmStatCounterInterfaceEgressOam && source != bcmStatCounterInterfaceOamp)
    {
        printf("Error in crps_oam_source_engines_set - call the function with OAM source\n");
         return BCM_E_PARAM;
    }
    /** configure expansion - if sources Ingress/EgressOam; for OAMP there is no expansion and API should not be called */
    if (source == bcmStatCounterInterfaceIngressOam || source == bcmStatCounterInterfaceEgressOam)
    {
        
        expansion_select.nof_elements = 0;
        interface_key.command_id = command_id;
        interface_key.core_id = core_id;
        interface_key.interface_source = source;
        interface_key.type_id = type_id;
        rv = bcm_stat_counter_expansion_select_set(unit, flags, &interface_key, &expansion_select);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_stat_counter_expansion_select_set\n");
            return rv;
        }
    }

    /** create database */
    rv = bcm_stat_counter_database_create(unit, BCM_STAT_DATABASE_CREATE_WITH_ID, core_id, database_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_database_create\n");
        return rv;
    }
    database.database_id = database_id;
    database.core_id = core_id;
    
    eviction.dma_fifo_select = 0;
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

    if (source != bcmStatCounterInterfaceOamp)
    {
        eviction.eviction_algorithmic_disable = 1;
    }

     /** calling the APIs per engine - attach engines to the
      *  database  */
    for (i = 0; i < nof_engines; i++)
    {
        engine.core_id = core_id;
        engine.engine_id = engine_id_array[i];
        /** attach the engines to the database */
        rv = bcm_stat_counter_engine_attach(unit, flags, &database, engine_id_array[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_stat_counter_engine_attach\n");
            return rv;
        }
        if (source != bcmStatCounterInterfaceEgressOam)
        {
            rv = bcm_stat_counter_eviction_set(unit, eviction_flags, &engine, &eviction);
            if (rv != BCM_E_NONE)
            {
                printf("Error in bcm_stat_counter_eviction_set\n");
                return rv;
            }
        }
        /** add engine size to the total number of counters in the
         *  database to get the database size */
        engine_size = dnxc_data_1d_get(unit, "crps", "engine", "engines_info", "nof_counters", engine_id_array[i]);
        total_nof_counters = total_nof_counters + *engine_size;
    }
    /** the mode is double entry */
    *actual_nof_counters = total_nof_counters * 2;

    /** configure the counter interface for the OAM crps database */
    interface.source = source;
    interface.command_id = command_id;
    interface.format_type = bcmStatCounterFormatDoublePackets; /** double entry*/
    interface.counter_set_size = counter_set_size;
    interface.type_config[type_id].valid = TRUE;
    interface.type_config[type_id].object_type_offset = 0; /** counter_set_size is 1 => there is one offset - offset 0*/
    interface.type_config[type_id].start = counter_base; /** user selected what the counter base is*/
    interface.type_config[type_id].end = counter_base + *actual_nof_counters - 1; /** untill the end of the database size*/
    rv = bcm_stat_counter_interface_set(unit, flags, &database, &interface);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_interface_set\n");
        return rv;
    }

    /** configure the counter set mapping for the database */
    ctr_set_map.nof_entries = 1;
    ctr_set_map.expansion_data_mapping[0].nof_key_conditions = 0;
    ctr_set_map.expansion_data_mapping[0].value.counter_set_offset = 0;
    ctr_set_map.expansion_data_mapping[0].value.valid = TRUE;
    rv = bcm_stat_counter_set_mapping_set(unit, flags, &database, &ctr_set_map);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_set_mapping_set\n");
        return rv;
    }

     /** enable the database */
    enable_config.enable_counting = TRUE;
    enable_config.enable_eviction = FALSE;
    if (crps_oam_config_enable_eviction)    {
        enable_config.enable_eviction = TRUE; /** enable eviction for OAMP/IngressOAM source */
    } 
    rv = bcm_stat_counter_database_enable_set(unit, flags, &database, &enable_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_database_enable_set\n");
        return rv;
    }

    return BCM_E_NONE;
}

/**
* \brief  
*   This function should be called after crps_oam_database_set
*   is called. Once database is created and user decides to add
*   new engines to it - use the database id and core id that was
*   used for crps_oam_database_set. 
*   To add new engines select their ids and provde them - please
*   provide only the new engine ids, also the number of engines
*   to be added. These new engines will be added in the end of
*   the database.
*   The new size of the database is returned.
*  
* \par DIRECT INPUT: 
*   \param [in] unit                  -  
*   \param [in] core                 -
*   \param [in] database_id     - database id
*   \param [in] nof_new_engines_to_add - number of the new
*          engines to be added
*   \param [in] new_engines_id_to_add_array   - array with the
*          new engine ids to be added
*   \param [out] actual_nof_counters - actual nof counters in
*          the database (old + new)
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
crps_oam_database_engines_add(
    int unit,
    int core,
    int database_id,
    int nof_new_engines_to_add,
    int *new_engines_id_to_add_array,
    uint32 *actual_nof_counters)
{
    int rv, i;
    int flags, base_engine_id;
    uint32 additonal_range_size = 0;
    bcm_stat_counter_database_t database;
    bcm_stat_engine_t engine;
    bcm_stat_counter_interface_t interface;
    bcm_stat_eviction_t eviction;
    uint32 *engine_size;
    int type_id = 0; /** only one type is configured and used*/

    bcm_stat_counter_database_t_init(&database);
    bcm_stat_engine_t_init(&engine);
    bcm_stat_eviction_t_init(&eviction);
    bcm_stat_counter_interface_t_init(&interface);
    flags = 0;

    /** configura database stucture */
    database.database_id = database_id;
    database.core_id = core;

    /** step 1: attach the new engines to the database and
     *  calculate their size */
    for (i = 0; i < nof_new_engines_to_add; i++)
    {
        engine.core_id = core;
        engine.engine_id = new_engines_id_to_add_array[i];

        /** attach engines to the database */
        rv = bcm_stat_counter_engine_attach(unit, flags, database, new_engines_id_to_add_array[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_stat_counter_engine_attach\n");
            return rv;
        }
        /** add engine size to the total number of counters in the
         *  database to get the database size */
        engine_size = dnxc_data_1d_get(unit, "crps", "engine", "engines_info", "nof_counters", new_engines_id_to_add_array[i]);
        additonal_range_size = additonal_range_size + *engine_size;
    }
    /** the format is double packets */
    additonal_range_size = additonal_range_size * 2;

    /** step 2: get the database configuration of interface */
    rv = bcm_stat_counter_interface_get(unit, flags, &database, &interface);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_interface_get\n");
        return rv;
    }

    /** hold the new end point of the data base range */
    *actual_nof_counters = (interface.type_config[type_id].end + 1 + additonal_range_size);

    /** step 3: extend the range of the last engine of the current
     *  data base and connect it to new engine if needed */
    interface.type_config[0].end = *actual_nof_counters - 1;
    /** this flag will allow the user to extend the range while the engine is enabled */
    flags = BCM_STAT_COUNTER_MODIFY_RANGE_ONLY;
    rv = bcm_stat_counter_interface_set(unit, flags, &database, &interface);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_interface_set\n");
        return rv;
    }

    return BCM_E_NONE;
}

/**
* \brief  
*   This function should be called after crps_oam_database_set
*   is called. Once database is created and user decides to
*   remove engines from it
*   - use the database id and core id that was used for
*   crps_oam_database_set.
*   To remove engines select their ids and provde them - please
*   provide only the engine ids that need to be removed, also
*   the number of engines to be removed. 
*   The engine ids removed from the database should be provided
*   from last to first from the current database.
*   The new size of the database is returned.
*  
* \par DIRECT INPUT: 
*   \param [in] unit                  -  
*   \param [in] core                 -
*   \param [in] database_id     - database id
*   \param [in] nof_new_engines_to_remove - number of engines to
*          be removed
*   \param [in] engines_id_to_remove_array   - array with the
*          engine ids to be removed
*   \param [out] actual_nof_counters - actual nof counters in
*          the database (old - removed)
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
crps_oam_database_engines_remove(
    int unit,
    int core,
    int database_id,
    int nof_new_engines_to_remove,
    int *engines_id_to_remove_array,
    uint32 *actual_nof_counters)
{
    int rv, i;
    int flags = 0, type_id = 0;
    bcm_stat_counter_database_t database;
    bcm_stat_engine_t engine;
    bcm_stat_counter_interface_t interface;
    uint32 remove_range_size = 0;
    uint32 *engine_size;
    
    bcm_stat_counter_database_t_init(&database);
    bcm_stat_engine_t_init(&engine);
    bcm_stat_counter_interface_t_init(&interface);

    /** configura database stucture */
    database.database_id = database_id;
    database.core_id = core;

     /** if needed - detach the unused engines */
    for (i = 0; i < nof_new_engines_to_remove; i++)
    {
        /** detach engines to the database */
        rv = bcm_stat_counter_engine_detach(unit, flags, &database, engines_id_to_remove_array[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_stat_counter_engine_attach\n");
            return rv;
        }
        /** add engine size to the total number of counters in the
         *  database to get the database size */
        engine_size = dnxc_data_1d_get(unit, "crps", "engine", "engines_info", "nof_counters", engines_id_to_remove_array[i]);
        remove_range_size = remove_range_size + *engine_size;
    }
    remove_range_size = remove_range_size * 2;

    /** get interface configuration for this database */
    rv = bcm_stat_counter_interface_get(unit, flags, &database, &interface);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_interface_get\n");
        return rv;
    }
    /** hold the new end point of the data base range */
    *actual_nof_counters = (interface.type_config[type_id].end + 1 - remove_range_size);

    /** reduce the range of the last engine of the
     *  current data base  */
    interface.type_config[type_id].end = *actual_nof_counters - 1;
    /** this flag will allow the user to extend the range while the engine is enabled */
    flags = BCM_STAT_COUNTER_MODIFY_RANGE_ONLY;
    rv = bcm_stat_counter_interface_set(unit, flags, &database, &interface);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_interface_set\n");
        return rv;
    }

    return BCM_E_NONE;
}

/**
* \brief 
*   Once finished with the database, destroy the database in
*   order to free the engines and the database_id
*  
*   Destroy the created CRPS OAM database with the given core id
*   and database_id
* \par DIRECT INPUT: 
*   \param [in] unit            -  UNIT_ID
*   \param [in] core            -  CORE_ID, BCM_CORE_ALL is 
*          supported
*   \param [in] database_id     -  DATABASE_ID
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
crps_oam_database_destroy(
    int unit,
    int core_id,
    int database_id)
{
    int flags = 0, rv;
    bcm_stat_counter_enable_t disable_config;
    bcm_stat_counter_database_t database;
    bcm_stat_counter_enable_t_init(&disable_config);
    bcm_stat_counter_database_t_init(&database);
    
    database.database_id = database_id;
    database.core_id = core_id;
    
    /** Disable the database */
    disable_config.enable_counting = FALSE;
    disable_config.enable_eviction = FALSE;
    rv = bcm_stat_counter_database_enable_set(unit, flags, &database, &disable_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_database_enable_set\n");
        return rv;
    }
    /** Destroy the database */
    rv = bcm_stat_counter_database_destroy(unit, flags, &database);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_database_destroy \n");
        return rv;
    }
    
    return BCM_E_NONE;        
}

/**
 * \brief
 *   example of crps databases connected to OAM source 
 *   3 different databases are created and then destroyed
 *   databases description:
 *   IngressOam; command_id = 7; engine_ids: 3,5,11,14;
 *   database_id = 0; counter_base = 1K
 *   EgressOam; command_id = 2; engine_ids: 0,20; database_id =
 *   1; counter_base = 2K
 *   Oamp; command_id = 0; engine_ids: 2,9,19; database_id = 2;
 *   counter_base = 0
 * \param [in] unit -  unit id
 *  \param [in] core_id - core_id, BCM_CORE_ALL supported
 * \return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
int
crps_oam_database_example(
    int unit,
    int core_id)
{
    uint32 max_nof_engines = *(dnxc_data_get(unit, "crps", "engine", "nof_engines", NULL));
    uint32 nof_big_engines = *(dnxc_data_get(unit, "crps", "engine", "nof_big_engines", NULL));
    uint32 nof_mid_engines = *(dnxc_data_get(unit, "crps", "engine", "nof_mid_engines", NULL));
    uint32 nof_small_engines = max_nof_engines - nof_big_engines - nof_mid_engines;

    int nof_engines[3] = {3,2,3};
    int ingress_oam_engines[3], ingress_oam_engines_add[2], ingress_oam_engine_remove;
    int egress_oam_engines[2], egress_oam_engines_add[2], egress_oam_engine_remove;
    int oamp_engines[3] = {2,9,19}, oamp_engines_add[2] = {12,13}, oamp_engine_remove = 13;
    int *engine_array[3] = {ingress_oam_engines, egress_oam_engines, oamp_engines};
    int *new_engines[3] = {ingress_oam_engines_add, egress_oam_engines_add, oamp_engines_add};
    int remove_engine[3] = {ingress_oam_engine_remove, egress_oam_engine_remove, oamp_engine_remove};
    int database_id[3] = {0,1,2};
    int command_id[3] = {7,2,0};
    int counter_base[3] = {65536,2048,0};
    bcm_stat_counter_interface_type_t oam_source[3] = {bcmStatCounterInterfaceIngressOam, bcmStatCounterInterfaceEgressOam, bcmStatCounterInterfaceOamp};
    uint32 total_nof_counters_get;
    uint32 total_nof_counters_to_compare;
    int rv,i,j;
    uint32 *engine_size;
    int nof_new_engines = 2, nof_engines_to_remove = 1;

    ingress_oam_engines[0] = max_nof_engines - 1; /** last big engine id */
    ingress_oam_engines[1] = max_nof_engines - 2; /** big engine id */
    ingress_oam_engines[2] = nof_small_engines -1; /** last small engine id */
    ingress_oam_engines_add[0] = nof_small_engines - 2; /** small engine id */
    ingress_oam_engines_add[1] =nof_small_engines + nof_mid_engines ; /** first big engine id */
    ingress_oam_engine_remove = ingress_oam_engines_add[1];

    egress_oam_engines[0] = 0;
    egress_oam_engines[1] = nof_small_engines + nof_mid_engines - 1; /** last middle engine id */
    egress_oam_engines_add[0] = nof_small_engines + nof_mid_engines - 2; /** middle engine id */
    egress_oam_engines_add[1] = 1;
    egress_oam_engine_remove = egress_oam_engines_add[1];

    oamp_engines[0] = nof_small_engines; /** first middle engine */
    oamp_engines[1] = nof_small_engines+1; /** middle engine */
    oamp_engines[2] = nof_small_engines + nof_mid_engines + 1; /** second big engine */
    oamp_engines_add[0] = nof_small_engines+2;
    oamp_engines_add[1] = nof_small_engines+3;
    oamp_engine_remove = oamp_engines_add[1];
 
    for (i = 0; i < 3; i++)
    {
        total_nof_counters_to_compare = 0;
        /** create OAM CRPS database*/
        rv = crps_oam_database_set(unit, core_id, oam_source[i], command_id[i], nof_engines[i], *(engine_array[i]), counter_base[i], database_id[i], &total_nof_counters_get);
        if (rv != BCM_E_NONE)
        {
            printf("Error in crps_oam_database_set \n");
            return rv;
        }

        /** calculate the size of the engines in double entry; done
         *  only for comparison */
        for (j = 0; j < nof_engines[i]; j++)
        {
             engine_size = dnxc_data_1d_get(unit, "crps", "engine", "engines_info", "nof_counters", engine_array[i][j]);
             total_nof_counters_to_compare = total_nof_counters_to_compare + *engine_size;
        }
        total_nof_counters_to_compare = total_nof_counters_to_compare * 2; /** since in double entry mode*/
        if (total_nof_counters_get != total_nof_counters_to_compare)
        {
            printf("Error in crps_oam_database_set, wrong actual_nof_counters=%d received, expected=%d \n",total_nof_counters_get,total_nof_counters_to_compare);
            return rv;
        }

        /** add two engines to the database */
        rv = crps_oam_database_engines_add(unit, core_id, database_id[i], nof_new_engines, *(new_engines[i]), &total_nof_counters_get);
        if (rv != BCM_E_NONE)
        {
            printf("Error in crps_oam_database_engines_add \n");
            return rv;
        }
        /** calculate the size of the engines in double entry; done
         *  only for comparison */
        for (j = 0; j < nof_new_engines; j++)
        {
             engine_size = dnxc_data_1d_get(unit, "crps", "engine", "engines_info", "nof_counters", new_engines[i][j]);
             total_nof_counters_to_compare = total_nof_counters_to_compare + (*engine_size) * 2; /** since in double entry mode*/
        }
        if (total_nof_counters_get != total_nof_counters_to_compare)
        {
            printf("Error in crps_oam_database_engines_add, wrong actual_nof_counters=%d received, expected=%d \n",total_nof_counters_get,total_nof_counters_to_compare);
            return rv;
        }

        /** remove one engine from the database */
        rv = crps_oam_database_engines_remove(unit, core_id, database_id[i], nof_engines_to_remove, remove_engine[i], &total_nof_counters_get);
        if (rv != BCM_E_NONE)
        {
            printf("Error in crps_oam_database_engines_remove \n");
            return rv;
        }
        engine_size = dnxc_data_1d_get(unit, "crps", "engine", "engines_info", "nof_counters", remove_engine[i]);
        total_nof_counters_to_compare = total_nof_counters_to_compare - (*engine_size) * 2;
        if (total_nof_counters_get != total_nof_counters_to_compare)
        {
            printf("Error in crps_oam_database_engines_remove, wrong actual_nof_counters=%d received, expected=%d \n",total_nof_counters_get,total_nof_counters_to_compare);
            return rv;
        }
    }
   
    /** Created 3 OAM databases - now destroy them */
    for (i = 0; i < 3; i++)
    {
        rv = crps_oam_database_destroy(unit, core_id, database_id[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error in crps_oam_database_destroy \n");
            return rv;
        }
    }
   
    return BCM_E_NONE;
}

