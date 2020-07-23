/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_crps_etpp_config.c
 * related files: cint_crps_db_utils.c 
 * Purpose:     Example of configuring a statistic database of etpp using the counter processor.
 *                    the data base is built from one or more object types 
 * 
 * The cint include:
 *  - configure a statistic data base for etpp, one configuration for all engines that belong to the data base.
 *  - configure specific parameters per engine (engine id,
 *    engine counters range etc) and enable the database
 */

int EXPANSION_TYPE_NO_EXPANSION_SET_1 = 1; /* No expansion. All packets goes to one counter. counter_set_size = 1 */

int EXPANSION_TYPE_META_DATA_FULL_4_BITS = 16; /* expansion is built from 4 bits of metadata, spread to 16 offsets*/

int EXPANSION_TYPE_META_DATA_SET_8 = 8; /* expansion is built from 3 bits of meta data that represent the right
                                         * object-id.  counter_set=8 */
int EXPANSION_TYPE_PORT_META_DATA_VARIABLE_SET_3 = 3;   /* expansion is built from 3 bits of port-meta-data. offset0
                                                         * for values0-63, offset 1 for 64-127. offset 2 for > 128 */
int DATA_MAPPING_ARRAY_MAX = 130;       /* defines the max number of entries for data mapping table for all examples in 
                                         * this file */
/*
 * counter_format_sel: use to select the counter format
 *      0: bcmStatCounterFormatPackets
 *      1: bcmStatCounterFormatBytes
 *      2: bcmStatCounterFormatPacketsAndBytes
 *      default select bcmStatCounterFormatPackets
 */
int counter_format_sel=0;

