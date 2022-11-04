
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __SRV6_TYPES_H__
#define __SRV6_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>


typedef struct {
    uint8 egress_is_psp;
    uint8 encap_is_reduced;
    uint8 gsid_prefix_is_64b;
    uint8 usid_prefix_is_48b;
    int lpm_destination_for_usp_trap;
} srv6_modes_struct;


#endif 
