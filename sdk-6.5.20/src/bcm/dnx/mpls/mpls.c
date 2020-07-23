/** \file mpls.c General MPLS functionality for DNX.
 *      Dedicated set of MPLS APIs are distributed between mpls_*.c
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
#include <shared/utilex/utilex_bitstream.h>
#include <bcm/vlan.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/vsi/vsi.h>
#include <bcm/mpls.h>
#include <bcm_int/dnx/mpls/mpls_range.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <src/bcm/dnx/mpls/mpls_ilm.h>
#include <bcm_int/dnx/mpls/mpls_tunnel_term.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mpls.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_qos.h>
#include <bcm_int/dnx/mpls/mpls.h>
#include <src/bcm/dnx/qos/qos_internal.h>
#include <src/bcm/dnx/mpls_port/mpls_port.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm_int/dnx/failover/failover.h>
#include <soc/sand/sand_aux_access.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/dnx/stat/stat_pp.h>
#include "mpls_evpn.h"
#include <bcm_int/dnx/vsi/vswitch_vpn.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_port_pp_access.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>
#include <bcm_int/dnx/l2/l2.h>
#include <bcm_int/dnx/field/field_entry.h>

/*
 * }
 */

/*
 * MACROs
 * {
 */

/** IP protocol identification nibble */
#define MPLS_FIRST_NIBBLE_PROTOCOL_IPV4 4
#define MPLS_FIRST_NIBBLE_PROTOCOL_IPV6 6

/** bit set/cleared, used for 1 bit fields */
#define MPLS_BIT_SET 1
#define MPLS_BIT_CLEAR 0

/** lsb nibble of mpls label indicating known special label. */
#define MPLS_LSB_NIBBLE_IPV4_EXPLICIT_NULL 0
#define MPLS_LSB_NIBBLE_RA 1
#define MPLS_LSB_NIBBLE_IPV6_EXPLICIT_NULL 2
#define MPLS_LSB_NIBBLE_ELI 7
#define MPLS_LSB_NIBBLE_OAM_ALERT 14
#define MPLS_LSB_NIBBLE_RSV 15

#define NUM_SWAP_COMMANDS 16

/** number of special labels to terminate */
#define TERMINATE_ONE_SPECIAL_LABEL DBAL_ENUM_FVAL_LABELS_TO_TERMINATE_ONE_LABEL - 1
#define TERMINATE_TWO_SPECIAL_LABEL DBAL_ENUM_FVAL_LABELS_TO_TERMINATE_TWO_LABELS - 1

/*EEI format under Jericho system header mode
 * EEI[15:0]:Out-LIF[15:0]
 * EEI[21:20]: Out-LIF[17:16]
 */
#define JR_MODE_EEI_ENCAP_PTR_NOF_BITS_MSB                   (2)
#define JR_MODE_EEI_ENCAP_PTR_NOF_BITS_LSB                   (16)
#define JR_MODE_EEI_ENCAP_PTR_NOF_BITS                       (JR_MODE_EEI_ENCAP_PTR_NOF_BITS_MSB+JR_MODE_EEI_ENCAP_PTR_NOF_BITS_LSB)

typedef struct
{
    /*
     *  Label
     */
    uint32 label_nibble;
    /*
     * Property
     */
    uint32 label_profile;
} special_label_property_t;
/*
 * }
 */

/*
 * ENUMs
 * {
 */
/*
 * MPLS pop model used for PHP through FHEI
 * pop model has 2bit, lsb used to qos, msb used to TTL
 */
#define TTL_MODEL_OFFSET 0x1

static const special_label_property_t special_label_property[] = {
  /** Label,                             label_profile*/
    {MPLS_LSB_NIBBLE_IPV4_EXPLICIT_NULL, DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_IPV4_EXPLICIT_NULL},
    {MPLS_LSB_NIBBLE_RA, DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_ROUTER_ALERT},
    {MPLS_LSB_NIBBLE_IPV6_EXPLICIT_NULL, DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_IPV6_EXPLICIT_NULL},
    {MPLS_LSB_NIBBLE_ELI, DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_ELI},
    {MPLS_LSB_NIBBLE_OAM_ALERT, DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_OAM_ALERT}
};

/*
 * }
 */

/*
 * APIs
 * {
 */

/**
* \brief
*  In case tunnel is defined by special match {label, label or lif}, validate second key.
*   \param [in] unit  -  Relevant unit.
*   \param [in] info  -  A pointer to a struct containing relevant information whether additional label or interface should be validated.
* \return
*   shr_error_e - Non-zero in case of an error.
*/
static shr_error_e
dnx_mpls_tunnel_switch_verify_2nd_key(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    bcm_mpls_label_t *in_labels_p = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL))
    {
        /*
         * Enter if a second label lookup is requested
         */
        int num_labels = 1;
        int is_iml;
        bcm_mpls_label_t found_labels_low, found_labels_high;

        SHR_ALLOC(in_labels_p, sizeof(*in_labels_p) * num_labels, "in_labels_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
        /*
         * Verify label is within legitimate range of labels.
         */
        if (!DNX_MPLS_LABEL_IN_RANGE(unit, info->second_label))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Second MPLS label (%d) is out of range. The range should be 0 - %d",
                         info->second_label, DNX_MPLS_LABEL_MAX(unit));
        }
        /*
         * Verify label is not in existing IML range
         */
        /*
         * The following two lines are not required but they keep the compiler happy.
         */
        is_iml = FALSE;
        found_labels_low = found_labels_high = 0;
        in_labels_p[0] = info->second_label;
        SHR_IF_ERR_EXIT(dnx_mpls_label_iml_range_check
                        (unit, num_labels, in_labels_p, &is_iml, &found_labels_low, &found_labels_high));
        if (is_iml != FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "specified label (%d) is within an IML range which is already in HW (%d,%d). Quit.\r\n",
                         in_labels_p[0], found_labels_low, found_labels_high);
        }
    }

    if (_SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL) &&
        _SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_LOOKUP_L3_INGRESS_INTF))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Can't simultaneously match context label and context interface. Flags: "
                     "BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL and BCM_MPLS_SWITCH_LOOKUP_L3_INGRESS_INTF "
                     "cannot both be set.");
    }

    if (_SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL) &&
        (info->action == BCM_MPLS_SWITCH_ACTION_SWAP))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "SWAP action is not supported when BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL is set.");
    }

    if (_SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_LOOKUP_L3_INGRESS_INTF))
    {
        /** If a lookup by L3 intf is required, make sure the intf is valid. */
        if (!(BCM_L3_ITF_TYPE_IS_LIF(info->ingress_if) || BCM_L3_ITF_TYPE_IS_RIF(info->ingress_if)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Supplied ingress interface is neither LIF nor RIF.");
        }
        if (BCM_L3_ITF_VAL_GET(info->ingress_if) == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Supplied ingress interface must be greater than 0 when BCM_MPLS_SWITCH_LOOKUP_L3_INGRESS_INTF is set.");
        }
    }

exit:
    SHR_FREE(in_labels_p);
    SHR_FUNC_EXIT;

}

