
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
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_hard_reset_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_hard_reset_db_t * dnx_hard_reset_db_data[SOC_MAX_NUM_DEVICES];



int
dnx_hard_reset_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_hard_reset_db_hard_reset_callback_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_hard_reset_db_hard_reset_callback_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_hard_reset_db_hard_reset_callback_callback_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "bcm_switch_hard_reset_callback_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_hard_reset hard_reset_callback callback") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_hard_reset hard_reset_callback callback\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_hard_reset_db/hard_reset_callback/callback.txt",
            "dnx_hard_reset_db[%d]->","((dnx_hard_reset_db_t*)sw_state_roots_array[%d][DNX_HARD_RESET_MODULE_ID])->","hard_reset_callback.callback: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_hard_reset_db_t*)sw_state_roots_array[unit][DNX_HARD_RESET_MODULE_ID]));

        DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
            unit,
            ((dnx_hard_reset_db_t*)sw_state_roots_array[unit][DNX_HARD_RESET_MODULE_ID])->hard_reset_callback.callback,
            "dnx_hard_reset_db[%d]->","((dnx_hard_reset_db_t*)sw_state_roots_array[%d][DNX_HARD_RESET_MODULE_ID])->","hard_reset_callback.callback: ");

        DNX_SW_STATE_PRINT_OPAQUE_FILE(
            unit,
            ((dnx_hard_reset_db_t*)sw_state_roots_array[unit][DNX_HARD_RESET_MODULE_ID])->hard_reset_callback.callback,
                " ");

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_hard_reset_db_hard_reset_callback_userdata_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_hard_reset_callback_userdata_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_hard_reset hard_reset_callback userdata") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_hard_reset hard_reset_callback userdata\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_hard_reset_db/hard_reset_callback/userdata.txt",
            "dnx_hard_reset_db[%d]->","((dnx_hard_reset_db_t*)sw_state_roots_array[%d][DNX_HARD_RESET_MODULE_ID])->","hard_reset_callback.userdata: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_hard_reset_db_t*)sw_state_roots_array[unit][DNX_HARD_RESET_MODULE_ID]));

        DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
            unit,
            ((dnx_hard_reset_db_t*)sw_state_roots_array[unit][DNX_HARD_RESET_MODULE_ID])->hard_reset_callback.userdata,
            "dnx_hard_reset_db[%d]->","((dnx_hard_reset_db_t*)sw_state_roots_array[%d][DNX_HARD_RESET_MODULE_ID])->","hard_reset_callback.userdata: ");

        DNX_SW_STATE_PRINT_OPAQUE_FILE(
            unit,
            ((dnx_hard_reset_db_t*)sw_state_roots_array[unit][DNX_HARD_RESET_MODULE_ID])->hard_reset_callback.userdata,
                " ");

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnx_hard_reset_db_info[SOC_MAX_NUM_DEVICES][DNX_HARD_RESET_DB_INFO_NOF_ENTRIES];
const char* dnx_hard_reset_db_layout_str[DNX_HARD_RESET_DB_INFO_NOF_ENTRIES] = {
    "DNX_HARD_RESET_DB~",
    "DNX_HARD_RESET_DB~HARD_RESET_CALLBACK~",
    "DNX_HARD_RESET_DB~HARD_RESET_CALLBACK~CALLBACK~",
    "DNX_HARD_RESET_DB~HARD_RESET_CALLBACK~USERDATA~",
};
#endif 
#undef BSL_LOG_MODULE
