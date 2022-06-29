/** \file vlan_port_flow.c
 *  General vlan port functionality using flow lif for DNX.
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
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dnx/failover/failover.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/port/port_match.h>
#include <bcm_int/dnx/auto_generated/dnx_flow_dispatch.h>
#include "vlan_port_flow.h"
#include "esem_flow.h"
#include <soc/dnx/dnx_data/auto_generated/dnx_data_failover.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trunk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_multicast.h>
#include <bcm_int/dnx/qos/qos.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>

typedef struct
{
    void *user_data;
    bcm_vlan_port_traverse_cb user_cb;
    int is_symmetric_traverse;
    uint32 drop_in_lif;
    uint32 initial_bridge_in_lif;
    uint32 recycle_default_inlif_mp;
    uint32 recycle_default_inlif_p2p;
    uint32 vtt4_default_trap_in_lif;
    uint32 vtt4_default_0_tag_in_lif;
} dnx_vlan_port_flow_ingress_user_data_t;

shr_error_e
dnx_vlan_port_flow_terminator_l2_learn_info_set(
    int unit,
    bcm_flow_terminator_info_t * vlan_port_flow_term,
    bcm_vlan_port_t * vlan_port)
{
    uint32 destination = 0;
    dbal_fields_e dbal_dest_type;
    uint32 dest_val;

    SHR_FUNC_INIT_VARS(unit);

    vlan_port_flow_term->l2_learn_info.flush_group = vlan_port->group;

    if (vlan_port->failover_port_id)
    {
        vlan_port_flow_term->l2_learn_info.dest_port = vlan_port->failover_port_id;
        vlan_port_flow_term->l2_learn_info.l2_learn_info_flags |= BCM_FLOW_L2_LEARN_INFO_DEST_ONLY;
    }
    else
    {
        if (vlan_port->failover_mc_group)
        {
            _SHR_GPORT_MCAST_SET(vlan_port_flow_term->l2_learn_info.dest_port,
                                 _SHR_MULTICAST_ID_GET(vlan_port->failover_mc_group));
            vlan_port_flow_term->l2_learn_info.l2_learn_info_flags |= BCM_FLOW_L2_LEARN_INFO_DEST_ONLY;
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
        if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_RECYCLE))
        {
            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
            {
                vlan_port_flow_term->valid_elements_set = 0;
                vlan_port_flow_term->learn_enable = FALSE;
                if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION))
                {
                    vlan_port_flow_term->valid_elements_set = (BCM_FLOW_TERMINATOR_ELEMENT_VSI_VALID |
                                                               BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID);

                    vlan_port_flow_term->l2_ingress_info.ingress_network_group_id = vlan_port->ingress_network_group_id;
                    vlan_port_flow_term->vsi = vlan_port->vsi;
                }

            }
            else
            {
                vlan_port_flow_term->valid_elements_set = (BCM_FLOW_TERMINATOR_ELEMENT_VSI_VALID |
                                                           BCM_FLOW_TERMINATOR_ELEMENT_L2_LEARN_INFO_VALID |
                                                           BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID);

                vlan_port_flow_term->l2_ingress_info.ingress_network_group_id = vlan_port->ingress_network_group_id;
                vlan_port_flow_term->vsi = vlan_port->vsi;
            }

            /*
             * The Learn enable is set during standard creation, but during update the previous
             * value remains.
             * Note:
             * For native learning is disabled!
             */
            if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_REPLACE) &&
                !_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
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

            if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
            {
                SHR_IF_ERR_EXIT(dnx_vlan_port_flow_terminator_l2_learn_info_set(unit, vlan_port_flow_term, vlan_port));
            }

            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_STAT_INGRESS_ENABLE))
            {
                vlan_port_flow_term->valid_elements_set |=
                    BCM_FLOW_TERMINATOR_ELEMENT_STAT_ID_VALID | BCM_FLOW_TERMINATOR_ELEMENT_STAT_PP_PROFILE_VALID;
                vlan_port_flow_term->stat_id = 0;
                vlan_port_flow_term->stat_pp_profile = 0;
            }
            else
            {
                vlan_port_flow_term->valid_elements_clear |=
                    BCM_FLOW_TERMINATOR_ELEMENT_STAT_ID_VALID | BCM_FLOW_TERMINATOR_ELEMENT_STAT_PP_PROFILE_VALID;
            }

            if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
            {
                if (DNX_FAILOVER_IS_PROTECTION_ENABLED(vlan_port->ingress_failover_id))
                {
                    vlan_port_flow_term->valid_elements_set |=
                        BCM_FLOW_TERMINATOR_ELEMENT_FAILOVER_ID_VALID |
                        BCM_FLOW_TERMINATOR_ELEMENT_FAILOVER_STATE_VALID;

                    vlan_port_flow_term->failover_state = (vlan_port->ingress_failover_port_id) ? 0 : 1;
                    BCM_FAILOVER_SET(vlan_port_flow_term->failover_id, vlan_port->ingress_failover_id,
                                     BCM_FAILOVER_TYPE_INGRESS);
                }

                if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_INGRESS_WIDE))
                {
                    vlan_port_flow_term->valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_ADDITIONAL_DATA_VALID;
                    vlan_port_flow_term->additional_data = 0;
                }
                else
                {
                    vlan_port_flow_term->valid_elements_clear |= BCM_FLOW_TERMINATOR_ELEMENT_ADDITIONAL_DATA_VALID;
                }
            }
            /**
             * AC default all ingress qos model (phb/remark/ttl) is pipe.
             * ingress native AC, all ingress qos model (phb/remark/ttl) should be uniform.
             */
            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
            {
                vlan_port_flow_term->valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_QOS_INGRESS_MODEL_VALID;
                vlan_port_flow_term->ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
                vlan_port_flow_term->ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
                vlan_port_flow_term->ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;
                vlan_port_flow_term->ingress_qos_model.ingress_ecn = bcmQosIngressEcnModelInvalid;
            }
        }
        else
        {
            /** Control LIF */
            vlan_port_flow_term->valid_elements_set = (BCM_FLOW_TERMINATOR_ELEMENT_VSI_VALID |
                                                       BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID |
                                                       BCM_FLOW_TERMINATOR_ELEMENT_QOS_INGRESS_MODEL_VALID);

            vlan_port_flow_term->l2_ingress_info.ingress_network_group_id = vlan_port->ingress_network_group_id;
            vlan_port_flow_term->vsi = vlan_port->vsi;
            vlan_port_flow_term->learn_enable = FALSE;

            vlan_port_flow_term->ingress_qos_model.ingress_phb = vlan_port->ingress_qos_model.ingress_phb;
            vlan_port_flow_term->ingress_qos_model.ingress_remark = vlan_port->ingress_qos_model.ingress_remark;
            vlan_port_flow_term->ingress_qos_model.ingress_ttl = vlan_port->ingress_qos_model.ingress_ttl;
            vlan_port_flow_term->ingress_qos_model.ingress_ecn = vlan_port->ingress_qos_model.ingress_ecn;

        }
    }

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
    bcm_flow_terminator_info_t * flow_terminator_info,
    bcm_flow_special_fields_t * special_fields,
    bcm_vlan_port_t * vlan_port)
{
    bcm_flow_field_id_t field_id = 0;
    int fodo_assigment_mode;
    int indexed_mode_enable = 0;
    ac_application_type_e ll_app_type = NUM_AC_APPLICATION_TYPE_E;

    SHR_FUNC_INIT_VARS(unit);

    special_fields->actual_nof_special_fields = 0;

    if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_REPLACE))
    {
        /** Ingress native virtual does not have VSI and FODO_ASSIGNMENT_MODE fields */
        if (!(_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE) &&
              _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION)))
        {
            special_fields->actual_nof_special_fields += 1;

            SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, "VSI_ASSIGNMENT_MODE", &field_id));
            SHR_IF_ERR_EXIT(dnx_vlan_port_set_ll_app_type(unit, vlan_port, &ll_app_type));
            SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_fodo_mode_calc
                            (unit, vlan_port, ll_app_type, &fodo_assigment_mode));
            special_fields->special_fields[0].field_id = field_id;
            special_fields->special_fields[0].symbol = fodo_assigment_mode;
        }

        if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_RECYCLE))
        {
            /** real value for VLAN_EDIT_PCP_DEI_PROFILE is being set by bcm_qos_port_map_set */
            SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get
                            (unit, flow_handle, "VLAN_EDIT_PCP_DEI_PROFILE", &field_id));
            special_fields->special_fields[special_fields->actual_nof_special_fields].field_id = field_id;
            special_fields->special_fields[special_fields->actual_nof_special_fields].shr_var_uint32 = 0;
            DNX_QOS_VLAN_PCP_MAP_SET(special_fields->
                                     special_fields[special_fields->actual_nof_special_fields].shr_var_uint32);

            special_fields->actual_nof_special_fields += 1;
        }

        /*
         * Set native index mode only when
         * - inlif is native and non virtual
         * - indexed mode is allocated in in_lif_profile
         */
        indexed_mode_enable = dnx_data_lif.in_lif.in_lif_profile_allocate_indexed_mode_get(unit);
        if ((indexed_mode_enable == IN_LIF_PROFILE_PWE_INDEXED_MODE_NOF_VALUES) &&
            _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE) &&
            (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION)))
        {
            SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, "NATIVE_INDEX_MODE", &field_id));
            special_fields->special_fields[special_fields->actual_nof_special_fields].field_id = field_id;
            special_fields->special_fields[special_fields->actual_nof_special_fields].shr_var_uint32 = 1;
            special_fields->actual_nof_special_fields += 1;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Set/get/delete TCAM match fields for vlan port terminator according criteria
 */
