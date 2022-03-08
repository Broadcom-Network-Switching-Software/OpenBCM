
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#if defined(INCLUDE_CTEST)
#include <soc/dnx/swstate/auto_generated/types/example_types.h>
#include <soc/dnx/swstate/auto_generated/layout/example_layout.h>

dnxc_sw_state_layout_t layout_array_example[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE_NOF_PARAMS)];

shr_error_e
example_init_layout_structure(int unit)
{

    uint32 example__dynamic_array_static__default_val = -1;
    SHR_BITDCL example__bitmap_variable__default_val = 1;
    DNX_SW_STATE_BUFF example__buffer__default_val = 0xAA;
    uint32 example__default_value_tree__array_dnxdata__array_dynamic__array_static_static__my_variable__default_val = 5;
    uint32 example__default_value_tree__array_dnxdata__array_dynamic__default_value_l3__my_variable__default_val = 5;
    uint32 example__default_value_tree__array_dnxdata__array_dynamic__array_dynamic_dynamic__my_variable__default_val = 5;
    uint32 example__default_value_tree__array_dnxdata__default_value_l2__my_variable__default_val = 5;
    uint32 example__default_value_tree__default_value_l1__my_variable__default_val = 5;


    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE);
    layout_array_example[idx].name = "example";
    layout_array_example[idx].type = "sw_state_example";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(sw_state_example);
    layout_array_example[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].first_child_index = 1;
    layout_array_example[idx].last_child_index = 59;
    layout_array_example[idx].next_node_index = 1;
    layout_array_example[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_GENERATE_FREE;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__MY_VARIABLE);
    layout_array_example[idx].name = "example__my_variable";
    layout_array_example[idx].type = "uint32";
    layout_array_example[idx].doc = "example variable";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(uint32);
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 2;
    layout_array_example[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__ROLLBACK_COMPARISON_EXCLUDED);
    layout_array_example[idx].name = "example__rollback_comparison_excluded";
    layout_array_example[idx].type = "uint32";
    layout_array_example[idx].doc = "variable used for testing permanent rollback and comparison journal exclusions";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(uint32);
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 4;
    layout_array_example[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__MY_ARRAY);
    layout_array_example[idx].name = "example__my_array";
    layout_array_example[idx].type = "uint32";
    layout_array_example[idx].doc = "example 10 cell array";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(uint32);
    layout_array_example[idx].array_indexes[0].size = 10;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 5;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__TWO_DIMENTIONAL_ARRAY);
    layout_array_example[idx].name = "example__two_dimentional_array";
    layout_array_example[idx].type = "uint32";
    layout_array_example[idx].doc = "example two dimentional array";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(uint32);
    layout_array_example[idx].array_indexes[0].name = "index_1";
    layout_array_example[idx].array_indexes[0].size = 10;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_example[idx].array_indexes[1].name = "index_2";
    layout_array_example[idx].array_indexes[1].size = 10;
    layout_array_example[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 6;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY);
    layout_array_example[idx].name = "example__dynamic_array";
    layout_array_example[idx].type = "uint32*";
    layout_array_example[idx].doc = "example dynamic array";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(uint32*);
    layout_array_example[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 7;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_ZERO_SIZE);
    layout_array_example[idx].name = "example__dynamic_array_zero_size";
    layout_array_example[idx].type = "uint32*";
    layout_array_example[idx].doc = "example dynamic array with zero size";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(uint32*);
    layout_array_example[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 8;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__DYNAMIC_DYNAMIC_ARRAY);
    layout_array_example[idx].name = "example__dynamic_dynamic_array";
    layout_array_example[idx].type = "uint32**";
    layout_array_example[idx].doc = "example 2d dynamic array";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(uint32**);
    layout_array_example[idx].array_indexes[0].name = "first_d_index";
    layout_array_example[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_example[idx].array_indexes[1].name = "dynamic_index";
    layout_array_example[idx].array_indexes[1].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 9;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_STATIC);
    layout_array_example[idx].name = "example__dynamic_array_static";
    layout_array_example[idx].type = "uint32*";
    layout_array_example[idx].doc = "example dynamic array of static array";
    layout_array_example[idx].default_value= &(example__dynamic_array_static__default_val);
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(uint32*);
    layout_array_example[idx].array_indexes[0].name = "dynamic_index";
    layout_array_example[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_example[idx].array_indexes[1].name = "static_index";
    layout_array_example[idx].array_indexes[1].size = 5;
    layout_array_example[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 10;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__COUNTER_TEST);
    layout_array_example[idx].name = "example__counter_test";
    layout_array_example[idx].type = "uint32";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(uint32);
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 11;
    layout_array_example[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_COUNTER;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_ALLOC_EXCEPTION);
    layout_array_example[idx].name = "example__DNXData_array_alloc_exception";
    layout_array_example[idx].type = "uint32*";
    layout_array_example[idx].doc = "Example of dynamic array with alloc exception.";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(uint32*);
    layout_array_example[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 12;
    layout_array_example[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ALLOC_EXCEPTION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY);
    layout_array_example[idx].name = "example__DNXData_array";
    layout_array_example[idx].type = "uint32*";
    layout_array_example[idx].doc = "Example DNXData array.";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(uint32*);
    layout_array_example[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 13;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DYN_DNXDATA);
    layout_array_example[idx].name = "example__DNXData_array_dyn_dnxdata";
    layout_array_example[idx].type = "uint32**";
    layout_array_example[idx].doc = "Example two dimension DNXData array, second dimension is DNX Data.";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(uint32**);
    layout_array_example[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_example[idx].array_indexes[1].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 14;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DYN);
    layout_array_example[idx].name = "example__DNXData_array_dnxdata_dyn";
    layout_array_example[idx].type = "uint32**";
    layout_array_example[idx].doc = "Example two dimension DNXData array, first dimension is DNX Data.";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(uint32**);
    layout_array_example[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_example[idx].array_indexes[1].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 15;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DNXDATA);
    layout_array_example[idx].name = "example__DNXData_array_dnxdata_dnxdata";
    layout_array_example[idx].type = "uint32**";
    layout_array_example[idx].doc = "Example two dimension DNXData array, both dimensions are DNX Data.";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(uint32**);
    layout_array_example[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_example[idx].array_indexes[1].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 16;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DYN);
    layout_array_example[idx].name = "example__DNXData_array_static_dnxdata_dyn";
    layout_array_example[idx].type = "uint32**";
    layout_array_example[idx].doc = "Example three dimension DNXData array, first static, second is DNX Data and third dynamic.";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(uint32**);
    layout_array_example[idx].array_indexes[0].size = 3;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_example[idx].array_indexes[1].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_example[idx].array_indexes[2].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[2].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 17;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DYN_DNXDATA);
    layout_array_example[idx].name = "example__DNXData_array_static_dyn_dnxdata";
    layout_array_example[idx].type = "uint32**";
    layout_array_example[idx].doc = "Example three dimension DNXData array, first static, third is DNX Data and third dynamic.";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(uint32**);
    layout_array_example[idx].array_indexes[0].size = 3;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_example[idx].array_indexes[1].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_example[idx].array_indexes[2].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[2].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 18;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA);
    layout_array_example[idx].name = "example__DNXData_array_static_dnxdata_dnxdata";
    layout_array_example[idx].type = "uint32**";
    layout_array_example[idx].doc = "Example three dimension DNXData array, first static, second and third is DNX Data.";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(uint32**);
    layout_array_example[idx].array_indexes[0].size = 3;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_example[idx].array_indexes[1].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_example[idx].array_indexes[2].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[2].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 19;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_TABLE_DATA);
    layout_array_example[idx].name = "example__DNXData_array_table_data";
    layout_array_example[idx].type = "uint32*";
    layout_array_example[idx].doc = "Example DNXData array that uses data from DNX table.";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(uint32*);
    layout_array_example[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 20;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__ARRAY_RANGE_EXAMPLE);
    layout_array_example[idx].name = "example__array_range_example";
    layout_array_example[idx].type = "uint32";
    layout_array_example[idx].doc = "example  array with label=ARRAY_RANGE_FUNCTIONS";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(uint32);
    layout_array_example[idx].array_indexes[0].size = 10;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 21;
    layout_array_example[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ARRAY_RANGE_FUNCTIONS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_VARIABLE);
    layout_array_example[idx].name = "example__alloc_after_init_variable";
    layout_array_example[idx].type = "uint32*";
    layout_array_example[idx].doc = "alloc after init test variable";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(uint32*);
    layout_array_example[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 22;
    layout_array_example[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ALLOC_AFTER_INIT;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_TEST);
    layout_array_example[idx].name = "example__alloc_after_init_test";
    layout_array_example[idx].type = "uint32*";
    layout_array_example[idx].doc = "alloc after init test variable";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(uint32*);
    layout_array_example[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 23;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__WRITE_DURING_WB_EXAMPLE);
    layout_array_example[idx].name = "example__write_during_wb_example";
    layout_array_example[idx].type = "int";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(int);
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 24;
    layout_array_example[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ALLOW_WRITES_DURING_WB;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__VALUE_RANGE_TEST);
    layout_array_example[idx].name = "example__value_range_test";
    layout_array_example[idx].type = "int";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(int);
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 25;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE);
    layout_array_example[idx].name = "example__bitmap_variable";
    layout_array_example[idx].type = "SHR_BITDCL*";
    layout_array_example[idx].doc = "Bitmap variable example.";
    layout_array_example[idx].default_value= &(example__bitmap_variable__default_val);
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(SHR_BITDCL*);
    layout_array_example[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_BITMAP;
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 26;
    layout_array_example[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_GENERATE_FREE;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__BITMAP_FIXED);
    layout_array_example[idx].name = "example__bitmap_fixed";
    layout_array_example[idx].type = "SHR_BITDCL";
    layout_array_example[idx].doc = "Bitmap fixed example.";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(SHR_BITDCL);
    layout_array_example[idx].array_indexes[0].size = ((1024)/SHR_BITWID)+1;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_BITMAP;
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 27;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA);
    layout_array_example[idx].name = "example__bitmap_dnx_data";
    layout_array_example[idx].type = "SHR_BITDCL*";
    layout_array_example[idx].doc = "Bitmap dnx data example.";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(SHR_BITDCL*);
    layout_array_example[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_BITMAP;
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 28;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE);
    layout_array_example[idx].name = "example__bitmap_exclude_example";
    layout_array_example[idx].type = "SHR_BITDCL*";
    layout_array_example[idx].doc = "Bitmap variable example.";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(SHR_BITDCL*);
    layout_array_example[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_BITMAP;
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 29;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__BITMAP_INCLUDE_ONLY_EXAMPLE);
    layout_array_example[idx].name = "example__bitmap_include_only_example";
    layout_array_example[idx].type = "SHR_BITDCL*";
    layout_array_example[idx].doc = "Bitmap variable example.";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(SHR_BITDCL*);
    layout_array_example[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_BITMAP;
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 30;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY);
    layout_array_example[idx].name = "example__bitmap_array";
    layout_array_example[idx].type = "SHR_BITDCL**";
    layout_array_example[idx].doc = "Bitmap array example.";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(SHR_BITDCL**);
    layout_array_example[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_example[idx].array_indexes[1].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_BITMAP;
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 31;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS);
    layout_array_example[idx].name = "example__bitmap_variable_access";
    layout_array_example[idx].type = "SHR_BITDCL";
    layout_array_example[idx].doc = "Bitmap variable example for ACCESS_PTR label.";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(SHR_BITDCL);
    layout_array_example[idx].array_indexes[0].size = ((10)/SHR_BITWID)+1;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_BITMAP;
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 32;
    layout_array_example[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ACCESS_PTR;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY);
    layout_array_example[idx].name = "example__buffer_array";
    layout_array_example[idx].type = "DNX_SW_STATE_BUFF**";
    layout_array_example[idx].doc = "buffer array variable";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(DNX_SW_STATE_BUFF**);
    layout_array_example[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_example[idx].array_indexes[1].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_BUFFER;
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 33;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__BUFFER);
    layout_array_example[idx].name = "example__buffer";
    layout_array_example[idx].type = "DNX_SW_STATE_BUFF*";
    layout_array_example[idx].doc = "buffer variable";
    layout_array_example[idx].default_value= &(example__buffer__default_val);
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(DNX_SW_STATE_BUFF*);
    layout_array_example[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_BUFFER;
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 34;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__BUFFER_FIXED);
    layout_array_example[idx].name = "example__buffer_fixed";
    layout_array_example[idx].type = "DNX_SW_STATE_BUFF";
    layout_array_example[idx].doc = "fixed buffer variable";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(DNX_SW_STATE_BUFF);
    layout_array_example[idx].array_indexes[0].size = 1024;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_BUFFER;
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 35;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE);
    layout_array_example[idx].name = "example__default_value_tree";
    layout_array_example[idx].type = "sw_state_default_l1";
    layout_array_example[idx].doc = "default value tree.";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(sw_state_default_l1);
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = 60;
    layout_array_example[idx].last_child_index = 62;
    layout_array_example[idx].next_node_index = 36;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__LL);
    layout_array_example[idx].name = "example__ll";
    layout_array_example[idx].type = "sw_state_ll_t";
    layout_array_example[idx].doc = "Linked List example.";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(sw_state_ll_t);
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 37;
    layout_array_example[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_EXPANDED_INTERFACE;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL);
    layout_array_example[idx].name = "example__multihead_ll";
    layout_array_example[idx].type = "sw_state_multihead_ll_t";
    layout_array_example[idx].doc = "MultiHead Linked List example.";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(sw_state_multihead_ll_t);
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 39;
    layout_array_example[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_EXPANDED_INTERFACE;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__SORTED_LL);
    layout_array_example[idx].name = "example__sorted_ll";
    layout_array_example[idx].type = "sw_state_sorted_ll_t";
    layout_array_example[idx].doc = "Sorted Linked List example.";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(sw_state_sorted_ll_t);
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 40;
    layout_array_example[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_EXPANDED_INTERFACE;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__BT);
    layout_array_example[idx].name = "example__bt";
    layout_array_example[idx].type = "sw_state_bt_t";
    layout_array_example[idx].doc = "Binary Tree example.";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(sw_state_bt_t);
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 41;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__HTB);
    layout_array_example[idx].name = "example__htb";
    layout_array_example[idx].type = "sw_state_htbl_t";
    layout_array_example[idx].doc = "Hash Table example.";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(sw_state_htbl_t);
    layout_array_example[idx].array_indexes[0].size = 2;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 42;
    layout_array_example[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_EXPANDED_INTERFACE;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__HTB_ARR);
    layout_array_example[idx].name = "example__htb_arr";
    layout_array_example[idx].type = "sw_state_htbl_t";
    layout_array_example[idx].doc = "array of hash tables with different key/value sizes";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(sw_state_htbl_t);
    layout_array_example[idx].array_indexes[0].size = 2;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 43;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__INDEX_HTB);
    layout_array_example[idx].name = "example__index_htb";
    layout_array_example[idx].type = "sw_state_htbl_t";
    layout_array_example[idx].doc = "Index Hash Table example.";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(sw_state_htbl_t);
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 44;
    layout_array_example[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_EXPANDED_INTERFACE;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__OCC);
    layout_array_example[idx].name = "example__occ";
    layout_array_example[idx].type = "sw_state_occ_bm_t";
    layout_array_example[idx].doc = "Occupation Bitmap example.";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(sw_state_occ_bm_t);
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 45;
    layout_array_example[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_EXPANDED_INTERFACE;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__OCC2);
    layout_array_example[idx].name = "example__occ2";
    layout_array_example[idx].type = "sw_state_occ_bm_t";
    layout_array_example[idx].doc = "Occupation Bitmap another example.";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(sw_state_occ_bm_t);
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 46;
    layout_array_example[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_EXPANDED_INTERFACE;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__CB);
    layout_array_example[idx].name = "example__cb";
    layout_array_example[idx].type = "sw_state_cb_t";
    layout_array_example[idx].doc = "Callback example.";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(sw_state_cb_t);
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 47;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__LEAF_STRUCT);
    layout_array_example[idx].name = "example__leaf_struct";
    layout_array_example[idx].type = "sw_state_leaf_struct";
    layout_array_example[idx].doc = "Sw State leaf struct.";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(sw_state_leaf_struct);
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = 76;
    layout_array_example[idx].last_child_index = 78;
    layout_array_example[idx].next_node_index = 48;
    layout_array_example[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_LEAF_STRUCT;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE);
    layout_array_example[idx].name = "example__defragmented_chunk_example";
    layout_array_example[idx].type = "sw_state_defragmented_chunk_t";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(sw_state_defragmented_chunk_t);
    layout_array_example[idx].array_indexes[0].size = 6;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 49;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__PACKED);
    layout_array_example[idx].name = "example__packed";
    layout_array_example[idx].type = "sw_state_packed";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(sw_state_packed);
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = 79;
    layout_array_example[idx].last_child_index = 80;
    layout_array_example[idx].next_node_index = 50;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS);
    layout_array_example[idx].name = "example__params_flags";
    layout_array_example[idx].type = "sw_state_params_flags";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(sw_state_params_flags);
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = 83;
    layout_array_example[idx].last_child_index = 93;
    layout_array_example[idx].next_node_index = 52;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__MY_UNION);
    layout_array_example[idx].name = "example__my_union";
    layout_array_example[idx].type = "sw_state_example_union";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(sw_state_example_union);
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = 94;
    layout_array_example[idx].last_child_index = 95;
    layout_array_example[idx].next_node_index = 53;
    layout_array_example[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_UNION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__STRING);
    layout_array_example[idx].name = "example__string";
    layout_array_example[idx].type = "sw_state_string_t";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(sw_state_string_t);
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 54;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK);
    layout_array_example[idx].name = "example__alloc_child_check";
    layout_array_example[idx].type = "sw_state_example_child_size_check_t*";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(sw_state_example_child_size_check_t*);
    layout_array_example[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = 96;
    layout_array_example[idx].last_child_index = 98;
    layout_array_example[idx].next_node_index = 55;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE);
    layout_array_example[idx].name = "example__pbmp_variable";
    layout_array_example[idx].type = "bcm_pbmp_t";
    layout_array_example[idx].doc = "Port bitmap variable example.";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(bcm_pbmp_t);
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 56;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES);
    layout_array_example[idx].name = "example__large_DS_examples";
    layout_array_example[idx].type = "sw_state_example_large_DS_allocations_t";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(sw_state_example_large_DS_allocations_t);
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = 99;
    layout_array_example[idx].last_child_index = 100;
    layout_array_example[idx].next_node_index = 57;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__PRETTY_PRINT_EXAMPLE);
    layout_array_example[idx].name = "example__pretty_print_example";
    layout_array_example[idx].type = "dnx_sw_state_pretty_print_example_t";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(dnx_sw_state_pretty_print_example_t);
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 58;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__MUTEX_TEST);
    layout_array_example[idx].name = "example__mutex_test";
    layout_array_example[idx].type = "sw_state_mutex_t";
    layout_array_example[idx].doc = "example mutex variable";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(sw_state_mutex_t);
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 59;
    layout_array_example[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_GENERATE_FREE;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__SEM_TEST);
    layout_array_example[idx].name = "example__sem_test";
    layout_array_example[idx].type = "sw_state_sem_t";
    layout_array_example[idx].doc = "example semapthore variable";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(sw_state_sem_t);
    layout_array_example[idx].parent  = 0;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA);
    layout_array_example[idx].name = "example__default_value_tree__array_dnxdata";
    layout_array_example[idx].type = "sw_state_default_l2*";
    layout_array_example[idx].doc = "static array of default variable type";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(sw_state_default_l2*);
    layout_array_example[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_example[idx].parent  = 35;
    layout_array_example[idx].first_child_index = 63;
    layout_array_example[idx].last_child_index = 64;
    layout_array_example[idx].next_node_index = 61;
    layout_array_example[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_GENERATE_FREE;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__DEFAULT_VALUE_L1);
    layout_array_example[idx].name = "example__default_value_tree__default_value_l1";
    layout_array_example[idx].type = "sw_state_default_value";
    layout_array_example[idx].doc = "example variable";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(sw_state_default_value);
    layout_array_example[idx].parent  = 35;
    layout_array_example[idx].first_child_index = 74;
    layout_array_example[idx].last_child_index = 74;
    layout_array_example[idx].next_node_index = 62;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC);
    layout_array_example[idx].name = "example__default_value_tree__array_dnxdata__array_dynamic";
    layout_array_example[idx].type = "sw_state_default_l3*";
    layout_array_example[idx].doc = "dynamic array of l3";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(sw_state_default_l3*);
    layout_array_example[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_example[idx].parent  = 60;
    layout_array_example[idx].first_child_index = 65;
    layout_array_example[idx].last_child_index = 68;
    layout_array_example[idx].next_node_index = 64;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__DEFAULT_VALUE_L2);
    layout_array_example[idx].name = "example__default_value_tree__array_dnxdata__default_value_l2";
    layout_array_example[idx].type = "sw_state_default_value";
    layout_array_example[idx].doc = "example variable";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(sw_state_default_value);
    layout_array_example[idx].parent  = 60;
    layout_array_example[idx].first_child_index = 73;
    layout_array_example[idx].last_child_index = 73;
    layout_array_example[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_STATIC_STATIC);
    layout_array_example[idx].name = "example__default_value_tree__array_dnxdata__array_dynamic__array_static_static";
    layout_array_example[idx].type = "sw_state_default_value";
    layout_array_example[idx].doc = "static array of default variable type";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(sw_state_default_value);
    layout_array_example[idx].array_indexes[0].size = SW_STATE_EXAMPLE_DEFINITION;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_example[idx].array_indexes[1].size = 10;
    layout_array_example[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_example[idx].parent  = 63;
    layout_array_example[idx].first_child_index = 69;
    layout_array_example[idx].last_child_index = 69;
    layout_array_example[idx].next_node_index = 66;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__DEFAULT_VALUE_L3);
    layout_array_example[idx].name = "example__default_value_tree__array_dnxdata__array_dynamic__default_value_l3";
    layout_array_example[idx].type = "sw_state_default_value";
    layout_array_example[idx].doc = "example variable";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(sw_state_default_value);
    layout_array_example[idx].parent  = 63;
    layout_array_example[idx].first_child_index = 70;
    layout_array_example[idx].last_child_index = 70;
    layout_array_example[idx].next_node_index = 67;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC);
    layout_array_example[idx].name = "example__default_value_tree__array_dnxdata__array_dynamic__array_dynamic_dynamic";
    layout_array_example[idx].type = "sw_state_default_value**";
    layout_array_example[idx].doc = "dynamic array of default variable type";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(sw_state_default_value**);
    layout_array_example[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_example[idx].array_indexes[1].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_example[idx].parent  = 63;
    layout_array_example[idx].first_child_index = 71;
    layout_array_example[idx].last_child_index = 71;
    layout_array_example[idx].next_node_index = 68;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_STATIC_STATIC__MY_VARIABLE);
    layout_array_example[idx].name = "example__default_value_tree__array_dnxdata__array_dynamic__array_static_static__my_variable";
    layout_array_example[idx].type = "uint32";
    layout_array_example[idx].doc = "example variable with default value";
    layout_array_example[idx].default_value= &(example__default_value_tree__array_dnxdata__array_dynamic__array_static_static__my_variable__default_val);
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(uint32);
    layout_array_example[idx].parent  = 65;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__DEFAULT_VALUE_L3__MY_VARIABLE);
    layout_array_example[idx].name = "example__default_value_tree__array_dnxdata__array_dynamic__default_value_l3__my_variable";
    layout_array_example[idx].type = "uint32";
    layout_array_example[idx].doc = "example variable with default value";
    layout_array_example[idx].default_value= &(example__default_value_tree__array_dnxdata__array_dynamic__default_value_l3__my_variable__default_val);
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(uint32);
    layout_array_example[idx].parent  = 66;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC__MY_VARIABLE);
    layout_array_example[idx].name = "example__default_value_tree__array_dnxdata__array_dynamic__array_dynamic_dynamic__my_variable";
    layout_array_example[idx].type = "uint32";
    layout_array_example[idx].doc = "example variable with default value";
    layout_array_example[idx].default_value= &(example__default_value_tree__array_dnxdata__array_dynamic__array_dynamic_dynamic__my_variable__default_val);
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(uint32);
    layout_array_example[idx].parent  = 67;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__DEFAULT_VALUE_L2__MY_VARIABLE);
    layout_array_example[idx].name = "example__default_value_tree__array_dnxdata__default_value_l2__my_variable";
    layout_array_example[idx].type = "uint32";
    layout_array_example[idx].doc = "example variable with default value";
    layout_array_example[idx].default_value= &(example__default_value_tree__array_dnxdata__default_value_l2__my_variable__default_val);
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(uint32);
    layout_array_example[idx].parent  = 64;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__DEFAULT_VALUE_L1__MY_VARIABLE);
    layout_array_example[idx].name = "example__default_value_tree__default_value_l1__my_variable";
    layout_array_example[idx].type = "uint32";
    layout_array_example[idx].doc = "example variable with default value";
    layout_array_example[idx].default_value= &(example__default_value_tree__default_value_l1__my_variable__default_val);
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(uint32);
    layout_array_example[idx].parent  = 61;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__LEAF_STRUCT__VAR);
    layout_array_example[idx].name = "example__leaf_struct__var";
    layout_array_example[idx].type = "int";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(int);
    layout_array_example[idx].parent  = 47;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 77;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__LEAF_STRUCT__BUFF_VARIABLE);
    layout_array_example[idx].name = "example__leaf_struct__buff_variable";
    layout_array_example[idx].type = "DNX_SW_STATE_BUFF";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(DNX_SW_STATE_BUFF);
    layout_array_example[idx].array_indexes[0].size = 10;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_BUFFER;
    layout_array_example[idx].parent  = 47;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__PACKED__EXPECTO_PATRONUM);
    layout_array_example[idx].name = "example__packed__expecto_patronum";
    layout_array_example[idx].type = "char";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(char);
    layout_array_example[idx].parent  = 49;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 80;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__PACKED__MY_INTEGER);
    layout_array_example[idx].name = "example__packed__my_integer";
    layout_array_example[idx].type = "int";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(int);
    layout_array_example[idx].parent  = 49;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

