/**
 * \file algo_l3.h Internal DNX L3 Managment APIs
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef ALGO_L3_H_INCLUDED
/*
 * {
 */
#define ALGO_L3_H_INCLUDED

#include <soc/dnx/mdb.h>
#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>

#include <bcm_int/dnx/l3/l3_ecmp.h>

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif
/**
 * Defines
 * {
 */

/**
 * Resource name defines for algo l3.
 * \see
 * dnx_algo_l3_source_address_table_init
 */
#define DNX_ALGO_L3_INTF_RIF_RESOURCE                       "Ingress interface RIF allocation"

#define DNX_ALGO_L3_EGRESS_SOURCE_ADDRESS_TABLE_RESOURCE    "Source address map table"

#define DNX_ALGO_L3_ECMP_PROFILE_HIER_1_TABLE_RESOURCE      "ECMP first hierarchy profile table"

#define DNX_ALGO_L3_ECMP_PROFILE_HIER_2_TABLE_RESOURCE      "ECMP second hierarchy profile table"

#define DNX_ALGO_L3_ECMP_PROFILE_HIER_3_TABLE_RESOURCE      "ECMP third hierarchy profile table"

#define DNX_ALGO_L3_ECMP_USER_PROFILE_TABLE_RESOURCE        "ECMP user-defined profile table"

#define DNX_ALGO_L3_ECMP_TUNNEL_PRIORITY_RESOURCE_MANAGER   "ECMP tunnel priority resource manager."

#define DNX_ALGO_L3_ECMP_CONSISTENT_MANAGER_HANDLE          "ECMP consistent hashing manager handle"

#define DNX_ALGO_L3_VRRP_VSI_TCAM_TABLE_RESOURCE            "VRRP MyMac TCAM VSI table"

#define DNX_ALGO_L3_VRRP_EXEM_TCAM_TABLE_RESOURCE           "VRRP MyMac TCAM EXEM table"

#define DNX_ALGO_L3_VRRP_EXEM_STATION_ID_RESOURCE           "VRRP MyMac EXEM station ID"

#define DNX_ALGO_L3_LIF_LOCAL_OUTLIF                        "LIF local outlif"

#define DNX_ALGO_L3_VIP_ECMP_TABLE_RESOURCE                 "VIP ECMP VIP ID resource manager"

#define DNX_ALGO_L3_ECMP_USER_PROFILE_RESOURCE              "ECMP user-defined profile manager"

/**
 * Resource name defines for algo l3 ECMP
 */
#define DNX_ALGO_L3_RES_ECMP_GROUP "DNX_ALGO_L3_RES_ECMP_GROUP"
#define DNX_ALGO_L3_RES_ECMP_EXTENDED_GROUP "DNX_ALGO_L3_RES_ECMP_EXTENDED_GROUP"
/*
 * represent a FEC invalid logical address.
 */
#define ALGO_L3_INVALID_LOGICAL_ADDRESS -1

/*
 * Represents an invalid Out-RIF result type
 */
#define DNX_ALGO_L3_INVALID_OUT_RIF_RESULT_TYPE (-1)

/**
 * Resource name defines for algo L3 FEC in the resource manager
 */
#define DNX_ALGO_L3_RES_FEC "DNX_ALGO_L3_FEC"

/** Define flags for the FEC allocation */
/** Flag which is used to indicate with-id allocation */
#define DNX_ALGO_L3_FEC_WITH_ID         (0x00000001)

/** Translate fec_id (20bits) to super fec_id (19 bits) by taking bits [1:19] */
#define DNX_ALGO_L3_FEC_ID_TO_SUPER_FEC_ID(_fec_id_) (_fec_id_ >> 1)

/** Retrieve the physical FEC address from the FEC ID ((super_FEC * fec_size) / row_size)*/
#define DNX_ALGO_L3_FEC_PHYSICAL_ADDRESS_GET(unit,fec_id) (DNX_ALGO_L3_FEC_ID_TO_SUPER_FEC_ID(fec_id) * dnx_data_l3.fec.super_fec_size_get(unit) /  dnx_data_mdb.direct.fec_row_width_get(unit))

