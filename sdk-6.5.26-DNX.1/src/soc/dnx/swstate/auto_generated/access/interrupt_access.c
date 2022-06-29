
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/interrupt_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/interrupt_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/interrupt_layout.h>





int
intr_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_INTR_DB,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_SW_STATE_INTR_DB,
        ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID]),
        "intr_db[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
intr_db_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_INTR_DB,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_SW_STATE_INTR_DB,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        soc_dnx_intr_t,
        DNX_SW_STATE_INTR_DB_NOF_PARAMS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "intr_db_init",
        DNX_SW_STATE_DIAG_ALLOC,
        interrupt_init_layout_structure);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_SW_STATE_INTR_DB,
        ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID]),
        "intr_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_INTR_DB,
        intr_db_info,
        INTR_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID]),
        sw_state_layout_array[unit][INTERRUPT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
intr_db_flags_set(int unit, uint32 flags_idx_0, uint32 flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_INTR_DB__FLAGS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_INTR_DB__FLAGS,
        sw_state_roots_array[unit][INTERRUPT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_INTR_DB__FLAGS,
        ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->flags,
        flags_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_INTR_DB__FLAGS,
        ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->flags);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_INTR_DB__FLAGS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_INTR_DB__FLAGS,
        ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->flags[flags_idx_0],
        flags,
        uint32,
        0,
        "intr_db_flags_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_INTR_DB__FLAGS,
        &flags,
        "intr_db[%d]->flags[%d]",
        unit, flags_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_INTR_DB__FLAGS,
        intr_db_info,
        INTR_DB_FLAGS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][INTERRUPT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
intr_db_flags_get(int unit, uint32 flags_idx_0, uint32 *flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_INTR_DB__FLAGS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_INTR_DB__FLAGS,
        sw_state_roots_array[unit][INTERRUPT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_INTR_DB__FLAGS,
        ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->flags,
        flags_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_INTR_DB__FLAGS,
        flags);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_INTR_DB__FLAGS,
        ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->flags);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_INTR_DB__FLAGS,
        DNX_SW_STATE_DIAG_READ);

    *flags = ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->flags[flags_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_INTR_DB__FLAGS,
        &((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->flags[flags_idx_0],
        "intr_db[%d]->flags[%d]",
        unit, flags_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_INTR_DB__FLAGS,
        intr_db_info,
        INTR_DB_FLAGS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][INTERRUPT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
intr_db_flags_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_INTR_DB__FLAGS,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_INTR_DB__FLAGS,
        sw_state_roots_array[unit][INTERRUPT_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_INTR_DB__FLAGS,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_INTR_DB__FLAGS,
        ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->flags,
        uint32,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][INTERRUPT_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "intr_db_flags_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_INTR_DB__FLAGS,
        ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->flags,
        "intr_db[%d]->flags",
        unit,
        nof_instances_to_alloc_0 * sizeof(uint32) / sizeof(*((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->flags));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_INTR_DB__FLAGS,
        intr_db_info,
        INTR_DB_FLAGS_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][INTERRUPT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
intr_db_storm_timed_count_set(int unit, uint32 storm_timed_count_idx_0, uint32 storm_timed_count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_COUNT,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_COUNT,
        sw_state_roots_array[unit][INTERRUPT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_INTR_DB__STORM_TIMED_COUNT,
        ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_timed_count,
        storm_timed_count_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_COUNT,
        ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_timed_count);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_COUNT,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_COUNT,
        ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_timed_count[storm_timed_count_idx_0],
        storm_timed_count,
        uint32,
        0,
        "intr_db_storm_timed_count_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_COUNT,
        &storm_timed_count,
        "intr_db[%d]->storm_timed_count[%d]",
        unit, storm_timed_count_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_COUNT,
        intr_db_info,
        INTR_DB_STORM_TIMED_COUNT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][INTERRUPT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
intr_db_storm_timed_count_get(int unit, uint32 storm_timed_count_idx_0, uint32 *storm_timed_count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_COUNT,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_COUNT,
        sw_state_roots_array[unit][INTERRUPT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_INTR_DB__STORM_TIMED_COUNT,
        ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_timed_count,
        storm_timed_count_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_COUNT,
        storm_timed_count);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_COUNT,
        ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_timed_count);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_COUNT,
        DNX_SW_STATE_DIAG_READ);

    *storm_timed_count = ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_timed_count[storm_timed_count_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_COUNT,
        &((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_timed_count[storm_timed_count_idx_0],
        "intr_db[%d]->storm_timed_count[%d]",
        unit, storm_timed_count_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_COUNT,
        intr_db_info,
        INTR_DB_STORM_TIMED_COUNT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][INTERRUPT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
intr_db_storm_timed_count_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_COUNT,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_COUNT,
        sw_state_roots_array[unit][INTERRUPT_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_COUNT,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_COUNT,
        ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_timed_count,
        uint32,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][INTERRUPT_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "intr_db_storm_timed_count_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_COUNT,
        ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_timed_count,
        "intr_db[%d]->storm_timed_count",
        unit,
        nof_instances_to_alloc_0 * sizeof(uint32) / sizeof(*((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_timed_count));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_COUNT,
        intr_db_info,
        INTR_DB_STORM_TIMED_COUNT_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][INTERRUPT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
intr_db_storm_timed_period_set(int unit, uint32 storm_timed_period_idx_0, uint32 storm_timed_period)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_PERIOD,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_PERIOD,
        sw_state_roots_array[unit][INTERRUPT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_INTR_DB__STORM_TIMED_PERIOD,
        ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_timed_period,
        storm_timed_period_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_PERIOD,
        ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_timed_period);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_PERIOD,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_PERIOD,
        ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_timed_period[storm_timed_period_idx_0],
        storm_timed_period,
        uint32,
        0,
        "intr_db_storm_timed_period_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_PERIOD,
        &storm_timed_period,
        "intr_db[%d]->storm_timed_period[%d]",
        unit, storm_timed_period_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_PERIOD,
        intr_db_info,
        INTR_DB_STORM_TIMED_PERIOD_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][INTERRUPT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
intr_db_storm_timed_period_get(int unit, uint32 storm_timed_period_idx_0, uint32 *storm_timed_period)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_PERIOD,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_PERIOD,
        sw_state_roots_array[unit][INTERRUPT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_INTR_DB__STORM_TIMED_PERIOD,
        ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_timed_period,
        storm_timed_period_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_PERIOD,
        storm_timed_period);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_PERIOD,
        ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_timed_period);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_PERIOD,
        DNX_SW_STATE_DIAG_READ);

    *storm_timed_period = ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_timed_period[storm_timed_period_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_PERIOD,
        &((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_timed_period[storm_timed_period_idx_0],
        "intr_db[%d]->storm_timed_period[%d]",
        unit, storm_timed_period_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_PERIOD,
        intr_db_info,
        INTR_DB_STORM_TIMED_PERIOD_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][INTERRUPT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
intr_db_storm_timed_period_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_PERIOD,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_PERIOD,
        sw_state_roots_array[unit][INTERRUPT_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_PERIOD,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_PERIOD,
        ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_timed_period,
        uint32,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][INTERRUPT_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "intr_db_storm_timed_period_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_PERIOD,
        ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_timed_period,
        "intr_db[%d]->storm_timed_period",
        unit,
        nof_instances_to_alloc_0 * sizeof(uint32) / sizeof(*((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_timed_period));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_TIMED_PERIOD,
        intr_db_info,
        INTR_DB_STORM_TIMED_PERIOD_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][INTERRUPT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
intr_db_storm_nominal_set(int unit, uint32 storm_nominal)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_NOMINAL,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_NOMINAL,
        sw_state_roots_array[unit][INTERRUPT_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_NOMINAL,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_NOMINAL,
        ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_nominal,
        storm_nominal,
        uint32,
        0,
        "intr_db_storm_nominal_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_INTR_DB__STORM_NOMINAL,
        &storm_nominal,
        "intr_db[%d]->storm_nominal",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_NOMINAL,
        intr_db_info,
        INTR_DB_STORM_NOMINAL_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][INTERRUPT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
intr_db_storm_nominal_get(int unit, uint32 *storm_nominal)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_NOMINAL,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_NOMINAL,
        sw_state_roots_array[unit][INTERRUPT_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_NOMINAL,
        storm_nominal);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_NOMINAL,
        DNX_SW_STATE_DIAG_READ);

    *storm_nominal = ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_nominal;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_INTR_DB__STORM_NOMINAL,
        &((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_nominal,
        "intr_db[%d]->storm_nominal",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_INTR_DB__STORM_NOMINAL,
        intr_db_info,
        INTR_DB_STORM_NOMINAL_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][INTERRUPT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}




intr_db_cbs intr_db = 	{
	
	intr_db_is_init,
	intr_db_init,
		{
		
		intr_db_flags_set,
		intr_db_flags_get,
		intr_db_flags_alloc}
	,
		{
		
		intr_db_storm_timed_count_set,
		intr_db_storm_timed_count_get,
		intr_db_storm_timed_count_alloc}
	,
		{
		
		intr_db_storm_timed_period_set,
		intr_db_storm_timed_period_get,
		intr_db_storm_timed_period_alloc}
	,
		{
		
		intr_db_storm_nominal_set,
		intr_db_storm_nominal_get}
	}
;
#undef BSL_LOG_MODULE
