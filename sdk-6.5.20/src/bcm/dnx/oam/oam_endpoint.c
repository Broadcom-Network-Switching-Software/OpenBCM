/** \file oam_endpoint.c
 * 
 *
 * OAM enpoint create procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_OAM

 /*
  * Include files.
  * {
  */
#include <bcm/oam.h>
#include <soc/dnx/dbal/dbal.h>
#include <src/bcm/dnx/oam/oam_internal.h>
#include <src/bcm/dnx/bfd/bfd_internal.h>
#include <include/soc/dnx/swstate/auto_generated/access/oam_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <src/bcm/dnx/oam/oam_oamp.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <bcm_int/dnx/algo/oam/algo_oam.h>
#include <bcm_int/dnx/algo/oamp/algo_oamp.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_oam_access.h>
#include <bcm_int/dnx_dispatch.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/bfd/bfd.h>

/*
 * }
 */
/*
 * DEFINEs
 * {
 */

/* unaccelerated mep index calculation */
#define DNX_OAM_UNACCELERATED_MEP_INDEX_GET(lif, mdl, is_up)  \
    (lif + (mdl << DNX_OAM_UNACCELERATED_MEP_MDL_INDEX_BIT) + (is_up << DNX_OAM_UNACCELERATED_MEP_UPMEP_INDEX_BIT) + \
        ((is_up || mdl>0) ? 0 : (1 << DNX_OAM_DOWN_MEP_LEVEL_0_NON_ACC_BIT)))

/*
 * }
 */

/*
 * MACROs
 * {
 */

/** Flag that denotes this is a client endpoint - accelerated using another device's OAMP */
#define IS_OAM_CLIENT(x) (_SHR_IS_FLAG_SET(x->flags2, BCM_OAM_ENDPOINT2_HW_ACCELERATION_SET))

/*
 * }
 */

/**
 * \brief -
 * Initial validatation of the endpoint create request.
 * Validating that values are in correct range and valid
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] endpoint_info - info structure of the requested
 *        MEP
 *
 * \retval Error indication.
 */
static shr_error_e
dnx_oam_endpoint_create_general_validity_check(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info)
{
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(endpoint_info, _SHR_E_PARAM, "endpoint_info")
        if ((_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_TX_STATISTICS) ||
             _SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_RX_STATISTICS))
            && (!DNX_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Endpoint create error: BCM_OAM_ENDPOINT_FLAGS2_TX_STATISTICS and BCM_OAM_ENDPOINT_FLAGS2_RX_STATISTICS flags are supported only for accelerated endpoints.\n");
    }

    if ((!_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_TX_STATISTICS) &&
         !_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_RX_STATISTICS))
        && _SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_PER_OPCODE_STATISTICS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Endpoint create error: BCM_OAM_ENDPOINT_FLAGS2_PER_OPCODE_STATISTICS should be used with BCM_OAM_ENDPOINT_FLAGS2_RX_STATISTICS or BCM_OAM_ENDPOINT_FLAGS2_TX_STATISTICS.\n");
    }

    if ((endpoint_info->level > DNX_OAM_MAX_MDL) || (endpoint_info->level < 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Endpoint create error: Illegal MEP level.\n");
    }

    /** Validate defunct Jer1 values are not set*/
    if (endpoint_info->action_reference_id != BCM_OAM_ENDPOINT_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "action_reference_id no longer supported. Profiles must be set on acc_profile_id For accelerated endpoints"
                     " or with bcm_oam_lif_profile_set() for non accelerated endpoints.\n");
    }

    if (endpoint_info->timestamp_format && !DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "timestamp_format must be set through profiles "
                     " (bcm_oam_profile_action_set() API).\n");
    }

    /*
     * Check dst_mac_address
     */
    for (ii = 0; ii < DNX_OAM_MAC_SIZE; ii++)
    {
        if (endpoint_info->dst_mac_address[ii] != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Endpoint create error: dst_mac_address may not be declared during endpoint create. dst_mac_address are per port and configured by BCM API bcm_l2_station_add\n");
        }
    }

    /** Port/interface TLV status - shared by remote and local accelerated endpoints*/
    if (((endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE) || DNX_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info))
        && endpoint_info->type == bcmOAMEndpointTypeEthernet)
    {
        if (endpoint_info->port_state > BCM_OAM_PORT_TLV_UP)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error: port_state field incorrect. Should be 0, BCM_OAM_PORT_TLV_BLOCKED or BCM_OAM_PORT_TLV_UP.");
        }

        if (endpoint_info->interface_state > BCM_OAM_INTERFACE_TLV_LLDOWN)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error: interface_state field incorrect."
                         " Should be 0 or between BCM_OAM_INTERFACE_TLV_UP (1) and BCM_OAM_INTERFACE_TLV_LLDOWN (7).");
        }
    }
    else
    {
        if (endpoint_info->port_state || endpoint_info->interface_state)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error: port_state, interface_state"
                         " only available in Jericho and for" " accelerated/remote endpoints.");

        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Validatation of a counter base for endpoint create request.
 * In case of PCP based counting requested (BCM_OAM_LM_PCP),
 * counter base should be outside the RANGE. Counter base should
 * also be aligned to 8.
 * In case of non PCP based counting requsted, counter base
 * should be in the range.
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] endpoint_info - info structure of the requested
 *        MEP
 *
 * \retval Error indication.
 */
static shr_error_e
dnx_oam_counter_verify(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info)
{
    uint64 arg;
    int is_pcp;
    int counter_base;
    int counter_range_min;
    int counter_range_max;
    SHR_FUNC_INIT_VARS(unit);

    counter_base = endpoint_info->lm_counter_base_id;
    if (counter_base > 0)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_oam_control_get(unit, bcmOamControlLmPcpCounterRangeMin, &arg));
        counter_range_min = COMPILER_64_LO(arg);
        SHR_IF_ERR_EXIT(bcm_dnx_oam_control_get(unit, bcmOamControlLmPcpCounterRangeMax, &arg));
        counter_range_max = COMPILER_64_LO(arg);

        is_pcp = (endpoint_info->lm_flags & BCM_OAM_LM_PCP);

        if (is_pcp)
        {       /* if min==max==0 it must be is_pcp */
            if ((!((counter_range_min == 0) && (counter_range_max == 0))) &&
                (counter_range_min <= counter_base) && (counter_range_max >= counter_base))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error: lm_counter_base_id %d should be outside the counter-singleton range (%d-%d).\n",
                             counter_base, counter_range_min, counter_range_max);
            }
            /*
             * For PCP 8 counters used for every counter_base - counter_base should be aligned to 8
             */
            if ((counter_base & 0x7) != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error: For PCP, lm_counter_base_id %d should be aligned to 8.\n", counter_base);
            }
        }
        else
        {
            /*
             * if min==max==0 it must be is_pcp
             */
            if (((counter_range_min == 0) && (counter_range_max == 0)) ||
                (counter_range_min > counter_base) || (counter_range_max < counter_base))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error: lm_counter_base_id %d should be inside the counter-singleton range (%d-%d).\n",
                             counter_base, counter_range_min, counter_range_max);
            }
        }
    }
    else
    {
        if (dnx_data_oam.general.oam_force_interface_for_meps_without_counter_get(unit) == 0)
        {
            if (endpoint_info->lm_counter_if > 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Counter interface for endpoint without counter should be 0.\n");
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Validatation of setting extra data index for endpoint.
 * extra data can be used to store LM/DM flexible DMAC or
 * LM/DM flexible DMAC + 48 bytes MAID.
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] endpoint_info -   info structure of the requested
 *        MEP
 * \param [in] sw_endpoint_info -  SW info structure of the requested
 *        MEP
 * \param [in] group_info - group name
 *
 * \retval Error indication.
 */
static shr_error_e
dnx_oam_lmdm_flexible_da_verify(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    const dnx_oam_endpoint_info_t * sw_endpoint_info,
    const bcm_oam_group_info_t * group_info)
{
    uint8 dm_exists = 0, lm_exists = 0, lm_stat_exists = 0;
    int extra_data_nof_bits, extra_data_len;
    int num_short_entries_in_bank, entry_bank, extra_data_header_bank;
    dnx_oam_oamp_endpoint_dm_params_t *dm_mep_db_entry = NULL;
    dnx_oam_oamp_endpoint_lm_params_t *lm_mep_db_entry = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(dm_mep_db_entry, sizeof(dnx_oam_oamp_endpoint_dm_params_t),
                       "Structure for DM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(lm_mep_db_entry, sizeof(dnx_oam_oamp_endpoint_lm_params_t),
                       "Structure for LM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_OAM_ENDPOINT_REPLACE))
    {
        if (!_SHR_IS_FLAG_SET(sw_endpoint_info->sw_state_flags, DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_INTERMEDIATE)
            && (endpoint_info->endpoint_memory_type != bcmOamEndpointMemoryTypeShortEntry)
            && (DNX_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info)))
        {
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_dm_endpoint_get(unit, endpoint_info->id, dm_mep_db_entry, &dm_exists));
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_lm_endpoint_get
                            (unit, endpoint_info->id, lm_mep_db_entry, &lm_exists, &lm_stat_exists));
        }

        if (DNX_OAM_LMDM_FLEXIBLE_DA_IS_SET_TO_ENABLE(endpoint_info, sw_endpoint_info))
        {
            if (dm_exists || lm_exists)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Error: LM/DM Flexible DMAC can not be enabled when LM or DM exists\n");
            }
            if (_SHR_IS_FLAG_SET(group_info->flags, BCM_OAM_GROUP_FLEXIBLE_MAID_20_BYTE))
            {
                extra_data_nof_bits = DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY + DNX_LMDM_FLEXIBLE_DA_BITS;
            }
            else if (_SHR_IS_FLAG_SET(group_info->flags, BCM_OAM_GROUP_FLEXIBLE_MAID_40_BYTE))
            {
                extra_data_nof_bits = DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY +
                    DNX_OAM_MEP_DB_NOF_BITS_IN_PAYLOAD_ENTRY + DNX_LMDM_FLEXIBLE_DA_BITS;
            }
            else if (_SHR_IS_FLAG_SET(group_info->flags, BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE))
            {
                extra_data_nof_bits = DNX_OAM_OAMP_NOF_MAID_BITS + DNX_LMDM_FLEXIBLE_DA_BITS;
            }
            else
            {
                extra_data_nof_bits = DNX_LMDM_FLEXIBLE_DA_BITS;
            }
            extra_data_len = extra_data_nof_bits / DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY;
            num_short_entries_in_bank = dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit) *
                dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
            entry_bank = endpoint_info->id / num_short_entries_in_bank;
            extra_data_header_bank = endpoint_info->extra_data_index / num_short_entries_in_bank;
            if ((entry_bank >= extra_data_header_bank) && (entry_bank < extra_data_header_bank + extra_data_len))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error: for self contained accelerated OAM endpoint, all extra data \n"
                             "entries must not be in the same bank as the main endpoint entry.\n");
            }

        }
        else if (DNX_OAM_LMDM_FLEXIBLE_DA_IS_SET_TO_DISABLE(endpoint_info, sw_endpoint_info))
        {
            if (dm_exists || lm_exists)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Error: LM/DM Flexible DMAC can not be disabled when LM or DM exists\n");
            }
        }
    }

exit:
    SHR_FREE(dm_mep_db_entry);
    SHR_FREE(lm_mep_db_entry);
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Validatation of a non accelerated local endpoint create
 * request.
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] endpoint_info - info structure of the requested
 *        MEP
 *
 * \retval Error indication.
 */
static shr_error_e
dnx_oam_endpoint_create_non_accelerated_local_mep_verify(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (endpoint_info->tx_gport != BCM_GPORT_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "tx_gport is for accelerated endpoint only.\n");
    }
    /** Non accelerated endpoint. Only RX fields should be filled.*/
    if (endpoint_info->ccm_period
        || endpoint_info->int_pri
        || endpoint_info->vlan
        || endpoint_info->inner_pkt_pri
        || endpoint_info->pkt_pri
        || endpoint_info->name
        || endpoint_info->intf_id
        || endpoint_info->egress_label.label != BCM_MPLS_LABEL_INVALID
        || endpoint_info->egress_label.ttl
        || endpoint_info->egress_label.exp
        || endpoint_info->inner_tpid
        || endpoint_info->inner_vlan
        || endpoint_info->outer_tpid
        || !BCM_MAC_IS_ZERO(endpoint_info->src_mac_address) || endpoint_info->endpoint_memory_type)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Non acceleration in HW" " marked but TX fields non zero.\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Validation of an accelerated local endpoint create request.
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] endpoint_info - info structure of the requested MEP
 * \param [in] group_name - group name
 * \param [in] is_48B_maid - 48B MAID group
 *
 * \retval Error indication.
 */
static shr_error_e
dnx_oam_endpoint_create_accelerated_local_mep_verify(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    const uint8 *group_name,
    int is_48B_maid)
{
    int mep_db_threshold;
    int nof_max_endpoints;
    int nof_mep_db_short_entries;
    int mep_db_lim;
    dnx_oam_ma_name_type_e group_name_type;
    bcm_oam_profile_type_t endpoint_profile_type;

    SHR_FUNC_INIT_VARS(unit);

    endpoint_profile_type = DNX_OAM_PROFILE_TYPE_GET(endpoint_info->acc_profile_id);
    mep_db_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
    nof_max_endpoints = dnx_data_oam.oamp.max_nof_endpoint_id_get(unit);
    nof_mep_db_short_entries = dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
    mep_db_lim = dnx_data_oam.oamp.mep_db_version_limitation_get(unit);
    group_name_type = BCM_DNX_OAM_GROUP_NAME_TO_NAME_TYPE(group_name);

    if ((dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
         dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit)) &&
        (endpoint_info->endpoint_memory_type != bcmOamEndpointMemoryTypeSelfContained))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Only self-contained endpoints are allowed in JR1 system headers mode");
    }

    if (endpoint_info->flags & BCM_OAM_ENDPOINT_WITH_ID)
    {
        if (endpoint_info->endpoint_memory_type == bcmOamEndpointMemoryTypeShortEntry)
        {
            if (endpoint_info->id >= mep_db_threshold)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Short Accelerated endpoint id should be in range 0-%d \n\t",
                             (mep_db_threshold - 1));
            }

            /** Limitations for short entry endpoints */
            if (mep_db_lim == 1)
            {
                SHR_IF_ERR_EXIT(dnx_oam_oamp_short_entry_type_verify(unit, endpoint_info->id, endpoint_info));
            }
        }
        else
        {
            /** Check that not all the memory allocated for short entries */
            if (mep_db_threshold == nof_max_endpoints)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Only Short accelerated endpoint could be configured");
            }
            if (endpoint_info->id < mep_db_threshold)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Self-contained/offloaded accelerated endpoint id should be in range %d - %d ",
                             mep_db_threshold, nof_max_endpoints);
            }
            if ((endpoint_info->id % nof_mep_db_short_entries) != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Self-contained/offloaded accelerated endpoint id should be multiply of %d",
                             nof_mep_db_short_entries);
            }
        }

        if ((group_name_type == DNX_OAMP_OAM_MA_NAME_TYPE_ICC) && ((endpoint_info->id % nof_mep_db_short_entries) != 0)
            && !DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info) && !is_48B_maid)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Accelerated endpoint id assigned to ICC based group should be multiply of %d",
                         nof_mep_db_short_entries);
        }
    }

    if (!DNX_OAM_PROFILE_VALID_GET(endpoint_info->acc_profile_id) && !IS_OAM_SERVER(endpoint_info))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "acc_profile_id must be set through bcm_oam_profile_create() API."
                     " It is also recommended to configure actions with bcm_oam_profile_action_set() before creating an endpoint.\n");
    }

    if ((endpoint_info->gport != BCM_GPORT_INVALID) && (endpoint_info->acc_profile_id == BCM_OAM_PROFILE_INVALID) &&
        !IS_OAM_SERVER(endpoint_info))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "if gport is valid, acc_profile_id must also be valid.\n");
    }

    if (DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info))
    {
        if ((endpoint_profile_type != bcmOAMProfileEgressAcceleratedEndpoint) && !IS_OAM_SERVER(endpoint_info))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "acc_profile_id must be set through bcm_oam_profile_create() API "
                         "(type should be bcmOAMProfileEgressAcceleratedEndpoint)."
                         " It is also recommended to configure actions with bcm_oam_profile_action_set() before createing an endpoint.\n");
        }

        if ((endpoint_info->tx_gport != BCM_GPORT_INVALID) && !IS_OAM_SERVER(endpoint_info))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "TX gport must be invalid for UP-MEP, except in server configuration");
        }

        if (endpoint_info->int_pri != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "int_pri must not be set for Up MEP");
        }
    }
    else
    {
        if (endpoint_profile_type != bcmOAMProfileIngressAcceleratedEndpoint)
        {
            if (endpoint_info->gport != BCM_GPORT_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "acc_profile_id must be set through bcm_oam_profile_create() API "
                             "(type should be bcmOAMProfileEgressAcceleratedEndpoint)."
                             " It is also recommended to configure actions with bcm_oam_profile_action_set() before creating an endpoint.\n");
            }
            else
            {
                if (endpoint_info->acc_profile_id != BCM_OAM_PROFILE_INVALID)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "If gport is invalid, acc_profile_id must also be invalid.\n");
                }
            }
        }
        if ((endpoint_info->type != bcmOAMEndpointTypeEthernet) && (BCM_L3_ITF_TYPE_IS_FEC(endpoint_info->intf_id)))
        {
            if (endpoint_info->tx_gport != BCM_GPORT_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Error: intf_id is a FEC but tx_gport is valid");
            }
        }
        else
        {
            if (!BCM_GPORT_IS_SYSTEM_PORT(endpoint_info->tx_gport) && !BCM_GPORT_IS_TRUNK(endpoint_info->tx_gport))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "TX gport must be system port or trunk");
            }
        }

        if (endpoint_info->int_pri > 0xFF)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Supporting range for int_pri is 0-255.\n");
        }

    }

    if (endpoint_info->type == bcmOAMEndpointTypeEthernet)
    {
        if (endpoint_info->outer_tpid == 0 && endpoint_info->vlan != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: VLAN fields inconsistent.\n");
        }

        if (endpoint_info->pkt_pri > 0xf || endpoint_info->inner_pkt_pri > 0xf)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "pkt_pri, inner_pkt_pri may not be greater than 0xf (PCP, DEI are 4 bits total) \n");
        }
    }
    else
    {
        /*
         *BHH endpoint: MPLS or PWE.
 */
        if (!BCM_MAC_IS_ZERO(endpoint_info->src_mac_address))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: src, dst MAC adresses must" " be zero in case of BHH type.\n");
        }

        /** If mpls_out_gport is invalid, Tx relevant fields are ignored */
        if ((endpoint_info->mpls_out_gport != BCM_GPORT_INVALID) &&
            ((endpoint_info->intf_id <= 0) ||
             (endpoint_info->egress_label.label == 0) || (endpoint_info->egress_label.ttl == 0)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: MPLS label fields must be filled.\n");
        }

        if ((endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_SIGNAL)
            && (endpoint_info->endpoint_memory_type != bcmOamEndpointMemoryTypeSelfContained))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error: Signal Degradation feature can be used only for self-contained entries.\n");
        }
    }

    if ((endpoint_info->ccm_period != BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED) &&
        (endpoint_info->ccm_period != BCM_OAM_ENDPOINT_CCM_PERIOD_3MS) &&
        (endpoint_info->ccm_period != BCM_OAM_ENDPOINT_CCM_PERIOD_10MS) &&
        (endpoint_info->ccm_period != BCM_OAM_ENDPOINT_CCM_PERIOD_100MS) &&
        (endpoint_info->ccm_period != BCM_OAM_ENDPOINT_CCM_PERIOD_1S) &&
        (endpoint_info->ccm_period != BCM_OAM_ENDPOINT_CCM_PERIOD_10S) &&
        (endpoint_info->ccm_period != BCM_OAM_ENDPOINT_CCM_PERIOD_1M) &&
        (endpoint_info->ccm_period != BCM_OAM_ENDPOINT_CCM_PERIOD_10M))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: CCM period must one of periods defined by 802.1ag.\n"
                     "Use BCM_OAM_ENDPOINT_CCM_PERIOD_{DISABLED/3MS/10MS/100MS/1S/10S/1M/10M} define");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * General validatation of a local endpoint create request.
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] endpoint_info - info structure of the requested
 * \param [in] group_info - group name
 *        MEP
 *
 * \retval Error indication.
 */
