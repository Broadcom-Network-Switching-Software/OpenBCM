/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_crps_db_utils.c
 * Purpose:     several utilities Examples for crps data-base
 * 
 * This cint invoke function of cint:
 *  cint ../../../src/examples/dnx/field/cint_field_always_hit_context.c
 * *
 * The cint include:
 *  - go over all databases andreset the configuration and
 *    clear the SW and HW counters memories of all engines.
 *  - go over all engines and interface of the data base and print it's configuration
 *  - read a counter value using API bcm_stat_counter_get
 *  *  cint ../../../src/examples/dnx/crps/cint_crps_db_utils.c
 */



bcm_field_context_t  cint_crps_context_id_ipmf1 = 0;
bcm_field_context_t  cint_crps_context_id_epmf = 0;
bcm_field_presel_t cint_crps_presel_id = 10;
bcm_mac_t cint_crps_trap_src_mac = {0};
int cint_crps_trap_id;
int cint_crps_trap_prog_index = 0;

enum crps_trap_stage_t {
    CRPS_TRAP_STAGE_IRPP = 0,
    CRPS_TRAP_STAGE_ERPP = 1
};
/**Information for ECMP group*/
struct cint_crps_db_ecmp_member_stat_config_s
{
    int start_stat_id;                   /** First stat id(the stat_id is associated with first FEC in the group) */
    int nof_members;                    /** Number of ECMP group members */
};
/**Array which holds the information for each ECMP group*/
cint_crps_db_ecmp_member_stat_config_s cint_crps_db_ecmp_info[3];
/**
* \brief
*  Create context for all Stage so that field groups of CRPS can be attached to it
*  Also configure PMF to always hit that context
* \param [in] unit        - Device ID
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int crps_field_context_create(int unit)
{
    int rv = 0;
    rv = cint_field_always_hit_context_without_sys_hdr_mod_main(unit,cint_crps_presel_id,bcmFieldStageIngressPMF1,&cint_crps_context_id_ipmf1);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_always_hit_context_without_sys_hdr_mod_main Ingress\n", rv);
        return rv;
    }

    rv = cint_field_always_hit_context_without_sys_hdr_mod_main(unit,cint_crps_presel_id,bcmFieldStageEgress,&cint_crps_context_id_epmf);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_always_hit_context_without_sys_hdr_mod_main Egress\n", rv);
        return rv;
    }

    return rv;
}
int crps_field_context_destroy(int unit)
{
    int rv = 0;
    rv = cint_field_always_hit_contex_destroy(unit,cint_crps_presel_id,bcmFieldStageIngressPMF1,cint_crps_context_id_ipmf1);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_always_hit_contex_destroy Ingress ctx %d \n", rv, cint_crps_context_id_ipmf1);
        return rv;
    }

    rv = cint_field_always_hit_contex_destroy(unit,cint_crps_presel_id,bcmFieldStageEgress,cint_crps_context_id_epmf);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_always_hit_contex_destroy Egress ctx %d \n", rv, cint_crps_context_id_epmf);
        return rv;
    }


    return rv;
}
/**
* \brief  
*   print the data base configuration
* \par DIRECT INPUT: 
*   \param [in] unit           -  
*   \param [in] interface_key  -  
*   \param [in] database -
*   \param [in] engine_id - one of the engines in the database
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
crps_data_base_print(
    int unit,
    bcm_stat_counter_interface_key_t * interface_key,
    bcm_stat_counter_database_t * database,
    int engine_id)
{
    int i, rv, flags;
    bcm_stat_engine_t engine;
    bcm_stat_counter_enable_t enable_config_received;
    bcm_stat_counter_interface_t cnt_interface_received;
    bcm_stat_counter_set_map_t ctr_set_map_recieved;
    bcm_stat_eviction_t eviction_revieved;
    bcm_stat_expansion_select_t expansion_select_recieved;

    flags = 0;
    bcm_stat_engine_t_init(&engine);
    bcm_stat_eviction_t_init(&eviction_revieved);
    bcm_stat_counter_set_map_t_init(&ctr_set_map_recieved);
    bcm_stat_expansion_select_t_init(&expansion_select_recieved);
    bcm_stat_counter_interface_t_init(&cnt_interface_received);
    bcm_stat_counter_enable_t_init(&enable_config_received);

    if (interface_key->interface_source == bcmStatCounterInterfaceIngressVoq
        || interface_key->interface_source == bcmStatCounterInterfaceEgressReceiveQueue)
    {
        printf("source=%d contain fixed expansion. No expansion selection required\n", interface_key->interface_source);
    }
    else
    {
        /** first print the expansion selection, which is the same for all data base */
        rv = bcm_stat_counter_expansion_select_get(unit, 0, interface_key, &expansion_select_recieved);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_stat_counter_expansion_select_get\n");
            return rv;
        }
        else
        {
            print expansion_select_recieved;
        }
    }

    rv = bcm_stat_counter_database_enable_get(unit, flags, database, &enable_config_received);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_database_enable_get\n");
        return rv;
    }
    else
    {
        print enable_config_received;
    }

    rv = bcm_stat_counter_interface_get(unit, flags, database, &cnt_interface_received);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_interface_get\n");
        return rv;
    }
    else
    {
        print cnt_interface_received;
    }

    /** ctr_set_map_recieved->nof_entries=0, therfore, nothing will be updated */
    rv = bcm_stat_counter_set_mapping_get(unit, flags, database, &ctr_set_map_recieved);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_set_mapping_get\n");
        return rv;
    }
    else
    {
        print ctr_set_map_recieved;
    }
    /** call get APIs and print the result */
    engine.core_id = interface_key->core_id;
    engine.engine_id = engine_id;

    rv = bcm_stat_counter_eviction_get(unit, flags, &engine, &eviction_revieved);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_eviction_get\n");
        return rv;
    }
    else
    {
        print eviction_revieved;
    }

    return BCM_E_NONE;
}

