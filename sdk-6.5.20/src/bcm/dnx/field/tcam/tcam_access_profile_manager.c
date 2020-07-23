/**
 * \file       tcam_access_profile_manager.c
 * This file implements all the functions for the access profile manager of the TCAM.
 * It also defines 2 global variables:
 *  Access_profiles: Holds information about all the access profiles in the system.
 *  Database_hash: A hash that maps between database id and its correspodning profile id.
 */
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <include/bcm_int/dnx/field/tcam/tcam_access_profile_manager.h>

#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_profile_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_access.h>

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

/**
 * Enum that represents what to update on the action bitmap of the access profile
 */
typedef enum
{
    DNX_FIELD_TCAM_ACCESS_PROFILE_ACTION_BMP_INVALID = -1,
    DNX_FIELD_TCAM_ACCESS_PROFILE_ACTION_BMP_REMOVE = 0,
    DNX_FIELD_TCAM_ACCESS_PROFILE_ACTION_BMP_LSB = 1,
    DNX_FIELD_TCAM_ACCESS_PROFILE_ACTION_BMP_MSB = 2,
    DNX_FIELD_TCAM_ACCESS_PROFILE_ACTION_BMP_ALL = 3,
} dnx_field_tcam_access_profile_action_bmp_e;

/**
 * \brief
 *  Searches for an unused access profile for the given db, then sets the output parameter with the found access
 *  profile ID.
 *
 * \param [in] unit               - Device ID
 * \param [in] db                 - The database to allocate an access profile to
 * \param [out] access_profile_id - The allocated access profile ID
 *
 * \return
 *  \retval _SHR_E_NONE - success
 * \remark
 * SPECIAL INPUT:
 *        GLOBALS:
 *            Access_profiles - An array that contains all information
 *            about TCAM access profiles
 */
