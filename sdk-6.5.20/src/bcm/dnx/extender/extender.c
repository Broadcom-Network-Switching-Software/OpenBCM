/** \file extender.c
 * 
 *
 * Port Extender procedures for DNX.
 * Allows to Set/Get/Delete Port extender entries.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_EXTENDER

/*
 * Include files.
 * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>

#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm_int/dnx/port/port_sit.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/extender/extender.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr_api.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/stat/stat_pp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fc.h>

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

/*
 * Inner functions
 * {
 */

/*
 * See header file of bcm_int/dnx/extender/extender.h for description.
 */

shr_error_e
dnx_extender_init(
    int unit)
{
    shr_error_e rv = _SHR_E_UNAVAIL;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header file of bcm_int/dnx/extender/extender.h for description.
 */

shr_error_e
dnx_extender_deinit(
    int unit)
{
    shr_error_e rv = _SHR_E_UNAVAIL;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verfity the extender_port according to the extender adding requirement
 *   \param [in] unit       -  Relevant unit.
 *   \param [in] extender_port  - -  Main struct of the API. Holds all the relevant information.
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */

static shr_error_e
dnx_extender_port_add_verify(
    int unit,
    bcm_extender_port_t * extender_port)
{

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(extender_port, _SHR_E_PARAM, "extender_port");

    /*
     * Check that only supported flags are set
     */
    if ((extender_port->flags) &
        (~
         (BCM_EXTENDER_PORT_WITH_ID | BCM_EXTENDER_PORT_REPLACE | BCM_EXTENDER_PORT_INGRESS_WIDE |
          BCM_EXTENDER_PORT_MULTICAST | BCM_EXTENDER_PORT_INITIAL_VLAN)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported Extender Add flags");
    }

    /*
     * If replace flag is set, with_id flag must also be set
     */
    if (_SHR_IS_FLAG_SET(extender_port->flags, BCM_EXTENDER_PORT_REPLACE)
        && !_SHR_IS_FLAG_SET(extender_port->flags, BCM_EXTENDER_PORT_WITH_ID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_EXTENDER_PORT_REPLACE can't be used without BCM_EXTENDER_PORT_WITH_ID as well");
    }

    /** then check global_lif should not be invalid for replace */
    if (_SHR_IS_FLAG_SET(extender_port->flags, BCM_EXTENDER_PORT_REPLACE)
        && (extender_port->extender_port_id == BCM_GPORT_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "REPLACE operation should not with invalid global_lif");
    }

    if (extender_port->flags & BCM_EXTENDER_PORT_INITIAL_VLAN)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM_EXTENDER_PORT_INITIAL_VLAN doest't support on JR2, use criteria BCM_EXTENDER_PORT_MATCH_PORT_EXTENDED_UNTAGGED");
    }

    /*
     * Validate the VLANs
     */
    BCM_DNX_VLAN_CHK_ID(unit, extender_port->extended_port_vid);
    /*
     * In Multicast only OutLif is created, hence match_vlan is irrelevant
     */
    if (!_SHR_IS_FLAG_SET(extender_port->flags, BCM_EXTENDER_PORT_MULTICAST))
    {
        BCM_DNX_VLAN_CHK_ID(unit, extender_port->match_vlan);
        BCM_DNX_VLAN_CHK_ID(unit, extender_port->match_inner_vlan);

        if ((extender_port->criteria < BCM_EXTENDER_PORT_MATCH_PORT_EXTENDED_PORT_VLAN)
            || (extender_port->criteria > BCM_EXTENDER_PORT_MATCH_PORT_EXTENDED_UNTAGGED))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Match_cirteria is not support!");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verfity the extender_port according to the extender getting requirement
 *   \param [in] unit           -  Relevant unit.
 *   \param [in] extender_port  -  Main struct of the API. Holds all the relevant information.
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_extender_port_get_verify(
    int unit,
    bcm_extender_port_t * extender_port)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(extender_port, _SHR_E_PARAM, "extender_port");

    SHR_IF_ERR_EXIT(BCM_GPORT_IS_EXTENDER_PORT(extender_port->extender_port_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verfity the extender gport according to the extender deleting requirement
 *   \param [in] unit              -  Relevant unit.
 *   \param [in] extender_port_id  -  gport ID, it indicates which extend port will be deleted.
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_extender_port_delete_verify(
    int unit,
    bcm_gport_t extender_port_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(BCM_GPORT_IS_EXTENDER_PORT(extender_port_id));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_extender_mapping_info_set_verify(
    int unit,
    uint32 flags,
    bcm_port_extender_mapping_type_t type,
    bcm_port_extender_mapping_info_t * mapping_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(mapping_info, _SHR_E_PARAM, "mapping_info");

    if (type == bcmPortExtenderMappingTypePonTunnel)
    {
        if (!(flags & (BCM_PORT_EXTENDER_MAPPING_INGRESS | BCM_PORT_EXTENDER_MAPPING_EGRESS)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Both ingress and egress Flags is needed!");
        }
        if (PON_SUBPORT_ID_GET(mapping_info->tunnel_id) > DNX_MAX_PON_SUBPORT_ID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "PON_SUBPORT_ID of PON out of range[0~3]!");
        }
    }
    else if (type == bcmPortExtenderMappingTypePortVlan)
    {
        if (!dnx_data_fc.coe.fc_support_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Flow Control doesn't work on COE port on this device!");
        }

        if (!(flags & (BCM_PORT_EXTENDER_MAPPING_INGRESS | BCM_PORT_EXTENDER_MAPPING_EGRESS)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Both ingress and egress Flags is needed!");
        }
        if (mapping_info->vlan > DNX_MAX_COE_VID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "VID of COE out of range[0~63]!");
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Mapping Type isn't supported");
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_extender_mapping_info_get_verify(
    int unit,
    uint32 flags,
    bcm_port_extender_mapping_type_t type,
    bcm_port_extender_mapping_info_t * mapping_info)
{
    int enable;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(mapping_info, _SHR_E_PARAM, "mapping_info");

    if (type == bcmPortExtenderMappingTypePonTunnel)
    {
        SHR_IF_ERR_EXIT(dnx_port_sit_pon_enable_get(unit, mapping_info->phy_port, &enable));
        if (!enable)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Port is not enabled for PON!");
        }
        if (flags & (BCM_PORT_EXTENDER_MAPPING_INGRESS | BCM_PORT_EXTENDER_MAPPING_EGRESS))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Flags is not support!");
        }
    }
    else if (type == bcmPortExtenderMappingTypePortVlan)
    {
        SHR_IF_ERR_EXIT(dnx_port_sit_coe_enable_get(unit, mapping_info->phy_port, &enable));
        if (!enable)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Port is not enabled for COE!");
        }
        if (flags & (BCM_PORT_EXTENDER_MAPPING_INGRESS | BCM_PORT_EXTENDER_MAPPING_EGRESS))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Flags is not support!");
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Mapping Type isn't supported");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Retreive a global LIF value,
 * According to the information in bcm_extender_port_t
 * A global LIF could be indicated by encap_id,
 * or extender_port_id
 *   \param [in] unit       -  Relevant unit.
 *   \param [in] extender_port  - -  Main struct of the API. Holds all the relevant information.
 *   \param [out] specified_global_lif_id  - specified global lif ID
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None

 */

static shr_error_e
dnx_extender_get_specified_global_lif(
    int unit,
    bcm_extender_port_t * extender_port,
    int *specified_global_lif_id)
{
    uint8 with_id, encap_with_id;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set default return values
     */
    *specified_global_lif_id = DNX_ALGO_GPM_LIF_INVALID;

    /*
     * Retrieve the relevant flags
     */
    with_id = (extender_port->flags & BCM_EXTENDER_PORT_WITH_ID) ? TRUE : FALSE;
    encap_with_id = (extender_port->flags & BCM_EXTENDER_PORT_ENCAP_WITH_ID) ? TRUE : FALSE;

    /*
     * If the encap_with_id flag is set, the Global LIF-ID is retrieved from the encap_id
     */
    if (encap_with_id)
    {
        if (extender_port->encap_id == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "When BCM_EXTENDER_PORT_ENCAP_WITH_ID is set, encap_id could not be 0");
        }

        *specified_global_lif_id = BCM_ENCAP_ID_GET(extender_port->encap_id);
    }
    /*
     * Otherwise, if with_id, retrieve the Global LIF-ID from the extender_port_id
     */
    else if (with_id)
    {
        if (extender_port->extender_port_id == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "When BCM_EXTENDER_PORT_WITH_ID is set, extender_port_id could not be 0");
        }

        *specified_global_lif_id = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(extender_port->extender_port_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Maps a gport to the accompanying forwarding information.
*
*   \param [in] unit       -  Relevant unit.
*   \param [in] extender_port  -  Holds all the relevant information on a AC LIF for extender port .
* \return
*   \retval  Zero if no error was detected
*   \retval  Negative if error was detected. See \ref shr_error_e
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_extender_port_gport_to_forward_information_set(
    int unit,
    bcm_extender_port_t * extender_port)
{
    dnx_algo_gpm_forward_info_t forward_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&forward_info, 0, sizeof(dnx_algo_gpm_forward_info_t));

    /*
     * Forwarding is done according to destination and Outlif (out LIF + port)
     */
    forward_info.fwd_info_result_type = DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF;
    forward_info.outlif = BCM_L3_ITF_VAL_GET(extender_port->encap_id);
    SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                    (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, extender_port->port, &forward_info.destination));

    /*
     * Fill destination (from Gport) info Forward Info table (SW state)
     */
    SHR_IF_ERR_EXIT(algo_gpm_gport_l2_forward_info_add(unit,
                                                       _SHR_IS_FLAG_SET(extender_port->flags,
                                                                        BCM_EXTENDER_PORT_REPLACE),
                                                       extender_port->extender_port_id, &forward_info));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Get forwarding information from extender gport.
*
*   \param [in] unit       -  Relevant unit.
*   \param [in] extender_port  -  Holds all the relevant information on a AC LIF for extender port .
* \return
*   \retval  Zero if no error was detected
*   \retval  Negative if error was detected. See \ref shr_error_e
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_extender_port_gport_to_forward_information_get(
    int unit,
    bcm_extender_port_t * extender_port)
{
    dnx_algo_gpm_forward_info_t forward_info;
    bcm_gport_t gport;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_gpm_gport_and_encap_to_forward_information
                    (unit, extender_port->extender_port_id, BCM_FORWARD_ENCAP_ID_INVALID, &forward_info));

    if (forward_info.fwd_info_result_type != DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "result_type (%d) is not supported for get api \r\n",
                     forward_info.fwd_info_result_type);
    }

    BCM_L3_ITF_SET(extender_port->encap_id, BCM_L3_ITF_TYPE_LIF, forward_info.outlif);
    SHR_IF_ERR_EXIT(algo_gpm_gport_from_encoded_destination_field
                    (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, forward_info.destination, &gport));

    if (BCM_GPORT_IS_TRUNK(gport))
    {
        extender_port->port = gport;
    }
    else if (BCM_GPORT_IS_SYSTEM_PORT(gport))
    {
        /*
         * Remove type prefix for DPP System-Port
         */
        extender_port->port = BCM_GPORT_SYSTEM_PORT_ID_GET(gport);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Fill externder port match information SW state.
 * \param [in] unit -
 *     unit number.
 * \param [in] local_inlif - local inlif is the key to the db.
 * \param [in] port - default port for untagged
 * \param [in] match - extender port match criteria
 * \param [in] vlan_domain  - vlan domain  for extender port SEM lookup
 * \param [in] ecid - E-Channel ID
 * \param [in] match_vlan - vlan ID
 * \param [in] match_inner_vlan - match inner Vlan ID
 *
 * \return
 *   shr_error_e - Non-zero in case of an error.
 * \remark
 *   Used in _get apis to find the match info according to lif
 *   id
 * \see
 *   dnx_extender_port_match_information_get
 *   dnx_extender_port_match_information_clear
 */
static shr_error_e
dnx_extender_port_match_information_set(
    int unit,
    int local_inlif,
    bcm_extender_port_match_t match,
    bcm_gport_t port,
    uint32 vlan_domain,
    uint16 ecid,
    uint16 match_vlan,
    uint16 match_inner_vlan)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Fill match information
     */

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_LIF, local_inlif);

    if (match == BCM_EXTENDER_PORT_MATCH_PORT_EXTENDED_PORT_VLAN)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_VD_E_CID_S_VLAN);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, INST_SINGLE, vlan_domain);
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_E_CID, INST_SINGLE, ecid);
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_S_VLAN, INST_SINGLE, match_vlan);
    }
    else if (match == BCM_EXTENDER_PORT_MATCH_PORT_EXTENDED_PORT_CVLAN)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_VD_E_CID_C_VLAN);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, INST_SINGLE, vlan_domain);
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_E_CID, INST_SINGLE, ecid);
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_C_VLAN_ID, INST_SINGLE, match_vlan);

    }
    else if (match == BCM_EXTENDER_PORT_MATCH_PORT_EXTENDED_PORT_VLAN_STACKED)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_VD_E_CID_S_VLAN_C_VLAN);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, INST_SINGLE, vlan_domain);
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_E_CID, INST_SINGLE, ecid);
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_S_VLAN, INST_SINGLE, match_vlan);
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_C_VLAN, INST_SINGLE, match_inner_vlan);
    }
    else if (match == BCM_EXTENDER_PORT_MATCH_PORT_EXTENDED_UNTAGGED)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_PORT_E_CID);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, (uint32) port);
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_E_CID, INST_SINGLE, ecid);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_FORCE));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get externder port match information SW state.
 * \param [in] unit -
 *     unit number.
 * \param [in] local_inlif - local inlif is the key to the db.
 * \param [out] port  - default port for untagged.
 * \param [out] vlan_domain  - vlan domain  for extender port SEM lookup
 * \param [out] ecid - E-Channel ID
 * \param [out] match_vlan - vlan ID
 * \param [out] match_inner_vlan - inner vlan ID if match criteria is EXTENDED_PORT_VLAN_STACKED
 * \param [out] criteria - match criteria.
 *
 * \return
 *   shr_error_e - Non-zero in case of an error.
 * \remark
 *   Used in _get apis to find the match info according to lif
 *   id
 * \see
 *   dnx_extender_port_match_information_get
 *   dnx_extender_port_match_information_clear
 */
