
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_L3_H_

#define _DNX_DATA_L3_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/mdb_global.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_enum_field_types.h>
#include <soc/dnx/dbal/dbal_external_defines.h>
#include <bcm/l3.h>
#include <bcm/ipmc.h>
#include <bcm_int/dnx/l3/l3.h>
#include <bcm_int/dnx/algo/l3/source_address_table_allocation.h>
#include <bcm_int/dnx/ipmc/ipmc.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_mdb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_l3.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
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






typedef struct
{
    uint8 is_protection_type;
    uint8 is_statistic_type;
    uint8 nof_statistics;
    dbal_result_type_t dbal_result_type;
} dnx_data_l3_fec_fec_resource_info_t;


typedef struct
{
    dbal_fields_e fec_type_in_super_fec;
    uint32 fec_resource_type;
} dnx_data_l3_fec_super_fec_result_types_map_t;


typedef struct
{
    uint8 has_global_outlif;
    uint8 has_global_outlif_second;
    uint8 has_mc_rpf;
    uint8 has_eei;
    uint8 has_htm;
} dnx_data_l3_fec_fec_result_type_fields_t;


typedef struct
{
    mdb_physical_table_e physical_table[6];
} dnx_data_l3_fec_fec_physical_db_t;


typedef struct
{
    dbal_tables_e dbal_table;
    uint32 hierarchy;
    int index;
} dnx_data_l3_fec_fec_tables_info_t;


typedef struct
{
    uint32 forced_position;
} dnx_data_l3_fec_fec_hierachy_position_in_range_t;



typedef enum
{
    dnx_data_l3_fec_mc_rpf_sip_based_supported,

    
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
    *dnx_data_l3_fec_bank_nof_physical_rows_get_f) (
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
    *dnx_data_l3_fec_first_shared_bank_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fec_max_super_fec_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fec_nof_fec_directions_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fec_fec_z_max_fec_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fec_nof_fec_result_types_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fec_uneven_bank_sizes_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fec_fer_hw_version_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fec_fec_property_supported_flags_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fec_supported_fec_flags_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fec_supported_fec_flags2_get_f) (
    int unit);


typedef const dnx_data_l3_fec_fec_resource_info_t *(
    *dnx_data_l3_fec_fec_resource_info_get_f) (
    int unit,
    int resource_type);


typedef const dnx_data_l3_fec_super_fec_result_types_map_t *(
    *dnx_data_l3_fec_super_fec_result_types_map_get_f) (
    int unit,
    int dbal_result_type);


typedef const dnx_data_l3_fec_fec_result_type_fields_t *(
    *dnx_data_l3_fec_fec_result_type_fields_get_f) (
    int unit,
    int result_type);


typedef const dnx_data_l3_fec_fec_physical_db_t *(
    *dnx_data_l3_fec_fec_physical_db_get_f) (
    int unit);


typedef const dnx_data_l3_fec_fec_tables_info_t *(
    *dnx_data_l3_fec_fec_tables_info_get_f) (
    int unit,
    int mdb_table);


