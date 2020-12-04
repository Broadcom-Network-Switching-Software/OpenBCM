

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_ESB_H_

#define _DNX_DATA_INTERNAL_ESB_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esb.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_esb_submodule_general,
    dnx_data_esb_submodule_dbal,

    
    _dnx_data_esb_submodule_nof
} dnx_data_esb_submodule_e;








int dnx_data_esb_general_feature_get(
    int unit,
    dnx_data_esb_general_feature_e feature);



typedef enum
{
    dnx_data_esb_general_define_total_nof_esb_queues,
    dnx_data_esb_general_define_nof_nif_eth_queues,
    dnx_data_esb_general_define_nof_nif_ilkn_queues,
    dnx_data_esb_general_define_nof_ilkn_tdm_queues,
    dnx_data_esb_general_define_nof_flexe_queues,
    dnx_data_esb_general_define_nof_special_queues,
    dnx_data_esb_general_define_nof_reserved_queues,
    dnx_data_esb_general_define_nof_fpc,
    dnx_data_esb_general_define_total_nof_buffers,
    dnx_data_esb_general_define_mux_cal_len,
    dnx_data_esb_general_define_ilkn_credits_resolution,
    dnx_data_esb_general_define_ilkn_credits_rate_resolution,
    dnx_data_esb_general_define_fpc_ptr_size,
    dnx_data_esb_general_define_nof_buffers_per_fpc,
    dnx_data_esb_general_define_l1_mux_cal_res,
    dnx_data_esb_general_define_tm_egr_queuing_mux_cal_res,
    dnx_data_esb_general_define_txi_irdy_th_factor,
    dnx_data_esb_general_define_txi_irdy_th_divident,
    dnx_data_esb_general_define_slow_port_dedicated_queue,
    dnx_data_esb_general_define_flexe_l1_dest_dedicated_queue,

    
    _dnx_data_esb_general_define_nof
} dnx_data_esb_general_define_e;



uint32 dnx_data_esb_general_total_nof_esb_queues_get(
    int unit);


uint32 dnx_data_esb_general_nof_nif_eth_queues_get(
    int unit);


uint32 dnx_data_esb_general_nof_nif_ilkn_queues_get(
    int unit);


uint32 dnx_data_esb_general_nof_ilkn_tdm_queues_get(
    int unit);


uint32 dnx_data_esb_general_nof_flexe_queues_get(
    int unit);


uint32 dnx_data_esb_general_nof_special_queues_get(
    int unit);


uint32 dnx_data_esb_general_nof_reserved_queues_get(
    int unit);


uint32 dnx_data_esb_general_nof_fpc_get(
    int unit);


uint32 dnx_data_esb_general_total_nof_buffers_get(
    int unit);


uint32 dnx_data_esb_general_mux_cal_len_get(
    int unit);


uint32 dnx_data_esb_general_ilkn_credits_resolution_get(
    int unit);


uint32 dnx_data_esb_general_ilkn_credits_rate_resolution_get(
    int unit);


uint32 dnx_data_esb_general_fpc_ptr_size_get(
    int unit);


uint32 dnx_data_esb_general_nof_buffers_per_fpc_get(
    int unit);


uint32 dnx_data_esb_general_l1_mux_cal_res_get(
    int unit);


uint32 dnx_data_esb_general_tm_egr_queuing_mux_cal_res_get(
    int unit);


uint32 dnx_data_esb_general_txi_irdy_th_factor_get(
    int unit);


uint32 dnx_data_esb_general_txi_irdy_th_divident_get(
    int unit);


uint32 dnx_data_esb_general_slow_port_dedicated_queue_get(
    int unit);


uint32 dnx_data_esb_general_flexe_l1_dest_dedicated_queue_get(
    int unit);



typedef enum
{
    dnx_data_esb_general_table_if_esb_params,

    
    _dnx_data_esb_general_table_nof
} dnx_data_esb_general_table_e;



const dnx_data_esb_general_if_esb_params_t * dnx_data_esb_general_if_esb_params_get(
    int unit,
    int idx);



shr_error_e dnx_data_esb_general_if_esb_params_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_esb_general_if_esb_params_info_get(
    int unit);






int dnx_data_esb_dbal_feature_get(
    int unit,
    dnx_data_esb_dbal_feature_e feature);



typedef enum
{
    dnx_data_esb_dbal_define_pm_internal_port_size,
    dnx_data_esb_dbal_define_counter_config_mask_size,

    
    _dnx_data_esb_dbal_define_nof
} dnx_data_esb_dbal_define_e;



uint32 dnx_data_esb_dbal_pm_internal_port_size_get(
    int unit);


uint32 dnx_data_esb_dbal_counter_config_mask_size_get(
    int unit);



typedef enum
{

    
    _dnx_data_esb_dbal_table_nof
} dnx_data_esb_dbal_table_e;






shr_error_e dnx_data_esb_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

