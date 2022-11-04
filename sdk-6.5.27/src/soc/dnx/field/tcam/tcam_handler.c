
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#include <soc/dnx/field/tcam/tcam_handler.h>

#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/field/field_group.h>

#include <soc/dnx/field/tcam/tcam_location_manager.h>
#include <soc/dnx/field/tcam/tcam_defrag.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_manager_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

static shr_error_e
dnx_field_tcam_access_fg_params_t_init(
    int unit,
    dnx_field_tcam_access_fg_params_t * fg_params_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fg_params_p, _SHR_E_PARAM, "fg_params_p");

    sal_memset(fg_params_p, 0x0, sizeof(*fg_params_p));
    fg_params_p->stage = DNX_FIELD_TCAM_STAGE_INVALID;
    fg_params_p->dt_bank_id = DNX_FIELD_TCAM_BANK_ID_INVALID;
    fg_params_p->bank_allocation_mode = DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_INVALID;
    fg_params_p->context_sharing_handlers_cb = NULL;

exit:
    SHR_FUNC_EXIT;
}

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

static shr_error_e
dnx_field_tcam_handler_destroy_verify(
    int unit,
    uint32 handler_id)
{

    uint32 entry_id;
    int core;

    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNXC(dnx_field_tcam_handler_verify(unit, handler_id));

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_first(unit, core, handler_id, &entry_id));
        if (entry_id != DNX_FIELD_ENTRY_ACCESS_ID_INVALID)
        {

            SHR_ERR_EXIT(_SHR_E_EXISTS, "Failed to destroy handler %d, there are still added entries for this handler",
                         handler_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_handler_access_profile_id_get_verify(
    int unit,
    uint32 handler_id,
    int *access_profile_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNXC(dnx_field_tcam_handler_verify(unit, handler_id));
    SHR_NULL_CHECK(access_profile_id, _SHR_E_PARAM, "access_profile_id is null");
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_handler_entry_dt_location_alloc_verify(
    int unit,
    uint32 handler_id,
    dnx_field_tcam_entry_t * entry)
{
    dnx_field_tcam_access_fg_params_t fg_params;
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNXC(dnx_field_tcam_handler_verify(unit, handler_id));
    SHR_INVOKE_VERIFY_DNXC(dnx_field_tcam_handler_entry_verify(unit, entry));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));
    if (!fg_params.direct_table)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "This function cannot be invoked for non-DT handlers (handler_id: %d)\n",
                     handler_id);
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_handler_entry_location_alloc_verify(
    int unit,
    bcm_core_t core,
    uint32 handler_id,
    dnx_field_tcam_entry_t * entry)
{
    dnx_field_tcam_access_fg_params_t fg_params;
    bcm_core_t core_iter;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_field_tcam_handler_verify(unit, handler_id));
    SHR_INVOKE_VERIFY_DNXC(dnx_field_tcam_handler_entry_verify(unit, entry));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));
    if (fg_params.direct_table)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "This function cannot be invoked for DT handlers (handler_id: %d)\n", handler_id);
    }
    if (fg_params.bank_allocation_mode == DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_SELECT_WITH_LOCATION)
    {
        if (!DNX_FIELD_TCAM_LOCATION_IS_VALID(entry->priority))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Given entry location (bank_id:%d, bank_offset:%d) for 'SELECT_WITH_LOCATION' handler is not a valid TCAM location\n",
                         DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(entry->priority),
                         DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(entry->priority));
        }
        DNXCMN_CORES_ITER(unit, core, core_iter)
        {
            int occupied;
            SHR_IF_ERR_EXIT(dnx_field_tcam_location_is_location_occupied(unit, core_iter, entry->priority, &occupied));
            if (occupied)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Given entry location (bank_id:%d, bank_offset:%d) for 'SELECT_WITH_LOCATION' is already occupied by another entry at core %d\n",
                             DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(entry->priority),
                             DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(entry->priority), core_iter);
            }
        }
    }
    else
    {
        uint32 location;
        bcm_core_t core_iter;

        if (entry->priority == DNX_FIELD_TCAM_ENTRY_PRIORITY_INVALID
            || entry->priority >= dnx_data_field.tcam.max_tcam_priority_get(unit) - 1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "TCAM handler ID %d enrtry add error: Invalid priority(0x%X): (valid range 0-0x%X).\r\n",
                         handler_id, entry->priority, dnx_data_field.tcam.max_tcam_priority_get(unit) - 2);
        }

        DNXCMN_CORES_ITER(unit, core, core_iter)
        {
            SHR_IF_ERR_EXIT_ELSE_SET_EXPECT(dnx_field_tcam_access_sw.
                                            entry_location_hash.find(unit, core_iter, &(entry->id), &location),
                                            _SHR_E_NOT_FOUND);
            if (SHR_GET_CURRENT_ERR() != _SHR_E_NOT_FOUND)
            {
                SHR_ERR_EXIT(_SHR_E_EXISTS,
                             "Request to add entry with id 0x%08x failed. Entry already exists at absolute location 0x%08x\n",
                             entry->id, location);
            }
            SHR_SET_CURRENT_ERR(_SHR_E_NONE);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_handler_entry_location_free_verify(
    int unit,
    uint32 handler_id,
    uint32 entry_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNXC(dnx_field_tcam_handler_verify(unit, handler_id));
exit:
    SHR_FUNC_EXIT;
}

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

            SHR_IF_ERR_EXIT(dnx_field_tcam_manager_sw.tcam_handlers.set(unit, handler_index, &handler));
            *handler_id = handler_index;
            SHR_EXIT();
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_handler_id_free(
    int unit,
    uint32 handler_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_manager_sw.tcam_handlers.
                    state.set(unit, handler_id, DNX_FIELD_TCAM_HANDLER_STATE_CLOSE));
    SHR_IF_ERR_EXIT(dnx_field_tcam_manager_sw.tcam_handlers.
                    mode.set(unit, handler_id, DNX_FIELD_TCAM_HANDLER_MODE_INVALID));

exit:
    SHR_FUNC_EXIT;
}

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
    bcm_core_t core_iter;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_fg_params_t_init(unit, &fg_params));

    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_action_size_conversion(unit, db->action_size, &action_size_in_bits));
    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_key_size_conversion(unit, db->key_size, &key_size_in_bits));
    fg_params.key_size = key_size_in_bits;
    fg_params.action_size = action_size_in_bits;
    fg_params.actual_action_size = db->actual_action_size;
    fg_params.prefix_size = db->prefix_size;
    fg_params.stage = db->stage;
    fg_params.direct_table = db->is_direct;
    fg_params.bank_allocation_mode = db->bank_allocation_mode;
    fg_params.dt_bank_id = DNX_FIELD_TCAM_BANK_ID_INVALID;
    fg_params.context_sharing_handlers_cb = context_sharing_handlers_cb;
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.set(unit, handler_id, &fg_params));
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_iter)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_prefix_per_core.set(unit, handler_id, core_iter, db->prefix_value));
    }
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Saved params for handler %d, actual_action_size: %d, prefix_value: %d, prefix_size: %d\n",
                 handler_id, db->actual_action_size, db->prefix_value, db->prefix_size);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_handler_is_direct(
    int unit,
    uint32 handler_id,
    uint8 *is_direct_table_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.direct_table.get(unit, handler_id, is_direct_table_p));

