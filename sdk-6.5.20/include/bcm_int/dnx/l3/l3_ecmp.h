/** \file bcm_int/dnx/l3/l3_ecmp.h Internal DNX L3 APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef L3_ECMP_H_INCLUDED
/*
 * {
 */
#define L3_ECMP_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
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
#define L3_ECMP_HIERARCHY_FIELD_SIZE                   utilex_log2_round_up(DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_ECMP_HIERARCHIES)
#define L3_ECMP_HIERARCHY_FIELD_MASK                   UTILEX_BITS_MASK((L3_ECMP_HIERARCHY_FIELD_SIZE - 1), 0)

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
/*
 * The profile ID is used as a key to the consistent hashing manager.
 * As the profile ID isn't unique (there is one per hierarchy) the hierarchy information is part of the key.
 * Below are defines that will provide the information how to pack the hierarchy and the profile ID in a uint32 variable.
 */
#define L3_ECMP_PROFILE_ID_FIELD_MASK(unit)            UTILEX_BITS_MASK(dnx_data_l3.ecmp.profile_id_size_get(unit) - 1, 0)
#define L3_ECMP_HIERARCHY_FIELD_OFFSET(unit)          (dnx_data_l3.ecmp.profile_id_size_get(unit))
/*
 * The total number of consistent resources available.
 * which is the total memory size divided by the smallest resource size
 */
#define L3_ECMP_TOTAL_NOF_CONSISTENT_RESOURCES(unit) ((dnx_data_l3.ecmp.consistent_mem_row_size_in_bits_get(unit)*(1 << dnx_data_l3.ecmp.member_table_nof_rows_size_in_bits_get(unit))) / L3_ECMP_CONSISTENT_TABLE_SIZE_IN_BITS(unit,DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_SMALL_SIZE))

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

/*
 * the consistent hashing manager unique key is composed from the profile id and the hierarchy.
 * This MACRO creates it
 */
#define L3_ECMP_CHM_KEY_GET(unit, profile_id, hierarchy) (((profile_id & L3_ECMP_PROFILE_ID_FIELD_MASK(unit)) << dnx_data_l3.ecmp.profile_id_offset_get(unit)) | ((hierarchy & L3_ECMP_HIERARCHY_FIELD_MASK) << L3_ECMP_HIERARCHY_FIELD_OFFSET(unit)) )
/*
 * TP mode has 4 values and each of them require a different NOF consistent tables
 * MODE    NOF tables
 *   0          1
 *   1          2
 *   2          4
 *   3          8
 */
#define L3_ECMP_NOF_CONSISTENT_TABLES_REQUIRED_BY_TP_MODE(tp_mode) (1 << tp_mode)

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
    uint32 members_table_id;
    /*
     * The ECMP group size (the maximal size of the group)
     */
    uint32 group_size;
    /*
     * Indicate if this ECMP is statful or not
     */
    uint8 stateful;
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
} dnx_l3_ecmp_profile_t;

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
/*
 * }
 */
#endif /* L3_ECMP_H_INCLUDED */
