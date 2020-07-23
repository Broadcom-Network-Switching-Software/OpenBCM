

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_PORT_PP_H_

#define _DNX_DATA_PORT_PP_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_port_pp.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_port_pp_init(
    int unit);







typedef enum
{

    
    _dnx_data_port_pp_general_feature_nof
} dnx_data_port_pp_general_feature_e;



typedef int(
    *dnx_data_port_pp_general_feature_get_f) (
    int unit,
    dnx_data_port_pp_general_feature_e feature);


typedef uint32(
    *dnx_data_port_pp_general_nof_ingress_llvp_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_pp_general_nof_egress_llvp_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_pp_general_default_ingress_native_ac_llvp_profile_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_pp_general_default_egress_native_ac_llvp_profile_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_pp_general_default_egress_ip_tunnel_llvp_profile_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_pp_general_nof_egress_sit_profile_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_pp_general_nof_egress_acceptable_frame_type_profile_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_pp_general_sit_inner_eth_additional_tag_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_pp_general_nof_virtual_port_tcam_entries_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_port_pp_general_feature_get_f feature_get;
    
    dnx_data_port_pp_general_nof_ingress_llvp_profiles_get_f nof_ingress_llvp_profiles_get;
    
    dnx_data_port_pp_general_nof_egress_llvp_profiles_get_f nof_egress_llvp_profiles_get;
    
    dnx_data_port_pp_general_default_ingress_native_ac_llvp_profile_id_get_f default_ingress_native_ac_llvp_profile_id_get;
    
    dnx_data_port_pp_general_default_egress_native_ac_llvp_profile_id_get_f default_egress_native_ac_llvp_profile_id_get;
    
    dnx_data_port_pp_general_default_egress_ip_tunnel_llvp_profile_id_get_f default_egress_ip_tunnel_llvp_profile_id_get;
    
    dnx_data_port_pp_general_nof_egress_sit_profile_get_f nof_egress_sit_profile_get;
    
    dnx_data_port_pp_general_nof_egress_acceptable_frame_type_profile_get_f nof_egress_acceptable_frame_type_profile_get;
    
    dnx_data_port_pp_general_sit_inner_eth_additional_tag_get_f sit_inner_eth_additional_tag_get;
    
    dnx_data_port_pp_general_nof_virtual_port_tcam_entries_get_f nof_virtual_port_tcam_entries_get;
} dnx_data_if_port_pp_general_t;





typedef struct
{
    
    dnx_data_if_port_pp_general_t general;
} dnx_data_if_port_pp_t;




extern dnx_data_if_port_pp_t dnx_data_port_pp;


#endif 

