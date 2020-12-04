/**
* \file        tcam_bank_manager.h
*
* DESCRIPTION :
*       This file represents the TCAM bank manager component. It exposes
*       bank managing functionality that allows the user to safely manage
*       TCAM banks without thinking of the constraints. This manager is
*       a submodule of TCAM module, and is only used through TCAM handler
*       submodule.
*
* PUBLIC FUNCTIONS (dnx_field_tcam omitted):
*       shr_error_e    bank_alloc( unit, db, bank_prealloc )
*       shr_error_e    bank_free( unit, db )
*/
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef TCAM_BANK_MANAGER_H_INCLUDED
/** { */
#define TCAM_BANK_MANAGER_H_INCLUDED

#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_field.h>
#include <include/bcm_int/dnx/field/field.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_access_types.h>

/**
 * \brief
 * bank_alloc( unit, handler_id, bank_prealloc ) - Allocates bank/s for the given
 * handler_id, according to bank_prealloc information. The allocated banks
 * can't be allocated by another handler in the same program as the given
 * handler.
 *
 * \param [in] unit               - Device ID
 * \param [in] handler_id         - TCAM handler id to allocate bank/s for
 * \param [in] nof_banks_to_alloc - Number of banks to allocate
 * \param [in] banks_to_alloc     - Array of banks IDs to allocate
 *
 * \return
 *  \retval _SHR_E_NONE - success
 * \remark
 * SPECIAL INPUT:
 *        SWSTATE:
 *           dnx_field_tcam_bank_sw - Holds information for all banks in device
 * SPECIAL OUTPUT:
 *        SWSTATE:
 *           dnx_field_tcam_bank_sw - Holds information for all banks in device
 *        HW:
 *            TCAM_ACCESS_PROFILE - adds bank to the handler's access profile
 */
shr_error_e dnx_field_tcam_bank_alloc(
    int unit,
    uint32 handler_id,
    int nof_banks_to_alloc,
    int banks_to_alloc[]);

/**
 * \brief
 * Allocates any single bank for the given handler_id.
 *
 * \param [in] unit       - Device ID
 * \param [in] handler_id - TCAM handler id to allocate the bank for
 * \return
 *  \retval _SHR_E_NONE - success
 * \remark
 * SPECIAL INPUT:
 *        SWSTATE:
 *           dnx_field_tcam_bank_sw - Holds information for all banks in device
 * SPECIAL OUTPUT:
 *        SWSTATE:
 *           dnx_field_tcam_bank_sw - Holds information for all banks in device
 *        HW:
 *            TCAM_ACCESS_PROFILE - adds bank to the database access profile
 */
shr_error_e dnx_field_tcam_bank_single_alloc_any(
    int unit,
    uint32 handler_id);

/**
 * \brief
 *  Frees the banks of the given TCAM handler id.
 *
 * \param [in] unit       - Device ID
 * \param [in] handler_id - The TCAM handler id to free its banks.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 * \remark
 * SPECIAL INPUT:
 *        SWSTATE:
 *           dnx_field_tcam_bank_sw - Holds information for all banks in device
 * SPECIAL OUTPUT:
 *        SWSTATE:
 *           dnx_field_tcam_bank_sw - Holds information for all banks in device
 *        HW:
 *            TCAM_ACCESS_PROFILE - Remove banks from the database access profile
 */
shr_error_e dnx_field_tcam_bank_handler_free(
    int unit,
    uint32 handler_id);

/**
 * \brief
 *  Updates about an entry at the given location that was allocated.
 *  The entry belongs to the given handler id on the given core.
 *
 * \param [in] unit       - Device ID
 * \param [in] core       - Core ID
 * \param [in] location   - The location that was allocated
 * \param [in] handler_id - The id of the handler that the allocated entry
 *                          belongs to
 *
 * \return
 *  \retval _SHR_E_NONE success
 */
shr_error_e dnx_field_tcam_bank_entry_alloc_update(
    int unit,
    int core,
    uint32 location,
    uint32 handler_id);

/**
 * \brief
 *  Updates about an entry at given location that was freed.
 *  the entry belongs to the given handler id on the given core.
 *
 * \param [in] unit       - Device ID
 * \param [in] core       - Core ID
 * \param [in] location   - The location that was freed
 * \param [in] handler_id - The id of the handler that the freed entry
 *                          belongs to
 *
 * \return
 *  \retval _SHR_E_NONE success
 */
