
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
ptp_1588_egress_tp_command_tx_stamp_cf_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__tp_command;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TP_COMMAND, key_value__tp_command);
    *offset = key_value__tp_command;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ptp_1588_port_properties_ingress_p2p_delay_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PORT, key_value__port);
    *offset = key_value__port;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ptp_1588_port_properties_ieee_1588_mac_enable_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PORT, key_value__port);
    *offset = key_value__port/256;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ptp_1588_port_properties_ieee_1588_mac_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PORT, key_value__port);
    *offset = key_value__port%256;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ptp_1588_action_update_time_stamp_compensation_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    *offset = full_key_value%256;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ptp_1588_action_update_time_stamp_compensation_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__port_profile;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PORT_PROFILE, key_value__port_profile);
    *offset = key_value__port_profile*6;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ptp_1588_action_update_time_stamp_compensation_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__port_profile;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PORT_PROFILE, key_value__port_profile);
    *offset = key_value__port_profile*9;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
