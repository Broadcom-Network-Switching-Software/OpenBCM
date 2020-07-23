

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_ingr_congestion.h>



extern shr_error_e jr2_a0_data_ingr_congestion_attach(
    int unit);
extern shr_error_e jr2_b0_data_ingr_congestion_attach(
    int unit);
extern shr_error_e j2c_a0_data_ingr_congestion_attach(
    int unit);
extern shr_error_e j2c_a1_data_ingr_congestion_attach(
    int unit);
extern shr_error_e q2a_a0_data_ingr_congestion_attach(
    int unit);
extern shr_error_e q2a_b1_data_ingr_congestion_attach(
    int unit);
extern shr_error_e j2p_a0_data_ingr_congestion_attach(
    int unit);



static shr_error_e
dnx_data_ingr_congestion_config_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "config";
    submodule_data->doc = "Ingress congestion configuration";
    
    submodule_data->nof_features = _dnx_data_ingr_congestion_config_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data ingr_congestion config features");

    submodule_data->features[dnx_data_ingr_congestion_config_pp_port_by_reassembly_overwrite].name = "pp_port_by_reassembly_overwrite";
    submodule_data->features[dnx_data_ingr_congestion_config_pp_port_by_reassembly_overwrite].doc = "Is PP port in CGM overwritten by Reassembly";
    submodule_data->features[dnx_data_ingr_congestion_config_pp_port_by_reassembly_overwrite].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_ingr_congestion_config_inband_telemetry_ftmh_extension_eco_fix].name = "inband_telemetry_ftmh_extension_eco_fix";
    submodule_data->features[dnx_data_ingr_congestion_config_inband_telemetry_ftmh_extension_eco_fix].doc = "Is inband telemetry FTMH extension issue fixed by ECO";
    submodule_data->features[dnx_data_ingr_congestion_config_inband_telemetry_ftmh_extension_eco_fix].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_ingr_congestion_config_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data ingr_congestion config defines");

    submodule_data->defines[dnx_data_ingr_congestion_config_define_guarantee_mode].name = "guarantee_mode";
    submodule_data->defines[dnx_data_ingr_congestion_config_define_guarantee_mode].doc = "guarantee mode";
    
    submodule_data->defines[dnx_data_ingr_congestion_config_define_guarantee_mode].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_ingr_congestion_config_define_wred_packet_size].name = "wred_packet_size";
    submodule_data->defines[dnx_data_ingr_congestion_config_define_wred_packet_size].doc = "max packet size used in WRED parameters computation";
    
    submodule_data->defines[dnx_data_ingr_congestion_config_define_wred_packet_size].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_ingr_congestion_config_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data ingr_congestion config tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_ingr_congestion_config_feature_get(
    int unit,
    dnx_data_ingr_congestion_config_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_config, feature);
}


uint32
dnx_data_ingr_congestion_config_guarantee_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_config, dnx_data_ingr_congestion_config_define_guarantee_mode);
}

uint32
dnx_data_ingr_congestion_config_wred_packet_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_config, dnx_data_ingr_congestion_config_define_wred_packet_size);
}










