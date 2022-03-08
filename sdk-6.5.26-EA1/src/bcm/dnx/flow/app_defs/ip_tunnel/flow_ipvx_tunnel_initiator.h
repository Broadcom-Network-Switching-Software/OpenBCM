 /** \file flow_ipvx_tunnel_initiator.h
 *
 * All dnx flow internal structs, callbacks structures etc.
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef _DNX_FLOW_IPVX_TUNNEL_INITIATOR_INCLUDED__
/*
 * {
 */
#define _DNX_FLOW_IPVX_TUNNEL_INITIATOR_INCLUDED__

/************
 * INCLUDES *
 ************/
#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/flow/flow.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_tunnel_access.h>

/***********
 * DEFINES *
 ***********/

typedef enum
{
    TUNNEL_TYPE_RAW,
    TUNNEL_TYPE_GRE,
    TUNNEL_TYPE_UDP,
    TUNNEL_TYPE_VXLAN,
    TUNNEL_TYPE_VXLAN_GPE,
} flow_ipvx_tunnel_type_t;

/*************
 * FUNCTIONS *
 *************/

shr_error_e flow_ipvx_tunnel_initiator_udp_ports_verify(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    bcm_flow_special_fields_t * special_fields,
    uint8 is_replace);

shr_error_e flow_ipvx_tunnel_initiator_gre_flags_verify(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    bcm_flow_special_fields_t * special_fields,
    bcm_flow_special_fields_t * prev_special_fields,
    uint8 is_replace);

shr_error_e flow_ipvx_tunnel_initiator_gre_flag_get(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    bcm_flow_special_fields_t * special_fields,
    bcm_flow_special_fields_t * prev_special_fields,
    uint8 is_replace,
    uint32 *gre_flag);

shr_error_e flow_ipvx_tunnel_initiator_dbal_field_lif_ahp_set(
    int unit,
    uint32 entry_handle_id,
    bcm_tunnel_type_t tunnel_type,
    uint32 gre_flag,
    udp_ports_t * udp_ports);

shr_error_e flow_ipvx_tunnel_initiator_tunnel_type_and_derivatives_set(
    int unit,
    uint32 entry_handle_id,
    bcm_tunnel_type_t tunnel_type,
    bcm_flow_special_fields_t * special_fields);

shr_error_e flow_ipvx_tunnel_initiator_esem_command_set(
    int unit,
    uint32 entry_handle_id,
    bcm_tunnel_type_t tunnel_type);

shr_error_e flow_ipvx_tunnel_initiator_replace_verify(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    bcm_flow_special_fields_t * special_fields,
    bcm_flow_special_fields_t * prev_special_fields);

shr_error_e flow_ipvx_tunnel_initiator_tunnel_type_and_derivatives_replace(
    int unit,
    uint32 current_entry_handle_id,
    uint32 prev_entry_handle_id,
    bcm_tunnel_type_t tunnel_type,
    bcm_flow_special_fields_t * special_fields,
    bcm_flow_special_fields_t * prev_special_fields);

void flow_ipvx_tunnel_initiator_special_field_gre_flags_add(
    uint32 lif_ahp_data,
    bcm_flow_special_fields_t * special_fields);

shr_error_e flow_ipvx_tunnel_initiator_udp_profile_delete(
    int unit,
    uint32 entry_handle_id);

shr_error_e flow_ipvx_tunnel_initiator_esem_command_profile_delete(
    int unit,
    uint32 entry_handle_id);

shr_error_e flow_ipvx_tunnel_initiator_special_fields_tunnel_type_derivatives_add(
    int unit,
    flow_ipvx_tunnel_type_t flow_ipvx_tunnel_type,
    uint32 lif_ahp,
    bcm_flow_special_fields_t * special_fields);

shr_error_e flow_ipvx_tunnel_initiator_flow_tunnel_type_and_lif_ahp_get(
    int unit,
    uint32 entry_handle_id,
    flow_ipvx_tunnel_type_t * flow_ipvx_tunnel_type,
    uint32 *lif_ahp);

#endif /** _DNX_FLOW_IPVX_TUNNEL_INITIATOR_INCLUDED__ */
