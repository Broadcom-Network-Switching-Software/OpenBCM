/** \file ppp_term.c
 *  PPP TUNNEL termination functionality for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PPP
/*
 * Include files.
 * {
 */
#include <bcm/ppp.h>
#include <bcm/vlan.h>

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

#include <bcm_int/dnx/stat/stat_pp.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>
#include <bcm_int/dnx/qos/qos.h>

#include <shared/shrextend/shrextend_debug.h>
/*
 * }
 */

/*
 * MACROs and ENUMs
 * {
 */
/** Total number of supported PPP termination Lookup type*/
#define DNX_PPP_TERM_LOOKUP_TYPE_NUM    0x2
#define DNX_PPP_TERM_SESSION_ID_MAX     0xffff
#define DNX_PPP_TERM_TUNNEL_ID_MAX      0xffff
#define DNX_PPP_TERM_NETWORK_DOMAIN_MAX 0x1ff
/*
 * End of MACROs
 * }
 */

/*
 * End of MACROs
 * }
 */

/*
 * Verify functions
 * {
 */

/**
 * \brief
 * Verify that PPP type is supported.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] type - PPP terminator type
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error.
 */
static shr_error_e
dnx_ppp_terminator_type_verify(
    int unit,
    bcm_ppp_type_t type)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify that PPP type is supported */
    switch (type)
    {
        case bcmPPPTypePPPoE:
        case bcmPPPTypeL2TPv2:
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "PPP termination: invalid type value (%d)", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify function for BCM-API: bcm_dnx_ppp_terminator_create
 *
 * \param [in] unit - Relevant unit.
 * \param [in] info - A pointer to the struct that holds information for the PPP terminator object \n
 *   [in] info->type - See bcmPPPTypeXXX \n
 *        bcmPPPTypePPPoE - PPPoE tunnel \n
 *        bcmPPPTypeL2TPv2 - L2TPv2 tunnel \n
 *   [in] info->flags - See BCM_PPP_TERM_XXX \n
 *        BCM_PPP_TERM_WITH_ID - Create object with ID \n
 *        BCM_PPP_TERM_REPLACE - Update existing object \n
 *        BCM_PPP_TERM_STAT_ENABLE - Statistics enabled \n
 *        BCM_PPP_TERM_SESSION_ANTI_SPOOFING - PPPoE session spoofing check enabled \n
 *        BCM_PPP_TERM_CROSS_CONNECT - P2P mode \n
 *   [in] info->src_mac - Source MAC address for PPPoE \n
 *   [in] info->session_id - Session ID for both PPPoE and L2TPv2 \n
 *   [in] info->l2tpv2_tunnel_id - Tunnel ID for L2TPv2 \n
 *   [in] info->network_domain - Distinguish multiple networks for L2TPv2.
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error.
 */
static shr_error_e
dnx_ppp_terminator_create_verify(
    int unit,
    bcm_ppp_terminator_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    /** Verify that PPP type is supported */
    SHR_INVOKE_VERIFY_DNX(dnx_ppp_terminator_type_verify(unit, info->type));

    /** Check REPLACE flag, must be used with WITH_ID flag */
    if (_SHR_IS_FLAG_SET(info->flags, BCM_PPP_TERM_REPLACE) && !_SHR_IS_FLAG_SET(info->flags, BCM_PPP_TERM_WITH_ID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_PPP_TERM_REPLACE can't be used without BCM_PPP_TERM_WITH_ID as well");
    }

    /** Verify WITH_ID */
    if (_SHR_IS_FLAG_SET(info->flags, BCM_PPP_INIT_WITH_ID))
    {
        /** Check Tunnel Gport is available */
        if (!BCM_GPORT_IS_TUNNEL(info->ppp_terminator_id))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel Gport is not given when BCM_PPP_INIT_WITH_ID");
        }
    }

    /** In-lif wide data feature is not available for PPP termination */
    if (_SHR_IS_FLAG_SET(info->flags, BCM_PPP_TERM_WIDE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flag BCM_PPP_TERM_WIDE is not support");
    }

    /** Check key fields is set */
    if (info->type == bcmPPPTypeL2TPv2)
    {
        /** Check key fields: {Next_Layer_Network_Domain(11), TUNNEL_ID(16) SESSION_ID(16)}  */
        if (info->session_id > DNX_PPP_TERM_SESSION_ID_MAX)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "PPP termination: invalid session id 0x%x", info->session_id);
        }
        if (info->l2tpv2_tunnel_id > DNX_PPP_TERM_TUNNEL_ID_MAX)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "PPP termination: invalid l2tpv2 tuennel id 0x%x", info->l2tpv2_tunnel_id);
        }
        if (info->network_domain > DNX_PPP_TERM_NETWORK_DOMAIN_MAX)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "PPP termination: invalid network_domain 0x%x", info->network_domain);
        }
    }
    else if (info->type == bcmPPPTypePPPoE)
    {
        /** Set key fields: {MAC_ADDRESS(48)(SA), SESSION_ID(16)} */
        if (BCM_MAC_IS_ZERO(info->src_mac))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "PPP termination: MAC address can't be a 0 MAC address");
        }
        if (info->session_id > DNX_PPP_TERM_SESSION_ID_MAX)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "PPP termination: invalid session id 0x%x", info->session_id);
        }
    }

    /** Verify qos model.*/
    if ((info->ingress_qos_model.ingress_remark > bcmQosIngressModelUniform) ||
        (info->ingress_qos_model.ingress_phb > bcmQosIngressModelUniform) ||
        (info->ingress_qos_model.ingress_ttl > bcmQosIngressModelUniform) ||
        (info->ingress_qos_model.ingress_ecn > bcmQosIngressEcnModelInvalid))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "PPP termination: one or more Ingress qos model remark(%d), phb(%d), ttl(%d) ECN(%d) are out of range",
                     info->ingress_qos_model.ingress_remark, info->ingress_qos_model.ingress_phb,
                     info->ingress_qos_model.ingress_ttl, info->ingress_qos_model.ingress_ecn);
    }
    if ((info->ingress_qos_model.ingress_remark == bcmQosIngressModelPipe) ||
        (info->ingress_qos_model.ingress_phb == bcmQosIngressModelPipe) ||
        (info->ingress_qos_model.ingress_ttl == bcmQosIngressModelPipe))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "PPP termination: Pipe mode is not supported, Please use Short Pipe.  remark(%d), phb(%d), ttl(%d)",
                     info->ingress_qos_model.ingress_remark, info->ingress_qos_model.ingress_phb,
                     info->ingress_qos_model.ingress_ttl);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify function for BCM-API: bcm_dnx_ppp_terminator_get
 *
 * \param [in] unit - Relevant unit.
 * \param [in] info - A pointer to the struct that holds information for the PPP terminator object \n
 *   [in] info->type - See bcmPPPTypeXXX \n
 *        bcmPPPTypePPPoE - PPPoE tunnel \n
 *        bcmPPPTypeL2TPv2 - L2TPv2 tunnel \n
 *   [in] info->ppp_terminator_id - PPP Tunnel Gport.
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error.
 */
