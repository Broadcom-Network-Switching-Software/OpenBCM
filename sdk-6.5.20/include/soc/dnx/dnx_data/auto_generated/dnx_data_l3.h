

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_L3_H_

#define _DNX_DATA_L3_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_l3.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_l3_init(
    int unit);







typedef enum
{

    
    _dnx_data_l3_egr_pointed_feature_nof
} dnx_data_l3_egr_pointed_feature_e;



typedef int(
    *dnx_data_l3_egr_pointed_feature_get_f) (
    int unit,
    dnx_data_l3_egr_pointed_feature_e feature);


typedef uint32(
    *dnx_data_l3_egr_pointed_first_egr_pointed_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_egr_pointed_nof_egr_pointed_ids_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_egr_pointed_nof_bits_in_egr_pointed_id_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_l3_egr_pointed_feature_get_f feature_get;
    
    dnx_data_l3_egr_pointed_first_egr_pointed_id_get_f first_egr_pointed_id_get;
    
    dnx_data_l3_egr_pointed_nof_egr_pointed_ids_get_f nof_egr_pointed_ids_get;
    
    dnx_data_l3_egr_pointed_nof_bits_in_egr_pointed_id_get_f nof_bits_in_egr_pointed_id_get;
} dnx_data_if_l3_egr_pointed_t;







typedef enum
{

    
    _dnx_data_l3_fec_feature_nof
} dnx_data_l3_fec_feature_e;



typedef int(
    *dnx_data_l3_fec_feature_get_f) (
    int unit,
    dnx_data_l3_fec_feature_e feature);


typedef uint32(
    *dnx_data_l3_fec_nof_fecs_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fec_first_valid_fec_ecmp_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fec_max_fec_id_for_single_dhb_cluster_pair_granularity_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fec_max_fec_id_for_double_dhb_cluster_pair_granularity_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fec_max_default_fec_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fec_bank_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fec_bank_size_round_up_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fec_max_nof_super_fecs_per_bank_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fec_nof_physical_fecs_per_bank_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fec_super_fec_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fec_max_nof_banks_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fec_first_bank_without_id_alloc_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fec_max_super_fec_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fec_fer_mdb_granularity_ratio_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_l3_fec_feature_get_f feature_get;
    
    dnx_data_l3_fec_nof_fecs_get_f nof_fecs_get;
    
    dnx_data_l3_fec_first_valid_fec_ecmp_id_get_f first_valid_fec_ecmp_id_get;
    
    dnx_data_l3_fec_max_fec_id_for_single_dhb_cluster_pair_granularity_get_f max_fec_id_for_single_dhb_cluster_pair_granularity_get;
    
    dnx_data_l3_fec_max_fec_id_for_double_dhb_cluster_pair_granularity_get_f max_fec_id_for_double_dhb_cluster_pair_granularity_get;
    
    dnx_data_l3_fec_max_default_fec_get_f max_default_fec_get;
    
    dnx_data_l3_fec_bank_size_get_f bank_size_get;
    
    dnx_data_l3_fec_bank_size_round_up_get_f bank_size_round_up_get;
    
    dnx_data_l3_fec_max_nof_super_fecs_per_bank_get_f max_nof_super_fecs_per_bank_get;
    
    dnx_data_l3_fec_nof_physical_fecs_per_bank_get_f nof_physical_fecs_per_bank_get;
    
    dnx_data_l3_fec_super_fec_size_get_f super_fec_size_get;
    
    dnx_data_l3_fec_max_nof_banks_get_f max_nof_banks_get;
    
    dnx_data_l3_fec_first_bank_without_id_alloc_get_f first_bank_without_id_alloc_get;
    
    dnx_data_l3_fec_max_super_fec_id_get_f max_super_fec_id_get;
    
    dnx_data_l3_fec_fer_mdb_granularity_ratio_get_f fer_mdb_granularity_ratio_get;
} dnx_data_if_l3_fec_t;







typedef enum
{

    
    _dnx_data_l3_source_address_feature_nof
} dnx_data_l3_source_address_feature_e;