static shr_error_e
dnx_oam_endpoint_create_local_mep_verify(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    const bcm_oam_group_info_t * group_info)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     *  Check ETH endpoint
     */
    if (endpoint_info->type == bcmOAMEndpointTypeEthernet)
    {
        if (endpoint_info->mpls_out_gport != BCM_GPORT_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Endpoint create error: Ethernet endpoint should not have a valid mpls_out_gport.\n");
        }

        /*
         * endpoint_info->gport == BCM_GPORT_INVALID is legal
         */

        if ((endpoint_info->flags2 == BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN) &&
            (endpoint_info->endpoint_memory_type != bcmOamEndpointMemoryTypeSelfContained))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error: Down MEP Egress Injection endpoints could be only self-contained entries.\n");
        }
    }
    /*
     *  Check MPLS endpoint
     */
    else if ((endpoint_info->type == bcmOAMEndpointTypeBHHPwe) ||
             (endpoint_info->type == bcmOAMEndpointTypeBHHPweGAL) ||
             (endpoint_info->type == bcmOAMEndpointTypeBhhSection) ||
             (endpoint_info->type == bcmOAMEndpointTypeBHHMPLS))
    {
        if (DNX_OAM_DISSECT_IS_ENDPOINT_MIP(endpoint_info))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Endpoint create error: Cannot create MIP on MPLS or PWE ports.\n");
        }

        if (endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Endpoint create error: MPLS endpoint may not be UP facing.\n");
        }

        if ((endpoint_info->mpls_out_gport == BCM_GPORT_INVALID) && (endpoint_info->gport == BCM_GPORT_INVALID) &&
            !IS_OAM_SERVER(endpoint_info))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Endpoint create error: MPLS endpoint should have at least on of gport and mpls_out_gport valid.\n");
        }

        if ((endpoint_info->mpls_out_gport != BCM_GPORT_INVALID))
        {
            if (!BCM_GPORT_IS_MPLS_PORT(endpoint_info->mpls_out_gport)
                && !BCM_GPORT_IS_TUNNEL(endpoint_info->mpls_out_gport))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Error: mpls_out_gport must" " be an MPLS port or a tunnel gport\n");
            }
        }

    }
    else if ((endpoint_info->type == bcmOAMEndpointTypeMplsLmDmLsp) ||
             (endpoint_info->type == bcmOAMEndpointTypeMplsLmDmPw) ||
             (endpoint_info->type == bcmOAMEndpointTypeMplsLmDmSection))
    {
        /*
         * RFC 6374 Endpoint
         */
        if (DNX_OAM_DISSECT_IS_ENDPOINT_MIP(endpoint_info))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Endpoint create error: Cannot create MIP on MPLS or PWE ports.\n");
        }

        if (endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Endpoint create error: MPLS endpoint may not be UP facing.\n");
        }
        if ((endpoint_info->mpls_out_gport == BCM_GPORT_INVALID) && (endpoint_info->gport == BCM_GPORT_INVALID))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Endpoint create error: MPLS endpoint should have at least on of gport and mpls_out_gport valid.\n");
        }

        if ((endpoint_info->mpls_out_gport != BCM_GPORT_INVALID))
        {
            if (!BCM_GPORT_IS_MPLS_PORT(endpoint_info->mpls_out_gport)
                && !BCM_GPORT_IS_TUNNEL(endpoint_info->mpls_out_gport))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Error: mpls_out_gport must" " be an MPLS port or a tunnel gport\n");
            }
        }
        if (endpoint_info->level != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "RFC 6374 endpoint should not have level.\n");
        }
        if (endpoint_info->endpoint_memory_type != bcmOamEndpointMemoryTypeSelfContained)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: MplsLmDm endpoints could be only self-contained entries.\n");
        }

    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Endpoint create error: Unsupported endpoint type.\n");
    }

    if ((endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_SIGNAL)
        && (endpoint_info->type != bcmOAMEndpointTypeBhhSection))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Endpoint create error: Signal degradation supported for MPLS section mode only.\n");
    }
    /*
     *  Check that the group exist - needed for both accelerated and not accelerated
     */
    if (!DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
    {
        SHR_IF_ERR_EXIT(dnx_oam_group_verify_is_allocated(unit, endpoint_info->group));
    }

    /*
     *  Check that the counter base is legal
     */
    SHR_IF_ERR_EXIT(dnx_oam_counter_verify(unit, endpoint_info));

    /*
     * Branch out to accelerated/non accelerated checks
     */
    if (DNX_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info))
    {
        SHR_INVOKE_VERIFY_DNX(dnx_oam_endpoint_create_accelerated_local_mep_verify
                              (unit, endpoint_info, group_info->name,
                               DNX_OAM_GROUP_MAID_IS_EXTRA_DATA(group_info->flags)));
    }
    else
    {
        SHR_INVOKE_VERIFY_DNX(dnx_oam_endpoint_create_non_accelerated_local_mep_verify(unit, endpoint_info));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Special verification of local oam endpoint update -
 * some .
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] existing_endpoint_info - info structure for the
 *             existing OAM endpoint to be modified.
 * \param [in] endpoint_info - info structure provided for
 *             modifying the OAM endpoint.
 *
 *
 * \retval Error indication.
 */
static shr_error_e
dnx_oam_endpoint_mod_verify(
    int unit,
    const bcm_oam_endpoint_info_t * existing_endpoint_info,
    const bcm_oam_endpoint_info_t * endpoint_info)
{
    uint8 was_upmep, to_upmep, was_mip, to_mip, was_signal_degradation, to_signal_degradation;
    SHR_FUNC_INIT_VARS(unit);

    was_upmep = DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(existing_endpoint_info);
    to_upmep = DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info);
    was_mip = DNX_OAM_DISSECT_IS_ENDPOINT_MIP(existing_endpoint_info);
    to_mip = DNX_OAM_DISSECT_IS_ENDPOINT_MIP(endpoint_info);
    was_signal_degradation = DNX_OAM_DISSECT_IS_SIGNAL_DETECT(existing_endpoint_info);
    to_signal_degradation = DNX_OAM_DISSECT_IS_SIGNAL_DETECT(endpoint_info);

    if ((was_upmep == TRUE) && (to_upmep == FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: up-MEP cannot be modified to down-MEP.\n");
    }

    if ((was_upmep == FALSE) && (to_upmep == TRUE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: down-MEP cannot be modified to up-MEP.\n");
    }

    if ((was_mip == TRUE) && (to_mip == FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: MIP cannot be modified to MEP.\n");
    }

    if ((was_mip == FALSE) && (to_mip == TRUE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: MEP cannot be modified to MIP.\n");
    }

    if (was_signal_degradation != to_signal_degradation)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Signal Degradation couldn't change.\n");
    }

    if (existing_endpoint_info->type != endpoint_info->type)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: type cannot be updated.\n"
                     "Current type: %d.  New type: %d.\n", existing_endpoint_info->type, endpoint_info->type);
    }

    if (existing_endpoint_info->level != endpoint_info->level)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: MDL cannot be updated.\n"
                     "Current MDL: %d.  New MDL: %d.\n", existing_endpoint_info->level, endpoint_info->level);
    }

    if (existing_endpoint_info->gport != endpoint_info->gport)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: gport cannot be updated.\n"
                     "Current gport: 0x%04X.  New gport: 0x%04X.\n",
                     existing_endpoint_info->gport, endpoint_info->gport);
    }

    if (existing_endpoint_info->group != endpoint_info->group)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: group cannot be updated.\n"
                     "Current group: 0x%04X.  New group: 0x%04X.\n",
                     existing_endpoint_info->group, endpoint_info->group);
    }

    if (existing_endpoint_info->mpls_out_gport != endpoint_info->mpls_out_gport)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: mpls_out_gport cannot be updated.\n"
                     "Current mpls_out_gport: 0x%04X.  New mpls_out_gport: 0x%04X.\n",
                     existing_endpoint_info->mpls_out_gport, endpoint_info->mpls_out_gport);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Get lif information from ports.
 *
 * \param [in] unit -  Relevant unit.
 * \param [in] endpoint_info - info structure of the requested
 *        MEP
 *     gport - gport of which the lif is requested
 *     mpls_out_gport - mpls_out_gport of which the lif
 *        is requested
 *     type - port type
 * \param [out] lif_resolution - Structure of lif resolution
 *
 * \retval Error indication.
 */
static shr_error_e
dnx_oam_lif_resolution_get(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    oam_lif_resolution_t * lif_resolution)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);

    /*
     *  Get local_lif and global_lif
     *
     * LIF resolution table:
     *
     *                                 Ingress Lif-DB        Egress-Lif-DB                   MEP id
     * Ethernet Up                  global-in-lif(gport)   local-out-lif(gport)           local-out-lif(gport)
     * Ethernet Down                global-in-lif(gport)   local-out-lif(gport)           global-in-lif(gport)
     * Ethernet MIP                 global-in-lif(gport)   local-out-lif(gport)           global-in-lif(gport)
     * MPLS/Section/PWE             global-in-lif(gport)   local-out-lif(mpls_out_gport)  global-in-lif(gport)
     * MPLS/Section/PWE - ing only  global-in-lif(gport)   NO                             global-in-lif(gport)
     * MPLS/Section/PWE - eg only   NO                     local-out-lif(mpls_out_gport)  local-out-lif(mpls_out_gport)
 */
    lif_resolution->global_in_lif = DNX_OAM_INVALID_LIF;
    lif_resolution->local_in_lif = DNX_OAM_INVALID_LIF;
    lif_resolution->local_out_lif = DNX_OAM_INVALID_LIF;
    lif_resolution->outlif_dbal_table_id = DBAL_NOF_TABLES;
    lif_resolution->outlif_dbal_result_type = 0;
    if (endpoint_info->gport != BCM_GPORT_INVALID)
    {
        if (endpoint_info->type == bcmOAMEndpointTypeEthernet)
        {
            flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_AND_GLOBAL_LIF;
        }
        else
        {
            flags =
                DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS;
        }
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, endpoint_info->gport, flags, &gport_hw_resources));
        lif_resolution->local_out_lif = gport_hw_resources.local_out_lif;
        lif_resolution->global_in_lif = gport_hw_resources.global_in_lif;
        lif_resolution->local_in_lif = gport_hw_resources.local_in_lif;
        lif_resolution->inlif_dbal_table_id = gport_hw_resources.inlif_dbal_table_id;
        lif_resolution->inlif_dbal_result_type = gport_hw_resources.inlif_dbal_result_type;
        lif_resolution->outlif_dbal_table_id = gport_hw_resources.outlif_dbal_table_id;
        lif_resolution->outlif_dbal_result_type = gport_hw_resources.outlif_dbal_result_type;
    }
    if (endpoint_info->type != bcmOAMEndpointTypeEthernet)
    {
        if (endpoint_info->mpls_out_gport != BCM_GPORT_INVALID)
        {
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                            (unit, endpoint_info->mpls_out_gport,
                             DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS |
                             DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT, &gport_hw_resources));
            lif_resolution->local_out_lif = gport_hw_resources.local_out_lif;
            lif_resolution->outlif_dbal_table_id = gport_hw_resources.outlif_dbal_table_id;
            lif_resolution->outlif_dbal_result_type = gport_hw_resources.outlif_dbal_result_type;
        }
        else
        {
            lif_resolution->local_out_lif = DNX_OAM_INVALID_LIF;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Perform analysis of the lif in regard of the existing MEPs
 *  on that lif.
 *  The result is used during the endpoint create proccess. Also
 *  verify that the new MEP map settles with HLM by MDL MEP
 *  positioning rules.
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] endpoint_info - info structure of the requested
 *        MEP
 * \param [in] lif_analysis - results of analysis performed on
 *        the lifs
 *
 * \retval Error indication.
 */
static shr_error_e
dnx_oam_endpoint_hlm_by_mdl_verify(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    oam_lif_analysis_results_t * lif_analysis)
{
    int index;
    int highest_level_mep_index;

    SHR_FUNC_INIT_VARS(unit);

    /*
     *  Counters may be assigned only on the highest and the 2 lowest level MEPs.
     *  -------------------------------------------------------------------------
     *
     *  Creating the requested MEP might cause existing MEPs to become illegal. Therefore all MEPs
     *  on the LIF should be verified.
     *
     *  Verification algorithm:
     *  Check that there are no meps allocated at illegel levels
 */

    /*
     * Validate the MEPs on the LIF
     */
    if (lif_analysis->nof_levels_with_lm_mep > NOF_OAM_PREFIXES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal LM-MEP create - More than 3 LM-valid MEPs on a LIF are not allowed.\n");
    }

    /*
     * if there are mips on the lif then ignore them when verifying the meps
     */
    highest_level_mep_index = lif_analysis->nof_levels_with_mep - lif_analysis->nof_levels_with_mip - 1;

    /*
     *  Check that the lm_meps are not positioned in 2nd -
     *  (total_meps-2)
     */
    for (index = 0; index < lif_analysis->nof_levels_with_lm_mep; index++)
    {
        if (lif_analysis->lm_meps_by_level[index] != lif_analysis->meps_by_level[highest_level_mep_index])
        {
            /*
             * not the highest
             */
            if (lif_analysis->lm_meps_by_level[index] != lif_analysis->meps_by_level[0])
            {
                /*
                 * not the lowest
                 */
                if (lif_analysis->lm_meps_by_level[index] != lif_analysis->meps_by_level[1])
                {
                    /*
                     * not the second lowest
                     *
                     * Error - Print the right error message
                     */
                    if (endpoint_info->lm_counter_base_id > 0)
                    {
                        /*
                         *  Error - Counter was assigned in a illegal place
                         */
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "Illegal LM-MEP create - Counters may be assigned only on the highest and the 2 lowest level MEPs on the same LIF.\n");
                    }
                    else
                    {
                        /*
                         *  Error - Non-lm-mep was assigned in a illegal place
                         */
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "Illegal MEP create - MEP without counter may not be assigned on the highest and the 2 lowest level MEPs on the same LIF.\n");
                    }

                }
                else
                {
                    /*
                     * second lowest - legal only if the lowest is also lm-mep
                     */
                    if (lif_analysis->level_is_lm[0] == 0)
                    {
                        /*
                         * Print the right error message
                         */
                        if (endpoint_info->lm_counter_base_id > 0)
                        {
                            /*
                             *  Error - Counter was assigned in a illegal place
                             */
                            SHR_ERR_EXIT(_SHR_E_PARAM,
                                         "Illegal LM-MEP create - Counters may be assigned only on the highest and the 2 lowest level MEPs on the same LIF.\n");
                        }
                        else
                        {
                            /*
                             *  Error - Non-lm-mep was assigned in a illegal place
                             */
                            SHR_ERR_EXIT(_SHR_E_PARAM,
                                         "Illegal MEP create - MEP without counter may not be assigned on the highest and the 2 lowest level MEPs on the same LIF.\n");
                        }
                    }
                }
            }
        }
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function will add MA name data to UMC table
 *          in the HW at MEP Index location if group is ICC based.
 * \param [in] unit - Number of hardware unit used.
 * \param [in] group_name -
 *  pointer to  group name.
 * \param [in] oamp_mep_db_index -
 *  OAMP MEP DB index.
 * \param [in] create -
 *  Accelreated endpoint create/delete.
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_oam_handle_acceleraed_endpoint_group_config(
    int unit,
    const uint8 *group_name,
    uint32 oamp_mep_db_index,
    uint8 create)
{
    oam_group_umc_data_t group_umc_data = { 0 };
    int i = 0;
    uint32 entry_handle_id;
    dnx_oam_ma_name_type_e group_name_type = BCM_DNX_OAM_GROUP_NAME_TO_NAME_TYPE(group_name);
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * program UMC table if group is ICC based format
     */
    if (group_name_type == DNX_OAMP_OAM_MA_NAME_TYPE_ICC)
    {
        if (create)
        {
            for (i = 0; i < DNX_OAMP_OAM_GROUP_UMC_DATA_LENGTH; i++)
            {
                group_umc_data[i] =
                    group_name[DNX_OAMP_OAM_GROUP_UMC_DATA_OFFSET + (DNX_OAMP_OAM_GROUP_UMC_DATA_LENGTH - 1) - i];
            }
        }
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_UMC_TABLE, &entry_handle_id));
        /*
         * Key
         * UMC table support only mep_id's with modulo num of sub enties in one entry(mep_db) equal 0
 */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_INDEX,
                                   oamp_mep_db_index / dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit));
        /*
         * Values
         */
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_UMC, INST_SINGLE, group_umc_data);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Add a new mep to configured_lif structure to prepare it for
 * analysis and hlm_by_mdl verification
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] endpoint_info - info structure of the requested
 *        MEP
 * \param [in,out] configured_lif - Structure of configured lif
 *        parameters including parameters of the meps allocated
 *        on that lif. The requested MEP's parameters are added
 *        to the structure.
 *
 * \retval Error indication.
 */
static shr_error_e
dnx_oam_add_mep_to_configured_lif(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    oam_lif_configured_lif_t * configured_lif)
{
    uint8 is_upmep;
    uint32 mpls_tp_mdl_ignore = 0;

    SHR_FUNC_INIT_VARS(unit);

    configured_lif->new_level = endpoint_info->level;

    /*
     * When mpls_tp_mdl_ignore is enabled, MDL is forced to 7 in OAM_LIF_DB
     * if MEP type is MPLS LSP, PWE and MPLS section)
     */
    SHR_IF_ERR_EXIT(algo_oam_db.mpls_tp_mdl_ignore.get(unit, &mpls_tp_mdl_ignore));
    if (mpls_tp_mdl_ignore)
    {
        if (endpoint_info->type == bcmOAMEndpointTypeBhhSection ||
            endpoint_info->type == bcmOAMEndpointTypeBHHMPLS || endpoint_info->type == bcmOAMEndpointTypeBHHPwe)
        {
            configured_lif->new_level = 7;
        }
    }

    /*
     * Add the requested MEP to configured_lif (already checked that no MEP was
     * previously allocated on that level)
     */
    is_upmep = DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info);
    if (is_upmep)
    {
        if (configured_lif->mep_dir[configured_lif->new_level] == MEP_DIRECTION_IS_DOWN)
        {
            configured_lif->mep_dir[configured_lif->new_level] = MEP_DIRECTION_IS_BOTH_DIRECTIONS;
            configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_INGRESS][configured_lif->new_level] =
                DBAL_ENUM_FVAL_MDL_MP_TYPE_ACTIVE_MEP;
            configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_EGRESS][configured_lif->new_level] =
                DBAL_ENUM_FVAL_MDL_MP_TYPE_ACTIVE_MEP;
        }
        else
        {
            configured_lif->mep_dir[configured_lif->new_level] = MEP_DIRECTION_IS_UP;
            configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_INGRESS][configured_lif->new_level] =
                DBAL_ENUM_FVAL_MDL_MP_TYPE_PASSIVE_MEP;
            configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_EGRESS][configured_lif->new_level] =
                DBAL_ENUM_FVAL_MDL_MP_TYPE_ACTIVE_MEP;
        }
    }
    else
    {
        if (endpoint_info->flags & BCM_OAM_ENDPOINT_INTERMEDIATE)
        {
            configured_lif->mep_dir[configured_lif->new_level] = MEP_IS_MIP;
            configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_INGRESS][configured_lif->new_level] =
                DBAL_ENUM_FVAL_MDL_MP_TYPE_MIP;
            configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_EGRESS][configured_lif->new_level] =
                DBAL_ENUM_FVAL_MDL_MP_TYPE_MIP;
        }
        else
        {
            if (configured_lif->mep_dir[configured_lif->new_level] == MEP_DIRECTION_IS_UP)
            {
                configured_lif->mep_dir[configured_lif->new_level] = MEP_DIRECTION_IS_BOTH_DIRECTIONS;
                configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_INGRESS][configured_lif->new_level] =
                    DBAL_ENUM_FVAL_MDL_MP_TYPE_ACTIVE_MEP;
                configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_EGRESS][configured_lif->new_level] =
                    DBAL_ENUM_FVAL_MDL_MP_TYPE_ACTIVE_MEP;
            }
            else
            {
                configured_lif->mep_dir[configured_lif->new_level] = MEP_DIRECTION_IS_DOWN;
                configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_INGRESS][configured_lif->new_level] =
                    DBAL_ENUM_FVAL_MDL_MP_TYPE_ACTIVE_MEP;
                /*
                 * Even though for MPLS there is not supposed to be a MEP, this
                 * is done to avoid collisions with other endpoints defined
                 * on the LIF.
                 */

                configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_EGRESS][configured_lif->new_level] =
                    DBAL_ENUM_FVAL_MDL_MP_TYPE_PASSIVE_MEP;
            }
        }
    }

    configured_lif->counter_base[configured_lif->new_level] = endpoint_info->lm_counter_base_id;
    configured_lif->counter_if[configured_lif->new_level] = endpoint_info->lm_counter_if;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Add a new mep to default oam profile structure to prepare it
 * for hw write
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] endpoint_info - info structure of the requested
 *        MEP
 * \param [in,out] default_mep_info - Structure of default mep
 *        parameters
 *
 * \retval Error indication.
 */
