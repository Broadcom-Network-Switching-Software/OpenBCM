/** \file bcm_int/dnx/l3/l3_ecmp.h Internal DNX L3 APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef L3_ECMP_H_INCLUDED
/*
 * {
 */
#define L3_ECMP_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/*
 * Include files.
 * {
 */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <bcm_int/dnx/algo/consistent_hashing/consistent_hashing_manager.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/l3.h>
/*
 * }
 */

/*
 * DEFINES
 * {
 */

/*
 * The max NOF entries that a consistent table can hold
 */
#define L3_ECMP_CONSISTENT_TABLE_MAX_NOF_ENTRIES        512
/*
 * Currently each member entry size can be either 4 bits (small table) or 8 bits (medium/large table).
 */
#define L3_ECMP_4_BITS_MEMBER                           4
#define L3_ECMP_8_BITS_MEMBER                           8
#define L3_ECMP_HIERARCHY_FIELD_SIZE                    utilex_log2_round_up(DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_ECMP_HIERARCHIES)
#define L3_ECMP_HIERARCHY_FIELD_MASK                    UTILEX_BITS_MASK((L3_ECMP_HIERARCHY_FIELD_SIZE - 1), 0)
#define L3_ECMP_IS_USER_PROFILE_SIZE                    1
#define L3_ECMP_USER_PROFILE_DATA_SIZE                  2
#define L3_ECMP_USER_PROFILE_DATA_MASK                  SAL_UPTO_BIT(L3_ECMP_USER_PROFILE_DATA_SIZE)
/*
 * The L3 supported flags of the ECMP.
 */
#define L3_ECMP_SUPPORTED_BCM_L3_FLAGS                 (BCM_L3_WITH_ID | BCM_L3_REPLACE | BCM_L3_2ND_HIERARCHY | BCM_L3_3RD_HIERARCHY)
/*
 * The BCM_L3_ECMP_* supported flags
 */
#define L3_ECMP_SUPPORTED_BCM_L3_ECMP_FLAGS            (BCM_L3_ECMP_LARGE_TABLE | BCM_L3_ECMP_EXTENDED | BCM_L3_ECMP_MEDIUM_TABLE | BCM_L3_ECMP_USER_PROFILE)
#define L3_ECMP_UDEP_SUPPORTED_BCM_L3_ECMP_FLAGS       (L3_ECMP_SUPPORTED_BCM_L3_ECMP_FLAGS | BCM_L3_ECMP_RESILIENT_REPLACE)
/*
 * In case an ECMP group doesn't contains all the members the ECMP ID will identify the unique consistent member table
 * that was allocated for it. In case the group uses all the members (group size = max_paths) the group will use this
 * identifier that will indicate that this ECMP group uses a general consistent table for ECMP groups with " group size"
 * members.
 */
#define L3_ECMP_FULL_MEMBERS_IN_GROUP                   0xFFFFFFFF
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
 * MACROs
 * {
 */
/* Convert from the BCM ECMP RPF enum into the DBAL ECMP RPF enum */
#define L3_ECMP_RPF_MODE_BCM_ENUM_TO_DBAL_ENUM(rpf_mode) ((rpf_mode == bcmL3EcmpUrpfLoose) ? DBAL_ENUM_FVAL_ECMP_RPF_MODE_LOOSE : \
        ((rpf_mode == bcmL3EcmpUrpfStrictEm) ? DBAL_ENUM_FVAL_ECMP_RPF_MODE_STRICT_EM : DBAL_ENUM_FVAL_ECMP_RPF_MODE_RESERVED) )

/* Convert from the DBAL ECMP RPF enum into the BCM ECMP RPF enum */
#define L3_ECMP_RPF_MODE_DBAL_ENUM_TO_BCM_ENUM(rpf_mode) ((rpf_mode == DBAL_ENUM_FVAL_ECMP_RPF_MODE_LOOSE) ? bcmL3EcmpUrpfLoose : \
        ((rpf_mode == DBAL_ENUM_FVAL_ECMP_RPF_MODE_STRICT_EM) ? bcmL3EcmpUrpfStrictEm : bcmL3EcmpUrpfInterfaceDefault) )
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
 * Return the hierarchy flags with which the ECMP group was created based on its hierarchy
 */
