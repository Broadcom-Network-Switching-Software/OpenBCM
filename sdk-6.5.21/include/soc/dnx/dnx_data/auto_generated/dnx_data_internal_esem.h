

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_ESEM_H_

#define _DNX_DATA_INTERNAL_ESEM_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_esem_submodule_access_cmd,
    dnx_data_esem_submodule_default_result_profile,
    dnx_data_esem_submodule_feature,

    
    _dnx_data_esem_submodule_nof
} dnx_data_esem_submodule_e;








int dnx_data_esem_access_cmd_feature_get(
    int unit,
    dnx_data_esem_access_cmd_feature_e feature);



typedef enum
{
    dnx_data_esem_access_cmd_define_nof_esem_accesses,
    dnx_data_esem_access_cmd_define_app_di_id_size_in_bits,
    dnx_data_esem_access_cmd_define_no_action,
    dnx_data_esem_access_cmd_define_default_native,
    dnx_data_esem_access_cmd_define_default_ac,
    dnx_data_esem_access_cmd_define_sflow_sample_interface,
    dnx_data_esem_access_cmd_define_ip_tunnel_no_action,
    dnx_data_esem_access_cmd_define_nof_esem_cmd_default_profiles,
    dnx_data_esem_access_cmd_define_nof_cmds_size_in_bits,
    dnx_data_esem_access_cmd_define_nof_cmds,
    dnx_data_esem_access_cmd_define_min_size_for_esem_cmd,
    dnx_data_esem_access_cmd_define_esem_cmd_arr_prefix_gre_any_ipv4,
    dnx_data_esem_access_cmd_define_esem_cmd_arr_prefix_vxlan_gpe,
    dnx_data_esem_access_cmd_define_esem_cmd_arr_prefix_ip_tunnel_no_action,

    
    _dnx_data_esem_access_cmd_define_nof
} dnx_data_esem_access_cmd_define_e;



uint32 dnx_data_esem_access_cmd_nof_esem_accesses_get(
    int unit);


uint32 dnx_data_esem_access_cmd_app_di_id_size_in_bits_get(
    int unit);


uint32 dnx_data_esem_access_cmd_no_action_get(
    int unit);


uint32 dnx_data_esem_access_cmd_default_native_get(
    int unit);


uint32 dnx_data_esem_access_cmd_default_ac_get(
    int unit);


uint32 dnx_data_esem_access_cmd_sflow_sample_interface_get(
    int unit);


uint32 dnx_data_esem_access_cmd_ip_tunnel_no_action_get(
    int unit);


uint32 dnx_data_esem_access_cmd_nof_esem_cmd_default_profiles_get(
    int unit);


uint32 dnx_data_esem_access_cmd_nof_cmds_size_in_bits_get(
    int unit);


uint32 dnx_data_esem_access_cmd_nof_cmds_get(
    int unit);


uint32 dnx_data_esem_access_cmd_min_size_for_esem_cmd_get(
    int unit);


uint32 dnx_data_esem_access_cmd_esem_cmd_arr_prefix_gre_any_ipv4_get(
    int unit);


uint32 dnx_data_esem_access_cmd_esem_cmd_arr_prefix_vxlan_gpe_get(
    int unit);


uint32 dnx_data_esem_access_cmd_esem_cmd_arr_prefix_ip_tunnel_no_action_get(
    int unit);



typedef enum
{

    
    _dnx_data_esem_access_cmd_table_nof
} dnx_data_esem_access_cmd_table_e;









int dnx_data_esem_default_result_profile_feature_get(
    int unit,
    dnx_data_esem_default_result_profile_feature_e feature);



typedef enum
{
    dnx_data_esem_default_result_profile_define_default_native,
    dnx_data_esem_default_result_profile_define_default_ac,
    dnx_data_esem_default_result_profile_define_sflow_sample_interface,
    dnx_data_esem_default_result_profile_define_default_dual_homing,
    dnx_data_esem_default_result_profile_define_vxlan,
    dnx_data_esem_default_result_profile_define_nof_profiles,
    dnx_data_esem_default_result_profile_define_nof_allocable_profiles,

    
    _dnx_data_esem_default_result_profile_define_nof
} dnx_data_esem_default_result_profile_define_e;



uint32 dnx_data_esem_default_result_profile_default_native_get(
    int unit);


uint32 dnx_data_esem_default_result_profile_default_ac_get(
    int unit);


uint32 dnx_data_esem_default_result_profile_sflow_sample_interface_get(
    int unit);


uint32 dnx_data_esem_default_result_profile_default_dual_homing_get(
    int unit);


uint32 dnx_data_esem_default_result_profile_vxlan_get(
    int unit);


uint32 dnx_data_esem_default_result_profile_nof_profiles_get(
    int unit);


uint32 dnx_data_esem_default_result_profile_nof_allocable_profiles_get(
    int unit);



typedef enum
{

    
    _dnx_data_esem_default_result_profile_table_nof
} dnx_data_esem_default_result_profile_table_e;









int dnx_data_esem_feature_feature_get(
    int unit,
    dnx_data_esem_feature_feature_e feature);



typedef enum
{

    
    _dnx_data_esem_feature_define_nof
} dnx_data_esem_feature_define_e;




typedef enum
{

    
    _dnx_data_esem_feature_table_nof
} dnx_data_esem_feature_table_e;






shr_error_e dnx_data_esem_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