static shr_error_e
dnx_oam_add_mep_to_default_oam_profile(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    oam_default_mep_info_t * default_mep_info)
{
    int index;
    SHR_FUNC_INIT_VARS(unit);

    for (index = endpoint_info->level; index >= 0; index--)
    {
        default_mep_info->mdl_mp_type[index] = DBAL_ENUM_FVAL_MDL_MP_TYPE_ACTIVE_MEP;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Remove a mep from default oam profile structure to prepare it
 * for hw write
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] endpoint_info - info structure of the requested
 *        MEP
 * \param [in,out] default_mep_info - Structure of default mep
 *        parameters
 *
 * \retval Error indication.
 */
static shr_error_e
dnx_oam_remove_mep_from_default_oam_profile(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    oam_default_mep_info_t * default_mep_info)
{
    int index;
    SHR_FUNC_INIT_VARS(unit);

    for (index = endpoint_info->level; index >= 0; index--)
    {
        default_mep_info->mdl_mp_type[index] = DBAL_ENUM_FVAL_MDL_MP_TYPE_NO_MEP;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Remove a mep from configured_lif structure
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] endpoint_info - info structure of the requested
 *        MEP
 * \param [in,out] configured_lif - Structure of configured lif
 *        parameters including parameters of the meps allocated
 *        on that lif. The requested MEP's parameters are
 *        removed from the structure.
 *
 * \retval Error indication.
 */
static shr_error_e
dnx_oam_remove_mep_from_configured_lif(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    oam_lif_configured_lif_t * configured_lif)
{
    uint8 is_upmep;

    SHR_FUNC_INIT_VARS(unit);

    configured_lif->new_level = endpoint_info->level;

    /*
     * Remove the requested MEP from configured_lif
     */
    is_upmep = DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info);

    if (is_upmep)
    {
        /*
         * Removing upmep:
         * if the currently there are 2 meps (upmep + downmep), after the removal only downmep stays,
         * if it was the only mep with this level then it is removed.
 */
        if (configured_lif->mep_dir[endpoint_info->level] == MEP_DIRECTION_IS_BOTH_DIRECTIONS)
        {
            configured_lif->mep_dir[endpoint_info->level] = MEP_DIRECTION_IS_DOWN;
            configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_INGRESS][endpoint_info->level] =
                DBAL_ENUM_FVAL_MDL_MP_TYPE_ACTIVE_MEP;
            configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_EGRESS][endpoint_info->level] =
                DBAL_ENUM_FVAL_MDL_MP_TYPE_PASSIVE_MEP;
        }
        else
        {
            configured_lif->mep_dir[endpoint_info->level] = NO_MEP;
            configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_INGRESS][endpoint_info->level] =
                DBAL_ENUM_FVAL_MDL_MP_TYPE_NO_MEP;
            configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_EGRESS][endpoint_info->level] =
                DBAL_ENUM_FVAL_MDL_MP_TYPE_NO_MEP;
            configured_lif->counter_base[endpoint_info->level] = 0;
            configured_lif->counter_if[endpoint_info->level] = endpoint_info->lm_counter_if;
        }
    }
    else
    {
        /*
         * Removing downmep:
         * if the currently there are 2 meps (upmep + downmep), after the removal only upmep stays,
         * if it was the only mep with this level then it is removed.
 */
        if (configured_lif->mep_dir[endpoint_info->level] == MEP_DIRECTION_IS_BOTH_DIRECTIONS)
        {
            configured_lif->mep_dir[endpoint_info->level] = MEP_DIRECTION_IS_UP;
            configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_INGRESS][endpoint_info->level] =
                DBAL_ENUM_FVAL_MDL_MP_TYPE_PASSIVE_MEP;
            configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_EGRESS][endpoint_info->level] =
                DBAL_ENUM_FVAL_MDL_MP_TYPE_ACTIVE_MEP;
        }
        else
        {
            configured_lif->mep_dir[endpoint_info->level] = NO_MEP;
            configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_INGRESS][endpoint_info->level] =
                DBAL_ENUM_FVAL_MDL_MP_TYPE_NO_MEP;
            configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_EGRESS][endpoint_info->level] =
                DBAL_ENUM_FVAL_MDL_MP_TYPE_NO_MEP;
            configured_lif->counter_base[endpoint_info->level] = 0;
            configured_lif->counter_if[endpoint_info->level] = 0;
        }
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * LIF specific validatation of a local endpoint create request.
 * Validate that:
 *  - no MEP exist on the same level with the same direction (1)
 *  - no MEP exists on higher level than a MIP (2)
 *  - no MEP and MIP created at the same level (3)
 *  - MIP facing down (4)
 *  - MIP has no counter (5)
 *  - MEP at the same level has the same counter (6)
 *  - same counter_if was not used for other mep on the lif (7)
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] endpoint_info - info structure of the requested
 *        MEP
 * \param [in] configured_lif - Structure of configured lif
 *        parameters including parameters of the meps allocated
 *        on that lif
 *
 * \retval Error indication.
 */
static shr_error_e
dnx_oam_endpoint_create_lif_verify(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    const oam_lif_configured_lif_t * configured_lif)
{
    int level;
    uint8 is_upmep;
    uint8 is_mip;
    uint8 is_acc;
    uint8 counter_if;
    uint8 counter_base;
    uint8 nof_configured_lm_meps;
    uint8 is_hlm_disable;
    SHR_FUNC_INIT_VARS(unit);

    if (endpoint_info->gport != BCM_GPORT_INVALID)
    {
        if (configured_lif->mp_profile[DNX_OAM_CLASSIFIER_INGRESS] == BCM_OAM_PROFILE_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Ingress LIF provided is not binded to an OAM profile. Use bcm_oam_lif_profile_set first.\n");
        }
        if (endpoint_info->type == bcmOAMEndpointTypeEthernet)
        {
            /** Ethernet MEPs are symetric */
            if (configured_lif->mp_profile[DNX_OAM_CLASSIFIER_EGRESS] == BCM_OAM_PROFILE_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Egress LIF provided is not binded to an OAM profile. Use bcm_oam_lif_profile_set first.\n");
            }
        }
    }
    else
    {
        if (endpoint_info->mpls_out_gport != BCM_GPORT_INVALID)
        {
            if (configured_lif->mp_profile[DNX_OAM_CLASSIFIER_EGRESS] == BCM_OAM_PROFILE_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Egress LIF provided is not binded to an OAM profile. Use bcm_oam_lif_profile_set first.\n");
            }
        }
    }
    /*
     *  Check that no MEP exist on this LIF with the same level (1)
     */
    is_upmep = DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info);
    is_mip = DNX_OAM_DISSECT_IS_ENDPOINT_MIP(endpoint_info);

    if (!_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_OAM_ENDPOINT_REPLACE) &&
        (((configured_lif->mep_dir[endpoint_info->level] == MEP_DIRECTION_IS_UP) && is_upmep) ||
         ((configured_lif->mep_dir[endpoint_info->level] == MEP_DIRECTION_IS_DOWN) && !is_upmep) ||
         (configured_lif->mep_dir[endpoint_info->level] == MEP_DIRECTION_IS_BOTH_DIRECTIONS) ||
         (configured_lif->mep_dir[endpoint_info->level] == MEP_IS_MIP)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Illegal MEP create - MEP with same level (%d) and direction already exists on LIF.\n",
                     endpoint_info->level);
    }

    if (is_mip)
    {
        /*
         * Upmep MIP check (4)
         */
        if (is_upmep)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal MEP create - MIP may not be facing up.\n");
        }
        /*
         * MIP with counter check (5)
         */
        if (endpoint_info->lm_counter_base_id > 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal MEP create - MIP may not assign a counter .\n");
        }
        /*
         *  MEP above MIP check (2)
         *  MEP and MIP with the same level check (3)
         */
        for (level = DNX_OAM_MAX_MDL; level > endpoint_info->level; level--)
        {
            if ((configured_lif->mep_dir[level] == MEP_DIRECTION_IS_UP) ||
                (configured_lif->mep_dir[level] == MEP_DIRECTION_IS_DOWN) ||
                (configured_lif->mep_dir[level] == MEP_DIRECTION_IS_BOTH_DIRECTIONS))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Illegal MEP create - MIP with lower or equal level (%d) than MEP (%d) is not allowed.\n",
                             endpoint_info->level, level);
            }
        }
    }
    else
    {
        for (level = 0; level < endpoint_info->level; level++)
        {
            if (configured_lif->mep_dir[level] == MEP_IS_MIP)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Illegal MEP create - MEP with higher or equal level (%d) than MIP (%d) is not allowed.\n",
                             level, endpoint_info->level);
            }
        }
    }

    /*
     *  Counter base validation (6)
     */
    if (((configured_lif->mep_dir[endpoint_info->level] == MEP_DIRECTION_IS_UP) ||
         (configured_lif->mep_dir[endpoint_info->level] == MEP_DIRECTION_IS_DOWN)) &&
        !_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_OAM_ENDPOINT_REPLACE))
    {
        if (!((configured_lif->counter_base[endpoint_info->level] == endpoint_info->lm_counter_base_id) &&
              (configured_lif->counter_if[endpoint_info->level] == endpoint_info->lm_counter_if)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Illegal MEP create - Different counter base was already defined on the same lif and level.\n");
        }
    }
    /*
     * Counter if validation (7)
     * Validation for both for counter_if 3 J2C, JR2_B0) and valid counter-if (JR2_A0) cases
     * for MEPs without counter.
     */
    counter_if = endpoint_info->lm_counter_if;
    counter_base = endpoint_info->lm_counter_base_id;

    nof_configured_lm_meps = 0;
    for (level = 0; level <= DNX_OAM_MAX_MDL; level++)
    {
        if (configured_lif->counter_base[level] > 0)
        {
            nof_configured_lm_meps++;
        }
    }

    for (level = 0; level <= DNX_OAM_MAX_MDL; level++)
    {
        if (configured_lif->counter_if[level] == counter_if)
        {
            if ((counter_base > 0) && (configured_lif->counter_base[level] > 0))
            {
                /*
                 *  To create a MEP on the opposite side of as existing MEP, same counter_if and counter_base should be used
                 */
                if (configured_lif->counter_base[level] != counter_base)
                {
                    if (((configured_lif->mep_dir[level] == MEP_DIRECTION_IS_UP) && (is_upmep)) ||
                        ((configured_lif->mep_dir[level] == MEP_DIRECTION_IS_DOWN) && (!is_upmep)) ||
                        (configured_lif->mep_dir[level] == MEP_DIRECTION_IS_BOTH_DIRECTIONS))
                    {
                        if (!(_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_OAM_ENDPOINT_REPLACE)))
                        {
                            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal MEP create - Counter if %d is already in use.\n",
                                         counter_if);
                        }
                        else if ((endpoint_info->level != level))
                        {
                            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal MEP create - Counter if %d is already in use.\n",
                                         counter_if);
                        }
                    }
                }
                else
                {
                    is_acc = DNX_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info);
                    if ((configured_lif->mp_profile[DNX_OAM_CLASSIFIER_INGRESS] == BCM_OAM_PROFILE_INVALID) &&
                        (configured_lif->mp_profile[DNX_OAM_CLASSIFIER_EGRESS] == BCM_OAM_PROFILE_INVALID))
                    {
                        is_hlm_disable = 0;
                    }
                    else
                    {
                        if (configured_lif->mp_profile[DNX_OAM_CLASSIFIER_INGRESS] != BCM_OAM_PROFILE_INVALID)
                        {
                            SHR_IF_ERR_EXIT(dnx_oam_sw_db_hlm_mode_get
                                            (unit, configured_lif->mp_profile[DNX_OAM_CLASSIFIER_INGRESS], 1, is_acc,
                                             &is_hlm_disable));
                        }
                        else
                        {
                            SHR_IF_ERR_EXIT(dnx_oam_sw_db_hlm_mode_get
                                            (unit, configured_lif->mp_profile[DNX_OAM_CLASSIFIER_EGRESS], 1, is_acc,
                                             &is_hlm_disable));
                        }
                    }

                    if (!is_hlm_disable)
                    {
                        /*
                         * Same counter_if and same counter_base: if it's on the same level - legal counter_base, counter-if
                         */
                        if (level != endpoint_info->level)
                        {
                            SHR_ERR_EXIT(_SHR_E_PARAM,
                                         "Illegal MEP create - this counter is already in use on different level. Counter if %d, counter base %d is already in use.\n",
                                         counter_if, counter_base);
                        }
                    }
                }
            }
            else
            {
                if (dnx_data_oam.general.oam_force_interface_for_meps_without_counter_get(unit) == 1)
                {

                    /*
                     * Same counter_if -
                     * At least one of counter_base or configured_lif->counter_base[level] is 0 -
                     *
                     * This is allowed only if all 3 prefixes are going to be occupied with valid counters.
                     * Print error if less than 2 prefixes already occupied.
                     * For example: In case this is the third MEP with counter, it is allowed to have
                     * additional MEPs without counter that have the same counter_if.
                     */
                    if (nof_configured_lm_meps < 2)
                    {
                        if ((counter_base != configured_lif->counter_base[level])
                            && (configured_lif->mep_dir[level] != NO_MEP)
                            && (configured_lif->mep_dir[level] != MEP_IS_MIP))
                        {
                            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal MEP create - Counter if %d is already in use.\n",
                                         counter_if);
                        }
                    }
                }
            }
        }
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * LIF specific validatation of a default endpoint create
 * request.
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] endpoint_info - info structure of the requested
 *        MEP
 * \param [in] default_mep_info - Structure of default mep
 *        parameters
 *
 * \retval Error indication.
 */
static shr_error_e
dnx_oam_endpoint_create_default_mep_verify(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    const oam_default_mep_info_t * default_mep_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * No counters on default MEP
     */
    if ((endpoint_info->lm_counter_base_id > 0) || (endpoint_info->lm_counter_if > 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal Default MEP create - Counters are not allowed on default MEP.\n");
    }

    /*
     * No MIPs on default MEP
     */
    if (DNX_OAM_DISSECT_IS_ENDPOINT_MIP(endpoint_info))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal Default MEP create - MIP may not be configured as default MEP.\n");
    }

    if (endpoint_info->gport != BCM_GPORT_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Default MEP may not be assigned to a specific gport.\n");
    }

    if (DNX_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Default MEP may not be accelerated.\n");
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * LIF specific validatation of a default endpoint destroy
 * request.
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] endpoint_info - info structure of the requested
 *        MEP
 * \param [in] default_mep_info - Structure of default mep
 *        parameters
 *
 * \retval Error indication.
 */
static shr_error_e
dnx_oam_endpoint_default_mep_destroy_verify(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    const oam_default_mep_info_t * default_mep_info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (default_mep_info->mdl_mp_type[0] != DBAL_ENUM_FVAL_MDL_MP_TYPE_ACTIVE_MEP)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Illegal Default MEP destroy - Default MEP was not configured on this default MEP profile.\n");
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * endpoint_id allocation for unaccalarated MEP
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] endpoint_info - info structure of the requested
 *        MEP
 * \param [in] lif_resolution - Structure of lif resolution
 *
 * \retval Error indication.
 */
static shr_error_e
dnx_oam_local_endpoint_fill_id(
    int unit,
    bcm_oam_endpoint_info_t * endpoint_info,
    const oam_lif_resolution_t * lif_resolution)
{
    uint8 is_up;
    int oam_lif;

    SHR_FUNC_INIT_VARS(unit);

    is_up = DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info);

    /*
     *
     * LIF resolution table:
     *
     *                                 oam-lif for MEP id
     *                                 ------------------
     * Ethernet Up                    local-out-lif(gport)
     * Ethernet Down                  global-in-lif(gport)
     * Ethernet MIP                   global-in-lif(gport)
     * MPLS/Section/PWE               global-in-lif(gport)
     * MPLS/Section/PWE - ing only    global-in-lif(gport)
     * MPLS/Section/PWE - eg only     local-out-lif(mpls_out_gport)
 */

    if (is_up)
    {
        oam_lif = lif_resolution->local_out_lif;
    }
    else
    {
        if (lif_resolution->global_in_lif != DNX_OAM_INVALID_LIF)
        {
            oam_lif = lif_resolution->global_in_lif;
        }
        else
        {
            /*
             * For MPLS - egress only case
             */
            oam_lif = lif_resolution->local_out_lif;
        }
    }

    if (!DNX_OAM_IS_DEFAULT_MEP(endpoint_info))
    {
        endpoint_info->id = DNX_OAM_UNACCELERATED_MEP_INDEX_GET(oam_lif, endpoint_info->level, is_up);
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Store endpoint information in sw_state:
 * Endpoint information is stored in
 * endpoint_info_sw_state
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] endpoint_info - info structure of the requested
 *        MEP
 * \param [in] tcam_index - index of entry in trap TCAM table
 *
 * \retval Error indication.
 */
static shr_error_e
dnx_oam_endpoint_info_store_in_sw_state(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    uint8 tcam_index)
{
    uint32 endpoint_id;
    bcm_oam_group_info_t group_info;
    dnx_oam_endpoint_info_t sw_endpoint_info;
    int is_maid20 = 0, is_maid48 = 0;
    int is_mip;
    int is_up;
    int is_egress_injection;

    SHR_FUNC_INIT_VARS(unit);
    sal_memset(&group_info, 0, sizeof(bcm_oam_group_info_t));
    sal_memset(&sw_endpoint_info, 0, sizeof(sw_endpoint_info));
    endpoint_id = endpoint_info->id;

    if (!DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
    {
        SHR_IF_ERR_EXIT(dnx_oam_group_info_get(unit, endpoint_info->group, &group_info));

        is_maid20 = _SHR_IS_FLAG_SET(group_info.flags, BCM_OAM_GROUP_FLEXIBLE_MAID_20_BYTE);
        is_maid48 = _SHR_IS_FLAG_SET(group_info.flags, BCM_OAM_GROUP_FLEXIBLE_MAID_40_BYTE) ||
            _SHR_IS_FLAG_SET(group_info.flags, BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE);
    }
    is_mip = DNX_OAM_DISSECT_IS_ENDPOINT_MIP(endpoint_info);
    is_up = DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info);
    is_egress_injection = DNX_OAM_DISSECT_IS_DOWN_MEP_EGRESS_INJECTION(endpoint_info);

    sw_endpoint_info.sw_state_flags = 0;
    if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_OAM_ENDPOINT_REPLACE))
    {
        SHR_IF_ERR_EXIT(dnx_oam_sw_state_endpoint_info_get(unit, endpoint_info->id, &sw_endpoint_info));
    }
    if (is_mip)
    {
        sw_endpoint_info.sw_state_flags |= DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_INTERMEDIATE;
    }
    else if (is_up)
    {
        sw_endpoint_info.sw_state_flags |= DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_UP_FACING;
    }
    else if (is_egress_injection)
    {
        sw_endpoint_info.sw_state_flags |= DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_DOWN_EGRESS_INJECTION;
    }

    if (DNX_OAM_DISSECT_IS_SIGNAL_DETECT(endpoint_info))
    {
        sw_endpoint_info.sw_state_flags |= DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_SIGNAL_DETECT;
        sw_endpoint_info.rx_signal = endpoint_info->rx_signal;
        sw_endpoint_info.tx_signal = endpoint_info->tx_signal;
    }
    if (DNX_OAM_DISSECT_IS_ENDPOINT_ENABLED_TX_STATISTICS(endpoint_info))
    {
        sw_endpoint_info.sw_state_flags |= DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_TX_STATISTICS;
    }

    if (DNX_OAM_DISSECT_IS_ENDPOINT_ENABLED_RX_STATISTICS(endpoint_info))
    {
        sw_endpoint_info.sw_state_flags |= DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_RX_STATISTICS;
    }

    if (DNX_OAM_DISSECT_IS_ENDPOINT_ENABLED_PER_OPCODE_STATISTICS(endpoint_info))
    {
        sw_endpoint_info.sw_state_flags |= DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_PER_OPCODE_STATISTICS;
    }

    if (DNX_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info))
    {
        sw_endpoint_info.sw_state_flags |= DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_ACCELERATED;
    }
    if (DNX_OAM_DISSECT_IS_ENDPOINT_PCP_BASED(endpoint_info))
    {
        sw_endpoint_info.sw_state_flags |= DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_PCP_BASED;
    }

    if (BCM_GPORT_IS_TRUNK(endpoint_info->tx_gport))
    {
        sw_endpoint_info.sw_state_flags |= DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_TX_GPORT_IS_TRUNK;
    }

    sw_endpoint_info.type = endpoint_info->type;
    sw_endpoint_info.group = endpoint_info->group;
    sw_endpoint_info.level = endpoint_info->level;
    sw_endpoint_info.gport = endpoint_info->gport;

    sw_endpoint_info.lm_counter_idx = endpoint_info->lm_counter_base_id;
    sw_endpoint_info.lm_counter_if = endpoint_info->lm_counter_if;
    sw_endpoint_info.mpls_out_gport = endpoint_info->mpls_out_gport;
    sw_endpoint_info.server_trap = endpoint_info->remote_gport;

    /** Used for following features:
     * 1. LMDM Flexible DA
     * 2. Signal Detect
     */
    if (DNX_OAM_DISSECT_IS_EXTRA_DATA_PER_ENDPOINT(endpoint_info))
    {
        sw_endpoint_info.extra_data_index = endpoint_info->extra_data_index;
        sw_endpoint_info.extra_data_length = DNX_OAM_CCM_ENDPOINT_EXTRA_DATA_LENGHT_CALC(is_maid20, is_maid48);
    }
    else
    {
        sw_endpoint_info.extra_data_index = 0;
        sw_endpoint_info.extra_data_length = 0;
    }

    if (IS_OAM_SERVER(endpoint_info))
    {
        /** For OAM server, store OAMP trap TCAM table entry index */
        sw_endpoint_info.tcam_index = tcam_index;
    }

    SHR_IF_ERR_EXIT(dnx_oam_sw_state_endpoint_info_set(unit, endpoint_id, &sw_endpoint_info,
                                                       _SHR_IS_FLAG_SET(endpoint_info->flags,
                                                                        BCM_OAM_ENDPOINT_REPLACE)));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Store default endpoint information in sw_state:
 * Endpoint information is stored in
 * endpoint_info_sw_state
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] endpoint_info - info structure of the requested
 *        MEP
 *
 * \retval Error indication.
 */
