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
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tsn.h>
#include <sal/core/boot.h>
#include <sal/core/time.h>
#include <shared/utilex/utilex_ptp_time.h>
#include <shared/utilex/utilex_u64.h>

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

    SHR_IF_ERR_EXIT(utilex_ptp_time_to_nanoseconds_convert(unit, ptp_time, &nanoseconds));
    SHR_IF_ERR_EXIT(dnx_tsn_counter_convert_nanoseconds_to_tsn_counter(unit, nanoseconds, tsn_counter));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_tsn_counter_nanoseconds_to_ticks_convert(
    int unit,
    uint64 nanoseconds,
    uint64 *ticks)
{
    uint64 calc;
    uint32 tick_period_ns = dnx_data_tsn.time.tick_period_ns_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_COPY(calc, nanoseconds);
    COMPILER_64_UDIV_64(calc, tick_period_ns);
    COMPILER_64_COPY(*ticks, calc);

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_tsn_counter_curr_time_get(
    int unit,
    uint64 *curr_time)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_counter_curr_time_hw_get(unit, curr_time));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_tsn_counter_ptp_curr_time_get(
    int unit,
    bcm_ptp_timestamp_t * ptp_curr_time)
{
    uint64 curr_time;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_counter_curr_time_get(unit, &curr_time));
    SHR_IF_ERR_EXIT(utilex_ptp_time_from_nanoseconds_convert(unit, curr_time, ptp_curr_time));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_tsn_counter_config_change_time_get(
    int unit,
    bcm_ptp_timestamp_t * ptp_base_time,
    uint64 cycle_time,
    uint32 time_margin,
    bcm_ptp_timestamp_t * actual_change_time)
{
    uint64 curr_time_nsec, base_time_nsec;
    uint64 diff, n;

    SHR_FUNC_INIT_VARS(unit);

    /** translate base PTP time into nanosecs */
    SHR_IF_ERR_EXIT(utilex_ptp_time_to_nanoseconds_convert(unit, ptp_base_time, &base_time_nsec));

    /** get the current time */
    SHR_IF_ERR_EXIT(dnx_tsn_counter_curr_time_get(unit, &curr_time_nsec));

    /** add time margin */
    COMPILER_64_ADD_32(curr_time_nsec, time_margin);

    if (COMPILER_64_LT(base_time_nsec, curr_time_nsec))
    {
        /*
         *  base time is in the past. base time should be updated to a future time.
         *  updated base time => original base time + N * cycle time >= current time (N is an integer)
         *
         */
        /** calculate the gap between current time and base time */
        COMPILER_64_COPY(diff, curr_time_nsec);
        COMPILER_64_SUB_64(diff, base_time_nsec);

        /** divide and round up gap by cycle times (calc is N, the number of cycle times to add) */
        utilex_u64_div_and_round_up(diff, cycle_time, &n);

        if (COMPILER_64_HI(n) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Cannot update PTP time to future, as number of cycle times is too large\n");
        }

        /** multiply cycle time by N and add to original base time  */
        COMPILER_64_COPY(diff, cycle_time);
        COMPILER_64_UMUL_32(diff, COMPILER_64_LO(n));
        COMPILER_64_ADD_64(base_time_nsec, diff);
    }

    /** convert to PTP format */
    SHR_IF_ERR_EXIT(utilex_ptp_time_from_nanoseconds_convert(unit, base_time_nsec, actual_change_time));

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
    uint64 nanoseconds_start, nanoseconds_ref;

    uint32 tas_tod_source = dnx_data_tsn.time.tas_tod_source_get(unit);
    uint32 tick_period_ns = dnx_data_tsn.time.tick_period_ns_get(unit);
    uint32 tick_period_min_ns = dnx_data_tsn.time.tick_period_min_ns_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    /** Enable TAS-ToD output */
    SHR_IF_ERR_EXIT(dnx_tsn_counter_tas_tod_enable_hw_set(unit, tas_tod_source, 1));

    /** Set tick period and tick period minimum */
    SHR_IF_ERR_EXIT(dnx_tsn_counter_tick_period_hw_set(unit, tick_period_ns, tick_period_min_ns));

    /** Get reference time */
    SHR_IF_ERR_EXIT(dnx_tsn_counter_curr_time_hw_get(unit, &nanoseconds_ref));

    /** Get the start time */
    SHR_IF_ERR_EXIT(dnx_tsn_counter_curr_time_hw_get(unit, &nanoseconds_start));

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
