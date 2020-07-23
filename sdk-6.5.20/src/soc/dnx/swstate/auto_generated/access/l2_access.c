
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
#include <soc/dnx/swstate/auto_generated/access/l2_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/l2_diagnostic.h>



l2_db_t* l2_db_context_dummy = NULL;



int
l2_db_context_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        L2_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        L2_MODULE_ID,
        ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID]),
        "l2_db_context[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        L2_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        L2_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        l2_db_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "l2_db_context_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        L2_MODULE_ID,
        ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID]),
        "l2_db_context[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        L2_MODULE_ID,
        l2_db_context_info,
        L2_DB_CONTEXT_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(l2_db_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_fid_profiles_changed_set(int unit, uint8 fid_profiles_changed)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        L2_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        L2_MODULE_ID,
        sw_state_roots_array[unit][L2_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        L2_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        L2_MODULE_ID,
        ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->fid_profiles_changed,
        fid_profiles_changed,
        uint8,
        0,
        "l2_db_context_fid_profiles_changed_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        L2_MODULE_ID,
        &fid_profiles_changed,
        "l2_db_context[%d]->fid_profiles_changed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        L2_MODULE_ID,
        l2_db_context_info,
        L2_DB_CONTEXT_FID_PROFILES_CHANGED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_fid_profiles_changed_get(int unit, uint8 *fid_profiles_changed)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        L2_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        L2_MODULE_ID,
        sw_state_roots_array[unit][L2_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        L2_MODULE_ID,
        fid_profiles_changed);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        L2_MODULE_ID);

    *fid_profiles_changed = ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->fid_profiles_changed;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        L2_MODULE_ID,
        &((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->fid_profiles_changed,
        "l2_db_context[%d]->fid_profiles_changed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        L2_MODULE_ID,
        l2_db_context_info,
        L2_DB_CONTEXT_FID_PROFILES_CHANGED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_opportunistic_learning_ctx_ipv4uc_counter_set(int unit, int ipv4uc_counter)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        L2_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        L2_MODULE_ID,
        sw_state_roots_array[unit][L2_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        L2_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        L2_MODULE_ID,
        ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv4uc_counter,
        ipv4uc_counter,
        int,
        0,
        "l2_db_context_opportunistic_learning_ctx_ipv4uc_counter_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        L2_MODULE_ID,
        &ipv4uc_counter,
        "l2_db_context[%d]->opportunistic_learning_ctx.ipv4uc_counter",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        L2_MODULE_ID,
        l2_db_context_info,
        L2_DB_CONTEXT_OPPORTUNISTIC_LEARNING_CTX_IPV4UC_COUNTER_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_opportunistic_learning_ctx_ipv4uc_counter_get(int unit, int *ipv4uc_counter)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        L2_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        L2_MODULE_ID,
        sw_state_roots_array[unit][L2_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        L2_MODULE_ID,
        ipv4uc_counter);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        L2_MODULE_ID);

    *ipv4uc_counter = ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv4uc_counter;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        L2_MODULE_ID,
        &((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv4uc_counter,
        "l2_db_context[%d]->opportunistic_learning_ctx.ipv4uc_counter",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        L2_MODULE_ID,
        l2_db_context_info,
        L2_DB_CONTEXT_OPPORTUNISTIC_LEARNING_CTX_IPV4UC_COUNTER_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_opportunistic_learning_ctx_ipv4mc_counter_set(int unit, int ipv4mc_counter)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        L2_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        L2_MODULE_ID,
        sw_state_roots_array[unit][L2_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        L2_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        L2_MODULE_ID,
        ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv4mc_counter,
        ipv4mc_counter,
        int,
        0,
        "l2_db_context_opportunistic_learning_ctx_ipv4mc_counter_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        L2_MODULE_ID,
        &ipv4mc_counter,
        "l2_db_context[%d]->opportunistic_learning_ctx.ipv4mc_counter",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        L2_MODULE_ID,
        l2_db_context_info,
        L2_DB_CONTEXT_OPPORTUNISTIC_LEARNING_CTX_IPV4MC_COUNTER_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_opportunistic_learning_ctx_ipv4mc_counter_get(int unit, int *ipv4mc_counter)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        L2_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        L2_MODULE_ID,
        sw_state_roots_array[unit][L2_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        L2_MODULE_ID,
        ipv4mc_counter);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        L2_MODULE_ID);

    *ipv4mc_counter = ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv4mc_counter;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        L2_MODULE_ID,
        &((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv4mc_counter,
        "l2_db_context[%d]->opportunistic_learning_ctx.ipv4mc_counter",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        L2_MODULE_ID,
        l2_db_context_info,
        L2_DB_CONTEXT_OPPORTUNISTIC_LEARNING_CTX_IPV4MC_COUNTER_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_opportunistic_learning_ctx_ipv6uc_counter_set(int unit, int ipv6uc_counter)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        L2_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        L2_MODULE_ID,
        sw_state_roots_array[unit][L2_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        L2_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        L2_MODULE_ID,
        ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv6uc_counter,
        ipv6uc_counter,
        int,
        0,
        "l2_db_context_opportunistic_learning_ctx_ipv6uc_counter_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        L2_MODULE_ID,
        &ipv6uc_counter,
        "l2_db_context[%d]->opportunistic_learning_ctx.ipv6uc_counter",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        L2_MODULE_ID,
        l2_db_context_info,
        L2_DB_CONTEXT_OPPORTUNISTIC_LEARNING_CTX_IPV6UC_COUNTER_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_opportunistic_learning_ctx_ipv6uc_counter_get(int unit, int *ipv6uc_counter)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        L2_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        L2_MODULE_ID,
        sw_state_roots_array[unit][L2_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        L2_MODULE_ID,
        ipv6uc_counter);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        L2_MODULE_ID);

    *ipv6uc_counter = ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv6uc_counter;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        L2_MODULE_ID,
        &((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv6uc_counter,
        "l2_db_context[%d]->opportunistic_learning_ctx.ipv6uc_counter",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        L2_MODULE_ID,
        l2_db_context_info,
        L2_DB_CONTEXT_OPPORTUNISTIC_LEARNING_CTX_IPV6UC_COUNTER_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_opportunistic_learning_ctx_ipv6mc_counter_set(int unit, int ipv6mc_counter)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        L2_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        L2_MODULE_ID,
        sw_state_roots_array[unit][L2_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        L2_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        L2_MODULE_ID,
        ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv6mc_counter,
        ipv6mc_counter,
        int,
        0,
        "l2_db_context_opportunistic_learning_ctx_ipv6mc_counter_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        L2_MODULE_ID,
        &ipv6mc_counter,
        "l2_db_context[%d]->opportunistic_learning_ctx.ipv6mc_counter",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        L2_MODULE_ID,
        l2_db_context_info,
        L2_DB_CONTEXT_OPPORTUNISTIC_LEARNING_CTX_IPV6MC_COUNTER_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_opportunistic_learning_ctx_ipv6mc_counter_get(int unit, int *ipv6mc_counter)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        L2_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        L2_MODULE_ID,
        sw_state_roots_array[unit][L2_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        L2_MODULE_ID,
        ipv6mc_counter);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        L2_MODULE_ID);

    *ipv6mc_counter = ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv6mc_counter;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        L2_MODULE_ID,
        &((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv6mc_counter,
        "l2_db_context[%d]->opportunistic_learning_ctx.ipv6mc_counter",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        L2_MODULE_ID,
        l2_db_context_info,
        L2_DB_CONTEXT_OPPORTUNISTIC_LEARNING_CTX_IPV6MC_COUNTER_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_opportunistic_learning_ctx_mpls_counter_set(int unit, int mpls_counter)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        L2_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        L2_MODULE_ID,
        sw_state_roots_array[unit][L2_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        L2_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        L2_MODULE_ID,
        ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.mpls_counter,
        mpls_counter,
        int,
        0,
        "l2_db_context_opportunistic_learning_ctx_mpls_counter_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        L2_MODULE_ID,
        &mpls_counter,
        "l2_db_context[%d]->opportunistic_learning_ctx.mpls_counter",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        L2_MODULE_ID,
        l2_db_context_info,
        L2_DB_CONTEXT_OPPORTUNISTIC_LEARNING_CTX_MPLS_COUNTER_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_opportunistic_learning_ctx_mpls_counter_get(int unit, int *mpls_counter)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        L2_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        L2_MODULE_ID,
        sw_state_roots_array[unit][L2_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        L2_MODULE_ID,
        mpls_counter);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        L2_MODULE_ID);

    *mpls_counter = ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.mpls_counter;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        L2_MODULE_ID,
        &((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.mpls_counter,
        "l2_db_context[%d]->opportunistic_learning_ctx.mpls_counter",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        L2_MODULE_ID,
        l2_db_context_info,
        L2_DB_CONTEXT_OPPORTUNISTIC_LEARNING_CTX_MPLS_COUNTER_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_opportunistic_learning_ctx_ipv4uc_set(int unit, uint32 ipv4uc_idx_0, int ipv4uc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        L2_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        L2_MODULE_ID,
        sw_state_roots_array[unit][L2_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        ipv4uc_idx_0,
        64);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        L2_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        L2_MODULE_ID,
        ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv4uc[ipv4uc_idx_0],
        ipv4uc,
        int,
        0,
        "l2_db_context_opportunistic_learning_ctx_ipv4uc_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        L2_MODULE_ID,
        &ipv4uc,
        "l2_db_context[%d]->opportunistic_learning_ctx.ipv4uc[%d]",
        unit, ipv4uc_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        L2_MODULE_ID,
        l2_db_context_info,
        L2_DB_CONTEXT_OPPORTUNISTIC_LEARNING_CTX_IPV4UC_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_opportunistic_learning_ctx_ipv4uc_get(int unit, uint32 ipv4uc_idx_0, int *ipv4uc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        L2_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        L2_MODULE_ID,
        sw_state_roots_array[unit][L2_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        ipv4uc_idx_0,
        64);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        L2_MODULE_ID,
        ipv4uc);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        L2_MODULE_ID);

    *ipv4uc = ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv4uc[ipv4uc_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        L2_MODULE_ID,
        &((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv4uc[ipv4uc_idx_0],
        "l2_db_context[%d]->opportunistic_learning_ctx.ipv4uc[%d]",
        unit, ipv4uc_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        L2_MODULE_ID,
        l2_db_context_info,
        L2_DB_CONTEXT_OPPORTUNISTIC_LEARNING_CTX_IPV4UC_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_opportunistic_learning_ctx_ipv4mc_set(int unit, uint32 ipv4mc_idx_0, int ipv4mc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        L2_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        L2_MODULE_ID,
        sw_state_roots_array[unit][L2_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        ipv4mc_idx_0,
        64);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        L2_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        L2_MODULE_ID,
        ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv4mc[ipv4mc_idx_0],
        ipv4mc,
        int,
        0,
        "l2_db_context_opportunistic_learning_ctx_ipv4mc_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        L2_MODULE_ID,
        &ipv4mc,
        "l2_db_context[%d]->opportunistic_learning_ctx.ipv4mc[%d]",
        unit, ipv4mc_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        L2_MODULE_ID,
        l2_db_context_info,
        L2_DB_CONTEXT_OPPORTUNISTIC_LEARNING_CTX_IPV4MC_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_opportunistic_learning_ctx_ipv4mc_get(int unit, uint32 ipv4mc_idx_0, int *ipv4mc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        L2_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        L2_MODULE_ID,
        sw_state_roots_array[unit][L2_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        ipv4mc_idx_0,
        64);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        L2_MODULE_ID,
        ipv4mc);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        L2_MODULE_ID);

    *ipv4mc = ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv4mc[ipv4mc_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        L2_MODULE_ID,
        &((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv4mc[ipv4mc_idx_0],
        "l2_db_context[%d]->opportunistic_learning_ctx.ipv4mc[%d]",
        unit, ipv4mc_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        L2_MODULE_ID,
        l2_db_context_info,
        L2_DB_CONTEXT_OPPORTUNISTIC_LEARNING_CTX_IPV4MC_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_opportunistic_learning_ctx_ipv6uc_set(int unit, uint32 ipv6uc_idx_0, int ipv6uc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        L2_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        L2_MODULE_ID,
        sw_state_roots_array[unit][L2_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        ipv6uc_idx_0,
        64);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        L2_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        L2_MODULE_ID,
        ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv6uc[ipv6uc_idx_0],
        ipv6uc,
        int,
        0,
        "l2_db_context_opportunistic_learning_ctx_ipv6uc_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        L2_MODULE_ID,
        &ipv6uc,
        "l2_db_context[%d]->opportunistic_learning_ctx.ipv6uc[%d]",
        unit, ipv6uc_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        L2_MODULE_ID,
        l2_db_context_info,
        L2_DB_CONTEXT_OPPORTUNISTIC_LEARNING_CTX_IPV6UC_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_opportunistic_learning_ctx_ipv6uc_get(int unit, uint32 ipv6uc_idx_0, int *ipv6uc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        L2_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        L2_MODULE_ID,
        sw_state_roots_array[unit][L2_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        ipv6uc_idx_0,
        64);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        L2_MODULE_ID,
        ipv6uc);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        L2_MODULE_ID);

    *ipv6uc = ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv6uc[ipv6uc_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        L2_MODULE_ID,
        &((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv6uc[ipv6uc_idx_0],
        "l2_db_context[%d]->opportunistic_learning_ctx.ipv6uc[%d]",
        unit, ipv6uc_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        L2_MODULE_ID,
        l2_db_context_info,
        L2_DB_CONTEXT_OPPORTUNISTIC_LEARNING_CTX_IPV6UC_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_opportunistic_learning_ctx_ipv6mc_set(int unit, uint32 ipv6mc_idx_0, int ipv6mc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        L2_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        L2_MODULE_ID,
        sw_state_roots_array[unit][L2_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        ipv6mc_idx_0,
        64);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        L2_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        L2_MODULE_ID,
        ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv6mc[ipv6mc_idx_0],
        ipv6mc,
        int,
        0,
        "l2_db_context_opportunistic_learning_ctx_ipv6mc_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        L2_MODULE_ID,
        &ipv6mc,
        "l2_db_context[%d]->opportunistic_learning_ctx.ipv6mc[%d]",
        unit, ipv6mc_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        L2_MODULE_ID,
        l2_db_context_info,
        L2_DB_CONTEXT_OPPORTUNISTIC_LEARNING_CTX_IPV6MC_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_opportunistic_learning_ctx_ipv6mc_get(int unit, uint32 ipv6mc_idx_0, int *ipv6mc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        L2_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        L2_MODULE_ID,
        sw_state_roots_array[unit][L2_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        ipv6mc_idx_0,
        64);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        L2_MODULE_ID,
        ipv6mc);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        L2_MODULE_ID);

    *ipv6mc = ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv6mc[ipv6mc_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        L2_MODULE_ID,
        &((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv6mc[ipv6mc_idx_0],
        "l2_db_context[%d]->opportunistic_learning_ctx.ipv6mc[%d]",
        unit, ipv6mc_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        L2_MODULE_ID,
        l2_db_context_info,
        L2_DB_CONTEXT_OPPORTUNISTIC_LEARNING_CTX_IPV6MC_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_opportunistic_learning_ctx_mpls_set(int unit, uint32 mpls_idx_0, int mpls)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        L2_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        L2_MODULE_ID,
        sw_state_roots_array[unit][L2_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        mpls_idx_0,
        64);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        L2_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        L2_MODULE_ID,
        ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.mpls[mpls_idx_0],
        mpls,
        int,
        0,
        "l2_db_context_opportunistic_learning_ctx_mpls_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        L2_MODULE_ID,
        &mpls,
        "l2_db_context[%d]->opportunistic_learning_ctx.mpls[%d]",
        unit, mpls_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        L2_MODULE_ID,
        l2_db_context_info,
        L2_DB_CONTEXT_OPPORTUNISTIC_LEARNING_CTX_MPLS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_opportunistic_learning_ctx_mpls_get(int unit, uint32 mpls_idx_0, int *mpls)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        L2_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        L2_MODULE_ID,
        sw_state_roots_array[unit][L2_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        mpls_idx_0,
        64);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        L2_MODULE_ID,
        mpls);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        L2_MODULE_ID);

    *mpls = ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.mpls[mpls_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        L2_MODULE_ID,
        &((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.mpls[mpls_idx_0],
        "l2_db_context[%d]->opportunistic_learning_ctx.mpls[%d]",
        unit, mpls_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        L2_MODULE_ID,
        l2_db_context_info,
        L2_DB_CONTEXT_OPPORTUNISTIC_LEARNING_CTX_MPLS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_routing_learning_set(int unit, uint32 routing_learning)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        L2_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        L2_MODULE_ID,
        sw_state_roots_array[unit][L2_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        L2_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        L2_MODULE_ID,
        ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->routing_learning,
        routing_learning,
        uint32,
        0,
        "l2_db_context_routing_learning_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        L2_MODULE_ID,
        &routing_learning,
        "l2_db_context[%d]->routing_learning",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        L2_MODULE_ID,
        l2_db_context_info,
        L2_DB_CONTEXT_ROUTING_LEARNING_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_routing_learning_get(int unit, uint32 *routing_learning)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        L2_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        L2_MODULE_ID,
        sw_state_roots_array[unit][L2_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        L2_MODULE_ID,
        routing_learning);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        L2_MODULE_ID);

    *routing_learning = ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->routing_learning;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        L2_MODULE_ID,
        &((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->routing_learning,
        "l2_db_context[%d]->routing_learning",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        L2_MODULE_ID,
        l2_db_context_info,
        L2_DB_CONTEXT_ROUTING_LEARNING_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}





l2_db_context_cbs l2_db_context = 	{
	
	l2_db_context_is_init,
	l2_db_context_init,
		{
		
		l2_db_context_fid_profiles_changed_set,
		l2_db_context_fid_profiles_changed_get}
	,
		{
		
			{
			
			l2_db_context_opportunistic_learning_ctx_ipv4uc_counter_set,
			l2_db_context_opportunistic_learning_ctx_ipv4uc_counter_get}
		,
			{
			
			l2_db_context_opportunistic_learning_ctx_ipv4mc_counter_set,
			l2_db_context_opportunistic_learning_ctx_ipv4mc_counter_get}
		,
			{
			
			l2_db_context_opportunistic_learning_ctx_ipv6uc_counter_set,
			l2_db_context_opportunistic_learning_ctx_ipv6uc_counter_get}
		,
			{
			
			l2_db_context_opportunistic_learning_ctx_ipv6mc_counter_set,
			l2_db_context_opportunistic_learning_ctx_ipv6mc_counter_get}
		,
			{
			
			l2_db_context_opportunistic_learning_ctx_mpls_counter_set,
			l2_db_context_opportunistic_learning_ctx_mpls_counter_get}
		,
			{
			
			l2_db_context_opportunistic_learning_ctx_ipv4uc_set,
			l2_db_context_opportunistic_learning_ctx_ipv4uc_get}
		,
			{
			
			l2_db_context_opportunistic_learning_ctx_ipv4mc_set,
			l2_db_context_opportunistic_learning_ctx_ipv4mc_get}
		,
			{
			
			l2_db_context_opportunistic_learning_ctx_ipv6uc_set,
			l2_db_context_opportunistic_learning_ctx_ipv6uc_get}
		,
			{
			
			l2_db_context_opportunistic_learning_ctx_ipv6mc_set,
			l2_db_context_opportunistic_learning_ctx_ipv6mc_get}
		,
			{
			
			l2_db_context_opportunistic_learning_ctx_mpls_set,
			l2_db_context_opportunistic_learning_ctx_mpls_get}
		}
	,
		{
		
		l2_db_context_routing_learning_set,
		l2_db_context_routing_learning_get}
	}
;
#undef BSL_LOG_MODULE
