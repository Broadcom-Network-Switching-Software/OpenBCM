/** \file crps_mgmt.c
 * 
 *  crps module gather all functionality that related to the counter/statisitc processor driver for DNX.
 *  crps_mgmt.c gather the managements CRPS functionality for DNX.
 *  It is external crps file . (it have interfaces with other modules)
 * 
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_CNT

/*
 * Include files.
 * {
 */
#include <soc/sand/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/stat.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_crps.h>
#include <bcm_int/dnx/stat/stat_mgmt.h>
#include <bcm_int/dnx/stat/crps/crps_mgmt.h>
#include <bcm_int/dnx/stat/crps/crps_src_interface.h>
#include "crps_engine.h"
#include "crps_eviction.h"
#include "crps_mgmt_internal.h"
#include <include/bcm_int/dnx/algo/crps/crps_ctrs_db.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/crps_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/crps_eviction_access.h>
#include <include/shared/stat.h>
#include <include/shared/utilex/utilex_integer_arithmetic.h>
#include <include/bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/cosq/cosq_stat.h>
#include <soc/dnxc/dnxc_ha.h>
#include <soc/dnxc/dnxc_time.h>
#include <shared/utilex/utilex_rand.h>
#include <soc/dnxc/swstate/dnxc_sw_state_verifications.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <soc/util.h>
#include <sal/core/boot.h>

/*
 * }
 */

/*
 * DEFINES
 * {
 */

#define CRPS_MGMT_FIRST_SEQUENCE_NUMBER (0)

#define CRPS_MGMT_INVALID_DMA_FIFO (-1)

#define CRPS_MGMT_NETWORK_EVICTION_INIT_PKT_SIZE (3) /*KB*/
#define CRPS_MGMT_NOF_BITS_IN_RATE_SHADOW (48)
/**
 * \brief
 *  calculate the number of maximum counters (from the engine's counter format)
 *  the can locate on the engine
 * \par DIRECT_INPUT:
 * \param [in] unit         - Unit ID
 * \param [in] core_id      -
 * \param [in] engine_id    -
 * \param [in] nof_counters -
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
crps_mgmt_nof_counters_per_allocated_engine_get(
    int unit,
    int core_id,
    int engine_id,
    int *nof_counters)
{
    bcm_stat_counter_format_type_t format_type;
    int counter_size, nof_counters_calc;

    SHR_FUNC_INIT_VARS(unit);

    /** Get the specific engine parameters from SW*/
    dnx_crps_db.proc.interface.format.get(unit, core_id, engine_id, &format_type);
    dnx_crps_db.proc.interface.counter_size.get(unit, core_id, engine_id, &counter_size);

    /** Calculate the nof counters*/
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_nof_counters_per_engine
                    (unit, engine_id, format_type, counter_size, &nof_counters_calc));

    *nof_counters = nof_counters_calc;

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  calculate the number of counters (in stat-ids) locates in the given set-config-array
*  source
*  \par DIRECT INPUT:
*  \param [in] unit -
*  \param [in] set_config_arr - array of dnx_set_config_t structure elements
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*  int
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
static int
crps_mgmt_nof_counters_in_sets_get(
    int unit,
    dnx_set_config_t set_config_arr[DNX_DATA_MAX_CRPS_ENGINE_MAX_NOF_COUNTER_SETS])
{
    int nof_counters = 0;
    int set_idx;

    for (set_idx = 0; set_idx < dnx_data_crps.engine.max_nof_counter_sets_get(unit); set_idx++)
    {
        nof_counters +=
            (set_config_arr[set_idx].end - set_config_arr[set_idx].start + 1) * set_config_arr[set_idx].size;
    }

    return nof_counters;
}

/**
 * \brief
 *  calculate the index of a specific stat-id and offset, in the given engine
 * \par DIRECT_INPUT:
 * \param [in] unit - Unit ID
 * \param [in] core_id -
 * \param [in] engine_id - engine id
 * \param [in] type_id - type id
 * \param [in] stat_object_id - stat object id
 * \param [in] ctr_set_offset - the offset of the counter in the set
 * \param [out] logical_object_id_get - the index of the counter in the database + stat-id-base*set-size
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
crps_mgmt_type_logical_object_id_get(
    int unit,
    int core_id,
    int engine_id,
    int type_id,
    int stat_object_id,
    int ctr_set_offset,
    uint32 *logical_object_id_get)
{
    uint32 type_offset;
    int database_id, set_size;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.database_id.get(unit, core_id, engine_id, &database_id));
    SHR_IF_ERR_EXIT(dnx_crps_db.database.type.offset.get(unit, core_id, database_id, type_id, &type_offset));

    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.counter_set_size.get(unit, core_id, engine_id, &set_size));
    type_offset = type_offset * set_size;

    /** calc logical object id + base normalize of the engine */
    *logical_object_id_get = (stat_object_id) * set_size + ctr_set_offset + type_offset;

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *  calculate the index of a specific stat-id and offset, in the given engine
 * \par DIRECT_INPUT:
 * \param [in] unit - Unit ID
 * \param [in] core_id -
 * \param [in] engine_id -
 * \param [in] stat_object_id - stat object id
 * \param [in] ctr_set_offset - the set of the counter in the counter set
 * \param [out] logical_object_id_get - the index of the given counter in the engine
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
crps_mgmt_set_logical_object_id_get(
    int unit,
    int core_id,
    int engine_id,
    int stat_object_id,
    int ctr_set_offset,
    uint32 *logical_object_id_get)
{
    uint32 logical_object_id = 0;
    uint32 base_set_object_id, last_set_object_id;
    int database_id;
    int set_idx, counter_set_id, set_size;
    int nof_prevuse_counters = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.database_id.get(unit, core_id, engine_id, &database_id));

    SHR_IF_ERR_EXIT(dnx_crps_mgmt_stat_object_id_set_id_get
                    (unit, core_id, database_id, stat_object_id, &counter_set_id));

    for (set_idx = 0; set_idx < dnx_data_crps.engine.max_nof_counter_sets_get(unit); set_idx++)
    {
        /** get the base counter id related to the current set*/
        SHR_IF_ERR_EXIT(dnx_crps_db.database.set.start.get(unit, core_id, database_id, set_idx, &base_set_object_id));
        SHR_IF_ERR_EXIT(dnx_crps_db.database.set.size.get(unit, core_id, database_id, set_idx, &set_size));

        if (set_idx == counter_set_id)
        {
            logical_object_id =
                nof_prevuse_counters + (stat_object_id - base_set_object_id) * set_size + ctr_set_offset;
            break;
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_crps_db.database.set.end.get(unit, core_id, database_id, set_idx, &last_set_object_id));
            nof_prevuse_counters += (base_set_object_id - last_set_object_id) * set_size;
        }

    }

    *logical_object_id_get = logical_object_id;

exit:
    SHR_FUNC_EXIT;
}

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
 *  init resource manager for counter database
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_crps_mgmt_database_res_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    uint32 core_id;

    SHR_FUNC_INIT_VARS(unit);
    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    data.first_element = 0;
    data.nof_elements = dnx_data_crps.engine.nof_engines_get(unit);
    sal_strncpy(data.name, DNX_CRPS_DATABASE_RESOURCE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(dnx_crps_db.database_res.alloc(unit, dnx_data_device.general.nof_cores_get(unit)));
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
    {
        SHR_IF_ERR_EXIT(dnx_crps_db.database_res.create(unit, core_id, &data, NULL));
    }

exit:
    SHR_FUNC_EXIT;

}

/** see .h file */
int
dnx_crps_mgmt_eviction_destination_for_format_type_is_supported(
    int unit,
    bcm_stat_counter_format_type_t format_type,
    bcm_eviction_destination_type_t destination_type)
{
    int i;

    for (i = 0; i < dnx_data_crps.eviction.nof_eviction_destination_types_get(unit); i++)
    {
        if (dnx_data_crps.engine.counter_format_types_get(unit, format_type)->valid_eviction_destinations[i] ==
            destination_type)
        {
            return TRUE;
        }
    }

    return FALSE;
}

static shr_error_e
dnx_crps_mgmt_rate_calculation_info_init(
    int unit)
{

    uint8 is_alloc;
    int core_id, nof_engines, engine_id;
    int max_nof_sub_counters_per_entry, nof_sub_counters_per_engine;

    SHR_FUNC_INIT_VARS(unit);

    nof_engines = dnx_data_crps.engine.nof_engines_get(unit);
    SHR_IF_ERR_EXIT(dnx_crps_max_nof_sub_counters_per_entry_get(unit, &max_nof_sub_counters_per_entry));

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
    {
        /** for each engine allocate array of SW counters, according to the engine_size*SUB_COUNT. */
        /** driver holds 2 uint64 counters for each HW counter (one for packet and one for bytes */
        for (engine_id = 0; engine_id < nof_engines; engine_id++)
        {

            SHR_IF_ERR_EXIT(dnx_crps_db.proc.rate.rate_calculation_enable.set(unit, core_id, engine_id, FALSE));
            SHR_IF_ERR_EXIT(dnx_crps_db.proc.rate.
                            next_seq_counter_idx.set(unit, core_id, engine_id, CRPS_MGMT_INVALID_LOGICAL_STAT_ID));

            /** The maximum number of counters per entry will be always for single counter type */
            nof_sub_counters_per_engine =
                dnx_data_crps.engine.engines_info_get(unit, engine_id)->nof_entries * max_nof_sub_counters_per_entry;

            /** Init the rate calculation shadows sequence number */
            {
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.rate.sequence_num.is_allocated(unit, core_id, engine_id, &is_alloc));
                if (is_alloc == FALSE)
                {
                    SHR_IF_ERR_EXIT(dnx_crps_db.proc.rate.
                                    sequence_num.alloc(unit, core_id, engine_id, nof_sub_counters_per_engine));
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INIT,
                                 "dnx_crps_db.proc.rate.sequence_num.is_allocated=TRUE for unit=%d, core=%d, engine=%d\n",
                                 unit, core_id, engine_id);
                }
            }

            /** Init the rate calculation shadows DBs*/
            {

                SHR_IF_ERR_EXIT(dnx_crps_db.proc.rate.
                                seq_period_shadow.is_allocated(unit, core_id, engine_id, &is_alloc));
                if (is_alloc == FALSE)
                {
                    SHR_IF_ERR_EXIT(dnx_crps_db.proc.rate.
                                    seq_period_shadow.alloc(unit, core_id, engine_id, nof_sub_counters_per_engine));
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INIT,
                                 "dnx_crps_db.proc.rate.seq_period_shadow.is_allocated=TRUE for unit=%d, core=%d, engine=%d\n",
                                 unit, core_id, engine_id);
                }

                SHR_IF_ERR_EXIT(dnx_crps_db.proc.rate.algo_shadow.is_allocated(unit, core_id, engine_id, &is_alloc));
                if (is_alloc == FALSE)
                {
                    SHR_IF_ERR_EXIT(dnx_crps_db.proc.rate.
                                    algo_shadow.alloc(unit, core_id, engine_id, nof_sub_counters_per_engine));
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INIT,
                                 "dnx_crps_db.proc.rate.algo_shadow.is_allocated=TRUE for unit=%d, core=%d, engine=%d\n",
                                 unit, core_id, engine_id);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 *  init sw state datat base
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_crps_mgmt_init_db(
    int unit)
{
    uint8 is_init;
    int nof_cores, core_id, nof_engines, engine_id, set_idx;
    int database_id;
    SHR_FUNC_INIT_VARS(unit);

    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    nof_engines = dnx_data_crps.engine.nof_engines_get(unit);

    SHR_IF_ERR_EXIT(dnx_crps_db.is_init(unit, &is_init));
    /** if init wasn't made => init */
    if (is_init == FALSE)
    {
        SHR_IF_ERR_EXIT(dnx_crps_db.init(unit));
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.alloc(unit, nof_cores, nof_engines));
        SHR_IF_ERR_EXIT(dnx_crps_db.database.alloc(unit, nof_cores, nof_engines));
        DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
        {
            for (engine_id = 0; engine_id < nof_engines; engine_id++)
            {
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.prev_engine_id.set(unit, core_id, engine_id,
                                                                              DNX_CRPS_MGMT_ENGINE_INVALID));
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.next_engine_id.set(unit, core_id, engine_id,
                                                                              DNX_CRPS_MGMT_ENGINE_INVALID));
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.
                                counter_set_size.set(unit, core_id, engine_id, DNX_CRPS_MGMT_ENGINE_FIELDS_INVALID));
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.
                                counter_size.set(unit, core_id, engine_id, DNX_CRPS_MGMT_ENGINE_FIELDS_INVALID));
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.
                                mode.set(unit, core_id, engine_id, DNX_CRPS_MGMT_COUNTER_MODE_DEFAULT));

                SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.eviction_range.
                                start.set(unit, core_id, engine_id, CRPS_MGMT_INVALID_LOGICAL_STAT_ID));
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.eviction_range.
                                end.set(unit, core_id, engine_id, CRPS_MGMT_INVALID_LOGICAL_STAT_ID));

                database_id = engine_id;

                SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.set(unit, core_id, database_id,
                                                                     DNX_CRPS_MGMT_ENGINE_INVALID));

                /** Initialize the counter-set's size */
                for (set_idx = 0; set_idx < dnx_data_crps.engine.max_nof_counter_sets_get(unit); set_idx++)
                {
                    SHR_IF_ERR_EXIT(dnx_crps_db.database.set.size.set
                                    (unit, core_id, database_id, set_idx, DNX_CRPS_MGMT_ENGINE_FIELDS_INVALID));
                }

                SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.dma_fifo.set
                                (unit, core_id, engine_id, CRPS_MGMT_INVALID_DMA_FIFO));
            }
        }
        /** Allocate the relevant counters memory space */
        SHR_IF_ERR_EXIT(dnx_crps_ctrs_db_init(unit));

        /** Initialize resource manager for counter database */
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_database_res_init(unit));

        /** Allocate the relevant counters memory space for rate calculation */
        if (dnx_data_crps.general.feature_get(unit, dnx_data_crps_general_crps_rate_calculation_support))
        {
            SHR_IF_ERR_EXIT(dnx_crps_mgmt_rate_calculation_info_init(unit));
        }

    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INIT, "dnx_crps_db.init=TRUE \n");
    }

exit:
    SHR_FUNC_EXIT;

}

