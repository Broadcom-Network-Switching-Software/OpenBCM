
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_L3_H_

#define _DNX_DATA_INTERNAL_L3_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



typedef enum
{
    dnx_data_l3_submodule_egr_pointed,
    dnx_data_l3_submodule_fec,
    dnx_data_l3_submodule_fer,
    dnx_data_l3_submodule_source_address,
    dnx_data_l3_submodule_vrf,
    dnx_data_l3_submodule_routing_enablers,
    dnx_data_l3_submodule_rif,
    dnx_data_l3_submodule_fwd,
    dnx_data_l3_submodule_ecmp,
    dnx_data_l3_submodule_vip_ecmp,
    dnx_data_l3_submodule_wcmp,
    dnx_data_l3_submodule_vrrp,
    dnx_data_l3_submodule_feature,

    
    _dnx_data_l3_submodule_nof
} dnx_data_l3_submodule_e;








int dnx_data_l3_egr_pointed_feature_get(
    int unit,
    dnx_data_l3_egr_pointed_feature_e feature);



typedef enum
{
    dnx_data_l3_egr_pointed_define_first_egr_pointed_id,
    dnx_data_l3_egr_pointed_define_nof_egr_pointed_ids,
    dnx_data_l3_egr_pointed_define_nof_bits_in_egr_pointed_id,

    
    _dnx_data_l3_egr_pointed_define_nof
} dnx_data_l3_egr_pointed_define_e;



uint32 dnx_data_l3_egr_pointed_first_egr_pointed_id_get(
    int unit);


uint32 dnx_data_l3_egr_pointed_nof_egr_pointed_ids_get(
    int unit);


uint32 dnx_data_l3_egr_pointed_nof_bits_in_egr_pointed_id_get(
    int unit);



typedef enum
{

    
    _dnx_data_l3_egr_pointed_table_nof
} dnx_data_l3_egr_pointed_table_e;









int dnx_data_l3_fec_feature_get(
    int unit,
    dnx_data_l3_fec_feature_e feature);



typedef enum
{
    dnx_data_l3_fec_define_nof_fecs,
    dnx_data_l3_fec_define_first_valid_fec_ecmp_id,
    dnx_data_l3_fec_define_max_fec_id_for_single_dhb_cluster_pair_granularity,
    dnx_data_l3_fec_define_max_fec_id_for_double_dhb_cluster_pair_granularity,
    dnx_data_l3_fec_define_max_default_fec,
    dnx_data_l3_fec_define_bank_size,
    dnx_data_l3_fec_define_bank_size_round_up,
    dnx_data_l3_fec_define_max_nof_super_fecs_per_bank,
    dnx_data_l3_fec_define_nof_physical_fecs_per_bank,
    dnx_data_l3_fec_define_bank_nof_physical_rows,
    dnx_data_l3_fec_define_super_fec_size,
    dnx_data_l3_fec_define_max_nof_banks,
    dnx_data_l3_fec_define_first_bank_without_id_alloc,
    dnx_data_l3_fec_define_first_shared_bank,
    dnx_data_l3_fec_define_max_super_fec_id,
    dnx_data_l3_fec_define_nof_fec_directions,
    dnx_data_l3_fec_define_fec_z_max_fec_id,
    dnx_data_l3_fec_define_nof_fec_result_types,
    dnx_data_l3_fec_define_uneven_bank_sizes,
    dnx_data_l3_fec_define_fer_hw_version,
    dnx_data_l3_fec_define_fec_property_supported_flags,
    dnx_data_l3_fec_define_supported_fec_flags,
    dnx_data_l3_fec_define_supported_fec_flags2,

    
    _dnx_data_l3_fec_define_nof
} dnx_data_l3_fec_define_e;



uint32 dnx_data_l3_fec_nof_fecs_get(
    int unit);


uint32 dnx_data_l3_fec_first_valid_fec_ecmp_id_get(
    int unit);


