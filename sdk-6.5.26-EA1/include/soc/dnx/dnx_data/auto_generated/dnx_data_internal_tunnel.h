
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_TUNNEL_H_

#define _DNX_DATA_INTERNAL_TUNNEL_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tunnel.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_tunnel_submodule_udp,
    dnx_data_tunnel_submodule_tunnel_type,
    dnx_data_tunnel_submodule_tcam_table,
    dnx_data_tunnel_submodule_feature,

    
    _dnx_data_tunnel_submodule_nof
} dnx_data_tunnel_submodule_e;








int dnx_data_tunnel_udp_feature_get(
    int unit,
    dnx_data_tunnel_udp_feature_e feature);



typedef enum
{
    dnx_data_tunnel_udp_define_nof_udp_ports_profiles,
    dnx_data_tunnel_udp_define_first_udp_ports_profile,

    
    _dnx_data_tunnel_udp_define_nof
} dnx_data_tunnel_udp_define_e;



uint32 dnx_data_tunnel_udp_nof_udp_ports_profiles_get(
    int unit);


uint32 dnx_data_tunnel_udp_first_udp_ports_profile_get(
    int unit);



typedef enum
{

    
    _dnx_data_tunnel_udp_table_nof
} dnx_data_tunnel_udp_table_e;









int dnx_data_tunnel_tunnel_type_feature_get(
    int unit,
    dnx_data_tunnel_tunnel_type_feature_e feature);



typedef enum
{
    dnx_data_tunnel_tunnel_type_define_udp_tunnel_type,
    dnx_data_tunnel_tunnel_type_define_geneve_tunnel_type_supported,

    
    _dnx_data_tunnel_tunnel_type_define_nof
} dnx_data_tunnel_tunnel_type_define_e;



uint32 dnx_data_tunnel_tunnel_type_udp_tunnel_type_get(
    int unit);


uint32 dnx_data_tunnel_tunnel_type_geneve_tunnel_type_supported_get(
    int unit);



typedef enum
{

    
    _dnx_data_tunnel_tunnel_type_table_nof
} dnx_data_tunnel_tunnel_type_table_e;









int dnx_data_tunnel_tcam_table_feature_get(
    int unit,
    dnx_data_tunnel_tcam_table_feature_e feature);



typedef enum
{
    dnx_data_tunnel_tcam_table_define_has_raw_udp_tunnel_type_is_2nd_pass_key_fields,

    
    _dnx_data_tunnel_tcam_table_define_nof
} dnx_data_tunnel_tcam_table_define_e;



uint32 dnx_data_tunnel_tcam_table_has_raw_udp_tunnel_type_is_2nd_pass_key_fields_get(
    int unit);



typedef enum
{

    
    _dnx_data_tunnel_tcam_table_table_nof
} dnx_data_tunnel_tcam_table_table_e;









int dnx_data_tunnel_feature_feature_get(
    int unit,
    dnx_data_tunnel_feature_feature_e feature);



typedef enum
{
    dnx_data_tunnel_feature_define_tunnel_route_disable,
    dnx_data_tunnel_feature_define_dedicated_fodo_to_vni_table,
    dnx_data_tunnel_feature_define_dip_idx_type_is_lif_field,
    dnx_data_tunnel_feature_define_fodo_to_vni_entry_created_by_vlan_api,
    dnx_data_tunnel_feature_define_has_flow_gport_to_tunnel_type,

    
    _dnx_data_tunnel_feature_define_nof
} dnx_data_tunnel_feature_define_e;



uint32 dnx_data_tunnel_feature_tunnel_route_disable_get(
    int unit);


uint32 dnx_data_tunnel_feature_dedicated_fodo_to_vni_table_get(
    int unit);


uint32 dnx_data_tunnel_feature_dip_idx_type_is_lif_field_get(
    int unit);


uint32 dnx_data_tunnel_feature_fodo_to_vni_entry_created_by_vlan_api_get(
    int unit);


uint32 dnx_data_tunnel_feature_has_flow_gport_to_tunnel_type_get(
    int unit);



typedef enum
{

    
    _dnx_data_tunnel_feature_table_nof
} dnx_data_tunnel_feature_table_e;






shr_error_e dnx_data_tunnel_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

