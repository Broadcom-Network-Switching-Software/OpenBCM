/*
 * ! \file mirror_egress.h Internal DNX MIRROR PROFILE APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 */

#ifndef _DNX_MIRROR_RCY_INCLUDED__
#define _DNX_MIRROR_RCY_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/mirror.h>
#include <bcm_int/dnx/port/port_ingr_reassembly.h>
#include <shared/shrextend/shrextend_error.h>

shr_error_e dnx_mirror_port_to_rcy_port_map_set(
    int unit,
    bcm_port_t forward_port,
    bcm_mirror_port_to_rcy_map_info_t * rcy_map_info);

shr_error_e dnx_mirror_port_to_rcy_port_unmap_set(
    int unit,
    bcm_port_t forward_port,
    bcm_mirror_port_to_rcy_map_info_t * rcy_map_info);

shr_error_e dnx_mirror_port_to_rcy_port_map_get(
    int unit,
    bcm_port_t forward_port,
    bcm_mirror_port_to_rcy_map_info_t * rcy_map_info);

#endif /** _DNX_MIRROR_RCY_INCLUDED__  */
