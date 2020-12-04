

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_TRAP_H_

#define _DNX_DATA_INTERNAL_TRAP_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trap.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_trap_submodule_ingress,
    dnx_data_trap_submodule_erpp,
    dnx_data_trap_submodule_etpp,
    dnx_data_trap_submodule_strength,
    dnx_data_trap_submodule_register_name_changes,

    
    _dnx_data_trap_submodule_nof
} dnx_data_trap_submodule_e;








int dnx_data_trap_ingress_feature_get(
    int unit,
    dnx_data_trap_ingress_feature_e feature);



typedef enum
{
    dnx_data_trap_ingress_define_protocol_traps_init_mode,
    dnx_data_trap_ingress_define_protocol_traps_lif_profile_starting_offset,
    dnx_data_trap_ingress_define_nof_predefeind_traps,
    dnx_data_trap_ingress_define_nof_user_defined_traps,
    dnx_data_trap_ingress_define_nof_1588_traps,
    dnx_data_trap_ingress_define_nof_entries_action_table,
    dnx_data_trap_ingress_define_nof_protocol_trap_profiles,
    dnx_data_trap_ingress_define_nof_recycle_cmds,
    dnx_data_trap_ingress_define_nof_programmable_traps,
    dnx_data_trap_ingress_define_nof_lif_traps,
    dnx_data_trap_ingress_define_nof_icmp_compressed_types,
    dnx_data_trap_ingress_define_nof_stat_objs,
    dnx_data_trap_ingress_define_max_nof_stat_objs_to_overwrite,
    dnx_data_trap_ingress_define_nof_error_code_profiles,
    dnx_data_trap_ingress_define_nof_sci_profiles,
    dnx_data_trap_ingress_define_nof_error_codes,
    dnx_data_trap_ingress_define_nof_scis,

    
    _dnx_data_trap_ingress_define_nof
} dnx_data_trap_ingress_define_e;



uint32 dnx_data_trap_ingress_protocol_traps_init_mode_get(
    int unit);


uint32 dnx_data_trap_ingress_protocol_traps_lif_profile_starting_offset_get(
    int unit);


uint32 dnx_data_trap_ingress_nof_predefeind_traps_get(
    int unit);


uint32 dnx_data_trap_ingress_nof_user_defined_traps_get(
    int unit);


uint32 dnx_data_trap_ingress_nof_1588_traps_get(
    int unit);


uint32 dnx_data_trap_ingress_nof_entries_action_table_get(
    int unit);


uint32 dnx_data_trap_ingress_nof_protocol_trap_profiles_get(
    int unit);


uint32 dnx_data_trap_ingress_nof_recycle_cmds_get(
    int unit);


uint32 dnx_data_trap_ingress_nof_programmable_traps_get(
    int unit);


uint32 dnx_data_trap_ingress_nof_lif_traps_get(
    int unit);


uint32 dnx_data_trap_ingress_nof_icmp_compressed_types_get(
    int unit);


uint32 dnx_data_trap_ingress_nof_stat_objs_get(
    int unit);


uint32 dnx_data_trap_ingress_max_nof_stat_objs_to_overwrite_get(
    int unit);


uint32 dnx_data_trap_ingress_nof_error_code_profiles_get(
    int unit);


uint32 dnx_data_trap_ingress_nof_sci_profiles_get(
    int unit);


uint32 dnx_data_trap_ingress_nof_error_codes_get(
    int unit);


uint32 dnx_data_trap_ingress_nof_scis_get(
    int unit);



typedef enum
{

    
    _dnx_data_trap_ingress_table_nof
} dnx_data_trap_ingress_table_e;









int dnx_data_trap_erpp_feature_get(
    int unit,
    dnx_data_trap_erpp_feature_e feature);



typedef enum
{
    dnx_data_trap_erpp_define_nof_erpp_app_traps,
    dnx_data_trap_erpp_define_nof_erpp_forward_profiles,
    dnx_data_trap_erpp_define_nof_erpp_user_configurable_profiles,
    dnx_data_trap_erpp_define_nof_erpp_snif_profiles,

    
    _dnx_data_trap_erpp_define_nof
} dnx_data_trap_erpp_define_e;



uint32 dnx_data_trap_erpp_nof_erpp_app_traps_get(
    int unit);


uint32 dnx_data_trap_erpp_nof_erpp_forward_profiles_get(
    int unit);


uint32 dnx_data_trap_erpp_nof_erpp_user_configurable_profiles_get(
    int unit);


