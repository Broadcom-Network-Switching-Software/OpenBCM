 /** \file flow_ipvx_tunnel_terminator.h
 *
 * All flow ipvx tunnel terminator service functions
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 */

#ifndef _DNX_FLOW_IPVX_TUNNEL_TERMINATOR_INCLUDED__
/*
 * {
 */
#define _DNX_FLOW_IPVX_TUNNEL_TERMINATOR_INCLUDED__

/************
 * INCLUDES *
 ************/
#include <shared/shrextend/shrextend_debug.h>
#include <src/bcm/dnx/vxlan/vxlan.h>
#include <bcm_int/dnx/flow/flow.h>

/*************
 * FUNCTIONS *
 *************/

shr_error_e dnx_flow_field_ipvx_term_l2_learn_info_set(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_l2_learn_info_t * l2_learn_info);


int flow_legacy_ipvx_match_app_is_valid(
    int unit);
int flow_legacy_ivpx_tunnel_terminator_is_valid(
    int unit);
#endif /** _DNX_FLOW_IPVX_TUNNEL_INCLUDED__ */
