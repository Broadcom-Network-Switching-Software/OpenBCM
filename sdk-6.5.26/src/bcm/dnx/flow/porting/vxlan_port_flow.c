/** \file vxlan_port_flow.c
 *  porting for vxlan port for flow.
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
#include "vxlan_port_flow.h"
#include <bcm/flow.h>
#include <bcm_int/dnx/flow/flow.h>
#include <bcm_int/dnx/auto_generated/dnx_flow_dispatch.h>
#include <src/bcm/dnx/flow/app_defs/ip_tunnel/flow_ipvx_tunnel_terminator.h>

/**********************
 * INTERNAL FUNCTIONS *
 **********************/
static shr_error_e
dnx_vxlan_port_flow_term_learning_fields_add(
    int unit,
    bcm_vxlan_port_t * vxlan_port,
    bcm_flow_terminator_info_t * common_fields)
{
    uint8 is_outlif_exists = _BCM_VXLAN_PORT_HAS_OUTLIF(vxlan_port);
    uint8 is_fec_exists = _BCM_VXLAN_PORT_HAS_FEC(vxlan_port);
    uint8 symmetric_flag = FALSE;
    dnx_algo_gpm_gport_hw_resources_t tunnel_term_gport_hw_resources = { 0 };
    dnx_algo_gpm_gport_hw_resources_t tunnel_init_gport_hw_resources = { 0 };
    uint32 destination;

    SHR_FUNC_INIT_VARS(unit);

    /** common fields bitmap update */
    common_fields->valid_elements_set |= (BCM_FLOW_TERMINATOR_ELEMENT_LEARN_ENABLE_VALID |
                                          BCM_FLOW_TERMINATOR_ELEMENT_L2_LEARN_INFO_VALID);

    /*
     * destination 
     */
    if (is_fec_exists)
    {
        /*
         * set destination with FEC
         */
        BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(destination, vxlan_port->egress_if);
    }
    else
    {
        /** dest_port */

        destination = vxlan_port->match_port;
    }
    common_fields->l2_learn_info.dest_port = destination;

    /** flags */
    /** encap id & symmetric flag */
    if (is_outlif_exists)
    {
        /** encap_id */
        common_fields->l2_learn_info.encap_id = vxlan_port->egress_tunnel_id;
        SHR_IF_ERR_EXIT(dnx_vxlan_port_gport_hw_resources_get(unit, vxlan_port, &tunnel_term_gport_hw_resources,
                                                              &tunnel_init_gport_hw_resources));

        /** calculate the symmetric flag */
        symmetric_flag = _BCM_VXLAN_PORT_HAS_SYMMETRIC_IN_LIF_WITH_OUT_LIF(vxlan_port, &tunnel_term_gport_hw_resources,
                                                                           &tunnel_init_gport_hw_resources);
    }

    /** flags update */
    common_fields->l2_learn_info.l2_learn_info_flags = ((symmetric_flag) ? BCM_FLOW_HANDLE_INFO_SYMMETRIC_ALLOC : 0);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vxlan_port_flow_term_common_fields_add(
    int unit,
    bcm_vxlan_port_t * vxlan_port,
    bcm_flow_terminator_info_t * common_fields)
{
    SHR_FUNC_INIT_VARS(unit);

    /** update the l2_ingress_info */
    common_fields->l2_ingress_info.ingress_network_group_id = vxlan_port->network_group_id;
    common_fields->valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID;

    /** updating the default vpn */
    common_fields->valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_VSI_VALID;
    common_fields->vsi = vxlan_port->default_vpn;

    /** learning */
    SHR_IF_ERR_EXIT(dnx_vxlan_port_flow_term_learning_fields_add(unit, vxlan_port, common_fields));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vxlan_port_gport_l2_forward_info_add(
    int unit,
    bcm_vxlan_port_t * vxlan_port,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_l2_learn_info_t * l2_learn_info)
{
    dnx_algo_gpm_forward_info_t forward_info;
    SHR_FUNC_INIT_VARS(unit);
    sal_memset(&forward_info, 0, sizeof(dnx_algo_gpm_forward_info_t));
    SHR_IF_ERR_EXIT(dnx_flow_field_ipvx_term_forward_information_get(unit, l2_learn_info, &forward_info));
    SHR_IF_ERR_EXIT(algo_gpm_gport_l2_forward_info_add(unit,
                                                       _SHR_IS_FLAG_SET(vxlan_port->flags, BCM_VXLAN_PORT_REPLACE),
                                                       FALSE, flow_handle_info->flow_id, &forward_info));
exit:
    SHR_FUNC_EXIT;

}

static shr_error_e
dnx_vxlan_port_terminator_create_flow(
    int unit,
    bcm_vxlan_port_t * vxlan_port)
{
    bcm_flow_handle_info_t flow_handle_info = { 0 };
    bcm_flow_terminator_info_t common_fields = { 0 };
    bcm_flow_special_fields_t special_fields = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    /** get the currently data */
    /** get the flow_handle */
    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_IPVX_TERMINATOR, &(flow_handle_info.flow_handle)));

    /** get the flow_id */
    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert(unit, vxlan_port->match_tunnel_id, BCM_GPORT_TYPE_FLOW_LIF,
                                                DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR,
                                                &flow_handle_info.flow_id));

    /** updating the relevant data */
    /** common_fields */
    SHR_IF_ERR_EXIT(dnx_vxlan_port_flow_term_common_fields_add(unit, vxlan_port, &common_fields));

    /** flow_handle_info */
    flow_handle_info.flags = (BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID);

    /** update the application */
    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_create(unit, &flow_handle_info, &common_fields, &special_fields));

    SHR_IF_ERR_EXIT(dnx_vxlan_port_gport_l2_forward_info_add
                    (unit, vxlan_port, &flow_handle_info, &common_fields.l2_learn_info));

