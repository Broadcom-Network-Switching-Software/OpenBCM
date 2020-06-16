/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_crps_irpp_config.c
 * related files: cint_crps_db_utils.c 
 * Purpose:     Example of configuring a statistic database of ingress PP using the counter processor.
 * 
 * The cint include:
 *  - configure a statistic data base for ingress pp (could be for LIF or VSi or others). 
 *      set one configuration for all engines that belong to the data base.
 *  - configure specific parameters per engine (engine id,
 *    engine counters range etc) and enable the database
 * related cints: 
 */

int EXPANSION_TYPE_NO_EXPANSION_SET_1 = 1;        /* No expansion. All packets goes to one counter. counter_set_size = 1  */

int EXPANSION_TYPE_META_DATA_10BITS_SET_24 = 24;        /* expansion according to 10 bits of meta data. will be spread
                                                         * cyclic * of 24 offsets */
int EXPANSION_TYPE_LEGACY_FINAL_DP_AND_DISPOSITION_SET_5 = 5;   /* like jericho full_color mode: {fwd grn, drop grn,
                                                                 * fwd not grn, drop yel, drop red} */
int EXPANSION_TYPE_FINAL_DP_AND_METER_0_DP_SET_32 = 32; /* full spread of 32 counter per final dp and meter0 value and
                                                         * * valid */
int EXPANSION_TYPE_DISPOSITION_AND_METADATA_PP_DROP_SET_2 = 2; /* expansion according to pp drop(via MetaData) and tm drop */
int DATA_MAPPING_ARRAY_MAX = 1024;      /* defines the max number of entries for data mapping table for all examples in 
                                         * this file */
enum crps_irpp_config_database_t {
    CRPS_IRPP_CONFIG_CREATE_SET_32 = 0, /* create IRPP with set 32*/
    CRPS_IRPP_CONFIG_CREATE_SET_24 = 1, /* create IRPP with set 24*/
    CRPS_IRPP_CONFIG_CREATE_SET_5 = 2, /* create IRPP with set 5*/
    CRPS_IRPP_CONFIG_CREATE_SET_2 = 3, /* create IRPP with set 2*/
    CRPS_IRPP_CONFIG_CREATE_ALL = 4  /* create the 3 databases*/
};

