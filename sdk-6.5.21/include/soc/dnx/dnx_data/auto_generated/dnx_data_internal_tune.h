

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_TUNE_H_

#define _DNX_DATA_INTERNAL_TUNE_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tune.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_tune_submodule_ecgm,
    dnx_data_tune_submodule_iqs,
    dnx_data_tune_submodule_fabric,

    
    _dnx_data_tune_submodule_nof
} dnx_data_tune_submodule_e;








int dnx_data_tune_ecgm_feature_get(
    int unit,
    dnx_data_tune_ecgm_feature_e feature);



typedef enum
{
    dnx_data_tune_ecgm_define_core_flow_control_percentage_ratio,
    dnx_data_tune_ecgm_define_port_uc_flow_control_min_divider,

    
    _dnx_data_tune_ecgm_define_nof
} dnx_data_tune_ecgm_define_e;



uint32 dnx_data_tune_ecgm_core_flow_control_percentage_ratio_get(
    int unit);


uint32 dnx_data_tune_ecgm_port_uc_flow_control_min_divider_get(
    int unit);



typedef enum
{
    dnx_data_tune_ecgm_table_port_dp_ratio,

    
    _dnx_data_tune_ecgm_table_nof
} dnx_data_tune_ecgm_table_e;



const dnx_data_tune_ecgm_port_dp_ratio_t * dnx_data_tune_ecgm_port_dp_ratio_get(
    int unit,
    int dp);



shr_error_e dnx_data_tune_ecgm_port_dp_ratio_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_tune_ecgm_port_dp_ratio_info_get(
    int unit);






int dnx_data_tune_iqs_feature_get(
    int unit,
    dnx_data_tune_iqs_feature_e feature);



typedef enum
{
    dnx_data_tune_iqs_define_credit_resolution_up_th,
    dnx_data_tune_iqs_define_fabric_delay,
    dnx_data_tune_iqs_define_fabric_multicast_delay,
    dnx_data_tune_iqs_define_fmq_max_rate,

    
    _dnx_data_tune_iqs_define_nof
} dnx_data_tune_iqs_define_e;



uint32 dnx_data_tune_iqs_credit_resolution_up_th_get(
    int unit);


uint32 dnx_data_tune_iqs_fabric_delay_get(
    int unit);


uint32 dnx_data_tune_iqs_fabric_multicast_delay_get(
    int unit);


uint32 dnx_data_tune_iqs_fmq_max_rate_get(
    int unit);



typedef enum
{
    dnx_data_tune_iqs_table_bw_level_rate,

    
    _dnx_data_tune_iqs_table_nof
} dnx_data_tune_iqs_table_e;



const dnx_data_tune_iqs_bw_level_rate_t * dnx_data_tune_iqs_bw_level_rate_get(
    int unit,
    int bw_level);



shr_error_e dnx_data_tune_iqs_bw_level_rate_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_tune_iqs_bw_level_rate_info_get(
    int unit);






int dnx_data_tune_fabric_feature_get(
    int unit,
    dnx_data_tune_fabric_feature_e feature);



typedef enum
{
    dnx_data_tune_fabric_define_cgm_llfc_pipe_th,
    dnx_data_tune_fabric_define_cgm_drop_fabric_class_th,
    dnx_data_tune_fabric_define_cgm_rci_high_sev_min_links_param,

    
    _dnx_data_tune_fabric_define_nof
} dnx_data_tune_fabric_define_e;



uint32 dnx_data_tune_fabric_cgm_llfc_pipe_th_get(
    int unit);


uint32 dnx_data_tune_fabric_cgm_drop_fabric_class_th_get(
    int unit);


uint32 dnx_data_tune_fabric_cgm_rci_high_sev_min_links_param_get(
    int unit);



typedef enum
{

    
    _dnx_data_tune_fabric_table_nof
} dnx_data_tune_fabric_table_e;






shr_error_e dnx_data_tune_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