static shr_error_e
dnx_extender_port_match_information_get(
    int unit,
    int local_inlif,
    bcm_gport_t * port,
    uint32 *vlan_domain,
    uint16 *ecid,
    uint16 *match_vlan,
    uint16 *match_inner_vlan,
    bcm_extender_port_match_t * criteria)
{
    uint32 entry_handle_id;
    uint32 result_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Fill match information
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_LIF, local_inlif);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &result_type));
    if (result_type == DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_VD_E_CID_S_VLAN)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, INST_SINGLE, vlan_domain));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field16_get
                        (unit, entry_handle_id, DBAL_FIELD_E_CID, INST_SINGLE, ecid));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field16_get
                        (unit, entry_handle_id, DBAL_FIELD_S_VLAN, INST_SINGLE, match_vlan));
        *criteria = BCM_EXTENDER_PORT_MATCH_PORT_EXTENDED_PORT_VLAN;
    }
    else if (result_type == DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_VD_E_CID_C_VLAN)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, INST_SINGLE, vlan_domain));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field16_get
                        (unit, entry_handle_id, DBAL_FIELD_E_CID, INST_SINGLE, ecid));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field16_get
                        (unit, entry_handle_id, DBAL_FIELD_C_VLAN_ID, INST_SINGLE, match_vlan));
        *criteria = BCM_EXTENDER_PORT_MATCH_PORT_EXTENDED_PORT_CVLAN;
    }
    else if (result_type == DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_VD_E_CID_S_VLAN_C_VLAN)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, INST_SINGLE, vlan_domain));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field16_get
                        (unit, entry_handle_id, DBAL_FIELD_E_CID, INST_SINGLE, ecid));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field16_get
                        (unit, entry_handle_id, DBAL_FIELD_S_VLAN, INST_SINGLE, match_vlan));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field16_get
                        (unit, entry_handle_id, DBAL_FIELD_C_VLAN, INST_SINGLE, match_inner_vlan));
        *criteria = BCM_EXTENDER_PORT_MATCH_PORT_EXTENDED_PORT_VLAN_STACKED;
    }
    else if (result_type == DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_PORT_E_CID)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, (uint32 *) port));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field16_get
                        (unit, entry_handle_id, DBAL_FIELD_E_CID, INST_SINGLE, ecid));

        *criteria = BCM_EXTENDER_PORT_MATCH_PORT_EXTENDED_UNTAGGED;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Result type (%d) not supported for port extender, wrong lif (%d) provided\n",
                     result_type, local_inlif);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Delete externder port match information SW state.
 * \param [in] unit -
 *     unit number.
 * \param [in] local_inlif - local inlif is the key to the db.
 *
 * \return
 *   shr_error_e - Non-zero in case of an error.
 * \remark
 *   Used in _get apis to find the match info according to lif
 *   id
 * \see
 *   dnx_extender_port_match_information_set
 *   dnx_extender_port_match_information_get
 */
