
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/trunk_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/trunk_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/trunk_layout.h>





int
trunk_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_SW_STATE_TRUNK_DB,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID]),
        "trunk_db[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_SW_STATE_TRUNK_DB,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        trunk_info_t,
        DNX_SW_STATE_TRUNK_DB_NOF_PARAMS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "trunk_db_init",
        DNX_SW_STATE_DIAG_ALLOC,
        trunk_init_layout_structure);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_SW_STATE_TRUNK_DB,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID]),
        "trunk_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB,
        trunk_db_info,
        TRUNK_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID]),
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        trunk_pool_info_t,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][TRUNK_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "trunk_db_pools_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_TRUNK_DB__POOLS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        "trunk_db[%d]->pools",
        unit,
        nof_instances_to_alloc_0 * sizeof(trunk_pool_info_t) / sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS,
        trunk_db_info,
        TRUNK_DB_POOLS_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_alloc(int unit, uint32 pools_idx_0, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        trunk_group_info_t,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][TRUNK_MODULE_ID],
        0,
        DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "trunk_db_pools_groups_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        "trunk_db[%d]->pools[%d].groups",
        unit, pools_idx_0,
        nof_instances_to_alloc_0 * sizeof(trunk_group_info_t) / sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_psc_set(int unit, uint32 pools_idx_0, uint32 groups_idx_0, int psc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].psc,
        psc,
        int,
        0,
        "trunk_db_pools_groups_psc_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC,
        &psc,
        "trunk_db[%d]->pools[%d].groups[%d].psc",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_PSC_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_psc_get(int unit, uint32 pools_idx_0, uint32 groups_idx_0, int *psc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC,
        psc);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC,
        DNX_SW_STATE_DIAG_READ);

    *psc = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].psc;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].psc,
        "trunk_db[%d]->pools[%d].groups[%d].psc",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_PSC_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_psc_profile_id_set(int unit, uint32 pools_idx_0, uint32 groups_idx_0, int psc_profile_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC_PROFILE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC_PROFILE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC_PROFILE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC_PROFILE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC_PROFILE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC_PROFILE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC_PROFILE_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC_PROFILE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].psc_profile_id,
        psc_profile_id,
        int,
        0,
        "trunk_db_pools_groups_psc_profile_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC_PROFILE_ID,
        &psc_profile_id,
        "trunk_db[%d]->pools[%d].groups[%d].psc_profile_id",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC_PROFILE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_PSC_PROFILE_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_psc_profile_id_get(int unit, uint32 pools_idx_0, uint32 groups_idx_0, int *psc_profile_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC_PROFILE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC_PROFILE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC_PROFILE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC_PROFILE_ID,
        psc_profile_id);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC_PROFILE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC_PROFILE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC_PROFILE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC_PROFILE_ID,
        DNX_SW_STATE_DIAG_READ);

    *psc_profile_id = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].psc_profile_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC_PROFILE_ID,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].psc_profile_id,
        "trunk_db[%d]->pools[%d].groups[%d].psc_profile_id",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC_PROFILE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_PSC_PROFILE_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_members_set(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 members_idx_0, CONST trunk_group_member_info_t *members)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members,
        members_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members[members_idx_0],
        members,
        trunk_group_member_info_t,
        0,
        "trunk_db_pools_groups_members_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        members,
        "trunk_db[%d]->pools[%d].groups[%d].members[%d]",
        unit, pools_idx_0, groups_idx_0, members_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_MEMBERS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_members_get(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 members_idx_0, trunk_group_member_info_t *members)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        members);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members,
        members_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        DNX_SW_STATE_DIAG_READ);

    *members = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members[members_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members[members_idx_0],
        "trunk_db[%d]->pools[%d].groups[%d].members[%d]",
        unit, pools_idx_0, groups_idx_0, members_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_MEMBERS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_members_range_read(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 from_idx, uint32 nof_elements, trunk_group_member_info_t *dest)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        SW_STATE_FUNC_RANGE_READ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        DNX_SW_STATE_DIAG_READ);

    DNX_SW_STATE_RANGE_READ(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        dest,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members,
        trunk_group_member_info_t,
        from_idx,
        nof_elements,
        0,
        "trunk_db_pools_groups_members_range_read");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_RANGE_READ_LOGGING,
        BSL_LS_SWSTATEDNX_RANGEREAD,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members,
        "trunk_db[%d]->pools[%d].groups[%d].members",
        unit, pools_idx_0, groups_idx_0, nof_elements);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_MEMBERS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_members_range_write(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 from_idx, uint32 nof_elements, const trunk_group_member_info_t *source)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        SW_STATE_FUNC_RANGE_WRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_RANGE_COPY(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members,
        source,
        trunk_group_member_info_t,
        from_idx,
        nof_elements,
        0,
        "trunk_db_pools_groups_members_range_write");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_RANGE_WRITE_LOGGING,
        BSL_LS_SWSTATEDNX_RANGEWRITE,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members,
        "trunk_db[%d]->pools[%d].groups[%d].members",
        unit, pools_idx_0, groups_idx_0, nof_elements);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_MEMBERS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_members_range_fill(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 from_idx, uint32 nof_elements, trunk_group_member_info_t value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        SW_STATE_FUNC_RANGE_FILL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_RANGE_FILL(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members,
        trunk_group_member_info_t,
        from_idx,
        nof_elements,
        value,
        0,
        "trunk_db_pools_groups_members_range_fill");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_RANGE_FILL_LOGGING,
        BSL_LS_SWSTATEDNX_RANGEFILL,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members,
        "trunk_db[%d]->pools[%d].groups[%d].members",
        unit, pools_idx_0, groups_idx_0, nof_elements);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_MEMBERS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_members_alloc(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members,
        trunk_group_member_info_t,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][TRUNK_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "trunk_db_pools_groups_members_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members,
        "trunk_db[%d]->pools[%d].groups[%d].members",
        unit, pools_idx_0, groups_idx_0,
        nof_instances_to_alloc_0 * sizeof(trunk_group_member_info_t) / sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_MEMBERS_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_members_system_port_set(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 members_idx_0, uint32 system_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SYSTEM_PORT,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SYSTEM_PORT,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SYSTEM_PORT,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SYSTEM_PORT,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SYSTEM_PORT,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SYSTEM_PORT,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SYSTEM_PORT,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members,
        members_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SYSTEM_PORT,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SYSTEM_PORT,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SYSTEM_PORT,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members[members_idx_0].system_port,
        system_port,
        uint32,
        0,
        "trunk_db_pools_groups_members_system_port_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SYSTEM_PORT,
        &system_port,
        "trunk_db[%d]->pools[%d].groups[%d].members[%d].system_port",
        unit, pools_idx_0, groups_idx_0, members_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SYSTEM_PORT,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_MEMBERS_SYSTEM_PORT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_members_system_port_get(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 members_idx_0, uint32 *system_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SYSTEM_PORT,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SYSTEM_PORT,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SYSTEM_PORT,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SYSTEM_PORT,
        system_port);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SYSTEM_PORT,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SYSTEM_PORT,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SYSTEM_PORT,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members,
        members_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SYSTEM_PORT,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SYSTEM_PORT,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SYSTEM_PORT,
        DNX_SW_STATE_DIAG_READ);

    *system_port = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members[members_idx_0].system_port;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SYSTEM_PORT,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members[members_idx_0].system_port,
        "trunk_db[%d]->pools[%d].groups[%d].members[%d].system_port",
        unit, pools_idx_0, groups_idx_0, members_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SYSTEM_PORT,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_MEMBERS_SYSTEM_PORT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_members_flags_set(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 members_idx_0, uint32 flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__FLAGS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__FLAGS,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__FLAGS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__FLAGS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__FLAGS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__FLAGS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__FLAGS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members,
        members_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__FLAGS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__FLAGS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__FLAGS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members[members_idx_0].flags,
        flags,
        uint32,
        0,
        "trunk_db_pools_groups_members_flags_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__FLAGS,
        &flags,
        "trunk_db[%d]->pools[%d].groups[%d].members[%d].flags",
        unit, pools_idx_0, groups_idx_0, members_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__FLAGS,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_MEMBERS_FLAGS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_members_flags_get(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 members_idx_0, uint32 *flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__FLAGS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__FLAGS,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__FLAGS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__FLAGS,
        flags);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__FLAGS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__FLAGS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__FLAGS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members,
        members_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__FLAGS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__FLAGS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__FLAGS,
        DNX_SW_STATE_DIAG_READ);

    *flags = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members[members_idx_0].flags;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__FLAGS,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members[members_idx_0].flags,
        "trunk_db[%d]->pools[%d].groups[%d].members[%d].flags",
        unit, pools_idx_0, groups_idx_0, members_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__FLAGS,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_MEMBERS_FLAGS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_members_index_set(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 members_idx_0, int index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__INDEX,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__INDEX,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__INDEX,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__INDEX,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__INDEX,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__INDEX,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__INDEX,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members,
        members_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__INDEX,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__INDEX,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__INDEX,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members[members_idx_0].index,
        index,
        int,
        0,
        "trunk_db_pools_groups_members_index_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__INDEX,
        &index,
        "trunk_db[%d]->pools[%d].groups[%d].members[%d].index",
        unit, pools_idx_0, groups_idx_0, members_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__INDEX,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_MEMBERS_INDEX_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_members_index_get(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 members_idx_0, int *index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__INDEX,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__INDEX,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__INDEX,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__INDEX,
        index);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__INDEX,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__INDEX,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__INDEX,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members,
        members_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__INDEX,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__INDEX,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__INDEX,
        DNX_SW_STATE_DIAG_READ);

    *index = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members[members_idx_0].index;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__INDEX,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members[members_idx_0].index,
        "trunk_db[%d]->pools[%d].groups[%d].members[%d].index",
        unit, pools_idx_0, groups_idx_0, members_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__INDEX,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_MEMBERS_INDEX_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_members_spa_member_id_set(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 members_idx_0, int spa_member_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SPA_MEMBER_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SPA_MEMBER_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SPA_MEMBER_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SPA_MEMBER_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SPA_MEMBER_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SPA_MEMBER_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SPA_MEMBER_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members,
        members_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SPA_MEMBER_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SPA_MEMBER_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SPA_MEMBER_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members[members_idx_0].spa_member_id,
        spa_member_id,
        int,
        0,
        "trunk_db_pools_groups_members_spa_member_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SPA_MEMBER_ID,
        &spa_member_id,
        "trunk_db[%d]->pools[%d].groups[%d].members[%d].spa_member_id",
        unit, pools_idx_0, groups_idx_0, members_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SPA_MEMBER_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_MEMBERS_SPA_MEMBER_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_members_spa_member_id_get(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 members_idx_0, int *spa_member_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SPA_MEMBER_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SPA_MEMBER_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SPA_MEMBER_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SPA_MEMBER_ID,
        spa_member_id);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SPA_MEMBER_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SPA_MEMBER_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SPA_MEMBER_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members,
        members_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SPA_MEMBER_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SPA_MEMBER_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SPA_MEMBER_ID,
        DNX_SW_STATE_DIAG_READ);

    *spa_member_id = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members[members_idx_0].spa_member_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SPA_MEMBER_ID,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members[members_idx_0].spa_member_id,
        "trunk_db[%d]->pools[%d].groups[%d].members[%d].spa_member_id",
        unit, pools_idx_0, groups_idx_0, members_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SPA_MEMBER_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_MEMBERS_SPA_MEMBER_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_members_protecting_system_port_set(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 members_idx_0, uint32 protecting_system_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__PROTECTING_SYSTEM_PORT,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__PROTECTING_SYSTEM_PORT,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__PROTECTING_SYSTEM_PORT,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__PROTECTING_SYSTEM_PORT,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__PROTECTING_SYSTEM_PORT,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__PROTECTING_SYSTEM_PORT,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__PROTECTING_SYSTEM_PORT,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members,
        members_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__PROTECTING_SYSTEM_PORT,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__PROTECTING_SYSTEM_PORT,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__PROTECTING_SYSTEM_PORT,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members[members_idx_0].protecting_system_port,
        protecting_system_port,
        uint32,
        0,
        "trunk_db_pools_groups_members_protecting_system_port_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__PROTECTING_SYSTEM_PORT,
        &protecting_system_port,
        "trunk_db[%d]->pools[%d].groups[%d].members[%d].protecting_system_port",
        unit, pools_idx_0, groups_idx_0, members_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__PROTECTING_SYSTEM_PORT,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_MEMBERS_PROTECTING_SYSTEM_PORT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_members_protecting_system_port_get(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 members_idx_0, uint32 *protecting_system_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__PROTECTING_SYSTEM_PORT,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__PROTECTING_SYSTEM_PORT,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__PROTECTING_SYSTEM_PORT,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__PROTECTING_SYSTEM_PORT,
        protecting_system_port);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__PROTECTING_SYSTEM_PORT,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__PROTECTING_SYSTEM_PORT,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__PROTECTING_SYSTEM_PORT,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members,
        members_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__PROTECTING_SYSTEM_PORT,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__PROTECTING_SYSTEM_PORT,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__PROTECTING_SYSTEM_PORT,
        DNX_SW_STATE_DIAG_READ);

    *protecting_system_port = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members[members_idx_0].protecting_system_port;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__PROTECTING_SYSTEM_PORT,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members[members_idx_0].protecting_system_port,
        "trunk_db[%d]->pools[%d].groups[%d].members[%d].protecting_system_port",
        unit, pools_idx_0, groups_idx_0, members_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__PROTECTING_SYSTEM_PORT,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_MEMBERS_PROTECTING_SYSTEM_PORT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_nof_members_set(int unit, uint32 pools_idx_0, uint32 groups_idx_0, int nof_members)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__NOF_MEMBERS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__NOF_MEMBERS,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__NOF_MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__NOF_MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__NOF_MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__NOF_MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__NOF_MEMBERS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__NOF_MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].nof_members,
        nof_members,
        int,
        0,
        "trunk_db_pools_groups_nof_members_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__NOF_MEMBERS,
        &nof_members,
        "trunk_db[%d]->pools[%d].groups[%d].nof_members",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__NOF_MEMBERS,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_NOF_MEMBERS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_nof_members_get(int unit, uint32 pools_idx_0, uint32 groups_idx_0, int *nof_members)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__NOF_MEMBERS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__NOF_MEMBERS,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__NOF_MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__NOF_MEMBERS,
        nof_members);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__NOF_MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__NOF_MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__NOF_MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__NOF_MEMBERS,
        DNX_SW_STATE_DIAG_READ);

    *nof_members = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].nof_members;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__NOF_MEMBERS,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].nof_members,
        "trunk_db[%d]->pools[%d].groups[%d].nof_members",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__NOF_MEMBERS,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_NOF_MEMBERS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_last_member_added_index_set(int unit, uint32 pools_idx_0, uint32 groups_idx_0, int last_member_added_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__LAST_MEMBER_ADDED_INDEX,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__LAST_MEMBER_ADDED_INDEX,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__LAST_MEMBER_ADDED_INDEX,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__LAST_MEMBER_ADDED_INDEX,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__LAST_MEMBER_ADDED_INDEX,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__LAST_MEMBER_ADDED_INDEX,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__LAST_MEMBER_ADDED_INDEX,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__LAST_MEMBER_ADDED_INDEX,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].last_member_added_index,
        last_member_added_index,
        int,
        0,
        "trunk_db_pools_groups_last_member_added_index_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__LAST_MEMBER_ADDED_INDEX,
        &last_member_added_index,
        "trunk_db[%d]->pools[%d].groups[%d].last_member_added_index",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__LAST_MEMBER_ADDED_INDEX,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_LAST_MEMBER_ADDED_INDEX_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_last_member_added_index_get(int unit, uint32 pools_idx_0, uint32 groups_idx_0, int *last_member_added_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__LAST_MEMBER_ADDED_INDEX,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__LAST_MEMBER_ADDED_INDEX,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__LAST_MEMBER_ADDED_INDEX,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__LAST_MEMBER_ADDED_INDEX,
        last_member_added_index);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__LAST_MEMBER_ADDED_INDEX,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__LAST_MEMBER_ADDED_INDEX,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__LAST_MEMBER_ADDED_INDEX,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__LAST_MEMBER_ADDED_INDEX,
        DNX_SW_STATE_DIAG_READ);

    *last_member_added_index = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].last_member_added_index;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__LAST_MEMBER_ADDED_INDEX,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].last_member_added_index,
        "trunk_db[%d]->pools[%d].groups[%d].last_member_added_index",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__LAST_MEMBER_ADDED_INDEX,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_LAST_MEMBER_ADDED_INDEX_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_in_use_set(int unit, uint32 pools_idx_0, uint32 groups_idx_0, int in_use)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_USE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_USE,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_USE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_USE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_USE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_USE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_USE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_USE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].in_use,
        in_use,
        int,
        0,
        "trunk_db_pools_groups_in_use_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_USE,
        &in_use,
        "trunk_db[%d]->pools[%d].groups[%d].in_use",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_USE,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_IN_USE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_in_use_get(int unit, uint32 pools_idx_0, uint32 groups_idx_0, int *in_use)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_USE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_USE,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_USE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_USE,
        in_use);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_USE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_USE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_USE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_USE,
        DNX_SW_STATE_DIAG_READ);

    *in_use = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].in_use;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_USE,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].in_use,
        "trunk_db[%d]->pools[%d].groups[%d].in_use",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_USE,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_IN_USE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_pp_ports_set(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 pp_ports_idx_0, uint32 pp_ports)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports,
        pp_ports_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports[pp_ports_idx_0],
        pp_ports,
        uint32,
        0,
        "trunk_db_pools_groups_pp_ports_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        &pp_ports,
        "trunk_db[%d]->pools[%d].groups[%d].pp_ports[%d]",
        unit, pools_idx_0, groups_idx_0, pp_ports_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_PP_PORTS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_pp_ports_get(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 pp_ports_idx_0, uint32 *pp_ports)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        pp_ports);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports,
        pp_ports_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        DNX_SW_STATE_DIAG_READ);

    *pp_ports = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports[pp_ports_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports[pp_ports_idx_0],
        "trunk_db[%d]->pools[%d].groups[%d].pp_ports[%d]",
        unit, pools_idx_0, groups_idx_0, pp_ports_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_PP_PORTS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_pp_ports_range_read(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 from_idx, uint32 nof_elements, uint32 *dest)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        SW_STATE_FUNC_RANGE_READ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        DNX_SW_STATE_DIAG_READ);

    DNX_SW_STATE_RANGE_READ(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        dest,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports,
        uint32,
        from_idx,
        nof_elements,
        0,
        "trunk_db_pools_groups_pp_ports_range_read");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_RANGE_READ_LOGGING,
        BSL_LS_SWSTATEDNX_RANGEREAD,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports,
        "trunk_db[%d]->pools[%d].groups[%d].pp_ports",
        unit, pools_idx_0, groups_idx_0, nof_elements);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_PP_PORTS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_pp_ports_range_write(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 from_idx, uint32 nof_elements, const uint32 *source)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        SW_STATE_FUNC_RANGE_WRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_RANGE_COPY(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports,
        source,
        uint32,
        from_idx,
        nof_elements,
        0,
        "trunk_db_pools_groups_pp_ports_range_write");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_RANGE_WRITE_LOGGING,
        BSL_LS_SWSTATEDNX_RANGEWRITE,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports,
        "trunk_db[%d]->pools[%d].groups[%d].pp_ports",
        unit, pools_idx_0, groups_idx_0, nof_elements);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_PP_PORTS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_pp_ports_range_fill(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 from_idx, uint32 nof_elements, uint32 value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        SW_STATE_FUNC_RANGE_FILL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_RANGE_FILL(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports,
        uint32,
        from_idx,
        nof_elements,
        value,
        0,
        "trunk_db_pools_groups_pp_ports_range_fill");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_RANGE_FILL_LOGGING,
        BSL_LS_SWSTATEDNX_RANGEFILL,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports,
        "trunk_db[%d]->pools[%d].groups[%d].pp_ports",
        unit, pools_idx_0, groups_idx_0, nof_elements);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_PP_PORTS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_pp_ports_alloc(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports,
        uint32,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][TRUNK_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "trunk_db_pools_groups_pp_ports_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports,
        "trunk_db[%d]->pools[%d].groups[%d].pp_ports",
        unit, pools_idx_0, groups_idx_0,
        nof_instances_to_alloc_0 * sizeof(uint32) / sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_PP_PORTS_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_in_header_type_set(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 in_header_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_HEADER_TYPE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_HEADER_TYPE,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_HEADER_TYPE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_HEADER_TYPE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_HEADER_TYPE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_HEADER_TYPE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_HEADER_TYPE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_HEADER_TYPE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].in_header_type,
        in_header_type,
        uint32,
        0,
        "trunk_db_pools_groups_in_header_type_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_HEADER_TYPE,
        &in_header_type,
        "trunk_db[%d]->pools[%d].groups[%d].in_header_type",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_HEADER_TYPE,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_IN_HEADER_TYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_in_header_type_get(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 *in_header_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_HEADER_TYPE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_HEADER_TYPE,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_HEADER_TYPE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_HEADER_TYPE,
        in_header_type);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_HEADER_TYPE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_HEADER_TYPE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_HEADER_TYPE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_HEADER_TYPE,
        DNX_SW_STATE_DIAG_READ);

    *in_header_type = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].in_header_type;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_HEADER_TYPE,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].in_header_type,
        "trunk_db[%d]->pools[%d].groups[%d].in_header_type",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_HEADER_TYPE,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_IN_HEADER_TYPE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_out_header_type_set(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 out_header_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__OUT_HEADER_TYPE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__OUT_HEADER_TYPE,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__OUT_HEADER_TYPE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__OUT_HEADER_TYPE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__OUT_HEADER_TYPE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__OUT_HEADER_TYPE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__OUT_HEADER_TYPE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__OUT_HEADER_TYPE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].out_header_type,
        out_header_type,
        uint32,
        0,
        "trunk_db_pools_groups_out_header_type_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__OUT_HEADER_TYPE,
        &out_header_type,
        "trunk_db[%d]->pools[%d].groups[%d].out_header_type",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__OUT_HEADER_TYPE,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_OUT_HEADER_TYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_out_header_type_get(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 *out_header_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__OUT_HEADER_TYPE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__OUT_HEADER_TYPE,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__OUT_HEADER_TYPE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__OUT_HEADER_TYPE,
        out_header_type);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__OUT_HEADER_TYPE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__OUT_HEADER_TYPE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__OUT_HEADER_TYPE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__OUT_HEADER_TYPE,
        DNX_SW_STATE_DIAG_READ);

    *out_header_type = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].out_header_type;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__OUT_HEADER_TYPE,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].out_header_type,
        "trunk_db[%d]->pools[%d].groups[%d].out_header_type",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__OUT_HEADER_TYPE,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_OUT_HEADER_TYPE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_flags_set(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__FLAGS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__FLAGS,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__FLAGS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__FLAGS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__FLAGS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__FLAGS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__FLAGS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__FLAGS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].flags,
        flags,
        uint32,
        0,
        "trunk_db_pools_groups_flags_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__FLAGS,
        &flags,
        "trunk_db[%d]->pools[%d].groups[%d].flags",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__FLAGS,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_FLAGS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_flags_get(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 *flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__FLAGS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__FLAGS,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__FLAGS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__FLAGS,
        flags);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__FLAGS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__FLAGS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__FLAGS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__FLAGS,
        DNX_SW_STATE_DIAG_READ);

    *flags = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].flags;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__FLAGS,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].flags,
        "trunk_db[%d]->pools[%d].groups[%d].flags",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__FLAGS,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_FLAGS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_master_trunk_id_set(int unit, uint32 pools_idx_0, uint32 groups_idx_0, int master_trunk_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MASTER_TRUNK_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MASTER_TRUNK_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MASTER_TRUNK_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MASTER_TRUNK_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MASTER_TRUNK_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MASTER_TRUNK_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MASTER_TRUNK_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MASTER_TRUNK_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].master_trunk_id,
        master_trunk_id,
        int,
        0,
        "trunk_db_pools_groups_master_trunk_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MASTER_TRUNK_ID,
        &master_trunk_id,
        "trunk_db[%d]->pools[%d].groups[%d].master_trunk_id",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MASTER_TRUNK_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_MASTER_TRUNK_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_pools_groups_master_trunk_id_get(int unit, uint32 pools_idx_0, uint32 groups_idx_0, int *master_trunk_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MASTER_TRUNK_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MASTER_TRUNK_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MASTER_TRUNK_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MASTER_TRUNK_ID,
        master_trunk_id);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MASTER_TRUNK_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MASTER_TRUNK_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MASTER_TRUNK_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MASTER_TRUNK_ID,
        DNX_SW_STATE_DIAG_READ);

    *master_trunk_id = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].master_trunk_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MASTER_TRUNK_ID,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].master_trunk_id,
        "trunk_db[%d]->pools[%d].groups[%d].master_trunk_id",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MASTER_TRUNK_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_MASTER_TRUNK_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_trunk_system_port_db_trunk_system_port_entries_set(int unit, uint32 trunk_system_port_entries_idx_0, CONST trunk_system_port_entry_t *trunk_system_port_entries)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries,
        trunk_system_port_entries_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries[trunk_system_port_entries_idx_0],
        trunk_system_port_entries,
        trunk_system_port_entry_t,
        0,
        "trunk_db_trunk_system_port_db_trunk_system_port_entries_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        trunk_system_port_entries,
        "trunk_db[%d]->trunk_system_port_db.trunk_system_port_entries[%d]",
        unit, trunk_system_port_entries_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        trunk_db_info,
        TRUNK_DB_TRUNK_SYSTEM_PORT_DB_TRUNK_SYSTEM_PORT_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_trunk_system_port_db_trunk_system_port_entries_get(int unit, uint32 trunk_system_port_entries_idx_0, trunk_system_port_entry_t *trunk_system_port_entries)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries,
        trunk_system_port_entries_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        trunk_system_port_entries);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        DNX_SW_STATE_DIAG_READ);

    *trunk_system_port_entries = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries[trunk_system_port_entries_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries[trunk_system_port_entries_idx_0],
        "trunk_db[%d]->trunk_system_port_db.trunk_system_port_entries[%d]",
        unit, trunk_system_port_entries_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        trunk_db_info,
        TRUNK_DB_TRUNK_SYSTEM_PORT_DB_TRUNK_SYSTEM_PORT_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_trunk_system_port_db_trunk_system_port_entries_range_read(int unit, uint32 from_idx, uint32 nof_elements, trunk_system_port_entry_t *dest)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        SW_STATE_FUNC_RANGE_READ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        DNX_SW_STATE_DIAG_READ);

    DNX_SW_STATE_RANGE_READ(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        dest,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries,
        trunk_system_port_entry_t,
        from_idx,
        nof_elements,
        0,
        "trunk_db_trunk_system_port_db_trunk_system_port_entries_range_read");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_RANGE_READ_LOGGING,
        BSL_LS_SWSTATEDNX_RANGEREAD,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries,
        "trunk_db[%d]->trunk_system_port_db.trunk_system_port_entries",
        unit, nof_elements);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        trunk_db_info,
        TRUNK_DB_TRUNK_SYSTEM_PORT_DB_TRUNK_SYSTEM_PORT_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_trunk_system_port_db_trunk_system_port_entries_range_write(int unit, uint32 from_idx, uint32 nof_elements, const trunk_system_port_entry_t *source)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        SW_STATE_FUNC_RANGE_WRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_RANGE_COPY(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries,
        source,
        trunk_system_port_entry_t,
        from_idx,
        nof_elements,
        0,
        "trunk_db_trunk_system_port_db_trunk_system_port_entries_range_write");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_RANGE_WRITE_LOGGING,
        BSL_LS_SWSTATEDNX_RANGEWRITE,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries,
        "trunk_db[%d]->trunk_system_port_db.trunk_system_port_entries",
        unit, nof_elements);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        trunk_db_info,
        TRUNK_DB_TRUNK_SYSTEM_PORT_DB_TRUNK_SYSTEM_PORT_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_trunk_system_port_db_trunk_system_port_entries_range_fill(int unit, uint32 from_idx, uint32 nof_elements, trunk_system_port_entry_t value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        SW_STATE_FUNC_RANGE_FILL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_RANGE_FILL(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries,
        trunk_system_port_entry_t,
        from_idx,
        nof_elements,
        value,
        0,
        "trunk_db_trunk_system_port_db_trunk_system_port_entries_range_fill");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_RANGE_FILL_LOGGING,
        BSL_LS_SWSTATEDNX_RANGEFILL,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries,
        "trunk_db[%d]->trunk_system_port_db.trunk_system_port_entries",
        unit, nof_elements);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        trunk_db_info,
        TRUNK_DB_TRUNK_SYSTEM_PORT_DB_TRUNK_SYSTEM_PORT_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_trunk_system_port_db_trunk_system_port_entries_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries,
        trunk_system_port_entry_t,
        dnx_data_device.general.max_nof_system_ports_get(unit),
        sw_state_layout_array[unit][TRUNK_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "trunk_db_trunk_system_port_db_trunk_system_port_entries_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries,
        "trunk_db[%d]->trunk_system_port_db.trunk_system_port_entries",
        unit,
        dnx_data_device.general.max_nof_system_ports_get(unit) * sizeof(trunk_system_port_entry_t) / sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES,
        trunk_db_info,
        TRUNK_DB_TRUNK_SYSTEM_PORT_DB_TRUNK_SYSTEM_PORT_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_trunk_system_port_db_trunk_system_port_entries_trunk_id_set(int unit, uint32 trunk_system_port_entries_idx_0, int trunk_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__TRUNK_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__TRUNK_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__TRUNK_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries,
        trunk_system_port_entries_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__TRUNK_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__TRUNK_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__TRUNK_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries[trunk_system_port_entries_idx_0].trunk_id,
        trunk_id,
        int,
        0,
        "trunk_db_trunk_system_port_db_trunk_system_port_entries_trunk_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__TRUNK_ID,
        &trunk_id,
        "trunk_db[%d]->trunk_system_port_db.trunk_system_port_entries[%d].trunk_id",
        unit, trunk_system_port_entries_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__TRUNK_ID,
        trunk_db_info,
        TRUNK_DB_TRUNK_SYSTEM_PORT_DB_TRUNK_SYSTEM_PORT_ENTRIES_TRUNK_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_trunk_system_port_db_trunk_system_port_entries_trunk_id_get(int unit, uint32 trunk_system_port_entries_idx_0, int *trunk_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__TRUNK_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__TRUNK_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__TRUNK_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries,
        trunk_system_port_entries_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__TRUNK_ID,
        trunk_id);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__TRUNK_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__TRUNK_ID,
        DNX_SW_STATE_DIAG_READ);

    *trunk_id = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries[trunk_system_port_entries_idx_0].trunk_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__TRUNK_ID,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries[trunk_system_port_entries_idx_0].trunk_id,
        "trunk_db[%d]->trunk_system_port_db.trunk_system_port_entries[%d].trunk_id",
        unit, trunk_system_port_entries_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__TRUNK_ID,
        trunk_db_info,
        TRUNK_DB_TRUNK_SYSTEM_PORT_DB_TRUNK_SYSTEM_PORT_ENTRIES_TRUNK_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_trunk_system_port_db_trunk_system_port_entries_flags_set(int unit, uint32 trunk_system_port_entries_idx_0, uint32 flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__FLAGS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__FLAGS,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__FLAGS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries,
        trunk_system_port_entries_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__FLAGS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__FLAGS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__FLAGS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries[trunk_system_port_entries_idx_0].flags,
        flags,
        uint32,
        0,
        "trunk_db_trunk_system_port_db_trunk_system_port_entries_flags_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__FLAGS,
        &flags,
        "trunk_db[%d]->trunk_system_port_db.trunk_system_port_entries[%d].flags",
        unit, trunk_system_port_entries_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__FLAGS,
        trunk_db_info,
        TRUNK_DB_TRUNK_SYSTEM_PORT_DB_TRUNK_SYSTEM_PORT_ENTRIES_FLAGS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_trunk_system_port_db_trunk_system_port_entries_flags_get(int unit, uint32 trunk_system_port_entries_idx_0, uint32 *flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__FLAGS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__FLAGS,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__FLAGS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries,
        trunk_system_port_entries_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__FLAGS,
        flags);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__FLAGS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__FLAGS,
        DNX_SW_STATE_DIAG_READ);

    *flags = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries[trunk_system_port_entries_idx_0].flags;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__FLAGS,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries[trunk_system_port_entries_idx_0].flags,
        "trunk_db[%d]->trunk_system_port_db.trunk_system_port_entries[%d].flags",
        unit, trunk_system_port_entries_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__FLAGS,
        trunk_db_info,
        TRUNK_DB_TRUNK_SYSTEM_PORT_DB_TRUNK_SYSTEM_PORT_ENTRIES_FLAGS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_psc_general_info_chm_handle_set(int unit, uint32 chm_handle)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__CHM_HANDLE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__CHM_HANDLE,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__CHM_HANDLE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__CHM_HANDLE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.chm_handle,
        chm_handle,
        uint32,
        0,
        "trunk_db_psc_general_info_chm_handle_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__CHM_HANDLE,
        &chm_handle,
        "trunk_db[%d]->psc_general_info.chm_handle",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__CHM_HANDLE,
        trunk_db_info,
        TRUNK_DB_PSC_GENERAL_INFO_CHM_HANDLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_psc_general_info_chm_handle_get(int unit, uint32 *chm_handle)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__CHM_HANDLE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__CHM_HANDLE,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__CHM_HANDLE,
        chm_handle);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__CHM_HANDLE,
        DNX_SW_STATE_DIAG_READ);

    *chm_handle = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.chm_handle;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__CHM_HANDLE,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.chm_handle,
        "trunk_db[%d]->psc_general_info.chm_handle",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__CHM_HANDLE,
        trunk_db_info,
        TRUNK_DB_PSC_GENERAL_INFO_CHM_HANDLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_psc_general_info_profile_info_set(int unit, uint32 profile_info_idx_0, CONST profile_info_t *profile_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.profile_info,
        profile_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.profile_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.profile_info[profile_info_idx_0],
        profile_info,
        profile_info_t,
        0,
        "trunk_db_psc_general_info_profile_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        profile_info,
        "trunk_db[%d]->psc_general_info.profile_info[%d]",
        unit, profile_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        trunk_db_info,
        TRUNK_DB_PSC_GENERAL_INFO_PROFILE_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_psc_general_info_profile_info_get(int unit, uint32 profile_info_idx_0, profile_info_t *profile_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.profile_info,
        profile_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        profile_info);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.profile_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        DNX_SW_STATE_DIAG_READ);

    *profile_info = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.profile_info[profile_info_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.profile_info[profile_info_idx_0],
        "trunk_db[%d]->psc_general_info.profile_info[%d]",
        unit, profile_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        trunk_db_info,
        TRUNK_DB_PSC_GENERAL_INFO_PROFILE_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_psc_general_info_profile_info_range_read(int unit, uint32 from_idx, uint32 nof_elements, profile_info_t *dest)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        SW_STATE_FUNC_RANGE_READ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.profile_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        DNX_SW_STATE_DIAG_READ);

    DNX_SW_STATE_RANGE_READ(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        dest,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.profile_info,
        profile_info_t,
        from_idx,
        nof_elements,
        0,
        "trunk_db_psc_general_info_profile_info_range_read");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_RANGE_READ_LOGGING,
        BSL_LS_SWSTATEDNX_RANGEREAD,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.profile_info,
        "trunk_db[%d]->psc_general_info.profile_info",
        unit, nof_elements);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        trunk_db_info,
        TRUNK_DB_PSC_GENERAL_INFO_PROFILE_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_psc_general_info_profile_info_range_write(int unit, uint32 from_idx, uint32 nof_elements, const profile_info_t *source)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        SW_STATE_FUNC_RANGE_WRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.profile_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_RANGE_COPY(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.profile_info,
        source,
        profile_info_t,
        from_idx,
        nof_elements,
        0,
        "trunk_db_psc_general_info_profile_info_range_write");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_RANGE_WRITE_LOGGING,
        BSL_LS_SWSTATEDNX_RANGEWRITE,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.profile_info,
        "trunk_db[%d]->psc_general_info.profile_info",
        unit, nof_elements);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        trunk_db_info,
        TRUNK_DB_PSC_GENERAL_INFO_PROFILE_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_psc_general_info_profile_info_range_fill(int unit, uint32 from_idx, uint32 nof_elements, profile_info_t value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        SW_STATE_FUNC_RANGE_FILL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.profile_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_RANGE_FILL(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.profile_info,
        profile_info_t,
        from_idx,
        nof_elements,
        value,
        0,
        "trunk_db_psc_general_info_profile_info_range_fill");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_RANGE_FILL_LOGGING,
        BSL_LS_SWSTATEDNX_RANGEFILL,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.profile_info,
        "trunk_db[%d]->psc_general_info.profile_info",
        unit, nof_elements);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        trunk_db_info,
        TRUNK_DB_PSC_GENERAL_INFO_PROFILE_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_psc_general_info_profile_info_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.profile_info,
        profile_info_t,
        dnx_data_trunk.psc.consistant_hashing_nof_resources_get(unit),
        sw_state_layout_array[unit][TRUNK_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "trunk_db_psc_general_info_profile_info_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.profile_info,
        "trunk_db[%d]->psc_general_info.profile_info",
        unit,
        dnx_data_trunk.psc.consistant_hashing_nof_resources_get(unit) * sizeof(profile_info_t) / sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.profile_info));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO,
        trunk_db_info,
        TRUNK_DB_PSC_GENERAL_INFO_PROFILE_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_psc_general_info_profile_info_psc_profile_type_set(int unit, uint32 profile_info_idx_0, int psc_profile_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__PSC_PROFILE_TYPE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__PSC_PROFILE_TYPE,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__PSC_PROFILE_TYPE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.profile_info,
        profile_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__PSC_PROFILE_TYPE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.profile_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__PSC_PROFILE_TYPE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__PSC_PROFILE_TYPE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.profile_info[profile_info_idx_0].psc_profile_type,
        psc_profile_type,
        int,
        0,
        "trunk_db_psc_general_info_profile_info_psc_profile_type_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__PSC_PROFILE_TYPE,
        &psc_profile_type,
        "trunk_db[%d]->psc_general_info.profile_info[%d].psc_profile_type",
        unit, profile_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__PSC_PROFILE_TYPE,
        trunk_db_info,
        TRUNK_DB_PSC_GENERAL_INFO_PROFILE_INFO_PSC_PROFILE_TYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_psc_general_info_profile_info_psc_profile_type_get(int unit, uint32 profile_info_idx_0, int *psc_profile_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__PSC_PROFILE_TYPE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__PSC_PROFILE_TYPE,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__PSC_PROFILE_TYPE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.profile_info,
        profile_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__PSC_PROFILE_TYPE,
        psc_profile_type);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__PSC_PROFILE_TYPE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.profile_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__PSC_PROFILE_TYPE,
        DNX_SW_STATE_DIAG_READ);

    *psc_profile_type = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.profile_info[profile_info_idx_0].psc_profile_type;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__PSC_PROFILE_TYPE,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.profile_info[profile_info_idx_0].psc_profile_type,
        "trunk_db[%d]->psc_general_info.profile_info[%d].psc_profile_type",
        unit, profile_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__PSC_PROFILE_TYPE,
        trunk_db_info,
        TRUNK_DB_PSC_GENERAL_INFO_PROFILE_INFO_PSC_PROFILE_TYPE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_psc_general_info_profile_info_max_nof_members_set(int unit, uint32 profile_info_idx_0, int max_nof_members)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__MAX_NOF_MEMBERS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__MAX_NOF_MEMBERS,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__MAX_NOF_MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.profile_info,
        profile_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__MAX_NOF_MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.profile_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__MAX_NOF_MEMBERS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__MAX_NOF_MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.profile_info[profile_info_idx_0].max_nof_members,
        max_nof_members,
        int,
        0,
        "trunk_db_psc_general_info_profile_info_max_nof_members_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__MAX_NOF_MEMBERS,
        &max_nof_members,
        "trunk_db[%d]->psc_general_info.profile_info[%d].max_nof_members",
        unit, profile_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__MAX_NOF_MEMBERS,
        trunk_db_info,
        TRUNK_DB_PSC_GENERAL_INFO_PROFILE_INFO_MAX_NOF_MEMBERS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_psc_general_info_profile_info_max_nof_members_get(int unit, uint32 profile_info_idx_0, int *max_nof_members)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__MAX_NOF_MEMBERS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__MAX_NOF_MEMBERS,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__MAX_NOF_MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.profile_info,
        profile_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__MAX_NOF_MEMBERS,
        max_nof_members);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__MAX_NOF_MEMBERS,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.profile_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__MAX_NOF_MEMBERS,
        DNX_SW_STATE_DIAG_READ);

    *max_nof_members = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.profile_info[profile_info_idx_0].max_nof_members;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__MAX_NOF_MEMBERS,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.profile_info[profile_info_idx_0].max_nof_members,
        "trunk_db[%d]->psc_general_info.profile_info[%d].max_nof_members",
        unit, profile_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__MAX_NOF_MEMBERS,
        trunk_db_info,
        TRUNK_DB_PSC_GENERAL_INFO_PROFILE_INFO_MAX_NOF_MEMBERS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_psc_general_info_user_profile_info_set(int unit, uint32 user_profile_info_idx_0, CONST user_profile_info_t *user_profile_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.user_profile_info,
        user_profile_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.user_profile_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.user_profile_info[user_profile_info_idx_0],
        user_profile_info,
        user_profile_info_t,
        0,
        "trunk_db_psc_general_info_user_profile_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        user_profile_info,
        "trunk_db[%d]->psc_general_info.user_profile_info[%d]",
        unit, user_profile_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        trunk_db_info,
        TRUNK_DB_PSC_GENERAL_INFO_USER_PROFILE_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_psc_general_info_user_profile_info_get(int unit, uint32 user_profile_info_idx_0, user_profile_info_t *user_profile_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.user_profile_info,
        user_profile_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        user_profile_info);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.user_profile_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        DNX_SW_STATE_DIAG_READ);

    *user_profile_info = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.user_profile_info[user_profile_info_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.user_profile_info[user_profile_info_idx_0],
        "trunk_db[%d]->psc_general_info.user_profile_info[%d]",
        unit, user_profile_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        trunk_db_info,
        TRUNK_DB_PSC_GENERAL_INFO_USER_PROFILE_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_psc_general_info_user_profile_info_range_read(int unit, uint32 from_idx, uint32 nof_elements, user_profile_info_t *dest)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        SW_STATE_FUNC_RANGE_READ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.user_profile_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        DNX_SW_STATE_DIAG_READ);

    DNX_SW_STATE_RANGE_READ(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        dest,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.user_profile_info,
        user_profile_info_t,
        from_idx,
        nof_elements,
        0,
        "trunk_db_psc_general_info_user_profile_info_range_read");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_RANGE_READ_LOGGING,
        BSL_LS_SWSTATEDNX_RANGEREAD,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.user_profile_info,
        "trunk_db[%d]->psc_general_info.user_profile_info",
        unit, nof_elements);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        trunk_db_info,
        TRUNK_DB_PSC_GENERAL_INFO_USER_PROFILE_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_psc_general_info_user_profile_info_range_write(int unit, uint32 from_idx, uint32 nof_elements, const user_profile_info_t *source)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        SW_STATE_FUNC_RANGE_WRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.user_profile_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_RANGE_COPY(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.user_profile_info,
        source,
        user_profile_info_t,
        from_idx,
        nof_elements,
        0,
        "trunk_db_psc_general_info_user_profile_info_range_write");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_RANGE_WRITE_LOGGING,
        BSL_LS_SWSTATEDNX_RANGEWRITE,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.user_profile_info,
        "trunk_db[%d]->psc_general_info.user_profile_info",
        unit, nof_elements);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        trunk_db_info,
        TRUNK_DB_PSC_GENERAL_INFO_USER_PROFILE_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_psc_general_info_user_profile_info_range_fill(int unit, uint32 from_idx, uint32 nof_elements, user_profile_info_t value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        SW_STATE_FUNC_RANGE_FILL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.user_profile_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_RANGE_FILL(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.user_profile_info,
        user_profile_info_t,
        from_idx,
        nof_elements,
        value,
        0,
        "trunk_db_psc_general_info_user_profile_info_range_fill");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_RANGE_FILL_LOGGING,
        BSL_LS_SWSTATEDNX_RANGEFILL,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.user_profile_info,
        "trunk_db[%d]->psc_general_info.user_profile_info",
        unit, nof_elements);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        trunk_db_info,
        TRUNK_DB_PSC_GENERAL_INFO_USER_PROFILE_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_psc_general_info_user_profile_info_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.user_profile_info,
        user_profile_info_t,
        dnx_data_trunk.psc.consistant_hashing_nof_resources_get(unit),
        sw_state_layout_array[unit][TRUNK_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "trunk_db_psc_general_info_user_profile_info_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.user_profile_info,
        "trunk_db[%d]->psc_general_info.user_profile_info",
        unit,
        dnx_data_trunk.psc.consistant_hashing_nof_resources_get(unit) * sizeof(user_profile_info_t) / sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.user_profile_info));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO,
        trunk_db_info,
        TRUNK_DB_PSC_GENERAL_INFO_USER_PROFILE_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_psc_general_info_user_profile_info_assigned_psc_profile_set(int unit, uint32 user_profile_info_idx_0, int assigned_psc_profile)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__ASSIGNED_PSC_PROFILE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__ASSIGNED_PSC_PROFILE,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__ASSIGNED_PSC_PROFILE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.user_profile_info,
        user_profile_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__ASSIGNED_PSC_PROFILE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.user_profile_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__ASSIGNED_PSC_PROFILE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__ASSIGNED_PSC_PROFILE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.user_profile_info[user_profile_info_idx_0].assigned_psc_profile,
        assigned_psc_profile,
        int,
        0,
        "trunk_db_psc_general_info_user_profile_info_assigned_psc_profile_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__ASSIGNED_PSC_PROFILE,
        &assigned_psc_profile,
        "trunk_db[%d]->psc_general_info.user_profile_info[%d].assigned_psc_profile",
        unit, user_profile_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__ASSIGNED_PSC_PROFILE,
        trunk_db_info,
        TRUNK_DB_PSC_GENERAL_INFO_USER_PROFILE_INFO_ASSIGNED_PSC_PROFILE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_psc_general_info_user_profile_info_assigned_psc_profile_get(int unit, uint32 user_profile_info_idx_0, int *assigned_psc_profile)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__ASSIGNED_PSC_PROFILE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__ASSIGNED_PSC_PROFILE,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__ASSIGNED_PSC_PROFILE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.user_profile_info,
        user_profile_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__ASSIGNED_PSC_PROFILE,
        assigned_psc_profile);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__ASSIGNED_PSC_PROFILE,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.user_profile_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__ASSIGNED_PSC_PROFILE,
        DNX_SW_STATE_DIAG_READ);

    *assigned_psc_profile = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.user_profile_info[user_profile_info_idx_0].assigned_psc_profile;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__ASSIGNED_PSC_PROFILE,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.user_profile_info[user_profile_info_idx_0].assigned_psc_profile,
        "trunk_db[%d]->psc_general_info.user_profile_info[%d].assigned_psc_profile",
        unit, user_profile_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__ASSIGNED_PSC_PROFILE,
        trunk_db_info,
        TRUNK_DB_PSC_GENERAL_INFO_USER_PROFILE_INFO_ASSIGNED_PSC_PROFILE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_psc_general_info_user_profile_info_ref_counter_set(int unit, uint32 user_profile_info_idx_0, int ref_counter)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__REF_COUNTER,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__REF_COUNTER,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__REF_COUNTER,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.user_profile_info,
        user_profile_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__REF_COUNTER,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.user_profile_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__REF_COUNTER,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__REF_COUNTER,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.user_profile_info[user_profile_info_idx_0].ref_counter,
        ref_counter,
        int,
        0,
        "trunk_db_psc_general_info_user_profile_info_ref_counter_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__REF_COUNTER,
        &ref_counter,
        "trunk_db[%d]->psc_general_info.user_profile_info[%d].ref_counter",
        unit, user_profile_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__REF_COUNTER,
        trunk_db_info,
        TRUNK_DB_PSC_GENERAL_INFO_USER_PROFILE_INFO_REF_COUNTER_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