/* Return the bank number given a logical FEC index*/
#define DNX_ALGO_L3_BANK_ID_FROM_FEC_ID_GET(unit,fec_id) (DNX_ALGO_L3_FEC_PHYSICAL_ADDRESS_GET(unit,fec_id) / dnx_data_l3.fec.bank_nof_physical_rows_get(unit))

/** The ID of the FEC sub-resouce index which has values in the ECMP and the FEC ranges. */
#define DNX_ALGO_L3_SHARED_RANGE_FEC_BANK  dnx_data_l3.fec.first_shared_bank_get(unit)

/**
 * }
 */

/**
 * emums
 * {
 */
/*
 * List of the the FEC banks groups types that are used during the FEC bank allocation.
 */
typedef enum
{
    /*
     * Information of the cluster and banks that are to be updated for the requested resource.
     */
    ALGOL_L3_FEC_BANKS_GROUP_TO_UPDATE = 0,
    /*
     * Information of the clusters that need to be moved in order to allocate the requested resource.
     */
    ALGOL_L3_FEC_BANKS_GROUP_TO_MOVE = 1,
    /*
     * Number of bank allocation information types.
     */
    ALGOL_L3_NOF_FEC_BANKS_GROUP_TYPES = 2
} algo_l3_fec_banks_group_type_e;

/*
 * List of the the FEC banks groups types that are used during the FEC bank allocation.
 */
typedef enum
{
    /*
     * The requested bank resource
     */
    ALGOL_L3_FEC_BANK_REQUESTED_TYPE = 0,
    /*
     * The pair of the requested bank resource (a pair of resources is two consecutive number with a leading even number)
     */
    ALGOL_L3_FEC_BANK_REQUESTED_PAIR_TYPE = 1,
    /*
     * NOF of bank types
     */
    ALGOL_L3_NOF_FEC_BANK_TYPES = 2
} algo_l3_fec_banks_types_in_group_e;

/**
 * }
 */

/**
 * Structures
 * {
 */
/**
 * \brief - VRRP tcam key fields
 */
typedef struct
{
    /**
     * Destination MAC address
     */
    bcm_mac_t mac_da;

    /**
     * Destination MAC address mask
     */
    bcm_mac_t mac_da_mask;

    /**
     * Protocol group ID
     */
    uint32 protocol_group;

    /**
     * Protocol group ID mask
     */
    uint32 protocol_group_mask;
} algo_dnx_l3_vrrp_tcam_key_t;

/**
 * \brief - Structure used for the allocation of FECs.
 */
typedef struct
{
    /** Type of the super FEC - indicating with/without Protection/Statistics */
    uint32 super_fec_type_tag;
    /** Id of the sub-resource in the FEC resource (bank_id) */
    uint32 sub_resource_index;
} algo_dnx_l3_fec_data_t;

/**
 * }
 */

/*
 * FUNCTIONS
 * {
 */

/**
* \brief
*   gets the NOF of FEC allocation banks.
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*     Identifier of the device to access.
*    \param [out] fec_banks -
*      NOF of FEC allocation banks.
*  \remark
*    None
*  \see
*    shr_error_e
*****************************************************/
shr_error_e dnx_algo_l3_nof_fec_banks_get(
    int unit,
    uint32 *fec_banks);

/**
* \brief
*   Intialize l3 algorithms.
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*      Relevant unit.
*  \par INDIRECT INPUT:
*    - DBAL table sizes, used to initialize source address table template.
*  \par DIRECT OUTPUT:
*    shr_error_e -
*      Error return value
*  \par INDIRECT OUTPUT:
*      None
*  \remark
*    None
*  \see
*    shr_error_e
*****************************************************/
shr_error_e dnx_algo_l3_init(
    int unit);

/**
* \brief
*   Deintialize l3 algorithms.
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*      Relevant unit.
*  \par INDIRECT INPUT:
*    - DBAL table sizes, used to initialize source address table template.
*  \par DIRECT OUTPUT:
*    shr_error_e -
*      Error return value
*  \par INDIRECT OUTPUT:
*      None
*  \remark
*    None
*  \see
*    shr_error_e
*****************************************************/
shr_error_e dnx_algo_l3_deinit(
    int unit);

/**
* \brief
*   FEC database to hierarchy mapping.
*
*  \par DIRECT INPUT:
*    \param [in] unit - Relevant unit
*    \param [in] data_base -
*     FEC physical data_base.
*    \param [out] hierarchy -
*      FEC hierarchy.
*  \remark
*    None
*  \see
*/
void dnx_algo_l3_db_hierarchy_map(
    int unit,
    mdb_physical_table_e data_base,
    uint32 *hierarchy);

