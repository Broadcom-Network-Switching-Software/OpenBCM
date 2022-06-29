
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_TUNE_H_

#define _DNX_DATA_INTERNAL_TUNE_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tune.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
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
    dnx_data_tune_ecgm_define_port_queue_uc_flow_control_min_thresh,
    dnx_data_tune_ecgm_define_total_dbs,
    dnx_data_tune_ecgm_define_uc_pool_size_global_core_th,
    dnx_data_tune_ecgm_define_mc_pool_size_global_core_th,
    dnx_data_tune_ecgm_define_mc_drop_global_core_th,
    dnx_data_tune_ecgm_define_uc_fc_global_core_th,

    
    _dnx_data_tune_ecgm_define_nof
} dnx_data_tune_ecgm_define_e;



uint32 dnx_data_tune_ecgm_core_flow_control_percentage_ratio_get(
    int unit);


uint32 dnx_data_tune_ecgm_port_queue_uc_flow_control_min_thresh_get(
    int unit);


uint32 dnx_data_tune_ecgm_total_dbs_get(
    int unit);


uint32 dnx_data_tune_ecgm_uc_pool_size_global_core_th_get(
    int unit);


uint32 dnx_data_tune_ecgm_mc_pool_size_global_core_th_get(
    int unit);


uint32 dnx_data_tune_ecgm_mc_drop_global_core_th_get(
    int unit);


uint32 dnx_data_tune_ecgm_uc_fc_global_core_th_get(
    int unit);



typedef enum
{
    dnx_data_tune_ecgm_table_port_dp_ratio,
    dnx_data_tune_ecgm_table_tc_dp_fadt_th,
    dnx_data_tune_ecgm_table_interface_general_fadt_th,
    dnx_data_tune_ecgm_table_interface_rcy_fadt_th,
    dnx_data_tune_ecgm_table_interface_cpu_fadt_th,

    
    _dnx_data_tune_ecgm_table_nof
} dnx_data_tune_ecgm_table_e;



const dnx_data_tune_ecgm_port_dp_ratio_t * dnx_data_tune_ecgm_port_dp_ratio_get(
    int unit,
    int dp);


const dnx_data_tune_ecgm_tc_dp_fadt_th_t * dnx_data_tune_ecgm_tc_dp_fadt_th_get(
    int unit,
    int cast,
    int tc_level);


const dnx_data_tune_ecgm_interface_general_fadt_th_t * dnx_data_tune_ecgm_interface_general_fadt_th_get(
    int unit,
    int threshold_type,
    int port_rate);


const dnx_data_tune_ecgm_interface_rcy_fadt_th_t * dnx_data_tune_ecgm_interface_rcy_fadt_th_get(
    int unit,
    int threshold_type);


const dnx_data_tune_ecgm_interface_cpu_fadt_th_t * dnx_data_tune_ecgm_interface_cpu_fadt_th_get(
    int unit,
    int threshold_type);



shr_error_e dnx_data_tune_ecgm_port_dp_ratio_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_tune_ecgm_tc_dp_fadt_th_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_tune_ecgm_interface_general_fadt_th_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_tune_ecgm_interface_rcy_fadt_th_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_tune_ecgm_interface_cpu_fadt_th_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_tune_ecgm_port_dp_ratio_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_tune_ecgm_tc_dp_fadt_th_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_tune_ecgm_interface_general_fadt_th_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_tune_ecgm_interface_rcy_fadt_th_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_tune_ecgm_interface_cpu_fadt_th_info_get(
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
    dnx_data_tune_fabric_define_cgm_rci_egress_pipe_level_th_base,
    dnx_data_tune_fabric_define_cgm_rci_egress_pipe_level_th_offset,
    dnx_data_tune_fabric_define_cgm_rci_total_egress_pipe_level_th_base,
    dnx_data_tune_fabric_define_cgm_rci_total_egress_pipe_level_th_offset,

    
    _dnx_data_tune_fabric_define_nof
} dnx_data_tune_fabric_define_e;



uint32 dnx_data_tune_fabric_cgm_llfc_pipe_th_get(
    int unit);


uint32 dnx_data_tune_fabric_cgm_drop_fabric_class_th_get(
    int unit);


uint32 dnx_data_tune_fabric_cgm_rci_high_sev_min_links_param_get(
    int unit);


uint32 dnx_data_tune_fabric_cgm_rci_egress_pipe_level_th_base_get(
    int unit);


uint32 dnx_data_tune_fabric_cgm_rci_egress_pipe_level_th_offset_get(
    int unit);


uint32 dnx_data_tune_fabric_cgm_rci_total_egress_pipe_level_th_base_get(
    int unit);


uint32 dnx_data_tune_fabric_cgm_rci_total_egress_pipe_level_th_offset_get(
    int unit);



typedef enum
{
    dnx_data_tune_fabric_table_cgm_drop_fabric_multicast_th,
    dnx_data_tune_fabric_table_cgm_rci_egress_level_factor,

    
    _dnx_data_tune_fabric_table_nof
} dnx_data_tune_fabric_table_e;



const dnx_data_tune_fabric_cgm_drop_fabric_multicast_th_t * dnx_data_tune_fabric_cgm_drop_fabric_multicast_th_get(
    int unit,
    int priority);


const dnx_data_tune_fabric_cgm_rci_egress_level_factor_t * dnx_data_tune_fabric_cgm_rci_egress_level_factor_get(
    int unit,
    int level);



shr_error_e dnx_data_tune_fabric_cgm_drop_fabric_multicast_th_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_tune_fabric_cgm_rci_egress_level_factor_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_tune_fabric_cgm_drop_fabric_multicast_th_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_tune_fabric_cgm_rci_egress_level_factor_info_get(
    int unit);



shr_error_e dnx_data_tune_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