static shr_error_e
dnx_ppp_terminator_get_verify(
    int unit,
    bcm_ppp_terminator_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    /** Verify that PPP type is supported */
    SHR_INVOKE_VERIFY_DNX(dnx_ppp_terminator_type_verify(unit, info->type));

    /** Check Tunnel Gport is available */
    if (!BCM_GPORT_IS_TUNNEL(info->ppp_terminator_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel Gport is not given");
    }

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify function for BCM-API: bcm_dnx_ppp_terminator_delete
 *
 * \param [in] unit - Relevant unit.
 * \param [in] info - A pointer to the struct that holds information for the PPP terminator object \n
 *   [in] info->type - See bcmPPPTypeXXX \n
 *        bcmPPPTypePPPoE - PPPoE tunnel \n
 *        bcmPPPTypeL2TPv2 - L2TPv2 tunnel \n
 *   [in] info->ppp_terminator_id - PPP Tunnel Gport.
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error.
 */
static shr_error_e
dnx_ppp_terminator_delete_verify(
    int unit,
    bcm_ppp_terminator_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    /** Verify that PPP type is supported */
    SHR_INVOKE_VERIFY_DNX(dnx_ppp_terminator_type_verify(unit, info->type));

    /** Check Tunnel Gport is available */
    if (!BCM_GPORT_IS_TUNNEL(info->ppp_terminator_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel Gport is not given");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify function for BCM-API: bcm_dnx_ppp_terminator_traverse
 *
 * \param [in] unit - Relevant unit.
 * \param [in] additional_info - A pointer to the struct that holds addtional information
 * \param [in] cb - user callback
 * \param [in] user_data - user data
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error.
 */
static shr_error_e
dnx_ppp_terminator_traverse_verify(
    int unit,
    bcm_ppp_terminator_additional_info_t * additional_info,
    bcm_ppp_terminator_traverse_cb cb,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(additional_info, _SHR_E_PARAM, "additional info");
    SHR_NULL_CHECK(cb, _SHR_E_PARAM, "user callback");
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify function for BCM-API: bcm_dnx_ppp_term_spoofing_check_add
 * \param [in] unit - Relevant unit.
 * \param [in] info - A pointer to the struct that holds information for spoofing check
 *   [in] info->ppp_terminator_id - PPP Tunnel Gport.
 *   [in] info->vlan_port_id - Vlan Gport.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
static int
dnx_ppp_term_spoofing_check_add_verify(
    int unit,
    bcm_ppp_term_spoofing_check_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!BCM_GPORT_IS_TUNNEL(info->ppp_terminator_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Gport = 0x%08X is not a Tunnel PORT\n", info->ppp_terminator_id);
    }
    if (!BCM_GPORT_IS_VLAN_PORT(info->vlan_port_id) && !BCM_GPORT_IS_TUNNEL(info->vlan_port_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Gport = 0x%08X is not a VLAN PORT or a TUNNEL PORT\n", info->vlan_port_id);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify function for BCM-API: bcm_dnx_ppp_term_spoofing_check_delete
 * \param [in] unit - Relevant unit.
 * \param [in] info - A pointer to the struct that holds information for spoofing check
 *   [in] info->ppp_terminator_id - PPP Tunnel Gport.
 *   [in] info->vlan_port_id - Vlan Gport.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
static int
dnx_ppp_term_spoofing_check_delete_verify(
    int unit,
    bcm_ppp_term_spoofing_check_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!BCM_GPORT_IS_TUNNEL(info->ppp_terminator_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Gport = 0x%08X is not a Tunnel PORT\n", info->ppp_terminator_id);
    }
    if (!BCM_GPORT_IS_VLAN_PORT(info->vlan_port_id) && !BCM_GPORT_IS_TUNNEL(info->vlan_port_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Gport = 0x%08X is not a VLAN PORT or a TUNNEL PORT\n", info->vlan_port_id);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify function for BCM-API: bcm_dnx_ppp_term_spoofing_check_traverse
 * \param [in] unit - Relevant unit.
 * \param [in] additional_info - the additional info for traversing.
 * \param [in] cb - the name of the callback function,
 *          it receives the value of the user_data variable and
 *          the key and result values for each found entry.
 * \param [in] user_data - user data that will be sent to the callback function
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
static int
dnx_ppp_term_spoofing_check_traverse_verify(
    int unit,
    bcm_ppp_term_spoofing_check_additional_info_t * additional_info,
    bcm_ppp_term_spoofing_check_traverse_cb cb,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(additional_info, _SHR_E_PARAM, "additional info");
    SHR_NULL_CHECK(cb, _SHR_E_PARAM, "user callback");
exit:
    SHR_FUNC_EXIT;
}

/*
 * End of Verify functions
 * }
 */

/*
 * Inner functions
 * {
 */

/**
 * \brief
 *  Add PPP termination VTT lookup entry (ISEM2).
 *
 * \param [in] unit - Relevant unit.f
 * \param [in] info - A pointer to the struct that holds information for the PPP terminator object \n
 *   [in] info->type - See bcmPPPTypeXXX \n
 *        bcmPPPTypePPPoE - PPPoE tunnel \n
 *        bcmPPPTypeL2TPv2 - L2TPv2 tunnel \n
 *   [in] info->ppp_terminator_id - PPP Tunnel Gport \n
 *   [in] info->src_mac - Source MAC address for PPPoE \n
 *   [in] info->session_id - Session ID for both PPPoE and L2TPv2 \n
 *   [in] info->l2tpv2_tunnel_id - Tunnel ID for L2TPv2 \n
 *   [in] info->network_domain - Distinguish multiple networks for L2TPv2.
 * \param [in] local_inlif - Local inLIF as lookup result.
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error.
 */
static shr_error_e
dnx_ppp_terminator_lookup_add(
    int unit,
    bcm_ppp_terminator_t * info,
    uint32 local_inlif)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (info->type == bcmPPPTypePPPoE)
    {
        /** PPPoE tunnel termination with full MAC */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PPPOE_O_ETH_TUNNEL_FULL_SA, &entry_handle_id));

        /** Set key fields: {MAC_ADDRESS(48)(SA), SESSION_ID(16)} */
        dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_MAC, info->src_mac);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SESSION_ID, info->session_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

        /** Set DATA field - {in LIF} */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_inlif);

        /** Write to HW */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else if (info->type == bcmPPPTypeL2TPv2)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L2TPV2_DATA_MESSAGE_TT, &entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN, info->network_domain);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_L2TPV2_TUNNEL_ID, info->l2tpv2_tunnel_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_L2TPV2_SESSION_ID, info->session_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

        /** Set DATA field - {in LIF} */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_inlif);

        /** Write to HW */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "PPP Tunnel termination unknown type(%d).", info->type);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Get termination VTT lookup keys per PPP type and terminator object.
 *
 * \param [in] unit - Relevant unit.
 * \param [in,out] info - A pointer to the struct that holds information for the PPP terminator object \n
 *   [in] info->type - See bcmPPPTypeXXX \n
 *        bcmPPPTypePPPoE - PPPoE tunnel \n
 *        bcmPPPTypeL2TPv2 - L2TPv2 tunnel \n
 *   [in] info->ppp_terminator_id - PPP Tunnel Gport \n
 *   [out]info->src_mac - Source MAC address for PPPoE \n
 *   [out]info->session_id - Session ID for both PPPoE and L2TPv2 \n
 *   [out]info->l2tpv2_tunnel_id - Tunnel ID for L2TPv2 \n
 *   [out]info->network_domain - Distinguish multiple networks for L2TPv2.
 * \param [in] local_inlif - Local inLIF as lookup result.
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error.
 */
static shr_error_e
dnx_ppp_terminator_lookup_get(
    int unit,
    bcm_ppp_terminator_t * info,
    uint32 local_inlif)
{
    uint32 entry_handle_id;
    uint32 dbal_table = DBAL_TABLE_PPPOE_O_ETH_TUNNEL_FULL_SA;
    int is_end = 0;
    uint32 field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (info->type == bcmPPPTypePPPoE)
    {
        dbal_table = DBAL_TABLE_PPPOE_O_ETH_TUNNEL_FULL_SA;
    }
    else if (info->type == bcmPPPTypeL2TPv2)
    {
        dbal_table = DBAL_TABLE_L2TPV2_DATA_MESSAGE_TT;
    }

    /** Traverse VTT lookup entries */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    /** Receive first entry in table. */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        /** Get lookup info about PPPoE termination */
        if (dbal_table == DBAL_TABLE_PPPOE_O_ETH_TUNNEL_FULL_SA)
        {
            /** Set pointer to receive field - {in LIF} */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, field_value));

            if (local_inlif == field_value[0])
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_get
                                (unit, entry_handle_id, DBAL_FIELD_L2_MAC, info->src_mac));

                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_SESSION_ID, &info->session_id));
                break;
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
                continue;
            }
        }
        /** Get lookup info about L2TPv2 termination */
        else if (dbal_table == DBAL_TABLE_L2TPV2_DATA_MESSAGE_TT)
        {
            /** Set pointer to receive field - {in LIF} */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, field_value));

            if (local_inlif == field_value[0])
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN, &info->network_domain));

                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_L2TPV2_TUNNEL_ID, &info->l2tpv2_tunnel_id));

                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_L2TPV2_SESSION_ID, &info->session_id));
                break;
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
                continue;
            }
        }

        /** Receive next entry in table.*/
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Delete PPP termination VTT lookup entry (ISEM2) per PPP type and terminator object.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] info - A pointer to the struct that holds information for the PPP terminator object \n
 *   [in] info->type - PPP Terminator type, see bcmPPPTypeXXX \n
 *        bcmPPPTypePPPoE - PPPoE tunnel \n
 *        bcmPPPTypeL2TPv2 - L2TPv2 tunnel \n
 *   [in] info->ppp_terminator_id - PPP Tunnel Gport.
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error.
 */
