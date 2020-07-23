/** \file crps_stat_apis.c
 * 
 *  crps module gather all functionality that related to the counter(statistic) processor driver for DNX.
 *  crps_stat_api.c holds all the CRPS APIs implemantation for DNX
 *  It's matching h file is bcm/stat.h. 
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_CNT

/*
 * Include files.
 * {
 */
#include <shared/utilex/utilex_bitstream.h>
#include <shared/shrextend/shrextend_error.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/stat.h>
#include "crps_eviction.h"
#include "crps_engine.h"
#include "crps_mgmt_internal.h"
#include "crps_verify.h"
#include <bcm_int/dnx/stat/crps/crps_src_interface.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <include/bcm_int/dnx/algo/crps/crps_ctrs_db.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_crps.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

/*
 * }
 */

/*
 * MACROs
 * {
 */

/*
 * }
 */

/**
* \brief  
*   API which enable or disable an engine. 
*   it verify the input parameters, update sw state and HW.
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit ID
*   \param [in] flags   -  reset flag, in order to clear configuration and clear SW+HW counters. can be called only if full disable is made.
*   \param [in] database  -  pointer to structure which contain the database_id and core_id
*   \param [in] enable  -  structure which define if to enable or disbale the counting(input of engine) and eviction (output of engine)
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * 
* \see
*   * None
*/
int
bcm_dnx_stat_counter_database_enable_set(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_enable_t * enable)
{
    bcm_stat_counter_database_t database_main;
    int core_idx;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_crps_mgmt_counter_database_enable_set_verify(unit, flags, database, enable));
    DNXCMN_CORES_ITER(unit, database->core_id, core_idx)
    {
        database_main.core_id = core_idx;
        database_main.database_id = database->database_id;
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_database_enable_set(unit, flags, &database_main, enable));
    }

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   API which get the  engine enable status
*   it verify the input parameters, and get the parameters from sw state
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit ID
*   \param [in] flags   -  NONE
*   \param [in] database  -  pointer to structure which contain the database_id and core_id
*   \param [out] enable  -  structure which define if to enable or disbale the counting(input of engine) and eviction (output of engine)
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * 
* \see
*   * None
*/

int
bcm_dnx_stat_counter_database_enable_get(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_enable_t * enable)
{
    bcm_stat_counter_database_t database_main;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_crps_mgmt_counter_database_enable_get_verify(unit, flags, database, enable));
    database_main.core_id = (database->core_id == BCM_CORE_ALL) ? 0 : database->core_id;
    database_main.database_id = database->database_id;
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_database_enable_get(unit, flags, &database_main, enable));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   control set API per database
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit ID
*   \param [in] flags   -  0
*   \param [in] database    -   core_id and database_id
*   \param [in] control   -  control enum
*   \param [in] arg   -  arg

* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   *
* \see
*   * None
*/
int
bcm_dnx_stat_counter_database_control_set(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_database_control_t control,
    uint32 arg)
{
    bcm_stat_counter_database_t database_main;
    int core_idx;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_crps_mgmt_counter_database_control_set_verify(unit, flags, database, control, arg));
    DNXCMN_CORES_ITER(unit, database->core_id, core_idx)
    {
        database_main.core_id = core_idx;
        database_main.database_id = database->database_id;
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_database_control_set(unit, flags, &database_main, control, arg));
    }

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   control get API per database
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit ID
*   \param [in] flags   -  0
*   \param [in] database    -   core_id and database_id
*   \param [in] control   -  control enum
*   \param [in] arg   -  arg

* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   *
* \see
*   * None
*/
int
bcm_dnx_stat_counter_database_control_get(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_database_control_t control,
    uint32 *arg)
{
    bcm_stat_counter_database_t database_main;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_crps_mgmt_counter_database_control_get_verify(unit, flags, database, control, arg));
    database_main.core_id = (database->core_id == BCM_CORE_ALL) ? 0 : database->core_id;
    database_main.database_id = database->database_id;
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_database_control_get(unit, flags, &database_main, control, arg));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

