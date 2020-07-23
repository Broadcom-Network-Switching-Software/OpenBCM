
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm_int/dnx/field/tcam/tcam_defrag.h>
#include <bcm_int/dnx/field/tcam/tcam_access_profile_manager.h>
#include <bcm_int/dnx/field/tcam/tcam_bank_manager.h>

#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_state_snapshot_manager.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_location_access.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX


static shr_error_e
dnx_field_tcam_defrag_bank_status_check(
    int unit,
    int core,
    uint32 handler_id,
    uint8 *full)
{
    int *bank_ids_p = NULL;

    int bank_count;
    int bank_i;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(bank_ids_p, sizeof(*bank_ids_p) * dnx_data_field.tcam.nof_banks_get(unit), "bank_ids_p", "%s%s%s", EMPTY,
              EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_banks_sorted_get(unit, handler_id, bank_ids_p, &bank_count));

    *full = TRUE;

    for (bank_i = 0; bank_i < bank_count; bank_i++)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_is_full(unit, core, bank_ids_p[bank_i], handler_id, full));
        if (!(*full))
        {
            
            break;
        }
    }

exit:
    SHR_FREE(bank_ids_p);
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_defrag_default(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_location_t * location)
{
    uint8 found;
    uint8 banks_full;
    uint32 empty_place;
    uint32 absolute_location;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_field_tcam_defrag_bank_status_check(unit, core, handler_id, &banks_full));
    if (banks_full)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_single_alloc_any(unit, handler_id));
    }

    
    absolute_location = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(location->bank_id, location->bank_offset);
    if (absolute_location == FIELD_TCAM_LOCATION_INVALID)
    {
        
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_empty_location_find
                    (unit, core, handler_id, location, DNX_FIELD_TCAM_LOCATION_FIND_MODE_CLOSEST, &empty_place,
                     &found));
    if (found && (absolute_location != empty_place))
    {
        if (absolute_location > empty_place)
        {
            
            SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_prev_get
                            (unit, core, handler_id, absolute_location, &absolute_location, &found));
            if (!found)
            {
                
                SHR_EXIT();
            }
        }
        else
        {
            
            SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_next_get
                            (unit, core, handler_id, absolute_location - 1, &absolute_location, &found));
            if (!found)
            {
                
                SHR_EXIT();
            }
        }

        
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_move(unit, core, handler_id, absolute_location, empty_place));
    }
    

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_defrag(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_location_t * location)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);
    DNX_STATE_COMPARISON_SUPPRESS(unit, TRUE);

    SHR_IF_ERR_EXIT(dnx_field_tcam_defrag_mode(unit, core, handler_id, location, DNX_FIELD_TCAM_DEFRAG_MODE_DEFAULT));

exit:
    DNX_STATE_COMPARISON_SUPPRESS(unit, FALSE);
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_defrag_mode(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_location_t * location,
    int mode)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (mode)
    {
        case DNX_FIELD_TCAM_DEFRAG_MODE_DEFAULT:
            SHR_IF_ERR_EXIT(dnx_field_tcam_defrag_default(unit, core, handler_id, location));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported mode - %d\n", mode);
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_defrag_banks_nof_free(
    int unit,
    int core,
    uint32 handler_id,
    int bank_id,
    uint32 *nof_free_forward,
    uint32 *nof_free_backward)
{
    int *bank_ids_p = NULL;

    int bank_count;
    int bank_i;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(bank_ids_p, sizeof(*bank_ids_p) * dnx_data_field.tcam.nof_banks_get(unit), "bank_ids_p", "%s%s%s", EMPTY,
              EMPTY, EMPTY);

    *nof_free_forward = 0;
    *nof_free_backward = 0;

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_banks_sorted_get(unit, handler_id, bank_ids_p, &bank_count));

    for (bank_i = 0; bank_i < bank_count; bank_i++)
    {
        uint32 bank_nof_free;

        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_available_entries_get(unit, core, bank_ids_p[bank_i], 0, &bank_nof_free));

        if (bank_ids_p[bank_i] < bank_id)
        {
            *nof_free_backward = *nof_free_backward + bank_nof_free;
        }
        else
        {
            *nof_free_forward = *nof_free_forward + bank_nof_free;
        }
    }