uint32 dnx_data_l3_fec_max_fec_id_for_single_dhb_cluster_pair_granularity_get(
    int unit);


uint32 dnx_data_l3_fec_max_fec_id_for_double_dhb_cluster_pair_granularity_get(
    int unit);


uint32 dnx_data_l3_fec_max_default_fec_get(
    int unit);


uint32 dnx_data_l3_fec_bank_size_get(
    int unit);


uint32 dnx_data_l3_fec_bank_size_round_up_get(
    int unit);


uint32 dnx_data_l3_fec_max_nof_super_fecs_per_bank_get(
    int unit);


uint32 dnx_data_l3_fec_nof_physical_fecs_per_bank_get(
    int unit);


uint32 dnx_data_l3_fec_bank_nof_physical_rows_get(
    int unit);


uint32 dnx_data_l3_fec_super_fec_size_get(
    int unit);


uint32 dnx_data_l3_fec_max_nof_banks_get(
    int unit);


uint32 dnx_data_l3_fec_first_bank_without_id_alloc_get(
    int unit);


uint32 dnx_data_l3_fec_first_shared_bank_get(
    int unit);


uint32 dnx_data_l3_fec_max_super_fec_id_get(
    int unit);


uint32 dnx_data_l3_fec_nof_fec_directions_get(
    int unit);


uint32 dnx_data_l3_fec_fec_z_max_fec_id_get(
    int unit);


uint32 dnx_data_l3_fec_nof_fec_result_types_get(
    int unit);


uint32 dnx_data_l3_fec_uneven_bank_sizes_get(
    int unit);


uint32 dnx_data_l3_fec_fer_hw_version_get(
    int unit);


uint32 dnx_data_l3_fec_fec_property_supported_flags_get(
    int unit);


uint32 dnx_data_l3_fec_supported_fec_flags_get(
    int unit);


uint32 dnx_data_l3_fec_supported_fec_flags2_get(
    int unit);



typedef enum
{
    dnx_data_l3_fec_table_fec_resource_info,
    dnx_data_l3_fec_table_super_fec_result_types_map,
    dnx_data_l3_fec_table_fec_result_type_fields,
    dnx_data_l3_fec_table_fec_physical_db,
    dnx_data_l3_fec_table_fec_tables_info,
    dnx_data_l3_fec_table_fec_hierachy_position_in_range,

    
    _dnx_data_l3_fec_table_nof
} dnx_data_l3_fec_table_e;



const dnx_data_l3_fec_fec_resource_info_t * dnx_data_l3_fec_fec_resource_info_get(
    int unit,
    int resource_type);


const dnx_data_l3_fec_super_fec_result_types_map_t * dnx_data_l3_fec_super_fec_result_types_map_get(
    int unit,
    int dbal_result_type);


const dnx_data_l3_fec_fec_result_type_fields_t * dnx_data_l3_fec_fec_result_type_fields_get(
    int unit,
    int result_type);


const dnx_data_l3_fec_fec_physical_db_t * dnx_data_l3_fec_fec_physical_db_get(
    int unit);


const dnx_data_l3_fec_fec_tables_info_t * dnx_data_l3_fec_fec_tables_info_get(
    int unit,
    int mdb_table);


const dnx_data_l3_fec_fec_hierachy_position_in_range_t * dnx_data_l3_fec_fec_hierachy_position_in_range_get(
    int unit,
    int hierarchy);



shr_error_e dnx_data_l3_fec_fec_resource_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_l3_fec_super_fec_result_types_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_l3_fec_fec_result_type_fields_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_l3_fec_fec_physical_db_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_l3_fec_fec_tables_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_l3_fec_fec_hierachy_position_in_range_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_l3_fec_fec_resource_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_l3_fec_super_fec_result_types_map_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_l3_fec_fec_result_type_fields_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_l3_fec_fec_physical_db_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_l3_fec_fec_tables_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_l3_fec_fec_hierachy_position_in_range_info_get(
    int unit);