static shr_error_e
dnx_data_ingr_congestion_info_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "info";
    submodule_data->doc = "Ingress congestion info";
    
    submodule_data->nof_features = _dnx_data_ingr_congestion_info_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data ingr_congestion info features");

    submodule_data->features[dnx_data_ingr_congestion_info_soft_dram_block_supported].name = "soft_dram_block_supported";
    submodule_data->features[dnx_data_ingr_congestion_info_soft_dram_block_supported].doc = "Soft DRAM block state supported";
    submodule_data->features[dnx_data_ingr_congestion_info_soft_dram_block_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_ingr_congestion_info_latency_based_admission].name = "latency_based_admission";
    submodule_data->features[dnx_data_ingr_congestion_info_latency_based_admission].doc = "Is latency based admission supported";
    submodule_data->features[dnx_data_ingr_congestion_info_latency_based_admission].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_ingr_congestion_info_latency_based_admission_cgm_extended_profiles_enable].name = "latency_based_admission_cgm_extended_profiles_enable";
    submodule_data->features[dnx_data_ingr_congestion_info_latency_based_admission_cgm_extended_profiles_enable].doc = "Is there cgm eco to be enabled in order to use voq-profiles(32) instead of being limited to only 16 flow-profiles";
    submodule_data->features[dnx_data_ingr_congestion_info_latency_based_admission_cgm_extended_profiles_enable].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_ingr_congestion_info_cgm_hw_support].name = "cgm_hw_support";
    submodule_data->features[dnx_data_ingr_congestion_info_cgm_hw_support].doc = "Indicate if cgm block is supported";
    submodule_data->features[dnx_data_ingr_congestion_info_cgm_hw_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_ingr_congestion_info_global_ecn_marking_supported].name = "global_ecn_marking_supported";
    submodule_data->features[dnx_data_ingr_congestion_info_global_ecn_marking_supported].doc = "ECN Marking supported";
    submodule_data->features[dnx_data_ingr_congestion_info_global_ecn_marking_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_ingr_congestion_info_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data ingr_congestion info defines");

    submodule_data->defines[dnx_data_ingr_congestion_info_define_threshold_granularity].name = "threshold_granularity";
    submodule_data->defines[dnx_data_ingr_congestion_info_define_threshold_granularity].doc = "Granularity (non bytes) thresholds are written to HW with";
    
    submodule_data->defines[dnx_data_ingr_congestion_info_define_threshold_granularity].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_congestion_info_define_words_resolution].name = "words_resolution";
    submodule_data->defines[dnx_data_ingr_congestion_info_define_words_resolution].doc = "Size of each HW word in bytes";
    
    submodule_data->defines[dnx_data_ingr_congestion_info_define_words_resolution].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_congestion_info_define_bytes_threshold_granularity].name = "bytes_threshold_granularity";
    submodule_data->defines[dnx_data_ingr_congestion_info_define_bytes_threshold_granularity].doc = "Granularity bytes thresholds are written to HW with";
    
    submodule_data->defines[dnx_data_ingr_congestion_info_define_bytes_threshold_granularity].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_congestion_info_define_nof_dropped_reasons_cgm].name = "nof_dropped_reasons_cgm";
    submodule_data->defines[dnx_data_ingr_congestion_info_define_nof_dropped_reasons_cgm].doc = "number of reasons for the dropped packets in CGM";
    
    submodule_data->defines[dnx_data_ingr_congestion_info_define_nof_dropped_reasons_cgm].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_congestion_info_define_wred_max_gain].name = "wred_max_gain";
    submodule_data->defines[dnx_data_ingr_congestion_info_define_wred_max_gain].doc = "maximal allowed value of gain for WRED";
    
    submodule_data->defines[dnx_data_ingr_congestion_info_define_wred_max_gain].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_congestion_info_define_wred_granularity].name = "wred_granularity";
    submodule_data->defines[dnx_data_ingr_congestion_info_define_wred_granularity].doc = "granularity of WRED parameters for coefficient calculation";
    
    submodule_data->defines[dnx_data_ingr_congestion_info_define_wred_granularity].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_congestion_info_define_nof_pds_in_pdb].name = "nof_pds_in_pdb";
    submodule_data->defines[dnx_data_ingr_congestion_info_define_nof_pds_in_pdb].doc = "number of SRAM PDs in PDB";
    
    submodule_data->defines[dnx_data_ingr_congestion_info_define_nof_pds_in_pdb].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_congestion_info_define_latency_based_admission_reject_flow_profile_value].name = "latency_based_admission_reject_flow_profile_value";
    submodule_data->defines[dnx_data_ingr_congestion_info_define_latency_based_admission_reject_flow_profile_value].doc = "";
    
    submodule_data->defines[dnx_data_ingr_congestion_info_define_latency_based_admission_reject_flow_profile_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_congestion_info_define_latency_based_admission_cgm_extended_profiles_enable_value].name = "latency_based_admission_cgm_extended_profiles_enable_value";
    submodule_data->defines[dnx_data_ingr_congestion_info_define_latency_based_admission_cgm_extended_profiles_enable_value].doc = "If there is cgm eco to be enabled for latency based admissions profiles - set this value to the spare register";
    
    submodule_data->defines[dnx_data_ingr_congestion_info_define_latency_based_admission_cgm_extended_profiles_enable_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_congestion_info_define_latency_based_admission_max_supported_profile].name = "latency_based_admission_max_supported_profile";
    submodule_data->defines[dnx_data_ingr_congestion_info_define_latency_based_admission_max_supported_profile].doc = "For latency based admission not all latency profiles are supported to be used";
    
    submodule_data->defines[dnx_data_ingr_congestion_info_define_latency_based_admission_max_supported_profile].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_congestion_info_define_max_sram_pdbs].name = "max_sram_pdbs";
    submodule_data->defines[dnx_data_ingr_congestion_info_define_max_sram_pdbs].doc = "Maximal number of SRAM PDBS";
    
    submodule_data->defines[dnx_data_ingr_congestion_info_define_max_sram_pdbs].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_ingr_congestion_info_define_max_dram_bdbs].name = "max_dram_bdbs";
    submodule_data->defines[dnx_data_ingr_congestion_info_define_max_dram_bdbs].doc = "Maximal number of DRAM BDBS";
    
    submodule_data->defines[dnx_data_ingr_congestion_info_define_max_dram_bdbs].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_ingr_congestion_info_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data ingr_congestion info tables");

    
    submodule_data->tables[dnx_data_ingr_congestion_info_table_resource].name = "resource";
    submodule_data->tables[dnx_data_ingr_congestion_info_table_resource].doc = "Resource Size - per resource";
    submodule_data->tables[dnx_data_ingr_congestion_info_table_resource].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_ingr_congestion_info_table_resource].size_of_values = sizeof(dnx_data_ingr_congestion_info_resource_t);
    submodule_data->tables[dnx_data_ingr_congestion_info_table_resource].entry_get = dnx_data_ingr_congestion_info_resource_entry_str_get;

    
    submodule_data->tables[dnx_data_ingr_congestion_info_table_resource].nof_keys = 1;
    submodule_data->tables[dnx_data_ingr_congestion_info_table_resource].keys[0].name = "type";
    submodule_data->tables[dnx_data_ingr_congestion_info_table_resource].keys[0].doc = "resource type";

    
    submodule_data->tables[dnx_data_ingr_congestion_info_table_resource].nof_values = 5;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_ingr_congestion_info_table_resource].values, dnxc_data_value_t, submodule_data->tables[dnx_data_ingr_congestion_info_table_resource].nof_values, "_dnx_data_ingr_congestion_info_table_resource table values");
    submodule_data->tables[dnx_data_ingr_congestion_info_table_resource].values[0].name = "max";
    submodule_data->tables[dnx_data_ingr_congestion_info_table_resource].values[0].type = "uint32";
    submodule_data->tables[dnx_data_ingr_congestion_info_table_resource].values[0].doc = "Maximal resource size";
    submodule_data->tables[dnx_data_ingr_congestion_info_table_resource].values[0].offset = UTILEX_OFFSETOF(dnx_data_ingr_congestion_info_resource_t, max);
    submodule_data->tables[dnx_data_ingr_congestion_info_table_resource].values[1].name = "hw_resolution_max";
    submodule_data->tables[dnx_data_ingr_congestion_info_table_resource].values[1].type = "uint32";
    submodule_data->tables[dnx_data_ingr_congestion_info_table_resource].values[1].doc = "Maximal resource size in HW granularity";
    submodule_data->tables[dnx_data_ingr_congestion_info_table_resource].values[1].offset = UTILEX_OFFSETOF(dnx_data_ingr_congestion_info_resource_t, hw_resolution_max);
    submodule_data->tables[dnx_data_ingr_congestion_info_table_resource].values[2].name = "hw_resolution_nof_bits";
    submodule_data->tables[dnx_data_ingr_congestion_info_table_resource].values[2].type = "uint32";
    submodule_data->tables[dnx_data_ingr_congestion_info_table_resource].values[2].doc = "Maximal nf bits to describe resource size in HW granularity";
    submodule_data->tables[dnx_data_ingr_congestion_info_table_resource].values[2].offset = UTILEX_OFFSETOF(dnx_data_ingr_congestion_info_resource_t, hw_resolution_nof_bits);
    submodule_data->tables[dnx_data_ingr_congestion_info_table_resource].values[3].name = "fadt_alpha_min";
    submodule_data->tables[dnx_data_ingr_congestion_info_table_resource].values[3].type = "int";
    submodule_data->tables[dnx_data_ingr_congestion_info_table_resource].values[3].doc = "fadt alpha min value per resource type";
    submodule_data->tables[dnx_data_ingr_congestion_info_table_resource].values[3].offset = UTILEX_OFFSETOF(dnx_data_ingr_congestion_info_resource_t, fadt_alpha_min);
    submodule_data->tables[dnx_data_ingr_congestion_info_table_resource].values[4].name = "fadt_alpha_max";
    submodule_data->tables[dnx_data_ingr_congestion_info_table_resource].values[4].type = "int";
    submodule_data->tables[dnx_data_ingr_congestion_info_table_resource].values[4].doc = "fadt alpha max value per resource type";
    submodule_data->tables[dnx_data_ingr_congestion_info_table_resource].values[4].offset = UTILEX_OFFSETOF(dnx_data_ingr_congestion_info_resource_t, fadt_alpha_max);

    
    submodule_data->tables[dnx_data_ingr_congestion_info_table_dp_free_res_presentage_drop].name = "dp_free_res_presentage_drop";
    submodule_data->tables[dnx_data_ingr_congestion_info_table_dp_free_res_presentage_drop].doc = "Amount of free resource precntage to cause drop per DP";
    submodule_data->tables[dnx_data_ingr_congestion_info_table_dp_free_res_presentage_drop].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_ingr_congestion_info_table_dp_free_res_presentage_drop].size_of_values = sizeof(dnx_data_ingr_congestion_info_dp_free_res_presentage_drop_t);
    submodule_data->tables[dnx_data_ingr_congestion_info_table_dp_free_res_presentage_drop].entry_get = dnx_data_ingr_congestion_info_dp_free_res_presentage_drop_entry_str_get;

    
    submodule_data->tables[dnx_data_ingr_congestion_info_table_dp_free_res_presentage_drop].nof_keys = 1;
    submodule_data->tables[dnx_data_ingr_congestion_info_table_dp_free_res_presentage_drop].keys[0].name = "dp";
    submodule_data->tables[dnx_data_ingr_congestion_info_table_dp_free_res_presentage_drop].keys[0].doc = "DP (0-3). 0=Green, 1=Yellow, 2=Red, 3=Black";

    
    submodule_data->tables[dnx_data_ingr_congestion_info_table_dp_free_res_presentage_drop].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_ingr_congestion_info_table_dp_free_res_presentage_drop].values, dnxc_data_value_t, submodule_data->tables[dnx_data_ingr_congestion_info_table_dp_free_res_presentage_drop].nof_values, "_dnx_data_ingr_congestion_info_table_dp_free_res_presentage_drop table values");
    submodule_data->tables[dnx_data_ingr_congestion_info_table_dp_free_res_presentage_drop].values[0].name = "drop_precentage";
    submodule_data->tables[dnx_data_ingr_congestion_info_table_dp_free_res_presentage_drop].values[0].type = "uint32";
    submodule_data->tables[dnx_data_ingr_congestion_info_table_dp_free_res_presentage_drop].values[0].doc = "precentage of free resources to cause drop";
    submodule_data->tables[dnx_data_ingr_congestion_info_table_dp_free_res_presentage_drop].values[0].offset = UTILEX_OFFSETOF(dnx_data_ingr_congestion_info_dp_free_res_presentage_drop_t, drop_precentage);

    
    submodule_data->tables[dnx_data_ingr_congestion_info_table_admission_preferences].name = "admission_preferences";
    submodule_data->tables[dnx_data_ingr_congestion_info_table_admission_preferences].doc = "admission preferences (admit/guaranteed) per DP.";
    submodule_data->tables[dnx_data_ingr_congestion_info_table_admission_preferences].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_ingr_congestion_info_table_admission_preferences].size_of_values = sizeof(dnx_data_ingr_congestion_info_admission_preferences_t);
    submodule_data->tables[dnx_data_ingr_congestion_info_table_admission_preferences].entry_get = dnx_data_ingr_congestion_info_admission_preferences_entry_str_get;

    
    submodule_data->tables[dnx_data_ingr_congestion_info_table_admission_preferences].nof_keys = 1;
    submodule_data->tables[dnx_data_ingr_congestion_info_table_admission_preferences].keys[0].name = "dp";
    submodule_data->tables[dnx_data_ingr_congestion_info_table_admission_preferences].keys[0].doc = "DP (0-3). 0=Green, 1=Yellow, 2=Red, 3=Black";

    
    submodule_data->tables[dnx_data_ingr_congestion_info_table_admission_preferences].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_ingr_congestion_info_table_admission_preferences].values, dnxc_data_value_t, submodule_data->tables[dnx_data_ingr_congestion_info_table_admission_preferences].nof_values, "_dnx_data_ingr_congestion_info_table_admission_preferences table values");
    submodule_data->tables[dnx_data_ingr_congestion_info_table_admission_preferences].values[0].name = "is_guarantee_over_admit";
    submodule_data->tables[dnx_data_ingr_congestion_info_table_admission_preferences].values[0].type = "uint32";
    submodule_data->tables[dnx_data_ingr_congestion_info_table_admission_preferences].values[0].doc = "if set, than in case statistic admission tests fail (for example WRED), but the queue level is within the guaranteed, it will enter.";
    submodule_data->tables[dnx_data_ingr_congestion_info_table_admission_preferences].values[0].offset = UTILEX_OFFSETOF(dnx_data_ingr_congestion_info_admission_preferences_t, is_guarantee_over_admit);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_ingr_congestion_info_feature_get(
    int unit,
    dnx_data_ingr_congestion_info_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_info, feature);
}


uint32
dnx_data_ingr_congestion_info_threshold_granularity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_info, dnx_data_ingr_congestion_info_define_threshold_granularity);
}

uint32
dnx_data_ingr_congestion_info_words_resolution_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_info, dnx_data_ingr_congestion_info_define_words_resolution);
}

uint32
dnx_data_ingr_congestion_info_bytes_threshold_granularity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_info, dnx_data_ingr_congestion_info_define_bytes_threshold_granularity);
}

uint32
dnx_data_ingr_congestion_info_nof_dropped_reasons_cgm_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_info, dnx_data_ingr_congestion_info_define_nof_dropped_reasons_cgm);
}

uint32
dnx_data_ingr_congestion_info_wred_max_gain_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_info, dnx_data_ingr_congestion_info_define_wred_max_gain);
}

uint32
dnx_data_ingr_congestion_info_wred_granularity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_info, dnx_data_ingr_congestion_info_define_wred_granularity);
}

uint32
dnx_data_ingr_congestion_info_nof_pds_in_pdb_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_info, dnx_data_ingr_congestion_info_define_nof_pds_in_pdb);
}

uint32
dnx_data_ingr_congestion_info_latency_based_admission_reject_flow_profile_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_info, dnx_data_ingr_congestion_info_define_latency_based_admission_reject_flow_profile_value);
}

uint32
dnx_data_ingr_congestion_info_latency_based_admission_cgm_extended_profiles_enable_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_info, dnx_data_ingr_congestion_info_define_latency_based_admission_cgm_extended_profiles_enable_value);
}

uint32
dnx_data_ingr_congestion_info_latency_based_admission_max_supported_profile_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_info, dnx_data_ingr_congestion_info_define_latency_based_admission_max_supported_profile);
}

uint32
dnx_data_ingr_congestion_info_max_sram_pdbs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_info, dnx_data_ingr_congestion_info_define_max_sram_pdbs);
}

