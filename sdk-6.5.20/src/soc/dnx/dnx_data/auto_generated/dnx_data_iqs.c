

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_iqs.h>



extern shr_error_e jr2_a0_data_iqs_attach(
    int unit);
extern shr_error_e j2c_a0_data_iqs_attach(
    int unit);
extern shr_error_e q2a_a0_data_iqs_attach(
    int unit);
extern shr_error_e j2p_a0_data_iqs_attach(
    int unit);



static shr_error_e
dnx_data_iqs_credit_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "credit";
    submodule_data->doc = "Data about credit profiles";
    
    submodule_data->nof_features = _dnx_data_iqs_credit_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data iqs credit features");

    submodule_data->features[dnx_data_iqs_credit_watchdog_fixed_period].name = "watchdog_fixed_period";
    submodule_data->features[dnx_data_iqs_credit_watchdog_fixed_period].doc = "";
    submodule_data->features[dnx_data_iqs_credit_watchdog_fixed_period].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_iqs_credit_fmq_shp_zero_rate_allowed].name = "fmq_shp_zero_rate_allowed";
    submodule_data->features[dnx_data_iqs_credit_fmq_shp_zero_rate_allowed].doc = "Is zero rate allowed on fmq shaper";
    submodule_data->features[dnx_data_iqs_credit_fmq_shp_zero_rate_allowed].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_iqs_credit_dram_delete_fix_hw].name = "dram_delete_fix_hw";
    submodule_data->features[dnx_data_iqs_credit_dram_delete_fix_hw].doc = "";
    submodule_data->features[dnx_data_iqs_credit_dram_delete_fix_hw].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_iqs_credit_credit_balance_support].name = "credit_balance_support";
    submodule_data->features[dnx_data_iqs_credit_credit_balance_support].doc = "indicate if credit balance is supported";
    submodule_data->features[dnx_data_iqs_credit_credit_balance_support].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_iqs_credit_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data iqs credit defines");

    submodule_data->defines[dnx_data_iqs_credit_define_nof_profiles].name = "nof_profiles";
    submodule_data->defines[dnx_data_iqs_credit_define_nof_profiles].doc = "Number of supported profiles";
    
    submodule_data->defines[dnx_data_iqs_credit_define_nof_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_iqs_credit_define_max_credit_balance_threshold].name = "max_credit_balance_threshold";
    submodule_data->defines[dnx_data_iqs_credit_define_max_credit_balance_threshold].doc = "max value for thresholds in credit balance resolution units";
    
    submodule_data->defines[dnx_data_iqs_credit_define_max_credit_balance_threshold].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_iqs_credit_define_max_worth].name = "max_worth";
    submodule_data->defines[dnx_data_iqs_credit_define_max_worth].doc = "max credit worth";
    
    submodule_data->defines[dnx_data_iqs_credit_define_max_worth].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_iqs_credit_define_satisified_back_min].name = "satisified_back_min";
    submodule_data->defines[dnx_data_iqs_credit_define_satisified_back_min].doc = "min value for satisified back thresholds";
    
    submodule_data->defines[dnx_data_iqs_credit_define_satisified_back_min].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_iqs_credit_define_satisified_back_max].name = "satisified_back_max";
    submodule_data->defines[dnx_data_iqs_credit_define_satisified_back_max].doc = "max value for satisified back thresholds";
    
    submodule_data->defines[dnx_data_iqs_credit_define_satisified_back_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_iqs_credit_define_satisified_empty_min].name = "satisified_empty_min";
    submodule_data->defines[dnx_data_iqs_credit_define_satisified_empty_min].doc = "min value for satisified empty thresholds";
    
    submodule_data->defines[dnx_data_iqs_credit_define_satisified_empty_min].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_iqs_credit_define_satisified_empty_max].name = "satisified_empty_max";
    submodule_data->defines[dnx_data_iqs_credit_define_satisified_empty_max].doc = "max value for satisified empty thresholds";
    
    submodule_data->defines[dnx_data_iqs_credit_define_satisified_empty_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_iqs_credit_define_hungry_min].name = "hungry_min";
    submodule_data->defines[dnx_data_iqs_credit_define_hungry_min].doc = "min value for hungry thresholds";
    
    submodule_data->defines[dnx_data_iqs_credit_define_hungry_min].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_iqs_credit_define_hungry_max].name = "hungry_max";
    submodule_data->defines[dnx_data_iqs_credit_define_hungry_max].doc = "max value for hungry thresholds";
    
    submodule_data->defines[dnx_data_iqs_credit_define_hungry_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_iqs_credit_define_hungry_mult_min].name = "hungry_mult_min";
    submodule_data->defines[dnx_data_iqs_credit_define_hungry_mult_min].doc = "min value for hungry multiply thresholds";
    
    submodule_data->defines[dnx_data_iqs_credit_define_hungry_mult_min].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_iqs_credit_define_hungry_mult_max].name = "hungry_mult_max";
    submodule_data->defines[dnx_data_iqs_credit_define_hungry_mult_max].doc = "max value for hungry multiply thresholds";
    
    submodule_data->defines[dnx_data_iqs_credit_define_hungry_mult_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_iqs_credit_define_nof_bw_levels].name = "nof_bw_levels";
    submodule_data->defines[dnx_data_iqs_credit_define_nof_bw_levels].doc = "number of supported bandwidth levels";
    
    submodule_data->defines[dnx_data_iqs_credit_define_nof_bw_levels].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_iqs_credit_define_fmq_credit_fc_on_th].name = "fmq_credit_fc_on_th";
    submodule_data->defines[dnx_data_iqs_credit_define_fmq_credit_fc_on_th].doc = "default thershold value for FMQ credit FC on";
    
    submodule_data->defines[dnx_data_iqs_credit_define_fmq_credit_fc_on_th].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_iqs_credit_define_fmq_credit_fc_off_th].name = "fmq_credit_fc_off_th";
    submodule_data->defines[dnx_data_iqs_credit_define_fmq_credit_fc_off_th].doc = "default thershold value for FMQ credit FC off";
    
    submodule_data->defines[dnx_data_iqs_credit_define_fmq_credit_fc_off_th].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_iqs_credit_define_fmq_byte_fc_on_th].name = "fmq_byte_fc_on_th";
    submodule_data->defines[dnx_data_iqs_credit_define_fmq_byte_fc_on_th].doc = "default thershold value for FMQ byte FC on";
    
    submodule_data->defines[dnx_data_iqs_credit_define_fmq_byte_fc_on_th].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_iqs_credit_define_fmq_byte_fc_off_th].name = "fmq_byte_fc_off_th";
    submodule_data->defines[dnx_data_iqs_credit_define_fmq_byte_fc_off_th].doc = "default thershold value for FMQ byte FC off";
    
    submodule_data->defines[dnx_data_iqs_credit_define_fmq_byte_fc_off_th].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_iqs_credit_define_fmq_eir_credit_fc_th].name = "fmq_eir_credit_fc_th";
    submodule_data->defines[dnx_data_iqs_credit_define_fmq_eir_credit_fc_th].doc = "default thershold value for all credit EIR FMQ FC thresholds";
    
    submodule_data->defines[dnx_data_iqs_credit_define_fmq_eir_credit_fc_th].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_iqs_credit_define_fmq_eir_byte_fc_th].name = "fmq_eir_byte_fc_th";
    submodule_data->defines[dnx_data_iqs_credit_define_fmq_eir_byte_fc_th].doc = "default thershold value for all byte EIR FMQ FC thresholds";
    
    submodule_data->defines[dnx_data_iqs_credit_define_fmq_eir_byte_fc_th].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_iqs_credit_define_fmq_nof_be_classes].name = "fmq_nof_be_classes";
    submodule_data->defines[dnx_data_iqs_credit_define_fmq_nof_be_classes].doc = "nof best-effort FMQ classes";
    
    submodule_data->defines[dnx_data_iqs_credit_define_fmq_nof_be_classes].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_iqs_credit_define_fmq_max_be_weight].name = "fmq_max_be_weight";
    submodule_data->defines[dnx_data_iqs_credit_define_fmq_max_be_weight].doc = "max weight for best-effort FMQ classes";
    
    submodule_data->defines[dnx_data_iqs_credit_define_fmq_max_be_weight].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_iqs_credit_define_fmq_max_burst_max].name = "fmq_max_burst_max";
    submodule_data->defines[dnx_data_iqs_credit_define_fmq_max_burst_max].doc = "FMQ max burst maximum value";
    
    submodule_data->defines[dnx_data_iqs_credit_define_fmq_max_burst_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_iqs_credit_define_fmq_shp_crdt_rate_mltp].name = "fmq_shp_crdt_rate_mltp";
    submodule_data->defines[dnx_data_iqs_credit_define_fmq_shp_crdt_rate_mltp].doc = "multiplier for clock resolution of the FMQ shapers credit rate";
    
    submodule_data->defines[dnx_data_iqs_credit_define_fmq_shp_crdt_rate_mltp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_iqs_credit_define_fmq_shp_crdt_rate_delta].name = "fmq_shp_crdt_rate_delta";
    submodule_data->defines[dnx_data_iqs_credit_define_fmq_shp_crdt_rate_delta].doc = "numeric correction used in FMQ shapers rate calculation";
    
    submodule_data->defines[dnx_data_iqs_credit_define_fmq_shp_crdt_rate_delta].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_iqs_credit_define_max_rate_profile_preset_gbps].name = "max_rate_profile_preset_gbps";
    submodule_data->defines[dnx_data_iqs_credit_define_max_rate_profile_preset_gbps].doc = "maximal rate for credit request profile preset get (Gpbs)";
    
    submodule_data->defines[dnx_data_iqs_credit_define_max_rate_profile_preset_gbps].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_iqs_credit_define_worth].name = "worth";
    submodule_data->defines[dnx_data_iqs_credit_define_worth].doc = "credit worth in bytes";
    
    submodule_data->defines[dnx_data_iqs_credit_define_worth].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_iqs_credit_define_fmq_shp_rate_max].name = "fmq_shp_rate_max";
    submodule_data->defines[dnx_data_iqs_credit_define_fmq_shp_rate_max].doc = "max value for FMQ shaper rate in Kbps";
    
    submodule_data->defines[dnx_data_iqs_credit_define_fmq_shp_rate_max].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_iqs_credit_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data iqs credit tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_iqs_credit_feature_get(
    int unit,
    dnx_data_iqs_credit_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_credit, feature);
}


