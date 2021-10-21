/** \file vlan_port_flow.c
 *  General vlan port functionality using flow lif for DNX.
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
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dnx/failover/failover.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include "vlan_port_flow.h"
#include "esem_flow.h"
#include <soc/dnx/dnx_data/auto_generated/dnx_data_failover.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trunk.h>
#include <bcm_int/dnx/qos/qos.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>

shr_error_e static
dnx_vlan_port_flow_terminator_l2_learn_info_set(
    int unit,
    bcm_flow_terminator_info_t * vlan_port_flow_term,
    bcm_vlan_port_t * vlan_port)
{

    uint32 destination;
    dbal_fields_e dbal_dest_type;
    uint32 dest_val;

    SHR_FUNC_INIT_VARS(unit);

    vlan_port_flow_term->l2_learn_info.flush_group = vlan_port->group;

    if ((vlan_port->failover_port_id))
    {
        vlan_port_flow_term->l2_learn_info.dest_port = vlan_port->failover_port_id;
    }
    else
    {
        if (vlan_port->failover_mc_group)
        {
            _SHR_GPORT_MCAST_SET(vlan_port_flow_term->l2_learn_info.dest_port,
                                 _SHR_MULTICAST_ID_GET(vlan_port->failover_mc_group));
        }
        else
        {
            vlan_port_flow_term->l2_learn_info.dest_port = vlan_port->port;
        }
    }
    SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                    (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, vlan_port_flow_term->l2_learn_info.dest_port,
                     &destination));

    SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                    (unit, DBAL_FIELD_DESTINATION, destination, &dbal_dest_type, &dest_val));

    vlan_port_flow_term->l2_learn_info.l2_learn_info_flags = 0;
    /*
     * Allow Optimization learn context only if the Destination is of a port type or LAG
     * and only if the port/LAG remains unchanged.
     */
    if (((dbal_dest_type != DBAL_FIELD_PORT_ID) && (dbal_dest_type != DBAL_FIELD_LAG_ID))
        || (vlan_port->failover_port_id != 0))
    {
        vlan_port_flow_term->l2_learn_info.l2_learn_info_flags |= BCM_FLOW_L2_LEARN_INFO_NOT_OPTIMIZED;
    }
    else
    {
        dnx_algo_gpm_gport_phy_info_t phy_port_info;
        uint32 sys_port_id;

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, vlan_port->port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &phy_port_info));
        sys_port_id = phy_port_info.sys_port;
        if (dbal_dest_type == DBAL_FIELD_LAG_ID)
        {
            uint32 spa_type_shift, spa_mask = 0;

            /*
             * In case of LAG, retrieve the SPA-ID from the encoded SPA,
             * in order to use the ID in the comparison below
             */
            spa_type_shift = dnx_data_trunk.parameters.spa_type_shift_get(unit);
            SHR_BITSET_RANGE(&spa_mask, 0, spa_type_shift);
            sys_port_id = phy_port_info.sys_port & spa_mask;
        }

        if (sys_port_id != dest_val)
        {
            vlan_port_flow_term->l2_learn_info.l2_learn_info_flags |= BCM_FLOW_L2_LEARN_INFO_NOT_OPTIMIZED;
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * Setting common fields for vlan port terminator
 */
static shr_error_e
dnx_vlan_port_flow_terminator_common_fields_set(
    int unit,
    bcm_flow_terminator_info_t * vlan_port_flow_term,
    bcm_vlan_port_t * vlan_port)
{
    SHR_FUNC_INIT_VARS(unit);

    vlan_port_flow_term->flags = 0;
    /*
     * Setting all common fields, consistent with legacy API
     */

    if ((_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_REPLACE)) &&
        (vlan_port_flow_term->valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID) &&
        (vlan_port_flow_term->l2_ingress_info.service_type == bcmFlowServiceTypeCrossConnect))
    {
        vlan_port_flow_term->valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID;
        vlan_port_flow_term->l2_ingress_info.ingress_network_group_id = vlan_port->ingress_network_group_id;

    }
    else
    {

        vlan_port_flow_term->valid_elements_set = (BCM_FLOW_TERMINATOR_ELEMENT_VSI_VALID |
                                                   BCM_FLOW_TERMINATOR_ELEMENT_L2_LEARN_INFO_VALID);

        vlan_port_flow_term->valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID;
        vlan_port_flow_term->l2_ingress_info.ingress_network_group_id = vlan_port->ingress_network_group_id;

        vlan_port_flow_term->vsi = vlan_port->vsi;
        /*
         * The Learn enable is set during standard creation, but during update the previous
         * value remains.
         */
        if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_REPLACE))
        {
            ac_application_type_e ll_app_type = NUM_AC_APPLICATION_TYPE_E;

            vlan_port_flow_term->valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_LEARN_ENABLE_VALID;
            vlan_port_flow_term->learn_enable = FALSE;
            SHR_IF_ERR_EXIT(dnx_vlan_port_set_ll_app_type(unit, vlan_port, &ll_app_type));
            if (!(ll_app_type == SERVICE_AC_APPLICATION_TYPE))
            {
                vlan_port_flow_term->learn_enable = TRUE;
            }
        }

        SHR_IF_ERR_EXIT(dnx_vlan_port_flow_terminator_l2_learn_info_set(unit, vlan_port_flow_term, vlan_port));

        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_STAT_INGRESS_ENABLE))
        {
            vlan_port_flow_term->valid_elements_set |=
                BCM_FLOW_TERMINATOR_ELEMENT_STAT_ID_VALID | BCM_FLOW_TERMINATOR_ELEMENT_STAT_PP_PROFILE_VALID;
            vlan_port_flow_term->stat_id = 0;
            vlan_port_flow_term->stat_pp_profile = 0;
        }

        if (DNX_FAILOVER_IS_PROTECTION_ENABLED(vlan_port->ingress_failover_id))
        {
            vlan_port_flow_term->valid_elements_set |=
                BCM_FLOW_TERMINATOR_ELEMENT_FAILOVER_ID_VALID | BCM_FLOW_TERMINATOR_ELEMENT_FAILOVER_STATE_VALID;
            vlan_port_flow_term->failover_state = (vlan_port->ingress_failover_port_id) ? 0 : 1;
            vlan_port_flow_term->failover_id = vlan_port->ingress_failover_id;
        }

        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_INGRESS_WIDE))
        {
            vlan_port_flow_term->valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_ADDITIONAL_DATA_VALID;
            vlan_port_flow_term->additional_data = 0;
        }
    }

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * Setting special fields for vlan port terminator
 */
