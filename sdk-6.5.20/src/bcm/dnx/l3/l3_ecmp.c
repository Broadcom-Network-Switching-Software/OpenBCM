/** \file l3_ecmp.c
 *
 * Handles the control over the ECMPs entries.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_L3

/*
 * Include files currently used for DNX.
 * {
 */

#include <bcm_int/dnx/l3/l3_ecmp.h>
#include <bcm_int/dnx/l3/l3_ecmp_vip.h>
#include <bcm_int/dnx/switch/switch.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/ecmp_access.h>
#include <shared/utilex/utilex_bitstream.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_l3_access.h>
#include <soc/dnx/swstate/auto_generated/access/switch_access.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
/*
 * }
 */
/*
 * Include files.
 * {
 */

#include <bcm/types.h>
#include <bcm_int/dnx/stat/stat_pp.h>
/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/*
 * In case an ECMP group doesn't contains all the members the ECMP ID will identify the unique consistent member table
 * that was allocated for it. In case the group uses all the members (group size = max_paths) the group will use this
 * identifier that will indicate that this ECMP group uses a general consistent table for ECMP groups with " group size"
 * members.
 */
#define L3_ECMP_FULL_MEMBERS_IN_GROUP                   0xFFFFFFFF
/*
 * The L3 supported flags of the ECMP.
 */
#define L3_ECMP_SUPPORTED_BCM_L3_FLAGS                 (BCM_L3_WITH_ID | BCM_L3_REPLACE | BCM_L3_2ND_HIERARCHY | BCM_L3_3RD_HIERARCHY)
/*
 * The L3 TP supported flags of the ECMP.
 */
#define L3_ECMP_TP_SUPPORTED_BCM_L3_FLAGS                 (BCM_L3_WITH_ID | BCM_L3_2ND_HIERARCHY | BCM_L3_3RD_HIERARCHY)
/*
 * The BCM_L3_ECMP_* supported flags
 */
#define L3_ECMP_SUPPORTED_BCM_L3_ECMP_FLAGS            (BCM_L3_ECMP_LARGE_TABLE | BCM_L3_ECMP_EXTENDED)
/*
 * Row size in the members table
 */
#define L3_ECMP_MEMBER_TABLE_ADDRESS_DEFAULT_VAL       0

/*
 * In case the consistent ECMP group isn't full (NOF members < group max size) a unique ID should be used to allocate an ECMP
 * profile just for that group.
 * In case of an add/remove action on the ECMP group we would like to first allocate a new profile to the group before releasing the
 * old one (to keep traffic going) so the current NOF members would also added to the ECMP unique key.
 */
#define L3_ECMP_ID_FIELD_SIZE                          16
#define L3_ECMP_ID_FIELD_MASK                          UTILEX_BITS_MASK(L3_ECMP_ID_FIELD_SIZE,0)
#define L3_ECMP_ID_FIELD_OFFSET                        0

#define L3_ECMP_NOF_MEMBERS_FIELD_SIZE(unit)           utilex_log2_round_up(dnx_data_l3.ecmp.max_group_size_get(unit))
#define L3_ECMP_NOF_MEMBERS_FIELD_MASK(unit)           UTILEX_BITS_MASK(L3_ECMP_NOF_MEMBERS_FIELD_SIZE(unit),0)
#define L3_ECMP_NOF_MEMBERS_FIELD_OFFSET               L3_ECMP_ID_FIELD_SIZE

/** Stat id is 16 bits long */
#define L3_ECMP_MAX_STAT_ID_VALUE                      0xFFFF

/*
 * MACROs
 * {
 */

/**
 * \remark
 *      This macro returns the ECMP hierarchy level based on the
 *      ECMP BCM_L3_* flags
 *      Valid BCM_L3_* flags are:
 *          * BCM_L3_2ND_HIERARCHY - if flag is set, then it's
 *              2nd hierarchy level
 *          * BCM_L3_3RD_HIERARCHY - if flag is set, then it's
 *              3rd hierarchy level
 *      If neither of the two flags is added, then it is 1st hierarchy level.
 */
#define L3_ECMP_GET_HIERARCHY(flags) (_SHR_IS_FLAG_SET(flags, BCM_L3_2ND_HIERARCHY) ? \
                                     DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_2 : \
                                     (_SHR_IS_FLAG_SET(flags, BCM_L3_3RD_HIERARCHY) ? DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_3 : \
                                     DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1))
/*
 * Return the consistent table type to use
 * use_large_table - indication if to use the large table or not
 * max_path - if the use_large_table if off then max_path > 16 is medium table and otherwise small.
 */
#define L3_ECMP_GET_CONSISTENT_TABLE_TYPE(unit,use_large_table, max_path) (use_large_table ? DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_LARGE_SIZE : (max_path  > dnx_data_l3.ecmp.consistent_tables_info_get(unit,DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_SMALL_SIZE)->max_nof_unique_members ? DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_MEDIUM_SIZE : DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_SMALL_SIZE))
/*
 *Verifies that the consistent table type is valid.
 * type - the consistent hashing type table
 */
#define L3_ECMP_CONSISTENT_TYPE_VERIFY(type) SHR_IF_ERR_EXIT(((type < DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_SMALL_SIZE) || (type > DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_LARGE_SIZE)) ? _SHR_E_INTERNAL : _SHR_E_NONE)
/*
 * translate the consistent hashing manager profile offset into an address in the  members table by multiplying is by a single resource element (smallest table) NOF rows.
 * chm_offset - Given (resource) offset by the chm.
 */
#define L3_ECMP_GET_MEMBERS_TBL_ADDR_FROM_CHM_OFFSET(unit,chm_offset) (chm_offset * L3_ECMP_CONSISTENT_NOF_ROWS_IN_MEMBER_TABLE(unit,DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_SMALL_SIZE))

/** Translate the member table address to a consistent hashing manager offset used for accessing the SW state for ECMP FEC members. */
#define L3_ECMP_GET_OFFSET_FROM_MEMBERS_TBL_ADDR(unit,members_address) (members_address / L3_ECMP_CONSISTENT_NOF_ROWS_IN_MEMBER_TABLE(unit,DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_SMALL_SIZE))
/*
 * the consistent hashing manager unique key is composed from the profile id and the hierarchy.
 * This MACRO extract the hierarchy from the key
 */
#define L3_ECMP_HIERARCHY_FROM_CHM_KEY_GET(unit, chm_key) ((chm_key >> L3_ECMP_HIERARCHY_FIELD_OFFSET(unit)) & L3_ECMP_HIERARCHY_FIELD_MASK)
/*
 * the consistent hashing manager unique key is composed from the profile id and the hierarchy.
 * This MACRO extract the profile from the key
 */
#define L3_ECMP_PROFILE_ID_FROM_CHM_KEY_GET(unit, chm_key) ((chm_key >> dnx_data_l3.ecmp.profile_id_offset_get(unit)) & L3_ECMP_PROFILE_ID_FIELD_MASK(unit))
/*
 * return a unique key to allocate a unique profile for a consistent ECMP group which isn't full (NOF members < group max size).
 *
 * In case the consistent ECMP group isn't full (NOF members < group max size) a unique ID should be used to allocate an ECMP
 * profile just for that group.
 * In case of an add/remove action on the ECMP group we would like to first allocate a new profile to the group before releasing the
 * old one (to keep traffic going) so the current NOF members would also added to the ECMP unique key.
 */
#define L3_ECMP_PROFILE_UNIQUE_KEY(unit, ecmp_id, intf_count) (((ecmp_id & L3_ECMP_ID_FIELD_MASK) << L3_ECMP_ID_FIELD_OFFSET) | ((intf_count & L3_ECMP_NOF_MEMBERS_FIELD_MASK(unit)) << L3_ECMP_NOF_MEMBERS_FIELD_OFFSET) )
/*
 * Determine whether the ECMP index belongs to the extended range of ECMP IDs.
 */
#define L3_ECMP_INDEX_IS_IN_EXTENDED_RANGE(unit, ecmp_index) (ecmp_index > dnx_data_l3.ecmp.max_ecmp_basic_mode_get(unit))
/*
 * Return the hierarchy flags with which the ECMP group was created based on its hierarchy
 */
#define L3_EGRESS_ECMP_GET_HIER_FLAGS(hierarchy) (hierarchy == DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1 ? 0 : (hierarchy == DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_2 ? BCM_L3_2ND_HIERARCHY : BCM_L3_3RD_HIERARCHY))
/*
 * Invoke an ECMP verification function only if the ECMP verification mode is enabled.
 */
#define L3_EGRESS_ECMP_INVOKE_VERIFY_IF_ENABLE(unit,_expr) DNX_SWITCH_VERIFY_IF_ENABLE_DNX(unit,_expr,bcmModuleL3EgressEcmp)
/*
 *Verifies that the specified dynamic mode is one of the supported modes.
 */
#define L3_ECMP_IS_DYNAMIC_MODE_SUPPORTED(mode) (mode == BCM_L3_ECMP_DYNAMIC_MODE_DISABLED || mode == BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT)
/*
 * }
 */
/*
 * Enumeration.
 * {
 */

/*
 * }
 */
/*
 * }
 */
/*
 * Structures
 * {
 */

/*
 * Globals.
 * {
 */
const char *ecmp_profiles_resources[DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_ECMP_HIERARCHIES] =
    { DNX_ALGO_L3_ECMP_PROFILE_HIER_1_TABLE_RESOURCE, DNX_ALGO_L3_ECMP_PROFILE_HIER_2_TABLE_RESOURCE,
    DNX_ALGO_L3_ECMP_PROFILE_HIER_3_TABLE_RESOURCE
};

/*
 * }
 */
/*
 * Functions
 * {
 */

/*
 * For more  details see .h file
 */
shr_error_e
dnx_l3_ecmp_consistent_table_type_from_ecmp_mode_get(
    int unit,
    dbal_enum_value_field_ecmp_mode_e ecmp_mode,
    dbal_enum_value_field_ecmp_consistent_table_e * consistent_table_type)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (ecmp_mode)
    {
        case DBAL_ENUM_FVAL_ECMP_MODE_CONSISTENT_SMALL_TABLE:
        {
            *consistent_table_type = DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_SMALL_SIZE;
            break;
        }
        case DBAL_ENUM_FVAL_ECMP_MODE_CONSISTENT_MEDIUM_TABLE:
        {
            *consistent_table_type = DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_MEDIUM_SIZE;
            break;
        }
        case DBAL_ENUM_FVAL_ECMP_MODE_CONSISTENT_LARGE_TABLE:
        {
            *consistent_table_type = DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_LARGE_SIZE;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "ECMP mode %d is either not consistent or invalid.", ecmp_mode);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   In case of a large consistent table not all the available entries are used but the largest multiplication of the ECMP
 *   group size that fit to ensure even distribution between the members.
 *   Due to HW limitation to hold only a representation of half the size of the used entries the NOF them must be
 *   even so in case of an odd number the NOF ECMP group multiplication which is used decreased by one.
 * \param [in] unit - The unit number.
 * \param [in] ecmp_group_size - the ECMP group size
 * \param [out] nof_valid_entries_in_table - The NOF entries that will be used in the large table for this group size
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static void
dnx_l3_ecmp_consistent_large_table_nof_valid_entries_get(
    int unit,
    uint32 ecmp_group_size,
    uint32 *nof_valid_entries_in_table)
{

    uint32 nof_entries = (dnx_data_l3.ecmp.consistent_tables_info_get(unit,
                                                                      DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_LARGE_SIZE)->nof_entries
                          / ecmp_group_size) * ecmp_group_size;
    /*
     * In case the nof_entries is odd remove one ecmp_group_size to make it even.
     */
    nof_entries -= (nof_entries & 0x1) * ecmp_group_size;

    *nof_valid_entries_in_table = nof_entries;
}

/**
 * \brief
 *   return the NOF of rows that are been used in the requested consistent table type.
 * \param [in] unit - The unit number.
 * \param [in] consistent_table_type - the type of the consistent table.
 * \param [in] ecmp_group_size - the ECMP group size.
 * \param [out] nof_entries_used - The NOF of entries that are used by the requested table.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
dnx_l3_ecmp_member_table_used_rows_get(
    int unit,
    dbal_enum_value_field_ecmp_consistent_table_e consistent_table_type,
    uint32 ecmp_group_size,
    uint32 *nof_entries_used)
{
    SHR_FUNC_INIT_VARS(unit);

    L3_ECMP_CONSISTENT_TYPE_VERIFY(consistent_table_type);

    if (consistent_table_type == DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_LARGE_SIZE)
    {
        dnx_l3_ecmp_consistent_large_table_nof_valid_entries_get(unit, ecmp_group_size, nof_entries_used);
    }
    else
    {
        *nof_entries_used = dnx_data_l3.ecmp.consistent_tables_info_get(unit, consistent_table_type)->nof_entries;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function verifies that the ECMP group is allocated and in range.
 *   It if isn't, an error is returned.
 * \param [in] unit - The unit number.
 * \param [in] ecmp -  The ECMP group information.
 * \return
 *   \retval Zero if no error was detected and the group is allocated
 *   \retval _SHR_E_NOT_FOUND if the group is not allocated
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_l3_egress_ecmp_get
 *  * bcm_dnx_l3_egress_ecmp_destroy
 *  * bcm_dnx_l3_egress_ecmp_add
 *  * bcm_dnx_l3_egress_ecmp_delete
 */
static shr_error_e
dnx_l3_ecmp_group_verify(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp)
{
    uint8 is_allocated = 0;
    int ecmp_group_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ecmp, _SHR_E_PARAM, "bcm_l3_egress_ecmp_t");

    /*
     * Verify that the specified dynamic_mode in the ecmp structure. 
     */
    if (!L3_ECMP_IS_DYNAMIC_MODE_SUPPORTED(ecmp->dynamic_mode))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Specified dynamic mode, %d, is not supported. \n", ecmp->dynamic_mode);
    }

    ecmp_group_id = BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf);
    /*
     * Verify the ecmp ID value is valid.
     */
    if ((ecmp_group_id >= dnx_data_l3.ecmp.nof_ecmp_get(unit)
         || ecmp_group_id < dnx_data_l3.fec.first_valid_fec_ecmp_id_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "ECMP %d is out of range, ECMP id must be below %d and above 0.", ecmp_group_id,
                     dnx_data_l3.ecmp.nof_ecmp_get(unit));
    }
    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_is_allocated(unit, ecmp_group_id, &is_allocated));
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "ECMP group with ID = %d is not allocated\n", ecmp_group_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify that the TP index is matching the TP mode
 * \param [in] unit - The unit number.
 * \param [in] tp_index - The tunnel priority index
 * \param [in] tunnel_priority_mode - The tunnel priority mode
 * \return
 *   \retval Zero if no error was detected and the group is allocated
 *   \retval _SHR_E_NOT_FOUND if the group is not allocated
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
static shr_error_e
dnx_l3_ecmp_tp_index_verify(
    int unit,
    int tp_index,
    bcm_l3_ecmp_tunnel_priority_mode_t tunnel_priority_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((tp_index < 0) || (tp_index >= L3_ECMP_NOF_CONSISTENT_TABLES_REQUIRED_BY_TP_MODE(tunnel_priority_mode)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "table_index %d is out of the allocation range 0-%d. \n", tp_index,
                     L3_ECMP_NOF_CONSISTENT_TABLES_REQUIRED_BY_TP_MODE(tunnel_priority_mode) - 1);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   return the ECMP basic attributes.
 * \param [in] unit - The unit number.
 * \param [in] ecmp - The ECMP group information.
 * \param [out] hierarchy - THe ECMP hierarchy
 * \param [out] ecmp_atr - The ECMP field attributes
 * \param [out] profile_data - The ECMP profile attributes
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_l3_ecmp_attributes_get(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp,
    uint32 *hierarchy,
    dnx_l3_ecmp_attributes_t * ecmp_atr,
    dnx_l3_ecmp_profile_t * profile_data)
{
    int ecmp_index;
    SHR_FUNC_INIT_VARS(unit);

    L3_EGRESS_ECMP_INVOKE_VERIFY_IF_ENABLE(unit, dnx_l3_ecmp_group_verify(unit, ecmp));

    ecmp_index = BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf);

    SHR_IF_ERR_EXIT(dnx_l3_ecmp_group_entry_result_get(unit, ecmp_index, ecmp_atr));

    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_hierarchy_get(unit, ecmp_index, hierarchy));

    SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.ecmp_profile_hier_table.profile_data_get
                    (unit, *hierarchy, ecmp_atr->profile_index, NULL, profile_data));

    L3_EGRESS_ECMP_INVOKE_VERIFY_IF_ENABLE(unit,
                                           dnx_l3_ecmp_tp_index_verify(unit, ecmp->tunnel_priority.index,
                                                                       profile_data->tunnel_priority_mode));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verifies the user input for the ECMP tunnel priority set function.
 * \param [in] unit - The unit number.
 * \param [in] ecmp_atr - The ECMP attributes.
 * \param [in] profile_data - The ECMP profile attributes.
 * \param [in] ecmp - The ECMP group information.
 * \param [in] intf_count - The ECMP current number of members in the group.
 * \param [in] intf_array - Array of the ECMP members.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_l3_egress_ecmp_create
 */