uint32
dnx_data_iqs_credit_nof_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_credit, dnx_data_iqs_credit_define_nof_profiles);
}

uint32
dnx_data_iqs_credit_max_credit_balance_threshold_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_credit, dnx_data_iqs_credit_define_max_credit_balance_threshold);
}

uint32
dnx_data_iqs_credit_max_worth_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_credit, dnx_data_iqs_credit_define_max_worth);
}

int
dnx_data_iqs_credit_satisified_back_min_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_credit, dnx_data_iqs_credit_define_satisified_back_min);
}

int
dnx_data_iqs_credit_satisified_back_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_credit, dnx_data_iqs_credit_define_satisified_back_max);
}

int
dnx_data_iqs_credit_satisified_empty_min_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_credit, dnx_data_iqs_credit_define_satisified_empty_min);
}

int
dnx_data_iqs_credit_satisified_empty_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_credit, dnx_data_iqs_credit_define_satisified_empty_max);
}

int
dnx_data_iqs_credit_hungry_min_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_credit, dnx_data_iqs_credit_define_hungry_min);
}

int
dnx_data_iqs_credit_hungry_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_credit, dnx_data_iqs_credit_define_hungry_max);
}

int
dnx_data_iqs_credit_hungry_mult_min_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_credit, dnx_data_iqs_credit_define_hungry_mult_min);
}

