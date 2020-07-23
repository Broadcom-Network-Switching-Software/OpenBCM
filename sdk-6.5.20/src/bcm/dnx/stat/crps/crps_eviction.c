/** \file crps_eviction.c
 * 
 *  crps module gather all functionality that related to the counter/statisitc processor driver for DNX.
 *  crps_eviction.c responsible of the eviction mechanism. copy the HW counters into a software image.
 *  It is internal crps file .   (do not have interfaces with other modules)
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
#include <soc/util.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/shrextend/shrextend_error.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/stat.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include "crps_mgmt_internal.h"
#include "crps_eviction.h"
#include <bcm_int/dnx/stat/crps/crps_mgmt.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_crps.h>
#include <soc/dnx/dnx_er_threading.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/crps_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/crps_eviction_access.h>
#include <include/shared/utilex/utilex_integer_arithmetic.h>
#include <include/shared/utilex/utilex_str.h>
#include <include/shared/utilex/utilex_bitstream.h>
#include <include/bcm_int/dnx/cmn/dnxcmn.h>
#include <include/bcm_int/common/debug.h>
#include <include/bcm_int/control.h>
#include <include/bcm_int/dnx/algo/crps/crps_ctrs_db.h>
#include <include/soc/dnx/dnx_fifodma.h>
#include <soc/dnxc/multithread_analyzer.h>
#include <soc/dnxc/swstate/dnxc_sw_state_verifications.h>
#include <soc/dnxc/dnxc_ha.h>

/*
 * DEFINES
 * {
 */

/**
* \brief
* defines the time interval in msec for the algorithm timer
*/
#define CRPS_ENGINE_ALG_TIMER_INTERVAL_MSEC (10)

#define DNX_CRPS_FIFO_ID_ALL (-1)

#define DNX_CRPS_CNT_CACHE_LENGTH (32*1024)

#define DNX_CRPS_UINT32_NOF_BITS (32)

#define CRPS_EVICTION_FIFO_DMA_MAX_ENTRIES (16384)
#define CRPS_EVICTION_FIFO_DMA_TIMOUT (1000)
#define CRPS_EVICTION_FIFO_DMA_THRESHOLD (1)

#define DNX_CRPS_INVALID_TYPE_ID (-1)

/*
 * }
 */

/*
 * }
 */

/*
 * strucutres
 * {
 */

/*
 * }
 */

/*
 * globals
 * {
 */

/*
 * }
 */

/*
 * MACROs
 * {
 */

/**
 * \brief - iterate over the relevant fifo_ids
 * if fifo_id is DNX_CRPS_FIFO_ID_ALL will iterate over all supported fifos.
 * Otherwise the iteration will be over the given fifo.
 */
#define DNX_CRPS_FIFOS_ITER(unit, fifo_id, fifo_index) \
    for(fifo_index = ((fifo_id == DNX_CRPS_FIFO_ID_ALL) ? 0 : fifo_id);\
        fifo_index < ((fifo_id == DNX_CRPS_FIFO_ID_ALL) ?  dnx_data_crps.eviction.nof_counters_fifo_get(unit) : \
                                                                                                    (fifo_id + 1));\
        fifo_index++)

/**
 * \brief set of macros to define the bg thread operation
 */
#define DNX_CRPS_THREAD_WAIT_TIMEOUT    (2000000) /** wait two seconds */
#define DNX_CRPS_FIFO_DEPTH_CRITICAL(unit)   (dnx_data_crps.eviction.counters_fifo_depth_get(unit)*3/4)
#define DNX_CRPS_FIFO_DEPTH_PREF  (500)
#define DNX_CRPS_BACKGROUND_THREAD_ITER_MIN      (0)
#define DNX_CRPS_BACKGROUND_THREAD_ITER_MAX     (1000000) /** 1sec */
#define DNX_CRPS_BACKGROUND_THREAD_ITER_DEFAULT (1000) /** 1 msec */
#define DNX_CRPS_BACKGROUND_THREAD_ITER_ADJUST(_bg_wait) ((_bg_wait + 10) / 10  ) /** adjust 10% */

/**
 * \brief - macro to check if SEQUENTIAL scanning mode is on/off
 */
#define DNX_CRPS_EVICTION_SEQUENTIAL_SCAN_MODE_IS_ON(_scan_mode) (_scan_mode & DBAL_ENUM_FVAL_CRPS_ENGINE_SCAN_MODE_SEQUENTIAL_SCAN)
/*
 * }
 */

/**
 * \brief - take lock of the counters dma fifos
 */