static shr_error_e
dnx_vlan_port_terminator_flow_match_tcam_action(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_vlan_port_t * vlan_port,
    bcm_flow_special_fields_t * special_fields)
{
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_field_id_t field_id = 0;
    uint32 vlan_domain;
    uint32 layer_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_LL_AC_TCAM_TERM_MATCH, &flow_handle));

    /*
     * Set attribute priority 
     */
    flow_handle_info->flow_priority = 3;

    /*
     * Set TCAM Keys
     * All cases sets VLAN_DOMAIN and EtherType 
     */
    special_fields->actual_nof_special_fields = 2;

    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, "VLAN_DOMAIN", &field_id));
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, vlan_port->port, &vlan_domain));
    special_fields->special_fields[0].field_id = field_id;
    special_fields->special_fields[0].shr_var_uint32 = vlan_domain;

    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, "LAYER_TYPE", &field_id));
    SHR_IF_ERR_EXIT(dnx_vlan_port_dbal_ethertype_convert(unit, vlan_port->match_ethertype, &layer_type));
    special_fields->special_fields[1].field_id = field_id;
    special_fields->special_fields[1].shr_var_uint32 = layer_type;

    /*
     * Check supported criterias
     */
    switch (vlan_port->criteria)
    {

        case BCM_VLAN_PORT_MATCH_PORT:
        case BCM_VLAN_PORT_MATCH_PORT_UNTAGGED:

            break;

        case BCM_VLAN_PORT_MATCH_PORT_VLAN:
        case BCM_VLAN_PORT_MATCH_PORT_CVLAN:

            special_fields->actual_nof_special_fields = 3;

            SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, "VID_OUTER_VLAN", &field_id));
            special_fields->special_fields[2].field_id = field_id;
            special_fields->special_fields[2].shr_var_uint32 = vlan_port->match_vlan;

            break;

        case BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED:

            special_fields->actual_nof_special_fields = 4;

            SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, "VID_OUTER_VLAN", &field_id));
            special_fields->special_fields[2].field_id = field_id;
            special_fields->special_fields[2].shr_var_uint32 = vlan_port->match_vlan;

            SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, "VID_INNER_VLAN", &field_id));
            special_fields->special_fields[3].field_id = field_id;
            special_fields->special_fields[3].shr_var_uint32 = vlan_port->match_inner_vlan;

            break;

        case BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN:

            special_fields->actual_nof_special_fields = 4;

            SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, "VID_OUTER_VLAN", &field_id));
            special_fields->special_fields[2].field_id = field_id;
            special_fields->special_fields[2].shr_var_uint32 = vlan_port->match_vlan;

            SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, "PCP_DEI_OUTER_VLAN", &field_id));
            special_fields->special_fields[3].field_id = field_id;
            special_fields->special_fields[3].shr_var_uint32 = vlan_port->match_pcp;

            break;

        case BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED:

            special_fields->actual_nof_special_fields = 5;

            SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, "VID_OUTER_VLAN", &field_id));
            special_fields->special_fields[2].field_id = field_id;
            special_fields->special_fields[2].shr_var_uint32 = vlan_port->match_vlan;

            SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, "VID_INNER_VLAN", &field_id));
            special_fields->special_fields[3].field_id = field_id;
            special_fields->special_fields[3].shr_var_uint32 = vlan_port->match_inner_vlan;

            SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, "PCP_DEI_OUTER_VLAN", &field_id));
            special_fields->special_fields[4].field_id = field_id;
            special_fields->special_fields[4].shr_var_uint32 = vlan_port->match_pcp;

            break;

        default:

            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported criteria = %d for match EtherType = 0x%x\n", vlan_port->criteria,
                         vlan_port->match_ethertype);
            break;
    }

    flow_handle_info->flow_handle = flow_handle;

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

    SHR_FUNC_INIT_VARS(unit);

    if (vlan_port->match_ethertype == 0)
    {
        uint32 vlan_domain = 0;
        uint32 is_intf_namespace = 0;
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources_match;

        /*
         * For Native, determine NW-scopped or LIF-scopped 
         * Note: not relevant for BCM_VLAN_PORT_MATCH_NONE case
         */
        if ((_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE)) &&
            (vlan_port->criteria != BCM_VLAN_PORT_MATCH_NONE))
        {
            bcm_gport_t flow_gport;

            SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                            (unit, vlan_port->port, BCM_GPORT_TYPE_FLOW_LIF,
                             DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR, &flow_gport));

            SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                        (unit, flow_gport,
                                         DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS |
                                         DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources_match),
                                        _SHR_E_NOT_FOUND, _SHR_E_PARAM);

            /*
             * get vlan_domain and check whether LIF or NETWORK scope should be used
             */
            SHR_IF_ERR_EXIT(dnx_ingress_native_ac_interface_namespace_check
                            (unit, flow_gport, &gport_hw_resources_match, &vlan_domain, &is_intf_namespace));
        }
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
                if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
                {
                    if (!is_intf_namespace)
                    {
                        SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get
                                        (unit, FLOW_APP_NAME_VLAN_PORT_AC_NATIVE_NW_SCOPED_0_VLAN_TERM_MATCH,
                                         &flow_handle));
                        special_fields.actual_nof_special_fields = 1;

                        SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "VLAN_DOMAIN", &field_id));
                        special_fields.special_fields[0].field_id = field_id;
                        special_fields.special_fields[0].shr_var_uint32 = vlan_domain;
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get
                                        (unit, FLOW_APP_NAME_VLAN_PORT_AC_NATIVE_LIF_SCOPED_0_VLAN_TERM_MATCH,
                                         &flow_handle));
                        special_fields.actual_nof_special_fields = 1;

                        SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "GLOB_IN_LIF", &field_id));
                        special_fields.special_fields[0].field_id = field_id;
                        special_fields.special_fields[0].shr_var_uint32 = vlan_port->port;
                    }
                }
                else
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get
                                    (unit, FLOW_APP_NAME_VLAN_PORT_LL_PP_PORT_TERM_MATCH, &flow_handle));
                    special_fields.actual_nof_special_fields = 1;

                    SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "PORT", &field_id));
                    special_fields.special_fields[0].field_id = field_id;
                    special_fields.special_fields[0].shr_var_uint32 = vlan_port->port;
                }
                break;

            case BCM_VLAN_PORT_MATCH_PORT_VLAN:
                if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
                {
                    if (!is_intf_namespace)
                    {
                        SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get
                                        (unit, FLOW_APP_NAME_VLAN_PORT_AC_NATIVE_NW_SCOPED_1_VLAN_TERM_MATCH,
                                         &flow_handle));
                        special_fields.actual_nof_special_fields = 2;

                        SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "VLAN_DOMAIN", &field_id));
                        special_fields.special_fields[0].field_id = field_id;
                        special_fields.special_fields[0].shr_var_uint32 = vlan_domain;
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get
                                        (unit, FLOW_APP_NAME_VLAN_PORT_AC_NATIVE_LIF_SCOPED_1_VLAN_TERM_MATCH,
                                         &flow_handle));
                        special_fields.actual_nof_special_fields = 2;

                        SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "GLOB_IN_LIF", &field_id));
                        special_fields.special_fields[0].field_id = field_id;
                        special_fields.special_fields[0].shr_var_uint32 = vlan_port->port;
                    }

                    SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "VID", &field_id));
                    special_fields.special_fields[1].field_id = field_id;
                    special_fields.special_fields[1].shr_var_uint32 = vlan_port->match_vlan;
                }
                else
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get
                                    (unit, FLOW_APP_NAME_VLAN_PORT_LL_AC_S_VLAN_TERM_MATCH, &flow_handle));
                    special_fields.actual_nof_special_fields = 2;

                    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, "S_VID", &field_id));
                    special_fields.special_fields[0].field_id = field_id;
                    special_fields.special_fields[0].shr_var_uint32 = vlan_port->match_vlan;

                    SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, "VLAN_DOMAIN", &field_id));
                    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, vlan_port->port, &vlan_domain));
                    special_fields.special_fields[1].field_id = field_id;
                    special_fields.special_fields[1].shr_var_uint32 = vlan_domain;
                }
                break;

            case BCM_VLAN_PORT_MATCH_PORT_CVLAN:
                SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get
                                (unit, FLOW_APP_NAME_VLAN_PORT_LL_AC_C_VLAN_TERM_MATCH, &flow_handle));
                special_fields.actual_nof_special_fields = 2;

                SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, "C_VID", &field_id));
                special_fields.special_fields[0].field_id = field_id;
                special_fields.special_fields[0].shr_var_uint32 = vlan_port->match_vlan;

                SHR_IF_ERR_EXIT(bcm_dnx_flow_logical_field_id_get(unit, flow_handle, "VLAN_DOMAIN", &field_id));
                SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, vlan_port->port, &vlan_domain));
                special_fields.special_fields[1].field_id = field_id;
                special_fields.special_fields[1].shr_var_uint32 = vlan_domain;
                break;

            case BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED:
                if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
                {
                    if (!is_intf_namespace)
                    {
                        SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get
                                        (unit, FLOW_APP_NAME_VLAN_PORT_AC_NATIVE_NW_SCOPED_2_VLAN_TERM_MATCH,
                                         &flow_handle));
                        special_fields.actual_nof_special_fields = 3;

                        SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "VLAN_DOMAIN", &field_id));
                        special_fields.special_fields[0].field_id = field_id;
                        special_fields.special_fields[0].shr_var_uint32 = vlan_domain;
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get
                                        (unit, FLOW_APP_NAME_VLAN_PORT_AC_NATIVE_LIF_SCOPED_2_VLAN_TERM_MATCH,
                                         &flow_handle));
                        special_fields.actual_nof_special_fields = 3;

                        SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "GLOB_IN_LIF", &field_id));
                        special_fields.special_fields[0].field_id = field_id;
                        special_fields.special_fields[0].shr_var_uint32 = vlan_port->port;
                    }

                    SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "VID_OUTER_VLAN", &field_id));
                    special_fields.special_fields[1].field_id = field_id;
                    special_fields.special_fields[1].shr_var_uint32 = vlan_port->match_vlan;

                    SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "VID_INNER_VLAN", &field_id));
                    special_fields.special_fields[2].field_id = field_id;
                    special_fields.special_fields[2].shr_var_uint32 = vlan_port->match_inner_vlan;

                }
                else
                {
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
                    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, vlan_port->port, &vlan_domain));
                    special_fields.special_fields[2].field_id = field_id;
                    special_fields.special_fields[2].shr_var_uint32 = vlan_domain;
                }
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
                SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, vlan_port->port, &vlan_domain));
                special_fields.special_fields[2].field_id = field_id;
                special_fields.special_fields[2].shr_var_uint32 = vlan_domain;
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
                SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, vlan_port->port, &vlan_domain));
                special_fields.special_fields[2].field_id = field_id;
                special_fields.special_fields[2].shr_var_uint32 = vlan_domain;
                break;

            case BCM_VLAN_PORT_MATCH_PORT_UNTAGGED:

                SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get
                                (unit, FLOW_APP_NAME_VLAN_PORT_LL_AC_UNTAGGED_DPC_TERM_MATCH, &flow_handle));
                SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "IN_PORT", &field_id));

                special_fields.actual_nof_special_fields = 1;
                special_fields.special_fields[0].field_id = field_id;
                special_fields.special_fields[0].shr_var_uint32 = vlan_port->port;
                break;

            default:

                SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported criteria = %d \n", vlan_port->criteria);

                break;
        }

        flow_handle_info->flow_handle = flow_handle;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_terminator_flow_match_tcam_action
                        (unit, flow_handle_info, vlan_port, &special_fields));
    }

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
     * BCM_FLOW_INITIATOR_ELEMENT_L2_EGRESS_INFO_VALID |
     */
    vlan_port_flow_init->valid_elements_set |= BCM_FLOW_INITIATOR_ELEMENT_L2_EGRESS_INFO_VALID;
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

        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_EGRESS_SVTAG_PORT_ENABLE) ||
            _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_EGRESS_SVTAG_ENABLE))
        {
            /** Because SVTAG is not replaceable, we will not set it in replace */
            vlan_port_flow_init->valid_elements_set |= BCM_FLOW_INITIATOR_ELEMENT_SVTAG_ENABLE_VALID;
        }

    }

    if ((dnx_data_multicast.params.feature_get(unit, dnx_data_multicast_params_mc_pruning_actions_not_supported)) &&
        (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_EGRESS_PROTECTION)))
    {
        vlan_port_flow_init->valid_elements_set |= BCM_FLOW_INITIATOR_ELEMENT_FAILOVER_ID_VALID |
            BCM_FLOW_INITIATOR_ELEMENT_FAILOVER_STATE_VALID;
        vlan_port_flow_init->failover_state = (vlan_port->egress_failover_port_id) ? 0 : 1;
        vlan_port_flow_init->failover_id = vlan_port->egress_failover_id;
    }
    vlan_port_flow_init->l3_intf_id = BCM_IF_INVALID;
    sal_memcpy(&vlan_port_flow_init->egress_qos_model, &vlan_port->egress_qos_model,
               sizeof(vlan_port->egress_qos_model));

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vlan_port_flow_initiator_special_fields_set(
    int unit,
    bcm_flow_special_fields_t * special_fields,
    bcm_vlan_port_t * vlan_port,
    uint8 svtag_port_get)
{
    SHR_FUNC_INIT_VARS(unit);

    SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, FLOW_S_F_QOS_PRI, vlan_port->pkt_pri);

    SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, FLOW_S_F_QOS_CFI, vlan_port->pkt_cfi);

    if ((_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE)) && (vlan_port->tunnel_id != 0))
    {
        bcm_gport_t flow_id;

        SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                        (unit, vlan_port->tunnel_id,
                         BCM_GPORT_TYPE_FLOW_LIF, DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_INITIATOR, &flow_id));

        SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, FLOW_S_F_NEXT_OUTLIF_POINTER, flow_id);
    }

    if (!svtag_port_get && _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_EGRESS_SVTAG_PORT_ENABLE))
    {
        SPECIAL_FIELD_ENABLER_ADD(special_fields, FLOW_S_F_VLAN_PORT_SVTAG_PORT);
    }
    else if (svtag_port_get && !_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_EGRESS_SVTAG_PORT_ENABLE))
    {
        SPECIAL_FIELD_CLEAR(special_fields, FLOW_S_F_VLAN_PORT_SVTAG_PORT);
    }
