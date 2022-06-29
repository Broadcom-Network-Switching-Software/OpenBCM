
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_TUNE_H_

#define _DNX_DATA_TUNE_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_tune.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



shr_error_e dnx_data_if_tune_init(
    int unit);






typedef struct
{
    int percentage_ratio;
} dnx_data_tune_ecgm_port_dp_ratio_t;


typedef struct
{
    uint32 thresh_min;
    uint32 thresh_max;
    int alpha;
    int alpha1;
} dnx_data_tune_ecgm_tc_dp_fadt_th_t;


typedef struct
{
    uint32 thresh_min;
    uint32 thresh_max;
    int alpha;
    int alpha1;
} dnx_data_tune_ecgm_interface_general_fadt_th_t;


typedef struct
{
    uint32 thresh_min;
    uint32 thresh_max;
    int alpha;
    int alpha1;
} dnx_data_tune_ecgm_interface_rcy_fadt_th_t;


typedef struct
{
    uint32 thresh_min;
    uint32 thresh_max;
    int alpha;
    int alpha1;
} dnx_data_tune_ecgm_interface_cpu_fadt_th_t;



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
    *dnx_data_tune_ecgm_port_queue_uc_flow_control_min_thresh_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tune_ecgm_total_dbs_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tune_ecgm_uc_pool_size_global_core_th_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tune_ecgm_mc_pool_size_global_core_th_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tune_ecgm_mc_drop_global_core_th_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tune_ecgm_uc_fc_global_core_th_get_f) (
    int unit);


typedef const dnx_data_tune_ecgm_port_dp_ratio_t *(
    *dnx_data_tune_ecgm_port_dp_ratio_get_f) (
    int unit,
    int dp);


typedef const dnx_data_tune_ecgm_tc_dp_fadt_th_t *(
    *dnx_data_tune_ecgm_tc_dp_fadt_th_get_f) (
    int unit,
    int cast,
    int tc_level);


typedef const dnx_data_tune_ecgm_interface_general_fadt_th_t *(
    *dnx_data_tune_ecgm_interface_general_fadt_th_get_f) (
    int unit,
    int threshold_type,
    int port_rate);


typedef const dnx_data_tune_ecgm_interface_rcy_fadt_th_t *(
    *dnx_data_tune_ecgm_interface_rcy_fadt_th_get_f) (
    int unit,
    int threshold_type);


typedef const dnx_data_tune_ecgm_interface_cpu_fadt_th_t *(
    *dnx_data_tune_ecgm_interface_cpu_fadt_th_get_f) (
    int unit,
    int threshold_type);



typedef struct
{
    
    dnx_data_tune_ecgm_feature_get_f feature_get;
    
    dnx_data_tune_ecgm_core_flow_control_percentage_ratio_get_f core_flow_control_percentage_ratio_get;
    
    dnx_data_tune_ecgm_port_queue_uc_flow_control_min_thresh_get_f port_queue_uc_flow_control_min_thresh_get;
    
    dnx_data_tune_ecgm_total_dbs_get_f total_dbs_get;
    
    dnx_data_tune_ecgm_uc_pool_size_global_core_th_get_f uc_pool_size_global_core_th_get;
    
    dnx_data_tune_ecgm_mc_pool_size_global_core_th_get_f mc_pool_size_global_core_th_get;
    
    dnx_data_tune_ecgm_mc_drop_global_core_th_get_f mc_drop_global_core_th_get;
    
    dnx_data_tune_ecgm_uc_fc_global_core_th_get_f uc_fc_global_core_th_get;
    
    dnx_data_tune_ecgm_port_dp_ratio_get_f port_dp_ratio_get;
    
    dnxc_data_table_info_get_f port_dp_ratio_info_get;
    
    dnx_data_tune_ecgm_tc_dp_fadt_th_get_f tc_dp_fadt_th_get;
    
    dnxc_data_table_info_get_f tc_dp_fadt_th_info_get;
    
    dnx_data_tune_ecgm_interface_general_fadt_th_get_f interface_general_fadt_th_get;
    
    dnxc_data_table_info_get_f interface_general_fadt_th_info_get;
    
    dnx_data_tune_ecgm_interface_rcy_fadt_th_get_f interface_rcy_fadt_th_get;
    
    dnxc_data_table_info_get_f interface_rcy_fadt_th_info_get;
    
    dnx_data_tune_ecgm_interface_cpu_fadt_th_get_f interface_cpu_fadt_th_get;
    
    dnxc_data_table_info_get_f interface_cpu_fadt_th_info_get;
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






typedef struct
{
    uint32 threshold;
} dnx_data_tune_fabric_cgm_drop_fabric_multicast_th_t;


typedef struct
{
    uint32 score;
} dnx_data_tune_fabric_cgm_rci_egress_level_factor_t;



typedef enum
{
    dnx_data_tune_fabric_cgm_rci_update_egress_th_values,

    
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


typedef uint32(
    *dnx_data_tune_fabric_cgm_rci_egress_pipe_level_th_base_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tune_fabric_cgm_rci_egress_pipe_level_th_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tune_fabric_cgm_rci_total_egress_pipe_level_th_base_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tune_fabric_cgm_rci_total_egress_pipe_level_th_offset_get_f) (
    int unit);


typedef const dnx_data_tune_fabric_cgm_drop_fabric_multicast_th_t *(
    *dnx_data_tune_fabric_cgm_drop_fabric_multicast_th_get_f) (
    int unit,
    int priority);


typedef const dnx_data_tune_fabric_cgm_rci_egress_level_factor_t *(
    *dnx_data_tune_fabric_cgm_rci_egress_level_factor_get_f) (
    int unit,
    int level);



typedef struct
{
    
    dnx_data_tune_fabric_feature_get_f feature_get;
    
    dnx_data_tune_fabric_cgm_llfc_pipe_th_get_f cgm_llfc_pipe_th_get;
    
    dnx_data_tune_fabric_cgm_drop_fabric_class_th_get_f cgm_drop_fabric_class_th_get;
    
    dnx_data_tune_fabric_cgm_rci_high_sev_min_links_param_get_f cgm_rci_high_sev_min_links_param_get;
    
    dnx_data_tune_fabric_cgm_rci_egress_pipe_level_th_base_get_f cgm_rci_egress_pipe_level_th_base_get;
    
    dnx_data_tune_fabric_cgm_rci_egress_pipe_level_th_offset_get_f cgm_rci_egress_pipe_level_th_offset_get;
    
    dnx_data_tune_fabric_cgm_rci_total_egress_pipe_level_th_base_get_f cgm_rci_total_egress_pipe_level_th_base_get;
    
    dnx_data_tune_fabric_cgm_rci_total_egress_pipe_level_th_offset_get_f cgm_rci_total_egress_pipe_level_th_offset_get;
    
    dnx_data_tune_fabric_cgm_drop_fabric_multicast_th_get_f cgm_drop_fabric_multicast_th_get;
    
    dnxc_data_table_info_get_f cgm_drop_fabric_multicast_th_info_get;
    
    dnx_data_tune_fabric_cgm_rci_egress_level_factor_get_f cgm_rci_egress_level_factor_get;
    
    dnxc_data_table_info_get_f cgm_rci_egress_level_factor_info_get;
} dnx_data_if_tune_fabric_t;





typedef struct
{
    
    dnx_data_if_tune_ecgm_t ecgm;
    
    dnx_data_if_tune_iqs_t iqs;
    
    dnx_data_if_tune_fabric_t fabric;
} dnx_data_if_tune_t;




extern dnx_data_if_tune_t dnx_data_tune;


#endif 