#ifdef BCM_WARM_BOOT_SUPPORT
    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IFDEF);
    layout_array_example[idx].name = "example__params_flags__var_warm_boot_ifdef";
    layout_array_example[idx].type = "int";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(int);
    layout_array_example[idx].parent  = 51;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 84;

#endif  
#ifndef BCM_WARM_BOOT_SUPPORT
    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IFNDEF);
    layout_array_example[idx].name = "example__params_flags__var_warm_boot_ifndef";
    layout_array_example[idx].type = "int";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(int);
    layout_array_example[idx].parent  = 51;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 85;

#endif  
#if defined(BCM_WARM_BOOT_SUPPORT)
    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IF);
    layout_array_example[idx].name = "example__params_flags__var_warm_boot_if";
    layout_array_example[idx].type = "int";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(int);
    layout_array_example[idx].parent  = 51;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 86;

#endif  
#if defined(BCM_WARM_BOOT_SUPPORT) && defined(BCM_DNX_SUPPORT) 
    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__INTEGER_MULTIPLE_FLAGS);
    layout_array_example[idx].name = "example__params_flags__integer_multiple_flags";
    layout_array_example[idx].type = "int";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(int);
    layout_array_example[idx].parent  = 51;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 89;

#endif  
    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__BUFF_VARIABLE);
    layout_array_example[idx].name = "example__params_flags__buff_variable";
    layout_array_example[idx].type = "DNX_SW_STATE_BUFF";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(DNX_SW_STATE_BUFF);
    layout_array_example[idx].array_indexes[0].size = 10;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_BUFFER;
    layout_array_example[idx].parent  = 51;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__MY_UNION__MY_INTEGER);
    layout_array_example[idx].name = "example__my_union__my_integer";
    layout_array_example[idx].type = "int";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(int);
    layout_array_example[idx].parent  = 52;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 95;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__MY_UNION__MY_CHAR);
    layout_array_example[idx].name = "example__my_union__my_char";
    layout_array_example[idx].type = "char";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(char);
    layout_array_example[idx].parent  = 52;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY_OF_P);
    layout_array_example[idx].name = "example__alloc_child_check__static_array_of_p";
    layout_array_example[idx].type = "int*";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(int*);
    layout_array_example[idx].array_indexes[0].size = 10000;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_example[idx].array_indexes[1].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_example[idx].parent  = 54;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 97;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY);
    layout_array_example[idx].name = "example__alloc_child_check__static_array";
    layout_array_example[idx].type = "int";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(int);
    layout_array_example[idx].array_indexes[0].size = 20000;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_example[idx].parent  = 54;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 98;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__INTEGER_VARIABLE);
    layout_array_example[idx].name = "example__alloc_child_check__integer_variable";
    layout_array_example[idx].type = "int";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(int);
    layout_array_example[idx].parent  = 54;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB);
    layout_array_example[idx].name = "example__large_DS_examples__large_htb";
    layout_array_example[idx].type = "sw_state_htbl_t";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(sw_state_htbl_t);
    layout_array_example[idx].array_indexes[0].size = 2;
    layout_array_example[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_example[idx].parent  = 56;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = 100;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB);
    layout_array_example[idx].name = "example__large_DS_examples__large_index_htb";
    layout_array_example[idx].type = "sw_state_htbl_t";
    layout_array_example[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].size_of = sizeof(sw_state_htbl_t);
    layout_array_example[idx].parent  = 56;
    layout_array_example[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_example[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_EXAMPLE, layout_array_example, sw_state_layout_array[unit][EXAMPLE_MODULE_ID], DNX_SW_STATE_EXAMPLE_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#endif  
#undef BSL_LOG_MODULE