exit:
    SHR_FUNC_EXIT;
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
    uint8 svtag_port_get = FALSE;
    int is_replace = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_REPLACE);

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_initiator_info_t_init(&vlan_port_flow_init);
    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_special_fields_t_init(&special_fields);

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_LL_INITIATOR, &flow_handle));

    flow_handle_info.flow_handle = flow_handle;

    /** Replace flags has to come with WITH_ID flag*/
    if (is_replace == TRUE)
    {
        svtag_port_get = _SHR_IS_FLAG_SET(old_vlan_port->flags, BCM_VLAN_PORT_EGRESS_SVTAG_PORT_ENABLE);
        flow_handle_info.flags = BCM_FLOW_HANDLE_INFO_REPLACE;
    }

    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_WITH_ID))
    {
        SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                        (unit, vlan_port->vlan_port_id, BCM_GPORT_TYPE_FLOW_LIF,
                         DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_INITIATOR, &flow_handle_info.flow_id));
        flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_WITH_ID;
    }
    else if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_ENCAP_WITH_ID))
    {
        flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_WITH_ID;
        flow_handle_info.flow_id = BCM_ENCAP_ID_GET(vlan_port->encap_id);
    }

    if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY) && !is_replace)
    {
        flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_SYMMETRIC_ALLOC;
    }

    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE) && (!is_replace))
    {
        flow_handle_info.flags |= BCM_FLOW_HANDLE_NATIVE;

        /*
         * Application encap_access_default_mapping is bcmEncapAccessAc (phase 8).
         * For native need to use bcmEncapAccessTunnel1 (phase 3)
         */
        vlan_port_flow_init.encap_access = bcmEncapAccessTunnel1;

        /** For native - support single side symmetric */
        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY) &&
            _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_ALLOC_SYMMETRIC))
        {
            flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_SYMMETRIC_ALLOC;
        }
    }

    SHR_IF_ERR_EXIT(dnx_vlan_port_flow_initiator_common_fields_set(unit, &vlan_port_flow_init, vlan_port));
    SHR_IF_ERR_EXIT(dnx_vlan_port_flow_initiator_special_fields_set(unit, &special_fields, vlan_port, svtag_port_get));
    SHR_IF_ERR_EXIT(bcm_dnx_flow_initiator_info_create(unit, &flow_handle_info, &vlan_port_flow_init, &special_fields));

    /** Update ALGO it is symmetric allocation */
    if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY) && !is_replace)
    {
        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_update_symmetric_indication
                        (unit, _SHR_GPORT_FLOW_LIF_VAL_GET_ID(flow_handle_info.flow_id), TRUE));
    }

    /** Encode vlan_port_id using flow_id */
    if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_WITH_ID))
    {
        SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert(unit, flow_handle_info.flow_id, _SHR_GPORT_TYPE_VLAN_PORT,
                                                    DNX_ALGO_GPM_CONVERSION_FLAG_NONE, &vlan_port->vlan_port_id));

    }
    vlan_port->encap_id = _SHR_GPORT_FLOW_LIF_VAL_GET_ID(flow_handle_info.flow_id);