static shr_error_e
dnx_crps_mgmt_crps_probabilistic_scan_low_rate_init(
    int unit)
{

    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_RESERVED_SPARE_GLOBAL_CFG, &entry_handle_id));
    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROBABILISTIC_EVICTION_MODE, INST_SINGLE, 0x3);
    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_crps_max_nof_sub_counters_per_entry_get(
    int unit,
    int *nof_sub_countes_per_entry)
{
    bcm_stat_counter_format_type_t format_type;
    int max_nof_sub_counters_per_entry = 0, nof_sub_counters = 0;
    int current_nof_counters_per_entry = 0, current_nof_sub_counters_per_entry = 0;

    SHR_FUNC_INIT_VARS(unit);

    for (format_type = bcmStatCounterFormatPacketsAndBytes; format_type < bcmStatCounterFormatCount; format_type++)
    {
        /** Skip formats that can't perform eviction to the host */
        if (!dnx_data_crps.engine.counter_format_types_get(unit, format_type)->valid ||
            !dnx_crps_mgmt_eviction_destination_for_format_type_is_supported(unit, format_type,
                                                                             bcmStatEvictionDestinationLocalHost))
        {
            continue;
        }

        current_nof_counters_per_entry =
            dnx_data_crps.engine.counter_format_types_get(unit, format_type)->nof_counters_per_entry;

        nof_sub_counters = DNX_CRPS_MGMT_NOF_SUB_COUNT(unit, format_type);

        current_nof_sub_counters_per_entry = current_nof_counters_per_entry * nof_sub_counters;

        if (current_nof_sub_counters_per_entry > max_nof_sub_counters_per_entry)
        {
            max_nof_sub_counters_per_entry = current_nof_sub_counters_per_entry;
        }
    }

    *nof_sub_countes_per_entry = max_nof_sub_counters_per_entry;

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_crps_mgmt_rate_seq_shadow_set(
    int unit,
    int core_id,
    int engine_id,
    int shadow_idx,
    uint64 shadow_val)
{
    uint8 u8_shadow_value;
    uint64 u64_shadow_value;
    int i;

    SHR_FUNC_INIT_VARS(unit);

    u64_shadow_value = shadow_val;

    for (i = 0; i < CRPS_MGMT_NOF_BITS_IN_RATE_SHADOW / UTILEX_NOF_BITS_IN_CHAR; i++)
    {
        u64_shadow_value = 0;
        COMPILER_64_BITCOPY_RANGE(u64_shadow_value, 0, shadow_val, UTILEX_NOF_BITS_IN_CHAR * i,
                                  UTILEX_NOF_BITS_IN_CHAR);

        u8_shadow_value = COMPILER_64_LO(u64_shadow_value);

        SHR_IF_ERR_EXIT(dnx_crps_db.proc.rate.
                        seq_period_shadow.set(unit, core_id, engine_id, shadow_idx, i, u8_shadow_value));
    }

exit:
    SHR_FUNC_EXIT;

}

shr_error_e
dnx_crps_mgmt_rate_seq_shadow_get(
    int unit,
    int core_id,
    int engine_id,
    int shadow_idx,
    uint64 *shadow_val)
{
    uint8 u8_shadow_value = 0;
    uint64 u64_shadow_value = 0, tmp = 0;
    int i;

    SHR_FUNC_INIT_VARS(unit);

    for (i = 0; i < CRPS_MGMT_NOF_BITS_IN_RATE_SHADOW / UTILEX_NOF_BITS_IN_CHAR; i++)
    {
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.rate.
                        seq_period_shadow.get(unit, core_id, engine_id, shadow_idx, i, &u8_shadow_value));

        COMPILER_64_SET(tmp, 0, u8_shadow_value);

        COMPILER_64_BITCOPY_RANGE(u64_shadow_value, UTILEX_NOF_BITS_IN_CHAR * i, tmp, 0, UTILEX_NOF_BITS_IN_CHAR);
    }

    *shadow_val = u64_shadow_value;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_crps_mgmt_rate_algo_shadow_set(
    int unit,
    int core_id,
    int engine_id,
    int shadow_idx,
    uint64 shadow_val)
{

    uint8 u8_shadow_value;
    uint64 u64_shadow_value;
    int i;

    SHR_FUNC_INIT_VARS(unit);

    u64_shadow_value = shadow_val;

    for (i = 0; i < CRPS_MGMT_NOF_BITS_IN_RATE_SHADOW / UTILEX_NOF_BITS_IN_CHAR; i++)
    {
        u64_shadow_value = 0;
        COMPILER_64_BITCOPY_RANGE(u64_shadow_value, 0, shadow_val, UTILEX_NOF_BITS_IN_CHAR * i,
                                  UTILEX_NOF_BITS_IN_CHAR);

        u8_shadow_value = COMPILER_64_LO(u64_shadow_value);

        SHR_IF_ERR_EXIT(dnx_crps_db.proc.rate.
                        algo_shadow.set(unit, core_id, engine_id, shadow_idx, i, u8_shadow_value));
    }
exit:
    SHR_FUNC_EXIT;

}

shr_error_e
dnx_crps_mgmt_rate_algo_shadow_get(
    int unit,
    int core_id,
    int engine_id,
    int shadow_idx,
    uint64 *shadow_val)
{

    uint8 u8_shadow_value = 0;
    uint64 u64_shadow_value = 0, tmp = 0;
    int i;

    SHR_FUNC_INIT_VARS(unit);

    for (i = 0; i < CRPS_MGMT_NOF_BITS_IN_RATE_SHADOW / UTILEX_NOF_BITS_IN_CHAR; i++)
    {
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.rate.
                        algo_shadow.get(unit, core_id, engine_id, shadow_idx, i, &u8_shadow_value));

        COMPILER_64_SET(tmp, 0, u8_shadow_value);

        COMPILER_64_BITCOPY_RANGE(u64_shadow_value, UTILEX_NOF_BITS_IN_CHAR * i, tmp, 0, UTILEX_NOF_BITS_IN_CHAR);
    }

    *shadow_val = u64_shadow_value;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *      get from HW whether or not the no_eviction conditional action enable
 * \param [in] unit - unit id
 * \param [in] core_id - one core at a time only
 * \param [in] engine_id - engine_id
 * \param [out] arg - TRUE/FALSE
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e
dnx_crps_mgmt_conditional_action_no_eviction_get(
    int unit,
    int core_id,
    int engine_id,
    uint32 *arg)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_CONDITIONAL_EVICTION_CFG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_ID, engine_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_ACTION_NO_EVICTION, INST_SINGLE, arg);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *      get from HW whether or not the conditional action enable
 * \param [in] unit - unit id
 * \param [in] core_id - one core at a time only
 * \param [in] engine_id - engine_id
 * \param [out] arg - TRUE/FALSE
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e
dnx_crps_mgmt_conditional_action_enable_get(
    int unit,
    int core_id,
    int engine_id,
    uint32 *arg)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_CONDITIONAL_EVICTION_CFG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_ID, engine_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, arg);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 *  init hw fields
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_crps_mgmt_init_hw(
    int unit)
{
    const dnx_data_crps_src_interface_command_id_t *itm_command_ids;

    SHR_FUNC_INIT_VARS(unit);

    /** init etpp metadata selection. empty selection in the beginning */
    SHR_IF_ERR_EXIT(dnx_crps_src_interface_dynamic_meta_data_selection_init(unit));

    /** init itm counter pointer settings */
    itm_command_ids = dnx_data_crps.src_interface.command_id_get(unit, bcmStatCounterInterfaceIngressVoq);
    SHR_IF_ERR_EXIT(dnx_cosq_stat_config_init(unit, itm_command_ids->size));

    /** init ingress compensation masks irpp and itm */
    SHR_IF_ERR_EXIT(dnx_crps_src_interface_compensation_init(unit));
    /** init filter drop reason reject masks */
    SHR_IF_ERR_EXIT(dnx_stat_mgmt_reject_filter_masks_init(unit));

    /** init dummy reply mechanism for OAM */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_oam_dummy_reply_mechanism_init(unit));

    /** init IRE eviction packing watchdog */
    if (dnx_data_crps.eviction.supported_eviction_destination_get(unit, bcmStatEvictionDestinationNetwork)->valid)
    {
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_network_eviction_pkt_size_set(unit, CRPS_MGMT_NETWORK_EVICTION_INIT_PKT_SIZE));
    }

    /** enable the probabilistic scan rate fix if supported*/
    if (dnx_data_crps.eviction.feature_get(unit, dnx_data_crps_eviction_low_rate_probabilistic_eviction))
    {
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_crps_probabilistic_scan_low_rate_init(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *      Power down CRPS and MCP modules
 * \param [in] unit - unit id
 * \param [in] power_down - is power down
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
static shr_error_e
dnx_crps_power_down_set(
    int unit,
    int power_down)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

   /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_SETTINGS, &entry_handle_id));

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_POWER_DOWN, INST_SINGLE, power_down);

   /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_crps_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_crps.general.feature_get(unit, dnx_data_crps_general_is_used))
    {
        /** crps init runs in both warm boot and cold boot */
        /** during warm-boot. only the eviction init should run */
        /** init sw state db */
        if (!sw_state_is_warm_boot(unit))
        {
            SHR_IF_ERR_EXIT(dnx_crps_mgmt_init_db(unit));
            SHR_IF_ERR_EXIT(dnx_crps_mgmt_init_hw(unit));
        }

        /** eviction sw state do not hold warmboot. Need to init it every init call. */
        SHR_IF_ERR_EXIT(dnx_crps_eviction_db.init(unit));
        SHR_IF_ERR_EXIT(dnx_crps_eviction_db.fifo_results.cnt_result.alloc(unit));
        SHR_IF_ERR_EXIT(dnx_crps_eviction_db.fifo_lock.create(unit));
        SHR_IF_ERR_EXIT(dnx_crps_eviction_db.cache_lock.create(unit));
    }
    else
    {
        if (dnx_data_crps.general.feature_get(unit, dnx_data_crps_general_power_down_supported)
            && !sw_state_is_warm_boot(unit))
        {
            SHR_IF_ERR_EXIT(dnx_crps_power_down_set(unit, 1));
        }
    }
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *      Initialize the next_expected_counter_idx for each active engine
 * \param [in] unit - unit id
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
static shr_error_e
dnx_crps_mgmt_rate_calculation_warmboot_deinit(
    int unit)
{
    int engine_id, nof_engines, core_id;
    int eviction_valid, eviction_enable, is_rate_calculation_enabled;
    SHR_FUNC_INIT_VARS(unit);

    nof_engines = dnx_data_crps.engine.nof_engines_get(unit);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
    {
        /** for each engine, expect the next sequential record  */
        for (engine_id = 0; engine_id < nof_engines; engine_id++)
        {
            SHR_IF_ERR_EXIT(dnx_crps_mgmt_engine_eviction_valid_get(unit, core_id, engine_id, &eviction_valid));

            if (eviction_valid)
            {
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.enablers.
                                eviction_enable.get(unit, core_id, engine_id, &eviction_enable));

                SHR_IF_ERR_EXIT(dnx_crps_db.proc.rate.rate_calculation_enable.get(unit, core_id, engine_id,
                                                                                  &is_rate_calculation_enabled));

                if (eviction_enable && is_rate_calculation_enabled)
                {
                    SHR_IF_ERR_EXIT(dnx_crps_rate_calculation_expected_sequential_counter_idx_init
                                    (unit, core_id, engine_id));
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;

}

/** See .h file */
shr_error_e
dnx_crps_deinit(
    int unit)
{
    uint8 is_init = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_CONT(dnx_crps_db.is_init(unit, &is_init));

    /** if init => deinit */
    if (is_init == TRUE)
    {
        if (sw_state_is_warm_boot(unit) &&
            dnx_data_crps.general.feature_get(unit, dnx_data_crps_general_crps_rate_calculation_support))
        {
            SHR_IF_ERR_EXIT(dnx_crps_mgmt_rate_calculation_warmboot_deinit(unit));
        }

        /** first free the sw counters memory */
        SHR_IF_ERR_CONT(dnx_crps_ctrs_db_deinit(unit));

        /*
         * sw state module deinit is done automatically at device deinit
         */
    }

exit:
    SHR_FUNC_EXIT;
}

/** see crps_mgmt_internal.h file */
shr_error_e
dnx_crps_mgmt_nof_counters_in_counter_shaddow_get(
    int unit,
    bcm_stat_counter_format_type_t format_type,
    int *nof_counters)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(nof_counters, _SHR_E_INTERNAL, "nof_counters");

    if ((format_type == bcmStatCounterFormatDoublePackets) || (format_type == bcmStatCounterFormatMaxSize) ||
        (format_type == bcmStatCounterFormatIngressLatency) || (format_type == bcmStatCounterFormatSlimPackets) ||
        (format_type == bcmStatCounterFormatDoubleBytes))
    {
        *nof_counters = 2;
    }
    else
    {
        *nof_counters = 1;
    }

exit:
    SHR_FUNC_EXIT;
}

/** see crps_mgmt_internal.h file */
shr_error_e
dnx_crps_mgmt_nof_counters_per_engine(
    int unit,
    int engine_id,
    bcm_stat_counter_format_type_t format_type,
    int counter_size,
    int *nof_counters)
{
    int nof_entries, nof_counters_per_entry;
    int nof_full_counters_per_entry, nof_state_counters_per_full_counter;
    SHR_FUNC_INIT_VARS(unit);

    *nof_counters = 0;

    if (format_type < bcmStatCounterFormatPacketsAndBytes || format_type >= bcmStatCounterFormatCount)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, " Incorrect format_type=%d\n", format_type);
    }

    if (!dnx_data_crps.engine.counter_format_types_get(unit, format_type)->valid)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, " Incorrect format_type=%d\n", format_type);
    }

    nof_entries = dnx_data_crps.engine.engines_info_get(unit, engine_id)->nof_entries;

    if (DNX_CRPS_COUNTER_FORMAT_TYPE_IS_STATE(format_type))
    {
        nof_full_counters_per_entry =
            dnx_data_crps.engine.counter_format_types_get(unit, bcmStatCounterFormatPackets)->nof_counters_per_entry;

        nof_state_counters_per_full_counter =
            dnx_data_crps.state_counters.state_counter_data_size_per_full_counter_get(unit) / counter_size;

        nof_counters_per_entry = nof_full_counters_per_entry * nof_state_counters_per_full_counter;
    }
    else
    {
        nof_counters_per_entry =
            dnx_data_crps.engine.counter_format_types_get(unit, format_type)->nof_counters_per_entry;
    }

    *nof_counters = nof_entries * nof_counters_per_entry;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *      whether or not the rate calculation feature is enable
 * \param [in] unit - unit id
 * \param [in] core_id - one core at a time only
 * \param [in] engine_id - engine_id
 * \param [out] is_enable - TRUE/FALSE
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e
dnx_crps_eviction_rate_calculation_is_enable(
    int unit,
    int core_id,
    int engine_id,
    int *is_enable)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_crps.general.feature_get(unit, dnx_data_crps_general_crps_rate_calculation_support))
    {
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.rate.rate_calculation_enable.get(unit, core_id, engine_id, is_enable));
    }
    else
    {
        *is_enable = FALSE;
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *      search the first engine of the database.  Function get a current engine and go back to the prev engine connected until getting to the first
 *      one. IN addition, it count the size of counters in all prev engines
 * \param [in] unit - unit id
 * \param [in] engine -engine_id and core
 * \param [out] total_nof_prev_counters - total number of counters, in all the previous engines.
 * \param [out] basic_offset - first logical object id in the database
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
static shr_error_e
dnx_crps_mgmt_base_engine_get(
    int unit,
    bcm_stat_engine_t * engine,
    uint32 *total_nof_prev_counters,
    uint32 *basic_offset)
{
    int prev_engine, cur_engine = engine->engine_id;
    int range_size = 0, nof_counters_in_engine;
    uint32 first_index = 0;
    SHR_FUNC_INIT_VARS(unit);

    while (cur_engine != DNX_CRPS_MGMT_ENGINE_INVALID)
    {
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.prev_engine_id.get(unit, engine->core_id, cur_engine, &prev_engine));
        if (prev_engine != -1)
        {
            SHR_IF_ERR_EXIT(crps_mgmt_nof_counters_per_allocated_engine_get
                            (unit, engine->core_id, prev_engine, &nof_counters_in_engine));
            range_size += nof_counters_in_engine;

            /** find the index in the database of the engine's base counter. */
            SHR_IF_ERR_EXIT(dnx_crps_db.proc.log_obj_id_range.logical_object_id_first.get
                            (unit, engine->core_id, prev_engine, &first_index));
        }
        cur_engine = prev_engine;
    }

    *total_nof_prev_counters = range_size;
    *basic_offset = first_index;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *      update the given set_config array with invalid set parameters
 * \param [in] start - the index of the first set_config element to change
 * \param [in] end - the index of the last set_config element to change
 * \param [out] set_config_arr - array to change
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
static void
dnx_crps_mgmt_set_config_invalid_set(
    int start,
    int end,
    dnx_set_config_t set_config_arr[DNX_DATA_MAX_CRPS_ENGINE_MAX_NOF_COUNTER_SETS])
{
    int set_idx;

    for (set_idx = start; set_idx < end; set_idx++)
    {
        set_config_arr[set_idx].start = CRPS_MGMT_INVALID_LOGICAL_STAT_ID;
        set_config_arr[set_idx].end = CRPS_MGMT_INVALID_LOGICAL_STAT_ID;
        set_config_arr[set_idx].size = 0;
    }
}

/**
 * \brief
 *      for each engine in the given database, calculate the sets bounderies (in stat-object-ids)
 * \param [in] unit -
 * \param [in] core_id - database's core id
 * \param [in] database_id - database id
 * \param [out] set_config_arr - array contains the set_config structure for each engine
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
static shr_error_e
dnx_crps_mgmt_sets_range_get(
    int unit,
    int core_id,
    int database_id,
    dnx_set_config_t
    set_config_arr[DNX_DATA_MAX_CRPS_ENGINE_NOF_ENGINES][DNX_DATA_MAX_CRPS_ENGINE_MAX_NOF_COUNTER_SETS])
{
    dnx_cprs_mgmt_range_t current_range_reminders;
    dnx_set_config_t set;
    int engine_id, set_id;
    int engine_size;
    int set_idx;
    int engine_size_not_allocated, nof_counters_in_engine;
    int max_nof_sets;

    SHR_FUNC_INIT_VARS(unit);

    max_nof_sets = dnx_data_crps.engine.max_nof_counter_sets_get(unit);

    set_id = 0;
    SHR_IF_ERR_EXIT(dnx_crps_db.database.set.get(unit, core_id, database_id, set_id, &set));
    current_range_reminders.start = set.start;
    current_range_reminders.end = set.end;

    SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.get(unit, core_id, database_id, &engine_id));

    while (engine_id != DNX_CRPS_MGMT_ENGINE_INVALID)
    {
        SHR_IF_ERR_EXIT(crps_mgmt_nof_counters_per_allocated_engine_get
                        (unit, core_id, engine_id, &nof_counters_in_engine));
        engine_size = nof_counters_in_engine;
        engine_size_not_allocated = engine_size;

        /*
         * Set all the engine-sets before the current set to invalid 
         */
        set_idx = 0;
        dnx_crps_mgmt_set_config_invalid_set(set_idx, set_id, set_config_arr[engine_id]);
        set_idx = set_id;

        while (set_idx < max_nof_sets)
        {
            set_config_arr[engine_id][set_idx].start = current_range_reminders.start;
            set_config_arr[engine_id][set_idx].size = set.size;

            /*
             * The current set ends in this engine 
             */
            if ((current_range_reminders.end - current_range_reminders.start) * set.size <= engine_size_not_allocated)
            {
                set_config_arr[engine_id][set_idx].end = current_range_reminders.end;

                engine_size_not_allocated =
                    engine_size_not_allocated - (set_config_arr[engine_id][set_idx].end -
                                                 set_config_arr[engine_id][set_idx].start) * set.size;

                /*
                 * get the next set parameters
                 */
                set_id++;
                set_idx++;

                if (set_id < max_nof_sets)
                {
                    SHR_IF_ERR_EXIT(dnx_crps_db.database.set.get(unit, core_id, database_id, set_id, &set));
                    current_range_reminders.start = set.start;
                    current_range_reminders.end = set.end;
                }

                /*
                 * if engine end or all the sets were allocated 
                 */
                if (engine_size_not_allocated == 0 || set_id >= max_nof_sets)
                {
                    break;
                }

            }

            /*
             * The current set end in one of the next engines
             */
            else
            {
                set_config_arr[engine_id][set_idx].end =
                    set_config_arr[engine_id][set_idx].start + engine_size_not_allocated / set.size;
                current_range_reminders.start = set_config_arr[engine_id][set_idx].end + 1;

                set_idx++;
                break;
            }
        }

        /*
         * All the reminds sets in the engine does not relevant
         */
        dnx_crps_mgmt_set_config_invalid_set(set_idx, max_nof_sets, set_config_arr[engine_id]);

        /*
         * Get the next engine id
         */
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.next_engine_id.get(unit, core_id, engine_id, &engine_id));
    }

