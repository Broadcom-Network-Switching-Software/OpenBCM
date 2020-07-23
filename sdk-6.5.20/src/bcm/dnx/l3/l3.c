/** \file l3.c
 *
 *  l3 procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_L3
/*
 * Include files.
 * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_elk.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <bcm/types.h>
#include <bcm/l3.h>
#include <bcm_int/dnx/l3/l3_arp.h>
#include <bcm_int/dnx/l3/l3_fec.h>
#include <bcm_int/dnx/l3/l3_vrrp.h>
#include <bcm_int/dnx/l3/l3_ecmp_vip.h>
#include <bcm_int/dnx/l3/l3.h>
#include <bcm_int/dnx/mpls/mpls.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/util.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/stat/stat_pp.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/*
 * }
 */

/*
 * MACROs
 * {
 */
#define L3_FEC_ID_FROM_SUPER_FEC_INSTANCE(sup_fec_id, instance) ((sup_fec_id << 1) | instance)
/*
 * }
 */

/*
 * See .h file
 */
shr_error_e
dnx_l3_module_init(
    int unit)
{
    uint32 entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;
    uint32 ecmp_enabled = 1;
    int range_ind, intf_ind, lpm_ind;
    int start_range_def, start_range_non_def;
    int end_range_def, end_range_non_def;
    int strength_def, strength_non_def;
    uint8 is_image_standard_1 = FALSE;
    uint8 update_sa_proc_v4 = 0;
    uint8 update_sa_proc_v6 = 0;
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    /*
     * Set the KAPS prefix length and LPM profile/devices that support strength profiles/ to entry strength mapping table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KAPS_STRENGTH_MAPPING, &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_image_standard_1));
    if (is_image_standard_1)
    {
        /*
         * Going over all the lpm profiles.Currently only profile 0 is used.
         */
        for (lpm_ind = 0; lpm_ind < dnx_data_l3.fwd.nof_lpm_profiles_get(unit); lpm_ind++)
        {
            /*
             * Going over all the KAPS interfaces
             */
            for (intf_ind = DBAL_ENUM_FVAL_KAPS_INTERFACE_INTERFACE_0; intf_ind < DBAL_NOF_ENUM_KAPS_INTERFACE_VALUES;
                 intf_ind++)
            {
                start_range_non_def =
                    dnx_data_l3.fwd.lpm_profile_to_entry_strength_get(unit, lpm_ind, intf_ind)->prefix_len_non_def[0];
                start_range_def =
                    dnx_data_l3.fwd.lpm_profile_to_entry_strength_get(unit, lpm_ind, intf_ind)->prefix_len_def[0];
                /**Going over all the prefix length ranges and setting the entry_strength*/
                for (range_ind = 0; range_ind < dnx_data_l3.fwd.nof_prefix_ranges_get(unit); range_ind++)
                {
                    end_range_non_def =
                        dnx_data_l3.fwd.lpm_profile_to_entry_strength_get(unit, lpm_ind,
                                                                          intf_ind)->prefix_len_non_def[range_ind + 1];
                    end_range_def =
                        dnx_data_l3.fwd.lpm_profile_to_entry_strength_get(unit, lpm_ind,
                                                                          intf_ind)->prefix_len_def[range_ind + 1];
                    strength_non_def =
                        dnx_data_l3.fwd.lpm_profile_to_entry_strength_get(unit, lpm_ind,
                                                                          intf_ind)->entry_strength_non_def[range_ind];
                    strength_def =
                        dnx_data_l3.fwd.lpm_profile_to_entry_strength_get(unit, lpm_ind,
                                                                          intf_ind)->entry_strength_def[range_ind];
                    /*
                     * Set the entry strength only if the prefix length range is valid range
                     */
                    if (end_range_def != -1 || end_range_non_def != -1)
                    {
                        /** set interfaces.*/
                        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KAPS_INTERFACE, intf_ind);
                        /** set profile*/
                        if (dnx_data_l3.fwd.lpm_strength_profile_support_get(unit))
                        {
                            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LPM_PROFILE, lpm_ind);
                        }
                        if (end_range_def != -1)
                        {
                            dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PREFIX_LENGTH,
                                                             start_range_def, end_range_def);
                            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STRENGTH_DEFAULT,
                                                         INST_SINGLE, strength_def);
                            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
                        }
                        if (end_range_non_def != -1)
                        {
                            dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PREFIX_LENGTH,
                                                             start_range_non_def, end_range_non_def);
                            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STRENGTH_NON_DEFAULT,
                                                         INST_SINGLE, strength_non_def);
                            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
                        }
                    }
                    start_range_def = end_range_def + 1;
                    start_range_non_def = end_range_non_def + 1;
                }
            }
        }
    }

    /** Set the ENABLE_40K_ECMP to enabled by default */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_L3_GENERAL_CONFIGURATION, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_40K_ECMP_RANGE, INST_SINGLE, ecmp_enabled);
    /** Set the KAPS interfaces default to enabled */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_KAPS_DEFAULT, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ENABLE_KAPS_DEFAULT_ALL_ENABLED);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** init vrrp (multiple my mac) */
    SHR_IF_ERR_EXIT(dnx_l3_vrrp_init(unit));

    /** Update SA procedure enabler */
    if (!dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4))
    {
        update_sa_proc_v4 = 1;
    }
    if (!dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv6))
    {
        update_sa_proc_v6 = 1;
    }
    if (update_sa_proc_v4 || update_sa_proc_v6)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PEMLA_FWD2_SA_PROCEDURE_RESOLUTION, entry_handle_id));
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_FWD2___IPV4___PRIVATE_UC, INST_SINGLE,
                                    update_sa_proc_v4 ? 0 : 1);
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_FWD2___IPV6___PRIVATE_UC, INST_SINGLE,
                                    update_sa_proc_v6 ? 0 : 1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** init sllb */
    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_vip_init(unit));

    /**FEC arr prefix default is: pop command prefix is 8 ,and swap command prefix is 9
     * Q2A and above use it as FHEI mpls command, and it is no meaning in JR2.
     * So, in JR2 mode, we set it as : pop command prefix is 1 ,and swap command prefix is 2
     */
    if (system_headers_mode != DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
        uint32 eei_identifier_offset;
        uint32 prefix;

        eei_identifier_offset = 20;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FEC_ARR_PREFIX, entry_handle_id));
        dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_FEC_TYPE, DBAL_ENUM_FVAL_FEC_TYPE_EEI_POP_COMMAND);
        prefix = DNX_EEI_IDENTIFIER_POP << eei_identifier_offset;
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PREFIX_VALUE, INST_SINGLE, prefix);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_FEC_TYPE, DBAL_ENUM_FVAL_FEC_TYPE_EEI_SWAP_COMMAND);
        prefix = DNX_EEI_IDENTIFIER_SWAP << eei_identifier_offset;
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PREFIX_VALUE, INST_SINGLE, prefix);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* Verfity the L3 egress parameter according to the getting requirement
*
*   \param [in] unit       -  Relevant unit.
*   \param [in] intf  -    egress object ID .
*   \param [in] egr  -    structure holding the egress object .

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
dnx_l3_egress_get_verify(
    int unit,
    bcm_if_t intf,
    bcm_l3_egress_t * egr)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(egr, _SHR_E_PARAM, "egr");

    if (!BCM_L3_ITF_TYPE_IS_LIF(intf) && !BCM_L3_ITF_TYPE_IS_FEC(intf))
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "Valid L3 egress object should be LIF or FEC, intf=%x", intf);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Verfity the L3 egress parameter according to the creation requirement
*
*   \param [in] unit       -  Relevant unit.
*   \param [in] flags  -    Similar to bcm_l3_egress_create api input
*   \param [in] egr  -    structure holding the egress object .
*   \param [in] if_id  -    egress object ID .
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
dnx_l3_egress_create_verify(
    int unit,
    uint32 flags,
    bcm_l3_egress_t * egr,
    bcm_if_t * if_id)
{
    uint32 supported_flags;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(egr, _SHR_E_PARAM, "egr");

    supported_flags =
        BCM_L3_INGRESS_ONLY | BCM_L3_EGRESS_ONLY | BCM_L3_REPLACE | BCM_L3_WITH_ID | BCM_L3_KEEP_DSTMAC |
        BCM_L3_KEEP_VLAN;

    if (_SHR_IS_FLAG_SET(flags, ~supported_flags))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid 'flags' provided - flags = %x, supported_flags = %x", flags,
                     supported_flags);
    }

    /** Currently Ingress + Egress is not supported */
    if (_SHR_IS_FLAG_SET(flags, BCM_L3_INGRESS_ONLY) && _SHR_IS_FLAG_SET(flags, BCM_L3_EGRESS_ONLY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Ingress + Egress is not supported in the same call, instead call to API twice: egress only and then ingress only.");
    }

    /** The if_id field isn't required for the egress case (although it is mention in the UM that the if_id should be NULL in the egress case this won't be enforced) */
    if (_SHR_IS_FLAG_SET(flags, BCM_L3_INGRESS_ONLY))
    {
        SHR_NULL_CHECK(if_id, _SHR_E_PARAM, "if_id");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Verfity the L3 egress parameter according to the deleting requirement
*
*   \param [in] unit       -  Relevant unit.
*   \param [in] intf  -    egress object ID .
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
dnx_l3_egress_destroy_verify(
    int unit,
    bcm_if_t intf)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!BCM_L3_ITF_TYPE_IS_LIF(intf) && !BCM_L3_ITF_TYPE_IS_FEC(intf))
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "Valid L3 egress object should be LIF or FEC");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Creation either FEC (BCM_L3_INGRESS_ONLY flag) or ARP (BCM_L3_EGRESS_ONLY flag). \n
 *
 * \param [in] unit -
 *   Relevant unit.
 * \param [in] flags - BCM_L3_XXX flags \n
 *   For FEC: \n
 *        BCM_L3_WITH_ID - WITH-ID option \n
 *        BCM_L3_REPLACE - Replace functionality, must be called when object already created \n
 *        BCM_L3_INGRESS_ONLY - must be set. \n
 *        BCM_L3_ENCAP_SPACE_OPTIMIZED - JR1 system header mode only, create the EEI+Dest type FEC(Format C in JR1). \n
 *   For ARP: \n
 *        BCM_L3_REPLACE - Replace functionality, must be called when object already created \n
 *        BCM_L3_EGRESS_ONLY - must be set \n
 *        BCM_L3_KEEP_DSTMAC, BCM_L3_KEEP_VLAN - do not edit ARP, only update next pointer of intf \n
 * \param [in,out] egr - The incoming egr object structure \n
 *   For FEC: \n
 *   [in] egr.port - Destination \n
 *   [in] egr.intf - Global-LIF1(EEI when using EEI+Dest format) \n
 *   [in] egr.encap_id - Global-LIF2 \n
 *   [in] egr.mpls_action - BCM_MPLS_EGRESS_ACTION_XXX \n
 *        BCM_MPLS_EGRESS_ACTION_PHP - action PHP      \n
 *        BCM_MPLS_EGRESS_ACTION_SWAP - action SWAP    \n
 *   [in] egr.qos_map_id - The Remark QOS ID (in case of MPLS PHP) \n
 *   [in] egr.mpls_label - The MPLS label in case of swap          \n
 *   [in] egr.flags - BCM_L3_XXX flags \n
 *        BCM_L3_2ND_HIERARCHY  - 2nd hier \n
 *        BCM_L3_3RD_HIERARCHY  - 3rd hier \n
 *        BCM_L3_MC_RPF_EXPLICIT  - MC RPF explicit \n
 *        BCM_L3_MC_RPF_SIP_BASE  - MC RPF SIP base \n
 *   [in] egr.flags2 - BCM_L3_XXX flags2   \n
 *        BCM_L3_FLAGS2_SKIP_HIT_CLEAR -   \n
 *          Skip hit clear when creating a FEC to enhance time performance. \n
 *   For ARP: \n
 *   [in, out] egr.encap_id -
 *        The allocated ARP ID.
 *        As input:
 *          ID != 0 means, this specific ID needs to be allocated (= WITH_ID).        \n
 *          In that case, it may or may not be encoded as 'lif' (but see 'as output') \n
 *        As output:
 *          This value is encoded as 'lif' and may also be encoded as 'egr_pointed'.  \n
 *          See 'egr.flags2' below. \n
 *   [in] egr.mac_addr - Next hop MAC address \n
 *   [in] egr.src_mac_addr - Source address   \n
 *   [in] egr.qos_map_id - The Remark QOS ID  \n
 *   [in] egr.vlan - The provided ETH-RIF ID. \n
 *   [in] egr.intf - Tunnel that should be pointing on the created ARP.
 *                   This field will not be returned on bcm_l3_egress_get call in case of ARP (BCM_L3_EGRESS_ONLY) \n
 *   [in] egr.vlan_port_id - Gport representing vlan translation port next entry (optional). \n
 *   [in] egr.flags - BCM_L3_XXX flags \n
 *        BCM_L3_NATIVE_ENCAP - \n
 *          ARP is required for Native Ethernet layer. Otherwise Link layer. \n
 *   [in] egr.flags2 - BCM_L3_XXX flags2 \n
 *        uint32 used as a bitmap.
 *        BCM_L3_FLAGS2_VIRTUAL_EGRESS_POINTED - \n
 *          If set then 'egr.encap_id', both 'in' and 'out', is an identifier of an 'egr_pointed' \n
 *          object representing a local egress lif. As 'i'n, it nay, or may not, be encoded as    \n
 *          'lif'. As 'out' it comes encoded as 'LIF' and as 'VIRTUAL_EGRESS_POINTED'.            \n
 *          \see _SHR_L3_ITF_TYPE_LIF
 *          \see _SHR_L3_ITF_TYPE_IS_LIF
 *          \see _SHR_L3_ITF_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED
 * \param [in,out] if_id - The incoming/outgoing egr object ID \n
 *   used for FEC only, provide the object ID (in case of with-ID) or the returned value upon creation.
 *
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 * \remark
 *
 * The supported result-types in FEC table:
 *
 * result-type                      | params decision                              | description
 * ---------------------------------| ---------------------------------------------|------------------------------------------
 * FEC_ENTRY_NO_PROTECTION_NO_STATS | default                                      |Default format
 * FEC_ENTRY_NO_PROTECTION_W_STATS  | (not supported yet)                          |In case stats required
 * FEC_ENTRY_W_PROTECTION_NO_STATS  | failover_id is valid,                        | in case protection required
 * FEC_ENTRY_W_PROTECTION_W_STATS   | failover_id is valid and (not supported yet) | in case both protection and stat required
 *
 * Note: FEC entry is part of Super-FEC and as such ID allocation has some constraints.
 * For more information see PG document.
 *
 * The supported result-types in ARP table:
 * result-type                      | params decision                              | description
 * ---------------------------------| ---------------------------------------------|------------------------------------------
 * ETPS_ARP_BASIC                   | default                                      |Default format
 * ETPS_ARP_EXTENDED                | BCM_L3_FLAGS2_SRC_MAC                        |SRC-MAC derive from ARP (and not from VSI)
 * ETPS_ARP_PLUS_AC                 | BCM_L3_FLAGS2_VLAN_TRANSLATION               |VLAN-Translation from ARP (and not from AC)
 * ETPS_ARP_EXTENDED_B              | egr.vlan != 0                                |In case VSI is required (e.g. Tunnel points to ARP)
 *
 *
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_l3_egress_create(
    int unit,
    uint32 flags,
    bcm_l3_egress_t * egr,
    bcm_if_t * if_id)
{

    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_START(unit);
    DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR(unit, egr->encap_id);

    SHR_INVOKE_VERIFY_DNX(dnx_l3_egress_create_verify(unit, flags, egr, if_id));

    /*
     * Currently Ingress + Egress is not supported
     */
    if (_SHR_IS_FLAG_SET(flags, BCM_L3_INGRESS_ONLY))
    {
        /** perform ingress (FEC) */
        SHR_IF_ERR_EXIT(dnx_l3_egress_create_fec(unit, flags, egr, if_id));
    }
    else if (_SHR_IS_FLAG_SET(flags, BCM_L3_EGRESS_ONLY))
    {
        /** perform egress (ARP) */
        SHR_IF_ERR_EXIT(dnx_l3_egress_create_arp(unit, flags, egr));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Either BCM_L3_INGRESS_ONLY or BCM_L3_EGRESS_ONLY should be set.\r\n");
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
* Destroy a L3 egress object
*
*   \param [in] unit       -  Relevant unit.
*   \param [in] intf  -    egress object ID .
*
* \return
*   \retval  Zero if no error was detected
*   \retval  Negative if error was detected. See \ref shr_error_e
* \remark
*   An egress object consists of 2 parts: FEC and ARP
*   One call to this routing will destroy one of the 2 parts.
*   When intf type is FEC, FEC will be destroyed.
*   When intf type is LIF, ARP will be destroyed.
* \see
*   BCM_L3_ITF_TYPE_XXX
*/
int
bcm_dnx_l3_egress_destroy(
    int unit,
    bcm_if_t intf)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l3_egress_destroy_verify(unit, intf));

    /*
     * Egress only object
     */
    if (BCM_L3_ITF_TYPE_IS_LIF(intf))
    {
        SHR_IF_ERR_EXIT(dnx_l3_egress_arp_info_delete(unit, intf));
    }
    else if (BCM_L3_ITF_TYPE_IS_FEC(intf))
    {
        SHR_IF_ERR_EXIT(dnx_l3_egress_fec_info_delete(unit, intf));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "intf must be of type LIF or FEC.\r\n");
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;

}

