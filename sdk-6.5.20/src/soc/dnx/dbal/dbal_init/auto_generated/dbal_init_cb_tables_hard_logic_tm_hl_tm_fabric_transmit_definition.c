
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
fabric_transmit_dtq_config_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__subdtq_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SUBDTQ_ID, key_value__subdtq_id);
    *offset = key_value__subdtq_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_transmit_dtq_fc_thresholds_threshold_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fabric_dtq_fc;
    
    uint32 result_size__threshold = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FABRIC_DTQ_FC, key_value__fabric_dtq_fc);
    
    result_size__threshold += dnx_data_fabric.dbal.fabric_transmit_dtq_fc_thresholds_bits_nof_get(unit);
    *offset = key_value__fabric_dtq_fc*result_size__threshold;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_transmit_dtq_ocb_fc_thresholds_threshold_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fabric_dtq_ocb_fc;
    
    uint32 result_size__threshold = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FABRIC_DTQ_OCB_FC, key_value__fabric_dtq_ocb_fc);
    
    result_size__threshold += 12;
    *offset = key_value__fabric_dtq_ocb_fc*result_size__threshold;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_transmit_pdq_config_pdq_sram_size_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fabric_pdq_context;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FABRIC_PDQ_CONTEXT, key_value__fabric_pdq_context);
    *offset = key_value__fabric_pdq_context;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_transmit_pdq_to_dqcq_fc_select_pdq_sram_fc_th_select_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__priority;
    
    uint32 result_size__pdq_sram_fc_th_select = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PRIORITY, key_value__priority);
    
    result_size__pdq_sram_fc_th_select += 4;
    *offset = key_value__priority*result_size__pdq_sram_fc_th_select;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_transmit_pdq_to_dqcq_fc_select_pdq_mixs_fc_th_select_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__priority;
    
    uint32 result_size__pdq_mixs_fc_th_select = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PRIORITY, key_value__priority);
    
    result_size__pdq_mixs_fc_th_select += 4;
    *offset = key_value__priority*result_size__pdq_mixs_fc_th_select;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_transmit_pdq_to_dqcq_fc_select_pdq_mixd_fc_th_select_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__priority;
    
    uint32 result_size__pdq_mixd_fc_th_select = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PRIORITY, key_value__priority);
    
    result_size__pdq_mixd_fc_th_select += 4;
    *offset = key_value__priority*result_size__pdq_mixd_fc_th_select;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_transmit_pdq_to_dqcq_fc_select_pdq_mix_mixs_fc_th_select_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__priority;
    
    uint32 result_size__pdq_mix_mixs_fc_th_select = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PRIORITY, key_value__priority);
    
    result_size__pdq_mix_mixs_fc_th_select += 4;
    *offset = key_value__priority*result_size__pdq_mix_mixs_fc_th_select;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_transmit_pdq_to_dqcq_fc_select_pdq_mix_mixd_fc_th_select_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__priority;
    
    uint32 result_size__pdq_mix_mixd_fc_th_select = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PRIORITY, key_value__priority);
    
    result_size__pdq_mix_mixd_fc_th_select += 4;
    *offset = key_value__priority*result_size__pdq_mix_mixd_fc_th_select;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_transmit_pdq_to_dqcq_fc_th_threshold_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_INDEX, key_value__index);
    *offset = key_value__index;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_transmit_pdq_ocb_config_pdq_ocb_size_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fabric_pdq_ocb_context;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FABRIC_PDQ_OCB_CONTEXT, key_value__fabric_pdq_ocb_context);
    *offset = key_value__fabric_pdq_ocb_context;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_transmit_pdq_to_dqcq_sram_to_dram_fc_select_pdq_sram_to_dram_fc_th_select_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fabric_pdq_s2d_context;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FABRIC_PDQ_S2D_CONTEXT, key_value__fabric_pdq_s2d_context);
    *offset = key_value__fabric_pdq_s2d_context;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_transmit_pdq_to_dqcq_sram_to_dram_fc_select_pdq_sram_to_dram_fc_th_select_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__priority;
    
    uint32 result_size__pdq_sram_to_dram_fc_th_select = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PRIORITY, key_value__priority);
    
    result_size__pdq_sram_to_dram_fc_th_select += 4;
    *offset = key_value__priority*result_size__pdq_sram_to_dram_fc_th_select;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_transmit_pdq_dtq_fc_enable_local_0_dtq_fc_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fabric_dtq_fc;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FABRIC_DTQ_FC, key_value__fabric_dtq_fc);
    *offset = key_value__fabric_dtq_fc;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fabric_transmit_wfq_config_default_packet_size_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__wfq_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_WFQ_INDEX, key_value__wfq_index);
    *offset = key_value__wfq_index;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
