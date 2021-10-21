/** \file esem_flow.c
 *  General ESEM functionality using flow lif for DNX.
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
#include "esem_flow.h"
#include <soc/dnx/dnx_data/auto_generated/dnx_data_failover.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>
#include <bcm_int/dnx/qos/qos.h>

/**
 * Setting common fields vlan port initiator
 */
static shr_error_e
dnx_esem_flow_initiator_common_fields_set(
    int unit,
    bcm_flow_initiator_info_t * esem_initiator_info,
    bcm_vlan_port_t * vlan_port)
{
    SHR_FUNC_INIT_VARS(unit);

    esem_initiator_info->flags = 0;
    /*
     * Setting all common fields, consistent with legacy API
     */
    
    esem_initiator_info->valid_elements_set = BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID;

    /*
     * BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID | 
     */

    
    if (dnx_data_lif.out_lif.feature_get(unit, dnx_data_lif_out_lif_out_lif_profile_supported))
    {
        esem_initiator_info->valid_elements_set |= BCM_FLOW_INITIATOR_ELEMENT_L2_EGRESS_INFO_VALID;
    }

    esem_initiator_info->l2_egress_info.egress_network_group_id = vlan_port->egress_network_group_id;

    if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_REPLACE))
    {
        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_STAT_EGRESS_ENABLE))
        {
            esem_initiator_info->valid_elements_set |= BCM_FLOW_INITIATOR_ELEMENT_STAT_ID_VALID |
                BCM_FLOW_INITIATOR_ELEMENT_STAT_PP_PROFILE_VALID;
            esem_initiator_info->stat_id = 0;
            esem_initiator_info->stat_pp_profile = 0;
        }
        esem_initiator_info->valid_elements_set |= BCM_FLOW_INITIATOR_ELEMENT_QOS_MAP_ID_VALID;
        esem_initiator_info->qos_map_id = 0;
    }

    esem_initiator_info->l3_intf_id = BCM_IF_INVALID;
    sal_memcpy(&esem_initiator_info->egress_qos_model, &vlan_port->egress_qos_model,
               sizeof(vlan_port->egress_qos_model));

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_esem_initiator_special_fields_set(
    int unit,
    int is_esem_default,
    bcm_flow_handle_t flow_handle,
    bcm_flow_special_fields_t * special_fields,
    bcm_vlan_port_t * vlan_port)
{
    bcm_flow_field_id_t field_id;
    char *field_name;
    uint32 vlan_domain;

    SHR_FUNC_INIT_VARS(unit);

    special_fields->actual_nof_special_fields = 0;

    if (!is_esem_default)
    {
        /** Set ESEM keys */
        special_fields->actual_nof_special_fields = 3;

        /** FODO (VSI) */
        SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "VSI_MATCH", &field_id));
        special_fields->special_fields[0].field_id = field_id;
        special_fields->special_fields[0].shr_var_uint32 = vlan_port->vsi;

        /** VLAN-Domain */
        if (dnx_data_esem.feature.feature_get(unit, dnx_data_esem_feature_esem_is_dpc))
        {
            field_name = "NAME_SPACE_MATCH";
        }
        else
        {
            field_name = "CLASS_ID_MATCH";
        }
        SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, field_name, &field_id));
        SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, vlan_port->port, &vlan_domain));
        special_fields->special_fields[1].field_id = field_id;
        special_fields->special_fields[1].shr_var_uint32 = vlan_domain;

        /** c_vid */
        SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "VID_MATCH", &field_id));
        special_fields->special_fields[2].field_id = field_id;
        special_fields->special_fields[2].shr_var_uint32 = vlan_port->match_vlan;
    }
    /** Service tag priority and cfi */
    SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, FLOW_S_F_QOS_PRI, vlan_port->pkt_pri);

    SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, FLOW_S_F_QOS_CFI, vlan_port->pkt_cfi);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_esem_initiator_special_fields_get(
    int unit,
    int is_esem_default,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_special_fields_t * special_fields,
    bcm_vlan_port_t * vlan_port)
{
    uint32 sw_table_handle_id;
    bcm_flow_special_field_t special_field_data;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** get pri */
    sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
    SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_special_field_data_get
                              (unit, special_fields, FLOW_S_F_QOS_PRI, &special_field_data), _SHR_E_NOT_FOUND);
    vlan_port->pkt_pri = special_field_data.shr_var_uint32;

    /** get cfi */
    sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
    SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_special_field_data_get
                              (unit, special_fields, FLOW_S_F_QOS_CFI, &special_field_data), _SHR_E_NOT_FOUND);
    vlan_port->pkt_cfi = special_field_data.shr_var_uint32;

    if (!is_esem_default)
    {
        /*
         * Get ESEM keys
         * Note:
         * port is taken from SW DB because special fields (keys) carry VLAN-Domain value
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, &sw_table_handle_id));
        dbal_entry_key_field32_set(unit, sw_table_handle_id, DBAL_FIELD_GPORT, flow_handle_info->flow_id);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, sw_table_handle_id, DBAL_GET_ALL_FIELDS));

        SHR_IF_ERR_EXIT(dnx_vlan_port_egress_virtual_key_get(unit, sw_table_handle_id, NULL, vlan_port));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * ESEM initiator flow handle get
 */
shr_error_e
dnx_esem_initiator_flow_handle_get(
    int unit,
    int is_esem_default,
    bcm_flow_handle_t * flow_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_esem_default == TRUE)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_ESEM_DEFAULT_INITIATOR, flow_handle));
    }
    else
    {
        if (dnx_data_esem.feature.feature_get(unit, dnx_data_esem_feature_esem_is_dpc))
        {
            SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_ESEM_DPC_INITIATOR, flow_handle));
        }
        else
        {
            SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_ESEM_INITIATOR, flow_handle));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Create ESEM initiator
 */
