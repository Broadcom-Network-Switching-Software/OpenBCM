/** \file mpls_port.c General MPLS VPLS functionality for DNX.
 * This file contains the dispatched functions of the mpls_port
 * APIs. These functions invoke verification of the user
 * supplied parameters and pass the actual implementation to
 * another module called mpls_l2vpn, which handles all MPLS
 * based L2VPN implementations.
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
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mpls.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <bcm_int/dnx/switch/switch.h>
#include <bcm_int/dnx/failover/failover.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/qos/qos.h>
#include <bcm_int/dnx/mpls/mpls.h>
#include <bcm_int/dnx/mpls/mpls_tunnel_encap.h>
#include <bcm_int/dnx/mpls_l2vpn/mpls_l2vpn.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

#include <src/bcm/dnx/init/init_pp.h>
/*
 * }
 */

/*
 * In JR push profile values 8,9,0 are reserved for swap, pop and default commands.
 */
#define DNX_MPLS_IS_RESERVED_PUSH_PROFILE(push_profile) \
        ((push_profile) == DBAL_FIELD_EEI_MPLS_POP_COMMAND || \
         (push_profile) == DBAL_FIELD_EEI_MPLS_SWAP_COMMAND || (push_profile) == 0) ? 1 : 0

shr_error_e
dnx_mpls_cw_set(
    int unit,
    int cw)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Whole CW is saved in DBAL_TABLE_MPLS_GENERAL_CONFIGURATION
     * Flag bits is from additional header profile
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MPLS_GENERAL_CONFIGURATION, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_ENCAP_CONTROL_WORD, INST_SINGLE, cw);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(egress_additional_headers_mpls_control_word_set(unit, cw));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_mpls_cw_get(
    int unit,
    int *cw)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MPLS_GENERAL_CONFIGURATION, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MPLS_ENCAP_CONTROL_WORD, INST_SINGLE, (uint32 *) cw);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* check if the given vpn is valid as l2 VPN */
