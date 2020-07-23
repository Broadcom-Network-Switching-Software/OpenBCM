/**
* \file        tcam_handler.c
*
* This file implements all the TCAM handler functions.
 */
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm_int/dnx/field/tcam/tcam_handler.h>
#include <bcm_int/dnx/field/tcam/tcam_location_manager.h>
#include <bcm_int/dnx/field/tcam/tcam_defrag.h>
#include <bcm_int/dnx/field/field_group.h>

#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_manager_access.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

/**
 * \brief
 * Handler verification:
 * Verifies that the given handler_id is valid
 * \param [in] unit
 * \param [in] handler_id - handler_id to verify
 * \return
 *  \retval _SHR_E_PARAM if handler_id is not in use.
 *  \retval _SHR_E_NONE success
 */
static shr_error_e
dnx_field_tcam_handler_verify(
    int unit,
    uint32 handler_id)
{
    dnx_field_tcam_handler_t handler;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_manager_sw.tcam_handlers.get(unit, handler_id, &handler));
    if (handler.state != DNX_FIELD_TCAM_HANDLER_STATE_OPEN)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Given handler_id %d not in use", handler_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Entry verification:
 * Verifies that the entry is not null, and that the entry's
 * database is equal to the handler's database
 * \param [in] unit
 * \param [in] entry - the entry to verify
 * \return
 *  \retval _SHR_E_PARAM if entry is null, or if handler's database
 *  not equal to entry's database.
 *  \retval _SHR_E_NONE if everything is fine
 * \remark
 * This function assumes handler is not null
 */
static shr_error_e
dnx_field_tcam_handler_entry_verify(
    int unit,
    dnx_field_tcam_entry_t * entry)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(entry, _SHR_E_PARAM, "entry is null");
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * handler_destroy_verify(unit, handler_id) - Verify function for handler_destroy verifies
 * that the handler is valid
 * \param [in] unit
 * \param [in] handler_id - the handler to verify
 * \return
 *  \retval _SHR_E_NONT - success
 */
static shr_error_e
dnx_field_tcam_handler_destroy_verify(
    int unit,
    uint32 handler_id)
{

    uint32 entry_id;
    int core;

    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_field_tcam_handler_verify(unit, handler_id));

    for (core = 0; core < dnx_data_device.general.nof_cores_get(unit); core++)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_first(unit, core, handler_id, &entry_id));
        if (entry_id != DNX_FIELD_ENTRY_ACCESS_ID_INVALID)
        {
            /*
             * There are still entries associated with this handler. Destroy should fail
             */
            SHR_ERR_EXIT(_SHR_E_EXISTS, "Failed to destroy handler %d, there are still added entries for this handler",
                         handler_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * handler_access_profile_id_verify(unit, handler_id) - Verify function for
 * handler_access_profile_id, verifies that the handler is valid.
 * Also checks that the access_profile_id is not NULL.
 * \param [in] unit
 * \param [in] handler_id - the handler to verify
 * \param [in] access_profile_id - the access profile id to verify
 * \return
 *  \retval _SHR_E_NONT - success
 */
static shr_error_e
dnx_field_tcam_handler_access_profile_id_get_verify(
    int unit,
    uint32 handler_id,
    int *access_profile_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_field_tcam_handler_verify(unit, handler_id));
    SHR_NULL_CHECK(access_profile_id, _SHR_E_PARAM, "access_profile_id is null");
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verifcation function for dnx_field_tcam_handler_entry_dt_location_alloc
 * \param [in] unit
 * \param [in] handler_id - the handler to verify
 * \param [in] entry - the entry to verify
 * \return
 *  \retval _SHR_E_NONT - success
 */
static shr_error_e
dnx_field_tcam_handler_entry_dt_location_alloc_verify(
    int unit,
    uint32 handler_id,
    dnx_field_tcam_entry_t * entry)
{
    dnx_field_tcam_access_fg_params_t fg_params;
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_field_tcam_handler_verify(unit, handler_id));
    SHR_INVOKE_VERIFY_DNX(dnx_field_tcam_handler_entry_verify(unit, entry));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));
    if (!fg_params.direct_table)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "This function cannot be invoked for non-DT handlers (handler_id: %d)\n",
                     handler_id);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verifcation function for dnx_field_tcam_handler_entry_location_alloc
 * \param [in] unit
 * \param [in] core       - Core(s) to install entry on
 * \param [in] handler_id - the handler to verify
 * \param [in] entry - the entry to verify
 * \return
 *  \retval _SHR_E_NONT - success
 */
