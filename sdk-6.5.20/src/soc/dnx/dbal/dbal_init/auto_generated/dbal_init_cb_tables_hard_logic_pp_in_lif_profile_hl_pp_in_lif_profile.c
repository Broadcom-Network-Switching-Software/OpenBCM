
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
in_lif_profile_table_l_2_cp_trap_handling_profile_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__in_lif_profile;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_IN_LIF_PROFILE, key_value__in_lif_profile);
    *offset = key_value__in_lif_profile+dnx_data_trap.ingress.protocol_traps_lif_profile_starting_offset_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
in_lif_profile_table_cs_in_lif_vtt_profile_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__in_lif_profile;
    
    uint32 result_size__cs_in_lif_vtt_profile = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_IN_LIF_PROFILE, key_value__in_lif_profile);
    
    result_size__cs_in_lif_vtt_profile += 4;
    *offset = key_value__in_lif_profile%128*result_size__cs_in_lif_vtt_profile;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
in_lif_profile_table_cs_in_lif_fwd_profile_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__in_lif_profile;
    
    uint32 result_size__cs_in_lif_fwd_profile = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_IN_LIF_PROFILE, key_value__in_lif_profile);
    
    result_size__cs_in_lif_fwd_profile += 4;
    *offset = key_value__in_lif_profile%128*result_size__cs_in_lif_fwd_profile;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
in_lif_profile_table_oam_default_profile_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__in_lif_profile;
    
    uint32 result_size__oam_default_profile = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_IN_LIF_PROFILE, key_value__in_lif_profile);
    
    result_size__oam_default_profile += 2;
    *offset = key_value__in_lif_profile*result_size__oam_default_profile;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
in_lif_profile_table_jr_mode_in_lif_profile_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__in_lif_profile;
    
    uint32 result_size__jr_mode_in_lif_profile = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_IN_LIF_PROFILE, key_value__in_lif_profile);
    if (dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_in_lif_profile_iop_mode))
    {
        
        result_size__jr_mode_in_lif_profile += 2;
    }
    *offset = key_value__in_lif_profile*result_size__jr_mode_in_lif_profile;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
in_lif_profile_table_mc_bridge_fallback_allow_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__in_lif_profile;
    
    uint32 result_size__mc_bridge_fallback_allow = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_IN_LIF_PROFILE, key_value__in_lif_profile);
    if (dnx_data_lif.in_lif.mc_bridge_fallback_per_lif_profile_get(unit))
    {
        
        result_size__mc_bridge_fallback_allow += 1;
    }
    *offset = key_value__in_lif_profile*result_size__mc_bridge_fallback_allow;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
in_lif_profile_table_jr_in_lif_orientation_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__in_lif_profile;
    
    uint32 result_size__jr_in_lif_orientation = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_IN_LIF_PROFILE, key_value__in_lif_profile);
    if (!dnx_data_lif.feature.feature_get(unit, dnx_data_lif_feature_iop_mode_orientation_selection))
    {
        
        result_size__jr_in_lif_orientation += 2;
    }
    *offset = key_value__in_lif_profile*result_size__jr_in_lif_orientation;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
in_lif_profile_table_in_lif_lb_profile_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__in_lif_profile;
    
    uint32 result_size__in_lif_lb_profile = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_IN_LIF_PROFILE, key_value__in_lif_profile);
    if (dnx_data_switch.load_balancing.hashing_selection_per_layer_get(unit))
    {
        
        result_size__in_lif_lb_profile += 2;
    }
    *offset = key_value__in_lif_profile*result_size__in_lif_lb_profile;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
vsi_profile_table_routing_enablers_profile_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    *offset = full_key_value/4;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
vsi_profile_table_routing_enablers_profile_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__routing_enablers_profile = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__routing_enablers_profile += 5;
    *offset = full_key_value%4*result_size__routing_enablers_profile;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
vsi_profile_table_l2_v6_mc_fwd_type_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    *offset = full_key_value/8;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
vsi_profile_table_l2_v6_mc_fwd_type_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    *offset = full_key_value%8*4;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
vsi_profile_table_l2_v4_mc_fwd_type_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    *offset = full_key_value%8*4+1;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
jr_mode_inlif_profile_orientation_map_jr_in_lif_orientation_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__jr_mode_in_lif_profile;
    
    uint32 result_size__jr_in_lif_orientation = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_JR_MODE_IN_LIF_PROFILE, key_value__jr_mode_in_lif_profile);
    
    result_size__jr_in_lif_orientation += 2;
    *offset = key_value__jr_mode_in_lif_profile*result_size__jr_in_lif_orientation;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