static shr_error_e
dnx_oam_default_endpoint_info_store_in_sw_state(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info)
{
    uint32 endpoint_id;
    dnx_oam_endpoint_info_t sw_endpoint_info;
    int is_up;

    SHR_FUNC_INIT_VARS(unit);

    endpoint_id = endpoint_info->id;
    sal_memset(&sw_endpoint_info, 0, sizeof(dnx_oam_endpoint_info_t));

    is_up = DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info);

    if (is_up)
    {
        sw_endpoint_info.sw_state_flags |= DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_UP_FACING;
    }
    sw_endpoint_info.type = endpoint_info->type;
    sw_endpoint_info.group = endpoint_info->group;
    sw_endpoint_info.level = endpoint_info->level;
    sw_endpoint_info.gport = endpoint_info->gport;
    sw_endpoint_info.mpls_out_gport = endpoint_info->mpls_out_gport;

    SHR_IF_ERR_EXIT(dnx_oam_sw_state_endpoint_info_set(unit, endpoint_id, &sw_endpoint_info,
                                                       _SHR_IS_FLAG_SET(endpoint_info->flags,
                                                                        BCM_OAM_ENDPOINT_REPLACE)));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Store oam lif information in sw_state: The endpoint_id is
 * added to lif_db_sw_state
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] endpoint_info - info structure of the requested
 *        MEP
 * \param [in] lif_resolution - Structure of lif resolution
 * \param [in] endpoint_id - id of the endpoint to be added to
 *        the oam_lif_db_sw_state. ( Use
 *        BCM_OAM_ENDPOINT_INVALID to remove endpoint from
 *        oam_lif)
 *
 * \retval Error indication.
 */
static shr_error_e
dnx_oam_lif_info_store_in_sw_state(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    const oam_lif_resolution_t * lif_resolution,
    const uint32 endpoint_id)
{
    dnx_oam_lif_info_t sw_lif_info;
    int is_up;

    SHR_FUNC_INIT_VARS(unit);

    is_up = DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info);
    /*
     * Store lif info in sw_state
     */
    if (lif_resolution->local_out_lif != DNX_OAM_INVALID_LIF)
    {
        /*
         * upmeps and mips for egress_only
         */
        if ((is_up) || (lif_resolution->global_in_lif == DNX_OAM_INVALID_LIF))
        {
            SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_oam_sw_state_oam_lif_info_get
                                      (unit, lif_resolution->local_out_lif, DNX_OAM_CLASSIFIER_EGRESS, &sw_lif_info),
                                      _SHR_E_NOT_FOUND);
            sw_lif_info.endpoint_id[endpoint_info->level] = endpoint_id;
            SHR_IF_ERR_EXIT(dnx_oam_sw_state_oam_lif_info_set
                            (unit, DBAL_COMMIT_FORCE, lif_resolution->local_out_lif, DNX_OAM_CLASSIFIER_EGRESS,
                             &sw_lif_info));
        }
    }
    if (lif_resolution->global_in_lif != DNX_OAM_INVALID_LIF)
    {
        if (!is_up)
        {
            SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_oam_sw_state_oam_lif_info_get
                                      (unit, lif_resolution->global_in_lif, DNX_OAM_CLASSIFIER_INGRESS, &sw_lif_info),
                                      _SHR_E_NOT_FOUND);
            sw_lif_info.endpoint_id[endpoint_info->level] = endpoint_id;
            SHR_IF_ERR_EXIT(dnx_oam_sw_state_oam_lif_info_set
                            (unit, DBAL_COMMIT_FORCE, lif_resolution->global_in_lif, DNX_OAM_CLASSIFIER_INGRESS,
                             &sw_lif_info));
        }
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Store endpoint information in sw_state:
 * Endpoint information is stored in
 * endpoint_info_sw_state and than the endpoint_id
 * is added to lif_db_sw_state
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] endpoint_info - info structure of the requested
 *        MEP
 * \param [in] lif_resolution - Structure of lif resolution
 * \param [in] tcam_index - index of entry in trap TCAM table
 *
 * \retval Error indication.
 */
