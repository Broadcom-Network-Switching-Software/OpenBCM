
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __EXAMPLE_ACCESS_H__
#define __EXAMPLE_ACCESS_H__

#if defined(INCLUDE_CTEST)
#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
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



typedef int (*example_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*example_init_cb)(
    int unit);


typedef int (*example_deinit_cb)(
    int unit);


typedef int (*example_my_variable_set_cb)(
    int unit, uint32 my_variable);


typedef int (*example_my_variable_get_cb)(
    int unit, uint32 *my_variable);


typedef int (*example_rollback_comparison_excluded_set_cb)(
    int unit, uint32 rollback_comparison_excluded);


typedef int (*example_rollback_comparison_excluded_get_cb)(
    int unit, uint32 *rollback_comparison_excluded);


typedef int (*example_my_array_set_cb)(
    int unit, uint32 my_array_idx_0, uint32 my_array);


typedef int (*example_my_array_get_cb)(
    int unit, uint32 my_array_idx_0, uint32 *my_array);


typedef int (*example_two_dimentional_array_set_cb)(
    int unit, uint32 index_1, uint32 index_2, uint32 two_dimentional_array);


typedef int (*example_two_dimentional_array_get_cb)(
    int unit, uint32 index_1, uint32 index_2, uint32 *two_dimentional_array);


typedef int (*example_dynamic_array_set_cb)(
    int unit, uint32 dynamic_array_idx_0, uint32 dynamic_array);


typedef int (*example_dynamic_array_get_cb)(
    int unit, uint32 dynamic_array_idx_0, uint32 *dynamic_array);


typedef int (*example_dynamic_array_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);


typedef int (*example_dynamic_array_zero_size_set_cb)(
    int unit, uint32 dynamic_array_zero_size_idx_0, uint32 dynamic_array_zero_size);


typedef int (*example_dynamic_array_zero_size_get_cb)(
    int unit, uint32 dynamic_array_zero_size_idx_0, uint32 *dynamic_array_zero_size);


typedef int (*example_dynamic_array_zero_size_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);


typedef int (*example_dynamic_array_zero_size_free_cb)(
    int unit);


typedef int (*example_dynamic_dynamic_array_set_cb)(
    int unit, uint32 first_d_index, uint32 dynamic_index, uint32 dynamic_dynamic_array);


typedef int (*example_dynamic_dynamic_array_get_cb)(
    int unit, uint32 first_d_index, uint32 dynamic_index, uint32 *dynamic_dynamic_array);


typedef int (*example_dynamic_dynamic_array_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0, uint32 nof_instances_to_alloc_1);


typedef int (*example_dynamic_array_static_set_cb)(
    int unit, uint32 dynamic_index, uint32 static_index, uint32 dynamic_array_static);


typedef int (*example_dynamic_array_static_get_cb)(
    int unit, uint32 dynamic_index, uint32 static_index, uint32 *dynamic_array_static);


typedef int (*example_dynamic_array_static_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);


typedef int (*example_counter_test_set_cb)(
    int unit, uint32 counter_test);


typedef int (*example_counter_test_get_cb)(
    int unit, uint32 *counter_test);


typedef int (*example_counter_test_inc_cb)(
    int unit, uint32 inc_value);


typedef int (*example_counter_test_dec_cb)(
    int unit, uint32 dec_value);


typedef int (*example_mutex_test_create_cb)(
    int unit);


typedef int (*example_mutex_test_is_created_cb)(
    int unit, uint8 *is_created);


typedef int (*example_mutex_test_destroy_cb)(
    int unit);


typedef int (*example_mutex_test_take_cb)(
    int unit, int usec);


typedef int (*example_mutex_test_give_cb)(
    int unit);


typedef int (*example_sem_test_create_cb)(
    int unit, int is_binary, int initial_count);


typedef int (*example_sem_test_is_created_cb)(
    int unit, uint8 *is_created);


typedef int (*example_sem_test_destroy_cb)(
    int unit);


typedef int (*example_sem_test_take_cb)(
    int unit, int usec);


typedef int (*example_sem_test_give_cb)(
    int unit);


typedef int (*example_DNXData_array_alloc_exception_set_cb)(
    int unit, uint32 DNXData_array_alloc_exception_idx_0, uint32 DNXData_array_alloc_exception);


typedef int (*example_DNXData_array_alloc_exception_get_cb)(
    int unit, uint32 DNXData_array_alloc_exception_idx_0, uint32 *DNXData_array_alloc_exception);


typedef int (*example_DNXData_array_alloc_exception_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);


typedef int (*example_DNXData_array_set_cb)(
    int unit, uint32 DNXData_array_idx_0, uint32 DNXData_array);


typedef int (*example_DNXData_array_get_cb)(
    int unit, uint32 DNXData_array_idx_0, uint32 *DNXData_array);


typedef int (*example_DNXData_array_alloc_cb)(
    int unit);


typedef int (*example_DNXData_array_dyn_dnxdata_set_cb)(
    int unit, uint32 DNXData_array_dyn_dnxdata_idx_0, uint32 DNXData_array_dyn_dnxdata_idx_1, uint32 DNXData_array_dyn_dnxdata);


typedef int (*example_DNXData_array_dyn_dnxdata_get_cb)(
    int unit, uint32 DNXData_array_dyn_dnxdata_idx_0, uint32 DNXData_array_dyn_dnxdata_idx_1, uint32 *DNXData_array_dyn_dnxdata);


typedef int (*example_DNXData_array_dyn_dnxdata_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);


typedef int (*example_DNXData_array_dnxdata_dyn_set_cb)(
    int unit, uint32 DNXData_array_dnxdata_dyn_idx_0, uint32 DNXData_array_dnxdata_dyn_idx_1, uint32 DNXData_array_dnxdata_dyn);


typedef int (*example_DNXData_array_dnxdata_dyn_get_cb)(
    int unit, uint32 DNXData_array_dnxdata_dyn_idx_0, uint32 DNXData_array_dnxdata_dyn_idx_1, uint32 *DNXData_array_dnxdata_dyn);


typedef int (*example_DNXData_array_dnxdata_dyn_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_1);


typedef int (*example_DNXData_array_dnxdata_dnxdata_set_cb)(
    int unit, uint32 DNXData_array_dnxdata_dnxdata_idx_0, uint32 DNXData_array_dnxdata_dnxdata_idx_1, uint32 DNXData_array_dnxdata_dnxdata);


typedef int (*example_DNXData_array_dnxdata_dnxdata_get_cb)(
    int unit, uint32 DNXData_array_dnxdata_dnxdata_idx_0, uint32 DNXData_array_dnxdata_dnxdata_idx_1, uint32 *DNXData_array_dnxdata_dnxdata);


typedef int (*example_DNXData_array_dnxdata_dnxdata_alloc_cb)(
    int unit);


typedef int (*example_DNXData_array_static_dnxdata_dyn_set_cb)(
    int unit, uint32 DNXData_array_static_dnxdata_dyn_idx_0, uint32 DNXData_array_static_dnxdata_dyn_idx_1, uint32 DNXData_array_static_dnxdata_dyn_idx_2, uint32 DNXData_array_static_dnxdata_dyn);


typedef int (*example_DNXData_array_static_dnxdata_dyn_get_cb)(
    int unit, uint32 DNXData_array_static_dnxdata_dyn_idx_0, uint32 DNXData_array_static_dnxdata_dyn_idx_1, uint32 DNXData_array_static_dnxdata_dyn_idx_2, uint32 *DNXData_array_static_dnxdata_dyn);


typedef int (*example_DNXData_array_static_dnxdata_dyn_alloc_cb)(
    int unit, uint32 DNXData_array_static_dnxdata_dyn_idx_0, uint32 nof_instances_to_alloc_2);


typedef int (*example_DNXData_array_static_dyn_dnxdata_set_cb)(
    int unit, uint32 DNXData_array_static_dyn_dnxdata_idx_0, uint32 DNXData_array_static_dyn_dnxdata_idx_1, uint32 DNXData_array_static_dyn_dnxdata_idx_2, uint32 DNXData_array_static_dyn_dnxdata);


typedef int (*example_DNXData_array_static_dyn_dnxdata_get_cb)(
    int unit, uint32 DNXData_array_static_dyn_dnxdata_idx_0, uint32 DNXData_array_static_dyn_dnxdata_idx_1, uint32 DNXData_array_static_dyn_dnxdata_idx_2, uint32 *DNXData_array_static_dyn_dnxdata);


typedef int (*example_DNXData_array_static_dyn_dnxdata_alloc_cb)(
    int unit, uint32 DNXData_array_static_dyn_dnxdata_idx_0, uint32 nof_instances_to_alloc_1);


typedef int (*example_DNXData_array_static_dnxdata_dnxdata_set_cb)(
    int unit, uint32 DNXData_array_static_dnxdata_dnxdata_idx_0, uint32 DNXData_array_static_dnxdata_dnxdata_idx_1, uint32 DNXData_array_static_dnxdata_dnxdata_idx_2, uint32 DNXData_array_static_dnxdata_dnxdata);


typedef int (*example_DNXData_array_static_dnxdata_dnxdata_get_cb)(
    int unit, uint32 DNXData_array_static_dnxdata_dnxdata_idx_0, uint32 DNXData_array_static_dnxdata_dnxdata_idx_1, uint32 DNXData_array_static_dnxdata_dnxdata_idx_2, uint32 *DNXData_array_static_dnxdata_dnxdata);


typedef int (*example_DNXData_array_static_dnxdata_dnxdata_alloc_cb)(
    int unit, uint32 DNXData_array_static_dnxdata_dnxdata_idx_0);


typedef int (*example_DNXData_array_table_data_set_cb)(
    int unit, uint32 DNXData_array_table_data_idx_0, uint32 DNXData_array_table_data);


typedef int (*example_DNXData_array_table_data_get_cb)(
    int unit, uint32 DNXData_array_table_data_idx_0, uint32 *DNXData_array_table_data);


typedef int (*example_DNXData_array_table_data_alloc_cb)(
    int unit);


typedef int (*example_value_range_test_set_cb)(
    int unit, int value_range_test);


typedef int (*example_value_range_test_get_cb)(
    int unit, int *value_range_test);


typedef int (*example_array_range_example_set_cb)(
    int unit, uint32 array_range_example_idx_0, uint32 array_range_example);


typedef int (*example_array_range_example_get_cb)(
    int unit, uint32 array_range_example_idx_0, uint32 *array_range_example);


typedef int (*example_array_range_example_range_read_cb)(
    int unit, uint32 nof_elements, uint32 *dest);


typedef int (*example_array_range_example_range_write_cb)(
    int unit, uint32 nof_elements, const uint32 *source);


typedef int (*example_array_range_example_range_fill_cb)(
    int unit, uint32 from_idx, uint32 nof_elements, uint32 value);


typedef int (*example_alloc_after_init_variable_set_cb)(
    int unit, uint32 alloc_after_init_variable_idx_0, uint32 alloc_after_init_variable);


