
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
#include <soc/dnx/swstate/auto_generated/access/example_access.h>
#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/dnx/dnx_data/auto_generated/dnx_data_bfd.h>
#endif  
#include <soc/dnx/dnx_data/auto_generated/dnx_data_infra_tests.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_module_testing.h>
#include <soc/dnxc/swstate/dnx_sw_state_pretty_dump.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/example_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/example_layout.h>





int
example_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_SW_STATE_EXAMPLE,
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
        DNX_SW_STATE_EXAMPLE,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_SW_STATE_EXAMPLE,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        sw_state_example,
        DNX_SW_STATE_EXAMPLE_NOF_PARAMS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_init",
        DNX_SW_STATE_DIAG_ALLOC,
        example_init_layout_structure);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_SW_STATE_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]),
        "example[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE,
        example_info,
        EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]),
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

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
        DNX_SW_STATE_EXAMPLE,
        SW_STATE_FUNC_DEINIT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE,
        DNX_SW_STATE_DIAG_FREE);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE,
        example_info,
        EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_FREE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID]),
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_MODULE_DEINIT(
        unit,
        DNX_SW_STATE_EXAMPLE,
        0,
        "example_deinit");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_DEINIT_LOGGING,
        BSL_LS_SWSTATEDNX_DEINIT,
        DNX_SW_STATE_EXAMPLE,
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
        DNX_SW_STATE_EXAMPLE__MY_VARIABLE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_VARIABLE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->my_variable,
        my_variable,
        uint32,
        0,
        "example_my_variable_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__MY_VARIABLE,
        &my_variable,
        "example[%d]->my_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_VARIABLE,
        example_info,
        EXAMPLE_MY_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_my_variable_get(int unit, uint32 *my_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_VARIABLE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_VARIABLE,
        my_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_VARIABLE,
        DNX_SW_STATE_DIAG_READ);

    *my_variable = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->my_variable;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__MY_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->my_variable,
        "example[%d]->my_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_VARIABLE,
        example_info,
        EXAMPLE_MY_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_rollback_comparison_excluded_set(int unit, uint32 rollback_comparison_excluded)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__ROLLBACK_COMPARISON_EXCLUDED,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__ROLLBACK_COMPARISON_EXCLUDED,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__ROLLBACK_COMPARISON_EXCLUDED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__ROLLBACK_COMPARISON_EXCLUDED,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->rollback_comparison_excluded,
        rollback_comparison_excluded,
        uint32,
        0,
        "example_rollback_comparison_excluded_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__ROLLBACK_COMPARISON_EXCLUDED,
        &rollback_comparison_excluded,
        "example[%d]->rollback_comparison_excluded",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__ROLLBACK_COMPARISON_EXCLUDED,
        example_info,
        EXAMPLE_ROLLBACK_COMPARISON_EXCLUDED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_rollback_comparison_excluded_get(int unit, uint32 *rollback_comparison_excluded)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__ROLLBACK_COMPARISON_EXCLUDED,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__ROLLBACK_COMPARISON_EXCLUDED,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__ROLLBACK_COMPARISON_EXCLUDED,
        rollback_comparison_excluded);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__ROLLBACK_COMPARISON_EXCLUDED,
        DNX_SW_STATE_DIAG_READ);

    *rollback_comparison_excluded = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->rollback_comparison_excluded;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__ROLLBACK_COMPARISON_EXCLUDED,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->rollback_comparison_excluded,
        "example[%d]->rollback_comparison_excluded",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__ROLLBACK_COMPARISON_EXCLUDED,
        example_info,
        EXAMPLE_ROLLBACK_COMPARISON_EXCLUDED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_my_array_set(int unit, uint32 my_array_idx_0, uint32 my_array)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_ARRAY,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        my_array_idx_0,
        10);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_ARRAY,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->my_array[my_array_idx_0],
        my_array,
        uint32,
        0,
        "example_my_array_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__MY_ARRAY,
        &my_array,
        "example[%d]->my_array[%d]",
        unit, my_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_ARRAY,
        example_info,
        EXAMPLE_MY_ARRAY_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_my_array_get(int unit, uint32 my_array_idx_0, uint32 *my_array)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_ARRAY,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        my_array_idx_0,
        10);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_ARRAY,
        my_array);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_ARRAY,
        DNX_SW_STATE_DIAG_READ);

    *my_array = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->my_array[my_array_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__MY_ARRAY,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->my_array[my_array_idx_0],
        "example[%d]->my_array[%d]",
        unit, my_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_ARRAY,
        example_info,
        EXAMPLE_MY_ARRAY_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_two_dimentional_array_set(int unit, uint32 index_1, uint32 index_2, uint32 two_dimentional_array)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__TWO_DIMENTIONAL_ARRAY,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__TWO_DIMENTIONAL_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        index_1,
        10);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        index_2,
        10);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__TWO_DIMENTIONAL_ARRAY,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__TWO_DIMENTIONAL_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->two_dimentional_array[index_1][index_2],
        two_dimentional_array,
        uint32,
        0,
        "example_two_dimentional_array_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__TWO_DIMENTIONAL_ARRAY,
        &two_dimentional_array,
        "example[%d]->two_dimentional_array[%d][%d]",
        unit, index_1, index_2);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__TWO_DIMENTIONAL_ARRAY,
        example_info,
        EXAMPLE_TWO_DIMENTIONAL_ARRAY_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_two_dimentional_array_get(int unit, uint32 index_1, uint32 index_2, uint32 *two_dimentional_array)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__TWO_DIMENTIONAL_ARRAY,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__TWO_DIMENTIONAL_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        index_1,
        10);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__TWO_DIMENTIONAL_ARRAY,
        two_dimentional_array);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        index_2,
        10);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__TWO_DIMENTIONAL_ARRAY,
        DNX_SW_STATE_DIAG_READ);

    *two_dimentional_array = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->two_dimentional_array[index_1][index_2];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__TWO_DIMENTIONAL_ARRAY,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->two_dimentional_array[index_1][index_2],
        "example[%d]->two_dimentional_array[%d][%d]",
        unit, index_1, index_2);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__TWO_DIMENTIONAL_ARRAY,
        example_info,
        EXAMPLE_TWO_DIMENTIONAL_ARRAY_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_dynamic_array_set(int unit, uint32 dynamic_array_idx_0, uint32 dynamic_array)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array,
        dynamic_array_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array[dynamic_array_idx_0],
        dynamic_array,
        uint32,
        0,
        "example_dynamic_array_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY,
        &dynamic_array,
        "example[%d]->dynamic_array[%d]",
        unit, dynamic_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY,
        example_info,
        EXAMPLE_DYNAMIC_ARRAY_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_dynamic_array_get(int unit, uint32 dynamic_array_idx_0, uint32 *dynamic_array)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array,
        dynamic_array_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY,
        dynamic_array);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY,
        DNX_SW_STATE_DIAG_READ);

    *dynamic_array = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array[dynamic_array_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array[dynamic_array_idx_0],
        "example[%d]->dynamic_array[%d]",
        unit, dynamic_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY,
        example_info,
        EXAMPLE_DYNAMIC_ARRAY_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_dynamic_array_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array,
        uint32,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_dynamic_array_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array,
        "example[%d]->dynamic_array",
        unit,
        nof_instances_to_alloc_0 * sizeof(uint32) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY,
        example_info,
        EXAMPLE_DYNAMIC_ARRAY_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_dynamic_array_zero_size_set(int unit, uint32 dynamic_array_zero_size_idx_0, uint32 dynamic_array_zero_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_ZERO_SIZE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_ZERO_SIZE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_ZERO_SIZE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_zero_size,
        dynamic_array_zero_size_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_ZERO_SIZE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_zero_size);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_ZERO_SIZE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_ZERO_SIZE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_zero_size[dynamic_array_zero_size_idx_0],
        dynamic_array_zero_size,
        uint32,
        0,
        "example_dynamic_array_zero_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_ZERO_SIZE,
        &dynamic_array_zero_size,
        "example[%d]->dynamic_array_zero_size[%d]",
        unit, dynamic_array_zero_size_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_ZERO_SIZE,
        example_info,
        EXAMPLE_DYNAMIC_ARRAY_ZERO_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_dynamic_array_zero_size_get(int unit, uint32 dynamic_array_zero_size_idx_0, uint32 *dynamic_array_zero_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_ZERO_SIZE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_ZERO_SIZE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_ZERO_SIZE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_zero_size,
        dynamic_array_zero_size_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_ZERO_SIZE,
        dynamic_array_zero_size);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_ZERO_SIZE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_zero_size);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_ZERO_SIZE,
        DNX_SW_STATE_DIAG_READ);

    *dynamic_array_zero_size = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_zero_size[dynamic_array_zero_size_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_ZERO_SIZE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_zero_size[dynamic_array_zero_size_idx_0],
        "example[%d]->dynamic_array_zero_size[%d]",
        unit, dynamic_array_zero_size_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_ZERO_SIZE,
        example_info,
        EXAMPLE_DYNAMIC_ARRAY_ZERO_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_dynamic_array_zero_size_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_ZERO_SIZE,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_ZERO_SIZE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_ZERO_SIZE,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_ZERO_SIZE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_zero_size,
        uint32,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_dynamic_array_zero_size_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_ZERO_SIZE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_zero_size,
        "example[%d]->dynamic_array_zero_size",
        unit,
        nof_instances_to_alloc_0 * sizeof(uint32) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_zero_size));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_ZERO_SIZE,
        example_info,
        EXAMPLE_DYNAMIC_ARRAY_ZERO_SIZE_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_dynamic_dynamic_array_set(int unit, uint32 first_d_index, uint32 dynamic_index, uint32 dynamic_dynamic_array)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_DYNAMIC_ARRAY,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_DYNAMIC_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DYNAMIC_DYNAMIC_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array,
        first_d_index);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_DYNAMIC_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DYNAMIC_DYNAMIC_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array[first_d_index],
        dynamic_index);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_DYNAMIC_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array[first_d_index]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_DYNAMIC_ARRAY,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_DYNAMIC_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array[first_d_index][dynamic_index],
        dynamic_dynamic_array,
        uint32,
        0,
        "example_dynamic_dynamic_array_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_DYNAMIC_ARRAY,
        &dynamic_dynamic_array,
        "example[%d]->dynamic_dynamic_array[%d][%d]",
        unit, first_d_index, dynamic_index);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_DYNAMIC_ARRAY,
        example_info,
        EXAMPLE_DYNAMIC_DYNAMIC_ARRAY_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_dynamic_dynamic_array_get(int unit, uint32 first_d_index, uint32 dynamic_index, uint32 *dynamic_dynamic_array)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_DYNAMIC_ARRAY,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_DYNAMIC_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DYNAMIC_DYNAMIC_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array,
        first_d_index);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_DYNAMIC_ARRAY,
        dynamic_dynamic_array);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DYNAMIC_DYNAMIC_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array[first_d_index],
        dynamic_index);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_DYNAMIC_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_DYNAMIC_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array[first_d_index]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_DYNAMIC_ARRAY,
        DNX_SW_STATE_DIAG_READ);

    *dynamic_dynamic_array = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array[first_d_index][dynamic_index];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_DYNAMIC_ARRAY,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array[first_d_index][dynamic_index],
        "example[%d]->dynamic_dynamic_array[%d][%d]",
        unit, first_d_index, dynamic_index);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_DYNAMIC_ARRAY,
        example_info,
        EXAMPLE_DYNAMIC_DYNAMIC_ARRAY_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_dynamic_dynamic_array_alloc(int unit, uint32 nof_instances_to_alloc_0, uint32 nof_instances_to_alloc_1)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_DYNAMIC_ARRAY,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_DYNAMIC_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_DYNAMIC_ARRAY,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_DOUBLE_DYNAMIC_ARRAY_SIZE_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_DYNAMIC_ARRAY,
        nof_instances_to_alloc_0,
        nof_instances_to_alloc_1,
        sizeof(uint32*));

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_DYNAMIC_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array,
        uint32*,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_dynamic_dynamic_array_alloc");

    for(uint32 first_d_index = 0;
         first_d_index < nof_instances_to_alloc_0;
         first_d_index++)

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_DYNAMIC_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array[first_d_index],
        uint32,
        nof_instances_to_alloc_1,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        1,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_dynamic_dynamic_array_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_DYNAMIC_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array,
        "example[%d]->dynamic_dynamic_array",
        unit,
        nof_instances_to_alloc_1 * sizeof(uint32) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_dynamic_array)+(nof_instances_to_alloc_1 * sizeof(uint32)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_DYNAMIC_ARRAY,
        example_info,
        EXAMPLE_DYNAMIC_DYNAMIC_ARRAY_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_dynamic_array_static_set(int unit, uint32 dynamic_index, uint32 static_index, uint32 dynamic_array_static)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_STATIC,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_STATIC,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_STATIC,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_static,
        dynamic_index);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_STATIC,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_static);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        static_index,
        5);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_STATIC,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_STATIC,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_static[dynamic_index][static_index],
        dynamic_array_static,
        uint32,
        0,
        "example_dynamic_array_static_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_STATIC,
        &dynamic_array_static,
        "example[%d]->dynamic_array_static[%d][%d]",
        unit, dynamic_index, static_index);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_STATIC,
        example_info,
        EXAMPLE_DYNAMIC_ARRAY_STATIC_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_dynamic_array_static_get(int unit, uint32 dynamic_index, uint32 static_index, uint32 *dynamic_array_static)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_STATIC,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_STATIC,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_STATIC,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_static,
        dynamic_index);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_STATIC,
        dynamic_array_static);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        static_index,
        5);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_STATIC,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_static);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_STATIC,
        DNX_SW_STATE_DIAG_READ);

    *dynamic_array_static = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_static[dynamic_index][static_index];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_STATIC,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_static[dynamic_index][static_index],
        "example[%d]->dynamic_array_static[%d][%d]",
        unit, dynamic_index, static_index);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_STATIC,
        example_info,
        EXAMPLE_DYNAMIC_ARRAY_STATIC_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_dynamic_array_static_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_STATIC,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_STATIC,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_STATIC,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_STATIC,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_static,
        uint32[5],
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_dynamic_array_static_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_STATIC,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_static,
        "example[%d]->dynamic_array_static",
        unit,
        nof_instances_to_alloc_0 * sizeof(uint32[5]) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->dynamic_array_static));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DYNAMIC_ARRAY_STATIC,
        example_info,
        EXAMPLE_DYNAMIC_ARRAY_STATIC_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

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
        DNX_SW_STATE_EXAMPLE__COUNTER_TEST,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__COUNTER_TEST,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__COUNTER_TEST,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__COUNTER_TEST,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->counter_test,
        counter_test,
        uint32,
        0,
        "example_counter_test_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__COUNTER_TEST,
        &counter_test,
        "example[%d]->counter_test",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__COUNTER_TEST,
        example_info,
        EXAMPLE_COUNTER_TEST_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_counter_test_get(int unit, uint32 *counter_test)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__COUNTER_TEST,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__COUNTER_TEST,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__COUNTER_TEST,
        counter_test);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__COUNTER_TEST,
        DNX_SW_STATE_DIAG_READ);

    *counter_test = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->counter_test;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__COUNTER_TEST,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->counter_test,
        "example[%d]->counter_test",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__COUNTER_TEST,
        example_info,
        EXAMPLE_COUNTER_TEST_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_counter_test_inc(int unit, uint32 inc_value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__COUNTER_TEST,
        SW_STATE_FUNC_INC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__COUNTER_TEST,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__COUNTER_TEST,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_COUNTER_INC(
        unit,
        DNX_SW_STATE_EXAMPLE__COUNTER_TEST,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->counter_test,
        inc_value,
        uint32,
        0,
        "example_counter_test_inc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_COUNTER_INC_LOGGING,
        BSL_LS_SWSTATEDNX_COUNTER_INC,
        DNX_SW_STATE_EXAMPLE__COUNTER_TEST,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->counter_test,
        "example[%d]->counter_test",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__COUNTER_TEST,
        example_info,
        EXAMPLE_COUNTER_TEST_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_counter_test_dec(int unit, uint32 dec_value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__COUNTER_TEST,
        SW_STATE_FUNC_DEC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__COUNTER_TEST,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__COUNTER_TEST,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_COUNTER_DEC(
        unit,
        DNX_SW_STATE_EXAMPLE__COUNTER_TEST,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->counter_test,
        dec_value,
        uint32,
        0,
        "example_counter_test_dec");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_COUNTER_DEC_LOGGING,
        BSL_LS_SWSTATEDNX_COUNTER_DEC,
        DNX_SW_STATE_EXAMPLE__COUNTER_TEST,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->counter_test,
        "example[%d]->counter_test",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__COUNTER_TEST,
        example_info,
        EXAMPLE_COUNTER_TEST_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_DNXData_array_alloc_exception_set(int unit, uint32 DNXData_array_alloc_exception_idx_0, uint32 DNXData_array_alloc_exception)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_ALLOC_EXCEPTION,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_ALLOC_EXCEPTION,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_ALLOC_EXCEPTION,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_alloc_exception,
        DNXData_array_alloc_exception_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_ALLOC_EXCEPTION,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_alloc_exception);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_ALLOC_EXCEPTION,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_ALLOC_EXCEPTION,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_alloc_exception[DNXData_array_alloc_exception_idx_0],
        DNXData_array_alloc_exception,
        uint32,
        0,
        "example_DNXData_array_alloc_exception_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_ALLOC_EXCEPTION,
        &DNXData_array_alloc_exception,
        "example[%d]->DNXData_array_alloc_exception[%d]",
        unit, DNXData_array_alloc_exception_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_ALLOC_EXCEPTION,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_ALLOC_EXCEPTION_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_DNXData_array_alloc_exception_get(int unit, uint32 DNXData_array_alloc_exception_idx_0, uint32 *DNXData_array_alloc_exception)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_ALLOC_EXCEPTION,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_ALLOC_EXCEPTION,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_ALLOC_EXCEPTION,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_alloc_exception,
        DNXData_array_alloc_exception_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_ALLOC_EXCEPTION,
        DNXData_array_alloc_exception);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_ALLOC_EXCEPTION,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_alloc_exception);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_ALLOC_EXCEPTION,
        DNX_SW_STATE_DIAG_READ);

    *DNXData_array_alloc_exception = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_alloc_exception[DNXData_array_alloc_exception_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_ALLOC_EXCEPTION,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_alloc_exception[DNXData_array_alloc_exception_idx_0],
        "example[%d]->DNXData_array_alloc_exception[%d]",
        unit, DNXData_array_alloc_exception_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_ALLOC_EXCEPTION,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_ALLOC_EXCEPTION_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_DNXData_array_alloc_exception_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_ALLOC_EXCEPTION,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_ALLOC_EXCEPTION,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_ALLOC_EXCEPTION,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_ALLOC_EXCEPTION,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_alloc_exception,
        uint32,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        0,
        DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_DNXData_array_alloc_exception_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_ALLOC_EXCEPTION,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_alloc_exception,
        "example[%d]->DNXData_array_alloc_exception",
        unit,
        nof_instances_to_alloc_0 * sizeof(uint32) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_alloc_exception));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_ALLOC_EXCEPTION,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_ALLOC_EXCEPTION_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_DNXData_array_set(int unit, uint32 DNXData_array_idx_0, uint32 DNXData_array)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array,
        DNXData_array_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array[DNXData_array_idx_0],
        DNXData_array,
        uint32,
        0,
        "example_DNXData_array_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY,
        &DNXData_array,
        "example[%d]->DNXData_array[%d]",
        unit, DNXData_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_DNXData_array_get(int unit, uint32 DNXData_array_idx_0, uint32 *DNXData_array)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array,
        DNXData_array_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY,
        DNXData_array);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY,
        DNX_SW_STATE_DIAG_READ);

    *DNXData_array = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array[DNXData_array_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array[DNXData_array_idx_0],
        "example[%d]->DNXData_array[%d]",
        unit, DNXData_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_DNXData_array_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array,
        uint32,
        dnx_data_module_testing.example_tests.field_size_get(unit),
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_DNXData_array_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array,
        "example[%d]->DNXData_array",
        unit,
        dnx_data_module_testing.example_tests.field_size_get(unit) * sizeof(uint32) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_DNXData_array_dyn_dnxdata_set(int unit, uint32 DNXData_array_dyn_dnxdata_idx_0, uint32 DNXData_array_dyn_dnxdata_idx_1, uint32 DNXData_array_dyn_dnxdata)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DYN_DNXDATA,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DYN_DNXDATA,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DYN_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata,
        DNXData_array_dyn_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DYN_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DYN_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata[DNXData_array_dyn_dnxdata_idx_0],
        DNXData_array_dyn_dnxdata_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DYN_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata[DNXData_array_dyn_dnxdata_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DYN_DNXDATA,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DYN_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata[DNXData_array_dyn_dnxdata_idx_0][DNXData_array_dyn_dnxdata_idx_1],
        DNXData_array_dyn_dnxdata,
        uint32,
        0,
        "example_DNXData_array_dyn_dnxdata_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DYN_DNXDATA,
        &DNXData_array_dyn_dnxdata,
        "example[%d]->DNXData_array_dyn_dnxdata[%d][%d]",
        unit, DNXData_array_dyn_dnxdata_idx_0, DNXData_array_dyn_dnxdata_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DYN_DNXDATA,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_DYN_DNXDATA_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_DNXData_array_dyn_dnxdata_get(int unit, uint32 DNXData_array_dyn_dnxdata_idx_0, uint32 DNXData_array_dyn_dnxdata_idx_1, uint32 *DNXData_array_dyn_dnxdata)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DYN_DNXDATA,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DYN_DNXDATA,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DYN_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata,
        DNXData_array_dyn_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DYN_DNXDATA,
        DNXData_array_dyn_dnxdata);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DYN_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata[DNXData_array_dyn_dnxdata_idx_0],
        DNXData_array_dyn_dnxdata_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DYN_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DYN_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata[DNXData_array_dyn_dnxdata_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DYN_DNXDATA,
        DNX_SW_STATE_DIAG_READ);

    *DNXData_array_dyn_dnxdata = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata[DNXData_array_dyn_dnxdata_idx_0][DNXData_array_dyn_dnxdata_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DYN_DNXDATA,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata[DNXData_array_dyn_dnxdata_idx_0][DNXData_array_dyn_dnxdata_idx_1],
        "example[%d]->DNXData_array_dyn_dnxdata[%d][%d]",
        unit, DNXData_array_dyn_dnxdata_idx_0, DNXData_array_dyn_dnxdata_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DYN_DNXDATA,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_DYN_DNXDATA_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_DNXData_array_dyn_dnxdata_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DYN_DNXDATA,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DYN_DNXDATA,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DYN_DNXDATA,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_DOUBLE_DYNAMIC_ARRAY_SIZE_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DYN_DNXDATA,
        nof_instances_to_alloc_0,
        dnx_data_module_testing.example_tests.field_size_get(unit),
        sizeof(uint32*));

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DYN_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata,
        uint32*,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_DNXData_array_dyn_dnxdata_alloc");

    for(uint32 DNXData_array_dyn_dnxdata_idx_0 = 0;
         DNXData_array_dyn_dnxdata_idx_0 < nof_instances_to_alloc_0;
         DNXData_array_dyn_dnxdata_idx_0++)

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DYN_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata[DNXData_array_dyn_dnxdata_idx_0],
        uint32,
        dnx_data_module_testing.example_tests.field_size_get(unit),
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_DNXData_array_dyn_dnxdata_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DYN_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata,
        "example[%d]->DNXData_array_dyn_dnxdata",
        unit,
        dnx_data_module_testing.example_tests.field_size_get(unit) * sizeof(uint32) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dyn_dnxdata)+(dnx_data_module_testing.example_tests.field_size_get(unit) * sizeof(uint32)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DYN_DNXDATA,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_DYN_DNXDATA_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_DNXData_array_dnxdata_dyn_set(int unit, uint32 DNXData_array_dnxdata_dyn_idx_0, uint32 DNXData_array_dnxdata_dyn_idx_1, uint32 DNXData_array_dnxdata_dyn)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DYN,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DYN,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DYN,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn,
        DNXData_array_dnxdata_dyn_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DYN,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DYN,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn[DNXData_array_dnxdata_dyn_idx_0],
        DNXData_array_dnxdata_dyn_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DYN,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn[DNXData_array_dnxdata_dyn_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DYN,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DYN,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn[DNXData_array_dnxdata_dyn_idx_0][DNXData_array_dnxdata_dyn_idx_1],
        DNXData_array_dnxdata_dyn,
        uint32,
        0,
        "example_DNXData_array_dnxdata_dyn_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DYN,
        &DNXData_array_dnxdata_dyn,
        "example[%d]->DNXData_array_dnxdata_dyn[%d][%d]",
        unit, DNXData_array_dnxdata_dyn_idx_0, DNXData_array_dnxdata_dyn_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DYN,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_DNXDATA_DYN_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_DNXData_array_dnxdata_dyn_get(int unit, uint32 DNXData_array_dnxdata_dyn_idx_0, uint32 DNXData_array_dnxdata_dyn_idx_1, uint32 *DNXData_array_dnxdata_dyn)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DYN,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DYN,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DYN,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn,
        DNXData_array_dnxdata_dyn_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DYN,
        DNXData_array_dnxdata_dyn);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DYN,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn[DNXData_array_dnxdata_dyn_idx_0],
        DNXData_array_dnxdata_dyn_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DYN,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DYN,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn[DNXData_array_dnxdata_dyn_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DYN,
        DNX_SW_STATE_DIAG_READ);

    *DNXData_array_dnxdata_dyn = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn[DNXData_array_dnxdata_dyn_idx_0][DNXData_array_dnxdata_dyn_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DYN,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn[DNXData_array_dnxdata_dyn_idx_0][DNXData_array_dnxdata_dyn_idx_1],
        "example[%d]->DNXData_array_dnxdata_dyn[%d][%d]",
        unit, DNXData_array_dnxdata_dyn_idx_0, DNXData_array_dnxdata_dyn_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DYN,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_DNXDATA_DYN_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_DNXData_array_dnxdata_dyn_alloc(int unit, uint32 nof_instances_to_alloc_1)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DYN,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DYN,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DYN,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_DOUBLE_DYNAMIC_ARRAY_SIZE_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DYN,
        dnx_data_module_testing.example_tests.field_size_get(unit),
        nof_instances_to_alloc_1,
        sizeof(uint32*));

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DYN,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn,
        uint32*,
        dnx_data_module_testing.example_tests.field_size_get(unit),
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_DNXData_array_dnxdata_dyn_alloc");

    for(uint32 DNXData_array_dnxdata_dyn_idx_0 = 0;
         DNXData_array_dnxdata_dyn_idx_0 < dnx_data_module_testing.example_tests.field_size_get(unit);
         DNXData_array_dnxdata_dyn_idx_0++)

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DYN,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn[DNXData_array_dnxdata_dyn_idx_0],
        uint32,
        nof_instances_to_alloc_1,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        1,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_DNXData_array_dnxdata_dyn_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DYN,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn,
        "example[%d]->DNXData_array_dnxdata_dyn",
        unit,
        nof_instances_to_alloc_1 * sizeof(uint32) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dyn)+(nof_instances_to_alloc_1 * sizeof(uint32)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DYN,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_DNXDATA_DYN_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_DNXData_array_dnxdata_dnxdata_set(int unit, uint32 DNXData_array_dnxdata_dnxdata_idx_0, uint32 DNXData_array_dnxdata_dnxdata_idx_1, uint32 DNXData_array_dnxdata_dnxdata)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DNXDATA,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DNXDATA,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata,
        DNXData_array_dnxdata_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata[DNXData_array_dnxdata_dnxdata_idx_0],
        DNXData_array_dnxdata_dnxdata_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata[DNXData_array_dnxdata_dnxdata_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DNXDATA,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata[DNXData_array_dnxdata_dnxdata_idx_0][DNXData_array_dnxdata_dnxdata_idx_1],
        DNXData_array_dnxdata_dnxdata,
        uint32,
        0,
        "example_DNXData_array_dnxdata_dnxdata_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DNXDATA,
        &DNXData_array_dnxdata_dnxdata,
        "example[%d]->DNXData_array_dnxdata_dnxdata[%d][%d]",
        unit, DNXData_array_dnxdata_dnxdata_idx_0, DNXData_array_dnxdata_dnxdata_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DNXDATA,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_DNXDATA_DNXDATA_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_DNXData_array_dnxdata_dnxdata_get(int unit, uint32 DNXData_array_dnxdata_dnxdata_idx_0, uint32 DNXData_array_dnxdata_dnxdata_idx_1, uint32 *DNXData_array_dnxdata_dnxdata)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DNXDATA,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DNXDATA,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata,
        DNXData_array_dnxdata_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DNXDATA,
        DNXData_array_dnxdata_dnxdata);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata[DNXData_array_dnxdata_dnxdata_idx_0],
        DNXData_array_dnxdata_dnxdata_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata[DNXData_array_dnxdata_dnxdata_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DNXDATA,
        DNX_SW_STATE_DIAG_READ);

    *DNXData_array_dnxdata_dnxdata = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata[DNXData_array_dnxdata_dnxdata_idx_0][DNXData_array_dnxdata_dnxdata_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DNXDATA,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata[DNXData_array_dnxdata_dnxdata_idx_0][DNXData_array_dnxdata_dnxdata_idx_1],
        "example[%d]->DNXData_array_dnxdata_dnxdata[%d][%d]",
        unit, DNXData_array_dnxdata_dnxdata_idx_0, DNXData_array_dnxdata_dnxdata_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DNXDATA,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_DNXDATA_DNXDATA_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_DNXData_array_dnxdata_dnxdata_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DNXDATA,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DNXDATA,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DNXDATA,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_DOUBLE_DYNAMIC_ARRAY_SIZE_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DNXDATA,
        dnx_data_module_testing.example_tests.field_size_get(unit),
        dnx_data_module_testing.example_tests.field_size_get(unit),
        sizeof(uint32*));

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata,
        uint32*,
        dnx_data_module_testing.example_tests.field_size_get(unit),
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_DNXData_array_dnxdata_dnxdata_alloc");

    for(uint32 DNXData_array_dnxdata_dnxdata_idx_0 = 0;
         DNXData_array_dnxdata_dnxdata_idx_0 < dnx_data_module_testing.example_tests.field_size_get(unit);
         DNXData_array_dnxdata_dnxdata_idx_0++)

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata[DNXData_array_dnxdata_dnxdata_idx_0],
        uint32,
        dnx_data_module_testing.example_tests.field_size_get(unit),
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_DNXData_array_dnxdata_dnxdata_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata,
        "example[%d]->DNXData_array_dnxdata_dnxdata",
        unit,
        dnx_data_module_testing.example_tests.field_size_get(unit) * sizeof(uint32) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_dnxdata_dnxdata)+(dnx_data_module_testing.example_tests.field_size_get(unit) * sizeof(uint32)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_DNXDATA_DNXDATA,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_DNXDATA_DNXDATA_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_DNXData_array_static_dnxdata_dyn_set(int unit, uint32 DNXData_array_static_dnxdata_dyn_idx_0, uint32 DNXData_array_static_dnxdata_dyn_idx_1, uint32 DNXData_array_static_dnxdata_dyn_idx_2, uint32 DNXData_array_static_dnxdata_dyn)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DYN,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DYN,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        DNXData_array_static_dnxdata_dyn_idx_0,
        3);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DYN,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[DNXData_array_static_dnxdata_dyn_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DYN,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[DNXData_array_static_dnxdata_dyn_idx_0],
        DNXData_array_static_dnxdata_dyn_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DYN,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[DNXData_array_static_dnxdata_dyn_idx_0][DNXData_array_static_dnxdata_dyn_idx_1]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DYN,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[DNXData_array_static_dnxdata_dyn_idx_0][DNXData_array_static_dnxdata_dyn_idx_1],
        DNXData_array_static_dnxdata_dyn_idx_2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DYN,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DYN,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[DNXData_array_static_dnxdata_dyn_idx_0][DNXData_array_static_dnxdata_dyn_idx_1][DNXData_array_static_dnxdata_dyn_idx_2],
        DNXData_array_static_dnxdata_dyn,
        uint32,
        0,
        "example_DNXData_array_static_dnxdata_dyn_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DYN,
        &DNXData_array_static_dnxdata_dyn,
        "example[%d]->DNXData_array_static_dnxdata_dyn[%d][%d][%d]",
        unit, DNXData_array_static_dnxdata_dyn_idx_0, DNXData_array_static_dnxdata_dyn_idx_1, DNXData_array_static_dnxdata_dyn_idx_2);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DYN,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_STATIC_DNXDATA_DYN_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_DNXData_array_static_dnxdata_dyn_get(int unit, uint32 DNXData_array_static_dnxdata_dyn_idx_0, uint32 DNXData_array_static_dnxdata_dyn_idx_1, uint32 DNXData_array_static_dnxdata_dyn_idx_2, uint32 *DNXData_array_static_dnxdata_dyn)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DYN,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DYN,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        DNXData_array_static_dnxdata_dyn_idx_0,
        3);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DYN,
        DNXData_array_static_dnxdata_dyn);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DYN,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[DNXData_array_static_dnxdata_dyn_idx_0],
        DNXData_array_static_dnxdata_dyn_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DYN,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[DNXData_array_static_dnxdata_dyn_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DYN,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[DNXData_array_static_dnxdata_dyn_idx_0][DNXData_array_static_dnxdata_dyn_idx_1],
        DNXData_array_static_dnxdata_dyn_idx_2);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DYN,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[DNXData_array_static_dnxdata_dyn_idx_0][DNXData_array_static_dnxdata_dyn_idx_1]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DYN,
        DNX_SW_STATE_DIAG_READ);

    *DNXData_array_static_dnxdata_dyn = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[DNXData_array_static_dnxdata_dyn_idx_0][DNXData_array_static_dnxdata_dyn_idx_1][DNXData_array_static_dnxdata_dyn_idx_2];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DYN,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[DNXData_array_static_dnxdata_dyn_idx_0][DNXData_array_static_dnxdata_dyn_idx_1][DNXData_array_static_dnxdata_dyn_idx_2],
        "example[%d]->DNXData_array_static_dnxdata_dyn[%d][%d][%d]",
        unit, DNXData_array_static_dnxdata_dyn_idx_0, DNXData_array_static_dnxdata_dyn_idx_1, DNXData_array_static_dnxdata_dyn_idx_2);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DYN,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_STATIC_DNXDATA_DYN_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_DNXData_array_static_dnxdata_dyn_alloc(int unit, uint32 DNXData_array_static_dnxdata_dyn_idx_0, uint32 nof_instances_to_alloc_2)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DYN,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DYN,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DYN,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_DOUBLE_DYNAMIC_ARRAY_SIZE_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DYN,
        dnx_data_module_testing.example_tests.field_size_get(unit),
        nof_instances_to_alloc_2,
        sizeof(uint32*));

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DYN,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[DNXData_array_static_dnxdata_dyn_idx_0],
        uint32*,
        dnx_data_module_testing.example_tests.field_size_get(unit),
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_DNXData_array_static_dnxdata_dyn_alloc");

    for(uint32 DNXData_array_static_dnxdata_dyn_idx_1 = 0;
         DNXData_array_static_dnxdata_dyn_idx_1 < dnx_data_module_testing.example_tests.field_size_get(unit);
         DNXData_array_static_dnxdata_dyn_idx_1++)

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DYN,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[DNXData_array_static_dnxdata_dyn_idx_0][DNXData_array_static_dnxdata_dyn_idx_1],
        uint32,
        nof_instances_to_alloc_2,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        2,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_DNXData_array_static_dnxdata_dyn_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DYN,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[DNXData_array_static_dnxdata_dyn_idx_0],
        "example[%d]->DNXData_array_static_dnxdata_dyn[%d]",
        unit, DNXData_array_static_dnxdata_dyn_idx_0,
        nof_instances_to_alloc_2 * sizeof(uint32) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dyn[DNXData_array_static_dnxdata_dyn_idx_0])+(nof_instances_to_alloc_2 * sizeof(uint32)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DYN,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_STATIC_DNXDATA_DYN_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_DNXData_array_static_dyn_dnxdata_set(int unit, uint32 DNXData_array_static_dyn_dnxdata_idx_0, uint32 DNXData_array_static_dyn_dnxdata_idx_1, uint32 DNXData_array_static_dyn_dnxdata_idx_2, uint32 DNXData_array_static_dyn_dnxdata)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DYN_DNXDATA,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DYN_DNXDATA,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        DNXData_array_static_dyn_dnxdata_idx_0,
        3);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DYN_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[DNXData_array_static_dyn_dnxdata_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DYN_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[DNXData_array_static_dyn_dnxdata_idx_0],
        DNXData_array_static_dyn_dnxdata_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DYN_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[DNXData_array_static_dyn_dnxdata_idx_0][DNXData_array_static_dyn_dnxdata_idx_1]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DYN_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[DNXData_array_static_dyn_dnxdata_idx_0][DNXData_array_static_dyn_dnxdata_idx_1],
        DNXData_array_static_dyn_dnxdata_idx_2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DYN_DNXDATA,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DYN_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[DNXData_array_static_dyn_dnxdata_idx_0][DNXData_array_static_dyn_dnxdata_idx_1][DNXData_array_static_dyn_dnxdata_idx_2],
        DNXData_array_static_dyn_dnxdata,
        uint32,
        0,
        "example_DNXData_array_static_dyn_dnxdata_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DYN_DNXDATA,
        &DNXData_array_static_dyn_dnxdata,
        "example[%d]->DNXData_array_static_dyn_dnxdata[%d][%d][%d]",
        unit, DNXData_array_static_dyn_dnxdata_idx_0, DNXData_array_static_dyn_dnxdata_idx_1, DNXData_array_static_dyn_dnxdata_idx_2);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DYN_DNXDATA,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_STATIC_DYN_DNXDATA_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_DNXData_array_static_dyn_dnxdata_get(int unit, uint32 DNXData_array_static_dyn_dnxdata_idx_0, uint32 DNXData_array_static_dyn_dnxdata_idx_1, uint32 DNXData_array_static_dyn_dnxdata_idx_2, uint32 *DNXData_array_static_dyn_dnxdata)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DYN_DNXDATA,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DYN_DNXDATA,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        DNXData_array_static_dyn_dnxdata_idx_0,
        3);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DYN_DNXDATA,
        DNXData_array_static_dyn_dnxdata);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DYN_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[DNXData_array_static_dyn_dnxdata_idx_0],
        DNXData_array_static_dyn_dnxdata_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DYN_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[DNXData_array_static_dyn_dnxdata_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DYN_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[DNXData_array_static_dyn_dnxdata_idx_0][DNXData_array_static_dyn_dnxdata_idx_1],
        DNXData_array_static_dyn_dnxdata_idx_2);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DYN_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[DNXData_array_static_dyn_dnxdata_idx_0][DNXData_array_static_dyn_dnxdata_idx_1]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DYN_DNXDATA,
        DNX_SW_STATE_DIAG_READ);

    *DNXData_array_static_dyn_dnxdata = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[DNXData_array_static_dyn_dnxdata_idx_0][DNXData_array_static_dyn_dnxdata_idx_1][DNXData_array_static_dyn_dnxdata_idx_2];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DYN_DNXDATA,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[DNXData_array_static_dyn_dnxdata_idx_0][DNXData_array_static_dyn_dnxdata_idx_1][DNXData_array_static_dyn_dnxdata_idx_2],
        "example[%d]->DNXData_array_static_dyn_dnxdata[%d][%d][%d]",
        unit, DNXData_array_static_dyn_dnxdata_idx_0, DNXData_array_static_dyn_dnxdata_idx_1, DNXData_array_static_dyn_dnxdata_idx_2);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DYN_DNXDATA,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_STATIC_DYN_DNXDATA_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_DNXData_array_static_dyn_dnxdata_alloc(int unit, uint32 DNXData_array_static_dyn_dnxdata_idx_0, uint32 nof_instances_to_alloc_1)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DYN_DNXDATA,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DYN_DNXDATA,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DYN_DNXDATA,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_DOUBLE_DYNAMIC_ARRAY_SIZE_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DYN_DNXDATA,
        nof_instances_to_alloc_1,
        dnx_data_module_testing.example_tests.field_size_get(unit),
        sizeof(uint32*));

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DYN_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[DNXData_array_static_dyn_dnxdata_idx_0],
        uint32*,
        nof_instances_to_alloc_1,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        1,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_DNXData_array_static_dyn_dnxdata_alloc");

    for(uint32 DNXData_array_static_dyn_dnxdata_idx_1 = 0;
         DNXData_array_static_dyn_dnxdata_idx_1 < nof_instances_to_alloc_1;
         DNXData_array_static_dyn_dnxdata_idx_1++)

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DYN_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[DNXData_array_static_dyn_dnxdata_idx_0][DNXData_array_static_dyn_dnxdata_idx_1],
        uint32,
        dnx_data_module_testing.example_tests.field_size_get(unit),
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_DNXData_array_static_dyn_dnxdata_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DYN_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[DNXData_array_static_dyn_dnxdata_idx_0],
        "example[%d]->DNXData_array_static_dyn_dnxdata[%d]",
        unit, DNXData_array_static_dyn_dnxdata_idx_0,
        dnx_data_module_testing.example_tests.field_size_get(unit) * sizeof(uint32) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dyn_dnxdata[DNXData_array_static_dyn_dnxdata_idx_0])+(dnx_data_module_testing.example_tests.field_size_get(unit) * sizeof(uint32)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DYN_DNXDATA,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_STATIC_DYN_DNXDATA_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_DNXData_array_static_dnxdata_dnxdata_set(int unit, uint32 DNXData_array_static_dnxdata_dnxdata_idx_0, uint32 DNXData_array_static_dnxdata_dnxdata_idx_1, uint32 DNXData_array_static_dnxdata_dnxdata_idx_2, uint32 DNXData_array_static_dnxdata_dnxdata)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        DNXData_array_static_dnxdata_dnxdata_idx_0,
        3);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[DNXData_array_static_dnxdata_dnxdata_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[DNXData_array_static_dnxdata_dnxdata_idx_0],
        DNXData_array_static_dnxdata_dnxdata_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[DNXData_array_static_dnxdata_dnxdata_idx_0][DNXData_array_static_dnxdata_dnxdata_idx_1]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[DNXData_array_static_dnxdata_dnxdata_idx_0][DNXData_array_static_dnxdata_dnxdata_idx_1],
        DNXData_array_static_dnxdata_dnxdata_idx_2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[DNXData_array_static_dnxdata_dnxdata_idx_0][DNXData_array_static_dnxdata_dnxdata_idx_1][DNXData_array_static_dnxdata_dnxdata_idx_2],
        DNXData_array_static_dnxdata_dnxdata,
        uint32,
        0,
        "example_DNXData_array_static_dnxdata_dnxdata_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA,
        &DNXData_array_static_dnxdata_dnxdata,
        "example[%d]->DNXData_array_static_dnxdata_dnxdata[%d][%d][%d]",
        unit, DNXData_array_static_dnxdata_dnxdata_idx_0, DNXData_array_static_dnxdata_dnxdata_idx_1, DNXData_array_static_dnxdata_dnxdata_idx_2);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_DNXData_array_static_dnxdata_dnxdata_get(int unit, uint32 DNXData_array_static_dnxdata_dnxdata_idx_0, uint32 DNXData_array_static_dnxdata_dnxdata_idx_1, uint32 DNXData_array_static_dnxdata_dnxdata_idx_2, uint32 *DNXData_array_static_dnxdata_dnxdata)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        DNXData_array_static_dnxdata_dnxdata_idx_0,
        3);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA,
        DNXData_array_static_dnxdata_dnxdata);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[DNXData_array_static_dnxdata_dnxdata_idx_0],
        DNXData_array_static_dnxdata_dnxdata_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[DNXData_array_static_dnxdata_dnxdata_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[DNXData_array_static_dnxdata_dnxdata_idx_0][DNXData_array_static_dnxdata_dnxdata_idx_1],
        DNXData_array_static_dnxdata_dnxdata_idx_2);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[DNXData_array_static_dnxdata_dnxdata_idx_0][DNXData_array_static_dnxdata_dnxdata_idx_1]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA,
        DNX_SW_STATE_DIAG_READ);

    *DNXData_array_static_dnxdata_dnxdata = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[DNXData_array_static_dnxdata_dnxdata_idx_0][DNXData_array_static_dnxdata_dnxdata_idx_1][DNXData_array_static_dnxdata_dnxdata_idx_2];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[DNXData_array_static_dnxdata_dnxdata_idx_0][DNXData_array_static_dnxdata_dnxdata_idx_1][DNXData_array_static_dnxdata_dnxdata_idx_2],
        "example[%d]->DNXData_array_static_dnxdata_dnxdata[%d][%d][%d]",
        unit, DNXData_array_static_dnxdata_dnxdata_idx_0, DNXData_array_static_dnxdata_dnxdata_idx_1, DNXData_array_static_dnxdata_dnxdata_idx_2);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_DNXData_array_static_dnxdata_dnxdata_alloc(int unit, uint32 DNXData_array_static_dnxdata_dnxdata_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_DOUBLE_DYNAMIC_ARRAY_SIZE_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA,
        dnx_data_module_testing.example_tests.field_size_get(unit),
        dnx_data_module_testing.example_tests.field_size_get(unit),
        sizeof(uint32*));

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[DNXData_array_static_dnxdata_dnxdata_idx_0],
        uint32*,
        dnx_data_module_testing.example_tests.field_size_get(unit),
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_DNXData_array_static_dnxdata_dnxdata_alloc");

    for(uint32 DNXData_array_static_dnxdata_dnxdata_idx_1 = 0;
         DNXData_array_static_dnxdata_dnxdata_idx_1 < dnx_data_module_testing.example_tests.field_size_get(unit);
         DNXData_array_static_dnxdata_dnxdata_idx_1++)

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[DNXData_array_static_dnxdata_dnxdata_idx_0][DNXData_array_static_dnxdata_dnxdata_idx_1],
        uint32,
        dnx_data_module_testing.example_tests.field_size_get(unit),
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_DNXData_array_static_dnxdata_dnxdata_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[DNXData_array_static_dnxdata_dnxdata_idx_0],
        "example[%d]->DNXData_array_static_dnxdata_dnxdata[%d]",
        unit, DNXData_array_static_dnxdata_dnxdata_idx_0,
        dnx_data_module_testing.example_tests.field_size_get(unit) * sizeof(uint32) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_static_dnxdata_dnxdata[DNXData_array_static_dnxdata_dnxdata_idx_0])+(dnx_data_module_testing.example_tests.field_size_get(unit) * sizeof(uint32)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_STATIC_DNXDATA_DNXDATA_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_DNXData_array_table_data_set(int unit, uint32 DNXData_array_table_data_idx_0, uint32 DNXData_array_table_data)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_TABLE_DATA,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_TABLE_DATA,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_TABLE_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_table_data,
        DNXData_array_table_data_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_TABLE_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_table_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_TABLE_DATA,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_TABLE_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_table_data[DNXData_array_table_data_idx_0],
        DNXData_array_table_data,
        uint32,
        0,
        "example_DNXData_array_table_data_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_TABLE_DATA,
        &DNXData_array_table_data,
        "example[%d]->DNXData_array_table_data[%d]",
        unit, DNXData_array_table_data_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_TABLE_DATA,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_TABLE_DATA_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_DNXData_array_table_data_get(int unit, uint32 DNXData_array_table_data_idx_0, uint32 *DNXData_array_table_data)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_TABLE_DATA,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_TABLE_DATA,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_TABLE_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_table_data,
        DNXData_array_table_data_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_TABLE_DATA,
        DNXData_array_table_data);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_TABLE_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_table_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_TABLE_DATA,
        DNX_SW_STATE_DIAG_READ);

    *DNXData_array_table_data = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_table_data[DNXData_array_table_data_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_TABLE_DATA,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_table_data[DNXData_array_table_data_idx_0],
        "example[%d]->DNXData_array_table_data[%d]",
        unit, DNXData_array_table_data_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_TABLE_DATA,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_TABLE_DATA_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_DNXData_array_table_data_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_TABLE_DATA,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_TABLE_DATA,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_TABLE_DATA,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_TABLE_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_table_data,
        uint32,
        dnx_data_module_testing.example_tests.all_get(unit)->vlan_id_in_ingress,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_DNXData_array_table_data_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_TABLE_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_table_data,
        "example[%d]->DNXData_array_table_data",
        unit,
        dnx_data_module_testing.example_tests.all_get(unit)->vlan_id_in_ingress * sizeof(uint32) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->DNXData_array_table_data));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DNXDATA_ARRAY_TABLE_DATA,
        example_info,
        EXAMPLE_DNXDATA_ARRAY_TABLE_DATA_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_array_range_example_set(int unit, uint32 array_range_example_idx_0, uint32 array_range_example)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__ARRAY_RANGE_EXAMPLE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__ARRAY_RANGE_EXAMPLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        array_range_example_idx_0,
        10);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__ARRAY_RANGE_EXAMPLE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__ARRAY_RANGE_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->array_range_example[array_range_example_idx_0],
        array_range_example,
        uint32,
        0,
        "example_array_range_example_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__ARRAY_RANGE_EXAMPLE,
        &array_range_example,
        "example[%d]->array_range_example[%d]",
        unit, array_range_example_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__ARRAY_RANGE_EXAMPLE,
        example_info,
        EXAMPLE_ARRAY_RANGE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_array_range_example_get(int unit, uint32 array_range_example_idx_0, uint32 *array_range_example)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__ARRAY_RANGE_EXAMPLE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__ARRAY_RANGE_EXAMPLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        array_range_example_idx_0,
        10);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__ARRAY_RANGE_EXAMPLE,
        array_range_example);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__ARRAY_RANGE_EXAMPLE,
        DNX_SW_STATE_DIAG_READ);

    *array_range_example = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->array_range_example[array_range_example_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__ARRAY_RANGE_EXAMPLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->array_range_example[array_range_example_idx_0],
        "example[%d]->array_range_example[%d]",
        unit, array_range_example_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__ARRAY_RANGE_EXAMPLE,
        example_info,
        EXAMPLE_ARRAY_RANGE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_array_range_example_range_read(int unit, uint32 nof_elements, uint32 *dest)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__ARRAY_RANGE_EXAMPLE,
        SW_STATE_FUNC_RANGE_READ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__ARRAY_RANGE_EXAMPLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__ARRAY_RANGE_EXAMPLE,
        DNX_SW_STATE_DIAG_READ);

    DNX_SW_STATE_RANGE_READ(
        unit,
        DNX_SW_STATE_EXAMPLE__ARRAY_RANGE_EXAMPLE,
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
        DNX_SW_STATE_EXAMPLE__ARRAY_RANGE_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->array_range_example,
        "example[%d]->array_range_example",
        unit, nof_elements);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__ARRAY_RANGE_EXAMPLE,
        example_info,
        EXAMPLE_ARRAY_RANGE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_array_range_example_range_write(int unit, uint32 nof_elements, const uint32 *source)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__ARRAY_RANGE_EXAMPLE,
        SW_STATE_FUNC_RANGE_WRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__ARRAY_RANGE_EXAMPLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__ARRAY_RANGE_EXAMPLE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_RANGE_COPY(
        unit,
        DNX_SW_STATE_EXAMPLE__ARRAY_RANGE_EXAMPLE,
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
        DNX_SW_STATE_EXAMPLE__ARRAY_RANGE_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->array_range_example,
        "example[%d]->array_range_example",
        unit, nof_elements);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__ARRAY_RANGE_EXAMPLE,
        example_info,
        EXAMPLE_ARRAY_RANGE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_array_range_example_range_fill(int unit, uint32 from_idx, uint32 nof_elements, uint32 value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__ARRAY_RANGE_EXAMPLE,
        SW_STATE_FUNC_RANGE_FILL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__ARRAY_RANGE_EXAMPLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__ARRAY_RANGE_EXAMPLE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_RANGE_FILL(
        unit,
        DNX_SW_STATE_EXAMPLE__ARRAY_RANGE_EXAMPLE,
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
        DNX_SW_STATE_EXAMPLE__ARRAY_RANGE_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->array_range_example,
        "example[%d]->array_range_example",
        unit, nof_elements);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__ARRAY_RANGE_EXAMPLE,
        example_info,
        EXAMPLE_ARRAY_RANGE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_alloc_after_init_variable_set(int unit, uint32 alloc_after_init_variable_idx_0, uint32 alloc_after_init_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_VARIABLE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_variable,
        alloc_after_init_variable_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_VARIABLE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_variable[alloc_after_init_variable_idx_0],
        alloc_after_init_variable,
        uint32,
        0,
        "example_alloc_after_init_variable_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_VARIABLE,
        &alloc_after_init_variable,
        "example[%d]->alloc_after_init_variable[%d]",
        unit, alloc_after_init_variable_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_VARIABLE,
        example_info,
        EXAMPLE_ALLOC_AFTER_INIT_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_alloc_after_init_variable_get(int unit, uint32 alloc_after_init_variable_idx_0, uint32 *alloc_after_init_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_VARIABLE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_variable,
        alloc_after_init_variable_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_VARIABLE,
        alloc_after_init_variable);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_VARIABLE,
        DNX_SW_STATE_DIAG_READ);

    *alloc_after_init_variable = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_variable[alloc_after_init_variable_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_variable[alloc_after_init_variable_idx_0],
        "example[%d]->alloc_after_init_variable[%d]",
        unit, alloc_after_init_variable_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_VARIABLE,
        example_info,
        EXAMPLE_ALLOC_AFTER_INIT_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_alloc_after_init_variable_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_VARIABLE,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_VARIABLE,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_variable,
        uint32,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        0,
        DNXC_SW_STATE_ALLOC_AFTER_INIT_EXCEPTION | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_alloc_after_init_variable_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_variable,
        "example[%d]->alloc_after_init_variable",
        unit,
        nof_instances_to_alloc_0 * sizeof(uint32) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_variable));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_VARIABLE,
        example_info,
        EXAMPLE_ALLOC_AFTER_INIT_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_alloc_after_init_test_set(int unit, uint32 alloc_after_init_test_idx_0, uint32 alloc_after_init_test)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_TEST,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_TEST,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_TEST,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_test,
        alloc_after_init_test_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_TEST,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_test);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_TEST,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_TEST,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_test[alloc_after_init_test_idx_0],
        alloc_after_init_test,
        uint32,
        0,
        "example_alloc_after_init_test_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_TEST,
        &alloc_after_init_test,
        "example[%d]->alloc_after_init_test[%d]",
        unit, alloc_after_init_test_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_TEST,
        example_info,
        EXAMPLE_ALLOC_AFTER_INIT_TEST_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_alloc_after_init_test_get(int unit, uint32 alloc_after_init_test_idx_0, uint32 *alloc_after_init_test)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_TEST,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_TEST,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_TEST,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_test,
        alloc_after_init_test_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_TEST,
        alloc_after_init_test);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_TEST,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_test);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_TEST,
        DNX_SW_STATE_DIAG_READ);

    *alloc_after_init_test = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_test[alloc_after_init_test_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_TEST,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_test[alloc_after_init_test_idx_0],
        "example[%d]->alloc_after_init_test[%d]",
        unit, alloc_after_init_test_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_TEST,
        example_info,
        EXAMPLE_ALLOC_AFTER_INIT_TEST_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_alloc_after_init_test_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_TEST,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_TEST,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_TEST,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_TEST,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_test,
        uint32,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_alloc_after_init_test_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_TEST,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_test,
        "example[%d]->alloc_after_init_test",
        unit,
        nof_instances_to_alloc_0 * sizeof(uint32) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_after_init_test));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_AFTER_INIT_TEST,
        example_info,
        EXAMPLE_ALLOC_AFTER_INIT_TEST_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_write_during_wb_example_set(int unit, int write_during_wb_example)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__WRITE_DURING_WB_EXAMPLE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_WB);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__WRITE_DURING_WB_EXAMPLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__WRITE_DURING_WB_EXAMPLE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__WRITE_DURING_WB_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->write_during_wb_example,
        write_during_wb_example,
        int,
        0,
        "example_write_during_wb_example_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__WRITE_DURING_WB_EXAMPLE,
        &write_during_wb_example,
        "example[%d]->write_during_wb_example",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__WRITE_DURING_WB_EXAMPLE,
        example_info,
        EXAMPLE_WRITE_DURING_WB_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_write_during_wb_example_get(int unit, int *write_during_wb_example)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__WRITE_DURING_WB_EXAMPLE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_WB);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__WRITE_DURING_WB_EXAMPLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__WRITE_DURING_WB_EXAMPLE,
        write_during_wb_example);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__WRITE_DURING_WB_EXAMPLE,
        DNX_SW_STATE_DIAG_READ);

    *write_during_wb_example = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->write_during_wb_example;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__WRITE_DURING_WB_EXAMPLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->write_during_wb_example,
        "example[%d]->write_during_wb_example",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__WRITE_DURING_WB_EXAMPLE,
        example_info,
        EXAMPLE_WRITE_DURING_WB_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_value_range_test_set(int unit, int value_range_test)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__VALUE_RANGE_TEST,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__VALUE_RANGE_TEST,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_VALID_VALUE_RANGE(
        unit,
        value_range_test,
        1,
        10);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__VALUE_RANGE_TEST,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__VALUE_RANGE_TEST,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->value_range_test,
        value_range_test,
        int,
        0,
        "example_value_range_test_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__VALUE_RANGE_TEST,
        &value_range_test,
        "example[%d]->value_range_test",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__VALUE_RANGE_TEST,
        example_info,
        EXAMPLE_VALUE_RANGE_TEST_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_value_range_test_get(int unit, int *value_range_test)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__VALUE_RANGE_TEST,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__VALUE_RANGE_TEST,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__VALUE_RANGE_TEST,
        value_range_test);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__VALUE_RANGE_TEST,
        DNX_SW_STATE_DIAG_READ);

    *value_range_test = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->value_range_test;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__VALUE_RANGE_TEST,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->value_range_test,
        "example[%d]->value_range_test",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__VALUE_RANGE_TEST,
        example_info,
        EXAMPLE_VALUE_RANGE_TEST_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_variable_alloc_bitmap(int unit, uint32 _nof_bits_to_alloc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        SW_STATE_FUNC_ALLOC_BITMAP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP);

    DNX_SW_STATE_ALLOC_BITMAP(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        _nof_bits_to_alloc,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_bitmap_variable_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_BITMAP_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOCBITMAP,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit,
        _nof_bits_to_alloc);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_BITMAP_DEFAULT_VALUE_SET(((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable, _nof_bits_to_alloc);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_variable_free(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        SW_STATE_FUNC_FREE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        DNX_SW_STATE_DIAG_FREE);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_FREE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_FREE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example_bitmap_variable_free");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_FREE_LOGGING,
        BSL_LS_SWSTATEDNX_FREE,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
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
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        SW_STATE_FUNC_BIT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_SET(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITSET,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_variable_bit_clear(int unit, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        SW_STATE_FUNC_BIT_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_CLEAR(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITCLEAR,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_variable_bit_get(int unit, uint32 _bit_num, uint8* result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        SW_STATE_FUNC_BIT_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_GET(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
         _bit_num,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_GET_LOGGING,
        BSL_LS_SWSTATEDNX_BITGET,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_variable_bit_range_read(int unit, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        SW_STATE_FUNC_BIT_RANGE_READ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_READ(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
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
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_variable_bit_range_write(int unit, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        SW_STATE_FUNC_BIT_RANGE_WRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_WRITE(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
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
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_variable_bit_range_and(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        SW_STATE_FUNC_BIT_RANGE_AND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_AND(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_AND_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEAND,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_variable_bit_range_or(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        SW_STATE_FUNC_BIT_RANGE_OR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_OR(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_OR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEOR,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_variable_bit_range_xor(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        SW_STATE_FUNC_BIT_RANGE_XOR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_XOR(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_XOR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEXOR,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_variable_bit_range_remove(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        SW_STATE_FUNC_BIT_RANGE_REMOVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_REMOVE(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_REMOVE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREMOVE,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_variable_bit_range_negate(int unit, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        SW_STATE_FUNC_BIT_RANGE_NEGATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_NEGATE(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NEGATE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENEGATE,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_variable_bit_range_clear(int unit, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        SW_STATE_FUNC_BIT_RANGE_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_CLEAR(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECLEAR,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_variable_bit_range_set(int unit, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        SW_STATE_FUNC_BIT_RANGE_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_SET(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGESET,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_variable_bit_range_null(int unit, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        SW_STATE_FUNC_BIT_RANGE_NULL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_NULL(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NULL_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENULL,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_variable_bit_range_test(int unit, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        SW_STATE_FUNC_BIT_RANGE_TEST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_TEST(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_TEST_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGETEST,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_variable_bit_range_eq(int unit, SHR_BITDCL *input_bmp, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        SW_STATE_FUNC_BIT_RANGE_EQ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_EQ(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
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
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_variable_bit_range_count(int unit, uint32 _first, uint32 _range, int *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        SW_STATE_FUNC_BIT_RANGE_COUNT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        (_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_COUNT(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
         _first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_COUNT_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECOUNT,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable,
        "example[%d]->bitmap_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_fixed_bit_set(int unit, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        SW_STATE_FUNC_BIT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        _bit_num,
        1024);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_SET(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITSET,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
        "example[%d]->bitmap_fixed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        example_info,
        EXAMPLE_BITMAP_FIXED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_fixed_bit_clear(int unit, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        SW_STATE_FUNC_BIT_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        _bit_num,
        1024);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_CLEAR(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITCLEAR,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
        "example[%d]->bitmap_fixed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        example_info,
        EXAMPLE_BITMAP_FIXED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_fixed_bit_get(int unit, uint32 _bit_num, uint8* result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        SW_STATE_FUNC_BIT_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        _bit_num,
        1024);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_GET(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
         _bit_num,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_GET_LOGGING,
        BSL_LS_SWSTATEDNX_BITGET,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
        "example[%d]->bitmap_fixed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        example_info,
        EXAMPLE_BITMAP_FIXED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_fixed_bit_range_read(int unit, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        SW_STATE_FUNC_BIT_RANGE_READ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (sw_state_bmp_first + _range - 1),
        1024);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_READ(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
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
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
        "example[%d]->bitmap_fixed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        example_info,
        EXAMPLE_BITMAP_FIXED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_fixed_bit_range_write(int unit, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        SW_STATE_FUNC_BIT_RANGE_WRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (sw_state_bmp_first + _range - 1),
        1024);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_WRITE(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
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
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
        "example[%d]->bitmap_fixed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        example_info,
        EXAMPLE_BITMAP_FIXED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_fixed_bit_range_and(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        SW_STATE_FUNC_BIT_RANGE_AND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (_first + _count - 1),
        1024);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_AND(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_AND_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEAND,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
        "example[%d]->bitmap_fixed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        example_info,
        EXAMPLE_BITMAP_FIXED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_fixed_bit_range_or(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        SW_STATE_FUNC_BIT_RANGE_OR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (_first + _count - 1),
        1024);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_OR(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_OR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEOR,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
        "example[%d]->bitmap_fixed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        example_info,
        EXAMPLE_BITMAP_FIXED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_fixed_bit_range_xor(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        SW_STATE_FUNC_BIT_RANGE_XOR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (_first + _count - 1),
        1024);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_XOR(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_XOR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEXOR,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
        "example[%d]->bitmap_fixed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        example_info,
        EXAMPLE_BITMAP_FIXED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_fixed_bit_range_remove(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        SW_STATE_FUNC_BIT_RANGE_REMOVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (_first + _count - 1),
        1024);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_REMOVE(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_REMOVE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREMOVE,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
        "example[%d]->bitmap_fixed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        example_info,
        EXAMPLE_BITMAP_FIXED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_fixed_bit_range_negate(int unit, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        SW_STATE_FUNC_BIT_RANGE_NEGATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (_first + _count - 1),
        1024);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_NEGATE(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NEGATE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENEGATE,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
        "example[%d]->bitmap_fixed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        example_info,
        EXAMPLE_BITMAP_FIXED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_fixed_bit_range_clear(int unit, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        SW_STATE_FUNC_BIT_RANGE_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (_first + _count - 1),
        1024);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_CLEAR(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECLEAR,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
        "example[%d]->bitmap_fixed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        example_info,
        EXAMPLE_BITMAP_FIXED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_fixed_bit_range_set(int unit, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        SW_STATE_FUNC_BIT_RANGE_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (_first + _count - 1),
        1024);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_SET(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGESET,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
        "example[%d]->bitmap_fixed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        example_info,
        EXAMPLE_BITMAP_FIXED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_fixed_bit_range_null(int unit, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        SW_STATE_FUNC_BIT_RANGE_NULL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (_first + _count - 1),
        1024);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_NULL(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NULL_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENULL,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
        "example[%d]->bitmap_fixed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        example_info,
        EXAMPLE_BITMAP_FIXED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_fixed_bit_range_test(int unit, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        SW_STATE_FUNC_BIT_RANGE_TEST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (_first + _count - 1),
        1024);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_TEST(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_TEST_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGETEST,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
        "example[%d]->bitmap_fixed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        example_info,
        EXAMPLE_BITMAP_FIXED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_fixed_bit_range_eq(int unit, SHR_BITDCL *input_bmp, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        SW_STATE_FUNC_BIT_RANGE_EQ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (_first + _count - 1),
        1024);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_EQ(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
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
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
        "example[%d]->bitmap_fixed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        example_info,
        EXAMPLE_BITMAP_FIXED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_fixed_bit_range_count(int unit, uint32 _first, uint32 _range, int *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        SW_STATE_FUNC_BIT_RANGE_COUNT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (_first + _range - 1),
        1024);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_COUNT(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
         _first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_COUNT_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECOUNT,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_fixed,
        "example[%d]->bitmap_fixed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_FIXED,
        example_info,
        EXAMPLE_BITMAP_FIXED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_dnx_data_alloc_bitmap(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        SW_STATE_FUNC_ALLOC_BITMAP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP);

    DNX_SW_STATE_ALLOC_BITMAP(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        dnx_data_module_testing.example_tests.field_size_get(unit),
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_bitmap_dnx_data_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_BITMAP_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOCBITMAP,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit,
        dnx_data_module_testing.example_tests.field_size_get(unit));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        example_info,
        EXAMPLE_BITMAP_DNX_DATA_INFO,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_dnx_data_bit_set(int unit, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        SW_STATE_FUNC_BIT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_SET(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITSET,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        example_info,
        EXAMPLE_BITMAP_DNX_DATA_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_dnx_data_bit_clear(int unit, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        SW_STATE_FUNC_BIT_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_CLEAR(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITCLEAR,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        example_info,
        EXAMPLE_BITMAP_DNX_DATA_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_dnx_data_bit_get(int unit, uint32 _bit_num, uint8* result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        SW_STATE_FUNC_BIT_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_GET(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
         _bit_num,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_GET_LOGGING,
        BSL_LS_SWSTATEDNX_BITGET,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        example_info,
        EXAMPLE_BITMAP_DNX_DATA_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_dnx_data_bit_range_read(int unit, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        SW_STATE_FUNC_BIT_RANGE_READ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_READ(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
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
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        example_info,
        EXAMPLE_BITMAP_DNX_DATA_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_dnx_data_bit_range_write(int unit, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        SW_STATE_FUNC_BIT_RANGE_WRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_WRITE(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
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
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        example_info,
        EXAMPLE_BITMAP_DNX_DATA_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_dnx_data_bit_range_and(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        SW_STATE_FUNC_BIT_RANGE_AND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_AND(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_AND_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEAND,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        example_info,
        EXAMPLE_BITMAP_DNX_DATA_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_dnx_data_bit_range_or(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        SW_STATE_FUNC_BIT_RANGE_OR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_OR(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_OR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEOR,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        example_info,
        EXAMPLE_BITMAP_DNX_DATA_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_dnx_data_bit_range_xor(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        SW_STATE_FUNC_BIT_RANGE_XOR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_XOR(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_XOR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEXOR,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        example_info,
        EXAMPLE_BITMAP_DNX_DATA_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_dnx_data_bit_range_remove(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        SW_STATE_FUNC_BIT_RANGE_REMOVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_REMOVE(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_REMOVE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREMOVE,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        example_info,
        EXAMPLE_BITMAP_DNX_DATA_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_dnx_data_bit_range_negate(int unit, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        SW_STATE_FUNC_BIT_RANGE_NEGATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_NEGATE(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NEGATE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENEGATE,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        example_info,
        EXAMPLE_BITMAP_DNX_DATA_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_dnx_data_bit_range_clear(int unit, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        SW_STATE_FUNC_BIT_RANGE_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_CLEAR(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECLEAR,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        example_info,
        EXAMPLE_BITMAP_DNX_DATA_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_dnx_data_bit_range_set(int unit, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        SW_STATE_FUNC_BIT_RANGE_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_SET(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGESET,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        example_info,
        EXAMPLE_BITMAP_DNX_DATA_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_dnx_data_bit_range_null(int unit, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        SW_STATE_FUNC_BIT_RANGE_NULL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_NULL(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NULL_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENULL,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        example_info,
        EXAMPLE_BITMAP_DNX_DATA_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_dnx_data_bit_range_test(int unit, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        SW_STATE_FUNC_BIT_RANGE_TEST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_TEST(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_TEST_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGETEST,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        example_info,
        EXAMPLE_BITMAP_DNX_DATA_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_dnx_data_bit_range_eq(int unit, SHR_BITDCL *input_bmp, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        SW_STATE_FUNC_BIT_RANGE_EQ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_EQ(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
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
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        example_info,
        EXAMPLE_BITMAP_DNX_DATA_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_dnx_data_bit_range_count(int unit, uint32 _first, uint32 _range, int *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        SW_STATE_FUNC_BIT_RANGE_COUNT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        (_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_COUNT(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
         _first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_COUNT_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECOUNT,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_dnx_data,
        "example[%d]->bitmap_dnx_data",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_DNX_DATA,
        example_info,
        EXAMPLE_BITMAP_DNX_DATA_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_exclude_example_alloc_bitmap(int unit, uint32 _nof_bits_to_alloc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        SW_STATE_FUNC_ALLOC_BITMAP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP);

    DNX_SW_STATE_ALLOC_BITMAP(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        _nof_bits_to_alloc,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_bitmap_exclude_example_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_BITMAP_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOCBITMAP,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        "example[%d]->bitmap_exclude_example",
        unit,
        _nof_bits_to_alloc);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        example_info,
        EXAMPLE_BITMAP_EXCLUDE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_exclude_example_bit_set(int unit, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        SW_STATE_FUNC_BIT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_SET(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITSET,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        "example[%d]->bitmap_exclude_example",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        example_info,
        EXAMPLE_BITMAP_EXCLUDE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_exclude_example_bit_get(int unit, uint32 _bit_num, uint8* result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        SW_STATE_FUNC_BIT_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_GET(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
         _bit_num,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_GET_LOGGING,
        BSL_LS_SWSTATEDNX_BITGET,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        "example[%d]->bitmap_exclude_example",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        example_info,
        EXAMPLE_BITMAP_EXCLUDE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_exclude_example_bit_range_read(int unit, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        SW_STATE_FUNC_BIT_RANGE_READ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_READ(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
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
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        "example[%d]->bitmap_exclude_example",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        example_info,
        EXAMPLE_BITMAP_EXCLUDE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_exclude_example_bit_range_write(int unit, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        SW_STATE_FUNC_BIT_RANGE_WRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_WRITE(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
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
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        "example[%d]->bitmap_exclude_example",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        example_info,
        EXAMPLE_BITMAP_EXCLUDE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_exclude_example_bit_range_and(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        SW_STATE_FUNC_BIT_RANGE_AND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_AND(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_AND_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEAND,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        "example[%d]->bitmap_exclude_example",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        example_info,
        EXAMPLE_BITMAP_EXCLUDE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_exclude_example_bit_range_or(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        SW_STATE_FUNC_BIT_RANGE_OR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_OR(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_OR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEOR,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        "example[%d]->bitmap_exclude_example",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        example_info,
        EXAMPLE_BITMAP_EXCLUDE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_exclude_example_bit_range_xor(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        SW_STATE_FUNC_BIT_RANGE_XOR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_XOR(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_XOR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEXOR,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        "example[%d]->bitmap_exclude_example",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        example_info,
        EXAMPLE_BITMAP_EXCLUDE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_exclude_example_bit_range_remove(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        SW_STATE_FUNC_BIT_RANGE_REMOVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_REMOVE(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_REMOVE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREMOVE,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        "example[%d]->bitmap_exclude_example",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        example_info,
        EXAMPLE_BITMAP_EXCLUDE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_exclude_example_bit_range_clear(int unit, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        SW_STATE_FUNC_BIT_RANGE_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_CLEAR(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECLEAR,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        "example[%d]->bitmap_exclude_example",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        example_info,
        EXAMPLE_BITMAP_EXCLUDE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_exclude_example_bit_range_set(int unit, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        SW_STATE_FUNC_BIT_RANGE_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_SET(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGESET,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        "example[%d]->bitmap_exclude_example",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        example_info,
        EXAMPLE_BITMAP_EXCLUDE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_exclude_example_bit_range_eq(int unit, SHR_BITDCL *input_bmp, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        SW_STATE_FUNC_BIT_RANGE_EQ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_EQ(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
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
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        "example[%d]->bitmap_exclude_example",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        example_info,
        EXAMPLE_BITMAP_EXCLUDE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_exclude_example_bit_range_count(int unit, uint32 _first, uint32 _range, int *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        SW_STATE_FUNC_BIT_RANGE_COUNT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        (_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_COUNT(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
         _first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_COUNT_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECOUNT,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_exclude_example,
        "example[%d]->bitmap_exclude_example",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_EXCLUDE_EXAMPLE,
        example_info,
        EXAMPLE_BITMAP_EXCLUDE_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_include_only_example_alloc_bitmap(int unit, uint32 _nof_bits_to_alloc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_INCLUDE_ONLY_EXAMPLE,
        SW_STATE_FUNC_ALLOC_BITMAP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_INCLUDE_ONLY_EXAMPLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_INCLUDE_ONLY_EXAMPLE,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP);

    DNX_SW_STATE_ALLOC_BITMAP(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_INCLUDE_ONLY_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_include_only_example,
        _nof_bits_to_alloc,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_bitmap_include_only_example_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_BITMAP_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOCBITMAP,
        DNX_SW_STATE_EXAMPLE__BITMAP_INCLUDE_ONLY_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_include_only_example,
        "example[%d]->bitmap_include_only_example",
        unit,
        _nof_bits_to_alloc);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_INCLUDE_ONLY_EXAMPLE,
        example_info,
        EXAMPLE_BITMAP_INCLUDE_ONLY_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_include_only_example_bit_set(int unit, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_INCLUDE_ONLY_EXAMPLE,
        SW_STATE_FUNC_BIT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_INCLUDE_ONLY_EXAMPLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_INCLUDE_ONLY_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_include_only_example,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_INCLUDE_ONLY_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_include_only_example);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_INCLUDE_ONLY_EXAMPLE,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_SET(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_INCLUDE_ONLY_EXAMPLE,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_include_only_example,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITSET,
        DNX_SW_STATE_EXAMPLE__BITMAP_INCLUDE_ONLY_EXAMPLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_include_only_example,
        "example[%d]->bitmap_include_only_example",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_INCLUDE_ONLY_EXAMPLE,
        example_info,
        EXAMPLE_BITMAP_INCLUDE_ONLY_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_include_only_example_bit_get(int unit, uint32 _bit_num, uint8* result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_INCLUDE_ONLY_EXAMPLE,
        SW_STATE_FUNC_BIT_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_INCLUDE_ONLY_EXAMPLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_INCLUDE_ONLY_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_include_only_example,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_INCLUDE_ONLY_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_include_only_example);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_INCLUDE_ONLY_EXAMPLE,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_GET(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_INCLUDE_ONLY_EXAMPLE,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_include_only_example,
         _bit_num,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_GET_LOGGING,
        BSL_LS_SWSTATEDNX_BITGET,
        DNX_SW_STATE_EXAMPLE__BITMAP_INCLUDE_ONLY_EXAMPLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_include_only_example,
        "example[%d]->bitmap_include_only_example",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_INCLUDE_ONLY_EXAMPLE,
        example_info,
        EXAMPLE_BITMAP_INCLUDE_ONLY_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_array_alloc_bitmap(int unit, uint32 bitmap_array_idx_0, uint32 _nof_bits_to_alloc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        SW_STATE_FUNC_ALLOC_BITMAP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        bitmap_array_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP);

    DNX_SW_STATE_ALLOC_BITMAP(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        _nof_bits_to_alloc,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_bitmap_array_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_BITMAP_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOCBITMAP,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        "example[%d]->bitmap_array[%d]",
        unit, bitmap_array_idx_0,
        _nof_bits_to_alloc);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_array_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        SHR_BITDCL*,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_bitmap_array_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        "example[%d]->bitmap_array",
        unit,
        nof_instances_to_alloc_0 * sizeof(SHR_BITDCL));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_array_bit_set(int unit, uint32 bitmap_array_idx_0, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        SW_STATE_FUNC_BIT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        bitmap_array_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        _bit_num);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_SET(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITSET,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        "example[%d]->bitmap_array[%d]",
        unit, bitmap_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_array_bit_clear(int unit, uint32 bitmap_array_idx_0, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        SW_STATE_FUNC_BIT_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        bitmap_array_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        _bit_num);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_CLEAR(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITCLEAR,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        "example[%d]->bitmap_array[%d]",
        unit, bitmap_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_array_bit_get(int unit, uint32 bitmap_array_idx_0, uint32 _bit_num, uint8* result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        SW_STATE_FUNC_BIT_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        bitmap_array_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        _bit_num);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_GET(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
         _bit_num,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_GET_LOGGING,
        BSL_LS_SWSTATEDNX_BITGET,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        "example[%d]->bitmap_array[%d]",
        unit, bitmap_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_array_bit_range_read(int unit, uint32 bitmap_array_idx_0, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        SW_STATE_FUNC_BIT_RANGE_READ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        bitmap_array_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_READ(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
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
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        "example[%d]->bitmap_array[%d]",
        unit, bitmap_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_array_bit_range_write(int unit, uint32 bitmap_array_idx_0, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        SW_STATE_FUNC_BIT_RANGE_WRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        bitmap_array_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_WRITE(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
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
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        "example[%d]->bitmap_array[%d]",
        unit, bitmap_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_array_bit_range_and(int unit, uint32 bitmap_array_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        SW_STATE_FUNC_BIT_RANGE_AND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        bitmap_array_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_AND(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_AND_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEAND,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        "example[%d]->bitmap_array[%d]",
        unit, bitmap_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_array_bit_range_or(int unit, uint32 bitmap_array_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        SW_STATE_FUNC_BIT_RANGE_OR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        bitmap_array_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_OR(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_OR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEOR,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        "example[%d]->bitmap_array[%d]",
        unit, bitmap_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_array_bit_range_xor(int unit, uint32 bitmap_array_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        SW_STATE_FUNC_BIT_RANGE_XOR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        bitmap_array_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_XOR(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_XOR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEXOR,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        "example[%d]->bitmap_array[%d]",
        unit, bitmap_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_array_bit_range_remove(int unit, uint32 bitmap_array_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        SW_STATE_FUNC_BIT_RANGE_REMOVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        bitmap_array_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_REMOVE(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_REMOVE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREMOVE,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        "example[%d]->bitmap_array[%d]",
        unit, bitmap_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_array_bit_range_negate(int unit, uint32 bitmap_array_idx_0, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        SW_STATE_FUNC_BIT_RANGE_NEGATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        bitmap_array_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_NEGATE(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NEGATE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENEGATE,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        "example[%d]->bitmap_array[%d]",
        unit, bitmap_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_array_bit_range_clear(int unit, uint32 bitmap_array_idx_0, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        SW_STATE_FUNC_BIT_RANGE_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        bitmap_array_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_CLEAR(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECLEAR,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        "example[%d]->bitmap_array[%d]",
        unit, bitmap_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_array_bit_range_set(int unit, uint32 bitmap_array_idx_0, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        SW_STATE_FUNC_BIT_RANGE_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        bitmap_array_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_SET(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGESET,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        "example[%d]->bitmap_array[%d]",
        unit, bitmap_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_array_bit_range_null(int unit, uint32 bitmap_array_idx_0, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        SW_STATE_FUNC_BIT_RANGE_NULL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        bitmap_array_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_NULL(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NULL_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENULL,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        "example[%d]->bitmap_array[%d]",
        unit, bitmap_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_array_bit_range_test(int unit, uint32 bitmap_array_idx_0, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        SW_STATE_FUNC_BIT_RANGE_TEST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        bitmap_array_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_TEST(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_TEST_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGETEST,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        "example[%d]->bitmap_array[%d]",
        unit, bitmap_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_array_bit_range_eq(int unit, uint32 bitmap_array_idx_0, SHR_BITDCL *input_bmp, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        SW_STATE_FUNC_BIT_RANGE_EQ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        bitmap_array_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_EQ(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
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
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        "example[%d]->bitmap_array[%d]",
        unit, bitmap_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_array_bit_range_count(int unit, uint32 bitmap_array_idx_0, uint32 _first, uint32 _range, int *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        SW_STATE_FUNC_BIT_RANGE_COUNT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array,
        bitmap_array_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        (_first + _range - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_COUNT(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
         _first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_COUNT_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECOUNT,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_array[bitmap_array_idx_0],
        "example[%d]->bitmap_array[%d]",
        unit, bitmap_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_ARRAY,
        example_info,
        EXAMPLE_BITMAP_ARRAY_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_variable_access_get(int unit, CONST SHR_BITDCL **bitmap_variable_access)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        0,
        ((10)/SHR_BITWID)+1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        bitmap_variable_access);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        DNX_SW_STATE_DIAG_READ);

    *bitmap_variable_access = &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable_access[0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable_access[0],
        "example[%d]->bitmap_variable_access[%d]",
        unit, 0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_ACCESS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_variable_access_bit_set(int unit, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        SW_STATE_FUNC_BIT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        _bit_num,
        10);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_SET(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable_access,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITSET,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable_access,
        "example[%d]->bitmap_variable_access",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_ACCESS_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_variable_access_bit_clear(int unit, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        SW_STATE_FUNC_BIT_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        _bit_num,
        10);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_CLEAR(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable_access,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITCLEAR,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable_access,
        "example[%d]->bitmap_variable_access",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_ACCESS_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_variable_access_bit_get(int unit, uint32 _bit_num, uint8* result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        SW_STATE_FUNC_BIT_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        _bit_num,
        10);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_GET(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable_access,
         _bit_num,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_GET_LOGGING,
        BSL_LS_SWSTATEDNX_BITGET,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable_access,
        "example[%d]->bitmap_variable_access",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_ACCESS_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_variable_access_bit_range_read(int unit, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        SW_STATE_FUNC_BIT_RANGE_READ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (sw_state_bmp_first + _range - 1),
        10);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_READ(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable_access,
         sw_state_bmp_first,
         result_first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_READ_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREAD,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable_access,
        "example[%d]->bitmap_variable_access",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_ACCESS_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_variable_access_bit_range_write(int unit, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        SW_STATE_FUNC_BIT_RANGE_WRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (sw_state_bmp_first + _range - 1),
        10);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_WRITE(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable_access,
         sw_state_bmp_first,
         input_bmp_first,
          _range,
         input_bmp);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_WRITE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEWRITE,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable_access,
        "example[%d]->bitmap_variable_access",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_ACCESS_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_variable_access_bit_range_and(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        SW_STATE_FUNC_BIT_RANGE_AND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (_first + _count - 1),
        10);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_AND(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable_access,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_AND_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEAND,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable_access,
        "example[%d]->bitmap_variable_access",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_ACCESS_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_variable_access_bit_range_or(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        SW_STATE_FUNC_BIT_RANGE_OR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (_first + _count - 1),
        10);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_OR(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable_access,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_OR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEOR,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable_access,
        "example[%d]->bitmap_variable_access",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_ACCESS_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_variable_access_bit_range_xor(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        SW_STATE_FUNC_BIT_RANGE_XOR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (_first + _count - 1),
        10);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_XOR(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable_access,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_XOR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEXOR,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable_access,
        "example[%d]->bitmap_variable_access",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_ACCESS_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_variable_access_bit_range_remove(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        SW_STATE_FUNC_BIT_RANGE_REMOVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (_first + _count - 1),
        10);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_REMOVE(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable_access,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_REMOVE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREMOVE,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable_access,
        "example[%d]->bitmap_variable_access",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_ACCESS_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_variable_access_bit_range_negate(int unit, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        SW_STATE_FUNC_BIT_RANGE_NEGATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (_first + _count - 1),
        10);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_NEGATE(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable_access,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NEGATE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENEGATE,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable_access,
        "example[%d]->bitmap_variable_access",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_ACCESS_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_variable_access_bit_range_clear(int unit, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        SW_STATE_FUNC_BIT_RANGE_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (_first + _count - 1),
        10);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_CLEAR(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable_access,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECLEAR,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable_access,
        "example[%d]->bitmap_variable_access",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_ACCESS_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_variable_access_bit_range_set(int unit, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        SW_STATE_FUNC_BIT_RANGE_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (_first + _count - 1),
        10);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_SET(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable_access,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGESET,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable_access,
        "example[%d]->bitmap_variable_access",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_ACCESS_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_variable_access_bit_range_null(int unit, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        SW_STATE_FUNC_BIT_RANGE_NULL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (_first + _count - 1),
        10);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_NULL(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable_access,
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NULL_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENULL,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable_access,
        "example[%d]->bitmap_variable_access",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_ACCESS_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_variable_access_bit_range_test(int unit, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        SW_STATE_FUNC_BIT_RANGE_TEST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (_first + _count - 1),
        10);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_TEST(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable_access,
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_TEST_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGETEST,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable_access,
        "example[%d]->bitmap_variable_access",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_ACCESS_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_variable_access_bit_range_eq(int unit, SHR_BITDCL *input_bmp, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        SW_STATE_FUNC_BIT_RANGE_EQ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (_first + _count - 1),
        10);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_EQ(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable_access,
         _first,
         _count,
         input_bmp,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_EQ_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEEQ,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable_access,
        "example[%d]->bitmap_variable_access",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_ACCESS_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bitmap_variable_access_bit_range_count(int unit, uint32 _first, uint32 _range, int *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        SW_STATE_FUNC_BIT_RANGE_COUNT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        (_first + _range - 1),
        10);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_COUNT(
        unit,
         DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
         0,
         ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable_access,
         _first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_COUNT_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECOUNT,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bitmap_variable_access,
        "example[%d]->bitmap_variable_access",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BITMAP_VARIABLE_ACCESS,
        example_info,
        EXAMPLE_BITMAP_VARIABLE_ACCESS_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_buffer_array_set(int unit, uint32 buffer_array_idx_0, uint32 buffer_array_idx_1, DNX_SW_STATE_BUFF buffer_array)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array,
        buffer_array_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0],
        buffer_array_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0][buffer_array_idx_1],
        buffer_array,
        DNX_SW_STATE_BUFF,
        0,
        "example_buffer_array_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        &buffer_array,
        "example[%d]->buffer_array[%d][%d]",
        unit, buffer_array_idx_0, buffer_array_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        example_info,
        EXAMPLE_BUFFER_ARRAY_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_buffer_array_get(int unit, uint32 buffer_array_idx_0, uint32 buffer_array_idx_1, DNX_SW_STATE_BUFF *buffer_array)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array,
        buffer_array_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        buffer_array);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0],
        buffer_array_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        DNX_SW_STATE_DIAG_READ);

    *buffer_array = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0][buffer_array_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0][buffer_array_idx_1],
        "example[%d]->buffer_array[%d][%d]",
        unit, buffer_array_idx_0, buffer_array_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        example_info,
        EXAMPLE_BUFFER_ARRAY_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_buffer_array_alloc(int unit, uint32 nof_instances_to_alloc_0, uint32 _nof_bytes_to_alloc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_DOUBLE_DYNAMIC_ARRAY_SIZE_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        nof_instances_to_alloc_0,
        _nof_bytes_to_alloc,
        sizeof(DNX_SW_STATE_BUFF*));

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array,
        DNX_SW_STATE_BUFF*,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_buffer_array_alloc");

    for(uint32 buffer_array_idx_0 = 0;
         buffer_array_idx_0 < nof_instances_to_alloc_0;
         buffer_array_idx_0++)

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0],
        DNX_SW_STATE_BUFF,
        _nof_bytes_to_alloc,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_buffer_array_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array,
        "example[%d]->buffer_array",
        unit,
        _nof_bytes_to_alloc * sizeof(DNX_SW_STATE_BUFF) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array)+(_nof_bytes_to_alloc * sizeof(DNX_SW_STATE_BUFF)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        example_info,
        EXAMPLE_BUFFER_ARRAY_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_buffer_array_memread(int unit, uint32 buffer_array_idx_0, uint8 *_dst, uint32 _offset, size_t _len)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        SW_STATE_FUNC_MEMREAD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array,
        (_offset + _len - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0],
        (_offset + _len - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        DNX_SW_STATE_DIAG_READ);

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
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0],
        "example[%d]->buffer_array[%d]",
        unit, buffer_array_idx_0, _len);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        example_info,
        EXAMPLE_BUFFER_ARRAY_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_buffer_array_memwrite(int unit, uint32 buffer_array_idx_0, const uint8 *_src, uint32 _offset, size_t _len)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        SW_STATE_FUNC_MEMWRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array,
        (_offset + _len - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0],
        (_offset + _len - 1));

    DNX_SW_STATE_MEMWRITE(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
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
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        SW_STATE_FUNC_MEMCMP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array,
        (_offset + _len - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
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
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        SW_STATE_FUNC_MEMSET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array,
        (_offset + _len - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0],
        (_offset + _len - 1));

    DNX_SW_STATE_MEMSET(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_array[buffer_array_idx_0],
        _offset,
        _value,
        _len,
        0,
        "example_buffer_array_BUFFER");

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_buffer_set(int unit, uint32 buffer_idx_0, DNX_SW_STATE_BUFF buffer)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__BUFFER,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer,
        buffer_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer[buffer_idx_0],
        buffer,
        DNX_SW_STATE_BUFF,
        0,
        "example_buffer_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__BUFFER,
        &buffer,
        "example[%d]->buffer[%d]",
        unit, buffer_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER,
        example_info,
        EXAMPLE_BUFFER_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_buffer_get(int unit, uint32 buffer_idx_0, DNX_SW_STATE_BUFF *buffer)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__BUFFER,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer,
        buffer_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER,
        buffer);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER,
        DNX_SW_STATE_DIAG_READ);

    *buffer = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer[buffer_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__BUFFER,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer[buffer_idx_0],
        "example[%d]->buffer[%d]",
        unit, buffer_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER,
        example_info,
        EXAMPLE_BUFFER_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_buffer_alloc(int unit, uint32 _nof_bytes_to_alloc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer,
        DNX_SW_STATE_BUFF,
        _nof_bytes_to_alloc,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_buffer_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_EXAMPLE__BUFFER,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer,
        "example[%d]->buffer",
        unit,
        _nof_bytes_to_alloc * sizeof(DNX_SW_STATE_BUFF) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER,
        example_info,
        EXAMPLE_BUFFER_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_BUFFER_DEFAULT_VALUE_SET(((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer, 0xAA, _nof_bytes_to_alloc);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_buffer_memread(int unit, uint8 *_dst, uint32 _offset, size_t _len)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER,
        SW_STATE_FUNC_MEMREAD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        DNX_SW_STATE_EXAMPLE__BUFFER,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer,
        (_offset + _len - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER,
        DNX_SW_STATE_DIAG_READ);

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
        DNX_SW_STATE_EXAMPLE__BUFFER,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer,
        "example[%d]->buffer",
        unit, _len);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER,
        example_info,
        EXAMPLE_BUFFER_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_buffer_memwrite(int unit, const uint8 *_src, uint32 _offset, size_t _len)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER,
        SW_STATE_FUNC_MEMWRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        DNX_SW_STATE_EXAMPLE__BUFFER,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer,
        (_offset + _len - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer);

    DNX_SW_STATE_MEMWRITE(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER,
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
        DNX_SW_STATE_EXAMPLE__BUFFER,
        SW_STATE_FUNC_MEMCMP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        DNX_SW_STATE_EXAMPLE__BUFFER,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer,
        (_offset + _len - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER,
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
        DNX_SW_STATE_EXAMPLE__BUFFER,
        SW_STATE_FUNC_MEMSET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        DNX_SW_STATE_EXAMPLE__BUFFER,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer,
        (_offset + _len - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer);

    DNX_SW_STATE_MEMSET(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER,
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
        DNX_SW_STATE_EXAMPLE__BUFFER_FIXED,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_FIXED,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        buffer_fixed_idx_0,
        1024);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_FIXED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_FIXED,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_fixed[buffer_fixed_idx_0],
        buffer_fixed,
        DNX_SW_STATE_BUFF,
        0,
        "example_buffer_fixed_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__BUFFER_FIXED,
        &buffer_fixed,
        "example[%d]->buffer_fixed[%d]",
        unit, buffer_fixed_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_FIXED,
        example_info,
        EXAMPLE_BUFFER_FIXED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_buffer_fixed_get(int unit, uint32 buffer_fixed_idx_0, DNX_SW_STATE_BUFF *buffer_fixed)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_FIXED,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_FIXED,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        buffer_fixed_idx_0,
        1024);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_FIXED,
        buffer_fixed);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_FIXED,
        DNX_SW_STATE_DIAG_READ);

    *buffer_fixed = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_fixed[buffer_fixed_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__BUFFER_FIXED,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_fixed[buffer_fixed_idx_0],
        "example[%d]->buffer_fixed[%d]",
        unit, buffer_fixed_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_FIXED,
        example_info,
        EXAMPLE_BUFFER_FIXED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_buffer_fixed_memread(int unit, uint8 *_dst, uint32 _offset, size_t _len)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_FIXED,
        SW_STATE_FUNC_MEMREAD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_FIXED,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BUFFER_CHECK(
        (_offset + _len - 1),
        1024);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_FIXED,
        DNX_SW_STATE_DIAG_READ);

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
        DNX_SW_STATE_EXAMPLE__BUFFER_FIXED,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_fixed,
        "example[%d]->buffer_fixed",
        unit, _len);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_FIXED,
        example_info,
        EXAMPLE_BUFFER_FIXED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_buffer_fixed_memwrite(int unit, const uint8 *_src, uint32 _offset, size_t _len)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_FIXED,
        SW_STATE_FUNC_MEMWRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_FIXED,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BUFFER_CHECK(
        (_offset + _len - 1),
        1024);

    DNX_SW_STATE_MEMWRITE(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_FIXED,
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
        DNX_SW_STATE_EXAMPLE__BUFFER_FIXED,
        SW_STATE_FUNC_MEMCMP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_FIXED,
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
        DNX_SW_STATE_EXAMPLE__BUFFER_FIXED,
        SW_STATE_FUNC_MEMSET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_FIXED,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BUFFER_CHECK(
        (_offset + _len - 1),
        1024);

    DNX_SW_STATE_MEMSET(
        unit,
        DNX_SW_STATE_EXAMPLE__BUFFER_FIXED,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->buffer_fixed,
        _offset,
        _value,
        _len,
        0,
        "example_buffer_fixed_BUFFER");

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_default_value_tree_array_dnxdata_alloc(int unit)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        sw_state_default_l2,
        dnx_data_module_testing.example_tests.field_size_get(unit),
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_default_value_tree_array_dnxdata_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        "example[%d]->default_value_tree.array_dnxdata",
        unit,
        dnx_data_module_testing.example_tests.field_size_get(unit) * sizeof(sw_state_default_l2) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_module_testing.example_tests.field_size_get(unit))

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
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA,
        SW_STATE_FUNC_FREE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA,
        DNX_SW_STATE_DIAG_FREE);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_INFO,
        DNX_SW_STATE_DIAG_FREE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_FREE(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        "example_default_value_tree_array_dnxdata_free");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_FREE_LOGGING,
        BSL_LS_SWSTATEDNX_FREE,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA,
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
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        array_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SWSTATE_ALLOC_SIZE_VERIFY(
        unit,
        sizeof(sw_state_default_value),
        SW_STATE_EXAMPLE_DEFINITION * 10,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic,
        sw_state_default_l3,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_default_value_tree_array_dnxdata_array_dynamic_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic,
        "example[%d]->default_value_tree.array_dnxdata[%d].array_dynamic",
        unit, array_dnxdata_idx_0,
        nof_instances_to_alloc_0 * sizeof(sw_state_default_l3) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_DYNAMIC_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

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
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_STATIC_STATIC__MY_VARIABLE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_STATIC_STATIC__MY_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_STATIC_STATIC__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        array_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_STATIC_STATIC__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_STATIC_STATIC__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic,
        array_dynamic_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_STATIC_STATIC__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        array_static_static_idx_0,
        SW_STATE_EXAMPLE_DEFINITION);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        array_static_static_idx_1,
        10);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_STATIC_STATIC__MY_VARIABLE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_STATIC_STATIC__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].array_static_static[array_static_static_idx_0][array_static_static_idx_1].my_variable,
        my_variable,
        uint32,
        0,
        "example_default_value_tree_array_dnxdata_array_dynamic_array_static_static_my_variable_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_STATIC_STATIC__MY_VARIABLE,
        &my_variable,
        "example[%d]->default_value_tree.array_dnxdata[%d].array_dynamic[%d].array_static_static[%d][%d].my_variable",
        unit, array_dnxdata_idx_0, array_dynamic_idx_0, array_static_static_idx_0, array_static_static_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_STATIC_STATIC__MY_VARIABLE,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_DYNAMIC_ARRAY_STATIC_STATIC_MY_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_default_value_tree_array_dnxdata_array_dynamic_array_static_static_my_variable_get(int unit, uint32 array_dnxdata_idx_0, uint32 array_dynamic_idx_0, uint32 array_static_static_idx_0, uint32 array_static_static_idx_1, uint32 *my_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_STATIC_STATIC__MY_VARIABLE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_STATIC_STATIC__MY_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_STATIC_STATIC__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        array_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_STATIC_STATIC__MY_VARIABLE,
        my_variable);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_STATIC_STATIC__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic,
        array_dynamic_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_STATIC_STATIC__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        array_static_static_idx_0,
        SW_STATE_EXAMPLE_DEFINITION);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_STATIC_STATIC__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        array_static_static_idx_1,
        10);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_STATIC_STATIC__MY_VARIABLE,
        DNX_SW_STATE_DIAG_READ);

    *my_variable = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].array_static_static[array_static_static_idx_0][array_static_static_idx_1].my_variable;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_STATIC_STATIC__MY_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].array_static_static[array_static_static_idx_0][array_static_static_idx_1].my_variable,
        "example[%d]->default_value_tree.array_dnxdata[%d].array_dynamic[%d].array_static_static[%d][%d].my_variable",
        unit, array_dnxdata_idx_0, array_dynamic_idx_0, array_static_static_idx_0, array_static_static_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_STATIC_STATIC__MY_VARIABLE,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_DYNAMIC_ARRAY_STATIC_STATIC_MY_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_default_value_tree_array_dnxdata_array_dynamic_default_value_l3_my_variable_set(int unit, uint32 array_dnxdata_idx_0, uint32 array_dynamic_idx_0, uint32 my_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__DEFAULT_VALUE_L3__MY_VARIABLE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__DEFAULT_VALUE_L3__MY_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__DEFAULT_VALUE_L3__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        array_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__DEFAULT_VALUE_L3__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__DEFAULT_VALUE_L3__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic,
        array_dynamic_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__DEFAULT_VALUE_L3__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__DEFAULT_VALUE_L3__MY_VARIABLE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__DEFAULT_VALUE_L3__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].default_value_l3.my_variable,
        my_variable,
        uint32,
        0,
        "example_default_value_tree_array_dnxdata_array_dynamic_default_value_l3_my_variable_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__DEFAULT_VALUE_L3__MY_VARIABLE,
        &my_variable,
        "example[%d]->default_value_tree.array_dnxdata[%d].array_dynamic[%d].default_value_l3.my_variable",
        unit, array_dnxdata_idx_0, array_dynamic_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__DEFAULT_VALUE_L3__MY_VARIABLE,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_DYNAMIC_DEFAULT_VALUE_L3_MY_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_default_value_tree_array_dnxdata_array_dynamic_default_value_l3_my_variable_get(int unit, uint32 array_dnxdata_idx_0, uint32 array_dynamic_idx_0, uint32 *my_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__DEFAULT_VALUE_L3__MY_VARIABLE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__DEFAULT_VALUE_L3__MY_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__DEFAULT_VALUE_L3__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        array_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__DEFAULT_VALUE_L3__MY_VARIABLE,
        my_variable);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__DEFAULT_VALUE_L3__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic,
        array_dynamic_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__DEFAULT_VALUE_L3__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__DEFAULT_VALUE_L3__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__DEFAULT_VALUE_L3__MY_VARIABLE,
        DNX_SW_STATE_DIAG_READ);

    *my_variable = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].default_value_l3.my_variable;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__DEFAULT_VALUE_L3__MY_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].default_value_l3.my_variable,
        "example[%d]->default_value_tree.array_dnxdata[%d].array_dynamic[%d].default_value_l3.my_variable",
        unit, array_dnxdata_idx_0, array_dynamic_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__DEFAULT_VALUE_L3__MY_VARIABLE,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_DYNAMIC_DEFAULT_VALUE_L3_MY_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_default_value_tree_array_dnxdata_array_dynamic_array_dynamic_dynamic_alloc(int unit, uint32 array_dnxdata_idx_0, uint32 array_dynamic_idx_0, uint32 nof_instances_to_alloc_0, uint32 nof_instances_to_alloc_1)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_DOUBLE_DYNAMIC_ARRAY_SIZE_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC,
        nof_instances_to_alloc_0,
        nof_instances_to_alloc_1,
        sizeof(sw_state_default_value*));

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].array_dynamic_dynamic,
        sw_state_default_value*,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_default_value_tree_array_dnxdata_array_dynamic_array_dynamic_dynamic_alloc");

    for(uint32 array_dynamic_dynamic_idx_0 = 0;
         array_dynamic_dynamic_idx_0 < nof_instances_to_alloc_0;
         array_dynamic_dynamic_idx_0++)

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].array_dynamic_dynamic[array_dynamic_dynamic_idx_0],
        sw_state_default_value,
        nof_instances_to_alloc_1,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        1,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_default_value_tree_array_dnxdata_array_dynamic_array_dynamic_dynamic_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].array_dynamic_dynamic,
        "example[%d]->default_value_tree.array_dnxdata[%d].array_dynamic[%d].array_dynamic_dynamic",
        unit, array_dnxdata_idx_0, array_dynamic_idx_0,
        nof_instances_to_alloc_1 * sizeof(sw_state_default_value) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].array_dynamic_dynamic)+(nof_instances_to_alloc_1 * sizeof(sw_state_default_value)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_DYNAMIC_ARRAY_DYNAMIC_DYNAMIC_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

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
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC__MY_VARIABLE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC__MY_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        array_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].array_dynamic_dynamic[array_dynamic_dynamic_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic,
        array_dynamic_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].array_dynamic_dynamic,
        array_dynamic_dynamic_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].array_dynamic_dynamic[array_dynamic_dynamic_idx_0],
        array_dynamic_dynamic_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC__MY_VARIABLE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].array_dynamic_dynamic[array_dynamic_dynamic_idx_0][array_dynamic_dynamic_idx_1].my_variable,
        my_variable,
        uint32,
        0,
        "example_default_value_tree_array_dnxdata_array_dynamic_array_dynamic_dynamic_my_variable_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC__MY_VARIABLE,
        &my_variable,
        "example[%d]->default_value_tree.array_dnxdata[%d].array_dynamic[%d].array_dynamic_dynamic[%d][%d].my_variable",
        unit, array_dnxdata_idx_0, array_dynamic_idx_0, array_dynamic_dynamic_idx_0, array_dynamic_dynamic_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC__MY_VARIABLE,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_DYNAMIC_ARRAY_DYNAMIC_DYNAMIC_MY_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_default_value_tree_array_dnxdata_array_dynamic_array_dynamic_dynamic_my_variable_get(int unit, uint32 array_dnxdata_idx_0, uint32 array_dynamic_idx_0, uint32 array_dynamic_dynamic_idx_0, uint32 array_dynamic_dynamic_idx_1, uint32 *my_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC__MY_VARIABLE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC__MY_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        array_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC__MY_VARIABLE,
        my_variable);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic,
        array_dynamic_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].array_dynamic_dynamic[array_dynamic_dynamic_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].array_dynamic_dynamic,
        array_dynamic_dynamic_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].array_dynamic_dynamic[array_dynamic_dynamic_idx_0],
        array_dynamic_dynamic_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC__MY_VARIABLE,
        DNX_SW_STATE_DIAG_READ);

    *my_variable = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].array_dynamic_dynamic[array_dynamic_dynamic_idx_0][array_dynamic_dynamic_idx_1].my_variable;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC__MY_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].array_dynamic[array_dynamic_idx_0].array_dynamic_dynamic[array_dynamic_dynamic_idx_0][array_dynamic_dynamic_idx_1].my_variable,
        "example[%d]->default_value_tree.array_dnxdata[%d].array_dynamic[%d].array_dynamic_dynamic[%d][%d].my_variable",
        unit, array_dnxdata_idx_0, array_dynamic_idx_0, array_dynamic_dynamic_idx_0, array_dynamic_dynamic_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__ARRAY_DYNAMIC__ARRAY_DYNAMIC_DYNAMIC__MY_VARIABLE,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_ARRAY_DYNAMIC_ARRAY_DYNAMIC_DYNAMIC_MY_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_default_value_tree_array_dnxdata_default_value_l2_my_variable_set(int unit, uint32 array_dnxdata_idx_0, uint32 my_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__DEFAULT_VALUE_L2__MY_VARIABLE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__DEFAULT_VALUE_L2__MY_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__DEFAULT_VALUE_L2__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        array_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__DEFAULT_VALUE_L2__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__DEFAULT_VALUE_L2__MY_VARIABLE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__DEFAULT_VALUE_L2__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].default_value_l2.my_variable,
        my_variable,
        uint32,
        0,
        "example_default_value_tree_array_dnxdata_default_value_l2_my_variable_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__DEFAULT_VALUE_L2__MY_VARIABLE,
        &my_variable,
        "example[%d]->default_value_tree.array_dnxdata[%d].default_value_l2.my_variable",
        unit, array_dnxdata_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__DEFAULT_VALUE_L2__MY_VARIABLE,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_DEFAULT_VALUE_L2_MY_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_default_value_tree_array_dnxdata_default_value_l2_my_variable_get(int unit, uint32 array_dnxdata_idx_0, uint32 *my_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__DEFAULT_VALUE_L2__MY_VARIABLE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__DEFAULT_VALUE_L2__MY_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__DEFAULT_VALUE_L2__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata,
        array_dnxdata_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__DEFAULT_VALUE_L2__MY_VARIABLE,
        my_variable);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__DEFAULT_VALUE_L2__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__DEFAULT_VALUE_L2__MY_VARIABLE,
        DNX_SW_STATE_DIAG_READ);

    *my_variable = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].default_value_l2.my_variable;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__DEFAULT_VALUE_L2__MY_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.array_dnxdata[array_dnxdata_idx_0].default_value_l2.my_variable,
        "example[%d]->default_value_tree.array_dnxdata[%d].default_value_l2.my_variable",
        unit, array_dnxdata_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__ARRAY_DNXDATA__DEFAULT_VALUE_L2__MY_VARIABLE,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_ARRAY_DNXDATA_DEFAULT_VALUE_L2_MY_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_default_value_tree_default_value_l1_my_variable_set(int unit, uint32 my_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__DEFAULT_VALUE_L1__MY_VARIABLE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__DEFAULT_VALUE_L1__MY_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__DEFAULT_VALUE_L1__MY_VARIABLE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__DEFAULT_VALUE_L1__MY_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.default_value_l1.my_variable,
        my_variable,
        uint32,
        0,
        "example_default_value_tree_default_value_l1_my_variable_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__DEFAULT_VALUE_L1__MY_VARIABLE,
        &my_variable,
        "example[%d]->default_value_tree.default_value_l1.my_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__DEFAULT_VALUE_L1__MY_VARIABLE,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_DEFAULT_VALUE_L1_MY_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_default_value_tree_default_value_l1_my_variable_get(int unit, uint32 *my_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__DEFAULT_VALUE_L1__MY_VARIABLE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__DEFAULT_VALUE_L1__MY_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__DEFAULT_VALUE_L1__MY_VARIABLE,
        my_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__DEFAULT_VALUE_L1__MY_VARIABLE,
        DNX_SW_STATE_DIAG_READ);

    *my_variable = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.default_value_l1.my_variable;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__DEFAULT_VALUE_L1__MY_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->default_value_tree.default_value_l1.my_variable,
        "example[%d]->default_value_tree.default_value_l1.my_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFAULT_VALUE_TREE__DEFAULT_VALUE_L1__MY_VARIABLE,
        example_info,
        EXAMPLE_DEFAULT_VALUE_TREE_DEFAULT_VALUE_L1_MY_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_ll_create_empty(int unit, sw_state_ll_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        SW_STATE_FUNC_CREATE_EMPTY,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        DNX_SW_STATE_DIAG_LL_CREATE);

    SW_STATE_LL_CREATE_EMPTY(
        DNX_SW_STATE_EXAMPLE__LL,
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
        DNX_SW_STATE_EXAMPLE__LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        "example[%d]->ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        example_info,
        EXAMPLE_LL_INFO,
        DNX_SW_STATE_DIAG_LL_CREATE,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_ll_nof_elements(int unit, uint32 *nof_elements)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        SW_STATE_FUNC_NOF_ELEMENTS,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_LL_NOF_ELEMENTS(
        DNX_SW_STATE_EXAMPLE__LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        nof_elements);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NOF_ELEMENTS_LOGGING,
        BSL_LS_SWSTATEDNX_LLNOF_ELEMENTS,
        DNX_SW_STATE_EXAMPLE__LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        "example[%d]->ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        example_info,
        EXAMPLE_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_ll_node_value(int unit, sw_state_ll_node_t node, int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        SW_STATE_FUNC_NODE_VALUE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_LL_NODE_VALUE(
        DNX_SW_STATE_EXAMPLE__LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        node,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NODE_VALUE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNODE_VALUE,
        DNX_SW_STATE_EXAMPLE__LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        "example[%d]->ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        example_info,
        EXAMPLE_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_ll_node_update(int unit, sw_state_ll_node_t node, const int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        SW_STATE_FUNC_NODE_UPDATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_LL_NODE_UPDATE(
        DNX_SW_STATE_EXAMPLE__LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        node,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NODE_UPDATE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNODE_UPDATE,
        DNX_SW_STATE_EXAMPLE__LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        "example[%d]->ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        example_info,
        EXAMPLE_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_ll_next_node(int unit, sw_state_ll_node_t node, sw_state_ll_node_t *next_node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        SW_STATE_FUNC_NEXT_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_LL_NEXT_NODE(
        DNX_SW_STATE_EXAMPLE__LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        node,
        next_node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NEXT_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNEXT_NODE,
        DNX_SW_STATE_EXAMPLE__LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        "example[%d]->ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        example_info,
        EXAMPLE_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_ll_previous_node(int unit, sw_state_ll_node_t node, sw_state_ll_node_t *prev_node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        SW_STATE_FUNC_PREVIOUS_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_LL_PREVIOUS_NODE(
        DNX_SW_STATE_EXAMPLE__LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        node,
        prev_node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_PREVIOUS_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLPREVIOUS_NODE,
        DNX_SW_STATE_EXAMPLE__LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        "example[%d]->ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        example_info,
        EXAMPLE_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_ll_remove_node(int unit, sw_state_ll_node_t node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        SW_STATE_FUNC_REMOVE_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_LL_REMOVE_NODE(
        DNX_SW_STATE_EXAMPLE__LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_REMOVE_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLREMOVE_NODE,
        DNX_SW_STATE_EXAMPLE__LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        "example[%d]->ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        example_info,
        EXAMPLE_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_ll_get_last(int unit, sw_state_ll_node_t *node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        SW_STATE_FUNC_GET_LAST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_LL_GET_LAST(
        DNX_SW_STATE_EXAMPLE__LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_GET_LAST_LOGGING,
        BSL_LS_SWSTATEDNX_LLGET_LAST,
        DNX_SW_STATE_EXAMPLE__LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        "example[%d]->ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        example_info,
        EXAMPLE_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_ll_get_first(int unit, sw_state_ll_node_t *node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        SW_STATE_FUNC_GET_FIRST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_LL_GET_FIRST(
        DNX_SW_STATE_EXAMPLE__LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_GET_FIRST_LOGGING,
        BSL_LS_SWSTATEDNX_LLGET_FIRST,
        DNX_SW_STATE_EXAMPLE__LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        "example[%d]->ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        example_info,
        EXAMPLE_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_ll_print(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        SW_STATE_FUNC_PRINT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_LL_PRINT(
        DNX_SW_STATE_EXAMPLE__LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_PRINT_LOGGING,
        BSL_LS_SWSTATEDNX_LLPRINT,
        DNX_SW_STATE_EXAMPLE__LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        "example[%d]->ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        example_info,
        EXAMPLE_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_ll_add_first(int unit, const int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        SW_STATE_FUNC_ADD_FIRST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_LL_ADD_FIRST(
        DNX_SW_STATE_EXAMPLE__LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        NULL,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_FIRST_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD_FIRST,
        DNX_SW_STATE_EXAMPLE__LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        "example[%d]->ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        example_info,
        EXAMPLE_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_ll_add_last(int unit, const int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        SW_STATE_FUNC_ADD_LAST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_LL_ADD_LAST(
        DNX_SW_STATE_EXAMPLE__LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        NULL,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_LAST_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD_LAST,
        DNX_SW_STATE_EXAMPLE__LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        "example[%d]->ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        example_info,
        EXAMPLE_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_ll_add_before(int unit, sw_state_ll_node_t node, const int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        SW_STATE_FUNC_ADD_BEFORE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_LL_ADD_BEFORE(
        DNX_SW_STATE_EXAMPLE__LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        node,
        NULL,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_BEFORE_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD_BEFORE,
        DNX_SW_STATE_EXAMPLE__LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        "example[%d]->ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        example_info,
        EXAMPLE_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_ll_add_after(int unit, sw_state_ll_node_t node, const int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        SW_STATE_FUNC_ADD_AFTER,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_LL_ADD_AFTER(
        DNX_SW_STATE_EXAMPLE__LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        node,
        NULL,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_AFTER_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD_AFTER,
        DNX_SW_STATE_EXAMPLE__LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->ll,
        "example[%d]->ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__LL,
        example_info,
        EXAMPLE_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_multihead_ll_create_empty(int unit, sw_state_ll_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        SW_STATE_FUNC_CREATE_EMPTY,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        DNX_SW_STATE_DIAG_LL_CREATE);

    SW_STATE_MULTIHEAD_LL_CREATE_EMPTY(
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
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
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        "example[%d]->multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        example_info,
        EXAMPLE_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL_CREATE,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_multihead_ll_nof_elements(int unit, uint32 ll_head_index, uint32 *nof_elements)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        SW_STATE_FUNC_NOF_ELEMENTS,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_NOF_ELEMENTS(
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        ll_head_index,
        nof_elements);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NOF_ELEMENTS_LOGGING,
        BSL_LS_SWSTATEDNX_LLNOF_ELEMENTS,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        "example[%d]->multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        example_info,
        EXAMPLE_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_multihead_ll_node_value(int unit, sw_state_ll_node_t node, int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        SW_STATE_FUNC_NODE_VALUE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_NODE_VALUE(
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        node,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NODE_VALUE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNODE_VALUE,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        "example[%d]->multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        example_info,
        EXAMPLE_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_multihead_ll_node_update(int unit, sw_state_ll_node_t node, const int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        SW_STATE_FUNC_NODE_UPDATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_NODE_UPDATE(
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        node,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NODE_UPDATE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNODE_UPDATE,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        "example[%d]->multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        example_info,
        EXAMPLE_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_multihead_ll_next_node(int unit, uint32 ll_head_index, sw_state_ll_node_t node, sw_state_ll_node_t *next_node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        SW_STATE_FUNC_NEXT_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_NEXT_NODE(
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        ll_head_index,
        node,
        next_node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NEXT_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNEXT_NODE,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        "example[%d]->multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        example_info,
        EXAMPLE_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_multihead_ll_previous_node(int unit, uint32 ll_head_index, sw_state_ll_node_t node, sw_state_ll_node_t *prev_node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        SW_STATE_FUNC_PREVIOUS_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_PREVIOUS_NODE(
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        ll_head_index,
        node,
        prev_node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_PREVIOUS_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLPREVIOUS_NODE,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        "example[%d]->multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        example_info,
        EXAMPLE_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_multihead_ll_remove_node(int unit, uint32 ll_head_index, sw_state_ll_node_t node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        SW_STATE_FUNC_REMOVE_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_REMOVE_NODE(
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        ll_head_index,
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_REMOVE_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLREMOVE_NODE,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        "example[%d]->multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        example_info,
        EXAMPLE_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_multihead_ll_get_last(int unit, uint32 ll_head_index, sw_state_ll_node_t *node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        SW_STATE_FUNC_GET_LAST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_GET_LAST(
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        ll_head_index,
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_GET_LAST_LOGGING,
        BSL_LS_SWSTATEDNX_LLGET_LAST,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        "example[%d]->multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        example_info,
        EXAMPLE_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_multihead_ll_get_first(int unit, uint32 ll_head_index, sw_state_ll_node_t *node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        SW_STATE_FUNC_GET_FIRST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_GET_FIRST(
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        ll_head_index,
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_GET_FIRST_LOGGING,
        BSL_LS_SWSTATEDNX_LLGET_FIRST,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        "example[%d]->multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        example_info,
        EXAMPLE_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_multihead_ll_print(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        SW_STATE_FUNC_PRINT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_PRINT(
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_PRINT_LOGGING,
        BSL_LS_SWSTATEDNX_LLPRINT,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        "example[%d]->multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        example_info,
        EXAMPLE_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_multihead_ll_add_first(int unit, uint32 ll_head_index, const int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        SW_STATE_FUNC_ADD_FIRST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_ADD_FIRST(
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        ll_head_index,
        NULL,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_FIRST_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD_FIRST,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        "example[%d]->multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        example_info,
        EXAMPLE_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_multihead_ll_add_last(int unit, uint32 ll_head_index, const int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        SW_STATE_FUNC_ADD_LAST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_ADD_LAST(
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        ll_head_index,
        NULL,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_LAST_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD_LAST,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        "example[%d]->multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        example_info,
        EXAMPLE_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_multihead_ll_add_before(int unit, uint32 ll_head_index, sw_state_ll_node_t node, const int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        SW_STATE_FUNC_ADD_BEFORE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_ADD_BEFORE(
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        ll_head_index,
        node,
        NULL,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_BEFORE_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD_BEFORE,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        "example[%d]->multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        example_info,
        EXAMPLE_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_multihead_ll_add_after(int unit, uint32 ll_head_index, sw_state_ll_node_t node, const int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        SW_STATE_FUNC_ADD_AFTER,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_ADD_AFTER(
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        ll_head_index,
        node,
        NULL,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_AFTER_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD_AFTER,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->multihead_ll,
        "example[%d]->multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__MULTIHEAD_LL,
        example_info,
        EXAMPLE_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_sorted_multihead_ll_create_empty(int unit, sw_state_ll_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        SW_STATE_FUNC_CREATE_EMPTY,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        DNX_SW_STATE_DIAG_LL_CREATE);

    SW_STATE_MULTIHEAD_LL_CREATE_EMPTY(
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_multihead_ll,
        init_info,
        SW_STATE_LL_MULTIHEAD | SW_STATE_LL_SORTED,
        uint32,
        uint32,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_CREATE_EMPTY_LOGGING,
        BSL_LS_SWSTATEDNX_LLCREATE_EMPTY,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_multihead_ll,
        "example[%d]->sorted_multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        example_info,
        EXAMPLE_SORTED_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL_CREATE,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_sorted_multihead_ll_nof_elements(int unit, uint32 ll_head_index, uint32 *nof_elements)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        SW_STATE_FUNC_NOF_ELEMENTS,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_NOF_ELEMENTS(
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_multihead_ll,
        ll_head_index,
        nof_elements);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NOF_ELEMENTS_LOGGING,
        BSL_LS_SWSTATEDNX_LLNOF_ELEMENTS,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_multihead_ll,
        "example[%d]->sorted_multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        example_info,
        EXAMPLE_SORTED_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_sorted_multihead_ll_node_value(int unit, sw_state_ll_node_t node, uint32 *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        SW_STATE_FUNC_NODE_VALUE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_NODE_VALUE(
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_multihead_ll,
        node,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NODE_VALUE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNODE_VALUE,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_multihead_ll,
        "example[%d]->sorted_multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        example_info,
        EXAMPLE_SORTED_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_sorted_multihead_ll_node_update(int unit, sw_state_ll_node_t node, const uint32 *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        SW_STATE_FUNC_NODE_UPDATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_NODE_UPDATE(
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_multihead_ll,
        node,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NODE_UPDATE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNODE_UPDATE,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_multihead_ll,
        "example[%d]->sorted_multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        example_info,
        EXAMPLE_SORTED_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_sorted_multihead_ll_next_node(int unit, uint32 ll_head_index, sw_state_ll_node_t node, sw_state_ll_node_t *next_node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        SW_STATE_FUNC_NEXT_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_NEXT_NODE(
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_multihead_ll,
        ll_head_index,
        node,
        next_node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NEXT_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNEXT_NODE,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_multihead_ll,
        "example[%d]->sorted_multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        example_info,
        EXAMPLE_SORTED_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_sorted_multihead_ll_previous_node(int unit, uint32 ll_head_index, sw_state_ll_node_t node, sw_state_ll_node_t *prev_node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        SW_STATE_FUNC_PREVIOUS_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_PREVIOUS_NODE(
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_multihead_ll,
        ll_head_index,
        node,
        prev_node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_PREVIOUS_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLPREVIOUS_NODE,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_multihead_ll,
        "example[%d]->sorted_multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        example_info,
        EXAMPLE_SORTED_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_sorted_multihead_ll_remove_node(int unit, uint32 ll_head_index, sw_state_ll_node_t node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        SW_STATE_FUNC_REMOVE_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_REMOVE_NODE(
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_multihead_ll,
        ll_head_index,
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_REMOVE_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLREMOVE_NODE,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_multihead_ll,
        "example[%d]->sorted_multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        example_info,
        EXAMPLE_SORTED_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_sorted_multihead_ll_get_last(int unit, uint32 ll_head_index, sw_state_ll_node_t *node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        SW_STATE_FUNC_GET_LAST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_GET_LAST(
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_multihead_ll,
        ll_head_index,
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_GET_LAST_LOGGING,
        BSL_LS_SWSTATEDNX_LLGET_LAST,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_multihead_ll,
        "example[%d]->sorted_multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        example_info,
        EXAMPLE_SORTED_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_sorted_multihead_ll_get_first(int unit, uint32 ll_head_index, sw_state_ll_node_t *node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        SW_STATE_FUNC_GET_FIRST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_GET_FIRST(
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_multihead_ll,
        ll_head_index,
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_GET_FIRST_LOGGING,
        BSL_LS_SWSTATEDNX_LLGET_FIRST,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_multihead_ll,
        "example[%d]->sorted_multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        example_info,
        EXAMPLE_SORTED_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_sorted_multihead_ll_print(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        SW_STATE_FUNC_PRINT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_PRINT(
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_multihead_ll);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_PRINT_LOGGING,
        BSL_LS_SWSTATEDNX_LLPRINT,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_multihead_ll,
        "example[%d]->sorted_multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        example_info,
        EXAMPLE_SORTED_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_sorted_multihead_ll_add(int unit, uint32 ll_head_index, const uint32 *key, const uint32 *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        SW_STATE_FUNC_ADD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_ADD(
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_multihead_ll,
        ll_head_index,
        key,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_multihead_ll,
        "example[%d]->sorted_multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        example_info,
        EXAMPLE_SORTED_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_sorted_multihead_ll_node_key(int unit, sw_state_ll_node_t node, uint32 *key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        SW_STATE_FUNC_NODE_KEY,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_NODE_KEY(
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_multihead_ll,
        node,
        key);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NODE_KEY_LOGGING,
        BSL_LS_SWSTATEDNX_LLNODE_KEY,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_multihead_ll,
        "example[%d]->sorted_multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        example_info,
        EXAMPLE_SORTED_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_sorted_multihead_ll_find(int unit, uint32 ll_head_index, sw_state_ll_node_t *node, const uint32 *key, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        SW_STATE_FUNC_FIND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_FIND(
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_multihead_ll,
        ll_head_index,
        node,
        key,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_FIND_LOGGING,
        BSL_LS_SWSTATEDNX_LLFIND,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_multihead_ll,
        "example[%d]->sorted_multihead_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_MULTIHEAD_LL,
        example_info,
        EXAMPLE_SORTED_MULTIHEAD_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_sorted_ll_create_empty(int unit, sw_state_ll_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        SW_STATE_FUNC_CREATE_EMPTY,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        DNX_SW_STATE_DIAG_LL_CREATE);

    SW_STATE_LL_CREATE_EMPTY(
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
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
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        "example[%d]->sorted_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        example_info,
        EXAMPLE_SORTED_LL_INFO,
        DNX_SW_STATE_DIAG_LL_CREATE,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_sorted_ll_nof_elements(int unit, uint32 *nof_elements)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        SW_STATE_FUNC_NOF_ELEMENTS,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_LL_NOF_ELEMENTS(
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        nof_elements);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NOF_ELEMENTS_LOGGING,
        BSL_LS_SWSTATEDNX_LLNOF_ELEMENTS,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        "example[%d]->sorted_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        example_info,
        EXAMPLE_SORTED_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_sorted_ll_node_value(int unit, sw_state_ll_node_t node, uint8 *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        SW_STATE_FUNC_NODE_VALUE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_LL_NODE_VALUE(
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        node,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NODE_VALUE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNODE_VALUE,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        "example[%d]->sorted_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        example_info,
        EXAMPLE_SORTED_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_sorted_ll_node_update(int unit, sw_state_ll_node_t node, const uint8 *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        SW_STATE_FUNC_NODE_UPDATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_LL_NODE_UPDATE(
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        node,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NODE_UPDATE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNODE_UPDATE,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        "example[%d]->sorted_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        example_info,
        EXAMPLE_SORTED_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_sorted_ll_next_node(int unit, sw_state_ll_node_t node, sw_state_ll_node_t *next_node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        SW_STATE_FUNC_NEXT_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_LL_NEXT_NODE(
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        node,
        next_node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NEXT_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNEXT_NODE,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        "example[%d]->sorted_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        example_info,
        EXAMPLE_SORTED_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_sorted_ll_previous_node(int unit, sw_state_ll_node_t node, sw_state_ll_node_t *prev_node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        SW_STATE_FUNC_PREVIOUS_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_LL_PREVIOUS_NODE(
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        node,
        prev_node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_PREVIOUS_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLPREVIOUS_NODE,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        "example[%d]->sorted_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        example_info,
        EXAMPLE_SORTED_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_sorted_ll_remove_node(int unit, sw_state_ll_node_t node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        SW_STATE_FUNC_REMOVE_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_LL_REMOVE_NODE(
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_REMOVE_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLREMOVE_NODE,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        "example[%d]->sorted_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        example_info,
        EXAMPLE_SORTED_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_sorted_ll_get_last(int unit, sw_state_ll_node_t *node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        SW_STATE_FUNC_GET_LAST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_LL_GET_LAST(
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_GET_LAST_LOGGING,
        BSL_LS_SWSTATEDNX_LLGET_LAST,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        "example[%d]->sorted_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        example_info,
        EXAMPLE_SORTED_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_sorted_ll_get_first(int unit, sw_state_ll_node_t *node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        SW_STATE_FUNC_GET_FIRST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_LL_GET_FIRST(
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_GET_FIRST_LOGGING,
        BSL_LS_SWSTATEDNX_LLGET_FIRST,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        "example[%d]->sorted_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        example_info,
        EXAMPLE_SORTED_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_sorted_ll_print(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        SW_STATE_FUNC_PRINT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_LL_PRINT(
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_PRINT_LOGGING,
        BSL_LS_SWSTATEDNX_LLPRINT,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        "example[%d]->sorted_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        example_info,
        EXAMPLE_SORTED_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_sorted_ll_add(int unit, const uint8 *key, const uint8 *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        SW_STATE_FUNC_ADD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_LL_ADD(
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        key,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        "example[%d]->sorted_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        example_info,
        EXAMPLE_SORTED_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_sorted_ll_node_key(int unit, sw_state_ll_node_t node, uint8 *key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        SW_STATE_FUNC_NODE_KEY,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_LL_NODE_KEY(
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        node,
        key);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NODE_KEY_LOGGING,
        BSL_LS_SWSTATEDNX_LLNODE_KEY,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        "example[%d]->sorted_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        example_info,
        EXAMPLE_SORTED_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_sorted_ll_find(int unit, sw_state_ll_node_t *node, const uint8 *key, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        SW_STATE_FUNC_FIND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_LL_FIND(
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        node,
        key,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_FIND_LOGGING,
        BSL_LS_SWSTATEDNX_LLFIND,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sorted_ll,
        "example[%d]->sorted_ll",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__SORTED_LL,
        example_info,
        EXAMPLE_SORTED_LL_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bt_create_empty(int unit, sw_state_bt_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        SW_STATE_FUNC_CREATE_EMPTY,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        DNX_SW_STATE_DIAG_BT_CREATE);

    SW_STATE_BT_CREATE_EMPTY(
        DNX_SW_STATE_EXAMPLE__BT,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        init_info,
        int,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BT_CREATE_EMPTY_LOGGING,
        BSL_LS_SWSTATEDNX_BTCREATE_EMPTY,
        DNX_SW_STATE_EXAMPLE__BT,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        "example[%d]->bt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        example_info,
        EXAMPLE_BT_INFO,
        DNX_SW_STATE_DIAG_BT_CREATE,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bt_destroy(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        SW_STATE_FUNC_DESTROY,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        DNX_SW_STATE_DIAG_BT);

    SW_STATE_BT_DESTROY(
        DNX_SW_STATE_EXAMPLE__BT,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BT_DESTROY_LOGGING,
        BSL_LS_SWSTATEDNX_BTDESTROY,
        DNX_SW_STATE_EXAMPLE__BT,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        "example[%d]->bt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        example_info,
        EXAMPLE_BT_INFO,
        DNX_SW_STATE_DIAG_BT,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bt_nof_elements(int unit, uint32 *nof_elements)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        SW_STATE_FUNC_NOF_ELEMENTS,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        DNX_SW_STATE_DIAG_BT);

    SW_STATE_BT_NOF_ELEMENTS(
        DNX_SW_STATE_EXAMPLE__BT,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        nof_elements);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BT_NOF_ELEMENTS_LOGGING,
        BSL_LS_SWSTATEDNX_BTNOF_ELEMENTS,
        DNX_SW_STATE_EXAMPLE__BT,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        "example[%d]->bt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        example_info,
        EXAMPLE_BT_INFO,
        DNX_SW_STATE_DIAG_BT,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bt_node_value(int unit, sw_state_bt_node_t node, int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        SW_STATE_FUNC_NODE_VALUE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        DNX_SW_STATE_DIAG_BT);

    SW_STATE_BT_NODE_VALUE(
        DNX_SW_STATE_EXAMPLE__BT,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        node,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BT_NODE_VALUE_LOGGING,
        BSL_LS_SWSTATEDNX_BTNODE_VALUE,
        DNX_SW_STATE_EXAMPLE__BT,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        "example[%d]->bt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        example_info,
        EXAMPLE_BT_INFO,
        DNX_SW_STATE_DIAG_BT,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bt_node_update(int unit, sw_state_bt_node_t node, const int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        SW_STATE_FUNC_NODE_UPDATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        DNX_SW_STATE_DIAG_BT);

    SW_STATE_BT_NODE_UPDATE(
        DNX_SW_STATE_EXAMPLE__BT,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        node,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BT_NODE_UPDATE_LOGGING,
        BSL_LS_SWSTATEDNX_BTNODE_UPDATE,
        DNX_SW_STATE_EXAMPLE__BT,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        "example[%d]->bt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        example_info,
        EXAMPLE_BT_INFO,
        DNX_SW_STATE_DIAG_BT,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bt_node_free(int unit, sw_state_bt_node_t node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        SW_STATE_FUNC_NODE_FREE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        DNX_SW_STATE_DIAG_BT);

    SW_STATE_BT_NODE_FREE(
        DNX_SW_STATE_EXAMPLE__BT,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BT_NODE_FREE_LOGGING,
        BSL_LS_SWSTATEDNX_BTNODE_FREE,
        DNX_SW_STATE_EXAMPLE__BT,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        "example[%d]->bt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        example_info,
        EXAMPLE_BT_INFO,
        DNX_SW_STATE_DIAG_BT,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bt_get_root(int unit, sw_state_bt_node_t *root)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        SW_STATE_FUNC_GET_ROOT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        DNX_SW_STATE_DIAG_BT);

    SW_STATE_BT_GET_ROOT(
        DNX_SW_STATE_EXAMPLE__BT,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        root);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BT_GET_ROOT_LOGGING,
        BSL_LS_SWSTATEDNX_BTGET_ROOT,
        DNX_SW_STATE_EXAMPLE__BT,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        "example[%d]->bt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        example_info,
        EXAMPLE_BT_INFO,
        DNX_SW_STATE_DIAG_BT,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bt_get_parent(int unit, sw_state_bt_node_t node, sw_state_bt_node_t *node_parent)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        SW_STATE_FUNC_GET_PARENT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        DNX_SW_STATE_DIAG_BT);

    SW_STATE_BT_GET_PARENT(
        DNX_SW_STATE_EXAMPLE__BT,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        node,
        node_parent);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BT_GET_PARENT_LOGGING,
        BSL_LS_SWSTATEDNX_BTGET_PARENT,
        DNX_SW_STATE_EXAMPLE__BT,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        "example[%d]->bt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        example_info,
        EXAMPLE_BT_INFO,
        DNX_SW_STATE_DIAG_BT,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bt_get_left_child(int unit, sw_state_bt_node_t node, sw_state_bt_node_t *node_left)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        SW_STATE_FUNC_GET_LEFT_CHILD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        DNX_SW_STATE_DIAG_BT);

    SW_STATE_BT_GET_LEFT_CHILD(
        DNX_SW_STATE_EXAMPLE__BT,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        node,
        node_left);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BT_GET_LEFT_CHILD_LOGGING,
        BSL_LS_SWSTATEDNX_BTGET_LEFT_CHILD,
        DNX_SW_STATE_EXAMPLE__BT,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        "example[%d]->bt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        example_info,
        EXAMPLE_BT_INFO,
        DNX_SW_STATE_DIAG_BT,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bt_get_right_child(int unit, sw_state_bt_node_t node, sw_state_bt_node_t *node_right)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        SW_STATE_FUNC_GET_RIGHT_CHILD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        DNX_SW_STATE_DIAG_BT);

    SW_STATE_BT_GET_RIGHT_CHILD(
        DNX_SW_STATE_EXAMPLE__BT,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        node,
        node_right);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BT_GET_RIGHT_CHILD_LOGGING,
        BSL_LS_SWSTATEDNX_BTGET_RIGHT_CHILD,
        DNX_SW_STATE_EXAMPLE__BT,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        "example[%d]->bt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        example_info,
        EXAMPLE_BT_INFO,
        DNX_SW_STATE_DIAG_BT,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bt_add_root(int unit, const int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        SW_STATE_FUNC_ADD_ROOT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        DNX_SW_STATE_DIAG_BT);

    SW_STATE_BT_ADD_ROOT(
        DNX_SW_STATE_EXAMPLE__BT,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BT_ADD_ROOT_LOGGING,
        BSL_LS_SWSTATEDNX_BTADD_ROOT,
        DNX_SW_STATE_EXAMPLE__BT,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        "example[%d]->bt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        example_info,
        EXAMPLE_BT_INFO,
        DNX_SW_STATE_DIAG_BT,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bt_add_left_child(int unit, sw_state_bt_node_t node, const int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        SW_STATE_FUNC_ADD_LEFT_CHILD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        DNX_SW_STATE_DIAG_BT);

    SW_STATE_BT_ADD_LEFT_CHILD(
        DNX_SW_STATE_EXAMPLE__BT,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        node,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BT_ADD_LEFT_CHILD_LOGGING,
        BSL_LS_SWSTATEDNX_BTADD_LEFT_CHILD,
        DNX_SW_STATE_EXAMPLE__BT,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        "example[%d]->bt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        example_info,
        EXAMPLE_BT_INFO,
        DNX_SW_STATE_DIAG_BT,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bt_add_right_child(int unit, sw_state_bt_node_t node, const int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        SW_STATE_FUNC_ADD_RIGHT_CHILD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        DNX_SW_STATE_DIAG_BT);

    SW_STATE_BT_ADD_RIGHT_CHILD(
        DNX_SW_STATE_EXAMPLE__BT,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        node,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BT_ADD_RIGHT_CHILD_LOGGING,
        BSL_LS_SWSTATEDNX_BTADD_RIGHT_CHILD,
        DNX_SW_STATE_EXAMPLE__BT,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        "example[%d]->bt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        example_info,
        EXAMPLE_BT_INFO,
        DNX_SW_STATE_DIAG_BT,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bt_set_parent(int unit, sw_state_bt_node_t node, sw_state_bt_node_t node_parent)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        SW_STATE_FUNC_SET_PARENT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        DNX_SW_STATE_DIAG_BT);

    SW_STATE_BT_SET_PARENT(
        DNX_SW_STATE_EXAMPLE__BT,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        node,
        node_parent);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BT_SET_PARENT_LOGGING,
        BSL_LS_SWSTATEDNX_BTSET_PARENT,
        DNX_SW_STATE_EXAMPLE__BT,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        "example[%d]->bt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        example_info,
        EXAMPLE_BT_INFO,
        DNX_SW_STATE_DIAG_BT,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bt_set_left_child(int unit, sw_state_bt_node_t node, sw_state_bt_node_t node_left)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        SW_STATE_FUNC_SET_LEFT_CHILD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        DNX_SW_STATE_DIAG_BT);

    SW_STATE_BT_SET_LEFT_CHILD(
        DNX_SW_STATE_EXAMPLE__BT,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        node,
        node_left);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BT_SET_LEFT_CHILD_LOGGING,
        BSL_LS_SWSTATEDNX_BTSET_LEFT_CHILD,
        DNX_SW_STATE_EXAMPLE__BT,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        "example[%d]->bt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        example_info,
        EXAMPLE_BT_INFO,
        DNX_SW_STATE_DIAG_BT,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bt_set_right_child(int unit, sw_state_bt_node_t node, sw_state_bt_node_t node_right)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        SW_STATE_FUNC_SET_RIGHT_CHILD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        DNX_SW_STATE_DIAG_BT);

    SW_STATE_BT_SET_RIGHT_CHILD(
        DNX_SW_STATE_EXAMPLE__BT,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        node,
        node_right);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BT_SET_RIGHT_CHILD_LOGGING,
        BSL_LS_SWSTATEDNX_BTSET_RIGHT_CHILD,
        DNX_SW_STATE_EXAMPLE__BT,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        "example[%d]->bt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        example_info,
        EXAMPLE_BT_INFO,
        DNX_SW_STATE_DIAG_BT,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_bt_print(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        SW_STATE_FUNC_PRINT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        DNX_SW_STATE_DIAG_BT);

    SW_STATE_BT_PRINT(
        DNX_SW_STATE_EXAMPLE__BT,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BT_PRINT_LOGGING,
        BSL_LS_SWSTATEDNX_BTPRINT,
        DNX_SW_STATE_EXAMPLE__BT,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->bt,
        "example[%d]->bt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__BT,
        example_info,
        EXAMPLE_BT_INFO,
        DNX_SW_STATE_DIAG_BT,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_htb_create(int unit, uint32 htb_idx_0, sw_state_htbl_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        htb_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB,
        DNX_SW_STATE_DIAG_HTB_CREATE);

    SW_STATE_HTB_CREATE(
        DNX_SW_STATE_EXAMPLE__HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb[htb_idx_0],
        init_info,
        int,
        int,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCREATE,
        DNX_SW_STATE_EXAMPLE__HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb[htb_idx_0],
        "example[%d]->htb[%d]",
        unit, htb_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB,
        example_info,
        EXAMPLE_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB_CREATE,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_htb_find(int unit, uint32 htb_idx_0, const int *key, int *value, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB,
        SW_STATE_FUNC_FIND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        htb_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_FIND(
        DNX_SW_STATE_EXAMPLE__HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb[htb_idx_0],
        key,
        value,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_FIND_LOGGING,
        BSL_LS_SWSTATEDNX_HTBFIND,
        DNX_SW_STATE_EXAMPLE__HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb[htb_idx_0],
        "example[%d]->htb[%d]",
        unit, htb_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB,
        example_info,
        EXAMPLE_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_htb_insert(int unit, uint32 htb_idx_0, const int *key, const int *value, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB,
        SW_STATE_FUNC_INSERT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        htb_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_INSERT(
        DNX_SW_STATE_EXAMPLE__HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb[htb_idx_0],
        key,
        value,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_INSERT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBINSERT,
        DNX_SW_STATE_EXAMPLE__HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb[htb_idx_0],
        "example[%d]->htb[%d]",
        unit, htb_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB,
        example_info,
        EXAMPLE_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_htb_get_next(int unit, uint32 htb_idx_0, SW_STATE_HASH_TABLE_ITER *iter, const int *key, const int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB,
        SW_STATE_FUNC_GET_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        htb_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_GET_NEXT(
        DNX_SW_STATE_EXAMPLE__HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb[htb_idx_0],
        iter,
        key,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_GET_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBGET_NEXT,
        DNX_SW_STATE_EXAMPLE__HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb[htb_idx_0],
        "example[%d]->htb[%d]",
        unit, htb_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB,
        example_info,
        EXAMPLE_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_htb_clear(int unit, uint32 htb_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB,
        SW_STATE_FUNC_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        htb_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_CLEAR(
        DNX_SW_STATE_EXAMPLE__HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb[htb_idx_0]);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCLEAR,
        DNX_SW_STATE_EXAMPLE__HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb[htb_idx_0],
        "example[%d]->htb[%d]",
        unit, htb_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB,
        example_info,
        EXAMPLE_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_htb_delete(int unit, uint32 htb_idx_0, const int *key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB,
        SW_STATE_FUNC_DELETE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        htb_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_DELETE(
        DNX_SW_STATE_EXAMPLE__HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb[htb_idx_0],
        key);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_DELETE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBDELETE,
        DNX_SW_STATE_EXAMPLE__HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb[htb_idx_0],
        "example[%d]->htb[%d]",
        unit, htb_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB,
        example_info,
        EXAMPLE_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_htb_delete_all(int unit, uint32 htb_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB,
        SW_STATE_FUNC_DELETE_ALL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        htb_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_DELETE_ALL(
        DNX_SW_STATE_EXAMPLE__HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb[htb_idx_0]);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_DELETE_ALL_LOGGING,
        BSL_LS_SWSTATEDNX_HTBDELETE_ALL,
        DNX_SW_STATE_EXAMPLE__HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb[htb_idx_0],
        "example[%d]->htb[%d]",
        unit, htb_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB,
        example_info,
        EXAMPLE_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_htb_print(int unit, uint32 htb_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB,
        SW_STATE_FUNC_PRINT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        htb_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_PRINT(
        DNX_SW_STATE_EXAMPLE__HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb[htb_idx_0]);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_PRINT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBPRINT,
        DNX_SW_STATE_EXAMPLE__HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb[htb_idx_0],
        "example[%d]->htb[%d]",
        unit, htb_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB,
        example_info,
        EXAMPLE_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_htb_rh_create(int unit, sw_state_htb_create_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        DNX_SW_STATE_DIAG_HTB_RH_CREATE);

    SW_STATE_HTB_RH_CREATE(
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_rh,
        init_info,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_RH_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_HTB_RHCREATE,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_rh,
        "example[%d]->htb_rh",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        example_info,
        EXAMPLE_HTB_RH_INFO,
        DNX_SW_STATE_DIAG_HTB_RH_CREATE,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_htb_rh_find(int unit, uint32 table_idx, const int *key, int *value, uint32 *offset)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        SW_STATE_FUNC_FIND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        DNX_SW_STATE_DIAG_HTB_RH);

    SW_STATE_HTB_RH_FIND(
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_rh,
        table_idx,
        key,
        value,
        offset);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_RH_FIND_LOGGING,
        BSL_LS_SWSTATEDNX_HTB_RHFIND,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_rh,
        "example[%d]->htb_rh",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        example_info,
        EXAMPLE_HTB_RH_INFO,
        DNX_SW_STATE_DIAG_HTB_RH,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_htb_rh_insert(int unit, uint32 table_idx, const int *key, const int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        SW_STATE_FUNC_INSERT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        DNX_SW_STATE_DIAG_HTB_RH);

    SW_STATE_HTB_RH_INSERT(
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_rh,
        table_idx,
        key,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_RH_INSERT_LOGGING,
        BSL_LS_SWSTATEDNX_HTB_RHINSERT,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_rh,
        "example[%d]->htb_rh",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        example_info,
        EXAMPLE_HTB_RH_INFO,
        DNX_SW_STATE_DIAG_HTB_RH,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_htb_rh_replace(int unit, uint32 table_idx, const int *key, const int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        SW_STATE_FUNC_REPLACE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        DNX_SW_STATE_DIAG_HTB_RH);

    SW_STATE_HTB_RH_REPLACE(
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_rh,
        table_idx,
        key,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_RH_REPLACE_LOGGING,
        BSL_LS_SWSTATEDNX_HTB_RHREPLACE,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_rh,
        "example[%d]->htb_rh",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        example_info,
        EXAMPLE_HTB_RH_INFO,
        DNX_SW_STATE_DIAG_HTB_RH,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_htb_rh_delete(int unit, uint32 table_idx, const int *key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        SW_STATE_FUNC_DELETE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        DNX_SW_STATE_DIAG_HTB_RH);

    SW_STATE_HTB_RH_DELETE(
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_rh,
        table_idx,
        key);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_RH_DELETE_LOGGING,
        BSL_LS_SWSTATEDNX_HTB_RHDELETE,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_rh,
        "example[%d]->htb_rh",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        example_info,
        EXAMPLE_HTB_RH_INFO,
        DNX_SW_STATE_DIAG_HTB_RH,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_htb_rh_delete_all(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        SW_STATE_FUNC_DELETE_ALL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        DNX_SW_STATE_DIAG_HTB_RH);

    SW_STATE_HTB_RH_DELETE_ALL(
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_rh);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_RH_DELETE_ALL_LOGGING,
        BSL_LS_SWSTATEDNX_HTB_RHDELETE_ALL,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_rh,
        "example[%d]->htb_rh",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        example_info,
        EXAMPLE_HTB_RH_INFO,
        DNX_SW_STATE_DIAG_HTB_RH,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_htb_rh_print(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        SW_STATE_FUNC_PRINT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        DNX_SW_STATE_DIAG_HTB_RH);

    SW_STATE_HTB_RH_PRINT(
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_rh);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_RH_PRINT_LOGGING,
        BSL_LS_SWSTATEDNX_HTB_RHPRINT,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_rh,
        "example[%d]->htb_rh",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_RH,
        example_info,
        EXAMPLE_HTB_RH_INFO,
        DNX_SW_STATE_DIAG_HTB_RH,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_htb_arr_create(int unit, uint32 htb_arr_idx_0, sw_state_htbl_init_info_t *init_info, int key_size, int value_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        htb_arr_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        DNX_SW_STATE_DIAG_HTB_CREATE);

    SW_STATE_HTB_CREATE_WITH_SIZE(
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_arr[htb_arr_idx_0],
        init_info,
        key_size,
        value_size,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCREATE,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_arr[htb_arr_idx_0],
        "example[%d]->htb_arr[%d]",
        unit, htb_arr_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        example_info,
        EXAMPLE_HTB_ARR_INFO,
        DNX_SW_STATE_DIAG_HTB_CREATE,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_htb_arr_find(int unit, uint32 htb_arr_idx_0, const void *key, void *value, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        SW_STATE_FUNC_FIND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        htb_arr_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_FIND(
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_arr[htb_arr_idx_0],
        key,
        value,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_FIND_LOGGING,
        BSL_LS_SWSTATEDNX_HTBFIND,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_arr[htb_arr_idx_0],
        "example[%d]->htb_arr[%d]",
        unit, htb_arr_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        example_info,
        EXAMPLE_HTB_ARR_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_htb_arr_insert(int unit, uint32 htb_arr_idx_0, const void *key, const void *value, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        SW_STATE_FUNC_INSERT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        htb_arr_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_INSERT(
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_arr[htb_arr_idx_0],
        key,
        value,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_INSERT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBINSERT,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_arr[htb_arr_idx_0],
        "example[%d]->htb_arr[%d]",
        unit, htb_arr_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        example_info,
        EXAMPLE_HTB_ARR_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_htb_arr_get_next(int unit, uint32 htb_arr_idx_0, SW_STATE_HASH_TABLE_ITER *iter, const void *key, const void *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        SW_STATE_FUNC_GET_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        htb_arr_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_GET_NEXT(
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_arr[htb_arr_idx_0],
        iter,
        key,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_GET_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBGET_NEXT,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_arr[htb_arr_idx_0],
        "example[%d]->htb_arr[%d]",
        unit, htb_arr_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        example_info,
        EXAMPLE_HTB_ARR_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_htb_arr_clear(int unit, uint32 htb_arr_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        SW_STATE_FUNC_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        htb_arr_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_CLEAR(
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_arr[htb_arr_idx_0]);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCLEAR,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_arr[htb_arr_idx_0],
        "example[%d]->htb_arr[%d]",
        unit, htb_arr_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        example_info,
        EXAMPLE_HTB_ARR_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_htb_arr_delete(int unit, uint32 htb_arr_idx_0, const void *key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        SW_STATE_FUNC_DELETE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        htb_arr_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_DELETE(
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_arr[htb_arr_idx_0],
        key);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_DELETE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBDELETE,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_arr[htb_arr_idx_0],
        "example[%d]->htb_arr[%d]",
        unit, htb_arr_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        example_info,
        EXAMPLE_HTB_ARR_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_htb_arr_delete_all(int unit, uint32 htb_arr_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        SW_STATE_FUNC_DELETE_ALL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        htb_arr_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_DELETE_ALL(
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_arr[htb_arr_idx_0]);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_DELETE_ALL_LOGGING,
        BSL_LS_SWSTATEDNX_HTBDELETE_ALL,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_arr[htb_arr_idx_0],
        "example[%d]->htb_arr[%d]",
        unit, htb_arr_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        example_info,
        EXAMPLE_HTB_ARR_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_htb_arr_print(int unit, uint32 htb_arr_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        SW_STATE_FUNC_PRINT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        htb_arr_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_PRINT(
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_arr[htb_arr_idx_0]);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_PRINT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBPRINT,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->htb_arr[htb_arr_idx_0],
        "example[%d]->htb_arr[%d]",
        unit, htb_arr_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__HTB_ARR,
        example_info,
        EXAMPLE_HTB_ARR_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_index_htb_create(int unit, sw_state_idx_htbl_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        DNX_SW_STATE_DIAG_INDEX_HTB_CREATE);

    SW_STATE_INDEX_HTB_CREATE(
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        init_info,
        int,
        uint32,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBCREATE,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        "example[%d]->index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        example_info,
        EXAMPLE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB_CREATE,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_index_htb_find(int unit, const int *key, uint32 *data_index, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        SW_STATE_FUNC_FIND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_FIND(
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        key,
        data_index,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_FIND_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBFIND,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        "example[%d]->index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        example_info,
        EXAMPLE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_index_htb_insert(int unit, const int *key, const uint32 *data_index, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        SW_STATE_FUNC_INSERT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_INSERT(
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        key,
        data_index,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_INSERT_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBINSERT,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        "example[%d]->index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        example_info,
        EXAMPLE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_index_htb_get_next(int unit, SW_STATE_INDEX_HASH_TABLE_ITER *iter, const int *key, const uint32 *data_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        SW_STATE_FUNC_GET_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_GET_NEXT(
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        iter,
        key,
        data_index);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_GET_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBGET_NEXT,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        "example[%d]->index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        example_info,
        EXAMPLE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_index_htb_clear(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        SW_STATE_FUNC_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_CLEAR(
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBCLEAR,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        "example[%d]->index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        example_info,
        EXAMPLE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_index_htb_delete(int unit, const int *key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        SW_STATE_FUNC_DELETE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_DELETE(
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        key);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_DELETE_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBDELETE,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        "example[%d]->index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        example_info,
        EXAMPLE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_index_htb_delete_all(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        SW_STATE_FUNC_DELETE_ALL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_DELETE_ALL(
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_DELETE_ALL_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBDELETE_ALL,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        "example[%d]->index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        example_info,
        EXAMPLE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_index_htb_print(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        SW_STATE_FUNC_PRINT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_PRINT(
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_PRINT_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBPRINT,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        "example[%d]->index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        example_info,
        EXAMPLE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_index_htb_insert_at_index(int unit, const int *key, uint32 data_idx, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        SW_STATE_FUNC_INSERT_AT_INDEX,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_INSERT_AT_INDEX(
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        key,
        data_idx,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_INSERT_AT_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBINSERT_AT_INDEX,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        "example[%d]->index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        example_info,
        EXAMPLE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_index_htb_delete_by_index(int unit, uint32 data_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        SW_STATE_FUNC_DELETE_BY_INDEX,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_DELETE_BY_INDEX(
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        data_index);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_DELETE_BY_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBDELETE_BY_INDEX,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        "example[%d]->index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        example_info,
        EXAMPLE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_index_htb_get_by_index(int unit, uint32 data_index, int *key, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        SW_STATE_FUNC_GET_BY_INDEX,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_GET_BY_INDEX(
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        data_index,
        key,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_GET_BY_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBGET_BY_INDEX,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->index_htb,
        "example[%d]->index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__INDEX_HTB,
        example_info,
        EXAMPLE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_occ_create(int unit, sw_state_occ_bitmap_init_info_t * init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC,
        DNX_SW_STATE_DIAG_OCC_BM_CREATE);

    SW_STATE_OCC_BM_CREATE(
        DNX_SW_STATE_EXAMPLE__OCC,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ,
        init_info,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMCREATE,
        DNX_SW_STATE_EXAMPLE__OCC,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ,
        "example[%d]->occ",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC,
        example_info,
        EXAMPLE_OCC_INFO,
        DNX_SW_STATE_DIAG_OCC_BM_CREATE,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_occ_get_next(int unit, uint32 *place, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC,
        SW_STATE_FUNC_GET_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC,
        DNX_SW_STATE_DIAG_OCC_BM);

    SW_STATE_OCC_BM_GET_NEXT(
        DNX_SW_STATE_EXAMPLE__OCC,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ,
        place,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_GET_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMGET_NEXT,
        DNX_SW_STATE_EXAMPLE__OCC,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ,
        "example[%d]->occ",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC,
        example_info,
        EXAMPLE_OCC_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_occ_get_next_in_range(int unit, uint32 start, uint32 end, uint8 forward, uint8 val, uint32 *place, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC,
        SW_STATE_FUNC_GET_NEXT_IN_RANGE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC,
        DNX_SW_STATE_DIAG_OCC_BM);

    SW_STATE_OCC_BM_GET_NEXT_IN_RANGE(
        DNX_SW_STATE_EXAMPLE__OCC,
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
        DNX_SW_STATE_EXAMPLE__OCC,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ,
        "example[%d]->occ",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC,
        example_info,
        EXAMPLE_OCC_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_occ_status_set(int unit, uint32 place, uint8 occupied)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC,
        SW_STATE_FUNC_STATUS_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC,
        DNX_SW_STATE_DIAG_OCC_BM);

    SW_STATE_OCC_BM_STATUS_SET(
        DNX_SW_STATE_EXAMPLE__OCC,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ,
        place,
        occupied);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_STATUS_SET_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMSTATUS_SET,
        DNX_SW_STATE_EXAMPLE__OCC,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ,
        "example[%d]->occ",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC,
        example_info,
        EXAMPLE_OCC_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_occ_is_occupied(int unit, uint32 place, uint8 *occupied)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC,
        SW_STATE_FUNC_IS_OCCUPIED,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC,
        DNX_SW_STATE_DIAG_OCC_BM);

    SW_STATE_OCC_BM_IS_OCCUPIED(
        DNX_SW_STATE_EXAMPLE__OCC,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ,
        place,
        occupied);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_IS_OCCUPIED_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMIS_OCCUPIED,
        DNX_SW_STATE_EXAMPLE__OCC,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ,
        "example[%d]->occ",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC,
        example_info,
        EXAMPLE_OCC_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_occ_alloc_next(int unit, uint32 *place, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC,
        SW_STATE_FUNC_ALLOC_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC,
        DNX_SW_STATE_DIAG_OCC_BM);

    SW_STATE_OCC_BM_ALLOC_NEXT(
        DNX_SW_STATE_EXAMPLE__OCC,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ,
        place,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_ALLOC_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMALLOC_NEXT,
        DNX_SW_STATE_EXAMPLE__OCC,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ,
        "example[%d]->occ",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC,
        example_info,
        EXAMPLE_OCC_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_occ_clear(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC,
        SW_STATE_FUNC_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC,
        DNX_SW_STATE_DIAG_OCC_BM);

    SW_STATE_OCC_BM_CLEAR(
        DNX_SW_STATE_EXAMPLE__OCC,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMCLEAR,
        DNX_SW_STATE_EXAMPLE__OCC,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ,
        "example[%d]->occ",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC,
        example_info,
        EXAMPLE_OCC_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_occ_print(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC,
        SW_STATE_FUNC_PRINT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC,
        DNX_SW_STATE_DIAG_OCC_BM);

    SW_STATE_OCC_BM_PRINT(
        DNX_SW_STATE_EXAMPLE__OCC,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_PRINT_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMPRINT,
        DNX_SW_STATE_EXAMPLE__OCC,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ,
        "example[%d]->occ",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC,
        example_info,
        EXAMPLE_OCC_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_occ2_create(int unit, sw_state_occ_bitmap_init_info_t * init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC2,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC2,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC2,
        DNX_SW_STATE_DIAG_OCC_BM_CREATE);

    SW_STATE_OCC_BM_CREATE(
        DNX_SW_STATE_EXAMPLE__OCC2,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ2,
        init_info,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMCREATE,
        DNX_SW_STATE_EXAMPLE__OCC2,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ2,
        "example[%d]->occ2",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC2,
        example_info,
        EXAMPLE_OCC2_INFO,
        DNX_SW_STATE_DIAG_OCC_BM_CREATE,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_occ2_get_next(int unit, uint32 *place, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC2,
        SW_STATE_FUNC_GET_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC2,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC2,
        DNX_SW_STATE_DIAG_OCC_BM);

    SW_STATE_OCC_BM_GET_NEXT(
        DNX_SW_STATE_EXAMPLE__OCC2,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ2,
        place,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_GET_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMGET_NEXT,
        DNX_SW_STATE_EXAMPLE__OCC2,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ2,
        "example[%d]->occ2",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC2,
        example_info,
        EXAMPLE_OCC2_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_occ2_get_next_in_range(int unit, uint32 start, uint32 end, uint8 forward, uint8 val, uint32 *place, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC2,
        SW_STATE_FUNC_GET_NEXT_IN_RANGE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC2,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC2,
        DNX_SW_STATE_DIAG_OCC_BM);

    SW_STATE_OCC_BM_GET_NEXT_IN_RANGE(
        DNX_SW_STATE_EXAMPLE__OCC2,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ2,
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
        DNX_SW_STATE_EXAMPLE__OCC2,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ2,
        "example[%d]->occ2",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC2,
        example_info,
        EXAMPLE_OCC2_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_occ2_status_set(int unit, uint32 place, uint8 occupied)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC2,
        SW_STATE_FUNC_STATUS_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC2,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC2,
        DNX_SW_STATE_DIAG_OCC_BM);

    SW_STATE_OCC_BM_STATUS_SET(
        DNX_SW_STATE_EXAMPLE__OCC2,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ2,
        place,
        occupied);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_STATUS_SET_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMSTATUS_SET,
        DNX_SW_STATE_EXAMPLE__OCC2,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ2,
        "example[%d]->occ2",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC2,
        example_info,
        EXAMPLE_OCC2_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_occ2_is_occupied(int unit, uint32 place, uint8 *occupied)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC2,
        SW_STATE_FUNC_IS_OCCUPIED,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC2,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC2,
        DNX_SW_STATE_DIAG_OCC_BM);

    SW_STATE_OCC_BM_IS_OCCUPIED(
        DNX_SW_STATE_EXAMPLE__OCC2,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ2,
        place,
        occupied);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_IS_OCCUPIED_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMIS_OCCUPIED,
        DNX_SW_STATE_EXAMPLE__OCC2,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ2,
        "example[%d]->occ2",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC2,
        example_info,
        EXAMPLE_OCC2_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_occ2_alloc_next(int unit, uint32 *place, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC2,
        SW_STATE_FUNC_ALLOC_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC2,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC2,
        DNX_SW_STATE_DIAG_OCC_BM);

    SW_STATE_OCC_BM_ALLOC_NEXT(
        DNX_SW_STATE_EXAMPLE__OCC2,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ2,
        place,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_ALLOC_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMALLOC_NEXT,
        DNX_SW_STATE_EXAMPLE__OCC2,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ2,
        "example[%d]->occ2",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC2,
        example_info,
        EXAMPLE_OCC2_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_occ2_clear(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC2,
        SW_STATE_FUNC_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC2,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC2,
        DNX_SW_STATE_DIAG_OCC_BM);

    SW_STATE_OCC_BM_CLEAR(
        DNX_SW_STATE_EXAMPLE__OCC2,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ2);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMCLEAR,
        DNX_SW_STATE_EXAMPLE__OCC2,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ2,
        "example[%d]->occ2",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC2,
        example_info,
        EXAMPLE_OCC2_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_occ2_print(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC2,
        SW_STATE_FUNC_PRINT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC2,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC2,
        DNX_SW_STATE_DIAG_OCC_BM);

    SW_STATE_OCC_BM_PRINT(
        DNX_SW_STATE_EXAMPLE__OCC2,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ2);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_PRINT_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMPRINT,
        DNX_SW_STATE_EXAMPLE__OCC2,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->occ2,
        "example[%d]->occ2",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__OCC2,
        example_info,
        EXAMPLE_OCC2_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_cb_get_cb(int unit, dnx_sw_state_callback_test_function_cb* cb)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__CB,
        SW_STATE_FUNC_GET_CB,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__CB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__CB,
        DNX_SW_STATE_DIAG_CB_DB);

    SW_STATE_CB_DB_GET_CB(
        DNX_SW_STATE_EXAMPLE__CB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->cb,
        cb,
        dnx_sw_state_callback_test_function_cb_get_cb);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_CB_DB_GET_CB_LOGGING,
        BSL_LS_SWSTATEDNX_CB_DBGET_CB,
        DNX_SW_STATE_EXAMPLE__CB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->cb,
        "example[%d]->cb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__CB,
        example_info,
        EXAMPLE_CB_INFO,
        DNX_SW_STATE_DIAG_CB_DB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_cb_register_cb(int unit, char *name)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__CB,
        SW_STATE_FUNC_REGISTER_CB,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__CB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__CB,
        DNX_SW_STATE_DIAG_CB_DB);

    SW_STATE_CB_DB_REGISTER_CB(
        DNX_SW_STATE_EXAMPLE__CB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->cb,
        name,
        dnx_sw_state_callback_test_function_cb_get_cb);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_CB_DB_REGISTER_CB_LOGGING,
        BSL_LS_SWSTATEDNX_CB_DBREGISTER_CB,
        DNX_SW_STATE_EXAMPLE__CB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->cb,
        "example[%d]->cb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__CB,
        example_info,
        EXAMPLE_CB_INFO,
        DNX_SW_STATE_DIAG_CB_DB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_cb_unregister_cb(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__CB,
        SW_STATE_FUNC_UNREGISTER_CB,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__CB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__CB,
        DNX_SW_STATE_DIAG_CB_DB);

    SW_STATE_CB_DB_UNREGISTER_CB(
        DNX_SW_STATE_EXAMPLE__CB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->cb);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_CB_DB_UNREGISTER_CB_LOGGING,
        BSL_LS_SWSTATEDNX_CB_DBUNREGISTER_CB,
        DNX_SW_STATE_EXAMPLE__CB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->cb,
        "example[%d]->cb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__CB,
        example_info,
        EXAMPLE_CB_INFO,
        DNX_SW_STATE_DIAG_CB_DB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_leaf_struct_set(int unit, CONST sw_state_leaf_struct *leaf_struct)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__LEAF_STRUCT,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__LEAF_STRUCT,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__LEAF_STRUCT,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_EXAMPLE__LEAF_STRUCT,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->leaf_struct,
        leaf_struct,
        sw_state_leaf_struct,
        0,
        "example_leaf_struct_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__LEAF_STRUCT,
        leaf_struct,
        "example[%d]->leaf_struct",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__LEAF_STRUCT,
        example_info,
        EXAMPLE_LEAF_STRUCT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_leaf_struct_get(int unit, sw_state_leaf_struct *leaf_struct)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__LEAF_STRUCT,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__LEAF_STRUCT,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__LEAF_STRUCT,
        leaf_struct);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__LEAF_STRUCT,
        DNX_SW_STATE_DIAG_READ);

    *leaf_struct = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->leaf_struct;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__LEAF_STRUCT,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->leaf_struct,
        "example[%d]->leaf_struct",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__LEAF_STRUCT,
        example_info,
        EXAMPLE_LEAF_STRUCT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_defragmented_chunk_example_create(int unit, uint32 defragmented_chunk_example_idx_0, sw_state_defragmented_chunk_init_info_t * init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        defragmented_chunk_example_idx_0,
        6);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        DNX_SW_STATE_DIAG_DEFRAGMENTED_CHUNK_CREATE);

    SW_STATE_DEFRAGMENTED_CHUNK_CREATE(
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->defragmented_chunk_example[defragmented_chunk_example_idx_0],
        init_info,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_DEFRAGMENTED_CHUNK_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_DEFRAGMENTED_CHUNKCREATE,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->defragmented_chunk_example[defragmented_chunk_example_idx_0],
        "example[%d]->defragmented_chunk_example[%d]",
        unit, defragmented_chunk_example_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        example_info,
        EXAMPLE_DEFRAGMENTED_CHUNK_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_DEFRAGMENTED_CHUNK_CREATE,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_defragmented_chunk_example_piece_alloc(int unit, uint32 defragmented_chunk_example_idx_0, uint32 slots_in_piece, void *move_cb_additional_info, uint32 *piece_offset)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        SW_STATE_FUNC_PIECE_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        defragmented_chunk_example_idx_0,
        6);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        DNX_SW_STATE_DIAG_DEFRAGMENTED_CHUNK);

    SW_STATE_DEFRAGMENTED_CHUNK_PIECE_ALLOC(
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->defragmented_chunk_example[defragmented_chunk_example_idx_0],
        slots_in_piece,
        move_cb_additional_info,
        piece_offset);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_DEFRAGMENTED_CHUNK_PIECE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_DEFRAGMENTED_CHUNKPIECE_ALLOC,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->defragmented_chunk_example[defragmented_chunk_example_idx_0],
        "example[%d]->defragmented_chunk_example[%d]",
        unit, defragmented_chunk_example_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        example_info,
        EXAMPLE_DEFRAGMENTED_CHUNK_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_DEFRAGMENTED_CHUNK,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_defragmented_chunk_example_piece_free(int unit, uint32 defragmented_chunk_example_idx_0, uint32 piece_offset)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        SW_STATE_FUNC_PIECE_FREE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        defragmented_chunk_example_idx_0,
        6);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        DNX_SW_STATE_DIAG_DEFRAGMENTED_CHUNK);

    SW_STATE_DEFRAGMENTED_CHUNK_PIECE_FREE(
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->defragmented_chunk_example[defragmented_chunk_example_idx_0],
        piece_offset);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_DEFRAGMENTED_CHUNK_PIECE_FREE_LOGGING,
        BSL_LS_SWSTATEDNX_DEFRAGMENTED_CHUNKPIECE_FREE,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->defragmented_chunk_example[defragmented_chunk_example_idx_0],
        "example[%d]->defragmented_chunk_example[%d]",
        unit, defragmented_chunk_example_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        example_info,
        EXAMPLE_DEFRAGMENTED_CHUNK_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_DEFRAGMENTED_CHUNK,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_defragmented_chunk_example_print(int unit, uint32 defragmented_chunk_example_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        SW_STATE_FUNC_PRINT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        defragmented_chunk_example_idx_0,
        6);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        DNX_SW_STATE_DIAG_DEFRAGMENTED_CHUNK);

    SW_STATE_DEFRAGMENTED_CHUNK_PRINT(
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->defragmented_chunk_example[defragmented_chunk_example_idx_0]);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_DEFRAGMENTED_CHUNK_PRINT_LOGGING,
        BSL_LS_SWSTATEDNX_DEFRAGMENTED_CHUNKPRINT,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->defragmented_chunk_example[defragmented_chunk_example_idx_0],
        "example[%d]->defragmented_chunk_example[%d]",
        unit, defragmented_chunk_example_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        example_info,
        EXAMPLE_DEFRAGMENTED_CHUNK_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_DEFRAGMENTED_CHUNK,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_defragmented_chunk_example_offset_unreserve(int unit, uint32 defragmented_chunk_example_idx_0, uint32 slots_in_offset, uint32 piece_offset)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        SW_STATE_FUNC_OFFSET_UNRESERVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        defragmented_chunk_example_idx_0,
        6);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        DNX_SW_STATE_DIAG_DEFRAGMENTED_CHUNK);

    SW_STATE_DEFRAGMENTED_CHUNK_OFFSET_UNRESERVE(
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->defragmented_chunk_example[defragmented_chunk_example_idx_0],
        slots_in_offset,
        piece_offset);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_DEFRAGMENTED_CHUNK_OFFSET_UNRESERVE_LOGGING,
        BSL_LS_SWSTATEDNX_DEFRAGMENTED_CHUNKOFFSET_UNRESERVE,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->defragmented_chunk_example[defragmented_chunk_example_idx_0],
        "example[%d]->defragmented_chunk_example[%d]",
        unit, defragmented_chunk_example_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        example_info,
        EXAMPLE_DEFRAGMENTED_CHUNK_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_DEFRAGMENTED_CHUNK,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_defragmented_chunk_example_offset_reserve(int unit, uint32 defragmented_chunk_example_idx_0, uint32 slots_in_offset, uint32 piece_offset)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        SW_STATE_FUNC_OFFSET_RESERVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        defragmented_chunk_example_idx_0,
        6);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        DNX_SW_STATE_DIAG_DEFRAGMENTED_CHUNK);

    SW_STATE_DEFRAGMENTED_CHUNK_OFFSET_RESERVE(
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->defragmented_chunk_example[defragmented_chunk_example_idx_0],
        slots_in_offset,
        piece_offset);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_DEFRAGMENTED_CHUNK_OFFSET_RESERVE_LOGGING,
        BSL_LS_SWSTATEDNX_DEFRAGMENTED_CHUNKOFFSET_RESERVE,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->defragmented_chunk_example[defragmented_chunk_example_idx_0],
        "example[%d]->defragmented_chunk_example[%d]",
        unit, defragmented_chunk_example_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        example_info,
        EXAMPLE_DEFRAGMENTED_CHUNK_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_DEFRAGMENTED_CHUNK,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_defragmented_chunk_example_defrag(int unit, uint32 defragmented_chunk_example_idx_0, uint32 wanted_size, void *move_cb_additional_info, uint32 *piece_offset)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        SW_STATE_FUNC_DEFRAG,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        defragmented_chunk_example_idx_0,
        6);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        DNX_SW_STATE_DIAG_DEFRAGMENTED_CHUNK);

    SW_STATE_DEFRAGMENTED_CHUNK_DEFRAG(
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->defragmented_chunk_example[defragmented_chunk_example_idx_0],
        wanted_size,
        move_cb_additional_info,
        piece_offset);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_DEFRAGMENTED_CHUNK_DEFRAG_LOGGING,
        BSL_LS_SWSTATEDNX_DEFRAGMENTED_CHUNKDEFRAG,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->defragmented_chunk_example[defragmented_chunk_example_idx_0],
        "example[%d]->defragmented_chunk_example[%d]",
        unit, defragmented_chunk_example_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__DEFRAGMENTED_CHUNK_EXAMPLE,
        example_info,
        EXAMPLE_DEFRAGMENTED_CHUNK_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_DEFRAGMENTED_CHUNK,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_packed_expecto_patronum_set(int unit, char expecto_patronum)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PACKED__EXPECTO_PATRONUM,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PACKED__EXPECTO_PATRONUM,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PACKED__EXPECTO_PATRONUM,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__PACKED__EXPECTO_PATRONUM,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->packed.expecto_patronum,
        expecto_patronum,
        char,
        0,
        "example_packed_expecto_patronum_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__PACKED__EXPECTO_PATRONUM,
        &expecto_patronum,
        "example[%d]->packed.expecto_patronum",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PACKED__EXPECTO_PATRONUM,
        example_info,
        EXAMPLE_PACKED_EXPECTO_PATRONUM_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_packed_expecto_patronum_get(int unit, char *expecto_patronum)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PACKED__EXPECTO_PATRONUM,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PACKED__EXPECTO_PATRONUM,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__PACKED__EXPECTO_PATRONUM,
        expecto_patronum);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PACKED__EXPECTO_PATRONUM,
        DNX_SW_STATE_DIAG_READ);

    *expecto_patronum = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->packed.expecto_patronum;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__PACKED__EXPECTO_PATRONUM,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->packed.expecto_patronum,
        "example[%d]->packed.expecto_patronum",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PACKED__EXPECTO_PATRONUM,
        example_info,
        EXAMPLE_PACKED_EXPECTO_PATRONUM_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_packed_my_integer_set(int unit, int my_integer)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PACKED__MY_INTEGER,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PACKED__MY_INTEGER,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PACKED__MY_INTEGER,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__PACKED__MY_INTEGER,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->packed.my_integer,
        my_integer,
        int,
        0,
        "example_packed_my_integer_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__PACKED__MY_INTEGER,
        &my_integer,
        "example[%d]->packed.my_integer",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PACKED__MY_INTEGER,
        example_info,
        EXAMPLE_PACKED_MY_INTEGER_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_packed_my_integer_get(int unit, int *my_integer)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PACKED__MY_INTEGER,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PACKED__MY_INTEGER,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__PACKED__MY_INTEGER,
        my_integer);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PACKED__MY_INTEGER,
        DNX_SW_STATE_DIAG_READ);

    *my_integer = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->packed.my_integer;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__PACKED__MY_INTEGER,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->packed.my_integer,
        "example[%d]->packed.my_integer",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PACKED__MY_INTEGER,
        example_info,
        EXAMPLE_PACKED_MY_INTEGER_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}


#ifdef BCM_WARM_BOOT_SUPPORT

int
example_params_flags_var_warm_boot_ifdef_set(int unit, int var_warm_boot_ifdef)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IFDEF,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IFDEF,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IFDEF,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IFDEF,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->params_flags.var_warm_boot_ifdef,
        var_warm_boot_ifdef,
        int,
        0,
        "example_params_flags_var_warm_boot_ifdef_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IFDEF,
        &var_warm_boot_ifdef,
        "example[%d]->params_flags.var_warm_boot_ifdef",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IFDEF,
        example_info,
        EXAMPLE_PARAMS_FLAGS_VAR_WARM_BOOT_IFDEF_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}


#endif  
#ifdef BCM_WARM_BOOT_SUPPORT

int
example_params_flags_var_warm_boot_ifdef_get(int unit, int *var_warm_boot_ifdef)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IFDEF,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IFDEF,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IFDEF,
        var_warm_boot_ifdef);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IFDEF,
        DNX_SW_STATE_DIAG_READ);

    *var_warm_boot_ifdef = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->params_flags.var_warm_boot_ifdef;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IFDEF,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->params_flags.var_warm_boot_ifdef,
        "example[%d]->params_flags.var_warm_boot_ifdef",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IFDEF,
        example_info,
        EXAMPLE_PARAMS_FLAGS_VAR_WARM_BOOT_IFDEF_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}


#endif  
#ifndef BCM_WARM_BOOT_SUPPORT

int
example_params_flags_var_warm_boot_ifndef_set(int unit, int var_warm_boot_ifndef)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IFNDEF,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IFNDEF,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IFNDEF,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IFNDEF,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->params_flags.var_warm_boot_ifndef,
        var_warm_boot_ifndef,
        int,
        0,
        "example_params_flags_var_warm_boot_ifndef_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IFNDEF,
        &var_warm_boot_ifndef,
        "example[%d]->params_flags.var_warm_boot_ifndef",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IFNDEF,
        example_info,
        EXAMPLE_PARAMS_FLAGS_VAR_WARM_BOOT_IFNDEF_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}


#endif  
#ifndef BCM_WARM_BOOT_SUPPORT

int
example_params_flags_var_warm_boot_ifndef_get(int unit, int *var_warm_boot_ifndef)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IFNDEF,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IFNDEF,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IFNDEF,
        var_warm_boot_ifndef);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IFNDEF,
        DNX_SW_STATE_DIAG_READ);

    *var_warm_boot_ifndef = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->params_flags.var_warm_boot_ifndef;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IFNDEF,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->params_flags.var_warm_boot_ifndef,
        "example[%d]->params_flags.var_warm_boot_ifndef",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IFNDEF,
        example_info,
        EXAMPLE_PARAMS_FLAGS_VAR_WARM_BOOT_IFNDEF_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}


#endif  
#if defined(BCM_WARM_BOOT_SUPPORT)

int
example_params_flags_var_warm_boot_if_set(int unit, int var_warm_boot_if)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IF,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IF,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IF,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IF,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->params_flags.var_warm_boot_if,
        var_warm_boot_if,
        int,
        0,
        "example_params_flags_var_warm_boot_if_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IF,
        &var_warm_boot_if,
        "example[%d]->params_flags.var_warm_boot_if",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IF,
        example_info,
        EXAMPLE_PARAMS_FLAGS_VAR_WARM_BOOT_IF_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}


#endif  
#if defined(BCM_WARM_BOOT_SUPPORT)

int
example_params_flags_var_warm_boot_if_get(int unit, int *var_warm_boot_if)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IF,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IF,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IF,
        var_warm_boot_if);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IF,
        DNX_SW_STATE_DIAG_READ);

    *var_warm_boot_if = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->params_flags.var_warm_boot_if;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IF,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->params_flags.var_warm_boot_if,
        "example[%d]->params_flags.var_warm_boot_if",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__VAR_WARM_BOOT_IF,
        example_info,
        EXAMPLE_PARAMS_FLAGS_VAR_WARM_BOOT_IF_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}


#endif  
#if defined(BCM_WARM_BOOT_SUPPORT) && defined(BCM_DNX_SUPPORT) 

int
example_params_flags_integer_multiple_flags_set(int unit, int integer_multiple_flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__INTEGER_MULTIPLE_FLAGS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__INTEGER_MULTIPLE_FLAGS,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__INTEGER_MULTIPLE_FLAGS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__INTEGER_MULTIPLE_FLAGS,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->params_flags.integer_multiple_flags,
        integer_multiple_flags,
        int,
        0,
        "example_params_flags_integer_multiple_flags_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__INTEGER_MULTIPLE_FLAGS,
        &integer_multiple_flags,
        "example[%d]->params_flags.integer_multiple_flags",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__INTEGER_MULTIPLE_FLAGS,
        example_info,
        EXAMPLE_PARAMS_FLAGS_INTEGER_MULTIPLE_FLAGS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}


#endif  
#if defined(BCM_WARM_BOOT_SUPPORT) && defined(BCM_DNX_SUPPORT) 

int
example_params_flags_integer_multiple_flags_get(int unit, int *integer_multiple_flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__INTEGER_MULTIPLE_FLAGS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__INTEGER_MULTIPLE_FLAGS,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__INTEGER_MULTIPLE_FLAGS,
        integer_multiple_flags);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__INTEGER_MULTIPLE_FLAGS,
        DNX_SW_STATE_DIAG_READ);

    *integer_multiple_flags = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->params_flags.integer_multiple_flags;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__INTEGER_MULTIPLE_FLAGS,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->params_flags.integer_multiple_flags,
        "example[%d]->params_flags.integer_multiple_flags",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__INTEGER_MULTIPLE_FLAGS,
        example_info,
        EXAMPLE_PARAMS_FLAGS_INTEGER_MULTIPLE_FLAGS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}


#endif  

int
example_params_flags_buff_variable_set(int unit, uint32 buff_variable_idx_0, DNX_SW_STATE_BUFF buff_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__BUFF_VARIABLE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__BUFF_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        buff_variable_idx_0,
        10);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__BUFF_VARIABLE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__BUFF_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->params_flags.buff_variable[buff_variable_idx_0],
        buff_variable,
        DNX_SW_STATE_BUFF,
        0,
        "example_params_flags_buff_variable_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__BUFF_VARIABLE,
        &buff_variable,
        "example[%d]->params_flags.buff_variable[%d]",
        unit, buff_variable_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__BUFF_VARIABLE,
        example_info,
        EXAMPLE_PARAMS_FLAGS_BUFF_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_params_flags_buff_variable_get(int unit, uint32 buff_variable_idx_0, DNX_SW_STATE_BUFF *buff_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__BUFF_VARIABLE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__BUFF_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        buff_variable_idx_0,
        10);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__BUFF_VARIABLE,
        buff_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__BUFF_VARIABLE,
        DNX_SW_STATE_DIAG_READ);

    *buff_variable = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->params_flags.buff_variable[buff_variable_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__BUFF_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->params_flags.buff_variable[buff_variable_idx_0],
        "example[%d]->params_flags.buff_variable[%d]",
        unit, buff_variable_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__BUFF_VARIABLE,
        example_info,
        EXAMPLE_PARAMS_FLAGS_BUFF_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_params_flags_buff_variable_memread(int unit, uint8 *_dst, uint32 _offset, size_t _len)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__BUFF_VARIABLE,
        SW_STATE_FUNC_MEMREAD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__BUFF_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BUFFER_CHECK(
        (_offset + _len - 1),
        10);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__BUFF_VARIABLE,
        DNX_SW_STATE_DIAG_READ);

    DNX_SW_STATE_MEMREAD(
        unit,
        _dst,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->params_flags.buff_variable,
        _offset,
        _len,
        0,
        "example_params_flags_buff_variable_BUFFER");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MEMREAD_LOGGING,
        BSL_LS_SWSTATEDNX_MEMREAD,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__BUFF_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->params_flags.buff_variable,
        "example[%d]->params_flags.buff_variable",
        unit, _len);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__BUFF_VARIABLE,
        example_info,
        EXAMPLE_PARAMS_FLAGS_BUFF_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_params_flags_buff_variable_memwrite(int unit, const uint8 *_src, uint32 _offset, size_t _len)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__BUFF_VARIABLE,
        SW_STATE_FUNC_MEMWRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__BUFF_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BUFFER_CHECK(
        (_offset + _len - 1),
        10);

    DNX_SW_STATE_MEMWRITE(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__BUFF_VARIABLE,
        _src,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->params_flags.buff_variable,
        _offset,
        _len,
        0,
        "example_params_flags_buff_variable_BUFFER");

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_params_flags_buff_variable_memcmp(int unit, const uint8 *_buff, uint32 _offset, size_t _len, int *cmp_result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__BUFF_VARIABLE,
        SW_STATE_FUNC_MEMCMP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__BUFF_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BUFFER_CHECK(
        (_offset + _len - 1),
        10);

    DNX_SW_STATE_MEMCMP(
        unit,
        _buff,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->params_flags.buff_variable,
        _offset,
        _len,
        cmp_result,
        0,
        "example_params_flags_buff_variable_BUFFER");

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_params_flags_buff_variable_memset(int unit, uint32 _offset, size_t _len, int _value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__BUFF_VARIABLE,
        SW_STATE_FUNC_MEMSET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__BUFF_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BUFFER_CHECK(
        (_offset + _len - 1),
        10);

    DNX_SW_STATE_MEMSET(
        unit,
        DNX_SW_STATE_EXAMPLE__PARAMS_FLAGS__BUFF_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->params_flags.buff_variable,
        _offset,
        _value,
        _len,
        0,
        "example_params_flags_buff_variable_BUFFER");

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_my_union_my_integer_set(int unit, int my_integer)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_UNION__MY_INTEGER,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_UNION__MY_INTEGER,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_UNION__MY_INTEGER,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_UNION__MY_INTEGER,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->my_union.my_integer,
        my_integer,
        int,
        0,
        "example_my_union_my_integer_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__MY_UNION__MY_INTEGER,
        &my_integer,
        "example[%d]->my_union.my_integer",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_UNION__MY_INTEGER,
        example_info,
        EXAMPLE_MY_UNION_MY_INTEGER_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_my_union_my_integer_get(int unit, int *my_integer)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_UNION__MY_INTEGER,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_UNION__MY_INTEGER,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_UNION__MY_INTEGER,
        my_integer);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_UNION__MY_INTEGER,
        DNX_SW_STATE_DIAG_READ);

    *my_integer = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->my_union.my_integer;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__MY_UNION__MY_INTEGER,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->my_union.my_integer,
        "example[%d]->my_union.my_integer",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_UNION__MY_INTEGER,
        example_info,
        EXAMPLE_MY_UNION_MY_INTEGER_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_my_union_my_char_set(int unit, char my_char)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_UNION__MY_CHAR,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_UNION__MY_CHAR,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_UNION__MY_CHAR,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_UNION__MY_CHAR,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->my_union.my_char,
        my_char,
        char,
        0,
        "example_my_union_my_char_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__MY_UNION__MY_CHAR,
        &my_char,
        "example[%d]->my_union.my_char",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_UNION__MY_CHAR,
        example_info,
        EXAMPLE_MY_UNION_MY_CHAR_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_my_union_my_char_get(int unit, char *my_char)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_UNION__MY_CHAR,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_UNION__MY_CHAR,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_UNION__MY_CHAR,
        my_char);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_UNION__MY_CHAR,
        DNX_SW_STATE_DIAG_READ);

    *my_char = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->my_union.my_char;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__MY_UNION__MY_CHAR,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->my_union.my_char,
        "example[%d]->my_union.my_char",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__MY_UNION__MY_CHAR,
        example_info,
        EXAMPLE_MY_UNION_MY_CHAR_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_string_stringncat(int unit, char *src)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__STRING,
        SW_STATE_FUNC_STRINGNCAT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__STRING,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__STRING,
        DNX_SW_STATE_DIAG_STRING);

    SW_STATE_STRING_STRINGNCAT(
        DNX_SW_STATE_EXAMPLE__STRING,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->string,
        src,
        100);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_STRING_STRINGNCAT_LOGGING,
        BSL_LS_SWSTATEDNX_STRINGSTRINGNCAT,
        DNX_SW_STATE_EXAMPLE__STRING,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->string,
        "example[%d]->string",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__STRING,
        example_info,
        EXAMPLE_STRING_INFO,
        DNX_SW_STATE_DIAG_STRING,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_string_stringncmp(int unit, char *cmp_string, int *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__STRING,
        SW_STATE_FUNC_STRINGNCMP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__STRING,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__STRING,
        DNX_SW_STATE_DIAG_STRING);

    SW_STATE_STRING_STRINGNCMP(
        DNX_SW_STATE_EXAMPLE__STRING,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->string,
        cmp_string,
        result,
        100);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_STRING_STRINGNCMP_LOGGING,
        BSL_LS_SWSTATEDNX_STRINGSTRINGNCMP,
        DNX_SW_STATE_EXAMPLE__STRING,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->string,
        "example[%d]->string",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__STRING,
        example_info,
        EXAMPLE_STRING_INFO,
        DNX_SW_STATE_DIAG_STRING,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_string_stringncpy(int unit, char *src)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__STRING,
        SW_STATE_FUNC_STRINGNCPY,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__STRING,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__STRING,
        DNX_SW_STATE_DIAG_STRING);

    SW_STATE_STRING_STRINGNCPY(
        DNX_SW_STATE_EXAMPLE__STRING,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->string,
        src,
        100);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_STRING_STRINGNCPY_LOGGING,
        BSL_LS_SWSTATEDNX_STRINGSTRINGNCPY,
        DNX_SW_STATE_EXAMPLE__STRING,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->string,
        "example[%d]->string",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__STRING,
        example_info,
        EXAMPLE_STRING_INFO,
        DNX_SW_STATE_DIAG_STRING,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_string_stringlen(int unit, uint32 *size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__STRING,
        SW_STATE_FUNC_STRINGLEN,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__STRING,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__STRING,
        DNX_SW_STATE_DIAG_STRING);

    SW_STATE_STRING_STRINGLEN(
        DNX_SW_STATE_EXAMPLE__STRING,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->string,
        size);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_STRING_STRINGLEN_LOGGING,
        BSL_LS_SWSTATEDNX_STRINGSTRINGLEN,
        DNX_SW_STATE_EXAMPLE__STRING,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->string,
        "example[%d]->string",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__STRING,
        example_info,
        EXAMPLE_STRING_INFO,
        DNX_SW_STATE_DIAG_STRING,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_string_stringget(int unit, char *o_string)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__STRING,
        SW_STATE_FUNC_STRINGGET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__STRING,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__STRING,
        DNX_SW_STATE_DIAG_STRING);

    SW_STATE_STRING_STRINGGET(
        DNX_SW_STATE_EXAMPLE__STRING,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->string,
        o_string);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_STRING_STRINGGET_LOGGING,
        BSL_LS_SWSTATEDNX_STRINGSTRINGGET,
        DNX_SW_STATE_EXAMPLE__STRING,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->string,
        "example[%d]->string",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__STRING,
        example_info,
        EXAMPLE_STRING_INFO,
        DNX_SW_STATE_DIAG_STRING,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_alloc_child_check_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK,
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
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check,
        sw_state_example_child_size_check_t,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_alloc_child_check_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check,
        "example[%d]->alloc_child_check",
        unit,
        nof_instances_to_alloc_0 * sizeof(sw_state_example_child_size_check_t) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK,
        example_info,
        EXAMPLE_ALLOC_CHILD_CHECK_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_alloc_child_check_static_array_of_p_set(int unit, uint32 alloc_child_check_idx_0, uint32 static_array_of_p_idx_0, uint32 static_array_of_p_idx_1, int static_array_of_p)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY_OF_P,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY_OF_P,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY_OF_P,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check,
        alloc_child_check_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY_OF_P,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[alloc_child_check_idx_0].static_array_of_p[static_array_of_p_idx_0]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        static_array_of_p_idx_0,
        10000);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY_OF_P,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY_OF_P,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[alloc_child_check_idx_0].static_array_of_p[static_array_of_p_idx_0],
        static_array_of_p_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY_OF_P,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY_OF_P,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[alloc_child_check_idx_0].static_array_of_p[static_array_of_p_idx_0][static_array_of_p_idx_1],
        static_array_of_p,
        int,
        0,
        "example_alloc_child_check_static_array_of_p_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY_OF_P,
        &static_array_of_p,
        "example[%d]->alloc_child_check[%d].static_array_of_p[%d][%d]",
        unit, alloc_child_check_idx_0, static_array_of_p_idx_0, static_array_of_p_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY_OF_P,
        example_info,
        EXAMPLE_ALLOC_CHILD_CHECK_STATIC_ARRAY_OF_P_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_alloc_child_check_static_array_of_p_get(int unit, uint32 alloc_child_check_idx_0, uint32 static_array_of_p_idx_0, uint32 static_array_of_p_idx_1, int *static_array_of_p)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY_OF_P,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY_OF_P,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY_OF_P,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check,
        alloc_child_check_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY_OF_P,
        static_array_of_p);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        static_array_of_p_idx_0,
        10000);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY_OF_P,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[alloc_child_check_idx_0].static_array_of_p[static_array_of_p_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY_OF_P,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[alloc_child_check_idx_0].static_array_of_p[static_array_of_p_idx_0],
        static_array_of_p_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY_OF_P,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY_OF_P,
        DNX_SW_STATE_DIAG_READ);

    *static_array_of_p = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[alloc_child_check_idx_0].static_array_of_p[static_array_of_p_idx_0][static_array_of_p_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY_OF_P,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[alloc_child_check_idx_0].static_array_of_p[static_array_of_p_idx_0][static_array_of_p_idx_1],
        "example[%d]->alloc_child_check[%d].static_array_of_p[%d][%d]",
        unit, alloc_child_check_idx_0, static_array_of_p_idx_0, static_array_of_p_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY_OF_P,
        example_info,
        EXAMPLE_ALLOC_CHILD_CHECK_STATIC_ARRAY_OF_P_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_alloc_child_check_static_array_of_p_alloc(int unit, uint32 alloc_child_check_idx_0, uint32 static_array_of_p_idx_0, uint32 nof_instances_to_alloc_1)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY_OF_P,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY_OF_P,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY_OF_P,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check,
        alloc_child_check_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY_OF_P,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        static_array_of_p_idx_0,
        10000);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY_OF_P,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY_OF_P,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[alloc_child_check_idx_0].static_array_of_p[static_array_of_p_idx_0],
        int,
        nof_instances_to_alloc_1,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID],
        1,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "example_alloc_child_check_static_array_of_p_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY_OF_P,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[alloc_child_check_idx_0].static_array_of_p[static_array_of_p_idx_0],
        "example[%d]->alloc_child_check[%d].static_array_of_p[%d]",
        unit, alloc_child_check_idx_0, static_array_of_p_idx_0,
        nof_instances_to_alloc_1 * sizeof(int) / sizeof(*((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[alloc_child_check_idx_0].static_array_of_p[static_array_of_p_idx_0]));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY_OF_P,
        example_info,
        EXAMPLE_ALLOC_CHILD_CHECK_STATIC_ARRAY_OF_P_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_alloc_child_check_static_array_set(int unit, uint32 alloc_child_check_idx_0, uint32 static_array_idx_0, int static_array)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check,
        alloc_child_check_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        static_array_idx_0,
        20000);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[alloc_child_check_idx_0].static_array[static_array_idx_0],
        static_array,
        int,
        0,
        "example_alloc_child_check_static_array_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY,
        &static_array,
        "example[%d]->alloc_child_check[%d].static_array[%d]",
        unit, alloc_child_check_idx_0, static_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY,
        example_info,
        EXAMPLE_ALLOC_CHILD_CHECK_STATIC_ARRAY_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_alloc_child_check_static_array_get(int unit, uint32 alloc_child_check_idx_0, uint32 static_array_idx_0, int *static_array)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check,
        alloc_child_check_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY,
        static_array);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        static_array_idx_0,
        20000);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY,
        DNX_SW_STATE_DIAG_READ);

    *static_array = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[alloc_child_check_idx_0].static_array[static_array_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[alloc_child_check_idx_0].static_array[static_array_idx_0],
        "example[%d]->alloc_child_check[%d].static_array[%d]",
        unit, alloc_child_check_idx_0, static_array_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__STATIC_ARRAY,
        example_info,
        EXAMPLE_ALLOC_CHILD_CHECK_STATIC_ARRAY_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_alloc_child_check_integer_variable_set(int unit, uint32 alloc_child_check_idx_0, int integer_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__INTEGER_VARIABLE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__INTEGER_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__INTEGER_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check,
        alloc_child_check_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__INTEGER_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__INTEGER_VARIABLE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__INTEGER_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[alloc_child_check_idx_0].integer_variable,
        integer_variable,
        int,
        0,
        "example_alloc_child_check_integer_variable_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__INTEGER_VARIABLE,
        &integer_variable,
        "example[%d]->alloc_child_check[%d].integer_variable",
        unit, alloc_child_check_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__INTEGER_VARIABLE,
        example_info,
        EXAMPLE_ALLOC_CHILD_CHECK_INTEGER_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_alloc_child_check_integer_variable_get(int unit, uint32 alloc_child_check_idx_0, int *integer_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__INTEGER_VARIABLE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__INTEGER_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__INTEGER_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check,
        alloc_child_check_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__INTEGER_VARIABLE,
        integer_variable);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__INTEGER_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__INTEGER_VARIABLE,
        DNX_SW_STATE_DIAG_READ);

    *integer_variable = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[alloc_child_check_idx_0].integer_variable;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__INTEGER_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->alloc_child_check[alloc_child_check_idx_0].integer_variable,
        "example[%d]->alloc_child_check[%d].integer_variable",
        unit, alloc_child_check_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__ALLOC_CHILD_CHECK__INTEGER_VARIABLE,
        example_info,
        EXAMPLE_ALLOC_CHILD_CHECK_INTEGER_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_pbmp_variable_set(int unit, bcm_pbmp_t pbmp_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        pbmp_variable,
        bcm_pbmp_t,
        0,
        "example_pbmp_variable_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        &pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_pbmp_variable_get(int unit, bcm_pbmp_t *pbmp_variable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        pbmp_variable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        DNX_SW_STATE_DIAG_READ);

    *pbmp_variable = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_pbmp_variable_pbmp_neq(int unit, _shr_pbmp_t input_pbmp, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        SW_STATE_FUNC_PBMP_NEQ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_NEQ(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        input_pbmp,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_NEQ_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPNEQ,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_pbmp_variable_pbmp_eq(int unit, _shr_pbmp_t input_pbmp, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        SW_STATE_FUNC_PBMP_EQ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_EQ(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        input_pbmp,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_EQ_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPEQ,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_pbmp_variable_pbmp_member(int unit, uint32 _input_port, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        SW_STATE_FUNC_PBMP_MEMBER,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_MEMBER(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        _input_port,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_MEMBER_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPMEMBER,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_pbmp_variable_pbmp_not_null(int unit, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        SW_STATE_FUNC_PBMP_NOT_NULL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_NOT_NULL(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_NOT_NULL_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPNOTNULL,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_pbmp_variable_pbmp_is_null(int unit, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        SW_STATE_FUNC_PBMP_IS_NULL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_IS_NULL(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_IS_NULL_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPISNULL,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_pbmp_variable_pbmp_count(int unit, int *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        SW_STATE_FUNC_PBMP_COUNT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_COUNT(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_COUNT_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPCOUNT,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_pbmp_variable_pbmp_xor(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        SW_STATE_FUNC_PBMP_XOR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_XOR(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_XOR_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPXOR,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_pbmp_variable_pbmp_remove(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        SW_STATE_FUNC_PBMP_REMOVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_REMOVE(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_REMOVE_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPREMOVE,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_pbmp_variable_pbmp_assign(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        SW_STATE_FUNC_PBMP_ASSIGN,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_ASSIGN(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_ASSIGN_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPASSIGN,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_pbmp_variable_pbmp_get(int unit, _shr_pbmp_t *output_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        SW_STATE_FUNC_PBMP_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_GET(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        output_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_GET_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPGET,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_pbmp_variable_pbmp_and(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        SW_STATE_FUNC_PBMP_AND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_AND(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_AND_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPAND,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_pbmp_variable_pbmp_negate(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        SW_STATE_FUNC_PBMP_NEGATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_NEGATE(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_NEGATE_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPNEGATE,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_pbmp_variable_pbmp_or(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        SW_STATE_FUNC_PBMP_OR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_OR(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_OR_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPOR,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_pbmp_variable_pbmp_clear(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        SW_STATE_FUNC_PBMP_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_CLEAR(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPCLEAR,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_pbmp_variable_pbmp_port_add(int unit, uint32 _input_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        SW_STATE_FUNC_PBMP_PORT_ADD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_PORT_ADD(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        _input_port,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_PORT_ADD_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPPORTADD,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_pbmp_variable_pbmp_port_flip(int unit, uint32 _input_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        SW_STATE_FUNC_PBMP_PORT_FLIP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_PORT_FLIP(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        _input_port,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_PORT_FLIP_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPPORTFLIP,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_pbmp_variable_pbmp_port_remove(int unit, uint32 _input_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        SW_STATE_FUNC_PBMP_PORT_REMOVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_PORT_REMOVE(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        _input_port,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_PORT_REMOVE_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPPORTREMOVE,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_pbmp_variable_pbmp_port_set(int unit, uint32 _input_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        SW_STATE_FUNC_PBMP_PORT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_PORT_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        _input_port,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_PORT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPPORTSET,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_pbmp_variable_pbmp_ports_range_add(int unit, uint32 _first_port, uint32 _range)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        SW_STATE_FUNC_PBMP_PORTS_RANGE_ADD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_PORTS_RANGE_ADD(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        _first_port,
        _range,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_PORTS_RANGE_ADD_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPPORTSRANGEADD,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_pbmp_variable_pbmp_fmt(int unit, char* _buffer)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        SW_STATE_FUNC_PBMP_FMT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_FMT(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        _buffer,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_FMT_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPFMT,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pbmp_variable,
        "example[%d]->pbmp_variable",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PBMP_VARIABLE,
        example_info,
        EXAMPLE_PBMP_VARIABLE_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_large_DS_examples_large_htb_create(int unit, uint32 large_htb_idx_0, sw_state_htbl_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        large_htb_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        DNX_SW_STATE_DIAG_HTB_CREATE);

    SW_STATE_HTB_CREATE(
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_htb[large_htb_idx_0],
        init_info,
        int,
        int,
        dnx_data_module_testing.example_tests.large_nof_elements_get(unit),
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCREATE,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_htb[large_htb_idx_0],
        "example[%d]->large_DS_examples.large_htb[%d]",
        unit, large_htb_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB_CREATE,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_large_DS_examples_large_htb_find(int unit, uint32 large_htb_idx_0, const int *key, int *value, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        SW_STATE_FUNC_FIND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        large_htb_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_FIND(
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_htb[large_htb_idx_0],
        key,
        value,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_FIND_LOGGING,
        BSL_LS_SWSTATEDNX_HTBFIND,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_htb[large_htb_idx_0],
        "example[%d]->large_DS_examples.large_htb[%d]",
        unit, large_htb_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_large_DS_examples_large_htb_insert(int unit, uint32 large_htb_idx_0, const int *key, const int *value, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        SW_STATE_FUNC_INSERT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        large_htb_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_INSERT(
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_htb[large_htb_idx_0],
        key,
        value,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_INSERT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBINSERT,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_htb[large_htb_idx_0],
        "example[%d]->large_DS_examples.large_htb[%d]",
        unit, large_htb_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_large_DS_examples_large_htb_get_next(int unit, uint32 large_htb_idx_0, SW_STATE_HASH_TABLE_ITER *iter, const int *key, const int *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        SW_STATE_FUNC_GET_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        large_htb_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_GET_NEXT(
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_htb[large_htb_idx_0],
        iter,
        key,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_GET_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBGET_NEXT,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_htb[large_htb_idx_0],
        "example[%d]->large_DS_examples.large_htb[%d]",
        unit, large_htb_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_large_DS_examples_large_htb_clear(int unit, uint32 large_htb_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        SW_STATE_FUNC_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        large_htb_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_CLEAR(
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_htb[large_htb_idx_0]);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCLEAR,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_htb[large_htb_idx_0],
        "example[%d]->large_DS_examples.large_htb[%d]",
        unit, large_htb_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_large_DS_examples_large_htb_delete(int unit, uint32 large_htb_idx_0, const int *key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        SW_STATE_FUNC_DELETE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        large_htb_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_DELETE(
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_htb[large_htb_idx_0],
        key);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_DELETE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBDELETE,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_htb[large_htb_idx_0],
        "example[%d]->large_DS_examples.large_htb[%d]",
        unit, large_htb_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_large_DS_examples_large_htb_delete_all(int unit, uint32 large_htb_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        SW_STATE_FUNC_DELETE_ALL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        large_htb_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_DELETE_ALL(
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_htb[large_htb_idx_0]);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_DELETE_ALL_LOGGING,
        BSL_LS_SWSTATEDNX_HTBDELETE_ALL,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_htb[large_htb_idx_0],
        "example[%d]->large_DS_examples.large_htb[%d]",
        unit, large_htb_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_HTB,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_large_DS_examples_large_index_htb_create(int unit, sw_state_idx_htbl_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        DNX_SW_STATE_DIAG_INDEX_HTB_CREATE);

    SW_STATE_INDEX_HTB_CREATE(
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        init_info,
        int,
        uint32,
        dnx_data_module_testing.example_tests.field_size_get(unit),
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBCREATE,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        "example[%d]->large_DS_examples.large_index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB_CREATE,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_large_DS_examples_large_index_htb_find(int unit, const int *key, uint32 *data_index, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        SW_STATE_FUNC_FIND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_FIND(
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        key,
        data_index,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_FIND_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBFIND,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        "example[%d]->large_DS_examples.large_index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_large_DS_examples_large_index_htb_insert(int unit, const int *key, const uint32 *data_index, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        SW_STATE_FUNC_INSERT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_INSERT(
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        key,
        data_index,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_INSERT_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBINSERT,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        "example[%d]->large_DS_examples.large_index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_large_DS_examples_large_index_htb_get_next(int unit, SW_STATE_INDEX_HASH_TABLE_ITER *iter, const int *key, const uint32 *data_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        SW_STATE_FUNC_GET_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_GET_NEXT(
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        iter,
        key,
        data_index);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_GET_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBGET_NEXT,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        "example[%d]->large_DS_examples.large_index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_large_DS_examples_large_index_htb_clear(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        SW_STATE_FUNC_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_CLEAR(
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBCLEAR,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        "example[%d]->large_DS_examples.large_index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_large_DS_examples_large_index_htb_delete(int unit, const int *key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        SW_STATE_FUNC_DELETE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_DELETE(
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        key);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_DELETE_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBDELETE,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        "example[%d]->large_DS_examples.large_index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_large_DS_examples_large_index_htb_delete_all(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        SW_STATE_FUNC_DELETE_ALL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_DELETE_ALL(
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_DELETE_ALL_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBDELETE_ALL,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        "example[%d]->large_DS_examples.large_index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_large_DS_examples_large_index_htb_insert_at_index(int unit, const int *key, uint32 data_idx, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        SW_STATE_FUNC_INSERT_AT_INDEX,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_INSERT_AT_INDEX(
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        key,
        data_idx,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_INSERT_AT_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBINSERT_AT_INDEX,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        "example[%d]->large_DS_examples.large_index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_large_DS_examples_large_index_htb_delete_by_index(int unit, uint32 data_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        SW_STATE_FUNC_DELETE_BY_INDEX,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_DELETE_BY_INDEX(
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        data_index);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_DELETE_BY_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBDELETE_BY_INDEX,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        "example[%d]->large_DS_examples.large_index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_large_DS_examples_large_index_htb_get_by_index(int unit, uint32 data_index, int *key, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        SW_STATE_FUNC_GET_BY_INDEX,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_GET_BY_INDEX(
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        data_index,
        key,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_GET_BY_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBGET_BY_INDEX,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->large_DS_examples.large_index_htb,
        "example[%d]->large_DS_examples.large_index_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_EXAMPLE__LARGE_DS_EXAMPLES__LARGE_INDEX_HTB,
        example_info,
        EXAMPLE_LARGE_DS_EXAMPLES_LARGE_INDEX_HTB_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_pretty_print_example_set(int unit, dnx_sw_state_pretty_print_example_t pretty_print_example)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PRETTY_PRINT_EXAMPLE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PRETTY_PRINT_EXAMPLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PRETTY_PRINT_EXAMPLE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_EXAMPLE__PRETTY_PRINT_EXAMPLE,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pretty_print_example,
        pretty_print_example,
        dnx_sw_state_pretty_print_example_t,
        0,
        "example_pretty_print_example_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_EXAMPLE__PRETTY_PRINT_EXAMPLE,
        &pretty_print_example,
        "example[%d]->pretty_print_example",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PRETTY_PRINT_EXAMPLE,
        example_info,
        EXAMPLE_PRETTY_PRINT_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_pretty_print_example_get(int unit, dnx_sw_state_pretty_print_example_t *pretty_print_example)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__PRETTY_PRINT_EXAMPLE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__PRETTY_PRINT_EXAMPLE,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_EXAMPLE__PRETTY_PRINT_EXAMPLE,
        pretty_print_example);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__PRETTY_PRINT_EXAMPLE,
        DNX_SW_STATE_DIAG_READ);

    *pretty_print_example = ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pretty_print_example;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_EXAMPLE__PRETTY_PRINT_EXAMPLE,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->pretty_print_example,
        "example[%d]->pretty_print_example",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__PRETTY_PRINT_EXAMPLE,
        example_info,
        EXAMPLE_PRETTY_PRINT_EXAMPLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_mutex_test_create(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__MUTEX_TEST,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__MUTEX_TEST,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__MUTEX_TEST,
        DNX_SW_STATE_DIAG_MUTEX_CREATE);

    DNX_SW_STATE_MUTEX_CREATE(
        unit,
        DNX_SW_STATE_EXAMPLE__MUTEX_TEST,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->mutex_test,
        "((sw_state_example*)sw_state_roots_array).mutex_test");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MUTEX_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXCREATE,
        DNX_SW_STATE_EXAMPLE__MUTEX_TEST,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->mutex_test,
        "example[%d]->mutex_test",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__MUTEX_TEST,
        example_info,
        EXAMPLE_MUTEX_TEST_INFO,
        DNX_SW_STATE_DIAG_MUTEX_CREATE,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_mutex_test_is_created(int unit, uint8 *is_created)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__MUTEX_TEST,
        SW_STATE_FUNC_IS_CREATED,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__MUTEX_TEST,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__MUTEX_TEST,
        DNX_SW_STATE_DIAG_MUTEX);

    DNX_SW_STATE_MUTEX_IS_CREATED(
        unit,
        DNX_SW_STATE_EXAMPLE__MUTEX_TEST,
        is_created,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->mutex_test,
        "((sw_state_example*)sw_state_roots_array).mutex_test");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MUTEX_IS_CREATED_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXCREATE,
        DNX_SW_STATE_EXAMPLE__MUTEX_TEST,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->mutex_test,
        "example[%d]->mutex_test",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__MUTEX_TEST,
        example_info,
        EXAMPLE_MUTEX_TEST_INFO,
        DNX_SW_STATE_DIAG_MUTEX,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_mutex_test_destroy(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__MUTEX_TEST,
        SW_STATE_FUNC_DESTROY,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__MUTEX_TEST,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__MUTEX_TEST,
        DNX_SW_STATE_DIAG_MUTEX);

    DNX_SW_STATE_MUTEX_DESTROY(
        unit,
        DNX_SW_STATE_EXAMPLE__MUTEX_TEST,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->mutex_test);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MUTEX_DESTROY_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXDESTROY,
        DNX_SW_STATE_EXAMPLE__MUTEX_TEST,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->mutex_test,
        "example[%d]->mutex_test",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__MUTEX_TEST,
        example_info,
        EXAMPLE_MUTEX_TEST_INFO,
        DNX_SW_STATE_DIAG_MUTEX,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_mutex_test_take(int unit, int usec)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__MUTEX_TEST,
        SW_STATE_FUNC_TAKE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__MUTEX_TEST,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__MUTEX_TEST,
        DNX_SW_STATE_DIAG_MUTEX);

    DNX_SW_STATE_MUTEX_TAKE(
        unit,
        DNX_SW_STATE_EXAMPLE__MUTEX_TEST,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->mutex_test,
        usec);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MUTEX_TAKE_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXTAKE,
        DNX_SW_STATE_EXAMPLE__MUTEX_TEST,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->mutex_test,
        "example[%d]->mutex_test",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__MUTEX_TEST,
        example_info,
        EXAMPLE_MUTEX_TEST_INFO,
        DNX_SW_STATE_DIAG_MUTEX,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_mutex_test_give(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__MUTEX_TEST,
        SW_STATE_FUNC_GIVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__MUTEX_TEST,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__MUTEX_TEST,
        DNX_SW_STATE_DIAG_MUTEX);

    DNX_SW_STATE_MUTEX_GIVE(
        unit,
        DNX_SW_STATE_EXAMPLE__MUTEX_TEST,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->mutex_test);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MUTEX_GIVE_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXGIVE,
        DNX_SW_STATE_EXAMPLE__MUTEX_TEST,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->mutex_test,
        "example[%d]->mutex_test",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__MUTEX_TEST,
        example_info,
        EXAMPLE_MUTEX_TEST_INFO,
        DNX_SW_STATE_DIAG_MUTEX,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_sem_test_create(int unit, int is_binary, int initial_count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__SEM_TEST,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__SEM_TEST,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__SEM_TEST,
        DNX_SW_STATE_DIAG_SEM_CREATE);

    DNX_SW_STATE_SEM_CREATE(
        unit,
        DNX_SW_STATE_EXAMPLE__SEM_TEST,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sem_test,
        is_binary,
        initial_count,
        "((sw_state_example*)sw_state_roots_array).sem_test");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SEM_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXCREATE,
        DNX_SW_STATE_EXAMPLE__SEM_TEST,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sem_test,
        "example[%d]->sem_test",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__SEM_TEST,
        example_info,
        EXAMPLE_SEM_TEST_INFO,
        DNX_SW_STATE_DIAG_SEM_CREATE,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_sem_test_is_created(int unit, uint8 *is_created)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__SEM_TEST,
        SW_STATE_FUNC_IS_CREATED,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__SEM_TEST,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__SEM_TEST,
        DNX_SW_STATE_DIAG_SEM);

    DNX_SW_STATE_SEM_IS_CREATED(
        unit,
        DNX_SW_STATE_EXAMPLE__SEM_TEST,
        is_created,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sem_test,
        "((sw_state_example*)sw_state_roots_array).sem_test");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SEM_IS_CREATED_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXCREATE,
        DNX_SW_STATE_EXAMPLE__SEM_TEST,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sem_test,
        "example[%d]->sem_test",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__SEM_TEST,
        example_info,
        EXAMPLE_SEM_TEST_INFO,
        DNX_SW_STATE_DIAG_SEM,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_sem_test_take(int unit, int usec)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__SEM_TEST,
        SW_STATE_FUNC_TAKE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__SEM_TEST,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__SEM_TEST,
        DNX_SW_STATE_DIAG_SEM);

    DNX_SW_STATE_SEM_TAKE(
        unit,
        DNX_SW_STATE_EXAMPLE__SEM_TEST,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sem_test,
        usec);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SEM_TAKE_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXTAKE,
        DNX_SW_STATE_EXAMPLE__SEM_TEST,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sem_test,
        "example[%d]->sem_test",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__SEM_TEST,
        example_info,
        EXAMPLE_SEM_TEST_INFO,
        DNX_SW_STATE_DIAG_SEM,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
example_sem_test_give(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_EXAMPLE__SEM_TEST,
        SW_STATE_FUNC_GIVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_EXAMPLE__SEM_TEST,
        sw_state_roots_array[unit][EXAMPLE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_EXAMPLE__SEM_TEST,
        DNX_SW_STATE_DIAG_SEM);

    DNX_SW_STATE_SEM_GIVE(
        unit,
        DNX_SW_STATE_EXAMPLE__SEM_TEST,
        ((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sem_test);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SEM_GIVE_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXGIVE,
        DNX_SW_STATE_EXAMPLE__SEM_TEST,
        &((sw_state_example*)sw_state_roots_array[unit][EXAMPLE_MODULE_ID])->sem_test,
        "example[%d]->sem_test",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_EXAMPLE__SEM_TEST,
        example_info,
        EXAMPLE_SEM_TEST_INFO,
        DNX_SW_STATE_DIAG_SEM,
        NULL,
        sw_state_layout_array[unit][EXAMPLE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



const char *
dnx_example_name_e_get_name(dnx_example_name_e value)
{
#if defined(INCLUDE_CTEST)
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_EXAMPLE_ENUM_NEGATIVE", value, DNX_EXAMPLE_ENUM_NEGATIVE);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_EXAMPLE_ENUM_ZERO", value, DNX_EXAMPLE_ENUM_ZERO);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_EXAMPLE_ENUM_ONE", value, DNX_EXAMPLE_ENUM_ONE);

#endif  
    return NULL;
}



const char *
dnx_example_inner_ifdef_e_get_name(dnx_example_inner_ifdef_e value)
{
#if defined(INCLUDE_CTEST)
#ifdef BCM_DNX_SUPPORT
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_EXAMPLE_STANDARD_ENUM_ENTRY_IFDEF", value, DNX_EXAMPLE_STANDARD_ENUM_ENTRY_IFDEF);

#endif  
#ifndef BCM_DNX_SUPPORT
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_EXAMPLE_STANDARD_ENUM_ENTRY_IFNDEF", value, DNX_EXAMPLE_STANDARD_ENUM_ENTRY_IFNDEF);

#endif  
#if defined(BCM_DNX_SUPPORT)
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_EXAMPLE_STANDARD_ENUM_ENTRY_IF", value, DNX_EXAMPLE_STANDARD_ENUM_ENTRY_IF);

#endif  
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_EXAMPLE_STANDARD_ENUM_ENTRY_STANDARD", value, DNX_EXAMPLE_STANDARD_ENUM_ENTRY_STANDARD);

#endif  
    return NULL;
}


#ifdef BCM_DNX_SUPPORT

const char *
dnx_example_ifdef_e_get_name(dnx_example_ifdef_e value)
{
#if defined(INCLUDE_CTEST)
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_EXAMPLE_IFDEF_ENUM_ENTRY_FAMILY", value, DNX_EXAMPLE_IFDEF_ENUM_ENTRY_FAMILY);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_EXAMPLE_IFDEF_ENUM_ENTRY_STANDARD", value, DNX_EXAMPLE_IFDEF_ENUM_ENTRY_STANDARD);

#endif  
    return NULL;
}


#endif  

shr_error_e
dnx_sw_state_callback_test_function_cb_get_cb(sw_state_cb_t * cb_db, uint8 dryRun, dnx_sw_state_callback_test_function_cb * cb)
{
    if (!sal_strncmp(cb_db->function_name,"dnx_sw_state_callback_test_second_function", 42))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = dnx_sw_state_callback_test_second_function;
        }
        return _SHR_E_NONE;
    }
    if (!sal_strncmp(cb_db->function_name,"dnx_sw_state_callback_test_first_function", 41))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = dnx_sw_state_callback_test_first_function;
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
		example_dynamic_array_zero_size_alloc}
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
		
		example_write_during_wb_example_set,
		example_write_during_wb_example_get}
	,
		{
		
		example_value_range_test_set,
		example_value_range_test_get}
	,
		{
		
		example_bitmap_variable_alloc_bitmap,
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
		
		example_bitmap_include_only_example_alloc_bitmap,
		example_bitmap_include_only_example_bit_set,
		example_bitmap_include_only_example_bit_get}
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
	,
		{
		
		example_bitmap_variable_access_get,
		example_bitmap_variable_access_bit_set,
		example_bitmap_variable_access_bit_clear,
		example_bitmap_variable_access_bit_get,
		example_bitmap_variable_access_bit_range_read,
		example_bitmap_variable_access_bit_range_write,
		example_bitmap_variable_access_bit_range_and,
		example_bitmap_variable_access_bit_range_or,
		example_bitmap_variable_access_bit_range_xor,
		example_bitmap_variable_access_bit_range_remove,
		example_bitmap_variable_access_bit_range_negate,
		example_bitmap_variable_access_bit_range_clear,
		example_bitmap_variable_access_bit_range_set,
		example_bitmap_variable_access_bit_range_null,
		example_bitmap_variable_access_bit_range_test,
		example_bitmap_variable_access_bit_range_eq,
		example_bitmap_variable_access_bit_range_count}
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
		
		example_buffer_set,
		example_buffer_get,
		example_buffer_alloc,
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
					
					example_default_value_tree_array_dnxdata_array_dynamic_array_dynamic_dynamic_alloc,
						{
						
						example_default_value_tree_array_dnxdata_array_dynamic_array_dynamic_dynamic_my_variable_set,
						example_default_value_tree_array_dnxdata_array_dynamic_array_dynamic_dynamic_my_variable_get}
					}
				,
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
		,
		}
	,
		{
		
		example_ll_create_empty,
		example_ll_nof_elements,
		example_ll_node_value,
		example_ll_node_update,
		example_ll_next_node,
		example_ll_previous_node,
		example_ll_remove_node,
		example_ll_get_last,
		example_ll_get_first,
		example_ll_print,
		example_ll_add_first,
		example_ll_add_last,
		example_ll_add_before,
		example_ll_add_after}
	,
	
		{
		
		example_multihead_ll_create_empty,
		example_multihead_ll_nof_elements,
		example_multihead_ll_node_value,
		example_multihead_ll_node_update,
		example_multihead_ll_next_node,
		example_multihead_ll_previous_node,
		example_multihead_ll_remove_node,
		example_multihead_ll_get_last,
		example_multihead_ll_get_first,
		example_multihead_ll_print,
		example_multihead_ll_add_first,
		example_multihead_ll_add_last,
		example_multihead_ll_add_before,
		example_multihead_ll_add_after}
	,
		{
		
		example_sorted_multihead_ll_create_empty,
		example_sorted_multihead_ll_nof_elements,
		example_sorted_multihead_ll_node_value,
		example_sorted_multihead_ll_node_update,
		example_sorted_multihead_ll_next_node,
		example_sorted_multihead_ll_previous_node,
		example_sorted_multihead_ll_remove_node,
		example_sorted_multihead_ll_get_last,
		example_sorted_multihead_ll_get_first,
		example_sorted_multihead_ll_print,
		example_sorted_multihead_ll_add,
		example_sorted_multihead_ll_node_key,
		example_sorted_multihead_ll_find}
	,
		{
		
		example_sorted_ll_create_empty,
		example_sorted_ll_nof_elements,
		example_sorted_ll_node_value,
		example_sorted_ll_node_update,
		example_sorted_ll_next_node,
		example_sorted_ll_previous_node,
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
		
		example_htb_rh_create,
		example_htb_rh_find,
		example_htb_rh_insert,
		example_htb_rh_replace,
		example_htb_rh_delete,
		example_htb_rh_delete_all,
		example_htb_rh_print}
	,
		{
		
		example_htb_arr_create,
		example_htb_arr_find,
		example_htb_arr_insert,
		example_htb_arr_get_next,
		example_htb_arr_clear,
		example_htb_arr_delete,
		example_htb_arr_delete_all,
		example_htb_arr_print}
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
		
		example_occ2_create,
		example_occ2_get_next,
		example_occ2_get_next_in_range,
		example_occ2_status_set,
		example_occ2_is_occupied,
		example_occ2_alloc_next,
		example_occ2_clear,
		example_occ2_print}
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
		example_defragmented_chunk_example_print,
		example_defragmented_chunk_example_offset_unreserve,
		example_defragmented_chunk_example_offset_reserve,
		example_defragmented_chunk_example_defrag}
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
		
#ifdef BCM_WARM_BOOT_SUPPORT
			{
			
			example_params_flags_var_warm_boot_ifdef_set,
			example_params_flags_var_warm_boot_ifdef_get}
		
		,
#endif  
		
#ifndef BCM_WARM_BOOT_SUPPORT
			{
			
			example_params_flags_var_warm_boot_ifndef_set,
			example_params_flags_var_warm_boot_ifndef_get}
		
		,
#endif  
		
#if defined(BCM_WARM_BOOT_SUPPORT)
			{
			
			example_params_flags_var_warm_boot_if_set,
			example_params_flags_var_warm_boot_if_get}
		
		,
#endif  
		
		
		
#if defined(BCM_WARM_BOOT_SUPPORT) && defined(BCM_DNX_SUPPORT) 
			{
			
			example_params_flags_integer_multiple_flags_set,
			example_params_flags_integer_multiple_flags_get}
		
		,
#endif  
		
		
		
		
		
			{
			
			example_params_flags_buff_variable_set,
			example_params_flags_buff_variable_get,
			example_params_flags_buff_variable_memread,
			example_params_flags_buff_variable_memwrite,
			example_params_flags_buff_variable_memcmp,
			example_params_flags_buff_variable_memset}
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
		}
	,
		{
		
		example_pretty_print_example_set,
		example_pretty_print_example_get}
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
		example_sem_test_take,
		example_sem_test_give}
	}
;
#endif  
#undef BSL_LOG_MODULE