static shr_error_e
dnx_ppp_terminator_lookup_delete(
    int unit,
    bcm_ppp_terminator_t * info)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (info->type == bcmPPPTypePPPoE)
    {
        /** Take handle to delete from DBAL table */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PPPOE_O_ETH_TUNNEL_FULL_SA, &entry_handle_id));

        /** Set key fields: {MAC_ADDRESS(48)(SA), SESSION_ID(16)} */
        dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_MAC, info->src_mac);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SESSION_ID, info->session_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

        /** Delete entry from DBAL table*/
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }
    else if (info->type == bcmPPPTypeL2TPv2)
    {
        /** Take handle to delete from DBAL table */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L2TPV2_DATA_MESSAGE_TT, &entry_handle_id));

        /** Set lookup-specific tunnel termination key fields based on table_id */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN, info->network_domain);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_L2TPV2_TUNNEL_ID, info->l2tpv2_tunnel_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_L2TPV2_SESSION_ID, info->session_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

        /** Delete entry from DBAL table*/
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "PPP Tunnel termination unknown type(%d).", info->type);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Allocate local inLIF.
 *  Write to PPP termination inLIF table.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] table_id - DBAL table id.
 * \param [in] info - A pointer to the struct that holds information for the PPP terminator entry.
 *   [in] intf->flags - See BCM_PPP_TERM_XXX. \n
 *        BCM_PPP_TERM_WITH_ID - Create object with ID \n
 *        BCM_PPP_TERM_REPLACE - Update existing object\n
 *        BCM_PPP_TERM_STAT_ENABLE - Statistics enabled \n
 *        BCM_PPP_TERM_SESSION_ANTI_SPOOFING - PPPoE session spoofing check enabled \n
 *        BCM_PPP_TERM_CROSS_CONNECT - P2P mode.
 * \param [in] global_inlif - Allocated Glboal inLIF.
 * \param [out] local_inlif - Allocated Local inLIF.
 * \param [in] in_lif_profile - In lif profile.
 * \param [in] propagation_prof - Allocated propagation profile.
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error.
 */