typedef int (*example_alloc_after_init_variable_get_cb)(
    int unit, uint32 alloc_after_init_variable_idx_0, uint32 *alloc_after_init_variable);


typedef int (*example_alloc_after_init_variable_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);


typedef int (*example_alloc_after_init_test_set_cb)(
    int unit, uint32 alloc_after_init_test_idx_0, uint32 alloc_after_init_test);


typedef int (*example_alloc_after_init_test_get_cb)(
    int unit, uint32 alloc_after_init_test_idx_0, uint32 *alloc_after_init_test);


typedef int (*example_alloc_after_init_test_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);


typedef int (*example_pointer_set_cb)(
    int unit, uint32 pointer);


typedef int (*example_pointer_get_cb)(
    int unit, uint32 *pointer);


typedef int (*example_pointer_alloc_cb)(
    int unit);


typedef int (*example_pointer_is_allocated_cb)(
    int unit, uint8 *is_allocated);


typedef int (*example_bitmap_variable_alloc_bitmap_cb)(
    int unit, uint32 _nof_bits_to_alloc);


typedef int (*example_bitmap_variable_is_allocated_cb)(
    int unit, uint8 *is_allocated);


typedef int (*example_bitmap_variable_free_cb)(
    int unit);


typedef int (*example_bitmap_variable_bit_set_cb)(
    int unit, uint32 _bit_num);


typedef int (*example_bitmap_variable_bit_clear_cb)(
    int unit, uint32 _bit_num);


typedef int (*example_bitmap_variable_bit_get_cb)(
    int unit, uint32 _bit_num, uint8* result);


typedef int (*example_bitmap_variable_bit_range_read_cb)(
    int unit, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result);


typedef int (*example_bitmap_variable_bit_range_write_cb)(
    int unit, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp);


typedef int (*example_bitmap_variable_bit_range_and_cb)(
    int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*example_bitmap_variable_bit_range_or_cb)(
    int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*example_bitmap_variable_bit_range_xor_cb)(
    int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*example_bitmap_variable_bit_range_remove_cb)(
    int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*example_bitmap_variable_bit_range_negate_cb)(
    int unit, uint32 _first, uint32 _count);


typedef int (*example_bitmap_variable_bit_range_clear_cb)(
    int unit, uint32 _first, uint32 _count);


typedef int (*example_bitmap_variable_bit_range_set_cb)(
    int unit, uint32 _first, uint32 _count);


typedef int (*example_bitmap_variable_bit_range_null_cb)(
    int unit, uint32 _first, uint32 _count, uint8 *result);


typedef int (*example_bitmap_variable_bit_range_test_cb)(
    int unit, uint32 _first, uint32 _count, uint8 *result);


typedef int (*example_bitmap_variable_bit_range_eq_cb)(
    int unit, SHR_BITDCL *input_bmp, uint32 _first, uint32 _count, uint8 *result);


typedef int (*example_bitmap_variable_bit_range_count_cb)(
    int unit, uint32 _first, uint32 _range, int *result);


typedef int (*example_pbmp_variable_set_cb)(
    int unit, bcm_pbmp_t pbmp_variable);


typedef int (*example_pbmp_variable_get_cb)(
    int unit, bcm_pbmp_t *pbmp_variable);


typedef int (*example_pbmp_variable_pbmp_neq_cb)(
    int unit, _shr_pbmp_t input_pbmp, uint8 *result);


typedef int (*example_pbmp_variable_pbmp_eq_cb)(
    int unit, _shr_pbmp_t input_pbmp, uint8 *result);


typedef int (*example_pbmp_variable_pbmp_member_cb)(
    int unit, uint32 _input_port, uint8 *result);


typedef int (*example_pbmp_variable_pbmp_not_null_cb)(
    int unit, uint8 *result);


typedef int (*example_pbmp_variable_pbmp_is_null_cb)(
    int unit, uint8 *result);


typedef int (*example_pbmp_variable_pbmp_count_cb)(
    int unit, int *result);


typedef int (*example_pbmp_variable_pbmp_xor_cb)(
    int unit, _shr_pbmp_t input_pbmp);


typedef int (*example_pbmp_variable_pbmp_remove_cb)(
    int unit, _shr_pbmp_t input_pbmp);


typedef int (*example_pbmp_variable_pbmp_assign_cb)(
    int unit, _shr_pbmp_t input_pbmp);


typedef int (*example_pbmp_variable_pbmp_get_cb)(
    int unit, _shr_pbmp_t *output_pbmp);


typedef int (*example_pbmp_variable_pbmp_and_cb)(
    int unit, _shr_pbmp_t input_pbmp);


typedef int (*example_pbmp_variable_pbmp_negate_cb)(
    int unit, _shr_pbmp_t input_pbmp);


typedef int (*example_pbmp_variable_pbmp_or_cb)(
    int unit, _shr_pbmp_t input_pbmp);


typedef int (*example_pbmp_variable_pbmp_clear_cb)(
    int unit);


typedef int (*example_pbmp_variable_pbmp_port_add_cb)(
    int unit, uint32 _input_port);


typedef int (*example_pbmp_variable_pbmp_port_flip_cb)(
    int unit, uint32 _input_port);


typedef int (*example_pbmp_variable_pbmp_port_remove_cb)(
    int unit, uint32 _input_port);


typedef int (*example_pbmp_variable_pbmp_port_set_cb)(
    int unit, uint32 _input_port);


typedef int (*example_pbmp_variable_pbmp_ports_range_add_cb)(
    int unit, uint32 _first_port, uint32 _range);


typedef int (*example_pbmp_variable_pbmp_fmt_cb)(
    int unit, char* _buffer);


typedef int (*example_bitmap_fixed_bit_set_cb)(
    int unit, uint32 _bit_num);


typedef int (*example_bitmap_fixed_bit_clear_cb)(
    int unit, uint32 _bit_num);


typedef int (*example_bitmap_fixed_bit_get_cb)(
    int unit, uint32 _bit_num, uint8* result);


typedef int (*example_bitmap_fixed_bit_range_read_cb)(
    int unit, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result);


typedef int (*example_bitmap_fixed_bit_range_write_cb)(
    int unit, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp);


typedef int (*example_bitmap_fixed_bit_range_and_cb)(
    int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*example_bitmap_fixed_bit_range_or_cb)(
    int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*example_bitmap_fixed_bit_range_xor_cb)(
    int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*example_bitmap_fixed_bit_range_remove_cb)(
    int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*example_bitmap_fixed_bit_range_negate_cb)(
    int unit, uint32 _first, uint32 _count);


typedef int (*example_bitmap_fixed_bit_range_clear_cb)(
    int unit, uint32 _first, uint32 _count);


typedef int (*example_bitmap_fixed_bit_range_set_cb)(
    int unit, uint32 _first, uint32 _count);


typedef int (*example_bitmap_fixed_bit_range_null_cb)(
    int unit, uint32 _first, uint32 _count, uint8 *result);


typedef int (*example_bitmap_fixed_bit_range_test_cb)(
    int unit, uint32 _first, uint32 _count, uint8 *result);


typedef int (*example_bitmap_fixed_bit_range_eq_cb)(
    int unit, SHR_BITDCL *input_bmp, uint32 _first, uint32 _count, uint8 *result);


typedef int (*example_bitmap_fixed_bit_range_count_cb)(
    int unit, uint32 _first, uint32 _range, int *result);


typedef int (*example_bitmap_dnx_data_alloc_bitmap_cb)(
    int unit);


typedef int (*example_bitmap_dnx_data_is_allocated_cb)(
    int unit, uint8 *is_allocated);


typedef int (*example_bitmap_dnx_data_bit_set_cb)(
    int unit, uint32 _bit_num);


typedef int (*example_bitmap_dnx_data_bit_clear_cb)(
    int unit, uint32 _bit_num);


typedef int (*example_bitmap_dnx_data_bit_get_cb)(
    int unit, uint32 _bit_num, uint8* result);


typedef int (*example_bitmap_dnx_data_bit_range_read_cb)(
    int unit, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result);


typedef int (*example_bitmap_dnx_data_bit_range_write_cb)(
    int unit, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp);


typedef int (*example_bitmap_dnx_data_bit_range_and_cb)(
    int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*example_bitmap_dnx_data_bit_range_or_cb)(
    int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*example_bitmap_dnx_data_bit_range_xor_cb)(
    int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*example_bitmap_dnx_data_bit_range_remove_cb)(
    int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*example_bitmap_dnx_data_bit_range_negate_cb)(
    int unit, uint32 _first, uint32 _count);


typedef int (*example_bitmap_dnx_data_bit_range_clear_cb)(
    int unit, uint32 _first, uint32 _count);


typedef int (*example_bitmap_dnx_data_bit_range_set_cb)(
    int unit, uint32 _first, uint32 _count);


typedef int (*example_bitmap_dnx_data_bit_range_null_cb)(
    int unit, uint32 _first, uint32 _count, uint8 *result);


typedef int (*example_bitmap_dnx_data_bit_range_test_cb)(
    int unit, uint32 _first, uint32 _count, uint8 *result);


typedef int (*example_bitmap_dnx_data_bit_range_eq_cb)(
    int unit, SHR_BITDCL *input_bmp, uint32 _first, uint32 _count, uint8 *result);


typedef int (*example_bitmap_dnx_data_bit_range_count_cb)(
    int unit, uint32 _first, uint32 _range, int *result);


typedef int (*example_buffer_set_cb)(
    int unit, uint32 buffer_idx_0, DNX_SW_STATE_BUFF buffer);


typedef int (*example_buffer_get_cb)(
    int unit, uint32 buffer_idx_0, DNX_SW_STATE_BUFF *buffer);


typedef int (*example_buffer_alloc_cb)(
    int unit, uint32 _nof_bytes_to_alloc);


typedef int (*example_buffer_is_allocated_cb)(
    int unit, uint8 *is_allocated);


typedef int (*example_buffer_memread_cb)(
    int unit, uint8 *_dst, uint32 _offset, size_t _len);


typedef int (*example_buffer_memwrite_cb)(
    int unit, const uint8 *_src, uint32 _offset, size_t _len);


typedef int (*example_buffer_memcmp_cb)(
    int unit, const uint8 *_buff, uint32 _offset, size_t _len, int *cmp_result);


typedef int (*example_buffer_memset_cb)(
    int unit, uint32 _offset, size_t _len, int _value);


typedef int (*example_buffer_fixed_set_cb)(
    int unit, uint32 buffer_fixed_idx_0, DNX_SW_STATE_BUFF buffer_fixed);


typedef int (*example_buffer_fixed_get_cb)(
    int unit, uint32 buffer_fixed_idx_0, DNX_SW_STATE_BUFF *buffer_fixed);


typedef int (*example_buffer_fixed_memread_cb)(
    int unit, uint8 *_dst, uint32 _offset, size_t _len);


typedef int (*example_buffer_fixed_memwrite_cb)(
    int unit, const uint8 *_src, uint32 _offset, size_t _len);