/**
* \brief  
*   expansion builder (selection and counter set mapping). example to few common cases
* \par DIRECT INPUT: 
*   \param [in] unit              -  
*   \param [in] type_flag         -  choose the type of expansion that the function will implement. 
*                                                    (according to the options defined in the begginning of this file).
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
crps_irpp_expansion_builder(
    int unit,
    int type_flag,
    bcm_stat_expansion_select_t * expansion_select,
    bcm_stat_counter_set_map_t * counter_set_map,
    int *counter_set_size)
{
    int rv, i;
    int meter_0_Valid, meter_0_val, final_meter_val;
    int reject, color;


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
    else if (type_flag == EXPANSION_TYPE_META_DATA_10BITS_SET_24)
    {
        /** expansion selection strucutre. */
        expansion_select->nof_elements = 1;
        expansion_select->expansion_elements[0].type = bcmStatExpansionTypeMetaData;
        expansion_select->expansion_elements[0].bitmap = 0x03FF;        /* example: select 10 bits from meta data. 10
                                                                         * lsb bits */

        /** counter set mappping strucutre. */
        *counter_set_size = 24;
        /** go over all values (1024) and map to different counter offset. The mapping is cyclic every 24 */
        counter_set_map->nof_entries = 1024;
        for (i = 0; i < counter_set_map->nof_entries; i++)
        {
            counter_set_map->expansion_data_mapping[i].nof_key_conditions = 1;
            counter_set_map->expansion_data_mapping[i].key[0].type = bcmStatExpansionTypeMetaData;
            counter_set_map->expansion_data_mapping[i].key[0].value = i;
            counter_set_map->expansion_data_mapping[i].value.counter_set_offset = i % 24;
            counter_set_map->expansion_data_mapping[i].value.valid = TRUE;
        }
    }
    else if (type_flag == EXPANSION_TYPE_FINAL_DP_AND_METER_0_DP_SET_32)
    {
        /** expansion selection strucutre. */
        expansion_select->nof_elements = 3;
        expansion_select->expansion_elements[0].type = bcmStatExpansionTypeDropPrecedenceMeter0Valid;
        expansion_select->expansion_elements[1].type = bcmStatExpansionTypeDropPrecedenceMeter0Value;
        expansion_select->expansion_elements[2].type = bcmStatExpansionTypeDropPrecedenceMeterResolved;

        /** counter set mappping strucutre. */
        *counter_set_size = 32;
        /** expansion is built from 5 bits. each value goes to different offset */
        counter_set_map->nof_entries = 0;
        for (meter_0_Valid = 0; meter_0_Valid < 2; meter_0_Valid++)
        {
            for (meter_0_val = 0; meter_0_val < 4; meter_0_val++)
            {
                for (final_meter_val = 0; final_meter_val < 4; final_meter_val++)
                {

                    counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].nof_key_conditions = 3;
                    counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].key[0].type =
                        bcmStatExpansionTypeDropPrecedenceMeter0Valid;
                    counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].key[0].value = meter_0_Valid;
                    counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].key[1].type =
                        bcmStatExpansionTypeDropPrecedenceMeter0Value;
                    counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].key[1].value = meter_0_val;
                    counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].key[2].type =
                        bcmStatExpansionTypeDropPrecedenceMeterResolved;
                    counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].key[2].value =
                        final_meter_val;
                    counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].value.counter_set_offset =
                        counter_set_map->nof_entries;
                    counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].value.valid = TRUE;
                    counter_set_map->nof_entries++;     /* should be 32 at the end */
                }
            }
        }

        /*
         * Note: instead of setting the entire structure as made above, user can use the flag
         * BCM_STAT_COUNTER_MAPPING_FULL_SPREAD in the bcm_stat_counter_ser_mapping_set API 
         */
    }
    /** example which comfigure jericho "full_color" mode */
    else if (type_flag == EXPANSION_TYPE_LEGACY_FINAL_DP_AND_DISPOSITION_SET_5)
    {
        bcm_color_t colors[4] = { bcmColorGreen, bcmColorYellow, bcmColorRed, bcmColorBlack };
        int full_color[2][4] = { {0, 2, 2, 2}, {1, 3, 4, -1} }; /* [disposition][color]=
                                                                 * {{fwd_grn,fwd_yel,fwd_red,fwd_blk},
                                                                 * {drp_grn,drp_yel,drp_red,drp_blk}} */

        /** expansion selection strucutre. */
        expansion_select->nof_elements = 2;
        expansion_select->expansion_elements[0].type = bcmStatExpansionTypeDispositionIsDrop;
        expansion_select->expansion_elements[1].type = bcmStatExpansionTypeDropPrecedenceMeterResolved;

        /** counter set mappping strucutre. */
        *counter_set_size = 5;
        /*
         * configure 8 entries in order to create the following counter_set mapping: {fwd grn, drop grn, fwd not grn,
         * drop yel, drop red}
         */
        counter_set_map->nof_entries = 0;
        for (color = bcmColorGreen; color <= bcmColorBlack; color++)
        {
            for (reject = 0; reject < 2; reject++)
            {
                counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].nof_key_conditions = 2;
                counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].key[0].type =
                    bcmStatExpansionTypeDispositionIsDrop;
                counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].key[0].value = reject;
                counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].key[1].type =
                    bcmStatExpansionTypeDropPrecedenceMeterResolved;
                counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].key[1].value = color;
                counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].value.counter_set_offset =
                    ((full_color[reject][color] == -1) ? 0 : full_color[reject][color]);
                counter_set_map->expansion_data_mapping[counter_set_map->nof_entries].value.valid =
                    (full_color[reject][color] == -1) ? FALSE : TRUE;
                counter_set_map->nof_entries++;
            }
        }
        /*
         * Note: Full color configuration could be made by 5 entries, but it would have to be written without loops.
         * here, we choose code visability over code run time. 
         */
    }
    else if(type_flag == EXPANSION_TYPE_DISPOSITION_AND_METADATA_PP_DROP_SET_2)
    {
          /** expansion selection strucutre. */
        expansion_select->nof_elements = 2;
        expansion_select->expansion_elements[0].type = bcmStatExpansionTypeDispositionIsDrop;
        expansion_select->expansion_elements[1].type = bcmStatExpansionTypeMetaData;
        expansion_select->expansion_elements[1].bitmap = 0x1;

        /** counter set mappping strucutre. */
        /** Offset 0 - TM drop */
       /** Offset 1 - PP drop  */
        *counter_set_size = 2;
        /** Entry 0 - TM drop; Entry 1 - clear PP drop and PP drop with
         *  priority
         *  
         *  The case of TM FWD and PP FWD will not be counted at all */
        counter_set_map->nof_entries = 2;
        /** Configure entry 0 - TM drop */
        /** For TM drop - DispositionIsDrop=1, MetaData(PP drop) = 0 */
        counter_set_map->expansion_data_mapping[0].nof_key_conditions = 2;
        counter_set_map->expansion_data_mapping[0].key[0].type = bcmStatExpansionTypeDispositionIsDrop;
        counter_set_map->expansion_data_mapping[0].key[0].value = 1;
        counter_set_map->expansion_data_mapping[0].key[1].type = bcmStatExpansionTypeMetaData;
        counter_set_map->expansion_data_mapping[0].key[1].value = 0;
        counter_set_map->expansion_data_mapping[0].value.counter_set_offset = 0;
        counter_set_map->expansion_data_mapping[0].value.valid = TRUE;

        /** Configure entry 1 - PP drop */
        /** PP drop consists from two entries */
        /** Entry 1: in case of clear pp drop */
        /** For PP drop - DispositionIsDrop=0, MetaData(PP drop)=1 */
         /** Entry 2: in case pp drop priority  */
        /** In case of both PP and TM drops - PP drop takes priority */
        /** For PP drop priority - DispositionIsDrop=1, MetaData(PP
         *  drop)=1 */
        /** Both entries can be merged in MetaData(PP drop)=1 and all
         *  distributions of DispositionIsDrop */
        counter_set_map->expansion_data_mapping[1].nof_key_conditions = 1;
        counter_set_map->expansion_data_mapping[1].key[0].type = bcmStatExpansionTypeMetaData;
        counter_set_map->expansion_data_mapping[1].key[0].value = 1;
        counter_set_map->expansion_data_mapping[1].value.counter_set_offset = 1;
        counter_set_map->expansion_data_mapping[1].value.valid = TRUE;
    }
    else
    {
        return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
}

