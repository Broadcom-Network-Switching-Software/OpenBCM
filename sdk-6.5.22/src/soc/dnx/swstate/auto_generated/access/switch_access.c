
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/switch_access.h>
#include <bcm/switch.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/switch_diagnostic.h>



switch_sw_state_t* switch_db_dummy = NULL;



int
switch_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        SWITCH_MODULE_ID,
        ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID]),
        "switch_db[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_init(int unit)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        SWITCH_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        switch_sw_state_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "switch_db_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        SWITCH_MODULE_ID,
        ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID]),
        "switch_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(switch_sw_state_t),
        NULL);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], bcmModuleCount)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->module_verification[def_val_idx[1]],
        TRUE);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], bcmModuleCount)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->module_error_recovery[def_val_idx[1]],
        TRUE);

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->l3mcastl2_ipv4_fwd_type,
        FALSE);

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->l3mcastl2_ipv6_fwd_type,
        FALSE);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_module_verification_set(int unit, uint32 module_verification_idx_0, uint8 module_verification)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        module_verification_idx_0,
        bcmModuleCount);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        SWITCH_MODULE_ID,
        ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->module_verification[module_verification_idx_0],
        module_verification,
        uint8,
        0,
        "switch_db_module_verification_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        SWITCH_MODULE_ID,
        &module_verification,
        "switch_db[%d]->module_verification[%d]",
        unit, module_verification_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_MODULE_VERIFICATION_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_module_verification_get(int unit, uint32 module_verification_idx_0, uint8 *module_verification)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        module_verification_idx_0,
        bcmModuleCount);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        SWITCH_MODULE_ID,
        module_verification);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    *module_verification = ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->module_verification[module_verification_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        SWITCH_MODULE_ID,
        &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->module_verification[module_verification_idx_0],
        "switch_db[%d]->module_verification[%d]",
        unit, module_verification_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_MODULE_VERIFICATION_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_module_error_recovery_set(int unit, uint32 module_error_recovery_idx_0, uint8 module_error_recovery)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        module_error_recovery_idx_0,
        bcmModuleCount);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        SWITCH_MODULE_ID,
        ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->module_error_recovery[module_error_recovery_idx_0],
        module_error_recovery,
        uint8,
        0,
        "switch_db_module_error_recovery_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        SWITCH_MODULE_ID,
        &module_error_recovery,
        "switch_db[%d]->module_error_recovery[%d]",
        unit, module_error_recovery_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_MODULE_ERROR_RECOVERY_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_module_error_recovery_get(int unit, uint32 module_error_recovery_idx_0, uint8 *module_error_recovery)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        module_error_recovery_idx_0,
        bcmModuleCount);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        SWITCH_MODULE_ID,
        module_error_recovery);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    *module_error_recovery = ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->module_error_recovery[module_error_recovery_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        SWITCH_MODULE_ID,
        &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->module_error_recovery[module_error_recovery_idx_0],
        "switch_db[%d]->module_error_recovery[%d]",
        unit, module_error_recovery_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_MODULE_ERROR_RECOVERY_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_l3mcastl2_ipv4_fwd_type_set(int unit, uint8 l3mcastl2_ipv4_fwd_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        SWITCH_MODULE_ID,
        ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->l3mcastl2_ipv4_fwd_type,
        l3mcastl2_ipv4_fwd_type,
        uint8,
        0,
        "switch_db_l3mcastl2_ipv4_fwd_type_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        SWITCH_MODULE_ID,
        &l3mcastl2_ipv4_fwd_type,
        "switch_db[%d]->l3mcastl2_ipv4_fwd_type",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_L3MCASTL2_IPV4_FWD_TYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_l3mcastl2_ipv4_fwd_type_get(int unit, uint8 *l3mcastl2_ipv4_fwd_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        SWITCH_MODULE_ID,
        l3mcastl2_ipv4_fwd_type);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    *l3mcastl2_ipv4_fwd_type = ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->l3mcastl2_ipv4_fwd_type;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        SWITCH_MODULE_ID,
        &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->l3mcastl2_ipv4_fwd_type,
        "switch_db[%d]->l3mcastl2_ipv4_fwd_type",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_L3MCASTL2_IPV4_FWD_TYPE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_l3mcastl2_ipv6_fwd_type_set(int unit, uint8 l3mcastl2_ipv6_fwd_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        SWITCH_MODULE_ID,
        ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->l3mcastl2_ipv6_fwd_type,
        l3mcastl2_ipv6_fwd_type,
        uint8,
        0,
        "switch_db_l3mcastl2_ipv6_fwd_type_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        SWITCH_MODULE_ID,
        &l3mcastl2_ipv6_fwd_type,
        "switch_db[%d]->l3mcastl2_ipv6_fwd_type",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_L3MCASTL2_IPV6_FWD_TYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_l3mcastl2_ipv6_fwd_type_get(int unit, uint8 *l3mcastl2_ipv6_fwd_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        SWITCH_MODULE_ID,
        l3mcastl2_ipv6_fwd_type);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    *l3mcastl2_ipv6_fwd_type = ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->l3mcastl2_ipv6_fwd_type;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        SWITCH_MODULE_ID,
        &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->l3mcastl2_ipv6_fwd_type,
        "switch_db[%d]->l3mcastl2_ipv6_fwd_type",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_L3MCASTL2_IPV6_FWD_TYPE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_header_enablers_hashing_set(int unit, uint32 header_enablers_hashing_idx_0, uint32 header_enablers_hashing)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        header_enablers_hashing_idx_0,
        DBAL_NOF_ENUM_HASH_FIELD_ENABLERS_HEADER_VALUES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        SWITCH_MODULE_ID,
        ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->header_enablers_hashing[header_enablers_hashing_idx_0],
        header_enablers_hashing,
        uint32,
        0,
        "switch_db_header_enablers_hashing_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        SWITCH_MODULE_ID,
        &header_enablers_hashing,
        "switch_db[%d]->header_enablers_hashing[%d]",
        unit, header_enablers_hashing_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_HEADER_ENABLERS_HASHING_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_header_enablers_hashing_get(int unit, uint32 header_enablers_hashing_idx_0, uint32 *header_enablers_hashing)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        header_enablers_hashing_idx_0,
        DBAL_NOF_ENUM_HASH_FIELD_ENABLERS_HEADER_VALUES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        SWITCH_MODULE_ID,
        header_enablers_hashing);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    *header_enablers_hashing = ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->header_enablers_hashing[header_enablers_hashing_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        SWITCH_MODULE_ID,
        &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->header_enablers_hashing[header_enablers_hashing_idx_0],
        "switch_db[%d]->header_enablers_hashing[%d]",
        unit, header_enablers_hashing_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_HEADER_ENABLERS_HASHING_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_tunnel_route_disabled_ctx_icmp_counter_set(int unit, uint32 icmp_counter_idx_0, int icmp_counter)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        icmp_counter_idx_0,
        4);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        SWITCH_MODULE_ID,
        ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.icmp_counter[icmp_counter_idx_0],
        icmp_counter,
        int,
        0,
        "switch_db_tunnel_route_disabled_ctx_icmp_counter_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        SWITCH_MODULE_ID,
        &icmp_counter,
        "switch_db[%d]->tunnel_route_disabled_ctx.icmp_counter[%d]",
        unit, icmp_counter_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_ICMP_COUNTER_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_tunnel_route_disabled_ctx_icmp_counter_get(int unit, uint32 icmp_counter_idx_0, int *icmp_counter)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        icmp_counter_idx_0,
        4);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        SWITCH_MODULE_ID,
        icmp_counter);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    *icmp_counter = ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.icmp_counter[icmp_counter_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        SWITCH_MODULE_ID,
        &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.icmp_counter[icmp_counter_idx_0],
        "switch_db[%d]->tunnel_route_disabled_ctx.icmp_counter[%d]",
        unit, icmp_counter_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_ICMP_COUNTER_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_tunnel_route_disabled_ctx_igmp_counter_set(int unit, uint32 igmp_counter_idx_0, int igmp_counter)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        igmp_counter_idx_0,
        4);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        SWITCH_MODULE_ID,
        ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.igmp_counter[igmp_counter_idx_0],
        igmp_counter,
        int,
        0,
        "switch_db_tunnel_route_disabled_ctx_igmp_counter_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        SWITCH_MODULE_ID,
        &igmp_counter,
        "switch_db[%d]->tunnel_route_disabled_ctx.igmp_counter[%d]",
        unit, igmp_counter_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_IGMP_COUNTER_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_tunnel_route_disabled_ctx_igmp_counter_get(int unit, uint32 igmp_counter_idx_0, int *igmp_counter)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        igmp_counter_idx_0,
        4);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        SWITCH_MODULE_ID,
        igmp_counter);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    *igmp_counter = ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.igmp_counter[igmp_counter_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        SWITCH_MODULE_ID,
        &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.igmp_counter[igmp_counter_idx_0],
        "switch_db[%d]->tunnel_route_disabled_ctx.igmp_counter[%d]",
        unit, igmp_counter_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_IGMP_COUNTER_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_tunnel_route_disabled_ctx_udp_counter_set(int unit, uint32 udp_counter_idx_0, int udp_counter)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        udp_counter_idx_0,
        4);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        SWITCH_MODULE_ID,
        ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.udp_counter[udp_counter_idx_0],
        udp_counter,
        int,
        0,
        "switch_db_tunnel_route_disabled_ctx_udp_counter_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        SWITCH_MODULE_ID,
        &udp_counter,
        "switch_db[%d]->tunnel_route_disabled_ctx.udp_counter[%d]",
        unit, udp_counter_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_UDP_COUNTER_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_tunnel_route_disabled_ctx_udp_counter_get(int unit, uint32 udp_counter_idx_0, int *udp_counter)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        udp_counter_idx_0,
        4);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        SWITCH_MODULE_ID,
        udp_counter);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    *udp_counter = ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.udp_counter[udp_counter_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        SWITCH_MODULE_ID,
        &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.udp_counter[udp_counter_idx_0],
        "switch_db[%d]->tunnel_route_disabled_ctx.udp_counter[%d]",
        unit, udp_counter_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_UDP_COUNTER_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_tunnel_route_disabled_ctx_tcp_counter_set(int unit, uint32 tcp_counter_idx_0, int tcp_counter)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        tcp_counter_idx_0,
        4);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        SWITCH_MODULE_ID,
        ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.tcp_counter[tcp_counter_idx_0],
        tcp_counter,
        int,
        0,
        "switch_db_tunnel_route_disabled_ctx_tcp_counter_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        SWITCH_MODULE_ID,
        &tcp_counter,
        "switch_db[%d]->tunnel_route_disabled_ctx.tcp_counter[%d]",
        unit, tcp_counter_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_TCP_COUNTER_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_tunnel_route_disabled_ctx_tcp_counter_get(int unit, uint32 tcp_counter_idx_0, int *tcp_counter)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        tcp_counter_idx_0,
        4);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        SWITCH_MODULE_ID,
        tcp_counter);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    *tcp_counter = ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.tcp_counter[tcp_counter_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        SWITCH_MODULE_ID,
        &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.tcp_counter[tcp_counter_idx_0],
        "switch_db[%d]->tunnel_route_disabled_ctx.tcp_counter[%d]",
        unit, tcp_counter_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_TCP_COUNTER_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_tunnel_route_disabled_ctx_unknown_counter_set(int unit, uint32 unknown_counter_idx_0, int unknown_counter)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        unknown_counter_idx_0,
        4);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        SWITCH_MODULE_ID,
        ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.unknown_counter[unknown_counter_idx_0],
        unknown_counter,
        int,
        0,
        "switch_db_tunnel_route_disabled_ctx_unknown_counter_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        SWITCH_MODULE_ID,
        &unknown_counter,
        "switch_db[%d]->tunnel_route_disabled_ctx.unknown_counter[%d]",
        unit, unknown_counter_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_UNKNOWN_COUNTER_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_tunnel_route_disabled_ctx_unknown_counter_get(int unit, uint32 unknown_counter_idx_0, int *unknown_counter)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        unknown_counter_idx_0,
        4);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        SWITCH_MODULE_ID,
        unknown_counter);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    *unknown_counter = ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.unknown_counter[unknown_counter_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        SWITCH_MODULE_ID,
        &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.unknown_counter[unknown_counter_idx_0],
        "switch_db[%d]->tunnel_route_disabled_ctx.unknown_counter[%d]",
        unit, unknown_counter_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_UNKNOWN_COUNTER_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_tunnel_route_disabled_ctx_gre_keep_alive_counter_set(int unit, uint32 gre_keep_alive_counter_idx_0, int gre_keep_alive_counter)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        gre_keep_alive_counter_idx_0,
        4);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        SWITCH_MODULE_ID,
        ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.gre_keep_alive_counter[gre_keep_alive_counter_idx_0],
        gre_keep_alive_counter,
        int,
        0,
        "switch_db_tunnel_route_disabled_ctx_gre_keep_alive_counter_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        SWITCH_MODULE_ID,
        &gre_keep_alive_counter,
        "switch_db[%d]->tunnel_route_disabled_ctx.gre_keep_alive_counter[%d]",
        unit, gre_keep_alive_counter_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_GRE_KEEP_ALIVE_COUNTER_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_tunnel_route_disabled_ctx_gre_keep_alive_counter_get(int unit, uint32 gre_keep_alive_counter_idx_0, int *gre_keep_alive_counter)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        gre_keep_alive_counter_idx_0,
        4);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        SWITCH_MODULE_ID,
        gre_keep_alive_counter);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    *gre_keep_alive_counter = ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.gre_keep_alive_counter[gre_keep_alive_counter_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        SWITCH_MODULE_ID,
        &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.gre_keep_alive_counter[gre_keep_alive_counter_idx_0],
        "switch_db[%d]->tunnel_route_disabled_ctx.gre_keep_alive_counter[%d]",
        unit, gre_keep_alive_counter_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_GRE_KEEP_ALIVE_COUNTER_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_tunnel_route_disabled_ctx_icmp_ctx_set(int unit, uint32 icmp_ctx_idx_0, uint32 icmp_ctx_idx_1, int icmp_ctx)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        icmp_ctx_idx_0,
        4);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        icmp_ctx_idx_1,
        7);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        SWITCH_MODULE_ID,
        ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.icmp_ctx[icmp_ctx_idx_0][icmp_ctx_idx_1],
        icmp_ctx,
        int,
        0,
        "switch_db_tunnel_route_disabled_ctx_icmp_ctx_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        SWITCH_MODULE_ID,
        &icmp_ctx,
        "switch_db[%d]->tunnel_route_disabled_ctx.icmp_ctx[%d][%d]",
        unit, icmp_ctx_idx_0, icmp_ctx_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_ICMP_CTX_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_tunnel_route_disabled_ctx_icmp_ctx_get(int unit, uint32 icmp_ctx_idx_0, uint32 icmp_ctx_idx_1, int *icmp_ctx)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        icmp_ctx_idx_0,
        4);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        SWITCH_MODULE_ID,
        icmp_ctx);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        icmp_ctx_idx_1,
        7);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    *icmp_ctx = ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.icmp_ctx[icmp_ctx_idx_0][icmp_ctx_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        SWITCH_MODULE_ID,
        &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.icmp_ctx[icmp_ctx_idx_0][icmp_ctx_idx_1],
        "switch_db[%d]->tunnel_route_disabled_ctx.icmp_ctx[%d][%d]",
        unit, icmp_ctx_idx_0, icmp_ctx_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_ICMP_CTX_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_tunnel_route_disabled_ctx_igmp_ctx_set(int unit, uint32 igmp_ctx_idx_0, uint32 igmp_ctx_idx_1, int igmp_ctx)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        igmp_ctx_idx_0,
        4);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        igmp_ctx_idx_1,
        7);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        SWITCH_MODULE_ID,
        ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.igmp_ctx[igmp_ctx_idx_0][igmp_ctx_idx_1],
        igmp_ctx,
        int,
        0,
        "switch_db_tunnel_route_disabled_ctx_igmp_ctx_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        SWITCH_MODULE_ID,
        &igmp_ctx,
        "switch_db[%d]->tunnel_route_disabled_ctx.igmp_ctx[%d][%d]",
        unit, igmp_ctx_idx_0, igmp_ctx_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_IGMP_CTX_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_tunnel_route_disabled_ctx_igmp_ctx_get(int unit, uint32 igmp_ctx_idx_0, uint32 igmp_ctx_idx_1, int *igmp_ctx)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        igmp_ctx_idx_0,
        4);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        SWITCH_MODULE_ID,
        igmp_ctx);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        igmp_ctx_idx_1,
        7);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    *igmp_ctx = ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.igmp_ctx[igmp_ctx_idx_0][igmp_ctx_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        SWITCH_MODULE_ID,
        &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.igmp_ctx[igmp_ctx_idx_0][igmp_ctx_idx_1],
        "switch_db[%d]->tunnel_route_disabled_ctx.igmp_ctx[%d][%d]",
        unit, igmp_ctx_idx_0, igmp_ctx_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_IGMP_CTX_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_tunnel_route_disabled_ctx_udp_ctx_set(int unit, uint32 udp_ctx_idx_0, uint32 udp_ctx_idx_1, int udp_ctx)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        udp_ctx_idx_0,
        4);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        udp_ctx_idx_1,
        7);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        SWITCH_MODULE_ID,
        ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.udp_ctx[udp_ctx_idx_0][udp_ctx_idx_1],
        udp_ctx,
        int,
        0,
        "switch_db_tunnel_route_disabled_ctx_udp_ctx_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        SWITCH_MODULE_ID,
        &udp_ctx,
        "switch_db[%d]->tunnel_route_disabled_ctx.udp_ctx[%d][%d]",
        unit, udp_ctx_idx_0, udp_ctx_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_UDP_CTX_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_tunnel_route_disabled_ctx_udp_ctx_get(int unit, uint32 udp_ctx_idx_0, uint32 udp_ctx_idx_1, int *udp_ctx)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        udp_ctx_idx_0,
        4);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        SWITCH_MODULE_ID,
        udp_ctx);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        udp_ctx_idx_1,
        7);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    *udp_ctx = ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.udp_ctx[udp_ctx_idx_0][udp_ctx_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        SWITCH_MODULE_ID,
        &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.udp_ctx[udp_ctx_idx_0][udp_ctx_idx_1],
        "switch_db[%d]->tunnel_route_disabled_ctx.udp_ctx[%d][%d]",
        unit, udp_ctx_idx_0, udp_ctx_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_UDP_CTX_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_tunnel_route_disabled_ctx_tcp_ctx_set(int unit, uint32 tcp_ctx_idx_0, uint32 tcp_ctx_idx_1, int tcp_ctx)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        tcp_ctx_idx_0,
        4);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        tcp_ctx_idx_1,
        7);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        SWITCH_MODULE_ID,
        ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.tcp_ctx[tcp_ctx_idx_0][tcp_ctx_idx_1],
        tcp_ctx,
        int,
        0,
        "switch_db_tunnel_route_disabled_ctx_tcp_ctx_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        SWITCH_MODULE_ID,
        &tcp_ctx,
        "switch_db[%d]->tunnel_route_disabled_ctx.tcp_ctx[%d][%d]",
        unit, tcp_ctx_idx_0, tcp_ctx_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_TCP_CTX_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_tunnel_route_disabled_ctx_tcp_ctx_get(int unit, uint32 tcp_ctx_idx_0, uint32 tcp_ctx_idx_1, int *tcp_ctx)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        tcp_ctx_idx_0,
        4);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        SWITCH_MODULE_ID,
        tcp_ctx);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        tcp_ctx_idx_1,
        7);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    *tcp_ctx = ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.tcp_ctx[tcp_ctx_idx_0][tcp_ctx_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        SWITCH_MODULE_ID,
        &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.tcp_ctx[tcp_ctx_idx_0][tcp_ctx_idx_1],
        "switch_db[%d]->tunnel_route_disabled_ctx.tcp_ctx[%d][%d]",
        unit, tcp_ctx_idx_0, tcp_ctx_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_TCP_CTX_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_tunnel_route_disabled_ctx_unknown_ctx_set(int unit, uint32 unknown_ctx_idx_0, uint32 unknown_ctx_idx_1, int unknown_ctx)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        unknown_ctx_idx_0,
        4);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        unknown_ctx_idx_1,
        7);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        SWITCH_MODULE_ID,
        ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.unknown_ctx[unknown_ctx_idx_0][unknown_ctx_idx_1],
        unknown_ctx,
        int,
        0,
        "switch_db_tunnel_route_disabled_ctx_unknown_ctx_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        SWITCH_MODULE_ID,
        &unknown_ctx,
        "switch_db[%d]->tunnel_route_disabled_ctx.unknown_ctx[%d][%d]",
        unit, unknown_ctx_idx_0, unknown_ctx_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_UNKNOWN_CTX_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_tunnel_route_disabled_ctx_unknown_ctx_get(int unit, uint32 unknown_ctx_idx_0, uint32 unknown_ctx_idx_1, int *unknown_ctx)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        unknown_ctx_idx_0,
        4);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        SWITCH_MODULE_ID,
        unknown_ctx);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        unknown_ctx_idx_1,
        7);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    *unknown_ctx = ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.unknown_ctx[unknown_ctx_idx_0][unknown_ctx_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        SWITCH_MODULE_ID,
        &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.unknown_ctx[unknown_ctx_idx_0][unknown_ctx_idx_1],
        "switch_db[%d]->tunnel_route_disabled_ctx.unknown_ctx[%d][%d]",
        unit, unknown_ctx_idx_0, unknown_ctx_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_UNKNOWN_CTX_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_tunnel_route_disabled_ctx_gre_keep_alive_ctx_set(int unit, uint32 gre_keep_alive_ctx_idx_0, uint32 gre_keep_alive_ctx_idx_1, int gre_keep_alive_ctx)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        gre_keep_alive_ctx_idx_0,
        4);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        gre_keep_alive_ctx_idx_1,
        14);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        SWITCH_MODULE_ID,
        ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.gre_keep_alive_ctx[gre_keep_alive_ctx_idx_0][gre_keep_alive_ctx_idx_1],
        gre_keep_alive_ctx,
        int,
        0,
        "switch_db_tunnel_route_disabled_ctx_gre_keep_alive_ctx_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        SWITCH_MODULE_ID,
        &gre_keep_alive_ctx,
        "switch_db[%d]->tunnel_route_disabled_ctx.gre_keep_alive_ctx[%d][%d]",
        unit, gre_keep_alive_ctx_idx_0, gre_keep_alive_ctx_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_GRE_KEEP_ALIVE_CTX_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_tunnel_route_disabled_ctx_gre_keep_alive_ctx_get(int unit, uint32 gre_keep_alive_ctx_idx_0, uint32 gre_keep_alive_ctx_idx_1, int *gre_keep_alive_ctx)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        gre_keep_alive_ctx_idx_0,
        4);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        SWITCH_MODULE_ID,
        gre_keep_alive_ctx);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        gre_keep_alive_ctx_idx_1,
        14);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    *gre_keep_alive_ctx = ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.gre_keep_alive_ctx[gre_keep_alive_ctx_idx_0][gre_keep_alive_ctx_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        SWITCH_MODULE_ID,
        &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.gre_keep_alive_ctx[gre_keep_alive_ctx_idx_0][gre_keep_alive_ctx_idx_1],
        "switch_db[%d]->tunnel_route_disabled_ctx.gre_keep_alive_ctx[%d][%d]",
        unit, gre_keep_alive_ctx_idx_0, gre_keep_alive_ctx_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_TUNNEL_ROUTE_DISABLED_CTX_GRE_KEEP_ALIVE_CTX_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_tunnel_route_disabled_set(int unit, uint32 tunnel_route_disabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        SWITCH_MODULE_ID,
        ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled,
        tunnel_route_disabled,
        uint32,
        0,
        "switch_db_tunnel_route_disabled_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        SWITCH_MODULE_ID,
        &tunnel_route_disabled,
        "switch_db[%d]->tunnel_route_disabled",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_TUNNEL_ROUTE_DISABLED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_tunnel_route_disabled_get(int unit, uint32 *tunnel_route_disabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SWITCH_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SWITCH_MODULE_ID,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        SWITCH_MODULE_ID,
        tunnel_route_disabled);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SWITCH_MODULE_ID);

    *tunnel_route_disabled = ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        SWITCH_MODULE_ID,
        &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled,
        "switch_db[%d]->tunnel_route_disabled",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SWITCH_MODULE_ID,
        switch_db_info,
        SWITCH_DB_TUNNEL_ROUTE_DISABLED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}





