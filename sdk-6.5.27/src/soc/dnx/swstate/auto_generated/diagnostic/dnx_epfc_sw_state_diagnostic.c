
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_epfc_sw_state_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_epfc_sw_state_layout.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_epfc_sw_state_t * dnx_epfc_interface_db_data[SOC_MAX_NUM_DEVICES];



int
dnx_epfc_interface_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_epfc_interface_db_is_initialized_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_epfc_interface_db_uc_num_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_epfc_interface_db_is_initialized_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_epfc_sw_state is_initialized") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_epfc_sw_state is_initialized\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_epfc_interface_db/is_initialized.txt",
            "dnx_epfc_interface_db[%d]->","((dnx_epfc_sw_state_t*)sw_state_roots_array[%d][DNX_EPFC_SW_STATE_MODULE_ID])->","is_initialized: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_epfc_sw_state_t*)sw_state_roots_array[unit][DNX_EPFC_SW_STATE_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_epfc_interface_db[%d]->","((dnx_epfc_sw_state_t*)sw_state_roots_array[%d][DNX_EPFC_SW_STATE_MODULE_ID])->","is_initialized: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint32(
            unit,
            &((dnx_epfc_sw_state_t*)sw_state_roots_array[unit][DNX_EPFC_SW_STATE_MODULE_ID])->is_initialized);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_epfc_interface_db_uc_num_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_epfc_sw_state uc_num") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_epfc_sw_state uc_num\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_epfc_interface_db/uc_num.txt",
            "dnx_epfc_interface_db[%d]->","((dnx_epfc_sw_state_t*)sw_state_roots_array[%d][DNX_EPFC_SW_STATE_MODULE_ID])->","uc_num: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_epfc_sw_state_t*)sw_state_roots_array[unit][DNX_EPFC_SW_STATE_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_epfc_interface_db[%d]->","((dnx_epfc_sw_state_t*)sw_state_roots_array[%d][DNX_EPFC_SW_STATE_MODULE_ID])->","uc_num: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint32(
            unit,
            &((dnx_epfc_sw_state_t*)sw_state_roots_array[unit][DNX_EPFC_SW_STATE_MODULE_ID])->uc_num);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




dnx_sw_state_diagnostic_info_t dnx_epfc_interface_db_info[SOC_MAX_NUM_DEVICES][DNX_EPFC_INTERFACE_DB_INFO_NOF_ENTRIES];
const char* dnx_epfc_interface_db_layout_str[DNX_EPFC_INTERFACE_DB_INFO_NOF_ENTRIES] = {
    "DNX_EPFC_INTERFACE_DB~",
    "DNX_EPFC_INTERFACE_DB~IS_INITIALIZED~",
    "DNX_EPFC_INTERFACE_DB~UC_NUM~",
};
#endif 
#undef BSL_LOG_MODULE
