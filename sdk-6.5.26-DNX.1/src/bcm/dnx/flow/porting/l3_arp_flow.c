/** \file l3_arp_flow.c
 *  * ARP APIs to flows APIs conversion.
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

/*
 * Include files.
 * {
 */
#include "l3_arp_flow.h"
#include <bcm/types.h>
#include <bcm_int/dnx/auto_generated/dnx_flow_dispatch.h>
#include <bcm_int/dnx/flow/flow.h>
#include <bcm_int/dnx/l3/l3_arp.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/qos/qos.h>
/*
 * }
 */

/*
 * Structures
 * {
 */
typedef struct
{
    void *data;
    bcm_l3_egress_traverse_cb user_cb;
} dnx_l3_egress_arp_flow_user_data_t;
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
 * Functions
 * {
 */

/* See the .h file for prototype define */
shr_error_e
dnx_l3_egress_flow_api_create_arp(
    int unit,
    uint32 flags,
    bcm_l3_egress_t * egr,
    bcm_l3_egress_t * old_egr)
{

    int nof_special_fields = 0;
    bcm_flow_initiator_info_t info;
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_special_fields_t special_fields;
    bcm_flow_field_id_t field_id = 0;
    bcm_gport_t tunnel_gport;
    bcm_flow_special_fields_t special_fields_get;
    bcm_flow_handle_info_t flow_handle_info_get;
    uint8 svtag_get = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&info, 0, sizeof(bcm_flow_initiator_info_t));
    sal_memset(&flow_handle_info, 0, sizeof(bcm_flow_handle_info_t));
    sal_memset(&special_fields, 0, sizeof(bcm_flow_special_fields_t));
    sal_memset(&special_fields_get, 0, sizeof(bcm_flow_special_fields_t));

    BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, FLOW_APP_NAME_ARP_INITIATOR, &flow_handle));

    flow_handle_info.flow_handle = flow_handle;

    if (egr->encap_id != 0)
    {
        /*
         * In 'replace' and 'with_id' cases, input (egr->encap_id)
         * must be encoded as 'interface'.
         * If it is not, make it.
         */
        if (!(BCM_L3_ITF_TYPE_IS_LIF(egr->encap_id)))
        {
            int l3_itf_dummy;

            l3_itf_dummy = BCM_L3_ITF_SUB_TYPE_VALUE_GET(egr->encap_id);
            BCM_L3_ITF_SET(egr->encap_id, BCM_L3_ITF_TYPE_LIF, l3_itf_dummy);
        }
        BCM_L3_ITF_LIF_TO_GPORT_FLOW_LIF(flow_handle_info.flow_id, egr->encap_id);
    }

    if (((BCM_L3_ITF_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED(egr->encap_id))
         || (_SHR_IS_FLAG_SET(egr->flags2, BCM_L3_FLAGS2_VIRTUAL_EGRESS_POINTED)))
        && (!_SHR_IS_FLAG_SET(flags, BCM_L3_REPLACE)))
    {
        flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_VIRTUAL;
        BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED_SET(flow_handle_info.flow_id, flow_handle_info.flow_id);
    }

    /*
     * If encap_id is given with BCM_L3_KEEP_DSTMAC | BCM_L3_KEEP_VLAN | BCM_L3_REPLACE flags,
     * we assume Link-layer is not changed and we don't update it.
     * Useful in case the only need is to update multiple Tunnel-intfs such as MPLS-Tunnel or IP-Tunnel to same Link-Layer
     */
    if ((flags & (BCM_L3_KEEP_DSTMAC | BCM_L3_KEEP_VLAN)) != (BCM_L3_KEEP_DSTMAC | BCM_L3_KEEP_VLAN))
    {
        if (_SHR_IS_FLAG_SET(flags, BCM_L3_REPLACE))
        {
            sal_memcpy(&flow_handle_info_get, &flow_handle_info, sizeof(bcm_flow_handle_info_t));
            flow_handle_info_get.flags &= ~(BCM_FLOW_HANDLE_INFO_WITH_ID | BCM_FLOW_HANDLE_INFO_VIRTUAL);
            SHR_IF_ERR_EXIT(bcm_dnx_flow_initiator_info_get(unit, &flow_handle_info_get, &info, &special_fields_get));
            flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_REPLACE;
            svtag_get = _SHR_IS_FLAG_SET(info.valid_elements_set, BCM_FLOW_INITIATOR_ELEMENT_SVTAG_ENABLE_VALID);
        }

        /*
         * Common fields
         */
        info.flags = 0;

        /*
         *  The non flow API didn't supported statistic update so if a statistic flag was set the stat fields
         *  were set to 0 later and updated using a different API call
         */
        if (_SHR_IS_FLAG_SET(egr->flags2, BCM_L3_FLAGS2_EGRESS_STAT_ENABLE))
        {
            info.valid_elements_set |=
                BCM_FLOW_INITIATOR_ELEMENT_STAT_ID_VALID | BCM_FLOW_INITIATOR_ELEMENT_STAT_PP_PROFILE_VALID;
            info.stat_id = 0;
            info.stat_pp_profile = 0;
        }

        if (_SHR_IS_FLAG_SET(egr->flags2, BCM_L3_FLAGS2_VLAN_TRANSLATION))
        {
            uint32 fields[3] = { FLOW_S_F_EMPTY };
            uint32 nof_fields = 0;
            uint32 field_index = 0;
            bcm_flow_special_field_t special_field_data;
            shr_error_e rv;

            /*
             * Indication that 1 VLAN tag is required
             */
            BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "VLAN_EDIT_VID_1", &field_id));
            fields[nof_fields++] = field_id;
            if (_SHR_IS_FLAG_SET(egr->flags2, BCM_L3_FLAGS2_VLAN_TRANSLATION_TWO_VLAN_TAGS))
            {
                BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "VLAN_EDIT_VID_2", &field_id));
                fields[nof_fields++] = field_id;
            }
            while (fields[field_index] != FLOW_S_F_EMPTY)
            {
                special_fields.special_fields[nof_special_fields].field_id = fields[field_index];

                if (_SHR_IS_FLAG_SET(flags, BCM_L3_REPLACE))
                {
                    rv = dnx_flow_special_field_data_get(unit, &special_fields_get, fields[field_index],
                                                         &special_field_data);
                    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
                    if (rv == _SHR_E_NONE)
                    {
                        special_fields.special_fields[nof_special_fields++].shr_var_uint32 =
                            special_field_data.shr_var_uint32;
                    }
                    else
                    {
                        special_fields.special_fields[nof_special_fields++].shr_var_uint32 = 0;
                    }
                }
                else
                {
                    special_fields.special_fields[nof_special_fields++].shr_var_uint32 = 0;
                }
                field_index++;
            }
            /*
             * Set QoS model
             */
            info.valid_elements_set |= BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID;
            info.egress_qos_model.egress_qos = egr->egress_qos_model.egress_qos;
            info.egress_qos_model.egress_ecn = egr->egress_qos_model.egress_ecn;
            info.egress_qos_model.egress_ttl = egr->egress_qos_model.egress_ttl;

            if ((egr->egress_qos_model.egress_qos == bcmQosEgressModelPipeMyNameSpace) ||
                (egr->egress_qos_model.egress_qos == bcmQosEgressModelPipeNextNameSpace))
            {
                BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "QOS_PRI", &field_id));
                special_fields.special_fields[nof_special_fields].field_id = field_id;
                special_fields.special_fields[nof_special_fields++].shr_var_uint32 = egr->mpls_pkt_pri;

                BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "QOS_CFI", &field_id));
                special_fields.special_fields[nof_special_fields].field_id = field_id;
                special_fields.special_fields[nof_special_fields++].shr_var_uint32 = egr->mpls_pkt_cfi;
            }
            /** set SVTAG enable field */
            if (_SHR_IS_FLAG_SET(egr->flags3, BCM_L3_FLAGS3_EGRESS_SVTAG_PORT_ENABLE) && !svtag_get)
            {
                info.valid_elements_set |= BCM_FLOW_INITIATOR_ELEMENT_SVTAG_ENABLE_VALID;
            }
            else
            {
                info.valid_elements_set &= ~BCM_FLOW_INITIATOR_ELEMENT_SVTAG_ENABLE_VALID;
            }
        }

        /** if required, add WITH_ID flag, should be done after bcm_dnx_flow_initiator_info_get */
        if (egr->encap_id != 0)
        {
            flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_WITH_ID;
        }

        /*
         * Set QoS MAP ID
         */
        if (egr->qos_map_id != 0)
        {
            info.valid_elements_set |= BCM_FLOW_INITIATOR_ELEMENT_QOS_MAP_ID_VALID;
            info.qos_map_id = egr->qos_map_id;
        }
        else if (_SHR_IS_FLAG_SET(info.valid_elements_set, BCM_FLOW_INITIATOR_ELEMENT_QOS_MAP_ID_VALID))
        {
            info.valid_elements_clear |= BCM_FLOW_INITIATOR_ELEMENT_QOS_MAP_ID_VALID;
            info.valid_elements_set &= ~BCM_FLOW_INITIATOR_ELEMENT_QOS_MAP_ID_VALID;
        }

        /*
         * Fill next pointer
         */
        if (egr->vlan_port_id != BCM_GPORT_INVALID)
        {
            info.valid_elements_set |= BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID;

            BCM_GPORT_TUNNEL_ID_SET(tunnel_gport, BCM_GPORT_SUB_TYPE_LIF_VAL_GET(egr->vlan_port_id));
            BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(info.l3_intf_id, tunnel_gport);
        }

        if (_SHR_IS_FLAG_SET(flags, BCM_L3_REPLACE))
        {
            /*
             * In case of replace, encap access must be invalid
             */
            info.encap_access = bcmEncapAccessInvalid;
        }
        else
        {
            /*
             * encap access
             * The none flow ARP API didn't support phase 1 so if phase was requested (bcmEncapAccessRif) it will be the
             * selected phase otherwise the original none flow function will be used (dnx_l3_egress_arp_outlif_phase_get).
             */
            if (egr->encap_access == bcmEncapAccessRif)
            {
                /*
                 * phase 1
                 */
                info.encap_access = bcmEncapAccessRif;
            }
            else
            {
                dnx_algo_local_outlif_logical_phase_e phase_selected =
                    dnx_l3_egress_arp_outlif_phase_get(unit, egr->flags, egr->flags2, egr->vlan, egr->encap_access);

                SHR_IF_ERR_EXIT(dnx_lif_lib_logical_phase_to_encap_access_convert
                                (unit, phase_selected, &(info.encap_access)));
            }
        }

        /*
         * Special fields
         */
        if (_SHR_IS_FLAG_SET(egr->flags2, BCM_L3_FLAGS2_SRC_MAC))
        {
            /*
             * SRC MAC address
             */
            BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "SRC_MAC_ADDRESS", &field_id));
            special_fields.special_fields[nof_special_fields].field_id = field_id;
            sal_memcpy(special_fields.special_fields[nof_special_fields++].shr_var_uint8_arr, egr->src_mac_addr,
                       sizeof(bcm_mac_t));
        }

        /*
         * DST MAC address
         */
        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "DST_MAC_ADDRESS", &field_id));
        special_fields.special_fields[nof_special_fields].field_id = field_id;
        sal_memcpy(special_fields.special_fields[nof_special_fields++].shr_var_uint8_arr, egr->mac_addr,
                   sizeof(bcm_mac_t));

        /*
         * VSI
         */
        if (egr->vlan != 0)
        {
            BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "VSI", &field_id));
            special_fields.special_fields[nof_special_fields].field_id = field_id;
            special_fields.special_fields[nof_special_fields++].shr_var_uint32 = egr->vlan;
        }

        special_fields.actual_nof_special_fields = nof_special_fields;
        if (!_SHR_IS_FLAG_SET(flags, BCM_L3_REPLACE))
        {
        }
        BCM_IF_ERROR_RETURN(bcm_dnx_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields));

        BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(egr->encap_id, flow_handle_info.flow_id);
    }

    /*
     * In case interface is valid, it represents tunnel that should point to the created ARP
     */
    if (egr->intf != 0)
    {
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
        uint32 lif_flags;

        sal_memset(&gport_hw_resources, 0, sizeof(gport_hw_resources));

        lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS;
        BCM_IF_ERROR_RETURN(dnx_algo_gpm_gport_to_hw_resources
                            (unit, flow_handle_info.flow_id, lif_flags, &gport_hw_resources));

        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(tunnel_gport, egr->intf);

        SHR_IF_ERR_EXIT(dnx_l3_arp_tunnel_encap_update_next_pointer
                        (unit, tunnel_gport, gport_hw_resources.local_out_lif));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Fills L3 egress ARP structure with the returned initiator info data
*
*   \param [in] unit - Relevant unit.
*   \param [in] intf - Egress object ID.
*   \param [in] flow_handle - The L3 ARP FLOW handle ID.
*   \param [in] initiator_info - Holds information about
*           the given egress object ID.
*   \param [in] special_fields - Holds information about
*           the special fields for the egress object.
*   \param [out] egr - Structure holding the egress object.
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
dnx_l3_egress_arp_fill_egr_struct(
    int unit,
    bcm_if_t intf,
    bcm_flow_handle_t flow_handle,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields,
    bcm_l3_egress_t * egr)
{
    int special_fields_iter;
    bcm_flow_field_id_t dst_mac_field_id = 0, src_mac_field_id = 0, vsi_mac_field_id = 0;
    bcm_flow_field_id_t vid_1_field_id = 0, vid_2_field_id = 0;
    bcm_flow_field_id_t qos_pri_field_id = 0, qos_cfi_field_id = 0;
    bcm_gport_t flow_gport;
    SHR_FUNC_INIT_VARS(unit);

    if (BCM_L3_ITF_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED(intf))
    {
        egr->flags2 |= BCM_L3_FLAGS2_VIRTUAL_EGRESS_POINTED;
    }

    /*
     * Get QoS MAP ID - valid not being set in FLOW valid_elements_set
     */
    egr->qos_map_id = initiator_info->qos_map_id;

    /*
     * Get QoS model - valid not being set in FLOW valid_elements_set
     */
    egr->egress_qos_model.egress_qos = initiator_info->egress_qos_model.egress_qos;
    egr->egress_qos_model.egress_ecn = initiator_info->egress_qos_model.egress_ecn;
    egr->egress_qos_model.egress_ttl = initiator_info->egress_qos_model.egress_ttl;

    /*
     * Get statistics flag
     */
    if (_SHR_IS_FLAG_SET(initiator_info->valid_elements_set, BCM_FLOW_INITIATOR_ELEMENT_STAT_PP_PROFILE_VALID))
    {
        egr->flags2 |= BCM_L3_FLAGS2_EGRESS_STAT_ENABLE;
    }

    /*
     * Get next pointer
     */
    if (_SHR_IS_FLAG_SET(initiator_info->valid_elements_set, BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID))
    {
        BCM_L3_ITF_LIF_TO_GPORT_FLOW_LIF(flow_gport, initiator_info->l3_intf_id);
        /*
         * Get VLAN-PORT gport from FLOW gport
         */
        SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                        (unit, flow_gport, BCM_GPORT_VLAN_PORT, DNX_ALGO_GPM_CONVERSION_FLAG_NONE, &egr->vlan_port_id));
    }
    /*
     * Get SVTAG falg
     */
    if (_SHR_IS_FLAG_SET(initiator_info->valid_elements_set, BCM_FLOW_INITIATOR_ELEMENT_SVTAG_ENABLE_VALID))
    {
        egr->flags3 |= BCM_L3_FLAGS3_EGRESS_SVTAG_PORT_ENABLE;
    }
    SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "VSI", &vsi_mac_field_id));
    SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "VLAN_EDIT_VID_1", &vid_1_field_id));
    SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "VLAN_EDIT_VID_2", &vid_2_field_id));
    SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "DST_MAC_ADDRESS", &dst_mac_field_id));
    SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "SRC_MAC_ADDRESS", &src_mac_field_id));
    SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "QOS_PRI", &qos_pri_field_id));
    SHR_IF_ERR_EXIT(bcm_flow_logical_field_id_get(unit, flow_handle, "QOS_CFI", &qos_cfi_field_id));

    for (special_fields_iter = 0; special_fields_iter < special_fields->actual_nof_special_fields;
         special_fields_iter++)
    {
        if (special_fields->special_fields[special_fields_iter].field_id == dst_mac_field_id)
        {
            sal_memcpy(egr->mac_addr, special_fields->special_fields[special_fields_iter].shr_var_uint8_arr,
                       sizeof(bcm_mac_t));
        }

        if (special_fields->special_fields[special_fields_iter].field_id == src_mac_field_id)
        {
            egr->flags2 |= BCM_L3_FLAGS2_SRC_MAC;

            sal_memcpy(egr->src_mac_addr, special_fields->special_fields[special_fields_iter].shr_var_uint8_arr,
                       sizeof(bcm_mac_t));
        }

        if (special_fields->special_fields[special_fields_iter].field_id == vsi_mac_field_id)
        {
            egr->vlan = special_fields->special_fields[special_fields_iter].shr_var_uint32;
        }

        if (special_fields->special_fields[special_fields_iter].field_id == vid_1_field_id)
        {
            egr->flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION;
        }

        if (special_fields->special_fields[special_fields_iter].field_id == vid_2_field_id)
        {
            egr->flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION_TWO_VLAN_TAGS;
        }

        if (special_fields->special_fields[special_fields_iter].field_id == qos_pri_field_id)
        {
            egr->mpls_pkt_pri = special_fields->special_fields[special_fields_iter].shr_var_uint32;
        }

        if (special_fields->special_fields[special_fields_iter].field_id == qos_cfi_field_id)
        {
            egr->mpls_pkt_cfi = special_fields->special_fields[special_fields_iter].shr_var_uint32;
        }
    }

    

    /*
     * Get encap access
     */
    switch (initiator_info->encap_access)
    {
        case bcmEncapAccessRif:
            
            if (!_SHR_IS_FLAG_SET(egr->flags2, BCM_L3_FLAGS2_VLAN_TRANSLATION))
            {
                egr->encap_access = bcmEncapAccessInvalid;
            }
            else
            {
                egr->encap_access = bcmEncapAccessRif;
            }
            break;
        case bcmEncapAccessNativeArp:
            
            egr->flags |= BCM_L3_NATIVE_ENCAP;
            break;
        case bcmEncapAccessAc:
            /*
             * ARP+AC
             */
            if (_SHR_IS_FLAG_SET(egr->flags2, BCM_L3_FLAGS2_VLAN_TRANSLATION))
            {
                egr->encap_access = bcmEncapAccessInvalid;
            }
            /*
             * AC
             */
            else
            {
                egr->encap_access = bcmEncapAccessAc;
            }
            break;
        case bcmEncapAccessArp:
        case bcmEncapAccessInvalid:
            /*
             * The default encap stage of the ARP is bcmEncapAccessArp.
             * The flow will return bcmEncapAccessInvalid in this case
             * and in order to match to non-flow API implementation, bcmEncapAccessInvalid will be returned.
             */
            egr->encap_access = bcmEncapAccessInvalid;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "unsupported phase %d\n", initiator_info->encap_access);
    }

    egr->encap_id = intf;