/**
* \brief  
*   configure an irpp data base and enable it's counting and eviction
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
crps_irpp_data_base_create(
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
    bcm_stat_counter_interface_key_t_init(&interface_key);
    bcm_stat_expansion_select_t_init(&expansion_select);
    bcm_stat_counter_interface_t_init(&cnt_interface);
    bcm_stat_counter_enable_t_init(&enable_config);

    /** configure the expansion selection and counter set mapping */
    ctr_set_map.expansion_data_mapping = data_mapping_array;
    rv = crps_irpp_expansion_builder(unit, expansion_type_flag, &expansion_select, &ctr_set_map, &counter_set_size);
    if (rv != BCM_E_NONE)
    {
        printf("Error in crps_irpp_expansion_builder\n");
        return rv;
    }

    /*
     * step 1: set the expansion bits format for each interface, using API bcm_stat_counter_expansion_select_set
     * set the interface key. (the selection structure was already feel in function "crps_irpp_expansion_builder" ) 
     */
    interface_key.core_id = core;
    interface_key.command_id = command_id;
    interface_key.interface_source = bcmStatCounterInterfaceIngressReceivePp;

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

        /** step 3: attach the engines to the database */
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
     /** step 5: configure the counter interface per database */
    cnt_interface.source = bcmStatCounterInterfaceIngressReceivePp;
    cnt_interface.command_id = command_id;
    cnt_interface.format_type = bcmStatCounterFormatPacketsAndBytes;
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

    /** step 6: configure the counter set mapping per engine */
    rv = bcm_stat_counter_set_mapping_set(unit, mapping_flags, &database, &ctr_set_map);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_set_mapping_set\n");
        return rv;
    }

    /** step 7: enable the engine */
    enable_config.enable_counting = TRUE;
    enable_config.enable_eviction = TRUE;
    rv = bcm_stat_counter_database_enable_set(unit, 0, &database, &enable_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_database_enable_set\n");
        return rv;
    }

    /** print the data base */
    /*
     * rv = crps_data_base_print(unit, &interface_key, &database, engine_id_arr[0]); if (rv != BCM_E_NONE) {
     * printf("Error in crps_data_base_print\n"); return rv; } 
     */
    return BCM_E_NONE;
}

