/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 * File: cint_crps_general_config.c
 * related files: cint_crps_wrapper.c
 * Purpose:
 *
 * The cint include:
 *  - wrapper functions for API for engine configuration with
 *    valid scenarios
 */

/**
* \brief
*   create crps database with one engine attached.
*   set the counter interface and eviction configuration
* \par DIRECT INPUT:
*   \param [in] unit                            -  UNIT_ID
*   \param [in] core_id                         -  core_id
*   \param [in] source                          -  interface source
*   \param [in] command_id                      -  command id
*   \param [in] format_type                     -  counter format type
*   \param [in] type_id                         -  type id
*   \param [in] database_id                     -  database id
*   \param [in] engine_id                       -  engine id
*                                                 for format_type Slim, use big engine only.
*                                                 for format_type Narrow with eviction_destination_type Network, use only small/medium engine.
*   \param [in] eviction_event_id               -  eviction event id
*                                                 for Slim format, have to be between 0 to 7.
*   \param [in] eviction_destination_type       -  eviction destination
*                                                 for Slim format_type, use EvictionDestinationNetwork only.
*   \param [in] algorithmic_is_disable          -  destination type
*                                                 for Slim format, have to be 0
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
crps_general_single_counter_configuration_sequence_example(int unit,
        bcm_core_t core_id,
        bcm_stat_counter_interface_type_t source,
        int command_id,
        bcm_stat_counter_format_type_t format_type,
        int type_id,
        int database_id,
        int engine_id,
        int eviction_event_id,
        bcm_eviction_destination_type_t eviction_destination_type,
        int algorithmic_is_disable)
{
    int rv, flags;
    int dma_fifo_select  = 0;
    bcm_eviction_record_format_t record_format = bcmStatEvictionRecordFormatPhysical;
    int types_id_list = type_id;
    int nof_types = 1;
    int counter_set_size = 1;
    int database_size = *dnxc_data_1d_get(unit, "crps", "engine", "engines_info", "nof_counters", engine_id);

    /* create one crps database */
    flags = BCM_STAT_DATABASE_CREATE_WITH_ID;
    rv = crps_database_create_wrapper(unit, flags, core_id, database_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in crps_database_create_wrapper\n");
        return rv;
    }

    flags = 0;

    /* attach one engine */
    rv = crps_engine_attach_wrapper(unit, flags, engine_id, core_id, database_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in crps_engine_attach_wrapper\n");
        return rv;
    }

    /* create crps interface for one counter */
    rv = crps_database_interface_generic_wrapper(unit, flags, core_id, source, command_id, format_type, counter_set_size,
            &types_id_list, nof_types, database_id, database_size);

    if (rv != BCM_E_NONE)
    {
        printf("Error in crps_database_interface_generic_wrapper\n");
        return rv;
    }

    /* set eviction configurations */
    rv = crps_engine_eviction_wrapper(unit, flags, engine_id, core_id, dma_fifo_select,
                            eviction_destination_type, record_format, eviction_event_id, algorithmic_is_disable);

    if (rv != BCM_E_NONE)
    {
        printf("Error in crps_engine_eviction_wrapper\n");
        return rv;
    }

    return BCM_E_NONE;
}


/**
* \brief
*   cleanup for crps_general_single_counter_configuration_sequence_example
* \par DIRECT INPUT:
*   \param [in] unit                            -  UNIT_ID
*   \param [in] core_id                         -  core_id
*   \param [in] database_id                     -  database id
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
crps_general_single_counter_configuration_sequence_cleanup(int unit,
        bcm_core_t core_id,
        int database_id)
{
    int rv;
    uint32 flags = 0;
    int enable_counting = 0;
    int enable_eviction = 0;

    rv = crps_database_enable_wrapper(unit, flags, database_id, core_id, enable_counting, enable_eviction);

    if (rv != BCM_E_NONE)
    {
        printf("Error in crps_database_enable_wrapper\n");
        return rv;
    }

    rv = crps_database_destroy_wrapper(unit, core_id, database_id);

    if (rv != BCM_E_NONE)
    {
        printf("Error in crps_database_destroy_wrapper\n");
        return rv;
    }

    return BCM_E_NONE;

}
