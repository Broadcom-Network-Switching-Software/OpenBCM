

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_TUNE_H_

#define _DNX_DATA_TUNE_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_tune.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_tune_init(
    int unit);






typedef struct
{
    
    int percentage_ratio;
} dnx_data_tune_ecgm_port_dp_ratio_t;



typedef enum
{

    
    _dnx_data_tune_ecgm_feature_nof
} dnx_data_tune_ecgm_feature_e;



typedef int(
    *dnx_data_tune_ecgm_feature_get_f) (
    int unit,
    dnx_data_tune_ecgm_feature_e feature);


typedef uint32(
    *dnx_data_tune_ecgm_core_flow_control_percentage_ratio_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tune_ecgm_port_uc_flow_control_min_divider_get_f) (
    int unit);


typedef const dnx_data_tune_ecgm_port_dp_ratio_t *(
    *dnx_data_tune_ecgm_port_dp_ratio_get_f) (
    int unit,
    int dp);



typedef struct
{
    
    dnx_data_tune_ecgm_feature_get_f feature_get;
    
    dnx_data_tune_ecgm_core_flow_control_percentage_ratio_get_f core_flow_control_percentage_ratio_get;
    
    dnx_data_tune_ecgm_port_uc_flow_control_min_divider_get_f port_uc_flow_control_min_divider_get;
    
    dnx_data_tune_ecgm_port_dp_ratio_get_f port_dp_ratio_get;
    
    dnxc_data_table_info_get_f port_dp_ratio_info_get;
} dnx_data_if_tune_ecgm_t;






typedef struct
{
    
    uint32 max;
    
    uint32 min;
} dnx_data_tune_iqs_bw_level_rate_t;



typedef enum
{

    
    _dnx_data_tune_iqs_feature_nof
} dnx_data_tune_iqs_feature_e;



typedef int(
    *dnx_data_tune_iqs_feature_get_f) (
    int unit,
    dnx_data_tune_iqs_feature_e feature);


typedef uint32(
    *dnx_data_tune_iqs_credit_resolution_up_th_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tune_iqs_fabric_delay_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tune_iqs_fabric_multicast_delay_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tune_iqs_fmq_max_rate_get_f) (
    int unit);


typedef const dnx_data_tune_iqs_bw_level_rate_t *(
    *dnx_data_tune_iqs_bw_level_rate_get_f) (
    int unit,
    int bw_level);



typedef struct
{
    
    dnx_data_tune_iqs_feature_get_f feature_get;
    
    dnx_data_tune_iqs_credit_resolution_up_th_get_f credit_resolution_up_th_get;
    
    dnx_data_tune_iqs_fabric_delay_get_f fabric_delay_get;
    
    dnx_data_tune_iqs_fabric_multicast_delay_get_f fabric_multicast_delay_get;
    
    dnx_data_tune_iqs_fmq_max_rate_get_f fmq_max_rate_get;
    
    dnx_data_tune_iqs_bw_level_rate_get_f bw_level_rate_get;
    
    dnxc_data_table_info_get_f bw_level_rate_info_get;
} dnx_data_if_tune_iqs_t;







typedef enum
{

    
    _dnx_data_tune_fabric_feature_nof
} dnx_data_tune_fabric_feature_e;



typedef int(
    *dnx_data_tune_fabric_feature_get_f) (
    int unit,
    dnx_data_tune_fabric_feature_e feature);


typedef uint32(
    *dnx_data_tune_fabric_cgm_llfc_pipe_th_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tune_fabric_cgm_drop_fabric_class_th_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tune_fabric_cgm_rci_high_sev_min_links_param_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_tune_fabric_feature_get_f feature_get;
    
    dnx_data_tune_fabric_cgm_llfc_pipe_th_get_f cgm_llfc_pipe_th_get;
    
    dnx_data_tune_fabric_cgm_drop_fabric_class_th_get_f cgm_drop_fabric_class_th_get;
    
    dnx_data_tune_fabric_cgm_rci_high_sev_min_links_param_get_f cgm_rci_high_sev_min_links_param_get;
} dnx_data_if_tune_fabric_t;





typedef struct
{
    
    dnx_data_if_tune_ecgm_t ecgm;
    
    dnx_data_if_tune_iqs_t iqs;
    
    dnx_data_if_tune_fabric_t fabric;
} dnx_data_if_tune_t;




extern dnx_data_if_tune_t dnx_data_tune;


#endif 