static shr_error_e
dnx_field_tcam_handler_entry_location_alloc_verify(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_entry_t * entry)
{
    dnx_field_tcam_access_fg_params_t fg_params;
    dnx_field_tcam_core_e core_iter;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_tcam_handler_verify(unit, handler_id));
    SHR_INVOKE_VERIFY_DNX(dnx_field_tcam_handler_entry_verify(unit, entry));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));
    if (fg_params.direct_table)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "This function cannot be invoked for DT handlers (handler_id: %d)\n", handler_id);
    }
    if (fg_params.bank_allocation_mode == DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_SELECT_WITH_LOCATION)
    {
        if (!FIELD_TCAM_LOCATION_IS_VALID(entry->priority))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Given entry location (bank_id:%d, bank_offset:%d) for 'SELECT_WITH_LOCATION' handler is not a valid TCAM location\n",
                         DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(entry->priority),
                         DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(entry->priority));
        }
        for (core_iter = 0; core_iter < dnx_data_device.general.nof_cores_get(unit); core_iter++)
        {
            if (core == DNX_FIELD_TCAM_CORE_ALL || core == core_iter)
            {
                int occupied;
                SHR_IF_ERR_EXIT(dnx_field_tcam_location_is_location_occupied
                                (unit, core_iter, entry->priority, &occupied));
                if (occupied)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Given entry location (bank_id:%d, bank_offset:%d) for 'SELECT_WITH_LOCATION' is already occupied by another entry at core %d\n",
                                 DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(entry->priority),
                                 DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(entry->priority), core_iter);
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * handler_entry_remove_verify(unit, handler_id, entry_id) - Verify function for
 * handler_entry_remove, verifies that the handler is valid.
 * Also checks that the entry is not NULL
 * \param [in] unit
 * \param [in] handler_id - the handler to verify
 * \param [in] entry_id - the entry to verify
 * \return
 *  \retval _SHR_E_NONT - success
 */
static shr_error_e
dnx_field_tcam_handler_entry_location_free_verify(
    int unit,
    uint32 handler_id,
    uint32 entry_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_field_tcam_handler_verify(unit, handler_id));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * handler_id_alloc( unit, db, mode, *handler_id ) - Allocates a new
 * handler id for new tcam handler creation. It searches for an unused
 * handler id and allocates for the newly tcam handler to be created.
 * \param [in] unit
 * \param [in] db - The database to create the handler for
 * \param [in] mode - The mode of the new handler
 * \param [out] handler_id - The id of the newly created handler
 * \return
 *  \retval _SHR_E_NONT - success
 */
static shr_error_e
dnx_field_tcam_handler_id_alloc(
    int unit,
    dnx_field_tcam_database_t * db,
    dnx_field_tcam_handler_mode_e mode,
    uint32 *handler_id)
{
    int handler_index;
    dnx_field_tcam_handler_t handler;

    SHR_FUNC_INIT_VARS(unit);

    for (handler_index = 0; handler_index < dnx_data_field.tcam.nof_tcam_handlers_get(unit); handler_index++)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_manager_sw.tcam_handlers.get(unit, handler_index, &handler));
        if (handler.state == DNX_FIELD_TCAM_HANDLER_STATE_CLOSE)
        {
            handler.mode = mode;
            handler.state = DNX_FIELD_TCAM_HANDLER_STATE_OPEN;
            /*
             * Update handler to be used
             */
            SHR_IF_ERR_EXIT(dnx_field_tcam_manager_sw.tcam_handlers.set(unit, handler_index, &handler));
            *handler_id = handler_index;
            SHR_EXIT();
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * handler_id_free( unit, handler_id )
 * Frees the given handler_id
 * \param [in] unit
 * \param [in] handler_id - handler_id to free
 * \return
 *  \retval _SHR_E_NONE success
 */

static shr_error_e
dnx_field_tcam_handler_id_free(
    int unit,
    uint32 handler_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_manager_sw.tcam_handlers.
                    state.set(unit, handler_id, DNX_FIELD_TCAM_HANDLER_STATE_CLOSE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Converts field action_size enum (dnx_field_action_length_type_e) to its corresponding
 *  value in bits (only for TCAM).
 * \param [in] unit                 - unit
 * \param [in] action_size          - Field's action size enum we want to convert
 * \param [out] action_size_in_bits - The converted action size in bits
 * \return
 *  \retval _SHR_E_NONE  - success
 *  \retval _SHR_E_PARAM - Action size enum received is not of type TCAM
 */
static shr_error_e
dnx_field_tcam_handler_action_size_conversion(
    int unit,
    dnx_field_action_length_type_e action_size,
    uint32 *action_size_in_bits)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (action_size)
    {
        case DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_HALF:
            *action_size_in_bits = dnx_data_field.tcam.action_size_half_get(unit);
            break;
        case DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_SINGLE:
            *action_size_in_bits = dnx_data_field.tcam.action_size_single_get(unit);
            break;
        case DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_DOUBLE:
            *action_size_in_bits = dnx_data_field.tcam.action_size_double_get(unit);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error converting action size enum (%d) to TCAM action size", action_size);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Converts field key_size enum (dnx_field_key_length_type_e) to its corresponding
 *  value in bits (only for TCAM).
 * \param [in] unit                 - unit
 * \param [in] key_size          - Field's key size enum we want to convert
 * \param [out] key_size_in_bits - The converted key size in bits
 * \return
 *  \retval _SHR_E_NONE  - success
 *  \retval _SHR_E_PARAM - Key size enum received is not of type TCAM
 */
static shr_error_e
dnx_field_tcam_handler_key_size_conversion(
    int unit,
    dnx_field_key_length_type_e key_size,
    uint32 *key_size_in_bits)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (key_size)
    {
        case DNX_FIELD_KEY_LENGTH_TYPE_DT:
            *key_size_in_bits = dnx_data_field.tcam.dt_max_key_size_get(unit);
            break;
        case DNX_FIELD_KEY_LENGTH_TYPE_HALF:
            *key_size_in_bits = dnx_data_field.tcam.key_size_half_get(unit);
            break;
        case DNX_FIELD_KEY_LENGTH_TYPE_SINGLE:
            *key_size_in_bits = dnx_data_field.tcam.key_size_single_get(unit);
            break;
        case DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE:
            *key_size_in_bits = dnx_data_field.tcam.key_size_double_get(unit);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error converting key size enum (%d) to TCAM key size", key_size);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Saves the database parameters for the allocated handler_id in SWSTATE
 * \param [in] unit                        - unit
 * \param [in] handler_id                  - The allocated handler_id to save the parameters for
 * \param [in] db                          - The database parameters to save
 * \param [in] context_sharing_handlers_cb - Callback function that handler will invoke whenever adding
 *                                           a new bank to the newly created handler that will provide
 *                                           information about same-context handlers in order to prevent
 *                                           same-context handlers from allocating the same bank
 * \return
 *  \retval _SHR_E_NONE  - success
 */
static shr_error_e
dnx_field_tcam_handler_save_params(
    int unit,
    uint32 handler_id,
    dnx_field_tcam_database_t * db,
    dnx_field_tcam_context_sharing_handlers_get_p context_sharing_handlers_cb)
{
    dnx_field_tcam_access_fg_params_t fg_params;
    uint32 action_size_in_bits = 0;
    uint32 key_size_in_bits = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_action_size_conversion(unit, db->action_size, &action_size_in_bits));
    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_key_size_conversion(unit, db->key_size, &key_size_in_bits));
    fg_params.key_size = key_size_in_bits;
    fg_params.action_size = action_size_in_bits;
    fg_params.actual_action_size = db->actual_action_size;
    fg_params.prefix_value = db->prefix_value;
    fg_params.prefix_size = db->prefix_size;
    fg_params.stage = db->stage;
    fg_params.direct_table = db->is_direct;
    fg_params.bank_allocation_mode = db->bank_allocation_mode;
    fg_params.dt_bank_id = DNX_FIELD_TCAM_BANK_ID_INVALID;
    fg_params.context_sharing_handlers_cb = context_sharing_handlers_cb;
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.set(unit, handler_id, &fg_params));
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Saved params for handler %d, actual_action_size: %d, prefix_value: %d, prefix_size: %d\n",
                 handler_id, db->actual_action_size, db->prefix_value, db->prefix_size);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  TCAM handler creation verification, verifies the following:
 *      1) DT databases have Select mode and specify one bank only
 *      2) Mode is legit
 * NOTE: Other verifications such as inner bank information/access_profile_id are performed in different modules!
 *
 * \param [in] unit          - Device ID
 * \param [in] db            - Database information for which we want to create a TCAM handler for
 * \param [in] mode          - Mode of the newly created handler
 *
 * \return
 *  \retval _SHR_E_NONE  - Success
 *  \retval _SHR_E_PARAM - One of the verifications failed
 */
static shr_error_e
dnx_field_tcam_handler_create_verify(
    int unit,
    dnx_field_tcam_database_t * db,
    dnx_field_tcam_handler_mode_e mode)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(db, _SHR_E_PARAM, "db");

    if (db->bank_allocation_mode == DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_INVALID ||
        db->bank_allocation_mode >= DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_NOF)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Bank allocation mode %d not supported\r\n", db->bank_allocation_mode);
    }

    if (db->bank_allocation_mode == DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_AUTO && db->nof_banks_to_alloc > 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error requesting TCAM banks to allocate when Bank Allocation Mode is 'AUTO'\r\n");
    }

    if (db->bank_allocation_mode == DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_SELECT && db->nof_banks_to_alloc == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Bank allocation mode is 'SELECT', while no banks are provided for allocation\r\n");
    }

    if (db->bank_allocation_mode == DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_SELECT_WITH_LOCATION
        && db->nof_banks_to_alloc == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Bank allocation mode is 'SELECT_WITH_LOCATION', while no banks are provided for allocation\r\n");
    }

    if (db->is_direct && db->bank_allocation_mode == DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_SELECT
        && db->nof_banks_to_alloc != 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "TCAM DT database created with 'SELECT' mode should exactly request one bank to be allocated: currently %d banks requested\r\n",
                     db->nof_banks_to_alloc);
    }

    if (db->is_direct && db->banks_to_alloc[0] >= dnx_data_field.tcam.nof_big_banks_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "TCAM DT is not allowed to be used on small banks\r\n");
    }

    if (mode >= DNX_FIELD_TCAM_HANDLER_MODE_NOF_MODES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "TCAM handler requested mode not support: %d\r\n", mode);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_handler_create(
    int unit,
    dnx_field_tcam_database_t * db,
    int access_profile_id,
    dnx_field_tcam_handler_mode_e mode,
    dnx_field_tcam_context_sharing_handlers_get_p context_sharing_handlers_cb,
    uint32 *handler_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_tcam_handler_create_verify(unit, db, mode));

    /*
     * Handler creation steps:
     * 1. Allocate handler_id using TCAM Manager SWSTATE
     * 2. Allocate access_profile
     * 3. Save database related info in SWSTATE
     * 4. Pre-allocate banks (if requested)
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_id_alloc(unit, db, mode, handler_id));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_alloc(unit, *handler_id, db, access_profile_id));
    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_save_params(unit, *handler_id, db, context_sharing_handlers_cb));

    if (db->nof_banks_to_alloc > 0)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_alloc(unit, *handler_id, db->nof_banks_to_alloc, db->banks_to_alloc));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_handler_destroy(
    int unit,
    uint32 handler_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_tcam_handler_destroy_verify(unit, handler_id));

    /*
     * Handler destruction steps:
     * 1. free all banks for the given handler_id
     * 2. free the access profile for the given handler_id
     * 2. free the handler id
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_handler_free(unit, handler_id));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_free(unit, handler_id));
    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_id_free(unit, handler_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_handler_access_profile_id_get(
    int unit,
    uint32 handler_id,
    int *access_profile_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_tcam_handler_access_profile_id_get_verify(unit, handler_id, access_profile_id));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_id_get(unit, handler_id, access_profile_id));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_handler_app_db_id_get(
    int unit,
    uint32 handler_id,
    dnx_field_app_db_id_t * app_db_id)
{
    int access_profile_id_in;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_access_profile_id_get(unit, handler_id, &access_profile_id_in));
    /*
     * Convert access_profile_id_in from int to dnx_field_app_db_id_t type.
     * Check that the convertion did not change the value. Shouldn't happen, as dnx_field_app_db_id_t is at the moment
     * uint32 due to DBAL.
     */
    *app_db_id = (dnx_field_app_db_id_t) access_profile_id_in;
    if ((uint32) access_profile_id_in != (uint32) (*app_db_id))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "dnx_field_app_db_id_t type not big enough to take in access_profile_id.\r\n"
                     "Converted %d to %d.\r\n", access_profile_id_in, *app_db_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Retrieves bank offset from entry_id for DT entries
 *
 * \param [in] unit         - Device ID
 * \param [in] entry_id     - Entry ID to retrieve bank offset from
 * \param [out] bank_offset - The retrieved bank offset from the entry ID
 *
 * \return
 *   \retval _SHR_E_NONE success
 */
