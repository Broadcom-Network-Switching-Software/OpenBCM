/** \file vswitch_flow.c
 *  General vswitch functionality using flow lif for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_VLAN

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm/flow.h>
#include <bcm_int/dnx/flow/flow.h>
#include <bcm_int/dnx/vsi/vsi.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/auto_generated/dnx_vswitch_dispatch.h>
#include <bcm_int/dnx/auto_generated/dnx_flow_dispatch.h>

#include "vswitch_flow.h"
#include "vlan_port_flow.h"

typedef struct
{
    void *data;
    bcm_vswitch_cross_connect_traverse_cb user_cb;
} dnx_vswitch_flow_user_data_t;

static shr_error_e
dnx_vswitch_cross_connect_flow_handle_get(
    int unit,
    bcm_gport_t src_port,
    uint32 *gport_type,
    bcm_flow_handle_t * flow_handle)
{

    bcm_gport_t flow_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, src_port, BCM_GPORT_TYPE_FLOW_LIF, DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR, &flow_id));

    /** get the flow_handle */
    SHR_IF_ERR_REPLACE_AND_EXIT(flow_lif_flow_app_from_gport_get
                                (unit, flow_id, FLOW_APP_TYPE_TERM, flow_handle), _SHR_E_PARAM, _SHR_E_NOT_FOUND);

    /** get the gport_type */
    if (BCM_GPORT_IS_TUNNEL(src_port))
    {
        *gport_type = _SHR_GPORT_TYPE_TUNNEL;
    }
    else if (BCM_GPORT_IS_MPLS_PORT(src_port))
    {
        *gport_type = _SHR_GPORT_TYPE_MPLS_PORT;
    }
    else if (BCM_GPORT_IS_VLAN_PORT(src_port))
    {
        *gport_type = _SHR_GPORT_TYPE_VLAN_PORT;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "gport (0x%08X) is not a valid gport as cross connect source port.\n", src_port);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h for details.
 */