exit:
    SHR_FUNC_EXIT;
}

/**
 * vlan port terminator flow handle get (using gport)
 */
static shr_error_e
dnx_vlan_port_terminator_flow_handle_from_gport_get(
    int unit,
    bcm_gport_t vlan_port_id,
    bcm_flow_handle_t * flow_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_INGRESS_NATIVE(vlan_port_id))
    {
        SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get
                        (unit, FLOW_APP_NAME_VLAN_PORT_AC_NATIVE_VIRTUAL_TERMINATOR, flow_handle));
    }
    else if (BCM_GPORT_SUB_TYPE_LIF_EXC_GET(vlan_port_id) == BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_AC_NATIVE_TERMINATOR, flow_handle));
    }
    else if (_SHR_GPORT_SUB_TYPE_IS_VIRTUAL_INGRESS_NON_NATIVE(vlan_port_id))
    {
        SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR, flow_handle));
    }
    else if (BCM_GPORT_SUB_TYPE_LIF_EXC_GET(vlan_port_id) == 0)
    {
        /*
         * For symmetric case, it could be native or non-native
         */
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

        bcm_gport_t flow_gport;

        SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                        (unit, vlan_port_id, BCM_GPORT_TYPE_FLOW_LIF,
                         DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR, &flow_gport));
        /*
         * Get In-LIF table ID:
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, flow_gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources));

        if (gport_hw_resources.inlif_dbal_table_id == DBAL_TABLE_IN_AC_INFO_DB)
        {
            SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR, flow_handle));
        }
        else if (gport_hw_resources.inlif_dbal_table_id == DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION)
        {
            SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_AC_NATIVE_TERMINATOR, flow_handle));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! vlan_port_id = 0x%08X but it's IN-LIF table = %d is wrong! \n", vlan_port_id,
                         gport_hw_resources.inlif_dbal_table_id);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error! Could not find VLAN-PORT flow application for vlan_port_id = 0x%08X! \n", vlan_port_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * vlan port terminator flow handle get (using flags)
 */
static shr_error_e
dnx_vlan_port_terminator_flow_handle_from_flags_get(
    int unit,
    uint32 flags,
    bcm_flow_handle_t * flow_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(flags, BCM_VLAN_PORT_NATIVE))
    {
        if (_SHR_IS_FLAG_SET(flags, BCM_VLAN_PORT_VLAN_TRANSLATION))
        {
            SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get
                            (unit, FLOW_APP_NAME_VLAN_PORT_AC_NATIVE_VIRTUAL_TERMINATOR, flow_handle));
        }
        else
        {
            SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_AC_NATIVE_TERMINATOR, flow_handle));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR, flow_handle));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Veify vlan port terminator "with ID"
 *
 */