typedef const dnx_data_l3_fec_fec_hierachy_position_in_range_t *(
    *dnx_data_l3_fec_fec_hierachy_position_in_range_get_f) (
    int unit,
    int hierarchy);



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
    
    dnx_data_l3_fec_bank_nof_physical_rows_get_f bank_nof_physical_rows_get;
    
    dnx_data_l3_fec_super_fec_size_get_f super_fec_size_get;
    
    dnx_data_l3_fec_max_nof_banks_get_f max_nof_banks_get;
    
    dnx_data_l3_fec_first_bank_without_id_alloc_get_f first_bank_without_id_alloc_get;
    
    dnx_data_l3_fec_first_shared_bank_get_f first_shared_bank_get;
    
    dnx_data_l3_fec_max_super_fec_id_get_f max_super_fec_id_get;
    
    dnx_data_l3_fec_nof_fec_directions_get_f nof_fec_directions_get;
    
    dnx_data_l3_fec_fec_z_max_fec_id_get_f fec_z_max_fec_id_get;
    
    dnx_data_l3_fec_nof_fec_result_types_get_f nof_fec_result_types_get;
    
    dnx_data_l3_fec_uneven_bank_sizes_get_f uneven_bank_sizes_get;
    
    dnx_data_l3_fec_fer_hw_version_get_f fer_hw_version_get;
    
    dnx_data_l3_fec_fec_property_supported_flags_get_f fec_property_supported_flags_get;
    
    dnx_data_l3_fec_supported_fec_flags_get_f supported_fec_flags_get;
    
    dnx_data_l3_fec_supported_fec_flags2_get_f supported_fec_flags2_get;
    
    dnx_data_l3_fec_fec_resource_info_get_f fec_resource_info_get;
    
    dnxc_data_table_info_get_f fec_resource_info_info_get;
    
    dnx_data_l3_fec_super_fec_result_types_map_get_f super_fec_result_types_map_get;
    
    dnxc_data_table_info_get_f super_fec_result_types_map_info_get;
    
    dnx_data_l3_fec_fec_result_type_fields_get_f fec_result_type_fields_get;
    
    dnxc_data_table_info_get_f fec_result_type_fields_info_get;
    
    dnx_data_l3_fec_fec_physical_db_get_f fec_physical_db_get;
    
    dnxc_data_table_info_get_f fec_physical_db_info_get;
    
    dnx_data_l3_fec_fec_tables_info_get_f fec_tables_info_get;
    
    dnxc_data_table_info_get_f fec_tables_info_info_get;
    
    dnx_data_l3_fec_fec_hierachy_position_in_range_get_f fec_hierachy_position_in_range_get;
    
    dnxc_data_table_info_get_f fec_hierachy_position_in_range_info_get;
} dnx_data_if_l3_fec_t;






typedef struct
{
    uint32 direction;
    uint32 hierarchy;
} dnx_data_l3_fer_mux_configuration_t;


typedef struct
{
    uint32 mux_ids[3];
    uint32 nof_muxes;
} dnx_data_l3_fer_mux_connection_t;


typedef struct
{
    dbal_tables_e bank_mapping_table;
    uint32 direction;
    uint32 hierarchy;
} dnx_data_l3_fer_sorting_stages_info_t;



typedef enum
{
    dnx_data_l3_fer_mux_connection_supported,

    
    _dnx_data_l3_fer_feature_nof
} dnx_data_l3_fer_feature_e;



typedef int(
    *dnx_data_l3_fer_feature_get_f) (
    int unit,
    dnx_data_l3_fer_feature_e feature);


typedef uint32(
    *dnx_data_l3_fer_mdb_granularity_ratio_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fer_nof_fer_sort_to_mdb_table_muxes_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fer_max_nof_sorting_blocks_per_fec_db_get_f) (
    int unit);


typedef const dnx_data_l3_fer_mux_configuration_t *(
    *dnx_data_l3_fer_mux_configuration_get_f) (
    int unit,
    int mux_id);


typedef const dnx_data_l3_fer_mux_connection_t *(
    *dnx_data_l3_fer_mux_connection_get_f) (
    int unit,
    int physical_db);


typedef const dnx_data_l3_fer_sorting_stages_info_t *(
    *dnx_data_l3_fer_sorting_stages_info_get_f) (
    int unit,
    int sorting_stage);



typedef struct
{
    
    dnx_data_l3_fer_feature_get_f feature_get;
    
    dnx_data_l3_fer_mdb_granularity_ratio_get_f mdb_granularity_ratio_get;
    
    dnx_data_l3_fer_nof_fer_sort_to_mdb_table_muxes_get_f nof_fer_sort_to_mdb_table_muxes_get;
    
    dnx_data_l3_fer_max_nof_sorting_blocks_per_fec_db_get_f max_nof_sorting_blocks_per_fec_db_get;
    
    dnx_data_l3_fer_mux_configuration_get_f mux_configuration_get;
    
    dnxc_data_table_info_get_f mux_configuration_info_get;
    
    dnx_data_l3_fer_mux_connection_get_f mux_connection_get;
    
    dnxc_data_table_info_get_f mux_connection_info_get;
    
    dnx_data_l3_fer_sorting_stages_info_get_f sorting_stages_info_get;
    
    dnxc_data_table_info_get_f sorting_stages_info_info_get;
} dnx_data_if_l3_fer_t;






typedef struct
{
    uint32 nof_entries;
} dnx_data_l3_source_address_address_type_info_t;



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
    *dnx_data_l3_source_address_nof_mymac_prefixes_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_source_address_max_nof_mac_addresses_used_by_arp_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_source_address_custom_sa_use_dual_homing_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_source_address_is_mymac_prefix_limitted_get_f) (
    int unit);


