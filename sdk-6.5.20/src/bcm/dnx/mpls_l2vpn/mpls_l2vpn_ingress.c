/** \file mpls_l2vpn_ingress.c General MPLS ingress L2VPN functionality for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MPLS

/*
 * Include files.
 * {
 */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_failover.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mpls.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/mpls/mpls.h>
#include <bcm_int/dnx/qos/qos.h>
#include <bcm_int/dnx/mpls/mpls_tunnel_term.h>
#include <bcm_int/dnx/mpls/mpls_tunnel_encap.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>
#include <bcm_int/dnx/algo/mpls/algo_mpls.h>
#include <bcm_int/dnx/failover/failover.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_mpls_access.h>

#include "mpls_l2vpn_ingress.h"
/*
 * }
 */

/*
 * Defines and macros
 * {
 */
/*
 * }
 */

/*
 * Local data structures
 * {
 */

/**
 * \brief
 *  Data structure for pwe in-lif information
 */
typedef struct
{
    /**
     * \brief InLIF table
     */
    dbal_tables_e dbal_table;
    /**
     * \brief P2P or MP
     */
    int is_p2p;
    /**
     * \brief Local in lif pointer
     */
    int local_in_lif;
    /**
     * \brief global in lif ID
     */
    int global_in_lif;
    /**
     * \brief allocated in lif profile
     */
    int in_lif_profile;
    /**
     * \brief allocated termination profile
     */
    int term_profile;
    /**
     * \brief propagation profile
     */
    int propagation_profile;
    /**
     * \brief vpn or 0 for P2P
     */
    bcm_vpn_t vsi;
    /**
     * \brief learning info buffer
     */
    uint32 learn_info[2];
    /**
     * \brief learning context type
     */
    dbal_enum_value_field_learn_payload_context_e learn_context;
    /**
     * \brief learning enabled. For future support of disable learning / EVPN.
     */
    uint32 learn_enable;
    /*
     * \brief Protection pointer
     */
    int protection_pointer;
    /*
     * \brief Protection path
     */
    int protection_path;
    /**
     * \brief Statistic info required
     */
    uint8 is_stat;
    /**
     * \brief protect info required
     */
    uint8 is_protect;

} dnx_mpls_l2vpn_in_lif_info_t;

/*
 * }
 */

/**
 * \brief
 *   Fill MPLS match information SW state. InLIF will return
 *   MPLS label that is the key to that lif.
 * \param [in] unit - unit number.
 * \param [in] core_id - core id in case only one core is set. _SHR_CORE_ALL for both.
 * \param [in] local_inlif - local inlif is the key to the db.
 * \param [in] match - match information and criteria to be stored.
 * \return
 *   shr_error_e - Non-zero in case of an error.
 */
shr_error_e
dnx_mpls_l2vpn_termination_match_information_set(
    int unit,
    int core_id,
    int local_inlif,
    dnx_mpls_term_match_t * match)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Fill match information.
     * In case of PWE it may be: mpls label, 2 labels or intf+label.
     * In case of EVPN it may be: mpls label or label+BOS
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_LIF, local_inlif);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    if (match->match_criteria == TERM_MATCH_CRITERIA_LABEL)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_LABEL);
    }
    else if (match->match_criteria == TERM_MATCH_CRITERIA_TWO_LABELS)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_LABEL_CTX_LABEL);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CTX_MPLS_LABEL, INST_SINGLE,
                                     match->context_label);
    }
    else if (match->match_criteria == TERM_MATCH_CRITERIA_INTF_LABEL)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_LABEL_L3_INTF);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_L3_INTF, INST_SINGLE, match->context_intf);
    }
    else if (match->match_criteria == TERM_MATCH_CRITERIA_LABEL_BOS)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_LABEL_BOS);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BOS, INST_SINGLE, match->bos_val);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Something went wrong: Unhandled match criteria");
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, INST_SINGLE, match->label);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SERVICE_TAGGED, INST_SINGLE, match->service_tagged);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Read MPLS match information SW state. InLIF will return
 *   MPLS label that is the key to that lif.
 * \param [in] unit - unit number.
 * \param [in] core_id - core id in case only one core is set. _SHR_CORE_ALL for both.
 * \param [in] local_inlif - local inlif is the key to the db.
 * \param [out] match - matching information and criteria to be retrieved.
 * \return
 *   shr_error_e - Non-zero in case of an error.
 */
shr_error_e
dnx_mpls_l2vpn_termination_match_information_get(
    int unit,
    int core_id,
    int local_inlif,
    dnx_mpls_term_match_t * match)
{
    uint32 entry_handle_id;
    uint32 result_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Fill match information, in case of MPLS tunnel/PWE it will always be mpls label
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_LIF, local_inlif);
    /*
     * Using DBAL_CORE_ALL in get operation is not legal for DPC table. core_id = 0 is used instead, under the
     * assumption that values are equal in both cores
     */
    if (core_id == DBAL_CORE_ALL)
    {
        core_id = DBAL_CORE_DEFAULT;
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &result_type));

    if (result_type == DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_LABEL)
    {
        match->match_criteria = TERM_MATCH_CRITERIA_LABEL;
    }
    else if (result_type == DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_LABEL_BOS)
    {
        uint32 dbal_bos_val;
        match->match_criteria = TERM_MATCH_CRITERIA_LABEL_BOS;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_BOS, INST_SINGLE, &dbal_bos_val));
        match->bos_val = dbal_bos_val;
    }
    else if (result_type == DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_LABEL_CTX_LABEL)
    {
        uint32 context_label_val;
        match->match_criteria = TERM_MATCH_CRITERIA_TWO_LABELS;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_CTX_MPLS_LABEL, INST_SINGLE, &context_label_val));
        match->context_label = context_label_val;
    }
    else if (result_type == DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_LABEL_L3_INTF)
    {
        uint32 context_intf_val;
        match->match_criteria = TERM_MATCH_CRITERIA_INTF_LABEL;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_L3_INTF, INST_SINGLE, &context_intf_val));
        match->context_intf = context_intf_val;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Result type (%d) not supported for VPLS, wrong lif (%d) provided\n", result_type,
                     local_inlif);
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, INST_SINGLE, &match->label));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SERVICE_TAGGED, INST_SINGLE, &match->service_tagged));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Free MPLS match information SW state.
 * \param [in] unit - unit number.
 * \param [in] core_id - core id in case only one core is set. _SHR_CORE_ALL for both.
 * \param [in] local_inlif - local inlif is the key to the db.
 * \return
 *   shr_error_e - Non-zero in case of an error.
 * \remark
 */