typedef int (*example_buffer_fixed_memcmp_cb)(
    int unit, const uint8 *_buff, uint32 _offset, size_t _len, int *cmp_result);


typedef int (*example_buffer_fixed_memset_cb)(
    int unit, uint32 _offset, size_t _len, int _value);


typedef int (*example_bitmap_exclude_example_alloc_bitmap_cb)(
    int unit, uint32 _nof_bits_to_alloc);


typedef int (*example_bitmap_exclude_example_bit_set_cb)(
    int unit, uint32 _bit_num);


typedef int (*example_bitmap_exclude_example_bit_get_cb)(
    int unit, uint32 _bit_num, uint8* result);


typedef int (*example_bitmap_exclude_example_bit_range_read_cb)(
    int unit, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result);


typedef int (*example_bitmap_exclude_example_bit_range_write_cb)(
    int unit, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp);


typedef int (*example_bitmap_exclude_example_bit_range_and_cb)(
    int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*example_bitmap_exclude_example_bit_range_or_cb)(
    int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*example_bitmap_exclude_example_bit_range_xor_cb)(
    int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*example_bitmap_exclude_example_bit_range_remove_cb)(
    int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*example_bitmap_exclude_example_bit_range_clear_cb)(
    int unit, uint32 _first, uint32 _count);


typedef int (*example_bitmap_exclude_example_bit_range_set_cb)(
    int unit, uint32 _first, uint32 _count);


typedef int (*example_bitmap_exclude_example_bit_range_eq_cb)(
    int unit, SHR_BITDCL *input_bmp, uint32 _first, uint32 _count, uint8 *result);


typedef int (*example_bitmap_exclude_example_bit_range_count_cb)(
    int unit, uint32 _first, uint32 _range, int *result);


typedef int (*example_default_value_tree_array_dnxdata_alloc_cb)(
    int unit);


typedef int (*example_default_value_tree_array_dnxdata_free_cb)(
    int unit);


typedef int (*example_default_value_tree_array_dnxdata_array_dynamic_alloc_cb)(
    int unit, uint32 array_dnxdata_idx_0, uint32 nof_instances_to_alloc_0);


typedef int (*example_default_value_tree_array_dnxdata_array_dynamic_array_static_static_my_variable_set_cb)(
    int unit, uint32 array_dnxdata_idx_0, uint32 array_dynamic_idx_0, uint32 array_static_static_idx_0, uint32 array_static_static_idx_1, uint32 my_variable);


typedef int (*example_default_value_tree_array_dnxdata_array_dynamic_array_static_static_my_variable_get_cb)(
    int unit, uint32 array_dnxdata_idx_0, uint32 array_dynamic_idx_0, uint32 array_static_static_idx_0, uint32 array_static_static_idx_1, uint32 *my_variable);


typedef int (*example_default_value_tree_array_dnxdata_array_dynamic_default_value_l3_my_variable_set_cb)(
    int unit, uint32 array_dnxdata_idx_0, uint32 array_dynamic_idx_0, uint32 my_variable);


typedef int (*example_default_value_tree_array_dnxdata_array_dynamic_default_value_l3_my_variable_get_cb)(
    int unit, uint32 array_dnxdata_idx_0, uint32 array_dynamic_idx_0, uint32 *my_variable);


typedef int (*example_default_value_tree_array_dnxdata_array_dynamic_pointer_set_cb)(
    int unit, uint32 array_dnxdata_idx_0, uint32 array_dynamic_idx_0, uint32 pointer);


typedef int (*example_default_value_tree_array_dnxdata_array_dynamic_pointer_get_cb)(
    int unit, uint32 array_dnxdata_idx_0, uint32 array_dynamic_idx_0, uint32 *pointer);


typedef int (*example_default_value_tree_array_dnxdata_array_dynamic_pointer_alloc_cb)(
    int unit, uint32 array_dnxdata_idx_0, uint32 array_dynamic_idx_0);


typedef int (*example_default_value_tree_array_dnxdata_array_dynamic_array_dynamic_dynamic_alloc_cb)(
    int unit, uint32 array_dnxdata_idx_0, uint32 array_dynamic_idx_0, uint32 nof_instances_to_alloc_0, uint32 nof_instances_to_alloc_1);


typedef int (*example_default_value_tree_array_dnxdata_array_dynamic_array_dynamic_dynamic_my_variable_set_cb)(
    int unit, uint32 array_dnxdata_idx_0, uint32 array_dynamic_idx_0, uint32 array_dynamic_dynamic_idx_0, uint32 array_dynamic_dynamic_idx_1, uint32 my_variable);


typedef int (*example_default_value_tree_array_dnxdata_array_dynamic_array_dynamic_dynamic_my_variable_get_cb)(
    int unit, uint32 array_dnxdata_idx_0, uint32 array_dynamic_idx_0, uint32 array_dynamic_dynamic_idx_0, uint32 array_dynamic_dynamic_idx_1, uint32 *my_variable);


typedef int (*example_default_value_tree_array_dnxdata_array_pointer_alloc_cb)(
    int unit, uint32 array_dnxdata_idx_0);


typedef int (*example_default_value_tree_array_dnxdata_array_pointer_array_static_static_my_variable_set_cb)(
    int unit, uint32 array_dnxdata_idx_0, uint32 array_static_static_idx_0, uint32 array_static_static_idx_1, uint32 my_variable);


typedef int (*example_default_value_tree_array_dnxdata_array_pointer_array_static_static_my_variable_get_cb)(
    int unit, uint32 array_dnxdata_idx_0, uint32 array_static_static_idx_0, uint32 array_static_static_idx_1, uint32 *my_variable);


typedef int (*example_default_value_tree_array_dnxdata_array_pointer_default_value_l3_my_variable_set_cb)(
    int unit, uint32 array_dnxdata_idx_0, uint32 my_variable);


typedef int (*example_default_value_tree_array_dnxdata_array_pointer_default_value_l3_my_variable_get_cb)(
    int unit, uint32 array_dnxdata_idx_0, uint32 *my_variable);


typedef int (*example_default_value_tree_array_dnxdata_array_pointer_pointer_set_cb)(
    int unit, uint32 array_dnxdata_idx_0, uint32 pointer);


typedef int (*example_default_value_tree_array_dnxdata_array_pointer_pointer_get_cb)(
    int unit, uint32 array_dnxdata_idx_0, uint32 *pointer);


typedef int (*example_default_value_tree_array_dnxdata_array_pointer_pointer_alloc_cb)(
    int unit, uint32 array_dnxdata_idx_0);


typedef int (*example_default_value_tree_array_dnxdata_array_pointer_array_dynamic_dynamic_alloc_cb)(
    int unit, uint32 array_dnxdata_idx_0, uint32 nof_instances_to_alloc_0, uint32 nof_instances_to_alloc_1);


typedef int (*example_default_value_tree_array_dnxdata_array_pointer_array_dynamic_dynamic_my_variable_set_cb)(
    int unit, uint32 array_dnxdata_idx_0, uint32 array_dynamic_dynamic_idx_0, uint32 array_dynamic_dynamic_idx_1, uint32 my_variable);


typedef int (*example_default_value_tree_array_dnxdata_array_pointer_array_dynamic_dynamic_my_variable_get_cb)(
    int unit, uint32 array_dnxdata_idx_0, uint32 array_dynamic_dynamic_idx_0, uint32 array_dynamic_dynamic_idx_1, uint32 *my_variable);


typedef int (*example_default_value_tree_array_dnxdata_default_value_l2_my_variable_set_cb)(
    int unit, uint32 array_dnxdata_idx_0, uint32 my_variable);


typedef int (*example_default_value_tree_array_dnxdata_default_value_l2_my_variable_get_cb)(
    int unit, uint32 array_dnxdata_idx_0, uint32 *my_variable);


typedef int (*example_default_value_tree_default_value_l1_my_variable_set_cb)(
    int unit, uint32 my_variable);


typedef int (*example_default_value_tree_default_value_l1_my_variable_get_cb)(
    int unit, uint32 *my_variable);


typedef int (*example_ll_create_empty_cb)(
    int unit, sw_state_ll_init_info_t *init_info);


typedef int (*example_ll_nof_elements_cb)(
    int unit, uint32 *nof_elements);


typedef int (*example_ll_node_value_cb)(
    int unit, sw_state_ll_node_t node, int *value);


typedef int (*example_ll_node_update_cb)(
    int unit, sw_state_ll_node_t node, const int *value);


typedef int (*example_ll_next_node_cb)(
    int unit, sw_state_ll_node_t node, sw_state_ll_node_t *next_node);


typedef int (*example_ll_previous_node_cb)(
    int unit, sw_state_ll_node_t node, sw_state_ll_node_t *prev_node);


typedef int (*example_ll_add_first_cb)(
    int unit, const int *value);


typedef int (*example_ll_add_last_cb)(
    int unit, const int *value);


typedef int (*example_ll_add_before_cb)(
    int unit, sw_state_ll_node_t node, const int *value);


typedef int (*example_ll_add_after_cb)(
    int unit, sw_state_ll_node_t node, const int *value);


typedef int (*example_ll_remove_node_cb)(
    int unit, sw_state_ll_node_t node);


typedef int (*example_ll_get_last_cb)(
    int unit, sw_state_ll_node_t *node);


typedef int (*example_ll_get_first_cb)(
    int unit, sw_state_ll_node_t *node);


typedef int (*example_ll_print_cb)(
    int unit);


typedef int (*example_multihead_ll_create_empty_cb)(
    int unit, sw_state_ll_init_info_t *init_info);


typedef int (*example_multihead_ll_nof_elements_cb)(
    int unit, uint32 ll_head_index, uint32 *nof_elements);


typedef int (*example_multihead_ll_node_value_cb)(
    int unit, sw_state_ll_node_t node, int *value);


typedef int (*example_multihead_ll_node_update_cb)(
    int unit, sw_state_ll_node_t node, const int *value);


typedef int (*example_multihead_ll_next_node_cb)(
    int unit, uint32 ll_head_index, sw_state_ll_node_t node, sw_state_ll_node_t *next_node);


typedef int (*example_multihead_ll_previous_node_cb)(
    int unit, uint32 ll_head_index, sw_state_ll_node_t node, sw_state_ll_node_t *prev_node);


typedef int (*example_multihead_ll_add_first_cb)(
    int unit, uint32 ll_head_index, const int *value);


typedef int (*example_multihead_ll_add_last_cb)(
    int unit, uint32 ll_head_index, const int *value);


typedef int (*example_multihead_ll_add_before_cb)(
    int unit, uint32 ll_head_index, sw_state_ll_node_t node, const int *value);


typedef int (*example_multihead_ll_add_after_cb)(
    int unit, uint32 ll_head_index, sw_state_ll_node_t node, const int *value);


typedef int (*example_multihead_ll_remove_node_cb)(
    int unit, uint32 ll_head_index, sw_state_ll_node_t node);


typedef int (*example_multihead_ll_get_last_cb)(
    int unit, uint32 ll_head_index, sw_state_ll_node_t *node);


