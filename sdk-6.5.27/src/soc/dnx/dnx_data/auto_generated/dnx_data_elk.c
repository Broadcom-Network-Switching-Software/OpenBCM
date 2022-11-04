
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_ELKDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_elk.h>




extern shr_error_e jr2_a0_data_elk_attach(
    int unit);


extern shr_error_e j2c_a0_data_elk_attach(
    int unit);


extern shr_error_e q2a_a0_data_elk_attach(
    int unit);


extern shr_error_e j2p_a0_data_elk_attach(
    int unit);


extern shr_error_e j2x_a0_data_elk_attach(
    int unit);




static shr_error_e
dnx_data_elk_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_elk_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data elk general features");

    submodule_data->features[dnx_data_elk_general_is_egw_block_exist].name = "is_egw_block_exist";
    submodule_data->features[dnx_data_elk_general_is_egw_block_exist].doc = "";
    submodule_data->features[dnx_data_elk_general_is_egw_block_exist].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_elk_general_is_egw_support_rr].name = "is_egw_support_rr";
    submodule_data->features[dnx_data_elk_general_is_egw_support_rr].doc = "";
    submodule_data->features[dnx_data_elk_general_is_egw_support_rr].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_elk_general_is_kbp_traffic_supported].name = "is_kbp_traffic_supported";
    submodule_data->features[dnx_data_elk_general_is_kbp_traffic_supported].doc = "";
    submodule_data->features[dnx_data_elk_general_is_kbp_traffic_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_elk_general_is_kbp_supported].name = "is_kbp_supported";
    submodule_data->features[dnx_data_elk_general_is_kbp_supported].doc = "";
    submodule_data->features[dnx_data_elk_general_is_kbp_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_elk_general_elk_nif_if_type_sel].name = "elk_nif_if_type_sel";
    submodule_data->features[dnx_data_elk_general_elk_nif_if_type_sel].doc = "";
    submodule_data->features[dnx_data_elk_general_elk_nif_if_type_sel].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_elk_general_back_pressure_bit_exists].name = "back_pressure_bit_exists";
    submodule_data->features[dnx_data_elk_general_back_pressure_bit_exists].doc = "";
    submodule_data->features[dnx_data_elk_general_back_pressure_bit_exists].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_elk_general_are_two_kbps_connected].name = "are_two_kbps_connected";
    submodule_data->features[dnx_data_elk_general_are_two_kbps_connected].doc = "";
    submodule_data->features[dnx_data_elk_general_are_two_kbps_connected].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_elk_general_hard_reset_after_kbp_soft_reset_fail].name = "hard_reset_after_kbp_soft_reset_fail";
    submodule_data->features[dnx_data_elk_general_hard_reset_after_kbp_soft_reset_fail].doc = "";
    submodule_data->features[dnx_data_elk_general_hard_reset_after_kbp_soft_reset_fail].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_elk_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data elk general defines");

    submodule_data->defines[dnx_data_elk_general_define_max_ports].name = "max_ports";
    submodule_data->defines[dnx_data_elk_general_define_max_ports].doc = "";
    
    submodule_data->defines[dnx_data_elk_general_define_max_ports].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_elk_general_define_rop_fifo_depth].name = "rop_fifo_depth";
    submodule_data->defines[dnx_data_elk_general_define_rop_fifo_depth].doc = "";
    
    submodule_data->defines[dnx_data_elk_general_define_rop_fifo_depth].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_elk_general_define_flp_full_treshold].name = "flp_full_treshold";
    submodule_data->defines[dnx_data_elk_general_define_flp_full_treshold].doc = "";
    
    submodule_data->defines[dnx_data_elk_general_define_flp_full_treshold].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_elk_general_define_max_lane_id].name = "max_lane_id";
    submodule_data->defines[dnx_data_elk_general_define_max_lane_id].doc = "";
    
    submodule_data->defines[dnx_data_elk_general_define_max_lane_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_elk_general_define_egw_max_opcode].name = "egw_max_opcode";
    submodule_data->defines[dnx_data_elk_general_define_egw_max_opcode].doc = "";
    
    submodule_data->defines[dnx_data_elk_general_define_egw_max_opcode].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_elk_general_define_max_kbp_instance].name = "max_kbp_instance";
    submodule_data->defines[dnx_data_elk_general_define_max_kbp_instance].doc = "";
    
    submodule_data->defines[dnx_data_elk_general_define_max_kbp_instance].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_elk_general_define_lookup_timeout_delay].name = "lookup_timeout_delay";
    submodule_data->defines[dnx_data_elk_general_define_lookup_timeout_delay].doc = "";
    
    submodule_data->defines[dnx_data_elk_general_define_lookup_timeout_delay].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_elk_general_define_trans_timer_delay].name = "trans_timer_delay";
    submodule_data->defines[dnx_data_elk_general_define_trans_timer_delay].doc = "";
    
    submodule_data->defines[dnx_data_elk_general_define_trans_timer_delay].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_elk_general_define_nof_dbs_per_core].name = "nof_dbs_per_core";
    submodule_data->defines[dnx_data_elk_general_define_nof_dbs_per_core].doc = "";
    
    submodule_data->defines[dnx_data_elk_general_define_nof_dbs_per_core].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_elk_general_define_device_type].name = "device_type";
    submodule_data->defines[dnx_data_elk_general_define_device_type].doc = "";
    
    submodule_data->defines[dnx_data_elk_general_define_device_type].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_elk_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data elk general tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_elk_general_feature_get(
    int unit,
    dnx_data_elk_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_general, feature);
}


