/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_crps_erpp_config.c
 * related files: cint_crps_db_utils.c
 * Purpose:     Example of configuring a statistic database of erpp using the counter processor.
 *                    the data base is built from one or more object types 
 *                     (example: egress_vsi/egress_pmf. the crps do not aware what the type represent).
 * 
 * The cint include:
 *  - configure a statistic data base for erpp, one configuration for all engines that belong to the data base.
 *  - configure specific parameters per engine (engine id,
 *    engine counters range etc) and enable the database
 */

int EXPANSION_TYPE_NO_EXPANSION_SET_1 = -1; /* No expansion. All packets goes to one counter. counter_set_size = 1 */

int EXPANSION_TYPE_PP_DROP_REASON_AND_TM_IS_MC_SET_2 = 0;       /* expansion is built from pp-drop-reason and tm-is-mc. 
                                                                 * uc- offset 0, mc -offset1. and filter according to
                                                                 * pp-drop */
int EXPANSION_TYPE_PP_DROP_REASON_AND_TM_ACTION_TYPE_SET_1 = 1; /* expansion is built from pp-drop and tm-action-type.
                                                                 * only one type will be counted which will be filtered
                                                                 * for specific pp-drop values */

int EXPANSION_TYPE_META_DATA_SET_16 = 2;        /* expansion is built from 4 bits of meta data (represent
                                                 * erpp-forward-context). counter_set=16 */
int DATA_MAPPING_ARRAY_MAX = 16;        /* defines the max number of entries for data mapping table for all examples in 
                                         * this file */

enum crps_erpp_config_database_t {
    CRPS_ERPP_CONFIG_CREATE_SET_2 = 0, /* create ERPP with set 2*/
    CRPS_ERPP_CONFIG_CREATE_SET_1 = 1, /* create ERPP with set 1*/
    CRPS_ERPP_CONFIG_CREATE_SET_16 = 2, /* create ERPP with set 16*/
    CRPS_ERPP_CONFIG_CREATE_ALL = 3  /* create the 3 databases*/
};
/**
* \brief  
*   expansion builder (selection and counter set mapping). example to few cases
* \par DIRECT INPUT: 
*   \param [in] unit              -  
*   \param [in] type_flag         -  choose the type of expansion that the function will implement. 
*                                                    (according to the options defined in the begginning of this file).
*   \param [out] *expansion_select  -  pointer to the strucute that the function will  update.
*   \param [out] *counter_set_map   -pointer to the strucute that the function will  update.  
*   \param [out] *counter_set_size   -returned counter_set_size
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
crps_erpp_expansion_builder(
    int unit,
    int type_flag,
    bcm_stat_expansion_select_t * expansion_select,
    bcm_stat_counter_set_map_t * counter_set_map,
    int *counter_set_size)
{
    int rv, drop_reason, mc_valid;
    int i;

    if (type_flag == EXPANSION_TYPE_NO_EXPANSION_SET_1)
    {
        /** expansion selection strucutre. */
        expansion_select->nof_elements = 0;
        /** counter set mappping strucutre. */
        *counter_set_size = 1;
        counter_set_map->nof_entries = 1;
        counter_set_map->expansion_data_mapping[0].nof_key_conditions = 0;
        counter_set_map->expansion_data_mapping[0].value.counter_set_offset = 0;
        counter_set_map->expansion_data_mapping[0].value.valid = TRUE;
    }
    else if (type_flag == EXPANSION_TYPE_PP_DROP_REASON_AND_TM_IS_MC_SET_2)
    {
        /** expansion selection strucutre. */
        expansion_select->nof_elements = 2;
        expansion_select->expansion_elements[0].type = bcmStatExpansionTypePpDropReason;
        expansion_select->expansion_elements[1].type = bcmStatExpansionTypeSystemMultiCast;

        /** counter set mappping strucutre. */
        *counter_set_size = 2;

        /** first, configure full distribution according to multicast field only */
        counter_set_map->nof_entries = 0;
        for (mc_valid = 0; mc_valid < 2; mc_valid++)
        {
            counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].nof_key_conditions = 1;
            counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].key[0].type =
                bcmStatExpansionTypeSystemMultiCast;
            counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].key[0].value = mc_valid;
            counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].value.counter_set_offset = mc_valid;
            counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].value.valid = TRUE;
            counter_set_map->nof_entries++;
        }
        /** now, filter some drop reasons that are not required for mc. for example: drop-reasons: 2 and 3 */
        for (drop_reason = 2; drop_reason <= 3; drop_reason++)
        {
            counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].nof_key_conditions = 2;
            counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].key[0].type =
                bcmStatExpansionTypeSystemMultiCast;
            counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].key[0].value = 1;
            counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].key[1].type =
                bcmStatExpansionTypePpDropReason;
            counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].key[1].value = drop_reason;
            counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].value.valid = FALSE;
            counter_set_map->nof_entries++;
        }
    }
    else if (type_flag == EXPANSION_TYPE_PP_DROP_REASON_AND_TM_ACTION_TYPE_SET_1)
    {
        /** expansion selection strucutre. */
        expansion_select->nof_elements = 2;
        expansion_select->expansion_elements[0].type = bcmStatExpansionTypePpDropReason;
        expansion_select->expansion_elements[1].type = bcmStatExpansionTypeEgressTmActionType;

        /** counter set mappping strucutre. */
        *counter_set_size = 1;
        /** choose only one actionType to be counted (for example:0), and filter drop-reason=0 */
        counter_set_map->nof_entries = 0;
        counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].nof_key_conditions = 2;
        counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].key[0].type =
            bcmStatExpansionTypeEgressTmActionType;
        counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].key[0].value = 0;
        counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].key[1].type =
            bcmStatExpansionTypePpDropReason;
        counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].key[1].value = 0;
        counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].value.valid = TRUE;
        counter_set_map->nof_entries++;
    }
    else if (type_flag == EXPANSION_TYPE_META_DATA_SET_16)
    {
        /** expansion selection strucutre. */
        expansion_select->nof_elements = 1;
        expansion_select->expansion_elements[0].type = bcmStatExpansionTypeMetaData;
        expansion_select->expansion_elements[0].bitmap = 0xF;   /* select all 4 bits */
        /** counter set mappping strucutre. */
        *counter_set_size = 16;

        /** configure full distribution according to meta data field only */
        counter_set_map->nof_entries = 16;
        for (i = 0; i < counter_set_map->nof_entries; i++)
        {
            counter_set_map->expansion_data_mapping[i].nof_key_conditions = 1;
            counter_set_map->expansion_data_mapping[i].key[0].type = bcmStatExpansionTypeMetaData;
            counter_set_map->expansion_data_mapping[i].key[0].value = i;
            counter_set_map->expansion_data_mapping[i].value.counter_set_offset = i;
            counter_set_map->expansion_data_mapping[i].value.valid = TRUE;
        }
    }
    else
    {
        return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;

}

