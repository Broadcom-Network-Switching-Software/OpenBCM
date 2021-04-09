/** \file tunnel_term_flow.c
 *  General TUNNEL termination functionality using flow lif for DNX.
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
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/qos/algo_qos.h>
#include <bcm_int/dnx/qos/qos.h>
#include "tunnel_term_flow.h"

#define TUNNEL_FLOW_FLOW_IPV4_TERM_APP "IPV4_TUNNEL_TERMINATOR"
#define TUNNEL_FLOW_FLOW_IPV4_MATCH_EM_P2P "IPV4_TUNNEL_P2P_EM_MATCH"
#define TUNNEL_TERM_FLOW_FIELD_IPV4_DIP "IPV4_DIP"
#define TUNNEL_TERM_FLOW_FIELD_IPV4_SIP "IPV4_SIP"
#define TUNNEL_TERM_FLOW_FIELD_VRF "VRF"
#define TUNNEL_TERM_FLOW_FIELD_IPV4_TUNNEL_TYPE "IPV4_TUNNEL_TYPE"

shr_error_e
dnx_tunnel_terminator_create_flow_common_fields_set(
    int unit,
    bcm_flow_terminator_info_t * flow_tunnel_term,
    bcm_tunnel_terminator_t * tunnel)
{
    SHR_FUNC_INIT_VARS(unit);

    flow_tunnel_term->flags = 0;
    /*
     * Setting all common fields, consistent with legacy API
     */
    flow_tunnel_term->valid_elements_set = (BCM_FLOW_TERMINATOR_ELEMENT_VRF_VALID |
                                            BCM_FLOW_TERMINATOR_ELEMENT_STAT_ID_VALID |
                                            BCM_FLOW_TERMINATOR_ELEMENT_STAT_PP_PROFILE_VALID |
                                            BCM_FLOW_TERMINATOR_ELEMENT_QOS_INGRESS_MODEL_VALID |
                                            BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID |
                                            BCM_FLOW_TERMINATOR_ELEMENT_FLOW_SERVICE_TYPE_VALID);

    flow_tunnel_term->vrf = tunnel->vrf;
    flow_tunnel_term->service_type =
        (_SHR_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_TERM_CROSS_CONNECT)) ? bcmFlowServiceTypeCrossConnect :
        bcmFlowServiceTypeMultiPoint;

    sal_memcpy(&flow_tunnel_term->ingress_qos_model, &tunnel->ingress_qos_model, sizeof(tunnel->ingress_qos_model));
    flow_tunnel_term->ingress_qos_model.ingress_ecn = bcmQosIngressEcnModelEligible;

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_terminator_create_flow_match_lookup_config(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_tunnel_terminator_t * tunnel)
{
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_field_id_t field_id = 0;
    bcm_flow_special_fields_t special_fields = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, TUNNEL_FLOW_FLOW_IPV4_MATCH_EM_P2P, &flow_handle));
    flow_handle_info->flow_handle = flow_handle;

    /** Special fields */
    special_fields.actual_nof_special_fields = 4;

    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_IPV4_DIP, &field_id));
    special_fields.special_fields[0].field_id = field_id;
    special_fields.special_fields[0].shr_var_uint32 = tunnel->dip;

    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_IPV4_SIP, &field_id));
    special_fields.special_fields[1].field_id = field_id;
    special_fields.special_fields[1].shr_var_uint32 = tunnel->sip;

    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_VRF, &field_id));
    special_fields.special_fields[2].field_id = field_id;
    special_fields.special_fields[2].shr_var_uint32 = tunnel->vrf;

    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get
                    (unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_IPV4_TUNNEL_TYPE, &field_id));
    special_fields.special_fields[3].field_id = field_id;
    special_fields.special_fields[3].shr_var_uint32 = DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_GRE4;

    SHR_IF_ERR_EXIT(bcm_dnx_flow_match_info_add(unit, flow_handle_info, &special_fields));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_terminator_create_flow(
    int unit,
    bcm_tunnel_terminator_t * tunnel)
{
    bcm_flow_terminator_info_t flow_tunnel_term = { 0 };
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_handle_info_t flow_handle_info = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, TUNNEL_FLOW_FLOW_IPV4_TERM_APP, &flow_handle));

    flow_handle_info.flow_handle = flow_handle;

    if (_SHR_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_REPLACE))
    {
        flow_handle_info.flags = BCM_FLOW_HANDLE_INFO_REPLACE;
        flow_handle_info.flow_id = tunnel->tunnel_id;
    }

    /** allocate global lif  */
    if (tunnel->flags & BCM_TUNNEL_TERM_TUNNEL_WITH_ID)
    {
        flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_WITH_ID;
        flow_handle_info.flow_id = tunnel->tunnel_id;
    }

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_create_flow_common_fields_set(unit, &flow_tunnel_term, tunnel));
    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_create(unit, &flow_handle_info, &flow_tunnel_term, NULL));
    BCM_GPORT_TUNNEL_ID_SET(tunnel->tunnel_id, BCM_GPORT_TUNNEL_ID_GET(flow_handle_info.flow_id));
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_create_flow_match_lookup_config(unit, &flow_handle_info, tunnel));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_terminator_get_flow_match_lookup_get(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_tunnel_terminator_t * tunnel)
{
    int ii;
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_special_fields_t special_fields = { 0 };
    bcm_flow_field_id_t dip_field_id = 0;
    bcm_flow_field_id_t sip_field_id = 0;
    bcm_flow_field_id_t vrf_field_id = 0;
    bcm_flow_field_id_t tunnel_type_field_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, TUNNEL_FLOW_FLOW_IPV4_MATCH_EM_P2P, &flow_handle));
    flow_handle_info->flow_handle = flow_handle;

    /** Special fields */
    special_fields.actual_nof_special_fields = 4;

    SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_IPV4_DIP, &dip_field_id));
    special_fields.special_fields[0].field_id = dip_field_id;
    special_fields.special_fields[0].shr_var_uint32 = tunnel->dip;

    SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_IPV4_SIP, &sip_field_id));
    special_fields.special_fields[1].field_id = sip_field_id;
    special_fields.special_fields[1].shr_var_uint32 = tunnel->sip;

    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_VRF, &vrf_field_id));
    special_fields.special_fields[2].field_id = vrf_field_id;
    special_fields.special_fields[2].shr_var_uint32 = tunnel->default_vrf;

    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get
                    (unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_IPV4_TUNNEL_TYPE, &tunnel_type_field_id));
    special_fields.special_fields[3].field_id = tunnel_type_field_id;
    special_fields.special_fields[3].shr_var_uint32 = DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_GRE4;

    SHR_IF_ERR_EXIT(bcm_dnx_flow_match_info_get(unit, flow_handle_info, &special_fields));

    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get
                    (unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_IPV4_DIP, &dip_field_id));
    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get
                    (unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_IPV4_SIP, &sip_field_id));
    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_VRF, &vrf_field_id));

    for (ii = 0; ii < special_fields.actual_nof_special_fields; ii++)
    {
        if (special_fields.special_fields[ii].field_id == dip_field_id)
        {
            tunnel->dip = special_fields.special_fields[ii].shr_var_uint32;
        }
        if (special_fields.special_fields[ii].field_id == sip_field_id)
        {
            tunnel->sip = special_fields.special_fields[ii].shr_var_uint32;
        }
        if (special_fields.special_fields[ii].field_id == vrf_field_id)
        {
            tunnel->vrf = special_fields.special_fields[ii].shr_var_uint32;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_terminator_get_flow_common_fields_get(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_tunnel_terminator_t * tunnel)
{
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_terminator_info_t flow_tunnel_term = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, TUNNEL_FLOW_FLOW_IPV4_TERM_APP, &flow_handle));
    flow_handle_info->flow_handle = flow_handle;
    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_get(unit, flow_handle_info, &flow_tunnel_term, NULL));
    tunnel->vrf = flow_tunnel_term.vrf;
    sal_memcpy(&tunnel->ingress_qos_model, &flow_tunnel_term.ingress_qos_model,
               sizeof(flow_tunnel_term.ingress_qos_model));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_terminator_get_flow(
    int unit,
    bcm_tunnel_terminator_t * tunnel)
{
    bcm_flow_handle_info_t flow_handle_info = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_get_flow_match_lookup_get(unit, &flow_handle_info, tunnel));
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_get_flow_common_fields_get(unit, &flow_handle_info, tunnel));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_terminator_delete_flow(
    int unit,
    bcm_tunnel_terminator_t * tunnel)
{
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_handle_info_t flow_handle_info = { 0 };
    bcm_flow_special_fields_t special_fields = { 0 };
    bcm_flow_field_id_t dip_field_id = 0;
    bcm_flow_field_id_t sip_field_id = 0;
    bcm_flow_field_id_t vrf_field_id = 0;
    bcm_flow_field_id_t tunnel_type_field_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, TUNNEL_FLOW_FLOW_IPV4_MATCH_EM_P2P, &flow_handle));

    flow_handle_info.flow_handle = flow_handle;
    BCM_GPORT_TUNNEL_ID_SET(flow_handle_info.flow_id, tunnel->tunnel_id);

    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get
                    (unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_IPV4_DIP, &dip_field_id));
    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get
                    (unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_IPV4_SIP, &sip_field_id));
    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_VRF, &vrf_field_id));
    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get
                    (unit, flow_handle, TUNNEL_TERM_FLOW_FIELD_IPV4_TUNNEL_TYPE, &tunnel_type_field_id));

    /** Special fields */
    special_fields.actual_nof_special_fields = 4;

    special_fields.special_fields[0].field_id = sip_field_id;
    special_fields.special_fields[0].shr_var_uint32 = tunnel->sip;

    special_fields.special_fields[1].field_id = dip_field_id;
    special_fields.special_fields[1].shr_var_uint32 = tunnel->dip;

    special_fields.special_fields[2].field_id = vrf_field_id;
    special_fields.special_fields[2].shr_var_uint32 = tunnel->vrf;

    special_fields.special_fields[3].field_id = tunnel_type_field_id;
    special_fields.special_fields[3].shr_var_uint32 = DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_GRE4;

    SHR_IF_ERR_EXIT(bcm_dnx_flow_match_info_delete(unit, &flow_handle_info, &special_fields));

    /*
     * Delete terminator entry
     */
    SHR_IF_ERR_EXIT(bcm_flow_handle_get(unit, TUNNEL_FLOW_FLOW_IPV4_TERM_APP, &flow_handle));

    flow_handle_info.flow_handle = flow_handle;
    BCM_GPORT_TUNNEL_ID_SET(flow_handle_info.flow_id, tunnel->tunnel_id);

    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_destroy(unit, &flow_handle_info));

exit:
    SHR_FUNC_EXIT;
}