int dnx_data_l3_fer_feature_get(
    int unit,
    dnx_data_l3_fer_feature_e feature);



typedef enum
{
    dnx_data_l3_fer_define_mdb_granularity_ratio,
    dnx_data_l3_fer_define_nof_fer_sort_to_mdb_table_muxes,
    dnx_data_l3_fer_define_max_nof_sorting_blocks_per_fec_db,

    
    _dnx_data_l3_fer_define_nof
} dnx_data_l3_fer_define_e;



uint32 dnx_data_l3_fer_mdb_granularity_ratio_get(
    int unit);


uint32 dnx_data_l3_fer_nof_fer_sort_to_mdb_table_muxes_get(
    int unit);


uint32 dnx_data_l3_fer_max_nof_sorting_blocks_per_fec_db_get(
    int unit);



typedef enum
{
    dnx_data_l3_fer_table_mux_configuration,
    dnx_data_l3_fer_table_mux_connection,
    dnx_data_l3_fer_table_sorting_stages_info,

    
    _dnx_data_l3_fer_table_nof
} dnx_data_l3_fer_table_e;



const dnx_data_l3_fer_mux_configuration_t * dnx_data_l3_fer_mux_configuration_get(
    int unit,
    int mux_id);


const dnx_data_l3_fer_mux_connection_t * dnx_data_l3_fer_mux_connection_get(
    int unit,
    int physical_db);


const dnx_data_l3_fer_sorting_stages_info_t * dnx_data_l3_fer_sorting_stages_info_get(
    int unit,
    int sorting_stage);



shr_error_e dnx_data_l3_fer_mux_configuration_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_l3_fer_mux_connection_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_l3_fer_sorting_stages_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_l3_fer_mux_configuration_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_l3_fer_mux_connection_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_l3_fer_sorting_stages_info_info_get(
    int unit);






int dnx_data_l3_source_address_feature_get(
    int unit,
    dnx_data_l3_source_address_feature_e feature);



typedef enum
{
    dnx_data_l3_source_address_define_source_address_table_size,
    dnx_data_l3_source_address_define_nof_mymac_prefixes,
    dnx_data_l3_source_address_define_max_nof_mac_addresses_used_by_arp,
    dnx_data_l3_source_address_define_custom_sa_use_dual_homing,
    dnx_data_l3_source_address_define_is_mymac_prefix_limitted,

    
    _dnx_data_l3_source_address_define_nof
} dnx_data_l3_source_address_define_e;



uint32 dnx_data_l3_source_address_source_address_table_size_get(
    int unit);


uint32 dnx_data_l3_source_address_nof_mymac_prefixes_get(
    int unit);


uint32 dnx_data_l3_source_address_max_nof_mac_addresses_used_by_arp_get(
    int unit);


uint32 dnx_data_l3_source_address_custom_sa_use_dual_homing_get(
    int unit);


uint32 dnx_data_l3_source_address_is_mymac_prefix_limitted_get(
    int unit);



typedef enum
{
    dnx_data_l3_source_address_table_address_type_info,

    
    _dnx_data_l3_source_address_table_nof
} dnx_data_l3_source_address_table_e;



const dnx_data_l3_source_address_address_type_info_t * dnx_data_l3_source_address_address_type_info_get(
    int unit,
    int address_type);



shr_error_e dnx_data_l3_source_address_address_type_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_l3_source_address_address_type_info_info_get(
    int unit);






int dnx_data_l3_vrf_feature_get(
    int unit,
    dnx_data_l3_vrf_feature_e feature);



typedef enum
{
    dnx_data_l3_vrf_define_nof_vrf,
    dnx_data_l3_vrf_define_nof_vrf_ipv6,

    
    _dnx_data_l3_vrf_define_nof
} dnx_data_l3_vrf_define_e;



uint32 dnx_data_l3_vrf_nof_vrf_get(
    int unit);