static shr_error_e
dnx_field_tcam_access_profile_algo_find_id(
    int unit,
    dnx_field_tcam_database_t * db,
    int *access_profile_id)
{
    int profile_id;
    uint8 occupied;

    SHR_FUNC_INIT_VARS(unit);

    for (profile_id = 0; profile_id < dnx_data_field.tcam.nof_access_profiles_get(unit); profile_id++)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.occupied.get(unit, profile_id, &occupied));
        if (!occupied)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE, "algo_find_id, found id: %d%s%s%s", profile_id, EMPTY, EMPTY, EMPTY);
            if (db->key_size == DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE)
            {
                /*
                 * For double keys two APs need to be allocated, the other AP is retrieved by calling PAIRED_ID_GET.
                 */
                int paired_profile_id = DNX_FIELD_TCAM_ACCESS_PROFILE_PAIRED_ID_GET(profile_id);
                if (paired_profile_id >= dnx_data_field.tcam.nof_access_profiles_get(unit))
                {
                    /*
                     * Pair doesn't exist, continue until for loop exit to reach error point
                     */
                    continue;
                }
                SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.
                                occupied.get(unit, paired_profile_id, &occupied));
                if (occupied)
                {
                    /*
                     * pair is in use, continue to next pair
                     */
                    continue;
                }
            }
            *access_profile_id = profile_id;
            break;
        }
    }

    /*
     * No access profile found
     */
    if (profile_id == dnx_data_field.tcam.nof_access_profiles_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_FULL, "Error allocating access profile: No access profile available");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_profile_prefix_write(
    int unit,
    uint32 handler_id,
    uint32 prefix_value)
{
    uint32 key_size;
    int access_profile_id;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.key_size.get(unit, handler_id, &key_size));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_TCAM_ACCESS_PROFILES, &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_id_get(unit, handler_id, &access_profile_id));

    if (key_size == dnx_data_field.tcam.key_size_double_get(unit))
    {
        /*
         * For Double-key FGs, MSB part (where prefix resides) is paired access profile 
         */
        access_profile_id = DNX_FIELD_TCAM_ACCESS_PROFILE_PAIRED_ID_GET(access_profile_id);
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_ACCESS_PROFILE_ID, access_profile_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PREFIX, INST_SINGLE, prefix_value);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Allocates the given access profile's id for the given db.
 *
 * \param [in] unit               - Device ID
 * \param [in] handler_id         - The handler ID to allocate the access profile for
 * \param [in] db                 - The database to allocate the access profile for
 * \param [out] access_profile_id - The access profile ID to allocate
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_access_profile_alloc_id_aux(
    int unit,
    uint32 handler_id,
    dnx_field_tcam_database_t * db,
    int access_profile_id)
{
    dnx_field_tcam_access_profile_t access_profile;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.get(unit, access_profile_id, &access_profile));

    access_profile.key_size = db->key_size;
    access_profile.action_size = db->action_size;
    /*
     * Banks get allocated later
     */
    access_profile.bank_ids_bmp = 0;
    access_profile.is_direct = db->is_direct;
    access_profile.prefix = db->prefix_value;
    access_profile.handler_id = handler_id;
    access_profile.occupied = TRUE;

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.set(unit, access_profile_id, &access_profile));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Allocates the given access profile's id for the given db.
 *  For double key DBs, it also allocates the paired access profile id.
 *
 * \param [in] unit               - Device ID
 * \param [in] handler_id         - The handler ID to allocate the access profile for
 * \param [in] db                 - The database to allocate the access profile for
 * \param [out] access_profile_id - The access profile ID to allocate
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_access_profile_alloc_id(
    int unit,
    uint32 handler_id,
    dnx_field_tcam_database_t * db,
    int access_profile_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_alloc_id_aux(unit, handler_id, db, access_profile_id));
    if (db->key_size == DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE)
    {
        /*
         * For double keys two APs need to be allocated, the other AP is retrieved by calling PAIRED_ID_GET.
         */
        int paired_profile_id = DNX_FIELD_TCAM_ACCESS_PROFILE_PAIRED_ID_GET(access_profile_id);
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_alloc_id_aux(unit, handler_id, db, paired_profile_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Converts key size to access profile key mode
 *
 * \param [in] unit      - Device ID
 * \param [in] key_size  - Size of key to convert
 * \param [out] key_mode - The converted access profile mode
 *
 * \return
 *  \retval _SHR_E_NONE  - success
 *  \retval _SHR_E_PARAM - Invalid key size
 */
static shr_error_e
dnx_field_tcam_access_profile_convert_key_to_mode(
    int unit,
    dnx_field_key_length_type_e key_size,
    int *key_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (key_size)
    {
        case DNX_FIELD_KEY_LENGTH_TYPE_DT:
        case DNX_FIELD_KEY_LENGTH_TYPE_HALF:
            *key_mode = dnx_data_field.tcam.access_profile_half_key_mode_get(unit);
            break;
        case DNX_FIELD_KEY_LENGTH_TYPE_SINGLE:
            *key_mode = dnx_data_field.tcam.access_profile_single_key_mode_get(unit);
            break;
        case DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE:
            *key_mode = dnx_data_field.tcam.access_profile_double_key_mode_get(unit);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid key_size %d", key_size);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Writes the information for the db associated with the given access_profile_id to HW.
 *
 * \param [in] unit              - Device ID
 * \param [in] db                - The database to write its information in the access profile memory
 * \param [in] access_profile_id - The index in the access profile memory to write the data to
 *
 * \remark
 * SPECIAL OUTPUT:
 *        HW:
 *            TCAM_ACCESS_PROFILE - Adds bank to the given db's access profile
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_access_profile_hw_set(
    int unit,
    dnx_field_tcam_database_t * db,
    int access_profile_id)
{
    uint32 entry_handle_id;
    int key_mode = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_convert_key_to_mode(unit, db->key_size, &key_mode));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_TCAM_ACCESS_PROFILES, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_ACCESS_PROFILE_ID, access_profile_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KEY_SIZE, INST_SINGLE, key_mode);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_DIRECT, INST_SINGLE, db->is_direct);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Clear access profile entry
 *
 * \param [in] unit              - Device ID
 * \param [in] access_profile_id - The access profile ID to clear
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_access_profile_hw_clear(
    int unit,
    int access_profile_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_TCAM_ACCESS_PROFILES_BANKS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_ACCESS_PROFILE_ID, access_profile_id);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FIELD_TCAM_BANK_ID, 0,
                                     dnx_data_field.tcam.nof_banks_get(unit) - 1);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Writes the information for the db associated with the given access_profile_id to HW.
 *
 * \param [in] unit              - Device ID
 * \param [in] db                - The database to write its information in the access profile memory
 * \param [in] access_profile_id - The index in the access profile memory to write the data to
 *
 * \remark
 * SPECIAL OUTPUT:
 *        HW:
 *            TCAM_ACCESS_PROFILE - Adds bank to the given db's access profile
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_access_profile_write_dbal(
    int unit,
    dnx_field_tcam_database_t * db,
    int access_profile_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_hw_set(unit, db, access_profile_id));

    if (db->key_size == DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE)
    {
        /*
         * For double keys two APs need to be allocated, the other AP is retrieved by calling PAIRED_ID_GET.
         */
        int paired_profile_id = DNX_FIELD_TCAM_ACCESS_PROFILE_PAIRED_ID_GET(access_profile_id);
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_hw_set(unit, db, paired_profile_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Access profile allocation verifier.
 *  Verifies that the requested access profile id is not already occupied by another handler.
 *
 * \param [in] unit              - Device ID
 * \param [in] handler_id        - The handler ID we want to allocate access profile ID for
 * \param [in] db                - Database to allocate access profile for
 * \param [in] access_profile_id - The access profile ID the user wants to allocate
 *
 * \return
 *  \retval _SHR_E_NONE  - Success
 *  \retval _SHR_E_PARAM - Access profile(s) id requested already occupied by another handler
 */
static shr_error_e
dnx_field_tcam_access_profile_alloc_verify(
    int unit,
    uint32 handler_id,
    dnx_field_tcam_database_t * db,
    int access_profile_id)
{
    uint8 occupied;

    SHR_FUNC_INIT_VARS(unit);

    /** Automatic allocation doesn't require pre-validation */
    if (access_profile_id != FIELD_TCAM_ACCESS_PROFILE_ID_AUTO)
    {
        if (access_profile_id >= dnx_data_field.tcam.nof_access_profiles_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Requested access profile id (%d) for handler (%d) is out of range (%d)\r\n",
                         access_profile_id, handler_id, dnx_data_field.tcam.nof_access_profiles_get(unit));
        }
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.
                        occupied.get(unit, access_profile_id, &occupied));
        if (occupied)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Access profile id requested (%d) for handler (%d) already in use",
                         access_profile_id, handler_id);
        }

        if (db->key_size == DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE)
        {
            /*
             * For double keys two APs need to be allocated, the other AP is retrieved by calling PAIRED_ID_GET.
             */
            int paired_profile_id = DNX_FIELD_TCAM_ACCESS_PROFILE_PAIRED_ID_GET(access_profile_id);
            if (paired_profile_id >= dnx_data_field.tcam.nof_access_profiles_get(unit))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Requested access profile id (%d) for double key mode handler (%d) is out of range (%d)\r\n",
                             access_profile_id, handler_id, dnx_data_field.tcam.nof_access_profiles_get(unit));
            }
            SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.
                            occupied.get(unit, paired_profile_id, &occupied));
            if (occupied)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Paired access profile id (%d) for the access profile id requested (%d) for double key mode handler (%d) already in use",
                             paired_profile_id, access_profile_id, handler_id);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_profile_alloc(
    int unit,
    uint32 handler_id,
    dnx_field_tcam_database_t * db,
    int access_profile_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_tcam_access_profile_alloc_verify(unit, handler_id, db, access_profile_id));

    if (access_profile_id == FIELD_TCAM_ACCESS_PROFILE_ID_AUTO)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_algo_find_id(unit, db, &access_profile_id));
    }

    /*
     * Allocate the access profile in SWSTATE
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_alloc_id(unit, handler_id, db, access_profile_id));

    /*
     * Write newly allocated access profile to HW
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_write_dbal(unit, db, access_profile_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Updates the access profile with the given access_profile_id with the given
 *  action_size for the given bank_id in HW.
 *  action_size determines what portion of the TCAM BANK ACTION to use. It's also
 *  used to indicate whether to use the bank for search or not.
 *  if action_size is 0, the access profile will stop using the specified bank
 *  for any other non-zero action_size, the access profile will start using the
 *  specified bank.
 *
 * \param [in] unit              - Device ID
 * \param [in] access_profile_id - The access profile ID in HW
 * \param [in] bank_id           - The TCAM bank ID
 * \param [in] fg_params         - The fg params to determine what portion of the
 *                                 TCAM BANK ACTION to use
 * \param [in] remove            - Whether to remove or add this bank to the given AP
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_access_profile_bank_update_dbal(
    int unit,
    int access_profile_id,
    int bank_id,
    dnx_field_tcam_access_fg_params_t * fg_params,
    uint8 remove)
{
    uint32 entry_handle_id;
    dnx_field_tcam_access_profile_action_bmp_e action_bmp;
    int profile_to_update;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fg_params, _SHR_E_PARAM, "fg_params");

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_TCAM_ACCESS_PROFILES_BANKS, &entry_handle_id));

    if (fg_params->key_size == dnx_data_field.tcam.key_size_double_get(unit))
    {
        /*
         * In double key mode, we set even bank in the base access profile and odd banks
         * in the paired access profile.
         */
        profile_to_update = ((bank_id % 2) == 0) ? access_profile_id :
            DNX_FIELD_TCAM_ACCESS_PROFILE_PAIRED_ID_GET(access_profile_id);
    }
    else
    {
        profile_to_update = access_profile_id;
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_ACCESS_PROFILE_ID, profile_to_update);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_TCAM_BANK_ID, bank_id);

    if (remove)
    {
        action_bmp = DNX_FIELD_TCAM_ACCESS_PROFILE_ACTION_BMP_REMOVE;
    }
    else
    {
        /*
         * ACTION_BITMAP is split as following:
         * 1) For DT FG: use MSB part of the payload
         * 2) For Special FG (single_key/half_action) use LSB part of the payload
         * 3) For everything else we use all payload (for half_key_size it's a must)
         */
        if (fg_params->direct_table)
        {
            action_bmp = DNX_FIELD_TCAM_ACCESS_PROFILE_ACTION_BMP_MSB;
        }
        else if (fg_params->key_size == dnx_data_field.tcam.key_size_single_get(unit) &&
                 fg_params->action_size == dnx_data_field.tcam.action_size_half_get(unit))
        {
            action_bmp = DNX_FIELD_TCAM_ACCESS_PROFILE_ACTION_BMP_LSB;
        }
        else
        {
            action_bmp = DNX_FIELD_TCAM_ACCESS_PROFILE_ACTION_BMP_ALL;
        }
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTION_SELECT, INST_SINGLE, action_bmp);
    /*
     * !! keeps 0 as 0, and turns every other value into 1
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_SELECT, INST_SINGLE, !!action_bmp);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_profile_bank_add(
    int unit,
    uint32 handler_id,
    int bank_id)
{
    int access_profile_id;
    uint16 bank_ids_bmp;
    dnx_field_tcam_access_fg_params_t fg_params;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_id_get(unit, handler_id, &access_profile_id));

    LOG_DEBUG_EX(BSL_LOG_MODULE, "Adding TCAM bank id: %d to handler: %d%s%s", bank_id, handler_id, EMPTY, EMPTY);

    /*
     * Add bank to SW State
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.
                    bank_ids_bmp.get(unit, access_profile_id, &bank_ids_bmp));

    bank_ids_bmp |= SAL_BIT(bank_id);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.
                    bank_ids_bmp.set(unit, access_profile_id, bank_ids_bmp));

    /*
     * Add bank to HW
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_bank_update_dbal
                    (unit, access_profile_id, bank_id, &fg_params, FALSE));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_profile_bank_remove(
    int unit,
    uint32 handler_id,
    int bank_id)
{
    int access_profile_id;
    uint16 bank_ids_bmp;
    dnx_field_tcam_access_fg_params_t fg_params;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_id_get(unit, handler_id, &access_profile_id));

    LOG_DEBUG_EX(BSL_LOG_MODULE, "Removing bank id: %d from handler: %d%s%s", bank_id, handler_id, EMPTY, EMPTY);

    /*
     * Remove bank from SWSTATE
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.
                    bank_ids_bmp.get(unit, access_profile_id, &bank_ids_bmp));

    bank_ids_bmp &= SAL_RBIT(bank_id);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.
                    bank_ids_bmp.set(unit, access_profile_id, bank_ids_bmp));

    /*
     * Remove bank from HW
     */
    /*
     * Setting action select value to 0 will make the access profile stop using the specified bank_id
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_bank_update_dbal(unit, access_profile_id, bank_id, &fg_params, TRUE));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_profile_free(
    int unit,
    uint32 handler_id)
{
    int access_profile_id;
    dnx_field_tcam_access_fg_params_t fg_params;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_id_get(unit, handler_id, &access_profile_id));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));

    /*
     * Set occupied to FALSE
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.occupied.set(unit, access_profile_id, FALSE));
    /*
     * Clear entry in HW
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_hw_clear(unit, access_profile_id));

    if (fg_params.key_size == dnx_data_field.tcam.key_size_double_get(unit))
    {
        int paired_profile_id = DNX_FIELD_TCAM_ACCESS_PROFILE_PAIRED_ID_GET(access_profile_id);
        /*
         * Sanity check
         */
        if (paired_profile_id >= dnx_data_field.tcam.nof_access_profiles_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Invalid state: double key-sized handler %d can't have access profile %d allocated (max number of access profiles: %d)\r\n",
                         handler_id, paired_profile_id, dnx_data_field.tcam.nof_access_profiles_get(unit));
        }
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.occupied.set(unit, paired_profile_id, FALSE));

        SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_hw_clear(unit, paired_profile_id));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_profile_banks_bmp_get(
    int unit,
    uint32 handler_id,
    uint32 *banks_bmp)
{
    uint16 banks_bmp_sw;
    int access_profile_id;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_id_get(unit, handler_id, &access_profile_id));

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.
                    bank_ids_bmp.get(unit, access_profile_id, &banks_bmp_sw));
    (*banks_bmp) = banks_bmp_sw;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_profile_banks_sorted_get(
    int unit,
    uint32 handler_id,
    int *bank_ids,
    int *bank_count)
{
    int access_profile_id;
    uint16 bank_ids_bmp;
    int bank_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_id_get(unit, handler_id, &access_profile_id));

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.
                    bank_ids_bmp.get(unit, access_profile_id, &bank_ids_bmp));

    *bank_count = 0;
    for (bank_id = 0; bank_id < dnx_data_field.tcam.nof_banks_get(unit); bank_id++)
    {
        if (bank_ids_bmp & SAL_BIT(bank_id))
        {
            bank_ids[*bank_count] = bank_id;
            *bank_count = *bank_count + 1;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_profile_id_get(
    int unit,
    uint32 handler_id,
    int *access_profile_id)
{
    int profile_id;
    dnx_field_tcam_access_profile_t access_profile;

    SHR_FUNC_INIT_VARS(unit);

    for (profile_id = 0; profile_id < dnx_data_field.tcam.nof_access_profiles_get(unit); profile_id++)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.get(unit, profile_id, &access_profile));
        if (access_profile.occupied == TRUE && access_profile.handler_id == handler_id)
        {
            *access_profile_id = profile_id;
            break;
        }
    }

    /*
     * No access profile found
     */
    if (profile_id == dnx_data_field.tcam.nof_access_profiles_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                     "Error retrieving access profile id for handler id: %d, no access profile allocated for given handler",
                     handler_id);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_profile_intersect(
    int unit,
    uint32 handler_id1,
    uint32 handler_id2,
    uint8 *intersect)
{
    int *bank_ids1_p = NULL;
    int banks_count1;
    int *bank_ids2_p = NULL;
    int banks_count2;
    int ii = 0;
    int jj = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(bank_ids1_p, sizeof(*bank_ids1_p) * dnx_data_field.tcam.nof_banks_get(unit), "bank_ids1_p", "%s%s%s",
              EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(bank_ids2_p, sizeof(*bank_ids2_p) * dnx_data_field.tcam.nof_banks_get(unit), "bank_ids2_p", "%s%s%s",
              EMPTY, EMPTY, EMPTY);

    *intersect = FALSE;
    if (handler_id1 == handler_id2)
    {
        /*
         * By definition handler can't intersect with itself.
         * We return with intersect = FALSE.
         */
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_banks_sorted_get(unit, handler_id1, bank_ids1_p, &banks_count1));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_banks_sorted_get(unit, handler_id2, bank_ids2_p, &banks_count2));

    while (ii < banks_count1 && jj < banks_count2)
    {
        if (bank_ids1_p[ii] == bank_ids2_p[jj])
        {
            *intersect = TRUE;
            break;
        }
        /*
         * Bank_ids are sorted, thus, moving the index
         * of the lower value is all we need.
         */
        if (bank_ids1_p[ii] < bank_ids2_p[jj])
        {
            ii++;
        }
        else
        {
            jj++;
        }
    }

exit:
    SHR_FREE(bank_ids1_p);
    SHR_FREE(bank_ids2_p);
    SHR_FUNC_EXIT;
}