/**
* \brief
*  Used by bcm_dnx_mpls_tunnel_switch_create to verify info struct.
* \par DIRECT INPUT:
*   \param [in] unit  -  Relevant unit.
*   \param [in] info  -  A pointer to a struct containing relevant information for ilm configuration.
* \par INDIRECT INPUT:
*   * \b *info \n
*     See 'info' in DIRECT INPUT above
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_mpls_tunnel_switch_create
*/
static shr_error_e
dnx_mpls_tunnel_switch_create_verify(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    uint32 not_supported_flags, not_supported_flags2;
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    not_supported_flags = (BCM_MPLS_SWITCH_COUNTED | BCM_MPLS_SWITCH_INT_PRI_SET |
                           BCM_MPLS_SWITCH_INT_PRI_MAP | BCM_MPLS_SWITCH_COLOR_MAP | BCM_MPLS_SWITCH_DROP |
                           
                           BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL |
                           BCM_MPLS_SWITCH_INNER_EXP | BCM_MPLS_SWITCH_INNER_TTL |
                           BCM_MPLS_SWITCH_SKIP_ETHERNET | BCM_MPLS_SWITCH_LOOKUP_INT_PRI | BCM_MPLS_SWITCH_FRR |
                           BCM_MPLS_SWITCH_DIRECT_ATTACHED |
                           BCM_MPLS_SWITCH_WIDE | BCM_MPLS_SWITCH_LOOKUP_NONE |
                           BCM_MPLS_SWITCH_INGRESS_ECN_MAP | BCM_MPLS_SWITCH_TUNNEL_TERM_ECN_MAP);
    if (!DNX_MPLS_SWITCH_IS_EVPN(info) &&
        (_SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_ENTROPY_ENABLE) ||
         _SHR_IS_FLAG_SET(info->flags2, BCM_MPLS_SWITCH2_CONTROL_WORD)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Flow label and Control word are not supported if not EVPN - flags = 0x%08X, flags2 = 0x%08X",
                     info->flags, info->flags2);
    }

    if (_SHR_IS_FLAG_SET(info->flags, not_supported_flags))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "one of the used flags is not supported - flags = 0x%08X", info->flags);
    }

    not_supported_flags2 = (BCM_MPLS_SWITCH2_ENABLE_IPV4 | BCM_MPLS_SWITCH2_ENABLE_IPV6 |
                            BCM_MPLS_SWITCH2_ENABLE_ROE | BCM_MPLS_SWITCH2_ENABLE_ROE_CUSTOM |
                            BCM_MPLS_SWITCH2_TUNNEL_HEADER_EGRESS_VLAN_TRANSLATION);

    if (_SHR_IS_FLAG_SET(info->flags2, not_supported_flags2))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "one of the used flags2 is not supported - flags = 0x%08X", info->flags2);
    }

    if ((info->action < BCM_MPLS_SWITCH_ACTION_SWAP) || (info->action > BCM_MPLS_SWITCH_ACTION_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "action not valid = %d", info->action);
    }
    /*
     * Only actions supported are NOP, PHP, POP and SWAP
     */
    else if (info->action == BCM_MPLS_SWITCH_ACTION_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "action not supported = %d", info->action);
    }
    else if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {
        if (info->action == BCM_MPLS_SWITCH_EGRESS_ACTION_PUSH)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "action push only supported under JR mode= %d", info->action);
        }
        if (_SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_NEXT_HEADER_IPV4 | BCM_MPLS_SWITCH_NEXT_HEADER_IPV6))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_MPLS_SWITCH_NEXT_HEADER_IPVx only support under JR mode");
        }
    }

    if (info->action != BCM_MPLS_SWITCH_ACTION_POP)
    {
        /** in case SWAP/NOP/PHP were requested verify that TTL_DECREMENT is set */
        if (((info->action == BCM_MPLS_SWITCH_ACTION_SWAP) || (info->action == BCM_MPLS_SWITCH_ACTION_NOP) ||
             (info->action == BCM_MPLS_SWITCH_ACTION_PHP) || (info->action == BCM_MPLS_SWITCH_ACTION_POP_DIRECT))
            && (!_SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_TTL_DECREMENT)))
        {
            /** TTL always decremented (even if inherited/copied */
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "BCM_MPLS_SWITCH_TTL_DECREMENT must be set in case of ILM SWAP or action NOP/PHP. action = %d, flags = 0x%08X",
                         info->action, info->flags);
        }
    }

    if (BCM_L3_ITF_TYPE_IS_LIF(info->egress_if) && (info->action != BCM_MPLS_SWITCH_ACTION_NOP))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "In case egress_if is LIF, action must be BCM_MPLS_SWITCH_ACTION_NOP");
    }

    if (!DNX_MPLS_LABEL_IN_RANGE(unit, info->label))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "MPLS label (%d) is out of range. The range should be 0 - %d", info->label,
                     DNX_MPLS_LABEL_MAX(unit));
    }

    /*
     * QOS ECN model
     */
    if (info->ingress_qos_model.ingress_ecn == bcmQosIngressEcnModelEligible)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "MPLS does not support ECN eligible");
    }

    /*
     * Range check only apply for genaral label
     */
    if (info->label > MPLS_LSB_NIBBLE_RSV)
    {
        /*
         * Verify if the key (label) is in IML range, in case of IML, or out of IML ranges otherwise.
         */
        int is_iml = FALSE;
        bcm_mpls_label_t found_labels_low = 0, found_labels_high = 0;
        /*
         * Not required but keeps the compiler happy.
         */
        SHR_IF_ERR_EXIT(dnx_mpls_label_iml_range_check
                        (unit, 1, &info->label, &is_iml, &found_labels_low, &found_labels_high));
        if (DNX_MPLS_SWITCH_IS_EVPN_MC(info) && !is_iml)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Specified IML label (%d) is not within any IML range.", info->label);
        }
        else if (is_iml && (!DNX_MPLS_SWITCH_IS_EVPN_MC(info)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Specified label (%d) is within an IML range (%d-%d), but the label is not defined as IML.",
                         info->label, found_labels_low, found_labels_high);
        }
    }

    /*
     * EVPN has no 2nd key. If not EVPN, check potential 2nd keys.
     */
    if (!DNX_MPLS_SWITCH_IS_EVPN(info))
    {
        SHR_IF_ERR_EXIT(dnx_mpls_tunnel_switch_verify_2nd_key(unit, info));
    }

    if (_SHR_IS_FLAG_SET(info->flags2, BCM_MPLS_SWITCH2_CROSS_CONNECT))
    {
        if (!DNX_MPLS_SWITCH_IS_EVPN(info))
        {
            /*
             * This flag is only valid for EVPN
             */
            SHR_ERR_EXIT(_SHR_E_PARAM, "flag BCM_MPLS_SWITCH2_CROSS_CONNECT (0x%x) is only valid for EVPN/IML.\n",
                         BCM_MPLS_SWITCH2_CROSS_CONNECT);
        }
        
        if (info->vpn > 0)
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Cross-Connected EVPN/IML with vpn > 0 is not supported. vpn = %d.\n",
                         info->vpn);
        }
    }
    else
    {
        if (DNX_MPLS_SWITCH_IS_EVPN(info) && (info->vpn == 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "EVPN/IML with vpn == 0 must have BCM_MPLS_SWITCH2_CROSS_CONNECT (0x%x) flag set, "
                         "but flags2 = 0x%08x.\n", BCM_MPLS_SWITCH2_CROSS_CONNECT, info->flags2);
        }
    }

    /** only SWAP action can use egress label */
    if ((info->action != BCM_MPLS_SWITCH_ACTION_SWAP) && (info->egress_label.label != BCM_MPLS_LABEL_INVALID)
        && (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "egress label is valid only in case of swap command");
    }

    if (DNX_FAILOVER_IS_PROTECTION_ENABLED(info->failover_id))
    {
        if (info->action != BCM_MPLS_SWITCH_ACTION_POP)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "protection can't be set without BCM_MPLS_SWITCH_ACTION_POP flag");
        }
        SHR_IF_ERR_EXIT(dnx_failover_id_valid_verify(unit, DNX_FAILOVER_TYPE_INGRESS, 0, info->failover_id));
    }

    /*
     * Statistics check
     */
    if ((info->action == BCM_MPLS_SWITCH_ACTION_NOP) && (_SHR_IS_FLAG_SET(info->flags2, BCM_MPLS_SWITCH2_STAT_ENABLE)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM_MPLS_SWITCH2_STAT_ENABLE flag (0x%08x) can't be set for NOP action (mpls forwarding). Flags value: 0x%08x",
                     BCM_MPLS_SWITCH2_STAT_ENABLE, info->flags2);
    }
    SHR_IF_ERR_EXIT(dnx_stat_pp_crps_verify
                    (unit, _SHR_CORE_ALL, info->stat_id, info->stat_pp_profile,
                     bcmStatCounterInterfaceIngressReceivePp));

    if ((info->action == BCM_MPLS_SWITCH_ACTION_NOP)
        && BCM_FORWARD_ENCAP_ID_IS_EEI(info->egress_if)
        && (BCM_FORWARD_ENCAP_ID_EEI_USAGE_GET(info->egress_if) == BCM_FORWARD_ENCAP_ID_EEI_USAGE_ENCAP_POINTER)
        && (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ILM format fec + eei.pointer is only supported under JR mode");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Used by bcm_dnx_mpls_tunnel_switch_get and bcm_dnx_mpls_tunnel_switch_delete to verify info struct.
* \par DIRECT INPUT:
*   \param [in] unit  -  Relevant unit.
*   \param [in] info  -
*   A pointer to the struct that holds information for the ingress MPLS entry.
*   This struct will be filled by previousely save information the given info.label
* \par INDIRECT INPUT:
*   * \b *info \n
*     See 'info' in DIRECT INPUT above
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_mpls_tunnel_switch_get
*   * \ref bcm_dnx_mpls_tunnel_switch_delete
*/
static shr_error_e
dnx_mpls_tunnel_switch_get_verify(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    if (!DNX_MPLS_LABEL_IN_RANGE(unit, info->label))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "MPLS label (%d) is out of range. The range should be 0 - %d", info->label,
                     DNX_MPLS_LABEL_MAX(unit));
    }

    if ((info->action < BCM_MPLS_SWITCH_ACTION_SWAP) || (info->action > BCM_MPLS_SWITCH_ACTION_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "action not valid = %d", info->action);
    }
    /*
     * Only actions supported are NOP, PHP, POP and SWAP
     */
    else if (info->action == BCM_MPLS_SWITCH_ACTION_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "action not supported = %d", info->action);
    }
    else if ((system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
             && (info->action == BCM_MPLS_SWITCH_EGRESS_ACTION_PUSH))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "action %d only supported under JR1 mode", info->action);
    }
    SHR_IF_ERR_EXIT(dnx_mpls_tunnel_switch_verify_2nd_key(unit, info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *      Used by bcm_dnx_mpls_vpn_id_create to verify API input.
 * \param [in] unit -
 *      Relevant unit.
 * \param [in, out] info - A pointer to the struct that holds information for the mpls vpn instance.
 *
 * \return
 *      \retval Zero in case of NO ERROR.
 *      \retval Negative in case of an error, see \ref shr_error_e.
 * \remark
 *      * None
 * \see
 *      * bcm_mpls_vpn_config_t

 */
static shr_error_e
dnx_mpls_vpn_create_verify(
    int unit,
    bcm_mpls_vpn_config_t * info)
{
    uint8 with_id, update;
    uint8 is_vsi_used;
    uint8 is_vsi_usage_mpls;
    uint32 vpn_id;
    uint32 uc, mc, bc;
    int nof_vsis;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");
    SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));
    /*
     *  Check flag for not support
     */
    if ((info->flags & (BCM_MPLS_VPN_L3 | BCM_MPLS_VPN_VPWS)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags BCM_MPLS_VPN_L3 & BCM_MPLS_VPN_VPWS are not supported!\n");
    }

    if (info->flags & BCM_MPLS_VPN_VPLS)
    {
        if (!(info->flags & BCM_MPLS_VPN_WITH_ID))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "MPLS VPN has to be called with id!\n");
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "MPLS VPN has to be called with flag BCM_MPLS_VPN_VPLS!\n");
    }

    /*
     * Check VPN usage
     */
    update = (info->flags & BCM_MPLS_VPN_REPLACE) ? TRUE : FALSE;
    with_id = (info->flags & BCM_MPLS_VPN_WITH_ID) ? TRUE : FALSE;
    vpn_id = DNX_VPN_ID_GET(info->vpn);

    if (vpn_id >= nof_vsis)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "VPN ID illegal!\n");
    }

    SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_per_type_get(unit, vpn_id, _bcmDnxVsiTypeMpls, &is_vsi_usage_mpls));

    /*
     * check if VSI used by none
     */
    SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_check_except_type(unit, vpn_id, _bcmDnxVsiTypeNone, &is_vsi_used));

    if (update)
    {
        if (!with_id)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_MPLS_VPN_REPLACE can't be used without BCM_MPLS_VPN_WITH_ID!\n");
        }
        else if (!is_vsi_used)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "VPN not allocated. In case of Replace assumed VPN is already allocated!\n");
        }
        else if (!is_vsi_usage_mpls)
        {
            SHR_ERR_EXIT(_SHR_E_EXISTS, "The VPN is already allocated but not as an MPLS type!\n");
        }
    }
    else if (with_id && is_vsi_usage_mpls)
    {
        SHR_ERR_EXIT(_SHR_E_EXISTS, "In case the VSI is already allocated, BCM_MPLS_VPN_REPLACE should be used!\n");
    }

    /*
     * Check flooding fields for BC/MC/UC
     */
    uc = _BCM_MULTICAST_ID_GET(info->unknown_unicast_group);
    mc = _BCM_MULTICAST_ID_GET(info->unknown_multicast_group);
    bc = _BCM_MULTICAST_ID_GET(info->broadcast_group);

    if ((mc != uc) || (bc != uc))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "For BC and MC group should qual to unknown UC group, call bcm_port_flood_group_set() to set per LIF!\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Used by bcm_dnx_mpls_vpn_id_destroy to verify API input.
 *
 * \param [in] unit -
 *      Relevant unit.
 * \param [in] vpn -
 *      VPN number
 * \return
 *      \retval Zero in case of NO ERROR.
 *      \retval Negative in case of an error, see \ref shr_error_e.
 * \remark
 *      * None
 * \see
 *      * None
 */