shr_error_e
dnx_crps_fifo_state_lock_take(
    int unit)
{
    uint8 is_created;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_crps_eviction_db.fifo_lock.is_created(unit, &is_created));
    if (is_created == TRUE)
    {
        /** Need to add here condition is created once SW state will support it */
        SHR_IF_ERR_EXIT(dnx_crps_eviction_db.fifo_lock.take(unit, sal_mutex_FOREVER));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - give lock of the counters dma fifos
 */
shr_error_e
dnx_crps_fifo_state_lock_give(
    int unit)
{
    uint8 is_created;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_crps_eviction_db.fifo_lock.is_created(unit, &is_created));
    if (is_created == TRUE)
    {
        /** Need to add here condition is created once SW state will support it */
        SHR_IF_ERR_EXIT(dnx_crps_eviction_db.fifo_lock.give(unit));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - take lock of the cache counters
 */
shr_error_e
dnx_crps_counter_state_lock_take(
    int unit)
{
    uint8 is_created;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_crps_eviction_db.cache_lock.is_created(unit, &is_created));
    if (is_created == TRUE)
    {
        /** Need to add here condition is created once SW state will support it */
        SHR_IF_ERR_EXIT(dnx_crps_eviction_db.cache_lock.take(unit, sal_mutex_FOREVER));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - give lock of the cache counters
 */
shr_error_e
dnx_crps_counter_state_lock_give(
    int unit)
{
    uint8 is_created;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_crps_eviction_db.cache_lock.is_created(unit, &is_created));
    if (is_created == TRUE)
    {
        /** Need to add here condition is created once SW state will support it */
        SHR_IF_ERR_EXIT(dnx_crps_eviction_db.cache_lock.give(unit));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   function convert the API qualifier type to DBAL enum
* \par DIRECT INPUT:
*   \param [in] unit         -
*   \param [in] qualifer  -  api qualifer type
*   \param [out] dbal_qualifier  -  dbal qualifer type
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
dnx_crps_eviction_qualifier_dbal_convert(
    int unit,
    bcm_stat_eviction_conditional_qual_t qualifer,
    dbal_enum_value_field_crps_cond_eviction_qual_mode_e * dbal_qualifier)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (qualifer)
    {
        case bcmStatEvictionConditionalQualAndEqualZero:
            *dbal_qualifier = DBAL_ENUM_FVAL_CRPS_COND_EVICTION_QUAL_MODE_OP_AND_EQUAL_ZERO;
            break;
        case bcmStatEvictionConditionalQualAndNoneEqualZero:
            *dbal_qualifier = DBAL_ENUM_FVAL_CRPS_COND_EVICTION_QUAL_MODE_OP_AND_NONE_EQUAL_ZERO;
            break;
        case bcmStatEvictionConditionalQualAndEqualAllOnes:
            *dbal_qualifier = DBAL_ENUM_FVAL_CRPS_COND_EVICTION_QUAL_MODE_OP_AND_EQUAL_ALL_ONES;
            break;
        case bcmStatEvictionConditionalQualAndNoneEqualAllOnes:
            *dbal_qualifier = DBAL_ENUM_FVAL_CRPS_COND_EVICTION_QUAL_MODE_OP_AND_NONE_EQUAL_ALL_ONES;
            break;
        case bcmStatEvictionConditionalQualOrEqualZero:
            *dbal_qualifier = DBAL_ENUM_FVAL_CRPS_COND_EVICTION_QUAL_MODE_OP_OR_EQUAL_ZERO;
            break;
        case bcmStatEvictionConditionalQualOrNoneEqualZero:
            *dbal_qualifier = DBAL_ENUM_FVAL_CRPS_COND_EVICTION_QUAL_MODE_OP_OR_NONE_EQUAL_ZERO;
            break;
        case bcmStatEvictionConditionalQualOrEqualAllOnes:
            *dbal_qualifier = DBAL_ENUM_FVAL_CRPS_COND_EVICTION_QUAL_MODE_OP_OR_EQUAL_ALL_ONES;
            break;
        case bcmStatEvictionConditionalQualOrNoneEqualAllOnes:
            *dbal_qualifier = DBAL_ENUM_FVAL_CRPS_COND_EVICTION_QUAL_MODE_OP_OR_NONE_EQUAL_ALL_ONES;
            break;
        case bcmStatEvictionConditionalQualDataBiggerThanSource:
            *dbal_qualifier = DBAL_ENUM_FVAL_CRPS_COND_EVICTION_QUAL_MODE_OP_DATA_BIGGER_THAN_SOURCE;
            break;
        case bcmStatEvictionConditionalQualSourceBiggerThanData:
            *dbal_qualifier = DBAL_ENUM_FVAL_CRPS_COND_EVICTION_QUAL_MODE_OP_SOURCE_BIGGER_THAN_DATA;
            break;
        case bcmStatEvictionConditionalQualSourceEqualToData:
            *dbal_qualifier = DBAL_ENUM_FVAL_CRPS_COND_EVICTION_QUAL_MODE_OP_SOURCE_EQUAL_TO_DATA;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "invalid qualifer (value=%d) \n", qualifer);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   function set the conditional eviction configuration in HW
* \par DIRECT INPUT:
*   \param [in] unit -  unit id
*   \param [in] engine  -  structure that holds the engine id and core id
*   \param [in] cond_eviction  -  conditional eviction configuration
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   1. In j2c and q2a, the HW compare the source and data after it was update the counter value.
*      In j2p and above, it compare before updating the counter value,
*   2. The conditional trigger was defined by Arch to compare only the 34 LSBs of the counter.
*       a. 34-34 mode - comparing the entire 34 bits counter
*       b. 68 mode - compare the 34 LSBs from the 68 bits counter
*       c. 30-38 mode - compare the 4 LSBs of bytes counter + the entire 30 bits of the packets counter
*       d. 6-14 mode - compare the entire total 20 bits of the counter, padded with leading zeroes.
*       The 34 bits above are compared either to the register value,
*       or to the 34 LSBs of the data that came with the command to the engine.
* \see
*   * None
*/
static shr_error_e
dnx_crps_eviction_conditional_engine_hw_set(
    int unit,
    bcm_stat_engine_t * engine,
    bcm_stat_eviction_conditional_t * cond_eviction)
{
    uint32 entry_handle_id;
    int enable;
    dbal_enum_value_field_crps_cond_eviction_qual_mode_e qual;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (dnx_data_crps.eviction.feature_get(unit, dnx_data_crps_eviction_conditional_action_is_supported) == 1)
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_CONDITIONAL_EVICTION_CFG, &entry_handle_id));
        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_ID, engine->engine_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, engine->core_id);
        /** Setting value fields */
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.conditional.enable.get
                        (unit, engine->core_id, engine->engine_id, &enable));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CFG_DATA_ENABLE, INST_SINGLE,
                                     (cond_eviction->condition_source_select ==
                                      bcmStatEvictionConditionalSourceCommandData));
        dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_CFG_DATA, INST_SINGLE,
                                     cond_eviction->condition_user_data);
        SHR_IF_ERR_EXIT(dnx_crps_eviction_qualifier_dbal_convert(unit, cond_eviction->qualifier, &qual));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COND_QUALIFIER, INST_SINGLE, qual);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTION_EVICT_COUNTER, INST_SINGLE,
                                     _SHR_IS_FLAG_SET(cond_eviction->action_flags,
                                                      BCM_STAT_EVICTION_CONDITIONAL_ACTION_EVICT_COUNTER));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTION_RESET_COUNTER, INST_SINGLE,
                                     _SHR_IS_FLAG_SET(cond_eviction->action_flags,
                                                      BCM_STAT_EVICTION_CONDITIONAL_ACTION_RESET_COUNTER));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTION_EVICT_TIME, INST_SINGLE,
                                     _SHR_IS_FLAG_SET(cond_eviction->action_flags,
                                                      BCM_STAT_EVICTION_CONDITIONAL_ACTION_EVICT_TIME));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTION_INTERRUPT, INST_SINGLE,
                                     _SHR_IS_FLAG_SET(cond_eviction->action_flags,
                                                      BCM_STAT_EVICTION_CONDITIONAL_ACTION_INTERRUPT));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTION_NO_EVICTION, INST_SINGLE,
                                     _SHR_IS_FLAG_SET(cond_eviction->action_flags,
                                                      BCM_STAT_EVICTION_CONDITIONAL_ACTION_NO_EVICTION));

        /** Preforming the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_eviction_engine_hw_set(
    int unit,
    uint32 seq_timer_interval,
    bcm_stat_engine_t * engine,
    bcm_stat_eviction_t * eviction)
{
    uint32 alg_timer_interval = 0;
    uint32 entry_handle_id;
    dnxcmn_time_t time;
    dbal_enum_value_field_crps_engine_scan_mode_e scan_mode;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_ENGINE_SCAN_CFG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_ID, engine->engine_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, engine->core_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_ENABLE_CLEAR_ON_READ_RESET_VALUE,
                                 INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_ENABLE_CLEAR_ON_READ, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_DMA_SELECT, INST_SINGLE,
                                 eviction->dma_fifo_select);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_WAIT_TIME_BETWEEN_SEQ_SCANS,
                                 INST_SINGLE, seq_timer_interval);
    if ((eviction->eviction_algorithmic_disable == TRUE) ||
        (FALSE == dnx_data_crps.eviction.feature_get(unit, dnx_data_crps_eviction_algorithmic_is_supported)))
    {
        scan_mode = DBAL_ENUM_FVAL_CRPS_ENGINE_SCAN_MODE_SEQUENTIAL_SCAN;
    }
    else
    {
        scan_mode = DBAL_ENUM_FVAL_CRPS_ENGINE_SCAN_MODE_SEQUENTIAL_AND_ALGORITHIM_SCAN;
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_MODE, INST_SINGLE, scan_mode);
    if (TRUE == dnx_data_crps.eviction.feature_get(unit, dnx_data_crps_eviction_algorithmic_is_supported))
    {
         /** alg_timer_interval calculation. resault is in clocks resolution */
        COMPILER_64_ZERO(time.time);
        COMPILER_64_ADD_32(time.time, CRPS_ENGINE_ALG_TIMER_INTERVAL_MSEC);
        time.time_units = DNXCMN_TIME_UNIT_MILISEC;
        SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &time, &alg_timer_interval));
        /** add one more to alg_timer_interval in order that the alg and seq timers will not have the same periodicity intervals.
                (they should not be multiple times to each other). otherwise, algo timer will always get priority over seq */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_WAIT_TIME_BETWEEN_ALG_SCANS,
                                     INST_SINGLE, (alg_timer_interval + 1));
         /** set the algorithim threshold. field refer to 8 msb bits of the counter.  */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_THRESHOLD, INST_SINGLE, 0x10);
        /** set the algorithim threshold. field refer to 4 msb bits of the packet counter.  relevant only in case of packet and bytes format */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_PKT_THRESHOLD, INST_SINGLE,
                                     0x1);
    }
    /** enable probabilistic scan, if the feature exist and if user didn't disable it */
    /** Note: eviction_algorithmic_disable refer to algorithmic or probabilistic eviction - depend on the device */
    if (TRUE == dnx_data_crps.eviction.feature_get(unit, dnx_data_crps_eviction_probabilistic_is_supported))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_PROBABILISTIC, INST_SINGLE,
                                     (eviction->eviction_algorithmic_disable == TRUE ? FALSE : TRUE));
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_OBJECT_TYPE_FOR_DMA, INST_SINGLE,
                                 eviction->eviction_event_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_DMA_RECORD_LOGICAL_SEL, INST_SINGLE,
                                 (eviction->record_format == bcmStatEvictionRecordFormatLogical));

    /** scan bubble parameters */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_BUBBLE_REQUEST_ENABLE, INST_SINGLE,
                                 TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_BUBBLE_TIMEOUT_BEFORE_REQUEST,
                                 INST_SINGLE, dnx_data_crps.eviction.bubble_before_request_timeout_get(unit));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_BUBBLE_TIMEOUT_AFTER_REQUEST,
                                 INST_SINGLE, dnx_data_crps.eviction.bubble_after_request_timeout_get(unit));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_FORCE_BUBBLE, INST_SINGLE, FALSE);

    /** Note: eviction bundaries is set in different function once all the relevant paramenters are known */
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dnx_crps_eviction_conditional_engine_hw_set(unit, engine, &eviction->cond));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_crps_eviction_counter_result_update(
    int unit,
    uint32 *curr_entry)
{
    uint32 err_ind;
    uint32 opcode;
    uint32 ctr_id, engine_id, core_id;
    const dnx_data_crps_eviction_phy_record_format_t *rcd_format = dnx_data_crps.eviction.phy_record_format_get(unit);
    const dnx_data_crps_engine_engines_info_t *engine_info;
    bcm_stat_counter_format_type_t format;
    /** array of uint32 that holds the counter value (68 bits for Jr2) */
    uint32 ctr[(DNX_DATA_MAX_CRPS_EVICTION_DMA_RECORD_ENTRY_NOF_BITS / DNX_CRPS_UINT32_NOF_BITS + 1)] = { 0 };
    int ctr_index;
    int ctr_size_first_section;
    uint32 temp32_arr[2] = { 0 };
    uint64 temp64;
    int nof_counters_per_entry;
    int is_zero = TRUE;
    uint32 nof_counters;
    SHR_FUNC_INIT_VARS(unit);

    /** extract the opcode */
    SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                    (curr_entry, rcd_format->opcode_offset, rcd_format->opcode_size, &opcode));

    /** only if the opcode is 0, which means Physical format */
    if (opcode == 0)
    {
        /** extract the error indication */
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                        (curr_entry, rcd_format->err_ind_offset, rcd_format->err_ind_size, &err_ind));
        if (err_ind == FALSE)
        {
            /** extract the ctr value into array of 3 ctrs. */
            for (ctr_index = 0; ctr_index < (rcd_format->ctr_value_size / DNX_CRPS_UINT32_NOF_BITS + 1); ctr_index++)
            {
                SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                                (curr_entry, (rcd_format->ctr_value_offset + DNX_CRPS_UINT32_NOF_BITS * ctr_index),
                                 ((ctr_index < (rcd_format->ctr_value_size / DNX_CRPS_UINT32_NOF_BITS)) ?
                                  DNX_CRPS_UINT32_NOF_BITS : (rcd_format->ctr_value_size -
                                                              DNX_CRPS_UINT32_NOF_BITS * ctr_index)), &ctr[ctr_index]));
                if (ctr[ctr_index] != 0)
                {
                    is_zero = FALSE;
                }
            }
            /** if counter is zero, do not update anything */
            /** Notes: */
            /** 1. this optimization is not feet to average format. Need to handle it once it will be implemented */
            /** 2. Once condional eviction will be implemented in the HW, we can remove it, */
            /**    because zero counters will not be evicted anyway */
            if (is_zero == FALSE)
            {
                SHR_IF_ERR_EXIT(dnx_crps_eviction_db.fifo_results.nof_counters.get(unit, &nof_counters));
                /** extract the core_id */
                SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                                (curr_entry, rcd_format->core_id_offset, rcd_format->core_id_size, &core_id));
                if (core_id >= dnx_data_device.general.nof_cores_get(unit))
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "core_id=%d. total_nof_cores= %d\n",
                                 core_id, dnx_data_device.general.nof_cores_get(unit));
                }
                SHR_IF_ERR_EXIT(dnx_crps_eviction_db.fifo_results.cnt_result.core_id.set(unit, nof_counters, core_id));

                /** extract the engine_id */
                SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                                (curr_entry, rcd_format->engine_id_offset, rcd_format->engine_id_size, &engine_id));
                if (engine_id > dnx_data_crps.engine.nof_engines_get(unit))
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "engine_id=%d. device total_nof_engine= %d\n",
                                 engine_id, dnx_data_crps.engine.nof_engines_get(unit));
                }
                SHR_IF_ERR_EXIT(dnx_crps_eviction_db.fifo_results.cnt_result.engine_id.set
                                (unit, nof_counters, engine_id));

                /** extract the counter_id */
                SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                                (curr_entry, rcd_format->local_ctr_id_offset, rcd_format->local_ctr_id_size, &ctr_id));
                engine_info = dnx_data_crps.engine.engines_info_get(unit, engine_id);

                /** get how many counters in each one counter entry: 2 counters for pkts_and_pkts and max formats, */
                /** else 1 counter */
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.format.get(unit, core_id, engine_id, &format));
                SHR_IF_ERR_EXIT(dnx_crps_eviction_db.fifo_results.cnt_result.format.set(unit, nof_counters, format));
                SHR_IF_ERR_EXIT(dnx_crps_mgmt_nof_counters_per_entry_get(unit, format, &nof_counters_per_entry));

                if (ctr_id >= engine_info->nof_counters * nof_counters_per_entry)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "counter_id=%d. bigger than nof_counters(%d) for engine=%d\n",
                                 ctr_id, engine_info->nof_counters * nof_counters_per_entry, engine_id);
                }
                SHR_IF_ERR_EXIT(dnx_crps_eviction_db.fifo_results.cnt_result.counter_entry.set
                                (unit, nof_counters, (ctr_id / nof_counters_per_entry)));

                /** init counters */
                COMPILER_64_ZERO(temp64);
                SHR_IF_ERR_EXIT(dnx_crps_eviction_db.fifo_results.cnt_result.cnt1.set(unit, nof_counters, temp64));
                SHR_IF_ERR_EXIT(dnx_crps_eviction_db.fifo_results.cnt_result.cnt2.set(unit, nof_counters, temp64));

                /** according to the format, decide how to split the ctr value into ctr1 and ctr2 */

                /** according to the format, update the relevant counter */
                /** if wide mode (one counter per entry), we write to ctr1. */
                /** if double entry we write just one counter - LSB bits. even counter to ctr1, or odd to ctr2 */
                /** if FormatPacketsAndBytes - the packets is written to ctr1, bytes to ctr2*/
                switch (format)
                {
                    case bcmStatCounterFormatPackets:
                    case bcmStatCounterFormatBytes:
                        /** wide format ==> it is not possible that the counter will be >= 2^64 */
                        if (ctr[rcd_format->ctr_value_size / DNX_CRPS_UINT32_NOF_BITS] != 0)
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                         "counter_value msb bits=%d. must be zero for WIDE format \n",
                                         ctr[rcd_format->ctr_value_size / DNX_CRPS_UINT32_NOF_BITS]);
                        }
                        COMPILER_64_SET(temp64, ctr[1], ctr[0]);
                        SHR_IF_ERR_EXIT(dnx_crps_eviction_db.fifo_results.cnt_result.cnt1.set
                                        (unit, nof_counters, temp64));
                        break;
                    case bcmStatCounterFormatPacketsAndBytes:
                        ctr_size_first_section = dnx_data_crps.eviction.packet_and_bytes_packet_size_get(unit);
                        /** extract the first counter */
                        SHR_BITCOPY_RANGE(temp32_arr, 0, ctr, 0, ctr_size_first_section);
                        COMPILER_64_SET(temp64, temp32_arr[1], temp32_arr[0]);
                        SHR_IF_ERR_EXIT(dnx_crps_eviction_db.fifo_results.cnt_result.cnt1.set
                                        (unit, nof_counters, temp64));
                        /** init the temp counter array */
                        temp32_arr[0] = 0;
                        temp32_arr[1] = 0;
                        COMPILER_64_ZERO(temp64);

                        /** extract the second counter */
                        SHR_BITCOPY_RANGE(temp32_arr, 0, ctr, ctr_size_first_section,
                                          (rcd_format->ctr_value_size - ctr_size_first_section));
                        COMPILER_64_ZERO(temp64);
                        COMPILER_64_SET(temp64, temp32_arr[1], temp32_arr[0]);
                        SHR_IF_ERR_EXIT(dnx_crps_eviction_db.fifo_results.cnt_result.cnt2.set
                                        (unit, nof_counters, temp64));
                        break;
                    case bcmStatCounterFormatDoublePackets:
                    case bcmStatCounterFormatMaxSize:
                        /** one counter size from total counter_value size (68 bits for Jr2) in the record. */
                        ctr_size_first_section = rcd_format->ctr_value_size / nof_counters_per_entry;

                        /** extract the LSB counter to the right SW counter - even to ctr1, odd to ctr2  */
                        SHR_BITCOPY_RANGE(temp32_arr, 0, ctr, 0, ctr_size_first_section);
                        if (ctr_id % 2 == 0)
                        {
                            COMPILER_64_SET(temp64, temp32_arr[1], temp32_arr[0]);
                            SHR_IF_ERR_EXIT(dnx_crps_eviction_db.fifo_results.cnt_result.cnt1.set
                                            (unit, nof_counters, temp64));
                        }
                        else
                        {
                            COMPILER_64_SET(temp64, temp32_arr[1], temp32_arr[0]);
                            SHR_IF_ERR_EXIT(dnx_crps_eviction_db.fifo_results.cnt_result.cnt2.set
                                            (unit, nof_counters, temp64));
                        }

                        break;
                    default:
                        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid format type (value=%d) \n", format);
                        break;
                }

                /** increment the array index */
                nof_counters++;
                SHR_IF_ERR_EXIT(dnx_crps_eviction_db.fifo_results.nof_counters.set(unit, nof_counters));
            }
        }
        else
        {
            LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit, "%s unit %d err_ind=%d \n"), "record_update", unit, err_ind));
        }
    }
    else
    {
        LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit, "%s unit %d opcode=%d \n"), "record_update", unit, opcode));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  run over all valid entries in the fifo, read the records and fill the