shr_error_e
dnx_mpls_l2vpn_termination_match_information_delete(
    int unit,
    int core_id,
    int local_inlif)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Fill match information, in case of MPLS tunnel/PWE it will always be mpls label
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_LIF, local_inlif);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Fill the dnx_mpls_termination_profile_t structure
 * According to the information in bcm_mpls_port_t
 *
 * \param [in] unit - Relevant unit.
 * \param [in] l2vpn - A pointer to a struct containing relevant
 *        information for the LIF TABLE entry.
 * \param [out] term_profile_info  -  A pointer to the struct representing the tunnel term profile entry.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_mpls_l2vpn_to_termination_profile(
    int unit,
    dnx_mpls_l2vpn_info_t * l2vpn,
    dnx_mpls_termination_profile_t * term_profile_info)
{
    bcm_mpls_port_t *mpls_port;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(l2vpn, _SHR_E_PARAM, "l2vpn");

    mpls_port = l2vpn->mpls_port;

    sal_memset(term_profile_info, 0, sizeof(dnx_mpls_termination_profile_t));
    term_profile_info->expect_bos = FALSE;
    term_profile_info->check_bos = FALSE;
    term_profile_info->ttl_exp_label_index = TTL_EXP_CURRENT_MPLS_LABEL;
    term_profile_info->has_cw = _SHR_IS_FLAG_SET(mpls_port->flags, BCM_MPLS_PORT_CONTROL_WORD) ? TRUE : FALSE;
    if (_SHR_IS_FLAG_SET(mpls_port->flags, BCM_MPLS_PORT_ENTROPY_ENABLE) &&
        (_SHR_IS_FLAG_SET(l2vpn->flags, DNX_MPLS_L2VPN_FLAG_INGRESS_IML) ||
         _SHR_IS_FLAG_SET(l2vpn->flags, DNX_MPLS_L2VPN_FLAG_INGRESS_IML_WITH_ESI) ||
         _SHR_IS_FLAG_SET(l2vpn->flags, DNX_MPLS_L2VPN_FLAG_INGRESS_IML_WITHOUT_ESI)))
    {
        term_profile_info->labels_to_terminate = ONE_MPLS_LABEL_TO_TERMINATE;
    }
    else if (_SHR_IS_FLAG_SET(l2vpn->flags, DNX_MPLS_L2VPN_FLAG_INGRESS_IML))
    {
        term_profile_info->labels_to_terminate = ONE_OR_TWO_MPLS_LABELS_TO_TERMINATE;
    }
    else if (_SHR_IS_FLAG_SET(l2vpn->flags, DNX_MPLS_L2VPN_FLAG_INGRESS_IML_WITH_ESI))
    {
        term_profile_info->labels_to_terminate = TWO_MPLS_LABELS_TO_TERMINATE;
    }
    else
    {
        if (mpls_port->vccv_type == bcmMplsPortControlChannelTtl)
        {
            term_profile_info->reject_ttl_0 = 1;
            term_profile_info->reject_ttl_1 = 1;
        }
        if (_SHR_IS_FLAG_SET(mpls_port->flags, BCM_MPLS_PORT_ENTROPY_ENABLE))
        {
            term_profile_info->labels_to_terminate = TWO_MPLS_LABELS_TO_TERMINATE;
        }
        else
        {
            term_profile_info->labels_to_terminate = ONE_MPLS_LABEL_TO_TERMINATE;
        }
        if (mpls_port->criteria == BCM_MPLS_PORT_MATCH_LABEL_CONTEXT_LABEL)
        {
            /*
             * Context label needs to be terminated as well.
             */
            term_profile_info->labels_to_terminate++;
        }
        if (term_profile_info->labels_to_terminate > THREE_MPLS_LABEL_TO_TERMINATE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Trying to add too many MPLS label terminations to a single object\n");
        }
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Fill the dnx_mpls_termination_profile_t structure
 * According to the information in bcm_mpls_port_t
 *
 * \param [in] unit - Relevant unit.
 * \param [in] term_profile_info - A pointer to the struct
 *        representing the mpls port term profile entry.
 * \param [out] l2vpn - A pointer to a struct containing relevant
 *        information for the LIF TABLE entry.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * Assumes l2vpn->mpls_port->criteria was already resolved.
 * \see
 *   * None
 */
static shr_error_e
dnx_mpls_termination_profile_to_mpls_l2vpn_info(
    int unit,
    dnx_mpls_termination_profile_t * term_profile_info,
    dnx_mpls_l2vpn_info_t * l2vpn)
{
    int labels_to_terminate_val;
    uint8 is_iml;

    SHR_FUNC_INIT_VARS(unit);

    if (term_profile_info->reject_ttl_0 && term_profile_info->reject_ttl_1)
    {
        l2vpn->mpls_port->vccv_type = bcmMplsPortControlChannelTtl;
    }
    if (term_profile_info->has_cw)
    {
        l2vpn->mpls_port->flags |= BCM_MPLS_PORT_CONTROL_WORD;
    }
    /*
     * The number of labels to terminate, may be affected from:
     * 1. Termination of the main matched label of this item
     * 2. Termination of additional label (Entropy for MPLS/VPLS, ESI for EVPN)
     * 3. Termination of a context label preceding the main label
     * The following logic deduces the API signals for the 2nd factor.
     */
    labels_to_terminate_val = term_profile_info->labels_to_terminate;
    if (l2vpn->mpls_port->criteria == BCM_MPLS_PORT_MATCH_LABEL_CONTEXT_LABEL)
    {
        /*
         * Context label added a label to the count. remove it.
         */
        labels_to_terminate_val--;
    }
    if (labels_to_terminate_val == TWO_MPLS_LABELS_TO_TERMINATE)
    {
        /*
         * After possibly removing a context label, we are still left with 2 terminated labels.
         * Set the relevant flag, depending on the specific application type.
         */
        is_iml = _SHR_IS_FLAG_SET(l2vpn->flags, DNX_MPLS_L2VPN_FLAG_INGRESS_IML_WITH_ESI) ||
                 _SHR_IS_FLAG_SET(l2vpn->flags, DNX_MPLS_L2VPN_FLAG_INGRESS_IML_WITHOUT_ESI);
        if ((l2vpn->type == MPLS_L2VPN_TYPE_MPLS_PORT) ||
            (l2vpn->type == MPLS_L2VPN_TYPE_EVPN && !is_iml))
        {
            /** In case of IML, we need to determine the flag per CS-PROFILE*/
            l2vpn->mpls_port->flags |= BCM_MPLS_PORT_ENTROPY_ENABLE;
        }
    }
    else if (labels_to_terminate_val == ONE_OR_TWO_MPLS_LABELS_TO_TERMINATE)
    {
        /** For other IML flags (W/O ESI), they are set according to the macth criteria*/
        l2vpn->flags |= DNX_MPLS_L2VPN_FLAG_INGRESS_IML;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Translates the fwd information in mpls port structure to internal fwd information structure.
* \par DIRECT INPUT:
*   \param [in] unit       -  Relevant unit.
*   \param [in] mpls_port  -  Holds all the relevant information for the different schemes that might be defined on a VPLS LIF.
*   \param [out] forward_info - Forward information buffer.
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_mpls_port_to_forward_information(
    int unit,
    bcm_mpls_port_t * mpls_port,
    dnx_algo_gpm_forward_info_t * forward_info)
{
    bcm_gport_t gport_forward_port;

    /*
     * Jericho mode beginning
     * {
     */
    int push_cmd = 0;
    uint32 eei_val;
    int system_headers_mode;
    /*
     * }
     * Jericho mode end
     */

    SHR_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    sal_memset(forward_info, 0, sizeof(dnx_algo_gpm_forward_info_t));

    /*
     * Forwarding is done according to destination and Outlif (optional) (PWE + FEC or PWE + port)
     */
    if (mpls_port->failover_port_id)
    {
        /*
         * In case failover port id (PWE FEC) is valid, only it is being learned
         */
        forward_info->fwd_info_result_type = DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_ONLY;
        _SHR_GPORT_FORWARD_PORT_SET(gport_forward_port, mpls_port->failover_port_id);
        mpls_port->failover_port_id = gport_forward_port;
    }
    else if (mpls_port->failover_mc_group)
    {
        /*
         * In case  failover mc group is valid, only it is being learned
         */
        forward_info->fwd_info_result_type = DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_ONLY;
        _SHR_GPORT_MCAST_SET(gport_forward_port, mpls_port->failover_mc_group);
    }
    /*
     * Jericho mode beginning
     * {
     */
    else if ((system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE) &&
             (_SHR_IS_FLAG_SET(mpls_port->flags2, BCM_MPLS_PORT2_LEARN_ENCAP_EEI)))
    {
        forward_info->fwd_info_result_type = DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_EEI;
        if (_SHR_L3_ITF_TYPE_IS_FEC(mpls_port->egress_tunnel_if))
        {
            _SHR_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(gport_forward_port, mpls_port->egress_tunnel_if);
        }
        else
        {
            gport_forward_port = mpls_port->port;
        }
        eei_val = BCM_L3_ITF_VAL_GET(mpls_port->encap_id);
        SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                        (unit, DBAL_FIELD_EEI, DBAL_FIELD_EEI_ENCAPSULATION_POINTER, &eei_val, &forward_info->eei));

    }
    else if ((system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE) &&
             (DNX_MPLS_LABEL_IN_RANGE(unit, mpls_port->egress_label.label)))
    {
        forward_info->fwd_info_result_type = DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_EEI;
        if (_SHR_L3_ITF_TYPE_IS_FEC(mpls_port->egress_tunnel_if))
        {
            _SHR_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(gport_forward_port, mpls_port->egress_tunnel_if);
        }
        else
        {
            gport_forward_port = mpls_port->port;
        }

        SHR_IF_ERR_EXIT(dnx_mpls_tunnel_create_push_entry_from_eei(unit, &(mpls_port->egress_label),
                                                                   _SHR_IS_FLAG_SET(mpls_port->flags,
                                                                                    BCM_MPLS_PORT_CONTROL_WORD), FALSE,
                                                                   &push_cmd));
         /* coverity[callee_ptr_arith:FALSE]  */
        SHR_IF_ERR_EXIT(dnx_mpls_eei_push_information_fill(unit, mpls_port->egress_label.label, push_cmd, &eei_val));
        SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                        (unit, DBAL_FIELD_EEI, DBAL_FIELD_EEI_MPLS_PUSH_COMMAND, &eei_val, &forward_info->eei));
    }
    /*
     * }
     * Jericho mode end
     */
    else
    {
        /*
         * In case neither failover_port_id and failover_mc_group are valid:
         * a. PWE outlif is part of learning information (encap_id)
         * b. destination is taken from egress_tunnel_if in case MPLS Tunnel is FEC
         * c. destination is taken from port (in case egress_tunnel_if is 0)
         */
        forward_info->fwd_info_result_type = DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF;
        forward_info->outlif = mpls_port->encap_id;
        if (_SHR_L3_ITF_TYPE_IS_FEC(mpls_port->egress_tunnel_if))
        {
            _SHR_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(gport_forward_port, mpls_port->egress_tunnel_if);
        }
        else
        {
            gport_forward_port = mpls_port->port;
        }
    }

    SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                    (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, gport_forward_port, &forward_info->destination));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Get gport from the accompanying forwarding information.
* \par DIRECT INPUT:
*   \param [in] unit       -  Relevant unit.
*   \param [in] mpls_port  -  Holds all the relevant information for the different schemes that might be defined on a VPLS LIF.
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_mpls_port_gport_to_forward_information_get(
    int unit,
    bcm_mpls_port_t * mpls_port)
{
    dnx_algo_gpm_forward_info_t forward_info;
    bcm_gport_t destination;

    /*
     * Jericho mode beginning
     * {
     */
    int push_cmd;
    dnx_mpls_encap_t encap_info;
    dbal_fields_e dbal_eei_type;
    uint32 eei_val;
    /*
     * }
     * Jericho mode end
     */

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_gpm_gport_l2_forward_info_get(unit, mpls_port->mpls_port_id, &forward_info));

    if ((forward_info.fwd_info_result_type != DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_ONLY) &&
        (forward_info.fwd_info_result_type != DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF) &&
        (forward_info.fwd_info_result_type != DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_EEI))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "result_type (%d) is not supported for get api \r\n",
                     forward_info.fwd_info_result_type);
    }

    /*
     * returning learning information depends on whether the destination is FEC or MC or physical port
     */
    SHR_IF_ERR_EXIT(algo_gpm_gport_from_encoded_destination_field
                    (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, forward_info.destination, &destination));

    /*
     *  Translating learning information to mpls port fields:
     *  a. In case of Protection (FEC as destination) than failover_port_id is set.
     *  b. In case of 1+1 protection (MC-ID as destination) then failover_mc_group is set
     *  c. In case of FEC+OutLIF (Destination as FEC + OutLIF) then fill egress_tunnel_if and encap_id
     * d. In case of Port+OutLIF (Destination as non FEC + OutLIF) then fill port and encap_id
     */
    if (forward_info.fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_ONLY)
    {
        if (_SHR_GPORT_IS_FORWARD_PORT(destination))
        {
            mpls_port->failover_port_id = destination;
        }
        else if (_SHR_GPORT_IS_MCAST(destination))
        {
            mpls_port->failover_mc_group = _SHR_GPORT_MCAST_GET(destination);
        }
    }
    /*
     * Jericho mode beginning
     * {
     */
    else if (forward_info.fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_EEI)
    {
        if (_SHR_GPORT_IS_FORWARD_PORT(destination))
        {
            _SHR_GPORT_FORWARD_GROUP_TO_L3_ITF_FEC(mpls_port->egress_tunnel_if, destination);
        }
        else
        {
            mpls_port->port = destination;
        }

        SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                        (unit, DBAL_FIELD_EEI, forward_info.eei, &dbal_eei_type, &eei_val));
        if (dbal_eei_type == DBAL_FIELD_EEI_ENCAPSULATION_POINTER)
        {
            uint8 tmp = 0;
            int eei = 0;
            SHR_IF_ERR_EXIT(dnx_mpls_eei_enc_pointer_information_retrieve(unit, forward_info.eei, &eei, &tmp));
            BCM_ENCAP_ID_SET(mpls_port->encap_id, BCM_L3_ITF_VAL_GET(eei));

            mpls_port->flags2 |= BCM_MPLS_PORT2_LEARN_ENCAP_EEI;
        }
        else
        {
            dnx_mpls_encap_t_init(unit, &encap_info);
            encap_info.label_array = &(mpls_port->egress_label);

            SHR_IF_ERR_EXIT(dnx_mpls_eei_push_information_retrieve
                            (unit, forward_info.eei, &mpls_port->egress_label.label, &push_cmd));
            SHR_IF_ERR_EXIT(dnx_mpls_encap_get_from_push_cmd(unit, push_cmd, &encap_info));
            if (encap_info.has_cw)
            {
                mpls_port->flags |= BCM_MPLS_PORT_CONTROL_WORD;
            }
        }
    }
    /*
     * }
     * Jericho mode end
     */
    else if (forward_info.fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF)
    {
        mpls_port->encap_id = forward_info.outlif;
        if (_SHR_GPORT_IS_FORWARD_PORT(destination))
        {
            _SHR_GPORT_FORWARD_GROUP_TO_L3_ITF_FEC(mpls_port->egress_tunnel_if, destination);
        }
        else
        {
            mpls_port->port = destination;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* delete mpls port forwarding information.
* \par DIRECT INPUT:
*   \param [in] unit       -  Relevant unit.
*   \param [in] mpls_port_id  -  mpls port ID.
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_mpls_port_gport_to_forward_information_delete(
    int unit,
    bcm_gport_t mpls_port_id)
{
    dnx_algo_gpm_forward_info_t forward_info;
    /*
     * Jericho mode beginning
     * {
     */
    int push_cmd;
    dbal_fields_e dbal_eei_type;
    uint32 eei_val, label_val;
    /*
     * }
     * Jericho mode end
     */

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_gpm_gport_l2_forward_info_get(unit, mpls_port_id, &forward_info));
    /*
     * Jericho mode beginning
     * {
     */
    if (forward_info.fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_EEI)
    {
        SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                        (unit, DBAL_FIELD_EEI, forward_info.eei, &dbal_eei_type, &eei_val));

        if (dbal_eei_type == DBAL_FIELD_EEI_MPLS_PUSH_COMMAND)
        {
            SHR_IF_ERR_EXIT(dnx_mpls_eei_push_information_retrieve(unit, forward_info.eei, &label_val, &push_cmd));

            SHR_IF_ERR_EXIT(dnx_mpls_tunnel_delete_push_entry_from_eei(unit, push_cmd));
        }
    }
    /*
     * }
     * Jericho mode end
     */

    SHR_IF_ERR_EXIT(algo_gpm_gport_l2_forward_info_delete(unit, mpls_port_id));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function gets the P2P in-LIF info.
 * \par DIRECT INPUT:
 *   \param [in] unit       - Relevant unit.
 *   \param [in] info       - pwe in-lif information.
 *   \param [in] result_type -  in-lif result type.
 *   \param [out] out_lif   - out_lif at P2P cross connect.
 *   \param [out] eei       - EEI at P2P cross connect.
 *   \param [out] destination       - destination at P2P cross connect.
 * \par DIRECT OUTPUT:
 *   shr_error_e - Non-zero in case of an error.
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_mpls_l2vpn_in_lif_p2p_info_get(
    int unit,
    dnx_mpls_l2vpn_in_lif_info_t * info,
    uint32 result_type,
    uint32 *out_lif,
    uint32 *eei,
    uint32 *destination)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, info->dbal_table, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, info->local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /*
     * For IN_LIF_FORMAT_PWE and IN_LIF_FORMAT_EVPN P2P result, only IN_LIF_PWE2EEI_NO_LEARNING has EEI field;
     * and other results always have out_lif field.
     */
    if (info->dbal_table == DBAL_TABLE_IN_LIF_FORMAT_PWE)
    {
        if (result_type == DBAL_RESULT_TYPE_IN_LIF_FORMAT_PWE_IN_LIF_PWE2EEI_NO_LEARNING)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_EEI, INST_SINGLE, eei));
        }
        else if (result_type != DBAL_RESULT_TYPE_IN_LIF_FORMAT_PWE_IN_LIF_PWE_MP)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, out_lif));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, destination));
        }
    }
    else if (info->dbal_table == DBAL_TABLE_IN_LIF_FORMAT_EVPN)
    {
        if (result_type != DBAL_RESULT_TYPE_IN_LIF_FORMAT_EVPN_IN_LIF_EVPN_EVI_MP)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, out_lif));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, destination));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Set termination profile and global lif using local in_lif from lif format table.