static shr_error_e
dnx_extender_port_match_information_clear(
    int unit,
    int local_inlif)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Fill match information
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_LIF, local_inlif);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* Add extender port lookup entry to DBAL_TABLE_IN_BR_*_DB table.
* This function configures ISEM lookup and is done for all cores.
*
*   \param [in] unit    -  Relevant unit.
*   \param [in] port  -  pp port
*   \param [in] match - extender port match criteria
*   \param [in] ecid  - E-channel ID
*   \param [in] vid  -  VLAN id.
*   \param [in] inner_vid  - inner VLAN id if match criteria is EXTENDED_PORT_VLAN_STACKED
*   \param [in] local_in_lif  -  local in LIF id for extender port.
* \return
*   shr_error_e - Non-zero in case of an error.
*
* \remark
*
* \see
 */
static shr_error_e
dnx_extender_port_ingress_match_add(
    int unit,
    bcm_gport_t port,
    bcm_extender_port_match_t match,
    uint16 ecid,
    uint16 vid,
    uint16 inner_vid,
    int local_in_lif)
{
    uint32 entry_handle_id = 0;
    uint32 vlan_domain;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    int pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));

    if (match == BCM_EXTENDER_PORT_MATCH_PORT_EXTENDED_PORT_VLAN)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_BR_E_S_VLAN_DB, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_S_VID, vid);
    }
    else if (match == BCM_EXTENDER_PORT_MATCH_PORT_EXTENDED_PORT_CVLAN)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_BR_E_C_VLAN_DB, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, vid);
    }
    else if (match == BCM_EXTENDER_PORT_MATCH_PORT_EXTENDED_PORT_VLAN_STACKED)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_BR_E_S_C_VLAN_DB, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_S_VID, vid);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, inner_vid);
    }
    else if (match == BCM_EXTENDER_PORT_MATCH_PORT_EXTENDED_UNTAGGED)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_BR_E_VLAN_DB, &entry_handle_id));
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Can't support this match cirteria!\n");
    }

    if (match == BCM_EXTENDER_PORT_MATCH_PORT_EXTENDED_UNTAGGED)
    {
        /*
         * This "for" loop is used to support for Trunk
         * In case of Trunk, one pp_port for each core, so there should set twice for JR2
         */
        for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
        {
            uint32 core = gport_info.internal_port_pp_info.core_id[pp_port_index];
            uint32 pp_port = gport_info.internal_port_pp_info.pp_port[pp_port_index];
            uint32 mapped_pp_port = pp_port | (core << dnx_data_port.general.pp_port_bits_size_get(unit));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAPPED_PP_PORT, mapped_pp_port);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_E_CID, ecid);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_in_lif);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_FORCE));
        }
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_E_CID, ecid);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_in_lif);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_FORCE));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* Delete extender port lookup entry from DBAL_TABLE_IN_BR_*_DB table.
* This function configures ISEM lookup and is done for all cores.
* \par DIRECT INPUT:
*   \param [in] unit    -  Relevant unit.
*   \param [in] port    -  pp port
*   \param [in] match - extender port match criteria
*   \param [in] ecid    - E-channel ID
*   \param [in] vid     -  VLAN id.
*   \param [in] inner_vid - inner VLAN id if match criteria is EXTENDED_PORT_VLAN_STACKED.