static shr_error_e
dnx_oam_endpoint_info_store_sw(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    const oam_lif_resolution_t * lif_resolution,
    uint8 tcam_index)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * First set endpoint info, then add endpoint_id to lif_info
     */
    SHR_IF_ERR_EXIT(dnx_oam_endpoint_info_store_in_sw_state(unit, endpoint_info, tcam_index));

    if (!_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_OAM_ENDPOINT_REPLACE) && !IS_OAM_SERVER(endpoint_info))
    {
        /*
         * Store lif info in sw_state
         */
        SHR_IF_ERR_EXIT(dnx_oam_lif_info_store_in_sw_state(unit, endpoint_info, lif_resolution, endpoint_info->id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Store default endpoint information in sw_state:
 * Endpoint information is stored in
 * endpoint_info_sw_state
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] endpoint_info - info structure of the requested
 *        MEP
 *
 * \retval Error indication.
 */
static shr_error_e
dnx_oam_default_endpoint_info_store_sw(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * First set endpoint info, then add endpoint_id to lif_info
     */
    SHR_IF_ERR_EXIT(dnx_oam_default_endpoint_info_store_in_sw_state(unit, endpoint_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Store endpoint information in sw_state:
 * Endpoint information is stored in
 * endpoint_info_sw_state and than the endpoint_id
 * is added to lif_db_sw_state
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] endpoint_info - info structure of the requested
 *        MEP
 * \param [in] lif_resolution - Structure of lif resolution
 *
 * \retval Error indication.
 */
static shr_error_e
dnx_oam_endpoint_info_remove_sw(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    const oam_lif_resolution_t * lif_resolution)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * First Remove lif info in sw_state
     */
    SHR_IF_ERR_EXIT(dnx_oam_lif_info_store_in_sw_state(unit, endpoint_info, lif_resolution, BCM_OAM_ENDPOINT_INVALID));
    /*
     * Delete endpoint info, then add endpoint_id to lif_info
     */
    SHR_IF_ERR_EXIT(dnx_oam_endpoint_info_remove_from_sw_state(unit, endpoint_info->id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Update the mp_type_map in lif_db (hw)
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] flags -    may be DNX_OAM_CLASSIFIER_INGRESS or
 *                        DNX_OAM_CLASSIFIER_EGRESS
 * \param [in] oam_lif - oam_lif of the entry to be moved
 * \param [in] configured_lif - Structure of configured lif
 *        parameters including parameters of the meps allocated
 *        on that lif
 * \param [in] prefix - prefix of the entry to be updated
 *
 * \retval Error indication.
 */
static shr_error_e
dnx_oam_update_mdl_mp_type_in_lif_db(
    int unit,
    int flags,
    int oam_lif,
    const oam_lif_configured_lif_t * configured_lif,
    uint8 prefix)
{
    dnx_oam_lif_entry_key_t oam_lif_entry_key;
    dnx_oam_lif_entry_value_t oam_lif_entry_value;
    int is_ingress;

    SHR_FUNC_INIT_VARS(unit);

    is_ingress = flags & DNX_OAM_CLASSIFIER_INGRESS;

    oam_lif_entry_key.core_id = _SHR_CORE_ALL;
    oam_lif_entry_key.oam_key_prefix = prefix;
    oam_lif_entry_key.flags = flags;
    oam_lif_entry_key.oam_lif_id = oam_lif;
    oam_lif_entry_value.mdl_mp_type[configured_lif->new_level] =
        configured_lif->mdl_mp_type[is_ingress][configured_lif->new_level];
    SHR_IF_ERR_EXIT(dnx_oam_lif_modify_mdl_mp_type
                    (unit, configured_lif->new_level, &oam_lif_entry_key, &oam_lif_entry_value));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam_lif_db table management helper fubction:
 * Add a MEP into oam-lif-db (hw) entry. if used with
 * DBAL_COMMIT flag the entry must not be occupied.
 * if used with DBAL_COMMIT_UPDATE, the entry
 * must exist.
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] flags -    may be DNX_OAM_CLASSIFIER_INGRESS or
 *                        DNX_OAM_CLASSIFIER_EGRESS
 *
 * \param [in] dbal_oper_flags - may be DBAL_COMMIT
 *                         or DBAL_COMMIT_UPDATE
 * \param [in] oam_lif - oam_lif of the entry to be added
 * \param [in,out] lif_db_current - oam_lif_db configuration
 *        before the endpoint_add.The struct is updated with the
 *        perfomed change in lif-db for the next iteration
 * \param [in] lif_db_after - oam_lif_db configuration after the
 *        endpoint_add
 * \param [in] configured_lif - Structure of configured lif
 *        parameters including parameters of the meps allocated
 *        on that lif
 * \param [in] prefix - prefix of the entry to be added
 *
 * \retval Error indication.
 */
static shr_error_e
dnx_oam_add_mep_to_lif_db_table(
    int unit,
    int flags,
    dbal_entry_action_flags_e dbal_oper_flags,
    int oam_lif,
    oam_lif_db_state_t * lif_db_current,
    const oam_lif_db_state_t * lif_db_after,
    const oam_lif_configured_lif_t * configured_lif,
    uint8 prefix)
{
    dnx_oam_lif_entry_key_t oam_lif_entry_key;
    dnx_oam_lif_entry_value_t oam_lif_entry_value;
    int level;
    int is_ingress;

    SHR_FUNC_INIT_VARS(unit);

    is_ingress = flags & DNX_OAM_CLASSIFIER_INGRESS;

    oam_lif_entry_key.core_id = _SHR_CORE_ALL;
    oam_lif_entry_key.oam_key_prefix = prefix;
    oam_lif_entry_key.flags = flags;
    oam_lif_entry_key.oam_lif_id = oam_lif;
    oam_lif_entry_value.counter_base = lif_db_after->counter_base_in_prefix[is_ingress][prefix];
    oam_lif_entry_value.counter_interface = lif_db_after->counter_if_in_prefix[is_ingress][prefix];

    if ((dnx_data_oam.general.oam_force_interface_for_meps_without_counter_get(unit) != 1) || (is_ingress))
    {
        /*
         * Invalid counter base (for MEPs without counter) is represented as 3 in lif-db.
         */
        if (oam_lif_entry_value.counter_base == 0)
        {
            oam_lif_entry_value.counter_interface = INVALID_COUNTER_IF;
        }
    }

    oam_lif_entry_value.mp_profile = configured_lif->mp_profile[is_ingress];
    for (level = 0; level <= DNX_OAM_MAX_MDL; level++)
    {
        oam_lif_entry_value.mdl_mp_type[level] = configured_lif->mdl_mp_type[is_ingress][level];
    }
    SHR_IF_ERR_EXIT(dnx_oam_lif_add(unit, dbal_oper_flags, &oam_lif_entry_key, &oam_lif_entry_value));

    /*
     * Update lif_analysis_current
     */
    lif_db_current->counter_base_in_prefix[is_ingress][prefix] =
        lif_db_after->counter_base_in_prefix[is_ingress][prefix];
    lif_db_current->counter_if_in_prefix[is_ingress][prefix] = lif_db_after->counter_if_in_prefix[is_ingress][prefix];
    if ((prefix + 1) > lif_db_current->occupied_prefixes[is_ingress])
    {
        lif_db_current->occupied_prefixes[is_ingress]++;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam_lif_db table management helper fubction: Modify
 * counter_base and counter_if of oam-lif-db (hw) entry.
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] flags -    may be DNX_OAM_CLASSIFIER_INGRESS or
 *                        DNX_OAM_CLASSIFIER_EGRESS
 * \param [in] oam_lif - oam_lif of the entry to be added
 * \param [in,out] lif_db_current - oam_lif_db configuration
 *        before the endpoint_add.The struct is updated with the
 *        perfomed change in lif-db for the next iteration
 * \param [in] lif_db_after - oam_lif_db configuration after the
 *        endpoint_add
 * \param [in] configured_lif - Structure of configured lif
 *        parameters including parameters of the meps allocated
 *        on that lif
 * \param [in] prefix - prefix of the entry to be added
 *
 * \retval Error indication.
 */
static shr_error_e
dnx_oam_modify_counter(
    int unit,
    int flags,
    int oam_lif,
    oam_lif_db_state_t * lif_db_current,
    const oam_lif_db_state_t * lif_db_after,
    const oam_lif_configured_lif_t * configured_lif,
    uint8 prefix)
{
    dnx_oam_lif_entry_key_t oam_lif_entry_key;
    dnx_oam_lif_entry_value_t oam_lif_entry_value;
    int level;
    int is_ingress;

    SHR_FUNC_INIT_VARS(unit);

    is_ingress = flags & DNX_OAM_CLASSIFIER_INGRESS;

    oam_lif_entry_key.core_id = _SHR_CORE_ALL;
    oam_lif_entry_key.oam_key_prefix = prefix;
    oam_lif_entry_key.flags = flags;
    oam_lif_entry_key.oam_lif_id = oam_lif;
    oam_lif_entry_value.counter_base = lif_db_after->counter_base_in_prefix[is_ingress][prefix];
    oam_lif_entry_value.counter_interface = lif_db_after->counter_if_in_prefix[is_ingress][prefix];

    if ((dnx_data_oam.general.oam_force_interface_for_meps_without_counter_get(unit) != 1) || (is_ingress))
    {
        /*
         * Invalid counter base (for MEPs without counter) is represented as 3 in lif-db.
         */
        if (oam_lif_entry_value.counter_base == 0)
        {
            oam_lif_entry_value.counter_interface = INVALID_COUNTER_IF;
        }
    }

    oam_lif_entry_value.mp_profile = configured_lif->mp_profile[is_ingress];
    for (level = 0; level <= DNX_OAM_MAX_MDL; level++)
    {
        oam_lif_entry_value.mdl_mp_type[level] = configured_lif->mdl_mp_type[is_ingress][level];
    }
    SHR_IF_ERR_EXIT(dnx_oam_lif_add(unit, DBAL_COMMIT_UPDATE, &oam_lif_entry_key, &oam_lif_entry_value));

    /*
     * Update lif_analysis_before
     */
    lif_db_current->counter_base_in_prefix[is_ingress][prefix] =
        lif_db_after->counter_base_in_prefix[is_ingress][prefix];
    lif_db_current->counter_if_in_prefix[is_ingress][prefix] = lif_db_after->counter_if_in_prefix[is_ingress][prefix];
    if ((prefix + 1) > lif_db_current->occupied_prefixes[is_ingress])
    {
        lif_db_current->occupied_prefixes[is_ingress]++;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *
 * oam_lif_db table management
 *
 * Place lif_analysis_after-> counter_base_in_prefix[is_ingress][prefix] in
 * prefix
 *
 * Adds an entry with the given counter_base into the given prefix,
 * if the entry is occupied with another counter it calls itself
 * recuresively, until an empty or irrelevant entry is found
 *
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] flags -    may be DNX_OAM_CLASSIFIER_INGRESS or
 *                        DNX_OAM_CLASSIFIER_EGRESS
 * \param [in] oam_lif - oam_lif of the entry to be deleted
 * \param [in] configured_lif - Structure of configured lif
 *        parameters including parameters of the meps allocated
 *        on that lif
 * \param [in,out] lif_db_current - oam_lif_db configuration
 *        before the endpoint_add.The struct is updated with the
 *        perfomed change in lif-db for the next iteration
 * \param [in] lif_db_after - oam_lif_db configuration after the
 *        endpoint_add
 * \param [in] prefix - prefix of the entry to be added
 *
 * \retval Error indication.
 */
static shr_error_e
dnx_oam_mep_to_prefix(
    int unit,
    int flags,
    int oam_lif,
    const oam_lif_configured_lif_t * configured_lif,
    oam_lif_db_state_t * lif_db_current,
    const oam_lif_db_state_t * lif_db_after,
    uint8 prefix)
{
    int mep_allocated;
    int duplicate_counter;
    uint8 index;
    uint8 temp_prefix;
    int is_ingress;

    SHR_FUNC_INIT_VARS(unit);

/*
 * copy MEP
 * to entry --> entry is free --> add_MEP (1)
 *          \
 *           -> entry is --> occupied by   --> modify_mdl_mp_type (2)
 *              occupied  \   the same
 *                         \  counter
 *                          \
 *                           --> occupied by      occupying          occupied by     overwrite
 *                               different   -->  MEP is needed  --> duplicate   --> entry with (3)
 *                               counter     \    in new lif-db  \   counter         new counter
 *                                            \                   \
 *                                             \                   \
 *                                              \                    --> occupied by
 *                                               \                       non duplicate --> copy occupying MEP      (4)
 *                                                \                                        to entry (recorsive call)
 *                                                 \
 *                                                  --> occupying
 *                                                      MEP is not
 *                                                      needed in new --> overwrite
 *                                                      lif-db            entry with (5)
 *                                                                        counter
 */
    is_ingress = flags & DNX_OAM_CLASSIFIER_INGRESS;

    lif_db_current->prefix_is_modified[is_ingress][prefix] = 1;

    /*
     * if counter base needs to be copied on itself, update_mdl_mp_type
     */
    if (!
        ((lif_db_after->counter_base_in_prefix[is_ingress][prefix] ==
          lif_db_current->counter_base_in_prefix[is_ingress][prefix])
         && (lif_db_after->counter_if_in_prefix[is_ingress][prefix] ==
             lif_db_current->counter_if_in_prefix[is_ingress][prefix])))
    {
        /*
         * if counter base needs to be copied to an empty place, just add entry
         */
        if ((prefix + 1) <= lif_db_current->occupied_prefixes[is_ingress])
        {
            index = 0;
            mep_allocated = 0;
            while ((index < lif_db_after->occupied_prefixes[is_ingress]) && (mep_allocated == 0))
            {
                /*
                 * if the requested prefix is occupied with counter that should be allocated in other prefix
                 * recursively call mep_to_prefix to replace it
                 */
                if ((lif_db_after->counter_base_in_prefix[is_ingress][index] ==
                     lif_db_current->counter_base_in_prefix[is_ingress][prefix])
                    && (lif_db_after->counter_if_in_prefix[is_ingress][index] ==
                        lif_db_current->counter_if_in_prefix[is_ingress][prefix]))
                {
                    /*
                     * Check if it's duplicate counter for hlm_by_lif.
                     * if so, the entry can be overwritten, if not it must be moved
                     */
                    duplicate_counter = 0;
                    for (temp_prefix = prefix + 1; temp_prefix < NOF_OAM_PREFIXES; temp_prefix++)
                    {
                        if ((lif_db_current->counter_base_in_prefix[is_ingress][prefix] ==
                             lif_db_current->counter_base_in_prefix[is_ingress][temp_prefix]) &&
                            (lif_db_current->counter_if_in_prefix[is_ingress][prefix] ==
                             lif_db_current->counter_if_in_prefix[is_ingress][temp_prefix]))
                        {
                            duplicate_counter = 1;
                        }
                    }
                    if (duplicate_counter == 0)
                    {
                        /*
                         * the prefix is occupied with a MEP that needs to be moved (4)
                         */
                        SHR_IF_ERR_EXIT(dnx_oam_mep_to_prefix
                                        (unit, flags, oam_lif, configured_lif, lif_db_current, lif_db_after, index));
                        lif_db_current->counter_base_in_prefix[is_ingress][index] =
                            lif_db_after->counter_base_in_prefix[is_ingress][index];
                        lif_db_current->counter_if_in_prefix[is_ingress][index] =
                            lif_db_after->counter_if_in_prefix[is_ingress][index];
                        if ((index + 1) > lif_db_current->occupied_prefixes[is_ingress])
                        {
                            lif_db_current->occupied_prefixes[is_ingress]++;
                        }

                        /*
                         * Now the counter can be modified to the new value (recursion stop condition 3)
                         */
                        SHR_IF_ERR_EXIT(dnx_oam_modify_counter
                                        (unit, flags, oam_lif, lif_db_current, lif_db_after, configured_lif, prefix));
                        mep_allocated = 1;
                    }
                    else
                    {
                        /*
                         * Entry is not needed in the new lif-db so can the new mep can be allocated instead. (3)
                         */
                        SHR_IF_ERR_EXIT(dnx_oam_modify_counter
                                        (unit, flags, oam_lif, lif_db_current, lif_db_after, configured_lif, prefix));
                        mep_allocated = 1;
                    }
                    SHR_EXIT();
                }
                index++;
            }
            /*
             * Entry is not needed in the new lif-db so can the new mep can be allocated instead. (5)
             */
            SHR_IF_ERR_EXIT(dnx_oam_modify_counter
                            (unit, flags, oam_lif, lif_db_current, lif_db_after, configured_lif, prefix));
        }
        else
        {
            /*
             * Entry is free - add mep (1)
             */
            SHR_IF_ERR_EXIT(dnx_oam_add_mep_to_lif_db_table
                            (unit, flags, DBAL_COMMIT, oam_lif, lif_db_current, lif_db_after, configured_lif, prefix));
        }
    }
    else
    {
        if ((prefix + 1) > lif_db_current->occupied_prefixes[is_ingress])
        {
            /*
             * Entry is free - add mep (1)
             */
            SHR_IF_ERR_EXIT(dnx_oam_add_mep_to_lif_db_table
                            (unit, flags, DBAL_COMMIT, oam_lif, lif_db_current, lif_db_after, configured_lif, prefix));
        }
        else
        {
            /*
             * update mdl_mp_type_map of prefix (2)
             */
            SHR_IF_ERR_EXIT(dnx_oam_update_mdl_mp_type_in_lif_db(unit, flags, oam_lif, configured_lif, prefix));
        }
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Clean lif_db entry (hw) from specific prefix
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] flags -    may be DNX_OAM_CLASSIFIER_INGRESS or
 *                        DNX_OAM_CLASSIFIER_EGRESS
 * \param [in] oam_lif - oam_lif of the entry to be added
 * \param [in] prefix - prefix of the entry to be added
 *
 * \retval Error indication.
 */
static shr_error_e
dnx_oam_clean_prefix(
    int unit,
    int flags,
    int oam_lif,
    uint8 prefix)
{
    dnx_oam_lif_entry_key_t oam_lif_entry_key;

    SHR_FUNC_INIT_VARS(unit);

    oam_lif_entry_key.core_id = _SHR_CORE_ALL;
    oam_lif_entry_key.oam_key_prefix = prefix;
    oam_lif_entry_key.flags = flags;
    oam_lif_entry_key.oam_lif_id = oam_lif;

    SHR_IF_ERR_EXIT(dnx_oam_lif_delete(unit, &oam_lif_entry_key));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam_lif_db table management main function: Add, remove or
 * update a MEP into oam_lif_db (hw). Manage lif entries (all
 * perfixes) of the lif.
 *
 * Entries are managed using the following helper functions:
 *
 * dnx_oam_add_mep_to_lif_db_table - add an entry
 *
 * dnx_oam_modify_counter -  modify counter_base and couunter_if
 *                          of an entry
 *
 * dnx_oam_update_mdl_mp_type_in_lif_db - update mdl_mp_type
 *
 *
 *  - Counting during transition time:
 *  Adding a MEP will not affect counting of data packets.
 *  OAM packets will be counted on best-effort basis as
 *  HW limitations prevent correct counting of OAM packets at
 *  all scenarios.
 *
 *  - mp_type_map updating:
 * In steady-state only the mp_type_map of prefix 0 is being
 * used. Still,we keep the mp_type_map field of all prefixes
 * updated.
 *
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] flags -    may be DNX_OAM_CLASSIFIER_INGRESS or
 *                        DNX_OAM_CLASSIFIER_EGRESS
 * \param [in] oam_lif - oam_lif of the entry to be added
 * \param [in] configured_lif - Structure of configured lif
 *        parameters including parameters of the meps allocated
 *        on that lif
 * \param [in] lif_db_before - oam_lif_db configuration before
 *        the endpoint_add
 * \param [in] lif_db_after - oam_lif_db configuration after the
 *        endpoint_add
 *
 * \retval Error indication.
 */
static shr_error_e
dnx_oam_lif_db_endpoint_update(
    int unit,
    int flags,
    int oam_lif,
    const oam_lif_configured_lif_t * configured_lif,
    const oam_lif_db_state_t * lif_db_before,
    const oam_lif_db_state_t * lif_db_after)
{
    uint8 prefix;
    oam_lif_db_state_t lif_db_current;
    int is_ingress;

    SHR_FUNC_INIT_VARS(unit);

    is_ingress = flags & DNX_OAM_CLASSIFIER_INGRESS;

    lif_db_current.counter_base_in_prefix[is_ingress][0] = lif_db_before->counter_base_in_prefix[is_ingress][0];
    lif_db_current.counter_if_in_prefix[is_ingress][0] = lif_db_before->counter_if_in_prefix[is_ingress][0];
    lif_db_current.counter_base_in_prefix[is_ingress][1] = lif_db_before->counter_base_in_prefix[is_ingress][1];
    lif_db_current.counter_if_in_prefix[is_ingress][1] = lif_db_before->counter_if_in_prefix[is_ingress][1];
    lif_db_current.counter_base_in_prefix[is_ingress][2] = lif_db_before->counter_base_in_prefix[is_ingress][2];
    lif_db_current.counter_if_in_prefix[is_ingress][2] = lif_db_before->counter_if_in_prefix[is_ingress][2];
    lif_db_current.occupied_prefixes[is_ingress] = lif_db_before->occupied_prefixes[is_ingress];
    /*
     * Mark all prefixes as not modidified
     */
    for (prefix = PREFIX_HIGH; prefix < NOF_OAM_PREFIXES; prefix++)
    {
        lif_db_current.prefix_is_modified[is_ingress][prefix] = 0;
    }

    /*
     *
     * Rules for allocating MEPs on lif-db are:
     *
     * MIPs:
     * -----
     * MIPs only lif: Single entry (PREFIX_HIGH)
     * MIPs and MEPs : No allocation for the MIPs, only MEPs will allocated by the rules for MEP allocation
     *                 (MIPs are ignored)
     * MEPs:
     * -----
     * 1 MEP - will be allocated on PREFIX_HIGH
     * 2 MEPs- higher level will be allocated on PREFIX_HIGH
     *         lower   level will be allocated on PREFIX_MID
     * 3 MEPs- highest level will be allocated on PREFIX_HIGH
     *         middle  level will be allocated on PREFIX_MID
     *         lowest  level will be allocated on PREFIX_LOW
     * More than 3 MEPs-
     *         highest    level will be allocated on PREFIX_HIGH
     *         2nd lowest level will be allocated on PREFIX_MID
     *         lowest     level will be allocated on PREFIX_LOW
     *         (Those are the levels thay may have counters assigned)
     *
     * Entries that doesn't need to be occupied by these rules should be cleared
     *
     * MP_TYPE_MAP: should always be updated for all occupied entries
 */

    /*
     * Allocate all MEPS (lif_analysis_after) in all non modified occupied prefixes
     * if a prefix was previously allocated no need to call dnx_oam_mep_to_prefix
     * again.
     */
    for (prefix = PREFIX_HIGH; prefix < lif_db_after->occupied_prefixes[is_ingress]; prefix++)
    {
        if (lif_db_current.prefix_is_modified[is_ingress][prefix] == 0)
        {
            SHR_IF_ERR_EXIT(dnx_oam_mep_to_prefix
                            (unit, flags, oam_lif, configured_lif, &lif_db_current, lif_db_after, prefix));
        }
    }
    /*
     * if prefix was deleted - Clean lif-db entry
     */
    if (lif_db_after->occupied_prefixes[is_ingress] < lif_db_before->occupied_prefixes[is_ingress])
    {
        SHR_IF_ERR_EXIT(dnx_oam_clean_prefix(unit, flags, oam_lif, prefix));
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Marks OAM LIF SET bit in the LIF table
 *        for the in LIF and out LIF (gport, mpls_out_gport).
 *        Modifies oam_set in hw only for the first lm_mep (set)
 *        and the last lm_mep (reset)
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] lif_analysis_before - results of analysis
 *        performed on the lifs before the endpoint change
 *       (endpoint_create, endpoint_delete, lm_add...)
 * \param [in] lif_analysis_after - results of analysis
 *        performed on the lifs after the endpoint change
 *       (endpoint_create, endpoint_delete, lm_add...)
 * \param [in] lif_resolution - Structure of lif resolution
 * \param [in] set_flag - TRUE for setting oam_lif_set (used in
 *        endpoint_add), and FALSE for clearing oam_lif_set bit
 *        (used in endpoint_destroy)
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_mark_lif_as_oam(
    int unit,
    const oam_lif_analysis_results_t * lif_analysis_before,
    const oam_lif_analysis_results_t * lif_analysis_after,
    const oam_lif_resolution_t * lif_resolution,
    const uint8 set_flag)
{
    uint32 entry_handle_id;
    int last_lm_mep;
    int first_lm_mep;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Modify oam_set in hw only for the first lm_mep (set) and the last lm_mep (clear)
     */
    first_lm_mep = (lif_analysis_before->nof_levels_with_lm_mep == 0)
        && (lif_analysis_after->nof_levels_with_lm_mep > 0);
    last_lm_mep = (lif_analysis_before->nof_levels_with_lm_mep > 0)
        && (lif_analysis_after->nof_levels_with_lm_mep == 0);

    if (((set_flag == TRUE) && first_lm_mep) || ((set_flag == FALSE) && last_lm_mep))
    {
        if (lif_resolution->local_out_lif != DNX_OAM_INVALID_LIF)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, lif_resolution->outlif_dbal_table_id, &entry_handle_id));

         /** write map value  */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, lif_resolution->local_out_lif);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         lif_resolution->outlif_dbal_result_type);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LIF_SET, INST_SINGLE, set_flag);

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
        if (lif_resolution->global_in_lif != DNX_OAM_INVALID_LIF)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, lif_resolution->inlif_dbal_table_id, &entry_handle_id));

         /** write map value  */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, lif_resolution->local_in_lif);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         lif_resolution->inlif_dbal_result_type);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LIF_SET, INST_SINGLE, set_flag);

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

    SHR_EXIT();
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add all required entries to hw tables,
 *          to add a default MEP. Tables to be updated:
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] default_mep_info - Structure of default mep
 *        parameters
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_endpoint_update_default_mep_db_hw(
    int unit,
    const oam_default_mep_info_t * default_mep_info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (default_mep_info->is_upmep)
    {
        SHR_IF_ERR_EXIT(dnx_oam_default_endpoint_update
                        (unit, DNX_OAM_CLASSIFIER_EGRESS, default_mep_info->oam_default_mep,
                         default_mep_info->mp_profile, default_mep_info->mdl_mp_type));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_oam_default_endpoint_update
                        (unit, DNX_OAM_CLASSIFIER_INGRESS, default_mep_info->oam_default_mep,
                         default_mep_info->mp_profile, default_mep_info->mdl_mp_type));
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add all required entries to lif-db hw tables,
 *          to add a new MEP.
 *          Tables to be updated:
 *             - INGRESS OAM LIF DB
 *             - EGRESS OAM LIF DB
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] lif_resolution - Structure of lif resolution
 * \param [in] configured_lif - Structure of configured lif
 *        parameters including parameters of the meps allocated
 *        on that lif
 * \param [in] lif_db_before - oam_lif_db configuration before
 *        the endpoint_add
 * \param [in] lif_db_after - oam_lif_db configuration after the
 *        endpoint_add
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_endpoint_update_lif_db_hw(
    int unit,
    const oam_lif_resolution_t * lif_resolution,
    const oam_lif_configured_lif_t * configured_lif,
    const oam_lif_db_state_t * lif_db_before,
    const oam_lif_db_state_t * lif_db_after)
{
    SHR_FUNC_INIT_VARS(unit);

    if (lif_resolution->global_in_lif != DNX_OAM_INVALID_LIF)
    {
        /*
         * OAM LIF DB - INGRESS
         */
        SHR_IF_ERR_EXIT(dnx_oam_lif_db_endpoint_update
                        (unit, DNX_OAM_CLASSIFIER_INGRESS, lif_resolution->global_in_lif,
                         configured_lif, lif_db_before, lif_db_after));
    }

    if (lif_resolution->local_out_lif != DNX_OAM_INVALID_LIF)
    {
        /*
         * OAM LIF DB - EGRESS
         */
        SHR_IF_ERR_EXIT(dnx_oam_lif_db_endpoint_update
                        (unit, DNX_OAM_CLASSIFIER_EGRESS, lif_resolution->local_out_lif,
                         configured_lif, lif_db_before, lif_db_after));
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set the Mpls Dm timestamp format.
 *
 * \param [in] unit - unit number
 * \param [in] is_1588 - IEEE-1588 - "1", NTP - "0"
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
dnx_oamp_dbal_rfc_6374_timestamp_format_set(
    int unit,
    uint32 is_1588)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_INIT_GENERAL_CFG, &entry_handle_id));
    if (is_1588)
    {
        /**
        * OTF = 3 (IEEE-1588)
        * RTF = 3 (IEEE-1588)
        * RPTF = 3 (IEEE-1588)
        * Reserved = 0
        */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RFC_6374_DM_DW_1, INST_SINGLE, 0x33300000);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RFC_6374_DM_RSP_DW_1, INST_SINGLE, 0x33300000);
    }
    else
    {
        /**
        * OTF = 3 (IEEE-1588)
        * RTF = 3 (IEEE-1588)
        * RPTF = 3 (IEEE-1588)
        * Reserved = 0
        */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RFC_6374_DM_DW_1, INST_SINGLE, 0x22200000);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RFC_6374_DM_RSP_DW_1, INST_SINGLE, 0x22200000);
    }

    /** Update entry */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - Add the new endpoint to group linked list,
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - info structure of the requested
 *        MEP
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_endpoint_add_to_group_list(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Add mep to Group linked list. This is executed at each Endpoint_create. */
    SHR_IF_ERR_EXIT(dnx_oam_sw_db_ma_to_mep_insert_mep(unit, endpoint_info->group, endpoint_info->id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Remove the new endpoint from group linked list,
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - info structure of the requested
 *        MEP
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_endpoint_remove_from_group_list(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Delete mep from Group linked list. This is executed at each Endpoint_destroy. */
    SHR_IF_ERR_EXIT(dnx_oam_sw_db_ma_to_mep_delete_mep(unit, endpoint_info->group, endpoint_info->id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Find a short entry to use for an OAM endpoint
 *          that will work correctly in light of the JR2 A0
 *          hardware, where the type used for a short
 *          entry endpoint is always the type in sub-index 0:
 *          either find an entry where sub-index 0 has the
 *          same type, or provide a sub-index 0 entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in,out] endpoint_info - info structure of the requested
 *        MEP
 * \param [in] group_name - MAID for this endpoint
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e
dnx_oam_endpoint_workaround_fill_id(
    int unit,
    bcm_oam_endpoint_info_t * endpoint_info,
    const uint8 *group_name)
{
    dnx_oam_oamp_mep_db_args_t *possible_leader_info = NULL;
    uint32 possible_leader_id = 0, possible_appropriate_entry_id;
    int nof_sub_entries, leader_index, index, full_entry_threshold, nof_icc_entries;
    dbal_enum_value_field_oamp_mep_type_e req_mep_type;
    uint8 possible_leader_found = TRUE, appropriate_entry_not_found = TRUE;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_oam_oamp_endpoint_bcm_mep_type_to_mep_db_mep_type(unit, endpoint_info->type, &req_mep_type));

    SHR_ALLOC_SET_ZERO(possible_leader_info, sizeof(dnx_oam_oamp_mep_db_args_t),
                       "OAM endpoint data read for type comparison", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    nof_sub_entries = dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
    full_entry_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
    nof_icc_entries = full_entry_threshold / nof_sub_entries;

    if (nof_icc_entries == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "An OAM short entry endpoint of type %d cannot be created", endpoint_info->type);
    }

    for (leader_index = 0;
         (leader_index < nof_icc_entries) &&
         ((possible_leader_found == TRUE) && (appropriate_entry_not_found == TRUE)); leader_index++)
    {
        /** Get the next allocated "ICC" short entry */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_get_next_icc_entry(unit, &possible_leader_id, &possible_leader_found));

        if (possible_leader_found == TRUE)
        {
            /** Get that entry's type and compare it to the configured type */
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, possible_leader_id, possible_leader_info));
            if (possible_leader_info->mep_type == req_mep_type)
            {
                /** Endpoint type matches.  Is there a sub-entry available? */
                for (index = 1; index < nof_sub_entries; index++)
                {
                    possible_appropriate_entry_id = possible_leader_id + index;
                    SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_is_allocated
                                    (unit, possible_appropriate_entry_id, &appropriate_entry_not_found));
                    if (appropriate_entry_not_found == FALSE)
                        break;
                }
            }
            /** Make sure the search starts from the next possible leader */
            possible_leader_id += nof_sub_entries;
        }
    }

    if (appropriate_entry_not_found == TRUE)
    {
        /**
         * No available short entries found where the leading entry has the correct type
         * Find a free leading entry and use it
         */
        SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_get_free_icc_id
                        (unit, &possible_appropriate_entry_id, &appropriate_entry_not_found));
        if (appropriate_entry_not_found == TRUE)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "An OAM short entry endpoint of type %d cannot be created",
                         endpoint_info->type);
        }
    }

    /** Found an entry - allocate it as if using "WITH_ID" */
    endpoint_info->id = possible_appropriate_entry_id;
    endpoint_info->flags |= BCM_OAM_ENDPOINT_WITH_ID;
    /** 48B MAID - always 0 in Jer2 a0 (last parameter) */
    SHR_IF_ERR_EXIT(dnx_oam_local_accelerated_endpoint_fill_id(unit, endpoint_info, group_name, 0));
    endpoint_info->flags &= ~BCM_OAM_ENDPOINT_WITH_ID;