uint32
dnx_data_ingr_congestion_info_max_dram_bdbs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_info, dnx_data_ingr_congestion_info_define_max_dram_bdbs);
}



const dnx_data_ingr_congestion_info_resource_t *
dnx_data_ingr_congestion_info_resource_get(
    int unit,
    int type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_info, dnx_data_ingr_congestion_info_table_resource);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, type, 0);
    return (const dnx_data_ingr_congestion_info_resource_t *) data;

}

const dnx_data_ingr_congestion_info_dp_free_res_presentage_drop_t *
dnx_data_ingr_congestion_info_dp_free_res_presentage_drop_get(
    int unit,
    int dp)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_info, dnx_data_ingr_congestion_info_table_dp_free_res_presentage_drop);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dp, 0);
    return (const dnx_data_ingr_congestion_info_dp_free_res_presentage_drop_t *) data;

}

const dnx_data_ingr_congestion_info_admission_preferences_t *
dnx_data_ingr_congestion_info_admission_preferences_get(
    int unit,
    int dp)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_info, dnx_data_ingr_congestion_info_table_admission_preferences);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dp, 0);
    return (const dnx_data_ingr_congestion_info_admission_preferences_t *) data;

}


shr_error_e
dnx_data_ingr_congestion_info_resource_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_ingr_congestion_info_resource_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_info, dnx_data_ingr_congestion_info_table_resource);
    data = (const dnx_data_ingr_congestion_info_resource_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->max);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->hw_resolution_max);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->hw_resolution_nof_bits);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fadt_alpha_min);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fadt_alpha_max);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_ingr_congestion_info_dp_free_res_presentage_drop_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_ingr_congestion_info_dp_free_res_presentage_drop_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_info, dnx_data_ingr_congestion_info_table_dp_free_res_presentage_drop);
    data = (const dnx_data_ingr_congestion_info_dp_free_res_presentage_drop_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->drop_precentage);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_ingr_congestion_info_admission_preferences_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_ingr_congestion_info_admission_preferences_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_info, dnx_data_ingr_congestion_info_table_admission_preferences);
    data = (const dnx_data_ingr_congestion_info_admission_preferences_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_guarantee_over_admit);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_ingr_congestion_info_resource_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_info, dnx_data_ingr_congestion_info_table_resource);

}

const dnxc_data_table_info_t *
dnx_data_ingr_congestion_info_dp_free_res_presentage_drop_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_info, dnx_data_ingr_congestion_info_table_dp_free_res_presentage_drop);

}

const dnxc_data_table_info_t *
dnx_data_ingr_congestion_info_admission_preferences_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_info, dnx_data_ingr_congestion_info_table_admission_preferences);

}






static shr_error_e
dnx_data_ingr_congestion_fadt_tail_drop_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "fadt_tail_drop";
    submodule_data->doc = "FADT tail drop thresholds and related sizes";
    
    submodule_data->nof_features = _dnx_data_ingr_congestion_fadt_tail_drop_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data ingr_congestion fadt_tail_drop features");

    
    submodule_data->nof_defines = _dnx_data_ingr_congestion_fadt_tail_drop_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data ingr_congestion fadt_tail_drop defines");

    submodule_data->defines[dnx_data_ingr_congestion_fadt_tail_drop_define_default_max_size_byte_threshold_for_ocb_only].name = "default_max_size_byte_threshold_for_ocb_only";
    submodule_data->defines[dnx_data_ingr_congestion_fadt_tail_drop_define_default_max_size_byte_threshold_for_ocb_only].doc = "Default for max size threshold for byte resource when rate class is OCB only";
    
    submodule_data->defines[dnx_data_ingr_congestion_fadt_tail_drop_define_default_max_size_byte_threshold_for_ocb_only].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_ingr_congestion_fadt_tail_drop_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data ingr_congestion fadt_tail_drop tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_ingr_congestion_fadt_tail_drop_feature_get(
    int unit,
    dnx_data_ingr_congestion_fadt_tail_drop_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_fadt_tail_drop, feature);
}


uint32
dnx_data_ingr_congestion_fadt_tail_drop_default_max_size_byte_threshold_for_ocb_only_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_fadt_tail_drop, dnx_data_ingr_congestion_fadt_tail_drop_define_default_max_size_byte_threshold_for_ocb_only);
}










static shr_error_e
dnx_data_ingr_congestion_dram_bound_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "dram_bound";
    submodule_data->doc = "dram bound thresholds and related sizes";
    
    submodule_data->nof_features = _dnx_data_ingr_congestion_dram_bound_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data ingr_congestion dram_bound features");

    
    submodule_data->nof_defines = _dnx_data_ingr_congestion_dram_bound_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data ingr_congestion dram_bound defines");

    submodule_data->defines[dnx_data_ingr_congestion_dram_bound_define_fadt_alpha_min].name = "fadt_alpha_min";
    submodule_data->defines[dnx_data_ingr_congestion_dram_bound_define_fadt_alpha_min].doc = "min value for fadt dram thresholds";
    
    submodule_data->defines[dnx_data_ingr_congestion_dram_bound_define_fadt_alpha_min].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_congestion_dram_bound_define_fadt_alpha_max].name = "fadt_alpha_max";
    submodule_data->defines[dnx_data_ingr_congestion_dram_bound_define_fadt_alpha_max].doc = "max value for fadt dram thresholds";
    
    submodule_data->defines[dnx_data_ingr_congestion_dram_bound_define_fadt_alpha_max].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_ingr_congestion_dram_bound_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data ingr_congestion dram_bound tables");

    
    submodule_data->tables[dnx_data_ingr_congestion_dram_bound_table_resource].name = "resource";
    submodule_data->tables[dnx_data_ingr_congestion_dram_bound_table_resource].doc = "DRAM Resource info per resource";
    submodule_data->tables[dnx_data_ingr_congestion_dram_bound_table_resource].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_ingr_congestion_dram_bound_table_resource].size_of_values = sizeof(dnx_data_ingr_congestion_dram_bound_resource_t);
    submodule_data->tables[dnx_data_ingr_congestion_dram_bound_table_resource].entry_get = dnx_data_ingr_congestion_dram_bound_resource_entry_str_get;

    
    submodule_data->tables[dnx_data_ingr_congestion_dram_bound_table_resource].nof_keys = 1;
    submodule_data->tables[dnx_data_ingr_congestion_dram_bound_table_resource].keys[0].name = "type";
    submodule_data->tables[dnx_data_ingr_congestion_dram_bound_table_resource].keys[0].doc = "resource type";

    
    submodule_data->tables[dnx_data_ingr_congestion_dram_bound_table_resource].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_ingr_congestion_dram_bound_table_resource].values, dnxc_data_value_t, submodule_data->tables[dnx_data_ingr_congestion_dram_bound_table_resource].nof_values, "_dnx_data_ingr_congestion_dram_bound_table_resource table values");
    submodule_data->tables[dnx_data_ingr_congestion_dram_bound_table_resource].values[0].name = "max";
    submodule_data->tables[dnx_data_ingr_congestion_dram_bound_table_resource].values[0].type = "uint32";
    submodule_data->tables[dnx_data_ingr_congestion_dram_bound_table_resource].values[0].doc = "Maximal resource size";
    submodule_data->tables[dnx_data_ingr_congestion_dram_bound_table_resource].values[0].offset = UTILEX_OFFSETOF(dnx_data_ingr_congestion_dram_bound_resource_t, max);
    submodule_data->tables[dnx_data_ingr_congestion_dram_bound_table_resource].values[1].name = "is_resource_range";
    submodule_data->tables[dnx_data_ingr_congestion_dram_bound_table_resource].values[1].type = "int";
    submodule_data->tables[dnx_data_ingr_congestion_dram_bound_table_resource].values[1].doc = "if set, FADT threshold algorithm for the resource type is limited to a specific free resources range";
    submodule_data->tables[dnx_data_ingr_congestion_dram_bound_table_resource].values[1].offset = UTILEX_OFFSETOF(dnx_data_ingr_congestion_dram_bound_resource_t, is_resource_range);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_ingr_congestion_dram_bound_feature_get(
    int unit,
    dnx_data_ingr_congestion_dram_bound_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_dram_bound, feature);
}


uint32
dnx_data_ingr_congestion_dram_bound_fadt_alpha_min_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_dram_bound, dnx_data_ingr_congestion_dram_bound_define_fadt_alpha_min);
}

uint32
dnx_data_ingr_congestion_dram_bound_fadt_alpha_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_dram_bound, dnx_data_ingr_congestion_dram_bound_define_fadt_alpha_max);
}



const dnx_data_ingr_congestion_dram_bound_resource_t *
dnx_data_ingr_congestion_dram_bound_resource_get(
    int unit,
    int type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_dram_bound, dnx_data_ingr_congestion_dram_bound_table_resource);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, type, 0);
    return (const dnx_data_ingr_congestion_dram_bound_resource_t *) data;

}


shr_error_e
dnx_data_ingr_congestion_dram_bound_resource_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_ingr_congestion_dram_bound_resource_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_dram_bound, dnx_data_ingr_congestion_dram_bound_table_resource);
    data = (const dnx_data_ingr_congestion_dram_bound_resource_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->max);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_resource_range);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_ingr_congestion_dram_bound_resource_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_dram_bound, dnx_data_ingr_congestion_dram_bound_table_resource);

}