static shr_error_e
dnx_ppp_terminator_allocate_lif_and_in_lif_table_set(
    int unit,
    dbal_tables_e table_id,
    bcm_ppp_terminator_t * info,
    uint32 in_lif_profile,
    uint32 global_inlif,
    uint32 *local_inlif,
    int propagation_prof)
{
    uint32 entry_handle_id;
    lif_table_mngr_inlif_info_t lif_table_mngr_inlif_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    sal_memset(&lif_table_mngr_inlif_info, 0, sizeof(lif_table_mngr_inlif_info_t));

    if (_SHR_IS_FLAG_SET(info->flags, BCM_PPP_TERM_REPLACE))
    {
        lif_table_mngr_inlif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_REPLACE;
    }

    lif_table_mngr_inlif_info.global_lif = global_inlif;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Result type is superset, will be decided internally by lif table manager */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, INST_SINGLE, global_inlif);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE,
                                 (uint32) in_lif_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, INST_SINGLE,
                                 (uint32) propagation_prof);

    /** Wide LIF can be either MP or P2P */
    if (_SHR_IS_FLAG_SET(info->flags, BCM_PPP_TERM_WIDE))
    {
        /**  Set GLOB_OUT_LIF to indicate wide LIF */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, 0);

        if (_SHR_IS_FLAG_SET(info->flags, BCM_PPP_TERM_CROSS_CONNECT))
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SERVICE_TYPE, INST_SINGLE,
                                         DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P);
            /** DESTINATION is supposed to be configured by API bcm_vswitch_cross_connect_add.*/
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, 0);
        }
        else
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SERVICE_TYPE, INST_SINGLE,
                                         DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2MP);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO_ASSIGNMENT_MODE, INST_SINGLE,
                                         DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LIF);
            /** FORWARD_DOMAIN is supposed to be updated by API bcm_l3_ingress_create.*/
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO, INST_SINGLE, 0);
        }
    }
    else if (_SHR_IS_FLAG_SET(info->flags, BCM_PPP_TERM_CROSS_CONNECT))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SERVICE_TYPE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P);
        /** DESTINATION is supposed to be configured by API bcm_vswitch_cross_connect_add.*/
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, 0);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SERVICE_TYPE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2MP);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO_ASSIGNMENT_MODE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LIF);
        /** FORWARD_DOMAIN is supposed to be updated by bcm_l3_ingress_create API.*/
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO, INST_SINGLE, 0);
    }

    /** NEXT_LAYER_NETWORK_DOMAIN indicats to do PPPoE session anti-spoofing */
    if (_SHR_IS_FLAG_SET(info->flags, BCM_PPP_TERM_SESSION_ANTI_SPOOFING))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN, INST_SINGLE, 0);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN, INST_SINGLE, 1);
    }

    STAT_PP_CHECK_AND_SET_STAT_OBJECT_LIF_FIELDS(info->flags, BCM_PPP_TERM_STAT_ENABLE);

    /** Write to HW */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_local_inlif_and_set_hw
                    (unit, _SHR_CORE_ALL, entry_handle_id, (int *) local_inlif, &lif_table_mngr_inlif_info));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Read from PPP termination InLIF table and update info struct fields.
 *
 * \param [in] unit - Relevant unit.
 * \param [out] info - A pointer to the struct that holds information for the PPP terminator entry.
 *   [out]info->flags - See BCM_PPP_TERM_XXX. \n
 *        BCM_PPP_TERM_STAT_ENABLE - Statistics enabled \n
 *        BCM_PPP_TERM_SESSION_ANTI_SPOOFING - PPPoE session spoofing check enabled \n
 *        BCM_PPP_TERM_CROSS_CONNECT - P2P mode.
 * \param [in] local_inlif - Local inLIF.
 * \param [out] global_inlif - Global inLIF.
 * \param [out] in_lif_profile - In lif profile.
 * \param [out] propagation_prof - Template allocated QOS propagation_prof.
 * \return
 *  \retval Zero in case of NO ERROR.
 *  \retval Negative in case of an error.
 */
static shr_error_e
dnx_ppp_terminator_in_lif_table_get(
    int unit,
    bcm_ppp_terminator_t * info,
    uint32 local_inlif,
    uint32 *global_inlif,
    uint32 *in_lif_profile,
    int *propagation_prof)
{
    uint32 entry_handle_id;
    uint8 is_field_valid;
    lif_table_mngr_inlif_info_t inlif_info;
    uint32 dbal_field[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
    /** PPPoE and L2TPv2 use same inLIF format */
    dbal_tables_e table_id = DBAL_TABLE_IN_LIF_FORMAT_L2TP;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    /** Take handle to read from DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /**  Get tunnel inLIF info from lif tble manager  */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_inlif_info(unit, _SHR_CORE_ALL, local_inlif, entry_handle_id, &inlif_info));

    /** Get global inLIF */
    if (global_inlif)
    {
        sal_memset(dbal_field, 0, sizeof(dbal_field));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, INST_SINGLE, dbal_field));
        *global_inlif = dbal_field[0];
    }

    /** Get inLIF profile */
    if (in_lif_profile)
    {
        sal_memset(dbal_field, 0, sizeof(dbal_field));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, (uint32 *) dbal_field));
        *in_lif_profile = dbal_field[0];
    }

    /** Get QoS propagation profile */
    if (propagation_prof)
    {
        sal_memset(dbal_field, 0, sizeof(dbal_field));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, INST_SINGLE, (uint32 *) dbal_field));
        *propagation_prof = (int) (dbal_field[0]);
    }

    /** Get P2P */
    sal_memset(dbal_field, 0, sizeof(dbal_field));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SERVICE_TYPE, INST_SINGLE, dbal_field));
    if (dbal_field[0] == DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P)
    {
        info->flags |= BCM_PPP_TERM_CROSS_CONNECT;
    }

    /** Get spoofing check */
    sal_memset(dbal_field, 0, sizeof(dbal_field));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN, INST_SINGLE, dbal_field));
    if (dbal_field[0] == FALSE)
    {
        info->flags |= BCM_PPP_TERM_SESSION_ANTI_SPOOFING;
    }

    /** Get statistics enable flag */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                    (unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, &is_field_valid));
    if (is_field_valid == TRUE)
    {
        info->flags |= BCM_PPP_TERM_STAT_ENABLE;
    }
