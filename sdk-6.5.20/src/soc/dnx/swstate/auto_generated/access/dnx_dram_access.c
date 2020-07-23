
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
#include <soc/dnx/swstate/auto_generated/access/dnx_dram_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_dram_diagnostic.h>



dnx_dram_db_t* dnx_dram_db_dummy = NULL;



int
dnx_dram_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]),
        "dnx_dram_db[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_DRAM_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_dram_db_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_dram_db_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]),
        "dnx_dram_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(dnx_dram_db_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_is_initialized_set(int unit, int is_initialized)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->is_initialized,
        is_initialized,
        int,
        0,
        "dnx_dram_db_is_initialized_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_DRAM_MODULE_ID,
        &is_initialized,
        "dnx_dram_db[%d]->is_initialized",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_IS_INITIALIZED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_is_initialized_get(int unit, int *is_initialized)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_DRAM_MODULE_ID,
        is_initialized);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    *is_initialized = ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->is_initialized;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->is_initialized,
        "dnx_dram_db[%d]->is_initialized",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_IS_INITIALIZED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_sync_manager_deleted_buffers_file_mutex_create(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_MUTEX_CREATE(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->sync_manager.deleted_buffers_file_mutex,
        "((dnx_dram_db_t*)sw_state_roots_array).sync_manager.deleted_buffers_file_mutex");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MUTEX_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXCREATE,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->sync_manager.deleted_buffers_file_mutex,
        "dnx_dram_db[%d]->sync_manager.deleted_buffers_file_mutex",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_SYNC_MANAGER_DELETED_BUFFERS_FILE_MUTEX_INFO,
        DNX_SW_STATE_DIAG_MUTEX,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_sync_manager_deleted_buffers_file_mutex_is_created(int unit, uint8 *is_created)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_IS_CREATED,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_MUTEX_IS_CREATED(
        unit,
        DNX_DRAM_MODULE_ID,
        is_created,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->sync_manager.deleted_buffers_file_mutex,
        "((dnx_dram_db_t*)sw_state_roots_array).sync_manager.deleted_buffers_file_mutex");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MUTEX_IS_CREATED_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXCREATE,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->sync_manager.deleted_buffers_file_mutex,
        "dnx_dram_db[%d]->sync_manager.deleted_buffers_file_mutex",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_SYNC_MANAGER_DELETED_BUFFERS_FILE_MUTEX_INFO,
        DNX_SW_STATE_DIAG_MUTEX,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_sync_manager_deleted_buffers_file_mutex_take(int unit, int usec)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_TAKE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_MUTEX_TAKE(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->sync_manager.deleted_buffers_file_mutex,
        usec);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MUTEX_TAKE_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXTAKE,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->sync_manager.deleted_buffers_file_mutex,
        "dnx_dram_db[%d]->sync_manager.deleted_buffers_file_mutex",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_SYNC_MANAGER_DELETED_BUFFERS_FILE_MUTEX_INFO,
        DNX_SW_STATE_DIAG_MUTEX,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_sync_manager_deleted_buffers_file_mutex_give(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_GIVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_MUTEX_GIVE(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->sync_manager.deleted_buffers_file_mutex);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MUTEX_GIVE_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXGIVE,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->sync_manager.deleted_buffers_file_mutex,
        "dnx_dram_db[%d]->sync_manager.deleted_buffers_file_mutex",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_SYNC_MANAGER_DELETED_BUFFERS_FILE_MUTEX_INFO,
        DNX_SW_STATE_DIAG_MUTEX,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_power_down_callback_callback_set(int unit, bcm_switch_dram_power_down_callback_t callback)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->power_down_callback.callback,
        callback,
        bcm_switch_dram_power_down_callback_t,
        0,
        "dnx_dram_db_power_down_callback_callback_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_DRAM_MODULE_ID,
        &callback,
        "dnx_dram_db[%d]->power_down_callback.callback",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_POWER_DOWN_CALLBACK_CALLBACK_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_power_down_callback_callback_get(int unit, bcm_switch_dram_power_down_callback_t *callback)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_DRAM_MODULE_ID,
        callback);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    *callback = ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->power_down_callback.callback;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->power_down_callback.callback,
        "dnx_dram_db[%d]->power_down_callback.callback",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_POWER_DOWN_CALLBACK_CALLBACK_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_power_down_callback_userdata_set(int unit, dnx_power_down_callback_userdata_t userdata)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->power_down_callback.userdata,
        userdata,
        dnx_power_down_callback_userdata_t,
        0,
        "dnx_dram_db_power_down_callback_userdata_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_DRAM_MODULE_ID,
        &userdata,
        "dnx_dram_db[%d]->power_down_callback.userdata",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_POWER_DOWN_CALLBACK_USERDATA_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_power_down_callback_userdata_get(int unit, dnx_power_down_callback_userdata_t *userdata)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_DRAM_MODULE_ID,
        userdata);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    *userdata = ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->power_down_callback.userdata;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->power_down_callback.userdata,
        "dnx_dram_db[%d]->power_down_callback.userdata",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_POWER_DOWN_CALLBACK_USERDATA_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_power_down_callback_power_on_set(int unit, uint32 power_on)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->power_down_callback.power_on,
        power_on,
        uint32,
        0,
        "dnx_dram_db_power_down_callback_power_on_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_DRAM_MODULE_ID,
        &power_on,
        "dnx_dram_db[%d]->power_down_callback.power_on",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_POWER_DOWN_CALLBACK_POWER_ON_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_power_down_callback_power_on_get(int unit, uint32 *power_on)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_DRAM_MODULE_ID,
        power_on);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    *power_on = ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->power_down_callback.power_on;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->power_down_callback.power_on,
        "dnx_dram_db[%d]->power_down_callback.power_on",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_POWER_DOWN_CALLBACK_POWER_ON_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_override_bist_configurations_during_tuning_set(int unit, int override_bist_configurations_during_tuning)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->override_bist_configurations_during_tuning,
        override_bist_configurations_during_tuning,
        int,
        0,
        "dnx_dram_db_override_bist_configurations_during_tuning_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_DRAM_MODULE_ID,
        &override_bist_configurations_during_tuning,
        "dnx_dram_db[%d]->override_bist_configurations_during_tuning",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_OVERRIDE_BIST_CONFIGURATIONS_DURING_TUNING_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_override_bist_configurations_during_tuning_get(int unit, int *override_bist_configurations_during_tuning)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_DRAM_MODULE_ID,
        override_bist_configurations_during_tuning);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    *override_bist_configurations_during_tuning = ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->override_bist_configurations_during_tuning;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->override_bist_configurations_during_tuning,
        "dnx_dram_db[%d]->override_bist_configurations_during_tuning",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_OVERRIDE_BIST_CONFIGURATIONS_DURING_TUNING_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_channel_soft_init_after_bist_set(int unit, int channel_soft_init_after_bist)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->channel_soft_init_after_bist,
        channel_soft_init_after_bist,
        int,
        0,
        "dnx_dram_db_channel_soft_init_after_bist_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_DRAM_MODULE_ID,
        &channel_soft_init_after_bist,
        "dnx_dram_db[%d]->channel_soft_init_after_bist",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_CHANNEL_SOFT_INIT_AFTER_BIST_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_channel_soft_init_after_bist_get(int unit, int *channel_soft_init_after_bist)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_DRAM_MODULE_ID,
        channel_soft_init_after_bist);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    *channel_soft_init_after_bist = ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->channel_soft_init_after_bist;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->channel_soft_init_after_bist,
        "dnx_dram_db[%d]->channel_soft_init_after_bist",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_CHANNEL_SOFT_INIT_AFTER_BIST_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_traffic_tuning_core_mode_set(int unit, int traffic_tuning_core_mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->traffic_tuning_core_mode,
        traffic_tuning_core_mode,
        int,
        0,
        "dnx_dram_db_traffic_tuning_core_mode_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_DRAM_MODULE_ID,
        &traffic_tuning_core_mode,
        "dnx_dram_db[%d]->traffic_tuning_core_mode",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_TRAFFIC_TUNING_CORE_MODE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_traffic_tuning_core_mode_get(int unit, int *traffic_tuning_core_mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_DRAM_MODULE_ID,
        traffic_tuning_core_mode);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    *traffic_tuning_core_mode = ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->traffic_tuning_core_mode;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->traffic_tuning_core_mode,
        "dnx_dram_db[%d]->traffic_tuning_core_mode",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_TRAFFIC_TUNING_CORE_MODE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_dynamic_calibration_enabled_set(int unit, int dynamic_calibration_enabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->dynamic_calibration_enabled,
        dynamic_calibration_enabled,
        int,
        0,
        "dnx_dram_db_dynamic_calibration_enabled_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_DRAM_MODULE_ID,
        &dynamic_calibration_enabled,
        "dnx_dram_db[%d]->dynamic_calibration_enabled",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_DYNAMIC_CALIBRATION_ENABLED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_dynamic_calibration_enabled_get(int unit, int *dynamic_calibration_enabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_DRAM_MODULE_ID,
        dynamic_calibration_enabled);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    *dynamic_calibration_enabled = ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->dynamic_calibration_enabled;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->dynamic_calibration_enabled,
        "dnx_dram_db[%d]->dynamic_calibration_enabled",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_DYNAMIC_CALIBRATION_ENABLED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_dynamic_calibration_was_enabled_after_dram_tune_set(int unit, uint32 dynamic_calibration_was_enabled_after_dram_tune_idx_0, int dynamic_calibration_was_enabled_after_dram_tune)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        dynamic_calibration_was_enabled_after_dram_tune_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->dynamic_calibration_was_enabled_after_dram_tune[dynamic_calibration_was_enabled_after_dram_tune_idx_0],
        dynamic_calibration_was_enabled_after_dram_tune,
        int,
        0,
        "dnx_dram_db_dynamic_calibration_was_enabled_after_dram_tune_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_DRAM_MODULE_ID,
        &dynamic_calibration_was_enabled_after_dram_tune,
        "dnx_dram_db[%d]->dynamic_calibration_was_enabled_after_dram_tune[%d]",
        unit, dynamic_calibration_was_enabled_after_dram_tune_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_DYNAMIC_CALIBRATION_WAS_ENABLED_AFTER_DRAM_TUNE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_dynamic_calibration_was_enabled_after_dram_tune_get(int unit, uint32 dynamic_calibration_was_enabled_after_dram_tune_idx_0, int *dynamic_calibration_was_enabled_after_dram_tune)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        dynamic_calibration_was_enabled_after_dram_tune_idx_0,
        2);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_DRAM_MODULE_ID,
        dynamic_calibration_was_enabled_after_dram_tune);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    *dynamic_calibration_was_enabled_after_dram_tune = ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->dynamic_calibration_was_enabled_after_dram_tune[dynamic_calibration_was_enabled_after_dram_tune_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->dynamic_calibration_was_enabled_after_dram_tune[dynamic_calibration_was_enabled_after_dram_tune_idx_0],
        "dnx_dram_db[%d]->dynamic_calibration_was_enabled_after_dram_tune[%d]",
        unit, dynamic_calibration_was_enabled_after_dram_tune_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_DYNAMIC_CALIBRATION_WAS_ENABLED_AFTER_DRAM_TUNE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_temperature_monitoring_thread_state_set(int unit, dnx_dram_temperature_monitoring_state_e temperature_monitoring_thread_state)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->temperature_monitoring_thread_state,
        temperature_monitoring_thread_state,
        dnx_dram_temperature_monitoring_state_e,
        0,
        "dnx_dram_db_temperature_monitoring_thread_state_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_DRAM_MODULE_ID,
        &temperature_monitoring_thread_state,
        "dnx_dram_db[%d]->temperature_monitoring_thread_state",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_TEMPERATURE_MONITORING_THREAD_STATE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_temperature_monitoring_thread_state_get(int unit, dnx_dram_temperature_monitoring_state_e *temperature_monitoring_thread_state)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_DRAM_MODULE_ID,
        temperature_monitoring_thread_state);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    *temperature_monitoring_thread_state = ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->temperature_monitoring_thread_state;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->temperature_monitoring_thread_state,
        "dnx_dram_db[%d]->temperature_monitoring_thread_state",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_TEMPERATURE_MONITORING_THREAD_STATE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_temperature_monitoring_thread_handler_set(int unit, periodic_event_handler_t temperature_monitoring_thread_handler)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_WB);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->temperature_monitoring_thread_handler,
        temperature_monitoring_thread_handler,
        periodic_event_handler_t,
        0,
        "dnx_dram_db_temperature_monitoring_thread_handler_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_DRAM_MODULE_ID,
        &temperature_monitoring_thread_handler,
        "dnx_dram_db[%d]->temperature_monitoring_thread_handler",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_TEMPERATURE_MONITORING_THREAD_HANDLER_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_temperature_monitoring_thread_handler_get(int unit, periodic_event_handler_t *temperature_monitoring_thread_handler)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_WB);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_DRAM_MODULE_ID,
        temperature_monitoring_thread_handler);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    *temperature_monitoring_thread_handler = ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->temperature_monitoring_thread_handler;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->temperature_monitoring_thread_handler,
        "dnx_dram_db[%d]->temperature_monitoring_thread_handler",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_TEMPERATURE_MONITORING_THREAD_HANDLER_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_is_init_set(int unit, uint32 vendor_info_idx_0, uint32 is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vendor_info_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].is_init,
        is_init,
        uint32,
        0,
        "dnx_dram_db_vendor_info_is_init_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_DRAM_MODULE_ID,
        &is_init,
        "dnx_dram_db[%d]->vendor_info[%d].is_init",
        unit, vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_VENDOR_INFO_IS_INIT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_is_init_get(int unit, uint32 vendor_info_idx_0, uint32 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vendor_info_idx_0,
        2);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_DRAM_MODULE_ID,
        is_init);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    *is_init = ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].is_init;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].is_init,
        "dnx_dram_db[%d]->vendor_info[%d].is_init",
        unit, vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_VENDOR_INFO_IS_INIT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_gen2_test_set(int unit, uint32 vendor_info_idx_0, uint32 gen2_test)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vendor_info_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].gen2_test,
        gen2_test,
        uint32,
        0,
        "dnx_dram_db_vendor_info_gen2_test_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_DRAM_MODULE_ID,
        &gen2_test,
        "dnx_dram_db[%d]->vendor_info[%d].gen2_test",
        unit, vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_VENDOR_INFO_GEN2_TEST_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_gen2_test_get(int unit, uint32 vendor_info_idx_0, uint32 *gen2_test)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vendor_info_idx_0,
        2);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_DRAM_MODULE_ID,
        gen2_test);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    *gen2_test = ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].gen2_test;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].gen2_test,
        "dnx_dram_db[%d]->vendor_info[%d].gen2_test",
        unit, vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_VENDOR_INFO_GEN2_TEST_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_ecc_set(int unit, uint32 vendor_info_idx_0, uint32 ecc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vendor_info_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].ecc,
        ecc,
        uint32,
        0,
        "dnx_dram_db_vendor_info_ecc_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_DRAM_MODULE_ID,
        &ecc,
        "dnx_dram_db[%d]->vendor_info[%d].ecc",
        unit, vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_VENDOR_INFO_ECC_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_ecc_get(int unit, uint32 vendor_info_idx_0, uint32 *ecc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vendor_info_idx_0,
        2);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_DRAM_MODULE_ID,
        ecc);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    *ecc = ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].ecc;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].ecc,
        "dnx_dram_db[%d]->vendor_info[%d].ecc",
        unit, vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_VENDOR_INFO_ECC_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_density_set(int unit, uint32 vendor_info_idx_0, uint32 density)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vendor_info_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].density,
        density,
        uint32,
        0,
        "dnx_dram_db_vendor_info_density_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_DRAM_MODULE_ID,
        &density,
        "dnx_dram_db[%d]->vendor_info[%d].density",
        unit, vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_VENDOR_INFO_DENSITY_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_density_get(int unit, uint32 vendor_info_idx_0, uint32 *density)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vendor_info_idx_0,
        2);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_DRAM_MODULE_ID,
        density);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    *density = ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].density;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].density,
        "dnx_dram_db[%d]->vendor_info[%d].density",
        unit, vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_VENDOR_INFO_DENSITY_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_manufacturer_id_set(int unit, uint32 vendor_info_idx_0, uint32 manufacturer_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vendor_info_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].manufacturer_id,
        manufacturer_id,
        uint32,
        0,
        "dnx_dram_db_vendor_info_manufacturer_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_DRAM_MODULE_ID,
        &manufacturer_id,
        "dnx_dram_db[%d]->vendor_info[%d].manufacturer_id",
        unit, vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_VENDOR_INFO_MANUFACTURER_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_manufacturer_id_get(int unit, uint32 vendor_info_idx_0, uint32 *manufacturer_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vendor_info_idx_0,
        2);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_DRAM_MODULE_ID,
        manufacturer_id);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    *manufacturer_id = ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].manufacturer_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].manufacturer_id,
        "dnx_dram_db[%d]->vendor_info[%d].manufacturer_id",
        unit, vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_VENDOR_INFO_MANUFACTURER_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_manufacturing_location_set(int unit, uint32 vendor_info_idx_0, uint32 manufacturing_location)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vendor_info_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].manufacturing_location,
        manufacturing_location,
        uint32,
        0,
        "dnx_dram_db_vendor_info_manufacturing_location_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_DRAM_MODULE_ID,
        &manufacturing_location,
        "dnx_dram_db[%d]->vendor_info[%d].manufacturing_location",
        unit, vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_VENDOR_INFO_MANUFACTURING_LOCATION_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_manufacturing_location_get(int unit, uint32 vendor_info_idx_0, uint32 *manufacturing_location)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vendor_info_idx_0,
        2);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_DRAM_MODULE_ID,
        manufacturing_location);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    *manufacturing_location = ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].manufacturing_location;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].manufacturing_location,
        "dnx_dram_db[%d]->vendor_info[%d].manufacturing_location",
        unit, vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_VENDOR_INFO_MANUFACTURING_LOCATION_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_manufacturing_year_set(int unit, uint32 vendor_info_idx_0, uint32 manufacturing_year)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vendor_info_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].manufacturing_year,
        manufacturing_year,
        uint32,
        0,
        "dnx_dram_db_vendor_info_manufacturing_year_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_DRAM_MODULE_ID,
        &manufacturing_year,
        "dnx_dram_db[%d]->vendor_info[%d].manufacturing_year",
        unit, vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_VENDOR_INFO_MANUFACTURING_YEAR_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_manufacturing_year_get(int unit, uint32 vendor_info_idx_0, uint32 *manufacturing_year)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vendor_info_idx_0,
        2);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_DRAM_MODULE_ID,
        manufacturing_year);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    *manufacturing_year = ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].manufacturing_year;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].manufacturing_year,
        "dnx_dram_db[%d]->vendor_info[%d].manufacturing_year",
        unit, vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_VENDOR_INFO_MANUFACTURING_YEAR_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_manufacturing_week_set(int unit, uint32 vendor_info_idx_0, uint32 manufacturing_week)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vendor_info_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].manufacturing_week,
        manufacturing_week,
        uint32,
        0,
        "dnx_dram_db_vendor_info_manufacturing_week_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_DRAM_MODULE_ID,
        &manufacturing_week,
        "dnx_dram_db[%d]->vendor_info[%d].manufacturing_week",
        unit, vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_VENDOR_INFO_MANUFACTURING_WEEK_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_manufacturing_week_get(int unit, uint32 vendor_info_idx_0, uint32 *manufacturing_week)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vendor_info_idx_0,
        2);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_DRAM_MODULE_ID,
        manufacturing_week);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    *manufacturing_week = ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].manufacturing_week;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].manufacturing_week,
        "dnx_dram_db[%d]->vendor_info[%d].manufacturing_week",
        unit, vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_VENDOR_INFO_MANUFACTURING_WEEK_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_serial_number_set(int unit, uint32 vendor_info_idx_0, uint32 serial_number_idx_0, uint32 serial_number)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vendor_info_idx_0,
        2);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        serial_number_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].serial_number[serial_number_idx_0],
        serial_number,
        uint32,
        0,
        "dnx_dram_db_vendor_info_serial_number_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_DRAM_MODULE_ID,
        &serial_number,
        "dnx_dram_db[%d]->vendor_info[%d].serial_number[%d]",
        unit, vendor_info_idx_0, serial_number_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_VENDOR_INFO_SERIAL_NUMBER_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_serial_number_get(int unit, uint32 vendor_info_idx_0, uint32 serial_number_idx_0, uint32 *serial_number)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vendor_info_idx_0,
        2);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_DRAM_MODULE_ID,
        serial_number);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        serial_number_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    *serial_number = ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].serial_number[serial_number_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].serial_number[serial_number_idx_0],
        "dnx_dram_db[%d]->vendor_info[%d].serial_number[%d]",
        unit, vendor_info_idx_0, serial_number_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_VENDOR_INFO_SERIAL_NUMBER_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_addressing_mode_set(int unit, uint32 vendor_info_idx_0, uint32 addressing_mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vendor_info_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].addressing_mode,
        addressing_mode,
        uint32,
        0,
        "dnx_dram_db_vendor_info_addressing_mode_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_DRAM_MODULE_ID,
        &addressing_mode,
        "dnx_dram_db[%d]->vendor_info[%d].addressing_mode",
        unit, vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_VENDOR_INFO_ADDRESSING_MODE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_addressing_mode_get(int unit, uint32 vendor_info_idx_0, uint32 *addressing_mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vendor_info_idx_0,
        2);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_DRAM_MODULE_ID,
        addressing_mode);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    *addressing_mode = ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].addressing_mode;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].addressing_mode,
        "dnx_dram_db[%d]->vendor_info[%d].addressing_mode",
        unit, vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_VENDOR_INFO_ADDRESSING_MODE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_channel_available_set(int unit, uint32 vendor_info_idx_0, uint32 channel_available)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vendor_info_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].channel_available,
        channel_available,
        uint32,
        0,
        "dnx_dram_db_vendor_info_channel_available_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_DRAM_MODULE_ID,
        &channel_available,
        "dnx_dram_db[%d]->vendor_info[%d].channel_available",
        unit, vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_VENDOR_INFO_CHANNEL_AVAILABLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_channel_available_get(int unit, uint32 vendor_info_idx_0, uint32 *channel_available)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vendor_info_idx_0,
        2);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_DRAM_MODULE_ID,
        channel_available);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    *channel_available = ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].channel_available;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].channel_available,
        "dnx_dram_db[%d]->vendor_info[%d].channel_available",
        unit, vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_VENDOR_INFO_CHANNEL_AVAILABLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_hbm_stack_hight_set(int unit, uint32 vendor_info_idx_0, uint32 hbm_stack_hight)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vendor_info_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].hbm_stack_hight,
        hbm_stack_hight,
        uint32,
        0,
        "dnx_dram_db_vendor_info_hbm_stack_hight_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_DRAM_MODULE_ID,
        &hbm_stack_hight,
        "dnx_dram_db[%d]->vendor_info[%d].hbm_stack_hight",
        unit, vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_VENDOR_INFO_HBM_STACK_HIGHT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_hbm_stack_hight_get(int unit, uint32 vendor_info_idx_0, uint32 *hbm_stack_hight)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vendor_info_idx_0,
        2);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_DRAM_MODULE_ID,
        hbm_stack_hight);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    *hbm_stack_hight = ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].hbm_stack_hight;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].hbm_stack_hight,
        "dnx_dram_db[%d]->vendor_info[%d].hbm_stack_hight",
        unit, vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_VENDOR_INFO_HBM_STACK_HIGHT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_model_part_number_set(int unit, uint32 vendor_info_idx_0, uint32 model_part_number)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vendor_info_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].model_part_number,
        model_part_number,
        uint32,
        0,
        "dnx_dram_db_vendor_info_model_part_number_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_DRAM_MODULE_ID,
        &model_part_number,
        "dnx_dram_db[%d]->vendor_info[%d].model_part_number",
        unit, vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_VENDOR_INFO_MODEL_PART_NUMBER_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_model_part_number_get(int unit, uint32 vendor_info_idx_0, uint32 *model_part_number)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vendor_info_idx_0,
        2);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_DRAM_MODULE_ID,
        model_part_number);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    *model_part_number = ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].model_part_number;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[vendor_info_idx_0].model_part_number,
        "dnx_dram_db[%d]->vendor_info[%d].model_part_number",
        unit, vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_VENDOR_INFO_MODEL_PART_NUMBER_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_gddr6_vendor_info_is_init_set(int unit, uint32 gddr6_vendor_info_idx_0, uint32 is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        gddr6_vendor_info_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[gddr6_vendor_info_idx_0].is_init,
        is_init,
        uint32,
        0,
        "dnx_dram_db_gddr6_vendor_info_is_init_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_DRAM_MODULE_ID,
        &is_init,
        "dnx_dram_db[%d]->gddr6_vendor_info[%d].is_init",
        unit, gddr6_vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_GDDR6_VENDOR_INFO_IS_INIT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_gddr6_vendor_info_is_init_get(int unit, uint32 gddr6_vendor_info_idx_0, uint32 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        gddr6_vendor_info_idx_0,
        2);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_DRAM_MODULE_ID,
        is_init);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    *is_init = ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[gddr6_vendor_info_idx_0].is_init;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[gddr6_vendor_info_idx_0].is_init,
        "dnx_dram_db[%d]->gddr6_vendor_info[%d].is_init",
        unit, gddr6_vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_GDDR6_VENDOR_INFO_IS_INIT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_gddr6_vendor_info_manufacturer_vendor_code_set(int unit, uint32 gddr6_vendor_info_idx_0, uint32 manufacturer_vendor_code)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        gddr6_vendor_info_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[gddr6_vendor_info_idx_0].manufacturer_vendor_code,
        manufacturer_vendor_code,
        uint32,
        0,
        "dnx_dram_db_gddr6_vendor_info_manufacturer_vendor_code_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_DRAM_MODULE_ID,
        &manufacturer_vendor_code,
        "dnx_dram_db[%d]->gddr6_vendor_info[%d].manufacturer_vendor_code",
        unit, gddr6_vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_GDDR6_VENDOR_INFO_MANUFACTURER_VENDOR_CODE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_gddr6_vendor_info_manufacturer_vendor_code_get(int unit, uint32 gddr6_vendor_info_idx_0, uint32 *manufacturer_vendor_code)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        gddr6_vendor_info_idx_0,
        2);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_DRAM_MODULE_ID,
        manufacturer_vendor_code);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    *manufacturer_vendor_code = ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[gddr6_vendor_info_idx_0].manufacturer_vendor_code;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[gddr6_vendor_info_idx_0].manufacturer_vendor_code,
        "dnx_dram_db[%d]->gddr6_vendor_info[%d].manufacturer_vendor_code",
        unit, gddr6_vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_GDDR6_VENDOR_INFO_MANUFACTURER_VENDOR_CODE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_gddr6_vendor_info_revision_id_set(int unit, uint32 gddr6_vendor_info_idx_0, uint32 revision_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        gddr6_vendor_info_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[gddr6_vendor_info_idx_0].revision_id,
        revision_id,
        uint32,
        0,
        "dnx_dram_db_gddr6_vendor_info_revision_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_DRAM_MODULE_ID,
        &revision_id,
        "dnx_dram_db[%d]->gddr6_vendor_info[%d].revision_id",
        unit, gddr6_vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_GDDR6_VENDOR_INFO_REVISION_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_gddr6_vendor_info_revision_id_get(int unit, uint32 gddr6_vendor_info_idx_0, uint32 *revision_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        gddr6_vendor_info_idx_0,
        2);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_DRAM_MODULE_ID,
        revision_id);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    *revision_id = ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[gddr6_vendor_info_idx_0].revision_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[gddr6_vendor_info_idx_0].revision_id,
        "dnx_dram_db[%d]->gddr6_vendor_info[%d].revision_id",
        unit, gddr6_vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_GDDR6_VENDOR_INFO_REVISION_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_gddr6_vendor_info_density_set(int unit, uint32 gddr6_vendor_info_idx_0, uint32 density)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        gddr6_vendor_info_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[gddr6_vendor_info_idx_0].density,
        density,
        uint32,
        0,
        "dnx_dram_db_gddr6_vendor_info_density_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_DRAM_MODULE_ID,
        &density,
        "dnx_dram_db[%d]->gddr6_vendor_info[%d].density",
        unit, gddr6_vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_GDDR6_VENDOR_INFO_DENSITY_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_gddr6_vendor_info_density_get(int unit, uint32 gddr6_vendor_info_idx_0, uint32 *density)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        gddr6_vendor_info_idx_0,
        2);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_DRAM_MODULE_ID,
        density);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    *density = ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[gddr6_vendor_info_idx_0].density;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[gddr6_vendor_info_idx_0].density,
        "dnx_dram_db[%d]->gddr6_vendor_info[%d].density",
        unit, gddr6_vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_GDDR6_VENDOR_INFO_DENSITY_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_gddr6_vendor_info_internal_wck_set(int unit, uint32 gddr6_vendor_info_idx_0, uint32 internal_wck)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        gddr6_vendor_info_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[gddr6_vendor_info_idx_0].internal_wck,
        internal_wck,
        uint32,
        0,
        "dnx_dram_db_gddr6_vendor_info_internal_wck_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_DRAM_MODULE_ID,
        &internal_wck,
        "dnx_dram_db[%d]->gddr6_vendor_info[%d].internal_wck",
        unit, gddr6_vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_GDDR6_VENDOR_INFO_INTERNAL_WCK_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_gddr6_vendor_info_internal_wck_get(int unit, uint32 gddr6_vendor_info_idx_0, uint32 *internal_wck)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        gddr6_vendor_info_idx_0,
        2);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_DRAM_MODULE_ID,
        internal_wck);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    *internal_wck = ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[gddr6_vendor_info_idx_0].internal_wck;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[gddr6_vendor_info_idx_0].internal_wck,
        "dnx_dram_db[%d]->gddr6_vendor_info[%d].internal_wck",
        unit, gddr6_vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_GDDR6_VENDOR_INFO_INTERNAL_WCK_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_gddr6_vendor_info_wck_granularity_set(int unit, uint32 gddr6_vendor_info_idx_0, uint32 wck_granularity)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        gddr6_vendor_info_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[gddr6_vendor_info_idx_0].wck_granularity,
        wck_granularity,
        uint32,
        0,
        "dnx_dram_db_gddr6_vendor_info_wck_granularity_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_DRAM_MODULE_ID,
        &wck_granularity,
        "dnx_dram_db[%d]->gddr6_vendor_info[%d].wck_granularity",
        unit, gddr6_vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_GDDR6_VENDOR_INFO_WCK_GRANULARITY_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_gddr6_vendor_info_wck_granularity_get(int unit, uint32 gddr6_vendor_info_idx_0, uint32 *wck_granularity)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        gddr6_vendor_info_idx_0,
        2);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_DRAM_MODULE_ID,
        wck_granularity);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    *wck_granularity = ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[gddr6_vendor_info_idx_0].wck_granularity;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[gddr6_vendor_info_idx_0].wck_granularity,
        "dnx_dram_db[%d]->gddr6_vendor_info[%d].wck_granularity",
        unit, gddr6_vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_GDDR6_VENDOR_INFO_WCK_GRANULARITY_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_gddr6_vendor_info_wck_frequency_set(int unit, uint32 gddr6_vendor_info_idx_0, uint32 wck_frequency)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        gddr6_vendor_info_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[gddr6_vendor_info_idx_0].wck_frequency,
        wck_frequency,
        uint32,
        0,
        "dnx_dram_db_gddr6_vendor_info_wck_frequency_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_DRAM_MODULE_ID,
        &wck_frequency,
        "dnx_dram_db[%d]->gddr6_vendor_info[%d].wck_frequency",
        unit, gddr6_vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_GDDR6_VENDOR_INFO_WCK_FREQUENCY_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_gddr6_vendor_info_wck_frequency_get(int unit, uint32 gddr6_vendor_info_idx_0, uint32 *wck_frequency)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        gddr6_vendor_info_idx_0,
        2);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_DRAM_MODULE_ID,
        wck_frequency);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    *wck_frequency = ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[gddr6_vendor_info_idx_0].wck_frequency;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[gddr6_vendor_info_idx_0].wck_frequency,
        "dnx_dram_db[%d]->gddr6_vendor_info[%d].wck_frequency",
        unit, gddr6_vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_GDDR6_VENDOR_INFO_WCK_FREQUENCY_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_gddr6_vendor_info_vddq_off_set(int unit, uint32 gddr6_vendor_info_idx_0, uint32 vddq_off)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        gddr6_vendor_info_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_DRAM_MODULE_ID,
        ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[gddr6_vendor_info_idx_0].vddq_off,
        vddq_off,
        uint32,
        0,
        "dnx_dram_db_gddr6_vendor_info_vddq_off_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_DRAM_MODULE_ID,
        &vddq_off,
        "dnx_dram_db[%d]->gddr6_vendor_info[%d].vddq_off",
        unit, gddr6_vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_GDDR6_VENDOR_INFO_VDDQ_OFF_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_gddr6_vendor_info_vddq_off_get(int unit, uint32 gddr6_vendor_info_idx_0, uint32 *vddq_off)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_DRAM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_DRAM_MODULE_ID,
        sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        gddr6_vendor_info_idx_0,
        2);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_DRAM_MODULE_ID,
        vddq_off);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_DRAM_MODULE_ID);

    *vddq_off = ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[gddr6_vendor_info_idx_0].vddq_off;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_DRAM_MODULE_ID,
        &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[gddr6_vendor_info_idx_0].vddq_off,
        "dnx_dram_db[%d]->gddr6_vendor_info[%d].vddq_off",
        unit, gddr6_vendor_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_DRAM_MODULE_ID,
        dnx_dram_db_info,
        DNX_DRAM_DB_GDDR6_VENDOR_INFO_VDDQ_OFF_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