/**
* \brief  
*   configure an erpp data base and enable it's counting and eviction.
*   the format in this example is double packet mode. {34'b pkt_ctr, 34'b pkt_ctr}
* \par DIRECT INPUT: 
*   \param [in] unit                                 -  
*   \param [in] core                                 -  
*   \param [in] database_id   -  id for the database that will may be used when reading a counter
*   \param [in] command_id                           -  
*   \param [in] nof_types                            -  
*   \param [in] start_object_stat_id - start range array per type
*   \param [in] end_object_stat_id  -  end range array per type
*   \param [in] nof_engines    
*   \param [in] engine_id_arr  
*   \param [in] expansion_type_flag - choose the type of expansion that the function will implement. 
*                                                    (according to the options defined in the begginning of this file).
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * Before calling this function, user need to configure also the validity and offset for each engine and type in the function:
*      crps_type_config_set. (file: cint_crps_db_utils.c)
*
* \see
*   * None
*/
int
crps_erpp_data_base_create(
    int unit,
    int core,
    int database_id,
    int command_id,
    int nof_types,
    int *start_object_stat_id,
    int *end_object_stat_id,
    int *type_valid,
    uint32 *type_offset,
    int nof_engines,
    int *engine_id_arr,
    int expansion_type_flag)
{
    int rv, i, type_id;
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
    bcm_stat_counter_interface_key_t interface_key;
    bcm_stat_expansion_select_t expansion_select;
    bcm_stat_counter_engine_control_t control = bcmStatCounterClearAll;
    bcm_stat_expansion_data_mapping_t data_mapping_array[DATA_MAPPING_ARRAY_MAX];
    int counter_set_size;
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
    bcm_stat_expansion_data_mapping_t_init(&data_mapping_array);
    bcm_stat_counter_interface_key_t_init(&interface_key);
    bcm_stat_expansion_select_t_init(&expansion_select);
    bcm_stat_counter_interface_t_init(&cnt_interface);
    bcm_stat_counter_enable_t_init(&enable_config);

    /** configure the expansion selection and counter set mapping */
    ctr_set_map.expansion_data_mapping = data_mapping_array;
    rv = crps_erpp_expansion_builder(unit, expansion_type_flag, &expansion_select, &ctr_set_map, &counter_set_size);
    if (rv != BCM_E_NONE)
    {
        printf("Error in crps_erpp_expansion_builder\n");
        return rv;
    }

    /** step 2: select the expansion bits format for each interface, using API bcm_stat_counter_expansion_select_set */
    /** set the interface key. (the selection structure was already feel in function "crps_erpp_expansion_builder" ) */
    interface_key.core_id = core;
    interface_key.command_id = command_id;
    interface_key.interface_source = bcmStatCounterInterfaceEgressReceivePp;

    /** each type requires expansion selection. */
    for (i = 0; i < nof_types; i++)
    {
        interface_key.type_id = i;
        rv = bcm_stat_counter_expansion_select_set(unit, 0, &interface_key, &expansion_select);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_stat_counter_expansion_select_set\n");
            return rv;
        }
    }

   /** step 3: configure and create database - use user selected
     *  database_id */
    database.database_id = database_id;
    database.core_id = core;
    rv = bcm_stat_counter_database_create(unit, BCM_STAT_DATABASE_CREATE_WITH_ID, core, database_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_database_create\n");
        return rv;
    }

    /** configure the structure for API bcm_stat_counter_eviction_set */
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
    /** calling the APIs per engine, in the right order - attach
     *  engines to the database and select eviction */
    for (i = 0; i < nof_engines; i++)
    {
        engine.core_id = core;
        engine.engine_id = engine_id_arr[i];

        printf("*********** set API: engine_id[%d], core=%d **************\n\n", engine.engine_id, engine.core_id);

        /** step 4: attach the engines to the database */
        rv = bcm_stat_counter_engine_attach(unit, 0, &database, engine_id_arr[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_stat_counter_engine_attach\n");
            return rv;
        }
        /** step 5: configure the counter eviction per engine */
        rv = bcm_stat_counter_eviction_set(unit, eviction_flags, &engine, &eviction);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_stat_counter_eviction_set\n");
            return rv;
        }
    }
    /** step 6: configure the counter interface per database */
    cnt_interface.source = bcmStatCounterInterfaceEgressReceivePp;
    cnt_interface.command_id = command_id;
    cnt_interface.format_type = bcmStatCounterFormatDoublePackets;
    cnt_interface.counter_set_size = counter_set_size;
    for (type_id = 0; type_id < nof_types; type_id++)
    {
        /** get type validity and offset from prior configuration */
        cnt_interface.type_config[type_id].valid = type_valid[type_id];
        cnt_interface.type_config[type_id].object_type_offset = type_offset[type_id];
        cnt_interface.type_config[type_id].start = start_object_stat_id[type_id];
        cnt_interface.type_config[type_id].end = end_object_stat_id[type_id];
    }
    rv = bcm_stat_counter_interface_set(unit, interface_flags, &database, &cnt_interface);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_interface_set\n");
        return rv;
    }

    /** step 7: configure the counter set mapping per database */
    rv = bcm_stat_counter_set_mapping_set(unit, mapping_flags, &database, &ctr_set_map);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_set_mapping_set\n");
        return rv;
    }

     /** step 8: enable the database */
    enable_config.enable_counting = TRUE;
    enable_config.enable_eviction = TRUE;
    rv = bcm_stat_counter_database_enable_set(unit, 0, &database, &enable_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_database_enable_set\n");
        return rv;
    }

    /** print the data base 
    rv = crps_data_base_print(unit, &interface_key, &database, engine_id_arr[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error in crps_data_base_print\n");
        return rv;
    }*/
    return BCM_E_NONE;
}

