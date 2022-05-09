/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:            cint_crps_wrapper.c
 * related files:
 * Purpose:
 *
 * The cint include:
 *  - wrapper functions for API for engine configuration with
 *    valid scenarios
 */


int NOF_VALID_TYPES = 4;

/**
* \brief
*   wrapper function for API
*   bcm_stat_counter_database_create
* \par DIRECT INPUT:
*   \param [in] unit                -  UNIT_ID
*   \param [in] flags               -  flags
*   \param [in] core_id             -  core_id
*   \param [in] database_id       -  database_id
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
crps_database_create_wrapper(
    int unit,
    uint32 flags,
    bcm_core_t core_id,
    int database_id)
{
    int rv;

    rv = bcm_stat_counter_database_create(unit, flags, core_id, &database_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_database_create\n");
        return rv;
    }
    return BCM_E_NONE;
}

/**
* \brief
*   destroy given database
* \par DIRECT INPUT:
*   \param [in] unit           -  UNIT_ID
*   \param [in] core          - core id
*   \param [in] database_id - database_id
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
crps_database_destroy_wrapper(
    int unit,
    int core,
    int database_id)
{
    int rv;
    bcm_stat_counter_database_t database;
    bcm_stat_counter_database_t_init(&database);

    database.database_id = database_id;
    database.core_id = core;

    rv = bcm_stat_counter_database_destroy(unit, 0, &database);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_database_destroy \n");
        return rv;
    }

    return BCM_E_NONE;
}

/**
* \brief
*   wrapper function for API
*   bcm_stat_counter_engine_attach
* \par DIRECT INPUT:
*   \param [in] unit                -  UNIT_ID
*   \param [in] flags               -  flags
*   \param [in] engine_id       -engine_id
*   \param [in] core_id             -  core_id
*   \param [in] database_id       -  database_id
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
crps_engine_attach_wrapper(
    int unit,
    uint32 flags,
    int engine_id,
    bcm_core_t core_id,
    int database_id)
{
    int rv;
    bcm_stat_counter_database_t database;
    bcm_stat_counter_database_t_init(&database);

    database.core_id = core_id;
    database.database_id = database_id;

    rv = bcm_stat_counter_engine_attach(unit, flags, &database, engine_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_engine_attach\n");
        return rv;
    }
    return BCM_E_NONE;
}

/**
* \brief
*   wrapper function for API bcm_stat_counter_engine_detach
* \par DIRECT INPUT:
*   \param [in] unit                -  UNIT_ID
*   \param [in] flags               -  flags
*   \param [in] engine_id       -engine_id
*   \param [in] core_id             -  core_id
*   \param [in] database_id       -  database_id
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
crps_engine_detach_wrapper(
    int unit,
    uint32 flags,
    int engine_id,
    bcm_core_t core_id,
    int database_id)
{
    int rv;
    bcm_stat_counter_database_t database;
    bcm_stat_counter_database_t_init(&database);

    database.core_id = core_id;
    database.database_id = database_id;

    rv = bcm_stat_counter_engine_detach(unit, flags, &database, engine_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_engine_detach\n");
        return rv;
    }
    return BCM_E_NONE;
}

/**
* \brief
*   wrapper function for API bcm_stat_counter_eviction_set with
*   different valid type configurations
* \par DIRECT INPUT:
*   \param [in] unit                -  UNIT_ID
*   \param [in] flags               -  flags
*   \param [in] engine_id           -  engine_id
*   \param [in] core_id             -  core_id
*   \param [in] dma_fifo_select     -  dma_fifo_select
*   \param [in] type                -  type
*   \param [in] record_format       -  record_format
*   \param [in] eviction_event_id   -  eviction_event_id
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
crps_engine_eviction_wrapper(
    int unit,
    int flags,
    int engine_id,
    bcm_core_t core_id,
    int dma_fifo_select,
    bcm_eviction_destination_type_t type,
    bcm_eviction_record_format_t record_format,
    int eviction_event_id,
    int algorithmic_is_disable)
{
    int rv;
    bcm_stat_engine_t engine;
    bcm_stat_eviction_t eviction;
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


    bcm_stat_engine_t_init(&engine);
    bcm_stat_eviction_t_init(&eviction);

    engine.core_id = core_id;
    engine.engine_id = engine_id;
    eviction.dma_fifo_select = dma_fifo_select;
    eviction.eviction_event_id = eviction_event_id;
    eviction.record_format = record_format;
    eviction.type = type;
    eviction.eviction_algorithmic_disable = algorithmic_is_disable;
    /** optimization - do not evict a counter which its value is zero */
    /** HW compare 34 bits of the counter */
    if ((*conditional_no_eviction_support == TRUE) && (feature_conditional_eviction_support == TRUE))
    {
        flags |= BCM_STAT_EVICTION_CONDITIONAL_ENABLE;
        eviction.cond.action_flags = BCM_STAT_EVICTION_CONDITIONAL_ACTION_NO_EVICTION;
        eviction.cond.condition_source_select = bcmStatEvictionConditionalSourceUserData;
        COMPILER_64_SET(eviction.cond.condition_user_data, 0x0, 0x0);
        eviction.cond.qualifier = bcmStatEvictionConditionalQualOrEqualZero;
    }

    rv = bcm_stat_counter_eviction_set(unit, flags, &engine, &eviction);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_eviction_set \n");
        return rv;
    }
    return BCM_E_NONE;
}