static shr_error_e
dnx_mpls_vpn_get_and_destroy_verify(
    int unit,
    bcm_vpn_t vpn)
{
    int vpn_id;
    uint8 is_vsi_allocated;
    uint8 is_vsi_usage_mpls;
    int nof_vsis;

    SHR_FUNC_INIT_VARS(unit);

    vpn_id = DNX_VPN_ID_GET(vpn);
    SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));

    /** Cross Connect VPN isn't allocated, but is valid for configuration */
    if (vpn_id == 0)
    {
        SHR_EXIT();
    }

    /** Check vsi range */
    if (vpn_id >= nof_vsis)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "VPN illegal!\n");
    }

    /** vsi/vpn is allocated*/
    SHR_IF_ERR_EXIT(vlan_db.vsi.is_allocated(unit, vpn_id, &is_vsi_allocated));
    if (!is_vsi_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "VPN doesn't exist!\n");
    }

    /** Is MPLS VPN*/
    SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_per_type_get(unit, vpn_id, _bcmDnxVsiTypeMpls, &is_vsi_usage_mpls));

    if (!is_vsi_usage_mpls)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "This is not MPLS VPN!\n");
    }

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
*  Initialize PHP_MAP_FIRST_NIBBLE table with predefined values
*   \param [in] unit         -  Relevant unit.
* \return
*   shr_error_e - Non-zero in case of an error.
*/
static shr_error_e
dnx_mpls_init_map_first_nibble(
    int unit)
{
    uint32 entry_handle_id;
    int nibble_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MPLS_PHP_MAP_FIRST_NIBBLE, &entry_handle_id));
    for (nibble_index = 0; nibble_index < 16; nibble_index++)
    {
        switch (nibble_index)
        {
            case MPLS_FIRST_NIBBLE_PROTOCOL_IPV4:
                /** set IPv4 nibble mapping */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIRST_NIBBLE,
                                           MPLS_FIRST_NIBBLE_PROTOCOL_IPV4);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIRST_NIBBLE_INDEX, INST_SINGLE,
                                             DBAL_ENUM_FVAL_FIRST_NIBBLE_INDEX_IPV4);
                break;
            case MPLS_FIRST_NIBBLE_PROTOCOL_IPV6:
                /** set IPv6 nibble mapping */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIRST_NIBBLE,
                                           MPLS_FIRST_NIBBLE_PROTOCOL_IPV6);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIRST_NIBBLE_INDEX, INST_SINGLE,
                                             DBAL_ENUM_FVAL_FIRST_NIBBLE_INDEX_IPV6);
                break;
            default:
                /** set default invalid values */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIRST_NIBBLE, nibble_index);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIRST_NIBBLE_INDEX, INST_SINGLE,
                                             DBAL_ENUM_FVAL_FIRST_NIBBLE_INDEX_INVALID);
                break;
        }
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Initialize PHP_MAP_FWD_CODE table with predefined values
*   \param [in] unit         -  Relevant unit.
* \return
*   shr_error_e - Non-zero in case of an error.
*/
static shr_error_e
dnx_mpls_init_map_fwd_code(
    int unit)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_first_nibble_index_e first_nibble_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Currently, EGRESS_FWD_CODE == *_R0 (FAI[3]==0) is mapped to REMAKR.
     *            EGRESS_FWD_CODE == *_R1 (FAI[3]==1) is mapped to PRESERVE.
     * In PHP, all should be REMARK. So, here the IPvx fwd-code will be *_R0 now.
     * Note: just replace them with *_R1 after ucode updating.
     */

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MPLS_PHP_MAP_FWD_CODE, &entry_handle_id));
    for (first_nibble_index = 0; first_nibble_index < DBAL_NOF_ENUM_FIRST_NIBBLE_INDEX_VALUES; first_nibble_index++)
    {
        switch (first_nibble_index)
        {
            case DBAL_ENUM_FVAL_FIRST_NIBBLE_INDEX_IPV4:
                /** set IPv4_uc forwarding code */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIRST_NIBBLE_INDEX, first_nibble_index);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MC, MPLS_BIT_CLEAR);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_CODE, INST_SINGLE,
                                             DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV4_UC_R0);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

                /** set IPv4_mc forwarding code */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIRST_NIBBLE_INDEX, first_nibble_index);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MC, MPLS_BIT_SET);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_CODE, INST_SINGLE,
                                             DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV4_MC_R0);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
                break;
            case DBAL_ENUM_FVAL_FIRST_NIBBLE_INDEX_IPV6:
                /** set IPv6_uc forwarding code */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIRST_NIBBLE_INDEX, first_nibble_index);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MC, MPLS_BIT_CLEAR);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_CODE, INST_SINGLE,
                                             DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6_UC_R0);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

                /** set IPv6_mc forwarding code */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIRST_NIBBLE_INDEX, first_nibble_index);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MC, MPLS_BIT_SET);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_CODE, INST_SINGLE,
                                             DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6_MC_R0);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
                break;
            default:
                /** set IPv6_uc forwarding code */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIRST_NIBBLE_INDEX, first_nibble_index);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MC, MPLS_BIT_CLEAR);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_CODE, INST_SINGLE,
                                             DBAL_ENUM_FVAL_EGRESS_FWD_CODE_FWD_CODE_BUG_CODE);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

                /** set IPv6_mc forwarding code */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIRST_NIBBLE_INDEX, first_nibble_index);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MC, MPLS_BIT_SET);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_CODE, INST_SINGLE,
                                             DBAL_ENUM_FVAL_EGRESS_FWD_CODE_FWD_CODE_BUG_CODE);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
                break;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Initialize DBAL_TABLE_EGRESS_MPLS_FHEI_TO_EES_RESOLUTION table with predefined values
*   \param [in] unit         -  Relevant unit.
* \return
*   shr_error_e - Non-zero in case of an error.
*/
static shr_error_e
dnx_mpls_init_fhei_to_ees_resolution(
    int unit)
{
    uint32 entry_handle_id;
    uint32 result_type_value;
    int qos_model;
    int ttl_model;
    int pop_model;
    int system_headers_mode;
    uint32 mpls_command;
    uint32 remark_profile = 0;
    uint32 ttl_profile, field_value;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_MPLS_FHEI_TO_EES_RESOLUTION, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                    (unit, DBAL_FIELD_ETPP_ETPS_FORMATS, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_PHP,
                     &result_type_value));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_MPLS_POP_TYPE, INST_SINGLE, result_type_value);

    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                    (unit, DBAL_FIELD_ETPP_ETPS_FORMATS, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_FHEI,
                     &result_type_value));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_MPLS_SWAP_TYPE, INST_SINGLE, result_type_value);

    /*
     *  POP models: traverse pipe and uniform for TTL and qos
     */
    for (ttl_model = DBAL_ENUM_FVAL_MODEL_TYPE_UNIFORM; ttl_model < DBAL_NOF_ENUM_MODEL_TYPE_VALUES; ttl_model++)
    {
        for (qos_model = DBAL_ENUM_FVAL_MODEL_TYPE_UNIFORM; qos_model < DBAL_NOF_ENUM_MODEL_TYPE_VALUES; qos_model++)
        {
            pop_model = (ttl_model << TTL_MODEL_OFFSET) | qos_model;
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_POP_TTL_MODEL, pop_model, ttl_model);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_POP_QOS_MODEL, pop_model, qos_model);
        }
    }

    /*
     * SWAP remark profile for multi mpls command
     */
    if (system_headers_mode != DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
        for (mpls_command = DNX_EEI_IDENTIFIER_MULTI_SWAP_START; mpls_command <= DNX_EEI_IDENTIFIER_MULTI_SWAP_LAST;
             mpls_command++)
        {
            /**mpls command 1:1 mapping to remark profile, multi_swap_start -> 0, ..., multi_swap_last -> 7*/
            remark_profile = mpls_command - DNX_EEI_IDENTIFIER_MULTI_SWAP_START;
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_SWAP_REMARK, mpls_command,
                                         remark_profile);
        }
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * SWAP QoS/TTL models: Configure Qos prefix for FHEI to EES
     */
    ttl_profile = dnx_data_qos.qos.eei_mpls_ttl_profile_get(unit);
    field_value = ttl_profile >> 1;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FHEI_ARR_PREFIX, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPS_FHEI_FIELD,
                               DBAL_ENUM_FVAL_ETPS_FHEI_FIELD_TTL_IDX);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PREFIX_VALUE, INST_SINGLE, field_value);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Initialize DBAL_TABLE_INGRESS_MPLS_EES_TO_FHEI_RESOLUTION table with predefined values
*   \param [in] unit         -  Relevant unit.
* \return
*   shr_error_e - Non-zero in case of an error.
*/
static shr_error_e
dnx_mpls_init_ees_to_fhei_resolution(
    int unit)
{
    uint32 entry_handle_id;
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    /** init command multi swap, include php, must set php later again*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_MPLS_EES_TO_FHEI_RESOLUTION, &entry_handle_id));
    if (system_headers_mode != DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_EEI_IDENTIFIER,
                                         DNX_EEI_IDENTIFIER_MULTI_SWAP_START, DNX_EEI_IDENTIFIER_MULTI_SWAP_LAST);
    }
    else
    {
        /**only set one identifer swap*/
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EEI_IDENTIFIER, DNX_EEI_IDENTIFIER_EEI_SWAP);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREATE_FHEI_INSTEAD_OF_EEI, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_FHEI_TYPE_MPLS_SWAP);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_SIZE, INST_SINGLE, DBAL_ENUM_FVAL_FHEI_SIZE_5B);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * JR2 mode command 1 is for ingress pop, In JR1 mode, pop eei identifier is 8
     */
    if (system_headers_mode != DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_INGRESS_MPLS_EES_TO_FHEI_RESOLUTION, entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EEI_IDENTIFIER, DNX_EEI_IDENTIFIER_POP);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREATE_FHEI_INSTEAD_OF_EEI, INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_TYPE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_FHEI_TYPE_MPLS_POP);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_SIZE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_FHEI_SIZE_3B);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_INGRESS_MPLS_EES_TO_FHEI_RESOLUTION, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EEI_IDENTIFIER, DNX_EEI_IDENTIFIER_EEI_PHP);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREATE_FHEI_INSTEAD_OF_EEI, INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_TYPE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_FHEI_TYPE_MPLS_POP);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_SIZE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_FHEI_SIZE_3B);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /*
     * vlan edit eei
     */
    if (system_headers_mode != DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_INGRESS_MPLS_EES_TO_FHEI_RESOLUTION, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EEI_IDENTIFIER, DNX_EEI_IDENTIFIER_VLAN_EDIT);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREATE_FHEI_INSTEAD_OF_EEI, INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_TYPE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_FHEI_TYPE_VLAN_EDIT);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_SIZE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_FHEI_SIZE_5B);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Initialize DBAL_TABLE_MPLS_GENERAL_CONFIGURATION table with predefined values
*   \param [in] unit         -  Relevant unit.
* \return
*   shr_error_e - Non-zero in case of an error.
*/
static shr_error_e
dnx_mpls_init_general_configuration(
    int unit)
{
    uint32 entry_handle_id;
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Initiate the FHEI mpls command HW table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MPLS_GENERAL_CONFIGURATION, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BLOCK_IS_ETPP, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_MPLS_SWAP_CMD, INST_SINGLE,
                                 DBAL_ENUM_FVAL_FHEI_MPLS_COMMAND_SWAP);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_MPLS_TYPE_POP, INST_SINGLE,
                                 DBAL_ENUM_FVAL_FHEI_TYPE_UNUSED);

    /*
     * Jericho mode beginning
     * {
     */
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_MPLS_POP_CMD, INST_SINGLE,
                                     DBAL_ENUM_FVAL_FHEI_MPLS_COMMAND_POP);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Initiate the profile 0 , which is default one.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_JR1_MPLS_PUSH_COMMAND, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PUSH_PROFILE, 0);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_JR1_MPLS_PUSH_COMMAND_ETPS_MPLS_1);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /*
     * Jericho mode end
     * }
     */

    /*
     * Set Alternate Marking special label value to 16 (init)
     */
    SHR_IF_ERR_EXIT(dnx_mpls_alternate_marking_special_label_set(unit, DNX_MPLS_ALTERNATE_MARKING_SL_DEFAULT));

    /*
     * Reserve MPLS label range 0 for default one, 
     * matching all general label
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MPLS_LABEL_RANGE_PROPERTIES, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RANGE_PROFILE, 0);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL_LOW, INST_SINGLE, MPLS_LSB_NIBBLE_RSV + 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL_HIGH, INST_SINGLE, 0xFFFFF);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_QUALIFIER_LABEL_RANGE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_MPLS_QUALIFIER_LABEL_RANGE_DEFAULT_LABEL_RANGE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BOS_MASK, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BOS_VALUE, INST_SINGLE, FALSE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* Add GAL, ELI, OAM alert to MPLS_TERMINATION_SINGLE_LABEL_DB table with dummy termination profile indicating tunnels_to_terminate=0.
* This is done in order to invoke special label after handling in the HW in case no MPLS tunnel found before those labels.
*   \param [in] unit         -  Relevant unit.
* \return
*   shr_error_e - Non-zero in case of an error.
*/
static shr_error_e
dnx_mpls_special_labels_termination_init(
    int unit)
{
    uint32 entry_handle_id;
    dnx_mpls_termination_profile_t term_profile_info;
    uint8 is_first_term_profile_reference;
    int term_profile;
    lif_mngr_local_inlif_info_t inlif_info;
    uint32 lif_flags;
    dnx_mpls_term_match_t match_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** define termination dummy profile with no tunnels to terminate */
    sal_memset(&term_profile_info, 0, sizeof(dnx_mpls_termination_profile_t));
    term_profile_info.labels_to_terminate = NO_MPLS_LABELS_TO_TERMINATE;

    /** at module init no need for profile exchange - old_term_profile and is_last_term_profile_reference are not needed */
    SHR_IF_ERR_EXIT(dnx_mpls_termination_profile_allocation
                    (unit, term_profile_info, &term_profile, 0, &is_first_term_profile_reference, NULL));

    if (is_first_term_profile_reference)
    {
        /** Set profile termination in case it is the first time. */
        SHR_IF_ERR_EXIT(dnx_mpls_termination_profile_hw_set(unit, &term_profile_info, term_profile));
    }

    sal_memset(&inlif_info, 0, sizeof(inlif_info));
    inlif_info.dbal_table_id = DBAL_TABLE_IN_LIF_FORMAT_DUMMY_MPLS;
    inlif_info.dbal_result_type = DBAL_RESULT_TYPE_IN_LIF_FORMAT_DUMMY_MPLS_IN_LIF_MPLS_DUMMY;
    inlif_info.core_id = _SHR_CORE_ALL;
    lif_flags = LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF;
    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, lif_flags, NULL, &inlif_info, NULL));

    /** save local lif with dumy profile */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_LIF_FORMAT_DUMMY_MPLS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, inlif_info.local_inlif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_IN_LIF_FORMAT_DUMMY_MPLS_IN_LIF_MPLS_DUMMY);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TERMINATION_TYPE, INST_SINGLE, term_profile);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** add GAL, ELI, OAM alert labels to DBAL_TABLE_MPLS_TERMINATION_SINGLE_LABEL_DB with dumy termination profile in order to invoke special after handling in the HW */
    match_info.match_criteria = TERM_MATCH_CRITERIA_LABEL;
    match_info.label = MPLS_LABEL_GAL;
    SHR_IF_ERR_EXIT(dnx_mpls_termination_lookup(unit, TERM_LOOKUP_SET, &match_info, &inlif_info.local_inlif));
    match_info.label = MPLS_LSB_NIBBLE_ELI;
    SHR_IF_ERR_EXIT(dnx_mpls_termination_lookup(unit, TERM_LOOKUP_SET, &match_info, &inlif_info.local_inlif));
    match_info.label = MPLS_LSB_NIBBLE_OAM_ALERT;
    SHR_IF_ERR_EXIT(dnx_mpls_termination_lookup(unit, TERM_LOOKUP_SET, &match_info, &inlif_info.local_inlif));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
