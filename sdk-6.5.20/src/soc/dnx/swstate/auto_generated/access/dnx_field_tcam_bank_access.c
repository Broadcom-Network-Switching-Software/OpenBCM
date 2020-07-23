
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
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_bank_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_tcam_bank_diagnostic.h>



dnx_field_tcam_bank_sw_t* dnx_field_tcam_bank_sw_dummy = NULL;



int
dnx_field_tcam_bank_sw_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID]),
        "dnx_field_tcam_bank_sw[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_bank_sw_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_field_tcam_bank_sw_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_bank_sw_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID]),
        "dnx_field_tcam_bank_sw[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        dnx_field_tcam_bank_sw_info,
        DNX_FIELD_TCAM_BANK_SW_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(dnx_field_tcam_bank_sw_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_bank_sw_tcam_banks_alloc(int unit)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks,
        dnx_field_tcam_bank_t,
        dnx_data_field.tcam.nof_banks_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_bank_sw_tcam_banks_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks,
        "dnx_field_tcam_bank_sw[%d]->tcam_banks",
        unit,
        dnx_data_field.tcam.nof_banks_get(unit) * sizeof(dnx_field_tcam_bank_t) / sizeof(*((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        dnx_field_tcam_bank_sw_info,
        DNX_FIELD_TCAM_BANK_SW_TCAM_BANKS_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.tcam.nof_banks_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[def_val_idx[0]].owner_stage,
        DNX_FIELD_TCAM_STAGE_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.tcam.nof_banks_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[def_val_idx[0]].bank_mode,
        DNX_FIELD_TCAM_BANK_MODE_NONE);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_bank_sw_tcam_banks_owner_stage_set(int unit, uint32 tcam_banks_idx_0, dnx_field_tcam_stage_e owner_stage)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks,
        tcam_banks_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].owner_stage,
        owner_stage,
        dnx_field_tcam_stage_e,
        0,
        "dnx_field_tcam_bank_sw_tcam_banks_owner_stage_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        &owner_stage,
        "dnx_field_tcam_bank_sw[%d]->tcam_banks[%d].owner_stage",
        unit, tcam_banks_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        dnx_field_tcam_bank_sw_info,
        DNX_FIELD_TCAM_BANK_SW_TCAM_BANKS_OWNER_STAGE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_bank_sw_tcam_banks_owner_stage_get(int unit, uint32 tcam_banks_idx_0, dnx_field_tcam_stage_e *owner_stage)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks,
        tcam_banks_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        owner_stage);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID);

    *owner_stage = ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].owner_stage;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        &((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].owner_stage,
        "dnx_field_tcam_bank_sw[%d]->tcam_banks[%d].owner_stage",
        unit, tcam_banks_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        dnx_field_tcam_bank_sw_info,
        DNX_FIELD_TCAM_BANK_SW_TCAM_BANKS_OWNER_STAGE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_create_empty(int unit, uint32 tcam_banks_idx_0, sw_state_ll_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        SW_STATE_FUNC_CREATE_EMPTY,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID);

    SW_STATE_LL_CREATE_EMPTY(
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].active_handlers_id,
        init_info,
        SW_STATE_LL_NO_FLAGS,
        non_existing_dummy_type_t,
        uint32,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_CREATE_EMPTY_LOGGING,
        BSL_LS_SWSTATEDNX_LLCREATE_EMPTY,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        &((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].active_handlers_id,
        "dnx_field_tcam_bank_sw[%d]->tcam_banks[%d].active_handlers_id",
        unit, tcam_banks_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        dnx_field_tcam_bank_sw_info,
        DNX_FIELD_TCAM_BANK_SW_TCAM_BANKS_ACTIVE_HANDLERS_ID_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_nof_elements(int unit, uint32 tcam_banks_idx_0, uint32 *nof_elements)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        SW_STATE_FUNC_NOF_ELEMENTS,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID);

    SW_STATE_LL_NOF_ELEMENTS(
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].active_handlers_id,
        nof_elements);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NOF_ELEMENTS_LOGGING,
        BSL_LS_SWSTATEDNX_LLNOF_ELEMENTS,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        &((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].active_handlers_id,
        "dnx_field_tcam_bank_sw[%d]->tcam_banks[%d].active_handlers_id",
        unit, tcam_banks_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        dnx_field_tcam_bank_sw_info,
        DNX_FIELD_TCAM_BANK_SW_TCAM_BANKS_ACTIVE_HANDLERS_ID_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_node_value(int unit, uint32 tcam_banks_idx_0, sw_state_ll_node_t node, uint32 *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        SW_STATE_FUNC_NODE_VALUE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID);

    SW_STATE_LL_NODE_VALUE(
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].active_handlers_id,
        node,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NODE_VALUE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNODE_VALUE,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        &((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].active_handlers_id,
        "dnx_field_tcam_bank_sw[%d]->tcam_banks[%d].active_handlers_id",
        unit, tcam_banks_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        dnx_field_tcam_bank_sw_info,
        DNX_FIELD_TCAM_BANK_SW_TCAM_BANKS_ACTIVE_HANDLERS_ID_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_node_update(int unit, uint32 tcam_banks_idx_0, sw_state_ll_node_t node, const uint32 *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        SW_STATE_FUNC_NODE_UPDATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID);

    SW_STATE_LL_NODE_UPDATE(
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].active_handlers_id,
        node,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NODE_UPDATE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNODE_UPDATE,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        &((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].active_handlers_id,
        "dnx_field_tcam_bank_sw[%d]->tcam_banks[%d].active_handlers_id",
        unit, tcam_banks_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        dnx_field_tcam_bank_sw_info,
        DNX_FIELD_TCAM_BANK_SW_TCAM_BANKS_ACTIVE_HANDLERS_ID_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_next_node(int unit, uint32 tcam_banks_idx_0, sw_state_ll_node_t node, sw_state_ll_node_t *next_node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        SW_STATE_FUNC_NEXT_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID);

    SW_STATE_LL_NEXT_NODE(
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].active_handlers_id,
        node,
        next_node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NEXT_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNEXT_NODE,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        &((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].active_handlers_id,
        "dnx_field_tcam_bank_sw[%d]->tcam_banks[%d].active_handlers_id",
        unit, tcam_banks_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        dnx_field_tcam_bank_sw_info,
        DNX_FIELD_TCAM_BANK_SW_TCAM_BANKS_ACTIVE_HANDLERS_ID_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_previous_node(int unit, uint32 tcam_banks_idx_0, sw_state_ll_node_t node, sw_state_ll_node_t *prev_node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        SW_STATE_FUNC_PREVIOUS_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID);

    SW_STATE_LL_PREVIOUS_NODE(
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].active_handlers_id,
        node,
        prev_node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_PREVIOUS_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLPREVIOUS_NODE,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        &((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].active_handlers_id,
        "dnx_field_tcam_bank_sw[%d]->tcam_banks[%d].active_handlers_id",
        unit, tcam_banks_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        dnx_field_tcam_bank_sw_info,
        DNX_FIELD_TCAM_BANK_SW_TCAM_BANKS_ACTIVE_HANDLERS_ID_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_add_first(int unit, uint32 tcam_banks_idx_0, const uint32 *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        SW_STATE_FUNC_ADD_FIRST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID);

    SW_STATE_LL_ADD_FIRST(
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].active_handlers_id,
        NULL,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_FIRST_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD_FIRST,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        &((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].active_handlers_id,
        "dnx_field_tcam_bank_sw[%d]->tcam_banks[%d].active_handlers_id",
        unit, tcam_banks_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        dnx_field_tcam_bank_sw_info,
        DNX_FIELD_TCAM_BANK_SW_TCAM_BANKS_ACTIVE_HANDLERS_ID_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_add_last(int unit, uint32 tcam_banks_idx_0, const uint32 *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        SW_STATE_FUNC_ADD_LAST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID);

    SW_STATE_LL_ADD_LAST(
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].active_handlers_id,
        NULL,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_LAST_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD_LAST,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        &((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].active_handlers_id,
        "dnx_field_tcam_bank_sw[%d]->tcam_banks[%d].active_handlers_id",
        unit, tcam_banks_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        dnx_field_tcam_bank_sw_info,
        DNX_FIELD_TCAM_BANK_SW_TCAM_BANKS_ACTIVE_HANDLERS_ID_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_remove_node(int unit, uint32 tcam_banks_idx_0, sw_state_ll_node_t node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        SW_STATE_FUNC_REMOVE_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID);

    SW_STATE_LL_REMOVE_NODE(
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].active_handlers_id,
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_REMOVE_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLREMOVE_NODE,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        &((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].active_handlers_id,
        "dnx_field_tcam_bank_sw[%d]->tcam_banks[%d].active_handlers_id",
        unit, tcam_banks_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        dnx_field_tcam_bank_sw_info,
        DNX_FIELD_TCAM_BANK_SW_TCAM_BANKS_ACTIVE_HANDLERS_ID_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_get_first(int unit, uint32 tcam_banks_idx_0, sw_state_ll_node_t *node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        SW_STATE_FUNC_GET_FIRST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID);

    SW_STATE_LL_GET_FIRST(
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].active_handlers_id,
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_GET_FIRST_LOGGING,
        BSL_LS_SWSTATEDNX_LLGET_FIRST,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        &((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].active_handlers_id,
        "dnx_field_tcam_bank_sw[%d]->tcam_banks[%d].active_handlers_id",
        unit, tcam_banks_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        dnx_field_tcam_bank_sw_info,
        DNX_FIELD_TCAM_BANK_SW_TCAM_BANKS_ACTIVE_HANDLERS_ID_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_bank_sw_tcam_banks_nof_free_entries_set(int unit, uint32 tcam_banks_idx_0, uint32 nof_free_entries_idx_0, uint32 nof_free_entries_idx_1, uint32 nof_free_entries)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks,
        tcam_banks_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].nof_free_entries);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].nof_free_entries,
        nof_free_entries_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].nof_free_entries[nof_free_entries_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].nof_free_entries[nof_free_entries_idx_0],
        nof_free_entries_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].nof_free_entries[nof_free_entries_idx_0][nof_free_entries_idx_1],
        nof_free_entries,
        uint32,
        0,
        "dnx_field_tcam_bank_sw_tcam_banks_nof_free_entries_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        &nof_free_entries,
        "dnx_field_tcam_bank_sw[%d]->tcam_banks[%d].nof_free_entries[%d][%d]",
        unit, tcam_banks_idx_0, nof_free_entries_idx_0, nof_free_entries_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        dnx_field_tcam_bank_sw_info,
        DNX_FIELD_TCAM_BANK_SW_TCAM_BANKS_NOF_FREE_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_bank_sw_tcam_banks_nof_free_entries_get(int unit, uint32 tcam_banks_idx_0, uint32 nof_free_entries_idx_0, uint32 nof_free_entries_idx_1, uint32 *nof_free_entries)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks,
        tcam_banks_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        nof_free_entries);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].nof_free_entries,
        nof_free_entries_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].nof_free_entries);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].nof_free_entries[nof_free_entries_idx_0],
        nof_free_entries_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].nof_free_entries[nof_free_entries_idx_0]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID);

    *nof_free_entries = ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].nof_free_entries[nof_free_entries_idx_0][nof_free_entries_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        &((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].nof_free_entries[nof_free_entries_idx_0][nof_free_entries_idx_1],
        "dnx_field_tcam_bank_sw[%d]->tcam_banks[%d].nof_free_entries[%d][%d]",
        unit, tcam_banks_idx_0, nof_free_entries_idx_0, nof_free_entries_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        dnx_field_tcam_bank_sw_info,
        DNX_FIELD_TCAM_BANK_SW_TCAM_BANKS_NOF_FREE_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_bank_sw_tcam_banks_nof_free_entries_alloc(int unit, uint32 tcam_banks_idx_0, uint32 nof_instances_to_alloc_0, uint32 nof_instances_to_alloc_1)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].nof_free_entries,
        uint32*,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_bank_sw_tcam_banks_nof_free_entries_alloc");

    for(uint32 nof_free_entries_idx_0 = 0;
         nof_free_entries_idx_0 < nof_instances_to_alloc_0;
         nof_free_entries_idx_0++)

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].nof_free_entries[nof_free_entries_idx_0],
        uint32,
        nof_instances_to_alloc_1,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_bank_sw_tcam_banks_nof_free_entries_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].nof_free_entries,
        "dnx_field_tcam_bank_sw[%d]->tcam_banks[%d].nof_free_entries",
        unit, tcam_banks_idx_0,
        nof_instances_to_alloc_1 * sizeof(uint32) / sizeof(*((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].nof_free_entries)+(nof_instances_to_alloc_1 * sizeof(uint32)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        dnx_field_tcam_bank_sw_info,
        DNX_FIELD_TCAM_BANK_SW_TCAM_BANKS_NOF_FREE_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_bank_sw_tcam_banks_bank_mode_set(int unit, uint32 tcam_banks_idx_0, dnx_field_tcam_bank_mode_e bank_mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks,
        tcam_banks_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].bank_mode,
        bank_mode,
        dnx_field_tcam_bank_mode_e,
        0,
        "dnx_field_tcam_bank_sw_tcam_banks_bank_mode_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        &bank_mode,
        "dnx_field_tcam_bank_sw[%d]->tcam_banks[%d].bank_mode",
        unit, tcam_banks_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        dnx_field_tcam_bank_sw_info,
        DNX_FIELD_TCAM_BANK_SW_TCAM_BANKS_BANK_MODE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_bank_sw_tcam_banks_bank_mode_get(int unit, uint32 tcam_banks_idx_0, dnx_field_tcam_bank_mode_e *bank_mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks,
        tcam_banks_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        bank_mode);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID);

    *bank_mode = ((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].bank_mode;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        &((dnx_field_tcam_bank_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID])->tcam_banks[tcam_banks_idx_0].bank_mode,
        "dnx_field_tcam_bank_sw[%d]->tcam_banks[%d].bank_mode",
        unit, tcam_banks_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_BANK_MODULE_ID,
        dnx_field_tcam_bank_sw_info,
        DNX_FIELD_TCAM_BANK_SW_TCAM_BANKS_BANK_MODE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




const char *
dnx_field_tcam_bank_mode_e_get_name(dnx_field_tcam_bank_mode_e value)
{
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_BANK_MODE_INVALID", value, DNX_FIELD_TCAM_BANK_MODE_INVALID);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_BANK_MODE_FIRST", value, DNX_FIELD_TCAM_BANK_MODE_FIRST);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_BANK_MODE_NONE", value, DNX_FIELD_TCAM_BANK_MODE_NONE);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_BANK_MODE_OPEN", value, DNX_FIELD_TCAM_BANK_MODE_OPEN);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_BANK_MODE_RESTRICTED", value, DNX_FIELD_TCAM_BANK_MODE_RESTRICTED);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_BANK_MODE_RESTRICTED_DT", value, DNX_FIELD_TCAM_BANK_MODE_RESTRICTED_DT);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_BANK_MODE_RESTRICTED_LOCATION", value, DNX_FIELD_TCAM_BANK_MODE_RESTRICTED_LOCATION);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_BANK_MODE_NOF", value, DNX_FIELD_TCAM_BANK_MODE_NOF);

    return NULL;
}





dnx_field_tcam_bank_sw_cbs dnx_field_tcam_bank_sw = 	{
	
	dnx_field_tcam_bank_sw_is_init,
	dnx_field_tcam_bank_sw_init,
		{
		
		dnx_field_tcam_bank_sw_tcam_banks_alloc,
			{
			
			dnx_field_tcam_bank_sw_tcam_banks_owner_stage_set,
			dnx_field_tcam_bank_sw_tcam_banks_owner_stage_get}
		,
			{
			
			dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_create_empty,
			dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_nof_elements,
			dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_node_value,
			dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_node_update,
			dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_next_node,
			dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_previous_node,
			dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_add_first,
			dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_add_last,
			dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_remove_node,
			dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_get_first}
		,
			{
			
			dnx_field_tcam_bank_sw_tcam_banks_nof_free_entries_set,
			dnx_field_tcam_bank_sw_tcam_banks_nof_free_entries_get,
			dnx_field_tcam_bank_sw_tcam_banks_nof_free_entries_alloc}
		,
			{
			
			dnx_field_tcam_bank_sw_tcam_banks_bank_mode_set,
			dnx_field_tcam_bank_sw_tcam_banks_bank_mode_get}
		}
	}
;
#undef BSL_LOG_MODULE