exit:
    SHR_FUNC_EXIT;

}

/**
* \brief
*   set sw state structure log_obj_id_range for multi-sets database
*   the start will be alwaize 0 and the end will be the number of counters configures to count in the engine
* \par DIRECT INPUT:
*   \param [in] unit    -  unit id
*   \param [in] database  -  structure of database id and core id.
*   \param [in] set_config  -  2D array holds for each engine's sets the size and the stat-ids range.
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
dnx_crps_mgmt_datatbase_logicel_range_sw_set(
    int unit,
    bcm_stat_counter_database_t * database,
    dnx_set_config_t set_config[DNX_DATA_MAX_CRPS_ENGINE_NOF_ENGINES][DNX_DATA_MAX_CRPS_ENGINE_MAX_NOF_COUNTER_SETS])
{
    int curr_engine_id;
    int nof_prevuse_sets_counters = 0;
    int nof_counters_in_engine, curr_log_obj_id_last, curr_log_obj_id_first;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_crps_db.database.
                    base_engine.get(unit, database->core_id, database->database_id, &curr_engine_id));

    while (curr_engine_id != DNX_CRPS_MGMT_ENGINE_INVALID)
    {
        SHR_IF_ERR_EXIT(crps_mgmt_nof_counters_per_allocated_engine_get
                        (unit, database->core_id, curr_engine_id, &nof_counters_in_engine));

        curr_log_obj_id_first = (nof_counters_in_engine > 0) ? nof_prevuse_sets_counters : 0;
        curr_log_obj_id_last =
            (nof_counters_in_engine > 0) ? (nof_prevuse_sets_counters + nof_counters_in_engine - 1) : 0;

        SHR_IF_ERR_EXIT(dnx_crps_db.proc.log_obj_id_range.logical_object_id_first.set
                        (unit, database->core_id, curr_engine_id, curr_log_obj_id_first));
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.log_obj_id_range.logical_object_id_last.set
                        (unit, database->core_id, curr_engine_id, curr_log_obj_id_last));

        nof_prevuse_sets_counters += nof_counters_in_engine;

        /** Get the next engine id*/
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.
                        next_engine_id.get(unit, database->core_id, curr_engine_id, &curr_engine_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *      for each engine in multi-sets database,
 *      enable the first type and update the offset_type field to the number of stat-ids used in the engine
 * \param [in] unit -
 * \param [in] database - database_id and core_id
 * \param [in] set_config_arr - array holds the sets size and stat-ids range per engine
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
static shr_error_e
dnx_crps_mgmt_first_type_config(
    int unit,
    bcm_stat_counter_database_t * database,
    dnx_set_config_t
    set_config_arr[DNX_DATA_MAX_CRPS_ENGINE_NOF_ENGINES][DNX_DATA_MAX_CRPS_ENGINE_MAX_NOF_COUNTER_SETS])
{
    int curr_engine_id, type_id;
    int first_type_id = 0;
    int type_valid[BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES] = { FALSE };
    uint32 type_offset[BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES] = { 0 };
    bcm_stat_engine_t engine;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_crps_db.database.
                    base_engine.get(unit, database->core_id, database->database_id, &curr_engine_id));

    /** Enable the first type only */
    type_valid[first_type_id] = TRUE;

    while (curr_engine_id != DNX_CRPS_MGMT_ENGINE_INVALID)
    {
        engine.engine_id = curr_engine_id;
        engine.core_id = database->core_id;

        for (type_id = 0; type_id < BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES; type_id++)
        {
            SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.
                            type_hw_valid.set(unit, engine.core_id, engine.engine_id, type_id, type_valid[type_id]));
        }

        /** first_type offset = number of counters in the first set*/
        type_offset[first_type_id] = crps_mgmt_nof_counters_in_sets_get(unit, set_config_arr[curr_engine_id]);

        /** Update all the types hw type_config parameters*/
        SHR_IF_ERR_EXIT(dnx_crps_engine_type_hw_set(unit, &engine, type_valid, type_offset));

        /** Get the next engine id*/
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.
                        next_engine_id.get(unit, database->core_id, curr_engine_id, &curr_engine_id));

    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   the function calculate the start counter range and the end counter of the range for given engine. 
* \par DIRECT INPUT: 
*   \param [in] unit    -  unid id
*   \param [in] database  -  structure of database id and core id.
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
dnx_crps_mgmt_multi_sets_range_config(
    int unit,
    bcm_stat_counter_database_t * database)
{
    int engine_id;
    bcm_stat_engine_t engine;
    dnx_set_config_t
        set_stat_ids_range[DNX_DATA_MAX_CRPS_ENGINE_NOF_ENGINES][DNX_DATA_MAX_CRPS_ENGINE_MAX_NOF_COUNTER_SETS];

    SHR_FUNC_INIT_VARS(unit);

    /** Sets range calculate */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_sets_range_get(unit, database->core_id, database->database_id, set_stat_ids_range));

    /** Update log_obj_id_range sw-state structure for each engine in the db */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_datatbase_logicel_range_sw_set(unit, database, set_stat_ids_range));

    SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.get(unit, database->core_id, database->database_id, &engine_id));

    /** For each engine in the database, set sets stat-ids boundaries */
    while (engine_id != DNX_CRPS_MGMT_ENGINE_INVALID)
    {
        engine.engine_id = engine_id;
        engine.core_id = database->core_id;

        /** Update eviction boundaries */
        SHR_IF_ERR_EXIT(dnx_crps_eviction_boundaries_set(unit, BCM_STAT_EVICTION_RANGE_ALL, &engine, NULL));

        /** Get the next engine id */
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.next_engine_id.get(unit, database->core_id, engine_id, &engine_id));

    }

    /*
     * multi sets database use with type zero only enable type zero with offset zero and disable all the additional
     * types 
     */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_first_type_config(unit, database, set_stat_ids_range));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   the function calculate the start counter range and the end counter of the range for given engine.
* \par DIRECT INPUT:
*   \param [in] unit    -  unid id
*   \param [in] engine  -  structure of engine id and core id.
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
dnx_crps_mgmt_engine_types_range_config(
    int unit,
    bcm_stat_engine_t * engine)
{
    int type_id;
    dnx_engine_interface_info_t inf;
    dnx_cprs_mgmt_range_t type_range[BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES], input_engine_range;
    int type_valid[BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES] = { FALSE };
    uint32 type_offset[BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES] = { 0 };
    int nof_counters_in_engine;
    uint32 total_prev_counters_range = 0;
    uint32 address_base_normalize;
    uint32 first_index = CRPS_MGMT_INVALID_LOGICAL_STAT_ID; /** logical_object_id units. It represent the base_normalize-cfg. */
    uint32 base_start = CRPS_MGMT_INVALID_LOGICAL_STAT_ID;
    uint32 last_index = 0; /** logical_object_id units */
    uint32 basic_offset = 0;
    dnx_type_config_t type;
    SHR_FUNC_INIT_VARS(unit);

    /** 1. get the engine configuration from db */
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.get(unit, engine->core_id, engine->engine_id, &inf));

    /**  2. find the total previous number of counters */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_base_engine_get(unit, engine, &total_prev_counters_range, &basic_offset));

    /** 3. find the range of each engine until the current engine (in stat-obj-id index) */
    SHR_IF_ERR_EXIT(crps_mgmt_nof_counters_per_allocated_engine_get
                    (unit, engine->core_id, engine->engine_id, &nof_counters_in_engine));
    input_engine_range.start = basic_offset + total_prev_counters_range;
    input_engine_range.end = input_engine_range.start + nof_counters_in_engine - 1;

    /* *INDENT-OFF* */

  /*
     * indent off General Explanations: 
     *
     * input range:                          ********************** 
     * type 0 range case 1.1:     #############
     * type 1 range case 1.2:      ###################################
     * type 2 range case 2.1:                     ########### 
     * type 3 range case 2.2:                         ######################
     * 
     * For each type, we are searching for the first_index and last_index addmitted for the engine. first and last index are in Logical_object_id units.
     * For case 1.x, the first_index addmitted to the engine is the first one that in the engine range, therefore it is input_engine_range.start 
     * For case 2.x, the first_index addmitted to the engine is the first one that in the type range, therfore, it is type_range.start driver hold the min
     * start_index (and max last_index) and use it to configure the address-base-normalize
     *
     */
   /* *INDENT-ON* */

    for (type_id = 0; type_id < BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES; type_id++)
    {
        SHR_IF_ERR_EXIT(dnx_crps_db.database.type.get(unit, engine->core_id, inf.database_id, type_id, &type));
        if (type.valid == TRUE)
        {
            /** calculate the range of the type */
            /** the range is in logical_object_id units (counter_pointer) */
            /** logical_object_id_first = (Object-ID="start") * cfg-Counter-Set-Size
                                + Counter-Set-Offset(5)=0 + Object-Type-Offset + sampling_offset=0 */
            type_range[type_id].start = type.start * inf.counter_set_size + (type.offset * inf.counter_set_size);
            /** logical_object_id_last = (Object-ID="end") * cfg-Counter-Set-Size
                                + Counter-Set-Offset(5)="counter_set_size-1" + Object-Type-Offset + sampling_offset=0 */
            type_range[type_id].end = type.end * inf.counter_set_size +
                (inf.counter_set_size - 1) + (type.offset * inf.counter_set_size);

            base_start = UTILEX_MIN(type_range[type_id].start, base_start);
        }
    }
    /** Case 3: Another special case is when all types start and ends above the engine range. this is relevant for the first engine.
    if so, shift the engine start value to the lowest start value and start the calculation. */
    if ((base_start > input_engine_range.end) && (total_prev_counters_range == 0))
    {
        input_engine_range.start = base_start;
        input_engine_range.end = input_engine_range.start + nof_counters_in_engine - 1;
    }
    for (type_id = 0; type_id < BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES; type_id++)
    {
        SHR_IF_ERR_EXIT(dnx_crps_db.database.type.get(unit, engine->core_id, inf.database_id, type_id, &type));
        if (type.valid == TRUE)
        {

            /** case-1: if input engine starts in the middle of the type range, the start index is the input engine start value */
            if ((type_range[type_id].start < input_engine_range.start) &&
                (type_range[type_id].end >= input_engine_range.start))
            {
                type_valid[type_id] = TRUE;
                first_index = UTILEX_MIN(first_index, input_engine_range.start);
            }
            /** case-2: if the type starts in the middle of the engine range, the start index is the range  start value */
            if ((type_range[type_id].start >= input_engine_range.start) &&
                (type_range[type_id].start <= input_engine_range.end))
            {
                type_valid[type_id] = TRUE;
                first_index = UTILEX_MIN(first_index, type_range[type_id].start);
            }

            /** if valid, calc the last index */
            if (type_valid[type_id] == TRUE)
            {
                /** if the type ends in the middle of the engine range, set its last index as the range end value*/
                if ((type_range[type_id].end >= input_engine_range.start) &&
                    (type_range[type_id].end <= input_engine_range.end))
                {
                    last_index = UTILEX_MAX(last_index, type_range[type_id].end);
                }
                /** if the type ends beyond the engine end value, set the last index as the engine end value  */
                else
                {
                    last_index = first_index + nof_counters_in_engine - 1;
                }
                type_offset[type_id] = type.offset * inf.counter_set_size;
            }
        }

        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.type_hw_valid.set
                        (unit, engine->core_id, engine->engine_id, type_id, type_valid[type_id]));
    }

    if (first_index == CRPS_MGMT_INVALID_LOGICAL_STAT_ID)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, " Unused engine - engine_id=%d core_id=%d is not used for any valid ranges \n",
                     engine->engine_id, engine->core_id);
    }

    /** 6. update the sw state with the start and end values */
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.log_obj_id_range.logical_object_id_first.set
                    (unit, engine->core_id, engine->engine_id, first_index));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.log_obj_id_range.
                    logical_object_id_last.set(unit, engine->core_id, engine->engine_id, last_index));

    /** 7. update hw */
    address_base_normalize = first_index;
    SHR_IF_ERR_EXIT(dnx_crps_engine_base_offset_hw_set(unit, engine, address_base_normalize));
    SHR_IF_ERR_EXIT(dnx_crps_engine_type_hw_set(unit, engine, type_valid, type_offset));
    /** update eviction boundaries */
    SHR_IF_ERR_EXIT(dnx_crps_eviction_boundaries_set(unit, BCM_STAT_EVICTION_RANGE_ALL, engine, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   for each engine in multi-types database, calculate the start counter range.
* \par DIRECT INPUT:
*   \param [in] unit    -  unid id
*   \param [in] database  -  structure of database id and core id.
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
dnx_crps_mgmt_types_range_config(
    int unit,
    bcm_stat_counter_database_t * database)
{

    bcm_stat_engine_t engine;
    int current_engine_id, base_engine_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.get
                    (unit, database->core_id, database->database_id, &base_engine_id));

    current_engine_id = base_engine_id;

    while (current_engine_id != DNX_CRPS_MGMT_ENGINE_INVALID)
    {
        engine.engine_id = current_engine_id;
        engine.core_id = database->core_id;

        SHR_IF_ERR_EXIT(dnx_crps_mgmt_engine_types_range_config(unit, &engine));

        /*
         * get the next engine
         */
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.next_engine_id.get
                        (unit, engine.core_id, current_engine_id, &current_engine_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   handle the counter interface set per engine. 
*   relevant when the API flag do not contain MODIFY_ONLY flag - meaning full configuration per engine
* \par DIRECT INPUT: 
*   \param [in] unit    -  Unit-ID 
*   \param [in] flags   -  
*   \param [in] engine  -  engine struct
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
dnx_crps_mgmt_counter_interface_engine_set(
    int unit,
    uint32 flags,
    bcm_stat_engine_t * engine,
    bcm_stat_counter_interface_t * config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.src_type.set(unit, engine->core_id, engine->engine_id, config->source));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.command_id.set
                    (unit, engine->core_id, engine->engine_id, config->command_id));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.format.set
                    (unit, engine->core_id, engine->engine_id, config->format_type));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.counter_size.set
                    (unit, engine->core_id, engine->engine_id, config->counter_size));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.mode.set(unit, engine->core_id, engine->engine_id, config->mode));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.counter_set_size.set
                    (unit, engine->core_id, engine->engine_id, config->counter_set_size));

    SHR_IF_ERR_EXIT(dnx_crps_engine_hw_config_set(unit, engine, config));

    /** For multi set database, this parameter will be configured with all the set_cfg parameters*/
    if (!(flags & BCM_STAT_COUNTER_MULTI_SETS_DATABASE))
    {
        SHR_IF_ERR_EXIT(dnx_crps_engine_single_set_size_hw_config_set(unit, engine, config->counter_set_size));
    }

    /** specific configuration per source */
    SHR_IF_ERR_EXIT(dnx_crps_src_config_set(unit, engine->core_id, config));

    /** set default counter overflow mode - OAM can use two
     *  overflow modes - default is Wrap Around; for normal
     *  counters (including Oamp) - default is stuck at 1(= fixed at
     *  max) */
    if (config->source == bcmStatCounterInterfaceIngressOam || config->source == bcmStatCounterInterfaceEgressOam)
    {
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_overflow_counter_mode_set
                        (unit, engine, BCM_STAT_COUNTER_OVERFLOW_MODE_WRAP_AROUND));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_overflow_counter_mode_set
                        (unit, engine, BCM_STAT_COUNTER_OVERFLOW_MODE_FIXED_AT_MAX));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Initialize the next_seq_counter_idx parameter for rate calculation
