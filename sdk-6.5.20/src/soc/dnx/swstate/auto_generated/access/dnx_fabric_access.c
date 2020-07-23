
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
#include <soc/dnx/swstate/auto_generated/access/dnx_fabric_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_fabric_diagnostic.h>



dnx_fabric_db_t* dnx_fabric_db_dummy = NULL;



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

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        DNX_FABRIC_MODULE_ID,
        dnx_fabric_db_info,
        DNX_FABRIC_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(dnx_fabric_db_t),
        NULL);

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->load_balancing_periodic_event_handler,
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
dnx_fabric_db_load_balancing_periodic_event_handler_set(int unit, periodic_event_handler_t load_balancing_periodic_event_handler)
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
        ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->load_balancing_periodic_event_handler,
        load_balancing_periodic_event_handler,
        periodic_event_handler_t,
        0,
        "dnx_fabric_db_load_balancing_periodic_event_handler_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FABRIC_MODULE_ID,
        &load_balancing_periodic_event_handler,
        "dnx_fabric_db[%d]->load_balancing_periodic_event_handler",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FABRIC_MODULE_ID,
        dnx_fabric_db_info,
        DNX_FABRIC_DB_LOAD_BALANCING_PERIODIC_EVENT_HANDLER_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fabric_db_load_balancing_periodic_event_handler_get(int unit, periodic_event_handler_t *load_balancing_periodic_event_handler)
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
        load_balancing_periodic_event_handler);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FABRIC_MODULE_ID);

    *load_balancing_periodic_event_handler = ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->load_balancing_periodic_event_handler;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FABRIC_MODULE_ID,
        &((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->load_balancing_periodic_event_handler,
        "dnx_fabric_db[%d]->load_balancing_periodic_event_handler",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FABRIC_MODULE_ID,
        dnx_fabric_db_info,
        DNX_FABRIC_DB_LOAD_BALANCING_PERIODIC_EVENT_HANDLER_INFO,
        DNX_SW_STATE_DIAG_READ,
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
		
		dnx_fabric_db_load_balancing_periodic_event_handler_set,
		dnx_fabric_db_load_balancing_periodic_event_handler_get}
	}
;
#undef BSL_LOG_MODULE
