

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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
    dnx_data_tunnel_submodule_ipv6,
    dnx_data_tunnel_submodule_udp,

    
    _dnx_data_tunnel_submodule_nof
} dnx_data_tunnel_submodule_e;








int dnx_data_tunnel_ipv6_feature_get(
    int unit,
    dnx_data_tunnel_ipv6_feature_e feature);



typedef enum
{
    dnx_data_tunnel_ipv6_define_nof_terminated_sips,

    
    _dnx_data_tunnel_ipv6_define_nof
} dnx_data_tunnel_ipv6_define_e;



uint32 dnx_data_tunnel_ipv6_nof_terminated_sips_get(
    int unit);



typedef enum
{

    
    _dnx_data_tunnel_ipv6_table_nof
} dnx_data_tunnel_ipv6_table_e;









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






shr_error_e dnx_data_tunnel_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

