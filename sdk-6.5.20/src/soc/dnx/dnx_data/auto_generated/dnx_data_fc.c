

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_GENERAL

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_fc.h>



extern shr_error_e jr2_a0_data_fc_attach(
    int unit);
extern shr_error_e j2c_a0_data_fc_attach(
    int unit);
extern shr_error_e q2a_a0_data_fc_attach(
    int unit);
extern shr_error_e j2p_a0_data_fc_attach(
    int unit);



static shr_error_e
dnx_data_fc_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "Flow Control general configurations.";
    
    submodule_data->nof_features = _dnx_data_fc_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data fc general features");

    submodule_data->features[dnx_data_fc_general_fc_to_nif_status].name = "fc_to_nif_status";
    submodule_data->features[dnx_data_fc_general_fc_to_nif_status].doc = "Indication if presenting status for FC from CFC to NIF is supported";
    submodule_data->features[dnx_data_fc_general_fc_to_nif_status].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_fc_general_ilu_fc_status].name = "ilu_fc_status";
    submodule_data->features[dnx_data_fc_general_ilu_fc_status].doc = "Indication if presenting status indication for FC from and to ILU is supported";
    submodule_data->features[dnx_data_fc_general_ilu_fc_status].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_fc_general_inb_ilkn_rx_status].name = "inb_ilkn_rx_status";
    submodule_data->features[dnx_data_fc_general_inb_ilkn_rx_status].doc = "Indication if presenting status indication for received Inband ILKN FC signals is supported";
    submodule_data->features[dnx_data_fc_general_inb_ilkn_rx_status].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_fc_general_glb_res_lp_to_llfc].name = "glb_res_lp_to_llfc";
    submodule_data->features[dnx_data_fc_general_glb_res_lp_to_llfc].doc = "Indication if generation from Global Resources Low Priority to LLFC is supported";
    submodule_data->features[dnx_data_fc_general_glb_res_lp_to_llfc].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_fc_general_vsqd_to_llfc_pfc].name = "vsqd_to_llfc_pfc";
    submodule_data->features[dnx_data_fc_general_vsqd_to_llfc_pfc].doc = "Indication if generation from VSQ D to LLFC and PFC is supported";
    submodule_data->features[dnx_data_fc_general_vsqd_to_llfc_pfc].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_fc_general_vsq_thr_en].name = "vsq_thr_en";
    submodule_data->features[dnx_data_fc_general_vsq_thr_en].doc = "Indication the threshold of VSQ";
    submodule_data->features[dnx_data_fc_general_vsq_thr_en].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_fc_general_oob_tx_reset].name = "oob_tx_reset";
    submodule_data->features[dnx_data_fc_general_oob_tx_reset].doc = "Indication if reset of OOB-TX interface is supported";
    submodule_data->features[dnx_data_fc_general_oob_tx_reset].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_fc_general_additional_enablers].name = "additional_enablers";
    submodule_data->features[dnx_data_fc_general_additional_enablers].doc = "Indication if additional CFC enablers are supported";
    submodule_data->features[dnx_data_fc_general_additional_enablers].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_fc_general_ilkn_inb_llfc].name = "ilkn_inb_llfc";
    submodule_data->features[dnx_data_fc_general_ilkn_inb_llfc].doc = "Indication if additional CFC enablers  for the Inband ILKN are supported";
    submodule_data->features[dnx_data_fc_general_ilkn_inb_llfc].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_fc_general_flexe_flow_control].name = "flexe_flow_control";
    submodule_data->features[dnx_data_fc_general_flexe_flow_control].doc = "FLEXE Flow Control is supported";
    submodule_data->features[dnx_data_fc_general_flexe_flow_control].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_fc_general_glb_rsc_oob_en].name = "glb_rsc_oob_en";
    submodule_data->features[dnx_data_fc_general_glb_rsc_oob_en].doc = "Indication if table GLB_RSC_OOB_EN is supported";
    submodule_data->features[dnx_data_fc_general_glb_rsc_oob_en].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_fc_general_cat2_oob_en].name = "cat2_oob_en";
    submodule_data->features[dnx_data_fc_general_cat2_oob_en].doc = "Indication if table CAT2_OOB_EN is supported";
    submodule_data->features[dnx_data_fc_general_cat2_oob_en].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_fc_general_pfc_rec_cdu_shift].name = "pfc_rec_cdu_shift";
    submodule_data->features[dnx_data_fc_general_pfc_rec_cdu_shift].doc = "The last two CDUs of each core have one bit shift left, which is affecting the reception of PFC - CFC is sending wrong index to EGQ";
    submodule_data->features[dnx_data_fc_general_pfc_rec_cdu_shift].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_fc_general_pfc_status_regs_double_read].name = "pfc_status_regs_double_read";
    submodule_data->features[dnx_data_fc_general_pfc_status_regs_double_read].doc = "Registers for EGQ PFC status and NIF PFC status are both 'sticky' and will clear on read only.Changing the STATUS_SEL for each of them will result with an OR between two different ranges of the PFC vector.";
    submodule_data->features[dnx_data_fc_general_pfc_status_regs_double_read].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_fc_general_ilkn_oob_fast_llfc].name = "ilkn_oob_fast_llfc";
    submodule_data->features[dnx_data_fc_general_ilkn_oob_fast_llfc].doc = "Dedicated enabler for ILKN OOB fast LLFC.";
    submodule_data->features[dnx_data_fc_general_ilkn_oob_fast_llfc].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_fc_general_pp_inb_enable].name = "pp_inb_enable";
    submodule_data->features[dnx_data_fc_general_pp_inb_enable].doc = "Indication if table PP Inand enabler is supported";
    submodule_data->features[dnx_data_fc_general_pp_inb_enable].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_fc_general_fc_from_llfc_vsq_status_sel].name = "fc_from_llfc_vsq_status_sel";
    submodule_data->features[dnx_data_fc_general_fc_from_llfc_vsq_status_sel].doc = "Indication if select is needed in order to present status for FC from VSQ LLFC to CFC is supported";
    submodule_data->features[dnx_data_fc_general_fc_from_llfc_vsq_status_sel].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_fc_general_if_2_cfc_enablers].name = "if_2_cfc_enablers";
    submodule_data->features[dnx_data_fc_general_if_2_cfc_enablers].doc = "Indication if enablers or interfaces to CFC are supported";
    submodule_data->features[dnx_data_fc_general_if_2_cfc_enablers].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_fc_general_ilkn_oob_en].name = "ilkn_oob_en";
    submodule_data->features[dnx_data_fc_general_ilkn_oob_en].doc = "Indication if enablers for the RX/TX ILKN OOB interfaces are supported";
    submodule_data->features[dnx_data_fc_general_ilkn_oob_en].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_fc_general_ilkn_polarity_set].name = "ilkn_polarity_set";
    submodule_data->features[dnx_data_fc_general_ilkn_polarity_set].doc = "Indication if setting the ILKN OOB polarity is supported";
    submodule_data->features[dnx_data_fc_general_ilkn_polarity_set].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_fc_general_additional_rx_pfc_bmp_types].name = "additional_rx_pfc_bmp_types";
    submodule_data->features[dnx_data_fc_general_additional_rx_pfc_bmp_types].doc = "Additional types (C and D) of RX PFC Bitmap to map PFC signal received from NIF.";
    submodule_data->features[dnx_data_fc_general_additional_rx_pfc_bmp_types].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_fc_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data fc general defines");

    submodule_data->defines[dnx_data_fc_general_define_nof_glb_rsc].name = "nof_glb_rsc";
    submodule_data->defines[dnx_data_fc_general_define_nof_glb_rsc].doc = "Number of Global Resources";
    
    submodule_data->defines[dnx_data_fc_general_define_nof_glb_rsc].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fc_general_define_nof_gen_bmps].name = "nof_gen_bmps";
    submodule_data->defines[dnx_data_fc_general_define_nof_gen_bmps].doc = "Number of Generic bitmaps";
    
    submodule_data->defines[dnx_data_fc_general_define_nof_gen_bmps].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fc_general_define_nof_af_nif_indications].name = "nof_af_nif_indications";
    submodule_data->defines[dnx_data_fc_general_define_nof_af_nif_indications].doc = "Number of AF NIF FC infications";
    
    submodule_data->defines[dnx_data_fc_general_define_nof_af_nif_indications].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fc_general_define_nof_cal_llfcs].name = "nof_cal_llfcs";
    submodule_data->defines[dnx_data_fc_general_define_nof_cal_llfcs].doc = "Number of Link-Level FC (from the NIF-RX-MLF) that can be mapped to calendar.";
    
    submodule_data->defines[dnx_data_fc_general_define_nof_cal_llfcs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fc_general_define_nof_entries_qmlf].name = "nof_entries_qmlf";
    submodule_data->defines[dnx_data_fc_general_define_nof_entries_qmlf].doc = "Number of entries in the Rx Quad MAC Lane FIFO (QMLF).";
    
    submodule_data->defines[dnx_data_fc_general_define_nof_entries_qmlf].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fc_general_define_bcm_cos_count].name = "bcm_cos_count";
    submodule_data->defines[dnx_data_fc_general_define_bcm_cos_count].doc = "Number of priorities per NIF port";
    
    submodule_data->defines[dnx_data_fc_general_define_bcm_cos_count].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_fc_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data fc general tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_fc_general_feature_get(
    int unit,
    dnx_data_fc_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_general, feature);
}


