
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_TRAPS_ACCESS_H__
#define __DNX_TRAPS_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_traps_types.h>



typedef int (*dnx_rx_traps_info_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*dnx_rx_traps_info_init_cb)(
    int unit);


typedef int (*dnx_rx_traps_info_etpp_keep_fabric_headers_profile_set_cb)(
    int unit, uint8 etpp_keep_fabric_headers_profile);


typedef int (*dnx_rx_traps_info_etpp_keep_fabric_headers_profile_get_cb)(
    int unit, uint8 *etpp_keep_fabric_headers_profile);




typedef struct {
    dnx_rx_traps_info_etpp_keep_fabric_headers_profile_set_cb set;
    dnx_rx_traps_info_etpp_keep_fabric_headers_profile_get_cb get;
} dnx_rx_traps_info_etpp_keep_fabric_headers_profile_cbs;


typedef struct {
    dnx_rx_traps_info_is_init_cb is_init;
    dnx_rx_traps_info_init_cb init;
    
    dnx_rx_traps_info_etpp_keep_fabric_headers_profile_cbs etpp_keep_fabric_headers_profile;
} dnx_rx_traps_info_cbs;





extern dnx_rx_traps_info_cbs dnx_rx_traps_info;

#endif 