const char *
dnx_dram_temperature_monitoring_state_e_get_name(dnx_dram_temperature_monitoring_state_e value)
{
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_DRAM_NORMAL_TEMP", value, DNX_DRAM_NORMAL_TEMP);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_DRAM_HIGH_TEMP", value, DNX_DRAM_HIGH_TEMP);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_DRAM_LOW_TEMP", value, DNX_DRAM_LOW_TEMP);

    return NULL;
}





dnx_dram_db_cbs dnx_dram_db = 	{
	
	dnx_dram_db_is_init,
	dnx_dram_db_init,
		{
		
		dnx_dram_db_is_initialized_set,
		dnx_dram_db_is_initialized_get}
	,
		{
		
			{
			
			dnx_dram_db_sync_manager_deleted_buffers_file_mutex_create,
			dnx_dram_db_sync_manager_deleted_buffers_file_mutex_is_created,
			dnx_dram_db_sync_manager_deleted_buffers_file_mutex_take,
			dnx_dram_db_sync_manager_deleted_buffers_file_mutex_give}
		}
	,
		{
		
			{
			
			dnx_dram_db_power_down_callback_callback_set,
			dnx_dram_db_power_down_callback_callback_get}
		,
			{
			
			dnx_dram_db_power_down_callback_userdata_set,
			dnx_dram_db_power_down_callback_userdata_get}
		,
			{
			
			dnx_dram_db_power_down_callback_power_on_set,
			dnx_dram_db_power_down_callback_power_on_get}
		}
	,
		{
		
		dnx_dram_db_override_bist_configurations_during_tuning_set,
		dnx_dram_db_override_bist_configurations_during_tuning_get}
	,
		{
		
		dnx_dram_db_channel_soft_init_after_bist_set,
		dnx_dram_db_channel_soft_init_after_bist_get}
	,
		{
		
		dnx_dram_db_traffic_tuning_core_mode_set,
		dnx_dram_db_traffic_tuning_core_mode_get}
	,
		{
		
		dnx_dram_db_dynamic_calibration_enabled_set,
		dnx_dram_db_dynamic_calibration_enabled_get}
	,
		{
		
		dnx_dram_db_dynamic_calibration_was_enabled_after_dram_tune_set,
		dnx_dram_db_dynamic_calibration_was_enabled_after_dram_tune_get}
	,
		{
		
		dnx_dram_db_temperature_monitoring_thread_state_set,
		dnx_dram_db_temperature_monitoring_thread_state_get}
	,
		{
		
		dnx_dram_db_temperature_monitoring_thread_handler_set,
		dnx_dram_db_temperature_monitoring_thread_handler_get}
	,
		{
		
			{
			
			dnx_dram_db_vendor_info_is_init_set,
			dnx_dram_db_vendor_info_is_init_get}
		,
			{
			
			dnx_dram_db_vendor_info_gen2_test_set,
			dnx_dram_db_vendor_info_gen2_test_get}
		,
			{
			
			dnx_dram_db_vendor_info_ecc_set,
			dnx_dram_db_vendor_info_ecc_get}
		,
			{
			
			dnx_dram_db_vendor_info_density_set,
			dnx_dram_db_vendor_info_density_get}
		,
			{
			
			dnx_dram_db_vendor_info_manufacturer_id_set,
			dnx_dram_db_vendor_info_manufacturer_id_get}
		,
			{
			
			dnx_dram_db_vendor_info_manufacturing_location_set,
			dnx_dram_db_vendor_info_manufacturing_location_get}
		,
			{
			
			dnx_dram_db_vendor_info_manufacturing_year_set,
			dnx_dram_db_vendor_info_manufacturing_year_get}
		,
			{
			
			dnx_dram_db_vendor_info_manufacturing_week_set,
			dnx_dram_db_vendor_info_manufacturing_week_get}
		,
			{
			
			dnx_dram_db_vendor_info_serial_number_set,
			dnx_dram_db_vendor_info_serial_number_get}
		,
			{
			
			dnx_dram_db_vendor_info_addressing_mode_set,
			dnx_dram_db_vendor_info_addressing_mode_get}
		,
			{
			
			dnx_dram_db_vendor_info_channel_available_set,
			dnx_dram_db_vendor_info_channel_available_get}
		,
			{
			
			dnx_dram_db_vendor_info_hbm_stack_hight_set,
			dnx_dram_db_vendor_info_hbm_stack_hight_get}
		,
			{
			
			dnx_dram_db_vendor_info_model_part_number_set,
			dnx_dram_db_vendor_info_model_part_number_get}
		}
	,
		{
		
			{
			
			dnx_dram_db_gddr6_vendor_info_is_init_set,
			dnx_dram_db_gddr6_vendor_info_is_init_get}
		,
			{
			
			dnx_dram_db_gddr6_vendor_info_manufacturer_vendor_code_set,
			dnx_dram_db_gddr6_vendor_info_manufacturer_vendor_code_get}
		,
			{
			
			dnx_dram_db_gddr6_vendor_info_revision_id_set,
			dnx_dram_db_gddr6_vendor_info_revision_id_get}
		,
			{
			
			dnx_dram_db_gddr6_vendor_info_density_set,
			dnx_dram_db_gddr6_vendor_info_density_get}
		,
			{
			
			dnx_dram_db_gddr6_vendor_info_internal_wck_set,
			dnx_dram_db_gddr6_vendor_info_internal_wck_get}
		,
			{
			
			dnx_dram_db_gddr6_vendor_info_wck_granularity_set,
			dnx_dram_db_gddr6_vendor_info_wck_granularity_get}
		,
			{
			
			dnx_dram_db_gddr6_vendor_info_wck_frequency_set,
			dnx_dram_db_gddr6_vendor_info_wck_frequency_get}
		,
			{
			
			dnx_dram_db_gddr6_vendor_info_vddq_off_set,
			dnx_dram_db_gddr6_vendor_info_vddq_off_get}
		}
	}
;
#undef BSL_LOG_MODULE