* \par DIRECT INPUT:
*   \param [in] unit    -  unit id
*   \param [in] core_id -  core_id. (0 or 1 or all)
*   \param [in] fifo_id -  fifo_id. (0 or 1 or DNX_CRPS_FIFO_ID_ALL)
*   \param [out] counters_to_read -  nof entries in the fifo
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -
* \par INDIRECT OUTPUT:
* \remark
*     This is called by both the background task and the demand access
*     functions, so it does not itself handle the locking protocol.
*
*     Because the calls to "read buffer" from the background process
*     and API caller may collide, or may collide with the direct read, we
*     don't do any background updates while performing updates in preparation
*     for foreground access.
* \see
*   * None
*/
static shr_error_e
dnx_crps_eviction_counter_fifo_read(
    int unit,
    int core_id,
    int fifo_id,
    int *counters_to_read)
{
    uint32 entry_words = 0, entry_bytes;
    int nof_read_cnt = 0;
    uint8 *host_buff;
    uint32 buff_index;
    uint32 *curr_entry = NULL;
    int fifo_channel = DNX_CRPS_INVALID_DMA_CHANNEL;
    SHR_FUNC_INIT_VARS(unit);

    /** init nof_ counters of the result array */
    SHR_IF_ERR_EXIT(dnx_crps_eviction_db.fifo_results.nof_counters.set(unit, 0));
    SHR_IF_ERR_EXIT(dnx_crps_eviction_db.fifo_channels.get(unit, core_id, fifo_id, &fifo_channel));

    if (fifo_channel == DNX_CRPS_INVALID_DMA_CHANNEL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "channel is invalid for core_id=%d, fifo_id=%d \n", core_id, fifo_id);
    }

    (*counters_to_read) = 0;

    SHR_IF_ERR_EXIT(soc_dnx_fifodma_num_entries_get(unit, fifo_channel, counters_to_read, &host_buff, &entry_bytes));
    entry_words = BYTES2WORDS(entry_bytes);

    for (buff_index = 0; buff_index < (*counters_to_read); buff_index++)
    {
        curr_entry = (uint32 *) host_buff + (buff_index * entry_words);
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U
                   (unit,
                    "ctr read: fifo=%d, counters_2read=%d, curr_line=%x val=%.8x %.8x %.8x %.8x\n"),
                   fifo_id, (*counters_to_read), buff_index, (curr_entry[0]),
                   curr_entry[(1 % entry_words)], curr_entry[(2 % entry_words)], curr_entry[(3 % entry_words)]));
        /** copy the counter to the results array */
        nof_read_cnt++;
        SHR_IF_ERR_EXIT(dnx_crps_eviction_counter_result_update(unit, curr_entry));
    }

exit:
    if (fifo_channel != DNX_CRPS_INVALID_DMA_CHANNEL)
    {
        SHR_IF_ERR_CONT(soc_dnx_fifodma_set_entries_read(unit, fifo_channel, nof_read_cnt));
    }
    SHR_FUNC_EXIT;
}

