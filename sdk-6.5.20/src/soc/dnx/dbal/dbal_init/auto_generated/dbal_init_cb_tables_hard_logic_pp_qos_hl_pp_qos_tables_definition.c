
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
qos_ingress_phb_profile_to_internal_map_id_qos_int_map_id_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    *offset = full_key_value%2048;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_ingress_phb_profile_to_internal_map_id_qos_int_map_id_dataoffset_0_cb(
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
    
    full_result_size += 8;
    *offset = full_key_value/2048*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_ingress_remark_profile_to_inter_map_id_qos_int_map_id_dataoffset_0_cb(
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
    
    full_result_size += 8;
    *offset = full_key_value/2048*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_ingress_phb_internal_map_entries_tc_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    *offset = full_key_value%4096;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_ingress_phb_internal_map_entries_tc_dataoffset_0_cb(
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
    
    full_result_size += 3 + 2;
    *offset = full_key_value/4096*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_ingress_remark_internal_map_entries_network_qos_dataoffset_0_cb(
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
    
    full_result_size += 8;
    *offset = full_key_value/4096*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_ingress_ecn_internal_map_entries_ecn_value_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__qos_int_map_key;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_QOS_INT_MAP_KEY, key_value__qos_int_map_key);
    *offset = key_value__qos_int_map_key;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_ingress_ecn_internal_map_entries_ecn_value_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__qos_int_map_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_QOS_INT_MAP_ID, key_value__qos_int_map_id);
    *offset = key_value__qos_int_map_id*3;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_ingress_init_context_selection_layer_types_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__qos_cam_priority;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_QOS_CAM_PRIORITY, key_value__qos_cam_priority);
    *offset = key_value__qos_cam_priority;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_ingress_ive_pcp_dei_explicit_map_out_dei_entryoffset_0_cb(
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
    
    full_result_size += 1 + 3;
    *offset = full_key_value/full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_ingress_ive_untag_map_outer_dei_dataoffset_0_cb(
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
    
    full_result_size += 1 + 3 + 1 + 3;
    *offset = full_key_value/256*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_ingress_ive_untag_map_outer_pcp_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    
    uint32 result_size__outer_dei = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 1 + 3 + 1 + 3;
    
    result_size__outer_dei += 1;
    *offset = full_key_value/256*full_result_size+result_size__outer_dei;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_ingress_ive_untag_map_inner_dei_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    
    uint32 result_size__outer_dei = 0;
    
    uint32 result_size__outer_pcp = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 1 + 3 + 1 + 3;
    
    result_size__outer_dei += 1;
    
    result_size__outer_pcp += 3;
    *offset = full_key_value/256*full_result_size+result_size__outer_dei+result_size__outer_pcp;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_ingress_ive_untag_map_inner_pcp_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    
    uint32 result_size__outer_dei = 0;
    
    uint32 result_size__outer_pcp = 0;
    
    uint32 result_size__inner_dei = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 1 + 3 + 1 + 3;
    
    result_size__outer_dei += 1;
    
    result_size__outer_pcp += 3;
    
    result_size__inner_dei += 1;
    *offset = full_key_value/256*full_result_size+result_size__outer_dei+result_size__outer_pcp+result_size__inner_dei;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_ingress_propag_profile_mapping_qos_propag_strength_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__propagation_prof;
    uint32 key_value__qos_ingress_app_type;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PROPAGATION_PROF, key_value__propagation_prof);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_QOS_INGRESS_APP_TYPE, key_value__qos_ingress_app_type);
    
    full_result_size += 2 + 2;
    *offset = key_value__propagation_prof*16+full_result_size*key_value__qos_ingress_app_type;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_ingress_propag_profile_mapping_qos_comp_strength_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__propagation_prof;
    uint32 key_value__qos_ingress_app_type;
    
    uint32 full_result_size = 0;
    
    uint32 result_size__qos_propag_strength = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PROPAGATION_PROF, key_value__propagation_prof);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_QOS_INGRESS_APP_TYPE, key_value__qos_ingress_app_type);
    
    full_result_size += 2 + 2;
    
    result_size__qos_propag_strength += 2;
    *offset = key_value__propagation_prof*16+full_result_size*key_value__qos_ingress_app_type+result_size__qos_propag_strength;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_egress_network_qos_initial_select_network_qos_initial_select_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__network_qos_initial_select = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__network_qos_initial_select += 2;
    *offset = full_key_value*result_size__network_qos_initial_select;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_egress_network_qos_initial_select_network_qos_initial_select_plus_1_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__network_qos_initial_select_plus_1 = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__network_qos_initial_select_plus_1 += 2;
    *offset = full_key_value*result_size__network_qos_initial_select_plus_1;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_egress_tc_nwk_qos_mapping_tc_nwk_qos_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__tc_nwk_qos = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__tc_nwk_qos += 8;
    *offset = full_key_value*result_size__tc_nwk_qos;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_ingress_nwk_qos_type_select_ingress_nwk_qos_type_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__ingress_nwk_qos_type = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__ingress_nwk_qos_type += 2;
    *offset = full_key_value*result_size__ingress_nwk_qos_type;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_fwd_layer_type_select_forward_type_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__forward_type = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__forward_type += 3;
    *offset = full_key_value*result_size__forward_type;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_fwd_layer_type_select_qos_layer_protocol_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__qos_layer_protocol = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__qos_layer_protocol += 3;
    *offset = full_key_value*result_size__qos_layer_protocol;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_fwd_layer_type_select_fwd_plus_1_layer_protocol_index_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__fwd_plus_1_layer_protocol_index = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__fwd_plus_1_layer_protocol_index += 2;
    *offset = full_key_value*result_size__fwd_plus_1_layer_protocol_index;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_egress_model_mapping_table_qos_dp_select_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__encap_qos_model;
    
    uint32 result_size__qos_dp_select = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENCAP_QOS_MODEL, key_value__encap_qos_model);
    
    result_size__qos_dp_select += 2;
    *offset = key_value__encap_qos_model*result_size__qos_dp_select;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_egress_model_mapping_table_ecn_is_eligible_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__encap_qos_model;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENCAP_QOS_MODEL, key_value__encap_qos_model);
    *offset = 16+key_value__encap_qos_model;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_egress_model_mapping_table_dp_is_preserve_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__encap_qos_model;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENCAP_QOS_MODEL, key_value__encap_qos_model);
    *offset = 24+key_value__encap_qos_model;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_egress_model_mapping_table_network_qos_is_preserve_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__encap_qos_model;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENCAP_QOS_MODEL, key_value__encap_qos_model);
    *offset = 32+key_value__encap_qos_model;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_egress_model_mapping_table_qos_model_type_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__encap_qos_model;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENCAP_QOS_MODEL, key_value__encap_qos_model);
    *offset = key_value__encap_qos_model;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_egress_model_mapping_table_fwd_plus_1_explicit_profile_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__fwd_plus_1_explicit_profile = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__fwd_plus_1_explicit_profile += 2;
    *offset = full_key_value*result_size__fwd_plus_1_explicit_profile;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_egress_protocol_profile_to_internal_map_id_initial_network_qos_int_map_id_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__initial_network_qos_int_map_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__initial_network_qos_int_map_id += 3;
    *offset = full_key_value*result_size__initial_network_qos_int_map_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_egress_fwd_header_remark_enabler_remark_or_preserve_dataoffset_0_cb(
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
    
    full_result_size += 1;
    *offset = full_key_value*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_egress_network_qos_mapping_packet_current_layer_network_qos_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__current_layer_network_qos = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__current_layer_network_qos += 8;
    *offset = full_key_value%16*result_size__current_layer_network_qos;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_egress_network_qos_mapping_next_layer_next_layer_network_qos_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__next_layer_network_qos = 0;
    
    uint32 result_size__next_layer_dp = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__next_layer_network_qos += 8;
    
    result_size__next_layer_dp += 2;
    *offset = full_key_value%16*result_size__next_layer_network_qos+full_key_value%16*result_size__next_layer_dp;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_egress_network_qos_mapping_next_layer_next_layer_dp_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__next_layer_network_qos = 0;
    
    uint32 result_size__next_layer_dp = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__next_layer_network_qos += 8;
    
    result_size__next_layer_dp += 2;
    *offset = full_key_value%16*result_size__next_layer_network_qos+full_key_value%16*result_size__next_layer_dp+result_size__next_layer_network_qos;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_egress_remark_profile_info_mapped_remark_profile_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 result_size__mapped_remark_profile = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    result_size__mapped_remark_profile += 5;
    *offset = full_key_value*result_size__mapped_remark_profile;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_egress_ttl_pipe_mapping_ttl_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ttl_pipe_profile;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TTL_PIPE_PROFILE, key_value__ttl_pipe_profile);
    
    full_result_size += 8;
    *offset = key_value__ttl_pipe_profile*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_egress_ttl_model_configuration_ttl_inheritance_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ttl_decrement_disable;
    uint32 key_value__in_ttl_mode;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TTL_DECREMENT_DISABLE, key_value__ttl_decrement_disable);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_IN_TTL_MODE, key_value__in_ttl_mode);
    *offset = key_value__ttl_decrement_disable*2+key_value__in_ttl_mode;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_egress_policer_offset_map_policer_offset_dataoffset_0_cb(
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
qos_egress_jr1_fhei_model_map_fhei_qos_model_dataoffset_0_cb(
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
    
    full_result_size += 1 + 1;
    *offset = full_key_value*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_egress_jr1_fhei_model_map_fhei_ttl_model_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    
    uint32 result_size__fhei_qos_model = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 1 + 1;
    
    result_size__fhei_qos_model += 1;
    *offset = full_key_value*full_result_size+result_size__fhei_qos_model;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_egress_cos_profile_table_tc_valid_dataoffset_0_cb(
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
    
    full_result_size += 1 + 3 + 1 + 2 + 1 + 8;
    *offset = full_key_value*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_egress_cos_profile_table_egress_tc_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    
    uint32 result_size__tc_valid = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 1 + 3 + 1 + 2 + 1 + 8;
    
    result_size__tc_valid += 1;
    *offset = full_key_value*full_result_size+result_size__tc_valid;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_egress_cos_profile_table_dp_valid_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    
    uint32 result_size__tc_valid = 0;
    
    uint32 result_size__egress_tc = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 1 + 3 + 1 + 2 + 1 + 8;
    
    result_size__tc_valid += 1;
    
    result_size__egress_tc += 3;
    *offset = full_key_value*full_result_size+result_size__tc_valid+result_size__egress_tc;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_egress_cos_profile_table_egress_dp_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    
    uint32 result_size__tc_valid = 0;
    
    uint32 result_size__egress_tc = 0;
    
    uint32 result_size__dp_valid = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 1 + 3 + 1 + 2 + 1 + 8;
    
    result_size__tc_valid += 1;
    
    result_size__egress_tc += 3;
    
    result_size__dp_valid += 1;
    *offset = full_key_value*full_result_size+result_size__tc_valid+result_size__egress_tc+result_size__dp_valid;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_egress_cos_profile_table_nwk_qos_valid_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    
    uint32 result_size__tc_valid = 0;
    
    uint32 result_size__egress_tc = 0;
    
    uint32 result_size__dp_valid = 0;
    
    uint32 result_size__egress_dp = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 1 + 3 + 1 + 2 + 1 + 8;
    
    result_size__tc_valid += 1;
    
    result_size__egress_tc += 3;
    
    result_size__dp_valid += 1;
    
    result_size__egress_dp += 2;
    *offset = full_key_value*full_result_size+result_size__tc_valid+result_size__egress_tc+result_size__dp_valid+result_size__egress_dp;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
qos_egress_cos_profile_table_egress_nwk_qos_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 full_key_value;
    
    uint32 full_result_size = 0;
    
    uint32 result_size__tc_valid = 0;
    
    uint32 result_size__egress_tc = 0;
    
    uint32 result_size__dp_valid = 0;
    
    uint32 result_size__egress_dp = 0;
    
    uint32 result_size__nwk_qos_valid = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, eh, full_key_value);
    
    full_result_size += 1 + 3 + 1 + 2 + 1 + 8;
    
    result_size__tc_valid += 1;
    
    result_size__egress_tc += 3;
    
    result_size__dp_valid += 1;
    
    result_size__egress_dp += 2;
    
    result_size__nwk_qos_valid += 1;
    *offset = full_key_value*full_result_size+result_size__tc_valid+result_size__egress_tc+result_size__dp_valid+result_size__egress_dp+result_size__nwk_qos_valid;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
