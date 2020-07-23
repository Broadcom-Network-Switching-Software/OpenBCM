
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
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_traps_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_rx_traps_info_t * dnx_rx_traps_info_data[SOC_MAX_NUM_DEVICES];



int
dnx_rx_traps_info_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_rx_traps_info_etpp_keep_fabric_headers_profile_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_rx_traps_info_etpp_keep_fabric_headers_profile_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_traps etpp_keep_fabric_headers_profile") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_traps etpp_keep_fabric_headers_profile\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_rx_traps_info/etpp_keep_fabric_headers_profile.txt",
            "dnx_rx_traps_info[%d]->","((dnx_rx_traps_info_t*)sw_state_roots_array[%d][DNX_TRAPS_MODULE_ID])->","etpp_keep_fabric_headers_profile: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_rx_traps_info_t*)sw_state_roots_array[unit][DNX_TRAPS_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_rx_traps_info[%d]->","((dnx_rx_traps_info_t*)sw_state_roots_array[%d][DNX_TRAPS_MODULE_ID])->","etpp_keep_fabric_headers_profile: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint8(
            unit,
            &((dnx_rx_traps_info_t*)sw_state_roots_array[unit][DNX_TRAPS_MODULE_ID])->etpp_keep_fabric_headers_profile);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnx_rx_traps_info_info[SOC_MAX_NUM_DEVICES][DNX_RX_TRAPS_INFO_INFO_NOF_ENTRIES];
const char* dnx_rx_traps_info_layout_str[DNX_RX_TRAPS_INFO_INFO_NOF_ENTRIES] = {
    "DNX_RX_TRAPS_INFO~",
    "DNX_RX_TRAPS_INFO~ETPP_KEEP_FABRIC_HEADERS_PROFILE~",
};
#endif 
#undef BSL_LOG_MODULE
