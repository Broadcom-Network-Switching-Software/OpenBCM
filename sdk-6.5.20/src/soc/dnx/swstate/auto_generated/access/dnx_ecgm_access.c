
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
#include <soc/dnx/swstate/auto_generated/access/dnx_ecgm_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_ecgm_diagnostic.h>



dnx_ecgm_sw_state_t* dnx_ecgm_db_dummy = NULL;



int
dnx_ecgm_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ECGM_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_ECGM_MODULE_ID,
        ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]),
        "dnx_ecgm_db[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ecgm_db_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ECGM_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_ECGM_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_ecgm_sw_state_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_ecgm_db_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_ECGM_MODULE_ID,
        ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]),
        "dnx_ecgm_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        DNX_ECGM_MODULE_ID,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(dnx_ecgm_sw_state_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ecgm_db_interface_caching_port_set(int unit, bcm_port_t port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ECGM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ECGM_MODULE_ID,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ECGM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_ECGM_MODULE_ID,
        ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->interface_caching.port,
        port,
        bcm_port_t,
        0,
        "dnx_ecgm_db_interface_caching_port_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ECGM_MODULE_ID,
        &port,
        "dnx_ecgm_db[%d]->interface_caching.port",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ECGM_MODULE_ID,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_INTERFACE_CACHING_PORT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ecgm_db_interface_caching_port_get(int unit, bcm_port_t *port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ECGM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ECGM_MODULE_ID,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ECGM_MODULE_ID,
        port);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ECGM_MODULE_ID);

    *port = ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->interface_caching.port;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ECGM_MODULE_ID,
        &((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->interface_caching.port,
        "dnx_ecgm_db[%d]->interface_caching.port",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ECGM_MODULE_ID,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_INTERFACE_CACHING_PORT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ecgm_db_interface_caching_info_set(int unit, dnx_ecgm_interface_profile_info_t info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ECGM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ECGM_MODULE_ID,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ECGM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_ECGM_MODULE_ID,
        ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->interface_caching.info,
        info,
        dnx_ecgm_interface_profile_info_t,
        0,
        "dnx_ecgm_db_interface_caching_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ECGM_MODULE_ID,
        &info,
        "dnx_ecgm_db[%d]->interface_caching.info",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ECGM_MODULE_ID,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_INTERFACE_CACHING_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ecgm_db_interface_caching_info_get(int unit, dnx_ecgm_interface_profile_info_t *info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ECGM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ECGM_MODULE_ID,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ECGM_MODULE_ID,
        info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ECGM_MODULE_ID);

    *info = ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->interface_caching.info;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ECGM_MODULE_ID,
        &((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->interface_caching.info,
        "dnx_ecgm_db[%d]->interface_caching.info",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ECGM_MODULE_ID,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_INTERFACE_CACHING_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ecgm_db_interface_caching_profile_id_set(int unit, int profile_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ECGM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ECGM_MODULE_ID,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ECGM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_ECGM_MODULE_ID,
        ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->interface_caching.profile_id,
        profile_id,
        int,
        0,
        "dnx_ecgm_db_interface_caching_profile_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ECGM_MODULE_ID,
        &profile_id,
        "dnx_ecgm_db[%d]->interface_caching.profile_id",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ECGM_MODULE_ID,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_INTERFACE_CACHING_PROFILE_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ecgm_db_interface_caching_profile_id_get(int unit, int *profile_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ECGM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ECGM_MODULE_ID,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ECGM_MODULE_ID,
        profile_id);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ECGM_MODULE_ID);

    *profile_id = ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->interface_caching.profile_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ECGM_MODULE_ID,
        &((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->interface_caching.profile_id,
        "dnx_ecgm_db[%d]->interface_caching.profile_id",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ECGM_MODULE_ID,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_INTERFACE_CACHING_PROFILE_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ecgm_db_interface_caching_valid_set(int unit, int valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ECGM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ECGM_MODULE_ID,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ECGM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_ECGM_MODULE_ID,
        ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->interface_caching.valid,
        valid,
        int,
        0,
        "dnx_ecgm_db_interface_caching_valid_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ECGM_MODULE_ID,
        &valid,
        "dnx_ecgm_db[%d]->interface_caching.valid",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ECGM_MODULE_ID,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_INTERFACE_CACHING_VALID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ecgm_db_interface_caching_valid_get(int unit, int *valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ECGM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ECGM_MODULE_ID,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ECGM_MODULE_ID,
        valid);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ECGM_MODULE_ID);

    *valid = ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->interface_caching.valid;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ECGM_MODULE_ID,
        &((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->interface_caching.valid,
        "dnx_ecgm_db[%d]->interface_caching.valid",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ECGM_MODULE_ID,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_INTERFACE_CACHING_VALID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ecgm_db_port_caching_port_set(int unit, bcm_port_t port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ECGM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ECGM_MODULE_ID,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ECGM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_ECGM_MODULE_ID,
        ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_caching.port,
        port,
        bcm_port_t,
        0,
        "dnx_ecgm_db_port_caching_port_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ECGM_MODULE_ID,
        &port,
        "dnx_ecgm_db[%d]->port_caching.port",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ECGM_MODULE_ID,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_PORT_CACHING_PORT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ecgm_db_port_caching_port_get(int unit, bcm_port_t *port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ECGM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ECGM_MODULE_ID,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ECGM_MODULE_ID,
        port);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ECGM_MODULE_ID);

    *port = ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_caching.port;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ECGM_MODULE_ID,
        &((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_caching.port,
        "dnx_ecgm_db[%d]->port_caching.port",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ECGM_MODULE_ID,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_PORT_CACHING_PORT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ecgm_db_port_caching_info_set(int unit, CONST dnx_ecgm_port_profile_info_t *info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ECGM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ECGM_MODULE_ID,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ECGM_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_ECGM_MODULE_ID,
        ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_caching.info,
        info,
        dnx_ecgm_port_profile_info_t,
        0,
        "dnx_ecgm_db_port_caching_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ECGM_MODULE_ID,
        info,
        "dnx_ecgm_db[%d]->port_caching.info",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ECGM_MODULE_ID,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_PORT_CACHING_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ecgm_db_port_caching_info_get(int unit, CONST dnx_ecgm_port_profile_info_t **info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ECGM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ECGM_MODULE_ID,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ECGM_MODULE_ID,
        info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ECGM_MODULE_ID);

    *info = &((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_caching.info;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ECGM_MODULE_ID,
        &((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_caching.info,
        "dnx_ecgm_db[%d]->port_caching.info",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ECGM_MODULE_ID,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_PORT_CACHING_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ecgm_db_port_caching_profile_id_set(int unit, int profile_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ECGM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ECGM_MODULE_ID,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ECGM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_ECGM_MODULE_ID,
        ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_caching.profile_id,
        profile_id,
        int,
        0,
        "dnx_ecgm_db_port_caching_profile_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ECGM_MODULE_ID,
        &profile_id,
        "dnx_ecgm_db[%d]->port_caching.profile_id",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ECGM_MODULE_ID,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_PORT_CACHING_PROFILE_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ecgm_db_port_caching_profile_id_get(int unit, int *profile_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ECGM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ECGM_MODULE_ID,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ECGM_MODULE_ID,
        profile_id);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ECGM_MODULE_ID);

    *profile_id = ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_caching.profile_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ECGM_MODULE_ID,
        &((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_caching.profile_id,
        "dnx_ecgm_db[%d]->port_caching.profile_id",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ECGM_MODULE_ID,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_PORT_CACHING_PROFILE_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ecgm_db_port_caching_valid_set(int unit, int valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ECGM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ECGM_MODULE_ID,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ECGM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_ECGM_MODULE_ID,
        ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_caching.valid,
        valid,
        int,
        0,
        "dnx_ecgm_db_port_caching_valid_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ECGM_MODULE_ID,
        &valid,
        "dnx_ecgm_db[%d]->port_caching.valid",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ECGM_MODULE_ID,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_PORT_CACHING_VALID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ecgm_db_port_caching_valid_get(int unit, int *valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ECGM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ECGM_MODULE_ID,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ECGM_MODULE_ID,
        valid);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ECGM_MODULE_ID);

    *valid = ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_caching.valid;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ECGM_MODULE_ID,
        &((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_caching.valid,
        "dnx_ecgm_db[%d]->port_caching.valid",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ECGM_MODULE_ID,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_PORT_CACHING_VALID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_ecgm_db_cbs dnx_ecgm_db = 	{
	
	dnx_ecgm_db_is_init,
	dnx_ecgm_db_init,
		{
		
			{
			
			dnx_ecgm_db_interface_caching_port_set,
			dnx_ecgm_db_interface_caching_port_get}
		,
			{
			
			dnx_ecgm_db_interface_caching_info_set,
			dnx_ecgm_db_interface_caching_info_get}
		,
			{
			
			dnx_ecgm_db_interface_caching_profile_id_set,
			dnx_ecgm_db_interface_caching_profile_id_get}
		,
			{
			
			dnx_ecgm_db_interface_caching_valid_set,
			dnx_ecgm_db_interface_caching_valid_get}
		}
	,
		{
		
			{
			
			dnx_ecgm_db_port_caching_port_set,
			dnx_ecgm_db_port_caching_port_get}
		,
			{
			
			dnx_ecgm_db_port_caching_info_set,
			dnx_ecgm_db_port_caching_info_get}
		,
			{
			
			dnx_ecgm_db_port_caching_profile_id_set,
			dnx_ecgm_db_port_caching_profile_id_get}
		,
			{
			
			dnx_ecgm_db_port_caching_valid_set,
			dnx_ecgm_db_port_caching_valid_get}
		}
	}
;
#undef BSL_LOG_MODULE