* \par DIRECT INPUT:
*   \param [in] unit    -  Unit-ID
*   \param [in] core_id -  Specific core
*   \param [in] engine_id  -  engine identifier
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -
* \par INDIRECT OUTPUT:
*   * update sw state  - parameter next_seq_counter_idx
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
dnx_crps_rate_calculation_expected_sequential_counter_idx_init(
    int unit,
    int core_id,
    int engine_id)
{
    uint32 first_counter_idx;
    SHR_FUNC_INIT_VARS(unit);

    /** Initialize the expected next counter object id for distinguish between algorithmic and sequential scan records */
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.eviction_range.start.get(unit, core_id, engine_id, &first_counter_idx));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.rate.next_seq_counter_idx.set(unit, core_id, engine_id, first_counter_idx));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   handle the counter enable set per engine. update the sw state and configure the HW
* \par DIRECT INPUT: 
*   \param [in] unit    -  Unit-ID 
*   \param [in] flags   -  
*   \param [in] engine  -  this is the key -defined by engine_id and core.
*   \param [in] enable  -  configuration structure
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e -
* \par INDIRECT OUTPUT: 
*   * update sw state  - structure dnx_engine_info_t
* \remark 
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_crps_mgmt_counter_engine_enable_set(
    int unit,
    uint32 flags,
    bcm_stat_engine_t * engine,
    bcm_stat_counter_enable_t * enable)
{
    uint32 clear_hw_done = FALSE;
    int is_rate_calculation_enabled;
    soc_timeout_t to;
    sal_usecs_t timeout_usec = 100;
    SHR_FUNC_INIT_VARS(unit);

    /** update sw state */
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.enablers.counting_enable.set
                    (unit, engine->core_id, engine->engine_id, enable->enable_counting));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.enablers.eviction_enable.set
                    (unit, engine->core_id, engine->engine_id, enable->enable_eviction));

#ifdef PLISIM
    if (!SAL_BOOT_PLISIM)       /* not pcid */
#endif
    {
        /** if enable counting, need to verify that that HW clear_counters_memory mode is DONE */
        if (enable->enable_counting == TRUE)
        {
            soc_timeout_init(&to, timeout_usec, 0);
            /** polling on the done indication or return error if timeout */
            while (clear_hw_done == FALSE)
            {
                SHR_IF_ERR_EXIT(dnx_crps_engine_clear_hw_mem_done_get(unit, engine, &clear_hw_done));
                if (soc_timeout_check(&to))
                {
                    if (clear_hw_done == TRUE)
                    {
                        break;
                    }
                    SHR_ERR_EXIT(_SHR_E_BUSY,
                                 "enable_counting=TRUE, but hw still didn't finished to clear it's engine memory \n");
                }
            }
        }
    }

    /** indicates that config is done and engine valid. from this point we can start evict and update counters */
    /** must be called before attach the dma and enable the HW scanning */
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.config_done.set(unit, engine->core_id, engine->engine_id, TRUE));

    /** attach dma fifo and activate bg thread if needed. The handle function check and activate FIFO/Bg thread if need to */
    SHR_IF_ERR_EXIT(dnx_crps_eviction_collection_handle(unit, engine->core_id, engine->engine_id));

    /** Initialize rate calculation */
    if (enable->enable_eviction
        && dnx_data_crps.general.feature_get(unit, dnx_data_crps_general_crps_rate_calculation_support))
    {
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.rate.rate_calculation_enable.get(unit, engine->core_id, engine->engine_id,
                                                                          &is_rate_calculation_enabled));
        if (is_rate_calculation_enabled)
        {
            SHR_IF_ERR_EXIT(dnx_crps_rate_calculation_expected_sequential_counter_idx_init
                            (unit, engine->core_id, engine->engine_id));
        }
    }

    /** update hw */
    SHR_IF_ERR_EXIT(dnx_crps_engine_enable_hw_set(unit, engine, enable->enable_eviction, enable->enable_counting));

exit:
    SHR_FUNC_EXIT;

}

/**
* \brief
*       Update sw-state with the given types parameters
* \par DIRECT INPUT:
*   \param [in] unit    -  unid id
*   \param [in] flags  -  structure of engine id and core id.
*   \param [in] database  -  structure of database id and core id.
*   \param [in] type_config - structure holds the type parameters
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
dnx_crps_mgmt_counter_database_info_type_sw_update(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_type_config_t * type_config)
{
    int type_id;
    SHR_FUNC_INIT_VARS(unit);

    /** update db and b for each one of the types */
    for (type_id = 0; type_id < BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES; type_id++)
    {
        if ((flags & BCM_STAT_COUNTER_MODIFY_RANGE_ONLY) == FALSE)
        {
            SHR_IF_ERR_EXIT(dnx_crps_db.database.type.valid.set
                            (unit, database->core_id, database->database_id, type_id, type_config[type_id].valid));
            SHR_IF_ERR_EXIT(dnx_crps_db.database.type.
                            offset.set(unit, database->core_id, database->database_id, type_id,
                                       type_config[type_id].object_type_offset));
        }

        SHR_IF_ERR_EXIT(dnx_crps_db.database.type.start.set
                        (unit, database->core_id, database->database_id, type_id, type_config[type_id].start));
        SHR_IF_ERR_EXIT(dnx_crps_db.database.type.end.set
                        (unit, database->core_id, database->database_id, type_id, type_config[type_id].end));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*       for multi-sets database, update sw-state with the given set parameters
*       update sw-state first type boundaries
* \par DIRECT INPUT:
*   \param [in] unit    -  unid id
*   \param [in] flags  -  structure of engine id and core id.
*   \param [in] database  -  structure of database id and core id.
*   \param [in] set_config - structure holds the set parameters
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
dnx_crps_mgmt_counter_database_info_set_sw_update(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_set_config_t * set_config)
{

    int set_id;
    int max_stat_id = 0;
    int min_stat_id = CRPS_MGMT_INVALID_LOGICAL_STAT_ID;
    int type_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Update the database's sets boundaries. Get the smallest and the largest stat-ids in all the sets. Those will be
     * the type boundaries. 
     */
    for (set_id = 0; set_id < dnx_data_crps.engine.max_nof_counter_sets_get(unit); set_id++)
    {
        if ((flags & BCM_STAT_COUNTER_MODIFY_RANGE_ONLY) == FALSE)
        {
            SHR_IF_ERR_EXIT(dnx_crps_db.database.set.size.set
                            (unit, database->core_id, database->database_id, set_id, set_config[set_id].set_size));
        }

        SHR_IF_ERR_EXIT(dnx_crps_db.database.set.start.set
                        (unit, database->core_id, database->database_id, set_id, set_config[set_id].start));
        SHR_IF_ERR_EXIT(dnx_crps_db.database.set.end.set
                        (unit, database->core_id, database->database_id, set_id, set_config[set_id].end));

        min_stat_id = UTILEX_MIN(set_config[set_id].start, min_stat_id);
        max_stat_id = UTILEX_MAX(set_config[set_id].end, max_stat_id);

    }

    /** Configure the first type range to all the sets range */
    SHR_IF_ERR_EXIT(dnx_crps_db.database.type.valid.set(unit, database->core_id, database->database_id, type_id, 1));
    SHR_IF_ERR_EXIT(dnx_crps_db.database.type.start.set
                    (unit, database->core_id, database->database_id, type_id, min_stat_id));
    SHR_IF_ERR_EXIT(dnx_crps_db.database.type.end.set
                    (unit, database->core_id, database->database_id, type_id, max_stat_id));

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_interface_set(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_interface_t * config)
{
    int valid;
    int current_engine_id, base_engine_id;
    bcm_stat_engine_t engine, src_engine;
    bcm_stat_counter_enable_t enable;
    bcm_stat_counter_interface_t modify_config;

    SHR_FUNC_INIT_VARS(unit);

    /** Set struct to 0 */
    sal_memset(&modify_config, 0, sizeof(modify_config));

    /** get the base interface configuration in case of modify scenario */
    if ((flags & BCM_STAT_COUNTER_MODIFY_RANGE_ONLY) == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_interface_get(unit, 0, database, &modify_config));
    }

    SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.get
                    (unit, database->core_id, database->database_id, &base_engine_id));
    current_engine_id = base_engine_id;

    /** set the configuration for all engines attached to the database */
    while (current_engine_id != DNX_CRPS_MGMT_ENGINE_INVALID)
    {
        engine.core_id = database->core_id;
        engine.engine_id = current_engine_id;

        /** if new configuration, need to configure all the engines in the database */
        if ((flags & BCM_STAT_COUNTER_MODIFY_RANGE_ONLY) == FALSE)
        {
            SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_interface_engine_set(unit, flags, &engine, config));
        }
        else
        {
            /** in modify_only scenario, need to configure new engines from scratch, */
            /** the configuration is taken from base_engine */
            DNX_CRPS_MGMT_ENGINE_VALID_GET(unit, database->core_id, current_engine_id, valid);
            if (valid == FALSE)
            {
                /** configure the engine with interface configuration, as taken from original config + modified range */
                SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_interface_engine_set(unit, flags, &engine, &modify_config));
                /** copy the data mapping configuration from the base engine into the current engine */
                src_engine.core_id = database->core_id;
                src_engine.engine_id = base_engine_id;
                SHR_IF_ERR_EXIT(dnx_crps_engine_data_mapping_hw_copy(unit, &src_engine, &engine));
                /** get the enablers configuration and set it for the new engine */
                SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_database_enable_get(unit, flags, database, &enable));
                SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_engine_enable_set(unit, flags, &engine, &enable));
            }
        }

        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.next_engine_id.get
                        (unit, engine.core_id, current_engine_id, &current_engine_id));
    }

    if ((flags & BCM_STAT_COUNTER_MULTI_SETS_DATABASE) != FALSE)
    {
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_database_info_set_sw_update(unit, flags, database, config->set_config));
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_multi_sets_range_config(unit, database));
    }
    else
    {
        /** range handle for all of the engines. */
        /**it is user responsibility not to create new range that modify the range of active engines
        (except the last active engine that might be increased/decreased) */
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_database_info_type_sw_update(unit, flags, database, config->type_config));
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_types_range_config(unit, database));
    }

    /** for etpp/epp with the relevant command, need to select the latency value as input to the engine and not the packet size. */
    if (_SHR_IS_FLAG_SET(flags, BCM_STAT_COUNTER_MODIFY_RANGE_ONLY) == FALSE &&
        (((config->command_id == dnx_data_crps.latency.etpp_command_id_get(unit))
          && (config->source == bcmStatCounterInterfaceEgressTransmitPp))
         || ((config->command_id == dnx_data_crps.latency.epp_command_id_get(unit))
             && (config->source == bcmStatCounterInterfaceEgressPp))))
    {
        if (_SHR_IS_FLAG_SET(flags, BCM_STAT_COUNTER_END_TO_END_LATENCY))
        {
            SHR_IF_ERR_EXIT(dnx_crps_src_interface_egress_latency_ctr_set(unit, database->core_id, TRUE));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_crps_src_interface_egress_latency_ctr_set(unit, database->core_id, FALSE));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_interface_get(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_interface_t * config)
{
    int type_id, set_idx;
    int base_engine;
    int counter_size;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.get(unit, database->core_id, database->database_id, &base_engine));
    /** update config from base_engine sw state db */
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.src_type.get(unit, database->core_id, base_engine, &config->source));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.command_id.get
                    (unit, database->core_id, base_engine, &config->command_id));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.format.get(unit, database->core_id, base_engine, &config->format_type));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.counter_size.get(unit, database->core_id, base_engine, &counter_size));
    config->counter_size = (uint32) (counter_size == DNX_CRPS_MGMT_ENGINE_FIELDS_INVALID ? 0 : counter_size);
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.mode.get(unit, database->core_id, base_engine, &config->mode));

    if ((_SHR_IS_FLAG_SET(flags, BCM_STAT_COUNTER_MULTI_SETS_DATABASE)))
    {
        /** update from db for each one of the sets */
        for (set_idx = 0; set_idx < dnx_data_crps.engine.max_nof_counter_sets_get(unit); set_idx++)
        {
            SHR_IF_ERR_EXIT(dnx_crps_db.database.set.start.get(unit, database->core_id, database->database_id, set_idx,
                                                               &config->set_config[set_idx].start));
            SHR_IF_ERR_EXIT(dnx_crps_db.database.set.end.get(unit, database->core_id, database->database_id, set_idx,
                                                             &config->set_config[set_idx].end));
            SHR_IF_ERR_EXIT(dnx_crps_db.database.set.size.get(unit, database->core_id, database->database_id, set_idx,
                                                              (int *) &config->set_config[set_idx].set_size));
        }

    }
    else
    {
        /** Get the single counter-set's size*/
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.counter_set_size.get
                        (unit, database->core_id, base_engine, &config->counter_set_size));

        /** update from db for each one of the types */
        for (type_id = 0; type_id < BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES; type_id++)
        {
            SHR_IF_ERR_EXIT(dnx_crps_db.database.type.valid.get(unit, database->core_id, database->database_id, type_id,
                                                                &config->type_config[type_id].valid));
            SHR_IF_ERR_EXIT(dnx_crps_db.database.type.
                            offset.get(unit, database->core_id, database->database_id, type_id,
                                       &config->type_config[type_id].object_type_offset));
            SHR_IF_ERR_EXIT(dnx_crps_db.database.type.
                            start.get(unit, database->core_id, database->database_id, type_id,
                                      &config->type_config[type_id].start));
            SHR_IF_ERR_EXIT(dnx_crps_db.database.type.
                            end.get(unit, database->core_id, database->database_id, type_id,
                                    &config->type_config[type_id].end));
        }
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *      handle reset engine. clear from database and clear HW memory and relevant registers.
 * \param [in] unit -
 *     unit id
 * \param [in] engine -
 *     strucutre contain engine_id and core_id
 * \return
 *   see shr_error_e
 * \remark
 *   NONE
 * \see
 *   NONE
 */
