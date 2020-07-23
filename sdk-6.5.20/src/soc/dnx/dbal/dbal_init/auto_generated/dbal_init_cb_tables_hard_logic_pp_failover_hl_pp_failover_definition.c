
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
failover_ingress_path_select_protection_path_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__protection_pointer;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PROTECTION_POINTER, key_value__protection_pointer);
    *offset = key_value__protection_pointer/4;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
failover_ingress_path_select_protection_path_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__protection_pointer;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PROTECTION_POINTER, key_value__protection_pointer);
    *offset = key_value__protection_pointer%4;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
failover_egress_path_select_protection_path_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__protection_pointer;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PROTECTION_POINTER, key_value__protection_pointer);
    *offset = key_value__protection_pointer/32;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
failover_egress_path_select_protection_path_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__protection_pointer;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PROTECTION_POINTER, key_value__protection_pointer);
    *offset = key_value__protection_pointer%32;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
failover_fec_path_select_protection_path_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__protection_pointer;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PROTECTION_POINTER, key_value__protection_pointer);
    *offset = key_value__protection_pointer/16384;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
failover_fec_path_select_protection_path_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__protection_pointer;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PROTECTION_POINTER, key_value__protection_pointer);
    *offset = key_value__protection_pointer%16384/32;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
failover_fec_path_select_protection_path_arrayoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__protection_pointer;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PROTECTION_POINTER, key_value__protection_pointer);
    *offset = key_value__protection_pointer/16384+dnx_data_failover.path_select.nof_fec_path_select_banks_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
failover_fec_path_select_bank_config_hierarchy_level_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__bank_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_BANK_ID, key_value__bank_id);
    *offset = key_value__bank_id*2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
failover_facility_destination_status_enable_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__system_port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SYSTEM_PORT, key_value__system_port);
    *offset = key_value__system_port/8;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
failover_facility_destination_status_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__system_port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SYSTEM_PORT, key_value__system_port);
    *offset = key_value__system_port%8;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
