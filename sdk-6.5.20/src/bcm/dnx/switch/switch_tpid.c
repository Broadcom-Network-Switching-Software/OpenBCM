/**
 * \file switch_tpid.c
 *
 * TPID - Tag Protocol Identifier.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SWITCH_TPID

/*
 * Include files.
 * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm/switch.h>
#include <include/bcm_int/dnx/stk/stk_sys.h>
#include <bcm_int/dnx/switch/switch_tpid.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

/*
 * }
 */

/**
 * \brief
 * Verify function for BCM-API: bcm_dnx_switch_tpid_add().
 */
static shr_error_e
dnx_switch_tpid_add_verify(
    int unit,
    uint32 options,
    bcm_switch_tpid_info_t * tpid_info)
{
    int is_traffic_enabled = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(tpid_info, _SHR_E_PARAM, "tpid_info");

    /*
     * Verify that the API is called before traffic is enabled
     */
    SHR_IF_ERR_EXIT(dnx_stk_sys_traffic_enable_get(unit, &is_traffic_enabled));
    if (is_traffic_enabled != 0)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Error, The API must be called before traffic is enabled\n");
    }

    /*
     * Verify that TPID value is valid:
     */
    if (BCM_SWITCH_TPID_VALUE_INVALID == tpid_info->tpid_value)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "tpid_value 0x%x is not valid!!!\n", BCM_SWITCH_TPID_VALUE_INVALID);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify function for BCM-API: bcm_dnx_switch_tpid_delete().
 */
static shr_error_e
dnx_switch_tpid_delete_verify(
    int unit,
    bcm_switch_tpid_info_t * tpid_info)
{
    int is_traffic_enabled;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(tpid_info, _SHR_E_PARAM, "tpid_info");

    /*
     * Verify that the API is called before traffic is enabled
     */
    SHR_IF_ERR_EXIT(dnx_stk_sys_traffic_enable_get(unit, &is_traffic_enabled));
    if (is_traffic_enabled == TRUE)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Error, The API must be called before traffic is enabled\n");
    }

    /*
     * Verify that TPID value is valid:
     */
    if (BCM_SWITCH_TPID_VALUE_INVALID == tpid_info->tpid_value)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "tpid_value 0x%x is not valid thus cannot be deleted!!!\n",
                     BCM_SWITCH_TPID_VALUE_INVALID);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify function for BCM-API: bcm_dnx_switch_tpid_delete_all().
 */
static shr_error_e
dnx_switch_tpid_delete_all_verify(
    int unit)
{
    int is_traffic_enabled;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that the API is called before traffic is enabled
     */
    SHR_IF_ERR_EXIT(dnx_stk_sys_traffic_enable_get(unit, &is_traffic_enabled));
    if (is_traffic_enabled == TRUE)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Error, The API must be called before traffic is enabled\n");
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify function for BCM-API: bcm_dnx_switch_tpid_get_all().
 */
static shr_error_e
dnx_switch_tpid_get_all_verify(
    int unit,
    int size,
    bcm_switch_tpid_info_t * tpid_info_array,
    int *count)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(tpid_info_array, _SHR_E_PARAM, "tpid_info_array");

    SHR_NULL_CHECK(count, _SHR_E_PARAM, "count");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Set a TPID value at the TPIDs virtual register
 *
 * \param [in] unit -relevant unit.
 * \param [in] tpid_index -designated tpid index. 
 * \param [in] tpid_value - TPID value.
 * \remark 
 * Since the TPID virtual register are not organised in table with key index,
 * the tpid_index input is used as an enum to indicate which TPID to set:
 * tpid_index 0 ==> TPID_1
 * tpid_index 1 ==> TPID_2
 * tpid_index 2 ==> TPID_3
 * tpid_index 3 ==> TPID_4
 * tpid_index 4 ==> TPID_5
 * tpid_index 5 ==> TPID_6
 * tpid_index 6 ==> TPID_7
 */
static shr_error_e
dnx_switch_tpid_virtual_reg_value_set(
    int unit,
    int tpid_index,
    uint16 tpid_value)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_PARSERETH, &entry_handle_id));

    switch (tpid_index)
    {
        case 0:
        {
            dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_TPID_1, INST_SINGLE, tpid_value);
            break;
        }
        case 1:
        {
            dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_TPID_2, INST_SINGLE, tpid_value);
            break;
        }
        case 2:
        {
            dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_TPID_3, INST_SINGLE, tpid_value);
            break;
        }
        case 3:
        {
            dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_TPID_4, INST_SINGLE, tpid_value);
            break;
        }
        case 4:
        {
            dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_TPID_5, INST_SINGLE, tpid_value);
            break;
        }
        case 5:
        {
            dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_TPID_6, INST_SINGLE, tpid_value);
            break;
        }
        case 6:
        {
            dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_TPID_7, INST_SINGLE, tpid_value);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "tpid_index %d is not valid!!! the range is [0:%d]\n", tpid_index,
                         (BCM_DNX_SWITCH_TPID_NUM_OF_VALID_GLOBALS - 1));
        }
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get the TPID value of a designated index in the parser's TPIDs table.
 *
 * \param [in] unit -relevant unit.
 * \param [in] tpid_index -designated index to the table. 
 * \param [out] tpid_value -pointer to TPID value.
 *
 * \return
 *   shr_error_e \n
 *      BCM_E_NONE - Success. \n
 *      BCM_E_XXX - Fails.
 *
 * \remark 
 * This function is not a BCM API!
 * It is implemented for SDK internal usage.
 */
