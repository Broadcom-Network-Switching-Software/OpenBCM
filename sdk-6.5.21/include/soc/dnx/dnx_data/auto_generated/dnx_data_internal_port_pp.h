

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_PORT_PP_H_

#define _DNX_DATA_INTERNAL_PORT_PP_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port_pp.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_port_pp_submodule_general,

    
    _dnx_data_port_pp_submodule_nof
} dnx_data_port_pp_submodule_e;








int dnx_data_port_pp_general_feature_get(
    int unit,
    dnx_data_port_pp_general_feature_e feature);



typedef enum
{
    dnx_data_port_pp_general_define_nof_ingress_llvp_profiles,
    dnx_data_port_pp_general_define_nof_egress_llvp_profiles,
    dnx_data_port_pp_general_define_default_ingress_native_ac_llvp_profile_id,
    dnx_data_port_pp_general_define_default_egress_native_ac_llvp_profile_id,
    dnx_data_port_pp_general_define_default_egress_ip_tunnel_llvp_profile_id,
    dnx_data_port_pp_general_define_nof_egress_sit_profile,
    dnx_data_port_pp_general_define_nof_egress_acceptable_frame_type_profile,
    dnx_data_port_pp_general_define_sit_inner_eth_additional_tag,
    dnx_data_port_pp_general_define_nof_virtual_port_tcam_entries,

    
    _dnx_data_port_pp_general_define_nof
} dnx_data_port_pp_general_define_e;



uint32 dnx_data_port_pp_general_nof_ingress_llvp_profiles_get(
    int unit);


uint32 dnx_data_port_pp_general_nof_egress_llvp_profiles_get(
    int unit);


uint32 dnx_data_port_pp_general_default_ingress_native_ac_llvp_profile_id_get(
    int unit);


uint32 dnx_data_port_pp_general_default_egress_native_ac_llvp_profile_id_get(
    int unit);


uint32 dnx_data_port_pp_general_default_egress_ip_tunnel_llvp_profile_id_get(
    int unit);


uint32 dnx_data_port_pp_general_nof_egress_sit_profile_get(
    int unit);


uint32 dnx_data_port_pp_general_nof_egress_acceptable_frame_type_profile_get(
    int unit);


uint32 dnx_data_port_pp_general_sit_inner_eth_additional_tag_get(
    int unit);


uint32 dnx_data_port_pp_general_nof_virtual_port_tcam_entries_get(
    int unit);



typedef enum
{

    
    _dnx_data_port_pp_general_table_nof
} dnx_data_port_pp_general_table_e;






shr_error_e dnx_data_port_pp_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

