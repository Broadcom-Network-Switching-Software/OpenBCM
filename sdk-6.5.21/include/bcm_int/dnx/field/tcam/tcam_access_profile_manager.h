/**
* \file        tcam_access_profile_manager.h
*
* DESCRIPTION :
*       This file represents the TCAM access profile manager.
*       All access profile operations are done through the API
*       this file exposes, such as allocating a new access profile
*       or freeing an already-allocated profile.
*       All operations are made relevant to "database" parameter,
*       that's why it's the main parameter that all functions take
*       as argument.
*       Each access profile is identified by a unique id. 
*
* Sample API usage:
* \code{.c}
* \endcode
*
*
* PUBLIC FUNCTIONS (dnx_field_tcam omitted):
*       shr_error_e    access_profile_alloc( unit, handler_id, db, *acces_profile_id )
*       shr_error_e    access_profile_bank_add( unit, handler_id, bank_id )
*       shr_error_e    access_profile_bank_remove( unit, handler_id, bank_id )
*       shr_error_e    access_profile_free( unit, handler_id )
*       shr_error_e    access_profile_banks_get( unit, handler_id, *bank_ids, *count )
*       shr_error_e    access_profile_id_get( unit, handler_id, *profile_id )
*/
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef TCAM_ACCESS_PROFILE_MANAGER_H_INCLUDED
/** { */
#define TCAM_ACCESS_PROFILE_MANAGER_H_INCLUDED

#include <include/bcm_int/dnx/field/tcam/tcam_prefix_manager.h>

#define FIELD_TCAM_ACCESS_PROFILE_ID_AUTO     -1
#define FIELD_TCAM_ACCESS_PROFILE_ID_INVALID  0x1023

/**
 * \brief
 *  Allocates an access profile for the given database.
 *
 * \param [in] unit              - Device ID
 * \param [in] handler_id        - Handler ID to allocate access profile for
 * \param [in] db                - Database to allocate access profile for
 * \param [in] access_profile_id - Access profile id to allocate (FIELD_TCAM_ACCESS_PROFILE_ID_AUTO
 *                                 for auto access profile ID allocation)
 * \return
 *  \retval _SHR_E_NONE - success
 * \remark
 * SPECIAL INPUT:
 *        SWSTATE:
 *            access_profiles - List of all access profiles in the system
 * SPECIAL OUTPUT:
 *        SWSTATE:
 *            access_profiles - List of all access profiles in the system
 *        HW:
 *            TCAM_ACCESS_PROFILE - Allocates access profile in the HW
 */
shr_error_e dnx_field_tcam_access_profile_alloc(
    int unit,
    uint32 handler_id,
    dnx_field_tcam_database_t * db,
    int access_profile_id);

/**
 * \brief
 *  Adds the given bank_id to the given database's access profile.
 *
 * \param [in] unit       - Device ID
 * \param [in] handler_id - TCAM handler ID to add the given bank_id to its access profile
 * \param [in] bank_id    - The bank ID to add to the given database's access profile
 *
 * \return
 *  \retval _SHR_E_NONE - success
 * \remark
 * SPECIAL OUTPUT:
 *        SWSTATE:
 *            access_profiles - List of all access profiles in the system, which in turn
 *            includes all allocated banks information
 *        HW:
 *            TCAM_ACCESS_PROFILE - Adds bank to the given db's access profile
 */
shr_error_e dnx_field_tcam_access_profile_bank_add(
    int unit,
    uint32 handler_id,
    int bank_id);

/**
 * \brief
 *  Removes the given bank_id from the given database's access profile.
 *
 * \param [in] unit       - Device ID
 * \param [in] handler_id - Handler ID to remove the given bank_id from its access profile
 * \param [in] bank_id    - The bank ID to remove from the given database's access profile
 *
 * \return
 *  \retval _SHR_E_NONE - success
 * \remark
 * SPECIAL OUTPUT:
 *        SWSTATE:
 *            access_profiles - List of all access profiles in the system, which in turn
 *            includes all allocated banks information
 *        HW:
 *            TCAM_ACCESS_PROFILE - Remove bank from given db's access profile
 */
shr_error_e dnx_field_tcam_access_profile_bank_remove(
    int unit,
    uint32 handler_id,
    int bank_id);

/**
 * \brief
 *  Frees the access profile for the given handler_id.
 *
 * \param [in] unit       - Device ID
 * \param [in] handler_id - Handler ID to free its access profile
 * \return
 *  \retval _SHR_E_NONE - success
 * \remark
 * SPECIAL INPUT:
 *        SWSTATE:
 *            access_profiles - List of all access profiles in the system
 * SPECIAL OUTPUT:
 *        SWSTATE:
 *            access_profiles - List of all access profiles in the system
 */
shr_error_e dnx_field_tcam_access_profile_free(
    int unit,
    uint32 handler_id);

/**
 * \brief
 *  Returns a bitmap of the ids allocated for the given handler
 *
 *  \param [in] unit       - Device ID
 *  \param [in] handler_id - The handler ID to return the banks bitmap for
 *  \param [out] banks_bmp - The bitmap of the allocated banks for the given handler_id
 *  \return
 *   \retval _SHR_E_NONE - Success
 */
shr_error_e dnx_field_tcam_access_profile_banks_bmp_get(
    int unit,
    uint32 handler_id,
    uint32 *banks_bmp);

/**
 * \brief
 *  Return the banks associated with the given database's access profile
 *
 * \param [in] unit - Device ID
 * \param [in] handler_id - Handler ID that its banks will be returned
 * \param [out] bank_ids - The bank IDs of the database's access profile
 * \param [out] bank_count    - The number of the returned banks
 * 
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_access_profile_banks_sorted_get(
    int unit,
    uint32 handler_id,
    int *bank_ids,
    int *bank_count);

/**
 * \brief
 *  Returns the access profile ID for the given handler ID
 *
 * \param [in] unit               - Device ID
 * \param [in] handler_id         - The handler ID to return the access profile ID for
 * \param [out] access_profile_id - The access profile ID of the given database
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_access_profile_id_get(
    int unit,
    uint32 handler_id,
    int *access_profile_id);

/**
 * \brief
 *  Checks whether the two given handler IDs intersect with each other or not.
 *  Two handlers are declared to be intersect if they share the same TCAM bank.
 *
 *  \param [in] unit        - Device ID
 *  \param [in] handler_id1 - First handler ID
 *  \param [in] handler_id2 - Second handler ID
 *  \param [out] intersect  - TRUE if the two given handler IDs intersect, FALSE otherwise
 *
 *  \return
 *   \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_access_profile_intersect(
    int unit,
    uint32 handler_id1,
    uint32 handler_id2,
    uint8 *intersect);

/**
 * \brief
 *  Writes the given prefix_value for the given handler_id to HW
 *
 * \param [in] unit         - Device ID
 * \param [in] handler_id   - The handler ID to update prefix for
 * \param [in] prefix_value - Prefix to write for the given handler_id
 */
shr_error_e dnx_field_tcam_access_profile_prefix_write(
    int unit,
    uint32 handler_id,
    uint32 prefix_value);

/** } */
#endif
