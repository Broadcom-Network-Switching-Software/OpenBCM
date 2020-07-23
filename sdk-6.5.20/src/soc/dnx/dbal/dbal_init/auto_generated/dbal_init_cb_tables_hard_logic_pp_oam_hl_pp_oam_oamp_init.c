
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
oamp_init_mep_db_bank_cfg_scan_rate_bank_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__mep_db_bank;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_MEP_DB_BANK, key_value__mep_db_bank);
    *offset = key_value__mep_db_bank;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
oamp_init_oam_opcode_cfg_oam_opcode_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__opcode_type;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_OPCODE_TYPE, key_value__opcode_type);
    *offset = key_value__opcode_type;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
oamp_init_rmep_access_enable_cfg_lookup_enable_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__oam_opcode;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_OAM_OPCODE, key_value__oam_opcode);
    *offset = key_value__oam_opcode;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
oamp_init_rmep_access_enable_cfg_lookup_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__oamp_mep_type;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_OAMP_MEP_TYPE, key_value__oamp_mep_type);
    *offset = key_value__oamp_mep_type;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
oamp_slm_measurement_period_timer_null_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__slm_measurement_period_timer_profile_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SLM_MEASUREMENT_PERIOD_TIMER_PROFILE_INDEX, key_value__slm_measurement_period_timer_profile_index);
    *offset = key_value__slm_measurement_period_timer_profile_index;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
oamp_opcode_for_count_null_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__opcode_for_count_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_OPCODE_FOR_COUNT_INDEX, key_value__opcode_for_count_index);
    *offset = key_value__opcode_for_count_index;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
oamp_rmep_punt_good_timer_null_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__punt_good_profile;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PUNT_GOOD_PROFILE, key_value__punt_good_profile);
    *offset = key_value__punt_good_profile;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