static shr_error_e
dnx_vlan_port_terminator_flow_create_with_id_verify(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    SHR_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_SUB_TYPE_LIF_EXC_GET(vlan_port->vlan_port_id) == BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY)
    {
        /*
         * Ingress only native: verify that native flag is set
         */
        if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error.  vlan_port_id = 0x%08X which is Ingress only native but NATIVE flag (0x%08X) is not set! flags = 0x%08X\n",
                         vlan_port->vlan_port_id, BCM_VLAN_PORT_NATIVE, vlan_port->flags);
        }
    }
    else if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_INGRESS_NATIVE(vlan_port->vlan_port_id))
    {
        /*
         * Ingress only native virtual: verify the native+virtual flags are set
         */
        if ((!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE)) ||
            (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error.  vlan_port_id = 0x%08X which is Ingress only native virtual but NATIVE (0x%08X) or VIRTUAL flags (0x%08X) are not set! flags = 0x%08X\n",
                         vlan_port->vlan_port_id, BCM_VLAN_PORT_NATIVE, BCM_VLAN_PORT_VLAN_TRANSLATION,
                         vlan_port->flags);
        }
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
    int is_replace, is_native, is_ingress_only, is_with_id;

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_terminator_info_t_init(&vlan_port_flow_term);
    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_special_fields_t_init(&special_fields);

    is_replace = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_REPLACE);
    is_native = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE);
    is_ingress_only = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_INGRESS_ONLY);
    is_with_id = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_WITH_ID);

    /** Single side symmetric is only supported in egress native!*/
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_ALLOC_SYMMETRIC))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error!  BCM_VLAN_PORT_ALLOC_SYMMETRIC (0x%08X) flag is only supported for Egress native! flags = 0x%08X\n",
                     BCM_VLAN_PORT_ALLOC_SYMMETRIC, vlan_port->flags);
    }

    if (is_with_id)
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_terminator_flow_create_with_id_verify(unit, vlan_port));
    }

    SHR_IF_ERR_EXIT(dnx_vlan_port_terminator_flow_handle_from_flags_get(unit, vlan_port->flags, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;

    if (is_replace)
    {
        SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                        (unit, vlan_port->vlan_port_id, BCM_GPORT_TYPE_FLOW_LIF,
                         DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR, &flow_handle_info.flow_id));

        SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_get
                        (unit, &flow_handle_info, &vlan_port_flow_term, &special_fields));

        flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID;
    }

    if (is_with_id)
    {
        SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                        (unit, vlan_port->vlan_port_id, BCM_GPORT_TYPE_FLOW_LIF,
                         DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR, &flow_handle_info.flow_id));

        flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_WITH_ID;

        if ((!is_replace) && FLOW_GPORT_IS_VIRTUAL(flow_handle_info.flow_id))
        {
            flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_VIRTUAL;
        }
    }
    else
    {
        if ((!is_replace) && is_ingress_only && (!is_native))
        {
            flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_VIRTUAL;
        }
    }

    /*
     * Note:
     * In symmetric case, the flow_id was created by the initiator.
     * Need to take it's value from vlan_port_id and properly set flow flags.
     */
    if ((!is_ingress_only) && (!is_replace))
    {
        flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_WITH_ID;
        SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                        (unit, vlan_port->vlan_port_id, BCM_GPORT_TYPE_FLOW_LIF,
                         DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR, &flow_handle_info.flow_id));
    }

    if (is_native && (!is_replace))
    {
        flow_handle_info.flags |= BCM_FLOW_HANDLE_NATIVE;

        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION))
        {
            flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_VIRTUAL;
        }
    }

    SHR_IF_ERR_EXIT(dnx_vlan_port_flow_terminator_common_fields_set(unit, &vlan_port_flow_term, vlan_port));
    SHR_IF_ERR_EXIT(dnx_vlan_port_flow_terminator_special_fields_set
                    (unit, flow_handle, &vlan_port_flow_term, &special_fields, vlan_port));
    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_create
                    (unit, &flow_handle_info, &vlan_port_flow_term, &special_fields));

    /** Rest flags are irrelevant for match */
    flow_handle_info.flags &= BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID;
    SHR_IF_ERR_EXIT(dnx_vlan_port_terminator_flow_match_action
                    (unit, &flow_handle_info, vlan_port, FLOW_MATCH_ACTION_ADD));

    /** Get Local inLIF ID */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, flow_handle_info.flow_id,
                     DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources));

    /** Encode vlan_port_id using flow_id */
    if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_WITH_ID))
    {
        /*
         * For symmetric case, vlan_port_id is updated by dnx_vlan_port_initiator_flow_create.
         * Here, only handle ingress-only
         */
        if (is_ingress_only)
        {
            if (is_native)
            {
                if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION))
                {
                    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                                    (unit, flow_handle_info.flow_id, _SHR_GPORT_TYPE_VLAN_PORT,
                                     DNX_ALGO_GPM_CONVERSION_FLAG_NATIVE, &vlan_port->vlan_port_id));
                }
                else
                {
                    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                                    (unit, flow_handle_info.flow_id, _SHR_GPORT_TYPE_VLAN_PORT,
                                     DNX_ALGO_GPM_CONVERSION_FLAG_INGRESS_LIF, &vlan_port->vlan_port_id));
                }
            }
            else
            {
                SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                                (unit, flow_handle_info.flow_id, _SHR_GPORT_TYPE_VLAN_PORT,
                                 DNX_ALGO_GPM_CONVERSION_FLAG_INGRESS_LIF, &vlan_port->vlan_port_id));
            }

            vlan_port->encap_id = 0;
        }

    }

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

    bcm_flow_initiator_info_t vlan_port_flow_init;
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_special_fields_t special_fields;
    bcm_flow_special_field_t special_field_data;

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_initiator_info_t_init(&vlan_port_flow_init);
    bcm_flow_special_fields_t_init(&special_fields);

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_LL_INITIATOR, &flow_handle));
    flow_handle_info->flow_handle = flow_handle;

    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, vlan_port->vlan_port_id, BCM_GPORT_TYPE_FLOW_LIF,
                     DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_INITIATOR, &flow_handle_info->flow_id));

    SHR_IF_ERR_EXIT(bcm_dnx_flow_initiator_info_get(unit, flow_handle_info, &vlan_port_flow_init, &special_fields));

    if (vlan_port_flow_init.valid_elements_set & BCM_FLOW_INITIATOR_ELEMENT_STAT_ID_VALID)
    {
        vlan_port->flags |= BCM_VLAN_PORT_STAT_EGRESS_ENABLE;
    }

    if ((dnx_data_multicast.params.feature_get(unit, dnx_data_multicast_params_mc_pruning_actions_not_supported)) &&
        (vlan_port_flow_init.valid_elements_set & BCM_FLOW_INITIATOR_ELEMENT_FAILOVER_ID_VALID))
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

    if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_NATIVE))
    {

        vlan_port->flags |= BCM_VLAN_PORT_NATIVE;

        sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
        SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_special_field_data_get
                                  (unit, &special_fields, FLOW_S_F_NEXT_OUTLIF_POINTER, &special_field_data),
                                  _SHR_E_NOT_FOUND);

        if (special_field_data.shr_var_uint32 != 0)
        {
            bcm_gport_t flow_id;
            dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

            flow_id = special_field_data.shr_var_uint32;

            /*
             * Convert flow_id to legacy gport
             */
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, flow_id,
                                                               DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                                                               &gport_hw_resources));

            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit,
                                                        DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                                                        _SHR_CORE_ALL, gport_hw_resources.local_out_lif,
                                                        &vlan_port->tunnel_id));

        }
        else
        {
            vlan_port->tunnel_id = 0;
        }

    }

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
    bcm_flow_special_field_t special_field_data;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_vlan_port_terminator_flow_handle_from_flags_get(unit, vlan_port->flags, &flow_handle));

    flow_handle_info->flow_handle = flow_handle;

    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_get(unit, flow_handle_info, &vlan_port_flow_term, &special_fields));

    vlan_port->vsi = vlan_port_flow_term.vsi;
    vlan_port->ingress_network_group_id = vlan_port_flow_term.l2_ingress_info.ingress_network_group_id;

    vlan_port->group = vlan_port_flow_term.l2_learn_info.flush_group;

    if (vlan_port_flow_term.valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_STAT_ID_VALID)
    {
        vlan_port->flags |= BCM_VLAN_PORT_STAT_INGRESS_ENABLE;
    }

    if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
    {
        bcm_gport_t destination;

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
            uint32 encoded_dest_port, encoded_learn_dest_port;

            SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                            (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, vlan_port->port, &encoded_dest_port));
            SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                            (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, vlan_port_flow_term.l2_learn_info.dest_port,
                             &encoded_learn_dest_port));
            if (encoded_dest_port != encoded_learn_dest_port)
            {
                vlan_port->failover_port_id = destination;
            }
        }

        /** handle large enrties and WIDE flag*/
        if (vlan_port_flow_term.valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_ADDITIONAL_DATA_VALID)
        {
            vlan_port->flags |= BCM_VLAN_PORT_INGRESS_WIDE;
        }
    }

    /** Set the BCM_VLAN_PORT_VSI_BASE_VID flag if applicable */
    sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
    SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_special_field_data_get
                              (unit, &special_fields, FLOW_S_F_VSI_ASSIGNMENT_MODE, &special_field_data),
                              _SHR_E_NOT_FOUND);
    if (special_field_data.symbol == DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_VLAN)
    {
        vlan_port->flags |= BCM_VLAN_PORT_VSI_BASE_VID;
    }
    if (special_field_data.symbol == DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LOOKUP)
    {
        vlan_port->flags |= BCM_VLAN_PORT_RECYCLE;
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
    bcm_gport_t vlan_port_id)
{

    bcm_flow_handle_info_t flow_handle_info = { 0 };
    bcm_flow_handle_t flow_handle = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, vlan_port_id, BCM_GPORT_TYPE_FLOW_LIF,
                     DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_INITIATOR, &flow_handle_info.flow_id));

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_LL_INITIATOR, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;

    /** Clear ALGO symmetric indication because we are performing one-side deallocation */
    if (BCM_GPORT_SUB_TYPE_LIF_EXC_GET(vlan_port_id) == 0)
    {
        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_update_symmetric_indication
                        (unit, _SHR_GPORT_FLOW_LIF_VAL_GET_ID(flow_handle_info.flow_id), FALSE));
    }

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
                    (unit, vlan_port->vlan_port_id, BCM_GPORT_TYPE_FLOW_LIF,
                     DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR, &flow_handle_info.flow_id));

    SHR_IF_ERR_EXIT(dnx_vlan_port_terminator_flow_match_action
                    (unit, &flow_handle_info, vlan_port, FLOW_MATCH_ACTION_DELETE));

    SHR_IF_ERR_EXIT(dnx_vlan_port_terminator_flow_handle_from_gport_get(unit, vlan_port->vlan_port_id, &flow_handle));

    flow_handle_info.flow_handle = flow_handle;

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, flow_handle_info.flow_id,
                     DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS |
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
         * Verify that the new VLAN-Port configuration is same as old VLAN-Port configuration
         */
        SHR_INVOKE_VERIFY_DNXC(dnx_vlan_port_create_replace_new_old_compare_verify(unit, vlan_port, &old_vlan_port));
    }

    if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_INGRESS_ONLY))
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_initiator_flow_create(unit, vlan_port, &old_vlan_port));
    }

    if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY))
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_terminator_flow_create(unit, vlan_port, &old_vlan_port));
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
        uint32 conversion_flags = (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE)) ?
            DNX_ALGO_GPM_CONVERSION_FLAG_NATIVE : DNX_ALGO_GPM_CONVERSION_FLAG_INGRESS_LIF;

        /** Find by vlan_port criteria etc*/
        SHR_IF_ERR_EXIT(dnx_vlan_port_terminator_flow_match_action
                        (unit, &flow_handle_info, vlan_port, FLOW_MATCH_ACTION_GET));

        /** Convert VLAN_PORT gport to Ingress FLOW LIF gport */
        SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                        (unit, flow_handle_info.flow_id, _SHR_GPORT_TYPE_VLAN_PORT, conversion_flags,
                         &vlan_port->vlan_port_id));
    }

    if ((BCM_GPORT_SUB_TYPE_LIF_EXC_GET(vlan_port->vlan_port_id) == BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY) ||
        BCM_GPORT_SUB_TYPE_IS_VIRTUAL_INGRESS_NATIVE(vlan_port->vlan_port_id) ||
        BCM_GPORT_SUB_TYPE_IS_VIRTUAL_INGRESS_NON_NATIVE(vlan_port->vlan_port_id))
    {
        /** Convert VLAN_PORT gport to Ingress FLOW LIF gport */
        SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                        (unit, vlan_port->vlan_port_id, BCM_GPORT_TYPE_FLOW_LIF,
                         DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR, &flow_handle_info.flow_id));

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, flow_handle_info.flow_id,
                         DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS |
                         DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT, &gport_hw_resources));

        SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_lif_match_info_get(unit, gport_hw_resources.inlif_dbal_table_id,
                                                                        gport_hw_resources.local_in_lif, vlan_port));
        vlan_port->flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
        if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_INGRESS_NATIVE(vlan_port->vlan_port_id))
        {
            vlan_port->flags |= (BCM_VLAN_PORT_NATIVE | BCM_VLAN_PORT_VLAN_TRANSLATION);
        }
        else if (gport_hw_resources.inlif_dbal_table_id == DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION)
        {
            vlan_port->flags |= BCM_VLAN_PORT_NATIVE;
        }
        vlan_port->encap_id = 0;
        SHR_IF_ERR_EXIT(dnx_vlan_port_terminator_flow_get(unit, vlan_port, &flow_handle_info));
    }
    else
    {
        if ((BCM_GPORT_SUB_TYPE_LIF_EXC_GET(vlan_port->vlan_port_id) == BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY))
        {
            /** Convert VLAN_PORT gport to Egress FLOW LIF gport */
            SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                            (unit, vlan_port->vlan_port_id, BCM_GPORT_TYPE_FLOW_LIF,
                             DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_INITIATOR, &flow_handle_info.flow_id));

            vlan_port->flags |= BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
            vlan_port->criteria = BCM_VLAN_PORT_MATCH_NONE;
            SHR_IF_ERR_EXIT(dnx_vlan_port_initiator_flow_get(unit, vlan_port, &flow_handle_info));
        }
        else
        {
            /** Convert VLAN_PORT gport to Ingress FLOW LIF gport */
            SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                            (unit, vlan_port->vlan_port_id, BCM_GPORT_TYPE_FLOW_LIF,
                             DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR, &flow_handle_info.flow_id));

            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                            (unit, flow_handle_info.flow_id,
                             DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS |
                             DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT, &gport_hw_resources));

            SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_lif_match_info_get(unit, DBAL_TABLE_IN_AC_INFO_DB,
                                                                            gport_hw_resources.local_in_lif,
                                                                            vlan_port));
            SHR_IF_ERR_EXIT(dnx_vlan_port_terminator_flow_get(unit, vlan_port, &flow_handle_info));

            /** Convert VLAN_PORT gport to Egress FLOW LIF gport */
            SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                            (unit, vlan_port->vlan_port_id, BCM_GPORT_TYPE_FLOW_LIF,
                             DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_INITIATOR, &flow_handle_info.flow_id));
            SHR_IF_ERR_EXIT(dnx_vlan_port_initiator_flow_get(unit, vlan_port, &flow_handle_info));
        }

        /** Set the encap_id field according to the flow_id */
        vlan_port->encap_id = _SHR_GPORT_FLOW_LIF_VAL_GET_ID(flow_handle_info.flow_id);
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
    int is_virtual_ingress_native;
    int is_ingress_only_non_native;

    SHR_FUNC_INIT_VARS(unit);

    is_virtual_ingress_native = BCM_GPORT_SUB_TYPE_IS_VIRTUAL_INGRESS_NATIVE(gport);
    is_ingress_only_non_native = BCM_GPORT_SUB_TYPE_IS_VIRTUAL_INGRESS_NON_NATIVE(gport);

    if ((BCM_GPORT_SUB_TYPE_LIF_EXC_GET(gport) == BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY) ||
        (BCM_GPORT_SUB_TYPE_LIF_EXC_GET(gport) == 0))
    {
        /** Symmetric or Egress-only: */
        SHR_IF_ERR_EXIT(dnx_vlan_port_initiator_flow_delete(unit, gport));
    }

    if ((BCM_GPORT_SUB_TYPE_LIF_EXC_GET(gport) == BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY) ||
        (BCM_GPORT_SUB_TYPE_LIF_EXC_GET(gport) == 0) || is_virtual_ingress_native || is_ingress_only_non_native)
    {
        bcm_vlan_port_t vlan_port;
        bcm_gport_t flow_gport;
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.vlan_port_id = gport;

        SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                        (unit, vlan_port.vlan_port_id, BCM_GPORT_TYPE_FLOW_LIF,
                         DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR, &flow_gport));

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, flow_gport,
                         DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS |
                         DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT, &gport_hw_resources));

        if (is_ingress_only_non_native)
        {
            vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
        }
        else if (BCM_GPORT_SUB_TYPE_LIF_EXC_GET(gport) == BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY)
        {
            vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_NATIVE;

        }
        else if (is_virtual_ingress_native)
        {
            vlan_port.flags =
                (BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_NATIVE | BCM_VLAN_PORT_VLAN_TRANSLATION);
        }
        else
        {
            /*
             * For symmetric, it can be native or non-native
             */
            if (gport_hw_resources.inlif_dbal_table_id == DBAL_TABLE_IN_AC_INFO_DB)
            {
                vlan_port.flags = 0;
            }
            else if (gport_hw_resources.inlif_dbal_table_id == DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION)
            {
                vlan_port.flags = BCM_VLAN_PORT_NATIVE;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error! gport = 0x%08X but it's IN-LIF table = %d is wrong! \n", gport,
                             gport_hw_resources.inlif_dbal_table_id);
            }
        }

        SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_lif_match_info_get(unit, gport_hw_resources.inlif_dbal_table_id,
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

    SHR_IF_ERR_EXIT(dnx_vlan_port_terminator_flow_handle_from_gport_get
                    (unit, vlan_port_translation->gport, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;

    /** Encode VLAN-PORT gport back to FLOW gport */
    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, vlan_port_translation->gport, BCM_GPORT_TYPE_FLOW_LIF,
                     DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR, &flow_handle_info.flow_id));

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

    SHR_IF_ERR_EXIT(dnx_vlan_port_terminator_flow_handle_from_gport_get
                    (unit, vlan_port_translation->gport, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;

    /** Encode VLAN-PORT gport back to FLOW gport */
    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, vlan_port_translation->gport, BCM_GPORT_TYPE_FLOW_LIF,
                     DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR, &flow_handle_info.flow_id));

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

/**
 * Sets EGRESS ESEM-Default EVE VLAN Transation parameters
 * Note:
 * ESEM-Default profiles EVE and Native EVE are not handled by FLOW thus
 * need to use direct DBAL access.
 */
static shr_error_e
dnx_vlan_port_flow_translation_set_esem_default_eve(
    int unit,
    bcm_vlan_port_translation_t * vlan_port_translation)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Allocate handle and set its keys
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE, &entry_handle_id));

    /** Set values */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_1, INST_SINGLE,
                                 vlan_port_translation->new_outer_vlan);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, INST_SINGLE,
                                 vlan_port_translation->vlan_edit_class_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_2, INST_SINGLE,
                                 vlan_port_translation->new_inner_vlan);
    /** commit */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * Gets EGRESS ESEM-Default EVE VLAN Transation parameters
 * Note:
 * ESEM-Default profiles EVE and Native EVE are not handled by FLOW thus
 * need to use direct DBAL access.
 */