/**
* \brief
*   Print an entry of the ingress mymac prefix template. See
*       \ref dnx_algo_template_print_data_cb for more details.
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*     Identifier of the device to access.
*    \param [in] data -
*      Pointer of the struct to be printed.
*      \b As \b input - \n
*       The pointer is expected to hold a struct of type bcm_mac_t. \n
*       It's the user's responsibility to verify it beforehand.
*  \par INDIRECT INPUT:
*    \b *data \n
*     See DIRECT INPUT above
*  \par DIRECT OUTPUT:
*    None.
*  \par INDIRECT OUTPUT:
*      The default output stream.
*  \remark
*    None
*  \see
*    dnx_algo_template_print_data_cb
*    shr_error_e
*****************************************************/
void dnx_algo_l3_print_ingress_mymac_prefix_entry_cb(
    int unit,
    const void *data);

/**
 * \brief
 *  prints a VRRP multiple MyMac TCAM profile.  See \ref dnx_algo_template_print_data_cb for more details.
 * \param [in] unit -
 *   The unit number.
 * \param [in] data -
 *  pointer to an VRRP TCAM profile structure.
 * \return
 *   None
 */
void dnx_algo_l3_print_vrrp_entry_cb(
    int unit,
    const void *data);

/**
 * \brief
 *  prints an ECMP profile.  See \ref dnx_algo_template_print_data_cb for more details.
 * \param [in] unit -
 *   The unit number.
 * \param [in] data -
 *  pointer to an ECMP profile structure.
 * \return
 *   None
 */
void dnx_algo_l3_print_ecmp_profiles_cb(
    int unit,
    const void *data);

/**
 * \brief
 *   Receive the hierarchy of the ECMP group based on its ID.
 *   The data is taken from the SW state parameter where the
 *   information for the hierarchy of each 2k stage is kept.
 * \param [in] unit - The unit number.
 * \param [in] ecmp_index - The ECMP ID of the wanted element.
 * \param [out] hierarchy -
 *   The hierarchy value of the ECMP group
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_l3_egress_ecmp_add
 *  * bcm_dnx_l3_egress_ecmp_delete
 *  * bcm_dnx_l3_egress_ecmp_destroy
 */
shr_error_e dnx_algo_l3_ecmp_hierarchy_get(
    int unit,
    int ecmp_index,
    uint32 *hierarchy);

/**
 * \brief
 *   Return the capacity that can be used for the VRRP table.
 * \param [in] unit - The unit number.
 * \param [out] capacity - The database capacity.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  *
 */
shr_error_e dnx_algo_l3_vrrp_capacity_get(
    int unit,
    int *capacity);

/**
 * \brief
 *   Finds out if the given ECMP group is allocated in the resource.
 * \param [in] unit - The unit number.
 * \param [in] ecmp_index - The ECMP group id.
 * \param [out] is_allocated - Indication whether the group exists.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_l3_ecmp_group_verify
 */
shr_error_e dnx_algo_l3_ecmp_is_allocated(
    int unit,
    int ecmp_index,
    uint8 *is_allocated);

/**
 * \brief
 *   Allocates an ECMP group in the resource with a given hierarchy.
 * \param [in] unit - The unit number.
 * \param [in,out] ecmp_index - The ECMP group id.
 *          As input:
 *              * when flag BCM_L3_WITH_ID is provided to ecmp_flags.
 *          As output:
 *              * when flag BCM_L3_WITH_ID is missing.
 * \param [in] ecmp_flags - Control flags for the allocation in the resource
 * \param [in] ecmp_group_flags - Control flags for ECMP group
 * \param [in] hierarchy -
 *   Hierarchy value of the ECMP group.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_l3_egress_ecmp_entry_alloc
 */
shr_error_e dnx_algo_l3_ecmp_allocate(
    int unit,
    int *ecmp_index,
    uint32 ecmp_flags,
    uint32 ecmp_group_flags,
    uint32 hierarchy);

/**
 * \brief
 *   Deallocates an ECMP from the resource using its ID.
 * \param [in] unit - The unit number.
 * \param [in] ecmp_index - The ECMP group id.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_l3_egress_ecmp_entry_dealloc
 */
