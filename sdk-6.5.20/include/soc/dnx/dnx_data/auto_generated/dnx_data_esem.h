

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_ESEM_H_

#define _DNX_DATA_ESEM_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_esem.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_esem_init(
    int unit);







typedef enum
{

    
    _dnx_data_esem_access_cmd_feature_nof
} dnx_data_esem_access_cmd_feature_e;



typedef int(
    *dnx_data_esem_access_cmd_feature_get_f) (
    int unit,
    dnx_data_esem_access_cmd_feature_e feature);


typedef uint32(
    *dnx_data_esem_access_cmd_nof_esem_accesses_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esem_access_cmd_app_di_id_size_in_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esem_access_cmd_no_action_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esem_access_cmd_default_native_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esem_access_cmd_default_ac_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esem_access_cmd_sflow_sample_interface_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esem_access_cmd_ip_tunnel_no_action_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esem_access_cmd_nof_esem_cmd_default_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esem_access_cmd_nof_cmds_size_in_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esem_access_cmd_nof_cmds_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esem_access_cmd_min_size_for_esem_cmd_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esem_access_cmd_esem_cmd_arr_prefix_gre_any_ipv4_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esem_access_cmd_esem_cmd_arr_prefix_vxlan_gpe_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esem_access_cmd_esem_cmd_arr_prefix_ip_tunnel_no_action_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_esem_access_cmd_feature_get_f feature_get;
    
    dnx_data_esem_access_cmd_nof_esem_accesses_get_f nof_esem_accesses_get;
    
    dnx_data_esem_access_cmd_app_di_id_size_in_bits_get_f app_di_id_size_in_bits_get;
    
    dnx_data_esem_access_cmd_no_action_get_f no_action_get;
    
    dnx_data_esem_access_cmd_default_native_get_f default_native_get;
    
    dnx_data_esem_access_cmd_default_ac_get_f default_ac_get;
    
    dnx_data_esem_access_cmd_sflow_sample_interface_get_f sflow_sample_interface_get;
    
    dnx_data_esem_access_cmd_ip_tunnel_no_action_get_f ip_tunnel_no_action_get;
    
    dnx_data_esem_access_cmd_nof_esem_cmd_default_profiles_get_f nof_esem_cmd_default_profiles_get;
    
    dnx_data_esem_access_cmd_nof_cmds_size_in_bits_get_f nof_cmds_size_in_bits_get;
    
    dnx_data_esem_access_cmd_nof_cmds_get_f nof_cmds_get;
    
    dnx_data_esem_access_cmd_min_size_for_esem_cmd_get_f min_size_for_esem_cmd_get;
    
    dnx_data_esem_access_cmd_esem_cmd_arr_prefix_gre_any_ipv4_get_f esem_cmd_arr_prefix_gre_any_ipv4_get;
    
    dnx_data_esem_access_cmd_esem_cmd_arr_prefix_vxlan_gpe_get_f esem_cmd_arr_prefix_vxlan_gpe_get;
    
    dnx_data_esem_access_cmd_esem_cmd_arr_prefix_ip_tunnel_no_action_get_f esem_cmd_arr_prefix_ip_tunnel_no_action_get;
} dnx_data_if_esem_access_cmd_t;







typedef enum
{

    
    _dnx_data_esem_default_result_profile_feature_nof
} dnx_data_esem_default_result_profile_feature_e;



typedef int(
    *dnx_data_esem_default_result_profile_feature_get_f) (
    int unit,
    dnx_data_esem_default_result_profile_feature_e feature);


typedef uint32(
    *dnx_data_esem_default_result_profile_default_native_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esem_default_result_profile_default_ac_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esem_default_result_profile_sflow_sample_interface_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esem_default_result_profile_default_dual_homing_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esem_default_result_profile_vxlan_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esem_default_result_profile_nof_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_esem_default_result_profile_nof_allocable_profiles_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_esem_default_result_profile_feature_get_f feature_get;
    
    dnx_data_esem_default_result_profile_default_native_get_f default_native_get;
    
    dnx_data_esem_default_result_profile_default_ac_get_f default_ac_get;
    
    dnx_data_esem_default_result_profile_sflow_sample_interface_get_f sflow_sample_interface_get;
    
    dnx_data_esem_default_result_profile_default_dual_homing_get_f default_dual_homing_get;
    
    dnx_data_esem_default_result_profile_vxlan_get_f vxlan_get;
    
    dnx_data_esem_default_result_profile_nof_profiles_get_f nof_profiles_get;
    
    dnx_data_esem_default_result_profile_nof_allocable_profiles_get_f nof_allocable_profiles_get;
} dnx_data_if_esem_default_result_profile_t;







typedef enum
{
    
    dnx_data_esem_feature_etps_properties_assignment,
    
    dnx_data_esem_feature_key_with_sspa_contains_member_id,

    
    _dnx_data_esem_feature_feature_nof
} dnx_data_esem_feature_feature_e;



typedef int(
    *dnx_data_esem_feature_feature_get_f) (
    int unit,
    dnx_data_esem_feature_feature_e feature);



typedef struct
{
    
    dnx_data_esem_feature_feature_get_f feature_get;
} dnx_data_if_esem_feature_t;





typedef struct
{
    
    dnx_data_if_esem_access_cmd_t access_cmd;
    
    dnx_data_if_esem_default_result_profile_t default_result_profile;
    
    dnx_data_if_esem_feature_t feature;
} dnx_data_if_esem_t;




extern dnx_data_if_esem_t dnx_data_esem;


#endif 