uint32
dnx_data_elk_general_max_ports_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_general, dnx_data_elk_general_define_max_ports);
}

uint32
dnx_data_elk_general_rop_fifo_depth_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_general, dnx_data_elk_general_define_rop_fifo_depth);
}

uint32
dnx_data_elk_general_flp_full_treshold_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_general, dnx_data_elk_general_define_flp_full_treshold);
}

uint32
dnx_data_elk_general_max_lane_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_general, dnx_data_elk_general_define_max_lane_id);
}

uint32
dnx_data_elk_general_egw_max_opcode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_general, dnx_data_elk_general_define_egw_max_opcode);
}

uint32
dnx_data_elk_general_max_kbp_instance_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_general, dnx_data_elk_general_define_max_kbp_instance);
}

uint32
dnx_data_elk_general_lookup_timeout_delay_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_general, dnx_data_elk_general_define_lookup_timeout_delay);
}

uint32
dnx_data_elk_general_trans_timer_delay_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_general, dnx_data_elk_general_define_trans_timer_delay);
}

uint32
dnx_data_elk_general_nof_dbs_per_core_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_general, dnx_data_elk_general_define_nof_dbs_per_core);
}

uint32
dnx_data_elk_general_device_type_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_general, dnx_data_elk_general_define_device_type);
}










