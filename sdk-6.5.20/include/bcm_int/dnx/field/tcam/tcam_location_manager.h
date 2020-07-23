/**
* \file        tcam_location_manager.h
*
* DESCRIPTION :
*       This file represent the TCAM location manager. It manages the
*       location of each entry, and exposes functionality to allow the
*       user to find new location for a new entry, or free an occupied
*       location.
 */
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef TCAM_LOCATION_MANAGER_H_INCLUDED
/** { */
#define TCAM_LOCATION_MANAGER_H_INCLUDED

#include <bcm_int/dnx/field/tcam/tcam_location_algo.h>

/**
 * \brief
 *  Searches for a location in TCAM on the given core to add the given entry to.
 *  If no free location was found then 'location' param contains the place where
 *  the entry should have been located or FIELD_TCAM_LOCATION_INVALID if algorithm
 *  fails propsing a valid place for the given entry.
 *
 * \param [in] unit       - Device ID
 * \param [in] core       - Core ID
 * \param [in] handler_id - The handler ID associated with the entry
 * \param [in] entry      - The entry to find a location for
 * \param [out] location  - The location to add the entry to if found.
 *                          Or the location where the entry should've been
 *                          located if no empty place found.
 * \param [out] found     - Whether a location for this entry was found or not
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_location_find(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_entry_t * entry,
    dnx_field_tcam_location_t * location,
    uint8 *found);

/**
 * \brief
 * Checks whether given location is occupied on given core
 *
 * \param [in] unit              - Device ID
 * \param [in] core              - Core ID
 * \param [in] absolute_location - Location to query on occupancy
 * \param [out] occupied         - TRUE if location is occupied
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_location_is_location_occupied(
    int unit,
    int core,
    uint32 absolute_location,
    int *occupied);

/**
 * \brief
 *  Allocates the given location in TCAM on the given core for the given entry.
 *
 * \param [in] unit       - Device ID
 * \param [in] core       - Core ID
 * \param [in] handler_id - The handler ID associated with the entry
 * \param [in] entry      - The entry to allocate the location for
 * \param [in] location   - The location to allocate
 * \param [in] is_dt      - Whether the handler is DT or not
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_location_alloc(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_entry_t * entry,
    dnx_field_tcam_location_t * location,
    uint8 is_dt);

/**
 * \brief
 *  Frees the location in TCAM on the given core of the given entry_id associated
 *  with the given handler_id.
 *
 * \param [in] unit       - Device ID
 * \param [in] core       - Core ID
 * \param [in] handler_id - The handler ID associated with the entry
 * \param [in] entry_id   - ID of the entry to remove
 * \param [in] is_dt      - Whether the handler is DT or not
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_location_free(
    int unit,
    int core,
    uint32 handler_id,
    uint32 entry_id,
    uint8 is_dt);

/**
 * \brief
 *  Updates the location manager regarding a move operation in TCAM on the given core
 *  from location_start to location_end that belongs to the handler which its ID is
 *  given handler_id.
 *
 * \param [in] unit           - Device ID
 * \param [in] core           - Core ID
 * \param [in] handler_id     - The handler ID to which the move opreation belongs to
 * \param [in] location_start - The starting location for the move operation
 * \param [in] location_end   - The ending location for the move operation
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_location_move(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location_start,
    uint32 location_end);

/**
 * \brief
 *  Updates the entry location hash regarding a move operation from location_start
 *  to location_end on the given core that belongs to the handler which its ID is
 *  given handler_id.
 *
 * \param [in] unit           - Device ID
 * \param [in] core           - Core ID
 * \param [in] handler_id     - The handler ID to which the move opreation belongs to
 * \param [in] location_start - The starting location for the move operation
 * \param [in] location_end   - The ending location for the move operation
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_location_move_extended(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location_start,
    uint32 location_end);

/**
 * \brief
 *  Returns the first entry ID for the given handler_id.
 *  The first entry ID for a given handler is defined to be the ID
 *  of the entry that both belongs to the handler and is the first
 *  in line between all other entries that belongs to the same handler
 *  inside the TCAM banks on the given core.
 *
 * \param [in] unit            - Device ID
 * \param [in] core            - Core ID
 * \param [in] handler_id      - the handler ID to get the first entry ID for
 * \param [out] first_entry_id - The ID of the first entry for the given handler, if exists.
 *                               DNX_FIELD_ENTRY_ACCESS_ID_INVALID otherwise
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_location_handler_first_get(
    int unit,
    int core,
    uint32 handler_id,
    uint32 *first_entry_id);

/**
 * \brief
 *  Returns the next entry ID following the given entry ID for the specified handler ID.
 *  The next entry is defined to be the next-in-line to come inside the TCAM banks on the
 *  given core after the given entry id that belongs to the given handler_id.
 *
 * \param [in] unit           - Device ID
 * \param [in] core           - Core ID
 * \param [in] handler_id     - The handler ID to get the next entry ID for
 * \param [in] entry_id       - The entry ID that precedes the entry that its ID will be returned
 * \param [out] next_entry_id - The entry ID that both belongs to the handler and comes 
 *                              after the given entry_id, if exists.
 *                              DNX_FIELD_ENTRY_ACCESS_ID_INVALID otherwise
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_location_handler_next_get(
    int unit,
    int core,
    uint32 handler_id,
    uint32 entry_id,
    uint32 *next_entry_id);

/**
 * \brief
 *  Returns the location for the given entry_id on the given core.
 *
 * \param [in] unit      - Device ID
 * \param [in] core      - Core to get the entry location on
 * \param [in] entry_id  - The entry ID to get the location for
 * \param [out] location - The location for the given entry_id
 *
 * \return
 *   \retval _SHR_E_NONE success
 */
