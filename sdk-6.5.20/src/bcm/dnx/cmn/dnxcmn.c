/** \file dnxcmn.c
 *  
 *  Common utils for dnx only.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COMMON

/*
 * Include files.
 * {
 */

#include <bcm/types.h>

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_integer_arithmetic.h>

#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/util.h>
#include <soc/dnx/dbal/dbal.h>
#include <sal/core/boot.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>

/*
 * }
 */

/*
 * DBAL utils
 * {
 */
/**
 * See egq_dbal.h
 */
shr_error_e
dnx_dbal_gen_get(
    uint32 unit,
    dbal_tables_e dbal_table_id,
    uint32 num_keys,
    uint32 num_fields,
    ...)
{
    uint32 entry_handle_id;
    dbal_fields_e dbal_field_id;
    uint32 dbal_value, key_index, field_index;
    uint32 *dbal_value_p;
    uint64 *dbal_value64_p;
    int inst_id;
    dnx_gen_dbal_field_e field_type;
    va_list ap;

    SHR_FUNC_INIT_VARS(unit);
    va_start(ap, num_fields);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    /*
     * key construction.
     */
    for (key_index = 0; key_index < num_keys; key_index++)
    {
        field_type = va_arg(ap, uint32);
        dbal_field_id = va_arg(ap, uint32);
        switch (field_type)
        {
            case GEN_DBAL_FIELD32:
            {
                dbal_value = va_arg(ap, uint32);
                dbal_entry_key_field32_set(unit, entry_handle_id, dbal_field_id, dbal_value);
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Currently, this field type (%d) is not supported for keys. See dnx_gen_dbal_field_e.\r\n",
                             field_type);
            }
        }
    }
    /*
     * This is a 'get' operation. Each field identifier is followed by a pointer.
     */
    for (field_index = 0; field_index < num_fields; field_index++)
    {
        field_type = va_arg(ap, uint32);
        dbal_field_id = va_arg(ap, uint32);
        inst_id = va_arg(ap, uint32);
        if ((inst_id != INST_SINGLE) && (inst_id != INST_ALL) && (inst_id < 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "Input INSTANCE (%d) is illegal. It is negative and neither INST_SINGLE (%d) nor INST_ALL (%d). Quit.\r\n",
                         inst_id, INST_SINGLE, INST_ALL);
        }
        switch (field_type)
        {
            case GEN_DBAL_FIELD32:
            {
                dbal_value_p = va_arg(ap, uint32 *);
                /*
                 * Always initialize to zero.
                 */
                *dbal_value_p = 0;
                dbal_value_field32_request(unit, entry_handle_id, dbal_field_id, inst_id, dbal_value_p);
                break;
            }
            case GEN_DBAL_FIELD64:
            {
                dbal_value64_p = va_arg(ap, uint64 *);
                /*
                 * Always initialize to zero.
                 */
                COMPILER_64_ZERO(*dbal_value64_p);
                dbal_value_field64_request(unit, entry_handle_id, dbal_field_id, inst_id, dbal_value64_p);
                break;
            }
            case GEN_DBAL_ARR32:
            {
                dbal_value_p = va_arg(ap, uint32 *);
                dbal_value_field_arr32_request(unit, entry_handle_id, dbal_field_id, inst_id, dbal_value_p);
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Currently, this field type (%d) is not supported for result-fields. See dnx_gen_dbal_field_e.\r\n",
                             field_type);
            }
        }
    }
    /*
     * Last field MUST be GEN_DBAL_FIELD_LAST_MARK
     */
    field_type = va_arg(ap, uint32);
    if (field_type != GEN_DBAL_FIELD_LAST_MARK)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "\r\n"
                     "Input last field (%d) is illegal. It must be GEN_DBAL_FIELD_LAST_MARK (%d). Quit.\r\n",
                     field_type, GEN_DBAL_FIELD_LAST_MARK);
    }
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    va_end(ap);
    SHR_FUNC_EXIT;
}
/**
 * See egq_dbal.h
 */
