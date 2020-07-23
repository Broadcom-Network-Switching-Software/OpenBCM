/** \file vswitch.c
 *
 *  Vswitch procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_VLAN
/*
 * Include files.
 * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/vswitch/vswitch.h>
#include <bcm_int/dnx/vsi/vswitch_vpn.h>
#include <bcm_int/dnx/l3/l3.h>
#include <bcm_int/dnx/vsi/vsi.h>
#include <bcm_int/dnx/stg/stg.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/vlan_access.h>
#include <bcm_int/dnx_dispatch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_vlan.h>
#include <bcm_int/common/multicast.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>

/*
 * }
 */

/*
 * DEFINES
 * {
 */

/*
 * }
 */

/*
 * MACROs
 * {
 */

/*
 * }
 */

/**
 * \brief - see description at vswitch.h
 */
shr_error_e
dnx_vswitch_entry_is_p2p(
    int unit,
    uint32 entry_handle_id,
    int *is_p2p)
{
    uint32 service_type;
    uint8 is_field_on_handle;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * VSWITCH is P2P:
     * - the field SERVICE_TYPE exist.
     * - SERVICE_TYPE value is P2P
     */

    *is_p2p = FALSE;

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_SERVICE_TYPE, &is_field_on_handle, &service_type));

    if (is_field_on_handle == TRUE)
    {
        dbal_enum_value_field_vtt_lif_service_type_e vtt_lif_service_type;
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                        (unit, DBAL_FIELD_SERVICE_TYPE, DBAL_FIELD_TYPE, &vtt_lif_service_type, &service_type));
        if (vtt_lif_service_type == DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P)
        {
            *is_p2p = TRUE;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - see description at vswitch.h
 */
shr_error_e
dnx_vswitch_is_p2p(
    int unit,
    bcm_gport_t gport,
    int *is_p2p)
{

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    *is_p2p = FALSE;

    if (_SHR_GPORT_IS_VSWITCH(gport))
    {
        uint32 entry_handle_id;
        lif_table_mngr_inlif_info_t lif_info;
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources));
        /*
         *
         * Expecting SERVICE_TYPE to be P2P
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));

        /*
         * Set keys and gets all fields:
         */
        
        if (_SHR_GPORT_IS_EXTENDER_PORT(gport))
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         gport_hw_resources.inlif_dbal_result_type);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_inlif_info
                            (unit, _SHR_CORE_ALL, gport_hw_resources.local_in_lif, entry_handle_id, &lif_info));
        }

        SHR_IF_ERR_EXIT(dnx_vswitch_entry_is_p2p(unit, entry_handle_id, is_p2p));

    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Wrong setting. gport = 0x%08X is not a VSWITCH-Port (VLAN-PORT, MPLS-PORT, TUNNEL or EXTENDER-Port).\n",
                     gport);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Verify function for BCM-API: bcm_dnx_vswitch_create().
 *
 * \param [in] unit - relevant unit.
 * \param [in] vsi - pointer to vsi
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 * bcm_dnx_vswitch_create
 */
static shr_error_e
dnx_vswitch_create_verify(
    int unit,
    bcm_vlan_t * vsi)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(vsi, _SHR_E_PARAM, "vsi");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Verify function for BCM-API: bcm_dnx_vswitch_create_with_id()
 *
 * \param [in] unit - relevant unit.
 * \param [in] vsi - vsi ID.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 * bcm_dnx_vswitch_create_with_id
 */