uint32
dnx_data_fc_general_nof_glb_rsc_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_general, dnx_data_fc_general_define_nof_glb_rsc);
}

uint32
dnx_data_fc_general_nof_gen_bmps_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_general, dnx_data_fc_general_define_nof_gen_bmps);
}

uint32
dnx_data_fc_general_nof_af_nif_indications_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_general, dnx_data_fc_general_define_nof_af_nif_indications);
}

uint32
dnx_data_fc_general_nof_cal_llfcs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_general, dnx_data_fc_general_define_nof_cal_llfcs);
}

uint32
dnx_data_fc_general_nof_entries_qmlf_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_general, dnx_data_fc_general_define_nof_entries_qmlf);
}

uint32
dnx_data_fc_general_bcm_cos_count_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_general, dnx_data_fc_general_define_bcm_cos_count);
}










static shr_error_e
dnx_data_fc_coe_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "coe";
    submodule_data->doc = "Flow Control COE general configurations.";
    
    submodule_data->nof_features = _dnx_data_fc_coe_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data fc coe features");

    
    submodule_data->nof_defines = _dnx_data_fc_coe_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data fc coe defines");

    submodule_data->defines[dnx_data_fc_coe_define_nof_coe_cal_instances].name = "nof_coe_cal_instances";
    submodule_data->defines[dnx_data_fc_coe_define_nof_coe_cal_instances].doc = "Number of PP COE calendar instances";
    
    submodule_data->defines[dnx_data_fc_coe_define_nof_coe_cal_instances].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fc_coe_define_nof_supported_vlan_ids].name = "nof_supported_vlan_ids";
    submodule_data->defines[dnx_data_fc_coe_define_nof_supported_vlan_ids].doc = "Number of supported VLAN IDs in PP COE flow control";
    
    submodule_data->defines[dnx_data_fc_coe_define_nof_supported_vlan_ids].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fc_coe_define_nof_coe_vid_mask_width].name = "nof_coe_vid_mask_width";
    submodule_data->defines[dnx_data_fc_coe_define_nof_coe_vid_mask_width].doc = "Number of COE_VID_MASK width";
    
    submodule_data->defines[dnx_data_fc_coe_define_nof_coe_vid_mask_width].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fc_coe_define_nof_coe_vid_right_shift_width].name = "nof_coe_vid_right_shift_width";
    submodule_data->defines[dnx_data_fc_coe_define_nof_coe_vid_right_shift_width].doc = "Number of COE VID right shift width";
    
    submodule_data->defines[dnx_data_fc_coe_define_nof_coe_vid_right_shift_width].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fc_coe_define_max_coe_ticks].name = "max_coe_ticks";
    submodule_data->defines[dnx_data_fc_coe_define_max_coe_ticks].doc = "Max value for COE ticks. N-1 where N number of clocks for pause counters decrement.";
    
    submodule_data->defines[dnx_data_fc_coe_define_max_coe_ticks].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fc_coe_define_fc_support].name = "fc_support";
    submodule_data->defines[dnx_data_fc_coe_define_fc_support].doc = "Indication to support of Flow_Control on COE port";
    
    submodule_data->defines[dnx_data_fc_coe_define_fc_support].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fc_coe_define_calendar_pause_resolution].name = "calendar_pause_resolution";
    submodule_data->defines[dnx_data_fc_coe_define_calendar_pause_resolution].doc = "PP COE Pause counter rate. Unit: usec.";
    
    submodule_data->defines[dnx_data_fc_coe_define_calendar_pause_resolution].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_fc_coe_define_data_offset].name = "data_offset";
    submodule_data->defines[dnx_data_fc_coe_define_data_offset].doc = "Define offset in bytes from after Ethernet Frame for COE FC data. Valid range = [0-31]";
    
    submodule_data->defines[dnx_data_fc_coe_define_data_offset].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_fc_coe_define_ethertype].name = "ethertype";
    submodule_data->defines[dnx_data_fc_coe_define_ethertype].doc = "Set ethertype of COE FC packet.(16bits)";
    
    submodule_data->defines[dnx_data_fc_coe_define_ethertype].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_fc_coe_define_control_opcode].name = "control_opcode";
    submodule_data->defines[dnx_data_fc_coe_define_control_opcode].doc = "Set PP COE Control Opcode used to identify COE.(16bits)";
    
    submodule_data->defines[dnx_data_fc_coe_define_control_opcode].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_fc_coe_define_coe_vid_offset].name = "coe_vid_offset";
    submodule_data->defines[dnx_data_fc_coe_define_coe_vid_offset].doc = "Set Offset for COE VID";
    
    submodule_data->defines[dnx_data_fc_coe_define_coe_vid_offset].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_fc_coe_define_coe_vid_mask].name = "coe_vid_mask";
    submodule_data->defines[dnx_data_fc_coe_define_coe_vid_mask].doc = "Set mask for COE VID";
    
    submodule_data->defines[dnx_data_fc_coe_define_coe_vid_mask].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_fc_coe_define_coe_vid_right_shift].name = "coe_vid_right_shift";
    submodule_data->defines[dnx_data_fc_coe_define_coe_vid_right_shift].doc = "Set right shift for COE VID shift";
    
    submodule_data->defines[dnx_data_fc_coe_define_coe_vid_right_shift].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_fc_coe_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data fc coe tables");

    
    submodule_data->tables[dnx_data_fc_coe_table_calendar_mode].name = "calendar_mode";
    submodule_data->tables[dnx_data_fc_coe_table_calendar_mode].doc = "PP COE";
    submodule_data->tables[dnx_data_fc_coe_table_calendar_mode].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_fc_coe_table_calendar_mode].size_of_values = sizeof(dnx_data_fc_coe_calendar_mode_t);
    submodule_data->tables[dnx_data_fc_coe_table_calendar_mode].entry_get = dnx_data_fc_coe_calendar_mode_entry_str_get;

    
    submodule_data->tables[dnx_data_fc_coe_table_calendar_mode].nof_keys = 0;

    
    submodule_data->tables[dnx_data_fc_coe_table_calendar_mode].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_fc_coe_table_calendar_mode].values, dnxc_data_value_t, submodule_data->tables[dnx_data_fc_coe_table_calendar_mode].nof_values, "_dnx_data_fc_coe_table_calendar_mode table values");
    submodule_data->tables[dnx_data_fc_coe_table_calendar_mode].values[0].name = "mode";
    submodule_data->tables[dnx_data_fc_coe_table_calendar_mode].values[0].type = "int";
    submodule_data->tables[dnx_data_fc_coe_table_calendar_mode].values[0].doc = "PP COE Mode";
    submodule_data->tables[dnx_data_fc_coe_table_calendar_mode].values[0].offset = UTILEX_OFFSETOF(dnx_data_fc_coe_calendar_mode_t, mode);

    
    submodule_data->tables[dnx_data_fc_coe_table_mac_address].name = "mac_address";
    submodule_data->tables[dnx_data_fc_coe_table_mac_address].doc = "MAC address of COE FC";
    submodule_data->tables[dnx_data_fc_coe_table_mac_address].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_fc_coe_table_mac_address].size_of_values = sizeof(dnx_data_fc_coe_mac_address_t);
    submodule_data->tables[dnx_data_fc_coe_table_mac_address].entry_get = dnx_data_fc_coe_mac_address_entry_str_get;

    
    submodule_data->tables[dnx_data_fc_coe_table_mac_address].nof_keys = 0;

    
    submodule_data->tables[dnx_data_fc_coe_table_mac_address].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_fc_coe_table_mac_address].values, dnxc_data_value_t, submodule_data->tables[dnx_data_fc_coe_table_mac_address].nof_values, "_dnx_data_fc_coe_table_mac_address table values");
    submodule_data->tables[dnx_data_fc_coe_table_mac_address].values[0].name = "mac";
    submodule_data->tables[dnx_data_fc_coe_table_mac_address].values[0].type = "uint8[6]";
    submodule_data->tables[dnx_data_fc_coe_table_mac_address].values[0].doc = "Set the mac address of COE FC packet (48 bits)";
    submodule_data->tables[dnx_data_fc_coe_table_mac_address].values[0].offset = UTILEX_OFFSETOF(dnx_data_fc_coe_mac_address_t, mac);

    
    submodule_data->tables[dnx_data_fc_coe_table_calender_length].name = "calender_length";
    submodule_data->tables[dnx_data_fc_coe_table_calender_length].doc = "Flow Control COE calender length.";
    submodule_data->tables[dnx_data_fc_coe_table_calender_length].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_fc_coe_table_calender_length].size_of_values = sizeof(dnx_data_fc_coe_calender_length_t);
    submodule_data->tables[dnx_data_fc_coe_table_calender_length].entry_get = dnx_data_fc_coe_calender_length_entry_str_get;

    
    submodule_data->tables[dnx_data_fc_coe_table_calender_length].nof_keys = 1;
    submodule_data->tables[dnx_data_fc_coe_table_calender_length].keys[0].name = "cal_id";
    submodule_data->tables[dnx_data_fc_coe_table_calender_length].keys[0].doc = "Calendar ID";

    
    submodule_data->tables[dnx_data_fc_coe_table_calender_length].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_fc_coe_table_calender_length].values, dnxc_data_value_t, submodule_data->tables[dnx_data_fc_coe_table_calender_length].nof_values, "_dnx_data_fc_coe_table_calender_length table values");
    submodule_data->tables[dnx_data_fc_coe_table_calender_length].values[0].name = "len";
    submodule_data->tables[dnx_data_fc_coe_table_calender_length].values[0].type = "int";
    submodule_data->tables[dnx_data_fc_coe_table_calender_length].values[0].doc = "Flow Control COE calender length.";
    submodule_data->tables[dnx_data_fc_coe_table_calender_length].values[0].offset = UTILEX_OFFSETOF(dnx_data_fc_coe_calender_length_t, len);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_fc_coe_feature_get(
    int unit,
    dnx_data_fc_coe_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_coe, feature);
}


