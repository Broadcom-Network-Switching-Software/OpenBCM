
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __EXAMPLE_DIAGNOSTIC_H__
#define __EXAMPLE_DIAGNOSTIC_H__

#if defined(INCLUDE_CTEST)
#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/example_types.h>
#include <soc/dnxc/swstate/callbacks/sw_state_htb_callbacks.h>
#include <soc/dnxc/swstate/callbacks/sw_state_ll_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_binary_tree.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>
#include <soc/dnxc/swstate/types/sw_state_cb.h>
#include <soc/dnxc/swstate/types/sw_state_defragmented_chunk.h>
#include <soc/dnxc/swstate/types/sw_state_hash_table.h>
#include <soc/dnxc/swstate/types/sw_state_linked_list.h>
#include <soc/dnxc/swstate/types/sw_state_mutex.h>
#include <soc/dnxc/swstate/types/sw_state_occupation_bitmap.h>
#include <soc/dnxc/swstate/types/sw_state_pbmp.h>
#include <soc/dnxc/swstate/types/sw_state_sem.h>
#include <soc/dnxc/swstate/types/sw_state_string.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    EXAMPLE_INFO,
    EXAMPLE_MY_VARIABLE_INFO,
    EXAMPLE_ROLLBACK_COMPARISON_EXCLUDED_INFO,
    EXAMPLE_MY_ARRAY_INFO,
    EXAMPLE_TWO_DIMENTIONAL_ARRAY_INFO,
    EXAMPLE_DYNAMIC_ARRAY_INFO,
    EXAMPLE_DYNAMIC_ARRAY_ZERO_SIZE_INFO,
    EXAMPLE_DYNAMIC_DYNAMIC_ARRAY_INFO,
    EXAMPLE_DYNAMIC_ARRAY_STATIC_INFO,
    EXAMPLE_COUNTER_TEST_INFO,
    EXAMPLE_MUTEX_TEST_INFO,
    EXAMPLE_SEM_TEST_INFO,
    EXAMPLE_DNXDATA_ARRAY_ALLOC_EXCEPTION_INFO,
    EXAMPLE_DNXDATA_ARRAY_INFO,
    EXAMPLE_DNXDATA_ARRAY_DYN_DNXDATA_INFO,
    EXAMPLE_DNXDATA_ARRAY_DNXDATA_DYN_INFO,
    EXAMPLE_DNXDATA_ARRAY_DNXDATA_DNXDATA_INFO,
    EXAMPLE_DNXDATA_ARRAY_STATIC_DNXDATA_DYN_INFO,
    EXAMPLE_DNXDATA_ARRAY_STATIC_DYN_DNXDATA_INFO,
    EXAMPLE_DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA_INFO,
    EXAMPLE_DNXDATA_ARRAY_TABLE_DATA_INFO,
    EXAMPLE_VALUE_RANGE_TEST_INFO,
    EXAMPLE_ARRAY_RANGE_EXAMPLE_INFO,
    EXAMPLE_ALLOC_AFTER_INIT_VARIABLE_INFO,
    EXAMPLE_ALLOC_AFTER_INIT_TEST_INFO,
    EXAMPLE_POINTER_INFO,
    EXAMPLE_BITMAP_VARIABLE_INFO,
    EXAMPLE_PBMP_VARIABLE_INFO,
    EXAMPLE_BITMAP_FIXED_INFO,
    EXAMPLE_BITMAP_DNX_DATA_INFO,
    EXAMPLE_BUFFER_INFO,
    EXAMPLE_BUFFER_FIXED_INFO,
    EXAMPLE_BITMAP_EXCLUDE_EXAMPLE_INFO,
    EXAMPLE_DEFAULT_VALUE_TREE_INFO,
    EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_INFO,
    EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_DYNAMIC_INFO,
    EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_DYNAMIC_ARRAY_STATIC_STATIC_INFO,
    EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_DYNAMIC_ARRAY_STATIC_STATIC_MY_VARIABLE_INFO,
    EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_DYNAMIC_DEFAULT_VALUE_L3_INFO,
    EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_DYNAMIC_DEFAULT_VALUE_L3_MY_VARIABLE_INFO,
    EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_DYNAMIC_POINTER_INFO,
    EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_DYNAMIC_ARRAY_DYNAMIC_DYNAMIC_INFO,
    EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_DYNAMIC_ARRAY_DYNAMIC_DYNAMIC_MY_VARIABLE_INFO,
    EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_POINTER_INFO,
    EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_POINTER_ARRAY_STATIC_STATIC_INFO,
    EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_POINTER_ARRAY_STATIC_STATIC_MY_VARIABLE_INFO,
    EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_POINTER_DEFAULT_VALUE_L3_INFO,
    EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_POINTER_DEFAULT_VALUE_L3_MY_VARIABLE_INFO,
    EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_POINTER_POINTER_INFO,
    EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_POINTER_ARRAY_DYNAMIC_DYNAMIC_INFO,
    EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_POINTER_ARRAY_DYNAMIC_DYNAMIC_MY_VARIABLE_INFO,
    EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_DEFAULT_VALUE_L2_INFO,
    EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_DEFAULT_VALUE_L2_MY_VARIABLE_INFO,
    EXAMPLE_DEFAULT_VALUE_TREE_DEFAULT_VALUE_L1_INFO,
    EXAMPLE_DEFAULT_VALUE_TREE_DEFAULT_VALUE_L1_MY_VARIABLE_INFO,
    EXAMPLE_LL_INFO,
    EXAMPLE_MULTIHEAD_LL_INFO,
    EXAMPLE_SORTED_LL_INFO,
    EXAMPLE_BT_INFO,
    EXAMPLE_HTB_INFO,
    EXAMPLE_HTB_ARR_INFO,
    EXAMPLE_INDEX_HTB_INFO,
    EXAMPLE_OCC_INFO,
    EXAMPLE_CB_INFO,
    EXAMPLE_LEAF_STRUCT_INFO,
    EXAMPLE_LEAF_STRUCT_VAR_INFO,
    EXAMPLE_LEAF_STRUCT_BUFF_VARIABLE_INFO,
    EXAMPLE_DEFRAGMENTED_CHUNK_EXAMPLE_INFO,
    EXAMPLE_PACKED_INFO,
    EXAMPLE_PACKED_EXPECTO_PATRONUM_INFO,
    EXAMPLE_PACKED_MY_INTEGER_INFO,
    EXAMPLE_MY_UNION_INFO,
    EXAMPLE_MY_UNION_MY_INTEGER_INFO,
    EXAMPLE_MY_UNION_MY_CHAR_INFO,
    EXAMPLE_STRING_INFO,
    EXAMPLE_ALLOC_CHILD_CHECK_INFO,
    EXAMPLE_ALLOC_CHILD_CHECK_STATIC_ARRAY_OF_P_INFO,
    EXAMPLE_ALLOC_CHILD_CHECK_STATIC_ARRAY_INFO,
    EXAMPLE_ALLOC_CHILD_CHECK_INTEGER_VARIABLE_INFO,
    EXAMPLE_WRITE_DURING_WB_EXAMPLE_INFO,
    EXAMPLE_LARGE_DS_EXAMPLES_INFO,
    EXAMPLE_LARGE_DS_EXAMPLES_LARGE_HTB_INFO,
    EXAMPLE_LARGE_DS_EXAMPLES_LARGE_INDEX_HTB_INFO,
    EXAMPLE_LARGE_DS_EXAMPLES_LARGE_OCC_INFO,
    EXAMPLE_PRETTY_PRINT_EXAMPLE_INFO,
    EXAMPLE_BUFFER_ARRAY_INFO,
    EXAMPLE_BITMAP_ARRAY_INFO,
    EXAMPLE_INFO_NOF_ENTRIES
} sw_state_example_layout_e;


