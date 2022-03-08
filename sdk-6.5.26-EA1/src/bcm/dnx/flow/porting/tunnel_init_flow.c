/** \file tunnel_init_flow.c
 *  General TUNNEL encapsulation functionality using flow lif for DNX.
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
#include <bcm_int/common/debug.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm_int/dnx/flow/flow.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/qos/algo_qos.h>
#include <bcm_int/dnx/qos/qos.h>
#include "tunnel_init_flow.h"

#define TUNNEL_INIT_FLOW_FIELD_IPV4_DIP "IPV4_DIP"
#define TUNNEL_INIT_FLOW_FIELD_IPV4_SIP "IPV4_SIP"
#define TUNNEL_INIT_FLOW_FIELD_IPV6_DIP "IPV6_DIP"
#define TUNNEL_INIT_FLOW_FIELD_IPV6_SIP "IPV6_SIP"
#define TUNNEL_INIT_FLOW_FIELD_IPV6_TUNNEL_TYPE "IPV6_TUNNEL_TYPE"
#define TUNNEL_INIT_FLOW_FIELD_IPV4_TUNNEL_TYPE "IPV4_TUNNEL_TYPE"
#define TUNNEL_INIT_FLOW_FIELD_IPV4_INIT_GRE_KEY_USE_LB "IPVX_INIT_GRE_KEY_USE_LB"
#define TUNNEL_INIT_FLOW_FIELD_IPV4_INIT_GRE_WITH_SN "IPV4_INIT_GRE_WITH_SN"
#define TUNNEL_INIT_FLOW_FIELD_SRC_UDP_PORT "SRC_UDP_PORT"
#define TUNNEL_INIT_FLOW_FIELD_DST_UDP_PORT "DST_UDP_PORT"
#define TUNNEL_INIT_FLOW_FIELD_IPV4_HEADER_DF "IPV4_HEADER_DF"
#define TUNNEL_INIT_FLOW_FIELD_QOS_DSCP "QOS_DSCP"

shr_error_e
dnx_tunnel_initiator_create_flow_common_fields_set(
    int unit,
    bcm_flow_initiator_info_t * flow_tunnel_init,
    bcm_tunnel_initiator_t * tunnel)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Setting all common fields, consistent with legacy API
     */
    flow_tunnel_init->valid_elements_set = (BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID |
                                            BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID |
                                            BCM_FLOW_INITIATOR_ELEMENT_QOS_MAP_ID_VALID |
                                            BCM_FLOW_INITIATOR_ELEMENT_TTL_VALID);

    if (_SHR_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_INIT_STAT_ENABLE))
    {
        flow_tunnel_init->valid_elements_set |= (BCM_FLOW_INITIATOR_ELEMENT_STAT_ID_VALID |
                                                 BCM_FLOW_INITIATOR_ELEMENT_STAT_PP_PROFILE_VALID);
    }

    flow_tunnel_init->encap_access =
        (tunnel->encap_access == bcmEncapAccessInvalid) ? bcmEncapAccessTunnel1 : tunnel->encap_access;
    flow_tunnel_init->l3_intf_id = tunnel->l3_intf_id;
    flow_tunnel_init->qos_map_id = tunnel->qos_map_id;
    flow_tunnel_init->ttl = tunnel->ttl;
    sal_memcpy(&flow_tunnel_init->egress_qos_model, &tunnel->egress_qos_model, sizeof(tunnel->egress_qos_model));
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_initiator_create_flow_special_fields_set(
    int unit,
    bcm_flow_handle_t flow_handle,
    bcm_gport_t flow_id,
    bcm_flow_initiator_info_t * flow_tunnel_init,
    bcm_flow_special_fields_t * special_fields,
    bcm_tunnel_initiator_t * tunnel)
{
    bcm_flow_field_id_t field_id = 0;
    bcm_tunnel_type_t tunnel_type;
    uint32 is_ipv6 = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    dnx_algo_tunnel_init_ipv6_is_ipv6_tunnel(unit, tunnel->type, &is_ipv6);

    if (is_ipv6)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get
                        (unit, flow_handle, TUNNEL_INIT_FLOW_FIELD_IPV6_DIP, &field_id));
        SPECIAL_FIELD_UINT8_ARR_DATA_ADD(special_fields, field_id, tunnel->dip6);

        SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get
                        (unit, flow_handle, TUNNEL_INIT_FLOW_FIELD_IPV6_SIP, &field_id));
        SPECIAL_FIELD_UINT8_ARR_DATA_ADD(special_fields, field_id, tunnel->sip6);
    }
    else
    {
        SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get
                        (unit, flow_handle, TUNNEL_INIT_FLOW_FIELD_IPV4_DIP, &field_id));
        SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, field_id, tunnel->dip);

        SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get
                        (unit, flow_handle, TUNNEL_INIT_FLOW_FIELD_IPV4_SIP, &field_id));
        SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, field_id, tunnel->sip);
    }

    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, TUNNEL_INIT_FLOW_FIELD_QOS_DSCP, &field_id));
    SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, field_id, tunnel->dscp);

    if (_SHR_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_INIT_IPV4_SET_DF))
    {
        SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get
                        (unit, flow_handle, TUNNEL_INIT_FLOW_FIELD_IPV4_HEADER_DF, &field_id));
        SPECIAL_FIELD_ENABLER_ADD(special_fields, field_id);
    }

    /*
     * special fields which can't be updated at replace 
     */
    if (!_SHR_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_REPLACE))
    {
        char *tunnel_type_flow_field =
            (is_ipv6 ? TUNNEL_INIT_FLOW_FIELD_IPV6_TUNNEL_TYPE : TUNNEL_INIT_FLOW_FIELD_IPV4_TUNNEL_TYPE);
        SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, tunnel_type_flow_field, &field_id));
        SPECIAL_FIELD_SYMBOL_DATA_ADD(special_fields, field_id, tunnel->type);
        tunnel_type = tunnel->type;
    }
    else
    {
        /*
         * get the entry using api 
         */
        bcm_tunnel_initiator_t previous_tunnel;
        bcm_l3_intf_t previous_l3_intf;
        bcm_l3_intf_t_init(&previous_l3_intf);
        bcm_tunnel_initiator_t_init(&previous_tunnel);
        BCM_GPORT_TUNNEL_ID_SET(previous_tunnel.tunnel_id, _SHR_GPORT_FLOW_LIF_VAL_GET_ID(flow_id));
        if (is_ipv6)
        {
            SHR_IF_ERR_EXIT(dnx_tunnel_initiator_ipv6_get_flow(unit, &previous_l3_intf, &previous_tunnel));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_tunnel_initiator_ipv4_get_flow(unit, &previous_l3_intf, &previous_tunnel));
        }
        tunnel_type = previous_tunnel.type;
    }

    if ((tunnel_type == bcmTunnelTypeGreAnyIn4) || (tunnel_type == bcmTunnelTypeGreAnyIn6))
    {
        if (_SHR_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_INIT_GRE_KEY_USE_LB))
        {
            SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get
                            (unit, flow_handle, TUNNEL_INIT_FLOW_FIELD_IPV4_INIT_GRE_KEY_USE_LB, &field_id));
            SPECIAL_FIELD_ENABLER_ADD(special_fields, field_id);
        }
        else if (_SHR_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_INIT_GRE_WITH_SN))
        {
            SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get
                            (unit, flow_handle, TUNNEL_INIT_FLOW_FIELD_IPV4_INIT_GRE_WITH_SN, &field_id));
            SPECIAL_FIELD_ENABLER_ADD(special_fields, field_id);
        }
    }
    else if ((tunnel_type == bcmTunnelTypeUdp) || (tunnel_type == bcmTunnelTypeUdp6))
    {
        SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get
                        (unit, flow_handle, TUNNEL_INIT_FLOW_FIELD_SRC_UDP_PORT, &field_id));
        SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, field_id, tunnel->udp_src_port);

        SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get
                        (unit, flow_handle, TUNNEL_INIT_FLOW_FIELD_DST_UDP_PORT, &field_id));
        SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, field_id, tunnel->udp_dst_port);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_init_flow_flow_id_get(
    int unit,
    bcm_l3_intf_t * intf,
    bcm_tunnel_initiator_t * tunnel,
    bcm_gport_t * flow_id)
{
    int is_tunneled = 0;
    bcm_gport_t tunnel_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_tunnel_initiator_gport_get(unit, intf->l3a_intf_id, tunnel, &tunnel_id, &is_tunneled));
    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, tunnel_id, _SHR_GPORT_TYPE_FLOW_LIF, DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_INITIATOR, flow_id));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_init_flow_lif_create_with_id_and_replace_flag_set(
    int unit,
    bcm_l3_intf_t * intf,
    bcm_tunnel_initiator_t * tunnel,
    bcm_flow_handle_info_t * flow_handle_info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_WITH_ID))
    {
        flow_handle_info->flags |= BCM_FLOW_HANDLE_INFO_WITH_ID;
        SHR_IF_ERR_EXIT(dnx_tunnel_init_flow_flow_id_get(unit, intf, tunnel, &flow_handle_info->flow_id));
    }

    if (_SHR_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_REPLACE))
    {
        flow_handle_info->flags |= BCM_FLOW_HANDLE_INFO_REPLACE;
    }
