/** \file crps_mgmt.c
 * 
 *  crps module gather all functionality that related to the counter/statisitc processor driver for DNX.
 *  crps_mgmt.c gather the managements CRPS functionality for DNX.
 *  It is external crps file . (it have interfaces with other modules)
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
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
#include <bcm_int/dnx/stat/stat_mgmt.h>
#include <bcm_int/dnx/stat/crps/crps_mgmt.h>
#include <bcm_int/dnx/stat/crps/crps_src_interface.h>
#include "crps_engine.h"
#include "crps_eviction.h"
#include "crps_mgmt_internal.h"
#include <include/bcm_int/dnx/algo/crps/crps_ctrs_db.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/crps_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/crps_eviction_access.h>
#include <include/shared/bitop.h>
#include <include/shared/stat.h>
#include <include/shared/utilex/utilex_integer_arithmetic.h>
#include <include/bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/cosq/cosq_stat.h>
#include <soc/dnxc/dnxc_ha.h>
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

#define CRPS_MGMT_INVALID_LOGICAL_STAT_ID (0xFFFFFFFF)

#define CRPS_MGMT_INVALID_DMA_FIFO (-1)

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
    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        SHR_IF_ERR_EXIT(dnx_crps_db.database_res.create(unit, core_id, &data, NULL));
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
    int nof_cores, core_id, nof_engines, engine_id;
    uint32 bg_thread_enable;
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
        for (core_id = 0; core_id < nof_cores; core_id++)
        {
            for (engine_id = 0; engine_id < nof_engines; engine_id++)
            {
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.prev_engine_id.set(unit, core_id, engine_id,
                                                                              DNX_CRPS_MGMT_ENGINE_INVALID));
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.next_engine_id.set(unit, core_id, engine_id,
                                                                              DNX_CRPS_MGMT_ENGINE_INVALID));
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.counter_set_size.set(unit, core_id, engine_id, -1));
                database_id = engine_id;
                SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.set(unit, core_id, database_id,
                                                                     DNX_CRPS_MGMT_ENGINE_INVALID));
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.dma_fifo.set
                                (unit, core_id, engine_id, CRPS_MGMT_INVALID_DMA_FIFO));
            }
        }
        /** allocate the relevant counters memory space */
        SHR_IF_ERR_EXIT(dnx_crps_ctrs_db_init(unit));
        /** update if bg thread enabled, from soc property only in cold boot */
        bg_thread_enable = dnx_data_crps.eviction.bg_thread_enable_get(unit);
        SHR_IF_ERR_EXIT(dnx_crps_db.bg_thread_enable.set(unit, (int) bg_thread_enable));
        /** init resource manager for counter database */
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_database_res_init(unit));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INIT, "dnx_crps_db.init=TRUE \n");
    }

exit:
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
    SHR_IF_ERR_EXIT(dnx_crps_src_interface_etpp_meta_data_selection_init(unit));

    /** init itm counter pointer settings */
    itm_command_ids = dnx_data_crps.src_interface.command_id_get(unit, bcmStatCounterInterfaceIngressVoq);
    SHR_IF_ERR_EXIT(dnx_cosq_stat_config_init(unit, itm_command_ids->size));

    /** init ingress compensation masks irpp and itm */
    SHR_IF_ERR_EXIT(dnx_crps_src_interface_compensation_init(unit));
    /** init filter drop reason reject masks */
    SHR_IF_ERR_EXIT(dnx_stat_mgmt_reject_filter_masks_init(unit));

    /** init dummy reply mechanism for OAM */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_oam_dummy_reply_mechanism_init(unit));
exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_crps_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

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
        /** first free the sw counters memory */
        SHR_IF_ERR_CONT(dnx_crps_ctrs_db_deinit(unit));

        /*
         * sw state module deinit is done automatically at device deinit 
         */
    }

    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_nof_counters_per_entry_get(
    int unit,
    bcm_stat_counter_format_type_t format,
    int *nof_counters)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(nof_counters, _SHR_E_INTERNAL, "nof_counters");

    if ((format == bcmStatCounterFormatDoublePackets) || (format == bcmStatCounterFormatMaxSize) ||
        (format == bcmStatCounterFormatIngressLatency))
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

