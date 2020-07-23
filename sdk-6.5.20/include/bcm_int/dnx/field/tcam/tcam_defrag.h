/**
* \file        tcam_defrag.h
*
* DESCRIPTION :
*       This file represents the defragment component. It exposes
*       funcionality which allows the user to order and organize
*       the TCAM banks in order to make room for other entries to add.
*
*
* PUBLIC FUNCTIONS (dnx_field_tcam omitted):
*       shr_error_e    defrag( unit, db, priority )
*       shr_error_e    defrag_mode( unit, db, priority, mode )
*/
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef TCAM_DEFRAG_H_INCLUDED
/** { */
#define TCAM_DEFRAG_H_INCLUDED

#include <bcm_int/dnx/field/tcam/tcam_location_manager.h>

/**
 * Default defrag mode
 */
#define DNX_FIELD_TCAM_DEFRAG_MODE_DEFAULT 0

/**
 * \brief
 *  Calls "defrag_mode(handler_id, location, 0)" which is the default mode for defrag.
 *
 * \param [in] unit       - Device ID
 * \param [in] core       - Core ID
 * \param [in] handler_id - The handler ID associated with this location
 * \param [in] location   - The location to make room for
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_defrag(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_location_t * location);

/**
 * \brief
 *  Makes room for the given location, according to the mode specified
 *
 * \param [in] unit       - Device ID
 * \param [in] core       - Core ID
 * \param [in] handler_id - The handler ID associated with this location
 * \param [in] location   - The location to make room for
 * \param [in] mode       - The mode of the defrag
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_defrag_mode(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_location_t * location,
    int mode);

/**
 * \brief
 * Evacuates the given handler_id from the given bank_id on all cores.
 * The evacuation process involves moving all handler's entries on the
 * given bank to other banks allocated by the same handler.
 * If the evacuation process succeeds, the handler is removed from the bank (therefore,
 * helping to remove constraints for other handlers to use the evacuated bank).
 *
 * This function starts moving entries to other banks allocated by the given handler
 * until either all handler's entries on the bank are moved to other banks, or no
 * more empty space is left in the other banks allocated by the given handler.
 * In case no empty space is left, all the entries that were moved until this point stay
 * as-is and the function fails.
 *
 * \param[in] unit       - Device ID
 * \param[in] handler_id - Handler ID to move its entries from given bank
 * \param[in] nof_banks  - Number of banks to evacuate
 * \param[in] bank_ids   - Array of bank IDs to evacuate
 *
 * \return
 *  \retval _SHR_E_NONE     - Success
 *  \retval _SHR_E_FAIL     - No more space left to move entries to
 *  \retval _SHR_E_INTERNAL - Something wrong happened with the algorithm (sanity checks failure)
 */
shr_error_e dnx_field_tcam_defrag_bank_evacuate(
    int unit,
    uint32 handler_id,
    int nof_banks,
    int bank_ids[]);

/** } */
#endif
