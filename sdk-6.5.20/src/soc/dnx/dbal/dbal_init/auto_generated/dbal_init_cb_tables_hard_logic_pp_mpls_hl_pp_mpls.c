
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
mpls_php_map_fwd_code_forward_code_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__is_mc;
    uint32 key_value__first_nibble_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_IS_MC, key_value__is_mc);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIRST_NIBBLE_INDEX, key_value__first_nibble_index);
    *offset = key_value__is_mc*4*6+key_value__first_nibble_index*6;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mpls_special_label_profile_map_mpls_special_label_profile_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += dnx_data_aod_sizes.AOD.sizes_get(unit, 146)->value;
    *offset = full_key_value*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mpls_special_label_profile_attributes_labels_to_terminate_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 2 + 1 + 1 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 10 + 2 + 1 + 5 + 1 + 1 + 1 + 1 + 1 + 8;
    *offset = full_key_value*full_result_size+0;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mpls_special_label_profile_attributes_special_before_tunnel_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 2 + 1 + 1 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 10 + 2 + 1 + 5 + 1 + 1 + 1 + 1 + 1 + 8;
    *offset = full_key_value*full_result_size+2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mpls_special_label_profile_attributes_special_after_tunnel_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 2 + 1 + 1 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 10 + 2 + 1 + 5 + 1 + 1 + 1 + 1 + 1 + 8;
    *offset = full_key_value*full_result_size+3;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mpls_special_label_profile_attributes_phb_compare_strength_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 2 + 1 + 1 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 10 + 2 + 1 + 5 + 1 + 1 + 1 + 1 + 1 + 8;
    *offset = full_key_value*full_result_size+4;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mpls_special_label_profile_attributes_remark_compare_strength_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 2 + 1 + 1 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 10 + 2 + 1 + 5 + 1 + 1 + 1 + 1 + 1 + 8;
    *offset = full_key_value*full_result_size+6;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mpls_special_label_profile_attributes_ecn_compare_strength_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 2 + 1 + 1 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 10 + 2 + 1 + 5 + 1 + 1 + 1 + 1 + 1 + 8;
    *offset = full_key_value*full_result_size+8;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mpls_special_label_profile_attributes_ttl_compare_strength_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 2 + 1 + 1 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 10 + 2 + 1 + 5 + 1 + 1 + 1 + 1 + 1 + 8;
    *offset = full_key_value*full_result_size+10;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mpls_special_label_profile_attributes_phb_propagation_strength_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 2 + 1 + 1 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 10 + 2 + 1 + 5 + 1 + 1 + 1 + 1 + 1 + 8;
    *offset = full_key_value*full_result_size+12;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mpls_special_label_profile_attributes_remark_propagation_strength_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 2 + 1 + 1 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 10 + 2 + 1 + 5 + 1 + 1 + 1 + 1 + 1 + 8;
    *offset = full_key_value*full_result_size+14;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mpls_special_label_profile_attributes_ecn_propagation_strength_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 2 + 1 + 1 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 10 + 2 + 1 + 5 + 1 + 1 + 1 + 1 + 1 + 8;
    *offset = full_key_value*full_result_size+16;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mpls_special_label_profile_attributes_ttl_propagation_strength_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 2 + 1 + 1 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 10 + 2 + 1 + 5 + 1 + 1 + 1 + 1 + 1 + 8;
    *offset = full_key_value*full_result_size+18;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mpls_special_label_profile_attributes_qos_mapping_profile_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 2 + 1 + 1 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 10 + 2 + 1 + 5 + 1 + 1 + 1 + 1 + 1 + 8;
    *offset = full_key_value*full_result_size+20;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mpls_special_label_profile_attributes_ecn_mapping_profile_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 2 + 1 + 1 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 10 + 2 + 1 + 5 + 1 + 1 + 1 + 1 + 1 + 8;
    *offset = full_key_value*full_result_size+30;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mpls_special_label_profile_attributes_force_parser_context_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 2 + 1 + 1 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 10 + 2 + 1 + 5 + 1 + 1 + 1 + 1 + 1 + 8;
    *offset = full_key_value*full_result_size+32;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mpls_special_label_profile_attributes_force_parser_context_value_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 2 + 1 + 1 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 10 + 2 + 1 + 5 + 1 + 1 + 1 + 1 + 1 + 8;
    *offset = full_key_value*full_result_size+33;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mpls_special_label_profile_attributes_force_has_cw_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 2 + 1 + 1 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 10 + 2 + 1 + 5 + 1 + 1 + 1 + 1 + 1 + 8;
    *offset = full_key_value*full_result_size+38;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mpls_special_label_profile_attributes_force_is_oam_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 2 + 1 + 1 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 10 + 2 + 1 + 5 + 1 + 1 + 1 + 1 + 1 + 8;
    *offset = full_key_value*full_result_size+39;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mpls_special_label_profile_attributes_check_bos_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 2 + 1 + 1 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 10 + 2 + 1 + 5 + 1 + 1 + 1 + 1 + 1 + 8;
    *offset = full_key_value*full_result_size+40;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mpls_special_label_profile_attributes_expected_bos_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 2 + 1 + 1 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 10 + 2 + 1 + 5 + 1 + 1 + 1 + 1 + 1 + 8;
    *offset = full_key_value*full_result_size+41;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mpls_special_label_profile_attributes_check_ttl_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 2 + 1 + 1 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 10 + 2 + 1 + 5 + 1 + 1 + 1 + 1 + 1 + 8;
    *offset = full_key_value*full_result_size+42;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mpls_special_label_profile_attributes_expected_ttl_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 2 + 1 + 1 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 10 + 2 + 1 + 5 + 1 + 1 + 1 + 1 + 1 + 8;
    *offset = full_key_value*full_result_size+43;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egress_mpls_fhei_to_ees_resolution_fhei_pop_ttl_model_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    
    uint32 result_size__fhei_pop_ttl_model = 0;
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    
    result_size__fhei_pop_ttl_model += 1;
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = instance_idx*result_size__fhei_pop_ttl_model;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egress_mpls_fhei_to_ees_resolution_fhei_pop_qos_model_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    
    uint32 result_size__fhei_pop_qos_model = 0;
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    
    result_size__fhei_pop_qos_model += 3;
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = instance_idx*result_size__fhei_pop_qos_model;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egress_mpls_fhei_to_ees_resolution_fhei_swap_remark_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = instance_idx*dnx_data_headers.feature.fhei_mpls_swap_remark_profile_size_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mpls_label_range_properties_mpls_qualifier_label_range_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    *offset = full_key_value*8;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