static shr_error_e
dnx_vswitch_create_with_id_verify(
    int unit,
    bcm_vlan_t vsi)
{
    int nof_vsis;
    uint8 is_allocated = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify vsi is in range:
     */
    SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));

    if (vsi >= nof_vsis)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "vsi = %d is not valid!!! (MAX value for vsi is %d)\n", vsi, (nof_vsis - 1));
    }

    /*
     * Check that the vsi was not allocated.
     * Note: in case the vsi is already allocated, dnx_algo_res_is_allocated returns _SHR_E_EXISTS
     */
    SHR_IF_ERR_EXIT(vlan_db.vsi.is_allocated(unit, vsi, &is_allocated));

    if (is_allocated == TRUE)
    {
        SHR_ERR_EXIT(_SHR_E_EXISTS, "vsi %d already exist!!!\n", vsi);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Verify function for BCM-API: bcm_dnx_vswitch_destroy()
 *
 * \param [in] unit - relevant unit.
 * \param [in] vsi - vsi ID.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 * bcm_dnx_vswitch_destroy
 */
static shr_error_e
dnx_vswitch_destroy_verify(
    int unit,
    bcm_vlan_t vsi)
{
    uint8 is_vswitch = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify vsi type is VSWITCH VSI
     */
    SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_per_type_get(unit, (int) vsi, _bcmDnxVsiTypeVswitch, &is_vswitch));
    if (!is_vswitch)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "VSI is not allocated for this usage!\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_vswitch_cross_connect_add/get/delete
 */
static shr_error_e
dnx_vswitch_cross_connect_verify(
    int unit,
    bcm_vswitch_cross_connect_t * gports)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(gports, _SHR_E_PARAM, "gports");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_vswitch_cross_connect_add/get/delete.
 * It checks the validity if the source and destination ports that to be cross-connected.
 * 1. Checks that the source gport types is VSWITCH_PORT
 * 2. Checks that the destination port type is either Physical port or FEC or VLAN-PORT / MPLS-PORT
 * 3. Checks that the source gport type is P2P (only VLAN-PORT may not be P2P)
 *
 * \param [in] unit - relevant unit.
 * \param [in] gport1 - source gport to be cross connected.
 * \param [in] gport2 - destination gport to be cross connected.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *  bcm_dnx_vswitch_cross_connect_add
 *  bcm_dnx_vswitch_cross_connect_get
 *  bcm_dnx_vswitch_cross_connect_delete
 */
static shr_error_e
dnx_vswitch_cross_connect_port_verify(
    int unit,
    bcm_gport_t gport1,
    bcm_gport_t gport2)
{
    uint8 is_physical_port;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Checks that the source gport type is VSWITCH-Port
     */
    if (!_SHR_GPORT_IS_VSWITCH(gport1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Wrong setting. port1 = 0x%08X is not a VSWITCH-Port (VLAN-PORT, MPLS-PORT, TUNNEL or EXTENDER-Port).\n",
                     gport1);
    }

    /*
     * For VLAN-Port, verify the LIF is not Egress virtual
     */
    if (_SHR_GPORT_IS_VLAN_PORT(gport1))
    {
        if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(gport1) || BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(gport1))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong setting. port1 = 0x%08X is an Egress Virtual VLAN-PORT!\n", gport1);
        }
    }

    /*
     * Checks that the destination port type is either Physical port or FEC or VLAN-PORT / MPLS-PORT
     */
    if (gport2 != 0)
    {

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, gport2, &is_physical_port));

        if (!(_SHR_GPORT_IS_VLAN_PORT(gport2)) &&
            !(_SHR_GPORT_IS_MPLS_PORT(gport2)) &&
            !(_SHR_GPORT_IS_EXTENDER_PORT(gport2)) && !_SHR_GPORT_IS_FORWARD_PORT(gport2) && !is_physical_port)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Wrong setting. port2 = 0x%08X is not a VLAN-PORT nor MPLS-PORT nor EXTENDER-PORT nor FEC nor physical port.\n",
                         gport2);
        }

        /*
         * For VLAN-PORT, check that it is not Virtual 
         */
        if (_SHR_GPORT_IS_VLAN_PORT(gport2))
        {
            if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_INGRESS_NATIVE(gport2) ||
                BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(gport2) ||
                BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(gport2))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong setting. port2 = 0x%08X is a Virtual VLAN-PORT!\n", gport2);
            }
        }
    }

    /*
     * Checks that the source In-LIF is P2P LIF
     *
     * Note: for VLAN-PORT - the In-LIF may be MP
     */
    if (!_SHR_GPORT_IS_VLAN_PORT(gport1) && !_SHR_GPORT_IS_EXTENDER_PORT(gport1))
    {
        int is_p2p;

        SHR_IF_ERR_EXIT(dnx_vswitch_is_p2p(unit, gport1, &is_p2p));

        if (is_p2p == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Wrong setting. IN-LIF of port1 = %d = 0x%08X is not a P2P IN-LIF\n", gport1, gport1);
        }

    }
    else
    {
        /*
         * For VLAN-PORT, only non-virtual AC can be cross-connected
         */
        if (_SHR_GPORT_IS_VLAN_PORT(gport1) && BCM_GPORT_SUB_TYPE_IS_VIRTUAL_INGRESS_NATIVE(gport1))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Wrong setting. IN-LIF of port1 = %d = 0x%08X is Virtual Native AC ! Only non Virtual AC can be cross-connected!\n",
                         gport1, gport1);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Internal function to get forward-info
 * from a DBAL entry.
 * \param [in] unit - relevant unit
 * \param [in] entry_handle_id -in-lif table handle
 *        dnx_algo_gpm_gport_hw_resources_t structure holding
 *        the GPORT hardware resources information.
 * \param [in] is_get - boolean indication, indicate if the function is clled from bcm get operation or not
 * \param [in] is_gport_ac - boolean indication, whether the crossed-connected source gport is ac port or not
 * \param [out] forward_info - pointer to forward-info structure
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *  bcm_dnx_vswitch_cross_connect_get
 *  bcm_dnx_vswitch_cross_connect_delete
 */
static shr_error_e
dnx_vswitch_cross_connect_get_forward_info(
    int unit,
    uint32 entry_handle_id,
    dnx_algo_gpm_forward_info_t * forward_info,
    uint8 is_get,
    uint8 is_gport_ac)
{
    uint32 temp_destination;
    uint32 temp_outlif;
    uint32 temp_eei;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    temp_destination = 0;
    temp_outlif = 0;
    temp_eei = 0;

    /*
     * Get fwd_info from DBAL table:
     *  - out-lif or eei
     *  - destination
     */
    rv = dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, &temp_outlif);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if (rv == _SHR_E_NOT_FOUND)
    {
        rv = dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_EEI, INST_SINGLE, &temp_eei);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, &temp_destination));

    if (is_get)
    {
        SHR_VAL_VERIFY(!(temp_destination | temp_outlif | temp_eei), 0, _SHR_E_NOT_FOUND,
                       "Cross-connect information not found on gport\n");
    }
    else if (is_gport_ac)
    {
        
        SHR_VAL_VERIFY(!(temp_destination | temp_outlif | temp_eei), 0, _SHR_E_NOT_FOUND,
                       "Port is not cross-connected\n");
    }
    /*
     * Assign destination and OutLif/eei to return values.
     */
    if (temp_outlif != 0)
    {
        forward_info->fwd_info_result_type = DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF;
    }
    else if (temp_eei != 0)
    {
        forward_info->fwd_info_result_type = DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_EEI;
    }
    else
    {
        forward_info->fwd_info_result_type = DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_ONLY;
    }

    forward_info->outlif = temp_outlif;
    forward_info->eei = temp_eei;
    forward_info->destination = temp_destination;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Internal function to get destination and outlif
 * from a DBAL entry.
 * \param [in] unit - relevant unit
 * \param [in] gport_hw_resources - pointer to
 *        dnx_algo_gpm_gport_hw_resources_t structure holding
 *        the GPORT hardware resources information.
 * \param [out] forward_info - pointer to forward-info structure
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *  bcm_dnx_vswitch_cross_connect_get
 *  bcm_dnx_vswitch_cross_connect_delete
 */
