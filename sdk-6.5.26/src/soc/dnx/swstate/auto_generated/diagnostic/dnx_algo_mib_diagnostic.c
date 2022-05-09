
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_algo_mib_diagnostic.h>
#include <bcm_int/dnx/algo/mib/algo_mib.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_algo_mib_layout.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_algo_mib_db_t * dnx_algo_mib_db_data[SOC_MAX_NUM_DEVICES];



int
dnx_algo_mib_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_algo_mib_db_mib_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_algo_mib_db_mib_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_algo_mib_db_mib_counter_pbmp_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_algo_mib_db_mib_interval_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_algo_mib_db_mib_counter_pbmp_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "bcm_pbmp_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_algo_mib mib counter_pbmp") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_algo_mib mib counter_pbmp\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_algo_mib_db/mib/counter_pbmp.txt",
            "dnx_algo_mib_db[%d]->","((dnx_algo_mib_db_t*)sw_state_roots_array[%d][DNX_ALGO_MIB_MODULE_ID])->","mib.counter_pbmp: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID]));

        DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
            unit,
            ((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
            "dnx_algo_mib_db[%d]->","((dnx_algo_mib_db_t*)sw_state_roots_array[%d][DNX_ALGO_MIB_MODULE_ID])->","mib.counter_pbmp: ");

        DNX_SW_STATE_PRINT_OPAQUE_FILE(
            unit,
            ((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
                " ");

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_algo_mib_db_mib_interval_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_algo_mib mib interval") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_algo_mib mib interval\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_algo_mib_db/mib/interval.txt",
            "dnx_algo_mib_db[%d]->","((dnx_algo_mib_db_t*)sw_state_roots_array[%d][DNX_ALGO_MIB_MODULE_ID])->","mib.interval: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_algo_mib_db[%d]->","((dnx_algo_mib_db_t*)sw_state_roots_array[%d][DNX_ALGO_MIB_MODULE_ID])->","mib.interval: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.interval);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




dnx_sw_state_diagnostic_info_t dnx_algo_mib_db_info[SOC_MAX_NUM_DEVICES][DNX_ALGO_MIB_DB_INFO_NOF_ENTRIES];
const char* dnx_algo_mib_db_layout_str[DNX_ALGO_MIB_DB_INFO_NOF_ENTRIES] = {
    "DNX_ALGO_MIB_DB~",
    "DNX_ALGO_MIB_DB~MIB~",
    "DNX_ALGO_MIB_DB~MIB~COUNTER_PBMP~",
    "DNX_ALGO_MIB_DB~MIB~INTERVAL~",
};
#endif 
#undef BSL_LOG_MODULE