enum crps_etpp_config_database_t {
    CRPS_ETPP_CONFIG_CREATE_SET_3 = 0, /* create ETPP with set 3*/
    CRPS_ETPP_CONFIG_CREATE_SET_8 = 1, /* create ETPP with set 8*/
    CRPS_ETPP_CONFIG_CREATE_SET_16 = 2, /* create ETPP with set 16*/
    CRPS_ETPP_CONFIG_CREATE_SET_1 = 3
};
/**
* \brief  
*   expansion builder (selection and counter set mapping). example to few cases
* \par DIRECT INPUT: 
*   \param [in] unit              -  
*   \param [in] type_flag         -  choose the type of expansion that the function will implement. 
*                                                    (according to the options defined in the begginning of this file).
*   \param [in] command_id        -  
*   \param [in] *expansion_select  -  pointer to the strucute that the function will  update.
*   \param [in] *counter_set_map   -pointer to the strucute that the function will  update.  
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
crps_etpp_expansion_builder(
    int unit,
    int type_flag,
    int command_id,
    bcm_stat_expansion_select_t * expansion_select,
    bcm_stat_counter_set_map_t * counter_set_map,
    int * counter_set_size)
{
    int rv, i;

    /** memset is required only for the mapping strucutre */

    sal_memset(counter_set_map, 0x0, sizeof(counter_set_map));


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
    else if (type_flag == EXPANSION_TYPE_PORT_META_DATA_VARIABLE_SET_3)
    {
        /** expansion selection strucutre. */
        expansion_select->nof_elements = 2;
        expansion_select->expansion_elements[0].type = bcmStatExpansionTypePpDropReason;
        expansion_select->expansion_elements[1].type = bcmStatExpansionTypePortMetaData;
        expansion_select->expansion_elements[1].bitmap = 0xFF;

        /** counter set mappping strucutre. */
        * counter_set_size = 3;

        /** first, configure all distributions to offset 2 */
        counter_set_map->nof_entries = 0;
        counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].nof_key_conditions = 0;
        counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].value.counter_set_offset = 2;
        counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].value.valid = TRUE;
        counter_set_map->nof_entries++;

        /** second, configure portData indexes 0-63 and PpDropReason=1 to offset 0, 64-127 and PpDropReason=1 to offset 1 */
        for (i = 0; i < 128; i++)
        {
            counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].nof_key_conditions = 2;
            counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].key[0].type =
                bcmStatExpansionTypePortMetaData;
            counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].key[1].type =
                bcmStatExpansionTypePpDropReason;                
            counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].key[0].value = i;
            counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].key[1].value = 0;
            counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].value.counter_set_offset = i / 64;
            counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].value.valid = TRUE;
            counter_set_map->nof_entries++;
        }
    }
    else if (type_flag == EXPANSION_TYPE_META_DATA_SET_8)
    {
        /** expansion selection strucutre. */
        expansion_select->nof_elements = 1;
        expansion_select->expansion_elements[0].type = bcmStatExpansionTypeMetaData;
        /** etpp metadata is is 4 bits per object_id (command_id) */
        expansion_select->expansion_elements[0].bitmap = 0xb;/* select 3 bits  not consecutive */

        /** counter set mappping strucutre. */
        * counter_set_size = 8;

        /** configure full distribution according to meta data field only */
        counter_set_map->nof_entries = 8;
        for (i = 0; i < counter_set_map->nof_entries; i++)
        {
            counter_set_map->expansion_data_mapping[i].nof_key_conditions = 1;
            counter_set_map->expansion_data_mapping[i].key[0].type = bcmStatExpansionTypeMetaData;
            /** In the driver, the condition value will be masked by the bitmap. */
            /** for example: for bitmap = 0xb, value=0x7 will be consider for the data mapping calculation as 0x3 */
            counter_set_map->expansion_data_mapping[i].key[0].value = (((i & 0x4) << 1) | (i & 0x3)); 
            counter_set_map->expansion_data_mapping[i].value.counter_set_offset = i;
            counter_set_map->expansion_data_mapping[i].value.valid = TRUE;
        }
    }
    else if (type_flag == EXPANSION_TYPE_META_DATA_FULL_4_BITS)
    {
        /** expansion selection strucutre. */
        expansion_select->nof_elements = 1;
        expansion_select->expansion_elements[0].type = bcmStatExpansionTypeMetaData;
        /** etpp metadata is is 4 bits per object_id (command_id) */
        expansion_select->expansion_elements[0].bitmap = 0xF;/* select 4 bits - fill whole MetaData */
        /** counter set mappping strucutre. */
        * counter_set_size = 16;

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
*   configure an erpp data base and enable it's counting and eviction
*   the format in this example is wide packet mode. {68'b pkt_ctr}
* \par DIRECT INPUT: 
*   \param [in] unit                                 -  
*   \param [in] core                                 -  
*   \param [in] database_id   -  id for the database that will may be used when reading a counter
*   \param [in] command_id                           -  
*   \param [in] nof_types                            -  
*   \param [in] start_object_stat_id - start range array per type
*   \param [in] end_object_stat_id  -  end range array per type
*   \param [in] nof_engines    
*   \param [in] *engine_id_arr  
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
* \see
*   * None
*/
int
crps_etpp_data_base_create(
    int unit,
    int core,
    int database_id,
    int command_id,
    int nof_types,
    int * start_object_stat_id,
    int * end_object_stat_id,
    int * type_valid,
    int * type_offset,
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
    bcm_stat_counter_interface_key_t_init(&interface_key);
    bcm_stat_expansion_select_t_init(&expansion_select);
    bcm_stat_counter_interface_t_init(&cnt_interface);
    bcm_stat_counter_enable_t_init(&enable_config);

    /** configure the expansion selection and counter set mapping */
    ctr_set_map.expansion_data_mapping = data_mapping_array;
    rv = crps_etpp_expansion_builder(unit, expansion_type_flag, command_id, &expansion_select, &ctr_set_map, &counter_set_size);
    if (rv != BCM_E_NONE)
    {
        printf("Error in crps_etpp_expansion_builder\n");
        return rv;
    }

    /*
     * step 1: select the expansion bits format for each interface, using API bcm_stat_counter_expansion_select_set
     * set the interface key. (the selection structure was already feel in function "crps_erpp_expansion_builder" ) 
     */
    interface_key.core_id = core;
    interface_key.command_id = command_id;
    interface_key.interface_source = bcmStatCounterInterfaceEgressTransmitPp;

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

    /** step 2: configure and create database - use user selected
     *  database id */
    database.database_id = database_id;
    database.core_id = core;
    rv = bcm_stat_counter_database_create(unit, BCM_STAT_DATABASE_CREATE_WITH_ID, core, database_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_database_create\n");
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
    /** calling the APIs per engine - attach engines to the
     *  database and select eviction */
    for (i = 0; i < nof_engines; i++)
    {
        engine.core_id = core;
        engine.engine_id = engine_id_arr[i];

        printf("*********** set API: engine_id[%d], core=%d **************\n\n", engine.engine_id, engine.core_id);

        /** step 3:  attach the engines to the database */
        rv = bcm_stat_counter_engine_attach(unit, 0, &database, engine_id_arr[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_stat_counter_engine_attach\n");
            return rv;
        }

       /** step 4: configure the counter eviction per engine */
        rv = bcm_stat_counter_eviction_set(unit, eviction_flags, &engine, &eviction);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_stat_counter_eviction_set\n");
            return rv;
        }
    }

     /*
     * configure the structure for API bcm_stat_counter_interface_set
     */
    cnt_interface.source = bcmStatCounterInterfaceEgressTransmitPp;
    cnt_interface.command_id = command_id;
    if (counter_format_sel == 1) 
    {
        cnt_interface.format_type = bcmStatCounterFormatBytes;
    }
    else if (counter_format_sel == 2)
    {
        cnt_interface.format_type = bcmStatCounterFormatPacketsAndBytes;
    }
    else
    {
        cnt_interface.format_type = bcmStatCounterFormatPackets;
    }
    cnt_interface.counter_set_size = counter_set_size;
    /** step 5: configure the counter interface per database  */
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

    /** step 6: configure the counter set mapping per database */
    rv = bcm_stat_counter_set_mapping_set(unit, mapping_flags, &database, &ctr_set_map);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_set_mapping_set\n");
        return rv;
    }

    /** step 7: enable the database */
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
    } 
    */ 
    return BCM_E_NONE;

}


/**
 * \brief
 *   example of crps database with selected core_id, connected
 *   to itpp and print the database to screen. database
 *   description: (parameters value are arbitrary example.)
 *       For each core, 2 engines (different size) connected to src etpp, command_id=1. 
 *       configured with 2 types, each type in different engine
 * \param [in] unit -
 *     unit id
 *     \param [in] database_id - database_id
 *     \param [in] core_id - core_id
 *     \param [in] command_id - command_id
 *     \param [in] create_db - select test case scenario
 * \return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
int
crps_etpp_database_example(
    int unit,
    int database_id,
    int core_id,
    int command_id,
    crps_etpp_config_database_t create_db)
{
    int rv;
    int type_id, index, engine_idx;
    int nof_engines = 2;
    int engine_arr[2];
    int nof_types = 2;
    int type_valid[2] = {1,1};
    uint32* engine_size;
    int type_offset[2] = {0,0};
    int start_object_stat_id_arr[2] = {0,0};
    int end_object_stat_id_arr[2];
    int expansion_flag[4] = {EXPANSION_TYPE_PORT_META_DATA_VARIABLE_SET_3, EXPANSION_TYPE_META_DATA_SET_8, EXPANSION_TYPE_META_DATA_FULL_4_BITS, EXPANSION_TYPE_NO_EXPANSION_SET_1};
    int counter_set_size[4] = {3,8,4,1}; /* match to the expansion flag array */
    
    engine_arr[0] = 0;
    engine_arr[1] = 2; 
    /** each type range holds 1 engine full entries */
    /** offset will be the prev offset + the end point of the prev type */            
    /** configure type 0 */
    start_object_stat_id_arr[0] = 0;
    engine_size = dnxc_data_1d_get(unit, "crps", "engine", "engines_info", "nof_counters", engine_arr[0]);
    end_object_stat_id_arr[0] = (*engine_size)/counter_set_size[create_db] - 1;
    type_offset[0] = 0;
    /** configure type 1 */
    start_object_stat_id_arr[1] = 0;
    engine_size = dnxc_data_1d_get(unit, "crps", "engine", "engines_info", "nof_counters", engine_arr[1]);            
    end_object_stat_id_arr[1] = (*engine_size)/counter_set_size[create_db] - 1;
    type_offset[1] = end_object_stat_id_arr[0] - start_object_stat_id_arr[0] + 1 + type_offset[0];

    rv = crps_etpp_data_base_create(unit, core_id, database_id, command_id, nof_types, 
        start_object_stat_id_arr, end_object_stat_id_arr, type_valid, type_offset, nof_engines, engine_arr, 
        expansion_flag[create_db]);
    if (rv != BCM_E_NONE)
    {
        printf("Error in crps_etpp_data_base_create \n");
        return rv;
    }
        

    return BCM_E_NONE;      
}