/**
* \brief
*   process the counters fifos. each call, empty the entire fifo and update the sw counters memory.
* \par DIRECT INPUT:
*   \param [in] unit    -  unit id
*   \param [in] core_id -  core_id. (0 or 1 or all)
*   \param [in] fifo_id -  fifo_id. (0 or 1 or DNX_CRPS_FIFO_ID_ALL)
*   \param [in] depth -  max number of valid entries read from each FIFO
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -
* \par INDIRECT OUTPUT:
*   * None
* \remark
*     Need to take mutex "fifo_lock" before calling this function !!!
*     This is called by both the background task and the demand access
*     functions, so it does not itself handle the locking protocol.
*
*     Because the calls to "read buffer" from the background process
*     and API caller may collide, or may collide with the direct read, we
*     don't do any background updates while performing updates in preparation
*     for foreground access.
* \see
*   * None
*/
static shr_error_e
dnx_crps_eviction_counter_fifo_process(
    int unit,
    bcm_core_t core_id,
    int fifo_id,
    int *depth)
{
    int core_index, fifo_index, ctr_idx;
    int nof_counters = 0, flags = 0;
    int counter_lock_taken = FALSE;
    int valid;
    uint32 nof_counter_entries;
    uint32 entry_engine_id;
    uint32 entry_core_id;
    uint32 counter_entry;
    bcm_stat_counter_format_type_t entry_format;
    uint64 entry_cnt1, entry_cnt2;
    int fifo_channel;
    SHR_FUNC_INIT_VARS(unit);

    (*depth) = 0;

    /** * run over all valid fifos and pull counters records from them */
    DNXCMN_CORES_ITER(unit, core_id, core_index)
    {
        DNX_CRPS_FIFOS_ITER(unit, fifo_id, fifo_index)
        {
            SHR_IF_ERR_EXIT(dnx_crps_eviction_db.fifo_channels.get(unit, core_index, fifo_index, &fifo_channel));
            if (fifo_channel != DNX_CRPS_INVALID_DMA_CHANNEL)
            {
                /** read the data records from the current sw fifo */
                SHR_IF_ERR_CONT(dnx_crps_eviction_counter_fifo_read(unit, core_index, fifo_index, &nof_counters));

                /** update the max nof_counters read from each FIFO */
                if (nof_counters > (*depth))
                {
                    (*depth) = nof_counters;
                }

                /** update the sw counters memory */
                /** while there are counters to update, update them */
                /** update anyway, ctr1 and ctr2 even if one of them not relevant. It is expected that it was init to zero */
                SHR_IF_ERR_EXIT(dnx_crps_counter_state_lock_take(unit));
                counter_lock_taken = TRUE;
                SHR_IF_ERR_EXIT(dnx_crps_eviction_db.fifo_results.nof_counters.get(unit, &nof_counter_entries));
                for (ctr_idx = 0; ctr_idx < nof_counter_entries; ctr_idx++)
                {
                    /** if the engine is not valid (meaning, not full active),  we do not want to update the counters */
                    /*
                     * scenario example: step #1: engine active => counters are being updates. step #2: reset engine => memset
                     * counters memory, but there are still counters in the fifo that are evicted. Don't update them!!!
                     */
                    SHR_IF_ERR_EXIT(dnx_crps_eviction_db.fifo_results.cnt_result.
                                    core_id.get(unit, ctr_idx, &entry_core_id));
                    SHR_IF_ERR_EXIT(dnx_crps_eviction_db.fifo_results.cnt_result.
                                    engine_id.get(unit, ctr_idx, &entry_engine_id));
                    DNX_CRPS_MGMT_ENGINE_VALID_GET(unit, entry_core_id, entry_engine_id, valid);
                    if (valid)
                    {
                        SHR_IF_ERR_EXIT(dnx_crps_eviction_db.fifo_results.cnt_result.counter_entry.get
                                        (unit, ctr_idx, &counter_entry));
                        SHR_IF_ERR_EXIT(dnx_crps_eviction_db.fifo_results.cnt_result.format.get
                                        (unit, ctr_idx, &entry_format));
                        SHR_IF_ERR_EXIT(dnx_crps_eviction_db.fifo_results.cnt_result.cnt1.get
                                        (unit, ctr_idx, &entry_cnt1));
                        SHR_IF_ERR_EXIT(dnx_crps_eviction_db.fifo_results.cnt_result.cnt2.get
                                        (unit, ctr_idx, &entry_cnt2));
                        /** update ctr1 */
                        SHR_IF_ERR_CONT(dnx_crps_ctrs_db_counter_set
                                        (unit, flags, entry_core_id,
                                         entry_engine_id,
                                         counter_entry, DNX_CRPS_MGMT_SUB_CTR1, entry_format, entry_cnt1));
                        /** update ctr2 */
                        SHR_IF_ERR_CONT(dnx_crps_ctrs_db_counter_set
                                        (unit, flags, entry_core_id,
                                         entry_engine_id,
                                         counter_entry, DNX_CRPS_MGMT_SUB_CTR2, entry_format, entry_cnt2));
                    }
                }
                SHR_IF_ERR_EXIT(dnx_crps_counter_state_lock_give(unit));
                counter_lock_taken = FALSE;
            }
        }
    }

exit:
    if (counter_lock_taken)
    {
        SHR_IF_ERR_EXIT(dnx_crps_counter_state_lock_give(unit));
    }
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Background handler (one thread per unit)
* \par DIRECT INPUT:
*   \param [in] unit_ptr    -  unit id pointer
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -
* \par INDIRECT OUTPUT:
*   * None
* \remark
*     This sets running to TRUE to indicate it has started.
*
*     This thread will terminate itself and free the unit descriptor during
*     the next iteration if running is set to FALSE.  This allows
*     the destroy function to safely shut down the background thread by simply
*     detaching it from the globals and setting the running flag to FALSE.
* \see
*   * None
*/
void
dnx_crps_eviction_counter_bg(
    void *unit_ptr)
{
    int semTaken = FALSE;
    int depth = 0;
    int unit = PTR_TO_INT(unit_ptr);
    int rv;
    int running;
    int bgWait;

    /*
     *int start, end;
     */
    /*
     * unsigned int proc_mask = 0;
 */
    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_UTILS_EXCLUDED_THREAD_ADD(unit, DNX_ERR_RECOVERY_INTERNAL_THREAD_CRPS_EVIC);
    DNXC_MTA(dnxc_multithread_analyzer_declare_api_in_play(unit, __FUNCTION__, MTA_FLAG_THREAD_MAIN_FUNC, TRUE));

    /*
     * loop until told to go away
     */
    SHR_IF_ERR_CONT(dnx_crps_eviction_db.bg_thread_data.running.get(unit, &running));
    while (running)
    {

        /*
         *  This should wait for the specified interval and then give up,
         *  but we don't care whether it gets triggered or it gives up, so
         *  just discard the result deliberately.  Note that this mechanism
         *  can also be used to force an immediate trigger, such as when
         *  destroying a unit.
         */
        SHR_IF_ERR_CONT(dnx_crps_eviction_db.bg_thread_data.bgWait.get(unit, &bgWait));
        SHR_IF_ERR_CONT(dnx_crps_eviction_db.bg_thread_data.bgSem.take(unit, bgWait));
        semTaken = TRUE;

        /** background processing is not deferred or disabled */
        /** interlock using mutex instead of spin loops */
        SHR_IF_ERR_CONT(dnx_crps_fifo_state_lock_take(unit));

        /** time measurements for FIFO processing */
        /** start = sal_time_usecs(); */
        /** process all of the active FIFOs */
        rv = dnx_crps_eviction_counter_fifo_process(unit, _SHR_CORE_ALL, DNX_CRPS_FIFO_ID_ALL, &depth);
        /** end=sal_time_usecs();*/
        /**LOG_CLI((BSL_META("%d unit: BG thread - eviction FIFO processing %dus\n FIFO depth %d\n "), unit, SAL_USECS_SUB(end,start), depth));*/
        if (rv != _SHR_E_NONE)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "unit %d error processing counter fifo:  dnx_crps_eviction_counter_fifo_process \n"),
                       unit));
        }

        /*
         * Automatically adjust delay based upon how busy.  Tries to  reduce this thread's overhead on the system
         * while avoiding conditions that result in missed counter updates.
         */
        if (DNX_CRPS_FIFO_DEPTH_CRITICAL(unit) < depth)
        {
            SHR_IF_ERR_CONT(dnx_crps_eviction_db.bg_thread_data.bgWait.set(unit, DNX_CRPS_BACKGROUND_THREAD_ITER_MIN));
            LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit, "%s unit %d counter background thread delay to %dus\n"),
                                       "decreasing", unit, DNX_CRPS_BACKGROUND_THREAD_ITER_MIN));
        }
        else if (DNX_CRPS_FIFO_DEPTH_PREF < depth)
        {
            /** decrease the bgWait, but don't go down from the min value */
            if (bgWait >= (DNX_CRPS_BACKGROUND_THREAD_ITER_ADJUST(bgWait) + DNX_CRPS_BACKGROUND_THREAD_ITER_MIN))
            {
                bgWait -= DNX_CRPS_BACKGROUND_THREAD_ITER_ADJUST(bgWait);
            }
            else
            {
                bgWait = DNX_CRPS_BACKGROUND_THREAD_ITER_MIN;
            }
            SHR_IF_ERR_CONT(dnx_crps_eviction_db.bg_thread_data.bgWait.set(unit, bgWait));
            LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit, "%s unit %d counter background thread delay to %dus\n"),
                                       "decreasing", unit, DNX_CRPS_BACKGROUND_THREAD_ITER_MIN));
        }
        else if (DNX_CRPS_FIFO_DEPTH_PREF > depth)
        {
            /** increase the bgWait, but don't go up from the min value */
            bgWait += DNX_CRPS_BACKGROUND_THREAD_ITER_ADJUST(bgWait);
            if (bgWait > DNX_CRPS_BACKGROUND_THREAD_ITER_MAX)
            {
                bgWait = DNX_CRPS_BACKGROUND_THREAD_ITER_MAX;
            }
            SHR_IF_ERR_CONT(dnx_crps_eviction_db.bg_thread_data.bgWait.set(unit, bgWait));
            LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit, "%s unit %d counter background thread delay to %dus\n"),
                                       "increasing", unit, bgWait));
        }

        /*
         * allow foreground access again
         */
        SHR_IF_ERR_CONT(dnx_crps_fifo_state_lock_give(unit));
        SHR_IF_ERR_CONT(dnx_crps_eviction_db.bg_thread_data.running.get(unit, &running));
    }

    /** dispose of the unit information */
    if (semTaken)
    {
        /*
         *  If we have it, then release it.  If not, nobody should have it
         *  now since it was released by the detach code, which is also
         *  where running was set to FALSE.
         */
        SHR_IF_ERR_CONT(dnx_crps_eviction_db.bg_thread_data.bgSem.give(unit));
    }

/** exit: */
    DNXC_MTA(dnxc_multithread_analyzer_declare_api_in_play(unit, __FUNCTION__, MTA_FLAG_THREAD_MAIN_FUNC, FALSE));
    DNX_ERR_RECOVERY_UTILS_EXCLUDED_THREAD_REMOVE(unit, DNX_ERR_RECOVERY_INTERNAL_THREAD_CRPS_EVIC);
    SHR_VOID_NO_RETURN_FUNC_WO_EXIT;
    SHR_IF_ERR_CONT(dnx_crps_eviction_db.bg_thread_data.thread_is_still_running.set(unit, FALSE));
    sal_thread_exit(0);
}
/** see .h file */
shr_error_e
dnx_crps_eviction_bg_thread_attach(
    int unit)
{
    int running;
    sal_thread_t background;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_crps_eviction_db.bg_thread_data.running.get(unit, &running));
    if (running)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "unit %d bg thread already run\n", unit);
    }

    SHR_IF_ERR_EXIT(dnx_crps_eviction_db.bg_thread_data.bgSem.create(unit, sal_sem_BINARY, 1));

    SHR_IF_ERR_EXIT(dnx_crps_eviction_db.bg_thread_data.running.set(unit, TRUE));
    background = sal_thread_create("DnxCtrBg", SAL_THREAD_STKSZ, 50, dnx_crps_eviction_counter_bg, INT_TO_PTR(unit));
    if (SAL_THREAD_ERROR == background)
    {
        SHR_IF_ERR_EXIT(dnx_crps_eviction_db.bg_thread_data.bgSem.destroy(unit));
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "unit %d unable to create counter background thread\n", unit);
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_eviction_bg_thread_detach(
    int unit)
{
    soc_timeout_t to;
    int running;
    int thread_is_still_running = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_crps_eviction_db.bg_thread_data.running.get(unit, &running));

    if (running)
    {
        SHR_IF_ERR_EXIT(dnx_crps_eviction_db.bg_thread_data.thread_is_still_running.set(unit, TRUE));
        SHR_IF_ERR_EXIT(dnx_crps_eviction_db.bg_thread_data.running.set(unit, FALSE));

        SHR_IF_ERR_EXIT(dnx_crps_eviction_db.bg_thread_data.bgSem.give(unit));

        soc_timeout_init(&to, DNX_CRPS_THREAD_WAIT_TIMEOUT, 0);
        /** wait up to two seconds for bg thread to finish its current loop */
        while (thread_is_still_running == TRUE)
        {
            if (soc_timeout_check(&to))
            {
                SHR_IF_ERR_EXIT(dnx_crps_eviction_db.bg_thread_data.thread_is_still_running.set(unit, FALSE));
                /** we cannot release the bgsem because we are not sure in this case, what is the thread status */
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "unit %d DnxCtrBg thread is not responding\n", unit);
            }
            SHR_IF_ERR_EXIT(dnx_crps_eviction_db.bg_thread_data.thread_is_still_running.get
                            (unit, &thread_is_still_running));
        }

        SHR_IF_ERR_EXIT(dnx_crps_eviction_db.bg_thread_data.bgSem.destroy(unit));

        LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit, "unit %d counter background thread exit\n"), unit));
    }
exit:
    SHR_FUNC_EXIT;

}

/** see crps_eviction.h file */
shr_error_e
dnx_crps_dma_fifo_detach(
    int unit,
    int core_id,
    int fifo_id)
{
    int lock_taken = FALSE;
    int fifo_channel;
    SHR_FUNC_INIT_VARS(unit);
    /** only if the fifo wasn't connected to DMA channle, connect it */
    SHR_IF_ERR_EXIT(dnx_crps_eviction_db.fifo_channels.get(unit, core_id, fifo_id, &fifo_channel));

    if (fifo_channel != DNX_CRPS_INVALID_DMA_CHANNEL)
    {
        /** use the cache lock to lock the bg thread process function. otherwise, we might free the memory the that process function works on. */
        SHR_IF_ERR_EXIT(dnx_crps_fifo_state_lock_take(unit));
        lock_taken = TRUE;

        SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_free(unit, fifo_channel));
        SHR_IF_ERR_EXIT(dnx_crps_eviction_db.fifo_channels.set(unit, core_id, fifo_id, DNX_CRPS_INVALID_DMA_CHANNEL));
    }

