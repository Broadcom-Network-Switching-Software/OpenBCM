/** \file crps_verify.c
 * 
 *  crps module gather all functionality that related to the counter/statisitc processor driver for DNX.
 *  crps_verify.c verifies the CRPS APIs functionality for DNX.
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
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_crps.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stat.h>
#include <bcm_int/dnx/stat/crps/crps_src_interface.h>
#include <bcm_int/dnx/stat/crps/crps_mgmt.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include "crps_engine.h"
#include "crps_eviction.h"
#include "crps_mgmt_internal.h"
#include "crps_verify.h"

/**
 * See crps_mgmt.h
*/
shr_error_e
dnx_crps_mgmt_counter_command_id_verify(
    int unit,
    bcm_stat_counter_interface_type_t source,
    int command_id)
{
    const dnx_data_crps_src_interface_command_id_t *values;
    SHR_FUNC_INIT_VARS(unit);
    /** get max value for command_id*/
    values = dnx_data_crps.src_interface.command_id_get(unit, source);
    /** check for correct command_id value*/
    if (command_id < values->base || command_id >= (values->base + values->size))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "  Incorrect command_id=%d\n", command_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   verification engine - engine_id in range and valid core_id
* \par DIRECT INPUT:
*   \param [in] unit    -  Unit-ID
*   \param [in] engine  -  engine configuration
*   \param [in] core_all_valid  -  TRUE - bcm_core_all allowed;
*          FALSE - unique core, bcm_core_all is not allowed
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_crps_mgmt_counter_engine_structure_verify(
    int unit,
    bcm_stat_engine_t * engine,
    int core_all_valid)
{
    SHR_FUNC_INIT_VARS(unit);
    /** verify the engine is valid */
    if (engine->engine_id >= dnx_data_crps.engine.nof_engines_get(unit) || engine->engine_id < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect engine_id=%d, possible values 0 till 21 \n", engine->engine_id);
    }
    /** check for valid core - BCM_CORE_ALL not allowed */
    DNXCMN_CORE_VALIDATE(unit, engine->core_id, core_all_valid);

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_database_structure_verify(
    int unit,
    bcm_stat_counter_database_t * database,
    int core_all_valid)
{
    int core_idx;
    uint8 is_allocated;
    SHR_FUNC_INIT_VARS(unit);

    /** check for valid core - BCM_CORE_ALL is allowed for set/get configuration, but not for reading counters APIs */
    DNXCMN_CORE_VALIDATE(unit, database->core_id, core_all_valid);

    DNXCMN_CORES_ITER(unit, database->core_id, core_idx)
    {
        SHR_IF_ERR_EXIT(dnx_crps_db.database_res.is_allocated(unit, core_idx, database->database_id, &is_allocated));
        /** check that database is allocated */
        if (is_allocated == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "database_id %d core_id %d doesn't exist\n",
                         database->database_id, core_idx);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   verify that given type belongs to the valid types according
*   to the given source
* \par DIRECT INPUT:
*   \param [in] unit    -  Unit-ID
*   \param [in] source  -  interface source
*   \param [in] type    -  type
*   \param [out] valid  -  if it is a valid type
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_crps_mgmt_counter_valid_type_per_source_get(
    int unit,
    bcm_stat_counter_interface_type_t source,
    bcm_stat_expansion_types_t type,
    int *valid)
{
    const dnx_data_crps_expansion_irpp_types_t *irpp_types;
    const dnx_data_crps_expansion_itpp_types_t *itpp_types;
    const dnx_data_crps_expansion_erpp_types_t *erpp_types;
    const dnx_data_crps_expansion_etpp_types_t *etpp_types;
    const dnx_data_crps_expansion_etm_types_t *etm_types;
    const dnx_data_crps_expansion_itm_types_t *itm_types;
    const bcm_stat_expansion_types_t *valid_types;
    int type_index = 0;
    SHR_FUNC_INIT_VARS(unit);
    switch (source)
    {
        case bcmStatCounterInterfaceIngressReceivePp:
        case bcmStatCounterInterfaceIngressOam:
            irpp_types = dnx_data_crps.expansion.irpp_types_get(unit);
            valid_types = irpp_types->valid_types;
            break;
        case bcmStatCounterInterfaceIngressVoq:
            itm_types = dnx_data_crps.expansion.itm_types_get(unit);
            valid_types = itm_types->valid_types;
            break;
        case bcmStatCounterInterfaceIngressTransmitPp:
            itpp_types = dnx_data_crps.expansion.itpp_types_get(unit);
            valid_types = itpp_types->valid_types;
            break;
        case bcmStatCounterInterfaceEgressReceivePp:
            erpp_types = dnx_data_crps.expansion.erpp_types_get(unit);
            valid_types = erpp_types->valid_types;
            break;
        case bcmStatCounterInterfaceEgressReceiveQueue:
            etm_types = dnx_data_crps.expansion.etm_types_get(unit);
            valid_types = etm_types->valid_types;
            break;
        case bcmStatCounterInterfaceEgressTransmitPp:
        case bcmStatCounterInterfaceEgressOam:
            etpp_types = dnx_data_crps.expansion.etpp_types_get(unit);
            valid_types = etpp_types->valid_types;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, " invalid interface source (=%d)\n", source);
            break;
    }
    *valid = 0;
    while (valid_types[type_index] != bcmStatExpansionTypeMax)
    {
        if (valid_types[type_index] == type)
        {
            *valid = 1;
            break;
        }
        type_index++;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   verification of dnx_crps_mgmt_range_config used in API
*   bcm_dnx_stat_counter_interface_set
*   1. find the biggest counter pointer ans check if it fits in
*   the database counter range size
*   2. check for overlapping between type ranges
*   3. check for unused counters
* \par DIRECT INPUT:
*   \param [in] unit    -  Unit-ID
*   \param [in] database  - defined by database_id and core.
*   \param [in] base_engine_id    -  base engine of the database
*   \param [in] config  -  configuration structure
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_crps_mgmt_database_type_ranges_verify(
    int unit,
    bcm_stat_counter_database_t * database,
    int base_engine_id,
    bcm_stat_counter_interface_t * config)
{
    int type_id = 0;
    int next_engine_id = base_engine_id;
    int total_range_size = 0, nof_counters;
    const dnx_data_crps_engine_engines_info_t *engine_info;
    int max_counter_pointer = 0, counter_pointer_per_type = 0, min_counter_pointer = 0x1FFFFFFF;
    int valid_types_values[BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES][3] = { {0} };
                                                                            /** offset/ start / end */
    int nof_valid_values = 0;
    int array_row_i, array_row_j, slap, temp, offset;
    int start, end;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(database, _SHR_E_PARAM, "database");

    /** get total engine space for the database */
    while (next_engine_id != DNX_CRPS_MGMT_ENGINE_INVALID)
    {
        engine_info = dnx_data_crps.engine.engines_info_get(unit, next_engine_id);
        total_range_size += engine_info->nof_counters;
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.
                        next_engine_id.get(unit, database->core_id, next_engine_id, &next_engine_id));
    }

    /** get how many counters in counter entry*/
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_nof_counters_per_entry_get(unit, config->format_type, &nof_counters));
    total_range_size = (total_range_size * nof_counters);

    /** 1.  find the biggest counter pointer and check if it fits in
     *  the database counter range size */

    /** get the biggest counter pointer and the lowest counter pointer */
    /** save the valid values in array for part 2. */
    for (type_id = 0; type_id < BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES; type_id++)
    {
        if (config->type_config[type_id].valid)
        {
            counter_pointer_per_type =
                config->type_config[type_id].end + config->type_config[type_id].object_type_offset;
            if (counter_pointer_per_type > max_counter_pointer)
            {
                max_counter_pointer = counter_pointer_per_type;
            }
            counter_pointer_per_type =
                config->type_config[type_id].start + config->type_config[type_id].object_type_offset;
            if (counter_pointer_per_type < min_counter_pointer)
            {
                min_counter_pointer = counter_pointer_per_type;
            }

            /** save valid type values to array */
            valid_types_values[nof_valid_values][0] = config->type_config[type_id].object_type_offset;
            valid_types_values[nof_valid_values][1] = config->type_config[type_id].start;
            valid_types_values[nof_valid_values][2] = config->type_config[type_id].end;
            nof_valid_values++;
        }
    }
    if (((max_counter_pointer - min_counter_pointer + 1) * config->counter_set_size) > total_range_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     " Out of range values for database_id=%d (core_id=%d). total counters size for the database is %d, which can't contain the min value (%d) and the max value (%d)\n",
                     database->database_id, database->core_id, total_range_size,
                     min_counter_pointer * config->counter_set_size, max_counter_pointer * config->counter_set_size);
    }
    /** 2.  check for overlapping between type ranges   */
    /** first - sort the array ascending according to offset+start value */
    offset = 0;
    start = 1;
    end = 2;
    for (array_row_i = 0; array_row_i < nof_valid_values; array_row_i++)
    {
        for (array_row_j = array_row_i + 1; array_row_j < nof_valid_values; array_row_j++)
        {
            if ((valid_types_values[array_row_i][offset] + valid_types_values[array_row_i][start]) >
                (valid_types_values[array_row_j][offset] + valid_types_values[array_row_j][start]))
            {
                for (slap = 0; slap < 3; slap++)
                {
                    temp = valid_types_values[array_row_i][slap];
                    valid_types_values[array_row_i][slap] = valid_types_values[array_row_j][slap];
                    valid_types_values[array_row_j][slap] = temp;
                }
            }
        }
    }
    /** loop the sorted array and check for overlapping ranges or
     *  unused counters */
    for (type_id = 1; type_id < nof_valid_values; type_id++)
    {
        /** check for overlapping */
        if (((valid_types_values[type_id][offset] + valid_types_values[type_id][start]) <=
             (valid_types_values[type_id - 1][offset] + valid_types_values[type_id - 1][end])))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         " Overlapping ranges for database_id=%d (core_id=%d) between types - range: offset=%d, start=%d, end=%d",
                         database->database_id, database->core_id, valid_types_values[type_id][start],
                         valid_types_values[type_id][end], valid_types_values[type_id][offset]);
        }
        /** print warning for unused counters */
        if (((valid_types_values[type_id][offset] + valid_types_values[type_id][start]) >
             (valid_types_values[type_id - 1][offset] + valid_types_values[type_id - 1][end] + 1)))
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Warning: Unused counters: unused range: start=%d, end=%d\n"),
                       (valid_types_values[type_id - 1][offset] + valid_types_values[type_id - 1][end] + 1),
                       (valid_types_values[type_id][offset] + valid_types_values[type_id][start])));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   verification on APIs bcm_stat_counter_get and