static shr_error_e
dnx_vlan_port_flow_terminator_special_fields_set(
    int unit,
    bcm_flow_handle_t flow_handle,
    bcm_flow_terminator_info_t * flow_tunnel_init,
    bcm_flow_special_fields_t * special_fields,
    bcm_vlan_port_t * vlan_port)
{
    bcm_flow_field_id_t field_id = 0;
    int fodo_assigment_mode;
    ac_application_type_e ll_app_type = NUM_AC_APPLICATION_TYPE_E;

    SHR_FUNC_INIT_VARS(unit);

    special_fields->actual_nof_special_fields = 0;

    if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_REPLACE))
    {
        special_fields->actual_nof_special_fields = 2;

        SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, "VSI_ASSIGNMENT_MODE", &field_id));
        SHR_IF_ERR_EXIT(dnx_vlan_port_set_ll_app_type(unit, vlan_port, &ll_app_type));
        SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_fodo_mode_calc
                        (unit, vlan_port, ll_app_type, &fodo_assigment_mode));
        special_fields->special_fields[0].field_id = field_id;
        special_fields->special_fields[0].symbol = fodo_assigment_mode;

        /** real value for VLAN_EDIT_PCP_DEI_PROFILE is being set by bcm_qos_port_map_set */
        SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, "VLAN_EDIT_PCP_DEI_PROFILE", &field_id));
        special_fields->special_fields[1].field_id = field_id;
        special_fields->special_fields[1].shr_var_uint32 = 0;
        DNX_QOS_VLAN_PCP_MAP_SET(special_fields->special_fields[1].shr_var_uint32);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Set/get/delete match fields for vlan port terminator according criteria
 */