exit:
    DBAL_FUNC_FREE_VARS;
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
 * Create PPP terminator object and set some of its properties.
 *
 * \param [in] unit - Relevant unit.
 * \param [in,out] info - A pointer to the struct that holds information for the PPP terminator object \n
 *   [in] info->type - See bcmPPPTypeXXX \n
 *        bcmPPPTypePPPoE - PPPoE tunnel \n
 *        bcmPPPTypeL2TPv2 - L2TPv2 tunnel \n
 *   [in] info->flags - See BCM_PPP_TERM_XXX \n
 *        BCM_PPP_TERM_WITH_ID - Create object with ID \n
 *        BCM_PPP_TERM_REPLACE - Update existing object \n
 *        BCM_PPP_TERM_STAT_ENABLE - Statistics enabled \n
 *        BCM_PPP_TERM_SESSION_ANTI_SPOOFING - PPPoE session spoofing check enabled \n
 *        BCM_PPP_TERM_CROSS_CONNECT - P2P mode \n
 *   [in] info->src_mac - Source MAC address for PPPoE \n
 *   [in] info->session_id - Session ID for both PPPoE and L2TPv2 \n
 *   [in] info->l2tpv2_tunnel_id - Tunnel ID for L2TPv2 \n
 *   [in] info->network_domain - Distinguish multiple networks for L2TPv2 \n
 *   [in,out] info->ppp_terminator_id - PPP terminator object id.
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error.
 */
int
bcm_dnx_ppp_terminator_create(
    int unit,
    bcm_ppp_terminator_t * info)
{
    int global_lif = 0;
    uint32 local_inlif;
    int old_in_lif_profile = DNX_IN_LIF_PROFILE_DEFAULT;
    int new_in_lif_profile = DNX_IN_LIF_PROFILE_DEFAULT;
    in_lif_profile_info_t in_lif_profile_info;
    /** IN_LIF_FORMAT_PPPOE same as IN_LIF_FORMAT_L2TP */
    dbal_tables_e inlif_dbal_table_id = DBAL_TABLE_IN_LIF_FORMAT_L2TP;
    dnx_qos_propagation_type_e ttl_propagation_type;
    dnx_qos_propagation_type_e phb_propagation_type;
    dnx_qos_propagation_type_e remark_propagation_type;
    dnx_qos_propagation_type_e ecn_propagation_type;
    int propag_profile, old_propag_profile;
    uint8 qos_propagation_prof_first_ref;
    uint8 qos_propagation_prof_last_ref;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_ppp_terminator_create_verify(unit, info));

    /** Set QOS */
    phb_propagation_type = remark_propagation_type = ecn_propagation_type = ttl_propagation_type =
        DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE;

    if (info->ingress_qos_model.ingress_phb != bcmQosIngressModelInvalid)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_qos_model_to_propag_type
                        (unit, info->ingress_qos_model.ingress_phb, &phb_propagation_type));
    }

    if (info->ingress_qos_model.ingress_remark != bcmQosIngressModelInvalid)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_qos_model_to_propag_type
                        (unit, info->ingress_qos_model.ingress_remark, &remark_propagation_type));
    }

    if (info->ingress_qos_model.ingress_ttl != bcmQosIngressModelInvalid)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_qos_model_to_propag_type
                        (unit, info->ingress_qos_model.ingress_ttl, &ttl_propagation_type));
    }

    /** Allocate LIF and set its properies */
    if (!_SHR_IS_FLAG_SET(info->flags, BCM_PPP_TERM_REPLACE))
    {
        uint32 lif_alloc_flags = 0;

        /** Allocate global lif  */
        if (info->flags & BCM_PPP_TERM_WITH_ID)
        {
            lif_alloc_flags |= LIF_MNGR_GLOBAL_LIF_WITH_ID;
            global_lif = BCM_GPORT_TUNNEL_ID_GET(info->ppp_terminator_id);
        }

        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_allocate
                        (unit, lif_alloc_flags, DNX_ALGO_LIF_INGRESS, &global_lif));

        BCM_GPORT_TUNNEL_ID_SET(info->ppp_terminator_id, global_lif);

        /** Alloc propagation profile ID corresponding to chosen QOS model */
        old_propag_profile = DNX_QOS_INGRESS_DEFAULT_PROPAGATION_PROFILE;
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_alloc(unit, phb_propagation_type,
                                                                  remark_propagation_type, ecn_propagation_type,
                                                                  ttl_propagation_type, old_propag_profile,
                                                                  &propag_profile,
                                                                  &qos_propagation_prof_first_ref,
                                                                  &qos_propagation_prof_last_ref));
        /*
         * Update H/W
         */
        if (qos_propagation_prof_first_ref == TRUE)
        {
            SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_hw_set(unit, propag_profile, phb_propagation_type,
                                                                       remark_propagation_type,
                                                                       ecn_propagation_type, ttl_propagation_type));
        }

        /** No termination to PPP layer in the case P2P */
        if (_SHR_IS_FLAG_SET(info->flags, BCM_PPP_TERM_CROSS_CONNECT))
        {
            /** Allocate inLIF profile */
            SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));
            in_lif_profile_info.ingress_fields.l3_ingress_flags |= ENABLERS_DISABLE_SUPPORTED_FLAGS;
            SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                            (unit, &in_lif_profile_info, old_in_lif_profile, &new_in_lif_profile, LIF,
                             inlif_dbal_table_id));
        }

        SHR_IF_ERR_EXIT(dnx_ppp_terminator_allocate_lif_and_in_lif_table_set
                        (unit, inlif_dbal_table_id, info, (uint32) new_in_lif_profile, global_lif, &local_inlif,
                         propag_profile));

        SHR_IF_ERR_EXIT(dnx_ppp_terminator_lookup_add(unit, info, local_inlif));

    }
    else
    {
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
        bcm_ppp_terminator_t tmp_info;

        /** Get Local inLIF ID */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, info->ppp_terminator_id,
                         DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources));
        local_inlif = (uint32) gport_hw_resources.local_in_lif;

        tmp_info.type = info->type;
        /** Get entry from ISEM to find local inLIF */
        SHR_IF_ERR_EXIT(dnx_ppp_terminator_lookup_get(unit, &tmp_info, local_inlif));
        /** Delete lookup entry from ISEM */
        SHR_IF_ERR_EXIT(dnx_ppp_terminator_lookup_delete(unit, &tmp_info));

        /** Get existing profile */
        SHR_IF_ERR_EXIT(dnx_ppp_terminator_in_lif_table_get
                        (unit, info, gport_hw_resources.local_in_lif,
                         (uint32 *) &global_lif, (uint32 *) &old_in_lif_profile, &propag_profile));
        if (BCM_GPORT_TUNNEL_ID_GET(info->ppp_terminator_id) != global_lif)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "given tunnel_id (%d) != existed tunnel_id (%d)",
                         BCM_GPORT_TUNNEL_ID_GET(info->ppp_terminator_id), global_lif);
        }

        old_propag_profile = propag_profile;

        /** Update propagation profile ID. */
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_alloc
                        (unit, phb_propagation_type, remark_propagation_type,
                         ecn_propagation_type, ttl_propagation_type, old_propag_profile, &propag_profile,
                         &qos_propagation_prof_first_ref, &qos_propagation_prof_last_ref));

        /** Update the profile table if it is first used*/
        if (qos_propagation_prof_first_ref == TRUE)
        {
            SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_hw_set(unit, propag_profile, phb_propagation_type,
                                                                       remark_propagation_type,
                                                                       ecn_propagation_type, ttl_propagation_type));
        }

        /** Allocate inLIF profile */
        SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));

        /** No termination to PPP layer in the case P2P */
        if (_SHR_IS_FLAG_SET(info->flags, BCM_PPP_TERM_CROSS_CONNECT))
        {
            in_lif_profile_info.ingress_fields.l3_ingress_flags |= ENABLERS_DISABLE_SUPPORTED_FLAGS;
        }

        SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                        (unit, &in_lif_profile_info, old_in_lif_profile, &new_in_lif_profile, LIF,
                         inlif_dbal_table_id));

        /** Update LIF attributes */
        SHR_IF_ERR_EXIT(dnx_ppp_terminator_allocate_lif_and_in_lif_table_set
                        (unit, inlif_dbal_table_id, info, new_in_lif_profile, global_lif, &local_inlif,
                         propag_profile));

        SHR_IF_ERR_EXIT(dnx_ppp_terminator_lookup_add(unit, info, local_inlif));

        /** Clear the unused profile which is indicated by last-referrence*/
        if (qos_propagation_prof_last_ref == TRUE)
        {
            SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_hw_clear(unit, old_propag_profile));
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get PPP Tunnel terminator object.
 *
 * \param [in] unit - Relevant unit.
 * \param [in,out] info - A pointer to the struct that holds information for the PPP terminator entry \n
 *   [in] info->type - See bcmPPPTypeXXX \n
 *        bcmPPPTypePPPoE - PPPoE tunnel \n
 *        bcmPPPTypeL2TPv2 - L2TPv2 tunnel \n
 *   [in] info->ppp_terminator_id - PPP Tunnel Gport \n
 *   [out]info->flags - See BCM_PPP_TERM_XXX \n
 *        BCM_PPP_TERM_STAT_ENABLE - Statistics enabled \n
 *        BCM_PPP_TERM_SESSION_ANTI_SPOOFING - PPPoE session spoofing check enable \n
 *        BCM_PPP_TERM_CROSS_CONNECT - P2P mode \n
 *   [out]info->src_mac - Source MAC address for PPPoE \n
 *   [out]info->session_id - Session ID for both PPPoE and L2TPv2 \n
 *   [out]info->l2tpv2_tunnel_id - Tunnel ID for L2TPv2 \n
 *   [out]info->network_domain - Distinguish multiple networks for L2TPv2.
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error.
 */
