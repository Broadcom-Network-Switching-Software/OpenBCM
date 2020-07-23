

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_TRUNK_H_

#define _DNX_DATA_INTERNAL_TRUNK_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trunk.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_trunk_submodule_parameters,
    dnx_data_trunk_submodule_psc,
    dnx_data_trunk_submodule_egress_trunk,
    dnx_data_trunk_submodule_graceful,

    
    _dnx_data_trunk_submodule_nof
} dnx_data_trunk_submodule_e;








int dnx_data_trunk_parameters_feature_get(
    int unit,
    dnx_data_trunk_parameters_feature_e feature);



typedef enum
{
    dnx_data_trunk_parameters_define_nof_pools,
    dnx_data_trunk_parameters_define_max_nof_members_in_pool,
    dnx_data_trunk_parameters_define_max_nof_groups_in_pool,
    dnx_data_trunk_parameters_define_spa_pool_shift,
    dnx_data_trunk_parameters_define_spa_type_shift,

    
    _dnx_data_trunk_parameters_define_nof
} dnx_data_trunk_parameters_define_e;



uint32 dnx_data_trunk_parameters_nof_pools_get(
    int unit);


uint32 dnx_data_trunk_parameters_max_nof_members_in_pool_get(
    int unit);


uint32 dnx_data_trunk_parameters_max_nof_groups_in_pool_get(
    int unit);


uint32 dnx_data_trunk_parameters_spa_pool_shift_get(
    int unit);


uint32 dnx_data_trunk_parameters_spa_type_shift_get(
    int unit);



typedef enum
{
    dnx_data_trunk_parameters_table_pool_info,

    
    _dnx_data_trunk_parameters_table_nof
} dnx_data_trunk_parameters_table_e;



const dnx_data_trunk_parameters_pool_info_t * dnx_data_trunk_parameters_pool_info_get(
    int unit,
    int pool_index);



shr_error_e dnx_data_trunk_parameters_pool_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_trunk_parameters_pool_info_info_get(
    int unit);






int dnx_data_trunk_psc_feature_get(
    int unit,
    dnx_data_trunk_psc_feature_e feature);



typedef enum
{
    dnx_data_trunk_psc_define_consistant_hashing_small_group_size_in_bits,
    dnx_data_trunk_psc_define_smooth_division_max_nof_member,
    dnx_data_trunk_psc_define_smooth_division_entries_per_profile,

    
    _dnx_data_trunk_psc_define_nof
} dnx_data_trunk_psc_define_e;



uint32 dnx_data_trunk_psc_consistant_hashing_small_group_size_in_bits_get(
    int unit);


uint32 dnx_data_trunk_psc_smooth_division_max_nof_member_get(
    int unit);


uint32 dnx_data_trunk_psc_smooth_division_entries_per_profile_get(
    int unit);



typedef enum
{

    
    _dnx_data_trunk_psc_table_nof
} dnx_data_trunk_psc_table_e;









int dnx_data_trunk_egress_trunk_feature_get(
    int unit,
    dnx_data_trunk_egress_trunk_feature_e feature);



typedef enum
{
    dnx_data_trunk_egress_trunk_define_nof,
    dnx_data_trunk_egress_trunk_define_nof_lb_keys_per_profile,
    dnx_data_trunk_egress_trunk_define_dbal_egress_trunk_index_multiplier,
    dnx_data_trunk_egress_trunk_define_dbal_entry_index_divider,
    dnx_data_trunk_egress_trunk_define_dbal_entry_index_msb_multiplier,
    dnx_data_trunk_egress_trunk_define_size_mode,
    dnx_data_trunk_egress_trunk_define_invalid_pp_dsp,

    
    _dnx_data_trunk_egress_trunk_define_nof
} dnx_data_trunk_egress_trunk_define_e;



uint32 dnx_data_trunk_egress_trunk_nof_get(
    int unit);


uint32 dnx_data_trunk_egress_trunk_nof_lb_keys_per_profile_get(
    int unit);


uint32 dnx_data_trunk_egress_trunk_dbal_egress_trunk_index_multiplier_get(
    int unit);


uint32 dnx_data_trunk_egress_trunk_dbal_entry_index_divider_get(
    int unit);


uint32 dnx_data_trunk_egress_trunk_dbal_entry_index_msb_multiplier_get(
    int unit);


uint32 dnx_data_trunk_egress_trunk_size_mode_get(
    int unit);


uint32 dnx_data_trunk_egress_trunk_invalid_pp_dsp_get(
    int unit);



typedef enum
{

    
    _dnx_data_trunk_egress_trunk_table_nof
} dnx_data_trunk_egress_trunk_table_e;









int dnx_data_trunk_graceful_feature_get(
    int unit,
    dnx_data_trunk_graceful_feature_e feature);



typedef enum
{
    dnx_data_trunk_graceful_define_first_hw_configuration,
    dnx_data_trunk_graceful_define_second_hw_configuration,

    
    _dnx_data_trunk_graceful_define_nof
} dnx_data_trunk_graceful_define_e;



uint32 dnx_data_trunk_graceful_first_hw_configuration_get(
    int unit);


uint32 dnx_data_trunk_graceful_second_hw_configuration_get(
    int unit);



typedef enum
{

    
    _dnx_data_trunk_graceful_table_nof
} dnx_data_trunk_graceful_table_e;






shr_error_e dnx_data_trunk_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

