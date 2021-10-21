
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

shr_error_e
sram_packet_buffer_ocb_last_in_chain_is_last_in_chain_blockindex_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ocb_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_OCB_INDEX, key_value__ocb_index);
    *offset = key_value__ocb_index;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sram_packet_buffer_ocbm_per_bank_cfg_bank_cfg_start_pointer_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ocb_ring_id;
    uint32 key_value__ocb_bank_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_OCB_RING_ID, key_value__ocb_ring_id);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_OCB_BANK_ID, key_value__ocb_bank_id);
    *offset = dnx_data_spb.ocb.nof_banks_per_ring_get(unit) * key_value__ocb_ring_id + key_value__ocb_bank_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sram_packet_buffer_ocbm_free_buffers_status_register_dbg_ptr_stat_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ocb_ring_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_OCB_RING_ID, key_value__ocb_ring_id);
    *offset = key_value__ocb_ring_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