*
*   \param [in] unit -  Relevant unit.
*   \param [in] info - All required in lif information
*   \param [in] entry_handle_id - Dbal handle id for lif table manager
*   \param [in] out_lif - out_lif at P2P cross connect.
*   \param [in] eei - eei at P2P cross connect.
*   \param [in] destination - destination at P2P cross connect.
* \return
*   shr_error_e
*
* \remark
*   None
* \see
*   dnx_mpls_port_add_ingress_only
*/
static shr_error_e
dnx_mpls_l2vpn_in_lif_to_lif_table_manager(
    int unit,
    dnx_mpls_l2vpn_in_lif_info_t * info,
    uint32 entry_handle_id,
    uint32 out_lif,
    uint32 eei,
    uint32 destination)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);

    if (info->is_p2p == FALSE)
    {
        /*
         * MP
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_ENABLE, INST_SINGLE, info->learn_enable);
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_INFO_ASYM_LIF_BASIC_EXT, INST_SINGLE,
                                         info->learn_info);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT, INST_SINGLE,
                                     info->learn_context);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SERVICE_TYPE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2MP);

    }
    else
    {
        /*
         * P2P
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SERVICE_TYPE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P);

        if (out_lif)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, out_lif);
        }

        if (eei)
        {
            /** Get eei */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EEI, INST_SINGLE, eei);
        }

        if (destination)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, destination);
        }
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, INST_SINGLE, info->global_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TERMINATION_TYPE, INST_SINGLE, info->term_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, info->vsi);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, INST_SINGLE,
                                 info->propagation_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, info->in_lif_profile);
    if (info->is_protect)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTECTION_POINTER, INST_SINGLE,
                                     info->protection_pointer);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTECTION_PATH, INST_SINGLE,
                                     info->protection_path);
    }

    if (info->is_stat)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE, 0);
    }

    SHR_EXIT();

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get termination profile and global lif using local in_lif from lif format table.
*
*   \param [in] unit -Relevant unit.
*   \param [in,out] info - key + retrieved information
* \return
*   shr_error_e
*
* \remark
*   None
* \see
*   None
*/
shr_error_e
dnx_mpls_l2vpn_in_lif_get(
    int unit,
    dnx_mpls_l2vpn_in_lif_info_t * info)
{
    uint32 entry_handle_id;
    lif_table_mngr_inlif_info_t lif_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, info->dbal_table, &entry_handle_id));
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_inlif_info
                    (unit, _SHR_CORE_ALL, info->local_in_lif, entry_handle_id, &lif_info));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_TERMINATION_TYPE, INST_SINGLE, (uint32 *) &info->term_profile));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, INST_SINGLE,
                     (uint32 *) &info->propagation_profile));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, (uint32 *) &info->in_lif_profile));

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                    (unit, entry_handle_id, DBAL_FIELD_PROTECTION_POINTER, &info->is_protect));
    if (info->is_protect)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_PROTECTION_POINTER, INST_SINGLE,
                         (uint32 *) &info->protection_pointer));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_PROTECTION_PATH, INST_SINGLE,
                         (uint32 *) &info->protection_path));
    }

    /** Statistics indication */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                    (unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, &info->is_stat));

    {
        uint32 service_type;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_SERVICE_TYPE, INST_SINGLE, &service_type));
        info->is_p2p = (service_type == DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P);
    }

    if (info->is_p2p == FALSE)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_LEARN_ENABLE, INST_SINGLE, &info->learn_enable));
    }

    
    if (!info->is_p2p)
    {
        uint32 vsi_val = 0;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, &vsi_val));
        info->vsi = vsi_val;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Update mpls_port struct with the relevant information