typedef int(
    *dnx_data_l3_source_address_feature_get_f) (
    int unit,
    dnx_data_l3_source_address_feature_e feature);


typedef uint32(
    *dnx_data_l3_source_address_source_address_table_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_source_address_custom_sa_use_dual_homing_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_l3_source_address_feature_get_f feature_get;
    
    dnx_data_l3_source_address_source_address_table_size_get_f source_address_table_size_get;
    
    dnx_data_l3_source_address_custom_sa_use_dual_homing_get_f custom_sa_use_dual_homing_get;
} dnx_data_if_l3_source_address_t;







typedef enum
{

    
    _dnx_data_l3_vrf_feature_nof
} dnx_data_l3_vrf_feature_e;



typedef int(
    *dnx_data_l3_vrf_feature_get_f) (
    int unit,
    dnx_data_l3_vrf_feature_e feature);


typedef uint32(
    *dnx_data_l3_vrf_nof_vrf_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_vrf_nof_vrf_ipv6_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_l3_vrf_feature_get_f feature_get;
    
    dnx_data_l3_vrf_nof_vrf_get_f nof_vrf_get;
    
    dnx_data_l3_vrf_nof_vrf_ipv6_get_f nof_vrf_ipv6_get;
} dnx_data_if_l3_vrf_t;







typedef enum
{

    
    _dnx_data_l3_routing_enablers_feature_nof
} dnx_data_l3_routing_enablers_feature_e;



typedef int(
    *dnx_data_l3_routing_enablers_feature_get_f) (
    int unit,
    dnx_data_l3_routing_enablers_feature_e feature);


typedef uint32(
    *dnx_data_l3_routing_enablers_nof_routing_enablers_profiles_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_l3_routing_enablers_feature_get_f feature_get;
    
    dnx_data_l3_routing_enablers_nof_routing_enablers_profiles_get_f nof_routing_enablers_profiles_get;
} dnx_data_if_l3_routing_enablers_t;







typedef enum
{

    
    _dnx_data_l3_rif_feature_nof
} dnx_data_l3_rif_feature_e;



typedef int(
    *dnx_data_l3_rif_feature_get_f) (
    int unit,
    dnx_data_l3_rif_feature_e feature);


typedef uint32(
    *dnx_data_l3_rif_max_nof_rifs_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_rif_nof_rifs_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_l3_rif_feature_get_f feature_get;
    
    dnx_data_l3_rif_max_nof_rifs_get_f max_nof_rifs_get;
    
    dnx_data_l3_rif_nof_rifs_get_f nof_rifs_get;
} dnx_data_if_l3_rif_t;






typedef struct
{
    
    uint32 prefix_len_non_def[8];
    
    uint32 prefix_len_def[8];
    
    uint32 entry_strength_non_def[8];
    
    uint32 entry_strength_def[8];
} dnx_data_l3_fwd_lpm_profile_to_entry_strength_t;



typedef enum
{

    
    _dnx_data_l3_fwd_feature_nof
} dnx_data_l3_fwd_feature_e;



typedef int(
    *dnx_data_l3_fwd_feature_get_f) (
    int unit,
    dnx_data_l3_fwd_feature_e feature);


typedef uint32(
    *dnx_data_l3_fwd_flp_fragment_support_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fwd_max_mc_group_lpm_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fwd_max_mc_group_em_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fwd_nof_my_mac_prefixes_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fwd_compressed_sip_svl_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fwd_nof_compressed_svl_sip_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fwd_lpm_strength_profile_support_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fwd_nof_lpm_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fwd_nof_prefix_ranges_get_f) (
    int unit);


typedef const dnx_data_l3_fwd_lpm_profile_to_entry_strength_t *(
    *dnx_data_l3_fwd_lpm_profile_to_entry_strength_get_f) (
    int unit,
    int lpm_profile,
    int kaps_intf);