static shr_error_e
dnx_l3_egress_ecmp_tunnel_priority_set_verify(
    int unit,
    dnx_l3_ecmp_attributes_t * ecmp_atr,
    dnx_l3_ecmp_profile_t * profile_data,
    bcm_l3_egress_ecmp_t * ecmp,
    int intf_count,
    bcm_if_t * intf_array)
{
    uint8 fec_protection;
    uint32 iter;
    bcm_if_t base_fec = BCM_L3_ITF_VAL_GET(ecmp_atr->fec_base);
    SHR_FUNC_INIT_VARS(unit);

    if (intf_count < 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "intf_count %d must be a positive number larger than 0. \n", intf_count);
    }

    for (iter = 0; iter < intf_count; iter++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_l3_fec_protection_state_get
                        (unit, BCM_L3_ITF_VAL_GET(intf_array[iter]), &fec_protection));

        if ((BCM_L3_ITF_VAL_GET(intf_array[iter]) < base_fec)
            || (BCM_L3_ITF_VAL_GET(intf_array[iter]) >= base_fec + profile_data->group_size))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "FEC 0x%x isn't in the allocated ECMP range 0x%x-0x%x. \n",
                         BCM_L3_ITF_VAL_GET(intf_array[iter]), base_fec, base_fec + profile_data->group_size);
        }

        if (fec_protection != profile_data->protection_flag)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "The protection state of 0x%x is different than the allocation protection state. \n",
                         BCM_L3_ITF_VAL_GET(intf_array[iter]));
        }

        if (profile_data->protection_flag && (intf_array[iter] % 2 == 1))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "In case protected FECs are used as ECMP group members, only the primary FECs (even numbered FECs, in this case %d) in the protection pair should be provided to the ECMP API and the not secondary FECs in the protection (odd numbered FECs, as provided in this case %d).",
                         intf_array[iter] - 1, intf_array[iter]);
        }
    }

    if ((ecmp->tunnel_priority.index < 0)
        || (ecmp->tunnel_priority.index >=
            L3_ECMP_NOF_CONSISTENT_TABLES_REQUIRED_BY_TP_MODE(profile_data->tunnel_priority_mode)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "table_index %d is out of the allocation range 0-%d. \n",
                     ecmp->tunnel_priority.index,
                     L3_ECMP_NOF_CONSISTENT_TABLES_REQUIRED_BY_TP_MODE(profile_data->tunnel_priority_mode) - 1);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * For more  details see .h file
 */
shr_error_e
dnx_l3_ecmp_hierarchy_profile_resource_get(
    int unit,
    dbal_enum_value_field_hierarchy_level_e hierarchy,
    char **profile_resouce)
{
    SHR_FUNC_INIT_VARS(unit);

    if (hierarchy >= DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1
        || hierarchy < dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit))
    {
        *profile_resouce = (char *) ecmp_profiles_resources[hierarchy];
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, ".Unknown hierarchy type %d.", hierarchy);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See the .h file for more information.
 */
shr_error_e
dnx_l3_ecmp_group_entry_result_get(
    int unit,
    bcm_if_t ecmp_intf,
    dnx_l3_ecmp_attributes_t * ecmp_atr)
{
    uint32 entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(ecmp_atr, 0, sizeof(dnx_l3_ecmp_attributes_t));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_TABLE, &entry_handle_id));
    /** Keys */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_ID, ecmp_intf);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** receive the group profile ID value and the FEC base ptr*/
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_ECMP_GROUP_PROFILE,
                                                        INST_SINGLE, &(ecmp_atr->profile_index)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_FEC_POINTER_BASE,
                                                        INST_SINGLE, &(ecmp_atr->fec_base)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get(unit, entry_handle_id, DBAL_FIELD_RPF_MODE_IS_EM,
                                                       INST_SINGLE, &(ecmp_atr->is_rpf_mode_em)));

    /*
     * Set the FEC base with an interface prefix
     */
    BCM_L3_ITF_SET(ecmp_atr->fec_base, BCM_L3_ITF_TYPE_FEC, BCM_L3_ITF_VAL_GET(ecmp_atr->fec_base));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function reads all the consistent hashing members offsets from the HW and returns a sorted array of the members offset
 * which contains a single instance of each member offset although it may appear more than once in the HW table.
 * \param [in] unit -
 *   The unit number.
 * \param [in] members_address -
 *   the base row in the members table to start read from.
 * \param [in] consistent_type -
 *   the consistent table type
 * \param [in] group_size -
 *   the ECMP max group size.
 * \param [in] protection -
 *   indicate the ECMP group is protected.
 * \param [in] fec_base -
 *   the ECMP group base FEC ID.
 * \param [in,out] members_array -
 *   An array that returns all the unique members that this ECMP group have.
 * \param [in,out] member_count -
 *   pointer to the return value of the NOF members in the ECMP group.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_l3_egress_ecmp_get_unique_consistent_members(
    int unit,
    uint32 members_address,
    dbal_enum_value_field_ecmp_consistent_table_e consistent_type,
    uint32 group_size,
    uint32 protection,
    uint32 fec_base,
    bcm_if_t * members_array,
    int *member_count)
{
    uint32 row_indx, member_indx, nof_members, members_pointer = 0, offset;
    uint32 exists_members[UTILEX_DIV_ROUND_UP(DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE, SAL_UINT32_NOF_BITS)];
    uint32 nof_memb_in_row;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * A bit stream is used to mark each member offset that was found in the HW members table.
     */
    SHR_IF_ERR_EXIT(utilex_bitstream_clear
                    (exists_members,
                     UTILEX_DIV_ROUND_UP(dnx_data_l3.ecmp.max_group_size_get(unit), SAL_UINT32_NOF_BITS)));
    offset = L3_ECMP_GET_OFFSET_FROM_MEMBERS_TBL_ADDR(unit, members_address);
    /*
     * Get the supported number of members in this table type
     */
    SHR_IF_ERR_EXIT(dnx_l3_ecmp_member_table_used_rows_get(unit, consistent_type, group_size, &nof_members));

    nof_memb_in_row = L3_ECMP_CONSISTENT_NOF_MEMBERS_IN_MEM_ROW(unit, consistent_type);
    /*
     * The NOF relevant members in a table doesn't necessarily fills a row in the HW, so as long as the current NOF members offset that
     * was read is smaller than the expected one the NOF rows will increment.
     */
    for (row_indx = 0; members_pointer < nof_members; row_indx++)
    {
        for (member_indx = 0; (member_indx < nof_memb_in_row)
             && (members_pointer < nof_members); member_indx++, members_pointer++)
        {
            uint8 member;
            SHR_IF_ERR_EXIT(ecmp_db_info.
                            ecmp_fec_members_table.get(unit, offset, row_indx * nof_memb_in_row + member_indx,
                                                       &member));
            SHR_IF_ERR_EXIT(utilex_bitstream_set_bit(exists_members, member));
        }
    }

    *member_count = 0;

    for (member_indx = 0; member_indx < dnx_data_l3.ecmp.max_group_size_get(unit); member_indx++)
    {
        /*
         * For each member offset that was found add the FEC base value and multiply the offset by two if protected.
         */
        if (utilex_bitstream_test_bit(exists_members, member_indx))
        {
            members_array[(*member_count)++] = fec_base + (member_indx << protection);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to the .h file for more information
 */
shr_error_e
dnx_l3_egress_ecmp_get_ecmp_group_info(
    int unit,
    bcm_if_t ecmp_index,
    int tp_table_index,
    int *member_count,
    bcm_if_t * intf_array,
    dnx_l3_ecmp_profile_t * ecmp_profile)
{
    uint8 group_size_minus_one = 0;
    uint32 address = 0, member_iter;
    uint32 entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;
    dbal_enum_value_field_hierarchy_level_e ecmp_hierarchy = 0;
    dbal_enum_value_field_ecmp_consistent_table_e consistent_table_type;
    dnx_l3_ecmp_attributes_t ecmp_atr;
    uint32 stat_pp_profile;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(ecmp_profile, 0, sizeof(*ecmp_profile));

    SHR_IF_ERR_EXIT(dnx_l3_ecmp_group_entry_result_get(unit, ecmp_index, &ecmp_atr));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE, &entry_handle_id));

    /*
     * get the hierarchy of the ECMP group using its ID.
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_hierarchy_get(unit, ecmp_index, (uint32 *) &ecmp_hierarchy));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_GROUP_PROFILE, ecmp_atr.profile_index);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HIERARCHY_LEVEL, ecmp_hierarchy);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_ECMP_MODE,
                                                        INST_SINGLE, &(ecmp_profile->ecmp_mode)));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_ECMP_MEMBERS_BASE,
                                                        INST_SINGLE, &address));
    /*
     * Get the statistic data if the profile is valid
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                        entry_handle_id,
                                                        DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE, &stat_pp_profile));
    if (stat_pp_profile != STAT_PP_PROFILE_INVALID)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID,
                                                            INST_SINGLE, &(ecmp_profile->statistic_object_id)));

        STAT_PP_ENGINE_PROFILE_SET(ecmp_profile->statistic_object_profile, stat_pp_profile,
                                   bcmStatCounterInterfaceIngressReceivePp);
    }

    if (dnx_data_l3.ecmp.tunnel_priority_support_get(unit))
    {
        /*
         * The tunnel_priority_mode enumeration matches the HW values for now, so no need to convert the values.
         */
        dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_ECMP_TUNNEL_PRIORITY_MODE, INST_SINGLE,
                                            &(ecmp_profile->tunnel_priority_mode));

        dbal_entry_handle_value_field8_get(unit, entry_handle_id, DBAL_FIELD_ECMP_TUNNEL_PRIORITY_MAP_PROFILE,
                                           INST_SINGLE, &(ecmp_profile->tunnel_priority_map_profile));
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get(unit, entry_handle_id, DBAL_FIELD_PROTECTION_ENABLE,
                                                       INST_SINGLE, &(ecmp_profile->protection_flag)));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_ECMP_RPF_MODE,
                                                        INST_SINGLE, &(ecmp_profile->rpf_mode)));

    /** The HW doesn't always contain the actual ECMP group size so the real value is kept in the SW state*/
    SHR_IF_ERR_EXIT(ecmp_db_info.
                    ecmp_group_size_minus_one.get(unit, ecmp_hierarchy, ecmp_atr.profile_index, &group_size_minus_one));
    ecmp_profile->group_size = group_size_minus_one + 1;

    /*
     * Update the interfaces array, in case of ECMP mode "multiply and divide" the interfaces are consecutive and can be retrieve using the
     * base FEC and the group size.
     * In the consistent case the HW member table should be used for the update.
     */
    if (ecmp_profile->ecmp_mode == DBAL_ENUM_FVAL_ECMP_MODE_MULTIPLY_AND_DIVIDE)
    {
        *member_count = ecmp_profile->group_size;
        for (member_iter = 0; member_iter < *member_count; member_iter++)
        {
            intf_array[member_iter] = ecmp_atr.fec_base + (member_iter << ecmp_profile->protection_flag);
        }

        ecmp_profile->members_table_id = 0;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_l3_ecmp_consistent_table_type_from_ecmp_mode_get
                        (unit, ecmp_profile->ecmp_mode, &consistent_table_type));
        /*
         * the address in the profile is just relative to the table size so we need to multiply in the NOF rows.
         * The address LSB is the TP index which is 0 in case the TP isn't in use.
         */
        ecmp_profile->members_table_id = address;
        address = (address + tp_table_index) * L3_ECMP_CONSISTENT_NOF_ROWS_IN_MEMBER_TABLE(unit, consistent_table_type);

        SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_get_unique_consistent_members
                        (unit, address, consistent_table_type, ecmp_profile->group_size,
                         ecmp_profile->protection_flag, ecmp_atr.fec_base, intf_array, member_count));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Receive information regarding a single ECMP group -
 *  mode, flags, number of members and list of members.
 * \param [in] unit -
 *   The unit number.
 * \param [in] ecmp_index -
 *   The ECMP ID.
 * \param [out] ecmp - an ECMP structure that will return flags,
 *   max paths and mode.
 * \param [in] intf_size -
 *   Number of members that can be added to the intf_array
 * \param [out] intf_array -
 *   Array of the group members
 * \param [out] intf_count -
 *   number of actual members in the intf_array.
 */
static shr_error_e
dnx_l3_egress_ecmp_group_data_get(
    int unit,
    bcm_if_t ecmp_index,
    bcm_l3_egress_ecmp_t * ecmp,
    int intf_size,
    bcm_if_t * intf_array,
    int *intf_count)
{
    dnx_l3_ecmp_profile_t ecmp_profile;
    bcm_if_t members[DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE];
    int actual_members_count = 0;
    uint32 hierarchy;

    SHR_FUNC_INIT_VARS(unit);

    /** Get the real hierarchy ID. */
    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_hierarchy_get(unit, ecmp_index, &hierarchy));
    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_get_ecmp_group_info
                    (unit, ecmp_index, ecmp->tunnel_priority.index, &actual_members_count, members, &ecmp_profile));
    /*
     * If the size allocated in the array is insufficient for all members in the group, then an error will be returned.
     */
    if (actual_members_count > intf_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The provided array of size %d can't hold all the %d members that were found for ECMP group %d.\n",
                     intf_size, actual_members_count, ecmp_index);
    }

    *intf_count = actual_members_count;

    sal_memcpy(intf_array, members, actual_members_count * sizeof(bcm_if_t));

    ecmp->max_paths = ecmp_profile.group_size;

    ecmp->ecmp_group_flags =
        (ecmp_profile.ecmp_mode == DBAL_ENUM_FVAL_ECMP_MODE_CONSISTENT_LARGE_TABLE) ? BCM_L3_ECMP_LARGE_TABLE : 0;

    ecmp->ecmp_group_flags |=
        L3_ECMP_INDEX_IS_IN_EXTENDED_RANGE(unit, BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf)) ? BCM_L3_ECMP_EXTENDED : 0;

    /*
     * Set the rpf mode if any is defined.
     */
    if (ecmp_profile.rpf_mode == DBAL_ENUM_FVAL_ECMP_RPF_MODE_LOOSE)
    {
        ecmp->rpf_mode = bcmL3EcmpUrpfLoose;
    }
    else if (ecmp_profile.rpf_mode == DBAL_ENUM_FVAL_ECMP_RPF_MODE_STRICT_EM)
    {
        ecmp->rpf_mode = bcmL3EcmpUrpfStrictEm;
    }
    else
    {
        ecmp->rpf_mode = bcmL3EcmpUrpfInterfaceDefault;
    }

    ecmp->tunnel_priority.map_profile = ecmp_profile.tunnel_priority_map_profile;
    ecmp->tunnel_priority.mode = ecmp_profile.tunnel_priority_mode;

    /*
     * Define the dynamic mode based on the mode of the ECMP profile.
     * If it is Multiply and Divide, then the dynamic mode is disabled.
     * Otherwise, it is Resilient as only these two modes are supported currently.
     */
    ecmp->dynamic_mode =
        (ecmp_profile.ecmp_mode ==
         DBAL_ENUM_FVAL_ECMP_MODE_MULTIPLY_AND_DIVIDE) ? BCM_L3_ECMP_DYNAMIC_MODE_DISABLED :
        BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;

    /** Update statistics info */
    ecmp->stat_id = ecmp_profile.statistic_object_id;
    ecmp->stat_pp_profile = ecmp_profile.statistic_object_profile;
    ecmp->flags |= L3_EGRESS_ECMP_GET_HIER_FLAGS(hierarchy);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Fill all the ECMP profile fields
 * \param [in] unit -
 *   The unit number.
 * \param [in] ecmp -
 *   The ECMP group information.
 * \param [in] intf -
 *   A single interface from the ECMP group that can provide protection information.
 * \param [in] intf_count -
 *  Actual NOF interfaces that were added to the group.
 * \param [in] force_unique_profile -
 *  Force a unique profile, in case we want to have two identical profiles that have the same attributes but different profiles ID (used
 *  for creating a profile that will be update later for add/remove member peruses).
 * \param [in,out] ecmp_profile -
 *   Pointer the ECMP profile number.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_l3_egress_ecmp_create
 */