static shr_error_e
dnx_data_elk_application_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "application";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_elk_application_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data elk application features");

    submodule_data->features[dnx_data_elk_application_ipv4].name = "ipv4";
    submodule_data->features[dnx_data_elk_application_ipv4].doc = "";
    submodule_data->features[dnx_data_elk_application_ipv4].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_elk_application_ipv6].name = "ipv6";
    submodule_data->features[dnx_data_elk_application_ipv6].doc = "";
    submodule_data->features[dnx_data_elk_application_ipv6].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_elk_application_nat].name = "nat";
    submodule_data->features[dnx_data_elk_application_nat].doc = "";
    submodule_data->features[dnx_data_elk_application_nat].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_elk_application_sllb].name = "sllb";
    submodule_data->features[dnx_data_elk_application_sllb].doc = "";
    submodule_data->features[dnx_data_elk_application_sllb].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_elk_application_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data elk application defines");

    submodule_data->defines[dnx_data_elk_application_define_large_mc_mode1].name = "large_mc_mode1";
    submodule_data->defines[dnx_data_elk_application_define_large_mc_mode1].doc = "";
    
    submodule_data->defines[dnx_data_elk_application_define_large_mc_mode1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_elk_application_define_large_mc_mode2].name = "large_mc_mode2";
    submodule_data->defines[dnx_data_elk_application_define_large_mc_mode2].doc = "";
    
    submodule_data->defines[dnx_data_elk_application_define_large_mc_mode2].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_elk_application_define_large_mc_mode3].name = "large_mc_mode3";
    submodule_data->defines[dnx_data_elk_application_define_large_mc_mode3].doc = "";
    
    submodule_data->defines[dnx_data_elk_application_define_large_mc_mode3].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_elk_application_define_optimized_result].name = "optimized_result";
    submodule_data->defines[dnx_data_elk_application_define_optimized_result].doc = "";
    
    submodule_data->defines[dnx_data_elk_application_define_optimized_result].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_elk_application_define_split_rpf].name = "split_rpf";
    submodule_data->defines[dnx_data_elk_application_define_split_rpf].doc = "";
    
    submodule_data->defines[dnx_data_elk_application_define_split_rpf].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_elk_application_define_thread_safety].name = "thread_safety";
    submodule_data->defines[dnx_data_elk_application_define_thread_safety].doc = "";
    
    submodule_data->defines[dnx_data_elk_application_define_thread_safety].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_elk_application_define_ipv4_large_mc_enable].name = "ipv4_large_mc_enable";
    submodule_data->defines[dnx_data_elk_application_define_ipv4_large_mc_enable].doc = "";
    
    submodule_data->defines[dnx_data_elk_application_define_ipv4_large_mc_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_elk_application_define_ipv6_large_mc_enable].name = "ipv6_large_mc_enable";
    submodule_data->defines[dnx_data_elk_application_define_ipv6_large_mc_enable].doc = "";
    
    submodule_data->defines[dnx_data_elk_application_define_ipv6_large_mc_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_elk_application_define_ipv4_mc_hash_table_size].name = "ipv4_mc_hash_table_size";
    submodule_data->defines[dnx_data_elk_application_define_ipv4_mc_hash_table_size].doc = "";
    
    submodule_data->defines[dnx_data_elk_application_define_ipv4_mc_hash_table_size].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_elk_application_define_ipv6_mc_hash_table_size].name = "ipv6_mc_hash_table_size";
    submodule_data->defines[dnx_data_elk_application_define_ipv6_mc_hash_table_size].doc = "";
    
    submodule_data->defines[dnx_data_elk_application_define_ipv6_mc_hash_table_size].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_elk_application_define_ipv4_mc_table_size].name = "ipv4_mc_table_size";
    submodule_data->defines[dnx_data_elk_application_define_ipv4_mc_table_size].doc = "";
    
    submodule_data->defines[dnx_data_elk_application_define_ipv4_mc_table_size].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_elk_application_define_ipv6_mc_table_size].name = "ipv6_mc_table_size";
    submodule_data->defines[dnx_data_elk_application_define_ipv6_mc_table_size].doc = "";
    
    submodule_data->defines[dnx_data_elk_application_define_ipv6_mc_table_size].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_elk_application_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data elk application tables");

    
    submodule_data->tables[dnx_data_elk_application_table_db_property].name = "db_property";
    submodule_data->tables[dnx_data_elk_application_table_db_property].doc = "";
    submodule_data->tables[dnx_data_elk_application_table_db_property].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_elk_application_table_db_property].size_of_values = sizeof(dnx_data_elk_application_db_property_t);
    submodule_data->tables[dnx_data_elk_application_table_db_property].entry_get = dnx_data_elk_application_db_property_entry_str_get;

    
    submodule_data->tables[dnx_data_elk_application_table_db_property].nof_keys = 1;
    submodule_data->tables[dnx_data_elk_application_table_db_property].keys[0].name = "db";
    submodule_data->tables[dnx_data_elk_application_table_db_property].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_elk_application_table_db_property].nof_values = 6;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_elk_application_table_db_property].values, dnxc_data_value_t, submodule_data->tables[dnx_data_elk_application_table_db_property].nof_values, "_dnx_data_elk_application_table_db_property table values");
    submodule_data->tables[dnx_data_elk_application_table_db_property].values[0].name = "initial_regular_capacity";
    submodule_data->tables[dnx_data_elk_application_table_db_property].values[0].type = "int";
    submodule_data->tables[dnx_data_elk_application_table_db_property].values[0].doc = "";
    submodule_data->tables[dnx_data_elk_application_table_db_property].values[0].offset = UTILEX_OFFSETOF(dnx_data_elk_application_db_property_t, initial_regular_capacity);
    submodule_data->tables[dnx_data_elk_application_table_db_property].values[1].name = "initial_optimized_capacity";
    submodule_data->tables[dnx_data_elk_application_table_db_property].values[1].type = "int";
    submodule_data->tables[dnx_data_elk_application_table_db_property].values[1].doc = "";
    submodule_data->tables[dnx_data_elk_application_table_db_property].values[1].offset = UTILEX_OFFSETOF(dnx_data_elk_application_db_property_t, initial_optimized_capacity);
    submodule_data->tables[dnx_data_elk_application_table_db_property].values[2].name = "use_large_optimized_result";
    submodule_data->tables[dnx_data_elk_application_table_db_property].values[2].type = "int";
    submodule_data->tables[dnx_data_elk_application_table_db_property].values[2].doc = "";
    submodule_data->tables[dnx_data_elk_application_table_db_property].values[2].offset = UTILEX_OFFSETOF(dnx_data_elk_application_db_property_t, use_large_optimized_result);
    submodule_data->tables[dnx_data_elk_application_table_db_property].values[3].name = "xl_db";
    submodule_data->tables[dnx_data_elk_application_table_db_property].values[3].type = "int";
    submodule_data->tables[dnx_data_elk_application_table_db_property].values[3].doc = "";
    submodule_data->tables[dnx_data_elk_application_table_db_property].values[3].offset = UTILEX_OFFSETOF(dnx_data_elk_application_db_property_t, xl_db);
    submodule_data->tables[dnx_data_elk_application_table_db_property].values[4].name = "locality";
    submodule_data->tables[dnx_data_elk_application_table_db_property].values[4].type = "int";
    submodule_data->tables[dnx_data_elk_application_table_db_property].values[4].doc = "";
    submodule_data->tables[dnx_data_elk_application_table_db_property].values[4].offset = UTILEX_OFFSETOF(dnx_data_elk_application_db_property_t, locality);
    submodule_data->tables[dnx_data_elk_application_table_db_property].values[5].name = "zero_size_ad";
    submodule_data->tables[dnx_data_elk_application_table_db_property].values[5].type = "int";
    submodule_data->tables[dnx_data_elk_application_table_db_property].values[5].doc = "";
    submodule_data->tables[dnx_data_elk_application_table_db_property].values[5].offset = UTILEX_OFFSETOF(dnx_data_elk_application_db_property_t, zero_size_ad);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_elk_application_feature_get(
    int unit,
    dnx_data_elk_application_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_application, feature);
}


