
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_TUNNEL_H_

#define _DNX_DATA_TUNNEL_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_tunnel.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
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



typedef struct
{
    
    dnx_data_tunnel_tunnel_type_feature_get_f feature_get;
    
    dnx_data_tunnel_tunnel_type_udp_tunnel_type_get_f udp_tunnel_type_get;
} dnx_data_if_tunnel_tunnel_type_t;







typedef enum
{

    
    _dnx_data_tunnel_tcam_table_feature_nof
} dnx_data_tunnel_tcam_table_feature_e;



typedef int(
    *dnx_data_tunnel_tcam_table_feature_get_f) (
    int unit,
    dnx_data_tunnel_tcam_table_feature_e feature);


typedef uint32(
    *dnx_data_tunnel_tcam_table_has_raw_udp_tunnel_type_is_2nd_pass_key_fields_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_tunnel_tcam_table_feature_get_f feature_get;
    
    dnx_data_tunnel_tcam_table_has_raw_udp_tunnel_type_is_2nd_pass_key_fields_get_f has_raw_udp_tunnel_type_is_2nd_pass_key_fields_get;
} dnx_data_if_tunnel_tcam_table_t;





typedef struct
{
    
    dnx_data_if_tunnel_udp_t udp;
    
    dnx_data_if_tunnel_tunnel_type_t tunnel_type;
    
    dnx_data_if_tunnel_tcam_table_t tcam_table;
} dnx_data_if_tunnel_t;




extern dnx_data_if_tunnel_t dnx_data_tunnel;


#endif 

