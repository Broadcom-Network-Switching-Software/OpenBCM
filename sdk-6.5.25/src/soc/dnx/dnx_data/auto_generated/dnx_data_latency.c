
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_latency.h>



#ifdef BCM_DNX2_SUPPORT

extern shr_error_e jr2_a0_data_latency_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e jr2_b0_data_latency_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e j2c_a0_data_latency_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e q2a_a0_data_latency_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e q2a_b1_data_latency_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e j2p_a0_data_latency_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e j2x_a0_data_latency_attach(
    int unit);

#endif 



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
    submodule_data->features[dnx_data_latency_features_valid_flow_profile_flag].labels[0] = "j2x_notrev";
    submodule_data->features[dnx_data_latency_features_valid_flow_profile_flag].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_latency_features_valid_end_to_end_aqm_profile].name = "valid_end_to_end_aqm_profile";
    submodule_data->features[dnx_data_latency_features_valid_end_to_end_aqm_profile].doc = "can end to end aqm profile be created ";
    submodule_data->features[dnx_data_latency_features_valid_end_to_end_aqm_profile].labels[0] = "j2x_notrev";
    submodule_data->features[dnx_data_latency_features_valid_end_to_end_aqm_profile].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_latency_features_probabilistic_legacy_mechanism_support].name = "probabilistic_legacy_mechanism_support";
    submodule_data->features[dnx_data_latency_features_probabilistic_legacy_mechanism_support].doc = "is the probibalistic mark cni mechanism supported for ingress";
    submodule_data->features[dnx_data_latency_features_probabilistic_legacy_mechanism_support].labels[0] = "j2x_notrev";
    submodule_data->features[dnx_data_latency_features_probabilistic_legacy_mechanism_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_latency_features_egress_ecn_probabilistic_mechanism_support].name = "egress_ecn_probabilistic_mechanism_support";
    submodule_data->features[dnx_data_latency_features_egress_ecn_probabilistic_mechanism_support].doc = "is the probibalistic mark cni mechanism supported for ingress";
    submodule_data->features[dnx_data_latency_features_egress_ecn_probabilistic_mechanism_support].labels[0] = "j2x_notrev";
    submodule_data->features[dnx_data_latency_features_egress_ecn_probabilistic_mechanism_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_latency_features_ingress_ecn_probabilistic_mechanism_support].name = "ingress_ecn_probabilistic_mechanism_support";
    submodule_data->features[dnx_data_latency_features_ingress_ecn_probabilistic_mechanism_support].doc = "is the probibalistic mark cni mechanism supported for egress";
    submodule_data->features[dnx_data_latency_features_ingress_ecn_probabilistic_mechanism_support].labels[0] = "j2x_notrev";
    submodule_data->features[dnx_data_latency_features_ingress_ecn_probabilistic_mechanism_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_latency_features_drop_probabilistic_mechanism_support].name = "drop_probabilistic_mechanism_support";
    submodule_data->features[dnx_data_latency_features_drop_probabilistic_mechanism_support].doc = "is the probibalistic drop mechanism supported";
    submodule_data->features[dnx_data_latency_features_drop_probabilistic_mechanism_support].labels[0] = "j2x_notrev";
    submodule_data->features[dnx_data_latency_features_drop_probabilistic_mechanism_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_latency_features_coupled_mode_support].name = "coupled_mode_support";
    submodule_data->features[dnx_data_latency_features_coupled_mode_support].doc = "is the probibalistic mark cni/drop mechanism supported fore coupled mode";
    submodule_data->features[dnx_data_latency_features_coupled_mode_support].labels[0] = "j2x_notrev";
    submodule_data->features[dnx_data_latency_features_coupled_mode_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_latency_features_latency_measurement_support].name = "latency_measurement_support";
    submodule_data->features[dnx_data_latency_features_latency_measurement_support].doc = "indicate if latency can be measured correctly";
    submodule_data->features[dnx_data_latency_features_latency_measurement_support].labels[0] = "j2x_notrev";
    submodule_data->features[dnx_data_latency_features_latency_measurement_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_latency_features_valid_track_expansion].name = "valid_track_expansion";
    submodule_data->features[dnx_data_latency_features_valid_track_expansion].doc = "indicate if track expansion is valid";
    submodule_data->features[dnx_data_latency_features_valid_track_expansion].labels[0] = "j2x_notrev";
    submodule_data->features[dnx_data_latency_features_valid_track_expansion].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_latency_features_egress_latency_track_support].name = "egress_latency_track_support";
    submodule_data->features[dnx_data_latency_features_egress_latency_track_support].doc = "indicate if egress latency track is supported";
    submodule_data->features[dnx_data_latency_features_egress_latency_track_support].labels[0] = "j2x_notrev";
    submodule_data->features[dnx_data_latency_features_egress_latency_track_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_latency_features_ingress_based_admission].name = "ingress_based_admission";
    submodule_data->features[dnx_data_latency_features_ingress_based_admission].doc = "Is latency based admission supported";
    submodule_data->features[dnx_data_latency_features_ingress_based_admission].labels[0] = "j2x_notrev";
    submodule_data->features[dnx_data_latency_features_ingress_based_admission].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_latency_features_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data latency features defines");

    submodule_data->defines[dnx_data_latency_features_define_latency_bins].name = "latency_bins";
    submodule_data->defines[dnx_data_latency_features_define_latency_bins].doc = "number of latency bins ranges";
    submodule_data->defines[dnx_data_latency_features_define_latency_bins].labels[0] = "j2x_notrev";
    
    submodule_data->defines[dnx_data_latency_features_define_latency_bins].flags |= DNXC_DATA_F_DEFINE;

    
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