int
dnx_data_iqs_credit_hungry_mult_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_credit, dnx_data_iqs_credit_define_hungry_mult_max);
}

uint32
dnx_data_iqs_credit_nof_bw_levels_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_credit, dnx_data_iqs_credit_define_nof_bw_levels);
}

uint32
dnx_data_iqs_credit_fmq_credit_fc_on_th_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_credit, dnx_data_iqs_credit_define_fmq_credit_fc_on_th);
}

uint32
dnx_data_iqs_credit_fmq_credit_fc_off_th_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_credit, dnx_data_iqs_credit_define_fmq_credit_fc_off_th);
}

uint32
dnx_data_iqs_credit_fmq_byte_fc_on_th_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_credit, dnx_data_iqs_credit_define_fmq_byte_fc_on_th);
}

uint32
dnx_data_iqs_credit_fmq_byte_fc_off_th_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_credit, dnx_data_iqs_credit_define_fmq_byte_fc_off_th);
}

uint32
dnx_data_iqs_credit_fmq_eir_credit_fc_th_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_credit, dnx_data_iqs_credit_define_fmq_eir_credit_fc_th);
}

uint32
dnx_data_iqs_credit_fmq_eir_byte_fc_th_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_credit, dnx_data_iqs_credit_define_fmq_eir_byte_fc_th);
}

uint32
dnx_data_iqs_credit_fmq_nof_be_classes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_credit, dnx_data_iqs_credit_define_fmq_nof_be_classes);
}

uint32
dnx_data_iqs_credit_fmq_max_be_weight_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_credit, dnx_data_iqs_credit_define_fmq_max_be_weight);
}

uint32
dnx_data_iqs_credit_fmq_max_burst_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_credit, dnx_data_iqs_credit_define_fmq_max_burst_max);
}

uint32
dnx_data_iqs_credit_fmq_shp_crdt_rate_mltp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_credit, dnx_data_iqs_credit_define_fmq_shp_crdt_rate_mltp);
}

uint32
dnx_data_iqs_credit_fmq_shp_crdt_rate_delta_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_credit, dnx_data_iqs_credit_define_fmq_shp_crdt_rate_delta);
}

uint32
dnx_data_iqs_credit_max_rate_profile_preset_gbps_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_credit, dnx_data_iqs_credit_define_max_rate_profile_preset_gbps);
}

uint32
dnx_data_iqs_credit_worth_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_credit, dnx_data_iqs_credit_define_worth);
}

uint32
dnx_data_iqs_credit_fmq_shp_rate_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_credit, dnx_data_iqs_credit_define_fmq_shp_rate_max);
}