static shr_error_e
dnx_field_tcam_handler_dt_calc_offset_from_entry_id(
    int unit,
    uint32 entry_id,
    uint32 *bank_offset)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * bank_offset should be equal to access_id * 2 for DT entries
     *                                 /\         /\
     *                                 |           |
     *                                Key Val     Skip masks
     */
    *bank_offset = DNX_FIELD_ENTRY_ACCESS_ID(entry_id) * 2;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_handler_entry_dt_location_alloc(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_entry_t * entry)
{
    dnx_field_tcam_location_t location;
    dnx_field_tcam_access_fg_params_t fg_params;
    dnx_field_tcam_core_e core_iter;
    int bank_id;
    uint32 bank_offset;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_tcam_handler_entry_dt_location_alloc_verify(unit, handler_id, entry));

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));

    /*
     * For AUTO bank mode allocation: If no bank is allocated, allocate a bank.
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.dt_bank_id.get(unit, handler_id, &bank_id));

    if (bank_id == DNX_FIELD_TCAM_BANK_ID_INVALID)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_single_alloc_any(unit, handler_id));
        /*
         * Get new bank ID to be used to add an entry
         */
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.dt_bank_id.get(unit, handler_id, &bank_id));
    }

    /*
     * Calculate location for DT
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_dt_calc_offset_from_entry_id(unit, entry->id, &bank_offset));

    location.bank_id = bank_id;
    /*
     * DT is allocated to the MSB part of the payload, therefore we add 1 to allocate the MSB (odd)
     * part of the location
     */
    location.bank_offset = bank_offset + 1;

    for (core_iter = 0; core_iter < dnx_data_device.general.nof_cores_get(unit); core_iter++)
    {
        if (core == DNX_FIELD_TCAM_CORE_ALL || core == core_iter)
        {
            /*
             * Allocate the location by updating both the location and the bank managers
             */
            SHR_IF_ERR_EXIT(dnx_field_tcam_location_alloc(unit, core_iter, handler_id, entry, &location, TRUE   /* is_dt 
                                                                                                                 */ ));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_handler_entry_location_alloc(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_entry_t * entry)
{
    dnx_field_tcam_location_t location;
    uint8 found;
    uint32 absolute_location;
    dnx_field_tcam_core_e core_iter;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_tcam_handler_entry_location_alloc_verify(unit, core, handler_id, entry));

    for (core_iter = 0; core_iter < dnx_data_device.general.nof_cores_get(unit); core_iter++)
    {
        if (core == DNX_FIELD_TCAM_CORE_ALL || core == core_iter)
        {
            SHR_IF_ERR_EXIT(dnx_field_tcam_location_find(unit, core_iter, handler_id, entry, &location, &found));
            if (!found)
            {
                /*
                 * After failing to add once, try and defrag to make space for the new entry.
                 */
                SHR_IF_ERR_EXIT(dnx_field_tcam_defrag(unit, core_iter, handler_id, &location));
                SHR_IF_ERR_EXIT(dnx_field_tcam_location_find(unit, core_iter, handler_id, entry, &location, &found));
                if (!found)
                {
                    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "No place found for entry %d (entry priority: %d)\n",
                                 entry->id, entry->priority);
                }
            }

            /*
             * Allocate the found location by updating both the location and the bank managers
             */
            SHR_IF_ERR_EXIT(dnx_field_tcam_location_alloc(unit, core_iter, handler_id, entry, &location, FALSE  /* is_dt 
                                                                                                                 */ ));

            absolute_location = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(location.bank_id, location.bank_offset);
            /*
             * Set valid bit (This is not related to the TCAM valid bit in HW, but rather an indication that an
             * entry is occupied)
             */
            SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.valid_bmp.v_bit.bit_set(unit, core_iter, absolute_location));

            /*
             * Notify bank manager about an entry being allocated (to update bank free entries number)
             */
            SHR_IF_ERR_EXIT(dnx_field_tcam_bank_entry_alloc_update(unit, core_iter, absolute_location, handler_id));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_handler_entry_valid_set(
    int unit,
    int core,
    uint32 entry_id,
    uint8 valid_bit)
{
    dnx_field_tcam_core_e core_iter;
    dnx_field_tcam_location_t location;
    uint32 absolute_location;

    SHR_FUNC_INIT_VARS(unit);

    for (core_iter = 0; core_iter < dnx_data_device.general.nof_cores_get(unit); core_iter++)
    {
        if (core == DNX_FIELD_TCAM_CORE_ALL || core == core_iter)
        {
            SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_location(unit, core_iter, entry_id, &location));
            absolute_location = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(location.bank_id, location.bank_offset);
            if (valid_bit)
            {
                SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.valid_bmp.v_bit.bit_set(unit, core_iter, absolute_location));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.valid_bmp.v_bit.bit_clear(unit, core_iter, absolute_location));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_handler_entry_valid_get(
    int unit,
    int core,
    uint32 entry_id,
    uint8 *valid_bit)
{
    dnx_field_tcam_location_t location;
    uint32 absolute_location;

    SHR_FUNC_INIT_VARS(unit);

    if (core == DNX_FIELD_TCAM_CORE_ALL)
    {
        core = DNX_FIELD_TCAM_CORE_FIRST;
    }

    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_location(unit, core, entry_id, &location));
    absolute_location = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(location.bank_id, location.bank_offset);
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.valid_bmp.v_bit.bit_get(unit, core, absolute_location, valid_bit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_handler_entry_location_free(
    int unit,
    uint32 handler_id,
    uint32 entry_id)
{
    dnx_field_tcam_location_t location;
    dnx_field_tcam_access_fg_params_t fg_params;
    uint32 absolute_location;
    dnx_field_tcam_core_e core = DNX_FIELD_TCAM_ENTRY_CORE_ID(entry_id);
    dnx_field_tcam_core_e core_iter;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));

    SHR_INVOKE_VERIFY_DNX(dnx_field_tcam_handler_entry_location_free_verify(unit, handler_id, entry_id));

    for (core_iter = 0; core_iter < dnx_data_device.general.nof_cores_get(unit); core_iter++)
    {
        if (core == DNX_FIELD_TCAM_CORE_ALL || core == core_iter)
        {

            SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_location(unit, core_iter, entry_id, &location));

            /*
             * Call location manager in order to free this entry
             */
            SHR_IF_ERR_EXIT(dnx_field_tcam_location_free
                            (unit, core_iter, handler_id, entry_id, fg_params.direct_table));

            if (!fg_params.direct_table)
            {
                absolute_location = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(location.bank_id, location.bank_offset);
                /*
                 * Clear valid bit
                 */
                SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.valid_bmp.v_bit.bit_clear(unit, core_iter, absolute_location));
                /*
                 * Notify bank manager about an entry being freed (to update bank free entries number)
                 */
                SHR_IF_ERR_EXIT(dnx_field_tcam_bank_entry_free_update(unit, core_iter, absolute_location, handler_id));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}
shr_error_e
dnx_field_tcam_handler_state_get(
    int unit,
    uint32 handler_id,
    dnx_field_tcam_handler_state_e * state)
{
    dnx_field_tcam_handler_t handler;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_manager_sw.tcam_handlers.get(unit, handler_id, &handler));

    *state = handler.state;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_handler_clear(
    int unit,
    uint32 handler_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Not implemented yet");
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_handler_program_attach(
    int unit,
    uint32 handler_id,
    uint32 *same_program_handler_ids,
    int handlers_count)
{
    uint8 intersect;
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < handlers_count; ii++)
    {
        if (same_program_handler_ids[ii] == DNX_FIELD_TCAM_HANDLER_INVALID)
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_intersect
                        (unit, handler_id, same_program_handler_ids[ii], &intersect));
        if (intersect)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "Handler ID %d can't be attached to program.\r\n"
                         "Reason: Error Sharing bank with Handler ID %d in same program\r\n",
                         handler_id, same_program_handler_ids[ii]);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_handler_entry_first(
    int unit,
    int core,
    uint32 handler_id,
    uint32 *first_entry_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_handler_first_get(unit, core, handler_id, first_entry_id));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_handler_entry_next(
    int unit,
    int core,
    uint32 handler_id,
    uint32 entry_id,
    uint32 *next_entry_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_handler_next_get(unit, core, handler_id, entry_id, next_entry_id));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_handler_entry_location(
    int unit,
    int core,
    uint32 entry_id,
    dnx_field_tcam_location_t * location)
{
    dnx_field_tcam_core_e ent_core = DNX_FIELD_TCAM_ENTRY_CORE_ID(entry_id);
    SHR_FUNC_INIT_VARS(unit);
    if (core >= DNX_FIELD_TCAM_CORE_ALL || core < DNX_FIELD_TCAM_CORE_FIRST)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid core ID %d\r\n", core);
    }
    if (ent_core != DNX_FIELD_TCAM_CORE_ALL && ent_core != core)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The given entry %d is not added to given core %d\r\n", entry_id, core);
    }
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_entry_id_to_location(unit, core, entry_id, location));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_handler_entries_info(
    int unit,
    int core,
    uint32 handler_id,
    uint32 bank_entries_count[DNX_DATA_MAX_FIELD_TCAM_NOF_BANKS],
    uint32 *banks_bmp)
{
    uint32 entries_count;
    uint32 bank_id;

    SHR_FUNC_INIT_VARS(unit);

    entries_count = 0;
    for (bank_id = 0; bank_id < dnx_data_field.tcam.nof_banks_get(unit); bank_id++)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_bank_entries_count(unit, core, handler_id, bank_id, &entries_count));
        bank_entries_count[bank_id] = entries_count;
    }
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_banks_bmp_get(unit, handler_id, banks_bmp));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_handler_entry_priority_get(
    int unit,
    uint32 handler_id,
    uint32 entry_id,
    uint32 *priority)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_entry_priority_get(unit, handler_id, entry_id, priority));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_handler_bank_allocation_mode_get(
    int unit,
    uint32 handler_id,
    dnx_field_tcam_bank_allocation_mode_e * bank_allocation_mode)
{
    dnx_field_tcam_access_fg_params_t fg_params;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));
    *bank_allocation_mode = fg_params.bank_allocation_mode;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verify bank evacuation process
 * \param [in] unit         - unit
 * \param [in] handler_id   - Handler ID to evacuate from given banks
 * \param [in] nof_banks    - Number of banks to evacuate FG from
 * \param [in] bank_ids     - Array of the bank IDs to evacuate FG from
 * \return
 *  \retval _SHR_E_NONE  - success
 */