#define L3_EGRESS_ECMP_GET_HIER_FLAGS(hierarchy) (hierarchy == DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1 ? 0 : (hierarchy == DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_2 ? BCM_L3_2ND_HIERARCHY : BCM_L3_3RD_HIERARCHY))
/*
 * The profile ID is used as a key to the consistent hashing manager.
 * As the profile ID isn't unique (there is one per hierarchy) the hierarchy information is part of the key.
 * Below are defines that will provide the information how to pack the hierarchy and the profile ID in a uint32 variable.
 */
#define L3_ECMP_PROFILE_ID_FIELD_MASK(unit)            UTILEX_BITS_MASK(dnx_data_l3.ecmp.profile_id_size_get(unit) - 1, 0)
#define L3_ECMP_HIERARCHY_FIELD_OFFSET(unit)          (dnx_data_l3.ecmp.profile_id_size_get(unit))
#define L3_ECMP_IS_USER_PROFILE_OFFSET(unit)          (dnx_data_l3.ecmp.profile_id_offset_get(unit) + L3_ECMP_HIERARCHY_FIELD_OFFSET(unit) + L3_ECMP_HIERARCHY_FIELD_SIZE)
#define L3_ECMP_USER_PROFILE_STATE_OFFSET(unit)       (L3_ECMP_IS_USER_PROFILE_OFFSET(unit) + L3_ECMP_IS_USER_PROFILE_SIZE)
#define L3_ECMP_USER_PROFILE_DATA_OFFSET(unit)        (L3_ECMP_IS_USER_PROFILE_OFFSET(unit))
/*
 * The total number of consistent resources available.
 * which is the total memory size divided by the smallest resource size
 */
#define L3_ECMP_TOTAL_NOF_CONSISTENT_RESOURCES(unit) ((dnx_data_l3.ecmp.consistent_mem_row_size_in_bits_get(unit)*(1 << dnx_data_l3.ecmp.member_table_nof_rows_size_in_bits_get(unit))) / L3_ECMP_CONSISTENT_TABLE_SIZE_IN_BITS(unit,DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_SMALL_SIZE))

/*
 * Return the NOF banks in the device that are used for consistent hashing.
 */
#define L3_ECMP_NOF_CONSISTENT_BANKS(unit) ((dnx_data_l3.ecmp.consistent_mem_row_size_in_bits_get(unit)*(1 << dnx_data_l3.ecmp.member_table_nof_rows_size_in_bits_get(unit))) / dnx_data_l3.ecmp.consistent_bank_size_in_bits_get(unit))

/*
 * The NOF members that a single row in the consistent memory contains depending on the table type
 */
#define L3_ECMP_CONSISTENT_NOF_MEMBERS_IN_MEM_ROW(unit,consistent_table_type) (dnx_data_l3.ecmp.consistent_mem_row_size_in_bits_get(unit)/dnx_data_l3.ecmp.consistent_tables_info_get(unit,consistent_table_type)->entry_size_in_bits)
/*
 * The number of rows that a consistent table takes in the member table memory.
 * The table address is calculated using this size
 */
#define L3_ECMP_CONSISTENT_NOF_ROWS_IN_MEMBER_TABLE(unit,consistent_table_type) (dnx_data_l3.ecmp.consistent_tables_info_get(unit,consistent_table_type)->nof_entries / L3_ECMP_CONSISTENT_NOF_MEMBERS_IN_MEM_ROW(unit,consistent_table_type))

/*
 * The size in bits that a consistent table takes from the member table memory
 */
#define L3_ECMP_CONSISTENT_TABLE_SIZE_IN_BITS(unit,consistent_table_type) (dnx_data_l3.ecmp.consistent_tables_info_get(unit,consistent_table_type)->entry_size_in_bits*dnx_data_l3.ecmp.consistent_tables_info_get(unit,consistent_table_type)->nof_entries)

/*
 * The smallest table size is the base resource and each table should be some multiplication of that table which is the NOF
 * resources that that table takes.
 */
#define L3_ECMP_CONSISTENT_NOF_RESOURCES_TAKEN(unit,consistent_table_type) (L3_ECMP_CONSISTENT_TABLE_SIZE_IN_BITS(unit,consistent_table_type) / L3_ECMP_CONSISTENT_TABLE_SIZE_IN_BITS(unit,DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_SMALL_SIZE))

/**
 * Get the User-defined profile data that will participate in the generation of the CHM unique key.
 * The data consists of is_user_profile indication and CHM allocation state.
 * If consecutive allocations of the same CHM table occur, the CHM allocation state will change from 0 to 1 to ensure uniqueness of the CHM key.
 */