shr_error_e
dnx_vswitch_is_p2p_flow(
    int unit,
    bcm_gport_t gport,
    int *is_p2p)
{
    bcm_gport_t src_port = gport;
    uint32 gport_type;
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_terminator_info_t vswitch_flow_term_old;
    bcm_flow_special_fields_t special_fields_old;

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_terminator_info_t_init(&vswitch_flow_term_old);
    bcm_flow_special_fields_t_init(&special_fields_old);

    SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_flow_handle_get
                    (unit, src_port, &gport_type, &flow_handle_info.flow_handle));

    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, src_port, BCM_GPORT_TYPE_FLOW_LIF, DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR,
                     &flow_handle_info.flow_id));

    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_get
                    (unit, &flow_handle_info, &vswitch_flow_term_old, &special_fields_old));

    if (((vswitch_flow_term_old.valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID) &&
         (vswitch_flow_term_old.l2_ingress_info.service_type == bcmFlowServiceTypeCrossConnect)) ||
        ((vswitch_flow_term_old.valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID) &&
         (vswitch_flow_term_old.l3_ingress_info.service_type == bcmFlowServiceTypeCrossConnect)))
    {
        *is_p2p = TRUE;
    }
    else
    {
        *is_p2p = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vswitch_cross_connect_direction_add_flow(
    int unit,
    bcm_gport_t src_port,
    int dst_port,
    int dst_encap)
{
    int is_p2p = FALSE;
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_terminator_info_t vswitch_flow_term;
    bcm_flow_terminator_info_t vswitch_flow_term_old;
    bcm_flow_special_fields_t special_fields;
    bcm_flow_special_fields_t special_fields_old;
    bcm_flow_field_id_t field_id;
    uint32 gport_type;
    dnx_algo_gpm_forward_info_t forward_info;
    int forward_info_valid;
    int rv = 0;
    const char *flow_app_name_str;
    int src_is_native_ac;

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_terminator_info_t_init(&vswitch_flow_term);
    bcm_flow_terminator_info_t_init(&vswitch_flow_term_old);
    bcm_flow_special_fields_t_init(&special_fields);
    bcm_flow_special_fields_t_init(&special_fields_old);
    forward_info_valid = FALSE;

    SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_flow_handle_get
                    (unit, src_port, &gport_type, &flow_handle_info.flow_handle));

    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, src_port, BCM_GPORT_TYPE_FLOW_LIF, DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR,
                     &flow_handle_info.flow_id));

    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_get
                    (unit, &flow_handle_info, &vswitch_flow_term_old, &special_fields_old));

    flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID;

    if (vswitch_flow_term_old.valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID)
    {
        is_p2p = (vswitch_flow_term_old.l2_ingress_info.service_type == bcmFlowServiceTypeCrossConnect) ? TRUE : FALSE;
    }

    if (vswitch_flow_term_old.valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID)
    {
        is_p2p = (vswitch_flow_term_old.l3_ingress_info.service_type == bcmFlowServiceTypeCrossConnect) ? TRUE : FALSE;
    }

    /** invalid destination means clear the element from the LIF. Use in case of delete */
    if (dst_port == BCM_PORT_INVALID)
    {
        if (!is_p2p)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "flow_id=%d is not P2P \n", flow_handle_info.flow_id);
        }
        vswitch_flow_term.valid_elements_clear |= BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID;
    }
    else
    {
        vswitch_flow_term.dest_info.dest_port = dst_port;
        vswitch_flow_term.dest_info.dest_encap = dst_encap;
        vswitch_flow_term.valid_elements_set = BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID;

        if (!is_p2p)
        {
            flow_app_name_str = dnx_flow_handle_to_string(unit, flow_handle_info.flow_handle);
            src_is_native_ac =
                !sal_strncasecmp(FLOW_APP_NAME_VLAN_PORT_AC_NATIVE_TERMINATOR, flow_app_name_str, FLOW_STR_MAX);
            if (gport_type == _SHR_GPORT_TYPE_VLAN_PORT)
            {
                /*
                 * Note:
                 * Need to retain forwarding information because it is deleted on moving from MP to P2P!!!
                 */
                rv = algo_gpm_gport_l2_forward_info_get(unit, flow_handle_info.flow_id,
                                                        ALGO_GPM_GPORT_L2_FWD_INFO_FLAGS_NO_ERR_MSG_NOT_FOUND,
                                                        &forward_info);
                if (rv == _SHR_E_NONE)
                {
                    forward_info_valid = TRUE;
                }

                special_fields.actual_nof_special_fields = 1;
                SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get
                                (unit, flow_handle_info.flow_handle, "VSI_ASSIGNMENT_MODE", &field_id));
                special_fields.special_fields[0].field_id = field_id;
                special_fields.special_fields[0].is_clear = 1;
                vswitch_flow_term.valid_elements_clear |= BCM_FLOW_TERMINATOR_ELEMENT_VSI_VALID;
            }
            /*
             * Native AC has not such field
             */
            if (!src_is_native_ac)
            {
                vswitch_flow_term.valid_elements_clear |=
                    BCM_FLOW_TERMINATOR_ELEMENT_L2_LEARN_INFO_VALID | BCM_FLOW_TERMINATOR_ELEMENT_LEARN_ENABLE_VALID;
            }
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
    bcm_flow_terminator_info_t vswitch_flow_term;
    bcm_flow_special_fields_t special_fields;
    uint32 gport_type;
    uint8 is_p2p = 0;
    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_terminator_info_t_init(&vswitch_flow_term);
    bcm_flow_special_fields_t_init(&special_fields);

    SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_flow_handle_get
                    (unit, gports->port1, &gport_type, &flow_handle_info.flow_handle));

    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, gports->port1, BCM_GPORT_TYPE_FLOW_LIF, DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR,
                     &flow_handle_info.flow_id));

    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_get(unit, &flow_handle_info, &vswitch_flow_term, &special_fields));

    is_p2p = (vswitch_flow_term.valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID) &&
        (vswitch_flow_term.l2_ingress_info.service_type == bcmFlowServiceTypeCrossConnect);
    is_p2p |= (vswitch_flow_term.valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID) &&
        (vswitch_flow_term.l3_ingress_info.service_type == bcmFlowServiceTypeCrossConnect);

    if (!is_p2p)
    {
        /*
         * The LIF is not P2P, return error
         */
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }

    if (vswitch_flow_term.valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID)
    {
        if (BCM_GPORT_IS_FLOW_LIF(vswitch_flow_term.dest_info.dest_port))
        {
            uint8 is_physical_port;

            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical
                            (unit, vswitch_flow_term.dest_info.dest_port, &is_physical_port));
            if (is_physical_port)
            {
                gports->port2 = vswitch_flow_term.dest_info.dest_port;
            }
            else
            {
                
                int global_lif = _SHR_GPORT_FLOW_LIF_VAL_GET_ID(vswitch_flow_term.dest_info.dest_port);
                gports->port2 = BCM_GPORT_INVALID;
                SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                                (unit,
                                 (DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS |
                                  DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT), _SHR_CORE_ALL,
                                 global_lif, &(gports->port2)));
                if (gports->port2 == BCM_GPORT_INVALID)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                                    (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS, _SHR_CORE_ALL,
                                     global_lif, &(gports->port2)));
                }
            }
        }
        else
        {
            gports->port2 = vswitch_flow_term.dest_info.dest_port;
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
            (vswitch_flow_term.l2_ingress_info.service_type != bcmFlowServiceTypeCrossConnect))
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
    bcm_vlan_port_t vlan_port;
    uint32 out_lif_is_zero = 0;

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_terminator_info_t_init(&vswitch_flow_term);
    bcm_flow_special_fields_t_init(&special_fields);

    if (BCM_GPORT_IS_VLAN_PORT(gport))
    {
        SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR, &flow_handle));
        flow_handle_info.flow_handle = flow_handle;

        SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                        (unit, gport, BCM_GPORT_TYPE_FLOW_LIF, DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR,
                         &flow_handle_info.flow_id));

        /*
         * Retrieve the VLAN-Port parameters that are required in order recalculate the learn info information.
         */
        SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_get(unit, &flow_handle_info, &vswitch_flow_term, &special_fields));

        if (!((vswitch_flow_term.valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID) &&
              (vswitch_flow_term.l2_ingress_info.service_type == bcmFlowServiceTypeCrossConnect)) &&
            !((vswitch_flow_term.valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID) &&
              (vswitch_flow_term.l3_ingress_info.service_type == bcmFlowServiceTypeCrossConnect)))
        {
            /*
             * The LIF is not P2P, return error
             */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Wrong setting. port = 0x%08X is not a P2P LIF.\n", gport);
        }

        flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID;

        /*
         * Setting all common fields, consistent with legacy API
         */
        vswitch_flow_term.l2_ingress_info.service_type = bcmFlowServiceTypeMultiPoint;

        vswitch_flow_term.valid_elements_clear = BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID;
        vswitch_flow_term.valid_elements_set &= (~BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID);

        /*
         * Note: in JR2, P2P --> MP transition:
         *    - learning is set to FALSE.
         *    - learning parameters are set (LEARN_INFO, LEARN_PAYLOAD_CONTEXT).
         */
        vswitch_flow_term.valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_LEARN_ENABLE_VALID;
        vswitch_flow_term.learn_enable = FALSE;

        vswitch_flow_term.valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_L2_LEARN_INFO_VALID;
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.vlan_port_id = gport;
        SHR_IF_ERR_EXIT(dnx_vlan_port_flow_get(unit, &vlan_port));

        SHR_IF_ERR_EXIT(dnx_vlan_port_gport_to_forward_information_get(unit, &vlan_port, &out_lif_is_zero));

        SHR_IF_ERR_EXIT(dnx_vlan_port_flow_terminator_l2_learn_info_set
                        (unit, &flow_handle_info, &vswitch_flow_term, &vlan_port));

        vswitch_flow_term.valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_VSI_VALID;
        vswitch_flow_term.vsi = DNX_VSI_DEFAULT;

        SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, "VSI_ASSIGNMENT_MODE", &field_id));

        special_fields.special_fields[special_fields.actual_nof_special_fields].field_id = field_id;
        special_fields.special_fields[special_fields.actual_nof_special_fields].symbol =
            DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LIF;
        special_fields.actual_nof_special_fields++;

        /*
         * Note:
         * See dnx_vswitch_cross_connect_direction_add_flow - 
         * On moving from MP to P2P, forwarding information is retained, thus need to delete it before performing the P2P to MP transition!!!
         */
        SHR_IF_ERR_EXIT(algo_gpm_gport_l2_forward_info_delete(unit, flow_handle_info.flow_id));

        SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_create
                        (unit, &flow_handle_info, &vswitch_flow_term, &special_fields));
    }
    else
    {
        /** delete clear the element from the LIF */
        SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_direction_add_flow(unit, gport, BCM_PORT_INVALID, 0));
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

