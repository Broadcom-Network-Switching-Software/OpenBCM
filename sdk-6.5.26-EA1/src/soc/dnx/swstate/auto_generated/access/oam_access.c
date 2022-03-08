
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/oam_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/oam_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/oam_layout.h>





int
oam_sw_db_info_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_SW_STATE_OAM_SW_DB_INFO,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID]),
        "oam_sw_db_info[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        oam_sw_db_info_t,
        DNX_SW_STATE_OAM_SW_DB_INFO_NOF_PARAMS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "oam_sw_db_info_init",
        DNX_SW_STATE_DIAG_ALLOC,
        oam_init_layout_structure);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_SW_STATE_OAM_SW_DB_INFO,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID]),
        "oam_sw_db_info[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID]));

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_create_empty(int unit, sw_state_ll_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        SW_STATE_FUNC_CREATE_EMPTY,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        sw_state_roots_array[unit][OAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        DNX_SW_STATE_DIAG_LL_CREATE);

    SW_STATE_MULTIHEAD_LL_CREATE_EMPTY(
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_group_sw_db_info.oam_group_array_of_linked_lists,
        init_info,
        SW_STATE_LL_MULTIHEAD,
        non_existing_dummy_type_t,
        bcm_oam_group_t,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_CREATE_EMPTY_LOGGING,
        BSL_LS_SWSTATEDNX_LLCREATE_EMPTY,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_group_sw_db_info.oam_group_array_of_linked_lists,
        "oam_sw_db_info[%d]->oam_group_sw_db_info.oam_group_array_of_linked_lists",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_OAM_GROUP_SW_DB_INFO_OAM_GROUP_ARRAY_OF_LINKED_LISTS_INFO,
        DNX_SW_STATE_DIAG_LL_CREATE,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_nof_elements(int unit, uint32 ll_head_index, uint32 *nof_elements)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        SW_STATE_FUNC_NOF_ELEMENTS,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        sw_state_roots_array[unit][OAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_NOF_ELEMENTS(
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_group_sw_db_info.oam_group_array_of_linked_lists,
        ll_head_index,
        nof_elements);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NOF_ELEMENTS_LOGGING,
        BSL_LS_SWSTATEDNX_LLNOF_ELEMENTS,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_group_sw_db_info.oam_group_array_of_linked_lists,
        "oam_sw_db_info[%d]->oam_group_sw_db_info.oam_group_array_of_linked_lists",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_OAM_GROUP_SW_DB_INFO_OAM_GROUP_ARRAY_OF_LINKED_LISTS_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_node_value(int unit, sw_state_ll_node_t node, bcm_oam_group_t *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        SW_STATE_FUNC_NODE_VALUE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        sw_state_roots_array[unit][OAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_NODE_VALUE(
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_group_sw_db_info.oam_group_array_of_linked_lists,
        node,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NODE_VALUE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNODE_VALUE,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_group_sw_db_info.oam_group_array_of_linked_lists,
        "oam_sw_db_info[%d]->oam_group_sw_db_info.oam_group_array_of_linked_lists",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_OAM_GROUP_SW_DB_INFO_OAM_GROUP_ARRAY_OF_LINKED_LISTS_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_node_update(int unit, sw_state_ll_node_t node, const bcm_oam_group_t *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        SW_STATE_FUNC_NODE_UPDATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        sw_state_roots_array[unit][OAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_NODE_UPDATE(
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_group_sw_db_info.oam_group_array_of_linked_lists,
        node,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NODE_UPDATE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNODE_UPDATE,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_group_sw_db_info.oam_group_array_of_linked_lists,
        "oam_sw_db_info[%d]->oam_group_sw_db_info.oam_group_array_of_linked_lists",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_OAM_GROUP_SW_DB_INFO_OAM_GROUP_ARRAY_OF_LINKED_LISTS_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_next_node(int unit, uint32 ll_head_index, sw_state_ll_node_t node, sw_state_ll_node_t *next_node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        SW_STATE_FUNC_NEXT_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        sw_state_roots_array[unit][OAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_NEXT_NODE(
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_group_sw_db_info.oam_group_array_of_linked_lists,
        ll_head_index,
        node,
        next_node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NEXT_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNEXT_NODE,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_group_sw_db_info.oam_group_array_of_linked_lists,
        "oam_sw_db_info[%d]->oam_group_sw_db_info.oam_group_array_of_linked_lists",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_OAM_GROUP_SW_DB_INFO_OAM_GROUP_ARRAY_OF_LINKED_LISTS_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_previous_node(int unit, uint32 ll_head_index, sw_state_ll_node_t node, sw_state_ll_node_t *prev_node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        SW_STATE_FUNC_PREVIOUS_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        sw_state_roots_array[unit][OAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_PREVIOUS_NODE(
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_group_sw_db_info.oam_group_array_of_linked_lists,
        ll_head_index,
        node,
        prev_node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_PREVIOUS_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLPREVIOUS_NODE,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_group_sw_db_info.oam_group_array_of_linked_lists,
        "oam_sw_db_info[%d]->oam_group_sw_db_info.oam_group_array_of_linked_lists",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_OAM_GROUP_SW_DB_INFO_OAM_GROUP_ARRAY_OF_LINKED_LISTS_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_add_first(int unit, uint32 ll_head_index, const bcm_oam_group_t *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        SW_STATE_FUNC_ADD_FIRST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        sw_state_roots_array[unit][OAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_ADD_FIRST(
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_group_sw_db_info.oam_group_array_of_linked_lists,
        ll_head_index,
        NULL,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_FIRST_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD_FIRST,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_group_sw_db_info.oam_group_array_of_linked_lists,
        "oam_sw_db_info[%d]->oam_group_sw_db_info.oam_group_array_of_linked_lists",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_OAM_GROUP_SW_DB_INFO_OAM_GROUP_ARRAY_OF_LINKED_LISTS_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_add_last(int unit, uint32 ll_head_index, const bcm_oam_group_t *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        SW_STATE_FUNC_ADD_LAST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        sw_state_roots_array[unit][OAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_ADD_LAST(
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_group_sw_db_info.oam_group_array_of_linked_lists,
        ll_head_index,
        NULL,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_LAST_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD_LAST,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_group_sw_db_info.oam_group_array_of_linked_lists,
        "oam_sw_db_info[%d]->oam_group_sw_db_info.oam_group_array_of_linked_lists",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_OAM_GROUP_SW_DB_INFO_OAM_GROUP_ARRAY_OF_LINKED_LISTS_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_remove_node(int unit, uint32 ll_head_index, sw_state_ll_node_t node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        SW_STATE_FUNC_REMOVE_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        sw_state_roots_array[unit][OAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_REMOVE_NODE(
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_group_sw_db_info.oam_group_array_of_linked_lists,
        ll_head_index,
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_REMOVE_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLREMOVE_NODE,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_group_sw_db_info.oam_group_array_of_linked_lists,
        "oam_sw_db_info[%d]->oam_group_sw_db_info.oam_group_array_of_linked_lists",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_OAM_GROUP_SW_DB_INFO_OAM_GROUP_ARRAY_OF_LINKED_LISTS_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_get_first(int unit, uint32 ll_head_index, sw_state_ll_node_t *node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        SW_STATE_FUNC_GET_FIRST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        sw_state_roots_array[unit][OAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_GET_FIRST(
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_group_sw_db_info.oam_group_array_of_linked_lists,
        ll_head_index,
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_GET_FIRST_LOGGING,
        BSL_LS_SWSTATEDNX_LLGET_FIRST,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_group_sw_db_info.oam_group_array_of_linked_lists,
        "oam_sw_db_info[%d]->oam_group_sw_db_info.oam_group_array_of_linked_lists",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_OAM_GROUP_SW_DB_INFO_OAM_GROUP_ARRAY_OF_LINKED_LISTS_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_create_empty(int unit, sw_state_ll_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        SW_STATE_FUNC_CREATE_EMPTY,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        sw_state_roots_array[unit][OAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        DNX_SW_STATE_DIAG_LL_CREATE);

    SW_STATE_MULTIHEAD_LL_CREATE_EMPTY(
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists,
        init_info,
        SW_STATE_LL_MULTIHEAD,
        non_existing_dummy_type_t,
        bcm_oam_endpoint_t,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_CREATE_EMPTY_LOGGING,
        BSL_LS_SWSTATEDNX_LLCREATE_EMPTY,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists,
        "oam_sw_db_info[%d]->oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_OAM_ENDPOINT_SW_DB_INFO_OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS_INFO,
        DNX_SW_STATE_DIAG_LL_CREATE,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_nof_elements(int unit, uint32 ll_head_index, uint32 *nof_elements)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        SW_STATE_FUNC_NOF_ELEMENTS,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        sw_state_roots_array[unit][OAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_NOF_ELEMENTS(
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists,
        ll_head_index,
        nof_elements);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NOF_ELEMENTS_LOGGING,
        BSL_LS_SWSTATEDNX_LLNOF_ELEMENTS,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists,
        "oam_sw_db_info[%d]->oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_OAM_ENDPOINT_SW_DB_INFO_OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_node_value(int unit, sw_state_ll_node_t node, bcm_oam_endpoint_t *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        SW_STATE_FUNC_NODE_VALUE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        sw_state_roots_array[unit][OAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_NODE_VALUE(
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists,
        node,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NODE_VALUE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNODE_VALUE,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists,
        "oam_sw_db_info[%d]->oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_OAM_ENDPOINT_SW_DB_INFO_OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_node_update(int unit, sw_state_ll_node_t node, const bcm_oam_endpoint_t *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        SW_STATE_FUNC_NODE_UPDATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        sw_state_roots_array[unit][OAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_NODE_UPDATE(
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists,
        node,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NODE_UPDATE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNODE_UPDATE,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists,
        "oam_sw_db_info[%d]->oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_OAM_ENDPOINT_SW_DB_INFO_OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_next_node(int unit, uint32 ll_head_index, sw_state_ll_node_t node, sw_state_ll_node_t *next_node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        SW_STATE_FUNC_NEXT_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        sw_state_roots_array[unit][OAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_NEXT_NODE(
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists,
        ll_head_index,
        node,
        next_node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NEXT_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNEXT_NODE,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists,
        "oam_sw_db_info[%d]->oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_OAM_ENDPOINT_SW_DB_INFO_OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_previous_node(int unit, uint32 ll_head_index, sw_state_ll_node_t node, sw_state_ll_node_t *prev_node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        SW_STATE_FUNC_PREVIOUS_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        sw_state_roots_array[unit][OAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_PREVIOUS_NODE(
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists,
        ll_head_index,
        node,
        prev_node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_PREVIOUS_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLPREVIOUS_NODE,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists,
        "oam_sw_db_info[%d]->oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_OAM_ENDPOINT_SW_DB_INFO_OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_add_first(int unit, uint32 ll_head_index, const bcm_oam_endpoint_t *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        SW_STATE_FUNC_ADD_FIRST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        sw_state_roots_array[unit][OAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_ADD_FIRST(
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists,
        ll_head_index,
        NULL,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_FIRST_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD_FIRST,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists,
        "oam_sw_db_info[%d]->oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_OAM_ENDPOINT_SW_DB_INFO_OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_add_last(int unit, uint32 ll_head_index, const bcm_oam_endpoint_t *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        SW_STATE_FUNC_ADD_LAST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        sw_state_roots_array[unit][OAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_ADD_LAST(
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists,
        ll_head_index,
        NULL,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_LAST_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD_LAST,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists,
        "oam_sw_db_info[%d]->oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_OAM_ENDPOINT_SW_DB_INFO_OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_remove_node(int unit, uint32 ll_head_index, sw_state_ll_node_t node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        SW_STATE_FUNC_REMOVE_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        sw_state_roots_array[unit][OAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_REMOVE_NODE(
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists,
        ll_head_index,
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_REMOVE_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLREMOVE_NODE,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists,
        "oam_sw_db_info[%d]->oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_OAM_ENDPOINT_SW_DB_INFO_OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_get_first(int unit, uint32 ll_head_index, sw_state_ll_node_t *node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        SW_STATE_FUNC_GET_FIRST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        sw_state_roots_array[unit][OAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_GET_FIRST(
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists,
        ll_head_index,
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_GET_FIRST_LOGGING,
        BSL_LS_SWSTATEDNX_LLGET_FIRST,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists,
        "oam_sw_db_info[%d]->oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_OAM_ENDPOINT_SW_DB_INFO_OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_mep_linked_lists_create_empty(int unit, sw_state_ll_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        SW_STATE_FUNC_CREATE_EMPTY,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        sw_state_roots_array[unit][OAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        DNX_SW_STATE_DIAG_LL_CREATE);

    SW_STATE_MULTIHEAD_LL_CREATE_EMPTY(
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_mep_linked_lists,
        init_info,
        SW_STATE_LL_MULTIHEAD,
        non_existing_dummy_type_t,
        bcm_oam_endpoint_t,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_CREATE_EMPTY_LOGGING,
        BSL_LS_SWSTATEDNX_LLCREATE_EMPTY,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_mep_linked_lists,
        "oam_sw_db_info[%d]->oam_endpoint_sw_db_info.oam_endpoint_array_of_mep_linked_lists",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_OAM_ENDPOINT_SW_DB_INFO_OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS_INFO,
        DNX_SW_STATE_DIAG_LL_CREATE,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_mep_linked_lists_nof_elements(int unit, uint32 ll_head_index, uint32 *nof_elements)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        SW_STATE_FUNC_NOF_ELEMENTS,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        sw_state_roots_array[unit][OAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_NOF_ELEMENTS(
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_mep_linked_lists,
        ll_head_index,
        nof_elements);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NOF_ELEMENTS_LOGGING,
        BSL_LS_SWSTATEDNX_LLNOF_ELEMENTS,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_mep_linked_lists,
        "oam_sw_db_info[%d]->oam_endpoint_sw_db_info.oam_endpoint_array_of_mep_linked_lists",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_OAM_ENDPOINT_SW_DB_INFO_OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_mep_linked_lists_node_value(int unit, sw_state_ll_node_t node, bcm_oam_endpoint_t *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        SW_STATE_FUNC_NODE_VALUE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        sw_state_roots_array[unit][OAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_NODE_VALUE(
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_mep_linked_lists,
        node,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NODE_VALUE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNODE_VALUE,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_mep_linked_lists,
        "oam_sw_db_info[%d]->oam_endpoint_sw_db_info.oam_endpoint_array_of_mep_linked_lists",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_OAM_ENDPOINT_SW_DB_INFO_OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_mep_linked_lists_node_update(int unit, sw_state_ll_node_t node, const bcm_oam_endpoint_t *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        SW_STATE_FUNC_NODE_UPDATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        sw_state_roots_array[unit][OAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_NODE_UPDATE(
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_mep_linked_lists,
        node,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NODE_UPDATE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNODE_UPDATE,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_mep_linked_lists,
        "oam_sw_db_info[%d]->oam_endpoint_sw_db_info.oam_endpoint_array_of_mep_linked_lists",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_OAM_ENDPOINT_SW_DB_INFO_OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_mep_linked_lists_next_node(int unit, uint32 ll_head_index, sw_state_ll_node_t node, sw_state_ll_node_t *next_node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        SW_STATE_FUNC_NEXT_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        sw_state_roots_array[unit][OAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_NEXT_NODE(
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_mep_linked_lists,
        ll_head_index,
        node,
        next_node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NEXT_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNEXT_NODE,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_mep_linked_lists,
        "oam_sw_db_info[%d]->oam_endpoint_sw_db_info.oam_endpoint_array_of_mep_linked_lists",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_OAM_ENDPOINT_SW_DB_INFO_OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_mep_linked_lists_previous_node(int unit, uint32 ll_head_index, sw_state_ll_node_t node, sw_state_ll_node_t *prev_node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        SW_STATE_FUNC_PREVIOUS_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        sw_state_roots_array[unit][OAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_PREVIOUS_NODE(
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_mep_linked_lists,
        ll_head_index,
        node,
        prev_node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_PREVIOUS_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLPREVIOUS_NODE,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_mep_linked_lists,
        "oam_sw_db_info[%d]->oam_endpoint_sw_db_info.oam_endpoint_array_of_mep_linked_lists",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_OAM_ENDPOINT_SW_DB_INFO_OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_mep_linked_lists_add_first(int unit, uint32 ll_head_index, const bcm_oam_endpoint_t *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        SW_STATE_FUNC_ADD_FIRST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        sw_state_roots_array[unit][OAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_ADD_FIRST(
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_mep_linked_lists,
        ll_head_index,
        NULL,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_FIRST_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD_FIRST,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_mep_linked_lists,
        "oam_sw_db_info[%d]->oam_endpoint_sw_db_info.oam_endpoint_array_of_mep_linked_lists",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_OAM_ENDPOINT_SW_DB_INFO_OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_mep_linked_lists_add_last(int unit, uint32 ll_head_index, const bcm_oam_endpoint_t *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        SW_STATE_FUNC_ADD_LAST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        sw_state_roots_array[unit][OAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_ADD_LAST(
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_mep_linked_lists,
        ll_head_index,
        NULL,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_LAST_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD_LAST,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_mep_linked_lists,
        "oam_sw_db_info[%d]->oam_endpoint_sw_db_info.oam_endpoint_array_of_mep_linked_lists",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_OAM_ENDPOINT_SW_DB_INFO_OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_mep_linked_lists_remove_node(int unit, uint32 ll_head_index, sw_state_ll_node_t node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        SW_STATE_FUNC_REMOVE_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        sw_state_roots_array[unit][OAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_REMOVE_NODE(
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_mep_linked_lists,
        ll_head_index,
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_REMOVE_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLREMOVE_NODE,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_mep_linked_lists,
        "oam_sw_db_info[%d]->oam_endpoint_sw_db_info.oam_endpoint_array_of_mep_linked_lists",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_OAM_ENDPOINT_SW_DB_INFO_OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_mep_linked_lists_get_first(int unit, uint32 ll_head_index, sw_state_ll_node_t *node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        SW_STATE_FUNC_GET_FIRST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        sw_state_roots_array[unit][OAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        DNX_SW_STATE_DIAG_LL);

    SW_STATE_MULTIHEAD_LL_GET_FIRST(
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_mep_linked_lists,
        ll_head_index,
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_GET_FIRST_LOGGING,
        BSL_LS_SWSTATEDNX_LLGET_FIRST,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_mep_linked_lists,
        "oam_sw_db_info[%d]->oam_endpoint_sw_db_info.oam_endpoint_array_of_mep_linked_lists",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_OAM_ENDPOINT_SW_DB_INFO_OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_reflector_encap_id_set(int unit, int encap_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__REFLECTOR__ENCAP_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__REFLECTOR__ENCAP_ID,
        sw_state_roots_array[unit][OAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__REFLECTOR__ENCAP_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__REFLECTOR__ENCAP_ID,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->reflector.encap_id,
        encap_id,
        int,
        0,
        "oam_sw_db_info_reflector_encap_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_OAM_SW_DB_INFO__REFLECTOR__ENCAP_ID,
        &encap_id,
        "oam_sw_db_info[%d]->reflector.encap_id",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__REFLECTOR__ENCAP_ID,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_REFLECTOR_ENCAP_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_reflector_encap_id_get(int unit, int *encap_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__REFLECTOR__ENCAP_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__REFLECTOR__ENCAP_ID,
        sw_state_roots_array[unit][OAM_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__REFLECTOR__ENCAP_ID,
        encap_id);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__REFLECTOR__ENCAP_ID,
        DNX_SW_STATE_DIAG_READ);

    *encap_id = ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->reflector.encap_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_OAM_SW_DB_INFO__REFLECTOR__ENCAP_ID,
        &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->reflector.encap_id,
        "oam_sw_db_info[%d]->reflector.encap_id",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__REFLECTOR__ENCAP_ID,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_REFLECTOR_ENCAP_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_reflector_is_allocated_set(int unit, int is_allocated)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__REFLECTOR__IS_ALLOCATED,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__REFLECTOR__IS_ALLOCATED,
        sw_state_roots_array[unit][OAM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__REFLECTOR__IS_ALLOCATED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__REFLECTOR__IS_ALLOCATED,
        ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->reflector.is_allocated,
        is_allocated,
        int,
        0,
        "oam_sw_db_info_reflector_is_allocated_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_OAM_SW_DB_INFO__REFLECTOR__IS_ALLOCATED,
        &is_allocated,
        "oam_sw_db_info[%d]->reflector.is_allocated",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__REFLECTOR__IS_ALLOCATED,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_REFLECTOR_IS_ALLOCATED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_reflector_is_allocated_get(int unit, int *is_allocated)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__REFLECTOR__IS_ALLOCATED,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__REFLECTOR__IS_ALLOCATED,
        sw_state_roots_array[unit][OAM_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__REFLECTOR__IS_ALLOCATED,
        is_allocated);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__REFLECTOR__IS_ALLOCATED,
        DNX_SW_STATE_DIAG_READ);

    *is_allocated = ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->reflector.is_allocated;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_OAM_SW_DB_INFO__REFLECTOR__IS_ALLOCATED,
        &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->reflector.is_allocated,
        "oam_sw_db_info[%d]->reflector.is_allocated",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_OAM_SW_DB_INFO__REFLECTOR__IS_ALLOCATED,
        oam_sw_db_info_info,
        OAM_SW_DB_INFO_REFLECTOR_IS_ALLOCATED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}





oam_sw_db_info_cbs oam_sw_db_info = 	{
	
	oam_sw_db_info_is_init,
	oam_sw_db_info_init,
		{
		
			{
			
			oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_create_empty,
			oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_nof_elements,
			oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_node_value,
			oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_node_update,
			oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_next_node,
			oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_previous_node,
			oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_add_first,
			oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_add_last,
			oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_remove_node,
			oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_get_first}
		}
	,
		{
		
			{
			
			oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_create_empty,
			oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_nof_elements,
			oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_node_value,
			oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_node_update,
			oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_next_node,
			oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_previous_node,
			oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_add_first,
			oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_add_last,
			oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_remove_node,
			oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_get_first}
		,
			{
			
			oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_mep_linked_lists_create_empty,
			oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_mep_linked_lists_nof_elements,
			oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_mep_linked_lists_node_value,
			oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_mep_linked_lists_node_update,
			oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_mep_linked_lists_next_node,
			oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_mep_linked_lists_previous_node,
			oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_mep_linked_lists_add_first,
			oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_mep_linked_lists_add_last,
			oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_mep_linked_lists_remove_node,
			oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_mep_linked_lists_get_first}
		}
	,
		{
		
			{
			
			oam_sw_db_info_reflector_encap_id_set,
			oam_sw_db_info_reflector_encap_id_get}
		,
			{
			
			oam_sw_db_info_reflector_is_allocated_set,
			oam_sw_db_info_reflector_is_allocated_get}
		}
	}
;
#undef BSL_LOG_MODULE