#define DNX_L3_ECMP_UDEP_ALLOC_STATE_APPLY(unit, chm_alloc_state) ((chm_alloc_state << L3_ECMP_IS_USER_PROFILE_SIZE) | 1)

/*
 * the consistent hashing manager unique key is composed from the profile id, the hierarchy and UDEP identification.
 * This MACRO creates it
 */
#define L3_ECMP_CHM_KEY_GET(unit, profile_id, hierarchy, user_profile_data) (((profile_id & L3_ECMP_PROFILE_ID_FIELD_MASK(unit)) << dnx_data_l3.ecmp.profile_id_offset_get(unit)) | ((hierarchy & L3_ECMP_HIERARCHY_FIELD_MASK) << L3_ECMP_HIERARCHY_FIELD_OFFSET(unit)) | (((user_profile_data & L3_ECMP_USER_PROFILE_DATA_MASK) << L3_ECMP_USER_PROFILE_DATA_OFFSET(unit))))
/*
 * TP mode has 4 values and each of them require a different NOF consistent tables
 * MODE    NOF tables
 *   0          1
 *   1          2
 *   2          4
 *   3          8
 */
#define L3_ECMP_NOF_CONSISTENT_TABLES_REQUIRED_BY_TP_MODE(tp_mode) (1 << tp_mode)
/** Indicate whether the ECMP index is in the extended range of IDs. */
#define DNX_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, ecmp_index) (ecmp_index > dnx_data_l3.ecmp.max_ecmp_basic_mode_get(unit))
/**
 * Resource cb defines for algo l3 ECMP
 */
#define DNX_L3_ECMP_RES_GROUP_CB ecmp_db_info.ecmp_res_manager
#define DNX_L3_ECMP_RES_EXTENDED_GROUP_CB ecmp_db_info.ecmp_extended_res_manager
/*
 * Return the needed resource name based on the range to which the ECMP ID belongs - basic or extended
 */
#define DNX_L3_ECMP_GROUP_RESOURCE(unit, ecmp_index) (DNX_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, ecmp_index) ? DNX_L3_RES_ECMP_EXTENDED_GROUP : DNX_L3_RES_ECMP_GROUP)
/*
 * This macro calls execute the exec given from the allocation manager that is compatible with the ECMP index
 */
#define DNX_L3_ECMP_GROUP_RESOURCE_CB(unit, ecmp_index, exec)\
    (DNX_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, ecmp_index) ?\
        DNX_L3_ECMP_RES_EXTENDED_GROUP_CB.exec :\
        DNX_L3_ECMP_RES_GROUP_CB.exec)

/*
 * }
 */
/*
 * Globals
 * {
 */

/*
 * }
 */
/*
 * Structures
 * {
 */

/*
 * This structure holds all the ECMP profile fields which match the HW profile.
 * This profile is used as a key for the template manager for profile allocations.
 */
typedef struct
{
    /*
     * Statistic object id
     */
    uint32 statistic_object_id;
    /*
     * Statistic object profile
     */
    uint8 statistic_object_profile;
    /*
     * The hashing mode (see dbal_enum_value_field_ecmp_mode_e enumeration for more details)
     */
    dbal_enum_value_field_ecmp_mode_e ecmp_mode;
    /*
     * Identification of the consistent members table allocation.
     * In case the actual NOF members equal the group size this value isn't important as a general table for the "group size" is used.
     * In case NOF members is smaller than the group size, a special members table is used for this group and it's indentified
     * by this value which will be the ECMP ID for simplification.
     */
    uint32 consistent_hashing_id;
    /*
     * The ECMP group size (the maximal size of the group)
     */
    uint32 group_size;
    /*
     * Is this ECMP group point to a group of protected FECs
     */
    uint8 protection_flag;

    /*
     * Tunnel Priority mode.
     */
    bcm_l3_ecmp_tunnel_priority_mode_t tunnel_priority_mode;
    /*
     * Tunnel Priority Map profile.
     */
    uint8 tunnel_priority_map_profile;

    /*
     * The ECMP RPF mode
     */
    dbal_enum_value_field_ecmp_rpf_mode_e rpf_mode;
    /** User-defined ECMP profile */
    uint16 user_defined_ecmp_profile;
} dnx_l3_ecmp_profile_t;

/*
 * This structure holds all the user-defined ECMP profile fields which match the HW profile.
 * The profile ID is allocated in a resource manager, the data is saved in SW state.
 */
