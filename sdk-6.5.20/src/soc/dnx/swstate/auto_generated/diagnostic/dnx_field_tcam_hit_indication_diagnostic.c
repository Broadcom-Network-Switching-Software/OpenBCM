
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
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_tcam_hit_indication_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_field_tcam_hit_indication_t * dnx_field_tcam_hit_indication_data[SOC_MAX_NUM_DEVICES];



int
dnx_field_tcam_hit_indication_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_tcam_hit_indication_tcam_action_hit_indication_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnx_field_tcam_hit_indication_tcam_action_hit_indication_small_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_hit_indication_tcam_action_hit_indication_dump(int  unit,  int  tcam_action_hit_indication_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_tcam_hit_indication_tcam_action_hit_indication_bank_line_id_dump(unit, tcam_action_hit_indication_idx_0, -1, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_hit_indication_tcam_action_hit_indication_bank_line_id_dump(int  unit,  int  tcam_action_hit_indication_idx_0,  int  bank_line_id_idx_0,  int  bank_line_id_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = tcam_action_hit_indication_idx_0, I0 = tcam_action_hit_indication_idx_0 + 1;
    int i1 = bank_line_id_idx_0, I1 = bank_line_id_idx_0 + 1, org_i1 = bank_line_id_idx_0;
    int i2 = bank_line_id_idx_1, I2 = bank_line_id_idx_1 + 1, org_i2 = bank_line_id_idx_1;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_hit_indication tcam_action_hit_indication bank_line_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_hit_indication tcam_action_hit_indication bank_line_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_hit_indication/tcam_action_hit_indication/bank_line_id.txt",
            "dnx_field_tcam_hit_indication[%d]->","((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->","tcam_action_hit_indication[].bank_line_id[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
                ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication
                , sizeof(*((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID, ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_hit_indication[]->((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[].bank_line_id[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID, ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[i0].bank_line_id);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
                    ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[i0].bank_line_id);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[i0].bank_line_id
                    , sizeof(*((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[i0].bank_line_id), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID, ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[i0].bank_line_id))
            {
                LOG_CLI((BSL_META("dnx_field_tcam_hit_indication[]->((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[].bank_line_id[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID, ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[i0].bank_line_id) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                
                
                    DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                        unit,
                        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[i0].bank_line_id[i1]);

                if (i2 == -1) {
                    I2 = dnx_sw_state_get_nof_elements(unit,
                        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
                        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[i0].bank_line_id[i1]);
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[i0].bank_line_id[i1]
                        , sizeof(*((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[i0].bank_line_id[i1]), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID, ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[i0].bank_line_id[i1]))
                {
                    LOG_CLI((BSL_META("dnx_field_tcam_hit_indication[]->((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[].bank_line_id[][]: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID, ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[i0].bank_line_id[i1]) == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_MONITOR(
                        unit,
                        "dnx_field_tcam_hit_indication[%d]->","((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->","tcam_action_hit_indication[%s%d].bank_line_id[%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_FILE(
                        unit,
                        "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    dnx_sw_state_print_uint8(
                        unit,
                        &((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[i0].bank_line_id[i1][i2]);

                }
                i2 = org_i2;
                dnx_sw_state_dump_end_of_stride(unit);
            }
            i1 = org_i1;
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_hit_indication_tcam_action_hit_indication_small_dump(int  unit,  int  tcam_action_hit_indication_small_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_tcam_hit_indication_tcam_action_hit_indication_small_bank_line_id_dump(unit, tcam_action_hit_indication_small_idx_0, -1, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_hit_indication_tcam_action_hit_indication_small_bank_line_id_dump(int  unit,  int  tcam_action_hit_indication_small_idx_0,  int  bank_line_id_idx_0,  int  bank_line_id_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = tcam_action_hit_indication_small_idx_0, I0 = tcam_action_hit_indication_small_idx_0 + 1;
    int i1 = bank_line_id_idx_0, I1 = bank_line_id_idx_0 + 1, org_i1 = bank_line_id_idx_0;
    int i2 = bank_line_id_idx_1, I2 = bank_line_id_idx_1 + 1, org_i2 = bank_line_id_idx_1;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_hit_indication tcam_action_hit_indication_small bank_line_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_hit_indication tcam_action_hit_indication_small bank_line_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_hit_indication/tcam_action_hit_indication_small/bank_line_id.txt",
            "dnx_field_tcam_hit_indication[%d]->","((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->","tcam_action_hit_indication_small[].bank_line_id[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
                ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small
                , sizeof(*((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID, ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_hit_indication[]->((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small[].bank_line_id[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID, ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small[i0].bank_line_id);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
                    ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small[i0].bank_line_id);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small[i0].bank_line_id
                    , sizeof(*((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small[i0].bank_line_id), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID, ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small[i0].bank_line_id))
            {
                LOG_CLI((BSL_META("dnx_field_tcam_hit_indication[]->((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small[].bank_line_id[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID, ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small[i0].bank_line_id) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                
                
                    DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                        unit,
                        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small[i0].bank_line_id[i1]);

                if (i2 == -1) {
                    I2 = dnx_sw_state_get_nof_elements(unit,
                        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
                        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small[i0].bank_line_id[i1]);
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small[i0].bank_line_id[i1]
                        , sizeof(*((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small[i0].bank_line_id[i1]), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID, ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small[i0].bank_line_id[i1]))
                {
                    LOG_CLI((BSL_META("dnx_field_tcam_hit_indication[]->((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small[].bank_line_id[][]: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID, ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small[i0].bank_line_id[i1]) == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_MONITOR(
                        unit,
                        "dnx_field_tcam_hit_indication[%d]->","((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->","tcam_action_hit_indication_small[%s%d].bank_line_id[%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_FILE(
                        unit,
                        "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    dnx_sw_state_print_uint8(
                        unit,
                        &((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small[i0].bank_line_id[i1][i2]);

                }
                i2 = org_i2;
                dnx_sw_state_dump_end_of_stride(unit);
            }
            i1 = org_i1;
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnx_field_tcam_hit_indication_info[SOC_MAX_NUM_DEVICES][DNX_FIELD_TCAM_HIT_INDICATION_INFO_NOF_ENTRIES];
const char* dnx_field_tcam_hit_indication_layout_str[DNX_FIELD_TCAM_HIT_INDICATION_INFO_NOF_ENTRIES] = {
    "DNX_FIELD_TCAM_HIT_INDICATION~",
    "DNX_FIELD_TCAM_HIT_INDICATION~TCAM_ACTION_HIT_INDICATION~",
    "DNX_FIELD_TCAM_HIT_INDICATION~TCAM_ACTION_HIT_INDICATION~BANK_LINE_ID~",
    "DNX_FIELD_TCAM_HIT_INDICATION~TCAM_ACTION_HIT_INDICATION_SMALL~",
    "DNX_FIELD_TCAM_HIT_INDICATION~TCAM_ACTION_HIT_INDICATION_SMALL~BANK_LINE_ID~",
};
#endif 
#undef BSL_LOG_MODULE
