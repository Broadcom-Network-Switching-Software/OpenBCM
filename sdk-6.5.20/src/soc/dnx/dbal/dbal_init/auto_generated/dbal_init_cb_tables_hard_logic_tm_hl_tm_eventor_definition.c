
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
eventor_tx_eng_cfg_padding_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__eventor_tx_engine_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_EVENTOR_TX_ENGINE_ID, key_value__eventor_tx_engine_id);
    *offset = key_value__eventor_tx_engine_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
eventor_tx_context_queue_type_is_standard_queue_type_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__eventor_context_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_EVENTOR_CONTEXT_ID, key_value__eventor_context_id);
    *offset = key_value__eventor_context_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
eventor_tx_context_main_buffer_th_tx_buffer_th_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = instance_idx*9;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
eventor_builder_transmit_header_stack_transmit_header_stack_size_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__eventor_builder_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_EVENTOR_BUILDER_ID, key_value__eventor_builder_id);
    *offset = key_value__eventor_builder_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
eventor_builder_transmit_header_stack_transmit_header_stack_data_high_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__eventor_builder_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_EVENTOR_BUILDER_ID, key_value__eventor_builder_id);
    *offset = 2*key_value__eventor_builder_id+1;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
eventor_builder_transmit_header_stack_transmit_header_stack_data_low_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__eventor_builder_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_EVENTOR_BUILDER_ID, key_value__eventor_builder_id);
    *offset = 2*key_value__eventor_builder_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
eventor_bank_type_is_tx_bank_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__eventor_sram_bank_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_EVENTOR_SRAM_BANK_ID, key_value__eventor_sram_bank_id);
    *offset = key_value__eventor_sram_bank_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
eventor_space_regions_buffer_start_addr_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__is_tx_context;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_IS_TX_CONTEXT, key_value__is_tx_context);
    *offset = key_value__is_tx_context;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
eventor_space_regions_buffer_start_addr_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__eventor_context_id;
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_EVENTOR_CONTEXT_ID, key_value__eventor_context_id);
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = 2*key_value__eventor_context_id+instance_idx;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
eventor_context_properites_cmic_buffer_start_addr_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = instance_idx*32;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