static shr_error_e
dnx_data_ingr_congestion_voq_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "voq";
    submodule_data->doc = "VOQ info";
    
    submodule_data->nof_features = _dnx_data_ingr_congestion_voq_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data ingr_congestion voq features");

    
    submodule_data->nof_defines = _dnx_data_ingr_congestion_voq_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data ingr_congestion voq defines");

    submodule_data->defines[dnx_data_ingr_congestion_voq_define_nof_rate_class].name = "nof_rate_class";
    submodule_data->defines[dnx_data_ingr_congestion_voq_define_nof_rate_class].doc = "number of VOQ rate classes";
    
    submodule_data->defines[dnx_data_ingr_congestion_voq_define_nof_rate_class].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_congestion_voq_define_rate_class_nof_bits].name = "rate_class_nof_bits";
    submodule_data->defines[dnx_data_ingr_congestion_voq_define_rate_class_nof_bits].doc = "number of bits in VOQ rate class";
    
    submodule_data->defines[dnx_data_ingr_congestion_voq_define_rate_class_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_congestion_voq_define_nof_compensation_profiles].name = "nof_compensation_profiles";
    submodule_data->defines[dnx_data_ingr_congestion_voq_define_nof_compensation_profiles].doc = "number of VOQ compensation profiles. used for voq compensation calculation";
    
    submodule_data->defines[dnx_data_ingr_congestion_voq_define_nof_compensation_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_congestion_voq_define_default_compensation].name = "default_compensation";
    submodule_data->defines[dnx_data_ingr_congestion_voq_define_default_compensation].doc = "set to eth compensation (Preamable + IFG + CRC)";
    
    submodule_data->defines[dnx_data_ingr_congestion_voq_define_default_compensation].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_congestion_voq_define_latency_bins].name = "latency_bins";
    submodule_data->defines[dnx_data_ingr_congestion_voq_define_latency_bins].doc = "number of latency admission VOQ ranges";
    submodule_data->defines[dnx_data_ingr_congestion_voq_define_latency_bins].labels[0] = "j2p_notrev";
    
    submodule_data->defines[dnx_data_ingr_congestion_voq_define_latency_bins].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_congestion_voq_define_voq_congestion_notification_fifo_size].name = "voq_congestion_notification_fifo_size";
    submodule_data->defines[dnx_data_ingr_congestion_voq_define_voq_congestion_notification_fifo_size].doc = "Number of elements in congestion notification FIFO for VOQ";
    
    submodule_data->defines[dnx_data_ingr_congestion_voq_define_voq_congestion_notification_fifo_size].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_ingr_congestion_voq_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data ingr_congestion voq tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_ingr_congestion_voq_feature_get(
    int unit,
    dnx_data_ingr_congestion_voq_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_voq, feature);
}


uint32
dnx_data_ingr_congestion_voq_nof_rate_class_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_voq, dnx_data_ingr_congestion_voq_define_nof_rate_class);
}

uint32
dnx_data_ingr_congestion_voq_rate_class_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_voq, dnx_data_ingr_congestion_voq_define_rate_class_nof_bits);
}

uint32
dnx_data_ingr_congestion_voq_nof_compensation_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_voq, dnx_data_ingr_congestion_voq_define_nof_compensation_profiles);
}

uint32
dnx_data_ingr_congestion_voq_default_compensation_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_voq, dnx_data_ingr_congestion_voq_define_default_compensation);
}

uint32
dnx_data_ingr_congestion_voq_latency_bins_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_voq, dnx_data_ingr_congestion_voq_define_latency_bins);
}

uint32
dnx_data_ingr_congestion_voq_voq_congestion_notification_fifo_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_voq, dnx_data_ingr_congestion_voq_define_voq_congestion_notification_fifo_size);
}










static shr_error_e
dnx_data_ingr_congestion_vsq_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "vsq";
    submodule_data->doc = "VSQ info";
    
    submodule_data->nof_features = _dnx_data_ingr_congestion_vsq_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data ingr_congestion vsq features");

    submodule_data->features[dnx_data_ingr_congestion_vsq_size_watermark_support].name = "size_watermark_support";
    submodule_data->features[dnx_data_ingr_congestion_vsq_size_watermark_support].doc = "support for VSQ size watermark retrieval";
    submodule_data->features[dnx_data_ingr_congestion_vsq_size_watermark_support].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_ingr_congestion_vsq_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data ingr_congestion vsq defines");

    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_rate_class_nof].name = "vsq_rate_class_nof";
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_rate_class_nof].doc = "Number of VSQ rate classes";
    
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_rate_class_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_a_rate_class_nof].name = "vsq_a_rate_class_nof";
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_a_rate_class_nof].doc = "Number of VSQ-A rate classes";
    
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_a_rate_class_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_a_nof].name = "vsq_a_nof";
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_a_nof].doc = "Number of VSQ-A";
    
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_a_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_b_nof].name = "vsq_b_nof";
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_b_nof].doc = "Number of VSQ-B";
    
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_b_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_c_nof].name = "vsq_c_nof";
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_c_nof].doc = "Number of VSQ-C";
    
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_c_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_d_nof].name = "vsq_d_nof";
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_d_nof].doc = "Number of VSQ-D";
    
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_d_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_e_nof].name = "vsq_e_nof";
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_e_nof].doc = "Number of VSQ-E";
    
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_e_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_e_hw_nof].name = "vsq_e_hw_nof";
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_e_hw_nof].doc = "Number of VSQ-E in HW, including reserved VSQ-E";
    
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_e_hw_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_f_nof].name = "vsq_f_nof";
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_f_nof].doc = "Number of VSQ-F";
    
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_f_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_f_hw_nof].name = "vsq_f_hw_nof";
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_f_hw_nof].doc = "Number of VSQ-F in HW, including reserved VSQ-F";
    
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_f_hw_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_nif_vsq_e_nof].name = "nif_vsq_e_nof";
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_nif_vsq_e_nof].doc = "Number of VSQ-E on NIF ports";
    
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_nif_vsq_e_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_non_nif_vsq_f_nof].name = "non_nif_vsq_f_nof";
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_non_nif_vsq_f_nof].doc = "Number of VSQ-F on non NIF ports";
    
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_non_nif_vsq_f_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_pool_nof].name = "pool_nof";
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_pool_nof].doc = "Number of resource pools";
    
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_pool_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_connection_class_nof].name = "connection_class_nof";
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_connection_class_nof].doc = "Number of connection classes";
    
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_connection_class_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_tc_pg_profile_nof].name = "tc_pg_profile_nof";
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_tc_pg_profile_nof].doc = "Number of tc->pg profiles";
    
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_tc_pg_profile_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_e_congestion_notification_fifo_size].name = "vsq_e_congestion_notification_fifo_size";
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_e_congestion_notification_fifo_size].doc = "Number of elements in congestion notification FIFO for PB VSQ PG";
    
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_e_congestion_notification_fifo_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_f_congestion_notification_fifo_size].name = "vsq_f_congestion_notification_fifo_size";
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_f_congestion_notification_fifo_size].doc = "Number of elements in congestion notification FIFO for PB VSQ LLFC";
    
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_f_congestion_notification_fifo_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_e_default].name = "vsq_e_default";
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_e_default].doc = "Default mapping of VSQ-E";
    
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_e_default].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_f_default].name = "vsq_f_default";
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_f_default].doc = "Default mapping of VSQ-F";
    
    submodule_data->defines[dnx_data_ingr_congestion_vsq_define_vsq_f_default].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_ingr_congestion_vsq_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data ingr_congestion vsq tables");

    
    submodule_data->tables[dnx_data_ingr_congestion_vsq_table_info].name = "info";
    submodule_data->tables[dnx_data_ingr_congestion_vsq_table_info].doc = "General VSQ info";
    submodule_data->tables[dnx_data_ingr_congestion_vsq_table_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_ingr_congestion_vsq_table_info].size_of_values = sizeof(dnx_data_ingr_congestion_vsq_info_t);
    submodule_data->tables[dnx_data_ingr_congestion_vsq_table_info].entry_get = dnx_data_ingr_congestion_vsq_info_entry_str_get;

    
    submodule_data->tables[dnx_data_ingr_congestion_vsq_table_info].nof_keys = 1;
    submodule_data->tables[dnx_data_ingr_congestion_vsq_table_info].keys[0].name = "vsq_group";
    submodule_data->tables[dnx_data_ingr_congestion_vsq_table_info].keys[0].doc = "vsq_group id as in enum SOC_DNX_ITM_VSQ_GROUP.";

    
    submodule_data->tables[dnx_data_ingr_congestion_vsq_table_info].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_ingr_congestion_vsq_table_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_ingr_congestion_vsq_table_info].nof_values, "_dnx_data_ingr_congestion_vsq_table_info table values");
    submodule_data->tables[dnx_data_ingr_congestion_vsq_table_info].values[0].name = "nof";
    submodule_data->tables[dnx_data_ingr_congestion_vsq_table_info].values[0].type = "uint32";
    submodule_data->tables[dnx_data_ingr_congestion_vsq_table_info].values[0].doc = "Number of VSQs in this group.";
    submodule_data->tables[dnx_data_ingr_congestion_vsq_table_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_ingr_congestion_vsq_info_t, nof);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_ingr_congestion_vsq_feature_get(
    int unit,
    dnx_data_ingr_congestion_vsq_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_vsq, feature);
}


uint32
dnx_data_ingr_congestion_vsq_vsq_rate_class_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_vsq, dnx_data_ingr_congestion_vsq_define_vsq_rate_class_nof);
}

uint32
dnx_data_ingr_congestion_vsq_vsq_a_rate_class_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_vsq, dnx_data_ingr_congestion_vsq_define_vsq_a_rate_class_nof);
}

uint32
dnx_data_ingr_congestion_vsq_vsq_a_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_vsq, dnx_data_ingr_congestion_vsq_define_vsq_a_nof);
}

uint32
dnx_data_ingr_congestion_vsq_vsq_b_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_vsq, dnx_data_ingr_congestion_vsq_define_vsq_b_nof);
}

uint32
dnx_data_ingr_congestion_vsq_vsq_c_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_vsq, dnx_data_ingr_congestion_vsq_define_vsq_c_nof);
}

uint32
dnx_data_ingr_congestion_vsq_vsq_d_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_vsq, dnx_data_ingr_congestion_vsq_define_vsq_d_nof);
}

uint32
dnx_data_ingr_congestion_vsq_vsq_e_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_vsq, dnx_data_ingr_congestion_vsq_define_vsq_e_nof);
}

