

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_TRAP_H_

#define _DNX_DATA_TRAP_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_trap.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_trap_init(
    int unit);







typedef enum
{
    
    dnx_data_trap_ingress_ndp_trap_en,
    
    dnx_data_trap_ingress_trap_in_lif_profile_mapping,
    
    dnx_data_trap_ingress_svtag_trap_en,
    
    dnx_data_trap_ingress_oam_offset_actions,
    
    dnx_data_trap_ingress_snoop_code_clear_action,
    
    dnx_data_trap_ingress_mirror_code_clear_action,
    
    dnx_data_trap_ingress_stat_sampling_code_clear_action,
    
    dnx_data_trap_ingress_ipv6_mapped_dest,
    
    dnx_data_trap_ingress_mpls_termination_fail_over,
    dnx_data_trap_ingress_checksum_feature_fixed,
    dnx_data_trap_ingress_header_size_err_blocking,
    dnx_data_trap_ingress_l4_trap_res_before_l3,
    dnx_data_trap_ingress_comp_mc_before_routing_traps,
    
    dnx_data_trap_ingress_soft_mem_err_over_dest_err,
    
    dnx_data_trap_ingress_eth_default_proc_fix,
    
    dnx_data_trap_ingress_fwd_domain_mode_trap,
    
    dnx_data_trap_ingress_sa_equals_zero_trap,
    dnx_data_trap_ingress_latency_measurement_bug,
    
    dnx_data_trap_ingress_ser_hw_support,
    
    dnx_data_trap_ingress_compatible_mc_hw_support,

    
    _dnx_data_trap_ingress_feature_nof
} dnx_data_trap_ingress_feature_e;



typedef int(
    *dnx_data_trap_ingress_feature_get_f) (
    int unit,
    dnx_data_trap_ingress_feature_e feature);


typedef uint32(
    *dnx_data_trap_ingress_protocol_traps_init_mode_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trap_ingress_protocol_traps_lif_profile_starting_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trap_ingress_nof_predefeind_traps_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trap_ingress_nof_user_defined_traps_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trap_ingress_nof_1588_traps_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trap_ingress_nof_entries_action_table_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trap_ingress_nof_protocol_trap_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trap_ingress_nof_recycle_cmds_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trap_ingress_nof_programmable_traps_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trap_ingress_nof_lif_traps_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trap_ingress_nof_icmp_compressed_types_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trap_ingress_nof_stat_objs_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trap_ingress_max_nof_stat_objs_to_overwrite_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trap_ingress_nof_error_code_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trap_ingress_nof_sci_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trap_ingress_nof_error_codes_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trap_ingress_nof_scis_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_trap_ingress_feature_get_f feature_get;
    
    dnx_data_trap_ingress_protocol_traps_init_mode_get_f protocol_traps_init_mode_get;
    
    dnx_data_trap_ingress_protocol_traps_lif_profile_starting_offset_get_f protocol_traps_lif_profile_starting_offset_get;
    
    dnx_data_trap_ingress_nof_predefeind_traps_get_f nof_predefeind_traps_get;
    
    dnx_data_trap_ingress_nof_user_defined_traps_get_f nof_user_defined_traps_get;
    
    dnx_data_trap_ingress_nof_1588_traps_get_f nof_1588_traps_get;
    
    dnx_data_trap_ingress_nof_entries_action_table_get_f nof_entries_action_table_get;
    
    dnx_data_trap_ingress_nof_protocol_trap_profiles_get_f nof_protocol_trap_profiles_get;
    
    dnx_data_trap_ingress_nof_recycle_cmds_get_f nof_recycle_cmds_get;
    
    dnx_data_trap_ingress_nof_programmable_traps_get_f nof_programmable_traps_get;
    
    dnx_data_trap_ingress_nof_lif_traps_get_f nof_lif_traps_get;
    
    dnx_data_trap_ingress_nof_icmp_compressed_types_get_f nof_icmp_compressed_types_get;
    
    dnx_data_trap_ingress_nof_stat_objs_get_f nof_stat_objs_get;
    
    dnx_data_trap_ingress_max_nof_stat_objs_to_overwrite_get_f max_nof_stat_objs_to_overwrite_get;
    
    dnx_data_trap_ingress_nof_error_code_profiles_get_f nof_error_code_profiles_get;
    
    dnx_data_trap_ingress_nof_sci_profiles_get_f nof_sci_profiles_get;
    
    dnx_data_trap_ingress_nof_error_codes_get_f nof_error_codes_get;
    
    dnx_data_trap_ingress_nof_scis_get_f nof_scis_get;
} dnx_data_if_trap_ingress_t;







typedef enum
{
    
    dnx_data_trap_erpp_unknown_da_trap_en,
    
    dnx_data_trap_erpp_glem_not_found_trap,

    
    _dnx_data_trap_erpp_feature_nof
} dnx_data_trap_erpp_feature_e;



typedef int(
    *dnx_data_trap_erpp_feature_get_f) (
    int unit,
    dnx_data_trap_erpp_feature_e feature);


