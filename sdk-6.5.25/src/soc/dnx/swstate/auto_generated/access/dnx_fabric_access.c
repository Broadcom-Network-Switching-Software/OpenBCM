
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/dnx_fabric_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_fabric_diagnostic.h>





int
dnx_fabric_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FABRIC_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]),
        "dnx_fabric_db[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fabric_db_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FABRIC_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_FABRIC_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_fabric_db_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_fabric_db_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]),
        "dnx_fabric_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FABRIC_MODULE_ID,
        dnx_fabric_db_info,
        DNX_FABRIC_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]));

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->load_balancing_shr_thread_manager_handler,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fabric_db_modids_to_group_map_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FABRIC_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FABRIC_MODULE_ID,
        sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FABRIC_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map,
        dnx_fabric_modids_group_map_t,
        dnx_data_fabric.mesh.nof_dest_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_fabric_db_modids_to_group_map_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map,
        "dnx_fabric_db[%d]->modids_to_group_map",
        unit,
        dnx_data_fabric.mesh.nof_dest_get(unit) * sizeof(dnx_fabric_modids_group_map_t) / sizeof(*((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FABRIC_MODULE_ID,
        dnx_fabric_db_info,
        DNX_FABRIC_DB_MODIDS_TO_GROUP_MAP_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fabric_db_modids_to_group_map_nof_modids_set(int unit, uint32 modids_to_group_map_idx_0, int nof_modids)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FABRIC_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FABRIC_MODULE_ID,
        sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map,
        modids_to_group_map_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FABRIC_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map[modids_to_group_map_idx_0].nof_modids,
        nof_modids,
        int,
        0,
        "dnx_fabric_db_modids_to_group_map_nof_modids_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FABRIC_MODULE_ID,
        &nof_modids,
        "dnx_fabric_db[%d]->modids_to_group_map[%d].nof_modids",
        unit, modids_to_group_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FABRIC_MODULE_ID,
        dnx_fabric_db_info,
        DNX_FABRIC_DB_MODIDS_TO_GROUP_MAP_NOF_MODIDS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fabric_db_modids_to_group_map_nof_modids_get(int unit, uint32 modids_to_group_map_idx_0, int *nof_modids)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FABRIC_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FABRIC_MODULE_ID,
        sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map,
        modids_to_group_map_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FABRIC_MODULE_ID,
        nof_modids);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FABRIC_MODULE_ID);

    *nof_modids = ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map[modids_to_group_map_idx_0].nof_modids;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FABRIC_MODULE_ID,
        &((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map[modids_to_group_map_idx_0].nof_modids,
        "dnx_fabric_db[%d]->modids_to_group_map[%d].nof_modids",
        unit, modids_to_group_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FABRIC_MODULE_ID,
        dnx_fabric_db_info,
        DNX_FABRIC_DB_MODIDS_TO_GROUP_MAP_NOF_MODIDS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fabric_db_modids_to_group_map_modids_set(int unit, uint32 modids_to_group_map_idx_0, uint32 modids_idx_0, int modids)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FABRIC_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FABRIC_MODULE_ID,
        sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map,
        modids_to_group_map_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map[modids_to_group_map_idx_0].modids);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map[modids_to_group_map_idx_0].modids,
        modids_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FABRIC_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map[modids_to_group_map_idx_0].modids[modids_idx_0],
        modids,
        int,
        0,
        "dnx_fabric_db_modids_to_group_map_modids_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FABRIC_MODULE_ID,
        &modids,
        "dnx_fabric_db[%d]->modids_to_group_map[%d].modids[%d]",
        unit, modids_to_group_map_idx_0, modids_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FABRIC_MODULE_ID,
        dnx_fabric_db_info,
        DNX_FABRIC_DB_MODIDS_TO_GROUP_MAP_MODIDS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fabric_db_modids_to_group_map_modids_get(int unit, uint32 modids_to_group_map_idx_0, uint32 modids_idx_0, int *modids)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FABRIC_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FABRIC_MODULE_ID,
        sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map,
        modids_to_group_map_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FABRIC_MODULE_ID,
        modids);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map[modids_to_group_map_idx_0].modids,
        modids_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map[modids_to_group_map_idx_0].modids);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FABRIC_MODULE_ID);

    *modids = ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map[modids_to_group_map_idx_0].modids[modids_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FABRIC_MODULE_ID,
        &((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map[modids_to_group_map_idx_0].modids[modids_idx_0],
        "dnx_fabric_db[%d]->modids_to_group_map[%d].modids[%d]",
        unit, modids_to_group_map_idx_0, modids_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FABRIC_MODULE_ID,
        dnx_fabric_db_info,
        DNX_FABRIC_DB_MODIDS_TO_GROUP_MAP_MODIDS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fabric_db_modids_to_group_map_modids_alloc(int unit, uint32 modids_to_group_map_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FABRIC_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FABRIC_MODULE_ID,
        sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map,
        modids_to_group_map_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FABRIC_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map[modids_to_group_map_idx_0].modids,
        int,
        dnx_data_device.general.max_nof_fap_ids_per_device_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_fabric_db_modids_to_group_map_modids_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map[modids_to_group_map_idx_0].modids,
        "dnx_fabric_db[%d]->modids_to_group_map[%d].modids",
        unit, modids_to_group_map_idx_0,
        dnx_data_device.general.max_nof_fap_ids_per_device_get(unit) * sizeof(int) / sizeof(*((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map[modids_to_group_map_idx_0].modids));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FABRIC_MODULE_ID,
        dnx_fabric_db_info,
        DNX_FABRIC_DB_MODIDS_TO_GROUP_MAP_MODIDS_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fabric_db_links_alloc(int unit)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FABRIC_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FABRIC_MODULE_ID,
        sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FABRIC_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->links,
        dnx_fabric_link_t,
        dnx_data_fabric.links.nof_links_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_fabric_db_links_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->links,
        "dnx_fabric_db[%d]->links",
        unit,
        dnx_data_fabric.links.nof_links_get(unit) * sizeof(dnx_fabric_link_t) / sizeof(*((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->links));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FABRIC_MODULE_ID,
        dnx_fabric_db_info,
        DNX_FABRIC_DB_LINKS_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_fabric.links.nof_links_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->links[def_val_idx[0]].is_link_allowed,
        1);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fabric_db_links_is_link_allowed_set(int unit, uint32 links_idx_0, int is_link_allowed)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FABRIC_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FABRIC_MODULE_ID,
        sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->links,
        links_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->links);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FABRIC_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->links[links_idx_0].is_link_allowed,
        is_link_allowed,
        int,
        0,
        "dnx_fabric_db_links_is_link_allowed_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FABRIC_MODULE_ID,
        &is_link_allowed,
        "dnx_fabric_db[%d]->links[%d].is_link_allowed",
        unit, links_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FABRIC_MODULE_ID,
        dnx_fabric_db_info,
        DNX_FABRIC_DB_LINKS_IS_LINK_ALLOWED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fabric_db_links_is_link_allowed_get(int unit, uint32 links_idx_0, int *is_link_allowed)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FABRIC_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FABRIC_MODULE_ID,
        sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->links,
        links_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FABRIC_MODULE_ID,
        is_link_allowed);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->links);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FABRIC_MODULE_ID);

    *is_link_allowed = ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->links[links_idx_0].is_link_allowed;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FABRIC_MODULE_ID,
        &((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->links[links_idx_0].is_link_allowed,
        "dnx_fabric_db[%d]->links[%d].is_link_allowed",
        unit, links_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FABRIC_MODULE_ID,
        dnx_fabric_db_info,
        DNX_FABRIC_DB_LINKS_IS_LINK_ALLOWED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fabric_db_links_is_link_isolated_set(int unit, uint32 links_idx_0, int is_link_isolated)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FABRIC_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FABRIC_MODULE_ID,
        sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->links,
        links_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->links);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FABRIC_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->links[links_idx_0].is_link_isolated,
        is_link_isolated,
        int,
        0,
        "dnx_fabric_db_links_is_link_isolated_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FABRIC_MODULE_ID,
        &is_link_isolated,
        "dnx_fabric_db[%d]->links[%d].is_link_isolated",
        unit, links_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FABRIC_MODULE_ID,
        dnx_fabric_db_info,
        DNX_FABRIC_DB_LINKS_IS_LINK_ISOLATED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fabric_db_links_is_link_isolated_get(int unit, uint32 links_idx_0, int *is_link_isolated)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FABRIC_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FABRIC_MODULE_ID,
        sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->links,
        links_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FABRIC_MODULE_ID,
        is_link_isolated);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->links);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FABRIC_MODULE_ID);

    *is_link_isolated = ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->links[links_idx_0].is_link_isolated;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FABRIC_MODULE_ID,
        &((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->links[links_idx_0].is_link_isolated,
        "dnx_fabric_db[%d]->links[%d].is_link_isolated",
        unit, links_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FABRIC_MODULE_ID,
        dnx_fabric_db_info,
        DNX_FABRIC_DB_LINKS_IS_LINK_ISOLATED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fabric_db_load_balancing_shr_thread_manager_handler_set(int unit, shr_thread_manager_handler_t load_balancing_shr_thread_manager_handler)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FABRIC_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_WB);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FABRIC_MODULE_ID,
        sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FABRIC_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->load_balancing_shr_thread_manager_handler,
        load_balancing_shr_thread_manager_handler,
        shr_thread_manager_handler_t,
        0,
        "dnx_fabric_db_load_balancing_shr_thread_manager_handler_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FABRIC_MODULE_ID,
        &load_balancing_shr_thread_manager_handler,
        "dnx_fabric_db[%d]->load_balancing_shr_thread_manager_handler",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FABRIC_MODULE_ID,
        dnx_fabric_db_info,
        DNX_FABRIC_DB_LOAD_BALANCING_SHR_THREAD_MANAGER_HANDLER_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fabric_db_load_balancing_shr_thread_manager_handler_get(int unit, shr_thread_manager_handler_t *load_balancing_shr_thread_manager_handler)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FABRIC_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_WB);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FABRIC_MODULE_ID,
        sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FABRIC_MODULE_ID,
        load_balancing_shr_thread_manager_handler);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FABRIC_MODULE_ID);

    *load_balancing_shr_thread_manager_handler = ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->load_balancing_shr_thread_manager_handler;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FABRIC_MODULE_ID,
        &((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->load_balancing_shr_thread_manager_handler,
        "dnx_fabric_db[%d]->load_balancing_shr_thread_manager_handler",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FABRIC_MODULE_ID,
        dnx_fabric_db_info,
        DNX_FABRIC_DB_LOAD_BALANCING_SHR_THREAD_MANAGER_HANDLER_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fabric_db_system_upgrade_state_0_set(int unit, uint32 state_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FABRIC_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FABRIC_MODULE_ID,
        sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FABRIC_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->system_upgrade.state_0,
        state_0,
        uint32,
        0,
        "dnx_fabric_db_system_upgrade_state_0_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FABRIC_MODULE_ID,
        &state_0,
        "dnx_fabric_db[%d]->system_upgrade.state_0",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FABRIC_MODULE_ID,
        dnx_fabric_db_info,
        DNX_FABRIC_DB_SYSTEM_UPGRADE_STATE_0_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fabric_db_system_upgrade_state_0_get(int unit, uint32 *state_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FABRIC_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FABRIC_MODULE_ID,
        sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FABRIC_MODULE_ID,
        state_0);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FABRIC_MODULE_ID);

    *state_0 = ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->system_upgrade.state_0;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FABRIC_MODULE_ID,
        &((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->system_upgrade.state_0,
        "dnx_fabric_db[%d]->system_upgrade.state_0",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FABRIC_MODULE_ID,
        dnx_fabric_db_info,
        DNX_FABRIC_DB_SYSTEM_UPGRADE_STATE_0_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fabric_db_system_upgrade_state_1_set(int unit, uint32 state_1)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FABRIC_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FABRIC_MODULE_ID,
        sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FABRIC_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->system_upgrade.state_1,
        state_1,
        uint32,
        0,
        "dnx_fabric_db_system_upgrade_state_1_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FABRIC_MODULE_ID,
        &state_1,
        "dnx_fabric_db[%d]->system_upgrade.state_1",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FABRIC_MODULE_ID,
        dnx_fabric_db_info,
        DNX_FABRIC_DB_SYSTEM_UPGRADE_STATE_1_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fabric_db_system_upgrade_state_1_get(int unit, uint32 *state_1)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FABRIC_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FABRIC_MODULE_ID,
        sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FABRIC_MODULE_ID,
        state_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FABRIC_MODULE_ID);

    *state_1 = ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->system_upgrade.state_1;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FABRIC_MODULE_ID,
        &((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->system_upgrade.state_1,
        "dnx_fabric_db[%d]->system_upgrade.state_1",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FABRIC_MODULE_ID,
        dnx_fabric_db_info,
        DNX_FABRIC_DB_SYSTEM_UPGRADE_STATE_1_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fabric_db_system_upgrade_state_2_set(int unit, uint32 state_2)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FABRIC_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FABRIC_MODULE_ID,
        sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FABRIC_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->system_upgrade.state_2,
        state_2,
        uint32,
        0,
        "dnx_fabric_db_system_upgrade_state_2_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FABRIC_MODULE_ID,
        &state_2,
        "dnx_fabric_db[%d]->system_upgrade.state_2",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FABRIC_MODULE_ID,
        dnx_fabric_db_info,
        DNX_FABRIC_DB_SYSTEM_UPGRADE_STATE_2_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fabric_db_system_upgrade_state_2_get(int unit, uint32 *state_2)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FABRIC_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FABRIC_MODULE_ID,
        sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FABRIC_MODULE_ID,
        state_2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FABRIC_MODULE_ID);

    *state_2 = ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->system_upgrade.state_2;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FABRIC_MODULE_ID,
        &((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->system_upgrade.state_2,
        "dnx_fabric_db[%d]->system_upgrade.state_2",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FABRIC_MODULE_ID,
        dnx_fabric_db_info,
        DNX_FABRIC_DB_SYSTEM_UPGRADE_STATE_2_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fabric_db_cable_swap_is_enable_set(int unit, uint32 is_enable_idx_0, int is_enable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FABRIC_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FABRIC_MODULE_ID,
        sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->cable_swap.is_enable,
        is_enable_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->cable_swap.is_enable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FABRIC_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->cable_swap.is_enable[is_enable_idx_0],
        is_enable,
        int,
        0,
        "dnx_fabric_db_cable_swap_is_enable_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FABRIC_MODULE_ID,
        &is_enable,
        "dnx_fabric_db[%d]->cable_swap.is_enable[%d]",
        unit, is_enable_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FABRIC_MODULE_ID,
        dnx_fabric_db_info,
        DNX_FABRIC_DB_CABLE_SWAP_IS_ENABLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fabric_db_cable_swap_is_enable_get(int unit, uint32 is_enable_idx_0, int *is_enable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FABRIC_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FABRIC_MODULE_ID,
        sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->cable_swap.is_enable,
        is_enable_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FABRIC_MODULE_ID,
        is_enable);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->cable_swap.is_enable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FABRIC_MODULE_ID);

    *is_enable = ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->cable_swap.is_enable[is_enable_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FABRIC_MODULE_ID,
        &((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->cable_swap.is_enable[is_enable_idx_0],
        "dnx_fabric_db[%d]->cable_swap.is_enable[%d]",
        unit, is_enable_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FABRIC_MODULE_ID,
        dnx_fabric_db_info,
        DNX_FABRIC_DB_CABLE_SWAP_IS_ENABLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fabric_db_cable_swap_is_enable_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FABRIC_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FABRIC_MODULE_ID,
        sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FABRIC_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->cable_swap.is_enable,
        int,
        dnx_data_fabric.blocks.nof_pms_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_fabric_db_cable_swap_is_enable_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->cable_swap.is_enable,
        "dnx_fabric_db[%d]->cable_swap.is_enable",
        unit,
        dnx_data_fabric.blocks.nof_pms_get(unit) * sizeof(int) / sizeof(*((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->cable_swap.is_enable));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FABRIC_MODULE_ID,
        dnx_fabric_db_info,
        DNX_FABRIC_DB_CABLE_SWAP_IS_ENABLE_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fabric_db_cable_swap_is_master_set(int unit, uint32 is_master_idx_0, int is_master)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FABRIC_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FABRIC_MODULE_ID,
        sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->cable_swap.is_master,
        is_master_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->cable_swap.is_master);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FABRIC_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->cable_swap.is_master[is_master_idx_0],
        is_master,
        int,
        0,
        "dnx_fabric_db_cable_swap_is_master_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FABRIC_MODULE_ID,
        &is_master,
        "dnx_fabric_db[%d]->cable_swap.is_master[%d]",
        unit, is_master_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FABRIC_MODULE_ID,
        dnx_fabric_db_info,
        DNX_FABRIC_DB_CABLE_SWAP_IS_MASTER_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fabric_db_cable_swap_is_master_get(int unit, uint32 is_master_idx_0, int *is_master)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FABRIC_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FABRIC_MODULE_ID,
        sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->cable_swap.is_master,
        is_master_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FABRIC_MODULE_ID,
        is_master);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->cable_swap.is_master);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FABRIC_MODULE_ID);

    *is_master = ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->cable_swap.is_master[is_master_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FABRIC_MODULE_ID,
        &((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->cable_swap.is_master[is_master_idx_0],
        "dnx_fabric_db[%d]->cable_swap.is_master[%d]",
        unit, is_master_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FABRIC_MODULE_ID,
        dnx_fabric_db_info,
        DNX_FABRIC_DB_CABLE_SWAP_IS_MASTER_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fabric_db_cable_swap_is_master_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FABRIC_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FABRIC_MODULE_ID,
        sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FABRIC_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->cable_swap.is_master,
        int,
        dnx_data_fabric.blocks.nof_pms_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_fabric_db_cable_swap_is_master_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_FABRIC_MODULE_ID,
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->cable_swap.is_master,
        "dnx_fabric_db[%d]->cable_swap.is_master",
        unit,
        dnx_data_fabric.blocks.nof_pms_get(unit) * sizeof(int) / sizeof(*((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->cable_swap.is_master));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FABRIC_MODULE_ID,
        dnx_fabric_db_info,
        DNX_FABRIC_DB_CABLE_SWAP_IS_MASTER_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_fabric_db_cbs dnx_fabric_db = 	{
	
	dnx_fabric_db_is_init,
	dnx_fabric_db_init,
		{
		
		dnx_fabric_db_modids_to_group_map_alloc,
			{
			
			dnx_fabric_db_modids_to_group_map_nof_modids_set,
			dnx_fabric_db_modids_to_group_map_nof_modids_get}
		,
			{
			
			dnx_fabric_db_modids_to_group_map_modids_set,
			dnx_fabric_db_modids_to_group_map_modids_get,
			dnx_fabric_db_modids_to_group_map_modids_alloc}
		}
	,
		{
		
		dnx_fabric_db_links_alloc,
			{
			
			dnx_fabric_db_links_is_link_allowed_set,
			dnx_fabric_db_links_is_link_allowed_get}
		,
			{
			
			dnx_fabric_db_links_is_link_isolated_set,
			dnx_fabric_db_links_is_link_isolated_get}
		}
	,
		{
		
		dnx_fabric_db_load_balancing_shr_thread_manager_handler_set,
		dnx_fabric_db_load_balancing_shr_thread_manager_handler_get}
	,
		{
		
			{
			
			dnx_fabric_db_system_upgrade_state_0_set,
			dnx_fabric_db_system_upgrade_state_0_get}
		,
			{
			
			dnx_fabric_db_system_upgrade_state_1_set,
			dnx_fabric_db_system_upgrade_state_1_get}
		,
			{
			
			dnx_fabric_db_system_upgrade_state_2_set,
			dnx_fabric_db_system_upgrade_state_2_get}
		}
	,
		{
		
			{
			
			dnx_fabric_db_cable_swap_is_enable_set,
			dnx_fabric_db_cable_swap_is_enable_get,
			dnx_fabric_db_cable_swap_is_enable_alloc}
		,
			{
			
			dnx_fabric_db_cable_swap_is_master_set,
			dnx_fabric_db_cable_swap_is_master_get,
			dnx_fabric_db_cable_swap_is_master_alloc}
		}
	}
;
#undef BSL_LOG_MODULE