typedef struct
{
    /*
     * The hashing mode (see dbal_enum_value_field_ecmp_mode_e enumeration for more details)
     * For the purposes of UDEP, it should be a consistent hashing mode.
     */
    dbal_enum_value_field_ecmp_mode_e ecmp_mode;
    /*
     * Identification of the consistent members table allocation.
     * In case the actual NOF members equal the group size this value isn't important as a general table for the "group size" is used.
     * In case NOF members is smaller than the group size, a special members table is used for this group and it's indentified
     * by this value which will be the ECMP ID for simplification.
     */
    uint32 consistent_members_table_addr;

    /** Number of unique offsets */
    uint32 group_size;
    /** Tunnel Priority mode. */
    bcm_l3_ecmp_tunnel_priority_mode_t tunnel_priority_mode;
    /** Tunnel Priority Map profile. */
    uint8 tunnel_priority_map_profile;
    /** The ECMP RPF mode */
    dbal_enum_value_field_ecmp_rpf_mode_e rpf_mode;
    /** The ECMP hierarchy */
    uint32 hierarchy;
    /** Indication for allocation state - True or False */
    uint8 chm_alloc_state;
} dnx_l3_ecmp_user_profile_t;

/*
 * Holds the ECMP attributes
 */
typedef struct
{
    /*
     * The ECMP group base FEC.
     */
    uint32 fec_base;
    /*
     * The index of the ECMP profile in the ECMP profile table
     */
    uint32 profile_index;
    /*
     * A bit that is set to 1 in case the ECMP RPF more is EM.
     */
    uint8 is_rpf_mode_em;
} dnx_l3_ecmp_attributes_t;
/*
 * }
 */
/*
 * Externs
 * {
 */

/*
 * }
 */
/*
 * Function declaration
 * {
 */

