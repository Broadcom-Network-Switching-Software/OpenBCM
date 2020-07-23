
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
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_tcam_manager_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_field_tcam_manager_sw_t * dnx_field_tcam_manager_sw_data[SOC_MAX_NUM_DEVICES];



int
dnx_field_tcam_manager_sw_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_tcam_manager_sw_tcam_handlers_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_manager_sw_tcam_handlers_dump(int  unit,  int  tcam_handlers_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_tcam_manager_sw_tcam_handlers_mode_dump(unit, tcam_handlers_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_tcam_manager_sw_tcam_handlers_state_dump(unit, tcam_handlers_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_manager_sw_tcam_handlers_mode_dump(int  unit,  int  tcam_handlers_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = tcam_handlers_idx_0, I0 = tcam_handlers_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_tcam_handler_mode_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_manager tcam_handlers mode") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_manager tcam_handlers mode\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_manager_sw/tcam_handlers/mode.txt",
            "dnx_field_tcam_manager_sw[%d]->","((dnx_field_tcam_manager_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_MANAGER_MODULE_ID])->","tcam_handlers[].mode: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_manager_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_MANAGER_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_manager_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_MANAGER_MODULE_ID])->tcam_handlers);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_MANAGER_MODULE_ID,
                ((dnx_field_tcam_manager_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_MANAGER_MODULE_ID])->tcam_handlers);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_manager_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_MANAGER_MODULE_ID])->tcam_handlers
                , sizeof(*((dnx_field_tcam_manager_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_MANAGER_MODULE_ID])->tcam_handlers), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_MANAGER_MODULE_ID, ((dnx_field_tcam_manager_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_MANAGER_MODULE_ID])->tcam_handlers))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_manager_sw[]->((dnx_field_tcam_manager_sw_t*)sw_state_roots_array[][DNX_FIELD_TCAM_MANAGER_MODULE_ID])->tcam_handlers[].mode: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_MANAGER_MODULE_ID, ((dnx_field_tcam_manager_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_MANAGER_MODULE_ID])->tcam_handlers) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_field_tcam_manager_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_MANAGER_MODULE_ID])->tcam_handlers[i0].mode,
                "dnx_field_tcam_manager_sw[%d]->","((dnx_field_tcam_manager_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_MANAGER_MODULE_ID])->","tcam_handlers[%s%d].mode: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_field_tcam_manager_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_MANAGER_MODULE_ID])->tcam_handlers[i0].mode,
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_manager_sw_tcam_handlers_state_dump(int  unit,  int  tcam_handlers_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = tcam_handlers_idx_0, I0 = tcam_handlers_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_tcam_handler_state_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_manager tcam_handlers state") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_manager tcam_handlers state\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_manager_sw/tcam_handlers/state.txt",
            "dnx_field_tcam_manager_sw[%d]->","((dnx_field_tcam_manager_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_MANAGER_MODULE_ID])->","tcam_handlers[].state: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_manager_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_MANAGER_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_manager_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_MANAGER_MODULE_ID])->tcam_handlers);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_MANAGER_MODULE_ID,
                ((dnx_field_tcam_manager_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_MANAGER_MODULE_ID])->tcam_handlers);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_manager_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_MANAGER_MODULE_ID])->tcam_handlers
                , sizeof(*((dnx_field_tcam_manager_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_MANAGER_MODULE_ID])->tcam_handlers), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_MANAGER_MODULE_ID, ((dnx_field_tcam_manager_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_MANAGER_MODULE_ID])->tcam_handlers))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_manager_sw[]->((dnx_field_tcam_manager_sw_t*)sw_state_roots_array[][DNX_FIELD_TCAM_MANAGER_MODULE_ID])->tcam_handlers[].state: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_MANAGER_MODULE_ID, ((dnx_field_tcam_manager_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_MANAGER_MODULE_ID])->tcam_handlers) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_field_tcam_manager_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_MANAGER_MODULE_ID])->tcam_handlers[i0].state,
                "dnx_field_tcam_manager_sw[%d]->","((dnx_field_tcam_manager_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_MANAGER_MODULE_ID])->","tcam_handlers[%s%d].state: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_field_tcam_manager_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_MANAGER_MODULE_ID])->tcam_handlers[i0].state,
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnx_field_tcam_manager_sw_info[SOC_MAX_NUM_DEVICES][DNX_FIELD_TCAM_MANAGER_SW_INFO_NOF_ENTRIES];
const char* dnx_field_tcam_manager_sw_layout_str[DNX_FIELD_TCAM_MANAGER_SW_INFO_NOF_ENTRIES] = {
    "DNX_FIELD_TCAM_MANAGER_SW~",
    "DNX_FIELD_TCAM_MANAGER_SW~TCAM_HANDLERS~",
    "DNX_FIELD_TCAM_MANAGER_SW~TCAM_HANDLERS~MODE~",
    "DNX_FIELD_TCAM_MANAGER_SW~TCAM_HANDLERS~STATE~",
};
#endif 
#undef BSL_LOG_MODULE