exit:
    SHR_FUNC_EXIT;
}

void
dnx_tunnel_initiator_flow_app_get(
    int unit,
    bcm_tunnel_initiator_t * tunnel,
    char **flow_app)
{
    uint32 is_ipv6 = FALSE;

    dnx_algo_tunnel_init_ipv6_is_ipv6_tunnel(unit, tunnel->type, &is_ipv6);
    if (is_ipv6)
    {
        *flow_app = FLOW_APP_NAME_IPV6_INITIATOR;
    }
    else
    {
        *flow_app = FLOW_APP_NAME_IPV4_INITIATOR;
    }
}

shr_error_e
dnx_tunnel_initiator_create_flow(
    int unit,
    bcm_l3_intf_t * intf,
    bcm_tunnel_initiator_t * tunnel)
{
    bcm_flow_initiator_info_t flow_tunnel_init = { 0 };
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_handle_info_t flow_handle_info = { 0 };
    bcm_flow_special_fields_t special_fields = { 0 };
    char *flow_app = NULL;

    SHR_FUNC_INIT_VARS(unit);
    dnx_tunnel_initiator_flow_app_get(unit, tunnel, &flow_app);
    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, flow_app, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;

    SHR_IF_ERR_EXIT(dnx_tunnel_init_flow_lif_create_with_id_and_replace_flag_set
                    (unit, intf, tunnel, &flow_handle_info));
    SHR_IF_ERR_EXIT(dnx_tunnel_initiator_create_flow_common_fields_set(unit, &flow_tunnel_init, tunnel));
    SHR_IF_ERR_EXIT(dnx_tunnel_initiator_create_flow_special_fields_set
                    (unit, flow_handle, flow_handle_info.flow_id, &flow_tunnel_init, &special_fields, tunnel));
    if (_SHR_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_REPLACE))
    {
        flow_tunnel_init.encap_access = bcmEncapAccessInvalid;
    }
    SHR_IF_ERR_EXIT(bcm_dnx_flow_initiator_info_create(unit, &flow_handle_info, &flow_tunnel_init, &special_fields));

    BCM_GPORT_TUNNEL_ID_SET(tunnel->tunnel_id, _SHR_GPORT_FLOW_LIF_VAL_GET_ID(flow_handle_info.flow_id));
    BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(intf->l3a_intf_id, flow_handle_info.flow_id);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_initiator_get_flow_common_fields(
    int unit,
    bcm_flow_initiator_info_t * flow_tunnel_init,
    bcm_tunnel_initiator_t * tunnel)
{
    SHR_FUNC_INIT_VARS(unit);

    tunnel->l3_intf_id = flow_tunnel_init->l3_intf_id;
    tunnel->encap_access = flow_tunnel_init->encap_access;
    tunnel->qos_map_id = flow_tunnel_init->qos_map_id;
    tunnel->ttl = flow_tunnel_init->ttl;
    sal_memcpy(&tunnel->egress_qos_model, &flow_tunnel_init->egress_qos_model, sizeof(tunnel->egress_qos_model));

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_initiator_get_flow_special_fields(
    int unit,
    bcm_flow_handle_t flow_handle,
    bcm_flow_initiator_info_t * flow_tunnel_init,
    bcm_flow_special_fields_t * special_fields,
    bcm_tunnel_initiator_t * tunnel)
{
    bcm_flow_special_field_t special_field_data;
    SHR_FUNC_INIT_VARS(unit);

    /** get dip */
    sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
    SHR_IF_ERR_EXIT(dnx_flow_special_field_data_get(unit, special_fields, FLOW_S_F_IPV4_DIP, &special_field_data));
    tunnel->dip = special_field_data.shr_var_uint32;

    /** get sip */
    sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
    SHR_IF_ERR_EXIT(dnx_flow_special_field_data_get(unit, special_fields, FLOW_S_F_IPV4_SIP, &special_field_data));
    tunnel->sip = special_field_data.shr_var_uint32;

    /** get type */
    sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
    SHR_IF_ERR_EXIT(dnx_flow_special_field_data_get
                    (unit, special_fields, FLOW_S_F_IPV4_TUNNEL_TYPE, &special_field_data));
    tunnel->type = (bcm_tunnel_type_t) special_field_data.symbol;

    if ((tunnel->type == bcmTunnelTypeGreAnyIn4) || (tunnel->type == bcmTunnelTypeGreAnyIn6))
    {
        shr_error_e rv;
        sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
        rv = dnx_flow_special_field_data_get
            (unit, special_fields, FLOW_S_F_IPVX_INIT_GRE_KEY_USE_LB, &special_field_data);
        if (rv == _SHR_E_NONE)
        {
            tunnel->flags |= BCM_TUNNEL_INIT_GRE_KEY_USE_LB;
        }

        sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
        rv = dnx_flow_special_field_data_get(unit, special_fields, FLOW_S_F_IPVX_INIT_GRE_WITH_SN, &special_field_data);
        if (rv == _SHR_E_NONE)
        {
            tunnel->flags |= BCM_TUNNEL_INIT_GRE_WITH_SN;
        }
    }

    /** get dscp */
    sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
    SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_special_field_data_get
                              (unit, special_fields, FLOW_S_F_QOS_DSCP, &special_field_data), _SHR_E_NOT_FOUND);
    tunnel->dscp = special_field_data.shr_var_uint32;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_initiator_get_flow(
    int unit,
    bcm_l3_intf_t * intf,
    bcm_tunnel_initiator_t * tunnel,
    char *flow_app)
{

    bcm_flow_initiator_info_t flow_tunnel_init = { 0 };
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_handle_info_t flow_handle_info = { 0 };
    bcm_flow_special_fields_t special_fields = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, flow_app, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;
    SHR_IF_ERR_EXIT(dnx_tunnel_init_flow_flow_id_get(unit, intf, tunnel, &flow_handle_info.flow_id));
    SHR_IF_ERR_EXIT(bcm_dnx_flow_initiator_info_get(unit, &flow_handle_info, &flow_tunnel_init, &special_fields));
    SHR_IF_ERR_EXIT(dnx_tunnel_initiator_get_flow_common_fields(unit, &flow_tunnel_init, tunnel));
    SHR_IF_ERR_EXIT(dnx_tunnel_initiator_get_flow_special_fields
                    (unit, flow_handle, &flow_tunnel_init, &special_fields, tunnel));
    BCM_GPORT_TUNNEL_ID_SET(tunnel->tunnel_id, _SHR_GPORT_FLOW_LIF_VAL_GET_ID(flow_handle_info.flow_id));
    BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(intf->l3a_intf_id, flow_handle_info.flow_id);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_initiator_ipv4_get_flow(
    int unit,
    bcm_l3_intf_t * intf,
    bcm_tunnel_initiator_t * tunnel)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_tunnel_initiator_get_flow(unit, intf, tunnel, FLOW_APP_NAME_IPV4_INITIATOR));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_initiator_ipv6_get_flow(
    int unit,
    bcm_l3_intf_t * intf,
    bcm_tunnel_initiator_t * tunnel)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_tunnel_initiator_get_flow(unit, intf, tunnel, FLOW_APP_NAME_IPV6_INITIATOR));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_initiator_clear_flow(
    int unit,
    bcm_gport_t tunnel_gport,
    char *flow_app)
{
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_handle_info_t flow_handle_info = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    flow_handle_info.flow_handle = flow_handle;
    flow_handle_info.flow_id = tunnel_gport;

    SHR_IF_ERR_EXIT(bcm_dnx_flow_initiator_info_destroy(unit, &flow_handle_info));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_initiator_ipv4_clear_flow(
    int unit,
    bcm_gport_t tunnel_gport)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_tunnel_initiator_clear_flow(unit, tunnel_gport, FLOW_APP_NAME_IPV4_INITIATOR));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_initiator_ipv6_clear_flow(
    int unit,
    bcm_gport_t tunnel_gport)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_tunnel_initiator_clear_flow(unit, tunnel_gport, FLOW_APP_NAME_IPV6_INITIATOR));
exit:
    SHR_FUNC_EXIT;
}
