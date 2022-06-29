
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_UTILS

#include <soc/drv.h>
#include <soc/dnxc/dnxc_time.h>

#include <shared/utilex/utilex_integer_arithmetic.h>

#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#endif
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#endif

shr_error_e
dnxc_time_units_convert(
    int unit,
    dnxc_time_t * time_src,
    dnxc_time_unit_e required_time_units,
    dnxc_time_t * time_dest)
{

    int time_units_power[] = { 0, 3, 6, 9 };
    uint32 abs_power_delta;
    uint64 multiplier;
    int power_delta;
    SHR_FUNC_INIT_VARS(unit);

    if (time_src->time_units < 0 || (time_src->time_units >= sizeof(time_units_power) / sizeof(int)))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "unsupported time units %d", time_src->time_units);
    }
    if (required_time_units < 0 || (required_time_units >= sizeof(time_units_power) / sizeof(int)))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "unsupported time units %d", required_time_units);
    }

    power_delta = time_units_power[required_time_units] - time_units_power[time_src->time_units];
    abs_power_delta = utilex_abs(power_delta);

    COMPILER_64_UPOW(multiplier, 10, abs_power_delta);

    if (power_delta >= 0)
    {
        uint64 work_reg_64_a;
        uint64 work_reg_64_b;

        COMPILER_64_COPY(time_dest->time, time_src->time);

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

shr_error_e
dnxc_time_clock_cycles_to_time_common_get(
    int unit,
    uint32 nof_clock_cycles,
    uint32 clock_freq_khz,
    dnxc_time_unit_e required_time_unit,
    dnxc_time_t * time)
{
    dnxc_time_t time_nano;
    SHR_FUNC_INIT_VARS(unit);

    time_nano.time_units = DNXC_TIME_UNIT_NSEC;
    COMPILER_64_SET(time_nano.time, 0, nof_clock_cycles);
    COMPILER_64_UMUL_32(time_nano.time, 1000000);
    COMPILER_64_UDIV_32(time_nano.time, clock_freq_khz);

    SHR_IF_ERR_EXIT(dnxc_time_units_convert(unit, &time_nano, required_time_unit, time));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnxc_time_time_to_clock_cycles_common_get(
    int unit,
    dnxc_time_t * time,
    uint32 clock_freq_khz,
    uint32 *nof_clock_cycles)
{
    uint64 long_nof_clock_cycles;
    dnxc_time_t time_nano;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_time_units_convert(unit, time, DNXC_TIME_UNIT_NSEC, &time_nano));

    COMPILER_64_COPY(long_nof_clock_cycles, time_nano.time);
    COMPILER_64_UMUL_32(long_nof_clock_cycles, clock_freq_khz);
    COMPILER_64_UDIV_32(long_nof_clock_cycles, 1000000);

    COMPILER_64_TO_32_LO(*nof_clock_cycles, long_nof_clock_cycles);
    if (COMPILER_64_HI(long_nof_clock_cycles) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Over flow when converting time to clock cycles");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnxc_time_clock_cycles_to_time_get(
    int unit,
    uint32 nof_clock_cycles,
    dnxc_time_unit_e required_time_unit,
    dnxc_time_t * time)
{
    uint32 core_clock_khz = 1;

    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        core_clock_khz = dnx_data_device.general.core_clock_khz_get(unit);
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        core_clock_khz = dnxf_data_device.general.core_clock_khz_get(unit);
    }
#endif

    SHR_IF_ERR_EXIT(dnxc_time_clock_cycles_to_time_common_get
                    (unit, nof_clock_cycles, core_clock_khz, required_time_unit, time));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnxc_time_system_ref_clock_cycles_to_time_get(
    int unit,
    uint32 nof_clock_cycles,
    dnxc_time_unit_e required_time_unit,
    dnxc_time_t * time)
{
    uint32 system_ref_core_clock_khz = 1;

    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        system_ref_core_clock_khz = dnx_data_device.general.system_ref_core_clock_khz_get(unit);
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        system_ref_core_clock_khz = dnxf_data_device.general.system_ref_core_clock_khz_get(unit);
    }
#endif

    SHR_IF_ERR_EXIT(dnxc_time_clock_cycles_to_time_common_get
                    (unit, nof_clock_cycles, system_ref_core_clock_khz, required_time_unit, time));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnxc_time_time_to_clock_cycles_get(
    int unit,
    dnxc_time_t * time,
    uint32 *nof_clock_cycles)
{
    uint32 core_clock_khz = 1;

    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        core_clock_khz = dnx_data_device.general.core_clock_khz_get(unit);
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        core_clock_khz = dnxf_data_device.general.core_clock_khz_get(unit);
    }
#endif

    SHR_IF_ERR_EXIT(dnxc_time_time_to_clock_cycles_common_get(unit, time, core_clock_khz, nof_clock_cycles));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnxc_time_time_to_system_ref_clock_cycles_get(
    int unit,
    dnxc_time_t * time,
    uint32 *nof_clock_cycles)
{
    uint32 system_ref_core_clock_khz = 1;

    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        system_ref_core_clock_khz = dnx_data_device.general.system_ref_core_clock_khz_get(unit);
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        system_ref_core_clock_khz = dnxf_data_device.general.system_ref_core_clock_khz_get(unit);
    }
#endif

    SHR_IF_ERR_EXIT(dnxc_time_time_to_clock_cycles_common_get(unit, time, system_ref_core_clock_khz, nof_clock_cycles));

exit:
    SHR_FUNC_EXIT;
}
