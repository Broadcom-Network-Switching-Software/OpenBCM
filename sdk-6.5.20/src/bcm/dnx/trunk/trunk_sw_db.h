/** \file src/bcm/dnx/trunk/trunk_sw_db.h
 *
 * Internal DNX TRUNK sw db APIs to be used in trunk module
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _BCM_DNX_TRUNK_TRUNKSWDB_H_INCLUDED
/*
 * {
 */
#define _BCM_DNX_TRUNK_TRUNKSWDB_H_INCLUDED

/**
 * \brief This file is only used by DNX (JR2 family). Including it by
 * software that is not specific to DNX is an error.
 */
#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <soc/dnx/swstate/auto_generated/types/trunk_types.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include "trunk_temp_structs_to_skip_papi.h"
#include <bcm/types.h>
#include <bcm_int/dnx/trunk/trunk.h>

#define TRUNK_SW_DB_INVALID_TRUNK_ID    (-1)
#define INVALID_TM_INDEX                (-1)

shr_error_e dnx_trunk_sw_db_init(
    int unit);

shr_error_e dnx_trunk_sw_db_deinit(
    int unit);

/**
 * \brief - add givem member to trunk sw db. next free available
 *        index is searched and returned back. if added member
 *        is disabled search first if it already exists in the
 *        sw db and update existing entry
 *
 * \param [in] unit - uint number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] member_info - member info
 * \param [in] member_index - member's assigned index
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_member_add(
    int unit,
    int pool,
    int group,
    trunk_group_member_info_t * member_info,
    int *member_index);

/**
 * \brief - get trunk member info
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] member - member index ( pp index )
 * \param [in] member_info - recieved info
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_member_get(
    int unit,
    int pool,
    int group,
    int member,
    trunk_group_member_info_t * member_info);

/**
 * \brief - delete a member from trunk sw db
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] member - member index
 * \param [in] nof_replications - number of replications of
 *        deleted member
 * \param [in] need_disable_instead_of_deleting - indication
 *        that the member needs to be disabled instead of
 *        removed
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_member_delete(
    int unit,
    int pool,
    int group,
    int member,
    int nof_replications,
    int need_disable_instead_of_deleting);

/**
 * \brief - find a member in a given group, include information
 *        of indexes of replications with first and last
 *        TM index, in addition count of members
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] member_info - member info, relevant input is
 *        system port
 * \param [in] first_replication_index - received index of
 *        member with first TM index
 * \param [in] last_replication_index - recieved index of member
 *        with last TM index
 * \param [in] count - number of replications found of this
 *        member
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_member_in_group_find(
    int unit,
    int pool,
    int group,
    trunk_group_member_info_t * member_info,
    int *first_replication_index,
    int *last_replication_index,
    int *count);

/**
 * \brief - find the member index in a given group
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in,out] member_info - the system port of member
 *   As output - index of the member inside the trunk group
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_member_index_in_group_find(
    int unit,
    int pool,
    int group,
    trunk_group_member_info_t * member_info);

/**
 * \brief - get number of members in trunk group
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] count - recieved number of members
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_nof_members_in_group_get(
    int unit,
    int pool,
    int group,
    int *count);

/**
 * \brief - get group is used indication
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] is_used - recieved indication if group is used
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_group_is_used_get(
    int unit,
    int pool,
    int group,
    int *is_used);

/**
 * \brief - set group is used indication
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] is_used - value to set
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_group_is_used_set(
    int unit,
    int pool,
    int group,
    int is_used);

/**
 * \brief - set group header types
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] in_header_type - value to set
 * \param [in] out_header_type - value to set
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_header_types_set(
    int unit,
    int pool,
    int group,
    uint32 in_header_type,
    uint32 out_header_type);

/**
 * \brief - get group header types
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [out] in_header_type - retrieved value
 * \param [out] out_header_type - retrieved value
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_header_types_get(
    int unit,
    int pool,
    int group,
    uint32 *in_header_type,
    uint32 *out_header_type);

/**
 * \brief - get port selection criteria of given trunk group
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] psc - retrieved port selection criteria
 * \param [in] psc_profile_id - retrieved PSC profile id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_group_psc_get(
    int unit,
    int pool,
    int group,
    int *psc,
    int *psc_profile_id);

/**
 * \brief - set port selection criteria of given trunk group
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] psc - port selection criteria
 * \param [in] psc_profile_id - PSC profile id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_group_psc_set(
    int unit,
    int pool,
    int group,
    int psc,
    int psc_profile_id);

/**
 * \brief - get pp port for core x in trunk group
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] core - core number
 * \param [in] pp_port - retrieved pp port
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_group_pp_port_get(
    int unit,
    int pool,
    int group,
    int core,
    uint32 *pp_port);

/**
 * \brief - set pp port for core x in trunk group
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] core - core number
 * \param [in] pp_port - pp port to set
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_group_pp_port_set(
    int unit,
    int pool,
    int group,
    int core,
    uint32 pp_port);

/**
 * \brief - reset a group to its initial state after creating
 *        it. the group will contain 0 members, and its PSC will
 *        be set to an invalid one.
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] is_currently_created - indication if the reset
 *        required is during creation of the group, if so needs
 *        to reset pp ports and header type as well.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_group_reset(
    int unit,
    int pool,
    int group,
    int is_currently_created);

/**
 * \brief - populate current_core_bitmap according with the
 *        cores to which a pp port is allocated in this trunk
 *        group.
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] current_core_bitmap - ptr to returned core bitmap
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_group_pp_port_core_bitmap_get(
    int unit,
    int pool,
    int group,
    uint32 *current_core_bitmap);

/**
 * \brief - add an entry to trunk system ports db
 *
 * \param [in] unit - unit number
 * \param [in] system_port - (valid) system port
 * \param [in] trunk_id - trunk ID
 * \param [in] flags - flags
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_trunk_system_port_db_set(
    int unit,
    uint32 system_port,
    bcm_trunk_t trunk_id,
    uint32 flags);

/**
 * \brief - set the index of last member added to the trunk
 *        group
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] last_member_added - index to update in last
 *        member added
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_group_last_member_added_set(
    int unit,
    int pool,
    int group,
    int last_member_added);

/**
 * \brief - get the index of last member add to the trunk group
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] last_member_added - recieved value of last member
 *        added
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_group_last_member_added_get(
    int unit,
    int pool,
    int group,
    int *last_member_added);

/**
 * \brief - get number of enabled members in given trunk group
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] nof_enabled_members - retrieved number of enabled
 *        members in group
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_nof_enabled_members_get(
    int unit,
    int pool,
    int group,
    int *nof_enabled_members);

/**
 * \brief - get number of replications of a system port in a
 *        trunk group
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] system_port_gport - system port to count
 * \param [in] nof_replications - retrieved number of
 *        replication
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_nof_replication_get(
    int unit,
    int pool,
    int group,
    uint32 system_port_gport,
    int *nof_replications);

/**
 * \brief - get information on unique members in a trunk
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] unique_members_arr - returned array of unique
 *        members
 * \param [in] arr_size - array size
 * \param [in] nof_unique_members - returned number of unique
 *        members
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_unique_members_info_get(
    int unit,
    int pool,
    int group,
    dnx_trunk_unique_member_t * unique_members_arr,
    int arr_size,
    int *nof_unique_members);

/**
 * \brief - get flags for a trunk
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [out] flags - returned flags
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_group_flags_get(
    int unit,
    int pool,
    int group,
    uint32 *flags);

/**
 * \brief - set flags for a trunk
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] flags - flags to set
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_group_flags_set(
    int unit,
    int pool,
    int group,
    uint32 flags);

/**
 * \brief - get master trunk id for a virtual trunk
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [out] tid - returned master trunk id
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_group_master_trunk_id_get(
    int unit,
    int pool,
    int group,
    bcm_trunk_t * tid);

/**
 * \brief - set master trunk id for a virtual trunk
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] tid - master trunk id
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_group_master_trunk_id_set(
    int unit,
    int pool,
    int group,
    bcm_trunk_t tid);

#endif /* _BCM_DNX_TRUNK_TRUNKSWDB_H_INCLUDED */
