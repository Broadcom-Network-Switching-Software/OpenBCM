/** \file esem_flow.c
 *  General ESEM functionality using flow lif for DNX.
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
#include <bcm_int/dnx/auto_generated/dnx_flow_dispatch.h>
#include "vlan_port_flow.h"
#include "esem_flow.h"
#include <soc/dnx/dnx_data/auto_generated/dnx_data_failover.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>
#include <bcm_int/dnx/qos/qos.h>
#include <bcm_int/dnx/flow/flow_virtual_lif_mgmt.h>


static shr_error_e
dnx_esem_flow_match_info_add(
    int unit,
    bcm_vlan_port_t * vlan_port)
{

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_EXIT();

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_esem_flow_match_info_get(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_EXIT();

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_esem_flow_match_info_delete(
    int unit,
    bcm_gport_t gport)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_EXIT();

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for dnx_esem_flow_create
 */
static shr_error_e
dnx_esem_flow_create_verify(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_WITH_ID)) ||
        (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_REPLACE)))
    {
        if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(vlan_port->vlan_port_id))
        {
            uint32 default_result_profile = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_GET(vlan_port->vlan_port_id);
            uint32 nof_allocable_profiles = dnx_data_esem.default_result_profile.nof_allocable_profiles_get(unit);
            if (default_result_profile >= nof_allocable_profiles)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "vlan_port_id (0x%08X) is an illegal virtual gport for esem default entry!\n",
                             vlan_port->vlan_port_id);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 *   Check if the given gport resource is in LIF scope or Network scope.
 */
