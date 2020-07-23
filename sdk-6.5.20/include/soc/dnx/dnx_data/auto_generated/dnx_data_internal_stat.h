

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_STAT_H_

#define _DNX_DATA_INTERNAL_STAT_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stat.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_stat_submodule_stat_pp,
    dnx_data_stat_submodule_diag_counter,
    dnx_data_stat_submodule_drop_reasons,

    
    _dnx_data_stat_submodule_nof
} dnx_data_stat_submodule_e;








int dnx_data_stat_stat_pp_feature_get(
    int unit,
    dnx_data_stat_stat_pp_feature_e feature);



typedef enum
{
    dnx_data_stat_stat_pp_define_max_irpp_profile_value,
    dnx_data_stat_stat_pp_define_max_etpp_profile_value,
    dnx_data_stat_stat_pp_define_max_etpp_counting_profile_value,
    dnx_data_stat_stat_pp_define_max_etpp_metering_profile_value,
    dnx_data_stat_stat_pp_define_max_erpp_profile_value,
    dnx_data_stat_stat_pp_define_etpp_reversed_stat_cmd,
    dnx_data_stat_stat_pp_define_etpp_enc_stat_cmd_is_reversed,
    dnx_data_stat_stat_pp_define_etpp_egress_vsi_always_pushed,
    dnx_data_stat_stat_pp_define_irpp_start_profile_value,
    dnx_data_stat_stat_pp_define_erpp_start_profile_value,
    dnx_data_stat_stat_pp_define_etpp_start_profile_value,
    dnx_data_stat_stat_pp_define_etpp_counting_start_profile_value,
    dnx_data_stat_stat_pp_define_etpp_metering_start_profile_value,

    
    _dnx_data_stat_stat_pp_define_nof
} dnx_data_stat_stat_pp_define_e;



uint32 dnx_data_stat_stat_pp_max_irpp_profile_value_get(
    int unit);


uint32 dnx_data_stat_stat_pp_max_etpp_profile_value_get(
    int unit);


uint32 dnx_data_stat_stat_pp_max_etpp_counting_profile_value_get(
    int unit);


uint32 dnx_data_stat_stat_pp_max_etpp_metering_profile_value_get(
    int unit);


uint32 dnx_data_stat_stat_pp_max_erpp_profile_value_get(
    int unit);


uint32 dnx_data_stat_stat_pp_etpp_reversed_stat_cmd_get(
    int unit);


uint32 dnx_data_stat_stat_pp_etpp_enc_stat_cmd_is_reversed_get(
    int unit);


uint32 dnx_data_stat_stat_pp_etpp_egress_vsi_always_pushed_get(
    int unit);


uint32 dnx_data_stat_stat_pp_irpp_start_profile_value_get(
    int unit);


uint32 dnx_data_stat_stat_pp_erpp_start_profile_value_get(
    int unit);


uint32 dnx_data_stat_stat_pp_etpp_start_profile_value_get(
    int unit);


uint32 dnx_data_stat_stat_pp_etpp_counting_start_profile_value_get(
    int unit);


uint32 dnx_data_stat_stat_pp_etpp_metering_start_profile_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_stat_stat_pp_table_nof
} dnx_data_stat_stat_pp_table_e;









int dnx_data_stat_diag_counter_feature_get(
    int unit,
    dnx_data_stat_diag_counter_feature_e feature);



typedef enum
{

    
    _dnx_data_stat_diag_counter_define_nof
} dnx_data_stat_diag_counter_define_e;




typedef enum
{
    dnx_data_stat_diag_counter_table_ovf_info,

    
    _dnx_data_stat_diag_counter_table_nof
} dnx_data_stat_diag_counter_table_e;



const dnx_data_stat_diag_counter_ovf_info_t * dnx_data_stat_diag_counter_ovf_info_get(
    int unit,
    int index);



shr_error_e dnx_data_stat_diag_counter_ovf_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_stat_diag_counter_ovf_info_info_get(
    int unit);






int dnx_data_stat_drop_reasons_feature_get(
    int unit,
    dnx_data_stat_drop_reasons_feature_e feature);



typedef enum
{
    dnx_data_stat_drop_reasons_define_nof_drop_reasons,

    
    _dnx_data_stat_drop_reasons_define_nof
} dnx_data_stat_drop_reasons_define_e;



uint32 dnx_data_stat_drop_reasons_nof_drop_reasons_get(
    int unit);



typedef enum
{
    dnx_data_stat_drop_reasons_table_group_drop_reason_index,
    dnx_data_stat_drop_reasons_table_drop_reason_index,
    dnx_data_stat_drop_reasons_table_drop_reasons_groups,

    
    _dnx_data_stat_drop_reasons_table_nof
} dnx_data_stat_drop_reasons_table_e;



const dnx_data_stat_drop_reasons_group_drop_reason_index_t * dnx_data_stat_drop_reasons_group_drop_reason_index_get(
    int unit,
    int group);


const dnx_data_stat_drop_reasons_drop_reason_index_t * dnx_data_stat_drop_reasons_drop_reason_index_get(
    int unit,
    int drop_reason_bit);


const dnx_data_stat_drop_reasons_drop_reasons_groups_t * dnx_data_stat_drop_reasons_drop_reasons_groups_get(
    int unit,
    int name);



shr_error_e dnx_data_stat_drop_reasons_group_drop_reason_index_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_stat_drop_reasons_drop_reason_index_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_stat_drop_reasons_drop_reasons_groups_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_stat_drop_reasons_group_drop_reason_index_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_stat_drop_reasons_drop_reason_index_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_stat_drop_reasons_drop_reasons_groups_info_get(
    int unit);



shr_error_e dnx_data_stat_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