/**
 * \brief
 *      search the first engine of the database.  Function get a current engine and go back to the prev engine connected untill getting to the first 
 *      one. IN addition, it count the size of counters in all prev engines
 * \param [in] unit - unit id
 * \param [in] engine -engine_id and core
 * \param [out] total_prev_range -
 *      total number of counters_id (not consider double entry), in all prev engines counted.
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
    uint32 *total_prev_range,
    uint32 *basic_offset)
{
    int prev_engine, cur_engine = engine->engine_id;
    int range_size = 0;
    int engine_index = 0;
    uint32 first_index = 0;
    const dnx_data_crps_engine_engines_info_t *engine_info;
    SHR_FUNC_INIT_VARS(unit);

    while (cur_engine != -1)
    {
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.prev_engine_id.get(unit, engine->core_id, cur_engine, &prev_engine));
        if (prev_engine != -1)
        {
            engine_info = dnx_data_crps.engine.engines_info_get(unit, prev_engine);
            range_size += engine_info->nof_counters;
            /** find the base offset of the database. */
            SHR_IF_ERR_EXIT(dnx_crps_db.proc.log_obj_id_range.logical_object_id_first.get
                            (unit, engine->core_id, prev_engine, &first_index));
        }
        if (engine_index >= dnx_data_crps.engine.nof_engines_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "engine_index=%d, exceed the max engines size\n", engine_index);
        }
        engine_index++;
        cur_engine = prev_engine;
    }

    *total_prev_range = range_size;
    *basic_offset = first_index;
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
dnx_crps_mgmt_range_config(
    int unit,
    bcm_stat_engine_t * engine)
{
    int type_id;
    dnx_engine_interface_info_t inf;
    dnx_cprs_mgmt_range_t type_range[BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES], input_engine_range;
    int type_valid[BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES] = { FALSE };
    uint32 type_offset[BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES] = { 0 };
    const dnx_data_crps_engine_engines_info_t *engine_info;
    int nof_counters_per_entry;
    uint32 total_prev_range = 0;
    uint32 address_base_normalize;
    int ptr_base_normalize = 0;
    uint32 first_index = CRPS_MGMT_INVALID_LOGICAL_STAT_ID; /** logical_object_id units. It represent the base_normalize-cfg. */
    uint32 base_start = CRPS_MGMT_INVALID_LOGICAL_STAT_ID;
    uint32 last_index = 0; /** logical_object_id units */
    uint32 basic_offset = 0;
    dnx_type_config_t type;
    SHR_FUNC_INIT_VARS(unit);

    /** 1. get the engine configuration from db */
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.get(unit, engine->core_id, engine->engine_id, &inf));

    engine_info = dnx_data_crps.engine.engines_info_get(unit, engine->engine_id);

    /** 2. get how many counters in each 68 bits counter entry: 2 counters for pkts_and_pkts and max formats, else 1 counter */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_nof_counters_per_entry_get(unit, inf.format, &nof_counters_per_entry));
    /**  3. find the total previuos number of counters */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_base_engine_get(unit, engine, &total_prev_range, &basic_offset));

     /** 4. find the range of each engine untill the current engine */
    input_engine_range.start = basic_offset + total_prev_range * nof_counters_per_entry;
    input_engine_range.end = input_engine_range.start + (engine_info->nof_counters * nof_counters_per_entry) - 1;

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
 */
   /* *INDENT-ON* */

    for (type_id = 0; type_id < BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES; type_id++)
    {
        SHR_IF_ERR_EXIT(dnx_crps_db.database.type.get(unit, engine->core_id, inf.database_id, type_id, &type));
        if (type.valid == TRUE)
        {
            /** calculate the range of the type */
            /** the range is in logical_object_id units (counter_pointer) */
            /** logical_object_id_first = (Object-ID="start" - cfg-Base-Obj-ID=0) * cfg-Counter-Set-Size 
                                + Counter-Set-Offset(5)=0 + Object-Type-Offset + sampling_offset=0 */
            type_range[type_id].start = (type.start - ptr_base_normalize) * inf.counter_set_size +
                (type.offset * inf.counter_set_size);
            /** logical_object_id_last = (Object-ID="end" - cfg-Base-Obj-ID=0) * cfg-Counter-Set-Size 
                                + Counter-Set-Offset(5)="counter_set_size-1" + Object-Type-Offset + sampling_offset=0 */
            type_range[type_id].end = (type.end - ptr_base_normalize) * inf.counter_set_size +
                (inf.counter_set_size - 1) + (type.offset * inf.counter_set_size);

            base_start = UTILEX_MIN(type_range[type_id].start, base_start);
        }
    }
    /** Case 3: Another special case is when all types start and ends above the engine range. this is relevant for the first engine.
    if so, shift the engine start value to the lowest start value and start the calculation. */
    if ((base_start > input_engine_range.end) && (total_prev_range == 0))
    {
        input_engine_range.start = base_start;
        input_engine_range.end = input_engine_range.start + (engine_info->nof_counters * nof_counters_per_entry) - 1;
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
                    last_index = first_index + (engine_info->nof_counters * nof_counters_per_entry) - 1;
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
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.counter_set_size.set
                    (unit, engine->core_id, engine->engine_id, config->counter_set_size));

    SHR_IF_ERR_EXIT(dnx_crps_engine_hw_config_set(unit, engine, config));
    /** specific configuration per source */
    SHR_IF_ERR_EXIT(dnx_crps_src_config_set(unit, engine->core_id, config));
    /** range handle */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_range_config(unit, engine));

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
    soc_timeout_t to;
    sal_usecs_t timeout_usec = 100;
    int bg_thread_enable_called = FALSE;
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
    SHR_IF_ERR_EXIT(dnx_crps_eviction_collection_handle
                    (unit, engine->core_id, engine->engine_id, &bg_thread_enable_called));

    /** update hw */
    SHR_IF_ERR_EXIT(dnx_crps_engine_enable_hw_set(unit, engine, enable->enable_eviction, enable->enable_counting));

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
    int type_id, valid;
    int current_engine_id, base_engine_id;
    bcm_stat_engine_t engine, src_engine;
    bcm_stat_counter_enable_t enable;
    bcm_stat_counter_interface_t modify_config;

    SHR_FUNC_INIT_VARS(unit);

    /** get the base interface configuration in case of modify scenario */
    if ((flags & BCM_STAT_COUNTER_MODIFY_RANGE_ONLY) == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_interface_get(unit, 0, database, &modify_config));
    }

    /** update db and b for each one of the types */
    for (type_id = 0; type_id < BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES; type_id++)
    {
        if ((flags & BCM_STAT_COUNTER_MODIFY_RANGE_ONLY) == FALSE)
        {
            SHR_IF_ERR_EXIT(dnx_crps_db.database.type.valid.set
                            (unit, database->core_id, database->database_id, type_id,
                             config->type_config[type_id].valid));
            SHR_IF_ERR_EXIT(dnx_crps_db.database.type.
                            offset.set(unit, database->core_id, database->database_id, type_id,
                                       config->type_config[type_id].object_type_offset));
        }
        else
        {
            /** update the modify configuration with the only parameters that can be modified */
            modify_config.type_config[type_id].start = config->type_config[type_id].start;
            modify_config.type_config[type_id].end = config->type_config[type_id].end;
        }
        SHR_IF_ERR_EXIT(dnx_crps_db.database.type.start.set
                        (unit, database->core_id, database->database_id, type_id, config->type_config[type_id].start));
        SHR_IF_ERR_EXIT(dnx_crps_db.database.type.end.set
                        (unit, database->core_id, database->database_id, type_id, config->type_config[type_id].end));
    }

    SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.get
                    (unit, database->core_id, database->database_id, &base_engine_id));
    current_engine_id = base_engine_id;

    /** set the configuration for all engines attached to the database */
    while (current_engine_id != DNX_CRPS_MGMT_ENGINE_INVALID)
    {
        engine.core_id = database->core_id;
        engine.engine_id = current_engine_id;

        /** if new cofiguration, need to configure all the engines in the database */
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
            else
            {
                /** range handle for all of the engines. */
                /**it is user responsibility not to create new range that modify the range of active engines 
                    (except the last active engine that might be increased/decreased) */
                SHR_IF_ERR_EXIT(dnx_crps_mgmt_range_config(unit, &engine));
            }
        }
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.next_engine_id.get
                        (unit, engine.core_id, current_engine_id, &current_engine_id));
    }

    /** for etpp latency measurements, need to select the latency value as input to the engine and not the packet size. */

    if ((_SHR_IS_FLAG_SET(flags, BCM_STAT_COUNTER_MODIFY_RANGE_ONLY) == FALSE) &&
        (config->command_id == dnx_data_crps.latency.etpp_command_id_get(unit)) &&
        (config->source == bcmStatCounterInterfaceEgressTransmitPp))
    {
        if (_SHR_IS_FLAG_SET(flags, BCM_STAT_COUNTER_END_TO_END_LATENCY))
        {
            SHR_IF_ERR_EXIT(dnx_crps_src_interface_etpp_latency_ctr_set(unit, database->core_id, TRUE));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_crps_src_interface_etpp_latency_ctr_set(unit, database->core_id, FALSE));
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
    int type_id;
    int base_engine;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.get(unit, database->core_id, database->database_id, &base_engine));
    /** update config from base_engine sw state db */
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.src_type.get(unit, database->core_id, base_engine, &config->source));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.command_id.get
                    (unit, database->core_id, base_engine, &config->command_id));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.format.get(unit, database->core_id, base_engine, &config->format_type));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.counter_set_size.get
                    (unit, database->core_id, base_engine, &config->counter_set_size));

    /** update from db for each one of the types */
    for (type_id = 0; type_id < BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES; type_id++)
    {
        SHR_IF_ERR_EXIT(dnx_crps_db.database.type.valid.get(unit, database->core_id, database->database_id, type_id,
                                                            &config->type_config[type_id].valid));
        SHR_IF_ERR_EXIT(dnx_crps_db.database.type.offset.get(unit, database->core_id, database->database_id, type_id,
                                                             &config->type_config[type_id].object_type_offset));
        SHR_IF_ERR_EXIT(dnx_crps_db.database.type.start.get(unit, database->core_id, database->database_id, type_id,
                                                            &config->type_config[type_id].start));
        SHR_IF_ERR_EXIT(dnx_crps_db.database.type.end.get(unit, database->core_id, database->database_id, type_id,
                                                          &config->type_config[type_id].end));
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *      check if a given engine is connected to etpp
 * \param [in] unit -
 *     unit id
 * \param [in] core_id -
 *    core_id
 * \param [in] engine_id -
 *    engine_id 
 * \param [out] is_etpp -
 *    is engine connected to source etpp or not. 
 * \return
 *   see shr_error_e
 * \remark
 *   NONE
 * \see
 *   NONE
 */
static shr_error_e
dnx_crps_mgmt_engine_src_is_etpp(
    int unit,
    int core_id,
    int engine_id,
    int *is_etpp)
{
    bcm_stat_counter_interface_type_t src;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.src_type.get(unit, core_id, engine_id, &src));
    if (src == bcmStatCounterInterfaceEgressTransmitPp || src == bcmStatCounterInterfaceEgressOam)
    {
        *is_etpp = TRUE;
    }
    else
    {
        *is_etpp = FALSE;
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
    int etpp_found = FALSE, etm_found = FALSE;
    int is_etpp, is_etm;
    int current_engine_valid_eviction;
    bcm_stat_counter_interface_type_t src;

    SHR_FUNC_INIT_VARS(unit);

    /** get from db the fifo_id parameters */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_engine_eviction_valid_get
                    (unit, engine->core_id, engine->engine_id, &current_engine_valid_eviction));
    if (current_engine_valid_eviction)
    {
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.dma_fifo.get(unit, engine->core_id, engine->engine_id, &fifo_id));
    }

    /** check if the engine we reset is connected to etpp */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_engine_src_is_etpp(unit, engine->core_id, engine->engine_id, &is_etpp));

    /** check if the engine we reset is connected to etm */
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.src_type.get(unit, engine->core_id, engine->engine_id, &src));
    is_etm = (src == bcmStatCounterInterfaceEgressReceiveQueue) ? TRUE : FALSE;

    /** set invalid counter set size */
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.counter_set_size.set(unit, engine->core_id, engine->engine_id, -1));
    /** disconnect the prev and next engines */
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.next_engine_id.get(unit, engine->core_id, engine->engine_id, &next));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.prev_engine_id.get(unit, engine->core_id, engine->engine_id, &prev));

    if (next != DNX_CRPS_MGMT_ENGINE_INVALID)
    {
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.prev_engine_id.set(unit, engine->core_id, next,
                                                                      DNX_CRPS_MGMT_ENGINE_INVALID));
    }
    if (prev != DNX_CRPS_MGMT_ENGINE_INVALID)
    {
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
            if (is_etpp == TRUE)
            {
                /** search for engines connected to etpp. if none, init the etpp metadata selection */
                SHR_IF_ERR_EXIT(dnx_crps_mgmt_engine_src_is_etpp(unit, engine->core_id, engine_idx, &valid));
                if (valid)
                {
                    etpp_found = TRUE;
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
    if ((etpp_found == FALSE) && (is_etpp == TRUE))
    {
        SHR_IF_ERR_EXIT(dnx_crps_src_interface_etpp_meta_data_selection_init(unit));
    }
    /** if no engine connected anymore to etm, we will disable the counter pointer generation from etm */
    if ((etm_found == FALSE) && (is_etm == TRUE))
    {
        SHR_IF_ERR_EXIT(dnx_crps_src_egress_queue_hw_set(unit, engine->core_id, FALSE));
    }
    /** disable engine access to counters memory space */
    SHR_IF_ERR_EXIT(dnx_crps_engine_memory_access_enable(unit, engine, FALSE));
    /** reset the dma fifo sw state to invalid value */
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.
                    dma_fifo.set(unit, engine->core_id, engine->engine_id, CRPS_MGMT_INVALID_DMA_FIFO));
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
    SHR_FUNC_INIT_VARS(unit);

    /** update hw */
    switch (interface->interface_source)
    {
        case bcmStatCounterInterfaceIngressReceivePp:
        case bcmStatCounterInterfaceIngressOam:
            SHR_IF_ERR_EXIT(dnx_crps_src_interface_irpp_expansion_select_set(unit, interface, expansion_select));
            break;
        case bcmStatCounterInterfaceIngressTransmitPp:
            SHR_IF_ERR_EXIT(dnx_crps_src_interface_itpp_expansion_select_set(unit, interface, expansion_select));
            break;
        case bcmStatCounterInterfaceEgressReceivePp:
            SHR_IF_ERR_EXIT(dnx_crps_src_interface_erpp_expansion_select_set(unit, interface, expansion_select));
            break;
        case bcmStatCounterInterfaceEgressTransmitPp:
        case bcmStatCounterInterfaceEgressOam:
            SHR_IF_ERR_EXIT(dnx_crps_src_interface_etpp_metadata_format_config(unit, interface, expansion_select));
            SHR_IF_ERR_EXIT(dnx_crps_src_interface_etpp_expansion_select_set(unit, interface, expansion_select));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, " invalid interface source (=%d)\n", interface->interface_source);
            break;
    }

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
    const dnx_data_crps_expansion_irpp_types_t *irpp_types;
    const dnx_data_crps_expansion_itpp_types_t *itpp_types;
    const dnx_data_crps_expansion_erpp_types_t *erpp_types;
    const dnx_data_crps_expansion_etpp_types_t *etpp_types;
    SHR_FUNC_INIT_VARS(unit);

    /** get data from hw per src block */
    switch (interface->interface_source)
    {
        case bcmStatCounterInterfaceIngressReceivePp:
        case bcmStatCounterInterfaceIngressOam:
            irpp_types = dnx_data_crps.expansion.irpp_types_get(unit);
            SHR_IF_ERR_EXIT(dnx_crps_src_interface_irpp_expansion_select_hw_get(unit, interface, selection_bitmap));
            SHR_IF_ERR_EXIT(dnx_crps_src_interface_expansion_select_get
                            (unit, interface->core_id, interface->interface_source, interface->command_id,
                             selection_bitmap, irpp_types->valid_types, expansion_select));
            break;
        case bcmStatCounterInterfaceIngressTransmitPp:
            itpp_types = dnx_data_crps.expansion.itpp_types_get(unit);
            SHR_IF_ERR_EXIT(dnx_crps_src_interface_itpp_expansion_select_hw_get(unit, interface, selection_bitmap));
            SHR_IF_ERR_EXIT(dnx_crps_src_interface_expansion_select_get
                            (unit, interface->core_id, interface->interface_source, interface->command_id,
                             selection_bitmap, itpp_types->valid_types, expansion_select));
            break;
        case bcmStatCounterInterfaceEgressReceivePp:
            erpp_types = dnx_data_crps.expansion.erpp_types_get(unit);
            SHR_IF_ERR_EXIT(dnx_crps_src_interface_erpp_expansion_select_hw_get(unit, interface, selection_bitmap));
            SHR_IF_ERR_EXIT(dnx_crps_src_interface_expansion_select_get
                            (unit, interface->core_id, interface->interface_source, interface->command_id,
                             selection_bitmap, erpp_types->valid_types, expansion_select));
            break;
        case bcmStatCounterInterfaceEgressTransmitPp:
        case bcmStatCounterInterfaceEgressOam:
            etpp_types = dnx_data_crps.expansion.etpp_types_get(unit);
            SHR_IF_ERR_EXIT(dnx_crps_src_interface_etpp_expansion_select_hw_get(unit, interface, selection_bitmap));
            SHR_IF_ERR_EXIT(dnx_crps_src_interface_expansion_select_get
                            (unit, interface->core_id, interface->interface_source, interface->command_id,
                             selection_bitmap, etpp_types->valid_types, expansion_select));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, " invalid interface source (=%d)\n", interface->interface_source);
            break;
    }

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
    SHR_FUNC_INIT_VARS(unit);

    /** 1. get the relevant params from db in order to build the keys*/
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.get(unit, engine->core_id, engine->engine_id, &inf));
    /** 2. run over all types and update only the ones that are valid */
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
  *   \param [out] selection_arr - array of the unique expansion selection bitmaps
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
    /** itm and etm metadata is smaller than 10 bits, therfore, they don't have expansion selection */
    const dnx_data_crps_expansion_itm_types_t *itm_types;
    const dnx_data_crps_expansion_etm_types_t *etm_types;

    SHR_FUNC_INIT_VARS(unit);

    /** 1. get all the types that are valid for this engine. each type is different interface_key */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_engine_interface_key_get(unit, engine, &nof_keys_all, interface_key_all));
    /** 2. run over all valid interfaces, and get the expansion selection for each one */
    for (all_index = 0; all_index < nof_keys_all; all_index++)
    {
        switch (interface_key_all[all_index].interface_source)
        {
            case bcmStatCounterInterfaceIngressReceivePp:
            case bcmStatCounterInterfaceIngressOam:
                SHR_IF_ERR_EXIT(dnx_crps_src_interface_irpp_expansion_select_hw_get
                                (unit, &interface_key_all[all_index], selection_bitmap));
                break;
            case bcmStatCounterInterfaceIngressTransmitPp:
                SHR_IF_ERR_EXIT(dnx_crps_src_interface_itpp_expansion_select_hw_get
                                (unit, &interface_key_all[all_index], selection_bitmap));
                break;
            case bcmStatCounterInterfaceEgressReceivePp:
                SHR_IF_ERR_EXIT(dnx_crps_src_interface_erpp_expansion_select_hw_get
                                (unit, &interface_key_all[all_index], selection_bitmap));
                break;
            case bcmStatCounterInterfaceEgressTransmitPp:
            case bcmStatCounterInterfaceEgressOam:
                SHR_IF_ERR_EXIT(dnx_crps_src_interface_etpp_expansion_select_hw_get
                                (unit, &interface_key_all[all_index], selection_bitmap));
                break;
            case bcmStatCounterInterfaceIngressVoq:
                itm_types = dnx_data_crps.expansion.itm_types_get(unit);
                SHR_BITSET_RANGE(selection_bitmap, 0, itm_types->total_nof_bits);
                break;
            case bcmStatCounterInterfaceEgressReceiveQueue:
                etm_types = dnx_data_crps.expansion.etm_types_get(unit);
                SHR_BITSET_RANGE(selection_bitmap, 0, etm_types->total_nof_bits);
                break;
            case bcmStatCounterInterfaceOamp:
                /** For OAMP source there are no expansion types and there is
                 *  no expansion, so return in the expansion bits bitmap 0 */
                SHR_BITSET_RANGE(selection_bitmap, 0, 0);
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "invalid interface source (=%d)\n",
                             interface_key_all[all_index].interface_source);
                break;
        }
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

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_eviction_set(
    int unit,
    uint32 flags,
    bcm_stat_engine_t * engine,
    bcm_stat_eviction_t * eviction)
{
    uint32 seq_timer_interval, sequential_timer_value;
    dnxcmn_time_t time;
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

    SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.
                    sequential_timer_value.get(unit, engine->core_id, engine->engine_id, &sequential_timer_value));
    /** seq_timer_interval calculation. result is in clocks resolution */
    COMPILER_64_SET(time.time, 0, sequential_timer_value);
    time.time_units = DNXCMN_TIME_UNIT_USEC;
    SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &time, &seq_timer_interval));

    /** set hw parameters - beside eviction bundaries which is set from interface API, once all parameters are determined */
    SHR_IF_ERR_EXIT(dnx_crps_eviction_engine_hw_set(unit, seq_timer_interval, engine, eviction));

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
 */
    *object_stat_id = (counter_id - sampling_offset - type_offset - ctr_set_offset + first_index) / set_size;

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
    int object_stat_id,
    int ctr_set_offset,
    int *found,
    uint32 *local_counter)
{
    uint32 logical_object_id;
    uint32 cfg_base_obj_id = 0;
    int set_size, database_id;
    uint32 type_offset;
    int sampling_offset = 0;
    uint32 first_index, last_index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.counter_set_size.get(unit, core_id, engine_id, &set_size));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.database_id.get(unit, core_id, engine_id, &database_id));
    SHR_IF_ERR_EXIT(dnx_crps_db.database.type.offset.get(unit, core_id, database_id, type_id, &type_offset));
    type_offset = type_offset * set_size;

    /** calc logical object id */
    logical_object_id = (object_stat_id - cfg_base_obj_id) * set_size + ctr_set_offset + type_offset + sampling_offset;

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
                   object_stat_id, core_id, engine_id, logical_object_id, first_index, last_index));
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
    int nof_counters_per_entry;
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
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_nof_counters_per_entry_get(unit, format, &nof_counters_per_entry));
        /** extruct the physical address (counter_id)*/
        *counter_id = local_counter / nof_counters_per_entry;

        /** if wide mode (one counter per entry), we read from ctr1 (sub_count=0). */
        /** if double entry we read even counters from ctr1, odd from ctr2*/
        /** if FormatPacketsAndBytes - the packets read from ctr1, bytes from ctr2*/
        is_bytes = _SHR_STAT_COUNTER_STAT_IS_BYTES_GET(stat);
        if ((format == bcmStatCounterFormatPacketsAndBytes) && (is_bytes == TRUE))
        {
            *sub_count = 1;
        }
        else if ((nof_counters_per_entry == 2) && (local_counter % 2 == 1))
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
            if ((type_valid == TRUE))
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
    int bg_thread_enable;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_crps_db.bg_thread_enable.get(unit, &bg_thread_enable));
    /** only if need to change the state - do it */
    if (enable != bg_thread_enable)
    {
        if (enable == TRUE)
        {
            SHR_IF_ERR_EXIT(dnx_crps_eviction_bg_thread_attach(unit));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_crps_eviction_bg_thread_detach(unit));
        }
        SHR_IF_ERR_EXIT(dnx_crps_db.bg_thread_enable.set(unit, enable));
    }