* \return
*   shr_error_e - Non-zero in case of an error.
*
* \remark
*
* \see
 */
static shr_error_e
dnx_extender_port_ingress_match_delete(
    int unit,
    bcm_gport_t port,
    bcm_extender_port_match_t match,
    uint16 ecid,
    uint16 vid,
    uint16 inner_vid)
{
    uint32 entry_handle_id = 0;
    uint32 vlan_domain;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    int pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));

    if (match == BCM_EXTENDER_PORT_MATCH_PORT_EXTENDED_PORT_VLAN)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_BR_E_S_VLAN_DB, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_E_CID, ecid);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_S_VID, vid);
    }
    else if (match == BCM_EXTENDER_PORT_MATCH_PORT_EXTENDED_PORT_CVLAN)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_BR_E_C_VLAN_DB, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_E_CID, ecid);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, vid);
    }
    else if (match == BCM_EXTENDER_PORT_MATCH_PORT_EXTENDED_PORT_VLAN_STACKED)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_BR_E_S_C_VLAN_DB, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_E_CID, ecid);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_S_VID, vid);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, inner_vid);
    }
    else if (match == BCM_EXTENDER_PORT_MATCH_PORT_EXTENDED_UNTAGGED)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_BR_E_VLAN_DB, &entry_handle_id));
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
        /*
         * This "for" loop is used to support for Trunk
         * In case of Trunk, one pp_port for each core, so there should set twice for JR2
         */
        for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
        {
            uint32 core = gport_info.internal_port_pp_info.core_id[pp_port_index];
            uint32 pp_port = gport_info.internal_port_pp_info.pp_port[pp_port_index];
            uint32 mapped_pp_port = pp_port | (core << dnx_data_port.general.pp_port_bits_size_get(unit));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAPPED_PP_PORT, mapped_pp_port);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_E_CID, ecid);
            SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
        }
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* Get extender port lookup entry from eedb DBAL_TABLE_EEDB_OUT_AC_EXTENDER table.
* This function configures ISEM lookup and is done for all cores.
* \par DIRECT INPUT:
*   \param [in] unit    -  Relevant unit.
*   \param [in] gport_hw_resources   -  gort hw resource.
*   \param [out] extender_port  -  extender port encap infomation.
* \return
*   shr_error_e - Non-zero in case of an error.
*
* \remark
*
* \see
 */
static shr_error_e
dnx_extender_port_egress_eedb_get(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_extender_port_t * extender_port)
{
    uint32 entry_handle_id;
    uint32 result_type;
    uint32 sub_type_format;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources->outlif_dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, gport_hw_resources->local_out_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources->outlif_dbal_result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &result_type));
    if (result_type != DBAL_RESULT_TYPE_EEDB_OUT_AC_ETPS_AC_TRIPLE_TAG)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Result type (%d) not supported for port extender, wrong lif (%d) provided\n",
                     result_type, gport_hw_resources->local_out_lif);
    }
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SUB_TYPE_FORMAT, INST_SINGLE, &sub_type_format));
    if (sub_type_format == 0)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field16_get
                        (unit, entry_handle_id, DBAL_FIELD_ECID_OR_PON_TAG, INST_SINGLE,
                         &(extender_port->extended_port_vid)));
    }
    else
    {
        extender_port->extended_port_vid = 0;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Set Inlif table and update mapping for global and local inlif.
 *
 *   \param [in] unit -  Relevant unit.
 *   \param [in] flags - flag for inlif table allocate
 *   \param [in] global_lif - global inlif id
 *   \param [out] local_in_lif - allocated local inlif ID
 *   \param [in] is_update - indication of replace operation
 * \return
 *   shr_error_e
 *
 * \remark
 *   None
 */
static shr_error_e
dnx_extender_port_in_lif_lif_table_manager_add(
    int unit,
    uint32 flags,
    int global_lif,
    int *local_in_lif,
    int is_update)
{
    uint32 entry_handle_id;
    lif_table_mngr_inlif_info_t inlif_mngr;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_INFO_DB, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, INST_SINGLE, global_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, 0);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, INST_SINGLE, 0);

    if (!(flags & BCM_EXTENDER_PORT_INGRESS_WIDE))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_INFO_NO_LIF_BASIC_SYM_LIF_BASIC,
                                     INST_SINGLE, 0);
    }

    STAT_PP_CHECK_AND_SET_STAT_OBJECT_LIF_FIELDS(flags, BCM_EXTENDER_PORT_INGRESS_WIDE);
    if (flags & BCM_EXTENDER_PORT_INGRESS_WIDE)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTECTION_POINTER, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTECTION_PATH, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_GENERIC_DATA_0, INST_SINGLE, 0);
    }

    sal_memset(&inlif_mngr, 0, sizeof(lif_table_mngr_inlif_info_t));
    inlif_mngr.global_lif = global_lif;
    if (is_update)
    {
        inlif_mngr.flags = LIF_TABLE_MNGR_LIF_INFO_REPLACE;
    }

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_local_inlif_and_set_hw
                    (unit, _SHR_CORE_ALL, entry_handle_id, local_in_lif, &inlif_mngr));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Set EEDB table and update GlEM + mapping for global and local outlif.
 *
 *   \param [in] unit -  Relevant unit.
 *   \param [in] global_lif - global inlif id
 *   \param [out] local_out_lif - allocated local outlif ID
 *   \param [in] extender_port - All required out lif information
 *   \param [in] is_update - indication for replace operation.
 * \return
 *   shr_error_e
 *
 * \remark
 *   None
 */
