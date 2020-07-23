
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
egq_shaper_global_configuration_ifc_spr_dis_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = 1*instance_idx;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egq_tcg_attributes_wfq_tcg_dis_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__tcg_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TCG_INDEX, key_value__tcg_index);
    *offset = key_value__tcg_index-dnx_data_egr_queuing.params.nof_q_pairs_get(unit)/2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egq_tcg_attributes_wfq_tcg_dis_entryoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__tcg_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TCG_INDEX, key_value__tcg_index);
    *offset = key_value__tcg_index;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egq_shaper_otm_shaper_length_port_offset_high_a_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__calendar_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CALENDAR_ID, key_value__calendar_id);
    *offset = key_value__calendar_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egq_shaper_otm_calendar_crdt_table_null_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__calendar_id;
    uint32 key_value__calendar_set;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CALENDAR_ID, key_value__calendar_id);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CALENDAR_SET, key_value__calendar_set);
    *offset = key_value__calendar_id+key_value__calendar_set*dnx_data_egr_queuing.params.nof_calendars_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egq_pqp_nif_cal_pqp_nif_port_mux_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__set_select;
    uint32 key_value__slot_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SET_SELECT, key_value__set_select);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SLOT_ID, key_value__slot_id);
    *offset = dnx_data_egr_queuing.params.nif_cal_len_pqp_get(unit)*key_value__set_select+key_value__slot_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egq_fqp_nif_cal_fqp_nif_port_mux_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__set_select;
    uint32 key_value__slot_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SET_SELECT, key_value__set_select);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SLOT_ID, key_value__slot_id);
    *offset = dnx_data_egr_queuing.params.nif_cal_len_fqp_get(unit)*key_value__set_select+key_value__slot_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egq_fqp_nif_cal_fqp_nif_port_mux_entryoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__slot_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SLOT_ID, key_value__slot_id);
    *offset = key_value__slot_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egq_shaper_cal_cal_calendar_cal_indx_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__set_select;
    uint32 key_value__cal_slot_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SET_SELECT, key_value__set_select);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CAL_SLOT_ID, key_value__cal_slot_id);
    *offset = dnx_data_egr_queuing.params.cal_cal_len_get(unit)*key_value__set_select+key_value__cal_slot_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egq_shaping_tcg_weights_weight_of_tcg_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__port_scheduler;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PORT_SCHEDULER, key_value__port_scheduler);
    *offset = key_value__port_scheduler;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egq_shaping_qpair_tcg_map_prio_map_element_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = dnx_data_egr_queuing.params.nof_bits_in_nof_tcg_get(unit)*instance_idx;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egq_shaping_ps_mode_ps_mode_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__port_scheduler;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PORT_SCHEDULER, key_value__port_scheduler);
    *offset = key_value__port_scheduler*2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egq_shaping_tcg_credit_table_tcg_crdt_to_add_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__tcg_calendar;
    uint32 key_value__calendar_set;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TCG_CALENDAR, key_value__tcg_calendar);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CALENDAR_SET, key_value__calendar_set);
    *offset = key_value__tcg_calendar+key_value__calendar_set*dnx_data_egr_queuing.params.tcg_calendar_size_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egq_shaping_qp_credit_table_qpair_crdt_to_add_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__port_prio_calendar;
    uint32 key_value__calendar_set;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PORT_PRIO_CALENDAR, key_value__port_prio_calendar);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CALENDAR_SET, key_value__calendar_set);
    *offset = key_value__port_prio_calendar+key_value__calendar_set*dnx_data_egr_queuing.params.port_prio_calendar_size_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egq_shaping_otm_crdt_table_otm_crdt_to_add_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__chan_arb_calendar;
    uint32 key_value__calendar_set;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CHAN_ARB_CALENDAR, key_value__chan_arb_calendar);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CALENDAR_SET, key_value__calendar_set);
    *offset = key_value__chan_arb_calendar+key_value__calendar_set*dnx_data_egr_queuing.params.chan_arb_calendar_size_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egq_channelized_interface_attributes_ifc_is_channelized_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__egq_ch_if;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_EGQ_CH_IF, key_value__egq_ch_if);
    *offset = key_value__egq_ch_if-32;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egq_channelized_interface_attributes_ifc_is_channelized_entryoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__egq_ch_if;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_EGQ_CH_IF, key_value__egq_ch_if);
    *offset = key_value__egq_ch_if;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egq_interface_attributes_min_gap_lp_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__egq_if;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_EGQ_IF, key_value__egq_if);
    *offset = key_value__egq_if-32;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egq_interface_attributes_min_gap_lp_entryoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__egq_if;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_EGQ_IF, key_value__egq_if);
    *offset = key_value__egq_if-64;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egq_fqp_profile_attributes_txq_max_bytes_th_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__profile_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PROFILE_INDEX, key_value__profile_index);
    *offset = key_value__profile_index;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egq_tc_dp_map_null_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__sys_dp;
    uint32 key_value__sys_tc;
    uint32 key_value__is_egress_mc;
    uint32 key_value__map_profile;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SYS_DP, key_value__sys_dp);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SYS_TC, key_value__sys_tc);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_IS_EGRESS_MC, key_value__is_egress_mc);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_MAP_PROFILE, key_value__map_profile);
    *offset = key_value__sys_dp+key_value__sys_tc*4+key_value__is_egress_mc*32+key_value__map_profile*64;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egq_phantom_queues_if_config_threshold_profile_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__egq_if;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_EGQ_IF, key_value__egq_if);
    *offset = key_value__egq_if*2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egq_phantom_queues_thresholds_threshold_value_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__threshold_profile;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_THRESHOLD_PROFILE, key_value__threshold_profile);
    *offset = 27+key_value__threshold_profile*10;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