typedef struct
{
    
    dnx_data_l3_fwd_feature_get_f feature_get;
    
    dnx_data_l3_fwd_flp_fragment_support_get_f flp_fragment_support_get;
    
    dnx_data_l3_fwd_max_mc_group_lpm_get_f max_mc_group_lpm_get;
    
    dnx_data_l3_fwd_max_mc_group_em_get_f max_mc_group_em_get;
    
    dnx_data_l3_fwd_nof_my_mac_prefixes_get_f nof_my_mac_prefixes_get;
    
    dnx_data_l3_fwd_compressed_sip_svl_size_get_f compressed_sip_svl_size_get;
    
    dnx_data_l3_fwd_nof_compressed_svl_sip_get_f nof_compressed_svl_sip_get;
    
    dnx_data_l3_fwd_lpm_strength_profile_support_get_f lpm_strength_profile_support_get;
    
    dnx_data_l3_fwd_nof_lpm_profiles_get_f nof_lpm_profiles_get;
    
    dnx_data_l3_fwd_nof_prefix_ranges_get_f nof_prefix_ranges_get;
    
    dnx_data_l3_fwd_lpm_profile_to_entry_strength_get_f lpm_profile_to_entry_strength_get;
    
    dnxc_data_table_info_get_f lpm_profile_to_entry_strength_info_get;
} dnx_data_if_l3_fwd_t;






typedef struct
{
    
    uint32 nof_entries;
    
    uint32 entry_size_in_bits;
    
    uint32 max_nof_unique_members;
} dnx_data_l3_ecmp_consistent_tables_info_t;



typedef enum
{
    
    dnx_data_l3_ecmp_ecmp_extended,

    
    _dnx_data_l3_ecmp_feature_nof
} dnx_data_l3_ecmp_feature_e;



typedef int(
    *dnx_data_l3_ecmp_feature_get_f) (
    int unit,
    dnx_data_l3_ecmp_feature_e feature);


typedef uint32(
    *dnx_data_l3_ecmp_group_size_multiply_and_divide_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_ecmp_group_size_consistent_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_ecmp_group_size_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_ecmp_max_group_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_ecmp_nof_ecmp_per_bank_of_low_ecmps_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_ecmp_nof_ecmp_basic_mode_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_ecmp_total_nof_ecmp_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_ecmp_max_ecmp_basic_mode_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_ecmp_max_ecmp_extended_mode_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_ecmp_nof_ecmp_per_bank_of_extended_ecmps_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_ecmp_nof_fec_banks_per_extended_ecmp_bank_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_ecmp_nof_group_profiles_per_hierarchy_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_ecmp_member_table_nof_rows_size_in_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_ecmp_member_table_address_size_in_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_ecmp_ecmp_stage_map_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_ecmp_nof_ecmp_banks_basic_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_ecmp_nof_ecmp_banks_extended_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_ecmp_nof_ecmp_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_ecmp_profile_id_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_ecmp_profile_id_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_ecmp_member_table_row_width_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_ecmp_tunnel_priority_support_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_ecmp_nof_tunnel_priority_map_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_ecmp_tunnel_priority_field_width_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_ecmp_tunnel_priority_index_field_width_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_ecmp_consistent_mem_row_size_in_bits_get_f) (
    int unit);


typedef const dnx_data_l3_ecmp_consistent_tables_info_t *(
    *dnx_data_l3_ecmp_consistent_tables_info_get_f) (
    int unit,
    int table_type);