shr_error_e
dnx_dbal_gen_set(
    uint32 unit,
    dbal_tables_e dbal_table_id,
    uint32 num_keys,
    uint32 num_fields,
    ...)
{
    uint32 entry_handle_id;
    dbal_fields_e dbal_field_id;
    uint32 dbal_value, key_index, field_index;
    uint32 dbal_range_from_value, dbal_range_to_value;
    int inst_id;
    uint32 *dbal_value_p;
    dnx_gen_dbal_field_e field_type;
    va_list ap;

    SHR_FUNC_INIT_VARS(unit);
    va_start(ap, num_fields);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    /*
     * key construction.
     */
    for (key_index = 0; key_index < num_keys; key_index++)
    {
        field_type = va_arg(ap, uint32);
        dbal_field_id = va_arg(ap, uint32);
        switch (field_type)
        {
            case GEN_DBAL_FIELD32:
            {
                dbal_value = va_arg(ap, uint32);
                dbal_entry_key_field32_set(unit, entry_handle_id, dbal_field_id, dbal_value);
                break;
            }
            case GEN_DBAL_RANGE32:
            {
                dbal_range_from_value = va_arg(ap, uint32);
                dbal_range_to_value = va_arg(ap, uint32);
                dbal_entry_key_field32_range_set(unit, entry_handle_id, dbal_field_id, dbal_range_from_value,
                                                 dbal_range_to_value);
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Currently, this field type (%d) is not supported for keys. See dnx_gen_dbal_field_e.\r\n",
                             field_type);
            }
        }
    }
    for (field_index = 0; field_index < num_fields; field_index++)
    {
        field_type = va_arg(ap, uint32);
        dbal_field_id = va_arg(ap, uint32);
        inst_id = va_arg(ap, uint32);
        if ((inst_id != INST_SINGLE) && (inst_id != INST_ALL) && (inst_id < 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "Input INSTANCE (%d) is illegal. It is negative and neither INST_SINGLE (%d) nor INST_ALL (%d). Quit.\r\n",
                         inst_id, INST_SINGLE, INST_ALL);
        }
        switch (field_type)
        {
            case GEN_DBAL_FIELD32:
            {
                dbal_value = va_arg(ap, uint32);
                dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, inst_id, dbal_value);
                break;
            }
            case GEN_DBAL_ARR32:
            {
                dbal_value_p = va_arg(ap, uint32 *);
                dbal_entry_value_field_arr32_set(unit, entry_handle_id, dbal_field_id, inst_id, dbal_value_p);
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Currently, this field type (%d) is not supported for result-fields. See dnx_gen_dbal_field_e.\r\n",
                             field_type);
            }
        }
    }
    /*
     * Last field MUST be GEN_DBAL_FIELD_LAST_MARK
     */
    field_type = va_arg(ap, uint32);
    if (field_type != GEN_DBAL_FIELD_LAST_MARK)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "\r\n"
                     "Input last field (%d) is illegal. It must be GEN_DBAL_FIELD_LAST_MARK (%d). Quit.\r\n",
                     field_type, GEN_DBAL_FIELD_LAST_MARK);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    va_end(ap);
    SHR_FUNC_EXIT;
}

/*
 * }
 */

/*
 * Time
 * {
 */

/*
 * See .h file
 */
shr_error_e
dnxcmn_time_units_convert(
    int unit,
    dnxcmn_time_t * time_src,
    dnxcmn_time_unit_e required_time_units,
    dnxcmn_time_t * time_dest)
{
    /*
     * sec milisec usec nsec
     */
    int time_units_power[] = { 0, 3, 6, 9 };
    uint32 abs_power_delta;
    uint64 multiplier;
    int power_delta;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that time units included in time_units_power
     */
    if (time_src->time_units < 0 || (time_src->time_units >= sizeof(time_units_power) / sizeof(int)))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "unsupported time units %d", time_src->time_units);
    }
    if (required_time_units < 0 || (required_time_units >= sizeof(time_units_power) / sizeof(int)))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "unsupported time units %d", required_time_units);
    }

    /*
     * Get the required multiplier
     */
    power_delta = time_units_power[required_time_units] - time_units_power[time_src->time_units];
    abs_power_delta = utilex_abs(power_delta);

    COMPILER_64_UPOW(multiplier, 10, abs_power_delta);
    /*
     * Convert
     */
    if (power_delta >= 0)
    {
        uint64 work_reg_64_a;
        uint64 work_reg_64_b;

        COMPILER_64_COPY(time_dest->time, time_src->time);
        /*
         * Implement
         *   COMPILER_64_UMUL_64(time_dest->time, multiplier);
         */
        COMPILER_64_COPY(work_reg_64_a, time_dest->time);
        COMPILER_64_COPY(work_reg_64_b, time_dest->time);
        COMPILER_64_UMUL_32(work_reg_64_b, COMPILER_64_HI(multiplier));
        COMPILER_64_SHL(work_reg_64_b, 32);
        COMPILER_64_UMUL_32(work_reg_64_a, COMPILER_64_LO(multiplier));
        COMPILER_64_ADD_64(work_reg_64_a, work_reg_64_b);
        COMPILER_64_COPY(time_dest->time, work_reg_64_a);
    }
    else
    {
        COMPILER_64_COPY(time_dest->time, time_src->time);
        COMPILER_64_UDIV_64(time_dest->time, multiplier);
    }
    time_dest->time_units = required_time_units;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Convert clock cycles to time (according to specified time units
 *   and specific clock frequency).
 */