exit:
    SHR_FUNC_EXIT;
}

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

    SHR_INVOKE_VERIFY_DNXC(dnx_field_tcam_handler_create_verify(unit, db, mode));

    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_id_alloc(unit, db, mode, handler_id));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_alloc(unit, *handler_id, db, access_profile_id));
    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_save_params(unit, *handler_id, db, context_sharing_handlers_cb));

    if (db->nof_banks_to_alloc > 0)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_alloc
                        (unit, *handler_id, db->nof_banks_to_alloc, db->banks_to_alloc, db->core_ids));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_handler_destroy(
    int unit,
    uint32 handler_id)
{
    bcm_core_t core_iter;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_field_tcam_handler_destroy_verify(unit, handler_id));

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_handler_free(unit, handler_id));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_free(unit, handler_id));
    {
        dnx_field_tcam_access_fg_params_t fg_params;
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));

        SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.stage.set(unit, handler_id, DNX_FIELD_TCAM_STAGE_INVALID));
        if (fg_params.bank_allocation_mode != DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_AUTO)
        {
            SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.
                            dt_bank_id.set(unit, handler_id, DNX_FIELD_TCAM_BANK_ID_INVALID));
        }
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.
                        bank_allocation_mode.set(unit, handler_id, DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_INVALID));
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.key_size.set(unit, handler_id, 0));
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.action_size.set(unit, handler_id, 0));
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.actual_action_size.set(unit, handler_id, 0));
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.prefix_size.set(unit, handler_id, 0));
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.direct_table.set(unit, handler_id, FALSE));
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.context_sharing_handlers_cb.set(unit, handler_id, NULL));
        DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_iter)
        {
            SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_prefix_per_core.set(unit, handler_id, core_iter, 0));
        }
    }
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

    SHR_INVOKE_VERIFY_DNXC(dnx_field_tcam_handler_access_profile_id_get_verify(unit, handler_id, access_profile_id));
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

    *app_db_id = (dnx_field_app_db_id_t) access_profile_id_in;
    if ((uint32) access_profile_id_in != (uint32) (*app_db_id))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "dnx_field_app_db_id_t type not big enough to take in access_profile_id.\r\n"
                     "Converted %d to %d.\r\n", access_profile_id_in, *app_db_id);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_handler_bank_status_check(
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

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_banks_sorted_get(unit, handler_id, core, bank_ids_p, &bank_count));

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
dnx_field_tcam_handler_dt_calc_offset_from_entry_id(
    int unit,
    uint32 entry_id,
    uint32 *bank_offset)
{
    SHR_FUNC_INIT_VARS(unit);

    *bank_offset = DNX_FIELD_ENTRY_TCAM_DT_ACCESS_ID(entry_id) * 2;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_handler_entry_dt_location_alloc(
    int unit,
    bcm_core_t core,
    uint32 handler_id,
    dnx_field_tcam_entry_t * entry)
{
    dnx_field_tcam_location_t location;
    dnx_field_tcam_access_fg_params_t fg_params;
    bcm_core_t core_iter;
    int bank_id;
    uint32 bank_offset;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_field_tcam_handler_entry_dt_location_alloc_verify(unit, handler_id, entry));

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.dt_bank_id.get(unit, handler_id, &bank_id));

    if (bank_id == DNX_FIELD_TCAM_BANK_ID_INVALID)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_single_alloc_any(unit, handler_id, BCM_CORE_ALL));

        SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.dt_bank_id.get(unit, handler_id, &bank_id));
    }

    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_dt_calc_offset_from_entry_id(unit, entry->id, &bank_offset));

    location.bank_id = bank_id;

    location.bank_offset = bank_offset + 1;

    DNXCMN_CORES_ITER(unit, core, core_iter)
    {

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_alloc(unit, core_iter, handler_id, entry, &location, TRUE));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_handler_entry_location_alloc(
    int unit,
    bcm_core_t core,
    uint32 handler_id,
    dnx_field_tcam_entry_t * entry)
{
    dnx_field_tcam_location_t location;
    uint8 found;
    uint32 absolute_location;
    bcm_core_t core_iter;
    uint8 banks_full;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_field_tcam_handler_entry_location_alloc_verify(unit, core, handler_id, entry));

    DNXCMN_CORES_ITER(unit, core, core_iter)
    {

        SHR_IF_ERR_EXIT(dnx_field_tcam_handler_bank_status_check(unit, core_iter, handler_id, &banks_full));
        if (banks_full)
        {
            dnx_field_tcam_access_fg_params_t fg_params;
            SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));

            if (fg_params.bank_allocation_mode != DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_AUTO)
            {

                SHR_ERR_EXIT(_SHR_E_RESOURCE,
                             "All SELECTED banks are full, no more space to add entry  BankAllocateMode(%s) core (%d)",
                             dnx_field_tcam_bank_allocation_mode_e_get_name(fg_params.bank_allocation_mode), core_iter);
            }
            DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(dnx_field_tcam_bank_single_alloc_any
                                                       (unit, handler_id, core_iter));
        }
    }

    DNXCMN_CORES_ITER(unit, core, core_iter)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_find(unit, core_iter, handler_id, entry, &location, &found));
        if (!found)
        {

            SHR_IF_ERR_EXIT(dnx_field_tcam_defrag(unit, core_iter, handler_id, &location));
            absolute_location = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(location.bank_id, location.bank_offset);
        }

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_alloc(unit, core_iter, handler_id, entry, &location, FALSE));

        absolute_location = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(location.bank_id, location.bank_offset);

        SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.valid_bmp.v_bit.bit_set(unit, core_iter, absolute_location));

        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_entry_alloc_update(unit, core_iter, absolute_location, handler_id));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_handler_entry_valid_set(
    int unit,
    bcm_core_t core,
    uint32 entry_id,
    uint8 valid_bit)
{
    bcm_core_t core_iter;
    dnx_field_tcam_location_t location;
    uint32 absolute_location;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, core, core_iter)
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

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_handler_entry_valid_get(
    int unit,
    bcm_core_t core,
    uint32 entry_id,
    uint8 *valid_bit)
{
    dnx_field_tcam_location_t location;
    uint32 absolute_location;

    SHR_FUNC_INIT_VARS(unit);

    if (core == BCM_CORE_ALL)
    {
        core = 0;
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
    bcm_core_t core_iter;
    uint8 entries_left = FALSE;
    uint8 entry_found = FALSE;
    int bank_to_evac_id[1];
    bcm_core_t core_ids[1];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));
    SHR_INVOKE_VERIFY_DNXC(dnx_field_tcam_handler_entry_location_free_verify(unit, handler_id, entry_id));

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_iter)
    {
        int rv;
        rv = dnx_field_tcam_handler_entry_location(unit, core_iter, entry_id, &location);
        if (rv == _SHR_E_NOT_FOUND)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(rv);
        entry_found = TRUE;
        bank_to_evac_id[0] = location.bank_id;

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_free(unit, core_iter, handler_id, entry_id, fg_params.direct_table));
        if (!fg_params.direct_table)
        {
            absolute_location = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(location.bank_id, location.bank_offset);

            SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.valid_bmp.v_bit.bit_clear(unit, core_iter, absolute_location));

            SHR_IF_ERR_EXIT(dnx_field_tcam_bank_entry_free_update(unit, core_iter, absolute_location, handler_id));
        }

        if (fg_params.bank_allocation_mode == DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_AUTO && !fg_params.direct_table)
        {
            uint8 entries_left_on_core;

            SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_is_bank_handler_free
                            (unit, core_iter, handler_id, location.bank_id, &entries_left_on_core));
            if (entries_left_on_core == 0)
            {
                core_ids[0] = core_iter;
                DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(dnx_field_tcam_defrag_bank_evacuate
                                                           (unit, handler_id, 1, bank_to_evac_id, core_ids));
            }
        }
    }

    if (!entry_found)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Entry 0x%08x could not be found in TCAM for handler %d\n", entry_id,
                     handler_id);
    }

    if (fg_params.bank_allocation_mode == DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_AUTO && fg_params.direct_table)
    {
        DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_iter)
        {
            uint8 entries_left_on_core;

            SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_is_bank_handler_free
                            (unit, core_iter, handler_id, location.bank_id, &entries_left_on_core));

            entries_left += entries_left_on_core;
        }

        if (entries_left == 0)
        {
            core_ids[0] = BCM_CORE_ALL;
            SHR_IF_ERR_EXIT(dnx_field_tcam_defrag_bank_evacuate(unit, handler_id, 1, bank_to_evac_id, core_ids));
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
                         "Reason: Error Sharing bank in the same core with Handler ID %d in same program\r\n",
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
    bcm_core_t core,
    uint32 entry_id,
    dnx_field_tcam_location_t * location)
{
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORE_VALIDATE(unit, core, FALSE);

    rv = dnx_field_tcam_location_entry_id_to_location(unit, core, entry_id, location);
    if (rv == _SHR_E_NOT_FOUND)
    {
        SHR_IF_ERR_EXIT_NO_MSG(rv);
    }
    SHR_IF_ERR_EXIT(rv);

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
    uint32 banks_bmp_sw;
    uint32 entries_count;
    uint32 bank_id;
    bcm_core_t core_iter;

    SHR_FUNC_INIT_VARS(unit);

    entries_count = 0;
    (*banks_bmp) = SAL_UPTO_BIT(dnx_data_field.tcam.nof_banks_get(unit));

    for (bank_id = 0; bank_id < dnx_data_field.tcam.nof_banks_get(unit); bank_id++)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_bank_entries_count(unit, core, handler_id, bank_id, &entries_count));
        bank_entries_count[bank_id] = entries_count;
    }
    DNXCMN_CORES_ITER(unit, core, core_iter)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_banks_bmp_get(unit, handler_id, core, &banks_bmp_sw));

        (*banks_bmp) &= banks_bmp_sw;
    }
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