static shr_error_e
dnx_crps_mgmt_engine_reset(
    int unit,
    bcm_stat_engine_t * engine)
{
    int current_fifo_id, engine_idx, valid;
    int data_mapping_table_size;
    int next, prev;
    int fifo_id;
    int fifo_found = FALSE;
    int dynamic_metadata_src_found = FALSE, etm_found = FALSE;
    int is_etm;
    int current_engine_valid_eviction;
    bcm_stat_counter_interface_type_t src;
    bcm_stat_counter_interface_type_t current_engine_src;

    SHR_FUNC_INIT_VARS(unit);

    /** get from db the fifo_id parameters */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_engine_eviction_valid_get
                    (unit, engine->core_id, engine->engine_id, &current_engine_valid_eviction));
    if (current_engine_valid_eviction)
    {
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.dma_fifo.get(unit, engine->core_id, engine->engine_id, &fifo_id));
    }

    /** check if the engine we reset is connected to etm */
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.src_type.get(unit, engine->core_id, engine->engine_id, &src));
    is_etm = (src == bcmStatCounterInterfaceEgressReceiveQueue) ? TRUE : FALSE;

    /** set invalid counter set size */
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.counter_set_size.set(unit, engine->core_id, engine->engine_id,
                                                                    DNX_CRPS_MGMT_ENGINE_FIELDS_INVALID));

    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.format.set(unit, engine->core_id, engine->engine_id,
                                                          DNX_CRPS_MGMT_COUNTER_FORMAT_DEFAULT));

    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.counter_size.set(unit, engine->core_id, engine->engine_id,
                                                                DNX_CRPS_MGMT_ENGINE_FIELDS_INVALID));

    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.mode.set(unit, engine->core_id, engine->engine_id,
                                                        DNX_CRPS_MGMT_COUNTER_MODE_DEFAULT));

    /** Get the previous and next engines. There are 4 possible cases:
     *  1. Both previous and next engines are invalid - the current engine is the base engine of the DB and
     *     is removed last. No need to update anything.
     *  2. Next engine is valid and the previous engine is invalid - the current engine is the base engine
     *     but it is not removed last. This should never happen, as there is a check at the detach API.
     *  3. Both previous and next engines are valid - current engine is from the middle of the DB, handled below.
     *  4. Next engine is invalid and the previous engine is valid - current engine is the last engine of the
     *     database, handled below. */
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.next_engine_id.get(unit, engine->core_id, engine->engine_id, &next));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.prev_engine_id.get(unit, engine->core_id, engine->engine_id, &prev));

    if ((next != DNX_CRPS_MGMT_ENGINE_INVALID) && (prev != DNX_CRPS_MGMT_ENGINE_INVALID))
    {
        /** If both previous and next engines are valid, the current engine is from the middle
         *  of the database - reconnect the linked list without it */
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.prev_engine_id.set(unit, engine->core_id, next, prev));
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.next_engine_id.set(unit, engine->core_id, prev, next));
    }

    if ((next == DNX_CRPS_MGMT_ENGINE_INVALID) && (prev != DNX_CRPS_MGMT_ENGINE_INVALID))
    {
        /** If the next engine is invalid and the previous engine is valid, the current engine is the
         *  last engine of the database - update the previous engine to be the new 'last' */
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.next_engine_id.set(unit, engine->core_id, prev,
                                                                      DNX_CRPS_MGMT_ENGINE_INVALID));
    }

    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.prev_engine_id.set(unit, engine->core_id, engine->engine_id,
                                                                  DNX_CRPS_MGMT_ENGINE_INVALID));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.next_engine_id.set(unit, engine->core_id, engine->engine_id,
                                                                  DNX_CRPS_MGMT_ENGINE_INVALID));

    /** clear engine config started and done flags */
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.config_started.set(unit, engine->core_id, engine->engine_id, FALSE));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.config_done.set(unit, engine->core_id, engine->engine_id, FALSE));

    /** clear data mapping table - configure all entries as not valid */
    data_mapping_table_size = dnx_data_crps.engine.data_mapping_table_size_get(unit);
    SHR_IF_ERR_EXIT(dnx_crps_engine_data_mapping_full_spread_set(unit, engine, data_mapping_table_size, FALSE));
    /** clear sw memory */
    SHR_IF_ERR_EXIT(dnx_crps_ctrs_db_counter_clear(unit, engine->core_id, engine->engine_id));

    for (engine_idx = 0; engine_idx < dnx_data_crps.engine.nof_engines_get(unit); engine_idx++)
    {
        if (current_engine_valid_eviction)
        {
            /** search if another engine use the same fifo that we want to detach. if not - detach it */
            SHR_IF_ERR_EXIT(dnx_crps_mgmt_engine_eviction_valid_get(unit, engine->core_id, engine_idx, &valid));
            if (valid)
            {
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.
                                dma_fifo.get(unit, engine->core_id, engine_idx, &current_fifo_id));
                if (fifo_id == current_fifo_id)
                {
                    fifo_found = TRUE;
                }
            }
        }

        DNX_CRPS_MGMT_ENGINE_VALID_GET(unit, engine->core_id, engine_idx, valid);
        if (valid)
        {

            /** check if etpp meta-data init is required */
            if (DNX_CRPS_EXPANSION_METADATA_IS_DYNAMIC(src))
            {
                /** search for engines connected to etpp. if none, init the etpp metadata selection */
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.
                                src_type.get(unit, engine->core_id, engine_idx, &current_engine_src));

                if (DNX_CRPS_EXPANSION_METADATA_IS_DYNAMIC(current_engine_src))
                {
                    dynamic_metadata_src_found = TRUE;
                }
            }

            /** check if etm counter pointer reset is required */
            if (is_etm == TRUE)
            {
                /** search for engines connected to etm. if none, disable the counter pointer from ETM */
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.src_type.get(unit, engine->core_id, engine_idx, &src));
                if (src == bcmStatCounterInterfaceEgressReceiveQueue)
                {
                    etm_found = TRUE;
                }
            }

        }

    }
    /** if engine_id == nof_engines, it means that we didn't find any engine that connected to the same fifo that we want to detach. */
    /** therefore, we can detach it. In addition, we check if bg thread can be detach (if no more fifo active) */
    if (fifo_found == FALSE && current_engine_valid_eviction == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_crps_dma_fifo_detach(unit, engine->core_id, fifo_id));
        SHR_IF_ERR_EXIT(dnx_crps_bg_thread_detach_check(unit));
    }
    /** if no engine connected anymore to etpp, we will clear the etpp data selection */
    if ((dynamic_metadata_src_found == FALSE) && DNX_CRPS_EXPANSION_METADATA_IS_DYNAMIC(src))
    {
        SHR_IF_ERR_EXIT(dnx_crps_src_interface_dynamic_meta_data_selection_init(unit));
    }
    /** if no engine connected anymore to etm, we will disable the counter pointer generation from etm */
    if (((etm_found == FALSE) && (is_etm == TRUE)) &&
        dnx_data_crps.src_interface.feature_get(unit,
                                                dnx_data_crps_src_interface_etm_count_pp_port_enable_table_is_supported))
    {
        SHR_IF_ERR_EXIT(dnx_crps_src_egress_queue_hw_set(unit, engine->core_id, FALSE));
    }
    /** disable engine access to counters memory space */
    SHR_IF_ERR_EXIT(dnx_crps_engine_memory_access_enable(unit, engine, FALSE));
    /** reset the dma fifo sw state to invalid value */
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.
                    dma_fifo.set(unit, engine->core_id, engine->engine_id, CRPS_MGMT_INVALID_DMA_FIFO));

    /** reset eviction event id to invalid value */
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.eviction_event_id.set(unit, engine->core_id, engine->engine_id,
                                                                    DNX_CRPS_MGMT_ENGINE_FIELDS_INVALID));

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_database_enable_set(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_enable_t * enable)
{
    bcm_stat_engine_t engine;
    SHR_FUNC_INIT_VARS(unit);

    engine.core_id = database->core_id;
    SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.get
                    (unit, database->core_id, database->database_id, &engine.engine_id));

    /** enable all engines in database */
    while (engine.engine_id != DNX_CRPS_MGMT_ENGINE_INVALID)
    {
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_engine_enable_set(unit, flags, &engine, enable));
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.next_engine_id.get
                        (unit, engine.core_id, engine.engine_id, &engine.engine_id));
    }

exit:
    SHR_FUNC_EXIT;

}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_database_enable_get(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_enable_t * enable)
{
    int base_engine_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.get
                    (unit, database->core_id, database->database_id, &base_engine_id));

    /** update sw state */
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.enablers.counting_enable.get
                    (unit, database->core_id, base_engine_id, &enable->enable_counting));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.enablers.eviction_enable.get
                    (unit, database->core_id, base_engine_id, &enable->enable_eviction));

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_expansion_select_set(
    int unit,
    uint32 flags,
    bcm_stat_counter_interface_key_t * interface,
    bcm_stat_expansion_select_t * expansion_select)
{
    uint32 selection[CRPS_MGMT_SELECTION_ARR_SIZE] = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    /** for dynamic metadata, define the types shift first*/
    if (DNX_CRPS_EXPANSION_METADATA_IS_DYNAMIC(interface->interface_source))
    {
        SHR_IF_ERR_EXIT(dnx_crps_src_interface_dynamic_metadata_format_config(unit, interface, expansion_select));
    }

    /**get the selection bitmap*/
    SHR_IF_ERR_EXIT(dnx_crps_src_interface_expansion_select_calc
                    (unit, interface->core_id, interface->interface_source, interface->command_id, expansion_select,
                     selection));

    SHR_IF_ERR_EXIT(dnx_crps_src_interface_expansion_select_hw_set(unit, interface, selection));

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_expansion_bitmap_get(
    int unit,
    bcm_stat_counter_interface_key_t * interface,
    uint32 *selection_bitmap)
{
    int total_nof_bits = 0;
    uint32 tmp_selection_bitmap[CRPS_MGMT_SELECTION_ARR_SIZE] = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    total_nof_bits =
        dnx_data_crps.expansion.source_expansion_info_get(unit, interface->interface_source)->expansion_size;
    /** in case of unconfigurable expansion, there is only one possible expansion */
    if (total_nof_bits <= (int) dnx_data_crps.expansion.expansion_size_get(unit))
    {
        SHR_BITSET_RANGE(tmp_selection_bitmap, 0, total_nof_bits);
        SHR_BITCOPY_RANGE(selection_bitmap, 0, tmp_selection_bitmap, 0, 64);

        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_crps_src_interface_expansion_select_hw_get(unit, interface, selection_bitmap));

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_expansion_select_get(
    int unit,
    uint32 flags,
    bcm_stat_counter_interface_key_t * interface,
    bcm_stat_expansion_select_t * expansion_select)
{
    uint32 selection_bitmap[CRPS_MGMT_SELECTION_ARR_SIZE] = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    /** get expansion select bitmap */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_expansion_bitmap_get(unit, interface, selection_bitmap));

    /** convert the selection bitmap to expansion_select element */
    SHR_IF_ERR_EXIT(dnx_crps_src_interface_expansion_bitmap_convert
                    (unit, interface->core_id, interface->interface_source, interface->command_id, selection_bitmap,
                     expansion_select));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 *  build an interface key for a given engine.
 *  interfcae_key is: {core, src, command_id, type_id}
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   \param [in] engine - the engine_id and core_id
 *   \param [out] nof_keys - size of interface keys array. max size = BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES
 *   \param [out] interface_arr - array of interface keys.
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_crps_mgmt_engine_interface_key_get(
    int unit,
    bcm_stat_engine_t * engine,
    int *nof_keys,
    bcm_stat_counter_interface_key_t * interface_arr)
{
    int type_id;
    int key_index = 0;
    dnx_engine_interface_info_t inf;
    dnx_type_config_t type;
    int is_multi_sets_database, database_id;
    SHR_FUNC_INIT_VARS(unit);

    /** 1. get the relevant params from db in order to build the keys*/
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.get(unit, engine->core_id, engine->engine_id, &inf));

    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.database_id.get(unit, engine->core_id, engine->engine_id, &database_id));
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_is_multi_sets_database(unit, engine->core_id, database_id, &is_multi_sets_database));

    /** 2.1. for multi-sets database, update only the first type interface key*/
    if (is_multi_sets_database)
    {
        interface_arr[key_index].core_id = engine->core_id;
        interface_arr[key_index].command_id = inf.command_id;
        interface_arr[key_index].interface_source = inf.src_type;
        interface_arr[key_index].type_id = 0;
        key_index = 1;
    }
    /** 2.2. run over all types and update only the ones that are valid */
    else
    {
        for (type_id = 0; type_id < BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES; type_id++)
        {
            SHR_IF_ERR_EXIT(dnx_crps_db.database.type.get(unit, engine->core_id, inf.database_id, type_id, &type));
            if (type.valid == TRUE)
            {
                interface_arr[key_index].core_id = engine->core_id;
                interface_arr[key_index].command_id = inf.command_id;
                interface_arr[key_index].interface_source = inf.src_type;
                interface_arr[key_index].type_id = type_id;
                key_index++;
            }
        }
    }

    (*nof_keys) = key_index;

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief 
 *   function should run over all valid types for a given engine and for each valid type get its expansion selection bitmap.
 *   For each unique bitmap (which means that two types has the same bitmap, it will take only one of them), 
 *   it generate arrays with the interface key and array of the bitmap. 
 *   this way, the function return all the unique expansion selections. and the caller function will use it to generate the data mapping table.
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   \param [in] engine - the engine_id and core_id
 *   \param [out] nof_keys - size of unique interface keys array. max size = BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES
 *   \param [out] interface_key_unique - array of unique interface keys.
  *  \param [out] selection_arr - array of the unique expansion selection bitmaps
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_crps_mgmt_unique_expansion_selection_get(
    int unit,
    bcm_stat_engine_t * engine,
    int *nof_keys,
    bcm_stat_counter_interface_key_t * interface_key_unique,
    uint32 selection_arr[BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES][CRPS_MGMT_SELECTION_ARR_SIZE])
{
    int all_index, unique_index;
    int nof_keys_all = 0;
    uint32 selection_bitmap[CRPS_MGMT_SELECTION_ARR_SIZE] = { 0 };
    int nof_keys_unique = 0;
    int unique;
    bcm_stat_counter_interface_key_t interface_key_all[BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES];
    SHR_FUNC_INIT_VARS(unit);
    bcm_stat_counter_interface_key_t_init(interface_key_all);

    /** 1. get all the types that are valid for this engine. each type is different interface_key */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_engine_interface_key_get(unit, engine, &nof_keys_all, interface_key_all));

    /** 2. run over all valid interfaces, and get the expansion selection for each one */
    for (all_index = 0; all_index < nof_keys_all; all_index++)
    {

        SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_expansion_bitmap_get
                        (unit, &interface_key_all[all_index], selection_bitmap));

        unique = TRUE;

        /** 3. fill and return back array, that hold all the interfaces that has unique expansion selection */
        for (unique_index = 0; unique_index < nof_keys_unique; unique_index++)
        {
            /** compare two 64 bitmaps. if equal, the expansion is not unique => no need to add another member to the unique key table */
            if (SHR_BITEQ_RANGE(selection_bitmap, selection_arr[unique_index], 0, 64))
            {
                unique = FALSE;
                break;
            }
        }
        if (unique)
        {
            /** copy the structure into unique key array */
            interface_key_unique[nof_keys_unique] = interface_key_all[all_index];
            SHR_BITCOPY_RANGE(selection_arr[nof_keys_unique], 0, selection_bitmap, 0, 64);
            nof_keys_unique++;
        }
    }
    *nof_keys = nof_keys_unique;

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   API which configure the counter set mapping. defines the counter_set size and what each counter in the set represent.
*   it verify the input parameters and update the HW.
* \par DIRECT INPUT: 
*   \param [in] unit             -  unit ID
*   \param [in] flags            -  FULL_SPREAD - will spread all the expansion data in the counter_set.
*   \param [in] engine  -  pointer to structure which contain the engine_id and core_id
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
static shr_error_e
dnx_crps_mgmt_counter_set_mapping_engine_set(
    int unit,
    uint32 flags,
    bcm_stat_engine_t * engine,
    bcm_stat_counter_set_map_t * counter_set_map)
{
    int nof_keys_unique, entry;
    uint32 selection_arr[BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES][CRPS_MGMT_SELECTION_ARR_SIZE] = { {0} };
    /** only keys that generate unique expansion selection */
    bcm_stat_counter_interface_key_t interface_key_unique[BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES];
    int counter_set_size;
    SHR_FUNC_INIT_VARS(unit);

    /** 1. configure data mapping */

    /** 1.1 go over all valid types, get the ones that have unique expansion selection. get its interface and expansion selection bitmap.  */
    /** next steps will find the first expansion selection type that has all the fields from each set of conditions and update the table accordingly */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_unique_expansion_selection_get
                    (unit, engine, &nof_keys_unique, interface_key_unique, selection_arr));

    /** 1.2 if full spread flag - set "couter_set_size" entries in data mapping table as follow: {valid=TRUE, offset=table_index}*/
    if (flags & BCM_STAT_COUNTER_MAPPING_FULL_SPREAD)
    {
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.counter_set_size.get
                        (unit, engine->core_id, engine->engine_id, &counter_set_size));
        SHR_IF_ERR_EXIT(dnx_crps_engine_data_mapping_full_spread_set(unit, engine, counter_set_size, TRUE));
    }
    /** 1.3 fill the data mapping table according to the API. each entry in the API can generate several indexes in the data mapping table */
    else
    {
        /** go over all entries in the API and find the relevant indexes in the data mapping table to update */
        for (entry = 0; entry < counter_set_map->nof_entries; entry++)
        {
            /** second parameter TRUE, means write the value. (FALSE means READ) */
            SHR_IF_ERR_EXIT(dnx_crps_engine_data_mapping(unit, TRUE, engine, nof_keys_unique, interface_key_unique,
                                                         selection_arr,
                                                         &counter_set_map->expansion_data_mapping[entry]));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_set_mapping_set(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_set_map_t * counter_set_map)
{
    bcm_stat_engine_t base_engine, next_engine;
    SHR_FUNC_INIT_VARS(unit);

    base_engine.core_id = next_engine.core_id = database->core_id;

    SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.get
                    (unit, database->core_id, database->database_id, &base_engine.engine_id));
    /** full calculation to the counter set mapping for the first engine in database */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_set_mapping_engine_set(unit, flags, &base_engine, counter_set_map));

    /** get next engine in database */
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.next_engine_id.get
                    (unit, next_engine.core_id, base_engine.engine_id, &next_engine.engine_id));
    /** the next engines do not need another calculation, they just copy the configuration from the base engine */
    /** Note: it is assumed that no sw state is being updated inside function "dnx_crps_mgmt_counter_set_mapping_engine_set" */
    while (next_engine.engine_id != DNX_CRPS_MGMT_ENGINE_INVALID)
    {
        SHR_IF_ERR_EXIT(dnx_crps_engine_data_mapping_hw_copy(unit, &base_engine, &next_engine));
        /** get next engine in database */
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.next_engine_id.get
                        (unit, next_engine.core_id, next_engine.engine_id, &next_engine.engine_id));
    }

