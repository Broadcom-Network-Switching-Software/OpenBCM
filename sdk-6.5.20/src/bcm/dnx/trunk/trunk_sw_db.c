/** \file src/bcm/dnx/trunk/trunk_sw_db.c
 *
 *
 *  This file contains the implementation of sw db functions of
 *  the trunk module.
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TRUNK

/*
 * Include files.
 * {
 */

#include <shared/shrextend/shrextend_debug.h>
#include "trunk_sw_db.h"
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trunk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/swstate/auto_generated/types/trunk_types.h>
#include <soc/dnx/swstate/auto_generated/access/trunk_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <bcm/switch.h>
#include <bcm/trunk.h>
#include <bcm/types.h>

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

/**
 * \brief - initiate group in pool (part of swstate mechanism)
 *
 * \param [in] unit - unit number
 * \param [in] pool_index - pool index
 * \param [in] group_id - group index
 * \param [in] max_nof_members_per_group - max number of members
 *        in group
 * \return
 *      shr_error_e
 * \remark
 *   * additional fields in group structure are initiated to 0.
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_sw_db_group_init(
    int unit,
    int pool_index,
    int group_id,
    int max_nof_members_per_group)
{
    int nof_cores;

    SHR_FUNC_INIT_VARS(unit);

    /** allocate members array */
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.members.alloc(unit, pool_index, group_id, max_nof_members_per_group));

    /** allocate pp ports array - each group can have upto one pp port per core. */
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.pp_ports.alloc(unit, pool_index, group_id, nof_cores));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - initiate pool in trunk software database (part of
 *        swstate mechanism)
 *
 * \param [in] unit - unit number
 * \param [in] pool_index - pool index
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_sw_db_pool_init(
    int unit,
    int pool_index)
{
    const dnx_data_trunk_parameters_pool_info_t *pool_info;
    int groups_in_pool;
    int group_index;
    int members_in_group;

    SHR_FUNC_INIT_VARS(unit);

    /** get pool_info from DNX_DATA, relevant info is members per group and groups per pool. */
    pool_info = dnx_data_trunk.parameters.pool_info_get(unit, pool_index);
    groups_in_pool = pool_info->max_nof_groups_in_pool;
    members_in_group = pool_info->max_nof_members_in_group;

    /** allocate groups, loop over groups and initiate each group */
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.alloc(unit, pool_index, groups_in_pool));
    for (group_index = 0; group_index < groups_in_pool; ++group_index)
    {
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_init(unit, pool_index, group_index, members_in_group));
    }

exit:
    SHR_FUNC_EXIT;
}

static void
trunk_system_port_entry_t_init(
    trunk_system_port_entry_t * entry)
{
    entry->flags = 0;
    entry->trunk_id = TRUNK_SW_DB_INVALID_TRUNK_ID;
}

/**
 * \brief - init trunk system port db in swstate
 *
 * \param [in] unit - unit number
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_sw_db_trunk_system_port_db_init(
    int unit)
{
    int max_nof_system_ports;
    trunk_system_port_entry_t invalid_entry;
    SHR_FUNC_INIT_VARS(unit);

    /** allocate system ports array and init them to Invalid values */
    max_nof_system_ports = dnx_data_device.general.max_nof_system_ports_get(unit);
    SHR_IF_ERR_EXIT(trunk_db.trunk_system_port_db.trunk_system_port_entries.alloc(unit));
    trunk_system_port_entry_t_init(&invalid_entry);
    SHR_IF_ERR_EXIT(trunk_db.trunk_system_port_db.trunk_system_port_entries.
                    range_fill(unit, 0, max_nof_system_ports, invalid_entry));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - deinit trunk_db (free memory)
 *
 * \param [in] unit - unit number
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_trunk_sw_db_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * sw state module deinit is done automatically at device deinit
     */

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - initiate trunk software database (part of swstate
 *        mechanism)
 *
 * \param [in] unit - unit number
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_trunk_sw_db_init(
    int unit)
{
    uint8 trunk_sw_db_is_init;
    int nof_pools;
    int pool_index;

    SHR_FUNC_INIT_VARS(unit);

    /** Initiate swstate (sw_db) if it was not initiated yet. */
    SHR_IF_ERR_EXIT(trunk_db.is_init(unit, &trunk_sw_db_is_init));
    if (trunk_sw_db_is_init)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "trunk_db is already initiated \n");
    }
    SHR_IF_ERR_EXIT(trunk_db.init(unit));

    /** get nof_pools from DNX_DATA, allocate pools, loop over nof_pools and initiate each pool */
    nof_pools = dnx_data_trunk.parameters.nof_pools_get(unit);
    SHR_IF_ERR_EXIT(trunk_db.pools.alloc(unit, nof_pools));
    for (pool_index = 0; pool_index < nof_pools; ++pool_index)
    {
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_pool_init(unit, pool_index));
    }

    /** init trunk_system_port_db */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_trunk_system_port_db_init(unit));