static shr_error_e
dnx_esem_initiator_flow_create(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    bcm_flow_initiator_info_t esem_initiator_info;
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_special_fields_t special_fields;
    int is_esem_default;

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_initiator_info_t_init(&esem_initiator_info);
    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_special_fields_t_init(&special_fields);

    is_esem_default = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_DEFAULT);

    SHR_IF_ERR_EXIT(dnx_esem_initiator_flow_handle_get(unit, is_esem_default, &flow_handle));

    flow_handle_info.flow_handle = flow_handle;

    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_WITH_ID))
    {
        flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_WITH_ID;

        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_REPLACE))
        {
            flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_REPLACE;
        }
        /** convert the VLAN-Port gport to flow gport */
        SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                        (unit, vlan_port->vlan_port_id, _SHR_GPORT_TYPE_FLOW_LIF, &flow_handle_info.flow_id));
    }

    SHR_IF_ERR_EXIT(dnx_esem_flow_initiator_common_fields_set(unit, &esem_initiator_info, vlan_port));
    SHR_IF_ERR_EXIT(dnx_esem_initiator_special_fields_set
                    (unit, is_esem_default, flow_handle, &special_fields, vlan_port));
    SHR_IF_ERR_EXIT(bcm_dnx_flow_initiator_info_create(unit, &flow_handle_info, &esem_initiator_info, &special_fields));

    /** convert the flow gport VLAN-Port gport */
    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, flow_handle_info.flow_id, BCM_GPORT_VLAN_PORT, &vlan_port->vlan_port_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * Get ESEM initiator fields 
 */