extern dnx_sw_state_diagnostic_info_t example_info[SOC_MAX_NUM_DEVICES][EXAMPLE_INFO_NOF_ENTRIES];

extern const char* example_layout_str[EXAMPLE_INFO_NOF_ENTRIES];
int example_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_my_variable_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_rollback_comparison_excluded_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_my_array_dump(
    int unit, int my_array_idx_0, dnx_sw_state_dump_filters_t filters);

int example_two_dimentional_array_dump(
    int unit, int index_1, int index_2, dnx_sw_state_dump_filters_t filters);

int example_dynamic_array_dump(
    int unit, int dynamic_array_idx_0, dnx_sw_state_dump_filters_t filters);

int example_dynamic_array_zero_size_dump(
    int unit, int dynamic_array_zero_size_idx_0, dnx_sw_state_dump_filters_t filters);

int example_dynamic_dynamic_array_dump(
    int unit, int first_d_index, int dynamic_index, dnx_sw_state_dump_filters_t filters);

int example_dynamic_array_static_dump(
    int unit, int dynamic_index, int static_index, dnx_sw_state_dump_filters_t filters);

int example_counter_test_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_mutex_test_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_sem_test_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_DNXData_array_alloc_exception_dump(
    int unit, int DNXData_array_alloc_exception_idx_0, dnx_sw_state_dump_filters_t filters);

int example_DNXData_array_dump(
    int unit, int DNXData_array_idx_0, dnx_sw_state_dump_filters_t filters);

int example_DNXData_array_dyn_dnxdata_dump(
    int unit, int DNXData_array_dyn_dnxdata_idx_0, int DNXData_array_dyn_dnxdata_idx_1, dnx_sw_state_dump_filters_t filters);

