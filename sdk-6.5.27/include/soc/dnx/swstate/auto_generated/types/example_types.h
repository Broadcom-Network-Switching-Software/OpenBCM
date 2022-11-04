
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __EXAMPLE_TYPES_H__
#define __EXAMPLE_TYPES_H__

#if defined(INCLUDE_CTEST)
#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/dnx/dnx_data/auto_generated/dnx_data_bfd.h>
#endif  
#include <soc/dnx/dnx_data/auto_generated/dnx_data_infra_tests.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_module_testing.h>
#include <soc/dnxc/swstate/callbacks/sw_state_htb_callbacks.h>
#include <soc/dnxc/swstate/callbacks/sw_state_ll_callbacks.h>
#include <soc/dnxc/swstate/dnx_sw_state_pretty_dump.h>
#include <soc/dnxc/swstate/types/sw_state_binary_tree.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>
#include <soc/dnxc/swstate/types/sw_state_cb.h>
#include <soc/dnxc/swstate/types/sw_state_defragmented_chunk.h>
#include <soc/dnxc/swstate/types/sw_state_hash_table.h>
#include <soc/dnxc/swstate/types/sw_state_htb.h>
#include <soc/dnxc/swstate/types/sw_state_index_hash_table.h>
#include <soc/dnxc/swstate/types/sw_state_index_htb.h>
#include <soc/dnxc/swstate/types/sw_state_linked_list.h>
#include <soc/dnxc/swstate/types/sw_state_mutex.h>
#include <soc/dnxc/swstate/types/sw_state_occupation_bitmap.h>
#include <soc/dnxc/swstate/types/sw_state_pbmp.h>
#include <soc/dnxc/swstate/types/sw_state_sem.h>
#include <soc/dnxc/swstate/types/sw_state_string.h>


#define SW_STATE_EXAMPLE_DEFINITION 10

#ifdef BCM_WARM_BOOT_SUPPORT
#define SW_STATE_EXAMPLE_DEFINITION_WARMBOOT 60

#endif  
#if defined(BCM_WARM_BOOT_SUPPORT) && defined(BCM_DNX_SUPPORT) 
#define SW_STATE_EXAMPLE_DEFINITION_IFDEF_MULTIPLE_FLAGS 80

#endif  
#if !defined(BCM_WARM_BOOT_SUPPORT) && !defined(BCM_DNX_SUPPORT) 
#define SW_STATE_EXAMPLE_DEFINITION_IFNDEF_MULTIPLE_FLAGS 80

#endif  


typedef uint8 (*dnx_sw_state_callback_test_function_cb) (int unit)  ;

#ifdef BCM_DNX_SUPPORT
typedef uint8 (*dnx_sw_state_callback_ifdef_test_function_cb) (int unit)  ;

#endif  


typedef enum {
    DNX_EXAMPLE_ENUM_NEGATIVE = -1,
    DNX_EXAMPLE_ENUM_ZERO = 0,
    DNX_EXAMPLE_ENUM_ONE,
    
    DNX_EXAMPLE_NAME_E_DUMMY
} dnx_example_name_e;

typedef enum {
#ifdef BCM_DNX_SUPPORT
    DNX_EXAMPLE_STANDARD_ENUM_ENTRY_IFDEF = 0,
#endif  
#ifndef BCM_DNX_SUPPORT
    DNX_EXAMPLE_STANDARD_ENUM_ENTRY_IFNDEF = 1,
#endif  
#if defined(BCM_DNX_SUPPORT)
    DNX_EXAMPLE_STANDARD_ENUM_ENTRY_IF = 2,
#endif  
    DNX_EXAMPLE_STANDARD_ENUM_ENTRY_STANDARD = 4,
    
    DNX_EXAMPLE_INNER_IFDEF_E_DUMMY
} dnx_example_inner_ifdef_e;

#ifdef BCM_DNX_SUPPORT
typedef enum {
    DNX_EXAMPLE_IFDEF_ENUM_ENTRY_FAMILY = 1,
    DNX_EXAMPLE_IFDEF_ENUM_ENTRY_STANDARD = 2,
    
    DNX_EXAMPLE_IFDEF_E_DUMMY
} dnx_example_ifdef_e;
#endif  