shr_error_e
dnx_vlan_port_terminator_flow_match_action(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_vlan_port_t * vlan_port,
    dnx_flow_match_action_e action)
{
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_field_id_t field_id = 0;
    bcm_flow_special_fields_t special_fields = { 0 };
    uint32 vlan_doimain;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Check supported criterias
     */
    switch (vlan_port->criteria)
    {
        case BCM_VLAN_PORT_MATCH_NONE:
            SHR_SET_CURRENT_ERR(_SHR_E_NONE);
            SHR_EXIT();
            break;

        case BCM_VLAN_PORT_MATCH_PORT:
            SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_LL_PP_PORT_TERM_MATCH, &flow_handle));
            special_fields.actual_nof_special_fields = 1;

            SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "PORT", &field_id));
            special_fields.special_fields[0].field_id = field_id;
            special_fields.special_fields[0].shr_var_uint32 = vlan_port->port;
            break;

        case BCM_VLAN_PORT_MATCH_PORT_VLAN:
            SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get
                            (unit, FLOW_APP_NAME_VLAN_PORT_LL_AC_S_VLAN_TERM_MATCH, &flow_handle));
            special_fields.actual_nof_special_fields = 2;

            SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, "S_VID", &field_id));
            special_fields.special_fields[0].field_id = field_id;
            special_fields.special_fields[0].shr_var_uint32 = vlan_port->match_vlan;

            SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, "VLAN_DOMAIN", &field_id));
            SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, vlan_port->port, &vlan_doimain));
            special_fields.special_fields[1].field_id = field_id;
            special_fields.special_fields[1].shr_var_uint32 = vlan_doimain;
            break;

        case BCM_VLAN_PORT_MATCH_PORT_CVLAN:
            SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get
                            (unit, FLOW_APP_NAME_VLAN_PORT_LL_AC_C_VLAN_TERM_MATCH, &flow_handle));
            special_fields.actual_nof_special_fields = 2;

            SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, "C_VID", &field_id));
            special_fields.special_fields[0].field_id = field_id;
            special_fields.special_fields[0].shr_var_uint32 = vlan_port->match_vlan;

            SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, "VLAN_DOMAIN", &field_id));
            SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, vlan_port->port, &vlan_doimain));
            special_fields.special_fields[1].field_id = field_id;
            special_fields.special_fields[1].shr_var_uint32 = vlan_doimain;
            break;

        case BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED:
            SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get
                            (unit, FLOW_APP_NAME_VLAN_PORT_LL_AC_S_C_VLAN_TERM_MATCH, &flow_handle));
            special_fields.actual_nof_special_fields = 3;

            SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, "S_VID", &field_id));
            special_fields.special_fields[0].field_id = field_id;
            special_fields.special_fields[0].shr_var_uint32 = vlan_port->match_vlan;

            SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, "C_VID", &field_id));
            special_fields.special_fields[1].field_id = field_id;
            special_fields.special_fields[1].shr_var_uint32 = vlan_port->match_inner_vlan;

            SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, "VLAN_DOMAIN", &field_id));
            SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, vlan_port->port, &vlan_doimain));
            special_fields.special_fields[2].field_id = field_id;
            special_fields.special_fields[2].shr_var_uint32 = vlan_doimain;
            break;

        case BCM_VLAN_PORT_MATCH_PORT_SVLAN_STACKED:
            SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get
                            (unit, FLOW_APP_NAME_VLAN_PORT_LL_AC_S_S_VLAN_TERM_MATCH, &flow_handle));
            special_fields.actual_nof_special_fields = 3;

            SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, "S_VID_2", &field_id));
            special_fields.special_fields[0].field_id = field_id;
            special_fields.special_fields[0].shr_var_uint32 = vlan_port->match_vlan;

            SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, "S_VID_1", &field_id));
            special_fields.special_fields[1].field_id = field_id;
            special_fields.special_fields[1].shr_var_uint32 = vlan_port->match_inner_vlan;

            SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, "VLAN_DOMAIN", &field_id));
            SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, vlan_port->port, &vlan_doimain));
            special_fields.special_fields[2].field_id = field_id;
            special_fields.special_fields[2].shr_var_uint32 = vlan_doimain;
            break;

        case BCM_VLAN_PORT_MATCH_PORT_CVLAN_STACKED:
            SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get
                            (unit, FLOW_APP_NAME_VLAN_PORT_LL_AC_C_C_VLAN_TERM_MATCH, &flow_handle));
            special_fields.actual_nof_special_fields = 3;

            SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, "C_VID_2", &field_id));
            special_fields.special_fields[0].field_id = field_id;
            special_fields.special_fields[0].shr_var_uint32 = vlan_port->match_vlan;

            SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, "C_VID_1", &field_id));
            special_fields.special_fields[1].field_id = field_id;
            special_fields.special_fields[1].shr_var_uint32 = vlan_port->match_inner_vlan;

            SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, "VLAN_DOMAIN", &field_id));
            SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, vlan_port->port, &vlan_doimain));
            special_fields.special_fields[2].field_id = field_id;
            special_fields.special_fields[2].shr_var_uint32 = vlan_doimain;
            break;

        case BCM_VLAN_PORT_MATCH_PORT_UNTAGGED:

            if (dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc))
            {
                SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get
                                (unit, FLOW_APP_NAME_VLAN_PORT_LL_AC_UNTAGGED_DPC_TERM_MATCH, &flow_handle));
                SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "IN_PORT", &field_id));
            }
            else
            {
                SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get
                                (unit, FLOW_APP_NAME_VLAN_PORT_LL_AC_UNTAGGED_TERM_MATCH, &flow_handle));
                SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "MAPPED_PORT", &field_id));
            }

            special_fields.actual_nof_special_fields = 1;
            special_fields.special_fields[0].field_id = field_id;
            special_fields.special_fields[0].shr_var_uint32 = vlan_port->port;
            break;

        default:
            SHR_SET_CURRENT_ERR(_SHR_E_NONE);
            SHR_EXIT();
            break;
    }

    flow_handle_info->flow_handle = flow_handle;
    if (action == FLOW_MATCH_ACTION_ADD)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_flow_match_info_add(unit, flow_handle_info, &special_fields));
    }
    else
    {
        if (action == FLOW_MATCH_ACTION_GET)
        {
            SHR_IF_ERR_EXIT(bcm_dnx_flow_match_info_get(unit, flow_handle_info, &special_fields));
        }
        else
        {
            SHR_IF_ERR_EXIT(bcm_dnx_flow_match_info_delete(unit, flow_handle_info, &special_fields));
        }
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * Setting common fields vlan port initiator
 */
static shr_error_e
dnx_vlan_port_flow_initiator_common_fields_set(
    int unit,
    bcm_flow_initiator_info_t * vlan_port_flow_init,
    bcm_vlan_port_t * vlan_port)
{
    SHR_FUNC_INIT_VARS(unit);

    vlan_port_flow_init->flags = 0;
    /*
     * Setting all common fields, consistent with legacy API
     */
    vlan_port_flow_init->valid_elements_set = (BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID);
    /*
     * BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID | 
     */

    
    if (dnx_data_lif.out_lif.feature_get(unit, dnx_data_lif_out_lif_out_lif_profile_supported))
    {
        vlan_port_flow_init->valid_elements_set |= BCM_FLOW_INITIATOR_ELEMENT_L2_EGRESS_INFO_VALID;
    }

    vlan_port_flow_init->l2_egress_info.egress_network_group_id = vlan_port->egress_network_group_id;

    if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_REPLACE))
    {
        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_STAT_EGRESS_ENABLE))
        {
            vlan_port_flow_init->valid_elements_set |= BCM_FLOW_INITIATOR_ELEMENT_STAT_ID_VALID |
                BCM_FLOW_INITIATOR_ELEMENT_STAT_PP_PROFILE_VALID;
            vlan_port_flow_init->stat_id = 0;
            vlan_port_flow_init->stat_pp_profile = 0;
        }
        vlan_port_flow_init->valid_elements_set |= BCM_FLOW_INITIATOR_ELEMENT_QOS_MAP_ID_VALID;
        vlan_port_flow_init->qos_map_id = 0;
    }

    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_EGRESS_PROTECTION))
    {
        vlan_port_flow_init->valid_elements_set |= BCM_FLOW_INITIATOR_ELEMENT_FAILOVER_ID_VALID |
            BCM_FLOW_INITIATOR_ELEMENT_FAILOVER_STATE_VALID;
        vlan_port_flow_init->failover_state = (vlan_port->egress_failover_port_id) ? 0 : 1;
        BCM_FAILOVER_ID_GET(vlan_port_flow_init->failover_id, vlan_port->egress_failover_id);
    }
    vlan_port_flow_init->l3_intf_id = BCM_IF_INVALID;
    sal_memcpy(&vlan_port_flow_init->egress_qos_model, &vlan_port->egress_qos_model,
               sizeof(vlan_port->egress_qos_model));

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static void
dnx_vlan_port_flow_initiator_special_fields_set(
    int unit,
    bcm_flow_special_fields_t * special_fields,
    bcm_vlan_port_t * vlan_port)
{
    SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, FLOW_S_F_QOS_PRI, vlan_port->pkt_pri);

    SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, FLOW_S_F_QOS_CFI, vlan_port->pkt_cfi);
}

/**
 * Create vlan port initiator
 */