int
bcm_dnx_stat_counter_engine_control_set(
    int unit,
    uint32 flags,
    bcm_stat_engine_t * engine,
    bcm_stat_counter_engine_control_t control,
    uint32 arg)
{
    bcm_stat_engine_t engine_main;
    int core_idx;
    bcm_stat_counter_interface_type_t engine_source;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_crps_mgmt_counter_engine_control_set_verify(unit, flags, engine, control, arg));
    DNXCMN_CORES_ITER(unit, engine->core_id, core_idx)
    {
        engine_main.core_id = core_idx;
        engine_main.engine_id = engine->engine_id;
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.src_type.get(unit, core_idx, engine->engine_id, &engine_source));
        switch (control)
        {
            case bcmStatCounterClearAll:
                /** if both flags set, return error */
                if ((flags & BCM_STAT_COUNTER_CONTROL_CLEAR_SW_ONLY)
                    && (flags & BCM_STAT_COUNTER_CONTROL_CLEAR_HW_ONLY))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "invalid flags config, Not allowed to activate both flags: clear_sw_only and clear_hw_only\n");
                }
                else
                {
                    /** if sw clear only set or none of the clear flags set ==> need to clear the sw*/
                    if ((flags & BCM_STAT_COUNTER_CONTROL_CLEAR_SW_ONLY) ||
                        (((flags & BCM_STAT_COUNTER_CONTROL_CLEAR_SW_ONLY) == FALSE) &&
                         ((flags & BCM_STAT_COUNTER_CONTROL_CLEAR_HW_ONLY) == FALSE)))
                    {
                        /** clear sw memory */
                        SHR_IF_ERR_EXIT(dnx_crps_ctrs_db_counter_clear(unit, core_idx, engine->engine_id));
                    }
                    /** if hw clear only set or none of the clear flags set ==> need to clear the hw*/
                    if ((flags & BCM_STAT_COUNTER_CONTROL_CLEAR_HW_ONLY) ||
                        (((flags & BCM_STAT_COUNTER_CONTROL_CLEAR_SW_ONLY) == FALSE) &&
                         ((flags & BCM_STAT_COUNTER_CONTROL_CLEAR_HW_ONLY) == FALSE)))
                    {
                        /** clear hw memory. */
                        SHR_IF_ERR_EXIT(dnx_crps_engine_hw_counters_reset(unit, &engine_main));
                    }
                }
                break;
            case bcmStatCounterSequentialSamplingInterval:
                /** check for correct source - Eviction is not allowed for source E-OAM  */
                if (engine_source == bcmStatCounterInterfaceEgressOam)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Engine source=%d - there is no eviction.\n", engine_source);
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_crps_eviction_seq_timer_set(unit, &engine_main, arg));
                }
                break;
            case bcmStatCounterEngineEvictionEnable:
                 /** check for correct source - Eviction is not allowed for source E-OAM  */
                if (engine_source == bcmStatCounterInterfaceEgressOam)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Engine source=%d - there is no eviction.\n", engine_source);
                }
                else
                {
                        /** update only enable_eviction and not counting */
                        /** do not update sw state, when it is being called from control API */
                    SHR_IF_ERR_EXIT(dnx_crps_engine_enable_hw_set(unit, &engine_main, (int) arg, -1));
                }
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, " invalid control (=%d)\n", control);
                break;
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

int
bcm_dnx_stat_counter_engine_control_get(
    int unit,
    uint32 flags,
    bcm_stat_engine_t * engine,
    bcm_stat_counter_engine_control_t control,
    uint32 *arg)
{
    bcm_stat_engine_t engine_main;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_crps_mgmt_counter_engine_control_get_verify(unit, flags, engine, control, arg));
    engine_main.core_id = (engine->core_id == BCM_CORE_ALL) ? 0 : engine->core_id;
    engine_main.engine_id = engine->engine_id;
    switch (control)
    {
        case bcmStatCounterClearAll:
            SHR_ERR_EXIT(_SHR_E_PARAM, "engine_control_get API is not allowed for control=bcmStatCounterClearAll \n");
            break;
        case bcmStatCounterSequentialSamplingInterval:
            SHR_IF_ERR_EXIT(dnx_crps_eviction_seq_timer_get(unit, &engine_main, arg));
            break;
        case bcmStatCounterEngineEvictionEnable:
            /** update only enable_eviction and not counting */
            SHR_IF_ERR_EXIT(dnx_crps_engine_enable_hw_get(unit, &engine_main, (int *) arg, NULL));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, " invalid control (=%d)\n", control);
            break;
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;

}

