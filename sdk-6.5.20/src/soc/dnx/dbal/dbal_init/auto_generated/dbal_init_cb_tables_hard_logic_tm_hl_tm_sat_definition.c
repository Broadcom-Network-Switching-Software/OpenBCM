
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
sat_tx_config_credit_backpressure_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__sat_pipe_id;
    
    uint32 result_size__credit_backpressure = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SAT_PIPE_ID, key_value__sat_pipe_id);
    
    result_size__credit_backpressure += 7;
    *offset = key_value__sat_pipe_id*result_size__credit_backpressure;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sat_evc_pair_config_evc_pair_set_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__evc_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_EVC_ID, key_value__evc_id);
    *offset = key_value__evc_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sat_tx_enable_pkt_gen_enable_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__tx_flow_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TX_FLOW_ID, key_value__tx_flow_id);
    *offset = key_value__tx_flow_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
