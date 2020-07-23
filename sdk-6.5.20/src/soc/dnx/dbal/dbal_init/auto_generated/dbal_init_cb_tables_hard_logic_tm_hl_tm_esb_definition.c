
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

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
esb_egq_if_2_q_map_null_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__egq_if;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_EGQ_IF, key_value__egq_if);
    *offset = key_value__egq_if-dnx_data_egr_queuing.params.egr_if_nif_base_get(unit);
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
esb_ilkn_tdm_queue_en_null_entryoffset_0_cb(
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
esb_flex_port_mapping_dest_nif_port_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__nif_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_NIF_ID, key_value__nif_id);
    *offset = key_value__nif_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