exit:
    SHR_FUNC_EXIT;
}

static void
dnx_vxlan_port_flow_init_common_fields_add(
    int unit,
    bcm_vxlan_port_t * vxlan_port,
    bcm_flow_initiator_info_t * common_fields)
{
    /** update the l2_ingress_info */
    common_fields->l2_egress_info.egress_network_group_id = vxlan_port->network_group_id;
    common_fields->valid_elements_set |= BCM_FLOW_INITIATOR_ELEMENT_L2_EGRESS_INFO_VALID;
}

static shr_error_e
dnx_vxlan_port_initiator_create_flow(
    int unit,
    bcm_vxlan_port_t * vxlan_port)
{
    bcm_flow_handle_info_t flow_handle_info = { 0 };
    bcm_flow_initiator_info_t common_fields = { 0 };
    bcm_flow_special_fields_t special_fields = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    /** get the currently data */
    /** get the flow_id */
    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert(unit, vxlan_port->egress_tunnel_id, BCM_GPORT_TYPE_FLOW_LIF,
                                                DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_INITIATOR,
                                                &flow_handle_info.flow_id));

    /** get the flow_handle */
    SHR_IF_ERR_EXIT(flow_lif_flow_app_from_gport_get
                    (unit, flow_handle_info.flow_id, FLOW_APP_TYPE_INIT, &(flow_handle_info.flow_handle)));

    /** updating the relevant data */
    /** common_fields */
    dnx_vxlan_port_flow_init_common_fields_add(unit, vxlan_port, &common_fields);

    /** flow_handle_info */
    flow_handle_info.flags = (BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID);

    /** update the application */
    SHR_IF_ERR_EXIT(bcm_dnx_flow_initiator_info_create(unit, &flow_handle_info, &common_fields, &special_fields));

exit:
    SHR_FUNC_EXIT;
}

static void
dnx_vxlan_port_common_fields_get(
    int unit,
    bcm_flow_terminator_info_t * common_fields,
    bcm_vxlan_port_t * vxlan_port)
{
    /** get the default_vpn */
    vxlan_port->default_vpn = common_fields->vsi;

    /** get the network_group_id */
    vxlan_port->network_group_id = common_fields->l2_ingress_info.ingress_network_group_id;

    /** get the learning params */
    /** egress_tunnel_id */
    vxlan_port->egress_tunnel_id = common_fields->l2_learn_info.encap_id;

    /** match_port */
    vxlan_port->match_port = BCM_GPORT_SYSTEM_PORT_ID_GET(common_fields->l2_learn_info.dest_port);
}