static shr_error_e
dnx_extender_port_out_lif_lif_table_manager_add(
    int unit,
    int global_lif,
    int *local_out_lif,
    bcm_extender_port_t * extender_port,
    int is_update)
{
    uint32 entry_handle_id;
    lif_table_mngr_outlif_info_t outlif_mngr;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_OUT_AC, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SUB_TYPE_FORMAT, INST_SINGLE, 0);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECID_OR_PON_TAG, INST_SINGLE,
                                 extender_port->extended_port_vid);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_LAST_LAYER, INST_SINGLE, 1);

    sal_memset(&outlif_mngr, 0, sizeof(lif_table_mngr_outlif_info_t));
    outlif_mngr.outlif_phase = LIF_MNGR_OUTLIF_PHASE_AC;

    outlif_mngr.global_lif = global_lif;
    if (is_update)
    {
        outlif_mngr.flags |= LIF_TABLE_MNGR_LIF_INFO_REPLACE;
    }

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_local_outlif_and_set_hw
                    (unit, entry_handle_id, local_out_lif, &outlif_mngr));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Clear inlif and outlif table and sw table.
 *
 *   \param [in] unit -  Relevant unit.
 *   \param [in] gport_hw_resources - All required in lif information
 * \return
 *   shr_error_e
 *
 * \remark
 *   None
 */
static shr_error_e
dnx_extender_port_lif_table_manager_delete(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources)
{
    uint32 direction_flags;
    int global_lif;

    SHR_FUNC_INIT_VARS(unit);

    direction_flags = 0;
    global_lif = DNX_ALGO_GPM_LIF_INVALID;

    if (gport_hw_resources->local_in_lif != DNX_ALGO_GPM_LIF_INVALID)
    {
        /*
         * Free Lif table and local LIF allocation
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_inlif_info_clear
                        (unit, _SHR_CORE_ALL, gport_hw_resources->local_in_lif, gport_hw_resources->inlif_dbal_table_id,
                         0));

        direction_flags = DNX_ALGO_LIF_INGRESS;
        global_lif = gport_hw_resources->global_in_lif;

    }

    if (gport_hw_resources->local_out_lif != DNX_ALGO_GPM_LIF_INVALID)
    {
        /*
         * Free Lif table and local LIF allocation
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_outlif_info_clear(unit, gport_hw_resources->local_out_lif, 0));

        direction_flags |= DNX_ALGO_LIF_EGRESS;

        /*
         * Note: for symmetric LIF, global_out_lif is equal to global_in_lif thus no problem to overwrite it.
         */
        global_lif = gport_hw_resources->global_out_lif;
    }

    /*
     * Delete global lif - if valid (for example, for ingress native virtual, it is skipped)
     */
    if (global_lif != DNX_ALGO_GPM_LIF_INVALID)
    {
        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_free(unit, direction_flags, global_lif));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Create a Port Extender on ingress side.
 *
 * \param [in] unit       -  device unit number.
 * \param [in] global_lif       -  global LIF id.
 * \param [in] local_in_lif       -  local in LIF id.
 * \param [in] result_type       -  service type of LIF entry
 * \param [in] extender_port        -  Main struct of the API. Holds all the relevant information.
 *        [in]  extender_port.flags   - BCM_EXTENDER_PORT_WITH_ID Create an object with a given ID.
 *                                    - BCM_EXTENDER_PORT_REPLACE    Replace existing entry
 *                                    - BCM_EXTENDER_PORT_MULTICAST Create multicast type
 *        [inout]  extender_port.extender_port_id - GPORT identifier..
 *        [in]    extender_port.port - Physical port / trunk.
 *        [in]    extender_port.extended_port_vid - Extender port VID..
 *        [in]    extender_port.match_vlan - Outer VLAN ID to match.
 *        [in]    extender_port.pcp_de_select - Selection of PCP and DE fields for egress ETAG.
 *                             - BCM_EXTENDER_PCP_DE_SELECT_OUTER_TAG, From outer VLAN tag
 *                             - BCM_EXTENDER_PCP_DE_SELECT_INNER_TAG, From inner VLAN tag
 *                             - BCM_EXTENDER_PCP_DE_SELECT_DEFAULT, per port configuration
 *        [in]    extender_port.qos_map_id - Qos map id for egress etag mapping profilel.
 *        [out]   extender_port.encap_id - Encap identifier(OUT LIF ID for egress).
 *
 * \return
 *   shr_error_e - Non-zero in case of an error.
 * \remark
 *   Extender function should be enabled on physical port first, so that the whole function would work.
 * \see
 *   * None
 */
static shr_error_e
dnx_extender_port_ingress_add(
    int unit,
    int global_lif,
    int local_in_lif,
    dbal_enum_value_result_type_in_ac_info_db_e result_type,
    bcm_extender_port_t * extender_port)
{
    uint32 vlan_domain = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, extender_port->port, &vlan_domain));

    SHR_IF_ERR_EXIT(dnx_extender_port_match_information_set
                    (unit, local_in_lif, extender_port->criteria, extender_port->port, vlan_domain,
                     extender_port->extended_port_vid, extender_port->match_vlan, extender_port->match_inner_vlan));

    SHR_IF_ERR_EXIT(dnx_extender_port_gport_to_forward_information_set(unit, extender_port));

    SHR_IF_ERR_EXIT(dnx_extender_port_ingress_match_add
                    (unit, extender_port->port, extender_port->criteria, extender_port->extended_port_vid,
                     extender_port->match_vlan, extender_port->match_inner_vlan, local_in_lif));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   Clear a Port Extender on ingress side.
 *
 * \param [in] unit       -  device unit number.
 * \param [in] global_lif       -  global LIF id.
 * \param [in] local_in_lif       -  local in LIF id.
 * \param [in] result_type       -  Result type.
 * \param [in] extender_port        -  Main struct of the API. Holds all the relevant information.
 *        [in] extender_port.flags   - BCM_EXTENDER_PORT_WITH_ID Create an object with a given ID.
 *                                    - BCM_EXTENDER_PORT_REPLACE    Replace existing entry
 *                                    - BCM_EXTENDER_PORT_MULTICAST Create multicast type
 *        [in]    extender_port.extender_port_id - GPORT identifier..
 *        [in]    extender_port.port - Physical port / trunk.
 *        [in]    extender_port.extended_port_vid - Extender port VID..
 *        [in]    extender_port.match_vlan - Outer VLAN ID to match.
 *        [in]    extender_port.pcp_de_select - Selection of PCP and DE fields for egress ETAG.
 *                             - BCM_EXTENDER_PCP_DE_SELECT_OUTER_TAG, From outer VLAN tag
 *                             - BCM_EXTENDER_PCP_DE_SELECT_INNER_TAG, From inner VLAN tag
 *                             - BCM_EXTENDER_PCP_DE_SELECT_DEFAULT, per port configuration
 *        [in]    extender_port.qos_map_id - Qos map id for egress etag mapping profilel.
 *        [in]    extender_port.encap_id - Encap identifier(OUT LIF ID for egress).
 *
 * \return
 *   shr_error_e - Non-zero in case of an error.
 * \remark
 *   Extender function should be enabled on physical port first, so that the whole function would work.
 * \see
 *   * None
 */