uint32
dnx_data_ingr_congestion_vsq_vsq_e_hw_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_vsq, dnx_data_ingr_congestion_vsq_define_vsq_e_hw_nof);
}

uint32
dnx_data_ingr_congestion_vsq_vsq_f_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_vsq, dnx_data_ingr_congestion_vsq_define_vsq_f_nof);
}

uint32
dnx_data_ingr_congestion_vsq_vsq_f_hw_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_vsq, dnx_data_ingr_congestion_vsq_define_vsq_f_hw_nof);
}

uint32
dnx_data_ingr_congestion_vsq_nif_vsq_e_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_vsq, dnx_data_ingr_congestion_vsq_define_nif_vsq_e_nof);
}

uint32
dnx_data_ingr_congestion_vsq_non_nif_vsq_f_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_vsq, dnx_data_ingr_congestion_vsq_define_non_nif_vsq_f_nof);
}

uint32
dnx_data_ingr_congestion_vsq_pool_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_vsq, dnx_data_ingr_congestion_vsq_define_pool_nof);
}

uint32
dnx_data_ingr_congestion_vsq_connection_class_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_vsq, dnx_data_ingr_congestion_vsq_define_connection_class_nof);
}

uint32
dnx_data_ingr_congestion_vsq_tc_pg_profile_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_vsq, dnx_data_ingr_congestion_vsq_define_tc_pg_profile_nof);
}

uint32
dnx_data_ingr_congestion_vsq_vsq_e_congestion_notification_fifo_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_vsq, dnx_data_ingr_congestion_vsq_define_vsq_e_congestion_notification_fifo_size);
}

uint32
dnx_data_ingr_congestion_vsq_vsq_f_congestion_notification_fifo_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_vsq, dnx_data_ingr_congestion_vsq_define_vsq_f_congestion_notification_fifo_size);
}

uint32
dnx_data_ingr_congestion_vsq_vsq_e_default_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_vsq, dnx_data_ingr_congestion_vsq_define_vsq_e_default);
}

uint32
dnx_data_ingr_congestion_vsq_vsq_f_default_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_vsq, dnx_data_ingr_congestion_vsq_define_vsq_f_default);
}



const dnx_data_ingr_congestion_vsq_info_t *
dnx_data_ingr_congestion_vsq_info_get(
    int unit,
    int vsq_group)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_vsq, dnx_data_ingr_congestion_vsq_table_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, vsq_group, 0);
    return (const dnx_data_ingr_congestion_vsq_info_t *) data;

}


shr_error_e
dnx_data_ingr_congestion_vsq_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_ingr_congestion_vsq_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_vsq, dnx_data_ingr_congestion_vsq_table_info);
    data = (const dnx_data_ingr_congestion_vsq_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_ingr_congestion_vsq_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_vsq, dnx_data_ingr_congestion_vsq_table_info);

}






static shr_error_e
dnx_data_ingr_congestion_init_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "init";
    submodule_data->doc = "init info";
    
    submodule_data->nof_features = _dnx_data_ingr_congestion_init_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data ingr_congestion init features");

    
    submodule_data->nof_defines = _dnx_data_ingr_congestion_init_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data ingr_congestion init defines");

    submodule_data->defines[dnx_data_ingr_congestion_init_define_fifo_size].name = "fifo_size";
    submodule_data->defines[dnx_data_ingr_congestion_init_define_fifo_size].doc = "size of each tar fifo";
    
    submodule_data->defines[dnx_data_ingr_congestion_init_define_fifo_size].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_ingr_congestion_init_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data ingr_congestion init tables");

    
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].name = "vsq_words_rjct_map";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].doc = "initialization info for VSQ words reject map - defines combinations which should be turn on";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].size_of_values = sizeof(dnx_data_ingr_congestion_init_vsq_words_rjct_map_t);
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].entry_get = dnx_data_ingr_congestion_init_vsq_words_rjct_map_entry_str_get;

    
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].nof_keys = 1;
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].keys[0].name = "index";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].keys[0].doc = "arbitrary index";

    
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].nof_values = 6;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].nof_values, "_dnx_data_ingr_congestion_init_table_vsq_words_rjct_map table values");
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].values[0].name = "is_total_shared_blocked";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].values[0].type = "uint32";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].values[0].doc = "is total shared blocked";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_ingr_congestion_init_vsq_words_rjct_map_t, is_total_shared_blocked);
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].values[1].name = "is_port_pg_shared_blocked";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].values[1].type = "uint32";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].values[1].doc = "is port pg shared blocked";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].values[1].offset = UTILEX_OFFSETOF(dnx_data_ingr_congestion_init_vsq_words_rjct_map_t, is_port_pg_shared_blocked);
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].values[2].name = "is_total_headroom_blocked";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].values[2].type = "uint32";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].values[2].doc = "is total headroom blocked";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].values[2].offset = UTILEX_OFFSETOF(dnx_data_ingr_congestion_init_vsq_words_rjct_map_t, is_total_headroom_blocked);
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].values[3].name = "is_port_pg_headroom_blocked";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].values[3].type = "uint32";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].values[3].doc = "is port pg headroom blocked";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].values[3].offset = UTILEX_OFFSETOF(dnx_data_ingr_congestion_init_vsq_words_rjct_map_t, is_port_pg_headroom_blocked);
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].values[4].name = "is_voq_in_guaranteed";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].values[4].type = "uint32";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].values[4].doc = "is voq in guaranteed";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].values[4].offset = UTILEX_OFFSETOF(dnx_data_ingr_congestion_init_vsq_words_rjct_map_t, is_voq_in_guaranteed);
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].values[5].name = "vsq_guaranteed_status";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].values[5].type = "uint32";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].values[5].doc = "vsq guaranteed status";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_words_rjct_map].values[5].offset = UTILEX_OFFSETOF(dnx_data_ingr_congestion_init_vsq_words_rjct_map_t, vsq_guaranteed_status);

    
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].name = "vsq_sram_rjct_map";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].doc = "initialization info for VSQ SRAM reject map - defines combinations which should be turn on";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].size_of_values = sizeof(dnx_data_ingr_congestion_init_vsq_sram_rjct_map_t);
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].entry_get = dnx_data_ingr_congestion_init_vsq_sram_rjct_map_entry_str_get;

    
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].nof_keys = 1;
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].keys[0].name = "index";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].keys[0].doc = "arbitrary index";

    
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].nof_values = 8;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].nof_values, "_dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map table values");
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].values[0].name = "is_total_shared_blocked";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].values[0].type = "uint32";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].values[0].doc = "is total shared blocked";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_ingr_congestion_init_vsq_sram_rjct_map_t, is_total_shared_blocked);
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].values[1].name = "is_port_pg_shared_blocked";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].values[1].type = "uint32";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].values[1].doc = "is port pg shared blocked";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].values[1].offset = UTILEX_OFFSETOF(dnx_data_ingr_congestion_init_vsq_sram_rjct_map_t, is_port_pg_shared_blocked);
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].values[2].name = "is_headroom_extension_blocked";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].values[2].type = "uint32";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].values[2].doc = "is headroom extension blocked";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].values[2].offset = UTILEX_OFFSETOF(dnx_data_ingr_congestion_init_vsq_sram_rjct_map_t, is_headroom_extension_blocked);
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].values[3].name = "is_total_headroom_blocked";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].values[3].type = "uint32";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].values[3].doc = "is total headroom blocked";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].values[3].offset = UTILEX_OFFSETOF(dnx_data_ingr_congestion_init_vsq_sram_rjct_map_t, is_total_headroom_blocked);
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].values[4].name = "is_port_headroom_blocked";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].values[4].type = "uint32";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].values[4].doc = "is port headroom blocked";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].values[4].offset = UTILEX_OFFSETOF(dnx_data_ingr_congestion_init_vsq_sram_rjct_map_t, is_port_headroom_blocked);
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].values[5].name = "is_pg_headroom_blocked";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].values[5].type = "uint32";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].values[5].doc = "is PG headroom blocked";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].values[5].offset = UTILEX_OFFSETOF(dnx_data_ingr_congestion_init_vsq_sram_rjct_map_t, is_pg_headroom_blocked);
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].values[6].name = "is_voq_in_guaranteed";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].values[6].type = "uint32";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].values[6].doc = "is voq in guaranteed";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].values[6].offset = UTILEX_OFFSETOF(dnx_data_ingr_congestion_init_vsq_sram_rjct_map_t, is_voq_in_guaranteed);
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].values[7].name = "vsq_guaranteed_status";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].values[7].type = "uint32";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].values[7].doc = "vsq guaranteed status";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map].values[7].offset = UTILEX_OFFSETOF(dnx_data_ingr_congestion_init_vsq_sram_rjct_map_t, vsq_guaranteed_status);

    
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_buffer_drop].name = "dp_global_sram_buffer_drop";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_buffer_drop].doc = "Amount of free sram buffer resource to cause drop per DP";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_buffer_drop].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_buffer_drop].size_of_values = sizeof(dnx_data_ingr_congestion_init_dp_global_sram_buffer_drop_t);
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_buffer_drop].entry_get = dnx_data_ingr_congestion_init_dp_global_sram_buffer_drop_entry_str_get;

    
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_buffer_drop].nof_keys = 1;
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_buffer_drop].keys[0].name = "dp";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_buffer_drop].keys[0].doc = "DP (0-3).";

    
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_buffer_drop].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_buffer_drop].values, dnxc_data_value_t, submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_buffer_drop].nof_values, "_dnx_data_ingr_congestion_init_table_dp_global_sram_buffer_drop table values");
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_buffer_drop].values[0].name = "set_threshold";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_buffer_drop].values[0].type = "uint32";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_buffer_drop].values[0].doc = "Amount of free sram buffer resource to set the drop (to support hysteresis)";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_buffer_drop].values[0].offset = UTILEX_OFFSETOF(dnx_data_ingr_congestion_init_dp_global_sram_buffer_drop_t, set_threshold);
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_buffer_drop].values[1].name = "clear_threshold";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_buffer_drop].values[1].type = "uint32";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_buffer_drop].values[1].doc = "Amount of free sram buffer resource to clear the drop (to support hysteresis)";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_buffer_drop].values[1].offset = UTILEX_OFFSETOF(dnx_data_ingr_congestion_init_dp_global_sram_buffer_drop_t, clear_threshold);

    
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_pdb_drop].name = "dp_global_sram_pdb_drop";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_pdb_drop].doc = "Amount of free sram pdb resource to cause drop per DP";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_pdb_drop].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_pdb_drop].size_of_values = sizeof(dnx_data_ingr_congestion_init_dp_global_sram_pdb_drop_t);
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_pdb_drop].entry_get = dnx_data_ingr_congestion_init_dp_global_sram_pdb_drop_entry_str_get;

    
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_pdb_drop].nof_keys = 1;
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_pdb_drop].keys[0].name = "dp";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_pdb_drop].keys[0].doc = "DP (0-3).";

    
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_pdb_drop].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_pdb_drop].values, dnxc_data_value_t, submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_pdb_drop].nof_values, "_dnx_data_ingr_congestion_init_table_dp_global_sram_pdb_drop table values");
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_pdb_drop].values[0].name = "set_threshold";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_pdb_drop].values[0].type = "uint32";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_pdb_drop].values[0].doc = "Amount of free sram pdb resource to set the drop (to support hysteresis)";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_pdb_drop].values[0].offset = UTILEX_OFFSETOF(dnx_data_ingr_congestion_init_dp_global_sram_pdb_drop_t, set_threshold);
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_pdb_drop].values[1].name = "clear_threshold";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_pdb_drop].values[1].type = "uint32";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_pdb_drop].values[1].doc = "Amount of free sram pdb resource to clear the drop (to support hysteresis)";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_sram_pdb_drop].values[1].offset = UTILEX_OFFSETOF(dnx_data_ingr_congestion_init_dp_global_sram_pdb_drop_t, clear_threshold);

    
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_dram_bdb_drop].name = "dp_global_dram_bdb_drop";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_dram_bdb_drop].doc = "Amount of free dram bdb resource to cause drop per DP";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_dram_bdb_drop].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_dram_bdb_drop].size_of_values = sizeof(dnx_data_ingr_congestion_init_dp_global_dram_bdb_drop_t);
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_dram_bdb_drop].entry_get = dnx_data_ingr_congestion_init_dp_global_dram_bdb_drop_entry_str_get;

    
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_dram_bdb_drop].nof_keys = 1;
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_dram_bdb_drop].keys[0].name = "dp";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_dram_bdb_drop].keys[0].doc = "DP (0-3).";

    
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_dram_bdb_drop].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_dram_bdb_drop].values, dnxc_data_value_t, submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_dram_bdb_drop].nof_values, "_dnx_data_ingr_congestion_init_table_dp_global_dram_bdb_drop table values");
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_dram_bdb_drop].values[0].name = "set_threshold";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_dram_bdb_drop].values[0].type = "uint32";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_dram_bdb_drop].values[0].doc = "Amount of free dram bdb resource to set the drop (to support hysteresis)";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_dram_bdb_drop].values[0].offset = UTILEX_OFFSETOF(dnx_data_ingr_congestion_init_dp_global_dram_bdb_drop_t, set_threshold);
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_dram_bdb_drop].values[1].name = "clear_threshold";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_dram_bdb_drop].values[1].type = "uint32";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_dram_bdb_drop].values[1].doc = "Amount of free dram bdb resource to clear the drop (to support hysteresis)";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_dp_global_dram_bdb_drop].values[1].offset = UTILEX_OFFSETOF(dnx_data_ingr_congestion_init_dp_global_dram_bdb_drop_t, clear_threshold);

    
    submodule_data->tables[dnx_data_ingr_congestion_init_table_equivalent_global_drop].name = "equivalent_global_drop";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_equivalent_global_drop].doc = "Equavalent amount per VOQ resource to per-DP global drop (not configurable by user)";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_equivalent_global_drop].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_ingr_congestion_init_table_equivalent_global_drop].size_of_values = sizeof(dnx_data_ingr_congestion_init_equivalent_global_drop_t);
    submodule_data->tables[dnx_data_ingr_congestion_init_table_equivalent_global_drop].entry_get = dnx_data_ingr_congestion_init_equivalent_global_drop_entry_str_get;

    
    submodule_data->tables[dnx_data_ingr_congestion_init_table_equivalent_global_drop].nof_keys = 1;
    submodule_data->tables[dnx_data_ingr_congestion_init_table_equivalent_global_drop].keys[0].name = "type";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_equivalent_global_drop].keys[0].doc = "VOQ resource";

    
    submodule_data->tables[dnx_data_ingr_congestion_init_table_equivalent_global_drop].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_ingr_congestion_init_table_equivalent_global_drop].values, dnxc_data_value_t, submodule_data->tables[dnx_data_ingr_congestion_init_table_equivalent_global_drop].nof_values, "_dnx_data_ingr_congestion_init_table_equivalent_global_drop table values");
    submodule_data->tables[dnx_data_ingr_congestion_init_table_equivalent_global_drop].values[0].name = "drop";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_equivalent_global_drop].values[0].type = "uint32";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_equivalent_global_drop].values[0].doc = "Equavalent amount in VOQ resource to per-DP global drop (not configurable by user) ";
    submodule_data->tables[dnx_data_ingr_congestion_init_table_equivalent_global_drop].values[0].offset = UTILEX_OFFSETOF(dnx_data_ingr_congestion_init_equivalent_global_drop_t, drop);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_ingr_congestion_init_feature_get(
    int unit,
    dnx_data_ingr_congestion_init_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_init, feature);
}