*  from in-lif-profile.
*
*   \param [in] unit -  Relevant unit.
*   \param [in] in_lif_profile -inlif profile.
*   \param [in,out] l2vpn - bcm struct with user info.
* \return
*   shr_error_e
*
* \remark
*   None
* \see
*   dnx_mpls_port_get_ingress_only
*/
static shr_error_e
dnx_mpls_inlif_profile_to_mpls_port_info(
    int unit,
    int in_lif_profile,
    dnx_mpls_l2vpn_info_t * l2vpn)
{
    in_lif_profile_info_t in_lif_profile_info;
    bcm_mpls_port_t * mpls_port = NULL;
    uint8 is_iml;

    SHR_FUNC_INIT_VARS(unit);

    mpls_port = l2vpn->mpls_port;

    /** Get in_lif_porfile data: */
    SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, in_lif_profile, &in_lif_profile_info, LIF));

    /** Update network_group_id: */
    mpls_port->network_group_id = in_lif_profile_info.egress_fields.in_lif_orientation;

    /** These IML flags were set already according to match-criteria or termination-profile */
    is_iml = _SHR_IS_FLAG_SET(l2vpn->flags, DNX_MPLS_L2VPN_FLAG_INGRESS_IML_WITHOUT_ESI) ||
             _SHR_IS_FLAG_SET(l2vpn->flags, DNX_MPLS_L2VPN_FLAG_INGRESS_IML_WITH_ESI) ||
             _SHR_IS_FLAG_SET(l2vpn->flags, DNX_MPLS_L2VPN_FLAG_INGRESS_IML);

    if (l2vpn->type == MPLS_L2VPN_TYPE_EVPN && is_iml)
    {
        if (_SHR_IS_FLAG_SET
            (in_lif_profile_info.ingress_fields.cs_in_lif_profile_flags, DNX_IN_LIF_PROFILE_CS_PROFILE_EVPN_IML_AH_FL_ONLY))
        {
            mpls_port->flags |= BCM_MPLS_PORT_ENTROPY_ENABLE;
        }
        else if (_SHR_IS_FLAG_SET
                 (in_lif_profile_info.ingress_fields.cs_in_lif_profile_flags, DNX_IN_LIF_PROFILE_CS_PROFILE_EVPN_IML_AH_FL_CW))
        {
            mpls_port->flags |= BCM_MPLS_PORT_ENTROPY_ENABLE;
            mpls_port->flags |= BCM_MPLS_PORT_CONTROL_WORD;
        }
    }
    else
    {
        if (_SHR_IS_FLAG_SET(mpls_port->flags, BCM_MPLS_PORT_SERVICE_TAGGED))
        {
            mpls_port->nof_service_tags = in_lif_profile_info.ingress_fields.pwe_nof_eth_tags;
        }

        if (!_SHR_IS_FLAG_SET
            (in_lif_profile_info.ingress_fields.cs_in_lif_profile_flags, DNX_IN_LIF_PROFILE_CS_PROFILE_PWE_LIF_SCOPED))
        {
            mpls_port->flags2 |= BCM_MPLS_PORT2_PLATFORM_NAMESPACE;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**

* \brief
*  Get local and global lifs to be used for the tunnel.
*  If REPLACE is set lookup the lifs, otherwise allocate new ones.
*   \param [in] unit -Relevant unit.
*   \param [in] l2vpn        -  Holds all the relevant information for the different schemes that might be defined on a VPLS LIF.
*   \param [in,out] l2vpn_inlif_info - Holds the information specific to the in lif allocation
*   \param [out] inlif_dbal_result_type - Holds the result type to the in lif allocation
* \return
*   shr_error_e
*/
shr_error_e
dnx_mpls_l2vpn_ingress_lif_allocation_or_fill(
    int unit,
    dnx_mpls_l2vpn_info_t * l2vpn,
    dnx_mpls_l2vpn_in_lif_info_t * l2vpn_inlif_info,
    uint32 *inlif_dbal_result_type)
{
    uint32 lif_flags;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(l2vpn->mpls_port->flags, BCM_MPLS_PORT_REPLACE))
    {
        /** get local and global out lifs */
        lif_flags =
            DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS;
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, l2vpn->mpls_port->mpls_port_id, lif_flags, &gport_hw_resources));
        l2vpn_inlif_info->global_in_lif = gport_hw_resources.global_in_lif;
        l2vpn_inlif_info->local_in_lif = gport_hw_resources.local_in_lif;
        *inlif_dbal_result_type = gport_hw_resources.inlif_dbal_result_type;
    }
    else
    {
        /** Allocate Global LIF */
        lif_flags = LIF_MNGR_L2_GPORT_GLOBAL_LIF;
        if (l2vpn->mpls_port->flags2 & BCM_MPLS_PORT2_ALLOC_SYMMETRIC)
        {
            lif_flags |= LIF_MNGR_ONE_SIDED_SYMMETRIC_GLOBAL_LIF;
        }
        if (l2vpn->mpls_port->flags & BCM_MPLS_PORT_WITH_ID)
        {
            l2vpn_inlif_info->global_in_lif = ((l2vpn->type == MPLS_L2VPN_TYPE_EVPN) ?
                                               BCM_GPORT_TUNNEL_ID_GET(l2vpn->mpls_port->mpls_port_id) :
                                               BCM_GPORT_SUB_TYPE_LIF_VAL_GET(l2vpn->mpls_port->mpls_port_id));
            lif_flags |= LIF_MNGR_GLOBAL_LIF_WITH_ID;
        }
        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_allocate
                        (unit, lif_flags, DNX_ALGO_LIF_INGRESS, &l2vpn_inlif_info->global_in_lif));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get lif profile.
