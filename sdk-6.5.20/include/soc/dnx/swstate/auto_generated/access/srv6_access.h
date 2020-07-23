
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __SRV6_ACCESS_H__
#define __SRV6_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/srv6_types.h>



typedef int (*srv6_modes_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*srv6_modes_init_cb)(
    int unit);


typedef int (*srv6_modes_egress_is_psp_set_cb)(
    int unit, uint8 egress_is_psp);


typedef int (*srv6_modes_egress_is_psp_get_cb)(
    int unit, uint8 *egress_is_psp);


typedef int (*srv6_modes_encap_is_reduced_set_cb)(
    int unit, uint8 encap_is_reduced);


typedef int (*srv6_modes_encap_is_reduced_get_cb)(
    int unit, uint8 *encap_is_reduced);




typedef struct {
    srv6_modes_egress_is_psp_set_cb set;
    srv6_modes_egress_is_psp_get_cb get;
} srv6_modes_egress_is_psp_cbs;


typedef struct {
    srv6_modes_encap_is_reduced_set_cb set;
    srv6_modes_encap_is_reduced_get_cb get;
} srv6_modes_encap_is_reduced_cbs;


typedef struct {
    srv6_modes_is_init_cb is_init;
    srv6_modes_init_cb init;
    
    srv6_modes_egress_is_psp_cbs egress_is_psp;
    
    srv6_modes_encap_is_reduced_cbs encap_is_reduced;
} srv6_modes_cbs;





extern srv6_modes_cbs srv6_modes;

#endif 
