
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TSN

#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm/types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tsn.h>
#include <sal/core/boot.h>
#include <sal/core/time.h>
#include <shared/utilex/utilex_ptp_time.h>
#include <shared/utilex/utilex_u64.h>

#include "tsn_dbal.h"

#define DNX_TSN_COUNTER_START_TIME_INCREMENT_USEC (5000)

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

shr_error_e
dnx_tsn_counter_convert_nanoseconds_from_tas_tod(
    int unit,
    uint64 tas_tod,
    uint64 ref_nanoseconds,
    uint64 *out_nanoseconds)
{
    uint64 calc;
    uint64 ref_tas_tod;

    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_ZERO(*out_nanoseconds);

    COMPILER_64_OR(*out_nanoseconds, tas_tod);

    COMPILER_64_COPY(calc, ref_nanoseconds);
    COMPILER_64_SHR(calc, dnx_data_tsn.time.tas_tod_nof_bits_get(unit));

    SHR_IF_ERR_EXIT(dnx_tsn_counter_convert_nanoseconds_to_tas_tod(unit, ref_nanoseconds, &ref_tas_tod));

    if (COMPILER_64_GT(tas_tod, ref_tas_tod))
    {

        COMPILER_64_SUB_32(calc, 1);
    }

    COMPILER_64_SHL(calc, dnx_data_tsn.time.tas_tod_nof_bits_get(unit));
    COMPILER_64_OR(*out_nanoseconds, calc);

exit:
    SHR_FUNC_EXIT;
}

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

    SHR_IF_ERR_EXIT(utilex_ptp_time_to_nanoseconds_convert(unit, ptp_base_time, &base_time_nsec));

    SHR_IF_ERR_EXIT(dnx_tsn_counter_curr_time_get(unit, &curr_time_nsec));

    COMPILER_64_ADD_32(curr_time_nsec, time_margin);

    if (COMPILER_64_LT(base_time_nsec, curr_time_nsec))
    {

        COMPILER_64_COPY(diff, curr_time_nsec);
        COMPILER_64_SUB_64(diff, base_time_nsec);

        utilex_u64_div_and_round_up(diff, cycle_time, &n);

        if (COMPILER_64_HI(n) == 0)
        {

            COMPILER_64_COPY(diff, cycle_time);
            COMPILER_64_UMUL_32(diff, COMPILER_64_LO(n));
        }
        else if (COMPILER_64_HI(cycle_time) == 0)
        {

            COMPILER_64_COPY(diff, n);
            COMPILER_64_UMUL_32(diff, COMPILER_64_LO(cycle_time));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "cycle_time * N is larger than 64-bit\n");
        }

        COMPILER_64_ADD_64(base_time_nsec, diff);
    }

    SHR_IF_ERR_EXIT(utilex_ptp_time_from_nanoseconds_convert(unit, base_time_nsec, actual_change_time));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_counter_start(
    int unit,
    uint64 nanoseconds_start)
{
    uint64 tas_tod_start;
    uint64 tsn_value_start;

    uint32 tick_period_ns = dnx_data_tsn.time.tick_period_ns_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_counter_enable_hw_set(unit, 0));

    COMPILER_64_ADD_32(nanoseconds_start, DNX_TSN_COUNTER_START_TIME_INCREMENT_USEC * 1000);

    COMPILER_64_UDIV_32(nanoseconds_start, tick_period_ns);
    COMPILER_64_UMUL_32(nanoseconds_start, tick_period_ns);

    SHR_IF_ERR_EXIT(dnx_tsn_counter_convert_nanoseconds_to_tas_tod(unit, nanoseconds_start, &tas_tod_start));

    SHR_IF_ERR_EXIT(dnx_tsn_counter_convert_nanoseconds_to_tsn_counter(unit, nanoseconds_start, &tsn_value_start));

    SHR_IF_ERR_EXIT(dnx_tsn_counter_start_attributes_hw_set(unit, tas_tod_start, tsn_value_start, 1));

    SHR_IF_ERR_EXIT(dnx_tsn_counter_enable_hw_set(unit, 1));

    if (!soc_sand_is_emulation_system(unit))
    {

        SHR_IF_ERR_EXIT(dnx_tsn_counter_start_trigger_down_hw_poll
                        (unit, DNX_TSN_COUNTER_START_TIME_INCREMENT_USEC, 10000));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tsn_counter_init(
    int unit)
{
    uint64 nanoseconds_start, nanoseconds_ref;

    uint32 tick_period_ns = dnx_data_tsn.time.tick_period_ns_get(unit);
    uint32 tick_period_min_ns = dnx_data_tsn.time.tick_period_min_ns_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_counter_tick_period_hw_set(unit, tick_period_ns, tick_period_min_ns));

    SHR_IF_ERR_EXIT(dnx_tsn_counter_curr_time_hw_get(unit, &nanoseconds_ref));

    SHR_IF_ERR_EXIT(dnx_tsn_counter_curr_time_hw_get(unit, &nanoseconds_start));

    if (!(SAL_BOOT_PLISIM || soc_sand_is_emulation_system(unit)))
    {

        if (COMPILER_64_LE(nanoseconds_start, nanoseconds_ref))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "PTP time is not running.\n");
        }
    }

    SHR_IF_ERR_EXIT(dnx_tsn_counter_start(unit, nanoseconds_start));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tsn_counter_restart(
    int unit)
{
    uint64 nanoseconds_start;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_counter_curr_time_hw_get(unit, &nanoseconds_start));

    SHR_IF_ERR_EXIT(dnx_tsn_counter_start(unit, nanoseconds_start));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tsn_counter_restart_required_get(
    int unit,
    int *is_restart_required)
{
    uint64 curr_tsn_counter, curr_tas_tod;
    uint64 ptp_nanoseconds, ref_nanoseconds;
    uint64 ptp_tsn_counter;
    uint64 diff, max_diff;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_counter_synced_counters_value_hw_get
                    (unit, &curr_tsn_counter, &curr_tas_tod, &ref_nanoseconds));

    if (!dnx_data_tsn.time.feature_get(unit, dnx_data_tsn_time_counters_atomic_read))
    {

        SHR_IF_ERR_EXIT(dnx_tsn_counter_convert_nanoseconds_from_tas_tod
                        (unit, curr_tas_tod, ref_nanoseconds, &ptp_nanoseconds));
    }
    else
    {
        COMPILER_64_COPY(ptp_nanoseconds, ref_nanoseconds);
    }

    SHR_IF_ERR_EXIT(dnx_tsn_counter_convert_nanoseconds_to_tsn_counter(unit, ptp_nanoseconds, &ptp_tsn_counter));

    if (COMPILER_64_GE(ptp_tsn_counter, curr_tsn_counter))
    {
        COMPILER_64_COPY(diff, ptp_tsn_counter);
        COMPILER_64_SUB_64(diff, curr_tsn_counter);
    }
    else
    {
        COMPILER_64_COPY(diff, curr_tsn_counter);
        COMPILER_64_SUB_64(diff, ptp_tsn_counter);
    }

    COMPILER_64_SET(max_diff, 0, dnx_data_tsn.time.tsn_counter_max_diff_get(unit));

    if (COMPILER_64_LT(diff, max_diff))
    {
        *is_restart_required = FALSE;
    }
    else
    {
        *is_restart_required = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}