static shr_error_e
dnx_l3_egress_ecmp_fill_profile(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp,
    bcm_if_t intf,
    int intf_count,
    uint8 force_unique_profile,
    dnx_l3_ecmp_profile_t * ecmp_profile)
{
    uint32 ecmp_group_max_size;

    SHR_FUNC_INIT_VARS(unit);

    ecmp_group_max_size = UTILEX_MAX(intf_count, ecmp->max_paths);

    sal_memset(ecmp_profile, 0, sizeof(*ecmp_profile));

    if (ecmp->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DISABLED)
    {
        ecmp_profile->ecmp_mode = DBAL_ENUM_FVAL_ECMP_MODE_MULTIPLY_AND_DIVIDE;
    }
    else if (_SHR_IS_FLAG_SET(ecmp->ecmp_group_flags, BCM_L3_ECMP_LARGE_TABLE))
    {
        ecmp_profile->ecmp_mode = DBAL_ENUM_FVAL_ECMP_MODE_CONSISTENT_LARGE_TABLE;
    }
    else if (ecmp_group_max_size <=
             dnx_data_l3.ecmp.consistent_tables_info_get(unit,
                                                         DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_SMALL_SIZE)->max_nof_unique_members)
    {
        ecmp_profile->ecmp_mode = DBAL_ENUM_FVAL_ECMP_MODE_CONSISTENT_SMALL_TABLE;
    }
    else
    {
        ecmp_profile->ecmp_mode = DBAL_ENUM_FVAL_ECMP_MODE_CONSISTENT_MEDIUM_TABLE;
    }

    ecmp_profile->group_size = ecmp_group_max_size;

    /*
     * As we don't have the members table address at this stage the assumption is that:
     * 1. non consistent ECMP groups don't care about the members table hence: members_table_id = L3_ECMP_FULL_MEMBERS_IN_GROUP
     * 2. consistent ECMP groups that have all the member in group share the same members table so they get the same
     * value of L3_ECMP_FULL_MEMBERS_IN_GROUP and the "group size" attribute will separate two ECMP with different full group sizes.
     * 3.consistent ECMP group that are not full have a unique members table and there ECMP ID will be used.
     * 4.The force_unique_profile will be set when there is a full consistent table but a unique ID is require, this is for a full group
     * that one of the members is about to be subtracted. the CHM should gives this ECMP group the same table at first and when a member is
     * removed the CHM will know how to remove the member in a consistent why using the current full members table.
     * 5.In case the tunnel priority option isn't disable the ECMP will also get a unique ID as the TP option is most likely used
     * for a single ECMP and it is hard to manage for several ECMPs.
     */
    ecmp_profile->members_table_id = (((ecmp_profile->ecmp_mode == DBAL_ENUM_FVAL_ECMP_MODE_MULTIPLY_AND_DIVIDE)
                                       || (intf_count ==
                                           ecmp_group_max_size))
                                      && (ecmp->tunnel_priority.mode == bcmL3EcmpTunnelPriorityModeDisable)
                                      && (!force_unique_profile)) ? L3_ECMP_FULL_MEMBERS_IN_GROUP :
        L3_ECMP_PROFILE_UNIQUE_KEY(unit, BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf), intf_count);

    /*
     * Update protection state
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_protection_state_get(unit, intf, &ecmp_profile->protection_flag));

    /*
     * Update the RPF state
     */
    if (ecmp->rpf_mode == bcmL3EcmpUrpfLoose)
    {
        ecmp_profile->rpf_mode = DBAL_ENUM_FVAL_ECMP_RPF_MODE_LOOSE;
    }
    else if (ecmp->rpf_mode == bcmL3EcmpUrpfStrictEm)
    {
        ecmp_profile->rpf_mode = DBAL_ENUM_FVAL_ECMP_RPF_MODE_STRICT_EM;
    }
    else
    {
        ecmp_profile->rpf_mode = DBAL_ENUM_FVAL_ECMP_RPF_MODE_RESERVED;
    }

    if (ecmp->stat_pp_profile != STAT_PP_PROFILE_INVALID)
    {
        ecmp_profile->statistic_object_id = ecmp->stat_id;
        ecmp_profile->statistic_object_profile = ecmp->stat_pp_profile;
    }

    /*
     * Tunnel priority
     */
    ecmp_profile->tunnel_priority_map_profile = (uint8) ecmp->tunnel_priority.map_profile;

    ecmp_profile->tunnel_priority_mode = ecmp->tunnel_priority.mode;

    

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Allocates a table in the members table memory using the consistent hashing manger.
 * \param [in] unit -
 *   The unit number.
 * \param [in] ecmp -
 *   ecmp group info.
 * \param [in] fec_base -
 *   FEC base address.
 * \param [in] ecmp_profile_index -
 *   ECMP profile index.
 * \param [in] hierarchy -
 *   the ECMP hierarchy level.
 * \param [in] intf_count -
 *   NOF members in ECMP group.
 * \param [in] intf_array -
 *   ECMP members.
 * \param [out] member_table_addr -
 *   Address pointer to the member table
 */
static shr_error_e
dnx_l3_consistent_hashing_allocation(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp,
    uint32 fec_base,
    int ecmp_profile_index,
    dbal_enum_value_field_hierarchy_level_e hierarchy,
    int intf_count,
    bcm_if_t * intf_array,
    uint32 *member_table_addr)
{
    uint32 members_offset_from_base[L3_ECMP_CONSISTENT_TABLE_MAX_NOF_ENTRIES];
    uint32 member_iter;
    uint32 consistent_hashing_manager_handle;
    uint32 max_path;
    uint32 user_info;
    uint8 is_protected;
    dbal_enum_value_field_ecmp_consistent_table_e consistent_table_type;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    max_path = UTILEX_MAX(intf_count, ecmp->max_paths);

    fec_base = BCM_L3_ITF_VAL_GET(fec_base);

    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_protection_state_get(unit, intf_array[0], &is_protected));

    consistent_table_type =
        L3_ECMP_GET_CONSISTENT_TABLE_TYPE(unit, _SHR_IS_FLAG_SET(ecmp->ecmp_group_flags, BCM_L3_ECMP_LARGE_TABLE),
                                          max_path);
    /*
     * Create an array of offset from the base FEC to be used as an input for the CHM.
     */
    for (member_iter = 0; member_iter < intf_count; member_iter++)
    {
        members_offset_from_base[member_iter] =
            ((BCM_L3_ITF_VAL_GET(intf_array[member_iter]) - fec_base) >> is_protected);
    }
    SHR_IF_ERR_EXIT(ecmp_db_info.consistent_hashing_manager_handle.get(unit, &consistent_hashing_manager_handle));

    /** Having user_info declared will trigger returning the member_table_address in it. */
    SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc(unit, consistent_hashing_manager_handle,
                                                                      L3_ECMP_CHM_KEY_GET(unit, ecmp_profile_index,
                                                                                          hierarchy),
                                                                      consistent_table_type,
                                                                      L3_ECMP_NOF_CONSISTENT_TABLES_REQUIRED_BY_TP_MODE
                                                                      (ecmp->tunnel_priority.mode),
                                                                      ecmp->tunnel_priority.index, max_path, intf_count,
                                                                      members_offset_from_base, NULL,
                                                                      (void *) &user_info));

    *member_table_addr = user_info;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Allocate an ECMP profile according to the ECMP parameters and members type and count
 * \param [in] unit -
 *   The unit number.
 * \param [in] ecmp -
 *   ecmp group user information.
 * \param [in] intf_array -
 *   The ECMP group members.
 * \param [in] intf_count -
 *  NOF members in the ECMP group
 * \param [in] fec_base -
 *  the base FEC of the ECMP group
 * \param [in] force_unique_profile -
 *  allocate a unique profile for this ECMP group.
 * \param [out] profile_index -
 *  the allocated profile ID
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_l3_egress_ecmp_create
 *  * bcm_dnx_l3_egress_ecmp_add
 *  * bcm_dnx_l3_egress_ecmp_delete
 */
static shr_error_e
dnx_l3_egress_ecmp_profile_allocation(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp,
    bcm_if_t * intf_array,
    int intf_count,
    uint32 fec_base,
    uint8 force_unique_profile,
    int *profile_index)
{
    uint8 first_reference;
    dnx_l3_ecmp_profile_t ecmp_profile;
    dbal_enum_value_field_hierarchy_level_e hierarchy;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_fill_profile
                    (unit, ecmp, intf_array[0], intf_count, force_unique_profile, &ecmp_profile));

    hierarchy = L3_ECMP_GET_HIERARCHY(ecmp->flags);

    SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.ecmp_profile_hier_table.allocate_single
                    (unit, hierarchy, 0, &ecmp_profile, NULL, profile_index, &first_reference));

    /*
     * If this is the first time this profile is allocated write it into the HW profile table.
     */
    if (first_reference)
    {
        uint32 entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;
        uint32 ecmp_group_size;
        uint32 member_table_addr = L3_ECMP_MEMBER_TABLE_ADDRESS_DEFAULT_VAL;

        /*
         * If this is consistent mode then allocate an address in the members table and update the profile
         */
        if (ecmp->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT)
        {
            SHR_IF_ERR_EXIT(dnx_l3_consistent_hashing_allocation
                            (unit, ecmp, fec_base, *profile_index, hierarchy, intf_count, intf_array,
                             &member_table_addr));
        }
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE, &entry_handle_id));
        /*
         * Keys
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_GROUP_PROFILE, (uint32) *profile_index);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HIERARCHY_LEVEL, (uint32) hierarchy);
        /*
         * Values
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_MODE, INST_SINGLE, ecmp_profile.ecmp_mode);
        /** Set the ECMP members base to 0, in case of consistent hashing this field will be update by the consistent hashing manager */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_MEMBERS_BASE, INST_SINGLE,
                                     member_table_addr);

        SHR_IF_ERR_EXIT(ecmp_db_info.
                        ecmp_group_size_minus_one.set(unit, hierarchy, *profile_index, ecmp_profile.group_size - 1));

        /*
         * If the consistent large table is used the HW ECMP group size isn't the actual ECMP group size but the NOF entries
         * that are been used in the large table divided by 2.
         */
        if (ecmp_profile.ecmp_mode == DBAL_ENUM_FVAL_ECMP_MODE_CONSISTENT_LARGE_TABLE)
        {
            dnx_l3_ecmp_consistent_large_table_nof_valid_entries_get(unit, ecmp_profile.group_size, &ecmp_group_size);
            ecmp_group_size >>= 1;
        }
        else
        {
            ecmp_group_size = ecmp_profile.group_size;
        }

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_GROUP_SIZE_MINUS_ONE, INST_SINGLE,
                                     ecmp_group_size - 1);

        /*
         * Write statistic data
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, ecmp->stat_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE,
                                     STAT_PP_PROFILE_ID_GET(ecmp->stat_pp_profile));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTECTION_ENABLE, INST_SINGLE,
                                     ecmp_profile.protection_flag);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_RPF_MODE, INST_SINGLE,
                                     ecmp_profile.rpf_mode);

        if (dnx_data_l3.ecmp.tunnel_priority_support_get(unit))
        {
            /*
             * The tunnel_priority_mode enumeration matches the HW values for now, so no need to convert the values.
             */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_TUNNEL_PRIORITY_MODE, INST_SINGLE,
                                         ecmp_profile.tunnel_priority_mode);

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_TUNNEL_PRIORITY_MAP_PROFILE,
                                         INST_SINGLE, ecmp_profile.tunnel_priority_map_profile);
        }

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function removes an ECMP group and frees the space it
 *  has occupied both HW and SW.
 * \param [in] unit -
 *   The unit number.
 * \param [in] ecmp_profile_index -
 *   The ID of the ECMP group profile
 * \param [in] ecmp_hierarchy -
 *   The ECMP hierarchy.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_l3_egress_ecmp_destroy
 *  * bcm_dnx_l3_egress_ecmp_add
 *  * bcm_dnx_l3_egress_ecmp_delete
 */
static shr_error_e
dnx_l3_egress_ecmp_profile_free(
    int unit,
    uint32 ecmp_profile_index,
    dbal_enum_value_field_hierarchy_level_e ecmp_hierarchy)
{
    uint32 entry_handle_id;
    uint32 member_base_address;
    uint32 chm_offset;
    uint8 last_reference = 0;
    int ref_count = 0;
    dnx_l3_ecmp_profile_t ecmp_profile;
    sw_state_ll_node_t node;
    sw_state_ll_node_t *node_ptr;
    uint32 profile_key, profile_key_expected;
    dbal_enum_value_field_ecmp_mode_e ecmp_mode;
    dbal_enum_value_field_ecmp_consistent_table_e consistent_table_type;
    uint32 consistent_hashing_manager_handle;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.ecmp_profile_hier_table.profile_data_get
                    (unit, ecmp_hierarchy, ecmp_profile_index, &ref_count, &ecmp_profile));
    /*
     * If the reference counter indicates 1, then later we will receive last reference indication.
     * DBAL entry clear needs to happen before template free.
     */
    if (ref_count == 1)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE, &entry_handle_id));

        /*
         * Receive the ECMP group profile index and use it to retrieve the ECMP group hashing type.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_GROUP_PROFILE, ecmp_profile_index);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HIERARCHY_LEVEL, ecmp_hierarchy);

        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ECMP_MODE, INST_SINGLE, &ecmp_mode);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ECMP_MEMBERS_BASE, INST_SINGLE,
                                   &member_base_address);

        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        /*
         * In case this a consistent mode remove this profile from the members address linked list and
         * release the profile from consistent hashing manager reference
         */
        if ((ecmp_mode != DBAL_ENUM_FVAL_ECMP_MODE_MULTIPLY_AND_DIVIDE))
        {

            /*
             * If this was the last profile, remove it from the link list of the matching resource of the address
             */
            SHR_IF_ERR_EXIT(dnx_l3_ecmp_consistent_table_type_from_ecmp_mode_get
                            (unit, ecmp_mode, &consistent_table_type));

            chm_offset = member_base_address * L3_ECMP_CONSISTENT_NOF_RESOURCES_TAKEN(unit, consistent_table_type);

            node_ptr = &node;

            profile_key_expected = L3_ECMP_CHM_KEY_GET(unit, ecmp_profile_index, ecmp_hierarchy);

            SHR_IF_ERR_EXIT(ecmp_db_info.members_tbl_addr_profile_link_lists.get_first(unit, chm_offset, node_ptr));

            /*
             * Iterate over the linked list to find the profile to remove.
             */
            while (DNX_SW_STATE_LL_IS_NODE_VALID(node))
            {
                SHR_IF_ERR_EXIT(ecmp_db_info.members_tbl_addr_profile_link_lists.node_value(unit, *node_ptr,
                                                                                            &profile_key));

                if (profile_key == profile_key_expected)
                {
                    SHR_IF_ERR_EXIT(ecmp_db_info.
                                    members_tbl_addr_profile_link_lists.remove_node(unit, chm_offset, *node_ptr));
                    break;
                }

                SHR_IF_ERR_EXIT(ecmp_db_info.members_tbl_addr_profile_link_lists.next_node(unit, chm_offset, *node_ptr,
                                                                                           node_ptr));

            }

            if (!DNX_SW_STATE_LL_IS_NODE_VALID(node))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Released profile %d (hierarchy %d) wasn't found in the members addresses %d linked list,",
                             ecmp_profile_index, ecmp_hierarchy, chm_offset);
            }

            /*
             * free the consistent hashing manager from this profile
             */
            SHR_IF_ERR_EXIT(ecmp_db_info.
                            consistent_hashing_manager_handle.get(unit, &consistent_hashing_manager_handle));

            SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free
                            (unit, consistent_hashing_manager_handle,
                             L3_ECMP_CHM_KEY_GET(unit, ecmp_profile_index, ecmp_hierarchy), consistent_table_type));
        }

        /*
         * If this is the last reference of this profile, remove it from the HW profile table.
         * This HW access is not mandatory but will prevent the DBAL from showing unused profiles.
         */

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE, &entry_handle_id));
        /** Keys */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_GROUP_PROFILE, (uint32) ecmp_profile_index);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HIERARCHY_LEVEL, (uint32) ecmp_hierarchy);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

        /*
         * Clear the group size from the SW state
         */
        SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_group_size_minus_one.set(unit, ecmp_hierarchy, ecmp_profile_index, 0));
    }
    /*
     * release the profile reference from the template manager
     */
    SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.ecmp_profile_hier_table.free_single
                    (unit, ecmp_hierarchy, ecmp_profile_index, &last_reference));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Create or update an ECMP group entry based on the ECMP ID.
 * \param [in] unit -
 *   The unit number.
 * \param [in] ecmp_intf -
 *   The ECMP group id.
 * \param [in] ecmp_profile_id -
 *   The ECMP profile (composed of all the ECMP attributes as
 *    defined in the HW profile definition).
 * \param [in] fec_base -
 *   FEC base pointer - first member in ECMP group
 * \param [in] rpf_mode_is_em -
 *   In case the RPF mode is EM this bit should be set to one, else it should remain 0.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_l3_egress_ecmp_add
 *  * bcm_dnx_l3_egress_ecmp_delete
 *  * bcm_dnx_l3_egress_ecmp_create
 */