*   bcm_stat_counter_explicit_get valid statistic array values
* \par DIRECT INPUT: 
*   \param [in] unit    -  Unit-ID
*   \param [in] stat_arr  -  the array to be checked
*   \param [in] nstat  -  size of the array
*   \param [in] counter_set_size  -  set size for check
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_crps_mgmt_counter_get_statistic_array_verify(
    int unit,
    int *stat_arr,
    int nstat,
    int counter_set_size)
{
    int stat_arr_id = 0, ctr_set_offset = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(stat_arr, _SHR_E_PARAM, "stat_counter_input_data->stat_arr");

    if (nstat > DNX_CRPS_MGMT_MAX_NSTAT)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "nstat=%d. max allowed=%d",
                     nstat, (DNX_DATA_MAX_CRPS_ENGINE_MAX_COUNTER_SET_SIZE * 2));
    }
    /** {offset,is_bytes} offset should be < counter_set_size */
    for (stat_arr_id = 0; stat_arr_id < nstat; stat_arr_id++)
    {
        ctr_set_offset = _SHR_STAT_COUNTER_STAT_OFFSET_GET(stat_arr[stat_arr_id]);
        if (ctr_set_offset > counter_set_size)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "  Incorrect stat array offset=%d for id=%d\n", ctr_set_offset, stat_arr_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_expansion_select_set_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_interface_key_t * interface,
    bcm_stat_expansion_select_t * expansion_select)
{
    int expansion_element = 0, is_valid_type = 0;
    int engine_id = 0, valid_engine = 0;
    bcm_stat_counter_interface_type_t interface_source;
    int command_id;
    uint32 offset = 0, nof_bits_per_type = 0, nof_bits = 0;
    uint32 metadata_bitmap;
    int bitmap_set_bits, valid_type;
    int core_idx;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(interface, _SHR_E_PARAM, "interface");
    SHR_NULL_CHECK(expansion_select, _SHR_E_PARAM, "expansion_select");

    /** check for valid flags value */
    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect flags=%d\n, no flags supported for this API - set flags to 0", flags);
    }
    /** check for valid core - BCM_CORE_ALL is also allowed */
    DNXCMN_CORE_VALIDATE(unit, interface->core_id, TRUE);
    DNXCMN_CORES_ITER(unit, interface->core_id, core_idx)
    {
        /** check for valid interface source - OAMP, IngressVoq and
         *  EgressReceiveQueue are not supported */
        if (interface->interface_source >= bcmStatCounterInterfaceCount || interface->interface_source < 0
            || interface->interface_source == bcmStatCounterInterfaceEgressReceiveQueue
            || interface->interface_source == bcmStatCounterInterfaceIngressVoq
            || interface->interface_source == bcmStatCounterInterfaceOamp)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         " Incorrect interface_source=%d expansion API is not supported for this source \n",
                         interface->interface_source);
        }
        /** check for valid command id*/
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_command_id_verify
                        (unit, interface->interface_source, interface->command_id));
        /** check for correct type_id value*/
        if (interface->type_id < 0 || interface->type_id >= BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "  Incorrect type_id=%d, possible values from 0 till 3 \n", interface->type_id);
        }
        /** check nof_elements*/
        if (expansion_select->nof_elements < 0 || expansion_select->nof_elements >= bcmStatExpansionTypeMax)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         " Incorrect nof_elements=%d, possible values from 0 till 20, where 0 means no expansion selection \n",
                         expansion_select->nof_elements);
        }
        /** For IngressOam and EgressOam expansion element count is 0
         *  - no expansion */
        if ((interface->interface_source == bcmStatCounterInterfaceIngressOam ||
             interface->interface_source == bcmStatCounterInterfaceEgressOam) && expansion_select->nof_elements != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         " Incorrect nof_elements=%d, possible value for IngressOam/EgressOam source is 0 meaning no expansion selection \n",
                         expansion_select->nof_elements);
        }
        nof_bits = 0;
        for (expansion_element = 0; expansion_element < expansion_select->nof_elements; expansion_element++)
        {
            SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_valid_type_per_source_get
                            (unit, interface->interface_source,
                             expansion_select->expansion_elements[expansion_element].type, &is_valid_type));
            if (is_valid_type)
            {
                    /** count bits*/
                SHR_IF_ERR_EXIT(dnx_crps_src_interface_data_field_params_get
                                (unit, core_idx, interface->interface_source, interface->command_id,
                                 expansion_select->expansion_elements[expansion_element].type, &nof_bits_per_type,
                                 &offset));
                    /** if MetaData is selected -  */
                if (expansion_select->expansion_elements[expansion_element].type == bcmStatExpansionTypeMetaData)
                {
                    metadata_bitmap = expansion_select->expansion_elements[expansion_element].bitmap;
                    SHR_BITCOUNT_RANGE(&metadata_bitmap, bitmap_set_bits, 0, (sizeof(uint32) * 8));
                        /** check if the set bits in the bitmap are in correct range
                         *  nof_bits_per_type holds the max bits that can be set for
                         *  metadata for this source */
                    if (bitmap_set_bits > nof_bits_per_type || bitmap_set_bits <= 0)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     " invalid expansion meta data bitmap=%d for interface source (=%d)\n",
                                     expansion_select->expansion_elements[expansion_element].bitmap,
                                     interface->interface_source);
                    }
                    nof_bits_per_type = bitmap_set_bits;
                }
                nof_bits = nof_bits + nof_bits_per_type;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, " invalid expansion element type=%d for interface source (=%d)\n",
                             expansion_select->expansion_elements[expansion_element].type, interface->interface_source);
            }
        }
        /** check the expansion value*/
        if (nof_bits > dnx_data_crps.expansion.expansion_size_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " More than 10b for expansion selected nof_bits=%d \n", nof_bits);
        }
        /** check that there is no such active engine - cannot change the expansion while the engine is active*/
        for (engine_id = 0; engine_id < dnx_data_crps.engine.nof_engines_get(unit); engine_id++)
        {
            DNX_CRPS_MGMT_ENGINE_VALID_GET(unit, core_idx, engine_id, valid_engine);
            if (valid_engine)
            {
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.src_type.get(unit, core_idx, engine_id, &interface_source));
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.command_id.get(unit, core_idx, engine_id, &command_id));
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.
                                type_hw_valid.get(unit, core_idx, engine_id, interface->type_id, &valid_type));
                if ((interface->interface_source == interface_source) && (interface->command_id == command_id)
                    && valid_type)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 " Active engine already exist on core_id=%d with source=%d , command_id=%d, type_id=%d \n",
                                 core_idx, interface_source, command_id, interface->type_id);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_expansion_select_get_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_interface_key_t * interface,
    bcm_stat_expansion_select_t * expansion_select)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(interface, _SHR_E_PARAM, "interface");
    SHR_NULL_CHECK(expansion_select, _SHR_E_PARAM, "expansion_select");

    /** check for valid flags value */
    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect flags=%d\n no flags are supported for this API, set flags=0", flags);
    }
    /** check for valid core - BCM_CORE_ALL not allowed */
    DNXCMN_CORE_VALIDATE(unit, interface->core_id, TRUE);
    /** check for valid interface source*/
    if (interface->interface_source >= bcmStatCounterInterfaceCount || interface->interface_source < 0
               || interface->interface_source == bcmStatCounterInterfaceEgressReceiveQueue
               || interface->interface_source == bcmStatCounterInterfaceIngressVoq
               || interface->interface_source == bcmStatCounterInterfaceOamp)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     " Incorrect interface_source=%d, expansion API is not supported for IngressVoq, EgressReceiveQueue, OAMP sources \n",
                     interface->interface_source);
    }
    /** check for correct command_id value*/
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_command_id_verify(unit, interface->interface_source, interface->command_id));
    /** check for correct type_id value*/
    if (interface->type_id < 0 || interface->type_id >= BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "  Incorrect type_id=%d, possible values from 0 till 3 \n", interface->type_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   verification on command_id and format for ITM source - not
*   allowed MaxSize and other formats to be configured on the
*   same command id
*  \param [in] unit    -  Unit-ID
*  \param [in] core_id - core_id
*  \param [in] database  - the database id
*  \param [in] command_id  -  the command id that has to be
*         checked
*  \param [in] format  -  the format that has to be checked
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_crps_mgmt_itm_format_command_id_verify(
    int unit,
    int core_id,
    int database,
    int command_id,
    bcm_stat_counter_format_type_t format)
{
    int database_id = 0, engine_id = 0, command_id_check = 0;
    uint8 is_allocated;
    bcm_stat_counter_interface_type_t interface_source;
    bcm_stat_counter_format_type_t format_check;
    SHR_FUNC_INIT_VARS(unit);
    for (database_id = 0; database_id < dnx_data_crps.engine.nof_databases_get(unit); database_id++)
    {
        SHR_IF_ERR_EXIT(dnx_crps_db.database_res.is_allocated(unit, core_id, database_id, &is_allocated));
        /** check that database is allocated */
        if (is_allocated == TRUE && database_id != database)
        {
            SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.get(unit, core_id, database_id, &engine_id));
            SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.src_type.get(unit, core_id, engine_id, &interface_source));
            if (interface_source == bcmStatCounterInterfaceIngressVoq)
            {
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.command_id.get(unit, core_id, engine_id, &command_id_check));
                if (command_id == command_id_check)
                {
                    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.format.get(unit, core_id, engine_id, &format_check));
                    if (format_check != format
                        && (format == bcmStatCounterFormatMaxSize || format_check == bcmStatCounterFormatMaxSize))
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     " Incorrect format=%d, for IngressVoq is not supported to have two databases counting on same command id with different formats (MaxSize / PacketSize), database with format=%d already exist, database_id=%d\n",
                                     format, format_check, database_id);
                    }
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_interface_set_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_interface_t * config)
{
    int type_idx = 0;
    int base_engine, core_idx;
    bcm_stat_counter_database_t database_types_verify;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(database, _SHR_E_PARAM, "database");
    SHR_NULL_CHECK(config, _SHR_E_PARAM, "config");
    SHR_MASK_VERIFY(flags, (BCM_STAT_COUNTER_MODIFY_RANGE_ONLY | BCM_STAT_COUNTER_END_TO_END_LATENCY),
                    _SHR_E_PARAM, "Incorrect flags\n");

    /** check that database created and specific core_id */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_database_structure_verify(unit, database, TRUE));

    /** etpp latency measurements can be made only on specific command_id */
    if ((_SHR_IS_FLAG_SET(flags, BCM_STAT_COUNTER_END_TO_END_LATENCY)) &&
        ((config->command_id != dnx_data_crps.latency.etpp_command_id_get(unit)) ||
         (config->source != bcmStatCounterInterfaceEgressTransmitPp)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "EGRESS_LATENCY flags is valid only for EgressTransmitPp source and command_id=%d \n",
                     dnx_data_crps.latency.etpp_command_id_get(unit));
    }

    DNXCMN_CORES_ITER(unit, database->core_id, core_idx)
    {
        /** check that at least one engine is attached to the database, */
        /** otherwise, you don't have engine that holds the sw_state configuration  */
        SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.get(unit, core_idx, database->database_id, &base_engine));
        if (base_engine == DNX_CRPS_MGMT_ENGINE_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "No engines attached to the database. attach engines before calling this API \n");
        }

        /** check only the range of the type - should be done for all
         *  cases - if flag is set - check only range */
        if ((flags & BCM_STAT_COUNTER_MODIFY_RANGE_ONLY) == FALSE)
        {
            /** verify config params are valid */
            /** check engine source */
            if (config->source >= bcmStatCounterInterfaceCount || config->source < 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect interface_source=%d, possible values 0 untill 9 \n",
                             config->source);
            }
            /** check command id*/
            SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_command_id_verify(unit, config->source, config->command_id));
            /** check valid counter set size*/
            if (config->counter_set_size > dnx_data_crps.engine.max_counter_set_size_get(unit)
                || config->counter_set_size < 1)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect counter_set_size=%d, possible values 1 untill 32 \n",
                             config->counter_set_size);
            }
            /** check valid counter_set_size for OAM source */
            if (DNX_CRPS_MGMT_SOURCE_IS_OAM(unit, config->source)
                && config->counter_set_size != dnx_data_crps.engine.oam_counter_set_size_get(unit))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "  Incorrect counter_set_size=%d, for OAM sources is supported only counter_set_size=%d \n",
                             config->counter_set_size, dnx_data_crps.engine.oam_counter_set_size_get(unit));
            }
            /** check for correct format type */
            if (config->format_type < bcmStatCounterFormatPacketsAndBytes
                || config->format_type >= bcmStatCounterFormatCount)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect format_type=%d\n", config->format_type);

            }
        }
            /** check types - for flag BCM_STAT_COUNTER_MODIFY_RANGE_ONLY
             *  - check only the types are filled correctly */
        for (type_idx = 0; type_idx < BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES; type_idx++)
        {
                /** check valid value */
            if ((config->type_config[type_idx].valid != 0) && (config->type_config[type_idx].valid != 1))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect valid value for type_config=%d \n",
                             config->type_config[type_idx].valid);
            }
            /** the following sources has only one type supported - type=0 */
            if ((type_idx) > 0 && (config->type_config[type_idx].valid == 1) &&
                ((config->source == bcmStatCounterInterfaceIngressVoq) ||
                 (config->source == bcmStatCounterInterfaceEgressReceivePp) ||
                 (config->source == bcmStatCounterInterfaceEgressReceiveQueue)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, " only type_id=0 is supported for source=%d \n", config->source);
            }
                /** make other checks if type is valid */
            if (config->type_config[type_idx].valid == TRUE)
            {
                    /** check for positive start and end values */
                if (config->type_config[type_idx].start < 0 || config->type_config[type_idx].end < 0)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 " Incorrect value for start=%d or end=%d - start and end must be positive \n",
                                 config->type_config[type_idx].start, config->type_config[type_idx].end);
                }
                    /** check for end>=start*/
                if (config->type_config[type_idx].end < config->type_config[type_idx].start)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 " Incorrect value for start=%d or end=%d - end must be bigger (or equal) than start \n",
                                 config->type_config[type_idx].start, config->type_config[type_idx].end);
                }
                    /** if double format - offset must be even*/
                if ((config->format_type == bcmStatCounterFormatDoublePackets
                     || config->format_type == bcmStatCounterFormatMaxSize)
                    && (config->type_config[type_idx].object_type_offset % 2) == 1)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 " Incorrect value for object_type_offset=%d - in double entry offset should be even\n",
                                 config->type_config[type_idx].object_type_offset);
                }
            }
        }
        /** For ITM source - verify the command id and the format as
         *  if regular format/MaxSize was configured on the same
         *  command id - the other cannot be on the same command id */
        if (config->source == bcmStatCounterInterfaceIngressVoq)
        {
            SHR_IF_ERR_EXIT(dnx_crps_mgmt_itm_format_command_id_verify
                            (unit, core_idx, database->database_id, config->command_id, config->format_type));
        }
        database_types_verify.core_id = core_idx;
        database_types_verify.database_id = database->database_id;
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_database_type_ranges_verify(unit, &database_types_verify, base_engine, config));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_interface_get_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_interface_t * config)
{
    int base_engine, core_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(database, _SHR_E_PARAM, "database");
    SHR_NULL_CHECK(config, _SHR_E_PARAM, "config");

    /** check for valid flags value */
    if ((flags != 0) && ((flags & BCM_STAT_COUNTER_MODIFY_RANGE_ONLY) == FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect flags=%d\n", flags);
    }
    /** verify the database is valid */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_database_structure_verify(unit, database, TRUE));
    core_id = (database->core_id == BCM_CORE_ALL) ? 0 : database->core_id;
    SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.get(unit, core_id, database->database_id, &base_engine));
    if (base_engine == DNX_CRPS_MGMT_ENGINE_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No engines attached to the database. can't get the configuration \n");
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_set_mapping_set_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_set_map_t * counter_set_map)
{
    int base_engine_id = 0, core_idx;
    int key_id = 0, is_valid_type = 0, command_id = 0;
    int expansion_element_id = 0, counter_set_size = 0;
    uint32 offset = 0, nof_bits_per_type = 0, max_value_per_type = 0;
    bcm_stat_counter_interface_type_t interface_source;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(database, _SHR_E_PARAM, "database");
    SHR_NULL_CHECK(counter_set_map, _SHR_E_PARAM, "counter_set_map");

    /** check for valid flags value */
    if (flags != 0 && ((flags & BCM_STAT_COUNTER_MAPPING_FULL_SPREAD) == FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect flags=%d\n, supported flags - 0 or BCM_STAT_COUNTER_MAPPING_FULL_SPREAD",
                     flags);
    }
    /** verify database structure */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_database_structure_verify(unit, database, TRUE));

    DNXCMN_CORES_ITER(unit, database->core_id, core_idx)
    {
        /** get the base_engine_id in database */
        SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.get(unit, core_idx, database->database_id, &base_engine_id));

        /** check that at least one engine attached to the database */
        if (base_engine_id == DNX_CRPS_MGMT_ENGINE_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " database_id=%d, core_id=%d do not have any engine attach \n",
                         database->database_id, core_idx);
        }

        /** check that counter_set_size were already configured */
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.counter_set_size.get
                        (unit, core_idx, base_engine_id, &counter_set_size));

        if (counter_set_size == -1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "counter_set_size was not configured yet. call bcm_stat_counter_interface_set API first \n");
        }

        /** verify counter_set_map */
        /** verify valid nof entries */
        if (counter_set_map->nof_entries < 0
            || counter_set_map->nof_entries > dnx_data_crps.engine.data_mapping_table_size_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect nof_entries=%d\n, should be between 0 and %d",
                         counter_set_map->nof_entries, dnx_data_crps.engine.data_mapping_table_size_get(unit));
        }
        /** verify expansion data mapping is not null */
        if (counter_set_map->nof_entries > 0)
        {
            SHR_NULL_CHECK(counter_set_map->expansion_data_mapping, _SHR_E_PARAM, "expansion_data_mapping");
        }
        for (expansion_element_id = 0; expansion_element_id < counter_set_map->nof_entries; expansion_element_id++)
        {
            /** verify correct nof key conditions */
            if (counter_set_map->expansion_data_mapping[expansion_element_id].nof_key_conditions < 0
                || counter_set_map->expansion_data_mapping[expansion_element_id].nof_key_conditions >=
                BCM_STAT_MAX_NOF_EXPANSION_KEY_ELEMENTS)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect nof_key_conditions=%d",
                             counter_set_map->expansion_data_mapping->nof_key_conditions);
            }
            /** verify valid value counter set offset */
            if (counter_set_map->expansion_data_mapping[expansion_element_id].value.counter_set_offset < 0
                || counter_set_map->expansion_data_mapping[expansion_element_id].value.counter_set_offset >
                counter_set_size)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect counter_set_offset=%d, should be between 0 and %d",
                             counter_set_map->expansion_data_mapping[expansion_element_id].value.counter_set_offset,
                             counter_set_size);
            }
            /** verify valid value */
            if (counter_set_map->expansion_data_mapping[expansion_element_id].value.valid != 0
                && counter_set_map->expansion_data_mapping[expansion_element_id].value.valid != 1)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             " Incorrect counter_set_map->expansion_data_mapping->value.valid=%d, should be 0 or 1",
                             counter_set_map->expansion_data_mapping[expansion_element_id].value.valid);
            }
            /** get source and command id*/
            SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.src_type.get(unit, core_idx, base_engine_id, &interface_source));
            SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.command_id.get(unit, core_idx, base_engine_id, &command_id));
            /** For OAM sources - verify that nof_entries is 1, there are
             *  no key conditions and as counter_set_size is 1 - there is
             *  one offset that is valid - offset 0  */
            if (DNX_CRPS_MGMT_SOURCE_IS_OAM(unit, interface_source))
            {
                if (counter_set_map->nof_entries != 1)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 " Incorrect counter_set_map->nof_entries=%d, should be 1 for OAM sources",
                                 counter_set_map->nof_entries);
                }
                if (counter_set_map->expansion_data_mapping[0].nof_key_conditions != 0)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 " Incorrect counter_set_map->expansion_data_mapping[0].nof_key_conditions=%d, should be 0 for OAM sources - no keys configuration",
                                 counter_set_map->expansion_data_mapping[0].nof_key_conditions);
                }
                /** valid=TRUE/FALSE  and offset is verified according to
                 *  counter set size for all sources */
            }
            /** verify valid keys */
            for (key_id = 0; key_id < counter_set_map->expansion_data_mapping[expansion_element_id].nof_key_conditions;
                 key_id++)
            {
                if (counter_set_map->expansion_data_mapping[expansion_element_id].key[key_id].type < 0
                    || counter_set_map->expansion_data_mapping[expansion_element_id].key[key_id].type >=
                    bcmStatExpansionTypeMax)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect key.type=%d for key with id=%d",
                                 counter_set_map->expansion_data_mapping[expansion_element_id].key[key_id].type,
                                 key_id);
                }
                is_valid_type = 0;
                /** verify all key fields belong to the source */
                SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_valid_type_per_source_get
                                (unit, interface_source,
                                 counter_set_map->expansion_data_mapping[expansion_element_id].key[key_id].type,
                                 &is_valid_type));
                if (!is_valid_type)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 " Incorrect key.type=%d for key with id=%d - not valid type for the given source=%d",
                                 counter_set_map->expansion_data_mapping[expansion_element_id].key[key_id].type, key_id,
                                 interface_source);
                }
                /** verify key value fits in the max value allowed for this type */
                SHR_IF_ERR_EXIT(dnx_crps_src_interface_data_field_params_get
                                (unit, core_idx, interface_source, command_id,
                                 counter_set_map->expansion_data_mapping[expansion_element_id].key[key_id].type,
                                 &nof_bits_per_type, &offset));
                max_value_per_type = (1 << nof_bits_per_type) - 1;
                if (counter_set_map->expansion_data_mapping[expansion_element_id].key[key_id].value > max_value_per_type
                    || counter_set_map->expansion_data_mapping[expansion_element_id].key[key_id].value < 0)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 " Incorrect key.value=%d for key with id=%d - does not fit in the type allowed bits",
                                 counter_set_map->expansion_data_mapping[expansion_element_id].key[key_id].value,
                                 key_id);
                }
            }
        }
    }
    /** verify all keys are part of the expansion  - no need because it is checked during the implemantation */
exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_set_mapping_get_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_set_map_t * counter_set_map)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(database, _SHR_E_PARAM, "database");
    SHR_NULL_CHECK(counter_set_map, _SHR_E_PARAM, "counter_set_map");
    /** check for valid flags value */
    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect flags=%d\n, no flags are supported for this API", flags);
    }
    /** verify the engine is valid */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_database_structure_verify(unit, database, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_eviction_set_verify(
    int unit,
    uint32 flags,
    bcm_stat_engine_t * engine,
    bcm_stat_eviction_t * eviction)
{
    int engine_id = 0, valid_engine = 0, started = 0;
    int engine_database_id = 0, database_id_get = 0;
    int core_idx;
    bcm_eviction_record_format_t record_format_get;
    const dnx_data_crps_eviction_condional_action_valid_t *action_valid;
    uint32 temp_action_flags[1], index;
    bcm_stat_counter_format_type_t format;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(engine, _SHR_E_PARAM, "engine");
    SHR_NULL_CHECK(eviction, _SHR_E_PARAM, "eviction");
    SHR_MASK_VERIFY(flags, (BCM_STAT_EVICTION_CONDITIONAL_ENABLE), _SHR_E_PARAM, "Incorrect flags\n");

    /** verify the engine is valid*/
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_engine_structure_verify(unit, engine, TRUE));
    DNXCMN_CORES_ITER(unit, engine->core_id, core_idx)
    {
        /** part of configuration cannot be changed if engine is active */
        DNX_CRPS_MGMT_ENGINE_VALID_GET(unit, core_idx, engine->engine_id, valid_engine);
        if (valid_engine)
        {
            int dma_fifo, eviction_event_id;
            bcm_eviction_destination_type_t eviction_type;
            bcm_eviction_record_format_t record_format;
            /** only the following fields cannot be changed */
            dnx_crps_db.proc.eviction.dma_fifo.get(unit, engine->core_id, engine->engine_id, &dma_fifo);
            dnx_crps_db.proc.eviction.eviction_type.get(unit, engine->core_id, engine->engine_id, &eviction_type);
            dnx_crps_db.proc.eviction.record_format.get(unit, engine->core_id, engine->engine_id, &record_format);
            dnx_crps_db.proc.eviction.eviction_event_id.get(unit, engine->core_id, engine->engine_id,
                                                            &eviction_event_id);
            if ((dma_fifo != eviction->dma_fifo_select) || (eviction_type != eviction->type)
                || (record_format != eviction->record_format) || (eviction_event_id != eviction->eviction_event_id))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Incorrect call of bcm_stat_counter_eviction_set - engine with engine id=%d and core=%d is already active and configuration cannot be changes",
                             engine->engine_id, core_idx);
            }
        }
        else
        {
            /** verify that attach API has already been called as it
             *  is the 1st one in the sequence */
            SHR_IF_ERR_EXIT(dnx_crps_db.proc.config_started.get(unit, core_idx, engine->engine_id, &started));
            if (!started)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             " Incorrect call of bcm_stat_counter_eviction_set - should call bcm_stat_counter_engine_attach first- for engine with engine id=%d and core=%d",
                             engine->engine_id, core_idx);
            }
        }

        /** check eviction parameters*/
        /** verify dma fifo  */
        if (eviction->dma_fifo_select < 0
            || eviction->dma_fifo_select >= dnx_data_crps.eviction.nof_counters_fifo_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect dma fifo=%d \n", eviction->dma_fifo_select);
        }
        /** eviction type */
        if (eviction->type != bcmStatEvictionDestinationLocalHost)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect eviction type=%d \n", eviction->type);
        }
        /** eviction  record format*/
        if (eviction->record_format != bcmStatEvictionRecordFormatPhysical
            && eviction->record_format != bcmStatEvictionRecordFormatLogical)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect eviction record format=%d \n", eviction->record_format);
        }
        /** for local host the format must be physical */
        if (eviction->type == bcmStatEvictionDestinationLocalHost
            && eviction->record_format != bcmStatEvictionRecordFormatPhysical)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         " Incorrect eviction record format=%d, when eviction type is LocalHost format must be Physical \n",
                         eviction->record_format);
        }
        /** check algorithmic eviction is correctly disabled/enabled */
        if (eviction->eviction_algorithmic_disable != 0 && eviction->eviction_algorithmic_disable != 1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         " Incorrect eviction algorithmic disable =%d, valid values - ENABLE=1 / DISABLE=0 \n",
                         eviction->eviction_algorithmic_disable);
        }
        /** check conditional eviction */
        if (_SHR_IS_FLAG_SET(flags, BCM_STAT_EVICTION_CONDITIONAL_ENABLE))
        {
            /** check feature supported*/
            if (dnx_data_crps.eviction.feature_get(unit, dnx_data_crps_eviction_conditional_action_is_supported) == 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             " flag BCM_STAT_EVICTION_CONDITIONAL_ENABLE not supported for this device\n");
            }
            SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.format.get(unit, core_idx, engine->engine_id, &format));
            if (_SHR_IS_FLAG_SET(eviction->cond.action_flags, BCM_STAT_EVICTION_CONDITIONAL_ACTION_RESET_COUNTER) &&
                (format != bcmStatCounterFormatDoublePackets))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "conditional action RESET_COUNTER is not supported for counter format=%d \n",
                             bcmStatCounterFormatDoublePackets);
            }
            /** verify that actions is valid for the current device */
            temp_action_flags[0] = eviction->cond.action_flags;
            index = 0;
            while (temp_action_flags[0] != 0)
            {
                if (SHR_BITGET(temp_action_flags, index))
                {
                    action_valid = dnx_data_crps.eviction.condional_action_valid_get(unit, index);
                    if ((action_valid->valid) == FALSE)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "action %d not supported for this device\n", (1 << index));
                    }
                }
                SHR_BITCLR(temp_action_flags, index);
                index++;
            }
        }

        /** check that all engines belonging to the same database
         *  have the same record format */
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.database_id.get
                        (unit, core_idx, engine->engine_id, &engine_database_id));
        for (engine_id = 0; engine_id < dnx_data_crps.engine.nof_engines_get(unit); engine_id++)
        {
            DNX_CRPS_MGMT_ENGINE_VALID_GET(unit, core_idx, engine->engine_id, valid_engine);
            if (valid_engine)
            {
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.database_id.get
                                (unit, core_idx, engine_id, &database_id_get));
                /** belong to the same DB */
                if (engine_database_id == database_id_get)
                {
                    SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.
                                    record_format.get(unit, core_idx, engine_id, &record_format_get));
                    if (record_format_get != eviction->record_format)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     " Incorrect eviction record format=%d, should be the same for all concatenated engines \n",
                                     eviction->record_format);
                    }
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_eviction_get_verify(
    int unit,
    uint32 flags,
    bcm_stat_engine_t * engine,
    bcm_stat_eviction_t * eviction)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(engine, _SHR_E_PARAM, "engine");
    SHR_NULL_CHECK(eviction, _SHR_E_PARAM, "eviction");
    SHR_MASK_VERIFY(flags, (BCM_STAT_EVICTION_CONDITIONAL_ENABLE), _SHR_E_PARAM, "Incorrect flags\n");

    /** verify the engine is valid */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_engine_structure_verify(unit, engine, TRUE));

    /** check conditional eviction */
    if (_SHR_IS_FLAG_SET(flags, BCM_STAT_EVICTION_CONDITIONAL_ENABLE))
    {
        /** check feature supported*/
        if (dnx_data_crps.eviction.feature_get(unit, dnx_data_crps_eviction_conditional_action_is_supported) == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " flag BCM_STAT_EVICTION_CONDITIONAL_ENABLE not supported for this device\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_database_enable_set_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_enable_t * enable)
{
    int base_engine_id, core_idx;
    bcm_stat_counter_interface_type_t engine_source;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(database, _SHR_E_PARAM, "database");
    SHR_NULL_CHECK(enable, _SHR_E_PARAM, "enable");
    /** check for valid flags value */
    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect flags=%d\n", flags);
    }
    /** verify the database is valid */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_database_structure_verify(unit, database, TRUE));
    DNXCMN_CORES_ITER(unit, database->core_id, core_idx)
    {
        /** get the base_engine_id in database */
        SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.get(unit, core_idx, database->database_id, &base_engine_id));

        /** check that at least one engine attached to the database */
        if (base_engine_id == DNX_CRPS_MGMT_ENGINE_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " database_id=%d, core_id=%d do not have any engine attach \n",
                         database->database_id, core_idx);
        }

        /** verify enable */
        if (enable->enable_counting != TRUE && enable->enable_counting != FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect enable_counting=%d\n, possible values - 0 or 1",
                         enable->enable_counting);
        }
        if (enable->enable_eviction != TRUE && enable->enable_eviction != FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect enable_eviction=%d\n, possible values - 0 or 1",
                         enable->enable_eviction);
        }
        /** verify that eviction cannot be enabled if the source is OAM */
        /** get the source type */
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.src_type.get(unit, core_idx, base_engine_id, &engine_source));
        /** check for correct source - if no eviction, can't read counters E-OAM  */
        if ((engine_source == bcmStatCounterInterfaceEgressOam) && enable->enable_eviction == TRUE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "  Incorrect enable_eviction=%d, for source=%d eviction cannot be enabled\n",
                         enable->enable_eviction, engine_source);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_database_enable_get_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_enable_t * enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(database, _SHR_E_PARAM, "database");
    SHR_NULL_CHECK(enable, _SHR_E_PARAM, "enable");
    /** check for valid flags value */
    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect flags=%d\n", flags);
    }
    /** verify the engine is valid */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_database_structure_verify(unit, database, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_get_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_input_data_t * stat_counter_input_data,
    bcm_stat_counter_output_data_t * stat_counter_output_data)
{
    int engine_id = 0;
    int counter_set_size = 0;
    bcm_stat_counter_database_t database;
    bcm_stat_counter_interface_type_t engine_source;
    dnx_type_config_t type;
    SHR_FUNC_INIT_VARS(unit);

    SHR_MASK_VERIFY(flags, BCM_STAT_COUNTER_CLEAR_ON_READ, _SHR_E_PARAM,
                    "Incorrect flags - only flag for clear on read is supported for this API\n");
    SHR_NULL_CHECK(stat_counter_input_data, _SHR_E_PARAM, "stat_counter_input_data");
    SHR_NULL_CHECK(stat_counter_output_data, _SHR_E_PARAM, "stat_counter_output_data");

    /** verify correct type_id */
    if (stat_counter_input_data->type_id < 0 || stat_counter_input_data->type_id >= BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "  Incorrect type_id=%d, possible values from 0 till 3 \n",
                     stat_counter_input_data->type_id);
    }
    /** check database_id  */
    database.core_id = stat_counter_input_data->core_id;
    database.database_id = stat_counter_input_data->database_id;
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_database_structure_verify(unit, &database, FALSE));

    /** get from base_engine_id the counter set size */
    SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.get(unit, database.core_id, database.database_id, &engine_id));
    /** get the source type */
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.src_type.get(unit, database.core_id, engine_id, &engine_source));
    /** check for correct source - if no eviction, can't read counters (E-OAM)  */
    if ((engine_source == bcmStatCounterInterfaceEgressOam))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "  Incorrect counter_source_type=%d, counter_get API is not supported for that source \n",
                     engine_source);
    }
    /** counter set size */
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.
                    counter_set_size.get(unit, stat_counter_input_data->core_id, engine_id, &counter_set_size));
    /** stat array check */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_get_statistic_array_verify
                    (unit, stat_counter_input_data->stat_arr, stat_counter_input_data->nstat, counter_set_size));
    /** verify the object stat_id is in the type range */
    SHR_IF_ERR_EXIT(dnx_crps_db.database.
                    type.get(unit, database.core_id, database.database_id, stat_counter_input_data->type_id, &type));
    if (stat_counter_input_data->counter_source_id < type.start
        || stat_counter_input_data->counter_source_id > type.end)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "  Incorrect counter_source_id=%d, for type=%d the configured range is start=%d end=%d \n",
                     stat_counter_input_data->counter_source_id, stat_counter_input_data->type_id, type.start,
                     type.end);
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
int
dnx_crps_mgmt_counter_explicit_get_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_explicit_input_data_t * stat_counter_input_data,
    bcm_stat_counter_output_data_t * stat_counter_output_data)
{
    bcm_stat_counter_interface_type_t engine_source;
    int valid, counter_set_size = 0;
    dnx_type_config_t type;
    int database_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_MASK_VERIFY(flags, BCM_STAT_COUNTER_CLEAR_ON_READ, _SHR_E_PARAM,
                    "Incorrect flags - only flag for clear on read is supported for this API\n");
    SHR_NULL_CHECK(stat_counter_input_data, _SHR_E_PARAM, "stat_counter_input_data");
    SHR_NULL_CHECK(stat_counter_output_data, _SHR_E_PARAM, "stat_counter_output_data");

    /** get engine interface configuration - source type */
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.src_type.get(unit, stat_counter_input_data->engine.core_id,
                                                            stat_counter_input_data->engine.engine_id, &engine_source));
    /** check for correct source - if no eviction, can't read counters (E-OAM)  */
    if (engine_source == bcmStatCounterInterfaceEgressOam)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "  Incorrect counter_source_type=%d, counter_explicit_get API is not supported for that source \n",
                     engine_source);
    }
    /** get if engine is valid, connected to local host and has phy record format*/
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_engine_eviction_valid_get(unit, stat_counter_input_data->engine.core_id,
                                                            stat_counter_input_data->engine.engine_id, &valid));
    if (valid == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "invalid engine. core=%d,engine=%d is not valid or not connected to local host or record format!=phy",
                     stat_counter_input_data->engine.core_id, stat_counter_input_data->engine.engine_id);
    }

    /** verify the engine is valid */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_engine_structure_verify(unit, &(stat_counter_input_data->engine), FALSE));
    /** verify correct type_id */
    if (stat_counter_input_data->type_id < 0 || stat_counter_input_data->type_id >= BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "  Incorrect type_id=%d, possible values from 0 till 3 \n",
                     stat_counter_input_data->type_id);
    }
    /** counter set size */
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.counter_set_size.get(unit, stat_counter_input_data->engine.core_id,
                                                                    stat_counter_input_data->engine.engine_id,
                                                                    &counter_set_size));
    /** stat array check */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_get_statistic_array_verify
                    (unit, stat_counter_input_data->stat_arr, stat_counter_input_data->nstat, counter_set_size));
    /** database id */
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.database_id.get(unit, stat_counter_input_data->engine.core_id,
                                                               stat_counter_input_data->engine.engine_id,
                                                               &database_id));
    /** verify the object stat_id is in the type range */
    SHR_IF_ERR_EXIT(dnx_crps_db.database.type.get(unit, stat_counter_input_data->engine.core_id, database_id,
                                                  stat_counter_input_data->type_id, &type));
    if (stat_counter_input_data->object_stat_id < type.start || stat_counter_input_data->object_stat_id > type.end)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "  Incorrect object_stat_id=%d, for type=%d the configured range is start=%d end=%d \n",
                     stat_counter_input_data->object_stat_id, stat_counter_input_data->type_id, type.start, type.end);
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_engine_control_set_verify(
    int unit,
    uint32 flags,
    bcm_stat_engine_t * engine,
    bcm_stat_counter_engine_control_t control,
    uint32 arg)
{
    int valid, core_idx;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(engine, _SHR_E_PARAM, "engine");
    /** verify the engine is valid */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_engine_structure_verify(unit, engine, TRUE));
    DNXCMN_CORES_ITER(unit, engine->core_id, core_idx)
    {
        DNX_CRPS_MGMT_ENGINE_VALID_GET(unit, core_idx, engine->engine_id, valid);
        if (valid == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Engine_id=%d, core_id=%d is not active", engine->engine_id, engine->core_id);
        }

        switch (control)
        {
            case bcmStatCounterClearAll:
                /** check for valid flags value */
                if (flags != 0 && ((flags & BCM_STAT_COUNTER_CONTROL_CLEAR_SW_ONLY) == FALSE)
                    && ((flags & BCM_STAT_COUNTER_CONTROL_CLEAR_HW_ONLY) == FALSE))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 " Incorrect flags=%d\n, supported flag 0 ,BCM_STAT_COUNTER_CONTROL_CLEAR_HW_ONLY, BCM_STAT_COUNTER_ENGINE_CONTROL_CLEAR_HW_ONLY",
                                 flags);
                }
                break;
            case bcmStatCounterSequentialSamplingInterval:
                if (flags != 0)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 " Incorrect flags=%d\n, if control is not bcmStatCounterClearAll - no flags allowed",
                                 flags);
                }
                break;
            case bcmStatCounterEngineEvictionEnable:
                if (flags != 0)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 " Incorrect flags=%d\n, if control is not bcmStatCounterEngineEvictionEnable - no flags allowed",
                                 flags);
                }
                if (arg != 0 && arg != 1)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 " Incorrect arg=%d\n, if control is bcmStatCounterEngineEvictionEnable - enable disable - only arguments 0 or 1 are allowed",
                                 flags);
                }
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, " invalid control (=%d)\n", control);
                break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_database_control_set_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_database_control_t control,
    uint32 arg)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(database, _SHR_E_PARAM, "database");
    /** verify the engine is valid */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_database_structure_verify(unit, database, TRUE));
    switch (control)
    {
        case bcmStatCounterDatabaseClearAll:
            /** check for valid flags value */
            if (flags != 0 && ((flags & BCM_STAT_COUNTER_CONTROL_CLEAR_SW_ONLY) == FALSE)
                && ((flags & BCM_STAT_COUNTER_CONTROL_CLEAR_HW_ONLY) == FALSE))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             " Incorrect flags=%d\n, supported flag 0 ,BCM_STAT_COUNTER_CONTROL_CLEAR_HW_ONLY, BCM_STAT_COUNTER_ENGINE_CONTROL_CLEAR_HW_ONLY",
                             flags);
            }
            break;

        case bcmStatCounterOverflowMode:
            if (flags != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Incorrect flags=%d\n", flags);
            }
            if (arg != BCM_STAT_COUNTER_OVERFLOW_MODE_RESET && arg != BCM_STAT_COUNTER_OVERFLOW_MODE_WRAP_AROUND
                && arg != BCM_STAT_COUNTER_OVERFLOW_MODE_FIXED_AT_MAX)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             " Invalid arg=%d\n, if control is  bcmStatCounterOverflowMode - arg value should be BCM_STAT_COUNTER_OVERFLOW_MODE_RESET / BCM_STAT_COUNTER_OVERFLOW_MODE_WRAP_AROUND / BCM_STAT_COUNTER_OVERFLOW_MODE_FIXED_AT_MAX",
                             arg);
            }
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, " invalid control (=%d)\n", control);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_engine_control_get_verify(
    int unit,
    uint32 flags,
    bcm_stat_engine_t * engine,
    bcm_stat_counter_engine_control_t control,
    uint32 *arg)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(engine, _SHR_E_PARAM, "engine");
    /** check for valid flags value */
    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect flags=%d\n, no flags supported for this API", flags);
    }
    /** verify the engine is valid */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_engine_structure_verify(unit, engine, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_database_control_get_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_database_control_t control,
    uint32 *arg)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(database, _SHR_E_PARAM, "database");
    /** check for valid flags value */
    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect flags=%d\n, no flags supported for this API", flags);
    }
    /** verify the engine is valid */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_database_structure_verify(unit, database, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_eviction_boundaries_set_verify(
    int unit,
    uint32 flags,
    bcm_stat_engine_t * engine,
    bcm_stat_eviction_boundaries_t * boundaries)
{
    int valid_engine = 0, core_idx;
    const dnx_data_crps_engine_engines_info_t *engine_size;
    int nof_counters_per_entry;
    bcm_stat_counter_format_type_t format;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(engine, _SHR_E_PARAM, "engine");
    SHR_NULL_CHECK(boundaries, _SHR_E_PARAM, "boundaries");
    /** check for valid flags value */
    if ((flags != 0) && ((flags & BCM_STAT_EVICTION_RANGE_ALL) == FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect flags=%d\n", flags);
    }
    /** verify the engine is valid */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_engine_structure_verify(unit, engine, TRUE));
    DNXCMN_CORES_ITER(unit, engine->core_id, core_idx)
    {
        /** verify that engine is already active */
        DNX_CRPS_MGMT_ENGINE_VALID_GET(unit, core_idx, engine->engine_id, valid_engine);
        if (!valid_engine)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         " Incorrect call of bcm_stat_eviction_boundaries_set - engine with engine id=%d and core=%d is not yet active and eviction boundaries cannot be changed at this stage",
                         engine->engine_id, core_idx);
        }

        /** verify that boundaries are in the correct range */
        engine_size = dnx_data_crps.engine.engines_info_get(unit, engine->engine_id);

        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.format.get(unit, core_idx, engine->engine_id, &format));
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_nof_counters_per_entry_get(unit, format, &nof_counters_per_entry));
        /** valid start */
        if (boundaries->start < 0 || boundaries->start >= (engine_size->nof_counters * nof_counters_per_entry))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         " Cannot set such boundaries, possible boundaries are in range: 0 untill %d , invalid start=%d \n",
                         engine_size->nof_counters - 1, boundaries->start);
        }
        /** valid end */
        if (boundaries->end < 0 || boundaries->end >= (engine_size->nof_counters * nof_counters_per_entry))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         " Cannot set such boundaries, possible boundaries are in range: 0 untill %d, invalid end=%d \n",
                         engine_size->nof_counters - 1, boundaries->end);
        }
        /** start < end */
        if (boundaries->start > boundaries->end)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         " Cannot set such boundaries, possible boundaries are in range: 0 untill %d, invalid start=%d and end=%d \n",
                         engine_size->nof_counters - 1, boundaries->start, boundaries->end);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_eviction_boundaries_get_verify(
    int unit,
    uint32 flags,
    bcm_stat_engine_t * engine,
    bcm_stat_eviction_boundaries_t * boundaries)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(engine, _SHR_E_PARAM, "engine");
    SHR_NULL_CHECK(boundaries, _SHR_E_PARAM, "boundaries");
    /** check for valid flags value */
    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect flags=%d\n", flags);
    }
    /** verify the engine is valid */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_engine_structure_verify(unit, engine, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
 *  verification bcm_stat_pkt_size_adjust_select_key_t
* \par DIRECT INPUT:
*   \param [in] unit    -  Unit-ID
*   \param [in] key  -  key struct to verify
*   \param [in] select_type - header adjust select type
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_crps_mgmt_pkt_size_adjust_select_key_verify(
    int unit,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_pkt_size_adjust_select_type_t select_type)
{
    SHR_FUNC_INIT_VARS(unit);

    /** check for valid core - BCM_CORE_ALL also allowed */
    DNXCMN_CORE_VALIDATE(unit, key->core_id, TRUE);
    /** verify source */
    if (key->source != bcmStatCounterInterfaceIngressOam && key->source != bcmStatCounterInterfaceIngressReceivePp &&
        key->source != bcmStatCounterInterfaceIngressVoq)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect source=%d \n", key->source);
    }
    /** verify command_id */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_command_id_verify(unit, key->source, key->command_id));
    /** verify type */
    if (select_type != bcmStatPktSizeSelectCounterIngressHeaderTruncate)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect type=%d \n", select_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_ingress_pkt_size_adjust_select_set_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_pkt_size_adjust_select_type_t select_type,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key, _SHR_E_PARAM, "key");
    /** check for valid flags value */
    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect flags=%d\n", flags);
    }
    /** verify enable 0/1 */
    if (enable != TRUE && enable != FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect enable=%d - must be true/false \n", enable);
    }
    /** verify the key parameters are valid */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_pkt_size_adjust_select_key_verify(unit, key, select_type));

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_ingress_pkt_size_adjust_select_get_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_pkt_size_adjust_select_type_t select_type,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key, _SHR_E_PARAM, "key");
    /** check for valid flags value */
    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect flags=%d\n", flags);
    }
    /** verify the key parameters are valid */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_pkt_size_adjust_select_key_verify(unit, key, select_type));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   verification of key bcm_stat_counter_command_id_key_t and