shr_error_e
dnx_vswitch_cross_connect_get_apply(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_algo_gpm_forward_info_t * forward_info)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a new handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources->inlif_dbal_table_id, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources->local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources->inlif_dbal_result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_get_forward_info(unit, entry_handle_id, forward_info, TRUE, FALSE));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Internal function for attaching one direction of AC P2P service.
 * It updates the given AC IN-LIF entry in the LIF table with the the given
 * forwarding information (out-lif, destination).
 *
 * \param [in] unit - relevant unit.
 * \param [in] gport - the gport to be cross-connected.
 * \param [in] gport_hw_resources - pointer to
 *        dnx_algo_gpm_gport_hw_resources_t structure holding
 *        the GPORT hardware resources information.
 * \param [in] forward_info - pointer to structure type
 *        dnx_algo_gpm_forward_info_t holding the pre-fec
 *        forward information.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 * bcm_dnx_vswitch_cross_connect_add
 */
static shr_error_e
dnx_vswitch_cross_connect_ac_add_direction_apply(
    int unit,
    bcm_gport_t gport,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_algo_gpm_forward_info_t * forward_info)
{
    uint32 entry_handle_id;
    lif_table_mngr_inlif_info_t lif_info;
    dnx_algo_gpm_forward_info_t current_forward_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get current forward-info */

    /** Taking a new handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources->inlif_dbal_table_id, &entry_handle_id));

    /*
     * Set keys and gets all fields:
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_inlif_info
                    (unit, _SHR_CORE_ALL, gport_hw_resources->local_in_lif, entry_handle_id, &lif_info));

    SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_get_forward_info
                    (unit, entry_handle_id, &current_forward_info, FALSE, TRUE));

    /*
     * Compare current and new forward-info type.
     * If not equal, need to perform REPLACE:
     */
    if (current_forward_info.fwd_info_result_type != forward_info->fwd_info_result_type)
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_replace_ingress_p2p_to_p2p(unit, gport, entry_handle_id, forward_info));
    }
    else
    {
        uint32 new_entry_handle_id;
        /*
         * Take table handle:
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources->inlif_dbal_table_id, &new_entry_handle_id));
        /*
         * Set key:
         */
        dbal_entry_key_field32_set(unit, new_entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources->local_in_lif);
        /*
         * Set values:
         */
        dbal_entry_value_field32_set(unit, new_entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     gport_hw_resources->inlif_dbal_result_type);

        if (forward_info->fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF)
        {
            dbal_entry_value_field32_set(unit, new_entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE,
                                         forward_info->outlif);
        }
        else if (forward_info->fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_EEI)
        {
            dbal_entry_value_field32_set(unit, new_entry_handle_id, DBAL_FIELD_EEI, INST_SINGLE, forward_info->eei);
        }
        else if (forward_info->fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_ONLY)
        {
            /*
             * For Destination only - no global-out-lif/eei
             */
        }
        dbal_entry_value_field32_set(unit, new_entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE,
                                     forward_info->destination);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, new_entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Internal function for deleting one port from relevant
 * table, port is taken from gport object and chosen by
 * value og port_num parameter.
 *
 * \param [in] unit - relevant unit
 * \param [in] gport - gport to delete it's P2P reference to peer.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *  bcm_dnx_vswitch_cross_connect_delete
 */
static shr_error_e
dnx_vswitch_cross_connect_delete_apply(
    int unit,
    bcm_gport_t gport)
{
    int rv;
    dbal_table_field_info_t field_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (_SHR_GPORT_IS_VLAN_PORT(gport) || _SHR_GPORT_IS_EXTENDER_PORT(gport))
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_replace_ingress_p2p_to_mp(unit, gport));
    }
    else
    {
        uint32 entry_handle_id;
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

        /*
         * gport: Get local In-LIF using DNX Algo Gport Managment:
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources));

        /** Taking a new handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));

        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     gport_hw_resources.inlif_dbal_result_type);

        /** Reseting added values in entry */
        rv = dbal_tables_field_info_get_no_err(unit, gport_hw_resources.inlif_dbal_table_id,
                                               DBAL_FIELD_GLOB_OUT_LIF, 0, gport_hw_resources.inlif_dbal_result_type, 0,
                                               &field_info);
        if (rv == _SHR_E_NONE)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, 0);
        }
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, 0);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Replaces a created Ingress P2P PWE, MPLS/extender Port to Ingress P2P P2P PWE, MPLS/extender Port -
 *  for the case were the new forward information is different then current forward information, the In-LIF
 *  is "replaced" to accommodate the new forward information.
 * \param [in] unit - Unit #
 * \param [in] entry_handle_id -table handler to be used to update the fields and commit.
 * \param [in] gport_hw_resources - pointer to
 *        dnx_algo_gpm_gport_hw_resources_t structure holding
 * \param [in] forward_info -pointer to the forward-information of the destiantion port.
 * \param [in] cur_fwd_info_result_type -result type of current forward-information.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 * \see
 * bcm_dnx_vswitch_cross_connect_add
 */
