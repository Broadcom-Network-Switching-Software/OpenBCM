
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
nif_ethu_tx_lane_ctrl_stop_data_blockindex_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__core_id;
    uint32 key_value__lane_in_core;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CORE_ID, key_value__core_id);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LANE_IN_CORE, key_value__lane_in_core);
    *offset = key_value__core_id*5+key_value__lane_in_core/8-1;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ethu_tx_lane_ctrl_stop_data_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__lane_in_core;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LANE_IN_CORE, key_value__lane_in_core);
    *offset = key_value__lane_in_core%8+40;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ethu_tx_lane_ctrl_stop_data_blockindex_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__core_id;
    uint32 key_value__lane_in_core;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CORE_ID, key_value__core_id);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LANE_IN_CORE, key_value__lane_in_core);
    *offset = key_value__core_id*(dnx_data_nif.eth.cdu_nof_per_core_get(unit)-1)+key_value__lane_in_core/8-1;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ethu_tx_lane_ctrl_stop_data_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__lane_in_core;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LANE_IN_CORE, key_value__lane_in_core);
    *offset = key_value__lane_in_core%8+16;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ethu_tx_lane_ctrl_stop_data_blockindex_2_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__lane_in_core;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LANE_IN_CORE, key_value__lane_in_core);
    *offset = key_value__lane_in_core/8;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ethu_tx_lane_ctrl_stop_data_dataoffset_2_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__lane_in_core;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LANE_IN_CORE, key_value__lane_in_core);
    *offset = key_value__lane_in_core%8;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ethu_tx_lane_ctrl_stop_data_blockindex_3_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__lane_in_core;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LANE_IN_CORE, key_value__lane_in_core);
    *offset = (key_value__lane_in_core-dnx_data_nif.eth.total_nof_cdu_lanes_in_device_get(unit))/dnx_data_nif.eth.nof_lanes_in_clu_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ethu_tx_lane_ctrl_stop_data_dataoffset_3_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__lane_in_core;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LANE_IN_CORE, key_value__lane_in_core);
    *offset = (key_value__lane_in_core-dnx_data_nif.eth.total_nof_cdu_lanes_in_device_get(unit))%dnx_data_nif.eth.nof_lanes_in_clu_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ethu_tx_lane_ctrl_flush_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__lane_in_core;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LANE_IN_CORE, key_value__lane_in_core);
    *offset = key_value__lane_in_core%8*2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ethu_tx_lane_ctrl_flush_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__lane_in_core;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LANE_IN_CORE, key_value__lane_in_core);
    *offset = ((key_value__lane_in_core-dnx_data_nif.eth.total_nof_cdu_lanes_in_device_get(unit))%dnx_data_nif.eth.nof_lanes_in_clu_get(unit))*2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ethu_tx_lane_ctrl_stop_egress_credits_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__lane_in_core;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LANE_IN_CORE, key_value__lane_in_core);
    *offset = key_value__lane_in_core%8*2+1;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ethu_tx_lane_ctrl_stop_egress_credits_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__lane_in_core;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LANE_IN_CORE, key_value__lane_in_core);
    *offset = ((key_value__lane_in_core-dnx_data_nif.eth.total_nof_cdu_lanes_in_device_get(unit))%dnx_data_nif.eth.nof_lanes_in_clu_get(unit))*2+1;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ethu_tx_lane_ctrl_nmg_reset_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__lane_in_core;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LANE_IN_CORE, key_value__lane_in_core);
    *offset = key_value__lane_in_core;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ethu_tx_lane_ctrl_credit_reset_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__lane_in_core;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LANE_IN_CORE, key_value__lane_in_core);
    *offset = 17+key_value__lane_in_core%8;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_cdu_tx_lane_ctrl_start_thr_blockindex_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__core_id;
    uint32 key_value__cdu;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CORE_ID, key_value__core_id);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CDU, key_value__cdu);
    *offset = key_value__core_id*5+key_value__cdu-1;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_cdu_tx_lane_ctrl_start_thr_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__lane_in_cdu;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LANE_IN_CDU, key_value__lane_in_cdu);
    *offset = key_value__lane_in_cdu*14;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_cdu_tx_lane_ctrl_start_thr_blockindex_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__core_id;
    uint32 key_value__cdu;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CORE_ID, key_value__core_id);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CDU, key_value__cdu);
    *offset = key_value__core_id*(dnx_data_nif.eth.cdu_nof_per_core_get(unit)-1)+key_value__cdu-1;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_cdu_tx_lane_ctrl_start_thr_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__lane_in_cdu;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LANE_IN_CDU, key_value__lane_in_cdu);
    *offset = key_value__lane_in_cdu*dnx_data_nif.dbal.cdu_tx_start_threshold_entry_size_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_cdu_tx_lane_ctrl_start_thr_blockindex_2_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__cdu;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CDU, key_value__cdu);
    *offset = key_value__cdu;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_clu_tx_lane_ctrl_start_thr_blockindex_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__clu;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CLU, key_value__clu);
    *offset = key_value__clu;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_clu_tx_lane_ctrl_start_thr_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__lane_in_clu;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LANE_IN_CLU, key_value__lane_in_clu);
    *offset = key_value__lane_in_clu*dnx_data_nif.dbal.clu_tx_start_threshold_entry_size_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_clu_tx_lane_ctrl_start_thr_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__lane_in_clu;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LANE_IN_CLU, key_value__lane_in_clu);
    *offset = key_value__lane_in_clu*dnx_data_nif.dbal.clu_tx_start_threshold_entry_size_get(unit)+10;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ethu_rx_rmc_ctrl_low_prio_weight_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__rmc;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_RMC, key_value__rmc);
    *offset = key_value__rmc+16;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ethu_rx_rmc_ctrl_low_prio_weight_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__rmc;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_RMC, key_value__rmc);
    *offset = key_value__rmc+32;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ethu_rx_rmc_ctrl_low_prio_weight_dataoffset_2_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__rmc;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_RMC, key_value__rmc);
    *offset = key_value__rmc+48;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__core_id;
    uint32 key_value__ethu;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CORE_ID, key_value__core_id);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ETHU, key_value__ethu);
    *offset = key_value__core_id*(dnx_data_nif.eth.cdu_nof_per_core_get(unit)-1)+key_value__ethu-1;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ethu_rx_rmc_ctrl_nmg_reset_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ethu;
    uint32 key_value__rmc;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ETHU, key_value__ethu);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_RMC, key_value__rmc);
    *offset = 16*key_value__ethu+key_value__rmc;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ethu_rx_rmc_ctrl_is_high_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__rmc;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_RMC, key_value__rmc);
    *offset = key_value__rmc+18;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ethu_rx_rmc_ctrl_start_tx_thr_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__rmc;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_RMC, key_value__rmc);
    *offset = key_value__rmc*15+61;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ethu_rx_lane_to_rmc_map_active_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__internal_lane;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_INTERNAL_LANE, key_value__internal_lane);
    *offset = key_value__internal_lane;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ethu_rx_priority_to_rmc_map_rmc_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__prd_priority;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PRD_PRIORITY, key_value__prd_priority);
    *offset = key_value__prd_priority*5;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ethu_rx_priority_to_rmc_map_rmc_valid_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__prd_priority;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PRD_PRIORITY, key_value__prd_priority);
    *offset = key_value__prd_priority*5+4;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_cdu_gear_shift_reset_gs_reset_groupoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__lane;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LANE, key_value__lane);
    *offset = key_value__lane/4;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_cdu_gear_shift_reset_gs_reset_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__lane;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LANE, key_value__lane);
    *offset = key_value__lane%4;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_cdu_gear_shift_reset_gs_reset_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__lane;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LANE, key_value__lane);
    *offset = key_value__lane%8;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_clu_gear_shift_reset_gs_reset_groupoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__lane_in_clu;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LANE_IN_CLU, key_value__lane_in_clu);
    *offset = key_value__lane_in_clu/4;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_clu_gear_shift_reset_gs_reset_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__lane_in_clu;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LANE_IN_CLU, key_value__lane_in_clu);
    *offset = key_value__lane_in_clu%4;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_cdu_pm_ctrl_eee_enable_blockindex_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__core_id;
    uint32 key_value__cdu;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CORE_ID, key_value__core_id);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CDU, key_value__cdu);
    *offset = key_value__core_id*(dnx_data_nif.eth.cdu_nof_per_core_get(unit))+key_value__cdu;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_scheduler_ctrl_sch_weight_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ethu;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ETHU, key_value__ethu);
    *offset = key_value__ethu+8;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_scheduler_ctrl_sch_weight_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ethu;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ETHU, key_value__ethu);
    *offset = key_value__ethu+16;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_scheduler_ctrl_sch_weight_dataoffset_2_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ethu;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ETHU, key_value__ethu);
    *offset = key_value__ethu+24;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_scheduler_ctrl_sch_weight_dataoffset_3_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ethu;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ETHU, key_value__ethu);
    *offset = key_value__ethu+32;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_scheduler_ctrl_sch_weight_dataoffset_4_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ethu;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ETHU, key_value__ethu);
    *offset = key_value__ethu+64;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_scheduler_ctrl_sch_weight_dataoffset_5_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ethu;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ETHU, key_value__ethu);
    *offset = key_value__ethu+96;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_scheduler_ctrl_sch_weight_dataoffset_6_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ethu;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ETHU, key_value__ethu);
    *offset = key_value__ethu+48;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ile_core_enablers_reset_blockindex_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ilkn_core_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ILKN_CORE_ID, key_value__ilkn_core_id);
    *offset = key_value__ilkn_core_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ile_status_is_latch_low_aligned_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ilkn_port_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ILKN_PORT_ID, key_value__ilkn_port_id);
    *offset = key_value__ilkn_port_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ile_lane_ctrl_bypass_if_enable_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__internal_lane;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_INTERNAL_LANE, key_value__internal_lane);
    *offset = key_value__internal_lane/4;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ile_lane_ctrl_bypass_if_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__internal_lane;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_INTERNAL_LANE, key_value__internal_lane);
    *offset = key_value__internal_lane%4;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ile_lane_ctrl_async_fifo_reset_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__internal_lane;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_INTERNAL_LANE, key_value__internal_lane);
    *offset = key_value__internal_lane%8;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ile_xpmd_ctrl_lane_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__lane_in_cdu;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LANE_IN_CDU, key_value__lane_in_cdu);
    *offset = key_value__lane_in_cdu;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ile_12lanes_mode_selectors_12lanes_mode_slector_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__lane;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LANE, key_value__lane);
    *offset = key_value__lane;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ile_cdu_selectors_facing_core_select_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__lane;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LANE, key_value__lane);
    *offset = key_value__lane+8;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ile_cdu_selectors_nif_fabric_select_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__lane;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LANE, key_value__lane);
    *offset = key_value__lane+16;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ile_cdu_selectors_nif_fabric_select_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__lane;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LANE, key_value__lane);
    *offset = key_value__lane+24;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ile_clu_pm_rx_selectors_non_facing_enable_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__pm;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PM, key_value__pm);
    *offset = key_value__pm;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ile_clu_enablers_tx_enabler_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__non_facing_lane;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_NON_FACING_LANE, key_value__non_facing_lane);
    *offset = key_value__non_facing_lane/4;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ile_clu_enablers_tx_enabler_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__non_facing_lane;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_NON_FACING_LANE, key_value__non_facing_lane);
    *offset = key_value__non_facing_lane%4;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ile_clu_enablers_rx_direction_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__non_facing_lane;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_NON_FACING_LANE, key_value__non_facing_lane);
    *offset = key_value__non_facing_lane;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_clu_port_profile_map_prd_profile_groupoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__lane_in_clu;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LANE_IN_CLU, key_value__lane_in_clu);
    *offset = key_value__lane_in_clu;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_feu_port_profile_map_prd_profile_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__client_channel;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CLIENT_CHANNEL, key_value__client_channel);
    *offset = key_value__client_channel;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ilu_enablers_enable_hard_stage_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__hrf;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_HRF, key_value__hrf);
    *offset = key_value__hrf;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_feu_enablers_enable_hard_stage_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__feu_rmc_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FEU_RMC_ID, key_value__feu_rmc_id);
    *offset = key_value__feu_rmc_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ethu_port_properties_outer_tag_size_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__prd_profile;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PRD_PROFILE, key_value__prd_profile);
    *offset = key_value__prd_profile;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ethu_tm_priority_map_priority_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__tc;
    uint32 key_value__dp;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TC, key_value__tc);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_DP, key_value__dp);
    *offset = key_value__tc*8+key_value__dp*2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ethu_tm_priority_map_priority_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__tc;
    uint32 key_value__dp;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TC, key_value__tc);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_DP, key_value__dp);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        
        full_result_size += 1;
    }
    
    full_result_size += 2;
    *offset = 4*full_result_size*key_value__tc+full_result_size*key_value__dp;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ethu_tm_priority_map_is_tdm_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__tc;
    uint32 key_value__dp;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TC, key_value__tc);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_DP, key_value__dp);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        
        full_result_size += 1;
    }
    
    full_result_size += 2;
    *offset = 4*full_result_size*key_value__tc+full_result_size*key_value__dp+2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ilu_tm_priority_map_priority_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__tc;
    uint32 key_value__dp;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TC, key_value__tc);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_DP, key_value__dp);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        
        full_result_size += 1;
    }
    
    full_result_size += 2;
    *offset = 4*full_result_size*key_value__tc+full_result_size*key_value__dp;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ilu_tm_priority_map_is_tdm_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__tc;
    uint32 key_value__dp;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TC, key_value__tc);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_DP, key_value__dp);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        
        full_result_size += 1;
    }
    
    full_result_size += 2;
    *offset = 4*full_result_size*key_value__tc+full_result_size*key_value__dp+2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_feu_tm_priority_map_priority_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__tc;
    uint32 key_value__dp;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TC, key_value__tc);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_DP, key_value__dp);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        
        full_result_size += 1;
    }
    
    full_result_size += 2;
    *offset = 4*full_result_size*key_value__tc+full_result_size*key_value__dp;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_feu_tm_priority_map_is_tdm_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__tc;
    uint32 key_value__dp;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TC, key_value__tc);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_DP, key_value__dp);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        
        full_result_size += 1;
    }
    
    full_result_size += 2;
    *offset = 4*full_result_size*key_value__tc+full_result_size*key_value__dp+2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ethu_ip_priority_map_priority_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__dscp;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_DSCP, key_value__dscp);
    *offset = key_value__dscp*2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ethu_ip_priority_map_priority_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__dscp;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_DSCP, key_value__dscp);
    *offset = key_value__dscp*3;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ethu_ip_priority_map_is_tdm_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__dscp;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_DSCP, key_value__dscp);
    *offset = key_value__dscp*3+2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ethu_mpls_priority_map_priority_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__exp;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_EXP, key_value__exp);
    *offset = key_value__exp*2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ethu_mpls_priority_map_priority_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__exp;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_EXP, key_value__exp);
    *offset = key_value__exp*3;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ethu_mpls_priority_map_is_tdm_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__exp;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_EXP, key_value__exp);
    *offset = key_value__exp*3+2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ethu_vlan_priority_map_priority_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__pcp;
    uint32 key_value__dei;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PCP, key_value__pcp);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_DEI, key_value__dei);
    *offset = key_value__pcp*4+key_value__dei*2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ethu_vlan_priority_map_priority_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__pcp;
    uint32 key_value__dei;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PCP, key_value__pcp);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_DEI, key_value__dei);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        
        full_result_size += 1;
    }
    
    full_result_size += 2;
    *offset = 2*full_result_size*key_value__pcp+full_result_size*key_value__dei;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ethu_vlan_priority_map_is_tdm_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__pcp;
    uint32 key_value__dei;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PCP, key_value__pcp);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_DEI, key_value__dei);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        
        full_result_size += 1;
    }
    
    full_result_size += 2;
    *offset = 2*full_result_size*key_value__pcp+full_result_size*key_value__dei+2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ilu_vlan_priority_map_priority_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__pcp;
    uint32 key_value__dei;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PCP, key_value__pcp);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_DEI, key_value__dei);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        
        full_result_size += 1;
    }
    
    full_result_size += 2;
    *offset = 2*full_result_size*key_value__pcp+full_result_size*key_value__dei;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ilu_vlan_priority_map_is_tdm_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__pcp;
    uint32 key_value__dei;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PCP, key_value__pcp);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_DEI, key_value__dei);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        
        full_result_size += 1;
    }
    
    full_result_size += 2;
    *offset = 2*full_result_size*key_value__pcp+full_result_size*key_value__dei+2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_feu_vlan_priority_map_priority_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__pcp;
    uint32 key_value__dei;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PCP, key_value__pcp);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_DEI, key_value__dei);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        
        full_result_size += 1;
    }
    
    full_result_size += 2;
    *offset = 2*full_result_size*key_value__pcp+full_result_size*key_value__dei;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_feu_vlan_priority_map_is_tdm_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__pcp;
    uint32 key_value__dei;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PCP, key_value__pcp);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_DEI, key_value__dei);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        
        full_result_size += 1;
    }
    
    full_result_size += 2;
    *offset = 2*full_result_size*key_value__pcp+full_result_size*key_value__dei+2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ethu_tpid_tpid_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__index;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_INDEX, key_value__index);
    
    full_result_size += 16;
    *offset = key_value__index*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ilu_tpid_tpid_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__index;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_INDEX, key_value__index);
    
    full_result_size += 16;
    *offset = key_value__index*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_feu_tpid_tpid_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__index;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_INDEX, key_value__index);
    
    full_result_size += 16;
    *offset = key_value__index*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_cdu_thresholds_threshold_blockindex_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__core_id;
    uint32 key_value__cdu_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CORE_ID, key_value__core_id);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CDU_ID, key_value__cdu_id);
    *offset = key_value__core_id*5+key_value__cdu_id-1;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_cdu_thresholds_threshold_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__priority;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PRIORITY, key_value__priority);
    
    full_result_size += utilex_log2_round_up(dnx_data_nif.prd.rmc_threshold_max_get(unit));
    *offset = key_value__priority*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_cdu_thresholds_threshold_blockindex_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__cdu_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CDU_ID, key_value__cdu_id);
    *offset = key_value__cdu_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_cdu_thresholds_threshold_blockindex_2_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__core_id;
    uint32 key_value__cdu_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CORE_ID, key_value__core_id);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CDU_ID, key_value__cdu_id);
    *offset = key_value__core_id*(dnx_data_nif.eth.cdu_nof_per_core_get(unit)-1)+key_value__cdu_id-1;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_clu_thresholds_threshold_blockindex_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__clu_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CLU_ID, key_value__clu_id);
    *offset = key_value__clu_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_clu_thresholds_threshold_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__priority;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PRIORITY, key_value__priority);
    
    full_result_size += utilex_log2_round_up(dnx_data_nif.prd.rmc_threshold_max_get(unit));
    *offset = key_value__priority*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ilu_thresholds_threshold_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__priority;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PRIORITY, key_value__priority);
    
    full_result_size += 13;
    *offset = key_value__priority*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_feu_thresholds_threshold_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__priority;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PRIORITY, key_value__priority);
    
    full_result_size += 15;
    *offset = key_value__priority*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ethu_configurable_ether_type_ether_type_val_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ether_type_code;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ETHER_TYPE_CODE, key_value__ether_type_code);
    
    full_result_size += 16;
    *offset = key_value__ether_type_code*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ilu_configurable_ether_type_ether_type_val_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ether_type_code;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ETHER_TYPE_CODE, key_value__ether_type_code);
    
    full_result_size += 16;
    *offset = key_value__ether_type_code*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_feu_configurable_ether_type_ether_type_val_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ether_type_code;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ETHER_TYPE_CODE, key_value__ether_type_code);
    
    full_result_size += 16;
    *offset = key_value__ether_type_code*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ethu_ether_type_code_offset_base_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ether_type_code;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ETHER_TYPE_CODE, key_value__ether_type_code);
    *offset = key_value__ether_type_code;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ethu_tcam_key_offset_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__offset_index;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_OFFSET_INDEX, key_value__offset_index);
    
    full_result_size += 7;
    *offset = 2+key_value__offset_index*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ilu_tcam_key_offset_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__offset_index;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_OFFSET_INDEX, key_value__offset_index);
    
    full_result_size += 7;
    *offset = 2+key_value__offset_index*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_feu_tcam_key_offset_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__offset_index;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_OFFSET_INDEX, key_value__offset_index);
    
    full_result_size += 7;
    *offset = 2+key_value__offset_index*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ethu_tcam_ether_type_code_groupoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY_ID, key_value__entry_id);
    *offset = key_value__entry_id/8;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ethu_tcam_ether_type_code_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY_ID, key_value__entry_id);
    *offset = key_value__entry_id%8*76+32;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ethu_tcam_ether_type_code_mask_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY_ID, key_value__entry_id);
    *offset = key_value__entry_id%8*76+36+32;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ethu_tcam_entry_offsets_value_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY_ID, key_value__entry_id);
    *offset = key_value__entry_id%8*76;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ethu_tcam_entry_offsets_mask_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY_ID, key_value__entry_id);
    *offset = key_value__entry_id%8*76+36;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ethu_tcam_priority_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY_ID, key_value__entry_id);
    *offset = key_value__entry_id%8*76+36+36;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ethu_tcam_tdm_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY_ID, key_value__entry_id);
    *offset = key_value__entry_id%8*76+36+36+2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ethu_tcam_valid_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY_ID, key_value__entry_id);
    *offset = key_value__entry_id%8*76+36+36+2+1;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ethu_tcam_ether_type_code_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY_ID, key_value__entry_id);
    *offset = key_value__entry_id%8*75+32;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ethu_tcam_ether_type_code_mask_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY_ID, key_value__entry_id);
    *offset = key_value__entry_id%8*75+36+32;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ethu_tcam_entry_offsets_value_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY_ID, key_value__entry_id);
    *offset = key_value__entry_id%8*75;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ethu_tcam_entry_offsets_mask_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY_ID, key_value__entry_id);
    *offset = key_value__entry_id%8*75+36;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ethu_tcam_priority_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY_ID, key_value__entry_id);
    *offset = key_value__entry_id%8*75+36+36;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ethu_tcam_valid_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY_ID, key_value__entry_id);
    *offset = key_value__entry_id%8*75+36+36+2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ilu_tcam_ether_type_code_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry_id;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY_ID, key_value__entry_id);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        
        full_result_size += 1;
    }
    
    full_result_size += 4 + 4 + 32 + 32 + 2 + 1;
    *offset = key_value__entry_id%8*full_result_size+32;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ilu_tcam_ether_type_code_mask_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry_id;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY_ID, key_value__entry_id);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        
        full_result_size += 1;
    }
    
    full_result_size += 4 + 4 + 32 + 32 + 2 + 1;
    *offset = key_value__entry_id%8*full_result_size+36+32;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ilu_tcam_entry_offsets_value_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry_id;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY_ID, key_value__entry_id);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        
        full_result_size += 1;
    }
    
    full_result_size += 4 + 4 + 32 + 32 + 2 + 1;
    *offset = key_value__entry_id%8*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ilu_tcam_entry_offsets_mask_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry_id;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY_ID, key_value__entry_id);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        
        full_result_size += 1;
    }
    
    full_result_size += 4 + 4 + 32 + 32 + 2 + 1;
    *offset = key_value__entry_id%8*full_result_size+36;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ilu_tcam_priority_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry_id;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY_ID, key_value__entry_id);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        
        full_result_size += 1;
    }
    
    full_result_size += 4 + 4 + 32 + 32 + 2 + 1;
    *offset = key_value__entry_id%8*full_result_size+36+36;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ilu_tcam_tdm_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry_id;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY_ID, key_value__entry_id);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        
        full_result_size += 1;
    }
    
    full_result_size += 4 + 4 + 32 + 32 + 2 + 1;
    *offset = key_value__entry_id%8*full_result_size+36+36+2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_ilu_tcam_valid_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry_id;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY_ID, key_value__entry_id);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        
        full_result_size += 1;
    }
    
    full_result_size += 4 + 4 + 32 + 32 + 2 + 1;
    *offset = key_value__entry_id%8*full_result_size+36+36+2+1;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_feu_tcam_ether_type_code_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry_id;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY_ID, key_value__entry_id);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        
        full_result_size += 1;
    }
    
    full_result_size += 4 + 4 + 32 + 32 + 2 + 1;
    *offset = key_value__entry_id%8*full_result_size+32;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_feu_tcam_ether_type_code_mask_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry_id;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY_ID, key_value__entry_id);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        
        full_result_size += 1;
    }
    
    full_result_size += 4 + 4 + 32 + 32 + 2 + 1;
    *offset = key_value__entry_id%8*full_result_size+36+32;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_feu_tcam_entry_offsets_value_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry_id;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY_ID, key_value__entry_id);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        
        full_result_size += 1;
    }
    
    full_result_size += 4 + 4 + 32 + 32 + 2 + 1;
    *offset = key_value__entry_id%8*full_result_size;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_feu_tcam_entry_offsets_mask_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry_id;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY_ID, key_value__entry_id);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        
        full_result_size += 1;
    }
    
    full_result_size += 4 + 4 + 32 + 32 + 2 + 1;
    *offset = key_value__entry_id%8*full_result_size+36;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_feu_tcam_priority_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry_id;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY_ID, key_value__entry_id);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        
        full_result_size += 1;
    }
    
    full_result_size += 4 + 4 + 32 + 32 + 2 + 1;
    *offset = key_value__entry_id%8*full_result_size+36+36;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_feu_tcam_tdm_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry_id;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY_ID, key_value__entry_id);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        
        full_result_size += 1;
    }
    
    full_result_size += 4 + 4 + 32 + 32 + 2 + 1;
    *offset = key_value__entry_id%8*full_result_size+36+36+2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_prd_feu_tcam_valid_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__entry_id;
    
    uint32 full_result_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ENTRY_ID, key_value__entry_id);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        
        full_result_size += 1;
    }
    
    full_result_size += 4 + 4 + 32 + 32 + 2 + 1;
    *offset = key_value__entry_id%8*full_result_size+36+36+2+1;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_egq_credit_mask_egq_credit_mask_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__nif_port_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_NIF_PORT_ID, key_value__nif_port_id);
    *offset = key_value__nif_port_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_stif_instance_map_inner_ethu_select_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__stif_instance;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_STIF_INSTANCE, key_value__stif_instance);
    *offset = key_value__stif_instance;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_fc_ethu_rx_qmlf_threshold_pfc_threshold_set_blockindex_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__core_id;
    uint32 key_value__ethu_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CORE_ID, key_value__core_id);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ETHU_ID, key_value__ethu_id);
    *offset = key_value__core_id*5+key_value__ethu_id-1;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_fc_ethu_rx_qmlf_threshold_pfc_threshold_set_blockindex_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ethu_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ETHU_ID, key_value__ethu_id);
    *offset = key_value__ethu_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_fc_ethu_rx_qmlf_threshold_pfc_threshold_set_blockindex_2_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__core_id;
    uint32 key_value__ethu_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CORE_ID, key_value__core_id);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ETHU_ID, key_value__ethu_id);
    *offset = key_value__core_id*8+key_value__ethu_id-1;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_fc_ethu_gen_llfc_from_glb_rcs_enable_blockindex_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__core_id;
    uint32 key_value__lane_in_core;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CORE_ID, key_value__core_id);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LANE_IN_CORE, key_value__lane_in_core);
    *offset = key_value__core_id*8+key_value__lane_in_core/8-1;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_fc_ethu_gen_pfc_bitmap_bitmap_type_a_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__cfc_prio;
    uint32 key_value__nif_prio;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CFC_PRIO, key_value__cfc_prio);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_NIF_PRIO, key_value__nif_prio);
    *offset = key_value__cfc_prio*(dnx_data_fc.general.bcm_cos_count_get(unit))+key_value__nif_prio;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_fc_ethu_rec_pfc_bitmap_bitmap_type_a_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__nif_prio;
    uint32 key_value__cfc_prio;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_NIF_PRIO, key_value__nif_prio);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CFC_PRIO, key_value__cfc_prio);
    *offset = key_value__nif_prio*(dnx_data_fc.general.bcm_cos_count_get(unit))+key_value__cfc_prio;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_fc_ethu_rec_pfc_to_stop_pm_bitmap_bmp_a_pfc_to_stop_nif_en_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__priority;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PRIORITY, key_value__priority);
    *offset = key_value__priority;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_fc_ethu_gen_pfc_from_llfc_bitmap_pfc_from_llfc_bitmap_type_a_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__nif_priority;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_NIF_PRIORITY, key_value__nif_priority);
    *offset = key_value__nif_priority;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_fc_ethu_rec_llfc_stop_pm_from_cfc_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__lane_in_core;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_LANE_IN_CORE, key_value__lane_in_core);
    *offset = key_value__lane_in_core%dnx_data_nif.eth.nof_lanes_in_cdu_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_flexe_lane_ctrl_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__internal_lane;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_INTERNAL_LANE, key_value__internal_lane);
    *offset = 44+key_value__internal_lane;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_feu_nb_rx_tdm_pattern_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__slot_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SLOT_ID, key_value__slot_id);
    *offset = key_value__slot_id+24;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_feu_nb_rx_tdm_pattern_enable_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__slot_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SLOT_ID, key_value__slot_id);
    *offset = key_value__slot_id+56;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_feu_nb_rx_tdm_pattern_flexe_core_port_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__slot_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SLOT_ID, key_value__slot_id);
    *offset = key_value__slot_id*3;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_feu_nb_rx_tdm_pattern_flexe_core_port_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__slot_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_SLOT_ID, key_value__slot_id);
    *offset = key_value__slot_id*3+32;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_feu_nb_flexe_core_port_ctrl_pm_index_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__flexe_core_port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FLEXE_CORE_PORT, key_value__flexe_core_port);
    *offset = key_value__flexe_core_port;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_feu_nb_pcs_port_ctrl_tx_reset_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__pm_index;
    uint32 key_value__pcs_port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PM_INDEX, key_value__pm_index);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PCS_PORT, key_value__pcs_port);
    *offset = key_value__pm_index*8+key_value__pcs_port;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_feu_nb_pcs_port_ctrl_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__pcs_port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PCS_PORT, key_value__pcs_port);
    *offset = key_value__pcs_port;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_feu_sb_rx_calendar_client_channel_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__cal_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CAL_INDEX, key_value__cal_index);
    *offset = key_value__cal_index*14+7;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_feu_sb_rx_fifo_link_list_next_mem_unit_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__curr_mem_unit;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CURR_MEM_UNIT, key_value__curr_mem_unit);
    *offset = key_value__curr_mem_unit;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_feu_tx_client_ctrl_nmg_reset_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__client_channel;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CLIENT_CHANNEL, key_value__client_channel);
    *offset = 56+key_value__client_channel;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_feu_rx_priority_to_rmc_map_rmc_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__prd_priority;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PRD_PRIORITY, key_value__prd_priority);
    *offset = key_value__prd_priority*8;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_clu_pm_ctrl_pm_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__pm_in_clu;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_PM_IN_CLU, key_value__pm_in_clu);
    *offset = key_value__pm_in_clu;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_feu_l1_rx_ctrl_null_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__client;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CLIENT, key_value__client);
    *offset = key_value__client;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_feu_l1_tx_ctrl_rx_alarm_source_client_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__tmc;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_TMC, key_value__tmc);
    *offset = key_value__tmc;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ilu_tdm_is_tdm_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__channel_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CHANNEL_ID, key_value__channel_id);
    *offset = key_value__channel_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ilu_rx_hrf_ctrl_low_prio_weight_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ilkn_port_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ILKN_PORT_ID, key_value__ilkn_port_id);
    *offset = key_value__ilkn_port_id+2;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ilu_rx_hrf_ctrl_low_prio_weight_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ilkn_port_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ILKN_PORT_ID, key_value__ilkn_port_id);
    *offset = key_value__ilkn_port_id+4;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ilu_rx_hrf_ctrl_low_prio_weight_dataoffset_2_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ilkn_port_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ILKN_PORT_ID, key_value__ilkn_port_id);
    *offset = key_value__ilkn_port_id+6;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ilu_rx_hrf_flush_flush_hrf_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ilkn_hrf_type;
    uint32 key_value__ilkn_port_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ILKN_HRF_TYPE, key_value__ilkn_hrf_type);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ILKN_PORT_ID, key_value__ilkn_port_id);
    *offset = key_value__ilkn_hrf_type*dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit)+key_value__ilkn_port_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ilu_rx_scheduler_flush_flush_context_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ilkn_core_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ILKN_CORE_ID, key_value__ilkn_core_id);
    *offset = 5+key_value__ilkn_core_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ilu_hrf_rx_configuration_prd_disable_generate_err_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    uint32 instance_idx;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, current_mapped_field_id, instance_idx);
    *offset = instance_idx+8;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ilu_port_scheduler_ctrl_is_max_sch_weight_tdm_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ilkn_core_id;
    uint32 key_value__ilkn_port_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ILKN_CORE_ID, key_value__ilkn_core_id);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ILKN_PORT_ID, key_value__ilkn_port_id);
    *offset = dnx_data_nif.eth.ethu_nof_get(unit)+key_value__ilkn_core_id*dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit)+key_value__ilkn_port_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ilu_port_scheduler_ctrl_sch_weight_low_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ilkn_core_id;
    uint32 key_value__ilkn_port_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ILKN_CORE_ID, key_value__ilkn_core_id);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ILKN_PORT_ID, key_value__ilkn_port_id);
    *offset = dnx_data_nif.eth.ethu_nof_get(unit)+key_value__ilkn_core_id*dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit)+key_value__ilkn_port_id+32;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ilu_port_scheduler_ctrl_sch_weight_low_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ilkn_core_id;
    uint32 key_value__ilkn_port_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ILKN_CORE_ID, key_value__ilkn_core_id);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ILKN_PORT_ID, key_value__ilkn_port_id);
    *offset = dnx_data_nif.eth.ethu_nof_get(unit)+key_value__ilkn_core_id*dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit)+key_value__ilkn_port_id+64;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ilu_port_scheduler_ctrl_sch_weight_low_dataoffset_2_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ilkn_core_id;
    uint32 key_value__ilkn_port_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ILKN_CORE_ID, key_value__ilkn_core_id);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ILKN_PORT_ID, key_value__ilkn_port_id);
    *offset = dnx_data_nif.eth.ethu_nof_get(unit)+key_value__ilkn_core_id*dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit)+key_value__ilkn_port_id+96;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ilu_scheduler_ctrl_is_max_sch_weight_tdm_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ilkn_core_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ILKN_CORE_ID, key_value__ilkn_core_id);
    *offset = dnx_data_nif.eth.ethu_nof_get(unit)+key_value__ilkn_core_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ilu_scheduler_ctrl_is_max_sch_weight_tdm_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ilkn_core_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ILKN_CORE_ID, key_value__ilkn_core_id);
    *offset = dnx_data_nif.eth.ethu_nof_get(unit)+key_value__ilkn_core_id+8;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ilu_scheduler_ctrl_is_max_sch_weight_tdm_dataoffset_2_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ilkn_core_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ILKN_CORE_ID, key_value__ilkn_core_id);
    *offset = dnx_data_nif.eth.ethu_nof_get(unit)+key_value__ilkn_core_id+16;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ilu_scheduler_ctrl_is_max_sch_weight_tdm_dataoffset_3_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ilkn_core_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ILKN_CORE_ID, key_value__ilkn_core_id);
    *offset = dnx_data_nif.eth.ethu_nof_get(unit)+key_value__ilkn_core_id+24;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ilu_scheduler_tx_reset_nmg_reset_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ilkn_core_id;
    uint32 key_value__ilkn_port_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ILKN_CORE_ID, key_value__ilkn_core_id);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ILKN_PORT_ID, key_value__ilkn_port_id);
    *offset = key_value__ilkn_core_id*dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit)+key_value__ilkn_port_id+128;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ilu_scheduler_tx_reset_nmg_reset_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ilkn_core_id;
    uint32 key_value__ilkn_port_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ILKN_CORE_ID, key_value__ilkn_core_id);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ILKN_PORT_ID, key_value__ilkn_port_id);
    *offset = key_value__ilkn_core_id*dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit)+key_value__ilkn_port_id+52;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ilu_scheduler_rx_hrf_reset_nmg_reset_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ilkn_core_id;
    uint32 key_value__ilkn_port_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ILKN_CORE_ID, key_value__ilkn_core_id);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ILKN_PORT_ID, key_value__ilkn_port_id);
    *offset = key_value__ilkn_core_id*dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit)+key_value__ilkn_port_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ilkn_fec_enable_fec_tx_serdes_en_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fec_unit;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FEC_UNIT, key_value__fec_unit);
    *offset = key_value__fec_unit;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ilkn_fec_enable_fec_tx_serdes_en_arrayoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fec_unit;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FEC_UNIT, key_value__fec_unit);
    *offset = key_value__fec_unit%4;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ilkn_fec_enable_fec_tx_serdes_en_blockindex_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__fec_unit;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FEC_UNIT, key_value__fec_unit);
    *offset = key_value__fec_unit/dnx_data_nif.eth.cdu_nof_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ethu_power_down_power_down_blockindex_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__ethu_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ETHU_ID, key_value__ethu_id);
    *offset = key_value__ethu_id-dnx_data_nif.eth.cdu_nof_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
nif_ethu_power_down_power_down_blockindex_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__core_id;
    uint32 key_value__ethu_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CORE_ID, key_value__core_id);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_ETHU_ID, key_value__ethu_id);
    *offset = key_value__core_id*(dnx_data_nif.eth.cdu_nof_per_core_get(unit)-1)+key_value__ethu_id-1;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