uint32 dnx_data_l3_vrf_nof_vrf_ipv6_get(
    int unit);



typedef enum
{

    
    _dnx_data_l3_vrf_table_nof
} dnx_data_l3_vrf_table_e;









int dnx_data_l3_routing_enablers_feature_get(
    int unit,
    dnx_data_l3_routing_enablers_feature_e feature);



typedef enum
{
    dnx_data_l3_routing_enablers_define_routing_enable_vector_length,
    dnx_data_l3_routing_enablers_define_routing_enable_action_support,
    dnx_data_l3_routing_enablers_define_routing_enable_action_profile_size,
    dnx_data_l3_routing_enablers_define_routing_enable_indication_nof_bits_per_layer,
    dnx_data_l3_routing_enablers_define_routing_enable_nof_profiles_per_layer,
    dnx_data_l3_routing_enablers_define_routing_enabled_action_profile_id,

    
    _dnx_data_l3_routing_enablers_define_nof
} dnx_data_l3_routing_enablers_define_e;



uint32 dnx_data_l3_routing_enablers_routing_enable_vector_length_get(
    int unit);


uint32 dnx_data_l3_routing_enablers_routing_enable_action_support_get(
    int unit);


uint32 dnx_data_l3_routing_enablers_routing_enable_action_profile_size_get(
    int unit);


uint32 dnx_data_l3_routing_enablers_routing_enable_indication_nof_bits_per_layer_get(
    int unit);


uint32 dnx_data_l3_routing_enablers_routing_enable_nof_profiles_per_layer_get(
    int unit);


uint32 dnx_data_l3_routing_enablers_routing_enabled_action_profile_id_get(
    int unit);



typedef enum
{
    dnx_data_l3_routing_enablers_table_layer_enablers_by_id,
    dnx_data_l3_routing_enablers_table_layer_enablers,

    
    _dnx_data_l3_routing_enablers_table_nof
} dnx_data_l3_routing_enablers_table_e;



const dnx_data_l3_routing_enablers_layer_enablers_by_id_t * dnx_data_l3_routing_enablers_layer_enablers_by_id_get(
    int unit,
    int idx);


const dnx_data_l3_routing_enablers_layer_enablers_t * dnx_data_l3_routing_enablers_layer_enablers_get(
    int unit,
    int layer_type);



shr_error_e dnx_data_l3_routing_enablers_layer_enablers_by_id_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_l3_routing_enablers_layer_enablers_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_l3_routing_enablers_layer_enablers_by_id_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_l3_routing_enablers_layer_enablers_info_get(
    int unit);






int dnx_data_l3_rif_feature_get(
    int unit,
    dnx_data_l3_rif_feature_e feature);



typedef enum
{
    dnx_data_l3_rif_define_max_nof_rifs,
    dnx_data_l3_rif_define_nof_rifs,

    
    _dnx_data_l3_rif_define_nof
} dnx_data_l3_rif_define_e;



uint32 dnx_data_l3_rif_max_nof_rifs_get(
    int unit);


uint32 dnx_data_l3_rif_nof_rifs_get(
    int unit);



typedef enum
{

    
    _dnx_data_l3_rif_table_nof
} dnx_data_l3_rif_table_e;









int dnx_data_l3_fwd_feature_get(
    int unit,
    dnx_data_l3_fwd_feature_e feature);



