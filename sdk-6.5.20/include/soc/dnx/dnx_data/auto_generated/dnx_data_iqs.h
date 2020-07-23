

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_IQS_H_

#define _DNX_DATA_IQS_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <bcm_int/dnx/cosq/ingress/iqs.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_iqs.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_iqs_init(
    int unit);







typedef enum
{
    dnx_data_iqs_credit_watchdog_fixed_period,
    
    dnx_data_iqs_credit_fmq_shp_zero_rate_allowed,
    dnx_data_iqs_credit_dram_delete_fix_hw,
    
    dnx_data_iqs_credit_credit_balance_support,

    
    _dnx_data_iqs_credit_feature_nof
} dnx_data_iqs_credit_feature_e;



typedef int(
    *dnx_data_iqs_credit_feature_get_f) (
    int unit,
    dnx_data_iqs_credit_feature_e feature);


typedef uint32(
    *dnx_data_iqs_credit_nof_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_iqs_credit_max_credit_balance_threshold_get_f) (
    int unit);


typedef uint32(
    *dnx_data_iqs_credit_max_worth_get_f) (
    int unit);


typedef int(
    *dnx_data_iqs_credit_satisified_back_min_get_f) (
    int unit);


typedef int(
    *dnx_data_iqs_credit_satisified_back_max_get_f) (
    int unit);


typedef int(
    *dnx_data_iqs_credit_satisified_empty_min_get_f) (
    int unit);


typedef int(
    *dnx_data_iqs_credit_satisified_empty_max_get_f) (
    int unit);


typedef int(
    *dnx_data_iqs_credit_hungry_min_get_f) (
    int unit);


typedef int(
    *dnx_data_iqs_credit_hungry_max_get_f) (
    int unit);


typedef int(
    *dnx_data_iqs_credit_hungry_mult_min_get_f) (
    int unit);


typedef int(
    *dnx_data_iqs_credit_hungry_mult_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_iqs_credit_nof_bw_levels_get_f) (
    int unit);


typedef uint32(
    *dnx_data_iqs_credit_fmq_credit_fc_on_th_get_f) (
    int unit);


typedef uint32(
    *dnx_data_iqs_credit_fmq_credit_fc_off_th_get_f) (
    int unit);


typedef uint32(
    *dnx_data_iqs_credit_fmq_byte_fc_on_th_get_f) (
    int unit);


typedef uint32(
    *dnx_data_iqs_credit_fmq_byte_fc_off_th_get_f) (
    int unit);


typedef uint32(
    *dnx_data_iqs_credit_fmq_eir_credit_fc_th_get_f) (
    int unit);


typedef uint32(
    *dnx_data_iqs_credit_fmq_eir_byte_fc_th_get_f) (
    int unit);


typedef uint32(
    *dnx_data_iqs_credit_fmq_nof_be_classes_get_f) (
    int unit);


typedef uint32(
    *dnx_data_iqs_credit_fmq_max_be_weight_get_f) (
    int unit);


typedef uint32(
    *dnx_data_iqs_credit_fmq_max_burst_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_iqs_credit_fmq_shp_crdt_rate_mltp_get_f) (
    int unit);


typedef uint32(
    *dnx_data_iqs_credit_fmq_shp_crdt_rate_delta_get_f) (
    int unit);


typedef uint32(
    *dnx_data_iqs_credit_max_rate_profile_preset_gbps_get_f) (
    int unit);


typedef uint32(
    *dnx_data_iqs_credit_worth_get_f) (
    int unit);