static shr_error_e
dnx_vswitch_cross_connect_result_update(
    int unit,
    uint32 entry_handle_id,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_algo_gpm_forward_info_t * forward_info,
    int cur_fwd_info_result_type)
{
    int local_inlif;
    lif_table_mngr_inlif_info_t lif_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&lif_info, 0, sizeof(lif_table_mngr_inlif_info_t));

    /*
     * Unset EEI and global-out-lif
     */
    if (cur_fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF)
    {
        dbal_entry_value_field_unset(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE);
    }
    else if (cur_fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_EEI)
    {
        dbal_entry_value_field_unset(unit, entry_handle_id, DBAL_FIELD_EEI, INST_SINGLE);
    }

    /*
     * Set global-out-lif or eei
     */
    if (forward_info->fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, forward_info->outlif);
    }
    else if (forward_info->fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_EEI)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EEI, INST_SINGLE, forward_info->eei);
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, forward_info->destination);

    /*
     * "replace" the local-inlif and write its info to HW
     */
    lif_info.global_lif = gport_hw_resources->global_in_lif;

    if (gport_hw_resources->global_in_lif == DNX_ALGO_GPM_LIF_INVALID)
    {
        lif_info.flags = LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING;
    }

    lif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_REPLACE;

    local_inlif = gport_hw_resources->local_in_lif;

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_local_inlif_and_set_hw
                    (unit, _SHR_CORE_ALL, entry_handle_id, &local_inlif, &lif_info));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Internal function for attaching one direction of non-AC P2P service.
 * It updates the given non-AC IN-LIF entry in the LIF table with the the given
 * forwarding information (out-lif, destination).
 *
 * \param [in] unit - relevant unit.
 * \param [in] gport - the gport to be cross-connected.
 * \param [in] gport_hw_resources - pointer to
 *        dnx_algo_gpm_gport_hw_resources_t structure holding
 *        the GPORT hardware resources information.
 * \param [in] forward_info - pointer to structure type
 *        dnx_algo_gpm_forward_info_t holding the pre-fec
 *        forward information.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 * bcm_dnx_vswitch_cross_connect_add
 */