shr_error_e
dnx_vlxan_vxlan_port_id_set(
    int unit,
    bcm_vxlan_port_t * vxlan_port)
{
    int global_in_lif;
    dnx_algo_gpm_gport_hw_resources_t tunnel_term_gport_hw_resources;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_vxlan_port_tunnel_term_gport_hw_resources_get
                    (unit, vxlan_port, &tunnel_term_gport_hw_resources));
    global_in_lif = tunnel_term_gport_hw_resources.global_in_lif;
    BCM_GPORT_VXLAN_PORT_ID_SET(vxlan_port->vxlan_port_id, global_in_lif);
exit:
    SHR_FUNC_EXIT;
}

/********************
 * SHARED FUNCTIONS *
 ********************/
shr_error_e
dnx_vxlan_port_create_flow(
    int unit,
    bcm_vxlan_port_t * vxlan_port)
{
    uint8 is_outlif_exists = _BCM_VXLAN_PORT_HAS_OUTLIF(vxlan_port);

    SHR_FUNC_INIT_VARS(unit);

    /** update the ipvx_tunnel_terminator */
    SHR_IF_ERR_EXIT(dnx_vxlan_port_terminator_create_flow(unit, vxlan_port));

    if (is_outlif_exists == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_vxlan_port_initiator_create_flow(unit, vxlan_port));
    }
    SHR_IF_ERR_EXIT(dnx_vlxan_vxlan_port_id_set(unit, vxlan_port));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_vxlan_port_get_flow(
    int unit,
    bcm_vxlan_port_t * vxlan_port)
{
    bcm_flow_handle_info_t flow_handle_info = { 0 };
    bcm_flow_terminator_info_t common_fields = { 0 };
    bcm_flow_special_fields_t special_fields = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    /** get the currently data */
       /** get the flow_handle */
    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_IPVX_TERMINATOR, &(flow_handle_info.flow_handle)));

    /** get the flow_id */
    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert(unit, vxlan_port->vxlan_port_id, BCM_GPORT_TYPE_FLOW_LIF,
                                                DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR,
                                                &flow_handle_info.flow_id));

    /** get the match_tunnel_id */
    BCM_GPORT_TUNNEL_ID_SET(vxlan_port->match_tunnel_id, vxlan_port->vxlan_port_id);

    /** get the previously set data */
    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_get(unit, &flow_handle_info, &common_fields, &special_fields));

    /** update the common fields data */
    dnx_vxlan_port_common_fields_get(unit, &common_fields, vxlan_port);

    /** flags */
    if (BCM_L3_ITF_TYPE_IS_FEC(vxlan_port->egress_if))
    {
        vxlan_port->flags |= BCM_VXLAN_PORT_EGRESS_TUNNEL;
    }

    /** const values */
    vxlan_port->criteria = BCM_VXLAN_PORT_MATCH_VN_ID;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_vxlan_port_delete_flow(
    int unit,
    bcm_gport_t vxlan_port_id)
{
    bcm_flow_handle_info_t flow_handle_info = { 0 };
    bcm_flow_terminator_info_t common_fields = { 0 };
    bcm_flow_special_fields_t special_fields = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    /** get the currently data */
    /** get the flow_handle */
    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_IPVX_TERMINATOR, &(flow_handle_info.flow_handle)));

    /** get the flow_id */
    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert(unit, vxlan_port_id, BCM_GPORT_TYPE_FLOW_LIF,
                                                DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR,
                                                &flow_handle_info.flow_id));

    common_fields.valid_elements_clear = (BCM_FLOW_TERMINATOR_ELEMENT_LEARN_ENABLE_VALID |
                                          BCM_FLOW_TERMINATOR_ELEMENT_L2_LEARN_INFO_VALID |
                                          BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID |
                                          BCM_FLOW_TERMINATOR_ELEMENT_VSI_VALID);

    /** flow_handle_info */
    flow_handle_info.flags = (BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID);

    /** update the application */
    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_create(unit, &flow_handle_info, &common_fields, &special_fields));

exit:
    SHR_FUNC_EXIT;
}