/**
 * \brief
 * Convert ECMP mode into consistent table type.
 * \param [in] unit -
 *   The unit number.
 * \param [in] ecmp_mode -
 *   The ECMP mode (see dbal_enum_value_field_ecmp_mode_e for more details).
 * \param [out] consistent_table_type -
 *   The consistent table type (see dbal_enum_value_field_ecmp_consistent_table_e for more details).
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e dnx_l3_ecmp_consistent_table_type_from_ecmp_mode_get(
    int unit,
    dbal_enum_value_field_ecmp_mode_e ecmp_mode,
    dbal_enum_value_field_ecmp_consistent_table_e * consistent_table_type);

/**
 * \brief
 * Returns the hierarchy ECMP profiles template manager handle name.
 * \param [in] unit -
 *   The unit number.
 * \param [in] hierarchy -
 *   The hierarchy of the requested ECMP profile resource handle name.
 * \param [out] profile_resouce -
 *   the returned name of the ECMP profiles resource name.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e dnx_l3_ecmp_hierarchy_profile_resource_get(
    int unit,
    dbal_enum_value_field_hierarchy_level_e hierarchy,
    char **profile_resouce);

/**
 * \brief
 * A consistent hashing manager CB.
 * Returns the amount of resource needed for a given profile type.
 * \param [in] unit -
 *   The unit number.
 * \param [in] profile_type -
 *   the consistent table type (small, medium or large).
 * \param [out] nof_resources -
 *   returns the NOF resources that the profile type consume.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e ecmp_chm_get_nof_resources_from_table_size(
    int unit,
    int profile_type,
    uint32 *nof_resources);
/**
 * \brief
 * A consistent hashing manager CB.
 * Returns a profile type that matches NOF resources.
 * \param [in] unit -
 *   The unit number.
 * \param [in] nof_resources -
 *  returns the NOF resources that the profile type consume.
 * \param [out] profile_type -
 *   returns the consistent table type (small, medium or large).
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e ecmp_chm_get_table_size_from_nof_resources(
    int unit,
    uint32 nof_resources,
    int *profile_type);
/**
 * \brief
 * A consistent hashing manager CB.
 * Gets a resource number which is used to locate the relevant HW members table and returns that members offsets that were found.
 * table in the HW.
 * \param [in] unit -
 *   The unit number.
 * \param [in] chm_offset -
 *   row in the ECMP members table.
 * \param [in ] calendar -
 *   Holds the type of the member table and an array of members to be updated from the HW.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e ecmp_chm_members_table_set(
    int unit,
    uint32 chm_offset,
    consistent_hashing_calendar_t * calendar);
/**
 * \brief
 * A consistent hashing manager CB.
 * Update the array of members offset (placed in calendar) from members offset that read from the HW.
 * \param [in] unit -
 *   The unit number.
 * \param [in] chm_offset -
 *   row in the ECMP members table.
 * \param [out] calendar -
 *   Holds the type of the member table and an array of members to be filled from the ECMP members table.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e ecmp_chm_members_table_get(
    int unit,
    uint32 chm_offset,
    consistent_hashing_calendar_t * calendar);

/**
 * \brief
 * A consistent hashing manager (chm) CB.
 * Sometimes the CHM moves tables inside the ECMP members memory banks for fragmentation reasons in the HW member table.
 * This function moves a single table from one place in the memory to a new position.
 * \param [in] unit -
 *   The unit number.
 * \param [in] old_offset -
 *   The address of the table that should be moved into a new address.
 * \param [in ] new_offset -
 *   The new address of the table that is placed in the old_offset.
 * \param [in] profile_type -
 *   the consistent table type (small, medium or large).
 * \param [in] nof_consecutive_profiles -
 *  the number of consecutive profiles.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e ecmp_chm_update_members_table_offset(
    int unit,
    uint32 old_offset,
    uint32 new_offset,
    int profile_type,
    uint32 nof_consecutive_profiles);
/**
 * \brief
 * A consistent hashing manager CB.
 * Once the consistent hashing manger allocated a resource, an address (resulted from the allocated resource)
 * should be update in the profile of the calling ECMP.
 * So basicly this function updates the "member base address" in the ECMP profile.
 * \param [in] unit -
 *   The unit number.
 * \param [in] chm_handle -
 *   consistent hashing manager handle.
 * \param [in ] unique_identifyer -
 *   this is the profile ID and hierarchy.
 * \param [in] chm_offset -
 *   address in the ECMP members table.
 * \param [in] user_info -
 *   additional user info.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e ecmp_chm_update_ecmp_profile_with_members_table_offset(
    int unit,
    uint32 chm_handle,
    uint32 unique_identifyer,
    uint32 chm_offset,
    void *user_info);

/**
 * \brief
 * A consistent hashing manager CB.
 * Given a profile ID (unique_identifyer) the resource offset (chm_offset) that calculate from the ECMP profile is returned.
 * \param [in] unit -
 *   The unit number.
 * \param [in ] unique_identifyer -
 *   this is the profile ID and hierarchy
 * \param [out] chm_offset -
 *   address in the ECMP members table to be returned.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e ecmp_chm_members_table_offset_get(
    int unit,
    uint32 unique_identifyer,
    uint32 *chm_offset);

/**
 * \brief
 * A consistent hashing manager CB.
 * Returns the actual NOF entries that a table uses (the tables doesn't always use all of their entries)
 * \param [in] unit -
 *   The unit number.
 * \param [in ] profile_type -
 *   the consistent table type (small, medium or large).
 * \param [in] max_nof_members_in_profile -
 *   This is the max size of the ECMP group (not the table).
 * \param [out] nof_calendar_entries -
 *   pointer to the returned value which is the NOF relevant members in the table.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e ecmp_chm_entries_in_profile_get_cb(
    int unit,
    int profile_type,
    uint32 max_nof_members_in_profile,
    uint32 *nof_calendar_entries);

/**
 * \brief
 * A consistent hashing manager helper function.
 * Sometimes the CHM moves tables inside the ECMP members memory banks for fragmentation reasons in the HW member table.
 * This function updates the ECMP profiles linked list with the new CHM table pointers.
 * \param [in] unit -
 *   The unit number.
 * \param [in] old_offset -
 *   The address of the table that should be moved into a new address.
 * \param [in ] new_offset -
 *   The new address of the table that is placed in the old_offset.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *   * ecmp_chm_update_members_table_offset
 */
shr_error_e ecmp_chm_update_linked_list_offset(
    int unit,
    uint32 old_offset,
    uint32 new_offset);

/**
 * \brief
 * Given an ECMP profile (profile_key) and an address in the ECMP members table (calculated from the chm_resource_offset)
 * updates the ECMP members base field in the profile.
 * Note that this function assumes that the ECMP mode field in the profile was already written.
 */
shr_error_e update_members_address_in_ecmp_profile(
    int unit,
    uint32 profile_key,
    uint32 chm_resource_offset);
