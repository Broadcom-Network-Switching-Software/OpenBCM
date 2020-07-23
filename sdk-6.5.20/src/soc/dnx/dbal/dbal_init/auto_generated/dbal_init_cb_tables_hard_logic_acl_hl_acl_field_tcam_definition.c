
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
field_tcam_access_profiles_key_size_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_access_profile_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_ACCESS_PROFILE_ID, key_value__field_access_profile_id);
    *offset = key_value__field_access_profile_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_tcam_access_profiles_banks_action_select_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_tcam_bank_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_TCAM_BANK_ID, key_value__field_tcam_bank_id);
    *offset = key_value__field_tcam_bank_id*2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_tcam_access_profiles_banks_pd_select_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_tcam_bank_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_TCAM_BANK_ID, key_value__field_tcam_bank_id);
    *offset = key_value__field_tcam_bank_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_tcam_tcam_action_hit_indication_tcam_entry_hit_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__tcam_bank_sram_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TCAM_BANK_SRAM_ID, key_value__tcam_bank_sram_id);
    *offset = key_value__tcam_bank_sram_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_tcam_tcam_action_hit_indication_tcam_entry_hit_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__tcam_bank_line_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TCAM_BANK_LINE_ID, key_value__tcam_bank_line_id);
    *offset = key_value__tcam_bank_line_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_tcam_tcam_action_hit_indication_tcam_entry_hit_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__tcam_entry_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TCAM_ENTRY_ID, key_value__tcam_entry_id);
    *offset = key_value__tcam_entry_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