static shr_error_e
dnx_esem_initiator_flow_get(
    int unit,
    bcm_vlan_port_t * vlan_port)
{

    bcm_flow_initiator_info_t esem_initiator_info;
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_special_fields_t special_fields;
    bcm_flow_handle_info_t flow_handle_info;
    int is_esem_default;

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_initiator_info_t_init(&esem_initiator_info);
    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_special_fields_t_init(&special_fields);

    is_esem_default = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_DEFAULT);

    SHR_IF_ERR_EXIT(dnx_esem_initiator_flow_handle_get(unit, is_esem_default, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;

    /** convert VLAN-Port gport to FLOW gport */
    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, vlan_port->vlan_port_id, _SHR_GPORT_TYPE_FLOW_LIF, &flow_handle_info.flow_id));

    SHR_IF_ERR_EXIT(bcm_dnx_flow_initiator_info_get(unit, &flow_handle_info, &esem_initiator_info, &special_fields));

    if (esem_initiator_info.valid_elements_set & BCM_FLOW_INITIATOR_ELEMENT_STAT_ID_VALID)
    {
        vlan_port->flags |= BCM_VLAN_PORT_STAT_EGRESS_ENABLE;
    }

    vlan_port->egress_network_group_id = esem_initiator_info.l2_egress_info.egress_network_group_id;

    sal_memcpy(&vlan_port->egress_qos_model, &esem_initiator_info.egress_qos_model,
               sizeof(vlan_port->egress_qos_model));

    SHR_IF_ERR_EXIT(dnx_esem_initiator_special_fields_get
                    (unit, is_esem_default, &flow_handle_info, &special_fields, vlan_port));
exit:
    SHR_FUNC_EXIT;
}

/**
 * Destroy ESEM initiator
 */
static shr_error_e
dnx_esem_initiator_flow_delete(
    int unit,
    bcm_gport_t gport)
{
    bcm_flow_initiator_info_t esem_initiator_info;
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_special_fields_t special_fields;
    int is_esem_default;

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_initiator_info_t_init(&esem_initiator_info);
    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_special_fields_t_init(&special_fields);

    is_esem_default = BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(gport);

    /** convert VLAN-Port gport to FLOW gport */
    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert(unit, gport, _SHR_GPORT_TYPE_FLOW_LIF, &flow_handle_info.flow_id));

    SHR_IF_ERR_EXIT(dnx_esem_initiator_flow_handle_get(unit, is_esem_default, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;

    SHR_IF_ERR_EXIT(bcm_dnx_flow_initiator_info_get(unit, &flow_handle_info, &esem_initiator_info, &special_fields));

    SHR_IF_ERR_EXIT(bcm_dnx_flow_initiator_info_destroy(unit, &flow_handle_info));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in .h file
 */
shr_error_e
dnx_esem_flow_create(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_REPLACE))
    {
        bcm_vlan_port_t old_vlan_port;

        bcm_vlan_port_t_init(&old_vlan_port);
        old_vlan_port.vlan_port_id = vlan_port->vlan_port_id;

        SHR_IF_ERR_EXIT(dnx_esem_flow_get(unit, &old_vlan_port));
        /*
         * Verify that the new VLAN-Port configuration is same as old VLAN-Port configuraion
         */
        SHR_INVOKE_VERIFY_DNXC(dnx_vlan_port_create_replace_new_old_compare_verify(unit, vlan_port, &old_vlan_port));
    }

    SHR_IF_ERR_EXIT(dnx_esem_initiator_flow_create(unit, vlan_port));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in .h file
 */
shr_error_e
dnx_esem_flow_get(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    SHR_FUNC_INIT_VARS(unit);

    if (vlan_port->vlan_port_id == 0)
    {
        /** Find by vlan_port criteria */
        
        SHR_SET_CURRENT_ERR(_SHR_E_UNAVAIL);
        SHR_EXIT();
    }

    vlan_port->flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
    if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(vlan_port->vlan_port_id))
    {
        vlan_port->flags |= BCM_VLAN_PORT_DEFAULT;
        vlan_port->criteria = BCM_VLAN_PORT_MATCH_NONE;
    }
    else
    {
        vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    }

    SHR_IF_ERR_EXIT(dnx_esem_initiator_flow_get(unit, vlan_port));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in .h file
 */
shr_error_e
dnx_esem_flow_delete(
    int unit,
    bcm_gport_t gport)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);

    /** check gport validity  */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_EGRESS_LOCAL_LIF_OR_VIRTUAL, &gport_hw_resources));

    SHR_IF_ERR_EXIT(dnx_esem_initiator_flow_delete(unit, gport));

exit:
    SHR_FUNC_EXIT;
}
