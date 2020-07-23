
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
meter_ing_database_memory_map_bank_id_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__database_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_DATABASE_ID, key_value__database_id);
    *offset = key_value__database_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
meter_ing_database_memory_map_bank_id_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_INDEX, key_value__index);
    *offset = key_value__index*dnx_data_meter.mem_mgmt.bank_id_size_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
meter_profile_set_for_small_engine_profile_id_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__is_egress;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_IS_EGRESS, key_value__is_egress);
    *offset = key_value__is_egress;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
meter_profile_set_for_small_engine_profile_id_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__meter_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_METER_ID, key_value__meter_id);
    *offset = key_value__meter_id/4;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
meter_profile_set_for_small_engine_profile_id_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__meter_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_METER_ID, key_value__meter_id);
    *offset = key_value__meter_id%4*10;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
meter_profile_set_for_big_engine_profile_id_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__bank_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_BANK_ID, key_value__bank_id);
    *offset = key_value__bank_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
meter_fwd_type_group_map_offset_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fwd_type;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FWD_TYPE, key_value__fwd_type);
    *offset = key_value__fwd_type*3;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
meter_tc_group_map_offset_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__tc;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TC, key_value__tc);
    *offset = key_value__tc*3;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
meter_tcsm_mant_config_mantissa_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__tc;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TC, key_value__tc);
    *offset = key_value__tc*8;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
meter_ing_profile_config_color_aware_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__database_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_DATABASE_ID, key_value__database_id);
    *offset = key_value__database_id+3;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
meter_egr_profile_config_color_aware_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__database_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_DATABASE_ID, key_value__database_id);
    *offset = key_value__database_id+2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
meter_protocol_type_is_eth_is_eth_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__prot_type;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PROT_TYPE, key_value__prot_type);
    *offset = key_value__prot_type;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
meter_statistic_10_map_meter_fwd_type_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__is_bc;
    uint32 key_value__is_mc;
    uint32 key_value__is_unknown;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_IS_BC, key_value__is_bc);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_IS_MC, key_value__is_mc);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_IS_UNKNOWN, key_value__is_unknown);
    *offset = key_value__is_bc*4*3+key_value__is_mc*2*3+key_value__is_unknown*3;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
meter_global_counters_red_color_counter_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__meter_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_METER_ID, key_value__meter_id);
    *offset = key_value__meter_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