int stat_array[64]; /** array which holds the statistics types  {is_bytes, offset} for one counter set. size=64=max_counter_set_size*2SwCounters*/
uint64 expected_values_array[64]; /** array which holds the expected values for each element in the stat_array */
int result_values_array[350]; /** array which holds the statistics info for each FEC*/
/**
 * \brief
 *      tcl interface function to set the statistics type in the stat_array and
 *      the expected returned value from API bcm_stat_counter_get/bcm_stat_counter_explicit get.
 *      since tcl is not able to send an array to the cint, or to the API itself, we use this function
 *      expected_output_value = -1, means do not validate, just copy the actual counter value to expected_output_value
 */
int
crps_stats_array_set(
    int unit,
    int index,
    int is_bytes,
    int offset,
    uint32 expected_output_value)
{
    uint32 high32_data = 0;
    stat_array[index] = BCM_STAT_COUNTER_STAT_SET(is_bytes, offset);

    printf("crps_stats_array_set: index=%d, is_bytes=%d, offset=%d, expected_output_value=%d \n", 
        index, is_bytes, offset, expected_output_value);
    COMPILER_64_SET(expected_values_array[index], high32_data, expected_output_value);
    return BCM_E_NONE;
}

int crps_utils_counter_compare(
    int unit,
    int core_id,
    int db_or_eng_id,
    int type_id,
    int object_stat_id,
    int nstat,
    bcm_stat_counter_output_data_t *output_data)
{
    int i;
    int error_ind = FALSE;
    uint32 high32_data = 0;
    for (i = 0; i < nstat; i++)
    {
        if (COMPILER_64_LO(expected_values_array[i]) == -1)
        {
            /** -1 means not to check, just copy the actual value to the expected (low 32 bits) */
            COMPILER_64_SET(expected_values_array[i], high32_data, COMPILER_64_LO(output_data->value_arr[i].value));
        }
        else
        {
            /** cint don't know to compare uint64, therefore, we split it to hi and low and compare */
            if ((COMPILER_64_HI(expected_values_array[i]) != COMPILER_64_HI(output_data->value_arr[i].value)) ||
                (COMPILER_64_LO(expected_values_array[i]) != COMPILER_64_LO(output_data->value_arr[i].value)))
            {
                printf("verify counters value: unit=%d, core=%d, db_or_eng_id=%d, type_id=%d, object_stat_id=%d: \n",
                       unit, core_id, db_or_eng_id, type_id, object_stat_id);
                printf
                    ("\t FAIL: stat_arr[%d]={isbytes=%d, offset=%d}: received value={0x%x,0x%x} expected value={0x%x,0x%x}\n",
                     i, BCM_STAT_COUNTER_STAT_IS_BYTES_GET(stat_array[i]), BCM_STAT_COUNTER_STAT_OFFSET_GET(stat_array[i]),
                     COMPILER_64_HI(output_data->value_arr[i].value), COMPILER_64_LO(output_data->value_arr[i].value),
                     COMPILER_64_HI(expected_values_array[i]), COMPILER_64_LO(expected_values_array[i]));
                error_ind = TRUE;
            }
            else
            {
                /*
                 * printf ("verify counters value: unit=%d, core=%d, database_id=%d, type_id=%d, object_stat_id=%d: \n",
                 * unit, core_id, database_id, type_id, object_stat_id); printf ("\t PASS: stat_arr[%d]={isbytes=%d,
                 * offset=%d}: received value={0x%x,0x%x}\n", i, BCM_STAT_COUNTER_STAT_IS_BYTES_GET(stat_array[i]),
                 * BCM_STAT_COUNTER_STAT_OFFSET_GET(stat_array[i]), COMPILER_64_HI(output_data.value_arr[i].value),
                 * COMPILER_64_LO(output_data.value_arr[i].value));
 */
            }            
        }

    }
    return error_ind;
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
crps_database_stat_counter_get(
    int unit,
    int flags,
    int core_id,
    int database_id,
    int type_id,
    int object_stat_id,
    int nstat)
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
    check_error = 
    crps_utils_counter_compare(unit, core_id, database_id, type_id, object_stat_id, nstat, &output_data);

    if (check_error == TRUE)
    {
        return BCM_E_FAIL;
    }
    else
    {
        return BCM_E_NONE;
    }
}


