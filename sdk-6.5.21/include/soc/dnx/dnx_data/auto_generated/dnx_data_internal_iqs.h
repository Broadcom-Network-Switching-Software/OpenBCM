

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_IQS_H_

#define _DNX_DATA_INTERNAL_IQS_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_iqs.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_iqs_submodule_credit,
    dnx_data_iqs_submodule_deq_default,
    dnx_data_iqs_submodule_dqcq,
    dnx_data_iqs_submodule_dbal,
    dnx_data_iqs_submodule_flush,

    
    _dnx_data_iqs_submodule_nof
} dnx_data_iqs_submodule_e;








int dnx_data_iqs_credit_feature_get(
    int unit,
    dnx_data_iqs_credit_feature_e feature);



typedef enum
{
    dnx_data_iqs_credit_define_nof_profiles,
    dnx_data_iqs_credit_define_max_credit_balance_threshold,
    dnx_data_iqs_credit_define_max_worth,
    dnx_data_iqs_credit_define_satisified_back_min,
    dnx_data_iqs_credit_define_satisified_back_max,
    dnx_data_iqs_credit_define_satisified_empty_min,
    dnx_data_iqs_credit_define_satisified_empty_max,
    dnx_data_iqs_credit_define_hungry_min,
    dnx_data_iqs_credit_define_hungry_max,
    dnx_data_iqs_credit_define_hungry_mult_min,
    dnx_data_iqs_credit_define_hungry_mult_max,
    dnx_data_iqs_credit_define_nof_bw_levels,
    dnx_data_iqs_credit_define_fmq_credit_fc_on_th,
    dnx_data_iqs_credit_define_fmq_credit_fc_off_th,
    dnx_data_iqs_credit_define_fmq_byte_fc_on_th,
    dnx_data_iqs_credit_define_fmq_byte_fc_off_th,
    dnx_data_iqs_credit_define_fmq_eir_credit_fc_th,
    dnx_data_iqs_credit_define_fmq_eir_byte_fc_th,
    dnx_data_iqs_credit_define_fmq_nof_be_classes,
    dnx_data_iqs_credit_define_fmq_max_be_weight,
    dnx_data_iqs_credit_define_fmq_max_burst_max,
    dnx_data_iqs_credit_define_fmq_shp_crdt_rate_mltp,
    dnx_data_iqs_credit_define_fmq_shp_crdt_rate_delta,
    dnx_data_iqs_credit_define_max_rate_profile_preset_gbps,
    dnx_data_iqs_credit_define_worth,
    dnx_data_iqs_credit_define_fmq_shp_rate_max,

    
    _dnx_data_iqs_credit_define_nof
} dnx_data_iqs_credit_define_e;



uint32 dnx_data_iqs_credit_nof_profiles_get(
    int unit);


uint32 dnx_data_iqs_credit_max_credit_balance_threshold_get(
    int unit);


uint32 dnx_data_iqs_credit_max_worth_get(
    int unit);


int dnx_data_iqs_credit_satisified_back_min_get(
    int unit);


int dnx_data_iqs_credit_satisified_back_max_get(
    int unit);


int dnx_data_iqs_credit_satisified_empty_min_get(
    int unit);


int dnx_data_iqs_credit_satisified_empty_max_get(
    int unit);


int dnx_data_iqs_credit_hungry_min_get(
    int unit);


int dnx_data_iqs_credit_hungry_max_get(
    int unit);


int dnx_data_iqs_credit_hungry_mult_min_get(
    int unit);


int dnx_data_iqs_credit_hungry_mult_max_get(
    int unit);


uint32 dnx_data_iqs_credit_nof_bw_levels_get(
    int unit);


uint32 dnx_data_iqs_credit_fmq_credit_fc_on_th_get(
    int unit);


uint32 dnx_data_iqs_credit_fmq_credit_fc_off_th_get(
    int unit);


uint32 dnx_data_iqs_credit_fmq_byte_fc_on_th_get(
    int unit);


uint32 dnx_data_iqs_credit_fmq_byte_fc_off_th_get(
    int unit);


uint32 dnx_data_iqs_credit_fmq_eir_credit_fc_th_get(
    int unit);


uint32 dnx_data_iqs_credit_fmq_eir_byte_fc_th_get(
    int unit);


uint32 dnx_data_iqs_credit_fmq_nof_be_classes_get(
    int unit);


uint32 dnx_data_iqs_credit_fmq_max_be_weight_get(
    int unit);


uint32 dnx_data_iqs_credit_fmq_max_burst_max_get(
    int unit);


uint32 dnx_data_iqs_credit_fmq_shp_crdt_rate_mltp_get(
    int unit);


uint32 dnx_data_iqs_credit_fmq_shp_crdt_rate_delta_get(
    int unit);


uint32 dnx_data_iqs_credit_max_rate_profile_preset_gbps_get(
    int unit);


uint32 dnx_data_iqs_credit_worth_get(
    int unit);


uint32 dnx_data_iqs_credit_fmq_shp_rate_max_get(
    int unit);



typedef enum
{

    
    _dnx_data_iqs_credit_table_nof
} dnx_data_iqs_credit_table_e;









int dnx_data_iqs_deq_default_feature_get(
    int unit,
    dnx_data_iqs_deq_default_feature_e feature);