static shr_error_e
dnx_esem_initiator_native_is_lif_scoped(
    int unit,
    bcm_gport_t gport,
    int *is_lif_scoped)
{
    uint32 entry_handle_id;

    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    bcm_gport_t flow_gport;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    *is_lif_scoped = FALSE;

    /** convert the VLAN-Port gport to flow gport and check it's validity*/
    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, gport, BCM_GPORT_TYPE_FLOW_LIF, DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_INITIATOR, &flow_gport));
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, flow_gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_EGRESS_LOCAL_LIF_OR_VIRTUAL,
                     &gport_hw_resources));

    /*
     * get vlan_domain and esem-command
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.outlif_dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, gport_hw_resources.local_out_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources.outlif_dbal_result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

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
    
    /*
     * Service tag priority and cfi 
     * QOS is irrelevant only for ESEM Namespace-VSI
     */
    if (vlan_port->criteria != BCM_VLAN_PORT_MATCH_NAMESPACE_VSI)
    {
        esem_initiator_info->valid_elements_set = BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID;

        /*
         * BCM_FLOW_INITIATOR_ELEMENT_L2_EGRESS_INFO_VALID | 
         */
        esem_initiator_info->valid_elements_set |= BCM_FLOW_INITIATOR_ELEMENT_L2_EGRESS_INFO_VALID;
        esem_initiator_info->l2_egress_info.egress_network_group_id = vlan_port->egress_network_group_id;

        if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_REPLACE))
        {
            esem_initiator_info->valid_elements_set |= BCM_FLOW_INITIATOR_ELEMENT_QOS_MAP_ID_VALID;
            esem_initiator_info->qos_map_id = 0;
        }

        sal_memcpy(&esem_initiator_info->egress_qos_model, &vlan_port->egress_qos_model,
                   sizeof(vlan_port->egress_qos_model));
    }

    esem_initiator_info->l3_intf_id = BCM_IF_INVALID;

    if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_REPLACE))
    {
        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_STAT_EGRESS_ENABLE))
        {
            esem_initiator_info->valid_elements_set |= BCM_FLOW_INITIATOR_ELEMENT_STAT_ID_VALID |
                BCM_FLOW_INITIATOR_ELEMENT_STAT_PP_PROFILE_VALID;
            esem_initiator_info->stat_id = 0;
            esem_initiator_info->stat_pp_profile = 0;
        }
    }

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_esem_initiator_special_fields_set(
    int unit,
    bcm_flow_handle_t flow_handle,
    bcm_flow_special_fields_t * special_fields,
    bcm_vlan_port_t * vlan_port,
    int is_lif_scoped)
{
    bcm_flow_field_id_t field_id;

    SHR_FUNC_INIT_VARS(unit);

    special_fields->actual_nof_special_fields = 0;

    /** Set ESEM keys */

    switch (vlan_port->criteria)
    {
        case BCM_VLAN_PORT_MATCH_NONE:

            /** ESEM-Default */

            break;

        case BCM_VLAN_PORT_MATCH_PORT_VLAN:

            /** ESEM NW/LIF-scoped */

            /** FODO (VSI) */
            SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "VSI_MATCH", &field_id));
            special_fields->special_fields[0].field_id = field_id;
            special_fields->special_fields[0].shr_var_uint32 = vlan_port->vsi;

            if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
            {
                uint32 vlan_domain;

                SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, vlan_port->port, &vlan_domain));

                /** vlan_domain */
                SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "NAME_SPACE_MATCH", &field_id));
                SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, vlan_port->port, &vlan_domain));
                special_fields->special_fields[1].field_id = field_id;
                special_fields->special_fields[1].shr_var_uint32 = vlan_domain;

                /** c_vid */
                SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "VID_MATCH", &field_id));
                special_fields->special_fields[2].field_id = field_id;
                special_fields->special_fields[2].shr_var_uint32 = vlan_port->match_vlan;

                special_fields->actual_nof_special_fields = 3;
            }
            else
            {

                if (is_lif_scoped == FALSE)
                {
                    /** vlan_domain */
                    uint32 vlan_domain;
                    SHR_IF_ERR_EXIT(dnx_port_lif_egress_vlan_domain_get(unit, vlan_port->port, &vlan_domain));

                    SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "NAME_SPACE_MATCH", &field_id));
                    special_fields->special_fields[1].field_id = field_id;
                    special_fields->special_fields[1].shr_var_uint32 = vlan_domain;

                    /** c_vid */
                    SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "VID_MATCH", &field_id));
                    special_fields->special_fields[2].field_id = field_id;
                    special_fields->special_fields[2].shr_var_uint32 = vlan_port->match_vlan;

                    special_fields->actual_nof_special_fields = 3;
                }
                else
                {
                    /** out-lif */
                    bcm_gport_t flow_gport;

                    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                                    (unit, vlan_port->port, BCM_GPORT_TYPE_FLOW_LIF,
                                     DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_INITIATOR, &flow_gport));

                    SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "OUT_LIF_MATCH", &field_id));
                    special_fields->special_fields[1].field_id = field_id;
                    special_fields->special_fields[1].shr_var_uint32 = flow_gport;

                    special_fields->actual_nof_special_fields = 2;
                }
            }

            break;

        case BCM_VLAN_PORT_MATCH_NAMESPACE_VSI:

            SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "VSI_MATCH", &field_id));
            special_fields->special_fields[0].field_id = field_id;
            special_fields->special_fields[0].shr_var_uint32 = vlan_port->vsi;

            SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "NAME_SPACE_MATCH", &field_id));
            special_fields->special_fields[1].field_id = field_id;
            special_fields->special_fields[1].shr_var_uint32 = vlan_port->match_class_id;

            special_fields->actual_nof_special_fields = 2;

            break;

        case BCM_VLAN_PORT_MATCH_NAMESPACE_PORT:
        {
            dnx_algo_gpm_gport_phy_info_t gport_phy_info;

            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                            (unit, vlan_port->port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_phy_info));

            SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "SYSTEM_PORT_MATCH", &field_id));
            special_fields->special_fields[0].field_id = field_id;
            special_fields->special_fields[0].shr_var_uint32 = gport_phy_info.sys_port;

            SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "NAME_SPACE_MATCH", &field_id));
            special_fields->special_fields[1].field_id = field_id;
            special_fields->special_fields[1].shr_var_uint32 = vlan_port->match_class_id;

            special_fields->actual_nof_special_fields = 2;
        }
            break;

        default:

            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error ! criteria = %d is not supported for Egress Virtual AC! gport = 0x%08X, flags = 0x%08X\n",
                         vlan_port->criteria, vlan_port->vlan_port_id, vlan_port->flags);

    }

    /*
     * Service tag priority and cfi 
     * QOS is irrelevant only for ESEM Namespace-VSI
     */
    if (vlan_port->criteria != BCM_VLAN_PORT_MATCH_NAMESPACE_VSI)
    {

        SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, FLOW_S_F_QOS_PRI, vlan_port->pkt_pri);

        SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, FLOW_S_F_QOS_CFI, vlan_port->pkt_cfi);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_esem_initiator_special_fields_get(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_special_fields_t * special_fields,
    bcm_vlan_port_t * vlan_port)
{
    bcm_flow_special_field_t special_field_data;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    switch (vlan_port->criteria)
    {
        case BCM_VLAN_PORT_MATCH_NONE:

            /** ESEM-Default */

            break;

        case BCM_VLAN_PORT_MATCH_PORT_VLAN:

            /*
             * ESEM NW-scoped:
             *    - FLOW_S_F_VSI_MATCH
             *    - FLOW_S_F_NAME_SPACE_MATCH - it contains the VLAN_DOMAIN of the port thus port is read from SW DB
             *    - FLOW_S_F_VID_MATCH
             *
             * ESEM LIF-scoped:
             *    - FLOW_S_F_VSI_MATCH
             *    - FLOW_S_F_OUT_LIF_MATCH -it contains the local out-lif thus outlif gport is read from SW DB
             */

            /** FODO (VSI) */
            sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
            SHR_IF_ERR_EXIT(dnx_flow_special_field_data_get
                            (unit, special_fields, FLOW_S_F_VSI_MATCH, &special_field_data));
            vlan_port->vsi = special_field_data.shr_var_uint32;

            /** VID */
            sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
            SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_special_field_data_get
                                      (unit, special_fields, FLOW_S_F_VID_MATCH, &special_field_data),
                                      _SHR_E_NOT_FOUND);
            vlan_port->match_vlan = special_field_data.shr_var_uint32;

            break;

        case BCM_VLAN_PORT_MATCH_NAMESPACE_VSI:

            /*
             * FLOW_S_F_VSI_MATCH
             * FLOW_S_F_NAME_SPACE_MATCH
             */

            /** FODO (VSI) */
            sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
            SHR_IF_ERR_EXIT(dnx_flow_special_field_data_get
                            (unit, special_fields, FLOW_S_F_VSI_MATCH, &special_field_data));
            vlan_port->vsi = special_field_data.shr_var_uint32;

            sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
            SHR_IF_ERR_EXIT(dnx_flow_special_field_data_get
                            (unit, special_fields, FLOW_S_F_NAME_SPACE_MATCH, &special_field_data));
            vlan_port->match_class_id = special_field_data.shr_var_uint32;

            break;

        case BCM_VLAN_PORT_MATCH_NAMESPACE_PORT:

            /*
             * FLOW_S_F_SYSTEM_PORT_MATCH - it contains the system port thus port gport is read from SW DB
             * FLOW_S_F_NAME_SPACE_MATCH
             */
            sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
            SHR_IF_ERR_EXIT(dnx_flow_special_field_data_get
                            (unit, special_fields, FLOW_S_F_NAME_SPACE_MATCH, &special_field_data));
            vlan_port->match_class_id = special_field_data.shr_var_uint32;

            break;

        default:

            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error ! criteria = %d is not supported for Egress Virtual AC! gport = 0x%08X, flags = 0x%08X\n",
                         vlan_port->criteria, vlan_port->vlan_port_id, vlan_port->flags);

    }

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
    bcm_vlan_port_t * vlan_port,
    bcm_flow_handle_t * flow_handle,
    int *is_lif_scoped)
{
    SHR_FUNC_INIT_VARS(unit);

    *is_lif_scoped = FALSE;

    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_DEFAULT))
    {
        SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_ESEM_DEFAULT_INITIATOR, flow_handle));
    }
    else if (vlan_port->criteria == BCM_VLAN_PORT_MATCH_NAMESPACE_VSI)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get
                        (unit, FLOW_APP_NAME_VLAN_PORT_ESEM_NAMESPACE_VSI_INITIATOR, flow_handle));
    }
    else if (vlan_port->criteria == BCM_VLAN_PORT_MATCH_NAMESPACE_PORT)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_HIGH_SCALE_MIRROR_MAPPING_INITIATOR, flow_handle));
    }
    else
    {
        if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
        {
            SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get
                            (unit, FLOW_APP_NAME_VLAN_PORT_ESEM_NW_SCOPED_INITIATOR, flow_handle));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_esem_initiator_native_is_lif_scoped(unit, vlan_port->port, is_lif_scoped));

            if (*is_lif_scoped == TRUE)
            {
                SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get
                                (unit, FLOW_APP_NAME_VLAN_PORT_ESEM_LIF_SCOPED_INITIATOR, flow_handle));

            }
            else
            {
                SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get
                                (unit, FLOW_APP_NAME_VLAN_PORT_ESEM_NW_SCOPED_INITIATOR, flow_handle));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * ESEM initiator flow handle get using gport
 */