static shr_error_e
dnx_extender_port_ingress_delete(
    int unit,
    int global_lif,
    int local_in_lif,
    int result_type,
    bcm_extender_port_t * extender_port)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_extender_port_match_information_clear(unit, local_in_lif));

    SHR_IF_ERR_EXIT(dnx_extender_port_ingress_match_delete
                    (unit, extender_port->port, extender_port->criteria, extender_port->extended_port_vid,
                     extender_port->match_vlan, extender_port->match_inner_vlan));
    SHR_IF_ERR_EXIT(algo_gpm_gport_l2_forward_info_delete(unit, extender_port->extender_port_id));

exit:
    SHR_FUNC_EXIT;

}

/*
 * End of Inner functions
 * }
 */

/*
 * APIs
 * {
 */

/**
 * \brief
 *   Create a Port Extender entity.
 *
 * \param [in] unit       -  device unit number.
 * \param [in] extender_port        -  Main struct of the API. Holds all the relevant information.
 *        [in]  extender_port.flags   - BCM_EXTENDER_PORT_WITH_ID Create an object with a given ID.
 *                                    - BCM_EXTENDER_PORT_REPLACE    Replace existing entry
 *                                    - BCM_EXTENDER_PORT_MULTICAST Create multicast type
 *        [inout]  extender_port.extender_port_id - GPORT identifier..
 *        [in]    extender_port.port - Physical port / trunk.
 *        [in]    extender_port.extended_port_vid - Extender port VID..
 *        [in]    extender_port.match_vlan - Outer VLAN ID to match.
 *        [in]    extender_port.pcp_de_select - Selection of PCP and DE fields for egress ETAG.
 *                             - BCM_EXTENDER_PCP_DE_SELECT_OUTER_TAG, From outer VLAN tag
 *                             - BCM_EXTENDER_PCP_DE_SELECT_INNER_TAG, From inner VLAN tag
 *                             - BCM_EXTENDER_PCP_DE_SELECT_DEFAULT, per port configuration
 *        [in]    extender_port.qos_map_id - Qos map id for egress etag mapping profilel.
 *        [out]   extender_port.encap_id - Encap identifier(OUT LIF ID for egress).
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t

 * \remark
 *   Extender function should be enabled on physical port first, so that the whole function would work.
 * \see
 *   * None
 */
