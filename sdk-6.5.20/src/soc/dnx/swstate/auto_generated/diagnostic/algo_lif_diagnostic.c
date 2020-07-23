
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
#include <soc/dnx/swstate/auto_generated/diagnostic/algo_lif_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern local_outlif_info_t * local_outlif_info_data[SOC_MAX_NUM_DEVICES];



int
local_outlif_info_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(local_outlif_info_disable_mdb_clusters_dump(unit, filters));
    SHR_IF_ERR_EXIT(local_outlif_info_disable_eedb_data_banks_dump(unit, filters));
    SHR_IF_ERR_EXIT(local_outlif_info_outlif_bank_info_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(local_outlif_info_logical_phase_info_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(local_outlif_info_physical_phase_to_logical_phase_map_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(local_outlif_info_eedb_banks_info_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_disable_mdb_clusters_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "algo_lif disable_mdb_clusters") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate algo_lif disable_mdb_clusters\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "local_outlif_info/disable_mdb_clusters.txt",
            "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","disable_mdb_clusters: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","disable_mdb_clusters: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint8(
            unit,
            &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->disable_mdb_clusters);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_disable_eedb_data_banks_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "algo_lif disable_eedb_data_banks") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate algo_lif disable_eedb_data_banks\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "local_outlif_info/disable_eedb_data_banks.txt",
            "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","disable_eedb_data_banks: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","disable_eedb_data_banks: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint8(
            unit,
            &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->disable_eedb_data_banks);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_outlif_bank_info_dump(int  unit,  int  outlif_bank_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(local_outlif_info_outlif_bank_info_assigned_dump(unit, outlif_bank_info_idx_0, filters));
    SHR_IF_ERR_EXIT(local_outlif_info_outlif_bank_info_in_use_dump(unit, outlif_bank_info_idx_0, filters));
    SHR_IF_ERR_EXIT(local_outlif_info_outlif_bank_info_used_logical_phase_dump(unit, outlif_bank_info_idx_0, filters));
    SHR_IF_ERR_EXIT(local_outlif_info_outlif_bank_info_ll_in_use_dump(unit, outlif_bank_info_idx_0, filters));
    SHR_IF_ERR_EXIT(local_outlif_info_outlif_bank_info_ll_index_dump(unit, outlif_bank_info_idx_0, filters));
    SHR_IF_ERR_EXIT(local_outlif_info_outlif_bank_info_eedb_data_banks_dump(unit, outlif_bank_info_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_outlif_bank_info_assigned_dump(int  unit,  int  outlif_bank_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = outlif_bank_info_idx_0, I0 = outlif_bank_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "algo_lif outlif_bank_info assigned") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate algo_lif outlif_bank_info assigned\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "local_outlif_info/outlif_bank_info/assigned.txt",
            "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","outlif_bank_info[].assigned: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                ALGO_LIF_MODULE_ID,
                ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info
                , sizeof(*((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, ALGO_LIF_MODULE_ID, ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info))
        {
            LOG_CLI((BSL_META("local_outlif_info[]->((local_outlif_info_t*)sw_state_roots_array[][ALGO_LIF_MODULE_ID])->outlif_bank_info[].assigned: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, ALGO_LIF_MODULE_ID, ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","outlif_bank_info[%s%d].assigned: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info[i0].assigned);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_outlif_bank_info_in_use_dump(int  unit,  int  outlif_bank_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = outlif_bank_info_idx_0, I0 = outlif_bank_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "algo_lif outlif_bank_info in_use") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate algo_lif outlif_bank_info in_use\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "local_outlif_info/outlif_bank_info/in_use.txt",
            "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","outlif_bank_info[].in_use: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                ALGO_LIF_MODULE_ID,
                ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info
                , sizeof(*((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, ALGO_LIF_MODULE_ID, ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info))
        {
            LOG_CLI((BSL_META("local_outlif_info[]->((local_outlif_info_t*)sw_state_roots_array[][ALGO_LIF_MODULE_ID])->outlif_bank_info[].in_use: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, ALGO_LIF_MODULE_ID, ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","outlif_bank_info[%s%d].in_use: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info[i0].in_use);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_outlif_bank_info_used_logical_phase_dump(int  unit,  int  outlif_bank_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = outlif_bank_info_idx_0, I0 = outlif_bank_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "algo_lif outlif_bank_info used_logical_phase") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate algo_lif outlif_bank_info used_logical_phase\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "local_outlif_info/outlif_bank_info/used_logical_phase.txt",
            "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","outlif_bank_info[].used_logical_phase: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                ALGO_LIF_MODULE_ID,
                ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info
                , sizeof(*((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, ALGO_LIF_MODULE_ID, ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info))
        {
            LOG_CLI((BSL_META("local_outlif_info[]->((local_outlif_info_t*)sw_state_roots_array[][ALGO_LIF_MODULE_ID])->outlif_bank_info[].used_logical_phase: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, ALGO_LIF_MODULE_ID, ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","outlif_bank_info[%s%d].used_logical_phase: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info[i0].used_logical_phase);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_outlif_bank_info_ll_in_use_dump(int  unit,  int  outlif_bank_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = outlif_bank_info_idx_0, I0 = outlif_bank_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "algo_lif outlif_bank_info ll_in_use") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate algo_lif outlif_bank_info ll_in_use\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "local_outlif_info/outlif_bank_info/ll_in_use.txt",
            "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","outlif_bank_info[].ll_in_use: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                ALGO_LIF_MODULE_ID,
                ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info
                , sizeof(*((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, ALGO_LIF_MODULE_ID, ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info))
        {
            LOG_CLI((BSL_META("local_outlif_info[]->((local_outlif_info_t*)sw_state_roots_array[][ALGO_LIF_MODULE_ID])->outlif_bank_info[].ll_in_use: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, ALGO_LIF_MODULE_ID, ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","outlif_bank_info[%s%d].ll_in_use: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info[i0].ll_in_use);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_outlif_bank_info_ll_index_dump(int  unit,  int  outlif_bank_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = outlif_bank_info_idx_0, I0 = outlif_bank_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "algo_lif outlif_bank_info ll_index") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate algo_lif outlif_bank_info ll_index\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "local_outlif_info/outlif_bank_info/ll_index.txt",
            "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","outlif_bank_info[].ll_index: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                ALGO_LIF_MODULE_ID,
                ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info
                , sizeof(*((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, ALGO_LIF_MODULE_ID, ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info))
        {
            LOG_CLI((BSL_META("local_outlif_info[]->((local_outlif_info_t*)sw_state_roots_array[][ALGO_LIF_MODULE_ID])->outlif_bank_info[].ll_index: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, ALGO_LIF_MODULE_ID, ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","outlif_bank_info[%s%d].ll_index: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info[i0].ll_index);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_outlif_bank_info_eedb_data_banks_dump(int  unit,  int  outlif_bank_info_idx_0,  int  eedb_data_banks_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = outlif_bank_info_idx_0, I0 = outlif_bank_info_idx_0 + 1;
    int i1 = eedb_data_banks_idx_0, I1 = eedb_data_banks_idx_0 + 1, org_i1 = eedb_data_banks_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "algo_lif outlif_bank_info eedb_data_banks") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate algo_lif outlif_bank_info eedb_data_banks\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "local_outlif_info/outlif_bank_info/eedb_data_banks.txt",
            "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","outlif_bank_info[].eedb_data_banks[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                ALGO_LIF_MODULE_ID,
                ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info
                , sizeof(*((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, ALGO_LIF_MODULE_ID, ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info))
        {
            LOG_CLI((BSL_META("local_outlif_info[]->((local_outlif_info_t*)sw_state_roots_array[][ALGO_LIF_MODULE_ID])->outlif_bank_info[].eedb_data_banks[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, ALGO_LIF_MODULE_ID, ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = MAX_EEDB_BANKS_PER_OUTLIF_BANK;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info[i0].eedb_data_banks
                    , sizeof(*((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info[i0].eedb_data_banks), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= MAX_EEDB_BANKS_PER_OUTLIF_BANK)
            {
                LOG_CLI((BSL_META("local_outlif_info[]->((local_outlif_info_t*)sw_state_roots_array[][ALGO_LIF_MODULE_ID])->outlif_bank_info[].eedb_data_banks[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(MAX_EEDB_BANKS_PER_OUTLIF_BANK == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","outlif_bank_info[%s%d].eedb_data_banks[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info[i0].eedb_data_banks[i1]);

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
local_outlif_info_logical_phase_info_dump(int  unit,  int  logical_phase_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(local_outlif_info_logical_phase_info_physical_phase_dump(unit, logical_phase_info_idx_0, filters));
    SHR_IF_ERR_EXIT(local_outlif_info_logical_phase_info_address_granularity_dump(unit, logical_phase_info_idx_0, filters));
    SHR_IF_ERR_EXIT(local_outlif_info_logical_phase_info_first_bank_dump(unit, logical_phase_info_idx_0, filters));
    SHR_IF_ERR_EXIT(local_outlif_info_logical_phase_info_last_bank_dump(unit, logical_phase_info_idx_0, filters));
    SHR_IF_ERR_EXIT(local_outlif_info_logical_phase_info_first_ll_bank_dump(unit, logical_phase_info_idx_0, filters));
    SHR_IF_ERR_EXIT(local_outlif_info_logical_phase_info_last_ll_bank_dump(unit, logical_phase_info_idx_0, filters));
    SHR_IF_ERR_EXIT(local_outlif_info_logical_phase_info_dbal_valid_combinations_dump(unit, logical_phase_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_logical_phase_info_physical_phase_dump(int  unit,  int  logical_phase_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = logical_phase_info_idx_0, I0 = logical_phase_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "algo_lif logical_phase_info physical_phase") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate algo_lif logical_phase_info physical_phase\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "local_outlif_info/logical_phase_info/physical_phase.txt",
            "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","logical_phase_info[].physical_phase: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                ALGO_LIF_MODULE_ID,
                ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info
                , sizeof(*((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, ALGO_LIF_MODULE_ID, ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info))
        {
            LOG_CLI((BSL_META("local_outlif_info[]->((local_outlif_info_t*)sw_state_roots_array[][ALGO_LIF_MODULE_ID])->logical_phase_info[].physical_phase: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, ALGO_LIF_MODULE_ID, ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","logical_phase_info[%s%d].physical_phase: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[i0].physical_phase);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_logical_phase_info_address_granularity_dump(int  unit,  int  logical_phase_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = logical_phase_info_idx_0, I0 = logical_phase_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "algo_lif logical_phase_info address_granularity") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate algo_lif logical_phase_info address_granularity\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "local_outlif_info/logical_phase_info/address_granularity.txt",
            "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","logical_phase_info[].address_granularity: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                ALGO_LIF_MODULE_ID,
                ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info
                , sizeof(*((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, ALGO_LIF_MODULE_ID, ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info))
        {
            LOG_CLI((BSL_META("local_outlif_info[]->((local_outlif_info_t*)sw_state_roots_array[][ALGO_LIF_MODULE_ID])->logical_phase_info[].address_granularity: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, ALGO_LIF_MODULE_ID, ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","logical_phase_info[%s%d].address_granularity: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[i0].address_granularity);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_logical_phase_info_first_bank_dump(int  unit,  int  logical_phase_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = logical_phase_info_idx_0, I0 = logical_phase_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "algo_lif logical_phase_info first_bank") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate algo_lif logical_phase_info first_bank\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "local_outlif_info/logical_phase_info/first_bank.txt",
            "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","logical_phase_info[].first_bank: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                ALGO_LIF_MODULE_ID,
                ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info
                , sizeof(*((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, ALGO_LIF_MODULE_ID, ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info))
        {
            LOG_CLI((BSL_META("local_outlif_info[]->((local_outlif_info_t*)sw_state_roots_array[][ALGO_LIF_MODULE_ID])->logical_phase_info[].first_bank: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, ALGO_LIF_MODULE_ID, ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","logical_phase_info[%s%d].first_bank: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[i0].first_bank);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_logical_phase_info_last_bank_dump(int  unit,  int  logical_phase_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = logical_phase_info_idx_0, I0 = logical_phase_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "algo_lif logical_phase_info last_bank") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate algo_lif logical_phase_info last_bank\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "local_outlif_info/logical_phase_info/last_bank.txt",
            "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","logical_phase_info[].last_bank: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                ALGO_LIF_MODULE_ID,
                ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info
                , sizeof(*((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, ALGO_LIF_MODULE_ID, ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info))
        {
            LOG_CLI((BSL_META("local_outlif_info[]->((local_outlif_info_t*)sw_state_roots_array[][ALGO_LIF_MODULE_ID])->logical_phase_info[].last_bank: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, ALGO_LIF_MODULE_ID, ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","logical_phase_info[%s%d].last_bank: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[i0].last_bank);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_logical_phase_info_first_ll_bank_dump(int  unit,  int  logical_phase_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = logical_phase_info_idx_0, I0 = logical_phase_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "algo_lif logical_phase_info first_ll_bank") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate algo_lif logical_phase_info first_ll_bank\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "local_outlif_info/logical_phase_info/first_ll_bank.txt",
            "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","logical_phase_info[].first_ll_bank: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                ALGO_LIF_MODULE_ID,
                ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info
                , sizeof(*((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, ALGO_LIF_MODULE_ID, ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info))
        {
            LOG_CLI((BSL_META("local_outlif_info[]->((local_outlif_info_t*)sw_state_roots_array[][ALGO_LIF_MODULE_ID])->logical_phase_info[].first_ll_bank: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, ALGO_LIF_MODULE_ID, ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","logical_phase_info[%s%d].first_ll_bank: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[i0].first_ll_bank);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_logical_phase_info_last_ll_bank_dump(int  unit,  int  logical_phase_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = logical_phase_info_idx_0, I0 = logical_phase_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "algo_lif logical_phase_info last_ll_bank") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate algo_lif logical_phase_info last_ll_bank\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "local_outlif_info/logical_phase_info/last_ll_bank.txt",
            "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","logical_phase_info[].last_ll_bank: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                ALGO_LIF_MODULE_ID,
                ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info
                , sizeof(*((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, ALGO_LIF_MODULE_ID, ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info))
        {
            LOG_CLI((BSL_META("local_outlif_info[]->((local_outlif_info_t*)sw_state_roots_array[][ALGO_LIF_MODULE_ID])->logical_phase_info[].last_ll_bank: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, ALGO_LIF_MODULE_ID, ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","logical_phase_info[%s%d].last_ll_bank: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[i0].last_ll_bank);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_logical_phase_info_dbal_valid_combinations_dump(int  unit,  int  logical_phase_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = logical_phase_info_idx_0, I0 = logical_phase_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_htbl_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "algo_lif logical_phase_info dbal_valid_combinations") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate algo_lif logical_phase_info dbal_valid_combinations\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "local_outlif_info/logical_phase_info/dbal_valid_combinations.txt",
            "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","logical_phase_info[].dbal_valid_combinations: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                ALGO_LIF_MODULE_ID,
                ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info
                , sizeof(*((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, ALGO_LIF_MODULE_ID, ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info))
        {
            LOG_CLI((BSL_META("local_outlif_info[]->((local_outlif_info_t*)sw_state_roots_array[][ALGO_LIF_MODULE_ID])->logical_phase_info[].dbal_valid_combinations: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, ALGO_LIF_MODULE_ID, ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","logical_phase_info[%s%d].dbal_valid_combinations: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            SW_STATE_HTB_DEFAULT_PRINT(
                unit,
                ALGO_LIF_MODULE_ID,
                &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[i0].dbal_valid_combinations);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_physical_phase_to_logical_phase_map_dump(int  unit,  int  physical_phase_to_logical_phase_map_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = physical_phase_to_logical_phase_map_idx_0, I0 = physical_phase_to_logical_phase_map_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "algo_lif physical_phase_to_logical_phase_map") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate algo_lif physical_phase_to_logical_phase_map\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "local_outlif_info/physical_phase_to_logical_phase_map.txt",
            "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","physical_phase_to_logical_phase_map[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->physical_phase_to_logical_phase_map);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                ALGO_LIF_MODULE_ID,
                ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->physical_phase_to_logical_phase_map);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->physical_phase_to_logical_phase_map
                , sizeof(*((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->physical_phase_to_logical_phase_map), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, ALGO_LIF_MODULE_ID, ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->physical_phase_to_logical_phase_map))
        {
            LOG_CLI((BSL_META("local_outlif_info[]->((local_outlif_info_t*)sw_state_roots_array[][ALGO_LIF_MODULE_ID])->physical_phase_to_logical_phase_map[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, ALGO_LIF_MODULE_ID, ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->physical_phase_to_logical_phase_map) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","physical_phase_to_logical_phase_map[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->physical_phase_to_logical_phase_map[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_eedb_banks_info_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(local_outlif_info_eedb_banks_info_bank_occupation_bitmap_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(local_outlif_info_eedb_banks_info_outlif_prefix_occupation_bitmap_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_eedb_banks_info_bank_occupation_bitmap_dump(int  unit,  int  bank_occupation_bitmap_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = bank_occupation_bitmap_idx_0, I0 = bank_occupation_bitmap_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "SHR_BITDCL") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "algo_lif eedb_banks_info bank_occupation_bitmap") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate algo_lif eedb_banks_info bank_occupation_bitmap\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "local_outlif_info/eedb_banks_info/bank_occupation_bitmap.txt",
            "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","eedb_banks_info.bank_occupation_bitmap[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.bank_occupation_bitmap);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                ALGO_LIF_MODULE_ID,
                ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.bank_occupation_bitmap);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.bank_occupation_bitmap
                , sizeof(*((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.bank_occupation_bitmap), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, ALGO_LIF_MODULE_ID, ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.bank_occupation_bitmap))
        {
            LOG_CLI((BSL_META("local_outlif_info[]->((local_outlif_info_t*)sw_state_roots_array[][ALGO_LIF_MODULE_ID])->eedb_banks_info.bank_occupation_bitmap[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, ALGO_LIF_MODULE_ID, ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.bank_occupation_bitmap) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","eedb_banks_info.bank_occupation_bitmap[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_shr_bitdcl(
                unit,
                &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.bank_occupation_bitmap[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_eedb_banks_info_outlif_prefix_occupation_bitmap_dump(int  unit,  int  outlif_prefix_occupation_bitmap_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = outlif_prefix_occupation_bitmap_idx_0, I0 = outlif_prefix_occupation_bitmap_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "SHR_BITDCL") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "algo_lif eedb_banks_info outlif_prefix_occupation_bitmap") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate algo_lif eedb_banks_info outlif_prefix_occupation_bitmap\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "local_outlif_info/eedb_banks_info/outlif_prefix_occupation_bitmap.txt",
            "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","eedb_banks_info.outlif_prefix_occupation_bitmap[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.outlif_prefix_occupation_bitmap);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                ALGO_LIF_MODULE_ID,
                ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.outlif_prefix_occupation_bitmap);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.outlif_prefix_occupation_bitmap
                , sizeof(*((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.outlif_prefix_occupation_bitmap), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, ALGO_LIF_MODULE_ID, ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.outlif_prefix_occupation_bitmap))
        {
            LOG_CLI((BSL_META("local_outlif_info[]->((local_outlif_info_t*)sw_state_roots_array[][ALGO_LIF_MODULE_ID])->eedb_banks_info.outlif_prefix_occupation_bitmap[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, ALGO_LIF_MODULE_ID, ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.outlif_prefix_occupation_bitmap) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","eedb_banks_info.outlif_prefix_occupation_bitmap[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_shr_bitdcl(
                unit,
                &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.outlif_prefix_occupation_bitmap[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_multihead_ll_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "algo_lif eedb_banks_info used_data_bank_per_logical_phase") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate algo_lif eedb_banks_info used_data_bank_per_logical_phase\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "local_outlif_info/eedb_banks_info/used_data_bank_per_logical_phase.txt",
            "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","eedb_banks_info.used_data_bank_per_logical_phase: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "local_outlif_info[%d]->","((local_outlif_info_t*)sw_state_roots_array[%d][ALGO_LIF_MODULE_ID])->","eedb_banks_info.used_data_bank_per_logical_phase: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        SW_STATE_LL_DEFAULT_PRINT(
            unit,
            &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.used_data_bank_per_logical_phase);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t local_outlif_info_info[SOC_MAX_NUM_DEVICES][LOCAL_OUTLIF_INFO_INFO_NOF_ENTRIES];
const char* local_outlif_info_layout_str[LOCAL_OUTLIF_INFO_INFO_NOF_ENTRIES] = {
    "LOCAL_OUTLIF_INFO~",
    "LOCAL_OUTLIF_INFO~DISABLE_MDB_CLUSTERS~",
    "LOCAL_OUTLIF_INFO~DISABLE_EEDB_DATA_BANKS~",
    "LOCAL_OUTLIF_INFO~OUTLIF_BANK_INFO~",
    "LOCAL_OUTLIF_INFO~OUTLIF_BANK_INFO~ASSIGNED~",
    "LOCAL_OUTLIF_INFO~OUTLIF_BANK_INFO~IN_USE~",
    "LOCAL_OUTLIF_INFO~OUTLIF_BANK_INFO~USED_LOGICAL_PHASE~",
    "LOCAL_OUTLIF_INFO~OUTLIF_BANK_INFO~LL_IN_USE~",
    "LOCAL_OUTLIF_INFO~OUTLIF_BANK_INFO~LL_INDEX~",
    "LOCAL_OUTLIF_INFO~OUTLIF_BANK_INFO~EEDB_DATA_BANKS~",
    "LOCAL_OUTLIF_INFO~LOGICAL_PHASE_INFO~",
    "LOCAL_OUTLIF_INFO~LOGICAL_PHASE_INFO~PHYSICAL_PHASE~",
    "LOCAL_OUTLIF_INFO~LOGICAL_PHASE_INFO~ADDRESS_GRANULARITY~",
    "LOCAL_OUTLIF_INFO~LOGICAL_PHASE_INFO~FIRST_BANK~",
    "LOCAL_OUTLIF_INFO~LOGICAL_PHASE_INFO~LAST_BANK~",
    "LOCAL_OUTLIF_INFO~LOGICAL_PHASE_INFO~FIRST_LL_BANK~",
    "LOCAL_OUTLIF_INFO~LOGICAL_PHASE_INFO~LAST_LL_BANK~",
    "LOCAL_OUTLIF_INFO~LOGICAL_PHASE_INFO~DBAL_VALID_COMBINATIONS~",
    "LOCAL_OUTLIF_INFO~PHYSICAL_PHASE_TO_LOGICAL_PHASE_MAP~",
    "LOCAL_OUTLIF_INFO~EEDB_BANKS_INFO~",
    "LOCAL_OUTLIF_INFO~EEDB_BANKS_INFO~BANK_OCCUPATION_BITMAP~",
    "LOCAL_OUTLIF_INFO~EEDB_BANKS_INFO~OUTLIF_PREFIX_OCCUPATION_BITMAP~",
    "LOCAL_OUTLIF_INFO~EEDB_BANKS_INFO~USED_DATA_BANK_PER_LOGICAL_PHASE~",
};
#endif 
#undef BSL_LOG_MODULE
