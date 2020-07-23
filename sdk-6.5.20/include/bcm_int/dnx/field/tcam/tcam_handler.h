/**
* \file        tcam_handler.h
*
* DESCRIPTION :
*       This files represents the TCAM handler class, which connects between
*       its database field and the TCAM, and exposes functions to add/remove
*       entries for the database. After opening a connection with required
*       database, user can start adding/removing entries from this database to
*       the TCAM by calling entry_add and entry_remove.
*
* Sample API usage:
* \code{.c}
*       uint32 handler_id;
*       dnx_field_tcam_database_t *db;
*       dnx_field_tcam_entry_t *entry;
*       fill_db(db); "foo function to fill data for the db"
*       fill_entry(entry); "foo function to fill data for the entry"
*       dnx_field_tcam_handler_create(unit,
*           db,
*           NULL, "No bank pre-allocation, struct defined in tcam_bank_manager.h"
*           FIELD_TCAM_ACCESS_PROFILE_ID_AUTO, "manager will allocate access_profile_id, defined in tcam_access_profile_manager.h"
*           DNX_FIELD_TCAM_HANDLER_MODE_DEFAULT,
*           &handler_id
*           );
*       dnx_field_tcam_handler_entry_add(unit, handler_id, entry);
* \endcode
*
*
* PUBLIC FUNCTIONS (dnx_field_tcam omitted):
*       shr_error_e    handler_create( unit, database, nof_prealloc, loose, *handler_id)
*       shr_error_e    handler_destroy( unit, handler_id )
*       shr_error_e    handler_access_profile_id_get( unit, handler_id, *access_p_id )
*       shr_error_e    handler_entry_location_alloc( unit, handler_id, entry )
*       shr_error_e    handler_entry_location_free( unit, handler_id, entry_id )
*       shr_error_e    handler_clear( unit, handler_id )
*/
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef TCAM_HANDLER_H_INCLUDED
/** { */
#define TCAM_HANDLER_H_INCLUDED

#include <include/bcm_int/dnx/field/tcam/tcam_entities.h>
#include <include/bcm_int/dnx/field/tcam/tcam_bank_manager.h>
#include <include/bcm_int/dnx/field/tcam/tcam_access_profile_manager.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_manager_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_location_types.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>

#define DNX_FIELD_TCAM_HANDLER_ID_INVALID 0x1023

/**
 * \brief
 *  Create a connection between the given database and the TCAM banks by allocating an access
 *  profile id for the given database, then pre-allocating banks according to bank_prealloc data.
 *
 * \param [in] unit                        - Device ID
 * \param [in] db                          - The database to open the connection to
 * \param [in] access_profile_id           - Requested access profile ID to allocate for this handler.
 *                                           If FIELD_TCAM_ACCESS_PROFILE_ID_AUTO then ID is automatically
 *                                           allocated.
 * \param [in] mode                        - Mode for the handler to be created
 * \param [in] context_sharing_handlers_cb - Callback function that handler will invoke whenever adding
 *                                           a new bank to the newly created handler that will provide
 *                                           information about same-context handlers in order to prevent
 *                                           same-context handlers from allocating the same bank
 * \param [out] handler_id                 - ID of the newly created handler
 *
 * \return
 *   \retval _SHR_E_NONE success
 * \remark
 * SPECIAL_OUTPUTS :
 *       HW:
 *           TCAM_ACCESS_PROFILE
 * PROCESS :
 *                   [1] Check that database doesn't already have an open connection
 *                   [2] Allocate an access profile id and write it to HW
 *                   [3] Pre-allocate nof_prealloc banks if prealloc info is not NULL
 */
shr_error_e dnx_field_tcam_handler_create(
    int unit,
    dnx_field_tcam_database_t * db,
    int access_profile_id,
    dnx_field_tcam_handler_mode_e mode,
    dnx_field_tcam_context_sharing_handlers_get_p context_sharing_handlers_cb,
    uint32 *handler_id);

/**
 * \brief
 *  Destroys the given handler_id by freeing the access profile ID associated with the handler's
 *  database. If the given handler has any entries installed inside the HW, the destroy operation
 *  will fail.
 *
 * \param [in] unit       - Device ID
 * \param [in] handler_id - The handler ID to destroy
 *
 * \return
 *   \retval _SHR_E_NONE success
 * \remark
 * SPECIAL OUTPUTS:
 *       HW:
 *           TCAM_ACCESS_PROFILE
 *           TCAM_BANK
 * PROCESS :
 *                   [1] Check that handler is valid
 *                   [2] Removes all database's entries from TCAM
 *                   [3] Frees all allocated banks
 *                   [4] Frees the access_profile_id
 */