uint32
dnx_data_ingr_congestion_init_fifo_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_init, dnx_data_ingr_congestion_init_define_fifo_size);
}



const dnx_data_ingr_congestion_init_vsq_words_rjct_map_t *
dnx_data_ingr_congestion_init_vsq_words_rjct_map_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_init, dnx_data_ingr_congestion_init_table_vsq_words_rjct_map);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_ingr_congestion_init_vsq_words_rjct_map_t *) data;

}

const dnx_data_ingr_congestion_init_vsq_sram_rjct_map_t *
dnx_data_ingr_congestion_init_vsq_sram_rjct_map_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_init, dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_ingr_congestion_init_vsq_sram_rjct_map_t *) data;

}

const dnx_data_ingr_congestion_init_dp_global_sram_buffer_drop_t *
dnx_data_ingr_congestion_init_dp_global_sram_buffer_drop_get(
    int unit,
    int dp)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_init, dnx_data_ingr_congestion_init_table_dp_global_sram_buffer_drop);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dp, 0);
    return (const dnx_data_ingr_congestion_init_dp_global_sram_buffer_drop_t *) data;

}

const dnx_data_ingr_congestion_init_dp_global_sram_pdb_drop_t *
dnx_data_ingr_congestion_init_dp_global_sram_pdb_drop_get(
    int unit,
    int dp)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_init, dnx_data_ingr_congestion_init_table_dp_global_sram_pdb_drop);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dp, 0);
    return (const dnx_data_ingr_congestion_init_dp_global_sram_pdb_drop_t *) data;

}

const dnx_data_ingr_congestion_init_dp_global_dram_bdb_drop_t *
dnx_data_ingr_congestion_init_dp_global_dram_bdb_drop_get(
    int unit,
    int dp)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_init, dnx_data_ingr_congestion_init_table_dp_global_dram_bdb_drop);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dp, 0);
    return (const dnx_data_ingr_congestion_init_dp_global_dram_bdb_drop_t *) data;

}

const dnx_data_ingr_congestion_init_equivalent_global_drop_t *
dnx_data_ingr_congestion_init_equivalent_global_drop_get(
    int unit,
    int type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_init, dnx_data_ingr_congestion_init_table_equivalent_global_drop);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, type, 0);
    return (const dnx_data_ingr_congestion_init_equivalent_global_drop_t *) data;

}


shr_error_e
dnx_data_ingr_congestion_init_vsq_words_rjct_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_ingr_congestion_init_vsq_words_rjct_map_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_init, dnx_data_ingr_congestion_init_table_vsq_words_rjct_map);
    data = (const dnx_data_ingr_congestion_init_vsq_words_rjct_map_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_total_shared_blocked);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_port_pg_shared_blocked);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_total_headroom_blocked);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_port_pg_headroom_blocked);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_voq_in_guaranteed);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->vsq_guaranteed_status);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_ingr_congestion_init_vsq_sram_rjct_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_ingr_congestion_init_vsq_sram_rjct_map_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_init, dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map);
    data = (const dnx_data_ingr_congestion_init_vsq_sram_rjct_map_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_total_shared_blocked);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_port_pg_shared_blocked);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_headroom_extension_blocked);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_total_headroom_blocked);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_port_headroom_blocked);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_pg_headroom_blocked);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_voq_in_guaranteed);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->vsq_guaranteed_status);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_ingr_congestion_init_dp_global_sram_buffer_drop_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_ingr_congestion_init_dp_global_sram_buffer_drop_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_init, dnx_data_ingr_congestion_init_table_dp_global_sram_buffer_drop);
    data = (const dnx_data_ingr_congestion_init_dp_global_sram_buffer_drop_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->set_threshold);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->clear_threshold);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_ingr_congestion_init_dp_global_sram_pdb_drop_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_ingr_congestion_init_dp_global_sram_pdb_drop_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_init, dnx_data_ingr_congestion_init_table_dp_global_sram_pdb_drop);
    data = (const dnx_data_ingr_congestion_init_dp_global_sram_pdb_drop_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->set_threshold);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->clear_threshold);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_ingr_congestion_init_dp_global_dram_bdb_drop_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_ingr_congestion_init_dp_global_dram_bdb_drop_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_init, dnx_data_ingr_congestion_init_table_dp_global_dram_bdb_drop);
    data = (const dnx_data_ingr_congestion_init_dp_global_dram_bdb_drop_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->set_threshold);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->clear_threshold);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_ingr_congestion_init_equivalent_global_drop_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_ingr_congestion_init_equivalent_global_drop_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_init, dnx_data_ingr_congestion_init_table_equivalent_global_drop);
    data = (const dnx_data_ingr_congestion_init_equivalent_global_drop_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->drop);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_ingr_congestion_init_vsq_words_rjct_map_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_init, dnx_data_ingr_congestion_init_table_vsq_words_rjct_map);

}