exit:
    SHR_FREE(bank_ids_p);
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_defrag_entry_in_bank_get(
    int unit,
    int core,
    uint32 handler_id,
    int bank_id,
    uint32 num,
    uint8 reverse,
    uint32 *location,
    uint8 *found)
{
    uint8 next_found;
    uint32 next_location;

    SHR_FUNC_INIT_VARS(unit);

    *found = FALSE;

    *location = (reverse) ?
        DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(bank_id + 1, 0) :
        DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(bank_id, 0) - 1;

    while (num > 0)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_next_get_extended
                        (unit, core, handler_id, *location, reverse, &next_location, &next_found));
        if (!next_found || (DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(next_location) != bank_id))
        {
            
            break;
        }
        
        *found = TRUE;
        
        *location = next_location;
        num--;
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_defrag_bank_evacuate_internal(
    int unit,
    int core,
    uint32 handler_id,
    int bank_id)
{
    uint32 nof_free_forward;
    uint32 nof_free_backward;
    uint8 found;
    uint32 dummy;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_field_tcam_defrag_banks_nof_free
                    (unit, core, handler_id, bank_id, &nof_free_forward, &nof_free_backward));

    
    if (nof_free_backward > 0)
    {
        uint32 first_empty;
        uint32 location_from;
        dnx_field_tcam_location_t first_location = { 0, 0 };
        
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_empty_location_find
                        (unit, core, handler_id, &first_location, DNX_FIELD_TCAM_LOCATION_FIND_MODE_FORWARD,
                         &first_empty, &found));
        
        if (!found)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Unexpected error while evacuating handler %d from bank %d core %d: No empty location found while expecting at least one to be\r\n",
                         handler_id, bank_id, core);
        }
        SHR_IF_ERR_EXIT(dnx_field_tcam_defrag_entry_in_bank_get
                        (unit, core, handler_id, bank_id, nof_free_backward, FALSE, &location_from, &found));
        if (!found)
        {
            
            SHR_EXIT();
        }
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_move_extended(unit, core, handler_id, location_from, first_empty));
    }
    
    if (nof_free_forward > 0)
    {
        uint32 last_empty;
        uint32 location_from;
        dnx_field_tcam_location_t last_location =
            { dnx_data_field.tcam.nof_banks_get(unit) - 1, dnx_data_field.tcam.nof_small_bank_lines_get(unit) };
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_empty_location_find
                        (unit, core, handler_id, &last_location, DNX_FIELD_TCAM_LOCATION_FIND_MODE_BACKWARD,
                         &last_empty, &found));
        if (!found)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Unexpected error while evacuating handler %d from bank %d core %d: No empty location found while expecting at least one to be\r\n",
                         handler_id, bank_id, core);
        }
        SHR_IF_ERR_EXIT(dnx_field_tcam_defrag_entry_in_bank_get
                        (unit, core, handler_id, bank_id, nof_free_forward, TRUE, &location_from, &found));
        if (!found)
        {
            
            SHR_EXIT();
        }
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_move_extended(unit, core, handler_id, location_from, last_empty));
    }
    SHR_IF_ERR_EXIT(dnx_field_tcam_defrag_entry_in_bank_get(unit, core, handler_id, bank_id, 1, TRUE, &dummy, &found));
    if (found)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                     "Failed to evacuate handler %d from bank %d on core %d, no more empty space in other allocated banks to move handler's entries to\r\n",
                     handler_id, bank_id, core);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_defrag_bank_evacuate(
    int unit,
    uint32 handler_id,
    int nof_banks,
    int bank_ids[])
{
    int core;
    int bank_idx;
    dnx_field_tcam_access_fg_params_t fg_params;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));

    
    for (bank_idx = 0; bank_idx < nof_banks; bank_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_remove_handler_from_bank(unit, bank_ids[bank_idx], handler_id));
        if (fg_params.key_size == dnx_data_field.tcam.key_size_double_get(unit))
        {
            SHR_IF_ERR_EXIT(dnx_field_tcam_bank_remove_handler_from_bank(unit, bank_ids[bank_idx] + 1, handler_id));
        }
    }

    for (bank_idx = 0; bank_idx < nof_banks; bank_idx++)
    {
        
        for (core = 0; core < dnx_data_device.general.nof_cores_get(unit); core++)
        {
            SHR_SET_CURRENT_ERR(dnx_field_tcam_defrag_bank_evacuate_internal
                                (unit, core, handler_id, bank_ids[bank_idx]));
            if (SHR_FUNC_ERR())
            {
                char msg[100];
                sal_snprintf(msg, 100, "Failed evacuating handler %d from bank %d!", handler_id, bank_ids[bank_idx]);
                
                for (; bank_idx < nof_banks; bank_idx++)
                {
                    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_add_handler_to_bank(unit, bank_ids[bank_idx], handler_id));
                    if (fg_params.key_size == dnx_data_field.tcam.key_size_double_get(unit))
                    {
                        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_add_handler_to_bank
                                        (unit, bank_ids[bank_idx] + 1, handler_id));
                    }
                }
                SHR_ERR_EXIT(SHR_GET_CURRENT_ERR(), "%s", msg);
            }
        }
        
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_bank_remove(unit, handler_id, bank_ids[bank_idx]));

        if (!fg_params.direct_table && fg_params.key_size != dnx_data_field.tcam.key_size_double_get(unit))
        {
            SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_free(unit, bank_ids[bank_idx], handler_id));
        }

        if (fg_params.key_size == dnx_data_field.tcam.key_size_double_get(unit))
        {
            SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_bank_remove(unit, handler_id, bank_ids[bank_idx] + 1));

            SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_free(unit, bank_ids[bank_idx] + 1, handler_id));
        }
    }

exit:
    SHR_FUNC_EXIT;
}