typedef enum
{
    dnx_data_l3_fwd_define_flp_fragment_support,
    dnx_data_l3_fwd_define_max_mc_group_lpm,
    dnx_data_l3_fwd_define_max_mc_group_em,
    dnx_data_l3_fwd_define_compressed_sip_svl_size,
    dnx_data_l3_fwd_define_nof_compressed_svl_sip,
    dnx_data_l3_fwd_define_lpm_strength_profiles_support,
    dnx_data_l3_fwd_define_nof_lpm_strength_profiles,
    dnx_data_l3_fwd_define_compressed_sip_ivl_size,
    dnx_data_l3_fwd_define_nof_compressed_ivl_sip,
    dnx_data_l3_fwd_define_uc_supported_route_flags,
    dnx_data_l3_fwd_define_uc_supported_route_flags2,
    dnx_data_l3_fwd_define_uc_supported_kbp_route_flags,
    dnx_data_l3_fwd_define_uc_supported_kbp_route_flags2,
    dnx_data_l3_fwd_define_ipmc_supported_flags,
    dnx_data_l3_fwd_define_ipmc_config_supported_flags,
    dnx_data_l3_fwd_define_ipmc_config_supported_traverse_flags,
    dnx_data_l3_fwd_define_last_kaps_intf_id,

    
    _dnx_data_l3_fwd_define_nof
} dnx_data_l3_fwd_define_e;



uint32 dnx_data_l3_fwd_flp_fragment_support_get(
    int unit);


uint32 dnx_data_l3_fwd_max_mc_group_lpm_get(
    int unit);


uint32 dnx_data_l3_fwd_max_mc_group_em_get(
    int unit);


uint32 dnx_data_l3_fwd_compressed_sip_svl_size_get(
    int unit);


uint32 dnx_data_l3_fwd_nof_compressed_svl_sip_get(
    int unit);


uint32 dnx_data_l3_fwd_lpm_strength_profiles_support_get(
    int unit);


uint32 dnx_data_l3_fwd_nof_lpm_strength_profiles_get(
    int unit);


uint32 dnx_data_l3_fwd_compressed_sip_ivl_size_get(
    int unit);


uint32 dnx_data_l3_fwd_nof_compressed_ivl_sip_get(
    int unit);


uint32 dnx_data_l3_fwd_uc_supported_route_flags_get(
    int unit);


uint32 dnx_data_l3_fwd_uc_supported_route_flags2_get(
    int unit);


uint32 dnx_data_l3_fwd_uc_supported_kbp_route_flags_get(
    int unit);


uint32 dnx_data_l3_fwd_uc_supported_kbp_route_flags2_get(
    int unit);


uint32 dnx_data_l3_fwd_ipmc_supported_flags_get(
    int unit);


uint32 dnx_data_l3_fwd_ipmc_config_supported_flags_get(
    int unit);


uint32 dnx_data_l3_fwd_ipmc_config_supported_traverse_flags_get(
    int unit);


uint32 dnx_data_l3_fwd_last_kaps_intf_id_get(
    int unit);



typedef enum
{
    dnx_data_l3_fwd_table_lpm_profile_to_entry_strength,
    dnx_data_l3_fwd_table_routing_tables,
    dnx_data_l3_fwd_table_mc_tables,
    dnx_data_l3_fwd_table_ipmc_compression_mode,
    dnx_data_l3_fwd_table_lpm_default_prefix_length_info,
    dnx_data_l3_fwd_table_lpm_app_db_to_strength_profile,

    
    _dnx_data_l3_fwd_table_nof
} dnx_data_l3_fwd_table_e;



const dnx_data_l3_fwd_lpm_profile_to_entry_strength_t * dnx_data_l3_fwd_lpm_profile_to_entry_strength_get(
    int unit,
    int lpm_profile,
    int kaps_intf);


const dnx_data_l3_fwd_routing_tables_t * dnx_data_l3_fwd_routing_tables_get(
    int unit,
    int ip_version);


const dnx_data_l3_fwd_mc_tables_t * dnx_data_l3_fwd_mc_tables_get(
    int unit,
    int ip_version,
    int route_enable);


const dnx_data_l3_fwd_ipmc_compression_mode_t * dnx_data_l3_fwd_ipmc_compression_mode_get(
    int unit,
    int route_enable,
    int dbal_table_type);


const dnx_data_l3_fwd_lpm_default_prefix_length_info_t * dnx_data_l3_fwd_lpm_default_prefix_length_info_get(
    int unit,
    int physical_db_id);