typedef int (*example_multihead_ll_get_first_cb)(
    int unit, uint32 ll_head_index, sw_state_ll_node_t *node);


typedef int (*example_multihead_ll_print_cb)(
    int unit);


typedef int (*example_sorted_ll_create_empty_cb)(
    int unit, sw_state_ll_init_info_t *init_info);


typedef int (*example_sorted_ll_nof_elements_cb)(
    int unit, uint32 *nof_elements);


typedef int (*example_sorted_ll_node_value_cb)(
    int unit, sw_state_ll_node_t node, uint8 *value);


typedef int (*example_sorted_ll_node_update_cb)(
    int unit, sw_state_ll_node_t node, const uint8 *value);


typedef int (*example_sorted_ll_next_node_cb)(
    int unit, sw_state_ll_node_t node, sw_state_ll_node_t *next_node);


typedef int (*example_sorted_ll_previous_node_cb)(
    int unit, sw_state_ll_node_t node, sw_state_ll_node_t *prev_node);


typedef int (*example_sorted_ll_add_first_cb)(
    int unit, const uint8 *key, const uint8 *value);


typedef int (*example_sorted_ll_add_last_cb)(
    int unit, const uint8 *key, const uint8 *value);


typedef int (*example_sorted_ll_add_before_cb)(
    int unit, sw_state_ll_node_t node, const uint8 *key, const uint8 *value);


typedef int (*example_sorted_ll_add_after_cb)(
    int unit, sw_state_ll_node_t node, const uint8 *key, const uint8 *value);


typedef int (*example_sorted_ll_remove_node_cb)(
    int unit, sw_state_ll_node_t node);


typedef int (*example_sorted_ll_get_last_cb)(
    int unit, sw_state_ll_node_t *node);


typedef int (*example_sorted_ll_get_first_cb)(
    int unit, sw_state_ll_node_t *node);


typedef int (*example_sorted_ll_print_cb)(
    int unit);


typedef int (*example_sorted_ll_add_cb)(
    int unit, const uint8 *key, const uint8 *value);


typedef int (*example_sorted_ll_node_key_cb)(
    int unit, sw_state_ll_node_t node, uint8 *key);


typedef int (*example_sorted_ll_find_cb)(
    int unit, sw_state_ll_node_t *node, const uint8 *key, uint8 *found);


typedef int (*example_bt_create_empty_cb)(
    int unit, sw_state_bt_init_info_t *init_info);


typedef int (*example_bt_destroy_cb)(
    int unit);


typedef int (*example_bt_nof_elements_cb)(
    int unit, uint32 *nof_elements);


typedef int (*example_bt_node_value_cb)(
    int unit, sw_state_bt_node_t node, int *value);


typedef int (*example_bt_node_update_cb)(
    int unit, sw_state_bt_node_t node, const int *value);


typedef int (*example_bt_node_free_cb)(
    int unit, sw_state_bt_node_t node);


typedef int (*example_bt_get_root_cb)(
    int unit, sw_state_bt_node_t *root);


typedef int (*example_bt_get_parent_cb)(
    int unit, sw_state_bt_node_t node, sw_state_bt_node_t *node_parent);


typedef int (*example_bt_get_left_child_cb)(
    int unit, sw_state_bt_node_t node, sw_state_bt_node_t *node_left);


typedef int (*example_bt_get_right_child_cb)(
    int unit, sw_state_bt_node_t node, sw_state_bt_node_t *node_right);


typedef int (*example_bt_add_root_cb)(
    int unit, const int *value);


typedef int (*example_bt_add_left_child_cb)(
    int unit, sw_state_bt_node_t node, const int *value);


typedef int (*example_bt_add_right_child_cb)(
    int unit, sw_state_bt_node_t node, const int *value);


typedef int (*example_bt_set_parent_cb)(
    int unit, sw_state_bt_node_t node, sw_state_bt_node_t node_parent);


typedef int (*example_bt_set_left_child_cb)(
    int unit, sw_state_bt_node_t node, sw_state_bt_node_t node_left);


typedef int (*example_bt_set_right_child_cb)(
    int unit, sw_state_bt_node_t node, sw_state_bt_node_t node_right);


typedef int (*example_bt_print_cb)(
    int unit);


typedef int (*example_htb_create_cb)(
    int unit, sw_state_htbl_init_info_t *init_info);


typedef int (*example_htb_find_cb)(
    int unit, const int *key, int *value, uint8 *found);


typedef int (*example_htb_insert_cb)(
    int unit, const int *key, const int *value, uint8 *success);


typedef int (*example_htb_get_next_cb)(
    int unit, SW_STATE_HASH_TABLE_ITER *iter, const int *key, const int *value);


typedef int (*example_htb_clear_cb)(
    int unit);


typedef int (*example_htb_delete_cb)(
    int unit, const int *key);


typedef int (*example_htb_delete_all_cb)(
    int unit);


typedef int (*example_htb_print_cb)(
    int unit);


typedef int (*example_htb_arr_create_cb)(
    int unit, uint32 htb_arr_idx_0, sw_state_htbl_init_info_t *init_info, int key_size, int value_size);


typedef int (*example_htb_arr_find_cb)(
    int unit, uint32 htb_arr_idx_0, const void *key, void *value, uint8 *found);


typedef int (*example_htb_arr_insert_cb)(
    int unit, uint32 htb_arr_idx_0, const void *key, const void *value, uint8 *success);


typedef int (*example_htb_arr_get_next_cb)(
    int unit, uint32 htb_arr_idx_0, SW_STATE_HASH_TABLE_ITER *iter, const void *key, const void *value);


typedef int (*example_htb_arr_clear_cb)(
    int unit, uint32 htb_arr_idx_0);


typedef int (*example_htb_arr_delete_cb)(
    int unit, uint32 htb_arr_idx_0, const void *key);


typedef int (*example_htb_arr_delete_all_cb)(
    int unit, uint32 htb_arr_idx_0);


typedef int (*example_index_htb_create_cb)(
    int unit, sw_state_htbl_init_info_t *init_info);


typedef int (*example_index_htb_find_cb)(
    int unit, const int *key, uint32 *data_index, uint8 *found);


typedef int (*example_index_htb_insert_cb)(
    int unit, const int *key, const uint32 *data_index, uint8 *success);


typedef int (*example_index_htb_get_next_cb)(
    int unit, SW_STATE_HASH_TABLE_ITER *iter, const int *key, const uint32 *data_index);


typedef int (*example_index_htb_clear_cb)(
    int unit);


typedef int (*example_index_htb_delete_cb)(
    int unit, const int *key);


typedef int (*example_index_htb_delete_all_cb)(
    int unit);


typedef int (*example_index_htb_print_cb)(
    int unit);


typedef int (*example_index_htb_insert_at_index_cb)(
    int unit, const int *key, uint32 data_idx, uint8 *success);


typedef int (*example_index_htb_delete_by_index_cb)(
    int unit, uint32 data_index);


typedef int (*example_index_htb_get_by_index_cb)(
    int unit, uint32 data_index, int *key, uint8 *found);


typedef int (*example_occ_create_cb)(
    int unit, sw_state_occ_bitmap_init_info_t * init_info);


typedef int (*example_occ_get_next_cb)(
    int unit, uint32 *place, uint8 *found);


typedef int (*example_occ_get_next_in_range_cb)(
    int unit, uint32 start, uint32 end, uint8 forward, uint8 val, uint32 *place, uint8 *found);


typedef int (*example_occ_status_set_cb)(
    int unit, uint32 place, uint8 occupied);


typedef int (*example_occ_is_occupied_cb)(
    int unit, uint32 place, uint8 *occupied);


typedef int (*example_occ_alloc_next_cb)(
    int unit, uint32 *place, uint8 *found);


typedef int (*example_occ_clear_cb)(
    int unit);


typedef int (*example_occ_print_cb)(
    int unit);


typedef int (*example_cb_get_cb_cb)(
    int unit, dnx_sw_state_callback_test_function_cb* cb);


typedef int (*example_cb_register_cb_cb)(
    int unit, char *name);


typedef int (*example_cb_unregister_cb_cb)(
    int unit);


typedef int (*example_leaf_struct_set_cb)(
    int unit, CONST sw_state_leaf_struct *leaf_struct);


typedef int (*example_leaf_struct_get_cb)(
    int unit, sw_state_leaf_struct *leaf_struct);


typedef int (*example_defragmented_chunk_example_create_cb)(
    int unit, uint32 defragmented_chunk_example_idx_0, sw_state_defragmented_chunk_init_info_t * init_info);


typedef int (*example_defragmented_chunk_example_piece_alloc_cb)(
    int unit, uint32 defragmented_chunk_example_idx_0, uint32 slots_in_piece, void *move_cb_additional_info, uint32 *piece_offset);


typedef int (*example_defragmented_chunk_example_piece_free_cb)(
    int unit, uint32 defragmented_chunk_example_idx_0, uint32 piece_offset);


typedef int (*example_defragmented_chunk_example_print_cb)(
    int unit, uint32 defragmented_chunk_example_idx_0);


typedef int (*example_packed_expecto_patronum_set_cb)(
    int unit, char expecto_patronum);


typedef int (*example_packed_expecto_patronum_get_cb)(
    int unit, char *expecto_patronum);


typedef int (*example_packed_my_integer_set_cb)(
    int unit, int my_integer);


typedef int (*example_packed_my_integer_get_cb)(
    int unit, int *my_integer);


typedef int (*example_my_union_my_integer_set_cb)(
    int unit, int my_integer);


typedef int (*example_my_union_my_integer_get_cb)(
    int unit, int *my_integer);


typedef int (*example_my_union_my_char_set_cb)(
    int unit, char my_char);


typedef int (*example_my_union_my_char_get_cb)(
    int unit, char *my_char);


typedef int (*example_string_stringncat_cb)(
    int unit, char *src);


typedef int (*example_string_stringncmp_cb)(
    int unit, char *cmp_string, int *result);


typedef int (*example_string_stringncpy_cb)(
    int unit, char *src);


typedef int (*example_string_stringlen_cb)(
    int unit, uint32 *size);


typedef int (*example_string_stringget_cb)(
    int unit, char *o_string);


typedef int (*example_alloc_child_check_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);


typedef int (*example_alloc_child_check_static_array_of_p_set_cb)(
    int unit, uint32 alloc_child_check_idx_0, uint32 static_array_of_p_idx_0, uint32 static_array_of_p_idx_1, int static_array_of_p);


typedef int (*example_alloc_child_check_static_array_of_p_get_cb)(
    int unit, uint32 alloc_child_check_idx_0, uint32 static_array_of_p_idx_0, uint32 static_array_of_p_idx_1, int *static_array_of_p);


typedef int (*example_alloc_child_check_static_array_of_p_alloc_cb)(
    int unit, uint32 alloc_child_check_idx_0, uint32 static_array_of_p_idx_0, uint32 nof_instances_to_alloc_1);