uint32
dnx_data_fc_coe_nof_coe_cal_instances_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_coe, dnx_data_fc_coe_define_nof_coe_cal_instances);
}

uint32
dnx_data_fc_coe_nof_supported_vlan_ids_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_coe, dnx_data_fc_coe_define_nof_supported_vlan_ids);
}

uint32
dnx_data_fc_coe_nof_coe_vid_mask_width_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_coe, dnx_data_fc_coe_define_nof_coe_vid_mask_width);
}

uint32
dnx_data_fc_coe_nof_coe_vid_right_shift_width_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_coe, dnx_data_fc_coe_define_nof_coe_vid_right_shift_width);
}

uint32
dnx_data_fc_coe_max_coe_ticks_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_coe, dnx_data_fc_coe_define_max_coe_ticks);
}

uint32
dnx_data_fc_coe_fc_support_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_coe, dnx_data_fc_coe_define_fc_support);
}

uint32
dnx_data_fc_coe_calendar_pause_resolution_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_coe, dnx_data_fc_coe_define_calendar_pause_resolution);
}

uint32
dnx_data_fc_coe_data_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_coe, dnx_data_fc_coe_define_data_offset);
}

uint32
dnx_data_fc_coe_ethertype_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_coe, dnx_data_fc_coe_define_ethertype);
}