/**
* \brief  
*   API which configure the counter interface parameters of an engine. 
*   it verify the input parameters, update sw state and HW.
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit ID
*   \param [in] flags   -  modify range only flag, in order to indicates that need to refer only to range configuration, 
*                                   which is allowed while engine is active
*   \param [in] database  -  pointer to structure which contain the database_id and core_id
*   \param [in] config  -  structure which gather all the counter interface parameters.
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*    can be called only if engine is fully disable (counting and eviction disabled) , except if "modify_range" flag is set
* \see
*   * None
*/

int
bcm_dnx_stat_counter_interface_set(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_interface_t * config)
{
    bcm_stat_counter_database_t database_main;
    int core_idx;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_crps_mgmt_counter_interface_set_verify(unit, flags, database, config));
    DNXCMN_CORES_ITER(unit, database->core_id, core_idx)
    {
        database_main.core_id = core_idx;
        database_main.database_id = database->database_id;
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_interface_set(unit, flags, &database_main, config));
    }

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   API which get the interface configurationof an engine. 
*   it verify the input parameters, and get the parameters from sw state
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit ID
*   \param [in] flags   -  NONE
*   \param [in] database  -  pointer to structure which contain the database_id and core_id
*   \param [out] config  -  structure which gather all the counter interface parameters.
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * 
* \see
*   * None
*/
int
bcm_dnx_stat_counter_interface_get(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_interface_t * config)
{
    bcm_stat_counter_database_t database_main;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_crps_mgmt_counter_interface_get_verify(unit, flags, database, config));
    database_main.core_id = (database->core_id == BCM_CORE_ALL) ? 0 : database->core_id;
    database_main.database_id = database->database_id;
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_interface_get(unit, flags, &database_main, config));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   API which select the expansion format (10 bits) for each interface.
*   it verify the input parameters, update sw state and HW.
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit ID
*   \param [in] flags   - NONE
*   \param [in] interface  -  pointer to structure which contain the interface params (core, source, command_id, type_id)
*   \param [in] expansion_select  -  structure which gather all the expansion selection configuration
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * 
* \see
*   * None
*/
int
bcm_dnx_stat_counter_expansion_select_set(
    int unit,
    uint32 flags,
    bcm_stat_counter_interface_key_t * interface,
    bcm_stat_expansion_select_t * expansion_select)
{
    bcm_stat_counter_interface_key_t interface_main;
    bcm_stat_expansion_select_t expansion_select_copy;
    int i, core_idx;
    uint32 nof_bits;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_crps_mgmt_counter_expansion_select_set_verify(unit, flags, interface, expansion_select));

    memcpy(&expansion_select_copy, expansion_select, sizeof(bcm_stat_expansion_select_t));
    DNXCMN_CORES_ITER(unit, interface->core_id, core_idx)
    {
        interface_main.core_id = core_idx;
        interface_main.command_id = interface->command_id;
        interface_main.interface_source = interface->interface_source;
        interface_main.type_id = interface->type_id;
        /** Each type in the API has its bitmap, but for some types the bitmap is not relevant (always use all bits of the types). */
        /** For these types, we do not force the user to set the bitmap. Instead, we set it by ourselfs in the driver. */
        /** this will help later in the driver impemantation to cosider the bitmap for all types */
        /** (because all types will have "full" bitmap according to the type size, except the ones that allowed to be not "full" */
        for (i = 0; i < expansion_select->nof_elements; i++)
        {
                /** if the type don't have valid bitmap, ignore the user configuration, and set it in the driver according the nof_bits of the type */
            if ((expansion_select_copy.expansion_elements[i].type != bcmStatExpansionTypeMetaData) &&
                (expansion_select_copy.expansion_elements[i].type != bcmStatExpansionTypePortMetaData))
            {
                SHR_IF_ERR_EXIT(dnx_crps_src_interface_data_field_params_get
                                (unit, core_idx, interface->interface_source, interface->command_id,
                                 expansion_select->expansion_elements[i].type, &nof_bits, NULL));
                SHR_BITSET_RANGE(&expansion_select_copy.expansion_elements[i].bitmap, 0, nof_bits);
            }
        }
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_expansion_select_set
                        (unit, flags, &interface_main, &expansion_select_copy));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   API which get the expansion format (10 bits) selection for each interface.