*   \param [in] unit -Relevant unit.
*   \param [in] l2vpn_info - Holds all the relevant information for the different schemes that might be defined on the object.
*   \param [in] old_in_lif_profile - If REPLACE was set is the previous lif profile allocated, otherwise DEFAULT
*   \param [in] inlif_dbal_table_id - includes information about lif table id
*   \param [out] new_in_lif_profile - New/updated lif profile to be used.
* \return
*   shr_error_e
*/
shr_error_e
dnx_mpls_port_ingress_lif_profile_fill_and_allocation(
    int unit,
    dnx_mpls_l2vpn_info_t * l2vpn_info,
    int old_in_lif_profile,
    int *new_in_lif_profile,
    dbal_tables_e inlif_dbal_table_id)
{
    bcm_mpls_port_t *mpls_port;
    in_lif_profile_info_t in_lif_profile_info;
    dbal_tables_e dbal_table_for_lif_profile;
    uint8 is_evpm_iml;

    SHR_FUNC_INIT_VARS(unit);

    mpls_port = l2vpn_info->mpls_port;
    is_evpm_iml = _SHR_IS_FLAG_SET(l2vpn_info->flags, (DNX_MPLS_L2VPN_FLAG_INGRESS_IML |
                                                       DNX_MPLS_L2VPN_FLAG_INGRESS_IML_WITH_ESI |
                                                       DNX_MPLS_L2VPN_FLAG_INGRESS_IML_WITHOUT_ESI));

    /*
     * For EVPN IML, the CS-profile is used for FL and CW indicator.
     * For other case, the CS-PROFILE is used for pwe-tagged-mode attributes:
     *     Set CS-PROFILE with the right no. of service tags.
     */
    SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));

    if ((inlif_dbal_table_id == DBAL_TABLE_IN_LIF_FORMAT_EVPN) && is_evpm_iml)
    {
        dbal_table_for_lif_profile = inlif_dbal_table_id;
        if (_SHR_IS_FLAG_SET(mpls_port->flags, BCM_MPLS_PORT_ENTROPY_ENABLE) &&
            _SHR_IS_FLAG_SET(mpls_port->flags, BCM_MPLS_PORT_CONTROL_WORD))
        {
            in_lif_profile_info.ingress_fields.cs_in_lif_profile_flags =
                DNX_IN_LIF_PROFILE_CS_PROFILE_EVPN_IML_AH_FL_CW;
        }
        else if (_SHR_IS_FLAG_SET(mpls_port->flags, BCM_MPLS_PORT_ENTROPY_ENABLE))
        {
            in_lif_profile_info.ingress_fields.cs_in_lif_profile_flags =
                DNX_IN_LIF_PROFILE_CS_PROFILE_EVPN_IML_AH_FL_ONLY;
        }
        else
        {
            in_lif_profile_info.ingress_fields.cs_in_lif_profile_flags =
                DNX_IN_LIF_PROFILE_CS_PROFILE_EVPN_IML_AH_NO_FL;
        }

        if (!_SHR_IS_FLAG_SET(mpls_port->flags2, BCM_MPLS_PORT2_PLATFORM_NAMESPACE))
        {
            in_lif_profile_info.ingress_fields.cs_in_lif_profile_flags |= DNX_IN_LIF_PROFILE_CS_PROFILE_EVPN_IML_LIF_SCOPED;
        }
    }
    else
    {
        dbal_table_for_lif_profile = DBAL_TABLE_IN_LIF_FORMAT_PWE;

        if (_SHR_IS_FLAG_SET(mpls_port->flags, BCM_MPLS_PORT_SERVICE_TAGGED))
        {
            in_lif_profile_info.ingress_fields.pwe_nof_eth_tags = mpls_port->nof_service_tags;
        }

        if (!_SHR_IS_FLAG_SET(mpls_port->flags2, BCM_MPLS_PORT2_PLATFORM_NAMESPACE))
        {
            in_lif_profile_info.ingress_fields.cs_in_lif_profile_flags = DNX_IN_LIF_PROFILE_CS_PROFILE_PWE_LIF_SCOPED;
        }
    }

    /*
     * Split Horizon. Set InLIF profile with the right in_lif_orientation.
     */
    in_lif_profile_info.egress_fields.in_lif_orientation = mpls_port->network_group_id;

    SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                    (unit, &in_lif_profile_info, old_in_lif_profile, new_in_lif_profile, LIF,
                     dbal_table_for_lif_profile));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Fill all the learning data needed for a PWE in-lif
 * \param [in] unit - Relevant unit ID
 * \param [in] fwd_info - Forwarding information that is used for packets traveling in the opposite direction (what should be learned)
 * \param [out] lif_info - This object's learning information would be set with the relevant data from fwd_info
 */
static shr_error_e
dnx_mpls_port_learn_info_from_fwd_info_fill(
    int unit,
    dnx_algo_gpm_forward_info_t * fwd_info,
    dnx_mpls_l2vpn_in_lif_info_t * lif_info)
{
    dbal_fields_e learn_info_field_id;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Input Validations
     */
    SHR_NULL_CHECK(fwd_info, _SHR_E_INTERNAL, "fwd_info");
    SHR_NULL_CHECK(lif_info, _SHR_E_INTERNAL, "lif_info");

    /*
     * For dest and outlif:
     * Use ASYMMETRIC_LIF_BASIC
     */
    if (fwd_info->fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF)
    {
        /** set field id for info structure encoding */
        learn_info_field_id = DBAL_FIELD_LEARN_INFO_ASYM_LIF_BASIC_EXT;

        /** set learn context*/
        lif_info->learn_context = DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_ASYMMETRIC_LIF_BASIC;

        /** set global out lif */
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                        (unit, learn_info_field_id, DBAL_FIELD_GLOB_OUT_LIF, &fwd_info->outlif, lif_info->learn_info));
    }
    /*
     * For dest only:
     * Use NO_LIF_BASIC
     */
    else if (fwd_info->fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_ONLY)
    {
        /** set field id for info structure encoding */
        learn_info_field_id = DBAL_FIELD_LEARN_INFO_NO_LIF_BASIC_SYM_LIF_BASIC;

        /** set learn context*/
        lif_info->learn_context = DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF_BASIC;
    }
    /*
     * Jericho mode beginning
     * {
     */
    else if (fwd_info->fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_EEI)
    {
        /** set field id for info structure encoding */
        learn_info_field_id = DBAL_FIELD_LEARN_INFO_ASYM_LIF_EXT_EEI;

        /** set learn context*/
        lif_info->learn_context = DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_ASYMMETRIC_LIF_EXTENDED_EEI;

        /** set EEI */
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                        (unit, learn_info_field_id, DBAL_FIELD_EEI, &fwd_info->eei, lif_info->learn_info));
    }
    /*
     * }
     * Jericho mode end
     */

    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Unexpected fwd_info_result_type value - %d. should be DEST_ONLY or DEST_OUTLIF\n",
                     fwd_info->fwd_info_result_type);
    }

    /** set destination */
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, learn_info_field_id, DBAL_FIELD_DESTINATION, &fwd_info->destination, lif_info->learn_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Fills the match information for L2VPN object.
 * For PWE/EVPN, this will be the match label.
 * For IML, this will be the match label + BOS expectation
 * \param [in] unit
 * \param [in] l2vpn - Input structure with the relevant information: l2vpn-type and flags
 * \param [out] match - Output structure with the matching information
 */