shr_error_e dnx_field_tcam_handler_destroy(
    int unit,
    uint32 handler_id);

/**
 * \brief
 *  Returns the access profile ID for the given handler_id.
 *
 * \param [in] unit               - Device ID
 * \param [in] handler_id         - The handler ID to get the access profile ID for
 * \param [out] access_profile_id - The access profile ID of the given handler
 *
 * \return
 *   \retval _SHR_E_NONE success
 * \remark
 * PROCESS :
 *                   [1] Checks that handler is valid
 *                   [2] Call dnx_field_tcam_access_profile_id_get
 */
shr_error_e dnx_field_tcam_handler_access_profile_id_get(
    int unit,
    uint32 handler_id,
    int *access_profile_id);

/**
 * \brief
 *  Returns the APP_DB_ID for the given tcam_handler.
 *
 * \param [in] unit       - Device ID
 * \param [in] handler_id - The handler ID to get the "app db ID" for
 * \param [out] app_db_id - the "app db ID" of the given handler_id
 *
 * \return
 *   \retval _SHR_E_NONE success
 * \remark
 *   Calls dnx_field_tcam_handler_access_profile_id_get and converts to dnx_field_app_db_id_t type.
 */
shr_error_e dnx_field_tcam_handler_app_db_id_get(
    int unit,
    uint32 handler_id,
    dnx_field_app_db_id_t * app_db_id);

/**
 * \brief
 *  Adds the given DT entry to TCAM on the given core.
 *  This function must be invoked by direct table handler only.
 *
 * \param [in] unit       - Device ID
 * \param [in] core       - Core ID
 * \param [in] handler_id - The DT handler ID associated with the entry
 * \param [in] entry      - The DT entry to allocate location for in TCAM
 *
 * \return
 *   \retval _SHR_E_NONE success
 * \remark
 * SPECIAL OUTPUT:
 *      HW:
 *          TCAM_BANK/TCAM_BANK_ACTION      Writes the entry and its action
 * PROCESS :
 *                  [1] Check that handler is valid/entry is not null
 *                  [2] Check that entry doesn't already exist
 *                  [3] Find an empty location for the entry
 *                  [4] Write entry to HW
 */
shr_error_e dnx_field_tcam_handler_entry_dt_location_alloc(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_entry_t * entry);

/**
 * \brief
 *  Adds the given entry to TCAM on the given core.
 *
 * \param [in] unit       - Device ID
 * \param [in] core       - Core ID
 * \param [in] handler_id - The handler ID associated with the entry
 * \param [in] entry      - The entry to allocate location for in TCAM
 *
 * \return
 *   \retval _SHR_E_NONE success
 * \remark
 * SPECIAL OUTPUT:
 *      HW:
 *          TCAM_BANK/TCAM_BANK_ACTION      Writes the entry and its action
 * PROCESS :
 *                  [1] Check that handler is valid/entry is not null
 *                  [2] Check that entry doesn't already exist
 *                  [3] Find an empty location for the entry
 *                  [4] Write entry to HW
 */
shr_error_e dnx_field_tcam_handler_entry_location_alloc(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_entry_t * entry);

/**
 * \brief
 *  Update valid bit for given entry
 *
 * \param [in] unit       - Device ID
 * \param [in] core       - Core ID entry is found on
 * \param [in] entry_id   - The entry to set valid bit for
 * \param [in] valid_bit  - New valid bit
 *
 * \return
 *   \retval _SHR_E_NONE Success
 *
 * \remark
 *  None
 */

shr_error_e dnx_field_tcam_handler_entry_valid_set(
    int unit,
    int core,
    uint32 entry_id,
    uint8 valid_bit);

/**
 * \brief
 *  Update valid bit for given entry
 *
 * \param [in] unit       - Device ID
 * \param [in] core       - Core ID entry is found on
 * \param [in] entry_id   - The entry to get its valid bit
 * \param [out] valid_bit - Entry's valid bit
 *
 * \return
 *   \retval _SHR_E_NONE Success
 *
 * \remark
 *  None
 */
shr_error_e dnx_field_tcam_handler_entry_valid_get(
    int unit,
    int core,
    uint32 entry_id,
    uint8 *valid_bit);