shr_error_e
dnx_switch_tpid_value_get(
    int unit,
    int tpid_index,
    uint16 *tpid_value)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(tpid_value, _SHR_E_PARAM, "tpid_value");

    if ((tpid_index < 0) || (tpid_index >= BCM_DNX_SWITCH_TPID_NUM_OF_VALID_GLOBALS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "tpid_index %d is not valid!!!\n", tpid_index);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_TPID, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TPID_INDEX, tpid_index);
    dbal_value_field16_request(unit, entry_handle_id, DBAL_FIELD_TPID_VALUE, INST_SINGLE, tpid_value);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Set a TPID value at a designated index in the parser's TPIDs table. 
 *
 * \param [in] unit -relevant unit.
 * \param [in] tpid_index -designated index to the table. 
 * \param [in] tpid_value -TPID value.
 *
 * \return
 *   shr_error_e \n
 *      BCM_E_NONE - Success. \n
 *      BCM_E_XXX - Fails.
 *
 * \remark 
 * This function is not a BCM API!
 * It is implemented for SDK internal usage.
 */
shr_error_e
dnx_switch_tpid_value_set(
    int unit,
    int tpid_index,
    uint16 tpid_value)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if ((tpid_index < 0) || (tpid_index >= BCM_DNX_SWITCH_TPID_NUM_OF_VALID_GLOBALS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "tpid_index %d is not valid!!!\n", tpid_index);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_TPID, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TPID_INDEX, tpid_index);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_TPID_VALUE, INST_SINGLE, tpid_value);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Update TPID virtual registers
     */
    SHR_IF_ERR_EXIT(dnx_switch_tpid_virtual_reg_value_set(unit, tpid_index, tpid_value));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get the index of a TPID value in the parser's TPIDs table.
 *
 * \param [in] unit -relevant unit.
 * \param [in] tpid_value -TPID value.
 * \param [out] tpid_index -pointer to designated index at the table. 
 *
 * \return
 *   shr_error_e \n
 *      BCM_E_NONE - Success. \n
 *      BCM_E_XXX - Fails.
 *
 * \remark 
 * If TPID value is not found, the tpid_index is updated with 
 * BCM_DNX_SWITCH_TPID_INDEX_INVALID.
 * This function is not a BCM API!
 * It is implemented for SDK internal usage.
 */
shr_error_e
dnx_switch_tpid_index_get(
    int unit,
    uint16 tpid_value,
    int *tpid_index)
{
    uint32 entry_handle_id;
    int tpid_tmp_index = 0;
    uint16 tpid_cur_value;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(tpid_index, _SHR_E_PARAM, "tpid_index");

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_TPID, &entry_handle_id));

    *tpid_index = BCM_DNX_SWITCH_TPID_INDEX_INVALID;

    do
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TPID_INDEX, tpid_tmp_index);
        dbal_value_field16_request(unit, entry_handle_id, DBAL_FIELD_TPID_VALUE, INST_SINGLE, &tpid_cur_value);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        if (tpid_cur_value == tpid_value)
        {
            /*
             * Match found! 
             */
            *tpid_index = tpid_tmp_index;
        }

        tpid_tmp_index++;
    }
    while ((tpid_tmp_index < BCM_DNX_SWITCH_TPID_NUM_OF_VALID_GLOBALS)
           && (*tpid_index == BCM_DNX_SWITCH_TPID_INDEX_INVALID));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * BCM API: Adding a TPID value to the global TPID's table.
 *
 * \param [in] unit -relevant unit.
 * \param [in] options -TPID options - not supported at this stage. 
 * \param [in] tpid_info -TPID information - only tpid_value field is used.
 *
 * \return
 *   shr_error_e \n
 *      BCM_E_NONE - Success. \n
 *      BCM_E_XXX - Fails.
 *
 * \remark 
 *  The options field is not supported at this stage.
 *  The tpid_info structure:
 *      - Only the tpid_value field will be supported for now.
 *      - The SDK will disallow adding the same TPID more than
 *          once.
 *      - An error is issued in case an addition is attempted
 *        while all 7 Global TPIDs are occupied (TPID 7 is
 *        reserved as NULL)
 *  It is required to configure the TPIDs on the initalization time.
 */