exit:
    SHR_FREE(possible_leader_info);
    SHR_FUNC_EXIT;
}

/**
 * \brief - If an endpoint is a server endpoint, read
 *          index from SW state table and get trap
 *          code.
 *
 * \param [in] endpoint_info - info structure of the requested
 *        MEP
  * \param [in] sw_endpoint_info - SW info structure of the requested
 *        MEP
 * \param [out] mep_hw_profiles_data - Pointer to struct where data
 *        will be written
 *
 * \return
 *
 * \see
 *   * None
 */
static void
dnx_oam_trap_tcam_data_get(
    const bcm_oam_endpoint_info_t * endpoint_info,
    const dnx_oam_endpoint_info_t * sw_endpoint_info,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_data)
{
    if (IS_OAM_SERVER(endpoint_info))
    {
        mep_hw_profiles_data->tcam_index = sw_endpoint_info->tcam_index;
        mep_hw_profiles_data->tcam_entry.trap_code = BCM_GPORT_TRAP_GET_ID(endpoint_info->remote_gport);
    }
}

/**
 * \brief - If an endpoint is a server endpoint,
 *          and last reference to TCAM entry was
 *          cleared, delete the entry from the
 *          physical table
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] mep_hw_profiles_data - Pointer to struct where data
 *        is stored
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_trap_tcam_entry_clear(
    int unit,
    const oam_oamp_profile_hw_data_t * mep_hw_profiles_data)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(mep_hw_profiles_data->flags, DNX_OAM_OAMP_PROFILE_TRAP_TCAM_FLAG))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_TYPE_TRAP_CODE_TCAM, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, mep_hw_profiles_data->tcam_index);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function checks whether the MPLS-LM-DM
 *          endpoint being created is being associated
 *          with an existing BFD endpoint.  If it is,
 *          some parameters are verified.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - info structure of the requested
 *        MEP
 * \param [out] is_bfd_alloc - TRUE if BFD endpoint exists,
 *                             FALSE otherwise
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_mpls_lm_dm_verify(
    int unit,
    bcm_oam_endpoint_info_t * endpoint_info,
    uint8 *is_bfd_alloc)
{
    bcm_bfd_endpoint_info_t *bfd_endpoint_info = NULL;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_is_allocated(unit, endpoint_info->id, is_bfd_alloc));
    if (*is_bfd_alloc)
    {
        SHR_ALLOC_SET_ZERO(bfd_endpoint_info, sizeof(bcm_bfd_endpoint_info_t),
                           "BFD endpoint data read for comparison", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT(bcm_bfd_endpoint_get(unit, endpoint_info->id, bfd_endpoint_info));
        if (BFD_TYPE_NOT_PWE(bfd_endpoint_info->type))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error: attempting to associate an MPLS-LM-DM OAM EP to and existing non-PWE BFD EP");
        }
        if (endpoint_info->gport != bfd_endpoint_info->gport)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error: MPLS-LM-DM EP that has the same ID as a BFD over PWE EP must also "
                         "have the same in-LIF");
        }
        if ((endpoint_info->tx_gport != bfd_endpoint_info->tx_gport) ||
            (endpoint_info->intf_id != bfd_endpoint_info->egress_if))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error: MPLS-LM-DM OAM EP that has the same ID as a BFD over PWE EP must also "
                         "have the same out-LIF");
        }
    }

exit:
    SHR_FREE(bfd_endpoint_info);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Main function for local MEP create.
 *          This function calls to sub-functions that
 *          verifies create request, gets current lif
 *          status, and adds the new MEP to sw and hw
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - info structure of the requested
 *        MEP
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_local_mep_create(
    int unit,
    bcm_oam_endpoint_info_t * endpoint_info)
{
    int mdl = 0;
    int is_48B_maid = 0;
    int oam_lif, mep_db_lim;
    uint32 mpls_tp_mdl_ignore = 0;
    bcm_oam_endpoint_info_t *existing_endpoint_info = NULL;
    dnx_oam_endpoint_info_t *sw_endpoint_info = NULL;
    oam_lif_resolution_t *lif_resolution = NULL;
    oam_lif_configured_lif_t *configured_lif = NULL;
    oam_lif_analysis_results_t *lif_analysis_before = NULL;
    oam_lif_analysis_results_t *lif_analysis_after = NULL;
    oam_oamp_profile_hw_data_t *mep_hw_profiles_write_data = NULL, *mep_hw_profiles_delete_data = NULL;
    bcm_oam_group_info_t *group_info = NULL;
    dnx_oam_oamp_ccm_endpoint_t *mep_db_entry = NULL;
    bcm_oam_endpoint_t classifier_ep_id;
    dnx_oamp_mep_db_memory_type_t memory_type;
    uint32 rfc_6374_pointer_id = 0;
    uint8 is_bfd_alloc = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC_SET_ZERO(existing_endpoint_info, sizeof(bcm_oam_endpoint_info_t), "OAM endpoint data read for comparison",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO(lif_resolution, sizeof(oam_lif_resolution_t), "lifs assigned to gport/mpls_out_gport",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO(configured_lif, sizeof(oam_lif_configured_lif_t), "Information about lif's configuration",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO(lif_analysis_before, sizeof(oam_lif_analysis_results_t),
                       "Analysis of lif's configuration before the MEP add", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO(lif_analysis_after, sizeof(oam_lif_analysis_results_t),
                       "Analysis of lif's configuration after the MEP add", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO(mep_db_entry, sizeof(dnx_oam_oamp_ccm_endpoint_t),
                       "MEP DB for accelerated endpoints", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO(group_info, sizeof(bcm_oam_group_info_t), "Group info struct", "%s%s%s\r\n", EMPTY, EMPTY,
                       EMPTY);

    SHR_ALLOC_SET_ZERO(mep_hw_profiles_write_data, sizeof(oam_oamp_profile_hw_data_t),
                       "New profiles for Accelerated endpoints", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO(mep_hw_profiles_delete_data, sizeof(oam_oamp_profile_hw_data_t),
                       "Existing profile to delete when modifying accelerated endpoints",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO(sw_endpoint_info, sizeof(dnx_oam_endpoint_info_t),
                       "Structure for endpoint sw information", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    if (!DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
    {
        SHR_IF_ERR_EXIT(dnx_oam_group_info_get(unit, endpoint_info->group, group_info));

        if (DNX_OAM_GROUP_MAID_IS_EXTRA_DATA(group_info->flags) &&
            !DNX_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info) && !IS_OAM_CLIENT(endpoint_info))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Non accelerated endpoints cannot be assigned a 48B MAID");
        }
    }

    SHR_INVOKE_VERIFY_DNX(dnx_oam_endpoint_create_local_mep_verify(unit, endpoint_info, group_info));

    /** For server endpoints, gport is not a LIF */
    if (!IS_OAM_SERVER(endpoint_info))
    {
        SHR_IF_ERR_EXIT(dnx_oam_lif_resolution_get(unit, endpoint_info, lif_resolution));

        SHR_IF_ERR_EXIT(dnx_oam_lif_configured_lif_get(unit, lif_resolution, configured_lif));

        SHR_INVOKE_VERIFY_DNX(dnx_oam_endpoint_create_lif_verify(unit, endpoint_info, configured_lif));

        /*
         * Analyze configured lifs
         */
        SHR_IF_ERR_EXIT(dnx_oam_analyze_configured_lif(unit, configured_lif, lif_analysis_before));

        /*
         *  Counters may be assigned only on the highest and the 2 lowest level MEPs.
         *  -------------------------------------------------------------------------
         *
         *  Creating the requested MEP might cause existing MEPs to become illegal. Therefore all MEPs
         *  on the LIF should be verified.
         *
         *  Verification algorithm:
         *  - Add the requested MEP to configured lif (already checked that no MEP is allocated
         *  in that level).
         *  - Analyze the meps on the lif
         *  - Test the validity of the new MEP map (all MEPs on the LIF
         *  including the requested MEP).
         */

        SHR_IF_ERR_EXIT(dnx_oam_add_mep_to_configured_lif(unit, endpoint_info, configured_lif));

        /*
         * Analyze configured lifs including the requested MEP
         */
        SHR_IF_ERR_EXIT(dnx_oam_analyze_configured_lif(unit, configured_lif, lif_analysis_after));

        /*
         *  HLM by MDL validation and management
         */
        SHR_INVOKE_VERIFY_DNX(dnx_oam_endpoint_hlm_by_mdl_verify(unit, endpoint_info, lif_analysis_after));
    }

    if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_OAM_ENDPOINT_REPLACE))
    {
        SHR_IF_ERR_EXIT(bcm_dnx_oam_endpoint_get(unit, endpoint_info->id, existing_endpoint_info));

        /** Check that only values that are allowed to be modified are different */
        SHR_INVOKE_VERIFY_DNX(dnx_oam_endpoint_mod_verify(unit, existing_endpoint_info, endpoint_info));

        SHR_IF_ERR_EXIT(dnx_oam_sw_state_endpoint_info_get(unit, endpoint_info->id, sw_endpoint_info));

        /** For OAM server, get trap TCAM data for modification */
        dnx_oam_trap_tcam_data_get(existing_endpoint_info, sw_endpoint_info, mep_hw_profiles_delete_data);
    }
    else
    {
        /*
         *  Allocate ID for the new MEP, handled separately per accelerated/non accelerated.
         *  Acc: According to OAM-ID (index in OAMP-MEP-DB DBAL table)
         *  Non acc: According to LIF + direction +level
         */
        if (DNX_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info))
        {
            mep_db_lim = dnx_data_oam.oamp.mep_db_version_limitation_get(unit);
            if ((mep_db_lim == 1) &&
                !_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_OAM_ENDPOINT_WITH_ID) &&
                (endpoint_info->endpoint_memory_type == bcmOamEndpointMemoryTypeShortEntry) &&
                ((BCM_DNX_OAM_GROUP_NAME_TO_NAME_TYPE(group_info->name) != DNX_OAMP_OAM_MA_NAME_TYPE_ICC) &&
                 !DNX_OAM_GROUP_MAID_IS_EXTRA_DATA(group_info->flags)))
            {
                /**
                 * MEP type is read from sub-index 0 in the same full entry.
                 * This means that all short entries contained in the full entry
                 * must have the same MEP type.  Not relevant for short entries
                 * with a 13 byte MAID ("ICC") since they must be with sub-index
                 * 0 anyway.  If endpoint ID is specified, verification is in
                 * "dnx_oam_local_accelerated_endpoint_fill_id()"
                 */
                SHR_IF_ERR_EXIT(dnx_oam_endpoint_workaround_fill_id(unit, endpoint_info, group_info->name));
            }
            else
            {
                if (DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
                {
                    /** Are we adding to an existing BFD endpoint? */
                    SHR_IF_ERR_EXIT(dnx_oam_mpls_lm_dm_verify(unit, endpoint_info, &is_bfd_alloc));
                }

                /** For MPLS-LM-DM, if the BFD EP exists, this is already allocated */
                if (!is_bfd_alloc)
                {
                    is_48B_maid = DNX_OAM_GROUP_MAID_IS_EXTRA_DATA(group_info->flags);
                    SHR_IF_ERR_EXIT(dnx_oam_local_accelerated_endpoint_fill_id
                                    (unit, endpoint_info, group_info->name, is_48B_maid));
                }
            }
        }
        else
        {
            if (IS_OAM_CLIENT(endpoint_info))
            {
                /** For client endpoints, we just add one bit */
                if (!_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_OAM_ENDPOINT_WITH_ID))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Client endpoints must be created WITH_ID");
                }
                UTILEX_SET_BIT(endpoint_info->id, 1, DNX_OAM_CLIENT_MEP_INDEX_BIT);

                /** For client endpoints, a valid accelerated profile is required */
                if (DNX_OAM_PROFILE_VALID_GET(endpoint_info->acc_profile_id) == 0)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Client endpoints have a valid acc_profile_id");
                }
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_oam_local_endpoint_fill_id(unit, endpoint_info, lif_resolution));
            }
        }
    }

    if (DNX_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info))
    {

        if (DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info)
            && !_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_OAM_ENDPOINT_REPLACE))
        {
            rfc_6374_pointer_id = endpoint_info->extra_data_index;
            /** Allocate this endpoint ID so that others cannot use it */
            SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_alloc
                            (unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, &memory_type, &rfc_6374_pointer_id));
        }
        /*
         *  Check that LM/DM flexible DMAC is enabled legally
         */
        if (!(endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_SIGNAL))
        {
            SHR_IF_ERR_EXIT(dnx_oam_lmdm_flexible_da_verify(unit, endpoint_info, sw_endpoint_info, group_info));
        }

        SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_endpoint_mep_db_create(unit,
                                                                endpoint_info, group_info, sw_endpoint_info,
                                                                mep_db_entry, mep_hw_profiles_write_data,
                                                                mep_hw_profiles_delete_data));
    }

    /*
     *  Store endpoint info into sw state
     */
    SHR_IF_ERR_EXIT(dnx_oam_endpoint_info_store_sw(unit, endpoint_info,
                                                   lif_resolution, mep_hw_profiles_write_data->tcam_index));

    if (!_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_OAM_ENDPOINT_REPLACE) && (!DNX_OAM_IS_DEFAULT_MEP(endpoint_info))
        && (!DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info)))
    {
        /*
         * Add endpoint to group linked list
         */
        SHR_IF_ERR_EXIT(dnx_oam_endpoint_add_to_group_list(unit, endpoint_info));
    }

    /*
     * ------------------ Write to HW from here-------------------
     */

    /*
     * Mark in-lif and out_lif as OAM
     */
    SHR_IF_ERR_EXIT(dnx_oam_mark_lif_as_oam(unit, lif_analysis_before, lif_analysis_after, lif_resolution,
                                            (endpoint_info->lm_counter_base_id > 0)));

    /*
     * Update MEP-DB and related
     */
    if (DNX_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info) || IS_OAM_CLIENT(endpoint_info))
    {
        if (!IS_OAM_CLIENT(endpoint_info))
        {
            if (DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
            {
                mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_MPLS_LM_DM_SET;
            }
            /** For client endpoints no OAMP_MEP_DB entry is written */
            SHR_IF_ERR_EXIT(dnx_oam_oamp_profile_hw_set(unit, mep_hw_profiles_write_data));
            if (!DNX_OAM_GROUP_MAID_IS_EXTRA_DATA(group_info->flags))
            {
                SHR_IF_ERR_EXIT(dnx_oam_handle_acceleraed_endpoint_group_config
                                (unit, group_info->name, endpoint_info->id, 1));
            }
            if (!DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
            {
                SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_set(unit, (uint16) endpoint_info->id, mep_db_entry));
            }
            else
            {
                /** Setting timestamp format */
                SHR_IF_ERR_EXIT(dnx_oamp_dbal_rfc_6374_timestamp_format_set(unit, mep_db_entry->timestamp_format));
                /** Set BFD PWE and RFC6374 entries */
                SHR_IF_ERR_EXIT(dnx_oamp_mep_db_rfc_6374_endpoint_set
                                (unit, (uint16) endpoint_info->id, mep_db_entry,
                                 TRUE /** Its last entry */ , is_bfd_alloc));

                if (is_bfd_alloc)
                {
                    SHR_IF_ERR_EXIT(dnx_bfd_modify_mp_profile_id(unit, endpoint_info->id,
                                                                 configured_lif->mp_profile
                                                                 [DNX_OAM_CLASSIFIER_INGRESS]));
                }
            }
        }

        oam_lif = DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info) ? lif_resolution->local_out_lif :
            lif_resolution->global_in_lif;
        if ((oam_lif != BCM_GPORT_INVALID) &&
            (endpoint_info->acc_profile_id != BCM_OAM_PROFILE_INVALID) &&
            (DNX_OAM_PROFILE_VALID_GET(endpoint_info->acc_profile_id) == 1))
        {
            /**
             * Add endpoint to ACC_MEP_DB for server ("server client")
             * if acc_profile_id is valid
             */
            classifier_ep_id = endpoint_info->id;

            /** Make sure client MEP bit is removed */
            UTILEX_SET_BIT(classifier_ep_id, 0, DNX_OAM_CLIENT_MEP_INDEX_BIT);

            mdl = endpoint_info->level;
            /*
             * When mpls_tp_mdl_ignore is enabled, MDL is forced to 7 in OAM_ACC_MEP_DB
             * if MEP type is MPLS LSP, PWE and MPLS section)
             */
            SHR_IF_ERR_EXIT(algo_oam_db.mpls_tp_mdl_ignore.get(unit, &mpls_tp_mdl_ignore));
            if (mpls_tp_mdl_ignore)
            {
                if (endpoint_info->type == bcmOAMEndpointTypeBhhSection ||
                    endpoint_info->type == bcmOAMEndpointTypeBHHMPLS || endpoint_info->type == bcmOAMEndpointTypeBHHPwe)
                {
                    mdl = 7;
                }
            }
            /** For MPLS-LM-DM, if the BFD EP exists, the classifier is already configured */
            SHR_IF_ERR_EXIT(dnx_oam_acc_mep_add
                            (unit, _SHR_CORE_ALL,
                             ((DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info) ? 0 : DNX_OAM_CLASSIFIER_INGRESS) |
                              ((_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_OAM_ENDPOINT_REPLACE) || is_bfd_alloc) ?
                               DNX_OAM_CLASSIFIER_UPDATE : 0)),
                             oam_lif, mdl, classifier_ep_id, DNX_OAM_PROFILE_IDX_GET(endpoint_info->acc_profile_id)));
        }
    }

    if (IS_OAM_SERVER(endpoint_info))
    {
        /** Delete relevant trap TCAM entry if no more references */
        SHR_IF_ERR_EXIT(dnx_oam_trap_tcam_entry_clear(unit, mep_hw_profiles_delete_data));
    }
    else
    {
        /*
         *  Add the endpoint to LIF-DB-HW
         */
        SHR_IF_ERR_EXIT(dnx_oam_endpoint_update_lif_db_hw
                        (unit, lif_resolution, configured_lif, &lif_analysis_before->lif_db,
                         &lif_analysis_after->lif_db));
    }

exit:
    SHR_FREE(existing_endpoint_info);
    SHR_FREE(sw_endpoint_info);
    SHR_FREE(lif_analysis_before);
    SHR_FREE(lif_analysis_after);
    SHR_FREE(configured_lif);
    SHR_FREE(group_info);
    SHR_FREE(lif_resolution);
    SHR_FREE(mep_db_entry);
    SHR_FREE(mep_hw_profiles_write_data);
    SHR_FREE(mep_hw_profiles_delete_data);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Main function for default MEP create.
 *          This function calls to sub-functions that
 *          verifies create request, gets current lif
 *          status, and adds the new MEP to sw and hw
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - info structure of the requested
 *        MEP
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_default_mep_create(
    int unit,
    bcm_oam_endpoint_info_t * endpoint_info)
{
    bcm_oam_endpoint_info_t *existing_endpoint_info = NULL;
    oam_default_mep_info_t *default_mep_info = NULL;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC_SET_ZERO(existing_endpoint_info, sizeof(bcm_oam_endpoint_info_t), "OAM endpoint data read for comparison",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO(default_mep_info, sizeof(oam_default_mep_info_t), "Default MEP configuration",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_oam_default_mep_profile_get(unit, endpoint_info, default_mep_info));

    SHR_INVOKE_VERIFY_DNX(dnx_oam_endpoint_create_default_mep_verify(unit, endpoint_info, default_mep_info));

    SHR_IF_ERR_EXIT(dnx_oam_add_mep_to_default_oam_profile(unit, endpoint_info, default_mep_info));

    if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_OAM_ENDPOINT_REPLACE))
    {
        SHR_IF_ERR_EXIT(bcm_dnx_oam_endpoint_get(unit, endpoint_info->id, existing_endpoint_info));

        /** Check that only values that are allowed to be modified are different */
        SHR_INVOKE_VERIFY_DNX(dnx_oam_endpoint_mod_verify(unit, existing_endpoint_info, endpoint_info));
    }

    /*
     *  Store endpoint info into sw state
     */
    SHR_IF_ERR_EXIT(dnx_oam_default_endpoint_info_store_sw(unit, endpoint_info));

    /*
     * ------------------ Write to HW from here-------------------
     */

    /*
     *  Add the default endpoint to HW
     */
    SHR_IF_ERR_EXIT(dnx_oam_endpoint_update_default_mep_db_hw(unit, default_mep_info));