static shr_error_e
dnx_vlan_port_initiator_flow_create(
    int unit,
    bcm_vlan_port_t * vlan_port,
    bcm_vlan_port_t * old_vlan_port)
{
    bcm_flow_initiator_info_t vlan_port_flow_init;
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_special_fields_t special_fields;

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_initiator_info_t_init(&vlan_port_flow_init);
    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_special_fields_t_init(&special_fields);

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_LL_INITIATOR, &flow_handle));

    flow_handle_info.flow_handle = flow_handle;

    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_REPLACE))
    {
        flow_handle_info.flags = BCM_FLOW_HANDLE_INFO_REPLACE;
    }

    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_WITH_ID))
    {
        flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_WITH_ID;
        BCM_GPORT_TUNNEL_ID_SET(flow_handle_info.flow_id, BCM_GPORT_SUB_TYPE_LIF_VAL_GET(vlan_port->vlan_port_id));
    }

    /*
     * Note:
     * In symmetric case, the flow_id was created by the terminator.
     * Need to take it's value from vlan_port_id and properly set flow flags. 
     */
    if ((!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY)) &&
        (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_REPLACE)))
    {
        flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_SYMMETRIC_ALLOC;
        flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_WITH_ID;
        BCM_GPORT_TUNNEL_ID_SET(flow_handle_info.flow_id, BCM_GPORT_SUB_TYPE_LIF_VAL_GET(vlan_port->vlan_port_id));

    }
    SHR_IF_ERR_EXIT(dnx_vlan_port_flow_initiator_common_fields_set(unit, &vlan_port_flow_init, vlan_port));
    dnx_vlan_port_flow_initiator_special_fields_set(unit, &special_fields, vlan_port);
    SHR_IF_ERR_EXIT(bcm_dnx_flow_initiator_info_create(unit, &flow_handle_info, &vlan_port_flow_init, &special_fields));

    /** Encode vlan_port_id using flow_id */
    if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_WITH_ID))
    {
        /*
         * For symmetric case, vlan_port_id is updated by dnx_vlan_port_terminator_flow_create.
         * Here, only handle egress-only
         */
        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY))
        {
            BCM_GPORT_SUB_TYPE_LIF_SET(vlan_port->vlan_port_id, BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY,
                                       BCM_GPORT_TUNNEL_ID_GET(flow_handle_info.flow_id));
            vlan_port->encap_id = BCM_GPORT_TUNNEL_ID_GET(flow_handle_info.flow_id);
        }

        BCM_GPORT_VLAN_PORT_ID_SET(vlan_port->vlan_port_id, vlan_port->vlan_port_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Create vlan port terminator
 */
static shr_error_e
dnx_vlan_port_terminator_flow_create(
    int unit,
    bcm_vlan_port_t * vlan_port,
    bcm_vlan_port_t * old_vlan_port)
{
    bcm_flow_terminator_info_t vlan_port_flow_term;
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_special_fields_t special_fields;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    int is_replace;

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_terminator_info_t_init(&vlan_port_flow_term);
    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_special_fields_t_init(&special_fields);

    is_replace = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_REPLACE);
    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR, &flow_handle));

    flow_handle_info.flow_handle = flow_handle;

    if (is_replace)
    {
        BCM_GPORT_TUNNEL_ID_SET(flow_handle_info.flow_id, BCM_GPORT_SUB_TYPE_LIF_VAL_GET(vlan_port->vlan_port_id));
        SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_get
                        (unit, &flow_handle_info, &vlan_port_flow_term, &special_fields));
        flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID;
    }

    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_WITH_ID))
    {
        flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_WITH_ID;
        BCM_GPORT_TUNNEL_ID_SET(flow_handle_info.flow_id, BCM_GPORT_SUB_TYPE_LIF_VAL_GET(vlan_port->vlan_port_id));
    }

    if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_INGRESS_ONLY) && !is_replace)
    {
        flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_SYMMETRIC_ALLOC;
    }

    SHR_IF_ERR_EXIT(dnx_vlan_port_flow_terminator_common_fields_set(unit, &vlan_port_flow_term, vlan_port));
    SHR_IF_ERR_EXIT(dnx_vlan_port_flow_terminator_special_fields_set
                    (unit, flow_handle, &vlan_port_flow_term, &special_fields, vlan_port));
    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_create
                    (unit, &flow_handle_info, &vlan_port_flow_term, &special_fields));

    if (!is_replace)
    {
        if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_INGRESS_ONLY))
        {
            BCM_GPORT_SUB_TYPE_LIF_SET(vlan_port->vlan_port_id, 0, BCM_GPORT_TUNNEL_ID_GET(flow_handle_info.flow_id));
            vlan_port->encap_id = BCM_GPORT_TUNNEL_ID_GET(flow_handle_info.flow_id);
        }
        else
        {
            BCM_GPORT_SUB_TYPE_LIF_SET(vlan_port->vlan_port_id, BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY,
                                       BCM_GPORT_TUNNEL_ID_GET(flow_handle_info.flow_id));
            vlan_port->encap_id = 0;
        }

        BCM_GPORT_VLAN_PORT_ID_SET(vlan_port->vlan_port_id, vlan_port->vlan_port_id);
    }

    /** Rest flags are irrelevant for match */
    flow_handle_info.flags &= BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID;
    SHR_IF_ERR_EXIT(dnx_vlan_port_terminator_flow_match_action
                    (unit, &flow_handle_info, vlan_port, FLOW_MATCH_ACTION_ADD));

    /** Get Local inLIF ID */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, flow_handle_info.flow_id,
                     DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources));

    SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_lif_match_info_set(unit,
                                                                    gport_hw_resources.local_in_lif, 0, is_replace,
                                                                    vlan_port, old_vlan_port));

exit:
    SHR_FUNC_EXIT;
}

/**
 * Setting vlan_port fields according terminator
 */