uint32
dnx_data_latency_features_latency_bins_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_latency, dnx_data_latency_submodule_features, dnx_data_latency_features_define_latency_bins);
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
    submodule_data->defines[dnx_data_latency_profile_define_egress_nof].labels[0] = "j2x_notrev";
    
    submodule_data->defines[dnx_data_latency_profile_define_egress_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_latency_profile_define_ingress_nof].name = "ingress_nof";
    submodule_data->defines[dnx_data_latency_profile_define_ingress_nof].doc = "Number of ingress latency profiles";
    submodule_data->defines[dnx_data_latency_profile_define_ingress_nof].labels[0] = "j2x_notrev";
    
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
dnx_data_latency_based_admission_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "based_admission";
    submodule_data->doc = "End-to-end Active Queue Managenemt definitions";
    
    submodule_data->nof_features = _dnx_data_latency_based_admission_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data latency based_admission features");

    submodule_data->features[dnx_data_latency_based_admission_cgm_extended_profiles_enable].name = "cgm_extended_profiles_enable";
    submodule_data->features[dnx_data_latency_based_admission_cgm_extended_profiles_enable].doc = "Is there cgm eco to be enabled in order to use voq-profiles(32) instead of being limited to only 16 flow-profiles";
    submodule_data->features[dnx_data_latency_based_admission_cgm_extended_profiles_enable].labels[0] = "j2x_notrev";
    submodule_data->features[dnx_data_latency_based_admission_cgm_extended_profiles_enable].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_latency_based_admission_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data latency based_admission defines");

    submodule_data->defines[dnx_data_latency_based_admission_define_reject_flow_profile_value].name = "reject_flow_profile_value";
    submodule_data->defines[dnx_data_latency_based_admission_define_reject_flow_profile_value].doc = "";
    submodule_data->defines[dnx_data_latency_based_admission_define_reject_flow_profile_value].labels[0] = "j2x_notrev";
    
    submodule_data->defines[dnx_data_latency_based_admission_define_reject_flow_profile_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_latency_based_admission_define_cgm_extended_profiles_enable_value].name = "cgm_extended_profiles_enable_value";
    submodule_data->defines[dnx_data_latency_based_admission_define_cgm_extended_profiles_enable_value].doc = "If there is cgm eco to be enabled for latency based admissions profiles - set this value to the spare register";
    submodule_data->defines[dnx_data_latency_based_admission_define_cgm_extended_profiles_enable_value].labels[0] = "j2x_notrev";
    
    submodule_data->defines[dnx_data_latency_based_admission_define_cgm_extended_profiles_enable_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_latency_based_admission_define_max_supported_profile].name = "max_supported_profile";
    submodule_data->defines[dnx_data_latency_based_admission_define_max_supported_profile].doc = "For latency based admission not all latency profiles are supported to be used";
    submodule_data->defines[dnx_data_latency_based_admission_define_max_supported_profile].labels[0] = "j2x_notrev";
    
    submodule_data->defines[dnx_data_latency_based_admission_define_max_supported_profile].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_latency_based_admission_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data latency based_admission tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_latency_based_admission_feature_get(
    int unit,
    dnx_data_latency_based_admission_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_latency, dnx_data_latency_submodule_based_admission, feature);
}


uint32
dnx_data_latency_based_admission_reject_flow_profile_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_latency, dnx_data_latency_submodule_based_admission, dnx_data_latency_based_admission_define_reject_flow_profile_value);
}

uint32
dnx_data_latency_based_admission_cgm_extended_profiles_enable_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_latency, dnx_data_latency_submodule_based_admission, dnx_data_latency_based_admission_define_cgm_extended_profiles_enable_value);
}

uint32
dnx_data_latency_based_admission_max_supported_profile_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_latency, dnx_data_latency_submodule_based_admission, dnx_data_latency_based_admission_define_max_supported_profile);
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
    submodule_data->defines[dnx_data_latency_ingress_define_jr_mode_latency_timestamp_left_shift].labels[0] = "j2x_notrev";
    
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
    SHR_IF_ERR_EXIT(dnx_data_latency_based_admission_init(unit, &module_data->submodules[dnx_data_latency_submodule_based_admission]));
    SHR_IF_ERR_EXIT(dnx_data_latency_ingress_init(unit, &module_data->submodules[dnx_data_latency_submodule_ingress]));
    
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_latency_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_latency_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_latency_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_latency_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_latency_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_latency_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_latency_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_latency_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_latency_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_latency_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_latency_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_latency_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_latency_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_latency_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_latency_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b1_data_latency_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_latency_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_latency_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_latency_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_latency_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_latency_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_latency_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_latency_attach(unit));
        SHR_IF_ERR_EXIT(j2x_a0_data_latency_attach(unit));
    }
    else

#endif 
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

