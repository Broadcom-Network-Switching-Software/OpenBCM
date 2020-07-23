/**
 * \file       tcam_bank_manager.c
 * This file implements all the functions for the bank manager of the TCAM.
 * It defines one global variable "Banks" to hold information about all the
 * banks in the TCAM.
 */
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm_int/dnx/field/tcam/tcam_bank_manager.h>
#include <bcm_int/dnx/field/tcam/tcam_bank_algo.h>
#include <bcm_int/dnx/field/tcam/tcam_location_algo.h>
#include <bcm_int/dnx/field/tcam/tcam_access_profile_manager.h>

#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_bank_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_access.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_location_types.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

#define DNX_FIELD_TCAM_BANK_ENTRY_FREED 0
#define DNX_FIELD_TCAM_BANK_ENTRY_ALLOCATED 1

/**
 * \brief
 *  Verifies that the given bank_id doesn't exceed the limit and that the given handler_id can be added to the given
 *  bank_id. (Important note: this function does not check the next bank for handlers with double-sized key, since
 *  handlers with double-sized key use a pair of banks, this function needs to be invoked twice, once for each bank in
 *  the pair in order to check if the handler with double-sized key can be added to a given pair of banks).
 *
 * \param [in] unit       - Device ID
 * \param [in] bank_id    - The bank ID to be checked
 * \param [in] handler_id - The handler ID to be checked if it's suitable to be added to the given bank
 * \param [in] fg_params  - the "field group" parameters for the given handler (key_size, stage, etc...)
 *
 * \return
 *  \retval _SHR_E_NONE  - Success
 *  \retval _SHR_E_PARAM - 1) Given bank_id exceeds limit
 *                         2) Bank stage different from handler stage
 *                         3) Given handler already in bank
 *                         4) Prefix of handler is already taken on bank
 */
static shr_error_e
dnx_field_tcam_bank_single_check(
    int unit,
    int bank_id,
    uint32 handler_id,
    dnx_field_tcam_access_fg_params_t * fg_params)
{
    dnx_field_tcam_stage_e bank_owner_stage;
    uint8 handler_in_bank;
    uint8 is_free;
    int bank_mode;

    SHR_FUNC_INIT_VARS(unit);

    if (bank_id >= dnx_data_field.tcam.nof_banks_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error allocating bank: Requested TCAM Bank ID %d exceeds maximum TCAM bank limit (15)", bank_id);
    }
    /*
     * Check if bank is allocated for a different stage 
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_algo_get_owner_stage(unit, bank_id, &bank_owner_stage));
    if (bank_owner_stage != DNX_FIELD_TCAM_STAGE_INVALID && bank_owner_stage != fg_params->stage)
    {
        
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error allocating bank: Requested bank %d is allocated for stage %d"
                     ", database stage is %d", bank_id, bank_owner_stage, fg_params->stage);
    }
    /*
     * Check if handler is already in bank 
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_algo_handler_in_bank(unit, bank_id, handler_id, &handler_in_bank));
    if (handler_in_bank)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error allocating bank: Bank %d is already allocated for handler %d", bank_id,
                     handler_id);
    }
    /*
     * Check whether the prefix for the handler is free on the given bank (only for non-direct)
     */
    if (!fg_params->direct_table)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_is_free(unit, bank_id, handler_id, &is_free));
        if (!is_free)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error allocating bank: Prefix of handler %d is already taken on bank %d",
                         handler_id, bank_id);
        }
    }
    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.bank_mode.get(unit, bank_id, &bank_mode));

    /*
     * Check all bad cases of non-suitable bank and allocation modes
     */
    if (fg_params->bank_allocation_mode != DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_AUTO
        && bank_mode == DNX_FIELD_TCAM_BANK_MODE_OPEN)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error allocating bank with mode 'OPEN' for a handler with bank allocation mode different than 'AUTO', bank ID: %d, handler ID: %d\r\n",
                     bank_id, handler_id);
    }
    if (fg_params->bank_allocation_mode != DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_SELECT &&
        (bank_mode == DNX_FIELD_TCAM_BANK_MODE_RESTRICTED || bank_mode == DNX_FIELD_TCAM_BANK_MODE_RESTRICTED_DT))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error allocating bank with mode 'RESTRICTED' for a handler with bank allocation mode different than 'SELECT', bank ID: %d, handler ID: %d\r\n",
                     bank_id, handler_id);
    }
    if (fg_params->bank_allocation_mode != DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_SELECT_WITH_LOCATION &&
        bank_mode == DNX_FIELD_TCAM_BANK_MODE_RESTRICTED_LOCATION)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error allocating bank with mode 'RESTRICTED_LOCATION' for a handler with bank allocation mode different than 'SELECT_WITH_LOCATION', bank ID: %d, handler ID: %d\r\n",
                     bank_id, handler_id);
    }

    /** DT/non-DT section */
    if (fg_params->direct_table && bank_mode != DNX_FIELD_TCAM_BANK_MODE_RESTRICTED_DT
        && bank_mode != DNX_FIELD_TCAM_BANK_MODE_NONE)
    {
        /*
         * DT handlers can't be added on restricted non-DT banks
         */
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error allocating bank: Direct Table field group (%d) can't allocate bank (%d) not intended for DT\r\n",
                     handler_id, bank_id);
    }
    else if (((fg_params->key_size != dnx_data_field.tcam.key_size_single_get(unit) ||
               fg_params->action_size != dnx_data_field.tcam.action_size_half_get(unit)) &&
              (bank_mode == DNX_FIELD_TCAM_BANK_MODE_RESTRICTED_DT) && !fg_params->direct_table))
    {
        /*
         * Regular (non-special) FG section.
         * Special FGs are the ones that have (single_key_size/half_action_size) combination
         * and are allowed to reside on same bank with DT
         */
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error allocating bank: Regular field group (%d) can't allocate bank (%d) with DT mode\r\n",
                     handler_id, bank_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verifies that the bank_prealloc information is correct.
 *
 * \param[in] unit               - Device ID
 * \param[in] handler_id         - The handler ID with which the given bank_prealloc data is associated
 * \param[in] nof_banks_to_alloc - Number of banks to allocate
 * \param[in] banks_to_alloc     - Array of bank IDs to allocate
 *
 * \return
 *  \retval _SHR_E_NONE  - Success
 *  \retval _SHR_E_PARAM - 1) Given bank_id exceeds limit
 *                         2) Bank stage different from handler stage
 *                         3) Given handler already in bank
 *                         4) Odd bank ID requested for handler with double-sized key
 * \remark
 * SPECIAL OUTPUT:
 *        SWSTATE:
 *           dnx_field_tcam_bank_sw - Holds information for all banks in device
 */