static shr_error_e
dnx_l3_egress_ecmp_entry_set(
    int unit,
    bcm_if_t ecmp_intf,
    uint32 ecmp_profile_id,
    uint32 fec_base,
    uint32 rpf_mode_is_em)
{
    uint32 entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_TABLE, &entry_handle_id));
    /*
     * Keys
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_ID, ecmp_intf);
    /*
     * Values
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_GROUP_PROFILE, INST_SINGLE, ecmp_profile_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_POINTER_BASE, INST_SINGLE,
                                 BCM_L3_ITF_VAL_GET(fec_base));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RPF_MODE_IS_EM, INST_SINGLE,
                                 rpf_mode_is_em != 0 ? 1 : 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Allocate an ECMP group entry using the resource manager.
 * \param [in] unit - The unit number.
 * \param [in] ecmp_index - The ECMP group id.
 * \param [in] ecmp_flags - Control flags for the ECMP group
 *        * BCM_L3_WITH_ID - the group will be allocated with the given ID
 *        * BCM_L3_2ND_HIERARCHY - the group belongs to the 2nd hierarchy
 *        * BCM_L3_3RD_HIERARCHY - the group belongs to the 3rd hierarchy
 * \param [in] ecmp_group_flags - Additional control flags for the ECMP group.
 *        * BCM_L3_ECMP_EXTENDED - indicate that the group will be in the extended range.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_l3_egress_ecmp_create
 */
static shr_error_e
dnx_l3_egress_ecmp_entry_alloc(
    int unit,
    bcm_if_t * ecmp_index,
    uint32 ecmp_flags,
    uint32 ecmp_group_flags)
{
    uint32 hierarchy = 0;
    int is_first;
    int bank_id;
    uint32 entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    hierarchy = L3_ECMP_GET_HIERARCHY(ecmp_flags);

    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_allocate(unit, ecmp_index, ecmp_flags, ecmp_group_flags, hierarchy));
    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_group_is_alone_in_bank(unit, *ecmp_index, &bank_id, &is_first));
    /*
     * If the function dnx_algo_l3_ecmp_group_is_alone_in_bank returns indication that the group we are currently
     * allocating is the first one in the bank of the stage map (is_first = 1), then we need to update the DBAL table
     * responsible for saving the ECMP bank to stage mapping.
     */
    if (is_first == 1)
    {
        /** Update the DBAL table with the hierarchy value for the given bank_id (instance) */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_BANK_TO_STAGE_MAP, &entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HIERARCHY_LEVEL, bank_id, hierarchy);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Remove an ECMP group entry from the DBAL table
 * \param [in] unit - The unit number.
 * \param [in] ecmp_index - The ECMP group id.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_l3_egress_ecmp_create
 */
static shr_error_e
dnx_l3_egress_ecmp_entry_remove(
    int unit,
    uint32 ecmp_index)
{
    uint32 entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Delete the DBAL entry from the ECMP table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_TABLE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_ID, ecmp_index);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Deallocate an ECMP group entry using the resource manager.
 *   Update the bank to hierarchy mapping if needed.
 * \param [in] unit - The unit number.
 * \param [in] ecmp_index - The ECMP group id.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_l3_egress_ecmp_create
 */
static shr_error_e
dnx_l3_egress_ecmp_entry_dealloc(
    int unit,
    bcm_if_t ecmp_index)
{
    uint32 init_hierarchy = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1;
    int is_last;
    int bank_id;
    uint32 entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_group_is_alone_in_bank(unit, ecmp_index, &bank_id, &is_last));
    /*
     * If the function dnx_algo_l3_ecmp_group_is_alone_in_bank returns indication that the group we are currently
     * deallocating is the last one in the bank of the hierarchy map (is_last = 1), then we need to update the DBAL table
     * responsible for saving the ECMP bank to hierarchy mapping.
     */
    if (is_last == 1)
    {
        /** Update the DBAL table with the hierarchy value for the given bank_id (instance). */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_BANK_TO_STAGE_MAP, &entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HIERARCHY_LEVEL, bank_id, init_hierarchy);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_deallocate(unit, ecmp_index));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verifies the validity of the new added member into the existing ECMP group
 * \param [in] unit -
 *   The unit number.
 * \param [in] ecmp_protection_state -
 *   The ECMP group protection state.
 * \param [in] ecmp_hierarchy -
 *   The ECMP group hierarchy.
 * \param [in] intf - the added member to the existsting ECMP group.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_l3_egress_ecmp_add
 */
static shr_error_e
dnx_l3_egress_ecmp_member_verify(
    int unit,
    uint8 ecmp_protection_state,
    dbal_enum_value_field_hierarchy_level_e ecmp_hierarchy,
    bcm_if_t intf)
{
    uint32 hierarchy;
    uint32 max_fec_id;
    uint8 is_allocated = 0;
    uint8 new_member_protection_satet;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_db_infos.max_fec_id_value.get(unit, &max_fec_id));
    /*
     * Verify the intf value - make sure it's not less than 0 or more than the maximum number of FECs.
     */
    if (BCM_L3_ITF_VAL_GET(intf) > max_fec_id)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The FEC value %d is out of range - should be non-negative and no higher than %d.",
                     intf, max_fec_id);
    }

    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_is_allocated(unit, BCM_L3_ITF_VAL_GET(intf), &is_allocated));
    if (is_allocated == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ECMP member FEC with ID %d has not been allocated\n", intf);
    }

    /** Get hierarchy of intf */
    SHR_IF_ERR_EXIT(algo_l3_fec_hierarchy_stage_map_get(unit, BCM_L3_ITF_VAL_GET(intf), &hierarchy));

    if (hierarchy != ecmp_hierarchy)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "FEC member %d is in hierarchy %d but other existing members are in hierarchy %d", intf,
                     hierarchy, ecmp_hierarchy);
    }

    /** Get the new member protection state */
    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_protection_state_get(unit, intf, &new_member_protection_satet));

    if (new_member_protection_satet != ecmp_protection_state)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "New member %d protection state %d isn't matching the ECMP group protection state %d.", intf,
                     new_member_protection_satet, ecmp_protection_state);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verify the ECMP delete member API input
 * \param [in] unit -
 *   The unit number.
 * \param [in] ecmp - The ECMP group information.
 * \param [in] ecmp_atr - The ECMP field attributes
 * \param [in] profile_data - The ECMP profile attributes
 * \param [in] ecmp_hierarchy - The ECMP group hierarchy.
 * \param [in] intf_count - The members array size.
 * \param [in] members - The current members in the ECMP group.
   \param [in] intf - the interface that about to be remove from the group.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
static shr_error_e
dnx_l3_egress_ecmp_delete_verify(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp,
    dnx_l3_ecmp_attributes_t * ecmp_atr,
    dnx_l3_ecmp_profile_t * profile_data,
    dbal_enum_value_field_hierarchy_level_e ecmp_hierarchy,
    int intf_count,
    bcm_if_t * members,
    bcm_if_t intf)
{
    int intf_iter;
    int ecmp_index = BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf);
    SHR_FUNC_INIT_VARS(unit);

    /*
     * The ECMP null check and index value verification is placed in the dnx_l3_ecmp_attributes_get function that is called
     * before this verification as well as the TP index check
     */

    /*
     * Verify that the specified dynamic_mode in the ecmp structure. 
     */
    if (!L3_ECMP_IS_DYNAMIC_MODE_SUPPORTED(ecmp->dynamic_mode))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Specified dynamic mode, %d, is not supported. \n", ecmp->dynamic_mode);
    }

    /*
     * Verify that we are not attempting to delete the last member of an ECMP group.
     * If so, return an error that this action is illegal.
     */
    if (intf_count == 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal action deleting last member of ECMP group. It cannot have zero members.\n");
    }

    if (profile_data->ecmp_mode == DBAL_ENUM_FVAL_ECMP_MODE_MULTIPLY_AND_DIVIDE)
    {
        if ((ecmp_atr->fec_base != intf)
            && (ecmp_atr->fec_base + ((intf_count - 1) << profile_data->protection_flag) != intf))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Invalid value %u for removing a member from an ECMP group, must be the first(%u) or last(%u) member in the ECMP group \n",
                         intf, ecmp_atr->fec_base,
                         ecmp_atr->fec_base + ((intf_count - 1) << profile_data->protection_flag));
        }
    }
    else
    {
        int remove_pos = -1;
        /*
         * Verify that the intf value is in range.
         */
        if ((((profile_data->group_size << profile_data->protection_flag) + ecmp_atr->fec_base) <= intf)
            || (intf < ecmp_atr->fec_base))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid value %u for deleting a member of ECMP group %u\n", intf, ecmp_index);
        }

        for (intf_iter = 0; intf_iter < intf_count; intf_iter++)
        {
            if (members[intf_iter] == intf)
            {
                remove_pos = intf_iter;
                break;
            }
        }

        if (remove_pos == -1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Interface %d to be removed was not found in ECMP group %d members\n",
                         intf, ecmp_index);
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *  Verify the ECMP add member API input
 * \param [in] unit -
 *   The unit number.
 * \param [in] ecmp - The ECMP group information.
 * \param [in] ecmp_atr - The ECMP field attributes
 * \param [in] profile_data - The ECMP profile attributes
 * \param [in] ecmp_hierarchy - The ECMP group hierarchy.
 * \param [in] intf_count - The members array size.
 * \param [in] members - The current members in the ECMP group.
   \param [in] intf - the interface that about to be added to the group.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
