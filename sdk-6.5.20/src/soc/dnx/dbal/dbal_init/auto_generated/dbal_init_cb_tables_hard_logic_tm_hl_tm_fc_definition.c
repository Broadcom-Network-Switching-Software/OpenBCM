
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
fc_gen_glb_rcs_pfc_en_enable_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__glb_rcs;
    uint32 key_value__glb_prio;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_GLB_RCS, key_value__glb_rcs);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_GLB_PRIO, key_value__glb_prio);
    *offset = key_value__glb_rcs*2+key_value__glb_prio;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fc_gen_glb_rcs_pfc_en_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__nif_port;
    uint32 key_value__nif_priority;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_NIF_PORT, key_value__nif_port);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_NIF_PRIORITY, key_value__nif_priority);
    *offset = key_value__nif_port*(dnx_data_fc.general.bcm_cos_count_get(unit))+key_value__nif_priority;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fc_gen_glb_rcs_pfc_en_enable_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__nif_port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_NIF_PORT, key_value__nif_port);
    *offset = 2*key_value__nif_port/dnx_data_nif.phys.nof_phys_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fc_gen_glb_rcs_pfc_en_enable_dataoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__nif_port;
    uint32 key_value__nif_priority;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_NIF_PORT, key_value__nif_port);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_NIF_PRIORITY, key_value__nif_priority);
    *offset = (key_value__nif_port-dnx_data_nif.phys.nof_phys_get(unit)/2)*(dnx_data_fc.general.bcm_cos_count_get(unit))+key_value__nif_priority;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fc_gen_glb_rcs_oob_en_enable_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__glb_rcs;
    uint32 key_value__glb_prio;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_GLB_RCS, key_value__glb_rcs);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_GLB_PRIO, key_value__glb_prio);
    *offset = key_value__glb_rcs*2+key_value__glb_prio+28;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fc_gen_glb_rcs_oob_en_enable_blockindex_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__oob_if;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_OOB_IF, key_value__oob_if);
    *offset = key_value__oob_if;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fc_gen_glb_rcs_oob_en_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__cal_idx;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CAL_IDX, key_value__cal_idx);
    *offset = key_value__cal_idx;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fc_gen_glb_rcs_llfc_en_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__nif_port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_NIF_PORT, key_value__nif_port);
    *offset = key_value__nif_port;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fc_gen_cat2_pfc_en_enable_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__cat2_prio;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CAT2_PRIO, key_value__cat2_prio);
    *offset = key_value__cat2_prio;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fc_rec_pfc_map_map_val_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__nif_port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_NIF_PORT, key_value__nif_port);
    *offset = key_value__nif_port-64;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fc_rec_pfc_map_map_val_entryoffset_1_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__nif_port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_NIF_PORT, key_value__nif_port);
    *offset = key_value__nif_port-32;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fc_rec_vsq_rcy_to_pfc_map_map_val_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__vsq_rcy_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_VSQ_RCY_INDEX, key_value__vsq_rcy_index);
    *offset = key_value__vsq_rcy_index;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fc_gen_vsqd_pfc_llfc_map_index_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__vsqd_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_VSQD_INDEX, key_value__vsqd_index);
    *offset = key_value__vsqd_index;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fc_gen_inb_ilkn_cal_map_fc_src_sel_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__cal_if;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CAL_IF, key_value__cal_if);
    *offset = key_value__cal_if;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fc_rx_cpu_map_qpair_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__cpu_chan;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CPU_CHAN, key_value__cpu_chan);
    *offset = key_value__cpu_chan*dnx_data_egr_queuing.params.nof_bits_in_q_pair_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fc_rx_cpu_mask_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__cpu_chan;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_CPU_CHAN, key_value__cpu_chan);
    *offset = key_value__cpu_chan;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fc_generic_bitmap_enable_arrayoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__bmp_idx;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_BMP_IDX, key_value__bmp_idx);
    *offset = key_value__bmp_idx;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fc_glb_rsc_masks_dram_enable_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__glb_prio;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_GLB_PRIO, key_value__glb_prio);
    *offset = key_value__glb_prio;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fc_coe_parameters_pp_inb_enb_blockindex_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__coe_if;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_COE_IF, key_value__coe_if);
    *offset = key_value__coe_if;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fc_coe_vid_channel_id_map_null_entryoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__vid;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_VID, key_value__vid);
    *offset = key_value__vid;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
fc_nif_ilu_status_rx_llfc_dataoffset_0_cb(
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
    *offset = key_value__ilkn_core_id+key_value__ilkn_port_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
