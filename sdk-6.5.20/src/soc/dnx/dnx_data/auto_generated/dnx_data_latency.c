

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_latency.h>



extern shr_error_e jr2_a0_data_latency_attach(
    int unit);
extern shr_error_e j2c_a0_data_latency_attach(
    int unit);
extern shr_error_e q2a_a0_data_latency_attach(
    int unit);
extern shr_error_e j2p_a0_data_latency_attach(
    int unit);



static shr_error_e
dnx_data_latency_features_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "features";
    submodule_data->doc = "latency stat id definitions";
    
    submodule_data->nof_features = _dnx_data_latency_features_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data latency features features");

    submodule_data->features[dnx_data_latency_features_valid_flow_profile_flag].name = "valid_flow_profile_flag";
    submodule_data->features[dnx_data_latency_features_valid_flow_profile_flag].doc = "should user add a flag to the latency profile create API in order to set valid bit and enable latency to be measured per pp flow profile";
    submodule_data->features[dnx_data_latency_features_valid_flow_profile_flag].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_latency_features_valid_end_to_end_aqm_profile].name = "valid_end_to_end_aqm_profile";
    submodule_data->features[dnx_data_latency_features_valid_end_to_end_aqm_profile].doc = "can end to end aqm profile be created ";
    submodule_data->features[dnx_data_latency_features_valid_end_to_end_aqm_profile].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_latency_features_probabilistic_mechanism_support].name = "probabilistic_mechanism_support";
    submodule_data->features[dnx_data_latency_features_probabilistic_mechanism_support].doc = "is the probibalistic mark cni/drop mechanism supported (ingress deqeue latency marking)";
    submodule_data->features[dnx_data_latency_features_probabilistic_mechanism_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_latency_features_latency_measurement_support].name = "latency_measurement_support";
    submodule_data->features[dnx_data_latency_features_latency_measurement_support].doc = "indicate if latency can be measured correctly";
    submodule_data->features[dnx_data_latency_features_latency_measurement_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_latency_features_valid_track_expansion].name = "valid_track_expansion";
    submodule_data->features[dnx_data_latency_features_valid_track_expansion].doc = "indicate if track expansion is valid";
    submodule_data->features[dnx_data_latency_features_valid_track_expansion].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_latency_features_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data latency features defines");

    
    submodule_data->nof_tables = _dnx_data_latency_features_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data latency features tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_latency_features_feature_get(
    int unit,
    dnx_data_latency_features_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_latency, dnx_data_latency_submodule_features, feature);
}











static shr_error_e
dnx_data_latency_profile_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "profile";
    submodule_data->doc = "latency profile definitions";
    
    submodule_data->nof_features = _dnx_data_latency_profile_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data latency profile features");

    
    submodule_data->nof_defines = _dnx_data_latency_profile_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data latency profile defines");

    submodule_data->defines[dnx_data_latency_profile_define_egress_nof].name = "egress_nof";
    submodule_data->defines[dnx_data_latency_profile_define_egress_nof].doc = "Number of egress (end-to-end) latency profiles";
    
    submodule_data->defines[dnx_data_latency_profile_define_egress_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_latency_profile_define_ingress_nof].name = "ingress_nof";
    submodule_data->defines[dnx_data_latency_profile_define_ingress_nof].doc = "Number of ingress latency profiles";
    
    submodule_data->defines[dnx_data_latency_profile_define_ingress_nof].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_latency_profile_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data latency profile tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_latency_profile_feature_get(
    int unit,
    dnx_data_latency_profile_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_latency, dnx_data_latency_submodule_profile, feature);
}


uint32
dnx_data_latency_profile_egress_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_latency, dnx_data_latency_submodule_profile, dnx_data_latency_profile_define_egress_nof);
}

uint32
dnx_data_latency_profile_ingress_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_latency, dnx_data_latency_submodule_profile, dnx_data_latency_profile_define_ingress_nof);
}










static shr_error_e
dnx_data_latency_aqm_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "aqm";
    submodule_data->doc = "End-to-end Active Queue Managenemt definitions";
    
    submodule_data->nof_features = _dnx_data_latency_aqm_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data latency aqm features");

    
    submodule_data->nof_defines = _dnx_data_latency_aqm_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data latency aqm defines");

    submodule_data->defines[dnx_data_latency_aqm_define_profiles_nof].name = "profiles_nof";
    submodule_data->defines[dnx_data_latency_aqm_define_profiles_nof].doc = "Number of AQM profiles";
    
    submodule_data->defines[dnx_data_latency_aqm_define_profiles_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_latency_aqm_define_flows_nof].name = "flows_nof";
    submodule_data->defines[dnx_data_latency_aqm_define_flows_nof].doc = "Number AQM flow IDs";
    
    submodule_data->defines[dnx_data_latency_aqm_define_flows_nof].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_latency_aqm_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data latency aqm tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_latency_aqm_feature_get(
    int unit,
    dnx_data_latency_aqm_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_latency, dnx_data_latency_submodule_aqm, feature);
}


uint32
dnx_data_latency_aqm_profiles_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_latency, dnx_data_latency_submodule_aqm, dnx_data_latency_aqm_define_profiles_nof);
}

uint32
dnx_data_latency_aqm_flows_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_latency, dnx_data_latency_submodule_aqm, dnx_data_latency_aqm_define_flows_nof);
}










static shr_error_e
dnx_data_latency_ingress_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ingress";
    submodule_data->doc = "Ingress Latency definitions";
    
    submodule_data->nof_features = _dnx_data_latency_ingress_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data latency ingress features");

    
    submodule_data->nof_defines = _dnx_data_latency_ingress_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data latency ingress defines");

    submodule_data->defines[dnx_data_latency_ingress_define_jr_mode_latency_timestamp_left_shift].name = "jr_mode_latency_timestamp_left_shift";
    submodule_data->defines[dnx_data_latency_ingress_define_jr_mode_latency_timestamp_left_shift].doc = "The time value taken from the OAM TS header is shifted left by this amount of bits";
    
    submodule_data->defines[dnx_data_latency_ingress_define_jr_mode_latency_timestamp_left_shift].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_latency_ingress_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data latency ingress tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_latency_ingress_feature_get(
    int unit,
    dnx_data_latency_ingress_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_latency, dnx_data_latency_submodule_ingress, feature);
}


uint32
dnx_data_latency_ingress_jr_mode_latency_timestamp_left_shift_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_latency, dnx_data_latency_submodule_ingress, dnx_data_latency_ingress_define_jr_mode_latency_timestamp_left_shift);
}







shr_error_e
dnx_data_latency_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "latency";
    module_data->nof_submodules = _dnx_data_latency_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data latency submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_latency_features_init(unit, &module_data->submodules[dnx_data_latency_submodule_features]));
    SHR_IF_ERR_EXIT(dnx_data_latency_profile_init(unit, &module_data->submodules[dnx_data_latency_submodule_profile]));
    SHR_IF_ERR_EXIT(dnx_data_latency_aqm_init(unit, &module_data->submodules[dnx_data_latency_submodule_aqm]));
    SHR_IF_ERR_EXIT(dnx_data_latency_ingress_init(unit, &module_data->submodules[dnx_data_latency_submodule_ingress]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_latency_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_latency_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_latency_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_latency_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_latency_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_latency_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_latency_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_latency_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_latency_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_latency_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_latency_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_latency_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_latency_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_latency_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_latency_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