switch_db_cbs switch_db = 	{
	
	switch_db_is_init,
	switch_db_init,
		{
		
		switch_db_module_verification_set,
		switch_db_module_verification_get}
	,
		{
		
		switch_db_module_error_recovery_set,
		switch_db_module_error_recovery_get}
	,
		{
		
		switch_db_l3mcastl2_ipv4_fwd_type_set,
		switch_db_l3mcastl2_ipv4_fwd_type_get}
	,
		{
		
		switch_db_l3mcastl2_ipv6_fwd_type_set,
		switch_db_l3mcastl2_ipv6_fwd_type_get}
	,
		{
		
		switch_db_header_enablers_hashing_set,
		switch_db_header_enablers_hashing_get}
	,
		{
		
			{
			
			switch_db_tunnel_route_disabled_ctx_icmp_counter_set,
			switch_db_tunnel_route_disabled_ctx_icmp_counter_get}
		,
			{
			
			switch_db_tunnel_route_disabled_ctx_igmp_counter_set,
			switch_db_tunnel_route_disabled_ctx_igmp_counter_get}
		,
			{
			
			switch_db_tunnel_route_disabled_ctx_udp_counter_set,
			switch_db_tunnel_route_disabled_ctx_udp_counter_get}
		,
			{
			
			switch_db_tunnel_route_disabled_ctx_tcp_counter_set,
			switch_db_tunnel_route_disabled_ctx_tcp_counter_get}
		,
			{
			
			switch_db_tunnel_route_disabled_ctx_unknown_counter_set,
			switch_db_tunnel_route_disabled_ctx_unknown_counter_get}
		,
			{
			
			switch_db_tunnel_route_disabled_ctx_gre_keep_alive_counter_set,
			switch_db_tunnel_route_disabled_ctx_gre_keep_alive_counter_get}
		,
			{
			
			switch_db_tunnel_route_disabled_ctx_icmp_ctx_set,
			switch_db_tunnel_route_disabled_ctx_icmp_ctx_get}
		,
			{
			
			switch_db_tunnel_route_disabled_ctx_igmp_ctx_set,
			switch_db_tunnel_route_disabled_ctx_igmp_ctx_get}
		,
			{
			
			switch_db_tunnel_route_disabled_ctx_udp_ctx_set,
			switch_db_tunnel_route_disabled_ctx_udp_ctx_get}
		,
			{
			
			switch_db_tunnel_route_disabled_ctx_tcp_ctx_set,
			switch_db_tunnel_route_disabled_ctx_tcp_ctx_get}
		,
			{
			
			switch_db_tunnel_route_disabled_ctx_unknown_ctx_set,
			switch_db_tunnel_route_disabled_ctx_unknown_ctx_get}
		,
			{
			
			switch_db_tunnel_route_disabled_ctx_gre_keep_alive_ctx_set,
			switch_db_tunnel_route_disabled_ctx_gre_keep_alive_ctx_get}
		}
	,
		{
		
		switch_db_tunnel_route_disabled_set,
		switch_db_tunnel_route_disabled_get}
	}
;
#undef BSL_LOG_MODULE