static shr_error_e
dnx_data_iqs_deq_default_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "deq_default";
    submodule_data->doc = "IQS dequeue default configuration on init";
    
    submodule_data->nof_features = _dnx_data_iqs_deq_default_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data iqs deq_default features");

    submodule_data->features[dnx_data_iqs_deq_default_dqcq_fc_to_dram].name = "dqcq_fc_to_dram";
    submodule_data->features[dnx_data_iqs_deq_default_dqcq_fc_to_dram].doc = "enable/disable dqcq flow control from IPS to DQM";
    submodule_data->features[dnx_data_iqs_deq_default_dqcq_fc_to_dram].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_iqs_deq_default_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data iqs deq_default defines");

    submodule_data->defines[dnx_data_iqs_deq_default_define_low_delay_deq_bytes].name = "low_delay_deq_bytes";
    submodule_data->defines[dnx_data_iqs_deq_default_define_low_delay_deq_bytes].doc = "dequeue low delay thresholds";
    
    submodule_data->defines[dnx_data_iqs_deq_default_define_low_delay_deq_bytes].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_iqs_deq_default_define_credit_balance_max].name = "credit_balance_max";
    submodule_data->defines[dnx_data_iqs_deq_default_define_credit_balance_max].doc = "max credit balance";
    
    submodule_data->defines[dnx_data_iqs_deq_default_define_credit_balance_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_iqs_deq_default_define_credit_balance_resolution].name = "credit_balance_resolution";
    submodule_data->defines[dnx_data_iqs_deq_default_define_credit_balance_resolution].doc = "credit balance resolution for dequeue parameters configuration";
    
    submodule_data->defines[dnx_data_iqs_deq_default_define_credit_balance_resolution].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_iqs_deq_default_define_s2d_credit_balance_max].name = "s2d_credit_balance_max";
    submodule_data->defines[dnx_data_iqs_deq_default_define_s2d_credit_balance_max].doc = "max credit balance in sram to dram context";
    
    submodule_data->defines[dnx_data_iqs_deq_default_define_s2d_credit_balance_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_iqs_deq_default_define_s2d_credit_balance_resolution].name = "s2d_credit_balance_resolution";
    submodule_data->defines[dnx_data_iqs_deq_default_define_s2d_credit_balance_resolution].doc = "credit balance resolution for dequeue parameters configuration in sram to dram context";
    
    submodule_data->defines[dnx_data_iqs_deq_default_define_s2d_credit_balance_resolution].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_iqs_deq_default_define_sram_to_fabric_credit_lfsr_thr].name = "sram_to_fabric_credit_lfsr_thr";
    submodule_data->defines[dnx_data_iqs_deq_default_define_sram_to_fabric_credit_lfsr_thr].doc = "default lfsr threshold on credits for deq from sram to fabric";
    
    submodule_data->defines[dnx_data_iqs_deq_default_define_sram_to_fabric_credit_lfsr_thr].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_iqs_deq_default_define_sram_to_fabric_credit_lfsr_mask].name = "sram_to_fabric_credit_lfsr_mask";
    submodule_data->defines[dnx_data_iqs_deq_default_define_sram_to_fabric_credit_lfsr_mask].doc = "default lfsr mask on credits for deq from sram to fabric";
    
    submodule_data->defines[dnx_data_iqs_deq_default_define_sram_to_fabric_credit_lfsr_mask].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_iqs_deq_default_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data iqs deq_default tables");

    
    submodule_data->tables[dnx_data_iqs_deq_default_table_sqm_read_weight_profiles].name = "sqm_read_weight_profiles";
    submodule_data->tables[dnx_data_iqs_deq_default_table_sqm_read_weight_profiles].doc = "stores the SQM read weight profiles that should configured to HW";
    submodule_data->tables[dnx_data_iqs_deq_default_table_sqm_read_weight_profiles].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_iqs_deq_default_table_sqm_read_weight_profiles].size_of_values = sizeof(dnx_data_iqs_deq_default_sqm_read_weight_profiles_t);
    submodule_data->tables[dnx_data_iqs_deq_default_table_sqm_read_weight_profiles].entry_get = dnx_data_iqs_deq_default_sqm_read_weight_profiles_entry_str_get;

    
    submodule_data->tables[dnx_data_iqs_deq_default_table_sqm_read_weight_profiles].nof_keys = 1;
    submodule_data->tables[dnx_data_iqs_deq_default_table_sqm_read_weight_profiles].keys[0].name = "profile_id";
    submodule_data->tables[dnx_data_iqs_deq_default_table_sqm_read_weight_profiles].keys[0].doc = "read weight profile id";

    
    submodule_data->tables[dnx_data_iqs_deq_default_table_sqm_read_weight_profiles].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_iqs_deq_default_table_sqm_read_weight_profiles].values, dnxc_data_value_t, submodule_data->tables[dnx_data_iqs_deq_default_table_sqm_read_weight_profiles].nof_values, "_dnx_data_iqs_deq_default_table_sqm_read_weight_profiles table values");
    submodule_data->tables[dnx_data_iqs_deq_default_table_sqm_read_weight_profiles].values[0].name = "weight";
    submodule_data->tables[dnx_data_iqs_deq_default_table_sqm_read_weight_profiles].values[0].type = "uint32";
    submodule_data->tables[dnx_data_iqs_deq_default_table_sqm_read_weight_profiles].values[0].doc = "queue read weight (1K resolution)";
    submodule_data->tables[dnx_data_iqs_deq_default_table_sqm_read_weight_profiles].values[0].offset = UTILEX_OFFSETOF(dnx_data_iqs_deq_default_sqm_read_weight_profiles_t, weight);

    
    submodule_data->tables[dnx_data_iqs_deq_default_table_dqm_read_weight_profiles].name = "dqm_read_weight_profiles";
    submodule_data->tables[dnx_data_iqs_deq_default_table_dqm_read_weight_profiles].doc = "stores the DQM read weight profiles that should configured to HW";
    submodule_data->tables[dnx_data_iqs_deq_default_table_dqm_read_weight_profiles].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_iqs_deq_default_table_dqm_read_weight_profiles].size_of_values = sizeof(dnx_data_iqs_deq_default_dqm_read_weight_profiles_t);
    submodule_data->tables[dnx_data_iqs_deq_default_table_dqm_read_weight_profiles].entry_get = dnx_data_iqs_deq_default_dqm_read_weight_profiles_entry_str_get;

    
    submodule_data->tables[dnx_data_iqs_deq_default_table_dqm_read_weight_profiles].nof_keys = 1;
    submodule_data->tables[dnx_data_iqs_deq_default_table_dqm_read_weight_profiles].keys[0].name = "profile_id";
    submodule_data->tables[dnx_data_iqs_deq_default_table_dqm_read_weight_profiles].keys[0].doc = "read weight profile id";

    
    submodule_data->tables[dnx_data_iqs_deq_default_table_dqm_read_weight_profiles].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_iqs_deq_default_table_dqm_read_weight_profiles].values, dnxc_data_value_t, submodule_data->tables[dnx_data_iqs_deq_default_table_dqm_read_weight_profiles].nof_values, "_dnx_data_iqs_deq_default_table_dqm_read_weight_profiles table values");
    submodule_data->tables[dnx_data_iqs_deq_default_table_dqm_read_weight_profiles].values[0].name = "weight";
    submodule_data->tables[dnx_data_iqs_deq_default_table_dqm_read_weight_profiles].values[0].type = "uint32";
    submodule_data->tables[dnx_data_iqs_deq_default_table_dqm_read_weight_profiles].values[0].doc = "queue read weight (1K resolution)";
    submodule_data->tables[dnx_data_iqs_deq_default_table_dqm_read_weight_profiles].values[0].offset = UTILEX_OFFSETOF(dnx_data_iqs_deq_default_dqm_read_weight_profiles_t, weight);

    
    submodule_data->tables[dnx_data_iqs_deq_default_table_params].name = "params";
    submodule_data->tables[dnx_data_iqs_deq_default_table_params].doc = "stores the read weight profiles that should configured to HW for SRAM and S2D";
    submodule_data->tables[dnx_data_iqs_deq_default_table_params].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_iqs_deq_default_table_params].size_of_values = sizeof(dnx_data_iqs_deq_default_params_t);
    submodule_data->tables[dnx_data_iqs_deq_default_table_params].entry_get = dnx_data_iqs_deq_default_params_entry_str_get;

    
    submodule_data->tables[dnx_data_iqs_deq_default_table_params].nof_keys = 1;
    submodule_data->tables[dnx_data_iqs_deq_default_table_params].keys[0].name = "bw_level";
    submodule_data->tables[dnx_data_iqs_deq_default_table_params].keys[0].doc = "parameters per bandwidth level.";

    
    submodule_data->tables[dnx_data_iqs_deq_default_table_params].nof_values = 6;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_iqs_deq_default_table_params].values, dnxc_data_value_t, submodule_data->tables[dnx_data_iqs_deq_default_table_params].nof_values, "_dnx_data_iqs_deq_default_table_params table values");
    submodule_data->tables[dnx_data_iqs_deq_default_table_params].values[0].name = "sram_read_weight_profile_val";
    submodule_data->tables[dnx_data_iqs_deq_default_table_params].values[0].type = "uint32[DNX_IQS_DEQ_PARAM_TYPE_NOF]";
    submodule_data->tables[dnx_data_iqs_deq_default_table_params].values[0].doc = "queue read weight profile for sram values";
    submodule_data->tables[dnx_data_iqs_deq_default_table_params].values[0].offset = UTILEX_OFFSETOF(dnx_data_iqs_deq_default_params_t, sram_read_weight_profile_val);
    submodule_data->tables[dnx_data_iqs_deq_default_table_params].values[1].name = "s2d_read_weight_profile_val";
    submodule_data->tables[dnx_data_iqs_deq_default_table_params].values[1].type = "uint32[DNX_IQS_DEQ_PARAM_TYPE_NOF]";
    submodule_data->tables[dnx_data_iqs_deq_default_table_params].values[1].doc = "queue read weight profile for sram to dram values";
    submodule_data->tables[dnx_data_iqs_deq_default_table_params].values[1].offset = UTILEX_OFFSETOF(dnx_data_iqs_deq_default_params_t, s2d_read_weight_profile_val);
    submodule_data->tables[dnx_data_iqs_deq_default_table_params].values[2].name = "sram_extra_credits_val";
    submodule_data->tables[dnx_data_iqs_deq_default_table_params].values[2].type = "uint32[DNX_IQS_DEQ_PARAM_TYPE_NOF]";
    submodule_data->tables[dnx_data_iqs_deq_default_table_params].values[2].doc = "sram extra credits values";
    submodule_data->tables[dnx_data_iqs_deq_default_table_params].values[2].offset = UTILEX_OFFSETOF(dnx_data_iqs_deq_default_params_t, sram_extra_credits_val);
    submodule_data->tables[dnx_data_iqs_deq_default_table_params].values[3].name = "s2d_extra_credits_val";
    submodule_data->tables[dnx_data_iqs_deq_default_table_params].values[3].type = "uint32[DNX_IQS_DEQ_PARAM_TYPE_NOF]";
    submodule_data->tables[dnx_data_iqs_deq_default_table_params].values[3].doc = "sram to dram extra credits values";
    submodule_data->tables[dnx_data_iqs_deq_default_table_params].values[3].offset = UTILEX_OFFSETOF(dnx_data_iqs_deq_default_params_t, s2d_extra_credits_val);
    submodule_data->tables[dnx_data_iqs_deq_default_table_params].values[4].name = "sram_extra_credits_lfsr_val";
    submodule_data->tables[dnx_data_iqs_deq_default_table_params].values[4].type = "uint32[DNX_IQS_DEQ_PARAM_TYPE_NOF]";
    submodule_data->tables[dnx_data_iqs_deq_default_table_params].values[4].doc = "sram extra credits lfsr values";
    submodule_data->tables[dnx_data_iqs_deq_default_table_params].values[4].offset = UTILEX_OFFSETOF(dnx_data_iqs_deq_default_params_t, sram_extra_credits_lfsr_val);
    submodule_data->tables[dnx_data_iqs_deq_default_table_params].values[5].name = "s2d_extra_credits_lfsr_val";
    submodule_data->tables[dnx_data_iqs_deq_default_table_params].values[5].type = "uint32[DNX_IQS_DEQ_PARAM_TYPE_NOF]";
    submodule_data->tables[dnx_data_iqs_deq_default_table_params].values[5].doc = "sram to dram extra credits lfsr values";
    submodule_data->tables[dnx_data_iqs_deq_default_table_params].values[5].offset = UTILEX_OFFSETOF(dnx_data_iqs_deq_default_params_t, s2d_extra_credits_lfsr_val);

    
    submodule_data->tables[dnx_data_iqs_deq_default_table_dram_params].name = "dram_params";
    submodule_data->tables[dnx_data_iqs_deq_default_table_dram_params].doc = "stores the read weight profiles that should configured to HW for DRAM";
    submodule_data->tables[dnx_data_iqs_deq_default_table_dram_params].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_iqs_deq_default_table_dram_params].size_of_values = sizeof(dnx_data_iqs_deq_default_dram_params_t);
    submodule_data->tables[dnx_data_iqs_deq_default_table_dram_params].entry_get = dnx_data_iqs_deq_default_dram_params_entry_str_get;

    
    submodule_data->tables[dnx_data_iqs_deq_default_table_dram_params].nof_keys = 1;
    submodule_data->tables[dnx_data_iqs_deq_default_table_dram_params].keys[0].name = "index";
    submodule_data->tables[dnx_data_iqs_deq_default_table_dram_params].keys[0].doc = "index to the table";

    
    submodule_data->tables[dnx_data_iqs_deq_default_table_dram_params].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_iqs_deq_default_table_dram_params].values, dnxc_data_value_t, submodule_data->tables[dnx_data_iqs_deq_default_table_dram_params].nof_values, "_dnx_data_iqs_deq_default_table_dram_params table values");
    submodule_data->tables[dnx_data_iqs_deq_default_table_dram_params].values[0].name = "read_weight_profile_val";
    submodule_data->tables[dnx_data_iqs_deq_default_table_dram_params].values[0].type = "uint32";
    submodule_data->tables[dnx_data_iqs_deq_default_table_dram_params].values[0].doc = "queue read weight profile for dram values";
    submodule_data->tables[dnx_data_iqs_deq_default_table_dram_params].values[0].offset = UTILEX_OFFSETOF(dnx_data_iqs_deq_default_dram_params_t, read_weight_profile_val);
    submodule_data->tables[dnx_data_iqs_deq_default_table_dram_params].values[1].name = "read_weight_profile_th";
    submodule_data->tables[dnx_data_iqs_deq_default_table_dram_params].values[1].type = "uint32";
    submodule_data->tables[dnx_data_iqs_deq_default_table_dram_params].values[1].doc = "queue read weight threshold for dram values";
    submodule_data->tables[dnx_data_iqs_deq_default_table_dram_params].values[1].offset = UTILEX_OFFSETOF(dnx_data_iqs_deq_default_dram_params_t, read_weight_profile_th);

    
    submodule_data->tables[dnx_data_iqs_deq_default_table_max_deq_cmd].name = "max_deq_cmd";
    submodule_data->tables[dnx_data_iqs_deq_default_table_max_deq_cmd].doc = "stores the read weight profiles that should configured to HW (sram/dram to fabric)";
    submodule_data->tables[dnx_data_iqs_deq_default_table_max_deq_cmd].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_iqs_deq_default_table_max_deq_cmd].size_of_values = sizeof(dnx_data_iqs_deq_default_max_deq_cmd_t);
    submodule_data->tables[dnx_data_iqs_deq_default_table_max_deq_cmd].entry_get = dnx_data_iqs_deq_default_max_deq_cmd_entry_str_get;

    
    submodule_data->tables[dnx_data_iqs_deq_default_table_max_deq_cmd].nof_keys = 2;
    submodule_data->tables[dnx_data_iqs_deq_default_table_max_deq_cmd].keys[0].name = "bw_level";
    submodule_data->tables[dnx_data_iqs_deq_default_table_max_deq_cmd].keys[0].doc = "parameters per bandwidth level.";
    submodule_data->tables[dnx_data_iqs_deq_default_table_max_deq_cmd].keys[1].name = "nof_active_queues";
    submodule_data->tables[dnx_data_iqs_deq_default_table_max_deq_cmd].keys[1].doc = "see dbal_enum_value_field_iqs_active_queues_e";

    
    submodule_data->tables[dnx_data_iqs_deq_default_table_max_deq_cmd].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_iqs_deq_default_table_max_deq_cmd].values, dnxc_data_value_t, submodule_data->tables[dnx_data_iqs_deq_default_table_max_deq_cmd].nof_values, "_dnx_data_iqs_deq_default_table_max_deq_cmd table values");
    submodule_data->tables[dnx_data_iqs_deq_default_table_max_deq_cmd].values[0].name = "value";
    submodule_data->tables[dnx_data_iqs_deq_default_table_max_deq_cmd].values[0].type = "uint32";
    submodule_data->tables[dnx_data_iqs_deq_default_table_max_deq_cmd].values[0].doc = "threshold per status of active queues. 64B resolution. A value of 0 means a single packet is to be dequeued at maximum.";
    submodule_data->tables[dnx_data_iqs_deq_default_table_max_deq_cmd].values[0].offset = UTILEX_OFFSETOF(dnx_data_iqs_deq_default_max_deq_cmd_t, value);

    
    submodule_data->tables[dnx_data_iqs_deq_default_table_s2d_max_deq_cmd].name = "s2d_max_deq_cmd";
    submodule_data->tables[dnx_data_iqs_deq_default_table_s2d_max_deq_cmd].doc = "stores the read weight profiles that should configured to HW (sram t0 dram)";
    submodule_data->tables[dnx_data_iqs_deq_default_table_s2d_max_deq_cmd].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_iqs_deq_default_table_s2d_max_deq_cmd].size_of_values = sizeof(dnx_data_iqs_deq_default_s2d_max_deq_cmd_t);
    submodule_data->tables[dnx_data_iqs_deq_default_table_s2d_max_deq_cmd].entry_get = dnx_data_iqs_deq_default_s2d_max_deq_cmd_entry_str_get;

    
    submodule_data->tables[dnx_data_iqs_deq_default_table_s2d_max_deq_cmd].nof_keys = 2;
    submodule_data->tables[dnx_data_iqs_deq_default_table_s2d_max_deq_cmd].keys[0].name = "bw_level";
    submodule_data->tables[dnx_data_iqs_deq_default_table_s2d_max_deq_cmd].keys[0].doc = "parameters per bandwidth level.";
    submodule_data->tables[dnx_data_iqs_deq_default_table_s2d_max_deq_cmd].keys[1].name = "nof_active_queues";
    submodule_data->tables[dnx_data_iqs_deq_default_table_s2d_max_deq_cmd].keys[1].doc = "see dbal_enum_value_field_iqs_active_queues_e";

    
    submodule_data->tables[dnx_data_iqs_deq_default_table_s2d_max_deq_cmd].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_iqs_deq_default_table_s2d_max_deq_cmd].values, dnxc_data_value_t, submodule_data->tables[dnx_data_iqs_deq_default_table_s2d_max_deq_cmd].nof_values, "_dnx_data_iqs_deq_default_table_s2d_max_deq_cmd table values");
    submodule_data->tables[dnx_data_iqs_deq_default_table_s2d_max_deq_cmd].values[0].name = "value";
    submodule_data->tables[dnx_data_iqs_deq_default_table_s2d_max_deq_cmd].values[0].type = "uint32";
    submodule_data->tables[dnx_data_iqs_deq_default_table_s2d_max_deq_cmd].values[0].doc = "threshold per status of active queues. 64B resolution. A value of 0 means a single packet is to be dequeued at maximum.";
    submodule_data->tables[dnx_data_iqs_deq_default_table_s2d_max_deq_cmd].values[0].offset = UTILEX_OFFSETOF(dnx_data_iqs_deq_default_s2d_max_deq_cmd_t, value);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_iqs_deq_default_feature_get(
    int unit,
    dnx_data_iqs_deq_default_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_deq_default, feature);
}