typedef int (*example_alloc_child_check_static_array_set_cb)(
    int unit, uint32 alloc_child_check_idx_0, uint32 static_array_idx_0, int static_array);


typedef int (*example_alloc_child_check_static_array_get_cb)(
    int unit, uint32 alloc_child_check_idx_0, uint32 static_array_idx_0, int *static_array);


typedef int (*example_alloc_child_check_integer_variable_set_cb)(
    int unit, uint32 alloc_child_check_idx_0, int integer_variable);


typedef int (*example_alloc_child_check_integer_variable_get_cb)(
    int unit, uint32 alloc_child_check_idx_0, int *integer_variable);


typedef int (*example_write_during_wb_example_set_cb)(
    int unit, int write_during_wb_example);


typedef int (*example_write_during_wb_example_get_cb)(
    int unit, int *write_during_wb_example);


typedef int (*example_large_DS_examples_large_htb_create_cb)(
    int unit, sw_state_htbl_init_info_t *init_info);


typedef int (*example_large_DS_examples_large_htb_find_cb)(
    int unit, const int *key, int *value, uint8 *found);


typedef int (*example_large_DS_examples_large_htb_insert_cb)(
    int unit, const int *key, const int *value, uint8 *success);


typedef int (*example_large_DS_examples_large_htb_get_next_cb)(
    int unit, SW_STATE_HASH_TABLE_ITER *iter, const int *key, const int *value);


typedef int (*example_large_DS_examples_large_htb_clear_cb)(
    int unit);


typedef int (*example_large_DS_examples_large_htb_delete_cb)(
    int unit, const int *key);


typedef int (*example_large_DS_examples_large_htb_delete_all_cb)(
    int unit);


typedef int (*example_large_DS_examples_large_index_htb_create_cb)(
    int unit, sw_state_htbl_init_info_t *init_info);


typedef int (*example_large_DS_examples_large_index_htb_find_cb)(
    int unit, const int *key, uint32 *data_index, uint8 *found);


typedef int (*example_large_DS_examples_large_index_htb_insert_cb)(
    int unit, const int *key, const uint32 *data_index, uint8 *success);


typedef int (*example_large_DS_examples_large_index_htb_get_next_cb)(
    int unit, SW_STATE_HASH_TABLE_ITER *iter, const int *key, const uint32 *data_index);


typedef int (*example_large_DS_examples_large_index_htb_clear_cb)(
    int unit);


typedef int (*example_large_DS_examples_large_index_htb_delete_cb)(
    int unit, const int *key);


typedef int (*example_large_DS_examples_large_index_htb_delete_all_cb)(
    int unit);


typedef int (*example_large_DS_examples_large_index_htb_insert_at_index_cb)(
    int unit, const int *key, uint32 data_idx, uint8 *success);


typedef int (*example_large_DS_examples_large_index_htb_delete_by_index_cb)(
    int unit, uint32 data_index);


typedef int (*example_large_DS_examples_large_index_htb_get_by_index_cb)(
    int unit, uint32 data_index, int *key, uint8 *found);


typedef int (*example_large_DS_examples_large_occ_create_cb)(
    int unit, sw_state_occ_bitmap_init_info_t * init_info);


typedef int (*example_large_DS_examples_large_occ_get_next_cb)(
    int unit, uint32 *place, uint8 *found);


typedef int (*example_large_DS_examples_large_occ_get_next_in_range_cb)(
    int unit, uint32 start, uint32 end, uint8 forward, uint8 val, uint32 *place, uint8 *found);


typedef int (*example_large_DS_examples_large_occ_status_set_cb)(
    int unit, uint32 place, uint8 occupied);


typedef int (*example_large_DS_examples_large_occ_is_occupied_cb)(
    int unit, uint32 place, uint8 *occupied);


typedef int (*example_large_DS_examples_large_occ_alloc_next_cb)(
    int unit, uint32 *place, uint8 *found);


typedef int (*example_large_DS_examples_large_occ_clear_cb)(
    int unit);


typedef int (*example_pretty_print_example_set_cb)(
    int unit, dnx_sw_state_pretty_print_example_t pretty_print_example);


typedef int (*example_pretty_print_example_get_cb)(
    int unit, dnx_sw_state_pretty_print_example_t *pretty_print_example);


typedef int (*example_buffer_array_set_cb)(
    int unit, uint32 buffer_array_idx_0, uint32 buffer_array_idx_1, DNX_SW_STATE_BUFF buffer_array);


typedef int (*example_buffer_array_get_cb)(
    int unit, uint32 buffer_array_idx_0, uint32 buffer_array_idx_1, DNX_SW_STATE_BUFF *buffer_array);


typedef int (*example_buffer_array_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0, uint32 _nof_bytes_to_alloc);


typedef int (*example_buffer_array_memread_cb)(
    int unit, uint32 buffer_array_idx_0, uint8 *_dst, uint32 _offset, size_t _len);


typedef int (*example_buffer_array_memwrite_cb)(
    int unit, uint32 buffer_array_idx_0, const uint8 *_src, uint32 _offset, size_t _len);


typedef int (*example_buffer_array_memcmp_cb)(
    int unit, uint32 buffer_array_idx_0, const uint8 *_buff, uint32 _offset, size_t _len, int *cmp_result);


typedef int (*example_buffer_array_memset_cb)(
    int unit, uint32 buffer_array_idx_0, uint32 _offset, size_t _len, int _value);


typedef int (*example_bitmap_array_alloc_bitmap_cb)(
    int unit, uint32 bitmap_array_idx_0, uint32 _nof_bits_to_alloc);


typedef int (*example_bitmap_array_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);


typedef int (*example_bitmap_array_bit_set_cb)(
    int unit, uint32 bitmap_array_idx_0, uint32 _bit_num);


typedef int (*example_bitmap_array_bit_clear_cb)(
    int unit, uint32 bitmap_array_idx_0, uint32 _bit_num);


typedef int (*example_bitmap_array_bit_get_cb)(
    int unit, uint32 bitmap_array_idx_0, uint32 _bit_num, uint8* result);


typedef int (*example_bitmap_array_bit_range_read_cb)(
    int unit, uint32 bitmap_array_idx_0, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result);


typedef int (*example_bitmap_array_bit_range_write_cb)(
    int unit, uint32 bitmap_array_idx_0, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp);


typedef int (*example_bitmap_array_bit_range_and_cb)(
    int unit, uint32 bitmap_array_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*example_bitmap_array_bit_range_or_cb)(
    int unit, uint32 bitmap_array_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*example_bitmap_array_bit_range_xor_cb)(
    int unit, uint32 bitmap_array_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*example_bitmap_array_bit_range_remove_cb)(
    int unit, uint32 bitmap_array_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*example_bitmap_array_bit_range_negate_cb)(
    int unit, uint32 bitmap_array_idx_0, uint32 _first, uint32 _count);


typedef int (*example_bitmap_array_bit_range_clear_cb)(
    int unit, uint32 bitmap_array_idx_0, uint32 _first, uint32 _count);


typedef int (*example_bitmap_array_bit_range_set_cb)(
    int unit, uint32 bitmap_array_idx_0, uint32 _first, uint32 _count);


typedef int (*example_bitmap_array_bit_range_null_cb)(
    int unit, uint32 bitmap_array_idx_0, uint32 _first, uint32 _count, uint8 *result);


typedef int (*example_bitmap_array_bit_range_test_cb)(
    int unit, uint32 bitmap_array_idx_0, uint32 _first, uint32 _count, uint8 *result);


typedef int (*example_bitmap_array_bit_range_eq_cb)(
    int unit, uint32 bitmap_array_idx_0, SHR_BITDCL *input_bmp, uint32 _first, uint32 _count, uint8 *result);


typedef int (*example_bitmap_array_bit_range_count_cb)(
    int unit, uint32 bitmap_array_idx_0, uint32 _first, uint32 _range, int *result);




typedef struct {
    example_my_variable_set_cb set;
    example_my_variable_get_cb get;
} example_my_variable_cbs;


typedef struct {
    example_rollback_comparison_excluded_set_cb set;
    example_rollback_comparison_excluded_get_cb get;
} example_rollback_comparison_excluded_cbs;


typedef struct {
    example_my_array_set_cb set;
    example_my_array_get_cb get;
} example_my_array_cbs;


typedef struct {
    example_two_dimentional_array_set_cb set;
    example_two_dimentional_array_get_cb get;
} example_two_dimentional_array_cbs;


typedef struct {
    example_dynamic_array_set_cb set;
    example_dynamic_array_get_cb get;
    example_dynamic_array_alloc_cb alloc;
} example_dynamic_array_cbs;


typedef struct {
    example_dynamic_array_zero_size_set_cb set;
    example_dynamic_array_zero_size_get_cb get;
    example_dynamic_array_zero_size_alloc_cb alloc;
    example_dynamic_array_zero_size_free_cb free;
} example_dynamic_array_zero_size_cbs;


typedef struct {
    example_dynamic_dynamic_array_set_cb set;
    example_dynamic_dynamic_array_get_cb get;
    example_dynamic_dynamic_array_alloc_cb alloc;
} example_dynamic_dynamic_array_cbs;


typedef struct {
    example_dynamic_array_static_set_cb set;
    example_dynamic_array_static_get_cb get;
    example_dynamic_array_static_alloc_cb alloc;
} example_dynamic_array_static_cbs;


typedef struct {
    example_counter_test_set_cb set;
    example_counter_test_get_cb get;
    example_counter_test_inc_cb inc;
    example_counter_test_dec_cb dec;
} example_counter_test_cbs;


typedef struct {
    example_mutex_test_create_cb create;
    example_mutex_test_is_created_cb is_created;
    example_mutex_test_destroy_cb destroy;
    example_mutex_test_take_cb take;
    example_mutex_test_give_cb give;
} example_mutex_test_cbs;


typedef struct {
    example_sem_test_create_cb create;
    example_sem_test_is_created_cb is_created;
    example_sem_test_destroy_cb destroy;
    example_sem_test_take_cb take;
    example_sem_test_give_cb give;
} example_sem_test_cbs;


typedef struct {
    example_DNXData_array_alloc_exception_set_cb set;
    example_DNXData_array_alloc_exception_get_cb get;
    example_DNXData_array_alloc_exception_alloc_cb alloc;
} example_DNXData_array_alloc_exception_cbs;


typedef struct {
    example_DNXData_array_set_cb set;
    example_DNXData_array_get_cb get;
    example_DNXData_array_alloc_cb alloc;
} example_DNXData_array_cbs;


typedef struct {
    example_DNXData_array_dyn_dnxdata_set_cb set;
    example_DNXData_array_dyn_dnxdata_get_cb get;
    example_DNXData_array_dyn_dnxdata_alloc_cb alloc;
} example_DNXData_array_dyn_dnxdata_cbs;


typedef struct {
    example_DNXData_array_dnxdata_dyn_set_cb set;
    example_DNXData_array_dnxdata_dyn_get_cb get;
    example_DNXData_array_dnxdata_dyn_alloc_cb alloc;
} example_DNXData_array_dnxdata_dyn_cbs;