/**
 * \brief
 *  Removes given entry_id associated with the given handler_id from TCAM database
 *
 * \param [in] unit       - Device ID
 * \param [in] handler_id - The Handler ID that contains the given entry
 * \param [in] entry_id   - The entry ID to remove from TCAM
 *
 * \return
 *  \retval _SHR_E_NONE success
 * \remark
 * PROCESS :
 *                  [1] Check that tcam_handler is valid
 *                  [2] Check that entry already exist
 *                  [3] Remove entry from database
 */
shr_error_e dnx_field_tcam_handler_entry_location_free(
    int unit,
    uint32 handler_id,
    uint32 entry_id);

/**
 * \brief
 *  Returns the state of the given TCAM handler_id.
 *
 * \param [in] unit       - Device ID
 * \param [in] handler_id - The handler ID to get the "state" for
 * \param [out] state - the "state" of the given handler_id
 *
 * \return
 *   \retval _SHR_E_NONE success
 */
shr_error_e dnx_field_tcam_handler_state_get(
    int unit,
    uint32 handler_id,
    dnx_field_tcam_handler_state_e * state);

/**
 * \brief
 *  Clears the given handler by removing all installed entries from TCAM.
 *
 * \param [in] unit       - Device ID
 * \param [in] handler_id - The handler ID to clear
 *
 * \return
 *   \retval _SHR_E_NONE success
 * \remark
 * SPECIAL OUTPUT:
 *      HW:
 *          TCAM_BANK      Invalidates all handler's database entries
 * PROCESS :
 *                  [1] Check that handler is valid
 *                  [2] Invalidate all database's installed entries
 */
shr_error_e dnx_field_tcam_handler_clear(
    int unit,
    uint32 handler_id);

/**
 * \brief
 *  Notifies the manager about the given handler ID being attached to a field program.
 *  It does so by supplying which other handler IDs are in the same program that the
 *  given handler ID is being attached to.
 *
 *  \param [in] unit                     - Device ID
 *  \param [in] handler_id               - The handler ID being attached to a program
 *  \param [in] same_program_handler_ids - Array of IDs of the handlers that are already in
 *                                         the program
 *  \param [in] handlers_count           - Count of the handler IDs array
 *
 *  \return
 *   \retval _SHR_E_NONE - success
 *   \retval _SHR_E_RESOURCE - Attaching error: two handlers in same program will share same bank
 */
shr_error_e dnx_field_tcam_handler_program_attach(
    int unit,
    uint32 handler_id,
    uint32 *same_program_handler_ids,
    int handlers_count);

/**
 * \brief
 *  Returns the first entry id for the given handler_id.
 *  The first entry id for a given handler is defined to be the id
 *  of the entry that both belongs to the handler and is the first
 *  in line between all other entries that belongs to the same handler
 *  inside the TCAM banks on the given core.
 *
 * \param [in] unit            - Device ID
 * \param [in] core            - Core ID
 * \param [in] handler_id      - handler ID to get the first entry ID for
 * \param [out] first_entry_id - The ID of the first entry for the given handler, if exists.
 *                               DNX_FIELD_ENTRY_ACCESS_ID_INVALID otherwise
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_handler_entry_first(
    int unit,
    int core,
    uint32 handler_id,
    uint32 *first_entry_id);

/**
 * \brief
 *  Returns the next entry id following the given entry id for the specified handler id.
 *  The next entry is defined to be the next-in-line to come inside the TCAM banks on
 *  the given core after the given entry id that belongs to the given handler id.
 *
 * \param [in] unit           - Device ID
 * \param [in] core           - Core ID
 * \param [in] handler_id     - handler ID to get the next entry for
 * \param [in] entry_id       - The entry ID that precedes the entry that will be returned
 * \param [out] next_entry_id - The entry ID that both belongs to the handler and comes
 *                              after the given entry_id, if exists.
 *                              DNX_FIELD_ENTRY_ACCESS_ID_INVALID otherwise
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_handler_entry_next(
    int unit,
    int core,
    uint32 handler_id,
    uint32 entry_id,
    uint32 *next_entry_id);

/**
 * \brief
 *  Returns the location/core for the given entry_id on the given core.
 *
 * \param [in] unit      - Device ID
 * \param [in] core      - Core ID to get the entry location on
 * \param [in] entry_id  - the entry ID to get the location for
 * \param [out] location - the location for the given entry_id
 *
 * \return
 *   \retval _SHR_E_NONE success
 */