/**
 * \brief
 *   example of crps database created on selected core,
 *   connected to erpp and print the database to screen.
 *   database description: (parameters value are arbitrary
 *   example.)
 *       For each core, one engine connected to src erpp, command_id=0. configured with 3 types, 1 engine, double_entry mode
 * \param [in] unit -
 *     unit id
 * \param [in] database_id - database 
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
crps_erpp_database_example(
    int unit,
    int database_id,
    int core_id,
    crps_erpp_config_database_t create_db)
{
    uint32 max_nof_engines = *(dnxc_data_get(unit, "crps", "engine", "nof_engines", NULL));
    uint32 nof_big_engines = *(dnxc_data_get(unit, "crps", "engine", "nof_big_engines", NULL));
    int rv;
    int index, type_id;
    int nof_engines = 2;
    int engine_arr[2];
    int nof_types = 3;
    int type_valid[3] = { 1, 0, 0 };
    uint32 type_offset[3];
    int start_object_stat_id_arr[3];
    int end_object_stat_id_arr[3];
    int command_id = 1;
    uint32 nof_counters;
    uint32 *engine_size;
    int expansion_flag[3] = { EXPANSION_TYPE_PP_DROP_REASON_AND_TM_IS_MC_SET_2,
        EXPANSION_TYPE_PP_DROP_REASON_AND_TM_ACTION_TYPE_SET_1,
        EXPANSION_TYPE_META_DATA_SET_16
    };
    int counter_set_size[3] = { 2, 1, 16 };     /* match to the expansion flag array */

    engine_arr[0] = max_nof_engines - nof_big_engines;
    engine_arr[1] = engine_arr[0] - 1;
    engine_size = dnxc_data_1d_get(unit, "crps", "engine", "engines_info", "nof_counters", engine_arr[0]);
    nof_counters = *engine_size;
    engine_size = dnxc_data_1d_get(unit, "crps", "engine", "engines_info", "nof_counters", engine_arr[1]);
    nof_counters = nof_counters + *engine_size;
    nof_counters = (nof_counters) * 2;  /* because the mode is duble entry */

    /** loop on all of the expansion flags. create database for each one of them */
    for (index = 0; index < 3; index++)
    {
        if (create_db != CRPS_ERPP_CONFIG_CREATE_ALL) {
            index = create_db;
        }
        /** configure type 0 */
        start_object_stat_id_arr[0] = 20;
        end_object_stat_id_arr[0] = (nof_counters / counter_set_size[index]);
        type_offset[0] = 0;
        rv = crps_erpp_data_base_create(unit, core_id, database_id, command_id, nof_types,
                                        start_object_stat_id_arr, end_object_stat_id_arr, type_valid, type_offset,
                                        nof_engines, engine_arr, expansion_flag[index]);
        if (rv != BCM_E_NONE)
        {
            printf("Error in crps_erpp_data_base_create \n");
            return rv;
        }

        if (create_db != CRPS_ERPP_CONFIG_CREATE_ALL) {
            break;
        }
    }
    return BCM_E_NONE;      
}
    