static shr_error_e
dnx_mpls_l2vpn_match_info_fill(
    int unit,
    dnx_mpls_l2vpn_info_t * l2vpn,
    dnx_mpls_term_match_t * match)
{
    SHR_FUNC_INIT_VARS(unit);

    /** add label to termination table */
    match->label = l2vpn->mpls_port->match_label;

    match->service_tagged = _SHR_IS_FLAG_SET(l2vpn->mpls_port->flags, BCM_MPLS_PORT_SERVICE_TAGGED);

    if (l2vpn->mpls_port->criteria == BCM_MPLS_PORT_MATCH_LABEL_CONTEXT_LABEL)
    {
        match->match_criteria = TERM_MATCH_CRITERIA_TWO_LABELS;
        match->context_label = l2vpn->mpls_port->context_label;
    }
    else if (l2vpn->mpls_port->criteria == BCM_MPLS_PORT_MATCH_LABEL_L3_INGRESS_INTF)
    {
        match->match_criteria = TERM_MATCH_CRITERIA_INTF_LABEL;
        match->context_intf = l2vpn->mpls_port->ingress_if;
    }
    else if (_SHR_IS_FLAG_SET(l2vpn->flags, DNX_MPLS_L2VPN_FLAG_INGRESS_IML_WITH_ESI))
    {
        match->match_criteria = TERM_MATCH_CRITERIA_LABEL_BOS;
        match->bos_val = 0;
    }
    else if (_SHR_IS_FLAG_SET(l2vpn->flags, DNX_MPLS_L2VPN_FLAG_INGRESS_IML_WITHOUT_ESI))
    {
        match->match_criteria = TERM_MATCH_CRITERIA_LABEL_BOS;
        match->bos_val = 1;
    }
    else
    {
        match->match_criteria = TERM_MATCH_CRITERIA_LABEL;
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Sets matching information fields in 'l2vpn' structure according to the matching information in 'match'.
 * \param [in] unit
 * \param [in] match - contains the match criteria and information relevant to this critiria
 * \param [out] l2vpn - mpls_l2vpn structure with BCM API facing information.
 */
static shr_error_e
dnx_mpls_match_info_to_l2vpn(
    int unit,
    dnx_mpls_term_match_t * match,
    dnx_mpls_l2vpn_info_t * l2vpn)
{
    SHR_FUNC_INIT_VARS(unit);

    l2vpn->mpls_port->match_label = match->label;
    l2vpn->mpls_port->criteria = BCM_MPLS_PORT_MATCH_LABEL;
    if (match->service_tagged)
    {
        l2vpn->mpls_port->flags |= BCM_MPLS_PORT_SERVICE_TAGGED;
    }

    if (match->match_criteria == TERM_MATCH_CRITERIA_LABEL_BOS)
    {
        l2vpn->flags |= (match->bos_val) ?
            DNX_MPLS_L2VPN_FLAG_INGRESS_IML_WITHOUT_ESI : DNX_MPLS_L2VPN_FLAG_INGRESS_IML_WITH_ESI;
    }
    else if (match->match_criteria == TERM_MATCH_CRITERIA_TWO_LABELS)
    {
        l2vpn->mpls_port->criteria = BCM_MPLS_PORT_MATCH_LABEL_CONTEXT_LABEL;
        l2vpn->mpls_port->context_label = match->context_label;
    }
    else if (match->match_criteria == TERM_MATCH_CRITERIA_INTF_LABEL)
    {
        l2vpn->mpls_port->criteria = BCM_MPLS_PORT_MATCH_LABEL_L3_INGRESS_INTF;
        l2vpn->mpls_port->ingress_if = match->context_intf;
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Reads the network group ID from the in lif profile.
 */
static shr_error_e
dnx_mpls_l2vpn_in_network_group_read(
    int unit,
    int in_lif_profile,
    dnx_mpls_l2vpn_info_t * l2vpn_info)
{
    in_lif_profile_info_t in_lif_profile_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, in_lif_profile, &in_lif_profile_info, LIF));
    l2vpn_info->mpls_port->network_group_id = in_lif_profile_info.egress_fields.in_lif_orientation;

exit:
    SHR_FUNC_EXIT;
}

/*
* See .h file for documentation
*/
shr_error_e
dnx_mpls_l2vpn_add_ingress_only(
    int unit,
    dnx_mpls_l2vpn_info_t * l2vpn_info)
{
    int gport_id;
    dnx_mpls_l2vpn_in_lif_info_t pwe_lif_info;
    dnx_mpls_l2vpn_in_lif_info_t orig_pwe_lif_info;
    dnx_mpls_termination_profile_t term_profile_info;
    uint8 is_first_term_profile_reference = 0, is_last_term_profile_reference = 0;
    dnx_qos_propagation_type_e ttl_propagation_type, phb_propagation_type,
        remark_propagation_type, ecn_propagation_type;
    uint8 propag_profile_first_reference = 0, propag_profile_last_reference = 0;
    dnx_mpls_term_match_t term_match, orig_term_match;
    lif_table_mngr_inlif_info_t lif_info;
    uint32 entry_handle_id;
    int is_replace = _SHR_IS_FLAG_SET(l2vpn_info->mpls_port->flags, BCM_MPLS_PORT_REPLACE);
    uint32 eei = 0, out_lif = 0, destination = 0;

    /** Needed to prevent Coverity issue of Out-of-bounds access (ARRAY_VS_SINGLETON) */
    int protection_pointer_encoded[1] = { 0 };
    int protection_pointer[1] = { 0 };
    uint32 inlif_dbal_result_type = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Init in-lif info structures
     */
    sal_memset(&pwe_lif_info, 0, sizeof(dnx_mpls_l2vpn_in_lif_info_t));
    sal_memset(&orig_pwe_lif_info, 0, sizeof(dnx_mpls_l2vpn_in_lif_info_t));
    /*
     * Init term match structures
     */
    sal_memset(&term_match, 0, sizeof(dnx_mpls_term_match_t));
    sal_memset(&orig_term_match, 0, sizeof(dnx_mpls_term_match_t));

    /*
     * Set user supplied information in the structure
     */
    pwe_lif_info.vsi = l2vpn_info->vpn;
    pwe_lif_info.is_p2p = _SHR_IS_FLAG_SET(l2vpn_info->mpls_port->flags2, BCM_MPLS_PORT2_CROSS_CONNECT);
    pwe_lif_info.is_stat = _SHR_IS_FLAG_SET(l2vpn_info->mpls_port->flags2, BCM_MPLS_PORT2_STAT_ENABLE);
    /** Get app db and result type */
    pwe_lif_info.dbal_table =
        (l2vpn_info->type == MPLS_L2VPN_TYPE_MPLS_PORT) ? DBAL_TABLE_IN_LIF_FORMAT_PWE : DBAL_TABLE_IN_LIF_FORMAT_EVPN;

    /** allocate global lif or retrieve in case of replace */
    SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_ingress_lif_allocation_or_fill
                    (unit, l2vpn_info, &pwe_lif_info, &inlif_dbal_result_type));

    /*
     * Get match criteria from input structure
     */
    SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_match_info_fill(unit, l2vpn_info, &term_match));

    if (is_replace)
    {
        /** Get entry from MPLS IN-LIF table */
        orig_pwe_lif_info.dbal_table = pwe_lif_info.dbal_table;
        orig_pwe_lif_info.local_in_lif = pwe_lif_info.local_in_lif;
        SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_in_lif_get(unit, &orig_pwe_lif_info));

        SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_termination_match_information_get
                        (unit, _SHR_CORE_ALL, pwe_lif_info.local_in_lif, &orig_term_match));
        if (sal_memcmp(&term_match, &orig_term_match, sizeof(dnx_mpls_term_match_t)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Matching information must not be changed if REPLACE was set");
        }
        if (l2vpn_info->type == MPLS_L2VPN_TYPE_EVPN)
        {
            /*
             * Nwk-Group-ID is set through another API for EVPN. When replacing the object, need to make sure the
             * Nwk-Group-ID is not being run over.
             */
            SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_in_network_group_read(unit, orig_pwe_lif_info.in_lif_profile, l2vpn_info));
        }
    }
    else
    {
        /*
         * "Original" in lif profile is still used, even without REPLACE flag.
         */
        orig_pwe_lif_info.in_lif_profile = DNX_IN_LIF_PROFILE_DEFAULT;
        orig_pwe_lif_info.propagation_profile = DNX_QOS_INGRESS_DEFAULT_PROPAGATION_PROFILE;
    }

    if (l2vpn_info->type == MPLS_L2VPN_TYPE_EVPN)
    {
        _SHR_GPORT_TUNNEL_ID_SET(l2vpn_info->mpls_port->mpls_port_id, pwe_lif_info.global_in_lif);
    }
    else
    {
        /** MPLS_PORT */
        _SHR_GPORT_SUB_TYPE_LIF_SET(gport_id, _SHR_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY, pwe_lif_info.global_in_lif);
        _SHR_GPORT_MPLS_PORT_ID_SET(l2vpn_info->mpls_port->mpls_port_id, gport_id);
    }

    if ((l2vpn_info->mpls_port->failover_mc_group == 0) && (l2vpn_info->mpls_port->failover_port_id == 0))
    {
        /*
         * If not failover port or MC is set, learning information including out lif, taken from encap_id.
         * If encap_id is 0, learn symmetric
         */
        if (l2vpn_info->mpls_port->encap_id == 0)
        {
            l2vpn_info->mpls_port->encap_id = pwe_lif_info.global_in_lif;
        }
    }

    /** Check if Protection is enabled */
    if (DNX_FAILOVER_IS_PROTECTION_ENABLED(l2vpn_info->mpls_port->ingress_failover_id))
    {
        DNX_FAILOVER_ID_GET(protection_pointer[0], l2vpn_info->mpls_port->ingress_failover_id);
        /** Encode IN-LIF protection pointer format from Failover ID */
        DNX_FAILOVER_ID_PROTECTION_POINTER_ENCODE(protection_pointer_encoded[0], protection_pointer[0]);
        pwe_lif_info.protection_pointer = protection_pointer_encoded[0];
        pwe_lif_info.protection_path = l2vpn_info->mpls_port->ingress_failover_port_id ? 0 : 1;
        pwe_lif_info.is_protect = 1;
    }
    else
    {
        pwe_lif_info.protection_pointer = dnx_data_failover.path_select.ing_no_protection_get(unit);
        pwe_lif_info.is_protect = 0;
    }

    /*
     * Not storing fwd info for EVPN. There is no learning in EVPN, and for P2P,
     * we don't support getting the fwd information from the logical gport.
     */
    if (l2vpn_info->type != MPLS_L2VPN_TYPE_EVPN)
    {
        dnx_algo_gpm_forward_info_t fwd_info;

        /*
         * Calculate the fwd info. In case of REPLACE, we still need this information in order
         * to set the learning information.
         */
        SHR_IF_ERR_EXIT(dnx_mpls_port_to_forward_information(unit, l2vpn_info->mpls_port, &fwd_info));
        if (pwe_lif_info.is_p2p == FALSE)
        {
            /*
             * Set learning information
             */
            pwe_lif_info.learn_enable = (is_replace ? orig_pwe_lif_info.learn_enable : 1);
            SHR_IF_ERR_EXIT(dnx_mpls_port_learn_info_from_fwd_info_fill(unit, &fwd_info, &pwe_lif_info));
        }
        /*
         * Fill destination (from Gport) into Forward Info table (SW state)
         * If REPLACE set first clear old entry
         */
        if (_SHR_IS_FLAG_SET(l2vpn_info->mpls_port->flags, BCM_MPLS_PORT_REPLACE))
        {
            SHR_IF_ERR_EXIT(algo_gpm_gport_l2_forward_info_delete(unit, l2vpn_info->mpls_port->mpls_port_id));
        }
        SHR_IF_ERR_EXIT(algo_gpm_gport_l2_forward_info_add(unit, 0, l2vpn_info->mpls_port->mpls_port_id, &fwd_info));
    }

    /*
     * There are differences on the usage of CS-PROFILE between EVPN-IML and MPLS-PORT. The table-id distinguishes them.
     */
    SHR_IF_ERR_EXIT(dnx_mpls_port_ingress_lif_profile_fill_and_allocation
                    (unit, l2vpn_info, orig_pwe_lif_info.in_lif_profile, &pwe_lif_info.in_lif_profile,
                     pwe_lif_info.dbal_table));

    /** get propagation profile, if REPLACE used profile will be updated */
    phb_propagation_type = remark_propagation_type = ttl_propagation_type = DNX_QOS_PROPAGATION_TYPE_PIPE;
    ecn_propagation_type = DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE;

    if (l2vpn_info->mpls_port->ingress_qos_model.ingress_phb != bcmQosIngressModelInvalid)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_qos_model_to_propag_type
                        (unit, l2vpn_info->mpls_port->ingress_qos_model.ingress_phb, &phb_propagation_type));
    }
    if (l2vpn_info->mpls_port->ingress_qos_model.ingress_remark != bcmQosIngressModelInvalid)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_qos_model_to_propag_type
                        (unit, l2vpn_info->mpls_port->ingress_qos_model.ingress_remark, &remark_propagation_type));
    }

    if (l2vpn_info->mpls_port->ingress_qos_model.ingress_ttl != bcmQosIngressModelInvalid)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_qos_model_to_propag_type
                        (unit, l2vpn_info->mpls_port->ingress_qos_model.ingress_ttl, &ttl_propagation_type));
    }

    SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_alloc(unit,
                                                              phb_propagation_type, remark_propagation_type,
                                                              ecn_propagation_type, ttl_propagation_type,
                                                              orig_pwe_lif_info.propagation_profile,
                                                              (int *) &pwe_lif_info.propagation_profile,
                                                              &propag_profile_first_reference,
                                                              &propag_profile_last_reference));

    /** Termination profile */
    SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_to_termination_profile(unit, l2vpn_info, &term_profile_info));
    if (is_replace)
    {
        SHR_IF_ERR_EXIT(dnx_mpls_termination_profile_allocation
                        (unit, term_profile_info, &pwe_lif_info.term_profile, orig_pwe_lif_info.term_profile,
                         &is_first_term_profile_reference, &is_last_term_profile_reference));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_mpls_termination_profile_allocation
                        (unit, term_profile_info, &pwe_lif_info.term_profile, 0, &is_first_term_profile_reference,
                         NULL));
    }

    /*
     * HW tables
     */
    /** propagation profile */
    if (propag_profile_first_reference)
    {
        /** Set propagation profile in case it is the first time. */
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_hw_set
                        (unit, pwe_lif_info.propagation_profile, phb_propagation_type, remark_propagation_type,
                         ecn_propagation_type, ttl_propagation_type));
    }

    /** termination profile */
    if (is_first_term_profile_reference)
    {
        /** Set profile termination in case it is the first time. */
        SHR_IF_ERR_EXIT(dnx_mpls_termination_profile_hw_set(unit, &term_profile_info, pwe_lif_info.term_profile));
    }
    if (pwe_lif_info.is_p2p && is_replace)
    {
        SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_in_lif_p2p_info_get
                        (unit, &pwe_lif_info, inlif_dbal_result_type, &out_lif, &eei, &destination));
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, pwe_lif_info.dbal_table, &entry_handle_id));
    SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_in_lif_to_lif_table_manager
                    (unit, &pwe_lif_info, entry_handle_id, out_lif, eei, destination));
    /*
     * Find result type, allocate lif and write to HW
     */
    sal_memset(&lif_info, 0, sizeof(lif_table_mngr_inlif_info_t));
    lif_info.global_lif = pwe_lif_info.global_in_lif;

    if (is_replace)
    {
        lif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_REPLACE;
    }
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_local_inlif_and_set_hw
                    (unit, _SHR_CORE_ALL, entry_handle_id, &pwe_lif_info.local_in_lif, &lif_info));

    if (!is_replace)
    {
        /*
         * Add match info.
         * REPLACE should not alter match information
         */
        SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_termination_match_information_set
                        (unit, _SHR_CORE_ALL, pwe_lif_info.local_in_lif, &term_match));
        SHR_IF_ERR_EXIT(dnx_mpls_termination_lookup
                        (unit, TERM_LOOKUP_FORCE_SET, &term_match, &pwe_lif_info.local_in_lif));
    }

    /*
     * After new entry values are saved to HW delete redundant old values from HW
     */

    if (is_last_term_profile_reference)
    {
        /** Delete profile termination in case it is the last time. */
        SHR_IF_ERR_EXIT(dnx_mpls_hw_termination_profile_delete(unit, orig_pwe_lif_info.term_profile));
    }

    if (propag_profile_last_reference)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_hw_clear(unit, orig_pwe_lif_info.propagation_profile));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
