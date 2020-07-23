
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
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_tcam_access_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_field_tcam_access_t * dnx_field_tcam_access_sw_data[SOC_MAX_NUM_DEVICES];



int
dnx_field_tcam_access_sw_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw_entry_location_hash_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw_valid_bmp_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw_entry_in_use_bmp_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw_fg_params_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_entry_location_hash_dump(int  unit,  int  entry_location_hash_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = entry_location_hash_idx_0, I0 = entry_location_hash_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_htbl_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_access entry_location_hash") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_access entry_location_hash\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_access_sw/entry_location_hash.txt",
            "dnx_field_tcam_access_sw[%d]->","((dnx_field_tcam_access_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->","entry_location_hash[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_ACCESS_MODULE_ID,
                ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash
                , sizeof(*((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MODULE_ID, ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_access_sw[]->((dnx_field_tcam_access_t*)sw_state_roots_array[][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MODULE_ID, ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_tcam_access_sw[%d]->","((dnx_field_tcam_access_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->","entry_location_hash[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            SW_STATE_HTB_DEFAULT_PRINT(
                unit,
                DNX_FIELD_TCAM_ACCESS_MODULE_ID,
                &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_valid_bmp_dump(int  unit,  int  valid_bmp_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw_valid_bmp_v_bit_dump(unit, valid_bmp_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_valid_bmp_v_bit_dump(int  unit,  int  valid_bmp_idx_0,  int  v_bit_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = valid_bmp_idx_0, I0 = valid_bmp_idx_0 + 1;
    int i1 = v_bit_idx_0, I1 = v_bit_idx_0 + 1, org_i1 = v_bit_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "SHR_BITDCL") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_access valid_bmp v_bit") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_access valid_bmp v_bit\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_access_sw/valid_bmp/v_bit.txt",
            "dnx_field_tcam_access_sw[%d]->","((dnx_field_tcam_access_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->","valid_bmp[].v_bit[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_ACCESS_MODULE_ID,
                ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp
                , sizeof(*((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MODULE_ID, ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_access_sw[]->((dnx_field_tcam_access_t*)sw_state_roots_array[][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[].v_bit[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MODULE_ID, ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[i0].v_bit);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_FIELD_TCAM_ACCESS_MODULE_ID,
                    ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[i0].v_bit);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[i0].v_bit
                    , sizeof(*((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[i0].v_bit), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MODULE_ID, ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[i0].v_bit))
            {
                LOG_CLI((BSL_META("dnx_field_tcam_access_sw[]->((dnx_field_tcam_access_t*)sw_state_roots_array[][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[].v_bit[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MODULE_ID, ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[i0].v_bit) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_field_tcam_access_sw[%d]->","((dnx_field_tcam_access_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->","valid_bmp[%s%d].v_bit[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_shr_bitdcl(
                    unit,
                    &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[i0].v_bit[i1]);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_entry_in_use_bmp_dump(int  unit,  int  entry_in_use_bmp_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_dump(unit, entry_in_use_bmp_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_dump(int  unit,  int  entry_in_use_bmp_idx_0,  int  v_bit_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = entry_in_use_bmp_idx_0, I0 = entry_in_use_bmp_idx_0 + 1;
    int i1 = v_bit_idx_0, I1 = v_bit_idx_0 + 1, org_i1 = v_bit_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "SHR_BITDCL") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_access entry_in_use_bmp v_bit") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_access entry_in_use_bmp v_bit\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_access_sw/entry_in_use_bmp/v_bit.txt",
            "dnx_field_tcam_access_sw[%d]->","((dnx_field_tcam_access_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->","entry_in_use_bmp[].v_bit[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_ACCESS_MODULE_ID,
                ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp
                , sizeof(*((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MODULE_ID, ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_access_sw[]->((dnx_field_tcam_access_t*)sw_state_roots_array[][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[].v_bit[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MODULE_ID, ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[i0].v_bit);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_FIELD_TCAM_ACCESS_MODULE_ID,
                    ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[i0].v_bit);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[i0].v_bit
                    , sizeof(*((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[i0].v_bit), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MODULE_ID, ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[i0].v_bit))
            {
                LOG_CLI((BSL_META("dnx_field_tcam_access_sw[]->((dnx_field_tcam_access_t*)sw_state_roots_array[][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[].v_bit[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MODULE_ID, ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[i0].v_bit) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_field_tcam_access_sw[%d]->","((dnx_field_tcam_access_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->","entry_in_use_bmp[%s%d].v_bit[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_shr_bitdcl(
                    unit,
                    &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[i0].v_bit[i1]);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_fg_params_dump(int  unit,  int  fg_params_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw_fg_params_key_size_dump(unit, fg_params_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw_fg_params_action_size_dump(unit, fg_params_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw_fg_params_actual_action_size_dump(unit, fg_params_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw_fg_params_stage_dump(unit, fg_params_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw_fg_params_prefix_size_dump(unit, fg_params_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw_fg_params_prefix_value_dump(unit, fg_params_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw_fg_params_direct_table_dump(unit, fg_params_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw_fg_params_dt_bank_id_dump(unit, fg_params_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw_fg_params_bank_allocation_mode_dump(unit, fg_params_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw_fg_params_context_sharing_handlers_cb_dump(unit, fg_params_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_fg_params_key_size_dump(int  unit,  int  fg_params_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = fg_params_idx_0, I0 = fg_params_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_access fg_params key_size") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_access fg_params key_size\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_access_sw/fg_params/key_size.txt",
            "dnx_field_tcam_access_sw[%d]->","((dnx_field_tcam_access_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->","fg_params[].key_size: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_ACCESS_MODULE_ID,
                ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params
                , sizeof(*((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MODULE_ID, ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_access_sw[]->((dnx_field_tcam_access_t*)sw_state_roots_array[][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[].key_size: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MODULE_ID, ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_tcam_access_sw[%d]->","((dnx_field_tcam_access_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->","fg_params[%s%d].key_size: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[i0].key_size);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_fg_params_action_size_dump(int  unit,  int  fg_params_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = fg_params_idx_0, I0 = fg_params_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_access fg_params action_size") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_access fg_params action_size\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_access_sw/fg_params/action_size.txt",
            "dnx_field_tcam_access_sw[%d]->","((dnx_field_tcam_access_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->","fg_params[].action_size: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_ACCESS_MODULE_ID,
                ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params
                , sizeof(*((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MODULE_ID, ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_access_sw[]->((dnx_field_tcam_access_t*)sw_state_roots_array[][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[].action_size: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MODULE_ID, ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_tcam_access_sw[%d]->","((dnx_field_tcam_access_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->","fg_params[%s%d].action_size: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[i0].action_size);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_fg_params_actual_action_size_dump(int  unit,  int  fg_params_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = fg_params_idx_0, I0 = fg_params_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_access fg_params actual_action_size") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_access fg_params actual_action_size\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_access_sw/fg_params/actual_action_size.txt",
            "dnx_field_tcam_access_sw[%d]->","((dnx_field_tcam_access_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->","fg_params[].actual_action_size: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_ACCESS_MODULE_ID,
                ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params
                , sizeof(*((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MODULE_ID, ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_access_sw[]->((dnx_field_tcam_access_t*)sw_state_roots_array[][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[].actual_action_size: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MODULE_ID, ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_tcam_access_sw[%d]->","((dnx_field_tcam_access_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->","fg_params[%s%d].actual_action_size: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[i0].actual_action_size);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_fg_params_stage_dump(int  unit,  int  fg_params_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = fg_params_idx_0, I0 = fg_params_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_tcam_stage_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_access fg_params stage") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_access fg_params stage\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_access_sw/fg_params/stage.txt",
            "dnx_field_tcam_access_sw[%d]->","((dnx_field_tcam_access_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->","fg_params[].stage: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_ACCESS_MODULE_ID,
                ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params
                , sizeof(*((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MODULE_ID, ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_access_sw[]->((dnx_field_tcam_access_t*)sw_state_roots_array[][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[].stage: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MODULE_ID, ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[i0].stage,
                "dnx_field_tcam_access_sw[%d]->","((dnx_field_tcam_access_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->","fg_params[%s%d].stage: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[i0].stage,
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_fg_params_prefix_size_dump(int  unit,  int  fg_params_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = fg_params_idx_0, I0 = fg_params_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_access fg_params prefix_size") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_access fg_params prefix_size\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_access_sw/fg_params/prefix_size.txt",
            "dnx_field_tcam_access_sw[%d]->","((dnx_field_tcam_access_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->","fg_params[].prefix_size: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_ACCESS_MODULE_ID,
                ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params
                , sizeof(*((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MODULE_ID, ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_access_sw[]->((dnx_field_tcam_access_t*)sw_state_roots_array[][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[].prefix_size: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MODULE_ID, ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_tcam_access_sw[%d]->","((dnx_field_tcam_access_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->","fg_params[%s%d].prefix_size: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[i0].prefix_size);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_fg_params_prefix_value_dump(int  unit,  int  fg_params_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = fg_params_idx_0, I0 = fg_params_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_access fg_params prefix_value") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_access fg_params prefix_value\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_access_sw/fg_params/prefix_value.txt",
            "dnx_field_tcam_access_sw[%d]->","((dnx_field_tcam_access_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->","fg_params[].prefix_value: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_ACCESS_MODULE_ID,
                ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params
                , sizeof(*((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MODULE_ID, ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_access_sw[]->((dnx_field_tcam_access_t*)sw_state_roots_array[][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[].prefix_value: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MODULE_ID, ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_tcam_access_sw[%d]->","((dnx_field_tcam_access_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->","fg_params[%s%d].prefix_value: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[i0].prefix_value);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_fg_params_direct_table_dump(int  unit,  int  fg_params_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = fg_params_idx_0, I0 = fg_params_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_access fg_params direct_table") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_access fg_params direct_table\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_access_sw/fg_params/direct_table.txt",
            "dnx_field_tcam_access_sw[%d]->","((dnx_field_tcam_access_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->","fg_params[].direct_table: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_ACCESS_MODULE_ID,
                ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params
                , sizeof(*((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MODULE_ID, ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_access_sw[]->((dnx_field_tcam_access_t*)sw_state_roots_array[][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[].direct_table: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MODULE_ID, ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_tcam_access_sw[%d]->","((dnx_field_tcam_access_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->","fg_params[%s%d].direct_table: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[i0].direct_table);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_fg_params_dt_bank_id_dump(int  unit,  int  fg_params_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = fg_params_idx_0, I0 = fg_params_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_access fg_params dt_bank_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_access fg_params dt_bank_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_access_sw/fg_params/dt_bank_id.txt",
            "dnx_field_tcam_access_sw[%d]->","((dnx_field_tcam_access_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->","fg_params[].dt_bank_id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_ACCESS_MODULE_ID,
                ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params
                , sizeof(*((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MODULE_ID, ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_access_sw[]->((dnx_field_tcam_access_t*)sw_state_roots_array[][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[].dt_bank_id: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MODULE_ID, ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_tcam_access_sw[%d]->","((dnx_field_tcam_access_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->","fg_params[%s%d].dt_bank_id: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[i0].dt_bank_id);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_fg_params_bank_allocation_mode_dump(int  unit,  int  fg_params_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = fg_params_idx_0, I0 = fg_params_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_tcam_bank_allocation_mode_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_access fg_params bank_allocation_mode") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_access fg_params bank_allocation_mode\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_access_sw/fg_params/bank_allocation_mode.txt",
            "dnx_field_tcam_access_sw[%d]->","((dnx_field_tcam_access_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->","fg_params[].bank_allocation_mode: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_ACCESS_MODULE_ID,
                ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params
                , sizeof(*((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MODULE_ID, ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_access_sw[]->((dnx_field_tcam_access_t*)sw_state_roots_array[][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[].bank_allocation_mode: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MODULE_ID, ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[i0].bank_allocation_mode,
                "dnx_field_tcam_access_sw[%d]->","((dnx_field_tcam_access_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->","fg_params[%s%d].bank_allocation_mode: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[i0].bank_allocation_mode,
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_fg_params_context_sharing_handlers_cb_dump(int  unit,  int  fg_params_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = fg_params_idx_0, I0 = fg_params_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_tcam_context_sharing_handlers_get_p") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_access fg_params context_sharing_handlers_cb") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_access fg_params context_sharing_handlers_cb\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_access_sw/fg_params/context_sharing_handlers_cb.txt",
            "dnx_field_tcam_access_sw[%d]->","((dnx_field_tcam_access_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->","fg_params[].context_sharing_handlers_cb: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_ACCESS_MODULE_ID,
                ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params
                , sizeof(*((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MODULE_ID, ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_access_sw[]->((dnx_field_tcam_access_t*)sw_state_roots_array[][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[].context_sharing_handlers_cb: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MODULE_ID, ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[i0].context_sharing_handlers_cb,
                "dnx_field_tcam_access_sw[%d]->","((dnx_field_tcam_access_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->","fg_params[%s%d].context_sharing_handlers_cb: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[i0].context_sharing_handlers_cb,
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnx_field_tcam_access_sw_info[SOC_MAX_NUM_DEVICES][DNX_FIELD_TCAM_ACCESS_SW_INFO_NOF_ENTRIES];
const char* dnx_field_tcam_access_sw_layout_str[DNX_FIELD_TCAM_ACCESS_SW_INFO_NOF_ENTRIES] = {
    "DNX_FIELD_TCAM_ACCESS_SW~",
    "DNX_FIELD_TCAM_ACCESS_SW~ENTRY_LOCATION_HASH~",
    "DNX_FIELD_TCAM_ACCESS_SW~VALID_BMP~",
    "DNX_FIELD_TCAM_ACCESS_SW~VALID_BMP~V_BIT~",
    "DNX_FIELD_TCAM_ACCESS_SW~ENTRY_IN_USE_BMP~",
    "DNX_FIELD_TCAM_ACCESS_SW~ENTRY_IN_USE_BMP~V_BIT~",
    "DNX_FIELD_TCAM_ACCESS_SW~FG_PARAMS~",
    "DNX_FIELD_TCAM_ACCESS_SW~FG_PARAMS~KEY_SIZE~",
    "DNX_FIELD_TCAM_ACCESS_SW~FG_PARAMS~ACTION_SIZE~",
    "DNX_FIELD_TCAM_ACCESS_SW~FG_PARAMS~ACTUAL_ACTION_SIZE~",
    "DNX_FIELD_TCAM_ACCESS_SW~FG_PARAMS~STAGE~",
    "DNX_FIELD_TCAM_ACCESS_SW~FG_PARAMS~PREFIX_SIZE~",
    "DNX_FIELD_TCAM_ACCESS_SW~FG_PARAMS~PREFIX_VALUE~",
    "DNX_FIELD_TCAM_ACCESS_SW~FG_PARAMS~DIRECT_TABLE~",
    "DNX_FIELD_TCAM_ACCESS_SW~FG_PARAMS~DT_BANK_ID~",
    "DNX_FIELD_TCAM_ACCESS_SW~FG_PARAMS~BANK_ALLOCATION_MODE~",
    "DNX_FIELD_TCAM_ACCESS_SW~FG_PARAMS~CONTEXT_SHARING_HANDLERS_CB~",
};
#endif 
#undef BSL_LOG_MODULE
