

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_LIF_H_

#define _DNX_DATA_INTERNAL_LIF_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_lif_submodule_global_lif,
    dnx_data_lif_submodule_in_lif,
    dnx_data_lif_submodule_out_lif,
    dnx_data_lif_submodule_lif_table_manager,
    dnx_data_lif_submodule_feature,

    
    _dnx_data_lif_submodule_nof
} dnx_data_lif_submodule_e;








int dnx_data_lif_global_lif_feature_get(
    int unit,
    dnx_data_lif_global_lif_feature_e feature);



typedef enum
{
    dnx_data_lif_global_lif_define_use_mdb_size,
    dnx_data_lif_global_lif_define_glem_rif_optimization_enabled,
    dnx_data_lif_global_lif_define_null_lif,
    dnx_data_lif_global_lif_define_nof_global_in_lifs,
    dnx_data_lif_global_lif_define_nof_global_out_lifs,
    dnx_data_lif_global_lif_define_egress_in_lif_null_value,
    dnx_data_lif_global_lif_define_egress_out_lif_null_value,
    dnx_data_lif_global_lif_define_nof_global_l2_gport_lifs,

    
    _dnx_data_lif_global_lif_define_nof
} dnx_data_lif_global_lif_define_e;



uint32 dnx_data_lif_global_lif_use_mdb_size_get(
    int unit);


uint32 dnx_data_lif_global_lif_glem_rif_optimization_enabled_get(
    int unit);


uint32 dnx_data_lif_global_lif_null_lif_get(
    int unit);


uint32 dnx_data_lif_global_lif_nof_global_in_lifs_get(
    int unit);


uint32 dnx_data_lif_global_lif_nof_global_out_lifs_get(
    int unit);


uint32 dnx_data_lif_global_lif_egress_in_lif_null_value_get(
    int unit);


uint32 dnx_data_lif_global_lif_egress_out_lif_null_value_get(
    int unit);


uint32 dnx_data_lif_global_lif_nof_global_l2_gport_lifs_get(
    int unit);



typedef enum
{

    
    _dnx_data_lif_global_lif_table_nof
} dnx_data_lif_global_lif_table_e;









int dnx_data_lif_in_lif_feature_get(
    int unit,
    dnx_data_lif_in_lif_feature_e feature);



typedef enum
{
    dnx_data_lif_in_lif_define_inlif_index_bit_size,
    dnx_data_lif_in_lif_define_inlif_minimum_index_size,
    dnx_data_lif_in_lif_define_inlif_resource_tag_size,
    dnx_data_lif_in_lif_define_nof_in_lif_profiles,
    dnx_data_lif_in_lif_define_nof_used_in_lif_profile_bits,
    dnx_data_lif_in_lif_define_nof_used_eth_rif_profile_bits,
    dnx_data_lif_in_lif_define_nof_in_lif_incoming_orientation,
    dnx_data_lif_in_lif_define_nof_eth_rif_profiles,
    dnx_data_lif_in_lif_define_nof_da_not_found_destination_profiles,
    dnx_data_lif_in_lif_define_default_da_not_found_destination_profile,
    dnx_data_lif_in_lif_define_drop_in_lif,
    dnx_data_lif_in_lif_define_mc_bridge_fallback_per_lif_profile,
    dnx_data_lif_in_lif_define_in_lif_profile_allocate_indexed_mode,
    dnx_data_lif_in_lif_define_in_lif_profile_allocate_orientation,
    dnx_data_lif_in_lif_define_in_lif_profile_allocate_policer_inner_dp,
    dnx_data_lif_in_lif_define_in_lif_profile_allocate_policer_outer_dp,
    dnx_data_lif_in_lif_define_in_lif_profile_allocate_same_interface_mode,

    
    _dnx_data_lif_in_lif_define_nof
} dnx_data_lif_in_lif_define_e;



uint32 dnx_data_lif_in_lif_inlif_index_bit_size_get(
    int unit);


uint32 dnx_data_lif_in_lif_inlif_minimum_index_size_get(
    int unit);


uint32 dnx_data_lif_in_lif_inlif_resource_tag_size_get(
    int unit);


uint32 dnx_data_lif_in_lif_nof_in_lif_profiles_get(
    int unit);


uint32 dnx_data_lif_in_lif_nof_used_in_lif_profile_bits_get(
    int unit);


uint32 dnx_data_lif_in_lif_nof_used_eth_rif_profile_bits_get(
    int unit);


uint32 dnx_data_lif_in_lif_nof_in_lif_incoming_orientation_get(
    int unit);


uint32 dnx_data_lif_in_lif_nof_eth_rif_profiles_get(
    int unit);


uint32 dnx_data_lif_in_lif_nof_da_not_found_destination_profiles_get(
    int unit);


uint32 dnx_data_lif_in_lif_default_da_not_found_destination_profile_get(
    int unit);


uint32 dnx_data_lif_in_lif_drop_in_lif_get(
    int unit);


uint32 dnx_data_lif_in_lif_mc_bridge_fallback_per_lif_profile_get(
    int unit);


uint32 dnx_data_lif_in_lif_in_lif_profile_allocate_indexed_mode_get(
    int unit);