shr_error_e
bcm_dnx_switch_tpid_add(
    int unit,
    uint32 options,
    bcm_switch_tpid_info_t * tpid_info)
{
    uint32 entry_handle_id;
    uint16 tpid_cur_values[BCM_DNX_SWITCH_TPID_NUM_OF_VALID_GLOBALS];
    int first_vacant_tpid_entry = BCM_DNX_SWITCH_TPID_INDEX_INVALID;
    int tpid_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_switch_tpid_add_verify(unit, options, tpid_info));

    /*
     * Up to 7 different TPIDs can be set. 
     * Adding same TPID more than once is disallowed. 
     * Thus:
     *     - read all TPIDs values from TPID table.
     *     - check that the new TPID is not already exist.
     *     - update new TPID if there is a vacant entry.
     */

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_TPID, &entry_handle_id));

    /*
     * Read all TPIDs values from TPID table.
     * Check that the new TPID is not already exist. 
     * File the first vacant entry.
     */

    for (tpid_index = 0; tpid_index < BCM_DNX_SWITCH_TPID_NUM_OF_VALID_GLOBALS; tpid_index++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TPID_INDEX, tpid_index);
        dbal_value_field16_request(unit, entry_handle_id, DBAL_FIELD_TPID_VALUE, INST_SINGLE,
                                   &tpid_cur_values[tpid_index]);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        if (tpid_cur_values[tpid_index] == tpid_info->tpid_value)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "tpid_value 0x%x already exist in TPID table (at entry %d)!!!\n",
                         tpid_info->tpid_value, tpid_index);
        }

        if (BCM_SWITCH_TPID_VALUE_INVALID == tpid_cur_values[tpid_index])
        {
            if (BCM_DNX_SWITCH_TPID_INDEX_INVALID == first_vacant_tpid_entry)
            {
                first_vacant_tpid_entry = tpid_index;
            }
        }
    }

    /*
     * At this point, 'first_vacant_tpid_entry' contains the first vacant index or BCM_DNX_SWITCH_TPID_INDEX_INVALID if there is none. 
     * Update the TPID table with the new TPID if there is a vacant entry.
     */
    if (BCM_DNX_SWITCH_TPID_INDEX_INVALID == first_vacant_tpid_entry)
    {
        for (tpid_index = 0; tpid_index < BCM_DNX_SWITCH_TPID_NUM_OF_VALID_GLOBALS; tpid_index++)
        {
            LOG_INFO_EX(BSL_LOG_MODULE, "tpid_index %d, tpid_value = 0x%04X %s %s\n", tpid_index,
                        tpid_cur_values[tpid_index], EMPTY, EMPTY);
        }
        SHR_ERR_EXIT(_SHR_E_FULL, "TPID table is full, cannot update with new value!!!\n");
    }

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TPID_INDEX, first_vacant_tpid_entry);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_TPID_VALUE, INST_SINGLE, tpid_info->tpid_value);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Update TPID virtual registers
     */
    SHR_IF_ERR_EXIT(dnx_switch_tpid_virtual_reg_value_set(unit, first_vacant_tpid_entry, tpid_info->tpid_value));

exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * BCM_API:Delete a configured TPID from the parser TPID's table.
 *
 * \param [in] unit - relevant unit.
 * \param [in] tpid_info -TPID information - only tpid_value field is used.
 *
 * \return
 *   shr_error_e \n
 *      BCM_E_NONE - Success. \n
 *      BCM_E_XXX - Fails.
 *
 *  \remark 
 *  An error is returned if the TPID value isn't found.
 *  The deleted entry is filled with the invalid TPID value, i.e
 *  0x0000 (BCM_SWITCH_TPID_VALUE_INVALID).
 * See the Programming Guide PP document for full details.
 */