const dnx_data_l3_fwd_lpm_app_db_to_strength_profile_t * dnx_data_l3_fwd_lpm_app_db_to_strength_profile_get(
    int unit,
    int kaps_intf);



shr_error_e dnx_data_l3_fwd_lpm_profile_to_entry_strength_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_l3_fwd_routing_tables_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_l3_fwd_mc_tables_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_l3_fwd_ipmc_compression_mode_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_l3_fwd_lpm_default_prefix_length_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_l3_fwd_lpm_app_db_to_strength_profile_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_l3_fwd_lpm_profile_to_entry_strength_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_l3_fwd_routing_tables_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_l3_fwd_mc_tables_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_l3_fwd_ipmc_compression_mode_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_l3_fwd_lpm_default_prefix_length_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_l3_fwd_lpm_app_db_to_strength_profile_info_get(
    int unit);






int dnx_data_l3_ecmp_feature_get(
    int unit,
    dnx_data_l3_ecmp_feature_e feature);



typedef enum
{
    dnx_data_l3_ecmp_define_group_size_multiply_and_divide_nof_bits,
    dnx_data_l3_ecmp_define_group_size_consistent_nof_bits,
    dnx_data_l3_ecmp_define_group_size_nof_bits,
    dnx_data_l3_ecmp_define_max_group_size,
    dnx_data_l3_ecmp_define_nof_ecmp_basic_mode,
    dnx_data_l3_ecmp_define_max_ecmp_basic_mode,
    dnx_data_l3_ecmp_define_max_ecmp_extended_mode,
    dnx_data_l3_ecmp_define_nof_extended_ecmp_per_failover_bank,
    dnx_data_l3_ecmp_define_nof_failover_banks_per_extended_ecmp_bank,
    dnx_data_l3_ecmp_define_nof_group_profiles_per_hierarchy,
    dnx_data_l3_ecmp_define_member_table_nof_rows_size_in_bits,
    dnx_data_l3_ecmp_define_member_table_address_size_in_bits,
    dnx_data_l3_ecmp_define_ecmp_bank_size,
    dnx_data_l3_ecmp_define_nof_ecmp_banks_basic,
    dnx_data_l3_ecmp_define_nof_ecmp_banks_extended,
    dnx_data_l3_ecmp_define_nof_ecmp,
    dnx_data_l3_ecmp_define_profile_id_size,
    dnx_data_l3_ecmp_define_profile_id_offset,
    dnx_data_l3_ecmp_define_tunnel_priority_support,
    dnx_data_l3_ecmp_define_nof_tunnel_priority_map_profiles,
    dnx_data_l3_ecmp_define_tunnel_priority_field_width,
    dnx_data_l3_ecmp_define_tunnel_priority_index_field_width,
    dnx_data_l3_ecmp_define_consistent_mem_row_size_in_bits,
    dnx_data_l3_ecmp_define_consistent_bank_size_in_bits,
    dnx_data_l3_ecmp_define_ecmp_small_consistent_group_nof_entries,
    dnx_data_l3_ecmp_define_ecmp_small_consistent_group_entry_size_in_bits,
    dnx_data_l3_ecmp_define_nof_consistent_resources,

    
    _dnx_data_l3_ecmp_define_nof
} dnx_data_l3_ecmp_define_e;



uint32 dnx_data_l3_ecmp_group_size_multiply_and_divide_nof_bits_get(
    int unit);


uint32 dnx_data_l3_ecmp_group_size_consistent_nof_bits_get(
    int unit);


uint32 dnx_data_l3_ecmp_group_size_nof_bits_get(
    int unit);


uint32 dnx_data_l3_ecmp_max_group_size_get(
    int unit);


uint32 dnx_data_l3_ecmp_nof_ecmp_basic_mode_get(
    int unit);