exit:
    SHR_FUNC_EXIT;

}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_set_mapping_get(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_set_map_t * counter_set_map)
{
    int nof_keys_unique, entry;
    uint32 selection_arr[BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES][CRPS_MGMT_SELECTION_ARR_SIZE] = { {0} };
    /** only keys that generate unique expansion selection */
    bcm_stat_counter_interface_key_t interface_key_unique[BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES];
    bcm_stat_engine_t engine;
    SHR_FUNC_INIT_VARS(unit);

    /** all the configuration is taken from the base engine of the database */
    engine.core_id = database->core_id;
    SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.get
                    (unit, database->core_id, database->database_id, &engine.engine_id));

    /** 1. configure data mapping */

    /** 1.1 go over all valid types, get the ones that have unique expansion selection. get its interface and expansion selection bitmap.  */
    /** next steps will find the first expansion selection type that has all the fields from each set of conditions and update the table accordingly */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_unique_expansion_selection_get
                    (unit, &engine, &nof_keys_unique, interface_key_unique, selection_arr));

    /** 1.2 fill back from the data mapping table the valid and offset fields according to the API inputs. */
    /** each entry in the API can generate several indexes in the data mapping table. driver will choose one of the valid entries and read its data */
    for (entry = 0; entry < counter_set_map->nof_entries; entry++)
    {
        /** second parameter FALSE, means read from table. (TRUE means WRITE) */
        SHR_IF_ERR_EXIT(dnx_crps_engine_data_mapping(unit, FALSE, &engine, nof_keys_unique, interface_key_unique,
                                                     selection_arr, &counter_set_map->expansion_data_mapping[entry]));
    }

exit:
    SHR_FUNC_EXIT;

}

static shr_error_e
dnx_crps_mgmt_counter_eviction_sw_set(
    int unit,
    uint32 flags,
    bcm_stat_engine_t * engine,
    bcm_stat_eviction_t * eviction)
{
    SHR_FUNC_INIT_VARS(unit);

    /** update data base */
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.record_format.set
                    (unit, engine->core_id, engine->engine_id, eviction->record_format));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.eviction_type.set
                    (unit, engine->core_id, engine->engine_id, eviction->type));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.eviction_event_id.set
                    (unit, engine->core_id, engine->engine_id, eviction->eviction_event_id));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.dma_fifo.set
                    (unit, engine->core_id, engine->engine_id, eviction->dma_fifo_select));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.eviction_algorithmic_disable.set
                    (unit, engine->core_id, engine->engine_id, eviction->eviction_algorithmic_disable));

    if (_SHR_IS_FLAG_SET(flags, BCM_STAT_EVICTION_CONDITIONAL_ENABLE))
    {
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.conditional.enable.set
                        (unit, engine->core_id, engine->engine_id, TRUE));
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.conditional.
                        action_flags.set(unit, engine->core_id, engine->engine_id, eviction->cond.action_flags));
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.conditional.
                        qualifier.set(unit, engine->core_id, engine->engine_id, eviction->cond.qualifier));
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.conditional.
                        condition_user_data.set(unit, engine->core_id, engine->engine_id,
                                                eviction->cond.condition_user_data));
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.conditional.
                        condition_source_select.set(unit, engine->core_id, engine->engine_id,
                                                    eviction->cond.condition_source_select));
    }
    else
    {
      /** if the current state is that conditional is disabled, set zero for all structure */
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.conditional.enable.set(unit, engine->core_id, engine->engine_id, 0));
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.conditional.action_flags.set
                        (unit, engine->core_id, engine->engine_id, 0));
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.conditional.qualifier.set
                        (unit, engine->core_id, engine->engine_id, 0));
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.conditional.condition_user_data.set
                        (unit, engine->core_id, engine->engine_id, 0));
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.conditional.condition_source_select.set
                        (unit, engine->core_id, engine->engine_id, 0));
    }

exit:
    SHR_FUNC_EXIT;

}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_eviction_set(
    int unit,
    uint32 flags,
    bcm_stat_engine_t * engine,
    bcm_stat_eviction_t * eviction)
{
    SHR_FUNC_INIT_VARS(unit);

    /** set sw configurations */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_eviction_sw_set(unit, flags, engine, eviction));

    /** set hw parameters - beside eviction bundaries which is set from interface API, once all parameters are determined */
    SHR_IF_ERR_EXIT(dnx_crps_eviction_engine_hw_set(unit, engine, eviction));

exit:
    SHR_FUNC_EXIT;

}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_eviction_get(
    int unit,
    uint32 flags,
    bcm_stat_engine_t * engine,
    bcm_stat_eviction_t * eviction)
{
    SHR_FUNC_INIT_VARS(unit);
    /** update the strucutre from sw state db */
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.record_format.get
                    (unit, engine->core_id, engine->engine_id, &eviction->record_format));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.eviction_type.get
                    (unit, engine->core_id, engine->engine_id, &eviction->type));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.eviction_event_id.get
                    (unit, engine->core_id, engine->engine_id, &eviction->eviction_event_id));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.dma_fifo.get
                    (unit, engine->core_id, engine->engine_id, &eviction->dma_fifo_select));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.eviction_algorithmic_disable.get
                    (unit, engine->core_id, engine->engine_id, &eviction->eviction_algorithmic_disable));

    /** if the current state is that conditional is disabled, all structure is zero (see eviction set procedure */
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.conditional.action_flags.get
                    (unit, engine->core_id, engine->engine_id, &eviction->cond.action_flags));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.conditional.qualifier.get
                    (unit, engine->core_id, engine->engine_id, &eviction->cond.qualifier));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.conditional.condition_source_select.get
                    (unit, engine->core_id, engine->engine_id, &eviction->cond.condition_source_select));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.conditional.condition_user_data.get
                    (unit, engine->core_id, engine->engine_id, &eviction->cond.condition_user_data));

exit:
    SHR_FUNC_EXIT;

}

/** see .h file */
shr_error_e
dnx_crps_mgmt_engine_eviction_valid_get(
    int unit,
    int core_id,
    int engine_id,
    int *valid)
{
    int conf_valid, dma_fifo;
    bcm_eviction_destination_type_t eviction_type;
    bcm_eviction_record_format_t record_format;
    bcm_stat_counter_interface_type_t engine_source;
    SHR_FUNC_INIT_VARS(unit);

    DNX_CRPS_MGMT_ENGINE_VALID_GET(unit, core_id, engine_id, conf_valid);
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.eviction_type.get(unit, core_id, engine_id, &eviction_type));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.record_format.get(unit, core_id, engine_id, &record_format));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.dma_fifo.get(unit, core_id, engine_id, &dma_fifo));
    /** if engine source is OAM type - eviction is not valid */
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.src_type.get(unit, core_id, engine_id, &engine_source));
    if ((conf_valid == TRUE) && (eviction_type == bcmStatEvictionDestinationLocalHost) &&
        (record_format == bcmStatEvictionRecordFormatPhysical) && (dma_fifo != CRPS_MGMT_INVALID_DMA_FIFO))
    {
        *valid = TRUE;
    }
    else
    {
        *valid = FALSE;
    }

exit:
    SHR_FUNC_EXIT;

}

/** see .h file */
shr_error_e
dnx_crps_mgmt_stat_id_find(
    int unit,
    int core_id,
    int engine_id,
    int type_id,
    uint32 counter_id,
    int ctr_set_offset,
    int *object_stat_id)
{
    int set_size, database_id;
    uint32 type_offset;
    int sampling_offset = 0;
    uint32 first_index;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.counter_set_size.get(unit, core_id, engine_id, &set_size));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.database_id.get(unit, core_id, engine_id, &database_id));
    SHR_IF_ERR_EXIT(dnx_crps_db.database.type.offset.get(unit, core_id, database_id, type_id, &type_offset));
    type_offset = type_offset * set_size;
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.log_obj_id_range.
                    logical_object_id_first.get(unit, core_id, engine_id, &first_index));
    /** reverse the calculation:
     *    logical_object_id =
     *    (object_stat_id - cfg_base_obj_id)*set_size +
     *    ctr_set_offset + type_offset +
     *      sampling_offset;
     *  counter_id = logical_object_id - first_index;
     *  
     *  currently cfg_base_obj_id = 0
     *  no need to use it in the formula as it does not impact
     *  refer to dnx_crps_mgmt_counter_find
     *       */
    *object_stat_id = (counter_id - sampling_offset - type_offset - ctr_set_offset + first_index) / set_size;

exit:
    SHR_FUNC_EXIT;

}

/** see .h file */
shr_error_e
dnx_crps_mgmt_stat_object_id_set_id_get(
    int unit,
    int core_id,
    int database_id,
    int stat_object_id,
    int *object_set_idx_get)
{
    int object_set_idx = DNX_CRPS_MGMT_SET_INVALID;
    int set_idx;
    uint32 stat_id_start, stat_id_end;

    SHR_FUNC_INIT_VARS(unit);

    for (set_idx = 0; set_idx < dnx_data_crps.engine.max_nof_counter_sets_get(unit); set_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_crps_db.database.set.start.get(unit, core_id, database_id, set_idx, &stat_id_start));
        SHR_IF_ERR_EXIT(dnx_crps_db.database.set.end.get(unit, core_id, database_id, set_idx, &stat_id_end));

        if (stat_object_id >= stat_id_start && stat_object_id <= stat_id_end)
        {
            object_set_idx = set_idx;
        }
    }

    *object_set_idx_get = object_set_idx;

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_is_multi_sets_database(
    int unit,
    int core_id,
    int database_id,
    int *is_multi_set_database_get)
{
    int set_idx, current_set_size;
    int is_multi_set_database = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_crps.engine.feature_get(unit, dnx_data_crps_engine_multi_sets_is_supported))
    {
        /** Check if at least one of the set-config sw-state structure were set*/
        for (set_idx = 0; set_idx < dnx_data_crps.engine.max_nof_counter_sets_get(unit); set_idx++)
        {

            SHR_IF_ERR_EXIT(dnx_crps_db.database.set.size.get(unit, core_id, database_id, set_idx, &current_set_size));

            if (current_set_size != DNX_CRPS_MGMT_ENGINE_FIELDS_INVALID)
            {
                is_multi_set_database = TRUE;
            }
        }
    }

    *is_multi_set_database_get = is_multi_set_database;

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_find(
    int unit,
    int core_id,
    int engine_id,
    int type_id,
    int stat_object_id,
    int ctr_set_offset,
    int *found,
    uint32 *local_counter)
{
    int database_id;
    uint32 first_index = 0, last_index = 0, logical_object_id = 0;
    int is_multi_sets_database;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.database_id.get(unit, core_id, engine_id, &database_id));
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_is_multi_sets_database(unit, core_id, database_id, &is_multi_sets_database));

    /*
     * for multi-sets database, logical_object_id = the index of the counter in the database 
     */
    if (is_multi_sets_database)
    {
        SHR_IF_ERR_EXIT(crps_mgmt_set_logical_object_id_get
                        (unit, core_id, engine_id, stat_object_id, ctr_set_offset, &logical_object_id));
    }
    /*
     * for multi-types database, logical_object_id = the index of the counter in the database + base-stat-id*set-size
     */
    else
    {
        SHR_IF_ERR_EXIT(crps_mgmt_type_logical_object_id_get
                        (unit, core_id, engine_id, type_id, stat_object_id, ctr_set_offset, &logical_object_id));
    }

    /** get the range of the engine and verify that the logical_object_id is in the range */
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.log_obj_id_range.logical_object_id_first.get
                    (unit, core_id, engine_id, &first_index));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.log_obj_id_range.logical_object_id_last.get
                    (unit, core_id, engine_id, &last_index));

    if ((logical_object_id < first_index) || (logical_object_id > last_index))
    {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U
                   (unit,
                    "object_stat_id=%d out of range. core=%d, engine=%d, logical_object_id=%d, first_index=%d, last_index=%d\n"),
                   stat_object_id, core_id, engine_id, logical_object_id, first_index, last_index));
        *found = FALSE;
    }
    else
    {
        *local_counter = logical_object_id - first_index;
        *found = TRUE;
    }

