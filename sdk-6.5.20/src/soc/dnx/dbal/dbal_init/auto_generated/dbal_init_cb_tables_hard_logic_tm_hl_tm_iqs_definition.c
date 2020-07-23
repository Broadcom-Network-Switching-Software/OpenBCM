
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
iqs_credit_request_profile_map_profile_id_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__queue_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_QUEUE_ID, key_value__queue_id);
    *offset = key_value__queue_id/8;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
iqs_credit_request_profile_map_profile_id_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__queue_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_QUEUE_ID, key_value__queue_id);
    *offset = key_value__queue_id%8*5;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
iqs_credit_request_profile_general_config_is_low_delay_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__profile_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PROFILE_ID, key_value__profile_id);
    *offset = key_value__profile_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
iqs_credit_request_profile_general_config_bw_level_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__profile_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PROFILE_ID, key_value__profile_id);
    *offset = key_value__profile_id*2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
iqs_queue_to_priority_map_priority_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__voq;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_VOQ, key_value__voq);
    *offset = key_value__voq/16;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
iqs_queue_to_priority_map_priority_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__voq;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_VOQ, key_value__voq);
    *offset = key_value__voq%16*3;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
iqs_credit_request_slow_factor_up_slow_factor_up_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__src_slow_level;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SRC_SLOW_LEVEL, key_value__src_slow_level);
    *offset = key_value__src_slow_level*12;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
iqs_credit_request_slow_factor_down_slow_factor_down_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__dest_slow_level;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_DEST_SLOW_LEVEL, key_value__dest_slow_level);
    *offset = key_value__dest_slow_level*12+84;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
iqs_credit_max_deq_cmds_max_deq_cmd_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__active_queues;
    uint32 key_value__bw_level;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ACTIVE_QUEUES, key_value__active_queues);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_BW_LEVEL, key_value__bw_level);
    *offset = key_value__active_queues*9+key_value__bw_level*36;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
iqs_credit_deq_params_sram_read_weight_profile_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__credit_balance_msb;
    uint32 key_value__bw_level;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CREDIT_BALANCE_MSB, key_value__credit_balance_msb);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_BW_LEVEL, key_value__bw_level);
    *offset = key_value__credit_balance_msb*4+key_value__bw_level;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
iqs_credit_deq_params_s2d_read_weight_profile_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__credit_balance_msb;
    uint32 key_value__bw_level;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CREDIT_BALANCE_MSB, key_value__credit_balance_msb);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_BW_LEVEL, key_value__bw_level);
    *offset = 128+key_value__credit_balance_msb*4+key_value__bw_level;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
iqs_credit_worth_groups_credit_worth_group_id_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fap_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FAP_ID, key_value__fap_id);
    *offset = key_value__fap_id%16;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
iqs_fsm_reorder_fap_mode_is_seq_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fap_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FAP_ID, key_value__fap_id);
    *offset = key_value__fap_id/8;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
iqs_fsm_reorder_fap_mode_is_seq_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fap_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FAP_ID, key_value__fap_id);
    *offset = key_value__fap_id%8;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
iqs_voq_state_info_queue_credit_request_state_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__voq;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_VOQ, key_value__voq);
    *offset = key_value__voq/(dnx_data_ipq.queues.nof_queues_get(unit)/2);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
iqs_voq_state_info_queue_credit_request_state_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__voq;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_VOQ, key_value__voq);
    *offset = key_value__voq%(dnx_data_ipq.queues.nof_queues_get(unit)/2);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
iqs_sram_to_fabric_crdt_lfsr_thr_lfsr_thr_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__bw_level;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_BW_LEVEL, key_value__bw_level);
    *offset = key_value__bw_level*17;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
iqs_sram_to_fabric_crdt_lfsr_thr_lfsr_mask_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__bw_level;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_BW_LEVEL, key_value__bw_level);
    *offset = key_value__bw_level*17+9;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
iqs_ocbo_dqcq_flow_control_status_ipt_dqcq_fc_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__dqcq_context;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_DQCQ_CONTEXT, key_value__dqcq_context);
    *offset = key_value__dqcq_context;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
iqs_dqcq_subcontexts_dqcq_flow_control_status_ipt_dqcq_fc_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__dqcq_context;
    uint32 key_value__priority;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_DQCQ_CONTEXT, key_value__dqcq_context);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PRIORITY, key_value__priority);
    *offset = key_value__dqcq_context*(dnx_data_iqs.dqcq.nof_priorities_get(unit))+key_value__priority;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