shr_error_e dnx_algo_l3_ecmp_deallocate(
    int unit,
    int ecmp_index);

/**
 * \brief
 *   Reserves a bank of memory for the use of the FEC protection
 * \param [in] unit - The unit number.
 * \param [in] bank_id - The bank ID that is going to be reserved.
 *             Valid bank IDs are 0 to 3.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e dnx_algo_l3_ecmp_extended_bank_for_failover_allocate(
    int unit,
    int bank_id);

/**
 * \brief
 *   Free a bank of memory from the use of FEC protection.
 *   It can be used by FEC protection and ECMP extended mode.
 * \param [in] unit - The unit number.
 * \param [in] bank_id - The bank ID that is going to be freed.
 *             Valid bank IDs are 0 to 3.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e dnx_algo_l3_ecmp_extended_bank_for_failover_destroy(
    int unit,
    int bank_id);

/**
 * \brief
 *   Allocates a FEC entry in the resource.
 * \param [in] unit - The unit number.
 * \param [in,out] fec_index - The FEC id.
 *          As input:
 *              * when flag BCM_L3_WITH_ID is provided to fec_flags.
 *          As output:
 *              * when flag BCM_L3_WITH_ID is missing.
 * \param [in] flags - DNX_ALGO_L3_ Control flags for the allocation in the resource
 * \param [in] fec_db - FEC physical database
 * \param [in] fec_alloc_data - Holds data required for the FEC allocation
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_l3_egress_create_fec_allocate
 */
shr_error_e dnx_algo_l3_fec_allocate(
    int unit,
    int *fec_index,
    uint32 flags,
    mdb_physical_table_e fec_db,
    algo_dnx_l3_fec_data_t * fec_alloc_data);

/**
 * \brief
 *   Deallocates a FEC ID from the resource.
 * \param [in] unit - The unit number.
 * \param [in] fec_index - The FEC id.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_l3_egress_destroy_fec_deallocate
 */
shr_error_e dnx_algo_l3_fec_deallocate(
    int unit,
    int fec_index);

/**
 * \brief
 *  Initialize all the resource managers that are needed for the FEC which are:
 *  FEC resource manager - to manage the FEC allocation
 *  SW state - to track the physical database according to direction and hierarchy
 *  SW state - to hold the information(hierarchy and direction) for the FECs
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e dnx_algo_l3_fec_init(
    int unit);

/**
 * \brief
 *   Receive the hierarchy of the FEC based on its ID.
 * \param [in] unit - The unit number.
 * \param [in] fec_index - The FEC ID of the wanted element.
 * \param [out] hierarchy_p -
 *   The hierarchy value of the FEC
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_algo_l3_fec_bitmap_allocate
 *  * dnx_l3_egress_fec_entry_add
 *  * dnx_l3_egress_fec_entry_get
 */
shr_error_e algo_l3_fec_hierarchy_stage_map_get(
    int unit,
    uint32 fec_index,
    uint32 *hierarchy_p);

/**
 * \brief
 *   Get the physical DB for a given FEC ID.
 * \param [in] unit - The unit number.
 * \param [in] fec_index - The FEC ID of the element.
 * \param [out] fec_db - The physical database of the FEC
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
shr_error_e algo_l3_fec_db_stage_map_get(
    int unit,
    uint32 fec_index,
    mdb_physical_table_e * fec_db);

/**
 * \brief
 *   Finds out if the given FEC ID is allocated in the resource.
 * \param [in] unit - The unit number.
 * \param [in] fec_index - The FEC id.
 * \param [out] is_allocated - Indication whether the ID exists.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_l3_egress_create_fec_verify
 *  * dnx_l3_egress_fec_info_get
 *  * dnx_l3_egress_fec_info_delete
 */
shr_error_e dnx_algo_l3_fec_is_allocated(
    int unit,
    int fec_index,
    uint8 *is_allocated);

/**
 * \brief
 *   Receive the type of the Super FEC - it indicates whether
 *   the FEC has protection or not and whether it has statistics or not.
 * \param [in] unit - The unit number.
 * \param [in] fec_index - The FEC id.
 * \param [out] super_fec_resource_type - Type of the super FEC.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_l3_egress_fec_entry_add
 */
