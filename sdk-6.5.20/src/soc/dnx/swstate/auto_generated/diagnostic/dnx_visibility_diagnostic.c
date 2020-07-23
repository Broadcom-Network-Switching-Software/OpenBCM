
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
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_visibility_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern visibility_params * visibility_info_db_data[SOC_MAX_NUM_DEVICES];



int
visibility_info_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(visibility_info_db_visib_mode_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
visibility_info_db_visib_mode_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_visibility visib_mode") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_visibility visib_mode\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "visibility_info_db/visib_mode.txt",
            "visibility_info_db[%d]->","((visibility_params*)sw_state_roots_array[%d][DNX_VISIBILITY_MODULE_ID])->","visib_mode: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((visibility_params*)sw_state_roots_array[unit][DNX_VISIBILITY_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "visibility_info_db[%d]->","((visibility_params*)sw_state_roots_array[%d][DNX_VISIBILITY_MODULE_ID])->","visib_mode: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((visibility_params*)sw_state_roots_array[unit][DNX_VISIBILITY_MODULE_ID])->visib_mode);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t visibility_info_db_info[SOC_MAX_NUM_DEVICES][VISIBILITY_INFO_DB_INFO_NOF_ENTRIES];
const char* visibility_info_db_layout_str[VISIBILITY_INFO_DB_INFO_NOF_ENTRIES] = {
    "VISIBILITY_INFO_DB~",
    "VISIBILITY_INFO_DB~VISIB_MODE~",
};
#endif 
#undef BSL_LOG_MODULE