exit:
    SHR_FREE(existing_endpoint_info);
    SHR_FREE(default_mep_info);
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Create an OAM Endpoint.
*  Define a Maintenance end point to provide mechanisms
*  for OAM functionality.
* \param [in] unit  -
*   Relevant unit.
* \param [in,out] endpoint_info -
*   Pointer to the structure, containing information about the
*   OAM endpoint.
*                   - flags [in]: Endpoint attributes
*                   - id [in/out]: Endpoint id
*                   - type [in]: Endpoint type (ETH / MPLS /
*                     PWE)
*                   - group [in]: The ID of the group to which
*                     this endpoint belongs
*                   - level [in]:This is equivalent to the
*                     Maintenance Domain Level (MDL) in 802.1ag.
*                   - gport [in]: The gport associated with this
*                     endpoint
*                   - lm_counter_base_id [in]: Counter id
*                     assosiated to the mep
*                   - mpls_out_gport [in]: Out gport used for TX
*                     counting by BHH endpoints.
*
* \retval
*   shr_error - Error indication
* \remark
*   * None
* \see
*   * bcm_dnx_oam_endpoint_destroy
*   * bcm_dnx_oam_endpoint_destroy_all
*   * bcm_dnx_oam_endpoint_get
*/
shr_error_e
bcm_dnx_oam_endpoint_create(
    int unit,
    bcm_oam_endpoint_info_t * endpoint_info)
{

    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_START(unit);
    DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR_PTR(unit, sizeof(bcm_oam_endpoint_info_t), endpoint_info);

    /*
     * General validation of inputs
     */
    SHR_INVOKE_VERIFY_DNX(dnx_oam_endpoint_create_general_validity_check(unit, endpoint_info));

    /*
     * REMOTE MEP
     */
    if (endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE)
    {
        SHR_IF_ERR_EXIT(dnx_oam_remote_endpoint_create(unit, endpoint_info));
    }
    else if (DNX_OAM_IS_DEFAULT_MEP(endpoint_info))
    {
        /*
         * DEFAULT MEP
         */
        SHR_IF_ERR_EXIT(dnx_oam_default_mep_create(unit, endpoint_info));
    }
    else
    {
        /*
         *  LOCAL NON-DEFAULT MEP
         */
        SHR_IF_ERR_EXIT(dnx_oam_local_mep_create(unit, endpoint_info));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get an Local OAM Endpoint information.
* \param [in] unit  -
*  Relevant unit.
* \param [in] sw_endpoint_info - Pointer to temporary structure
*        to which sw state data will be written for subsequent
*        use.
* \param [out] endpoint_info -
*  Pointer to the structure, in which to write the information
*  from the Get procedure.
* \retval
*   shr_error - Error indication
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_endpoint_create
*   * \ref bcm_dnx_oam_endpoint_destroy
*   * \ref bcm_dnx_oam_endpoint_destroy_all
*/
static shr_error_e
dnx_oam_local_non_acc_endpoint_get(
    int unit,
    const dnx_oam_endpoint_info_t * sw_endpoint_info,
    bcm_oam_endpoint_info_t * endpoint_info)
{

    SHR_FUNC_INIT_VARS(unit);

    endpoint_info->gport = sw_endpoint_info->gport;
    endpoint_info->group = sw_endpoint_info->group;
    endpoint_info->level = sw_endpoint_info->level;
    endpoint_info->lm_counter_base_id = sw_endpoint_info->lm_counter_idx;
    endpoint_info->lm_counter_if = sw_endpoint_info->lm_counter_if;
    endpoint_info->mpls_out_gport = sw_endpoint_info->mpls_out_gport;
    endpoint_info->type = sw_endpoint_info->type;
    endpoint_info->remote_gport = sw_endpoint_info->server_trap;

    endpoint_info->flags = 0;
    if (sw_endpoint_info->sw_state_flags & DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_UP_FACING)
    {
        endpoint_info->flags |= BCM_OAM_ENDPOINT_UP_FACING;
    }
    if (sw_endpoint_info->sw_state_flags & DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_INTERMEDIATE)
    {
        endpoint_info->flags |= BCM_OAM_ENDPOINT_INTERMEDIATE;
    }
    if (sw_endpoint_info->sw_state_flags & DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_REMOTE)
    {
        endpoint_info->flags |= BCM_OAM_ENDPOINT_REMOTE;
    }
    if (sw_endpoint_info->sw_state_flags & DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_PCP_BASED)
    {
        endpoint_info->lm_flags |= BCM_OAM_LM_PCP;
    }
    if ((sw_endpoint_info->sw_state_flags & DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_ACCELERATED)
        && !DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
    {
        endpoint_info->opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;
    }
    endpoint_info->flags2 = 0;
    if (UTILEX_GET_BIT(endpoint_info->id, DNX_OAM_CLIENT_MEP_INDEX_BIT) == 1)
    {
        endpoint_info->flags2 |= BCM_OAM_ENDPOINT2_HW_ACCELERATION_SET;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get an Local OAM Endpoint information.
* \param [in] unit  -
*  Relevant unit.
* \param [in] sw_endpoint_info - Pointer to temporary structure
*        to which sw state data will be written for subsequent
*        use.
* \param [out] endpoint_info -
*  Pointer to the structure, in which to write the information
*  from the Get procedure.
* \retval
*   shr_error - Error indication
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_endpoint_create
*   * \ref bcm_dnx_oam_endpoint_destroy
*   * \ref bcm_dnx_oam_endpoint_destroy_all
*/
static shr_error_e
dnx_oam_local_endpoint_get(
    int unit,
    const dnx_oam_endpoint_info_t * sw_endpoint_info,
    bcm_oam_endpoint_info_t * endpoint_info)
{

    oam_lif_resolution_t *lif_resolution = NULL;
    uint32 mep_id;
    uint32 acc_profile_id;
    uint32 mpls_tp_mdl_ignore = 0;
    int mdl = 0;
    int oam_lif;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(lif_resolution, sizeof(oam_lif_resolution_t), "lifs assigned to gport/mpls_out_gport",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /*
     * Local endpoint get
     */
    SHR_IF_ERR_EXIT(dnx_oam_local_non_acc_endpoint_get(unit, sw_endpoint_info, endpoint_info));

    /*
     * Accelerated endpoint get
     */
    if (sw_endpoint_info->sw_state_flags & DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_ACCELERATED)
    {
        if (!DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
        {
            /** Set Accelerated flag */
            endpoint_info->opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;
        }

        if (sw_endpoint_info->sw_state_flags & DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_DOWN_EGRESS_INJECTION)
        {
            /** Set Down MEP Egress injection flag */
            endpoint_info->flags2 |= BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN;
        }

        if (sw_endpoint_info->sw_state_flags & DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_SIGNAL_DETECT)
        {
            /** Set Signal Degradation flag and Signals(transmitted and expected) */
            endpoint_info->flags2 |= BCM_OAM_ENDPOINT_FLAGS2_SIGNAL;
            endpoint_info->rx_signal = sw_endpoint_info->rx_signal;
            endpoint_info->tx_signal = sw_endpoint_info->tx_signal;
        }

        /** Retrieve endpoint info from mep_db/mdb */
        SHR_IF_ERR_EXIT(dnx_oam_local_acc_endpoint_get(unit, endpoint_info));

        /** Set TX statistics flag */
        if (_SHR_IS_FLAG_SET(sw_endpoint_info->sw_state_flags, DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_TX_STATISTICS))
        {
            endpoint_info->flags2 |= BCM_OAM_ENDPOINT_FLAGS2_TX_STATISTICS;
        }

        /** Set RX statistics flag */
        if (_SHR_IS_FLAG_SET(sw_endpoint_info->sw_state_flags, DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_RX_STATISTICS))
        {
            endpoint_info->flags2 |= BCM_OAM_ENDPOINT_FLAGS2_RX_STATISTICS;
        }

        /** Set per opcode statistics flag */
        if (_SHR_IS_FLAG_SET
            (sw_endpoint_info->sw_state_flags, DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_PER_OPCODE_STATISTICS))
        {
            endpoint_info->flags2 |= BCM_OAM_ENDPOINT_FLAGS2_PER_OPCODE_STATISTICS;
        }
    }

    if ((sw_endpoint_info->sw_state_flags & DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_ACCELERATED) ||
        IS_OAM_CLIENT(endpoint_info))
    {
        /** Get Lif resolution */
        if (IS_OAM_SERVER(endpoint_info))
        {
            /** For OAM server, no LIF is provided */
            oam_lif = BCM_GPORT_INVALID;
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_oam_lif_resolution_get(unit, endpoint_info, lif_resolution));

            /** Get accelerated profile */
            oam_lif = DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info) ?
                lif_resolution->local_out_lif : lif_resolution->global_in_lif;
        }
        if (oam_lif != BCM_GPORT_INVALID)
        {
            mdl = endpoint_info->level;
            /*
             * When mpls_tp_mdl_ignore is enabled, MDL is forced to 7 in OAM_ACC_MEP_DB
             * if MEP type is MPLS LSP, PWE and MPLS section)
             */
            SHR_IF_ERR_EXIT(algo_oam_db.mpls_tp_mdl_ignore.get(unit, &mpls_tp_mdl_ignore));
            if (mpls_tp_mdl_ignore)
            {
                if (endpoint_info->type == bcmOAMEndpointTypeBhhSection ||
                    endpoint_info->type == bcmOAMEndpointTypeBHHMPLS || endpoint_info->type == bcmOAMEndpointTypeBHHPwe)
                {
                    mdl = 7;
                }
            }
            SHR_IF_ERR_EXIT(dnx_oam_acc_mep_get(unit, _SHR_CORE_ALL,
                                                DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info) ?
                                                0 : DNX_OAM_CLASSIFIER_INGRESS,
                                                oam_lif, mdl, &mep_id, &acc_profile_id));
            /** Set profile encoding */
            endpoint_info->acc_profile_id = DNX_OAM_PROFILE_SET(acc_profile_id,
                                                                DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info) ?
                                                                bcmOAMProfileEgressAcceleratedEndpoint :
                                                                bcmOAMProfileIngressAcceleratedEndpoint);
        }
        else
        {
            endpoint_info->acc_profile_id = BCM_OAM_PROFILE_INVALID;
        }

        if (_SHR_IS_FLAG_SET
            (sw_endpoint_info->sw_state_flags, DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_TX_GPORT_IS_TRUNK))
        {
            /** Only lower 8 bits of tx_gport should be used to rebuild trunk ID */
            BCM_TRUNK_ID_SET(endpoint_info->tx_gport, 0,
                             UTILEX_GET_BITS_RANGE(endpoint_info->tx_gport, UTILEX_NOF_BITS_IN_BYTE - 1, 0));
            BCM_GPORT_TRUNK_SET(endpoint_info->tx_gport, endpoint_info->tx_gport);
        }

        if (!DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
        {
            endpoint_info->extra_data_index = sw_endpoint_info->extra_data_index;
        }

    }

exit:
    SHR_FREE(lif_resolution);
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get an OAM Endpoint information.
* \param [in] unit  -
*  Relevant unit.
* \param [in] endpoint_id -
*  ID of the OAM endpoint from which to get the information.
* \param [out] endpoint_info -
*  Pointer to the structure, in which to write the information
*  from the Get procedure.
* \retval
*   shr_error - Error indication
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_endpoint_create
*   * \ref bcm_dnx_oam_endpoint_destroy
*   * \ref bcm_dnx_oam_endpoint_destroy_all
*/
shr_error_e
bcm_dnx_oam_endpoint_get(
    int unit,
    int endpoint_id,
    bcm_oam_endpoint_info_t * endpoint_info)
{
    dnx_oam_endpoint_info_t sw_endpoint_info;
    int rmep_idx_internal = -1;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_NULL_CHECK(endpoint_info, _SHR_E_PARAM, "endpoint_info");

    bcm_oam_endpoint_info_t_init(endpoint_info);

    endpoint_info->id = endpoint_id;

    if (DNX_OAM_IS_DEFAULT_MEP(endpoint_info))
    {
        SHR_IF_ERR_EXIT(dnx_oam_sw_state_endpoint_info_get(unit, endpoint_id, &sw_endpoint_info));
        SHR_IF_ERR_EXIT(dnx_oam_local_endpoint_get(unit, &sw_endpoint_info, endpoint_info));
    }
    else if (DNX_OAM_MEP_INDEX_IS_REMOTE(endpoint_id))
    {
        DNX_OAM_REMOTE_MEP_INDEX_TO_INTERNAL(rmep_idx_internal, endpoint_id);
        /*
         * Remote endpoint GET
         */
        SHR_IF_ERR_EXIT(dnx_oam_remote_endpoint_get(unit, rmep_idx_internal, endpoint_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_oam_sw_state_endpoint_info_get(unit, endpoint_id, &sw_endpoint_info));
        SHR_IF_ERR_EXIT(dnx_oam_local_endpoint_get(unit, &sw_endpoint_info, endpoint_info));

    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Determine whether the BFD EP associated with an MPLS-LM-DM
*  OAM EP does not exist (does not transmit or receive packets)
* \param [in] unit  -
*  Relevant unit.
* \param [in] endpoint -
*  ID of the OAM endpoint from which to get the information.
* \param [out] is_mpls_lm_dm_without_bfd_endpoint -
*  Pointer to the resulting TRUE/FALSE
* \retval
*   shr_error - Error indication
* \remark
*   * None
*/
shr_error_e
dnx_oam_mpls_lm_dm_without_bfd_endpoint(
    int unit,
    bcm_oam_endpoint_t endpoint,
    uint8 *is_mpls_lm_dm_without_bfd_endpoint)
{
    uint32 entry_handle_id, flags;
    int rv;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    *is_mpls_lm_dm_without_bfd_endpoint = FALSE;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BFD_ENDPOINT_SW_INFO, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_BFD_ENDPOINT_SW_STATE_FLAGS, INST_SINGLE, &flags);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_ENDPOINT_ID, endpoint);
    rv = dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT);

    if (rv == _SHR_E_NOT_FOUND)
    {
        *is_mpls_lm_dm_without_bfd_endpoint = TRUE;
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Remove an OAM Endpoint.
*
*  Main function for local MEP destroy.
*  This function calls to sub-functions that
*  removes a MEP from sw and hw
*
* \param [in] unit  -
*   Relevant unit.
* \param [out] endpoint_info -
*  Pointer to the structure, in which to write the information
*  from the Get procedure.
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_endpoint_create
*   * \ref bcm_dnx_oam_endpoint_destroy_all
*   * \ref bcm_dnx_oam_endpoint_get
*/
static shr_error_e
dnx_oam_endpoint_destroy(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info)
{

    oam_lif_resolution_t *lif_resolution = NULL;
    oam_lif_configured_lif_t *configured_lif = NULL;
    oam_lif_analysis_results_t *lif_analysis_before = NULL;
    oam_lif_analysis_results_t *lif_analysis_after = NULL;
    oam_oamp_profile_hw_data_t *mep_hw_profiles_write_data = NULL;
    dnx_oam_oamp_ccm_endpoint_t *mep_db_entry = NULL;
    bcm_oam_group_info_t *group_info = NULL;
    int oam_lif;
    uint32 extra_data_length;
    uint8 mep_db_extra_data_length = 0;
    uint8 lm_exists = 0, lm_stat_exists = 0;
    dnx_oam_oamp_endpoint_lm_params_t lm_mep_db_entry;
    bcm_oam_loss_t loss_ptr;
    bcm_oam_delay_t delay_ptr;
    uint8 dm_exists = 0;
    bcm_oam_loss_t session_loss_ptr;
    bcm_oam_delay_t session_delay_ptr;
    dnx_oam_oamp_endpoint_dm_params_t delay_params;
    dnx_oam_oamp_endpoint_lm_params_t *loss_params = NULL;
    dnx_oam_oamp_endpoint_dm_params_t *dm_mep_db_entry = NULL;
    dnx_oam_endpoint_info_t sw_endpoint_info;
    uint32 session_oam_id;
    uint32 mpls_tp_mdl_ignore = 0;
    int mdl = 0;
    int priority;
    int rv;
    uint8 is_mpls_lm_dm_without_bfd_endpoint = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(lif_resolution, sizeof(oam_lif_resolution_t), "lifs assigned to gport/mpls_out_gport",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO(configured_lif, sizeof(oam_lif_configured_lif_t), "Information about lif's configuration",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO(lif_analysis_before, sizeof(oam_lif_analysis_results_t),
                       "Analysis of lif's configuration before the MEP add", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO(lif_analysis_after, sizeof(oam_lif_analysis_results_t),
                       "Analysis of lif's configuration after the MEP add", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO(mep_db_entry, sizeof(dnx_oam_oamp_ccm_endpoint_t),
                       "MEP DB for accelerated endpoints", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO(mep_hw_profiles_write_data, sizeof(oam_oamp_profile_hw_data_t),
                       "Reset Profiles for Accelerated endpoints", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO(group_info, sizeof(bcm_oam_group_info_t),
                       "Info about group associated with endpoint", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO(loss_params, sizeof(dnx_oam_oamp_endpoint_lm_params_t),
                       "Structure for LM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO(dm_mep_db_entry, sizeof(dnx_oam_oamp_endpoint_dm_params_t),
                       "Structure for DM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    if (!IS_OAM_SERVER(endpoint_info))
    {
        /*
         * Get lif resolution
         */
        SHR_IF_ERR_EXIT(dnx_oam_lif_resolution_get(unit, endpoint_info, lif_resolution));

        /*
         * Get configured lif
         */
        SHR_IF_ERR_EXIT(dnx_oam_lif_configured_lif_get(unit, lif_resolution, configured_lif));

        /*
         * Analyze configured lifs
         */
        SHR_IF_ERR_EXIT(dnx_oam_analyze_configured_lif(unit, configured_lif, lif_analysis_before));

        /*
         * Remove the MEP from configured_lif
         */
        SHR_IF_ERR_EXIT(dnx_oam_remove_mep_from_configured_lif(unit, endpoint_info, configured_lif));

        /*
         * Analyze configured lifs including the requested MEP
         */
        SHR_IF_ERR_EXIT(dnx_oam_analyze_configured_lif(unit, configured_lif, lif_analysis_after));
    }

    /** Check if endpoint is accelerated and delete loss entries  */
    if ((DNX_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info))
        && (endpoint_info->endpoint_memory_type != bcmOamEndpointMemoryTypeShortEntry))
    {
        /*
         * Delete associated loss entry
         */
        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_lm_endpoint_get
                        (unit, (uint16) endpoint_info->id, &lm_mep_db_entry, &lm_exists, &lm_stat_exists));

        if (lm_exists)
        {
            loss_ptr.id = endpoint_info->id;
            loss_ptr.pkt_pri_bitmap = 0xff;
            SHR_IF_ERR_EXIT(bcm_dnx_oam_loss_delete(unit, &loss_ptr));
        }

        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_dm_endpoint_get(unit, (uint16) endpoint_info->id, &delay_params, &dm_exists));
        if (dm_exists)
        {
            delay_ptr.id = endpoint_info->id;
            delay_ptr.pkt_pri_bitmap = 0xff;
            SHR_IF_ERR_EXIT(bcm_dnx_oam_delay_delete(unit, &delay_ptr));
        }

        /*
         * Delete all associated priority loss entries
         */
        for (priority = 0; priority < 8; priority++)
        {
            loss_ptr.id = endpoint_info->id;
            rv = dnx_oamp_lm_with_priority_session_map_get(unit, loss_ptr.id, priority, &session_oam_id);
            if (rv != _SHR_E_NOT_FOUND)
            {
                SHR_IF_ERR_EXIT(rv);

                SHR_IF_ERR_EXIT(dnx_oamp_mep_db_lm_endpoint_get
                                (unit, session_oam_id, loss_params, &lm_exists, &lm_stat_exists));
                if (lm_exists)
                {
                    session_loss_ptr.id = loss_ptr.id;
                    session_loss_ptr.pkt_pri_bitmap = 1 << priority;
                    rv = bcm_dnx_oam_loss_delete(unit, &session_loss_ptr);
                    if (rv != _SHR_E_NOT_FOUND)
                    {
                        SHR_IF_ERR_EXIT(rv);
                    }
                }
            }
            rv = dnx_oamp_lm_with_priority_session_map_get(unit, loss_ptr.id, priority, &session_oam_id);
            if (rv != _SHR_E_NOT_FOUND)
            {
                SHR_IF_ERR_EXIT(dnx_oamp_mep_db_dm_endpoint_get(unit, session_oam_id, dm_mep_db_entry, &dm_exists));
                if (dm_exists)
                {
                    session_delay_ptr.id = endpoint_info->id;
                    session_delay_ptr.pkt_pri_bitmap = 1 << priority;
                    SHR_IF_ERR_EXIT(bcm_dnx_oam_delay_delete(unit, &session_delay_ptr));
                    if (rv != _SHR_E_NOT_FOUND)
                    {
                        SHR_IF_ERR_EXIT(rv);
                    }
                }
            }
        }
    }

    /*
     *  Get length of mep_db extra data attached to ccm endpoint
     */
    if (DNX_OAM_DISSECT_IS_EXTRA_DATA_PER_ENDPOINT(endpoint_info) && !DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
    {
        sal_memset(&sw_endpoint_info, 0, sizeof(dnx_oam_endpoint_info_t));
        SHR_IF_ERR_EXIT(dnx_oam_sw_state_endpoint_info_get(unit, endpoint_info->id, &sw_endpoint_info));
        mep_db_extra_data_length = sw_endpoint_info.extra_data_length;
    }

    /*
     *  Remove endpoint from sw state
     */
    if (!IS_OAM_SERVER(endpoint_info))
    {
        SHR_IF_ERR_EXIT(dnx_oam_endpoint_info_remove_sw(unit, endpoint_info, lif_resolution));
    }
    else
    {
        /** No LIFs for OAM server, so just remove from SW state table */
        SHR_IF_ERR_EXIT(dnx_oam_endpoint_info_remove_from_sw_state(unit, endpoint_info->id));
    }

    if (!DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
    {
        /*
         * Remove endpoint from group linked list
         */
        SHR_IF_ERR_EXIT(dnx_oam_endpoint_remove_from_group_list(unit, endpoint_info));
    }

    if (DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
    {
        /** Check if the OAM-LM-DM is without BFD endpoint */
        SHR_IF_ERR_EXIT(dnx_oam_mpls_lm_dm_without_bfd_endpoint
                        (unit, endpoint_info->id, &is_mpls_lm_dm_without_bfd_endpoint));
    }

    /** Check if endpoint is accelerated and free profiles(sw only)*/
    if (DNX_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info))
    {
        if (DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
        {
            mep_db_entry->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374;
        }
        else
        {
            /** Set default value, since we have check in next step */
            mep_db_entry->mep_type = 0;
        }
        /** Get mep_db entry */
        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_get(unit, (uint16) endpoint_info->id,
                                                         DNX_OAM_DISSECT_IS_DOWN_MEP_EGRESS_INJECTION(endpoint_info),
                                                         mep_db_entry));

        /** For MPLS-LM-DM, if the BFD endpoint exists, don't touch it */
        if (!DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info) || is_mpls_lm_dm_without_bfd_endpoint)
        {
            /** Free allocated profiles(sw) and sign profiles that should be updated(cleared) in hw */
            SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_endpoint_mep_db_profiles_free
                            (unit, mep_db_entry, mep_hw_profiles_write_data));
        }
    }

    /** Start HW remove */

    if (!IS_OAM_SERVER(endpoint_info))
    {
        /*
         * Mark in-lif and out_lif as OAM
         */
        SHR_IF_ERR_EXIT(dnx_oam_mark_lif_as_oam(unit, lif_analysis_before, lif_analysis_after, lif_resolution, FALSE));

        /*
         *  Remove the endpoint from LIF-DB-HW.
         *  No classifier entries for server endpoint.
         */
        SHR_IF_ERR_EXIT(dnx_oam_endpoint_update_lif_db_hw
                        (unit, lif_resolution, configured_lif, &lif_analysis_before->lif_db,
                         &lif_analysis_after->lif_db));
    }

    if (DNX_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info))
    {
        if (!DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
        {
            /** Are there extra data mdb entries to remove? */
            SHR_IF_ERR_EXIT(bcm_oam_group_get(unit, endpoint_info->group, group_info));
        }

        extra_data_length = 0;

        if (endpoint_info->endpoint_memory_type != bcmOamEndpointMemoryTypeSelfContained)
        {
            /** If ep is maid48 extra data have ITMH Ext Header(4 bytes) + PPH header(12) + maid48(48) */
            if (_SHR_IS_FLAG_SET(group_info->flags, BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE))
            {
                extra_data_length = DNX_OAM_OAMP_NOF_MAID_BITS;
            }
        }

        /** Remove endpoint from mep_db and mdb */
        /** For MPLS-LM-DM, this is allowed only if there is no BFD endpoint */
        if (!DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info) || is_mpls_lm_dm_without_bfd_endpoint)
        {
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_clear(unit, (uint16) endpoint_info->id, extra_data_length));
        }

        if (DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
        {
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_clear
                            (unit, MEP_DB_ENTRY_TO_OAM_ID(mep_db_entry->flex_lm_dm_entry), extra_data_length));
            /** Release the endpoint id */
            SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_dealloc(unit, MEP_DB_ENTRY_TO_OAM_ID(mep_db_entry->flex_lm_dm_entry)));
        }

        /** Remove mep_db extra data attached to ccm endpoint
         *  If it's maid48 and Signal Detect removing not requiered as maid48 already removed by group remove
 */
        if (DNX_OAM_DISSECT_IS_EXTRA_DATA_PER_ENDPOINT(endpoint_info) &&
            !DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info) &&
            !((DNX_OAM_GROUP_MAID_IS_EXTRA_DATA(group_info->flags) &&
               _SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_SIGNAL))))
        {
            dnx_oamp_ccm_endpoint_extra_data_t extra_data;
            sal_memset(&extra_data, 0, sizeof(dnx_oamp_ccm_endpoint_extra_data_t));
            extra_data.index = OAM_ID_TO_MEP_DB_ENTRY(endpoint_info->extra_data_index);
            extra_data.length = mep_db_extra_data_length;
            SHR_IF_ERR_EXIT(dnx_oamp_ccm_endpoint_extra_data_set(unit, &extra_data));
        }

        /** Remove remote endpoints related to destroyed endpoint */
        SHR_IF_ERR_EXIT(dnx_oam_lmep_remote_endpoints_destroy(unit, endpoint_info->id));
    }

    /** Check if endpoint is accelerated and perform following hw update:
     *                                       1.Remove from mep_db
     *                                       2.Delete profiles
     *                                       3.Delete entry from accelerated mep_db
     *                                       4. Deallocate endpoint id.
 */
    if (DNX_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info) || IS_OAM_CLIENT(endpoint_info))
    {

        /** Delete unused profiles from HW */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_profile_hw_set(unit, mep_hw_profiles_write_data));

        /** Delete dbal entries from acc mep db table */
        if (!IS_OAM_SERVER(endpoint_info))
        {
            oam_lif = DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info) ?
                lif_resolution->local_out_lif : lif_resolution->global_in_lif;
            if (oam_lif != BCM_GPORT_INVALID)
            {
                mdl = endpoint_info->level;
                /*
                 * When mpls_tp_mdl_ignore is enabled, MDL is forced to 7 in OAM_ACC_MEP_DB
                 * if MEP type is MPLS LSP, PWE and MPLS section)
                 */
                SHR_IF_ERR_EXIT(algo_oam_db.mpls_tp_mdl_ignore.get(unit, &mpls_tp_mdl_ignore));
                if (mpls_tp_mdl_ignore)
                {
                    if (endpoint_info->type == bcmOAMEndpointTypeBhhSection ||
                        endpoint_info->type == bcmOAMEndpointTypeBHHMPLS ||
                        endpoint_info->type == bcmOAMEndpointTypeBHHPwe)
                    {
                        mdl = 7;
                    }
                }
                /** For MPLS-LM-DM, if the BFD endpoint exists, don't touch it */
                if (!DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info) || is_mpls_lm_dm_without_bfd_endpoint)
                {
                    SHR_IF_ERR_EXIT(dnx_oam_acc_mep_delete(unit, _SHR_CORE_ALL,
                                                           DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info) ?
                                                           0 : DNX_OAM_CLASSIFIER_INGRESS, oam_lif, mdl));
                }
            }
        }
        else
        {
            /** Delete relevant trap TCAM entry if no more references */
            SHR_IF_ERR_EXIT(dnx_oam_trap_tcam_entry_clear(unit, mep_hw_profiles_write_data));
        }
    }

    if (DNX_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info))
    {
        /** For MPLS-LM-DM, if the BFD endpoint exists, don't touch it */
        if (!DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info) || is_mpls_lm_dm_without_bfd_endpoint)
        {
            /** Release the endpoint id */
            SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_dealloc(unit, endpoint_info->id));
        }
    }

