
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_ESB_H_

#define _DNX_DATA_ESB_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_esb.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



shr_error_e dnx_data_if_esb_init(
    int unit);






typedef struct
{
    int speed;
    int if_id;
    int queue_id;
    int interleave_queue_id;
    int min_gap;
} dnx_data_esb_general_if_esb_params_t;



typedef enum
{
    dnx_data_esb_general_esb_support,
    dnx_data_esb_general_add_remove_queues_support,
    dnx_data_esb_general_slow_port_dedicated_queue_support,
    dnx_data_esb_general_flexe_l1_dest_dedicated_queue_support,
    dnx_data_esb_general_graceful_calendar_switching,
    dnx_data_esb_general_per_priority_queues_mapping,
    dnx_data_esb_general_internal_offset_mapping_not_supported,
    dnx_data_esb_general_is_used,
    dnx_data_esb_general_power_down_supported,

    
    _dnx_data_esb_general_feature_nof
} dnx_data_esb_general_feature_e;



typedef int(
    *dnx_data_esb_general_feature_get_f) (
    int unit,
    dnx_data_esb_general_feature_e feature);


typedef uint32(
    *dnx_data_esb_general_total_nof_esb_queues_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esb_general_nof_nif_eth_queues_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esb_general_nof_eth_lp_queues_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esb_general_nof_nif_ilkn_queues_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esb_general_nof_ilkn_tdm_or_lp_queues_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esb_general_nof_flexe_queues_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esb_general_nof_special_queues_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esb_general_nof_reserved_queues_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esb_general_nof_fpc_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esb_general_total_nof_buffers_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esb_general_mux_cal_len_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esb_general_mux_cal_window_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esb_general_nif_credits_resolution_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esb_general_nif_credits_rate_resolution_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esb_general_fpc_ptr_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esb_general_nof_buffers_per_fpc_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esb_general_l1_mux_cal_res_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esb_general_tm_egr_queuing_mux_cal_res_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esb_general_flexe_queue_id_base_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esb_general_ilkn_queue_id_base_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esb_general_ilkn_tdm_or_lp_queue_id_base_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esb_general_eth_lp_queue_id_base_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esb_general_nof_egr_txi_interfaces_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esb_general_max_queue_to_map_to_nif_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esb_general_txi_irdy_th_factor_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esb_general_txi_irdy_th_divident_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esb_general_slow_port_dedicated_queue_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esb_general_flexe_l1_dest_dedicated_queue_get_f) (
    int unit);


typedef const dnx_data_esb_general_if_esb_params_t *(
    *dnx_data_esb_general_if_esb_params_get_f) (
    int unit,
    int idx);



typedef struct
{
    
    dnx_data_esb_general_feature_get_f feature_get;
    
    dnx_data_esb_general_total_nof_esb_queues_get_f total_nof_esb_queues_get;
    
    dnx_data_esb_general_nof_nif_eth_queues_get_f nof_nif_eth_queues_get;
    
    dnx_data_esb_general_nof_eth_lp_queues_get_f nof_eth_lp_queues_get;
    
    dnx_data_esb_general_nof_nif_ilkn_queues_get_f nof_nif_ilkn_queues_get;
    
    dnx_data_esb_general_nof_ilkn_tdm_or_lp_queues_get_f nof_ilkn_tdm_or_lp_queues_get;
    
    dnx_data_esb_general_nof_flexe_queues_get_f nof_flexe_queues_get;
    
    dnx_data_esb_general_nof_special_queues_get_f nof_special_queues_get;
    
    dnx_data_esb_general_nof_reserved_queues_get_f nof_reserved_queues_get;
    
    dnx_data_esb_general_nof_fpc_get_f nof_fpc_get;
    
    dnx_data_esb_general_total_nof_buffers_get_f total_nof_buffers_get;
    
    dnx_data_esb_general_mux_cal_len_get_f mux_cal_len_get;
    
    dnx_data_esb_general_mux_cal_window_size_get_f mux_cal_window_size_get;
    
    dnx_data_esb_general_nif_credits_resolution_get_f nif_credits_resolution_get;
    
    dnx_data_esb_general_nif_credits_rate_resolution_get_f nif_credits_rate_resolution_get;
    
    dnx_data_esb_general_fpc_ptr_size_get_f fpc_ptr_size_get;
    
    dnx_data_esb_general_nof_buffers_per_fpc_get_f nof_buffers_per_fpc_get;
    
    dnx_data_esb_general_l1_mux_cal_res_get_f l1_mux_cal_res_get;
    
    dnx_data_esb_general_tm_egr_queuing_mux_cal_res_get_f tm_egr_queuing_mux_cal_res_get;
    
    dnx_data_esb_general_flexe_queue_id_base_get_f flexe_queue_id_base_get;
    
    dnx_data_esb_general_ilkn_queue_id_base_get_f ilkn_queue_id_base_get;
    
    dnx_data_esb_general_ilkn_tdm_or_lp_queue_id_base_get_f ilkn_tdm_or_lp_queue_id_base_get;
    
    dnx_data_esb_general_eth_lp_queue_id_base_get_f eth_lp_queue_id_base_get;
    
    dnx_data_esb_general_nof_egr_txi_interfaces_get_f nof_egr_txi_interfaces_get;
    
    dnx_data_esb_general_max_queue_to_map_to_nif_get_f max_queue_to_map_to_nif_get;
    
    dnx_data_esb_general_txi_irdy_th_factor_get_f txi_irdy_th_factor_get;
    
    dnx_data_esb_general_txi_irdy_th_divident_get_f txi_irdy_th_divident_get;
    
    dnx_data_esb_general_slow_port_dedicated_queue_get_f slow_port_dedicated_queue_get;
    
    dnx_data_esb_general_flexe_l1_dest_dedicated_queue_get_f flexe_l1_dest_dedicated_queue_get;
    
    dnx_data_esb_general_if_esb_params_get_f if_esb_params_get;
    
    dnxc_data_table_info_get_f if_esb_params_info_get;
} dnx_data_if_esb_general_t;







typedef enum
{

    
    _dnx_data_esb_dbal_feature_nof
} dnx_data_esb_dbal_feature_e;



typedef int(
    *dnx_data_esb_dbal_feature_get_f) (
    int unit,
    dnx_data_esb_dbal_feature_e feature);


typedef uint32(
    *dnx_data_esb_dbal_pm_internal_port_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esb_dbal_counter_config_mask_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esb_dbal_credit_size_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esb_dbal_txi_irdy_thresh_size_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_esb_dbal_feature_get_f feature_get;
    
    dnx_data_esb_dbal_pm_internal_port_size_get_f pm_internal_port_size_get;
    
    dnx_data_esb_dbal_counter_config_mask_size_get_f counter_config_mask_size_get;
    
    dnx_data_esb_dbal_credit_size_bits_get_f credit_size_bits_get;
    
    dnx_data_esb_dbal_txi_irdy_thresh_size_get_f txi_irdy_thresh_size_get;
} dnx_data_if_esb_dbal_t;





typedef struct
{
    
    dnx_data_if_esb_general_t general;
    
    dnx_data_if_esb_dbal_t dbal;
} dnx_data_if_esb_t;




extern dnx_data_if_esb_t dnx_data_esb;


#endif 