typedef uint32(
    *dnx_data_iqs_credit_fmq_shp_rate_max_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_iqs_credit_feature_get_f feature_get;
    
    dnx_data_iqs_credit_nof_profiles_get_f nof_profiles_get;
    
    dnx_data_iqs_credit_max_credit_balance_threshold_get_f max_credit_balance_threshold_get;
    
    dnx_data_iqs_credit_max_worth_get_f max_worth_get;
    
    dnx_data_iqs_credit_satisified_back_min_get_f satisified_back_min_get;
    
    dnx_data_iqs_credit_satisified_back_max_get_f satisified_back_max_get;
    
    dnx_data_iqs_credit_satisified_empty_min_get_f satisified_empty_min_get;
    
    dnx_data_iqs_credit_satisified_empty_max_get_f satisified_empty_max_get;
    
    dnx_data_iqs_credit_hungry_min_get_f hungry_min_get;
    
    dnx_data_iqs_credit_hungry_max_get_f hungry_max_get;
    
    dnx_data_iqs_credit_hungry_mult_min_get_f hungry_mult_min_get;
    
    dnx_data_iqs_credit_hungry_mult_max_get_f hungry_mult_max_get;
    
    dnx_data_iqs_credit_nof_bw_levels_get_f nof_bw_levels_get;
    
    dnx_data_iqs_credit_fmq_credit_fc_on_th_get_f fmq_credit_fc_on_th_get;
    
    dnx_data_iqs_credit_fmq_credit_fc_off_th_get_f fmq_credit_fc_off_th_get;
    
    dnx_data_iqs_credit_fmq_byte_fc_on_th_get_f fmq_byte_fc_on_th_get;
    
    dnx_data_iqs_credit_fmq_byte_fc_off_th_get_f fmq_byte_fc_off_th_get;
    
    dnx_data_iqs_credit_fmq_eir_credit_fc_th_get_f fmq_eir_credit_fc_th_get;
    
    dnx_data_iqs_credit_fmq_eir_byte_fc_th_get_f fmq_eir_byte_fc_th_get;
    
    dnx_data_iqs_credit_fmq_nof_be_classes_get_f fmq_nof_be_classes_get;
    
    dnx_data_iqs_credit_fmq_max_be_weight_get_f fmq_max_be_weight_get;
    
    dnx_data_iqs_credit_fmq_max_burst_max_get_f fmq_max_burst_max_get;
    
    dnx_data_iqs_credit_fmq_shp_crdt_rate_mltp_get_f fmq_shp_crdt_rate_mltp_get;
    
    dnx_data_iqs_credit_fmq_shp_crdt_rate_delta_get_f fmq_shp_crdt_rate_delta_get;
    
    dnx_data_iqs_credit_max_rate_profile_preset_gbps_get_f max_rate_profile_preset_gbps_get;
    
    dnx_data_iqs_credit_worth_get_f worth_get;
    
    dnx_data_iqs_credit_fmq_shp_rate_max_get_f fmq_shp_rate_max_get;
} dnx_data_if_iqs_credit_t;






typedef struct
{
    
    uint32 weight;
} dnx_data_iqs_deq_default_sqm_read_weight_profiles_t;


typedef struct
{
    
    uint32 weight;
} dnx_data_iqs_deq_default_dqm_read_weight_profiles_t;


typedef struct
{
    
    uint32 sram_read_weight_profile_val[DNX_IQS_DEQ_PARAM_TYPE_NOF];
    
    uint32 s2d_read_weight_profile_val[DNX_IQS_DEQ_PARAM_TYPE_NOF];
    
    uint32 sram_extra_credits_val[DNX_IQS_DEQ_PARAM_TYPE_NOF];
    
    uint32 s2d_extra_credits_val[DNX_IQS_DEQ_PARAM_TYPE_NOF];
    
    uint32 sram_extra_credits_lfsr_val[DNX_IQS_DEQ_PARAM_TYPE_NOF];
    
    uint32 s2d_extra_credits_lfsr_val[DNX_IQS_DEQ_PARAM_TYPE_NOF];
} dnx_data_iqs_deq_default_params_t;


typedef struct
{
    
    uint32 read_weight_profile_val;
    
    uint32 read_weight_profile_th;
} dnx_data_iqs_deq_default_dram_params_t;


typedef struct
{
    
    uint32 value;
} dnx_data_iqs_deq_default_max_deq_cmd_t;


typedef struct
{
    
    uint32 value;
} dnx_data_iqs_deq_default_s2d_max_deq_cmd_t;



typedef enum
{
    
    dnx_data_iqs_deq_default_dqcq_fc_to_dram,

    
    _dnx_data_iqs_deq_default_feature_nof
} dnx_data_iqs_deq_default_feature_e;



typedef int(
    *dnx_data_iqs_deq_default_feature_get_f) (
    int unit,
    dnx_data_iqs_deq_default_feature_e feature);


typedef uint32(
    *dnx_data_iqs_deq_default_low_delay_deq_bytes_get_f) (
    int unit);


typedef uint32(
    *dnx_data_iqs_deq_default_credit_balance_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_iqs_deq_default_credit_balance_resolution_get_f) (
    int unit);


typedef uint32(
    *dnx_data_iqs_deq_default_s2d_credit_balance_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_iqs_deq_default_s2d_credit_balance_resolution_get_f) (
    int unit);


typedef uint32(
    *dnx_data_iqs_deq_default_sram_to_fabric_credit_lfsr_thr_get_f) (
    int unit);


typedef uint32(
    *dnx_data_iqs_deq_default_sram_to_fabric_credit_lfsr_mask_get_f) (
    int unit);


typedef const dnx_data_iqs_deq_default_sqm_read_weight_profiles_t *(
    *dnx_data_iqs_deq_default_sqm_read_weight_profiles_get_f) (
    int unit,
    int profile_id);


typedef const dnx_data_iqs_deq_default_dqm_read_weight_profiles_t *(
    *dnx_data_iqs_deq_default_dqm_read_weight_profiles_get_f) (
    int unit,
    int profile_id);


typedef const dnx_data_iqs_deq_default_params_t *(
    *dnx_data_iqs_deq_default_params_get_f) (
    int unit,
    int bw_level);


typedef const dnx_data_iqs_deq_default_dram_params_t *(
    *dnx_data_iqs_deq_default_dram_params_get_f) (
    int unit,
    int index);