/*
 * \brief -
 *  Flow tranverse cb for the vswitch cross connection.
 *  Works for AC, PWE and EVPN.
 */
static shr_error_e
dnx_vswitch_cross_connect_traverse_flow_cb(
    int unit,
    uint32 gport_type,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields,
    void *data)
{
    bcm_vswitch_cross_connect_t gports;
    dnx_vswitch_flow_user_data_t *user_data = (dnx_vswitch_flow_user_data_t *) data;
    uint8 is_p2p = 0;
    SHR_FUNC_INIT_VARS(unit);

    is_p2p = ((terminator_info->valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID) &&
              (terminator_info->l2_ingress_info.service_type == bcmFlowServiceTypeCrossConnect));

    is_p2p |= ((terminator_info->valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID) &&
               (terminator_info->l3_ingress_info.service_type == bcmFlowServiceTypeCrossConnect));

    if (is_p2p &&
        (terminator_info->valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID) &&
        (terminator_info->dest_info.dest_port != 0 || terminator_info->dest_info.dest_encap != 0))
    {
        bcm_vswitch_cross_connect_t_init(&gports);
        gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;

        /*
         * Process for the src gport: gport1
         */
        SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                        (unit, flow_handle_info->flow_id, gport_type,
                         DNX_ALGO_GPM_CONVERSION_FLAG_INGRESS_LIF, &gports.port1));

        /*
         * Process for the dst gport: gport2
         */
        if (BCM_GPORT_IS_FLOW_LIF(terminator_info->dest_info.dest_port))
        {
            uint8 is_physical_port;
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical
                            (unit, terminator_info->dest_info.dest_port, &is_physical_port));
            if (is_physical_port)
            {
                gports.port2 = terminator_info->dest_info.dest_port;
            }
            else
            {
                
                int global_lif = _SHR_GPORT_FLOW_LIF_VAL_GET_ID(terminator_info->dest_info.dest_port);
                gports.port2 = BCM_GPORT_INVALID;
                SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                                (unit,
                                 (DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS |
                                  DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT), _SHR_CORE_ALL,
                                 global_lif, &(gports.port2)));
                if (gports.port2 == BCM_GPORT_INVALID)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                                    (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS, _SHR_CORE_ALL,
                                     global_lif, &(gports.port2)));
                }
            }
        }
        else
        {
            gports.port2 = terminator_info->dest_info.dest_port;
        }

        /** legacy API expect that encap2 will not be updated in case OUT_LF not valid */
        if (terminator_info->dest_info.dest_encap != 0)
        {
            gports.encap2 = terminator_info->dest_info.dest_encap;
        }

        SHR_IF_ERR_EXIT(user_data->user_cb(unit, &gports, user_data->data));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vswitch_cross_connect_traverse_flow_cb_vlan(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields,
    void *data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_traverse_flow_cb
                    (unit, _SHR_GPORT_TYPE_VLAN_PORT, flow_handle_info, terminator_info, special_fields, data));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vswitch_cross_connect_traverse_flow_cb_pwe(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields,
    void *data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_traverse_flow_cb
                    (unit, _SHR_GPORT_TYPE_MPLS_PORT, flow_handle_info, terminator_info, special_fields, data));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Works for PWE and EVPN
 */
