
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
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_algo_synce_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_algo_synce_db_t * dnx_algo_synce_db_data[SOC_MAX_NUM_DEVICES];



int
dnx_algo_synce_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_algo_synce_db_fabric_synce_status_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_synce_db_fabric_synce_status_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_algo_synce_db_fabric_synce_status_master_synce_enabled_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_algo_synce_db_fabric_synce_status_slave_synce_enabled_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_synce_db_fabric_synce_status_master_synce_enabled_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_algo_synce fabric_synce_status master_synce_enabled") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_algo_synce fabric_synce_status master_synce_enabled\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_algo_synce_db/fabric_synce_status/master_synce_enabled.txt",
            "dnx_algo_synce_db[%d]->","((dnx_algo_synce_db_t*)sw_state_roots_array[%d][DNX_ALGO_SYNCE_MODULE_ID])->","fabric_synce_status.master_synce_enabled: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_algo_synce_db_t*)sw_state_roots_array[unit][DNX_ALGO_SYNCE_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_algo_synce_db[%d]->","((dnx_algo_synce_db_t*)sw_state_roots_array[%d][DNX_ALGO_SYNCE_MODULE_ID])->","fabric_synce_status.master_synce_enabled: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnx_algo_synce_db_t*)sw_state_roots_array[unit][DNX_ALGO_SYNCE_MODULE_ID])->fabric_synce_status.master_synce_enabled);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_synce_db_fabric_synce_status_slave_synce_enabled_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_algo_synce fabric_synce_status slave_synce_enabled") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_algo_synce fabric_synce_status slave_synce_enabled\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_algo_synce_db/fabric_synce_status/slave_synce_enabled.txt",
            "dnx_algo_synce_db[%d]->","((dnx_algo_synce_db_t*)sw_state_roots_array[%d][DNX_ALGO_SYNCE_MODULE_ID])->","fabric_synce_status.slave_synce_enabled: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_algo_synce_db_t*)sw_state_roots_array[unit][DNX_ALGO_SYNCE_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_algo_synce_db[%d]->","((dnx_algo_synce_db_t*)sw_state_roots_array[%d][DNX_ALGO_SYNCE_MODULE_ID])->","fabric_synce_status.slave_synce_enabled: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnx_algo_synce_db_t*)sw_state_roots_array[unit][DNX_ALGO_SYNCE_MODULE_ID])->fabric_synce_status.slave_synce_enabled);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnx_algo_synce_db_info[SOC_MAX_NUM_DEVICES][DNX_ALGO_SYNCE_DB_INFO_NOF_ENTRIES];
const char* dnx_algo_synce_db_layout_str[DNX_ALGO_SYNCE_DB_INFO_NOF_ENTRIES] = {
    "DNX_ALGO_SYNCE_DB~",
    "DNX_ALGO_SYNCE_DB~FABRIC_SYNCE_STATUS~",
    "DNX_ALGO_SYNCE_DB~FABRIC_SYNCE_STATUS~MASTER_SYNCE_ENABLED~",
    "DNX_ALGO_SYNCE_DB~FABRIC_SYNCE_STATUS~SLAVE_SYNCE_ENABLED~",
};
#endif 
#undef BSL_LOG_MODULE