uint32 dnx_data_trap_erpp_nof_erpp_snif_profiles_get(
    int unit);



typedef enum
{

    
    _dnx_data_trap_erpp_table_nof
} dnx_data_trap_erpp_table_e;









int dnx_data_trap_etpp_feature_get(
    int unit,
    dnx_data_trap_etpp_feature_e feature);



typedef enum
{
    dnx_data_trap_etpp_define_nof_etpp_app_traps,
    dnx_data_trap_etpp_define_nof_etpp_user_configurable_profiles,
    dnx_data_trap_etpp_define_nof_etpp_trap_profiles,
    dnx_data_trap_etpp_define_nof_etpp_oam_traps,
    dnx_data_trap_etpp_define_nof_etpp_snif_profiles,
    dnx_data_trap_etpp_define_nof_mtu_profiles,
    dnx_data_trap_etpp_define_nof_etpp_lif_traps,
    dnx_data_trap_etpp_define_max_mtu_value,
    dnx_data_trap_etpp_define_trap_context_port_profile_4b,

    
    _dnx_data_trap_etpp_define_nof
} dnx_data_trap_etpp_define_e;



uint32 dnx_data_trap_etpp_nof_etpp_app_traps_get(
    int unit);


uint32 dnx_data_trap_etpp_nof_etpp_user_configurable_profiles_get(
    int unit);


uint32 dnx_data_trap_etpp_nof_etpp_trap_profiles_get(
    int unit);


uint32 dnx_data_trap_etpp_nof_etpp_oam_traps_get(
    int unit);


uint32 dnx_data_trap_etpp_nof_etpp_snif_profiles_get(
    int unit);


uint32 dnx_data_trap_etpp_nof_mtu_profiles_get(
    int unit);


uint32 dnx_data_trap_etpp_nof_etpp_lif_traps_get(
    int unit);


uint32 dnx_data_trap_etpp_max_mtu_value_get(
    int unit);


uint32 dnx_data_trap_etpp_trap_context_port_profile_4b_get(
    int unit);



typedef enum
{

    
    _dnx_data_trap_etpp_table_nof
} dnx_data_trap_etpp_table_e;









int dnx_data_trap_strength_feature_get(
    int unit,
    dnx_data_trap_strength_feature_e feature);



typedef enum
{
    dnx_data_trap_strength_define_max_strength,
    dnx_data_trap_strength_define_max_snp_strength,
    dnx_data_trap_strength_define_max_ingress_to_egress_compressed_strength,
    dnx_data_trap_strength_define_default_trap_strength,
    dnx_data_trap_strength_define_default_snoop_strength,

    
    _dnx_data_trap_strength_define_nof
} dnx_data_trap_strength_define_e;



uint32 dnx_data_trap_strength_max_strength_get(
    int unit);


uint32 dnx_data_trap_strength_max_snp_strength_get(
    int unit);


uint32 dnx_data_trap_strength_max_ingress_to_egress_compressed_strength_get(
    int unit);


uint32 dnx_data_trap_strength_default_trap_strength_get(
    int unit);


uint32 dnx_data_trap_strength_default_snoop_strength_get(
    int unit);



typedef enum
{

    
    _dnx_data_trap_strength_table_nof
} dnx_data_trap_strength_table_e;









int dnx_data_trap_register_name_changes_feature_get(
    int unit,
    dnx_data_trap_register_name_changes_feature_e feature);



typedef enum
{
    dnx_data_trap_register_name_changes_define_out_lif_mcdb_ptr_signal,
    dnx_data_trap_register_name_changes_define_added_n,
    dnx_data_trap_register_name_changes_define_per_pp_port_change,
    dnx_data_trap_register_name_changes_define_visibility_signal_0,

    
    _dnx_data_trap_register_name_changes_define_nof
} dnx_data_trap_register_name_changes_define_e;



uint32 dnx_data_trap_register_name_changes_out_lif_mcdb_ptr_signal_get(
    int unit);


uint32 dnx_data_trap_register_name_changes_added_n_get(
    int unit);


uint32 dnx_data_trap_register_name_changes_per_pp_port_change_get(
    int unit);


uint32 dnx_data_trap_register_name_changes_visibility_signal_0_get(
    int unit);



typedef enum
{

    
    _dnx_data_trap_register_name_changes_table_nof
} dnx_data_trap_register_name_changes_table_e;






shr_error_e dnx_data_trap_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