static shr_error_e
dnx_vlan_port_flow_translation_get_esem_default_eve(
    int unit,
    bcm_vlan_port_translation_t * vlan_port_translation)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Allocate handle and set its keys
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE, &entry_handle_id));

    /** Get values */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, INST_SINGLE,
                     &vlan_port_translation->vlan_edit_class_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_1, INST_SINGLE,
                     &vlan_port_translation->new_outer_vlan));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_2, INST_SINGLE,
                     &vlan_port_translation->new_inner_vlan));

exit:
    DBAL_FUNC_FREE_VARS;
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

    if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(vlan_port_translation->gport))
    {
        uint32 default_result_profile = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_GET(vlan_port_translation->gport);
        uint32 default_native_profile = dnx_data_esem.default_result_profile.default_native_get(unit);
        uint32 default_ac_profile = dnx_data_esem.default_result_profile.default_ac_get(unit);
        if ((default_result_profile == default_native_profile) || (default_result_profile == default_ac_profile))
        {
            SHR_IF_ERR_EXIT(dnx_vlan_port_flow_translation_set_esem_default_eve(unit, vlan_port_translation));
            SHR_EXIT();
        }
    }

    bcm_flow_initiator_info_t_init(&vlan_port_flow_init);
    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_special_fields_t_init(&special_fields);

    if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(vlan_port_translation->gport))
    {
        bcm_flow_handle_t namespace_vsi_handle = 0;

        SHR_IF_ERR_EXIT(dnx_esem_initiator_flow_handle_from_gport_get
                        (unit, vlan_port_translation->gport, &flow_handle));

        /** ESEM Namesapce-VSI has only one VLAN */
        SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get
                        (unit, FLOW_APP_NAME_VLAN_PORT_ESEM_NAMESPACE_VSI_INITIATOR, &namespace_vsi_handle));

        if (flow_handle == namespace_vsi_handle)
        {
            special_fields.actual_nof_special_fields = 2;
        }
        else
        {
            special_fields.actual_nof_special_fields = 3;
        }

    }
    else if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(vlan_port_translation->gport))
    {
        special_fields.actual_nof_special_fields = 3;
        SHR_IF_ERR_EXIT(dnx_esem_initiator_flow_handle_from_gport_get
                        (unit, vlan_port_translation->gport, &flow_handle));
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
                        (unit, vlan_port_translation->gport, BCM_GPORT_TYPE_FLOW_LIF,
                         DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_INITIATOR, &gport));

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
                    (unit, vlan_port_translation->gport, BCM_GPORT_TYPE_FLOW_LIF,
                     DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_INITIATOR, &flow_handle_info.flow_id));

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

    if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(vlan_port_translation->gport))
    {
        uint32 default_result_profile = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_GET(vlan_port_translation->gport);
        uint32 default_native_profile = dnx_data_esem.default_result_profile.default_native_get(unit);
        uint32 default_ac_profile = dnx_data_esem.default_result_profile.default_ac_get(unit);
        if ((default_result_profile == default_native_profile) || (default_result_profile == default_ac_profile))
        {
            SHR_IF_ERR_EXIT(dnx_vlan_port_flow_translation_get_esem_default_eve(unit, vlan_port_translation));
            SHR_EXIT();
        }
    }

    bcm_flow_initiator_info_t_init(&vlan_port_flow_init);
    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_special_fields_t_init(&special_fields);

    if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(vlan_port_translation->gport))

    {
        SHR_IF_ERR_EXIT(dnx_esem_initiator_flow_handle_from_gport_get
                        (unit, vlan_port_translation->gport, &flow_handle));
    }
    else if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(vlan_port_translation->gport))
    {
        SHR_IF_ERR_EXIT(dnx_esem_initiator_flow_handle_from_gport_get
                        (unit, vlan_port_translation->gport, &flow_handle));
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
                    (unit, vlan_port_translation->gport, BCM_GPORT_TYPE_FLOW_LIF,
                     DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_INITIATOR, &flow_handle_info.flow_id));

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