shr_error_e dnx_field_tcam_handler_entry_location(
    int unit,
    int core,
    uint32 entry_id,
    dnx_field_tcam_location_t * location);

/**
 * \brief
 *  Returns information about the entries for the given TCAM handler id.
 *  The info returned includes info about the number of inserted entries that
 *  belong to the given handler_id distributed over all the TCAM banks on the
 *  given core in the device. This information is returned in the
 *  variable bank_entries_count which is an array of size
 *  DNX_DATA_MAX_FIELD_TCAM_NOF_BANKS that the user needs to allocate before
 *  calling this function.
 *  Another piece of information returned by this function through the banks_bmp
 *  variable, is a bitmap of all allocated banks for the given handler_id.
 *
 * \param [in] unit                - Device ID
 * \param [in] core                - Core ID
 * \param [in] handler_id          - The handler to get the information for.
 * \param [out] bank_entries_count - An array that the user should allocate
 *                                    the space for, should be of size
 *                                    DNX_DATA_MAX_FIELD_TCAM_NOF_BANKS.
 *                                    Each element in the array represents
 *                                    the number of entries for the corresponding
 *                                    TCAM_BANK which ID is the index to the
 *                                    element.
 * \param [out] banks_bmp          - An output bitmap of all allocated banks
 *                                    for the given handler.
 *
 * \return
 *  \retval _SHR_E_NONE - Success
 */
shr_error_e dnx_field_tcam_handler_entries_info(
    int unit,
    int core,
    uint32 handler_id,
    uint32 bank_entries_count[DNX_DATA_MAX_FIELD_TCAM_NOF_BANKS],
    uint32 *banks_bmp);

/**
 * \brief
 *  Returns the priority for the entry whose id is the given entry_id.
 *
 * \param [in] unit       - Device ID
 * \param [in] handler_id - The handler ID that contains the entry
 * \param [in] entry_id   - The entry ID to return the priority for
 * \param [out] priority  - The priority of the entry
 *
 * \return
 *  \retval _SHR_E_NONE - Success
 */
shr_error_e dnx_field_tcam_handler_entry_priority_get(
    int unit,
    uint32 handler_id,
    uint32 entry_id,
    uint32 *priority);

/**
* \brief
*  Returns the bank allocation mode (BAM) for the given handler id.
*
 * \param [in] unit                  - Device ID
 * \param [in] handler_id            - The handler ID to get the BAM for
 * \param [out] bank_allocation_mode - The BAM for the given handler ID
 *
 * \return
 *  \retval _SHR_E_NONE - Success
*/
shr_error_e dnx_field_tcam_handler_bank_allocation_mode_get(
    int unit,
    uint32 handler_id,
    dnx_field_tcam_bank_allocation_mode_e * bank_allocation_mode);

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
 *  \retval _SHR_E_PARAM    - One of the given params is invalid or OOR
 *  \retval _SHR_E_FAIL     - No more space left to move entries to
 *  \retval _SHR_E_INTERNAL - Something wrong happened with the algorithm (sanity checks failure)
 */
shr_error_e dnx_field_tcam_handler_bank_evacuate(
    int unit,
    uint32 handler_id,
    int nof_banks,
    int bank_ids[]);

/**
 * \brief
 * Adds the given handler_id to the given bank_ids.
 *
 * \param[in] unit       - Device ID
 * \param[in] handler_id - Handler ID to add to given banks
 * \param[in] nof_banks  - Number of banks to add
 * \param[in] bank_ids   - Array of bank IDs to add
 *
 * \return
 *  \retval _SHR_E_NONE     - Success
 *  \retval _SHR_E_PARAM    - One of the given params is invalid or OOR
 */
shr_error_e dnx_field_tcam_handler_bank_add(
    int unit,
    uint32 handler_id,
    int nof_banks,
    int bank_ids[]);

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
shr_error_e dnx_field_tcam_handler_entry_id_exists(
    int unit,
    int core,
    uint32 entry_id,
    uint8 *found_p);

/**
 * \brief
 *  Returns the TCAM handler ID of the given entry ID.
 *
 * \param [in] unit        - Device ID
 * \param [in] entry_id    - Entry ID to return the TCAM Handler ID for
 * \param [out] handler_id - TCAM Handler ID of the given entry ID
 *
 * \return
 *   \retval _SHR_E_NONE success
 */
shr_error_e dnx_field_tcam_handler_entry_handler(
    int unit,
    uint32 entry_id,
    uint32 *handler_id);

/** } */
#endif