uint32
dnx_data_fc_coe_control_opcode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_coe, dnx_data_fc_coe_define_control_opcode);
}

uint32
dnx_data_fc_coe_coe_vid_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_coe, dnx_data_fc_coe_define_coe_vid_offset);
}

uint32
dnx_data_fc_coe_coe_vid_mask_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_coe, dnx_data_fc_coe_define_coe_vid_mask);
}

uint32
dnx_data_fc_coe_coe_vid_right_shift_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_coe, dnx_data_fc_coe_define_coe_vid_right_shift);
}



const dnx_data_fc_coe_calendar_mode_t *
dnx_data_fc_coe_calendar_mode_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_coe, dnx_data_fc_coe_table_calendar_mode);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_fc_coe_calendar_mode_t *) data;

}

const dnx_data_fc_coe_mac_address_t *
dnx_data_fc_coe_mac_address_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_coe, dnx_data_fc_coe_table_mac_address);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_fc_coe_mac_address_t *) data;

}

const dnx_data_fc_coe_calender_length_t *
dnx_data_fc_coe_calender_length_get(
    int unit,
    int cal_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_coe, dnx_data_fc_coe_table_calender_length);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, cal_id, 0);
    return (const dnx_data_fc_coe_calender_length_t *) data;

}


shr_error_e
dnx_data_fc_coe_calendar_mode_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_fc_coe_calendar_mode_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_coe, dnx_data_fc_coe_table_calendar_mode);
    data = (const dnx_data_fc_coe_calendar_mode_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mode);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_fc_coe_mac_address_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_fc_coe_mac_address_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_coe, dnx_data_fc_coe_table_mac_address);
    data = (const dnx_data_fc_coe_mac_address_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, 6, data->mac);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_fc_coe_calender_length_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_fc_coe_calender_length_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_coe, dnx_data_fc_coe_table_calender_length);
    data = (const dnx_data_fc_coe_calender_length_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->len);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_fc_coe_calendar_mode_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_coe, dnx_data_fc_coe_table_calendar_mode);

}

const dnxc_data_table_info_t *
dnx_data_fc_coe_mac_address_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_coe, dnx_data_fc_coe_table_mac_address);

}

const dnxc_data_table_info_t *
dnx_data_fc_coe_calender_length_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_coe, dnx_data_fc_coe_table_calender_length);

}






