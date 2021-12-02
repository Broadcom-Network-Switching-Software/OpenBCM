
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_FLOW_H_

#define _DNX_DATA_FLOW_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_flow.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_flow_init(
    int unit);







typedef enum
{
    dnx_data_flow_general_is_tests_supported,
    
    dnx_data_flow_general_is_inlif_profile_dedicated_logic_enabled,

    
    _dnx_data_flow_general_feature_nof
} dnx_data_flow_general_feature_e;



typedef int(
    *dnx_data_flow_general_feature_get_f) (
    int unit,
    dnx_data_flow_general_feature_e feature);



typedef struct
{
    
    dnx_data_flow_general_feature_get_f feature_get;
} dnx_data_if_flow_general_t;







typedef enum
{

    
    _dnx_data_flow_ipv4_feature_nof
} dnx_data_flow_ipv4_feature_e;



typedef int(
    *dnx_data_flow_ipv4_feature_get_f) (
    int unit,
    dnx_data_flow_ipv4_feature_e feature);


typedef uint32(
    *dnx_data_flow_ipv4_use_flow_lif_init_get_f) (
    int unit);


typedef uint32(
    *dnx_data_flow_ipv4_use_flow_lif_term_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_flow_ipv4_feature_get_f feature_get;
    
    dnx_data_flow_ipv4_use_flow_lif_init_get_f use_flow_lif_init_get;
    
    dnx_data_flow_ipv4_use_flow_lif_term_get_f use_flow_lif_term_get;
} dnx_data_if_flow_ipv4_t;







typedef enum
{
    
    dnx_data_flow_srv6_use_flow_lif_init,
    
    dnx_data_flow_srv6_use_flow_lif_term,

    
    _dnx_data_flow_srv6_feature_nof
} dnx_data_flow_srv6_feature_e;



typedef int(
    *dnx_data_flow_srv6_feature_get_f) (
    int unit,
    dnx_data_flow_srv6_feature_e feature);



typedef struct
{
    
    dnx_data_flow_srv6_feature_get_f feature_get;
} dnx_data_if_flow_srv6_t;







typedef enum
{
    
    dnx_data_flow_in_lif_flow_in_use,

    
    _dnx_data_flow_in_lif_feature_nof
} dnx_data_flow_in_lif_feature_e;



typedef int(
    *dnx_data_flow_in_lif_feature_get_f) (
    int unit,
    dnx_data_flow_in_lif_feature_e feature);



typedef struct
{
    
    dnx_data_flow_in_lif_feature_get_f feature_get;
} dnx_data_if_flow_in_lif_t;







typedef enum
{
    
    dnx_data_flow_mpls_ingress_flow_in_use,
    
    dnx_data_flow_mpls_egress_flow_in_use,

    
    _dnx_data_flow_mpls_feature_nof
} dnx_data_flow_mpls_feature_e;



typedef int(
    *dnx_data_flow_mpls_feature_get_f) (
    int unit,
    dnx_data_flow_mpls_feature_e feature);



typedef struct
{
    
    dnx_data_flow_mpls_feature_get_f feature_get;
} dnx_data_if_flow_mpls_t;







typedef enum
{
    
    dnx_data_flow_mpls_port_flow_in_use,

    
    _dnx_data_flow_mpls_port_feature_nof
} dnx_data_flow_mpls_port_feature_e;



typedef int(
    *dnx_data_flow_mpls_port_feature_get_f) (
    int unit,
    dnx_data_flow_mpls_port_feature_e feature);



typedef struct
{
    
    dnx_data_flow_mpls_port_feature_get_f feature_get;
} dnx_data_if_flow_mpls_port_t;







typedef enum
{
    
    dnx_data_flow_arp_flow_in_use,
    
    dnx_data_flow_arp_virtual_lif_support,

    
    _dnx_data_flow_arp_feature_nof
} dnx_data_flow_arp_feature_e;



typedef int(
    *dnx_data_flow_arp_feature_get_f) (
    int unit,
    dnx_data_flow_arp_feature_e feature);



typedef struct
{
    
    dnx_data_flow_arp_feature_get_f feature_get;
} dnx_data_if_flow_arp_t;







typedef enum
{
    
    dnx_data_flow_vlan_port_flow_in_use,

    
    _dnx_data_flow_vlan_port_feature_nof
} dnx_data_flow_vlan_port_feature_e;



