
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#ifdef BCM_DNX_SUPPORT
#include <soc/dnxc/swstate/dnxc_sw_state_c_includes.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_stk_sys_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_stk_sys_db_t * dnx_stk_sys_db_data[SOC_MAX_NUM_DEVICES];



int
dnx_stk_sys_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_stk_sys_db_nof_fap_ids_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_stk_sys_db_module_enable_done_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_stk_sys_db_nof_fap_ids_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_stk_sys nof_fap_ids") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_stk_sys nof_fap_ids\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_stk_sys_db/nof_fap_ids.txt",
            "dnx_stk_sys_db[%d]->","((dnx_stk_sys_db_t*)sw_state_roots_array[%d][DNX_STK_SYS_MODULE_ID])->","nof_fap_ids: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_stk_sys_db_t*)sw_state_roots_array[unit][DNX_STK_SYS_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_stk_sys_db[%d]->","((dnx_stk_sys_db_t*)sw_state_roots_array[%d][DNX_STK_SYS_MODULE_ID])->","nof_fap_ids: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnx_stk_sys_db_t*)sw_state_roots_array[unit][DNX_STK_SYS_MODULE_ID])->nof_fap_ids);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_stk_sys_db_module_enable_done_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_stk_sys module_enable_done") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_stk_sys module_enable_done\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_stk_sys_db/module_enable_done.txt",
            "dnx_stk_sys_db[%d]->","((dnx_stk_sys_db_t*)sw_state_roots_array[%d][DNX_STK_SYS_MODULE_ID])->","module_enable_done: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_stk_sys_db_t*)sw_state_roots_array[unit][DNX_STK_SYS_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_stk_sys_db[%d]->","((dnx_stk_sys_db_t*)sw_state_roots_array[%d][DNX_STK_SYS_MODULE_ID])->","module_enable_done: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnx_stk_sys_db_t*)sw_state_roots_array[unit][DNX_STK_SYS_MODULE_ID])->module_enable_done);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnx_stk_sys_db_info[SOC_MAX_NUM_DEVICES][DNX_STK_SYS_DB_INFO_NOF_ENTRIES];
const char* dnx_stk_sys_db_layout_str[DNX_STK_SYS_DB_INFO_NOF_ENTRIES] = {
    "DNX_STK_SYS_DB~",
    "DNX_STK_SYS_DB~NOF_FAP_IDS~",
    "DNX_STK_SYS_DB~MODULE_ENABLE_DONE~",
};
#endif 
#endif  
#undef BSL_LOG_MODULE