typedef uint32(
    *dnx_data_trap_erpp_nof_erpp_app_traps_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trap_erpp_nof_erpp_forward_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trap_erpp_nof_erpp_user_configurable_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trap_erpp_nof_erpp_snif_profiles_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_trap_erpp_feature_get_f feature_get;
    
    dnx_data_trap_erpp_nof_erpp_app_traps_get_f nof_erpp_app_traps_get;
    
    dnx_data_trap_erpp_nof_erpp_forward_profiles_get_f nof_erpp_forward_profiles_get;
    
    dnx_data_trap_erpp_nof_erpp_user_configurable_profiles_get_f nof_erpp_user_configurable_profiles_get;
    
    dnx_data_trap_erpp_nof_erpp_snif_profiles_get_f nof_erpp_snif_profiles_get;
} dnx_data_if_trap_erpp_t;







typedef enum
{
    
    dnx_data_trap_etpp_glem_hw_support,

    
    _dnx_data_trap_etpp_feature_nof
} dnx_data_trap_etpp_feature_e;



typedef int(
    *dnx_data_trap_etpp_feature_get_f) (
    int unit,
    dnx_data_trap_etpp_feature_e feature);


typedef uint32(
    *dnx_data_trap_etpp_nof_etpp_app_traps_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trap_etpp_nof_etpp_user_configurable_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trap_etpp_nof_etpp_trap_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trap_etpp_nof_etpp_oam_traps_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trap_etpp_nof_etpp_snif_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trap_etpp_nof_mtu_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trap_etpp_nof_etpp_lif_traps_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trap_etpp_max_mtu_value_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trap_etpp_trap_context_port_profile_4b_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_trap_etpp_feature_get_f feature_get;
    
    dnx_data_trap_etpp_nof_etpp_app_traps_get_f nof_etpp_app_traps_get;
    
    dnx_data_trap_etpp_nof_etpp_user_configurable_profiles_get_f nof_etpp_user_configurable_profiles_get;
    
    dnx_data_trap_etpp_nof_etpp_trap_profiles_get_f nof_etpp_trap_profiles_get;
    
    dnx_data_trap_etpp_nof_etpp_oam_traps_get_f nof_etpp_oam_traps_get;
    
    dnx_data_trap_etpp_nof_etpp_snif_profiles_get_f nof_etpp_snif_profiles_get;
    
    dnx_data_trap_etpp_nof_mtu_profiles_get_f nof_mtu_profiles_get;
    
    dnx_data_trap_etpp_nof_etpp_lif_traps_get_f nof_etpp_lif_traps_get;
    
    dnx_data_trap_etpp_max_mtu_value_get_f max_mtu_value_get;
    
    dnx_data_trap_etpp_trap_context_port_profile_4b_get_f trap_context_port_profile_4b_get;
} dnx_data_if_trap_etpp_t;







typedef enum
{

    
    _dnx_data_trap_strength_feature_nof
} dnx_data_trap_strength_feature_e;



typedef int(
    *dnx_data_trap_strength_feature_get_f) (
    int unit,
    dnx_data_trap_strength_feature_e feature);


typedef uint32(
    *dnx_data_trap_strength_max_strength_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trap_strength_max_snp_strength_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trap_strength_max_ingress_to_egress_compressed_strength_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trap_strength_default_trap_strength_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trap_strength_default_snoop_strength_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_trap_strength_feature_get_f feature_get;
    
    dnx_data_trap_strength_max_strength_get_f max_strength_get;
    
    dnx_data_trap_strength_max_snp_strength_get_f max_snp_strength_get;
    
    dnx_data_trap_strength_max_ingress_to_egress_compressed_strength_get_f max_ingress_to_egress_compressed_strength_get;
    
    dnx_data_trap_strength_default_trap_strength_get_f default_trap_strength_get;
    
    dnx_data_trap_strength_default_snoop_strength_get_f default_snoop_strength_get;
} dnx_data_if_trap_strength_t;







typedef enum
{

    
    _dnx_data_trap_register_name_changes_feature_nof
} dnx_data_trap_register_name_changes_feature_e;



typedef int(
    *dnx_data_trap_register_name_changes_feature_get_f) (
    int unit,
    dnx_data_trap_register_name_changes_feature_e feature);


typedef uint32(
    *dnx_data_trap_register_name_changes_out_lif_mcdb_ptr_signal_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trap_register_name_changes_added_n_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trap_register_name_changes_per_pp_port_change_get_f) (
    int unit);


typedef uint32(
    *dnx_data_trap_register_name_changes_visibility_signal_0_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_trap_register_name_changes_feature_get_f feature_get;
    
    dnx_data_trap_register_name_changes_out_lif_mcdb_ptr_signal_get_f out_lif_mcdb_ptr_signal_get;
    
    dnx_data_trap_register_name_changes_added_n_get_f added_n_get;
    
    dnx_data_trap_register_name_changes_per_pp_port_change_get_f per_pp_port_change_get;
    
    dnx_data_trap_register_name_changes_visibility_signal_0_get_f visibility_signal_0_get;
} dnx_data_if_trap_register_name_changes_t;





typedef struct
{
    
    dnx_data_if_trap_ingress_t ingress;
    
    dnx_data_if_trap_erpp_t erpp;
    
    dnx_data_if_trap_etpp_t etpp;
    
    dnx_data_if_trap_strength_t strength;
    
    dnx_data_if_trap_register_name_changes_t register_name_changes;
} dnx_data_if_trap_t;




extern dnx_data_if_trap_t dnx_data_trap;


#endif 

