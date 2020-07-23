
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
latency_voq_quartet_profile_profile_id_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__quartet_queue;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_QUARTET_QUEUE, key_value__quartet_queue);
    *offset = key_value__quartet_queue/64;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
latency_voq_quartet_profile_profile_id_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__quartet_queue;
    
    uint32 result_size__profile_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_QUARTET_QUEUE, key_value__quartet_queue);
    
    result_size__profile_id += 3;
    *offset = key_value__quartet_queue%64*result_size__profile_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
latency_voq_quartet_profile_map_lat_flow_profile_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__quartet_profile;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_QUARTET_PROFILE, key_value__quartet_profile);
    *offset = key_value__quartet_profile*6;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
latency_voq_quartet_profile_map_tc_map_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__quartet_profile;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_QUARTET_PROFILE, key_value__quartet_profile);
    *offset = key_value__quartet_profile*6+3;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
latency_egress_l4q_profile_null_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__l4q_flow_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_L4Q_FLOW_ID, key_value__l4q_flow_id);
    *offset = key_value__l4q_flow_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
latency_egress_l4q_map_aqm_flow_id_port_tc_l4q_flow_id_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__tc;
    
    uint32 result_size__l4q_flow_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TC, key_value__tc);
    
    result_size__l4q_flow_id += utilex_log2_round_up(dnx_data_latency.aqm.flows_nof_get(unit));
    *offset = key_value__tc*result_size__l4q_flow_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
latency_egress_packet_classification_cfg_de_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ecn_profile;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ECN_PROFILE, key_value__ecn_profile);
    *offset = key_value__ecn_profile;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
latency_egress_maintain_average_latency_use_new_avg_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__pkt_type_key;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PKT_TYPE_KEY, key_value__pkt_type_key);
    *offset = key_value__pkt_type_key*2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
latency_egress_maintain_average_latency_use_new_inst_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__pkt_type_key;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PKT_TYPE_KEY, key_value__pkt_type_key);
    *offset = key_value__pkt_type_key*2+1;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