uint32
dnx_data_elk_application_large_mc_mode1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_application, dnx_data_elk_application_define_large_mc_mode1);
}

uint32
dnx_data_elk_application_large_mc_mode2_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_application, dnx_data_elk_application_define_large_mc_mode2);
}

uint32
dnx_data_elk_application_large_mc_mode3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_application, dnx_data_elk_application_define_large_mc_mode3);
}

uint32
dnx_data_elk_application_optimized_result_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_application, dnx_data_elk_application_define_optimized_result);
}

uint32
dnx_data_elk_application_split_rpf_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_application, dnx_data_elk_application_define_split_rpf);
}

uint32
dnx_data_elk_application_thread_safety_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_application, dnx_data_elk_application_define_thread_safety);
}

uint32
dnx_data_elk_application_ipv4_large_mc_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_application, dnx_data_elk_application_define_ipv4_large_mc_enable);
}

uint32
dnx_data_elk_application_ipv6_large_mc_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_application, dnx_data_elk_application_define_ipv6_large_mc_enable);
}

uint32
dnx_data_elk_application_ipv4_mc_hash_table_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_application, dnx_data_elk_application_define_ipv4_mc_hash_table_size);
}

uint32
dnx_data_elk_application_ipv6_mc_hash_table_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_application, dnx_data_elk_application_define_ipv6_mc_hash_table_size);
}

uint32
dnx_data_elk_application_ipv4_mc_table_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_application, dnx_data_elk_application_define_ipv4_mc_table_size);
}

uint32
dnx_data_elk_application_ipv6_mc_table_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_application, dnx_data_elk_application_define_ipv6_mc_table_size);
}



const dnx_data_elk_application_db_property_t *
dnx_data_elk_application_db_property_get(
    int unit,
    int db)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_application, dnx_data_elk_application_table_db_property);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, db, 0);
    return (const dnx_data_elk_application_db_property_t *) data;

}