typedef struct
{
    
    dnx_data_l3_ecmp_feature_get_f feature_get;
    
    dnx_data_l3_ecmp_group_size_multiply_and_divide_nof_bits_get_f group_size_multiply_and_divide_nof_bits_get;
    
    dnx_data_l3_ecmp_group_size_consistent_nof_bits_get_f group_size_consistent_nof_bits_get;
    
    dnx_data_l3_ecmp_group_size_nof_bits_get_f group_size_nof_bits_get;
    
    dnx_data_l3_ecmp_max_group_size_get_f max_group_size_get;
    
    dnx_data_l3_ecmp_nof_ecmp_per_bank_of_low_ecmps_get_f nof_ecmp_per_bank_of_low_ecmps_get;
    
    dnx_data_l3_ecmp_nof_ecmp_basic_mode_get_f nof_ecmp_basic_mode_get;
    
    dnx_data_l3_ecmp_total_nof_ecmp_get_f total_nof_ecmp_get;
    
    dnx_data_l3_ecmp_max_ecmp_basic_mode_get_f max_ecmp_basic_mode_get;
    
    dnx_data_l3_ecmp_max_ecmp_extended_mode_get_f max_ecmp_extended_mode_get;
    
    dnx_data_l3_ecmp_nof_ecmp_per_bank_of_extended_ecmps_get_f nof_ecmp_per_bank_of_extended_ecmps_get;
    
    dnx_data_l3_ecmp_nof_fec_banks_per_extended_ecmp_bank_get_f nof_fec_banks_per_extended_ecmp_bank_get;
    
    dnx_data_l3_ecmp_nof_group_profiles_per_hierarchy_get_f nof_group_profiles_per_hierarchy_get;
    
    dnx_data_l3_ecmp_member_table_nof_rows_size_in_bits_get_f member_table_nof_rows_size_in_bits_get;
    
    dnx_data_l3_ecmp_member_table_address_size_in_bits_get_f member_table_address_size_in_bits_get;
    
    dnx_data_l3_ecmp_ecmp_stage_map_size_get_f ecmp_stage_map_size_get;
    
    dnx_data_l3_ecmp_nof_ecmp_banks_basic_get_f nof_ecmp_banks_basic_get;
    
    dnx_data_l3_ecmp_nof_ecmp_banks_extended_get_f nof_ecmp_banks_extended_get;
    
    dnx_data_l3_ecmp_nof_ecmp_get_f nof_ecmp_get;
    
    dnx_data_l3_ecmp_profile_id_size_get_f profile_id_size_get;
    
    dnx_data_l3_ecmp_profile_id_offset_get_f profile_id_offset_get;
    
    dnx_data_l3_ecmp_member_table_row_width_get_f member_table_row_width_get;
    
    dnx_data_l3_ecmp_tunnel_priority_support_get_f tunnel_priority_support_get;
    
    dnx_data_l3_ecmp_nof_tunnel_priority_map_profiles_get_f nof_tunnel_priority_map_profiles_get;
    
    dnx_data_l3_ecmp_tunnel_priority_field_width_get_f tunnel_priority_field_width_get;
    
    dnx_data_l3_ecmp_tunnel_priority_index_field_width_get_f tunnel_priority_index_field_width_get;
    
    dnx_data_l3_ecmp_consistent_mem_row_size_in_bits_get_f consistent_mem_row_size_in_bits_get;
    
    dnx_data_l3_ecmp_consistent_tables_info_get_f consistent_tables_info_get;
    
    dnxc_data_table_info_get_f consistent_tables_info_info_get;
} dnx_data_if_l3_ecmp_t;







typedef enum
{

    
    _dnx_data_l3_vip_ecmp_feature_nof
} dnx_data_l3_vip_ecmp_feature_e;



typedef int(
    *dnx_data_l3_vip_ecmp_feature_get_f) (
    int unit,
    dnx_data_l3_vip_ecmp_feature_e feature);


typedef uint32(
    *dnx_data_l3_vip_ecmp_nof_vip_ecmp_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_vip_ecmp_max_vip_ecmp_table_size_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_l3_vip_ecmp_feature_get_f feature_get;
    
    dnx_data_l3_vip_ecmp_nof_vip_ecmp_get_f nof_vip_ecmp_get;
    
    dnx_data_l3_vip_ecmp_max_vip_ecmp_table_size_get_f max_vip_ecmp_table_size_get;
} dnx_data_if_l3_vip_ecmp_t;







typedef enum
{

    
    _dnx_data_l3_source_address_ethernet_feature_nof
} dnx_data_l3_source_address_ethernet_feature_e;



