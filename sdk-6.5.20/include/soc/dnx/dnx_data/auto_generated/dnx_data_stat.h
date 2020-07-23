

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_STAT_H_

#define _DNX_DATA_STAT_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/mcm/allenum.h>
#include <bcm/stat.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_stat.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_stat_init(
    int unit);







typedef enum
{
    
    dnx_data_stat_stat_pp_full_stat_support,

    
    _dnx_data_stat_stat_pp_feature_nof
} dnx_data_stat_stat_pp_feature_e;



typedef int(
    *dnx_data_stat_stat_pp_feature_get_f) (
    int unit,
    dnx_data_stat_stat_pp_feature_e feature);


typedef uint32(
    *dnx_data_stat_stat_pp_max_irpp_profile_value_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stat_stat_pp_max_etpp_profile_value_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stat_stat_pp_max_etpp_counting_profile_value_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stat_stat_pp_max_etpp_metering_profile_value_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stat_stat_pp_max_erpp_profile_value_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stat_stat_pp_etpp_reversed_stat_cmd_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stat_stat_pp_etpp_enc_stat_cmd_is_reversed_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stat_stat_pp_etpp_egress_vsi_always_pushed_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stat_stat_pp_irpp_start_profile_value_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stat_stat_pp_erpp_start_profile_value_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stat_stat_pp_etpp_start_profile_value_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stat_stat_pp_etpp_counting_start_profile_value_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stat_stat_pp_etpp_metering_start_profile_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_stat_stat_pp_feature_get_f feature_get;
    
    dnx_data_stat_stat_pp_max_irpp_profile_value_get_f max_irpp_profile_value_get;
    
    dnx_data_stat_stat_pp_max_etpp_profile_value_get_f max_etpp_profile_value_get;
    
    dnx_data_stat_stat_pp_max_etpp_counting_profile_value_get_f max_etpp_counting_profile_value_get;
    
    dnx_data_stat_stat_pp_max_etpp_metering_profile_value_get_f max_etpp_metering_profile_value_get;
    
    dnx_data_stat_stat_pp_max_erpp_profile_value_get_f max_erpp_profile_value_get;
    
    dnx_data_stat_stat_pp_etpp_reversed_stat_cmd_get_f etpp_reversed_stat_cmd_get;
    
    dnx_data_stat_stat_pp_etpp_enc_stat_cmd_is_reversed_get_f etpp_enc_stat_cmd_is_reversed_get;
    
    dnx_data_stat_stat_pp_etpp_egress_vsi_always_pushed_get_f etpp_egress_vsi_always_pushed_get;
    
    dnx_data_stat_stat_pp_irpp_start_profile_value_get_f irpp_start_profile_value_get;
    
    dnx_data_stat_stat_pp_erpp_start_profile_value_get_f erpp_start_profile_value_get;
    
    dnx_data_stat_stat_pp_etpp_start_profile_value_get_f etpp_start_profile_value_get;
    
    dnx_data_stat_stat_pp_etpp_counting_start_profile_value_get_f etpp_counting_start_profile_value_get;
    
    dnx_data_stat_stat_pp_etpp_metering_start_profile_value_get_f etpp_metering_start_profile_value_get;
} dnx_data_if_stat_stat_pp_t;






typedef struct
{
    
    soc_reg_t reg;
    
    soc_field_t cnt_field;
    
    soc_field_t ovf_field;
} dnx_data_stat_diag_counter_ovf_info_t;



typedef enum
{

    
    _dnx_data_stat_diag_counter_feature_nof
} dnx_data_stat_diag_counter_feature_e;



typedef int(
    *dnx_data_stat_diag_counter_feature_get_f) (
    int unit,
    dnx_data_stat_diag_counter_feature_e feature);


typedef const dnx_data_stat_diag_counter_ovf_info_t *(
    *dnx_data_stat_diag_counter_ovf_info_get_f) (
    int unit,
    int index);



typedef struct
{
    
    dnx_data_stat_diag_counter_feature_get_f feature_get;
    
    dnx_data_stat_diag_counter_ovf_info_get_f ovf_info_get;
    
    dnxc_data_table_info_get_f ovf_info_info_get;
} dnx_data_if_stat_diag_counter_t;






typedef struct
{
    
    uint32 index;
} dnx_data_stat_drop_reasons_group_drop_reason_index_t;


typedef struct
{
    
    uint32 index;
} dnx_data_stat_drop_reasons_drop_reason_index_t;


typedef struct
{
    
    bcm_cosq_drop_reason_t drop_reasons[DNX_DATA_MAX_STAT_DROP_REASONS_NOF_DROP_REASONS];
} dnx_data_stat_drop_reasons_drop_reasons_groups_t;



typedef enum
{

    
    _dnx_data_stat_drop_reasons_feature_nof
} dnx_data_stat_drop_reasons_feature_e;



typedef int(
    *dnx_data_stat_drop_reasons_feature_get_f) (
    int unit,
    dnx_data_stat_drop_reasons_feature_e feature);


typedef uint32(
    *dnx_data_stat_drop_reasons_nof_drop_reasons_get_f) (
    int unit);


typedef const dnx_data_stat_drop_reasons_group_drop_reason_index_t *(
    *dnx_data_stat_drop_reasons_group_drop_reason_index_get_f) (
    int unit,
    int group);


typedef const dnx_data_stat_drop_reasons_drop_reason_index_t *(
    *dnx_data_stat_drop_reasons_drop_reason_index_get_f) (
    int unit,
    int drop_reason_bit);


typedef const dnx_data_stat_drop_reasons_drop_reasons_groups_t *(
    *dnx_data_stat_drop_reasons_drop_reasons_groups_get_f) (
    int unit,
    int name);



typedef struct
{
    
    dnx_data_stat_drop_reasons_feature_get_f feature_get;
    
    dnx_data_stat_drop_reasons_nof_drop_reasons_get_f nof_drop_reasons_get;
    
    dnx_data_stat_drop_reasons_group_drop_reason_index_get_f group_drop_reason_index_get;
    
    dnxc_data_table_info_get_f group_drop_reason_index_info_get;
    
    dnx_data_stat_drop_reasons_drop_reason_index_get_f drop_reason_index_get;
    
    dnxc_data_table_info_get_f drop_reason_index_info_get;
    
    dnx_data_stat_drop_reasons_drop_reasons_groups_get_f drop_reasons_groups_get;
    
    dnxc_data_table_info_get_f drop_reasons_groups_info_get;
} dnx_data_if_stat_drop_reasons_t;





typedef struct
{
    
    dnx_data_if_stat_stat_pp_t stat_pp;
    
    dnx_data_if_stat_diag_counter_t diag_counter;
    
    dnx_data_if_stat_drop_reasons_t drop_reasons;
} dnx_data_if_stat_t;




extern dnx_data_if_stat_t dnx_data_stat;


#endif 

