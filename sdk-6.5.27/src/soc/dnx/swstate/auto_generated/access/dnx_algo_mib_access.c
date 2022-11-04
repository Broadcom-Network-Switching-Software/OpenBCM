
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/dnx_algo_mib_access.h>
#include <bcm_int/dnx/algo/mib/algo_mib.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_algo_mib_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_algo_mib_layout.h>





int
dnx_algo_mib_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_SW_STATE_DNX_ALGO_MIB_DB,
        ((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID]),
        "dnx_algo_mib_db[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_algo_mib_db_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_algo_mib_db_t,
        DNX_SW_STATE_DNX_ALGO_MIB_DB_NOF_PARAMS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_algo_mib_db_init",
        DNX_SW_STATE_DIAG_ALLOC,
        dnx_algo_mib_init_layout_structure);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_SW_STATE_DNX_ALGO_MIB_DB,
        ((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID]),
        "dnx_algo_mib_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB,
        dnx_algo_mib_db_info,
        DNX_ALGO_MIB_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID]),
        sw_state_layout_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_algo_mib_db_mib_counter_pbmp_set(int unit, bcm_pbmp_t counter_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        ((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        counter_pbmp,
        bcm_pbmp_t,
        0,
        "dnx_algo_mib_db_mib_counter_pbmp_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        &counter_pbmp,
        "dnx_algo_mib_db[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        dnx_algo_mib_db_info,
        DNX_ALGO_MIB_DB_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_algo_mib_db_mib_counter_pbmp_get(int unit, bcm_pbmp_t *counter_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        counter_pbmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_READ);

    *counter_pbmp = ((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        &((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        "dnx_algo_mib_db[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        dnx_algo_mib_db_info,
        DNX_ALGO_MIB_DB_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_algo_mib_db_mib_counter_pbmp_pbmp_neq(int unit, _shr_pbmp_t input_pbmp, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_NEQ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_NEQ(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        ((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        input_pbmp,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_NEQ_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPNEQ,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        &((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        "dnx_algo_mib_db[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        dnx_algo_mib_db_info,
        DNX_ALGO_MIB_DB_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_algo_mib_db_mib_counter_pbmp_pbmp_eq(int unit, _shr_pbmp_t input_pbmp, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_EQ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_EQ(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        ((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        input_pbmp,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_EQ_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPEQ,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        &((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        "dnx_algo_mib_db[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        dnx_algo_mib_db_info,
        DNX_ALGO_MIB_DB_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_algo_mib_db_mib_counter_pbmp_pbmp_member(int unit, uint32 _input_port, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_MEMBER,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_MEMBER(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        ((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        _input_port,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_MEMBER_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPMEMBER,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        &((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        "dnx_algo_mib_db[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        dnx_algo_mib_db_info,
        DNX_ALGO_MIB_DB_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_algo_mib_db_mib_counter_pbmp_pbmp_not_null(int unit, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_NOT_NULL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_NOT_NULL(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        ((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_NOT_NULL_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPNOTNULL,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        &((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        "dnx_algo_mib_db[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        dnx_algo_mib_db_info,
        DNX_ALGO_MIB_DB_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_algo_mib_db_mib_counter_pbmp_pbmp_is_null(int unit, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_IS_NULL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_IS_NULL(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        ((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_IS_NULL_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPISNULL,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        &((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        "dnx_algo_mib_db[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        dnx_algo_mib_db_info,
        DNX_ALGO_MIB_DB_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_algo_mib_db_mib_counter_pbmp_pbmp_count(int unit, int *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_COUNT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_COUNT(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        ((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_COUNT_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPCOUNT,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        &((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        "dnx_algo_mib_db[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        dnx_algo_mib_db_info,
        DNX_ALGO_MIB_DB_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_algo_mib_db_mib_counter_pbmp_pbmp_xor(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_XOR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_XOR(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        ((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_XOR_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPXOR,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        &((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        "dnx_algo_mib_db[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        dnx_algo_mib_db_info,
        DNX_ALGO_MIB_DB_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_algo_mib_db_mib_counter_pbmp_pbmp_remove(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_REMOVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_REMOVE(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        ((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_REMOVE_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPREMOVE,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        &((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        "dnx_algo_mib_db[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        dnx_algo_mib_db_info,
        DNX_ALGO_MIB_DB_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_algo_mib_db_mib_counter_pbmp_pbmp_assign(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_ASSIGN,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_ASSIGN(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        ((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_ASSIGN_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPASSIGN,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        &((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        "dnx_algo_mib_db[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        dnx_algo_mib_db_info,
        DNX_ALGO_MIB_DB_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_algo_mib_db_mib_counter_pbmp_pbmp_get(int unit, _shr_pbmp_t *output_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_GET(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        ((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        output_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_GET_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPGET,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        &((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        "dnx_algo_mib_db[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        dnx_algo_mib_db_info,
        DNX_ALGO_MIB_DB_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_algo_mib_db_mib_counter_pbmp_pbmp_and(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_AND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_AND(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        ((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_AND_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPAND,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        &((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        "dnx_algo_mib_db[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        dnx_algo_mib_db_info,
        DNX_ALGO_MIB_DB_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_algo_mib_db_mib_counter_pbmp_pbmp_negate(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_NEGATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_NEGATE(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        ((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_NEGATE_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPNEGATE,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        &((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        "dnx_algo_mib_db[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        dnx_algo_mib_db_info,
        DNX_ALGO_MIB_DB_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_algo_mib_db_mib_counter_pbmp_pbmp_or(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_OR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_OR(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        ((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_OR_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPOR,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        &((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        "dnx_algo_mib_db[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        dnx_algo_mib_db_info,
        DNX_ALGO_MIB_DB_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_algo_mib_db_mib_counter_pbmp_pbmp_clear(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_CLEAR(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        ((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPCLEAR,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        &((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        "dnx_algo_mib_db[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        dnx_algo_mib_db_info,
        DNX_ALGO_MIB_DB_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_algo_mib_db_mib_counter_pbmp_pbmp_port_add(int unit, uint32 _input_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_PORT_ADD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_PORT_ADD(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        ((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        _input_port,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_PORT_ADD_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPPORTADD,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        &((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        "dnx_algo_mib_db[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        dnx_algo_mib_db_info,
        DNX_ALGO_MIB_DB_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_algo_mib_db_mib_counter_pbmp_pbmp_port_flip(int unit, uint32 _input_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_PORT_FLIP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_PORT_FLIP(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        ((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        _input_port,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_PORT_FLIP_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPPORTFLIP,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        &((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        "dnx_algo_mib_db[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        dnx_algo_mib_db_info,
        DNX_ALGO_MIB_DB_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_algo_mib_db_mib_counter_pbmp_pbmp_port_remove(int unit, uint32 _input_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_PORT_REMOVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_PORT_REMOVE(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        ((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        _input_port,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_PORT_REMOVE_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPPORTREMOVE,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        &((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        "dnx_algo_mib_db[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        dnx_algo_mib_db_info,
        DNX_ALGO_MIB_DB_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_algo_mib_db_mib_counter_pbmp_pbmp_port_set(int unit, uint32 _input_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_PORT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_PORT_SET(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        ((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        _input_port,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_PORT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPPORTSET,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        &((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        "dnx_algo_mib_db[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        dnx_algo_mib_db_info,
        DNX_ALGO_MIB_DB_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_algo_mib_db_mib_counter_pbmp_pbmp_ports_range_add(int unit, uint32 _first_port, uint32 _range)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_PORTS_RANGE_ADD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_PORTS_RANGE_ADD(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        ((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        _first_port,
        _range,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_PORTS_RANGE_ADD_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPPORTSRANGEADD,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        &((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        "dnx_algo_mib_db[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        dnx_algo_mib_db_info,
        DNX_ALGO_MIB_DB_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_algo_mib_db_mib_counter_pbmp_pbmp_fmt(int unit, char* _buffer)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_FMT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_FMT(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        ((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        _buffer,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_FMT_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPFMT,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        &((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.counter_pbmp,
        "dnx_algo_mib_db[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
        dnx_algo_mib_db_info,
        DNX_ALGO_MIB_DB_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_algo_mib_db_mib_interval_set(int unit, int interval)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__INTERVAL,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__INTERVAL,
        sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__INTERVAL,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__INTERVAL,
        ((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.interval,
        interval,
        int,
        0,
        "dnx_algo_mib_db_mib_interval_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__INTERVAL,
        &interval,
        "dnx_algo_mib_db[%d]->mib.interval",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__INTERVAL,
        dnx_algo_mib_db_info,
        DNX_ALGO_MIB_DB_MIB_INTERVAL_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_algo_mib_db_mib_interval_get(int unit, int *interval)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__INTERVAL,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__INTERVAL,
        sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__INTERVAL,
        interval);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__INTERVAL,
        DNX_SW_STATE_DIAG_READ);

    *interval = ((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.interval;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__INTERVAL,
        &((dnx_algo_mib_db_t*)sw_state_roots_array[unit][DNX_ALGO_MIB_MODULE_ID])->mib.interval,
        "dnx_algo_mib_db[%d]->mib.interval",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__INTERVAL,
        dnx_algo_mib_db_info,
        DNX_ALGO_MIB_DB_MIB_INTERVAL_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_ALGO_MIB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}




dnx_algo_mib_db_cbs dnx_algo_mib_db = 	{
	
	dnx_algo_mib_db_is_init,
	dnx_algo_mib_db_init,
		{
		
			{
			
			dnx_algo_mib_db_mib_counter_pbmp_set,
			dnx_algo_mib_db_mib_counter_pbmp_get,
			dnx_algo_mib_db_mib_counter_pbmp_pbmp_neq,
			dnx_algo_mib_db_mib_counter_pbmp_pbmp_eq,
			dnx_algo_mib_db_mib_counter_pbmp_pbmp_member,
			dnx_algo_mib_db_mib_counter_pbmp_pbmp_not_null,
			dnx_algo_mib_db_mib_counter_pbmp_pbmp_is_null,
			dnx_algo_mib_db_mib_counter_pbmp_pbmp_count,
			dnx_algo_mib_db_mib_counter_pbmp_pbmp_xor,
			dnx_algo_mib_db_mib_counter_pbmp_pbmp_remove,
			dnx_algo_mib_db_mib_counter_pbmp_pbmp_assign,
			dnx_algo_mib_db_mib_counter_pbmp_pbmp_get,
			dnx_algo_mib_db_mib_counter_pbmp_pbmp_and,
			dnx_algo_mib_db_mib_counter_pbmp_pbmp_negate,
			dnx_algo_mib_db_mib_counter_pbmp_pbmp_or,
			dnx_algo_mib_db_mib_counter_pbmp_pbmp_clear,
			dnx_algo_mib_db_mib_counter_pbmp_pbmp_port_add,
			dnx_algo_mib_db_mib_counter_pbmp_pbmp_port_flip,
			dnx_algo_mib_db_mib_counter_pbmp_pbmp_port_remove,
			dnx_algo_mib_db_mib_counter_pbmp_pbmp_port_set,
			dnx_algo_mib_db_mib_counter_pbmp_pbmp_ports_range_add,
			dnx_algo_mib_db_mib_counter_pbmp_pbmp_fmt}
		,
			{
			
			dnx_algo_mib_db_mib_interval_set,
			dnx_algo_mib_db_mib_interval_get}
		}
	}
;
#undef BSL_LOG_MODULE
