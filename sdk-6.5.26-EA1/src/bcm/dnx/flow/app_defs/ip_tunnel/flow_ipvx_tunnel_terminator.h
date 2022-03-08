 /** \file flow_ipvx_tunnel_terminator.h
 *
 * All flow ipvx tunnel terminator service functions
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
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
#include <soc/sand/shrextend/shrextend_debug.h>
#include <src/bcm/dnx/vxlan/vxlan.h>
#include <bcm_int/dnx/flow/flow.h>

typedef enum
{
    TUNNEL_TYPE_ELEMENT_IS_RAW,
    TUNNEL_TYPE_ELEMENT_ADDITIONAL_HEADER_LAYER_TYPE,
    TUNNEL_TYPE_ELEMENT_UDP_TUNNEL_TYPE,
    NOF_TUNNEL_TYPE_ELEMENTS
} tunnel_type_element_enum_t;

typedef struct
{
    uint32 value;
    uint8 is_dont_care;
} flow_special_fields_tunnel_type_element_t;

typedef struct
{
    flow_special_fields_tunnel_type_element_t elements[NOF_TUNNEL_TYPE_ELEMENTS];
} flow_special_fields_tunnel_type_elements_t;

#define DNX_IPV6_TUNNEL_LIF_TYPE_INTERMEDIATE 0
#define DNX_IPV6_TUNNEL_LIF_TYPE_FINAL        1

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

shr_error_e flow_ipvx_term_match_tunnel_type_to_tunnel_type_elements(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    flow_special_fields_tunnel_type_elements_t * tunnel_type_elements);

shr_error_e flow_ipvx_term_match_tunnel_type_elements_key_fields_set(
    int unit,
    uint32 entry_handle_id,
    flow_special_fields_tunnel_type_elements_t * tunnel_type_elements);

shr_error_e flow_ipvx_term_match_tcam_tunnel_type_elements_dbal_key_fields_set(
    int unit,
    uint32 entry_handle_id,
    flow_special_fields_tunnel_type_elements_t * tunnel_type_elements);

shr_error_e flow_ipvx_term_match_tcam_tunnel_type_elements_dbal_key_fields_get(
    int unit,
    uint32 entry_handle_id,
    flow_special_fields_tunnel_type_elements_t * tunnel_type_elements);

shr_error_e flow_ipvx_term_match_tunnel_type_elements_dbal_key_field_set(
    int unit,
    uint32 entry_handle_id,
    flow_special_fields_tunnel_type_elements_t * tunnel_type_elements,
    tunnel_type_element_enum_t tunnel_type_element_enum);

shr_error_e flow_ipvx_term_match_tunnel_type_elements_dbal_key_field_get(
    int unit,
    uint32 entry_handle_id,
    flow_special_fields_tunnel_type_elements_t * tunnel_type_elements,
    tunnel_type_element_enum_t tunnel_type_element_enum);

#endif /** _DNX_FLOW_IPVX_TUNNEL_INCLUDED__ */