exit:
    SHR_FREE(lif_analysis_after);
    SHR_FREE(lif_analysis_before);
    SHR_FREE(configured_lif);
    SHR_FREE(lif_resolution);
    SHR_FREE(mep_hw_profiles_write_data);
    SHR_FREE(mep_db_entry);
    SHR_FREE(group_info);
    SHR_FREE(loss_params);
    SHR_FREE(dm_mep_db_entry);
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Remove a Default OAM Endpoint.
*
* \param [in] unit  -
*   Relevant unit.
* \param [out] endpoint_info -
*  Pointer to the structure, in which to write the information
*  from the Get procedure.
* \remark
*   * None
* \see
*   * \ref dnx_oam_default_mep_create
*/
static shr_error_e
dnx_oam_default_endpoint_destroy(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info)
{

    oam_default_mep_info_t *default_mep_info = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(default_mep_info, sizeof(oam_default_mep_info_t), "Default MEP configuration",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_oam_default_mep_profile_get(unit, endpoint_info, default_mep_info));

    SHR_INVOKE_VERIFY_DNX(dnx_oam_endpoint_default_mep_destroy_verify(unit, endpoint_info, default_mep_info));

    /*
     * Delete endpoint info, then add endpoint_id to lif_info
     */
    SHR_IF_ERR_EXIT(dnx_oam_endpoint_info_remove_from_sw_state(unit, endpoint_info->id));

    SHR_IF_ERR_EXIT(dnx_oam_remove_mep_from_default_oam_profile(unit, endpoint_info, default_mep_info));

    /** Start HW remove */

    SHR_IF_ERR_EXIT(dnx_oam_endpoint_update_default_mep_db_hw(unit, default_mep_info));

exit:
    SHR_FREE(default_mep_info);
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Destroy an OAM Endpoint.
*
* \param [in] unit  - Relevant unit.
* \param [in] endpoint - Id of the endpoint to destroy.
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_endpoint_create
*   * \ref bcm_dnx_oam_endpoint_destroy_all
*   * \ref bcm_dnx_oam_endpoint_get
*/
shr_error_e
bcm_dnx_oam_endpoint_destroy(
    int unit,
    bcm_oam_endpoint_t endpoint)
{
    bcm_oam_endpoint_info_t endpoint_info;
    bcm_oam_endpoint_info_t *p_endpoint_info;
    int rmep_idx_internal = -1;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    p_endpoint_info = &endpoint_info;
    bcm_oam_endpoint_info_t_init(&endpoint_info);

    SHR_IF_ERR_EXIT(bcm_dnx_oam_endpoint_get(unit, endpoint, &endpoint_info));

    if (DNX_OAM_IS_DEFAULT_MEP(p_endpoint_info))
    {
        /*
         * DEFAULT MEP
         */
        SHR_IF_ERR_EXIT(dnx_oam_default_endpoint_destroy(unit, &endpoint_info));
    }
    else if (DNX_OAM_MEP_INDEX_IS_REMOTE(endpoint))
        /*
         * REMOTE MEP
         */
    {
        DNX_OAM_REMOTE_MEP_INDEX_TO_INTERNAL(rmep_idx_internal, endpoint);
        /*
         * Remote endpoint DESTROY
         */
        SHR_IF_ERR_EXIT(dnx_oam_remote_endpoint_destroy(unit, rmep_idx_internal, &endpoint_info));
    }
    else
    {
        /*
         *  LOCAL NON-DEFAULT MEP
         */
        SHR_IF_ERR_EXIT(dnx_oam_endpoint_destroy(unit, &endpoint_info));
    }

    SHR_EXIT();
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete all MplsLmDm endpoints.
 * This function creates an iterator for the OAM endpoint SW
 * state table, then uses bcm_dnx_oam_endpoint_destroy on each
 * endpoint in that table.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] first_mep_id - Store first deleted MEP.
 * \param [out] last_mep_id - Store last deleted MEP.
 * \param [out] destroyed_meps - Number of deleted MEPs.
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
dnx_oam_rfc6374_endpoint_destroy_all_internal(
    int unit,
    uint32 *first_mep_id,
    uint32 *last_mep_id,
    uint16 *destroyed_meps)
{
    uint32 entry_handle_id, endpoint_id = 0;
    int is_end;
    uint32 *field_val;
    uint32 mep_type;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    /** Get handle to table   */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_ENDPOINT_INFO_SW, &entry_handle_id));

    /** Allocate array for reading key */
    field_val = NULL;
    SHR_ALLOC(field_val, DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS * sizeof(uint32),
              "Array for reading profile table entry key", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** Create iterator   */
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));

    /** Read first entry   */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    /** Loop over all entries in table   */
    while (!is_end)
    {
        /** Get endpoint ID from the entry key  */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_OAM_ENDPOINT_SW_STATE_TYPE, INST_SINGLE, &mep_type));

        if (mep_type == bcmOAMEndpointTypeMplsLmDmPw || mep_type == bcmOAMEndpointTypeMplsLmDmLsp
            || mep_type == bcmOAMEndpointTypeMplsLmDmSection)
        {
            /** Get endpoint ID from the entry key  */
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_OAM_ENDPOINT_ID, field_val));
            endpoint_id = field_val[0];

            /** Use the endpoint ID to destroy the endpoint   */
            SHR_IF_ERR_EXIT(bcm_dnx_oam_endpoint_destroy(unit, endpoint_id));

            /** Count number of destroyed MEPs */
            destroyed_meps++;
            /** Store the first MEP id. For diagnostic puprose.*/
            if (*destroyed_meps == 1)
            {
                *first_mep_id = endpoint_id;
            }
        }

        /** Read next entry   */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

    /** Store the last endpoint ID */
    *last_mep_id = endpoint_id;

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Destroy all OAM endpoints in OAM group.
* \par DIRECT INPUT:
*   \param [in] unit  -
*   Relevant unit.
*   \param [in] group -
*   ID of the OAM group in which to destroy all endpoints.
*   \param [out] first_mep_id - Store first deleted MEP.
*   \param [out] last_mep_id - Store last deleted MEP.
*   \param [out] destroyed_meps - Number of deleted MEPs.
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_endpoint_create
*   * \ref bcm_dnx_oam_endpoint_destroy
*   * \ref bcm_dnx_oam_endpoint_get
*/
static shr_error_e
dnx_oam_endpoint_destroy_all_internal(
    int unit,
    bcm_oam_group_t group,
    uint32 *first_mep_id,
    uint32 *last_mep_id,
    uint16 *destroyed_meps)
{
    sw_state_ll_node_t node;
    bcm_oam_group_t mep_id = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Get the first element of Oam group linked list*/
    SHR_IF_ERR_EXIT(oam_sw_db_info.oam_group_sw_db_info.oam_group_array_of_linked_lists.get_first(unit, group, &node));

    /** While there are elements in the Linked list */
    while (DNX_SW_STATE_LL_IS_NODE_VALID(node))
    {
        /** Get mep_id based on Node and Group */
        SHR_IF_ERR_EXIT(oam_sw_db_info.oam_group_sw_db_info.
                        oam_group_array_of_linked_lists.node_value(unit, node, &mep_id));

        /** Get next Node from the linked list*/
        oam_sw_db_info.oam_group_sw_db_info.oam_group_array_of_linked_lists.next_node(unit, group, node, &node);

        /** Destroy the endpoint, using the endpoint_destroy API */
        SHR_IF_ERR_EXIT(bcm_dnx_oam_endpoint_destroy(unit, mep_id));

        /** Count number of destroyed MEPs */
        *destroyed_meps = *destroyed_meps + 1;
        /** Store the first MEP id. For diagnostic puprose.*/
        if (*destroyed_meps == 1)
        {
            *first_mep_id = mep_id;
        }
    }

    /** Store the last endpoint ID */
    *last_mep_id = mep_id;

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Destroy all OAM endpoints in OAM group.
* \par DIRECT INPUT:
*   \param [in] unit  -
*   Relevant unit.
*   \param [in] group -
*   ID of the OAM group in which to destroy all endpoints.
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_endpoint_create
*   * \ref bcm_dnx_oam_endpoint_destroy
*   * \ref bcm_dnx_oam_endpoint_get
*/
shr_error_e
bcm_dnx_oam_endpoint_destroy_all(
    int unit,
    bcm_oam_group_t group)
{
    uint16 destroyed_meps = 0;
    uint32 first_mep_id = 0, last_mep_id = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    if (group == BCM_OAM_GROUP_INVALID)
    {
        /** */
        SHR_IF_ERR_EXIT(dnx_oam_rfc6374_endpoint_destroy_all_internal
                        (unit, &first_mep_id, &last_mep_id, &destroyed_meps));
    }
    else
    {
        /** */
        SHR_IF_ERR_EXIT(dnx_oam_endpoint_destroy_all_internal
                        (unit, group, &first_mep_id, &last_mep_id, &destroyed_meps));
    }

    /*
     *   Warning message : Print how many MEPs were destroyed in a group
     *   if there were any.
     *   Print First/Last MEP IDs as well
     */
    if (destroyed_meps == 0)
    {
        LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "\n Group %d does not have any MEPs \n"), group));
    }
    else if (destroyed_meps == 1)
    {
        LOG_VERBOSE(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit, "\n Destroyed one MEP, in group %d. \n With ID : 0x%x \n"), group, first_mep_id));
    }
    else
    {
        LOG_VERBOSE(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "\n Destroyed a total of %d MEPs, in group %d. \n With first ID : 0x%x \n And last ID : 0x%x \n"),
                     destroyed_meps, group, first_mep_id, last_mep_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Traverse the set of OAM endpoints and call a
*  function with the data for each entry found,
*  associated with specific group
* \param [in] unit -  The unit number.
* \param [in] group -  The group id.
* \param [in] cb - the name of the callback function that is
*        going to be called for each valid entry
* \param [in] user_data - data that is passed to the callback
 *       function
* \return
*   * Error indication based on enum shr_error_e
* \remark
*   * None
* \see
*   * bcm_dnx_oam_endpoint_create
*   * bcm_dnx_oam_endpoint_get
*   * bcm_dnx_oam_endpoint_destroy
*   * bcm_dnx_oam_endpoint_destroy_all
*/
shr_error_e
bcm_dnx_oam_endpoint_traverse(
    int unit,
    bcm_oam_group_t group,
    bcm_oam_endpoint_traverse_cb cb,
    void *user_data)
{
    bcm_oam_endpoint_info_t endpoint_info;
    sw_state_ll_node_t node;
    bcm_oam_endpoint_t endpoint;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Setting pointers value to receive the fields */
    bcm_oam_endpoint_info_t_init(&endpoint_info);

    /** Get the first element of Oam group linked list*/
    SHR_IF_ERR_EXIT(oam_sw_db_info.oam_group_sw_db_info.oam_group_array_of_linked_lists.get_first(unit, group, &node));

    /** While there are elements in the Linked list */
    while (DNX_SW_STATE_LL_IS_NODE_VALID(node))
    {
        /** Get mep_id based on Node and Group */
        SHR_IF_ERR_EXIT(oam_sw_db_info.oam_group_sw_db_info.
                        oam_group_array_of_linked_lists.node_value(unit, node, &endpoint));

        /** Get next Node from the linked list*/
        oam_sw_db_info.oam_group_sw_db_info.oam_group_array_of_linked_lists.next_node(unit, group, node, &node);

        /** Getting enpoint info */
        SHR_IF_ERR_EXIT(bcm_dnx_oam_endpoint_get(unit, endpoint, &endpoint_info));

        if (cb != NULL)
        {
            /*
             * Invoke callback function
             */
            SHR_IF_ERR_EXIT((*cb) (unit, &endpoint_info, user_data));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong callback function was provided");
        }

    }

exit:
    SHR_FUNC_EXIT;
}