int
bcm_dnx_ppp_terminator_get(
    int unit,
    bcm_ppp_terminator_t * info)
{
    uint32 lif_flags;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    int propagation_prof;
    dnx_qos_propagation_type_e phb_propagation_type;
    dnx_qos_propagation_type_e remark_propagation_type;
    dnx_qos_propagation_type_e ecn_propagation_type;
    dnx_qos_propagation_type_e ttl_propagation_type;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_ppp_terminator_get_verify(unit, info));

    /** Get local inLIF from GPort hw resources (global and local inLIF) */
    lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, info->ppp_terminator_id, lif_flags, &gport_hw_resources));

    /** Get Lookup entry */
    SHR_IF_ERR_EXIT(dnx_ppp_terminator_lookup_get(unit, info, gport_hw_resources.local_in_lif));

    /** Get inLIF entry */
    SHR_IF_ERR_EXIT(dnx_ppp_terminator_in_lif_table_get
                    (unit, info, gport_hw_resources.local_in_lif, NULL, NULL, &propagation_prof));

    /** Update QOS flags according to returned propagation_prof */
    SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_attr_get
                    (unit, propagation_prof, &phb_propagation_type, &remark_propagation_type, &ecn_propagation_type,
                     &ttl_propagation_type));

    if (phb_propagation_type != DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propag_type_to_qos_model
                        (unit, phb_propagation_type, &(info->ingress_qos_model.ingress_phb)));
    }

    if (remark_propagation_type != DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propag_type_to_qos_model
                        (unit, remark_propagation_type, &(info->ingress_qos_model.ingress_remark)));
    }

    if (ttl_propagation_type != DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propag_type_to_qos_model
                        (unit, ttl_propagation_type, &(info->ingress_qos_model.ingress_ttl)));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Delete PPP Tunnel terminator object.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] info - A pointer to the struct that holds information for the PPP terminator object \n
 *   [in] info->type - See bcmPPPTypeXXX \n
 *        bcmPPPTypePPPoE - PPPoE tunnel \n
 *        bcmPPPTypeL2TPv2 - L2TPv2 tunnel \n
 *   [in] info->ppp_terminator_id - PPP Tunnel Gport.
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error.
 */