typedef struct {
    example_DNXData_array_dnxdata_dnxdata_set_cb set;
    example_DNXData_array_dnxdata_dnxdata_get_cb get;
    example_DNXData_array_dnxdata_dnxdata_alloc_cb alloc;
} example_DNXData_array_dnxdata_dnxdata_cbs;


typedef struct {
    example_DNXData_array_static_dnxdata_dyn_set_cb set;
    example_DNXData_array_static_dnxdata_dyn_get_cb get;
    example_DNXData_array_static_dnxdata_dyn_alloc_cb alloc;
} example_DNXData_array_static_dnxdata_dyn_cbs;


typedef struct {
    example_DNXData_array_static_dyn_dnxdata_set_cb set;
    example_DNXData_array_static_dyn_dnxdata_get_cb get;
    example_DNXData_array_static_dyn_dnxdata_alloc_cb alloc;
} example_DNXData_array_static_dyn_dnxdata_cbs;


typedef struct {
    example_DNXData_array_static_dnxdata_dnxdata_set_cb set;
    example_DNXData_array_static_dnxdata_dnxdata_get_cb get;
    example_DNXData_array_static_dnxdata_dnxdata_alloc_cb alloc;
} example_DNXData_array_static_dnxdata_dnxdata_cbs;


typedef struct {
    example_DNXData_array_table_data_set_cb set;
    example_DNXData_array_table_data_get_cb get;
    example_DNXData_array_table_data_alloc_cb alloc;
} example_DNXData_array_table_data_cbs;


typedef struct {
    example_value_range_test_set_cb set;
    example_value_range_test_get_cb get;
} example_value_range_test_cbs;


typedef struct {
    example_array_range_example_set_cb set;
    example_array_range_example_get_cb get;
    example_array_range_example_range_read_cb range_read;
    example_array_range_example_range_write_cb range_write;
    example_array_range_example_range_fill_cb range_fill;
} example_array_range_example_cbs;


typedef struct {
    example_alloc_after_init_variable_set_cb set;
    example_alloc_after_init_variable_get_cb get;
    example_alloc_after_init_variable_alloc_cb alloc;
} example_alloc_after_init_variable_cbs;


typedef struct {
    example_alloc_after_init_test_set_cb set;
    example_alloc_after_init_test_get_cb get;
    example_alloc_after_init_test_alloc_cb alloc;
} example_alloc_after_init_test_cbs;


typedef struct {
    example_pointer_set_cb set;
    example_pointer_get_cb get;
    example_pointer_alloc_cb alloc;
    example_pointer_is_allocated_cb is_allocated;
} example_pointer_cbs;


typedef struct {
    example_bitmap_variable_alloc_bitmap_cb alloc_bitmap;
    example_bitmap_variable_is_allocated_cb is_allocated;
    example_bitmap_variable_free_cb free;
    example_bitmap_variable_bit_set_cb bit_set;
    example_bitmap_variable_bit_clear_cb bit_clear;
    example_bitmap_variable_bit_get_cb bit_get;
    example_bitmap_variable_bit_range_read_cb bit_range_read;
    example_bitmap_variable_bit_range_write_cb bit_range_write;
    example_bitmap_variable_bit_range_and_cb bit_range_and;
    example_bitmap_variable_bit_range_or_cb bit_range_or;
    example_bitmap_variable_bit_range_xor_cb bit_range_xor;
    example_bitmap_variable_bit_range_remove_cb bit_range_remove;
    example_bitmap_variable_bit_range_negate_cb bit_range_negate;
    example_bitmap_variable_bit_range_clear_cb bit_range_clear;
    example_bitmap_variable_bit_range_set_cb bit_range_set;
    example_bitmap_variable_bit_range_null_cb bit_range_null;
    example_bitmap_variable_bit_range_test_cb bit_range_test;
    example_bitmap_variable_bit_range_eq_cb bit_range_eq;
    example_bitmap_variable_bit_range_count_cb bit_range_count;
} example_bitmap_variable_cbs;


typedef struct {
    example_pbmp_variable_set_cb set;
    example_pbmp_variable_get_cb get;
    example_pbmp_variable_pbmp_neq_cb pbmp_neq;
    example_pbmp_variable_pbmp_eq_cb pbmp_eq;
    example_pbmp_variable_pbmp_member_cb pbmp_member;
    example_pbmp_variable_pbmp_not_null_cb pbmp_not_null;
    example_pbmp_variable_pbmp_is_null_cb pbmp_is_null;
    example_pbmp_variable_pbmp_count_cb pbmp_count;
    example_pbmp_variable_pbmp_xor_cb pbmp_xor;
    example_pbmp_variable_pbmp_remove_cb pbmp_remove;
    example_pbmp_variable_pbmp_assign_cb pbmp_assign;
    example_pbmp_variable_pbmp_get_cb pbmp_get;
    example_pbmp_variable_pbmp_and_cb pbmp_and;
    example_pbmp_variable_pbmp_negate_cb pbmp_negate;
    example_pbmp_variable_pbmp_or_cb pbmp_or;
    example_pbmp_variable_pbmp_clear_cb pbmp_clear;
    example_pbmp_variable_pbmp_port_add_cb pbmp_port_add;
    example_pbmp_variable_pbmp_port_flip_cb pbmp_port_flip;
    example_pbmp_variable_pbmp_port_remove_cb pbmp_port_remove;
    example_pbmp_variable_pbmp_port_set_cb pbmp_port_set;
    example_pbmp_variable_pbmp_ports_range_add_cb pbmp_ports_range_add;
    example_pbmp_variable_pbmp_fmt_cb pbmp_fmt;
} example_pbmp_variable_cbs;


typedef struct {
    example_bitmap_fixed_bit_set_cb bit_set;
    example_bitmap_fixed_bit_clear_cb bit_clear;
    example_bitmap_fixed_bit_get_cb bit_get;
    example_bitmap_fixed_bit_range_read_cb bit_range_read;
    example_bitmap_fixed_bit_range_write_cb bit_range_write;
    example_bitmap_fixed_bit_range_and_cb bit_range_and;
    example_bitmap_fixed_bit_range_or_cb bit_range_or;
    example_bitmap_fixed_bit_range_xor_cb bit_range_xor;
    example_bitmap_fixed_bit_range_remove_cb bit_range_remove;
    example_bitmap_fixed_bit_range_negate_cb bit_range_negate;
    example_bitmap_fixed_bit_range_clear_cb bit_range_clear;
    example_bitmap_fixed_bit_range_set_cb bit_range_set;
    example_bitmap_fixed_bit_range_null_cb bit_range_null;
    example_bitmap_fixed_bit_range_test_cb bit_range_test;
    example_bitmap_fixed_bit_range_eq_cb bit_range_eq;
    example_bitmap_fixed_bit_range_count_cb bit_range_count;
} example_bitmap_fixed_cbs;


typedef struct {
    example_bitmap_dnx_data_alloc_bitmap_cb alloc_bitmap;
    example_bitmap_dnx_data_is_allocated_cb is_allocated;
    example_bitmap_dnx_data_bit_set_cb bit_set;
    example_bitmap_dnx_data_bit_clear_cb bit_clear;
    example_bitmap_dnx_data_bit_get_cb bit_get;
    example_bitmap_dnx_data_bit_range_read_cb bit_range_read;
    example_bitmap_dnx_data_bit_range_write_cb bit_range_write;
    example_bitmap_dnx_data_bit_range_and_cb bit_range_and;
    example_bitmap_dnx_data_bit_range_or_cb bit_range_or;
    example_bitmap_dnx_data_bit_range_xor_cb bit_range_xor;
    example_bitmap_dnx_data_bit_range_remove_cb bit_range_remove;
    example_bitmap_dnx_data_bit_range_negate_cb bit_range_negate;
    example_bitmap_dnx_data_bit_range_clear_cb bit_range_clear;
    example_bitmap_dnx_data_bit_range_set_cb bit_range_set;
    example_bitmap_dnx_data_bit_range_null_cb bit_range_null;
    example_bitmap_dnx_data_bit_range_test_cb bit_range_test;
    example_bitmap_dnx_data_bit_range_eq_cb bit_range_eq;
    example_bitmap_dnx_data_bit_range_count_cb bit_range_count;
} example_bitmap_dnx_data_cbs;


typedef struct {
    example_buffer_set_cb set;
    example_buffer_get_cb get;
    example_buffer_alloc_cb alloc;
    example_buffer_is_allocated_cb is_allocated;
    example_buffer_memread_cb memread;
    example_buffer_memwrite_cb memwrite;
    example_buffer_memcmp_cb memcmp;
    example_buffer_memset_cb memset;
} example_buffer_cbs;


typedef struct {
    example_buffer_fixed_set_cb set;
    example_buffer_fixed_get_cb get;
    example_buffer_fixed_memread_cb memread;
    example_buffer_fixed_memwrite_cb memwrite;
    example_buffer_fixed_memcmp_cb memcmp;
    example_buffer_fixed_memset_cb memset;
} example_buffer_fixed_cbs;


typedef struct {
    example_bitmap_exclude_example_alloc_bitmap_cb alloc_bitmap;
    example_bitmap_exclude_example_bit_set_cb bit_set;
    example_bitmap_exclude_example_bit_get_cb bit_get;
    example_bitmap_exclude_example_bit_range_read_cb bit_range_read;
    example_bitmap_exclude_example_bit_range_write_cb bit_range_write;
    example_bitmap_exclude_example_bit_range_and_cb bit_range_and;
    example_bitmap_exclude_example_bit_range_or_cb bit_range_or;
    example_bitmap_exclude_example_bit_range_xor_cb bit_range_xor;
    example_bitmap_exclude_example_bit_range_remove_cb bit_range_remove;
    example_bitmap_exclude_example_bit_range_clear_cb bit_range_clear;
    example_bitmap_exclude_example_bit_range_set_cb bit_range_set;
    example_bitmap_exclude_example_bit_range_eq_cb bit_range_eq;
    example_bitmap_exclude_example_bit_range_count_cb bit_range_count;
} example_bitmap_exclude_example_cbs;


typedef struct {
    example_default_value_tree_array_dnxdata_array_dynamic_array_static_static_my_variable_set_cb set;
    example_default_value_tree_array_dnxdata_array_dynamic_array_static_static_my_variable_get_cb get;
} example_default_value_tree_array_dnxdata_array_dynamic_array_static_static_my_variable_cbs;


typedef struct {
    
    example_default_value_tree_array_dnxdata_array_dynamic_array_static_static_my_variable_cbs my_variable;
} example_default_value_tree_array_dnxdata_array_dynamic_array_static_static_cbs;


typedef struct {
    example_default_value_tree_array_dnxdata_array_dynamic_default_value_l3_my_variable_set_cb set;
    example_default_value_tree_array_dnxdata_array_dynamic_default_value_l3_my_variable_get_cb get;
} example_default_value_tree_array_dnxdata_array_dynamic_default_value_l3_my_variable_cbs;