shr_error_e
dnx_data_elk_application_db_property_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_elk_application_db_property_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_application, dnx_data_elk_application_table_db_property);
    data = (const dnx_data_elk_application_db_property_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->initial_regular_capacity);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->initial_optimized_capacity);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->use_large_optimized_result);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->xl_db);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->locality);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->zero_size_ad);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_elk_application_db_property_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_application, dnx_data_elk_application_table_db_property);

}






static shr_error_e
dnx_data_elk_connectivity_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "connectivity";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_elk_connectivity_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data elk connectivity features");

    
    submodule_data->nof_defines = _dnx_data_elk_connectivity_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data elk connectivity defines");

    submodule_data->defines[dnx_data_elk_connectivity_define_connect_mode].name = "connect_mode";
    submodule_data->defines[dnx_data_elk_connectivity_define_connect_mode].doc = "";
    
    submodule_data->defines[dnx_data_elk_connectivity_define_connect_mode].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_elk_connectivity_define_blackhole].name = "blackhole";
    submodule_data->defines[dnx_data_elk_connectivity_define_blackhole].doc = "";
    
    submodule_data->defines[dnx_data_elk_connectivity_define_blackhole].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_elk_connectivity_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data elk connectivity tables");

    
    submodule_data->tables[dnx_data_elk_connectivity_table_mdio].name = "mdio";
    submodule_data->tables[dnx_data_elk_connectivity_table_mdio].doc = "";
    submodule_data->tables[dnx_data_elk_connectivity_table_mdio].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_elk_connectivity_table_mdio].size_of_values = sizeof(dnx_data_elk_connectivity_mdio_t);
    submodule_data->tables[dnx_data_elk_connectivity_table_mdio].entry_get = dnx_data_elk_connectivity_mdio_entry_str_get;

    
    submodule_data->tables[dnx_data_elk_connectivity_table_mdio].nof_keys = 1;
    submodule_data->tables[dnx_data_elk_connectivity_table_mdio].keys[0].name = "kbp_inst";
    submodule_data->tables[dnx_data_elk_connectivity_table_mdio].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_elk_connectivity_table_mdio].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_elk_connectivity_table_mdio].values, dnxc_data_value_t, submodule_data->tables[dnx_data_elk_connectivity_table_mdio].nof_values, "_dnx_data_elk_connectivity_table_mdio table values");
    submodule_data->tables[dnx_data_elk_connectivity_table_mdio].values[0].name = "mdio_id";
    submodule_data->tables[dnx_data_elk_connectivity_table_mdio].values[0].type = "uint32";
    submodule_data->tables[dnx_data_elk_connectivity_table_mdio].values[0].doc = "";
    submodule_data->tables[dnx_data_elk_connectivity_table_mdio].values[0].offset = UTILEX_OFFSETOF(dnx_data_elk_connectivity_mdio_t, mdio_id);

    
    submodule_data->tables[dnx_data_elk_connectivity_table_ilkn_reverse].name = "ilkn_reverse";
    submodule_data->tables[dnx_data_elk_connectivity_table_ilkn_reverse].doc = "";
    submodule_data->tables[dnx_data_elk_connectivity_table_ilkn_reverse].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_elk_connectivity_table_ilkn_reverse].size_of_values = sizeof(dnx_data_elk_connectivity_ilkn_reverse_t);
    submodule_data->tables[dnx_data_elk_connectivity_table_ilkn_reverse].entry_get = dnx_data_elk_connectivity_ilkn_reverse_entry_str_get;

    
    submodule_data->tables[dnx_data_elk_connectivity_table_ilkn_reverse].nof_keys = 1;
    submodule_data->tables[dnx_data_elk_connectivity_table_ilkn_reverse].keys[0].name = "ilkn_id";
    submodule_data->tables[dnx_data_elk_connectivity_table_ilkn_reverse].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_elk_connectivity_table_ilkn_reverse].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_elk_connectivity_table_ilkn_reverse].values, dnxc_data_value_t, submodule_data->tables[dnx_data_elk_connectivity_table_ilkn_reverse].nof_values, "_dnx_data_elk_connectivity_table_ilkn_reverse table values");
    submodule_data->tables[dnx_data_elk_connectivity_table_ilkn_reverse].values[0].name = "reverse";
    submodule_data->tables[dnx_data_elk_connectivity_table_ilkn_reverse].values[0].type = "uint32";
    submodule_data->tables[dnx_data_elk_connectivity_table_ilkn_reverse].values[0].doc = "";
    submodule_data->tables[dnx_data_elk_connectivity_table_ilkn_reverse].values[0].offset = UTILEX_OFFSETOF(dnx_data_elk_connectivity_ilkn_reverse_t, reverse);

    
    submodule_data->tables[dnx_data_elk_connectivity_table_topology].name = "topology";
    submodule_data->tables[dnx_data_elk_connectivity_table_topology].doc = "";
    submodule_data->tables[dnx_data_elk_connectivity_table_topology].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_elk_connectivity_table_topology].size_of_values = sizeof(dnx_data_elk_connectivity_topology_t);
    submodule_data->tables[dnx_data_elk_connectivity_table_topology].entry_get = dnx_data_elk_connectivity_topology_entry_str_get;

    
    submodule_data->tables[dnx_data_elk_connectivity_table_topology].nof_keys = 0;

    
    submodule_data->tables[dnx_data_elk_connectivity_table_topology].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_elk_connectivity_table_topology].values, dnxc_data_value_t, submodule_data->tables[dnx_data_elk_connectivity_table_topology].nof_values, "_dnx_data_elk_connectivity_table_topology table values");
    submodule_data->tables[dnx_data_elk_connectivity_table_topology].values[0].name = "start_lane";
    submodule_data->tables[dnx_data_elk_connectivity_table_topology].values[0].type = "uint32[DNX_DATA_MAX_ELK_GENERAL_MAX_PORTS]";
    submodule_data->tables[dnx_data_elk_connectivity_table_topology].values[0].doc = "";
    submodule_data->tables[dnx_data_elk_connectivity_table_topology].values[0].offset = UTILEX_OFFSETOF(dnx_data_elk_connectivity_topology_t, start_lane);
    submodule_data->tables[dnx_data_elk_connectivity_table_topology].values[1].name = "port_core_mapping";
    submodule_data->tables[dnx_data_elk_connectivity_table_topology].values[1].type = "uint32[DNX_DATA_MAX_ELK_GENERAL_MAX_PORTS]";
    submodule_data->tables[dnx_data_elk_connectivity_table_topology].values[1].doc = "";
    submodule_data->tables[dnx_data_elk_connectivity_table_topology].values[1].offset = UTILEX_OFFSETOF(dnx_data_elk_connectivity_topology_t, port_core_mapping);

    
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].name = "kbp_serdes_tx_taps";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].doc = "";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].size_of_values = sizeof(dnx_data_elk_connectivity_kbp_serdes_tx_taps_t);
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].entry_get = dnx_data_elk_connectivity_kbp_serdes_tx_taps_entry_str_get;

    
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].nof_keys = 1;
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].keys[0].name = "kbp_inst";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].nof_values = 7;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].values, dnxc_data_value_t, submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].nof_values, "_dnx_data_elk_connectivity_table_kbp_serdes_tx_taps table values");
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].values[0].name = "pre";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].values[0].type = "int";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].values[0].doc = "";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].values[0].offset = UTILEX_OFFSETOF(dnx_data_elk_connectivity_kbp_serdes_tx_taps_t, pre);
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].values[1].name = "main";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].values[1].type = "int";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].values[1].doc = "";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].values[1].offset = UTILEX_OFFSETOF(dnx_data_elk_connectivity_kbp_serdes_tx_taps_t, main);
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].values[2].name = "post";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].values[2].type = "int";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].values[2].doc = "";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].values[2].offset = UTILEX_OFFSETOF(dnx_data_elk_connectivity_kbp_serdes_tx_taps_t, post);
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].values[3].name = "pre2";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].values[3].type = "int";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].values[3].doc = "";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].values[3].offset = UTILEX_OFFSETOF(dnx_data_elk_connectivity_kbp_serdes_tx_taps_t, pre2);
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].values[4].name = "post2";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].values[4].type = "int";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].values[4].doc = "";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].values[4].offset = UTILEX_OFFSETOF(dnx_data_elk_connectivity_kbp_serdes_tx_taps_t, post2);
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].values[5].name = "post3";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].values[5].type = "int";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].values[5].doc = "";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].values[5].offset = UTILEX_OFFSETOF(dnx_data_elk_connectivity_kbp_serdes_tx_taps_t, post3);
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].values[6].name = "txfir_tap_enable";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].values[6].type = "enum kbp_blackhawk_tsc_txfir_tap_enable_enum";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].values[6].doc = "";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_serdes_tx_taps].values[6].offset = UTILEX_OFFSETOF(dnx_data_elk_connectivity_kbp_serdes_tx_taps_t, txfir_tap_enable);

    
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].name = "kbp_lane_serdes_tx_taps";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].doc = "";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].size_of_values = sizeof(dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_t);
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].entry_get = dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_entry_str_get;

    
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].nof_keys = 1;
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].keys[0].name = "lane_id";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].nof_values = 7;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].values, dnxc_data_value_t, submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].nof_values, "_dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps table values");
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].values[0].name = "pre";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].values[0].type = "int";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].values[0].doc = "";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].values[0].offset = UTILEX_OFFSETOF(dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_t, pre);
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].values[1].name = "main";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].values[1].type = "int";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].values[1].doc = "";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].values[1].offset = UTILEX_OFFSETOF(dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_t, main);
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].values[2].name = "post";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].values[2].type = "int";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].values[2].doc = "";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].values[2].offset = UTILEX_OFFSETOF(dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_t, post);
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].values[3].name = "pre2";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].values[3].type = "int";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].values[3].doc = "";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].values[3].offset = UTILEX_OFFSETOF(dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_t, pre2);
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].values[4].name = "post2";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].values[4].type = "int";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].values[4].doc = "";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].values[4].offset = UTILEX_OFFSETOF(dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_t, post2);
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].values[5].name = "post3";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].values[5].type = "int";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].values[5].doc = "";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].values[5].offset = UTILEX_OFFSETOF(dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_t, post3);
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].values[6].name = "txfir_tap_enable";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].values[6].type = "enum kbp_blackhawk_tsc_txfir_tap_enable_enum";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].values[6].doc = "";
    submodule_data->tables[dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps].values[6].offset = UTILEX_OFFSETOF(dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_t, txfir_tap_enable);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_elk_connectivity_feature_get(
    int unit,
    dnx_data_elk_connectivity_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_connectivity, feature);
}