exit:
    SHR_FUNC_EXIT;
}

static void
dnx_trunk_sw_db_member_init(
    int unit,
    trunk_group_member_info_t * member)
{
    /** no flags  */
    member->flags = 0;
    /** invalid index */
    member->index = INVALID_TM_INDEX;
    /** invalid system port */
    member->system_port = dnx_data_device.general.invalid_system_port_get(unit);

}

/*
 * see header file
 */
shr_error_e
dnx_trunk_sw_db_group_reset(
    int unit,
    int pool_index,
    int group_index,
    int is_currently_created)
{
    trunk_group_member_info_t member_info;
    int nof_members_in_group;
    int nof_cores;

    SHR_FUNC_INIT_VARS(unit);

    /** init all group members to invalid */
    dnx_trunk_sw_db_member_init(unit, &member_info);
    nof_members_in_group = dnx_data_trunk.parameters.pool_info_get(unit, pool_index)->max_nof_members_in_group;
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.members.range_fill(unit, pool_index, group_index,
                                                             0, nof_members_in_group, member_info));

    if (is_currently_created)
    {
        /** init all pp ports to invalid */
        nof_cores = dnx_data_device.general.nof_cores_get(unit);
        SHR_IF_ERR_EXIT(trunk_db.pools.groups.pp_ports.range_fill(unit, pool_index, group_index, 0, nof_cores,
                                                                  dnx_data_port.general.nof_pp_ports_get(unit)));
        /** set header type to invalid value */
        SHR_IF_ERR_EXIT(trunk_db.pools.groups.
                        in_header_type.set(unit, pool_index, group_index, BCM_SWITCH_PORT_HEADER_TYPE_NONE));
        SHR_IF_ERR_EXIT(trunk_db.pools.groups.
                        out_header_type.set(unit, pool_index, group_index, BCM_SWITCH_PORT_HEADER_TYPE_NONE));

        /** set flags to 0 */
        SHR_IF_ERR_EXIT(trunk_db.pools.groups.flags.set(unit, pool_index, group_index, 0));

        /** set master trunk id to 0 */
        SHR_IF_ERR_EXIT(trunk_db.pools.groups.
                        master_trunk_id.set(unit, pool_index, group_index, TRUNK_SW_DB_INVALID_TRUNK_ID));
    }

    /** init the other group global attributes */
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.last_member_added_index.set(unit, pool_index, group_index, -1));
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.psc.set(unit, pool_index, group_index, -1));
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.psc_profile_id.set(unit, pool_index, group_index, -1));
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.nof_members.set(unit, pool_index, group_index, 0));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_trunk_sw_db_trunk_system_port_db_set(
    int unit,
    uint32 system_port,
    bcm_trunk_t trunk_id,
    uint32 flags)
{
    trunk_system_port_entry_t entry;
    SHR_FUNC_INIT_VARS(unit);

    /** update entry to sw db */
    entry.trunk_id = trunk_id;
    entry.flags = flags;
    SHR_IF_ERR_EXIT(trunk_db.trunk_system_port_db.trunk_system_port_entries.set(unit, system_port, &entry));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_trunk_sw_db_trunk_system_port_db_get(
    int unit,
    uint32 system_port,
    bcm_trunk_t * trunk_id,
    uint32 *flags)
{
    trunk_system_port_entry_t entry;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(trunk_db.trunk_system_port_db.trunk_system_port_entries.get(unit, system_port, &entry));
    *trunk_id = entry.trunk_id;
    *flags = entry.flags;

exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_trunk_sw_db_group_pp_port_core_bitmap_get(
    int unit,
    int pool,
    int group,
    uint32 *current_core_bitmap)
{
    int core;
    int nof_cores;
    uint32 pp_port;
    uint32 core_bit;

    SHR_FUNC_INIT_VARS(unit);

    *current_core_bitmap = 0;
    core_bit = 1;
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    for (core = 0; core < nof_cores; ++core)
    {
        SHR_IF_ERR_EXIT(trunk_db.pools.groups.pp_ports.get(unit, pool, group, core, &pp_port));
        if (pp_port != dnx_data_port.general.nof_pp_ports_get(unit))
        {
            *current_core_bitmap |= core_bit;
        }
        core_bit <<= 1;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_trunk_sw_db_group_pp_port_set(
    int unit,
    int pool_index,
    int group_index,
    int core,
    uint32 pp_port)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.pp_ports.set(unit, pool_index, group_index, core, pp_port));
exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_trunk_sw_db_group_pp_port_get(
    int unit,
    int pool_index,
    int group_index,
    int core,
    uint32 *pp_port)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.pp_ports.get(unit, pool_index, group_index, core, pp_port));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - find next free member in group, if group is full -
 *        fail
 *
 * \param [in] unit - uint number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] free_member - returned free member index
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_sw_db_next_free_member_find(
    int unit,
    int pool,
    int group,
    int *free_member)
{
    int max_nof_members_in_group;
    int remaining_members_to_check;
    int last_added;
    uint32 system_port;
    int invalid_system_port;
    int member;
    SHR_FUNC_INIT_VARS(unit);

    max_nof_members_in_group = remaining_members_to_check =
        dnx_data_trunk.parameters.pool_info_get(unit, pool)->max_nof_members_in_group;
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.last_member_added_index.get(unit, pool, group, &last_added));
    invalid_system_port = dnx_data_device.general.invalid_system_port_get(unit);
    /*
     * the members in the trunk sw db are not neccerily consicuintial, therefore we need to iterate over all the
     * members. as an optimization it is suffice to do si untill we iterated over the remaining members
     */
    for (member = last_added + 1; remaining_members_to_check > 0; ++member, --remaining_members_to_check)
    {
        member %= max_nof_members_in_group;
        SHR_IF_ERR_EXIT(trunk_db.pools.groups.members.system_port.get(unit, pool, group, member, &system_port));
        if (system_port == invalid_system_port)
        {
            *free_member = member;
            SHR_EXIT();
        }
    }
    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "trunk group %d in pool %d is full\n", group, pool);
exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_trunk_sw_db_member_add(
    int unit,
    int pool,
    int group,
    trunk_group_member_info_t * member_info,
    int *member_index)
{
    bcm_trunk_t trunk_id;
    uint32 flags;
    uint32 sys_port;
    int first;
    int last;
    int count;
    int free_member = 0;
    int nof_members;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_in_group_find(unit, pool, group, member_info, &first, &last, &count));
    sys_port = BCM_GPORT_SYSTEM_PORT_ID_GET(member_info->system_port);
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_trunk_system_port_db_get(unit, sys_port, &trunk_id, &flags));
    /*
     * there are 3 possible scenarios for adding a member. member is added and is first replication - need
     * to add it to both system port db and member db. member is added and is not first replication- need to add it
     * only to member db. member is already found as disabled - need only to modify existing entries in the system port
     * and member db
     */
    /** if member needs to be added */
    if ((flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE) == 0)
    {
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_next_free_member_find(unit, pool, group, &free_member));
        SHR_IF_ERR_EXIT(trunk_db.pools.groups.members.set(unit, pool, group, free_member, member_info));
        SHR_IF_ERR_EXIT(trunk_db.pools.groups.nof_members.get(unit, pool, group, &nof_members));
        SHR_IF_ERR_EXIT(trunk_db.pools.groups.nof_members.set(unit, pool, group, nof_members + 1));
        SHR_IF_ERR_EXIT(trunk_db.pools.groups.last_member_added_index.set(unit, pool, group, free_member));
        /** Member index is the index of the next free member allocated */
        *member_index = free_member;
        /** if member had no replications before */
        if (count == 0)
        {
            BCM_TRUNK_ID_SET(trunk_id, pool, group);
            SHR_IF_ERR_EXIT(dnx_trunk_sw_db_trunk_system_port_db_set(unit, sys_port, trunk_id, member_info->flags));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(trunk_db.pools.groups.members.set(unit, pool, group, first, member_info));
        SHR_IF_ERR_EXIT(trunk_db.trunk_system_port_db.trunk_system_port_entries.
                        flags.set(unit, sys_port, member_info->flags));
        /** The existing members index should be used */
        *member_index = first;
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_trunk_sw_db_member_get(
    int unit,
    int pool,
    int group,
    int member,
    trunk_group_member_info_t * member_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.members.get(unit, pool, group, member, member_info));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - remove an entry from the sw db. the removal of the
 *        entry is done both from the group and from the system
 *        port db if needed. from the group is done always, from
 *        the sys port db only for the last replication.
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] member - member index
 * \param [in] is_last_replication - indication that this is the
 *        last replication of this member
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_sw_db_entry_remove(
    int unit,
    int pool,
    int group,
    int member,
    int is_last_replication)
{
    trunk_group_member_info_t member_info;
    uint32 gport_sys_port;
    int nof_members;
    SHR_FUNC_INIT_VARS(unit);
    /** get gport_system_port to access the trunk sys port db */
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.members.system_port.get(unit, pool, group, member, &gport_sys_port));
    /** overrun entry in member sw db with an empty one */
    dnx_trunk_sw_db_member_init(unit, &member_info);
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.members.set(unit, pool, group, member, &member_info));
    /** if the replication of the member was removed, need to
     *  clear it from the system port db as well */
    if (is_last_replication)
    {
        trunk_system_port_entry_t entry;
        uint32 sys_port;
        trunk_system_port_entry_t_init(&entry);
        sys_port = BCM_GPORT_SYSTEM_PORT_ID_GET(gport_sys_port);
        SHR_IF_ERR_EXIT(trunk_db.trunk_system_port_db.trunk_system_port_entries.set(unit, sys_port, &entry));
    }
    /** update nof members */
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.nof_members.get(unit, pool, group, &nof_members));
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.nof_members.set(unit, pool, group, nof_members - 1));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - search a trunk group for the member with the largest
 *        TM index
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] member_with_greatest_tm_index - returned index of
 *        member with greatest TM index
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_sw_db_member_with_greatest_tm_index_find(
    int unit,
    int pool,
    int group,
    int *member_with_greatest_tm_index)
{
    int max_nof_members;
    int member;
    int max_tm_index = -1;
    int tm_index;
    SHR_FUNC_INIT_VARS(unit);
    max_nof_members = dnx_data_trunk.parameters.pool_info_get(unit, pool)->max_nof_members_in_group;
    for (member = 0; member < max_nof_members; ++member)
    {
        SHR_IF_ERR_EXIT(trunk_db.pools.groups.members.index.get(unit, pool, group, member, &tm_index));
        if (tm_index > max_tm_index)
        {
            max_tm_index = tm_index;
            *member_with_greatest_tm_index = member;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - swap TM indexes of the member with the largest one
 *        and the member that you want to delete
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] member - index of the member you want to delete
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_tm_indexes_to_allow_deleting_align(
    int unit,
    int pool,
    int group,
    int member)
{
    int member_to_swap = -1;
    int index;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_with_greatest_tm_index_find(unit, pool, group, &member_to_swap));
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.members.index.get(unit, pool, group, member, &index));
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.members.index.set(unit, pool, group, member_to_swap, index));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - disable given member in both sw db and system port
 *        db
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] member - index of the member to disable
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_member_disable(
    int unit,
    int pool,
    int group,
    int member)
{
    uint32 gport_sys_port;
    uint32 sys_port;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.members.system_port.get(unit, pool, group, member, &gport_sys_port));
    sys_port = BCM_GPORT_SYSTEM_PORT_ID_GET(gport_sys_port);
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.members.
                    flags.set(unit, pool, group, member, BCM_TRUNK_MEMBER_EGRESS_DISABLE));
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.members.index.set(unit, pool, group, member, INVALID_TM_INDEX));
    SHR_IF_ERR_EXIT(trunk_db.trunk_system_port_db.trunk_system_port_entries.
                    flags.set(unit, sys_port, BCM_TRUNK_MEMBER_EGRESS_DISABLE));
exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_trunk_sw_db_member_delete(
    int unit,
    int pool,
    int group,
    int member,
    int nof_replications,
    int need_disable_instead_of_deleting)
{
    uint32 curr_flags;
    int disabled_member;
    int deleting_a_regular_member;
    int deleting_a_disabled_member;
    int disabling_a_regular_member;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(trunk_db.pools.groups.members.flags.get(unit, pool, group, member, &curr_flags));
    disabled_member = (curr_flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE);
    deleting_a_regular_member = !need_disable_instead_of_deleting && !disabled_member;
    deleting_a_disabled_member = !need_disable_instead_of_deleting && disabled_member;
    disabling_a_regular_member = need_disable_instead_of_deleting && !disabled_member;

    /*
     * there are 3 valid scenarios at this point. deleting a disabled member, deleting a regular member and disabling
     * a regular member. every attampt to perform other actions should have already been blocked from above and so we
     * don't check for it now. additional assumptions are that in case of disabling a regular member we can have only
     * a single replication from it - this is also verified in calling functions. prior to deleting a member from the
     * sw db, we must align it in such a way that we're deleting the member with greatest TM index - meaning that we
     * are actually required in certain cases to swap between member's TM index.
     */
    if (deleting_a_disabled_member)
    {
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_entry_remove(unit, pool, group, member, TRUE));
    }
    else if (deleting_a_regular_member)
    {
        int is_last_replication = (nof_replications == 1) ? TRUE : FALSE;
        SHR_IF_ERR_EXIT(dnx_trunk_tm_indexes_to_allow_deleting_align(unit, pool, group, member));
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_entry_remove(unit, pool, group, member, is_last_replication));
    }
    else if (disabling_a_regular_member)
    {
        SHR_IF_ERR_EXIT(dnx_trunk_tm_indexes_to_allow_deleting_align(unit, pool, group, member));
        SHR_IF_ERR_EXIT(dnx_trunk_member_disable(unit, pool, group, member));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "input doesn't fit any valid configuration\n");
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_trunk_sw_db_group_last_member_added_set(
    int unit,
    int pool,
    int group,
    int last_member_added)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.last_member_added_index.set(unit, pool, group, last_member_added));
exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_trunk_sw_db_group_last_member_added_get(
    int unit,
    int pool,
    int group,
    int *last_member_added)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.last_member_added_index.get(unit, pool, group, last_member_added));
exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_trunk_sw_db_member_in_group_find(
    int unit,
    int pool,
    int group,
    trunk_group_member_info_t * member_info,
    int *first_replication_index,
    int *last_replication_index,
    int *count)
{
    trunk_group_member_info_t curr_member_info;
    int max_nof_members;
    int member;
    int first_rep_tm_index;
    int last_rep_tm_index;

    SHR_FUNC_INIT_VARS(unit);
    max_nof_members = dnx_data_trunk.parameters.pool_info_get(unit, pool)->max_nof_members_in_group;
    *first_replication_index = *last_replication_index = max_nof_members;
    first_rep_tm_index = max_nof_members;
    last_rep_tm_index = INVALID_TM_INDEX;
    *count = 0;
    for (member = 0; member < max_nof_members; ++member)
    {
        SHR_IF_ERR_EXIT(trunk_db.pools.groups.members.get(unit, pool, group, member, &curr_member_info));
        if (curr_member_info.system_port == member_info->system_port)
        {
            ++(*count);
         /** When adding disabled members, they are considered invalid,
          *  so the index in the member_info is set as  INVALID_TM_INDEX.
          *  In order to be able to properly find and delete these
          *  members we need to be able to get their
          *  last_replication_index, so by setting the condition here
          *  to >= we allow invalid members to also be found */
            if (curr_member_info.index >= last_rep_tm_index)
            {
                last_rep_tm_index = curr_member_info.index;
                *last_replication_index = member;
            }
            if (curr_member_info.index < first_rep_tm_index)
            {
                first_rep_tm_index = curr_member_info.index;
                *first_replication_index = member;
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_trunk_sw_db_member_index_in_group_find(
    int unit,
    int pool,
    int group,
    trunk_group_member_info_t * member_info)
{
    trunk_group_member_info_t curr_member_info;
    int max_nof_members;
    int member;

    SHR_FUNC_INIT_VARS(unit);

    max_nof_members = dnx_data_trunk.parameters.pool_info_get(unit, pool)->max_nof_members_in_group;

    member_info->index = INVALID_TM_INDEX;

    for (member = 0; member < max_nof_members; ++member)
    {
        SHR_IF_ERR_EXIT(trunk_db.pools.groups.members.get(unit, pool, group, member, &curr_member_info));
        if (curr_member_info.system_port == member_info->system_port)
        {
            member_info->index = member;
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_trunk_sw_db_nof_members_in_group_get(
    int unit,
    int pool,
    int group,
    int *count)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.nof_members.get(unit, pool, group, count));
exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_trunk_sw_db_group_is_used_get(
    int unit,
    int pool,
    int group,
    int *is_used)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.in_use.get(unit, pool, group, is_used));
exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_trunk_sw_db_group_is_used_set(
    int unit,
    int pool,
    int group,
    int is_used)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.in_use.set(unit, pool, group, is_used));
exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_trunk_sw_db_header_types_set(
    int unit,
    int pool,
    int group,
    uint32 in_header_type,
    uint32 out_header_type)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.in_header_type.set(unit, pool, group, in_header_type));
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.out_header_type.set(unit, pool, group, out_header_type));
exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_trunk_sw_db_header_types_get(
    int unit,
    int pool,
    int group,
    uint32 *in_header_type,
    uint32 *out_header_type)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.in_header_type.get(unit, pool, group, in_header_type));
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.out_header_type.get(unit, pool, group, out_header_type));
exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_trunk_sw_db_group_psc_get(
    int unit,
    int pool,
    int group,
    int *psc,
    int *psc_profile_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.psc.get(unit, pool, group, psc));
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.psc_profile_id.get(unit, pool, group, psc_profile_id));
exit:
    SHR_FUNC_EXIT;
}