* \brief
*  Set single entry in  MPLS_SPECIAL_LABEL_PROFILE_MAP.
* The key of the table is {MPLS-label.label(4 lsb), MPLS-label.BOS} and the result is the label profile to be used.
* MPLS special label < 16, thus only 4 lsb are used.
*   \param [in] unit         -  Relevant unit.
*   \param [in] label_nibble         -  last LSB of mpls special label
*   \param [in] bos         -  BOS set/unset indication.
*   \param [in] special_label_profile         -  special label profile to set.
* \return
*   shr_error_e - Non-zero in case of an error.
*/
static shr_error_e
dnx_mpls_init_mpls_special_label_profile_map_single_entry(
    int unit,
    uint32 label_nibble,
    uint32 bos,
    uint32 special_label_profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MPLS_SPECIAL_LABEL_PROFILE_MAP, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL_NIBBLE, label_nibble);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BOS, bos);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_SPECIAL_LABEL_PROFILE,
                                 INST_SINGLE, special_label_profile);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Initialize MPLS_SPECIAL_LABEL_PROFILE_MAP table with predefined values.
* The key of the table is {MPLS-label.label(4 lsb), MPLS-label.BOS} and the result is the label profile to be used.
* MPLS special label < 16, thus only 4 lsb are used.
*   \param [in] unit         -  Relevant unit.
* \return
*   shr_error_e - Non-zero in case of an error.
*/
shr_error_e
dnx_mpls_init_mpls_special_label_profile_map(
    int unit)
{
    int nibble_index;

    SHR_FUNC_INIT_VARS(unit);

    for (nibble_index = 0; nibble_index < 16; nibble_index++)
    {
        switch (nibble_index)
        {
            case MPLS_LSB_NIBBLE_IPV4_EXPLICIT_NULL:

                SHR_IF_ERR_EXIT(dnx_mpls_init_mpls_special_label_profile_map_single_entry
                                (unit, MPLS_LSB_NIBBLE_IPV4_EXPLICIT_NULL, MPLS_BIT_CLEAR,
                                 DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_IPV4_EXPLICIT_NULL));
                SHR_IF_ERR_EXIT(dnx_mpls_init_mpls_special_label_profile_map_single_entry
                                (unit, MPLS_LSB_NIBBLE_IPV4_EXPLICIT_NULL, MPLS_BIT_SET,
                                 DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_IPV4_EXPLICIT_NULL));
                break;
            case MPLS_LSB_NIBBLE_RA:
                SHR_IF_ERR_EXIT(dnx_mpls_init_mpls_special_label_profile_map_single_entry
                                (unit, MPLS_LSB_NIBBLE_RA, MPLS_BIT_CLEAR,
                                 DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_ROUTER_ALERT));
                SHR_IF_ERR_EXIT(dnx_mpls_init_mpls_special_label_profile_map_single_entry
                                (unit, MPLS_LSB_NIBBLE_RA, MPLS_BIT_SET,
                                 DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_ROUTER_ALERT));
                break;
            case MPLS_LSB_NIBBLE_IPV6_EXPLICIT_NULL:
                SHR_IF_ERR_EXIT(dnx_mpls_init_mpls_special_label_profile_map_single_entry
                                (unit, MPLS_LSB_NIBBLE_IPV6_EXPLICIT_NULL, MPLS_BIT_CLEAR,
                                 DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_IPV6_EXPLICIT_NULL));
                SHR_IF_ERR_EXIT(dnx_mpls_init_mpls_special_label_profile_map_single_entry
                                (unit, MPLS_LSB_NIBBLE_IPV6_EXPLICIT_NULL, MPLS_BIT_SET,
                                 DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_IPV6_EXPLICIT_NULL));
                break;
            case MPLS_LSB_NIBBLE_ELI:
                SHR_IF_ERR_EXIT(dnx_mpls_init_mpls_special_label_profile_map_single_entry
                                (unit, MPLS_LSB_NIBBLE_ELI, MPLS_BIT_CLEAR,
                                 DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_ELI));
                SHR_IF_ERR_EXIT(dnx_mpls_init_mpls_special_label_profile_map_single_entry
                                (unit, MPLS_LSB_NIBBLE_ELI, MPLS_BIT_SET,
                                 DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_ELI));
                break;
            case MPLS_LABEL_GAL:
                SHR_IF_ERR_EXIT(dnx_mpls_init_mpls_special_label_profile_map_single_entry
                                (unit, MPLS_LABEL_GAL, MPLS_BIT_CLEAR,
                                 DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_GAL));
                SHR_IF_ERR_EXIT(dnx_mpls_init_mpls_special_label_profile_map_single_entry
                                (unit, MPLS_LABEL_GAL, MPLS_BIT_SET,
                                 DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_GAL));
                break;
            case MPLS_LSB_NIBBLE_OAM_ALERT:
                SHR_IF_ERR_EXIT(dnx_mpls_init_mpls_special_label_profile_map_single_entry
                                (unit, MPLS_LSB_NIBBLE_OAM_ALERT, MPLS_BIT_CLEAR,
                                 DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_OAM_ALERT));
                SHR_IF_ERR_EXIT(dnx_mpls_init_mpls_special_label_profile_map_single_entry
                                (unit, MPLS_LSB_NIBBLE_OAM_ALERT, MPLS_BIT_SET,
                                 DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_OAM_ALERT));
                break;
            default:
                /** not supported mpls labels, thus invalid profile is set leading to trap error */
                SHR_IF_ERR_EXIT(dnx_mpls_init_mpls_special_label_profile_map_single_entry
                                (unit, nibble_index, MPLS_BIT_CLEAR,
                                 DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_INVALID_LABEL));
                SHR_IF_ERR_EXIT(dnx_mpls_init_mpls_special_label_profile_map_single_entry
                                (unit, nibble_index, MPLS_BIT_SET,
                                 DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_INVALID_LABEL));
                break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_mpls_special_label_as_general_label_set(
    int unit,
    int label,
    int enable)
{
    int nibble_index;
    uint32 entry_handle_id;
    int nof_entries;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (label >= MPLS_LSB_NIBBLE_RSV)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Only special label support this feature!");
    }

    nof_entries = sizeof(special_label_property) / sizeof(special_label_property_t);

    /*
     * There are 2 tables may recognize a label as special one.
     * By default EN(0) is different from other lables,it's recoginzied by
     * MPLS_DEFAULT_SPECIAL_LABEL_PROPERTY 
     */
    if (enable == TRUE)
    {
        if (label == MPLS_LSB_NIBBLE_IPV4_EXPLICIT_NULL)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MPLS_DEFAULT_SPECIAL_LABEL_PROPERTY, &entry_handle_id));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPECIAL_LABEL_TO_SPECIAL_PROFILE_0,
                                         INST_SINGLE, MPLS_LSB_NIBBLE_RSV);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPECIAL_LABEL_TO_SPECIAL_PROFILE_1,
                                         INST_SINGLE, MPLS_LSB_NIBBLE_RSV);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }

        SHR_IF_ERR_EXIT(dnx_mpls_init_mpls_special_label_profile_map_single_entry
                        (unit, label, MPLS_BIT_CLEAR, DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_TUNNEL_LABEL));
        SHR_IF_ERR_EXIT(dnx_mpls_init_mpls_special_label_profile_map_single_entry
                        (unit, label, MPLS_BIT_SET, DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_TUNNEL_LABEL));
    }
    else
    {
        if (label == MPLS_LSB_NIBBLE_IPV4_EXPLICIT_NULL)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MPLS_DEFAULT_SPECIAL_LABEL_PROPERTY, &entry_handle_id));

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPECIAL_LABEL_TO_SPECIAL_PROFILE_0,
                                         INST_SINGLE, MPLS_LSB_NIBBLE_IPV4_EXPLICIT_NULL);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPECIAL_LABEL_TO_SPECIAL_PROFILE_1,
                                         INST_SINGLE, MPLS_LSB_NIBBLE_IPV4_EXPLICIT_NULL);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }

        for (nibble_index = 0; nibble_index < nof_entries; nibble_index++)
        {
            if (special_label_property[nibble_index].label_nibble == label)
            {
                SHR_IF_ERR_EXIT(dnx_mpls_init_mpls_special_label_profile_map_single_entry
                                (unit, label, MPLS_BIT_CLEAR, special_label_property[nibble_index].label_profile));
                SHR_IF_ERR_EXIT(dnx_mpls_init_mpls_special_label_profile_map_single_entry
                                (unit, label, MPLS_BIT_SET, special_label_property[nibble_index].label_profile));
                break;
            }
        }

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_mpls_special_label_as_general_label_get(
    int unit,
    int label,
    int *enable)
{
    uint32 entry_handle_id;
    uint32 special_label_profile = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (label >= MPLS_LSB_NIBBLE_RSV)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Only special label support this feature!");
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MPLS_SPECIAL_LABEL_PROFILE_MAP, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL_NIBBLE, label);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BOS, TRUE);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VTT_MPLS_SPECIAL_LABEL_PROFILE, INST_SINGLE,
                               &special_label_profile);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *enable = (special_label_profile == DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_TUNNEL_LABEL) ? TRUE : FALSE;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_mpls_init_dual_homing_default_result(
    int unit)
{
    int esem_handle;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    esem_handle = dnx_data_esem.default_result_profile.default_dual_homing_get(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ESEM_DEFAULT_RESULT_PROFILE, esem_handle);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_ESEM_DEFAULT_RESULT_TABLE_ETPS_NULL);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
