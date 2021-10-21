/** \file vswitch_flow.c
 *  General vswitch functionality using flow lif for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_VLAN

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm/flow.h>
#include <bcm_int/dnx/flow/flow.h>
#include <bcm_int/dnx/vsi/vsi.h>
#include <bcm_int/dnx/vlan/vlan.h>

#include "vswitch_flow.h"
#include "vlan_port_flow.h"

typedef struct
{
    void *data;
    bcm_vswitch_cross_connect_traverse_cb user_cb;
} dnx_vswitch_flow_user_data_t;

static shr_error_e
dnx_vswitch_cross_connect_direction_add_flow(
    int unit,
    bcm_gport_t src_port,
    int dst_port,
    int dst_encap)
{
    int is_p2p = FALSE;
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_terminator_info_t vswitch_flow_term;
    bcm_flow_terminator_info_t vswitch_flow_term_old;
    bcm_flow_special_fields_t special_fields;
    bcm_flow_special_fields_t special_fields_old;
    bcm_flow_field_id_t field_id;
    uint8 is_physical_port;
    int vlan_port_app;
    dnx_algo_gpm_forward_info_t forward_info;
    int forward_info_valid;

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_terminator_info_t_init(&vswitch_flow_term);
    bcm_flow_terminator_info_t_init(&vswitch_flow_term_old);
    bcm_flow_special_fields_t_init(&special_fields);
    bcm_flow_special_fields_t_init(&special_fields_old);
    forward_info_valid = FALSE;

    if (BCM_GPORT_IS_MPLS_PORT(src_port))
    {
        SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_MPLS_PORT_TERMINATOR, &flow_handle));
        vlan_port_app = FALSE;
    }
    else
    {
        SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR, &flow_handle));
        vlan_port_app = TRUE;
    }

    flow_handle_info.flow_handle = flow_handle;

    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert(unit, src_port, _SHR_GPORT_TYPE_FLOW_LIF, &flow_handle_info.flow_id));

    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_get
                    (unit, &flow_handle_info, &vswitch_flow_term_old, &special_fields_old));

    flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID;

    /** invalid destination means clear the element from the LIF. USe in case of delete */
    if (dst_port == BCM_PORT_INVALID)
    {
        vswitch_flow_term.valid_elements_clear = BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID;
    }
    else
    {
        vswitch_flow_term.valid_elements_set = BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID;

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, dst_port, &is_physical_port));
        if (is_physical_port)
        {
            vswitch_flow_term.dest_info.dest_port = dst_port;
        }
        else
        {
            SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                            (unit, dst_port, _SHR_GPORT_TYPE_FLOW_LIF, &vswitch_flow_term.dest_info.dest_port));
        }
        vswitch_flow_term.dest_info.dest_encap = dst_encap;

        is_p2p = FALSE;

        if (vswitch_flow_term_old.valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID)
        {
            is_p2p =
                (vswitch_flow_term_old.l2_ingress_info.service_type == bcmFlowServiceTypeCrossConnect) ? TRUE : FALSE;
        }

        if (!is_p2p)
        {
            if (vlan_port_app)
            {
                /*
                 * Note:
                 * Need to retain forwarding information because it is deleted on moving from MP to P2P!!!
                 */
                forward_info_valid = TRUE;
                SHR_IF_ERR_EXIT(algo_gpm_gport_l2_forward_info_get(unit, flow_handle_info.flow_id, &forward_info));

                special_fields.actual_nof_special_fields = 1;
                SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, "VSI_ASSIGNMENT_MODE", &field_id));
                special_fields.special_fields[0].field_id = field_id;
                special_fields.special_fields[0].is_clear = 1;
                vswitch_flow_term.valid_elements_clear |= BCM_FLOW_TERMINATOR_ELEMENT_VSI_VALID;
            }
            vswitch_flow_term.valid_elements_clear |=
                BCM_FLOW_TERMINATOR_ELEMENT_L2_LEARN_INFO_VALID | BCM_FLOW_TERMINATOR_ELEMENT_LEARN_ENABLE_VALID;

            vswitch_flow_term.valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID;
            vswitch_flow_term.l2_ingress_info.service_type = bcmFlowServiceTypeCrossConnect;
            vswitch_flow_term.l2_ingress_info.ingress_network_group_id = DEFAULT_IN_LIF_ORIENTATION;
        }
    }

    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_create(unit, &flow_handle_info, &vswitch_flow_term, &special_fields));

    if (forward_info_valid == TRUE)
    {
        SHR_IF_ERR_EXIT(algo_gpm_gport_l2_forward_info_add
                        (unit, FALSE, FALSE, flow_handle_info.flow_id, &forward_info));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in .h file
 */
shr_error_e
dnx_vswitch_cross_connect_add_flow(
    int unit,
    bcm_vswitch_cross_connect_t * gports)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_direction_add_flow(unit, gports->port1, gports->port2, gports->encap2));

    if ((_SHR_IS_FLAG_SET(gports->flags, BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL) == FALSE))
    {
        SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_direction_add_flow
                        (unit, gports->port2, gports->port1, gports->encap1));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in .h file
 */
