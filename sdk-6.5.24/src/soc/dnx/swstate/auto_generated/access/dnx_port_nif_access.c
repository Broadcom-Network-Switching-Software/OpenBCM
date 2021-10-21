
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/dnx_port_nif_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_port_nif_diagnostic.h>





int
dnx_port_nif_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]),
        "dnx_port_nif_db[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_port_nif_db_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_port_nif_db_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]),
        "dnx_port_nif_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(dnx_port_nif_db_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_arb_link_list_list_info_head_set(int unit, int head)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.list_info.head,
        head,
        int,
        0,
        "dnx_port_nif_db_arb_link_list_list_info_head_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_PORT_NIF_MODULE_ID,
        &head,
        "dnx_port_nif_db[%d]->arb_link_list.list_info.head",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ARB_LINK_LIST_LIST_INFO_HEAD_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_arb_link_list_list_info_head_get(int unit, int *head)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        head);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    *head = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.list_info.head;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_PORT_NIF_MODULE_ID,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.list_info.head,
        "dnx_port_nif_db[%d]->arb_link_list.list_info.head",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ARB_LINK_LIST_LIST_INFO_HEAD_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_arb_link_list_list_info_tail_set(int unit, int tail)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.list_info.tail,
        tail,
        int,
        0,
        "dnx_port_nif_db_arb_link_list_list_info_tail_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_PORT_NIF_MODULE_ID,
        &tail,
        "dnx_port_nif_db[%d]->arb_link_list.list_info.tail",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ARB_LINK_LIST_LIST_INFO_TAIL_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_arb_link_list_list_info_tail_get(int unit, int *tail)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        tail);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    *tail = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.list_info.tail;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_PORT_NIF_MODULE_ID,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.list_info.tail,
        "dnx_port_nif_db[%d]->arb_link_list.list_info.tail",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ARB_LINK_LIST_LIST_INFO_TAIL_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_arb_link_list_list_info_size_set(int unit, int size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.list_info.size,
        size,
        int,
        0,
        "dnx_port_nif_db_arb_link_list_list_info_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_PORT_NIF_MODULE_ID,
        &size,
        "dnx_port_nif_db[%d]->arb_link_list.list_info.size",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ARB_LINK_LIST_LIST_INFO_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_arb_link_list_list_info_size_get(int unit, int *size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        size);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    *size = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.list_info.size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_PORT_NIF_MODULE_ID,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.list_info.size,
        "dnx_port_nif_db[%d]->arb_link_list.list_info.size",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ARB_LINK_LIST_LIST_INFO_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_arb_link_list_linking_info_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.linking_info,
        dnx_port_nif_db_linking_info_t,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_port_nif_db_arb_link_list_linking_info_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.linking_info,
        "dnx_port_nif_db[%d]->arb_link_list.linking_info",
        unit,
        nof_instances_to_alloc_0 * sizeof(dnx_port_nif_db_linking_info_t) / sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.linking_info));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ARB_LINK_LIST_LINKING_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_arb_link_list_linking_info_next_section_set(int unit, uint32 linking_info_idx_0, int next_section)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.linking_info,
        linking_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.linking_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.linking_info[linking_info_idx_0].next_section,
        next_section,
        int,
        0,
        "dnx_port_nif_db_arb_link_list_linking_info_next_section_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_PORT_NIF_MODULE_ID,
        &next_section,
        "dnx_port_nif_db[%d]->arb_link_list.linking_info[%d].next_section",
        unit, linking_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ARB_LINK_LIST_LINKING_INFO_NEXT_SECTION_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_arb_link_list_linking_info_next_section_get(int unit, uint32 linking_info_idx_0, int *next_section)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.linking_info,
        linking_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        next_section);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.linking_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    *next_section = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.linking_info[linking_info_idx_0].next_section;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_PORT_NIF_MODULE_ID,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.linking_info[linking_info_idx_0].next_section,
        "dnx_port_nif_db[%d]->arb_link_list.linking_info[%d].next_section",
        unit, linking_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ARB_LINK_LIST_LINKING_INFO_NEXT_SECTION_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_arb_link_list_is_client_allocated_set(int unit, uint32 is_client_allocated_idx_0, uint8 is_client_allocated)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.is_client_allocated,
        is_client_allocated_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.is_client_allocated);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.is_client_allocated[is_client_allocated_idx_0],
        is_client_allocated,
        uint8,
        0,
        "dnx_port_nif_db_arb_link_list_is_client_allocated_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_PORT_NIF_MODULE_ID,
        &is_client_allocated,
        "dnx_port_nif_db[%d]->arb_link_list.is_client_allocated[%d]",
        unit, is_client_allocated_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ARB_LINK_LIST_IS_CLIENT_ALLOCATED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_arb_link_list_is_client_allocated_get(int unit, uint32 is_client_allocated_idx_0, uint8 *is_client_allocated)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.is_client_allocated,
        is_client_allocated_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        is_client_allocated);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.is_client_allocated);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    *is_client_allocated = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.is_client_allocated[is_client_allocated_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_PORT_NIF_MODULE_ID,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.is_client_allocated[is_client_allocated_idx_0],
        "dnx_port_nif_db[%d]->arb_link_list.is_client_allocated[%d]",
        unit, is_client_allocated_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ARB_LINK_LIST_IS_CLIENT_ALLOCATED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_arb_link_list_is_client_allocated_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.is_client_allocated,
        uint8,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_port_nif_db_arb_link_list_is_client_allocated_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.is_client_allocated,
        "dnx_port_nif_db[%d]->arb_link_list.is_client_allocated",
        unit,
        nof_instances_to_alloc_0 * sizeof(uint8) / sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.is_client_allocated));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ARB_LINK_LIST_IS_CLIENT_ALLOCATED_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_oft_link_list_list_info_head_set(int unit, int head)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.list_info.head,
        head,
        int,
        0,
        "dnx_port_nif_db_oft_link_list_list_info_head_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_PORT_NIF_MODULE_ID,
        &head,
        "dnx_port_nif_db[%d]->oft_link_list.list_info.head",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFT_LINK_LIST_LIST_INFO_HEAD_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_oft_link_list_list_info_head_get(int unit, int *head)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        head);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    *head = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.list_info.head;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_PORT_NIF_MODULE_ID,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.list_info.head,
        "dnx_port_nif_db[%d]->oft_link_list.list_info.head",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFT_LINK_LIST_LIST_INFO_HEAD_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_oft_link_list_list_info_tail_set(int unit, int tail)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.list_info.tail,
        tail,
        int,
        0,
        "dnx_port_nif_db_oft_link_list_list_info_tail_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_PORT_NIF_MODULE_ID,
        &tail,
        "dnx_port_nif_db[%d]->oft_link_list.list_info.tail",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFT_LINK_LIST_LIST_INFO_TAIL_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_oft_link_list_list_info_tail_get(int unit, int *tail)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        tail);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    *tail = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.list_info.tail;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_PORT_NIF_MODULE_ID,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.list_info.tail,
        "dnx_port_nif_db[%d]->oft_link_list.list_info.tail",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFT_LINK_LIST_LIST_INFO_TAIL_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_oft_link_list_list_info_size_set(int unit, int size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.list_info.size,
        size,
        int,
        0,
        "dnx_port_nif_db_oft_link_list_list_info_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_PORT_NIF_MODULE_ID,
        &size,
        "dnx_port_nif_db[%d]->oft_link_list.list_info.size",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFT_LINK_LIST_LIST_INFO_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_oft_link_list_list_info_size_get(int unit, int *size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        size);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    *size = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.list_info.size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_PORT_NIF_MODULE_ID,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.list_info.size,
        "dnx_port_nif_db[%d]->oft_link_list.list_info.size",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFT_LINK_LIST_LIST_INFO_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_oft_link_list_linking_info_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.linking_info,
        dnx_port_nif_db_linking_info_t,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_port_nif_db_oft_link_list_linking_info_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.linking_info,
        "dnx_port_nif_db[%d]->oft_link_list.linking_info",
        unit,
        nof_instances_to_alloc_0 * sizeof(dnx_port_nif_db_linking_info_t) / sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.linking_info));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFT_LINK_LIST_LINKING_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_oft_link_list_linking_info_next_section_set(int unit, uint32 linking_info_idx_0, int next_section)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.linking_info,
        linking_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.linking_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.linking_info[linking_info_idx_0].next_section,
        next_section,
        int,
        0,
        "dnx_port_nif_db_oft_link_list_linking_info_next_section_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_PORT_NIF_MODULE_ID,
        &next_section,
        "dnx_port_nif_db[%d]->oft_link_list.linking_info[%d].next_section",
        unit, linking_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFT_LINK_LIST_LINKING_INFO_NEXT_SECTION_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_oft_link_list_linking_info_next_section_get(int unit, uint32 linking_info_idx_0, int *next_section)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.linking_info,
        linking_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        next_section);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.linking_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    *next_section = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.linking_info[linking_info_idx_0].next_section;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_PORT_NIF_MODULE_ID,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.linking_info[linking_info_idx_0].next_section,
        "dnx_port_nif_db[%d]->oft_link_list.linking_info[%d].next_section",
        unit, linking_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFT_LINK_LIST_LINKING_INFO_NEXT_SECTION_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_oft_link_list_is_client_allocated_set(int unit, uint32 is_client_allocated_idx_0, uint8 is_client_allocated)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.is_client_allocated,
        is_client_allocated_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.is_client_allocated);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.is_client_allocated[is_client_allocated_idx_0],
        is_client_allocated,
        uint8,
        0,
        "dnx_port_nif_db_oft_link_list_is_client_allocated_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_PORT_NIF_MODULE_ID,
        &is_client_allocated,
        "dnx_port_nif_db[%d]->oft_link_list.is_client_allocated[%d]",
        unit, is_client_allocated_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFT_LINK_LIST_IS_CLIENT_ALLOCATED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_oft_link_list_is_client_allocated_get(int unit, uint32 is_client_allocated_idx_0, uint8 *is_client_allocated)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.is_client_allocated,
        is_client_allocated_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        is_client_allocated);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.is_client_allocated);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    *is_client_allocated = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.is_client_allocated[is_client_allocated_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_PORT_NIF_MODULE_ID,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.is_client_allocated[is_client_allocated_idx_0],
        "dnx_port_nif_db[%d]->oft_link_list.is_client_allocated[%d]",
        unit, is_client_allocated_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFT_LINK_LIST_IS_CLIENT_ALLOCATED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_oft_link_list_is_client_allocated_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.is_client_allocated,
        uint8,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_port_nif_db_oft_link_list_is_client_allocated_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.is_client_allocated,
        "dnx_port_nif_db[%d]->oft_link_list.is_client_allocated",
        unit,
        nof_instances_to_alloc_0 * sizeof(uint8) / sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.is_client_allocated));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFT_LINK_LIST_IS_CLIENT_ALLOCATED_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_ofr_link_list_list_info_head_set(int unit, int head)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info.head,
        head,
        int,
        0,
        "dnx_port_nif_db_ofr_link_list_list_info_head_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_PORT_NIF_MODULE_ID,
        &head,
        "dnx_port_nif_db[%d]->ofr_link_list.list_info.head",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFR_LINK_LIST_LIST_INFO_HEAD_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_ofr_link_list_list_info_head_get(int unit, int *head)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        head);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    *head = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info.head;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_PORT_NIF_MODULE_ID,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info.head,
        "dnx_port_nif_db[%d]->ofr_link_list.list_info.head",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFR_LINK_LIST_LIST_INFO_HEAD_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_ofr_link_list_list_info_tail_set(int unit, int tail)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info.tail,
        tail,
        int,
        0,
        "dnx_port_nif_db_ofr_link_list_list_info_tail_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_PORT_NIF_MODULE_ID,
        &tail,
        "dnx_port_nif_db[%d]->ofr_link_list.list_info.tail",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFR_LINK_LIST_LIST_INFO_TAIL_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_ofr_link_list_list_info_tail_get(int unit, int *tail)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        tail);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    *tail = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info.tail;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_PORT_NIF_MODULE_ID,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info.tail,
        "dnx_port_nif_db[%d]->ofr_link_list.list_info.tail",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFR_LINK_LIST_LIST_INFO_TAIL_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_ofr_link_list_list_info_size_set(int unit, int size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info.size,
        size,
        int,
        0,
        "dnx_port_nif_db_ofr_link_list_list_info_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_PORT_NIF_MODULE_ID,
        &size,
        "dnx_port_nif_db[%d]->ofr_link_list.list_info.size",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFR_LINK_LIST_LIST_INFO_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_ofr_link_list_list_info_size_get(int unit, int *size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        size);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    *size = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info.size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_PORT_NIF_MODULE_ID,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info.size,
        "dnx_port_nif_db[%d]->ofr_link_list.list_info.size",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFR_LINK_LIST_LIST_INFO_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_ofr_link_list_linking_info_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info,
        dnx_port_nif_db_linking_info_t,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_port_nif_db_ofr_link_list_linking_info_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info,
        "dnx_port_nif_db[%d]->ofr_link_list.linking_info",
        unit,
        nof_instances_to_alloc_0 * sizeof(dnx_port_nif_db_linking_info_t) / sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFR_LINK_LIST_LINKING_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_ofr_link_list_linking_info_next_section_set(int unit, uint32 linking_info_idx_0, int next_section)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info,
        linking_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info[linking_info_idx_0].next_section,
        next_section,
        int,
        0,
        "dnx_port_nif_db_ofr_link_list_linking_info_next_section_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_PORT_NIF_MODULE_ID,
        &next_section,
        "dnx_port_nif_db[%d]->ofr_link_list.linking_info[%d].next_section",
        unit, linking_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFR_LINK_LIST_LINKING_INFO_NEXT_SECTION_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_ofr_link_list_linking_info_next_section_get(int unit, uint32 linking_info_idx_0, int *next_section)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info,
        linking_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        next_section);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    *next_section = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info[linking_info_idx_0].next_section;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_PORT_NIF_MODULE_ID,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info[linking_info_idx_0].next_section,
        "dnx_port_nif_db[%d]->ofr_link_list.linking_info[%d].next_section",
        unit, linking_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFR_LINK_LIST_LINKING_INFO_NEXT_SECTION_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_ofr_link_list_is_client_allocated_set(int unit, uint32 is_client_allocated_idx_0, uint8 is_client_allocated)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.is_client_allocated,
        is_client_allocated_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.is_client_allocated);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.is_client_allocated[is_client_allocated_idx_0],
        is_client_allocated,
        uint8,
        0,
        "dnx_port_nif_db_ofr_link_list_is_client_allocated_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_PORT_NIF_MODULE_ID,
        &is_client_allocated,
        "dnx_port_nif_db[%d]->ofr_link_list.is_client_allocated[%d]",
        unit, is_client_allocated_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFR_LINK_LIST_IS_CLIENT_ALLOCATED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_ofr_link_list_is_client_allocated_get(int unit, uint32 is_client_allocated_idx_0, uint8 *is_client_allocated)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.is_client_allocated,
        is_client_allocated_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        is_client_allocated);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.is_client_allocated);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    *is_client_allocated = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.is_client_allocated[is_client_allocated_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_PORT_NIF_MODULE_ID,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.is_client_allocated[is_client_allocated_idx_0],
        "dnx_port_nif_db[%d]->ofr_link_list.is_client_allocated[%d]",
        unit, is_client_allocated_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFR_LINK_LIST_IS_CLIENT_ALLOCATED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_port_nif_db_ofr_link_list_is_client_allocated_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_PORT_NIF_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.is_client_allocated,
        uint8,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_port_nif_db_ofr_link_list_is_client_allocated_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_PORT_NIF_MODULE_ID,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.is_client_allocated,
        "dnx_port_nif_db[%d]->ofr_link_list.is_client_allocated",
        unit,
        nof_instances_to_alloc_0 * sizeof(uint8) / sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.is_client_allocated));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_PORT_NIF_MODULE_ID,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFR_LINK_LIST_IS_CLIENT_ALLOCATED_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_port_nif_db_cbs dnx_port_nif_db = 	{
	
	dnx_port_nif_db_is_init,
	dnx_port_nif_db_init,
		{
		
			{
			
				{
				
				dnx_port_nif_db_arb_link_list_list_info_head_set,
				dnx_port_nif_db_arb_link_list_list_info_head_get}
			,
				{
				
				dnx_port_nif_db_arb_link_list_list_info_tail_set,
				dnx_port_nif_db_arb_link_list_list_info_tail_get}
			,
				{
				
				dnx_port_nif_db_arb_link_list_list_info_size_set,
				dnx_port_nif_db_arb_link_list_list_info_size_get}
			}
		,
			{
			
			dnx_port_nif_db_arb_link_list_linking_info_alloc,
				{
				
				dnx_port_nif_db_arb_link_list_linking_info_next_section_set,
				dnx_port_nif_db_arb_link_list_linking_info_next_section_get}
			}
		,
			{
			
			dnx_port_nif_db_arb_link_list_is_client_allocated_set,
			dnx_port_nif_db_arb_link_list_is_client_allocated_get,
			dnx_port_nif_db_arb_link_list_is_client_allocated_alloc}
		}
	,
		{
		
			{
			
				{
				
				dnx_port_nif_db_oft_link_list_list_info_head_set,
				dnx_port_nif_db_oft_link_list_list_info_head_get}
			,
				{
				
				dnx_port_nif_db_oft_link_list_list_info_tail_set,
				dnx_port_nif_db_oft_link_list_list_info_tail_get}
			,
				{
				
				dnx_port_nif_db_oft_link_list_list_info_size_set,
				dnx_port_nif_db_oft_link_list_list_info_size_get}
			}
		,
			{
			
			dnx_port_nif_db_oft_link_list_linking_info_alloc,
				{
				
				dnx_port_nif_db_oft_link_list_linking_info_next_section_set,
				dnx_port_nif_db_oft_link_list_linking_info_next_section_get}
			}
		,
			{
			
			dnx_port_nif_db_oft_link_list_is_client_allocated_set,
			dnx_port_nif_db_oft_link_list_is_client_allocated_get,
			dnx_port_nif_db_oft_link_list_is_client_allocated_alloc}
		}
	,
		{
		
			{
			
				{
				
				dnx_port_nif_db_ofr_link_list_list_info_head_set,
				dnx_port_nif_db_ofr_link_list_list_info_head_get}
			,
				{
				
				dnx_port_nif_db_ofr_link_list_list_info_tail_set,
				dnx_port_nif_db_ofr_link_list_list_info_tail_get}
			,
				{
				
				dnx_port_nif_db_ofr_link_list_list_info_size_set,
				dnx_port_nif_db_ofr_link_list_list_info_size_get}
			}
		,
			{
			
			dnx_port_nif_db_ofr_link_list_linking_info_alloc,
				{
				
				dnx_port_nif_db_ofr_link_list_linking_info_next_section_set,
				dnx_port_nif_db_ofr_link_list_linking_info_next_section_get}
			}
		,
			{
			
			dnx_port_nif_db_ofr_link_list_is_client_allocated_set,
			dnx_port_nif_db_ofr_link_list_is_client_allocated_get,
			dnx_port_nif_db_ofr_link_list_is_client_allocated_alloc}
		}
	}
;
#undef BSL_LOG_MODULE