/**
 * \brief
 *  Verifies the user input for the ECMP create function.
 * \param [in] unit -
 *   The unit number.
 * \param [in] is_wcmp -
 *   Indicates a WCMP group.
 * \param [in] ecmp -
 *   The ECMP group information.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e dnx_l3_egress_ecmp_create_common_verify(
    int unit,
    uint8 is_wcmp,
    bcm_l3_egress_ecmp_t * ecmp);

/**
 * \brief
 * Return all the relevant information that a single ECMP group holds including the interface array.
 * \param [in] unit -
 *   The unit number.
 * \param [in] ecmp_index -
 *   The ECMP ID.
 * \param [in] tp_table_index -
 *   the tunnel priority (TP) table index, in case TP isn't use this parameter should be set to 0.
 * \param [out] member_count -
 *   pointer that receive the NOF members in the group
 * \param [out] intf_array -
 *   Array of the group members
 * \param [out] ecmp_profile -
 *   pointer that returns the attributes of the ECMP profile
 */
shr_error_e dnx_l3_egress_ecmp_get_ecmp_group_info(
    int unit,
    bcm_if_t ecmp_index,
    int tp_table_index,
    int *member_count,
    bcm_if_t * intf_array,
    dnx_l3_ecmp_profile_t * ecmp_profile);

/**
 * \brief
 * This function find an ECMP group profile based on the ECMP ID.
 * \param [in] unit -
 *   The unit number.
 * \param [in] ecmp_intf -
 *   The ECMP group ID.
 * \param [out] ecmp_atr -
 *   The ECMP group attributes
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_l3_egress_ecmp_get
 *  * bcm_dnx_l3_egress_ecmp_destroy
 *  * bcm_dnx_l3_egress_ecmp_add
 *  * bcm_dnx_l3_egress_ecmp_delete
 */
shr_error_e dnx_l3_ecmp_group_entry_result_get(
    int unit,
    bcm_if_t ecmp_intf,
    dnx_l3_ecmp_attributes_t * ecmp_atr);

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
 * \param [in] is_wcmp -
 *  Indicates if the pofile is associated with a WCMP group.
 * \param [in,out] ecmp_profile -
 *   Pointer the ECMP profile number.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_l3_egress_ecmp_create
 */
shr_error_e dnx_l3_egress_ecmp_fill_profile(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp,
    bcm_if_t intf,
    int intf_count,
    uint8 force_unique_profile,
    uint8 is_wcmp,
    dnx_l3_ecmp_profile_t * ecmp_profile);

/**
 * \brief
 *   Get ECMP profile data from SW.
 * \param [in] unit - The unit number.
 * \param [in] ecmp_index - The ECMP ID.
 * \param [out] ecmp_profile - The ECMP profile attributes.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e dnx_l3_egress_ecmp_profile_sw_get(
    int unit,
    bcm_if_t ecmp_index,
    dnx_l3_ecmp_profile_t * ecmp_profile);

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
shr_error_e dnx_l3_egress_ecmp_entry_alloc(
    int unit,
    bcm_if_t * ecmp_index,
    uint32 ecmp_flags,
    uint32 ecmp_group_flags);

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
shr_error_e dnx_l3_egress_ecmp_entry_set(
    int unit,
    bcm_if_t ecmp_intf,
    uint32 ecmp_profile_id,
    uint32 fec_base,
    uint32 rpf_mode_is_em);

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
shr_error_e dnx_l3_egress_ecmp_entry_remove(
    int unit,
    uint32 ecmp_index);

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
 *  * bcm_dnx_l3_egress_ecmp_destroy
 */
shr_error_e dnx_l3_egress_ecmp_entry_dealloc(
    int unit,
    bcm_if_t ecmp_index);

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
shr_error_e dnx_l3_consistent_hashing_allocation(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp,
    uint32 fec_base,
    int ecmp_profile_index,
    dbal_enum_value_field_hierarchy_level_e hierarchy,
    int intf_count,
    bcm_if_t * intf_array,
    uint32 *member_table_addr);

/**
 * \brief
 *   Gets the configured consistent hashing members for the given profile.
 * \param [in] unit - The unit number.
 * \param [in] ecmp_profile - A structure that holds the data for the UDEP.
 * \param [out] intf_count - number of members in the offsets array.
 * \param [out] intf_array - An array of offsets
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e dnx_l3_egress_ecmp_consistent_hashing_table_get(
    int unit,
    dnx_l3_ecmp_user_profile_t * ecmp_profile,
    int *intf_count,
    bcm_if_t * intf_array);
/*
 * }
 */
#endif /* L3_ECMP_H_INCLUDED */