typedef int(
    *dnx_data_l3_source_address_ethernet_feature_get_f) (
    int unit,
    dnx_data_l3_source_address_ethernet_feature_e feature);


typedef uint32(
    *dnx_data_l3_source_address_ethernet_nof_mymac_prefixes_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_l3_source_address_ethernet_feature_get_f feature_get;
    
    dnx_data_l3_source_address_ethernet_nof_mymac_prefixes_get_f nof_mymac_prefixes_get;
} dnx_data_if_l3_source_address_ethernet_t;







typedef enum
{

    
    _dnx_data_l3_vrrp_feature_nof
} dnx_data_l3_vrrp_feature_e;



typedef int(
    *dnx_data_l3_vrrp_feature_get_f) (
    int unit,
    dnx_data_l3_vrrp_feature_e feature);


typedef uint32(
    *dnx_data_l3_vrrp_nof_protocol_groups_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_vrrp_nof_tcam_entries_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_vrrp_nof_vsi_tcam_entries_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_vrrp_exem_vrid_ipv4_tcam_index_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_vrrp_exem_vrid_ipv6_tcam_index_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_vrrp_exem_default_tcam_index_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_vrrp_exem_default_tcam_index_msb_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_vrrp_vrrp_default_num_entries_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_l3_vrrp_feature_get_f feature_get;
    
    dnx_data_l3_vrrp_nof_protocol_groups_get_f nof_protocol_groups_get;
    
    dnx_data_l3_vrrp_nof_tcam_entries_get_f nof_tcam_entries_get;
    
    dnx_data_l3_vrrp_nof_vsi_tcam_entries_get_f nof_vsi_tcam_entries_get;
    
    dnx_data_l3_vrrp_exem_vrid_ipv4_tcam_index_get_f exem_vrid_ipv4_tcam_index_get;
    
    dnx_data_l3_vrrp_exem_vrid_ipv6_tcam_index_get_f exem_vrid_ipv6_tcam_index_get;
    
    dnx_data_l3_vrrp_exem_default_tcam_index_get_f exem_default_tcam_index_get;
    
    dnx_data_l3_vrrp_exem_default_tcam_index_msb_get_f exem_default_tcam_index_msb_get;
    
    dnx_data_l3_vrrp_vrrp_default_num_entries_get_f vrrp_default_num_entries_get;
} dnx_data_if_l3_vrrp_t;







typedef enum
{
    
    dnx_data_l3_feature_ipv6_mc_compatible_dmac,
    
    dnx_data_l3_feature_fec_hit_bit,
    
    dnx_data_l3_feature_fer_fec_granularity_double_size,
    
    dnx_data_l3_feature_mc_bridge_fallback,
    
    dnx_data_l3_feature_nat_on_a_stick,

    
    _dnx_data_l3_feature_feature_nof
} dnx_data_l3_feature_feature_e;



typedef int(
    *dnx_data_l3_feature_feature_get_f) (
    int unit,
    dnx_data_l3_feature_feature_e feature);



typedef struct
{
    
    dnx_data_l3_feature_feature_get_f feature_get;
} dnx_data_if_l3_feature_t;





typedef struct
{
    
    dnx_data_if_l3_egr_pointed_t egr_pointed;
    
    dnx_data_if_l3_fec_t fec;
    
    dnx_data_if_l3_source_address_t source_address;
    
    dnx_data_if_l3_vrf_t vrf;
    
    dnx_data_if_l3_routing_enablers_t routing_enablers;
    
    dnx_data_if_l3_rif_t rif;
    
    dnx_data_if_l3_fwd_t fwd;
    
    dnx_data_if_l3_ecmp_t ecmp;
    
    dnx_data_if_l3_vip_ecmp_t vip_ecmp;
    
    dnx_data_if_l3_source_address_ethernet_t source_address_ethernet;
    
    dnx_data_if_l3_vrrp_t vrrp;
    
    dnx_data_if_l3_feature_t feature;
} dnx_data_if_l3_t;




extern dnx_data_if_l3_t dnx_data_l3;


#endif 