typedef enum
{
    dnx_data_iqs_deq_default_define_low_delay_deq_bytes,
    dnx_data_iqs_deq_default_define_credit_balance_max,
    dnx_data_iqs_deq_default_define_credit_balance_resolution,
    dnx_data_iqs_deq_default_define_s2d_credit_balance_max,
    dnx_data_iqs_deq_default_define_s2d_credit_balance_resolution,
    dnx_data_iqs_deq_default_define_sram_to_fabric_credit_lfsr_thr,
    dnx_data_iqs_deq_default_define_sram_to_fabric_credit_lfsr_mask,

    
    _dnx_data_iqs_deq_default_define_nof
} dnx_data_iqs_deq_default_define_e;



uint32 dnx_data_iqs_deq_default_low_delay_deq_bytes_get(
    int unit);


uint32 dnx_data_iqs_deq_default_credit_balance_max_get(
    int unit);


uint32 dnx_data_iqs_deq_default_credit_balance_resolution_get(
    int unit);


uint32 dnx_data_iqs_deq_default_s2d_credit_balance_max_get(
    int unit);


uint32 dnx_data_iqs_deq_default_s2d_credit_balance_resolution_get(
    int unit);


uint32 dnx_data_iqs_deq_default_sram_to_fabric_credit_lfsr_thr_get(
    int unit);


uint32 dnx_data_iqs_deq_default_sram_to_fabric_credit_lfsr_mask_get(
    int unit);



typedef enum
{
    dnx_data_iqs_deq_default_table_sqm_read_weight_profiles,
    dnx_data_iqs_deq_default_table_dqm_read_weight_profiles,
    dnx_data_iqs_deq_default_table_params,
    dnx_data_iqs_deq_default_table_dram_params,
    dnx_data_iqs_deq_default_table_max_deq_cmd,
    dnx_data_iqs_deq_default_table_s2d_max_deq_cmd,

    
    _dnx_data_iqs_deq_default_table_nof
} dnx_data_iqs_deq_default_table_e;



const dnx_data_iqs_deq_default_sqm_read_weight_profiles_t * dnx_data_iqs_deq_default_sqm_read_weight_profiles_get(
    int unit,
    int profile_id);


const dnx_data_iqs_deq_default_dqm_read_weight_profiles_t * dnx_data_iqs_deq_default_dqm_read_weight_profiles_get(
    int unit,
    int profile_id);


const dnx_data_iqs_deq_default_params_t * dnx_data_iqs_deq_default_params_get(
    int unit,
    int bw_level);


const dnx_data_iqs_deq_default_dram_params_t * dnx_data_iqs_deq_default_dram_params_get(
    int unit,
    int index);


const dnx_data_iqs_deq_default_max_deq_cmd_t * dnx_data_iqs_deq_default_max_deq_cmd_get(
    int unit,
    int bw_level,
    int nof_active_queues);


const dnx_data_iqs_deq_default_s2d_max_deq_cmd_t * dnx_data_iqs_deq_default_s2d_max_deq_cmd_get(
    int unit,
    int bw_level,
    int nof_active_queues);



shr_error_e dnx_data_iqs_deq_default_sqm_read_weight_profiles_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_iqs_deq_default_dqm_read_weight_profiles_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_iqs_deq_default_params_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_iqs_deq_default_dram_params_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_iqs_deq_default_max_deq_cmd_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_iqs_deq_default_s2d_max_deq_cmd_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_iqs_deq_default_sqm_read_weight_profiles_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_iqs_deq_default_dqm_read_weight_profiles_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_iqs_deq_default_params_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_iqs_deq_default_dram_params_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_iqs_deq_default_max_deq_cmd_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_iqs_deq_default_s2d_max_deq_cmd_info_get(
    int unit);






int dnx_data_iqs_dqcq_feature_get(
    int unit,
    dnx_data_iqs_dqcq_feature_e feature);



typedef enum
{
    dnx_data_iqs_dqcq_define_max_nof_contexts,
    dnx_data_iqs_dqcq_define_nof_priorities,

    
    _dnx_data_iqs_dqcq_define_nof
} dnx_data_iqs_dqcq_define_e;



uint32 dnx_data_iqs_dqcq_max_nof_contexts_get(
    int unit);


uint32 dnx_data_iqs_dqcq_nof_priorities_get(
    int unit);



typedef enum
{

    
    _dnx_data_iqs_dqcq_table_nof
} dnx_data_iqs_dqcq_table_e;









int dnx_data_iqs_dbal_feature_get(
    int unit,
    dnx_data_iqs_dbal_feature_e feature);



typedef enum
{
    dnx_data_iqs_dbal_define_ipt_counters_nof_bits,

    
    _dnx_data_iqs_dbal_define_nof
} dnx_data_iqs_dbal_define_e;



uint32 dnx_data_iqs_dbal_ipt_counters_nof_bits_get(
    int unit);



typedef enum
{

    
    _dnx_data_iqs_dbal_table_nof
} dnx_data_iqs_dbal_table_e;









int dnx_data_iqs_flush_feature_get(
    int unit,
    dnx_data_iqs_flush_feature_e feature);



typedef enum
{
    dnx_data_iqs_flush_define_bytes_units_to_send,

    
    _dnx_data_iqs_flush_define_nof
} dnx_data_iqs_flush_define_e;



uint32 dnx_data_iqs_flush_bytes_units_to_send_get(
    int unit);



typedef enum
{

    
    _dnx_data_iqs_flush_table_nof
} dnx_data_iqs_flush_table_e;






shr_error_e dnx_data_iqs_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