bcm_error_t
bcm_dnx_extender_port_add(
    int unit,
    bcm_extender_port_t * extender_port)
{
    int global_lif, gport_id;
    int local_in_lif, local_out_lif;
    uint8 update = 0;
    uint8 ingress;
    bcm_extender_port_t extender_port_old;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    uint32 allocation_flags = LIF_MNGR_L2_GPORT_GLOBAL_LIF;
    uint32 direction_flags = DNX_ALGO_LIF_EGRESS;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_extender_port_add_verify(unit, extender_port));

    SHR_IF_ERR_EXIT(dnx_extender_get_specified_global_lif(unit, extender_port, &global_lif));
    update = (extender_port->flags & BCM_EXTENDER_PORT_REPLACE) ? TRUE : FALSE;

    /*
     * Clear the old info first if these info need update
     */
    if (update)
    {
        ingress = !_SHR_IS_FLAG_SET(extender_port->flags, BCM_EXTENDER_PORT_MULTICAST);

        /** Get existing info */
        bcm_extender_port_t_init(&extender_port_old);
        extender_port_old.extender_port_id = extender_port->extender_port_id;
        SHR_IF_ERR_EXIT(bcm_dnx_extender_port_get(unit, &extender_port_old));

        /** check is valid for updating */
        if (extender_port->port != extender_port_old.port)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Can't change extender port port\n");
        }
        if (extender_port->extended_port_vid != extender_port_old.extended_port_vid)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Can't change extender port vid\n");
        }
        if (extender_port->match_vlan != extender_port_old.match_vlan)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Can't change extender port match vlan\n");
        }
        if (extender_port->flags & BCM_EXTENDER_PORT_ENCAP_WITH_ID
            && extender_port->encap_id != extender_port_old.encap_id)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Can't change extender port encap id\n");
        }

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, extender_port->extender_port_id,
                         DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_AND_GLOBAL_LIF |
                         DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT, &gport_hw_resources));

        if (ingress)
        {
            local_in_lif = gport_hw_resources.local_in_lif;
            /*
             * set in_lif table and sw mapping of global and local
             */
            SHR_IF_ERR_EXIT(dnx_extender_port_in_lif_lif_table_manager_add
                            (unit, extender_port->flags, global_lif, &local_in_lif, 1));
        }

        /*
         * set EEDB, GLEM and sw mapping of global and local
         */
        local_out_lif = gport_hw_resources.local_out_lif;
        SHR_IF_ERR_EXIT(dnx_extender_port_out_lif_lif_table_manager_add
                        (unit, global_lif, &local_out_lif, extender_port, 1));
    }
    else
    {
        ingress = !_SHR_IS_FLAG_SET(extender_port->flags, BCM_EXTENDER_PORT_MULTICAST);

        /*
         * should allocate global_lif
         */
        if (global_lif != DNX_ALGO_GPM_LIF_INVALID)
        {
            allocation_flags |= LIF_MNGR_GLOBAL_LIF_WITH_ID;
        }
        if (ingress)
        {
            direction_flags |= DNX_ALGO_LIF_INGRESS;
        }
        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_allocate(unit, allocation_flags, direction_flags, &global_lif));

        if (ingress)
        {
            /*
             * set in_lif table and sw mapping of global and local
             */
            SHR_IF_ERR_EXIT(dnx_extender_port_in_lif_lif_table_manager_add
                            (unit, extender_port->flags, global_lif, &local_in_lif, 0));
        }

        /*
         * set EEDB, GLEM and sw mapping of global and local
         */
        SHR_IF_ERR_EXIT(dnx_extender_port_out_lif_lif_table_manager_add
                        (unit, global_lif, &local_out_lif, extender_port, 0));

        /*
         * Calculate the gport ID
         */
        if (!ingress)
        {
            BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY, global_lif);
        }
        else
        {
            BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, global_lif);
        }

        BCM_GPORT_EXTENDER_PORT_ID_SET(extender_port->extender_port_id, gport_id);
        BCM_L3_ITF_SET(extender_port->encap_id, BCM_L3_ITF_TYPE_LIF, global_lif);
    }

    /*
     * set match table on ingress
     */
    if (ingress)
    {
        SHR_IF_ERR_EXIT(dnx_extender_port_ingress_add(unit, global_lif, local_in_lif, 0, extender_port));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Retrieve a Port Extender entity attributes.
 *
 * \param [in] unit       -  device unit number.
 * \param [in] extender_port        -  Main struct of the API. Holds all the relevant information.
 *        [inout]   extender_port.extender_port_id - GPORT identifier..
 *        [out]     extender_port.flags  - BCM_EXTENDER_PORT_WITH_ID Create an object with a given ID.
 *                                       - BCM_EXTENDER_PORT_REPLACE    Replace existing entry
 *                                       - BCM_EXTENDER_PORT_MULTICAST Create multicast type
 *        [out]     extender_port.port - Extender port VID.
 *        [out]     extender_port.match_vlan - Outer VLAN ID to match.
 *        [out]     extender_port.pcp_de_select - Selection of PCP and DE fields for egress ETAG.
 *                                            - BCM_EXTENDER_PCP_DE_SELECT_OUTER_TAG, From outer VLAN tag
 *                                            - BCM_EXTENDER_PCP_DE_SELECT_INNER_TAG, From inner VLAN tag
 *                                            - BCM_EXTENDER_PCP_DE_SELECT_DEFAULT, per port configuration
 *        [out]     extender_port.qos_map_id - Qos map id for egress etag mapping profilel.
 *        [out]     extender_port.encap_id - Encap identifier(OUT LIF ID for egress).
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
bcm_error_t
bcm_dnx_extender_port_get(
    int unit,
    bcm_extender_port_t * extender_port)
{
    bcm_gport_t gport_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 lif_flags = 0;
    uint32 vlan_domain = 0;
    uint16 ecid;
    uint8 is_ingress;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_extender_port_get_verify(unit, extender_port));

    lif_flags |= DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT |
        DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS |
        DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;

    gport_id = extender_port->extender_port_id;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport_id, lif_flags, &gport_hw_resources));

    /*
     * Don't find valid local_in_lif and local_out_lif with extender gport ID
     */
    if (gport_hw_resources.local_in_lif == DNX_ALGO_GPM_LIF_INVALID &&
        gport_hw_resources.local_out_lif == DNX_ALGO_GPM_LIF_INVALID)
    {
        SHR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND, " gport 0x%08X not found\r\n%s%s", gport_id, EMPTY, EMPTY);
    }

    is_ingress = (gport_hw_resources.local_in_lif == DNX_ALGO_GPM_LIF_INVALID) ? 0 : 1;

    if (is_ingress)
    {
        /** Get extender_port->match_vlan based on gport_hw_resources.local_in_lif using the SW state */
        SHR_IF_ERR_EXIT(dnx_extender_port_match_information_get
                        (unit, gport_hw_resources.local_in_lif, &(extender_port->port), &vlan_domain, &ecid,
                         &(extender_port->match_vlan), &(extender_port->match_inner_vlan), &(extender_port->criteria)));
        extender_port->extended_port_vid = ecid;

        /** Get extender_port->port and extender_port->encap_id based on gport ID using the SW state */
        SHR_IF_ERR_EXIT(dnx_extender_port_gport_to_forward_information_get(unit, extender_port));

        /*
         * Set flags value
         */
        if (gport_hw_resources.inlif_dbal_result_type == DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE)
        {
            extender_port->flags |= BCM_EXTENDER_PORT_INGRESS_WIDE;
        }
    }
    else
    {
        /** Get extender port VID from EEDB */
        SHR_IF_ERR_EXIT(dnx_extender_port_egress_eedb_get(unit, &gport_hw_resources, extender_port));
        /** Get extender_port->encap_id with egress only */
        BCM_L3_ITF_SET(extender_port->encap_id, BCM_L3_ITF_TYPE_LIF, gport_hw_resources.global_out_lif);
    }

    if (gport_hw_resources.local_in_lif == DNX_ALGO_GPM_LIF_INVALID &&
        gport_hw_resources.local_out_lif != DNX_ALGO_GPM_LIF_INVALID)
    {
        extender_port->flags |= BCM_EXTENDER_PORT_MULTICAST;
    }

    extender_port->pcp_de_select = BCM_EXTENDER_PCP_DE_SELECT_OUTER_TAG;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Delete a Port Extender entity.
 *
 * \param [in] unit       -  device unit number.
 * \param [in] extender_port_id        -  GPORT identifier.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