exit:
    if (lock_taken)
    {
        /** allow access again */
        SHR_IF_ERR_EXIT(dnx_crps_fifo_state_lock_give(unit));
    }
    SHR_FUNC_EXIT;
}

/**
 * See crps_eviction.h
 */
shr_error_e
dnx_crps_eviction_dma_fifo_full_detach(
    int unit,
    bcm_core_t core)
{
    int core_id;
    int fifo_id;
    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, core, core_id)
    {
        DNX_CRPS_FIFOS_ITER(unit, DNX_CRPS_FIFO_ID_ALL, fifo_id)
        {
            SHR_IF_ERR_EXIT(dnx_crps_dma_fifo_detach(unit, core_id, fifo_id));
        }
    }

exit:
    SHR_FUNC_EXIT;

}

/** see .h file */
shr_error_e
dnx_crps_dma_fifo_attach(
    int unit,
    int core_id,
    int fifo_id)
{
    int channel_number;
    soc_dnx_fifodma_config_t info;
    soc_dnx_fifodma_src_t src[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][DNX_DATA_MAX_CRPS_EVICTION_NOF_COUNTERS_FIFO] =
        { {soc_dnx_fifodma_src_crps0_dma0, soc_dnx_fifodma_src_crps0_dma1},
    {soc_dnx_fifodma_src_crps1_dma0, soc_dnx_fifodma_src_crps1_dma1}
    };
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_crps_eviction_db.fifo_channels.get(unit, core_id, fifo_id, &channel_number));
    /** only if the fifo wasn't connected to DMA channel, connect it */
    if (channel_number == DNX_CRPS_INVALID_DMA_CHANNEL)
    {
        if (core_id >= DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "invalid core id (=%d)\n", core_id);
        }
        if (fifo_id >= DNX_DATA_MAX_CRPS_EVICTION_NOF_COUNTERS_FIFO)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "invalid fifo id (=%d)\n", fifo_id);
        }

        SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_alloc(unit, src[core_id][fifo_id], &channel_number));

        SHR_IF_ERR_EXIT(soc_dnx_fifodma_config_t_init(unit, &info));

        info.is_mem = FALSE;
        info.is_poll = TRUE;
        info.force_entry_size = 0;
        info.max_entries = CRPS_EVICTION_FIFO_DMA_MAX_ENTRIES;
        info.reg = CRPS_CRPS_DMA_ACCESSr;
        info.threshold = CRPS_EVICTION_FIFO_DMA_THRESHOLD;
        info.timeout = CRPS_EVICTION_FIFO_DMA_TIMOUT;
        info.copyno = core_id;
        info.array_index = fifo_id;

        SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_start(unit, channel_number, &info, NULL));
        SHR_IF_ERR_EXIT(dnx_crps_eviction_db.fifo_channels.set(unit, core_id, fifo_id, channel_number));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* See crps_eviction.h
