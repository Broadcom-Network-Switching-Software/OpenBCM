
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
#include <soc/dnx/swstate/auto_generated/access/example_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_module_testing.h>
#include <soc/dnxc/swstate/dnx_sw_state_pretty_dump.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/example_diagnostic.h>



sw_state_example* example_dummy = NULL;



int
example_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]),
        "example[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        EXAMPLE_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        sw_state_example,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]),
        "example[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(sw_state_example),
        NULL);

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.default_value_l1.my_variable,
        5);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_deinit(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_DEINIT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_FREE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));

    DNX_SW_STATE_MODULE_DEINIT(
        unit,
        EXAMPLE_MODULE_ID,
        0,
        "example_deinit");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_DEINIT_LOGGING,
        BSL_LS_SWSTATEDNX_DEINIT,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]),
        "example[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_my_variable_set(int unit, uint32 my_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->my_variable,
        my_variable,
        uint32,
        0,
        "example_my_variable_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &my_variable,
        "example[%d]->my_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_MY_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_my_variable_get(int unit, uint32 *my_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        my_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *my_variable = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->my_variable;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->my_variable,
        "example[%d]->my_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_MY_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}








int
example_rollback_comparison_excluded_set(int unit, uint32 rollback_comparison_excluded)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->rollback_comparison_excluded,
        rollback_comparison_excluded,
        uint32,
        0,
        "example_rollback_comparison_excluded_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &rollback_comparison_excluded,
        "example[%d]->rollback_comparison_excluded",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_ROLLBACK_COMPARISON_EXCLUDED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_rollback_comparison_excluded_get(int unit, uint32 *rollback_comparison_excluded)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        rollback_comparison_excluded);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *rollback_comparison_excluded = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->rollback_comparison_excluded;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->rollback_comparison_excluded,
        "example[%d]->rollback_comparison_excluded",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_ROLLBACK_COMPARISON_EXCLUDED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_my_array_set(int unit, uint32 my_array_idx_0, uint32 my_array)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        my_array_idx_0,
        10);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->my_array[my_array_idx_0],
        my_array,
        uint32,
        0,
        "example_my_array_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &my_array,
        "example[%d]->my_array[%d]",
        unit, my_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_MY_ARRAY_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_my_array_get(int unit, uint32 my_array_idx_0, uint32 *my_array)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        my_array_idx_0,
        10);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        my_array);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *my_array = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->my_array[my_array_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->my_array[my_array_idx_0],
        "example[%d]->my_array[%d]",
        unit, my_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_MY_ARRAY_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_two_dimentional_array_set(int unit, uint32 index_1, uint32 index_2, uint32 two_dimentional_array)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        index_1,
        10);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        index_2,
        10);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->two_dimentional_array[index_1][index_2],
        two_dimentional_array,
        uint32,
        0,
        "example_two_dimentional_array_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &two_dimentional_array,
        "example[%d]->two_dimentional_array[%d][%d]",
        unit, index_1, index_2);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_TWO_DIMENTIONAL_ARRAY_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_two_dimentional_array_get(int unit, uint32 index_1, uint32 index_2, uint32 *two_dimentional_array)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        index_1,
        10);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        two_dimentional_array);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        index_2,
        10);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *two_dimentional_array = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->two_dimentional_array[index_1][index_2];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->two_dimentional_array[index_1][index_2],
        "example[%d]->two_dimentional_array[%d][%d]",
        unit, index_1, index_2);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_TWO_DIMENTIONAL_ARRAY_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_dynamic_array_set(int unit, uint32 dynamic_array_idx_0, uint32 dynamic_array)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array,
        dynamic_array_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array[dynamic_array_idx_0],
        dynamic_array,
        uint32,
        0,
        "example_dynamic_array_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &dynamic_array,
        "example[%d]->dynamic_array[%d]",
        unit, dynamic_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DYNAMIC_ARRAY_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_dynamic_array_get(int unit, uint32 dynamic_array_idx_0, uint32 *dynamic_array)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array,
        dynamic_array_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        dynamic_array);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *dynamic_array = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array[dynamic_array_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array[dynamic_array_idx_0],
        "example[%d]->dynamic_array[%d]",
        unit, dynamic_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DYNAMIC_ARRAY_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_dynamic_array_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array,
        uint32,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_dynamic_array_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array,
        "example[%d]->dynamic_array",
        unit,
        nof_instances_to_alloc_0 * sizeof(uint32) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DYNAMIC_ARRAY_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_dynamic_array_zero_size_set(int unit, uint32 dynamic_array_zero_size_idx_0, uint32 dynamic_array_zero_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_zero_size,
        dynamic_array_zero_size_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_zero_size);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_zero_size[dynamic_array_zero_size_idx_0],
        dynamic_array_zero_size,
        uint32,
        0,
        "example_dynamic_array_zero_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &dynamic_array_zero_size,
        "example[%d]->dynamic_array_zero_size[%d]",
        unit, dynamic_array_zero_size_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DYNAMIC_ARRAY_ZERO_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_dynamic_array_zero_size_get(int unit, uint32 dynamic_array_zero_size_idx_0, uint32 *dynamic_array_zero_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_zero_size,
        dynamic_array_zero_size_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        dynamic_array_zero_size);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_zero_size);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *dynamic_array_zero_size = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_zero_size[dynamic_array_zero_size_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_zero_size[dynamic_array_zero_size_idx_0],
        "example[%d]->dynamic_array_zero_size[%d]",
        unit, dynamic_array_zero_size_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DYNAMIC_ARRAY_ZERO_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_dynamic_array_zero_size_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_zero_size,
        uint32,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_dynamic_array_zero_size_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_zero_size,
        "example[%d]->dynamic_array_zero_size",
        unit,
        nof_instances_to_alloc_0 * sizeof(uint32) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_zero_size));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DYNAMIC_ARRAY_ZERO_SIZE_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_dynamic_array_zero_size_free(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_FREE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_zero_size);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DYNAMIC_ARRAY_ZERO_SIZE_INFO,
        DNX_SW_STATE_DIAG_FREE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_zero_size);

    DNX_SW_STATE_FREE(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_zero_size,
        "example_dynamic_array_zero_size_free");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_FREE_LOGGING,
        BSL_LS_SWSTATEDNX_FREE,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_zero_size,
        "example[%d]->dynamic_array_zero_size",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_dynamic_dynamic_array_set(int unit, uint32 first_d_index, uint32 dynamic_index, uint32 dynamic_dynamic_array)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array,
        first_d_index);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array[first_d_index],
        dynamic_index);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array[first_d_index]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array[first_d_index][dynamic_index],
        dynamic_dynamic_array,
        uint32,
        0,
        "example_dynamic_dynamic_array_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &dynamic_dynamic_array,
        "example[%d]->dynamic_dynamic_array[%d][%d]",
        unit, first_d_index, dynamic_index);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DYNAMIC_DYNAMIC_ARRAY_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_dynamic_dynamic_array_get(int unit, uint32 first_d_index, uint32 dynamic_index, uint32 *dynamic_dynamic_array)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array,
        first_d_index);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        dynamic_dynamic_array);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array[first_d_index],
        dynamic_index);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array[first_d_index]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *dynamic_dynamic_array = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array[first_d_index][dynamic_index];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array[first_d_index][dynamic_index],
        "example[%d]->dynamic_dynamic_array[%d][%d]",
        unit, first_d_index, dynamic_index);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DYNAMIC_DYNAMIC_ARRAY_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_dynamic_dynamic_array_alloc(int unit, uint32 nof_instances_to_alloc_0, uint32 nof_instances_to_alloc_1)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array,
        uint32*,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_dynamic_dynamic_array_alloc");

    for(uint32 first_d_index = 0;
         first_d_index < nof_instances_to_alloc_0;
         first_d_index++)

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array[first_d_index],
        uint32,
        nof_instances_to_alloc_1,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_dynamic_dynamic_array_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array,
        "example[%d]->dynamic_dynamic_array",
        unit,
        nof_instances_to_alloc_1 * sizeof(uint32) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array)+(nof_instances_to_alloc_1 * sizeof(uint32)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DYNAMIC_DYNAMIC_ARRAY_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_dynamic_array_static_set(int unit, uint32 dynamic_index, uint32 static_index, uint32 dynamic_array_static)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_static,
        dynamic_index);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_static);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        static_index,
        5);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_static[dynamic_index][static_index],
        dynamic_array_static,
        uint32,
        0,
        "example_dynamic_array_static_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &dynamic_array_static,
        "example[%d]->dynamic_array_static[%d][%d]",
        unit, dynamic_index, static_index);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DYNAMIC_ARRAY_STATIC_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_dynamic_array_static_get(int unit, uint32 dynamic_index, uint32 static_index, uint32 *dynamic_array_static)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_static,
        dynamic_index);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        dynamic_array_static);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        static_index,
        5);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_static);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *dynamic_array_static = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_static[dynamic_index][static_index];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_static[dynamic_index][static_index],
        "example[%d]->dynamic_array_static[%d][%d]",
        unit, dynamic_index, static_index);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DYNAMIC_ARRAY_STATIC_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_dynamic_array_static_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_static,
        uint32[5],
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_dynamic_array_static_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_static,
        "example[%d]->dynamic_array_static",
        unit,
        nof_instances_to_alloc_0 * sizeof(uint32[5]) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_static));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DYNAMIC_ARRAY_STATIC_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], nof_instances_to_alloc_0)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], 5)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_static[def_val_idx[0]][def_val_idx[1]],
        -1);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_counter_test_set(int unit, uint32 counter_test)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->counter_test,
        counter_test,
        uint32,
        0,
        "example_counter_test_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &counter_test,
        "example[%d]->counter_test",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_COUNTER_TEST_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_counter_test_get(int unit, uint32 *counter_test)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        counter_test);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *counter_test = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->counter_test;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->counter_test,
        "example[%d]->counter_test",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_COUNTER_TEST_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_counter_test_inc(int unit, uint32 inc_value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_INC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_COUNTER_INC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->counter_test,
        inc_value,
        uint32,
        0,
        "example_counter_test_inc");

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_COUNTER_TEST_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_counter_test_dec(int unit, uint32 dec_value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_DEC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_COUNTER_DEC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->counter_test,
        dec_value,
        uint32,
        0,
        "example_counter_test_dec");

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_COUNTER_TEST_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_mutex_test_create(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_MUTEX_CREATE(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->mutex_test,
        "((sw_state_example*)sw_state_roots_array).mutex_test");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MUTEX_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXCREATE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->mutex_test,
        "example[%d]->mutex_test",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_MUTEX_TEST_INFO,
        DNX_SW_STATE_DIAG_MUTEX,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_mutex_test_is_created(int unit, uint8 *is_created)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_IS_CREATED,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_MUTEX_IS_CREATED(
        unit,
        EXAMPLE_MODULE_ID,
        is_created,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->mutex_test,
        "((sw_state_example*)sw_state_roots_array).mutex_test");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MUTEX_IS_CREATED_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXCREATE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->mutex_test,
        "example[%d]->mutex_test",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_MUTEX_TEST_INFO,
        DNX_SW_STATE_DIAG_MUTEX,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_mutex_test_destroy(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_DESTROY,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_MUTEX_DESTROY(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->mutex_test);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MUTEX_DESTROY_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXDESTROY,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->mutex_test,
        "example[%d]->mutex_test",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_MUTEX_TEST_INFO,
        DNX_SW_STATE_DIAG_MUTEX,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_mutex_test_take(int unit, int usec)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_TAKE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_MUTEX_TAKE(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->mutex_test,
        usec);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MUTEX_TAKE_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXTAKE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->mutex_test,
        "example[%d]->mutex_test",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_MUTEX_TEST_INFO,
        DNX_SW_STATE_DIAG_MUTEX,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_mutex_test_give(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GIVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_MUTEX_GIVE(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->mutex_test);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MUTEX_GIVE_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXGIVE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->mutex_test,
        "example[%d]->mutex_test",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_MUTEX_TEST_INFO,
        DNX_SW_STATE_DIAG_MUTEX,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_sem_test_create(int unit, int is_binary, int initial_count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SEM_CREATE(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sem_test,
        is_binary,
        initial_count,
        "((sw_state_example*)sw_state_roots_array).sem_test");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SEM_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXCREATE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sem_test,
        "example[%d]->sem_test",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_SEM_TEST_INFO,
        DNX_SW_STATE_DIAG_SEM,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_sem_test_is_created(int unit, uint8 *is_created)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_IS_CREATED,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SEM_IS_CREATED(
        unit,
        EXAMPLE_MODULE_ID,
        is_created,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sem_test,
        "((sw_state_example*)sw_state_roots_array).sem_test");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SEM_IS_CREATED_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXCREATE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sem_test,
        "example[%d]->sem_test",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_SEM_TEST_INFO,
        DNX_SW_STATE_DIAG_SEM,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_sem_test_destroy(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_DESTROY,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SEM_DESTROY(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sem_test);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SEM_DESTROY_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXDESTROY,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sem_test,
        "example[%d]->sem_test",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_SEM_TEST_INFO,
        DNX_SW_STATE_DIAG_SEM,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_sem_test_take(int unit, int usec)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_TAKE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SEM_TAKE(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sem_test,
        usec);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SEM_TAKE_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXTAKE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sem_test,
        "example[%d]->sem_test",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_SEM_TEST_INFO,
        DNX_SW_STATE_DIAG_SEM,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_sem_test_give(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GIVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SEM_GIVE(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sem_test);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SEM_GIVE_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXGIVE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sem_test,
        "example[%d]->sem_test",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_SEM_TEST_INFO,
        DNX_SW_STATE_DIAG_SEM,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_DNXData_array_alloc_exception_set(int unit, uint32 DNXData_array_alloc_exception_idx_0, uint32 DNXData_array_alloc_exception)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_alloc_exception,
        DNXData_array_alloc_exception_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_alloc_exception);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_alloc_exception[DNXData_array_alloc_exception_idx_0],
        DNXData_array_alloc_exception,
        uint32,
        0,
        "example_DNXData_array_alloc_exception_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &DNXData_array_alloc_exception,
        "example[%d]->DNXData_array_alloc_exception[%d]",
        unit, DNXData_array_alloc_exception_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_ALLOC_EXCEPTION_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_DNXData_array_alloc_exception_get(int unit, uint32 DNXData_array_alloc_exception_idx_0, uint32 *DNXData_array_alloc_exception)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_alloc_exception,
        DNXData_array_alloc_exception_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        DNXData_array_alloc_exception);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_alloc_exception);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *DNXData_array_alloc_exception = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_alloc_exception[DNXData_array_alloc_exception_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_alloc_exception[DNXData_array_alloc_exception_idx_0],
        "example[%d]->DNXData_array_alloc_exception[%d]",
        unit, DNXData_array_alloc_exception_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_ALLOC_EXCEPTION_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_DNXData_array_alloc_exception_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_alloc_exception,
        uint32,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_DNXData_array_alloc_exception_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_alloc_exception,
        "example[%d]->DNXData_array_alloc_exception",
        unit,
        nof_instances_to_alloc_0 * sizeof(uint32) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_alloc_exception));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_ALLOC_EXCEPTION_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_DNXData_array_set(int unit, uint32 DNXData_array_idx_0, uint32 DNXData_array)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array,
        DNXData_array_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array[DNXData_array_idx_0],
        DNXData_array,
        uint32,
        0,
        "example_DNXData_array_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &DNXData_array,
        "example[%d]->DNXData_array[%d]",
        unit, DNXData_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_DNXData_array_get(int unit, uint32 DNXData_array_idx_0, uint32 *DNXData_array)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array,
        DNXData_array_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        DNXData_array);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *DNXData_array = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array[DNXData_array_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array[DNXData_array_idx_0],
        "example[%d]->DNXData_array[%d]",
        unit, DNXData_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_DNXData_array_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array,
        uint32,
        dnx_data_module_testing.dbal.vrf_field_size_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_DNXData_array_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array,
        "example[%d]->DNXData_array",
        unit,
        dnx_data_module_testing.dbal.vrf_field_size_get(unit) * sizeof(uint32) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_DNXData_array_dyn_dnxdata_set(int unit, uint32 DNXData_array_dyn_dnxdata_idx_0, uint32 DNXData_array_dyn_dnxdata_idx_1, uint32 DNXData_array_dyn_dnxdata)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata,
        DNXData_array_dyn_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata[DNXData_array_dyn_dnxdata_idx_0],
        DNXData_array_dyn_dnxdata_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata[DNXData_array_dyn_dnxdata_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata[DNXData_array_dyn_dnxdata_idx_0][DNXData_array_dyn_dnxdata_idx_1],
        DNXData_array_dyn_dnxdata,
        uint32,
        0,
        "example_DNXData_array_dyn_dnxdata_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &DNXData_array_dyn_dnxdata,
        "example[%d]->DNXData_array_dyn_dnxdata[%d][%d]",
        unit, DNXData_array_dyn_dnxdata_idx_0, DNXData_array_dyn_dnxdata_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_DYN_DNXDATA_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_DNXData_array_dyn_dnxdata_get(int unit, uint32 DNXData_array_dyn_dnxdata_idx_0, uint32 DNXData_array_dyn_dnxdata_idx_1, uint32 *DNXData_array_dyn_dnxdata)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata,
        DNXData_array_dyn_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        DNXData_array_dyn_dnxdata);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata[DNXData_array_dyn_dnxdata_idx_0],
        DNXData_array_dyn_dnxdata_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata[DNXData_array_dyn_dnxdata_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *DNXData_array_dyn_dnxdata = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata[DNXData_array_dyn_dnxdata_idx_0][DNXData_array_dyn_dnxdata_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata[DNXData_array_dyn_dnxdata_idx_0][DNXData_array_dyn_dnxdata_idx_1],
        "example[%d]->DNXData_array_dyn_dnxdata[%d][%d]",
        unit, DNXData_array_dyn_dnxdata_idx_0, DNXData_array_dyn_dnxdata_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_DYN_DNXDATA_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_DNXData_array_dyn_dnxdata_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata,
        uint32*,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_DNXData_array_dyn_dnxdata_alloc");

    for(uint32 DNXData_array_dyn_dnxdata_idx_0 = 0;
         DNXData_array_dyn_dnxdata_idx_0 < nof_instances_to_alloc_0;
         DNXData_array_dyn_dnxdata_idx_0++)

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata[DNXData_array_dyn_dnxdata_idx_0],
        uint32,
        dnx_data_module_testing.dbal.vrf_field_size_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_DNXData_array_dyn_dnxdata_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata,
        "example[%d]->DNXData_array_dyn_dnxdata",
        unit,
        dnx_data_module_testing.dbal.vrf_field_size_get(unit) * sizeof(uint32) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata)+(dnx_data_module_testing.dbal.vrf_field_size_get(unit) * sizeof(uint32)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_DYN_DNXDATA_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_DNXData_array_dnxdata_dyn_set(int unit, uint32 DNXData_array_dnxdata_dyn_idx_0, uint32 DNXData_array_dnxdata_dyn_idx_1, uint32 DNXData_array_dnxdata_dyn)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn,
        DNXData_array_dnxdata_dyn_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn[DNXData_array_dnxdata_dyn_idx_0],
        DNXData_array_dnxdata_dyn_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn[DNXData_array_dnxdata_dyn_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn[DNXData_array_dnxdata_dyn_idx_0][DNXData_array_dnxdata_dyn_idx_1],
        DNXData_array_dnxdata_dyn,
        uint32,
        0,
        "example_DNXData_array_dnxdata_dyn_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &DNXData_array_dnxdata_dyn,
        "example[%d]->DNXData_array_dnxdata_dyn[%d][%d]",
        unit, DNXData_array_dnxdata_dyn_idx_0, DNXData_array_dnxdata_dyn_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_DNXDATA_DYN_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_DNXData_array_dnxdata_dyn_get(int unit, uint32 DNXData_array_dnxdata_dyn_idx_0, uint32 DNXData_array_dnxdata_dyn_idx_1, uint32 *DNXData_array_dnxdata_dyn)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn,
        DNXData_array_dnxdata_dyn_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        DNXData_array_dnxdata_dyn);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn[DNXData_array_dnxdata_dyn_idx_0],
        DNXData_array_dnxdata_dyn_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn[DNXData_array_dnxdata_dyn_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *DNXData_array_dnxdata_dyn = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn[DNXData_array_dnxdata_dyn_idx_0][DNXData_array_dnxdata_dyn_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn[DNXData_array_dnxdata_dyn_idx_0][DNXData_array_dnxdata_dyn_idx_1],
        "example[%d]->DNXData_array_dnxdata_dyn[%d][%d]",
        unit, DNXData_array_dnxdata_dyn_idx_0, DNXData_array_dnxdata_dyn_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_DNXDATA_DYN_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_DNXData_array_dnxdata_dyn_alloc(int unit, uint32 nof_instances_to_alloc_1)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn,
        uint32*,
        dnx_data_module_testing.dbal.vrf_field_size_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_DNXData_array_dnxdata_dyn_alloc");

    for(uint32 DNXData_array_dnxdata_dyn_idx_0 = 0;
         DNXData_array_dnxdata_dyn_idx_0 < dnx_data_module_testing.dbal.vrf_field_size_get(unit);
         DNXData_array_dnxdata_dyn_idx_0++)

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn[DNXData_array_dnxdata_dyn_idx_0],
        uint32,
        nof_instances_to_alloc_1,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_DNXData_array_dnxdata_dyn_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn,
        "example[%d]->DNXData_array_dnxdata_dyn",
        unit,
        nof_instances_to_alloc_1 * sizeof(uint32) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn)+(nof_instances_to_alloc_1 * sizeof(uint32)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_DNXDATA_DYN_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_DNXData_array_dnxdata_dnxdata_set(int unit, uint32 DNXData_array_dnxdata_dnxdata_idx_0, uint32 DNXData_array_dnxdata_dnxdata_idx_1, uint32 DNXData_array_dnxdata_dnxdata)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata,
        DNXData_array_dnxdata_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata[DNXData_array_dnxdata_dnxdata_idx_0],
        DNXData_array_dnxdata_dnxdata_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata[DNXData_array_dnxdata_dnxdata_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata[DNXData_array_dnxdata_dnxdata_idx_0][DNXData_array_dnxdata_dnxdata_idx_1],
        DNXData_array_dnxdata_dnxdata,
        uint32,
        0,
        "example_DNXData_array_dnxdata_dnxdata_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &DNXData_array_dnxdata_dnxdata,
        "example[%d]->DNXData_array_dnxdata_dnxdata[%d][%d]",
        unit, DNXData_array_dnxdata_dnxdata_idx_0, DNXData_array_dnxdata_dnxdata_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_DNXDATA_DNXDATA_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_DNXData_array_dnxdata_dnxdata_get(int unit, uint32 DNXData_array_dnxdata_dnxdata_idx_0, uint32 DNXData_array_dnxdata_dnxdata_idx_1, uint32 *DNXData_array_dnxdata_dnxdata)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata,
        DNXData_array_dnxdata_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        DNXData_array_dnxdata_dnxdata);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata[DNXData_array_dnxdata_dnxdata_idx_0],
        DNXData_array_dnxdata_dnxdata_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata[DNXData_array_dnxdata_dnxdata_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *DNXData_array_dnxdata_dnxdata = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata[DNXData_array_dnxdata_dnxdata_idx_0][DNXData_array_dnxdata_dnxdata_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata[DNXData_array_dnxdata_dnxdata_idx_0][DNXData_array_dnxdata_dnxdata_idx_1],
        "example[%d]->DNXData_array_dnxdata_dnxdata[%d][%d]",
        unit, DNXData_array_dnxdata_dnxdata_idx_0, DNXData_array_dnxdata_dnxdata_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_DNXDATA_DNXDATA_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_DNXData_array_dnxdata_dnxdata_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata,
        uint32*,
        dnx_data_module_testing.dbal.vrf_field_size_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_DNXData_array_dnxdata_dnxdata_alloc");

    for(uint32 DNXData_array_dnxdata_dnxdata_idx_0 = 0;
         DNXData_array_dnxdata_dnxdata_idx_0 < dnx_data_module_testing.dbal.vrf_field_size_get(unit);
         DNXData_array_dnxdata_dnxdata_idx_0++)

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata[DNXData_array_dnxdata_dnxdata_idx_0],
        uint32,
        dnx_data_module_testing.dbal.vrf_field_size_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_DNXData_array_dnxdata_dnxdata_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata,
        "example[%d]->DNXData_array_dnxdata_dnxdata",
        unit,
        dnx_data_module_testing.dbal.vrf_field_size_get(unit) * sizeof(uint32) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata)+(dnx_data_module_testing.dbal.vrf_field_size_get(unit) * sizeof(uint32)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_DNXDATA_DNXDATA_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_DNXData_array_static_dnxdata_dyn_set(int unit, uint32 DNXData_array_static_dnxdata_dyn_idx_0, uint32 DNXData_array_static_dnxdata_dyn_idx_1, uint32 DNXData_array_static_dnxdata_dyn_idx_2, uint32 DNXData_array_static_dnxdata_dyn)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        DNXData_array_static_dnxdata_dyn_idx_0,
        3);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[DNXData_array_static_dnxdata_dyn_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[DNXData_array_static_dnxdata_dyn_idx_0],
        DNXData_array_static_dnxdata_dyn_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[DNXData_array_static_dnxdata_dyn_idx_0][DNXData_array_static_dnxdata_dyn_idx_1]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[DNXData_array_static_dnxdata_dyn_idx_0][DNXData_array_static_dnxdata_dyn_idx_1],
        DNXData_array_static_dnxdata_dyn_idx_2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[DNXData_array_static_dnxdata_dyn_idx_0][DNXData_array_static_dnxdata_dyn_idx_1][DNXData_array_static_dnxdata_dyn_idx_2],
        DNXData_array_static_dnxdata_dyn,
        uint32,
        0,
        "example_DNXData_array_static_dnxdata_dyn_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &DNXData_array_static_dnxdata_dyn,
        "example[%d]->DNXData_array_static_dnxdata_dyn[%d][%d][%d]",
        unit, DNXData_array_static_dnxdata_dyn_idx_0, DNXData_array_static_dnxdata_dyn_idx_1, DNXData_array_static_dnxdata_dyn_idx_2);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_STATIC_DNXDATA_DYN_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_DNXData_array_static_dnxdata_dyn_get(int unit, uint32 DNXData_array_static_dnxdata_dyn_idx_0, uint32 DNXData_array_static_dnxdata_dyn_idx_1, uint32 DNXData_array_static_dnxdata_dyn_idx_2, uint32 *DNXData_array_static_dnxdata_dyn)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        DNXData_array_static_dnxdata_dyn_idx_0,
        3);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        DNXData_array_static_dnxdata_dyn);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[DNXData_array_static_dnxdata_dyn_idx_0],
        DNXData_array_static_dnxdata_dyn_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[DNXData_array_static_dnxdata_dyn_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[DNXData_array_static_dnxdata_dyn_idx_0][DNXData_array_static_dnxdata_dyn_idx_1],
        DNXData_array_static_dnxdata_dyn_idx_2);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[DNXData_array_static_dnxdata_dyn_idx_0][DNXData_array_static_dnxdata_dyn_idx_1]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *DNXData_array_static_dnxdata_dyn = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[DNXData_array_static_dnxdata_dyn_idx_0][DNXData_array_static_dnxdata_dyn_idx_1][DNXData_array_static_dnxdata_dyn_idx_2];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[DNXData_array_static_dnxdata_dyn_idx_0][DNXData_array_static_dnxdata_dyn_idx_1][DNXData_array_static_dnxdata_dyn_idx_2],
        "example[%d]->DNXData_array_static_dnxdata_dyn[%d][%d][%d]",
        unit, DNXData_array_static_dnxdata_dyn_idx_0, DNXData_array_static_dnxdata_dyn_idx_1, DNXData_array_static_dnxdata_dyn_idx_2);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_STATIC_DNXDATA_DYN_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_DNXData_array_static_dnxdata_dyn_alloc(int unit, uint32 DNXData_array_static_dnxdata_dyn_idx_0, uint32 nof_instances_to_alloc_2)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[DNXData_array_static_dnxdata_dyn_idx_0],
        uint32*,
        dnx_data_module_testing.dbal.vrf_field_size_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_DNXData_array_static_dnxdata_dyn_alloc");

    for(uint32 DNXData_array_static_dnxdata_dyn_idx_1 = 0;
         DNXData_array_static_dnxdata_dyn_idx_1 < dnx_data_module_testing.dbal.vrf_field_size_get(unit);
         DNXData_array_static_dnxdata_dyn_idx_1++)

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[DNXData_array_static_dnxdata_dyn_idx_0][DNXData_array_static_dnxdata_dyn_idx_1],
        uint32,
        nof_instances_to_alloc_2,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_DNXData_array_static_dnxdata_dyn_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[DNXData_array_static_dnxdata_dyn_idx_0],
        "example[%d]->DNXData_array_static_dnxdata_dyn[%d]",
        unit, DNXData_array_static_dnxdata_dyn_idx_0,
        nof_instances_to_alloc_2 * sizeof(uint32) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[DNXData_array_static_dnxdata_dyn_idx_0])+(nof_instances_to_alloc_2 * sizeof(uint32)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_STATIC_DNXDATA_DYN_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_DNXData_array_static_dyn_dnxdata_set(int unit, uint32 DNXData_array_static_dyn_dnxdata_idx_0, uint32 DNXData_array_static_dyn_dnxdata_idx_1, uint32 DNXData_array_static_dyn_dnxdata_idx_2, uint32 DNXData_array_static_dyn_dnxdata)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        DNXData_array_static_dyn_dnxdata_idx_0,
        3);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[DNXData_array_static_dyn_dnxdata_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[DNXData_array_static_dyn_dnxdata_idx_0],
        DNXData_array_static_dyn_dnxdata_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[DNXData_array_static_dyn_dnxdata_idx_0][DNXData_array_static_dyn_dnxdata_idx_1]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[DNXData_array_static_dyn_dnxdata_idx_0][DNXData_array_static_dyn_dnxdata_idx_1],
        DNXData_array_static_dyn_dnxdata_idx_2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[DNXData_array_static_dyn_dnxdata_idx_0][DNXData_array_static_dyn_dnxdata_idx_1][DNXData_array_static_dyn_dnxdata_idx_2],
        DNXData_array_static_dyn_dnxdata,
        uint32,
        0,
        "example_DNXData_array_static_dyn_dnxdata_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &DNXData_array_static_dyn_dnxdata,
        "example[%d]->DNXData_array_static_dyn_dnxdata[%d][%d][%d]",
        unit, DNXData_array_static_dyn_dnxdata_idx_0, DNXData_array_static_dyn_dnxdata_idx_1, DNXData_array_static_dyn_dnxdata_idx_2);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_STATIC_DYN_DNXDATA_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_DNXData_array_static_dyn_dnxdata_get(int unit, uint32 DNXData_array_static_dyn_dnxdata_idx_0, uint32 DNXData_array_static_dyn_dnxdata_idx_1, uint32 DNXData_array_static_dyn_dnxdata_idx_2, uint32 *DNXData_array_static_dyn_dnxdata)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        DNXData_array_static_dyn_dnxdata_idx_0,
        3);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        DNXData_array_static_dyn_dnxdata);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[DNXData_array_static_dyn_dnxdata_idx_0],
        DNXData_array_static_dyn_dnxdata_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[DNXData_array_static_dyn_dnxdata_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[DNXData_array_static_dyn_dnxdata_idx_0][DNXData_array_static_dyn_dnxdata_idx_1],
        DNXData_array_static_dyn_dnxdata_idx_2);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[DNXData_array_static_dyn_dnxdata_idx_0][DNXData_array_static_dyn_dnxdata_idx_1]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *DNXData_array_static_dyn_dnxdata = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[DNXData_array_static_dyn_dnxdata_idx_0][DNXData_array_static_dyn_dnxdata_idx_1][DNXData_array_static_dyn_dnxdata_idx_2];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[DNXData_array_static_dyn_dnxdata_idx_0][DNXData_array_static_dyn_dnxdata_idx_1][DNXData_array_static_dyn_dnxdata_idx_2],
        "example[%d]->DNXData_array_static_dyn_dnxdata[%d][%d][%d]",
        unit, DNXData_array_static_dyn_dnxdata_idx_0, DNXData_array_static_dyn_dnxdata_idx_1, DNXData_array_static_dyn_dnxdata_idx_2);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_STATIC_DYN_DNXDATA_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_DNXData_array_static_dyn_dnxdata_alloc(int unit, uint32 DNXData_array_static_dyn_dnxdata_idx_0, uint32 nof_instances_to_alloc_1)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[DNXData_array_static_dyn_dnxdata_idx_0],
        uint32*,
        nof_instances_to_alloc_1,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_DNXData_array_static_dyn_dnxdata_alloc");

    for(uint32 DNXData_array_static_dyn_dnxdata_idx_1 = 0;
         DNXData_array_static_dyn_dnxdata_idx_1 < nof_instances_to_alloc_1;
         DNXData_array_static_dyn_dnxdata_idx_1++)

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[DNXData_array_static_dyn_dnxdata_idx_0][DNXData_array_static_dyn_dnxdata_idx_1],
        uint32,
        dnx_data_module_testing.dbal.vrf_field_size_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_DNXData_array_static_dyn_dnxdata_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[DNXData_array_static_dyn_dnxdata_idx_0],
        "example[%d]->DNXData_array_static_dyn_dnxdata[%d]",
        unit, DNXData_array_static_dyn_dnxdata_idx_0,
        dnx_data_module_testing.dbal.vrf_field_size_get(unit) * sizeof(uint32) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[DNXData_array_static_dyn_dnxdata_idx_0])+(dnx_data_module_testing.dbal.vrf_field_size_get(unit) * sizeof(uint32)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_STATIC_DYN_DNXDATA_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_DNXData_array_static_dnxdata_dnxdata_set(int unit, uint32 DNXData_array_static_dnxdata_dnxdata_idx_0, uint32 DNXData_array_static_dnxdata_dnxdata_idx_1, uint32 DNXData_array_static_dnxdata_dnxdata_idx_2, uint32 DNXData_array_static_dnxdata_dnxdata)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        DNXData_array_static_dnxdata_dnxdata_idx_0,
        3);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[DNXData_array_static_dnxdata_dnxdata_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[DNXData_array_static_dnxdata_dnxdata_idx_0],
        DNXData_array_static_dnxdata_dnxdata_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[DNXData_array_static_dnxdata_dnxdata_idx_0][DNXData_array_static_dnxdata_dnxdata_idx_1]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[DNXData_array_static_dnxdata_dnxdata_idx_0][DNXData_array_static_dnxdata_dnxdata_idx_1],
        DNXData_array_static_dnxdata_dnxdata_idx_2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[DNXData_array_static_dnxdata_dnxdata_idx_0][DNXData_array_static_dnxdata_dnxdata_idx_1][DNXData_array_static_dnxdata_dnxdata_idx_2],
        DNXData_array_static_dnxdata_dnxdata,
        uint32,
        0,
        "example_DNXData_array_static_dnxdata_dnxdata_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &DNXData_array_static_dnxdata_dnxdata,
        "example[%d]->DNXData_array_static_dnxdata_dnxdata[%d][%d][%d]",
        unit, DNXData_array_static_dnxdata_dnxdata_idx_0, DNXData_array_static_dnxdata_dnxdata_idx_1, DNXData_array_static_dnxdata_dnxdata_idx_2);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_DNXData_array_static_dnxdata_dnxdata_get(int unit, uint32 DNXData_array_static_dnxdata_dnxdata_idx_0, uint32 DNXData_array_static_dnxdata_dnxdata_idx_1, uint32 DNXData_array_static_dnxdata_dnxdata_idx_2, uint32 *DNXData_array_static_dnxdata_dnxdata)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        DNXData_array_static_dnxdata_dnxdata_idx_0,
        3);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        DNXData_array_static_dnxdata_dnxdata);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[DNXData_array_static_dnxdata_dnxdata_idx_0],
        DNXData_array_static_dnxdata_dnxdata_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[DNXData_array_static_dnxdata_dnxdata_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[DNXData_array_static_dnxdata_dnxdata_idx_0][DNXData_array_static_dnxdata_dnxdata_idx_1],
        DNXData_array_static_dnxdata_dnxdata_idx_2);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[DNXData_array_static_dnxdata_dnxdata_idx_0][DNXData_array_static_dnxdata_dnxdata_idx_1]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *DNXData_array_static_dnxdata_dnxdata = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[DNXData_array_static_dnxdata_dnxdata_idx_0][DNXData_array_static_dnxdata_dnxdata_idx_1][DNXData_array_static_dnxdata_dnxdata_idx_2];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[DNXData_array_static_dnxdata_dnxdata_idx_0][DNXData_array_static_dnxdata_dnxdata_idx_1][DNXData_array_static_dnxdata_dnxdata_idx_2],
        "example[%d]->DNXData_array_static_dnxdata_dnxdata[%d][%d][%d]",
        unit, DNXData_array_static_dnxdata_dnxdata_idx_0, DNXData_array_static_dnxdata_dnxdata_idx_1, DNXData_array_static_dnxdata_dnxdata_idx_2);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_DNXData_array_static_dnxdata_dnxdata_alloc(int unit, uint32 DNXData_array_static_dnxdata_dnxdata_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[DNXData_array_static_dnxdata_dnxdata_idx_0],
        uint32*,
        dnx_data_module_testing.dbal.vrf_field_size_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_DNXData_array_static_dnxdata_dnxdata_alloc");

    for(uint32 DNXData_array_static_dnxdata_dnxdata_idx_1 = 0;
         DNXData_array_static_dnxdata_dnxdata_idx_1 < dnx_data_module_testing.dbal.vrf_field_size_get(unit);
         DNXData_array_static_dnxdata_dnxdata_idx_1++)

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[DNXData_array_static_dnxdata_dnxdata_idx_0][DNXData_array_static_dnxdata_dnxdata_idx_1],
        uint32,
        dnx_data_module_testing.dbal.vrf_field_size_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_DNXData_array_static_dnxdata_dnxdata_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[DNXData_array_static_dnxdata_dnxdata_idx_0],
        "example[%d]->DNXData_array_static_dnxdata_dnxdata[%d]",
        unit, DNXData_array_static_dnxdata_dnxdata_idx_0,
        dnx_data_module_testing.dbal.vrf_field_size_get(unit) * sizeof(uint32) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[DNXData_array_static_dnxdata_dnxdata_idx_0])+(dnx_data_module_testing.dbal.vrf_field_size_get(unit) * sizeof(uint32)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_DNXData_array_table_data_set(int unit, uint32 DNXData_array_table_data_idx_0, uint32 DNXData_array_table_data)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_table_data,
        DNXData_array_table_data_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_table_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_table_data[DNXData_array_table_data_idx_0],
        DNXData_array_table_data,
        uint32,
        0,
        "example_DNXData_array_table_data_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &DNXData_array_table_data,
        "example[%d]->DNXData_array_table_data[%d]",
        unit, DNXData_array_table_data_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_TABLE_DATA_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_DNXData_array_table_data_get(int unit, uint32 DNXData_array_table_data_idx_0, uint32 *DNXData_array_table_data)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_table_data,
        DNXData_array_table_data_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        DNXData_array_table_data);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_table_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *DNXData_array_table_data = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_table_data[DNXData_array_table_data_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_table_data[DNXData_array_table_data_idx_0],
        "example[%d]->DNXData_array_table_data[%d]",
        unit, DNXData_array_table_data_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_TABLE_DATA_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_DNXData_array_table_data_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_table_data,
        uint32,
        dnx_data_module_testing.dbal.all_get(unit)->vlan_id_in_ingress,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_DNXData_array_table_data_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_table_data,
        "example[%d]->DNXData_array_table_data",
        unit,
        dnx_data_module_testing.dbal.all_get(unit)->vlan_id_in_ingress * sizeof(uint32) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_table_data));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_TABLE_DATA_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_value_range_test_set(int unit, int value_range_test)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_VALID_VALUE_RANGE(
        unit,
        value_range_test,
        1,
        10);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->value_range_test,
        value_range_test,
        int,
        0,
        "example_value_range_test_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &value_range_test,
        "example[%d]->value_range_test",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_VALUE_RANGE_TEST_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_value_range_test_get(int unit, int *value_range_test)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        value_range_test);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *value_range_test = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->value_range_test;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->value_range_test,
        "example[%d]->value_range_test",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_VALUE_RANGE_TEST_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_array_range_example_set(int unit, uint32 array_range_example_idx_0, uint32 array_range_example)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        array_range_example_idx_0,
        10);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->array_range_example[array_range_example_idx_0],
        array_range_example,
        uint32,
        0,
        "example_array_range_example_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &array_range_example,
        "example[%d]->array_range_example[%d]",
        unit, array_range_example_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_ARRAY_RANGE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_array_range_example_get(int unit, uint32 array_range_example_idx_0, uint32 *array_range_example)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        array_range_example_idx_0,
        10);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        array_range_example);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *array_range_example = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->array_range_example[array_range_example_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->array_range_example[array_range_example_idx_0],
        "example[%d]->array_range_example[%d]",
        unit, array_range_example_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_ARRAY_RANGE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_array_range_example_range_read(int unit, uint32 nof_elements, uint32 *dest)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_RANGE_READ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_RANGE_READ(
        unit,
        EXAMPLE_MODULE_ID,
        dest,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->array_range_example,
        uint32,
        nof_elements,
        0,
        "example_array_range_example_range_read");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_RANGE_READ_LOGGING,
        BSL_LS_SWSTATEDNX_RANGEREAD,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->array_range_example,
        "example[%d]->array_range_example",
        unit, nof_elements);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_ARRAY_RANGE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_array_range_example_range_write(int unit, uint32 nof_elements, const uint32 *source)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_RANGE_WRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_RANGE_COPY(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->array_range_example,
        source,
        uint32,
        nof_elements,
        0,
        "example_array_range_example_range_write");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_RANGE_WRITE_LOGGING,
        BSL_LS_SWSTATEDNX_RANGEWRITE,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->array_range_example,
        "example[%d]->array_range_example",
        unit, nof_elements);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_ARRAY_RANGE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_array_range_example_range_fill(int unit, uint32 from_idx, uint32 nof_elements, uint32 value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_RANGE_FILL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_RANGE_FILL(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->array_range_example,
        uint32,
        from_idx,
        nof_elements,
        value,
        0,
        "example_array_range_example_range_fill");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_RANGE_FILL_LOGGING,
        BSL_LS_SWSTATEDNX_RANGEFILL,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->array_range_example,
        "example[%d]->array_range_example",
        unit, nof_elements);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_ARRAY_RANGE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_alloc_after_init_variable_set(int unit, uint32 alloc_after_init_variable_idx_0, uint32 alloc_after_init_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_variable,
        alloc_after_init_variable_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_variable[alloc_after_init_variable_idx_0],
        alloc_after_init_variable,
        uint32,
        0,
        "example_alloc_after_init_variable_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &alloc_after_init_variable,
        "example[%d]->alloc_after_init_variable[%d]",
        unit, alloc_after_init_variable_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_ALLOC_AFTER_INIT_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_alloc_after_init_variable_get(int unit, uint32 alloc_after_init_variable_idx_0, uint32 *alloc_after_init_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_variable,
        alloc_after_init_variable_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        alloc_after_init_variable);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *alloc_after_init_variable = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_variable[alloc_after_init_variable_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_variable[alloc_after_init_variable_idx_0],
        "example[%d]->alloc_after_init_variable[%d]",
        unit, alloc_after_init_variable_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_ALLOC_AFTER_INIT_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_alloc_after_init_variable_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_variable,
        uint32,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_ALLOC_AFTER_INIT_EXCEPTION | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_alloc_after_init_variable_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_variable,
        "example[%d]->alloc_after_init_variable",
        unit,
        nof_instances_to_alloc_0 * sizeof(uint32) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_variable));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_ALLOC_AFTER_INIT_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_alloc_after_init_test_set(int unit, uint32 alloc_after_init_test_idx_0, uint32 alloc_after_init_test)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_test,
        alloc_after_init_test_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_test);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_test[alloc_after_init_test_idx_0],
        alloc_after_init_test,
        uint32,
        0,
        "example_alloc_after_init_test_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &alloc_after_init_test,
        "example[%d]->alloc_after_init_test[%d]",
        unit, alloc_after_init_test_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_ALLOC_AFTER_INIT_TEST_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_alloc_after_init_test_get(int unit, uint32 alloc_after_init_test_idx_0, uint32 *alloc_after_init_test)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_test,
        alloc_after_init_test_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        alloc_after_init_test);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_test);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *alloc_after_init_test = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_test[alloc_after_init_test_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_test[alloc_after_init_test_idx_0],
        "example[%d]->alloc_after_init_test[%d]",
        unit, alloc_after_init_test_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_ALLOC_AFTER_INIT_TEST_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_alloc_after_init_test_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_test,
        uint32,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_alloc_after_init_test_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_test,
        "example[%d]->alloc_after_init_test",
        unit,
        nof_instances_to_alloc_0 * sizeof(uint32) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_test));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_ALLOC_AFTER_INIT_TEST_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_pointer_set(int unit, uint32 pointer)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pointer);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pointer[0],
        pointer,
        uint32,
        0,
        "example_pointer_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &pointer,
        "example[%d]->pointer[%d]",
        unit, 0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_POINTER_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_pointer_get(int unit, uint32 *pointer)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        pointer);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pointer);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *pointer = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pointer[0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pointer[0],
        "example[%d]->pointer[%d]",
        unit, 0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_POINTER_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_pointer_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pointer,
        uint32,
        1,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_pointer_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pointer,
        "example[%d]->pointer",
        unit,
        sizeof(uint32) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pointer));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_POINTER_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_pointer_is_allocated(int unit, uint8 *is_allocated)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_IS_ALLOCATED,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        is_allocated);

    DNX_SW_STATE_IS_ALLOC(
        unit,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pointer,
        is_allocated,
        DNXC_SW_STATE_NONE,
        "example_pointer_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ISALLOC,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pointer,
        "example[%d]->pointer",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_variable_alloc_bitmap(int unit, uint32 _nof_bits_to_alloc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ALLOC_BITMAP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_ALLOC_BITMAP(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        _nof_bits_to_alloc,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_bitmap_variable_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_BITMAP_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOCBITMAP,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit,
        _nof_bits_to_alloc);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP,
        NULL);

    DNX_SW_STATE_BITMAP_DEFAULT_VALUE_SET(((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable, _nof_bits_to_alloc);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_variable_is_allocated(int unit, uint8 *is_allocated)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_IS_ALLOCATED,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        is_allocated);

    DNX_SW_STATE_IS_ALLOC(
        unit,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        is_allocated,
        DNXC_SW_STATE_NONE,
        "example_bitmap_variable_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ISALLOC,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_variable_free(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_FREE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_FREE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_FREE(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example_bitmap_variable_free");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_FREE_LOGGING,
        BSL_LS_SWSTATEDNX_FREE,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_variable_bit_set(int unit, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_SET(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITSET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_variable_bit_clear(int unit, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_CLEAR(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITCLEAR,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_variable_bit_get(int unit, uint32 _bit_num, uint8* result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_GET(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
         _bit_num,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_GET_LOGGING,
        BSL_LS_SWSTATEDNX_BITGET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_variable_bit_range_read(int unit, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_READ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_READ(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
         sw_state_bmp_first,
         result_first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_READ_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREAD,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_variable_bit_range_write(int unit, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_WRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_WRITE(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
         sw_state_bmp_first,
         input_bmp_first,
          _range,
         input_bmp);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_WRITE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEWRITE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_variable_bit_range_and(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_AND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_AND(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_AND_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEAND,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_variable_bit_range_or(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_OR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_OR(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_OR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEOR,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_variable_bit_range_xor(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_XOR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_XOR(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_XOR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEXOR,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_variable_bit_range_remove(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_REMOVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_REMOVE(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_REMOVE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREMOVE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_variable_bit_range_negate(int unit, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_NEGATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_NEGATE(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NEGATE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENEGATE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_variable_bit_range_clear(int unit, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_CLEAR(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECLEAR,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_variable_bit_range_set(int unit, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_SET(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGESET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_variable_bit_range_null(int unit, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_NULL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_NULL(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NULL_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENULL,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_variable_bit_range_test(int unit, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_TEST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_TEST(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_TEST_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGETEST,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_variable_bit_range_eq(int unit, SHR_BITDCL *input_bmp, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_EQ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_EQ(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
         _first,
         _count,
         input_bmp,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_EQ_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEEQ,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_variable_bit_range_count(int unit, uint32 _first, uint32 _range, int *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_COUNT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        (_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_COUNT(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
         _first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_COUNT_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECOUNT,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_pbmp_variable_set(int unit, bcm_pbmp_t pbmp_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        pbmp_variable,
        bcm_pbmp_t,
        0,
        "example_pbmp_variable_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_pbmp_variable_get(int unit, bcm_pbmp_t *pbmp_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        pbmp_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *pbmp_variable = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_pbmp_variable_pbmp_neq(int unit, _shr_pbmp_t input_pbmp, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_PBMP_NEQ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_PBMP_NEQ(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        input_pbmp,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_NEQ_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPNEQ,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_pbmp_variable_pbmp_eq(int unit, _shr_pbmp_t input_pbmp, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_PBMP_EQ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_PBMP_EQ(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        input_pbmp,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_EQ_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPEQ,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_pbmp_variable_pbmp_member(int unit, uint32 _input_port, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_PBMP_MEMBER,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_PBMP_MEMBER(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        _input_port,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_MEMBER_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPMEMBER,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_pbmp_variable_pbmp_not_null(int unit, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_PBMP_NOT_NULL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_PBMP_NOT_NULL(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_NOT_NULL_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPNOTNULL,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_pbmp_variable_pbmp_is_null(int unit, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_PBMP_IS_NULL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_PBMP_IS_NULL(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_IS_NULL_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPISNULL,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_pbmp_variable_pbmp_count(int unit, int *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_PBMP_COUNT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_PBMP_COUNT(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_COUNT_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPCOUNT,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_pbmp_variable_pbmp_xor(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_PBMP_XOR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_PBMP_XOR(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_XOR_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPXOR,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_pbmp_variable_pbmp_remove(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_PBMP_REMOVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_PBMP_REMOVE(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_REMOVE_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPREMOVE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_pbmp_variable_pbmp_assign(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_PBMP_ASSIGN,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_PBMP_ASSIGN(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_ASSIGN_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPASSIGN,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_pbmp_variable_pbmp_get(int unit, _shr_pbmp_t *output_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_PBMP_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_PBMP_GET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        output_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_GET_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPGET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_pbmp_variable_pbmp_and(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_PBMP_AND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_PBMP_AND(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_AND_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPAND,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_pbmp_variable_pbmp_negate(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_PBMP_NEGATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_PBMP_NEGATE(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_NEGATE_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPNEGATE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_pbmp_variable_pbmp_or(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_PBMP_OR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_PBMP_OR(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_OR_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPOR,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_pbmp_variable_pbmp_clear(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_PBMP_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_PBMP_CLEAR(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPCLEAR,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_pbmp_variable_pbmp_port_add(int unit, uint32 _input_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_PBMP_PORT_ADD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_PBMP_PORT_ADD(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        _input_port,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_PORT_ADD_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPPORTADD,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_pbmp_variable_pbmp_port_flip(int unit, uint32 _input_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_PBMP_PORT_FLIP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_PBMP_PORT_FLIP(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        _input_port,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_PORT_FLIP_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPPORTFLIP,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_pbmp_variable_pbmp_port_remove(int unit, uint32 _input_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_PBMP_PORT_REMOVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_PBMP_PORT_REMOVE(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        _input_port,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_PORT_REMOVE_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPPORTREMOVE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_pbmp_variable_pbmp_port_set(int unit, uint32 _input_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_PBMP_PORT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_PBMP_PORT_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        _input_port,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_PORT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPPORTSET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_pbmp_variable_pbmp_ports_range_add(int unit, uint32 _first_port, uint32 _range)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_PBMP_PORTS_RANGE_ADD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_PBMP_PORTS_RANGE_ADD(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        _first_port,
        _range,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_PORTS_RANGE_ADD_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPPORTSRANGEADD,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_pbmp_variable_pbmp_fmt(int unit, char* _buffer)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_PBMP_FMT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_PBMP_FMT(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        _buffer,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_FMT_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPFMT,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_fixed_bit_set(int unit, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        _bit_num,
        ((1024)/SHR_BITWID)+1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_SET(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITSET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
        "example[%d]->bitmap_fixed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_FIXED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_fixed_bit_clear(int unit, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        _bit_num,
        ((1024)/SHR_BITWID)+1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_CLEAR(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITCLEAR,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
        "example[%d]->bitmap_fixed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_FIXED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_fixed_bit_get(int unit, uint32 _bit_num, uint8* result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        _bit_num,
        ((1024)/SHR_BITWID)+1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_GET(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
         _bit_num,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_GET_LOGGING,
        BSL_LS_SWSTATEDNX_BITGET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
        "example[%d]->bitmap_fixed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_FIXED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_fixed_bit_range_read(int unit, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_READ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (sw_state_bmp_first + _range - 1),
        ((1024)/SHR_BITWID)+1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_READ(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
         sw_state_bmp_first,
         result_first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_READ_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREAD,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
        "example[%d]->bitmap_fixed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_FIXED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_fixed_bit_range_write(int unit, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_WRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (sw_state_bmp_first + _range - 1),
        ((1024)/SHR_BITWID)+1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_WRITE(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
         sw_state_bmp_first,
         input_bmp_first,
          _range,
         input_bmp);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_WRITE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEWRITE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
        "example[%d]->bitmap_fixed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_FIXED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_fixed_bit_range_and(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_AND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (_first + _count - 1),
        ((1024)/SHR_BITWID)+1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_AND(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_AND_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEAND,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
        "example[%d]->bitmap_fixed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_FIXED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_fixed_bit_range_or(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_OR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (_first + _count - 1),
        ((1024)/SHR_BITWID)+1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_OR(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_OR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEOR,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
        "example[%d]->bitmap_fixed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_FIXED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_fixed_bit_range_xor(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_XOR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (_first + _count - 1),
        ((1024)/SHR_BITWID)+1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_XOR(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_XOR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEXOR,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
        "example[%d]->bitmap_fixed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_FIXED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_fixed_bit_range_remove(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_REMOVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (_first + _count - 1),
        ((1024)/SHR_BITWID)+1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_REMOVE(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_REMOVE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREMOVE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
        "example[%d]->bitmap_fixed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_FIXED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_fixed_bit_range_negate(int unit, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_NEGATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (_first + _count - 1),
        ((1024)/SHR_BITWID)+1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_NEGATE(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NEGATE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENEGATE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
        "example[%d]->bitmap_fixed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_FIXED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_fixed_bit_range_clear(int unit, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (_first + _count - 1),
        ((1024)/SHR_BITWID)+1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_CLEAR(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECLEAR,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
        "example[%d]->bitmap_fixed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_FIXED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_fixed_bit_range_set(int unit, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (_first + _count - 1),
        ((1024)/SHR_BITWID)+1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_SET(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGESET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
        "example[%d]->bitmap_fixed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_FIXED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_fixed_bit_range_null(int unit, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_NULL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (_first + _count - 1),
        ((1024)/SHR_BITWID)+1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_NULL(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NULL_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENULL,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
        "example[%d]->bitmap_fixed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_FIXED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_fixed_bit_range_test(int unit, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_TEST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (_first + _count - 1),
        ((1024)/SHR_BITWID)+1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_TEST(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_TEST_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGETEST,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
        "example[%d]->bitmap_fixed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_FIXED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_fixed_bit_range_eq(int unit, SHR_BITDCL *input_bmp, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_EQ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (_first + _count - 1),
        ((1024)/SHR_BITWID)+1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_EQ(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
         _first,
         _count,
         input_bmp,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_EQ_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEEQ,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
        "example[%d]->bitmap_fixed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_FIXED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_fixed_bit_range_count(int unit, uint32 _first, uint32 _range, int *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_COUNT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (_first + _range - 1),
        ((1024)/SHR_BITWID)+1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_COUNT(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
         _first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_COUNT_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECOUNT,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
        "example[%d]->bitmap_fixed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_FIXED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_dnx_data_alloc_bitmap(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ALLOC_BITMAP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_ALLOC_BITMAP(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        dnx_data_module_testing.dbal.vrf_field_size_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_bitmap_dnx_data_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_BITMAP_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOCBITMAP,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit,
        dnx_data_module_testing.dbal.vrf_field_size_get(unit));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_DNX_DATA_INFO,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_dnx_data_is_allocated(int unit, uint8 *is_allocated)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_IS_ALLOCATED,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        is_allocated);

    DNX_SW_STATE_IS_ALLOC(
        unit,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        is_allocated,
        DNXC_SW_STATE_NONE,
        "example_bitmap_dnx_data_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ISALLOC,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_dnx_data_bit_set(int unit, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_SET(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITSET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_DNX_DATA_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_dnx_data_bit_clear(int unit, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_CLEAR(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITCLEAR,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_DNX_DATA_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_dnx_data_bit_get(int unit, uint32 _bit_num, uint8* result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_GET(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
         _bit_num,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_GET_LOGGING,
        BSL_LS_SWSTATEDNX_BITGET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_DNX_DATA_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_dnx_data_bit_range_read(int unit, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_READ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_READ(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
         sw_state_bmp_first,
         result_first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_READ_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREAD,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_DNX_DATA_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_dnx_data_bit_range_write(int unit, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_WRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_WRITE(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
         sw_state_bmp_first,
         input_bmp_first,
          _range,
         input_bmp);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_WRITE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEWRITE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_DNX_DATA_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_dnx_data_bit_range_and(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_AND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_AND(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_AND_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEAND,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_DNX_DATA_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_dnx_data_bit_range_or(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_OR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_OR(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_OR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEOR,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_DNX_DATA_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_dnx_data_bit_range_xor(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_XOR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_XOR(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_XOR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEXOR,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_DNX_DATA_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_dnx_data_bit_range_remove(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_REMOVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_REMOVE(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_REMOVE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREMOVE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_DNX_DATA_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_dnx_data_bit_range_negate(int unit, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_NEGATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_NEGATE(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NEGATE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENEGATE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_DNX_DATA_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_dnx_data_bit_range_clear(int unit, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_CLEAR(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECLEAR,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_DNX_DATA_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_dnx_data_bit_range_set(int unit, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_SET(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGESET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_DNX_DATA_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_dnx_data_bit_range_null(int unit, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_NULL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_NULL(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NULL_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENULL,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_DNX_DATA_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_dnx_data_bit_range_test(int unit, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_TEST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_TEST(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_TEST_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGETEST,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_DNX_DATA_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_dnx_data_bit_range_eq(int unit, SHR_BITDCL *input_bmp, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_EQ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_EQ(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
         _first,
         _count,
         input_bmp,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_EQ_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEEQ,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_DNX_DATA_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_dnx_data_bit_range_count(int unit, uint32 _first, uint32 _range, int *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_COUNT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        (_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_COUNT(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
         _first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_COUNT_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECOUNT,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_DNX_DATA_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_buffer_set(int unit, uint32 buffer_idx_0, DNX_SW_STATE_BUFF buffer)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer,
        buffer_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer[buffer_idx_0],
        buffer,
        DNX_SW_STATE_BUFF,
        0,
        "example_buffer_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &buffer,
        "example[%d]->buffer[%d]",
        unit, buffer_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BUFFER_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_buffer_get(int unit, uint32 buffer_idx_0, DNX_SW_STATE_BUFF *buffer)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer,
        buffer_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        buffer);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *buffer = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer[buffer_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer[buffer_idx_0],
        "example[%d]->buffer[%d]",
        unit, buffer_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BUFFER_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_buffer_alloc(int unit, uint32 _nof_bytes_to_alloc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer,
        DNX_SW_STATE_BUFF,
        _nof_bytes_to_alloc,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_buffer_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer,
        "example[%d]->buffer",
        unit,
        _nof_bytes_to_alloc * sizeof(DNX_SW_STATE_BUFF) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BUFFER_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNX_SW_STATE_BUFFER_DEFAULT_VALUE_SET(((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer, 0xAA, _nof_bytes_to_alloc);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_buffer_is_allocated(int unit, uint8 *is_allocated)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_IS_ALLOCATED,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        is_allocated);

    DNX_SW_STATE_IS_ALLOC(
        unit,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer,
        is_allocated,
        DNXC_SW_STATE_NONE,
        "example_buffer_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ISALLOC,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer,
        "example[%d]->buffer",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_buffer_memread(int unit, uint8 *_dst, uint32 _offset, size_t _len)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_MEMREAD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer,
        (_offset + _len - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_MEMREAD(
        unit,
        _dst,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer,
        _offset,
        _len,
        0,
        "example_buffer_BUFFER");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MEMREAD_LOGGING,
        BSL_LS_SWSTATEDNX_MEMREAD,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer,
        "example[%d]->buffer",
        unit, _len);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BUFFER_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_buffer_memwrite(int unit, const uint8 *_src, uint32 _offset, size_t _len)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_MEMWRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer,
        (_offset + _len - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer);

    DNX_SW_STATE_MEMWRITE(
        unit,
        EXAMPLE_MODULE_ID,
        _src,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer,
        _offset,
        _len,
        0,
        "example_buffer_BUFFER");

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_buffer_memcmp(int unit, const uint8 *_buff, uint32 _offset, size_t _len, int *cmp_result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_MEMCMP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer,
        (_offset + _len - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer);

    DNX_SW_STATE_MEMCMP(
        unit,
        _buff,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer,
        _offset,
        _len,
        cmp_result,
        0,
        "example_buffer_BUFFER");

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_buffer_memset(int unit, uint32 _offset, size_t _len, int _value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_MEMSET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer,
        (_offset + _len - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer);

    DNX_SW_STATE_MEMSET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer,
        _offset,
        _value,
        _len,
        0,
        "example_buffer_BUFFER");

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_buffer_fixed_set(int unit, uint32 buffer_fixed_idx_0, DNX_SW_STATE_BUFF buffer_fixed)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        buffer_fixed_idx_0,
        1024);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_fixed[buffer_fixed_idx_0],
        buffer_fixed,
        DNX_SW_STATE_BUFF,
        0,
        "example_buffer_fixed_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &buffer_fixed,
        "example[%d]->buffer_fixed[%d]",
        unit, buffer_fixed_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BUFFER_FIXED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_buffer_fixed_get(int unit, uint32 buffer_fixed_idx_0, DNX_SW_STATE_BUFF *buffer_fixed)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        buffer_fixed_idx_0,
        1024);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        buffer_fixed);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *buffer_fixed = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_fixed[buffer_fixed_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_fixed[buffer_fixed_idx_0],
        "example[%d]->buffer_fixed[%d]",
        unit, buffer_fixed_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BUFFER_FIXED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_buffer_fixed_memread(int unit, uint8 *_dst, uint32 _offset, size_t _len)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_MEMREAD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BUFFER_CHECK(
        (_offset + _len - 1),
        1024);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_MEMREAD(
        unit,
        _dst,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_fixed,
        _offset,
        _len,
        0,
        "example_buffer_fixed_BUFFER");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MEMREAD_LOGGING,
        BSL_LS_SWSTATEDNX_MEMREAD,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_fixed,
        "example[%d]->buffer_fixed",
        unit, _len);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BUFFER_FIXED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_buffer_fixed_memwrite(int unit, const uint8 *_src, uint32 _offset, size_t _len)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_MEMWRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BUFFER_CHECK(
        (_offset + _len - 1),
        1024);

    DNX_SW_STATE_MEMWRITE(
        unit,
        EXAMPLE_MODULE_ID,
        _src,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_fixed,
        _offset,
        _len,
        0,
        "example_buffer_fixed_BUFFER");

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_buffer_fixed_memcmp(int unit, const uint8 *_buff, uint32 _offset, size_t _len, int *cmp_result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_MEMCMP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BUFFER_CHECK(
        (_offset + _len - 1),
        1024);

    DNX_SW_STATE_MEMCMP(
        unit,
        _buff,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_fixed,
        _offset,
        _len,
        cmp_result,
        0,
        "example_buffer_fixed_BUFFER");

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_buffer_fixed_memset(int unit, uint32 _offset, size_t _len, int _value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_MEMSET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BUFFER_CHECK(
        (_offset + _len - 1),
        1024);

    DNX_SW_STATE_MEMSET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_fixed,
        _offset,
        _value,
        _len,
        0,
        "example_buffer_fixed_BUFFER");

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_exclude_example_alloc_bitmap(int unit, uint32 _nof_bits_to_alloc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ALLOC_BITMAP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_ALLOC_BITMAP(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        _nof_bits_to_alloc,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_bitmap_exclude_example_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_BITMAP_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOCBITMAP,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        "example[%d]->bitmap_exclude_example",
        unit,
        _nof_bits_to_alloc);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_EXCLUDE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_exclude_example_bit_set(int unit, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_SET(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITSET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        "example[%d]->bitmap_exclude_example",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_EXCLUDE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_exclude_example_bit_get(int unit, uint32 _bit_num, uint8* result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_GET(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
         _bit_num,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_GET_LOGGING,
        BSL_LS_SWSTATEDNX_BITGET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        "example[%d]->bitmap_exclude_example",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_EXCLUDE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_exclude_example_bit_range_read(int unit, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_READ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_READ(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
         sw_state_bmp_first,
         result_first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_READ_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREAD,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        "example[%d]->bitmap_exclude_example",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_EXCLUDE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_exclude_example_bit_range_write(int unit, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_WRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_WRITE(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
         sw_state_bmp_first,
         input_bmp_first,
          _range,
         input_bmp);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_WRITE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEWRITE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        "example[%d]->bitmap_exclude_example",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_EXCLUDE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_exclude_example_bit_range_and(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_AND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_AND(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_AND_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEAND,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        "example[%d]->bitmap_exclude_example",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_EXCLUDE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_exclude_example_bit_range_or(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_OR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_OR(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_OR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEOR,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        "example[%d]->bitmap_exclude_example",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_EXCLUDE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_exclude_example_bit_range_xor(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_XOR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_XOR(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_XOR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEXOR,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        "example[%d]->bitmap_exclude_example",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_EXCLUDE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_exclude_example_bit_range_remove(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_REMOVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_REMOVE(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_REMOVE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREMOVE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        "example[%d]->bitmap_exclude_example",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_EXCLUDE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_exclude_example_bit_range_clear(int unit, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_CLEAR(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECLEAR,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        "example[%d]->bitmap_exclude_example",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_EXCLUDE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_exclude_example_bit_range_set(int unit, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_SET(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGESET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        "example[%d]->bitmap_exclude_example",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_EXCLUDE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_exclude_example_bit_range_eq(int unit, SHR_BITDCL *input_bmp, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_EQ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_EQ(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
         _first,
         _count,
         input_bmp,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_EQ_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEEQ,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        "example[%d]->bitmap_exclude_example",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_EXCLUDE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_exclude_example_bit_range_count(int unit, uint32 _first, uint32 _range, int *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_COUNT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        (_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_COUNT(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
         _first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_COUNT_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECOUNT,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        "example[%d]->bitmap_exclude_example",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_EXCLUDE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_alloc(int unit)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        sw_state_default_l2,
        dnx_data_module_testing.dbal.vrf_field_size_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_default_value_tree_array_dnxdata_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        "example[%d]->default_value_tree.array_dnxdata",
        unit,
        dnx_data_module_testing.dbal.vrf_field_size_get(unit) * sizeof(sw_state_default_l2) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_module_testing.dbal.vrf_field_size_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[def_val_idx[0]].default_value_l2.my_variable,
        5);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_free(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_FREE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_INFO,
        DNX_SW_STATE_DIAG_FREE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SW_STATE_FREE(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        "example_default_value_tree_array_dnxdata_free");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_FREE_LOGGING,
        BSL_LS_SWSTATEDNX_FREE,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        "example[%d]->default_value_tree.array_dnxdata",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_array_dynamic_alloc(int unit, uint32 array_dnxdata_idx_0, uint32 nof_instances_to_alloc_0)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        array_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SWSTATE_ALLOC_SIZE_VERIFY(
        unit,
        sizeof(sw_state_default_value),
        SW_STATE_EXAMPLE_DEFINITION * 10,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic,
        sw_state_default_l3,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_default_value_tree_array_dnxdata_array_dynamic_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic,
        "example[%d]->default_value_tree.array_dnxdata[%d].array_dynamic",
        unit, array_dnxdata_idx_0,
        nof_instances_to_alloc_0 * sizeof(sw_state_default_l3) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_DYNAMIC_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], nof_instances_to_alloc_0)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], SW_STATE_EXAMPLE_DEFINITION)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], 10)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[def_val_idx[0]].array_static_static[def_val_idx[1]][def_val_idx[2]].my_variable,
        5);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], nof_instances_to_alloc_0)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[def_val_idx[0]].default_value_l3.my_variable,
        5);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_array_dynamic_array_static_static_my_variable_set(int unit, uint32 array_dnxdata_idx_0, uint32 array_dynamic_idx_0, uint32 array_static_static_idx_0, uint32 array_static_static_idx_1, uint32 my_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        array_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic,
        array_dynamic_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        array_static_static_idx_0,
        SW_STATE_EXAMPLE_DEFINITION);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        array_static_static_idx_1,
        10);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].array_static_static[array_static_static_idx_0][array_static_static_idx_1].my_variable,
        my_variable,
        uint32,
        0,
        "example_default_value_tree_array_dnxdata_array_dynamic_array_static_static_my_variable_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &my_variable,
        "example[%d]->default_value_tree.array_dnxdata[%d].array_dynamic[%d].array_static_static[%d][%d].my_variable",
        unit, array_dnxdata_idx_0, array_dynamic_idx_0, array_static_static_idx_0, array_static_static_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_DYNAMIC_ARRAY_STATIC_STATIC_MY_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_array_dynamic_array_static_static_my_variable_get(int unit, uint32 array_dnxdata_idx_0, uint32 array_dynamic_idx_0, uint32 array_static_static_idx_0, uint32 array_static_static_idx_1, uint32 *my_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        array_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        my_variable);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic,
        array_dynamic_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        array_static_static_idx_0,
        SW_STATE_EXAMPLE_DEFINITION);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        array_static_static_idx_1,
        10);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *my_variable = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].array_static_static[array_static_static_idx_0][array_static_static_idx_1].my_variable;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].array_static_static[array_static_static_idx_0][array_static_static_idx_1].my_variable,
        "example[%d]->default_value_tree.array_dnxdata[%d].array_dynamic[%d].array_static_static[%d][%d].my_variable",
        unit, array_dnxdata_idx_0, array_dynamic_idx_0, array_static_static_idx_0, array_static_static_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_DYNAMIC_ARRAY_STATIC_STATIC_MY_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_array_dynamic_default_value_l3_my_variable_set(int unit, uint32 array_dnxdata_idx_0, uint32 array_dynamic_idx_0, uint32 my_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        array_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic,
        array_dynamic_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].default_value_l3.my_variable,
        my_variable,
        uint32,
        0,
        "example_default_value_tree_array_dnxdata_array_dynamic_default_value_l3_my_variable_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &my_variable,
        "example[%d]->default_value_tree.array_dnxdata[%d].array_dynamic[%d].default_value_l3.my_variable",
        unit, array_dnxdata_idx_0, array_dynamic_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_DYNAMIC_DEFAULT_VALUE_L3_MY_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_array_dynamic_default_value_l3_my_variable_get(int unit, uint32 array_dnxdata_idx_0, uint32 array_dynamic_idx_0, uint32 *my_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        array_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        my_variable);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic,
        array_dynamic_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *my_variable = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].default_value_l3.my_variable;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].default_value_l3.my_variable,
        "example[%d]->default_value_tree.array_dnxdata[%d].array_dynamic[%d].default_value_l3.my_variable",
        unit, array_dnxdata_idx_0, array_dynamic_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_DYNAMIC_DEFAULT_VALUE_L3_MY_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_array_dynamic_pointer_set(int unit, uint32 array_dnxdata_idx_0, uint32 array_dynamic_idx_0, uint32 pointer)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        array_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].pointer);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic,
        array_dynamic_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].pointer[0],
        pointer,
        uint32,
        0,
        "example_default_value_tree_array_dnxdata_array_dynamic_pointer_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &pointer,
        "example[%d]->default_value_tree.array_dnxdata[%d].array_dynamic[%d].pointer[%d]",
        unit, array_dnxdata_idx_0, array_dynamic_idx_0, 0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_DYNAMIC_POINTER_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_array_dynamic_pointer_get(int unit, uint32 array_dnxdata_idx_0, uint32 array_dynamic_idx_0, uint32 *pointer)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        array_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        pointer);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic,
        array_dynamic_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].pointer);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *pointer = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].pointer[0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].pointer[0],
        "example[%d]->default_value_tree.array_dnxdata[%d].array_dynamic[%d].pointer[%d]",
        unit, array_dnxdata_idx_0, array_dynamic_idx_0, 0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_DYNAMIC_POINTER_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_array_dynamic_pointer_alloc(int unit, uint32 array_dnxdata_idx_0, uint32 array_dynamic_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        array_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic,
        array_dynamic_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].pointer,
        uint32,
        1,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_default_value_tree_array_dnxdata_array_dynamic_pointer_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].pointer,
        "example[%d]->default_value_tree.array_dnxdata[%d].array_dynamic[%d].pointer",
        unit, array_dnxdata_idx_0, array_dynamic_idx_0,
        sizeof(uint32) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].pointer));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_DYNAMIC_POINTER_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].pointer[0],
        5);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_array_dynamic_array_dynamic_dynamic_alloc(int unit, uint32 array_dnxdata_idx_0, uint32 array_dynamic_idx_0, uint32 nof_instances_to_alloc_0, uint32 nof_instances_to_alloc_1)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].array_dynamic_dynamic,
        sw_state_default_value*,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_default_value_tree_array_dnxdata_array_dynamic_array_dynamic_dynamic_alloc");

    for(uint32 array_dynamic_dynamic_idx_0 = 0;
         array_dynamic_dynamic_idx_0 < nof_instances_to_alloc_0;
         array_dynamic_dynamic_idx_0++)

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].array_dynamic_dynamic[array_dynamic_dynamic_idx_0],
        sw_state_default_value,
        nof_instances_to_alloc_1,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_default_value_tree_array_dnxdata_array_dynamic_array_dynamic_dynamic_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].array_dynamic_dynamic,
        "example[%d]->default_value_tree.array_dnxdata[%d].array_dynamic[%d].array_dynamic_dynamic",
        unit, array_dnxdata_idx_0, array_dynamic_idx_0,
        nof_instances_to_alloc_1 * sizeof(sw_state_default_value) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].array_dynamic_dynamic)+(nof_instances_to_alloc_1 * sizeof(sw_state_default_value)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_DYNAMIC_ARRAY_DYNAMIC_DYNAMIC_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], nof_instances_to_alloc_0)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], nof_instances_to_alloc_1)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].array_dynamic_dynamic[def_val_idx[0]][def_val_idx[1]].my_variable,
        5);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_array_dynamic_array_dynamic_dynamic_my_variable_set(int unit, uint32 array_dnxdata_idx_0, uint32 array_dynamic_idx_0, uint32 array_dynamic_dynamic_idx_0, uint32 array_dynamic_dynamic_idx_1, uint32 my_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        array_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].array_dynamic_dynamic[array_dynamic_dynamic_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic,
        array_dynamic_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].array_dynamic_dynamic,
        array_dynamic_dynamic_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].array_dynamic_dynamic[array_dynamic_dynamic_idx_0],
        array_dynamic_dynamic_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].array_dynamic_dynamic[array_dynamic_dynamic_idx_0][array_dynamic_dynamic_idx_1].my_variable,
        my_variable,
        uint32,
        0,
        "example_default_value_tree_array_dnxdata_array_dynamic_array_dynamic_dynamic_my_variable_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &my_variable,
        "example[%d]->default_value_tree.array_dnxdata[%d].array_dynamic[%d].array_dynamic_dynamic[%d][%d].my_variable",
        unit, array_dnxdata_idx_0, array_dynamic_idx_0, array_dynamic_dynamic_idx_0, array_dynamic_dynamic_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_DYNAMIC_ARRAY_DYNAMIC_DYNAMIC_MY_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_array_dynamic_array_dynamic_dynamic_my_variable_get(int unit, uint32 array_dnxdata_idx_0, uint32 array_dynamic_idx_0, uint32 array_dynamic_dynamic_idx_0, uint32 array_dynamic_dynamic_idx_1, uint32 *my_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        array_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        my_variable);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic,
        array_dynamic_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].array_dynamic_dynamic[array_dynamic_dynamic_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].array_dynamic_dynamic,
        array_dynamic_dynamic_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].array_dynamic_dynamic[array_dynamic_dynamic_idx_0],
        array_dynamic_dynamic_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *my_variable = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].array_dynamic_dynamic[array_dynamic_dynamic_idx_0][array_dynamic_dynamic_idx_1].my_variable;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].array_dynamic_dynamic[array_dynamic_dynamic_idx_0][array_dynamic_dynamic_idx_1].my_variable,
        "example[%d]->default_value_tree.array_dnxdata[%d].array_dynamic[%d].array_dynamic_dynamic[%d][%d].my_variable",
        unit, array_dnxdata_idx_0, array_dynamic_idx_0, array_dynamic_dynamic_idx_0, array_dynamic_dynamic_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_DYNAMIC_ARRAY_DYNAMIC_DYNAMIC_MY_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}










int
example_default_value_tree_array_dnxdata_array_pointer_alloc(int unit, uint32 array_dnxdata_idx_0)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        array_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SWSTATE_ALLOC_SIZE_VERIFY(
        unit,
        sizeof(sw_state_default_value),
        SW_STATE_EXAMPLE_DEFINITION * 10,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer,
        sw_state_default_l3,
        1,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_default_value_tree_array_dnxdata_array_pointer_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer,
        "example[%d]->default_value_tree.array_dnxdata[%d].array_pointer",
        unit, array_dnxdata_idx_0,
        sizeof(sw_state_default_l3) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_POINTER_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], SW_STATE_EXAMPLE_DEFINITION)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], 10)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer->array_static_static[def_val_idx[1]][def_val_idx[2]].my_variable,
        5);

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer->default_value_l3.my_variable,
        5);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_array_pointer_array_static_static_my_variable_set(int unit, uint32 array_dnxdata_idx_0, uint32 array_static_static_idx_0, uint32 array_static_static_idx_1, uint32 my_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        array_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        array_static_static_idx_0,
        SW_STATE_EXAMPLE_DEFINITION);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        array_static_static_idx_1,
        10);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer->array_static_static[array_static_static_idx_0][array_static_static_idx_1].my_variable,
        my_variable,
        uint32,
        0,
        "example_default_value_tree_array_dnxdata_array_pointer_array_static_static_my_variable_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &my_variable,
        "example[%d]->default_value_tree.array_dnxdata[%d].array_pointer->array_static_static[%d][%d].my_variable",
        unit, array_dnxdata_idx_0, array_static_static_idx_0, array_static_static_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_POINTER_ARRAY_STATIC_STATIC_MY_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_array_pointer_array_static_static_my_variable_get(int unit, uint32 array_dnxdata_idx_0, uint32 array_static_static_idx_0, uint32 array_static_static_idx_1, uint32 *my_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        array_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        my_variable);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        array_static_static_idx_0,
        SW_STATE_EXAMPLE_DEFINITION);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        array_static_static_idx_1,
        10);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *my_variable = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer->array_static_static[array_static_static_idx_0][array_static_static_idx_1].my_variable;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer->array_static_static[array_static_static_idx_0][array_static_static_idx_1].my_variable,
        "example[%d]->default_value_tree.array_dnxdata[%d].array_pointer->array_static_static[%d][%d].my_variable",
        unit, array_dnxdata_idx_0, array_static_static_idx_0, array_static_static_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_POINTER_ARRAY_STATIC_STATIC_MY_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_array_pointer_default_value_l3_my_variable_set(int unit, uint32 array_dnxdata_idx_0, uint32 my_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        array_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer->default_value_l3.my_variable,
        my_variable,
        uint32,
        0,
        "example_default_value_tree_array_dnxdata_array_pointer_default_value_l3_my_variable_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &my_variable,
        "example[%d]->default_value_tree.array_dnxdata[%d].array_pointer->default_value_l3.my_variable",
        unit, array_dnxdata_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_POINTER_DEFAULT_VALUE_L3_MY_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_array_pointer_default_value_l3_my_variable_get(int unit, uint32 array_dnxdata_idx_0, uint32 *my_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        array_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        my_variable);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *my_variable = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer->default_value_l3.my_variable;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer->default_value_l3.my_variable,
        "example[%d]->default_value_tree.array_dnxdata[%d].array_pointer->default_value_l3.my_variable",
        unit, array_dnxdata_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_POINTER_DEFAULT_VALUE_L3_MY_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_array_pointer_pointer_set(int unit, uint32 array_dnxdata_idx_0, uint32 pointer)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        array_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer->pointer);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer->pointer[0],
        pointer,
        uint32,
        0,
        "example_default_value_tree_array_dnxdata_array_pointer_pointer_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &pointer,
        "example[%d]->default_value_tree.array_dnxdata[%d].array_pointer->pointer[%d]",
        unit, array_dnxdata_idx_0, 0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_POINTER_POINTER_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_array_pointer_pointer_get(int unit, uint32 array_dnxdata_idx_0, uint32 *pointer)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        array_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        pointer);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer->pointer);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *pointer = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer->pointer[0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer->pointer[0],
        "example[%d]->default_value_tree.array_dnxdata[%d].array_pointer->pointer[%d]",
        unit, array_dnxdata_idx_0, 0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_POINTER_POINTER_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_array_pointer_pointer_alloc(int unit, uint32 array_dnxdata_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        array_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer->pointer,
        uint32,
        1,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_default_value_tree_array_dnxdata_array_pointer_pointer_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer->pointer,
        "example[%d]->default_value_tree.array_dnxdata[%d].array_pointer->pointer",
        unit, array_dnxdata_idx_0,
        sizeof(uint32) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer->pointer));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_POINTER_POINTER_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer->pointer[0],
        5);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_array_pointer_array_dynamic_dynamic_alloc(int unit, uint32 array_dnxdata_idx_0, uint32 nof_instances_to_alloc_0, uint32 nof_instances_to_alloc_1)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer->array_dynamic_dynamic,
        sw_state_default_value*,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_default_value_tree_array_dnxdata_array_pointer_array_dynamic_dynamic_alloc");

    for(uint32 array_dynamic_dynamic_idx_0 = 0;
         array_dynamic_dynamic_idx_0 < nof_instances_to_alloc_0;
         array_dynamic_dynamic_idx_0++)

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer->array_dynamic_dynamic[array_dynamic_dynamic_idx_0],
        sw_state_default_value,
        nof_instances_to_alloc_1,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_default_value_tree_array_dnxdata_array_pointer_array_dynamic_dynamic_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer->array_dynamic_dynamic,
        "example[%d]->default_value_tree.array_dnxdata[%d].array_pointer->array_dynamic_dynamic",
        unit, array_dnxdata_idx_0,
        nof_instances_to_alloc_1 * sizeof(sw_state_default_value) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer->array_dynamic_dynamic)+(nof_instances_to_alloc_1 * sizeof(sw_state_default_value)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_POINTER_ARRAY_DYNAMIC_DYNAMIC_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], nof_instances_to_alloc_0)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], nof_instances_to_alloc_1)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer->array_dynamic_dynamic[def_val_idx[0]][def_val_idx[1]].my_variable,
        5);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_array_pointer_array_dynamic_dynamic_my_variable_set(int unit, uint32 array_dnxdata_idx_0, uint32 array_dynamic_dynamic_idx_0, uint32 array_dynamic_dynamic_idx_1, uint32 my_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        array_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer->array_dynamic_dynamic[array_dynamic_dynamic_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer->array_dynamic_dynamic,
        array_dynamic_dynamic_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer->array_dynamic_dynamic[array_dynamic_dynamic_idx_0],
        array_dynamic_dynamic_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer->array_dynamic_dynamic[array_dynamic_dynamic_idx_0][array_dynamic_dynamic_idx_1].my_variable,
        my_variable,
        uint32,
        0,
        "example_default_value_tree_array_dnxdata_array_pointer_array_dynamic_dynamic_my_variable_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &my_variable,
        "example[%d]->default_value_tree.array_dnxdata[%d].array_pointer->array_dynamic_dynamic[%d][%d].my_variable",
        unit, array_dnxdata_idx_0, array_dynamic_dynamic_idx_0, array_dynamic_dynamic_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_POINTER_ARRAY_DYNAMIC_DYNAMIC_MY_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_array_pointer_array_dynamic_dynamic_my_variable_get(int unit, uint32 array_dnxdata_idx_0, uint32 array_dynamic_dynamic_idx_0, uint32 array_dynamic_dynamic_idx_1, uint32 *my_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        array_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        my_variable);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer->array_dynamic_dynamic,
        array_dynamic_dynamic_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer->array_dynamic_dynamic[array_dynamic_dynamic_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer->array_dynamic_dynamic[array_dynamic_dynamic_idx_0],
        array_dynamic_dynamic_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *my_variable = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer->array_dynamic_dynamic[array_dynamic_dynamic_idx_0][array_dynamic_dynamic_idx_1].my_variable;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_pointer->array_dynamic_dynamic[array_dynamic_dynamic_idx_0][array_dynamic_dynamic_idx_1].my_variable,
        "example[%d]->default_value_tree.array_dnxdata[%d].array_pointer->array_dynamic_dynamic[%d][%d].my_variable",
        unit, array_dnxdata_idx_0, array_dynamic_dynamic_idx_0, array_dynamic_dynamic_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_POINTER_ARRAY_DYNAMIC_DYNAMIC_MY_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}










int
example_default_value_tree_array_dnxdata_default_value_l2_my_variable_set(int unit, uint32 array_dnxdata_idx_0, uint32 my_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        array_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].default_value_l2.my_variable,
        my_variable,
        uint32,
        0,
        "example_default_value_tree_array_dnxdata_default_value_l2_my_variable_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &my_variable,
        "example[%d]->default_value_tree.array_dnxdata[%d].default_value_l2.my_variable",
        unit, array_dnxdata_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_DEFAULT_VALUE_L2_MY_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_array_dnxdata_default_value_l2_my_variable_get(int unit, uint32 array_dnxdata_idx_0, uint32 *my_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        array_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        my_variable);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *my_variable = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].default_value_l2.my_variable;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].default_value_l2.my_variable,
        "example[%d]->default_value_tree.array_dnxdata[%d].default_value_l2.my_variable",
        unit, array_dnxdata_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_DEFAULT_VALUE_L2_MY_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_default_value_l1_my_variable_set(int unit, uint32 my_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.default_value_l1.my_variable,
        my_variable,
        uint32,
        0,
        "example_default_value_tree_default_value_l1_my_variable_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &my_variable,
        "example[%d]->default_value_tree.default_value_l1.my_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_DEFAULT_VALUE_L1_MY_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_default_value_tree_default_value_l1_my_variable_get(int unit, uint32 *my_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        my_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *my_variable = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.default_value_l1.my_variable;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.default_value_l1.my_variable,
        "example[%d]->default_value_tree.default_value_l1.my_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_DEFAULT_VALUE_L1_MY_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}








int
example_ll_create_empty(int unit, sw_state_ll_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_CREATE_EMPTY,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_LL_CREATE_EMPTY(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        init_info,
        SW_STATE_LL_NO_FLAGS,
        non_existing_dummy_type_t,
        int,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_CREATE_EMPTY_LOGGING,
        BSL_LS_SWSTATEDNX_LLCREATE_EMPTY,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        "example[%d]->ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_ll_nof_elements(int unit, uint32 *nof_elements)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_NOF_ELEMENTS,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_LL_NOF_ELEMENTS(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        nof_elements);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NOF_ELEMENTS_LOGGING,
        BSL_LS_SWSTATEDNX_LLNOF_ELEMENTS,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        "example[%d]->ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_ll_node_value(int unit, sw_state_ll_node_t node, int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_NODE_VALUE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_LL_NODE_VALUE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        node,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NODE_VALUE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNODE_VALUE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        "example[%d]->ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_ll_node_update(int unit, sw_state_ll_node_t node, const int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_NODE_UPDATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_LL_NODE_UPDATE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        node,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NODE_UPDATE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNODE_UPDATE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        "example[%d]->ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_ll_next_node(int unit, sw_state_ll_node_t node, sw_state_ll_node_t *next_node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_NEXT_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_LL_NEXT_NODE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        node,
        next_node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NEXT_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNEXT_NODE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        "example[%d]->ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_ll_previous_node(int unit, sw_state_ll_node_t node, sw_state_ll_node_t *prev_node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_PREVIOUS_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_LL_PREVIOUS_NODE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        node,
        prev_node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_PREVIOUS_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLPREVIOUS_NODE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        "example[%d]->ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_ll_add_first(int unit, const int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ADD_FIRST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_LL_ADD_FIRST(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        NULL,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_FIRST_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD_FIRST,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        "example[%d]->ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_ll_add_last(int unit, const int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ADD_LAST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_LL_ADD_LAST(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        NULL,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_LAST_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD_LAST,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        "example[%d]->ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_ll_add_before(int unit, sw_state_ll_node_t node, const int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ADD_BEFORE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_LL_ADD_BEFORE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        node,
        NULL,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_BEFORE_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD_BEFORE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        "example[%d]->ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_ll_add_after(int unit, sw_state_ll_node_t node, const int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ADD_AFTER,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_LL_ADD_AFTER(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        node,
        NULL,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_AFTER_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD_AFTER,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        "example[%d]->ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_ll_remove_node(int unit, sw_state_ll_node_t node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_REMOVE_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_LL_REMOVE_NODE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_REMOVE_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLREMOVE_NODE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        "example[%d]->ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_ll_get_last(int unit, sw_state_ll_node_t *node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET_LAST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_LL_GET_LAST(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_GET_LAST_LOGGING,
        BSL_LS_SWSTATEDNX_LLGET_LAST,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        "example[%d]->ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_ll_get_first(int unit, sw_state_ll_node_t *node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET_FIRST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_LL_GET_FIRST(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_GET_FIRST_LOGGING,
        BSL_LS_SWSTATEDNX_LLGET_FIRST,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        "example[%d]->ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_ll_print(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_PRINT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_LL_PRINT(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_PRINT_LOGGING,
        BSL_LS_SWSTATEDNX_LLPRINT,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        "example[%d]->ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}
































int
example_multihead_ll_create_empty(int unit, sw_state_ll_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_CREATE_EMPTY,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_MULTIHEAD_LL_CREATE_EMPTY(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        init_info,
        SW_STATE_LL_MULTIHEAD,
        non_existing_dummy_type_t,
        int,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_CREATE_EMPTY_LOGGING,
        BSL_LS_SWSTATEDNX_LLCREATE_EMPTY,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        "example[%d]->multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_multihead_ll_nof_elements(int unit, uint32 ll_head_index, uint32 *nof_elements)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_NOF_ELEMENTS,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_MULTIHEAD_LL_NOF_ELEMENTS(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        ll_head_index,
        nof_elements);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NOF_ELEMENTS_LOGGING,
        BSL_LS_SWSTATEDNX_LLNOF_ELEMENTS,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        "example[%d]->multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_multihead_ll_node_value(int unit, sw_state_ll_node_t node, int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_NODE_VALUE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_MULTIHEAD_LL_NODE_VALUE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        node,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NODE_VALUE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNODE_VALUE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        "example[%d]->multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_multihead_ll_node_update(int unit, sw_state_ll_node_t node, const int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_NODE_UPDATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_MULTIHEAD_LL_NODE_UPDATE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        node,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NODE_UPDATE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNODE_UPDATE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        "example[%d]->multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_multihead_ll_next_node(int unit, uint32 ll_head_index, sw_state_ll_node_t node, sw_state_ll_node_t *next_node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_NEXT_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_MULTIHEAD_LL_NEXT_NODE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        ll_head_index,
        node,
        next_node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NEXT_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNEXT_NODE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        "example[%d]->multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_multihead_ll_previous_node(int unit, uint32 ll_head_index, sw_state_ll_node_t node, sw_state_ll_node_t *prev_node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_PREVIOUS_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_MULTIHEAD_LL_PREVIOUS_NODE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        ll_head_index,
        node,
        prev_node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_PREVIOUS_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLPREVIOUS_NODE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        "example[%d]->multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_multihead_ll_add_first(int unit, uint32 ll_head_index, const int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ADD_FIRST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_MULTIHEAD_LL_ADD_FIRST(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        ll_head_index,
        NULL,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_FIRST_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD_FIRST,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        "example[%d]->multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_multihead_ll_add_last(int unit, uint32 ll_head_index, const int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ADD_LAST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_MULTIHEAD_LL_ADD_LAST(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        ll_head_index,
        NULL,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_LAST_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD_LAST,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        "example[%d]->multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_multihead_ll_add_before(int unit, uint32 ll_head_index, sw_state_ll_node_t node, const int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ADD_BEFORE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_MULTIHEAD_LL_ADD_BEFORE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        ll_head_index,
        node,
        NULL,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_BEFORE_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD_BEFORE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        "example[%d]->multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_multihead_ll_add_after(int unit, uint32 ll_head_index, sw_state_ll_node_t node, const int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ADD_AFTER,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_MULTIHEAD_LL_ADD_AFTER(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        ll_head_index,
        node,
        NULL,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_AFTER_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD_AFTER,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        "example[%d]->multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_multihead_ll_remove_node(int unit, uint32 ll_head_index, sw_state_ll_node_t node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_REMOVE_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_MULTIHEAD_LL_REMOVE_NODE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        ll_head_index,
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_REMOVE_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLREMOVE_NODE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        "example[%d]->multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_multihead_ll_get_last(int unit, uint32 ll_head_index, sw_state_ll_node_t *node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET_LAST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_MULTIHEAD_LL_GET_LAST(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        ll_head_index,
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_GET_LAST_LOGGING,
        BSL_LS_SWSTATEDNX_LLGET_LAST,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        "example[%d]->multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_multihead_ll_get_first(int unit, uint32 ll_head_index, sw_state_ll_node_t *node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET_FIRST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_MULTIHEAD_LL_GET_FIRST(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        ll_head_index,
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_GET_FIRST_LOGGING,
        BSL_LS_SWSTATEDNX_LLGET_FIRST,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        "example[%d]->multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_multihead_ll_print(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_PRINT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_MULTIHEAD_LL_PRINT(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_PRINT_LOGGING,
        BSL_LS_SWSTATEDNX_LLPRINT,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        "example[%d]->multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_sorted_ll_create_empty(int unit, sw_state_ll_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_CREATE_EMPTY,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_LL_CREATE_EMPTY(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        init_info,
        SW_STATE_LL_SORTED,
        uint8,
        uint8,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_CREATE_EMPTY_LOGGING,
        BSL_LS_SWSTATEDNX_LLCREATE_EMPTY,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        "example[%d]->sorted_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_SORTED_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_sorted_ll_nof_elements(int unit, uint32 *nof_elements)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_NOF_ELEMENTS,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_LL_NOF_ELEMENTS(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        nof_elements);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NOF_ELEMENTS_LOGGING,
        BSL_LS_SWSTATEDNX_LLNOF_ELEMENTS,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        "example[%d]->sorted_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_SORTED_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_sorted_ll_node_value(int unit, sw_state_ll_node_t node, uint8 *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_NODE_VALUE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_LL_NODE_VALUE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        node,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NODE_VALUE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNODE_VALUE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        "example[%d]->sorted_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_SORTED_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_sorted_ll_node_update(int unit, sw_state_ll_node_t node, const uint8 *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_NODE_UPDATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_LL_NODE_UPDATE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        node,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NODE_UPDATE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNODE_UPDATE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        "example[%d]->sorted_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_SORTED_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_sorted_ll_next_node(int unit, sw_state_ll_node_t node, sw_state_ll_node_t *next_node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_NEXT_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_LL_NEXT_NODE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        node,
        next_node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NEXT_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNEXT_NODE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        "example[%d]->sorted_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_SORTED_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_sorted_ll_previous_node(int unit, sw_state_ll_node_t node, sw_state_ll_node_t *prev_node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_PREVIOUS_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_LL_PREVIOUS_NODE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        node,
        prev_node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_PREVIOUS_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLPREVIOUS_NODE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        "example[%d]->sorted_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_SORTED_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_sorted_ll_add_first(int unit, const uint8 *key, const uint8 *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ADD_FIRST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_LL_ADD_FIRST(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        key,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_FIRST_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD_FIRST,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        "example[%d]->sorted_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_SORTED_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_sorted_ll_add_last(int unit, const uint8 *key, const uint8 *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ADD_LAST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_LL_ADD_LAST(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        key,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_LAST_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD_LAST,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        "example[%d]->sorted_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_SORTED_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_sorted_ll_add_before(int unit, sw_state_ll_node_t node, const uint8 *key, const uint8 *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ADD_BEFORE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_LL_ADD_BEFORE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        node,
        key,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_BEFORE_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD_BEFORE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        "example[%d]->sorted_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_SORTED_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_sorted_ll_add_after(int unit, sw_state_ll_node_t node, const uint8 *key, const uint8 *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ADD_AFTER,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_LL_ADD_AFTER(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        node,
        key,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_AFTER_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD_AFTER,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        "example[%d]->sorted_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_SORTED_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_sorted_ll_remove_node(int unit, sw_state_ll_node_t node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_REMOVE_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_LL_REMOVE_NODE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_REMOVE_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLREMOVE_NODE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        "example[%d]->sorted_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_SORTED_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_sorted_ll_get_last(int unit, sw_state_ll_node_t *node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET_LAST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_LL_GET_LAST(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_GET_LAST_LOGGING,
        BSL_LS_SWSTATEDNX_LLGET_LAST,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        "example[%d]->sorted_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_SORTED_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_sorted_ll_get_first(int unit, sw_state_ll_node_t *node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET_FIRST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_LL_GET_FIRST(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_GET_FIRST_LOGGING,
        BSL_LS_SWSTATEDNX_LLGET_FIRST,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        "example[%d]->sorted_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_SORTED_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_sorted_ll_print(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_PRINT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_LL_PRINT(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_PRINT_LOGGING,
        BSL_LS_SWSTATEDNX_LLPRINT,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        "example[%d]->sorted_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_SORTED_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_sorted_ll_add(int unit, const uint8 *key, const uint8 *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ADD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_LL_ADD(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        key,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        "example[%d]->sorted_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_SORTED_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_sorted_ll_node_key(int unit, sw_state_ll_node_t node, uint8 *key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_NODE_KEY,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_LL_NODE_KEY(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        node,
        key);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NODE_KEY_LOGGING,
        BSL_LS_SWSTATEDNX_LLNODE_KEY,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        "example[%d]->sorted_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_SORTED_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_sorted_ll_find(int unit, sw_state_ll_node_t *node, const uint8 *key, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_FIND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_LL_FIND(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        node,
        key,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_FIND_LOGGING,
        BSL_LS_SWSTATEDNX_LLFIND,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        "example[%d]->sorted_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_SORTED_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bt_create_empty(int unit, sw_state_bt_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_CREATE_EMPTY,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_BT_CREATE_EMPTY(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        init_info,
        int,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BT_CREATE_EMPTY_LOGGING,
        BSL_LS_SWSTATEDNX_BTCREATE_EMPTY,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        "example[%d]->bt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BT_INFO,
        DNX_SW_STATE_DIAG_BT,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bt_destroy(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_DESTROY,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_BT_DESTROY(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BT_DESTROY_LOGGING,
        BSL_LS_SWSTATEDNX_BTDESTROY,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        "example[%d]->bt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BT_INFO,
        DNX_SW_STATE_DIAG_BT,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bt_nof_elements(int unit, uint32 *nof_elements)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_NOF_ELEMENTS,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_BT_NOF_ELEMENTS(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        nof_elements);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BT_NOF_ELEMENTS_LOGGING,
        BSL_LS_SWSTATEDNX_BTNOF_ELEMENTS,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        "example[%d]->bt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BT_INFO,
        DNX_SW_STATE_DIAG_BT,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bt_node_value(int unit, sw_state_bt_node_t node, int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_NODE_VALUE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_BT_NODE_VALUE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        node,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BT_NODE_VALUE_LOGGING,
        BSL_LS_SWSTATEDNX_BTNODE_VALUE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        "example[%d]->bt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BT_INFO,
        DNX_SW_STATE_DIAG_BT,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bt_node_update(int unit, sw_state_bt_node_t node, const int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_NODE_UPDATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_BT_NODE_UPDATE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        node,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BT_NODE_UPDATE_LOGGING,
        BSL_LS_SWSTATEDNX_BTNODE_UPDATE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        "example[%d]->bt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BT_INFO,
        DNX_SW_STATE_DIAG_BT,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bt_node_free(int unit, sw_state_bt_node_t node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_NODE_FREE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_BT_NODE_FREE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BT_NODE_FREE_LOGGING,
        BSL_LS_SWSTATEDNX_BTNODE_FREE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        "example[%d]->bt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BT_INFO,
        DNX_SW_STATE_DIAG_BT,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bt_get_root(int unit, sw_state_bt_node_t *root)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET_ROOT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_BT_GET_ROOT(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        root);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BT_GET_ROOT_LOGGING,
        BSL_LS_SWSTATEDNX_BTGET_ROOT,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        "example[%d]->bt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BT_INFO,
        DNX_SW_STATE_DIAG_BT,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bt_get_parent(int unit, sw_state_bt_node_t node, sw_state_bt_node_t *node_parent)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET_PARENT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_BT_GET_PARENT(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        node,
        node_parent);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BT_GET_PARENT_LOGGING,
        BSL_LS_SWSTATEDNX_BTGET_PARENT,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        "example[%d]->bt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BT_INFO,
        DNX_SW_STATE_DIAG_BT,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bt_get_left_child(int unit, sw_state_bt_node_t node, sw_state_bt_node_t *node_left)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET_LEFT_CHILD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_BT_GET_LEFT_CHILD(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        node,
        node_left);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BT_GET_LEFT_CHILD_LOGGING,
        BSL_LS_SWSTATEDNX_BTGET_LEFT_CHILD,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        "example[%d]->bt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BT_INFO,
        DNX_SW_STATE_DIAG_BT,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bt_get_right_child(int unit, sw_state_bt_node_t node, sw_state_bt_node_t *node_right)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET_RIGHT_CHILD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_BT_GET_RIGHT_CHILD(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        node,
        node_right);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BT_GET_RIGHT_CHILD_LOGGING,
        BSL_LS_SWSTATEDNX_BTGET_RIGHT_CHILD,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        "example[%d]->bt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BT_INFO,
        DNX_SW_STATE_DIAG_BT,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bt_add_root(int unit, const int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ADD_ROOT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_BT_ADD_ROOT(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BT_ADD_ROOT_LOGGING,
        BSL_LS_SWSTATEDNX_BTADD_ROOT,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        "example[%d]->bt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BT_INFO,
        DNX_SW_STATE_DIAG_BT,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bt_add_left_child(int unit, sw_state_bt_node_t node, const int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ADD_LEFT_CHILD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_BT_ADD_LEFT_CHILD(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        node,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BT_ADD_LEFT_CHILD_LOGGING,
        BSL_LS_SWSTATEDNX_BTADD_LEFT_CHILD,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        "example[%d]->bt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BT_INFO,
        DNX_SW_STATE_DIAG_BT,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bt_add_right_child(int unit, sw_state_bt_node_t node, const int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ADD_RIGHT_CHILD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_BT_ADD_RIGHT_CHILD(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        node,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BT_ADD_RIGHT_CHILD_LOGGING,
        BSL_LS_SWSTATEDNX_BTADD_RIGHT_CHILD,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        "example[%d]->bt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BT_INFO,
        DNX_SW_STATE_DIAG_BT,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bt_set_parent(int unit, sw_state_bt_node_t node, sw_state_bt_node_t node_parent)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET_PARENT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_BT_SET_PARENT(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        node,
        node_parent);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BT_SET_PARENT_LOGGING,
        BSL_LS_SWSTATEDNX_BTSET_PARENT,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        "example[%d]->bt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BT_INFO,
        DNX_SW_STATE_DIAG_BT,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bt_set_left_child(int unit, sw_state_bt_node_t node, sw_state_bt_node_t node_left)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET_LEFT_CHILD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_BT_SET_LEFT_CHILD(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        node,
        node_left);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BT_SET_LEFT_CHILD_LOGGING,
        BSL_LS_SWSTATEDNX_BTSET_LEFT_CHILD,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        "example[%d]->bt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BT_INFO,
        DNX_SW_STATE_DIAG_BT,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bt_set_right_child(int unit, sw_state_bt_node_t node, sw_state_bt_node_t node_right)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET_RIGHT_CHILD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_BT_SET_RIGHT_CHILD(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        node,
        node_right);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BT_SET_RIGHT_CHILD_LOGGING,
        BSL_LS_SWSTATEDNX_BTSET_RIGHT_CHILD,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        "example[%d]->bt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BT_INFO,
        DNX_SW_STATE_DIAG_BT,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bt_print(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_PRINT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_BT_PRINT(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BT_PRINT_LOGGING,
        BSL_LS_SWSTATEDNX_BTPRINT,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        "example[%d]->bt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BT_INFO,
        DNX_SW_STATE_DIAG_BT,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_htb_create(int unit, sw_state_htbl_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_CREATE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb,
        init_info,
        int,
        int,
        FALSE,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCREATE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb,
        "example[%d]->htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_htb_find(int unit, const int *key, int *value, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_FIND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_FIND(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb,
        key,
        value,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_FIND_LOGGING,
        BSL_LS_SWSTATEDNX_HTBFIND,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb,
        "example[%d]->htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_htb_insert(int unit, const int *key, const int *value, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_INSERT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_INSERT(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb,
        key,
        value,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_INSERT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBINSERT,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb,
        "example[%d]->htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_htb_get_next(int unit, SW_STATE_HASH_TABLE_ITER *iter, const int *key, const int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_GET_NEXT(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb,
        iter,
        key,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_GET_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBGET_NEXT,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb,
        "example[%d]->htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_htb_clear(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_CLEAR(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCLEAR,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb,
        "example[%d]->htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_htb_delete(int unit, const int *key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_DELETE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_DELETE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb,
        key);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_DELETE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBDELETE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb,
        "example[%d]->htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_htb_delete_all(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_DELETE_ALL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_DELETE_ALL(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_DELETE_ALL_LOGGING,
        BSL_LS_SWSTATEDNX_HTBDELETE_ALL,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb,
        "example[%d]->htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_htb_print(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_PRINT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_PRINT(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_PRINT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBPRINT,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb,
        "example[%d]->htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_htb_arr_create(int unit, uint32 htb_arr_idx_0, sw_state_htbl_init_info_t *init_info, int key_size, int value_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_CREATE_WITH_SIZE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_arr[htb_arr_idx_0],
        init_info,
        key_size,
        value_size,
        FALSE,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCREATE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_arr[htb_arr_idx_0],
        "example[%d]->htb_arr[%d]",
        unit, htb_arr_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_HTB_ARR_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_htb_arr_find(int unit, uint32 htb_arr_idx_0, const void *key, void *value, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_FIND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_FIND(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_arr[htb_arr_idx_0],
        key,
        value,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_FIND_LOGGING,
        BSL_LS_SWSTATEDNX_HTBFIND,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_arr[htb_arr_idx_0],
        "example[%d]->htb_arr[%d]",
        unit, htb_arr_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_HTB_ARR_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_htb_arr_insert(int unit, uint32 htb_arr_idx_0, const void *key, const void *value, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_INSERT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_INSERT(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_arr[htb_arr_idx_0],
        key,
        value,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_INSERT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBINSERT,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_arr[htb_arr_idx_0],
        "example[%d]->htb_arr[%d]",
        unit, htb_arr_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_HTB_ARR_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_htb_arr_get_next(int unit, uint32 htb_arr_idx_0, SW_STATE_HASH_TABLE_ITER *iter, const void *key, const void *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_GET_NEXT(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_arr[htb_arr_idx_0],
        iter,
        key,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_GET_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBGET_NEXT,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_arr[htb_arr_idx_0],
        "example[%d]->htb_arr[%d]",
        unit, htb_arr_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_HTB_ARR_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_htb_arr_clear(int unit, uint32 htb_arr_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_CLEAR(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_arr[htb_arr_idx_0]);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCLEAR,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_arr[htb_arr_idx_0],
        "example[%d]->htb_arr[%d]",
        unit, htb_arr_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_HTB_ARR_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_htb_arr_delete(int unit, uint32 htb_arr_idx_0, const void *key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_DELETE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_DELETE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_arr[htb_arr_idx_0],
        key);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_DELETE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBDELETE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_arr[htb_arr_idx_0],
        "example[%d]->htb_arr[%d]",
        unit, htb_arr_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_HTB_ARR_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_htb_arr_delete_all(int unit, uint32 htb_arr_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_DELETE_ALL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_DELETE_ALL(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_arr[htb_arr_idx_0]);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_DELETE_ALL_LOGGING,
        BSL_LS_SWSTATEDNX_HTBDELETE_ALL,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_arr[htb_arr_idx_0],
        "example[%d]->htb_arr[%d]",
        unit, htb_arr_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_HTB_ARR_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_index_htb_create(int unit, sw_state_htbl_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_CREATE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        init_info,
        int,
        uint32,
        TRUE,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCREATE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        "example[%d]->index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_index_htb_find(int unit, const int *key, uint32 *data_index, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_FIND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_FIND(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        key,
        data_index,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_FIND_LOGGING,
        BSL_LS_SWSTATEDNX_HTBFIND,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        "example[%d]->index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_index_htb_insert(int unit, const int *key, const uint32 *data_index, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_INSERT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_INSERT(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        key,
        data_index,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_INSERT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBINSERT,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        "example[%d]->index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_index_htb_get_next(int unit, SW_STATE_HASH_TABLE_ITER *iter, const int *key, const uint32 *data_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_GET_NEXT(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        iter,
        key,
        data_index);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_GET_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBGET_NEXT,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        "example[%d]->index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_index_htb_clear(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_CLEAR(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCLEAR,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        "example[%d]->index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_index_htb_delete(int unit, const int *key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_DELETE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_DELETE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        key);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_DELETE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBDELETE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        "example[%d]->index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_index_htb_delete_all(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_DELETE_ALL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_DELETE_ALL(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_DELETE_ALL_LOGGING,
        BSL_LS_SWSTATEDNX_HTBDELETE_ALL,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        "example[%d]->index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_index_htb_print(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_PRINT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_PRINT(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_PRINT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBPRINT,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        "example[%d]->index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_index_htb_insert_at_index(int unit, const int *key, uint32 data_idx, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_INSERT_AT_INDEX,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_INSERT_AT_INDEX(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        key,
        data_idx,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_INSERT_AT_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_HTBINSERT_AT_INDEX,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        "example[%d]->index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_index_htb_delete_by_index(int unit, uint32 data_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_DELETE_BY_INDEX,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_DELETE_BY_INDEX(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        data_index);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_DELETE_BY_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_HTBDELETE_BY_INDEX,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        "example[%d]->index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_index_htb_get_by_index(int unit, uint32 data_index, int *key, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET_BY_INDEX,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_GET_BY_INDEX(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        data_index,
        key,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_GET_BY_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_HTBGET_BY_INDEX,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        "example[%d]->index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_occ_create(int unit, sw_state_occ_bitmap_init_info_t * init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_OCC_BM_CREATE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ,
        init_info,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMCREATE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ,
        "example[%d]->occ",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_OCC_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_occ_get_next(int unit, uint32 *place, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_OCC_BM_GET_NEXT(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ,
        place,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_GET_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMGET_NEXT,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ,
        "example[%d]->occ",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_OCC_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_occ_get_next_in_range(int unit, uint32 start, uint32 end, uint8 forward, uint8 val, uint32 *place, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET_NEXT_IN_RANGE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_OCC_BM_GET_NEXT_IN_RANGE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ,
        start,
        end,
        forward,
        val,
        place,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_GET_NEXT_IN_RANGE_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMGET_NEXT_IN_RANGE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ,
        "example[%d]->occ",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_OCC_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_occ_status_set(int unit, uint32 place, uint8 occupied)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_STATUS_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_OCC_BM_STATUS_SET(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ,
        place,
        occupied);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_STATUS_SET_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMSTATUS_SET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ,
        "example[%d]->occ",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_OCC_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_occ_is_occupied(int unit, uint32 place, uint8 *occupied)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_IS_OCCUPIED,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_OCC_BM_IS_OCCUPIED(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ,
        place,
        occupied);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_IS_OCCUPIED_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMIS_OCCUPIED,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ,
        "example[%d]->occ",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_OCC_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_occ_alloc_next(int unit, uint32 *place, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ALLOC_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_OCC_BM_ALLOC_NEXT(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ,
        place,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_ALLOC_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMALLOC_NEXT,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ,
        "example[%d]->occ",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_OCC_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_occ_clear(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_OCC_BM_CLEAR(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMCLEAR,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ,
        "example[%d]->occ",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_OCC_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_occ_print(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_PRINT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_OCC_BM_PRINT(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_PRINT_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMPRINT,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ,
        "example[%d]->occ",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_OCC_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_cb_get_cb(int unit, dnx_sw_state_callback_test_function_cb* cb)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET_CB,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_CB_DB_GET_CB(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->cb,
        cb,
        dnx_sw_state_callback_test_function_cb_get_cb);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_CB_DB_GET_CB_LOGGING,
        BSL_LS_SWSTATEDNX_CB_DBGET_CB,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->cb,
        "example[%d]->cb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_CB_INFO,
        DNX_SW_STATE_DIAG_CB_DB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_cb_register_cb(int unit, char *name)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_REGISTER_CB,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_CB_DB_REGISTER_CB(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->cb,
        name,
        dnx_sw_state_callback_test_function_cb_get_cb);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_CB_DB_REGISTER_CB_LOGGING,
        BSL_LS_SWSTATEDNX_CB_DBREGISTER_CB,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->cb,
        "example[%d]->cb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_CB_INFO,
        DNX_SW_STATE_DIAG_CB_DB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_cb_unregister_cb(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_UNREGISTER_CB,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_CB_DB_UNREGISTER_CB(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->cb);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_CB_DB_UNREGISTER_CB_LOGGING,
        BSL_LS_SWSTATEDNX_CB_DBUNREGISTER_CB,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->cb,
        "example[%d]->cb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_CB_INFO,
        DNX_SW_STATE_DIAG_CB_DB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_leaf_struct_set(int unit, CONST sw_state_leaf_struct *leaf_struct)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->leaf_struct,
        leaf_struct,
        sw_state_leaf_struct,
        0,
        "example_leaf_struct_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        leaf_struct,
        "example[%d]->leaf_struct",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LEAF_STRUCT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_leaf_struct_get(int unit, sw_state_leaf_struct *leaf_struct)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        leaf_struct);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *leaf_struct = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->leaf_struct;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->leaf_struct,
        "example[%d]->leaf_struct",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LEAF_STRUCT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_defragmented_chunk_example_create(int unit, uint32 defragmented_chunk_example_idx_0, sw_state_defragmented_chunk_init_info_t * init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_DEFRAGMENTED_CHUNK_CREATE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->defragmented_chunk_example[defragmented_chunk_example_idx_0],
        init_info,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_DEFRAGMENTED_CHUNK_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_DEFRAGMENTED_CHUNKCREATE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->defragmented_chunk_example[defragmented_chunk_example_idx_0],
        "example[%d]->defragmented_chunk_example[%d]",
        unit, defragmented_chunk_example_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DEFRAGMENTED_CHUNK_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_DEFRAGMENTED_CHUNK,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_defragmented_chunk_example_piece_alloc(int unit, uint32 defragmented_chunk_example_idx_0, uint32 slots_in_piece, void *move_cb_additional_info, uint32 *piece_offset)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_PIECE_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_DEFRAGMENTED_CHUNK_PIECE_ALLOC(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->defragmented_chunk_example[defragmented_chunk_example_idx_0],
        slots_in_piece,
        move_cb_additional_info,
        piece_offset);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_DEFRAGMENTED_CHUNK_PIECE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_DEFRAGMENTED_CHUNKPIECE_ALLOC,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->defragmented_chunk_example[defragmented_chunk_example_idx_0],
        "example[%d]->defragmented_chunk_example[%d]",
        unit, defragmented_chunk_example_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DEFRAGMENTED_CHUNK_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_DEFRAGMENTED_CHUNK,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_defragmented_chunk_example_piece_free(int unit, uint32 defragmented_chunk_example_idx_0, uint32 piece_offset)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_PIECE_FREE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_DEFRAGMENTED_CHUNK_PIECE_FREE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->defragmented_chunk_example[defragmented_chunk_example_idx_0],
        piece_offset);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_DEFRAGMENTED_CHUNK_PIECE_FREE_LOGGING,
        BSL_LS_SWSTATEDNX_DEFRAGMENTED_CHUNKPIECE_FREE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->defragmented_chunk_example[defragmented_chunk_example_idx_0],
        "example[%d]->defragmented_chunk_example[%d]",
        unit, defragmented_chunk_example_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DEFRAGMENTED_CHUNK_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_DEFRAGMENTED_CHUNK,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_defragmented_chunk_example_print(int unit, uint32 defragmented_chunk_example_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_PRINT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_DEFRAGMENTED_CHUNK_PRINT(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->defragmented_chunk_example[defragmented_chunk_example_idx_0]);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_DEFRAGMENTED_CHUNK_PRINT_LOGGING,
        BSL_LS_SWSTATEDNX_DEFRAGMENTED_CHUNKPRINT,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->defragmented_chunk_example[defragmented_chunk_example_idx_0],
        "example[%d]->defragmented_chunk_example[%d]",
        unit, defragmented_chunk_example_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_DEFRAGMENTED_CHUNK_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_DEFRAGMENTED_CHUNK,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_packed_expecto_patronum_set(int unit, char expecto_patronum)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->packed.expecto_patronum,
        expecto_patronum,
        char,
        0,
        "example_packed_expecto_patronum_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &expecto_patronum,
        "example[%d]->packed.expecto_patronum",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_PACKED_EXPECTO_PATRONUM_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_packed_expecto_patronum_get(int unit, char *expecto_patronum)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        expecto_patronum);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *expecto_patronum = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->packed.expecto_patronum;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->packed.expecto_patronum,
        "example[%d]->packed.expecto_patronum",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_PACKED_EXPECTO_PATRONUM_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_packed_my_integer_set(int unit, int my_integer)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->packed.my_integer,
        my_integer,
        int,
        0,
        "example_packed_my_integer_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &my_integer,
        "example[%d]->packed.my_integer",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_PACKED_MY_INTEGER_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_packed_my_integer_get(int unit, int *my_integer)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        my_integer);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *my_integer = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->packed.my_integer;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->packed.my_integer,
        "example[%d]->packed.my_integer",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_PACKED_MY_INTEGER_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_my_union_my_integer_set(int unit, int my_integer)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->my_union.my_integer,
        my_integer,
        int,
        0,
        "example_my_union_my_integer_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &my_integer,
        "example[%d]->my_union.my_integer",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_MY_UNION_MY_INTEGER_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_my_union_my_integer_get(int unit, int *my_integer)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        my_integer);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *my_integer = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->my_union.my_integer;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->my_union.my_integer,
        "example[%d]->my_union.my_integer",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_MY_UNION_MY_INTEGER_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_my_union_my_char_set(int unit, char my_char)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->my_union.my_char,
        my_char,
        char,
        0,
        "example_my_union_my_char_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &my_char,
        "example[%d]->my_union.my_char",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_MY_UNION_MY_CHAR_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_my_union_my_char_get(int unit, char *my_char)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        my_char);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *my_char = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->my_union.my_char;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->my_union.my_char,
        "example[%d]->my_union.my_char",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_MY_UNION_MY_CHAR_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_string_stringncat(int unit, char *src)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_STRINGNCAT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_STRING_STRINGNCAT(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->string,
        src,
        100);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_STRING_STRINGNCAT_LOGGING,
        BSL_LS_SWSTATEDNX_STRINGSTRINGNCAT,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->string,
        "example[%d]->string",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_STRING_INFO,
        DNX_SW_STATE_DIAG_STRING,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_string_stringncmp(int unit, char *cmp_string, int *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_STRINGNCMP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_STRING_STRINGNCMP(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->string,
        cmp_string,
        result,
        100);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_STRING_STRINGNCMP_LOGGING,
        BSL_LS_SWSTATEDNX_STRINGSTRINGNCMP,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->string,
        "example[%d]->string",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_STRING_INFO,
        DNX_SW_STATE_DIAG_STRING,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_string_stringncpy(int unit, char *src)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_STRINGNCPY,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_STRING_STRINGNCPY(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->string,
        src,
        100);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_STRING_STRINGNCPY_LOGGING,
        BSL_LS_SWSTATEDNX_STRINGSTRINGNCPY,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->string,
        "example[%d]->string",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_STRING_INFO,
        DNX_SW_STATE_DIAG_STRING,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_string_stringlen(int unit, uint32 *size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_STRINGLEN,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_STRING_STRINGLEN(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->string,
        size);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_STRING_STRINGLEN_LOGGING,
        BSL_LS_SWSTATEDNX_STRINGSTRINGLEN,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->string,
        "example[%d]->string",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_STRING_INFO,
        DNX_SW_STATE_DIAG_STRING,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_string_stringget(int unit, char *o_string)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_STRINGGET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_STRING_STRINGGET(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->string,
        o_string);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_STRING_STRINGGET_LOGGING,
        BSL_LS_SWSTATEDNX_STRINGSTRINGGET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->string,
        "example[%d]->string",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_STRING_INFO,
        DNX_SW_STATE_DIAG_STRING,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_alloc_child_check_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SWSTATE_ALLOC_SIZE_VERIFY(
        unit,
        sizeof(int*),
        10000,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNX_SWSTATE_ALLOC_SIZE_VERIFY(
        unit,
        sizeof(int),
        20000,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check,
        sw_state_example_child_size_check_t,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_alloc_child_check_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check,
        "example[%d]->alloc_child_check",
        unit,
        nof_instances_to_alloc_0 * sizeof(sw_state_example_child_size_check_t) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_ALLOC_CHILD_CHECK_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_alloc_child_check_static_array_of_p_set(int unit, uint32 alloc_child_check_idx_0, uint32 static_array_of_p_idx_0, uint32 static_array_of_p_idx_1, int static_array_of_p)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check,
        alloc_child_check_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[alloc_child_check_idx_0].static_array_of_p[static_array_of_p_idx_0]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        static_array_of_p_idx_0,
        10000);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[alloc_child_check_idx_0].static_array_of_p[static_array_of_p_idx_0],
        static_array_of_p_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[alloc_child_check_idx_0].static_array_of_p[static_array_of_p_idx_0][static_array_of_p_idx_1],
        static_array_of_p,
        int,
        0,
        "example_alloc_child_check_static_array_of_p_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &static_array_of_p,
        "example[%d]->alloc_child_check[%d].static_array_of_p[%d][%d]",
        unit, alloc_child_check_idx_0, static_array_of_p_idx_0, static_array_of_p_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_ALLOC_CHILD_CHECK_STATIC_ARRAY_OF_P_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_alloc_child_check_static_array_of_p_get(int unit, uint32 alloc_child_check_idx_0, uint32 static_array_of_p_idx_0, uint32 static_array_of_p_idx_1, int *static_array_of_p)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check,
        alloc_child_check_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        static_array_of_p);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        static_array_of_p_idx_0,
        10000);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[alloc_child_check_idx_0].static_array_of_p[static_array_of_p_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[alloc_child_check_idx_0].static_array_of_p[static_array_of_p_idx_0],
        static_array_of_p_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *static_array_of_p = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[alloc_child_check_idx_0].static_array_of_p[static_array_of_p_idx_0][static_array_of_p_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[alloc_child_check_idx_0].static_array_of_p[static_array_of_p_idx_0][static_array_of_p_idx_1],
        "example[%d]->alloc_child_check[%d].static_array_of_p[%d][%d]",
        unit, alloc_child_check_idx_0, static_array_of_p_idx_0, static_array_of_p_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_ALLOC_CHILD_CHECK_STATIC_ARRAY_OF_P_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_alloc_child_check_static_array_of_p_alloc(int unit, uint32 alloc_child_check_idx_0, uint32 static_array_of_p_idx_0, uint32 nof_instances_to_alloc_1)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check,
        alloc_child_check_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        static_array_of_p_idx_0,
        10000);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[alloc_child_check_idx_0].static_array_of_p[static_array_of_p_idx_0],
        int,
        nof_instances_to_alloc_1,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_alloc_child_check_static_array_of_p_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[alloc_child_check_idx_0].static_array_of_p[static_array_of_p_idx_0],
        "example[%d]->alloc_child_check[%d].static_array_of_p[%d]",
        unit, alloc_child_check_idx_0, static_array_of_p_idx_0,
        nof_instances_to_alloc_1 * sizeof(int) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[alloc_child_check_idx_0].static_array_of_p[static_array_of_p_idx_0]));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_ALLOC_CHILD_CHECK_STATIC_ARRAY_OF_P_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_alloc_child_check_static_array_set(int unit, uint32 alloc_child_check_idx_0, uint32 static_array_idx_0, int static_array)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check,
        alloc_child_check_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        static_array_idx_0,
        20000);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[alloc_child_check_idx_0].static_array[static_array_idx_0],
        static_array,
        int,
        0,
        "example_alloc_child_check_static_array_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &static_array,
        "example[%d]->alloc_child_check[%d].static_array[%d]",
        unit, alloc_child_check_idx_0, static_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_ALLOC_CHILD_CHECK_STATIC_ARRAY_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_alloc_child_check_static_array_get(int unit, uint32 alloc_child_check_idx_0, uint32 static_array_idx_0, int *static_array)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check,
        alloc_child_check_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        static_array);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        static_array_idx_0,
        20000);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *static_array = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[alloc_child_check_idx_0].static_array[static_array_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[alloc_child_check_idx_0].static_array[static_array_idx_0],
        "example[%d]->alloc_child_check[%d].static_array[%d]",
        unit, alloc_child_check_idx_0, static_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_ALLOC_CHILD_CHECK_STATIC_ARRAY_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_alloc_child_check_integer_variable_set(int unit, uint32 alloc_child_check_idx_0, int integer_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check,
        alloc_child_check_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[alloc_child_check_idx_0].integer_variable,
        integer_variable,
        int,
        0,
        "example_alloc_child_check_integer_variable_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &integer_variable,
        "example[%d]->alloc_child_check[%d].integer_variable",
        unit, alloc_child_check_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_ALLOC_CHILD_CHECK_INTEGER_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_alloc_child_check_integer_variable_get(int unit, uint32 alloc_child_check_idx_0, int *integer_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check,
        alloc_child_check_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        integer_variable);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *integer_variable = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[alloc_child_check_idx_0].integer_variable;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[alloc_child_check_idx_0].integer_variable,
        "example[%d]->alloc_child_check[%d].integer_variable",
        unit, alloc_child_check_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_ALLOC_CHILD_CHECK_INTEGER_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_write_during_wb_example_set(int unit, int write_during_wb_example)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_WB);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->write_during_wb_example,
        write_during_wb_example,
        int,
        0,
        "example_write_during_wb_example_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &write_during_wb_example,
        "example[%d]->write_during_wb_example",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_WRITE_DURING_WB_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_write_during_wb_example_get(int unit, int *write_during_wb_example)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_WB);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        write_during_wb_example);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *write_during_wb_example = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->write_during_wb_example;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->write_during_wb_example,
        "example[%d]->write_during_wb_example",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_WRITE_DURING_WB_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_large_DS_examples_large_htb_create(int unit, sw_state_htbl_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_CREATE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_htb,
        init_info,
        int,
        int,
        FALSE,
        dnx_data_module_testing.dbal.vrf_field_size_get(unit),
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCREATE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_htb,
        "example[%d]->large_DS_examples.large_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_large_DS_examples_large_htb_find(int unit, const int *key, int *value, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_FIND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_FIND(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_htb,
        key,
        value,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_FIND_LOGGING,
        BSL_LS_SWSTATEDNX_HTBFIND,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_htb,
        "example[%d]->large_DS_examples.large_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_large_DS_examples_large_htb_insert(int unit, const int *key, const int *value, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_INSERT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_INSERT(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_htb,
        key,
        value,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_INSERT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBINSERT,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_htb,
        "example[%d]->large_DS_examples.large_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_large_DS_examples_large_htb_get_next(int unit, SW_STATE_HASH_TABLE_ITER *iter, const int *key, const int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_GET_NEXT(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_htb,
        iter,
        key,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_GET_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBGET_NEXT,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_htb,
        "example[%d]->large_DS_examples.large_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_large_DS_examples_large_htb_clear(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_CLEAR(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_htb);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCLEAR,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_htb,
        "example[%d]->large_DS_examples.large_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_large_DS_examples_large_htb_delete(int unit, const int *key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_DELETE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_DELETE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_htb,
        key);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_DELETE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBDELETE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_htb,
        "example[%d]->large_DS_examples.large_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_large_DS_examples_large_htb_delete_all(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_DELETE_ALL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_DELETE_ALL(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_htb);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_DELETE_ALL_LOGGING,
        BSL_LS_SWSTATEDNX_HTBDELETE_ALL,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_htb,
        "example[%d]->large_DS_examples.large_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_large_DS_examples_large_index_htb_create(int unit, sw_state_htbl_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_CREATE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        init_info,
        int,
        uint32,
        TRUE,
        dnx_data_module_testing.dbal.vrf_field_size_get(unit),
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCREATE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        "example[%d]->large_DS_examples.large_index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_large_DS_examples_large_index_htb_find(int unit, const int *key, uint32 *data_index, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_FIND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_FIND(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        key,
        data_index,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_FIND_LOGGING,
        BSL_LS_SWSTATEDNX_HTBFIND,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        "example[%d]->large_DS_examples.large_index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_large_DS_examples_large_index_htb_insert(int unit, const int *key, const uint32 *data_index, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_INSERT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_INSERT(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        key,
        data_index,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_INSERT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBINSERT,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        "example[%d]->large_DS_examples.large_index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_large_DS_examples_large_index_htb_get_next(int unit, SW_STATE_HASH_TABLE_ITER *iter, const int *key, const uint32 *data_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_GET_NEXT(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        iter,
        key,
        data_index);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_GET_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBGET_NEXT,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        "example[%d]->large_DS_examples.large_index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_large_DS_examples_large_index_htb_clear(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_CLEAR(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCLEAR,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        "example[%d]->large_DS_examples.large_index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_large_DS_examples_large_index_htb_delete(int unit, const int *key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_DELETE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_DELETE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        key);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_DELETE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBDELETE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        "example[%d]->large_DS_examples.large_index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_large_DS_examples_large_index_htb_delete_all(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_DELETE_ALL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_DELETE_ALL(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_DELETE_ALL_LOGGING,
        BSL_LS_SWSTATEDNX_HTBDELETE_ALL,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        "example[%d]->large_DS_examples.large_index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_large_DS_examples_large_index_htb_insert_at_index(int unit, const int *key, uint32 data_idx, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_INSERT_AT_INDEX,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_INSERT_AT_INDEX(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        key,
        data_idx,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_INSERT_AT_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_HTBINSERT_AT_INDEX,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        "example[%d]->large_DS_examples.large_index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_large_DS_examples_large_index_htb_delete_by_index(int unit, uint32 data_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_DELETE_BY_INDEX,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_DELETE_BY_INDEX(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        data_index);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_DELETE_BY_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_HTBDELETE_BY_INDEX,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        "example[%d]->large_DS_examples.large_index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_large_DS_examples_large_index_htb_get_by_index(int unit, uint32 data_index, int *key, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET_BY_INDEX,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_HTB_GET_BY_INDEX(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        data_index,
        key,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_GET_BY_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_HTBGET_BY_INDEX,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        "example[%d]->large_DS_examples.large_index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_large_DS_examples_large_occ_create(int unit, sw_state_occ_bitmap_init_info_t * init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_OCC_BM_CREATE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_occ,
        init_info,
        dnx_data_module_testing.dbal.vrf_field_size_get(unit),
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMCREATE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_occ,
        "example[%d]->large_DS_examples.large_occ",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_OCC_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_large_DS_examples_large_occ_get_next(int unit, uint32 *place, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_OCC_BM_GET_NEXT(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_occ,
        place,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_GET_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMGET_NEXT,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_occ,
        "example[%d]->large_DS_examples.large_occ",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_OCC_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_large_DS_examples_large_occ_get_next_in_range(int unit, uint32 start, uint32 end, uint8 forward, uint8 val, uint32 *place, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET_NEXT_IN_RANGE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_OCC_BM_GET_NEXT_IN_RANGE(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_occ,
        start,
        end,
        forward,
        val,
        place,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_GET_NEXT_IN_RANGE_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMGET_NEXT_IN_RANGE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_occ,
        "example[%d]->large_DS_examples.large_occ",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_OCC_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_large_DS_examples_large_occ_status_set(int unit, uint32 place, uint8 occupied)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_STATUS_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_OCC_BM_STATUS_SET(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_occ,
        place,
        occupied);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_STATUS_SET_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMSTATUS_SET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_occ,
        "example[%d]->large_DS_examples.large_occ",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_OCC_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_large_DS_examples_large_occ_is_occupied(int unit, uint32 place, uint8 *occupied)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_IS_OCCUPIED,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_OCC_BM_IS_OCCUPIED(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_occ,
        place,
        occupied);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_IS_OCCUPIED_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMIS_OCCUPIED,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_occ,
        "example[%d]->large_DS_examples.large_occ",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_OCC_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_large_DS_examples_large_occ_alloc_next(int unit, uint32 *place, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ALLOC_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_OCC_BM_ALLOC_NEXT(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_occ,
        place,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_ALLOC_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMALLOC_NEXT,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_occ,
        "example[%d]->large_DS_examples.large_occ",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_OCC_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_large_DS_examples_large_occ_clear(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    SW_STATE_OCC_BM_CLEAR(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_occ);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMCLEAR,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_occ,
        "example[%d]->large_DS_examples.large_occ",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_OCC_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_pretty_print_example_set(int unit, dnx_sw_state_pretty_print_example_t pretty_print_example)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pretty_print_example,
        pretty_print_example,
        dnx_sw_state_pretty_print_example_t,
        0,
        "example_pretty_print_example_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &pretty_print_example,
        "example[%d]->pretty_print_example",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_PRETTY_PRINT_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_pretty_print_example_get(int unit, dnx_sw_state_pretty_print_example_t *pretty_print_example)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        pretty_print_example);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *pretty_print_example = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pretty_print_example;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pretty_print_example,
        "example[%d]->pretty_print_example",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_PRETTY_PRINT_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_buffer_array_set(int unit, uint32 buffer_array_idx_0, uint32 buffer_array_idx_1, DNX_SW_STATE_BUFF buffer_array)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array,
        buffer_array_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0],
        buffer_array_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0][buffer_array_idx_1],
        buffer_array,
        DNX_SW_STATE_BUFF,
        0,
        "example_buffer_array_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EXAMPLE_MODULE_ID,
        &buffer_array,
        "example[%d]->buffer_array[%d][%d]",
        unit, buffer_array_idx_0, buffer_array_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BUFFER_ARRAY_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_buffer_array_get(int unit, uint32 buffer_array_idx_0, uint32 buffer_array_idx_1, DNX_SW_STATE_BUFF *buffer_array)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array,
        buffer_array_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        buffer_array);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0],
        buffer_array_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    *buffer_array = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0][buffer_array_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0][buffer_array_idx_1],
        "example[%d]->buffer_array[%d][%d]",
        unit, buffer_array_idx_0, buffer_array_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BUFFER_ARRAY_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_buffer_array_alloc(int unit, uint32 nof_instances_to_alloc_0, uint32 _nof_bytes_to_alloc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array,
        DNX_SW_STATE_BUFF*,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_buffer_array_alloc");

    for(uint32 buffer_array_idx_0 = 0;
         buffer_array_idx_0 < nof_instances_to_alloc_0;
         buffer_array_idx_0++)

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0],
        DNX_SW_STATE_BUFF,
        _nof_bytes_to_alloc,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_buffer_array_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array,
        "example[%d]->buffer_array",
        unit,
        _nof_bytes_to_alloc * sizeof(DNX_SW_STATE_BUFF) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array)+(_nof_bytes_to_alloc * sizeof(DNX_SW_STATE_BUFF)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BUFFER_ARRAY_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_buffer_array_memread(int unit, uint32 buffer_array_idx_0, uint8 *_dst, uint32 _offset, size_t _len)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_MEMREAD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array,
        (_offset + _len - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0],
        (_offset + _len - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_MEMREAD(
        unit,
        _dst,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0],
        _offset,
        _len,
        0,
        "example_buffer_array_BUFFER");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MEMREAD_LOGGING,
        BSL_LS_SWSTATEDNX_MEMREAD,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0],
        "example[%d]->buffer_array[%d]",
        unit, buffer_array_idx_0, _len);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BUFFER_ARRAY_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_buffer_array_memwrite(int unit, uint32 buffer_array_idx_0, const uint8 *_src, uint32 _offset, size_t _len)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_MEMWRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array,
        (_offset + _len - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0],
        (_offset + _len - 1));

    DNX_SW_STATE_MEMWRITE(
        unit,
        EXAMPLE_MODULE_ID,
        _src,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0],
        _offset,
        _len,
        0,
        "example_buffer_array_BUFFER");

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_buffer_array_memcmp(int unit, uint32 buffer_array_idx_0, const uint8 *_buff, uint32 _offset, size_t _len, int *cmp_result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_MEMCMP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array,
        (_offset + _len - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0],
        (_offset + _len - 1));

    DNX_SW_STATE_MEMCMP(
        unit,
        _buff,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0],
        _offset,
        _len,
        cmp_result,
        0,
        "example_buffer_array_BUFFER");

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_buffer_array_memset(int unit, uint32 buffer_array_idx_0, uint32 _offset, size_t _len, int _value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_MEMSET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array,
        (_offset + _len - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0],
        (_offset + _len - 1));

    DNX_SW_STATE_MEMSET(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0],
        _offset,
        _value,
        _len,
        0,
        "example_buffer_array_BUFFER");

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_array_alloc_bitmap(int unit, uint32 bitmap_array_idx_0, uint32 _nof_bits_to_alloc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ALLOC_BITMAP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        bitmap_array_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_ALLOC_BITMAP(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        _nof_bits_to_alloc,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_bitmap_array_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_BITMAP_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOCBITMAP,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        "example[%d]->bitmap_array[%d]",
        unit, bitmap_array_idx_0,
        _nof_bits_to_alloc);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_array_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        SHR_BITDCL*,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_bitmap_array_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        "example[%d]->bitmap_array",
        unit,
        nof_instances_to_alloc_0 * sizeof(SHR_BITDCL));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_array_bit_set(int unit, uint32 bitmap_array_idx_0, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        _bit_num);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_SET(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITSET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        "example[%d]->bitmap_array[%d]",
        unit, bitmap_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_array_bit_clear(int unit, uint32 bitmap_array_idx_0, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        _bit_num);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_CLEAR(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITCLEAR,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        "example[%d]->bitmap_array[%d]",
        unit, bitmap_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_array_bit_get(int unit, uint32 bitmap_array_idx_0, uint32 _bit_num, uint8* result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        _bit_num);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_GET(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
         _bit_num,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_GET_LOGGING,
        BSL_LS_SWSTATEDNX_BITGET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        "example[%d]->bitmap_array[%d]",
        unit, bitmap_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_array_bit_range_read(int unit, uint32 bitmap_array_idx_0, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_READ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_READ(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
         sw_state_bmp_first,
         result_first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_READ_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREAD,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        "example[%d]->bitmap_array[%d]",
        unit, bitmap_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_array_bit_range_write(int unit, uint32 bitmap_array_idx_0, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_WRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_WRITE(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
         sw_state_bmp_first,
         input_bmp_first,
          _range,
         input_bmp);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_WRITE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEWRITE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        "example[%d]->bitmap_array[%d]",
        unit, bitmap_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_array_bit_range_and(int unit, uint32 bitmap_array_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_AND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_AND(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_AND_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEAND,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        "example[%d]->bitmap_array[%d]",
        unit, bitmap_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_array_bit_range_or(int unit, uint32 bitmap_array_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_OR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_OR(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_OR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEOR,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        "example[%d]->bitmap_array[%d]",
        unit, bitmap_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_array_bit_range_xor(int unit, uint32 bitmap_array_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_XOR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_XOR(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_XOR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEXOR,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        "example[%d]->bitmap_array[%d]",
        unit, bitmap_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_array_bit_range_remove(int unit, uint32 bitmap_array_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_REMOVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_REMOVE(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_REMOVE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREMOVE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        "example[%d]->bitmap_array[%d]",
        unit, bitmap_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_array_bit_range_negate(int unit, uint32 bitmap_array_idx_0, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_NEGATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_NEGATE(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NEGATE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENEGATE,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        "example[%d]->bitmap_array[%d]",
        unit, bitmap_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_array_bit_range_clear(int unit, uint32 bitmap_array_idx_0, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_CLEAR(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECLEAR,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        "example[%d]->bitmap_array[%d]",
        unit, bitmap_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_array_bit_range_set(int unit, uint32 bitmap_array_idx_0, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_SET(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGESET,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        "example[%d]->bitmap_array[%d]",
        unit, bitmap_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_array_bit_range_null(int unit, uint32 bitmap_array_idx_0, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_NULL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_NULL(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NULL_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENULL,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        "example[%d]->bitmap_array[%d]",
        unit, bitmap_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_array_bit_range_test(int unit, uint32 bitmap_array_idx_0, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_TEST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_TEST(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_TEST_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGETEST,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        "example[%d]->bitmap_array[%d]",
        unit, bitmap_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_array_bit_range_eq(int unit, uint32 bitmap_array_idx_0, SHR_BITDCL *input_bmp, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_EQ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_EQ(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
         _first,
         _count,
         input_bmp,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_EQ_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEEQ,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        "example[%d]->bitmap_array[%d]",
        unit, bitmap_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
example_bitmap_array_bit_range_count(int unit, uint32 bitmap_array_idx_0, uint32 _first, uint32 _range, int *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EXAMPLE_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_COUNT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EXAMPLE_MODULE_ID,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        (_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        EXAMPLE_MODULE_ID,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        (_first + _range - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EXAMPLE_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_COUNT(
        unit,
         EXAMPLE_MODULE_ID,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
         _first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_COUNT_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECOUNT,
        EXAMPLE_MODULE_ID,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        "example[%d]->bitmap_array[%d]",
        unit, bitmap_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EXAMPLE_MODULE_ID,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




const char *
dnx_example_name_e_get_name(dnx_example_name_e value)
{
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_EXAMPLE_ENUM_NEGATIVE", value, DNX_EXAMPLE_ENUM_NEGATIVE);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_EXAMPLE_ENUM_ZERO", value, DNX_EXAMPLE_ENUM_ZERO);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_EXAMPLE_ENUM_ONE", value, DNX_EXAMPLE_ENUM_ONE);

    return NULL;
}




shr_error_e
dnx_sw_state_callback_test_function_cb_get_cb(sw_state_cb_t * cb_db, uint8 dryRun, dnx_sw_state_callback_test_function_cb * cb)
{
    if (!strcmp(cb_db->function_name,"dnx_sw_state_callback_test_first_function"))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = dnx_sw_state_callback_test_first_function;
        }
        return _SHR_E_NONE;
    }
    if (!strcmp(cb_db->function_name,"dnx_sw_state_callback_test_second_function"))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = dnx_sw_state_callback_test_second_function;
        }
        return _SHR_E_NONE;
    }

    return _SHR_E_NOT_FOUND;
}





example_cbs example = 	{
	
	example_is_init,
	example_init,
	example_deinit,
		{
		
		example_my_variable_set,
		example_my_variable_get}
	,
		{
		
		example_rollback_comparison_excluded_set,
		example_rollback_comparison_excluded_get}
	,
		{
		
		example_my_array_set,
		example_my_array_get}
	,
		{
		
		example_two_dimentional_array_set,
		example_two_dimentional_array_get}
	,
		{
		
		example_dynamic_array_set,
		example_dynamic_array_get,
		example_dynamic_array_alloc}
	,
		{
		
		example_dynamic_array_zero_size_set,
		example_dynamic_array_zero_size_get,
		example_dynamic_array_zero_size_alloc,
		example_dynamic_array_zero_size_free}
	,
		{
		
		example_dynamic_dynamic_array_set,
		example_dynamic_dynamic_array_get,
		example_dynamic_dynamic_array_alloc}
	,
		{
		
		example_dynamic_array_static_set,
		example_dynamic_array_static_get,
		example_dynamic_array_static_alloc}
	,
		{
		
		example_counter_test_set,
		example_counter_test_get,
		example_counter_test_inc,
		example_counter_test_dec}
	,
		{
		
		example_mutex_test_create,
		example_mutex_test_is_created,
		example_mutex_test_destroy,
		example_mutex_test_take,
		example_mutex_test_give}
	,
		{
		
		example_sem_test_create,
		example_sem_test_is_created,
		example_sem_test_destroy,
		example_sem_test_take,
		example_sem_test_give}
	,
		{
		
		example_DNXData_array_alloc_exception_set,
		example_DNXData_array_alloc_exception_get,
		example_DNXData_array_alloc_exception_alloc}
	,
		{
		
		example_DNXData_array_set,
		example_DNXData_array_get,
		example_DNXData_array_alloc}
	,
		{
		
		example_DNXData_array_dyn_dnxdata_set,
		example_DNXData_array_dyn_dnxdata_get,
		example_DNXData_array_dyn_dnxdata_alloc}
	,
		{
		
		example_DNXData_array_dnxdata_dyn_set,
		example_DNXData_array_dnxdata_dyn_get,
		example_DNXData_array_dnxdata_dyn_alloc}
	,
		{
		
		example_DNXData_array_dnxdata_dnxdata_set,
		example_DNXData_array_dnxdata_dnxdata_get,
		example_DNXData_array_dnxdata_dnxdata_alloc}
	,
		{
		
		example_DNXData_array_static_dnxdata_dyn_set,
		example_DNXData_array_static_dnxdata_dyn_get,
		example_DNXData_array_static_dnxdata_dyn_alloc}
	,
		{
		
		example_DNXData_array_static_dyn_dnxdata_set,
		example_DNXData_array_static_dyn_dnxdata_get,
		example_DNXData_array_static_dyn_dnxdata_alloc}
	,
		{
		
		example_DNXData_array_static_dnxdata_dnxdata_set,
		example_DNXData_array_static_dnxdata_dnxdata_get,
		example_DNXData_array_static_dnxdata_dnxdata_alloc}
	,
		{
		
		example_DNXData_array_table_data_set,
		example_DNXData_array_table_data_get,
		example_DNXData_array_table_data_alloc}
	,
		{
		
		example_value_range_test_set,
		example_value_range_test_get}
	,
		{
		
		example_array_range_example_set,
		example_array_range_example_get,
		example_array_range_example_range_read,
		example_array_range_example_range_write,
		example_array_range_example_range_fill}
	,
		{
		
		example_alloc_after_init_variable_set,
		example_alloc_after_init_variable_get,
		example_alloc_after_init_variable_alloc}
	,
		{
		
		example_alloc_after_init_test_set,
		example_alloc_after_init_test_get,
		example_alloc_after_init_test_alloc}
	,
		{
		
		example_pointer_set,
		example_pointer_get,
		example_pointer_alloc,
		example_pointer_is_allocated}
	,
		{
		
		example_bitmap_variable_alloc_bitmap,
		example_bitmap_variable_is_allocated,
		example_bitmap_variable_free,
		example_bitmap_variable_bit_set,
		example_bitmap_variable_bit_clear,
		example_bitmap_variable_bit_get,
		example_bitmap_variable_bit_range_read,
		example_bitmap_variable_bit_range_write,
		example_bitmap_variable_bit_range_and,
		example_bitmap_variable_bit_range_or,
		example_bitmap_variable_bit_range_xor,
		example_bitmap_variable_bit_range_remove,
		example_bitmap_variable_bit_range_negate,
		example_bitmap_variable_bit_range_clear,
		example_bitmap_variable_bit_range_set,
		example_bitmap_variable_bit_range_null,
		example_bitmap_variable_bit_range_test,
		example_bitmap_variable_bit_range_eq,
		example_bitmap_variable_bit_range_count}
	,
		{
		
		example_pbmp_variable_set,
		example_pbmp_variable_get,
		example_pbmp_variable_pbmp_neq,
		example_pbmp_variable_pbmp_eq,
		example_pbmp_variable_pbmp_member,
		example_pbmp_variable_pbmp_not_null,
		example_pbmp_variable_pbmp_is_null,
		example_pbmp_variable_pbmp_count,
		example_pbmp_variable_pbmp_xor,
		example_pbmp_variable_pbmp_remove,
		example_pbmp_variable_pbmp_assign,
		example_pbmp_variable_pbmp_get,
		example_pbmp_variable_pbmp_and,
		example_pbmp_variable_pbmp_negate,
		example_pbmp_variable_pbmp_or,
		example_pbmp_variable_pbmp_clear,
		example_pbmp_variable_pbmp_port_add,
		example_pbmp_variable_pbmp_port_flip,
		example_pbmp_variable_pbmp_port_remove,
		example_pbmp_variable_pbmp_port_set,
		example_pbmp_variable_pbmp_ports_range_add,
		example_pbmp_variable_pbmp_fmt}
	,
		{
		
		example_bitmap_fixed_bit_set,
		example_bitmap_fixed_bit_clear,
		example_bitmap_fixed_bit_get,
		example_bitmap_fixed_bit_range_read,
		example_bitmap_fixed_bit_range_write,
		example_bitmap_fixed_bit_range_and,
		example_bitmap_fixed_bit_range_or,
		example_bitmap_fixed_bit_range_xor,
		example_bitmap_fixed_bit_range_remove,
		example_bitmap_fixed_bit_range_negate,
		example_bitmap_fixed_bit_range_clear,
		example_bitmap_fixed_bit_range_set,
		example_bitmap_fixed_bit_range_null,
		example_bitmap_fixed_bit_range_test,
		example_bitmap_fixed_bit_range_eq,
		example_bitmap_fixed_bit_range_count}
	,
		{
		
		example_bitmap_dnx_data_alloc_bitmap,
		example_bitmap_dnx_data_is_allocated,
		example_bitmap_dnx_data_bit_set,
		example_bitmap_dnx_data_bit_clear,
		example_bitmap_dnx_data_bit_get,
		example_bitmap_dnx_data_bit_range_read,
		example_bitmap_dnx_data_bit_range_write,
		example_bitmap_dnx_data_bit_range_and,
		example_bitmap_dnx_data_bit_range_or,
		example_bitmap_dnx_data_bit_range_xor,
		example_bitmap_dnx_data_bit_range_remove,
		example_bitmap_dnx_data_bit_range_negate,
		example_bitmap_dnx_data_bit_range_clear,
		example_bitmap_dnx_data_bit_range_set,
		example_bitmap_dnx_data_bit_range_null,
		example_bitmap_dnx_data_bit_range_test,
		example_bitmap_dnx_data_bit_range_eq,
		example_bitmap_dnx_data_bit_range_count}
	,
		{
		
		example_buffer_set,
		example_buffer_get,
		example_buffer_alloc,
		example_buffer_is_allocated,
		example_buffer_memread,
		example_buffer_memwrite,
		example_buffer_memcmp,
		example_buffer_memset}
	,
		{
		
		example_buffer_fixed_set,
		example_buffer_fixed_get,
		example_buffer_fixed_memread,
		example_buffer_fixed_memwrite,
		example_buffer_fixed_memcmp,
		example_buffer_fixed_memset}
	,
		{
		
		example_bitmap_exclude_example_alloc_bitmap,
		example_bitmap_exclude_example_bit_set,
		example_bitmap_exclude_example_bit_get,
		example_bitmap_exclude_example_bit_range_read,
		example_bitmap_exclude_example_bit_range_write,
		example_bitmap_exclude_example_bit_range_and,
		example_bitmap_exclude_example_bit_range_or,
		example_bitmap_exclude_example_bit_range_xor,
		example_bitmap_exclude_example_bit_range_remove,
		example_bitmap_exclude_example_bit_range_clear,
		example_bitmap_exclude_example_bit_range_set,
		example_bitmap_exclude_example_bit_range_eq,
		example_bitmap_exclude_example_bit_range_count}
	,
		{
		
			{
			
			example_default_value_tree_array_dnxdata_alloc,
			example_default_value_tree_array_dnxdata_free,
				{
				
				example_default_value_tree_array_dnxdata_array_dynamic_alloc,
					{
					
						{
						
						example_default_value_tree_array_dnxdata_array_dynamic_array_static_static_my_variable_set,
						example_default_value_tree_array_dnxdata_array_dynamic_array_static_static_my_variable_get}
					}
				,
					{
					
						{
						
						example_default_value_tree_array_dnxdata_array_dynamic_default_value_l3_my_variable_set,
						example_default_value_tree_array_dnxdata_array_dynamic_default_value_l3_my_variable_get}
					}
				,
					{
					
					example_default_value_tree_array_dnxdata_array_dynamic_pointer_set,
					example_default_value_tree_array_dnxdata_array_dynamic_pointer_get,
					example_default_value_tree_array_dnxdata_array_dynamic_pointer_alloc}
				,
					{
					
					example_default_value_tree_array_dnxdata_array_dynamic_array_dynamic_dynamic_alloc,
						{
						
						example_default_value_tree_array_dnxdata_array_dynamic_array_dynamic_dynamic_my_variable_set,
						example_default_value_tree_array_dnxdata_array_dynamic_array_dynamic_dynamic_my_variable_get}
					}
				}
			,
				{
				
				example_default_value_tree_array_dnxdata_array_pointer_alloc,
					{
					
						{
						
						example_default_value_tree_array_dnxdata_array_pointer_array_static_static_my_variable_set,
						example_default_value_tree_array_dnxdata_array_pointer_array_static_static_my_variable_get}
					}
				,
					{
					
						{
						
						example_default_value_tree_array_dnxdata_array_pointer_default_value_l3_my_variable_set,
						example_default_value_tree_array_dnxdata_array_pointer_default_value_l3_my_variable_get}
					}
				,
					{
					
					example_default_value_tree_array_dnxdata_array_pointer_pointer_set,
					example_default_value_tree_array_dnxdata_array_pointer_pointer_get,
					example_default_value_tree_array_dnxdata_array_pointer_pointer_alloc}
				,
					{
					
					example_default_value_tree_array_dnxdata_array_pointer_array_dynamic_dynamic_alloc,
						{
						
						example_default_value_tree_array_dnxdata_array_pointer_array_dynamic_dynamic_my_variable_set,
						example_default_value_tree_array_dnxdata_array_pointer_array_dynamic_dynamic_my_variable_get}
					}
				}
			,
				{
				
					{
					
					example_default_value_tree_array_dnxdata_default_value_l2_my_variable_set,
					example_default_value_tree_array_dnxdata_default_value_l2_my_variable_get}
				}
			}
		,
			{
			
				{
				
				example_default_value_tree_default_value_l1_my_variable_set,
				example_default_value_tree_default_value_l1_my_variable_get}
			}
		}
	,
		{
		
		example_ll_create_empty,
		example_ll_nof_elements,
		example_ll_node_value,
		example_ll_node_update,
		example_ll_next_node,
		example_ll_previous_node,
		example_ll_add_first,
		example_ll_add_last,
		example_ll_add_before,
		example_ll_add_after,
		example_ll_remove_node,
		example_ll_get_last,
		example_ll_get_first,
		example_ll_print}
	,
		{
		
		example_multihead_ll_create_empty,
		example_multihead_ll_nof_elements,
		example_multihead_ll_node_value,
		example_multihead_ll_node_update,
		example_multihead_ll_next_node,
		example_multihead_ll_previous_node,
		example_multihead_ll_add_first,
		example_multihead_ll_add_last,
		example_multihead_ll_add_before,
		example_multihead_ll_add_after,
		example_multihead_ll_remove_node,
		example_multihead_ll_get_last,
		example_multihead_ll_get_first,
		example_multihead_ll_print}
	,
		{
		
		example_sorted_ll_create_empty,
		example_sorted_ll_nof_elements,
		example_sorted_ll_node_value,
		example_sorted_ll_node_update,
		example_sorted_ll_next_node,
		example_sorted_ll_previous_node,
		example_sorted_ll_add_first,
		example_sorted_ll_add_last,
		example_sorted_ll_add_before,
		example_sorted_ll_add_after,
		example_sorted_ll_remove_node,
		example_sorted_ll_get_last,
		example_sorted_ll_get_first,
		example_sorted_ll_print,
		example_sorted_ll_add,
		example_sorted_ll_node_key,
		example_sorted_ll_find}
	,
		{
		
		example_bt_create_empty,
		example_bt_destroy,
		example_bt_nof_elements,
		example_bt_node_value,
		example_bt_node_update,
		example_bt_node_free,
		example_bt_get_root,
		example_bt_get_parent,
		example_bt_get_left_child,
		example_bt_get_right_child,
		example_bt_add_root,
		example_bt_add_left_child,
		example_bt_add_right_child,
		example_bt_set_parent,
		example_bt_set_left_child,
		example_bt_set_right_child,
		example_bt_print}
	,
		{
		
		example_htb_create,
		example_htb_find,
		example_htb_insert,
		example_htb_get_next,
		example_htb_clear,
		example_htb_delete,
		example_htb_delete_all,
		example_htb_print}
	,
		{
		
		example_htb_arr_create,
		example_htb_arr_find,
		example_htb_arr_insert,
		example_htb_arr_get_next,
		example_htb_arr_clear,
		example_htb_arr_delete,
		example_htb_arr_delete_all}
	,
		{
		
		example_index_htb_create,
		example_index_htb_find,
		example_index_htb_insert,
		example_index_htb_get_next,
		example_index_htb_clear,
		example_index_htb_delete,
		example_index_htb_delete_all,
		example_index_htb_print,
		example_index_htb_insert_at_index,
		example_index_htb_delete_by_index,
		example_index_htb_get_by_index}
	,
		{
		
		example_occ_create,
		example_occ_get_next,
		example_occ_get_next_in_range,
		example_occ_status_set,
		example_occ_is_occupied,
		example_occ_alloc_next,
		example_occ_clear,
		example_occ_print}
	,
		{
		
		example_cb_get_cb,
		example_cb_register_cb,
		example_cb_unregister_cb}
	,
		{
		
		example_leaf_struct_set,
		example_leaf_struct_get}
	,
		{
		
		example_defragmented_chunk_example_create,
		example_defragmented_chunk_example_piece_alloc,
		example_defragmented_chunk_example_piece_free,
		example_defragmented_chunk_example_print}
	,
		{
		
			{
			
			example_packed_expecto_patronum_set,
			example_packed_expecto_patronum_get}
		,
			{
			
			example_packed_my_integer_set,
			example_packed_my_integer_get}
		}
	,
		{
		
			{
			
			example_my_union_my_integer_set,
			example_my_union_my_integer_get}
		,
			{
			
			example_my_union_my_char_set,
			example_my_union_my_char_get}
		}
	,
		{
		
		example_string_stringncat,
		example_string_stringncmp,
		example_string_stringncpy,
		example_string_stringlen,
		example_string_stringget}
	,
		{
		
		example_alloc_child_check_alloc,
			{
			
			example_alloc_child_check_static_array_of_p_set,
			example_alloc_child_check_static_array_of_p_get,
			example_alloc_child_check_static_array_of_p_alloc}
		,
			{
			
			example_alloc_child_check_static_array_set,
			example_alloc_child_check_static_array_get}
		,
			{
			
			example_alloc_child_check_integer_variable_set,
			example_alloc_child_check_integer_variable_get}
		}
	,
		{
		
		example_write_during_wb_example_set,
		example_write_during_wb_example_get}
	,
		{
		
			{
			
			example_large_DS_examples_large_htb_create,
			example_large_DS_examples_large_htb_find,
			example_large_DS_examples_large_htb_insert,
			example_large_DS_examples_large_htb_get_next,
			example_large_DS_examples_large_htb_clear,
			example_large_DS_examples_large_htb_delete,
			example_large_DS_examples_large_htb_delete_all}
		,
			{
			
			example_large_DS_examples_large_index_htb_create,
			example_large_DS_examples_large_index_htb_find,
			example_large_DS_examples_large_index_htb_insert,
			example_large_DS_examples_large_index_htb_get_next,
			example_large_DS_examples_large_index_htb_clear,
			example_large_DS_examples_large_index_htb_delete,
			example_large_DS_examples_large_index_htb_delete_all,
			example_large_DS_examples_large_index_htb_insert_at_index,
			example_large_DS_examples_large_index_htb_delete_by_index,
			example_large_DS_examples_large_index_htb_get_by_index}
		,
			{
			
			example_large_DS_examples_large_occ_create,
			example_large_DS_examples_large_occ_get_next,
			example_large_DS_examples_large_occ_get_next_in_range,
			example_large_DS_examples_large_occ_status_set,
			example_large_DS_examples_large_occ_is_occupied,
			example_large_DS_examples_large_occ_alloc_next,
			example_large_DS_examples_large_occ_clear}
		}
	,
		{
		
		example_pretty_print_example_set,
		example_pretty_print_example_get}
	,
		{
		
		example_buffer_array_set,
		example_buffer_array_get,
		example_buffer_array_alloc,
		example_buffer_array_memread,
		example_buffer_array_memwrite,
		example_buffer_array_memcmp,
		example_buffer_array_memset}
	,
		{
		
		example_bitmap_array_alloc_bitmap,
		example_bitmap_array_alloc,
		example_bitmap_array_bit_set,
		example_bitmap_array_bit_clear,
		example_bitmap_array_bit_get,
		example_bitmap_array_bit_range_read,
		example_bitmap_array_bit_range_write,
		example_bitmap_array_bit_range_and,
		example_bitmap_array_bit_range_or,
		example_bitmap_array_bit_range_xor,
		example_bitmap_array_bit_range_remove,
		example_bitmap_array_bit_range_negate,
		example_bitmap_array_bit_range_clear,
		example_bitmap_array_bit_range_set,
		example_bitmap_array_bit_range_null,
		example_bitmap_array_bit_range_test,
		example_bitmap_array_bit_range_eq,
		example_bitmap_array_bit_range_count}
	}
;
#endif  
#undef BSL_LOG_MODULE