/**
 * \brief
 *      get statistic counters value base on core, database_id, type
 *      the statistics type is only packets (and not bytes).
 *      The stat_id is associated with number of hits in each FEC.
 *      The function calculates the difference between the FEC with the most traffic and the
 *      FEC with lowest traffic.
 * \param [in] unit - unit id
 * \param [in] flags - crps flags
 * \param [in] core_id -core id
 * \param [in] database_id - database id that will be used when reading a counter
 * \param [in] type_id - used in crps to distinguish between different segments on the same engine
 * \param [in] index - the index of the array cint_crps_db_ecmp_info,which holds the information for each ECMP group.
 * \param [in] nstat -number of statistics to read that belong to the same counter_set
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case the percentage is not expected
 *   \retval Zero (= _SHR_E_NONE) in case of expected percentage
 * \remark
 *   NONE
 * \see
 *   NONE
 */
int
crps_database_stat_counter_get_no_verify(
    int unit,
    int flags,
    int core_id,
    int database_id,
    int type_id,
    int index,
    int nstat)
{
    int rv, stat_id;
    int max, min;
    int nof_stat_ids;
    double sum;
    double percent;
    double exp_percent = 5.0;
    const uint32 *max_counter_set_size = dnxc_data_get(unit, "crps", "engine", "max_counter_set_size", NULL);
    bcm_stat_counter_value_t counter_value[(*max_counter_set_size) * 2];
    bcm_stat_counter_input_data_t input_data;
    bcm_stat_counter_output_data_t output_data;
    nof_stat_ids = cint_crps_db_ecmp_info[index].start_stat_id + cint_crps_db_ecmp_info[index].nof_members;
    bcm_stat_counter_input_data_t_init(&input_data);
    bcm_stat_counter_output_data_t_init(&output_data);
    for(stat_id = cint_crps_db_ecmp_info[index].start_stat_id; stat_id < nof_stat_ids; stat_id++)
    {
        /** fill the input structure */
        input_data.core_id = core_id;
        input_data.database_id = database_id;
        input_data.type_id = type_id;
        input_data.counter_source_id = stat_id;
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
        COMPILER_64_TO_32_LO(result_values_array[stat_id], output_data.value_arr[0].value);
    }
    max = result_values_array[cint_crps_db_ecmp_info[index].start_stat_id];
    min = result_values_array[cint_crps_db_ecmp_info[index].start_stat_id];

    /** Get the min and the max element in each ECMP group */
    /** Calculate the sum of the elements */
    /** Each elements present the hits in the FEC*/
    for(stat_id = cint_crps_db_ecmp_info[index].start_stat_id; stat_id < nof_stat_ids; stat_id++)
    {
        if (max < result_values_array[stat_id])
        {
            max = result_values_array[stat_id];
        }
        if (result_values_array[stat_id] < min)
        {
            min = result_values_array[stat_id];
        }
        sum = result_values_array[stat_id] + sum;
    }
    /** Calculate the difference between the FEC with the most traffic and the one with lowest traffic.*/
    percent = 100*(max - min) / sum;
    printf ("ECMP group result:\n");
    printf ("The difference between the FEC with the most traffic and the one with lowest traffic is %f\n\n",percent);
    if (percent > exp_percent)
    {
        printf("The percent is not as expected.");
        return BCM_E_FAIL;
    } else {
        return BCM_E_NONE;
    }
}

/**
 * \brief
 *      get statisitic counters value base on core, database_id, type
 *      the statistics type is only packets (and not bytes)
 * \param [in] unit -
 *     unit
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
crps_database_stat_counter_explicit_get(
    int unit,
    int flags,
    int core_id,
    int engine_id,
    int type_id,
    int object_stat_id,
    int nstat)
{
    int rv, i;
    const uint32 *max_counter_set_size = dnxc_data_get(unit, "crps", "engine", "max_counter_set_size", NULL);
    bcm_stat_counter_explicit_input_data_t input_data;
    bcm_stat_counter_output_data_t output_data;
    bcm_stat_counter_value_t counter_value[(*max_counter_set_size) * 2];
    bcm_stat_engine_t engine;
    int check_error = FALSE;

    printf("crps_database_stat_counter_explicit_get: core_id=%d, engine_id=%d, type_id=%d,object_stat_id=%d,nstat=%d \n", 
            core_id, engine_id, type_id, object_stat_id, nstat);

    bcm_stat_engine_t_init(&engine);
    bcm_stat_counter_explicit_input_data_t_init(&input_data);
    bcm_stat_counter_output_data_t_init(&output_data);

    engine.engine_id = engine_id;
    engine.core_id = core_id;
    /** fill the input structure */
    input_data.engine = engine;
    input_data.object_stat_id = object_stat_id;
    input_data.type_id = type_id;
    input_data.nstat = nstat;
    input_data.stat_arr = stat_array;
    output_data.value_arr = counter_value;
    /** call the API */
    rv = bcm_stat_counter_explicit_get(unit, flags, &input_data, &output_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_get\n");
        return rv;
    }
    /** compare the received values to the input expected values */
    check_error = 
    crps_utils_counter_compare(unit, core_id, engine_id, type_id, object_stat_id, nstat, &output_data);    
    if (check_error == TRUE)
    {
        return BCM_E_FAIL;
    }
    else
    {
        return BCM_E_NONE;
    }
}


