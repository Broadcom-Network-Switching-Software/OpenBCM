/**
 *
 * \file dnx_port_nif_ofr_prd.c
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 * NIF OFR procedures for DNX.
 *
 */

/*
 * Include files:
 * {
 */

#include "dnx_port_nif_ofr_internal.h"
#include "dnx_port_nif_link_list_internal.h"

#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <shared/shrextend/shrextend_error.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <bcm_int/dnx/port/nif/dnx_port_nif_ofr.h>

/*
 * }
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/**
 * \brief - set TPID value for the port profile
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] tpid_index - tpid index (0-3). per port there can
 *        be 4 identified TPIDs
 * \param [in] tpid_value - TPID value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_ofr_prd_tpid_set(
    int unit,
    bcm_port_t port,
    uint32 tpid_index,
    uint32 tpid_value)
{
    uint32 prd_profile;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get prd profile for the given port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_prd_profile_get(unit, port, &prd_profile));

    SHR_IF_ERR_EXIT(dnx_ofr_prd_tpid_hw_set(unit, prd_profile, tpid_index, tpid_value));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get TPID value for the port profile
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] tpid_index - tpid index (0-3). per port there can
 *        be 4 identified TPIDs
 * \param [in] tpid_value - TPID value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_ofr_prd_tpid_get(
    int unit,
    bcm_port_t port,
    uint32 tpid_index,
    uint32 *tpid_value)
{
    uint32 prd_profile;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get prd profile for the given port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_prd_profile_get(unit, port, &prd_profile));

    SHR_IF_ERR_EXIT(dnx_ofr_prd_tpid_hw_get(unit, prd_profile, tpid_index, tpid_value));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - set PRD hard stage priority map.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] map - map type:
 *  see bcm_cosq_ingress_port_drop_map_t.
 * \param [in] key - key index for the map (depends on the map
 *        type)
 * \param [in] priority - priority value to set to the key in
 *        the map.
 * \param [in] is_tdm - TDM signal
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_ofr_prd_map_set(
    int unit,
    bcm_port_t port,
    bcm_cosq_ingress_port_drop_map_t map,
    uint32 key,
    uint32 priority,
    uint32 is_tdm)
{
    uint32 dp, tc, dscp, pcp, dei, exp;
    SHR_FUNC_INIT_VARS(unit);

    switch (map)
    {
        case bcmCosqIngressPortDropTmTcDpPriorityTable:
            IMB_COSQ_PRD_TM_KEY_TC_GET(key, tc);
            IMB_COSQ_PRD_TM_KEY_DP_GET(key, dp);
            SHR_IF_ERR_EXIT(dnx_ofr_prd_map_tm_tc_dp_hw_set(unit, tc, dp, priority, is_tdm));
            break;
        case bcmCosqIngressPortDropIpDscpToPriorityTable:
            IMB_COSQ_PRD_IP_KEY_DSCP_GET(key, dscp);
            SHR_IF_ERR_EXIT(dnx_ofr_prd_map_ip_dscp_hw_set(unit, dscp, priority, is_tdm));
            break;
        case bcmCosqIngressPortDropEthPcpDeiToPriorityTable:
            IMB_COSQ_PRD_ETH_KEY_PCP_GET(key, pcp);
            IMB_COSQ_PRD_ETH_KEY_DEI_GET(key, dei);
            SHR_IF_ERR_EXIT(dnx_ofr_prd_map_pcp_dei_hw_set(unit, pcp, dei, priority, is_tdm));
            break;
        case bcmCosqIngressPortDropMplsExpToPriorityTable:
            IMB_COSQ_PRD_MPLS_KEY_EXP_GET(key, exp);
            SHR_IF_ERR_EXIT(dnx_ofr_prd_map_mpls_exp_hw_set(unit, exp, priority, is_tdm));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid PRD Map %d\n", map);
    }
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get priority value mapped to a key in one of the PRD
 *        priority tables
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] map - type of map. see
 *        bcm_cosq_ingress_port_drop_map_t
 * \param [in] key - key index for the map (depends on the map
 *        type)
 * \param [out] priority - returned priority value
 * \param [out] is_tdm - TDM signal
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_ofr_prd_map_get(
    int unit,
    bcm_port_t port,
    bcm_cosq_ingress_port_drop_map_t map,
    uint32 key,
    uint32 *priority,
    uint32 *is_tdm)
{
    uint32 dp, tc, dscp, pcp, dei, exp;
    SHR_FUNC_INIT_VARS(unit);

    *is_tdm = 0;
    switch (map)
    {
        case bcmCosqIngressPortDropTmTcDpPriorityTable:
            IMB_COSQ_PRD_TM_KEY_TC_GET(key, tc);
            IMB_COSQ_PRD_TM_KEY_DP_GET(key, dp);
            SHR_IF_ERR_EXIT(dnx_ofr_prd_map_tm_tc_dp_hw_get(unit, tc, dp, priority, is_tdm));
            break;
        case bcmCosqIngressPortDropIpDscpToPriorityTable:
            IMB_COSQ_PRD_IP_KEY_DSCP_GET(key, dscp);
            SHR_IF_ERR_EXIT(dnx_ofr_prd_map_ip_dscp_hw_get(unit, dscp, priority, is_tdm));
            break;
        case bcmCosqIngressPortDropEthPcpDeiToPriorityTable:
            IMB_COSQ_PRD_ETH_KEY_PCP_GET(key, pcp);
            IMB_COSQ_PRD_ETH_KEY_DEI_GET(key, dei);
            SHR_IF_ERR_EXIT(dnx_ofr_prd_map_pcp_dei_hw_get(unit, pcp, dei, priority, is_tdm));
            break;
        case bcmCosqIngressPortDropMplsExpToPriorityTable:
            IMB_COSQ_PRD_MPLS_KEY_EXP_GET(key, exp);
            SHR_IF_ERR_EXIT(dnx_ofr_prd_map_mpls_exp_hw_get(unit, exp, priority, is_tdm));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid PRD Map %d\n", map);
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - set different PRD properties
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] property - PRD property to set:
 *  see enum imb_prd_properties_t.
 * \param [in] val - value of the property to set.
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_ofr_prd_properties_set(
    int unit,
    bcm_port_t port,
    imb_prd_properties_t property,
    uint32 val)
{
    uint32 prd_profile;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get prd profile for the given port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_prd_profile_get(unit, port, &prd_profile));

    switch (property)
    {
        case imbImbPrdIgnoreIpDscp:
            SHR_IF_ERR_EXIT(dnx_ofr_prd_ignore_ip_dscp_hw_set(unit, prd_profile, val));
            break;
        case imbImbPrdIgnoreMplsExp:
            SHR_IF_ERR_EXIT(dnx_ofr_prd_ignore_mpls_exp_hw_set(unit, prd_profile, val));
            break;
        case imbImbPrdIgnoreInnerTag:
            SHR_IF_ERR_EXIT(dnx_ofr_prd_ignore_inner_tag_hw_set(unit, prd_profile, val));
            break;
        case imbImbPrdIgnoreOuterTag:
            SHR_IF_ERR_EXIT(dnx_ofr_prd_ignore_outer_tag_hw_set(unit, prd_profile, val));
            break;
        case imbImbPrdDefaultPriority:
            SHR_IF_ERR_EXIT(dnx_ofr_prd_default_priority_hw_set(unit, prd_profile, val));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid PRD property %d\n", property);
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get PRD property value
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] property - PRD property to get.
 * see enum imb_prd_properties_t.
 * \param [out] val - returned property value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_ofr_prd_properties_get(
    int unit,
    bcm_port_t port,
    imb_prd_properties_t property,
    uint32 *val)
{
    uint32 prd_profile;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get prd profile for the given port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_prd_profile_get(unit, port, &prd_profile));

    switch (property)
    {
        case imbImbPrdIgnoreIpDscp:
            SHR_IF_ERR_EXIT(dnx_ofr_prd_ignore_ip_dscp_hw_get(unit, prd_profile, val));
            break;
        case imbImbPrdIgnoreMplsExp:
            SHR_IF_ERR_EXIT(dnx_ofr_prd_ignore_mpls_exp_hw_get(unit, prd_profile, val));
            break;
        case imbImbPrdIgnoreInnerTag:
            SHR_IF_ERR_EXIT(dnx_ofr_prd_ignore_inner_tag_hw_get(unit, prd_profile, val));
            break;
        case imbImbPrdIgnoreOuterTag:
            SHR_IF_ERR_EXIT(dnx_ofr_prd_ignore_outer_tag_hw_get(unit, prd_profile, val));
            break;
        case imbImbPrdDefaultPriority:
            SHR_IF_ERR_EXIT(dnx_ofr_prd_default_priority_hw_get(unit, prd_profile, val));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid PRD property %d\n", property);
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - set configurable ether type to be recognized by the
 *        PRD Parser
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code to recognize
 *        the new ether type (1-6)
 * \param [in] ether_type_val - ether type value
 *
 * \return
 *   int  -see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_ofr_prd_custom_ether_type_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 ether_type_val)
{
    uint32 ether_type_code_max;
    SHR_FUNC_INIT_VARS(unit);

    ether_type_code_max = dnx_data_nif.prd.custom_ether_type_code_max_get(unit);

    SHR_IF_ERR_EXIT(dnx_ofr_prd_custom_ether_type_hw_set(unit, ether_type_code - 1, ether_type_val));

    if (ether_type_code == ether_type_code_max)
    {
        /*
         * The last configurable ether type (with ether type code 7) is saved for TM port types. the way to save it is
         * to always configure ether type code 6 and ether type code 7 to the same value. so if the user want to
         * configure ether type code 6, we copy the same value to ether type code 7
         */
        SHR_IF_ERR_EXIT(dnx_ofr_prd_custom_ether_type_hw_set(unit, ether_type_code, ether_type_val));
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get ether type mapped to a specific configurable
 *        ether type  code
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - configurable ether type code to
 *        get (1-6)
 * \param [in] ether_type_val - eter type value mapped to the
 *        given ether type code.
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_ofr_prd_custom_ether_type_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *ether_type_val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ofr_prd_custom_ether_type_hw_get(unit, ether_type_code - 1, ether_type_val));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - set MPLS special label properties. the special
 *        labels are compared globally against all the MPLS ports
 *        of the same ETHU.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] label_index - label index to set (0-3)
 * \param [in] label_config - special label configuration.
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_ofr_prd_mpls_special_label_set(
    int unit,
    bcm_port_t port,
    uint32 label_index,
    const bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ofr_prd_mpls_special_label_hw_set(unit, label_index, label_config));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get MPLS special label properties
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] label_index - label index to get (0-3)
 * \param [out] label_config - label properties
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_ofr_prd_mpls_special_label_get(
    int unit,
    bcm_port_t port,
    uint32 label_index,
    bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ofr_prd_mpls_special_label_hw_get(unit, label_index, label_config));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - set PRD threshold value per priority per port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] priority - priority for the threshold
 * \param [in] threshold - threshold value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_ofr_prd_threshold_set(
    int unit,
    bcm_port_t port,
    uint32 priority,
    uint32 threshold)
{
    dnx_algo_port_rmc_info_t rmc;
    uint32 rmc_fifo_size, rmc_fifo_resolution, max_threshold;
    int nif_interface_id, core;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * get RMC
     */
    SHR_IF_ERR_EXIT(dnx_cosq_prd_threshold_rmc_get(unit, port, priority, &rmc));

    

    /*
     * get port nif interface id
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, port, 0, &core, &nif_interface_id));

    /*
     * Get FIFO size
     */
    SHR_IF_ERR_EXIT(dnx_port_link_list_db_nof_sections_allocated_get
                    (unit, DNX_PORT_NIF_OFR_LINK_LIST, nif_interface_id, &rmc_fifo_size));

    rmc_fifo_resolution = dnx_data_nif.prd.rmc_fifo_2_threshold_resolution_get(unit);
    max_threshold = rmc_fifo_size * rmc_fifo_resolution;
    if (threshold > max_threshold)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Threshold %d for priority %d of port %d is higher than the fifo size %d\n",
                     threshold, priority, port, max_threshold);
    }

    SHR_IF_ERR_EXIT(dnx_ofr_prd_threshold_hw_set(unit, rmc.rmc_id, priority, threshold));
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get Threshold value for the given priority
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] priority - priority to get threshold
 * \param [in] threshold - returned threshold
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_ofr_prd_threshold_get(
    int unit,
    bcm_port_t port,
    uint32 priority,
    uint32 *threshold)
{
    dnx_algo_port_rmc_info_t rmc;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_prd_threshold_rmc_get(unit, port, priority, &rmc));

    SHR_IF_ERR_EXIT(dnx_ofr_prd_threshold_hw_get(unit, rmc.rmc_id, priority, threshold));