bcm_error_t
bcm_dnx_extender_port_delete(
    int unit,
    bcm_gport_t extender_port_id)
{
    bcm_gport_t gport_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 gpm_flags = 0;
    bcm_extender_port_t extender_port;
    uint8 is_ingress = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_extender_port_delete_verify(unit, extender_port_id));

    sal_memset(&extender_port, 0, sizeof(bcm_extender_port_t));
    extender_port.extender_port_id = extender_port_id;
    SHR_IF_ERR_EXIT(bcm_dnx_extender_port_get(unit, &extender_port));

    /*
     * Get HW resources. No need for strict check - it was already done when we called extender_port_get.
     */
    gpm_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT;
    gpm_flags |= DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS;
    gpm_flags |= DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;

    gport_id = extender_port_id;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport_id, gpm_flags, &gport_hw_resources));
    is_ingress = (gport_hw_resources.local_in_lif == DNX_ALGO_GPM_LIF_INVALID) ? 0 : 1;

    if (is_ingress)
    {
        SHR_IF_ERR_EXIT(dnx_extender_port_ingress_delete(unit,
                                                         gport_hw_resources.global_in_lif,
                                                         gport_hw_resources.local_in_lif,
                                                         gport_hw_resources.inlif_dbal_result_type, &extender_port));
    }

    SHR_IF_ERR_EXIT(dnx_extender_port_lif_table_manager_delete(unit, &gport_hw_resources));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Delete all Port Extender entities.
 *
 * \param [in] unit       -  device unit number.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 *
 * \remark
 *   Found all port extender entries in EEDB table and free related port extender resource with local out lif.
 */
bcm_error_t
bcm_dnx_extender_port_delete_all(
    int unit)
{
    uint32 entry_handle_id;
    int is_end;
    bcm_gport_t gport_id = 0;
    uint32 local_outlif[1] = { 0 };
    int core_id = _SHR_CORE_ALL;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Allocate handle to the table of the iteration and initialise an iterator entity.
     * The iterator is in mode ALL, which means that it will consider all entries regardless
     * of them being default entries or not.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_OUT_AC, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));

    /*
     * Receive first entry in table.
     */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_outlif));
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit,
                                                    DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                                                    core_id, local_outlif[0], &gport_id));
        if (BCM_GPORT_IS_EXTENDER_PORT(gport_id))
        {
            SHR_IF_ERR_EXIT(bcm_dnx_extender_port_delete(unit, gport_id));
        }
        /*
         * Receive next entry in table.
         */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set port mapping according to application type.
 *
 * \param [in] unit     - device unit number.
 * \param [in] flags    - flags for mapping direction. \n
 *                          - BCM_PORT_EXTENDER_MAPPING_INGRESS \n
 *                          - BCM_PORT_EXTENDER_MAPPING_EGRESS \n
 * \param [in] type     - application type. \n
 *                          - bcmPortExtenderMappingTypePonTunnel \n
 *                          - bcmPortExtenderMappingTypePortVlan \n
 *                          - bcmPortExtenderMappingTypeUserDefineValue \n
 * \param [in] mapping_info - point to mapping information. \n
 *        [in] mapping_info.pp_port: mapping pp port. \n
 *        [in] mapping_info.tunnel_id: PON tunnel mapping to, used for PON. \n
 *        [in] mapping_info.phy_port: local port for mapping. \n
 *        [in] mapping_info.vlan: vid for COE mapping. \n
 *        [in] mapping_info.user_define_value: use defined value. \n
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 *
 * \remark
 *   * For COE application, need to set both ingress and egress flags.
 *   * For COE application, mapping_info.user_define_value will hold input of source system port.
 * \see
 *   * None
 */

bcm_error_t
bcm_dnx_port_extender_mapping_info_set(
    int unit,
    uint32 flags,
    bcm_port_extender_mapping_type_t type,
    bcm_port_extender_mapping_info_t * mapping_info)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** verify */
    SHR_INVOKE_VERIFY_DNX(dnx_port_extender_mapping_info_set_verify(unit, flags, type, mapping_info));

    SHR_IF_ERR_EXIT(dnx_port_sit_application_mapping_info_set(unit, type, mapping_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get port mapping according to application type.
 *
 * \param [in] unit     - device unit number.
 * \param [in] flags    - flags for mapping direction. \n
 *                          - BCM_PORT_EXTENDER_MAPPING_INGRESS \n
 *                          - BCM_PORT_EXTENDER_MAPPING_EGRESS \n
 * \param [in] type     - application type. \n
 *                          - bcmPortExtenderMappingTypePonTunnel \n
 *                          - bcmPortExtenderMappingTypePortVlan \n
 *                          - bcmPortExtenderMappingTypeUserDefineValue \n
 * \param [in, out] mapping_info - point to mapping information. \n
 *        [out] mapping_info.pp_port: mapping pp port. \n
 *        [out] mapping_info.tunnel_id: tunnel id for PON. \n
 *        [in] mapping_info.phy_port: local port for mapping. \n
 *        [out] mapping_info.vlan: vid for COE mapping. \n
 *        [out] mapping_info.user_define_value: use defined value. \n
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 *
 * \remark
 *   * For COE application, mapping_info.user_define_value will hold input of source system port.
 * \see
 *   * None
 */

bcm_error_t
bcm_dnx_port_extender_mapping_info_get(
    int unit,
    uint32 flags,
    bcm_port_extender_mapping_type_t type,
    bcm_port_extender_mapping_info_t * mapping_info)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** verify */
    SHR_INVOKE_VERIFY_DNX(dnx_port_extender_mapping_info_get_verify(unit, flags, type, mapping_info));

    SHR_IF_ERR_EXIT(dnx_port_sit_application_mapping_info_get(unit, type, mapping_info));

exit:
    SHR_FUNC_EXIT;

}

/*
 * End of APIs
 * }
 */
