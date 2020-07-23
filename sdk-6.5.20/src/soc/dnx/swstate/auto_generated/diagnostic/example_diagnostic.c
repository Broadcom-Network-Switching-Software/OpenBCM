
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#if defined(INCLUDE_CTEST)
#include <soc/dnxc/swstate/dnxc_sw_state_c_includes.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/example_diagnostic.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_module_testing.h>
#include <soc/dnxc/swstate/dnx_sw_state_pretty_dump.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern sw_state_example * example_data[SOC_MAX_NUM_DEVICES];



int
example_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(example_my_variable_dump(unit, filters));
    SHR_IF_ERR_EXIT(example_rollback_comparison_excluded_dump(unit, filters));
    SHR_IF_ERR_EXIT(example_my_array_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(example_two_dimentional_array_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(example_dynamic_array_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(example_dynamic_array_zero_size_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(example_dynamic_dynamic_array_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(example_dynamic_array_static_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(example_counter_test_dump(unit, filters));
    SHR_IF_ERR_EXIT(example_mutex_test_dump(unit, filters));
    SHR_IF_ERR_EXIT(example_sem_test_dump(unit, filters));
    SHR_IF_ERR_EXIT(example_DNXData_array_alloc_exception_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(example_DNXData_array_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(example_DNXData_array_dyn_dnxdata_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(example_DNXData_array_dnxdata_dyn_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(example_DNXData_array_dnxdata_dnxdata_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(example_DNXData_array_static_dnxdata_dyn_dump(unit, -1, -1, -1, filters));
    SHR_IF_ERR_EXIT(example_DNXData_array_static_dyn_dnxdata_dump(unit, -1, -1, -1, filters));
    SHR_IF_ERR_EXIT(example_DNXData_array_static_dnxdata_dnxdata_dump(unit, -1, -1, -1, filters));
    SHR_IF_ERR_EXIT(example_DNXData_array_table_data_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(example_value_range_test_dump(unit, filters));
    SHR_IF_ERR_EXIT(example_array_range_example_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(example_alloc_after_init_variable_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(example_alloc_after_init_test_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(example_pointer_dump(unit, filters));
    SHR_IF_ERR_EXIT(example_bitmap_variable_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(example_pbmp_variable_dump(unit, filters));
    SHR_IF_ERR_EXIT(example_bitmap_fixed_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(example_bitmap_dnx_data_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(example_buffer_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(example_buffer_fixed_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(example_bitmap_exclude_example_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(example_default_value_tree_dump(unit, filters));
    SHR_IF_ERR_EXIT(example_ll_dump(unit, filters));
    SHR_IF_ERR_EXIT(example_multihead_ll_dump(unit, filters));
    SHR_IF_ERR_EXIT(example_sorted_ll_dump(unit, filters));
    SHR_IF_ERR_EXIT(example_bt_dump(unit, filters));
    SHR_IF_ERR_EXIT(example_htb_dump(unit, filters));
    SHR_IF_ERR_EXIT(example_htb_arr_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(example_index_htb_dump(unit, filters));
    SHR_IF_ERR_EXIT(example_occ_dump(unit, filters));
    SHR_IF_ERR_EXIT(example_cb_dump(unit, filters));
    SHR_IF_ERR_EXIT(example_leaf_struct_dump(unit, filters));
    SHR_IF_ERR_EXIT(example_defragmented_chunk_example_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(example_packed_dump(unit, filters));
    SHR_IF_ERR_EXIT(example_my_union_dump(unit, filters));
    SHR_IF_ERR_EXIT(example_string_dump(unit, filters));
    SHR_IF_ERR_EXIT(example_alloc_child_check_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(example_write_during_wb_example_dump(unit, filters));
    SHR_IF_ERR_EXIT(example_large_DS_examples_dump(unit, filters));
    SHR_IF_ERR_EXIT(example_pretty_print_example_dump(unit, filters));
    SHR_IF_ERR_EXIT(example_buffer_array_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(example_bitmap_array_dump(unit, -1, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_my_variable_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}






int
example_rollback_comparison_excluded_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_my_array_dump(int  unit,  int  my_array_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = my_array_idx_0, I0 = my_array_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example my_array") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example my_array\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/my_array.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","my_array[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        if (i0 == -1) {
            I0 = 10;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->my_array
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->my_array), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 10)
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->my_array[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(10 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","my_array[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->my_array[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_two_dimentional_array_dump(int  unit,  int  index_1,  int  index_2,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = index_1, I0 = index_1 + 1;
    int i1 = index_2, I1 = index_2 + 1, org_i1 = index_2;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example two_dimentional_array") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example two_dimentional_array\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/two_dimentional_array.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","two_dimentional_array[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        if (i0 == -1) {
            I0 = 10;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->two_dimentional_array
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->two_dimentional_array), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 10)
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->two_dimentional_array[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(10 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = 10;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->two_dimentional_array[i0]
                    , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->two_dimentional_array[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= 10)
            {
                LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->two_dimentional_array[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(10 == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","two_dimentional_array[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->two_dimentional_array[i0][i1]);

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
example_dynamic_array_dump(int  unit,  int  dynamic_array_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = dynamic_array_idx_0, I0 = dynamic_array_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example dynamic_array") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example dynamic_array\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/dynamic_array.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","dynamic_array[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                EXAMPLE_MODULE_ID,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array))
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->dynamic_array[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","dynamic_array[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_dynamic_array_zero_size_dump(int  unit,  int  dynamic_array_zero_size_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = dynamic_array_zero_size_idx_0, I0 = dynamic_array_zero_size_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example dynamic_array_zero_size") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example dynamic_array_zero_size\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/dynamic_array_zero_size.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","dynamic_array_zero_size[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_zero_size);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                EXAMPLE_MODULE_ID,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_zero_size);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_zero_size
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_zero_size), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_zero_size))
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->dynamic_array_zero_size[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_zero_size) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","dynamic_array_zero_size[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_zero_size[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_dynamic_dynamic_array_dump(int  unit,  int  first_d_index,  int  dynamic_index,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = first_d_index, I0 = first_d_index + 1;
    int i1 = dynamic_index, I1 = dynamic_index + 1, org_i1 = dynamic_index;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example dynamic_dynamic_array") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example dynamic_dynamic_array\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/dynamic_dynamic_array.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","dynamic_dynamic_array[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                EXAMPLE_MODULE_ID,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array))
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->dynamic_dynamic_array[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    EXAMPLE_MODULE_ID,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array[i0]
                    , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array[i0]))
            {
                LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->dynamic_dynamic_array[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","dynamic_dynamic_array[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array[i0][i1]);

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
example_dynamic_array_static_dump(int  unit,  int  dynamic_index,  int  static_index,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = dynamic_index, I0 = dynamic_index + 1;
    int i1 = static_index, I1 = static_index + 1, org_i1 = static_index;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example dynamic_array_static") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example dynamic_array_static\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/dynamic_array_static.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","dynamic_array_static[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_static);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                EXAMPLE_MODULE_ID,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_static);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_static
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_static), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_static))
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->dynamic_array_static[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_static) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = 5;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_static[i0]
                    , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_static[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= 5)
            {
                LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->dynamic_array_static[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(5 == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","dynamic_array_static[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_static[i0][i1]);

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
example_counter_test_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example counter_test") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example counter_test\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/counter_test.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","counter_test: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","counter_test: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint32(
            unit,
            &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->counter_test);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_mutex_test_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_mutex_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example mutex_test") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example mutex_test\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/mutex_test.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","mutex_test: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","mutex_test: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_mutex(
            unit,
            &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->mutex_test);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_sem_test_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_sem_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example sem_test") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example sem_test\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/sem_test.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","sem_test: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","sem_test: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_sem(
            unit,
            &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sem_test);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_DNXData_array_alloc_exception_dump(int  unit,  int  DNXData_array_alloc_exception_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = DNXData_array_alloc_exception_idx_0, I0 = DNXData_array_alloc_exception_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example DNXData_array_alloc_exception") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example DNXData_array_alloc_exception\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/DNXData_array_alloc_exception.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","DNXData_array_alloc_exception[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_alloc_exception);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                EXAMPLE_MODULE_ID,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_alloc_exception);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_alloc_exception
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_alloc_exception), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_alloc_exception))
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->DNXData_array_alloc_exception[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_alloc_exception) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","DNXData_array_alloc_exception[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_alloc_exception[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_DNXData_array_dump(int  unit,  int  DNXData_array_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = DNXData_array_idx_0, I0 = DNXData_array_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example DNXData_array") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example DNXData_array\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/DNXData_array.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","DNXData_array[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                EXAMPLE_MODULE_ID,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array))
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->DNXData_array[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","DNXData_array[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_DNXData_array_dyn_dnxdata_dump(int  unit,  int  DNXData_array_dyn_dnxdata_idx_0,  int  DNXData_array_dyn_dnxdata_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = DNXData_array_dyn_dnxdata_idx_0, I0 = DNXData_array_dyn_dnxdata_idx_0 + 1;
    int i1 = DNXData_array_dyn_dnxdata_idx_1, I1 = DNXData_array_dyn_dnxdata_idx_1 + 1, org_i1 = DNXData_array_dyn_dnxdata_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example DNXData_array_dyn_dnxdata") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example DNXData_array_dyn_dnxdata\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/DNXData_array_dyn_dnxdata.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","DNXData_array_dyn_dnxdata[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                EXAMPLE_MODULE_ID,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata))
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    EXAMPLE_MODULE_ID,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata[i0]
                    , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata[i0]))
            {
                LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","DNXData_array_dyn_dnxdata[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata[i0][i1]);

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
example_DNXData_array_dnxdata_dyn_dump(int  unit,  int  DNXData_array_dnxdata_dyn_idx_0,  int  DNXData_array_dnxdata_dyn_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = DNXData_array_dnxdata_dyn_idx_0, I0 = DNXData_array_dnxdata_dyn_idx_0 + 1;
    int i1 = DNXData_array_dnxdata_dyn_idx_1, I1 = DNXData_array_dnxdata_dyn_idx_1 + 1, org_i1 = DNXData_array_dnxdata_dyn_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example DNXData_array_dnxdata_dyn") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example DNXData_array_dnxdata_dyn\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/DNXData_array_dnxdata_dyn.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","DNXData_array_dnxdata_dyn[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                EXAMPLE_MODULE_ID,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn))
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    EXAMPLE_MODULE_ID,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn[i0]
                    , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn[i0]))
            {
                LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","DNXData_array_dnxdata_dyn[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn[i0][i1]);

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
example_DNXData_array_dnxdata_dnxdata_dump(int  unit,  int  DNXData_array_dnxdata_dnxdata_idx_0,  int  DNXData_array_dnxdata_dnxdata_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = DNXData_array_dnxdata_dnxdata_idx_0, I0 = DNXData_array_dnxdata_dnxdata_idx_0 + 1;
    int i1 = DNXData_array_dnxdata_dnxdata_idx_1, I1 = DNXData_array_dnxdata_dnxdata_idx_1 + 1, org_i1 = DNXData_array_dnxdata_dnxdata_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example DNXData_array_dnxdata_dnxdata") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example DNXData_array_dnxdata_dnxdata\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/DNXData_array_dnxdata_dnxdata.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","DNXData_array_dnxdata_dnxdata[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                EXAMPLE_MODULE_ID,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata))
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    EXAMPLE_MODULE_ID,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata[i0]
                    , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata[i0]))
            {
                LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","DNXData_array_dnxdata_dnxdata[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata[i0][i1]);

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
example_DNXData_array_static_dnxdata_dyn_dump(int  unit,  int  DNXData_array_static_dnxdata_dyn_idx_0,  int  DNXData_array_static_dnxdata_dyn_idx_1,  int  DNXData_array_static_dnxdata_dyn_idx_2,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = DNXData_array_static_dnxdata_dyn_idx_0, I0 = DNXData_array_static_dnxdata_dyn_idx_0 + 1;
    int i1 = DNXData_array_static_dnxdata_dyn_idx_1, I1 = DNXData_array_static_dnxdata_dyn_idx_1 + 1, org_i1 = DNXData_array_static_dnxdata_dyn_idx_1;
    int i2 = DNXData_array_static_dnxdata_dyn_idx_2, I2 = DNXData_array_static_dnxdata_dyn_idx_2 + 1, org_i2 = DNXData_array_static_dnxdata_dyn_idx_2;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example DNXData_array_static_dnxdata_dyn") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example DNXData_array_static_dnxdata_dyn\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/DNXData_array_static_dnxdata_dyn.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","DNXData_array_static_dnxdata_dyn[][][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        if (i0 == -1) {
            I0 = 3;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 3)
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[][][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(3 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    EXAMPLE_MODULE_ID,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[i0]
                    , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[i0]))
            {
                LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[][][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                
                
                    DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                        unit,
                        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[i0][i1]);

                if (i2 == -1) {
                    I2 = dnx_sw_state_get_nof_elements(unit,
                        EXAMPLE_MODULE_ID,
                        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[i0][i1]);
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[i0][i1]
                        , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[i0][i1]), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[i0][i1]))
                {
                    LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[][][]: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[i0][i1]) == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_MONITOR(
                        unit,
                        "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","DNXData_array_static_dnxdata_dyn[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_FILE(
                        unit,
                        "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    dnx_sw_state_print_uint32(
                        unit,
                        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[i0][i1][i2]);

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
example_DNXData_array_static_dyn_dnxdata_dump(int  unit,  int  DNXData_array_static_dyn_dnxdata_idx_0,  int  DNXData_array_static_dyn_dnxdata_idx_1,  int  DNXData_array_static_dyn_dnxdata_idx_2,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = DNXData_array_static_dyn_dnxdata_idx_0, I0 = DNXData_array_static_dyn_dnxdata_idx_0 + 1;
    int i1 = DNXData_array_static_dyn_dnxdata_idx_1, I1 = DNXData_array_static_dyn_dnxdata_idx_1 + 1, org_i1 = DNXData_array_static_dyn_dnxdata_idx_1;
    int i2 = DNXData_array_static_dyn_dnxdata_idx_2, I2 = DNXData_array_static_dyn_dnxdata_idx_2 + 1, org_i2 = DNXData_array_static_dyn_dnxdata_idx_2;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example DNXData_array_static_dyn_dnxdata") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example DNXData_array_static_dyn_dnxdata\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/DNXData_array_static_dyn_dnxdata.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","DNXData_array_static_dyn_dnxdata[][][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        if (i0 == -1) {
            I0 = 3;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 3)
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[][][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(3 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    EXAMPLE_MODULE_ID,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[i0]
                    , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[i0]))
            {
                LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[][][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                
                
                    DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                        unit,
                        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[i0][i1]);

                if (i2 == -1) {
                    I2 = dnx_sw_state_get_nof_elements(unit,
                        EXAMPLE_MODULE_ID,
                        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[i0][i1]);
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[i0][i1]
                        , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[i0][i1]), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[i0][i1]))
                {
                    LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[][][]: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[i0][i1]) == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_MONITOR(
                        unit,
                        "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","DNXData_array_static_dyn_dnxdata[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_FILE(
                        unit,
                        "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    dnx_sw_state_print_uint32(
                        unit,
                        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[i0][i1][i2]);

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
example_DNXData_array_static_dnxdata_dnxdata_dump(int  unit,  int  DNXData_array_static_dnxdata_dnxdata_idx_0,  int  DNXData_array_static_dnxdata_dnxdata_idx_1,  int  DNXData_array_static_dnxdata_dnxdata_idx_2,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = DNXData_array_static_dnxdata_dnxdata_idx_0, I0 = DNXData_array_static_dnxdata_dnxdata_idx_0 + 1;
    int i1 = DNXData_array_static_dnxdata_dnxdata_idx_1, I1 = DNXData_array_static_dnxdata_dnxdata_idx_1 + 1, org_i1 = DNXData_array_static_dnxdata_dnxdata_idx_1;
    int i2 = DNXData_array_static_dnxdata_dnxdata_idx_2, I2 = DNXData_array_static_dnxdata_dnxdata_idx_2 + 1, org_i2 = DNXData_array_static_dnxdata_dnxdata_idx_2;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example DNXData_array_static_dnxdata_dnxdata") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example DNXData_array_static_dnxdata_dnxdata\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/DNXData_array_static_dnxdata_dnxdata.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","DNXData_array_static_dnxdata_dnxdata[][][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        if (i0 == -1) {
            I0 = 3;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 3)
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[][][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(3 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    EXAMPLE_MODULE_ID,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[i0]
                    , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[i0]))
            {
                LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[][][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                
                
                    DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                        unit,
                        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[i0][i1]);

                if (i2 == -1) {
                    I2 = dnx_sw_state_get_nof_elements(unit,
                        EXAMPLE_MODULE_ID,
                        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[i0][i1]);
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[i0][i1]
                        , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[i0][i1]), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[i0][i1]))
                {
                    LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[][][]: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[i0][i1]) == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_MONITOR(
                        unit,
                        "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","DNXData_array_static_dnxdata_dnxdata[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_FILE(
                        unit,
                        "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    dnx_sw_state_print_uint32(
                        unit,
                        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[i0][i1][i2]);

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
example_DNXData_array_table_data_dump(int  unit,  int  DNXData_array_table_data_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = DNXData_array_table_data_idx_0, I0 = DNXData_array_table_data_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example DNXData_array_table_data") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example DNXData_array_table_data\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/DNXData_array_table_data.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","DNXData_array_table_data[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_table_data);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                EXAMPLE_MODULE_ID,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_table_data);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_table_data
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_table_data), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_table_data))
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->DNXData_array_table_data[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_table_data) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","DNXData_array_table_data[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_table_data[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_value_range_test_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example value_range_test") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example value_range_test\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/value_range_test.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","value_range_test: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","value_range_test: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->value_range_test);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_array_range_example_dump(int  unit,  int  array_range_example_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = array_range_example_idx_0, I0 = array_range_example_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example array_range_example") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example array_range_example\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/array_range_example.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","array_range_example[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        if (i0 == -1) {
            I0 = 10;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->array_range_example
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->array_range_example), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 10)
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->array_range_example[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(10 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","array_range_example[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->array_range_example[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_alloc_after_init_variable_dump(int  unit,  int  alloc_after_init_variable_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = alloc_after_init_variable_idx_0, I0 = alloc_after_init_variable_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example alloc_after_init_variable") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example alloc_after_init_variable\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/alloc_after_init_variable.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","alloc_after_init_variable[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_variable);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                EXAMPLE_MODULE_ID,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_variable);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_variable
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_variable), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_variable))
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->alloc_after_init_variable[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_variable) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","alloc_after_init_variable[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_variable[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_alloc_after_init_test_dump(int  unit,  int  alloc_after_init_test_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = alloc_after_init_test_idx_0, I0 = alloc_after_init_test_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example alloc_after_init_test") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example alloc_after_init_test\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/alloc_after_init_test.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","alloc_after_init_test[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_test);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                EXAMPLE_MODULE_ID,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_test);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_test
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_test), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_test))
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->alloc_after_init_test[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_test) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","alloc_after_init_test[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_test[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_pointer_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example pointer") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example pointer\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/pointer.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","pointer: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pointer);

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","pointer: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint32(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pointer);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_variable_dump(int  unit,  int  bitmap_variable_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = bitmap_variable_idx_0, I0 = bitmap_variable_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "SHR_BITDCL") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example bitmap_variable") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example bitmap_variable\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/bitmap_variable.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","bitmap_variable[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                EXAMPLE_MODULE_ID,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable))
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->bitmap_variable[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","bitmap_variable[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_shr_bitdcl(
                unit,
                &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_pbmp_variable_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "bcm_pbmp_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example pbmp_variable") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example pbmp_variable\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/pbmp_variable.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","pbmp_variable: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","pbmp_variable: ");

        DNX_SW_STATE_PRINT_OPAQUE_FILE(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
                " ");

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_fixed_dump(int  unit,  int  bitmap_fixed_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = bitmap_fixed_idx_0, I0 = bitmap_fixed_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "SHR_BITDCL") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example bitmap_fixed") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example bitmap_fixed\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/bitmap_fixed.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","bitmap_fixed[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        if (i0 == -1) {
            I0 = ((1024)/SHR_BITWID)+1;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= ((1024)/SHR_BITWID)+1)
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->bitmap_fixed[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(((1024)/SHR_BITWID)+1 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","bitmap_fixed[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_shr_bitdcl(
                unit,
                &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_dnx_data_dump(int  unit,  int  bitmap_dnx_data_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = bitmap_dnx_data_idx_0, I0 = bitmap_dnx_data_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "SHR_BITDCL") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example bitmap_dnx_data") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example bitmap_dnx_data\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/bitmap_dnx_data.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","bitmap_dnx_data[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                EXAMPLE_MODULE_ID,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data))
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->bitmap_dnx_data[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","bitmap_dnx_data[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_shr_bitdcl(
                unit,
                &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_buffer_dump(int  unit,  int  buffer_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = buffer_idx_0, I0 = buffer_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "DNX_SW_STATE_BUFF") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example buffer") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example buffer\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/buffer.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","buffer[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                EXAMPLE_MODULE_ID,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer))
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->buffer[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","buffer[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_buffer_fixed_dump(int  unit,  int  buffer_fixed_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = buffer_fixed_idx_0, I0 = buffer_fixed_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "DNX_SW_STATE_BUFF") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example buffer_fixed") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example buffer_fixed\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/buffer_fixed.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","buffer_fixed[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        if (i0 == -1) {
            I0 = 1024;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_fixed
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_fixed), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 1024)
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->buffer_fixed[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(1024 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","buffer_fixed[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_fixed[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_exclude_example_dump(int  unit,  int  bitmap_exclude_example_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = bitmap_exclude_example_idx_0, I0 = bitmap_exclude_example_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "SHR_BITDCL") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example bitmap_exclude_example") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example bitmap_exclude_example\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/bitmap_exclude_example.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","bitmap_exclude_example[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                EXAMPLE_MODULE_ID,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example))
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->bitmap_exclude_example[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","bitmap_exclude_example[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_shr_bitdcl(
                unit,
                &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(example_default_value_tree_array_dnxdata_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(example_default_value_tree_default_value_l1_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_dump(int  unit,  int  array_dnxdata_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(example_default_value_tree_array_dnxdata_array_dynamic_dump(unit, array_dnxdata_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(example_default_value_tree_array_dnxdata_array_pointer_dump(unit, array_dnxdata_idx_0, filters));
    SHR_IF_ERR_EXIT(example_default_value_tree_array_dnxdata_default_value_l2_dump(unit, array_dnxdata_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_array_dynamic_dump(int  unit,  int  array_dnxdata_idx_0,  int  array_dynamic_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(example_default_value_tree_array_dnxdata_array_dynamic_array_static_static_dump(unit, array_dnxdata_idx_0, array_dynamic_idx_0, -1, -1, filters));
    SHR_IF_ERR_EXIT(example_default_value_tree_array_dnxdata_array_dynamic_default_value_l3_dump(unit, array_dnxdata_idx_0, array_dynamic_idx_0, filters));
    SHR_IF_ERR_EXIT(example_default_value_tree_array_dnxdata_array_dynamic_pointer_dump(unit, array_dnxdata_idx_0, array_dynamic_idx_0, filters));
    SHR_IF_ERR_EXIT(example_default_value_tree_array_dnxdata_array_dynamic_array_dynamic_dynamic_dump(unit, array_dnxdata_idx_0, array_dynamic_idx_0, -1, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_array_dynamic_array_static_static_dump(int  unit,  int  array_dnxdata_idx_0,  int  array_dynamic_idx_0,  int  array_static_static_idx_0,  int  array_static_static_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(example_default_value_tree_array_dnxdata_array_dynamic_array_static_static_my_variable_dump(unit, array_dnxdata_idx_0, array_dynamic_idx_0, array_static_static_idx_0,array_static_static_idx_1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_array_dynamic_array_static_static_my_variable_dump(int  unit,  int  array_dnxdata_idx_0,  int  array_dynamic_idx_0,  int  array_static_static_idx_0,  int  array_static_static_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = array_dnxdata_idx_0, I0 = array_dnxdata_idx_0 + 1;
    int i1 = array_dynamic_idx_0, I1 = array_dynamic_idx_0 + 1, org_i1 = array_dynamic_idx_0;
    int i2 = array_static_static_idx_0, I2 = array_static_static_idx_0 + 1, org_i2 = array_static_static_idx_0;
    int i3 = array_static_static_idx_1, I3 = array_static_static_idx_1 + 1, org_i3 = array_static_static_idx_1;
    char *s0 = "", *s1 = "", *s2 = "", *s3 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example default_value_tree array_dnxdata array_dynamic array_static_static my_variable") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example default_value_tree array_dnxdata array_dynamic array_static_static my_variable\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/default_value_tree/array_dnxdata/array_dynamic/array_static_static/my_variable.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","default_value_tree.array_dnxdata[].array_dynamic[].array_static_static[][].my_variable: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                EXAMPLE_MODULE_ID,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata))
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[].array_dynamic[].array_static_static[][].my_variable: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    EXAMPLE_MODULE_ID,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic
                    , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic))
            {
                LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[].array_dynamic[].array_static_static[][].my_variable: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                if (i2 == -1) {
                    I2 = SW_STATE_EXAMPLE_DEFINITION;
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic[i1].array_static_static
                        , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic[i1].array_static_static), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= SW_STATE_EXAMPLE_DEFINITION)
                {
                    LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[].array_dynamic[].array_static_static[][].my_variable: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(SW_STATE_EXAMPLE_DEFINITION == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    if (i3 == -1) {
                        I3 = 10;
                        i3 = dnx_sw_state_dump_check_all_the_same(unit,
                            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic[i1].array_static_static[i2]
                            , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic[i1].array_static_static[i2]), I3, &s3) ? I3 - 1 : 0;
                    }

                    if(i3 >= 10)
                    {
                        LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[].array_dynamic[].array_static_static[][].my_variable: ")));
                        LOG_CLI((BSL_META("Invalid index: %d \n"),i3));
                        SHR_EXIT();
                    }

                    if(10 == 0)
                    {
                        SHR_EXIT();
                    }

                    for(; i3 < I3; i3++) {
                        dnx_sw_state_dump_update_current_idx(unit, i3);
                        DNX_SW_STATE_PRINT_MONITOR(
                            unit,
                            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","default_value_tree.array_dnxdata[%s%d].array_dynamic[%s%d].array_static_static[%s%d][%s%d].my_variable: ", s0, i0, s1, i1, s2, i2, s3, i3);

                        DNX_SW_STATE_PRINT_FILE(
                            unit,
                            "[%s%d][%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2, s3, i3);

                        dnx_sw_state_print_uint32(
                            unit,
                            &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic[i1].array_static_static[i2][i3].my_variable);

                    }
                    i3 = org_i3;
                    dnx_sw_state_dump_end_of_stride(unit);
                }
                i2 = org_i2;
            }
            i1 = org_i1;
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_array_dynamic_default_value_l3_dump(int  unit,  int  array_dnxdata_idx_0,  int  array_dynamic_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(example_default_value_tree_array_dnxdata_array_dynamic_default_value_l3_my_variable_dump(unit, array_dnxdata_idx_0, array_dynamic_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_array_dynamic_default_value_l3_my_variable_dump(int  unit,  int  array_dnxdata_idx_0,  int  array_dynamic_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = array_dnxdata_idx_0, I0 = array_dnxdata_idx_0 + 1;
    int i1 = array_dynamic_idx_0, I1 = array_dynamic_idx_0 + 1, org_i1 = array_dynamic_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example default_value_tree array_dnxdata array_dynamic default_value_l3 my_variable") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example default_value_tree array_dnxdata array_dynamic default_value_l3 my_variable\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/default_value_tree/array_dnxdata/array_dynamic/default_value_l3/my_variable.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","default_value_tree.array_dnxdata[].array_dynamic[].default_value_l3.my_variable: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                EXAMPLE_MODULE_ID,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata))
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[].array_dynamic[].default_value_l3.my_variable: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    EXAMPLE_MODULE_ID,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic
                    , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic))
            {
                LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[].array_dynamic[].default_value_l3.my_variable: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","default_value_tree.array_dnxdata[%s%d].array_dynamic[%s%d].default_value_l3.my_variable: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic[i1].default_value_l3.my_variable);

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
example_default_value_tree_array_dnxdata_array_dynamic_pointer_dump(int  unit,  int  array_dnxdata_idx_0,  int  array_dynamic_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = array_dnxdata_idx_0, I0 = array_dnxdata_idx_0 + 1;
    int i1 = array_dynamic_idx_0, I1 = array_dynamic_idx_0 + 1, org_i1 = array_dynamic_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example default_value_tree array_dnxdata array_dynamic pointer") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example default_value_tree array_dnxdata array_dynamic pointer\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/default_value_tree/array_dnxdata/array_dynamic/pointer.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","default_value_tree.array_dnxdata[].array_dynamic[].pointer: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                EXAMPLE_MODULE_ID,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata))
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[].array_dynamic[].pointer: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    EXAMPLE_MODULE_ID,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic
                    , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic))
            {
                LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[].array_dynamic[].pointer: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                
                
                    DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                        unit,
                        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic[i1].pointer);

                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","default_value_tree.array_dnxdata[%s%d].array_dynamic[%s%d].pointer: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic[i1].pointer);

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
example_default_value_tree_array_dnxdata_array_dynamic_array_dynamic_dynamic_dump(int  unit,  int  array_dnxdata_idx_0,  int  array_dynamic_idx_0,  int  array_dynamic_dynamic_idx_0,  int  array_dynamic_dynamic_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(example_default_value_tree_array_dnxdata_array_dynamic_array_dynamic_dynamic_my_variable_dump(unit, array_dnxdata_idx_0, array_dynamic_idx_0, array_dynamic_dynamic_idx_0,array_dynamic_dynamic_idx_1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_array_dynamic_array_dynamic_dynamic_my_variable_dump(int  unit,  int  array_dnxdata_idx_0,  int  array_dynamic_idx_0,  int  array_dynamic_dynamic_idx_0,  int  array_dynamic_dynamic_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = array_dnxdata_idx_0, I0 = array_dnxdata_idx_0 + 1;
    int i1 = array_dynamic_idx_0, I1 = array_dynamic_idx_0 + 1, org_i1 = array_dynamic_idx_0;
    int i2 = array_dynamic_dynamic_idx_0, I2 = array_dynamic_dynamic_idx_0 + 1, org_i2 = array_dynamic_dynamic_idx_0;
    int i3 = array_dynamic_dynamic_idx_1, I3 = array_dynamic_dynamic_idx_1 + 1, org_i3 = array_dynamic_dynamic_idx_1;
    char *s0 = "", *s1 = "", *s2 = "", *s3 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example default_value_tree array_dnxdata array_dynamic array_dynamic_dynamic my_variable") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example default_value_tree array_dnxdata array_dynamic array_dynamic_dynamic my_variable\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/default_value_tree/array_dnxdata/array_dynamic/array_dynamic_dynamic/my_variable.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","default_value_tree.array_dnxdata[].array_dynamic[].array_dynamic_dynamic[][].my_variable: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                EXAMPLE_MODULE_ID,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata))
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[].array_dynamic[].array_dynamic_dynamic[][].my_variable: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    EXAMPLE_MODULE_ID,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic
                    , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic))
            {
                LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[].array_dynamic[].array_dynamic_dynamic[][].my_variable: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                
                
                    DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                        unit,
                        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic[i1].array_dynamic_dynamic);

                if (i2 == -1) {
                    I2 = dnx_sw_state_get_nof_elements(unit,
                        EXAMPLE_MODULE_ID,
                        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic[i1].array_dynamic_dynamic);
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic[i1].array_dynamic_dynamic
                        , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic[i1].array_dynamic_dynamic), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic[i1].array_dynamic_dynamic))
                {
                    LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[].array_dynamic[].array_dynamic_dynamic[][].my_variable: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic[i1].array_dynamic_dynamic) == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    
                    
                        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                            unit,
                            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic[i1].array_dynamic_dynamic[i2]);

                    if (i3 == -1) {
                        I3 = dnx_sw_state_get_nof_elements(unit,
                            EXAMPLE_MODULE_ID,
                            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic[i1].array_dynamic_dynamic[i2]);
                        i3 = dnx_sw_state_dump_check_all_the_same(unit,
                            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic[i1].array_dynamic_dynamic[i2]
                            , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic[i1].array_dynamic_dynamic[i2]), I3, &s3) ? I3 - 1 : 0;
                    }

                    if(i3 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic[i1].array_dynamic_dynamic[i2]))
                    {
                        LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[].array_dynamic[].array_dynamic_dynamic[][].my_variable: ")));
                        LOG_CLI((BSL_META("Invalid index: %d \n"),i3));
                        SHR_EXIT();
                    }

                    if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic[i1].array_dynamic_dynamic[i2]) == 0)
                    {
                        SHR_EXIT();
                    }

                    for(; i3 < I3; i3++) {
                        dnx_sw_state_dump_update_current_idx(unit, i3);
                        DNX_SW_STATE_PRINT_MONITOR(
                            unit,
                            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","default_value_tree.array_dnxdata[%s%d].array_dynamic[%s%d].array_dynamic_dynamic[%s%d][%s%d].my_variable: ", s0, i0, s1, i1, s2, i2, s3, i3);

                        DNX_SW_STATE_PRINT_FILE(
                            unit,
                            "[%s%d][%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2, s3, i3);

                        dnx_sw_state_print_uint32(
                            unit,
                            &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_dynamic[i1].array_dynamic_dynamic[i2][i3].my_variable);

                    }
                    i3 = org_i3;
                    dnx_sw_state_dump_end_of_stride(unit);
                }
                i2 = org_i2;
            }
            i1 = org_i1;
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}








int
example_default_value_tree_array_dnxdata_array_pointer_dump(int  unit,  int  array_dnxdata_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(example_default_value_tree_array_dnxdata_array_pointer_array_static_static_dump(unit, array_dnxdata_idx_0, -1, -1, filters));
    SHR_IF_ERR_EXIT(example_default_value_tree_array_dnxdata_array_pointer_default_value_l3_dump(unit, array_dnxdata_idx_0, filters));
    SHR_IF_ERR_EXIT(example_default_value_tree_array_dnxdata_array_pointer_pointer_dump(unit, array_dnxdata_idx_0, filters));
    SHR_IF_ERR_EXIT(example_default_value_tree_array_dnxdata_array_pointer_array_dynamic_dynamic_dump(unit, array_dnxdata_idx_0, -1, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_array_pointer_array_static_static_dump(int  unit,  int  array_dnxdata_idx_0,  int  array_static_static_idx_0,  int  array_static_static_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(example_default_value_tree_array_dnxdata_array_pointer_array_static_static_my_variable_dump(unit, array_dnxdata_idx_0, array_static_static_idx_0,array_static_static_idx_1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_array_pointer_array_static_static_my_variable_dump(int  unit,  int  array_dnxdata_idx_0,  int  array_static_static_idx_0,  int  array_static_static_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = array_dnxdata_idx_0, I0 = array_dnxdata_idx_0 + 1;
    int i1 = array_static_static_idx_0, I1 = array_static_static_idx_0 + 1, org_i1 = array_static_static_idx_0;
    int i2 = array_static_static_idx_1, I2 = array_static_static_idx_1 + 1, org_i2 = array_static_static_idx_1;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example default_value_tree array_dnxdata array_pointer array_static_static my_variable") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example default_value_tree array_dnxdata array_pointer array_static_static my_variable\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/default_value_tree/array_dnxdata/array_pointer/array_static_static/my_variable.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","default_value_tree.array_dnxdata[].array_pointer->array_static_static[][].my_variable: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                EXAMPLE_MODULE_ID,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata))
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[].array_pointer->array_static_static[][].my_variable: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_pointer);

            if (i1 == -1) {
                I1 = SW_STATE_EXAMPLE_DEFINITION;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_pointer->array_static_static
                    , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_pointer->array_static_static), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= SW_STATE_EXAMPLE_DEFINITION)
            {
                LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[].array_pointer->array_static_static[][].my_variable: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(SW_STATE_EXAMPLE_DEFINITION == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                if (i2 == -1) {
                    I2 = 10;
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_pointer->array_static_static[i1]
                        , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_pointer->array_static_static[i1]), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= 10)
                {
                    LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[].array_pointer->array_static_static[][].my_variable: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(10 == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_MONITOR(
                        unit,
                        "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","default_value_tree.array_dnxdata[%s%d].array_pointer->array_static_static[%s%d][%s%d].my_variable: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_FILE(
                        unit,
                        "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    dnx_sw_state_print_uint32(
                        unit,
                        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_pointer->array_static_static[i1][i2].my_variable);

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
example_default_value_tree_array_dnxdata_array_pointer_default_value_l3_dump(int  unit,  int  array_dnxdata_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(example_default_value_tree_array_dnxdata_array_pointer_default_value_l3_my_variable_dump(unit, array_dnxdata_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_array_pointer_default_value_l3_my_variable_dump(int  unit,  int  array_dnxdata_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = array_dnxdata_idx_0, I0 = array_dnxdata_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example default_value_tree array_dnxdata array_pointer default_value_l3 my_variable") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example default_value_tree array_dnxdata array_pointer default_value_l3 my_variable\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/default_value_tree/array_dnxdata/array_pointer/default_value_l3/my_variable.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","default_value_tree.array_dnxdata[].array_pointer->default_value_l3.my_variable: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                EXAMPLE_MODULE_ID,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata))
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[].array_pointer->default_value_l3.my_variable: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_pointer);

            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","default_value_tree.array_dnxdata[%s%d].array_pointer->default_value_l3.my_variable: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_pointer->default_value_l3.my_variable);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_array_pointer_pointer_dump(int  unit,  int  array_dnxdata_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = array_dnxdata_idx_0, I0 = array_dnxdata_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example default_value_tree array_dnxdata array_pointer pointer") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example default_value_tree array_dnxdata array_pointer pointer\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/default_value_tree/array_dnxdata/array_pointer/pointer.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","default_value_tree.array_dnxdata[].array_pointer->pointer: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                EXAMPLE_MODULE_ID,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata))
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[].array_pointer->pointer: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_pointer);

            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_pointer->pointer);

            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","default_value_tree.array_dnxdata[%s%d].array_pointer->pointer: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_pointer->pointer);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_array_pointer_array_dynamic_dynamic_dump(int  unit,  int  array_dnxdata_idx_0,  int  array_dynamic_dynamic_idx_0,  int  array_dynamic_dynamic_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(example_default_value_tree_array_dnxdata_array_pointer_array_dynamic_dynamic_my_variable_dump(unit, array_dnxdata_idx_0, array_dynamic_dynamic_idx_0,array_dynamic_dynamic_idx_1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_array_pointer_array_dynamic_dynamic_my_variable_dump(int  unit,  int  array_dnxdata_idx_0,  int  array_dynamic_dynamic_idx_0,  int  array_dynamic_dynamic_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = array_dnxdata_idx_0, I0 = array_dnxdata_idx_0 + 1;
    int i1 = array_dynamic_dynamic_idx_0, I1 = array_dynamic_dynamic_idx_0 + 1, org_i1 = array_dynamic_dynamic_idx_0;
    int i2 = array_dynamic_dynamic_idx_1, I2 = array_dynamic_dynamic_idx_1 + 1, org_i2 = array_dynamic_dynamic_idx_1;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example default_value_tree array_dnxdata array_pointer array_dynamic_dynamic my_variable") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example default_value_tree array_dnxdata array_pointer array_dynamic_dynamic my_variable\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/default_value_tree/array_dnxdata/array_pointer/array_dynamic_dynamic/my_variable.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","default_value_tree.array_dnxdata[].array_pointer->array_dynamic_dynamic[][].my_variable: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                EXAMPLE_MODULE_ID,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata))
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[].array_pointer->array_dynamic_dynamic[][].my_variable: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_pointer);

            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_pointer->array_dynamic_dynamic);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    EXAMPLE_MODULE_ID,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_pointer->array_dynamic_dynamic);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_pointer->array_dynamic_dynamic
                    , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_pointer->array_dynamic_dynamic), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_pointer->array_dynamic_dynamic))
            {
                LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[].array_pointer->array_dynamic_dynamic[][].my_variable: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_pointer->array_dynamic_dynamic) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                
                
                    DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                        unit,
                        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_pointer->array_dynamic_dynamic[i1]);

                if (i2 == -1) {
                    I2 = dnx_sw_state_get_nof_elements(unit,
                        EXAMPLE_MODULE_ID,
                        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_pointer->array_dynamic_dynamic[i1]);
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_pointer->array_dynamic_dynamic[i1]
                        , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_pointer->array_dynamic_dynamic[i1]), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_pointer->array_dynamic_dynamic[i1]))
                {
                    LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[].array_pointer->array_dynamic_dynamic[][].my_variable: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_pointer->array_dynamic_dynamic[i1]) == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_MONITOR(
                        unit,
                        "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","default_value_tree.array_dnxdata[%s%d].array_pointer->array_dynamic_dynamic[%s%d][%s%d].my_variable: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_FILE(
                        unit,
                        "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    dnx_sw_state_print_uint32(
                        unit,
                        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].array_pointer->array_dynamic_dynamic[i1][i2].my_variable);

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
example_default_value_tree_array_dnxdata_default_value_l2_dump(int  unit,  int  array_dnxdata_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(example_default_value_tree_array_dnxdata_default_value_l2_my_variable_dump(unit, array_dnxdata_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_default_value_l2_my_variable_dump(int  unit,  int  array_dnxdata_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = array_dnxdata_idx_0, I0 = array_dnxdata_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example default_value_tree array_dnxdata default_value_l2 my_variable") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example default_value_tree array_dnxdata default_value_l2 my_variable\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/default_value_tree/array_dnxdata/default_value_l2/my_variable.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","default_value_tree.array_dnxdata[].default_value_l2.my_variable: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                EXAMPLE_MODULE_ID,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata))
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[].default_value_l2.my_variable: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","default_value_tree.array_dnxdata[%s%d].default_value_l2.my_variable: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[i0].default_value_l2.my_variable);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_default_value_l1_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(example_default_value_tree_default_value_l1_my_variable_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_default_value_l1_my_variable_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example default_value_tree default_value_l1 my_variable") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example default_value_tree default_value_l1 my_variable\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/default_value_tree/default_value_l1/my_variable.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","default_value_tree.default_value_l1.my_variable: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","default_value_tree.default_value_l1.my_variable: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint32(
            unit,
            &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.default_value_l1.my_variable);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}








int
example_ll_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_ll_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example ll") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example ll\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/ll.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","ll: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","ll: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        SW_STATE_LL_DEFAULT_PRINT(
            unit,
            &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}






int
example_multihead_ll_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_multihead_ll_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example multihead_ll") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example multihead_ll\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/multihead_ll.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","multihead_ll: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","multihead_ll: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        SW_STATE_LL_DEFAULT_PRINT(
            unit,
            &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_sorted_ll_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_sorted_ll_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example sorted_ll") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example sorted_ll\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/sorted_ll.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","sorted_ll: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","sorted_ll: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        SW_STATE_LL_DEFAULT_PRINT(
            unit,
            &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bt_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_bt_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example bt") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example bt\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/bt.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","bt: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","bt: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        SW_STATE_BT_DEFAULT_PRINT(
            unit,
            &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_htb_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_htbl_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example htb") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example htb\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/htb.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","htb: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","htb: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        SW_STATE_HTB_DEFAULT_PRINT(
            unit,
            EXAMPLE_MODULE_ID,
            &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_htb_arr_dump(int  unit,  int  htb_arr_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = htb_arr_idx_0, I0 = htb_arr_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_htbl_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example htb_arr") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example htb_arr\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/htb_arr.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","htb_arr[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        if (i0 == -1) {
            I0 = 2;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_arr
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_arr), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 2)
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->htb_arr[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(2 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","htb_arr[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            SW_STATE_HTB_DEFAULT_PRINT(
                unit,
                EXAMPLE_MODULE_ID,
                &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_arr[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_index_htb_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_htbl_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example index_htb") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example index_htb\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/index_htb.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","index_htb: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","index_htb: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        SW_STATE_HTB_DEFAULT_PRINT(
            unit,
            EXAMPLE_MODULE_ID,
            &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_occ_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_occ_bm_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example occ") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example occ\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/occ.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","occ: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","occ: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        SW_STATE_OCC_BM_DEFAULT_PRINT(
            unit,
            &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_cb_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_cb_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example cb") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example cb\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/cb.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","cb: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","cb: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        sw_state_cb_db_print(
            unit,
            &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->cb);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_leaf_struct_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(example_leaf_struct_var_dump(unit, filters));
    SHR_IF_ERR_EXIT(example_leaf_struct_buff_variable_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_leaf_struct_var_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example leaf_struct var") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example leaf_struct var\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/leaf_struct/var.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","leaf_struct.var: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","leaf_struct.var: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->leaf_struct.var);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}






int
example_leaf_struct_buff_variable_dump(int  unit,  int  buff_variable_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = buff_variable_idx_0, I0 = buff_variable_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "DNX_SW_STATE_BUFF") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example leaf_struct buff_variable") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example leaf_struct buff_variable\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/leaf_struct/buff_variable.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","leaf_struct.buff_variable[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        if (i0 == -1) {
            I0 = 10;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->leaf_struct.buff_variable
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->leaf_struct.buff_variable), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 10)
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->leaf_struct.buff_variable[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(10 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","leaf_struct.buff_variable[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->leaf_struct.buff_variable[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_defragmented_chunk_example_dump(int  unit,  int  defragmented_chunk_example_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = defragmented_chunk_example_idx_0, I0 = defragmented_chunk_example_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_defragmented_chunk_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example defragmented_chunk_example") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example defragmented_chunk_example\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/defragmented_chunk_example.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","defragmented_chunk_example[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        if (i0 == -1) {
            I0 = 5;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->defragmented_chunk_example
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->defragmented_chunk_example), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 5)
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->defragmented_chunk_example[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(5 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","defragmented_chunk_example[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            SW_STATE_DEFRAGMENTED_CHUNK_DEFAULT_PRINT(
                unit,
                EXAMPLE_MODULE_ID,
                &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->defragmented_chunk_example[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_packed_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(example_packed_expecto_patronum_dump(unit, filters));
    SHR_IF_ERR_EXIT(example_packed_my_integer_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_packed_expecto_patronum_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "char") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example packed expecto_patronum") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example packed expecto_patronum\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/packed/expecto_patronum.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","packed.expecto_patronum: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","packed.expecto_patronum: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_char(
            unit,
            &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->packed.expecto_patronum);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_packed_my_integer_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example packed my_integer") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example packed my_integer\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/packed/my_integer.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","packed.my_integer: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","packed.my_integer: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->packed.my_integer);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_my_union_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(example_my_union_my_integer_dump(unit, filters));
    SHR_IF_ERR_EXIT(example_my_union_my_char_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_my_union_my_integer_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example my_union my_integer") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example my_union my_integer\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/my_union/my_integer.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","my_union.my_integer: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","my_union.my_integer: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->my_union.my_integer);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_my_union_my_char_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "char") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example my_union my_char") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example my_union my_char\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/my_union/my_char.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","my_union.my_char: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","my_union.my_char: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_char(
            unit,
            &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->my_union.my_char);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_string_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_string_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example string") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example string\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/string.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","string: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","string: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        SW_STATE_STRING_STRING_PRINT(
            unit,
            EXAMPLE_MODULE_ID,
            &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->string);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_alloc_child_check_dump(int  unit,  int  alloc_child_check_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(example_alloc_child_check_static_array_of_p_dump(unit, alloc_child_check_idx_0, -1, -1, filters));
    SHR_IF_ERR_EXIT(example_alloc_child_check_static_array_dump(unit, alloc_child_check_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(example_alloc_child_check_integer_variable_dump(unit, alloc_child_check_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_alloc_child_check_static_array_of_p_dump(int  unit,  int  alloc_child_check_idx_0,  int  static_array_of_p_idx_0,  int  static_array_of_p_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = alloc_child_check_idx_0, I0 = alloc_child_check_idx_0 + 1;
    int i1 = static_array_of_p_idx_0, I1 = static_array_of_p_idx_0 + 1, org_i1 = static_array_of_p_idx_0;
    int i2 = static_array_of_p_idx_1, I2 = static_array_of_p_idx_1 + 1, org_i2 = static_array_of_p_idx_1;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example alloc_child_check static_array_of_p") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example alloc_child_check static_array_of_p\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/alloc_child_check/static_array_of_p.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","alloc_child_check[].static_array_of_p[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                EXAMPLE_MODULE_ID,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check))
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->alloc_child_check[].static_array_of_p[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = 10000;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[i0].static_array_of_p
                    , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[i0].static_array_of_p), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= 10000)
            {
                LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->alloc_child_check[].static_array_of_p[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(10000 == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                
                
                    DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                        unit,
                        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[i0].static_array_of_p[i1]);

                if (i2 == -1) {
                    I2 = dnx_sw_state_get_nof_elements(unit,
                        EXAMPLE_MODULE_ID,
                        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[i0].static_array_of_p[i1]);
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[i0].static_array_of_p[i1]
                        , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[i0].static_array_of_p[i1]), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[i0].static_array_of_p[i1]))
                {
                    LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->alloc_child_check[].static_array_of_p[][]: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[i0].static_array_of_p[i1]) == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_MONITOR(
                        unit,
                        "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","alloc_child_check[%s%d].static_array_of_p[%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_FILE(
                        unit,
                        "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    dnx_sw_state_print_int(
                        unit,
                        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[i0].static_array_of_p[i1][i2]);

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
example_alloc_child_check_static_array_dump(int  unit,  int  alloc_child_check_idx_0,  int  static_array_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = alloc_child_check_idx_0, I0 = alloc_child_check_idx_0 + 1;
    int i1 = static_array_idx_0, I1 = static_array_idx_0 + 1, org_i1 = static_array_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example alloc_child_check static_array") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example alloc_child_check static_array\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/alloc_child_check/static_array.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","alloc_child_check[].static_array[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                EXAMPLE_MODULE_ID,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check))
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->alloc_child_check[].static_array[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = 20000;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[i0].static_array
                    , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[i0].static_array), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= 20000)
            {
                LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->alloc_child_check[].static_array[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(20000 == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","alloc_child_check[%s%d].static_array[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[i0].static_array[i1]);

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
example_alloc_child_check_integer_variable_dump(int  unit,  int  alloc_child_check_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = alloc_child_check_idx_0, I0 = alloc_child_check_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example alloc_child_check integer_variable") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example alloc_child_check integer_variable\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/alloc_child_check/integer_variable.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","alloc_child_check[].integer_variable: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                EXAMPLE_MODULE_ID,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check))
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->alloc_child_check[].integer_variable: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","alloc_child_check[%s%d].integer_variable: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[i0].integer_variable);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_write_during_wb_example_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example write_during_wb_example") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example write_during_wb_example\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/write_during_wb_example.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","write_during_wb_example: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","write_during_wb_example: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->write_during_wb_example);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_large_DS_examples_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(example_large_DS_examples_large_htb_dump(unit, filters));
    SHR_IF_ERR_EXIT(example_large_DS_examples_large_index_htb_dump(unit, filters));
    SHR_IF_ERR_EXIT(example_large_DS_examples_large_occ_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_large_DS_examples_large_htb_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_htbl_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example large_DS_examples large_htb") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example large_DS_examples large_htb\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/large_DS_examples/large_htb.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","large_DS_examples.large_htb: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","large_DS_examples.large_htb: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        SW_STATE_HTB_DEFAULT_PRINT(
            unit,
            EXAMPLE_MODULE_ID,
            &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_htb);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_large_DS_examples_large_index_htb_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_htbl_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example large_DS_examples large_index_htb") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example large_DS_examples large_index_htb\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/large_DS_examples/large_index_htb.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","large_DS_examples.large_index_htb: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","large_DS_examples.large_index_htb: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        SW_STATE_HTB_DEFAULT_PRINT(
            unit,
            EXAMPLE_MODULE_ID,
            &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_large_DS_examples_large_occ_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_occ_bm_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example large_DS_examples large_occ") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example large_DS_examples large_occ\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/large_DS_examples/large_occ.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","large_DS_examples.large_occ: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","large_DS_examples.large_occ: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        SW_STATE_OCC_BM_DEFAULT_PRINT(
            unit,
            &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_occ);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_pretty_print_example_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_sw_state_pretty_print_example_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example pretty_print_example") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example pretty_print_example\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/pretty_print_example.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","pretty_print_example: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","pretty_print_example: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_pretty_print_example(
            unit,
            &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pretty_print_example);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_buffer_array_dump(int  unit,  int  buffer_array_idx_0,  int  buffer_array_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = buffer_array_idx_0, I0 = buffer_array_idx_0 + 1;
    int i1 = buffer_array_idx_1, I1 = buffer_array_idx_1 + 1, org_i1 = buffer_array_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "DNX_SW_STATE_BUFF") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example buffer_array") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example buffer_array\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/buffer_array.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","buffer_array[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                EXAMPLE_MODULE_ID,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array))
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->buffer_array[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    EXAMPLE_MODULE_ID,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[i0]
                    , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[i0]))
            {
                LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->buffer_array[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","buffer_array[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[i0][i1]);

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
example_bitmap_array_dump(int  unit,  int  bitmap_array_idx_0,  int  bitmap_array_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = bitmap_array_idx_0, I0 = bitmap_array_idx_0 + 1;
    int i1 = bitmap_array_idx_1, I1 = bitmap_array_idx_1 + 1, org_i1 = bitmap_array_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "SHR_BITDCL") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "example bitmap_array") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate example bitmap_array\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "example/bitmap_array.txt",
            "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","bitmap_array[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                EXAMPLE_MODULE_ID,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array
                , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array))
        {
            LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->bitmap_array[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    EXAMPLE_MODULE_ID,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[i0]
                    , sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[i0]))
            {
                LOG_CLI((BSL_META("example[]->((sw_state_example*)sw_state_roots_array[][EXAMPLE_MODULE_ID])->bitmap_array[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, EXAMPLE_MODULE_ID, ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "example[%d]->","((sw_state_example*)sw_state_roots_array[%d][EXAMPLE_MODULE_ID])->","bitmap_array[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_shr_bitdcl(
                    unit,
                    &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[i0][i1]);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t example_info[SOC_MAX_NUM_DEVICES][EXAMPLE_INFO_NOF_ENTRIES];
const char* example_layout_str[EXAMPLE_INFO_NOF_ENTRIES] = {
    "EXAMPLE~",
    "EXAMPLE~MY_VARIABLE~",
    "EXAMPLE~ROLLBACK_COMPARISON_EXCLUDED~",
    "EXAMPLE~MY_ARRAY~",
    "EXAMPLE~TWO_DIMENTIONAL_ARRAY~",
    "EXAMPLE~DYNAMIC_ARRAY~",
    "EXAMPLE~DYNAMIC_ARRAY_ZERO_SIZE~",
    "EXAMPLE~DYNAMIC_DYNAMIC_ARRAY~",
    "EXAMPLE~DYNAMIC_ARRAY_STATIC~",
    "EXAMPLE~COUNTER_TEST~",
    "EXAMPLE~MUTEX_TEST~",
    "EXAMPLE~SEM_TEST~",
    "EXAMPLE~DNXDATA_ARRAY_ALLOC_EXCEPTION~",
    "EXAMPLE~DNXDATA_ARRAY~",
    "EXAMPLE~DNXDATA_ARRAY_DYN_DNXDATA~",
    "EXAMPLE~DNXDATA_ARRAY_DNXDATA_DYN~",
    "EXAMPLE~DNXDATA_ARRAY_DNXDATA_DNXDATA~",
    "EXAMPLE~DNXDATA_ARRAY_STATIC_DNXDATA_DYN~",
    "EXAMPLE~DNXDATA_ARRAY_STATIC_DYN_DNXDATA~",
    "EXAMPLE~DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA~",
    "EXAMPLE~DNXDATA_ARRAY_TABLE_DATA~",
    "EXAMPLE~VALUE_RANGE_TEST~",
    "EXAMPLE~ARRAY_RANGE_EXAMPLE~",
    "EXAMPLE~ALLOC_AFTER_INIT_VARIABLE~",
    "EXAMPLE~ALLOC_AFTER_INIT_TEST~",
    "EXAMPLE~POINTER~",
    "EXAMPLE~BITMAP_VARIABLE~",
    "EXAMPLE~PBMP_VARIABLE~",
    "EXAMPLE~BITMAP_FIXED~",
    "EXAMPLE~BITMAP_DNX_DATA~",
    "EXAMPLE~BUFFER~",
    "EXAMPLE~BUFFER_FIXED~",
    "EXAMPLE~BITMAP_EXCLUDE_EXAMPLE~",
    "EXAMPLE~DEFAULT_VALUE_TREE~",
    "EXAMPLE~DEFAULT_VALUE_TREE~ARRAY_DNXDATA~",
    "EXAMPLE~DEFAULT_VALUE_TREE~ARRAY_DNXDATA~ARRAY_DYNAMIC~",
    "EXAMPLE~DEFAULT_VALUE_TREE~ARRAY_DNXDATA~ARRAY_DYNAMIC~ARRAY_STATIC_STATIC~",
    "EXAMPLE~DEFAULT_VALUE_TREE~ARRAY_DNXDATA~ARRAY_DYNAMIC~ARRAY_STATIC_STATIC~MY_VARIABLE~",
    "EXAMPLE~DEFAULT_VALUE_TREE~ARRAY_DNXDATA~ARRAY_DYNAMIC~DEFAULT_VALUE_L3~",
    "EXAMPLE~DEFAULT_VALUE_TREE~ARRAY_DNXDATA~ARRAY_DYNAMIC~DEFAULT_VALUE_L3~MY_VARIABLE~",
    "EXAMPLE~DEFAULT_VALUE_TREE~ARRAY_DNXDATA~ARRAY_DYNAMIC~POINTER~",
    "EXAMPLE~DEFAULT_VALUE_TREE~ARRAY_DNXDATA~ARRAY_DYNAMIC~ARRAY_DYNAMIC_DYNAMIC~",
    "EXAMPLE~DEFAULT_VALUE_TREE~ARRAY_DNXDATA~ARRAY_DYNAMIC~ARRAY_DYNAMIC_DYNAMIC~MY_VARIABLE~",
    "EXAMPLE~DEFAULT_VALUE_TREE~ARRAY_DNXDATA~ARRAY_POINTER~",
    "EXAMPLE~DEFAULT_VALUE_TREE~ARRAY_DNXDATA~ARRAY_POINTER~ARRAY_STATIC_STATIC~",
    "EXAMPLE~DEFAULT_VALUE_TREE~ARRAY_DNXDATA~ARRAY_POINTER~ARRAY_STATIC_STATIC~MY_VARIABLE~",
    "EXAMPLE~DEFAULT_VALUE_TREE~ARRAY_DNXDATA~ARRAY_POINTER~DEFAULT_VALUE_L3~",
    "EXAMPLE~DEFAULT_VALUE_TREE~ARRAY_DNXDATA~ARRAY_POINTER~DEFAULT_VALUE_L3~MY_VARIABLE~",
    "EXAMPLE~DEFAULT_VALUE_TREE~ARRAY_DNXDATA~ARRAY_POINTER~POINTER~",
    "EXAMPLE~DEFAULT_VALUE_TREE~ARRAY_DNXDATA~ARRAY_POINTER~ARRAY_DYNAMIC_DYNAMIC~",
    "EXAMPLE~DEFAULT_VALUE_TREE~ARRAY_DNXDATA~ARRAY_POINTER~ARRAY_DYNAMIC_DYNAMIC~MY_VARIABLE~",
    "EXAMPLE~DEFAULT_VALUE_TREE~ARRAY_DNXDATA~DEFAULT_VALUE_L2~",
    "EXAMPLE~DEFAULT_VALUE_TREE~ARRAY_DNXDATA~DEFAULT_VALUE_L2~MY_VARIABLE~",
    "EXAMPLE~DEFAULT_VALUE_TREE~DEFAULT_VALUE_L1~",
    "EXAMPLE~DEFAULT_VALUE_TREE~DEFAULT_VALUE_L1~MY_VARIABLE~",
    "EXAMPLE~LL~",
    "EXAMPLE~MULTIHEAD_LL~",
    "EXAMPLE~SORTED_LL~",
    "EXAMPLE~BT~",
    "EXAMPLE~HTB~",
    "EXAMPLE~HTB_ARR~",
    "EXAMPLE~INDEX_HTB~",
    "EXAMPLE~OCC~",
    "EXAMPLE~CB~",
    "EXAMPLE~LEAF_STRUCT~",
    "EXAMPLE~LEAF_STRUCT~VAR~",
    "EXAMPLE~LEAF_STRUCT~BUFF_VARIABLE~",
    "EXAMPLE~DEFRAGMENTED_CHUNK_EXAMPLE~",
    "EXAMPLE~PACKED~",
    "EXAMPLE~PACKED~EXPECTO_PATRONUM~",
    "EXAMPLE~PACKED~MY_INTEGER~",
    "EXAMPLE~MY_UNION~",
    "EXAMPLE~MY_UNION~MY_INTEGER~",
    "EXAMPLE~MY_UNION~MY_CHAR~",
    "EXAMPLE~STRING~",
    "EXAMPLE~ALLOC_CHILD_CHECK~",
    "EXAMPLE~ALLOC_CHILD_CHECK~STATIC_ARRAY_OF_P~",
    "EXAMPLE~ALLOC_CHILD_CHECK~STATIC_ARRAY~",
    "EXAMPLE~ALLOC_CHILD_CHECK~INTEGER_VARIABLE~",
    "EXAMPLE~WRITE_DURING_WB_EXAMPLE~",
    "EXAMPLE~LARGE_DS_EXAMPLES~",
    "EXAMPLE~LARGE_DS_EXAMPLES~LARGE_HTB~",
    "EXAMPLE~LARGE_DS_EXAMPLES~LARGE_INDEX_HTB~",
    "EXAMPLE~LARGE_DS_EXAMPLES~LARGE_OCC~",
    "EXAMPLE~PRETTY_PRINT_EXAMPLE~",
    "EXAMPLE~BUFFER_ARRAY~",
    "EXAMPLE~BITMAP_ARRAY~",
};
#endif 
#endif  
#undef BSL_LOG_MODULE
