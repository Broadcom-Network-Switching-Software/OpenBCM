
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
egress_oam_process_map_oam_process_action_dataoffset_0_cb(
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
    
    full_result_size += 3;
    *offset = full_key_value*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ingress_oam_sub_type_to_data_type_map_ingress_oam_data_type_dataoffset_0_cb(
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
    
    full_result_size += 2;
    *offset = full_key_value*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
oamp_interrupt_report_jr2_stat_event_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    
    uint32 result_size__jr2_stat_event = 0;
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    
    result_size__jr2_stat_event += 32;
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = instance_idx*result_size__jr2_stat_event;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
oamp_interrupt_data_jr2_oamp_event_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    
    uint32 result_size__jr2_oamp_event = 0;
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    
    result_size__jr2_oamp_event += 48;
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = instance_idx*result_size__jr2_oamp_event;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
oamp_interrupt_data_jr2_oamp_event_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    
    uint32 result_size__jr2_oamp_event = 0;
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    
    result_size__jr2_oamp_event += 48;
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = instance_idx*result_size__jr2_oamp_event+16;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
