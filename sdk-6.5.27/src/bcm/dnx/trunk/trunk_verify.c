/** \file src/bcm/dnx/trunk/trunk_verify.c
 * 
 *  Internal DNX TRUNK verify APIs to be used in trunk module 
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

#include <soc/sand/shrextend/shrextend_debug.h>
#include <shared/trunk.h>
#include <bcm/trunk.h>
#include <bcm/switch.h>
#include <bcm_int/dnx/auto_generated/dnx_switch_dispatch.h>
#include <bcm_int/dnx/auto_generated/dnx_stk_dispatch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trunk.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/trunk/algo_trunk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_port.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_trunk_access.h>
#include "trunk_utils.h"
#include "trunk_sw_db.h"
#include "trunk_temp_structs_to_skip_papi.h"
#include "trunk_verify.h"
#include "trunk_chm.h"

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
 * \brief - verify id_info is within valid range as defined by 
 *        user in soc properties.
 * 
 * \param [in] unit - unit number
 * \param [in] id_info - id info structure
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
dnx_trunk_id_info_t_verify(
    int unit,
    bcm_trunk_id_info_t * id_info)
{
    uint32 pool_index;
    uint32 group_index;
    uint32 nof_pools;
    uint32 nof_groups;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(id_info, _SHR_E_PARAM, "id_info");

    /** check pool is not larger than max pool */
    pool_index = id_info->pool_index;
    nof_pools = dnx_data_trunk.parameters.nof_pools_get(unit);
    if (pool_index >= nof_pools)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "provided pool index is too big, received %d when max possible value is %d\n",
                     pool_index, nof_pools - 1);
    }

    /** check group is not larger than max group */
    group_index = id_info->group_index;
    nof_groups = dnx_data_trunk.parameters.pool_info_get(unit, pool_index)->max_nof_groups_in_pool;
    if (group_index >= nof_groups)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "provided group index is too big, received %d when max possible value is %d\n",
                     group_index, nof_groups - 1);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_trunk_t_verify(
    int unit,
    bcm_trunk_t trunk_id)
{
    bcm_trunk_id_info_t id_info;
    SHR_FUNC_INIT_VARS(unit);
    if (!_SHR_TRUNK_ID_IS_VALID(trunk_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid trunk ID, need to use BCM_TRUNK_ID_SET to create trunk IDs");
    }

    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
    SHR_IF_ERR_EXIT(dnx_trunk_id_info_t_verify(unit, &id_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_trunk_was_created_verify(
    int unit,
    bcm_trunk_id_info_t * id_info)
{
    int in_use;
    bcm_trunk_t trunk_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_is_used_get(unit, id_info->pool_index, id_info->group_index, &in_use));
    if (in_use == 0)
    {
        SHR_IF_ERR_EXIT(dnx_trunk_utils_id_info_to_trunk_id_convert(unit, &trunk_id, id_info));
        SHR_ERR_EXIT(_SHR_E_PARAM, "given trunk ID 0x%x was never created\n"
                     "use bcm_trunk_create to create this trunk ID\n", trunk_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_trunk_can_fit_new_amount_of_members_verify(
    int unit,
    bcm_trunk_id_info_t * id_info,
    int new_nof_members)
{
    int max_group_size;

    SHR_FUNC_INIT_VARS(unit);

    max_group_size = dnx_data_trunk.parameters.pool_info_get(unit, id_info->pool_index)->max_nof_members_in_group;
    if (new_nof_members > max_group_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "given nof members %d is too big for pool %d\n",
                     new_nof_members, id_info->pool_index);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify function to check if a the psc_info is valid for c-lag psc
 *
 * \param [in] unit - unit number
 * \param [in] psc_info - psc_info
 * \param [in] id_info - id info
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
dnx_trunk_c_lag_psc_info_verify(
    int unit,
    bcm_trunk_psc_profile_info_t * psc_info,
    bcm_trunk_id_info_t * id_info)
{
    shr_error_e rv;
    int max_nof_members_in_profile;
    int max_group_is_support;
    int profile_type;
    int max_group_size;
    SHR_FUNC_INIT_VARS(unit);

    max_group_is_support = dnx_data_trunk.psc.feature_get(unit, dnx_data_trunk_psc_consistant_hashing_max_group);
    /** Devices which support "max_group_is_support" writes to dbal max_nof_members_in_profile-1.
     *  Other devices writes to dbal max_nof_members_in_profile. To use the same function for both
     *  needs to add one for none-support devices. */
    max_nof_members_in_profile =
        (max_group_is_support ==
         TRUE) ? psc_info->max_nof_members_in_profile : psc_info->max_nof_members_in_profile + 1;

    /** verify that max_nof_members_in_profile is valid */
    rv = dnx_trunk_c_lag_profile_type_get(unit, max_nof_members_in_profile, &profile_type);
    if (rv != _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported number of member (%d) for existing profiles",
                     psc_info->max_nof_members_in_profile);
    }

    max_group_size = dnx_data_trunk.parameters.pool_info_get(unit, id_info->pool_index)->max_nof_members_in_group;
    if (psc_info->max_nof_members_in_profile > max_group_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "given psc_info->max_nof_members_in_profile %d is too big for pool %d\n",
                     psc_info->max_nof_members_in_profile, id_info->pool_index);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify function to check if a the psc_info is valid for user profile psc
 *
 * \param [in] unit - unit number
 * \param [in] psc_info - psc_info
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
dnx_trunk_user_profile_psc_info_verify(
    int unit,
    bcm_trunk_psc_profile_info_t * psc_info)
{
    uint8 is_allocated;
    SHR_FUNC_INIT_VARS(unit);

    /** check that psc_flag with ID is set */
    if (!_SHR_IS_FLAG_SET(psc_info->psc_flags, BCM_TRUNK_PSC_WITH_ID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "USer Profile PSC requires to be set with ID\n");
    }

    /** verify profile ID is valid */
    SHR_IF_ERR_EXIT(dnx_algo_trunk_user_profile_is_allocated(unit, psc_info->profile_id, &is_allocated));
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "user profile ID %d is not allocated\n", psc_info->profile_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_trunk_psc_verify(
    int unit,
    int psc,
    bcm_trunk_psc_profile_info_t * psc_info,
    bcm_trunk_id_info_t * id_info)
{
    int psc_supported;
    char *str_ptr;
    SHR_FUNC_INIT_VARS(unit);

    switch (psc)
    {
        case BCM_TRUNK_PSC_PORTFLOW:
        {
            psc_supported = dnx_data_trunk.psc.feature_get(unit, dnx_data_trunk_psc_multiply_and_divide);
            str_ptr = "Multiply and Divide";
            break;
        }
        case BCM_TRUNK_PSC_SMOOTH_DIVISION:
        {
            psc_supported = dnx_data_trunk.psc.feature_get(unit, dnx_data_trunk_psc_smooth_division);
            str_ptr = "Smooth Division";
            break;
        }
        case BCM_TRUNK_PSC_C_LAG:
        {
            psc_supported = dnx_data_trunk.psc.feature_get(unit, dnx_data_trunk_psc_consistant_hashing);
            str_ptr = "C-lag (Consistent Hashing)";
            SHR_IF_ERR_EXIT(dnx_trunk_c_lag_psc_info_verify(unit, psc_info, id_info));
            break;
        }
        case BCM_TRUNK_PSC_USER_PROFILE:
        {
            /** User profile PSC uses C-Lag infra, so the blocking feature is  psc_consistant_hashing */
            psc_supported = dnx_data_trunk.psc.feature_get(unit, dnx_data_trunk_psc_consistant_hashing);
            str_ptr = "User Profile";
            SHR_IF_ERR_EXIT(dnx_trunk_user_profile_psc_info_verify(unit, psc_info));
            break;
        }
        default:
        {
            psc_supported = FALSE;
            str_ptr = "Unknown PSC";
            break;
        }
    }

    if (!psc_supported)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Given PSC %s (%d) is not supported for this unit(%d)\n", str_ptr, psc, unit);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_trunk_member_flags_verify(
    int unit,
    uint32 *flags)
{
    SHR_FUNC_INIT_VARS(unit);

    if (*flags &
        ~(BCM_TRUNK_MEMBER_INGRESS_DISABLE | BCM_TRUNK_MEMBER_EGRESS_DISABLE | BCM_TRUNK_MEMBER_SPA_WITH_ID |
          BCM_TRUNK_MEMBER_WITH_ID |
          ((dnx_data_trunk.protection.feature_get(unit,
                                                  dnx_data_trunk_protection_backup_destination)) ?
           BCM_TRUNK_MEMBER_WITH_PROTECTION : 0)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "API was used with invalid flags\n");
    }

    if (*flags & (BCM_TRUNK_MEMBER_INGRESS_DISABLE))
    {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "attempt to use legacy flag - BCM_TRUNK_MEMBER_INGRESS_DISABLE\n"
                             "changing them to relevant flag instead - BCM_TRUNK_MEMBER_EGRESS_DISABLE\n")));

        *flags &= ~BCM_TRUNK_MEMBER_INGRESS_DISABLE;
        *flags |= BCM_TRUNK_MEMBER_EGRESS_DISABLE;
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_trunk_new_member_has_matching_header_type_verify(
    int unit,
    bcm_trunk_id_info_t * id_info,
    bcm_gport_t gport)
{
    uint32 trunk_in_header_type;
    uint32 trunk_out_header_type;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t in_header_type;
    bcm_switch_control_info_t out_header_type;
    uint32 trunk_flags;
    SHR_FUNC_INIT_VARS(unit);

    /** if new member is not local to this FAP or lag is virtual, don't check header type */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_flags_get(unit, id_info->pool_index, id_info->group_index, &trunk_flags));
    if (trunk_flags & BCM_TRUNK_FLAG_VP)
    {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE, &gport_info));
    if (0 == (gport_info.flags & DNX_ALGO_GPM_GPORT_INFO_F_IS_LOCAL_PORT))
    {
        SHR_EXIT();
    }

    /** get trunk header type from trunk_sw_db */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_header_types_get
                    (unit, id_info->pool_index, id_info->group_index, &trunk_in_header_type, &trunk_out_header_type));

    /** get added member header types */
    key.type = bcmSwitchPortHeaderType;
    /** in direction*/
    key.index = 1;
    SHR_IF_ERR_EXIT(bcm_dnx_switch_control_indexed_port_get(unit, gport, key, &in_header_type));
    /** out direction*/
    key.index = 2;
    SHR_IF_ERR_EXIT(bcm_dnx_switch_control_indexed_port_get(unit, gport, key, &out_header_type));

    /** if either in or out header types of new member and trunk are different, fail */
    if ((in_header_type.value != trunk_in_header_type) || (out_header_type.value != trunk_out_header_type))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "new member header type is different from trunk header type\n"
                     "trunk header types      : IN 0x%x, OUT 0x%x\n"
                     "new_member header types : IN 0x%x, OUT 0x%x\n"
                     "change either the member or the trunk header type and try again\n",
                     trunk_in_header_type, trunk_out_header_type, in_header_type.value, out_header_type.value);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_trunk_modport_membership_verify(
    int unit,
    bcm_gport_t sysport,
    int member_count,
    bcm_trunk_member_t * member_array)
{
    int member_i;
    bcm_gport_t modport, modport_i;

    SHR_FUNC_INIT_VARS(unit);

    /** get modport from the given sysport */
    SHR_IF_ERR_EXIT(bcm_dnx_stk_sysport_gport_get(unit, sysport, &modport));

    /** iterate over members and find a match in physical mapping */
    for (member_i = 0; member_i < member_count; member_i++)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_stk_sysport_gport_get(unit, member_array[member_i].gport, &modport_i));
        if (modport == modport_i)
        {
            SHR_EXIT();
        }
    }

    /** if we reached this point then it means that modport match wasn't found */
    SHR_ERR_EXIT(_SHR_E_PARAM, "given modport 0x%x is nod part of the member list", modport);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_trunk_member_with_id_verify(
    int unit,
    bcm_trunk_member_t * member,
    int psc,
    int pool,
    int group)
{
    int provided_member_id;
    uint8 provided_member_id_is_allocated;
    uint32 flags = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (!(member->flags & BCM_TRUNK_MEMBER_WITH_ID))
    {
        SHR_EXIT();
    }

    if (!((psc == BCM_TRUNK_PSC_C_LAG) || (psc == BCM_TRUNK_PSC_USER_PROFILE)))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "psc provided (%d) is not compatible with specifying member_id \n", psc);
    }

    /** check if this member_id allocated */
    provided_member_id = member->member_id;
    SHR_IF_ERR_EXIT(algo_trunk_db.pools.groups.tm_index_manager.is_allocated(unit, pool, group, provided_member_id,
                                                                             &provided_member_id_is_allocated));
    if (provided_member_id_is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "member_id provided (%d) is already assigned to another member\n",
                     provided_member_id);
    }

    /** check that the member_id is within range */
    flags |= DNX_ALGO_RES_ALLOCATE_WITH_ID;
    flags |= DNX_ALGO_RES_ALLOCATE_SIMULATION;
    SHR_IF_ERR_EXIT_WITH_LOG(algo_trunk_db.pools.groups.
                             tm_index_manager.allocate_single(unit, pool, group, flags, NULL, &provided_member_id),
                             "Invalid configuration, BCM_TRUNK_MEMBER_WITH_ID used but bcm_trunk_member_t.member_id should be in valid range. id provided: %d %s %s",
                             provided_member_id, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_trunk_member_spa_with_id_verify(
    int unit,
    bcm_trunk_member_t * member,
    int pool,
    int group)
{
    int existing_spa_id = INVALID_SPA_MEMBER_ID;
    int gport_exists_in_trunk = FALSE;
    int first_index = 0;
    int last_index = 0;
    int nof_replications = 0;
    int provided_spa_id = 0;
    uint8 provided_spa_id_is_allocated = FALSE;
    uint32 flags = 0;
    trunk_group_member_info_t existing_member_info;

    SHR_FUNC_INIT_VARS(unit);
    if (!(member->flags & BCM_TRUNK_MEMBER_SPA_WITH_ID))
    {
        SHR_EXIT();
    }
    /** check if this spa_id allocated */
    provided_spa_id = member->spa_id;
    SHR_IF_ERR_EXIT(algo_trunk_db.pools.groups.
                    spa_id_manager.is_allocated(unit, pool, group, provided_spa_id, &provided_spa_id_is_allocated));

    /** get the gport matched member_info */
    existing_member_info.system_port = member->gport;
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_in_group_find
                    (unit, pool, group, &existing_member_info, &first_index, &last_index, &nof_replications));
    if (nof_replications > 0)
    {
        gport_exists_in_trunk = TRUE;
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_get(unit, pool, group, last_index, &existing_member_info));
        existing_spa_id = existing_member_info.spa_member_id;
    }
    /** 1 to 1 relationship between gport and spa_id enforcement*/
    if (!gport_exists_in_trunk && provided_spa_id_is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "Invalid configuration, BCM_TRUNK_MEMBER_SPA_WITH_ID used with bcm_trunk_member_t.spa_id %d"
                     " but the spa_id is already allocated in this trunk, and not to the provided system port %d",
                     provided_spa_id, member->gport);
    }
    if (gport_exists_in_trunk && (provided_spa_id != existing_spa_id))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "Invalid configuration, BCM_TRUNK_MEMBER_SPA_WITH_ID used and bcm_trunk_member_t.spa_id is %d"
                     " but the system_port is already allocated with spa_id = %d", provided_spa_id, existing_spa_id);
    }

    /** check that the spa_id is within range, only if not allocated already*/
    if (!provided_spa_id_is_allocated)
    {
        flags |= DNX_ALGO_RES_ALLOCATE_WITH_ID;
        flags |= DNX_ALGO_RES_ALLOCATE_SIMULATION;
        SHR_IF_ERR_EXIT_WITH_LOG(algo_trunk_db.pools.groups.
                                 spa_id_manager.allocate_single(unit, pool, group, flags, NULL, &provided_spa_id),
                                 "Invalid configuration, BCM_TRUNK_MEMBER_SPA_WITH_ID used but bcm_trunk_member_t.spa_id should be in valid range. id provided: %d %s %s",
                                 existing_spa_id, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_trunk_backup_member_verify(
    int unit,
    bcm_trunk_t trunk_id,
    bcm_trunk_member_t * backup_member)
{
    bcm_trunk_id_info_t id_info;
    int pool;
    int group;
    algo_gpm_gport_verify_type_e allowed_port_infos[] = { ALGO_GPM_GPORT_VERIFY_TYPE_SYSTEM_PORT };
    uint32 system_port;
    bcm_trunk_t curr_trunk_id;
    uint32 curr_flags;
    int system_port_is_not_a_member_of_another_trunk;
    int system_port_is_already_a_member_of_current_trunk;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
    pool = id_info.pool_index;
    group = id_info.group_index;

    /** check validity of system port */
    SHR_IF_ERR_EXIT(algo_gpm_gport_verify(unit, backup_member->gport, COUNTOF(allowed_port_infos), allowed_port_infos));
    /** check that system port can be added to trunk */
    system_port = BCM_GPORT_SYSTEM_PORT_ID_GET(backup_member->gport);
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_trunk_system_port_db_get(unit, system_port, &curr_trunk_id, &curr_flags));
    system_port_is_not_a_member_of_another_trunk = (curr_trunk_id == TRUNK_SW_DB_INVALID_TRUNK_ID);
    system_port_is_already_a_member_of_current_trunk = (curr_trunk_id == trunk_id);
    if (!(system_port_is_not_a_member_of_another_trunk || system_port_is_already_a_member_of_current_trunk))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Invalid configuration, backup member is already found in another trunk");
    }

    /** perform checks for adding new member to trunk */
    if (!system_port_is_already_a_member_of_current_trunk)
    {
        int member_count;
        /** Check that number of members can fit into group */
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_nof_members_in_group_get(unit, pool, group, &member_count));
        SHR_IF_ERR_EXIT(dnx_trunk_can_fit_new_amount_of_members_verify(unit, &id_info, member_count + 1));
        SHR_IF_ERR_EXIT(dnx_trunk_new_member_has_matching_header_type_verify(unit, &id_info, backup_member->gport));
    }

    /** verify validity of spa_id in case BCM_TRUNK_MEMBER_SPA_WITH_ID was provided*/
    SHR_IF_ERR_EXIT(dnx_trunk_member_spa_with_id_verify(unit, backup_member, pool, group));

exit:
    SHR_FUNC_EXIT;
}