static shr_error_e
dnx_l3_egress_ecmp_add_verify(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp,
    dnx_l3_ecmp_attributes_t * ecmp_atr,
    dnx_l3_ecmp_profile_t * profile_data,
    dbal_enum_value_field_hierarchy_level_e ecmp_hierarchy,
    int intf_count,
    bcm_if_t * members,
    bcm_if_t intf)
{
    int intf_iter;
    int ecmp_index = BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf);
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that the specified dynamic_mode in the ecmp structure. 
     */
    if (!L3_ECMP_IS_DYNAMIC_MODE_SUPPORTED(ecmp->dynamic_mode))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Specified dynamic mode, %d, is not supported. \n", ecmp->dynamic_mode);
    }

    /*
     * The ECMP null check and index value verification is placed in the dnx_l3_ecmp_attributes_get function that is called
     * before this verification as well as the TP index check
     */

    if (profile_data->ecmp_mode == DBAL_ENUM_FVAL_ECMP_MODE_MULTIPLY_AND_DIVIDE)
    {
        if (profile_data->group_size == DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The ECMP already contains the maximal number of members %d\n",
                         DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE);
        }

        if ((ecmp_atr->fec_base != intf + (1 + profile_data->protection_flag))
            && (ecmp_atr->fec_base + (intf_count << profile_data->protection_flag) != intf))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid value %u for new member of ECMP group %u\n", intf, ecmp_index);
        }

    }
    else
    {
        /*
         * Verify that the intf value is in range.
         */
        if (((profile_data->group_size << profile_data->protection_flag) + ecmp_atr->fec_base <= intf)
            || intf < ecmp_atr->fec_base)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Invalid value %u for new member of ECMP group %u - expecting value between %u and %u\n",
                         intf, ecmp_index, ecmp_atr->fec_base,
                         (profile_data->group_size << profile_data->protection_flag) + ecmp_atr->fec_base);
        }

        for (intf_iter = 0; intf_iter < intf_count; intf_iter++)
        {
            if (members[intf_iter] == intf)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Interface %u already exists in ECMP group %u\n", intf, ecmp_index);
            }
        }
    }

    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_member_verify(unit, profile_data->protection_flag, ecmp_hierarchy, intf));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verify the ECMP get member API input
 * \param [in] unit -
 *   The unit number.
 * \param [in] ecmp - The ECMP group information.
 * \param [in] intf_size - The size of the provided array to update the ECMP group members in,
 * \param [in] intf_array - The array that was provided to get the ECMP group members.
 * \param [in] intf_count - This pointer will return the actual NOF members in the ECMP group..
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
static shr_error_e
dnx_l3_egress_ecmp_get_verify(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp,
    int intf_size,
    bcm_if_t * intf_array,
    int *intf_count)
{
    uint32 ecmp_hierarchy;
    dnx_l3_ecmp_attributes_t ecmp_atr;
    dnx_l3_ecmp_profile_t profile_data;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(intf_array, _SHR_E_PARAM, "bcm_if_t");
    SHR_NULL_CHECK(intf_count, _SHR_E_PARAM, "int");

    /*
     * The ECMP null check and index value verification is placed in this function as well as the TP index check
     */
    SHR_IF_ERR_EXIT(dnx_l3_ecmp_attributes_get(unit, ecmp, &ecmp_hierarchy, &ecmp_atr, &profile_data));

    if (intf_size < 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The provided array size (%d) to for receiving the ECMP members must be a positive number.",
                     intf_size);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verifies the user input for the ECMP create function.
 * \param [in] unit -
 *   The unit number.
 * \param [in] ecmp -
 *   The ECMP group information.
 * \param [in] intf_count -
 *   The ECMP current number of members in the group.
 * \param [in] intf_array -
 *  Array of the ECMP members.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_l3_egress_ecmp_create
 */
static shr_error_e
dnx_l3_egress_ecmp_create_verify(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp,
    int intf_count,
    bcm_if_t * intf_array)
{
    uint32 fec_index;
    uint32 min_fec;
    uint32 max_fec;
    uint32 hierarchy = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY;
    uint32 fec_hierarchy = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY;
    uint32 ecmp_hierarchy;
    int fec_iter;
    int group_size;
    uint8 fec_duplicates[DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE];
    uint8 is_allocated = 0;
    uint8 is_protected = 0, fec_protection = 0;
    uint32 max_fec_id;
    uint32 max_group_size;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(intf_array, _SHR_E_PARAM, "bcm_if_t");
    SHR_NULL_CHECK(ecmp, _SHR_E_PARAM, "bcm_l3_egress_ecmp_t");

    /*
     * Verify the flags supplied to the ECMP structure.
     */
    if (_SHR_IS_FLAG_SET(ecmp->flags, ~L3_ECMP_SUPPORTED_BCM_L3_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "One or more of the used flags 0x%x aren't supported (supported flags are 0x%x).",
                     ecmp->flags, L3_ECMP_SUPPORTED_BCM_L3_FLAGS);
    }

    /*
     * Verify that the specified dynamic_mode in the ecmp structure. 
     */
    if (!L3_ECMP_IS_DYNAMIC_MODE_SUPPORTED(ecmp->dynamic_mode))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Specified dynamic mode, %d, is not supported. \n", ecmp->dynamic_mode);
    }

    /*
     * The max group size of the consistent mode and the multiply and divide mode can be different.
     */
    max_group_size =
        1 << ((ecmp->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DISABLED) ? dnx_data_l3.
              ecmp.group_size_multiply_and_divide_nof_bits_get(unit) : dnx_data_l3.
              ecmp.group_size_consistent_nof_bits_get(unit));

    ecmp_hierarchy = L3_ECMP_GET_HIERARCHY(ecmp->flags);

    if (_SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_WITH_ID))
    {
        /*
         * Verify the ecmp ID value is valid.
         */
        if ((BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf) >= dnx_data_l3.ecmp.nof_ecmp_get(unit)
             || BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf) < dnx_data_l3.fec.first_valid_fec_ecmp_id_get(unit)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "ECMP %d is out of range, ECMP id must be below %d and above 0.",
                         BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf), dnx_data_l3.ecmp.nof_ecmp_get(unit));
        }
        /*
         * In case no ECMP ID was specified an unallocated ECMP ID will be taken, so there is no need to check the allocation state.
         */
        SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_is_allocated(unit, BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf), &is_allocated));
        /*
         * To update ECMP group, it must already exist.
         */
        if (_SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_REPLACE))
        {
            uint32 current_ecmp_hierarchy;

            if (!is_allocated)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "BCM_L3_REPLACE flag was used but the ECMP group with ID = %d is not allocated.\n",
                             BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf));
            }

            /*
             * Get the current ECMP hierarchy
             */
            SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_hierarchy_get
                            (unit, BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf), &current_ecmp_hierarchy));

            if (current_ecmp_hierarchy != ecmp_hierarchy)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Replacing the ECMP hierarchy using the BCM_L3_REPLACE isn't supported, please delete the current ECMP (%d) and create a new one. \n",
                             BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf));
            }
        }
        else if (is_allocated && (!_SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_REPLACE)))
        {
            SHR_ERR_EXIT(_SHR_E_EXISTS, "ECMP group %d is already allocated please use the BCM_L3_REPLACE flag.\n",
                         BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf));
        }
        /*
         * Verify the ecmp ID value is valid.
         */
        if ((BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf) >= dnx_data_l3.ecmp.nof_ecmp_get(unit)
             || BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf) < dnx_data_l3.fec.first_valid_fec_ecmp_id_get(unit))
            && _SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_WITH_ID))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "ECMP %d is out of range, ECMP id must be below %d and above 0.",
                         BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf), dnx_data_l3.ecmp.nof_ecmp_get(unit));
        }
        if (_SHR_IS_FLAG_SET(ecmp->ecmp_group_flags, BCM_L3_ECMP_EXTENDED)
            && (!L3_ECMP_INDEX_IS_IN_EXTENDED_RANGE(unit, BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf))))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "The ECMP group ID %d is not in the extended range but flag BCM_L3_ECMP_EXTENDED is provided.",
                         BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf));
        }
        if (!_SHR_IS_FLAG_SET(ecmp->ecmp_group_flags, BCM_L3_ECMP_EXTENDED)
            && (L3_ECMP_INDEX_IS_IN_EXTENDED_RANGE(unit, BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf))))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "The ECMP group ID %d is in the extended range but flag BCM_L3_ECMP_EXTENDED is not provided.",
                         BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf));
        }
    }
    else
    {
        if (BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "WITH_ID flag is not provided, but user has specified an ECMP interface %d.\n",
                         BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf));
        }
        /*
         * Using the replace flag requieres a the "with ID" flag (to indicate the replaced ECMP member).
         */
        if (_SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_REPLACE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Using the BCM_L3_REPLACE flag is only valid when used with BCM_L3_WITH_ID.\n");
        }
    }

    if (_SHR_IS_FLAG_SET(ecmp->ecmp_group_flags, ~L3_ECMP_SUPPORTED_BCM_L3_ECMP_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal L3 ECMP flags are used.");
    }

    if (_SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_2ND_HIERARCHY) && _SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_3RD_HIERARCHY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Two hierarchy levels were selected when a single hierarchy per ECMP is supported.");
    }

    /*
     * Verify that the max_paths value is valid - between 0 and the ecmp maximum size of a group.
     */
    if ((ecmp->max_paths < 0) || (ecmp->max_paths > max_group_size))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The value %d isn't a valid value for max_paths, the maximal number of paths must be a positive number smaller than %d or 0 if unused.",
                     ecmp->max_paths, max_group_size);
    }

    /*
     * Verify that the max_paths value is valid and that its value is bigger than the number of interfaces
     * passed to the function.
     */
    if (ecmp->max_paths != 0 && intf_count > ecmp->max_paths)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "ECMP number of paths %d exceeds the defined maximal number of path %d.", intf_count,
                     ecmp->max_paths);
    }

    if (intf_count <= 0 || intf_count > max_group_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Invalid number of interfaces value %d, should be positive and less than or equal to %d.",
                     intf_count, max_group_size);
    }

    if (ecmp->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DISABLED
        && (ecmp->max_paths != 0 && ecmp->max_paths != intf_count))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Incompatible max_paths %u and intf_count %d values in multiply and divide mode. They should be equal to one another.\n",
                     ecmp->max_paths, intf_count);
    }

    /*
     * As we don't assume sorted array of interface, we need to check that the lowest and highest FECs are in the range
     * of the group size and that the interfaces are not duplicated in the array.
     * In case of multiply and divide hashing where the NOF interfaces equal to the group size,
     * if the high and low FECs are in range and there are no duplication then all the FECs are consecutive.
     */
    group_size = UTILEX_MAX(ecmp->max_paths, intf_count);
    sal_memset(fec_duplicates, 0, sizeof(uint8) * DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE);
    SHR_IF_ERR_EXIT(mdb_db_infos.max_fec_id_value.get(unit, &max_fec_id));

    /*
     * Verify that that all the ECMP group members are
     *  - allocated
     *  - in the same hierarchy
     *  - in the same protection state
     *  - in range
     *  - not duplicated
     */
    for (fec_iter = 0, max_fec = 0, min_fec = max_fec_id; fec_iter < intf_count; fec_iter++)
    {
        fec_index = BCM_L3_ITF_VAL_GET(intf_array[fec_iter]);
        /** Confirm the object existence with FEC allocation mngr */
        SHR_IF_ERR_EXIT(dnx_algo_l3_fec_is_allocated(unit, fec_index, &is_allocated));
        if (is_allocated == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "ECMP member FEC with ID %d has not been allocated\n", intf_array[fec_iter]);
        }
        /** Get hierarchy from fec_id */
        SHR_IF_ERR_EXIT(algo_l3_fec_hierarchy_stage_map_get(unit, fec_index, &hierarchy));
        /** Get the FEC protection state */
        SHR_IF_ERR_EXIT(dnx_algo_l3_fec_protection_state_get(unit, fec_index, &fec_protection));
        /*
         * If checking hierarchy of first element in array, copy the hierarchy value to fec_hierarchy
         * and continue to next iteration.
         */
        if (fec_iter == 0)
        {
            fec_hierarchy = hierarchy;
            is_protected = fec_protection;

        }
        /** If hierarchy of FEC member doesn't match previously found hierarchies, return an error. */
        else if (fec_hierarchy != hierarchy)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Hierarchy value %d of FEC member %d doesn't match previously found value - %d\n",
                         hierarchy, intf_array[fec_iter], fec_hierarchy);
        }
        else if (fec_protection != is_protected)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Protection state %d of FEC member %d doesn't match other ECMP members protection state %d.\n",
                         fec_protection, intf_array[fec_iter], is_protected);
        }
        /** Make sure that if a FEC is protected the primary one is the one that was provided */
        if (fec_protection && (intf_array[fec_iter] % 2 == 1))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "In case protected FECs are used as ECMP group members, only the primary FECs (even numbered FECs, in this case %d) in the protection pair should be provided to the ECMP API and the not secondary FECs in the protection (odd numbered FECs, as provided in this case %d).",
                         intf_array[fec_iter] - 1, intf_array[fec_iter]);
        }

        if (fec_index > max_fec_id)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "FEC index %d is out of range, max FEC index is %d.", fec_index, max_fec_id);
        }

        /** get min and get max */
        max_fec = UTILEX_MAX(fec_index, max_fec);
        min_fec = UTILEX_MIN(fec_index, min_fec);
    }
    /*
     * Verify that the min FEC and the max FEC are in range regarding the group size.
     */
    if (min_fec + (group_size << is_protected) <= max_fec)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Group size %d is incompatible with FEC min member %u and max member %u.",
                     group_size, min_fec, max_fec);
    }
    /*
     * Check for duplicates.
     * Map each member of the interface array to the fec_duplicates array based on its value.
     */
    for (fec_iter = 0; fec_iter < intf_count; fec_iter++)
    {
        fec_index = BCM_L3_ITF_VAL_GET(intf_array[fec_iter]);
        if (fec_duplicates[(fec_index - min_fec) >> is_protected] == 1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "FEC ID %d is duplicated in the interface array.", fec_index);
        }
        fec_duplicates[(fec_index - min_fec) >> is_protected] = 1;
    }

    /** Verify that the ECMP members have equal or higher hierarchy than the ECMP. */
    if (fec_hierarchy != ecmp_hierarchy)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Hierarchy value %d of the FEC members must be the same as the hierarchy of the ECMP group - %d\n",
                     fec_hierarchy, ecmp_hierarchy);
    }

    /*
     * Statistics check
     */
    if (ecmp->stat_id > L3_ECMP_MAX_STAT_ID_VALUE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Stat id %d, max value 0x%x", ecmp->stat_id, L3_ECMP_MAX_STAT_ID_VALUE);
    }
    SHR_IF_ERR_EXIT(dnx_stat_pp_crps_verify
                    (unit, BCM_CORE_ALL, ecmp->stat_id, ecmp->stat_pp_profile,
                     bcmStatCounterInterfaceIngressReceivePp));

    /*
     * Tunnel priority check
     */
    if (ecmp->tunnel_priority.mode == bcmL3EcmpTunnelPriorityModeDisable)
    {
        if (ecmp->tunnel_priority.map_profile != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "All the tunnel priority values should be set to zero in case the tunnel priority mode is disable.\n");
        }
    }
    else
    {
        if (!dnx_data_l3.ecmp.tunnel_priority_support_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "tunnel priority isn't supported in this device and should be disabled.\n");
        }

        if ((ecmp->tunnel_priority.mode < bcmL3EcmpTunnelPriorityModeDisable)
            || (ecmp->tunnel_priority.mode > bcmL3EcmpTunnelPriorityModeEightPriorities))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The ECMP tunnel priority mode value %d isn't supported.\n",
                         ecmp->tunnel_priority.mode);
        }

        SHR_IF_ERR_EXIT(algo_l3_db.tp_modes.l3_ecmp_tunnel_priority_modes_res_mngr.is_allocated(unit, ecmp_hierarchy,
                                                                                                ecmp->
                                                                                                tunnel_priority.map_profile,
                                                                                                &is_allocated));
        if (!is_allocated)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "The TP map profile %d wasn't allocated yet.",
                         ecmp->tunnel_priority.map_profile);
        }

        if (ecmp->dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Resilient dynamic mode must be used in case the tunnel priority isn't disabled.\n");
        }

        if (intf_count != 1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "When creating an ECMP with tunnel priority only a single FEC should be passed which is the base FEC of the ECMP group.\n");
        }
    }

    if ((ecmp->tunnel_priority.index != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The tunnel priority index must be set to zero, in case the tunnel priority is used the bcm_l3_egress_ecmp_tunnel_priority_set API is used to update the tables.\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Removes an ECMP entry.
 * \param [in] unit -
 *   The unit number.
 * \param [in] ecmp_index -
 *   The ECMP ID
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_l3_egress_ecmp_group_destroy(
    int unit,
    uint32 ecmp_index)
{
    dbal_enum_value_field_hierarchy_level_e ecmp_hierarchy;
    dnx_l3_ecmp_attributes_t ecmp_atr;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get the real hierarchy ID.
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_hierarchy_get(unit, ecmp_index, (uint32 *) &ecmp_hierarchy));

    /*
     * Get the ECMP attributes
     */
    SHR_IF_ERR_EXIT(dnx_l3_ecmp_group_entry_result_get(unit, ecmp_index, &ecmp_atr));

    /** Delete the entry from the DBAL table */
    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_entry_remove(unit, ecmp_index));

    /*
     * Deallocate the ECMP group from the resource manager
     */
    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_entry_dealloc(unit, ecmp_index));

    /*
     * Release the profile
     */
    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_profile_free(unit, ecmp_atr.profile_index, ecmp_hierarchy));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Creates or updates an ECMP entry.
 * \param [in] unit -
 *   The unit number.
 * \param [in,out] ecmp -
 *   The ECMP group information.
 *   The following are relevant field of the bcm_l3_egress_ecmp_t structure:
 *  - flags            -> BCM_L3_* flags (supported flags are BCM_L3_REPLACE and BCM_L3_WITH_ID).
 *  - ecmp_intf        -> The ECMP object.
 *  - max_paths        -> The maximal size of the ECMP group size.
 *                        if max_paths = 0 then it is ignore, otherwise -  ECMP max group size => max_paths >= intf_count.
 *                        In general members can be added to the group till it reaches MAX(max_paths,intf_count) size but no more than that.
 *  - ecmp_group_flags -> BCM_L3_ECMP_* flags (supported flags are BCM_L3_ECMP_LARGE_TABLE, BCM_L3_ECMP_EXTENDED).
 *  - dynamic_mode     -> The ECMP hashing mode, Indicate if the hashing mode is consistent of non-consistent using multiply and divide.
 *  - rpf_mode         -> Indicates the URPF mode for the ECMP group - loose, strict or disabled.
 * \param [in] intf_count -
 *   The ECMP current number of members in the group.
 * \param [in] intf_array -
 *  Array of the ECMP members either FEC-IDs or ECMP-IDs.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
int
bcm_dnx_l3_egress_ecmp_create(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp,
    int intf_count,
    bcm_if_t * intf_array)
{
    int ecmp_profile_index;
    int prev_ecmp_profile_index = -1;
    dbal_enum_value_field_hierarchy_level_e prev_ecmp_hierarchy = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY;
    uint32 fec_base;
    int intf_iter;
    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_START_MODULE_ID(unit, bcmModuleL3EgressEcmp);
    DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR_PTR(unit, sizeof(bcm_l3_egress_ecmp_t), ecmp);

    /*
     * If mode is VIP, branch off to VIP ECMP processing 
     */
    SHR_NULL_CHECK(ecmp, _SHR_E_PARAM, "bcm_l3_egress_ecmp_t");
    if (ecmp->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_VIP)
    {
        SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_vip_create(unit, ecmp, intf_count, intf_array));
        SHR_EXIT();
    }

    L3_EGRESS_ECMP_INVOKE_VERIFY_IF_ENABLE(unit, dnx_l3_egress_ecmp_create_verify(unit, ecmp, intf_count, intf_array));

    /** Remove ecmp_intf type encapsulation before starting */
    ecmp->ecmp_intf = BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf);

    /*
     * To avoid traffic loss, in case the ECMP is replaced (updated) the old profile index and the hierarchy will be stored
     * in order to released the current profile after a new profile was created and updated to the existing ECMP.
     */
    if (_SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_REPLACE))
    {
        dnx_l3_ecmp_attributes_t ecmp_atr;

        /*
         * Get the previous hierarchy ID.
         */
        SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_hierarchy_get(unit, ecmp->ecmp_intf, (uint32 *) &prev_ecmp_hierarchy));

        /*
         * Get the previous ECMP index ID
         */
        SHR_IF_ERR_EXIT(dnx_l3_ecmp_group_entry_result_get(unit, ecmp->ecmp_intf, &ecmp_atr));

        prev_ecmp_profile_index = ecmp_atr.profile_index;

    }
    else
    {
        /*
         * Allocate the ECMP group using the resource manager.
         */
        SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_entry_alloc(unit, &ecmp->ecmp_intf, ecmp->flags, ecmp->ecmp_group_flags));

    }
    /*
     * As the interface array isn't necessarily sorted we need to search for the base FEC.
     */
    fec_base = intf_array[0];
    for (intf_iter = 1; intf_iter < intf_count; intf_iter++)
    {
        fec_base = UTILEX_MIN(fec_base, intf_array[intf_iter]);
    }

    /*
     * Allocate an ECMP profile
     */
    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_profile_allocation
                    (unit, ecmp, intf_array, intf_count, fec_base, FALSE, &ecmp_profile_index));

    /*
     * Insert the ECMP group entry into the DBAL table.
     */
    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_entry_set
                    (unit, ecmp->ecmp_intf, ecmp_profile_index, fec_base,
                     (ecmp->rpf_mode == bcmL3EcmpUrpfStrictEm ? 1 : 0)));

    if (_SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_REPLACE))
    {
        /*
         * Release the old profile.
         */
        SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_profile_free(unit, prev_ecmp_profile_index, prev_ecmp_hierarchy));
    }

    /*
     * Update the ECMP interface with a prefix
     */
    BCM_L3_ITF_SET(ecmp->ecmp_intf, BCM_L3_ITF_TYPE_FEC, BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf));
