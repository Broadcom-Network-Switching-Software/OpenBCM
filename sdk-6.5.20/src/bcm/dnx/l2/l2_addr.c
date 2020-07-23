/** \file l2_addr.c
 * 
 *
 * L2 procedures for DNX.
 *
 * This file contains functions for manipulating the MACT entries.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_L2
/*
 * Include files which are specifically for DNX. Final location.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
/*
 * }
 */
/*
 * Include files currently used for DNX. To be modified and moved to
 * final location.
 * {
 */
#include <soc/dnx/dbal/dbal.h>

/*
 * }
 */
/*
 * Include files.
 * {
 */
#include <bcm/trunk.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/vsi/vsi.h>
#include <bcm_int/dnx/l2/l2.h>
#include <bcm_int/dnx/l2/l2_addr.h>
#include <src/bcm/dnx/trunk/trunk_verify.h>
#include <src/bcm/dnx/trunk/trunk_utils.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_multicast.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trap.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_l2_access.h>
#include <bcm_int/dnx/stat/stat_pp.h>

/*
 * }
 */

/*
 * }
 */
/*
 * Function Declaration.
 * {
 */

/*
 * }
 */
/*
 * Defines.
 * {
 */

/** Define currently supported flags */
#define L2_ADDR_SUPPORTED_BCM_L2_FLAGS  (BCM_L2_STATIC | BCM_L2_MCAST | BCM_L2_TRUNK_MEMBER | BCM_L2_DISCARD_SRC | BCM_L2_DISCARD_DST | BCM_L2_DES_HIT | BCM_L2_SRC_HIT | BCM_L2_MOVE_PORT)
/** Define currently supported flags for flags2 */
#define L2_ADDR_SUPPORTED_BCM_L2_FLAGS2 (BCM_L2_FLAGS2_AGE_GET | BCM_L2_FLAGS2_HIT_GET)
 /**
 * \brief - Verify input parameters to the bcm_l2_addr_add API.
 *
 * \param [in] unit - Unit ID
 * \param [in] l2addr - L2 address structure
 *    l2addr->vid - Forwarding-ID
 *    l2addr->tgid - Trunk group id
 *    l2addr->flags - can have the following flags concatenated: BCM_L2_STATIC, BCM_L2_MCAST, BCM_L2_TRUNK_MEMBER.
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static int
dnx_l2_addr_add_verify(
    int unit,
    bcm_l2_addr_t * l2addr)
{
    uint32 max_mc_groups;
    uint32 trunk_id = 0;
    int nof_vsis;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(l2addr, _SHR_E_PARAM, "l2addr");
    SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));
    if (l2addr->vid >= nof_vsis)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Wrong setting. vsi = %d is not a valid vsi. Expected a number smaller than %d \n", l2addr->vid,
                     nof_vsis);
    }

    /** Verify that only supported flags are used. */
    if (_SHR_IS_FLAG_SET(l2addr->flags, ~L2_ADDR_SUPPORTED_BCM_L2_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal L2 flags are used. flags: %d", l2addr->flags);
    }

    if ((l2addr->flags & BCM_L2_MOVE_PORT) && !(l2addr->flags & BCM_L2_STATIC))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal L2 flags. BCM_L2_MOVE_PORT must be set together with BCM_L2_STATIC.");
    }

    /*
     * Verify the validity of the tgid if the BCM_L2_TRUNK_MEMBER has been provided.
     */
    if (_SHR_IS_FLAG_SET(l2addr->flags, BCM_L2_TRUNK_MEMBER))
    {
        /*
         * Verify if the tgid is valid
         */
        if (l2addr->tgid > 0)
        {
            SHR_IF_ERR_EXIT(dnx_trunk_t_verify(unit, l2addr->tgid));
        }
        /*
         * In case that the port is set, verify if it is a valid trunk gport
         */
        if (l2addr->port != 0)
        {
            if (BCM_GPORT_IS_TRUNK(l2addr->port))
            {
                /*
                 * In case of trunk gport, get the trunk id and verify it
                 */
                trunk_id = BCM_GPORT_TRUNK_GET(l2addr->port);
                SHR_IF_ERR_EXIT(dnx_trunk_t_verify(unit, trunk_id));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "port should be configured with valid trunk gport!");
            }
        }

    }

    /*
     * Verify the modid is 0
     */
    if (l2addr->modid != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "modid should be zero, not %d!", l2addr->modid);
    }

    if (_SHR_IS_FLAG_SET(l2addr->flags, BCM_L2_MCAST))
    {
        if (dnx_data_multicast.params.max_ing_mc_groups_get(unit) >=
            dnx_data_multicast.params.max_egr_mc_groups_get(unit))
        {
            max_mc_groups = dnx_data_multicast.params.max_ing_mc_groups_get(unit);
        }
        else
        {
            max_mc_groups = dnx_data_multicast.params.max_egr_mc_groups_get(unit);
        }

        if ((l2addr->l2mc_group < 0) || (l2addr->l2mc_group > max_mc_groups))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The L2 multicast group %d is not valid!", l2addr->l2mc_group);
        }
    }

    if (l2addr->cos_dst != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "cos_dst should be zero, not %d!", l2addr->cos_dst);
    }

    SHR_MAX_VERIFY(l2addr->group, SAL_UPTO_BIT(dnx_data_l2.dma.flush_group_size_get(unit)), _SHR_E_PARAM,
                   "l2addr->group exceeded allowed range!\n");

    /*
     * Statistics check
     */
    SHR_IF_ERR_EXIT(dnx_stat_pp_crps_verify
                    (unit, BCM_CORE_ALL, l2addr->stat_id, l2addr->stat_pp_profile,
                     bcmStatCounterInterfaceIngressReceivePp));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify vid and l2_addr parameters for BCM-API: bcm_dnx_l2_addr_get.
 *
 * \param [in] unit -  Unit ID
 * \param [in] mac_addr - MAC address
 * \param [in] vsi - VSI ID
 * \param [out] l2addr - the L2 address structure to be verified
 * \param [in] age_state_not_updated - '1' if updating the age_state of the l2 entry is not supported
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static int
dnx_l2_addr_get_verify(
    int unit,
    bcm_mac_t mac_addr,
    bcm_vlan_t vsi,
    bcm_l2_addr_t * l2addr,
    uint32 age_state_not_updated)
{
    int nof_vsis;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(l2addr, _SHR_E_PARAM, "l2addr");

    SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));
    if (l2addr->vid >= nof_vsis)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Wrong setting. vsi = %d is not a valid vsi. Expected a number smaller than %d \n", l2addr->vid,
                     nof_vsis);
    }

    if (_SHR_IS_FLAG_SET(l2addr->flags, ~L2_ADDR_SUPPORTED_BCM_L2_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal L2 flags are used.");
    }

    if (_SHR_IS_FLAG_SET(l2addr->flags2, ~L2_ADDR_SUPPORTED_BCM_L2_FLAGS2))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal L2 flags2 are used.");
    }

    if (age_state_not_updated && (_SHR_IS_FLAG_SET(l2addr->flags2, BCM_L2_FLAGS2_AGE_GET)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM_L2_FLAGS2_AGE_GET should not be set in case that updating the age state is not supported!");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_l2_addr_from_dbal_to_struct(
    int unit,
    uint32 entry_handle_id,
    dbal_tables_e table_id,
    bcm_l2_addr_t * l2addr)
{
    uint32 result_type;
    uint32 result_value;
    uint32 destination;
    dnx_algo_gpm_forward_info_t forward_info;
    uint32 entry_strength;
    dbal_fields_e dbal_sub_field;
    bcm_mac_t mac_addr;
    int vsi;
    uint32 stat_pp_profile;
    uint32 trap_id = 0;
    uint32 ingress_trap_id;
    uint32 ingress_trap_destination;
    bcm_gport_t trunk_gport = 0;
    uint32 source_drop = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (table_id != DBAL_TABLE_FWD_MACT)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "table ID must be DBAL_TABLE_FWD_MACT.");
    }
    /*
     * Get key fields
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_VSI, (uint32 *) &vsi));
    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_get(unit, entry_handle_id, DBAL_FIELD_L2_MAC, mac_addr));

    /*
     * Extract payload
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &result_type));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MAC_STRENGTH, INST_SINGLE, &entry_strength));

    bcm_l2_addr_t_init(l2addr, mac_addr, vsi);
    /*
     * Source drop
     */
    if ((result_type == DBAL_RESULT_TYPE_FWD_MACT_FWD_MACT_RESULT_DEST_STAT) ||
        (result_type == DBAL_RESULT_TYPE_FWD_MACT_FWD_MACT_RESULT_DEST_LIF_STAT))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_SOURCE_DROP, INST_SINGLE, &source_drop));
        if (source_drop)
        {
            l2addr->flags = l2addr->flags | BCM_L2_DISCARD_SRC;
        }
    }

    if (entry_strength >= STATIC_MACT_ENTRY_STRENGTH)
    {
        l2addr->flags = l2addr->flags | BCM_L2_STATIC;
    }

    if (entry_strength == STATIC_TRANSPLANTABLE_MACT_ENTRY_STRENGTH)
    {
        l2addr->flags = l2addr->flags | (BCM_L2_STATIC | BCM_L2_MOVE_PORT);
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_ENTRY_GROUPING,
                                                        INST_SINGLE, (uint32 *) (&l2addr->group)));
    /*
     * Receive the sub field of the destination that is in use and its decoded value.
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, &destination));
    SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                    (unit, DBAL_FIELD_DESTINATION, destination, &dbal_sub_field, &result_value));

    /*
     * The supported result types for table FWD_MACT are FWD_MACT_RESULT_NO_OUTLIF, FWD_MACT_RESULT_SINGLE_OUTLIF
     * FWD_MACT_RESULT_DEST_STAT and FWD_MACT_RESULT_DEST_LIF_STAT.
     * If not supported result_type is used, then an error will be returned.
     */
    /** According to result_type fetch other fields */
    if ((((result_type == DBAL_RESULT_TYPE_FWD_MACT_FWD_MACT_RESULT_NO_OUTLIF) ||
          (result_type == DBAL_RESULT_TYPE_FWD_MACT_FWD_MACT_RESULT_DEST_STAT) ||
          (result_type == DBAL_RESULT_TYPE_FWD_MACT_FWD_MACT_RESULT_SINGLE_OUTLIF) ||
          (result_type == DBAL_RESULT_TYPE_FWD_MACT_FWD_MACT_RESULT_DEST_LIF_STAT) ||
          (result_type == DBAL_RESULT_TYPE_FWD_MACT_FWD_MACT_RESULT_EEI_FEC)) && table_id == DBAL_TABLE_FWD_MACT))
    {
        /*
         * Based on the result type, the fwd_result_type of the forward_info will be defined.
         * The valid types for FWD_MACT are DEST_ONLY or DEST_OUTLIF.
         * If the result type is DEST_OUTLIF, then the outlif value will be extracted.
         */
        if (((result_type == DBAL_RESULT_TYPE_FWD_MACT_FWD_MACT_RESULT_NO_OUTLIF ||
              result_type == DBAL_RESULT_TYPE_FWD_MACT_FWD_MACT_RESULT_DEST_STAT) && table_id == DBAL_TABLE_FWD_MACT))
        {
            forward_info.fwd_info_result_type = DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_ONLY;
            forward_info.outlif = 0;
        }
        else if ((result_type == DBAL_RESULT_TYPE_FWD_MACT_FWD_MACT_RESULT_EEI_FEC))
        {
            /** Relevant only to JR1 mode */
            forward_info.fwd_info_result_type = DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_EEI;
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_EEI,
                                                                INST_SINGLE, &forward_info.eei));
        }
        else
        {
            forward_info.fwd_info_result_type = DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF;
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id,
                                                                DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE,
                                                                &forward_info.outlif));
        }
        forward_info.destination = destination;

        if (dbal_sub_field == DBAL_FIELD_INGRESS_TRAP_DEST)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_INGRESS_TRAP_DEST, INST_SINGLE,
                             &ingress_trap_destination));
            SHR_IF_ERR_EXIT(bcm_rx_trap_type_get(unit, 0, bcmRxTrapDfltDroppedPacket, (int *) &trap_id));
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode(unit, DBAL_FIELD_INGRESS_TRAP_DEST,
                                                            DBAL_FIELD_INGRESS_TRAP_ID, &ingress_trap_id,
                                                            &ingress_trap_destination));
            /** Check if ingress_trap_id is equal to bcmRxTrapDfltDroppedPacket trap_id */
            if (ingress_trap_id == trap_id)
            {
                l2addr->flags = l2addr->flags | BCM_L2_DISCARD_DST;
            }

        }
        else if (dbal_sub_field == DBAL_FIELD_LAG_ID)
        {

            /** Get the encap_id */
            SHR_IF_ERR_EXIT(algo_gpm_gport_and_encap_from_forward_information(unit, &trunk_gport,
                                                                              (uint32 *) (&l2addr->encap_id),
                                                                              &forward_info, 1));
            l2addr->tgid = BCM_GPORT_TRUNK_GET(trunk_gport);
            l2addr->port = trunk_gport;
            l2addr->flags = l2addr->flags | BCM_L2_TRUNK_MEMBER;
        }
        else
        {
            /*
             * Resolve the destination fields of the entry.
             */
            SHR_IF_ERR_EXIT(algo_gpm_gport_and_encap_from_forward_information(unit, &l2addr->port,
                                                                              (uint32 *) (&l2addr->encap_id),
                                                                              &forward_info, 0));
        }
        if (dbal_sub_field == DBAL_FIELD_MC_ID)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_MC_ID, INST_SINGLE, (uint32 *) &l2addr->l2mc_group));
            l2addr->port = 0;
            l2addr->flags = l2addr->flags | BCM_L2_MCAST;
        }

        if (dnx_stat_pp_result_type_verify(unit, table_id, result_type) == _SHR_E_NONE)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                                entry_handle_id,
                                                                DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE,
                                                                &stat_pp_profile));
            /*
             * Get the statistic data only if the profile is valid
             */
            if (stat_pp_profile != STAT_PP_PROFILE_INVALID)
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                                    entry_handle_id,
                                                                    DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE,
                                                                    &l2addr->stat_id));

                STAT_PP_ENGINE_PROFILE_SET(l2addr->stat_pp_profile, stat_pp_profile,
                                           bcmStatCounterInterfaceIngressReceivePp);
            }
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "result_type (%d) is not supported for API bcm_l2_addr_get \r\n", result_type);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_convert_l2_addr_to_dest(
    int unit,
    bcm_l2_addr_t * l2addr,
    uint32 *destination,
    uint32 *outlif,
    uint32 *result_type,
    uint32 *eei)
{
    uint32 trunk_gport = 0;
    dnx_algo_gpm_forward_info_t forward_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l2_addr_add_verify(unit, l2addr));

    *outlif = 0;
    *result_type = 0;
    *eei = 0;

    if (l2addr->flags & BCM_L2_MCAST)
    {
        SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                        (unit, DBAL_FIELD_DESTINATION, DBAL_FIELD_MC_ID, (uint32 *) &(l2addr->l2mc_group),
                         destination));

        *result_type = (l2addr->stat_pp_profile != STAT_PP_PROFILE_INVALID) ?
            DBAL_RESULT_TYPE_FWD_MACT_FWD_MACT_RESULT_DEST_STAT :
            (dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
             DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE) ?
            DBAL_RESULT_TYPE_FWD_MACT_FWD_MACT_RESULT_SINGLE_OUTLIF :
            DBAL_RESULT_TYPE_FWD_MACT_FWD_MACT_RESULT_NO_OUTLIF;

        SHR_EXIT();
    }

    /*
     * Currently assuming GPORT encoding for destination + outLIF.
     */
    if (_SHR_IS_FLAG_SET(l2addr->flags, BCM_L2_TRUNK_MEMBER))
    {
        if (l2addr->tgid != 0)
        {
            BCM_GPORT_TRUNK_SET(trunk_gport, l2addr->tgid);
        }
        else
        {
            trunk_gport = l2addr->port;
        }
        SHR_IF_ERR_EXIT(algo_gpm_gport_and_encap_to_forward_information
                        (unit, trunk_gport, l2addr->encap_id, &forward_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(algo_gpm_gport_and_encap_to_forward_information
                        (unit, l2addr->port, l2addr->encap_id, &forward_info));
    }
    *destination = forward_info.destination;
    if ((forward_info.fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF)
        || ((dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
             DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
            && forward_info.fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_ONLY))
    {
        *outlif = forward_info.outlif;

        *result_type = ((l2addr->stat_pp_profile != STAT_PP_PROFILE_INVALID)
                        || (l2addr->flags & BCM_L2_DISCARD_SRC)) ?
            DBAL_RESULT_TYPE_FWD_MACT_FWD_MACT_RESULT_DEST_LIF_STAT :
            DBAL_RESULT_TYPE_FWD_MACT_FWD_MACT_RESULT_SINGLE_OUTLIF;
    }
    else if (forward_info.fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_ONLY)
    {

        *result_type = ((l2addr->stat_pp_profile != STAT_PP_PROFILE_INVALID)
                        || (l2addr->flags & BCM_L2_DISCARD_SRC)) ?
            DBAL_RESULT_TYPE_FWD_MACT_FWD_MACT_RESULT_DEST_STAT : DBAL_RESULT_TYPE_FWD_MACT_FWD_MACT_RESULT_NO_OUTLIF;

    }
    else if (forward_info.fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_EEI)
    {

        *result_type = DBAL_RESULT_TYPE_FWD_MACT_FWD_MACT_RESULT_EEI_FEC;
        *eei = forward_info.eei;
    }
    else
    {
        
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported fwd_info_result_type, gport (0x%x)\n", l2addr->port);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Implement the bcm_l2_addr_add in the case it adds to the FWD_MACT logical table
 */
static int
dnx_l2_addr_add_fwd_mact(
    int unit,
    bcm_l2_addr_t * l2addr)
{
    uint32 entry_handle_id;
    uint32 outlif;
    uint32 destination;
    uint32 result_type;
    uint8 entry_strength;
    uint32 trap_dest, snp_strength, fwd_strength, trap_id;
    uint32 eei = 0;
    uint32 source_drop = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    snp_strength = 0;
    fwd_strength = dnx_data_trap.strength.max_strength_get(unit);
    trap_id = 0;
    trap_dest = 0;

    SHR_IF_ERR_EXIT(dnx_convert_l2_addr_to_dest(unit, l2addr, &destination, &outlif, &result_type, &eei));

    /*
     *  Set the entry's strength. Static entries should have a higher strength than dynamic.
     *  MACT payloads are updated when the new entry has a bigger strength than the previous.
     *  In order to give the static entries a priority over the dynamic, they should have a higher or equal strength.
     */
    if (l2addr->flags & BCM_L2_STATIC)
    {
        if (l2addr->flags & BCM_L2_MOVE_PORT)
        {
            entry_strength = STATIC_TRANSPLANTABLE_MACT_ENTRY_STRENGTH;
        }
        else
        {
            entry_strength = STATIC_MACT_ENTRY_STRENGTH;
        }
    }
    else
    {
        entry_strength = DYNAMIC_MACT_ENTRY_STRENGTH;
    }

    /*
     * Write to MAC table
     */

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FWD_MACT, &entry_handle_id));

    /** Key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, l2addr->vid);
    dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_MAC, l2addr->mac);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);

    if ((l2addr->flags & BCM_L2_DISCARD_DST) == 0)
    {
        /** The destination is encoded to the correct sub field in dnx_convert_l2_addr_to_dest */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, destination);
    }
    else
    {
        /** Configure trap with action drop */
        SHR_IF_ERR_EXIT(bcm_rx_trap_type_get(unit, 0, bcmRxTrapDfltDroppedPacket, (int *) &trap_id));

        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                        (unit, DBAL_FIELD_INGRESS_TRAP_DEST, DBAL_FIELD_TRAP_SNP_STRENGTH, &snp_strength, &trap_dest));
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                        (unit, DBAL_FIELD_INGRESS_TRAP_DEST, DBAL_FIELD_TRAP_FWD_STRENGTH, &fwd_strength, &trap_dest));
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                        (unit, DBAL_FIELD_INGRESS_TRAP_DEST, DBAL_FIELD_INGRESS_TRAP_ID, &trap_id, &trap_dest));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_TRAP_DEST, INST_SINGLE, trap_dest);
    }

    if (result_type == DBAL_RESULT_TYPE_FWD_MACT_FWD_MACT_RESULT_SINGLE_OUTLIF ||
        result_type == DBAL_RESULT_TYPE_FWD_MACT_FWD_MACT_RESULT_DEST_LIF_STAT)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, outlif);
    }

    if (result_type == DBAL_RESULT_TYPE_FWD_MACT_FWD_MACT_RESULT_EEI_FEC)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EEI, INST_SINGLE, eei);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENTRY_GROUPING, INST_SINGLE, l2addr->group);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC_STRENGTH, INST_SINGLE, entry_strength);
    /*
     * dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_AGING, INST_SINGLE, is_dynamic);
     */

    /*
     * Statistics enabled
     */
    if ((dnx_stat_pp_result_type_verify(unit, DBAL_TABLE_FWD_MACT, result_type) == _SHR_E_NONE))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, l2addr->stat_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE,
                                     STAT_PP_PROFILE_ID_GET(l2addr->stat_pp_profile));
    }

    if (result_type == DBAL_RESULT_TYPE_FWD_MACT_FWD_MACT_RESULT_DEST_LIF_STAT ||
        result_type == DBAL_RESULT_TYPE_FWD_MACT_FWD_MACT_RESULT_DEST_STAT)
    {
        source_drop = ((l2addr->flags & BCM_L2_DISCARD_SRC) ? 1 : 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_DROP, INST_SINGLE, source_drop);
    }

    /*
     * setting the entry with the default values
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_FORCE));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Print a l2 key to the console.
 *
 * \param [in] unit - unit id
 * \param [in] pfx - string to print before the key
 * \param [in] entry - struct holding the key to print
 * \param [in] sfx - string to print after the key
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_l2_key_dump(
    int unit,
    char *pfx,
    bcm_l2_addr_t * entry,
    char *sfx)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_NULL_CHECK(pfx, _SHR_E_PARAM, "pfx should not be NULL");
    SHR_NULL_CHECK(entry, _SHR_E_PARAM, "entry should not be NULL");
    SHR_NULL_CHECK(sfx, _SHR_E_PARAM, "sfx should not be NULL");

    LOG_CLI((BSL_META_U(unit,
                        "l2: %sVLAN=0x%03x MAC=0x%02x%02x%02x"
                        "%02x%02x%02x%s"), pfx, entry->vid,
             entry->mac[0], entry->mac[1], entry->mac[2], entry->mac[3], entry->mac[4], entry->mac[5], sfx));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Create a MAC table entry
 *          Write to HW MAC table.
 * \param [in] unit - Relevant unit
 * \param [in] l2addr -  The entry's information
 *     l2addr.vid - Forwarding-ID
 *     l2addr.mac - MAC address
 *     l2addr.port - A gport to a system-destination (DSPA)
 *     l2addr.flags - can have the following flags concatenated: BCM_L2_STATIC, BCM_L2_MCAST, BCM_L2_TRUNK_MEMBER.
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_l2_addr_add(
    int unit,
    bcm_l2_addr_t * l2addr)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_IF_ERR_EXIT(dnx_l2_addr_add_fwd_mact(unit, l2addr));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete an L2 address entry from the specified device.
 *
 * \param [in] unit - Unit id
 * \param [in] mac -  Mac address
 * \param [in] vid - VSI
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_l2_addr_delete(
    int unit,
    bcm_mac_t mac,
    bcm_vlan_t vid)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FWD_MACT, &entry_handle_id));

    /** Set key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vid);
    dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_MAC, mac);

    /** Delete the entry */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Calculate the age state of l2 entry in seconds
 *
 * \param [in] unit - Relevant unit
 * \param [in] vsi -  VSI
 * \param [in] l2_entry_current_age_state - The current age state of l2 entry
 * \param [out] age_state_in_seconds - The age state of l2 entry in seconds
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_l2_addr_age_state_in_seconds_get(
    int unit,
    int vsi,
    int l2_entry_current_age_state,
    int *age_state_in_seconds)
{
    int nof_meta_cycles_for_ageout;
    int nof_age_states_until_age_out;
    int learning_profile_id;
    int learning_profile_ref_count;
    int aging_profile_ref_count;
    dnx_aging_profile_info_t aging_profile_info;
    dnx_vsi_learning_profile_info_t learning_profile_info;
    uint32 mask_factor;
    int meta_cycle_time = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** Get the learning profile of the vsi */
    SHR_IF_ERR_EXIT(dnx_vsi_learning_profile_id_get(unit, vsi, &learning_profile_id));

    /** Get the vsi learning profile info */
    SHR_IF_ERR_EXIT(algo_l2_db.vsi_learning_profile.profile_data_get
                    (unit, learning_profile_id, &learning_profile_ref_count, (void *) &learning_profile_info));

    SHR_IF_ERR_EXIT(algo_l2_db.vsi_aging_profile.profile_data_get
                    (unit, learning_profile_info.aging_profile, &aging_profile_ref_count,
                     (void *) &aging_profile_info));

    /** Get the number of seconds between two meta cycles of the age machine */
    SHR_IF_ERR_EXIT(bcm_dnx_l2_age_timer_meta_cycle_get(unit, &meta_cycle_time));

    if (aging_profile_info.ageout_age != AGING_PROFILE_INVALID_AGE)
    {
        /** Number of age states until age out */
        nof_age_states_until_age_out = l2_entry_current_age_state - aging_profile_info.ageout_age + 1;
        if ((nof_age_states_until_age_out < 0)
            || (nof_age_states_until_age_out > dnx_data_l2.age_and_flush_machine.max_age_states_get(unit)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Wrong number of age states until age out. Expected a number bigger than 0 and equal to or less than %d \n",
                         dnx_data_l2.age_and_flush_machine.max_age_states_get(unit));
        }

        mask_factor = utilex_nof_on_bits_in_long(learning_profile_info.cycle_mask);

        nof_meta_cycles_for_ageout = nof_age_states_until_age_out << mask_factor;
    }
    else
    {
        nof_meta_cycles_for_ageout = 0;
    }

    *age_state_in_seconds = nof_meta_cycles_for_ageout * meta_cycle_time;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve an L2 entry from the L2 address table.
 *
 * \param [in] unit - Unit id
 * \param [in] mac_addr - Mac address
 * \param [in] vid - VSI
 * \param [out] l2addr - L2 entry`s information
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_l2_addr_get(
    int unit,
    bcm_mac_t mac_addr,
    bcm_vlan_t vid,
    bcm_l2_addr_t * l2addr)
{
    uint32 entry_handle_id;
    bcm_vlan_t vsi;
    uint32 age_state = 0;
    uint32 age_state_not_updated;
    uint32 hit_bit = 0;
    int age_state_in_seconds = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    vsi = vid;
    age_state_not_updated = dnx_data_l2.feature.feature_get(unit, dnx_data_l2_feature_age_state_not_updated);

    SHR_INVOKE_VERIFY_DNX(dnx_l2_addr_get_verify(unit, mac_addr, vsi, l2addr, age_state_not_updated));

    /** Get table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FWD_MACT, &entry_handle_id));

    /** Set key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
    dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_MAC, mac_addr);
    if ((!age_state_not_updated) && (_SHR_IS_FLAG_SET(l2addr->flags2, BCM_L2_FLAGS2_AGE_GET)))
    {
        /** Request the age_state of the l2 entry */
        dbal_entry_attribute_request(unit, entry_handle_id, DBAL_ENTRY_ATTR_AGE, &age_state);
    }
    if (_SHR_IS_FLAG_SET(l2addr->flags2, BCM_L2_FLAGS2_HIT_GET))
    {
        /** Request the hit_bit of the l2 entry */
        dbal_entry_attribute_request(unit, entry_handle_id, DBAL_ENTRY_ATTR_HIT_GET, &hit_bit);
    }
    /** Perform the action - all results fields of the table will be read to handle buffer */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

        /** Request the hit_bit of the l2 entry */

    SHR_IF_ERR_EXIT(dnx_l2_addr_from_dbal_to_struct(unit, entry_handle_id, DBAL_TABLE_FWD_MACT, l2addr));
    if (age_state)
    {
        /** Calculate the age state of the l2 entry in seconds */
        SHR_IF_ERR_EXIT(dnx_l2_addr_age_state_in_seconds_get(unit, vsi, age_state, &age_state_in_seconds));
        /** Set the age_state in seconds */
        l2addr->age_state = age_state_in_seconds;
    }

    if (hit_bit & DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED_A)
    {
        /** Set the source hit bit */
        l2addr->flags |= BCM_L2_SRC_HIT;
    }

    if (hit_bit & DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED_B)
    {
        /** Set the destination hit bit */
        l2addr->flags |= BCM_L2_DES_HIT;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_l2_addr_by_struct_delete API
 *
 * \param [in] unit - Unit id
 * \param [in] l2addr - L2 address structure
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_l2_addr_by_struct_delete_verify(
    int unit,
    bcm_l2_addr_t * l2addr)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(l2addr, _SHR_E_PARAM, "l2addr");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set the key fields in FWD_MACT tables.
 *
 * \param [in] unit - Relevant unit
 * \param [in] entry_handle_id - DBAL entry handle
 * \param [in] l2addr - L2 address structure
 *     l2addr.vid - Forwarding-ID
 *     l2addr.mac - MAC address
 *     l2addr.modid - VLAN (used only for IVL) - not supported
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_l2_addr_by_struct_key_set(
    int unit,
    uint32 *entry_handle_id,
    bcm_l2_addr_t * l2addr)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_VSI, l2addr->vid);
    dbal_entry_key_field_arr8_set(unit, *entry_handle_id, DBAL_FIELD_L2_MAC, l2addr->mac);

    SHR_EXIT();
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete an L2 address entry from the specified device.
 *          Support FWD_MACT table.
 *
 * \param [in] unit - Unit id
 * \param [in] l2addr - L2 address structure
 *      l2addr.vid - Forwarding-ID
 *      l2addr.mac - MAC address
 *      l2addr.port - A gport to a system-destination (DSPA)
 *      l2addr.encap_id - Out logical interface
 *      l2addr.tgid - Trunk group id l2addr.flags - can have the following flags concatenated:
 *        BCM_L2_STATIC, BCM_L2_MCAST, BCM_L2_TRUNK_MEMBER.
 *     l2addr.modid - VLAN (used only for IVL) - not supported
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_l2_addr_by_struct_delete(
    int unit,
    bcm_l2_addr_t * l2addr)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l2_addr_by_struct_delete_verify(unit, l2addr));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FWD_MACT, &entry_handle_id));

    /** Set the key fields */
    SHR_IF_ERR_EXIT(dnx_l2_addr_by_struct_key_set(unit, &entry_handle_id, l2addr));

    /** Delete the entry */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_l2_addr_by_struct_get API
 *
 * \param [in] unit - Unit id
 * \param [in,out] l2addr - L2 address structure
 *      l2addr.modid - VLAN (used only for IVL) - not supported
 *      l2addr.vid - Forwarding-ID
 *      l2addr.flags - can have the following flags concatenated: BCM_L2_STATIC,
 *        BCM_L2_MCAST, BCM_L2_TRUNK_MEMBER.
 *      l2addr.flags2 - can have the following flags concatenated: BCM_L2_FLAGS2_AGE_GET,
 *        BCM_L2_FLAGS2_HIT_GET
 * \param [in] age_state_not_updated - '1' if updating the age_state of the l2 entry is not supported
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_l2_addr_by_struct_get_verify(
    int unit,
    bcm_l2_addr_t * l2addr,
    uint32 age_state_not_updated)
{
    int nof_vsis;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(l2addr, _SHR_E_PARAM, "l2addr");
    SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));
    if (l2addr->vid >= nof_vsis)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Wrong setting. vsi = %d is not a valid vsi. Expected a number smaller than %d \n", l2addr->vid,
                     nof_vsis);
    }

    if (_SHR_IS_FLAG_SET(l2addr->flags, ~L2_ADDR_SUPPORTED_BCM_L2_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal L2 flags are used.");
    }

    if (_SHR_IS_FLAG_SET(l2addr->flags2, ~L2_ADDR_SUPPORTED_BCM_L2_FLAGS2))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal L2 flags2 are used.");
    }

    if (age_state_not_updated && (_SHR_IS_FLAG_SET(l2addr->flags2, BCM_L2_FLAGS2_AGE_GET)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM_L2_FLAGS2_AGE_GET should not be set in case that updating the age state is not supported!");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve an L2 entry from the L2 address table.
 *          Support FWD_MACT table.
 *
 * \param [in] unit - Unit id
 * \param [in,out] l2addr - L2 address structure
 *     l2addr.vid - Forwarding-ID
 *     l2addr.mac - MAC address
 *     l2addr.port - A gport to a system-destination (DSPA)
 *     l2addr.encap_id - Out logical interface
 *     l2addr.tgid - Trunk group id
 *     l2addr.flags - can have the following flags concatenated: BCM_L2_STATIC,
 *        BCM_L2_MCAST, BCM_L2_TRUNK_MEMBER.
 *     l2addr.modid - VLAN (used only for IVL) - not supported
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_l2_addr_by_struct_get(
    int unit,
    bcm_l2_addr_t * l2addr)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id;
    uint32 age_state = 0;
    uint32 age_state_not_updated;
    uint32 hit_bit = 0;
    int age_state_in_seconds = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    age_state_not_updated = dnx_data_l2.feature.feature_get(unit, dnx_data_l2_feature_age_state_not_updated);

    SHR_INVOKE_VERIFY_DNX(dnx_l2_addr_by_struct_get_verify(unit, l2addr, age_state_not_updated));

    table_id = DBAL_TABLE_FWD_MACT;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Set the key fields */
    SHR_IF_ERR_EXIT(dnx_l2_addr_by_struct_key_set(unit, &entry_handle_id, l2addr));
    if ((!age_state_not_updated) && (_SHR_IS_FLAG_SET(l2addr->flags2, BCM_L2_FLAGS2_AGE_GET)))
    {
        /** Request the age_state of the l2 entry */
        dbal_entry_attribute_request(unit, entry_handle_id, DBAL_ENTRY_ATTR_AGE, &age_state);
    }

    if (_SHR_IS_FLAG_SET(l2addr->flags2, BCM_L2_FLAGS2_HIT_GET))
    {
        /** Request the hit_bit of the l2 entry */
        dbal_entry_attribute_request(unit, entry_handle_id, DBAL_ENTRY_ATTR_HIT_GET, &hit_bit);
    }

    /** Perform the action - all results fields of the table will be read to handle buffer */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dnx_l2_addr_from_dbal_to_struct(unit, entry_handle_id, table_id, l2addr));

    if (age_state)
    {
        /** Calculate the age state of the l2 entry in seconds */
        SHR_IF_ERR_EXIT(dnx_l2_addr_age_state_in_seconds_get(unit, l2addr->vid, age_state, &age_state_in_seconds));
        /** Set the age_state in seconds */
        l2addr->age_state = age_state_in_seconds;
    }

    if (hit_bit & DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED_A)
    {
        /** Set the source hit bit */
        l2addr->flags |= BCM_L2_SRC_HIT;
    }

    if (hit_bit & DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED_B)
    {
        /** Set the destination hit bit */
        l2addr->flags |= BCM_L2_DES_HIT;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
