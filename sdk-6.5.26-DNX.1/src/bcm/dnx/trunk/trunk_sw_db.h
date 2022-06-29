/** \file src/bcm/dnx/trunk/trunk_sw_db.h
 *
 * Internal DNX TRUNK sw db APIs to be used in trunk module
 *
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
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
#error "This file is for use by DNX family only!"
#endif

#include <soc/dnx/swstate/auto_generated/types/trunk_types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include "trunk_temp_structs_to_skip_papi.h"
#include <bcm/types.h>
#include <bcm_int/dnx/trunk/trunk.h>

#define TRUNK_SW_DB_INVALID_TRUNK_ID                (-1)
#define INVALID_TM_INDEX                            (-1)
#define INVALID_TM_INDEX_BUT_NEED_ALLOCATE_ONE      (-2)
#define INVALID_SPA_MEMBER_ID                       (-1)

shr_error_e dnx_trunk_sw_db_init(
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
 * \param [in,out] member_info - member info, member_spa_id field is an output of this function
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
 * \param [out] member_info - recieved info
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
 * \param [out] first_replication_index - received index of
 *        member with first TM index
 * \param [out] last_replication_index - recieved index of member
 *        with last TM index
 * \param [out] count - number of replications found of this
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
 * \brief - find the member info for member with given spa member id
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in,out] member_info - in parameter is the spa_member_id, out parameter is system port
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * if match was not found member_info is set to invalid values
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_spa_member_id_in_group_find(
    int unit,
    int pool,
    int group,
    trunk_group_member_info_t * member_info);

/**
 * \brief - find members in a given group based on supplied criteria.
 *          the criteria can include system port, tm_index, spa_id, member_flags or any combination of them.
 *          If a criterion should not be included in the search it should be passed as its invalid value.
 *          meaning:
 *           - for member_info->system_port use dnx_data_device.general.invalid_system_port
 *           - for member_info->index use INVALID_TM_INDEX
 *           - for member_info->spa_member_id use INVALID_SPA_MEMBER_ID
 *           - for member_info->flags use member_flags_mask = 0
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] member_info - member info, found members are matched against this info
 * \param [in] member_flags_mask -  a mask to indicate which member flags are relevant for the matching criteria.
 *                                  if a flag is set in the mask it will be checked as part of the matching criteria.
 *                                  if the mask is 0, flags are not checked at all as part of the matching criteria.
 * \param [in] arr_size - max size of the array provided, members indexes will be returned only up to the size of the array
 * \param [out] found_index_arr - array with sw db indexes of found members
 * \param [out] count - number of matched members found
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_member_in_group_per_criteria_find(
    int unit,
    int pool,
    int group,
    trunk_group_member_info_t * member_info,
    uint32 member_flags_mask,
    int arr_size,
    int *found_index_arr,
    int *count);

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
 * \param [out] psc - retrieved port selection criteria
 * \param [out] psc_profile_id - retrieved PSC profile id
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
 * \brief - limit size of resource manager for trunk group. the
 *          resource manager limited size is determined by provided
 *          max_nof_members. currently it is used for tm-indexes.
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] max_nof_members - Max number of members supported by this group.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_group_res_manager_size_limit(
    int unit,
    int pool,
    int group,
    int max_nof_members);

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
 * \brief - get array of enabled members in given trunk group
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] arr_size - input array size
 * \param [out] nof_enabled_members - retrieved number of enabled
 *        members in group
 * \param [out] member_index_arr - array with enabled members indexes
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * member_index_arr is getting filled until there is no more room, nof_enabled_members will have the real number regardless to the size if the array.
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_enabled_members_get(
    int unit,
    int pool,
    int group,
    int arr_size,
    int *nof_enabled_members,
    int *member_index_arr);

/**
 * \brief - get number of enabled members in given trunk group
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [out] nof_enabled_members - retrieved number of enabled
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

/**
 * \brief - check if a gport is a virtual trunk
 *
 * \param [in] unit - unit number
 * \param [in] gport - gport
 * \param [out] is_virtual_trunk - TRUE when gport is a virtual trunk, others FALSE
 * \param [out] master_tid - master trunk ID, valid only when is_virtual_trunk is TRUE
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_group_gport_is_virtual_trunk(
    int unit,
    bcm_gport_t gport,
    uint8 *is_virtual_trunk,
    bcm_trunk_t * master_tid);

/**
 * \brief - get CHM handle
 *
 * \param [in] unit - unit number
 * \param [out] chm_handle - CHM handle
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_chm_handle_get(
    int unit,
    uint32 *chm_handle);

/**
 * \brief - set CHM handle
 *
 * \param [in] unit - unit number
 * \param [in] chm_handle - CHM handle
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_chm_handle_set(
    int unit,
    uint32 chm_handle);

/**
 * \brief - get psc profile info
 *
 * \param [in] unit - unit number
 * \param [in] psc_profile - psc profile id
 * \param [out] profile_type - profile type
 * \param [out] max_nof_members - max number of members
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_profile_info_get(
    int unit,
    int psc_profile,
    int *profile_type,
    int *max_nof_members);

/**
 * \brief - set psc profile info
 *
 * \param [in] unit - unit number
 * \param [in] psc_profile - psc profile id
 * \param [in] profile_type - profile type
 * \param [in] max_nof_members - max number of members
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_profile_info_set(
    int unit,
    int psc_profile,
    int profile_type,
    int max_nof_members);

/**
 * \brief - get user profile info psc profile
 *
 * \param [in] unit - unit number
 * \param [in] user_profile - user profile id
 * \param [out] psc_profile - psc profile id
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_user_profile_info_psc_profile_get(
    int unit,
    int user_profile,
    int *psc_profile);

/**
 * \brief - set psc profile to user profile info
 *
 * \param [in] unit - unit number
 * \param [in] user_profile - user profile id
 * \param [in] psc_profile - psc profile id
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_user_profile_info_psc_profile_set(
    int unit,
    int user_profile,
    int psc_profile);

/**
 * \brief - clear user profile info
 *
 * \param [in] unit - unit number
 * \param [in] user_profile - user profile id
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_user_profile_info_clear(
    int unit,
    int user_profile);

/**
 * \brief - increment ref counter of given profile
 *
 * \param [in] unit - unit number
 * \param [in] user_profile - user profile id to increment
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_user_profile_info_ref_count_increment(
    int unit,
    int user_profile);

/**
 * \brief - decrement ref counter of given profile
 *
 * \param [in] unit - unit number
 * \param [in] user_profile - user profile id to decrement
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_user_profile_info_ref_count_decrement(
    int unit,
    int user_profile);

/**
 * \brief - get ref counter of given profile
 *
 * \param [in] unit - unit number
 * \param [in] user_profile - user profile id to decrement
 * \param [out] ref_counter - ref counter value
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_user_profile_info_ref_count_get(
    int unit,
    int user_profile,
    int *ref_counter);

#endif /* _BCM_DNX_TRUNK_TRUNKSWDB_H_INCLUDED */