static shr_error_e
dnx_vswitch_cross_connect_traverse_flow_cb_type_tunnel(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields,
    void *data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_traverse_flow_cb
                    (unit, _SHR_GPORT_TYPE_TUNNEL, flow_handle_info, terminator_info, special_fields, data));

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
                    (unit, &flow_handle_info, dnx_vswitch_cross_connect_traverse_flow_cb_vlan, &user_data));

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_MPLS_PORT_TERMINATOR, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;
    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_traverse
                    (unit, &flow_handle_info, dnx_vswitch_cross_connect_traverse_flow_cb_pwe, &user_data));

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_MPLS_EVPN_TERMINATOR, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;
    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_traverse
                    (unit, &flow_handle_info, dnx_vswitch_cross_connect_traverse_flow_cb_type_tunnel, &user_data));

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_IPVX_TERMINATOR, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;
    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_traverse
                    (unit, &flow_handle_info, dnx_vswitch_cross_connect_traverse_flow_cb_type_tunnel, &user_data));

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief -
 *  Delete the vswitch cross connection.
 *  Works for AC, PWE and EVPN.
 */
static shr_error_e
dnx_vswitch_cross_connect_delete_cb(
    int unit,
    uint32 gport_type,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields,
    void *data)
{
    bcm_vswitch_cross_connect_t gports;
    uint8 is_p2p = 0;
    SHR_FUNC_INIT_VARS(unit);

    is_p2p = ((terminator_info->valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID) &&
              (terminator_info->l2_ingress_info.service_type == bcmFlowServiceTypeCrossConnect));

    is_p2p |= ((terminator_info->valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID) &&
               (terminator_info->l3_ingress_info.service_type == bcmFlowServiceTypeCrossConnect));

    if (is_p2p &&
        (terminator_info->valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID) &&
        (terminator_info->dest_info.dest_port != 0 || terminator_info->dest_info.dest_encap != 0))
    {
        bcm_vswitch_cross_connect_t_init(&gports);
        gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;

        if (gport_type == _SHR_GPORT_TYPE_MPLS_PORT)
        {
            SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                            (unit, flow_handle_info->flow_id, _SHR_GPORT_TYPE_MPLS_PORT,
                             DNX_ALGO_GPM_CONVERSION_FLAG_NONE, &gports.port1));
        }
        else if (gport_type == _SHR_GPORT_TYPE_VLAN_PORT)
        {
            SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                            (unit, flow_handle_info->flow_id, _SHR_GPORT_TYPE_VLAN_PORT,
                             DNX_ALGO_GPM_CONVERSION_FLAG_INGRESS_LIF, &gports.port1));
        }
        else
        {
            BCM_GPORT_TUNNEL_ID_SET(gports.port1, _SHR_GPORT_FLOW_LIF_VAL_GET_ID(flow_handle_info->flow_id));
        }

        SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_delete_flow(unit, &gports));
    }

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
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_delete_cb
                    (unit, _SHR_GPORT_TYPE_VLAN_PORT, flow_handle_info, terminator_info, special_fields, data));

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

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_delete_cb
                    (unit, _SHR_GPORT_TYPE_MPLS_PORT, flow_handle_info, terminator_info, special_fields, data));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vswitch_cross_connect_delete_cb_type_tunnel(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields,
    void *data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_delete_cb
                    (unit, _SHR_GPORT_TYPE_TUNNEL, flow_handle_info, terminator_info, special_fields, data));

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

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_MPLS_EVPN_TERMINATOR, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;
    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_traverse
                    (unit, &flow_handle_info, dnx_vswitch_cross_connect_delete_cb_type_tunnel, &data));

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_IPVX_TERMINATOR, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;
    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_traverse
                    (unit, &flow_handle_info, dnx_vswitch_cross_connect_delete_cb_type_tunnel, &data));

exit:
    SHR_FUNC_EXIT;
}
