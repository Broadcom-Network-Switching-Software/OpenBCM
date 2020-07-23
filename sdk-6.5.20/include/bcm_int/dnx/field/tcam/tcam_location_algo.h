/**
* \file        tcam_location_algo.h
*
* DESCRIPTION :
*       This file represents the TCAM location algo component. It exposes
*       all the location algorithmic functionality the TCAM Location Manager needs.
*
* PUBLIC FUNCTIONS (dnx_field_tcam omitted):
*       shr_error_e    location_find_algo( unit, entry, *location )
*/
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef TCAM_LOCATION_ALGO_H_INCLUDED
/** { */
#define TCAM_LOCATION_ALGO_H_INCLUDED

#include <bcm_int/dnx/field/tcam/tcam_entities.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_location_types.h>

/**
 * Empty location find mode, sepcifies how to search for the empty location:
 * MODE_FORWARD - Find empty location only by doing forward search from given the location.
 * MODE_BACKWARD - Find empty location only by doing backward search from given the location.
 * MODE_CLOSEST - Find empty location that is the closest to the given location
 */
typedef enum
{
    DNX_FIELD_TCAM_LOCATION_FIND_MODE_FORWARD = 1,
    DNX_FIELD_TCAM_LOCATION_FIND_MODE_BACKWARD = 2,
    DNX_FIELD_TCAM_LOCATION_FIND_MODE_CLOSEST = 3,
} dnx_field_tcam_location_find_mode_e;

/**
 * \brief
 * Marks the given location in TCAM SWSTATE on given core as free.
 * This function is used to update the algo algorithm about a location with
 * is being freed so that this module changes its databases accordingly to
 * stay updated.
 *
 * \param [in] unit       - Device ID
 * \param [in] core       - Core ID
 * \param [in] handler_id - The handler_id that is associated with the given entry
 * \param [in] location   - The location to be updated.
 * \param [in] is_dt      - Specifies whether the TCAM handler is DT or not
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_location_algo_update_free(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_location_t * location,
    uint8 is_dt);

/**
 * \brief
 *  Updates the given location with the given entry for the given handler_id.
 *  This function is used to update the algo algorithm with an inserted entry to ensure
 *  the data that is being used in order to search for an entry is up-to-date.
 *  The function updates the TCAM SWSTATE on the given core.
 *
 * \param [in] unit       - Device ID
 * \param [in] core       - Core ID
 * \param [in] handler_id - The handler_id that is associated with the given entry
 * \param [in] entry      - The entry to appoint to the given location.
 * \param [in] location   - The location to be updated.
 * \param [in] is_dt      - Specifies whether the TCAM handler is DT or not
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_location_algo_update_alloc(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_entry_t * entry,
    dnx_field_tcam_location_t * location,
    uint8 is_dt);

/**
 * \brief
 *  Searches for a location in TCAM SWSTATE on the given core to add the given entry to,
 *  the entry is associated with the given handler id.
 *
 * \param [in] unit       - Device ID
 * \param [in] core       - Core ID
 * \param [in] handler_id - The handler_id of the given entry
 * \param [in] entry      - The entry to find a location for
 * \param [out] location  - The location to add the entry to if found.
 *                          Or the location where the entry should've been
 *                          located if no empty place found.
 * \param [out] found     - Whether this entry was found or not
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_location_algo_find(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_entry_t * entry,
    dnx_field_tcam_location_t * location,
    uint8 *found);

/**
 * \brief
 *  Marks the given location as occupied, the number of marked slots depends
 *  on the key_size of the given handler_id.
 *
 * \param [in] unit          - Device ID
 * \param [in] core          - Core ID
 * \param [in] handler_id    - ID of the handler that its entry is occupying
 *                             the given location
 * \param [in] location      - The location to mark as occupied
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_location_algo_mark_as_occupied(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location);

/**
 * \brief
 *  This function makes a total move for the location algo, it updates
 *  all the involved SWSTATEs that are in use of the location algo with
 *  the indicated move, the move operation is performed for the given core.
 *
 * \param [in] unit          - Device ID
 * \param [in] core          - Core ID
 * \param [in] handler_id    - ID to the handler involved in the move operation
 * \param [in] location_from - The location from which the first entry is going
 *                             to be moved from
 * \param [in] location_to   - The location to which the last entry is going to be
 *                             moved to
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_location_algo_move(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location_from,
    uint32 location_to);

/**
 * \brief
 *  Gets the next given handler's entry location that is closest to the given location.
 *  This function searches forward starting from the given location for a suitable entry.
 *  This function searches the TCAM SWSTATE on the given core.
 *
 * \param [in] unit           - Device ID
 * \param [in] core           - Core ID
 * \param [in] handler_id     - ID to the handler which entries are to consider
 * \param [in] location       - The location to which the next adjacent entry
 *                              should be returned
 * \param [out] next_location - The absolute location of the next adjacent entry if found
 * \param [out] found         - TRUE if such an entry was found.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_location_algo_handler_next_get(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location,
    uint32 *next_location,
    uint8 *found);

/**
 * \brief
 *  Gets the prev given handler's entry location that is closest to the given location.
 *  This function searches backwards starting from the given location for a suitable entry.
 *  This function searches the TCAM SWSTATE on the given core.
 *
 * \param [in] unit           - Device ID
 * \param [in] core           - Core ID
 * \param [in] handler_id     - ID to the handler which entries are to consider
 * \param [in] location       - The location to which the next adjacent entry
 *                              should be returned
 * \param [out] prev_location - The absolute location of the prev adjacent entry if found
 * \param [out] found         - TRUE if such an entry was found.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */

shr_error_e dnx_field_tcam_location_algo_handler_prev_get(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location,
    uint32 *prev_location,
    uint8 *found);

/**
 * \brief
 *  Gets the next/prev given handler's entry location that is closest to the given location.
 *  This function searches forward/backward starting from the given location for a suitable entry.
 *  This function searches the TCAM SWSTATE on the given core.
 *
 * \param [in] unit           - Device ID
 * \param [in] core           - Core ID
 * \param [in] handler_id     - ID to the handler which entries are to consider
 * \param [in] location       - The location to which the next/prev adjacent entry should be
 *                              retrieved
 * \param [in] backward       - If TRUE get prev entry, otherwise next
 * \param [out] next_location - The absolute location of the next/prev adjacent entry if found
 * \param [out] found         - TRUE if such an entry was found
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_location_algo_handler_next_get_extended(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location,
    uint8 backward,
    uint32 *next_location,
    uint8 *found);

/**
 * \brief
 *   Retruns the state of the complementary halves of
 *   the single/double entries at the given location
 *   on the given core.
 *
 * \param[in] unit            - Device ID
 * \param[in] core            - Core ID
 * \param[in] location        - Location to get the complementary state for
 * \param[in] single_occupied - True if the complementary half of the single entry at the given location is pre-occupied
 * \param[in] double_occupied - True if one of the complementary halves of the double entry at the given location is pre-occupied
 *
 * \return
 *  \retval _SHR_E_NONE - Success
 */
shr_error_e dnx_field_tcam_location_algo_entry_comp_state(
    int unit,
    int core,
    uint32 location,
    uint8 *single_occupied,
    uint8 *double_occupied);

/**
 * \brief
 *  Finds an empty place for the given handler_id on given core.
 *  The search starts from the given location, the direction, forward or backwards (or the closest betweeen them)
 *  is decided according to find_mode. The empty place for the handler is defined to be present in one of the banks
 *  allocated by the given handler.
 *
 * \param [in] unit         - Device ID
 * \param [in] core         - Core ID
 * \param [in] handler_id   - The handler_id to find the empty place for
 * \param [in] location     - The location to start from
 * \param [in] find_mode    - Determines how to find the empty place
 * \param [out] empty_place - The location of the empty place/undefined if not found
 * \param [out] found       - Whether an empty place was found or not
 *
 * \return
 *  \retval _SHR_E_NONE - success
 *  \retval _SHR_E_INTERNAL - If something wrong internally with the SWSTATE used by
 *                            this function
 */
shr_error_e dnx_field_tcam_location_algo_empty_location_find(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_location_t * location,
    dnx_field_tcam_location_find_mode_e find_mode,
    uint32 *empty_place,
    uint8 *found);

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
shr_error_e dnx_field_tcam_location_algo_allocating_handler(
    int unit,
    int core,
    dnx_field_tcam_location_t * location,
    uint32 *handler_id);

/** } */

#endif
