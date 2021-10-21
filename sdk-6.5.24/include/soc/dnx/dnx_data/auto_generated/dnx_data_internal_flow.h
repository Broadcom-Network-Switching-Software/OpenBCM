
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_FLOW_H_

#define _DNX_DATA_INTERNAL_FLOW_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_flow.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_flow_submodule_general,
    dnx_data_flow_submodule_ipv4,
    dnx_data_flow_submodule_in_lif,
    dnx_data_flow_submodule_mpls,
    dnx_data_flow_submodule_mpls_port,
    dnx_data_flow_submodule_arp,
    dnx_data_flow_submodule_vlan_port,
    dnx_data_flow_submodule_algo_gpm,
    dnx_data_flow_submodule_wide_data,
    dnx_data_flow_submodule_stat_pp,
    dnx_data_flow_submodule_rch,
    dnx_data_flow_submodule_trap,
    dnx_data_flow_submodule_reflector,

    
    _dnx_data_flow_submodule_nof
} dnx_data_flow_submodule_e;








int dnx_data_flow_general_feature_get(
    int unit,
    dnx_data_flow_general_feature_e feature);



typedef enum
{

    
    _dnx_data_flow_general_define_nof
} dnx_data_flow_general_define_e;




typedef enum
{

    
    _dnx_data_flow_general_table_nof
} dnx_data_flow_general_table_e;









int dnx_data_flow_ipv4_feature_get(
    int unit,
    dnx_data_flow_ipv4_feature_e feature);



typedef enum
{
    dnx_data_flow_ipv4_define_use_flow_lif_init,
    dnx_data_flow_ipv4_define_use_flow_lif_term,

    
    _dnx_data_flow_ipv4_define_nof
} dnx_data_flow_ipv4_define_e;



uint32 dnx_data_flow_ipv4_use_flow_lif_init_get(
    int unit);


uint32 dnx_data_flow_ipv4_use_flow_lif_term_get(
    int unit);



typedef enum
{

    
    _dnx_data_flow_ipv4_table_nof
} dnx_data_flow_ipv4_table_e;









int dnx_data_flow_in_lif_feature_get(
    int unit,
    dnx_data_flow_in_lif_feature_e feature);



typedef enum
{

    
    _dnx_data_flow_in_lif_define_nof
} dnx_data_flow_in_lif_define_e;




typedef enum
{

    
    _dnx_data_flow_in_lif_table_nof
} dnx_data_flow_in_lif_table_e;









int dnx_data_flow_mpls_feature_get(
    int unit,
    dnx_data_flow_mpls_feature_e feature);



typedef enum
{

    
    _dnx_data_flow_mpls_define_nof
} dnx_data_flow_mpls_define_e;




typedef enum
{

    
    _dnx_data_flow_mpls_table_nof
} dnx_data_flow_mpls_table_e;









int dnx_data_flow_mpls_port_feature_get(
    int unit,
    dnx_data_flow_mpls_port_feature_e feature);



typedef enum
{

    
    _dnx_data_flow_mpls_port_define_nof
} dnx_data_flow_mpls_port_define_e;




typedef enum
{

    
    _dnx_data_flow_mpls_port_table_nof
} dnx_data_flow_mpls_port_table_e;









int dnx_data_flow_arp_feature_get(
    int unit,
    dnx_data_flow_arp_feature_e feature);



typedef enum
{

    
    _dnx_data_flow_arp_define_nof
} dnx_data_flow_arp_define_e;




typedef enum
{

    
    _dnx_data_flow_arp_table_nof
} dnx_data_flow_arp_table_e;









int dnx_data_flow_vlan_port_feature_get(
    int unit,
    dnx_data_flow_vlan_port_feature_e feature);



typedef enum
{

    
    _dnx_data_flow_vlan_port_define_nof
} dnx_data_flow_vlan_port_define_e;




typedef enum
{

    
    _dnx_data_flow_vlan_port_table_nof
} dnx_data_flow_vlan_port_table_e;









int dnx_data_flow_algo_gpm_feature_get(
    int unit,
    dnx_data_flow_algo_gpm_feature_e feature);



typedef enum
{
    dnx_data_flow_algo_gpm_define_flow_gport_in_use,

    
    _dnx_data_flow_algo_gpm_define_nof
} dnx_data_flow_algo_gpm_define_e;



uint32 dnx_data_flow_algo_gpm_flow_gport_in_use_get(
    int unit);



typedef enum
{

    
    _dnx_data_flow_algo_gpm_table_nof
} dnx_data_flow_algo_gpm_table_e;









int dnx_data_flow_wide_data_feature_get(
    int unit,
    dnx_data_flow_wide_data_feature_e feature);



typedef enum
{

    
    _dnx_data_flow_wide_data_define_nof
} dnx_data_flow_wide_data_define_e;




typedef enum
{

    
    _dnx_data_flow_wide_data_table_nof
} dnx_data_flow_wide_data_table_e;









int dnx_data_flow_stat_pp_feature_get(
    int unit,
    dnx_data_flow_stat_pp_feature_e feature);



typedef enum
{

    
    _dnx_data_flow_stat_pp_define_nof
} dnx_data_flow_stat_pp_define_e;




typedef enum
{

    
    _dnx_data_flow_stat_pp_table_nof
} dnx_data_flow_stat_pp_table_e;









int dnx_data_flow_rch_feature_get(
    int unit,
    dnx_data_flow_rch_feature_e feature);



typedef enum
{

    
    _dnx_data_flow_rch_define_nof
} dnx_data_flow_rch_define_e;




typedef enum
{

    
    _dnx_data_flow_rch_table_nof
} dnx_data_flow_rch_table_e;









int dnx_data_flow_trap_feature_get(
    int unit,
    dnx_data_flow_trap_feature_e feature);



typedef enum
{

    
    _dnx_data_flow_trap_define_nof
} dnx_data_flow_trap_define_e;




typedef enum
{

    
    _dnx_data_flow_trap_table_nof
} dnx_data_flow_trap_table_e;









int dnx_data_flow_reflector_feature_get(
    int unit,
    dnx_data_flow_reflector_feature_e feature);



typedef enum
{

    
    _dnx_data_flow_reflector_define_nof
} dnx_data_flow_reflector_define_e;




typedef enum
{

    
    _dnx_data_flow_reflector_table_nof
} dnx_data_flow_reflector_table_e;






shr_error_e dnx_data_flow_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