static shr_error_e
dnx_field_tcam_handler_bank_evacuate_verify(
    int unit,
    uint32 handler_id,
    int nof_banks,
    int bank_ids[])
{
    int bank_idx;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_verify(unit, handler_id));

    if (nof_banks <= 0 || nof_banks > dnx_data_field.tcam.nof_banks_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Number of banks to evacuate needs to be between 1-%d, value given: %d\n",
                     dnx_data_field.tcam.nof_banks_get(unit), nof_banks);
    }

    for (bank_idx = 0; bank_idx < nof_banks; bank_idx++)
    {
        uint8 found;

        if (bank_ids[bank_idx] < 0 || bank_ids[bank_idx] >= dnx_data_field.tcam.nof_banks_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Given Bank ID %d at index %d during bank add evacuate is out of range\n",
                         bank_ids[bank_idx], bank_idx);
        }
        /*
         * Check that bank allocated for given handler
         */
        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_is_handler_on_bank(unit, bank_ids[bank_idx], handler_id, &found));

        if (!found)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Given bank %d is not allocated for handler %d\n", bank_ids[bank_idx],
                         handler_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verify bank add process
 * \param [in] unit         - unit
 * \param [in] handler_id   - Handler ID to add to given banks
 * \param [in] nof_banks    - Number of banks to add FG to
 * \param [in] bank_ids     - Array of the bank IDs to add FG to
 * \return
 *  \retval _SHR_E_NONE  - success
 */