* See .h file for documentation
*/
shr_error_e
dnx_mpls_l2vpn_get_ingress_only(
    int unit,
    dnx_mpls_l2vpn_info_t * l2vpn_info)
{
    bcm_gport_t gport_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 lif_flags = 0;
    dnx_mpls_l2vpn_in_lif_info_t lif_info;
    int ref_count;
    dnx_mpls_termination_profile_t term_profile_info;
    dnx_qos_propagation_type_e phb_propagation_type, remark_propagation_type, ecn_propagation_type,
        ttl_propagation_type;
    dnx_mpls_term_match_t term_match;

    /** Needed to prevent Coverity issue of Out-of-bounds access (ARRAY_VS_SINGLETON) */
    int protection_pointer_decoded[1] = { 0 };
    int protection_pointer[1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&term_match, 0, sizeof(dnx_mpls_term_match_t));
    sal_memset(&lif_info, 0, sizeof(dnx_mpls_l2vpn_in_lif_info_t));

    /** get local and global out lifs */
    lif_flags |= DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS;
    gport_id = l2vpn_info->mpls_port->mpls_port_id;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport_id, lif_flags, &gport_hw_resources));

    /*
     * Initialize the structure
     */
    bcm_mpls_port_t_init(l2vpn_info->mpls_port);
    l2vpn_info->mpls_port->mpls_port_id = gport_id;
    l2vpn_info->mpls_port->flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY;

    /** get info->mpls_port->match_label based on gport_hw_resources.local_in_lif using the SW state */
    SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_termination_match_information_get
                    (unit, _SHR_CORE_ALL, gport_hw_resources.local_in_lif, &term_match));
    SHR_IF_ERR_EXIT(dnx_mpls_match_info_to_l2vpn(unit, &term_match, l2vpn_info));

    /** Get entry from MPLS IN-LIF table */
    lif_info.dbal_table = gport_hw_resources.inlif_dbal_table_id;
    lif_info.local_in_lif = gport_hw_resources.local_in_lif;
    SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_in_lif_get(unit, &lif_info));

    /** Get Failover ID from IN-LIF protection pointer format */
    protection_pointer[0] = lif_info.protection_pointer;
    DNX_FAILOVER_ID_PROTECTION_POINTER_DECODE(protection_pointer_decoded[0], protection_pointer[0]);
    l2vpn_info->mpls_port->ingress_failover_id = protection_pointer_decoded[0];

    if (l2vpn_info->mpls_port->ingress_failover_id != dnx_data_failover.path_select.ing_no_protection_get(unit))
    {
        DNX_FAILOVER_SET(l2vpn_info->mpls_port->ingress_failover_id, l2vpn_info->mpls_port->ingress_failover_id,
                         DNX_FAILOVER_TYPE_INGRESS);
        l2vpn_info->mpls_port->ingress_failover_port_id = lif_info.protection_path ? 0 : 1;
    }

    l2vpn_info->mpls_port->flags2 |= (lif_info.is_p2p ? BCM_MPLS_PORT2_CROSS_CONNECT : 0);
    l2vpn_info->mpls_port->flags2 |= (lif_info.is_stat ? BCM_MPLS_PORT2_STAT_ENABLE : 0);
    l2vpn_info->vpn = lif_info.vsi;

    /** Get propagation profile properties */
    SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_attr_get
                    (unit, lif_info.propagation_profile, &phb_propagation_type, &remark_propagation_type,
                     &ecn_propagation_type, &ttl_propagation_type));

    if (ttl_propagation_type != DNX_QOS_PROPAGATION_TYPE_PIPE)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propag_type_to_qos_model
                        (unit, ttl_propagation_type, &(l2vpn_info->mpls_port->ingress_qos_model.ingress_ttl)));
    }

    if (phb_propagation_type != DNX_QOS_PROPAGATION_TYPE_PIPE)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propag_type_to_qos_model
                        (unit, phb_propagation_type, &(l2vpn_info->mpls_port->ingress_qos_model.ingress_phb)));
    }
    if (remark_propagation_type != DNX_QOS_PROPAGATION_TYPE_PIPE)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propag_type_to_qos_model
                        (unit, remark_propagation_type, &(l2vpn_info->mpls_port->ingress_qos_model.ingress_remark)));
    }

    if (l2vpn_info->type != MPLS_L2VPN_TYPE_EVPN)
    {
        /** get forwarding info */
        SHR_IF_ERR_EXIT(dnx_mpls_port_gport_to_forward_information_get(unit, l2vpn_info->mpls_port));
    }

    /** get termination profile info */
    sal_memset(&term_profile_info, 0, sizeof(term_profile_info));
    SHR_IF_ERR_EXIT(algo_mpls_db.
                    termination_profile.profile_data_get(unit, lif_info.term_profile, &ref_count, &term_profile_info));
    SHR_IF_ERR_EXIT(dnx_mpls_termination_profile_to_mpls_l2vpn_info(unit, &term_profile_info, l2vpn_info));

    /*
     * Get inlif profile info
     */
    SHR_IF_ERR_EXIT(dnx_mpls_inlif_profile_to_mpls_port_info(unit, lif_info.in_lif_profile, l2vpn_info));

