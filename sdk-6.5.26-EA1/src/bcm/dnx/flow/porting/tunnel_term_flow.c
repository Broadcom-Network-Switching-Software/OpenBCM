/** \file tunnel_term_flow.c
 *  General TUNNEL termination functionality using flow lif for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TUNNEL

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm/tunnel.h>
#include <bcm_int/dnx/auto_generated/dnx_flow_dispatch.h>
#include <bcm_int/dnx/algo/tunnel/algo_tunnel.h>
#include <bcm/flow.h>
#include <bcm_int/dnx/flow/flow.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/qos/algo_qos.h>
#include <bcm_int/dnx/qos/qos.h>
#include "tunnel_term_flow.h"
#include <src/bcm/dnx/flow/app_defs/ip_tunnel/flow_ipvx_tunnel_terminator.h>
#include <src/bcm/dnx/tunnel/tunnel_term.h>
#include <src/bcm/dnx/tunnel/tunnel_types.h>

#define TUNNEL_TERM_FLOW_FIELD_IPV4_DIP "IPV4_DIP"
#define TUNNEL_TERM_FLOW_FIELD_IPV4_DIP_MASK "IPV4_DIP_MASK"
#define TUNNEL_TERM_FLOW_FIELD_IPV4_SIP "IPV4_SIP"
#define TUNNEL_TERM_FLOW_FIELD_IPV4_SIP_MASK "IPV4_SIP_MASK"
#define TUNNEL_TERM_FLOW_FIELD_VRF "VRF"
#define TUNNEL_TERM_FLOW_FIELD_IPV4_TUNNEL_TYPE "IPV4_TUNNEL_TYPE"
#define TUNNEL_TERM_FLOW_FIELD_IPVX_IPSEC_TUNNEL_TYPE "IPVX_IPSEC_TUNNEL_TYPE"
#define TUNNEL_TERM_FLOW_FIELD_SVTAG_TXSCI "SVTAG_TXSCI"
#define TUNNEL_TERM_FLOW_FIELD_NOF_LAYERS_TO_TERMINATE "NOF_LAYERS_TO_TERMINATE"
#define TUNNEL_TERM_FLOW_FIELD_FODO_ASSIGNMENT_MODE "FORWARD_DOMAIN_ASSIGNMENT_MODE"
#define TUNNEL_TERM_FLOW_FIELD_TUNNEL_CLASS "TUNNEL_CLASS"
#define TUNNEL_TERM_FLOW_FIELD_IPV6 "IPV6"
#define TUNNEL_TERM_FLOW_FIELD_IPV6_MASK "IPV6_MASK"
#define TUNNEL_TERM_FLOW_FIELD_IPV6_SIP "IPV6_SIP"
#define TUNNEL_TERM_FLOW_FIELD_IPV6_TUNNEL_TYPE "IPV6_TUNNEL_TYPE"
#define TUNNEL_TERM_FLOW_FIELD_TUNNEL_LIF_IS_INTERMEDIATE "IPV6_TUNNEL_LIF_IS_INTERMEDIATE"
#define TUNNEL_TERM_FLOW_FIELD_VSI "VSI"
#define TUNNEL_TERM_FLOW_FIELD_IPVX_TUNNEL_TYPE "IPVX_TUNNEL_TYPE"

#define DNX_TUNNEL_TERM_FLOW_COMMON_FIELD_HAS_VSI(_tunnel) (_tunnel->type == bcmTunnelTypeEthIn6)

shr_error_e
dnx_tunnel_term_create_flow_common_fields_set(
    int unit,
    bcm_tunnel_terminator_t * tunnel,
    bcm_flow_terminator_info_t * flow_tunnel_term)
{
    SHR_FUNC_INIT_VARS(unit);

    flow_tunnel_term->flags = 0;
    /** Setting all common fields, consistent with legacy API */
    flow_tunnel_term->valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_QOS_INGRESS_MODEL_VALID;
    sal_memcpy(&flow_tunnel_term->ingress_qos_model, &tunnel->ingress_qos_model, sizeof(tunnel->ingress_qos_model));
    flow_tunnel_term->ingress_qos_model.ingress_ecn = bcmQosIngressEcnModelEligible;

    if (tunnel->default_vrf != 0)
    {
        flow_tunnel_term->valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_VRF_VALID;
        flow_tunnel_term->vrf = tunnel->default_vrf;
    }

    if (_SHR_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_TERM_CROSS_CONNECT))
    {
        flow_tunnel_term->valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID;
        flow_tunnel_term->l3_ingress_info.service_type = bcmFlowServiceTypeCrossConnect;
    }

    if (DNX_TUNNEL_TERM_FLOW_COMMON_FIELD_HAS_VSI(tunnel))
    {
        flow_tunnel_term->valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_VSI_VALID;
        flow_tunnel_term->vsi = tunnel->vlan;
    }

    /*
     * if vxlan and not replace
     */
    if (DNX_TUNNEL_TYPE_IS_VXLAN(tunnel->type) && !_SHR_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_REPLACE))
    {
        flow_tunnel_term->valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_LEARN_ENABLE_VALID;
        flow_tunnel_term->learn_enable = TRUE;
    }

    if (_SHR_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_TERM_STAT_ENABLE))
    {
        flow_tunnel_term->valid_elements_set |= (BCM_FLOW_TERMINATOR_ELEMENT_STAT_ID_VALID |
                                                 BCM_FLOW_TERMINATOR_ELEMENT_STAT_PP_PROFILE_VALID);
    }

    if (_SHR_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_TERM_WIDE))
    {
        flow_tunnel_term->valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_ADDITIONAL_DATA_VALID;
    }

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_tunnel_term_create_flow_special_fields_set(
    int unit,
    bcm_tunnel_terminator_t * tunnel,
    bcm_flow_special_fields_t * special_fields)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