shr_error_e dnx_field_tcam_bank_entry_free_update(
    int unit,
    int core,
    uint32 location,
    uint32 handler_id);

/**
 * \brief
 *  Updates the bank manager regarding an entry that belongs to the given
 *  handler ID and that is being moved from given location_from to the given
 *  location_to on the given core.
 *
 * \param [in] unit          - Device ID
 * \param [in] core          - Core ID
 * \param [in] location_from - Location entry was moved from
 * \param [in] location_to   - Location entry was moved to
 * \param [in] handler_id    - The handler id that the entry belongs to
 *
 * \return
 *  \retval _SHR_E_NONE success
 */
shr_error_e dnx_field_tcam_bank_entry_move_update(
    int unit,
    int core,
    uint32 location_from,
    uint32 location_to,
    uint32 handler_id);

/**
 * \brief
 *  Gets the number of available entries of size equal to the given entry_size on the given TCAM bank
 *  on the given core.
 *
 * \param [in] unit                - Device ID
 * \param [in] core                - Core ID
 * \param [in] bank_id             - The id of the bank the entry belongs to
 * \param [in] entry_size          - Entry size
 * \param [out] nof_free_entries   - Number of free entries that their size is equal to entry_size
 *                                   in the given TCAM bank
 *
 * \return
 *  \retval _SHR_E_NONE success
 */
shr_error_e dnx_field_tcam_bank_available_entries_get(
    int unit,
    int core,
    int bank_id,
    dnx_field_tcam_entry_size_e entry_size,
    uint32 *nof_free_entries);

/**
 * \brief
 *  Checks whether the given bank id on the given core is full or not for adding
 *  an entry that belongs to the given handler id.
 *
 * \param [in] unit       - Device ID
 * \param [in] core       - Core ID
 * \param [in] bank_id    - The id of the bank to check its state
 * \param [in] handler_id - The id of the handler to check if its entry fits in
 *                          the bank.
 * \param [out] full      - TRUE if bank is full, FALSE otherwise
 *
 * \return
 *  \retval _SHR_E_NONE success
 */
shr_error_e dnx_field_tcam_bank_is_full(
    int unit,
    int core,
    int bank_id,
    uint32 handler_id,
    uint8 *full);

/**
 * \brief
 *  Checks whether given handler is found on allocating handlers list
 *  for the bank that its ID is equal to the given bank_id.
 *
 * \param [in] unit       - Device ID
 * \param [in] bank_id    - The bank ID to check if given handler is on its list
 * \param [in] handler_id - the ID of the handler to check whether it's on the list
 * \param [out] found     - Whether the given handler was found on the given bank's allocating handlers list or not.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_bank_is_handler_on_bank(
    int unit,
    int bank_id,
    uint32 handler_id,
    uint8 *found);

/**
 * \brief
 *  Removes the given handler_id from the given bank_id's handler_ids list.
 *  If the given handler_id is not inside the list, it returns a _SHR_E_NOT_FOUND
 *  error.
 *
 * \param [in] unit       - Device ID
 * \param [in] bank_id    - The bank ID to remove the handler from its list
 * \param [in] handler_id - the ID of the handler to remove from the list
 *
 * \return
 *  \retval _SHR_E_NONE - success
 *  \retval _SHR_E_NOT_FOUND - handler_id not found in the bank's handlers list
 */
shr_error_e dnx_field_tcam_bank_remove_handler_from_bank(
    int unit,
    int bank_id,
    uint32 handler_id);

/**
 * \brief
 *  Allocates the bank with the given bank_id for the given handler_id.
 *
 * \param [in] unit       - Device ID
 * \param [in] bank_id    - The bank ID of the given bank to allocate.
 * \param [in] handler_id - The handler ID to allocate the bank for.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 * \remark
 * SPECIAL OUTPUT:
 *        SWSTATE:
 *           dnx_field_tcam_bank_sw - Holds information for all banks in device
 */
shr_error_e dnx_field_tcam_bank_add_handler_to_bank(
    int unit,
    int bank_id,
    uint32 handler_id);

/** } */
#endif