static shr_error_e
dnx_vswitch_cross_connect_add_direction_apply(
    int unit,
    bcm_gport_t gport,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_algo_gpm_forward_info_t * forward_info)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_forward_info_t current_forward_info;
    lif_table_mngr_inlif_info_t lif_info;
    shr_error_e rv;
    uint8 need_update_result = FALSE;
    uint8 fwd_info_is_eei = 0;
    uint8 cur_fwd_info_is_eei = 0;
    uint8 fwd_info_is_outlif = 0;
    uint8 cur_fwd_info_is_outlif = 0;
    uint32 temp_destination = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&current_forward_info, 0x0, sizeof(dnx_algo_gpm_forward_info_t));
    current_forward_info.fwd_info_result_type = DBAL_NOF_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO;

    /** Taking a new handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources->inlif_dbal_table_id, &entry_handle_id));

    /*
     * Set keys and gets all fields:
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_inlif_info
                    (unit, _SHR_CORE_ALL, gport_hw_resources->local_in_lif, entry_handle_id, &lif_info));

    /*
     * E_NOT_FOUND indicates the gport is cross connecting at first time
     */
    rv = dbal_entry_handle_value_field32_get
        (unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, &temp_destination);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_get_forward_info
                        (unit, entry_handle_id, &current_forward_info, FALSE, FALSE));
    }

    if ((current_forward_info.fwd_info_result_type != forward_info->fwd_info_result_type)
        && (gport_hw_resources->inlif_dbal_table_id == DBAL_TABLE_IN_LIF_IPvX_TUNNELS))
    {
        if (forward_info->outlif != BCM_FORWARD_ENCAP_ID_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Wrong value of encap2. It should be set to BCM_FORWARD_ENCAP_ID_INVALID for IPvX tunnels!\n");
        }
    }
    /*
     * Compare current and new forward-info type.
     * If it's not equal and 1st time to add cross connecting,
     * need to perform REPLACE.
     */
    if (current_forward_info.fwd_info_result_type != forward_info->fwd_info_result_type || rv == _SHR_E_NOT_FOUND)
    {
        need_update_result = TRUE;
    }

    /*
     * For IN_LIF_FORMAT_EVPN, needn't to update result because only have 1 P2P result
     * For IN_LIF_FORMAT_PWE, only need to update result by followed cases.
     *     1. old fwd info result type==DEST_EEI and new fwd info result type!=DEST_EEI
     *     2. old fwd info result type!=DEST_EEI and new fwd info result type==DEST_EEI
     *     3. old fwd info result type==DEST_OUTLIF and new fwd info result type!=DEST_OUTLIF
     *     4. old fwd info result type!=DEST_OUTLIF and new fwd info result type==DEST_OUTLIF
     */
    if (gport_hw_resources->inlif_dbal_table_id == DBAL_TABLE_IN_LIF_FORMAT_PWE)
    {
        cur_fwd_info_is_eei =
            (current_forward_info.fwd_info_result_type ==
             DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_EEI ? 1 : 0);
        fwd_info_is_eei =
            (forward_info->fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_EEI ? 1 : 0);

        cur_fwd_info_is_outlif = (current_forward_info.fwd_info_result_type ==
                                  DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF ? 1 : 0);
        fwd_info_is_outlif =
            (forward_info->fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF ? 1 : 0);
        if (!(cur_fwd_info_is_eei ^ fwd_info_is_eei) && !(cur_fwd_info_is_outlif ^ fwd_info_is_outlif))
        {
            need_update_result = FALSE;
        }
    }
    else if (gport_hw_resources->inlif_dbal_table_id == DBAL_TABLE_IN_LIF_FORMAT_EVPN)
    {
        need_update_result = FALSE;
    }

    if (need_update_result)
    {
        SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_result_update
                        (unit, entry_handle_id, gport_hw_resources, forward_info,
                         current_forward_info.fwd_info_result_type));
    }
    else
    {
        uint32 new_entry_handle_id;

        /*
         * Take table handle:
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources->inlif_dbal_table_id, &new_entry_handle_id));

        /*
         * Set key:
         */
        dbal_entry_key_field32_set(unit, new_entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources->local_in_lif);
        /*
         * Set values:
         */
        dbal_entry_value_field32_set(unit, new_entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     gport_hw_resources->inlif_dbal_result_type);

        if (forward_info->fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF)
        {
            dbal_entry_value_field32_set(unit, new_entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE,
                                         forward_info->outlif);
        }
        else if (forward_info->fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_EEI)
        {
            dbal_entry_value_field32_set(unit, new_entry_handle_id, DBAL_FIELD_EEI, INST_SINGLE, forward_info->eei);
        }

        /*
         * Always need dest field for P2P service
         */
        dbal_entry_value_field32_set(unit, new_entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE,
                                     forward_info->destination);
        /*
         * For IN_LIF_FORMAT_PWE and IN_LIF_FORMAT_EVPN,
         * Use same result when fwd information result type is DEST_OUTLIF and DEST_ONLY.
         * So clear outlif when result type change DEST_OUTLIF to DEST_ONLY.
         */
        if ((gport_hw_resources->inlif_dbal_table_id == DBAL_TABLE_IN_LIF_FORMAT_PWE) ||
            (gport_hw_resources->inlif_dbal_table_id == DBAL_TABLE_IN_LIF_FORMAT_EVPN))
        {
            if ((current_forward_info.fwd_info_result_type ==
                 DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF)
                && (forward_info->fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_ONLY))
            {
                dbal_entry_value_field32_set(unit, new_entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, 0);
            }
        }

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, new_entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Internal function for traversing P2P sevices
 * \param [in] unit - relevant unit.
 * \param [in] table_id - table to traverse, can be:
 *                             DBAL_TABLE_IN_AC_INFO_DB
 *                             DBAL_TABLE_IN_LIF_FORMAT_PWE
 *                             DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION
 * \param [in] cb - Callback function to perform
 * \param [in] user_data - pointer to additional user data.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 * bcm_dnx_vswitch_cross_connect_traverse
 */
static shr_error_e
dnx_vswitch_cross_connect_traverse_apply(
    int unit,
    dbal_tables_e table_id,
    bcm_vswitch_cross_connect_traverse_cb cb,
    void *user_data)
{
    uint32 entry_handle_id;
    uint32 drop_in_lif;
    uint32 initial_bridge_in_lif;
    uint32 recycle_default_inlif;
    int is_end;
    uint32 local_in_lif;
    uint32 hw_resource;
    bcm_vswitch_cross_connect_t gports;
    int is_p2p;
    uint32 vtt5_default_in_lif;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Look for In-LIF data in relevant hw_resource
     */
    hw_resource = (table_id == DBAL_TABLE_IN_AC_INFO_DB) ?
        DNX_ALGO_GPM_GPORT_HW_RESOURCES_SBC_LOCAL_LIF_INGRESS : DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS;

    /*
     * Taking a handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    /*
     * Add rules to avoid traverse of default non-native entries
     */
    if (table_id == DBAL_TABLE_IN_AC_INFO_DB)
    {
        drop_in_lif = dnx_data_lif.in_lif.drop_in_lif_get(unit);
        SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                        (unit, entry_handle_id, DBAL_FIELD_IN_LIF, DBAL_CONDITION_NOT_EQUAL_TO, &drop_in_lif, NULL));

        SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_initial_bridge_lif_get(unit, (int *) &initial_bridge_in_lif));
        SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                        (unit, entry_handle_id, DBAL_FIELD_IN_LIF, DBAL_CONDITION_NOT_EQUAL_TO, &initial_bridge_in_lif,
                         NULL));

        SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_default_recyle_lif_get(unit, &recycle_default_inlif));
        SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                        (unit, entry_handle_id, DBAL_FIELD_IN_LIF, DBAL_CONDITION_NOT_EQUAL_TO, &recycle_default_inlif,
                         NULL));
    }

    /*
     * All the entries (that is all in-lifs) that are not created by default (see dnx_vlan_native_vpls_service_tagged_miss_init):
     *  - PWE tagged native miss lif
     *  - 0 tags native miss lif
     */
    if (table_id == DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION)
    {
        uint32 native_default_handle_id;

        /*
         * Get lif value from virtual register of vtt5 native default trap
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_VTT5_DEFAULT_LIF, &native_default_handle_id));
        dbal_value_field32_request(unit, native_default_handle_id, DBAL_FIELD_VTT5___NATIVE_ETHERNET___TRAP_DEFAULT_LIF,
                                   INST_SINGLE, &vtt5_default_in_lif);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, native_default_handle_id, DBAL_COMMIT));

        SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                        (unit, entry_handle_id, DBAL_FIELD_IN_LIF, DBAL_CONDITION_NOT_EQUAL_TO, &vtt5_default_in_lif,
                         NULL));
    }
    else if (table_id == DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_VLAN_EDIT)
    {
        uint32 native_default_handle_id;

        /*
         * Get lif value from virtual register of vtt5 native default 0 tags
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_VTT5_DEFAULT_LIF, &native_default_handle_id));
        dbal_value_field32_request(unit, native_default_handle_id,
                                   DBAL_FIELD_VTT5___NATIVE_ETHERNET___0_TAGS_DEFAULT_LIF, INST_SINGLE,
                                   &vtt5_default_in_lif);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, native_default_handle_id, DBAL_COMMIT));

        SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                        (unit, entry_handle_id, DBAL_FIELD_IN_LIF, DBAL_CONDITION_NOT_EQUAL_TO, &vtt5_default_in_lif,
                         NULL));
    }

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        /*
         * Initialize gport
         */
        sal_memset(&gports, 0x0, sizeof(bcm_vswitch_cross_connect_t));

        SHR_IF_ERR_EXIT(dnx_vswitch_entry_is_p2p(unit, entry_handle_id, &is_p2p));

        /*
         * Verify lif is P2P
         */
        if (is_p2p == TRUE)
        {
            /*
             * Get local In-LIF -- table key
             */
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_IN_LIF, &local_in_lif));

            /*
             * Get gport from In-LIF
             */
            if (table_id == DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_VLAN_EDIT)
            {

                /*
                 * The local in-lif is "virtual", look for it in the SW DB
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_native_virtual_match_sw_state(unit, (int) local_in_lif,
                                                                                         &(gports.port1)));
            }
            else
            {

                SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                                (unit, hw_resource, DBAL_CORE_ALL, local_in_lif, &(gports.port1)));
            }

            /*
             * since traverse will look up all table and all gports, so always with flag
             * BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL 
             */
            gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
            /*
             * Get connected gport
             */
            SHR_IF_ERR_REPLACE_AND_EXIT(bcm_dnx_vswitch_cross_connect_get(unit, &gports),
                                        _SHR_E_NOT_FOUND, _SHR_E_NONE);

            /*
             * Make sure Out-LIF is connected
             */
            if (gports.port2 != 0)
            {
                /*
                 * Run callback function
                 */
                SHR_IF_ERR_EXIT(cb(unit, &gports, user_data));
            }
        }

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * BCM API: Create a Virtual Switching Instance.
 *
 * \param [in] unit - Unit id
 * \param [out] vsi - pointer to Virtual Switching Instance
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_vswitch_create(
    int unit,
    bcm_vlan_t * vsi)
{
    int allocated_vsi = 0;
    uint32 flags;
    uint8 replaced = 0;
    bcm_stg_t stg_defl = BCM_STG_INVALID;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_vswitch_create_verify(unit, vsi));

    /*
     * DNX SW Algorithm, allocate VSI.
     */
    flags = 0;
    SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_alloc(unit, flags, _bcmDnxVsiTypeVswitch, &allocated_vsi, &replaced));

    *vsi = allocated_vsi;

    if (!replaced)
    {
        /*
         * Write to HW VSI table
         */
        SHR_IF_ERR_EXIT(dnx_vsi_table_default_set(unit, allocated_vsi));

        /*
         * Add the vsi to default STG
         */
        SHR_IF_ERR_EXIT(bcm_dnx_stg_default_get(unit, &stg_defl));
        SHR_IF_ERR_EXIT(bcm_dnx_stg_vlan_add(unit, stg_defl, *vsi));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * BCM API: Create a Virtual Switching Instance.
 * The ID of the VSI is an input parameter: vsi.
 *
 * \param [in] unit - relevant unit.
 * \param [in] vsi - the vsi ID.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_vswitch_create_with_id(
    int unit,
    bcm_vlan_t vsi)
{
    int allocated_vsi;
    uint8 replaced = 0;
    bcm_stg_t stg_defl = BCM_STG_INVALID;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_vswitch_create_with_id_verify(unit, vsi));
    allocated_vsi = vsi;
    /*
     * DNX SW Algorithm, allocate VSI.
     */
    SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_alloc
                    (unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, _bcmDnxVsiTypeVswitch, &allocated_vsi, &replaced));

    if (!replaced)
    {
        /*
         * Write to HW VSI table (only if required)
         */
        SHR_IF_ERR_EXIT(dnx_vsi_table_default_set(unit, allocated_vsi));

        /*
         * Add the vsi to default STG
         */
        SHR_IF_ERR_EXIT(bcm_dnx_stg_default_get(unit, &stg_defl));
        SHR_IF_ERR_EXIT(bcm_dnx_stg_vlan_add(unit, stg_defl, vsi));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * BCM API: attachs given 2 ports in P2P service
 * \param [in] unit - relevant unit.
 * \param [in] gports - pointer to structure type
 *        bcm_vswitch_cross_connect_t which holds the
 *        information about the two ports to attached to P2P
 *        service:
 *          - port1 - First gport in cross connect.
 *          - port2 - Second gport in cross connect.
 *          - encap1 - First gport encap id.
 *          - encap2 - Second gport encap id.
 *          - flags - BCM_VSWITCH_CROSS_CONNECT_XXX flags.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *  In case the LIF is not P2P LIF (that is it is MP LIF), this function changes it to P2P LIF!
 *  LIF field that exists on both MP-LIF and the new P2P-LIF - it's value is kept (unless fields range differ thus default value
 *  is taken).
 *  For MP-LIF field that is absent in the new P2P-LIF - it's value is gone and it is the user  responsibility to set it if the LIF
 *  is changed back to be MP LIF (using bcm_dnx_vswitch_cross_connect_delete).
 *  For new P2P-LIF fields (that is fields that are absent in the MP-LIF) - default value is set.
 * \see
 *   dnx_vlan_port_replace_mp_to_p2p
 */