*   it verify the input parameters, and get the data from HW.
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit ID
*   \param [in] flags   - NONE
*   \param [in] interface  -  pointer to structure which contain the interface params (core, source, command_id, type_id)
*   \param [out] expansion_select  -  structure which gather all the expansion selection configuration
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * 
* \see
*   * None
*/
int
bcm_dnx_stat_counter_expansion_select_get(
    int unit,
    uint32 flags,
    bcm_stat_counter_interface_key_t * interface,
    bcm_stat_expansion_select_t * expansion_select)
{
    bcm_stat_counter_interface_key_t interface_main;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_crps_mgmt_counter_expansion_select_get_verify(unit, flags, interface, expansion_select));
    interface_main.core_id = (interface->core_id == BCM_CORE_ALL) ? 0 : interface->core_id;
    interface_main.command_id = interface->command_id;
    interface_main.interface_source = interface->interface_source;
    interface_main.type_id = interface->type_id;
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_expansion_select_get(unit, flags, &interface_main, expansion_select));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   API which configure the counter set mapping. defines the counter_set size and what each counter in the set represent.
*   it verify the input parameters and update the HW.
* \par DIRECT INPUT: 
*   \param [in] unit             -  unit ID
*   \param [in] flags            -  FULL_SPREAD - will spread all the expansion data in the counter_set.
*   \param [in] database  -  pointer to structure which contain the database_id and core_id
*   \param [in] counter_set_map  -  configuration strucutre
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * the API set also defines the range of the engine (together with parameters that was configure in API bcm_dnx_stat_counter_interface_set)
* \see
*   * None
*/
int
bcm_dnx_stat_counter_set_mapping_set(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_set_map_t * counter_set_map)
{
    bcm_stat_counter_database_t database_main;
    int core_idx;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_crps_mgmt_counter_set_mapping_set_verify(unit, flags, database, counter_set_map));
    DNXCMN_CORES_ITER(unit, database->core_id, core_idx)
    {
        database_main.core_id = core_idx;
        database_main.database_id = database->database_id;
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_set_mapping_set(unit, flags, &database_main, counter_set_map));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   API which get the configuration of the counter set mapping.
* \par DIRECT INPUT: 
*   \param [in] unit             -  unit ID
*   \param [in] flags            -  NONE
*   \param [in] database  -  pointer to structure which contain the database_id and core_id
*   \param [out] counter_set_map  - configuration strucutre
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   *
* \see
*   * None
*/
int
bcm_dnx_stat_counter_set_mapping_get(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_set_map_t * counter_set_map)
{
    bcm_stat_counter_database_t database_main;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_crps_mgmt_counter_set_mapping_get_verify(unit, flags, database, counter_set_map));
    database_main.core_id = (database->core_id == BCM_CORE_ALL) ? 0 : database->core_id;
    database_main.database_id = database->database_id;
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_set_mapping_get(unit, flags, &database_main, counter_set_map));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   API which configure all parameters related to the eviction from the engine.
*   it verify the input parameters, update sw state and HW.
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit ID
*   \param [in] flags   -  NONE
*   \param [in] engine  -  pointer to structure which contain the engine_id and core_id
*   \param [in] eviction  -  structure which gather the eviction parameters
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   1. can be called only if engine is fully disable (counting and eviction disabled) 
*   2. eviction_algorithmic_disable refer to algorithmic or probabilistic eviction - 
*      It depend on which one of the option the device support.
*      (see dnx_data_crps.eviction.probabilistic_is_supported and dnx_data_crps.eviction.algorithmic_is_supported)
*       
* \see
*   * None
*/
int
bcm_dnx_stat_counter_eviction_set(
    int unit,
    uint32 flags,
    bcm_stat_engine_t * engine,
    bcm_stat_eviction_t * eviction)
{
    bcm_stat_engine_t engine_main;
    int core_idx;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_crps_mgmt_counter_eviction_set_verify(unit, flags, engine, eviction));
    DNXCMN_CORES_ITER(unit, engine->core_id, core_idx)
    {
        engine_main.core_id = core_idx;
        engine_main.engine_id = engine->engine_id;
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_eviction_set(unit, flags, &engine_main, eviction));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   API which get the eviction configuration from an engine.
*   it verify the input parameters, and get the arameters from the sw state
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit ID
*   \param [in] flags   -  NONE
*   \param [in] engine  -  pointer to structure which contain the engine_id and core_id
*   \param [out] eviction  -  structure which gather the eviction parameters
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   *
* \see
*   * None
*/
int
bcm_dnx_stat_counter_eviction_get(
    int unit,
    uint32 flags,
    bcm_stat_engine_t * engine,
    bcm_stat_eviction_t * eviction)
{
    bcm_stat_engine_t engine_main;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_crps_mgmt_counter_eviction_get_verify(unit, flags, engine, eviction));
    engine_main.core_id = (engine->core_id == BCM_CORE_ALL) ? 0 : engine->core_id;
    engine_main.engine_id = engine->engine_id;
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_eviction_get(unit, flags, &engine_main, eviction));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *      API get array of counters from sw crps database
 * \param [in] unit -
 *     unit id
 * \param [in] flags -
 *     currently not used
 * \param [in] stat_counter_input_data -
 *     structure of input parameters which dfeine which counter to collect
 * \param [out] stat_counter_output_data -
 *     structure of output counters value array
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
int
bcm_dnx_stat_counter_get(
    int unit,
    uint32 flags,
    bcm_stat_counter_input_data_t * stat_counter_input_data,
    bcm_stat_counter_output_data_t * stat_counter_output_data)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_crps_mgmt_counter_get_verify
                          (unit, flags, stat_counter_input_data, stat_counter_output_data));
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_get(unit, flags, stat_counter_input_data, stat_counter_output_data));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_stat_counter_explicit_get(
    int unit,
    uint32 flags,
    bcm_stat_counter_explicit_input_data_t * stat_counter_input_data,
    bcm_stat_counter_output_data_t * stat_counter_output_data)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_crps_mgmt_counter_explicit_get_verify
                          (unit, flags, stat_counter_input_data, stat_counter_output_data));

    SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_explicit_get(unit, flags, stat_counter_input_data->engine.core_id,
                                                       stat_counter_input_data->engine.engine_id,
                                                       stat_counter_input_data->type_id,
                                                       stat_counter_input_data->object_stat_id,
                                                       stat_counter_input_data->nstat,
                                                       stat_counter_input_data->stat_arr, NULL, NULL,
                                                       stat_counter_output_data));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   API to set sequential boundaries - default or different
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit ID
*   \param [in] flags   -  0 or BCM_STAT_EVICTION_RANGE_ALL
*   \param [in] engine  -  pointer to structure which contain the engine_id and core_id
*   \param [in] boundaries  - pointer to structure which contain
*          start and end of the eviction boundaries
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   *
* \see
*   * None
*/
int
bcm_dnx_stat_eviction_boundaries_set(
    int unit,
    uint32 flags,
    bcm_stat_engine_t * engine,
    bcm_stat_eviction_boundaries_t * boundaries)
{
    bcm_stat_engine_t engine_main;
    int core_idx;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_crps_eviction_boundaries_set_verify(unit, flags, engine, boundaries));
    DNXCMN_CORES_ITER(unit, engine->core_id, core_idx)
    {
        engine_main.core_id = core_idx;
        engine_main.engine_id = engine->engine_id;
        SHR_IF_ERR_EXIT(dnx_crps_eviction_boundaries_set(unit, flags, &engine_main, boundaries));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   API to get sequential boundaries from HW
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit ID
*   \param [in] flags   -  0 or BCM_STAT_EVICTION_RANGE_ALL
*   \param [in] engine  -  pointer to structure which contain the engine_id and core_id
*   \param [out] boundaries  - pointer to structure which
*          contain start and end of the eviction boundaries
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   *
* \see
*   * None
*/
int
bcm_dnx_stat_eviction_boundaries_get(
    int unit,
    uint32 flags,
    bcm_stat_engine_t * engine,
    bcm_stat_eviction_boundaries_t * boundaries)
{
    bcm_stat_engine_t engine_main;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_crps_eviction_boundaries_get_verify(unit, flags, engine, boundaries));
    engine_main.core_id = (engine->core_id == BCM_CORE_ALL) ? 0 : engine->core_id;
    engine_main.engine_id = engine->engine_id;
    SHR_IF_ERR_EXIT(dnx_crps_eviction_boundaries_get(unit, flags, &engine_main, boundaries));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   API creates a counter database - SW only
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit ID
*   \param [in] flags   -  0 or BCM_STAT_DATABASE_CREATE_WITH_ID
*   \param [in] core_id    -  specific core id
*   \param [out] database_id    -  database id, if WITH_ID set by usr, otherwise set by SDK
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   *
* \see
*   * None
*/
int
bcm_dnx_stat_counter_database_create(
    int unit,
    uint32 flags,
    bcm_core_t core_id,
    int *database_id)
{
    int core_idx;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_crps_counter_database_create_verify(unit, flags, core_id, database_id));
    DNXCMN_CORES_ITER(unit, core_id, core_idx)
    {
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_database_create(unit, flags, core_idx, database_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   API destroy a counter database - SW only
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit ID
*   \param [in] flags   -  0 or BCM_STAT_DATABASE_CREATE_WITH_ID
*   \param [in] database    -  structure that gather the core_id and database_id
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   *
* \see
*   * None
*/
int
bcm_dnx_stat_counter_database_destroy(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database)
{
    bcm_stat_counter_database_t database_main;
    int core_idx;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_crps_counter_database_destroy_verify(unit, flags, database));
    DNXCMN_CORES_ITER(unit, database->core_id, core_idx)
    {
        database_main.core_id = core_idx;
        database_main.database_id = database->database_id;
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_database_destroy(unit, flags, &database_main));
    }

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   API attach an engine to  database
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit ID
*   \param [in] flags   -  0 
*   \param [in] database    -  structure that gather the core_id and database_id
*   \param [in] engine_id   -  0 
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   *
* \see
*   * None
*/
int
bcm_dnx_stat_counter_engine_attach(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    int engine_id)
{
    bcm_stat_counter_database_t database_main;
    int core_idx;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_crps_counter_engine_attach_verify(unit, flags, database, engine_id));
    DNXCMN_CORES_ITER(unit, database->core_id, core_idx)
    {
        database_main.core_id = core_idx;
        database_main.database_id = database->database_id;
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_engine_attach(unit, flags, &database_main, engine_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   detach an engine from a database. It is forbidden to detach the base engine of the database using the API
*   Detach is poosible only for the last engine in database
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit ID
*   \param [in] flags   -  0
*   \param [in] database    -  structure that gather the core_id and database_id
*   \param [in] engine_id   -  engine_id
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   *
* \see
*   * None
*/
int
bcm_dnx_stat_counter_engine_detach(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    int engine_id)
{
    int core_idx;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_crps_counter_engine_detach_verify(unit, flags, database, engine_id));
    DNXCMN_CORES_ITER(unit, database->core_id, core_idx)
    {
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_engine_detach(unit, flags, core_idx, engine_id));
    }

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;

}

/**
* \brief  
*   API to set sequential boundaries - default or different
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit ID
*   \param [in] flags   -  0 or BCM_STAT_EVICTION_RANGE_ALL
*   \param [in] database  -  pointer to structure which contain
*          the database_id and core_id
*   \param [in] type_id - relevant type_id for the modification
*   \param [in] boundaries  - pointer to structure which contain
*          start and end of the eviction boundaries in obj stat
*          id resolution within the type id range
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   *
* \see
*   * None
*/
int
bcm_dnx_stat_database_eviction_boundaries_set(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    int type_id,
    bcm_stat_eviction_boundaries_t * boundaries)
{
    bcm_stat_counter_database_t database_main;
    int core_idx;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_crps_database_eviction_boundaries_set_verify(unit, flags, database, type_id, boundaries));
    DNXCMN_CORES_ITER(unit, database->core_id, core_idx)
    {
        database_main.core_id = core_idx;
        database_main.database_id = database->database_id;
        if ((flags & BCM_STAT_EVICTION_RANGE_ALL) == TRUE)
        {
            SHR_IF_ERR_EXIT(dnx_crps_database_eviction_boundaries_range_all_set
                            (unit, &database_main, type_id, boundaries));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_crps_database_eviction_boundaries_set
                            (unit, flags, &database_main, type_id, boundaries));
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   API to get sequential boundaries from HW
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit ID
*   \param [in] flags   -  0 or BCM_STAT_EVICTION_RANGE_ALL
*   \param [in] database  -  pointer to structure which contain
*          the database_id and core_id
*   \param [in] type_id - relevant type_id for the modification
*   \param [out] boundaries  - pointer to structure which
*          contain start and end of the eviction boundaries in
*          obj stat id resolution
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   *
* \see
*   * None
*/
int
bcm_dnx_stat_database_eviction_boundaries_get(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    int type_id,
    bcm_stat_eviction_boundaries_t * boundaries)
{
    bcm_stat_counter_database_t database_main;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_crps_database_eviction_boundaries_get_verify(unit, flags, database, type_id, boundaries));
    database_main.core_id = (database->core_id == BCM_CORE_ALL) ? 0 : database->core_id;
    database_main.database_id = database->database_id;
    SHR_IF_ERR_EXIT(dnx_crps_database_eviction_boundaries_get(unit, flags, &database_main, type_id, boundaries));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}