/**
* \brief
*   wrapper function for API bcm_stat_counter_interface_set with
*   different valid type configurations
* \par DIRECT INPUT:
*   \param [in] unit                -  UNIT_ID
*   \param [in] flags               -  flags
*   \param [in] core_id             -  core id
*   \param [in] source              -  interface source
*   \param [in] command_id          -  command id
*   \param [in] format_type         -  counter format type
*   \param [in] counter_set _size   -  counter set size
*   \param [in] type_id_list        -  list of the valid types id
*   \param [in] nof_types           -  number of valid types
*   \param [in] database_id         -  database id
*   \param [in] dtabase_size        -  database size
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
crps_database_interface_generic_wrapper(
    int unit,
    uint32 flags,
    bcm_core_t core_id,
    bcm_stat_counter_interface_type_t source,
    int command_id,
    bcm_stat_counter_format_type_t format_type,
    int counter_set_size,
    int *types_id_list,
    int nof_types,
    int database_id,
    int database_size)
{
    int rv, curr_type_id, i;
    bcm_stat_counter_database_t database;
    bcm_stat_counter_interface_t config;
    bcm_stat_counter_database_t_init(&database);
    bcm_stat_counter_interface_t_init(&config);

    database.core_id = core_id;
    database.database_id = database_id;

    config.command_id = command_id;
    config.source = source;
    config.format_type = format_type;
    config.counter_set_size = counter_set_size;

    for(curr_type_id = 0; curr_type_id < NOF_VALID_TYPES; curr_type_id++)
    {
        config.type_config[curr_type_id].valid = 0;
    }

    /* Split database equally between the valid types*/
    int type_range = database_size/nof_types;

    for(i = 0; i < nof_types; i++)
    {
        int type_id = types_id_list[i];

        config.type_config[type_id].valid = 1;
        config.type_config[type_id].start = 0;
        config.type_config[type_id].end = type_range-1;
        config.type_config[type_id].object_type_offset = i*type_range;
    }

    rv = bcm_stat_counter_interface_set(unit, flags, &database, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_interface_set\n");
        return rv;
    }
    return BCM_E_NONE;
}

/**
* \brief
*   wrapper function for API
*   bcm_stat_counter_database_enable_set with different valid
*   type configurations
* \par DIRECT INPUT:
*   \param [in] unit                -  UNIT_ID
*   \param [in] flags               -  flags
*   \param [in] database_id           -  database_id
*   \param [in] core_id             -  core_id
*   \param [in] enable_counting     -  enable_counting
*   \param [in] enable_eviction     -  enable_eviction
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
crps_database_enable_wrapper(
    int unit,
    uint32 flags,
    int database_id,
    bcm_core_t core_id,
    int enable_counting,
    int enable_eviction)
{
    int rv;
    bcm_stat_counter_database_t database;
    bcm_stat_counter_enable_t enable;

    bcm_stat_counter_database_t_init(&database);
    bcm_stat_counter_enable_t_init(&enable);

    database.core_id = core_id;
    database.database_id = database_id;
    enable.enable_counting = enable_counting;
    enable.enable_eviction = enable_eviction;

    rv = bcm_stat_counter_database_enable_set(unit, flags, &database, &enable);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_database_enable_set\n");
        return rv;
    }
    return BCM_E_NONE;
}