/**
* \brief
* Retrieve a L3 egress object
*
*   \param [in] unit       -  Relevant unit.
*   \param [in] intf  -    egress object ID .
*   \param [in] egr  -    structure holding the egress object \n.
*        When intf is ARP, below fields are valid: \n
*        [out]   egr.mac_addr - Next hop forwarding destination mac Next hop vlan id \n
*        [out]   egr.vlan - Next hop vlan id Encapsulation index \n
*        When intf is FEC, below fields are valid: \n
*        [in]    egr.flags, bit BCM_L3_HIT_CLEAR -
*                    If set then, when getting the 'hit' bit, it is also cleared.
*        [out]   egr.flags, bit BCM_L3_HIT -
*                    If set then 'hit' bit was found to have been 'set'.
*        [out]   egr.encap_id - Encapsulation index \n
*        [out]   egr.port - Outgoing port id (if !BCM_L3_TGID) \n
*        [out]   egr.trunk - Outgoing Trunk id (if BCM_L3_TGID) General QOS map id \n
*        [out]   egr.qos_map_id - General QOS map id  \n
*        [out]   egr.failover_id - Failover Object Index Failover Egress Object index \n
*        [out]   egr.failover_if_id - Failover Egress Object index

*
* \return
*   \retval  Zero if no error was detected
*   \retval  Negative if error was detected. See \ref shr_error_e
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
bcm_dnx_l3_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_l3_egress_t * egr)
{
    int hit_clear;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l3_egress_get_verify(unit, intf, egr));
    /*
     * Before initializing, preserve the only bit which is input, on 'flags'.
     */
    hit_clear = egr->flags & BCM_L3_HIT_CLEAR;

    bcm_l3_egress_t_init(egr);
    /*
     * Egress only object
     */
    if (BCM_L3_ITF_TYPE_IS_LIF(intf))
    {
        SHR_IF_ERR_EXIT(dnx_l3_egress_arp_info_get(unit, intf, egr));
    }
    else if (BCM_L3_ITF_TYPE_IS_FEC(intf))
    {
        egr->flags |= hit_clear;

        SHR_IF_ERR_EXIT(dnx_l3_egress_fec_info_get(unit, intf, egr));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "intf(%x) must be of type LIF or FEC created by bcm_l3_egress_create.\r\n", intf);
    }