exit:
    SHR_FUNC_EXIT;

}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_background_collection_get(
    int unit,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_crps_db.bg_thread_enable.get(unit, enable));

exit:
    SHR_FUNC_EXIT;

}

/** see .h file */
shr_error_e
dnx_crps_mgmt_next_engine_set(
    int unit,
    bcm_stat_engine_t * engine,
    uint32 next_engine_id)
{
    int engine_id;
    SHR_FUNC_INIT_VARS(unit);

    if (next_engine_id == 0xFFFFFFFF)
    {
        engine_id = -1;
    }
    else
    {
        engine_id = (int) next_engine_id;
    }
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.next_engine_id.set(unit, engine->core_id, engine->engine_id, engine_id));

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_next_engine_get(
    int unit,
    bcm_stat_engine_t * engine,
    uint32 *next_engine_id)
{
    int engine_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.next_engine_id.get
                    (unit, engine->core_id, engine->engine_id, &engine_id));
    if (engine_id == -1)
    {
        *next_engine_id = 0xFFFFFFFF;
    }
    else
    {
        *next_engine_id = (uint32) engine_id;
    }
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
            if ((section == dnx_crps_mgmt_shmem_section_low) || (section == dnx_crps_mgmt_shmem_section_high))
            {
                get_section = (section == dnx_crps_mgmt_shmem_section_low) ?
                    dnx_crps_mgmt_shmem_section_high : dnx_crps_mgmt_shmem_section_low;
                SHR_IF_ERR_EXIT(dnx_crps_engine_bank_src_hw_get(unit, core_id, engine_id, get_section, &type));
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.used_for_meter.set(unit, core_idx, engine_id,
                                                                    (type !=
                                                                     DBAL_ENUM_FVAL_METER_CRPS_SMEM_SRC_TYPES_CRPS)));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.used_for_meter.set(unit, core_idx, engine_id, FALSE));
            }

            /** set CRPS type according to dbal enum in order to update hw */
            type = DBAL_ENUM_FVAL_METER_CRPS_SMEM_SRC_TYPES_CRPS;
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
            /** set the type according to dbal enum in order to update hw */
            type = is_ingress ? DBAL_ENUM_FVAL_METER_CRPS_SMEM_SRC_TYPES_METER_INGRESS :
                DBAL_ENUM_FVAL_METER_CRPS_SMEM_SRC_TYPES_METER_EGRESS;
        }
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
    /** free from resource manager the database */
    SHR_IF_ERR_EXIT(dnx_crps_db.database_res.free_single(unit, database->core_id, database->database_id));
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
    (*bank_id) =
        ((engine_id - dnx_data_meter.mem_mgmt.base_big_engine_for_meter_get(unit)) * multi_val) + section_offset;

/** exit: */
    SHR_FUNC_EXIT;
}
