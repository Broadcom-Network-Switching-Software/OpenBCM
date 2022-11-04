
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/dnx_port_nif_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_port_nif_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_port_nif_layout.h>





int
dnx_port_nif_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_SW_STATE_DNX_PORT_NIF_DB,
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
        DNX_SW_STATE_DNX_PORT_NIF_DB,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_port_nif_db_t,
        DNX_SW_STATE_DNX_PORT_NIF_DB_NOF_PARAMS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_port_nif_db_init",
        DNX_SW_STATE_DIAG_ALLOC,
        dnx_port_nif_init_layout_structure);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_SW_STATE_DNX_PORT_NIF_DB,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]),
        "dnx_port_nif_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]),
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_arb_link_list_list_info_head_set(int unit, int head)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__HEAD,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__HEAD,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__HEAD,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__HEAD,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.list_info.head,
        head,
        int,
        0,
        "dnx_port_nif_db_arb_link_list_list_info_head_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__HEAD,
        &head,
        "dnx_port_nif_db[%d]->arb_link_list.list_info.head",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__HEAD,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ARB_LINK_LIST_LIST_INFO_HEAD_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_arb_link_list_list_info_head_get(int unit, int *head)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__HEAD,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__HEAD,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__HEAD,
        head);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__HEAD,
        DNX_SW_STATE_DIAG_READ);

    *head = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.list_info.head;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__HEAD,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.list_info.head,
        "dnx_port_nif_db[%d]->arb_link_list.list_info.head",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__HEAD,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ARB_LINK_LIST_LIST_INFO_HEAD_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_arb_link_list_list_info_tail_set(int unit, int tail)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__TAIL,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__TAIL,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__TAIL,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__TAIL,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.list_info.tail,
        tail,
        int,
        0,
        "dnx_port_nif_db_arb_link_list_list_info_tail_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__TAIL,
        &tail,
        "dnx_port_nif_db[%d]->arb_link_list.list_info.tail",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__TAIL,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ARB_LINK_LIST_LIST_INFO_TAIL_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_arb_link_list_list_info_tail_get(int unit, int *tail)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__TAIL,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__TAIL,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__TAIL,
        tail);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__TAIL,
        DNX_SW_STATE_DIAG_READ);

    *tail = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.list_info.tail;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__TAIL,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.list_info.tail,
        "dnx_port_nif_db[%d]->arb_link_list.list_info.tail",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__TAIL,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ARB_LINK_LIST_LIST_INFO_TAIL_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_arb_link_list_list_info_size_set(int unit, int size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__SIZE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__SIZE,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__SIZE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__SIZE,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.list_info.size,
        size,
        int,
        0,
        "dnx_port_nif_db_arb_link_list_list_info_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__SIZE,
        &size,
        "dnx_port_nif_db[%d]->arb_link_list.list_info.size",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__SIZE,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ARB_LINK_LIST_LIST_INFO_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_arb_link_list_list_info_size_get(int unit, int *size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__SIZE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__SIZE,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__SIZE,
        size);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__SIZE,
        DNX_SW_STATE_DIAG_READ);

    *size = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.list_info.size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__SIZE,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.list_info.size,
        "dnx_port_nif_db[%d]->arb_link_list.list_info.size",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__SIZE,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ARB_LINK_LIST_LIST_INFO_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_arb_link_list_linking_info_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LINKING_INFO,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LINKING_INFO,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LINKING_INFO,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LINKING_INFO,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.linking_info,
        dnx_port_nif_db_linking_info_t,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_port_nif_db_arb_link_list_linking_info_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LINKING_INFO,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.linking_info,
        "dnx_port_nif_db[%d]->arb_link_list.linking_info",
        unit,
        nof_instances_to_alloc_0 * sizeof(dnx_port_nif_db_linking_info_t) / sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.linking_info));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LINKING_INFO,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ARB_LINK_LIST_LINKING_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_arb_link_list_linking_info_next_section_set(int unit, uint32 linking_info_idx_0, int next_section)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.linking_info,
        linking_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.linking_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.linking_info[linking_info_idx_0].next_section,
        next_section,
        int,
        0,
        "dnx_port_nif_db_arb_link_list_linking_info_next_section_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        &next_section,
        "dnx_port_nif_db[%d]->arb_link_list.linking_info[%d].next_section",
        unit, linking_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ARB_LINK_LIST_LINKING_INFO_NEXT_SECTION_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_arb_link_list_linking_info_next_section_get(int unit, uint32 linking_info_idx_0, int *next_section)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.linking_info,
        linking_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        next_section);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.linking_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        DNX_SW_STATE_DIAG_READ);

    *next_section = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.linking_info[linking_info_idx_0].next_section;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.linking_info[linking_info_idx_0].next_section,
        "dnx_port_nif_db[%d]->arb_link_list.linking_info[%d].next_section",
        unit, linking_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ARB_LINK_LIST_LINKING_INFO_NEXT_SECTION_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_arb_link_list_allocated_list_info_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info,
        dnx_port_nif_db_list_info_t,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_port_nif_db_arb_link_list_allocated_list_info_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info,
        "dnx_port_nif_db[%d]->arb_link_list.allocated_list_info",
        unit,
        nof_instances_to_alloc_0 * sizeof(dnx_port_nif_db_list_info_t) / sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ARB_LINK_LIST_ALLOCATED_LIST_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_arb_link_list_allocated_list_info_head_set(int unit, uint32 allocated_list_info_idx_0, int head)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info,
        allocated_list_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info[allocated_list_info_idx_0].head,
        head,
        int,
        0,
        "dnx_port_nif_db_arb_link_list_allocated_list_info_head_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        &head,
        "dnx_port_nif_db[%d]->arb_link_list.allocated_list_info[%d].head",
        unit, allocated_list_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ARB_LINK_LIST_ALLOCATED_LIST_INFO_HEAD_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_arb_link_list_allocated_list_info_head_get(int unit, uint32 allocated_list_info_idx_0, int *head)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info,
        allocated_list_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        head);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        DNX_SW_STATE_DIAG_READ);

    *head = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info[allocated_list_info_idx_0].head;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info[allocated_list_info_idx_0].head,
        "dnx_port_nif_db[%d]->arb_link_list.allocated_list_info[%d].head",
        unit, allocated_list_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ARB_LINK_LIST_ALLOCATED_LIST_INFO_HEAD_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_arb_link_list_allocated_list_info_tail_set(int unit, uint32 allocated_list_info_idx_0, int tail)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info,
        allocated_list_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info[allocated_list_info_idx_0].tail,
        tail,
        int,
        0,
        "dnx_port_nif_db_arb_link_list_allocated_list_info_tail_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        &tail,
        "dnx_port_nif_db[%d]->arb_link_list.allocated_list_info[%d].tail",
        unit, allocated_list_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ARB_LINK_LIST_ALLOCATED_LIST_INFO_TAIL_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_arb_link_list_allocated_list_info_tail_get(int unit, uint32 allocated_list_info_idx_0, int *tail)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info,
        allocated_list_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        tail);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        DNX_SW_STATE_DIAG_READ);

    *tail = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info[allocated_list_info_idx_0].tail;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info[allocated_list_info_idx_0].tail,
        "dnx_port_nif_db[%d]->arb_link_list.allocated_list_info[%d].tail",
        unit, allocated_list_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ARB_LINK_LIST_ALLOCATED_LIST_INFO_TAIL_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_arb_link_list_allocated_list_info_size_set(int unit, uint32 allocated_list_info_idx_0, int size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info,
        allocated_list_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info[allocated_list_info_idx_0].size,
        size,
        int,
        0,
        "dnx_port_nif_db_arb_link_list_allocated_list_info_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        &size,
        "dnx_port_nif_db[%d]->arb_link_list.allocated_list_info[%d].size",
        unit, allocated_list_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ARB_LINK_LIST_ALLOCATED_LIST_INFO_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_arb_link_list_allocated_list_info_size_get(int unit, uint32 allocated_list_info_idx_0, int *size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info,
        allocated_list_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        size);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        DNX_SW_STATE_DIAG_READ);

    *size = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info[allocated_list_info_idx_0].size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info[allocated_list_info_idx_0].size,
        "dnx_port_nif_db[%d]->arb_link_list.allocated_list_info[%d].size",
        unit, allocated_list_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ARB_LINK_LIST_ALLOCATED_LIST_INFO_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_oft_link_list_list_info_head_set(int unit, int head)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__HEAD,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__HEAD,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__HEAD,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__HEAD,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.list_info.head,
        head,
        int,
        0,
        "dnx_port_nif_db_oft_link_list_list_info_head_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__HEAD,
        &head,
        "dnx_port_nif_db[%d]->oft_link_list.list_info.head",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__HEAD,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFT_LINK_LIST_LIST_INFO_HEAD_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_oft_link_list_list_info_head_get(int unit, int *head)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__HEAD,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__HEAD,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__HEAD,
        head);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__HEAD,
        DNX_SW_STATE_DIAG_READ);

    *head = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.list_info.head;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__HEAD,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.list_info.head,
        "dnx_port_nif_db[%d]->oft_link_list.list_info.head",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__HEAD,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFT_LINK_LIST_LIST_INFO_HEAD_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_oft_link_list_list_info_tail_set(int unit, int tail)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__TAIL,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__TAIL,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__TAIL,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__TAIL,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.list_info.tail,
        tail,
        int,
        0,
        "dnx_port_nif_db_oft_link_list_list_info_tail_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__TAIL,
        &tail,
        "dnx_port_nif_db[%d]->oft_link_list.list_info.tail",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__TAIL,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFT_LINK_LIST_LIST_INFO_TAIL_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_oft_link_list_list_info_tail_get(int unit, int *tail)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__TAIL,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__TAIL,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__TAIL,
        tail);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__TAIL,
        DNX_SW_STATE_DIAG_READ);

    *tail = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.list_info.tail;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__TAIL,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.list_info.tail,
        "dnx_port_nif_db[%d]->oft_link_list.list_info.tail",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__TAIL,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFT_LINK_LIST_LIST_INFO_TAIL_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_oft_link_list_list_info_size_set(int unit, int size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__SIZE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__SIZE,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__SIZE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__SIZE,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.list_info.size,
        size,
        int,
        0,
        "dnx_port_nif_db_oft_link_list_list_info_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__SIZE,
        &size,
        "dnx_port_nif_db[%d]->oft_link_list.list_info.size",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__SIZE,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFT_LINK_LIST_LIST_INFO_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_oft_link_list_list_info_size_get(int unit, int *size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__SIZE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__SIZE,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__SIZE,
        size);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__SIZE,
        DNX_SW_STATE_DIAG_READ);

    *size = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.list_info.size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__SIZE,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.list_info.size,
        "dnx_port_nif_db[%d]->oft_link_list.list_info.size",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__SIZE,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFT_LINK_LIST_LIST_INFO_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_oft_link_list_linking_info_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LINKING_INFO,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LINKING_INFO,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LINKING_INFO,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LINKING_INFO,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.linking_info,
        dnx_port_nif_db_linking_info_t,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_port_nif_db_oft_link_list_linking_info_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LINKING_INFO,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.linking_info,
        "dnx_port_nif_db[%d]->oft_link_list.linking_info",
        unit,
        nof_instances_to_alloc_0 * sizeof(dnx_port_nif_db_linking_info_t) / sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.linking_info));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LINKING_INFO,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFT_LINK_LIST_LINKING_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_oft_link_list_linking_info_next_section_set(int unit, uint32 linking_info_idx_0, int next_section)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.linking_info,
        linking_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.linking_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.linking_info[linking_info_idx_0].next_section,
        next_section,
        int,
        0,
        "dnx_port_nif_db_oft_link_list_linking_info_next_section_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        &next_section,
        "dnx_port_nif_db[%d]->oft_link_list.linking_info[%d].next_section",
        unit, linking_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFT_LINK_LIST_LINKING_INFO_NEXT_SECTION_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_oft_link_list_linking_info_next_section_get(int unit, uint32 linking_info_idx_0, int *next_section)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.linking_info,
        linking_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        next_section);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.linking_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        DNX_SW_STATE_DIAG_READ);

    *next_section = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.linking_info[linking_info_idx_0].next_section;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.linking_info[linking_info_idx_0].next_section,
        "dnx_port_nif_db[%d]->oft_link_list.linking_info[%d].next_section",
        unit, linking_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFT_LINK_LIST_LINKING_INFO_NEXT_SECTION_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_oft_link_list_allocated_list_info_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info,
        dnx_port_nif_db_list_info_t,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_port_nif_db_oft_link_list_allocated_list_info_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info,
        "dnx_port_nif_db[%d]->oft_link_list.allocated_list_info",
        unit,
        nof_instances_to_alloc_0 * sizeof(dnx_port_nif_db_list_info_t) / sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFT_LINK_LIST_ALLOCATED_LIST_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_oft_link_list_allocated_list_info_head_set(int unit, uint32 allocated_list_info_idx_0, int head)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info,
        allocated_list_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info[allocated_list_info_idx_0].head,
        head,
        int,
        0,
        "dnx_port_nif_db_oft_link_list_allocated_list_info_head_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        &head,
        "dnx_port_nif_db[%d]->oft_link_list.allocated_list_info[%d].head",
        unit, allocated_list_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFT_LINK_LIST_ALLOCATED_LIST_INFO_HEAD_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_oft_link_list_allocated_list_info_head_get(int unit, uint32 allocated_list_info_idx_0, int *head)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info,
        allocated_list_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        head);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        DNX_SW_STATE_DIAG_READ);

    *head = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info[allocated_list_info_idx_0].head;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info[allocated_list_info_idx_0].head,
        "dnx_port_nif_db[%d]->oft_link_list.allocated_list_info[%d].head",
        unit, allocated_list_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFT_LINK_LIST_ALLOCATED_LIST_INFO_HEAD_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_oft_link_list_allocated_list_info_tail_set(int unit, uint32 allocated_list_info_idx_0, int tail)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info,
        allocated_list_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info[allocated_list_info_idx_0].tail,
        tail,
        int,
        0,
        "dnx_port_nif_db_oft_link_list_allocated_list_info_tail_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        &tail,
        "dnx_port_nif_db[%d]->oft_link_list.allocated_list_info[%d].tail",
        unit, allocated_list_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFT_LINK_LIST_ALLOCATED_LIST_INFO_TAIL_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_oft_link_list_allocated_list_info_tail_get(int unit, uint32 allocated_list_info_idx_0, int *tail)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info,
        allocated_list_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        tail);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        DNX_SW_STATE_DIAG_READ);

    *tail = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info[allocated_list_info_idx_0].tail;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info[allocated_list_info_idx_0].tail,
        "dnx_port_nif_db[%d]->oft_link_list.allocated_list_info[%d].tail",
        unit, allocated_list_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFT_LINK_LIST_ALLOCATED_LIST_INFO_TAIL_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_oft_link_list_allocated_list_info_size_set(int unit, uint32 allocated_list_info_idx_0, int size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info,
        allocated_list_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info[allocated_list_info_idx_0].size,
        size,
        int,
        0,
        "dnx_port_nif_db_oft_link_list_allocated_list_info_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        &size,
        "dnx_port_nif_db[%d]->oft_link_list.allocated_list_info[%d].size",
        unit, allocated_list_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFT_LINK_LIST_ALLOCATED_LIST_INFO_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_oft_link_list_allocated_list_info_size_get(int unit, uint32 allocated_list_info_idx_0, int *size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info,
        allocated_list_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        size);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        DNX_SW_STATE_DIAG_READ);

    *size = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info[allocated_list_info_idx_0].size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info[allocated_list_info_idx_0].size,
        "dnx_port_nif_db[%d]->oft_link_list.allocated_list_info[%d].size",
        unit, allocated_list_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFT_LINK_LIST_ALLOCATED_LIST_INFO_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_ofr_link_list_list_info_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info,
        dnx_port_nif_db_list_info_t,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_port_nif_db_ofr_link_list_list_info_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info,
        "dnx_port_nif_db[%d]->ofr_link_list.list_info",
        unit,
        nof_instances_to_alloc_0 * sizeof(dnx_port_nif_db_list_info_t) / sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFR_LINK_LIST_LIST_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_ofr_link_list_list_info_head_set(int unit, uint32 list_info_idx_0, int head)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__HEAD,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__HEAD,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__HEAD,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info,
        list_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__HEAD,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__HEAD,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__HEAD,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info[list_info_idx_0].head,
        head,
        int,
        0,
        "dnx_port_nif_db_ofr_link_list_list_info_head_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__HEAD,
        &head,
        "dnx_port_nif_db[%d]->ofr_link_list.list_info[%d].head",
        unit, list_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__HEAD,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFR_LINK_LIST_LIST_INFO_HEAD_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_ofr_link_list_list_info_head_get(int unit, uint32 list_info_idx_0, int *head)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__HEAD,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__HEAD,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__HEAD,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info,
        list_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__HEAD,
        head);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__HEAD,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__HEAD,
        DNX_SW_STATE_DIAG_READ);

    *head = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info[list_info_idx_0].head;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__HEAD,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info[list_info_idx_0].head,
        "dnx_port_nif_db[%d]->ofr_link_list.list_info[%d].head",
        unit, list_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__HEAD,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFR_LINK_LIST_LIST_INFO_HEAD_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_ofr_link_list_list_info_tail_set(int unit, uint32 list_info_idx_0, int tail)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__TAIL,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__TAIL,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__TAIL,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info,
        list_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__TAIL,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__TAIL,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__TAIL,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info[list_info_idx_0].tail,
        tail,
        int,
        0,
        "dnx_port_nif_db_ofr_link_list_list_info_tail_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__TAIL,
        &tail,
        "dnx_port_nif_db[%d]->ofr_link_list.list_info[%d].tail",
        unit, list_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__TAIL,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFR_LINK_LIST_LIST_INFO_TAIL_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_ofr_link_list_list_info_tail_get(int unit, uint32 list_info_idx_0, int *tail)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__TAIL,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__TAIL,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__TAIL,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info,
        list_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__TAIL,
        tail);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__TAIL,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__TAIL,
        DNX_SW_STATE_DIAG_READ);

    *tail = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info[list_info_idx_0].tail;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__TAIL,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info[list_info_idx_0].tail,
        "dnx_port_nif_db[%d]->ofr_link_list.list_info[%d].tail",
        unit, list_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__TAIL,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFR_LINK_LIST_LIST_INFO_TAIL_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_ofr_link_list_list_info_size_set(int unit, uint32 list_info_idx_0, int size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__SIZE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__SIZE,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__SIZE,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info,
        list_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__SIZE,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__SIZE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__SIZE,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info[list_info_idx_0].size,
        size,
        int,
        0,
        "dnx_port_nif_db_ofr_link_list_list_info_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__SIZE,
        &size,
        "dnx_port_nif_db[%d]->ofr_link_list.list_info[%d].size",
        unit, list_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__SIZE,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFR_LINK_LIST_LIST_INFO_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_ofr_link_list_list_info_size_get(int unit, uint32 list_info_idx_0, int *size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__SIZE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__SIZE,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__SIZE,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info,
        list_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__SIZE,
        size);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__SIZE,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__SIZE,
        DNX_SW_STATE_DIAG_READ);

    *size = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info[list_info_idx_0].size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__SIZE,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info[list_info_idx_0].size,
        "dnx_port_nif_db[%d]->ofr_link_list.list_info[%d].size",
        unit, list_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__SIZE,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFR_LINK_LIST_LIST_INFO_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_ofr_link_list_linking_info_alloc(int unit, uint32 nof_instances_to_alloc_0, uint32 nof_instances_to_alloc_1)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LINKING_INFO,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LINKING_INFO,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LINKING_INFO,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_DOUBLE_DYNAMIC_ARRAY_SIZE_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LINKING_INFO,
        nof_instances_to_alloc_0,
        nof_instances_to_alloc_1,
        sizeof(dnx_port_nif_db_linking_info_t*));

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LINKING_INFO,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info,
        dnx_port_nif_db_linking_info_t*,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_port_nif_db_ofr_link_list_linking_info_alloc");

    for(uint32 linking_info_idx_0 = 0;
         linking_info_idx_0 < nof_instances_to_alloc_0;
         linking_info_idx_0++)

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LINKING_INFO,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info[linking_info_idx_0],
        dnx_port_nif_db_linking_info_t,
        nof_instances_to_alloc_1,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID],
        1,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_port_nif_db_ofr_link_list_linking_info_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LINKING_INFO,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info,
        "dnx_port_nif_db[%d]->ofr_link_list.linking_info",
        unit,
        nof_instances_to_alloc_1 * sizeof(dnx_port_nif_db_linking_info_t) / sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info)+(nof_instances_to_alloc_1 * sizeof(dnx_port_nif_db_linking_info_t)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LINKING_INFO,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFR_LINK_LIST_LINKING_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_ofr_link_list_linking_info_next_section_set(int unit, uint32 linking_info_idx_0, uint32 linking_info_idx_1, int next_section)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info,
        linking_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info[linking_info_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info[linking_info_idx_0],
        linking_info_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info[linking_info_idx_0][linking_info_idx_1].next_section,
        next_section,
        int,
        0,
        "dnx_port_nif_db_ofr_link_list_linking_info_next_section_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        &next_section,
        "dnx_port_nif_db[%d]->ofr_link_list.linking_info[%d][%d].next_section",
        unit, linking_info_idx_0, linking_info_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFR_LINK_LIST_LINKING_INFO_NEXT_SECTION_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_ofr_link_list_linking_info_next_section_get(int unit, uint32 linking_info_idx_0, uint32 linking_info_idx_1, int *next_section)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info,
        linking_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        next_section);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info[linking_info_idx_0],
        linking_info_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info[linking_info_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        DNX_SW_STATE_DIAG_READ);

    *next_section = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info[linking_info_idx_0][linking_info_idx_1].next_section;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info[linking_info_idx_0][linking_info_idx_1].next_section,
        "dnx_port_nif_db[%d]->ofr_link_list.linking_info[%d][%d].next_section",
        unit, linking_info_idx_0, linking_info_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LINKING_INFO__NEXT_SECTION,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFR_LINK_LIST_LINKING_INFO_NEXT_SECTION_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_ofr_link_list_allocated_list_info_alloc(int unit, uint32 nof_instances_to_alloc_0, uint32 nof_instances_to_alloc_1)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_DOUBLE_DYNAMIC_ARRAY_SIZE_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO,
        nof_instances_to_alloc_0,
        nof_instances_to_alloc_1,
        sizeof(dnx_port_nif_db_list_info_t*));

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info,
        dnx_port_nif_db_list_info_t*,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_port_nif_db_ofr_link_list_allocated_list_info_alloc");

    for(uint32 allocated_list_info_idx_0 = 0;
         allocated_list_info_idx_0 < nof_instances_to_alloc_0;
         allocated_list_info_idx_0++)

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[allocated_list_info_idx_0],
        dnx_port_nif_db_list_info_t,
        nof_instances_to_alloc_1,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID],
        1,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_port_nif_db_ofr_link_list_allocated_list_info_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info,
        "dnx_port_nif_db[%d]->ofr_link_list.allocated_list_info",
        unit,
        nof_instances_to_alloc_1 * sizeof(dnx_port_nif_db_list_info_t) / sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info)+(nof_instances_to_alloc_1 * sizeof(dnx_port_nif_db_list_info_t)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFR_LINK_LIST_ALLOCATED_LIST_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_ofr_link_list_allocated_list_info_head_set(int unit, uint32 allocated_list_info_idx_0, uint32 allocated_list_info_idx_1, int head)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info,
        allocated_list_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[allocated_list_info_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[allocated_list_info_idx_0],
        allocated_list_info_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[allocated_list_info_idx_0][allocated_list_info_idx_1].head,
        head,
        int,
        0,
        "dnx_port_nif_db_ofr_link_list_allocated_list_info_head_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        &head,
        "dnx_port_nif_db[%d]->ofr_link_list.allocated_list_info[%d][%d].head",
        unit, allocated_list_info_idx_0, allocated_list_info_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFR_LINK_LIST_ALLOCATED_LIST_INFO_HEAD_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_ofr_link_list_allocated_list_info_head_get(int unit, uint32 allocated_list_info_idx_0, uint32 allocated_list_info_idx_1, int *head)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info,
        allocated_list_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        head);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[allocated_list_info_idx_0],
        allocated_list_info_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[allocated_list_info_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        DNX_SW_STATE_DIAG_READ);

    *head = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[allocated_list_info_idx_0][allocated_list_info_idx_1].head;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[allocated_list_info_idx_0][allocated_list_info_idx_1].head,
        "dnx_port_nif_db[%d]->ofr_link_list.allocated_list_info[%d][%d].head",
        unit, allocated_list_info_idx_0, allocated_list_info_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFR_LINK_LIST_ALLOCATED_LIST_INFO_HEAD_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_ofr_link_list_allocated_list_info_tail_set(int unit, uint32 allocated_list_info_idx_0, uint32 allocated_list_info_idx_1, int tail)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info,
        allocated_list_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[allocated_list_info_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[allocated_list_info_idx_0],
        allocated_list_info_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[allocated_list_info_idx_0][allocated_list_info_idx_1].tail,
        tail,
        int,
        0,
        "dnx_port_nif_db_ofr_link_list_allocated_list_info_tail_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        &tail,
        "dnx_port_nif_db[%d]->ofr_link_list.allocated_list_info[%d][%d].tail",
        unit, allocated_list_info_idx_0, allocated_list_info_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFR_LINK_LIST_ALLOCATED_LIST_INFO_TAIL_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_ofr_link_list_allocated_list_info_tail_get(int unit, uint32 allocated_list_info_idx_0, uint32 allocated_list_info_idx_1, int *tail)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info,
        allocated_list_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        tail);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[allocated_list_info_idx_0],
        allocated_list_info_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[allocated_list_info_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        DNX_SW_STATE_DIAG_READ);

    *tail = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[allocated_list_info_idx_0][allocated_list_info_idx_1].tail;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[allocated_list_info_idx_0][allocated_list_info_idx_1].tail,
        "dnx_port_nif_db[%d]->ofr_link_list.allocated_list_info[%d][%d].tail",
        unit, allocated_list_info_idx_0, allocated_list_info_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFR_LINK_LIST_ALLOCATED_LIST_INFO_TAIL_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_ofr_link_list_allocated_list_info_size_set(int unit, uint32 allocated_list_info_idx_0, uint32 allocated_list_info_idx_1, int size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info,
        allocated_list_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[allocated_list_info_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[allocated_list_info_idx_0],
        allocated_list_info_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[allocated_list_info_idx_0][allocated_list_info_idx_1].size,
        size,
        int,
        0,
        "dnx_port_nif_db_ofr_link_list_allocated_list_info_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        &size,
        "dnx_port_nif_db[%d]->ofr_link_list.allocated_list_info[%d][%d].size",
        unit, allocated_list_info_idx_0, allocated_list_info_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFR_LINK_LIST_ALLOCATED_LIST_INFO_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_ofr_link_list_allocated_list_info_size_get(int unit, uint32 allocated_list_info_idx_0, uint32 allocated_list_info_idx_1, int *size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info,
        allocated_list_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        size);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[allocated_list_info_idx_0],
        allocated_list_info_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[allocated_list_info_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        DNX_SW_STATE_DIAG_READ);

    *size = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[allocated_list_info_idx_0][allocated_list_info_idx_1].size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[allocated_list_info_idx_0][allocated_list_info_idx_1].size,
        "dnx_port_nif_db[%d]->ofr_link_list.allocated_list_info[%d][%d].size",
        unit, allocated_list_info_idx_0, allocated_list_info_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_OFR_LINK_LIST_ALLOCATED_LIST_INFO_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_active_calendars_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars,
        dnx_port_nif_db_calendar_data_t,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_port_nif_db_active_calendars_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars,
        "dnx_port_nif_db[%d]->active_calendars",
        unit,
        nof_instances_to_alloc_0 * sizeof(dnx_port_nif_db_calendar_data_t) / sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ACTIVE_CALENDARS_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_active_calendars_client_last_objects_set(int unit, uint32 active_calendars_idx_0, uint32 client_last_objects_idx_0, uint32 client_last_objects_idx_1, uint32 client_last_objects)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars,
        active_calendars_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[active_calendars_idx_0].client_last_objects);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[active_calendars_idx_0].client_last_objects,
        client_last_objects_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[active_calendars_idx_0].client_last_objects[client_last_objects_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[active_calendars_idx_0].client_last_objects[client_last_objects_idx_0],
        client_last_objects_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[active_calendars_idx_0].client_last_objects[client_last_objects_idx_0][client_last_objects_idx_1],
        client_last_objects,
        uint32,
        0,
        "dnx_port_nif_db_active_calendars_client_last_objects_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
        &client_last_objects,
        "dnx_port_nif_db[%d]->active_calendars[%d].client_last_objects[%d][%d]",
        unit, active_calendars_idx_0, client_last_objects_idx_0, client_last_objects_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ACTIVE_CALENDARS_CLIENT_LAST_OBJECTS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_active_calendars_client_last_objects_get(int unit, uint32 active_calendars_idx_0, uint32 client_last_objects_idx_0, uint32 client_last_objects_idx_1, uint32 *client_last_objects)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars,
        active_calendars_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
        client_last_objects);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[active_calendars_idx_0].client_last_objects,
        client_last_objects_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[active_calendars_idx_0].client_last_objects);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[active_calendars_idx_0].client_last_objects[client_last_objects_idx_0],
        client_last_objects_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[active_calendars_idx_0].client_last_objects[client_last_objects_idx_0]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
        DNX_SW_STATE_DIAG_READ);

    *client_last_objects = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[active_calendars_idx_0].client_last_objects[client_last_objects_idx_0][client_last_objects_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[active_calendars_idx_0].client_last_objects[client_last_objects_idx_0][client_last_objects_idx_1],
        "dnx_port_nif_db[%d]->active_calendars[%d].client_last_objects[%d][%d]",
        unit, active_calendars_idx_0, client_last_objects_idx_0, client_last_objects_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ACTIVE_CALENDARS_CLIENT_LAST_OBJECTS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_active_calendars_client_last_objects_alloc(int unit, uint32 active_calendars_idx_0, uint32 nof_instances_to_alloc_0, uint32 nof_instances_to_alloc_1)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_DOUBLE_DYNAMIC_ARRAY_SIZE_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
        nof_instances_to_alloc_0,
        nof_instances_to_alloc_1,
        sizeof(uint32*));

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[active_calendars_idx_0].client_last_objects,
        uint32*,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_port_nif_db_active_calendars_client_last_objects_alloc");

    for(uint32 client_last_objects_idx_0 = 0;
         client_last_objects_idx_0 < nof_instances_to_alloc_0;
         client_last_objects_idx_0++)

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[active_calendars_idx_0].client_last_objects[client_last_objects_idx_0],
        uint32,
        nof_instances_to_alloc_1,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID],
        1,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_port_nif_db_active_calendars_client_last_objects_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[active_calendars_idx_0].client_last_objects,
        "dnx_port_nif_db[%d]->active_calendars[%d].client_last_objects",
        unit, active_calendars_idx_0,
        nof_instances_to_alloc_1 * sizeof(uint32) / sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[active_calendars_idx_0].client_last_objects)+(nof_instances_to_alloc_1 * sizeof(uint32)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ACTIVE_CALENDARS_CLIENT_LAST_OBJECTS_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_active_calendars_nof_client_slots_set(int unit, uint32 active_calendars_idx_0, uint32 nof_client_slots_idx_0, uint32 nof_client_slots)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__NOF_CLIENT_SLOTS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__NOF_CLIENT_SLOTS,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__NOF_CLIENT_SLOTS,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars,
        active_calendars_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__NOF_CLIENT_SLOTS,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[active_calendars_idx_0].nof_client_slots);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__NOF_CLIENT_SLOTS,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[active_calendars_idx_0].nof_client_slots,
        nof_client_slots_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__NOF_CLIENT_SLOTS,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__NOF_CLIENT_SLOTS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__NOF_CLIENT_SLOTS,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[active_calendars_idx_0].nof_client_slots[nof_client_slots_idx_0],
        nof_client_slots,
        uint32,
        0,
        "dnx_port_nif_db_active_calendars_nof_client_slots_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__NOF_CLIENT_SLOTS,
        &nof_client_slots,
        "dnx_port_nif_db[%d]->active_calendars[%d].nof_client_slots[%d]",
        unit, active_calendars_idx_0, nof_client_slots_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__NOF_CLIENT_SLOTS,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ACTIVE_CALENDARS_NOF_CLIENT_SLOTS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_active_calendars_nof_client_slots_get(int unit, uint32 active_calendars_idx_0, uint32 nof_client_slots_idx_0, uint32 *nof_client_slots)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__NOF_CLIENT_SLOTS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__NOF_CLIENT_SLOTS,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__NOF_CLIENT_SLOTS,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars,
        active_calendars_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__NOF_CLIENT_SLOTS,
        nof_client_slots);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__NOF_CLIENT_SLOTS,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[active_calendars_idx_0].nof_client_slots,
        nof_client_slots_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__NOF_CLIENT_SLOTS,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[active_calendars_idx_0].nof_client_slots);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__NOF_CLIENT_SLOTS,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__NOF_CLIENT_SLOTS,
        DNX_SW_STATE_DIAG_READ);

    *nof_client_slots = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[active_calendars_idx_0].nof_client_slots[nof_client_slots_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__NOF_CLIENT_SLOTS,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[active_calendars_idx_0].nof_client_slots[nof_client_slots_idx_0],
        "dnx_port_nif_db[%d]->active_calendars[%d].nof_client_slots[%d]",
        unit, active_calendars_idx_0, nof_client_slots_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__NOF_CLIENT_SLOTS,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ACTIVE_CALENDARS_NOF_CLIENT_SLOTS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_active_calendars_nof_client_slots_alloc(int unit, uint32 active_calendars_idx_0, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__NOF_CLIENT_SLOTS,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__NOF_CLIENT_SLOTS,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__NOF_CLIENT_SLOTS,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars,
        active_calendars_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__NOF_CLIENT_SLOTS,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__NOF_CLIENT_SLOTS,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__NOF_CLIENT_SLOTS,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[active_calendars_idx_0].nof_client_slots,
        uint32,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_port_nif_db_active_calendars_nof_client_slots_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__NOF_CLIENT_SLOTS,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[active_calendars_idx_0].nof_client_slots,
        "dnx_port_nif_db[%d]->active_calendars[%d].nof_client_slots",
        unit, active_calendars_idx_0,
        nof_instances_to_alloc_0 * sizeof(uint32) / sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[active_calendars_idx_0].nof_client_slots));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__NOF_CLIENT_SLOTS,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ACTIVE_CALENDARS_NOF_CLIENT_SLOTS_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_active_calendars_last_client_set(int unit, uint32 active_calendars_idx_0, uint32 last_client)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__LAST_CLIENT,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__LAST_CLIENT,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__LAST_CLIENT,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars,
        active_calendars_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__LAST_CLIENT,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__LAST_CLIENT,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__LAST_CLIENT,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[active_calendars_idx_0].last_client,
        last_client,
        uint32,
        0,
        "dnx_port_nif_db_active_calendars_last_client_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__LAST_CLIENT,
        &last_client,
        "dnx_port_nif_db[%d]->active_calendars[%d].last_client",
        unit, active_calendars_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__LAST_CLIENT,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ACTIVE_CALENDARS_LAST_CLIENT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_active_calendars_last_client_get(int unit, uint32 active_calendars_idx_0, uint32 *last_client)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__LAST_CLIENT,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__LAST_CLIENT,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__LAST_CLIENT,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars,
        active_calendars_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__LAST_CLIENT,
        last_client);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__LAST_CLIENT,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__LAST_CLIENT,
        DNX_SW_STATE_DIAG_READ);

    *last_client = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[active_calendars_idx_0].last_client;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__LAST_CLIENT,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[active_calendars_idx_0].last_client,
        "dnx_port_nif_db[%d]->active_calendars[%d].last_client",
        unit, active_calendars_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__LAST_CLIENT,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_ACTIVE_CALENDARS_LAST_CLIENT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_g_hao_active_client_set(int unit, int g_hao_active_client)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__G_HAO_ACTIVE_CLIENT,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__G_HAO_ACTIVE_CLIENT,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__G_HAO_ACTIVE_CLIENT,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__G_HAO_ACTIVE_CLIENT,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->g_hao_active_client,
        g_hao_active_client,
        int,
        0,
        "dnx_port_nif_db_g_hao_active_client_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__G_HAO_ACTIVE_CLIENT,
        &g_hao_active_client,
        "dnx_port_nif_db[%d]->g_hao_active_client",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__G_HAO_ACTIVE_CLIENT,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_G_HAO_ACTIVE_CLIENT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_g_hao_active_client_get(int unit, int *g_hao_active_client)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__G_HAO_ACTIVE_CLIENT,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__G_HAO_ACTIVE_CLIENT,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__G_HAO_ACTIVE_CLIENT,
        g_hao_active_client);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__G_HAO_ACTIVE_CLIENT,
        DNX_SW_STATE_DIAG_READ);

    *g_hao_active_client = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->g_hao_active_client;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__G_HAO_ACTIVE_CLIENT,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->g_hao_active_client,
        "dnx_port_nif_db[%d]->g_hao_active_client",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__G_HAO_ACTIVE_CLIENT,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_G_HAO_ACTIVE_CLIENT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_priority_group_num_of_entries_set(int unit, uint32 priority_group_num_of_entries_idx_0, uint32 priority_group_num_of_entries_idx_1, int priority_group_num_of_entries)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__PRIORITY_GROUP_NUM_OF_ENTRIES,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__PRIORITY_GROUP_NUM_OF_ENTRIES,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        priority_group_num_of_entries_idx_0,
        DNX_DATA_MAX_NIF_ETH_PRIORITY_GROUPS_NOF);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        priority_group_num_of_entries_idx_1,
        DNX_DATA_MAX_NIF_OFR_NOF_RMC_PER_PRIORITY_GROUP);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__PRIORITY_GROUP_NUM_OF_ENTRIES,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__PRIORITY_GROUP_NUM_OF_ENTRIES,
        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->priority_group_num_of_entries[priority_group_num_of_entries_idx_0][priority_group_num_of_entries_idx_1],
        priority_group_num_of_entries,
        int,
        0,
        "dnx_port_nif_db_priority_group_num_of_entries_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__PRIORITY_GROUP_NUM_OF_ENTRIES,
        &priority_group_num_of_entries,
        "dnx_port_nif_db[%d]->priority_group_num_of_entries[%d][%d]",
        unit, priority_group_num_of_entries_idx_0, priority_group_num_of_entries_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__PRIORITY_GROUP_NUM_OF_ENTRIES,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_PRIORITY_GROUP_NUM_OF_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_priority_group_num_of_entries_get(int unit, uint32 priority_group_num_of_entries_idx_0, uint32 priority_group_num_of_entries_idx_1, int *priority_group_num_of_entries)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__PRIORITY_GROUP_NUM_OF_ENTRIES,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__PRIORITY_GROUP_NUM_OF_ENTRIES,
        sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        priority_group_num_of_entries_idx_0,
        DNX_DATA_MAX_NIF_ETH_PRIORITY_GROUPS_NOF);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__PRIORITY_GROUP_NUM_OF_ENTRIES,
        priority_group_num_of_entries);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        priority_group_num_of_entries_idx_1,
        DNX_DATA_MAX_NIF_OFR_NOF_RMC_PER_PRIORITY_GROUP);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__PRIORITY_GROUP_NUM_OF_ENTRIES,
        DNX_SW_STATE_DIAG_READ);

    *priority_group_num_of_entries = ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->priority_group_num_of_entries[priority_group_num_of_entries_idx_0][priority_group_num_of_entries_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_PORT_NIF_DB__PRIORITY_GROUP_NUM_OF_ENTRIES,
        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->priority_group_num_of_entries[priority_group_num_of_entries_idx_0][priority_group_num_of_entries_idx_1],
        "dnx_port_nif_db[%d]->priority_group_num_of_entries[%d][%d]",
        unit, priority_group_num_of_entries_idx_0, priority_group_num_of_entries_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_PORT_NIF_DB__PRIORITY_GROUP_NUM_OF_ENTRIES,
        dnx_port_nif_db_info,
        DNX_PORT_NIF_DB_PRIORITY_GROUP_NUM_OF_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID]);

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
			
			dnx_port_nif_db_arb_link_list_allocated_list_info_alloc,
				{
				
				dnx_port_nif_db_arb_link_list_allocated_list_info_head_set,
				dnx_port_nif_db_arb_link_list_allocated_list_info_head_get}
			,
				{
				
				dnx_port_nif_db_arb_link_list_allocated_list_info_tail_set,
				dnx_port_nif_db_arb_link_list_allocated_list_info_tail_get}
			,
				{
				
				dnx_port_nif_db_arb_link_list_allocated_list_info_size_set,
				dnx_port_nif_db_arb_link_list_allocated_list_info_size_get}
			}
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
			
			dnx_port_nif_db_oft_link_list_allocated_list_info_alloc,
				{
				
				dnx_port_nif_db_oft_link_list_allocated_list_info_head_set,
				dnx_port_nif_db_oft_link_list_allocated_list_info_head_get}
			,
				{
				
				dnx_port_nif_db_oft_link_list_allocated_list_info_tail_set,
				dnx_port_nif_db_oft_link_list_allocated_list_info_tail_get}
			,
				{
				
				dnx_port_nif_db_oft_link_list_allocated_list_info_size_set,
				dnx_port_nif_db_oft_link_list_allocated_list_info_size_get}
			}
		}
	,
		{
		
			{
			
			dnx_port_nif_db_ofr_link_list_list_info_alloc,
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
			
			dnx_port_nif_db_ofr_link_list_allocated_list_info_alloc,
				{
				
				dnx_port_nif_db_ofr_link_list_allocated_list_info_head_set,
				dnx_port_nif_db_ofr_link_list_allocated_list_info_head_get}
			,
				{
				
				dnx_port_nif_db_ofr_link_list_allocated_list_info_tail_set,
				dnx_port_nif_db_ofr_link_list_allocated_list_info_tail_get}
			,
				{
				
				dnx_port_nif_db_ofr_link_list_allocated_list_info_size_set,
				dnx_port_nif_db_ofr_link_list_allocated_list_info_size_get}
			}
		}
	,
		{
		
		dnx_port_nif_db_active_calendars_alloc,
			{
			
			dnx_port_nif_db_active_calendars_client_last_objects_set,
			dnx_port_nif_db_active_calendars_client_last_objects_get,
			dnx_port_nif_db_active_calendars_client_last_objects_alloc}
		,
			{
			
			dnx_port_nif_db_active_calendars_nof_client_slots_set,
			dnx_port_nif_db_active_calendars_nof_client_slots_get,
			dnx_port_nif_db_active_calendars_nof_client_slots_alloc}
		,
			{
			
			dnx_port_nif_db_active_calendars_last_client_set,
			dnx_port_nif_db_active_calendars_last_client_get}
		}
	,
		{
		
		dnx_port_nif_db_g_hao_active_client_set,
		dnx_port_nif_db_g_hao_active_client_get}
	,
		{
		
		dnx_port_nif_db_priority_group_num_of_entries_set,
		dnx_port_nif_db_priority_group_num_of_entries_get}
	}
;
#undef BSL_LOG_MODULE