typedef const dnx_data_l3_source_address_address_type_info_t *(
    *dnx_data_l3_source_address_address_type_info_get_f) (
    int unit,
    int address_type);



typedef struct
{
    
    dnx_data_l3_source_address_feature_get_f feature_get;
    
    dnx_data_l3_source_address_source_address_table_size_get_f source_address_table_size_get;
    
    dnx_data_l3_source_address_nof_mymac_prefixes_get_f nof_mymac_prefixes_get;
    
    dnx_data_l3_source_address_max_nof_mac_addresses_used_by_arp_get_f max_nof_mac_addresses_used_by_arp_get;
    
    dnx_data_l3_source_address_custom_sa_use_dual_homing_get_f custom_sa_use_dual_homing_get;
    
    dnx_data_l3_source_address_is_mymac_prefix_limitted_get_f is_mymac_prefix_limitted_get;
    
    dnx_data_l3_source_address_address_type_info_get_f address_type_info_get;
    
    dnxc_data_table_info_get_f address_type_info_info_get;
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






typedef struct
{
    uint32 layer_type;
} dnx_data_l3_routing_enablers_layer_enablers_by_id_t;


typedef struct
{
    uint8 uc_enable;
    uint8 mc_enable;
    uint8 disable_profile;
} dnx_data_l3_routing_enablers_layer_enablers_t;



typedef enum
{
    dnx_data_l3_routing_enablers_ptc_routing_enable_profile_support,

    
    _dnx_data_l3_routing_enablers_feature_nof
} dnx_data_l3_routing_enablers_feature_e;



typedef int(
    *dnx_data_l3_routing_enablers_feature_get_f) (
    int unit,
    dnx_data_l3_routing_enablers_feature_e feature);


typedef uint32(
    *dnx_data_l3_routing_enablers_routing_enable_vector_length_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_routing_enablers_routing_enable_action_support_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_routing_enablers_routing_enable_action_profile_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_routing_enablers_routing_enable_indication_nof_bits_per_layer_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_routing_enablers_routing_enable_nof_profiles_per_layer_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_routing_enablers_routing_enabled_action_profile_id_get_f) (
    int unit);


typedef const dnx_data_l3_routing_enablers_layer_enablers_by_id_t *(
    *dnx_data_l3_routing_enablers_layer_enablers_by_id_get_f) (
    int unit,
    int idx);


typedef const dnx_data_l3_routing_enablers_layer_enablers_t *(
    *dnx_data_l3_routing_enablers_layer_enablers_get_f) (
    int unit,
    int layer_type);



typedef struct
{
    
    dnx_data_l3_routing_enablers_feature_get_f feature_get;
    
    dnx_data_l3_routing_enablers_routing_enable_vector_length_get_f routing_enable_vector_length_get;
    
    dnx_data_l3_routing_enablers_routing_enable_action_support_get_f routing_enable_action_support_get;
    
    dnx_data_l3_routing_enablers_routing_enable_action_profile_size_get_f routing_enable_action_profile_size_get;
    
    dnx_data_l3_routing_enablers_routing_enable_indication_nof_bits_per_layer_get_f routing_enable_indication_nof_bits_per_layer_get;
    
    dnx_data_l3_routing_enablers_routing_enable_nof_profiles_per_layer_get_f routing_enable_nof_profiles_per_layer_get;
    
    dnx_data_l3_routing_enablers_routing_enabled_action_profile_id_get_f routing_enabled_action_profile_id_get;
    
    dnx_data_l3_routing_enablers_layer_enablers_by_id_get_f layer_enablers_by_id_get;
    
    dnxc_data_table_info_get_f layer_enablers_by_id_info_get;
    
    dnx_data_l3_routing_enablers_layer_enablers_get_f layer_enablers_get;
    
    dnxc_data_table_info_get_f layer_enablers_info_get;
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
    int prefix_len_non_def[DNX_DATA_MAX_L3_FWD_NOF_LPM_STRENGTH_PROFILES];
    int prefix_len_def[DNX_DATA_MAX_L3_FWD_NOF_LPM_STRENGTH_PROFILES];
    int entry_strength_non_def[DNX_DATA_MAX_L3_FWD_NOF_LPM_STRENGTH_PROFILES];
    int entry_strength_def[DNX_DATA_MAX_L3_FWD_NOF_LPM_STRENGTH_PROFILES];
} dnx_data_l3_fwd_lpm_profile_to_entry_strength_t;


typedef struct
{
    dbal_tables_e tables[L3_MAX_NOF_ROUTING_TABLES_PER_PROTOCOL_VERSION+1];
} dnx_data_l3_fwd_routing_tables_t;


typedef struct
{
    dbal_tables_e tables[6];
    uint32 count;
} dnx_data_l3_fwd_mc_tables_t;


typedef struct
{
    dnx_ipmc_config_compression_mode_e mode;
    dbal_tables_e dbal_table;
    dbal_physical_tables_e physical_table;
    dbal_fields_e result_field;
} dnx_data_l3_fwd_ipmc_compression_mode_t;


typedef struct
{
    uint32 dbal_table[DNX_DATA_MAX_MDB_KAPS_NOF_APP_IDS];
    uint32 default_prefix_length[DNX_DATA_MAX_MDB_KAPS_NOF_APP_IDS];
} dnx_data_l3_fwd_lpm_default_prefix_length_info_t;


typedef struct
{
    uint32 dbal_table[DNX_DATA_MAX_MDB_KAPS_NOF_APP_IDS];
    dnx_l3_kaps_lpm_strength_profiles_e lpm_profile[DNX_DATA_MAX_MDB_KAPS_NOF_APP_IDS];
} dnx_data_l3_fwd_lpm_app_db_to_strength_profile_t;



typedef enum
{
    dnx_data_l3_fwd_ipmc_rif_key_participation,
    dnx_data_l3_fwd_host_entry_support,
    dnx_data_l3_fwd_tcam_entry_support,
    dnx_data_l3_fwd_ipmc_config_cmprs_vrf_group,
    dnx_data_l3_fwd_lpm_default_entry_set_by_prefix,
    dnx_data_l3_fwd_kaps_lpm_strength_profile_map_support,
    dnx_data_l3_fwd_native_arp_rif_encap_access_support,

    
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
    *dnx_data_l3_fwd_compressed_sip_svl_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fwd_nof_compressed_svl_sip_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fwd_lpm_strength_profiles_support_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fwd_nof_lpm_strength_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fwd_compressed_sip_ivl_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fwd_nof_compressed_ivl_sip_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fwd_uc_supported_route_flags_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fwd_uc_supported_route_flags2_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fwd_uc_supported_kbp_route_flags_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fwd_uc_supported_kbp_route_flags2_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fwd_ipmc_supported_flags_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fwd_ipmc_config_supported_flags_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fwd_ipmc_config_supported_traverse_flags_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_fwd_last_kaps_intf_id_get_f) (
    int unit);


typedef const dnx_data_l3_fwd_lpm_profile_to_entry_strength_t *(
    *dnx_data_l3_fwd_lpm_profile_to_entry_strength_get_f) (
    int unit,
    int lpm_profile,
    int kaps_intf);


typedef const dnx_data_l3_fwd_routing_tables_t *(
    *dnx_data_l3_fwd_routing_tables_get_f) (
    int unit,
    int ip_version);


typedef const dnx_data_l3_fwd_mc_tables_t *(
    *dnx_data_l3_fwd_mc_tables_get_f) (
    int unit,
    int ip_version,
    int route_enable);


typedef const dnx_data_l3_fwd_ipmc_compression_mode_t *(
    *dnx_data_l3_fwd_ipmc_compression_mode_get_f) (
    int unit,
    int route_enable,
    int dbal_table_type);


typedef const dnx_data_l3_fwd_lpm_default_prefix_length_info_t *(
    *dnx_data_l3_fwd_lpm_default_prefix_length_info_get_f) (
    int unit,
    int physical_db_id);


typedef const dnx_data_l3_fwd_lpm_app_db_to_strength_profile_t *(
    *dnx_data_l3_fwd_lpm_app_db_to_strength_profile_get_f) (
    int unit,
    int kaps_intf);



typedef struct
{
    
    dnx_data_l3_fwd_feature_get_f feature_get;
    
    dnx_data_l3_fwd_flp_fragment_support_get_f flp_fragment_support_get;
    
    dnx_data_l3_fwd_max_mc_group_lpm_get_f max_mc_group_lpm_get;
    
    dnx_data_l3_fwd_max_mc_group_em_get_f max_mc_group_em_get;
    
    dnx_data_l3_fwd_compressed_sip_svl_size_get_f compressed_sip_svl_size_get;
    
    dnx_data_l3_fwd_nof_compressed_svl_sip_get_f nof_compressed_svl_sip_get;
    
    dnx_data_l3_fwd_lpm_strength_profiles_support_get_f lpm_strength_profiles_support_get;
    
    dnx_data_l3_fwd_nof_lpm_strength_profiles_get_f nof_lpm_strength_profiles_get;
    
    dnx_data_l3_fwd_compressed_sip_ivl_size_get_f compressed_sip_ivl_size_get;
    
    dnx_data_l3_fwd_nof_compressed_ivl_sip_get_f nof_compressed_ivl_sip_get;
    
    dnx_data_l3_fwd_uc_supported_route_flags_get_f uc_supported_route_flags_get;
    
    dnx_data_l3_fwd_uc_supported_route_flags2_get_f uc_supported_route_flags2_get;
    
    dnx_data_l3_fwd_uc_supported_kbp_route_flags_get_f uc_supported_kbp_route_flags_get;
    
    dnx_data_l3_fwd_uc_supported_kbp_route_flags2_get_f uc_supported_kbp_route_flags2_get;
    
    dnx_data_l3_fwd_ipmc_supported_flags_get_f ipmc_supported_flags_get;
    
    dnx_data_l3_fwd_ipmc_config_supported_flags_get_f ipmc_config_supported_flags_get;
    
    dnx_data_l3_fwd_ipmc_config_supported_traverse_flags_get_f ipmc_config_supported_traverse_flags_get;
    
    dnx_data_l3_fwd_last_kaps_intf_id_get_f last_kaps_intf_id_get;
    
    dnx_data_l3_fwd_lpm_profile_to_entry_strength_get_f lpm_profile_to_entry_strength_get;
    
    dnxc_data_table_info_get_f lpm_profile_to_entry_strength_info_get;
    
    dnx_data_l3_fwd_routing_tables_get_f routing_tables_get;
    
    dnxc_data_table_info_get_f routing_tables_info_get;
    
    dnx_data_l3_fwd_mc_tables_get_f mc_tables_get;
    
    dnxc_data_table_info_get_f mc_tables_info_get;
    
    dnx_data_l3_fwd_ipmc_compression_mode_get_f ipmc_compression_mode_get;
    
    dnxc_data_table_info_get_f ipmc_compression_mode_info_get;
    
    dnx_data_l3_fwd_lpm_default_prefix_length_info_get_f lpm_default_prefix_length_info_get;
    
    dnxc_data_table_info_get_f lpm_default_prefix_length_info_info_get;
    
    dnx_data_l3_fwd_lpm_app_db_to_strength_profile_get_f lpm_app_db_to_strength_profile_get;
    
    dnxc_data_table_info_get_f lpm_app_db_to_strength_profile_info_get;
} dnx_data_if_l3_fwd_t;






typedef struct
{
    uint32 nof_entries;
    uint32 entry_size_in_bits;
} dnx_data_l3_ecmp_consistent_tables_info_t;



typedef enum
{
    dnx_data_l3_ecmp_ecmp_group_profile_multiple_result_types_support,
    dnx_data_l3_ecmp_cbts_implementation_using_field_support,

    
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
    *dnx_data_l3_ecmp_nof_ecmp_basic_mode_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_ecmp_max_ecmp_basic_mode_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_ecmp_max_ecmp_extended_mode_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_ecmp_nof_extended_ecmp_per_failover_bank_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_ecmp_nof_failover_banks_per_extended_ecmp_bank_get_f) (
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
    *dnx_data_l3_ecmp_ecmp_bank_size_get_f) (
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


typedef uint32(
    *dnx_data_l3_ecmp_consistent_bank_size_in_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_ecmp_ecmp_small_consistent_group_nof_entries_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_ecmp_ecmp_small_consistent_group_entry_size_in_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_ecmp_nof_consistent_resources_get_f) (
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
    
    dnx_data_l3_ecmp_nof_ecmp_basic_mode_get_f nof_ecmp_basic_mode_get;
    
    dnx_data_l3_ecmp_max_ecmp_basic_mode_get_f max_ecmp_basic_mode_get;
    
    dnx_data_l3_ecmp_max_ecmp_extended_mode_get_f max_ecmp_extended_mode_get;
    
    dnx_data_l3_ecmp_nof_extended_ecmp_per_failover_bank_get_f nof_extended_ecmp_per_failover_bank_get;
    
    dnx_data_l3_ecmp_nof_failover_banks_per_extended_ecmp_bank_get_f nof_failover_banks_per_extended_ecmp_bank_get;
    
    dnx_data_l3_ecmp_nof_group_profiles_per_hierarchy_get_f nof_group_profiles_per_hierarchy_get;
    
    dnx_data_l3_ecmp_member_table_nof_rows_size_in_bits_get_f member_table_nof_rows_size_in_bits_get;
    
    dnx_data_l3_ecmp_member_table_address_size_in_bits_get_f member_table_address_size_in_bits_get;
    
    dnx_data_l3_ecmp_ecmp_bank_size_get_f ecmp_bank_size_get;
    
    dnx_data_l3_ecmp_nof_ecmp_banks_basic_get_f nof_ecmp_banks_basic_get;
    
    dnx_data_l3_ecmp_nof_ecmp_banks_extended_get_f nof_ecmp_banks_extended_get;
    
    dnx_data_l3_ecmp_nof_ecmp_get_f nof_ecmp_get;
    
    dnx_data_l3_ecmp_profile_id_size_get_f profile_id_size_get;
    
    dnx_data_l3_ecmp_profile_id_offset_get_f profile_id_offset_get;
    
    dnx_data_l3_ecmp_tunnel_priority_support_get_f tunnel_priority_support_get;
    
    dnx_data_l3_ecmp_nof_tunnel_priority_map_profiles_get_f nof_tunnel_priority_map_profiles_get;
    
    dnx_data_l3_ecmp_tunnel_priority_field_width_get_f tunnel_priority_field_width_get;
    
    dnx_data_l3_ecmp_tunnel_priority_index_field_width_get_f tunnel_priority_index_field_width_get;
    
    dnx_data_l3_ecmp_consistent_mem_row_size_in_bits_get_f consistent_mem_row_size_in_bits_get;
    
    dnx_data_l3_ecmp_consistent_bank_size_in_bits_get_f consistent_bank_size_in_bits_get;
    
    dnx_data_l3_ecmp_ecmp_small_consistent_group_nof_entries_get_f ecmp_small_consistent_group_nof_entries_get;
    
    dnx_data_l3_ecmp_ecmp_small_consistent_group_entry_size_in_bits_get_f ecmp_small_consistent_group_entry_size_in_bits_get;
    
    dnx_data_l3_ecmp_nof_consistent_resources_get_f nof_consistent_resources_get;
    
    dnx_data_l3_ecmp_consistent_tables_info_get_f consistent_tables_info_get;
    
    dnxc_data_table_info_get_f consistent_tables_info_info_get;
} dnx_data_if_l3_ecmp_t;







typedef enum
{
    dnx_data_l3_vip_ecmp_supported,

    
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

    
    _dnx_data_l3_wcmp_feature_nof
} dnx_data_l3_wcmp_feature_e;



typedef int(
    *dnx_data_l3_wcmp_feature_get_f) (
    int unit,
    dnx_data_l3_wcmp_feature_e feature);


typedef uint32(
    *dnx_data_l3_wcmp_group_size_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_wcmp_max_member_weight_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_wcmp_member_weight_quant_factor_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_l3_wcmp_feature_get_f feature_get;
    
    dnx_data_l3_wcmp_group_size_nof_bits_get_f group_size_nof_bits_get;
    
    dnx_data_l3_wcmp_max_member_weight_get_f max_member_weight_get;
    
    dnx_data_l3_wcmp_member_weight_quant_factor_get_f member_weight_quant_factor_get;
} dnx_data_if_l3_wcmp_t;






typedef struct
{
    dbal_tables_e table[2];
} dnx_data_l3_vrrp_vrrp_tables_t;


typedef struct
{
    dbal_physical_tables_e data_base[DNX_DATA_MAX_L3_VRRP_NOF_DBS];
} dnx_data_l3_vrrp_data_bases_t;



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
    *dnx_data_l3_vrrp_nof_tcam_entries_used_by_exem_get_f) (
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


typedef uint32(
    *dnx_data_l3_vrrp_is_mapped_pp_port_used_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l3_vrrp_nof_dbs_get_f) (
    int unit);


typedef const dnx_data_l3_vrrp_vrrp_tables_t *(
    *dnx_data_l3_vrrp_vrrp_tables_get_f) (
    int unit,
    int table_type);


typedef const dnx_data_l3_vrrp_data_bases_t *(
    *dnx_data_l3_vrrp_data_bases_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_l3_vrrp_feature_get_f feature_get;
    
    dnx_data_l3_vrrp_nof_protocol_groups_get_f nof_protocol_groups_get;
    
    dnx_data_l3_vrrp_nof_tcam_entries_get_f nof_tcam_entries_get;
    
    dnx_data_l3_vrrp_nof_vsi_tcam_entries_get_f nof_vsi_tcam_entries_get;
    
    dnx_data_l3_vrrp_nof_tcam_entries_used_by_exem_get_f nof_tcam_entries_used_by_exem_get;
    
    dnx_data_l3_vrrp_exem_vrid_ipv4_tcam_index_get_f exem_vrid_ipv4_tcam_index_get;
    
    dnx_data_l3_vrrp_exem_vrid_ipv6_tcam_index_get_f exem_vrid_ipv6_tcam_index_get;
    
    dnx_data_l3_vrrp_exem_default_tcam_index_get_f exem_default_tcam_index_get;
    
    dnx_data_l3_vrrp_exem_default_tcam_index_msb_get_f exem_default_tcam_index_msb_get;
    
    dnx_data_l3_vrrp_vrrp_default_num_entries_get_f vrrp_default_num_entries_get;
    
    dnx_data_l3_vrrp_is_mapped_pp_port_used_get_f is_mapped_pp_port_used_get;
    
    dnx_data_l3_vrrp_nof_dbs_get_f nof_dbs_get;
    
    dnx_data_l3_vrrp_vrrp_tables_get_f vrrp_tables_get;
    
    dnxc_data_table_info_get_f vrrp_tables_info_get;
    
    dnx_data_l3_vrrp_data_bases_get_f data_bases_get;
    
    dnxc_data_table_info_get_f data_bases_info_get;
} dnx_data_if_l3_vrrp_t;







typedef enum
{
    dnx_data_l3_feature_ipv6_mc_compatible_dmac,
    dnx_data_l3_feature_fec_hit_bit,
    dnx_data_l3_feature_fer_fec_granularity_double_size,
    dnx_data_l3_feature_mc_bridge_fallback,
    dnx_data_l3_feature_nat_on_a_stick,
    dnx_data_l3_feature_ingress_frag_not_first,
    dnx_data_l3_feature_public_routing_support,
    dnx_data_l3_feature_separate_fwd_rpf_dbs,
    dnx_data_l3_feature_default_fec_limitation,
    dnx_data_l3_feature_wcmp_support,
    dnx_data_l3_feature_default_kaps_interface,
    dnx_data_l3_feature_lpm_uses_lart,
    dnx_data_l3_feature_my_mac_prefix_0_is_invalid,
    dnx_data_l3_feature_vpbr_used_for_bfd,
    dnx_data_l3_feature_vpbr_tunnel_available,
    dnx_data_l3_feature_mc_bridge_fallback_outlifs_set,
    dnx_data_l3_feature_rpf_default_route_supported,
    dnx_data_l3_feature_anti_spoofing_support,
    dnx_data_l3_feature_l3_ipv6_mc_tcam_not_used,
    dnx_data_l3_feature_fec_rpf_protection_supported,
    dnx_data_l3_feature_mc_bridge_fallback_use_default_destination,
    dnx_data_l3_feature_nof_layers_to_terminate_support,

    
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
    
    dnx_data_if_l3_fer_t fer;
    
    dnx_data_if_l3_source_address_t source_address;
    
    dnx_data_if_l3_vrf_t vrf;
    
    dnx_data_if_l3_routing_enablers_t routing_enablers;
    
    dnx_data_if_l3_rif_t rif;
    
    dnx_data_if_l3_fwd_t fwd;
    
    dnx_data_if_l3_ecmp_t ecmp;
    
    dnx_data_if_l3_vip_ecmp_t vip_ecmp;
    
    dnx_data_if_l3_wcmp_t wcmp;
    
    dnx_data_if_l3_vrrp_t vrrp;
    
    dnx_data_if_l3_feature_t feature;
} dnx_data_if_l3_t;




extern dnx_data_if_l3_t dnx_data_l3;


#endif 