exit:
    SHR_FUNC_EXIT;
}

/*
* See .h file for documentation
*/
shr_error_e
dnx_mpls_l2vpn_delete_ingress_only(
    int unit,
    bcm_gport_t mpls_l2vpn_id)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 lif_flags = 0;
    lif_mngr_local_inlif_info_t inlif_info;
    dnx_mpls_l2vpn_in_lif_info_t lif_info;
    uint8 is_last;
    int new_in_lif_profile;
    int removing_local_in_lif;
    dnx_mpls_term_match_t match_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&inlif_info, 0, sizeof(inlif_info));
    sal_memset(&lif_info, 0, sizeof(lif_info));
    sal_memset(&match_info, 0, sizeof(match_info));

    /** get local and global out lifs */
    lif_flags |= DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, mpls_l2vpn_id, lif_flags, &gport_hw_resources));

    /** Get entry from MPLS IN-LIF table */
    lif_info.dbal_table = gport_hw_resources.inlif_dbal_table_id;
    lif_info.local_in_lif = gport_hw_resources.local_in_lif;
    SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_in_lif_get(unit, &lif_info));

    /** get termination label match SW state entry */
    SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_termination_match_information_get
                    (unit, _SHR_CORE_ALL, gport_hw_resources.local_in_lif, &match_info));

    /*
     * delete HW tables
     */
    /** Get the label match entry from table to check if the lif matches*/
    SHR_IF_ERR_EXIT(dnx_mpls_termination_lookup(unit, TERM_LOOKUP_GET, &match_info, &removing_local_in_lif));

    /** delete label from termination table if the lif matches. Otherwise, leave it there.*/
    if (removing_local_in_lif == gport_hw_resources.local_in_lif)
    {
        SHR_IF_ERR_EXIT(dnx_mpls_termination_lookup(unit, TERM_LOOKUP_DELETE, &match_info, NULL));
    }

    /*
     * Free Lif table and local LIF allocation
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_inlif_info_clear
                    (unit, _SHR_CORE_ALL, gport_hw_resources.local_in_lif, gport_hw_resources.inlif_dbal_table_id, 0));

    /** delete propagation profile from SW and HW */
    SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_free_and_hw_clear(unit, lif_info.propagation_profile, NULL));

    /*
     * delete SW tables
     */
    /** delete termination profile SW state entry */
    SHR_IF_ERR_EXIT(algo_mpls_db.termination_profile.free_single(unit, lif_info.term_profile, &is_last));

    /** Delete profile termination entry from the HW table */
    if (is_last)
    {
        SHR_IF_ERR_EXIT(dnx_mpls_hw_termination_profile_delete(unit, lif_info.term_profile));
    }

    if (gport_hw_resources.inlif_dbal_table_id != DBAL_TABLE_IN_LIF_FORMAT_EVPN)
    {
        /** delete forwarding info */
        SHR_IF_ERR_EXIT(dnx_mpls_port_gport_to_forward_information_delete(unit, mpls_l2vpn_id));
    }

    /** delete match SW state entry */
    SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_termination_match_information_delete
                    (unit, _SHR_CORE_ALL, gport_hw_resources.local_in_lif));

    /** Free the inlif_profile */
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_dealloc(unit, lif_info.in_lif_profile, &new_in_lif_profile, LIF));

    /** delete global lif */
    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_free(unit, DNX_ALGO_LIF_INGRESS, gport_hw_resources.global_in_lif));

exit:
    SHR_FUNC_EXIT;
}
