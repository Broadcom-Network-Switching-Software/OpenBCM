
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/diagnostic/flow_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern flow_sw_state_t * flow_db_data[SOC_MAX_NUM_DEVICES];



int
flow_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(flow_db_flow_application_info_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
flow_db_flow_application_info_dump(int  unit,  int  flow_application_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(flow_db_flow_application_info_is_verify_disabled_dump(unit, flow_application_info_idx_0, filters));
    SHR_IF_ERR_EXIT(flow_db_flow_application_info_is_error_recovery_disabled_dump(unit, flow_application_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
flow_db_flow_application_info_is_verify_disabled_dump(int  unit,  int  flow_application_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = flow_application_info_idx_0, I0 = flow_application_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "flow flow_application_info is_verify_disabled") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate flow flow_application_info is_verify_disabled\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "flow_db/flow_application_info/is_verify_disabled.txt",
            "flow_db[%d]->","((flow_sw_state_t*)sw_state_roots_array[%d][FLOW_MODULE_ID])->","flow_application_info[].is_verify_disabled: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                FLOW_MODULE_ID,
                ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info
                , sizeof(*((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, FLOW_MODULE_ID, ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info))
        {
            LOG_CLI((BSL_META("flow_db[]->((flow_sw_state_t*)sw_state_roots_array[][FLOW_MODULE_ID])->flow_application_info[].is_verify_disabled: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, FLOW_MODULE_ID, ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "flow_db[%d]->","((flow_sw_state_t*)sw_state_roots_array[%d][FLOW_MODULE_ID])->","flow_application_info[%s%d].is_verify_disabled: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info[i0].is_verify_disabled);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
flow_db_flow_application_info_is_error_recovery_disabled_dump(int  unit,  int  flow_application_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = flow_application_info_idx_0, I0 = flow_application_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "flow flow_application_info is_error_recovery_disabled") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate flow flow_application_info is_error_recovery_disabled\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "flow_db/flow_application_info/is_error_recovery_disabled.txt",
            "flow_db[%d]->","((flow_sw_state_t*)sw_state_roots_array[%d][FLOW_MODULE_ID])->","flow_application_info[].is_error_recovery_disabled: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                FLOW_MODULE_ID,
                ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info
                , sizeof(*((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, FLOW_MODULE_ID, ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info))
        {
            LOG_CLI((BSL_META("flow_db[]->((flow_sw_state_t*)sw_state_roots_array[][FLOW_MODULE_ID])->flow_application_info[].is_error_recovery_disabled: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, FLOW_MODULE_ID, ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "flow_db[%d]->","((flow_sw_state_t*)sw_state_roots_array[%d][FLOW_MODULE_ID])->","flow_application_info[%s%d].is_error_recovery_disabled: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info[i0].is_error_recovery_disabled);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t flow_db_info[SOC_MAX_NUM_DEVICES][FLOW_DB_INFO_NOF_ENTRIES];
const char* flow_db_layout_str[FLOW_DB_INFO_NOF_ENTRIES] = {
    "FLOW_DB~",
    "FLOW_DB~FLOW_APPLICATION_INFO~",
    "FLOW_DB~FLOW_APPLICATION_INFO~IS_VERIFY_DISABLED~",
    "FLOW_DB~FLOW_APPLICATION_INFO~IS_ERROR_RECOVERY_DISABLED~",
};
#endif 
#undef BSL_LOG_MODULE