exit:
    SHR_FUNC_EXIT;
}

/* See the .h file for prototype define */
shr_error_e
dnx_l3_egress_arp_info_flow_api_get(
    int unit,
    bcm_if_t intf,
    bcm_l3_egress_t * egr)
{
    bcm_flow_initiator_info_t info;
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_special_fields_t special_fields;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&flow_handle_info, 0, sizeof(bcm_flow_handle_info_t));
    sal_memset(&special_fields, 0, sizeof(bcm_flow_special_fields_t));
    sal_memset(&info, 0, sizeof(bcm_flow_initiator_info_t));

    BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, FLOW_APP_NAME_ARP_INITIATOR, &flow_handle));

    flow_handle_info.flow_handle = flow_handle;
    BCM_L3_ITF_LIF_TO_GPORT_FLOW_LIF(flow_handle_info.flow_id, intf);
    BCM_IF_ERROR_RETURN(bcm_dnx_flow_initiator_info_get(unit, &flow_handle_info, &info, &special_fields));
    SHR_IF_ERR_EXIT(dnx_l3_egress_arp_fill_egr_struct(unit, intf, flow_handle, &info, &special_fields, egr));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_l3_egress_arp_cb(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields,
    void *data)
{
    bcm_if_t intf;
    bcm_l3_egress_t egr_itf_struct;
    dnx_l3_egress_arp_flow_user_data_t *user_data = (dnx_l3_egress_arp_flow_user_data_t *) data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&egr_itf_struct, 0, sizeof(bcm_l3_egress_t));

    BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(intf, flow_handle_info->flow_id);
    /*
     * Fill the received data in egr_itf_struct
     */
    SHR_IF_ERR_EXIT(dnx_l3_egress_arp_fill_egr_struct(unit, intf, flow_handle_info->flow_handle,
                                                      initiator_info, special_fields, &egr_itf_struct));

    /*
     * Run callback function
     */
    SHR_IF_ERR_EXIT(user_data->user_cb(unit, intf, &egr_itf_struct, user_data->data));

