/**
 * \file src/bcm/dnx/tsn/tsn_counter.c
 * 
 *
 * TSN counter related functionality
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TSN

/*
 * Include files.
 * {
 */

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tsn.h>
#include <sal/core/boot.h>

#include "tsn_dbal.h"

/*
 * }
 */

/*
 * Defines:
 * {
 */

#define DNX_TSN_COUNTER_START_TIME_INCREMENT_USEC (10000)

 /*
  * }
  */

/**
 * \brief - Convert PTP time format to nanoseconds
 *
 *  nanoseconds = (ptp_time->seconds * 1000000000) + ptp_time->nanoseconds
 *
 */
static shr_error_e
dnx_tsn_counter_convert_ptp_time_to_nanoseconds(
    int unit,
    bcm_ptp_timestamp_t * ptp_time,
    uint64 *nanoseconds)
{
    uint64 calc;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_COPY(calc, ptp_time->seconds);
    COMPILER_64_UMUL_32(calc, 1000000000);
    COMPILER_64_ADD_32(calc, ptp_time->nanoseconds);
    COMPILER_64_COPY(*nanoseconds, calc);

    SHR_FUNC_EXIT;
}

/**
 * \brief - Convert nanoseconds into TAS-ToD
 *  TAS-ToD = nanoseconds % 2^48
 */
static shr_error_e
dnx_tsn_counter_convert_nanoseconds_to_tas_tod(
    int unit,
    uint64 nanoseconds,
    uint64 *tas_tod)
{
    uint64 calc;
    uint64 mask;

    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_COPY(calc, nanoseconds);
    COMPILER_64_MASK_CREATE(mask, dnx_data_tsn.time.tas_tod_nof_bits_get(unit), 0);
    COMPILER_64_AND(calc, mask);
    COMPILER_64_COPY(*tas_tod, calc);

    SHR_FUNC_EXIT;
}

/**
 * \brief - Convert nanoseconds into TSN-counter
 *  TSN-counter = (nanoseconds / TICK_PERIOD) % 2^48
 */
static shr_error_e
dnx_tsn_counter_convert_nanoseconds_to_tsn_counter(
    int unit,
    uint64 nanoseconds,
    uint64 *tsn_counter)
{
    uint64 calc;
    uint64 mask;

    uint32 tick_period_ns = dnx_data_tsn.time.tick_period_ns_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_COPY(calc, nanoseconds);
    COMPILER_64_UDIV_32(calc, tick_period_ns);
    COMPILER_64_MASK_CREATE(mask, dnx_data_tsn.time.tsn_counter_nof_bits_get(unit), 0);
    COMPILER_64_AND(calc, mask);
    COMPILER_64_COPY(*tsn_counter, calc);

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_tsn_counter_convert_ptp_time_to_tsn_counter(
    int unit,
    bcm_ptp_timestamp_t * ptp_time,
    uint64 *tsn_counter)
{
    uint64 nanoseconds;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_counter_convert_ptp_time_to_nanoseconds(unit, ptp_time, &nanoseconds));
    SHR_IF_ERR_EXIT(dnx_tsn_counter_convert_nanoseconds_to_tsn_counter(unit, nanoseconds, tsn_counter));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_tsn_counter_init(
    int unit)
{
    uint64 tas_tod_start;
    uint64 tsn_value_start;
    bcm_ptp_timestamp_t ptp_time_start, ptp_time_ref;
    uint64 nanoseconds_start, nanoseconds_ref;

    uint32 tas_tod_source = dnx_data_tsn.time.tas_tod_source_get(unit);
    uint32 tick_period_ns = dnx_data_tsn.time.tick_period_ns_get(unit);
    uint32 tick_period_min_ns = dnx_data_tsn.time.tick_period_min_ns_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    /** Enable TAS-ToD output */
    SHR_IF_ERR_EXIT(dnx_tsn_counter_tas_tod_enable_hw_set(unit, tas_tod_source, 1));

    /** Set tick period and tick period minimum */
    SHR_IF_ERR_EXIT(dnx_tsn_counter_tick_period_hw_set(unit, tick_period_ns, tick_period_min_ns));

    /** Get reference PTP time */
    SHR_IF_ERR_EXIT(dnx_tsn_counter_ptp_time_hw_get(unit, &ptp_time_ref));
    SHR_IF_ERR_EXIT(dnx_tsn_counter_convert_ptp_time_to_nanoseconds(unit, &ptp_time_ref, &nanoseconds_ref));

    /** Get the start PTP time */
    SHR_IF_ERR_EXIT(dnx_tsn_counter_ptp_time_hw_get(unit, &ptp_time_start));
    SHR_IF_ERR_EXIT(dnx_tsn_counter_convert_ptp_time_to_nanoseconds(unit, &ptp_time_start, &nanoseconds_start));

    if (!(SAL_BOOT_PLISIM || soc_sand_is_emulation_system(unit)))
    {
        /** Check PTP time is running */
        if (COMPILER_64_LE(nanoseconds_start, nanoseconds_ref))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "PTP time is not running.\n");
        }
    }

    /** Add 10msec to the current PTP time to compensate the delay for setting it to HW  */
    COMPILER_64_ADD_32(nanoseconds_start, DNX_TSN_COUNTER_START_TIME_INCREMENT_USEC * 1000);

    /** Round the current PTP time to tick-period */
    COMPILER_64_UDIV_32(nanoseconds_start, tick_period_ns);
    COMPILER_64_UMUL_32(nanoseconds_start, tick_period_ns);

    /** Convert start time to TAS-ToD */
    SHR_IF_ERR_EXIT(dnx_tsn_counter_convert_nanoseconds_to_tas_tod(unit, nanoseconds_start, &tas_tod_start));

    /** Convert start time to tsn counter */
    SHR_IF_ERR_EXIT(dnx_tsn_counter_convert_nanoseconds_to_tsn_counter(unit, nanoseconds_start, &tsn_value_start));

    /** Trigger TSN counter start */
    SHR_IF_ERR_EXIT(dnx_tsn_counter_start_attributes_hw_set(unit, tas_tod_start, tsn_value_start, 1));

    if (!soc_sand_is_emulation_system(unit))
    {
        /** Polling TSN counter has started */
        SHR_IF_ERR_EXIT(dnx_tsn_counter_start_trigger_down_hw_poll
                        (unit, DNX_TSN_COUNTER_START_TIME_INCREMENT_USEC, 10000));
    }

    /** Enable TSN counter output */
    SHR_IF_ERR_EXIT(dnx_tsn_counter_enable_hw_set(unit, 1));

exit:
    SHR_FUNC_EXIT;
}