typedef int(
    *dnx_data_flow_vlan_port_feature_get_f) (
    int unit,
    dnx_data_flow_vlan_port_feature_e feature);



typedef struct
{
    
    dnx_data_flow_vlan_port_feature_get_f feature_get;
} dnx_data_if_flow_vlan_port_t;







typedef enum
{

    
    _dnx_data_flow_algo_gpm_feature_nof
} dnx_data_flow_algo_gpm_feature_e;



typedef int(
    *dnx_data_flow_algo_gpm_feature_get_f) (
    int unit,
    dnx_data_flow_algo_gpm_feature_e feature);


typedef uint32(
    *dnx_data_flow_algo_gpm_flow_gport_in_use_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_flow_algo_gpm_feature_get_f feature_get;
    
    dnx_data_flow_algo_gpm_flow_gport_in_use_get_f flow_gport_in_use_get;
} dnx_data_if_flow_algo_gpm_t;







typedef enum
{
    
    dnx_data_flow_wide_data_flow_in_use,

    
    _dnx_data_flow_wide_data_feature_nof
} dnx_data_flow_wide_data_feature_e;



typedef int(
    *dnx_data_flow_wide_data_feature_get_f) (
    int unit,
    dnx_data_flow_wide_data_feature_e feature);



typedef struct
{
    
    dnx_data_flow_wide_data_feature_get_f feature_get;
} dnx_data_if_flow_wide_data_t;







typedef enum
{
    
    dnx_data_flow_stat_pp_flow_in_use,

    
    _dnx_data_flow_stat_pp_feature_nof
} dnx_data_flow_stat_pp_feature_e;



typedef int(
    *dnx_data_flow_stat_pp_feature_get_f) (
    int unit,
    dnx_data_flow_stat_pp_feature_e feature);



typedef struct
{
    
    dnx_data_flow_stat_pp_feature_get_f feature_get;
} dnx_data_if_flow_stat_pp_t;







typedef enum
{
    
    dnx_data_flow_rch_flow_in_use,

    
    _dnx_data_flow_rch_feature_nof
} dnx_data_flow_rch_feature_e;



typedef int(
    *dnx_data_flow_rch_feature_get_f) (
    int unit,
    dnx_data_flow_rch_feature_e feature);



typedef struct
{
    
    dnx_data_flow_rch_feature_get_f feature_get;
} dnx_data_if_flow_rch_t;







typedef enum
{
    
    dnx_data_flow_trap_flow_in_use,

    
    _dnx_data_flow_trap_feature_nof
} dnx_data_flow_trap_feature_e;



typedef int(
    *dnx_data_flow_trap_feature_get_f) (
    int unit,
    dnx_data_flow_trap_feature_e feature);



typedef struct
{
    
    dnx_data_flow_trap_feature_get_f feature_get;
} dnx_data_if_flow_trap_t;







typedef enum
{
    
    dnx_data_flow_reflector_flow_in_use,

    
    _dnx_data_flow_reflector_feature_nof
} dnx_data_flow_reflector_feature_e;



typedef int(
    *dnx_data_flow_reflector_feature_get_f) (
    int unit,
    dnx_data_flow_reflector_feature_e feature);



typedef struct
{
    
    dnx_data_flow_reflector_feature_get_f feature_get;
} dnx_data_if_flow_reflector_t;





typedef struct
{
    
    dnx_data_if_flow_general_t general;
    
    dnx_data_if_flow_ipv4_t ipv4;
    
    dnx_data_if_flow_srv6_t srv6;
    
    dnx_data_if_flow_in_lif_t in_lif;
    
    dnx_data_if_flow_mpls_t mpls;
    
    dnx_data_if_flow_mpls_port_t mpls_port;
    
    dnx_data_if_flow_arp_t arp;
    
    dnx_data_if_flow_vlan_port_t vlan_port;
    
    dnx_data_if_flow_algo_gpm_t algo_gpm;
    
    dnx_data_if_flow_wide_data_t wide_data;
    
    dnx_data_if_flow_stat_pp_t stat_pp;
    
    dnx_data_if_flow_rch_t rch;
    
    dnx_data_if_flow_trap_t trap;
    
    dnx_data_if_flow_reflector_t reflector;
} dnx_data_if_flow_t;




extern dnx_data_if_flow_t dnx_data_flow;


#endif 