static shr_error_e
dnx_vlan_port_initiator_flow_get(
    int unit,
    bcm_vlan_port_t * vlan_port,
    bcm_flow_handle_info_t * flow_handle_info)
{

    bcm_flow_initiator_info_t vlan_port_flow_init = { 0 };
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_special_fields_t special_fields = { 0 };
    bcm_flow_special_field_t special_field_data;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_LL_INITIATOR, &flow_handle));
    flow_handle_info->flow_handle = flow_handle;

    BCM_GPORT_TUNNEL_ID_SET(flow_handle_info->flow_id, BCM_GPORT_SUB_TYPE_LIF_VAL_GET(vlan_port->vlan_port_id));

    SHR_IF_ERR_EXIT(bcm_dnx_flow_initiator_info_get(unit, flow_handle_info, &vlan_port_flow_init, &special_fields));

    if (vlan_port_flow_init.valid_elements_set & BCM_FLOW_INITIATOR_ELEMENT_STAT_ID_VALID)
    {
        vlan_port->flags |= BCM_VLAN_PORT_STAT_EGRESS_ENABLE;
    }

    if (vlan_port_flow_init.valid_elements_set & BCM_FLOW_INITIATOR_ELEMENT_FAILOVER_ID_VALID)
    {
        BCM_FAILOVER_SET(vlan_port->egress_failover_id, vlan_port_flow_init.failover_id, BCM_FAILOVER_TYPE_ENCAP);
        vlan_port->egress_failover_port_id = (vlan_port_flow_init.failover_state) ? 0 : 1;
        vlan_port->flags |= BCM_VLAN_PORT_EGRESS_PROTECTION;
    }

    vlan_port->egress_network_group_id = vlan_port_flow_init.l2_egress_info.egress_network_group_id;

    sal_memcpy(&vlan_port->egress_qos_model, &vlan_port_flow_init.egress_qos_model,
               sizeof(vlan_port->egress_qos_model));

    /** get pri */
    sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
    SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_special_field_data_get
                              (unit, &special_fields, FLOW_S_F_QOS_PRI, &special_field_data), _SHR_E_NOT_FOUND);
    vlan_port->pkt_pri = special_field_data.shr_var_uint32;

    /** get cfi */
    sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
    SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_special_field_data_get
                              (unit, &special_fields, FLOW_S_F_QOS_CFI, &special_field_data), _SHR_E_NOT_FOUND);
    vlan_port->pkt_cfi = special_field_data.shr_var_uint32;

exit:
    SHR_FUNC_EXIT;
}

/**
 * Setting vlan_port fields according terminator
 */
shr_error_e
dnx_vlan_port_terminator_flow_get(
    int unit,
    bcm_vlan_port_t * vlan_port,
    bcm_flow_handle_info_t * flow_handle_info)
{
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_terminator_info_t vlan_port_flow_term = { 0 };
    bcm_flow_special_fields_t special_fields = { 0 };
    bcm_gport_t destination;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR, &flow_handle));
    flow_handle_info->flow_handle = flow_handle;

    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_get(unit, flow_handle_info, &vlan_port_flow_term, &special_fields));

    vlan_port->vsi = vlan_port_flow_term.vsi;
    vlan_port->ingress_network_group_id = vlan_port_flow_term.l2_ingress_info.ingress_network_group_id;

    vlan_port->group = vlan_port_flow_term.l2_learn_info.flush_group;

    if (vlan_port_flow_term.valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_STAT_ID_VALID)
    {
        vlan_port->flags |= BCM_VLAN_PORT_STAT_INGRESS_ENABLE;
    }

    if (vlan_port_flow_term.valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_FAILOVER_ID_VALID)
    {
        if (vlan_port_flow_term.failover_id != dnx_data_failover.path_select.ing_no_protection_get(unit))
        {
            vlan_port->ingress_failover_id = vlan_port_flow_term.failover_id;
        }
        vlan_port->ingress_failover_port_id = (vlan_port_flow_term.failover_state) ? 0 : 1;
    }

    destination = vlan_port_flow_term.l2_learn_info.dest_port;

    if (_SHR_GPORT_IS_FORWARD_PORT(destination))
    {
        vlan_port->failover_port_id = destination;
    }
    else if (_SHR_GPORT_IS_MCAST(destination))
    {
        vlan_port->failover_mc_group = _SHR_GPORT_MCAST_GET(destination);
    }
    else
    {
        /*
         * If the gport's forwarding destination is the same as the supplied physical port,
         * failover_port_id won't be set as we don't know whether the value originated from
         * the port field or from failover_port_id field.
         */
        uint32 encoded_dest_port;
        SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                        (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, vlan_port->port, &encoded_dest_port));
        if (encoded_dest_port != vlan_port_flow_term.l2_learn_info.dest_port)
        {
            vlan_port->failover_port_id = destination;
        }
    }

    /** handle large enrties and WIDE flag*/
    if (vlan_port_flow_term.valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_ADDITIONAL_DATA_VALID)
    {
        vlan_port->flags |= BCM_VLAN_PORT_INGRESS_WIDE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Destroy vlan port initiator
 */
static shr_error_e
dnx_vlan_port_initiator_flow_delete(
    int unit,
    bcm_vlan_port_t * vlan_port)
{

    bcm_flow_handle_info_t flow_handle_info = { 0 };
    bcm_flow_handle_t flow_handle = 0;
    uint32 global_lif;

    SHR_FUNC_INIT_VARS(unit);

    global_lif = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(vlan_port->vlan_port_id);

    BCM_GPORT_TUNNEL_ID_SET(flow_handle_info.flow_id, global_lif);

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_LL_INITIATOR, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;

    SHR_IF_ERR_EXIT(bcm_dnx_flow_initiator_info_destroy(unit, &flow_handle_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * Destroy vlan port terminator
 */
static shr_error_e
dnx_vlan_port_terminator_flow_delete(
    int unit,
    bcm_vlan_port_t * vlan_port)
{

    bcm_flow_handle_info_t flow_handle_info = { 0 };
    bcm_flow_handle_t flow_handle = 0;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);

    /** convert VLAN_PORT gport to LIF GPORT */
    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, vlan_port->vlan_port_id, _SHR_GPORT_TYPE_FLOW_LIF, &flow_handle_info.flow_id));

    SHR_IF_ERR_EXIT(dnx_vlan_port_terminator_flow_match_action
                    (unit, &flow_handle_info, vlan_port, FLOW_MATCH_ACTION_DELETE));

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, vlan_port->vlan_port_id,
                     DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_AND_GLOBAL_LIF |
                     DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT, &gport_hw_resources));

    SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_lif_match_info_clear(unit, 0, gport_hw_resources.local_in_lif));

    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_destroy(unit, &flow_handle_info));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in .h file
 */
