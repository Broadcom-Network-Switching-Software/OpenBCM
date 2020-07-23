
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
#include <soc/dnx/swstate/auto_generated/diagnostic/srv6_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern srv6_modes_struct * srv6_modes_data[SOC_MAX_NUM_DEVICES];



int
srv6_modes_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(srv6_modes_egress_is_psp_dump(unit, filters));
    SHR_IF_ERR_EXIT(srv6_modes_encap_is_reduced_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
srv6_modes_egress_is_psp_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "srv6 egress_is_psp") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate srv6 egress_is_psp\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "srv6_modes/egress_is_psp.txt",
            "srv6_modes[%d]->","((srv6_modes_struct*)sw_state_roots_array[%d][SRV6_MODULE_ID])->","egress_is_psp: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((srv6_modes_struct*)sw_state_roots_array[unit][SRV6_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "srv6_modes[%d]->","((srv6_modes_struct*)sw_state_roots_array[%d][SRV6_MODULE_ID])->","egress_is_psp: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint8(
            unit,
            &((srv6_modes_struct*)sw_state_roots_array[unit][SRV6_MODULE_ID])->egress_is_psp);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
srv6_modes_encap_is_reduced_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "srv6 encap_is_reduced") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate srv6 encap_is_reduced\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "srv6_modes/encap_is_reduced.txt",
            "srv6_modes[%d]->","((srv6_modes_struct*)sw_state_roots_array[%d][SRV6_MODULE_ID])->","encap_is_reduced: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((srv6_modes_struct*)sw_state_roots_array[unit][SRV6_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "srv6_modes[%d]->","((srv6_modes_struct*)sw_state_roots_array[%d][SRV6_MODULE_ID])->","encap_is_reduced: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint8(
            unit,
            &((srv6_modes_struct*)sw_state_roots_array[unit][SRV6_MODULE_ID])->encap_is_reduced);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t srv6_modes_info[SOC_MAX_NUM_DEVICES][SRV6_MODES_INFO_NOF_ENTRIES];
const char* srv6_modes_layout_str[SRV6_MODES_INFO_NOF_ENTRIES] = {
    "SRV6_MODES~",
    "SRV6_MODES~EGRESS_IS_PSP~",
    "SRV6_MODES~ENCAP_IS_REDUCED~",
};
#endif 
#undef BSL_LOG_MODULE
