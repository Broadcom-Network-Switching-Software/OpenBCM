
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
#include <soc/dnx/swstate/auto_generated/diagnostic/flush_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern flush_db_t * flush_db_context_data[SOC_MAX_NUM_DEVICES];



int
flush_db_context_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(flush_db_context_nof_valid_rules_dump(unit, filters));
    SHR_IF_ERR_EXIT(flush_db_context_flush_in_bulk_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
flush_db_context_nof_valid_rules_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "flush nof_valid_rules") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate flush nof_valid_rules\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "flush_db_context/nof_valid_rules.txt",
            "flush_db_context[%d]->","((flush_db_t*)sw_state_roots_array[%d][FLUSH_MODULE_ID])->","nof_valid_rules: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((flush_db_t*)sw_state_roots_array[unit][FLUSH_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "flush_db_context[%d]->","((flush_db_t*)sw_state_roots_array[%d][FLUSH_MODULE_ID])->","nof_valid_rules: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint32(
            unit,
            &((flush_db_t*)sw_state_roots_array[unit][FLUSH_MODULE_ID])->nof_valid_rules);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
flush_db_context_flush_in_bulk_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "flush flush_in_bulk") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate flush flush_in_bulk\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "flush_db_context/flush_in_bulk.txt",
            "flush_db_context[%d]->","((flush_db_t*)sw_state_roots_array[%d][FLUSH_MODULE_ID])->","flush_in_bulk: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((flush_db_t*)sw_state_roots_array[unit][FLUSH_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "flush_db_context[%d]->","((flush_db_t*)sw_state_roots_array[%d][FLUSH_MODULE_ID])->","flush_in_bulk: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint8(
            unit,
            &((flush_db_t*)sw_state_roots_array[unit][FLUSH_MODULE_ID])->flush_in_bulk);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t flush_db_context_info[SOC_MAX_NUM_DEVICES][FLUSH_DB_CONTEXT_INFO_NOF_ENTRIES];
const char* flush_db_context_layout_str[FLUSH_DB_CONTEXT_INFO_NOF_ENTRIES] = {
    "FLUSH_DB_CONTEXT~",
    "FLUSH_DB_CONTEXT~NOF_VALID_RULES~",
    "FLUSH_DB_CONTEXT~FLUSH_IN_BULK~",
};
#endif 
#undef BSL_LOG_MODULE