static shr_error_e
dnx_mpls_port_l2_vpn_check(
    int unit,
    bcm_vpn_t vpn)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Check that the vsi was allocated:
     */
    if (vpn != 0)
    {
        uint8 is_allocated = 0;

        SHR_IF_ERR_EXIT(vlan_db.vsi.is_allocated(unit, vpn, &is_allocated));

        if (is_allocated == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "vsi %d doesn't exist\n", vpn);
        }

    }
    /*
     * else
     * In case of P2P, vpn = 0, thus skipping the allocation check.
     */

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Verify mpls_port_id and vpn correctness.
* \par DIRECT INPUT:
*   \param [in] unit       -  Relevant unit.
*   \param [in] vpn        -  vpn (VSI) value.
*   \param [in] mpls_port_id  -  Global lif that needs to be deleted.
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \see
*   * \ref bcm_dnx_mpls_port_delete
*/
static shr_error_e
dnx_mpls_port_vpn_port_verify(
    int unit,
    bcm_vpn_t vpn,
    bcm_gport_t mpls_port_id)
{
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    /*
     * is push profile ID
     */
    if (BCM_GPORT_SUB_TYPE_IS_MPLS_PUSH_PROFILE(mpls_port_id))
    {
        if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "PUSH_INFO operation is noly supported in JR mode");
        }

        SHR_EXIT();

    }

    /*
     * is mpls port
     */
    if (!BCM_GPORT_IS_MPLS_PORT(mpls_port_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "mpls_port_id is not an MPLS port type");
    }

    if ((BCM_GPORT_SUB_TYPE_LIF_EXC_GET(mpls_port_id) == BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY))
    {
        if (vpn != 0)
        {
            /** vpn is not used in egress flow thus it must be 0 to avoid confusion */
            SHR_ERR_EXIT(_SHR_E_PARAM, "vpn(%d) must be zero for egress flow", vpn);
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_mpls_port_l2_vpn_check(unit, vpn));
    }

    /*
     * Jericho mode beginning
     * {
     */
    if (system_headers_mode != DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
        if (BCM_GPORT_SUB_TYPE_IS_MPLS_PUSH_PROFILE(mpls_port_id))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "MPLS push profile subtype is noly supported in JR mode");
        }
    }
    /*
     * }
     * Jericho mode end
     */

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Verify mpls_port struct and vpn according to MPLS port addition requirements.
* \par DIRECT INPUT:
*   \param [in] unit       -  Relevant unit.
*   \param [in] mpls_port  -  Holds all the relevant information for the different schemes that might be defined on a VPLS LIF.
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \remark
*   * 'mpls_port' is not NULL
*/
static shr_error_e
dnx_mpls_port_add_ingress_only_verify(
    int unit,
    bcm_mpls_port_t * mpls_port)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * only criteria:
     *   BCM_MPLS_PORT_MATCH_LABEL
     *   BCM_MPLS_PORT_MATCH_LABEL_CONTEXT_LABEL
     *   BCM_MPLS_PORT_MATCH_LABEL_L3_INGRESS_INTF
     * are suported */
    if ((mpls_port->criteria != BCM_MPLS_PORT_MATCH_LABEL) &&
        (mpls_port->criteria != BCM_MPLS_PORT_MATCH_LABEL_CONTEXT_LABEL) &&
        (mpls_port->criteria != BCM_MPLS_PORT_MATCH_LABEL_L3_INGRESS_INTF))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "match criteria must be BCM_MPLS_PORT_MATCH_LABEL, "
                     "BCM_MPLS_PORT_MATCH_LABEL_CONTEXT_LABEL or BCM_MPLS_PORT_MATCH_LABEL_L3_INGRESS_INTF.");
    }

    if (BCM_L3_ITF_TYPE_IS_LIF(mpls_port->egress_tunnel_if))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "mpls_port.egress_tunnel_if(%d) is LIF - not supported",
                     mpls_port->egress_tunnel_if);
    }

    if (!DNX_MPLS_LABEL_IN_RANGE(unit, mpls_port->match_label))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "mpls_port.match_label(%d) is out of range. The range should be 0 - %d",
                     mpls_port->match_label, DNX_MPLS_LABEL_MAX(unit));
    }

    if (mpls_port->flags & BCM_MPLS_PORT_SERVICE_TAGGED)
    {
        /*
         * PWE tagged mode. Make sure there are tags.
         */
        if (!DNX_MPLS_L2VPN_INGRESS_SERVICE_TAGS_NOF_IN_RANGE(mpls_port->nof_service_tags))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "mpls_port.nof_service_tags(%d) is out of range. The range should be %d - %d",
                         mpls_port->nof_service_tags, DNX_MPLS_L2VPN_INGRESS_SERVICE_TAGS_MIN,
                         DNX_MPLS_L2VPN_INGRESS_SERVICE_TAGS_MAX);
        }
    }
    else if (mpls_port->nof_service_tags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "mpls_port.nof_service_tags(%d) must be 0 if BCM_MPLS_PORT_SERVICE_TAGGED not set",
                     mpls_port->nof_service_tags);
    }

    if (mpls_port->qos_map_id != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "qos_map_id is cannot be configured through bcm_mpls_port api."
                     "bcm_qos_port_map_set should be used");
    }

    /** Check the learning information if Protection is enabled for MP */
    if (DNX_FAILOVER_IS_PROTECTION_ENABLED(mpls_port->ingress_failover_id)
        && !_SHR_IS_FLAG_SET(mpls_port->flags2, BCM_MPLS_PORT2_CROSS_CONNECT))
    {
        if ((mpls_port->failover_mc_group == 0) && (mpls_port->failover_port_id == 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "In case of Ingress protection one of failover_mc_group or failover_port_id must be set, both cannot be 0, because one of them is used for learning information");
        }
    }

    /*
     * Check learn info validity:
     */
    if (mpls_port->failover_port_id != 0)
    {
        /*
         * If failover_port_id is set
         * and both egress_tunnel_if and failover_mc_group have to be invalid (0)
         */
        if ((mpls_port->egress_tunnel_if != 0) || (mpls_port->failover_mc_group != 0) || (mpls_port->encap_id != 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "encap_id(0x%08x), failover_mc_group(0x%08x) and egress_tunnel_if(0x%08x) have to be zero when failover_port_id(0x%08x) is set as FEC.\n",
                         mpls_port->encap_id, mpls_port->failover_mc_group, mpls_port->egress_tunnel_if,
                         mpls_port->failover_port_id);
        }
    }
    else if (mpls_port->failover_mc_group != 0)
    {
        /*
         * If failover_mc_group is set
         * and both egress_tunnel_if and failover_port_id have to be invalid (0)
         */
        if ((mpls_port->egress_tunnel_if != 0) || (mpls_port->encap_id != 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "encap_id(0x%08x), failover_port_id(0x%08x) and egress_tunnel_if(0x%08x) have to be zero when failover_mc_group(0x%08x) is set as MC group.\n",
                         mpls_port->encap_id, mpls_port->failover_port_id, mpls_port->egress_tunnel_if,
                         mpls_port->failover_mc_group);
        }
    }
    else
    {
        if (!_SHR_L3_ITF_TYPE_IS_FEC(mpls_port->egress_tunnel_if) && (mpls_port->egress_tunnel_if != 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "egress_tunnel_if(0x%08x) have to be zero or FEC.\n", mpls_port->egress_tunnel_if);
        }
        if (BCM_L3_ITF_VAL_GET(mpls_port->encap_id) > (dnx_data_lif.global_lif.nof_global_out_lifs_get(unit) - 1))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "encap_id (0x%08x) is used for learning information, must be valid out-lif or 0",
                         mpls_port->encap_id);
        }
    }

    if (_SHR_IS_FLAG_SET(mpls_port->flags, BCM_MPLS_PORT_ENCAP_WITH_ID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flag BCM_MPLS_PORT_ENCAP_WITH_ID cannot be used along with INGRESS_ONLY");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Verify mpls_port struct according to MPLS port addition requirements.
* \par DIRECT INPUT:
*   \param [in] unit       -  Relevant unit.
*   \param [in] mpls_port  -  Holds all the relevant information for the different schemes that might be defined on a VPLS LIF.
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \remark
*   * 'l2vpn_info' is not NULL
*/
static shr_error_e
dnx_mpls_port_add_egress_only_verify(
    int unit,
    bcm_mpls_port_t * mpls_port)
{

    SHR_FUNC_INIT_VARS(unit);

    if (BCM_L3_ITF_TYPE_IS_FEC(mpls_port->egress_tunnel_if))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "mpls_port.egress_tunnel_if(%d) is FEC - not supported",
                     mpls_port->egress_tunnel_if);
    }
    else if (mpls_port->egress_label.l3_intf_id)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "mpls_port.egress_label.l3_intf_id must be 0");
    }

    if (!DNX_MPLS_LABEL_IN_RANGE(unit, mpls_port->egress_label.label))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "mpls_port.egress_label.label(%d) is out of range. The range should be 0 - %d",
                     mpls_port->egress_label.label, DNX_MPLS_LABEL_MAX(unit));
    }

    /** Validate TTL decrement configurations */
    if (!_SHR_IS_FLAG_SET(mpls_port->egress_label.flags, BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flag BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT must be used");
    }

    if (_SHR_IS_FLAG_SET(mpls_port->flags, BCM_MPLS_PORT_ENCAP_WITH_ID) &&
        (BCM_L3_ITF_VAL_GET(mpls_port->encap_id) > (dnx_data_lif.global_lif.nof_global_out_lifs_get(unit) - 1)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "mpls_port->encap_id(0x%x) is invalid", mpls_port->encap_id);
    }
    if ((_SHR_IS_FLAG_SET(mpls_port->flags, BCM_MPLS_PORT_WITH_ID)
         && _SHR_IS_FLAG_SET(mpls_port->flags, BCM_MPLS_PORT_ENCAP_WITH_ID))
        && (BCM_GPORT_SUB_TYPE_LIF_VAL_GET(mpls_port->mpls_port_id) != mpls_port->encap_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "mpls_port_id(0x%x) value != encap_id(0x%x) but BCM_MPLS_PORT_WITH_ID and BCM_MPLS_PORT_ENCAP_WITH_ID are set",
                     mpls_port->mpls_port_id, mpls_port->encap_id);
    }

    /** PWE and MPLS cannot share same EEDB entry */
    if (_SHR_IS_FLAG_SET(mpls_port->egress_tunnel_label.flags, BCM_MPLS_EGRESS_LABEL_ACTION_VALID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "mpls port and mpls tunnel cannot share same entry. two entries should be used");

    }

    if (!_SHR_IS_FLAG_SET(mpls_port->egress_label.flags, BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE) &&
        _SHR_IS_FLAG_SET(mpls_port->egress_label.flags, BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE must be set with BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE");
    }

    /** encap stage */
    if ((mpls_port->egress_label.encap_access != bcmEncapAccessInvalid)
        && (mpls_port->egress_label.encap_access != bcmEncapAccessTunnel1)
        && (mpls_port->egress_label.encap_access != bcmEncapAccessTunnel2)
        && (mpls_port->egress_label.encap_access != bcmEncapAccessNativeArp)
        && (mpls_port->egress_label.encap_access != bcmEncapAccessRif))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "mpls_port->egress_label.encap_access (%d) is invalid, supported values are %d - %d",
                     mpls_port->egress_label.encap_access, bcmEncapAccessInvalid, bcmEncapAccessArp);
    }

    /** remark profile*/
    if (mpls_port->egress_label.qos_map_id != DNX_QOS_INITIAL_MAP_ID
        && !DNX_QOS_MAP_IS_REMARK(mpls_port->egress_label.qos_map_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "qos profile(%d) is not remark profile", mpls_port->egress_label.qos_map_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Verify mpls_port struct and vpn according to MPLS port addition requirements.
* Invoked from bcm_dnx_mpls_port_add().
* \par DIRECT INPUT:
*   \param [in] unit       -  Relevant unit.
*   \param [in] vpn        -  vpn (VSI) value.
*   \param [in] mpls_port  -  Holds all the relevant information for the different schemes that might be defined on a VPLS LIF.
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \see
*   * \ref bcm_dnx_mpls_port_add
*/
static shr_error_e
dnx_mpls_port_add_verify(
    int unit,
    bcm_vpn_t vpn,
    bcm_mpls_port_t * mpls_port)
{
    uint32 supported_flags, supported_flags2;
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(mpls_port, _SHR_E_PARAM, "mpls_port");

    supported_flags =
        BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_SERVICE_TAGGED | BCM_MPLS_PORT_ENTROPY_ENABLE |
        BCM_MPLS_PORT_CONTROL_WORD | BCM_MPLS_PORT_REPLACE | BCM_MPLS_PORT_EGRESS_TUNNEL | BCM_MPLS_PORT_ENCAP_WITH_ID;
    supported_flags2 =
        BCM_MPLS_PORT2_INGRESS_ONLY | BCM_MPLS_PORT2_EGRESS_ONLY |
        BCM_MPLS_PORT2_EGRESS_PROTECTION | BCM_MPLS_PORT2_STAT_ENABLE | BCM_MPLS_PORT2_ALLOC_SYMMETRIC |
        BCM_MPLS_PORT2_TUNNEL_PUSH_INFO | BCM_MPLS_PORT2_LEARN_ENCAP | BCM_MPLS_PORT2_LEARN_ENCAP_EEI |
        BCM_MPLS_PORT2_CROSS_CONNECT | BCM_MPLS_PORT2_PLATFORM_NAMESPACE;

    if ((mpls_port->flags & ~supported_flags) || (mpls_port->flags2 & ~supported_flags2))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported flags are used: flags(0x%x), flags2(0x%x))", mpls_port->flags,
                     mpls_port->flags2);
    }

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    if (_SHR_IS_FLAG_SET(mpls_port->flags2, BCM_MPLS_PORT2_TUNNEL_PUSH_INFO))
    {
        if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_MPLS_PORT2_TUNNEL_PUSH_INFO is noly supported in JR mode");
        }

        SHR_EXIT();
    }

    if (_SHR_IS_FLAG_SET(mpls_port->flags2, BCM_MPLS_PORT2_INGRESS_ONLY)
        && _SHR_IS_FLAG_SET(mpls_port->flags2, BCM_MPLS_PORT2_EGRESS_ONLY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "creation of either an ingress object or an egress object is supported (not both)");
    }
    else if (!_SHR_IS_FLAG_SET(mpls_port->flags2, BCM_MPLS_PORT2_INGRESS_ONLY)
             && !_SHR_IS_FLAG_SET(mpls_port->flags2, BCM_MPLS_PORT2_EGRESS_ONLY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "creation of either an ingress object or an egress object is supported (not none)");
    }

    if (_SHR_IS_FLAG_SET(mpls_port->flags2, BCM_MPLS_PORT2_EGRESS_ONLY))
    {
        if (vpn != 0)
        {
            /** vpn is not used in egress flow thus it must be 0 to avoid confusion */
            SHR_ERR_EXIT(_SHR_E_PARAM, "vpn(%d) must be zero for egress flow", vpn);
        }
        if (_SHR_IS_FLAG_SET(mpls_port->flags2, BCM_MPLS_PORT2_CROSS_CONNECT))
        {
            /** Egress objects don't have a service type (can't be P2P) */
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "BCM_MPLS_PORT2_CROSS_CONNECT (0x%x) should not be set for egress only MPLS Port. "
                         "flags2 = 0x%08x\n", BCM_MPLS_PORT2_CROSS_CONNECT, mpls_port->flags2);
        }
        if (_SHR_IS_FLAG_SET(mpls_port->flags2, BCM_MPLS_PORT2_STAT_ENABLE))
        {
            /** BCM_MPLS_PORT2_STAT_ENABLE flag is used only in ingress */
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "BCM_MPLS_PORT2_STAT_ENABLE flag is used only in ingress. For egress stats use BCM_MPLS_EGRESS_LABEL_STAT_ENABLE inside mpls_port->egress_label struct.");
        }
        if (_SHR_IS_FLAG_SET(mpls_port->flags, BCM_MPLS_PORT_SERVICE_TAGGED))
        {
            /** BCM_MPLS_PORT_SERVICE_TAGGED flag is used only in ingress */
            SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_MPLS_PORT_SERVICE_TAGGED flag is used only in ingress. flags = 0x%08x\n",
                         mpls_port->flags2);
        }
    }
    else
    {
        /** INGRESS */
        if (_SHR_IS_FLAG_SET(mpls_port->flags2, BCM_MPLS_PORT2_CROSS_CONNECT))
        {
            
            if (vpn > 0)
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Cross-Connected MPLS Port with vpn > 0 is not supported. vpn = %d\n",
                             vpn);
            }
        }
        else
        {
            if (vpn == 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "vpn == 0 must have BCM_MPLS_PORT2_CROSS_CONNECT (0x%x) flag set, "
                             "but flags2 = 0x%08x.\n", BCM_MPLS_PORT2_CROSS_CONNECT, mpls_port->flags2);
            }
            SHR_IF_ERR_EXIT(dnx_mpls_port_l2_vpn_check(unit, vpn));
        }
    }

    /** flag enforcement is for backward compatibility only */
    if (!_SHR_IS_FLAG_SET(mpls_port->flags, BCM_MPLS_PORT_EGRESS_TUNNEL))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_MPLS_PORT_EGRESS_TUNNEL must be set");
    }

    if (_SHR_IS_FLAG_SET(mpls_port->flags, BCM_MPLS_PORT_WITH_ID) &&
        (!BCM_GPORT_IS_MPLS_PORT(mpls_port->mpls_port_id) || !BCM_GPORT_SUB_TYPE_IS_LIF(mpls_port->mpls_port_id)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM_MPLS_PORT_WITH_ID set but mpls_port_id(0x%x) not valid, must be encoded as MPLS_PORT and SUB_TYPE_LIF",
                     mpls_port->mpls_port_id);
    }

    /*
     * Split Horizon - Verify network_group_id range:
     */
    if (_SHR_IS_FLAG_SET(mpls_port->flags2, BCM_MPLS_PORT2_INGRESS_ONLY))
    {
        SHR_IF_ERR_EXIT(dnx_switch_network_group_id_verify(unit, TRUE, mpls_port->network_group_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_switch_network_group_id_verify(unit, FALSE, mpls_port->network_group_id));
    }

    /** replace entry */
    if (_SHR_IS_FLAG_SET(mpls_port->flags, BCM_MPLS_PORT_REPLACE)
        && !_SHR_IS_FLAG_SET(mpls_port->flags, BCM_MPLS_PORT_WITH_ID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flag BCM_MPLS_PORT_WITH_ID must be set for replace");
    }

    /** Verify L2 protection information */
    SHR_IF_ERR_EXIT(dnx_failover_l2_protection_info_verify(unit, NULL, mpls_port));

    if (_SHR_IS_FLAG_SET(mpls_port->flags2, BCM_MPLS_PORT2_INGRESS_ONLY))
    {
        SHR_IF_ERR_EXIT(dnx_mpls_port_add_ingress_only_verify(unit, mpls_port));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_mpls_port_add_egress_only_verify(unit, mpls_port));
    }

    /** Symmetric allocation flag cannot be used with replace */
    if (_SHR_IS_FLAG_SET(mpls_port->flags2, BCM_MPLS_PORT2_ALLOC_SYMMETRIC))
    {
        if (_SHR_IS_FLAG_SET(mpls_port->flags, BCM_MPLS_PORT_REPLACE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "BCM_MPLS_PORT2_ALLOC_SYMMETRIC and BCM_MPLS_PORT_REPLACE cannot be used together");
        }
    }

    /*
     * Verify qos model.
     */
    if (_SHR_IS_FLAG_SET(mpls_port->flags2, BCM_MPLS_PORT2_INGRESS_ONLY))
    {
        if ((mpls_port->ingress_qos_model.ingress_remark > bcmQosIngressModelUniform) ||
            (mpls_port->ingress_qos_model.ingress_phb > bcmQosIngressModelUniform) ||
            (mpls_port->ingress_qos_model.ingress_ttl > bcmQosIngressModelUniform))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Available Ingress qos model for remark(%d), phb(%d), ttl(%d) are Invalid(SDK default), Pipe, ShortPipe and Uniform.",
                         mpls_port->ingress_qos_model.ingress_remark, mpls_port->ingress_qos_model.ingress_phb,
                         mpls_port->ingress_qos_model.ingress_ttl);
        }
        /*
         * QOS ECN model
         */
        if (mpls_port->ingress_qos_model.ingress_ecn == bcmQosIngressEcnModelEligible)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "MPLS port does not support ECN eligible");
        }
    }
    else
    {
        /** Egress TTL model*/
        if ((mpls_port->egress_label.egress_qos_model.egress_ttl != bcmQosEgressModelPipeMyNameSpace) &&
            (mpls_port->egress_label.egress_qos_model.egress_ttl != bcmQosEgressModelUniform))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "TTL model only support bcmQosEgressModelPipeMyNameSpace and bcmQosEgressModelUniform.");
        }
        if (mpls_port->egress_label.egress_qos_model.egress_ttl != bcmQosEgressModelPipeMyNameSpace)
        {
            if (mpls_port->egress_label.ttl != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "TTL must be 0 if ttl model is not pipe");
            }
        }

        /** Egress EXP model */
        if ((mpls_port->egress_label.egress_qos_model.egress_qos != bcmQosEgressModelPipeNextNameSpace) &&
            (mpls_port->egress_label.egress_qos_model.egress_qos != bcmQosEgressModelPipeMyNameSpace))
        {
            if (mpls_port->egress_label.exp != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "EXP must be 0 if QoS model is not bcmQosEgressModelPipeNextNameSpace or bcmQosEgressModelPipeMyNameSpace.");
            }
        }
    }

    if (system_headers_mode != DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
        if (_SHR_IS_FLAG_SET(mpls_port->flags2, BCM_MPLS_PORT2_TUNNEL_PUSH_INFO))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_MPLS_PORT2_TUNNEL_PUSH_INFO is noly supported in JR mode");
        }

        if (_SHR_IS_FLAG_SET(mpls_port->flags2, BCM_MPLS_PORT2_LEARN_ENCAP))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_MPLS_PORT2_LEARN_ENCAP is noly supported in JR mode");
        }

        if (_SHR_IS_FLAG_SET(mpls_port->flags2, BCM_MPLS_PORT2_LEARN_ENCAP_EEI))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_MPLS_PORT2_LEARN_ENCAP_EEI is noly supported in JR mode");
        }

        if (_SHR_IS_FLAG_SET(mpls_port->flags2, BCM_MPLS_PORT2_INGRESS_ONLY))
        {
            if (mpls_port->egress_label.label != BCM_MPLS_LABEL_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "In JR2 mode, If create ingress mpls port, egress_label.label (%d) must be INVALID",
                             mpls_port->egress_label.label);
            }
        }
    }
    else
    {
        /*
         * Jericho mode beginning {
         */
        if (_SHR_IS_FLAG_SET(mpls_port->flags2, BCM_MPLS_PORT2_INGRESS_ONLY)
            && _SHR_IS_FLAG_SET(mpls_port->flags2, BCM_MPLS_PORT2_LEARN_ENCAP_EEI | BCM_MPLS_PORT2_LEARN_ENCAP))
        {
            if (mpls_port->egress_label.label != BCM_MPLS_LABEL_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "When using BCM_MPLS_PORT2_LEARN_ENCAP learning info in ingress only, we learn the encap id, not the VC label");
            }

            if (!BCM_L3_ITF_TYPE_IS_FEC(mpls_port->egress_tunnel_if))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "BCM_MPLS_PORT2_LEARN_ENCAP flag is supported only when mpls_port->egress_tunnel_if is FEC");
            }
        }

        if (_SHR_IS_FLAG_SET(mpls_port->flags2, BCM_MPLS_PORT2_INGRESS_ONLY)
            && !_SHR_IS_FLAG_SET(mpls_port->flags2, BCM_MPLS_PORT2_LEARN_ENCAP)
            && !_SHR_IS_FLAG_SET(mpls_port->flags2, BCM_MPLS_PORT2_LEARN_ENCAP_EEI))
        {
            if ((mpls_port->egress_label.label != BCM_MPLS_LABEL_INVALID) &&
                !BCM_L3_ITF_TYPE_IS_FEC(mpls_port->egress_tunnel_if))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "When learning info include VC label, mpls_port->egress_tunnel_if must be FEC");
            }
        }
        /*
         * Jericho mode end }
         */
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Verify mpls_port struct and vpn according to MPLS port lookup requirements.
* Invoked from bcm_dnx_mpls_port_get().
* \par DIRECT INPUT:
*   \param [in] unit       -  Relevant unit.
*   \param [in] vpn        -  vpn (VSI) value.
*   \param [in] mpls_port  -  Holds all the relevant information for the different schemes that might be defined on a VPLS LIF.
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \see
*   * \ref bcm_dnx_mpls_port_get
*/
static shr_error_e
dnx_mpls_port_get_verify(
    int unit,
    bcm_vpn_t vpn,
    bcm_mpls_port_t * mpls_port)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(mpls_port, _SHR_E_PARAM, "mpls_port");

    SHR_IF_ERR_EXIT(dnx_mpls_port_vpn_port_verify(unit, vpn, mpls_port->mpls_port_id));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Verify mpls_port struct and vpn according to MPLS port lookup requirements.
