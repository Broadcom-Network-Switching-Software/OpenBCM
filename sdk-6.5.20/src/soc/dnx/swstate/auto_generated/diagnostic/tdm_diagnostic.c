
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
#include <soc/dnx/swstate/auto_generated/diagnostic/tdm_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern tdm_ftmh_headers_params * tdm_ftmh_info_data[SOC_MAX_NUM_DEVICES];



int
tdm_ftmh_info_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(tdm_ftmh_info_tdm_header_is_ftmh_dump(unit, filters));
    SHR_IF_ERR_EXIT(tdm_ftmh_info_use_optimized_ftmh_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
tdm_ftmh_info_tdm_header_is_ftmh_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "tdm tdm_header_is_ftmh") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate tdm tdm_header_is_ftmh\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "tdm_ftmh_info/tdm_header_is_ftmh.txt",
            "tdm_ftmh_info[%d]->","((tdm_ftmh_headers_params*)sw_state_roots_array[%d][TDM_MODULE_ID])->","tdm_header_is_ftmh: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((tdm_ftmh_headers_params*)sw_state_roots_array[unit][TDM_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "tdm_ftmh_info[%d]->","((tdm_ftmh_headers_params*)sw_state_roots_array[%d][TDM_MODULE_ID])->","tdm_header_is_ftmh: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint8(
            unit,
            &((tdm_ftmh_headers_params*)sw_state_roots_array[unit][TDM_MODULE_ID])->tdm_header_is_ftmh);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
tdm_ftmh_info_use_optimized_ftmh_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "tdm use_optimized_ftmh") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate tdm use_optimized_ftmh\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "tdm_ftmh_info/use_optimized_ftmh.txt",
            "tdm_ftmh_info[%d]->","((tdm_ftmh_headers_params*)sw_state_roots_array[%d][TDM_MODULE_ID])->","use_optimized_ftmh: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((tdm_ftmh_headers_params*)sw_state_roots_array[unit][TDM_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "tdm_ftmh_info[%d]->","((tdm_ftmh_headers_params*)sw_state_roots_array[%d][TDM_MODULE_ID])->","use_optimized_ftmh: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint8(
            unit,
            &((tdm_ftmh_headers_params*)sw_state_roots_array[unit][TDM_MODULE_ID])->use_optimized_ftmh);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t tdm_ftmh_info_info[SOC_MAX_NUM_DEVICES][TDM_FTMH_INFO_INFO_NOF_ENTRIES];
const char* tdm_ftmh_info_layout_str[TDM_FTMH_INFO_INFO_NOF_ENTRIES] = {
    "TDM_FTMH_INFO~",
    "TDM_FTMH_INFO~TDM_HEADER_IS_FTMH~",
    "TDM_FTMH_INFO~USE_OPTIMIZED_FTMH~",
};
#endif 
#undef BSL_LOG_MODULE