uint32
dnx_data_iqs_deq_default_low_delay_deq_bytes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_deq_default, dnx_data_iqs_deq_default_define_low_delay_deq_bytes);
}

uint32
dnx_data_iqs_deq_default_credit_balance_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_deq_default, dnx_data_iqs_deq_default_define_credit_balance_max);
}

uint32
dnx_data_iqs_deq_default_credit_balance_resolution_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_deq_default, dnx_data_iqs_deq_default_define_credit_balance_resolution);
}

uint32
dnx_data_iqs_deq_default_s2d_credit_balance_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_deq_default, dnx_data_iqs_deq_default_define_s2d_credit_balance_max);
}

uint32
dnx_data_iqs_deq_default_s2d_credit_balance_resolution_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_deq_default, dnx_data_iqs_deq_default_define_s2d_credit_balance_resolution);
}

uint32
dnx_data_iqs_deq_default_sram_to_fabric_credit_lfsr_thr_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_deq_default, dnx_data_iqs_deq_default_define_sram_to_fabric_credit_lfsr_thr);
}

uint32
dnx_data_iqs_deq_default_sram_to_fabric_credit_lfsr_mask_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_deq_default, dnx_data_iqs_deq_default_define_sram_to_fabric_credit_lfsr_mask);
}



const dnx_data_iqs_deq_default_sqm_read_weight_profiles_t *
dnx_data_iqs_deq_default_sqm_read_weight_profiles_get(
    int unit,
    int profile_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_deq_default, dnx_data_iqs_deq_default_table_sqm_read_weight_profiles);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, profile_id, 0);
    return (const dnx_data_iqs_deq_default_sqm_read_weight_profiles_t *) data;

}

