
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/diagnostic/edk_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/edk_layout.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern edk_sw_state * edk_state_data[SOC_MAX_NUM_DEVICES];



int
edk_state_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(edk_state_edk_seq_dump(unit, -1, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
edk_state_edk_seq_dump(int  unit,  int  edk_seq_idx_0,  int  edk_seq_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = edk_seq_idx_0, I0 = edk_seq_idx_0 + 1;
    int i1 = edk_seq_idx_1, I1 = edk_seq_idx_1 + 1, org_i1 = edk_seq_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "edk edk_seq") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate edk edk_seq\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "edk_state/edk_seq.txt",
            "edk_state[%d]->","((edk_sw_state*)sw_state_roots_array[%d][EDK_MODULE_ID])->","edk_seq[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((edk_sw_state*)sw_state_roots_array[unit][EDK_MODULE_ID]));

        if (i0 == -1) {
            I0 = EDK_SEQ_MAX;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((edk_sw_state*)sw_state_roots_array[unit][EDK_MODULE_ID])->edk_seq
                , sizeof(*((edk_sw_state*)sw_state_roots_array[unit][EDK_MODULE_ID])->edk_seq), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= EDK_SEQ_MAX)
        {
            LOG_CLI((BSL_META("edk_state[]->((edk_sw_state*)sw_state_roots_array[][EDK_MODULE_ID])->edk_seq[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(EDK_SEQ_MAX == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((edk_sw_state*)sw_state_roots_array[unit][EDK_MODULE_ID])->edk_seq[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_SW_STATE_EDK_STATE__EDK_SEQ,
                    ((edk_sw_state*)sw_state_roots_array[unit][EDK_MODULE_ID])->edk_seq[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((edk_sw_state*)sw_state_roots_array[unit][EDK_MODULE_ID])->edk_seq[i0]
                    , sizeof(*((edk_sw_state*)sw_state_roots_array[unit][EDK_MODULE_ID])->edk_seq[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_EDK_STATE__EDK_SEQ, ((edk_sw_state*)sw_state_roots_array[unit][EDK_MODULE_ID])->edk_seq[i0]))
            {
                LOG_CLI((BSL_META("edk_state[]->((edk_sw_state*)sw_state_roots_array[][EDK_MODULE_ID])->edk_seq[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_EDK_STATE__EDK_SEQ, ((edk_sw_state*)sw_state_roots_array[unit][EDK_MODULE_ID])->edk_seq[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "edk_state[%d]->","((edk_sw_state*)sw_state_roots_array[%d][EDK_MODULE_ID])->","edk_seq[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((edk_sw_state*)sw_state_roots_array[unit][EDK_MODULE_ID])->edk_seq[i0][i1]);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




dnx_sw_state_diagnostic_info_t edk_state_info[SOC_MAX_NUM_DEVICES][EDK_STATE_INFO_NOF_ENTRIES];
const char* edk_state_layout_str[EDK_STATE_INFO_NOF_ENTRIES] = {
    "EDK_STATE~",
    "EDK_STATE~EDK_SEQ~",
};
#endif 
#undef BSL_LOG_MODULE