exit:
    SHR_FUNC_EXIT;

}

int
dnx_ofr_prd_threshold_max_get(
    int unit,
    bcm_port_t port,
    uint32 *threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    *threshold = dnx_data_nif.prd.rmc_threshold_max_get(unit);

    SHR_FUNC_EXIT;
}

/**
 * \brief - set an entry to the TCAM table in the PRD soft stage
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] key_index - index in the TCAM
 * \param [in] entry_info - info of the TCAM entry
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_ofr_prd_flex_key_entry_set(
    int unit,
    bcm_port_t port,
    uint32 key_index,
    const dnx_cosq_prd_tcam_entry_info_t * entry_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ofr_prd_tcam_entry_hw_set(unit, key_index, entry_info));
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get an entry from the PRD soft stage TCAM table
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] key_index - index in the TCAM table
 * \param [in] entry_info - returned entry info.
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_ofr_prd_flex_key_entry_get(
    int unit,
    bcm_port_t port,
    uint32 key_index,
    dnx_cosq_prd_tcam_entry_info_t * entry_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ofr_prd_tcam_entry_hw_get(unit, key_index, entry_info));

exit:
    SHR_FUNC_EXIT;

}

int
dnx_ofr_prd_tcam_entry_key_offset_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_index,
    uint32 offset_value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ofr_prd_tcam_entry_key_offset_hw_set(unit, ether_type_code, offset_index, offset_value));

exit:
    SHR_FUNC_EXIT;

}

int
dnx_ofr_prd_tcam_entry_key_offset_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_index,
    uint32 *offset_value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ofr_prd_tcam_entry_key_offset_hw_get(unit, ether_type_code, offset_index, offset_value));

exit:
    SHR_FUNC_EXIT;
}

int
dnx_ofr_prd_tcam_entry_key_offset_base_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_base)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ofr_prd_tcam_entry_key_offset_base_hw_set(unit, ether_type_code, offset_base));

exit:
    SHR_FUNC_EXIT;

}

int
dnx_ofr_prd_tcam_entry_key_offset_base_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *offset_base)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ofr_prd_tcam_entry_key_offset_base_hw_get(unit, ether_type_code, offset_base));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - set all fixed ether types sizes. (in bytes). this
 *        function is called only once per port, when the PRD is
 *        enabled (fixed ether types should always have the
 *        same size). the configurable ether types sizes are set
 *        by the user, see API
 *        imb_ethu_prd_flex_key_construct_set
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code
 * \param [in] ether_type_size - ether type size
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_ofr_prd_ether_type_size_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 ether_type_size)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ofr_prd_ether_type_size_hw_set(unit, ether_type_code, ether_type_size));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get ether types sizes. (in bytes).
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code
 * \param [out] ether_type_size - ether type size
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_ofr_prd_ether_type_size_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *ether_type_size)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ofr_prd_ether_type_size_hw_get(unit, ether_type_code, ether_type_size));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set control frame properties to be recognized by the
 *        PRD. if a control packet is recognized (each packet is
 *        compared against all control planes defined for the
 *        ETHU), it gets the highest priority
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] control_frame_index - index of control frame to
 *        configure (0-3)
 * \param [in] control_frame_config - control frame properties
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_ofr_prd_control_frame_set(
    int unit,
    bcm_port_t port,
    uint32 control_frame_index,
    const bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ofr_prd_control_frame_hw_set(unit, control_frame_index, control_frame_config));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get Control frame properties recognized by the PRD
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] control_frame_index - index of control frame to
 *        get
 * \param [out] control_frame_config - returned control frame
 *        properties
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_ofr_prd_control_frame_get(
    int unit,
    bcm_port_t port,
    uint32 control_frame_index,
    bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ofr_prd_control_frame_hw_get(unit, control_frame_index, control_frame_config));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get port profile of a port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] prd_profile - prd profile assigned to a port
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_ofr_prd_port_profile_map_get(
    int unit,
    bcm_port_t port,
    uint32 *prd_profile)
{
    int nif_interface_id;
    int core;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, port, 0, &core, &nif_interface_id));

    SHR_IF_ERR_EXIT(dnx_algo_port_db.prd.port_to_profile_map.get(unit, core, nif_interface_id, prd_profile));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set port profile to a port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] prd_profile - prd profile to assign to port
 *        get
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_ofr_prd_port_profile_map_set(
    int unit,
    bcm_port_t port,
    uint32 prd_profile)
{
    int nif_interface_id;
    bcm_port_prio_config_t priority_config;
    bcm_pbmp_t nif_pbmp;
    int ii, rmc_id, prd_prio, port_iter;
    uint8 rmc_group_found = FALSE, is_first_port_on_profile = TRUE;
    uint32 profile_rmc_group[DNX_DATA_MAX_NIF_PRD_NOF_PRIORITIES], rmc_group, profile;
    int core;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, port, 0, &core, &nif_interface_id));

    /*
     * In case mapping to non-default profile, need to check that the added port priority configuration is aligned to
     * profile 
     */
    if (prd_profile != DNX_PORT_NIF_OFR_DEFAULT_PRD_PROFILE)
    {
        /*
         * get priorities of this port
         */
        SHR_IF_ERR_EXIT(dnx_port_ofr_port_priority_config_internal_get(unit, port, &priority_config));

        /** NIF ports */
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                        (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF, DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_ELK,
                         &nif_pbmp));

            /** iterate over the ports bitmap and search for another port on same profile */
        _SHR_PBMP_ITER(nif_pbmp, port_iter)
        {
            if (port != port_iter)
            {
                SHR_IF_ERR_EXIT(dnx_ofr_prd_port_profile_map_get(unit, port_iter, &profile));
                if (prd_profile == profile)
                {
                    is_first_port_on_profile = FALSE;
                    break;
                }
            }
        }

        if (!is_first_port_on_profile)
        {
            SHR_IF_ERR_EXIT(dnx_ofr_profile_rx_priority_to_rmc_map_get(unit, prd_profile, profile_rmc_group));

            for (ii = 0; ii < priority_config.nof_priority_groups; ++ii)
            {
                SHR_IF_ERR_EXIT(dnx_port_ofr_rmc_id_get(unit, port, &priority_config, ii, &rmc_id));
                rmc_group = rmc_id < dnx_data_nif.ofr.nof_rmc_per_priority_group_get(unit) ? 0 : 1;

                rmc_group_found = FALSE;
                for (prd_prio = 0; prd_prio < dnx_data_nif.prd.nof_priorities_get(unit); prd_prio++)
                {
                    if (rmc_group == profile_rmc_group[prd_prio])
                    {
                        rmc_group_found = TRUE;
                        break;
                    }
                }
                if (!rmc_group_found)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Trying to change profile which other ports are already assigned to\n");
                }
            }
        }
    }

    /*
     * Save the mapping in sw state
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.prd.port_to_profile_map.set(unit, core, nif_interface_id, prd_profile));

    /*
     * Set the mapping in hw
     */
    SHR_IF_ERR_EXIT(dnx_ofr_rx_prd_port_profile_map_hw_set(unit, nif_interface_id, prd_profile));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get port profile of a port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] is_single_port_on_profile - is single priority on port
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_ofr_prd_is_single_port_on_profile_get(
    int unit,
    bcm_port_t port,
    uint8 *is_single_port_on_profile)
{
    bcm_port_t port_iter;
    bcm_pbmp_t nif_pbmp;
    uint32 prd_profile, profile;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ofr_prd_port_profile_map_get(unit, port, &prd_profile));

    /** NIF ports */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF, DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_ELK,
                     &nif_pbmp));

    /** iterate over the ports bitmap and search for another port on same profile */
    _SHR_PBMP_ITER(nif_pbmp, port_iter)
    {
        if (port != port_iter)
        {
            SHR_IF_ERR_EXIT(dnx_ofr_prd_port_profile_map_get(unit, port_iter, &profile));
            if (prd_profile == profile)
            {
                *is_single_port_on_profile = FALSE;
                break;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable PRD Hard stage per port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable indication
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_ofr_prd_hard_stage_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable)
{
    uint32 max_rmcs_per_port;
    int rmc_index;
    dnx_algo_port_rmc_info_t rmc;
    SHR_FUNC_INIT_VARS(unit);

    max_rmcs_per_port = dnx_data_nif.eth.priority_groups_nof_get(unit);

    for (rmc_index = 0; rmc_index < max_rmcs_per_port; ++rmc_index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, rmc_index, &rmc));
        if (rmc.rmc_id == IMB_COSQ_RMC_INVALID)
        {
            /*
             * RMC is invalid
             */
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_ofr_prd_hard_stage_enable_hw_set(unit, rmc.rmc_id, enable));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get enable indication per port for the PRD hard
 *        stage
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable indication
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_ofr_prd_hard_stage_enable_get(
    int unit,
    bcm_port_t port,
    uint32 *enable)
{
    uint32 max_rmcs_per_port;
    int rmc_index;
    dnx_algo_port_rmc_info_t rmc;
    SHR_FUNC_INIT_VARS(unit);

    max_rmcs_per_port = dnx_data_nif.eth.priority_groups_nof_get(unit);

    for (rmc_index = 0; rmc_index < max_rmcs_per_port; ++rmc_index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, rmc_index, &rmc));
        if (rmc.rmc_id == IMB_COSQ_RMC_INVALID)
        {
            /*
             * RMC is invalid
             */
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_ofr_prd_hard_stage_enable_hw_get(unit, rmc.rmc_id, enable));
        break;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - enable/disable PRD for the port. this call should be
 *        last, after the PRD is already configured.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable_mode - enable modes for the PRD:
 * bcmCosqIngressPortDropDisable
 * bcmCosqIngressPortDropEnableHardStage
 * bcmCosqIngressPortDropEnableHardAndSoftStage
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * it is required to call header type set before enabling
 *     the PRD.
 * \see
 *   * None
 */
int
dnx_ofr_prd_enable_set(
    int unit,
    bcm_port_t port,
    int enable_mode)
{
    uint32 prd_profile;
    uint32 en_soft_stage_eth = 0, en_soft_stage_tm = 0;
    uint32 is_eth_port = 0, is_tm_port = 0;
    uint32 port_type = 0;
    uint32 outer_tag_size = 0;
    int tc_offset = 0;
    int dp_offset = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get prd profile for the given port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_prd_profile_get(unit, port, &prd_profile));

    if (enable_mode == bcmCosqIngressPortDropDisable)
    {
        /*
         * Disable Hard stage
         */
        SHR_IF_ERR_EXIT(dnx_ofr_prd_hard_stage_enable_set(unit, port, 0));

        /*
         * Disable Soft Stage
         */
        SHR_IF_ERR_EXIT(dnx_ofr_prd_soft_stage_enable_hw_set(unit, prd_profile, 0, 0));

    }
    else
    {

        /*
         * set TC/DP offsets for ITMH/FTMH
         */
        SHR_IF_ERR_EXIT(dnx_cosq_prd_itmh_offsets_get(unit, port, &tc_offset, &dp_offset));
        SHR_IF_ERR_EXIT(dnx_ofr_prd_itmh_offsets_hw_set(unit, tc_offset, dp_offset));

        SHR_IF_ERR_EXIT(dnx_cosq_prd_ftmh_offsets_get(unit, port, &tc_offset, &dp_offset));
        SHR_IF_ERR_EXIT(dnx_ofr_prd_ftmh_offsets_hw_set(unit, tc_offset, dp_offset));
        /*
         * Get is port extender
         */
        SHR_IF_ERR_EXIT(dnx_cosq_prd_outer_tag_size_get(unit, port, &outer_tag_size));
        SHR_IF_ERR_EXIT(dnx_ofr_prd_outer_tag_size_hw_set(unit, prd_profile, outer_tag_size));

        /*
         * Set Ether type sizes for fixed Ether types
         */
        SHR_IF_ERR_EXIT(dnx_cosq_prd_ether_type_size_set(unit, port));

        /*
         * Set prd port type
         */
        SHR_IF_ERR_EXIT(dnx_cosq_prd_port_type_get(unit, port, &port_type, &is_eth_port, &is_tm_port));
        SHR_IF_ERR_EXIT(dnx_ofr_prd_port_type_hw_set(unit, prd_profile, port_type));

        /*
         * Enable soft stage
         */
        en_soft_stage_eth = ((enable_mode == bcmCosqIngressPortDropEnableHardAndSoftStage) && (is_eth_port)) ? 1 : 0;
        en_soft_stage_tm = ((enable_mode == bcmCosqIngressPortDropEnableHardAndSoftStage) && (is_tm_port)) ? 1 : 0;
        SHR_IF_ERR_EXIT(dnx_ofr_prd_soft_stage_enable_hw_set(unit, prd_profile, en_soft_stage_eth, en_soft_stage_tm));

        /*
         * Enable Hard stage
         */
        SHR_IF_ERR_EXIT(dnx_ofr_prd_hard_stage_enable_set(unit, port, 1));
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get PRD enable mode per port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable_mode - PRD enable modes:
 *  bcmCosqIngressPortDropDisable
 *  bcmCosqIngressPortDropEnableHardStage
 *  bcmCosqIngressPortDropEnableHardAndSoftStage
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_ofr_prd_enable_get(
    int unit,
    bcm_port_t port,
    int *enable_mode)
{
    uint32 is_eth_port = 0, is_tm_port = 0, en_hard_stage, en_soft_stage_eth = 0, en_soft_stage_tm = 0;
    uint32 prd_profile;
    uint32 port_type = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ofr_prd_hard_stage_enable_get(unit, port, &en_hard_stage));

    if (en_hard_stage)
    {
        /*
         * Get prd profile for the given port
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_prd_profile_get(unit, port, &prd_profile));

        /*
         * The expectation is that eth soft stage will only be set for eth ports, and tm soft stage will only be set
         * for tm ports. but we check for the port type just in case
         */
        SHR_IF_ERR_EXIT(dnx_cosq_prd_port_type_get(unit, port, &port_type, &is_eth_port, &is_tm_port));

        SHR_IF_ERR_EXIT(dnx_ofr_prd_soft_stage_enable_hw_get(unit, prd_profile, &en_soft_stage_eth, &en_soft_stage_tm));

        if (((en_soft_stage_eth && is_eth_port) || (en_soft_stage_tm && is_tm_port))
            && port_type != BCM_SWITCH_PORT_HEADER_TYPE_NONE)
        {
            *enable_mode = bcmCosqIngressPortDropEnableHardAndSoftStage;
        }
        else
        {
            *enable_mode = bcmCosqIngressPortDropEnableHardStage;
        }
    }
    else
    {
        *enable_mode = bcmCosqIngressPortDropDisable;
    }

exit:
    SHR_FUNC_EXIT;
}