static shr_error_e
dnxcmn_clock_cycles_to_time_common_get(
    int unit,
    uint32 nof_clock_cycles,
    uint32 clock_freq,
    dnxcmn_time_unit_e required_time_unit,
    dnxcmn_time_t * time)
{
    dnxcmn_time_t time_nano;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * First convert clock cycles to nano sec 
     * time [NSEC] = 1000000000 * ((nof_clock_cycles) / (clock_freq * 1000)) / 
     */
    time_nano.time_units = DNXCMN_TIME_UNIT_NSEC;
    COMPILER_64_SET(time_nano.time, 0, nof_clock_cycles);
    COMPILER_64_UMUL_32(time_nano.time, 1000000);
    COMPILER_64_UDIV_32(time_nano.time, clock_freq);

    /*
     * Convert to time according to requested units
     */
    SHR_IF_ERR_EXIT(dnxcmn_time_units_convert(unit, &time_nano, required_time_unit, time));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnxcmn_clock_cycles_to_time_get(
    int unit,
    uint32 nof_clock_cycles,
    dnxcmn_time_unit_e required_time_unit,
    dnxcmn_time_t * time)
{
    uint32 core_clock_khz;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get core clock freq
     */
    core_clock_khz = dnx_data_device.general.core_clock_khz_get(unit);

    SHR_IF_ERR_EXIT(dnxcmn_clock_cycles_to_time_common_get
                    (unit, nof_clock_cycles, core_clock_khz, required_time_unit, time));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnxcmn_system_ref_clock_cycles_to_time_get(
    int unit,
    uint32 nof_clock_cycles,
    dnxcmn_time_unit_e required_time_unit,
    dnxcmn_time_t * time)
{
    uint32 system_ref_core_clock_khz;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get system ref core clock freq
     */
    system_ref_core_clock_khz = dnx_data_device.general.system_ref_core_clock_khz_get(unit);

    SHR_IF_ERR_EXIT(dnxcmn_clock_cycles_to_time_common_get
                    (unit, nof_clock_cycles, system_ref_core_clock_khz, required_time_unit, time));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Convert time to clock cycles (according to specified time units
 *   and specific clock frequency).
 */
static shr_error_e
dnxcmn_time_to_clock_cycles_common_get(
    int unit,
    dnxcmn_time_t * time,
    uint32 clock_freq,
    uint32 *nof_clock_cycles)
{
    uint64 long_nof_clock_cycles;
    dnxcmn_time_t time_nano;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Convert time to nano sec
     */
    SHR_IF_ERR_EXIT(dnxcmn_time_units_convert(unit, time, DNXCMN_TIME_UNIT_NSEC, &time_nano));

    /*
     * Get nof of clock cycles 
     * nof_clock_cycles = (core_clock [KHZ] * 1000) * (time [NSEC] / 1000000000)
     */
    COMPILER_64_COPY(long_nof_clock_cycles, time_nano.time);
    COMPILER_64_UMUL_32(long_nof_clock_cycles, clock_freq);
    COMPILER_64_UDIV_32(long_nof_clock_cycles, 1000000);

    /*
     * Extract nof long cycles
     */
    COMPILER_64_TO_32_LO(*nof_clock_cycles, long_nof_clock_cycles);
    if (COMPILER_64_HI(long_nof_clock_cycles) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Over flow when converting time to clock cycles");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnxcmn_time_to_clock_cycles_get(
    int unit,
    dnxcmn_time_t * time,
    uint32 *nof_clock_cycles)
{
    uint32 core_clock_khz;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get core clock freq
     */
    core_clock_khz = dnx_data_device.general.core_clock_khz_get(unit);

    SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_common_get(unit, time, core_clock_khz, nof_clock_cycles));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnxcmn_time_to_system_ref_clock_cycles_get(
    int unit,
    dnxcmn_time_t * time,
    uint32 *nof_clock_cycles)
{
    uint32 system_ref_core_clock_khz;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get system ref core clock freq
     */
    system_ref_core_clock_khz = dnx_data_device.general.system_ref_core_clock_khz_get(unit);

    SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_common_get(unit, time, system_ref_core_clock_khz, nof_clock_cycles));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnxcmn_polling(
    int unit,
    sal_usecs_t time_out,
    int32 min_polls,
    uint32 entry_handle_id,
    dbal_fields_e field,
    uint32 expected_value)
{
    soc_timeout_t to;
    uint32 result = 0;

    SHR_FUNC_INIT_VARS(unit);

#ifdef JR2_CRASH_RECOVERY_SUPPORT
    /*
     * Implement crash recovery support here
     */
#endif /* JR2_CRASH_RECOVERY_SUPPORT */

    /*
     * Get field value
     */
    dbal_value_field32_request(unit, entry_handle_id, field, INST_SINGLE, &result);

    soc_timeout_init(&to, time_out, min_polls);
    for (;;)
    {
        /*
         * Get entry
         */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

#ifdef PLISIM
        if (SAL_BOOT_PLISIM)
        {
            result = expected_value;
        }
#endif
        if (result == expected_value)
        {
            break;
        }
        if (soc_timeout_check(&to))
        {
            LOG_ERROR(BSL_LS_SOC_REG,
                      (BSL_META("TIMEOUT when polling field: %s \n"), dbal_field_to_string(unit, field)));
            SHR_ERR_EXIT(_SHR_E_TIMEOUT, "_SHR_E_TIMEOUT");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