const dnx_data_iqs_deq_default_dqm_read_weight_profiles_t *
dnx_data_iqs_deq_default_dqm_read_weight_profiles_get(
    int unit,
    int profile_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_deq_default, dnx_data_iqs_deq_default_table_dqm_read_weight_profiles);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, profile_id, 0);
    return (const dnx_data_iqs_deq_default_dqm_read_weight_profiles_t *) data;

}

const dnx_data_iqs_deq_default_params_t *
dnx_data_iqs_deq_default_params_get(
    int unit,
    int bw_level)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_deq_default, dnx_data_iqs_deq_default_table_params);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, bw_level, 0);
    return (const dnx_data_iqs_deq_default_params_t *) data;

}

const dnx_data_iqs_deq_default_dram_params_t *
dnx_data_iqs_deq_default_dram_params_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_deq_default, dnx_data_iqs_deq_default_table_dram_params);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_iqs_deq_default_dram_params_t *) data;

}

const dnx_data_iqs_deq_default_max_deq_cmd_t *
dnx_data_iqs_deq_default_max_deq_cmd_get(
    int unit,
    int bw_level,
    int nof_active_queues)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_deq_default, dnx_data_iqs_deq_default_table_max_deq_cmd);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, bw_level, nof_active_queues);
    return (const dnx_data_iqs_deq_default_max_deq_cmd_t *) data;

}