shr_error_e
dnx_vlan_port_flow_create(
    int unit,
    bcm_vlan_port_t * vlan_port)
{

    bcm_vlan_port_t old_vlan_port;

    SHR_FUNC_INIT_VARS(unit);

    bcm_vlan_port_t_init(&old_vlan_port);

    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_REPLACE))
    {
        old_vlan_port.vlan_port_id = vlan_port->vlan_port_id;
        SHR_IF_ERR_EXIT(dnx_vlan_port_flow_get(unit, &old_vlan_port));
        /*
         * Verify that the new VLAN-Port configuration is same as old VLAN-Port configuraion
         */
        SHR_INVOKE_VERIFY_DNXC(dnx_vlan_port_create_replace_new_old_compare_verify(unit, vlan_port, &old_vlan_port));
    }

    if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY))
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_terminator_flow_create(unit, vlan_port, &old_vlan_port));
    }

    if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_INGRESS_ONLY))
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_initiator_flow_create(unit, vlan_port, &old_vlan_port));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in .h file
 */
shr_error_e
dnx_vlan_port_flow_get(
    int unit,
    bcm_vlan_port_t * vlan_port)
{

    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    bcm_flow_handle_info_t flow_handle_info;

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_handle_info_t_init(&flow_handle_info);

    if (vlan_port->vlan_port_id == 0)
    {

        /** Find by vlan_port criteria etc*/
        SHR_IF_ERR_EXIT(dnx_vlan_port_terminator_flow_match_action
                        (unit, &flow_handle_info, vlan_port, FLOW_MATCH_ACTION_GET));

        if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_INGRESS_ONLY))
        {
            BCM_GPORT_SUB_TYPE_LIF_SET(vlan_port->vlan_port_id, 0, BCM_GPORT_TUNNEL_ID_GET(flow_handle_info.flow_id));
            vlan_port->encap_id = BCM_GPORT_TUNNEL_ID_GET(flow_handle_info.flow_id);
        }
        else
        {
            BCM_GPORT_SUB_TYPE_LIF_SET(vlan_port->vlan_port_id, BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY,
                                       BCM_GPORT_TUNNEL_ID_GET(flow_handle_info.flow_id));
            vlan_port->encap_id = 0;
        }
        BCM_GPORT_VLAN_PORT_ID_SET(vlan_port->vlan_port_id, vlan_port->vlan_port_id);
    }

    /** convert VLAN_PORT gport to LIF GPORT */
    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, vlan_port->vlan_port_id, _SHR_GPORT_TYPE_FLOW_LIF, &flow_handle_info.flow_id));

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, vlan_port->vlan_port_id,
                     DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_AND_GLOBAL_LIF |
                     DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT, &gport_hw_resources));

    if ((BCM_GPORT_SUB_TYPE_LIF_EXC_GET(vlan_port->vlan_port_id) == BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY))
    {

        SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_lif_match_info_get(unit, DBAL_TABLE_IN_AC_INFO_DB,
                                                                        gport_hw_resources.local_in_lif, vlan_port));
        vlan_port->flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
        vlan_port->encap_id = 0;
        SHR_IF_ERR_EXIT(dnx_vlan_port_terminator_flow_get(unit, vlan_port, &flow_handle_info));
    }
    else
    {
        if ((BCM_GPORT_SUB_TYPE_LIF_EXC_GET(vlan_port->vlan_port_id) == BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY))
        {
            vlan_port->flags |= BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
            SHR_IF_ERR_EXIT(dnx_vlan_port_initiator_flow_get(unit, vlan_port, &flow_handle_info));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_lif_match_info_get(unit, DBAL_TABLE_IN_AC_INFO_DB,
                                                                            gport_hw_resources.local_in_lif,
                                                                            vlan_port));
            vlan_port->encap_id = gport_hw_resources.global_out_lif;
            SHR_IF_ERR_EXIT(dnx_vlan_port_terminator_flow_get(unit, vlan_port, &flow_handle_info));
            SHR_IF_ERR_EXIT(dnx_vlan_port_initiator_flow_get(unit, vlan_port, &flow_handle_info));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in .h file
 */
shr_error_e
dnx_vlan_port_flow_delete(
    int unit,
    bcm_gport_t gport)
{

    bcm_vlan_port_t vlan_port;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.vlan_port_id = gport;

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, vlan_port.vlan_port_id,
                     DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_AND_GLOBAL_LIF |
                     DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT, &gport_hw_resources));

    if (gport_hw_resources.local_out_lif != DNX_ALGO_GPM_LIF_INVALID)
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_initiator_flow_delete(unit, &vlan_port));
    }

    if (gport_hw_resources.local_in_lif != DNX_ALGO_GPM_LIF_INVALID)
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_lif_match_info_get(unit, DBAL_TABLE_IN_AC_INFO_DB,
                                                                        gport_hw_resources.local_in_lif, &vlan_port));

        SHR_IF_ERR_EXIT(dnx_vlan_port_terminator_flow_delete(unit, &vlan_port));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in .h file
 */