shr_error_e
dnx_esem_initiator_flow_handle_from_gport_get(
    int unit,
    bcm_gport_t gport,
    bcm_flow_handle_t * flow_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    *flow_handle = 0;

    if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(gport))
    {
        SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_VLAN_PORT_ESEM_DEFAULT_INITIATOR, flow_handle));
    }
    else
    {
        /*
         * ESEM:
         *  - NW scoped
         *  - LIF scoped
         *  - Namespace-VSI
         *  - Namespace-Port
         */
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
        bcm_gport_t flow_gport;

        /** convert the VLAN-Port gport to flow gport and check it's validity*/
        SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                        (unit, gport, BCM_GPORT_TYPE_FLOW_LIF, DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_INITIATOR,
                         &flow_gport));
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, flow_gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_EGRESS_LOCAL_LIF_OR_VIRTUAL,
                         &gport_hw_resources));

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
    int is_lif_scoped;

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_initiator_info_t_init(&esem_initiator_info);
    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_special_fields_t_init(&special_fields);

    SHR_IF_ERR_EXIT(dnx_esem_initiator_flow_handle_get(unit, vlan_port, &flow_handle, &is_lif_scoped));

    flow_handle_info.flow_handle = flow_handle;
    flow_handle_info.flags = BCM_FLOW_HANDLE_INFO_VIRTUAL;

    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_WITH_ID))
    {
        flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_WITH_ID;

        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_REPLACE))
        {
            flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_REPLACE;
            flow_handle_info.flags &= (~BCM_FLOW_HANDLE_INFO_VIRTUAL);
        }

        /** convert the VLAN-Port gport to flow gport */
        SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                        (unit, vlan_port->vlan_port_id, BCM_GPORT_TYPE_FLOW_LIF,
                         DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_INITIATOR, &flow_handle_info.flow_id));
    }

    if ((_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE)) &&
        (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_REPLACE)))
    {
        flow_handle_info.flags |= BCM_FLOW_HANDLE_NATIVE;
    }

    SHR_IF_ERR_EXIT(dnx_esem_flow_initiator_common_fields_set(unit, &esem_initiator_info, vlan_port));
    SHR_IF_ERR_EXIT(dnx_esem_initiator_special_fields_set
                    (unit, flow_handle, &special_fields, vlan_port, is_lif_scoped));
    SHR_IF_ERR_EXIT(bcm_dnx_flow_initiator_info_create(unit, &flow_handle_info, &esem_initiator_info, &special_fields));

    /** convert the flow gport VLAN-Port gport */
    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, flow_handle_info.flow_id, BCM_GPORT_VLAN_PORT, DNX_ALGO_GPM_CONVERSION_FLAG_NONE,
                     &vlan_port->vlan_port_id));

    /*
     * Add ESEM match-info to SW DB:
     */
    if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_REPLACE))
    {
        SHR_IF_ERR_EXIT(dnx_esem_flow_match_info_add(unit, vlan_port));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_esem_initiator_flow_find_by_lookup_fields_update(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields,
    bcm_vlan_port_t * vlan_port)
{

    SHR_FUNC_INIT_VARS(unit);

    /** convert the flow gport VLAN-Port gport */
    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, flow_handle_info->flow_id, BCM_GPORT_VLAN_PORT, DNX_ALGO_GPM_CONVERSION_FLAG_NONE,
                     &vlan_port->vlan_port_id));

    if (initiator_info->valid_elements_set & BCM_FLOW_INITIATOR_ELEMENT_STAT_ID_VALID)
    {
        vlan_port->flags |= BCM_VLAN_PORT_STAT_EGRESS_ENABLE;
    }

    vlan_port->egress_network_group_id = initiator_info->l2_egress_info.egress_network_group_id;

    sal_memcpy(&vlan_port->egress_qos_model, &initiator_info->egress_qos_model, sizeof(vlan_port->egress_qos_model));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_esem_initiator_flow_find_by_lookup_nw_scoped_cb(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields,
    void *data)
{
    dnx_vlan_port_flow_egress_user_data_t *flow_user_data = (dnx_vlan_port_flow_egress_user_data_t *) data;
    bcm_vlan_port_t *vlan_port = (bcm_vlan_port_t *) flow_user_data->user_data;
    bcm_flow_special_field_t special_field_data;
    uint32 hw_vsi;

    SHR_FUNC_INIT_VARS(unit);

    /** Get FODO (VSI) */
    sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
    SHR_IF_ERR_EXIT(dnx_flow_special_field_data_get(unit, special_fields, FLOW_S_F_VSI_MATCH, &special_field_data));
    hw_vsi = special_field_data.shr_var_uint32;

    /** Compare first key */
    if (hw_vsi == vlan_port->vsi)
    {
        uint32 hw_vid;
        /** Get VID */
        sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
        SHR_IF_ERR_EXIT(dnx_flow_special_field_data_get(unit, special_fields, FLOW_S_F_VID_MATCH, &special_field_data));
        hw_vid = special_field_data.shr_var_uint32;

        /** Compare second key */
        if (hw_vid == vlan_port->match_vlan)
        {
            uint32 hw_name_space;
            uint32 vlan_domain;

            if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
            {
                SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, vlan_port->port, &vlan_domain));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_port_lif_egress_vlan_domain_get(unit, vlan_port->port, &vlan_domain));
            }

            /** Get NAME_SPACE (VLAN-Domain) */
            sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
            SHR_IF_ERR_EXIT(dnx_flow_special_field_data_get
                            (unit, special_fields, FLOW_S_F_NAME_SPACE_MATCH, &special_field_data));
            hw_name_space = special_field_data.shr_var_uint32;

            /** Compare third key */
            if (hw_name_space == vlan_domain)
            {
                /*
                 * Bingo! we have found it!
                 * 
                 * Update vlan_port_id and other relevant fields
                 */

                SHR_IF_ERR_EXIT(dnx_esem_initiator_flow_find_by_lookup_fields_update
                                (unit, flow_handle_info, initiator_info, special_fields, vlan_port));

            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_esem_initiator_flow_find_by_lookup_lif_scoped_cb(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields,
    void *data)
{
    dnx_vlan_port_flow_egress_user_data_t *flow_user_data = (dnx_vlan_port_flow_egress_user_data_t *) data;
    bcm_vlan_port_t *vlan_port = (bcm_vlan_port_t *) flow_user_data->user_data;
    bcm_flow_special_field_t special_field_data;
    uint32 hw_vsi;

    SHR_FUNC_INIT_VARS(unit);

    /** Get FODO (VSI) */
    sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
    SHR_IF_ERR_EXIT(dnx_flow_special_field_data_get(unit, special_fields, FLOW_S_F_VSI_MATCH, &special_field_data));
    hw_vsi = special_field_data.shr_var_uint32;

    /** Compare first key */
    if (hw_vsi == vlan_port->vsi)
    {
        uint32 hw_flow_gport;
        bcm_gport_t flow_gport;

        /** Get OUT-LIF */
        sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
        SHR_IF_ERR_EXIT(dnx_flow_special_field_data_get
                        (unit, special_fields, FLOW_S_F_OUT_LIF_MATCH, &special_field_data));
        hw_flow_gport = special_field_data.shr_var_uint32;

        SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                        (unit, vlan_port->port, BCM_GPORT_TYPE_FLOW_LIF,
                         DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_INITIATOR, &flow_gport));

        /** Compare second key */
        if (hw_flow_gport == flow_gport)
        {
            /*
             * Bingo! we have found it!
             * 
             * Update vlan_port_id and other relevant fields
             */
            SHR_IF_ERR_EXIT(dnx_esem_initiator_flow_find_by_lookup_fields_update
                            (unit, flow_handle_info, initiator_info, special_fields, vlan_port));

        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_esem_initiator_flow_find_by_lookup_namespace_vsi_cb(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields,
    void *data)
{
    dnx_vlan_port_flow_egress_user_data_t *flow_user_data = (dnx_vlan_port_flow_egress_user_data_t *) data;
    bcm_vlan_port_t *vlan_port = (bcm_vlan_port_t *) flow_user_data->user_data;
    bcm_flow_special_field_t special_field_data;
    uint32 hw_vsi;

    SHR_FUNC_INIT_VARS(unit);

    /** Get FODO (VSI) */
    sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
    SHR_IF_ERR_EXIT(dnx_flow_special_field_data_get(unit, special_fields, FLOW_S_F_VSI_MATCH, &special_field_data));
    hw_vsi = special_field_data.shr_var_uint32;

    /** Compare first key */
    if (hw_vsi == vlan_port->vsi)
    {
        uint32 hw_name_space;

        /** Get NAME_SPACE */
        sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
        SHR_IF_ERR_EXIT(dnx_flow_special_field_data_get
                        (unit, special_fields, FLOW_S_F_NAME_SPACE_MATCH, &special_field_data));
        hw_name_space = special_field_data.shr_var_uint32;

        /** Compare second key */
        if (hw_name_space == vlan_port->match_class_id)
        {
            /*
             * Bingo! we have found it!
             * 
             * Update vlan_port_id and other relevant fields
             */
            SHR_IF_ERR_EXIT(dnx_esem_initiator_flow_find_by_lookup_fields_update
                            (unit, flow_handle_info, initiator_info, special_fields, vlan_port));

        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_esem_initiator_flow_find_by_lookup_namespace_port_cb(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields,
    void *data)
{
    dnx_vlan_port_flow_egress_user_data_t *flow_user_data = (dnx_vlan_port_flow_egress_user_data_t *) data;
    bcm_vlan_port_t *vlan_port = (bcm_vlan_port_t *) flow_user_data->user_data;
    dnx_algo_gpm_gport_phy_info_t gport_phy_info;
    bcm_flow_special_field_t special_field_data;
    uint32 hw_sys_port;

    SHR_FUNC_INIT_VARS(unit);

    /** Get System port */
    sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
    SHR_IF_ERR_EXIT(dnx_flow_special_field_data_get
                    (unit, special_fields, FLOW_S_F_SYSTEM_PORT_MATCH, &special_field_data));
    hw_sys_port = special_field_data.shr_var_uint32;

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, vlan_port->port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_phy_info));

    /** Compare first key */
    if (hw_sys_port == gport_phy_info.sys_port)
    {
        uint32 hw_name_space;

        /** Get NAME_SPACE */
        sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
        SHR_IF_ERR_EXIT(dnx_flow_special_field_data_get
                        (unit, special_fields, FLOW_S_F_NAME_SPACE_MATCH, &special_field_data));
        hw_name_space = special_field_data.shr_var_uint32;

        /** Compare second key */
        if (hw_name_space == vlan_port->match_class_id)
        {
            /*
             * Bingo! we have found it!
             * 
             * Update vlan_port_id and other relevant fields
             */
            SHR_IF_ERR_EXIT(dnx_esem_initiator_flow_find_by_lookup_fields_update
                            (unit, flow_handle_info, initiator_info, special_fields, vlan_port));

        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Get ESEM initiator fields "by lookup" that is the keys are the input and not the gport
 */
static shr_error_e
dnx_esem_initiator_flow_get_by_lookup(
    int unit,
    bcm_vlan_port_t * vlan_port)
{

    SHR_FUNC_INIT_VARS(unit);

    switch (vlan_port->criteria)
    {
        case BCM_VLAN_PORT_MATCH_PORT_VLAN:
        {
            /*
             * ESEM AC - native or non-native:
             */

            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
            {
                int is_lif_scoped;
                SHR_IF_ERR_EXIT(dnx_esem_initiator_native_is_lif_scoped(unit, vlan_port->port, &is_lif_scoped));

                if (is_lif_scoped == TRUE)
                {
                    /*
                     * Handle ESEM LIF-scoped native
                     */
                    SHR_IF_ERR_EXIT(dnx_vlan_port_flow_traverse_egress_app
                                    (unit, FLOW_APP_NAME_VLAN_PORT_ESEM_LIF_SCOPED_INITIATOR,
                                     dnx_esem_initiator_flow_find_by_lookup_lif_scoped_cb, NULL, vlan_port));
                }
                else
                {
                    /*
                     * Handle ESEM NW-scoped native
                     */
                    SHR_IF_ERR_EXIT(dnx_vlan_port_flow_traverse_egress_app
                                    (unit, FLOW_APP_NAME_VLAN_PORT_ESEM_NW_SCOPED_INITIATOR,
                                     dnx_esem_initiator_flow_find_by_lookup_nw_scoped_cb, NULL, vlan_port));
                }
            }
            else
            {
                /*
                 * Handle ESEM NW-scoped non-native
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_flow_traverse_egress_app
                                (unit, FLOW_APP_NAME_VLAN_PORT_ESEM_NW_SCOPED_INITIATOR,
                                 dnx_esem_initiator_flow_find_by_lookup_nw_scoped_cb, NULL, vlan_port));
            }

            break;
        }
        case BCM_VLAN_PORT_MATCH_NAMESPACE_VSI:
        {
            /*
             * Handle ESEM Namespace-vsi:
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_flow_traverse_egress_app
                            (unit, FLOW_APP_NAME_VLAN_PORT_ESEM_NAMESPACE_VSI_INITIATOR,
                             dnx_esem_initiator_flow_find_by_lookup_namespace_vsi_cb, NULL, vlan_port));
            break;
        }
        case BCM_VLAN_PORT_MATCH_NAMESPACE_PORT:
        {
            /*
             * Handle ESEM Namespace-port:
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_flow_traverse_egress_app
                            (unit, FLOW_APP_NAME_HIGH_SCALE_MIRROR_MAPPING_INITIATOR,
                             dnx_esem_initiator_flow_find_by_lookup_namespace_port_cb, NULL, vlan_port));
            break;
        }
        default:

            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error ! criteria = %d is not supported for Egress ESEM! flags = 0x%08X\n",
                         vlan_port->criteria, vlan_port->flags);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Gets EGRESS ESEM-Default EVE  parameters
 * Note:
 * ESEM-Default profiles EVE and Native EVE are not handled by FLOW thus
 * need to use direct DBAL access.
 * Note2:
 * Since no parameter is set on init (see dnx_port_esem_default_program_init), no need to perform any DBAL access
 */
static shr_error_e
dnx_esem_initiator_flow_get_esem_default_eve(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    SHR_FUNC_INIT_VARS(unit);

    vlan_port->criteria = BCM_VLAN_PORT_MATCH_NONE;
    vlan_port->flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION | BCM_VLAN_PORT_DEFAULT;

    if (BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_GET(vlan_port->vlan_port_id) ==
        dnx_data_esem.default_result_profile.default_native_get(unit))
    {
        vlan_port->flags |= BCM_VLAN_PORT_NATIVE;
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * Get ESEM initiator fields 
 */
static shr_error_e
dnx_esem_initiator_flow_get_by_gport(
    int unit,
    bcm_vlan_port_t * vlan_port)
{

    bcm_flow_initiator_info_t esem_initiator_info;
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_special_fields_t special_fields;
    bcm_flow_handle_info_t flow_handle_info;

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_initiator_info_t_init(&esem_initiator_info);
    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_special_fields_t_init(&special_fields);

    if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(vlan_port->vlan_port_id))
    {
        uint32 default_result_profile = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_GET(vlan_port->vlan_port_id);
        uint32 default_native_profile = dnx_data_esem.default_result_profile.default_native_get(unit);
        uint32 default_ac_profile = dnx_data_esem.default_result_profile.default_ac_get(unit);
        if ((default_result_profile == default_native_profile) || (default_result_profile == default_ac_profile))
        {
            SHR_IF_ERR_EXIT(dnx_esem_initiator_flow_get_esem_default_eve(unit, vlan_port));
            SHR_EXIT();
        }
    }

    SHR_IF_ERR_EXIT(dnx_esem_initiator_flow_handle_from_gport_get(unit, vlan_port->vlan_port_id, &flow_handle));

    flow_handle_info.flow_handle = flow_handle;

    /** convert VLAN-Port gport to FLOW gport */
    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, vlan_port->vlan_port_id, BCM_GPORT_TYPE_FLOW_LIF,
                     DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_INITIATOR, &flow_handle_info.flow_id));

    SHR_IF_ERR_EXIT(bcm_dnx_flow_initiator_info_get(unit, &flow_handle_info, &esem_initiator_info, &special_fields));

    vlan_port->flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
    if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(vlan_port->vlan_port_id))
    {
        vlan_port->flags |= BCM_VLAN_PORT_DEFAULT;
        vlan_port->criteria = BCM_VLAN_PORT_MATCH_NONE;
    }

    if (esem_initiator_info.valid_elements_set & BCM_FLOW_INITIATOR_ELEMENT_STAT_ID_VALID)
    {
        vlan_port->flags |= BCM_VLAN_PORT_STAT_EGRESS_ENABLE;
    }

    if (_SHR_IS_FLAG_SET(flow_handle_info.flags, BCM_FLOW_HANDLE_NATIVE))
    {
        vlan_port->flags |= BCM_VLAN_PORT_NATIVE;
    }

    vlan_port->egress_network_group_id = esem_initiator_info.l2_egress_info.egress_network_group_id;

    sal_memcpy(&vlan_port->egress_qos_model, &esem_initiator_info.egress_qos_model,
               sizeof(vlan_port->egress_qos_model));

    if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(vlan_port->vlan_port_id))
    {
        vlan_port->criteria = BCM_VLAN_PORT_MATCH_NONE;
    }
    else
    {
        /*
         * Get ESEM keys from SW DB
         */
        SHR_IF_ERR_EXIT(dnx_esem_flow_match_info_get(unit, vlan_port));
    }

    SHR_IF_ERR_EXIT(dnx_esem_initiator_special_fields_get(unit, &flow_handle_info, &special_fields, vlan_port));

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

    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_initiator_info_t_init(&esem_initiator_info);
    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_special_fields_t_init(&special_fields);

    /** convert VLAN-Port gport to FLOW gport */
    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, gport, BCM_GPORT_TYPE_FLOW_LIF, DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_INITIATOR,
                     &flow_handle_info.flow_id));

    SHR_IF_ERR_EXIT(dnx_esem_initiator_flow_handle_from_gport_get(unit, gport, &flow_handle));

    flow_handle_info.flow_handle = flow_handle;

    SHR_IF_ERR_EXIT(bcm_dnx_flow_initiator_info_destroy(unit, &flow_handle_info));

    /*
     * Delete ESEM match-info from SW DB.
     */
    if (!(BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(gport)))
    {
        SHR_IF_ERR_EXIT(dnx_esem_flow_match_info_delete(unit, gport));
    }

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

    SHR_INVOKE_VERIFY_DNXC(dnx_esem_flow_create_verify(unit, vlan_port));

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
        /** ESEM - Find by lookup */
        SHR_IF_ERR_EXIT(dnx_esem_initiator_flow_get_by_lookup(unit, vlan_port));
    }
    else
    {
        /** ESEM - Find by gport */
        SHR_IF_ERR_EXIT(dnx_esem_initiator_flow_get_by_gport(unit, vlan_port));
    }
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
    bcm_gport_t flow_gport;

    SHR_FUNC_INIT_VARS(unit);

    /** convert the VLAN-Port gport to flow gport and check it's validity*/
    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, gport, BCM_GPORT_TYPE_FLOW_LIF, DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_INITIATOR, &flow_gport));
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, flow_gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_EGRESS_LOCAL_LIF_OR_VIRTUAL,
                     &gport_hw_resources));

    SHR_IF_ERR_EXIT(dnx_esem_initiator_flow_delete(unit, gport));

exit:
    SHR_FUNC_EXIT;
}