shr_error_e
dnx_vswitch_cross_connect_get_flow(
    int unit,
    bcm_vswitch_cross_connect_t * gports)
{
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_terminator_info_t vswitch_flow_term;
    bcm_flow_special_fields_t special_fields;
    uint32 gport_type;
    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_terminator_info_t_init(&vswitch_flow_term);
    bcm_flow_special_fields_t_init(&special_fields);

    if (BCM_GPORT_IS_MPLS_PORT(gports->port1))
    {
        SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_MPLS_PORT_TERMINATOR, &flow_handle));
        gport_type = _SHR_GPORT_TYPE_MPLS_PORT;
    }
    else
    {
        SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR, &flow_handle));
        gport_type = _SHR_GPORT_TYPE_VLAN_PORT;
    }

    flow_handle_info.flow_handle = flow_handle;

    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, gports->port1, _SHR_GPORT_TYPE_FLOW_LIF, &flow_handle_info.flow_id));

    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_get(unit, &flow_handle_info, &vswitch_flow_term, &special_fields));

    if (vswitch_flow_term.valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID)
    {
        uint8 is_physical_port;

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, vswitch_flow_term.dest_info.dest_port, &is_physical_port));
        if (is_physical_port)
        {
            gports->port2 = vswitch_flow_term.dest_info.dest_port;
        }
        else
        {
            
            if (gport_type == _SHR_GPORT_TYPE_MPLS_PORT)
            {
                BCM_GPORT_SUB_TYPE_LIF_SET(gports->port2, _SHR_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY,
                                           _SHR_GPORT_FLOW_LIF_VAL_GET(vswitch_flow_term.dest_info.dest_port));
                BCM_GPORT_MPLS_PORT_ID_SET(gports->port2, gports->port2);
            }
            else
            {
                SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                                (unit, vswitch_flow_term.dest_info.dest_port, gport_type, &gports->port2));
            }
        }
        /** legacy API expect that encap2 will not be updated in case out lif not valid */
        if (vswitch_flow_term.dest_info.dest_encap != 0)
        {
            gports->encap2 = vswitch_flow_term.dest_info.dest_encap;
        }
    }
    else
    {
        if ((vswitch_flow_term.valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID) &&
            (vswitch_flow_term.service_type != bcmFlowServiceTypeCrossConnect))
        {
            SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
        }
        else
        {
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vswitch_cross_connect_direction_clear_flow(
    int unit,
    bcm_gport_t gport)
{
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_terminator_info_t vswitch_flow_term;
    bcm_flow_special_fields_t special_fields;
    bcm_flow_field_id_t field_id;

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_terminator_info_t_init(&vswitch_flow_term);
    bcm_flow_special_fields_t_init(&special_fields);

    if (BCM_GPORT_IS_MPLS_PORT(gport))
    {
        SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_MPLS_PORT_TERMINATOR, &flow_handle));
        /** delete clear the element from the LIF */
        SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_direction_add_flow(unit, gport, BCM_PORT_INVALID, 0));
    }
    else
    {
        SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR, &flow_handle));
        flow_handle_info.flow_handle = flow_handle;

        SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert(unit, gport, _SHR_GPORT_TYPE_FLOW_LIF, &flow_handle_info.flow_id));

        /*
         * Retrieve the VLAN-Port parameters that are required in order recalculate the learn info information.
         */
        SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_get(unit, &flow_handle_info, &vswitch_flow_term, &special_fields));

        flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID;

        /*
         * Setting all common fields, consistent with legacy API
         */
        vswitch_flow_term.valid_elements_clear = BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID;

        
        vswitch_flow_term.valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_LEARN_ENABLE_VALID;
        vswitch_flow_term.learn_enable = FALSE;

        vswitch_flow_term.valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_VSI_VALID;
        vswitch_flow_term.vsi = DNX_VSI_DEFAULT;

        special_fields.actual_nof_special_fields = 1;

        SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, "VSI_ASSIGNMENT_MODE", &field_id));
        special_fields.special_fields[0].field_id = field_id;
        special_fields.special_fields[0].shr_var_uint32 =
            DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LIF;

        SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_create
                        (unit, &flow_handle_info, &vswitch_flow_term, &special_fields));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in .h file
 */
