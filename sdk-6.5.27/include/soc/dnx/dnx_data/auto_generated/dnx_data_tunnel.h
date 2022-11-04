
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_TUNNEL_H_

#define _DNX_DATA_TUNNEL_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_tunnel.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



shr_error_e dnx_data_if_tunnel_init(
    int unit);







typedef enum
{

    
    _dnx_data_tunnel_udp_feature_nof
} dnx_data_tunnel_udp_feature_e;



typedef int(
    *dnx_data_tunnel_udp_feature_get_f) (
    int unit,
    dnx_data_tunnel_udp_feature_e feature);


typedef uint32(
    *dnx_data_tunnel_udp_nof_udp_ports_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tunnel_udp_first_udp_ports_profile_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_tunnel_udp_feature_get_f feature_get;
    
    dnx_data_tunnel_udp_nof_udp_ports_profiles_get_f nof_udp_ports_profiles_get;
    
    dnx_data_tunnel_udp_first_udp_ports_profile_get_f first_udp_ports_profile_get;
} dnx_data_if_tunnel_udp_t;







typedef enum
{

    
    _dnx_data_tunnel_tunnel_type_feature_nof
} dnx_data_tunnel_tunnel_type_feature_e;



typedef int(
    *dnx_data_tunnel_tunnel_type_feature_get_f) (
    int unit,
    dnx_data_tunnel_tunnel_type_feature_e feature);


typedef uint32(
    *dnx_data_tunnel_tunnel_type_udp_tunnel_type_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tunnel_tunnel_type_gre_tunnel_type_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tunnel_tunnel_type_geneve_tunnel_type_supported_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_tunnel_tunnel_type_feature_get_f feature_get;
    
    dnx_data_tunnel_tunnel_type_udp_tunnel_type_get_f udp_tunnel_type_get;
    
    dnx_data_tunnel_tunnel_type_gre_tunnel_type_get_f gre_tunnel_type_get;
    
    dnx_data_tunnel_tunnel_type_geneve_tunnel_type_supported_get_f geneve_tunnel_type_supported_get;
} dnx_data_if_tunnel_tunnel_type_t;







typedef enum
{

    
    _dnx_data_tunnel_roo_feature_nof
} dnx_data_tunnel_roo_feature_e;



typedef int(
    *dnx_data_tunnel_roo_feature_get_f) (
    int unit,
    dnx_data_tunnel_roo_feature_e feature);


typedef uint32(
    *dnx_data_tunnel_roo_terminate_only_8B_in_ip_at_term_block_using_pmf_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_tunnel_roo_feature_get_f feature_get;
    
    dnx_data_tunnel_roo_terminate_only_8B_in_ip_at_term_block_using_pmf_get_f terminate_only_8B_in_ip_at_term_block_using_pmf_get;
} dnx_data_if_tunnel_roo_t;







typedef enum
{
    dnx_data_tunnel_feature_ipvx_tunnel_over_mpls_supported,
    dnx_data_tunnel_feature_ipvx_tunnel_over_mpls_use_vtt_tcam,

    
    _dnx_data_tunnel_feature_feature_nof
} dnx_data_tunnel_feature_feature_e;



typedef int(
    *dnx_data_tunnel_feature_feature_get_f) (
    int unit,
    dnx_data_tunnel_feature_feature_e feature);


typedef uint32(
    *dnx_data_tunnel_feature_tunnel_route_disable_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tunnel_feature_dedicated_fodo_to_vni_table_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tunnel_feature_dip_idx_type_is_lif_field_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tunnel_feature_fodo_to_vni_entry_created_by_vlan_api_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tunnel_feature_has_flow_gport_to_tunnel_type_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tunnel_feature_support_learn_mode_async_lif_and_fec_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tunnel_feature_l3_fodo_result_type_supported_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tunnel_feature_udp_tunnel_nat_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tunnel_feature_short_network_domain_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_tunnel_feature_feature_get_f feature_get;
    
    dnx_data_tunnel_feature_tunnel_route_disable_get_f tunnel_route_disable_get;
    
    dnx_data_tunnel_feature_dedicated_fodo_to_vni_table_get_f dedicated_fodo_to_vni_table_get;
    
    dnx_data_tunnel_feature_dip_idx_type_is_lif_field_get_f dip_idx_type_is_lif_field_get;
    
    dnx_data_tunnel_feature_fodo_to_vni_entry_created_by_vlan_api_get_f fodo_to_vni_entry_created_by_vlan_api_get;
    
    dnx_data_tunnel_feature_has_flow_gport_to_tunnel_type_get_f has_flow_gport_to_tunnel_type_get;
    
    dnx_data_tunnel_feature_support_learn_mode_async_lif_and_fec_get_f support_learn_mode_async_lif_and_fec_get;
    
    dnx_data_tunnel_feature_l3_fodo_result_type_supported_get_f l3_fodo_result_type_supported_get;
    
    dnx_data_tunnel_feature_udp_tunnel_nat_get_f udp_tunnel_nat_get;
    
    dnx_data_tunnel_feature_short_network_domain_get_f short_network_domain_get;
} dnx_data_if_tunnel_feature_t;





typedef struct
{
    
    dnx_data_if_tunnel_udp_t udp;
    
    dnx_data_if_tunnel_tunnel_type_t tunnel_type;
    
    dnx_data_if_tunnel_roo_t roo;
    
    dnx_data_if_tunnel_feature_t feature;
} dnx_data_if_tunnel_t;




extern dnx_data_if_tunnel_t dnx_data_tunnel;


#endif 