static shr_error_e
dnx_field_tcam_handler_bank_evacuate_verify(
    int unit,
    uint32 handler_id,
    int nof_banks,
    int bank_ids[],
    bcm_core_t core_ids[])
{
    int bank_idx;
    bcm_core_t core_iter;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_verify(unit, handler_id));

    for (bank_idx = 0; bank_idx < nof_banks; bank_idx++)
    {
        uint8 found;

        DNXCMN_CORES_ITER(unit, core_ids[bank_idx], core_iter)
        {

            SHR_IF_ERR_EXIT(dnx_field_tcam_bank_is_handler_on_bank
                            (unit, bank_ids[bank_idx], handler_id, core_iter, &found));

            if (!found)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Given bank %d is not allocated for handler %d on core %d\n",
                             bank_ids[bank_idx], handler_id, core_iter);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_handler_bank_add_verify(
    int unit,
    uint32 handler_id,
    int nof_banks,
    int bank_ids[])
{
    uint32 banks_bmp;
    bcm_core_t core_iter;
    dnx_field_tcam_bank_allocation_mode_e bank_allocation_mode;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_verify(unit, handler_id));

    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_bank_allocation_mode_get(unit, handler_id, &bank_allocation_mode));
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_iter)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_banks_bmp_get(unit, handler_id, core_iter, &banks_bmp));

        if (bank_allocation_mode == DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_AUTO && banks_bmp != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error adding bank(s) for TCAM Database with 'AUTO' bank allocation mode\n");
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
    int bank_ids[],
    bcm_core_t core_ids[])
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_field_tcam_handler_bank_evacuate_verify
                           (unit, handler_id, nof_banks, bank_ids, core_ids));

    SHR_IF_ERR_EXIT(dnx_field_tcam_defrag_bank_evacuate(unit, handler_id, nof_banks, bank_ids, core_ids));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_handler_bank_add(
    int unit,
    uint32 handler_id,
    int nof_banks,
    int bank_ids[],
    bcm_core_t core_ids[])
{
    dnx_field_tcam_bank_allocation_mode_e bank_allocation_mode;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_field_tcam_handler_bank_add_verify(unit, handler_id, nof_banks, bank_ids));

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.
                    bank_allocation_mode.get(unit, handler_id, &bank_allocation_mode));

    if (bank_allocation_mode == DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_AUTO)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.
                        bank_allocation_mode.set(unit, handler_id, DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_SELECT));
    }

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_alloc(unit, handler_id, nof_banks, bank_ids, core_ids));

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
dnx_field_tcam_handler_entry_core_get(
    int unit,
    uint32 entry_id,
    int *core)
{
    uint8 found;
    uint8 exists_on_all = TRUE;
    bcm_core_t core_iter;
    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_iter)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_entry_id_exists(unit, core_iter, entry_id, &found));
        if (found)
        {
            *core = core_iter;
        }
        else
        {
            exists_on_all = FALSE;
        }
    }
    if (exists_on_all && dnx_data_device.general.nof_cores_get(unit) > 1)
    {
        *core = BCM_CORE_ALL;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_handler_entry_handler(
    int unit,
    uint32 entry_id,
    uint32 *handler_id)
{
    dnx_field_tcam_location_t location;
    bcm_core_t core_iter;
    uint8 entry_found = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_iter)
    {
        int rv = dnx_field_tcam_handler_entry_location(unit, core_iter, entry_id, &location);
        if (rv == _SHR_E_NOT_FOUND)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(rv);
        entry_found = TRUE;

        SHR_IF_ERR_EXIT_WITH_LOG(dnx_field_tcam_location_allocating_handler(unit, core_iter, &location, handler_id),
                                 "No TCAM handler found for the given entry ID 0x%08x%s%s", entry_id, EMPTY, EMPTY);
        break;
    }
    if (!entry_found)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT;
}
