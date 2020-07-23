
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnxc/swstate/dnxc_sw_state_c_includes.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_cosq_ingress_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_cosq_ingress_diagnostic.h>



dnx_cosq_ingress_db_t* dnx_cosq_ingress_db_dummy = NULL;



int
dnx_cosq_ingress_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_COSQ_INGRESS_MODULE_ID,
        ((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID]),
        "dnx_cosq_ingress_db[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_cosq_ingress_db_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_cosq_ingress_db_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_cosq_ingress_db_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_COSQ_INGRESS_MODULE_ID,
        ((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID]),
        "dnx_cosq_ingress_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        dnx_cosq_ingress_db_info,
        DNX_COSQ_INGRESS_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(dnx_cosq_ingress_db_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_cosq_ingress_db_fixed_priority_nif_set(int unit, bcm_pbmp_t nif)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        ((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        nif,
        bcm_pbmp_t,
        0,
        "dnx_cosq_ingress_db_fixed_priority_nif_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_COSQ_INGRESS_MODULE_ID,
        &nif,
        "dnx_cosq_ingress_db[%d]->fixed_priority.nif",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        dnx_cosq_ingress_db_info,
        DNX_COSQ_INGRESS_DB_FIXED_PRIORITY_NIF_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_cosq_ingress_db_fixed_priority_nif_get(int unit, bcm_pbmp_t *nif)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        nif);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID);

    *nif = ((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_COSQ_INGRESS_MODULE_ID,
        &((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        "dnx_cosq_ingress_db[%d]->fixed_priority.nif",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        dnx_cosq_ingress_db_info,
        DNX_COSQ_INGRESS_DB_FIXED_PRIORITY_NIF_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_cosq_ingress_db_fixed_priority_nif_pbmp_neq(int unit, _shr_pbmp_t input_pbmp, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        SW_STATE_FUNC_PBMP_NEQ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID);

    DNX_SW_STATE_PBMP_NEQ(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        ((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        input_pbmp,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_NEQ_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPNEQ,
        DNX_COSQ_INGRESS_MODULE_ID,
        &((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        "dnx_cosq_ingress_db[%d]->fixed_priority.nif",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        dnx_cosq_ingress_db_info,
        DNX_COSQ_INGRESS_DB_FIXED_PRIORITY_NIF_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_cosq_ingress_db_fixed_priority_nif_pbmp_eq(int unit, _shr_pbmp_t input_pbmp, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        SW_STATE_FUNC_PBMP_EQ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID);

    DNX_SW_STATE_PBMP_EQ(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        ((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        input_pbmp,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_EQ_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPEQ,
        DNX_COSQ_INGRESS_MODULE_ID,
        &((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        "dnx_cosq_ingress_db[%d]->fixed_priority.nif",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        dnx_cosq_ingress_db_info,
        DNX_COSQ_INGRESS_DB_FIXED_PRIORITY_NIF_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_cosq_ingress_db_fixed_priority_nif_pbmp_member(int unit, uint32 _input_port, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        SW_STATE_FUNC_PBMP_MEMBER,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID);

    DNX_SW_STATE_PBMP_MEMBER(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        ((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        _input_port,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_MEMBER_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPMEMBER,
        DNX_COSQ_INGRESS_MODULE_ID,
        &((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        "dnx_cosq_ingress_db[%d]->fixed_priority.nif",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        dnx_cosq_ingress_db_info,
        DNX_COSQ_INGRESS_DB_FIXED_PRIORITY_NIF_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_cosq_ingress_db_fixed_priority_nif_pbmp_not_null(int unit, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        SW_STATE_FUNC_PBMP_NOT_NULL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID);

    DNX_SW_STATE_PBMP_NOT_NULL(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        ((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_NOT_NULL_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPNOTNULL,
        DNX_COSQ_INGRESS_MODULE_ID,
        &((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        "dnx_cosq_ingress_db[%d]->fixed_priority.nif",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        dnx_cosq_ingress_db_info,
        DNX_COSQ_INGRESS_DB_FIXED_PRIORITY_NIF_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_cosq_ingress_db_fixed_priority_nif_pbmp_is_null(int unit, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        SW_STATE_FUNC_PBMP_IS_NULL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID);

    DNX_SW_STATE_PBMP_IS_NULL(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        ((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_IS_NULL_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPISNULL,
        DNX_COSQ_INGRESS_MODULE_ID,
        &((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        "dnx_cosq_ingress_db[%d]->fixed_priority.nif",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        dnx_cosq_ingress_db_info,
        DNX_COSQ_INGRESS_DB_FIXED_PRIORITY_NIF_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_cosq_ingress_db_fixed_priority_nif_pbmp_count(int unit, int *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        SW_STATE_FUNC_PBMP_COUNT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID);

    DNX_SW_STATE_PBMP_COUNT(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        ((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_COUNT_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPCOUNT,
        DNX_COSQ_INGRESS_MODULE_ID,
        &((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        "dnx_cosq_ingress_db[%d]->fixed_priority.nif",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        dnx_cosq_ingress_db_info,
        DNX_COSQ_INGRESS_DB_FIXED_PRIORITY_NIF_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_cosq_ingress_db_fixed_priority_nif_pbmp_xor(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        SW_STATE_FUNC_PBMP_XOR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID);

    DNX_SW_STATE_PBMP_XOR(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        ((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_XOR_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPXOR,
        DNX_COSQ_INGRESS_MODULE_ID,
        &((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        "dnx_cosq_ingress_db[%d]->fixed_priority.nif",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        dnx_cosq_ingress_db_info,
        DNX_COSQ_INGRESS_DB_FIXED_PRIORITY_NIF_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_cosq_ingress_db_fixed_priority_nif_pbmp_remove(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        SW_STATE_FUNC_PBMP_REMOVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID);

    DNX_SW_STATE_PBMP_REMOVE(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        ((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_REMOVE_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPREMOVE,
        DNX_COSQ_INGRESS_MODULE_ID,
        &((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        "dnx_cosq_ingress_db[%d]->fixed_priority.nif",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        dnx_cosq_ingress_db_info,
        DNX_COSQ_INGRESS_DB_FIXED_PRIORITY_NIF_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_cosq_ingress_db_fixed_priority_nif_pbmp_assign(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        SW_STATE_FUNC_PBMP_ASSIGN,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID);

    DNX_SW_STATE_PBMP_ASSIGN(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        ((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_ASSIGN_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPASSIGN,
        DNX_COSQ_INGRESS_MODULE_ID,
        &((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        "dnx_cosq_ingress_db[%d]->fixed_priority.nif",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        dnx_cosq_ingress_db_info,
        DNX_COSQ_INGRESS_DB_FIXED_PRIORITY_NIF_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_cosq_ingress_db_fixed_priority_nif_pbmp_get(int unit, _shr_pbmp_t *output_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        SW_STATE_FUNC_PBMP_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID);

    DNX_SW_STATE_PBMP_GET(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        ((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        output_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_GET_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPGET,
        DNX_COSQ_INGRESS_MODULE_ID,
        &((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        "dnx_cosq_ingress_db[%d]->fixed_priority.nif",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        dnx_cosq_ingress_db_info,
        DNX_COSQ_INGRESS_DB_FIXED_PRIORITY_NIF_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_cosq_ingress_db_fixed_priority_nif_pbmp_and(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        SW_STATE_FUNC_PBMP_AND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID);

    DNX_SW_STATE_PBMP_AND(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        ((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_AND_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPAND,
        DNX_COSQ_INGRESS_MODULE_ID,
        &((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        "dnx_cosq_ingress_db[%d]->fixed_priority.nif",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        dnx_cosq_ingress_db_info,
        DNX_COSQ_INGRESS_DB_FIXED_PRIORITY_NIF_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_cosq_ingress_db_fixed_priority_nif_pbmp_negate(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        SW_STATE_FUNC_PBMP_NEGATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID);

    DNX_SW_STATE_PBMP_NEGATE(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        ((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_NEGATE_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPNEGATE,
        DNX_COSQ_INGRESS_MODULE_ID,
        &((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        "dnx_cosq_ingress_db[%d]->fixed_priority.nif",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        dnx_cosq_ingress_db_info,
        DNX_COSQ_INGRESS_DB_FIXED_PRIORITY_NIF_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_cosq_ingress_db_fixed_priority_nif_pbmp_or(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        SW_STATE_FUNC_PBMP_OR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID);

    DNX_SW_STATE_PBMP_OR(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        ((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_OR_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPOR,
        DNX_COSQ_INGRESS_MODULE_ID,
        &((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        "dnx_cosq_ingress_db[%d]->fixed_priority.nif",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        dnx_cosq_ingress_db_info,
        DNX_COSQ_INGRESS_DB_FIXED_PRIORITY_NIF_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_cosq_ingress_db_fixed_priority_nif_pbmp_clear(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        SW_STATE_FUNC_PBMP_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID);

    DNX_SW_STATE_PBMP_CLEAR(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        ((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPCLEAR,
        DNX_COSQ_INGRESS_MODULE_ID,
        &((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        "dnx_cosq_ingress_db[%d]->fixed_priority.nif",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        dnx_cosq_ingress_db_info,
        DNX_COSQ_INGRESS_DB_FIXED_PRIORITY_NIF_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_cosq_ingress_db_fixed_priority_nif_pbmp_port_add(int unit, uint32 _input_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        SW_STATE_FUNC_PBMP_PORT_ADD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID);

    DNX_SW_STATE_PBMP_PORT_ADD(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        ((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        _input_port,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_PORT_ADD_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPPORTADD,
        DNX_COSQ_INGRESS_MODULE_ID,
        &((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        "dnx_cosq_ingress_db[%d]->fixed_priority.nif",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        dnx_cosq_ingress_db_info,
        DNX_COSQ_INGRESS_DB_FIXED_PRIORITY_NIF_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_cosq_ingress_db_fixed_priority_nif_pbmp_port_flip(int unit, uint32 _input_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        SW_STATE_FUNC_PBMP_PORT_FLIP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID);

    DNX_SW_STATE_PBMP_PORT_FLIP(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        ((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        _input_port,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_PORT_FLIP_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPPORTFLIP,
        DNX_COSQ_INGRESS_MODULE_ID,
        &((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        "dnx_cosq_ingress_db[%d]->fixed_priority.nif",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        dnx_cosq_ingress_db_info,
        DNX_COSQ_INGRESS_DB_FIXED_PRIORITY_NIF_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_cosq_ingress_db_fixed_priority_nif_pbmp_port_remove(int unit, uint32 _input_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        SW_STATE_FUNC_PBMP_PORT_REMOVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID);

    DNX_SW_STATE_PBMP_PORT_REMOVE(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        ((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        _input_port,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_PORT_REMOVE_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPPORTREMOVE,
        DNX_COSQ_INGRESS_MODULE_ID,
        &((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        "dnx_cosq_ingress_db[%d]->fixed_priority.nif",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        dnx_cosq_ingress_db_info,
        DNX_COSQ_INGRESS_DB_FIXED_PRIORITY_NIF_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_cosq_ingress_db_fixed_priority_nif_pbmp_port_set(int unit, uint32 _input_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        SW_STATE_FUNC_PBMP_PORT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID);

    DNX_SW_STATE_PBMP_PORT_SET(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        ((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        _input_port,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_PORT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPPORTSET,
        DNX_COSQ_INGRESS_MODULE_ID,
        &((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        "dnx_cosq_ingress_db[%d]->fixed_priority.nif",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        dnx_cosq_ingress_db_info,
        DNX_COSQ_INGRESS_DB_FIXED_PRIORITY_NIF_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_cosq_ingress_db_fixed_priority_nif_pbmp_ports_range_add(int unit, uint32 _first_port, uint32 _range)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        SW_STATE_FUNC_PBMP_PORTS_RANGE_ADD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID);

    DNX_SW_STATE_PBMP_PORTS_RANGE_ADD(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        ((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        _first_port,
        _range,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_PORTS_RANGE_ADD_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPPORTSRANGEADD,
        DNX_COSQ_INGRESS_MODULE_ID,
        &((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        "dnx_cosq_ingress_db[%d]->fixed_priority.nif",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        dnx_cosq_ingress_db_info,
        DNX_COSQ_INGRESS_DB_FIXED_PRIORITY_NIF_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_cosq_ingress_db_fixed_priority_nif_pbmp_fmt(int unit, char* _buffer)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        SW_STATE_FUNC_PBMP_FMT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID);

    DNX_SW_STATE_PBMP_FMT(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        ((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        _buffer,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_FMT_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPFMT,
        DNX_COSQ_INGRESS_MODULE_ID,
        &((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
        "dnx_cosq_ingress_db[%d]->fixed_priority.nif",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        dnx_cosq_ingress_db_info,
        DNX_COSQ_INGRESS_DB_FIXED_PRIORITY_NIF_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_cosq_ingress_db_fixed_priority_mirror_bit_set(int unit, uint32 mirror_idx_0, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        SW_STATE_FUNC_BIT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        _bit_num,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        _bit_num,
        ((DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_EGR_INTERFACES)/SHR_BITWID)+1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID);

    DNX_SW_STATE_BIT_SET(
        unit,
         DNX_COSQ_INGRESS_MODULE_ID,
         0,
         ((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.mirror[mirror_idx_0],
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITSET,
        DNX_COSQ_INGRESS_MODULE_ID,
        &((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.mirror[mirror_idx_0],
        "dnx_cosq_ingress_db[%d]->fixed_priority.mirror[%d]",
        unit, mirror_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        dnx_cosq_ingress_db_info,
        DNX_COSQ_INGRESS_DB_FIXED_PRIORITY_MIRROR_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_cosq_ingress_db_fixed_priority_mirror_bit_clear(int unit, uint32 mirror_idx_0, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        SW_STATE_FUNC_BIT_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        _bit_num,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        _bit_num,
        ((DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_EGR_INTERFACES)/SHR_BITWID)+1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID);

    DNX_SW_STATE_BIT_CLEAR(
        unit,
         DNX_COSQ_INGRESS_MODULE_ID,
         0,
         ((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.mirror[mirror_idx_0],
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITCLEAR,
        DNX_COSQ_INGRESS_MODULE_ID,
        &((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.mirror[mirror_idx_0],
        "dnx_cosq_ingress_db[%d]->fixed_priority.mirror[%d]",
        unit, mirror_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        dnx_cosq_ingress_db_info,
        DNX_COSQ_INGRESS_DB_FIXED_PRIORITY_MIRROR_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_cosq_ingress_db_fixed_priority_mirror_bit_get(int unit, uint32 mirror_idx_0, uint32 _bit_num, uint8* result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        SW_STATE_FUNC_BIT_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        _bit_num,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(
        _bit_num,
        ((DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_EGR_INTERFACES)/SHR_BITWID)+1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID);

    DNX_SW_STATE_BIT_GET(
        unit,
         DNX_COSQ_INGRESS_MODULE_ID,
         0,
         ((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.mirror[mirror_idx_0],
         _bit_num,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_GET_LOGGING,
        BSL_LS_SWSTATEDNX_BITGET,
        DNX_COSQ_INGRESS_MODULE_ID,
        &((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.mirror[mirror_idx_0],
        "dnx_cosq_ingress_db[%d]->fixed_priority.mirror[%d]",
        unit, mirror_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_COSQ_INGRESS_MODULE_ID,
        dnx_cosq_ingress_db_info,
        DNX_COSQ_INGRESS_DB_FIXED_PRIORITY_MIRROR_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_cosq_ingress_db_cbs dnx_cosq_ingress_db = 	{
	
	dnx_cosq_ingress_db_is_init,
	dnx_cosq_ingress_db_init,
		{
		
			{
			
			dnx_cosq_ingress_db_fixed_priority_nif_set,
			dnx_cosq_ingress_db_fixed_priority_nif_get,
			dnx_cosq_ingress_db_fixed_priority_nif_pbmp_neq,
			dnx_cosq_ingress_db_fixed_priority_nif_pbmp_eq,
			dnx_cosq_ingress_db_fixed_priority_nif_pbmp_member,
			dnx_cosq_ingress_db_fixed_priority_nif_pbmp_not_null,
			dnx_cosq_ingress_db_fixed_priority_nif_pbmp_is_null,
			dnx_cosq_ingress_db_fixed_priority_nif_pbmp_count,
			dnx_cosq_ingress_db_fixed_priority_nif_pbmp_xor,
			dnx_cosq_ingress_db_fixed_priority_nif_pbmp_remove,
			dnx_cosq_ingress_db_fixed_priority_nif_pbmp_assign,
			dnx_cosq_ingress_db_fixed_priority_nif_pbmp_get,
			dnx_cosq_ingress_db_fixed_priority_nif_pbmp_and,
			dnx_cosq_ingress_db_fixed_priority_nif_pbmp_negate,
			dnx_cosq_ingress_db_fixed_priority_nif_pbmp_or,
			dnx_cosq_ingress_db_fixed_priority_nif_pbmp_clear,
			dnx_cosq_ingress_db_fixed_priority_nif_pbmp_port_add,
			dnx_cosq_ingress_db_fixed_priority_nif_pbmp_port_flip,
			dnx_cosq_ingress_db_fixed_priority_nif_pbmp_port_remove,
			dnx_cosq_ingress_db_fixed_priority_nif_pbmp_port_set,
			dnx_cosq_ingress_db_fixed_priority_nif_pbmp_ports_range_add,
			dnx_cosq_ingress_db_fixed_priority_nif_pbmp_fmt}
		,
			{
			
			dnx_cosq_ingress_db_fixed_priority_mirror_bit_set,
			dnx_cosq_ingress_db_fixed_priority_mirror_bit_clear,
			dnx_cosq_ingress_db_fixed_priority_mirror_bit_get}
		}
	}
;
#undef BSL_LOG_MODULE
