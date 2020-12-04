/**
* \file        tcam_prefix_manager.h
*
* DESCRIPTION :
*       This file represents the TCAM prefix manager, the prefix manager
*       is responsible for allocating and freeing a prefix per a given
*       database.
*
* Sample API usage:
* \code{.c}
*       dnx_field_tcam_database_t *db;
*       fill_db(db);
*       dnx_field_tcam_prefix_alloc(db, &prefix); "allocates a new prefix for db"
* \endcode
*
*
* PUBLIC FUNCTIONS (dnx_field_tcam omitted):
*       shr_error_e    prefix_alloc( unit, db, *prefix )
*       shr_error_e    prefix_free( unit, db )
*/
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef TCAM_PREFIX_MANAGER_H_INCLUDED
/** { */
#define TCAM_PREFIX_MANAGER_H_INCLUDED

#include <include/bcm_int/dnx/field/tcam/tcam_entities.h>

/**
 * When this value is set in the prefix value field we get from user,
 * it tells the prefix manager to choose the prefix value for the user.
 */
#define DNX_FIELD_TCAM_PREFIX_VAL_AUTO 0x512

/**
 * \brief
 *  Sets a new prefix for the given handler in accordance to the free prefixes found on the
 *  given banks. If no common prefix was available on all the given banks, this function returns
 *  a _SHR_E_NOT_FOUND error.
 *
 * \param [in] unit        - Device ID
 * \param [in] handler_id  - Handler to set a new prefix for
 * \param [in] banks_count - Number of banks to search prefix on
 * \param [in] bank_ids    - The IDs of the banks to search prefix on
 *
 * \return
 *  \retval _SHR_E_NONE      - success
 *  \retval _SHR_E_NOT_FOUND - No common free prefix was found on given banks
 */
shr_error_e dnx_field_tcam_prefix_set(
    int unit,
    uint32 handler_id,
    int banks_count,
    int bank_ids[]);

/**
 * \brief
 *  Allocates the prefix of the given handler ID on the given bank ID.
 *
 * \param [in] unit              - Device ID
 * \param [in] bank_id           - Bank ID to allocate prefix on
 * \param [in] handler_id        - The handler id to allocate a prefix for
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_prefix_bank_alloc(
    int unit,
    int bank_id,
    uint32 handler_id);

/**
 * \brief prefix_bank_free( unit, handler_id, bank_id ) - frees the prefix allocated for the
 *  given tcam handler on the given bank_id.
 * \param [in] unit       - unit
 * \param [in] bank_id    - The bank_id to delete prefix for
 * \param [in] handler_id - The handler id to free its prefix
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_prefix_free(
    int unit,
    int bank_id,
    uint32 handler_id);

/**
 * \brief
 *  Checks whether the given handler_id prefix is free to be used on the given bank_id
 *
 * \param [in] unit         - Device ID
 * \param [in] bank_id      - TCAM bank ID to check if the given handler_id's prefix is free to be used on
 * \param [in] handler_id   - TCAM handler ID to check if its prefix can be used on the bank
 * \param [out] is_free     - TRUE if the given handler's prefix is free to be used on the given bank_id
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_prefix_is_free(
    int unit,
    int bank_id,
    uint32 handler_id,
    uint8 *is_free);

/** } */
#endif
