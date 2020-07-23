
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
#include <soc/dnx/swstate/auto_generated/access/trunk_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/trunk_diagnostic.h>



trunk_info_t* trunk_db_dummy = NULL;



int
trunk_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        TRUNK_MODULE_ID,
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
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        TRUNK_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        trunk_info_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "trunk_db_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID]),
        "trunk_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(trunk_info_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        trunk_pool_info_t,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "trunk_db_pools_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        "trunk_db[%d]->pools",
        unit,
        nof_instances_to_alloc_0 * sizeof(trunk_pool_info_t) / sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_alloc(int unit, uint32 pools_idx_0, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        trunk_group_info_t,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "trunk_db_pools_groups_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        "trunk_db[%d]->pools[%d].groups",
        unit, pools_idx_0,
        nof_instances_to_alloc_0 * sizeof(trunk_group_info_t) / sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_psc_set(int unit, uint32 pools_idx_0, uint32 groups_idx_0, int psc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].psc,
        psc,
        int,
        0,
        "trunk_db_pools_groups_psc_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        TRUNK_MODULE_ID,
        &psc,
        "trunk_db[%d]->pools[%d].groups[%d].psc",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_PSC_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_psc_get(int unit, uint32 pools_idx_0, uint32 groups_idx_0, int *psc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        psc);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    *psc = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].psc;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        TRUNK_MODULE_ID,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].psc,
        "trunk_db[%d]->pools[%d].groups[%d].psc",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_PSC_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_psc_profile_id_set(int unit, uint32 pools_idx_0, uint32 groups_idx_0, int psc_profile_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].psc_profile_id,
        psc_profile_id,
        int,
        0,
        "trunk_db_pools_groups_psc_profile_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        TRUNK_MODULE_ID,
        &psc_profile_id,
        "trunk_db[%d]->pools[%d].groups[%d].psc_profile_id",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_PSC_PROFILE_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_psc_profile_id_get(int unit, uint32 pools_idx_0, uint32 groups_idx_0, int *psc_profile_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        psc_profile_id);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    *psc_profile_id = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].psc_profile_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        TRUNK_MODULE_ID,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].psc_profile_id,
        "trunk_db[%d]->pools[%d].groups[%d].psc_profile_id",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_PSC_PROFILE_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_members_set(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 members_idx_0, CONST trunk_group_member_info_t *members)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members[members_idx_0],
        members,
        trunk_group_member_info_t,
        0,
        "trunk_db_pools_groups_members_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        TRUNK_MODULE_ID,
        members,
        "trunk_db[%d]->pools[%d].groups[%d].members[%d]",
        unit, pools_idx_0, groups_idx_0, members_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_MEMBERS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_members_get(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 members_idx_0, trunk_group_member_info_t *members)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        members);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members,
        members_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    *members = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members[members_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        TRUNK_MODULE_ID,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members[members_idx_0],
        "trunk_db[%d]->pools[%d].groups[%d].members[%d]",
        unit, pools_idx_0, groups_idx_0, members_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_MEMBERS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_members_range_read(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 nof_elements, trunk_group_member_info_t *dest)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_RANGE_READ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    DNX_SW_STATE_RANGE_READ(
        unit,
        TRUNK_MODULE_ID,
        dest,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members,
        trunk_group_member_info_t,
        nof_elements,
        0,
        "trunk_db_pools_groups_members_range_read");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_RANGE_READ_LOGGING,
        BSL_LS_SWSTATEDNX_RANGEREAD,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members,
        "trunk_db[%d]->pools[%d].groups[%d].members",
        unit, pools_idx_0, groups_idx_0, nof_elements);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_MEMBERS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_members_range_write(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 nof_elements, const trunk_group_member_info_t *source)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_RANGE_WRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    DNX_SW_STATE_RANGE_COPY(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members,
        source,
        trunk_group_member_info_t,
        nof_elements,
        0,
        "trunk_db_pools_groups_members_range_write");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_RANGE_WRITE_LOGGING,
        BSL_LS_SWSTATEDNX_RANGEWRITE,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members,
        "trunk_db[%d]->pools[%d].groups[%d].members",
        unit, pools_idx_0, groups_idx_0, nof_elements);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_MEMBERS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_members_range_fill(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 from_idx, uint32 nof_elements, trunk_group_member_info_t value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_RANGE_FILL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    DNX_SW_STATE_RANGE_FILL(
        unit,
        TRUNK_MODULE_ID,
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
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members,
        "trunk_db[%d]->pools[%d].groups[%d].members",
        unit, pools_idx_0, groups_idx_0, nof_elements);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_MEMBERS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_members_alloc(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members,
        trunk_group_member_info_t,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "trunk_db_pools_groups_members_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members,
        "trunk_db[%d]->pools[%d].groups[%d].members",
        unit, pools_idx_0, groups_idx_0,
        nof_instances_to_alloc_0 * sizeof(trunk_group_member_info_t) / sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_MEMBERS_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_members_system_port_set(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 members_idx_0, uint32 system_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members,
        members_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members[members_idx_0].system_port,
        system_port,
        uint32,
        0,
        "trunk_db_pools_groups_members_system_port_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        TRUNK_MODULE_ID,
        &system_port,
        "trunk_db[%d]->pools[%d].groups[%d].members[%d].system_port",
        unit, pools_idx_0, groups_idx_0, members_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_MEMBERS_SYSTEM_PORT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_members_system_port_get(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 members_idx_0, uint32 *system_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        system_port);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members,
        members_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    *system_port = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members[members_idx_0].system_port;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        TRUNK_MODULE_ID,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members[members_idx_0].system_port,
        "trunk_db[%d]->pools[%d].groups[%d].members[%d].system_port",
        unit, pools_idx_0, groups_idx_0, members_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_MEMBERS_SYSTEM_PORT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_members_flags_set(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 members_idx_0, uint32 flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members,
        members_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members[members_idx_0].flags,
        flags,
        uint32,
        0,
        "trunk_db_pools_groups_members_flags_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        TRUNK_MODULE_ID,
        &flags,
        "trunk_db[%d]->pools[%d].groups[%d].members[%d].flags",
        unit, pools_idx_0, groups_idx_0, members_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_MEMBERS_FLAGS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_members_flags_get(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 members_idx_0, uint32 *flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        flags);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members,
        members_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    *flags = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members[members_idx_0].flags;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        TRUNK_MODULE_ID,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members[members_idx_0].flags,
        "trunk_db[%d]->pools[%d].groups[%d].members[%d].flags",
        unit, pools_idx_0, groups_idx_0, members_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_MEMBERS_FLAGS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_members_index_set(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 members_idx_0, int index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members,
        members_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members[members_idx_0].index,
        index,
        int,
        0,
        "trunk_db_pools_groups_members_index_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        TRUNK_MODULE_ID,
        &index,
        "trunk_db[%d]->pools[%d].groups[%d].members[%d].index",
        unit, pools_idx_0, groups_idx_0, members_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_MEMBERS_INDEX_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_members_index_get(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 members_idx_0, int *index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        index);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members,
        members_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    *index = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members[members_idx_0].index;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        TRUNK_MODULE_ID,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].members[members_idx_0].index,
        "trunk_db[%d]->pools[%d].groups[%d].members[%d].index",
        unit, pools_idx_0, groups_idx_0, members_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_MEMBERS_INDEX_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_nof_members_set(int unit, uint32 pools_idx_0, uint32 groups_idx_0, int nof_members)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].nof_members,
        nof_members,
        int,
        0,
        "trunk_db_pools_groups_nof_members_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        TRUNK_MODULE_ID,
        &nof_members,
        "trunk_db[%d]->pools[%d].groups[%d].nof_members",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_NOF_MEMBERS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_nof_members_get(int unit, uint32 pools_idx_0, uint32 groups_idx_0, int *nof_members)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        nof_members);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    *nof_members = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].nof_members;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        TRUNK_MODULE_ID,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].nof_members,
        "trunk_db[%d]->pools[%d].groups[%d].nof_members",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_NOF_MEMBERS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_last_member_added_index_set(int unit, uint32 pools_idx_0, uint32 groups_idx_0, int last_member_added_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].last_member_added_index,
        last_member_added_index,
        int,
        0,
        "trunk_db_pools_groups_last_member_added_index_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        TRUNK_MODULE_ID,
        &last_member_added_index,
        "trunk_db[%d]->pools[%d].groups[%d].last_member_added_index",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_LAST_MEMBER_ADDED_INDEX_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_last_member_added_index_get(int unit, uint32 pools_idx_0, uint32 groups_idx_0, int *last_member_added_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        last_member_added_index);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    *last_member_added_index = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].last_member_added_index;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        TRUNK_MODULE_ID,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].last_member_added_index,
        "trunk_db[%d]->pools[%d].groups[%d].last_member_added_index",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_LAST_MEMBER_ADDED_INDEX_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_in_use_set(int unit, uint32 pools_idx_0, uint32 groups_idx_0, int in_use)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].in_use,
        in_use,
        int,
        0,
        "trunk_db_pools_groups_in_use_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        TRUNK_MODULE_ID,
        &in_use,
        "trunk_db[%d]->pools[%d].groups[%d].in_use",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_IN_USE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_in_use_get(int unit, uint32 pools_idx_0, uint32 groups_idx_0, int *in_use)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        in_use);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    *in_use = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].in_use;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        TRUNK_MODULE_ID,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].in_use,
        "trunk_db[%d]->pools[%d].groups[%d].in_use",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_IN_USE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_pp_ports_set(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 pp_ports_idx_0, uint32 pp_ports)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports,
        pp_ports_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports[pp_ports_idx_0],
        pp_ports,
        uint32,
        0,
        "trunk_db_pools_groups_pp_ports_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        TRUNK_MODULE_ID,
        &pp_ports,
        "trunk_db[%d]->pools[%d].groups[%d].pp_ports[%d]",
        unit, pools_idx_0, groups_idx_0, pp_ports_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_PP_PORTS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_pp_ports_get(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 pp_ports_idx_0, uint32 *pp_ports)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        pp_ports);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports,
        pp_ports_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    *pp_ports = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports[pp_ports_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        TRUNK_MODULE_ID,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports[pp_ports_idx_0],
        "trunk_db[%d]->pools[%d].groups[%d].pp_ports[%d]",
        unit, pools_idx_0, groups_idx_0, pp_ports_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_PP_PORTS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_pp_ports_range_read(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 nof_elements, uint32 *dest)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_RANGE_READ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    DNX_SW_STATE_RANGE_READ(
        unit,
        TRUNK_MODULE_ID,
        dest,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports,
        uint32,
        nof_elements,
        0,
        "trunk_db_pools_groups_pp_ports_range_read");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_RANGE_READ_LOGGING,
        BSL_LS_SWSTATEDNX_RANGEREAD,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports,
        "trunk_db[%d]->pools[%d].groups[%d].pp_ports",
        unit, pools_idx_0, groups_idx_0, nof_elements);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_PP_PORTS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_pp_ports_range_write(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 nof_elements, const uint32 *source)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_RANGE_WRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    DNX_SW_STATE_RANGE_COPY(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports,
        source,
        uint32,
        nof_elements,
        0,
        "trunk_db_pools_groups_pp_ports_range_write");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_RANGE_WRITE_LOGGING,
        BSL_LS_SWSTATEDNX_RANGEWRITE,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports,
        "trunk_db[%d]->pools[%d].groups[%d].pp_ports",
        unit, pools_idx_0, groups_idx_0, nof_elements);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_PP_PORTS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_pp_ports_range_fill(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 from_idx, uint32 nof_elements, uint32 value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_RANGE_FILL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    DNX_SW_STATE_RANGE_FILL(
        unit,
        TRUNK_MODULE_ID,
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
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports,
        "trunk_db[%d]->pools[%d].groups[%d].pp_ports",
        unit, pools_idx_0, groups_idx_0, nof_elements);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_PP_PORTS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_pp_ports_alloc(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports,
        uint32,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "trunk_db_pools_groups_pp_ports_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports,
        "trunk_db[%d]->pools[%d].groups[%d].pp_ports",
        unit, pools_idx_0, groups_idx_0,
        nof_instances_to_alloc_0 * sizeof(uint32) / sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].pp_ports));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_PP_PORTS_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_in_header_type_set(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 in_header_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].in_header_type,
        in_header_type,
        uint32,
        0,
        "trunk_db_pools_groups_in_header_type_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        TRUNK_MODULE_ID,
        &in_header_type,
        "trunk_db[%d]->pools[%d].groups[%d].in_header_type",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_IN_HEADER_TYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_in_header_type_get(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 *in_header_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        in_header_type);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    *in_header_type = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].in_header_type;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        TRUNK_MODULE_ID,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].in_header_type,
        "trunk_db[%d]->pools[%d].groups[%d].in_header_type",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_IN_HEADER_TYPE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_out_header_type_set(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 out_header_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].out_header_type,
        out_header_type,
        uint32,
        0,
        "trunk_db_pools_groups_out_header_type_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        TRUNK_MODULE_ID,
        &out_header_type,
        "trunk_db[%d]->pools[%d].groups[%d].out_header_type",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_OUT_HEADER_TYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_out_header_type_get(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 *out_header_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        out_header_type);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    *out_header_type = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].out_header_type;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        TRUNK_MODULE_ID,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].out_header_type,
        "trunk_db[%d]->pools[%d].groups[%d].out_header_type",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_OUT_HEADER_TYPE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_flags_set(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].flags,
        flags,
        uint32,
        0,
        "trunk_db_pools_groups_flags_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        TRUNK_MODULE_ID,
        &flags,
        "trunk_db[%d]->pools[%d].groups[%d].flags",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_FLAGS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_flags_get(int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 *flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        flags);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    *flags = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].flags;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        TRUNK_MODULE_ID,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].flags,
        "trunk_db[%d]->pools[%d].groups[%d].flags",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_FLAGS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_master_trunk_id_set(int unit, uint32 pools_idx_0, uint32 groups_idx_0, int master_trunk_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].master_trunk_id,
        master_trunk_id,
        int,
        0,
        "trunk_db_pools_groups_master_trunk_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        TRUNK_MODULE_ID,
        &master_trunk_id,
        "trunk_db[%d]->pools[%d].groups[%d].master_trunk_id",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_MASTER_TRUNK_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_master_trunk_id_get(int unit, uint32 pools_idx_0, uint32 groups_idx_0, int *master_trunk_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools,
        pools_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        master_trunk_id);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups,
        groups_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    *master_trunk_id = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].master_trunk_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        TRUNK_MODULE_ID,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[pools_idx_0].groups[groups_idx_0].master_trunk_id,
        "trunk_db[%d]->pools[%d].groups[%d].master_trunk_id",
        unit, pools_idx_0, groups_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_POOLS_GROUPS_MASTER_TRUNK_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_trunk_system_port_db_trunk_system_port_entries_set(int unit, uint32 trunk_system_port_entries_idx_0, CONST trunk_system_port_entry_t *trunk_system_port_entries)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries[trunk_system_port_entries_idx_0],
        trunk_system_port_entries,
        trunk_system_port_entry_t,
        0,
        "trunk_db_trunk_system_port_db_trunk_system_port_entries_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        TRUNK_MODULE_ID,
        trunk_system_port_entries,
        "trunk_db[%d]->trunk_system_port_db.trunk_system_port_entries[%d]",
        unit, trunk_system_port_entries_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_TRUNK_SYSTEM_PORT_DB_TRUNK_SYSTEM_PORT_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_trunk_system_port_db_trunk_system_port_entries_get(int unit, uint32 trunk_system_port_entries_idx_0, trunk_system_port_entry_t *trunk_system_port_entries)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries,
        trunk_system_port_entries_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        trunk_system_port_entries);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    *trunk_system_port_entries = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries[trunk_system_port_entries_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        TRUNK_MODULE_ID,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries[trunk_system_port_entries_idx_0],
        "trunk_db[%d]->trunk_system_port_db.trunk_system_port_entries[%d]",
        unit, trunk_system_port_entries_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_TRUNK_SYSTEM_PORT_DB_TRUNK_SYSTEM_PORT_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_trunk_system_port_db_trunk_system_port_entries_range_read(int unit, uint32 nof_elements, trunk_system_port_entry_t *dest)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_RANGE_READ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    DNX_SW_STATE_RANGE_READ(
        unit,
        TRUNK_MODULE_ID,
        dest,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries,
        trunk_system_port_entry_t,
        nof_elements,
        0,
        "trunk_db_trunk_system_port_db_trunk_system_port_entries_range_read");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_RANGE_READ_LOGGING,
        BSL_LS_SWSTATEDNX_RANGEREAD,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries,
        "trunk_db[%d]->trunk_system_port_db.trunk_system_port_entries",
        unit, nof_elements);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_TRUNK_SYSTEM_PORT_DB_TRUNK_SYSTEM_PORT_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_trunk_system_port_db_trunk_system_port_entries_range_write(int unit, uint32 nof_elements, const trunk_system_port_entry_t *source)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_RANGE_WRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    DNX_SW_STATE_RANGE_COPY(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries,
        source,
        trunk_system_port_entry_t,
        nof_elements,
        0,
        "trunk_db_trunk_system_port_db_trunk_system_port_entries_range_write");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_RANGE_WRITE_LOGGING,
        BSL_LS_SWSTATEDNX_RANGEWRITE,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries,
        "trunk_db[%d]->trunk_system_port_db.trunk_system_port_entries",
        unit, nof_elements);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_TRUNK_SYSTEM_PORT_DB_TRUNK_SYSTEM_PORT_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_trunk_system_port_db_trunk_system_port_entries_range_fill(int unit, uint32 from_idx, uint32 nof_elements, trunk_system_port_entry_t value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_RANGE_FILL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    DNX_SW_STATE_RANGE_FILL(
        unit,
        TRUNK_MODULE_ID,
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
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries,
        "trunk_db[%d]->trunk_system_port_db.trunk_system_port_entries",
        unit, nof_elements);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_TRUNK_SYSTEM_PORT_DB_TRUNK_SYSTEM_PORT_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_trunk_system_port_db_trunk_system_port_entries_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries,
        trunk_system_port_entry_t,
        dnx_data_device.general.max_nof_system_ports_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "trunk_db_trunk_system_port_db_trunk_system_port_entries_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries,
        "trunk_db[%d]->trunk_system_port_db.trunk_system_port_entries",
        unit,
        dnx_data_device.general.max_nof_system_ports_get(unit) * sizeof(trunk_system_port_entry_t) / sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_TRUNK_SYSTEM_PORT_DB_TRUNK_SYSTEM_PORT_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_trunk_system_port_db_trunk_system_port_entries_trunk_id_set(int unit, uint32 trunk_system_port_entries_idx_0, int trunk_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries,
        trunk_system_port_entries_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries[trunk_system_port_entries_idx_0].trunk_id,
        trunk_id,
        int,
        0,
        "trunk_db_trunk_system_port_db_trunk_system_port_entries_trunk_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        TRUNK_MODULE_ID,
        &trunk_id,
        "trunk_db[%d]->trunk_system_port_db.trunk_system_port_entries[%d].trunk_id",
        unit, trunk_system_port_entries_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_TRUNK_SYSTEM_PORT_DB_TRUNK_SYSTEM_PORT_ENTRIES_TRUNK_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_trunk_system_port_db_trunk_system_port_entries_trunk_id_get(int unit, uint32 trunk_system_port_entries_idx_0, int *trunk_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries,
        trunk_system_port_entries_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        trunk_id);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    *trunk_id = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries[trunk_system_port_entries_idx_0].trunk_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        TRUNK_MODULE_ID,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries[trunk_system_port_entries_idx_0].trunk_id,
        "trunk_db[%d]->trunk_system_port_db.trunk_system_port_entries[%d].trunk_id",
        unit, trunk_system_port_entries_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_TRUNK_SYSTEM_PORT_DB_TRUNK_SYSTEM_PORT_ENTRIES_TRUNK_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_trunk_system_port_db_trunk_system_port_entries_flags_set(int unit, uint32 trunk_system_port_entries_idx_0, uint32 flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries,
        trunk_system_port_entries_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries[trunk_system_port_entries_idx_0].flags,
        flags,
        uint32,
        0,
        "trunk_db_trunk_system_port_db_trunk_system_port_entries_flags_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        TRUNK_MODULE_ID,
        &flags,
        "trunk_db[%d]->trunk_system_port_db.trunk_system_port_entries[%d].flags",
        unit, trunk_system_port_entries_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_TRUNK_SYSTEM_PORT_DB_TRUNK_SYSTEM_PORT_ENTRIES_FLAGS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_trunk_system_port_db_trunk_system_port_entries_flags_get(int unit, uint32 trunk_system_port_entries_idx_0, uint32 *flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        TRUNK_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        TRUNK_MODULE_ID,
        sw_state_roots_array[unit][TRUNK_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries,
        trunk_system_port_entries_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        flags);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        TRUNK_MODULE_ID,
        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        TRUNK_MODULE_ID);

    *flags = ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries[trunk_system_port_entries_idx_0].flags;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        TRUNK_MODULE_ID,
        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries[trunk_system_port_entries_idx_0].flags,
        "trunk_db[%d]->trunk_system_port_db.trunk_system_port_entries[%d].flags",
        unit, trunk_system_port_entries_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        TRUNK_MODULE_ID,
        trunk_db_info,
        TRUNK_DB_TRUNK_SYSTEM_PORT_DB_TRUNK_SYSTEM_PORT_ENTRIES_FLAGS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

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
	}
;
#undef BSL_LOG_MODULE