#ifdef BCM_WARM_BOOT_SUPPORT
typedef struct {
    int var;
    DNX_SW_STATE_BUFF buff_variable[10];
} sw_state_ifdef;

#endif  
typedef struct {
#ifdef BCM_WARM_BOOT_SUPPORT
    int var_warm_boot_ifdef;
#endif  
#ifndef BCM_WARM_BOOT_SUPPORT
    int var_warm_boot_ifndef;
#endif  
#if defined(BCM_WARM_BOOT_SUPPORT)
    int var_warm_boot_if;
#endif  
#if defined(BCM_WARM_BOOT_SUPPORT) && defined(BCM_DNX_SUPPORT) 
    int integer_multiple_flags;
#endif  
    DNX_SW_STATE_BUFF buff_variable[10];
} sw_state_params_flags;

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
    sw_state_default_value** array_dynamic_dynamic;
} sw_state_default_l3;

typedef struct {
    sw_state_default_l3* array_dynamic;
    sw_state_default_value default_value_l2;
} sw_state_default_l2;

typedef struct {
    sw_state_default_l2* array_dnxdata;
    sw_state_default_value default_value_l1;
} sw_state_default_l1;

typedef struct {
    char expecto_patronum;
    int my_integer;
} sw_state_packed;

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
    sw_state_htbl_t large_htb[2];
    sw_state_idx_htbl_t large_index_htb;
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
    uint32* DNXData_array_alloc_exception;
    uint32* DNXData_array;
    uint32** DNXData_array_dyn_dnxdata;
    uint32** DNXData_array_dnxdata_dyn;
    uint32** DNXData_array_dnxdata_dnxdata;
    uint32** DNXData_array_static_dnxdata_dyn[3];
    uint32** DNXData_array_static_dyn_dnxdata[3];
    uint32** DNXData_array_static_dnxdata_dnxdata[3];
    uint32* DNXData_array_table_data;
    uint32 array_range_example[10];
    int value_range_test;
    SHR_BITDCL* bitmap_variable;
    SHR_BITDCL bitmap_fixed[((1024)/SHR_BITWID)+1];
    SHR_BITDCL* bitmap_dnx_data;
    SHR_BITDCL* bitmap_exclude_example;
    SHR_BITDCL* bitmap_include_only_example;
    SHR_BITDCL** bitmap_array;
    SHR_BITDCL bitmap_variable_access[((10)/SHR_BITWID)+1];
    DNX_SW_STATE_BUFF** buffer_array;
    DNX_SW_STATE_BUFF* buffer;
    DNX_SW_STATE_BUFF buffer_fixed[1024];
    sw_state_default_l1 default_value_tree;
    sw_state_ll_t ll;
    sw_state_multihead_ll_t multihead_ll;
    sw_state_sorted_multihead_ll_t sorted_multihead_ll;
    sw_state_sorted_ll_t sorted_ll;
    sw_state_bt_t bt;
    sw_state_htbl_t htb[2];
    sw_state_htb_t htb_rh;
    sw_state_multihead_htb_t multihead_htb_rh;
    sw_state_index_htb_t index_htb_rh;
    sw_state_htbl_t htb_arr[2];
    sw_state_idx_htbl_t index_htb;
    sw_state_occ_bm_t occ;
    sw_state_occ_bm_t occ2;
    sw_state_cb_t cb;
    sw_state_leaf_struct leaf_struct;
    sw_state_defragmented_chunk_t defragmented_chunk_example[6];
    sw_state_packed packed;
    sw_state_params_flags params_flags;
    sw_state_example_union my_union;
    sw_state_string_t string[100];
    sw_state_example_child_size_check_t* alloc_child_check;
    bcm_pbmp_t pbmp_variable;
    sw_state_example_large_DS_allocations_t large_DS_examples;
    dnx_sw_state_pretty_print_example_t pretty_print_example;
    sw_state_mutex_t mutex_test;
    sw_state_sem_t sem_test;
    int write_during_wb_example;
    uint32* alloc_after_init_variable;
    uint32* alloc_after_init_test;
} sw_state_example;

#endif  

#endif 