void
dnx_tunnel_term_match_flow_app_get(
    int unit,
    bcm_tunnel_terminator_t * tunnel,
    char **flow_app)
{
    if (flow_legacy_ipvx_match_app_is_valid(unit))
    {
        *flow_app = FLOW_APP_NAME_IPV4_P2P_EM_TERM_MATCH;
    }
}

static shr_error_e
dnx_tunnel_term_flow_match_ipv4_p2p_special_fields_fill(
    int unit,
    bcm_tunnel_terminator_t * tunnel,
    bcm_flow_handle_t flow_handle,
    bcm_flow_special_fields_t * special_fields)
{
    bcm_flow_field_id_t field_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_IPV4_DIP, &field_id));
    SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, field_id, tunnel->dip);
    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_IPV4_SIP, &field_id));
    SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, field_id, tunnel->sip);
    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_VRF, &field_id));
    SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, field_id, tunnel->vrf);
    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get
                    (unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_IPV4_TUNNEL_TYPE, &field_id));
    SPECIAL_FIELD_SYMBOL_DATA_ADD(special_fields, field_id, tunnel->type);
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_term_flow_match_ipv4_mp_special_fields_fill(
    int unit,
    bcm_tunnel_terminator_t * tunnel,
    bcm_flow_handle_t flow_handle,
    bcm_flow_special_fields_t * special_fields)
{
    bcm_flow_field_id_t field_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_IPV4_DIP, &field_id));
    SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, field_id, tunnel->dip);
    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_VRF, &field_id));
    SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, field_id, tunnel->vrf);
    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get
                    (unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_IPV4_TUNNEL_TYPE, &field_id));
    SPECIAL_FIELD_SYMBOL_DATA_ADD(special_fields, field_id, tunnel->type);
exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_tunnel_term_flow_match_ipv4_tcam_special_fields_fill(
    int unit,
    bcm_tunnel_terminator_t * tunnel,
    bcm_flow_handle_t flow_handle,
    bcm_flow_special_fields_t * special_fields)
{
    bcm_flow_field_id_t field_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_IPV4_DIP, &field_id));
    SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, field_id, tunnel->dip);
    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get
                    (unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_IPV4_DIP_MASK, &field_id));
    SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, field_id, tunnel->dip_mask);
    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_IPV4_SIP, &field_id));
    SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, field_id, tunnel->sip);
    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get
                    (unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_IPV4_SIP_MASK, &field_id));
    SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, field_id, tunnel->sip_mask);
    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_VRF, &field_id));
    SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, field_id, tunnel->vrf);
    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get
                    (unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_IPV4_TUNNEL_TYPE, &field_id));
    SPECIAL_FIELD_SYMBOL_DATA_ADD(special_fields, field_id, tunnel->type);
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_term_flow_match_ipv4_special_fields_fill(
    int unit,
    bcm_tunnel_terminator_t * tunnel,
    bcm_flow_handle_t flow_handle,
    bcm_flow_special_fields_t * special_fields)
{
    SHR_FUNC_INIT_VARS(unit);

    if (DNX_TUNNEL_TERM_IS_IPV4_P2P(tunnel))
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_term_flow_match_ipv4_p2p_special_fields_fill
                        (unit, tunnel, flow_handle, special_fields));
    }
    else if (DNX_TUNNEL_TERM_IS_IPV4_MP(tunnel))
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_term_flow_match_ipv4_mp_special_fields_fill
                        (unit, tunnel, flow_handle, special_fields));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_term_flow_match_ipv4_tcam_special_fields_fill
                        (unit, tunnel, flow_handle, special_fields));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_flow_match_ipsec_fill_special_fields(
    int unit,
    bcm_tunnel_terminator_t * tunnel,
    bcm_flow_handle_t flow_handle,
    bcm_flow_special_fields_t * special_fields)
{
    bcm_flow_field_id_t field_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get
                    (unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_SVTAG_TXSCI, &field_id));
    SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, field_id, tunnel->sci);
    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get
                    (unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_IPVX_IPSEC_TUNNEL_TYPE, &field_id));
    SPECIAL_FIELD_SYMBOL_DATA_ADD(special_fields, field_id, tunnel->type);
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_term_flow_tunnel_gport_to_flow_lif_gport(
    int unit,
    bcm_gport_t tunnel_gport,
    bcm_gport_t * flow_lif_gport)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, tunnel_gport, _SHR_GPORT_TYPE_FLOW_LIF, DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR,
                     flow_lif_gport));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_term_flow_match_ipv6_p2p_special_fields_fill(
    int unit,
    bcm_tunnel_terminator_t * tunnel,
    bcm_flow_handle_t flow_handle,
    bcm_flow_special_fields_t * special_fields)
{
    bcm_flow_field_id_t field_id = 0;
    bcm_gport_t flow_gport_tunnel_id;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get
                    (unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_TUNNEL_CLASS, &field_id));
    SHR_IF_ERR_EXIT(dnx_tunnel_term_flow_tunnel_gport_to_flow_lif_gport
                    (unit, tunnel->tunnel_class, &flow_gport_tunnel_id));
    SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, field_id, flow_gport_tunnel_id);
    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_IPV6_SIP, &field_id));
    SPECIAL_FIELD_UINT8_ARR_DATA_ADD(special_fields, field_id, tunnel->sip6);
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_term_flow_match_ipv6_tcam_table_key_special_fields_fill(
    int unit,
    dnx_tunnel_term_ipv6_tcam_table_key_t * tcam_table_key,
    bcm_flow_handle_t flow_handle,
    bcm_flow_special_fields_t * special_fields)
{
    bcm_flow_field_id_t field_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_VRF, &field_id));
    SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, field_id, tcam_table_key->vrf);
    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_IPV6, &field_id));
    SPECIAL_FIELD_UINT8_ARR_DATA_ADD(special_fields, field_id, tcam_table_key->dip6);
    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_IPV6_MASK, &field_id));
    SPECIAL_FIELD_UINT8_ARR_DATA_ADD(special_fields, field_id, tcam_table_key->dip6_mask);
    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get
                    (unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_IPV6_TUNNEL_TYPE, &field_id));
    SPECIAL_FIELD_SYMBOL_DATA_ADD(special_fields, field_id, tcam_table_key->type);
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_term_flow_match_ipv6_mp_special_fields_fill(
    int unit,
    bcm_tunnel_terminator_t * tunnel,
    bcm_flow_handle_t flow_handle,
    bcm_flow_special_fields_t * special_fields)
{
    dnx_tunnel_term_ipv6_tcam_table_key_t tcam_table_key;
    SHR_FUNC_INIT_VARS(unit);
    dnx_tunnel_term_to_ipv6_tcam_table_key(tunnel, &tcam_table_key);
    SHR_IF_ERR_EXIT(dnx_tunnel_term_flow_match_ipv6_tcam_table_key_special_fields_fill
                    (unit, &tcam_table_key, flow_handle, special_fields));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_term_flow_match_ipv6_special_fields_fill(
    int unit,
    bcm_tunnel_terminator_t * tunnel,
    bcm_flow_handle_t flow_handle,
    bcm_flow_special_fields_t * special_fields)
{
    SHR_FUNC_INIT_VARS(unit);

    if (DNX_TUNNEL_TERM_IS_IPV6_P2P(tunnel))
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_term_flow_match_ipv6_p2p_special_fields_fill
                        (unit, tunnel, flow_handle, special_fields));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_term_flow_match_ipv6_mp_special_fields_fill
                        (unit, tunnel, flow_handle, special_fields));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_term_flow_match_special_fields_fill(
    int unit,
    bcm_tunnel_terminator_t * tunnel,
    bcm_flow_handle_t flow_handle,
    bcm_flow_special_fields_t * special_fields)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_tunnel_terminator_tunnel_type_is_ipv4(tunnel->type))
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_term_flow_match_ipv4_special_fields_fill(unit, tunnel, flow_handle, special_fields));
    }
    else if (dnx_tunnel_terminator_tunnel_type_is_txsci(tunnel->type))
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_flow_match_ipsec_fill_special_fields
                        (unit, tunnel, flow_handle, special_fields));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_term_flow_match_ipv6_special_fields_fill(unit, tunnel, flow_handle, special_fields));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * Update priority.
 * Returns flow_id
 */