const dnx_data_iqs_deq_default_s2d_max_deq_cmd_t *
dnx_data_iqs_deq_default_s2d_max_deq_cmd_get(
    int unit,
    int bw_level,
    int nof_active_queues)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_deq_default, dnx_data_iqs_deq_default_table_s2d_max_deq_cmd);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, bw_level, nof_active_queues);
    return (const dnx_data_iqs_deq_default_s2d_max_deq_cmd_t *) data;

}


shr_error_e
dnx_data_iqs_deq_default_sqm_read_weight_profiles_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_iqs_deq_default_sqm_read_weight_profiles_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_deq_default, dnx_data_iqs_deq_default_table_sqm_read_weight_profiles);
    data = (const dnx_data_iqs_deq_default_sqm_read_weight_profiles_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->weight);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_iqs_deq_default_dqm_read_weight_profiles_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_iqs_deq_default_dqm_read_weight_profiles_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_deq_default, dnx_data_iqs_deq_default_table_dqm_read_weight_profiles);
    data = (const dnx_data_iqs_deq_default_dqm_read_weight_profiles_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->weight);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_iqs_deq_default_params_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_iqs_deq_default_params_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_deq_default, dnx_data_iqs_deq_default_table_params);
    data = (const dnx_data_iqs_deq_default_params_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_IQS_DEQ_PARAM_TYPE_NOF, data->sram_read_weight_profile_val);
            break;
        case 1:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_IQS_DEQ_PARAM_TYPE_NOF, data->s2d_read_weight_profile_val);
            break;
        case 2:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_IQS_DEQ_PARAM_TYPE_NOF, data->sram_extra_credits_val);
            break;
        case 3:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_IQS_DEQ_PARAM_TYPE_NOF, data->s2d_extra_credits_val);
            break;
        case 4:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_IQS_DEQ_PARAM_TYPE_NOF, data->sram_extra_credits_lfsr_val);
            break;
        case 5:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_IQS_DEQ_PARAM_TYPE_NOF, data->s2d_extra_credits_lfsr_val);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_iqs_deq_default_dram_params_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_iqs_deq_default_dram_params_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_deq_default, dnx_data_iqs_deq_default_table_dram_params);
    data = (const dnx_data_iqs_deq_default_dram_params_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->read_weight_profile_val);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->read_weight_profile_th);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_iqs_deq_default_max_deq_cmd_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_iqs_deq_default_max_deq_cmd_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_deq_default, dnx_data_iqs_deq_default_table_max_deq_cmd);
    data = (const dnx_data_iqs_deq_default_max_deq_cmd_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->value);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_iqs_deq_default_s2d_max_deq_cmd_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_iqs_deq_default_s2d_max_deq_cmd_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_deq_default, dnx_data_iqs_deq_default_table_s2d_max_deq_cmd);
    data = (const dnx_data_iqs_deq_default_s2d_max_deq_cmd_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->value);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_iqs_deq_default_sqm_read_weight_profiles_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_deq_default, dnx_data_iqs_deq_default_table_sqm_read_weight_profiles);

}