*/
shr_error_e
dnx_crps_dma_fifo_full_attach(
    int unit,
    bcm_core_t core)
{
    int core_id, engine_id;
    int fifo_id;
    int valid;
    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, core, core_id)
    {
        for (engine_id = 0; engine_id < dnx_data_crps.engine.nof_engines_get(unit); engine_id++)
        {
            /** if engine is valid, connected to local host and in phy record_format, need to attach it */
            SHR_IF_ERR_EXIT(dnx_crps_mgmt_engine_eviction_valid_get(unit, core_id, engine_id, &valid));
            if (valid)
            {
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.dma_fifo.get(unit, core_id, engine_id, &fifo_id));
                /** attach this fifo_id. will be made only if not already attached */
                SHR_IF_ERR_EXIT(dnx_crps_dma_fifo_attach(unit, core_id, fifo_id));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   wrapper for attach the bg thread and connect the DMA FIFO to host
* \par DIRECT INPUT:
*   \param [in] unit    -  unit id
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
dnx_crps_eviction_counter_attach_init(
    int unit)
{
    int core_idx, engine_idx;
    int bg_thread_enable_called = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    /** attach dma channels for all active engines and if needed, also acivate the bg thread one time */
    /** attached it anyway, even if bg thread is disabled, for users who has their own bg thread */
    /** relevant only in warmboot. for cold boot, all engines are not active yet, so no need to attach. */
    /** each time new engine will be enabled, we will attached its fifo */
    if (sw_state_is_warm_boot(unit))
    {
        DNXCMN_CORES_ITER(unit, _SHR_CORE_ALL, core_idx)
        {
            for (engine_idx = 0; engine_idx < dnx_data_crps.engine.nof_engines_get(unit); engine_idx++)
            {
                /** attach dma fifo and activate bg thread if needed. The handle function check and activate FIFO/Bg thread if need to */
                SHR_IF_ERR_EXIT(dnx_crps_eviction_collection_handle
                                (unit, core_idx, engine_idx, &bg_thread_enable_called));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  enable/disable the eviction for all engines, that the database indicated that their eviction should be enabled.
 *  Enable/Disable is made in this function without update the sw state.
 *  It is made to ensure during warmboot, there will not be any eviction process.
 * \param [in] unit -unit id
 * \param [in] enable -True=enable/ false=disable
 * \return
 *  shr_error_e
 * \remark
 *   NONE
 * \see
 *   NONE
 */
static shr_error_e
dnx_crps_eviction_enable_all_active(
    int unit,
    int enable)
{
    uint32 entry_handle_id;
    int core_idx;
    int engine_idx;
    int sw_state_eviction_enable, valid;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, _SHR_CORE_ALL, core_idx)
    {
        for (engine_idx = 0; engine_idx < dnx_data_crps.engine.nof_engines_get(unit); engine_idx++)
        {
            SHR_IF_ERR_EXIT(dnx_crps_mgmt_engine_eviction_valid_get(unit, core_idx, engine_idx, &valid));
            SHR_IF_ERR_EXIT(dnx_crps_db.proc.enablers.eviction_enable.get
                            (unit, core_idx, engine_idx, &sw_state_eviction_enable));
            /** only if the engine eviction is valid and eviction is enabled - enable/disable the eviction */
            if (sw_state_eviction_enable == TRUE && valid == TRUE)
            {
                /** Taking a handle */
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_ENGINE_ENABLE, &entry_handle_id));

                /** Setting key fields */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_ID, engine_idx);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_idx);
                /** enable/disable the eviction */
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_ENABLE, INST_SINGLE,
                                             enable);
                /** Preforming the action */
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
                DBAL_HANDLE_FREE(unit, entry_handle_id);
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/** see .h file */
shr_error_e
dnx_crps_eviction_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** attach the bg thread and DMA FIFO */
    SHR_IF_ERR_EXIT(dnx_crps_eviction_counter_attach_init(unit));

    /** if warmboot, enable eviction for all engines that their sw state indicates on eviction enabled  */
    if (sw_state_is_warm_boot(unit))
    {
        SHR_IF_ERR_EXIT(dnx_crps_eviction_enable_all_active(unit, TRUE));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  function calling the fifo process procedure by demend, as foreground operation.
 *  the function lock the counters database, before calling the process and release it after.
 *
 * \param [in] unit -unit id
 * \return
 *  shr_error_e
 * \remark
 *   NONE
 * \see
 *   NONE
 */
static shr_error_e
dnx_crps_eviction_foreground_fifo_process(
    int unit)
{
    int depth;
    int lock_taken = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    /** lock using mutex instead of spin loops */
    SHR_IF_ERR_EXIT(dnx_crps_fifo_state_lock_take(unit));
    lock_taken = TRUE;
    /** process all of the active FIFOs */
    SHR_IF_ERR_EXIT(dnx_crps_eviction_counter_fifo_process(unit, _SHR_CORE_ALL, DNX_CRPS_FIFO_ID_ALL, &depth));

exit:
    if (lock_taken)
    {
        /** allow access again */
        SHR_IF_ERR_EXIT(dnx_crps_fifo_state_lock_give(unit));
    }
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_eviction_disable_wrapper(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** disable the eviction from all engines */
    SHR_IF_ERR_EXIT(dnx_crps_eviction_enable_all_active(unit, FALSE));

    /** wait 20 micro to ensure last counters were copied to cpu */
    sal_usleep(20);

    /** process the fifo one more time before exit */
    SHR_IF_ERR_EXIT(dnx_crps_eviction_foreground_fifo_process(unit));

    /** destroy the background thread */
    SHR_IF_ERR_CONT(dnx_crps_eviction_bg_thread_detach(unit));
exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_eviction_deinit(
    int unit)
{
    uint8 is_init = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_CONT(dnx_crps_eviction_db.is_init(unit, &is_init));

    if (is_init)
    {
        /** disable the eviction from all engines */
        SHR_IF_ERR_CONT(dnx_crps_eviction_disable_wrapper(unit));
        /** detach the DMA FIFO */
        SHR_IF_ERR_CONT(dnx_crps_eviction_dma_fifo_full_detach(unit, _SHR_CORE_ALL));
    }

    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_eviction_counters_get(
    int unit,
    int flags,
    int core_id,
    int engine_id,
    int nstat,
    int *counters_id,
    int *sub_count,
    bcm_stat_counter_output_data_t * output_data)
{
    int cnt_idx;
    uint64 value;
    int lock_taken = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_crps_counter_state_lock_take(unit));
    lock_taken = TRUE;
    /** read the counter from sw state counters database */
    for (cnt_idx = 0; cnt_idx < nstat; cnt_idx++)
    {
        if ((counters_id[cnt_idx] != -1) && (sub_count[cnt_idx] != -1))
        {
            SHR_IF_ERR_EXIT(dnx_crps_ctrs_db_counter_get
                            (unit, flags, core_id, engine_id, counters_id[cnt_idx], sub_count[cnt_idx], &value));
            COMPILER_64_COPY(output_data->value_arr[cnt_idx].value, value);
        }
    }

exit:
    if (lock_taken)
    {
        SHR_IF_ERR_EXIT(dnx_crps_counter_state_lock_give(unit));
    }
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_eviction_seq_timer_set(
    int unit,
    bcm_stat_engine_t * engine,
    uint32 time_between_scan_usec)
{
    uint32 seq_timer_interval, alg_timer_interval;
    uint32 entry_handle_id;
    dnxcmn_time_t time;
    uint32 was_initially_enabled;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.
                    sequential_timer_value.set(unit, engine->core_id, engine->engine_id, time_between_scan_usec));

    /** Taking a handle - engine scan disable */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_ENGINE_ENABLE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_ID, engine->engine_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, engine->core_id);
    /** first check if scan is enabled at all */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_ENABLE, INST_SINGLE,
                               &was_initially_enabled);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    if (was_initially_enabled == TRUE)
    {
        /** enable the scan */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_ENABLE, INST_SINGLE, FALSE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_ENGINE_SCAN_CFG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_ID, engine->engine_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, engine->core_id);

    /** Setting value fields */
    /** seq_timer_interval calculation. result is in clocks resolution */
    COMPILER_64_ZERO(time.time);
    COMPILER_64_ADD_32(time.time, time_between_scan_usec);
    time.time_units = DNXCMN_TIME_UNIT_USEC;
    SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &time, &seq_timer_interval));

    if (TRUE == dnx_data_crps.eviction.feature_get(unit, dnx_data_crps_eviction_algorithmic_is_supported))
    {
        /** Get alg timer interval */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_WAIT_TIME_BETWEEN_ALG_SCANS,
                                   INST_SINGLE, &alg_timer_interval);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        /** if alg and seq overlap, alg has priority over seq causing
         *  seq not to be able to evict */
        if (alg_timer_interval == seq_timer_interval)
        {
            seq_timer_interval += 1;
        }
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_WAIT_TIME_BETWEEN_SEQ_SCANS,
                                 INST_SINGLE, seq_timer_interval);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    if (was_initially_enabled == TRUE)
    {
        /** Taking a handle - engine scan disable */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_ENGINE_ENABLE, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_ID, engine->engine_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, engine->core_id);
        /** enable the scan */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_ENABLE, INST_SINGLE, TRUE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_eviction_seq_timer_get(
    int unit,
    bcm_stat_engine_t * engine,
    uint32 *time_between_scan_usec)
{
    uint32 entry_handle_id;
    uint32 seq_timer_interval;
    dnxcmn_time_t time;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_ENGINE_SCAN_CFG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_ID, engine->engine_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, engine->core_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_WAIT_TIME_BETWEEN_SEQ_SCANS,
                               INST_SINGLE, &seq_timer_interval);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** convert clocks to micro sec */
    SHR_IF_ERR_EXIT(dnxcmn_clock_cycles_to_time_get(unit, seq_timer_interval, DNXCMN_TIME_UNIT_USEC, &time));

    *time_between_scan_usec = COMPILER_64_LO(time.time);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_eviction_boundaries_set(
    int unit,
    uint32 flags,
    bcm_stat_engine_t * engine,
    bcm_stat_eviction_boundaries_t * boundaries)
{
    bcm_stat_counter_format_type_t format;
    int nof_counters_per_entry;
    uint32 entry_handle_id, was_initially_enabled = 0;
    uint32 first, last;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** In order to make HW refresh the scan according to the new
     *  boundaries need to disable the scan -> change the
     *  boundaries -> enable the scan */
     /** Taking a handle - engine scan disable */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_ENGINE_ENABLE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_ID, engine->engine_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, engine->core_id);
    /** first check if scan is enabled at all */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_ENABLE, INST_SINGLE,
                               &was_initially_enabled);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    if (was_initially_enabled)
    {
        /** disable the scan */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_ENABLE, INST_SINGLE, FALSE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    DBAL_HANDLE_FREE(unit, entry_handle_id);

    /** Taking a handle - engine scann configuration */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_ENGINE_SCAN_CFG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_ID, engine->engine_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, engine->core_id);

    /** Setting value fields */
    /** set default values */
    if (flags & BCM_STAT_EVICTION_RANGE_ALL)
    {
        /** fisrt addres to scan sequentialy is alsway zero, unless the user asked something else explicitly. */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_SEQ_BASE_ADDRESS_START,
                                     INST_SINGLE, 0);

        /** get how many counters in each one counter entry: 2 counters for pkts_and_pkts and max formats, else 1 counter */
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.format.get(unit, engine->core_id, engine->engine_id, &format));
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_nof_counters_per_entry_get(unit, format, &nof_counters_per_entry));
         /** the last address to scan sequentialy, is calculated according to the range size */
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.log_obj_id_range.logical_object_id_first.get
                        (unit, engine->core_id, engine->engine_id, &first));
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.log_obj_id_range.logical_object_id_last.get
                        (unit, engine->core_id, engine->engine_id, &last));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_SEQ_BASE_ADDRESS_END,
                                     INST_SINGLE, (last - first));
    }
    /** set the user given values */
    else
    {
        /**  fisrt addres to scan sequentialy is given by the user  */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_SEQ_BASE_ADDRESS_START,
                                     INST_SINGLE, boundaries->start);

         /** the last address to scan sequentialy is given by the user */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_SEQ_BASE_ADDRESS_END,
                                     INST_SINGLE, boundaries->end);
    }

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    DBAL_HANDLE_FREE(unit, entry_handle_id);

    if (was_initially_enabled)
    {
        /** Taking a handle - engine scan disable */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_ENGINE_ENABLE, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_ID, engine->engine_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, engine->core_id);
        /** enable the scan */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_ENABLE, INST_SINGLE, TRUE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_eviction_boundaries_get(
    int unit,
    uint32 flags,
    bcm_stat_engine_t * engine,
    bcm_stat_eviction_boundaries_t * boundaries)
{
    uint32 start, end;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_ENGINE_SCAN_CFG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_ID, engine->engine_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, engine->core_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_SEQ_BASE_ADDRESS_START,
                               INST_SINGLE, &start);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_SEQ_BASE_ADDRESS_END,
                               INST_SINGLE, &end);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    boundaries->start = start;
    boundaries->end = end;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_eviction_collection_handle(
    int unit,
    int core_id,
    int engine_id,
    int *bg_thread_enable_called)
{
    int bg_thread_enable;
    int valid;
    int fifo_id;
    int running;
    SHR_FUNC_INIT_VARS(unit);

    /** attach dma fifo before enable the CRPS hw. attach will be made only if fifo_id is not already attached */
    /** if engine is valid, connected to local host and has phy record format, need to attach it and also activate the bg thread (if not active) */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_engine_eviction_valid_get(unit, core_id, engine_id, &valid));
    if (valid)
    {
        /** get from db the fifo_id parameters */
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.dma_fifo.get(unit, core_id, engine_id, &fifo_id));

        /** dma fifo attach. it will be made only if not already attached. */
        /** (attach should be made even if bg thread disable for user who mange their own bg thread */
        SHR_IF_ERR_EXIT(dnx_crps_dma_fifo_attach(unit, core_id, fifo_id));

        /** get from sw state  (updated by Soc/API user config, if the user decide to enable the thread  */
        SHR_IF_ERR_EXIT(dnx_crps_db.bg_thread_enable.get(unit, &bg_thread_enable));

        /** attach the bg thread one time. */
        /** it happens in the first engine enable with valid eviction */
        SHR_IF_ERR_EXIT(dnx_crps_eviction_db.bg_thread_data.running.get(unit, &running));
        if ((bg_thread_enable == TRUE) && (running == FALSE) && (*bg_thread_enable_called == FALSE))
        {
            LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit, "unit %d counter background thread init\n"), unit));
            SHR_IF_ERR_EXIT(dnx_crps_eviction_db.bg_thread_data.bgWait.set
                            (unit, DNX_CRPS_BACKGROUND_THREAD_ITER_DEFAULT));
            SHR_IF_ERR_EXIT(dnx_crps_eviction_bg_thread_attach(unit));
            *bg_thread_enable_called = TRUE;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_bg_thread_detach_check(
    int unit)
{
    int core_idx, fifo_id;
    int bg_thread_detach = TRUE;
    int fifo_channel;
    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, _SHR_CORE_ALL, core_idx)
    {
        DNX_CRPS_FIFOS_ITER(unit, DNX_CRPS_FIFO_ID_ALL, fifo_id)
        {
            SHR_IF_ERR_EXIT(dnx_crps_eviction_db.fifo_channels.get(unit, core_idx, fifo_id, &fifo_channel));
            if (fifo_channel != DNX_CRPS_INVALID_DMA_CHANNEL)
            {
                bg_thread_detach = FALSE;
                break;
            }
        }
    }
    /** if all FIFOs (all cores) are invalid, no need to keep the bg thread active.*/
    if (bg_thread_detach == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_crps_eviction_bg_thread_detach(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  set the scan mode for given engine - no scan/
 *  sseq&algorithmic/ only algorithmic - used to enable/disable
 *  the sequential
 * \param [in] unit -unit id
 * \param [in] core_id - core id
 * \param [in] engine_id - engine id
 * \param [in] set_seq_on - if sequential scan is on/off
 * \param [in] scan_mode - eviction scan mode
 * \return
 *  shr_error_e
 * \remark
 *   NONE
 * \see
 *   NONE
 */
static shr_error_e
dnx_crps_engine_eviction_scan_mode_set(
    int unit,
    int core_id,
    int engine_id,
    int set_seq_on,
    uint32 scan_mode)
{
    uint32 entry_handle_id, scan_mode_to_set[1];
    int seq_bit_index = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    scan_mode_to_set[0] = scan_mode;
    /** possible cases: no scan = 0, seq=1, alg=2, seq+alg=3
     *  set_seq_on = 1 should set the sequential bit on,
     *  sequential bit index is 0
     *  set_seq_on = 0 should set the sequentail bit off */
    if (set_seq_on)
    {
        SHR_BITSET(scan_mode_to_set, seq_bit_index);
    }
    else
    {
        SHR_BITCLR(scan_mode_to_set, seq_bit_index);
    }
    /** algoritmic scan is not supported for some devices */
    if (FALSE == dnx_data_crps.eviction.feature_get(unit, dnx_data_crps_eviction_algorithmic_is_supported)
        && scan_mode == DBAL_ENUM_FVAL_CRPS_ENGINE_SCAN_MODE_ALGORITHIM_SCAN)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Incorrect configuration - scan mode not supported. \n");
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_ENGINE_SCAN_CFG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_ID, engine_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_MODE, INST_SINGLE,
                                 scan_mode_to_set[0]);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  get the scan mode for given engine - no scan/
 *  sseq&algorithmic/ only algorithmic
 * \param [in] unit -unit id
 * \param [in] core_id - core id
 * \param [in] engine_id - engine id
 * \param [out] scan_mode - eviction scan mode
 * \return
 *  shr_error_e
 * \remark
 *   NONE
 * \see
 *   NONE
 */
static shr_error_e
dnx_crps_engine_eviction_scan_mode_get(
    int unit,
    int core_id,
    int engine_id,
    uint32 *scan_mode)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_ENGINE_SCAN_CFG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_ID, engine_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_MODE, INST_SINGLE, scan_mode);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  get the start and end engine belonging to a given type
 * \param [in] unit -unit id
 * \param [in] database - database id and core id
 * \param [in] type_id - type id
 * \param [out] type_engine_start - first engine belonging to
 *        this type
 * \param [out] type_counter_id_start - the real counter id
 *        representing the type range start
 * \param [out] type_engine_end - last engine belonging to this
 *        type
 * \param [out] type_counter_id_end - the real counter id
 *        representing the type range end
 * \return
 *  shr_error_e
 * \remark
 *   NONE
 * \see
 *   NONE
 */
static shr_error_e
dnx_crps_database_type_range_start_end_engines_get(
    int unit,
    bcm_stat_counter_database_t * database,
    int type_id,
    int *type_engine_start,
    int *type_counter_id_start,
    int *type_engine_end,
    int *type_counter_id_end)
{
    int engine_id, start_found = 0, found = 0, counter_set_offset_start = 0, counter_set_offset_end =
        0, counter_set_size, nof_counters_per_entry;
    uint32 counter_id;
    dnx_type_config_t type;
    bcm_stat_counter_format_type_t format;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_crps_db.database.type.get(unit, database->core_id, database->database_id, type_id, &type));

    /** start going through the engines in the database - search
     *  for the type range start, if found search for type end  */
    SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.get(unit, database->core_id, database->database_id, &engine_id));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.
                    counter_set_size.get(unit, database->core_id, engine_id, &counter_set_size));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.format.get(unit, database->core_id, engine_id, &format));
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_nof_counters_per_entry_get(unit, format, &nof_counters_per_entry));
    counter_set_offset_end = counter_set_size - 1;
    while (engine_id != DNX_CRPS_MGMT_ENGINE_INVALID)
    {
        if (!start_found)
        {
            SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_find
                            (unit, database->core_id, engine_id, type_id, type.start, counter_set_offset_start, &found,
                             &counter_id));
            if (found)
            {
                start_found = 1;
                *type_engine_start = engine_id;
                *type_counter_id_start = counter_id / nof_counters_per_entry;
            }
        }
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_find
                        (unit, database->core_id, engine_id, type_id, type.end, counter_set_offset_end, &found,
                         &counter_id));
        if (found)
        {
            *type_engine_end = engine_id;
            *type_counter_id_end = counter_id / nof_counters_per_entry;
            break;
        }
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.next_engine_id.get(unit, database->core_id, engine_id, &engine_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  give type id and get the prev/next type id that belong to
 *  the engine
 * \param [in] unit -unit id
 * \param [in] database - database id and core id
 * \param [in] engine_id - engine id
 * \param [in] current_type_id - type id used in the modify
 * \param [out] prev_type_id - previous type id belonging to the
 *        engine (if such exists)
 * \param [out] last_next_type_id - next type id belonging to
 *                the engine (if such exists) - return the last
 *                type id + in the engine after the current
 *                given type id
 * \return
 *  shr_error_e
 * \remark
 *   NONE
 * \see
 *   NONE
 */
static shr_error_e
dnx_crps_engine_get_types(
    int unit,
    bcm_stat_counter_database_t * database,
    int engine_id,
    int current_type_id,
    int *prev_type_id,
    int *last_next_type_id)
{
    int type_id, valid = 0;
    dnx_type_config_t type;
    SHR_FUNC_INIT_VARS(unit);

    *prev_type_id = DNX_CRPS_INVALID_TYPE_ID;
    *last_next_type_id = DNX_CRPS_INVALID_TYPE_ID;
    for (type_id = 0; type_id < BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES; type_id++)
    {
        if (type_id == current_type_id)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_crps_db.database.type.get(unit, database->core_id, database->database_id, type_id, &type));
        if (type.valid == TRUE)
        {
            /** check if it is valid for the given engine */
            SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.
                            type_hw_valid.get(unit, database->core_id, engine_id, type_id, &valid));
            if (valid)
            {
                if (type_id < current_type_id)
                {
                    *prev_type_id = type_id;
                }
                else
                {
                    *last_next_type_id = type_id;
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  once engine is excluded from the new boundaries need to
 *  verify if it is in use by other types - if it is - need to
 *  update the boundaries of the engine according to the other
 *  types; if it is not used by other engines - need to
 * \param [in] unit -unit id
 * \param [in] database - database id and core id
 * \param [in] engine_id - engine id
 * \param [in] type_id - type id used in the modify
 * \return
 *  shr_error_e
 * \remark
 *   NONE
 * \see
 *   NONE
 */
static shr_error_e
dnx_crps_engine_set_seq_off_or_update_boundaries(
    int unit,
    bcm_stat_counter_database_t * database,
    int engine_id,
    int type_id)
{
    int prev_type_id, next_type_id;
    uint32 scan_mode;
    bcm_stat_engine_t engine;
    bcm_stat_eviction_boundaries_t new_boundaries, old_boundaries;
    int type_counter_id_start, type_counter_id_end, type_engine_start, type_engine_end;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_crps_engine_get_types(unit, database, engine_id, type_id, &prev_type_id, &next_type_id));
    engine.core_id = database->core_id;
    engine.engine_id = engine_id;
    /** get current boundaries of the engine */
    SHR_IF_ERR_EXIT(dnx_crps_eviction_boundaries_get(unit, 0, &engine, &old_boundaries));
    /** depending on the type configuraion there is a possibility
     *  that more then one type belongs to active engine -
     *  meaning that the sequential cannot be disabled for that
     *  case */
    if (prev_type_id == DNX_CRPS_INVALID_TYPE_ID && next_type_id == DNX_CRPS_INVALID_TYPE_ID)
    {
        /** if there are none other types to this engine - disable
         *  the sequential */
        SHR_IF_ERR_EXIT(dnx_crps_engine_eviction_scan_mode_get(unit, database->core_id, engine_id, &scan_mode));
        SHR_IF_ERR_EXIT(dnx_crps_engine_eviction_scan_mode_set(unit, database->core_id, engine_id, 0, scan_mode));
    }
    else
    {
        /** valid case will be to have either previous type id in the
         *  engine or next type id in the engine */
        if (prev_type_id != DNX_CRPS_INVALID_TYPE_ID)
        {
            /** example
             *  [engine1][engine2][engine3]
             *  [******type0][type1**]
             *  new boundaries for type1
             *                                    [**]
             * engine2 should have boundaries:
             * [first index of engine,end of type0]
 */
            new_boundaries.start = old_boundaries.start;
            SHR_IF_ERR_EXIT(dnx_crps_database_type_range_start_end_engines_get
                            (unit, database, prev_type_id, &type_engine_start, &type_counter_id_start, &type_engine_end,
                             &type_counter_id_end));
            new_boundaries.end = type_counter_id_end;
        }
        if (next_type_id != DNX_CRPS_INVALID_TYPE_ID)
        {
            /** example
             *  [engine1][engine2][engine3]
             *  [******type0][type1**]
             *  new boundaries for type0
             *    [**]
             * engine2 should have boundaries:
             * [start of type1, last index of engine]
 */
            SHR_IF_ERR_EXIT(dnx_crps_database_type_range_start_end_engines_get
                            (unit, database, next_type_id, &type_engine_start, &type_counter_id_start, &type_engine_end,
                             &type_counter_id_end));
            new_boundaries.start = type_counter_id_start;
            new_boundaries.end = old_boundaries.end;
        }

        SHR_IF_ERR_EXIT(dnx_crps_eviction_boundaries_set(unit, 0, &engine, &new_boundaries));
    }
exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_database_eviction_boundaries_range_all_set(
    int unit,
    bcm_stat_counter_database_t * database,
    int type_id,
    bcm_stat_eviction_boundaries_t * boundaries)
{
    int engine_id, type_engine_start, type_engine_end;
    bcm_stat_engine_t engine;
    int type_counter_id_start, type_counter_id_end;
    uint32 scan_mode;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_crps_database_type_range_start_end_engines_get
                    (unit, database, type_id, &type_engine_start, &type_counter_id_start, &type_engine_end,
                     &type_counter_id_end));
    /** go over all engines belonging to the type and set range all
     *  to each engine */
    engine_id = type_engine_start;
    while (engine_id != DNX_CRPS_MGMT_ENGINE_INVALID)
    {
        engine.core_id = database->core_id;
        engine.engine_id = engine_id;
        SHR_IF_ERR_EXIT(dnx_crps_engine_eviction_scan_mode_get(unit, database->core_id, engine_id, &scan_mode));
        if (!DNX_CRPS_EVICTION_SEQUENTIAL_SCAN_MODE_IS_ON(scan_mode))
        {
            SHR_IF_ERR_EXIT(dnx_crps_engine_eviction_scan_mode_set(unit, database->core_id, engine_id, 1, scan_mode));
        }
        SHR_IF_ERR_EXIT(dnx_crps_eviction_boundaries_set(unit, BCM_STAT_EVICTION_RANGE_ALL, &engine, boundaries));
        if (engine_id == type_engine_end)
        {
            break;
        }
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.next_engine_id.get(unit, database->core_id, engine_id, &engine_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  according to input parameters set new boundaries for the
 *  given engine (current engine)
 * \param [in] unit -unit id
 * \param [in] core_id - core id
 * \param [in] next_type_id - next type id (needed for the last
 *        engine - to check if there are other valid types in
 *        the engine)
 * \param [in] start_engine_id - range start engine id
 * \param [in] end_engine_id - range end engine id
 * \param [in] current_engine_id - engine id currently checked
 * \param [in] end_is_not_found - is the end of range found yet
 * \param [in] counter_id_start -  counter id start
 * \param [in] counter_id_end - counter  id end
 * \return
 *  shr_error_e
 * \remark
 *   NONE
 * \see
 *   NONE
 */
static shr_error_e
dnx_crps_database_eviction_current_engine_boundaries_set(
    int unit,
    int core_id,
    int next_type_id,
    int start_engine_id,
    int end_engine_id,
    int current_engine_id,
    int end_is_not_found,
    int counter_id_start,
    int counter_id_end)
{
    bcm_stat_engine_t engine;
    bcm_stat_eviction_boundaries_t new_boundaries;
    uint32 last = 0, first = 0, first_id, last_id;
    SHR_FUNC_INIT_VARS(unit);

     /** regarding boundaries per engine - they are according to
     *  the engine counters meaning start from 0 until the end
     *  counter of the engine - need to calculate the end */
    first = 0;
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.log_obj_id_range.
                    logical_object_id_first.get(unit, core_id, current_engine_id, &first_id));
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.log_obj_id_range.
                    logical_object_id_last.get(unit, core_id, current_engine_id, &last_id));
    last = last_id - first_id;

    engine.core_id = core_id;
    /** set the new boundaries - start from start engine */
    if (current_engine_id == start_engine_id)
    {
        engine.engine_id = start_engine_id;
        new_boundaries.start = counter_id_start;
        /** check if end is in this engine */
        if (start_engine_id == end_engine_id || end_is_not_found == 0)
        {
            new_boundaries.end = counter_id_end;
        }
        else
        {
            /** if end is not in the same engine as the start - set the
             *  boundaries for the start engine - from the start till the
             *  last possible counter id in the engine */
            new_boundaries.end = last;
        }
    }
    else
    {
        /** in this case start and end are not in the same engine,
         *  meaning the engine after the start should start from the
         *  begining of the engine boundaries */
        engine.engine_id = current_engine_id;
        new_boundaries.start = first;
        if (end_is_not_found)
        {
            SHR_IF_ERR_EXIT(dnx_crps_eviction_boundaries_set
                            (unit, BCM_STAT_EVICTION_RANGE_ALL, &engine, &new_boundaries));
            SHR_EXIT();
        }
        else
        {
            /**
             * if we are in the end engine we need to check if there is
             * other type that has range in the current engine - if there
             * is, the boundaries should not be changed for this engine - as
             * there is oter valid type we don't want to corrupt
             */
            if (next_type_id != DNX_CRPS_INVALID_TYPE_ID)
            {
                SHR_IF_ERR_EXIT(dnx_crps_eviction_boundaries_set
                                (unit, BCM_STAT_EVICTION_RANGE_ALL, &engine, &new_boundaries));
                SHR_EXIT();
            }
            else
            {
                new_boundaries.end = counter_id_end;
            }
        }
    }
    SHR_IF_ERR_EXIT(dnx_crps_eviction_boundaries_set(unit, 0, &engine, &new_boundaries));
exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_database_eviction_boundaries_set(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    int type_id,
    bcm_stat_eviction_boundaries_t * boundaries)
{
    int engine_id, start_engine_id = 0, end_engine_id = 0, counter_set_offset_start = 0, counter_set_offset_end = 0;
    uint32 counter_id_start = 0, counter_id_end = 0, scan_mode;
    int found = 0, start_is_found = 0, end_is_not_found = 1, counter_set_size = 0, type_engine_start, type_engine_end;
    int type_counter_id_start, type_counter_id_end;
    int prev_type_id, next_type_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Need to convert the given boundaries from obj stat id
     *  resolution to counter id resolution - go over each engine
     *  belonging to the given type and modify accordingly the
     *  engine sequential eviction boundaries */
    SHR_IF_ERR_EXIT(dnx_crps_database_type_range_start_end_engines_get
                    (unit, database, type_id, &type_engine_start, &type_counter_id_start, &type_engine_end,
                     &type_counter_id_end));
    /** specific case is when type range belongs to only 1 engine
     *  and two other types belong to it - the engine should
     *  remain with the default boundaries otherwise the other
     *  types will be corrupted. Example:
     *  |***engine1***||***engine2***||***engine3***|
     *   |~~~type0~~~||type1||~~~type2~~~| */
    if (type_engine_start == type_engine_end)
    {
        SHR_IF_ERR_EXIT(dnx_crps_engine_get_types
                        (unit, database, type_engine_start, type_id, &prev_type_id, &next_type_id));
        if (prev_type_id != DNX_CRPS_INVALID_TYPE_ID && next_type_id != DNX_CRPS_INVALID_TYPE_ID)
        {
            SHR_EXIT();
        }
    }
    /** start going through the engines in the database - search
     *  for the start, if there are unused engines in the
     *  begining of the database - disable the sequential
     *  mechanism for them */
    engine_id = type_engine_start;
    while (engine_id != DNX_CRPS_MGMT_ENGINE_INVALID)
    {
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_find
                        (unit, database->core_id, engine_id, type_id, boundaries->start, counter_set_offset_start,
                         &found, &counter_id_start));
        SHR_IF_ERR_EXIT(dnx_crps_engine_eviction_scan_mode_get(unit, database->core_id, engine_id, &scan_mode));
        if (!found)
        {
            /** valid input is checked - we expect sequential range, if
             *  the start counter is not found in this engine means this
             *  engine is not used - disable the sequential eviction;
             *  check if sequential eviction was enabled at all - it no
             *  eviction - keep it like that */
            if (DNX_CRPS_EVICTION_SEQUENTIAL_SCAN_MODE_IS_ON(scan_mode))
            {
                /** depending on the type configuraion there is a possibility
                 *  that more then one type belongs to active engine -
                 *  meaning that the sequential cannot be disabled for that
                 *  case */
                SHR_IF_ERR_EXIT(dnx_crps_engine_set_seq_off_or_update_boundaries(unit, database, engine_id, type_id));
            }
        }
        else
        {
            /** start boundary is found in current engine */
            /** calculate the coutner id according to the nof counter per
             *  counter entry */
            counter_id_start = counter_id_start;
            start_engine_id = engine_id;
            start_is_found = 1;
            /** if sequential eviction was disabled at some point - need
             *  to enable it again */
            if (!DNX_CRPS_EVICTION_SEQUENTIAL_SCAN_MODE_IS_ON(scan_mode))
            {
                SHR_IF_ERR_EXIT(dnx_crps_engine_eviction_scan_mode_set
                                (unit, database->core_id, engine_id, 1, scan_mode));
            }
        }

        if (start_is_found)
        {
            break;
        }
        if (engine_id == type_engine_end)
        {
            break;
        }
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.next_engine_id.get(unit, database->core_id, engine_id, &engine_id));
    }

    /** start is already found, continue going through the
     *  engines from start engine - end can be in the same engine */
    end_is_not_found = 1;
    engine_id = start_engine_id;
    /** for end boundary - last counter is the last offset
     *  counter id - need to know the counter set size */
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.
                    counter_set_size.get(unit, database->core_id, engine_id, &counter_set_size));
    counter_set_offset_end = counter_set_size - 1;

    while (engine_id != DNX_CRPS_MGMT_ENGINE_INVALID)
    {
        SHR_IF_ERR_EXIT(dnx_crps_engine_eviction_scan_mode_get(unit, database->core_id, engine_id, &scan_mode));
        if (end_is_not_found)
        {
            /** doesn't matter if end is in current engine or in some of
             *  the next - this engine is part of the eviction range and
             *  sequential eviction should be enabled */
            if (!DNX_CRPS_EVICTION_SEQUENTIAL_SCAN_MODE_IS_ON(scan_mode))
            {
                SHR_IF_ERR_EXIT(dnx_crps_engine_eviction_scan_mode_set
                                (unit, database->core_id, engine_id, 1, scan_mode));
            }
            SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_find
                            (unit, database->core_id, engine_id, type_id, boundaries->end, counter_set_offset_end,
                             &found, &counter_id_end));
            if (found)
            {
                end_is_not_found = 0;
                end_engine_id = engine_id;
            }
            /** get if there is next type id for the current engine */
            SHR_IF_ERR_EXIT(dnx_crps_engine_get_types
                            (unit, database, engine_id, type_id, &prev_type_id, &next_type_id));
            /** set the new boundaries for the current engine */
            SHR_IF_ERR_EXIT(dnx_crps_database_eviction_current_engine_boundaries_set
                            (unit, database->core_id, next_type_id, start_engine_id, end_engine_id, engine_id,
                             end_is_not_found, counter_id_start, counter_id_end));
        }
        else
        {
            /** if end is already found - need to go over the other
             *  engines and disable the sequential eviction for them
             *  depending on the type configuraion there is a
             *  possibility
             *  that more then one type belongs to active engine
             *  meaning that the sequential cannot be disabled for that
             *  case */
            if (DNX_CRPS_EVICTION_SEQUENTIAL_SCAN_MODE_IS_ON(scan_mode))
            {
                SHR_IF_ERR_EXIT(dnx_crps_engine_set_seq_off_or_update_boundaries(unit, database, engine_id, type_id));
            }
        }
        if (engine_id == type_engine_end)
        {
            break;
        }
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.next_engine_id.get(unit, database->core_id, engine_id, &engine_id));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_database_eviction_boundaries_get(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    int type_id,
    bcm_stat_eviction_boundaries_t * boundaries)
{
    int engine_id, start_found = 0, type_engine_start, type_engine_end;
    int counter_set_offset_start = 0, counter_set_offset_end = 0, counter_set_size;
    uint32 scan_mode;
    int start_engine_id = 0, end_engine_id = 0, start_counter_id = 0, end_counter_id = 0;
    bcm_stat_engine_t engine;
    bcm_stat_eviction_boundaries_t boundaries_get;
    int type_counter_id_start, type_counter_id_end, prev_type_id, next_type_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_crps_database_type_range_start_end_engines_get
                    (unit, database, type_id, &type_engine_start, &type_counter_id_start, &type_engine_end,
                     &type_counter_id_end));
    engine_id = type_engine_start;
    while (engine_id != DNX_CRPS_MGMT_ENGINE_INVALID)
    {
        /** as the set goes - the sequential eviction is off if the
         *  engine is not used - check the scan mode */
        SHR_IF_ERR_EXIT(dnx_crps_engine_eviction_scan_mode_get(unit, database->core_id, engine_id, &scan_mode));
        if (DNX_CRPS_EVICTION_SEQUENTIAL_SCAN_MODE_IS_ON(scan_mode))
        {
            /** get the boundaries for this engine */
            engine.core_id = database->core_id;
            engine.engine_id = engine_id;
            SHR_IF_ERR_EXIT(dnx_crps_eviction_boundaries_get(unit, 0, &engine, &boundaries_get));
            SHR_IF_ERR_EXIT(dnx_crps_engine_get_types
                            (unit, database, engine_id, type_id, &prev_type_id, &next_type_id));
            /** if seq is on - engine is part of range; if start was not
             *  yet found - this is the first engine from the range */
            if (!start_found)
            {
                start_engine_id = engine_id;
                start_found = 1;
                /** valid scenario is type start to be in the middle of
                 *  engine, if no changes were made the boundaries of the
                 *  engines will be the default - full engine, we want to
                 *  return the user the boundaries that are relevant for the
                 *  type it was selected example: database = engine0+engine7,
                 *  type0 holds half engine0, type 1 holds other half
                 *  engine0&engine7, API called with type 1 should return the
                 *  boundaries relevant for type 1 not for engine 0 */
                start_counter_id = (type_counter_id_start > boundaries_get.start
                                    && engine_id == type_engine_start) ? type_counter_id_start : boundaries_get.start;
            }
            /** save the last found end - at some point it will be the
             *  valid end */
            /** if last engine and there is other type id in this engine
             *  - need to return the type end instaed of engine boundary
             *    end */
            end_counter_id = (engine_id == type_engine_end
                              && next_type_id != DNX_CRPS_INVALID_TYPE_ID) ? type_counter_id_end : boundaries_get.end;
            end_engine_id = engine_id;
        }
        if ((DNX_CRPS_EVICTION_SEQUENTIAL_SCAN_MODE_IS_ON(scan_mode) == FALSE) && (start_found == 1))
        {
            /** if start was already found and current engine is not in
             *  sequential mode - this is the first engine after the
             *  range previous engine was holding the end of the
             *  boundaries */
            SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.
                            prev_engine_id.get(unit, database->core_id, engine_id, &end_engine_id));
            break;
        }
        if (engine_id == type_engine_end)
        {
            break;
        }
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.next_engine_id.get(unit, database->core_id, engine_id, &engine_id));
    }

    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.
                    counter_set_size.get(unit, database->core_id, engine_id, &counter_set_size));
    counter_set_offset_end = counter_set_size - 1;
    /** now we have the start and end counter id - need to
     *  convert them to obj stat id resolution */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_stat_id_find
                    (unit, database->core_id, start_engine_id, type_id, start_counter_id, counter_set_offset_start,
                     &boundaries->start));
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_stat_id_find
                    (unit, database->core_id, end_engine_id, type_id, end_counter_id, counter_set_offset_end,
                     &boundaries->end));

exit:
    SHR_FUNC_EXIT;
}
