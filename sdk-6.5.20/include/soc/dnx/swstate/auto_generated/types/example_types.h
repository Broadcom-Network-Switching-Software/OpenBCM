
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __EXAMPLE_TYPES_H__
#define __EXAMPLE_TYPES_H__

#if defined(INCLUDE_CTEST)
#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_module_testing.h>
#include <soc/dnxc/swstate/callbacks/sw_state_htb_callbacks.h>
#include <soc/dnxc/swstate/callbacks/sw_state_ll_callbacks.h>
#include <soc/dnxc/swstate/dnx_sw_state_pretty_dump.h>
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


#define SW_STATE_EXAMPLE_DEFINITION 10



typedef uint8 (*dnx_sw_state_callback_test_function_cb) (int unit)  ;




typedef enum {
    
    DNX_EXAMPLE_ENUM_NEGATIVE = -1,
    
    DNX_EXAMPLE_ENUM_ZERO = 0,
    
    DNX_EXAMPLE_ENUM_ONE
} dnx_example_name_e;





typedef struct {
    int var;
    DNX_SW_STATE_BUFF buff_variable[10];
} sw_state_leaf_struct;

typedef struct {
    
    uint32 my_variable;
} sw_state_default_value;

typedef struct {
    
    sw_state_default_value array_static_static[SW_STATE_EXAMPLE_DEFINITION][10];
    
    sw_state_default_value default_value_l3;
    
    uint32* pointer;
    
    sw_state_default_value** array_dynamic_dynamic;
} sw_state_default_l3;

typedef struct {
    
    sw_state_default_l3* array_dynamic;
    
    sw_state_default_l3* array_pointer;
    
    sw_state_default_value default_value_l2;
} sw_state_default_l2;

typedef struct {
    
    sw_state_default_l2* array_dnxdata;
    
    sw_state_default_value default_value_l1;
} sw_state_default_l1;

typedef struct {
    char expecto_patronum;
    int my_integer;
} __ATTRIBUTE_PACKED__ sw_state_packed;

typedef union {
    int my_integer;
    char my_char;
} sw_state_example_union;

typedef struct {
    int* static_array_of_p[10000];
    int static_array[20000];
    int integer_variable;
} sw_state_example_child_size_check_t;

typedef struct {
    sw_state_htbl_t large_htb;
    sw_state_htbl_t large_index_htb;
    sw_state_occ_bm_t large_occ;
} sw_state_example_large_DS_allocations_t;

typedef struct {
    
    uint32 my_variable;
    
    uint32 rollback_comparison_excluded;
    
    uint32 my_array[10];
    
    uint32 two_dimentional_array[10][10];
    
    uint32* dynamic_array;
    
    uint32* dynamic_array_zero_size;
    
    uint32** dynamic_dynamic_array;
    
    uint32 (*dynamic_array_static)[5];
    uint32 counter_test;
    
    sw_state_mutex_t mutex_test;
    
    sw_state_sem_t sem_test;
    
    uint32* DNXData_array_alloc_exception;
    
    uint32* DNXData_array;
    
    uint32** DNXData_array_dyn_dnxdata;
    
    uint32** DNXData_array_dnxdata_dyn;
    
    uint32** DNXData_array_dnxdata_dnxdata;
    
    uint32** DNXData_array_static_dnxdata_dyn[3];
    
    uint32** DNXData_array_static_dyn_dnxdata[3];
    
    uint32** DNXData_array_static_dnxdata_dnxdata[3];
    
    uint32* DNXData_array_table_data;
    int value_range_test;
    
    uint32 array_range_example[10];
    
    uint32* alloc_after_init_variable;
    
    uint32* alloc_after_init_test;
    
    uint32* pointer;
    
    SHR_BITDCL* bitmap_variable;
    
    bcm_pbmp_t pbmp_variable;
    
    SHR_BITDCL bitmap_fixed[((1024)/SHR_BITWID)+1];
    
    SHR_BITDCL* bitmap_dnx_data;
    
    DNX_SW_STATE_BUFF* buffer;
    
    DNX_SW_STATE_BUFF buffer_fixed[1024];
    
    SHR_BITDCL* bitmap_exclude_example;
    
    sw_state_default_l1 default_value_tree;
    
    sw_state_ll_t ll;
    
    sw_state_multihead_ll_t multihead_ll;
    
    sw_state_sorted_ll_t sorted_ll;
    
    sw_state_bt_t bt;
    
    sw_state_htbl_t htb;
    
    sw_state_htbl_t htb_arr[2];
    
    sw_state_htbl_t index_htb;
    
    sw_state_occ_bm_t occ;
    
    sw_state_cb_t cb;
    
    sw_state_leaf_struct leaf_struct;
    sw_state_defragmented_chunk_t defragmented_chunk_example[5];
    sw_state_packed packed;
    sw_state_example_union my_union;
    sw_state_string_t string[100];
    sw_state_example_child_size_check_t* alloc_child_check;
    int write_during_wb_example;
    sw_state_example_large_DS_allocations_t large_DS_examples;
    dnx_sw_state_pretty_print_example_t pretty_print_example;
    
    DNX_SW_STATE_BUFF** buffer_array;
    
    SHR_BITDCL** bitmap_array;
} sw_state_example;

#endif  

#endif 