shr_error_e
dnx_vlan_port_flow_translation_set_ingress(
    int unit,
    bcm_vlan_port_translation_t * vlan_port_translation)
{
    bcm_flow_terminator_info_t vlan_port_flow_term;
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_special_fields_t special_fields;
    bcm_flow_field_id_t field_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_terminator_info_t_init(&vlan_port_flow_term);
    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_special_fields_t_init(&special_fields);

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;

    /** Encode VLAN-PORT gport back to FLOW gport */
    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, vlan_port_translation->gport, _SHR_GPORT_TYPE_FLOW_LIF, &flow_handle_info.flow_id));

    /** Use  REPLACE to set the VLAN Translation parameters */
    flow_handle_info.flags = BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID;

    /*
     * Special fields 
     */
    special_fields.actual_nof_special_fields = 3;

    SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "VLAN_EDIT_PROFILE", &field_id));
    special_fields.special_fields[0].field_id = field_id;
    special_fields.special_fields[0].shr_var_uint32 = vlan_port_translation->vlan_edit_class_id;

    SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "VLAN_EDIT_VID_1", &field_id));
    special_fields.special_fields[1].field_id = field_id;
    special_fields.special_fields[1].shr_var_uint32 = vlan_port_translation->new_outer_vlan;

    SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "VLAN_EDIT_VID_2", &field_id));
    special_fields.special_fields[2].field_id = field_id;
    special_fields.special_fields[2].shr_var_uint32 = vlan_port_translation->new_inner_vlan;

    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_create
                    (unit, &flow_handle_info, &vlan_port_flow_term, &special_fields));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in .h file
 */
shr_error_e
dnx_vlan_port_flow_translation_get_ingress(
    int unit,
    bcm_vlan_port_translation_t * vlan_port_translation)
{
    bcm_flow_terminator_info_t vlan_port_flow_term;
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_special_fields_t special_fields;
    bcm_flow_special_field_t special_field_data;

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_terminator_info_t_init(&vlan_port_flow_term);
    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_special_fields_t_init(&special_fields);

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;

    /** Encode VLAN-PORT gport back to FLOW gport */
    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, vlan_port_translation->gport, _SHR_GPORT_TYPE_FLOW_LIF, &flow_handle_info.flow_id));

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;

    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_get(unit, &flow_handle_info, &vlan_port_flow_term, &special_fields));

    /*
     * Get VLAN-Translation parameters.
     * Note:
     * In case SET was not called, the field is "not found".
     */

    /** Get VLAN_EDIT_PROFILE */
    sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
    SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_special_field_data_get
                              (unit, &special_fields, FLOW_S_F_VLAN_EDIT_PROFILE, &special_field_data),
                              _SHR_E_NOT_FOUND);
    vlan_port_translation->vlan_edit_class_id = special_field_data.shr_var_uint32;

    /** Get VID1 */
    sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
    SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_special_field_data_get
                              (unit, &special_fields, FLOW_S_F_VLAN_EDIT_VID_1, &special_field_data), _SHR_E_NOT_FOUND);
    vlan_port_translation->new_outer_vlan = special_field_data.shr_var_uint32;

    /** Get VID2 */
    sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
    SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_special_field_data_get
                              (unit, &special_fields, FLOW_S_F_VLAN_EDIT_VID_2, &special_field_data), _SHR_E_NOT_FOUND);
    vlan_port_translation->new_inner_vlan = special_field_data.shr_var_uint32;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in .h file
 */
shr_error_e
dnx_vlan_port_flow_translation_set_egress(
    int unit,
    bcm_vlan_port_translation_t * vlan_port_translation)
{
    bcm_flow_initiator_info_t vlan_port_flow_init;
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_special_fields_t special_fields;
    bcm_flow_field_id_t field_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_initiator_info_t_init(&vlan_port_flow_init);
    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_special_fields_t_init(&special_fields);

    if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(vlan_port_translation->gport))
    {
        special_fields.actual_nof_special_fields = 3;
        SHR_IF_ERR_EXIT(dnx_esem_initiator_flow_handle_get(unit, FALSE, &flow_handle));
    }
    else if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(vlan_port_translation->gport))
    {
        special_fields.actual_nof_special_fields = 3;
        SHR_IF_ERR_EXIT(dnx_esem_initiator_flow_handle_get(unit, TRUE, &flow_handle));
    }
    else if (BCM_GPORT_SUB_TYPE_IS_L3_VLAN_TRANSLATION(vlan_port_translation->gport))
    {
        uint8 is_2tag_valid = TRUE;
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
        bcm_gport_t gport;

        /*
         * Get local In-LIF using DNX Algo Gport Management:
         */

        /** convert VLAN-Port gport to FLOW gport */
        SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                        (unit, vlan_port_translation->gport, _SHR_GPORT_TYPE_FLOW_LIF, &gport));

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport,
                                                           DNX_ALGO_GPM_GPORT_HW_RESOURCES_EGRESS_LOCAL_LIF_OR_VIRTUAL,
                                                           &gport_hw_resources));
        if (gport_hw_resources.outlif_dbal_table_id == DBAL_TABLE_EEDB_ARP)
        {
            SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_valid_field
                            (unit, _SHR_CORE_ALL, gport_hw_resources.local_out_lif,
                             gport_hw_resources.outlif_dbal_table_id, FALSE, DBAL_FIELD_VLAN_EDIT_VID_2,
                             &is_2tag_valid));
        }

        if (is_2tag_valid)
        {
            special_fields.actual_nof_special_fields = 3;
        }
        else
        {
            special_fields.actual_nof_special_fields = 2;
        }

        SHR_IF_ERR_EXIT(bcm_flow_handle_get(unit, FLOW_APP_NAME_ARP_INITIATOR, &flow_handle));
    }
    else
    {
        special_fields.actual_nof_special_fields = 3;
        SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_LL_INITIATOR, &flow_handle));
    }

    flow_handle_info.flow_handle = flow_handle;

    /** Encode VLAN-PORT gport back to FLOW gport */
    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, vlan_port_translation->gport, _SHR_GPORT_TYPE_FLOW_LIF, &flow_handle_info.flow_id));

    /** Use  REPLACE to set the VLAN Translation parameters */
    flow_handle_info.flags = BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID;

    /*
     * Special fields 
     */

    SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "VLAN_EDIT_PROFILE", &field_id));
    special_fields.special_fields[0].field_id = field_id;
    special_fields.special_fields[0].shr_var_uint32 = vlan_port_translation->vlan_edit_class_id;

    SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "VLAN_EDIT_VID_1", &field_id));
    special_fields.special_fields[1].field_id = field_id;
    special_fields.special_fields[1].shr_var_uint32 = vlan_port_translation->new_outer_vlan;

    if (special_fields.actual_nof_special_fields == 3)
    {
        SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "VLAN_EDIT_VID_2", &field_id));
        special_fields.special_fields[2].field_id = field_id;
        special_fields.special_fields[2].shr_var_uint32 = vlan_port_translation->new_inner_vlan;
    }
    SHR_IF_ERR_EXIT(bcm_dnx_flow_initiator_info_create(unit, &flow_handle_info, &vlan_port_flow_init, &special_fields));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in .h file
 */
