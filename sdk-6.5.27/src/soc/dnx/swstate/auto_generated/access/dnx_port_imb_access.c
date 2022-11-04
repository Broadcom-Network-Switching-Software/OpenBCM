
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/dnx_port_imb_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_port_imb_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_port_imb_layout.h>





int
imbm_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_IMBM,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_SW_STATE_IMBM,
        ((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID]),
        "imbm[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
imbm_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_IMBM,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_SW_STATE_IMBM,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_imbm_info_t,
        DNX_SW_STATE_IMBM_NOF_PARAMS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "imbm_init",
        DNX_SW_STATE_DIAG_ALLOC,
        dnx_port_imb_init_layout_structure);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_SW_STATE_IMBM,
        ((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID]),
        "imbm[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_IMBM,
        imbm_info,
        IMBM_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID]),
        sw_state_layout_array[unit][DNX_PORT_IMB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
imbm_imbs_in_use_set(int unit, uint32 imbs_in_use)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_IMBM__IMBS_IN_USE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_IMBM__IMBS_IN_USE,
        sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_IMBM__IMBS_IN_USE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_IMBM__IMBS_IN_USE,
        ((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->imbs_in_use,
        imbs_in_use,
        uint32,
        0,
        "imbm_imbs_in_use_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_IMBM__IMBS_IN_USE,
        &imbs_in_use,
        "imbm[%d]->imbs_in_use",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_IMBM__IMBS_IN_USE,
        imbm_info,
        IMBM_IMBS_IN_USE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_IMB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
imbm_imbs_in_use_get(int unit, uint32 *imbs_in_use)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_IMBM__IMBS_IN_USE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_IMBM__IMBS_IN_USE,
        sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_IMBM__IMBS_IN_USE,
        imbs_in_use);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_IMBM__IMBS_IN_USE,
        DNX_SW_STATE_DIAG_READ);

    *imbs_in_use = ((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->imbs_in_use;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_IMBM__IMBS_IN_USE,
        &((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->imbs_in_use,
        "imbm[%d]->imbs_in_use",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_IMBM__IMBS_IN_USE,
        imbm_info,
        IMBM_IMBS_IN_USE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_IMB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
imbm_imb_set(int unit, uint32 imb_idx_0, imb_create_info_t imb)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_IMBM__IMB,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_IMBM__IMB,
        sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_IMBM__IMB,
        ((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->imb,
        imb_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_IMBM__IMB,
        ((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->imb);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_IMBM__IMB,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_IMBM__IMB,
        ((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->imb[imb_idx_0],
        imb,
        imb_create_info_t,
        0,
        "imbm_imb_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_IMBM__IMB,
        &imb,
        "imbm[%d]->imb[%d]",
        unit, imb_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_IMBM__IMB,
        imbm_info,
        IMBM_IMB_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_IMB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
imbm_imb_get(int unit, uint32 imb_idx_0, imb_create_info_t *imb)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_IMBM__IMB,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_IMBM__IMB,
        sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_IMBM__IMB,
        ((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->imb,
        imb_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_IMBM__IMB,
        imb);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_IMBM__IMB,
        ((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->imb);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_IMBM__IMB,
        DNX_SW_STATE_DIAG_READ);

    *imb = ((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->imb[imb_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_IMBM__IMB,
        &((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->imb[imb_idx_0],
        "imbm[%d]->imb[%d]",
        unit, imb_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_IMBM__IMB,
        imbm_info,
        IMBM_IMB_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_IMB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
imbm_imb_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_IMBM__IMB,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_IMBM__IMB,
        sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_IMBM__IMB,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_IMBM__IMB,
        ((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->imb,
        imb_create_info_t,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][DNX_PORT_IMB_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "imbm_imb_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_IMBM__IMB,
        ((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->imb,
        "imbm[%d]->imb",
        unit,
        nof_instances_to_alloc_0 * sizeof(imb_create_info_t) / sizeof(*((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->imb));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_IMBM__IMB,
        imbm_info,
        IMBM_IMB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_IMB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
imbm_imb_type_set(int unit, uint32 imb_type_idx_0, imb_dispatch_type_t imb_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_IMBM__IMB_TYPE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_IMBM__IMB_TYPE,
        sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        imb_type_idx_0,
        SOC_MAX_NUM_PORTS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_IMBM__IMB_TYPE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_IMBM__IMB_TYPE,
        ((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->imb_type[imb_type_idx_0],
        imb_type,
        imb_dispatch_type_t,
        0,
        "imbm_imb_type_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_IMBM__IMB_TYPE,
        &imb_type,
        "imbm[%d]->imb_type[%d]",
        unit, imb_type_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_IMBM__IMB_TYPE,
        imbm_info,
        IMBM_IMB_TYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_IMB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
imbm_imb_type_get(int unit, uint32 imb_type_idx_0, imb_dispatch_type_t *imb_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_IMBM__IMB_TYPE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_IMBM__IMB_TYPE,
        sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        imb_type_idx_0,
        SOC_MAX_NUM_PORTS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_IMBM__IMB_TYPE,
        imb_type);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_IMBM__IMB_TYPE,
        DNX_SW_STATE_DIAG_READ);

    *imb_type = ((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->imb_type[imb_type_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_IMBM__IMB_TYPE,
        &((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->imb_type[imb_type_idx_0],
        "imbm[%d]->imb_type[%d]",
        unit, imb_type_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_IMBM__IMB_TYPE,
        imbm_info,
        IMBM_IMB_TYPE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_IMB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
imbm_pmd_lock_counter_set(int unit, uint32 pmd_lock_counter_idx_0, uint32 pmd_lock_counter)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_IMBM__PMD_LOCK_COUNTER,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_IMBM__PMD_LOCK_COUNTER,
        sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        pmd_lock_counter_idx_0,
        SOC_MAX_NUM_PORTS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_IMBM__PMD_LOCK_COUNTER,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_IMBM__PMD_LOCK_COUNTER,
        ((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->pmd_lock_counter[pmd_lock_counter_idx_0],
        pmd_lock_counter,
        uint32,
        0,
        "imbm_pmd_lock_counter_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_IMBM__PMD_LOCK_COUNTER,
        &pmd_lock_counter,
        "imbm[%d]->pmd_lock_counter[%d]",
        unit, pmd_lock_counter_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_IMBM__PMD_LOCK_COUNTER,
        imbm_info,
        IMBM_PMD_LOCK_COUNTER_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_IMB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
imbm_pmd_lock_counter_get(int unit, uint32 pmd_lock_counter_idx_0, uint32 *pmd_lock_counter)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_IMBM__PMD_LOCK_COUNTER,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_IMBM__PMD_LOCK_COUNTER,
        sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        pmd_lock_counter_idx_0,
        SOC_MAX_NUM_PORTS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_IMBM__PMD_LOCK_COUNTER,
        pmd_lock_counter);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_IMBM__PMD_LOCK_COUNTER,
        DNX_SW_STATE_DIAG_READ);

    *pmd_lock_counter = ((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->pmd_lock_counter[pmd_lock_counter_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_IMBM__PMD_LOCK_COUNTER,
        &((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->pmd_lock_counter[pmd_lock_counter_idx_0],
        "imbm[%d]->pmd_lock_counter[%d]",
        unit, pmd_lock_counter_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_IMBM__PMD_LOCK_COUNTER,
        imbm_info,
        IMBM_PMD_LOCK_COUNTER_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_IMB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
imbm_credit_tx_reset_mutex_create(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_IMBM__CREDIT_TX_RESET_MUTEX,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_IMBM__CREDIT_TX_RESET_MUTEX,
        sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_IMBM__CREDIT_TX_RESET_MUTEX,
        DNX_SW_STATE_DIAG_MUTEX_CREATE);

    DNX_SW_STATE_MUTEX_CREATE(
        unit,
        DNX_SW_STATE_IMBM__CREDIT_TX_RESET_MUTEX,
        ((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->credit_tx_reset_mutex,
        "((dnx_imbm_info_t*)sw_state_roots_array).credit_tx_reset_mutex");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MUTEX_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXCREATE,
        DNX_SW_STATE_IMBM__CREDIT_TX_RESET_MUTEX,
        &((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->credit_tx_reset_mutex,
        "imbm[%d]->credit_tx_reset_mutex",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_IMBM__CREDIT_TX_RESET_MUTEX,
        imbm_info,
        IMBM_CREDIT_TX_RESET_MUTEX_INFO,
        DNX_SW_STATE_DIAG_MUTEX_CREATE,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_IMB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
imbm_credit_tx_reset_mutex_is_created(int unit, uint8 *is_created)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_IMBM__CREDIT_TX_RESET_MUTEX,
        SW_STATE_FUNC_IS_CREATED,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_IMBM__CREDIT_TX_RESET_MUTEX,
        sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_IMBM__CREDIT_TX_RESET_MUTEX,
        DNX_SW_STATE_DIAG_MUTEX);

    DNX_SW_STATE_MUTEX_IS_CREATED(
        unit,
        DNX_SW_STATE_IMBM__CREDIT_TX_RESET_MUTEX,
        is_created,
        ((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->credit_tx_reset_mutex,
        "((dnx_imbm_info_t*)sw_state_roots_array).credit_tx_reset_mutex");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MUTEX_IS_CREATED_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXCREATE,
        DNX_SW_STATE_IMBM__CREDIT_TX_RESET_MUTEX,
        &((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->credit_tx_reset_mutex,
        "imbm[%d]->credit_tx_reset_mutex",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_IMBM__CREDIT_TX_RESET_MUTEX,
        imbm_info,
        IMBM_CREDIT_TX_RESET_MUTEX_INFO,
        DNX_SW_STATE_DIAG_MUTEX,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_IMB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
imbm_credit_tx_reset_mutex_take(int unit, int usec)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_IMBM__CREDIT_TX_RESET_MUTEX,
        SW_STATE_FUNC_TAKE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_IMBM__CREDIT_TX_RESET_MUTEX,
        sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_IMBM__CREDIT_TX_RESET_MUTEX,
        DNX_SW_STATE_DIAG_MUTEX);

    DNX_SW_STATE_MUTEX_TAKE(
        unit,
        DNX_SW_STATE_IMBM__CREDIT_TX_RESET_MUTEX,
        ((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->credit_tx_reset_mutex,
        usec);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MUTEX_TAKE_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXTAKE,
        DNX_SW_STATE_IMBM__CREDIT_TX_RESET_MUTEX,
        &((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->credit_tx_reset_mutex,
        "imbm[%d]->credit_tx_reset_mutex",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_IMBM__CREDIT_TX_RESET_MUTEX,
        imbm_info,
        IMBM_CREDIT_TX_RESET_MUTEX_INFO,
        DNX_SW_STATE_DIAG_MUTEX,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_IMB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
imbm_credit_tx_reset_mutex_give(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_IMBM__CREDIT_TX_RESET_MUTEX,
        SW_STATE_FUNC_GIVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_IMBM__CREDIT_TX_RESET_MUTEX,
        sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_IMBM__CREDIT_TX_RESET_MUTEX,
        DNX_SW_STATE_DIAG_MUTEX);

    DNX_SW_STATE_MUTEX_GIVE(
        unit,
        DNX_SW_STATE_IMBM__CREDIT_TX_RESET_MUTEX,
        ((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->credit_tx_reset_mutex);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MUTEX_GIVE_LOGGING,
        BSL_LS_SWSTATEDNX_MUTEXGIVE,
        DNX_SW_STATE_IMBM__CREDIT_TX_RESET_MUTEX,
        &((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->credit_tx_reset_mutex,
        "imbm[%d]->credit_tx_reset_mutex",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_IMBM__CREDIT_TX_RESET_MUTEX,
        imbm_info,
        IMBM_CREDIT_TX_RESET_MUTEX_INFO,
        DNX_SW_STATE_DIAG_MUTEX,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_IMB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}




imbm_cbs imbm = 	{
	
	imbm_is_init,
	imbm_init,
		{
		
		imbm_imbs_in_use_set,
		imbm_imbs_in_use_get}
	,
		{
		
		imbm_imb_set,
		imbm_imb_get,
		imbm_imb_alloc}
	,
		{
		
		imbm_imb_type_set,
		imbm_imb_type_get}
	,
		{
		
		imbm_pmd_lock_counter_set,
		imbm_pmd_lock_counter_get}
	,
		{
		
		imbm_credit_tx_reset_mutex_create,
		imbm_credit_tx_reset_mutex_is_created,
		imbm_credit_tx_reset_mutex_take,
		imbm_credit_tx_reset_mutex_give}
	}
;
#undef BSL_LOG_MODULE
