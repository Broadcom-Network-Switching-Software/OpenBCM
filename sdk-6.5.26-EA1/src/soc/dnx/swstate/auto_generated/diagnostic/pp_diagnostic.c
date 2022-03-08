
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/diagnostic/pp_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/pp_layout.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_pp_sw_state_t * pp_db_data[SOC_MAX_NUM_DEVICES];



int
pp_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(pp_db_pp_stage_kbr_select_dump(unit, -1, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
pp_db_pp_stage_kbr_select_dump(int  unit,  int  pp_stage_kbr_select_idx_0,  int  pp_stage_kbr_select_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = pp_stage_kbr_select_idx_0, I0 = pp_stage_kbr_select_idx_0 + 1;
    int i1 = pp_stage_kbr_select_idx_1, I1 = pp_stage_kbr_select_idx_1 + 1, org_i1 = pp_stage_kbr_select_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "pp pp_stage_kbr_select") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate pp pp_stage_kbr_select\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "pp_db/pp_stage_kbr_select.txt",
            "pp_db[%d]->","((dnx_pp_sw_state_t*)sw_state_roots_array[%d][PP_MODULE_ID])->","pp_stage_kbr_select[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_pp_sw_state_t*)sw_state_roots_array[unit][PP_MODULE_ID]));

        if (i0 == -1) {
            I0 = MAX_NOF_PP_KBR;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_pp_sw_state_t*)sw_state_roots_array[unit][PP_MODULE_ID])->pp_stage_kbr_select
                , sizeof(*((dnx_pp_sw_state_t*)sw_state_roots_array[unit][PP_MODULE_ID])->pp_stage_kbr_select), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= MAX_NOF_PP_KBR)
        {
            LOG_CLI((BSL_META("pp_db[]->((dnx_pp_sw_state_t*)sw_state_roots_array[][PP_MODULE_ID])->pp_stage_kbr_select[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(MAX_NOF_PP_KBR == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = MAX_NOF_KBR_SELCTORS;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_pp_sw_state_t*)sw_state_roots_array[unit][PP_MODULE_ID])->pp_stage_kbr_select[i0]
                    , sizeof(*((dnx_pp_sw_state_t*)sw_state_roots_array[unit][PP_MODULE_ID])->pp_stage_kbr_select[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= MAX_NOF_KBR_SELCTORS)
            {
                LOG_CLI((BSL_META("pp_db[]->((dnx_pp_sw_state_t*)sw_state_roots_array[][PP_MODULE_ID])->pp_stage_kbr_select[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(MAX_NOF_KBR_SELCTORS == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "pp_db[%d]->","((dnx_pp_sw_state_t*)sw_state_roots_array[%d][PP_MODULE_ID])->","pp_stage_kbr_select[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((dnx_pp_sw_state_t*)sw_state_roots_array[unit][PP_MODULE_ID])->pp_stage_kbr_select[i0][i1]);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t pp_db_info[SOC_MAX_NUM_DEVICES][PP_DB_INFO_NOF_ENTRIES];
const char* pp_db_layout_str[PP_DB_INFO_NOF_ENTRIES] = {
    "PP_DB~",
    "PP_DB~PP_STAGE_KBR_SELECT~",
};
#endif 
#undef BSL_LOG_MODULE