* \brief
*  Allocate and set a dummy LIF for Flow-Label termination in EVPN IML.
*
*   \param [in] unit         -  Relevant unit.
*   \param [in] nof_labels_to_term -  Number of labels to terminate in the LIF
*   \param [in] has_cw       -  HAS_CW indicator for MPLS termiantion.
*   \param [in] mpls_bos     -  MPLS BOS indicator for MPLS termiantion.
* \return
*   shr_error_e - Non-zero in case of an error.
*/
static shr_error_e
dnx_mpls_dual_homing_fl_termination_dummy_set(
    int unit,
    uint32 nof_labels_to_term,
    uint32 has_cw,
    uint32 mpls_bos)
{
    uint32 entry_handle_id;
    dnx_mpls_termination_profile_t term_profile_info;
    uint8 is_first_term_profile_reference;
    int term_profile;
    lif_mngr_local_inlif_info_t inlif_info;
    uint32 lif_flags;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** define termination dummy profile */
    sal_memset(&term_profile_info, 0, sizeof(dnx_mpls_termination_profile_t));
    term_profile_info.labels_to_terminate = nof_labels_to_term;
    term_profile_info.has_cw = has_cw;

    SHR_IF_ERR_EXIT(dnx_mpls_termination_profile_allocation
                    (unit, term_profile_info, &term_profile, 0, &is_first_term_profile_reference, NULL));

    if (is_first_term_profile_reference)
    {
        /** Set profile termination in case it is the first time. */
        SHR_IF_ERR_EXIT(dnx_mpls_termination_profile_hw_set(unit, &term_profile_info, term_profile));
    }

    sal_memset(&inlif_info, 0, sizeof(inlif_info));
    inlif_info.dbal_table_id = DBAL_TABLE_IN_LIF_FORMAT_DUMMY_MPLS;
    inlif_info.dbal_result_type = DBAL_RESULT_TYPE_IN_LIF_FORMAT_DUMMY_MPLS_IN_LIF_MPLS_DUMMY_WITH_PARSER;
    inlif_info.core_id = _SHR_CORE_ALL;
    lif_flags = LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF;
    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, lif_flags, NULL, &inlif_info, NULL));

    /** save local lif with dumy profile */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_LIF_FORMAT_DUMMY_MPLS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, inlif_info.local_inlif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_IN_LIF_FORMAT_DUMMY_MPLS_IN_LIF_MPLS_DUMMY_WITH_PARSER);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TERMINATION_TYPE, INST_SINGLE, term_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_PARSER_CONTEXT, INST_SINGLE,
                                 DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_ETH_B1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Add match key to the local lif */
    if (has_cw != FALSE)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR
                        (unit, DBAL_TABLE_EVPN_IML_ADDITIONAL_HEADERS_FL_AND_CW_ESI_CHECK, entry_handle_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR
                        (unit, DBAL_TABLE_EVPN_IML_ADDITIONAL_HEADERS_FL_ONLY_ESI_CHECK, entry_handle_id));
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_BOS, mpls_bos);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, inlif_info.local_inlif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
* \brief
*  Init some dummy LIF for Flow-Label termination in EVPN IML.
* 
*   \param [in] unit         -  Relevant unit.
* \return
*   shr_error_e - Non-zero in case of an error.
*/
static shr_error_e
dnx_mpls_dual_homing_fl_termination_init(
    int unit)
{
    uint32 nof_labels_to_term_group[] =
        { ONE_MPLS_LABEL_TO_TERMINATE, ONE_MPLS_LABEL_TO_TERMINATE, TWO_MPLS_LABELS_TO_TERMINATE,
        TWO_MPLS_LABELS_TO_TERMINATE
    };
    uint32 mpls_bos_group[] = { 1, 1, 0, 0 };
    uint32 has_cw_group[] = { FALSE, TRUE, FALSE, TRUE };

    int idx;

    SHR_FUNC_INIT_VARS(unit);

    for (idx = 0; idx < sizeof(mpls_bos_group) / sizeof(uint32); idx++)
    {
        SHR_IF_ERR_EXIT(dnx_mpls_dual_homing_fl_termination_dummy_set(unit, nof_labels_to_term_group[idx],
                                                                      has_cw_group[idx], mpls_bos_group[idx]));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_mpls_module_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** initialize MAP_FIRST_NIBBLE table */
    SHR_IF_ERR_EXIT(dnx_mpls_init_map_first_nibble(unit));

    /** initialize MAP_FWD_CODE table */
    SHR_IF_ERR_EXIT(dnx_mpls_init_map_fwd_code(unit));

    /** initialize VTT_SPECIAL_LABELS_PROFILE table */
    SHR_IF_ERR_EXIT(dnx_mpls_init_mpls_special_label_profile_map(unit));

    /** initialize EGRESS_MPLS_FHEI_TO_EES_RESOLUTION table */
    SHR_IF_ERR_EXIT(dnx_mpls_init_fhei_to_ees_resolution(unit));

    /** initialize INGRESS_MPLS_EES_TO_FHEI_RESOLUTION table */
    SHR_IF_ERR_EXIT(dnx_mpls_init_ees_to_fhei_resolution(unit));

    /** initialize INGRESS_MPLS_EES_TO_FHEI_RESOLUTION table */
    SHR_IF_ERR_EXIT(dnx_mpls_init_general_configuration(unit));

    /** initialize DUAL_HOMING ESEM-DB default entry */
    SHR_IF_ERR_EXIT(dnx_mpls_init_dual_homing_default_result(unit));

    /** initialize Flow-Label termination tables */
    if (dnx_data_mpls.general.feature_get(unit, dnx_data_mpls_general_evpn_with_fl_cw))
    {
        SHR_IF_ERR_EXIT(dnx_mpls_dual_homing_fl_termination_init(unit));
    }

    if (!(soc_sand_is_emulation_system(unit) != 0))
    {
        /** Add GAL, ELI OAM alert to MPLS_TERMINATION_SINGLE_LABEL_DB table with dummy termination profile */
        SHR_IF_ERR_EXIT(dnx_mpls_special_labels_termination_init(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_mpls_eei_php_information_fill(
    int unit,
    int qos_map_id,
    dbal_enum_value_field_model_type_e qos_model,
    dbal_enum_value_field_model_type_e ttl_model,
    dbal_enum_value_field_jr_fwd_code_e upper_layer_type,
    uint32 *full_field_val)
{
    dbal_fields_e field_in_struct_id;
    uint32 field_in_struct_encoded_val;
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    field_in_struct_id = DBAL_FIELD_EEI_QOS_MODEL_IS_PIPE;
    field_in_struct_encoded_val = (qos_model == DBAL_ENUM_FVAL_MODEL_TYPE_PIPE) ? 1 : 0;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_EEI_MPLS_POP_COMMAND, field_in_struct_id, &field_in_struct_encoded_val,
                     full_field_val));

    /** Only the LSB is used in this case */
    field_in_struct_id = DBAL_FIELD_EEI_TTL_MODEL_IS_PIPE;
    field_in_struct_encoded_val = (ttl_model == DBAL_ENUM_FVAL_MODEL_TYPE_PIPE) ? 1 : 0;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_EEI_MPLS_POP_COMMAND, field_in_struct_id, &field_in_struct_encoded_val,
                     full_field_val));

    field_in_struct_id = DBAL_FIELD_REMARK_PROFILE;
    field_in_struct_encoded_val = DNX_QOS_MAP_PROFILE_GET(qos_map_id);
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_EEI_MPLS_POP_COMMAND, field_in_struct_id, &field_in_struct_encoded_val,
                     full_field_val));

    /*
     * Jericho mode beginning
     * {
     */
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
        field_in_struct_encoded_val = upper_layer_type;
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                        (unit, DBAL_FIELD_EEI_MPLS_POP_COMMAND, DBAL_FIELD_EEI_UPPER_LAYER_PROTOCOL,
                         &field_in_struct_encoded_val, full_field_val));

    }
    /*
     * }
     * Jericho mode end
     */

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_mpls_eei_php_information_retrieve(
    int unit,
    uint32 full_field_val,
    int *qos_map_id,
    dbal_enum_value_field_model_type_e * qos_model,
    dbal_enum_value_field_model_type_e * ttl_model,
    dbal_enum_value_field_jr_fwd_code_e * upper_layer_type)
{
    uint32 field_id;
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                    (unit, DBAL_FIELD_EEI_MPLS_POP_COMMAND, DBAL_FIELD_REMARK_PROFILE, &field_id, &full_field_val));
    *qos_map_id = (int) field_id;

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                    (unit, DBAL_FIELD_EEI_MPLS_POP_COMMAND, DBAL_FIELD_EEI_QOS_MODEL_IS_PIPE, &field_id,
                     &full_field_val));
    *qos_model = field_id ? DBAL_ENUM_FVAL_MODEL_TYPE_PIPE : DBAL_ENUM_FVAL_MODEL_TYPE_UNIFORM;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                    (unit, DBAL_FIELD_EEI_MPLS_POP_COMMAND, DBAL_FIELD_EEI_TTL_MODEL_IS_PIPE, &field_id,
                     &full_field_val));
    *ttl_model = field_id ? DBAL_ENUM_FVAL_MODEL_TYPE_PIPE : DBAL_ENUM_FVAL_MODEL_TYPE_UNIFORM;

    /*
     * Jericho mode beginning
     * {
     */
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {

        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                        (unit, DBAL_FIELD_EEI_MPLS_POP_COMMAND, DBAL_FIELD_EEI_UPPER_LAYER_PROTOCOL, &field_id,
                         &full_field_val));
        *upper_layer_type = (dbal_enum_value_field_jr_fwd_code_e) field_id;
    }
    /*
     * }
     * Jericho mode end
     */

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_mpls_eei_enc_pointer_information_fill(
    int unit,
    bcm_if_t out_lif,
    uint8 host_idx,
    uint32 *full_field_val)
{
    dbal_fields_e field_in_struct_id;
    uint32 field_in_struct_encoded_val;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    field_in_struct_id = DBAL_FIELD_ENCAPSULATION_POINTER;
    /*
     * eei[15:0] = outlif[15:0],eei[21:20] = outlif[17:16]
     */
    field_in_struct_encoded_val = out_lif & UTILEX_BITS_MASK(JR_MODE_EEI_ENCAP_PTR_NOF_BITS_LSB - 1, 0);
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_EEI_ENCAPSULATION_POINTER, field_in_struct_id, &field_in_struct_encoded_val,
                     full_field_val));

    field_in_struct_id = DBAL_FIELD_ENCAPSULATION_POINTER_MSB;
    field_in_struct_encoded_val = (out_lif >> JR_MODE_EEI_ENCAP_PTR_NOF_BITS_LSB)
        & UTILEX_BITS_MASK(JR_MODE_EEI_ENCAP_PTR_NOF_BITS_MSB - 1, 0);
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_EEI_ENCAPSULATION_POINTER, field_in_struct_id, &field_in_struct_encoded_val,
                     full_field_val));

    field_in_struct_id = DBAL_FIELD_EEI_HOST_INDEX;
    field_in_struct_encoded_val = host_idx;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_EEI_ENCAPSULATION_POINTER, field_in_struct_id, &field_in_struct_encoded_val,
                     full_field_val));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_mpls_eei_enc_pointer_information_retrieve(
    int unit,
    uint32 full_field_val,
    bcm_if_t * out_lif,
    uint8 *host_idx)
{
    uint32 field_id;
    uint32 tmp;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                    (unit, DBAL_FIELD_EEI_ENCAPSULATION_POINTER, DBAL_FIELD_ENCAPSULATION_POINTER, &field_id,
                     &full_field_val));

    tmp = (int) field_id;

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                    (unit, DBAL_FIELD_EEI_ENCAPSULATION_POINTER, DBAL_FIELD_ENCAPSULATION_POINTER_MSB, &field_id,
                     &full_field_val));

    tmp |= (int) field_id << JR_MODE_EEI_ENCAP_PTR_NOF_BITS_LSB;

    BCM_L3_ITF_SET(*out_lif, BCM_L3_ITF_TYPE_LIF, tmp);

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                    (unit, DBAL_FIELD_EEI_ENCAPSULATION_POINTER, DBAL_FIELD_EEI_HOST_INDEX, &field_id,
                     &full_field_val));

    *host_idx |= (int) field_id;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_mpls_eei_push_information_fill(
    int unit,
    uint32 mpls_label,
    int push_cmd,
    uint32 *full_field_val)
{
    uint32 field_in_struct_encoded_val;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    field_in_struct_encoded_val = mpls_label;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_EEI_MPLS_PUSH_COMMAND, DBAL_FIELD_MPLS_LABEL, &field_in_struct_encoded_val,
                     full_field_val));

    field_in_struct_encoded_val = (uint32) push_cmd;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_EEI_MPLS_PUSH_COMMAND, DBAL_FIELD_PUSH_CMD_ID, &field_in_struct_encoded_val,
                     full_field_val));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_mpls_eei_push_information_retrieve(
    int unit,
    uint32 full_field_val,
    uint32 *mpls_label,
    int *push_cmd)
{
    uint32 field_val;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                    (unit, DBAL_FIELD_EEI_MPLS_PUSH_COMMAND, DBAL_FIELD_MPLS_LABEL, &field_val, &full_field_val));

    *mpls_label = field_val;

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                    (unit, DBAL_FIELD_EEI_MPLS_PUSH_COMMAND, DBAL_FIELD_PUSH_CMD_ID, &field_val, &full_field_val));

    *push_cmd = (int) field_val;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Call bcm_dnx_mpls_vpn_id_destroy to destroy a given VPN.
