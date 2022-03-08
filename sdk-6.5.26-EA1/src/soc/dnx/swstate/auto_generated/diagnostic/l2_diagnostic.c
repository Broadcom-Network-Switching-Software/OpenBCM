
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/diagnostic/l2_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/l2_layout.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern l2_db_t * l2_db_context_data[SOC_MAX_NUM_DEVICES];



int
l2_db_context_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(l2_db_context_fid_profiles_changed_dump(unit, filters));
    SHR_IF_ERR_EXIT(l2_db_context_routing_learning_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_fid_profiles_changed_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "l2 fid_profiles_changed") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate l2 fid_profiles_changed\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "l2_db_context/fid_profiles_changed.txt",
            "l2_db_context[%d]->","((l2_db_t*)sw_state_roots_array[%d][L2_MODULE_ID])->","fid_profiles_changed: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "l2_db_context[%d]->","((l2_db_t*)sw_state_roots_array[%d][L2_MODULE_ID])->","fid_profiles_changed: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint8(
            unit,
            &((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->fid_profiles_changed);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_routing_learning_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "l2 routing_learning") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate l2 routing_learning\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "l2_db_context/routing_learning.txt",
            "l2_db_context[%d]->","((l2_db_t*)sw_state_roots_array[%d][L2_MODULE_ID])->","routing_learning: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "l2_db_context[%d]->","((l2_db_t*)sw_state_roots_array[%d][L2_MODULE_ID])->","routing_learning: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint32(
            unit,
            &((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->routing_learning);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t l2_db_context_info[SOC_MAX_NUM_DEVICES][L2_DB_CONTEXT_INFO_NOF_ENTRIES];
const char* l2_db_context_layout_str[L2_DB_CONTEXT_INFO_NOF_ENTRIES] = {
    "L2_DB_CONTEXT~",
    "L2_DB_CONTEXT~FID_PROFILES_CHANGED~",
    "L2_DB_CONTEXT~ROUTING_LEARNING~",
};
#endif 
#undef BSL_LOG_MODULE
