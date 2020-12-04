
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

shr_error_e
hbmc_pll_control_hbm_pll_resetb_blockindex_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__hbmc_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_HBMC_INDEX, key_value__hbmc_index);
    *offset = key_value__hbmc_index;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
hbmc_phy_channel_control_hbm_phy_ch_reset_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__channel_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CHANNEL_INDEX, key_value__channel_index);
    *offset = key_value__channel_index;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
hcc_enable_dynamic_memory_access_enable_dynamic_memory_access_blockindex_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__hbmc_index;
    uint32 key_value__channel;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_HBMC_INDEX, key_value__hbmc_index);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CHANNEL, key_value__channel);
    *offset = dnx_data_dram.general_info.nof_channels_get(unit)*key_value__hbmc_index+key_value__channel;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
tdu_counters_write_request_32_byte_count_blockindex_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__core_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CORE_ID, key_value__core_id);
    *offset = dnx_data_dram.address_translation.nof_tdus_per_dram_get(unit)*key_value__core_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
tdu_data_source_id_data_source_id_blockindex_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__global_hbmc_index;
    uint32 key_value__hbmc_tdu_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_GLOBAL_HBMC_INDEX, key_value__global_hbmc_index);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_HBMC_TDU_INDEX, key_value__hbmc_tdu_index);
    *offset = dnx_data_dram.address_translation.nof_tdus_per_dram_get(unit)*key_value__global_hbmc_index+key_value__hbmc_tdu_index;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
hrc_config_read_request_path_enable_blockindex_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__global_hbmc_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_GLOBAL_HBMC_INDEX, key_value__global_hbmc_index);
    *offset = key_value__global_hbmc_index;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
hcc_parity_err_counters_write_parity_dword_err_cnt_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    
    uint32 result_size__write_parity_dword_err_cnt = 0;
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    
    result_size__write_parity_dword_err_cnt += 40;
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = 40+result_size__write_parity_dword_err_cnt*instance_idx;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
hcc_parity_err_counters_read_parity_dword_err_cnt_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    
    uint32 result_size__read_parity_dword_err_cnt = 0;
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    
    result_size__read_parity_dword_err_cnt += 40;
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = 200+result_size__read_parity_dword_err_cnt*instance_idx;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
hcc_hbm_last_returned_data_last_returned_data_blockindex_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__hbmc_index;
    uint32 key_value__channel;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_HBMC_INDEX, key_value__hbmc_index);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CHANNEL, key_value__channel);
    *offset = key_value__hbmc_index*dnx_data_dram.general_info.nof_channels_get(unit)+key_value__channel;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
hbmc_phy_firmware_general_access_data_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__address;
    uint32 key_value__reg_address;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ADDRESS, key_value__address);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_REG_ADDRESS, key_value__reg_address);
    *offset = key_value__address*0x100+key_value__reg_address;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
