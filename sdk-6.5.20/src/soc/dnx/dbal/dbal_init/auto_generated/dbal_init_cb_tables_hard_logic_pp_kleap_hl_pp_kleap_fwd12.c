
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
kleap_fwd12_resources_mapping_kbr_selector_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = instance_idx;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
kleap_fwd12_ffc_quad_is_acl_ffc_quad_is_acl_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__context_profile;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CONTEXT_PROFILE, key_value__context_profile);
    *offset = key_value__context_profile;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
kleap_fwd12_ffc_quad_is_acl_ffc_quad_is_acl_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ffc_quad_idx;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FFC_QUAD_IDX, key_value__ffc_quad_idx);
    *offset = key_value__ffc_quad_idx-2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
kleap_fwd12_ffc_quad_is_acl_ffc_quad_is_acl_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ffc_quad_idx;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FFC_QUAD_IDX, key_value__ffc_quad_idx);
    *offset = key_value__ffc_quad_idx-4;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
kleap_fwd12_ffc_quad_is_acl_ffc_quad_is_acl_dataoffset_2_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ffc_quad_idx;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FFC_QUAD_IDX, key_value__ffc_quad_idx);
    *offset = key_value__ffc_quad_idx-8;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
kleap_fwd12_kbr_info_kbr_is_acl_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fwd12_kbr_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FWD12_KBR_ID, key_value__fwd12_kbr_id);
    *offset = key_value__fwd12_kbr_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
kleap_fwd12_ffc_instruction_fwd12_ffc_instruction_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ffc_idx;
    
    uint32 result_size__fwd12_ffc_instruction = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FFC_IDX, key_value__ffc_idx);
    
    result_size__fwd12_ffc_instruction += 29;
    *offset = key_value__ffc_idx*result_size__fwd12_ffc_instruction;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
kleap_fwd12_ffc_instruction_fwd12_ffc_instruction_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ffc_idx;
    
    uint32 result_size__fwd12_ffc_instruction = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FFC_IDX, key_value__ffc_idx);
    
    result_size__fwd12_ffc_instruction += 29;
    *offset = key_value__ffc_idx*result_size__fwd12_ffc_instruction-2*result_size__fwd12_ffc_instruction;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
kleap_fwd12_ffc_instruction_fwd12_ffc_instruction_dataoffset_2_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ffc_idx;
    
    uint32 result_size__fwd12_ffc_instruction = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FFC_IDX, key_value__ffc_idx);
    
    result_size__fwd12_ffc_instruction += 29;
    *offset = key_value__ffc_idx*result_size__fwd12_ffc_instruction-6*result_size__fwd12_ffc_instruction;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
kleap_fwd12_ffc_instruction_fwd12_ffc_instruction_dataoffset_3_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ffc_idx;
    
    uint32 result_size__fwd12_ffc_instruction = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FFC_IDX, key_value__ffc_idx);
    
    result_size__fwd12_ffc_instruction += 29;
    *offset = key_value__ffc_idx*result_size__fwd12_ffc_instruction-10*result_size__fwd12_ffc_instruction;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
kleap_fwd12_ffc_instruction_fwd12_ffc_instruction_dataoffset_4_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ffc_idx;
    
    uint32 result_size__fwd12_ffc_instruction = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FFC_IDX, key_value__ffc_idx);
    
    result_size__fwd12_ffc_instruction += 29;
    *offset = key_value__ffc_idx*result_size__fwd12_ffc_instruction-14*result_size__fwd12_ffc_instruction;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
kleap_fwd12_ffc_instruction_fwd12_ffc_instruction_dataoffset_5_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ffc_idx;
    
    uint32 result_size__fwd12_ffc_instruction = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FFC_IDX, key_value__ffc_idx);
    
    result_size__fwd12_ffc_instruction += 29;
    *offset = key_value__ffc_idx*result_size__fwd12_ffc_instruction-18*result_size__fwd12_ffc_instruction;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
kleap_fwd12_ffc_instruction_fwd12_ffc_instruction_dataoffset_6_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ffc_idx;
    
    uint32 result_size__fwd12_ffc_instruction = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FFC_IDX, key_value__ffc_idx);
    
    result_size__fwd12_ffc_instruction += 29;
    *offset = key_value__ffc_idx*result_size__fwd12_ffc_instruction-22*result_size__fwd12_ffc_instruction;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
kleap_fwd12_ffc_instruction_fwd12_ffc_instruction_dataoffset_7_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ffc_idx;
    
    uint32 result_size__fwd12_ffc_instruction = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FFC_IDX, key_value__ffc_idx);
    
    result_size__fwd12_ffc_instruction += 29;
    *offset = key_value__ffc_idx*result_size__fwd12_ffc_instruction-26*result_size__fwd12_ffc_instruction;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
kleap_fwd12_ffc_instruction_fwd12_ffc_instruction_dataoffset_8_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ffc_idx;
    
    uint32 result_size__fwd12_ffc_instruction = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FFC_IDX, key_value__ffc_idx);
    
    result_size__fwd12_ffc_instruction += 29;
    *offset = key_value__ffc_idx*result_size__fwd12_ffc_instruction-30*result_size__fwd12_ffc_instruction;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
kleap_fwd12_ffc_instruction_fwd12_ffc_instruction_dataoffset_9_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ffc_idx;
    
    uint32 result_size__fwd12_ffc_instruction = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FFC_IDX, key_value__ffc_idx);
    
    result_size__fwd12_ffc_instruction += 29;
    *offset = key_value__ffc_idx*result_size__fwd12_ffc_instruction-34*result_size__fwd12_ffc_instruction;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
kleap_fwd12_pd_info_pd_strength_width_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fwd12_kbr_id;
    
    uint32 result_size__pd_strength_width = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FWD12_KBR_ID, key_value__fwd12_kbr_id);
    
    result_size__pd_strength_width += 2;
    *offset = key_value__fwd12_kbr_id*result_size__pd_strength_width;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
kleap_fwd12_pd_info_pd_order_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fwd12_kbr_id;
    
    uint32 result_size__pd_order = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FWD12_KBR_ID, key_value__fwd12_kbr_id);
    
    result_size__pd_order += 5;
    *offset = key_value__fwd12_kbr_id*result_size__pd_order;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
kleap_fwd12_pd_info_pd_const_strength_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fwd12_kbr_id;
    
    uint32 result_size__pd_const_strength = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FWD12_KBR_ID, key_value__fwd12_kbr_id);
    
    result_size__pd_const_strength += 4;
    *offset = key_value__fwd12_kbr_id*result_size__pd_const_strength;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
kleap_fwd12_pd_info_pd_profile_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fwd12_kbr_id;
    
    uint32 result_size__pd_profile = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FWD12_KBR_ID, key_value__fwd12_kbr_id);
    
    result_size__pd_profile += 7;
    *offset = key_value__fwd12_kbr_id*result_size__pd_profile;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
kleap_fwd2_context_profile_null_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__context;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CONTEXT, key_value__context);
    *offset = key_value__context;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
