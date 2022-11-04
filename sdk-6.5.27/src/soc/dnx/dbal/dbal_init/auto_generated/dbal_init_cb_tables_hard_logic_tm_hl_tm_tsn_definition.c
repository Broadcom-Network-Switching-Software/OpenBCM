
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * 
 */

shr_error_e
tsn_taf_gate_params_last_ti_entry_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__set;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SET, key_value__set);
    *offset = key_value__set;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
tsn_taf_gate_params_last_ti_entry_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__taf_gate;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TAF_GATE, key_value__taf_gate);
    *offset = key_value__taf_gate;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
tsn_taf_gate_control_list_num_tick_periods_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ti_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TI_INDEX, key_value__ti_index);
    *offset = key_value__ti_index;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
tsn_tas_port_params_nof_ti_entries_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__tas_port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TAS_PORT, key_value__tas_port);
    *offset = key_value__tas_port;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
tsn_tas_port_control_list_num_tick_periods_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__tas_port;
    uint32 key_value__ti_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TAS_PORT, key_value__tas_port);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TI_INDEX, key_value__ti_index);
    *offset = key_value__tas_port*dnx_data_tsn.tas.max_time_intervals_entries_get(unit)+key_value__ti_index;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
tsn_cqf_port_params_nof_ti_entries_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__cqf_port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CQF_PORT, key_value__cqf_port);
    *offset = key_value__cqf_port;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
tsn_cqf_port_control_list_num_tick_periods_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__cqf_port;
    uint32 key_value__ti_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CQF_PORT, key_value__cqf_port);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TI_INDEX, key_value__ti_index);
    *offset = key_value__cqf_port*dnx_data_tsn.cqf.max_time_intervals_entries_get(unit)+key_value__ti_index;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
