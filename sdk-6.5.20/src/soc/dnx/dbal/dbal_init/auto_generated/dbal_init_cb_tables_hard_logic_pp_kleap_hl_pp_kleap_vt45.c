
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
kleap_vt45_ffc_instruction_vt45_ffc_instruction_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ffc_idx;
    
    uint32 result_size__vt45_ffc_instruction = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FFC_IDX, key_value__ffc_idx);
    
    result_size__vt45_ffc_instruction += 29;
    *offset = key_value__ffc_idx*result_size__vt45_ffc_instruction;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
kleap_vt45_ffc_instruction_vt45_ffc_instruction_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ffc_idx;
    
    uint32 result_size__vt45_ffc_instruction = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FFC_IDX, key_value__ffc_idx);
    
    result_size__vt45_ffc_instruction += 29;
    *offset = key_value__ffc_idx*result_size__vt45_ffc_instruction-2*result_size__vt45_ffc_instruction;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
kleap_vt45_ffc_instruction_vt45_ffc_instruction_dataoffset_2_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ffc_idx;
    
    uint32 result_size__vt45_ffc_instruction = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FFC_IDX, key_value__ffc_idx);
    
    result_size__vt45_ffc_instruction += 29;
    *offset = key_value__ffc_idx*result_size__vt45_ffc_instruction-6*result_size__vt45_ffc_instruction;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
kleap_vt45_ffc_instruction_vt45_ffc_instruction_dataoffset_3_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ffc_idx;
    
    uint32 result_size__vt45_ffc_instruction = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FFC_IDX, key_value__ffc_idx);
    
    result_size__vt45_ffc_instruction += 29;
    *offset = key_value__ffc_idx*result_size__vt45_ffc_instruction-10*result_size__vt45_ffc_instruction;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
kleap_vt45_ffc_instruction_vt45_ffc_instruction_dataoffset_4_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ffc_idx;
    
    uint32 result_size__vt45_ffc_instruction = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FFC_IDX, key_value__ffc_idx);
    
    result_size__vt45_ffc_instruction += 29;
    *offset = key_value__ffc_idx*result_size__vt45_ffc_instruction-14*result_size__vt45_ffc_instruction;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
kleap_vt45_ffc_instruction_vt45_ffc_instruction_dataoffset_5_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ffc_idx;
    
    uint32 result_size__vt45_ffc_instruction = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FFC_IDX, key_value__ffc_idx);
    
    result_size__vt45_ffc_instruction += 29;
    *offset = key_value__ffc_idx*result_size__vt45_ffc_instruction-18*result_size__vt45_ffc_instruction;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
kleap_vt45_pd_info_pd_strength_width_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__kbr_idx;
    
    uint32 result_size__pd_strength_width = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_KBR_IDX, key_value__kbr_idx);
    
    result_size__pd_strength_width += 2;
    *offset = key_value__kbr_idx*result_size__pd_strength_width;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
kleap_vt45_pd_info_pd_order_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__kbr_idx;
    
    uint32 result_size__pd_order = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_KBR_IDX, key_value__kbr_idx);
    
    result_size__pd_order += 3;
    *offset = key_value__kbr_idx*result_size__pd_order;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
kleap_vt45_pd_info_pd_const_strength_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__kbr_idx;
    
    uint32 result_size__pd_const_strength = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_KBR_IDX, key_value__kbr_idx);
    
    result_size__pd_const_strength += 4;
    *offset = key_value__kbr_idx*result_size__pd_const_strength;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
kleap_vt45_pd_info_pd_profile_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__kbr_idx;
    
    uint32 result_size__pd_profile = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_KBR_IDX, key_value__kbr_idx);
    
    result_size__pd_profile += 7;
    *offset = key_value__kbr_idx*result_size__pd_profile;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