exit:
    SHR_FUNC_EXIT;

}

/*
 * See description in include/bcm_int/dnx/l3/l3.h
 */
shr_error_e
dnx_l3_rpf_default_route_set(
    int unit,
    int is_unicast_switch,
    int value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L3_GENERAL_CONFIGURATION, &entry_handle_id));

    if (is_unicast_switch == 1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEFAULT_ROUTE_STRICT_RPF, INST_SINGLE, value);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEFAULT_ROUTE_LOOSE_RPF, INST_SINGLE, value);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEFAULT_ROUTE_MC_SIP_BASED_RPF, INST_SINGLE,
                                     value);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See description in include/bcm_int/dnx/l3/l3.h
 */
shr_error_e
dnx_l3_rpf_default_route_get(
    int unit,
    int is_unicast_switch,
    int *value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L3_GENERAL_CONFIGURATION, &entry_handle_id));
    if (is_unicast_switch == 1)
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DEFAULT_ROUTE_STRICT_RPF, INST_SINGLE,
                                   (uint32 *) value);
    }
    else
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DEFAULT_ROUTE_MC_SIP_BASED_RPF, INST_SINGLE,
                                   (uint32 *) value);
    }
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Traverse all L3 egress objects and run a callback function
 * provided by the user for each one.
 * \param [in] unit - The unit number.
 * \param [in] trav_fn - the name of the callback function,
 *          it receives the value of the user_data variable and
 *          the key and result values for each found entry
 * \param [in] user_data - user data that will be sent to the callback function
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * DBAL_TABLE_EEDB_ARP
 *  * DBAL_TABLE_SUPER_FEC_1ST_HIERARCHY
 *  * DBAL_TABLE_SUPER_FEC_2ND_HIERARCHY
 *  * DBAL_TABLE_SUPER_FEC_3RD_HIERARCHY
 */