const dnxc_data_table_info_t *
dnx_data_iqs_deq_default_dqm_read_weight_profiles_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_deq_default, dnx_data_iqs_deq_default_table_dqm_read_weight_profiles);

}

const dnxc_data_table_info_t *
dnx_data_iqs_deq_default_params_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_deq_default, dnx_data_iqs_deq_default_table_params);

}

const dnxc_data_table_info_t *
dnx_data_iqs_deq_default_dram_params_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_deq_default, dnx_data_iqs_deq_default_table_dram_params);

}

const dnxc_data_table_info_t *
dnx_data_iqs_deq_default_max_deq_cmd_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_deq_default, dnx_data_iqs_deq_default_table_max_deq_cmd);

}

const dnxc_data_table_info_t *
dnx_data_iqs_deq_default_s2d_max_deq_cmd_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_deq_default, dnx_data_iqs_deq_default_table_s2d_max_deq_cmd);

}






static shr_error_e
dnx_data_iqs_dqcq_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "dqcq";
    submodule_data->doc = "DQCQ related data";
    
    submodule_data->nof_features = _dnx_data_iqs_dqcq_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data iqs dqcq features");

    submodule_data->features[dnx_data_iqs_dqcq_8_priorities].name = "8_priorities";
    submodule_data->features[dnx_data_iqs_dqcq_8_priorities].doc = "enable/disable DQCQ 8 priorities feature";
    submodule_data->features[dnx_data_iqs_dqcq_8_priorities].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_iqs_dqcq_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data iqs dqcq defines");

    submodule_data->defines[dnx_data_iqs_dqcq_define_max_nof_contexts].name = "max_nof_contexts";
    submodule_data->defines[dnx_data_iqs_dqcq_define_max_nof_contexts].doc = "max number of DQCQ contexts";
    
    submodule_data->defines[dnx_data_iqs_dqcq_define_max_nof_contexts].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_iqs_dqcq_define_nof_priorities].name = "nof_priorities";
    submodule_data->defines[dnx_data_iqs_dqcq_define_nof_priorities].doc = "number of DQCQ priorities";
    
    submodule_data->defines[dnx_data_iqs_dqcq_define_nof_priorities].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_iqs_dqcq_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data iqs dqcq tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_iqs_dqcq_feature_get(
    int unit,
    dnx_data_iqs_dqcq_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_dqcq, feature);
}


uint32
dnx_data_iqs_dqcq_max_nof_contexts_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_dqcq, dnx_data_iqs_dqcq_define_max_nof_contexts);
}

uint32
dnx_data_iqs_dqcq_nof_priorities_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_dqcq, dnx_data_iqs_dqcq_define_nof_priorities);
}










static shr_error_e
dnx_data_iqs_dbal_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "dbal";
    submodule_data->doc = "IQS specific information for DBAL";
    
    submodule_data->nof_features = _dnx_data_iqs_dbal_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data iqs dbal features");

    
    submodule_data->nof_defines = _dnx_data_iqs_dbal_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data iqs dbal defines");

    submodule_data->defines[dnx_data_iqs_dbal_define_ipt_counters_nof_bits].name = "ipt_counters_nof_bits";
    submodule_data->defines[dnx_data_iqs_dbal_define_ipt_counters_nof_bits].doc = "nof bits in IPT counters from IPS/GCM";
    
    submodule_data->defines[dnx_data_iqs_dbal_define_ipt_counters_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_iqs_dbal_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data iqs dbal tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_iqs_dbal_feature_get(
    int unit,
    dnx_data_iqs_dbal_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_dbal, feature);
}


uint32
dnx_data_iqs_dbal_ipt_counters_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_dbal, dnx_data_iqs_dbal_define_ipt_counters_nof_bits);
}










static shr_error_e
dnx_data_iqs_flush_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "flush";
    submodule_data->doc = "queue flush data";
    
    submodule_data->nof_features = _dnx_data_iqs_flush_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data iqs flush features");

    
    submodule_data->nof_defines = _dnx_data_iqs_flush_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data iqs flush defines");

    submodule_data->defines[dnx_data_iqs_flush_define_bytes_units_to_send].name = "bytes_units_to_send";
    submodule_data->defines[dnx_data_iqs_flush_define_bytes_units_to_send].doc = "Number of bytes to send from the queue in flush process (64B units)";
    
    submodule_data->defines[dnx_data_iqs_flush_define_bytes_units_to_send].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_iqs_flush_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data iqs flush tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_iqs_flush_feature_get(
    int unit,
    dnx_data_iqs_flush_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_flush, feature);
}


uint32
dnx_data_iqs_flush_bytes_units_to_send_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_iqs, dnx_data_iqs_submodule_flush, dnx_data_iqs_flush_define_bytes_units_to_send);
}







shr_error_e
dnx_data_iqs_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "iqs";
    module_data->nof_submodules = _dnx_data_iqs_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data iqs submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_iqs_credit_init(unit, &module_data->submodules[dnx_data_iqs_submodule_credit]));
    SHR_IF_ERR_EXIT(dnx_data_iqs_deq_default_init(unit, &module_data->submodules[dnx_data_iqs_submodule_deq_default]));
    SHR_IF_ERR_EXIT(dnx_data_iqs_dqcq_init(unit, &module_data->submodules[dnx_data_iqs_submodule_dqcq]));
    SHR_IF_ERR_EXIT(dnx_data_iqs_dbal_init(unit, &module_data->submodules[dnx_data_iqs_submodule_dbal]));
    SHR_IF_ERR_EXIT(dnx_data_iqs_flush_init(unit, &module_data->submodules[dnx_data_iqs_submodule_flush]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_iqs_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_iqs_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_iqs_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_iqs_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_iqs_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_iqs_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_iqs_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_iqs_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_iqs_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_iqs_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_iqs_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_iqs_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_iqs_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_iqs_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_iqs_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

