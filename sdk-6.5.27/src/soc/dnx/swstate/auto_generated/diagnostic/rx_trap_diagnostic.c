
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/diagnostic/rx_trap_diagnostic.h>
#include <include/bcm/rx.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trap.h>
#include <soc/dnx/swstate/auto_generated/layout/rx_trap_layout.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern rx_trap_info_t * rx_trap_info_data[SOC_MAX_NUM_DEVICES];



int
rx_trap_info_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(rx_trap_info_user_trap_info_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
rx_trap_info_user_trap_info_dump(int  unit,  int  user_trap_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(rx_trap_info_user_trap_info_name_dump(unit, user_trap_info_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
rx_trap_info_user_trap_info_name_dump(int  unit,  int  user_trap_info_idx_0,  int  name_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = user_trap_info_idx_0, I0 = user_trap_info_idx_0 + 1;
    int i1 = name_idx_0, I1 = name_idx_0 + 1, org_i1 = name_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "char") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "rx_trap user_trap_info name") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate rx_trap user_trap_info name\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "rx_trap_info/user_trap_info/name.txt",
            "rx_trap_info[%d]->","((rx_trap_info_t*)sw_state_roots_array[%d][RX_TRAP_MODULE_ID])->","user_trap_info[].name[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((rx_trap_info_t*)sw_state_roots_array[unit][RX_TRAP_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((rx_trap_info_t*)sw_state_roots_array[unit][RX_TRAP_MODULE_ID])->user_trap_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_RX_TRAP_INFO__USER_TRAP_INFO__NAME,
                ((rx_trap_info_t*)sw_state_roots_array[unit][RX_TRAP_MODULE_ID])->user_trap_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((rx_trap_info_t*)sw_state_roots_array[unit][RX_TRAP_MODULE_ID])->user_trap_info
                , sizeof(*((rx_trap_info_t*)sw_state_roots_array[unit][RX_TRAP_MODULE_ID])->user_trap_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_RX_TRAP_INFO__USER_TRAP_INFO__NAME, ((rx_trap_info_t*)sw_state_roots_array[unit][RX_TRAP_MODULE_ID])->user_trap_info))
        {
            LOG_CLI((BSL_META("rx_trap_info[]->((rx_trap_info_t*)sw_state_roots_array[][RX_TRAP_MODULE_ID])->user_trap_info[].name[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_RX_TRAP_INFO__USER_TRAP_INFO__NAME, ((rx_trap_info_t*)sw_state_roots_array[unit][RX_TRAP_MODULE_ID])->user_trap_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = BCM_RX_TRAP_MAX_NAME_LEN;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((rx_trap_info_t*)sw_state_roots_array[unit][RX_TRAP_MODULE_ID])->user_trap_info[i0].name
                    , sizeof(*((rx_trap_info_t*)sw_state_roots_array[unit][RX_TRAP_MODULE_ID])->user_trap_info[i0].name), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= BCM_RX_TRAP_MAX_NAME_LEN)
            {
                LOG_CLI((BSL_META("rx_trap_info[]->((rx_trap_info_t*)sw_state_roots_array[][RX_TRAP_MODULE_ID])->user_trap_info[].name[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(BCM_RX_TRAP_MAX_NAME_LEN == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "rx_trap_info[%d]->","((rx_trap_info_t*)sw_state_roots_array[%d][RX_TRAP_MODULE_ID])->","user_trap_info[%s%d].name[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_char(
                    unit,
                    &((rx_trap_info_t*)sw_state_roots_array[unit][RX_TRAP_MODULE_ID])->user_trap_info[i0].name[i1]);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




dnx_sw_state_diagnostic_info_t rx_trap_info_info[SOC_MAX_NUM_DEVICES][RX_TRAP_INFO_INFO_NOF_ENTRIES];
const char* rx_trap_info_layout_str[RX_TRAP_INFO_INFO_NOF_ENTRIES] = {
    "RX_TRAP_INFO~",
    "RX_TRAP_INFO~USER_TRAP_INFO~",
    "RX_TRAP_INFO~USER_TRAP_INFO~NAME~",
};
#endif 
#undef BSL_LOG_MODULE
