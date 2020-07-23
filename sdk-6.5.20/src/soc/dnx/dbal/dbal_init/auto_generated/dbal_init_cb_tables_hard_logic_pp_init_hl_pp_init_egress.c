
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
egress_selected_global_outlifs_outlif_0_decision_dataoffset_0_cb(
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
    
    full_result_size += 1 + 2 + 2 + 2;
    *offset = full_key_value*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egress_selected_global_outlifs_outlif_1_decision_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    
    uint32 result_size__outlif_0_decision = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 1 + 2 + 2 + 2;
    
    result_size__outlif_0_decision += 1;
    *offset = full_key_value*full_result_size+result_size__outlif_0_decision;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egress_selected_global_outlifs_outlif_2_decision_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    
    uint32 result_size__outlif_0_decision = 0;
    
    uint32 result_size__outlif_1_decision = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 1 + 2 + 2 + 2;
    
    result_size__outlif_0_decision += 1;
    
    result_size__outlif_1_decision += 2;
    *offset = full_key_value*full_result_size+result_size__outlif_0_decision+result_size__outlif_1_decision;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egress_selected_global_outlifs_outlif_3_decision_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    
    uint32 result_size__outlif_0_decision = 0;
    
    uint32 result_size__outlif_1_decision = 0;
    
    uint32 result_size__outlif_2_decision = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 1 + 2 + 2 + 2;
    
    result_size__outlif_0_decision += 1;
    
    result_size__outlif_1_decision += 2;
    
    result_size__outlif_2_decision += 2;
    *offset = full_key_value*full_result_size+result_size__outlif_0_decision+result_size__outlif_1_decision+result_size__outlif_2_decision;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egress_system_header_generation_table_bytes_to_add_dataoffset_0_cb(
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
    
    full_result_size += 6;
    *offset = full_key_value*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egress_global_inlif_resolution_inlif_decision_0_dataoffset_0_cb(
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
    
    full_result_size += 1 + 1;
    *offset = full_key_value*full_result_size+full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egress_global_inlif_resolution_inlif_decision_0_dataoffset_1_cb(
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
    
    full_result_size += 1 + 1;
    *offset = full_key_value*full_result_size-full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egress_global_inlif_resolution_inlif_decision_0_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    *offset = full_key_value+1;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egress_global_inlif_resolution_inlif_decision_0_entryoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    *offset = full_key_value-1;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egress_global_inlif_resolution_inlif_decision_1_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    
    uint32 result_size__inlif_decision_0 = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 1 + 1;
    
    result_size__inlif_decision_0 += 1;
    *offset = full_key_value*full_result_size+full_result_size+result_size__inlif_decision_0;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egress_global_inlif_resolution_inlif_decision_1_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    
    uint32 result_size__inlif_decision_0 = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 1 + 1;
    
    result_size__inlif_decision_0 += 1;
    *offset = full_key_value*full_result_size-full_result_size+result_size__inlif_decision_0;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egress_forwarding_additional_info_fwd_plus_1_remark_ingress_profile_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__fwd_plus_1_remark_ingress_profile = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__fwd_plus_1_remark_ingress_profile += 2;
    *offset = full_key_value*result_size__fwd_plus_1_remark_ingress_profile;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egress_same_interface_filter_same_interface_filter_enable_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    *offset = full_key_value/32;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egress_same_interface_filter_same_interface_filter_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    *offset = full_key_value%32;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
