
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * 
 */

shr_error_e
esb_per_nif_if_credits_config_txi_init_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__nif_if;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_NIF_IF, key_value__nif_if);
    *offset = key_value__nif_if;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
esb_egq_if_2_q_map_q_num_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__egress_if;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_EGRESS_IF, key_value__egress_if);
    *offset = key_value__egress_if-dnx_data_egr_queuing.params.egr_if_nif_base_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
esb_q_2_egq_if_map_port_macro_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__esb_q_num;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ESB_Q_NUM, key_value__esb_q_num);
    *offset = key_value__esb_q_num;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
esb_queue_muxes_esb_q_num_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__slot_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SLOT_ID, key_value__slot_id);
    *offset = key_value__slot_id/dnx_data_esb.general.mux_cal_window_size_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
esb_queue_muxes_esb_q_num_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__slot_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SLOT_ID, key_value__slot_id);
    *offset = (key_value__slot_id%dnx_data_esb.general.mux_cal_window_size_get(unit))*9;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
esb_ilkn_tdm_queue_en_tdm_queue_id_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ilkn_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ILKN_ID, key_value__ilkn_id);
    *offset = key_value__ilkn_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
esb_sar_config_is_sar_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__sar_queue_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SAR_QUEUE_ID, key_value__sar_queue_id);
    *offset = key_value__sar_queue_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
