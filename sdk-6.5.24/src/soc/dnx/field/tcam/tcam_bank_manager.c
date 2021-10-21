
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#include <soc/dnx/field/tcam/tcam_bank_manager.h>

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>

#include <soc/dnx/field/tcam/tcam_bank_algo.h>
#include <soc/dnx/field/tcam/tcam_location_manager.h>
#include <soc/dnx/field/tcam/tcam_access_profile_manager.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_bank_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_access.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_location_types.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

#define DNX_FIELD_TCAM_BANK_ENTRY_FREED 0
#define DNX_FIELD_TCAM_BANK_ENTRY_ALLOCATED 1

static shr_error_e
dnx_field_tcam_bank_single_check(
    int unit,
    int bank_id,
    uint32 handler_id,
    bcm_core_t core_id,
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

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_algo_get_owner_stage(unit, bank_id, &bank_owner_stage));
    if (bank_owner_stage != DNX_FIELD_TCAM_STAGE_INVALID && bank_owner_stage != fg_params->stage)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error allocating bank: Requested bank_id %d is allocated for stage (%d,%s)"
                     ", database stage is (%d,%s)", bank_id,
                     bank_owner_stage, dnx_field_tcam_stage_e_get_name(bank_owner_stage),
                     fg_params->stage, dnx_field_tcam_stage_e_get_name(fg_params->stage));
    }

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_algo_handler_in_bank(unit, bank_id, handler_id, core_id, &handler_in_bank));
    if (handler_in_bank)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error allocating bank: Bank %d is already allocated for handler %d on core %d",
                     bank_id, handler_id, core_id);
    }

    if (!fg_params->direct_table)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_is_free(unit, bank_id, handler_id, core_id, &is_free));
        if (!is_free)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error allocating bank: Prefix of handler %d is already taken on bank %d on core %d",
                         handler_id, bank_id, core_id);
        }
    }
    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.bank_mode.get(unit, bank_id, &bank_mode));

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

    if (fg_params->direct_table && bank_mode != DNX_FIELD_TCAM_BANK_MODE_RESTRICTED_DT
        && bank_mode != DNX_FIELD_TCAM_BANK_MODE_NONE)
    {

        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error allocating bank: Direct Table field group (%d) can't allocate bank (%d) not intended for DT\r\n",
                     handler_id, bank_id);
    }
    else if (((fg_params->key_size != dnx_data_field.tcam.key_size_single_get(unit) ||
               fg_params->action_size != dnx_data_field.tcam.action_size_half_get(unit)) &&
              (bank_mode == DNX_FIELD_TCAM_BANK_MODE_RESTRICTED_DT) && !fg_params->direct_table))
    {

        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error allocating bank: Regular field group (%d) can't allocate bank (%d) with DT mode\r\n",
                     handler_id, bank_id);
    }

    if (!dnx_data_field.tcam.feature_get(unit, dnx_data_field_tcam_mixed_ratio_supported))
    {
        uint32 bank_key_size;

        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.key_size.get(unit, bank_id, &bank_key_size));
        if (bank_key_size != 0 && bank_key_size != fg_params->key_size)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error allocating bank: Bank %d is already allocated for handlers with key size %d, requesting handler key size %d\r\n",
                         bank_id, bank_key_size, fg_params->key_size);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_bank_alloc_verify(
    int unit,
    uint32 handler_id,
    int nof_banks_to_alloc,
    int banks_to_alloc[],
    bcm_core_t core_ids[])
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
        bcm_core_t core_iter;
        DNXCMN_CORES_ITER(unit, core_ids[ii], core_iter)
        {
            if (fg_params.key_size == dnx_data_field.tcam.key_size_double_get(unit))
            {
                if (bank_id % 2)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Handler %d with double-sized key can only be added to even bank ids,"
                                 " given bank id %d is not even\r\n", handler_id, bank_id);
                }

                SHR_IF_ERR_EXIT(dnx_field_tcam_bank_single_check(unit, bank_id + 1, handler_id, core_iter, &fg_params));
            }
            SHR_IF_ERR_EXIT(dnx_field_tcam_bank_single_check(unit, bank_id, handler_id, core_iter, &fg_params));
        }
        for (jj = ii + 1; jj < nof_banks_to_alloc; jj++)
        {
            if (bank_id == banks_to_alloc[jj])
            {
                if (core_ids[ii] == core_ids[jj] || core_ids[ii] == BCM_CORE_ALL || core_ids[jj] == BCM_CORE_ALL)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Error: Bank and core requested at index %d is equal to bank and core requested at index %d\r\n",
                                 ii, jj);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_bank_add_handler_to_bank(
    int unit,
    int bank_id,
    uint32 handler_id,
    bcm_core_t core_id)
{
    dnx_field_tcam_access_fg_params_t fg_params;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORE_VALIDATE(unit, core_id, FALSE);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.active_handlers_id.add_last(unit, bank_id, core_id, &handler_id));

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.owner_stage.set(unit, bank_id, fg_params.stage));
    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.key_size.set(unit, bank_id, fg_params.key_size));
    if (fg_params.direct_table)
    {

        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                        bank_mode.set(unit, bank_id, DNX_FIELD_TCAM_BANK_MODE_RESTRICTED_DT));

        SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.dt_bank_id.set(unit, handler_id, bank_id));
    }
    else
    {

        switch (fg_params.bank_allocation_mode)
        {
            case DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_AUTO:
            {

                SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                                bank_mode.set(unit, bank_id, DNX_FIELD_TCAM_BANK_MODE_OPEN));
                break;
            }
            case DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_SELECT:
            {
                int bank_mode;
                SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.bank_mode.get(unit, bank_id, &bank_mode));

                if (bank_mode != DNX_FIELD_TCAM_BANK_MODE_RESTRICTED_DT)
                {
                    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                                    bank_mode.set(unit, bank_id, DNX_FIELD_TCAM_BANK_MODE_RESTRICTED));
                }
                break;
            }
            case DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_SELECT_WITH_LOCATION:
            {

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
    bcm_core_t core_id,
    uint8 *found)
{
    sw_state_ll_node_t handler_node;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORE_VALIDATE(unit, core_id, FALSE);

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_algo_get_handler_node_in_bank
                    (unit, bank_id, handler_id, core_id, &handler_node));

    *found = (handler_node != SW_STATE_LL_INVALID);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_bank_remove_handler_from_bank(
    int unit,
    int bank_id,
    uint32 handler_id,
    bcm_core_t core_id,
    int *found)
{
    sw_state_ll_node_t handler_node;
    uint32 list_size;
    dnx_field_tcam_access_fg_params_t fg_params;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORE_VALIDATE(unit, core_id, FALSE);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_algo_get_handler_node_in_bank
                    (unit, bank_id, handler_id, core_id, &handler_node));

    if (handler_node != SW_STATE_LL_INVALID)
    {
        *found = TRUE;

        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                        active_handlers_id.remove_node(unit, bank_id, core_id, handler_node));

        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                        active_handlers_id.nof_elements(unit, bank_id, core_id, &list_size));

        if (list_size == 0)
        {

            SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                            owner_stage.set(unit, bank_id, DNX_FIELD_TCAM_STAGE_INVALID));
            SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.key_size.set(unit, bank_id, 0));
            SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                            bank_mode.set(unit, bank_id, DNX_FIELD_TCAM_BANK_MODE_NONE));
        }

        if (fg_params.direct_table)
        {
            SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.
                            dt_bank_id.set(unit, handler_id, DNX_FIELD_TCAM_BANK_ID_INVALID));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_bank_single_alloc(
    int unit,
    uint32 handler_id,
    uint32 bank_id,
    bcm_core_t core_id)
{
    dnx_field_tcam_access_fg_params_t fg_params;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_add_handler_to_bank(unit, bank_id, handler_id, core_id));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_bank_add(unit, handler_id, bank_id, core_id));

    if (!fg_params.direct_table && fg_params.key_size != dnx_data_field.tcam.key_size_double_get(unit))
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_bank_alloc(unit, bank_id, handler_id, core_id));
    }

    if (fg_params.key_size == dnx_data_field.tcam.key_size_double_get(unit))
    {

        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_add_handler_to_bank(unit, bank_id + 1, handler_id, core_id));
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_bank_add(unit, handler_id, bank_id + 1, core_id));
        SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_bank_alloc(unit, bank_id + 1, handler_id, core_id));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_bank_alloc(
    int unit,
    uint32 handler_id,
    int nof_banks_to_alloc,
    int banks_to_alloc[],
    bcm_core_t core_ids[])
{
    int ii;
    int bank_id;
    uint32 prefix_value;
    uint8 is_dt;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_field_tcam_bank_alloc_verify
                           (unit, handler_id, nof_banks_to_alloc, banks_to_alloc, core_ids));

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.prefix_value.get(unit, handler_id, &prefix_value));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.direct_table.get(unit, handler_id, &is_dt));

    if (!is_dt && prefix_value == DNX_FIELD_TCAM_PREFIX_VAL_AUTO)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_set(unit, handler_id, nof_banks_to_alloc, banks_to_alloc, core_ids));
    }

    for (ii = 0; ii < nof_banks_to_alloc; ii++)
    {
        bcm_core_t core_iter;
        bank_id = banks_to_alloc[ii];
        DNXCMN_CORES_ITER(unit, core_ids[ii], core_iter)
        {
            SHR_IF_ERR_EXIT(dnx_field_tcam_bank_single_alloc(unit, handler_id, bank_id, core_iter));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_bank_single_alloc_any(
    int unit,
    uint32 handler_id,
    bcm_core_t core_id)
{
    int bank_id;
    int banks_to_alloc[1];
    bcm_core_t core_ids[1];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_algo_find_id(unit, handler_id, core_id, &bank_id));
    banks_to_alloc[0] = bank_id;
    core_ids[0] = core_id;
    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_alloc(unit, handler_id, 1, banks_to_alloc, core_ids));

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
    bcm_core_t core_iter;
    int found = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));

    for (bank_id = 0; bank_id < dnx_data_field.tcam.nof_banks_get(unit); bank_id++)
    {
        DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_iter)
        {

            SHR_SET_CURRENT_ERR(dnx_field_tcam_bank_remove_handler_from_bank
                                (unit, bank_id, handler_id, core_iter, &found));

            if (found)
            {

                if (!fg_params.direct_table &&
                    (fg_params.key_size != dnx_data_field.tcam.key_size_double_get(unit) || bank_id % 2 == 1))
                {
                    SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_free(unit, bank_id, handler_id, core_iter));
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

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

            nof_free_halves += 2 * change;
            nof_free_singles += change;
            if (!double_occupied)
            {
                nof_free_doubles += change;
            }
            break;
        case DNX_FIELD_TCAM_ENTRY_SIZE_DOUBLE:

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
    uint8 contains_half_key_handler;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.key_size.get(unit, handler_id, &key_size));

    *full = FALSE;
    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_algo_bank_contains_half_key_handler
                    (unit, bank_id, core, &contains_half_key_handler));

    if (!dnx_data_field.tcam.feature_get(unit, dnx_data_field_tcam_tcam_80b_traffic_safe_write) &&
        contains_half_key_handler)
    {
        if (key_size == dnx_data_field.tcam.key_size_half_get(unit))
        {
            uint32 nof_free_160_entries;
            SHR_IF_ERR_EXIT(dnx_field_tcam_bank_available_entries_get
                            (unit, core, bank_id, DNX_FIELD_TCAM_ENTRY_SIZE_SINGLE, &nof_free_160_entries));
            if (nof_free_160_entries == 1)
            {
                uint8 is_vacant_80b_entry;
                SHR_IF_ERR_EXIT(dnx_field_tcam_location_vacant_80b_entries_get
                                (unit, core, handler_id, bank_id, &is_vacant_80b_entry));
                *full = !is_vacant_80b_entry;
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_field_tcam_bank_entry_size(unit, key_size, &entry_size));

            SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                            nof_free_entries.get(unit, bank_id, core, DNX_FIELD_TCAM_ENTRY_SIZE_SINGLE,
                                                 &nof_free_entries));

            *full = (nof_free_entries == 1);
            if (!*full && key_size == dnx_data_field.tcam.key_size_half_get(unit))
            {
                SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                                nof_free_entries.get(unit, bank_id + 1, core, DNX_FIELD_TCAM_ENTRY_SIZE_SINGLE,
                                                     &nof_free_entries));
                *full = (nof_free_entries == 1);
            }

        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_entry_size(unit, key_size, &entry_size));

        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                        nof_free_entries.get(unit, bank_id, core, entry_size, &nof_free_entries));

        *full = (nof_free_entries == 0);
    }

exit:
    SHR_FUNC_EXIT;
}