shr_error_e dnx_field_tcam_location_entry_id_to_location(
    int unit,
    int core,
    uint32 entry_id,
    dnx_field_tcam_location_t * location);

/**
 * \brief
 *  Returns the number of all entries that belong to the given handler and reside
 *  in the given bank_id on the given core.
 *
 * \param [in] unit           - Device ID
 * \param [in] core           - Core ID
 * \param [in] handler_id     - The handler ID we want to count its entries in the bank
 * \param [in] bank_id        - The bank we want to count entries in
 * \param [out] entries_count - The number of entries for the given handler in the given bank 
 *
 * \return
 *  \retval _SHR_E_NONE - Success
 */
shr_error_e dnx_field_tcam_location_bank_entries_count(
    int unit,
    int core,
    uint32 handler_id,
    uint32 bank_id,
    uint32 *entries_count);

/**
 * \brief
 *  Returns the priority for the entry whose ID is the given entry_id.
 *
 * \param [in] unit       - Device ID
 * \param [in] handler_id - The handler ID associated with the entry
 * \param [in] entry_id   - The entry_id to return the priority for
 * \param [out] priority  - The priority of the entry
 *
 * \return
 *  \retval _SHR_E_NONE - Success
 */
shr_error_e dnx_field_tcam_location_entry_priority_get(
    int unit,
    uint32 handler_id,
    uint32 entry_id,
    uint32 *priority);

/**
 * \brief
 *  Returns an indication for status of the given entry_id on the given core.
 *  If '1' entry exists, if '0' entry doesn't exist.
 *
 * \param [in] unit      - Device ID
 * \param [in] core      - Core on which entry have to be checked
 * \param [in] entry_id  - The entry ID to check if exists
 * \param [out] found_p - '1' if entry exists, '0' if not.
 *
 * \return
 *   \retval _SHR_E_NONE success
 */
shr_error_e dnx_field_tcam_location_entry_id_exists(
    int unit,
    int core,
    uint32 entry_id,
    uint8 *found_p);

/**
 * \brief
 *  Returns the handler_id that is allocating the given location
 *  on the given core.
 *
 * \param [in] unit        - Device ID
 * \param [in] core        - Core on which location is found
 * \param [in] location    - Location that is allocated
 * \param [out] handler_id - Handler ID that is allocating the given location
 *
 * \return
 *   \retval _SHR_E_NONE success
 */
shr_error_e dnx_field_tcam_location_allocating_handler(
    int unit,
    int core,
    dnx_field_tcam_location_t * location,
    uint32 *handler_id);

/** } */
#endif