/**
* \brief
*   wrapper for crps_erpp_data_base_create function which create a crps database.
*   this wrapper produce a single engine database, with a single type.
*/
int
crps_erpp_data_base_create_wrapper(
    int unit,
    int core,
    int database_id,
    int command_id,
    int type_id,
    int type_start,
    int type_end,
    int engine_id,
    int expansion_type_flag)
{
    int rv;
    int nof_types = 4;
    int type_valid_arr[4] = { 0, 0, 0, 0 };
    uint32 type_offset_arr[4] = { 0, 0, 0, 0 };
    int type_start_arr[4] = { 0, 0, 0, 0 };
    int type_end_arr[4] = { 0, 0, 0, 0 };
    uint32 nof_cores_tmp = *(dnxc_data_get(unit, "device", "general", "nof_cores", NULL));
    int nof_cores = nof_cores_tmp;

    if (core >= nof_cores) {
        printf("Core ID doesn't exist, skip it\n");
        return BCM_E_NONE;
    }
 
    if (type_id > 3 || type_id < 0)
    {
        printf("invalid type_id=%d \n", type_id);
        return BCM_E_PARAM;
    }
 
    type_valid_arr[type_id] = 1;
    type_start_arr[type_id] = type_start;
    type_end_arr[type_id] = type_end;
 
    rv = crps_erpp_data_base_create(unit, core, database_id, command_id, nof_types, type_start_arr, type_end_arr, type_valid_arr, type_offset_arr, 1,   /* nof_engines 
                                                                                                                                                         */
                                    &engine_id, expansion_type_flag);
 
    if (rv != BCM_E_NONE)
    {
        printf("Error in crps_erpp_data_base_create \n");
        return rv;
    }
 
   return BCM_E_NONE;
}