enum crps_config_field_group_t {
    CRPS_INGRESS_STAT_ID = 0, 
    CRPS_INGRESS_META_DATA = 1,
    CRPS_ERPP_STAT_ID = 2
};
/** Field group id - used when adding/deleting ACL rule */
bcm_field_group_t fg_id[3] = {0};
/** Field entry handle - used when adding/deleting ACL rule */
bcm_field_entry_t ent_id[3];

/**
 * \brief
 * based on policer_ingress_generic_traffic_example
 * set ACL rule, generate stat_id for meter or/and counter
 * \param [in] unit -
 *     unit id
 * \param [in] core_id - core_id
 * \param [in] in_port - in port 
 * \param [in] command_id - For CRPS - command_id; For MRPS 
 *        - meter database id (should be the same as the crps
 *          command id)
 * \param [in] stat_id - the stat_id that is selected to be 
 *        generated for counter and/or meter
 * \param [in] type_id -  for profile creating - type
 * \param [in] is_meter -  for profile creating - meter
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
int
crps_irpp_acl_rule_generate_stat_id(
    int unit,
    int core_id,
    int in_port, 
    int command_id,
    int stat_id,
    int type_id,
    int is_meter)
{
    int rv;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_qualify_t qual_type;
    uint32 qual_value;
    bcm_gport_t gport;
    void *dest_char;
    char *char_dest_char;

    printf("crps_irpp_acl_rule_generate_stat_id: unit=%d, core_id=%d, in_port=%d, command_id=%d, stat_id=%d, type_id=%d, is_meter=%d \n",
        unit, core_id, in_port, command_id, stat_id, type_id, is_meter);
        
    /** map packet to meter_id, according to in-port 
        based on  policer_ingress_meter_id_mapping_by_acl
        create ingress acl rule which generate meter pointer on
       one meter database and crps interface id */
    qual_type = bcmFieldQualifyInPort;
    if (!BCM_GPORT_IS_TRUNK(in_port)) {
        BCM_GPORT_LOCAL_SET(gport, in_port);
    } else {
        gport = in_port;
    }
    qual_value = gport; 
    
    /** Create and attach first group */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /* Set quals */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = qual_type;
    /* Set actions */
    /* if MetaData will also be set add action type for it*/
    fg_info.nof_actions = 2;
    fg_info.action_types[0] = (bcmFieldActionStatId0 + command_id);
    fg_info.action_types[1] = (bcmFieldActionStatProfile0 + command_id);
    dest_char = &(fg_info.name[0]);
    char_dest_char = dest_char;
    snprintf(char_dest_char, sizeof(fg_info.name), "CRPS_stat_id_%d", command_id);
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id[CRPS_INGRESS_STAT_ID]);
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

    rv = bcm_field_group_context_attach(unit, 0, fg_id[CRPS_INGRESS_STAT_ID], cint_crps_context_id_ipmf1, &attach_info);
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
    ent_info.entry_qual[0].value[0] = qual_value;
    ent_info.entry_qual[0].mask[0] = 0x1FF;
    ent_info.core = core_id;
    ent_info.nof_entry_actions = 2;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = stat_id;
    ent_info.entry_action[1].type = fg_info.action_types[1];
    /** create profile for counter and meter*/
    ent_info.entry_action[1].value[0] = is_meter;
    ent_info.entry_action[1].value[1] = 0;
    ent_info.entry_action[1].value[2] = type_id;
    ent_info.entry_action[1].value[3] = 1;
    
    rv = bcm_field_entry_add(unit, 0, fg_id[CRPS_INGRESS_STAT_ID], &ent_info, &ent_id[CRPS_INGRESS_STAT_ID]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }
    printf("Entry add: id:(0x%x) crps_command/mrps_database id:(%d) stat_id:(%d)\n", fg_id[CRPS_INGRESS_STAT_ID] ,command_id, stat_id);

    return rv;
}

