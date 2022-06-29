/** \file port_pp_flow.c
 *  General port pp functionality using flow lif for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dnx/flow/flow.h>
#include <src/bcm/dnx/flow/flow_def.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/auto_generated/dnx_flow_dispatch.h>
#include <bcm_int/dnx/qos/qos.h>

/*
 * See .h for reference
 */
shr_error_e
dnx_port_class_flow_lif_property_set(
    int unit,
    bcm_gport_t gport_lif,
    bcm_port_class_t pclass,
    uint32 value)
{
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_terminator_info_t flow_term_info;
    bcm_flow_initiator_info_t flow_init_info;
    bcm_flow_special_fields_t special_fields;
    dnx_flow_app_type_e flow_app_type;
    uint32 gport_flag;

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_terminator_info_t_init(&flow_term_info);
    bcm_flow_initiator_info_t_init(&flow_init_info);
    bcm_flow_special_fields_t_init(&special_fields);

    
    if (pclass == bcmPortClassForwardIngress)
    {
        /** Convert the gport to flow id*/
        SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                        (unit, gport_lif, BCM_GPORT_TYPE_FLOW_LIF, DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR,
                         &flow_handle_info.flow_id));

        SHR_IF_ERR_EXIT(flow_lif_flow_app_from_gport_get(unit, flow_handle_info.flow_id,
                                                         FLOW_APP_TYPE_TERM, &flow_handle_info.flow_handle));
        SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_get(unit, &flow_handle_info, &flow_term_info, &special_fields));
        flow_term_info.valid_elements_set = 0;
    }
    else if (pclass == bcmPortClassForwardEgress)
    {
        /** Convert the gport to flow id*/
        SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                        (unit, gport_lif, BCM_GPORT_TYPE_FLOW_LIF, DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_INITIATOR,
                         &flow_handle_info.flow_id));

        SHR_IF_ERR_EXIT(flow_lif_flow_app_from_gport_get(unit, flow_handle_info.flow_id,
                                                         FLOW_APP_TYPE_INIT, &flow_handle_info.flow_handle));
        SHR_IF_ERR_EXIT(bcm_dnx_flow_initiator_info_get(unit, &flow_handle_info, &flow_init_info, &special_fields));
        flow_init_info.valid_elements_set = 0;
    }

    /** Set the info for LIF entry updating.*/
    switch (pclass)
    {
        case bcmPortClassIngress:
            flow_term_info.valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_CLASS_ID_VALID;
            flow_term_info.class_id = value;
            flow_app_type = FLOW_APP_TYPE_TERM;
            break;
        case bcmPortClassEgress:
            flow_init_info.valid_elements_set |= BCM_FLOW_INITIATOR_ELEMENT_CLASS_ID_VALID;
            flow_init_info.class_id = value;
            flow_app_type = FLOW_APP_TYPE_INIT;
            break;
        case bcmPortClassForwardIngress:
            flow_term_info.valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID;
            flow_term_info.l2_ingress_info.ingress_network_group_id = value;
            flow_app_type = FLOW_APP_TYPE_TERM;
            break;
        case bcmPortClassForwardEgress:
            flow_init_info.valid_elements_set |= BCM_FLOW_INITIATOR_ELEMENT_L2_EGRESS_INFO_VALID;
            flow_init_info.l2_egress_info.egress_network_group_id = value;
            flow_init_info.encap_access = bcmEncapAccessInvalid;
            flow_app_type = FLOW_APP_TYPE_INIT;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported port class type (%d)\n", pclass);
    }

    /** Convert the gport to flow id*/
    gport_flag =
        (flow_app_type ==
         FLOW_APP_TYPE_TERM) ? DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR :
        DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_INITIATOR;
    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, gport_lif, BCM_GPORT_TYPE_FLOW_LIF, gport_flag, &flow_handle_info.flow_id));
    SHR_IF_ERR_EXIT(flow_lif_flow_app_from_gport_get
                    (unit, flow_handle_info.flow_id, flow_app_type, &flow_handle_info.flow_handle));

    /** Update the LIF entry.*/
    flow_handle_info.flags = BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID;
    if (flow_app_type == FLOW_APP_TYPE_TERM)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_create(unit, &flow_handle_info, &flow_term_info, &special_fields));
    }
    else
    {
        SHR_IF_ERR_EXIT(bcm_dnx_flow_initiator_info_create(unit, &flow_handle_info, &flow_init_info, &special_fields));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h for reference
 */
shr_error_e
dnx_port_class_flow_lif_property_get(
    int unit,
    bcm_gport_t gport_lif,
    bcm_port_class_t pclass,
    uint32 *value)
{
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_terminator_info_t flow_term_info;
    bcm_flow_initiator_info_t flow_init_info;
    bcm_flow_special_fields_t special_fields;
    dnx_flow_app_type_e flow_app_type;
    uint32 gport_flag;

    void *value_ptr = NULL;
    uint8 value_bytes = 0;
    uint32 common_field_bit_set = 0;
    uint32 common_field_valid_bit = 0;

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_terminator_info_t_init(&flow_term_info);
    bcm_flow_initiator_info_t_init(&flow_init_info);
    bcm_flow_special_fields_t_init(&special_fields);

    switch (pclass)
    {
        case bcmPortClassIngress:
            common_field_valid_bit = BCM_FLOW_TERMINATOR_ELEMENT_CLASS_ID_VALID;
            value_ptr = &(flow_term_info.class_id);
            value_bytes = sizeof(flow_term_info.class_id);
            flow_app_type = FLOW_APP_TYPE_TERM;
            break;
        case bcmPortClassEgress:
            common_field_valid_bit = BCM_FLOW_INITIATOR_ELEMENT_CLASS_ID_VALID;
            value_ptr = &(flow_init_info.class_id);
            value_bytes = sizeof(flow_init_info.class_id);
            flow_app_type = FLOW_APP_TYPE_INIT;
            break;
        case bcmPortClassForwardIngress:
            common_field_valid_bit = BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID;
            value_ptr = &(flow_term_info.l2_ingress_info.ingress_network_group_id);
            value_bytes = sizeof(flow_term_info.l2_ingress_info.ingress_network_group_id);
            flow_app_type = FLOW_APP_TYPE_TERM;
            break;
        case bcmPortClassForwardEgress:
            common_field_valid_bit = BCM_FLOW_INITIATOR_ELEMENT_L2_EGRESS_INFO_VALID;
            value_ptr = &(flow_init_info.l2_egress_info.egress_network_group_id);
            value_bytes = sizeof(flow_init_info.l2_egress_info.egress_network_group_id);
            flow_app_type = FLOW_APP_TYPE_INIT;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported port class type (%d)\n", pclass);
    }

    /** Convert the gport to flow id*/
    gport_flag =
        (flow_app_type ==
         FLOW_APP_TYPE_TERM) ? DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR :
        DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_INITIATOR;
    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, gport_lif, BCM_GPORT_TYPE_FLOW_LIF, gport_flag, &flow_handle_info.flow_id));
    SHR_IF_ERR_EXIT(flow_lif_flow_app_from_gport_get
                    (unit, flow_handle_info.flow_id, flow_app_type, &flow_handle_info.flow_handle));

    /** Get the object.*/
    if (flow_app_type == FLOW_APP_TYPE_TERM)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_get(unit, &flow_handle_info, &flow_term_info, &special_fields));
        common_field_bit_set = flow_term_info.valid_elements_set;
    }
    else
    {
        SHR_IF_ERR_EXIT(bcm_dnx_flow_initiator_info_get(unit, &flow_handle_info, &flow_init_info, &special_fields));
        common_field_bit_set = flow_init_info.valid_elements_set;
    }

    if (_SHR_IS_FLAG_SET(common_field_bit_set, common_field_valid_bit) && value_ptr)
    {
        sal_memcpy(value, value_ptr, value_bytes);
    }
    else
    {
        *value = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See h. for reference
 */
shr_error_e
dnx_port_control_flow_lif_property_set(
    int unit,
    bcm_gport_t gport_lif,
    bcm_port_control_t port_control_type,
    int value)
{
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_terminator_info_t flow_term_info, flow_term_info_get;
    dnx_flow_app_type_e flow_app_type = FLOW_APP_TYPE_TERM;
    bcm_flow_special_fields_t special_fields;

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_terminator_info_t_init(&flow_term_info);
    bcm_flow_terminator_info_t_init(&flow_term_info_get);
    bcm_flow_special_fields_t_init(&special_fields);

    /**Get the already allocated terminator and store the other L2 ingress property fields*/
    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, gport_lif, BCM_GPORT_TYPE_FLOW_LIF, DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR,
                     &flow_handle_info.flow_id));
    SHR_IF_ERR_EXIT(flow_lif_flow_app_from_gport_get
                    (unit, flow_handle_info.flow_id, flow_app_type, &flow_handle_info.flow_handle));

    /**Get the already allocated terminator and store the other L2 ingress property fields*/
    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_get(unit, &flow_handle_info, &flow_term_info_get, &special_fields));
    switch (port_control_type)
    {
        case bcmPortControlBridge:
        {
            /** bcmPortControlBridge toggles between device scope and disable*/
            flow_term_info.valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID;
            flow_term_info.l2_ingress_info.same_interface_mode =
                (value == 0) ? bcmFlowSameInterfaceModeDisable : bcmFlowSameInterfaceModeDeviceScope;
            flow_term_info.l2_ingress_info.outer_policer_remark =
                flow_term_info_get.l2_ingress_info.outer_policer_remark;
            flow_term_info.l2_ingress_info.inner_policer_remark =
                flow_term_info_get.l2_ingress_info.inner_policer_remark;
            break;
        }
        case bcmPortControlLogicalInterfaceSameFilter:
        {
            /** bcmPortControlBridge toggles between device scope and system scope*/
            flow_term_info.valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID;
            flow_term_info.l2_ingress_info.same_interface_mode =
                (value == 0) ? bcmFlowSameInterfaceModeDeviceScope : bcmFlowSameInterfaceModeSystemScope;
            flow_term_info.l2_ingress_info.outer_policer_remark =
                flow_term_info_get.l2_ingress_info.outer_policer_remark;
            flow_term_info.l2_ingress_info.inner_policer_remark =
                flow_term_info_get.l2_ingress_info.inner_policer_remark;
            break;
        }
        case bcmPortControlOuterPolicerRemark:
        {
            flow_term_info.valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID;
            flow_term_info.l2_ingress_info.outer_policer_remark = DNX_QOS_MAP_PROFILE_GET(value);
            flow_term_info.l2_ingress_info.inner_policer_remark =
                flow_term_info_get.l2_ingress_info.inner_policer_remark;
            flow_term_info.l2_ingress_info.same_interface_mode = flow_term_info_get.l2_ingress_info.same_interface_mode;
            break;
        }
        case bcmPortControlInnerPolicerRemark:
        {
            flow_term_info.valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID;
            flow_term_info.l2_ingress_info.inner_policer_remark = DNX_QOS_MAP_PROFILE_GET(value);;
            flow_term_info.l2_ingress_info.outer_policer_remark =
                flow_term_info_get.l2_ingress_info.outer_policer_remark;
            flow_term_info.l2_ingress_info.same_interface_mode = flow_term_info_get.l2_ingress_info.same_interface_mode;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported port control type (%d)\n", port_control_type);
        }
    }

    flow_handle_info.flags = BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID;
    flow_term_info.l2_ingress_info.ingress_network_group_id =
        flow_term_info_get.l2_ingress_info.ingress_network_group_id;
    flow_term_info.l2_ingress_info.service_type = flow_term_info_get.l2_ingress_info.service_type;

    /**Replace the flow terminator with only the same interface mode changed*/
    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_create(unit, &flow_handle_info, &flow_term_info, NULL));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See h. for reference
 */