static shr_error_e
dnx_vlan_port_flow_traverse_ingress_non_native_cb(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields,
    void *data)
{
    dnx_vlan_port_flow_ingress_user_data_t *flow_user_data = (dnx_vlan_port_flow_ingress_user_data_t *) data;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 gpm_flags;
    int local_in_lif;
    uint32 call_cb;
    uint8 lif_id_is_symmetric;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    call_cb = FALSE;

    /*
     * Get local In-LIF value
     */

    gpm_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT | DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, flow_handle_info->flow_id, gpm_flags, &gport_hw_resources));

    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_lif_id_is_symmetric
                    (unit, _SHR_GPORT_FLOW_LIF_VAL_GET_ID(flow_handle_info->flow_id), &lif_id_is_symmetric));

    local_in_lif = gport_hw_resources.local_in_lif;

    if ((local_in_lif != flow_user_data->drop_in_lif) &&
        (local_in_lif != flow_user_data->initial_bridge_in_lif) &&
        (local_in_lif != flow_user_data->recycle_default_inlif_mp) &&
        ((dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_p2p_or_mp_based_on_cs_in_lif_profile)) &&
         (local_in_lif != flow_user_data->recycle_default_inlif_p2p)))
    {
        if (flow_user_data->is_symmetric_traverse == FALSE)
        {
            /*
             * Traverse ingress-only entries 
             * Skip symmetric LIF:
             */
            if (lif_id_is_symmetric == 0)
            {
                call_cb = TRUE;
            }
        }
        else
        {
            /*
             * Traverse both ingress-only and symmetric
             */
            call_cb = TRUE;
        }

    }

    if (call_cb == TRUE)
    {
        bcm_vlan_port_t vlan_port;

        bcm_vlan_port_t_init(&vlan_port);

        /** Encode VLAN-PORT gport back from FLOW gport */
        SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                        (unit, flow_handle_info->flow_id, BCM_GPORT_VLAN_PORT,
                         DNX_ALGO_GPM_CONVERSION_FLAG_INGRESS_LIF, &vlan_port.vlan_port_id));

        /*
         * Get vlan_port struct
         */
        SHR_IF_ERR_EXIT(bcm_vlan_port_find(unit, &vlan_port));

        /*
         * Run callback function
         */
        SHR_IF_ERR_EXIT(flow_user_data->user_cb(unit, &vlan_port, flow_user_data->user_data));

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vlan_port_flow_traverse_ingress_non_native(
    int unit,
    int is_symmetric_traverse,
    bcm_vlan_port_traverse_cb trav_fn,
    void *user_data)
{
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_handle_info_t flow_handle_info;
    dnx_vlan_port_flow_ingress_user_data_t flow_user_data;

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_handle_info_t_init(&flow_handle_info);

    flow_user_data.user_data = user_data;
    flow_user_data.user_cb = trav_fn;
    flow_user_data.is_symmetric_traverse = is_symmetric_traverse;

    /*
     * The entries (that is all in-lifs) that are not created by default (see dnx_vlan_port_default_init):
     *    - drop LIF
     *    - bridge LIF
     *    - recycle LIF
     * Should be skipped!
     */
    flow_user_data.drop_in_lif = dnx_data_lif.in_lif.drop_in_lif_get(unit);
    SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_initial_bridge_lif_get(unit, (int *) &flow_user_data.initial_bridge_in_lif));
    SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_default_recyle_lif_get
                    (unit, FALSE, &flow_user_data.recycle_default_inlif_mp));
    if (dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_p2p_or_mp_based_on_cs_in_lif_profile))
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_default_recyle_lif_get
                        (unit, TRUE, &flow_user_data.recycle_default_inlif_p2p));
    }
    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;

    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_traverse
                    (unit, &flow_handle_info, dnx_vlan_port_flow_traverse_ingress_non_native_cb, &flow_user_data));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vlan_port_flow_traverse_ingress_native_cb(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields,
    void *data)
{
    dnx_vlan_port_flow_ingress_user_data_t *flow_user_data = (dnx_vlan_port_flow_ingress_user_data_t *) data;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 gpm_flags;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * See dnx_vlan_eth_native_service_tagged_miss_init -
     * Need to skip vtt4_default_trap_in_lif
     */

    /*
     * Get local In-LIF value
     */
    gpm_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT | DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, flow_handle_info->flow_id, gpm_flags, &gport_hw_resources));

    if (gport_hw_resources.local_in_lif != flow_user_data->vtt4_default_trap_in_lif)
    {
        bcm_vlan_port_t vlan_port;

        bcm_vlan_port_t_init(&vlan_port);

        /** Encode VLAN-PORT gport back from FLOW gport */
        SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                        (unit, flow_handle_info->flow_id, BCM_GPORT_VLAN_PORT,
                         DNX_ALGO_GPM_CONVERSION_FLAG_INGRESS_LIF, &vlan_port.vlan_port_id));

        /*
         * Get vlan_port struct
         */
        SHR_IF_ERR_EXIT(bcm_vlan_port_find(unit, &vlan_port));

        /*
         * Run callback function
         */
        SHR_IF_ERR_EXIT(flow_user_data->user_cb(unit, &vlan_port, flow_user_data->user_data));

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vlan_port_flow_traverse_ingress_native(
    int unit,
    bcm_vlan_port_traverse_cb trav_fn,
    void *user_data)
{
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_handle_info_t flow_handle_info;
    dnx_vlan_port_flow_ingress_user_data_t flow_user_data;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    bcm_flow_handle_info_t_init(&flow_handle_info);

    flow_user_data.user_data = user_data;
    flow_user_data.user_cb = trav_fn;

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_AC_NATIVE_TERMINATOR, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;

    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_traverse
                    (unit, &flow_handle_info, dnx_vlan_port_flow_traverse_ingress_native_cb, &flow_user_data));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vlan_port_flow_traverse_ingress_native_virtual_cb(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields,
    void *data)
{
    dnx_vlan_port_flow_ingress_user_data_t *flow_user_data = (dnx_vlan_port_flow_ingress_user_data_t *) data;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 gpm_flags;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * See dnx_vlan_eth_native_no_tags_default_init -
     * Need to skip vtt4_default_0_tag_in_lif
     */

    /*
     * Get local In-LIF value
     */
    gpm_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT | DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, flow_handle_info->flow_id, gpm_flags, &gport_hw_resources));

    if (gport_hw_resources.local_in_lif != flow_user_data->vtt4_default_0_tag_in_lif)
    {
        bcm_vlan_port_t vlan_port;

        bcm_vlan_port_t_init(&vlan_port);

        /** Encode VLAN-PORT gport back from FLOW gport */
        SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                        (unit, flow_handle_info->flow_id, BCM_GPORT_VLAN_PORT,
                         DNX_ALGO_GPM_CONVERSION_FLAG_NATIVE, &vlan_port.vlan_port_id));

        /*
         * Get vlan_port struct
         */
        SHR_IF_ERR_EXIT(bcm_vlan_port_find(unit, &vlan_port));

        /*
         * Run callback function
         */
        SHR_IF_ERR_EXIT(flow_user_data->user_cb(unit, &vlan_port, flow_user_data->user_data));

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vlan_port_flow_traverse_ingress_native_virtual(
    int unit,
    bcm_vlan_port_traverse_cb trav_fn,
    void *user_data)
{
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_handle_info_t flow_handle_info;
    dnx_vlan_port_flow_ingress_user_data_t flow_user_data;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    bcm_flow_handle_info_t_init(&flow_handle_info);

    flow_user_data.user_data = user_data;
    flow_user_data.user_cb = trav_fn;

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_AC_NATIVE_VIRTUAL_TERMINATOR, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;

    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_traverse
                    (unit, &flow_handle_info, dnx_vlan_port_flow_traverse_ingress_native_virtual_cb, &flow_user_data));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vlan_port_flow_traverse_ingress(
    int unit,
    uint32 flags,
    bcm_vlan_port_traverse_cb trav_fn,
    void *user_data)
{
    int is_traverse_all, is_native, is_virtual;

    SHR_FUNC_INIT_VARS(unit);

    is_traverse_all = (flags == 0) ? TRUE : FALSE;
    is_native = _SHR_IS_FLAG_SET(flags, BCM_VLAN_PORT_NATIVE);
    is_virtual = _SHR_IS_FLAG_SET(flags, BCM_VLAN_PORT_VLAN_TRANSLATION);

    /*
     * Traverse Ingress non-native objects
     *      Note: symmetric LIFs will be skipped based on "is_symmetric_traverse" indication.
     */
    if ((is_traverse_all == TRUE) || (is_native == FALSE))
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_flow_traverse_ingress_non_native(unit, is_traverse_all, trav_fn, user_data));
    }

    /*
     * Traverse Ingress native non-virtual objects
     */
    if ((is_traverse_all == TRUE) || (is_native == TRUE && is_virtual == FALSE))
    {

        SHR_IF_ERR_EXIT(dnx_vlan_port_flow_traverse_ingress_native(unit, trav_fn, user_data));
    }

    /*
     * Traverse Ingress native virtual objects
     */
    if ((is_traverse_all == TRUE) || (is_native == TRUE && is_virtual == TRUE))
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_flow_traverse_ingress_native_virtual(unit, trav_fn, user_data));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vlan_port_flow_traverse_egress_non_virtual_cb(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields,
    void *data)
{
    uint8 lif_id_is_symmetric;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_lif_id_is_symmetric
                    (unit, _SHR_GPORT_FLOW_LIF_VAL_GET_ID(flow_handle_info->flow_id), &lif_id_is_symmetric));

    /*
     * Traverse Egress-only entires
     * Note:
     * Symmetric LIF callback is called by ingress traverse.
     */
    if (lif_id_is_symmetric == 0)
    {
        dnx_vlan_port_flow_egress_user_data_t *flow_user_data = (dnx_vlan_port_flow_egress_user_data_t *) data;
        bcm_vlan_port_t vlan_port;

        bcm_vlan_port_t_init(&vlan_port);

        /** Encode VLAN-PORT gport back from FLOW LIF gport */
        SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                        (unit, flow_handle_info->flow_id, BCM_GPORT_VLAN_PORT, DNX_ALGO_GPM_CONVERSION_FLAG_NONE,
                         &vlan_port.vlan_port_id));

        /*
         * Get vlan_port struct
         */
        SHR_IF_ERR_EXIT(bcm_vlan_port_find(unit, &vlan_port));

        /*
         * Run callback function
         */
        SHR_IF_ERR_EXIT(flow_user_data->user_cb(unit, &vlan_port, flow_user_data->user_data));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vlan_port_flow_traverse_egress_esem_cb(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields,
    void *data)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 gpm_flags;
    dnx_vlan_port_flow_egress_user_data_t *flow_user_data = (dnx_vlan_port_flow_egress_user_data_t *) data;
    bcm_vlan_port_t vlan_port;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get flow-id HW resources
     */
    gpm_flags =
        DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT | DNX_ALGO_GPM_GPORT_HW_RESOURCES_EGRESS_LOCAL_LIF_OR_VIRTUAL;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, flow_handle_info->flow_id, gpm_flags, &gport_hw_resources));

    bcm_vlan_port_t_init(&vlan_port);

    /** convert the flow gport VLAN-Port gport */
    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, flow_handle_info->flow_id, BCM_GPORT_VLAN_PORT, DNX_ALGO_GPM_CONVERSION_FLAG_NONE,
                     &vlan_port.vlan_port_id));

    /*
     * Get vlan_port struct
     */
    SHR_IF_ERR_EXIT(bcm_vlan_port_find(unit, &vlan_port));

    /*
     * Run callback function
     */
    SHR_IF_ERR_EXIT(flow_user_data->user_cb(unit, &vlan_port, flow_user_data->user_data));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vlan_port_flow_traverse_egress_esem_default_cb(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields,
    void *data)
{
    return _SHR_E_UNAVAIL;
}

