/** \file tunnel_init_flow.c
 *  General TUNNEL encapsulation functionality using flow lif for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TUNNEL

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm/tunnel.h>
#include <bcm_int/dnx_dispatch.h>
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
#define TUNNEL_INIT_FLOW_FIELD_IPV4_TUNNEL_TYPE "IPV4_TUNNEL_TYPE"
#define TUNNEL_INIT_FLOW_FIELD_IPV4_INIT_GRE_KEY_USE_LB "IPV4_INIT_GRE_KEY_USE_LB"
#define TUNNEL_INIT_FLOW_FIELD_IPV4_INIT_GRE_WITH_SN "IPV4_INIT_GRE_WITH_SN"
#define TUNNEL_INIT_FLOW_FIELD_SRC_UDP_PORT "SRC_UDP_PORT"
#define TUNNEL_INIT_FLOW_FIELD_DST_UDP_PORT "DST_UDP_PORT"
#define TUNNEL_INIT_FLOW_FIELD_IPV4_HEADER_DF "IPV4_HEADER_DF"
#define TUNNEL_INIT_FLOW_FIELD_QOS_TTL "QOS_TTL"
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
                                            BCM_FLOW_INITIATOR_ELEMENT_QOS_MAP_ID_VALID);

    flow_tunnel_init->encap_access =
        (tunnel->encap_access == bcmEncapAccessInvalid) ? bcmEncapAccessTunnel1 : tunnel->encap_access;
    flow_tunnel_init->l3_intf_id = tunnel->l3_intf_id;
    flow_tunnel_init->qos_map_id = tunnel->qos_map_id;
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
    bcm_flow_initiator_info_t * flow_tunnel_init,
    bcm_flow_special_fields_t * special_fields,
    bcm_tunnel_initiator_t * tunnel)
{
    bcm_flow_field_id_t field_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, TUNNEL_INIT_FLOW_FIELD_IPV4_DIP, &field_id));
    SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, field_id, tunnel->dip);

    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, TUNNEL_INIT_FLOW_FIELD_IPV4_SIP, &field_id));
    SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, field_id, tunnel->sip);

    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get
                    (unit, flow_handle, TUNNEL_INIT_FLOW_FIELD_IPV4_TUNNEL_TYPE, &field_id));
    SPECIAL_FIELD_SYMBOL_DATA_ADD(special_fields, field_id, tunnel->type);

    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, TUNNEL_INIT_FLOW_FIELD_QOS_TTL, &field_id));
    SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, field_id, tunnel->ttl);

    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, TUNNEL_INIT_FLOW_FIELD_QOS_DSCP, &field_id));
    SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, field_id, tunnel->dscp);

    if (tunnel->type == bcmTunnelTypeGreAnyIn4)
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
    else if (tunnel->type == bcmTunnelTypeUdp)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get
                        (unit, flow_handle, TUNNEL_INIT_FLOW_FIELD_SRC_UDP_PORT, &field_id));
        SPECIAL_FIELD_SYMBOL_DATA_ADD(special_fields, field_id, tunnel->udp_src_port);

        SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get
                        (unit, flow_handle, TUNNEL_INIT_FLOW_FIELD_DST_UDP_PORT, &field_id));
        SPECIAL_FIELD_SYMBOL_DATA_ADD(special_fields, field_id, tunnel->udp_dst_port);
    }

    if (_SHR_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_INIT_IPV4_SET_DF))
    {
        SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get
                        (unit, flow_handle, TUNNEL_INIT_FLOW_FIELD_IPV4_HEADER_DF, &field_id));
        SPECIAL_FIELD_ENABLER_ADD(special_fields, field_id);
    }

exit:
    SHR_FUNC_EXIT;
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

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_IPV4_INITIATOR, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;

    if (_SHR_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_REPLACE))
    {
        flow_handle_info.flags = BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID;
        flow_handle_info.flow_id = tunnel->tunnel_id;
    }

    /** WITH_ID flag is used - get global out-lif ID */
    if (_SHR_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_WITH_ID))
    {
        int is_tunneled = 0;
        bcm_gport_t tunnel_id = 0;
        /** Get tunnel GPORT if WITH_ID flag is set */
        SHR_IF_ERR_EXIT(dnx_algo_tunnel_initiator_gport_get(unit, intf->l3a_intf_id, tunnel, &tunnel_id, &is_tunneled));

        flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_WITH_ID;
        flow_handle_info.flow_id = tunnel_id;
    }

    SHR_IF_ERR_EXIT(dnx_tunnel_initiator_create_flow_common_fields_set(unit, &flow_tunnel_init, tunnel));
    SHR_IF_ERR_EXIT(dnx_tunnel_initiator_create_flow_special_fields_set
                    (unit, flow_handle, &flow_tunnel_init, &special_fields, tunnel));
    SHR_IF_ERR_EXIT(bcm_dnx_flow_initiator_info_create(unit, &flow_handle_info, &flow_tunnel_init, &special_fields));
    /** update returned Interface ID's */
    BCM_L3_ITF_SET(intf->l3a_intf_id, BCM_L3_ITF_TYPE_LIF, BCM_GPORT_TUNNEL_ID_GET(flow_handle_info.flow_id));

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
    int ii;
    bcm_flow_field_id_t dip_field_id = 0;
    bcm_flow_field_id_t sip_field_id = 0;
    bcm_flow_special_field_t special_field_data;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get
                    (unit, flow_handle, TUNNEL_INIT_FLOW_FIELD_IPV4_DIP, &dip_field_id));
    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get
                    (unit, flow_handle, TUNNEL_INIT_FLOW_FIELD_IPV4_SIP, &sip_field_id));

    for (ii = 0; ii < special_fields->actual_nof_special_fields; ii++)
    {
        if (special_fields->special_fields[ii].field_id == dip_field_id)
        {
            tunnel->dip = special_fields->special_fields[ii].shr_var_uint32;
        }
        if (special_fields->special_fields[ii].field_id == sip_field_id)
        {
            tunnel->sip = special_fields->special_fields[ii].shr_var_uint32;
        }
    }

    /** get ttl */
    sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
    SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_special_field_data_get
                              (unit, special_fields, FLOW_S_F_QOS_TTL, &special_field_data), _SHR_E_NOT_FOUND);
    tunnel->ttl = special_field_data.shr_var_uint32;

    /** get dscp */
    sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
    SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_special_field_data_get
                              (unit, special_fields, FLOW_S_F_QOS_DSCP, &special_field_data), _SHR_E_NOT_FOUND);
    tunnel->dscp = special_field_data.shr_var_uint32;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_initiator_get_flow(
    int unit,
    bcm_tunnel_initiator_t * tunnel)
{

    bcm_flow_initiator_info_t flow_tunnel_init = { 0 };
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_handle_info_t flow_handle_info = { 0 };
    bcm_flow_special_fields_t special_fields = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_IPV4_INITIATOR, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;

    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(flow_handle_info.flow_id, tunnel->tunnel_id);
    SHR_IF_ERR_EXIT(bcm_dnx_flow_initiator_info_get(unit, &flow_handle_info, &flow_tunnel_init, &special_fields));

    SHR_IF_ERR_EXIT(dnx_tunnel_initiator_get_flow_common_fields(unit, &flow_tunnel_init, tunnel));
    SHR_IF_ERR_EXIT(dnx_tunnel_initiator_get_flow_special_fields
                    (unit, flow_handle, &flow_tunnel_init, &special_fields, tunnel));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_initiator_clear_flow(
    int unit,
    bcm_gport_t tunnel_gport)
{
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_handle_info_t flow_handle_info = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_IPV4_INITIATOR, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;
    flow_handle_info.flow_id = tunnel_gport;

    SHR_IF_ERR_EXIT(bcm_dnx_flow_initiator_info_destroy(unit, &flow_handle_info));

exit:
    SHR_FUNC_EXIT;
}
