
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
oamp_mpls_lm_dm_filter_ignore_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    *offset = full_key_value>>2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
oamp_mpls_lm_dm_filter_ignore_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__mpls_lm_dm_control_code;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_MPLS_LM_DM_CONTROL_CODE, key_value__mpls_lm_dm_control_code);
    *offset = (key_value__mpls_lm_dm_control_code&3)*2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
oamp_mpls_lm_dm_filter_punt_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__mpls_lm_dm_control_code;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_MPLS_LM_DM_CONTROL_CODE, key_value__mpls_lm_dm_control_code);
    *offset = ((key_value__mpls_lm_dm_control_code&3)*2)+1;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