static shr_error_e
dnx_data_fc_oob_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "oob";
    submodule_data->doc = "Flow Control OOB general configurations.";
    
    submodule_data->nof_features = _dnx_data_fc_oob_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data fc oob features");

    
    submodule_data->nof_defines = _dnx_data_fc_oob_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data fc oob defines");

    submodule_data->defines[dnx_data_fc_oob_define_nof_oob_ids].name = "nof_oob_ids";
    submodule_data->defines[dnx_data_fc_oob_define_nof_oob_ids].doc = "Number of Out of Band interfaces.";
    
    submodule_data->defines[dnx_data_fc_oob_define_nof_oob_ids].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_fc_oob_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data fc oob tables");

    
    submodule_data->tables[dnx_data_fc_oob_table_calender_length].name = "calender_length";
    submodule_data->tables[dnx_data_fc_oob_table_calender_length].doc = "Flow Control Out-Of-Band calender length.";
    submodule_data->tables[dnx_data_fc_oob_table_calender_length].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_fc_oob_table_calender_length].size_of_values = sizeof(dnx_data_fc_oob_calender_length_t);
    submodule_data->tables[dnx_data_fc_oob_table_calender_length].entry_get = dnx_data_fc_oob_calender_length_entry_str_get;

    
    submodule_data->tables[dnx_data_fc_oob_table_calender_length].nof_keys = 1;
    submodule_data->tables[dnx_data_fc_oob_table_calender_length].keys[0].name = "oob_id";
    submodule_data->tables[dnx_data_fc_oob_table_calender_length].keys[0].doc = "Out-Of-Band ID";

    
    submodule_data->tables[dnx_data_fc_oob_table_calender_length].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_fc_oob_table_calender_length].values, dnxc_data_value_t, submodule_data->tables[dnx_data_fc_oob_table_calender_length].nof_values, "_dnx_data_fc_oob_table_calender_length table values");
    submodule_data->tables[dnx_data_fc_oob_table_calender_length].values[0].name = "tx";
    submodule_data->tables[dnx_data_fc_oob_table_calender_length].values[0].type = "int";
    submodule_data->tables[dnx_data_fc_oob_table_calender_length].values[0].doc = "Flow Control Out-Of-Band TX calender length.";
    submodule_data->tables[dnx_data_fc_oob_table_calender_length].values[0].offset = UTILEX_OFFSETOF(dnx_data_fc_oob_calender_length_t, tx);
    submodule_data->tables[dnx_data_fc_oob_table_calender_length].values[1].name = "rx";
    submodule_data->tables[dnx_data_fc_oob_table_calender_length].values[1].type = "int";
    submodule_data->tables[dnx_data_fc_oob_table_calender_length].values[1].doc = "Flow Control Out-Of-Band RX calender length.";
    submodule_data->tables[dnx_data_fc_oob_table_calender_length].values[1].offset = UTILEX_OFFSETOF(dnx_data_fc_oob_calender_length_t, rx);

    
    submodule_data->tables[dnx_data_fc_oob_table_calender_rep_count].name = "calender_rep_count";
    submodule_data->tables[dnx_data_fc_oob_table_calender_rep_count].doc = "Flow Control Out-Of-Band repeat count.";
    submodule_data->tables[dnx_data_fc_oob_table_calender_rep_count].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_fc_oob_table_calender_rep_count].size_of_values = sizeof(dnx_data_fc_oob_calender_rep_count_t);
    submodule_data->tables[dnx_data_fc_oob_table_calender_rep_count].entry_get = dnx_data_fc_oob_calender_rep_count_entry_str_get;

    
    submodule_data->tables[dnx_data_fc_oob_table_calender_rep_count].nof_keys = 1;
    submodule_data->tables[dnx_data_fc_oob_table_calender_rep_count].keys[0].name = "oob_id";
    submodule_data->tables[dnx_data_fc_oob_table_calender_rep_count].keys[0].doc = "Out-Of-Band ID";

    
    submodule_data->tables[dnx_data_fc_oob_table_calender_rep_count].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_fc_oob_table_calender_rep_count].values, dnxc_data_value_t, submodule_data->tables[dnx_data_fc_oob_table_calender_rep_count].nof_values, "_dnx_data_fc_oob_table_calender_rep_count table values");
    submodule_data->tables[dnx_data_fc_oob_table_calender_rep_count].values[0].name = "tx";
    submodule_data->tables[dnx_data_fc_oob_table_calender_rep_count].values[0].type = "int";
    submodule_data->tables[dnx_data_fc_oob_table_calender_rep_count].values[0].doc = "Flow Control Out-Of-Band TX repeat count.";
    submodule_data->tables[dnx_data_fc_oob_table_calender_rep_count].values[0].offset = UTILEX_OFFSETOF(dnx_data_fc_oob_calender_rep_count_t, tx);
    submodule_data->tables[dnx_data_fc_oob_table_calender_rep_count].values[1].name = "rx";
    submodule_data->tables[dnx_data_fc_oob_table_calender_rep_count].values[1].type = "int";
    submodule_data->tables[dnx_data_fc_oob_table_calender_rep_count].values[1].doc = "Flow Control Out-Of-Band RX repeat count.";
    submodule_data->tables[dnx_data_fc_oob_table_calender_rep_count].values[1].offset = UTILEX_OFFSETOF(dnx_data_fc_oob_calender_rep_count_t, rx);

    
    submodule_data->tables[dnx_data_fc_oob_table_tx_freq].name = "tx_freq";
    submodule_data->tables[dnx_data_fc_oob_table_tx_freq].doc = "Flow Control OOB TX Speed relatively to the Core Clock.";
    submodule_data->tables[dnx_data_fc_oob_table_tx_freq].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_fc_oob_table_tx_freq].size_of_values = sizeof(dnx_data_fc_oob_tx_freq_t);
    submodule_data->tables[dnx_data_fc_oob_table_tx_freq].entry_get = dnx_data_fc_oob_tx_freq_entry_str_get;

    
    submodule_data->tables[dnx_data_fc_oob_table_tx_freq].nof_keys = 1;
    submodule_data->tables[dnx_data_fc_oob_table_tx_freq].keys[0].name = "oob_id";
    submodule_data->tables[dnx_data_fc_oob_table_tx_freq].keys[0].doc = "Out-Of-Band ID";

    
    submodule_data->tables[dnx_data_fc_oob_table_tx_freq].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_fc_oob_table_tx_freq].values, dnxc_data_value_t, submodule_data->tables[dnx_data_fc_oob_table_tx_freq].nof_values, "_dnx_data_fc_oob_table_tx_freq table values");
    submodule_data->tables[dnx_data_fc_oob_table_tx_freq].values[0].name = "ratio";
    submodule_data->tables[dnx_data_fc_oob_table_tx_freq].values[0].type = "int";
    submodule_data->tables[dnx_data_fc_oob_table_tx_freq].values[0].doc = "Flow Control OOB TX Speed ratio ";
    submodule_data->tables[dnx_data_fc_oob_table_tx_freq].values[0].offset = UTILEX_OFFSETOF(dnx_data_fc_oob_tx_freq_t, ratio);

    
    submodule_data->tables[dnx_data_fc_oob_table_port].name = "port";
    submodule_data->tables[dnx_data_fc_oob_table_port].doc = "Flow Control Out-Of-Band port type and port mode.";
    submodule_data->tables[dnx_data_fc_oob_table_port].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_fc_oob_table_port].size_of_values = sizeof(dnx_data_fc_oob_port_t);
    submodule_data->tables[dnx_data_fc_oob_table_port].entry_get = dnx_data_fc_oob_port_entry_str_get;

    
    submodule_data->tables[dnx_data_fc_oob_table_port].nof_keys = 1;
    submodule_data->tables[dnx_data_fc_oob_table_port].keys[0].name = "oob_id";
    submodule_data->tables[dnx_data_fc_oob_table_port].keys[0].doc = "Out-Of-Band ID";

    
    submodule_data->tables[dnx_data_fc_oob_table_port].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_fc_oob_table_port].values, dnxc_data_value_t, submodule_data->tables[dnx_data_fc_oob_table_port].nof_values, "_dnx_data_fc_oob_table_port table values");
    submodule_data->tables[dnx_data_fc_oob_table_port].values[0].name = "type";
    submodule_data->tables[dnx_data_fc_oob_table_port].values[0].type = "int";
    submodule_data->tables[dnx_data_fc_oob_table_port].values[0].doc = "Flow Control Out-Of-Band port type.";
    submodule_data->tables[dnx_data_fc_oob_table_port].values[0].offset = UTILEX_OFFSETOF(dnx_data_fc_oob_port_t, type);
    submodule_data->tables[dnx_data_fc_oob_table_port].values[1].name = "mode";
    submodule_data->tables[dnx_data_fc_oob_table_port].values[1].type = "int";
    submodule_data->tables[dnx_data_fc_oob_table_port].values[1].doc = "Flow Control Out-Of-Band port mode.";
    submodule_data->tables[dnx_data_fc_oob_table_port].values[1].offset = UTILEX_OFFSETOF(dnx_data_fc_oob_port_t, mode);

    
    submodule_data->tables[dnx_data_fc_oob_table_indication_invert].name = "indication_invert";
    submodule_data->tables[dnx_data_fc_oob_table_indication_invert].doc = "his option controls the meaning of '0'/'1' in the of Calendar FC indications per interface.";
    submodule_data->tables[dnx_data_fc_oob_table_indication_invert].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_fc_oob_table_indication_invert].size_of_values = sizeof(dnx_data_fc_oob_indication_invert_t);
    submodule_data->tables[dnx_data_fc_oob_table_indication_invert].entry_get = dnx_data_fc_oob_indication_invert_entry_str_get;

    
    submodule_data->tables[dnx_data_fc_oob_table_indication_invert].nof_keys = 1;
    submodule_data->tables[dnx_data_fc_oob_table_indication_invert].keys[0].name = "oob_id";
    submodule_data->tables[dnx_data_fc_oob_table_indication_invert].keys[0].doc = "Out-Of-Band ID";

    
    submodule_data->tables[dnx_data_fc_oob_table_indication_invert].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_fc_oob_table_indication_invert].values, dnxc_data_value_t, submodule_data->tables[dnx_data_fc_oob_table_indication_invert].nof_values, "_dnx_data_fc_oob_table_indication_invert table values");
    submodule_data->tables[dnx_data_fc_oob_table_indication_invert].values[0].name = "ilkn_tx";
    submodule_data->tables[dnx_data_fc_oob_table_indication_invert].values[0].type = "int";
    submodule_data->tables[dnx_data_fc_oob_table_indication_invert].values[0].doc = "Polarity for OOB-ILKN TX.";
    submodule_data->tables[dnx_data_fc_oob_table_indication_invert].values[0].offset = UTILEX_OFFSETOF(dnx_data_fc_oob_indication_invert_t, ilkn_tx);
    submodule_data->tables[dnx_data_fc_oob_table_indication_invert].values[1].name = "ilkn_rx";
    submodule_data->tables[dnx_data_fc_oob_table_indication_invert].values[1].type = "int";
    submodule_data->tables[dnx_data_fc_oob_table_indication_invert].values[1].doc = "Polarity for OOB-ILKN RX.";
    submodule_data->tables[dnx_data_fc_oob_table_indication_invert].values[1].offset = UTILEX_OFFSETOF(dnx_data_fc_oob_indication_invert_t, ilkn_rx);
    submodule_data->tables[dnx_data_fc_oob_table_indication_invert].values[2].name = "spi";
    submodule_data->tables[dnx_data_fc_oob_table_indication_invert].values[2].type = "int";
    submodule_data->tables[dnx_data_fc_oob_table_indication_invert].values[2].doc = "Polarity for OOB-SPI.";
    submodule_data->tables[dnx_data_fc_oob_table_indication_invert].values[2].offset = UTILEX_OFFSETOF(dnx_data_fc_oob_indication_invert_t, spi);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_fc_oob_feature_get(
    int unit,
    dnx_data_fc_oob_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_oob, feature);
}