/**
 * \brief
 *   example of crps database with selected core_id, connected
 *   to irpp and print the database to screen. database
 *   description: (parameters value are arbitrary example.)
 *       For each core, 3 engines connected to src irpp, command_id=9. 
 *       configured with 4 types, different offsets.
 *       In this example , the types ranges do not cross engines. (meaning each type is valid only in one engine).
 * \param [in] unit -
 *     unit id
 * \param [in] base_engine_id -
 *     the first engine id in the database
 *     \param [in] database_id - database_id
 *     \param [in] core_id - core_id
 * \param [in] database_id
 * \return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
int
crps_irpp_database_example(
    int unit,
    int base_engine_id,
    int database_id,
    int core_id,
    crps_irpp_config_database_t create_db)
{
    int rv;
    int type_id, index, engine_idx;
    int nof_engines = 3;
    int engine_arr[3] = { base_engine_id, (base_engine_id + 1), (base_engine_id + 2) };
    int nof_types = 4;
    int type_valid[4] = { 1, 1, 1, 1 };
    uint32 *engine_size;
    uint32 type_offset[4] = { 0, 0, 0, 0 };
    int start_object_stat_id_arr[4] = { 0, 0, 0, 0 };
    int end_object_stat_id_arr[4];
    /* for case CRPS_IRPP_CONFIG_CREATE_SET_32 - use command id 0 as this expansion is
    for meter and ingress meter uses command ids 0/1/2 */
    int command_id = (create_db == CRPS_IRPP_CONFIG_CREATE_SET_32) ? 0 : 9;
    int expansion_flag[4] = { EXPANSION_TYPE_FINAL_DP_AND_METER_0_DP_SET_32,
        EXPANSION_TYPE_META_DATA_10BITS_SET_24,
        EXPANSION_TYPE_LEGACY_FINAL_DP_AND_DISPOSITION_SET_5,
        EXPANSION_TYPE_DISPOSITION_AND_METADATA_PP_DROP_SET_2
    };
    int counter_set_size[4] = { 32, 24, 5, 2 };    /* match to the expansion flag array */

    /** loop on all of the expansion flags. create database for each one of them */
    for (index = 0; index < 4; index++)
    {
        if (create_db != CRPS_IRPP_CONFIG_CREATE_ALL)
        {
            index = create_db;
        }
        /** configure type 0 */
        /** each type range holds 1 engine full entries, except type=2,3 which split one engine toghether */
        engine_size = dnxc_data_1d_get(unit, "crps", "engine", "engines_info", "nof_counters", engine_arr[0]);
        end_object_stat_id_arr[0] = (*engine_size) / counter_set_size[index] - 1;
        type_offset[0] = 0;

        /** configure type 1 */
        engine_size = dnxc_data_1d_get(unit, "crps", "engine", "engines_info", "nof_counters", engine_arr[1]);
        end_object_stat_id_arr[1] = (*engine_size) / counter_set_size[index] - 1;
        /** offset will be the prev offset + the end point of the prev type */
        type_offset[1] = (end_object_stat_id_arr[0] - start_object_stat_id_arr[0] + 1) + type_offset[0];

        /** configure type 2 */
        engine_size = dnxc_data_1d_get(unit, "crps", "engine", "engines_info", "nof_counters", engine_arr[2]);
        end_object_stat_id_arr[2] = ((*engine_size) / counter_set_size[index] - 1) / 2;
        type_offset[2] = (end_object_stat_id_arr[1] - start_object_stat_id_arr[1] + 1) + type_offset[1];

        /** configure type 3 */
        engine_size = dnxc_data_1d_get(unit, "crps", "engine", "engines_info", "nof_counters", engine_arr[2]);
        end_object_stat_id_arr[3] = ((*engine_size) / counter_set_size[index] - 1) / 2;
        type_offset[3] = (end_object_stat_id_arr[2] - start_object_stat_id_arr[2] + 1) + type_offset[2];

        rv = crps_irpp_data_base_create(unit, core_id, database_id, command_id, nof_types,
                                        start_object_stat_id_arr, end_object_stat_id_arr, type_valid, type_offset,
                                        nof_engines, engine_arr, expansion_flag[index]);
        if (rv != BCM_E_NONE)
        {
            printf("Error in crps_irpp_data_base_create \n");
            return rv;
        }
        
        if (create_db != CRPS_IRPP_CONFIG_CREATE_ALL)
        {
            break;
        }
    }

    return BCM_E_NONE;
}

/**
 * \brief
 *   wrapper for crps_irpp_data_base_create function which create a crps database.
 *   this wrapper produce a single engine database, with a single type.
 */
int
crps_irpp_data_base_create_wrapper(
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

    rv = crps_irpp_data_base_create(unit, core, database_id, command_id, nof_types, type_start_arr, type_end_arr, type_valid_arr, type_offset_arr, 1,   /* nof_engines 
                                                                                                                                                         */
                                    &engine_id, expansion_type_flag);

    if (rv != BCM_E_NONE)
    {
        printf("Error in crps_irpp_data_base_create \n");
        return rv;
    }

    printf("database %d was created\n", database_id);
    return BCM_E_NONE;
}