trunk_db_psc_general_info_user_profile_info_ref_counter_get(int unit, uint32 user_profile_info_idx_0, int *ref_counter)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__REF_COUNTER,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__REF_COUNTER,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__REF_COUNTER,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.user_profile_info,
        user_profile_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__REF_COUNTER,
        ref_counter);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__REF_COUNTER,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.user_profile_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__REF_COUNTER,
        DNX_SW_STATE_DIAG_READ);

    *ref_counter = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.user_profile_info[user_profile_info_idx_0].ref_counter;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__REF_COUNTER,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->psc_general_info.user_profile_info[user_profile_info_idx_0].ref_counter,
        "trunk_db[%d]->psc_general_info.user_profile_info[%d].ref_counter",
        unit, user_profile_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__REF_COUNTER,
        trunk_db_info,
        TRUNK_DB_PSC_GENERAL_INFO_USER_PROFILE_INFO_REF_COUNTER_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][TRUNK_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}




trunk_db_cbs trunk_db = 	{
	
	trunk_db_is_init,
	trunk_db_init,
		{
		
		trunk_db_pools_alloc,
			{
			
			trunk_db_pools_groups_alloc,
				{
				
				trunk_db_pools_groups_psc_set,
				trunk_db_pools_groups_psc_get}
			,
				{
				
				trunk_db_pools_groups_psc_profile_id_set,
				trunk_db_pools_groups_psc_profile_id_get}
			,
				{
				
				trunk_db_pools_groups_members_set,
				trunk_db_pools_groups_members_get,
				trunk_db_pools_groups_members_range_read,
				trunk_db_pools_groups_members_range_write,
				trunk_db_pools_groups_members_range_fill,
				trunk_db_pools_groups_members_alloc,
					{
					
					trunk_db_pools_groups_members_system_port_set,
					trunk_db_pools_groups_members_system_port_get}
				,
					{
					
					trunk_db_pools_groups_members_flags_set,
					trunk_db_pools_groups_members_flags_get}
				,
					{
					
					trunk_db_pools_groups_members_index_set,
					trunk_db_pools_groups_members_index_get}
				,
					{
					
					trunk_db_pools_groups_members_spa_member_id_set,
					trunk_db_pools_groups_members_spa_member_id_get}
				,
					{
					
					trunk_db_pools_groups_members_protecting_system_port_set,
					trunk_db_pools_groups_members_protecting_system_port_get}
				}
			,
				{
				
				trunk_db_pools_groups_nof_members_set,
				trunk_db_pools_groups_nof_members_get}
			,
				{
				
				trunk_db_pools_groups_last_member_added_index_set,
				trunk_db_pools_groups_last_member_added_index_get}
			,
				{
				
				trunk_db_pools_groups_in_use_set,
				trunk_db_pools_groups_in_use_get}
			,
				{
				
				trunk_db_pools_groups_pp_ports_set,
				trunk_db_pools_groups_pp_ports_get,
				trunk_db_pools_groups_pp_ports_range_read,
				trunk_db_pools_groups_pp_ports_range_write,
				trunk_db_pools_groups_pp_ports_range_fill,
				trunk_db_pools_groups_pp_ports_alloc}
			,
				{
				
				trunk_db_pools_groups_in_header_type_set,
				trunk_db_pools_groups_in_header_type_get}
			,
				{
				
				trunk_db_pools_groups_out_header_type_set,
				trunk_db_pools_groups_out_header_type_get}
			,
				{
				
				trunk_db_pools_groups_flags_set,
				trunk_db_pools_groups_flags_get}
			,
				{
				
				trunk_db_pools_groups_master_trunk_id_set,
				trunk_db_pools_groups_master_trunk_id_get}
			}
		}
	,
		{
		
			{
			
			trunk_db_trunk_system_port_db_trunk_system_port_entries_set,
			trunk_db_trunk_system_port_db_trunk_system_port_entries_get,
			trunk_db_trunk_system_port_db_trunk_system_port_entries_range_read,
			trunk_db_trunk_system_port_db_trunk_system_port_entries_range_write,
			trunk_db_trunk_system_port_db_trunk_system_port_entries_range_fill,
			trunk_db_trunk_system_port_db_trunk_system_port_entries_alloc,
				{
				
				trunk_db_trunk_system_port_db_trunk_system_port_entries_trunk_id_set,
				trunk_db_trunk_system_port_db_trunk_system_port_entries_trunk_id_get}
			,
				{
				
				trunk_db_trunk_system_port_db_trunk_system_port_entries_flags_set,
				trunk_db_trunk_system_port_db_trunk_system_port_entries_flags_get}
			}
		}
	,
		{
		
			{
			
			trunk_db_psc_general_info_chm_handle_set,
			trunk_db_psc_general_info_chm_handle_get}
		,
			{
			
			trunk_db_psc_general_info_profile_info_set,
			trunk_db_psc_general_info_profile_info_get,
			trunk_db_psc_general_info_profile_info_range_read,
			trunk_db_psc_general_info_profile_info_range_write,
			trunk_db_psc_general_info_profile_info_range_fill,
			trunk_db_psc_general_info_profile_info_alloc,
				{
				
				trunk_db_psc_general_info_profile_info_psc_profile_type_set,
				trunk_db_psc_general_info_profile_info_psc_profile_type_get}
			,
				{
				
				trunk_db_psc_general_info_profile_info_max_nof_members_set,
				trunk_db_psc_general_info_profile_info_max_nof_members_get}
			}
		,
			{
			
			trunk_db_psc_general_info_user_profile_info_set,
			trunk_db_psc_general_info_user_profile_info_get,
			trunk_db_psc_general_info_user_profile_info_range_read,
			trunk_db_psc_general_info_user_profile_info_range_write,
			trunk_db_psc_general_info_user_profile_info_range_fill,
			trunk_db_psc_general_info_user_profile_info_alloc,
				{
				
				trunk_db_psc_general_info_user_profile_info_assigned_psc_profile_set,
				trunk_db_psc_general_info_user_profile_info_assigned_psc_profile_get}
			,
				{
				
				trunk_db_psc_general_info_user_profile_info_ref_counter_set,
				trunk_db_psc_general_info_user_profile_info_ref_counter_get}
			}
		}
	}
;
#undef BSL_LOG_MODULE