int
bcm_dnx_ppp_terminator_delete(
    int unit,
    bcm_ppp_terminator_t * info)
{
    int new_in_lif_profile;
    uint32 in_lif_profile;
    uint32 lif_flags;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    int propagation_prof;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_ppp_terminator_delete_verify(unit, info));

    /** Get local inLIF from GPort hw resources (global and local inLIF) */
    lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, info->ppp_terminator_id, lif_flags, &gport_hw_resources));

    /** Get entry from ISEM to find local inLIF */
    SHR_IF_ERR_EXIT(dnx_ppp_terminator_lookup_get(unit, info, gport_hw_resources.local_in_lif));

    /** Get entry from PPP inLIF table */
    SHR_IF_ERR_EXIT(dnx_ppp_terminator_in_lif_table_get
                    (unit, info, gport_hw_resources.local_in_lif, NULL, &in_lif_profile, &propagation_prof));

    /** Deallocates in_lif_profile */
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_dealloc(unit, in_lif_profile, &new_in_lif_profile, LIF));

    /** Delete lookup entry from ISEM */
    SHR_IF_ERR_EXIT(dnx_ppp_terminator_lookup_delete(unit, info));

    /** Free propagation profile template */
    SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_free_and_hw_clear(unit, propagation_prof, NULL));

    /** Delete local inLIF SW and HW allocation */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_inlif_info_clear
                    (unit, _SHR_CORE_ALL, gport_hw_resources.local_in_lif, DBAL_TABLE_IN_LIF_FORMAT_L2TP, 0));

    /** Delete global inLIF */
    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_free
                    (unit, DNX_ALGO_LIF_INGRESS, BCM_GPORT_TUNNEL_ID_GET(info->ppp_terminator_id)));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Traverse all PPP Tunnel terminator objects.
 * \param [in] unit - Relevant unit.
 * \param [in] additional_info - A pointer to the struct that holds addtional information
 * \param [in] cb - user callback
 * \param [in] user_data - user data
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
int
bcm_dnx_ppp_terminator_traverse(
    int unit,
    bcm_ppp_terminator_additional_info_t * additional_info,
    bcm_ppp_terminator_traverse_cb cb,
    void *user_data)
{
    uint32 entry_handle_id;
    uint32 dbal_tables[DNX_PPP_TERM_LOOKUP_TYPE_NUM] = { 0 };
    uint32 nof_tables = 0;
    uint32 table, local_inlif, global_inlif;
    int is_end;
    int propagation_prof;
    dnx_qos_propagation_type_e phb_propagation_type;
    dnx_qos_propagation_type_e remark_propagation_type;
    dnx_qos_propagation_type_e ecn_propagation_type;
    dnx_qos_propagation_type_e ttl_propagation_type;
    bcm_ppp_terminator_t info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_ppp_terminator_traverse_verify(unit, additional_info, cb, user_data));

    /** Apply additional limit */
    if (additional_info->type == bcmPPPTypePPPoE)
    {
        dbal_tables[nof_tables] = DBAL_TABLE_PPPOE_O_ETH_TUNNEL_FULL_SA;
        nof_tables++;
    }
    else if (additional_info->type == bcmPPPTypeL2TPv2)
    {
        dbal_tables[nof_tables] = DBAL_TABLE_L2TPV2_DATA_MESSAGE_TT;
        nof_tables++;
    }
    else
    {
        dbal_tables[nof_tables] = DBAL_TABLE_PPPOE_O_ETH_TUNNEL_FULL_SA;
        nof_tables++;
        dbal_tables[nof_tables] = DBAL_TABLE_L2TPV2_DATA_MESSAGE_TT;
        nof_tables++;
    }

    /** Iterate over all tables and all their entries */
    for (table = 0; table < nof_tables; table++)
    {
        /*
         * Allocate handle to the table of the iteration and initialize an iterator entity.
         * The iterator is in mode ALL, which means that it will consider all entries regardless
         * of them being default entries or not.
         */
        if (table == 0)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_tables[table], &entry_handle_id));
        }
        else
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_tables[table], entry_handle_id));
        }
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

        /** Receive first entry in table. */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {
            sal_memset(&info, 0, sizeof(bcm_ppp_terminator_t));
            /** Get info about PPPoE ingress tunnels */
            if (dbal_tables[table] == DBAL_TABLE_PPPOE_O_ETH_TUNNEL_FULL_SA)
            {
                info.type = bcmPPPTypePPPoE;
                /** Get Lookup Keys */
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_get
                                (unit, entry_handle_id, DBAL_FIELD_L2_MAC, info.src_mac));

                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_SESSION_ID, &info.session_id));

                /** Get Lookup Result */
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, &local_inlif));

                /** Get entry from PPP tunnel inLIF table */
                SHR_IF_ERR_EXIT(dnx_ppp_terminator_in_lif_table_get
                                (unit, &info, local_inlif, &global_inlif, NULL, &propagation_prof));

                /** Set tunnel_id */
                BCM_GPORT_TUNNEL_ID_SET(info.ppp_terminator_id, global_inlif);

                /** Update QOS flags according to returned propagation_prof */
                SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_attr_get
                                (unit, propagation_prof, &phb_propagation_type, &remark_propagation_type,
                                 &ecn_propagation_type, &ttl_propagation_type));
                if (phb_propagation_type != DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE)
                {
                    SHR_IF_ERR_EXIT(dnx_qos_ingress_propag_type_to_qos_model
                                    (unit, phb_propagation_type, &(info.ingress_qos_model.ingress_phb)));
                }

                if (remark_propagation_type != DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE)
                {
                    SHR_IF_ERR_EXIT(dnx_qos_ingress_propag_type_to_qos_model
                                    (unit, remark_propagation_type, &(info.ingress_qos_model.ingress_remark)));
                }

                if (ttl_propagation_type != DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE)
                {
                    SHR_IF_ERR_EXIT(dnx_qos_ingress_propag_type_to_qos_model
                                    (unit, ttl_propagation_type, &(info.ingress_qos_model.ingress_ttl)));
                }
            }
            /** Get info about L2TPv2 ingress tunnels */
            else if (dbal_tables[table] == DBAL_TABLE_L2TPV2_DATA_MESSAGE_TT)
            {
                info.type = bcmPPPTypeL2TPv2;
                /** Get Lookup Keys */
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN, &info.network_domain));
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_L2TPV2_TUNNEL_ID, &info.l2tpv2_tunnel_id));
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_SESSION_ID, &info.session_id));
                /** Get Lookup Result */
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, &local_inlif));

                /** Get entry from PPP tunnel inLIF table */
                SHR_IF_ERR_EXIT(dnx_ppp_terminator_in_lif_table_get
                                (unit, &info, local_inlif, &global_inlif, NULL, &propagation_prof));

                /** Set tunnel_id */
                BCM_GPORT_TUNNEL_ID_SET(info.ppp_terminator_id, global_inlif);

                /** Update QOS flags according to returned propagation_prof */
                SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_attr_get
                                (unit, propagation_prof, &phb_propagation_type, &remark_propagation_type,
                                 &ecn_propagation_type, &ttl_propagation_type));
                if (phb_propagation_type != DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE)
                {
                    SHR_IF_ERR_EXIT(dnx_qos_ingress_propag_type_to_qos_model
                                    (unit, phb_propagation_type, &(info.ingress_qos_model.ingress_phb)));
                }

                if (remark_propagation_type != DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE)
                {
                    SHR_IF_ERR_EXIT(dnx_qos_ingress_propag_type_to_qos_model
                                    (unit, remark_propagation_type, &(info.ingress_qos_model.ingress_remark)));
                }

                if (ttl_propagation_type != DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE)
                {
                    SHR_IF_ERR_EXIT(dnx_qos_ingress_propag_type_to_qos_model
                                    (unit, ttl_propagation_type, &(info.ingress_qos_model.ingress_ttl)));
                }
            }
            if (cb != NULL)
            {
                /** Invoke callback function */
                SHR_IF_ERR_EXIT((*cb) (unit, &info, user_data));
            }
            /** Receive next entry in table.*/
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Add a PPP Term spoofing check per given PPP Tunnel Gport and Vlan Gport.
 * \param [in] unit - Relevant unit.
 * \param [in] info - A pointer to the struct that holds information for spoofing check
 *   [in] info->ppp_terminator_id - PPP Tunnel Gport.
 *   [in] info->vlan_port_id - Vlan Gport.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