typedef const dnx_data_iqs_deq_default_max_deq_cmd_t *(
    *dnx_data_iqs_deq_default_max_deq_cmd_get_f) (
    int unit,
    int bw_level,
    int nof_active_queues);


typedef const dnx_data_iqs_deq_default_s2d_max_deq_cmd_t *(
    *dnx_data_iqs_deq_default_s2d_max_deq_cmd_get_f) (
    int unit,
    int bw_level,
    int nof_active_queues);



typedef struct
{
    
    dnx_data_iqs_deq_default_feature_get_f feature_get;
    
    dnx_data_iqs_deq_default_low_delay_deq_bytes_get_f low_delay_deq_bytes_get;
    
    dnx_data_iqs_deq_default_credit_balance_max_get_f credit_balance_max_get;
    
    dnx_data_iqs_deq_default_credit_balance_resolution_get_f credit_balance_resolution_get;
    
    dnx_data_iqs_deq_default_s2d_credit_balance_max_get_f s2d_credit_balance_max_get;
    
    dnx_data_iqs_deq_default_s2d_credit_balance_resolution_get_f s2d_credit_balance_resolution_get;
    
    dnx_data_iqs_deq_default_sram_to_fabric_credit_lfsr_thr_get_f sram_to_fabric_credit_lfsr_thr_get;
    
    dnx_data_iqs_deq_default_sram_to_fabric_credit_lfsr_mask_get_f sram_to_fabric_credit_lfsr_mask_get;
    
    dnx_data_iqs_deq_default_sqm_read_weight_profiles_get_f sqm_read_weight_profiles_get;
    
    dnxc_data_table_info_get_f sqm_read_weight_profiles_info_get;
    
    dnx_data_iqs_deq_default_dqm_read_weight_profiles_get_f dqm_read_weight_profiles_get;
    
    dnxc_data_table_info_get_f dqm_read_weight_profiles_info_get;
    
    dnx_data_iqs_deq_default_params_get_f params_get;
    
    dnxc_data_table_info_get_f params_info_get;
    
    dnx_data_iqs_deq_default_dram_params_get_f dram_params_get;
    
    dnxc_data_table_info_get_f dram_params_info_get;
    
    dnx_data_iqs_deq_default_max_deq_cmd_get_f max_deq_cmd_get;
    
    dnxc_data_table_info_get_f max_deq_cmd_info_get;
    
    dnx_data_iqs_deq_default_s2d_max_deq_cmd_get_f s2d_max_deq_cmd_get;
    
    dnxc_data_table_info_get_f s2d_max_deq_cmd_info_get;
} dnx_data_if_iqs_deq_default_t;







typedef enum
{
    
    dnx_data_iqs_dqcq_8_priorities,

    
    _dnx_data_iqs_dqcq_feature_nof
} dnx_data_iqs_dqcq_feature_e;



typedef int(
    *dnx_data_iqs_dqcq_feature_get_f) (
    int unit,
    dnx_data_iqs_dqcq_feature_e feature);


typedef uint32(
    *dnx_data_iqs_dqcq_max_nof_contexts_get_f) (
    int unit);


typedef uint32(
    *dnx_data_iqs_dqcq_nof_priorities_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_iqs_dqcq_feature_get_f feature_get;
    
    dnx_data_iqs_dqcq_max_nof_contexts_get_f max_nof_contexts_get;
    
    dnx_data_iqs_dqcq_nof_priorities_get_f nof_priorities_get;
} dnx_data_if_iqs_dqcq_t;







typedef enum
{

    
    _dnx_data_iqs_dbal_feature_nof
} dnx_data_iqs_dbal_feature_e;



typedef int(
    *dnx_data_iqs_dbal_feature_get_f) (
    int unit,
    dnx_data_iqs_dbal_feature_e feature);


typedef uint32(
    *dnx_data_iqs_dbal_ipt_counters_nof_bits_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_iqs_dbal_feature_get_f feature_get;
    
    dnx_data_iqs_dbal_ipt_counters_nof_bits_get_f ipt_counters_nof_bits_get;
} dnx_data_if_iqs_dbal_t;







typedef enum
{

    
    _dnx_data_iqs_flush_feature_nof
} dnx_data_iqs_flush_feature_e;



typedef int(
    *dnx_data_iqs_flush_feature_get_f) (
    int unit,
    dnx_data_iqs_flush_feature_e feature);


typedef uint32(
    *dnx_data_iqs_flush_bytes_units_to_send_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_iqs_flush_feature_get_f feature_get;
    
    dnx_data_iqs_flush_bytes_units_to_send_get_f bytes_units_to_send_get;
} dnx_data_if_iqs_flush_t;





typedef struct
{
    
    dnx_data_if_iqs_credit_t credit;
    
    dnx_data_if_iqs_deq_default_t deq_default;
    
    dnx_data_if_iqs_dqcq_t dqcq;
    
    dnx_data_if_iqs_dbal_t dbal;
    
    dnx_data_if_iqs_flush_t flush;
} dnx_data_if_iqs_t;




extern dnx_data_if_iqs_t dnx_data_iqs;


#endif 