uint32
dnx_data_elk_connectivity_connect_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_connectivity, dnx_data_elk_connectivity_define_connect_mode);
}

uint32
dnx_data_elk_connectivity_blackhole_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_connectivity, dnx_data_elk_connectivity_define_blackhole);
}



const dnx_data_elk_connectivity_mdio_t *
dnx_data_elk_connectivity_mdio_get(
    int unit,
    int kbp_inst)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_connectivity, dnx_data_elk_connectivity_table_mdio);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, kbp_inst, 0);
    return (const dnx_data_elk_connectivity_mdio_t *) data;

}

const dnx_data_elk_connectivity_ilkn_reverse_t *
dnx_data_elk_connectivity_ilkn_reverse_get(
    int unit,
    int ilkn_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_connectivity, dnx_data_elk_connectivity_table_ilkn_reverse);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, ilkn_id, 0);
    return (const dnx_data_elk_connectivity_ilkn_reverse_t *) data;

}

const dnx_data_elk_connectivity_topology_t *
dnx_data_elk_connectivity_topology_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_connectivity, dnx_data_elk_connectivity_table_topology);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_elk_connectivity_topology_t *) data;

}

const dnx_data_elk_connectivity_kbp_serdes_tx_taps_t *
dnx_data_elk_connectivity_kbp_serdes_tx_taps_get(
    int unit,
    int kbp_inst)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_connectivity, dnx_data_elk_connectivity_table_kbp_serdes_tx_taps);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, kbp_inst, 0);
    return (const dnx_data_elk_connectivity_kbp_serdes_tx_taps_t *) data;

}

const dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_t *
dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_get(
    int unit,
    int lane_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_connectivity, dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, lane_id, 0);
    return (const dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_t *) data;

}


shr_error_e
dnx_data_elk_connectivity_mdio_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_elk_connectivity_mdio_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_connectivity, dnx_data_elk_connectivity_table_mdio);
    data = (const dnx_data_elk_connectivity_mdio_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdio_id);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_elk_connectivity_ilkn_reverse_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_elk_connectivity_ilkn_reverse_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_connectivity, dnx_data_elk_connectivity_table_ilkn_reverse);
    data = (const dnx_data_elk_connectivity_ilkn_reverse_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->reverse);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_elk_connectivity_topology_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_elk_connectivity_topology_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_connectivity, dnx_data_elk_connectivity_table_topology);
    data = (const dnx_data_elk_connectivity_topology_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_ELK_GENERAL_MAX_PORTS, data->start_lane);
            break;
        case 1:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_ELK_GENERAL_MAX_PORTS, data->port_core_mapping);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_elk_connectivity_kbp_serdes_tx_taps_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_elk_connectivity_kbp_serdes_tx_taps_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_connectivity, dnx_data_elk_connectivity_table_kbp_serdes_tx_taps);
    data = (const dnx_data_elk_connectivity_kbp_serdes_tx_taps_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pre);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->main);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->post);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pre2);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->post2);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->post3);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->txfir_tap_enable);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_connectivity, dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps);
    data = (const dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pre);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->main);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->post);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pre2);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->post2);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->post3);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->txfir_tap_enable);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_elk_connectivity_mdio_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_connectivity, dnx_data_elk_connectivity_table_mdio);

}