uint32 dnx_data_l3_ecmp_max_ecmp_basic_mode_get(
    int unit);


uint32 dnx_data_l3_ecmp_max_ecmp_extended_mode_get(
    int unit);


uint32 dnx_data_l3_ecmp_nof_extended_ecmp_per_failover_bank_get(
    int unit);


uint32 dnx_data_l3_ecmp_nof_failover_banks_per_extended_ecmp_bank_get(
    int unit);


uint32 dnx_data_l3_ecmp_nof_group_profiles_per_hierarchy_get(
    int unit);


uint32 dnx_data_l3_ecmp_member_table_nof_rows_size_in_bits_get(
    int unit);


uint32 dnx_data_l3_ecmp_member_table_address_size_in_bits_get(
    int unit);


uint32 dnx_data_l3_ecmp_ecmp_bank_size_get(
    int unit);


uint32 dnx_data_l3_ecmp_nof_ecmp_banks_basic_get(
    int unit);


uint32 dnx_data_l3_ecmp_nof_ecmp_banks_extended_get(
    int unit);


uint32 dnx_data_l3_ecmp_nof_ecmp_get(
    int unit);


uint32 dnx_data_l3_ecmp_profile_id_size_get(
    int unit);


uint32 dnx_data_l3_ecmp_profile_id_offset_get(
    int unit);


uint32 dnx_data_l3_ecmp_tunnel_priority_support_get(
    int unit);


uint32 dnx_data_l3_ecmp_nof_tunnel_priority_map_profiles_get(
    int unit);


uint32 dnx_data_l3_ecmp_tunnel_priority_field_width_get(
    int unit);


uint32 dnx_data_l3_ecmp_tunnel_priority_index_field_width_get(
    int unit);


uint32 dnx_data_l3_ecmp_consistent_mem_row_size_in_bits_get(
    int unit);


uint32 dnx_data_l3_ecmp_consistent_bank_size_in_bits_get(
    int unit);


uint32 dnx_data_l3_ecmp_ecmp_small_consistent_group_nof_entries_get(
    int unit);


uint32 dnx_data_l3_ecmp_ecmp_small_consistent_group_entry_size_in_bits_get(
    int unit);


uint32 dnx_data_l3_ecmp_nof_consistent_resources_get(
    int unit);



typedef enum
{
    dnx_data_l3_ecmp_table_consistent_tables_info,

    
    _dnx_data_l3_ecmp_table_nof
} dnx_data_l3_ecmp_table_e;



const dnx_data_l3_ecmp_consistent_tables_info_t * dnx_data_l3_ecmp_consistent_tables_info_get(
    int unit,
    int table_type);



shr_error_e dnx_data_l3_ecmp_consistent_tables_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_l3_ecmp_consistent_tables_info_info_get(
    int unit);






int dnx_data_l3_vip_ecmp_feature_get(
    int unit,
    dnx_data_l3_vip_ecmp_feature_e feature);



typedef enum
{
    dnx_data_l3_vip_ecmp_define_nof_vip_ecmp,
    dnx_data_l3_vip_ecmp_define_max_vip_ecmp_table_size,

    
    _dnx_data_l3_vip_ecmp_define_nof
} dnx_data_l3_vip_ecmp_define_e;



uint32 dnx_data_l3_vip_ecmp_nof_vip_ecmp_get(
    int unit);


uint32 dnx_data_l3_vip_ecmp_max_vip_ecmp_table_size_get(
    int unit);



typedef enum
{

    
    _dnx_data_l3_vip_ecmp_table_nof
} dnx_data_l3_vip_ecmp_table_e;









int dnx_data_l3_wcmp_feature_get(
    int unit,
    dnx_data_l3_wcmp_feature_e feature);



typedef enum
{
    dnx_data_l3_wcmp_define_group_size_nof_bits,
    dnx_data_l3_wcmp_define_max_member_weight,
    dnx_data_l3_wcmp_define_member_weight_quant_factor,

    
    _dnx_data_l3_wcmp_define_nof
} dnx_data_l3_wcmp_define_e;



