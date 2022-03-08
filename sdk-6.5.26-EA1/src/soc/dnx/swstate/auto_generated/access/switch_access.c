
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/switch_access.h>
#include <bcm/switch.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/switch_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/switch_layout.h>





int
switch_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_SWITCH_DB,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_SW_STATE_SWITCH_DB,
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
        DNX_SW_STATE_SWITCH_DB,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_SW_STATE_SWITCH_DB,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        switch_sw_state_t,
        DNX_SW_STATE_SWITCH_DB_NOF_PARAMS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "switch_db_init",
        DNX_SW_STATE_DIAG_ALLOC,
        switch_init_layout_structure);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_SW_STATE_SWITCH_DB,
        ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID]),
        "switch_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_SWITCH_DB,
        switch_db_info,
        SWITCH_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID]));

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
        DNX_SW_STATE_SWITCH_DB__MODULE_VERIFICATION,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_SWITCH_DB__MODULE_VERIFICATION,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        module_verification_idx_0,
        bcmModuleCount);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_SWITCH_DB__MODULE_VERIFICATION,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_SWITCH_DB__MODULE_VERIFICATION,
        ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->module_verification[module_verification_idx_0],
        module_verification,
        uint8,
        0,
        "switch_db_module_verification_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_SWITCH_DB__MODULE_VERIFICATION,
        &module_verification,
        "switch_db[%d]->module_verification[%d]",
        unit, module_verification_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_SWITCH_DB__MODULE_VERIFICATION,
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
        DNX_SW_STATE_SWITCH_DB__MODULE_VERIFICATION,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_SWITCH_DB__MODULE_VERIFICATION,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        module_verification_idx_0,
        bcmModuleCount);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_SWITCH_DB__MODULE_VERIFICATION,
        module_verification);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_SWITCH_DB__MODULE_VERIFICATION,
        DNX_SW_STATE_DIAG_READ);

    *module_verification = ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->module_verification[module_verification_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_SWITCH_DB__MODULE_VERIFICATION,
        &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->module_verification[module_verification_idx_0],
        "switch_db[%d]->module_verification[%d]",
        unit, module_verification_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_SWITCH_DB__MODULE_VERIFICATION,
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
        DNX_SW_STATE_SWITCH_DB__MODULE_ERROR_RECOVERY,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_SWITCH_DB__MODULE_ERROR_RECOVERY,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        module_error_recovery_idx_0,
        bcmModuleCount);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_SWITCH_DB__MODULE_ERROR_RECOVERY,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_SWITCH_DB__MODULE_ERROR_RECOVERY,
        ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->module_error_recovery[module_error_recovery_idx_0],
        module_error_recovery,
        uint8,
        0,
        "switch_db_module_error_recovery_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_SWITCH_DB__MODULE_ERROR_RECOVERY,
        &module_error_recovery,
        "switch_db[%d]->module_error_recovery[%d]",
        unit, module_error_recovery_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_SWITCH_DB__MODULE_ERROR_RECOVERY,
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
        DNX_SW_STATE_SWITCH_DB__MODULE_ERROR_RECOVERY,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_SWITCH_DB__MODULE_ERROR_RECOVERY,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        module_error_recovery_idx_0,
        bcmModuleCount);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_SWITCH_DB__MODULE_ERROR_RECOVERY,
        module_error_recovery);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_SWITCH_DB__MODULE_ERROR_RECOVERY,
        DNX_SW_STATE_DIAG_READ);

    *module_error_recovery = ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->module_error_recovery[module_error_recovery_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_SWITCH_DB__MODULE_ERROR_RECOVERY,
        &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->module_error_recovery[module_error_recovery_idx_0],
        "switch_db[%d]->module_error_recovery[%d]",
        unit, module_error_recovery_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_SWITCH_DB__MODULE_ERROR_RECOVERY,
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
        DNX_SW_STATE_SWITCH_DB__L3MCASTL2_IPV4_FWD_TYPE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_SWITCH_DB__L3MCASTL2_IPV4_FWD_TYPE,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_SWITCH_DB__L3MCASTL2_IPV4_FWD_TYPE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_SWITCH_DB__L3MCASTL2_IPV4_FWD_TYPE,
        ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->l3mcastl2_ipv4_fwd_type,
        l3mcastl2_ipv4_fwd_type,
        uint8,
        0,
        "switch_db_l3mcastl2_ipv4_fwd_type_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_SWITCH_DB__L3MCASTL2_IPV4_FWD_TYPE,
        &l3mcastl2_ipv4_fwd_type,
        "switch_db[%d]->l3mcastl2_ipv4_fwd_type",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_SWITCH_DB__L3MCASTL2_IPV4_FWD_TYPE,
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
        DNX_SW_STATE_SWITCH_DB__L3MCASTL2_IPV4_FWD_TYPE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_SWITCH_DB__L3MCASTL2_IPV4_FWD_TYPE,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_SWITCH_DB__L3MCASTL2_IPV4_FWD_TYPE,
        l3mcastl2_ipv4_fwd_type);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_SWITCH_DB__L3MCASTL2_IPV4_FWD_TYPE,
        DNX_SW_STATE_DIAG_READ);

    *l3mcastl2_ipv4_fwd_type = ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->l3mcastl2_ipv4_fwd_type;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_SWITCH_DB__L3MCASTL2_IPV4_FWD_TYPE,
        &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->l3mcastl2_ipv4_fwd_type,
        "switch_db[%d]->l3mcastl2_ipv4_fwd_type",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_SWITCH_DB__L3MCASTL2_IPV4_FWD_TYPE,
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
        DNX_SW_STATE_SWITCH_DB__L3MCASTL2_IPV6_FWD_TYPE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_SWITCH_DB__L3MCASTL2_IPV6_FWD_TYPE,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_SWITCH_DB__L3MCASTL2_IPV6_FWD_TYPE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_SWITCH_DB__L3MCASTL2_IPV6_FWD_TYPE,
        ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->l3mcastl2_ipv6_fwd_type,
        l3mcastl2_ipv6_fwd_type,
        uint8,
        0,
        "switch_db_l3mcastl2_ipv6_fwd_type_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_SWITCH_DB__L3MCASTL2_IPV6_FWD_TYPE,
        &l3mcastl2_ipv6_fwd_type,
        "switch_db[%d]->l3mcastl2_ipv6_fwd_type",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_SWITCH_DB__L3MCASTL2_IPV6_FWD_TYPE,
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
        DNX_SW_STATE_SWITCH_DB__L3MCASTL2_IPV6_FWD_TYPE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_SWITCH_DB__L3MCASTL2_IPV6_FWD_TYPE,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_SWITCH_DB__L3MCASTL2_IPV6_FWD_TYPE,
        l3mcastl2_ipv6_fwd_type);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_SWITCH_DB__L3MCASTL2_IPV6_FWD_TYPE,
        DNX_SW_STATE_DIAG_READ);

    *l3mcastl2_ipv6_fwd_type = ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->l3mcastl2_ipv6_fwd_type;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_SWITCH_DB__L3MCASTL2_IPV6_FWD_TYPE,
        &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->l3mcastl2_ipv6_fwd_type,
        "switch_db[%d]->l3mcastl2_ipv6_fwd_type",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_SWITCH_DB__L3MCASTL2_IPV6_FWD_TYPE,
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
        DNX_SW_STATE_SWITCH_DB__HEADER_ENABLERS_HASHING,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_SWITCH_DB__HEADER_ENABLERS_HASHING,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        header_enablers_hashing_idx_0,
        DBAL_NOF_ENUM_HASH_FIELD_ENABLERS_HEADER_VALUES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_SWITCH_DB__HEADER_ENABLERS_HASHING,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_SWITCH_DB__HEADER_ENABLERS_HASHING,
        ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->header_enablers_hashing[header_enablers_hashing_idx_0],
        header_enablers_hashing,
        uint32,
        0,
        "switch_db_header_enablers_hashing_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_SWITCH_DB__HEADER_ENABLERS_HASHING,
        &header_enablers_hashing,
        "switch_db[%d]->header_enablers_hashing[%d]",
        unit, header_enablers_hashing_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_SWITCH_DB__HEADER_ENABLERS_HASHING,
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
        DNX_SW_STATE_SWITCH_DB__HEADER_ENABLERS_HASHING,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_SWITCH_DB__HEADER_ENABLERS_HASHING,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        header_enablers_hashing_idx_0,
        DBAL_NOF_ENUM_HASH_FIELD_ENABLERS_HEADER_VALUES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_SWITCH_DB__HEADER_ENABLERS_HASHING,
        header_enablers_hashing);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_SWITCH_DB__HEADER_ENABLERS_HASHING,
        DNX_SW_STATE_DIAG_READ);

    *header_enablers_hashing = ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->header_enablers_hashing[header_enablers_hashing_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_SWITCH_DB__HEADER_ENABLERS_HASHING,
        &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->header_enablers_hashing[header_enablers_hashing_idx_0],
        "switch_db[%d]->header_enablers_hashing[%d]",
        unit, header_enablers_hashing_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_SWITCH_DB__HEADER_ENABLERS_HASHING,
        switch_db_info,
        SWITCH_DB_HEADER_ENABLERS_HASHING_INFO,
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
        DNX_SW_STATE_SWITCH_DB__TUNNEL_ROUTE_DISABLED,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_SWITCH_DB__TUNNEL_ROUTE_DISABLED,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_SWITCH_DB__TUNNEL_ROUTE_DISABLED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_SWITCH_DB__TUNNEL_ROUTE_DISABLED,
        ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled,
        tunnel_route_disabled,
        uint32,
        0,
        "switch_db_tunnel_route_disabled_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_SWITCH_DB__TUNNEL_ROUTE_DISABLED,
        &tunnel_route_disabled,
        "switch_db[%d]->tunnel_route_disabled",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_SWITCH_DB__TUNNEL_ROUTE_DISABLED,
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
        DNX_SW_STATE_SWITCH_DB__TUNNEL_ROUTE_DISABLED,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_SWITCH_DB__TUNNEL_ROUTE_DISABLED,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_SWITCH_DB__TUNNEL_ROUTE_DISABLED,
        tunnel_route_disabled);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_SWITCH_DB__TUNNEL_ROUTE_DISABLED,
        DNX_SW_STATE_DIAG_READ);

    *tunnel_route_disabled = ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_SWITCH_DB__TUNNEL_ROUTE_DISABLED,
        &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled,
        "switch_db[%d]->tunnel_route_disabled",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_SWITCH_DB__TUNNEL_ROUTE_DISABLED,
        switch_db_info,
        SWITCH_DB_TUNNEL_ROUTE_DISABLED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_per_port_special_label_termination_disabled_set(int unit, uint32 per_port_special_label_termination_disabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_SWITCH_DB__PER_PORT_SPECIAL_LABEL_TERMINATION_DISABLED,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_SWITCH_DB__PER_PORT_SPECIAL_LABEL_TERMINATION_DISABLED,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_SWITCH_DB__PER_PORT_SPECIAL_LABEL_TERMINATION_DISABLED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_SWITCH_DB__PER_PORT_SPECIAL_LABEL_TERMINATION_DISABLED,
        ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->per_port_special_label_termination_disabled,
        per_port_special_label_termination_disabled,
        uint32,
        0,
        "switch_db_per_port_special_label_termination_disabled_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_SWITCH_DB__PER_PORT_SPECIAL_LABEL_TERMINATION_DISABLED,
        &per_port_special_label_termination_disabled,
        "switch_db[%d]->per_port_special_label_termination_disabled",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_SWITCH_DB__PER_PORT_SPECIAL_LABEL_TERMINATION_DISABLED,
        switch_db_info,
        SWITCH_DB_PER_PORT_SPECIAL_LABEL_TERMINATION_DISABLED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_per_port_special_label_termination_disabled_get(int unit, uint32 *per_port_special_label_termination_disabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_SWITCH_DB__PER_PORT_SPECIAL_LABEL_TERMINATION_DISABLED,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_SWITCH_DB__PER_PORT_SPECIAL_LABEL_TERMINATION_DISABLED,
        sw_state_roots_array[unit][SWITCH_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_SWITCH_DB__PER_PORT_SPECIAL_LABEL_TERMINATION_DISABLED,
        per_port_special_label_termination_disabled);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_SWITCH_DB__PER_PORT_SPECIAL_LABEL_TERMINATION_DISABLED,
        DNX_SW_STATE_DIAG_READ);

    *per_port_special_label_termination_disabled = ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->per_port_special_label_termination_disabled;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_SWITCH_DB__PER_PORT_SPECIAL_LABEL_TERMINATION_DISABLED,
        &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->per_port_special_label_termination_disabled,
        "switch_db[%d]->per_port_special_label_termination_disabled",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_SWITCH_DB__PER_PORT_SPECIAL_LABEL_TERMINATION_DISABLED,
        switch_db_info,
        SWITCH_DB_PER_PORT_SPECIAL_LABEL_TERMINATION_DISABLED_INFO,
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
		
		switch_db_tunnel_route_disabled_set,
		switch_db_tunnel_route_disabled_get}
	,
		{
		
		switch_db_per_port_special_label_termination_disabled_set,
		switch_db_per_port_special_label_termination_disabled_get}
	}
;
#undef BSL_LOG_MODULE