const dnxc_data_table_info_t *
dnx_data_ingr_congestion_init_vsq_sram_rjct_map_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_init, dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map);

}

const dnxc_data_table_info_t *
dnx_data_ingr_congestion_init_dp_global_sram_buffer_drop_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_init, dnx_data_ingr_congestion_init_table_dp_global_sram_buffer_drop);

}

const dnxc_data_table_info_t *
dnx_data_ingr_congestion_init_dp_global_sram_pdb_drop_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_init, dnx_data_ingr_congestion_init_table_dp_global_sram_pdb_drop);

}

const dnxc_data_table_info_t *
dnx_data_ingr_congestion_init_dp_global_dram_bdb_drop_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_init, dnx_data_ingr_congestion_init_table_dp_global_dram_bdb_drop);

}

const dnxc_data_table_info_t *
dnx_data_ingr_congestion_init_equivalent_global_drop_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_init, dnx_data_ingr_congestion_init_table_equivalent_global_drop);

}






static shr_error_e
dnx_data_ingr_congestion_dbal_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "dbal";
    submodule_data->doc = "dbal info";
    
    submodule_data->nof_features = _dnx_data_ingr_congestion_dbal_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data ingr_congestion dbal features");

    
    submodule_data->nof_defines = _dnx_data_ingr_congestion_dbal_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data ingr_congestion dbal defines");

    submodule_data->defines[dnx_data_ingr_congestion_dbal_define_admission_test_nof].name = "admission_test_nof";
    submodule_data->defines[dnx_data_ingr_congestion_dbal_define_admission_test_nof].doc = "Number of admission test";
    
    submodule_data->defines[dnx_data_ingr_congestion_dbal_define_admission_test_nof].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_ingr_congestion_dbal_define_dram_bdbs_th_nof_bits].name = "dram_bdbs_th_nof_bits";
    submodule_data->defines[dnx_data_ingr_congestion_dbal_define_dram_bdbs_th_nof_bits].doc = "Number of bits dram bdbs set/clear threshold";
    
    submodule_data->defines[dnx_data_ingr_congestion_dbal_define_dram_bdbs_th_nof_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_ingr_congestion_dbal_define_sram_pdbs_th_nof_bits].name = "sram_pdbs_th_nof_bits";
    submodule_data->defines[dnx_data_ingr_congestion_dbal_define_sram_pdbs_th_nof_bits].doc = "Number of bits sram pdbs set/clear threshold";
    
    submodule_data->defines[dnx_data_ingr_congestion_dbal_define_sram_pdbs_th_nof_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_ingr_congestion_dbal_define_sram_buffer_th_nof_bits].name = "sram_buffer_th_nof_bits";
    submodule_data->defines[dnx_data_ingr_congestion_dbal_define_sram_buffer_th_nof_bits].doc = "Number of bits sram buffer set/clear threshold";
    
    submodule_data->defines[dnx_data_ingr_congestion_dbal_define_sram_buffer_th_nof_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_ingr_congestion_dbal_define_sram_buffer_free_th_nof_bits].name = "sram_buffer_free_th_nof_bits";
    submodule_data->defines[dnx_data_ingr_congestion_dbal_define_sram_buffer_free_th_nof_bits].doc = "Number of bits sram buffer set/clear threshold for reaching full size of the resource";
    
    submodule_data->defines[dnx_data_ingr_congestion_dbal_define_sram_buffer_free_th_nof_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_ingr_congestion_dbal_define_sram_pds_th_nof_bits].name = "sram_pds_th_nof_bits";
    submodule_data->defines[dnx_data_ingr_congestion_dbal_define_sram_pds_th_nof_bits].doc = "Number of bits sram PDs set/clear threshold";
    
    submodule_data->defines[dnx_data_ingr_congestion_dbal_define_sram_pds_th_nof_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_ingr_congestion_dbal_define_total_bytes_th_nof_bits].name = "total_bytes_th_nof_bits";
    submodule_data->defines[dnx_data_ingr_congestion_dbal_define_total_bytes_th_nof_bits].doc = "Number of bits total bytes set/clear threshold";
    
    submodule_data->defines[dnx_data_ingr_congestion_dbal_define_total_bytes_th_nof_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_ingr_congestion_dbal_define_dram_bdbs_nof_bits].name = "dram_bdbs_nof_bits";
    submodule_data->defines[dnx_data_ingr_congestion_dbal_define_dram_bdbs_nof_bits].doc = "Number of bits for dram bdbs";
    
    submodule_data->defines[dnx_data_ingr_congestion_dbal_define_dram_bdbs_nof_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_ingr_congestion_dbal_define_sram_pdbs_nof_bits].name = "sram_pdbs_nof_bits";
    submodule_data->defines[dnx_data_ingr_congestion_dbal_define_sram_pdbs_nof_bits].doc = "Number of bits for sram pdbs";
    
    submodule_data->defines[dnx_data_ingr_congestion_dbal_define_sram_pdbs_nof_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_ingr_congestion_dbal_define_sram_buffer_nof_bits].name = "sram_buffer_nof_bits";
    submodule_data->defines[dnx_data_ingr_congestion_dbal_define_sram_buffer_nof_bits].doc = "Number of bits for sram buffer";
    
    submodule_data->defines[dnx_data_ingr_congestion_dbal_define_sram_buffer_nof_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_ingr_congestion_dbal_define_sqm_debug_pkt_ctr_nof_bits].name = "sqm_debug_pkt_ctr_nof_bits";
    submodule_data->defines[dnx_data_ingr_congestion_dbal_define_sqm_debug_pkt_ctr_nof_bits].doc = "Number of bits for SRAM Queue Manager enqueued/dequeued packets degug counters";
    
    submodule_data->defines[dnx_data_ingr_congestion_dbal_define_sqm_debug_pkt_ctr_nof_bits].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_ingr_congestion_dbal_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data ingr_congestion dbal tables");

    
    submodule_data->tables[dnx_data_ingr_congestion_dbal_table_admission_bits_mapping].name = "admission_bits_mapping";
    submodule_data->tables[dnx_data_ingr_congestion_dbal_table_admission_bits_mapping].doc = "Mapping of bits in addmission mask";
    submodule_data->tables[dnx_data_ingr_congestion_dbal_table_admission_bits_mapping].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_ingr_congestion_dbal_table_admission_bits_mapping].size_of_values = sizeof(dnx_data_ingr_congestion_dbal_admission_bits_mapping_t);
    submodule_data->tables[dnx_data_ingr_congestion_dbal_table_admission_bits_mapping].entry_get = dnx_data_ingr_congestion_dbal_admission_bits_mapping_entry_str_get;

    
    submodule_data->tables[dnx_data_ingr_congestion_dbal_table_admission_bits_mapping].nof_keys = 1;
    submodule_data->tables[dnx_data_ingr_congestion_dbal_table_admission_bits_mapping].keys[0].name = "rjct_bit";
    submodule_data->tables[dnx_data_ingr_congestion_dbal_table_admission_bits_mapping].keys[0].doc = "admission bit - defined by dnx_ingress_congestion_reject_bit_e";

    
    submodule_data->tables[dnx_data_ingr_congestion_dbal_table_admission_bits_mapping].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_ingr_congestion_dbal_table_admission_bits_mapping].values, dnxc_data_value_t, submodule_data->tables[dnx_data_ingr_congestion_dbal_table_admission_bits_mapping].nof_values, "_dnx_data_ingr_congestion_dbal_table_admission_bits_mapping table values");
    submodule_data->tables[dnx_data_ingr_congestion_dbal_table_admission_bits_mapping].values[0].name = "index";
    submodule_data->tables[dnx_data_ingr_congestion_dbal_table_admission_bits_mapping].values[0].type = "int";
    submodule_data->tables[dnx_data_ingr_congestion_dbal_table_admission_bits_mapping].values[0].doc = "the offset of the bit in the mask";
    submodule_data->tables[dnx_data_ingr_congestion_dbal_table_admission_bits_mapping].values[0].offset = UTILEX_OFFSETOF(dnx_data_ingr_congestion_dbal_admission_bits_mapping_t, index);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_ingr_congestion_dbal_feature_get(
    int unit,
    dnx_data_ingr_congestion_dbal_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_dbal, feature);
}


uint32
dnx_data_ingr_congestion_dbal_admission_test_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_dbal, dnx_data_ingr_congestion_dbal_define_admission_test_nof);
}

uint32
dnx_data_ingr_congestion_dbal_dram_bdbs_th_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_dbal, dnx_data_ingr_congestion_dbal_define_dram_bdbs_th_nof_bits);
}

uint32
dnx_data_ingr_congestion_dbal_sram_pdbs_th_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_dbal, dnx_data_ingr_congestion_dbal_define_sram_pdbs_th_nof_bits);
}

uint32
dnx_data_ingr_congestion_dbal_sram_buffer_th_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_dbal, dnx_data_ingr_congestion_dbal_define_sram_buffer_th_nof_bits);
}

uint32
dnx_data_ingr_congestion_dbal_sram_buffer_free_th_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_dbal, dnx_data_ingr_congestion_dbal_define_sram_buffer_free_th_nof_bits);
}

uint32
dnx_data_ingr_congestion_dbal_sram_pds_th_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_dbal, dnx_data_ingr_congestion_dbal_define_sram_pds_th_nof_bits);
}