* Invoked from bcm_dnx_mpls_port_delete().
* \par DIRECT INPUT:
*   \param [in] unit       -  Relevant unit.
*   \param [in] vpn        -  vpn (VSI) value (ignored if 0).
*   \param [in] mpls_port_id - ID of the mpls port to delete
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \see
*   * \ref bcm_dnx_mpls_port_delete
*/
static shr_error_e
dnx_mpls_port_delete_verify(
    int unit,
    bcm_vpn_t vpn,
    bcm_gport_t mpls_port_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_mpls_port_vpn_port_verify(unit, vpn, mpls_port_id));

    if (vpn && (BCM_GPORT_SUB_TYPE_LIF_EXC_GET(mpls_port_id) == BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY))
    {
        /*
         * Ingress. Verify that the lif is part of the vpn.
         */
        bcm_mpls_port_t mpls_port;
        bcm_mpls_port_t_init(&mpls_port);
        mpls_port.mpls_port_id = mpls_port_id;
        /** Will fail if mpls_port_id is not associated with vpn */
        SHR_IF_ERR_EXIT(bcm_dnx_mpls_port_get(unit, vpn, &mpls_port));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Verify traverse additional info struct according to MPLS port traverse requirements.
* Invoked from bcm_dnx_mpls_port_traverse().
* \par DIRECT INPUT:
*   \param [in] unit       -  Relevant unit.
*   \param [in] additional_info  -  Holds all the relevant information for traverse action.
*   \param [in] trav_fn - The function to call with every object that is returned.
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \see
*   * \ref bcm_dnx_mpls_port_traverse
*/
static shr_error_e
dnx_mpls_port_traverse_verify(
    int unit,
    bcm_mpls_port_traverse_info_t additional_info,
    bcm_mpls_port_traverse_cb trav_fn)
{
    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(additional_info.traverse_flags, BCM_MPLS_TRAVERSE_DELETE))
    {
        if (trav_fn != NULL)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "trav_fn should be NULL when delete mpls entries.");
        }
    }
    else
    {
        SHR_NULL_CHECK(trav_fn, _SHR_E_PARAM, "trav_fn");
    }

    if (_SHR_IS_FLAG_SET(additional_info.flags2, BCM_MPLS_PORT2_INGRESS_ONLY) &&
        _SHR_IS_FLAG_SET(additional_info.flags2, BCM_MPLS_PORT2_EGRESS_ONLY))
    {
        /*
         * Ingress only and Egress only cannot be set together
         */
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM_MPLS_PORT2_INGRESS_ONLY and BCM_MPLS_PORT2_EGRESS_ONLY cannot be set together.To traverse all entries, do not use any of those flags");
    }
    else if ((additional_info.vpn != 0) && _SHR_IS_FLAG_SET(additional_info.flags2, BCM_MPLS_PORT2_EGRESS_ONLY))
    {
        /*
         * vpn is usable for Ingress traverse only, so cannot be set with Egress only.
         */
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "vpn not equal 0 affect INGRESS entries, cannot be set with BCM_MPLS_PORT2_EGRESS_ONLY");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* retrive gport from local lif (both ingress and egress), getting the MPLS PORT entry of the gport
* and invoke the callbak, with the retreived entry.
* Invoked from bcm_dnx_mpls_port_traverse().
* \par DIRECT INPUT:
*   \param [in] unit       -  Relevant unit.
*   \param [in] local_lif  -  Local Lif (out or in Lif)
*   \param [in] gport_flags  -  Flags for gport APIs (distinguish between in and out lif).
*   \param [in] trav_fn  -  callback function to invoke.
*   \param [in] user_data  -  user additional data to callback call.
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \see
*   * \ref bcm_dnx_mpls_port_traverse
*/
static shr_error_e
dnx_mpls_port_from_local_lif_get_with_callback(
    int unit,
    uint32 local_lif,
    uint32 gport_flags,
    bcm_mpls_port_traverse_cb trav_fn,
    void *user_data)
{
    bcm_mpls_port_t mpls_port;
    SHR_FUNC_INIT_VARS(unit);

    bcm_mpls_port_t_init(&mpls_port);
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit, gport_flags, _SHR_CORE_ALL, (int) local_lif,
                                                &mpls_port.mpls_port_id));
    if (trav_fn != NULL)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_mpls_port_get(unit, 0, &mpls_port));
        SHR_IF_ERR_EXIT(trav_fn(unit, &mpls_port, user_data));
    }
    else
    {
        SHR_IF_ERR_EXIT(bcm_dnx_mpls_port_delete(unit, 0, mpls_port.mpls_port_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize and populate a mpls_l2vpn object from a mpls_port input
 * \param [in] unit - Unit ID
 * \param [in] vpn - VPN/VSI ID
 * \param [in] mpls_port - API input object
 * \param [out] l2vpn - pointer to allocated mpls_l2vpn object to initialize and fill.
 * \return Standard error handling
 */
static shr_error_e
dnx_mpls_port_to_l2vpn_translate(
    int unit,
    bcm_vpn_t vpn,
    bcm_mpls_port_t * mpls_port,
    dnx_mpls_l2vpn_info_t * l2vpn)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(l2vpn, _SHR_E_PARAM, "l2vpn");

    dnx_mpls_l2vpn_info_t_init(unit, l2vpn, MPLS_L2VPN_TYPE_MPLS_PORT, mpls_port);
    l2vpn->vpn = vpn;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   create/update mpls push profile,only relevant in JR mode.
 * \param [in] unit - Unit ID
 * \param [in] mpls_port - API input object
 * \return Standard error handling
 */
static shr_error_e
dnx_mpls_port_push_profile_add(
    int unit,
    bcm_mpls_port_t * mpls_port)
{
    int push_cmd;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Jericho mode beginning
     * {
     */
    /*
     * For sub-type of push-profile, only alloc the push profile.
     * This is used to synchronize the push profile ID  between
     * multiple divice.
     */

    if (BCM_GPORT_SUB_TYPE_IS_MPLS_PUSH_PROFILE(mpls_port->mpls_port_id))
    {
        push_cmd = BCM_GPORT_SUB_TYPE_MPLS_PUSH_PROFILE_GET(mpls_port->mpls_port_id);
    }
    else
    {
        push_cmd = mpls_port->mpls_port_id;
    }

    /*
     * In JR push profile values 8,9,0 are reserved for swap, pop and default commands.
     */
    if (DNX_MPLS_IS_RESERVED_PUSH_PROFILE(push_cmd))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Push command values 8,9 are reserved for swap and pop actions. Value 0 is not in usage.");
    }

    SHR_IF_ERR_EXIT(dnx_mpls_tunnel_create_push_entry_from_eei(unit, &(mpls_port->egress_label),
                                                               _SHR_IS_FLAG_SET(mpls_port->flags,
                                                                                BCM_MPLS_PORT_CONTROL_WORD),
                                                               _SHR_IS_FLAG_SET(mpls_port->flags,
                                                                                BCM_MPLS_PORT_REPLACE), &push_cmd));
    BCM_GPORT_SUB_TYPE_MPLS_PUSH_PROFILE_SET(mpls_port->mpls_port_id, push_cmd);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port->mpls_port_id, mpls_port->mpls_port_id);
    /*
     * Jericho mode end
     * }
     */

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   delete mpls push profile,only relevant in JR mode.
 * \param [in] unit - Unit ID
 * \param [in] mpls_port_id - API input object
 * \return Standard error handling
 */
static shr_error_e
dnx_mpls_port_push_profile_delete(
    int unit,
    bcm_gport_t mpls_port_id)
{
    int push_cmd;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Jericho mode beginning
     * {
     */
    /*
     * For sub-type of push-profile, only free the push profile.
     * This is used to synchronize the push profile ID  between
     * multiple divice.
     */

    push_cmd = BCM_GPORT_SUB_TYPE_MPLS_PUSH_PROFILE_GET(mpls_port_id);

    /*
     * In JR push profile values 8,9,0 are reserved for swap, pop and default commands.
     */
    if (DNX_MPLS_IS_RESERVED_PUSH_PROFILE(push_cmd))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Push command values 8,9 are reserved for swap and pop actions. Value 0 is not in usage.");
    }

    SHR_IF_ERR_EXIT(dnx_mpls_tunnel_delete_push_entry_from_eei(unit, push_cmd));

    /*
     * Jericho mode end
     * }
     */

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   get mpls push profile, only relevant in JR mode.
 * \param [in] unit - Unit ID
 * \param [in] mpls_port - API input object
 * \return Standard error handling
 */
static shr_error_e
dnx_mpls_port_push_profile_get(
    int unit,
    bcm_mpls_port_t * mpls_port)
{
    int push_cmd;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Jericho mode beginning
     * {
     */
    /*
     * For sub-type of push-profile, only get the push profile.
     * This is used to synchronize the push profile ID  between
     * multiple divice.
     */

    push_cmd = BCM_GPORT_SUB_TYPE_MPLS_PUSH_PROFILE_GET(mpls_port->mpls_port_id);

    /*
     * In JR push profile values 8,9,0 are reserved for swap, pop and default commands.
     */
    if (DNX_MPLS_IS_RESERVED_PUSH_PROFILE(push_cmd))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Push command values 8,9 are reserved for swap and pop actions. Value 0 is not in usage.");
    }

    SHR_IF_ERR_EXIT(dnx_push_profile_to_mpls_label_info(unit, push_cmd, mpls_port));

    mpls_port->flags2 |= BCM_MPLS_PORT2_TUNNEL_PUSH_INFO;

    /*
     * Jericho mode end
     * }
     */

exit:
    SHR_FUNC_EXIT;
}

/*
 * APIs
 * {
 */

/**
* \brief
* This api creates the mpls port object described above with its ingress and egress objects.
* Ingress and egress objects can not be created at the same API call.
*   \param [in] unit       -  device unit number.
*   \param [in] vpn        -  vpn (VSI) value. For Multipoint configuration, this value needs to be different than 0 and not bigger than number of VSIs. For Point to point, this value should be 0.
*   \param [in,out] mpls_port - Main structre of the API. Holds all the relevant information for the different schemes that might be defined on a VPLS LIF.
*           egress flow:
*               mpls_port.mpls_port_id - global lif of the VPLS tunnel, encoded as BCM_GPORT_MPLS_PORT and BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY sub type.
*               mpls_port.encap_id - outlif id.
*               mpls_port.egress_label -holding all information relevant for the (encapsulated) PWE label.
*                                                   detailed information can be found at 'bcm_dnx_mpls_tunnel_initiator_create'
*               mpls_port.egress_tunnel_if - next Global-LIF ID, can be ARP, MPLS or none. encoded as BCM_L3_ITF_TYPE_LIF.
*               mpls_port.egress_failover_port_id - Set to 1 in case PWE object is the primary object.
*               mpls_port.egress_failover_id - The failover protection pointer
*               mpls_port.network_group_id - Network Group ID, used for orientation filtering.
*               mpls_port.flags -
*                   BCM_MPLS_PORT_EGRESS_TUNNEL - mandatory
*                   BCM_MPLS_PORT_WITH_ID - use given global lif in 'mpls_port.mpls_port_id'
*                   BCM_MPLS_PORT_ENCAP_WITH_ID - use given global lif in 'mpls_port.encap_id'
*                   BCM_MPLS_PORT_REPLACE - replace preconfigured VPLS object of 'mpls_port.mpls_port_id'
*                   BCM_MPLS_PORT_CONTROL_WORD - adds a control word above this PWE tunnel
*               mpls_port.flags2 -
*                   BCM_MPLS_PORT2_EGRESS_ONLY - indicates egress flow
*                   BCM_MPLS_PORT2_ALLOC_SYMMETRIC - validate 'mpls_port.mpls_port_id' ingress global lif is reserved.
*                   BCM_MPLS_PORT2_STAT_ENABLE - Indicates statistics enabled
*                   BCM_MPLS_PORT2_EGRESS_PROTECTION - pretection enabled
*
*           ingress flow:
*               mpls_port.mpls_port_id - global lif of the VPLS tunnel, encoded as BCM_GPORT_MPLS_PORT and BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY sub type.
*               mpls_port.criteria - indicate Match criteria is Label only (BCM_MPLS_PORT_MATCH_LABEL)
*               mpls_port.match_label - this is the terminated label that is associated with this PWE object.
*               mpls_port.qos_map_id - must be 0
*               mpls_port.ingress_qos_model - EXP and TTL attributes
*               mpls_port.vccv_type - set with bcmMplsPortControlChannelTtl to trap a packet with TTL = 0 / 1
*               mpls_port.ingress_failover_port_id - set to 1 in case PWE object is the primary object.
*               mpls_port.ingress_failover_id - the failvoer protection pointer.
*               mpls_port.network_group_id - network group ID. Orientation for this PWE object
*               mpls_port.encap_id - the Egress PWE Global-LIF - used for learning
*               mpls_port.egress_tunnel_if - the MPLS FEC ID - used for learning, encoded as BCM_L3_ITF_TYPE_FEC.
*               mpls_port.port - the physical port destination - used for learning
*               mpls_port.failover_mc_group - the multicast group that was configured as part of a 1+1 protection scheme - used for learning
*               mpls_port.failover_port_id - the PWE FEC-ID that was configured as part of a 1:1 protection scheme - used for learning
*               mpls_port.flags -
*                   BCM_MPLS_PORT_EGRESS_TUNNEL - mandatory
*                   BCM_MPLS_PORT_WITH_ID - use given global lif in 'mpls_port.mpls_port_id'
*                   BCM_MPLS_PORT_REPLACE - replace preconfigured VPLS object of 'mpls_port.mpls_port_id'
*                   BCM_MPLS_PORT_ENTROPY_ENABLE - entropy label is expected above this PWE label, with no Entropy label indication (ELI) label preceding it
*                   BCM_MPLS_PORT_CONTROL_WORD - control word is expected above this PWE label
*               mpls_port.flags2 -
*                   BCM_MPLS_PORT2_INGRESS_ONLY - indicates ingress flow
*                   BCM_MPLS_PORT2_CROSS_CONNECT - indicate this PWE object is used Point-to-Point (P2P)
*                   BCM_MPLS_PORT2_STAT_ENABLE - Indicates statistics enabled
*
*           push profile (JR1 mode):
*               mpls_port.mpls_port_id - push profile id
*               mpls_port.egress_label -holding all information relevant for the pushed label.
*                                                   detailed information can be found at 'bcm_dnx_mpls_tunnel_initiator_create'
*               mpls_port.flags -
*                   BCM_MPLS_PORT_REPLACE - replace entry
*                   BCM_MPLS_PORT_CONTROL_WORD - include control word
*                   BCM_MPLS_PORT_WITH_ID - use given global lif in 'mpls_port.mpls_port_id'
*               mpls_port.flags2 -
*                   BCM_MPLS_PORT2_TUNNEL_PUSH_INFO - configure push profile
* \remark
*   * Egress: It is not possible to set encap_access phase during replace (as it changes EEDB OutLIF location)
* \see
*   * None
*/
int
bcm_dnx_mpls_port_add(
    int unit,
    bcm_vpn_t vpn,
    bcm_mpls_port_t * mpls_port)
{
    dnx_mpls_l2vpn_info_t mpls_l2vpn;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /*
     * BCM level verification
     */
    SHR_INVOKE_VERIFY_DNX(dnx_mpls_port_add_verify(unit, vpn, mpls_port));

    if (_SHR_IS_FLAG_SET(mpls_port->flags2, BCM_MPLS_PORT2_TUNNEL_PUSH_INFO))
    {
        SHR_IF_ERR_EXIT(dnx_mpls_port_push_profile_add(unit, mpls_port));
        SHR_EXIT();
    }
    else
    {
        /*
         * Send to MPLS L2VPN module
         */
        SHR_IF_ERR_EXIT(dnx_mpls_port_to_l2vpn_translate(unit, vpn, mpls_port, &mpls_l2vpn));
        SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_add(unit, &mpls_l2vpn));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

int
bcm_dnx_mpls_port_delete(
    int unit,
    bcm_vpn_t vpn,
    bcm_gport_t mpls_port_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /*
     * BCM level verification
     */
    SHR_INVOKE_VERIFY_DNX(dnx_mpls_port_delete_verify(unit, vpn, mpls_port_id));

    if (BCM_GPORT_SUB_TYPE_IS_MPLS_PUSH_PROFILE(mpls_port_id))
    {
        SHR_IF_ERR_EXIT(dnx_mpls_port_push_profile_delete(unit, mpls_port_id));
        SHR_EXIT();
    }
    else
    {
        /*
         * Send to MPLS L2VPN module
         */
        SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_delete
                        (unit, MPLS_L2VPN_TYPE_MPLS_PORT,
                         (_SHR_GPORT_SUB_TYPE_LIF_EXC_GET(mpls_port_id) == _SHR_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY),
                         mpls_port_id));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
* This api gets the mpls port object described above with its ingress and egress objects.
* Ingress and egress objects can not be fetched at the same API call.
*   \param [in] unit       -  device unit number.
*   \param [in] vpn        -  vpn (VSI) value. For Multipoint configuration, this value needs to be different than 0 and not bigger than number of VSIs. For Point to point, this value should be 0.
*   \param [in] mpls_port - Main struct of the API holding the retrieved information.
*               [inout] mpls_port.mpls_port_id - Id (gport) for both ingress and egress objects. It is a global lif encoded as mpls port.
*                                                 type BCM_GPORT_SUB_TYPE_MPLS_PUSH_PROFILE means operation on mpls push profile
* \return
*   int - non-zero upon error
* \remark
*   full list of 'mpls_port' parameters is in 'bcm_dnx_mpls_port_add'
* \see
*   * None
*/
int
bcm_dnx_mpls_port_get(
    int unit,
    bcm_vpn_t vpn,
    bcm_mpls_port_t * mpls_port)
{
    dnx_mpls_l2vpn_info_t mpls_l2vpn;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * BCM level verification
     */
    SHR_INVOKE_VERIFY_DNX(dnx_mpls_port_get_verify(unit, vpn, mpls_port));

    if (BCM_GPORT_SUB_TYPE_IS_MPLS_PUSH_PROFILE(mpls_port->mpls_port_id))
    {
        SHR_IF_ERR_EXIT(dnx_mpls_port_push_profile_get(unit, mpls_port));
        SHR_EXIT();
    }
    else
    {
        /*
         * Send to MPLS L2VPN module
         */
        SHR_IF_ERR_EXIT(dnx_mpls_port_to_l2vpn_translate(unit, vpn, mpls_port, &mpls_l2vpn));
        if (_SHR_GPORT_SUB_TYPE_LIF_EXC_GET(mpls_port->mpls_port_id) == _SHR_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY)
        {
            mpls_port->flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY;
        }
        else
        {
            mpls_port->flags2 |= BCM_MPLS_PORT2_EGRESS_ONLY;
        }
        SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_get(unit, &mpls_l2vpn));
        if (vpn && (vpn != mpls_l2vpn.vpn))
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "GPort 0x%08x is not associated with vpn %d.\n", mpls_port->mpls_port_id,
                         vpn);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Traversing MPLS PORT object, according to given rules. \n
 * Each returned object is called with the given trav_fn, the traverse callback. \n
 *
 * \param [in] unit -
 *   Relevant unit.
 * \param [in] additional_info - Additional info which can translate to traverse rules \n
 *   flags: not supported\n
 *   flags2: \n
 *        BCM_MPLS_PORT2_INGRESS_ONLY - Traverse Ingress objects only \n
 *        BCM_MPLS_PORT2_EGRESS_ONLY - Traverse Egress objects only \n
 *        Note: For travarsing all entries, non of the above flags should be set
 *   vpn: vpn to iterate (VSI), valid for Ingress entries only.\n
 * \param [in] trav_fn - The function to call with every object that is returned \n
 * \param [in] user_data - additional data to pass to the trav_fn, along with the returned object \n
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 * \remark
 * \see
 *   * None
 */
int
bcm_dnx_mpls_port_traverse(
    int unit,
    bcm_mpls_port_traverse_info_t additional_info,
    bcm_mpls_port_traverse_cb trav_fn,
    void *user_data)
{
    uint8 is_ingress = FALSE, is_egress = FALSE;
    int is_end = 0;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    /*
     * Verify
     */
    SHR_INVOKE_VERIFY_DNX(dnx_mpls_port_traverse_verify(unit, additional_info, trav_fn));

    /*
     * Set ingress and/or egress indications
     */
    if (_SHR_IS_FLAG_SET(additional_info.flags2, BCM_MPLS_PORT2_INGRESS_ONLY))
    {
        is_ingress = TRUE;
    }
    else if (_SHR_IS_FLAG_SET(additional_info.flags2, BCM_MPLS_PORT2_EGRESS_ONLY))
    {
        is_egress = TRUE;
    }
    else
    {
        is_ingress = TRUE;
        is_egress = TRUE;
    }

    /*
     * If ingress:
     * Iterate the IN_LIF PWE table (optional Rule: according to vpn)
     * Translate to gport and call the mpls port get API
     * Call the cb.
     */
    if (is_ingress)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_LIF_FORMAT_PWE, &entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

        /*
         * Add a vpn rule if != 0.
         * vpn rule is set on VSI field
         */
        if (additional_info.vpn != 0)
        {
            uint32 vsi = (uint32) additional_info.vpn;
            SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                            (unit, entry_handle_id, DBAL_FIELD_VSI, 0, DBAL_CONDITION_EQUAL_TO, &vsi, NULL));
        }

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {
            uint32 local_lif = 0;
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_IN_LIF, &local_lif));
            SHR_IF_ERR_EXIT(dnx_mpls_port_from_local_lif_get_with_callback
                            (unit, local_lif, DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS, trav_fn,
                             user_data));
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
    }

    /*
     * If egress:
     * Iterate the EEDB PWE table (No Rules)
     * Translate to gport and call the mpls port get API
     * Call the cb.
     */

    if (is_egress)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_PWE, &entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {
            uint32 local_lif = 0;
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_OUT_LIF, &local_lif));
            SHR_IF_ERR_EXIT(dnx_mpls_port_from_local_lif_get_with_callback
                            (unit, local_lif, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, trav_fn, user_data));
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
    }

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}
/*
 * End of APIs
 * }
 */