*   filter group
* \par DIRECT INPUT:
*   \param [in] unit    -  Unit-ID
*   \param [in] key  -  key struct to verify
*   \param [in] filter - filter group
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_crps_mgmt_counter_group_filter_key_verify(
    int unit,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_counter_group_filter_t filter)
{
    const dnx_data_stat_drop_reasons_group_drop_reason_index_t *group_index;
    SHR_FUNC_INIT_VARS(unit);

    /** check for valid core - BCM_CORE_ALL also allowed */
    DNXCMN_CORE_VALIDATE(unit, key->core_id, TRUE);
    /** verify source */
    if (key->source != bcmStatCounterInterfaceIngressOam && key->source != bcmStatCounterInterfaceIngressReceivePp &&
        key->source != bcmStatCounterInterfaceIngressVoq)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect source=%d \n", key->source);
    }
    /** verify command_id */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_command_id_verify(unit, key->source, key->command_id));
    /** verify filter group */
    group_index = dnx_data_stat.drop_reasons.group_drop_reason_index_get(unit, filter);
    if (filter < 0 || filter >= bcmStatCounterGroupFilterCount
        || (group_index->index == -1 && filter == bcmStatCounterGroupFilterLatencyDrop))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect filter group=%d\n", filter);
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_filter_group_set_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_counter_group_filter_t filter,
    int is_active)
{

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key, _SHR_E_PARAM, "key");
    /** check for valid flags value */
    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect flags=%d\n", flags);
    }
    /** verify key struct and filter */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_group_filter_key_verify(unit, key, filter));
    /** verify is_active */
    if (is_active != TRUE && is_active != FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect is_active=%d\n - TRUE/FALSE", is_active);
    }
exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_filter_group_get_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_counter_group_filter_t filter,
    int *is_active)
{

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key, _SHR_E_PARAM, "key");
    /** check for valid flags value */
    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect flags=%d\n", flags);
    }
    /** verify key struct and filter group   */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_group_filter_key_verify(unit, key, filter));

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_counter_database_create_verify(
    int unit,
    uint32 flags,
    bcm_core_t core_id,
    int *database_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(database_id, _SHR_E_PARAM, "database_id");
    /** check for valid flags value */
    SHR_MASK_VERIFY(flags, BCM_STAT_DATABASE_CREATE_WITH_ID, _SHR_E_PARAM, "Incorrect flags\n");
    if (core_id == BCM_CORE_ALL && ((flags & BCM_STAT_DATABASE_CREATE_WITH_ID) == FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "core_id=BCM_CORE_ALL allowed only when flag BCM_STAT_DATABASE_CREATE_WITH_ID is set\n");
    }
exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_counter_database_destroy_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database)
{
    int next_engine_id, core_idx;
    dnx_engine_enablers_t enable;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(database, _SHR_E_PARAM, "database");

    /** check for valid flags value */
    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect flags=%d\n", flags);
    }
    /** check that database is allocated and specific core_id */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_database_structure_verify(unit, database, TRUE));

    DNXCMN_CORES_ITER(unit, database->core_id, core_idx)
    {
        SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.get(unit, core_idx, database->database_id, &next_engine_id));
        /** verify that all engines in the database are disabled */
        while (next_engine_id != DNX_CRPS_MGMT_ENGINE_INVALID)
        {
            /** verify that the engine is fully disabled before detach it */
            /** because destroy will detach all engines */
            SHR_IF_ERR_EXIT(dnx_crps_db.proc.enablers.get(unit, core_idx, next_engine_id, &enable));
            if (enable.counting_enable == TRUE || enable.eviction_enable == TRUE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "engine_id=%d which belong to database_id=%d is not fully disabled, cannot destroy the database",
                             next_engine_id, database->database_id);
            }
            SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.next_engine_id.get
                            (unit, core_idx, next_engine_id, &next_engine_id));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_counter_engine_attach_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    int engine_id)
{
    uint8 is_allocated;
    int started, used_for_meter, core_idx;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(database, _SHR_E_PARAM, "database");
    /** check for valid flags value */
    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect flags=%d\n", flags);
    }
    DNXCMN_CORES_ITER(unit, database->core_id, core_idx)
    {
        SHR_IF_ERR_EXIT(dnx_crps_db.database_res.is_allocated(unit, core_idx, database->database_id, &is_allocated));
        if (is_allocated == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "database were not created. (core_id=%d database_id=%d)\n",
                         core_idx, database->database_id);
        }
        /** verify the engine_id is valid */
        if (engine_id >= dnx_data_crps.engine.nof_engines_get(unit) || engine_id < 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect engine_id=%d\n", engine_id);
        }
        /** verify the engine is not active */
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.config_started.get(unit, core_idx, engine_id, &started));
        if (started)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " Engine is already active: core_id=%d, engine_id=%d \n", core_idx, engine_id);
        }
        /** check that the engine is not used for meter. some of the engines has shared memory with meter processor */
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.used_for_meter.get(unit, core_idx, engine_id, &used_for_meter));
        if (used_for_meter == TRUE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " core_id=%d, engine_id=%d is already used for meter \n", core_idx, engine_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_counter_engine_detach_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    int engine_id)
{
    uint8 is_allocated;
    int started;
    int database_id;
    int base_engine, next_engine_id, core_idx;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(database, _SHR_E_PARAM, "database");
    /** check for valid flags value */
    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect flags=%d\n", flags);
    }
    DNXCMN_CORES_ITER(unit, database->core_id, core_idx)
    {
        SHR_IF_ERR_EXIT(dnx_crps_db.database_res.is_allocated(unit, core_idx, database->database_id, &is_allocated));
        if (is_allocated == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "database were not created. (core_id=%d database_id=%d)\n",
                         core_idx, database->database_id);
        }
        /** verify the engine_id is valid */
        if (engine_id >= dnx_data_crps.engine.nof_engines_get(unit) || engine_id < 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect engine_id=%d\n", engine_id);
        }
        /** verify the engine is active - which means attached already */
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.config_started.get(unit, core_idx, engine_id, &started));
        if (started == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " Engine is not active (not attached): core_id=%d, engine_id=%d \n",
                         core_idx, engine_id);
        }
        /** verify that the engine_id belong to the given database */
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.database_id.get(unit, core_idx, engine_id, &database_id));
        if (database_id != database->database_id)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "engine_id does not belong to the given database_id\n");
        }
        /** verify that the base engine is not remove because if removing the base_engine, */
        /** we will not have the database configuration. (scenario of detach all, attach new will fail).    */
        /** only database destroy API detach the base engine */
        SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.get(unit, core_idx, database->database_id, &base_engine));
        if (base_engine == engine_id)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "engine_id=%d is the base engine. It is forbidden to detach the base engine of the database\n",
                         engine_id);
        }
        /** verify that the engine detached is the last engine in database */
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.next_engine_id.get(unit, core_idx, engine_id, &next_engine_id));
        if (next_engine_id != DNX_CRPS_MGMT_ENGINE_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "engine_id=%d is not the last engine in database therefore, cannot be detach\n", engine_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_database_eviction_boundaries_set_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    int type_id,
    bcm_stat_eviction_boundaries_t * boundaries)
{
    int core_idx;
    dnx_type_config_t type;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(database, _SHR_E_PARAM, "database");
    SHR_MASK_VERIFY(flags, BCM_STAT_EVICTION_RANGE_ALL, _SHR_E_PARAM,
                    "Incorrect flags - only flag BCM_STAT_EVICTION_RANGE_ALL is supported for this API\n");
    if (!_SHR_IS_FLAG_SET(flags, BCM_STAT_EVICTION_RANGE_ALL))
    {
        SHR_NULL_CHECK(boundaries, _SHR_E_PARAM, "boundaries");
    }

    /** verify the database is valid */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_database_structure_verify(unit, database, TRUE));
    /** check for correct type_id value*/
    if (type_id < 0 || type_id >= BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "  Incorrect type_id=%d, possible values from 0 till %d\n", type_id,
                     BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES);
    }

    DNXCMN_CORES_ITER(unit, database->core_id, core_idx)
    {
        /** verify type id is set to valid */
        SHR_IF_ERR_EXIT(dnx_crps_db.database.type.get(unit, core_idx, database->database_id, type_id, &type));
        if (type.valid == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " Cannot modify boundaries for type_id=% is set to not be valid \n", type_id);
        }
        /** valid start */
        if (boundaries->start < 0 || boundaries->start > type.end || boundaries->start < type.start)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         " Cannot set such boundaries, possible boundaries are in range: from %d untill %d , invalid start=%d \n",
                         type.start, type.end, boundaries->start);
        }
        /** valid end */
        if (boundaries->end < 0 || boundaries->end > type.end)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         " Cannot set such boundaries, possible boundaries are in range: from %d untill %d , invalid end=%d \n",
                         type.start, type.end, boundaries->end);
        }
        /** start < end */
        if (boundaries->start > boundaries->end)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         " Cannot set such boundaries, possible boundaries are in range:  from %d untill %d, invalid start=%d and end=%d \n",
                         type.start, type.end, boundaries->start, boundaries->end);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_database_eviction_boundaries_get_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    int type_id,
    bcm_stat_eviction_boundaries_t * boundaries)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(database, _SHR_E_PARAM, "database");
    SHR_NULL_CHECK(boundaries, _SHR_E_PARAM, "boundaries");
    /** check for valid flags value */
    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect flags=%d\n", flags);
    }
    /** verify the database is valid */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_database_structure_verify(unit, database, TRUE));
    /** check for correct type_id value*/
    if (type_id < 0 || type_id >= BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "  Incorrect type_id=%d, possible values from 0 till %d\n", type_id,
                     BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES);
    }

exit:
    SHR_FUNC_EXIT;
}
