
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
trap_fwd_rcy_cmd_map_ingress_trap_id_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__recycle_command_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_RECYCLE_COMMAND_ID, key_value__recycle_command_id);
    *offset = key_value__recycle_command_id*13;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
trap_fwd_rcy_cmd_map_trap_fwd_strength_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__recycle_command_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_RECYCLE_COMMAND_ID, key_value__recycle_command_id);
    *offset = key_value__recycle_command_id*13+9;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
trap_ingress_lif_action_profile_ingress_trap_id_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__action_profile_idx;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ACTION_PROFILE_IDX, key_value__action_profile_idx);
    *offset = key_value__action_profile_idx*16;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
trap_ingress_lif_action_profile_trap_fwd_strength_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__action_profile_idx;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ACTION_PROFILE_IDX, key_value__action_profile_idx);
    *offset = key_value__action_profile_idx*16+9;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
trap_ingress_lif_action_profile_trap_snp_strength_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__action_profile_idx;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ACTION_PROFILE_IDX, key_value__action_profile_idx);
    *offset = key_value__action_profile_idx*16+13;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
trap_ingress_ethernet_layer_protocol_map_action_profile_idx_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    *offset = full_key_value*3;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