int example_DNXData_array_dnxdata_dyn_dump(
    int unit, int DNXData_array_dnxdata_dyn_idx_0, int DNXData_array_dnxdata_dyn_idx_1, dnx_sw_state_dump_filters_t filters);

int example_DNXData_array_dnxdata_dnxdata_dump(
    int unit, int DNXData_array_dnxdata_dnxdata_idx_0, int DNXData_array_dnxdata_dnxdata_idx_1, dnx_sw_state_dump_filters_t filters);

int example_DNXData_array_static_dnxdata_dyn_dump(
    int unit, int DNXData_array_static_dnxdata_dyn_idx_0, int DNXData_array_static_dnxdata_dyn_idx_1, int DNXData_array_static_dnxdata_dyn_idx_2, dnx_sw_state_dump_filters_t filters);

int example_DNXData_array_static_dyn_dnxdata_dump(
    int unit, int DNXData_array_static_dyn_dnxdata_idx_0, int DNXData_array_static_dyn_dnxdata_idx_1, int DNXData_array_static_dyn_dnxdata_idx_2, dnx_sw_state_dump_filters_t filters);

int example_DNXData_array_static_dnxdata_dnxdata_dump(
    int unit, int DNXData_array_static_dnxdata_dnxdata_idx_0, int DNXData_array_static_dnxdata_dnxdata_idx_1, int DNXData_array_static_dnxdata_dnxdata_idx_2, dnx_sw_state_dump_filters_t filters);

int example_DNXData_array_table_data_dump(
    int unit, int DNXData_array_table_data_idx_0, dnx_sw_state_dump_filters_t filters);

int example_value_range_test_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_array_range_example_dump(
    int unit, int array_range_example_idx_0, dnx_sw_state_dump_filters_t filters);

int example_alloc_after_init_variable_dump(
    int unit, int alloc_after_init_variable_idx_0, dnx_sw_state_dump_filters_t filters);

int example_alloc_after_init_test_dump(
    int unit, int alloc_after_init_test_idx_0, dnx_sw_state_dump_filters_t filters);

int example_pointer_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_bitmap_variable_dump(
    int unit, int bitmap_variable_idx_0, dnx_sw_state_dump_filters_t filters);

int example_pbmp_variable_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_bitmap_fixed_dump(
    int unit, int bitmap_fixed_idx_0, dnx_sw_state_dump_filters_t filters);

int example_bitmap_dnx_data_dump(
    int unit, int bitmap_dnx_data_idx_0, dnx_sw_state_dump_filters_t filters);

int example_buffer_dump(
    int unit, int buffer_idx_0, dnx_sw_state_dump_filters_t filters);

int example_buffer_fixed_dump(
    int unit, int buffer_fixed_idx_0, dnx_sw_state_dump_filters_t filters);

int example_bitmap_exclude_example_dump(
    int unit, int bitmap_exclude_example_idx_0, dnx_sw_state_dump_filters_t filters);

int example_default_value_tree_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_default_value_tree_array_dnxdata_dump(
    int unit, int array_dnxdata_idx_0, dnx_sw_state_dump_filters_t filters);

int example_default_value_tree_array_dnxdata_array_dynamic_dump(
    int unit, int array_dnxdata_idx_0, int array_dynamic_idx_0, dnx_sw_state_dump_filters_t filters);

int example_default_value_tree_array_dnxdata_array_dynamic_array_static_static_dump(
    int unit, int array_dnxdata_idx_0, int array_dynamic_idx_0, int array_static_static_idx_0, int array_static_static_idx_1, dnx_sw_state_dump_filters_t filters);

int example_default_value_tree_array_dnxdata_array_dynamic_array_static_static_my_variable_dump(
    int unit, int array_dnxdata_idx_0, int array_dynamic_idx_0, int array_static_static_idx_0, int array_static_static_idx_1, dnx_sw_state_dump_filters_t filters);

int example_default_value_tree_array_dnxdata_array_dynamic_default_value_l3_dump(
    int unit, int array_dnxdata_idx_0, int array_dynamic_idx_0, dnx_sw_state_dump_filters_t filters);

int example_default_value_tree_array_dnxdata_array_dynamic_default_value_l3_my_variable_dump(
    int unit, int array_dnxdata_idx_0, int array_dynamic_idx_0, dnx_sw_state_dump_filters_t filters);

int example_default_value_tree_array_dnxdata_array_dynamic_pointer_dump(
    int unit, int array_dnxdata_idx_0, int array_dynamic_idx_0, dnx_sw_state_dump_filters_t filters);

