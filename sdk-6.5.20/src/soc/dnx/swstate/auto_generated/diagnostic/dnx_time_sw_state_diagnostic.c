
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnxc/swstate/dnxc_sw_state_c_includes.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_time_sw_state_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_time_interface_sw_state_t * dnx_time_interface_db_data[SOC_MAX_NUM_DEVICES];



int
dnx_time_interface_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_time_interface_db_flags_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_time_interface_db_id_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_time_interface_db_heartbeat_hz_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_time_interface_db_clk_resolution_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_time_interface_db_bitclock_hz_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_time_interface_db_status_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_time_interface_db_flags_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_time_sw_state flags") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_time_sw_state flags\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_time_interface_db/flags.txt",
            "dnx_time_interface_db[%d]->","((dnx_time_interface_sw_state_t*)sw_state_roots_array[%d][DNX_TIME_SW_STATE_MODULE_ID])->","flags: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_time_interface_sw_state_t*)sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_time_interface_db[%d]->","((dnx_time_interface_sw_state_t*)sw_state_roots_array[%d][DNX_TIME_SW_STATE_MODULE_ID])->","flags: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint32(
            unit,
            &((dnx_time_interface_sw_state_t*)sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID])->flags);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_time_interface_db_id_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_time_sw_state id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_time_sw_state id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_time_interface_db/id.txt",
            "dnx_time_interface_db[%d]->","((dnx_time_interface_sw_state_t*)sw_state_roots_array[%d][DNX_TIME_SW_STATE_MODULE_ID])->","id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_time_interface_sw_state_t*)sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_time_interface_db[%d]->","((dnx_time_interface_sw_state_t*)sw_state_roots_array[%d][DNX_TIME_SW_STATE_MODULE_ID])->","id: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnx_time_interface_sw_state_t*)sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID])->id);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_time_interface_db_heartbeat_hz_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_time_sw_state heartbeat_hz") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_time_sw_state heartbeat_hz\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_time_interface_db/heartbeat_hz.txt",
            "dnx_time_interface_db[%d]->","((dnx_time_interface_sw_state_t*)sw_state_roots_array[%d][DNX_TIME_SW_STATE_MODULE_ID])->","heartbeat_hz: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_time_interface_sw_state_t*)sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_time_interface_db[%d]->","((dnx_time_interface_sw_state_t*)sw_state_roots_array[%d][DNX_TIME_SW_STATE_MODULE_ID])->","heartbeat_hz: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnx_time_interface_sw_state_t*)sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID])->heartbeat_hz);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_time_interface_db_clk_resolution_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_time_sw_state clk_resolution") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_time_sw_state clk_resolution\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_time_interface_db/clk_resolution.txt",
            "dnx_time_interface_db[%d]->","((dnx_time_interface_sw_state_t*)sw_state_roots_array[%d][DNX_TIME_SW_STATE_MODULE_ID])->","clk_resolution: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_time_interface_sw_state_t*)sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_time_interface_db[%d]->","((dnx_time_interface_sw_state_t*)sw_state_roots_array[%d][DNX_TIME_SW_STATE_MODULE_ID])->","clk_resolution: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnx_time_interface_sw_state_t*)sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID])->clk_resolution);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_time_interface_db_bitclock_hz_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_time_sw_state bitclock_hz") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_time_sw_state bitclock_hz\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_time_interface_db/bitclock_hz.txt",
            "dnx_time_interface_db[%d]->","((dnx_time_interface_sw_state_t*)sw_state_roots_array[%d][DNX_TIME_SW_STATE_MODULE_ID])->","bitclock_hz: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_time_interface_sw_state_t*)sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_time_interface_db[%d]->","((dnx_time_interface_sw_state_t*)sw_state_roots_array[%d][DNX_TIME_SW_STATE_MODULE_ID])->","bitclock_hz: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnx_time_interface_sw_state_t*)sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID])->bitclock_hz);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_time_interface_db_status_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_time_sw_state status") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_time_sw_state status\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_time_interface_db/status.txt",
            "dnx_time_interface_db[%d]->","((dnx_time_interface_sw_state_t*)sw_state_roots_array[%d][DNX_TIME_SW_STATE_MODULE_ID])->","status: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_time_interface_sw_state_t*)sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_time_interface_db[%d]->","((dnx_time_interface_sw_state_t*)sw_state_roots_array[%d][DNX_TIME_SW_STATE_MODULE_ID])->","status: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnx_time_interface_sw_state_t*)sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID])->status);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnx_time_interface_db_info[SOC_MAX_NUM_DEVICES][DNX_TIME_INTERFACE_DB_INFO_NOF_ENTRIES];
const char* dnx_time_interface_db_layout_str[DNX_TIME_INTERFACE_DB_INFO_NOF_ENTRIES] = {
    "DNX_TIME_INTERFACE_DB~",
    "DNX_TIME_INTERFACE_DB~FLAGS~",
    "DNX_TIME_INTERFACE_DB~ID~",
    "DNX_TIME_INTERFACE_DB~HEARTBEAT_HZ~",
    "DNX_TIME_INTERFACE_DB~CLK_RESOLUTION~",
    "DNX_TIME_INTERFACE_DB~BITCLOCK_HZ~",
    "DNX_TIME_INTERFACE_DB~STATUS~",
};
#endif 
#undef BSL_LOG_MODULE