shr_error_e
dnx_port_control_flow_lif_property_get(
    int unit,
    bcm_gport_t gport_lif,
    bcm_port_control_t port_control_type,
    int *value)
{
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_terminator_info_t flow_term_info;
    bcm_flow_special_fields_t special_fields;
    dnx_flow_app_type_e flow_app_type = FLOW_APP_TYPE_TERM;

    void *value_ptr = NULL;
    uint8 value_bytes = 0;
    uint32 common_field_bit_set = 0;
    uint32 common_field_valid_bit = 0;

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_terminator_info_t_init(&flow_term_info);
    bcm_flow_special_fields_t_init(&special_fields);

    switch (port_control_type)
    {
        case bcmPortControlBridge:
        {
            common_field_valid_bit = BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID;
            value_ptr = &(flow_term_info.l2_ingress_info.same_interface_mode);
            value_bytes = sizeof(flow_term_info.l2_ingress_info.same_interface_mode);
            break;
        }
        case bcmPortControlLogicalInterfaceSameFilter:
        {
            common_field_valid_bit = BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID;
            value_ptr = &(flow_term_info.l2_ingress_info.same_interface_mode);
            value_bytes = sizeof(flow_term_info.l2_ingress_info.same_interface_mode);
            break;
        }
        case bcmPortControlOuterPolicerRemark:
        {
            common_field_valid_bit = BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID;
            value_ptr = &(flow_term_info.l2_ingress_info.outer_policer_remark);
            value_bytes = sizeof(flow_term_info.l2_ingress_info.outer_policer_remark);
            break;
        }
        case bcmPortControlInnerPolicerRemark:
        {
            common_field_valid_bit = BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID;
            value_ptr = &(flow_term_info.l2_ingress_info.inner_policer_remark);
            value_bytes = sizeof(flow_term_info.l2_ingress_info.inner_policer_remark);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported port control type (%d)\n", port_control_type);
        }
    }

    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, gport_lif, BCM_GPORT_TYPE_FLOW_LIF, DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR,
                     &flow_handle_info.flow_id));
    SHR_IF_ERR_EXIT(flow_lif_flow_app_from_gport_get
                    (unit, flow_handle_info.flow_id, flow_app_type, &flow_handle_info.flow_handle));
    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_get(unit, &flow_handle_info, &flow_term_info, &special_fields));
    common_field_bit_set = flow_term_info.valid_elements_set;

    if (_SHR_IS_FLAG_SET(common_field_bit_set, common_field_valid_bit) && value_ptr)
    {
        sal_memcpy(value, value_ptr, value_bytes);
        if (port_control_type == bcmPortControlBridge)
        {
            *value = (*value == bcmFlowSameInterfaceModeDeviceScope) ? 1 : 0;
        }
        if (port_control_type == bcmPortControlLogicalInterfaceSameFilter)
        {
            *value = (*value == bcmFlowSameInterfaceModeDeviceScope) ? 0 : 1;
        }
        if ((port_control_type == bcmPortControlInnerPolicerRemark)
            || (port_control_type == bcmPortControlOuterPolicerRemark))
        {
            /** set map policer */
            DNX_QOS_POLICER_MAP_SET(*value);
            /** set map direction*/
            DNX_QOS_INGRESS_MAP_SET(*value);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h for reference
 */
shr_error_e
dnx_port_pp_flow_lif_flood_set(
    int unit,
    bcm_gport_t gport_lif,
    bcm_port_flood_group_t * flood_groups)
{
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_terminator_info_t flow_term_info;
    bcm_flow_special_fields_t special_fields;

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_terminator_info_t_init(&flow_term_info);
    bcm_flow_special_fields_t_init(&special_fields);

    if (flood_groups->unknown_unicast_group != BCM_GPORT_INVALID)
    {
        SPECIAL_FIELD_UINT32_DATA_ADD(&special_fields, FLOW_S_F_UNKNOWN_UNICAST_DEFAULT_DESTINATION,
                                      flood_groups->unknown_unicast_group);
    }

    if (flood_groups->unknown_multicast_group != BCM_GPORT_INVALID)
    {
        SPECIAL_FIELD_UINT32_DATA_ADD(&special_fields, FLOW_S_F_UNKNOWN_MULTICAST_DEFAULT_DESTINATION,
                                      flood_groups->unknown_multicast_group);
    }

    if (flood_groups->broadcast_group != BCM_GPORT_INVALID)
    {
        SPECIAL_FIELD_UINT32_DATA_ADD(&special_fields, FLOW_S_F_BROADCAST_DEFAULT_DESTINATION,
                                      flood_groups->broadcast_group);
    }

    /** Convert the gport to flow id*/
    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert(unit, gport_lif, BCM_GPORT_TYPE_FLOW_LIF,
                                                DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR,
                                                &flow_handle_info.flow_id));

    SHR_IF_ERR_EXIT(flow_lif_flow_app_from_gport_get(unit, flow_handle_info.flow_id,
                                                     FLOW_APP_TYPE_TERM, &flow_handle_info.flow_handle));

    /** Update the LIF object. */
    flow_handle_info.flags = BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID;
    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_create(unit, &flow_handle_info, &flow_term_info, &special_fields));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h for reference
 */
shr_error_e
dnx_port_pp_flow_lif_flood_get(
    int unit,
    bcm_gport_t gport_lif,
    bcm_port_flood_group_t * flood_groups)
{
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_terminator_info_t flow_term_info;
    bcm_flow_special_fields_t special_fields;
    int field_idx, nof_field = 0;
    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_terminator_info_t_init(&flow_term_info);
    bcm_flow_special_fields_t_init(&special_fields);

    /** Convert the gport to flow id*/
    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert(unit, gport_lif, BCM_GPORT_TYPE_FLOW_LIF,
                                                DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR,
                                                &flow_handle_info.flow_id));

    SHR_IF_ERR_EXIT(flow_lif_flow_app_from_gport_get(unit, flow_handle_info.flow_id,
                                                     FLOW_APP_TYPE_TERM, &flow_handle_info.flow_handle));
    /** Get the LIF object. */
    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_get(unit, &flow_handle_info, &flow_term_info, &special_fields));

    /** Get the flood groups.*/
    for (field_idx = 0; field_idx < special_fields.actual_nof_special_fields; field_idx++)
    {
        if (special_fields.special_fields[field_idx].field_id == FLOW_S_F_UNKNOWN_UNICAST_DEFAULT_DESTINATION)
        {
            flood_groups->unknown_unicast_group = special_fields.special_fields[field_idx].shr_var_uint32;
            nof_field++;
        }
        if (special_fields.special_fields[field_idx].field_id == FLOW_S_F_UNKNOWN_MULTICAST_DEFAULT_DESTINATION)
        {
            flood_groups->unknown_multicast_group = special_fields.special_fields[field_idx].shr_var_uint32;
            nof_field++;
        }
        if (special_fields.special_fields[field_idx].field_id == FLOW_S_F_BROADCAST_DEFAULT_DESTINATION)
            nof_field++;
        {
            flood_groups->broadcast_group = special_fields.special_fields[field_idx].shr_var_uint32;
        }

        if (nof_field == (sizeof(bcm_port_flood_group_t) + 1) / sizeof(bcm_gport_t))
        {
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h for reference
 */
shr_error_e
dnx_port_pp_flow_lif_learn_set(
    int unit,
    bcm_gport_t gport,
    uint32 flags)
{
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_special_fields_t special_fields;
    bcm_flow_terminator_info_t terminator_info;
    uint8 learn_enable;

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_special_fields_t_init(&special_fields);
    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_terminator_info_t_init(&terminator_info);

    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, gport, BCM_GPORT_TYPE_FLOW_LIF, DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR,
                     &flow_handle_info.flow_id));
    SHR_IF_ERR_EXIT(flow_lif_flow_app_from_gport_get
                    (unit, flow_handle_info.flow_id, FLOW_APP_TYPE_TERM, &flow_handle_info.flow_handle));
    flow_handle_info.flags = BCM_FLOW_HANDLE_INFO_WITH_ID | BCM_FLOW_HANDLE_INFO_REPLACE;
    learn_enable = _SHR_IS_FLAG_SET(flags, BCM_PORT_LEARN_ARL) ? TRUE : FALSE;

    terminator_info.learn_enable = learn_enable;
    terminator_info.valid_elements_set = BCM_FLOW_TERMINATOR_ELEMENT_LEARN_ENABLE_VALID;

    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_create(unit, &flow_handle_info, &terminator_info, &special_fields));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h for reference
 */
shr_error_e
dnx_port_pp_flow_lif_learn_get(
    int unit,
    bcm_gport_t gport,
    uint32 *flags)
{
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_special_fields_t special_fields;
    bcm_flow_terminator_info_t terminator_info;
    uint8 learn_enable = 0;

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_special_fields_t_init(&special_fields);
    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_terminator_info_t_init(&terminator_info);

    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, gport, BCM_GPORT_TYPE_FLOW_LIF, DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR,
                     &flow_handle_info.flow_id));
    SHR_IF_ERR_EXIT(flow_lif_flow_app_from_gport_get
                    (unit, flow_handle_info.flow_id, FLOW_APP_TYPE_TERM, &flow_handle_info.flow_handle));

    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_get(unit, &flow_handle_info, &terminator_info, &special_fields));

    if (terminator_info.valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_LEARN_ENABLE_VALID)
    {
        learn_enable = terminator_info.learn_enable;
    }

    /*
     * Assign the flags accordingly.
     */
    *flags = BCM_PORT_LEARN_FWD;
    if (learn_enable != FALSE)
    {
        *flags |= BCM_PORT_LEARN_ARL;
    }

exit:
    SHR_FUNC_EXIT;
}