/**
* \brief
*   wrapper for crps_etpp_data_base_create function which create a crps database.
*   this wrapper produce a single engine database, with a single type.
*/
int
crps_etpp_data_base_create_wrapper(
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
    int type_offset_arr[4] = { 0, 0, 0, 0 };
    int type_start_arr[4] = { 0, 0, 0, 0 };
    int type_end_arr[4] = { 0, 0, 0, 0 };
    uint32 nof_cores_tmp = *(dnxc_data_get(unit, "device", "general", "nof_cores", NULL)); 
    int nof_cores = nof_cores_tmp;

    if (core >= nof_cores) {
        printf("Core ID doesn't exist, skip it! core_ID[%d]- nof_core[%d]\n", core, nof_cores);
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
 
    rv = crps_etpp_data_base_create(unit, core, database_id, command_id, nof_types, type_start_arr, type_end_arr, type_valid_arr, type_offset_arr, 1,   /* nof_engines 
                                                                                                                                                         */
                                    &engine_id, expansion_type_flag);
 
    if (rv != BCM_E_NONE)
    {
        printf("Error in crps_etpp_data_base_create \n");
        return rv;
    }
 
    return BCM_E_NONE;
}


/* necessary for crps_etpp_ace_stat_meter_config cleanup */
bcm_field_ace_format_t g_ace_format_id = 50;
bcm_field_entry_info_t ent_info;

/**
 * add field group with vpn qualify with one action
 * attach group context
 */
int
crps_etpp_pmf_group_create(int unit, int pmf_group_id)
{
    int i, rv;
    bcm_field_group_info_t fg_info_t;
    bcm_field_group_info_t_init(&fg_info_t);

    fg_info_t.stage = bcmFieldStageEgress;
    fg_info_t.fg_type = bcmFieldGroupTypeTcam;
    fg_info_t.nof_quals=1;
    fg_info_t.qual_types[0]=bcmFieldQualifyVpn;
    fg_info_t.nof_actions =1;
    fg_info_t.action_types[0] = bcmFieldActionAceEntryId;

    rv = bcm_field_group_add(unit, 5, &fg_info_t, &pmf_group_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    /**Attach context**/
    bcm_field_group_t cint_attach_fg_id=pmf_group_id;
    bcm_field_context_t cint_attach_context_id_eth=0;
    bcm_field_group_attach_info_t attach_info;

    bcm_field_qualify_attach_info_t qual_info2[1] = {
    {bcmFieldInputTypeMetaData,0,0},
    };

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info_t.nof_quals;
    for (i = 0; i < fg_info_t.nof_quals; i++)
    {
        attach_info.key_info.qual_types[i] = fg_info_t.qual_types[i];
        attach_info.key_info.qual_info[i].input_type = qual_info2[i].input_type;
        attach_info.key_info.qual_info[i].input_arg = qual_info2[i].input_arg;
        attach_info.key_info.qual_info[i].offset = qual_info2[i].offset;
    }

    attach_info.payload_info.nof_actions = fg_info_t.nof_actions;
    for (i = 0; i < fg_info_t.nof_actions; i++)
    {
        attach_info.payload_info.action_types[i] = fg_info_t.action_types[i];
        attach_info.payload_info.action_info[i].priority = BCM_FIELD_ACTION_PRIORITY(0,1);
    }

    rv = bcm_field_group_context_attach(unit, 0, cint_attach_fg_id, cint_attach_context_id_eth, &attach_info);
    if (rv != BCM_E_NONE)
    {

        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    return BCM_E_NONE;
}

/**
 * \brief
 * add ace format
 * add ace entry
 * create pmf group
 * create pp profile with statistic command for meter and crps
 */
int
crps_etpp_ace_stat_meter_config(int unit,
        int stat_object_id,
        int command_id,
        int vlan,
        int pmf_group_id,
        int *pp_field_entry_id,
        int *pp_profile_id){

    int rv, flags;
    int type_id, is_meter;
    int trap_id;
    uint32 qual_value;
    bcm_field_ace_format_info_t ace_format_info;
    bcm_field_ace_entry_info_t ace_entry_info;
    bcm_field_qualify_t qual_type;
    bcm_gport_t gport;
    bcm_stat_pp_profile_info_t egress_stat_pp_profile_info;
    int vpnid = vlan;
    uint32 g_ace_entry_handle = 0;

    /* add ace format */
    bcm_field_ace_format_info_t_init(&ace_format_info);
    ace_format_info.nof_actions = 2;
    g_ace_format_id = 50;
    ace_format_info.action_types[0] = bcmFieldActionStat0; /**meter*/
    ace_format_info.action_types[1] = bcmFieldActionStat1; /**crps*/
    flags = BCM_FIELD_ENTRY_CREATE_WITH_ID;

    rv = bcm_field_ace_format_add(unit, flags, &ace_format_info, &g_ace_format_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_ace_format_add\n", rv);
        return rv;
    }

    /* create pp profile */
    flags = 0;
    bcm_stat_pp_profile_info_t_init(&egress_stat_pp_profile_info);
    egress_stat_pp_profile_info.meter_command_id = command_id;
    egress_stat_pp_profile_info.is_meter_enable = 1;
    egress_stat_pp_profile_info.counter_command_id = command_id;

    rv = bcm_stat_pp_profile_create(unit, flags, bcmStatCounterInterfaceEgressTransmitPp,
                                    pp_profile_id, &egress_stat_pp_profile_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_stat_pp_profile_create\n", rv);
        return rv;
    }

    rv = crps_etpp_pmf_group_create(unit, pmf_group_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in crps_etpp_pmf_group_create \n", rv);
        return rv;
    }

    /* add auc entry */
    bcm_field_ace_entry_info_t_init(&ace_entry_info);
    ace_entry_info.nof_entry_actions = 2;
    flags = 0;
    /** add meter action info */
    ace_entry_info.entry_action[0].type = bcmFieldActionStat0;
    ace_entry_info.entry_action[0].value[0] = *pp_profile_id & 0x1F;
    ace_entry_info.entry_action[0].value[1] = stat_object_id;
    /** add crps action info */
    ace_entry_info.entry_action[1].type = bcmFieldActionStat1;
    ace_entry_info.entry_action[1].value[0] = *pp_profile_id & 0x1F;
    ace_entry_info.entry_action[1].value[1] = stat_object_id;
    ace_entry_info.entry_action[1].value[2] = 1;

    rv = bcm_field_ace_entry_add(unit, flags, g_ace_format_id, &ace_entry_info, &g_ace_entry_handle);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_ace_entry_add\n", rv);
        return rv;
    }

    /* add entry fields */
    qual_value = vpnid;
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;
    ent_info.nof_entry_quals = 0;
    ent_info.entry_qual[0].type = bcmFieldQualifyVpn;
    ent_info.entry_qual[0].value[0] = qual_value;
    ent_info.entry_qual[0].mask[0] = 0xFFFF;
    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = bcmFieldActionAceEntryId;
    ent_info.entry_action[0].value[0] = g_ace_entry_handle;

    rv = bcm_field_entry_add(unit, 0, pmf_group_id, &ent_info, pp_field_entry_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    return BCM_E_NONE;
}

/**
 * \brief
 *   example of counting and policing with ACE pointer
 * \param [in] unit - unit id
 *     \param [in] core_id - core_id
 *     \param [in] vlan - virtual lan id
 *     \param [in] pmf_group_id
 *     \param [out] pp_field_entry_id
 *     \param [out] pp_profile_id
 * \return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *  Note: function use service from cint cint_policer_generic_meter.c - please load it first
 *  Note: function use service from cint cint_policer_generic_meter.c - please load it first
 */
int
crps_etpp_policer_pmf_example(int unit,
                            int core,
                            int vlan,
                            int pmf_group_id,
                            int policer_engine_id,
                            int counter_engine_id,
                            int *pp_field_entry_id,
                            int *pp_profile_id)
{
    int rv;
    int stat_object_id = 0;
    int command_id = 0;

    rv = crps_etpp_ace_stat_meter_config(unit, stat_object_id, command_id, vlan, pmf_group_id, pp_field_entry_id, pp_profile_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in crps_etpp_ace_stat_meter_config\n", rv);
        return rv;
    }

    int policer_database_id = 0;
    int is_ingress = 0;
    int base_pointer = stat_object_id; /* meter index for one meter, represent the stat object id */
    int single_bucket_mode = 0;
    int expansion_enable = 0;
    int total_nof_meters = 1;
    int configure_meters = 1;

    /* create a policer database with one engine attached */
    /* create one meter */
    /* enable database */
    rv = policer_generic_meter_database_example(unit,
                                                core,
                                                is_ingress,
                                                policer_database_id,
                                                base_pointer,
                                                single_bucket_mode,
                                                expansion_enable,
                                                total_nof_meters,
                                                configure_meters,
                                                policer_engine_id);

    rv = policer_egress_drop_packet(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, policer_egress_drop_packet\n", rv);
        return rv;
    }

    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in policer_generic_meter_database_example\n", rv);
        return rv;
    }

    int crps_database_id = 0;
    int type_id = 0;
    int type_start = 0;
    int type_end = 1000;
    int expansion_type_flag = 1;

    /* create crps database with one engine attached */
    /* create a counter */
    /* enable database */
    rv = crps_etpp_data_base_create_wrapper(unit,
                                            core,
                                            crps_database_id,
                                            command_id,
                                            type_id,
                                            type_start,
                                            type_end,
                                            counter_engine_id,
                                            expansion_type_flag);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in crps_etpp_data_base_create_wrapper\n", rv);
        return rv;
    }
    return BCM_E_NONE;
}


/**
 * cleanup for "crps_etpp_ace_stat_meter_config
 */
int
crps_etpp_ace_stat_meter_delete(int unit, int pmf_group_id, int pp_field_entry_id, int pp_profile_id){

    int rv;

    rv = bcm_field_ace_format_delete(unit, g_ace_format_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_ace_format_delete\n", rv);
        return rv;
    }

    rv = bcm_stat_pp_profile_delete(unit, pp_profile_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_stat_pp_profile_delete\n", rv);
        return rv;
    }

    rv = bcm_field_entry_delete(unit, pmf_group_id, NULL, pp_field_entry_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete\n", rv);
        return rv;
    }

    rv = bcm_field_group_context_detach(unit,pmf_group_id,0);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach\n", rv);
        return rv;
    }

    rv = bcm_field_group_delete(unit, pmf_group_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete\n", rv);
        return rv;
    }

    return BCM_E_NONE;
}