shr_error_e dnx_algo_l3_fec_super_fec_type_get(
    int unit,
    int fec_index,
    dbal_enum_value_field_fec_resource_type_e * super_fec_resource_type);

/**
 * \brief
 *   return the FEC protection state
 * \param [in] unit - The unit number.
 * \param [in] fec_index - The FEC id.
 * \param [out] is_protected - the protection state of the FEC 0/1.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e dnx_algo_l3_fec_protection_state_get(
    int unit,
    int fec_index,
    uint8 *is_protected);

/**
 * \brief
 *   Find a start FEC for a given range of free IDs.
 * \param [in] unit - The unit number.
 * \param [in] nof_members - The number of members
 * \param [in] fec_db - The required FEC DB
 * \param [in] fec_resource_type - super FEC resourse type
 * \param [out] start_if_id - The start FEC ID which was found
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e dnx_algo_l3_fec_allocation_get(
    int unit,
    int nof_members,
    mdb_physical_table_e fec_db,
    uint32 fec_resource_type,
    int *start_if_id);

/**
 * \brief
 *   Receive the MDB clusters which are currently allocated
 *   for the use of a particular FEC table.
 * \param [in] unit - The unit number.
 * \param [in] mdb_physical_fec_db - The MDB physical table ID
 * \param [out] nof_clusters - number of clusters allocated for the FEC table.
 * \param [out] clusters - An array of clusters in use by the given FEC table.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * algo_l3_fec_mdb_table_resources_set
 *  * algo_l3_fec_mdb_table_resources_check
 */
shr_error_e algo_l3_fec_mdb_table_resources_get(
    int unit,
    mdb_physical_table_e mdb_physical_fec_db,
    int *nof_clusters,
    mdb_cluster_alloc_info_t * clusters);

/**
 * \brief
 *   Verify whether the ECMP profile ID is allocated by checking if there are any references to it.
 * \param [in] unit - The unit number.
 * \param [in] index - The ECMP group profile index.
 * \param [out] is_allocated - Indication whether the profile exists.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_algo_res_dbal_is_allocated
 */
shr_error_e dnx_algo_l3_res_dbal_ecmp_group_profile_is_allocated(
    int unit,
    int index,
    uint8 *is_allocated);

/**
 * \brief
 *   Allocate an ECMP profile ID in the template manager
 *   with unique data result and a particular ID
 * \param [in] unit - The unit number.
 * \param [in] profile_index - The ECMP group profile index.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_algo_res_dbal_alloc
 */
shr_error_e dnx_algo_l3_res_dbal_ecmp_group_profile_allocate(
    int unit,
    int *profile_index);

/**
 * \brief
 *   Free an ECMP profile ID from the template manager.
 * \param [in] unit - The unit number.
 * \param [in] profile_index - The ECMP group profile index.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_algo_res_dbal_free
 */
shr_error_e dnx_algo_l3_res_dbal_ecmp_group_profile_free(
    int unit,
    int profile_index);

/**
 * \brief
 *   Receive the following ECMP profile ID after the current one
 * \param [in] unit - The unit number.
 * \param [in,out] profile_index - The ECMP group profile index.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_algo_res_dbal_get_next
 */
shr_error_e dnx_algo_l3_res_dbal_ecmp_group_profile_get_next(
    int unit,
    int *profile_index);

/**
 * \brief
 *  Allocate a rif, and save its information in the sw state.
 *
 * \param [in] unit  -  The unit number.
 * \param [in] rif_id  -  The l3 interface structure.
 * \param [in] rif_result_type  -  The result type of RIF EEDB table.
 * \param [in] table_id  -  The dbal table id of the of RIF EEDB table.
 *
 * \return
 *   shr_error_e
 */
shr_error_e dnx_algo_l3_rif_allocate_generic(
    int unit,
    int rif_id,
    dbal_result_type_t rif_result_type,
    dbal_tables_e table_id);

/**
 * \brief
 *  Allocate a rif, and save its information in the sw state.
 *
 * \param [in] unit  -  The unit number.
 * \param [in] rif_id  -  The l3 interface structure.
 * \param [in] rif_result_type  -  The result type of RIF EEDB table.
 * \param [in] fixed_entry_size  -  Minimal entry size to allocate.
 *
 * \return
 *   shr_error_e
 */