exit:
    DNX_ERR_RECOVERY_END_MODULE_ID(unit, bcmModuleL3EgressEcmp);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Removes an ECMP entry.
 * \param [in] unit -
 *   The unit number.
 * \param [in] ecmp -
 *   The ECMP group information.
 *   The following are relevant field of the bcm_l3_egress_ecmp_t structure:
 *  - ecmp_intf        -> The ECMP object.
 *  - dynamic_mode     -> The ECMP hashing mode, Indicate if the
 *    hashing mode is consistent of non-consistent using multiply and divide.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
int
bcm_dnx_l3_egress_ecmp_destroy(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp)
{
    uint32 ecmp_index;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START_MODULE_ID(unit, bcmModuleL3EgressEcmp);

    /*
     * If mode is VIP, branch off to VIP ECMP processing 
     */
    SHR_NULL_CHECK(ecmp, _SHR_E_PARAM, "bcm_l3_egress_ecmp_t");
    if (ecmp->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_VIP)
    {
        SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_vip_destroy(unit, ecmp));
        SHR_EXIT();
    }
    else
    {
        L3_EGRESS_ECMP_INVOKE_VERIFY_IF_ENABLE(unit, dnx_l3_ecmp_group_verify(unit, ecmp));
    }

    ecmp_index = BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf);

    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_group_destroy(unit, ecmp_index));

exit:
    DNX_ERR_RECOVERY_END_MODULE_ID(unit, bcmModuleL3EgressEcmp);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Retrieves the member list for an ECMP entry.
 * \param [in] unit -
 *   The unit number.
 * \param [in,out] ecmp -
 *   The ECMP group information.
 *   As input:
 *      - flags            -> BCM_L3_* flags (supported flags are BCM_L3_REPLACE and BCM_L3_WITH_ID).
 *      - ecmp_intf        -> The ECMP object.
 *      - max_paths        -> The maximal size of the ECMP group size.
 *                        if max_paths = 0 then it is ignore, otherwise -  ECMP max group size => max_paths >= intf_count.
 *                        In general members can be added to the group till it reaches MAX(max_paths,intf_count) size but no more than that.
 *      - ecmp_group_flags -> BCM_L3_ECMP_* flags.
 *   As output:
 *      - dynamic_mode     -> The ECMP hashing mode, Indicate if the hashing mode is consistent of non-consistent using multiply and divide.
 * \param [in] intf_size - number of allocated entries in
 *        intf_array
 * \param [out] intf_array - Array of the ECMP members either
 *        FEC-IDs or ECMP-IDs.
 * \param [out] intf_count - The number of members returned in
 *          the intf_array.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
int
bcm_dnx_l3_egress_ecmp_get(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp,
    int intf_size,
    bcm_if_t * intf_array,
    int *intf_count)
{
    uint32 ecmp_index;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /* If mode is VIP, branch off to VIP ECMP processing */
    SHR_NULL_CHECK(ecmp, _SHR_E_PARAM, "bcm_l3_egress_ecmp_t");
    if (ecmp->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_VIP)
    {
        SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_vip_get(unit, ecmp, intf_size, intf_array, intf_count));
    }
    else
    {
        L3_EGRESS_ECMP_INVOKE_VERIFY_IF_ENABLE(unit,
                dnx_l3_egress_ecmp_get_verify(unit, ecmp, intf_size, intf_array,
                    intf_count));

        ecmp_index = BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf);

        SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_group_data_get(unit, ecmp_index, ecmp, intf_size, intf_array, intf_count));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Adds another member to an ECMP group.
 * \param [in] unit -
 *   The unit number.
 * \param [in] ecmp -
 *   The ECMP group information.
 * \param [in] intf - member to be added
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
int
bcm_dnx_l3_egress_ecmp_add(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp,
    bcm_if_t intf)
{
    int ecmp_profile_index;
    int intf_count = 0;
    bcm_l3_egress_ecmp_t current_ecmp;
    dbal_enum_value_field_hierarchy_level_e ecmp_hierarchy;
    bcm_if_t members[DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE];
    dnx_l3_ecmp_attributes_t ecmp_atr;
    dnx_l3_ecmp_profile_t profile_data;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START_MODULE_ID(unit, bcmModuleL3EgressEcmp);

    SHR_IF_ERR_EXIT(dnx_l3_ecmp_attributes_get(unit, ecmp, &ecmp_hierarchy, &ecmp_atr, &profile_data));

    sal_memcpy(&current_ecmp, ecmp, sizeof(bcm_l3_egress_ecmp_t));

    SHR_IF_ERR_EXIT(bcm_dnx_l3_egress_ecmp_get
                    (unit, &current_ecmp, DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE, members, &intf_count));

    L3_EGRESS_ECMP_INVOKE_VERIFY_IF_ENABLE(unit,
                                           dnx_l3_egress_ecmp_add_verify(unit, ecmp, &ecmp_atr, &profile_data,
                                                                         ecmp_hierarchy, intf_count, members, intf));

    /*
     * Determine the place for the new member in the group - front of queue or back.
     */
    if (current_ecmp.dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT)
    {

        /*
         * Add the new interface into the array and update the NOF interfaces in the ECMP
         */
        members[intf_count++] = intf;
        /*
         * In case the table becomes a full member group it can use a general profile with a general member table that all the full tables
         * with the same NOF members are using, otherwise use a unique profile that was allocated for this ECMP group.
         */
        if (current_ecmp.max_paths == intf_count)
        {

            /*
             * Get the general profile group of all the consistent ECMP group with the size of current_ecmp.max_paths and without any removed
             * member
             */
            SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_profile_allocation
                            (unit, &current_ecmp, members, intf_count, ecmp_atr.fec_base, FALSE, &ecmp_profile_index));

            /*
             * Update the ECMP group with the general ECMP profile index used for all the ECMP group with the same parameters.
             */
            SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_entry_set
                            (unit, BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf), ecmp_profile_index, ecmp_atr.fec_base,
                             ecmp_atr.is_rpf_mode_em));

            /*
             * release the old profile which was dedicated to this ECMP group without all the members in it after a new one was allocated
             * to preserve the traffic.
             */
            SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_profile_free(unit, ecmp_atr.profile_index, ecmp_hierarchy));
        }
        else
        {
            uint32 consistent_hashing_manager_handle;

            SHR_IF_ERR_EXIT(ecmp_db_info.
                            consistent_hashing_manager_handle.get(unit, &consistent_hashing_manager_handle));
            /*
             * The ECMP group isn't full yet (even after adding this member), to add the requested member use the CHM add function
             * that uses the current members table to add the new member with minimal affect on the existing ECMP paths.
             */
            SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_member_add(unit,
                                                                                   consistent_hashing_manager_handle,
                                                                                   L3_ECMP_CHM_KEY_GET
                                                                                   (unit, ecmp_atr.profile_index,
                                                                                    ecmp_hierarchy),
                                                                                   ecmp->tunnel_priority.index,
                                                                                   intf - ecmp_atr.fec_base,
                                                                                   intf_count - 1, NULL));
        }

    }
    else
    {

        /*
         * If the ID of the new member that we are adding is:
         *          * the first current member ID - 1 then it goes to the front.
         *          * equal to the first member + the size of the group, then it goes to the back.
         */
        if (ecmp_atr.fec_base == intf + (1 + profile_data.protection_flag))
        {
            /** new member goes in the front */
            ecmp_atr.fec_base = intf;
        }

        intf_count++;
        current_ecmp.max_paths++;

        /*
         * Create the new profile with the new member
         */
        SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_profile_allocation
                        (unit, &current_ecmp, members, intf_count, ecmp_atr.fec_base, FALSE, &ecmp_profile_index));
        /*
         * Update the ECMP group with the new ecmp profile index and the fec base.
         */
        SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_entry_set
                        (unit, BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf), ecmp_profile_index, ecmp_atr.fec_base,
                         ecmp_atr.is_rpf_mode_em));
        /*
         * release the old profile after a new one was allocated to preserve the traffic.
         */
        SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_profile_free(unit, ecmp_atr.profile_index, ecmp_hierarchy));
    }

exit:
    DNX_ERR_RECOVERY_END_MODULE_ID(unit, bcmModuleL3EgressEcmp);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Remove a member from an ECMP group.
 * \param [in] unit -
 *   The unit number.
 * \param [in] ecmp -
 *   The ECMP group information.
 * \param [in] intf - member to be removed
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
int
bcm_dnx_l3_egress_ecmp_delete(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp,
    bcm_if_t intf)
{
    int ecmp_profile_index;
    int intf_count;
    bcm_l3_egress_ecmp_t current_ecmp;
    dbal_enum_value_field_hierarchy_level_e ecmp_hierarchy;
    bcm_if_t members[DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE];
    dnx_l3_ecmp_attributes_t ecmp_atr;
    dnx_l3_ecmp_profile_t profile_data;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START_MODULE_ID(unit, bcmModuleL3EgressEcmp);

    SHR_IF_ERR_EXIT(dnx_l3_ecmp_attributes_get(unit, ecmp, &ecmp_hierarchy, &ecmp_atr, &profile_data));

    sal_memcpy(&current_ecmp, ecmp, sizeof(bcm_l3_egress_ecmp_t));

    SHR_IF_ERR_EXIT(bcm_dnx_l3_egress_ecmp_get
                    (unit, &current_ecmp, DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE, members, &intf_count));

    L3_EGRESS_ECMP_INVOKE_VERIFY_IF_ENABLE(unit,
                                           dnx_l3_egress_ecmp_delete_verify(unit, ecmp, &ecmp_atr, &profile_data,
                                                                            ecmp_hierarchy, intf_count, members, intf));

    /*
     * Determine the place from where the member is to be deleted - front of queue or back.
     */
    if (current_ecmp.dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT)
    {
        uint32 consistent_hashing_manager_handle;

        SHR_IF_ERR_EXIT(ecmp_db_info.consistent_hashing_manager_handle.get(unit, &consistent_hashing_manager_handle));
        /*
         * In case this is the first member that is removed a dedicated member table should be allocated which mean a dedicated ECMP profile
         * should be allocated.
         */
        if (current_ecmp.max_paths == intf_count)
        {

            /*
             * Create a new ECMP profile with the same attributes that the ECMP group holds before any member was removed from but is dedicated
             * only for this ECMP group.
             */
            SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_profile_allocation
                            (unit, &current_ecmp, members, intf_count, ecmp_atr.fec_base, TRUE, &ecmp_profile_index));

            /*
             * Update the ECMP group with the new profile index the is dedicated for it.
             */
            SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_entry_set
                            (unit, BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf), ecmp_profile_index, ecmp_atr.fec_base,
                             ecmp_atr.is_rpf_mode_em));

            /*
             * Using the CHM use the new profile to remove a member from the group an assign a new members table to this profile which is
             * unique only for this ECMP group.
             */
            SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_member_remove(unit,
                                                                                      consistent_hashing_manager_handle,
                                                                                      L3_ECMP_CHM_KEY_GET
                                                                                      (unit, ecmp_profile_index,
                                                                                       ecmp_hierarchy),
                                                                                      ecmp->tunnel_priority.index,
                                                                                      intf - ecmp_atr.fec_base,
                                                                                      intf_count, NULL));

            /*
             * release the old profile after a new one was allocated to preserve the traffic.
             */
            SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_profile_free(unit, ecmp_atr.profile_index, ecmp_hierarchy));
        }
        else
        {

            /*
             * The ECMP group wasn't full (even before removing this member), to remove the requested member use the CHM remove function
             * that uses the current members table to remove the requested member without any affect on the existing ECMP paths.
             */
            SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_member_remove(unit,
                                                                                      consistent_hashing_manager_handle,
                                                                                      L3_ECMP_CHM_KEY_GET
                                                                                      (unit, ecmp_atr.profile_index,
                                                                                       ecmp_hierarchy),
                                                                                      ecmp->tunnel_priority.index,
                                                                                      intf - ecmp_atr.fec_base,
                                                                                      intf_count, NULL));
        }

    }
    else
    {
        /*
         * If the ID of the member that we are removing is: * equal to the first current member ID, then it will be
         * removed from the front.
         * if equal to the first member + the size of the group, then it will be removed from the back.
         */
        if (ecmp_atr.fec_base == intf)
        {
            /** removing member from the front */
            ecmp_atr.fec_base = intf + (1 + profile_data.protection_flag);
        }

        intf_count--;
        current_ecmp.max_paths--;

        /*
         * Create the new ecmp profile without the intf member.
         */
        SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_profile_allocation
                        (unit, &current_ecmp, members, intf_count, ecmp_atr.fec_base, FALSE, &ecmp_profile_index));
        /*
         * Update the ECMP group with the new profile index and fec base.
         */
        SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_entry_set
                        (unit, BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf), ecmp_profile_index, ecmp_atr.fec_base,
                         ecmp_atr.is_rpf_mode_em));
        /*
         * release the old profile after a new one was allocated to preserve the traffic.
         */
        SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_profile_free(unit, ecmp_atr.profile_index, ecmp_hierarchy));
    }

exit:
    DNX_ERR_RECOVERY_END_MODULE_ID(unit, bcmModuleL3EgressEcmp);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Update one of the tunnel priorities tables .
 * \param [in] unit - The unit number.
 * \param [in] ecmp -  The ECMP group information.
 * \param [in] intf_count - NOF members to update in the table.
 * \param [in] intf_array - the members to hold into the table.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
int
bcm_dnx_l3_egress_ecmp_tunnel_priority_set(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp,
    int intf_count,
    bcm_if_t * intf_array)
{
    uint32 hierarchy;
    uint32 member_iter;
    uint32 consistent_hashing_manager_handle;
    uint32 members_offset_from_base[L3_ECMP_CONSISTENT_TABLE_MAX_NOF_ENTRIES];
    bcm_if_t base_fec;
    dnx_l3_ecmp_attributes_t ecmp_atr;
    dnx_l3_ecmp_profile_t profile_data;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START_MODULE_ID(unit, bcmModuleL3EgressEcmp);

    SHR_IF_ERR_EXIT(dnx_l3_ecmp_attributes_get(unit, ecmp, &hierarchy, &ecmp_atr, &profile_data));

    L3_EGRESS_ECMP_INVOKE_VERIFY_IF_ENABLE(unit,
                                           dnx_l3_egress_ecmp_tunnel_priority_set_verify(unit, &ecmp_atr, &profile_data,
                                                                                         ecmp, intf_count, intf_array));

    base_fec = BCM_L3_ITF_VAL_GET(ecmp_atr.fec_base);
    /*
     * Create an array of offset from the base FEC to be used as an input for the CHM.
     */
    for (member_iter = 0; member_iter < intf_count; member_iter++)
    {
        members_offset_from_base[member_iter] =
            (BCM_L3_ITF_VAL_GET(intf_array[member_iter]) - base_fec) >> profile_data.protection_flag;
    }

    SHR_IF_ERR_EXIT(ecmp_db_info.consistent_hashing_manager_handle.get(unit, &consistent_hashing_manager_handle));

    SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_calender_set(unit,
                                                                     consistent_hashing_manager_handle,
                                                                     L3_ECMP_CHM_KEY_GET(unit, ecmp_atr.profile_index,
                                                                                         hierarchy),
                                                                     ecmp->tunnel_priority.index, intf_count,
                                                                     members_offset_from_base, NULL));

exit:
    DNX_ERR_RECOVERY_END_MODULE_ID(unit, bcmModuleL3EgressEcmp);
    SHR_FUNC_EXIT;

}

/**
 * \brief
 * This function verifies that the consistent hashing manager (chm) is legal.
 */
shr_error_e
chm_input_verify(
    int unit,
    uint32 chm_profile_offset,
    dbal_enum_value_field_ecmp_consistent_table_e consistent_table_type)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((consistent_table_type < DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_SMALL_SIZE)
        || (consistent_table_type > DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_LARGE_SIZE))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Type %d isn't a valid consistent hashing table type.", consistent_table_type);
    }

    if (chm_profile_offset >= L3_ECMP_TOTAL_NOF_CONSISTENT_RESOURCES(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "chm profile offset %d exceeds max NOF resources (%d).", chm_profile_offset,
                     L3_ECMP_TOTAL_NOF_CONSISTENT_RESOURCES(unit));
    }

    /*
     * Each table is placed in an offset which is a multiplication of it NOF resources.
     */
    if ((chm_profile_offset % L3_ECMP_CONSISTENT_NOF_RESOURCES_TAKEN(unit, consistent_table_type)) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Profile offset %d is invalid for table of type %d that takes %d resources.",
                     chm_profile_offset, consistent_table_type,
                     L3_ECMP_CONSISTENT_NOF_RESOURCES_TAKEN(unit, consistent_table_type));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Given an ECMP profile (profile_key) and an address in the ECMP members table (calculated from the chm_resource_offset)
 * get the ECMP members base field in the profile.
 * Note that this function assumes that the ECMP mode field in the profile is already written in template manager.
 */