exit:
    SHR_FUNC_EXIT;

}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_id_calc(
    int unit,
    int core_id,
    int engine_id,
    int type_id,
    int object_stat_id,
    int stat,
    int *found,
    int *counter_id,
    int *sub_count)
{
    int nof_counters_per_shadow;
    int ctr_set_offset;
    int is_bytes;
    bcm_stat_counter_format_type_t format;
    uint32 local_counter;
    SHR_FUNC_INIT_VARS(unit);

    ctr_set_offset = _SHR_STAT_COUNTER_STAT_OFFSET_GET(stat);
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_find
                    (unit, core_id, engine_id, type_id, object_stat_id, ctr_set_offset, found, &local_counter));

    if (*found)
    {
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.format.get(unit, core_id, engine_id, &format));
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_nof_counters_in_counter_shaddow_get(unit, format, &nof_counters_per_shadow));
        /** extruct the physical address (counter_id)*/
        *counter_id = local_counter / nof_counters_per_shadow;

        /** if wide mode (one counter per entry), we read from ctr1 (sub_count=0). */
        /** if double entry we read even counters from ctr1, odd from ctr2*/
        /** if FormatPacketsAndBytes - the packets read from ctr1, bytes from ctr2*/
        is_bytes = _SHR_STAT_COUNTER_STAT_IS_BYTES_GET(stat);
        if ((DNX_CRPS_MGMT_IS_DOUAL_COUNTER_FORMAT(unit, format) == TRUE) && (is_bytes == TRUE))
        {
            *sub_count = 1;
        }
        else if ((nof_counters_per_shadow == 2) && (local_counter % 2 == 1))
        {
            *sub_count = 1;
        }
        else
        {
            *sub_count = 0;
        }
        *found = TRUE;
    }

exit:
    SHR_FUNC_EXIT;

}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_explicit_get(
    int unit,
    uint32 flags,
    int core_id,
    int engine_id,
    int type_id,
    int object_stat_id,
    int nstat,
    int *stat_arr,
    int *nof_found,
    int *found_arr,
    bcm_stat_counter_output_data_t * output_data)
{
    int ctr_idx;
    int counter_ids[DNX_CRPS_MGMT_MAX_NSTAT];
    int sub_counter_ids[DNX_CRPS_MGMT_MAX_NSTAT];
    int found = FALSE;
    int found_count = 0;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(counter_ids, -1, sizeof(int) * DNX_CRPS_MGMT_MAX_NSTAT);
    sal_memset(sub_counter_ids, -1, sizeof(int) * DNX_CRPS_MGMT_MAX_NSTAT);
    /** 1. run over all stats and find the sw counters to read from */
    /** run over all engines belong to the input core_id */
    for (ctr_idx = 0; ctr_idx < nstat; ctr_idx++)
    {
        /** get the sw counter id */
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_id_calc
                        (unit, core_id, engine_id, type_id, object_stat_id, stat_arr[ctr_idx], &found,
                         &counter_ids[ctr_idx], &sub_counter_ids[ctr_idx]));
        found_count += found;
        /** if found_arr != NULL,  update the found_arr. Else, print error if counter wasn't found */
        if (found_arr != NULL)
        {
            found_arr[ctr_idx] = found;
        }
        else
        {
            if (found == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Can't get counter. Invalid inputs: unit %d core %d engine %d type %d object_id %u,stat_offset %d",
                             unit, core_id, engine_id, type_id, object_stat_id,
                             _SHR_STAT_COUNTER_STAT_OFFSET_GET(stat_arr[ctr_idx]));
            }
        }
    }

    /*
     * only if sw counters were found for the given input parameters, do the next steps 
     */
    if (found_count > 0)
    {
        SHR_IF_ERR_EXIT(dnx_crps_eviction_counters_get
                        (unit, flags, core_id, engine_id, nstat, counter_ids, sub_counter_ids, output_data));
    }

    if (nof_found != NULL)
    {
        *nof_found = found_count;
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_get(
    int unit,
    uint32 flags,
    bcm_stat_counter_input_data_t * stat_counter_input_data,
    bcm_stat_counter_output_data_t * stat_counter_output_data)
{
    int ctr_idx;
    int temp_found_arr[DNX_CRPS_MGMT_MAX_NSTAT] = { FALSE };
    int found_arr[DNX_CRPS_MGMT_MAX_NSTAT] = { 0 };
    int found_count = 0, temp_count;
    int valid;
    int type_valid;
    int engine_id;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * run over all engines belong to the input core_id and database_id untill 
     * all stats were updated or untill go over all engines of the database
     */
    SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.get
                    (unit, stat_counter_input_data->core_id, stat_counter_input_data->database_id, &engine_id));
    /** verify that all engines in the database are disabled */
    while (engine_id != DNX_CRPS_MGMT_ENGINE_INVALID)
    {
        /** check if engine configuration is valid, if connected to local host and if the record type is physical */
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_engine_eviction_valid_get
                        (unit, stat_counter_input_data->core_id, engine_id, &valid));

        /** if valid */
        if (valid == TRUE)
        {
            /** get if type is valid */
            SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.type_hw_valid.get
                            (unit, stat_counter_input_data->core_id, engine_id, stat_counter_input_data->type_id,
                             &type_valid));

            /** check if the type is addmitted (valid) to the engine */
            if (type_valid == TRUE)
            {
                temp_count = 0;
                /** call counter explicit get. It will found the sw counter to read from and update its value in the  output strucutre */
                /** in addition, it return array of stats that was found */
                SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_explicit_get
                                (unit, flags, stat_counter_input_data->core_id, engine_id,
                                 stat_counter_input_data->type_id, stat_counter_input_data->counter_source_id,
                                 stat_counter_input_data->nstat, (int *) stat_counter_input_data->stat_arr, &temp_count,
                                 temp_found_arr, stat_counter_output_data));

                /** count how many stats were found and updated */
                if (temp_count > 0)
                {
                    for (ctr_idx = 0; ctr_idx < stat_counter_input_data->nstat; ctr_idx++)
                    {
                        if (temp_found_arr[ctr_idx] != FALSE)
                        {
                            found_arr[ctr_idx] = temp_found_arr[ctr_idx];
                            /** clear the temp_found_arr for next round */
                            temp_found_arr[ctr_idx] = FALSE;
                            found_count++;
                        }
                    }
                    /** if all stats were updated, break*/
                    if (found_count == stat_counter_input_data->nstat)
                    {
                        break;
                    }
                }
            }
        }
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.next_engine_id.get
                        (unit, stat_counter_input_data->core_id, engine_id, &engine_id));
    }

    if (found_count != stat_counter_input_data->nstat)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "not all stats could be read. nstat=%d, found_count=%d, object_stat_id=%d \n"),
                   stat_counter_input_data->nstat, found_count, stat_counter_input_data->counter_source_id));
        for (ctr_idx = 0; ctr_idx < stat_counter_input_data->nstat; ctr_idx++)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "stat[%d] found=%d \n"), ctr_idx, found_arr[ctr_idx]));
        }
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "get counter error. see prev msg \n");
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_background_collection_set(
    int unit,
    int enable)
{
    int is_active;
    shr_thread_manager_handler_t crps_eviction_handler;

    SHR_FUNC_INIT_VARS(unit);
    /** Get thread handle from SW-State */
    SHR_IF_ERR_EXIT(dnx_crps_eviction_db.bg_thread_data.crps_eviction_thread_handler.get(unit, &crps_eviction_handler));

    SHR_IF_ERR_EXIT(shr_thread_manager_is_active_get(crps_eviction_handler, &is_active));
    /** only if need to change the state - do it */
    if (enable != is_active)
    {
        if (enable == TRUE)
        {
            SHR_IF_ERR_EXIT(dnx_crps_eviction_bg_thread_attach(unit));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_crps_eviction_bg_thread_detach(unit));
        }
    }

exit:
    SHR_FUNC_EXIT;

}

/** see .h file */
shr_error_e
dnx_crps_mgmt_network_eviction_pkt_size_set(
    int unit,
    int pkt_size_in_KB)
{
    uint32 entry_handle_id;
    int field_val;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_IRE_SCAN_CFG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, BCM_CORE_ALL);

    /** The actual pkt size is 3Kb << IRE_PACKET_SIZEf*/
    /** arg is equal to 3/6/12 for 3KB/6KB/12KB respectively*/
    field_val = utilex_log2_round_up(pkt_size_in_KB / 3);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IRE_PACKET_SIZE, INST_SINGLE, field_val);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_network_eviction_pkt_size_get(
    int unit,
    int *pkt_size_in_KB)
{
    uint32 entry_handle_id;
    uint32 field_val;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_IRE_SCAN_CFG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, BCM_CORE_ALL);

    /** Setting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IRE_PACKET_SIZE, INST_SINGLE, &field_val);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** The actual pkt size is 3Kb << IRE_PACKET_SIZEf*/
    /** pkt_size_in_KB is equal to 3/6/12 for 3KB/6KB/12KB respectively*/
    *pkt_size_in_KB = (1 << field_val) * 3;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_background_collection_get(
    int unit,
    int *is_active)
{
    shr_thread_manager_handler_t crps_eviction_handler;
    SHR_FUNC_INIT_VARS(unit);
    /** Get thread handle from SW-State */
    SHR_IF_ERR_EXIT(dnx_crps_eviction_db.bg_thread_data.crps_eviction_thread_handler.get(unit, &crps_eviction_handler));

    SHR_IF_ERR_EXIT(shr_thread_manager_is_active_get(crps_eviction_handler, is_active));

exit:
    SHR_FUNC_EXIT;

}