typedef struct {
    
    example_default_value_tree_array_dnxdata_array_dynamic_default_value_l3_my_variable_cbs my_variable;
} example_default_value_tree_array_dnxdata_array_dynamic_default_value_l3_cbs;


typedef struct {
    example_default_value_tree_array_dnxdata_array_dynamic_pointer_set_cb set;
    example_default_value_tree_array_dnxdata_array_dynamic_pointer_get_cb get;
    example_default_value_tree_array_dnxdata_array_dynamic_pointer_alloc_cb alloc;
} example_default_value_tree_array_dnxdata_array_dynamic_pointer_cbs;


typedef struct {
    example_default_value_tree_array_dnxdata_array_dynamic_array_dynamic_dynamic_my_variable_set_cb set;
    example_default_value_tree_array_dnxdata_array_dynamic_array_dynamic_dynamic_my_variable_get_cb get;
} example_default_value_tree_array_dnxdata_array_dynamic_array_dynamic_dynamic_my_variable_cbs;


typedef struct {
    example_default_value_tree_array_dnxdata_array_dynamic_array_dynamic_dynamic_alloc_cb alloc;
    
    example_default_value_tree_array_dnxdata_array_dynamic_array_dynamic_dynamic_my_variable_cbs my_variable;
} example_default_value_tree_array_dnxdata_array_dynamic_array_dynamic_dynamic_cbs;


typedef struct {
    example_default_value_tree_array_dnxdata_array_dynamic_alloc_cb alloc;
    
    example_default_value_tree_array_dnxdata_array_dynamic_array_static_static_cbs array_static_static;
    
    example_default_value_tree_array_dnxdata_array_dynamic_default_value_l3_cbs default_value_l3;
    
    example_default_value_tree_array_dnxdata_array_dynamic_pointer_cbs pointer;
    
    example_default_value_tree_array_dnxdata_array_dynamic_array_dynamic_dynamic_cbs array_dynamic_dynamic;
} example_default_value_tree_array_dnxdata_array_dynamic_cbs;


typedef struct {
    example_default_value_tree_array_dnxdata_array_pointer_array_static_static_my_variable_set_cb set;
    example_default_value_tree_array_dnxdata_array_pointer_array_static_static_my_variable_get_cb get;
} example_default_value_tree_array_dnxdata_array_pointer_array_static_static_my_variable_cbs;


typedef struct {
    
    example_default_value_tree_array_dnxdata_array_pointer_array_static_static_my_variable_cbs my_variable;
} example_default_value_tree_array_dnxdata_array_pointer_array_static_static_cbs;


typedef struct {
    example_default_value_tree_array_dnxdata_array_pointer_default_value_l3_my_variable_set_cb set;
    example_default_value_tree_array_dnxdata_array_pointer_default_value_l3_my_variable_get_cb get;
} example_default_value_tree_array_dnxdata_array_pointer_default_value_l3_my_variable_cbs;


typedef struct {
    
    example_default_value_tree_array_dnxdata_array_pointer_default_value_l3_my_variable_cbs my_variable;
} example_default_value_tree_array_dnxdata_array_pointer_default_value_l3_cbs;


typedef struct {
    example_default_value_tree_array_dnxdata_array_pointer_pointer_set_cb set;
    example_default_value_tree_array_dnxdata_array_pointer_pointer_get_cb get;
    example_default_value_tree_array_dnxdata_array_pointer_pointer_alloc_cb alloc;
} example_default_value_tree_array_dnxdata_array_pointer_pointer_cbs;


typedef struct {
    example_default_value_tree_array_dnxdata_array_pointer_array_dynamic_dynamic_my_variable_set_cb set;
    example_default_value_tree_array_dnxdata_array_pointer_array_dynamic_dynamic_my_variable_get_cb get;
} example_default_value_tree_array_dnxdata_array_pointer_array_dynamic_dynamic_my_variable_cbs;


typedef struct {
    example_default_value_tree_array_dnxdata_array_pointer_array_dynamic_dynamic_alloc_cb alloc;
    
    example_default_value_tree_array_dnxdata_array_pointer_array_dynamic_dynamic_my_variable_cbs my_variable;
} example_default_value_tree_array_dnxdata_array_pointer_array_dynamic_dynamic_cbs;


typedef struct {
    example_default_value_tree_array_dnxdata_array_pointer_alloc_cb alloc;
    
    example_default_value_tree_array_dnxdata_array_pointer_array_static_static_cbs array_static_static;
    
    example_default_value_tree_array_dnxdata_array_pointer_default_value_l3_cbs default_value_l3;
    
    example_default_value_tree_array_dnxdata_array_pointer_pointer_cbs pointer;
    
    example_default_value_tree_array_dnxdata_array_pointer_array_dynamic_dynamic_cbs array_dynamic_dynamic;
} example_default_value_tree_array_dnxdata_array_pointer_cbs;


typedef struct {
    example_default_value_tree_array_dnxdata_default_value_l2_my_variable_set_cb set;
    example_default_value_tree_array_dnxdata_default_value_l2_my_variable_get_cb get;
} example_default_value_tree_array_dnxdata_default_value_l2_my_variable_cbs;


typedef struct {
    
    example_default_value_tree_array_dnxdata_default_value_l2_my_variable_cbs my_variable;
} example_default_value_tree_array_dnxdata_default_value_l2_cbs;


typedef struct {
    example_default_value_tree_array_dnxdata_alloc_cb alloc;
    example_default_value_tree_array_dnxdata_free_cb free;
    
    example_default_value_tree_array_dnxdata_array_dynamic_cbs array_dynamic;
    
    example_default_value_tree_array_dnxdata_array_pointer_cbs array_pointer;
    
    example_default_value_tree_array_dnxdata_default_value_l2_cbs default_value_l2;
} example_default_value_tree_array_dnxdata_cbs;


typedef struct {
    example_default_value_tree_default_value_l1_my_variable_set_cb set;
    example_default_value_tree_default_value_l1_my_variable_get_cb get;
} example_default_value_tree_default_value_l1_my_variable_cbs;


typedef struct {
    
    example_default_value_tree_default_value_l1_my_variable_cbs my_variable;
} example_default_value_tree_default_value_l1_cbs;


typedef struct {
    
    example_default_value_tree_array_dnxdata_cbs array_dnxdata;
    
    example_default_value_tree_default_value_l1_cbs default_value_l1;
} example_default_value_tree_cbs;


typedef struct {
    example_ll_create_empty_cb create_empty;
    example_ll_nof_elements_cb nof_elements;
    example_ll_node_value_cb node_value;
    example_ll_node_update_cb node_update;
    example_ll_next_node_cb next_node;
    example_ll_previous_node_cb previous_node;
    example_ll_add_first_cb add_first;
    example_ll_add_last_cb add_last;
    example_ll_add_before_cb add_before;
    example_ll_add_after_cb add_after;
    example_ll_remove_node_cb remove_node;
    example_ll_get_last_cb get_last;
    example_ll_get_first_cb get_first;
    example_ll_print_cb print;
} example_ll_cbs;


typedef struct {
    example_multihead_ll_create_empty_cb create_empty;
    example_multihead_ll_nof_elements_cb nof_elements;
    example_multihead_ll_node_value_cb node_value;
    example_multihead_ll_node_update_cb node_update;
    example_multihead_ll_next_node_cb next_node;
    example_multihead_ll_previous_node_cb previous_node;
    example_multihead_ll_add_first_cb add_first;
    example_multihead_ll_add_last_cb add_last;
    example_multihead_ll_add_before_cb add_before;
    example_multihead_ll_add_after_cb add_after;
    example_multihead_ll_remove_node_cb remove_node;
    example_multihead_ll_get_last_cb get_last;
    example_multihead_ll_get_first_cb get_first;
    example_multihead_ll_print_cb print;
} example_multihead_ll_cbs;


typedef struct {
    example_sorted_ll_create_empty_cb create_empty;
    example_sorted_ll_nof_elements_cb nof_elements;
    example_sorted_ll_node_value_cb node_value;
    example_sorted_ll_node_update_cb node_update;
    example_sorted_ll_next_node_cb next_node;
    example_sorted_ll_previous_node_cb previous_node;
    example_sorted_ll_add_first_cb add_first;
    example_sorted_ll_add_last_cb add_last;
    example_sorted_ll_add_before_cb add_before;
    example_sorted_ll_add_after_cb add_after;
    example_sorted_ll_remove_node_cb remove_node;
    example_sorted_ll_get_last_cb get_last;
    example_sorted_ll_get_first_cb get_first;
    example_sorted_ll_print_cb print;
    example_sorted_ll_add_cb add;
    example_sorted_ll_node_key_cb node_key;
    example_sorted_ll_find_cb find;
} example_sorted_ll_cbs;


typedef struct {
    example_bt_create_empty_cb create_empty;
    example_bt_destroy_cb destroy;
    example_bt_nof_elements_cb nof_elements;
    example_bt_node_value_cb node_value;
    example_bt_node_update_cb node_update;
    example_bt_node_free_cb node_free;
    example_bt_get_root_cb get_root;
    example_bt_get_parent_cb get_parent;
    example_bt_get_left_child_cb get_left_child;
    example_bt_get_right_child_cb get_right_child;
    example_bt_add_root_cb add_root;
    example_bt_add_left_child_cb add_left_child;
    example_bt_add_right_child_cb add_right_child;
    example_bt_set_parent_cb set_parent;
    example_bt_set_left_child_cb set_left_child;
    example_bt_set_right_child_cb set_right_child;
    example_bt_print_cb print;
} example_bt_cbs;


typedef struct {
    example_htb_create_cb create;
    example_htb_find_cb find;
    example_htb_insert_cb insert;
    example_htb_get_next_cb get_next;
    example_htb_clear_cb clear;
    example_htb_delete_cb delete;
    example_htb_delete_all_cb delete_all;
    example_htb_print_cb print;
} example_htb_cbs;


typedef struct {
    example_htb_arr_create_cb create;
    example_htb_arr_find_cb find;
    example_htb_arr_insert_cb insert;
    example_htb_arr_get_next_cb get_next;
    example_htb_arr_clear_cb clear;
    example_htb_arr_delete_cb delete;
    example_htb_arr_delete_all_cb delete_all;
} example_htb_arr_cbs;


typedef struct {
    example_index_htb_create_cb create;
    example_index_htb_find_cb find;
    example_index_htb_insert_cb insert;
    example_index_htb_get_next_cb get_next;
    example_index_htb_clear_cb clear;
    example_index_htb_delete_cb delete;
    example_index_htb_delete_all_cb delete_all;
    example_index_htb_print_cb print;
    example_index_htb_insert_at_index_cb insert_at_index;
    example_index_htb_delete_by_index_cb delete_by_index;
    example_index_htb_get_by_index_cb get_by_index;
} example_index_htb_cbs;


typedef struct {
    example_occ_create_cb create;
    example_occ_get_next_cb get_next;
    example_occ_get_next_in_range_cb get_next_in_range;
    example_occ_status_set_cb status_set;
    example_occ_is_occupied_cb is_occupied;
    example_occ_alloc_next_cb alloc_next;
    example_occ_clear_cb clear;
    example_occ_print_cb print;
} example_occ_cbs;