shr_error_e
get_members_address_in_ecmp_profile(
    int unit,
    uint32 profile_key,
    uint32 chm_resource_offset,
    uint32 *table_address)
{
    dbal_enum_value_field_ecmp_consistent_table_e consistent_table_type = 0;
    uint32 ecmp_profile_indx;
    int ref_count;
    dbal_enum_value_field_hierarchy_level_e ecmp_hierarchy;
    dnx_l3_ecmp_profile_t ecmp_profile;
    SHR_FUNC_INIT_VARS(unit);

    ecmp_profile_indx = L3_ECMP_PROFILE_ID_FROM_CHM_KEY_GET(unit, profile_key);
    ecmp_hierarchy = L3_ECMP_HIERARCHY_FROM_CHM_KEY_GET(unit, profile_key);

    SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.ecmp_profile_hier_table.profile_data_get
                    (unit, ecmp_hierarchy, ecmp_profile_indx, &ref_count, &ecmp_profile));

    SHR_IF_ERR_EXIT(dnx_l3_ecmp_consistent_table_type_from_ecmp_mode_get
                    (unit, ecmp_profile.ecmp_mode, &consistent_table_type));

    SHR_IF_ERR_EXIT(chm_input_verify(unit, chm_resource_offset, consistent_table_type));
    /*
     * The chm NOF of resources divided by the table type NOF resources will provide us the relative
     * place of this table in the HW.
     * relative place means that if the HW members banks had only this type of table, it will be it serial number.
     */
    *table_address = chm_resource_offset / L3_ECMP_CONSISTENT_NOF_RESOURCES_TAKEN(unit, consistent_table_type);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Given an ECMP profile (profile_key) and an address in the ECMP members table (calculated from the chm_resource_offset)
 * updates the ECMP members base field in the profile.
 * Note that this function assumes that the ECMP mode field in the profile was already written.
 */
shr_error_e
update_members_address_in_ecmp_profile(
    int unit,
    uint32 profile_key,
    uint32 chm_resource_offset)
{
    dbal_enum_value_field_ecmp_consistent_table_e consistent_table_type = 0;
    uint32 entry_handle_id;
    uint32 ecmp_profile_indx;
    int ref_count;
    dbal_enum_value_field_hierarchy_level_e ecmp_hierarchy;
    dnx_l3_ecmp_profile_t ecmp_profile;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    ecmp_profile_indx = L3_ECMP_PROFILE_ID_FROM_CHM_KEY_GET(unit, profile_key);

    ecmp_hierarchy = L3_ECMP_HIERARCHY_FROM_CHM_KEY_GET(unit, profile_key);

    /*
     * The members table offset in the table determine by the actual table offset in the table and the consistent table size.
     * The addresses is alginate to the table size so the address in the profile will be "actual offset / table size"
     * An access to to the profile which was already field with the table type is made to get the table size and then the
     * relative offset to the table is updated.
     */
    SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.ecmp_profile_hier_table.profile_data_get
                    (unit, ecmp_hierarchy, ecmp_profile_indx, &ref_count, &ecmp_profile));
    SHR_IF_ERR_EXIT(dnx_l3_ecmp_consistent_table_type_from_ecmp_mode_get
                    (unit, ecmp_profile.ecmp_mode, &consistent_table_type));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_GROUP_PROFILE, ecmp_profile_indx);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HIERARCHY_LEVEL, ecmp_hierarchy);

    SHR_IF_ERR_EXIT(chm_input_verify(unit, chm_resource_offset, consistent_table_type));
    /*
     * The chm NOF of resources divided by the table type NOF resources will provide us the relative
     * place of this table in the HW.
     * relative place means that if the HW members banks had only this type of table, it will be it serial number.
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_MEMBERS_BASE, INST_SINGLE,
                                 chm_resource_offset / L3_ECMP_CONSISTENT_NOF_RESOURCES_TAKEN(unit,
                                                                                              consistent_table_type));

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * For more  details see .h file
 */
shr_error_e
ecmp_chm_get_nof_resources_from_table_size(
    int unit,
    int profile_type,
    uint32 *nof_resources)
{
    SHR_FUNC_INIT_VARS(unit);

    L3_ECMP_CONSISTENT_TYPE_VERIFY((dbal_enum_value_field_ecmp_consistent_table_e) profile_type);

    *nof_resources =
        L3_ECMP_CONSISTENT_NOF_RESOURCES_TAKEN(unit, (dbal_enum_value_field_ecmp_consistent_table_e) profile_type);

exit:
    SHR_FUNC_EXIT;
}

/*
 * For more  details see .h file
 */
shr_error_e
ecmp_chm_get_table_size_from_nof_resources(
    int unit,
    uint32 nof_resources,
    int *profile_type)
{
    dbal_enum_value_field_ecmp_consistent_table_e tbl_type_iter;
    SHR_FUNC_INIT_VARS(unit);

    for (tbl_type_iter = DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_SMALL_SIZE;
         tbl_type_iter < DBAL_NOF_ENUM_ECMP_CONSISTENT_TABLE_VALUES; tbl_type_iter++)
    {
        if (nof_resources == L3_ECMP_CONSISTENT_NOF_RESOURCES_TAKEN(unit, tbl_type_iter))
        {
            *profile_type = tbl_type_iter;
            break;
        }
    }

    if (tbl_type_iter == DBAL_NOF_ENUM_ECMP_CONSISTENT_TABLE_VALUES)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "There is no consistent profile type that holds %d resources.", nof_resources);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * For more  details see .h file
 */
shr_error_e
ecmp_chm_members_table_set(
    int unit,
    uint32 chm_offset,
    consistent_hashing_calendar_t * calendar)
{

    uint32 entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;
    uint32 row_indx, member_indx;
    uint32 calendar_ptr = 0;
    uint32 member_tbl_address;
    uint32 nof_memb_in_row;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(chm_input_verify(unit, chm_offset, calendar->profile_type));

    member_tbl_address = L3_ECMP_GET_MEMBERS_TBL_ADDR_FROM_CHM_OFFSET(unit, chm_offset);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FEC_ECMP_MEMBERS, &entry_handle_id));

    if (dnx_data_l3.ecmp.consistent_tables_info_get(unit, calendar->profile_type)->entry_size_in_bits ==
        L3_ECMP_4_BITS_MEMBER)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_FEC_ECMP_MEMBERS_4B_MEMBER_SIZE);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_FEC_ECMP_MEMBERS_8B_MEMBER_SIZE);
    }

    nof_memb_in_row = L3_ECMP_CONSISTENT_NOF_MEMBERS_IN_MEM_ROW(unit, calendar->profile_type);
    /*
     * Using a nested loop each member in each row of the table is updated
     */
    for (row_indx = 0; row_indx < L3_ECMP_CONSISTENT_NOF_ROWS_IN_MEMBER_TABLE(unit, calendar->profile_type); row_indx++)
    {

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_MEMBER_ROW_ID, member_tbl_address + row_indx);

        for (member_indx = 0; member_indx < nof_memb_in_row; member_indx++, calendar_ptr++)
        {
            uint8 member = (calendar->lb_key_member_array[calendar_ptr] & UTILEX_U8_MAX);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_MEMBER_VAL, member_indx,
                                         calendar->lb_key_member_array[calendar_ptr]);
            SHR_IF_ERR_EXIT(ecmp_db_info.
                            ecmp_fec_members_table.set(unit, chm_offset, row_indx * nof_memb_in_row + member_indx,
                                                       member));
        }

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * For more  details see .h file
 */
shr_error_e
ecmp_chm_members_table_get(
    int unit,
    uint32 chm_offset,
    consistent_hashing_calendar_t * calendar)
{
    uint32 row_indx, member_indx;
    uint32 member_index = 0;
    uint32 nof_memb_in_row;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(chm_input_verify(unit, chm_offset, calendar->profile_type));

    nof_memb_in_row = L3_ECMP_CONSISTENT_NOF_MEMBERS_IN_MEM_ROW(unit, calendar->profile_type);
    sal_memset(calendar->lb_key_member_array, 0,
               sizeof(calendar->lb_key_member_array[0]) *
               DNX_DATA_MAX_CONSISTENT_HASHING_CALENDAR_MAX_NOF_ENTRIES_IN_CALENDAR);
    /*
     * Using a nested loop each member in each row of the table is updated into the consistent hashing manager calendar.
     */
    for (row_indx = 0; row_indx < L3_ECMP_CONSISTENT_NOF_ROWS_IN_MEMBER_TABLE(unit, calendar->profile_type); row_indx++)
    {
        for (member_indx = 0; member_indx < nof_memb_in_row; member_indx++, member_index++)
        {
            uint8 member;
            SHR_IF_ERR_EXIT(ecmp_db_info.
                            ecmp_fec_members_table.get(unit, chm_offset, row_indx * nof_memb_in_row + member_indx,
                                                       &member));
            calendar->lb_key_member_array[member_index] = member;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * For more  details see .h file
 */
shr_error_e
ecmp_chm_update_members_table_offset(
    int unit,
    uint32 old_offset,
    uint32 new_offset,
    int profile_type,
    uint32 nof_consecutive_profiles)
{
    sw_state_ll_node_t node;
    sw_state_ll_node_t *node_ptr;
    uint32 profile_key;
    consistent_hashing_calendar_t calendar;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(chm_input_verify(unit, old_offset, profile_type));
    SHR_IF_ERR_EXIT(chm_input_verify(unit, new_offset, profile_type));

    /*
     * Get the members from the old members table and place it in the new one.
     */
    calendar.profile_type = profile_type;
    SHR_IF_ERR_EXIT(ecmp_chm_members_table_get(unit, old_offset, &calendar));
    SHR_IF_ERR_EXIT(ecmp_chm_members_table_set(unit, new_offset, &calendar));

    /*
     * Read the profile that need to be updated
     */

    node_ptr = &node;

    SHR_IF_ERR_EXIT(ecmp_db_info.members_tbl_addr_profile_link_lists.get_first(unit, old_offset, node_ptr));

    while (DNX_SW_STATE_LL_IS_NODE_VALID(node))
    {
        /*
         * Read the profile that need to be updated
         */
        SHR_IF_ERR_EXIT(ecmp_db_info.members_tbl_addr_profile_link_lists.node_value(unit, *node_ptr, &profile_key));
        /*
         * Update the profile in the HW with the new address
         */
        SHR_IF_ERR_EXIT(update_members_address_in_ecmp_profile(unit, profile_key, new_offset));
        /*
         * Add the new address resource the update profile
         */
        SHR_IF_ERR_EXIT(ecmp_db_info.members_tbl_addr_profile_link_lists.add_last(unit, new_offset, &profile_key));
        /*
         * Remove the profile from the old resource
         */
        SHR_IF_ERR_EXIT(ecmp_db_info.members_tbl_addr_profile_link_lists.remove_node(unit, old_offset, *node_ptr));
        /*
         * Take the next profile from the old resource list if exists for update with the new address
         */
        SHR_IF_ERR_EXIT(ecmp_db_info.members_tbl_addr_profile_link_lists.get_first(unit, old_offset, node_ptr));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * For more  details see .h file
 */
shr_error_e
ecmp_chm_update_ecmp_profile_with_members_table_offset(
    int unit,
    uint32 chm_handle,
    uint32 unique_identifyer,
    uint32 chm_offset,
    void *user_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /**
     * In case of allocating a new profile, the user_info is used to return the member table address without updating HW.
     * If the user_info is NULL, then the HW table will be updated. This happens in case of fragmentation.
     */
    if (user_info != NULL)
    {
        SHR_IF_ERR_EXIT(get_members_address_in_ecmp_profile(unit, unique_identifyer, chm_offset, (uint32 *) user_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(update_members_address_in_ecmp_profile(unit, unique_identifyer, chm_offset));
    }
    /*
     * Add this profile to the link list
     */
    SHR_IF_ERR_EXIT(ecmp_db_info.members_tbl_addr_profile_link_lists.add_last(unit, chm_offset, &unique_identifyer));

exit:
    SHR_FUNC_EXIT;
}

/*
 * For more  details see .h file
 */
shr_error_e
ecmp_chm_members_table_offset_get(
    int unit,
    uint32 unique_identifyer,
    uint32 *chm_offset)
{
    dbal_enum_value_field_ecmp_consistent_table_e consistent_table_type;
    dbal_enum_value_field_ecmp_mode_e ecmp_mode = 0;
    uint32 entry_handle_id;
    uint32 member_base_address = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_GROUP_PROFILE,
                               L3_ECMP_PROFILE_ID_FROM_CHM_KEY_GET(unit, unique_identifyer));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HIERARCHY_LEVEL,
                               L3_ECMP_HIERARCHY_FROM_CHM_KEY_GET(unit, unique_identifyer));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ECMP_MODE, INST_SINGLE, &ecmp_mode);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ECMP_MEMBERS_BASE, INST_SINGLE, &member_base_address);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dnx_l3_ecmp_consistent_table_type_from_ecmp_mode_get(unit, ecmp_mode, &consistent_table_type));
    /*
     * The profile in the HW is relative to the used table so the get the real address the HW address is multiply by the table size.
     */
    *chm_offset = member_base_address * L3_ECMP_CONSISTENT_NOF_RESOURCES_TAKEN(unit, consistent_table_type);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * For more  details see .h file
 */
shr_error_e
ecmp_chm_entries_in_profile_get_cb(
    int unit,
    int profile_type,
    uint32 max_nof_members_in_profile,
    uint32 *nof_calendar_entries)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_l3_ecmp_member_table_used_rows_get
                    (unit, profile_type, max_nof_members_in_profile, nof_calendar_entries));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Goes over all ECMP groups that have been created and runs
 *  a callback function for each one.
 * \param [in] unit - The unit number.
 * \param [in] trav_fn - a pointer to the callback function
 * \param [in] user_data - data sent from the user that will be
 *             passed to the callback function as 5th input parameter
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
int
bcm_dnx_l3_egress_ecmp_traverse(
    int unit,
    bcm_l3_egress_ecmp_traverse_cb trav_fn,
    void *user_data)
{
    bcm_l3_egress_ecmp_t ecmp;
    int is_end;
    int intf_count = 0;
    uint32 group_count = 0;
    uint32 entry_handle_id;
    uint32 field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
    bcm_if_t intf_array[DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE];

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verify that a callback function has been provided */
    if (trav_fn == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No callback function has been provided to the ECMP traverse API\n");
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_TABLE, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
    /*
     * Receive first entry in table.
     * If is_end is set to 1 after calling this API, then the table is empty.
     */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    /*
     * is_end variable will be set to 1 when all entries including the last one are traversed
     * and *_iterator_get_next API is called again.
     */
    while (!is_end)
    {
        bcm_l3_egress_ecmp_t_init(&ecmp);
        /** Receive ECMP group ID which is key for the ECMP_TABLE - its value can be up to 40959 */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_ECMP_ID, field_value));
        ecmp.ecmp_intf = field_value[0];

        SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_group_data_get
                        (unit, ecmp.ecmp_intf, &ecmp, DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE, intf_array, &intf_count));
        BCM_L3_ITF_SET(ecmp.ecmp_intf, BCM_L3_ITF_TYPE_FEC, ecmp.ecmp_intf);

        /** Invoke callback function */
        SHR_IF_ERR_EXIT((*trav_fn) (unit, &ecmp, intf_count, intf_array, user_data));

        /*
         * Receive next entry in table.
         */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        group_count++;
    }

    /*
     * Traverse the VIP ECMP groups 
     */
    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_vip_traverse(unit, trav_fn, user_data));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The basic verification that is used by all the bcm_dnx_l3_ecmp_tunnel_priority_map_* APIs for their common fields.
 * map_profile_alloc_expected - indicate if the TP map API require the map profile to be already allocated (get/delete calls).
 */