uint32
dnx_data_fc_oob_nof_oob_ids_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_oob, dnx_data_fc_oob_define_nof_oob_ids);
}



const dnx_data_fc_oob_calender_length_t *
dnx_data_fc_oob_calender_length_get(
    int unit,
    int oob_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_oob, dnx_data_fc_oob_table_calender_length);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, oob_id, 0);
    return (const dnx_data_fc_oob_calender_length_t *) data;

}

const dnx_data_fc_oob_calender_rep_count_t *
dnx_data_fc_oob_calender_rep_count_get(
    int unit,
    int oob_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_oob, dnx_data_fc_oob_table_calender_rep_count);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, oob_id, 0);
    return (const dnx_data_fc_oob_calender_rep_count_t *) data;

}

const dnx_data_fc_oob_tx_freq_t *
dnx_data_fc_oob_tx_freq_get(
    int unit,
    int oob_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_oob, dnx_data_fc_oob_table_tx_freq);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, oob_id, 0);
    return (const dnx_data_fc_oob_tx_freq_t *) data;

}

const dnx_data_fc_oob_port_t *
dnx_data_fc_oob_port_get(
    int unit,
    int oob_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_oob, dnx_data_fc_oob_table_port);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, oob_id, 0);
    return (const dnx_data_fc_oob_port_t *) data;

}

