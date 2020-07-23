

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_TRUNK_H_

#define _DNX_DATA_TRUNK_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_trunk.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_trunk_init(
    int unit);






typedef struct
{
    
    uint32 max_nof_members_in_group;
    
    uint32 max_nof_groups_in_pool;
    
    int pool_hw_mode;
} dnx_data_trunk_parameters_pool_info_t;



typedef enum
{

    
    _dnx_data_trunk_parameters_feature_nof
} dnx_data_trunk_parameters_feature_e;



typedef int(
    *dnx_data_trunk_parameters_feature_get_f) (
    int unit,
    dnx_data_trunk_parameters_feature_e feature);


typedef uint32(
    *dnx_data_trunk_parameters_nof_pools_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trunk_parameters_max_nof_members_in_pool_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trunk_parameters_max_nof_groups_in_pool_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trunk_parameters_spa_pool_shift_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trunk_parameters_spa_type_shift_get_f) (
    int unit);


typedef const dnx_data_trunk_parameters_pool_info_t *(
    *dnx_data_trunk_parameters_pool_info_get_f) (
    int unit,
    int pool_index);



typedef struct
{
    
    dnx_data_trunk_parameters_feature_get_f feature_get;
    
    dnx_data_trunk_parameters_nof_pools_get_f nof_pools_get;
    
    dnx_data_trunk_parameters_max_nof_members_in_pool_get_f max_nof_members_in_pool_get;
    
    dnx_data_trunk_parameters_max_nof_groups_in_pool_get_f max_nof_groups_in_pool_get;
    
    dnx_data_trunk_parameters_spa_pool_shift_get_f spa_pool_shift_get;
    
    dnx_data_trunk_parameters_spa_type_shift_get_f spa_type_shift_get;
    
    dnx_data_trunk_parameters_pool_info_get_f pool_info_get;
    
    dnxc_data_table_info_get_f pool_info_info_get;
} dnx_data_if_trunk_parameters_t;







typedef enum
{
    
    dnx_data_trunk_psc_multiply_and_divide,
    
    dnx_data_trunk_psc_smooth_division,
    
    dnx_data_trunk_psc_consistant_hashing,

    
    _dnx_data_trunk_psc_feature_nof
} dnx_data_trunk_psc_feature_e;



typedef int(
    *dnx_data_trunk_psc_feature_get_f) (
    int unit,
    dnx_data_trunk_psc_feature_e feature);


typedef uint32(
    *dnx_data_trunk_psc_consistant_hashing_small_group_size_in_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trunk_psc_smooth_division_max_nof_member_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trunk_psc_smooth_division_entries_per_profile_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_trunk_psc_feature_get_f feature_get;
    
    dnx_data_trunk_psc_consistant_hashing_small_group_size_in_bits_get_f consistant_hashing_small_group_size_in_bits_get;
    
    dnx_data_trunk_psc_smooth_division_max_nof_member_get_f smooth_division_max_nof_member_get;
    
    dnx_data_trunk_psc_smooth_division_entries_per_profile_get_f smooth_division_entries_per_profile_get;
} dnx_data_if_trunk_psc_t;







typedef enum
{
    
    dnx_data_trunk_egress_trunk_multiple_egress_trunk_sizes,

    
    _dnx_data_trunk_egress_trunk_feature_nof
} dnx_data_trunk_egress_trunk_feature_e;



typedef int(
    *dnx_data_trunk_egress_trunk_feature_get_f) (
    int unit,
    dnx_data_trunk_egress_trunk_feature_e feature);


typedef uint32(
    *dnx_data_trunk_egress_trunk_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trunk_egress_trunk_nof_lb_keys_per_profile_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trunk_egress_trunk_dbal_egress_trunk_index_multiplier_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trunk_egress_trunk_dbal_entry_index_divider_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trunk_egress_trunk_dbal_entry_index_msb_multiplier_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trunk_egress_trunk_size_mode_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trunk_egress_trunk_invalid_pp_dsp_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_trunk_egress_trunk_feature_get_f feature_get;
    
    dnx_data_trunk_egress_trunk_nof_get_f nof_get;
    
    dnx_data_trunk_egress_trunk_nof_lb_keys_per_profile_get_f nof_lb_keys_per_profile_get;
    
    dnx_data_trunk_egress_trunk_dbal_egress_trunk_index_multiplier_get_f dbal_egress_trunk_index_multiplier_get;
    
    dnx_data_trunk_egress_trunk_dbal_entry_index_divider_get_f dbal_entry_index_divider_get;
    
    dnx_data_trunk_egress_trunk_dbal_entry_index_msb_multiplier_get_f dbal_entry_index_msb_multiplier_get;
    
    dnx_data_trunk_egress_trunk_size_mode_get_f size_mode_get;
    
    dnx_data_trunk_egress_trunk_invalid_pp_dsp_get_f invalid_pp_dsp_get;
} dnx_data_if_trunk_egress_trunk_t;







typedef enum
{
    
    dnx_data_trunk_graceful_allow,

    
    _dnx_data_trunk_graceful_feature_nof
} dnx_data_trunk_graceful_feature_e;



typedef int(
    *dnx_data_trunk_graceful_feature_get_f) (
    int unit,
    dnx_data_trunk_graceful_feature_e feature);


typedef uint32(
    *dnx_data_trunk_graceful_first_hw_configuration_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trunk_graceful_second_hw_configuration_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_trunk_graceful_feature_get_f feature_get;
    
    dnx_data_trunk_graceful_first_hw_configuration_get_f first_hw_configuration_get;
    
    dnx_data_trunk_graceful_second_hw_configuration_get_f second_hw_configuration_get;
} dnx_data_if_trunk_graceful_t;





typedef struct
{
    
    dnx_data_if_trunk_parameters_t parameters;
    
    dnx_data_if_trunk_psc_t psc;
    
    dnx_data_if_trunk_egress_trunk_t egress_trunk;
    
    dnx_data_if_trunk_graceful_t graceful;
} dnx_data_if_trunk_t;




extern dnx_data_if_trunk_t dnx_data_trunk;


#endif 