shr_error_e
dnx_vlan_port_flow_traverse_egress_app(
    int unit,
    char *app_name,
    bcm_flow_initiator_info_traverse_cb cb,
    bcm_vlan_port_traverse_cb trav_fn,
    void *user_data)
{
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_handle_info_t flow_handle_info;
    dnx_vlan_port_flow_egress_user_data_t flow_user_data;

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_handle_info_t_init(&flow_handle_info);

    flow_user_data.user_data = user_data;
    flow_user_data.user_cb = trav_fn;

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, app_name, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;

    SHR_IF_ERR_EXIT(bcm_dnx_flow_initiator_info_traverse(unit, &flow_handle_info, cb, &flow_user_data));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vlan_port_flow_traverse_egress(
    int unit,
    uint32 flags,
    bcm_vlan_port_traverse_cb trav_fn,
    void *user_data)
{
    int is_traverse_all, is_virtual, is_default;

    SHR_FUNC_INIT_VARS(unit);

    is_traverse_all = (flags == 0) ? TRUE : FALSE;
    is_virtual = _SHR_IS_FLAG_SET(flags, BCM_VLAN_PORT_VLAN_TRANSLATION);
    is_default = _SHR_IS_FLAG_SET(flags, BCM_VLAN_PORT_DEFAULT);

    /*
     * Traverse Egress non virtual (EEDB) objects only
     *      Note: symmetric LIF objects will be skipped.
     */
    if ((is_traverse_all == TRUE) || (is_virtual == FALSE))
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_flow_traverse_egress_app
                        (unit, FLOW_APP_NAME_VLAN_PORT_LL_INITIATOR, dnx_vlan_port_flow_traverse_egress_non_virtual_cb,
                         trav_fn, user_data));
    }

    /*
     * Traverse Egress Virtual (ESEM) objects only
     */
    if ((is_traverse_all == TRUE) || (is_virtual == TRUE && is_default == FALSE))
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_flow_traverse_egress_app
                        (unit, FLOW_APP_NAME_VLAN_PORT_ESEM_NW_SCOPED_INITIATOR,
                         dnx_vlan_port_flow_traverse_egress_esem_cb, trav_fn, user_data));

        SHR_IF_ERR_EXIT(dnx_vlan_port_flow_traverse_egress_app
                        (unit, FLOW_APP_NAME_VLAN_PORT_ESEM_LIF_SCOPED_INITIATOR,
                         dnx_vlan_port_flow_traverse_egress_esem_cb, trav_fn, user_data));

        SHR_IF_ERR_EXIT(dnx_vlan_port_flow_traverse_egress_app
                        (unit, FLOW_APP_NAME_VLAN_PORT_ESEM_NAMESPACE_VSI_INITIATOR,
                         dnx_vlan_port_flow_traverse_egress_esem_cb, trav_fn, user_data));

        SHR_IF_ERR_EXIT(dnx_vlan_port_flow_traverse_egress_app
                        (unit, FLOW_APP_NAME_VLAN_PORT_ESEM_NAMESPACE_PORT_INITIATOR,
                         dnx_vlan_port_flow_traverse_egress_esem_cb, trav_fn, user_data));
    }

    /*
     * Traverse Egress Virtual default (ESEM-default) objects only
     */
    if ((is_traverse_all == TRUE) || (is_virtual == TRUE && is_default == TRUE))
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_flow_traverse_egress_app
                        (unit, FLOW_APP_NAME_VLAN_PORT_ESEM_DEFAULT_INITIATOR,
                         dnx_vlan_port_flow_traverse_egress_esem_default_cb, trav_fn, user_data));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in .h file
 */
shr_error_e
dnx_vlan_port_flow_traverse(
    int unit,
    bcm_vlan_port_traverse_info_t * additional_info,
    bcm_vlan_port_traverse_cb trav_fn,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(additional_info->flags, BCM_VLAN_PORT_CREATE_INGRESS_ONLY))
    {
        /*
         * Traverse only Ingress objects
         */
        SHR_IF_ERR_EXIT(dnx_vlan_port_flow_traverse_ingress(unit, additional_info->flags, trav_fn, user_data));
    }
    else if (_SHR_IS_FLAG_SET(additional_info->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY))
    {
        /*
         * Traverse only Egress objects
         */
        SHR_IF_ERR_EXIT(dnx_vlan_port_flow_traverse_egress(unit, additional_info->flags, trav_fn, user_data));
    }
    else
    {
        /*
         * Traverse ALL objects (Ingress and Egress)
         */
        SHR_IF_ERR_EXIT(dnx_vlan_port_flow_traverse_ingress(unit, additional_info->flags, trav_fn, user_data));
        SHR_IF_ERR_EXIT(dnx_vlan_port_flow_traverse_egress(unit, additional_info->flags, trav_fn, user_data));
    }

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
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    bcm_gport_t flow_gport;

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_terminator_info_t_init(&vlan_port_flow_term);
    bcm_flow_handle_info_t_init(&flow_handle_info);

    SHR_IF_ERR_EXIT(dnx_vlan_port_terminator_flow_handle_from_gport_get(unit, port, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;

    /** Encode VLAN-PORT gport back to FLOW gport */
    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, port, BCM_GPORT_TYPE_FLOW_LIF, DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR,
                     &flow_handle_info.flow_id));

    /** Use  REPLACE to set VSI and learning */
    flow_handle_info.flags = BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID;

    /** Update LIF's VSI*/
    vlan_port_flow_term.valid_elements_set = BCM_FLOW_TERMINATOR_ELEMENT_VSI_VALID;
    vlan_port_flow_term.vsi = vsi;

    /*
     * No learning for native:
     */
    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, port, BCM_GPORT_TYPE_FLOW_LIF, DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR, &flow_gport));

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, flow_gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources));

    if (!((gport_hw_resources.inlif_dbal_table_id == DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION) ||
          ((gport_hw_resources.inlif_dbal_table_id == DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_VLAN_EDIT))))
    {
        vlan_port_flow_term.valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_LEARN_ENABLE_VALID;
        vlan_port_flow_term.learn_enable = learn_enable;
    }

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

    SHR_IF_ERR_EXIT(dnx_vlan_port_terminator_flow_handle_from_gport_get(unit, port, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;

    /** Encode VLAN-PORT gport back to FLOW gport */
    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, port, BCM_GPORT_TYPE_FLOW_LIF, DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR,
                     &flow_handle_info.flow_id));

    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_get(unit, &flow_handle_info, &vlan_port_flow_term, &special_fields));

    *vsi = vlan_port_flow_term.vsi;

exit:
    SHR_FUNC_EXIT;
}