static shr_error_e
dnx_tunnel_term_flow_match_get(
    int unit,
    bcm_tunnel_terminator_t * tunnel,
    bcm_gport_t * flow_id)
{
    bcm_flow_handle_info_t flow_handle_info = { 0 };
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_special_fields_t special_fields = { 0 };
    char *flow_app = NULL;

    SHR_FUNC_INIT_VARS(unit);

    dnx_tunnel_term_match_flow_app_get(unit, tunnel, &flow_app);
    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, flow_app, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;
    SHR_IF_ERR_EXIT(dnx_tunnel_term_flow_match_special_fields_fill(unit, tunnel, flow_handle, &special_fields));
    SHR_IF_ERR_EXIT(bcm_dnx_flow_match_info_get(unit, &flow_handle_info, &special_fields));
    tunnel->priority = flow_handle_info.flow_priority;
    *flow_id = flow_handle_info.flow_id;
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_term_flow_match_flow_id_get(
    int unit,
    bcm_tunnel_terminator_t * tunnel,
    bcm_gport_t * flow_id)
{
    bcm_flow_handle_info_t flow_handle_info = { 0 };
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_special_fields_t special_fields = { 0 };
    char *flow_app = NULL;

    SHR_FUNC_INIT_VARS(unit);

    dnx_tunnel_term_match_flow_app_get(unit, tunnel, &flow_app);
    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, flow_app, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;
    SHR_IF_ERR_EXIT(dnx_tunnel_term_flow_match_special_fields_fill(unit, tunnel, flow_handle, &special_fields));
    SHR_IF_ERR_EXIT(bcm_dnx_flow_match_info_get(unit, &flow_handle_info, &special_fields));
    *flow_id = flow_handle_info.flow_id;
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_term_flow_lif_create_with_id_and_replace_flag_set(
    int unit,
    bcm_tunnel_terminator_t * tunnel,
    bcm_flow_handle_info_t * flow_handle_info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (tunnel->flags & BCM_TUNNEL_TERM_TUNNEL_WITH_ID)
    {
        flow_handle_info->flags |= BCM_FLOW_HANDLE_INFO_WITH_ID;
        SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                        (unit, tunnel->tunnel_id, _SHR_GPORT_TYPE_FLOW_LIF,
                         DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR, &flow_handle_info->flow_id));
    }

    if (_SHR_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_REPLACE))
    {
        flow_handle_info->flags |= BCM_FLOW_HANDLE_INFO_REPLACE;
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_term_flow_match_create(
    int unit,
    bcm_tunnel_terminator_t * tunnel,
    bcm_gport_t flow_id)
{
    bcm_flow_handle_info_t flow_handle_info = { 0 };
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_special_fields_t special_fields = { 0 };
    char *flow_app = NULL;

    SHR_FUNC_INIT_VARS(unit);

    flow_handle_info.flow_id = flow_id;
    dnx_tunnel_term_match_flow_app_get(unit, tunnel, &flow_app);
    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, flow_app, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;
    flow_handle_info.flow_priority = tunnel->priority;
    SHR_IF_ERR_EXIT(dnx_tunnel_term_flow_lif_create_with_id_and_replace_flag_set(unit, tunnel, &flow_handle_info));
    SHR_IF_ERR_EXIT(dnx_tunnel_term_flow_match_special_fields_fill(unit, tunnel, flow_handle, &special_fields));
    SHR_IF_ERR_EXIT(bcm_dnx_flow_match_info_add(unit, &flow_handle_info, &special_fields));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_term_flow_lif_create(
    int unit,
    bcm_tunnel_terminator_t * tunnel,
    bcm_gport_t * flow_id)
{
    bcm_flow_handle_info_t flow_handle_info = { 0 };
    bcm_flow_terminator_info_t flow_tunnel_term = { 0 };
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_special_fields_t flow_special_fields = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_IPVX_TERMINATOR, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;

    SHR_IF_ERR_EXIT(dnx_tunnel_term_flow_lif_create_with_id_and_replace_flag_set(unit, tunnel, &flow_handle_info));
    SHR_IF_ERR_EXIT(dnx_tunnel_term_create_flow_common_fields_set(unit, tunnel, &flow_tunnel_term));
    SHR_IF_ERR_EXIT(dnx_tunnel_term_create_flow_special_fields_set(unit, tunnel, &flow_special_fields));
    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_create
                    (unit, &flow_handle_info, &flow_tunnel_term, &flow_special_fields));
    BCM_GPORT_TUNNEL_ID_SET(tunnel->tunnel_id, _SHR_GPORT_FLOW_LIF_VAL_GET_ID(flow_handle_info.flow_id));
    *flow_id = flow_handle_info.flow_id;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_term_create_flow(
    int unit,
    bcm_tunnel_terminator_t * tunnel)
{
    bcm_gport_t flow_id;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_tunnel_term_flow_lif_create(unit, tunnel, &flow_id));
    SHR_IF_ERR_EXIT(dnx_tunnel_term_flow_match_create(unit, tunnel, flow_id));
exit:
    SHR_FUNC_EXIT;
}

static void
dnx_tunnel_term_get_bcm_struct_fill_default_vrf(
    int unit,
    bcm_tunnel_terminator_t * tunnel,
    bcm_flow_terminator_info_t * flow_tunnel_term)
{
    if (DNX_TUNNEL_TYPE_IS_VXLAN_GPE(tunnel->type))
    {
        if (_SHR_IS_FLAG_SET(flow_tunnel_term->valid_elements_set, BCM_FLOW_TERMINATOR_ELEMENT_VRF_VALID))
        {
            tunnel->default_vrf = flow_tunnel_term->vrf;
        }
    }
}

static void
dnx_tunnel_term_get_bcm_struct_fill_stat_enable(
    int unit,
    bcm_tunnel_terminator_t * tunnel,
    bcm_flow_terminator_info_t * flow_tunnel_term)
{
    if (_SHR_IS_FLAG_SET(flow_tunnel_term->valid_elements_set, BCM_FLOW_TERMINATOR_ELEMENT_STAT_ID_VALID)
        && _SHR_IS_FLAG_SET(flow_tunnel_term->valid_elements_set, BCM_FLOW_TERMINATOR_ELEMENT_STAT_PP_PROFILE_VALID))
    {
        tunnel->flags |= BCM_TUNNEL_TERM_STAT_ENABLE;
    }
}

static void
dnx_tunnel_term_get_bcm_struct_fill_wide_flag(
    int unit,
    bcm_tunnel_terminator_t * tunnel,
    bcm_flow_terminator_info_t * flow_tunnel_term)
{
    if (_SHR_IS_FLAG_SET(flow_tunnel_term->valid_elements_set, BCM_FLOW_TERMINATOR_ELEMENT_ADDITIONAL_DATA_VALID))
    {
        tunnel->flags |= BCM_TUNNEL_TERM_WIDE;
    }
}

static shr_error_e
dnx_tunnel_term_get_bcm_struct_fill(
    int unit,
    bcm_gport_t flow_id,
    bcm_tunnel_terminator_t * tunnel)
{
    bcm_flow_handle_info_t flow_handle_info = { 0 };
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_terminator_info_t flow_tunnel_term = { 0 };
    bcm_flow_special_fields_t flow_special_fields = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    flow_handle_info.flow_id = flow_id;
    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_IPVX_TERMINATOR, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;
    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_get(unit, &flow_handle_info, &flow_tunnel_term, &flow_special_fields));

    sal_memcpy(&tunnel->ingress_qos_model, &flow_tunnel_term.ingress_qos_model,
               sizeof(flow_tunnel_term.ingress_qos_model));

    dnx_tunnel_term_get_bcm_struct_fill_default_vrf(unit, tunnel, &flow_tunnel_term);
    dnx_tunnel_term_get_bcm_struct_fill_stat_enable(unit, tunnel, &flow_tunnel_term);
    dnx_tunnel_term_get_bcm_struct_fill_wide_flag(unit, tunnel, &flow_tunnel_term);
    BCM_GPORT_TUNNEL_ID_SET(tunnel->tunnel_id, _SHR_GPORT_FLOW_LIF_VAL_GET_ID(flow_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_term_get_flow(
    int unit,
    bcm_tunnel_terminator_t * tunnel)
{
    bcm_gport_t flow_id;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_tunnel_term_flow_match_get(unit, tunnel, &flow_id));
    SHR_IF_ERR_EXIT(dnx_tunnel_term_get_bcm_struct_fill(unit, flow_id, tunnel));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_term_flow_match_delete(
    int unit,
    bcm_tunnel_terminator_t * tunnel)
{
    bcm_flow_handle_info_t flow_handle_info = { 0 };
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_special_fields_t special_fields = { 0 };
    char *flow_app = NULL;

    SHR_FUNC_INIT_VARS(unit);

    dnx_tunnel_term_match_flow_app_get(unit, tunnel, &flow_app);
    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, flow_app, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;
    SHR_IF_ERR_EXIT(dnx_tunnel_term_flow_match_special_fields_fill(unit, tunnel, flow_handle, &special_fields));
    SHR_IF_ERR_EXIT(bcm_dnx_flow_match_info_delete(unit, &flow_handle_info, &special_fields));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_term_flow_lif_delete(
    int unit,
    bcm_gport_t flow_id)
{
    bcm_flow_handle_info_t flow_handle_info = { 0 };
    bcm_flow_handle_t flow_handle = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_IPVX_TERMINATOR, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;
    flow_handle_info.flow_id = flow_id;
    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_destroy(unit, &flow_handle_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_term_delete_flow(
    int unit,
    bcm_tunnel_terminator_t * tunnel)
{
    bcm_gport_t flow_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_tunnel_term_flow_match_flow_id_get(unit, tunnel, &flow_id));
    SHR_IF_ERR_EXIT(dnx_tunnel_term_flow_match_delete(unit, tunnel));
    SHR_IF_ERR_EXIT(dnx_tunnel_term_flow_lif_delete(unit, flow_id));
exit:
    SHR_FUNC_EXIT;
}

