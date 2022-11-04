/** \file src/bcm/dnx/trunk/trunk_protection.c
 * 
 *  This file contains the implementation of
 *  trunk protection functions
 * 
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TRUNK

/*
 * Include files.
 * {
 */

#include "trunk_dbal_access.h"
#include "trunk_protection.h"
#include "trunk_sw_db.h"
#include <bcm_int/dnx/trunk/trunk.h>
#include <bcm_int/dnx/algo/algo_gpm.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/*
 * }
 */

/*
 * MACROs
 * {
 */

/*
 * }
 */

/** see header file */
shr_error_e
dnx_trunk_member_protection_add(
    int unit,
    int pool,
    int group,
    int member,
    bcm_trunk_member_t * updated_member)
{
    int added_member_index;
    int need_to_add_new_member_to_pp;
    bcm_trunk_t trunk_id;
    trunk_group_member_info_t master_member_info;
    trunk_group_member_info_t added_member_info;
    SHR_FUNC_INIT_VARS(unit);

    /** add protecting to sw db */
    dnx_trunk_sw_db_member_init(unit, &added_member_info);
    added_member_info.system_port = updated_member->backup_member.gport;
    added_member_info.spa_member_id = updated_member->backup_member.spa_id;
    added_member_info.index = INVALID_TM_INDEX_TO_INDICATE_MEMBER_IS_ONLY_PROTECTING;
    added_member_info.flags = updated_member->backup_member.flags;
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_add(unit, pool, group, &added_member_info, &added_member_index));
    /** get added member */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_get(unit, pool, group, added_member_index, &added_member_info));
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_is_last_spa_ref
                    (unit, pool, group, added_member_index, &need_to_add_new_member_to_pp));

    /** update master to have new protection */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_get(unit, pool, group, member, &master_member_info));
    master_member_info.protecting_system_port = updated_member->backup_member.gport;
    master_member_info.flags |= BCM_TRUNK_MEMBER_WITH_PROTECTION;
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_set(unit, pool, group, member, &master_member_info));

    BCM_TRUNK_ID_SET(trunk_id, pool, group);
    if (need_to_add_new_member_to_pp)
    {
        SHR_IF_ERR_EXIT(dnx_trunk_add_port_to_pp(unit, trunk_id, &added_member_info));
    }

    /** set protection to HW only if member is active */
    if (master_member_info.index >= 0)
    {
        dnx_algo_gpm_gport_phy_info_t phy_gport_info;
        /** get system port info */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, updated_member->backup_member.gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE,
                         &phy_gport_info));
        SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_protection_members_set
                        (unit, pool, group, master_member_info.index, phy_gport_info.sys_port));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_trunk_member_protection_remove(
    int unit,
    uint32 flags,
    int pool,
    int group,
    int member)
{
    int first_index;
    int last_index;
    int count;
    int need_to_remove_old_member_from_pp;
    bcm_trunk_t trunk_id;
    trunk_group_member_info_t master_member_info;
    trunk_group_member_info_t removed_member_info;
    SHR_FUNC_INIT_VARS(unit);

    /** remove protecting from sw db */
    /** get master member */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_get(unit, pool, group, member, &master_member_info));
    /** get protecting member */
    dnx_trunk_sw_db_member_init(unit, &removed_member_info);
    removed_member_info.system_port = master_member_info.protecting_system_port;
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_in_group_find
                    (unit, pool, group, &removed_member_info, &first_index, &last_index, &count));
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_get(unit, pool, group, first_index, &removed_member_info));
    /** Check if need to remove from pp as well */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_is_last_spa_ref
                    (unit, pool, group, first_index, &need_to_remove_old_member_from_pp));
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_protecting_member_delete(unit, pool, group, first_index));

    /** update master to remove protection */
    master_member_info.protecting_system_port = dnx_data_device.general.invalid_system_port_get(unit);
    master_member_info.flags &= ~BCM_TRUNK_MEMBER_WITH_PROTECTION;
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_set(unit, pool, group, member, &master_member_info));

    /** set protection to HW only if member is active */
    if ((master_member_info.index >= 0) && !(flags & DNX_TRUNK_PROTECTION_REMOVE_IGNORE_MEMBER_TABLE))
    {
        SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_protection_members_set
                        (unit, pool, group, master_member_info.index,
                         dnx_data_device.general.invalid_system_port_get(unit)));
    }

    BCM_TRUNK_ID_SET(trunk_id, pool, group);
    if (need_to_remove_old_member_from_pp)
    {
        SHR_IF_ERR_EXIT(dnx_trunk_delete_port_from_pp(unit, trunk_id, &removed_member_info));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_trunk_member_protection_replace(
    int unit,
    int pool,
    int group,
    int member,
    bcm_trunk_member_t * updated_member)
{

    int first_index;
    int last_index;
    int count;
    int added_member_index;
    int need_to_remove_old_member_from_pp;
    int need_to_add_new_member_to_pp;
    bcm_trunk_t trunk_id;
    trunk_group_member_info_t master_member_info;
    trunk_group_member_info_t removed_member_info;
    trunk_group_member_info_t added_member_info;
    SHR_FUNC_INIT_VARS(unit);

    /** get master member */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_get(unit, pool, group, member, &master_member_info));
    /** remove old backup from sw db */
    dnx_trunk_sw_db_member_init(unit, &removed_member_info);
    removed_member_info.system_port = master_member_info.protecting_system_port;
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_in_group_find
                    (unit, pool, group, &removed_member_info, &first_index, &last_index, &count));
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_get(unit, pool, group, first_index, &removed_member_info));
    /** Check if need to remove from pp as well */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_is_last_spa_ref
                    (unit, pool, group, first_index, &need_to_remove_old_member_from_pp));
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_protecting_member_delete(unit, pool, group, first_index));

    /** add new backup to sw db */
    dnx_trunk_sw_db_member_init(unit, &added_member_info);
    added_member_info.system_port = updated_member->backup_member.gport;
    added_member_info.spa_member_id = updated_member->backup_member.spa_id;
    added_member_info.index = INVALID_TM_INDEX_TO_INDICATE_MEMBER_IS_ONLY_PROTECTING;
    added_member_info.flags = updated_member->backup_member.flags;
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_add(unit, pool, group, &added_member_info, &added_member_index));
    /** get added member */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_get(unit, pool, group, added_member_index, &added_member_info));
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_is_last_spa_ref
                    (unit, pool, group, added_member_index, &need_to_add_new_member_to_pp));

    /** update master to have new protection */
    master_member_info.protecting_system_port = updated_member->backup_member.gport;
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_set(unit, pool, group, member, &master_member_info));

    BCM_TRUNK_ID_SET(trunk_id, pool, group);
    if (need_to_add_new_member_to_pp)
    {
        SHR_IF_ERR_EXIT(dnx_trunk_add_port_to_pp(unit, trunk_id, &added_member_info));
    }
    /** set protection to HW only if member is active */
    if (master_member_info.index >= 0)
    {
        dnx_algo_gpm_gport_phy_info_t phy_gport_info;
        /** get system port info */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, updated_member->backup_member.gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE,
                         &phy_gport_info));
        SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_protection_members_set
                        (unit, pool, group, master_member_info.index, phy_gport_info.sys_port));
    }
    if (need_to_remove_old_member_from_pp)
    {
        SHR_IF_ERR_EXIT(dnx_trunk_delete_port_from_pp(unit, trunk_id, &removed_member_info));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_trunk_member_protection_swap(
    int unit,
    int pool,
    int group,
    int member)
{
    int member_index;
    int first_index;
    int last_index;
    int count;
    trunk_group_member_info_t old_master_member_info;
    trunk_group_member_info_t new_master_member_info;
    trunk_group_member_info_t old_backup_member_info;
    trunk_group_member_info_t new_backup_member_info;
    SHR_FUNC_INIT_VARS(unit);

    /** get old master */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_get(unit, pool, group, member, &old_master_member_info));

    /** remove old backup from sw db */
    dnx_trunk_sw_db_member_init(unit, &old_backup_member_info);
    old_backup_member_info.system_port = old_master_member_info.protecting_system_port;
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_in_group_find
                    (unit, pool, group, &old_backup_member_info, &first_index, &last_index, &count));
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_get(unit, pool, group, first_index, &old_backup_member_info));
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_protecting_member_delete(unit, pool, group, first_index));

    /** remove old master from sw db */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_nof_replication_get(unit, pool, group, old_master_member_info.system_port, &count));
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_entry_remove(unit, pool, group, member, (count == 1), FALSE));

    /** add new backup to sw db */
    dnx_trunk_sw_db_member_init(unit, &new_backup_member_info);
    new_backup_member_info.system_port = old_master_member_info.system_port;
    new_backup_member_info.spa_member_id = old_master_member_info.spa_member_id;
    new_backup_member_info.index = INVALID_TM_INDEX_TO_INDICATE_MEMBER_IS_ONLY_PROTECTING;
    new_backup_member_info.flags = BCM_TRUNK_MEMBER_SPA_WITH_ID;
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_add(unit, pool, group, &new_backup_member_info, &member_index));

    /** add new master to sw db */
    dnx_trunk_sw_db_member_init(unit, &new_master_member_info);
    new_master_member_info.system_port = old_master_member_info.protecting_system_port;
    new_master_member_info.protecting_system_port = old_master_member_info.system_port;
    new_master_member_info.index = old_master_member_info.index;
    new_master_member_info.spa_member_id = old_backup_member_info.spa_member_id;
    new_master_member_info.flags = old_master_member_info.flags | BCM_TRUNK_MEMBER_SPA_WITH_ID;
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_add(unit, pool, group, &new_master_member_info, &member_index));
    /** add has protection flag to the new entry, dnx_trunk_sw_db_member_add ignores this flag */
    new_master_member_info.flags |= BCM_TRUNK_MEMBER_WITH_PROTECTION;
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_set(unit, pool, group, member_index, &new_master_member_info));

    /** update master HW to swap only if member is active */
    if (new_master_member_info.index >= 0)
    {
        SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_protection_members_swap
                        (unit, pool, group, new_master_member_info.index));
    }

exit:
    SHR_FUNC_EXIT;
}