const dnxc_data_table_info_t *
dnx_data_elk_connectivity_ilkn_reverse_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_connectivity, dnx_data_elk_connectivity_table_ilkn_reverse);

}

const dnxc_data_table_info_t *
dnx_data_elk_connectivity_topology_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_connectivity, dnx_data_elk_connectivity_table_topology);

}

const dnxc_data_table_info_t *
dnx_data_elk_connectivity_kbp_serdes_tx_taps_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_connectivity, dnx_data_elk_connectivity_table_kbp_serdes_tx_taps);

}

const dnxc_data_table_info_t *
dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_connectivity, dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps);

}






static shr_error_e
dnx_data_elk_recovery_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "recovery";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_elk_recovery_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data elk recovery features");

    
    submodule_data->nof_defines = _dnx_data_elk_recovery_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data elk recovery defines");

    submodule_data->defines[dnx_data_elk_recovery_define_enable].name = "enable";
    submodule_data->defines[dnx_data_elk_recovery_define_enable].doc = "";
    
    submodule_data->defines[dnx_data_elk_recovery_define_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_elk_recovery_define_iteration].name = "iteration";
    submodule_data->defines[dnx_data_elk_recovery_define_iteration].doc = "";
    
    submodule_data->defines[dnx_data_elk_recovery_define_iteration].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_elk_recovery_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data elk recovery tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_elk_recovery_feature_get(
    int unit,
    dnx_data_elk_recovery_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_recovery, feature);
}


uint32
dnx_data_elk_recovery_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_recovery, dnx_data_elk_recovery_define_enable);
}

uint32
dnx_data_elk_recovery_iteration_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_elk, dnx_data_elk_submodule_recovery, dnx_data_elk_recovery_define_iteration);
}







shr_error_e
dnx_data_elk_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "elk";
    module_data->nof_submodules = _dnx_data_elk_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data elk submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_elk_general_init(unit, &module_data->submodules[dnx_data_elk_submodule_general]));
    SHR_IF_ERR_EXIT(dnx_data_elk_application_init(unit, &module_data->submodules[dnx_data_elk_submodule_application]));
    SHR_IF_ERR_EXIT(dnx_data_elk_connectivity_init(unit, &module_data->submodules[dnx_data_elk_submodule_connectivity]));
    SHR_IF_ERR_EXIT(dnx_data_elk_recovery_init(unit, &module_data->submodules[dnx_data_elk_submodule_recovery]));
    

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_elk_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_elk_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_elk_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_elk_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_elk_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_elk_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_elk_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_elk_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_elk_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_elk_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_elk_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_elk_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_elk_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_elk_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_elk_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_elk_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_elk_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_elk_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_elk_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_elk_attach(unit));
        SHR_IF_ERR_EXIT(j2x_a0_data_elk_attach(unit));
    }
    else

    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