shr_error_e dnx_algo_l3_rif_allocate(
    int unit,
    int rif_id,
    dbal_result_type_t rif_result_type,
    uint32 fixed_entry_size);

/**
 * \brief
 *  Free a rif, and remove its information from the sw state.
 *
 * \param [in] unit  -  The unit number.
 * \param [in] rif_id  -  The l3 interface structure.
 *
 * \return
 *   shr_error_e
 */
shr_error_e dnx_algo_l3_rif_free(
    int unit,
    int rif_id);

/* ALGO CINT exported API - START */
shr_error_e dnx_cint_algo_l3_rif_allocate_generic(
    int unit,
    int rif_id,
    char *rif_result_type_name,
    char *table_name);

shr_error_e dnx_cint_algo_l3_fec_allocate(
    int unit,
    int *fec_index,
    uint32 flags,
    char *hierarchy_name,
    char *fec_resource_type_name);

/**
 * \brief
 *  Allocates an ECMP user profile and its specific data using the sw state.
 * \param [in] unit - The unit number.
 * \param [in,out] user_profile_id - The ECMP user profile index
 * \param [in] ecmp_profile - The ECMP user profile data
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_l3_egress_ecmp_user_profile_create
 */
shr_error_e dnx_algo_l3_ecmp_user_profile_allocate(
    int unit,
    int *user_profile_id,
    dnx_l3_ecmp_user_profile_t * ecmp_profile);

/**
 * \brief
 *  Returns information whether an ECMP user profile exists
 * \param [in] unit - The unit number.
 * \param [in] user_profile_id - The ECMP user profile index
 * \param [in] is_allocated - indication whether the profile exists.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_l3_egress_ecmp_user_profile_create
 */
shr_error_e dnx_algo_l3_ecmp_user_profile_is_allocated(
    int unit,
    int user_profile_id,
    uint8 *is_allocated);

/**
 * \brief
 *  Delete an ECMP User profile and its data.
 * \param [in] unit - The unit number.
 * \param [in] user_profile_id - The ECMP user profile index
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_l3_egress_ecmp_user_profile_destroy
 */
shr_error_e dnx_algo_l3_ecmp_user_profile_destroy(
    int unit,
    int user_profile_id);

/**
 * \brief
 *  Update the number of references to an ECMP User profile.
 * \param [in] unit - The unit number.
 * \param [in] user_profile_id - The ECMP user profile index
 * \param [in] increment - Indication whether the reference count should be incremented or decremented.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_l3_egress_ecmp_fill_profile
 *  * dnx_l3_egress_ecmp_profile_free
 */
shr_error_e dnx_algo_l3_ecmp_user_profile_update_ref_count(
    int unit,
    int user_profile_id,
    uint8 increment);

/**
 * \brief
 *  Update the group size of an ECMP User profile.
 * \param [in] unit - The unit number.
 * \param [in] user_profile_id - The ECMP user profile index
 * \param [in] group_size - New group size
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_l3_egress_ecmp_user_profile_create
 */
shr_error_e dnx_algo_l3_ecmp_user_profile_update_group_size(
    int unit,
    int user_profile_id,
    uint32 group_size);
/**
 * \brief
 *  Get the data of an ECMP user profile based on its ID.
 * \param [in] unit - The unit number.
 * \param [in] user_profile_id - The ECMP user profile index
 * \param [out] ecmp_profile_data - The ECMP user profile data
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_l3_egress_ecmp_user_profile_create
 *  * bcm_dnx_l3_egress_ecmp_user_profile_destroy
 */
shr_error_e dnx_algo_l3_ecmp_user_profile_data_get(
    int unit,
    int user_profile_id,
    dnx_l3_ecmp_user_profile_t * ecmp_profile_data);
/**
 * \brief
 * Return the relative position of a FECz FEC int the FECz DB, that is (fec_id - first_fec_z_id).
 * \param [in] unit - The unit number.-
 * \param [in] fecz_id - the original fecz_id that is needed to be converted.
 * \param [out] relative_fecz_id - The converted fecz id.
 * \return
 *   \retval The updated fec_id with the right offset
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
shr_error_e dnx_algo_l3_convert_fecz_id(
    int unit,
    uint32 fecz_id,
    uint32 *relative_fecz_id);

/**
 * \brief
 * Get the max value of DBAL_FIELD_xxx_ID
 * \param [in] unit - The unit number.
 * \param [in] table_id - the value of DBAL_TABLE_xxx_ID
 * \param [in] field_id - the value of DBAL_FIELD_xxx_ID
 * \return
 *   \retval the max value of DBAL_FIELD_xxx_ID if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */

uint32 dnx_algo_l3_compress_field_max_size_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id);

/** Find an mac_compress_id that wasn't assigned already, and mark it as assigned.
 * \param [in] unit - The unit number.
 * \param [out] mac_compress_id - The mac_compress_id
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e dnx_algo_l3_mac_compress_id_allocate(
    int unit,
    uint32 *mac_compress_id);

/**
 * \brief
 * Find an ip6_msb32_compress_id that wasn't assigned already, and mark it as assigned.
 * \param [in] unit - The unit number.
 * \param [out] ip6_msb32_compress_id - The mac_compress_id that will be freed
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e dnx_algo_l3_ip6_msb32_compress_id_allocate(
    int unit,
    uint32 *ip6_msb32_compress_id);

/**
 * \brief
 * Free an mac_compress_id, and mark it as unassigned.
 * \param [in] unit - The unit number.
 * \param [in] mac_compress_id - The mac_compress_id that will be freed
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e dnx_algo_l3_mac_compress_id_free(
    int unit,
    uint32 mac_compress_id);

/**
 * \brief
 * Free an ip6_msb32_compress_id, and mark it as unassigned.
 * \param [in] unit - The unit number.
 * \param [in] ip6_msb32_compress_id - The ip6_msb32_compress_id that will be freed
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e dnx_algo_l3_ip6_msb32_compress_id_free(
    int unit,
    uint32 ip6_msb32_compress_id);
/* ALGO CINT exported API - END */

/**
 * \brief
 * update number of reference to an mac_compress_id.
 * \param [in] unit - The unit number.
 * \param [in] user_id - The mac_compress_id.
 * \param [in] increment - need increment or decrement
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e dnx_algo_l3_mac_compress_update_ref_count(
    int unit,
    uint32 user_id,
    uint8 increment);

/**
 * \brief
 * update number of reference to an mac_compress_id.
 * \param [in] unit - The unit number.
 * \param [in] user_id - The mac_compress_id.
 * \param [out] nof_ref - reference number of the mac_compress_id
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */

shr_error_e dnx_algo_l3_mac_compress_ref_get(
    int unit,
    uint32 user_id,
    uint32 *nof_ref);

/**
 * \brief
 * set number of reference to an mac_compress_id to ZERO.
 * \param [in] unit - The unit number.
 * \param [in] user_id - The mac_compress_id.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */

shr_error_e dnx_algo_l3_mac_compress_ref_clear(
    int unit,
    uint32 user_id);

/**
 * \brief
 * update number of reference to an ip6_msb32_compress_id.
 * \param [in] unit - The unit number.
 * \param [in] user_id - The ip6_msb32_compress_id.
 * \param [in] increment - need increment or decrement
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e dnx_algo_l3_ip6_msb32_compress_update_ref_count(
    int unit,
    uint32 user_id,
    uint8 increment);

/**
 * \brief
 * update number of reference to an ip6_msb32_compress_id.
 * \param [in] unit - The unit number.
 * \param [in] user_id - The ip6_msb32_compress_id.
 * \param [out] nof_ref - reference number of the ip6_msb32_compress_id
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e dnx_algo_l3_ip6_msb32_compress_ref_get(
    int unit,
    uint32 user_id,
    uint32 *nof_ref);

/**
 * \brief
 * set number of reference to an ip6_msb32_compress_id to ZERO.
 * \param [in] unit - The unit number.
 * \param [in] user_id - The ip6_msb32_compress_id.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e dnx_algo_l3_ip6_msb32_compress_ref_clear(
    int unit,
    uint32 user_id);
/* ALGO CINT exported API - END */

/**
 * }
 */
/*
 * Info related to EGR_POINTED
 * {
 */
/**
 * Resource name defines for algo egr_pointed
 */
#define DNX_ALGO_EGR_POINTED_RESOURCE "EGR_POINTED"
/*
 * }
 */
/*
 * }
 */
#endif/*_ALGO_L3_API_INCLUDED__*/