shr_error_e
dnx_vswitch_cross_connect_delete_flow(
    int unit,
    bcm_vswitch_cross_connect_t * gports)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((_SHR_IS_FLAG_SET(gports->flags, BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL) == FALSE))
    {
        SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_direction_clear_flow(unit, gports->port2));
    }

    SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_direction_clear_flow(unit, gports->port1));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vswitch_cross_connect_cb_vlan(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields,
    void *data)
{
    bcm_vswitch_cross_connect_t gports;
    dnx_vswitch_flow_user_data_t *user_data = (dnx_vswitch_flow_user_data_t *) data;
    uint8 is_physical_port;
    SHR_FUNC_INIT_VARS(unit);

    if ((terminator_info->valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID) &&
        (terminator_info->l2_ingress_info.service_type == bcmFlowServiceTypeCrossConnect))
    {

        
        /*
         * if(flow_handle_info->flags & BCM_FLOW_HANDLE_INFO_SYMMETRIC_ALLOC) {
         */
        BCM_GPORT_SUB_TYPE_LIF_SET(gports.port1, 0, flow_handle_info->flow_id);
        /*
         * } else { BCM_GPORT_SUB_TYPE_LIF_SET(gports.port1, BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY,
         * flow_handle_info->flow_id); }
         */

        BCM_GPORT_VLAN_PORT_ID_SET(gports.port1, gports.port1);

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, terminator_info->dest_info.dest_port, &is_physical_port));
        if (is_physical_port)
        {
            gports.port2 = terminator_info->dest_info.dest_port;
        }
        else
        {
            SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                            (unit, terminator_info->dest_info.dest_port, _SHR_GPORT_TYPE_VLAN_PORT, &gports.port2));
        }
        gports.encap2 = terminator_info->dest_info.dest_encap;

        /*
         * Run callback function
         */
        SHR_IF_ERR_EXIT(user_data->user_cb(unit, &gports, user_data->data));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vswitch_cross_connect_cb_pwe(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields,
    void *data)
{
    bcm_vswitch_cross_connect_t gports;
    dnx_vswitch_flow_user_data_t *user_data = (dnx_vswitch_flow_user_data_t *) data;
    uint8 is_physical_port;
    SHR_FUNC_INIT_VARS(unit);

    bcm_vswitch_cross_connect_t_init(&gports);

    if ((terminator_info->valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID) &&
        (terminator_info->l2_ingress_info.service_type == bcmFlowServiceTypeCrossConnect) &&
        (terminator_info->valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID) &&
        (terminator_info->dest_info.dest_port != 0 || terminator_info->dest_info.dest_encap != 0))
    {

        
        BCM_GPORT_SUB_TYPE_LIF_SET(gports.port1, _SHR_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY,
                                   _SHR_GPORT_FLOW_LIF_VAL_GET(flow_handle_info->flow_id));
        BCM_GPORT_MPLS_PORT_ID_SET(gports.port1, gports.port1);
/*        SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
 *                      (unit, flow_handle_info->flow_id, _SHR_GPORT_TYPE_MPLS_PORT, &gports.port1));
 */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, terminator_info->dest_info.dest_port, &is_physical_port));
        if (is_physical_port)
        {
            gports.port2 = terminator_info->dest_info.dest_port;
        }
        else
        {
            
            BCM_GPORT_SUB_TYPE_LIF_SET(gports.port2, _SHR_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY,
                                       _SHR_GPORT_FLOW_LIF_VAL_GET(terminator_info->dest_info.dest_port));
            BCM_GPORT_MPLS_PORT_ID_SET(gports.port2, gports.port2);
/*            SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
 *                         (unit, terminator_info->dest_info.dest_port, _SHR_GPORT_TYPE_MPLS_PORT, &gports.port2));
*/
        }
        /** legacy API expect that encap2 will not be updated in case OUT_LF not valid */
        if (terminator_info->dest_info.dest_encap != 0)
        {
            gports.encap2 = terminator_info->dest_info.dest_encap;
        }
        /*
         * Run callback function
         */
        SHR_IF_ERR_EXIT(user_data->user_cb(unit, &gports, user_data->data));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in .h file
 */