typedef struct {
    example_cb_get_cb_cb get_cb;
    example_cb_register_cb_cb register_cb;
    example_cb_unregister_cb_cb unregister_cb;
} example_cb_cbs;


typedef struct {
    example_leaf_struct_set_cb set;
    example_leaf_struct_get_cb get;
} example_leaf_struct_cbs;


typedef struct {
    example_defragmented_chunk_example_create_cb create;
    example_defragmented_chunk_example_piece_alloc_cb piece_alloc;
    example_defragmented_chunk_example_piece_free_cb piece_free;
    example_defragmented_chunk_example_print_cb print;
} example_defragmented_chunk_example_cbs;


typedef struct {
    example_packed_expecto_patronum_set_cb set;
    example_packed_expecto_patronum_get_cb get;
} example_packed_expecto_patronum_cbs;


typedef struct {
    example_packed_my_integer_set_cb set;
    example_packed_my_integer_get_cb get;
} example_packed_my_integer_cbs;


typedef struct {
    
    example_packed_expecto_patronum_cbs expecto_patronum;
    
    example_packed_my_integer_cbs my_integer;
} example_packed_cbs;


typedef struct {
    example_my_union_my_integer_set_cb set;
    example_my_union_my_integer_get_cb get;
} example_my_union_my_integer_cbs;


typedef struct {
    example_my_union_my_char_set_cb set;
    example_my_union_my_char_get_cb get;
} example_my_union_my_char_cbs;


typedef struct {
    
    example_my_union_my_integer_cbs my_integer;
    
    example_my_union_my_char_cbs my_char;
} example_my_union_cbs;


typedef struct {
    example_string_stringncat_cb stringncat;
    example_string_stringncmp_cb stringncmp;
    example_string_stringncpy_cb stringncpy;
    example_string_stringlen_cb stringlen;
    example_string_stringget_cb stringget;
} example_string_cbs;


typedef struct {
    example_alloc_child_check_static_array_of_p_set_cb set;
    example_alloc_child_check_static_array_of_p_get_cb get;
    example_alloc_child_check_static_array_of_p_alloc_cb alloc;
} example_alloc_child_check_static_array_of_p_cbs;


typedef struct {
    example_alloc_child_check_static_array_set_cb set;
    example_alloc_child_check_static_array_get_cb get;
} example_alloc_child_check_static_array_cbs;


typedef struct {
    example_alloc_child_check_integer_variable_set_cb set;
    example_alloc_child_check_integer_variable_get_cb get;
} example_alloc_child_check_integer_variable_cbs;


typedef struct {
    example_alloc_child_check_alloc_cb alloc;
    
    example_alloc_child_check_static_array_of_p_cbs static_array_of_p;
    
    example_alloc_child_check_static_array_cbs static_array;
    
    example_alloc_child_check_integer_variable_cbs integer_variable;
} example_alloc_child_check_cbs;


typedef struct {
    example_write_during_wb_example_set_cb set;
    example_write_during_wb_example_get_cb get;
} example_write_during_wb_example_cbs;


typedef struct {
    example_large_DS_examples_large_htb_create_cb create;
    example_large_DS_examples_large_htb_find_cb find;
    example_large_DS_examples_large_htb_insert_cb insert;
    example_large_DS_examples_large_htb_get_next_cb get_next;
    example_large_DS_examples_large_htb_clear_cb clear;
    example_large_DS_examples_large_htb_delete_cb delete;
    example_large_DS_examples_large_htb_delete_all_cb delete_all;
} example_large_DS_examples_large_htb_cbs;


typedef struct {
    example_large_DS_examples_large_index_htb_create_cb create;
    example_large_DS_examples_large_index_htb_find_cb find;
    example_large_DS_examples_large_index_htb_insert_cb insert;
    example_large_DS_examples_large_index_htb_get_next_cb get_next;
    example_large_DS_examples_large_index_htb_clear_cb clear;
    example_large_DS_examples_large_index_htb_delete_cb delete;
    example_large_DS_examples_large_index_htb_delete_all_cb delete_all;
    example_large_DS_examples_large_index_htb_insert_at_index_cb insert_at_index;
    example_large_DS_examples_large_index_htb_delete_by_index_cb delete_by_index;
    example_large_DS_examples_large_index_htb_get_by_index_cb get_by_index;
} example_large_DS_examples_large_index_htb_cbs;


typedef struct {
    example_large_DS_examples_large_occ_create_cb create;
    example_large_DS_examples_large_occ_get_next_cb get_next;
    example_large_DS_examples_large_occ_get_next_in_range_cb get_next_in_range;
    example_large_DS_examples_large_occ_status_set_cb status_set;
    example_large_DS_examples_large_occ_is_occupied_cb is_occupied;
    example_large_DS_examples_large_occ_alloc_next_cb alloc_next;
    example_large_DS_examples_large_occ_clear_cb clear;
} example_large_DS_examples_large_occ_cbs;


typedef struct {
    
    example_large_DS_examples_large_htb_cbs large_htb;
    
    example_large_DS_examples_large_index_htb_cbs large_index_htb;
    
    example_large_DS_examples_large_occ_cbs large_occ;
} example_large_DS_examples_cbs;


typedef struct {
    example_pretty_print_example_set_cb set;
    example_pretty_print_example_get_cb get;
} example_pretty_print_example_cbs;


typedef struct {
    example_buffer_array_set_cb set;
    example_buffer_array_get_cb get;
    example_buffer_array_alloc_cb alloc;
    example_buffer_array_memread_cb memread;
    example_buffer_array_memwrite_cb memwrite;
    example_buffer_array_memcmp_cb memcmp;
    example_buffer_array_memset_cb memset;
} example_buffer_array_cbs;


typedef struct {
    example_bitmap_array_alloc_bitmap_cb alloc_bitmap;
    example_bitmap_array_alloc_cb alloc;
    example_bitmap_array_bit_set_cb bit_set;
    example_bitmap_array_bit_clear_cb bit_clear;
    example_bitmap_array_bit_get_cb bit_get;
    example_bitmap_array_bit_range_read_cb bit_range_read;
    example_bitmap_array_bit_range_write_cb bit_range_write;
    example_bitmap_array_bit_range_and_cb bit_range_and;
    example_bitmap_array_bit_range_or_cb bit_range_or;
    example_bitmap_array_bit_range_xor_cb bit_range_xor;
    example_bitmap_array_bit_range_remove_cb bit_range_remove;
    example_bitmap_array_bit_range_negate_cb bit_range_negate;
    example_bitmap_array_bit_range_clear_cb bit_range_clear;
    example_bitmap_array_bit_range_set_cb bit_range_set;
    example_bitmap_array_bit_range_null_cb bit_range_null;
    example_bitmap_array_bit_range_test_cb bit_range_test;
    example_bitmap_array_bit_range_eq_cb bit_range_eq;
    example_bitmap_array_bit_range_count_cb bit_range_count;
} example_bitmap_array_cbs;


typedef struct {
    example_is_init_cb is_init;
    example_init_cb init;
    example_deinit_cb deinit;
    
    example_my_variable_cbs my_variable;
    
    example_rollback_comparison_excluded_cbs rollback_comparison_excluded;
    
    example_my_array_cbs my_array;
    
    example_two_dimentional_array_cbs two_dimentional_array;
    
    example_dynamic_array_cbs dynamic_array;
    
    example_dynamic_array_zero_size_cbs dynamic_array_zero_size;
    
    example_dynamic_dynamic_array_cbs dynamic_dynamic_array;
    
    example_dynamic_array_static_cbs dynamic_array_static;
    
    example_counter_test_cbs counter_test;
    
    example_mutex_test_cbs mutex_test;
    
    example_sem_test_cbs sem_test;
    
    example_DNXData_array_alloc_exception_cbs DNXData_array_alloc_exception;
    
    example_DNXData_array_cbs DNXData_array;
    
    example_DNXData_array_dyn_dnxdata_cbs DNXData_array_dyn_dnxdata;
    
    example_DNXData_array_dnxdata_dyn_cbs DNXData_array_dnxdata_dyn;
    
    example_DNXData_array_dnxdata_dnxdata_cbs DNXData_array_dnxdata_dnxdata;
    
    example_DNXData_array_static_dnxdata_dyn_cbs DNXData_array_static_dnxdata_dyn;
    
    example_DNXData_array_static_dyn_dnxdata_cbs DNXData_array_static_dyn_dnxdata;
    
    example_DNXData_array_static_dnxdata_dnxdata_cbs DNXData_array_static_dnxdata_dnxdata;
    
    example_DNXData_array_table_data_cbs DNXData_array_table_data;
    
    example_value_range_test_cbs value_range_test;
    
    example_array_range_example_cbs array_range_example;
    
    example_alloc_after_init_variable_cbs alloc_after_init_variable;
    
    example_alloc_after_init_test_cbs alloc_after_init_test;
    
    example_pointer_cbs pointer;
    
    example_bitmap_variable_cbs bitmap_variable;
    
    example_pbmp_variable_cbs pbmp_variable;
    
    example_bitmap_fixed_cbs bitmap_fixed;
    
    example_bitmap_dnx_data_cbs bitmap_dnx_data;
    
    example_buffer_cbs buffer;
    
    example_buffer_fixed_cbs buffer_fixed;
    
    example_bitmap_exclude_example_cbs bitmap_exclude_example;
    
    example_default_value_tree_cbs default_value_tree;
    
    example_ll_cbs ll;
    
    example_multihead_ll_cbs multihead_ll;
    
    example_sorted_ll_cbs sorted_ll;
    
    example_bt_cbs bt;
    
    example_htb_cbs htb;
    
    example_htb_arr_cbs htb_arr;
    
    example_index_htb_cbs index_htb;
    
    example_occ_cbs occ;
    
    example_cb_cbs cb;
    
    example_leaf_struct_cbs leaf_struct;
    
    example_defragmented_chunk_example_cbs defragmented_chunk_example;
    
    example_packed_cbs packed;
    
    example_my_union_cbs my_union;
    
    example_string_cbs string;
    
    example_alloc_child_check_cbs alloc_child_check;
    
    example_write_during_wb_example_cbs write_during_wb_example;
    
    example_large_DS_examples_cbs large_DS_examples;
    
    example_pretty_print_example_cbs pretty_print_example;
    
    example_buffer_array_cbs buffer_array;
    
    example_bitmap_array_cbs bitmap_array;
} example_cbs;






int
example_cb_get_cb(int unit, dnx_sw_state_callback_test_function_cb* cb);



const char *
dnx_example_name_e_get_name(dnx_example_name_e value);



shr_error_e
dnx_sw_state_callback_test_function_cb_get_cb(sw_state_cb_t * cb_db, uint8 dryRun, dnx_sw_state_callback_test_function_cb * cb);




extern example_cbs example;
#endif  

#endif 
