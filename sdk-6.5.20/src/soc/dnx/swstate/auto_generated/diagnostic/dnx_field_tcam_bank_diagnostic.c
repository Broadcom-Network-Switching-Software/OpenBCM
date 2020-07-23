
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
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_tcam_bank_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_field_tcam_bank_sw_t * dnx_field_tcam_bank_sw_data[SOC_MAX_NUM_DEVICES];



int
dnx_field_tcam_bank_sw_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw_tcam_banks_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_bank_sw_tcam_banks_dump(int  unit,  int  tcam_banks_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw_tcam_banks_owner_stage_dump(unit, tcam_banks_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_dump(unit, tcam_banks_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw_tcam_banks_nof_free_entries_dump(unit, tcam_banks_idx_0, -1, -1, filters));
    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw_tcam_banks_bank_mode_dump(unit, tcam_banks_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_bank_sw_tcam_banks_owner_stage_dump(int  unit,  int  tcam_banks_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = tcam_banks_idx_0, I0 = tcam_banks_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_tcam_stage_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_bank tcam_banks owner_stage") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_bank tcam_banks owner_stage\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_bank_sw/tcam_banks/owner_stage.txt",
            "dnx_field_tcam_bank_sw[%d]->","((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_BANK_MODULE_ID])->","tcam_banks[].owner_stage: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_BANK_MODULE_ID,
                ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks
                , sizeof(*((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_BANK_MODULE_ID, ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_bank_sw[]->((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[].owner_stage: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_BANK_MODULE_ID, ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[i0].owner_stage,
                "dnx_field_tcam_bank_sw[%d]->","((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_BANK_MODULE_ID])->","tcam_banks[%s%d].owner_stage: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[i0].owner_stage,
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_dump(int  unit,  int  tcam_banks_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = tcam_banks_idx_0, I0 = tcam_banks_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_ll_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_bank tcam_banks active_handlers_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_bank tcam_banks active_handlers_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_bank_sw/tcam_banks/active_handlers_id.txt",
            "dnx_field_tcam_bank_sw[%d]->","((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_BANK_MODULE_ID])->","tcam_banks[].active_handlers_id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_BANK_MODULE_ID,
                ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks
                , sizeof(*((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_BANK_MODULE_ID, ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_bank_sw[]->((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[].active_handlers_id: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_BANK_MODULE_ID, ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_tcam_bank_sw[%d]->","((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_BANK_MODULE_ID])->","tcam_banks[%s%d].active_handlers_id: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            SW_STATE_LL_DEFAULT_PRINT(
                unit,
                &((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[i0].active_handlers_id);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_bank_sw_tcam_banks_nof_free_entries_dump(int  unit,  int  tcam_banks_idx_0,  int  nof_free_entries_idx_0,  int  nof_free_entries_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = tcam_banks_idx_0, I0 = tcam_banks_idx_0 + 1;
    int i1 = nof_free_entries_idx_0, I1 = nof_free_entries_idx_0 + 1, org_i1 = nof_free_entries_idx_0;
    int i2 = nof_free_entries_idx_1, I2 = nof_free_entries_idx_1 + 1, org_i2 = nof_free_entries_idx_1;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_bank tcam_banks nof_free_entries") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_bank tcam_banks nof_free_entries\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_bank_sw/tcam_banks/nof_free_entries.txt",
            "dnx_field_tcam_bank_sw[%d]->","((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_BANK_MODULE_ID])->","tcam_banks[].nof_free_entries[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_BANK_MODULE_ID,
                ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks
                , sizeof(*((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_BANK_MODULE_ID, ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_bank_sw[]->((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[].nof_free_entries[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_BANK_MODULE_ID, ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[i0].nof_free_entries);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_FIELD_TCAM_BANK_MODULE_ID,
                    ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[i0].nof_free_entries);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[i0].nof_free_entries
                    , sizeof(*((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[i0].nof_free_entries), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_BANK_MODULE_ID, ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[i0].nof_free_entries))
            {
                LOG_CLI((BSL_META("dnx_field_tcam_bank_sw[]->((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[].nof_free_entries[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_BANK_MODULE_ID, ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[i0].nof_free_entries) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                
                
                    DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                        unit,
                        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[i0].nof_free_entries[i1]);

                if (i2 == -1) {
                    I2 = dnx_sw_state_get_nof_elements(unit,
                        DNX_FIELD_TCAM_BANK_MODULE_ID,
                        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[i0].nof_free_entries[i1]);
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[i0].nof_free_entries[i1]
                        , sizeof(*((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[i0].nof_free_entries[i1]), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_BANK_MODULE_ID, ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[i0].nof_free_entries[i1]))
                {
                    LOG_CLI((BSL_META("dnx_field_tcam_bank_sw[]->((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[].nof_free_entries[][]: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_BANK_MODULE_ID, ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[i0].nof_free_entries[i1]) == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_MONITOR(
                        unit,
                        "dnx_field_tcam_bank_sw[%d]->","((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_BANK_MODULE_ID])->","tcam_banks[%s%d].nof_free_entries[%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_FILE(
                        unit,
                        "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    dnx_sw_state_print_uint32(
                        unit,
                        &((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[i0].nof_free_entries[i1][i2]);

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
dnx_field_tcam_bank_sw_tcam_banks_bank_mode_dump(int  unit,  int  tcam_banks_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = tcam_banks_idx_0, I0 = tcam_banks_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_tcam_bank_mode_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_bank tcam_banks bank_mode") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_bank tcam_banks bank_mode\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_bank_sw/tcam_banks/bank_mode.txt",
            "dnx_field_tcam_bank_sw[%d]->","((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_BANK_MODULE_ID])->","tcam_banks[].bank_mode: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_BANK_MODULE_ID,
                ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks
                , sizeof(*((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_BANK_MODULE_ID, ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_bank_sw[]->((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[].bank_mode: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_BANK_MODULE_ID, ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[i0].bank_mode,
                "dnx_field_tcam_bank_sw[%d]->","((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_BANK_MODULE_ID])->","tcam_banks[%s%d].bank_mode: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[i0].bank_mode,
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnx_field_tcam_bank_sw_info[SOC_MAX_NUM_DEVICES][DNX_FIELD_TCAM_BANK_SW_INFO_NOF_ENTRIES];
const char* dnx_field_tcam_bank_sw_layout_str[DNX_FIELD_TCAM_BANK_SW_INFO_NOF_ENTRIES] = {
    "DNX_FIELD_TCAM_BANK_SW~",
    "DNX_FIELD_TCAM_BANK_SW~TCAM_BANKS~",
    "DNX_FIELD_TCAM_BANK_SW~TCAM_BANKS~OWNER_STAGE~",
    "DNX_FIELD_TCAM_BANK_SW~TCAM_BANKS~ACTIVE_HANDLERS_ID~",
    "DNX_FIELD_TCAM_BANK_SW~TCAM_BANKS~NOF_FREE_ENTRIES~",
    "DNX_FIELD_TCAM_BANK_SW~TCAM_BANKS~BANK_MODE~",
};
#endif 
#undef BSL_LOG_MODULE