const dnx_data_fc_oob_indication_invert_t *
dnx_data_fc_oob_indication_invert_get(
    int unit,
    int oob_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_oob, dnx_data_fc_oob_table_indication_invert);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, oob_id, 0);
    return (const dnx_data_fc_oob_indication_invert_t *) data;

}


shr_error_e
dnx_data_fc_oob_calender_length_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_fc_oob_calender_length_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_oob, dnx_data_fc_oob_table_calender_length);
    data = (const dnx_data_fc_oob_calender_length_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tx);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->rx);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_fc_oob_calender_rep_count_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_fc_oob_calender_rep_count_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_oob, dnx_data_fc_oob_table_calender_rep_count);
    data = (const dnx_data_fc_oob_calender_rep_count_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tx);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->rx);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_fc_oob_tx_freq_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_fc_oob_tx_freq_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_oob, dnx_data_fc_oob_table_tx_freq);
    data = (const dnx_data_fc_oob_tx_freq_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ratio);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_fc_oob_port_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_fc_oob_port_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_oob, dnx_data_fc_oob_table_port);
    data = (const dnx_data_fc_oob_port_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->type);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mode);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_fc_oob_indication_invert_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_fc_oob_indication_invert_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_oob, dnx_data_fc_oob_table_indication_invert);
    data = (const dnx_data_fc_oob_indication_invert_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ilkn_tx);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ilkn_rx);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->spi);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_fc_oob_calender_length_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_oob, dnx_data_fc_oob_table_calender_length);

}

const dnxc_data_table_info_t *
dnx_data_fc_oob_calender_rep_count_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_oob, dnx_data_fc_oob_table_calender_rep_count);

}

const dnxc_data_table_info_t *
dnx_data_fc_oob_tx_freq_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_oob, dnx_data_fc_oob_table_tx_freq);

}

const dnxc_data_table_info_t *
dnx_data_fc_oob_port_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_oob, dnx_data_fc_oob_table_port);

}

const dnxc_data_table_info_t *
dnx_data_fc_oob_indication_invert_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_oob, dnx_data_fc_oob_table_indication_invert);

}






static shr_error_e
dnx_data_fc_inband_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "inband";
    submodule_data->doc = "Flow Control inband general configurations.";
    
    submodule_data->nof_features = _dnx_data_fc_inband_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data fc inband features");

    
    submodule_data->nof_defines = _dnx_data_fc_inband_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data fc inband defines");

    submodule_data->defines[dnx_data_fc_inband_define_nof_inband_intlkn_cal_instances].name = "nof_inband_intlkn_cal_instances";
    submodule_data->defines[dnx_data_fc_inband_define_nof_inband_intlkn_cal_instances].doc = "Number of Inband ILKN calendar instances";
    
    submodule_data->defines[dnx_data_fc_inband_define_nof_inband_intlkn_cal_instances].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_fc_inband_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data fc inband tables");

    
    submodule_data->tables[dnx_data_fc_inband_table_calender_length].name = "calender_length";
    submodule_data->tables[dnx_data_fc_inband_table_calender_length].doc = "Flow Control In-Band calender length.";
    submodule_data->tables[dnx_data_fc_inband_table_calender_length].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_fc_inband_table_calender_length].size_of_values = sizeof(dnx_data_fc_inband_calender_length_t);
    submodule_data->tables[dnx_data_fc_inband_table_calender_length].entry_get = dnx_data_fc_inband_calender_length_entry_str_get;

    
    submodule_data->tables[dnx_data_fc_inband_table_calender_length].nof_keys = 1;
    submodule_data->tables[dnx_data_fc_inband_table_calender_length].keys[0].name = "ilkn_id";
    submodule_data->tables[dnx_data_fc_inband_table_calender_length].keys[0].doc = "ILKN ID";

    
    submodule_data->tables[dnx_data_fc_inband_table_calender_length].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_fc_inband_table_calender_length].values, dnxc_data_value_t, submodule_data->tables[dnx_data_fc_inband_table_calender_length].nof_values, "_dnx_data_fc_inband_table_calender_length table values");
    submodule_data->tables[dnx_data_fc_inband_table_calender_length].values[0].name = "tx";
    submodule_data->tables[dnx_data_fc_inband_table_calender_length].values[0].type = "int";
    submodule_data->tables[dnx_data_fc_inband_table_calender_length].values[0].doc = "Flow Control In-Band ILKN TX calender length.";
    submodule_data->tables[dnx_data_fc_inband_table_calender_length].values[0].offset = UTILEX_OFFSETOF(dnx_data_fc_inband_calender_length_t, tx);
    submodule_data->tables[dnx_data_fc_inband_table_calender_length].values[1].name = "rx";
    submodule_data->tables[dnx_data_fc_inband_table_calender_length].values[1].type = "int";
    submodule_data->tables[dnx_data_fc_inband_table_calender_length].values[1].doc = "Flow Control In-Band ILKN RX calender length.";
    submodule_data->tables[dnx_data_fc_inband_table_calender_length].values[1].offset = UTILEX_OFFSETOF(dnx_data_fc_inband_calender_length_t, rx);

    
    submodule_data->tables[dnx_data_fc_inband_table_port].name = "port";
    submodule_data->tables[dnx_data_fc_inband_table_port].doc = "Flow Control InBand(interlaken) port mode.";
    submodule_data->tables[dnx_data_fc_inband_table_port].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_fc_inband_table_port].size_of_values = sizeof(dnx_data_fc_inband_port_t);
    submodule_data->tables[dnx_data_fc_inband_table_port].entry_get = dnx_data_fc_inband_port_entry_str_get;

    
    submodule_data->tables[dnx_data_fc_inband_table_port].nof_keys = 1;
    submodule_data->tables[dnx_data_fc_inband_table_port].keys[0].name = "ilkn_id";
    submodule_data->tables[dnx_data_fc_inband_table_port].keys[0].doc = "ILKN ID";

    
    submodule_data->tables[dnx_data_fc_inband_table_port].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_fc_inband_table_port].values, dnxc_data_value_t, submodule_data->tables[dnx_data_fc_inband_table_port].nof_values, "_dnx_data_fc_inband_table_port table values");
    submodule_data->tables[dnx_data_fc_inband_table_port].values[0].name = "mode";
    submodule_data->tables[dnx_data_fc_inband_table_port].values[0].type = "int";
    submodule_data->tables[dnx_data_fc_inband_table_port].values[0].doc = "Flow Control InBand(interlaken) port mode.";
    submodule_data->tables[dnx_data_fc_inband_table_port].values[0].offset = UTILEX_OFFSETOF(dnx_data_fc_inband_port_t, mode);
    submodule_data->tables[dnx_data_fc_inband_table_port].values[1].name = "llfc_mode";
    submodule_data->tables[dnx_data_fc_inband_table_port].values[1].type = "int";
    submodule_data->tables[dnx_data_fc_inband_table_port].values[1].doc = "Flow Control InBand(interlaken) LLFC mode.";
    submodule_data->tables[dnx_data_fc_inband_table_port].values[1].offset = UTILEX_OFFSETOF(dnx_data_fc_inband_port_t, llfc_mode);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_fc_inband_feature_get(
    int unit,
    dnx_data_fc_inband_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_inband, feature);
}


