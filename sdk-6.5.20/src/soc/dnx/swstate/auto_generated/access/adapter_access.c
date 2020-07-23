
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
#include <soc/dnx/swstate/auto_generated/access/adapter_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/adapter_diagnostic.h>



adapter_basic_access_info_t* adapter_context_db_dummy = NULL;
adapter_basic_access_info_t* adapter_root[SOC_MAX_NUM_DEVICES] = {NULL};



int
adapter_context_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ADAPTER_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    *is_init = (NULL != ((adapter_basic_access_info_t*)adapter_root[unit]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit]),
        "adapter_context_db[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ADAPTER_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    DNX_SW_STATE_NO_WB_MODULE_INIT(
        unit,
        ADAPTER_MODULE_ID,
        adapter_root[unit],
        DNXC_SW_STATE_IMPLEMENTATION_PLAIN,
        adapter_basic_access_info_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "adapter_context_db_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit]),
        "adapter_context_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        ADAPTER_MODULE_ID,
        adapter_context_db_info,
        ADAPTER_CONTEXT_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(adapter_basic_access_info_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ADAPTER_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit]));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ADAPTER_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params,
        dnx_adapter_context_params_t,
        dnx_data_device.general.nof_sub_units_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "adapter_context_db_params_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params,
        "adapter_context_db[%d]->->params",
        unit,
        dnx_data_device.general.nof_sub_units_get(unit) * sizeof(dnx_adapter_context_params_t) / sizeof(*((adapter_basic_access_info_t*)adapter_root[unit])->params));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ADAPTER_MODULE_ID,
        adapter_context_db_info,
        ADAPTER_CONTEXT_DB_PARAMS_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_rx_tx_fd_set(int unit, uint32 params_idx_0, int adapter_rx_tx_fd)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ADAPTER_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit]));

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params,
        params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ADAPTER_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_rx_tx_fd,
        adapter_rx_tx_fd,
        int,
        0,
        "adapter_context_db_params_adapter_rx_tx_fd_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        ADAPTER_MODULE_ID,
        &adapter_rx_tx_fd,
        "adapter_context_db[%d]->->params[%d].adapter_rx_tx_fd",
        unit, params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ADAPTER_MODULE_ID,
        adapter_context_db_info,
        ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_RX_TX_FD_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_rx_tx_fd_get(int unit, uint32 params_idx_0, int *adapter_rx_tx_fd)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ADAPTER_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit]));

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params,
        params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ADAPTER_MODULE_ID,
        adapter_rx_tx_fd);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ADAPTER_MODULE_ID);

    *adapter_rx_tx_fd = ((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_rx_tx_fd;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        ADAPTER_MODULE_ID,
        &((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_rx_tx_fd,
        "adapter_context_db[%d]->->params[%d].adapter_rx_tx_fd",
        unit, params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ADAPTER_MODULE_ID,
        adapter_context_db_info,
        ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_RX_TX_FD_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_rx_tx_mutex_create(int unit, uint32 params_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ADAPTER_MODULE_ID,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit]));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ADAPTER_MODULE_ID);

    DNX_SW_STATE_MUTEX_CREATE(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_rx_tx_mutex,
        "((adapter_basic_access_info_t*)adapter_root.adapter_rx_tx_mutex");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MUTEX_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXCREATE,
        ADAPTER_MODULE_ID,
        &((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_rx_tx_mutex,
        "adapter_context_db[%d]->->params[%d].adapter_rx_tx_mutex",
        unit, params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ADAPTER_MODULE_ID,
        adapter_context_db_info,
        ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_RX_TX_MUTEX_INFO,
        DNX_SW_STATE_DIAG_MUTEX,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_rx_tx_mutex_is_created(int unit, uint32 params_idx_0, uint8 *is_created)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ADAPTER_MODULE_ID,
        SW_STATE_FUNC_IS_CREATED,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit]));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ADAPTER_MODULE_ID);

    DNX_SW_STATE_MUTEX_IS_CREATED(
        unit,
        ADAPTER_MODULE_ID,
        is_created,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_rx_tx_mutex,
        "((adapter_basic_access_info_t*)adapter_root.adapter_rx_tx_mutex");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MUTEX_IS_CREATED_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXCREATE,
        ADAPTER_MODULE_ID,
        &((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_rx_tx_mutex,
        "adapter_context_db[%d]->->params[%d].adapter_rx_tx_mutex",
        unit, params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ADAPTER_MODULE_ID,
        adapter_context_db_info,
        ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_RX_TX_MUTEX_INFO,
        DNX_SW_STATE_DIAG_MUTEX,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_rx_tx_mutex_take(int unit, uint32 params_idx_0, int usec)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ADAPTER_MODULE_ID,
        SW_STATE_FUNC_TAKE,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit]));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ADAPTER_MODULE_ID);

    DNX_SW_STATE_MUTEX_TAKE(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_rx_tx_mutex,
        usec);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MUTEX_TAKE_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXTAKE,
        ADAPTER_MODULE_ID,
        &((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_rx_tx_mutex,
        "adapter_context_db[%d]->->params[%d].adapter_rx_tx_mutex",
        unit, params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ADAPTER_MODULE_ID,
        adapter_context_db_info,
        ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_RX_TX_MUTEX_INFO,
        DNX_SW_STATE_DIAG_MUTEX,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_rx_tx_mutex_give(int unit, uint32 params_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ADAPTER_MODULE_ID,
        SW_STATE_FUNC_GIVE,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit]));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ADAPTER_MODULE_ID);

    DNX_SW_STATE_MUTEX_GIVE(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_rx_tx_mutex);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MUTEX_GIVE_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXGIVE,
        ADAPTER_MODULE_ID,
        &((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_rx_tx_mutex,
        "adapter_context_db[%d]->->params[%d].adapter_rx_tx_mutex",
        unit, params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ADAPTER_MODULE_ID,
        adapter_context_db_info,
        ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_RX_TX_MUTEX_INFO,
        DNX_SW_STATE_DIAG_MUTEX,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_mem_reg_fd_set(int unit, uint32 params_idx_0, int adapter_mem_reg_fd)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ADAPTER_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit]));

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params,
        params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ADAPTER_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_mem_reg_fd,
        adapter_mem_reg_fd,
        int,
        0,
        "adapter_context_db_params_adapter_mem_reg_fd_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        ADAPTER_MODULE_ID,
        &adapter_mem_reg_fd,
        "adapter_context_db[%d]->->params[%d].adapter_mem_reg_fd",
        unit, params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ADAPTER_MODULE_ID,
        adapter_context_db_info,
        ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_MEM_REG_FD_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_mem_reg_fd_get(int unit, uint32 params_idx_0, int *adapter_mem_reg_fd)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ADAPTER_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit]));

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params,
        params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ADAPTER_MODULE_ID,
        adapter_mem_reg_fd);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ADAPTER_MODULE_ID);

    *adapter_mem_reg_fd = ((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_mem_reg_fd;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        ADAPTER_MODULE_ID,
        &((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_mem_reg_fd,
        "adapter_context_db[%d]->->params[%d].adapter_mem_reg_fd",
        unit, params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ADAPTER_MODULE_ID,
        adapter_context_db_info,
        ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_MEM_REG_FD_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_mem_reg_mutex_create(int unit, uint32 params_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ADAPTER_MODULE_ID,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit]));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ADAPTER_MODULE_ID);

    DNX_SW_STATE_MUTEX_CREATE(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_mem_reg_mutex,
        "((adapter_basic_access_info_t*)adapter_root.adapter_mem_reg_mutex");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MUTEX_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXCREATE,
        ADAPTER_MODULE_ID,
        &((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_mem_reg_mutex,
        "adapter_context_db[%d]->->params[%d].adapter_mem_reg_mutex",
        unit, params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ADAPTER_MODULE_ID,
        adapter_context_db_info,
        ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_MEM_REG_MUTEX_INFO,
        DNX_SW_STATE_DIAG_MUTEX,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_mem_reg_mutex_is_created(int unit, uint32 params_idx_0, uint8 *is_created)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ADAPTER_MODULE_ID,
        SW_STATE_FUNC_IS_CREATED,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit]));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ADAPTER_MODULE_ID);

    DNX_SW_STATE_MUTEX_IS_CREATED(
        unit,
        ADAPTER_MODULE_ID,
        is_created,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_mem_reg_mutex,
        "((adapter_basic_access_info_t*)adapter_root.adapter_mem_reg_mutex");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MUTEX_IS_CREATED_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXCREATE,
        ADAPTER_MODULE_ID,
        &((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_mem_reg_mutex,
        "adapter_context_db[%d]->->params[%d].adapter_mem_reg_mutex",
        unit, params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ADAPTER_MODULE_ID,
        adapter_context_db_info,
        ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_MEM_REG_MUTEX_INFO,
        DNX_SW_STATE_DIAG_MUTEX,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_mem_reg_mutex_take(int unit, uint32 params_idx_0, int usec)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ADAPTER_MODULE_ID,
        SW_STATE_FUNC_TAKE,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit]));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ADAPTER_MODULE_ID);

    DNX_SW_STATE_MUTEX_TAKE(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_mem_reg_mutex,
        usec);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MUTEX_TAKE_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXTAKE,
        ADAPTER_MODULE_ID,
        &((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_mem_reg_mutex,
        "adapter_context_db[%d]->->params[%d].adapter_mem_reg_mutex",
        unit, params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ADAPTER_MODULE_ID,
        adapter_context_db_info,
        ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_MEM_REG_MUTEX_INFO,
        DNX_SW_STATE_DIAG_MUTEX,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_mem_reg_mutex_give(int unit, uint32 params_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ADAPTER_MODULE_ID,
        SW_STATE_FUNC_GIVE,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit]));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ADAPTER_MODULE_ID);

    DNX_SW_STATE_MUTEX_GIVE(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_mem_reg_mutex);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MUTEX_GIVE_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXGIVE,
        ADAPTER_MODULE_ID,
        &((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_mem_reg_mutex,
        "adapter_context_db[%d]->->params[%d].adapter_mem_reg_mutex",
        unit, params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ADAPTER_MODULE_ID,
        adapter_context_db_info,
        ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_MEM_REG_MUTEX_INFO,
        DNX_SW_STATE_DIAG_MUTEX,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_sdk_interface_fd_set(int unit, uint32 params_idx_0, int adapter_sdk_interface_fd)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ADAPTER_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit]));

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params,
        params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ADAPTER_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_sdk_interface_fd,
        adapter_sdk_interface_fd,
        int,
        0,
        "adapter_context_db_params_adapter_sdk_interface_fd_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        ADAPTER_MODULE_ID,
        &adapter_sdk_interface_fd,
        "adapter_context_db[%d]->->params[%d].adapter_sdk_interface_fd",
        unit, params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ADAPTER_MODULE_ID,
        adapter_context_db_info,
        ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_SDK_INTERFACE_FD_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_sdk_interface_fd_get(int unit, uint32 params_idx_0, int *adapter_sdk_interface_fd)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ADAPTER_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit]));

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params,
        params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ADAPTER_MODULE_ID,
        adapter_sdk_interface_fd);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ADAPTER_MODULE_ID);

    *adapter_sdk_interface_fd = ((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_sdk_interface_fd;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        ADAPTER_MODULE_ID,
        &((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_sdk_interface_fd,
        "adapter_context_db[%d]->->params[%d].adapter_sdk_interface_fd",
        unit, params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ADAPTER_MODULE_ID,
        adapter_context_db_info,
        ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_SDK_INTERFACE_FD_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_sdk_interface_mutex_create(int unit, uint32 params_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ADAPTER_MODULE_ID,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit]));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ADAPTER_MODULE_ID);

    DNX_SW_STATE_MUTEX_CREATE(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_sdk_interface_mutex,
        "((adapter_basic_access_info_t*)adapter_root.adapter_sdk_interface_mutex");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MUTEX_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXCREATE,
        ADAPTER_MODULE_ID,
        &((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_sdk_interface_mutex,
        "adapter_context_db[%d]->->params[%d].adapter_sdk_interface_mutex",
        unit, params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ADAPTER_MODULE_ID,
        adapter_context_db_info,
        ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_SDK_INTERFACE_MUTEX_INFO,
        DNX_SW_STATE_DIAG_MUTEX,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_sdk_interface_mutex_is_created(int unit, uint32 params_idx_0, uint8 *is_created)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ADAPTER_MODULE_ID,
        SW_STATE_FUNC_IS_CREATED,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit]));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ADAPTER_MODULE_ID);

    DNX_SW_STATE_MUTEX_IS_CREATED(
        unit,
        ADAPTER_MODULE_ID,
        is_created,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_sdk_interface_mutex,
        "((adapter_basic_access_info_t*)adapter_root.adapter_sdk_interface_mutex");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MUTEX_IS_CREATED_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXCREATE,
        ADAPTER_MODULE_ID,
        &((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_sdk_interface_mutex,
        "adapter_context_db[%d]->->params[%d].adapter_sdk_interface_mutex",
        unit, params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ADAPTER_MODULE_ID,
        adapter_context_db_info,
        ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_SDK_INTERFACE_MUTEX_INFO,
        DNX_SW_STATE_DIAG_MUTEX,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_sdk_interface_mutex_take(int unit, uint32 params_idx_0, int usec)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ADAPTER_MODULE_ID,
        SW_STATE_FUNC_TAKE,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit]));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ADAPTER_MODULE_ID);

    DNX_SW_STATE_MUTEX_TAKE(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_sdk_interface_mutex,
        usec);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MUTEX_TAKE_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXTAKE,
        ADAPTER_MODULE_ID,
        &((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_sdk_interface_mutex,
        "adapter_context_db[%d]->->params[%d].adapter_sdk_interface_mutex",
        unit, params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ADAPTER_MODULE_ID,
        adapter_context_db_info,
        ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_SDK_INTERFACE_MUTEX_INFO,
        DNX_SW_STATE_DIAG_MUTEX,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_sdk_interface_mutex_give(int unit, uint32 params_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ADAPTER_MODULE_ID,
        SW_STATE_FUNC_GIVE,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit]));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ADAPTER_MODULE_ID);

    DNX_SW_STATE_MUTEX_GIVE(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_sdk_interface_mutex);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MUTEX_GIVE_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXGIVE,
        ADAPTER_MODULE_ID,
        &((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_sdk_interface_mutex,
        "adapter_context_db[%d]->->params[%d].adapter_sdk_interface_mutex",
        unit, params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ADAPTER_MODULE_ID,
        adapter_context_db_info,
        ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_SDK_INTERFACE_MUTEX_INFO,
        DNX_SW_STATE_DIAG_MUTEX,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_external_events_fd_set(int unit, uint32 params_idx_0, int adapter_external_events_fd)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ADAPTER_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit]));

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params,
        params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ADAPTER_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_external_events_fd,
        adapter_external_events_fd,
        int,
        0,
        "adapter_context_db_params_adapter_external_events_fd_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        ADAPTER_MODULE_ID,
        &adapter_external_events_fd,
        "adapter_context_db[%d]->->params[%d].adapter_external_events_fd",
        unit, params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ADAPTER_MODULE_ID,
        adapter_context_db_info,
        ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_EXTERNAL_EVENTS_FD_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_external_events_fd_get(int unit, uint32 params_idx_0, int *adapter_external_events_fd)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ADAPTER_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit]));

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params,
        params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ADAPTER_MODULE_ID,
        adapter_external_events_fd);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ADAPTER_MODULE_ID);

    *adapter_external_events_fd = ((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_external_events_fd;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        ADAPTER_MODULE_ID,
        &((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_external_events_fd,
        "adapter_context_db[%d]->->params[%d].adapter_external_events_fd",
        unit, params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ADAPTER_MODULE_ID,
        adapter_context_db_info,
        ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_EXTERNAL_EVENTS_FD_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_external_events_mutex_create(int unit, uint32 params_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ADAPTER_MODULE_ID,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit]));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ADAPTER_MODULE_ID);

    DNX_SW_STATE_MUTEX_CREATE(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_external_events_mutex,
        "((adapter_basic_access_info_t*)adapter_root.adapter_external_events_mutex");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MUTEX_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXCREATE,
        ADAPTER_MODULE_ID,
        &((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_external_events_mutex,
        "adapter_context_db[%d]->->params[%d].adapter_external_events_mutex",
        unit, params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ADAPTER_MODULE_ID,
        adapter_context_db_info,
        ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_EXTERNAL_EVENTS_MUTEX_INFO,
        DNX_SW_STATE_DIAG_MUTEX,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_external_events_mutex_is_created(int unit, uint32 params_idx_0, uint8 *is_created)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ADAPTER_MODULE_ID,
        SW_STATE_FUNC_IS_CREATED,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit]));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ADAPTER_MODULE_ID);

    DNX_SW_STATE_MUTEX_IS_CREATED(
        unit,
        ADAPTER_MODULE_ID,
        is_created,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_external_events_mutex,
        "((adapter_basic_access_info_t*)adapter_root.adapter_external_events_mutex");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MUTEX_IS_CREATED_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXCREATE,
        ADAPTER_MODULE_ID,
        &((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_external_events_mutex,
        "adapter_context_db[%d]->->params[%d].adapter_external_events_mutex",
        unit, params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ADAPTER_MODULE_ID,
        adapter_context_db_info,
        ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_EXTERNAL_EVENTS_MUTEX_INFO,
        DNX_SW_STATE_DIAG_MUTEX,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_external_events_mutex_take(int unit, uint32 params_idx_0, int usec)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ADAPTER_MODULE_ID,
        SW_STATE_FUNC_TAKE,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit]));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ADAPTER_MODULE_ID);

    DNX_SW_STATE_MUTEX_TAKE(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_external_events_mutex,
        usec);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MUTEX_TAKE_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXTAKE,
        ADAPTER_MODULE_ID,
        &((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_external_events_mutex,
        "adapter_context_db[%d]->->params[%d].adapter_external_events_mutex",
        unit, params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ADAPTER_MODULE_ID,
        adapter_context_db_info,
        ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_EXTERNAL_EVENTS_MUTEX_INFO,
        DNX_SW_STATE_DIAG_MUTEX,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_external_events_mutex_give(int unit, uint32 params_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ADAPTER_MODULE_ID,
        SW_STATE_FUNC_GIVE,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit]));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ADAPTER_MODULE_ID);

    DNX_SW_STATE_MUTEX_GIVE(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_external_events_mutex);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MUTEX_GIVE_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXGIVE,
        ADAPTER_MODULE_ID,
        &((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_external_events_mutex,
        "adapter_context_db[%d]->->params[%d].adapter_external_events_mutex",
        unit, params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ADAPTER_MODULE_ID,
        adapter_context_db_info,
        ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_EXTERNAL_EVENTS_MUTEX_INFO,
        DNX_SW_STATE_DIAG_MUTEX,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_external_events_port_set(int unit, uint32 params_idx_0, uint32 adapter_external_events_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ADAPTER_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit]));

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params,
        params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ADAPTER_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_external_events_port,
        adapter_external_events_port,
        uint32,
        0,
        "adapter_context_db_params_adapter_external_events_port_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        ADAPTER_MODULE_ID,
        &adapter_external_events_port,
        "adapter_context_db[%d]->->params[%d].adapter_external_events_port",
        unit, params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ADAPTER_MODULE_ID,
        adapter_context_db_info,
        ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_EXTERNAL_EVENTS_PORT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_external_events_port_get(int unit, uint32 params_idx_0, uint32 *adapter_external_events_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ADAPTER_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit]));

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params,
        params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ADAPTER_MODULE_ID,
        adapter_external_events_port);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ADAPTER_MODULE_ID);

    *adapter_external_events_port = ((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_external_events_port;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        ADAPTER_MODULE_ID,
        &((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_external_events_port,
        "adapter_context_db[%d]->->params[%d].adapter_external_events_port",
        unit, params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ADAPTER_MODULE_ID,
        adapter_context_db_info,
        ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_EXTERNAL_EVENTS_PORT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_server_address_set(int unit, uint32 params_idx_0, uint32 adapter_server_address)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ADAPTER_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit]));

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params,
        params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ADAPTER_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_server_address,
        adapter_server_address,
        uint32,
        0,
        "adapter_context_db_params_adapter_server_address_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        ADAPTER_MODULE_ID,
        &adapter_server_address,
        "adapter_context_db[%d]->->params[%d].adapter_server_address",
        unit, params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ADAPTER_MODULE_ID,
        adapter_context_db_info,
        ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_SERVER_ADDRESS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_server_address_get(int unit, uint32 params_idx_0, uint32 *adapter_server_address)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ADAPTER_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit]));

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params,
        params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ADAPTER_MODULE_ID,
        adapter_server_address);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ADAPTER_MODULE_ID,
        ((adapter_basic_access_info_t*)adapter_root[unit])->params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ADAPTER_MODULE_ID);

    *adapter_server_address = ((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_server_address;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        ADAPTER_MODULE_ID,
        &((adapter_basic_access_info_t*)adapter_root[unit])->params[params_idx_0].adapter_server_address,
        "adapter_context_db[%d]->->params[%d].adapter_server_address",
        unit, params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ADAPTER_MODULE_ID,
        adapter_context_db_info,
        ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_SERVER_ADDRESS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}





adapter_context_db_cbs adapter_context_db = 	{
	
	adapter_context_db_is_init,
	adapter_context_db_init,
		{
		
		adapter_context_db_params_alloc,
			{
			
			adapter_context_db_params_adapter_rx_tx_fd_set,
			adapter_context_db_params_adapter_rx_tx_fd_get}
		,
			{
			
			adapter_context_db_params_adapter_rx_tx_mutex_create,
			adapter_context_db_params_adapter_rx_tx_mutex_is_created,
			adapter_context_db_params_adapter_rx_tx_mutex_take,
			adapter_context_db_params_adapter_rx_tx_mutex_give}
		,
			{
			
			adapter_context_db_params_adapter_mem_reg_fd_set,
			adapter_context_db_params_adapter_mem_reg_fd_get}
		,
			{
			
			adapter_context_db_params_adapter_mem_reg_mutex_create,
			adapter_context_db_params_adapter_mem_reg_mutex_is_created,
			adapter_context_db_params_adapter_mem_reg_mutex_take,
			adapter_context_db_params_adapter_mem_reg_mutex_give}
		,
			{
			
			adapter_context_db_params_adapter_sdk_interface_fd_set,
			adapter_context_db_params_adapter_sdk_interface_fd_get}
		,
			{
			
			adapter_context_db_params_adapter_sdk_interface_mutex_create,
			adapter_context_db_params_adapter_sdk_interface_mutex_is_created,
			adapter_context_db_params_adapter_sdk_interface_mutex_take,
			adapter_context_db_params_adapter_sdk_interface_mutex_give}
		,
			{
			
			adapter_context_db_params_adapter_external_events_fd_set,
			adapter_context_db_params_adapter_external_events_fd_get}
		,
			{
			
			adapter_context_db_params_adapter_external_events_mutex_create,
			adapter_context_db_params_adapter_external_events_mutex_is_created,
			adapter_context_db_params_adapter_external_events_mutex_take,
			adapter_context_db_params_adapter_external_events_mutex_give}
		,
			{
			
			adapter_context_db_params_adapter_external_events_port_set,
			adapter_context_db_params_adapter_external_events_port_get}
		,
			{
			
			adapter_context_db_params_adapter_server_address_set,
			adapter_context_db_params_adapter_server_address_get}
		}
	}
;
#undef BSL_LOG_MODULE