exit:
    SHR_FUNC_EXIT;
}

/* See the .h file for prototype define */
shr_error_e
dnx_l3_egress_arp_flow_api_traverse(
    int unit,
    bcm_l3_egress_traverse_cb cb,
    void *data)
{
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_handle_info_t flow_handle_info = { 0 };
    dnx_l3_egress_arp_flow_user_data_t user_data = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_flow_handle_get(unit, FLOW_APP_NAME_ARP_INITIATOR, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;

    user_data.user_cb = cb;
    user_data.data = data;

    SHR_IF_ERR_EXIT(bcm_dnx_flow_initiator_info_traverse(unit, &flow_handle_info, dnx_l3_egress_arp_cb, &user_data));

exit:
    SHR_FUNC_EXIT;
}

/* See the .h file for prototype define */
shr_error_e
dnx_l3_egress_arp_info_flow_api_delete(
    int unit,
    bcm_if_t intf)
{
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_handle_info_t flow_handle_info = { 0 };
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(bcm_flow_handle_get(unit, FLOW_APP_NAME_ARP_INITIATOR, &flow_handle));

    flow_handle_info.flow_handle = flow_handle;

    BCM_L3_ITF_LIF_TO_GPORT_FLOW_LIF(flow_handle_info.flow_id, intf);

    SHR_IF_ERR_EXIT(bcm_dnx_flow_initiator_info_destroy(unit, &flow_handle_info));

exit:
    SHR_FUNC_EXIT;
}
/*
 * }
 */