uint32
dnx_data_ingr_congestion_dbal_total_bytes_th_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_dbal, dnx_data_ingr_congestion_dbal_define_total_bytes_th_nof_bits);
}

uint32
dnx_data_ingr_congestion_dbal_dram_bdbs_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_dbal, dnx_data_ingr_congestion_dbal_define_dram_bdbs_nof_bits);
}

uint32
dnx_data_ingr_congestion_dbal_sram_pdbs_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_dbal, dnx_data_ingr_congestion_dbal_define_sram_pdbs_nof_bits);
}

uint32
dnx_data_ingr_congestion_dbal_sram_buffer_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_dbal, dnx_data_ingr_congestion_dbal_define_sram_buffer_nof_bits);
}

uint32
dnx_data_ingr_congestion_dbal_sqm_debug_pkt_ctr_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_dbal, dnx_data_ingr_congestion_dbal_define_sqm_debug_pkt_ctr_nof_bits);
}



const dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *
dnx_data_ingr_congestion_dbal_admission_bits_mapping_get(
    int unit,
    int rjct_bit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_dbal, dnx_data_ingr_congestion_dbal_table_admission_bits_mapping);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, rjct_bit, 0);
    return (const dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) data;

}


shr_error_e
dnx_data_ingr_congestion_dbal_admission_bits_mapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_dbal, dnx_data_ingr_congestion_dbal_table_admission_bits_mapping);
    data = (const dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->index);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_ingr_congestion_dbal_admission_bits_mapping_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_dbal, dnx_data_ingr_congestion_dbal_table_admission_bits_mapping);

}






static shr_error_e
dnx_data_ingr_congestion_mirror_on_drop_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "mirror_on_drop";
    submodule_data->doc = "mirror on drop feature";
    
    submodule_data->nof_features = _dnx_data_ingr_congestion_mirror_on_drop_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data ingr_congestion mirror_on_drop features");

    submodule_data->features[dnx_data_ingr_congestion_mirror_on_drop_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_ingr_congestion_mirror_on_drop_is_supported].doc = "Indication whether mirror on drop feature is supported";
    submodule_data->features[dnx_data_ingr_congestion_mirror_on_drop_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_ingr_congestion_mirror_on_drop_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data ingr_congestion mirror_on_drop defines");

    submodule_data->defines[dnx_data_ingr_congestion_mirror_on_drop_define_nof_groups].name = "nof_groups";
    submodule_data->defines[dnx_data_ingr_congestion_mirror_on_drop_define_nof_groups].doc = "number of drop groups";
    
    submodule_data->defines[dnx_data_ingr_congestion_mirror_on_drop_define_nof_groups].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_congestion_mirror_on_drop_define_aging_clocks_resolution].name = "aging_clocks_resolution";
    submodule_data->defines[dnx_data_ingr_congestion_mirror_on_drop_define_aging_clocks_resolution].doc = "resolution of aging clock cycles";
    
    submodule_data->defines[dnx_data_ingr_congestion_mirror_on_drop_define_aging_clocks_resolution].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_ingr_congestion_mirror_on_drop_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data ingr_congestion mirror_on_drop tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_ingr_congestion_mirror_on_drop_feature_get(
    int unit,
    dnx_data_ingr_congestion_mirror_on_drop_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_mirror_on_drop, feature);
}


uint32
dnx_data_ingr_congestion_mirror_on_drop_nof_groups_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_mirror_on_drop, dnx_data_ingr_congestion_mirror_on_drop_define_nof_groups);
}

uint32
dnx_data_ingr_congestion_mirror_on_drop_aging_clocks_resolution_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_mirror_on_drop, dnx_data_ingr_congestion_mirror_on_drop_define_aging_clocks_resolution);
}










static shr_error_e
dnx_data_ingr_congestion_sram_buffer_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "sram_buffer";
    submodule_data->doc = "sram buffers information";
    
    submodule_data->nof_features = _dnx_data_ingr_congestion_sram_buffer_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data ingr_congestion sram_buffer features");

    
    submodule_data->nof_defines = _dnx_data_ingr_congestion_sram_buffer_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data ingr_congestion sram_buffer defines");

    
    submodule_data->nof_tables = _dnx_data_ingr_congestion_sram_buffer_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data ingr_congestion sram_buffer tables");

    
    submodule_data->tables[dnx_data_ingr_congestion_sram_buffer_table_drop_tresholds].name = "drop_tresholds";
    submodule_data->tables[dnx_data_ingr_congestion_sram_buffer_table_drop_tresholds].doc = "free sram buffers drop thresholds";
    submodule_data->tables[dnx_data_ingr_congestion_sram_buffer_table_drop_tresholds].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_ingr_congestion_sram_buffer_table_drop_tresholds].size_of_values = sizeof(dnx_data_ingr_congestion_sram_buffer_drop_tresholds_t);
    submodule_data->tables[dnx_data_ingr_congestion_sram_buffer_table_drop_tresholds].entry_get = dnx_data_ingr_congestion_sram_buffer_drop_tresholds_entry_str_get;

    
    submodule_data->tables[dnx_data_ingr_congestion_sram_buffer_table_drop_tresholds].nof_keys = 1;
    submodule_data->tables[dnx_data_ingr_congestion_sram_buffer_table_drop_tresholds].keys[0].name = "index";
    submodule_data->tables[dnx_data_ingr_congestion_sram_buffer_table_drop_tresholds].keys[0].doc = "threshold index";

    
    submodule_data->tables[dnx_data_ingr_congestion_sram_buffer_table_drop_tresholds].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_ingr_congestion_sram_buffer_table_drop_tresholds].values, dnxc_data_value_t, submodule_data->tables[dnx_data_ingr_congestion_sram_buffer_table_drop_tresholds].nof_values, "_dnx_data_ingr_congestion_sram_buffer_table_drop_tresholds table values");
    submodule_data->tables[dnx_data_ingr_congestion_sram_buffer_table_drop_tresholds].values[0].name = "value";
    submodule_data->tables[dnx_data_ingr_congestion_sram_buffer_table_drop_tresholds].values[0].type = "uint32";
    submodule_data->tables[dnx_data_ingr_congestion_sram_buffer_table_drop_tresholds].values[0].doc = "threshold value";
    submodule_data->tables[dnx_data_ingr_congestion_sram_buffer_table_drop_tresholds].values[0].offset = UTILEX_OFFSETOF(dnx_data_ingr_congestion_sram_buffer_drop_tresholds_t, value);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_ingr_congestion_sram_buffer_feature_get(
    int unit,
    dnx_data_ingr_congestion_sram_buffer_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_sram_buffer, feature);
}




const dnx_data_ingr_congestion_sram_buffer_drop_tresholds_t *
dnx_data_ingr_congestion_sram_buffer_drop_tresholds_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_sram_buffer, dnx_data_ingr_congestion_sram_buffer_table_drop_tresholds);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_ingr_congestion_sram_buffer_drop_tresholds_t *) data;

}


shr_error_e
dnx_data_ingr_congestion_sram_buffer_drop_tresholds_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_ingr_congestion_sram_buffer_drop_tresholds_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_sram_buffer, dnx_data_ingr_congestion_sram_buffer_table_drop_tresholds);
    data = (const dnx_data_ingr_congestion_sram_buffer_drop_tresholds_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->value);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_ingr_congestion_sram_buffer_drop_tresholds_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_ingr_congestion, dnx_data_ingr_congestion_submodule_sram_buffer, dnx_data_ingr_congestion_sram_buffer_table_drop_tresholds);

}



shr_error_e
dnx_data_ingr_congestion_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "ingr_congestion";
    module_data->nof_submodules = _dnx_data_ingr_congestion_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data ingr_congestion submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_ingr_congestion_config_init(unit, &module_data->submodules[dnx_data_ingr_congestion_submodule_config]));
    SHR_IF_ERR_EXIT(dnx_data_ingr_congestion_info_init(unit, &module_data->submodules[dnx_data_ingr_congestion_submodule_info]));
    SHR_IF_ERR_EXIT(dnx_data_ingr_congestion_fadt_tail_drop_init(unit, &module_data->submodules[dnx_data_ingr_congestion_submodule_fadt_tail_drop]));
    SHR_IF_ERR_EXIT(dnx_data_ingr_congestion_dram_bound_init(unit, &module_data->submodules[dnx_data_ingr_congestion_submodule_dram_bound]));
    SHR_IF_ERR_EXIT(dnx_data_ingr_congestion_voq_init(unit, &module_data->submodules[dnx_data_ingr_congestion_submodule_voq]));
    SHR_IF_ERR_EXIT(dnx_data_ingr_congestion_vsq_init(unit, &module_data->submodules[dnx_data_ingr_congestion_submodule_vsq]));
    SHR_IF_ERR_EXIT(dnx_data_ingr_congestion_init_init(unit, &module_data->submodules[dnx_data_ingr_congestion_submodule_init]));
    SHR_IF_ERR_EXIT(dnx_data_ingr_congestion_dbal_init(unit, &module_data->submodules[dnx_data_ingr_congestion_submodule_dbal]));
    SHR_IF_ERR_EXIT(dnx_data_ingr_congestion_mirror_on_drop_init(unit, &module_data->submodules[dnx_data_ingr_congestion_submodule_mirror_on_drop]));
    SHR_IF_ERR_EXIT(dnx_data_ingr_congestion_sram_buffer_init(unit, &module_data->submodules[dnx_data_ingr_congestion_submodule_sram_buffer]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingr_congestion_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingr_congestion_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_ingr_congestion_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingr_congestion_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_ingr_congestion_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingr_congestion_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_ingr_congestion_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingr_congestion_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_ingr_congestion_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a1_data_ingr_congestion_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingr_congestion_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_ingr_congestion_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingr_congestion_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_ingr_congestion_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingr_congestion_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_ingr_congestion_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b1_data_ingr_congestion_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingr_congestion_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_ingr_congestion_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