static shr_error_e
dnx_field_tcam_bank_alloc_verify(
    int unit,
    uint32 handler_id,
    int nof_banks_to_alloc,
    int banks_to_alloc[])
{
    int ii;
    dnx_field_tcam_access_fg_params_t fg_params;

    SHR_FUNC_INIT_VARS(unit);

    if (nof_banks_to_alloc > dnx_data_field.tcam.nof_banks_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Can't allocate more than %d banks. Number requested: %d\r\n",
                     dnx_data_field.tcam.nof_banks_get(unit), nof_banks_to_alloc);
    }

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));

    if (fg_params.direct_table && nof_banks_to_alloc != 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Direct Table TCAM can't have more than 1 bank. (handler: %d, number of banks requested: %d)\r\n",
                     handler_id, nof_banks_to_alloc);
    }

    for (ii = 0; ii < nof_banks_to_alloc; ii++)
    {
        int jj;
        int bank_id = banks_to_alloc[ii];
        if (fg_params.key_size == dnx_data_field.tcam.key_size_double_get(unit))
        {
            if (bank_id % 2)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Handler %d with double-sized key can only be added to even bank ids,"
                             " given bank id %d is not even\r\n", handler_id, bank_id);
            }
            /*
             * Handlers with double-sized key always use a pair of adjacent banks, therefore, we do an additional
             * check for the adjacent bank.
             */
            SHR_IF_ERR_EXIT(dnx_field_tcam_bank_single_check(unit, bank_id + 1, handler_id, &fg_params));
        }
        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_single_check(unit, bank_id, handler_id, &fg_params));
        for (jj = ii + 1; jj < nof_banks_to_alloc; jj++)
        {
            if (bank_id == banks_to_alloc[jj])
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error: Bank requested at index %d is equal to bank requested at index %d\r\n", ii, jj);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

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
shr_error_e
dnx_field_tcam_bank_add_handler_to_bank(
    int unit,
    int bank_id,
    uint32 handler_id)
{
    dnx_field_tcam_access_fg_params_t fg_params;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));
    /*
     * Add the handler id to the active_handlers_id linked list of this bank 
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.active_handlers_id.add_last(unit, bank_id, &handler_id));

    /*
     * Update bank params, in this section we re-set stage and mode, but we already made sure they are the same
     * for this handler, so this has no meaning after adding the first handler to the bank
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.owner_stage.set(unit, bank_id, fg_params.stage));
    if (fg_params.direct_table)
    {
        /** --- DT FGs --- */
        /*
         * Set DT mode for DT handlers 
         */
        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                        bank_mode.set(unit, bank_id, DNX_FIELD_TCAM_BANK_MODE_RESTRICTED_DT));
        /*
         * Update DT handler bank ID 
         */
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.dt_bank_id.set(unit, handler_id, bank_id));
    }
    else
    {
        /*
         * non-DT FGs 
         */
        switch (fg_params.bank_allocation_mode)
        {
            case DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_AUTO:
            {
                /*
                 * Set open mode for handler with 'AUTO' bank allocation mode
                 */
                SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                                bank_mode.set(unit, bank_id, DNX_FIELD_TCAM_BANK_MODE_OPEN));
                break;
            }
            case DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_SELECT:
            {
                int bank_mode;
                SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.bank_mode.get(unit, bank_id, &bank_mode));
                /*
                 * Set restricted mode for handlers with 'SELECT' bank allocation mode, Except for special handlers. Since
                 * special handler validation was done before, we only check that mode isn't DT before setting
                 */
                if (bank_mode != DNX_FIELD_TCAM_BANK_MODE_RESTRICTED_DT)
                {
                    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                                    bank_mode.set(unit, bank_id, DNX_FIELD_TCAM_BANK_MODE_RESTRICTED));
                }
                break;
            }
            case DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_SELECT_WITH_LOCATION:
            {
                /*
                 * Set RESTRICED_SELECT mode for handler with 'SELECT_WITH_LOCATION'' bank allocation mode
                 */
                SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                                bank_mode.set(unit, bank_id, DNX_FIELD_TCAM_BANK_MODE_RESTRICTED_LOCATION));
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "unrecognized bank allocation mode %d\n", fg_params.bank_allocation_mode);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_bank_is_handler_on_bank(
    int unit,
    int bank_id,
    uint32 handler_id,
    uint8 *found)
{
    sw_state_ll_node_t handler_node;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_algo_get_handler_node_in_bank(unit, bank_id, handler_id, &handler_node));

    *found = (handler_node != SW_STATE_LL_INVALID);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_bank_remove_handler_from_bank(
    int unit,
    int bank_id,
    uint32 handler_id)
{
    sw_state_ll_node_t handler_node;
    uint32 list_size;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_algo_get_handler_node_in_bank(unit, bank_id, handler_id, &handler_node));

    if (handler_node != SW_STATE_LL_INVALID)
    {
        /*
         * In order to delete handler from bank, we remove the handler node from active handlers list 
         */
        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.active_handlers_id.remove_node(unit, bank_id, handler_node));

        /*
         * Reset stage in case no more handlers are present
         */
        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.active_handlers_id.nof_elements(unit, bank_id, &list_size));
        if (list_size == 0)
        {
            /** This bank is empty, we reset stage and mode*/
            SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                            owner_stage.set(unit, bank_id, DNX_FIELD_TCAM_STAGE_INVALID));
            SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                            bank_mode.set(unit, bank_id, DNX_FIELD_TCAM_BANK_MODE_NONE));
        }
    }
    else
    {
        /*
         * handler is not present in the bank, return an error without printing anything
         */
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_bank_single_alloc(
    int unit,
    uint32 handler_id,
    uint32 bank_id)
{
    dnx_field_tcam_access_fg_params_t fg_params;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));

    /*
     * We allocate the bank by; 1) Adding the handler to bank's active_handlers_id list 2) Adding the bank to the
     * handler's access profile 
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_add_handler_to_bank(unit, bank_id, handler_id));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_bank_add(unit, handler_id, bank_id));
    /*
     * For double key sized FGs, we only allocate prefix on odd (msb) bank 
     */
    if (!fg_params.direct_table && fg_params.key_size != dnx_data_field.tcam.key_size_double_get(unit))
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_bank_alloc(unit, bank_id, handler_id));
    }

    if (fg_params.key_size == dnx_data_field.tcam.key_size_double_get(unit))
    {
        /*
         * Add the adjacent bank for handlers with double-sized key 
         */
        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_add_handler_to_bank(unit, bank_id + 1, handler_id));
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_bank_add(unit, handler_id, bank_id + 1));
        SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_bank_alloc(unit, bank_id + 1, handler_id));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_bank_alloc(
    int unit,
    uint32 handler_id,
    int nof_banks_to_alloc,
    int banks_to_alloc[])
{
    int ii;
    int bank_id;
    uint32 prefix_value;
    uint8 is_dt;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_tcam_bank_alloc_verify(unit, handler_id, nof_banks_to_alloc, banks_to_alloc));

    /*
     * Take care of prefix when auto allocation is requested 
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.prefix_value.get(unit, handler_id, &prefix_value));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.direct_table.get(unit, handler_id, &is_dt));

    if (!is_dt && prefix_value == DNX_FIELD_TCAM_PREFIX_VAL_AUTO)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_set(unit, handler_id, nof_banks_to_alloc, banks_to_alloc));
    }

    for (ii = 0; ii < nof_banks_to_alloc; ii++)
    {
        bank_id = banks_to_alloc[ii];
        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_single_alloc(unit, handler_id, bank_id));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_bank_single_alloc_any(
    int unit,
    uint32 handler_id)
{
    int bank_id;
    int banks_to_alloc[1];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_algo_find_id(unit, handler_id, &bank_id));
    banks_to_alloc[0] = bank_id;
    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_alloc(unit, handler_id, 1, banks_to_alloc));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_bank_handler_free(
    int unit,
    uint32 handler_id)
{
    int bank_id;
    dnx_field_tcam_access_fg_params_t fg_params;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));
    /*
     * We free handler by deleting all the handler nodes from all banks active_handlers_id list 
     */
    for (bank_id = 0; bank_id < dnx_data_field.tcam.nof_banks_get(unit); bank_id++)
    {
        /*
         * We skip not found errors, we just want to delete the handler from all banks 
         */
        SHR_SET_CURRENT_ERR(dnx_field_tcam_bank_remove_handler_from_bank(unit, bank_id, handler_id));

        /*
         * Ignore not found errors, report the rest 
         */
        if (!SHR_FUNC_VAL_IS(_SHR_E_NOT_FOUND))
        {
            SHR_IF_ERR_EXIT(SHR_GET_CURRENT_ERR());
            /*
             * In case of no error, free prefix if relevant 
             */
            if (!fg_params.direct_table &&
                (fg_params.key_size != dnx_data_field.tcam.key_size_double_get(unit) || bank_id % 2 == 1))
            {
                SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_free(unit, bank_id, handler_id));
            }
        }
        else
        {
            SHR_SET_CURRENT_ERR(_SHR_E_NONE);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Returns the entry size per bank for the given key_size
 * 
 * \param[in] unit        - Device ID
 * \param[in] key_size    - Size of the key to return the entry size for
 * \param[out] entry_size - The size of the entry per bank for the given key_size
 *
 * \return
 *  \retval _SHR_E_NONE - Success
 */
static shr_error_e
dnx_field_tcam_bank_entry_size(
    int unit,
    uint32 key_size,
    dnx_field_tcam_entry_size_e * entry_size)
{
    SHR_FUNC_INIT_VARS(unit);

    *entry_size = DNX_FIELD_TCAM_ENTRY_SIZE_HALF;

    if (key_size == dnx_data_field.tcam.key_size_single_get(unit))
    {
        *entry_size = DNX_FIELD_TCAM_ENTRY_SIZE_SINGLE;
    }
    else if (key_size == dnx_data_field.tcam.key_size_double_get(unit))
    {
        *entry_size = DNX_FIELD_TCAM_ENTRY_SIZE_DOUBLE;
    }
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Updates about an entry at the given location that its state was changed.
 *  The entry was either freed or allocated and belongs to the given handler ID on the given core,
 *  for handlers double-sized key, this function updates both adjacent banks regarding the entry state
 *  change.
 *
 * \param [in] unit       - Device ID
 * \param [in] core       - Core ID
 * \param [in] location   - Location that was allocated/freed
 * \param [in] handler_id - The handler ID that the freed/allocated entry belongs to
 * \param [in] new_state  - The new state of the entry, whether freed or allocated
 *
 * \return
 *  \retval _SHR_E_NONE success
 */
static shr_error_e
dnx_field_tcam_bank_entry_state_change_update(
    int unit,
    int core,
    uint32 location,
    uint32 handler_id,
    uint8 new_state)
{
    uint32 key_size;
    dnx_field_tcam_entry_size_e entry_size;
    uint32 nof_free_halves;
    uint32 nof_free_singles;
    uint32 nof_free_c_halves;
    uint32 nof_free_c_singles;
    uint32 nof_free_doubles;
    uint8 single_occupied;
    uint8 double_occupied;
    int bank_id;
    int c_bank_id;
    int even_bank_id;
    int change = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.key_size.get(unit, handler_id, &key_size));

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_entry_size(unit, key_size, &entry_size));

    bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(location);
    c_bank_id = bank_id + ((bank_id % 2) ? -1 : 1);
    even_bank_id = bank_id - (bank_id % 2);

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                    nof_free_entries.get(unit, bank_id, core, DNX_FIELD_TCAM_ENTRY_SIZE_HALF, &nof_free_halves));
    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                    nof_free_entries.get(unit, bank_id, core, DNX_FIELD_TCAM_ENTRY_SIZE_SINGLE, &nof_free_singles));
    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                    nof_free_entries.get(unit, c_bank_id, core, DNX_FIELD_TCAM_ENTRY_SIZE_HALF, &nof_free_c_halves));
    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                    nof_free_entries.get(unit, c_bank_id, core, DNX_FIELD_TCAM_ENTRY_SIZE_SINGLE, &nof_free_c_singles));
    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                    nof_free_entries.get(unit, even_bank_id, core, DNX_FIELD_TCAM_ENTRY_SIZE_DOUBLE,
                                         &nof_free_doubles));

    if (new_state == DNX_FIELD_TCAM_BANK_ENTRY_ALLOCATED)
    {
        change = -1;
    }
    else
    {
        change = 1;
    }

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_entry_comp_state
                    (unit, core, location, &single_occupied, &double_occupied));

    switch (entry_size)
    {
        case DNX_FIELD_TCAM_ENTRY_SIZE_HALF:
        /**
         * Half entry can free/occupy single/double entries place if
         * they're not preoccupied.
         */
            nof_free_halves += change;
            if (!single_occupied)
            {
                nof_free_singles += change;
                if (!double_occupied)
                {
                    nof_free_doubles += change;
                }
            }
            break;
        case DNX_FIELD_TCAM_ENTRY_SIZE_SINGLE:
        /**
         * Single occupies both halves in the same bank.
         * And may occupy/free the double entry if it's not pre-occupied.
         */
            nof_free_halves += 2 * change;
            nof_free_singles += change;
            if (!double_occupied)
            {
                nof_free_doubles += change;
            }
            break;
        case DNX_FIELD_TCAM_ENTRY_SIZE_DOUBLE:
        /**
         * Double entry occupies both halves in both banks (even, odd).
         * and singles in both banks are also affected
         */
            nof_free_halves += 2 * change;
            nof_free_c_halves += 2 * change;
            nof_free_singles += change;
            nof_free_c_singles += change;
            nof_free_doubles += change;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid entry size %d", entry_size);
    }

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                    nof_free_entries.set(unit, bank_id, core, DNX_FIELD_TCAM_ENTRY_SIZE_HALF, nof_free_halves));
    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                    nof_free_entries.set(unit, bank_id, core, DNX_FIELD_TCAM_ENTRY_SIZE_SINGLE, nof_free_singles));
    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                    nof_free_entries.set(unit, c_bank_id, core, DNX_FIELD_TCAM_ENTRY_SIZE_HALF, nof_free_c_halves));
    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                    nof_free_entries.set(unit, c_bank_id, core, DNX_FIELD_TCAM_ENTRY_SIZE_SINGLE, nof_free_c_singles));
    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                    nof_free_entries.set(unit, even_bank_id, core, DNX_FIELD_TCAM_ENTRY_SIZE_DOUBLE, nof_free_doubles));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_bank_entry_alloc_update(
    int unit,
    int core,
    uint32 location,
    uint32 handler_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_entry_state_change_update
                    (unit, core, location, handler_id, DNX_FIELD_TCAM_BANK_ENTRY_ALLOCATED));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_bank_entry_free_update(
    int unit,
    int core,
    uint32 location,
    uint32 handler_id)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_entry_state_change_update
                    (unit, core, location, handler_id, DNX_FIELD_TCAM_BANK_ENTRY_FREED));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_bank_entry_move_update(
    int unit,
    int core,
    uint32 location_from,
    uint32 location_to,
    uint32 handler_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if (location_from == location_to)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_entry_free_update(unit, core, location_from, handler_id));
    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_entry_alloc_update(unit, core, location_to, handler_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_bank_available_entries_get(
    int unit,
    int core,
    int bank_id,
    dnx_field_tcam_entry_size_e entry_size,
    uint32 *nof_free_entries)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                    nof_free_entries.get(unit, bank_id, core, entry_size, nof_free_entries));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_bank_is_full(
    int unit,
    int core,
    int bank_id,
    uint32 handler_id,
    uint8 *full)
{
    uint32 key_size;
    dnx_field_tcam_entry_size_e entry_size;
    uint32 nof_free_entries;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.key_size.get(unit, handler_id, &key_size));

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_entry_size(unit, key_size, &entry_size));

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                    nof_free_entries.get(unit, bank_id, core, entry_size, &nof_free_entries));

    *full = (nof_free_entries == 0);

exit:
    SHR_FUNC_EXIT;
}