shr_error_e
dnx_vswitch_cross_connect_traverse_flow(
    int unit,
    bcm_vswitch_cross_connect_traverse_cb cb,
    void *data)
{
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_handle_info_t flow_handle_info;
    dnx_vswitch_flow_user_data_t user_data = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_handle_info_t_init(&flow_handle_info);

    user_data.user_cb = cb;
    user_data.data = data;

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;

    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_traverse
                    (unit, &flow_handle_info, dnx_vswitch_cross_connect_cb_vlan, &user_data));

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_MPLS_PORT_TERMINATOR, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;

    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_traverse
                    (unit, &flow_handle_info, dnx_vswitch_cross_connect_cb_pwe, &user_data));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vswitch_cross_connect_delete_cb_vlan(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields,
    void *data)
{
    bcm_vswitch_cross_connect_t gports;
    SHR_FUNC_INIT_VARS(unit);

    if ((terminator_info->valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID) &&
        (terminator_info->l2_ingress_info.service_type == bcmFlowServiceTypeCrossConnect))
    {
        bcm_vswitch_cross_connect_t_init(&gports);
        gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;

        
        /*
         * if(flow_handle_info->flags & BCM_FLOW_HANDLE_INFO_SYMMETRIC_ALLOC) {
         */
        BCM_GPORT_SUB_TYPE_LIF_SET(gports.port1, 0, flow_handle_info->flow_id);
        /*
         * } else { BCM_GPORT_SUB_TYPE_LIF_SET(gports.port1, BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY,
         * flow_handle_info->flow_id); }
         */
        BCM_GPORT_VLAN_PORT_ID_SET(gports.port1, gports.port1);

        SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_delete_flow(unit, &gports));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vswitch_cross_connect_delete_cb_pwe(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields,
    void *data)
{
    bcm_vswitch_cross_connect_t gports;
    SHR_FUNC_INIT_VARS(unit);

    if ((terminator_info->valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID) &&
        (terminator_info->l2_ingress_info.service_type == bcmFlowServiceTypeCrossConnect) &&
        (terminator_info->valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID) &&
        (terminator_info->dest_info.dest_port != 0 || terminator_info->dest_info.dest_encap != 0))
    {
        bcm_vswitch_cross_connect_t_init(&gports);
        gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
        BCM_GPORT_SUB_TYPE_LIF_SET(gports.port1, _SHR_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY, flow_handle_info->flow_id);
        BCM_GPORT_MPLS_PORT_ID_SET(gports.port1, gports.port1);
        SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_delete_flow(unit, &gports));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in .h file
 */
shr_error_e
dnx_vswitch_cross_connect_traverse_delete_flow(
    int unit)
{
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_handle_info_t flow_handle_info;
    int data = 0;

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_handle_info_t_init(&flow_handle_info);

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;
    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_traverse
                    (unit, &flow_handle_info, dnx_vswitch_cross_connect_delete_cb_vlan, &data));

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_MPLS_PORT_TERMINATOR, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;

    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_traverse
                    (unit, &flow_handle_info, dnx_vswitch_cross_connect_delete_cb_pwe, &data));

exit:
    SHR_FUNC_EXIT;
}