*   \param [in] unit         -  Relevant unit.
*   \param [in] info         -  Pointer for given VPN information.
*   \param [in] user_data    -  Pointer for extra user data.
* \return
*   shr_error_e - Non-zero in case of an error.
*/
static int
dnx_mpls_vpn_id_destroy_cb(
    int unit,
    bcm_mpls_vpn_config_t * info,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_dnx_mpls_vpn_id_destroy(unit, info->vpn));

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
 * Create MPLS tunnels in the ingress for either termination (pop), forwarding (swap) or push.
 * A tunnel can of several types: platform (single label), upstream assigned (2 adjacent labels), per-interface (label and RIF/LIF intf).
 *    \param [in] unit -Relevant unit.
 *    \param [in] info - A pointer to the struct that holds information for the ingress MPLS entry.
 *      forwarding (swap) or push - ILM flow:
 *          info.label - A label according to which the lookup will be done.
 *          info.second_label - second label used for lookup in case BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL flag is set
 *          info.ingress_if - used as second key if BCM_MPLS_SWITCH_LOOKUP_L3_INGRESS_INTF is set
 *          info.action - indicates whether the tunnel is:
 *              PHP (BCM_MPLS_SWITCH_ACTION_PHP or BCM_MPLS_SWITCH_ACTION_POP_DIRECT)
 *              SWAP (BCM_MPLS_SWITCH_ACTION_SWAP)
 *              PUSH (BCM_MPLS_SWITCH_EGRESS_ACTION_PUSH) - JR1 mode only - push MPLS label through system headers
 *              action not from ILM (BCM_MPLS_SWITCH_ACTION_NOP) - forward destination can be retrieved from FEC stage or as a result of outlif information
 *          info.egress_if - egress interface FEC-ID encoded as BCM_L3_ITF_TYPE_FEC or egress MPLS tunnel(EEDB), encoded as BCM_L3_ITF_TYPE_LIF.
 *          info.egress_label.label - new label in case of SWAP action.
 *          info.qos_map_id - Egress MPLS PHP QOS ID
 *          info.port - destination port
 *          info.mc_group - destination multicast group id
 *          info.flags - indicates tunnel configuration
 *              BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL - Key contains label and second_label.
 *              BCM_MPLS_SWITCH_LOOKUP_L3_INGRESS_INTF - indicate that ingress interface ingress_if be a part of the lookup key.
 *              BCM_MPLS_SWITCH_P2MP - Indicate that destination is multicast given by mc_group parameter.
 *              BCM_MPLS_SWITCH_NEXT_HEADER_L2 - JR1 mode only - next header is Ethernet, relevant for POP and PHP action.
 *              BCM_MPLS_SWITCH_NEXT_HEADER_IPV4 - JR1 mode only - next header is IPv4, relevant for POP and PHP action.
 *              BCM_MPLS_SWITCH_NEXT_HEADER_IPV6 - JR1 mode only - next header is IPv6, relevant for POP and PHP action.
 *              BCM_MPLS_SWITCH_REPLACE - JR1 mode only - Replace existing entry, relevant only for PUSH action.
 *          info.stat_id - Object statistics ID.
 *          info.stat_pp_profile - PP statistics ID and profile
 *
 *      termination (pop) - LER flow:
 *          info.label - A label according to which the lookup will be done.
 *          info.second_label - second label used for lookup in case BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL flag is set
 *          info.ingress_if - used as second key if BCM_MPLS_SWITCH_LOOKUP_L3_INGRESS_INTF is set
 *          info.action - termination (BCM_MPLS_SWITCH_ACTION_POP)
 *          info.tunnel_id - (in-out parameter)termination tunnel global lif id encoded as BCM_GPORT_TYPE_TUNNEL.
 *          info.vpn - VRF-ID
 *          info.failover_id - protection pointer.
 *          info.failover_tunnel_id - Set to 1 in case the tunnel is the primary one, otherwise is the secondary.
 *          info.ingress_qos_model - ingress QoS and TTL model.
 *          info.flags - indicates tunnel configuration
 *          info.qos_map_id - QOS map identifier.
 *              BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL - Key contains label and second_label.
 *              BCM_MPLS_SWITCH_LOOKUP_L3_INGRESS_INTF - indicate that ingress interface ingress_if be a part of the lookup key.
 *              BCM_MPLS_SWITCH_WITH_ID - use value from 'info.tunnel_id' for tunnel allocation.
 *              BCM_MPLS_SWITCH_REPLACE - replace preallocated termination tunnel properties
 *              BCM_MPLS_SWITCH_EXPECT_BOS - use in case BOS is expected when tunnel termination succeed. In case packet is terminated and label is not BOS, trap is invoked
 *              BCM_MPLS_SWITCH_TRAP_TTL_0 - use in case Trap is required when label is terminated and TTL is 0
 *              BCM_MPLS_SWITCH_TRAP_TTL_1 - use in case Trap is required when label is terminated and TTL is 1
 *              BCM_MPLS_SWITCH_NEXT_HEADER_L2 - terminated label is of type EVPN
 *              BCM_MPLS_SWITCH_EVPN_IML - terminated label is of type IML
 *          info.flags2 - indicates entry property(IPv4/IPv6 lookup,mLDP label)
 *              BCM_MPLS_SWITCH2_STAT_ENABLE - indicates statistics are enabled for this tunnel
 *              BCM_MPLS_SWITCH2_TERM_BUD  - Use in mLDP label termination for MPLS multicast BUD node.
 *          info.stat_id - Object statistics ID.
 *          info.stat_pp_profile - PP statistics ID and profile
 * \return
 *   Negative in case of an error. See \ref shr_error_e, for example: MPLS label is out of range.
 *   Zero in case of NO ERROR
 */