shr_error_e
bcm_dnx_vswitch_cross_connect_add(
    int unit,
    bcm_vswitch_cross_connect_t * gports)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources1;
    dnx_algo_gpm_forward_info_t forward_info2;
    int is_p2p;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_vswitch_cross_connect_verify(unit, gports));

    /** Verify port1 ==> port2 */
    SHR_INVOKE_VERIFY_DNX(dnx_vswitch_cross_connect_port_verify(unit, gports->port1, gports->port2));

    if ((_SHR_IS_FLAG_SET(gports->flags, BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL) == FALSE))
    {
        /** Verify port2 ==> port1 */
        SHR_INVOKE_VERIFY_DNX(dnx_vswitch_cross_connect_port_verify(unit, gports->port2, gports->port1));
    }

    /*
     * 1. Configure gport1 ==> gport2:
     * Update In-LIF (P2P) of gport1 with FORWARD INFO of gport2
     */

    /*
     * gport1: Get local In-LIF using DNX Algo Gport Managment:
     */
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, gports->port1, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                 &gport_hw_resources1), _SHR_E_NOT_FOUND, _SHR_E_PARAM);

    /*
     * gport2: Resolve forward information:
     */
    SHR_IF_ERR_EXIT(algo_gpm_gport_and_encap_to_forward_information
                    (unit, gports->port2, gports->encap2, &forward_info2));

    /** For VLAN-PORT - LIF may not be P2P */
    if (_SHR_GPORT_IS_VLAN_PORT(gports->port1) || _SHR_GPORT_IS_EXTENDER_PORT(gports->port1))
    {
        /*
         * gport1: if it is not P2P LIF, perform "replace" to change it to P2P
         */
        SHR_IF_ERR_EXIT(dnx_vswitch_is_p2p(unit, gports->port1, &is_p2p));

        if (is_p2p == FALSE)
        {
            SHR_IF_ERR_EXIT(dnx_vlan_port_replace_ingress_mp_to_p2p(unit, gports->port1, &forward_info2));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_ac_add_direction_apply
                            (unit, gports->port1, &gport_hw_resources1, &forward_info2));
        }
    }
    else
    {
        /*
         * Update HW: direction gport1 ==> gport2
         */
        SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_add_direction_apply
                        (unit, gports->port1, &gport_hw_resources1, &forward_info2));
    }

    /*
     * Symmetrical cross-connect?
     */
    if ((_SHR_IS_FLAG_SET(gports->flags, BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL) == FALSE))
    {
        /*
         * 2. Configure gport2 ==> gport1:
         * Update In-LIF (P2P) of gport2 with FORWARD INFO of gport1
 */
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources2;
        dnx_algo_gpm_forward_info_t forward_info1;

        /*
         * gport2: Get local In-LIF using DNX Algo Gport Managment:
         */
        SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                    (unit, gports->port2, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                     &gport_hw_resources2), _SHR_E_NOT_FOUND, _SHR_E_PARAM);

        /*
         * gport1: Resolve forward information:
         */
        SHR_IF_ERR_EXIT(algo_gpm_gport_and_encap_to_forward_information
                        (unit, gports->port1, gports->encap1, &forward_info1));

        /*
         * For VLAN-PORT - LIF may not be P2P
         */
        if (_SHR_GPORT_IS_VLAN_PORT(gports->port2) || _SHR_GPORT_IS_EXTENDER_PORT(gports->port2))
        {
            /*
             * gport2: if it is not P2P LIF, perform "replace" to change it to P2P
             */
            SHR_IF_ERR_EXIT(dnx_vswitch_is_p2p(unit, gports->port2, &is_p2p));

            if (is_p2p == FALSE)
            {
                SHR_IF_ERR_EXIT(dnx_vlan_port_replace_ingress_mp_to_p2p(unit, gports->port2, &forward_info1));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_ac_add_direction_apply
                                (unit, gports->port2, &gport_hw_resources2, &forward_info1));
            }
        }
        else
        {
            /*
             * Update HW: direction gport2 ==> gport1
             */
            SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_add_direction_apply
                            (unit, gports->port2, &gport_hw_resources2, &forward_info1));
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * BCM API: For a given P2P gport, returns its P2P peer.
 * \param [in] unit - relevant unit.
 * \param [in, out] gports - pointer to structure type
 *        bcm_vswitch_cross_connect_t which holds the
 *        information about the port given and will contain information
 *        on peer port on return.
 *        service:
 *          - [in] port1 - given port.
 *          - [out] port2 - peer of port1, retun value.
 *          - [in] encap1 - First gport encap id.
 *          - [out] encap2 - Second gport encap id.
 *          - [in] flags - BCM_VSWITCH_CROSS_CONNECT_XXX flags.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_vswitch_cross_connect_get(
    int unit,
    bcm_vswitch_cross_connect_t * gports)
{
    dnx_algo_gpm_forward_info_t forward_info;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_vswitch_cross_connect_verify(unit, gports));

    /*
     * Verify port1 in gports struct is valid.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_vswitch_cross_connect_port_verify(unit, gports->port1, 0));

    /*
     * GPORT: Get local In-LIF using DNX Algo GPORT Managment:
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, gports->port1, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources));

    /*
     * gport1: Get destination and outlif from DBAL table with gports->port1 as key
     */
    SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_get_apply(unit, &gport_hw_resources, &forward_info));

    /*
     * gport2: Resolve forward information:
     */
    SHR_IF_ERR_EXIT(algo_gpm_gport_and_encap_from_forward_information
                    (unit, &(gports->port2), (uint32 *) &(gports->encap2), &forward_info, 0));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * BCM API: For a given P2P gport, delete it's cross connect entry.
 * \param [in] unit - relevant unit.
 * \param [in] gports - pointer to structure type
 *        bcm_vswitch_cross_connect_t which holds the
 *        information about the port given.
 *        service:
 *          - [in] port1 - given port.
 *          - [in] port2 - if flags=0, 2nd given port to delete.
 *          - [in] flags - BCM_VSWITCH_CROSS_CONNECT_XXX flags.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *  This function changes the P2P-LIF to MP LIF!
 *  LIF field that exists on both P2P-LIF and the new MP-LIF - it's value is kept (unless fields range differ thus default value
 *  is taken).
 *  For P2P-LIF field that is absent in the new MP-LIF - it's value is gone and it is the user  responsibility to set it if the LIF
 *  is changed back to be P2P LIF (using bcm_dnx_vswitch_cross_connect_add).
 *  For new MP-LIF fields (that is fields that are absent in the P2P-LIF) - default value is set.
 * \see
 *   dnx_vlan_port_replace_p2p_to_mp
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_vswitch_cross_connect_delete(
    int unit,
    bcm_vswitch_cross_connect_t * gports)
{
    dnx_algo_gpm_forward_info_t forward_info;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources1;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_vswitch_cross_connect_verify(unit, gports));

    /*
     * Verifying gport is valid
     */
    SHR_INVOKE_VERIFY_DNX(dnx_vswitch_cross_connect_port_verify(unit, gports->port1, gports->port2));

    /*
     * Symmetrical cross-connect?
     */
    if ((_SHR_IS_FLAG_SET(gports->flags, BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL) == FALSE))
    {
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources2;
        /*
         * Verifying gport is valid
         */
        SHR_INVOKE_VERIFY_DNX(dnx_vswitch_cross_connect_port_verify(unit, gports->port2, gports->port1));

        /*
         * GPORT: Get local In-LIF using DNX Algo GPORT Managment:
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, gports->port2, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources2));

        /*
         * Verifying gport exists as P2P
         */
        SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_get_apply(unit, &gport_hw_resources2, &forward_info));

        SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_delete_apply(unit, gports->port2));
    }

    /*
     * GPORT: Get local In-LIF using DNX Algo GPORT Managment:
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, gports->port1, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources1));

    /*
     * Verifying gport exists as P2P
     */
    SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_get_apply(unit, &gport_hw_resources1, &forward_info));

    SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_delete_apply(unit, gports->port1));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * BCM API: Traverse on all existing P2P services and preform given Callback function.
 * \param [in] unit - relevant unit.
 * \param [in] cb - Callback function to perform
 * \param [in] user_data - pointer to additional user data.
 *
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_vswitch_cross_connect_traverse(
    int unit,
    bcm_vswitch_cross_connect_traverse_cb cb,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_NULL_CHECK(cb, _SHR_E_PARAM, "callback");

    SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_traverse_apply(unit, DBAL_TABLE_IN_AC_INFO_DB, cb, user_data));
    SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_traverse_apply(unit, DBAL_TABLE_IN_LIF_FORMAT_PWE, cb, user_data));
    SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_traverse_apply
                    (unit, DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION, cb, user_data));
    SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_traverse_apply
                    (unit, DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_VLAN_EDIT, cb, user_data));
    SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_traverse_apply(unit, DBAL_TABLE_IN_LIF_IPvX_TUNNELS, cb, user_data));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * internal cb function for BCM-API:
 * bcm_dnx_vswitch_cross_connect_delete_all
 * \param [in] unit - relevant unit
 * \param [in] gports - pointer to structure type
 *        bcm_vswitch_cross_connect_t which holds the
 *        information about the P2P entry to delete
 * \param [in] user_data - unused pointer added becuase
 *           of function typedef
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *  bcm_dnx_vswitch_cross_connect_delete_all
 *  bcm_dnx_vswitch_cross_connect_delete
 */
