
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_LATENCY_H_

#define _DNX_DATA_INTERNAL_LATENCY_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_latency.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



typedef enum
{
    dnx_data_latency_submodule_features,
    dnx_data_latency_submodule_profile,
    dnx_data_latency_submodule_based_admission,
    dnx_data_latency_submodule_ingress,

    
    _dnx_data_latency_submodule_nof
} dnx_data_latency_submodule_e;








int dnx_data_latency_features_feature_get(
    int unit,
    dnx_data_latency_features_feature_e feature);



typedef enum
{
    dnx_data_latency_features_define_latency_bins,

    
    _dnx_data_latency_features_define_nof
} dnx_data_latency_features_define_e;



uint32 dnx_data_latency_features_latency_bins_get(
    int unit);



typedef enum
{

    
    _dnx_data_latency_features_table_nof
} dnx_data_latency_features_table_e;









int dnx_data_latency_profile_feature_get(
    int unit,
    dnx_data_latency_profile_feature_e feature);



typedef enum
{
    dnx_data_latency_profile_define_egress_nof,
    dnx_data_latency_profile_define_ingress_nof,

    
    _dnx_data_latency_profile_define_nof
} dnx_data_latency_profile_define_e;



uint32 dnx_data_latency_profile_egress_nof_get(
    int unit);


uint32 dnx_data_latency_profile_ingress_nof_get(
    int unit);



typedef enum
{

    
    _dnx_data_latency_profile_table_nof
} dnx_data_latency_profile_table_e;









int dnx_data_latency_based_admission_feature_get(
    int unit,
    dnx_data_latency_based_admission_feature_e feature);



typedef enum
{
    dnx_data_latency_based_admission_define_reject_flow_profile_value,
    dnx_data_latency_based_admission_define_cgm_extended_profiles_enable_value,
    dnx_data_latency_based_admission_define_max_supported_profile,

    
    _dnx_data_latency_based_admission_define_nof
} dnx_data_latency_based_admission_define_e;



uint32 dnx_data_latency_based_admission_reject_flow_profile_value_get(
    int unit);


uint32 dnx_data_latency_based_admission_cgm_extended_profiles_enable_value_get(
    int unit);


uint32 dnx_data_latency_based_admission_max_supported_profile_get(
    int unit);



typedef enum
{

    
    _dnx_data_latency_based_admission_table_nof
} dnx_data_latency_based_admission_table_e;









int dnx_data_latency_ingress_feature_get(
    int unit,
    dnx_data_latency_ingress_feature_e feature);



typedef enum
{
    dnx_data_latency_ingress_define_jr_mode_latency_timestamp_left_shift,
    dnx_data_latency_ingress_define_nof_flow_profile,

    
    _dnx_data_latency_ingress_define_nof
} dnx_data_latency_ingress_define_e;



uint32 dnx_data_latency_ingress_jr_mode_latency_timestamp_left_shift_get(
    int unit);


uint32 dnx_data_latency_ingress_nof_flow_profile_get(
    int unit);



typedef enum
{

    
    _dnx_data_latency_ingress_table_nof
} dnx_data_latency_ingress_table_e;






shr_error_e dnx_data_latency_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