shr_error_e
dnx_vlan_port_flow_translation_get_egress(
    int unit,
    bcm_vlan_port_translation_t * vlan_port_translation)
{
    bcm_flow_initiator_info_t vlan_port_flow_init;
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_special_fields_t special_fields;
    bcm_flow_special_field_t special_field_data;

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_initiator_info_t_init(&vlan_port_flow_init);
    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_special_fields_t_init(&special_fields);

    if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(vlan_port_translation->gport))

    {
        SHR_IF_ERR_EXIT(dnx_esem_initiator_flow_handle_get(unit, FALSE, &flow_handle));
    }
    else if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(vlan_port_translation->gport))
    {
        SHR_IF_ERR_EXIT(dnx_esem_initiator_flow_handle_get(unit, TRUE, &flow_handle));
    }
    else if (BCM_GPORT_SUB_TYPE_IS_L3_VLAN_TRANSLATION(vlan_port_translation->gport))
    {
        SHR_IF_ERR_EXIT(bcm_flow_handle_get(unit, FLOW_APP_NAME_ARP_INITIATOR, &flow_handle));
    }
    else
    {
        SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_LL_INITIATOR, &flow_handle));
    }
    flow_handle_info.flow_handle = flow_handle;

    /** Encode VLAN-PORT gport back to FLOW gport */
    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, vlan_port_translation->gport, _SHR_GPORT_TYPE_FLOW_LIF, &flow_handle_info.flow_id));

    SHR_IF_ERR_EXIT(bcm_dnx_flow_initiator_info_get(unit, &flow_handle_info, &vlan_port_flow_init, &special_fields));

    /*
     * Get VLAN-Translation parameters.
     * Note:
     * In case SET was not called, the field is "not found".
     */

    /** Get VLAN_EDIT_PROFILE */
    sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
    SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_special_field_data_get
                              (unit, &special_fields, FLOW_S_F_VLAN_EDIT_PROFILE, &special_field_data),
                              _SHR_E_NOT_FOUND);
    vlan_port_translation->vlan_edit_class_id = special_field_data.shr_var_uint32;

    /** Get VID1 */
    sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
    SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_special_field_data_get
                              (unit, &special_fields, FLOW_S_F_VLAN_EDIT_VID_1, &special_field_data), _SHR_E_NOT_FOUND);
    vlan_port_translation->new_outer_vlan = special_field_data.shr_var_uint32;

    /** Get VID2 - not always exist */
    sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
    SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_special_field_data_get
                              (unit, &special_fields, FLOW_S_F_VLAN_EDIT_VID_2, &special_field_data), _SHR_E_NOT_FOUND);
    vlan_port_translation->new_inner_vlan = special_field_data.shr_var_uint32;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in .h file
 */
shr_error_e
dnx_vswitch_port_flow_add(
    int unit,
    bcm_vlan_t vsi,
    int learn_enable,
    bcm_gport_t port)
{
    bcm_flow_terminator_info_t vlan_port_flow_term;
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_handle_info_t flow_handle_info;

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_terminator_info_t_init(&vlan_port_flow_term);
    bcm_flow_handle_info_t_init(&flow_handle_info);

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;

    /** Encode VLAN-PORT gport back to FLOW gport */
    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert(unit, port, _SHR_GPORT_TYPE_FLOW_LIF, &flow_handle_info.flow_id));

    /** Use  REPLACE to set VSI and learning */
    flow_handle_info.flags = BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID;

    /** Update LIF's VSI*/
    vlan_port_flow_term.valid_elements_set = BCM_FLOW_TERMINATOR_ELEMENT_VSI_VALID;
    vlan_port_flow_term.vsi = vsi;

    vlan_port_flow_term.valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_LEARN_ENABLE_VALID;
    vlan_port_flow_term.learn_enable = learn_enable;

    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_create(unit, &flow_handle_info, &vlan_port_flow_term, NULL));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in .h file
 */
shr_error_e
dnx_vswitch_port_flow_get(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t * vsi)
{
    bcm_flow_terminator_info_t vlan_port_flow_term;
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_special_fields_t special_fields;

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_terminator_info_t_init(&vlan_port_flow_term);
    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_special_fields_t_init(&special_fields);

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;

    /** Encode VLAN-PORT gport back to FLOW gport */
    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert(unit, port, _SHR_GPORT_TYPE_FLOW_LIF, &flow_handle_info.flow_id));

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;

    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_get(unit, &flow_handle_info, &vlan_port_flow_term, &special_fields));

    *vsi = vlan_port_flow_term.vsi;

exit:
    SHR_FUNC_EXIT;
}