shr_error_e
bcm_dnx_l3_egress_traverse(
    int unit,
    bcm_l3_egress_traverse_cb trav_fn,
    void *user_data)
{
    uint32 entry_handle_id;
    uint32 dbal_tables[] =
        { DBAL_TABLE_SUPER_FEC_1ST_HIERARCHY, DBAL_TABLE_SUPER_FEC_2ND_HIERARCHY, DBAL_TABLE_SUPER_FEC_3RD_HIERARCHY };
    uint32 field_value[1];
    uint32 nof_tables = sizeof(dbal_tables) / sizeof(dbal_tables[0]);
    uint32 table;
    int is_end;
    int instance;
    int nof_instances = 2;
    bcm_l3_egress_t egr_itf_struct;
    bcm_if_t intf;
    uint8 is_allocated = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (trav_fn == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No callback function has been provided to the traverse.\n");
    }
    /*
     * Iterate over all tables and all their entries
     */
    for (table = 0; table < nof_tables; table++)
    {
        /*
         * Allocate handle to the table of the iteration and initialise an iterator entity.
         * The iterator is in mode ALL, which means that it will consider all entries regardless
         * of them being default entries or not.
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_tables[table], &entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
        /*
         * Receive first entry in table.
         */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_SUPER_FEC_ID, field_value));
            for (instance = 0; instance < nof_instances; instance++)
            {
                bcm_l3_egress_t_init(&egr_itf_struct);
                intf = L3_FEC_ID_FROM_SUPER_FEC_INSTANCE(field_value[0], instance);
                SHR_IF_ERR_EXIT(dnx_algo_l3_fec_is_allocated(unit, intf, &is_allocated));
                if (is_allocated == 1)
                {
                    BCM_L3_ITF_SET(intf, BCM_L3_ITF_TYPE_FEC, intf);
                    SHR_IF_ERR_EXIT(dnx_l3_egress_fec_info_get(unit, intf, &egr_itf_struct));
                    /** Invoke callback function */
                    SHR_IF_ERR_EXIT((*trav_fn) (unit, intf, &egr_itf_struct, user_data));
                }
            }
            /*
             * Receive next entry in table.
             */
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify function for BCM-API: bcm_dnx_l3_egress_arp_traverse
 * \param [in] unit - the unit number.
 * \param [in] trav_fn - the name of the callback function,
 *          it receives the value of the user_data variable and
 *          the key and result values for each found entry
 * \param [in] user_data - user data that will be sent to the callback function
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
static shr_error_e
dnx_l3_egress_arp_traverse_verify(
    int unit,
    bcm_l3_egress_traverse_cb trav_fn,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(trav_fn, _SHR_E_PARAM, "No callback function has been provided to the traverse.");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Traverse all L3 egress ARP objects and run a callback function
 * provided by the user for each one.
 * \param [in] unit - the unit number.
 * \param [in] trav_fn - the name of the callback function,
 *          it receives the value of the user_data variable and
 *          the key and result values for each found entry
 * \param [in] user_data - user data that will be sent to the callback function
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * DBAL_TABLE_EEDB_ARP
 */
shr_error_e
bcm_dnx_l3_egress_arp_traverse(
    int unit,
    bcm_l3_egress_traverse_cb trav_fn,
    void *user_data)
{
    uint32 entry_handle_id = 0;
    uint32 dbal_tables[] = { DBAL_TABLE_EEDB_ARP };
    uint32 local_out_lif = 0;
    uint32 nof_tables = sizeof(dbal_tables) / sizeof(dbal_tables[0]);
    uint32 table_id = 0;
    int is_end = 0;
    bcm_if_t intf = -1;
    bcm_l3_egress_t egr_itf_struct;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    bcm_gport_t next_gport;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l3_egress_arp_traverse_verify(unit, trav_fn, user_data));
    next_gport = BCM_GPORT_INVALID;
    /*
     * Iterate over all tables and all their entries
     */
    for (table_id = 0; table_id < nof_tables; table_id++)
    {
        /*
         * Allocate handle to the table of the iteration and initialise an iterator entity. * The iterator is in mode
         * ALL, which means that it will consider all entries regardless * of them being default entries or not.
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_tables[table_id], &entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
        /*
         * Receive first entry in table.
         */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {
            bcm_l3_egress_t_init(&egr_itf_struct);
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_OUT_LIF, &local_out_lif));

            /*
             * Verify the out lif is valid
             */
            if (local_out_lif != 0)
            {
                next_gport = BCM_GPORT_INVALID;

                /** In case local lif is not found, return next pointer 0 */
                SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit,
                                                            DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS |
                                                            DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT, _SHR_CORE_ALL,
                                                            local_out_lif, &next_gport));
                if (next_gport != BCM_GPORT_INVALID)
                {
                    if (BCM_GPORT_IS_TUNNEL(next_gport))
                    {
                        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(intf, next_gport);
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                        (unit, next_gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS,
                                         &gport_hw_resources));
                        BCM_L3_ITF_SET(intf, BCM_L3_ITF_TYPE_LIF, gport_hw_resources.global_out_lif);
                    }
                    SHR_IF_ERR_EXIT(dnx_l3_egress_arp_info_get(unit, intf, &egr_itf_struct));

                    /** Invoke callback function */
                    SHR_IF_ERR_EXIT((*trav_fn) (unit, intf, &egr_itf_struct, user_data));
                }
            }
            /*
             * Receive next entry in table.
             */
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