/**
 * \brief
 * Create an Ingress Programmable trap to match on the DA of the packet,
 * pair it with a UserDefine trap, which is set to generate stat_id for meter or/and counter
 * \param [in] unit -
 *     unit id
 * \param [in] core_id - core_id
 * \param [in] command_id - For CRPS - command_id; For MRPS
 *        - meter database id (should be the same as the crps
 *          command id)
 * \param [in] stat_id - the stat_id that is selected to be
 *        generated for counter and/or meter
 * \param [in] type_id -  for profile creating - type
 *  \param [in] is_meter -  for profile creating - meter
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
int
crps_irpp_trap_rule_generate_stat_id(
    int unit,
    int core_id,
    int command_id,
    int stat_id,
    int type_id,
    int is_meter)
{
    int rv;
    int trap_strength = 15;
    int snoop_strength = 0;
    bcm_rx_trap_config_t config;
    bcm_rx_trap_prog_config_t prog_config;
    bcm_gport_t trap_gport;

    printf("crps_irpp_acl_rule_generate_stat_id: unit=%d, core_id=%d, command_id=%d, stat_id=%d, type_id=%d, is_meter=%d \n",
        unit, core_id, command_id, stat_id, type_id, is_meter);

    bcm_rx_trap_config_t_init(&config);
    bcm_rx_trap_prog_config_t_init(&prog_config);

    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &cint_crps_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_rx_trap_type_create\n", rv);
        return rv;
    }

    config.stat_obj_config_len = 1;
    config.stat_obj_config_arr[0].counter_command_id = command_id;
    config.stat_obj_config_arr[0].stat_id = stat_id;
    config.stat_obj_config_arr[0].is_meter = is_meter;
    config.stat_obj_config_arr[0].stat_object_type = type_id;

    rv = bcm_rx_trap_set(unit, cint_crps_trap_id, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_rx_trap_set\n", rv);
        return rv;
    }

    BCM_GPORT_TRAP_SET(trap_gport, cint_crps_trap_id, trap_strength, snoop_strength);

    prog_config.src_mac = cint_crps_trap_src_mac;
    prog_config.src_mac_nof_bits = 48;
    prog_config.src_mac_enable = bcmRxTrapProgEnableMatch;
    prog_config.trap_gport = trap_gport;

    rv = bcm_rx_trap_prog_set(unit, 0, cint_crps_trap_prog_index, &prog_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_rx_trap_prog_set\n", rv);
        return rv;
    }


    return rv;
}

/**
 * \brief
 * create an Ingress Programmable trap to match on the DA of the packet,
 * pair it with a UserDefine trap, which is set to generate stat_id for meter or/and counter
 * \param [in] unit -
 *     unit id
 * \param [in] core_id - core_id
 * \param [in] command_id - For CRPS - command_id; For MRPS
 *        - meter database id (should be the same as the crps
 *          command id)
 * \param [in] stat_id - the stat_id that is selected to be
 *        generated for counter and/or meter
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
int
crps_erpp_trap_rule_generate_stat_id(
    int unit,
    int core_id,
    int command_id,
    int stat_id)
{
    int rv;
    int trap_strength = 15;
    int snoop_strength = 0;
    bcm_rx_trap_config_t config;
    bcm_rx_trap_prog_config_t prog_config;
    bcm_gport_t trap_gport;

    bcm_rx_trap_config_t_init(&config);

    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapEgUserDefine, &cint_crps_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_rx_trap_type_create\n", rv);
        return rv;
    }

    config.flags |= BCM_RX_TRAP_UPDATE_COUNTER;
    config.stat_obj_config_arr[0].stat_id = stat_id;
    config.stat_obj_config_arr[0].stat_profile = command_id;

    rv = bcm_rx_trap_set(unit, cint_crps_trap_id, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_rx_trap_set\n", rv);
        return rv;
    }

    BCM_GPORT_TRAP_SET(trap_gport, cint_crps_trap_id, trap_strength, snoop_strength);

    rv = bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgInvalidDestPort, trap_gport);
    if(rv != BCM_E_NONE)
    {
        printf("Error, in bcm_dnx_rx_trap_action_profile_set \n");
        return rv;
    }


    return rv;
}
/**
 * \brief
 * Destroy trap config for stat_obj creation.
 * \param [in] unit -
 *     unit id
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
int crps_trap_rule_destroy(
    int unit,
    crps_trap_stage_t stage)
{
    int rv = BCM_E_NONE;
    bcm_rx_trap_prog_config_t prog_config;
    bcm_gport_t trap_gport;

    bcm_rx_trap_prog_config_t_init(&prog_config);

    BCM_GPORT_TRAP_SET(trap_gport, cint_crps_trap_id, 0, 0);
    if(stage == CRPS_TRAP_STAGE_IRPP)
    {
        prog_config.trap_gport = trap_gport;

        rv = bcm_rx_trap_prog_set(unit, 0, cint_crps_trap_prog_index, &prog_config);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_rx_trap_prog_set\n", rv);
            return rv;
        }
    } else if(stage == CRPS_TRAP_STAGE_ERPP) {

        bcm_rx_trap_action_profile_clear(unit,bcmRxTrapEgInvalidDestPort);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_rx_trap_action_profile_clear\n", rv);
            return rv;
        }
    }

    rv = bcm_rx_trap_type_destroy(unit, cint_crps_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_rx_trap_type_destroy\n", rv);
        return rv;
    }

    return rv;
}



/**
 * \brief
 * ACL rule to generete IRPP MetaData
 * \param [in] unit -
 *     unit id
 * \param [in] core_id - core_id
 * \param [in] in_port - in port 
 *  \param [in] metadata_value - if MetaData is used, set its
 *         value
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
int
crps_ingress_metadata_generate(
    int unit,
    int core_id,
    int in_port,    
    int metadata_value)
{
    int rv;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_qualify_t qual_type;
    uint32 qual_value;
    bcm_gport_t gport;      
    
    qual_type = bcmFieldQualifyInPort;
    BCM_GPORT_LOCAL_SET(gport, in_port);
    qual_value = gport;
    
    /** Create and attach first group */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /* Set quals */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = qual_type;
    /* Set actions */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionStatMetaData;
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id[CRPS_INGRESS_META_DATA]);
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
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

    rv = bcm_field_group_context_attach(unit, 0, fg_id[CRPS_INGRESS_META_DATA], cint_crps_context_id_ipmf1, &attach_info);
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
    ent_info.entry_qual[0].value[0] = qual_value;
    ent_info.entry_qual[0].mask[0] = 0x1FF;

    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = metadata_value;
        
    rv = bcm_field_entry_add(unit, 0, fg_id[CRPS_INGRESS_META_DATA], &ent_info, &ent_id[CRPS_INGRESS_META_DATA]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    return rv;
}

