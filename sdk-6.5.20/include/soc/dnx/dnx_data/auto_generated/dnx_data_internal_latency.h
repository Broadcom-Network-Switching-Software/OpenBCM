

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_LATENCY_H_

#define _DNX_DATA_INTERNAL_LATENCY_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_latency.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_latency_submodule_features,
    dnx_data_latency_submodule_profile,
    dnx_data_latency_submodule_aqm,
    dnx_data_latency_submodule_ingress,

    
    _dnx_data_latency_submodule_nof
} dnx_data_latency_submodule_e;








int dnx_data_latency_features_feature_get(
    int unit,
    dnx_data_latency_features_feature_e feature);



typedef enum
{

    
    _dnx_data_latency_features_define_nof
} dnx_data_latency_features_define_e;




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









int dnx_data_latency_aqm_feature_get(
    int unit,
    dnx_data_latency_aqm_feature_e feature);



typedef enum
{
    dnx_data_latency_aqm_define_profiles_nof,
    dnx_data_latency_aqm_define_flows_nof,

    
    _dnx_data_latency_aqm_define_nof
} dnx_data_latency_aqm_define_e;



uint32 dnx_data_latency_aqm_profiles_nof_get(
    int unit);


uint32 dnx_data_latency_aqm_flows_nof_get(
    int unit);



typedef enum
{

    
    _dnx_data_latency_aqm_table_nof
} dnx_data_latency_aqm_table_e;









int dnx_data_latency_ingress_feature_get(
    int unit,
    dnx_data_latency_ingress_feature_e feature);



typedef enum
{
    dnx_data_latency_ingress_define_jr_mode_latency_timestamp_left_shift,

    
    _dnx_data_latency_ingress_define_nof
} dnx_data_latency_ingress_define_e;



uint32 dnx_data_latency_ingress_jr_mode_latency_timestamp_left_shift_get(
    int unit);



typedef enum
{

    
    _dnx_data_latency_ingress_table_nof
} dnx_data_latency_ingress_table_e;






shr_error_e dnx_data_latency_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