/** see .h file */
shr_error_e
dnx_crps_mgmt_meter_engine_attach_detach(
    int unit,
    int core_id,
    int engine_id,
    dnx_crps_mgmt_shmem_section_t section,
    int is_ingress,
    int attach)
{
    const dnx_data_crps_engine_engines_info_t *engine_info;
    int core_idx;
    int conf_started;
    dnx_crps_mgmt_shmem_section_t get_section;
    dbal_enum_value_field_meter_crps_smem_src_types_e type;
    SHR_FUNC_INIT_VARS(unit);

    /** check valid engine id */
    if (engine_id >= dnx_data_crps.engine.nof_engines_get(unit) || engine_id < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect engine_id=%d, possible values 0..21 \n", engine_id);
    }
    engine_info = dnx_data_crps.engine.engines_info_get(unit, engine_id);

    /** check engine id is one of the engines that are shared between crps and meter. */
    if (engine_info->meter_shared == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect engine_id=%d, is not valid for meter\n", engine_id);
    }

    /** go over all cores or specific core and update the sw state and verify that the engine is not in used for crps already */
    DNXCMN_CORES_ITER(unit, core_id, core_idx)
    {
        /** detach */
        if (attach == FALSE)
        {
            /** sw state do not has the banks resolution. 
                            if the section is not ALL, need to check first what is the other section use. 
                            only if it used for crps, we can sigh the entrire engine as CRPS */
            if (((section == dnx_crps_mgmt_shmem_section_low) || (section == dnx_crps_mgmt_shmem_section_high)) ||
                ((section == dnx_crps_mgmt_shmem_section_all)
                 && (dnx_data_meter.mem_mgmt.nof_banks_in_big_engine_get(unit) == 1)))
            {
                if (dnx_data_meter.mem_mgmt.nof_banks_in_big_engine_get(unit) == 1)
                {
                    get_section = dnx_crps_mgmt_shmem_section_all;
                }
                else
                {
                    get_section =
                        (section ==
                         dnx_crps_mgmt_shmem_section_low) ? dnx_crps_mgmt_shmem_section_high :
                        dnx_crps_mgmt_shmem_section_low;
                }
                SHR_IF_ERR_EXIT(dnx_crps_engine_bank_src_hw_get(unit, core_id, engine_id, get_section, &type));
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.used_for_meter.set(unit, core_idx, engine_id,
                                                                    (type !=
                                                                     DBAL_ENUM_FVAL_METER_CRPS_SMEM_SRC_TYPES_CRPS)));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.used_for_meter.set(unit, core_idx, engine_id, FALSE));
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_crps_db.proc.config_started.get(unit, core_id, engine_id, &conf_started));
            /** if the engine is already used for crps, it can't be used for meter. */
            /** even if the engine configuration started (and not done yet), it is forbidden to use it for meter */
            if (conf_started == TRUE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "core=%d, engine=%d, is already in used for crps.\n", core_idx, engine_id);
            }

            SHR_IF_ERR_EXIT(dnx_crps_db.proc.used_for_meter.set(unit, core_idx, engine_id, TRUE));
        }
    }

    if (attach == FALSE)
    {
        /** set CRPS type according to dbal enum in order to update hw */
        type = DBAL_ENUM_FVAL_METER_CRPS_SMEM_SRC_TYPES_CRPS;
    }
    else
    {
        /** set the type according to dbal enum in order to update hw */
        type = is_ingress ? DBAL_ENUM_FVAL_METER_CRPS_SMEM_SRC_TYPES_METER_INGRESS :
            DBAL_ENUM_FVAL_METER_CRPS_SMEM_SRC_TYPES_METER_EGRESS;
    }
    SHR_IF_ERR_EXIT(dnx_crps_engine_bank_src_hw_set(unit, core_id, engine_id, section, type));

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_meter_engine_available_verify(
    int unit,
    int core_id,
    int engine_id,
    dnx_crps_mgmt_shmem_section_t section)
{
    dbal_enum_value_field_meter_crps_smem_src_types_e type;
    const dnx_data_crps_engine_engines_info_t *engine_info;

    SHR_FUNC_INIT_VARS(unit);

    engine_info = dnx_data_crps.engine.engines_info_get(unit, engine_id);

    /** check engine id is one of the engines that are shared between crps and meter. */
    if (engine_info->meter_shared == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect engine_id=%d, is not valid for meter\n", engine_id);
    }

    if (dnx_data_meter.mem_mgmt.nof_banks_in_big_engine_get(unit) != 1)
    {
        /** if section_all - need to do the check for both sections */
        if (section == dnx_crps_mgmt_shmem_section_all)
        {
            section = dnx_crps_mgmt_shmem_section_low;
            /** get from HW the type that the section (bank) is used for (crps, meter_ing, meter_eg)*/
            SHR_IF_ERR_EXIT(dnx_crps_engine_bank_src_hw_get(unit, core_id, engine_id, section, &type));
            /*
             * if type is one of the meters types, it means that it is not available.
             */
            if (type != DBAL_ENUM_FVAL_METER_CRPS_SMEM_SRC_TYPES_CRPS)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "engine_id=%d, core=%d, section=%d is already in use for meter\n",
                             engine_id, core_id, section);
            }
            section = dnx_crps_mgmt_shmem_section_high;
        }
    }
    /** get from HW the type that the section (bank) is used for (crps, meter_ing, meter_eg)*/
    SHR_IF_ERR_EXIT(dnx_crps_engine_bank_src_hw_get(unit, core_id, engine_id, section, &type));
    /*
     * if type is one of the meters types, it means that it is not available. 
     */
    if (type != DBAL_ENUM_FVAL_METER_CRPS_SMEM_SRC_TYPES_CRPS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "engine_id=%d, core=%d, section=%d is already in use for meter\n",
                     engine_id, core_id, section);
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_overflow_counter_mode_set(
    int unit,
    bcm_stat_engine_t * engine,
    uint32 overflow_mode)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_OVERFLOW_COUNTER, &entry_handle_id));
    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_ID, engine->engine_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, engine->core_id);

    switch (overflow_mode)
    {
        case BCM_STAT_COUNTER_OVERFLOW_MODE_RESET:
            /** reset to 0 */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_ENABLE_CLEAR_ON_OVF, INST_SINGLE,
                                         TRUE);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_OVF_RESET_VALUE, INST_SINGLE,
                                         DBAL_ENUM_FVAL_CRPS_ENGINE_OVF_RESET_VALUE_RESET_TO_0);
            break;
        case BCM_STAT_COUNTER_OVERFLOW_MODE_WRAP_AROUND:
            /** wrap around */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_ENABLE_CLEAR_ON_OVF, INST_SINGLE,
                                         FALSE);
            break;
        case BCM_STAT_COUNTER_OVERFLOW_MODE_FIXED_AT_MAX:
            /** stuck all at ones */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_ENABLE_CLEAR_ON_OVF, INST_SINGLE,
                                         TRUE);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_OVF_RESET_VALUE, INST_SINGLE,
                                         DBAL_ENUM_FVAL_CRPS_ENGINE_OVF_RESET_VALUE_FIXED_AT_MAX);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, " invalid overflow mode arg (=%d)\n", overflow_mode);
            break;
    }
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_overflow_counter_mode_get(
    int unit,
    bcm_stat_engine_t * engine,
    uint32 *overflow_mode)
{
    uint32 enable_clear = 0;
    uint32 reset_value = 0;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_OVERFLOW_COUNTER, &entry_handle_id));
    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_ID, engine->engine_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, engine->core_id);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_ENABLE_CLEAR_ON_OVF, INST_SINGLE,
                               &enable_clear);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_OVF_RESET_VALUE, INST_SINGLE,
                               &reset_value);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (enable_clear == FALSE)
    {
        *overflow_mode = BCM_STAT_COUNTER_OVERFLOW_MODE_WRAP_AROUND;
    }
    else
    {
        if (reset_value == DBAL_ENUM_FVAL_CRPS_ENGINE_OVF_RESET_VALUE_RESET_TO_0)
        {
            *overflow_mode = BCM_STAT_COUNTER_OVERFLOW_MODE_RESET;
        }
        else
        {
            *overflow_mode = BCM_STAT_COUNTER_OVERFLOW_MODE_FIXED_AT_MAX;
        }

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_filter_group_set(
    int unit,
    uint32 flags,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_counter_group_filter_t filter,
    int is_active)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** IRPP source */
    if (key->source == bcmStatCounterInterfaceIngressOam || key->source == bcmStatCounterInterfaceIngressReceivePp)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_IRPP_FILTER_MASK, &entry_handle_id));
    }
    /** TM source */
    else if (key->source == bcmStatCounterInterfaceIngressVoq)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_TM_FILTER_MASK, &entry_handle_id));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect source=%d \n", key->source);
    }

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_COMMAND_ID, key->command_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FILTER_GROUP_ID, filter);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, key->core_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FILTER_MASK_ENABLE, INST_SINGLE, is_active);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_filter_group_get(
    int unit,
    uint32 flags,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_counter_group_filter_t filter,
    int *is_active)
{
    uint32 active;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** IRPP source */
    if (key->source == bcmStatCounterInterfaceIngressOam || key->source == bcmStatCounterInterfaceIngressReceivePp)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_IRPP_FILTER_MASK, &entry_handle_id));
    }
    /** TM source */
    else if (key->source == bcmStatCounterInterfaceIngressVoq)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_TM_FILTER_MASK, &entry_handle_id));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect source=%d \n", key->source);
    }
    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_COMMAND_ID, key->command_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FILTER_GROUP_ID, filter);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, key->core_id);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FILTER_MASK_ENABLE, INST_SINGLE, &active);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *is_active = active;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_generation_verify(
    int unit,
    int core_id,
    bcm_stat_counter_interface_type_t source_type,
    int command_id,
    int type_id,
    int object_stat_id)
{
    int database_command_id;
    int engine_id;
    int valid;
    int set_size;
    int found_first_ctr_set_idx, found_last_ctr_set_idx;
    uint32 local_counter;
    int core_idx;
    bcm_stat_counter_interface_type_t database_source;
    SHR_FUNC_INIT_VARS(unit);

    /** go over all engines and search an engine with the input command_id and the type_id that its range match to the 
        input object_stat_id. Note: Since we don't have data about the source, we allow all sources */
    DNXCMN_CORES_ITER(unit, core_id, core_idx)
    {
        found_first_ctr_set_idx = FALSE;
        found_last_ctr_set_idx = FALSE;
        for (engine_id = 0; engine_id < dnx_data_crps.engine.nof_engines_get(unit); engine_id++)
        {
            DNX_CRPS_MGMT_ENGINE_VALID_GET(unit, core_idx, engine_id, valid);
            /** if engine is valid */
            if (valid)
            {
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.src_type.get(unit, core_idx, engine_id, &database_source));
                   /** if the source match */
                if (database_source == source_type)
                {
                    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.command_id.get
                                    (unit, core_idx, engine_id, &database_command_id));

                       /** if command id match */
                    if (database_command_id == command_id)
                    {
                        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.type_hw_valid.get
                                        (unit, core_idx, engine_id, type_id, &valid));

                        /** if type is valid */
                        if (valid)
                        {
                            if (found_first_ctr_set_idx == FALSE)
                            {
                                /** check if the first counter_set index was found */
                                SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_find(unit, core_idx, engine_id, type_id,
                                                                           object_stat_id, 0,
                                                                           &found_first_ctr_set_idx, &local_counter));
                            }

                            SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.counter_set_size.get
                                            (unit, core_idx, engine_id, &set_size));

                            /** ig set_size=1, the first and the last are the same */
                            if (set_size == 1)
                            {
                                found_last_ctr_set_idx = found_first_ctr_set_idx;
                            }
                            else
                            {
                                if (found_last_ctr_set_idx == FALSE)
                                {
                                    /** check if the last counter_set index was found */
                                    SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_find(unit, core_idx, engine_id, type_id,
                                                                               object_stat_id, (set_size - 1),
                                                                               &found_last_ctr_set_idx,
                                                                               &local_counter));
                                }
                            }
                        }

                        /** if the object_stat_id, first and last offset in the counter_set found */
                        if (found_first_ctr_set_idx == TRUE && found_last_ctr_set_idx == TRUE)
                        {
                            break;
                        }
                    }
                }
            }
        }

        /** error is triggered if engines not found for the entire counter_set */
        if (found_first_ctr_set_idx == FALSE || found_last_ctr_set_idx == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "No engine is possible to count core_id=%d, command_id=%d, type_id=%d, object_stat_id=%d\n",
                         core_idx, command_id, type_id, object_stat_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_database_create(
    int unit,
    uint32 flags,
    bcm_core_t core_id,
    int *database_id)
{
    uint32 res_flags = 0;
    SHR_FUNC_INIT_VARS(unit);

    res_flags = (flags & BCM_STAT_DATABASE_CREATE_WITH_ID) ? DNX_ALGO_RES_ALLOCATE_WITH_ID : 0;
    SHR_IF_ERR_EXIT(dnx_crps_db.database_res.allocate_single(unit, core_id, res_flags, NULL, database_id));

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_database_destroy(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database)
{
    int next_engine_id, current_engine_id;
    int set_idx;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.get
                    (unit, database->core_id, database->database_id, &next_engine_id));
    /** detach the entire engines in the database */
    while (next_engine_id != DNX_CRPS_MGMT_ENGINE_INVALID)
    {
        current_engine_id = next_engine_id;
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.next_engine_id.get
                        (unit, database->core_id, current_engine_id, &next_engine_id));
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_engine_detach(unit, 0, database->core_id, current_engine_id));
    }

    /** clear base engine of the database */
    SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.set(unit, database->core_id, database->database_id,
                                                         DNX_CRPS_MGMT_ENGINE_INVALID));
    /** Clear counter-set's size */
    for (set_idx = 0; set_idx < dnx_data_crps.engine.max_nof_counter_sets_get(unit); set_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_crps_db.database.set.size.set
                        (unit, database->core_id, database->database_id, set_idx, DNX_CRPS_MGMT_ENGINE_FIELDS_INVALID));
    }

    /** free from resource manager the database */
    SHR_IF_ERR_EXIT(dnx_crps_db.database_res.free_single(unit, database->core_id, database->database_id, NULL));
exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_engine_attach(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    int engine_id)
{
    int counter;
    int next_engine_id, current_engine_id, base_engine;
    const dnx_data_crps_engine_engines_info_t *engine_info = dnx_data_crps.engine.engines_info_get(unit, engine_id);
    bcm_stat_engine_t engine;

    SHR_FUNC_INIT_VARS(unit);
    /** update sw state db */
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.used_for_meter.set(unit, database->core_id, engine_id, FALSE));
    /** since it is the first API in the configuration sequence, it will set the config started flag */
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.config_started.set(unit, database->core_id, engine_id, TRUE));

    /** set the database_id */
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.database_id.set
                    (unit, database->core_id, engine_id, database->database_id));

    /** if it is the first engine in database, need to update base engine */
    SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.get(unit, database->core_id, database->database_id, &base_engine));
    if (base_engine == DNX_CRPS_MGMT_ENGINE_INVALID)
    {
        SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.set
                        (unit, database->core_id, database->database_id, engine_id));
        /** set prev and next engine as invalid if the engine is the first engine in database */
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.next_engine_id.set
                        (unit, database->core_id, engine_id, DNX_CRPS_MGMT_ENGINE_INVALID));
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.prev_engine_id.set
                        (unit, database->core_id, engine_id, DNX_CRPS_MGMT_ENGINE_INVALID));

    }
    else
    {
        /** update the prev_engine and next engine for the current engine */
        next_engine_id = base_engine;
        counter = 0;
        do
        {
            current_engine_id = next_engine_id;
            SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.next_engine_id.get
                            (unit, database->core_id, current_engine_id, &next_engine_id));
            counter++;
        }
        while ((next_engine_id != DNX_CRPS_MGMT_ENGINE_INVALID) &&
               (counter <= dnx_data_crps.engine.nof_engines_get(unit)));

        if (counter > dnx_data_crps.engine.nof_engines_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, " loop in next_engine_id linked list\n");
        }

        /** exit loop when we got to the last engine in the database. */
        /** The last (cuurent) engine should point to the new engine that attached */
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.next_engine_id.set
                        (unit, database->core_id, current_engine_id, engine_id));
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.prev_engine_id.set
                        (unit, database->core_id, engine_id, current_engine_id));
    }

    if (engine_info->meter_shared == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_crps_engine_bank_src_hw_set
                        (unit, database->core_id, engine_id, dnx_crps_mgmt_shmem_section_all,
                         DBAL_ENUM_FVAL_METER_CRPS_SMEM_SRC_TYPES_CRPS));
    }

    /** first enable access to memories, and then reset memory */
    engine.core_id = database->core_id;
    engine.engine_id = engine_id;
    SHR_IF_ERR_EXIT(dnx_crps_engine_memory_access_enable(unit, &engine, TRUE));
    /** reset counters - operation is made in attach and detach, in order to support engine usage transiotion from/to meter */
    SHR_IF_ERR_EXIT(dnx_crps_engine_hw_counters_reset(unit, &engine));

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_engine_detach(
    int unit,
    uint32 flags,
    int core_id,
    int engine_id)
{
    bcm_stat_engine_t engine;

    SHR_FUNC_INIT_VARS(unit);
    engine.core_id = core_id;
    engine.engine_id = engine_id;
   /** disable the engine */
    SHR_IF_ERR_EXIT(dnx_crps_engine_enable_hw_set(unit, &engine, FALSE, FALSE));
   /** reset counters */
    SHR_IF_ERR_EXIT(dnx_crps_engine_hw_counters_reset(unit, &engine));
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_engine_reset(unit, &engine));

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_database_control_set(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_database_control_t control,
    uint32 arg)
{
    bcm_stat_engine_t engine;
    SHR_FUNC_INIT_VARS(unit);

    engine.core_id = database->core_id;
    SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.get
                    (unit, database->core_id, database->database_id, &engine.engine_id));
    /** set the control for each engine in database */
    while (engine.engine_id != DNX_CRPS_MGMT_ENGINE_INVALID)
    {
        switch (control)
        {
            case bcmStatCounterDatabaseClearAll:
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
                        SHR_IF_ERR_EXIT(dnx_crps_ctrs_db_counter_clear(unit, engine.core_id, engine.engine_id));
                    }
                    /** if hw clear only set or none of the clear flags set ==> need to clear the hw*/
                    if ((flags & BCM_STAT_COUNTER_CONTROL_CLEAR_HW_ONLY) ||
                        (((flags & BCM_STAT_COUNTER_CONTROL_CLEAR_SW_ONLY) == FALSE) &&
                         ((flags & BCM_STAT_COUNTER_CONTROL_CLEAR_HW_ONLY) == FALSE)))
                    {
                        /** clear hw memory. */
                        SHR_IF_ERR_EXIT(dnx_crps_engine_hw_counters_reset(unit, &engine));
                    }
                }
                break;
            case bcmStatCounterOverflowMode:
                SHR_IF_ERR_EXIT(dnx_crps_mgmt_overflow_counter_mode_set(unit, &engine, arg));
                break;

            case bcmStatCounterDatabaseRateCalculationEnable:
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.rate.
                                rate_calculation_enable.set(unit, engine.core_id, engine.engine_id, (int) arg));
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, " invalid control (=%d)\n", control);
                break;
        }

        /** get next engine */
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.next_engine_id.get
                        (unit, engine.core_id, engine.engine_id, &engine.engine_id));
    }

exit:
    SHR_FUNC_EXIT;

}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_database_control_get(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_database_control_t control,
    uint32 *arg)
{
    bcm_stat_engine_t engine;

    SHR_FUNC_INIT_VARS(unit);

    engine.core_id = database->core_id;
    /** get arg from base engine of the database */
    SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.get
                    (unit, database->core_id, database->database_id, &engine.engine_id));

    switch (control)
    {
        case bcmStatCounterOverflowMode:
            SHR_IF_ERR_EXIT(dnx_crps_mgmt_overflow_counter_mode_get(unit, &engine, arg));
            break;

        case bcmStatCounterDatabaseRateCalculationEnable:
            SHR_IF_ERR_EXIT(dnx_crps_db.proc.rate.
                            rate_calculation_enable.get(unit, engine.core_id, engine.engine_id, (int *) arg));
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
dnx_crps_mgmt_oam_dummy_reply_mechanism_init(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_OAM_DUMMY_REPLY, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DUMMY_ENABLE, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_meter_shmem_bank_id_get(
    int unit,
    int engine_id,
    dnx_crps_mgmt_shmem_section_t section,
    int *bank_id)
{
    int multi_val = (dnx_data_meter.mem_mgmt.sections_offset_get(unit) == 1) ? 2 : 1;
    int section_offset = (section == dnx_crps_mgmt_shmem_section_high) ?
        dnx_data_meter.mem_mgmt.sections_offset_get(unit) : 0;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_meter.mem_mgmt.nof_banks_in_big_engine_get(unit) == 1)
    {
        *bank_id = engine_id - dnx_data_meter.mem_mgmt.base_big_engine_for_meter_get(unit);
    }

    else
    {
        *bank_id =
            ((engine_id - dnx_data_meter.mem_mgmt.base_big_engine_for_meter_get(unit)) * multi_val) + section_offset;
    }

/** exit: */
    SHR_FUNC_EXIT;
}