int
bcm_dnx_mpls_tunnel_switch_create(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /*
     * Verification of supported features.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_mpls_tunnel_switch_create_verify(unit, info));

    /*
     * Execution
     */
    /** if action is pop then this is TERMINATION action */
    if (info->action == BCM_MPLS_SWITCH_ACTION_POP)
    {
        /*
         * Verification.
         */
        SHR_INVOKE_VERIFY_DNX(dnx_mpls_tunnel_switch_term_add_verify(unit, info));

        if (DNX_MPLS_SWITCH_IS_EVPN(info))
        {
            SHR_IF_ERR_EXIT(dnx_mpls_evpn_term_add(unit, info));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_mpls_tunnel_switch_term_add(unit, info));
        }
    }
    /** else this is FORWARDING, adding to ILM */
    else if ((info->action == BCM_MPLS_SWITCH_ACTION_PHP) || (info->action == BCM_MPLS_SWITCH_ACTION_SWAP)
             || (info->action == BCM_MPLS_SWITCH_ACTION_POP_DIRECT)
             || (info->action == BCM_MPLS_SWITCH_EGRESS_ACTION_PUSH))
    {
        /** PHP is of type EEI */
        SHR_IF_ERR_EXIT(dnx_mpls_tunnel_switch_ilm_add(unit,
                                                       (info->stat_pp_profile != STAT_PP_PROFILE_INVALID) ?
                                                       DBAL_RESULT_TYPE_MPLS_FWD_FWD_DEST_EEI_STAT :
                                                       DBAL_RESULT_TYPE_MPLS_FWD_FWD_DEST_EEI, info));
    }
    else if (info->action == BCM_MPLS_SWITCH_ACTION_NOP)
    {
        /** action NOP is of type destination/destination + outlif/detination + EEI, according to egress_if */
        if (BCM_FORWARD_ENCAP_ID_IS_EEI(info->egress_if)
            && (BCM_FORWARD_ENCAP_ID_EEI_USAGE_GET(info->egress_if) == BCM_FORWARD_ENCAP_ID_EEI_USAGE_ENCAP_POINTER))
        {
            SHR_IF_ERR_EXIT(dnx_mpls_tunnel_switch_ilm_add(unit,
                                                           (info->stat_pp_profile != STAT_PP_PROFILE_INVALID) ?
                                                           DBAL_RESULT_TYPE_MPLS_FWD_FWD_DEST_EEI_STAT :
                                                           DBAL_RESULT_TYPE_MPLS_FWD_FWD_DEST_EEI, info));
        }
        else if (BCM_L3_ITF_TYPE_IS_LIF(info->egress_if))
        {
            SHR_IF_ERR_EXIT(dnx_mpls_tunnel_switch_ilm_add(unit,
                                                           (info->stat_pp_profile != STAT_PP_PROFILE_INVALID) ?
                                                           DBAL_RESULT_TYPE_MPLS_FWD_FWD_DEST_OUTLIF_STAT :
                                                           DBAL_RESULT_TYPE_MPLS_FWD_FWD_DEST_OUTLIF, info));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_mpls_tunnel_switch_ilm_add(unit,
                                                           (info->stat_pp_profile != STAT_PP_PROFILE_INVALID) ?
                                                           DBAL_RESULT_TYPE_MPLS_FWD_FWD_DEST_STAT :
                                                           DBAL_RESULT_TYPE_MPLS_FWD_FWD_DEST, info));
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "action %d not supported\n", info->action);
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get created MPLS tunnels in the ingress for either termination (pop), forwarding (swap) or push.
*   \param [in] unit  -  Relevant unit.
*   \param [in] info  - A pointer to the struct that holds information for the ingress MPLS entry.
*   This struct will be filled by previously save information the given info.label / info.label+info.second_label / info.label+info.ingress_if
*   info.flags - indicates entry type - single label / coupling
*   info.action - indicates whether the tunnel is termination or not
*   info.label - A label according to which the lookup will be done.
*   info.second_label - second label used for lookup in case BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL flag is set
*   info.ingress_if - used as second key if BCM_MPLS_SWITCH_LOOKUP_L3_INGRESS_INTF is set
* \remark
*   full list of 'info' parameters is in 'bcm_dnx_mpls_tunnel_switch_create'
* \return
*   Negative in case of an error. See \ref shr_error_e, for example: MPLS label is out of range.
*   Zero in case of NO ERROR
*/
int
bcm_dnx_mpls_tunnel_switch_get(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * Verification.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_mpls_tunnel_switch_get_verify(unit, info));

    /*
     * Execution
     */
    /** if action is not pop, then this is ILM action (forwarding/push) */
    if (info->action != BCM_MPLS_SWITCH_ACTION_POP)
    {
        SHR_IF_ERR_EXIT(dnx_mpls_tunnel_switch_ilm_get(unit, info));
    }
    else
    {
        /** else this is termination operation */
        int local_in_lif;
        uint8 is_evpn = 0;
        /*
         * Check if EVPN handling is needed according to the local inlif
         */
        SHR_IF_ERR_EXIT(dnx_mpls_evpn_local_in_lif_is_evpn(unit, info, &local_in_lif, &is_evpn));
        if (is_evpn)
        {
            /*
             * EVPN handling
             */
            SHR_IF_ERR_EXIT(dnx_mpls_evpn_term_get(unit, local_in_lif, info));
        }
        else
        {
            /*
             * MPLS termination
             */
            SHR_IF_ERR_EXIT(dnx_mpls_tunnel_switch_term_get(unit, local_in_lif, info));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Delete created MPLS tunnels in the ingress for either termination (pop), forwarding (swap) or push.
*   \param [in] unit  -  Relevant unit.
*   \param [in] info  -
*   A pointer to the struct that holds information for the ingress MPLS entry.
*   info.flags - indicates entry type - single label / coupling
*   info.action - indicates whether the tunnel is trmination or not
*   info.label - A label according to which the deletion will be done.
*   info.second_label - second label used for lookup in case BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL flag is set
*   info.ingress_if - used as second key if BCM_MPLS_SWITCH_LOOKUP_L3_INGRESS_INTF is set
* \return
*   Negative in case of an error. See \ref shr_error_e, for example: MPLS label is out of range.
*   Zero in case of NO ERROR
*/
int
bcm_dnx_mpls_tunnel_switch_delete(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    /*
     * Verification.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_mpls_tunnel_switch_get_verify(unit, info));

    /*
     * Execution
     */
    /** if action is not pop, then this is ILM action */
    if (info->action != BCM_MPLS_SWITCH_ACTION_POP)
    {
        SHR_IF_ERR_EXIT(dnx_mpls_tunnel_switch_ilm_delete(unit, info));
    }
    else
    {
        /** else this is termination operation */
        int local_in_lif;
        uint8 is_evpn = 0;
        /*
         * Check if EVPN handling is needed according to the local inlif
         */
        SHR_IF_ERR_EXIT(dnx_mpls_evpn_local_in_lif_is_evpn(unit, info, &local_in_lif, &is_evpn));
        if (is_evpn)
        {
            /*
             * EVPN handling
             */
            SHR_IF_ERR_EXIT(dnx_mpls_evpn_term_delete(unit, local_in_lif));
        }
        else
        {
            /*
             * MPLS termination
             */
            SHR_IF_ERR_EXIT(dnx_mpls_tunnel_switch_term_delete(unit, local_in_lif, info));
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Delete all created MPLS tunnels in the ingress for either termination (pop), forwarding (swap) or push.
* \par DIRECT INPUT:
*   \param [in] unit  -  Relevant unit.
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   \retval Negative in case of an error. See \ref shr_error_e, for example: MPLS label is out of range.
*   \retval Zero in case of NO ERROR
* \par INDIRECT OUTPUT:
*   * Delete all INGRESS MPLS Hardware.
*   * And delete all MPLS-ILM.
* \remark
*   * None
* \see
*   * None
*/
int
bcm_dnx_mpls_tunnel_switch_delete_all(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * Execution
     */
    /** delete all ILM actions */
    SHR_IF_ERR_EXIT(dnx_mpls_tunnel_switch_ilm_delete_all(unit));

    /** delete all termination operations */
    SHR_IF_ERR_EXIT(dnx_mpls_tunnel_switch_term_delete_all(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Traverse all MPLS tunnels in the ingress for either termination (pop), forwarding (swap) or push and run a callback function
 * provided by the user for each one.
 * \param [in] unit - The unit number.
 * \param [in] cb   - the name of the callback function,
 *          it receives the value of the user_data variable and
 *          the key and result values for each found entry
 * \param [in] user_data - user data that will be sent to the callback function
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
int
bcm_dnx_mpls_tunnel_switch_traverse(
    int unit,
    bcm_mpls_tunnel_switch_traverse_cb cb,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    if (cb == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No callback function has been provided to the traverse.\n");
    }

     /** ILM entries */
    SHR_IF_ERR_EXIT(dnx_mpls_tunnel_switch_ilm_traverse(unit, cb, user_data));

    /** termination entries */
    SHR_IF_ERR_EXIT(dnx_mpls_tunnel_switch_term_traverse(unit, cb, user_data));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *      Updating or creating MPLS VPN with configration.
 * \param [in] unit -
 *      Relevant unit.
 * \param [in, out] info - A pointer to the struct that holds information for the mpls vpn instance. \n
 *      [in,out] info.vpn - virtual switch instance (VSI). \n
 *      [in] info.broadcast_group - MC group for broadcast \n
 *      [in] info.unknown_multicast_group - MC group for unknown MC \n
 *      [in] info.unknown_unicast_group - MC group for unknown UC \n
 *      [in] info.flags - BCM_MPLS_VPN_XXX flags \n
 *          BCM_MPLS_VPN_VPLS       -  indicate this is vpls vpn \n
 *          BCM_MPLS_VPN_REPLACE    -  update an existing VPN \n
 *          BCM_MPLS_VPN_WITH_ID    -  WITH-ID on the VPN ID (otherwise it is allocated) \n
 * \return
 *      \retval Zero in case of NO ERROR.
 *      \retval Negative in case of an error, see \ref shr_error_e.
 * \remark
 *      * Allocated VSI if not updated.
 *      * Write SW table to keep VSI usage.
 *      * Configure VSI of 3 MC group with same value.
 * \see
 *      * bcm_mpls_vpn_config_t
 */
int
bcm_dnx_mpls_vpn_id_create(
    int unit,
    bcm_mpls_vpn_config_t * info)
{
    int vpn_id;
    uint8 update, with_id;
    uint32 alloc_flags;
    uint8 replaced;
    int rv;

    bcm_vlan_control_vlan_t vlan_control;

    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_mpls_vpn_create_verify(unit, info));

    update = (info->flags & BCM_MPLS_VPN_REPLACE) ? TRUE : FALSE;
    with_id = (info->flags & BCM_MPLS_VPN_WITH_ID) ? TRUE : FALSE;
    vpn_id = DNX_VPN_ID_GET(info->vpn);

    /** New VPN, allocate VPN ID */
    if (!update)
    {
        alloc_flags = 0;
        if (with_id)
        {
            alloc_flags |= DNX_ALGO_RES_ALLOCATE_WITH_ID;
        }

        SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_alloc(unit, alloc_flags, _bcmDnxVsiTypeMpls, &vpn_id, &replaced));
    }

    /** Set VPN info */
    DNX_VPN_ID_SET(info->vpn, vpn_id);
    bcm_vlan_control_vlan_t_init(&vlan_control);
    vlan_control.broadcast_group = info->broadcast_group;
    vlan_control.unknown_multicast_group = info->unknown_multicast_group;
    vlan_control.unknown_unicast_group = info->unknown_unicast_group;
    vlan_control.forwarding_vlan = info->vpn;
    if (dnx_data_l2.general.l2_learn_limit_mode_get(unit) == LEARN_LIMIT_MODE_VLAN)
    {
        vlan_control.aging_cycles = DEFAULT_NOF_AGE_CYCLES;
    }

    rv = bcm_dnx_vlan_control_vlan_set(unit, info->vpn, vlan_control);
    if (SHR_FAILURE(rv))
    {
        SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_dealloc(unit, _bcmDnxVsiTypeMpls, vpn_id));
    }
    SHR_IF_ERR_EXIT(rv);

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *      Destroy a MPLS VPN.
 * \param [in] unit -
 *      Relevant unit.
 * \param [in] vpn -
 *      VPN number
 * \return
 *      \retval Zero in case of NO ERROR.
 *      \retval Negative in case of an error, see \ref shr_error_e.
 * \remark
 *      * Delete all MPLS port on this VSI,
 *      * Free VSI usage resource on MPLS property.
 *      * Free VSI resource if only used by MPLS VPN
 *      * Re-init VSI table if only used by MPLS VPN
 * \see
 *      * None
 */
int
bcm_dnx_mpls_vpn_id_destroy(
    int unit,
    bcm_vpn_t vpn)
{
    int vpn_id;
    uint8 is_vsi_used;
    bcm_mpls_port_traverse_info_t traverse_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** check parameters */
    SHR_IF_ERR_EXIT(dnx_mpls_vpn_get_and_destroy_verify(unit, vpn));

    /** Delete all the mpls ports on this VPN */
    sal_memset(&traverse_info, 0, sizeof(traverse_info));
    traverse_info.vpn = vpn;
    traverse_info.traverse_flags = BCM_MPLS_TRAVERSE_DELETE;
    traverse_info.flags2 = (vpn != 0) ? BCM_MPLS_PORT2_INGRESS_ONLY : 0;
    SHR_IF_ERR_EXIT(bcm_dnx_mpls_port_traverse(unit, traverse_info, NULL, NULL));

    vpn_id = DNX_VPN_ID_GET(vpn);

    /** check if VSI used by only by MPLS */
    is_vsi_used = 0;
    SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_check_except_type(unit, vpn_id, _bcmDnxVsiTypeMpls, &is_vsi_used));

    /** free vsi resource */
    SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_dealloc(unit, _bcmDnxVsiTypeMpls, vpn_id));

    /** re-initialize vswitch instance, clear MC group */
    if (!is_vsi_used)
    {
        SHR_IF_ERR_EXIT(dnx_vsi_table_clear_set(unit, vpn_id));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *      Destroy all MPLS VPN.
 * \param [in] unit -
 *      Relevant unit.
 * \return
 *      \retval Zero in case of NO ERROR.
 *      \retval Negative in case of an error, see \ref shr_error_e.
 * \remark
 *      * Traversing all VSI, destroy it if it's MPLS VPN.
 * \see
 *      * None
 */
int
bcm_dnx_mpls_vpn_id_destroy_all(
    int unit)
{
    bcm_mpls_port_traverse_info_t traverse_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Delete any MPLS ports on the cross connect VPN(0) */
    sal_memset(&traverse_info, 0, sizeof(traverse_info));
    traverse_info.vpn = 0;
    traverse_info.traverse_flags = BCM_MPLS_TRAVERSE_DELETE;
    SHR_IF_ERR_EXIT(bcm_dnx_mpls_port_traverse(unit, traverse_info, NULL, NULL));

    /** Destroy other valid VPNs by traverse. */
    SHR_IF_ERR_EXIT(bcm_dnx_mpls_vpn_traverse(unit, dnx_mpls_vpn_id_destroy_cb, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *      Get an existing MPLS VPN information.
 * \param [in] unit -
 *      Relevant unit.
 * \param [in] vpn -
 *      vpn id.
 * \param [in, out] info - A pointer to the struct that will holds information for the mpls vpn instance. \n
 *      [in,out] info.vpn - VPN ID, set with input VPN. \n
 *      [in,out] info.broadcast_group - MC group for broadcast \n
 *      [in,out] info.unknown_multicast_group - MC group for unknown MC \n
 *      [in,out] info.unknown_unicast_group - MC group for unknown UC \n
 *      [in,out] info.flags - BCM_MPLS_VPN_VPLS \n
 *
 * \return
 *      \retval Zero in case of NO ERROR.
 *      \retval Negative in case of an error, see \ref shr_error_e.
 * \remark
 *      * Get 3 MC group from VSI table.
 * \see
 *      * bcm_mpls_vpn_config_t
 */
int
bcm_dnx_mpls_vpn_id_get(
    int unit,
    bcm_vpn_t vpn,
    bcm_mpls_vpn_config_t * info)
{
    bcm_vlan_control_vlan_t vlan_control;
    int vpn_id;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** check for input */
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    /** clear information */
    bcm_mpls_vpn_config_t_init(info);
    info->flags = BCM_MPLS_VPN_VPLS;
    info->vpn = vpn;

    SHR_IF_ERR_EXIT(dnx_mpls_vpn_get_and_destroy_verify(unit, vpn));

    vpn_id = DNX_VPN_ID_GET(info->vpn);
    bcm_vlan_control_vlan_t_init(&vlan_control);
    SHR_IF_ERR_EXIT(bcm_dnx_vlan_control_vlan_get(unit, vpn_id, &vlan_control));

    info->unknown_unicast_group = vlan_control.unknown_unicast_group;
    info->unknown_multicast_group = vlan_control.unknown_multicast_group;
    info->broadcast_group = vlan_control.broadcast_group;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *      Traverse all MPLS VPN and run callback function with user data for each MPSL VPN.
 * \param [in] unit -
 *      Relevant unit.
 * \param [in] cb -
 *      call back function pointer to run.
 * \param [in] user_data -
 *      Pointer for user data which callback function will execute as parameter.
 * \return
 *      \retval Zero in case of NO ERROR.
 *      \retval Negative in case of an error, see \ref shr_error_e.
 * \remark
 *      * Get VPN configration if it's MPLS VPN.
 *      * Runing callback function with user_data and configration info.
 * \see
 *      * bcm_mpls_vpn_config_t
 *      * bcm_mpls_vpn_traverse_cb
 */

int
bcm_dnx_mpls_vpn_traverse(
    int unit,
    bcm_mpls_vpn_traverse_cb cb,
    void *user_data)
{
    int vpn;
    uint8 is_vsi_allocated;
    uint8 is_vsi_usage_mpls;
    bcm_mpls_vpn_config_t vpn_info;
    int nof_vsis;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));

    for (vpn = BCM_VLAN_DEFAULT; vpn < nof_vsis; vpn++)
    {
        SHR_IF_ERR_EXIT(vlan_db.vsi.is_allocated(unit, vpn, &is_vsi_allocated));
        if (is_vsi_allocated)
        {
            SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_per_type_get(unit, vpn, _bcmDnxVsiTypeMpls, &is_vsi_usage_mpls));

            if (is_vsi_usage_mpls)
            {
                SHR_IF_ERR_EXIT(bcm_dnx_mpls_vpn_id_get(unit, vpn, &vpn_info));
                cb(unit, &vpn_info, user_data);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

#if defined(INCLUDE_L3)
/* { */
/*
 * Procedures related to handling of special MPLS labels.
 * {
 */
/**
 * \brief
 *   Add an entry to a table related to a special MPLS label type.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] label_type -
 *   Enumeration of type 'bcm_mpls_special_label_type_t'.
 *   Currently, only 'bcmMplsSpecialLabelTypeFrr' (MPLS Fast Reroute) is acceptable.
 * \param [in] label_info -
 *   Structure of type 'bcm_mpls_special_label_t'. \n
 *   Usage may depend on 'label_type'.
 *   Currently, for 'bcmMplsSpecialLabelTypeFrr', only the 'label_value' is used
 *   and it stands for "mpls label" that indicates passage of packet through a
 *   'backup path' router and needs to be removed without any further action.
 * \return
 *   Negative in case of an error. See \ref shr_error_e, for example: 'MPLS label is
 *   out of range' or 'no space on tcam'.
 *   Zero in case of NO ERROR
 * \see
 *   * bcm_dnx_mpls_frr_label_identifier_get()
 */
int
bcm_dnx_mpls_special_label_identifier_add(
    int unit,
    bcm_mpls_special_label_type_t label_type,
    bcm_mpls_special_label_t label_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /*
     * Here add validity checks (e.g., range of label_type).
     */
    DNX_MPLS_SPECIAL_LABEL_VERIFY(unit, label_type);
    /*
     * Here call procedures specific to each type.
     * For example, for FRR:
     *     dnx_mpls_frr_label_identifier_add()
     */
    switch (label_type)
    {
        case bcmMplsSpecialLabelTypeFrr:
        {
            SHR_IF_ERR_EXIT(dnx_mpls_frr_label_identifier_add(unit, &label_info));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "This label_type (%d) is currently not supported\r\n", label_type);
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Get info on a specified entry to a table related to a special MPLS label type.
 *   Note that this info may only be 'exists/does-not-exist'.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] label_type -
 *   Enumeration of type 'bcm_mpls_special_label_type_t'.
 *   Currently, only 'bcmMplsSpecialLabelTypeFrr' (MPLS Fast Reroute) is acceptable.
 * \param [in] label_info_p -
 *   Pointer tp a structure of type 'bcm_mpls_special_label_t'. \n
 *   Usage may depend on 'label_type'.
 *   Currently, for 'bcmMplsSpecialLabelTypeFrr', only the 'label_value' is used
 *   and it stands for "mpls label" that indicates passage of packet through a
 *   'backup path' router and needs to be removed without any further action.
 *   This procedure, then, seaches for the entry as per 'label_value'. If
 *   not found, the corresponding error is returned (but no error log is ejecetd).
 *   Otherwise, this procedure returns with no error to indicate such entry exists.
 * \return
 *   Negative in case of an error. See \ref shr_error_e, for example: 'MPLS label is
 *     out of range' or 'no space on tcam'.
 *     However, if returned error is _SHR_E_NOT_FOUND, no error log is ejected and
 *     it is up to the caller to decide what to do. (Note that this may be q quite
 *     standard operation and not a real error at all)
 *   Zero in case of NO ERROR
 * \see
 *   * bcm_dnx_mpls_frr_label_identifier_add()
 *   * bcm_dnx_mpls_frr_label_identifier_delete()
 */
int
bcm_dnx_mpls_special_label_identifier_get(
    int unit,
    bcm_mpls_special_label_type_t label_type,
    bcm_mpls_special_label_t * label_info_p)
{
    shr_error_e shr_error;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /*
     * Here add validity checks (e.g., range of label_type).
     */
    SHR_NULL_CHECK(label_info_p, _SHR_E_PARAM, "label_info_p");
    DNX_MPLS_SPECIAL_LABEL_VERIFY(unit, label_type);
    /*
     * Here call procedures specific to each type.
     * For example, for FRR:
     *     dnx_mpls_frr_label_identifier_get()
     */
    switch (label_type)
    {
        case bcmMplsSpecialLabelTypeFrr:
        {
            shr_error = dnx_mpls_frr_label_identifier_get(unit, label_info_p);
            if (shr_error != _SHR_E_NONE)
            {
                if (shr_error == _SHR_E_NOT_FOUND)
                {
                    SHR_SET_CURRENT_ERR(shr_error);
                    SHR_EXIT();
                }
                else
                {
                    SHR_IF_ERR_EXIT(shr_error);
                }
            }
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "This label_type (%d) is currently not supported\r\n", label_type);
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Delete a specified entry from a table related to a special MPLS label type.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] label_type -
 *   Enumeration of type 'bcm_mpls_special_label_type_t'.
 *   Currently, only 'bcmMplsSpecialLabelTypeFrr' (MPLS Fast Reroute) is acceptable.
 * \param [in] label_info -
 *   Structure of type 'bcm_mpls_special_label_t'. \n
 *   Usage may depend on 'label_type'.
 *   Currently, for 'bcmMplsSpecialLabelTypeFrr', only the 'label_value' is used
 *   and it stands for "mpls label" that indicates passage of packet through a
 *   'backup path' router and needs to be removed without any further action.
 *   This procedure deletes, then, the entry specified by 'label_value'. If no
 *   such entry exists, an error log is ejected.
 *   Otherwise, this procedure returns with no error indication.
 * \return
 *   Negative in case of an error. See \ref shr_error_e, for example: 'MPLS label is
 *     out of range'.
 *     However, if returned error is _SHR_E_NOT_FOUND, no error log is ejected and
 *     it is up to the caller to decide what to do. (Note that this may be q quite
 *     standard operation and not a real error at all)
 *   Zero in case of NO ERROR
 * \see
 *   * bcm_dnx_mpls_frr_label_identifier_add()
 *   * bcm_dnx_mpls_frr_label_identifier_delete()
 */
int
bcm_dnx_mpls_special_label_identifier_delete(
    int unit,
    bcm_mpls_special_label_type_t label_type,
    bcm_mpls_special_label_t label_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /*
     * Here add validity checks (e.g., range of label_type).
     */
    DNX_MPLS_SPECIAL_LABEL_VERIFY(unit, label_type);
    /*
     * Here call procedures specific to each type.
     * For example, for FRR:
     *     dnx_mpls_frr_label_identifier_delete()
     */
    switch (label_type)
    {
        case bcmMplsSpecialLabelTypeFrr:
        {
            SHR_IF_ERR_EXIT(dnx_mpls_frr_label_identifier_delete(unit, &label_info));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "This label_type (%d) is currently not supported\r\n", label_type);
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Delete all entries all tables related to special MPLS label type
 * \param [in] unit -
 *   Identifier of HW platform.
 * \return
 *   Negative in case of an error. See \ref shr_error_e
 *   Zero in case of NO ERROR
 * \see
 *   * bcm_dnx_mpls_frr_label_identifier_add()
 *   * bcm_dnx_mpls_frr_label_identifier_delete()
 */
int
bcm_dnx_mpls_special_label_identifier_delete_all(
    int unit)
{
    uint32 entry_handle_id;
    int is_end;
    uint32 entry_access_id;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MPLS_FRR_TCAM_DB, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));

        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(dnx_field_entry_access_id_destroy(unit, DBAL_TABLE_MPLS_FRR_TCAM_DB, entry_access_id));

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Traverse over all entries all tables related to special MPLS label type
 *   for which 'traverse' is implemented.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] cb -
 *   Pointer to a call-back procedure of type 'bcm_mpls_special_label_identifier_traverse_cb'.
 *   This procedure will invoke this call-back per each entry of each of
 *   the implemented tables.
 * \param [in] user_data -
 *   General pointer. \n
 *   Caller may point to any type of data and it will be transferred to
 *   the call-back procedure. May be NULL to indicate 'no user data'
 * \return
 *   Negative in case of an error. See \ref shr_error_e
 *   Zero in case of NO ERROR
 * \see
 *   * bcm_dnx_mpls_frr_label_identifier_add()
 *   * bcm_dnx_mpls_frr_label_identifier_delete()
 */
int
bcm_dnx_mpls_special_label_identifier_traverse(
    int unit,
    bcm_mpls_special_label_identifier_traverse_cb cb,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_NULL_CHECK(cb, _SHR_E_PARAM, "cb - CallBack procedure");
    /*
     * Now invoke all known 'specific' traverse procedures.
     * For now, only 'bcmMplsSpecialLabelTypeFrr' is implemented.
     */
    SHR_IF_ERR_EXIT(dnx_mpls_frr_label_identifier_traverse(unit, cb, user_data));
    /*
     * Here add other 'specific' traverse procedures.
     */
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the value of Alternate Marking Special Label input.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] special_label_value - Special label value
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   None
 * \see
 *   * None
 */
static shr_error_e
dnx_mpls_alternate_marking_special_label_set_verify(
    int unit,
    uint32 special_label_value)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Check that special label is in the range 0-15
     */
    if ((special_label_value > 15) && (special_label_value > DNX_MPLS_ALTERNATE_MARKING_SL_DEFAULT))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "special label value should be in the range 0-15. Given value: = 0x%08X",
                     special_label_value);
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * see mpls.h file
 */
shr_error_e
dnx_mpls_alternate_marking_special_label_set(
    int unit,
    uint32 special_label_value)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_mpls_alternate_marking_special_label_set_verify(unit, special_label_value));

    /*
     * Write to virtual register of special label
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_IOAM_MPLS_SPECIAL_LABEL_INDICATION, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IOAM_MPLS_SPECIAL_LABEL_INDICATION, INST_SINGLE,
                                 special_label_value);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /*
     * Write to special label termination table.
     * Alternate Marking profile is sumilar to ELI, thus using the same profile for both
     */
    SHR_IF_ERR_EXIT(dnx_mpls_init_mpls_special_label_profile_map_single_entry
                    (unit, (special_label_value & 0xF), 0, DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_ELI));
    SHR_IF_ERR_EXIT(dnx_mpls_init_mpls_special_label_profile_map_single_entry
                    (unit, (special_label_value & 0xF), 1, DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_ELI));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * see mpls.h file
 */
shr_error_e
dnx_mpls_alternate_marking_special_label_get(
    int unit,
    uint32 *special_label_value)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write to virtual register of special label
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_IOAM_MPLS_SPECIAL_LABEL_INDICATION, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IOAM_MPLS_SPECIAL_LABEL_INDICATION, INST_SINGLE,
                               special_label_value);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * }
 */
/* } */
#endif /* INCLUDE_L3 */
/*
 * End of APIs
 * }
 */

/*
 * Utilities
 * {
 */

/*
 * }
 * End of utilities
 */