uint32 dnx_data_lif_in_lif_in_lif_profile_allocate_orientation_get(
    int unit);


uint32 dnx_data_lif_in_lif_in_lif_profile_allocate_policer_inner_dp_get(
    int unit);


uint32 dnx_data_lif_in_lif_in_lif_profile_allocate_policer_outer_dp_get(
    int unit);


uint32 dnx_data_lif_in_lif_in_lif_profile_allocate_same_interface_mode_get(
    int unit);



typedef enum
{

    
    _dnx_data_lif_in_lif_table_nof
} dnx_data_lif_in_lif_table_e;









int dnx_data_lif_out_lif_feature_get(
    int unit,
    dnx_data_lif_out_lif_feature_e feature);



typedef enum
{
    dnx_data_lif_out_lif_define_nof_out_lif_profiles,
    dnx_data_lif_out_lif_define_nof_erpp_out_lif_profiles,
    dnx_data_lif_out_lif_define_nof_out_lif_outgoing_orientation,
    dnx_data_lif_out_lif_define_outrif_profile_width,
    dnx_data_lif_out_lif_define_glem_result,
    dnx_data_lif_out_lif_define_physical_bank_pointer_size,
    dnx_data_lif_out_lif_define_outlif_eedb_banks_pointer_size,
    dnx_data_lif_out_lif_define_outlif_pointer_size,
    dnx_data_lif_out_lif_define_allocation_bank_size,
    dnx_data_lif_out_lif_define_local_outlif_width,
    dnx_data_lif_out_lif_define_outlif_profile_width,
    dnx_data_lif_out_lif_define_nof_local_out_lifs,
    dnx_data_lif_out_lif_define_physical_bank_size,

    
    _dnx_data_lif_out_lif_define_nof
} dnx_data_lif_out_lif_define_e;



uint32 dnx_data_lif_out_lif_nof_out_lif_profiles_get(
    int unit);


uint32 dnx_data_lif_out_lif_nof_erpp_out_lif_profiles_get(
    int unit);


uint32 dnx_data_lif_out_lif_nof_out_lif_outgoing_orientation_get(
    int unit);


uint32 dnx_data_lif_out_lif_outrif_profile_width_get(
    int unit);


uint32 dnx_data_lif_out_lif_glem_result_get(
    int unit);


uint32 dnx_data_lif_out_lif_physical_bank_pointer_size_get(
    int unit);


uint32 dnx_data_lif_out_lif_outlif_eedb_banks_pointer_size_get(
    int unit);


uint32 dnx_data_lif_out_lif_outlif_pointer_size_get(
    int unit);


uint32 dnx_data_lif_out_lif_allocation_bank_size_get(
    int unit);


uint32 dnx_data_lif_out_lif_local_outlif_width_get(
    int unit);


uint32 dnx_data_lif_out_lif_outlif_profile_width_get(
    int unit);


uint32 dnx_data_lif_out_lif_nof_local_out_lifs_get(
    int unit);


uint32 dnx_data_lif_out_lif_physical_bank_size_get(
    int unit);



typedef enum
{
    dnx_data_lif_out_lif_table_logical_to_physical_phase_map,

    
    _dnx_data_lif_out_lif_table_nof
} dnx_data_lif_out_lif_table_e;



const dnx_data_lif_out_lif_logical_to_physical_phase_map_t * dnx_data_lif_out_lif_logical_to_physical_phase_map_get(
    int unit,
    int logical_phase);



shr_error_e dnx_data_lif_out_lif_logical_to_physical_phase_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_lif_out_lif_logical_to_physical_phase_map_info_get(
    int unit);






int dnx_data_lif_lif_table_manager_feature_get(
    int unit,
    dnx_data_lif_lif_table_manager_feature_e feature);



typedef enum
{
    dnx_data_lif_lif_table_manager_define_max_nof_lif_tables,
    dnx_data_lif_lif_table_manager_define_max_nof_result_types,
    dnx_data_lif_lif_table_manager_define_max_fields_per_result_type,
    dnx_data_lif_lif_table_manager_define_max_fields_per_table,

    
    _dnx_data_lif_lif_table_manager_define_nof
} dnx_data_lif_lif_table_manager_define_e;



uint32 dnx_data_lif_lif_table_manager_max_nof_lif_tables_get(
    int unit);


uint32 dnx_data_lif_lif_table_manager_max_nof_result_types_get(
    int unit);


uint32 dnx_data_lif_lif_table_manager_max_fields_per_result_type_get(
    int unit);


uint32 dnx_data_lif_lif_table_manager_max_fields_per_table_get(
    int unit);



typedef enum
{

    
    _dnx_data_lif_lif_table_manager_table_nof
} dnx_data_lif_lif_table_manager_table_e;









int dnx_data_lif_feature_feature_get(
    int unit,
    dnx_data_lif_feature_feature_e feature);



typedef enum
{

    
    _dnx_data_lif_feature_define_nof
} dnx_data_lif_feature_define_e;




typedef enum
{

    
    _dnx_data_lif_feature_table_nof
} dnx_data_lif_feature_table_e;






shr_error_e dnx_data_lif_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