/** expectations are that this will update the sw db with the
 *  bcm encoded PSC, the psc_profile_id is relevant for all
 *  PSC, including SMD and M&D, in those cases the profile
 *  will actually represent (number_of_enabled_members - 1) */
/*
 * see header file
 */
shr_error_e
dnx_trunk_sw_db_group_psc_set(
    int unit,
    int pool,
    int group,
    int psc,
    int psc_profile_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.psc.set(unit, pool, group, psc));
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.psc_profile_id.set(unit, pool, group, psc_profile_id));
exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_trunk_sw_db_nof_enabled_members_get(
    int unit,
    int pool,
    int group,
    int *nof_enabled_members)
{
    trunk_group_member_info_t member_info;
    int member;
    int max_nof_members;
    int invalid_system_port;
    SHR_FUNC_INIT_VARS(unit);
    *nof_enabled_members = 0;
    max_nof_members = dnx_data_trunk.parameters.pool_info_get(unit, pool)->max_nof_members_in_group;
    invalid_system_port = dnx_data_device.general.invalid_system_port_get(unit);
    for (member = 0; member < max_nof_members; ++member)
    {
        SHR_IF_ERR_EXIT(trunk_db.pools.groups.members.get(unit, pool, group, member, &member_info));
        if ((member_info.system_port == invalid_system_port || member_info.index == INVALID_TM_INDEX))
        {
            continue;
        }

        /** if member is enabled, count it */
        if ((member_info.flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE) == 0)
        {
            ++(*nof_enabled_members);
        }

    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_trunk_sw_db_nof_replication_get(
    int unit,
    int pool,
    int group,
    uint32 system_port_gport,
    int *nof_replications)
{
    int first;
    int last;
    trunk_group_member_info_t member_info;

    SHR_FUNC_INIT_VARS(unit);
    member_info.system_port = system_port_gport;
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_in_group_find(unit, pool, group, &member_info,
                                                         &first, &last, nof_replications));
exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_trunk_sw_db_unique_members_info_get(
    int unit,
    int pool,
    int group,
    dnx_trunk_unique_member_t * unique_members_arr,
    int arr_size,
    int *nof_unique_members)
{
    uint32 system_port;
    int max_nof_members;
    int nof_members_in_group;
    uint32 invalid_system_port;
    int member;
    int unique_member;
    int tm_index;

    SHR_FUNC_INIT_VARS(unit);
    max_nof_members = dnx_data_trunk.parameters.pool_info_get(unit, pool)->max_nof_members_in_group;
    SHR_IF_ERR_EXIT(trunk_db.pools.groups.nof_members.get(unit, pool, group, &nof_members_in_group));
    invalid_system_port = dnx_data_device.general.invalid_system_port_get(unit);
    *nof_unique_members = 0;
    sal_memset(unique_members_arr, 0, sizeof(unique_members_arr[0]) * arr_size);

    /** sanity check, if the given array is smaller than the number of members in the group return error */
    /** this is over defensive, we actually need only to check that there sufficint size for unique members */
    if (arr_size < nof_members_in_group)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "input array size(%d) is smaller than number of members in group(%d)",
                     arr_size, nof_members_in_group);
    }

    /** iterate members in group */
    for (member = 0; member < max_nof_members; ++member)
    {
        /** skip invalid members */
        SHR_IF_ERR_EXIT(trunk_db.pools.groups.members.system_port.get(unit, pool, group, member, &system_port));
        if (system_port == invalid_system_port)
        {
            continue;
        }

        /** if member exists already as unique member increase number of replications */
        for (unique_member = 0; unique_member < *nof_unique_members; ++unique_member)
        {
            if (system_port == unique_members_arr[unique_member].system_port)
            {
                ++unique_members_arr[unique_member].nof_replications;
                break;
            }
        }

        /** if not first replication continue */
        if (unique_member < *nof_unique_members)
        {
            continue;
        }

        /** for first replication of a unique member, add it to the array */
        unique_members_arr[unique_member].system_port = system_port;
        unique_members_arr[unique_member].nof_replications = 1;
        SHR_IF_ERR_EXIT(trunk_db.pools.groups.members.index.get(unit, pool, group, member, &tm_index));
        unique_members_arr[unique_member].is_enabled = (tm_index != INVALID_TM_INDEX);
        ++(*nof_unique_members);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_trunk_sw_db_group_flags_get(
    int unit,
    int pool,
    int group,
    uint32 *flags)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(trunk_db.pools.groups.flags.get(unit, pool, group, flags));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_trunk_sw_db_group_flags_set(
    int unit,
    int pool,
    int group,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(trunk_db.pools.groups.flags.set(unit, pool, group, flags));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_trunk_sw_db_group_master_trunk_id_get(
    int unit,
    int pool,
    int group,
    bcm_trunk_t * tid)
{
    uint32 trunk_flags;

    SHR_FUNC_INIT_VARS(unit);

    /** master trunk is only relevant for virtual trunks */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_flags_get(unit, pool, group, &trunk_flags));
    if ((trunk_flags & BCM_TRUNK_FLAG_VP) == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "master trunk is only supported for virtual trunks");
    }

    SHR_IF_ERR_EXIT(trunk_db.pools.groups.master_trunk_id.get(unit, pool, group, tid));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_trunk_sw_db_group_master_trunk_id_set(
    int unit,
    int pool,
    int group,
    bcm_trunk_t tid)
{
    uint32 trunk_flags;

    SHR_FUNC_INIT_VARS(unit);

    /** master trunk is only relevant for virtual trunks */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_flags_get(unit, pool, group, &trunk_flags));
    if ((trunk_flags & BCM_TRUNK_FLAG_VP) == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "master trunk is only supported for virtual trunks");
    }

    SHR_IF_ERR_EXIT(trunk_db.pools.groups.master_trunk_id.set(unit, pool, group, tid));

exit:
    SHR_FUNC_EXIT;
}
