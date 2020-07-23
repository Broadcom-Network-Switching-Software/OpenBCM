
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
sch_region_odd_even_mode_odd_even_mode_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = instance_idx/32;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_region_odd_even_mode_odd_even_mode_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = instance_idx%32;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_shared_shaper_shared_shaper_mode_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__flow_octet_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FLOW_OCTET_ID, key_value__flow_octet_id);
    *offset = key_value__flow_octet_id/8-dnx_data_sch.flow.first_se_flow_id_get(unit)/64;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_shared_shaper_shared_shaper_mode_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__flow_octet_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FLOW_OCTET_ID, key_value__flow_octet_id);
    *offset = key_value__flow_octet_id%8*2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_scheduler_composite_enable_table_is_composite_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__flow_id_pair;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FLOW_ID_PAIR, key_value__flow_id_pair);
    *offset = key_value__flow_id_pair/16;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_scheduler_composite_enable_table_is_composite_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__flow_id_pair;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FLOW_ID_PAIR, key_value__flow_id_pair);
    *offset = key_value__flow_id_pair%16;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_flow_shaper_dynamic_table_token_count_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__sch_flow_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SCH_FLOW_ID, key_value__sch_flow_id);
    *offset = key_value__sch_flow_id%8;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_flow_shaper_dynamic_table_token_count_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__sch_flow_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SCH_FLOW_ID, key_value__sch_flow_id);
    *offset = key_value__sch_flow_id/8;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_shaper_descriptor_table_rate_man_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__sch_flow_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SCH_FLOW_ID, key_value__sch_flow_id);
    *offset = key_value__sch_flow_id%8*dnx_data_sch.dbal.flow_shaper_descr_bits_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_shaper_descriptor_table_rate_exp_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__sch_flow_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SCH_FLOW_ID, key_value__sch_flow_id);
    *offset = key_value__sch_flow_id%8*dnx_data_sch.dbal.flow_shaper_descr_bits_get(unit)+dnx_data_sch.dbal.flow_shaper_mant_bits_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_shaper_descriptor_table_max_burst_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__sch_flow_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SCH_FLOW_ID, key_value__sch_flow_id);
    *offset = key_value__sch_flow_id%8*dnx_data_sch.dbal.flow_shaper_descr_bits_get(unit)+dnx_data_sch.dbal.flow_shaper_mant_bits_get(unit)+4;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_shaper_descriptor_table_slow_rate_2_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__sch_flow_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SCH_FLOW_ID, key_value__sch_flow_id);
    *offset = key_value__sch_flow_id%8*dnx_data_sch.dbal.flow_shaper_descr_bits_get(unit)+dnx_data_sch.dbal.flow_shaper_mant_bits_get(unit)+4+9;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_slow_rate_table_rate_man_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__slow_level;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SLOW_LEVEL, key_value__slow_level);
    *offset = key_value__slow_level;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_flow_slow_status_flow_slow_status_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__sch_flow_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SCH_FLOW_ID, key_value__sch_flow_id);
    *offset = key_value__sch_flow_id%8*2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_bw_profile_biasing_probability_uninstall_threshold_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__rci_level;
    uint32 key_value__is_fabric;
    uint32 key_value__is_high_prio;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_RCI_LEVEL, key_value__rci_level);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_IS_FABRIC, key_value__is_fabric);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_IS_HIGH_PRIO, key_value__is_high_prio);
    *offset = key_value__rci_level+key_value__is_fabric*8+key_value__is_high_prio*16;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_flow_attr_is_slow_enable_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__sch_flow_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SCH_FLOW_ID, key_value__sch_flow_id);
    *offset = key_value__sch_flow_id/4;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_flow_attr_is_slow_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__sch_flow_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SCH_FLOW_ID, key_value__sch_flow_id);
    *offset = key_value__sch_flow_id%4;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_se_color_group_group_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__se_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SE_ID, key_value__se_id);
    *offset = key_value__se_id/8;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_se_color_group_group_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__se_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SE_ID, key_value__se_id);
    *offset = key_value__se_id%8*2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_se_color_group_group_entryoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__se_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SE_ID, key_value__se_id);
    *offset = key_value__se_id/8-dnx_data_sch.flow.hr_se_id_min_get(unit)/8;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_hr_is_port_is_port_hr_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__hr_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_HR_ID, key_value__hr_id);
    *offset = key_value__hr_id/8;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_hr_is_port_is_port_hr_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__hr_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_HR_ID, key_value__hr_id);
    *offset = key_value__hr_id%8;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_hr_force_fc_force_fc_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__hr_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_HR_ID, key_value__hr_id);
    *offset = key_value__hr_id/32;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_hr_force_fc_force_fc_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__hr_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_HR_ID, key_value__hr_id);
    *offset = key_value__hr_id%32;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_port_hr_to_tcg_tcg_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__hr_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_HR_ID, key_value__hr_id);
    *offset = key_value__hr_id%8*3;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_port_tcg_weight_valid_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ps_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PS_ID, key_value__ps_id);
    *offset = key_value__ps_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_port_tcg_weight_valid_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__tcg_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TCG_ID, key_value__tcg_id);
    *offset = key_value__tcg_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_port_tcg_weight_weight_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__tcg_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TCG_ID, key_value__tcg_id);
    *offset = key_value__tcg_id*10;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_port_tc_shaper_quanta_to_add_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__hr_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_HR_ID, key_value__hr_id);
    *offset = key_value__hr_id%8*dnx_data_sch.dbal.ps_shaper_bits_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_port_tc_shaper_max_burst_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__hr_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_HR_ID, key_value__hr_id);
    *offset = key_value__hr_id%8*dnx_data_sch.dbal.ps_shaper_bits_get(unit)+dnx_data_sch.dbal.ps_shaper_quanta_bits_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_port_tcg_shaper_quanta_to_add_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__global_tcg_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_GLOBAL_TCG_ID, key_value__global_tcg_id);
    *offset = key_value__global_tcg_id/8;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_port_tcg_shaper_quanta_to_add_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__global_tcg_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_GLOBAL_TCG_ID, key_value__global_tcg_id);
    *offset = key_value__global_tcg_id%8*dnx_data_sch.dbal.ps_shaper_bits_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_port_tcg_shaper_max_burst_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__global_tcg_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_GLOBAL_TCG_ID, key_value__global_tcg_id);
    *offset = key_value__global_tcg_id%8*dnx_data_sch.dbal.ps_shaper_bits_get(unit)+dnx_data_sch.dbal.ps_shaper_quanta_bits_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_port_priority_nof_ps_priorities_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ps_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PS_ID, key_value__ps_id);
    *offset = key_value__ps_id/32;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_port_priority_nof_ps_priorities_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ps_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PS_ID, key_value__ps_id);
    *offset = key_value__ps_id%32;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_interface_priority_nof_priority_propagation_priorities_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__if_group;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_IF_GROUP, key_value__if_group);
    *offset = key_value__if_group;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_interface_force_pause_force_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__sch_interface_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SCH_INTERFACE_ID, key_value__sch_interface_id);
    *offset = key_value__sch_interface_id/32;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_interface_force_pause_force_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__sch_interface_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SCH_INTERFACE_ID, key_value__sch_interface_id);
    *offset = key_value__sch_interface_id%32;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_egq_to_sch_if_map_sch_interface_id_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__egq_interface_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_EGQ_INTERFACE_ID, key_value__egq_interface_id);
    *offset = key_value__egq_interface_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_egq_to_sch_if_map_sch_interface_id_entryoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__egq_interface_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_EGQ_INTERFACE_ID, key_value__egq_interface_id);
    *offset = key_value__egq_interface_id+dnx_data_sch.interface.nof_sch_interfaces_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_non_channelized_if_shaper_credit_rate_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__sch_if_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SCH_IF_ID, key_value__sch_if_id);
    *offset = key_value__sch_if_id-dnx_data_sch.interface.nof_channelized_calendars_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_if_big_calendar_hr_id_groupoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__cal_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CAL_ID, key_value__cal_id);
    *offset = key_value__cal_id/2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_if_big_calendar_hr_id_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry;
    uint32 key_value__select;
    uint32 key_value__cal_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY, key_value__entry);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SELECT, key_value__select);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CAL_ID, key_value__cal_id);
    *offset = key_value__entry+key_value__select*1024+key_value__cal_id%2*2048;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_if_regular_calendar_hr_id_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry;
    uint32 key_value__select;
    uint32 key_value__cal_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY, key_value__entry);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SELECT, key_value__select);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CAL_ID, key_value__cal_id);
    *offset = key_value__entry+key_value__select*256+key_value__cal_id%2*512;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_qpair_to_hr_map_hr_id_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__qpair;
    uint32 key_value__egq_core;
    uint32 key_value__core_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_QPAIR, key_value__qpair);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_EGQ_CORE, key_value__egq_core);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CORE_ID, key_value__core_id);
    *offset = key_value__qpair/16+(key_value__egq_core^key_value__core_id)*dnx_data_egr_queuing.params.nof_q_pairs_get(unit)/16;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_qpair_to_hr_map_hr_id_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__qpair;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_QPAIR, key_value__qpair);
    *offset = key_value__qpair%16*(dnx_data_sch.dbal.hr_bits_get(unit)+1);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
sch_qpair_to_hr_map_valid_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__qpair;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_QPAIR, key_value__qpair);
    *offset = key_value__qpair%16*(dnx_data_sch.dbal.hr_bits_get(unit)+1)+dnx_data_sch.dbal.hr_bits_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
