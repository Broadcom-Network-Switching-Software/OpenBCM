
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * 
 */

shr_error_e
nif_oft_credits_counter_port_reset_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__oft_context;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_OFT_CONTEXT, key_value__oft_context);
    *offset = key_value__oft_context;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_oft_calendar_client_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY_INDEX, key_value__entry_index);
    *offset = key_value__entry_index/dnx_data_nif.oft.nof_internal_calendar_entries_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_oft_calendar_client_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY_INDEX, key_value__entry_index);
    *offset = ((key_value__entry_index)%dnx_data_nif.oft.nof_internal_calendar_entries_get(unit))*dnx_data_nif.oft.nof_bit_per_internal_entry_in_calendar_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