int
bcm_dnx_ppp_term_spoofing_check_add(
    int unit,
    bcm_ppp_term_spoofing_check_t * info)
{
    int global_lif = 0;
    int global_lif_1 = 0;
    int globless_msb = 0;
    int globless_with_nothing = 0;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_ppp_term_spoofing_check_add_verify(unit, info));

    /** Get GLIFless with nothing from vrCfg.PPPoE_SESSION_SPOOFING_CHECK_MSB(19) */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_PPPOE_SESSION_SPOOFING_CHECK_MSB, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DEFAULT_LIF_MSB, INST_SINGLE,
                               (uint32 *) &globless_msb);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    globless_with_nothing = ((globless_msb < 2) | 0x2);

    /** In_LIFs[0] PPPoE LIF */
    global_lif_1 = BCM_GPORT_TUNNEL_ID_GET(info->ppp_terminator_id);
    /** In_LIFs[1] AC LIF */
    if (BCM_GPORT_IS_VLAN_PORT(info->vlan_port_id))
    {
        global_lif = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(BCM_GPORT_VLAN_PORT_ID_GET(info->vlan_port_id));

    }
    else if (BCM_GPORT_IS_TUNNEL(info->vlan_port_id))
    {
        global_lif = BCM_GPORT_TUNNEL_ID_GET(info->vlan_port_id);
    }

    /** Get table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LIF_SPOOFING_CHECK, &entry_handle_id));
    /** Set key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF_1, global_lif_1);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, global_lif);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    /** Set GLIFless with nothing as result */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, globless_with_nothing);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Delete a PPP Term spoofing check per given PPP Tunnel Gport and Vlan Gport.
 * \param [in] unit - Relevant unit.
 * \param [in] info - A pointer to the struct that holds information for spoofing check
 *   [in] info->ppp_terminator_id - PPP Tunnel Gport.
 *   [in] info->vlan_port_id - Vlan Gport.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
int
bcm_dnx_ppp_term_spoofing_check_delete(
    int unit,
    bcm_ppp_term_spoofing_check_t * info)
{
    int global_lif = 0;
    int global_lif_1 = 0;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_ppp_term_spoofing_check_delete_verify(unit, info));

    /** In_LIFs[0] PPPoE LIF */
    global_lif_1 = BCM_GPORT_TUNNEL_ID_GET(info->ppp_terminator_id);

    /** In_LIFs[1] AC LIF*/
    if (BCM_GPORT_IS_VLAN_PORT(info->vlan_port_id))
    {
        global_lif = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(BCM_GPORT_VLAN_PORT_ID_GET(info->vlan_port_id));

    }
    else if (BCM_GPORT_IS_TUNNEL(info->vlan_port_id))
    {
        global_lif = BCM_GPORT_TUNNEL_ID_GET(info->vlan_port_id);
    }

    /** Get table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LIF_SPOOFING_CHECK, &entry_handle_id));
    /** Set key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF_1, global_lif_1);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, global_lif);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    /** Delete the entry */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Traverse PPP Term Spoofing Check Info.
 * \param [in] unit - Relevant unit.
 * \param [in] additional_info - the additional info for traversing.
 * \param [in] cb - the name of the callback function,
 *          it receives the value of the user_data variable and
 *          the key and result values for each found entry.
 * \param [in] user_data - user data that will be sent to the callback function
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
int
bcm_dnx_ppp_term_spoofing_check_traverse(
    int unit,
    bcm_ppp_term_spoofing_check_additional_info_t * additional_info,
    bcm_ppp_term_spoofing_check_traverse_cb cb,
    void *user_data)
{
    uint32 entry_handle_id;
    uint32 dbal_table = DBAL_TABLE_LIF_SPOOFING_CHECK;
    uint32 global_inlif, global_inlif_1;
    int is_end;
    bcm_ppp_term_spoofing_check_t info;
    bcm_vlan_port_t vlan_port;
    int rv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_ppp_term_spoofing_check_traverse_verify(unit, additional_info, cb, user_data));

    /** Apply additional limit */

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    /** Receive first entry in table. */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        /** In_LIFs[0] PPPoE LIF */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF_1, &global_inlif_1));
        BCM_GPORT_TUNNEL_ID_SET(info.ppp_terminator_id, global_inlif_1);
        /** In_LIFs[1] AC LIF */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, &global_inlif));
        /** Encode subtype */
        sal_memset(&vlan_port, 0, sizeof(bcm_vlan_port_t));
        BCM_GPORT_SUB_TYPE_LIF_SET(vlan_port.vlan_port_id, 0, global_inlif);
        BCM_GPORT_VLAN_PORT_ID_SET(vlan_port.vlan_port_id, vlan_port.vlan_port_id);
        rv = bcm_vlan_port_find(unit, &vlan_port);
        /** PPPoE anti-spoofing */
        if (rv == BCM_E_NONE)
        {
            info.vlan_port_id = vlan_port.vlan_port_id;
        }
        else                 /** L2TP anti-spoofing */
        {
            BCM_GPORT_TUNNEL_ID_SET(info.vlan_port_id, global_inlif);
        }

        if (cb != NULL)
        {
            /** Invoke callback function */
            SHR_IF_ERR_EXIT((*cb) (unit, &info, user_data));
        }
        /** Receive next entry in table.*/
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * End of APIs
 * }
 */