/**
 * \brief 
 * set ACL rule, generate stat_id for ERPP CRPS counter
 * \param [in] unit -
 *     unit id
 * \param [in] core_id - core_id
 * \param [in] out_port - out port 
 * \param [in] command_id - command_id;
 * \param [in] stat_id - the stat_id that is selected to be 
 *        generated for counter
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
int
crps_erpp_acl_rule_generate_stat_id(
    int unit,
    int core_id,
    int out_port, 
    int command_id,
    int stat_id)
{
    int rv;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_qualify_t qual_type;
    uint32 qual_value;
    bcm_gport_t gport;
      
    /** map packet according to out-port */
    qual_type = bcmFieldQualifyOutPort;
    BCM_GPORT_LOCAL_SET(gport, out_port);
    qual_value = gport;

    /** Create and attach first group */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageEgress;

    /* Set quals */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = qual_type;
    /* Set actions */
    fg_info.nof_actions = 2;
    fg_info.action_types[0] = (bcmFieldActionStatId0 + command_id);
    fg_info.action_types[1] = (bcmFieldActionStatProfile0 + command_id);
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id[CRPS_ERPP_STAT_ID]);
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

    rv = bcm_field_group_context_attach(unit, 0, fg_id[CRPS_ERPP_STAT_ID], cint_crps_context_id_epmf, &attach_info);
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
    ent_info.entry_qual[0].value[0] = qual_value;
    ent_info.entry_qual[0].mask[0] = 0x1FF;

    ent_info.nof_entry_actions = 2;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = stat_id;
    ent_info.entry_action[1].type = fg_info.action_types[1];
    ent_info.entry_action[1].value[0] = command_id; /** 0x0 pmf_counter_0_ptr; 0x1 pmf_counter_1_ptr; 0x2 local_outlif; 0x3 pph_forward_domain; 0x4 out_pp_port;  else: counter_ptr_null */
    ent_info.entry_action[1].value[1] = 0;
    ent_info.entry_action[1].value[2] = 0;
    ent_info.entry_action[1].value[3] = 0;
    
    rv = bcm_field_entry_add(unit, 0, fg_id[CRPS_ERPP_STAT_ID], &ent_info, &ent_id[CRPS_ERPP_STAT_ID]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }
    printf("Entry add: id:(0x%x) crps_command/mrps_database id:(%d) stat_id:(%d)\n", fg_id[CRPS_ERPP_STAT_ID] ,command_id, stat_id);

    return rv;
}

/** Destroy ACL rule related - filed entry/ field group/
 *  field context - select index (stat_id/metadata) */
int crps_acl_rule_destroy(int unit,
                          crps_config_field_group_t i)
{
    int rv = BCM_E_NONE;
    bcm_field_context_t ctx;
    bcm_field_stage_t stage;

    if(i  == CRPS_ERPP_STAT_ID)
    {
        ctx = cint_crps_context_id_epmf;
        stage = bcmFieldStageEgress;
    }
    else
    {
        ctx = cint_crps_context_id_ipmf1;
        stage = bcmFieldStageIngressPMF1;
    }

    rv = bcm_field_entry_delete(unit, fg_id[i], NULL, ent_id[i]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete  fg %d \n", rv, fg_id[i]);
        return rv;
    }
    /** Sequence to destroy the context and delete the groups -
     *  for now is used the default context -  Cannot destroy
     *  context ID 0, since it is the default context */
     
    rv = bcm_field_group_context_detach(unit, fg_id[i], ctx);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d stage %s\n", rv, fg_id[i], 0);
        return rv;
    } 
    rv = bcm_field_group_delete(unit, fg_id[i]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete  fg %d \n", rv, fg_id[i]);
        return rv;
    } 

    return rv;
}