int example_default_value_tree_array_dnxdata_array_dynamic_array_dynamic_dynamic_dump(
    int unit, int array_dnxdata_idx_0, int array_dynamic_idx_0, int array_dynamic_dynamic_idx_0, int array_dynamic_dynamic_idx_1, dnx_sw_state_dump_filters_t filters);

int example_default_value_tree_array_dnxdata_array_dynamic_array_dynamic_dynamic_my_variable_dump(
    int unit, int array_dnxdata_idx_0, int array_dynamic_idx_0, int array_dynamic_dynamic_idx_0, int array_dynamic_dynamic_idx_1, dnx_sw_state_dump_filters_t filters);

int example_default_value_tree_array_dnxdata_array_pointer_dump(
    int unit, int array_dnxdata_idx_0, dnx_sw_state_dump_filters_t filters);

int example_default_value_tree_array_dnxdata_array_pointer_array_static_static_dump(
    int unit, int array_dnxdata_idx_0, int array_static_static_idx_0, int array_static_static_idx_1, dnx_sw_state_dump_filters_t filters);

int example_default_value_tree_array_dnxdata_array_pointer_array_static_static_my_variable_dump(
    int unit, int array_dnxdata_idx_0, int array_static_static_idx_0, int array_static_static_idx_1, dnx_sw_state_dump_filters_t filters);

int example_default_value_tree_array_dnxdata_array_pointer_default_value_l3_dump(
    int unit, int array_dnxdata_idx_0, dnx_sw_state_dump_filters_t filters);

int example_default_value_tree_array_dnxdata_array_pointer_default_value_l3_my_variable_dump(
    int unit, int array_dnxdata_idx_0, dnx_sw_state_dump_filters_t filters);

int example_default_value_tree_array_dnxdata_array_pointer_pointer_dump(
    int unit, int array_dnxdata_idx_0, dnx_sw_state_dump_filters_t filters);

int example_default_value_tree_array_dnxdata_array_pointer_array_dynamic_dynamic_dump(
    int unit, int array_dnxdata_idx_0, int array_dynamic_dynamic_idx_0, int array_dynamic_dynamic_idx_1, dnx_sw_state_dump_filters_t filters);

int example_default_value_tree_array_dnxdata_array_pointer_array_dynamic_dynamic_my_variable_dump(
    int unit, int array_dnxdata_idx_0, int array_dynamic_dynamic_idx_0, int array_dynamic_dynamic_idx_1, dnx_sw_state_dump_filters_t filters);

int example_default_value_tree_array_dnxdata_default_value_l2_dump(
    int unit, int array_dnxdata_idx_0, dnx_sw_state_dump_filters_t filters);

int example_default_value_tree_array_dnxdata_default_value_l2_my_variable_dump(
    int unit, int array_dnxdata_idx_0, dnx_sw_state_dump_filters_t filters);

int example_default_value_tree_default_value_l1_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_default_value_tree_default_value_l1_my_variable_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_ll_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_multihead_ll_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_sorted_ll_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_bt_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_htb_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_htb_arr_dump(
    int unit, int htb_arr_idx_0, dnx_sw_state_dump_filters_t filters);

int example_index_htb_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_occ_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_cb_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_leaf_struct_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_leaf_struct_var_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_leaf_struct_buff_variable_dump(
    int unit, int buff_variable_idx_0, dnx_sw_state_dump_filters_t filters);

int example_defragmented_chunk_example_dump(
    int unit, int defragmented_chunk_example_idx_0, dnx_sw_state_dump_filters_t filters);

int example_packed_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_packed_expecto_patronum_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_packed_my_integer_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_my_union_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_my_union_my_integer_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_my_union_my_char_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_string_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_alloc_child_check_dump(
    int unit, int alloc_child_check_idx_0, dnx_sw_state_dump_filters_t filters);

int example_alloc_child_check_static_array_of_p_dump(
    int unit, int alloc_child_check_idx_0, int static_array_of_p_idx_0, int static_array_of_p_idx_1, dnx_sw_state_dump_filters_t filters);

int example_alloc_child_check_static_array_dump(
    int unit, int alloc_child_check_idx_0, int static_array_idx_0, dnx_sw_state_dump_filters_t filters);

int example_alloc_child_check_integer_variable_dump(
    int unit, int alloc_child_check_idx_0, dnx_sw_state_dump_filters_t filters);

int example_write_during_wb_example_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_large_DS_examples_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_large_DS_examples_large_htb_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_large_DS_examples_large_index_htb_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_large_DS_examples_large_occ_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_pretty_print_example_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int example_buffer_array_dump(
    int unit, int buffer_array_idx_0, int buffer_array_idx_1, dnx_sw_state_dump_filters_t filters);

int example_bitmap_array_dump(
    int unit, int bitmap_array_idx_0, int bitmap_array_idx_1, dnx_sw_state_dump_filters_t filters);

#endif 
#endif  

#endif 