static shr_error_e
dnx_l3_ecmp_tunnel_priority_map_verify(
    int unit,
    uint8 map_profile_alloc_expected,
    bcm_l3_ecmp_tunnel_priority_map_info_t * map_info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_l3.ecmp.tunnel_priority_support_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Tunnel priority isn't supported in this device.");
    }

    if (_SHR_IS_FLAG_SET(map_info->l3_flags, ~L3_ECMP_TP_SUPPORTED_BCM_L3_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "One or more of the used flags 0x%x aren't supported (supported flags are 0x%x).",
                     map_info->l3_flags, L3_ECMP_TP_SUPPORTED_BCM_L3_FLAGS);
    }

    /*
     * In case the map profile is expected to be allocated (get/delete calls) or supposed to be created with an ID
     * it must be in range
     */
    if ((map_profile_alloc_expected || _SHR_IS_FLAG_SET(map_info->l3_flags, BCM_L3_WITH_ID))
        && ((map_info->map_profile >= dnx_data_l3.ecmp.nof_tunnel_priority_map_profiles_get(unit))
            || (map_info->map_profile < 0)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The TP map profile value is %d but must be between 0-%d.",
                     map_info->map_profile, dnx_data_l3.ecmp.nof_tunnel_priority_map_profiles_get(unit) - 1);
    }

    if (map_profile_alloc_expected || _SHR_IS_FLAG_SET(map_info->l3_flags, BCM_L3_WITH_ID))
    {
        int hierarchy_index;
        uint8 is_allocated;

        hierarchy_index = L3_ECMP_GET_HIERARCHY(map_info->l3_flags);

        SHR_IF_ERR_EXIT(algo_l3_db.tp_modes.l3_ecmp_tunnel_priority_modes_res_mngr.is_allocated(unit, hierarchy_index,
                                                                                                map_info->map_profile,
        /** It is expected that the profile will be allocated at this point **/
                                                                                                &is_allocated));
        if ((!is_allocated) && map_profile_alloc_expected)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "The TP map profile %d wasn't allocated but was expected to.",
                         map_info->map_profile);
        }
        /** The profile is yet to be allocated and doesn't expected to be at this check **/
        if (is_allocated && _SHR_IS_FLAG_SET(map_info->l3_flags, BCM_L3_WITH_ID) && !map_profile_alloc_expected)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "The TP map profile %d is already allocated but was expected not to.",
                         map_info->map_profile);
        }
    }

exit:
    SHR_FUNC_EXIT;

}
/**
 * \brief
 * The bcm_dnx_l3_ecmp_tunnel_priority_map_get verification function.
 */
static shr_error_e
dnx_l3_ecmp_tunnel_priority_map_get_verify(
    int unit,
    bcm_l3_ecmp_tunnel_priority_map_info_t * map_info)
{

    int tunnel_priority_max_value;
    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(map_info->l3_flags, ~(BCM_L3_2ND_HIERARCHY | BCM_L3_3RD_HIERARCHY)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "One or more of the used flags 0x%x aren't supported (supported flags are 0x%x).",
                     map_info->l3_flags, BCM_L3_2ND_HIERARCHY | BCM_L3_3RD_HIERARCHY);
    }

    SHR_IF_ERR_EXIT(dnx_l3_ecmp_tunnel_priority_map_verify(unit, TRUE, map_info));

    tunnel_priority_max_value = (1 << dnx_data_l3.ecmp.tunnel_priority_field_width_get(unit)) - 1;

    if ((map_info->tunnel_priority < 0) || (map_info->tunnel_priority > tunnel_priority_max_value))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The tunnel priority value is %d but must be between 0-%d.",
                     map_info->tunnel_priority, tunnel_priority_max_value);
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 * The bcm_dnx_l3_ecmp_tunnel_priority_map_set verification function.
 */
static shr_error_e
dnx_l3_ecmp_tunnel_priority_map_set_verify(
    int unit,
    bcm_l3_ecmp_tunnel_priority_map_info_t * map_info)
{

    int tunnel_priority_index_max_value;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_l3_ecmp_tunnel_priority_map_get_verify(unit, map_info));

    tunnel_priority_index_max_value = (1 << dnx_data_l3.ecmp.tunnel_priority_index_field_width_get(unit)) - 1;

    if ((map_info->index < 0) || (map_info->index > tunnel_priority_index_max_value))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The tunnel priority index value is %d but must be between 0-%d.",
                     map_info->tunnel_priority, tunnel_priority_index_max_value);
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *  Allocate a TP map profile
 * \param [in] unit -
 *   The unit number.
 * \param [in] map_info -
 *   The tunnel priority mapping information.
 *   The following are relevant field of the bcm_l3_ecmp_tunnel_priority_map_info_t structure:
 *  - map_profile      -> The map profile value to set an index for.
 *  - l3_flags         -> BCM_L3_XXX flags that will be used for setting the hierarchy and for the "with id" option.
 *                        BCM_L3_WITH_ID, BCM_L3_2ND_HIERARCHY, BCM_L3_3RD_HIERARCHY
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
int
bcm_dnx_l3_ecmp_tunnel_priority_map_create(
    int unit,
    bcm_l3_ecmp_tunnel_priority_map_info_t * map_info)
{
    int alloc_flags;
    int hierarchy_index;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    L3_EGRESS_ECMP_INVOKE_VERIFY_IF_ENABLE(unit, dnx_l3_ecmp_tunnel_priority_map_verify(unit, FALSE, map_info));

    alloc_flags = (_SHR_IS_FLAG_SET(map_info->l3_flags, BCM_L3_WITH_ID)) ? DNX_ALGO_RES_ALLOCATE_WITH_ID : 0;

    hierarchy_index = L3_ECMP_GET_HIERARCHY(map_info->l3_flags);

    SHR_IF_ERR_EXIT(algo_l3_db.tp_modes.
                    l3_ecmp_tunnel_priority_modes_res_mngr.allocate_single(unit, hierarchy_index, alloc_flags, NULL,
                                                                           &(map_info->map_profile)));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *  Destroy a TP map profile
 * \param [in] unit -
 *   The unit number.
 * \param [in] map_info -
 *   The tunnel priority mapping information.
 *   The following are relevant field of the bcm_l3_ecmp_tunnel_priority_map_info_t structure:
 *  - map_profile      -> The map profile value to set an index for.
 *  - l3_flags         -> BCM_L3_XXX flags that will be used for setting the hierarchy (BCM_L3_2ND_HIERARCHY, BCM_L3_3RD_HIERARCHY)
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
int
bcm_dnx_l3_ecmp_tunnel_priority_map_destroy(
    int unit,
    bcm_l3_ecmp_tunnel_priority_map_info_t * map_info)
{
    int hierarchy_index;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    L3_EGRESS_ECMP_INVOKE_VERIFY_IF_ENABLE(unit, dnx_l3_ecmp_tunnel_priority_map_verify(unit, TRUE, map_info));

    hierarchy_index = L3_ECMP_GET_HIERARCHY(map_info->l3_flags);

    SHR_IF_ERR_EXIT(algo_l3_db.tp_modes.
                    l3_ecmp_tunnel_priority_modes_res_mngr.free_single(unit, hierarchy_index, map_info->map_profile));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Gets a tunnel priority index for a tunnel priority map profile.
 * \param [in] unit -
 *   The unit number.
 * \param [in,out] map_info -
 *   The tunnel priority mapping information.
 *   The following are relevant field of the bcm_l3_ecmp_tunnel_priority_map_info_t structure:
 *  - tunnel_priority(in)  -> The tunnel priority value to set an index for.
 *  - map_profile(in)      -> The map profile value to set an index for.
 *  - l3_flags(in)         -> BCM_L3_XXX flags that will be used for setting the hierarchy (BCM_L3_2ND_HIERARCHY, BCM_L3_3RD_HIERARCHY).
 *  - index(out)           -> The priority index (this is the consistent table number in the TP that will selected) for the given TP and map profile that will be returned.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
int
bcm_dnx_l3_ecmp_tunnel_priority_map_get(
    int unit,
    bcm_l3_ecmp_tunnel_priority_map_info_t * map_info)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    L3_EGRESS_ECMP_INVOKE_VERIFY_IF_ENABLE(unit, dnx_l3_ecmp_tunnel_priority_map_get_verify(unit, map_info));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_TUNNEL_PRIORITY_MAP, &entry_handle_id));
    /*
     * Keys
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TP_VALUE, map_info->tunnel_priority);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TP_MAP_PROFILE, map_info->map_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HIERARCHY_LEVEL,
                               L3_ECMP_GET_HIERARCHY(map_info->l3_flags));
    /*
     * Values
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TP_INDEX, INST_SINGLE,
                               (uint32 *) (&(map_info->index)));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Sets a tunnel priority index for a tunnel priority map profile.
 * \param [in] unit -
 *   The unit number.
 * \param [in] map_info -
 *   The tunnel priority mapping information.
 *   The following are relevant field of the bcm_l3_ecmp_tunnel_priority_map_info_t structure:
 *  - tunnel_priority  -> The tunnel priority value to set an index for.
 *  - map_profile      -> The map profile value to set an index for.
 *  - l3_flags         -> BCM_L3_XXX flags that will be used for setting the hierarchy (BCM_L3_2ND_HIERARCHY, BCM_L3_3RD_HIERARCHY)..
 *  - index            -> The priority index (this is the consistent table number in the TP that will selected) for the given TP and map profile.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
int
bcm_dnx_l3_ecmp_tunnel_priority_map_set(
    int unit,
    bcm_l3_ecmp_tunnel_priority_map_info_t * map_info)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    L3_EGRESS_ECMP_INVOKE_VERIFY_IF_ENABLE(unit, dnx_l3_ecmp_tunnel_priority_map_set_verify(unit, map_info));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_TUNNEL_PRIORITY_MAP, &entry_handle_id));
    /*
     * Keys
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TP_VALUE, map_info->tunnel_priority);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TP_MAP_PROFILE, map_info->map_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HIERARCHY_LEVEL,
                               L3_ECMP_GET_HIERARCHY(map_info->l3_flags));
    /*
     * Values
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TP_INDEX, INST_SINGLE, map_info->index);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Adds and entry to the resilient table
 * \param [in] unit -
 *   The unit number.
 * \param [in] flags -
 *   supported flags:
 *   - BCM_L3_ECMP_RESILIENT_PCC: Add and entry to the internal PCC State table in LEM.
 *   - BCM_L3_ECMP_RESILIENT_PCC_EXTERNAL: Add and entry to the external PCC State table in KBP.
 * \param [in] entry -
 *   The entry data. The relevant fields
 *   - entry.ecmp.ecmp_intf -> VIP ECMP FEC ID
 *   - entry.hash_key       -> user flow signature (ECMP-LB-Key[2](16), NWK-LB-Key}
 *   - entry.intf           -> destination FEC ID
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
int
bcm_dnx_l3_egress_ecmp_resilient_add(
    int unit,
    uint32 flags,
    bcm_l3_egress_ecmp_resilient_entry_t * entry)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if ((_SHR_IS_FLAG_SET(flags, BCM_L3_ECMP_RESILIENT_PCC))
        || (_SHR_IS_FLAG_SET(flags, BCM_L3_ECMP_RESILIENT_PCC_EXTERNAL)))
    {
        SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_pcc_resilient_add(unit, flags, entry));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Missing flag BCM_L3_ECMP_RESILIENT_PCC or BCM_L3_ECMP_RESILIENT_PCC_EXTERNAL");
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Deletes an entry from the resilient table
 * \param [in] unit -
 *   The unit number.
 * \param [in] flags -
 *   supported flags:
 *   - BCM_L3_ECMP_RESILIENT_PCC: Add and entry to the internal PCC State table in LEM.
 *   - BCM_L3_ECMP_RESILIENT_PCC_EXTERNAL: Add and entry to the external PCC State table in KBP.
 * \param [in] entry -
 *   The entry data. The relevant fields
 *   - entry.ecmp.ecmp_intf -> VIP ECMP FEC ID
 *   - entry.hash_key       -> user flow signature (ECMP-LB-Key[2](16), NWK-LB-Key}
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
int
bcm_dnx_l3_egress_ecmp_resilient_delete(
    int unit,
    uint32 flags,
    bcm_l3_egress_ecmp_resilient_entry_t * entry)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if ((_SHR_IS_FLAG_SET(flags, BCM_L3_ECMP_RESILIENT_PCC))
        || (_SHR_IS_FLAG_SET(flags, BCM_L3_ECMP_RESILIENT_PCC_EXTERNAL)))
    {
        SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_pcc_resilient_delete(unit, flags, entry));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Missing flag BCM_L3_ECMP_RESILIENT_PCC or BCM_L3_ECMP_RESILIENT_PCC_EXTERNAL");
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Replaces an entry in the resilient table
 * \param [in] unit -
 *   The unit number.
 * \param [in] flags -
 *   supported flags:
 *   - BCM_L3_ECMP_RESILIENT_PCC: Add and entry to the internal PCC State table in LEM.
 *   - BCM_L3_ECMP_RESILIENT_PCC_EXTERNAL: Add and entry to the external PCC State table in KBP.
 *   - BCM_L3_ECMP_RESILIENT_MATCH_HASH_KEY 
 *   - BCM_L3_ECMP_RESILIENT_MATCH_INTF 
 *   - BCM_L3_ECMP_RESILIENT_MATCH_ECMP 
 *   - BCM_L3_ECMP_RESILIENT_REPLACE 
 *   - BCM_L3_ECMP_RESILIENT_DELETE 
 * \param [in] match_entry -
 *   The match entry data. The relevant fields acccording to flags
 *   - entry.ecmp.ecmp_intf -> VIP ECMP FEC ID
 *   - entry.hash_key       -> user flow signature (ECMP-LB-Key[2](16), NWK-LB-Key}
 *   - entry.intf           -> destination FEC ID
 * \param [out] num_entries - the number of entries updated or deleted
 * \param [in] replace_entry
 *    The replace entry data
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
int
bcm_dnx_l3_egress_ecmp_resilient_replace(
    int unit,
    uint32 flags,
    bcm_l3_egress_ecmp_resilient_entry_t * match_entry,
    int *num_entries,
    bcm_l3_egress_ecmp_resilient_entry_t * replace_entry)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if ((_SHR_IS_FLAG_SET(flags, BCM_L3_ECMP_RESILIENT_PCC))
        || (_SHR_IS_FLAG_SET(flags, BCM_L3_ECMP_RESILIENT_PCC_EXTERNAL)))
    {
        SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_pcc_resilient_replace(unit, flags, match_entry, num_entries, replace_entry));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Missing flag BCM_L3_ECMP_RESILIENT_PCC or BCM_L3_ECMP_RESILIENT_PCC_EXTERNAL");
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Traverses the resilient table, calling the specified callback function for each matching entry.
 * \param [in] unit -
 *   The unit number.
 * \param [in] flags -
 *   supported flags:
 *   - BCM_L3_ECMP_RESILIENT_PCC: Add and entry to the internal PCC State table in LEM.
 *   - BCM_L3_ECMP_RESILIENT_PCC_EXTERNAL: Add and entry to the external PCC State table in KBP.
 *   - BCM_L3_ECMP_RESILIENT_MATCH_HASH_KEY   
 *   - BCM_L3_ECMP_RESILIENT_MATCH_INTF 
 *   - BCM_L3_ECMP_RESILIENT_MATCH_ECMP
 * \param [in] match_entry -
 *   The entry data. The relevant fields
 *   - entry.ecmp.ecmp_intf -> VIP ECMP FEC ID
 *   - entry.hash_key       -> user flow signature (ECMP-LB-Key[2](16), NWK-LB-Key}
 *   - entry.intf           -> destination FEC ID
 * \param [in] trav_fn - callback function
 * \param [in] user_data - Opaque user data to be passed back to the callback
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
int
bcm_dnx_l3_egress_ecmp_resilient_traverse(
    int unit,
    uint32 flags,
    bcm_l3_egress_ecmp_resilient_entry_t * match_entry,
    bcm_l3_egress_ecmp_resilient_traverse_cb trav_fn,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verify that a callback function has been provided */
    if (trav_fn == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No callback function has been provided to the ECMP Resilient traverse API\n");
    }

    if ((_SHR_IS_FLAG_SET(flags, BCM_L3_ECMP_RESILIENT_PCC))
        || (_SHR_IS_FLAG_SET(flags, BCM_L3_ECMP_RESILIENT_PCC_EXTERNAL)))
    {
        SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_pcc_resilient_traverse(unit, flags, match_entry, trav_fn, user_data));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Missing flag BCM_L3_ECMP_RESILIENT_PCC or BCM_L3_ECMP_RESILIENT_PCC_EXTERNAL");
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * }
 */