/** ETPP ACL pointer generation - used as example
 *  cint_field_ace.c */
bcm_field_ace_format_t ace_format_id;
uint32 ace_entry_handle;
bcm_field_group_t epmf_fg_id;
bcm_field_entry_t epmf_entry_handle;

/**
* \brief
*  Creates an ACE format and ACE entry and an ePMF field group that points to the ACE entry.
*  Generate stat-id for ETPP
* \param [in] unit             - Device ID
* \param [in] stat_id        - stat_id to be set 
* \param [in] profile        - profile (PP stat profile)
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int crps_etpp_ace_generate_stat_id(
    int unit,
    int out_port,
    int stat_id,
    int profile)
{
    bcm_field_ace_format_info_t ace_format_info;
    bcm_field_ace_entry_info_t ace_entry_info;
    bcm_field_group_info_t epmf_fg_info;
    bcm_field_group_attach_info_t epmf_attach_info;
    bcm_field_entry_info_t epmf_entry_info;
    bcm_gport_t gport;
    int flags = 0;
    int rv = BCM_E_NONE;
    int i = 0;

    bcm_field_ace_format_info_t_init(&ace_format_info);

    ace_format_info.nof_actions = 1;

    ace_format_info.action_types[0] = bcmFieldActionStat1; /**crps*/

    rv = bcm_field_ace_format_add(unit, 0, &ace_format_info, &ace_format_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_ace_format_add\n", rv);
        return rv;
    }

    bcm_field_ace_entry_info_t_init(&ace_entry_info);
    
    ace_entry_info.nof_entry_actions = ace_format_info.nof_actions;

    ace_entry_info.entry_action[0].type = bcmFieldActionStat1;
    /** composed of command (profile) and stat_id. valid is 1.*/
    ace_entry_info.entry_action[0].value[0] = profile & 0x1F;
    ace_entry_info.entry_action[0].value[1] = stat_id;
    ace_entry_info.entry_action[0].value[2] = 1;

    rv = bcm_field_ace_entry_add(unit, 0, ace_format_id, &ace_entry_info, &ace_entry_handle);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_ace_entry_add\n", rv);
        return rv;
    }

    /*
     * Create a field group in ePMF that uses the ACE entry.
     */
    
    bcm_field_group_info_t_init(&epmf_fg_info);
    epmf_fg_info.fg_type = bcmFieldGroupTypeTcam;
    epmf_fg_info.nof_quals = 1;
    epmf_fg_info.stage = bcmFieldStageEgress;
    epmf_fg_info.qual_types[0] = bcmFieldQualifyOutPort;
    epmf_fg_info.nof_actions = 1;
    /* The first action decides the ACE entry that will happen. */
    epmf_fg_info.action_types[0] = bcmFieldActionAceEntryId;
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &epmf_fg_info, &epmf_fg_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    bcm_field_entry_info_t_init(&epmf_entry_info);
    epmf_entry_info.nof_entry_quals = epmf_fg_info.nof_quals;
    epmf_entry_info.nof_entry_actions = epmf_fg_info.nof_actions;
    for(i=0; i< epmf_fg_info.nof_quals; i++)
    {
        epmf_entry_info.entry_qual[i].type = epmf_fg_info.qual_types[i];
    }
    for(i=0; i<epmf_fg_info.nof_actions; i++)
    {
        epmf_entry_info.entry_action[i].type = epmf_fg_info.action_types[i];
    }
    BCM_GPORT_LOCAL_SET(gport, out_port);
    epmf_entry_info.entry_qual[0].value[0] = gport;
    epmf_entry_info.entry_qual[0].mask[0] = 0;
    epmf_entry_info.entry_action[0].value[0] = ace_entry_handle;

    rv = bcm_field_entry_add(unit, 0, epmf_fg_id, &epmf_entry_info, &epmf_entry_handle);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }  
    
    bcm_field_group_attach_info_t_init(&epmf_attach_info);
    epmf_attach_info.key_info.nof_quals = epmf_fg_info.nof_quals;
    epmf_attach_info.payload_info.nof_actions = epmf_fg_info.nof_actions;
    for(i=0; i< epmf_fg_info.nof_quals; i++)
    {
        epmf_attach_info.key_info.qual_types[i] = epmf_fg_info.qual_types[i];
    }
    for(i=0; i<epmf_fg_info.nof_actions; i++)
    {
        epmf_attach_info.payload_info.action_types[i] = epmf_fg_info.action_types[i];
    }
    epmf_attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    epmf_attach_info.key_info.qual_info[0].input_arg = 0;
    epmf_attach_info.key_info.qual_info[0].offset = 0;

    rv = bcm_field_group_context_attach(unit, 0, epmf_fg_id, cint_crps_context_id_epmf, &epmf_attach_info);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }  

    printf("ACE format (%d), ACE entry (%d) and ePMF FG (%d) created.\n",
           ace_format_id, ace_entry_handle, epmf_fg_id);
    return rv;
    
}