uint32
dnx_data_fc_inband_nof_inband_intlkn_cal_instances_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_inband, dnx_data_fc_inband_define_nof_inband_intlkn_cal_instances);
}



const dnx_data_fc_inband_calender_length_t *
dnx_data_fc_inband_calender_length_get(
    int unit,
    int ilkn_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_inband, dnx_data_fc_inband_table_calender_length);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, ilkn_id, 0);
    return (const dnx_data_fc_inband_calender_length_t *) data;

}

const dnx_data_fc_inband_port_t *
dnx_data_fc_inband_port_get(
    int unit,
    int ilkn_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_inband, dnx_data_fc_inband_table_port);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, ilkn_id, 0);
    return (const dnx_data_fc_inband_port_t *) data;

}


shr_error_e
dnx_data_fc_inband_calender_length_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_fc_inband_calender_length_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_inband, dnx_data_fc_inband_table_calender_length);
    data = (const dnx_data_fc_inband_calender_length_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tx);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->rx);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_fc_inband_port_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_fc_inband_port_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_inband, dnx_data_fc_inband_table_port);
    data = (const dnx_data_fc_inband_port_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mode);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->llfc_mode);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_fc_inband_calender_length_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_inband, dnx_data_fc_inband_table_calender_length);

}

const dnxc_data_table_info_t *
dnx_data_fc_inband_port_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_inband, dnx_data_fc_inband_table_port);

}






static shr_error_e
dnx_data_fc_pfc_deadlock_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "pfc_deadlock";
    submodule_data->doc = "PFC Deadlock detection and recovery.";
    
    submodule_data->nof_features = _dnx_data_fc_pfc_deadlock_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data fc pfc_deadlock features");

    
    submodule_data->nof_defines = _dnx_data_fc_pfc_deadlock_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data fc pfc_deadlock defines");

    submodule_data->defines[dnx_data_fc_pfc_deadlock_define_cdum_intr_id].name = "cdum_intr_id";
    submodule_data->defines[dnx_data_fc_pfc_deadlock_define_cdum_intr_id].doc = "PFC Deadlock detection interrupt ID for CDUM block.";
    
    submodule_data->defines[dnx_data_fc_pfc_deadlock_define_cdum_intr_id].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_fc_pfc_deadlock_define_cdu_intr_id].name = "cdu_intr_id";
    submodule_data->defines[dnx_data_fc_pfc_deadlock_define_cdu_intr_id].doc = "PFC Deadlock detection interrupt ID for CDU block.";
    
    submodule_data->defines[dnx_data_fc_pfc_deadlock_define_cdu_intr_id].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_fc_pfc_deadlock_define_clu_intr_id].name = "clu_intr_id";
    submodule_data->defines[dnx_data_fc_pfc_deadlock_define_clu_intr_id].doc = "PFC Deadlock detection interrupt ID for CLU block.";
    
    submodule_data->defines[dnx_data_fc_pfc_deadlock_define_clu_intr_id].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_fc_pfc_deadlock_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data fc pfc_deadlock tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_fc_pfc_deadlock_feature_get(
    int unit,
    dnx_data_fc_pfc_deadlock_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_pfc_deadlock, feature);
}


uint32
dnx_data_fc_pfc_deadlock_cdum_intr_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_pfc_deadlock, dnx_data_fc_pfc_deadlock_define_cdum_intr_id);
}

uint32
dnx_data_fc_pfc_deadlock_cdu_intr_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_pfc_deadlock, dnx_data_fc_pfc_deadlock_define_cdu_intr_id);
}

uint32
dnx_data_fc_pfc_deadlock_clu_intr_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fc, dnx_data_fc_submodule_pfc_deadlock, dnx_data_fc_pfc_deadlock_define_clu_intr_id);
}







shr_error_e
dnx_data_fc_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "fc";
    module_data->nof_submodules = _dnx_data_fc_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data fc submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_fc_general_init(unit, &module_data->submodules[dnx_data_fc_submodule_general]));
    SHR_IF_ERR_EXIT(dnx_data_fc_coe_init(unit, &module_data->submodules[dnx_data_fc_submodule_coe]));
    SHR_IF_ERR_EXIT(dnx_data_fc_oob_init(unit, &module_data->submodules[dnx_data_fc_submodule_oob]));
    SHR_IF_ERR_EXIT(dnx_data_fc_inband_init(unit, &module_data->submodules[dnx_data_fc_submodule_inband]));
    SHR_IF_ERR_EXIT(dnx_data_fc_pfc_deadlock_init(unit, &module_data->submodules[dnx_data_fc_submodule_pfc_deadlock]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_fc_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_fc_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_fc_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_fc_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_fc_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_fc_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_fc_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_fc_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_fc_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_fc_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_fc_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_fc_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_fc_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_fc_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_fc_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