shr_error_e
bcm_dnx_switch_tpid_delete(
    int unit,
    bcm_switch_tpid_info_t * tpid_info)
{
    uint32 entry_handle_id;
    int tpid_index = 0;
    uint16 tpid_value;
    int tpid_found = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_switch_tpid_delete_verify(unit, tpid_info));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_TPID, &entry_handle_id));

    do
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TPID_INDEX, tpid_index);
        dbal_value_field16_request(unit, entry_handle_id, DBAL_FIELD_TPID_VALUE, INST_SINGLE, &tpid_value);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        if (tpid_info->tpid_value == tpid_value)
        {
            tpid_found = TRUE;

            /*
             * Invalidate the matched entry in the TPID table
             */

            dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_TPID_VALUE, INST_SINGLE,
                                         BCM_SWITCH_TPID_VALUE_INVALID);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }

        tpid_index++;
    }
    while ((tpid_index < BCM_DNX_SWITCH_TPID_NUM_OF_VALID_GLOBALS) && (tpid_found == FALSE));

    if (FALSE == tpid_found)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "TPID value 0x%x was not found in TPID table\n", tpid_info->tpid_value);
    }

    /*
     * Update TPID virtual registers
     */
    SHR_IF_ERR_EXIT(dnx_switch_tpid_virtual_reg_value_set(unit, (tpid_index - 1), BCM_SWITCH_TPID_VALUE_INVALID));

exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * BCM_API: Delete all configured TPIDs from parser TPIDs table.
 *
 * \param [in] unit -relevant unit.
 *
 * \return
 *   shr_error_e \n
 *      BCM_E_NONE - Success. \n
 *      BCM_E_XXX - Fails.
 *
 *  \remark 
 *  The deleted entries are filled with the invalid TPID value,
 *  i.e 0x0000 (BCM_SWITCH_TPID_VALUE_INVALID).
 * See the Programming Guide PP document for full details.
 */
shr_error_e
bcm_dnx_switch_tpid_delete_all(
    int unit)
{
    int tpid_index;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_switch_tpid_delete_all_verify(unit));

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_GLOBAL_TPID));

    /*
     * Update TPID virtual registers
     */
    for (tpid_index = 0; tpid_index < BCM_DNX_SWITCH_TPID_NUM_OF_VALID_GLOBALS; tpid_index++)
    {
        SHR_IF_ERR_EXIT(dnx_switch_tpid_virtual_reg_value_set(unit, tpid_index, BCM_SWITCH_TPID_VALUE_INVALID));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * BCM_API: Returns all the configured TPID values at the parser TPID's table.
 *
 * \param [in] unit -relevant unit.
 * \param [in] size -the size field isn't used.
 * \param [out] tpid_info_array -pointer to array of bcm_switch_tpid_info_t.
 * Only tpid_info_array[i].tpid_value is used.
 * \param [out] count - the total numbers of valid TPIDs.
 *
 * \return
 *   shr_error_e \n
 *      BCM_E_NONE - Success. \n
 *      BCM_E_XXX - Fails.
 *
 *  \remark 
 *  The size field isn't used.
 *  The tpid_info_array field is filled with only valid TPIDs
 *  value, i.e TPID != 0x0000
 *  (BCM_SWITCH_TPID_VALUE_INVALID).
 *  The count field returns total number of valid TPID values.
 * See the Programming Guide PP document for full details.
 */
int
bcm_dnx_switch_tpid_get_all(
    int unit,
    int size,
    bcm_switch_tpid_info_t * tpid_info_array,
    int *count)
{
    uint32 entry_handle_id;
    int tpid_index = 0;
    uint16 tpid_value;
    int tpid_count_valid = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_switch_tpid_get_all_verify(unit, size, tpid_info_array, count));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_TPID, &entry_handle_id));

    /*
     * Read all TPIDs values from TPID table.
     */

    do
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TPID_INDEX, tpid_index);
        dbal_value_field16_request(unit, entry_handle_id, DBAL_FIELD_TPID_VALUE, INST_SINGLE, &tpid_value);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        if (BCM_SWITCH_TPID_VALUE_INVALID != tpid_value)
        {
            tpid_info_array[tpid_count_valid].tpid_value = tpid_value;
            tpid_count_valid++;
        }
        tpid_index++;

    }
    while (tpid_index < BCM_DNX_SWITCH_TPID_NUM_OF_VALID_GLOBALS);

    *count = tpid_count_valid;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