/**
* \brief
*  Update the value of the action in the ACE entry.
* \param [in] unit             - Device ID 
* \param [in] stat_id        - stat_id to be set 
* \param [in] profile        - profile (PP stat profile) 
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int crps_etpp_ace_update_stat_id(
    int unit,
    int stat_id,
    int profile)
{
    bcm_field_ace_entry_info_t ace_entry_info;
    int rv = BCM_E_NONE;
    int flags;

    bcm_field_ace_entry_info_t_init(&ace_entry_info);

    ace_entry_info.nof_entry_actions = 1;
    
    ace_entry_info.entry_action[0].type = bcmFieldActionStat1;
    /** The value the ACE entry is {b'1 valid, b'20 stat_id, b'5 CMD (profile} */
    ace_entry_info.entry_action[0].value[0] = profile & 0x1F;
    ace_entry_info.entry_action[0].value[1] = stat_id;
    ace_entry_info.entry_action[0].value[2] = 1;
    
    flags = BCM_FIELD_FLAG_UPDATE;
    rv = bcm_field_ace_entry_add(unit, flags, ace_format_id, &ace_entry_info, &ace_entry_handle);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_ace_entry_add\n", rv);
        return rv;
    }

    printf("Updated ACE entry %d with profile %d stat_id %d.\n", ace_entry_handle, (profile & 0x1F), stat_id);
    return rv;
    
}

/**
* \brief
*  Delete the ACE format and the ePMF field group that uses it.
* \param [in] unit             - Device ID
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int crps_etpp_ace_destroy(int unit)
{
    int rv = BCM_E_NONE;
    int i= 0;
    bcm_field_entry_qual_t entry_qual_info[BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP];

    rv = bcm_field_group_context_detach(unit, epmf_fg_id, cint_crps_context_id_epmf);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach\n", rv);
        return rv;
    }

    rv = bcm_field_entry_delete(unit, epmf_fg_id, &entry_qual_info, epmf_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete\n", rv);
        return rv;
    }
  
    rv = bcm_field_group_delete(unit, epmf_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete\n", rv);
        return rv;
    }

    rv = bcm_field_ace_entry_delete(unit, ace_entry_handle);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_ace_entry_delete\n", rv);
        return rv;
    }   
  
    rv = bcm_field_ace_format_delete(unit, ace_format_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_ace_format_delete\n", rv);
        return rv;
    }

    return rv;
}

int crps_qos_map_id;
/**
* \brief
*  set TC and DP for a cetain in port
* \param [in] unit             - Device ID
* \param [in] port             - in port
* \param [in] tc               - mapped TC
* \param [in] dp               - mapped DP
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int crps_qos_ingress_port_set(
    int unit,
    int port,
    int tc,
    int dp)
{
    int rc = BCM_E_NONE;
    int flags;
    bcm_qos_map_t map_entry;
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK;
    rc = bcm_qos_map_create(unit, flags, &crps_qos_map_id);
    if (BCM_E_NONE != rc)
    {
        printf("Error in bcm_qos_map_create\n");
        return rc;
    }    
    bcm_qos_map_t_init(&map_entry);
    map_entry.int_pri = tc;
    map_entry.color = dp;
    flags = BCM_QOS_MAP_PHB | BCM_QOS_MAP_PORT;
    rc = bcm_qos_map_add(unit, flags, &map_entry, crps_qos_map_id);
    if (BCM_E_NONE != rc)
    {
        printf("Error in bcm_qos_map_add\n");
        return rc;
    }      
    rc = bcm_qos_port_map_set(unit, port, crps_qos_map_id, -1);
    if (BCM_E_NONE != rc)
    {
        printf("Error in bcm_qos_port_map_set\n");
        return rc;
    }      
    return rc;    
}

/* */

/**
* \brief
*  Deletes and destroys the cos profile entry
* \param [in] unit             - Device ID
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int crps_qos_ingress_port_destroy(
    int unit)
{
    /* dummy map struct for the function */
    bcm_qos_map_t map;
    int rc = BCM_E_NONE;
/*
    int flags = BCM_QOS_MAP_PHB | BCM_QOS_MAP_PORT;
    bcm_qos_map_t_init(&map);

    rc= bcm_qos_map_delete( unit, flags, &map, crps_qos_map_id); 
    if (BCM_E_NONE != rc)
    {
        printf("Error in bcm_qos_map_delete\n");
        return rc;
    }
*/
    rc = bcm_qos_map_destroy(unit, crps_qos_map_id);
    if (BCM_E_NONE != rc)
    {
        printf("Error in bcm_qos_map_destroy\n");
        return rc;
    }

    return rc;
}