static shr_error_e
dnx_vswitch_cross_connect_delete_all_cb(
    int unit,
    bcm_vswitch_cross_connect_t * gports,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set Cross Connect one direction flag.
     */
    gports->flags |= BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;

    SHR_IF_ERR_REPLACE_AND_EXIT(bcm_dnx_vswitch_cross_connect_delete(unit, gports), _SHR_E_NOT_FOUND, _SHR_E_NONE);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * BCM API: Delete all P2P LIFs.
 * \param [in] unit - relevant unit.
 *
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_vswitch_cross_connect_delete_all(
    int unit)
{
    uint32 additional_data = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_IF_ERR_EXIT(bcm_dnx_vswitch_cross_connect_traverse
                    (unit, dnx_vswitch_cross_connect_delete_all_cb, &additional_data));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * BCM API: Destroy a Virtual Switching Instance.
 * \param [in] unit - relevant unit.
 * \param [in] vsi - vsi to destroy.
 *
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_vswitch_destroy(
    int unit,
    bcm_vlan_t vsi)
{
    uint8 used = 0;
    bcm_stg_t stg;

    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_vswitch_destroy_verify(unit, vsi));

    /*
     * Remove vsi from its spanning tree group, and don't add it to default STG.
     */
    SHR_IF_ERR_EXIT(bcm_dnx_vlan_stg_get(unit, vsi, &stg));
    SHR_IF_ERR_EXIT(dnx_stg_vlan_remove(unit, stg, vsi, FALSE));

    /*
     * DNX SW Algorithm, free VSI.
     */
    SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_dealloc(unit, _bcmDnxVsiTypeVswitch, (int) vsi));

    /*
     *  check if this VSI is also used by other type of services
     */
    SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_check_except_type(unit, vsi, _bcmDnxVsiTypeVswitch, &used));
    if (!used)
    {
        /*
         * Clear HW VSI table entry
         */
        SHR_IF_ERR_EXIT(dnx_vsi_table_clear_set(unit, vsi));
    }

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}
