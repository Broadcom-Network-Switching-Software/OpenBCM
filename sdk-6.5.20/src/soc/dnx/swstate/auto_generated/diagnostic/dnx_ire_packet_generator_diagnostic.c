
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
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_ire_packet_generator_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_ire_packet_generator_info_struct * ire_packet_generator_info_data[SOC_MAX_NUM_DEVICES];



int
ire_packet_generator_info_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(ire_packet_generator_info_ire_datapath_enable_state_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
ire_packet_generator_info_ire_datapath_enable_state_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_ire_packet_generator ire_datapath_enable_state") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_ire_packet_generator ire_datapath_enable_state\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "ire_packet_generator_info/ire_datapath_enable_state.txt",
            "ire_packet_generator_info[%d]->","((dnx_ire_packet_generator_info_struct*)sw_state_roots_array[%d][DNX_IRE_PACKET_GENERATOR_MODULE_ID])->","ire_datapath_enable_state: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_ire_packet_generator_info_struct*)sw_state_roots_array[unit][DNX_IRE_PACKET_GENERATOR_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "ire_packet_generator_info[%d]->","((dnx_ire_packet_generator_info_struct*)sw_state_roots_array[%d][DNX_IRE_PACKET_GENERATOR_MODULE_ID])->","ire_datapath_enable_state: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint32(
            unit,
            &((dnx_ire_packet_generator_info_struct*)sw_state_roots_array[unit][DNX_IRE_PACKET_GENERATOR_MODULE_ID])->ire_datapath_enable_state);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t ire_packet_generator_info_info[SOC_MAX_NUM_DEVICES][IRE_PACKET_GENERATOR_INFO_INFO_NOF_ENTRIES];
const char* ire_packet_generator_info_layout_str[IRE_PACKET_GENERATOR_INFO_INFO_NOF_ENTRIES] = {
    "IRE_PACKET_GENERATOR_INFO~",
    "IRE_PACKET_GENERATOR_INFO~IRE_DATAPATH_ENABLE_STATE~",
};
#endif 
#undef BSL_LOG_MODULE