uint32 dnx_data_l3_wcmp_group_size_nof_bits_get(
    int unit);


uint32 dnx_data_l3_wcmp_max_member_weight_get(
    int unit);


uint32 dnx_data_l3_wcmp_member_weight_quant_factor_get(
    int unit);



typedef enum
{

    
    _dnx_data_l3_wcmp_table_nof
} dnx_data_l3_wcmp_table_e;









int dnx_data_l3_vrrp_feature_get(
    int unit,
    dnx_data_l3_vrrp_feature_e feature);



typedef enum
{
    dnx_data_l3_vrrp_define_nof_protocol_groups,
    dnx_data_l3_vrrp_define_nof_tcam_entries,
    dnx_data_l3_vrrp_define_nof_vsi_tcam_entries,
    dnx_data_l3_vrrp_define_nof_tcam_entries_used_by_exem,
    dnx_data_l3_vrrp_define_exem_vrid_ipv4_tcam_index,
    dnx_data_l3_vrrp_define_exem_vrid_ipv6_tcam_index,
    dnx_data_l3_vrrp_define_exem_default_tcam_index,
    dnx_data_l3_vrrp_define_exem_default_tcam_index_msb,
    dnx_data_l3_vrrp_define_vrrp_default_num_entries,
    dnx_data_l3_vrrp_define_is_mapped_pp_port_used,
    dnx_data_l3_vrrp_define_nof_dbs,

    
    _dnx_data_l3_vrrp_define_nof
} dnx_data_l3_vrrp_define_e;



uint32 dnx_data_l3_vrrp_nof_protocol_groups_get(
    int unit);


uint32 dnx_data_l3_vrrp_nof_tcam_entries_get(
    int unit);


uint32 dnx_data_l3_vrrp_nof_vsi_tcam_entries_get(
    int unit);


uint32 dnx_data_l3_vrrp_nof_tcam_entries_used_by_exem_get(
    int unit);


uint32 dnx_data_l3_vrrp_exem_vrid_ipv4_tcam_index_get(
    int unit);


uint32 dnx_data_l3_vrrp_exem_vrid_ipv6_tcam_index_get(
    int unit);


uint32 dnx_data_l3_vrrp_exem_default_tcam_index_get(
    int unit);


uint32 dnx_data_l3_vrrp_exem_default_tcam_index_msb_get(
    int unit);


uint32 dnx_data_l3_vrrp_vrrp_default_num_entries_get(
    int unit);


uint32 dnx_data_l3_vrrp_is_mapped_pp_port_used_get(
    int unit);


uint32 dnx_data_l3_vrrp_nof_dbs_get(
    int unit);



typedef enum
{
    dnx_data_l3_vrrp_table_vrrp_tables,
    dnx_data_l3_vrrp_table_data_bases,

    
    _dnx_data_l3_vrrp_table_nof
} dnx_data_l3_vrrp_table_e;



const dnx_data_l3_vrrp_vrrp_tables_t * dnx_data_l3_vrrp_vrrp_tables_get(
    int unit,
    int table_type);


const dnx_data_l3_vrrp_data_bases_t * dnx_data_l3_vrrp_data_bases_get(
    int unit);



shr_error_e dnx_data_l3_vrrp_vrrp_tables_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_l3_vrrp_data_bases_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_l3_vrrp_vrrp_tables_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_l3_vrrp_data_bases_info_get(
    int unit);






int dnx_data_l3_feature_feature_get(
    int unit,
    dnx_data_l3_feature_feature_e feature);



typedef enum
{

    
    _dnx_data_l3_feature_define_nof
} dnx_data_l3_feature_define_e;




typedef enum
{

    
    _dnx_data_l3_feature_table_nof
} dnx_data_l3_feature_table_e;






shr_error_e dnx_data_l3_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