static shr_error_e
dnx_field_tcam_handler_bank_add_verify(
    int unit,
    uint32 handler_id,
    int nof_banks,
    int bank_ids[])
{
    int bank_idx;
    uint32 banks_bmp;
    dnx_field_tcam_bank_allocation_mode_e bank_allocation_mode;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_verify(unit, handler_id));

    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_bank_allocation_mode_get(unit, handler_id, &bank_allocation_mode));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_banks_bmp_get(unit, handler_id, &banks_bmp));

    if (bank_allocation_mode == DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_AUTO && banks_bmp != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error adding bank(s) for TCAM Database with 'AUTO' bank allocation mode\n");
    }

    if (nof_banks < 0 || nof_banks > dnx_data_field.tcam.nof_banks_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Number of banks to add needs to be between 0-%d, value given: %d\n",
                     dnx_data_field.tcam.nof_banks_get(unit) - 1, nof_banks);
    }

    for (bank_idx = 0; bank_idx < nof_banks; bank_idx++)
    {
        if (bank_ids[bank_idx] < 0 || bank_ids[bank_idx] > dnx_data_field.tcam.nof_banks_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Given Bank ID %d at index %d during bank add call is out of range\n",
                         bank_ids[bank_idx], bank_idx);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_handler_bank_evacuate(
    int unit,
    uint32 handler_id,
    int nof_banks,
    int bank_ids[])
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_tcam_handler_bank_evacuate_verify(unit, handler_id, nof_banks, bank_ids));

    SHR_IF_ERR_EXIT(dnx_field_tcam_defrag_bank_evacuate(unit, handler_id, nof_banks, bank_ids));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_handler_bank_add(
    int unit,
    uint32 handler_id,
    int nof_banks,
    int bank_ids[])
{
    dnx_field_tcam_bank_allocation_mode_e bank_allocation_mode;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_tcam_handler_bank_add_verify(unit, handler_id, nof_banks, bank_ids));

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.
                    bank_allocation_mode.get(unit, handler_id, &bank_allocation_mode));
    /**
     * If Bank Add was requested for 'AUTO' bank allocation mode, we change the mode to 'SELECT'.
     * Note that this can only happen when the 'AUTO' mode has no banks allocated (it was already verified in the
     * verify function
     */
    if (bank_allocation_mode == DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_AUTO)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.
                        bank_allocation_mode.set(unit, handler_id, DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_SELECT));
    }

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_alloc(unit, handler_id, nof_banks, bank_ids));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_handler_entry_id_exists(
    int unit,
    int core,
    uint32 entry_id,
    uint8 *found_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_entry_id_exists(unit, core, entry_id, found_p));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_handler_entry_handler(
    int unit,
    uint32 entry_id,
    uint32 *handler_id)
{
    int core = DNX_FIELD_TCAM_ENTRY_CORE_ID(entry_id);
    dnx_field_tcam_location_t location;
    SHR_FUNC_INIT_VARS(unit);

    if (core == DNX_FIELD_TCAM_CORE_ALL)
    {
        core = 0;
    }

    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_location(unit, core, entry_id, &location));

    SHR_IF_ERR_EXIT_WITH_LOG(dnx_field_tcam_location_allocating_handler(unit, core, &location, handler_id),
                             "No TCAM handler found for the given entry ID 0x%08x%s%s", entry_id, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}
