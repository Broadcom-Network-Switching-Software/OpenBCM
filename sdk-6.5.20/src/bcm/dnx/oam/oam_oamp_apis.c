/** \file oam_oamp_apis.c
 *
 * APIs that access the OAMP-relevant dbal tables
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif /* */
#define BSL_LOG_MODULE BSL_LS_BCMDNX_OAM

/*
 *  * Include files. * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <src/bcm/dnx/oam/oam_internal.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <src/bcm/dnx/oam/oam_oamp.h>
#include <bcm_int/dnx/algo/oamp/algo_oamp.h>
#include <bcm_int/dnx/bfd/bfd.h>

/*
 *  }
 */

/*
 *  DEFINEs {
 */
/** For result types, these bits denote the new, MDB supported entry formats */
#define OFFLOAD_BIT 4
#define Q_ENTRY_BIT 5

/** Needed for the PP_PORT/PORT_CORE field */
#define PORT_CORE_SHIFT 8

/** Needed to split the CCM Interval/BFD Tx rate to two parts */
#define CCM_INT_BFD_RATE_MSB_MASK 0x6

/** Maximum number of times to attempt writing a MEP DB entry */
#define MAX_WRITE_ATTEMPTS 10

/*
 *  }
 */

/*
 *  MACROs {
 */

/*
 *  }
 */

/*
 *  * Global and Static
 */
/*
 *  }
 */

/*
 * Argument verification that is the same for dnx_oam_oamp_mep_db_get and dnx_oam_oamp_mep_db_add
 */
static shr_error_e
dnx_oam_oamp_mep_db_verify(
    int unit,
    const uint32 oamp_entry_id)
{
    uint32 max_mep_db_entries;
    SHR_FUNC_INIT_VARS(unit);

    /** This parameter is given in 1/4 entries   */
    max_mep_db_entries = MEP_DB_ENTRY_TO_OAM_ID(dnx_data_oam.oamp.nof_mep_db_entries_get(unit));
    if (oamp_entry_id >= max_mep_db_entries)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Entry ID 0x%08X out of range, highest is 0x%08X\n", oamp_entry_id,
                     max_mep_db_entries - 1);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function adds a single ethernet OAM CCM
 *        self-contained endpoint entry to the OAMP MEP DB table
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [in] entry_values - struct containing the values to be
 *        written to the entry.
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
dnx_oam_oamp_mep_db_eth_oam_add(
    int unit,
    const uint32 entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values)
{
    uint32 entry_handle_id, flag;
    dbal_fields_e result_type;
    int system_headers_mode;
    uint8 upmep_server = FALSE;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DB, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_ENTRY_ID, entry_id);

    /** Set result type   */
    if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_IS_UPMEP))
    {
        if ((entry_values->exclusive.oam_only.server_destination != 0) &&
            (entry_values->exclusive.oam_only.server_destination != BCM_GPORT_INVALID))
        {
            result_type = DBAL_RESULT_TYPE_OAMP_MEP_DB_ETH_OAM_UP_MEP_SERVER;
            upmep_server = TRUE;
        }
        else
        {
            result_type = DBAL_RESULT_TYPE_OAMP_MEP_DB_ETH_OAM_UP_MEP;
        }
    }
    else
    {
        if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_EGRESS_INJECTION))
        {
            result_type = DBAL_RESULT_TYPE_OAMP_MEP_DB_ETH_OAM_DOWN_MEP_EGRESS_INJECTION;
        }
        else
        {
            result_type = DBAL_RESULT_TYPE_OAMP_MEP_DB_ETH_OAM_DOWN_MEP;
        }
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);

    /** Set field values   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CCM_INTERVAL, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_ccm_interval);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ITMH_TC_DP_PROFILE, INST_SINGLE,
                                 entry_values->itmh_tc_dp_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDL, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_mdl);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAID, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_maid);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ICC_MAP_INDEX, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_icc_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_ID, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_mep_id);

    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_RDI_FROM_SCANNER);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RDI_FROM_SCANNER, INST_SINGLE, flag);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_RDI_FROM_PACKET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RDI_FROM_PACKET, INST_SINGLE, flag);
    if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_IS_UPMEP))
    {
        /**
         * CCM Ethernet up-MEP is the only endpoint type where
         * the separation "PP port" and "port core" mean
         * something
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, INST_SINGLE,
                                     entry_values->unified_port_access.ccm_eth_up_mep_port.pp_port);

        if (dnx_data_oam.oamp.core_nof_bits_get(unit) > 0)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_CORE, INST_SINGLE,
                                         entry_values->unified_port_access.ccm_eth_up_mep_port.port_core);
        }
        else
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_CORE, INST_SINGLE, 0);
        }

        if (upmep_server)
        {
            /** Field mapped to "reserved" relevant only to UP-MEP in server mode */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SERVER_DESTINATION, INST_SINGLE,
                                         entry_values->exclusive.oam_only.server_destination);
        }
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, INST_SINGLE,
                                     entry_values->unified_port_access.port_profile);
        if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_EGRESS_INJECTION))
        {
            /*
             * Mapping of VSI and OUT_LIF is different on inter-op mode Part of VSI in interop mode is stored in Gen
             * Mem
             */
            if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI_JR1, INST_SINGLE,
                                             entry_values->exclusive.oam_only.vsi);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF_JR1, INST_SINGLE,
                                             entry_values->fec_id_or_glob_out_lif);
            }
            else
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE,
                                             entry_values->exclusive.oam_only.vsi);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE,
                                             entry_values->fec_id_or_glob_out_lif);
            }
        }
    }

    /*
     * In inter-op mode Counter should be stamped in OAM-TS Header
     */
    if (!upmep_server && !(UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_EGRESS_INJECTION) &&
                           (system_headers_mode != DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)))
    {

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_INTERFACE, INST_SINGLE,
                                     entry_values->counter_interface);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_BASE, INST_SINGLE,
                                     entry_values->counter_ptr);
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INNER_TPID_INDEX, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_inner_tpid_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INNER_VID, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_inner_vid);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INNER_DEI_PCP, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_inner_dei_pcp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_TPID_INDEX, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_outer_tpid_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_VID, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_outer_vid);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_DEI_PCP, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_outer_dei_pcp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_VLAN_TAGS, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_nof_vlan_tags);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UP_PTCH_OPAQUE_PT_ATTR_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_prt_qualifier_profile);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_PORT_TLV_EN);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_STATUS_TLV_EN, INST_SINGLE, flag);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_PORT_TLV_VAL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_STATUS_TLV_VAL, INST_SINGLE, flag);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INTERFACE_STATUS_TLV_CODE, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_interface_status_tlv_code);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE, INST_SINGLE, entry_values->mep_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LMM_DA_LSB_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_lmm_da_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PE_PROFILE, INST_SINGLE,
                                 entry_values->mep_pe_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SA_MAC_LSB, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_sa_mac_lsb);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SA_GEN_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_sa_mac_msb_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXTRA_DATA_PTR, INST_SINGLE,
                                 entry_values->extra_data_ptr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLEX_LM_DM_PTR, INST_SINGLE,
                                 entry_values->flex_lm_dm_ptr);
    /** Ethernet OAM CCM packets are always multicast, unicast not supported */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DA_UC_MC_TYPE, INST_SINGLE,
                                 UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_MC_UC_SEL));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_CORE_SELECT, INST_SINGLE,
                                 entry_values->crps_core_select);

    /** Write entry   */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function adds a single OAM CCM over MPLS-TP
 *        self-contained endpoint entry to the OAMP MEP DB table
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [in] entry_values - struct containing the values to be
 *        written to the entry.
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
dnx_oam_oamp_mep_db_mplstp_oam_add(
    int unit,
    const uint32 entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values)
{
    uint32 entry_handle_id, flag;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DB, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_ENTRY_ID, entry_id);

    /** Set result type   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_OAMP_MEP_DB_Y1731_MPLSTP);

    /** Set field values   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CCM_INTERVAL, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_ccm_interval);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ITMH_TC_DP_PROFILE, INST_SINGLE,
                                 entry_values->itmh_tc_dp_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, INST_SINGLE,
                                 entry_values->dest_sys_port_agr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDL, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_mdl);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAID, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_maid);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ICC_MAP_INDEX, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_icc_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_ID, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_mep_id);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_RDI_FROM_SCANNER);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RDI_FROM_SCANNER, INST_SINGLE, flag);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_RDI_FROM_PACKET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RDI_FROM_PACKET, INST_SINGLE, flag);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF, INST_SINGLE,
                                 entry_values->fec_id_or_glob_out_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, INST_SINGLE,
                                 entry_values->mpls_pwe.label);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_PUSH_PROFILE, INST_SINGLE,
                                 entry_values->mpls_pwe.push_profile);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_PORT_TLV_EN);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_STATUS_TLV_EN, INST_SINGLE, flag);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_PORT_TLV_VAL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_STATUS_TLV_VAL, INST_SINGLE, flag);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INTERFACE_STATUS_TLV_CODE, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_interface_status_tlv_code);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE, INST_SINGLE, entry_values->mep_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_BASE, INST_SINGLE,
                                 entry_values->counter_ptr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_INTERFACE, INST_SINGLE,
                                 entry_values->counter_interface);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PE_PROFILE, INST_SINGLE,
                                 entry_values->mep_pe_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_PORT_TO_OAMP_PE, INST_SINGLE,
                                 entry_values->unified_port_access.port_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXTRA_DATA_PTR, INST_SINGLE,
                                 entry_values->extra_data_ptr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLEX_LM_DM_PTR, INST_SINGLE,
                                 entry_values->flex_lm_dm_ptr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_CORE_SELECT, INST_SINGLE,
                                 entry_values->crps_core_select);

    /** Write entry   */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function adds a single OAM CCM over PWE
 *        self-contained endpoint entry to the OAMP MEP DB table
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [in] entry_values - struct containing the values to be
 *        written to the entry.
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
dnx_oam_oamp_mep_db_pwe_oam_add(
    int unit,
    const uint32 entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values)
{
    uint32 entry_handle_id, flag;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DB, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_ENTRY_ID, entry_id);

    /** Set result type   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_OAMP_MEP_DB_Y1731_PWE);

    /** Set field values   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CCM_INTERVAL, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_ccm_interval);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ITMH_TC_DP_PROFILE, INST_SINGLE,
                                 entry_values->itmh_tc_dp_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, INST_SINGLE,
                                 entry_values->dest_sys_port_agr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDL, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_mdl);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAID, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_maid);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ICC_MAP_INDEX, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_icc_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_ID, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_mep_id);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_RDI_FROM_SCANNER);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RDI_FROM_SCANNER, INST_SINGLE, flag);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_RDI_FROM_PACKET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RDI_FROM_PACKET, INST_SINGLE, flag);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF, INST_SINGLE,
                                 entry_values->fec_id_or_glob_out_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, INST_SINGLE,
                                 entry_values->mpls_pwe.label);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_PUSH_PROFILE, INST_SINGLE,
                                 entry_values->mpls_pwe.push_profile);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_PORT_TLV_EN);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_STATUS_TLV_EN, INST_SINGLE, flag);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_PORT_TLV_VAL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_STATUS_TLV_VAL, INST_SINGLE, flag);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INTERFACE_STATUS_TLV_CODE, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_interface_status_tlv_code);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE, INST_SINGLE, entry_values->mep_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_BASE, INST_SINGLE,
                                 entry_values->counter_ptr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_INTERFACE, INST_SINGLE,
                                 entry_values->counter_interface);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PE_PROFILE, INST_SINGLE,
                                 entry_values->mep_pe_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_PORT_TO_OAMP_PE, INST_SINGLE,
                                 entry_values->unified_port_access.port_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXTRA_DATA_PTR, INST_SINGLE,
                                 entry_values->extra_data_ptr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLEX_LM_DM_PTR, INST_SINGLE,
                                 entry_values->flex_lm_dm_ptr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_CORE_SELECT, INST_SINGLE,
                                 entry_values->crps_core_select);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SIGNAL_DEGRADATION, INST_SINGLE,
                                 entry_values->exclusive.oam_only.tx_signal);

    /** Write entry   */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function adds a single BFD single-hop
 *        self-contained endpoint entry to the OAMP MEP DB table
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [in] entry_values - struct containing the values to be
 *        written to the entry.
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
dnx_oam_oamp_mep_db_bfd_one_hop_add(
    int unit,
    const uint32 entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values)
{
    uint32 entry_handle_id, flag;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DB, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_ENTRY_ID, entry_id);

    /** Set result type   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_OAMP_MEP_DB_BFD_IPV4_ONE_HOP);

    /** Set field values   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_TX_RATE_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_tx_rate);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ITMH_TC_DP_PROFILE, INST_SINGLE,
                                 entry_values->itmh_tc_dp_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, INST_SINGLE,
                                 entry_values->unified_port_access.port_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_YOUR_DISC, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_your_disc);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_SRC_IP_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_src_ip_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DETECT_MULT, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_detect_mult);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIN_TX_INTERVAL_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_min_tx_interval_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIN_RX_INTERVAL_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_min_rx_interval_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF, INST_SINGLE,
                                 entry_values->fec_id_or_glob_out_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_DIAG_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_diag_profile);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_BFD_TUNNEL_MPLS_ENABLE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_TUNNEL_MPLS_ENABLE, INST_SINGLE, flag);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_FLAGS_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_flags_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_STATE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_sta);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PE_PROFILE, INST_SINGLE,
                                 entry_values->mep_pe_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE, INST_SINGLE, entry_values->mep_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4_DIP, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_ipv4_dip);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IP_SUBNET_LEN, INST_SINGLE,
                                 entry_values->ip_subnet_len);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXTRA_DATA_PTR, INST_SINGLE,
                                 entry_values->extra_data_ptr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLEX_LM_DM_PTR, INST_SINGLE,
                                 entry_values->flex_lm_dm_ptr);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_MICRO_BFD);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MICRO_BFD, INST_SINGLE, flag);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_CORE_SELECT, INST_SINGLE,
                                 entry_values->crps_core_select);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_SET_DISCR_MSB);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SET_DISCR_MSB, INST_SINGLE, flag);

    /** Write entry   */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function adds a single BFD multi-hop
 *        self-contained endpoint entry to the OAMP MEP DB table
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [in] entry_values - struct containing the values to be
 *        written to the entry.
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
dnx_oam_oamp_mep_db_bfd_multi_hop_add(
    int unit,
    const uint32 entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values)
{
    uint32 entry_handle_id, flag;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DB, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_ENTRY_ID, entry_id);

    /** Set result type   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_OAMP_MEP_DB_BFD_IPV4_MULTI_HOP);

    /** Set field values   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_TX_RATE_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_tx_rate);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ITMH_TC_DP_PROFILE, INST_SINGLE,
                                 entry_values->itmh_tc_dp_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, INST_SINGLE,
                                 entry_values->unified_port_access.port_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_YOUR_DISC, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_your_disc);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4_DIP, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_ipv4_dip);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_SRC_IP_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_src_ip_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IP_TTL_TOS_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_ip_ttl_tos_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DETECT_MULT, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_detect_mult);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIN_TX_INTERVAL_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_min_tx_interval_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIN_RX_INTERVAL_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_min_rx_interval_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF, INST_SINGLE,
                                 entry_values->fec_id_or_glob_out_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_DIAG_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_diag_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_FLAGS_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_flags_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_STATE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_sta);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE, INST_SINGLE, entry_values->mep_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PE_PROFILE, INST_SINGLE,
                                 entry_values->mep_pe_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXTRA_DATA_PTR, INST_SINGLE,
                                 entry_values->extra_data_ptr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLEX_LM_DM_PTR, INST_SINGLE,
                                 entry_values->flex_lm_dm_ptr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_CORE_SELECT, INST_SINGLE,
                                 entry_values->crps_core_select);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_SET_DISCR_MSB);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SET_DISCR_MSB, INST_SINGLE, flag);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_SEAMLESS_BFD);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SET_SEAMLESS_PORTS, INST_SINGLE, flag);

    /** Write entry   */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function adds a single BFD over MPLS
 *        self-contained endpoint entry to the OAMP MEP DB table
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [in] entry_values - struct containing the values to be
 *        written to the entry.
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
dnx_oam_oamp_mep_db_bfd_mpls_add(
    int unit,
    const uint32 entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values)
{
    uint32 entry_handle_id, flag;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DB, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_ENTRY_ID, entry_id);

    /** Set result type   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_OAMP_MEP_DB_BFD_MPLS);

    /** Set field values   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_TX_RATE_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_tx_rate);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ITMH_TC_DP_PROFILE, INST_SINGLE,
                                 entry_values->itmh_tc_dp_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, INST_SINGLE,
                                 entry_values->unified_port_access.port_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_YOUR_DISC, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_your_disc);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_SRC_IP_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_src_ip_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DETECT_MULT, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_detect_mult);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIN_TX_INTERVAL_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_min_tx_interval_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIN_RX_INTERVAL_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_min_rx_interval_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF, INST_SINGLE,
                                 entry_values->fec_id_or_glob_out_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_DIAG_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_diag_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, INST_SINGLE,
                                 entry_values->mpls_pwe.label);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_PUSH_PROFILE, INST_SINGLE,
                                 entry_values->mpls_pwe.push_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IP_TTL_TOS_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_ip_ttl_tos_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_FLAGS_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_flags_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_STATE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_sta);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PE_PROFILE, INST_SINGLE,
                                 entry_values->mep_pe_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE, INST_SINGLE, entry_values->mep_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXTRA_DATA_PTR, INST_SINGLE,
                                 entry_values->extra_data_ptr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLEX_LM_DM_PTR, INST_SINGLE,
                                 entry_values->flex_lm_dm_ptr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_CORE_SELECT, INST_SINGLE,
                                 entry_values->crps_core_select);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_SET_DISCR_MSB);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SET_DISCR_MSB, INST_SINGLE, flag);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_SEAMLESS_BFD);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SET_SEAMLESS_PORTS, INST_SINGLE, flag);

    /** Write entry   */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function adds a single BFD over PWE
 *        self-contained endpoint entry to the OAMP MEP DB table
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [in] entry_values - struct containing the values to be
 *        written to the entry.
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
dnx_oam_oamp_mep_db_bfd_pwe_add(
    int unit,
    const uint32 entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values)
{
    uint32 entry_handle_id, flag;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DB, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_ENTRY_ID, entry_id);

    /** Set result type   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_OAMP_MEP_DB_BFD_PWE);

    /** Set field values   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_TX_RATE_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_tx_rate);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ITMH_TC_DP_PROFILE, INST_SINGLE,
                                 entry_values->itmh_tc_dp_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, INST_SINGLE,
                                 entry_values->dest_sys_port_agr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_YOUR_DISC, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_your_disc);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DETECT_MULT, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_detect_mult);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIN_TX_INTERVAL_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_min_tx_interval_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIN_RX_INTERVAL_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_min_rx_interval_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF, INST_SINGLE,
                                 entry_values->fec_id_or_glob_out_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_DIAG_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_diag_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, INST_SINGLE,
                                 entry_values->mpls_pwe.label);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_PUSH_PROFILE, INST_SINGLE,
                                 entry_values->mpls_pwe.push_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_FLAGS_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_flags_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_STATE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_sta);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_ACH);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACH, INST_SINGLE, flag);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_GAL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GAL, INST_SINGLE, flag);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_ROUTER_ALERT);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ROUTER_ALERT, INST_SINGLE, flag);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PE_PROFILE, INST_SINGLE,
                                 entry_values->mep_pe_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE, INST_SINGLE, entry_values->mep_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACH_SEL, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_ach_sel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, INST_SINGLE,
                                 entry_values->unified_port_access.port_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXTRA_DATA_PTR, INST_SINGLE,
                                 entry_values->extra_data_ptr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLEX_LM_DM_PTR, INST_SINGLE,
                                 entry_values->flex_lm_dm_ptr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_CORE_SELECT, INST_SINGLE,
                                 entry_values->crps_core_select);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_SET_DISCR_MSB);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SET_DISCR_MSB, INST_SINGLE, flag);

    /** Write entry   */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function adds a single RFC 6374
 *        self-contained endpoint entry to the OAMP MEP DB table
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table
 * \param [in] entry_values - struct containing the values to be
 *        written to the entry.
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
dnx_oam_oamp_mep_db_rfc_6374_add(
    int unit,
    const uint32 entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values)
{
    uint32 entry_handle_id, flag;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DB, &entry_handle_id));
    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_ENTRY_ID, entry_id);

    /** Set result type   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_OAMP_MEP_DB_RFC_6374);

    /** Set field values   */
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_LAST_ENTRY);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAST_ENTRY, INST_SINGLE, flag);

    /** Set field values   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_BASE, INST_SINGLE,
                                 entry_values->counter_ptr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_INTERFACE, INST_SINGLE,
                                 entry_values->counter_interface);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TIMESTAMP_FORMAT, INST_SINGLE,
                                 entry_values->exclusive.rfc_6374_only.timestamp_format);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SESSION_IDENTIFIER, INST_SINGLE,
                                 entry_values->exclusive.rfc_6374_only.session_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DM_T_FLAG, INST_SINGLE,
                                 entry_values->exclusive.rfc_6374_only.dm_t_flag);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DM_DS_FIELD, INST_SINGLE,
                                 entry_values->exclusive.rfc_6374_only.dm_ds);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LM_CW_CHOOSE, INST_SINGLE,
                                 entry_values->exclusive.rfc_6374_only.lm_cw_choose);
    /** Write entry   */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function adds an LM_DB type entry to the
 *        OAMP MEP DB.  This is an optional extension of a
 *        self-contained OAM endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [in] entry_values - struct containing the values to be
 *        written to the entry.
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
dnx_oam_oamp_mep_db_lm_db_add(
    int unit,
    const uint32 entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values)
{
    uint32 entry_handle_id, flag;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DB, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_ENTRY_ID, entry_id);

    /** Set result type   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_OAMP_MEP_DB_LM_DB);

    /** Set field values   */
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_LAST_ENTRY);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAST_ENTRY, INST_SINGLE, flag);

    if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_COPY_LM_STATISTICS))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LM_MY_TX, INST_SINGLE,
                                     entry_values->exclusive.lm_dm.lm_my_tx);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LM_MY_RX, INST_SINGLE,
                                     entry_values->exclusive.lm_dm.lm_my_rx);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LM_PEER_TX, INST_SINGLE,
                                     entry_values->exclusive.lm_dm.lm_peer_tx);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LM_PEER_RX, INST_SINGLE,
                                     entry_values->exclusive.lm_dm.lm_peer_rx);
        flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_LM_CNTRS_VALID);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LM_CNTRS_VALID, INST_SINGLE, flag);
        flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_MEASURE_NEXT_RECEIVED_SLR);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEASURE_NEXT_RECEIVED_SLR, INST_SINGLE, flag);
    }

    /** Write entry   */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function adds an LM_STAT type entry to the
 *        OAMP MEP DB.  This is an optional extension of a
 *        self-contained OAM endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [in] entry_values - struct containing the values to be
 *        written to the entry.
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
dnx_oam_oamp_mep_db_lm_stat_add(
    int unit,
    const uint32 entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values)
{
    uint32 entry_handle_id, flag;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DB, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_ENTRY_ID, entry_id);

    /** Set result type   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_OAMP_MEP_DB_LM_STAT);

    /** Set field values   */
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_LAST_ENTRY);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAST_ENTRY, INST_SINGLE, flag);

    if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_COPY_LM_STATISTICS))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAST_LM_FAR, INST_SINGLE,
                                     entry_values->exclusive.lm_dm.last_lm_far);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAST_LM_NEAR, INST_SINGLE,
                                     entry_values->exclusive.lm_dm.last_lm_near);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACC_LM_FAR, INST_SINGLE,
                                     entry_values->exclusive.lm_dm.acc_lm_far);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACC_LM_NEAR, INST_SINGLE,
                                     entry_values->exclusive.lm_dm.acc_lm_near);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_LM_FAR, INST_SINGLE,
                                     entry_values->exclusive.lm_dm.max_lm_far);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_LM_NEAR, INST_SINGLE,
                                     entry_values->exclusive.lm_dm.max_lm_near);
    }

    /** Write entry   */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function adds a one-way DM_STAT type entry to
 *        the OAMP MEP DB.  This is an optional extension of a
 *        self-contained OAM endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [in] entry_values - struct containing the values to be
 *        written to the entry.
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
dnx_oam_oamp_mep_db_dm_stat_one_way_add(
    int unit,
    const uint32 entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values)
{
    uint32 entry_handle_id, flag;
    uint8 *value_array8 = NULL;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(value_array8, DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES, "Array for over 32-bit dbal fields",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DB, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_ENTRY_ID, entry_id);

    /** Set result type   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_OAMP_MEP_DB_DM_STAT_ONE_WAY);

    /** Set field values   */
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_LAST_ENTRY);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAST_ENTRY, INST_SINGLE, flag);

    if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_RESET_DM_STATISTICS))
    {
        /** Clear all "read only" DM fields */
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_LAST_DELAY_ONE_WAY, INST_SINGLE,
                                        value_array8);
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_MAX_DELAY_ONE_WAY, INST_SINGLE, value_array8);
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_LAST_DELAY_ONE_WAY_NEAR_END, INST_SINGLE,
                                        value_array8);
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_MAX_DELAY_ONE_WAY_NEAR_END, INST_SINGLE,
                                        value_array8);
        /** Reset minimum must be the maximum value */
        sal_memset(value_array8, 0xFF, DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES);
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_MIN_DELAY_ONE_WAY, INST_SINGLE, value_array8);
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_MIN_DELAY_ONE_WAY_NEAR_END, INST_SINGLE,
                                        value_array8);
    }

    if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_COPY_DM_STATISTICS))
    {
        /** If we move the entry, we need to copy the values */
        sal_memcpy(value_array8, entry_values->exclusive.lm_dm.last_delay_one_way, ONE_WAY_DELAY_SIZE);
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_LAST_DELAY_ONE_WAY, INST_SINGLE,
                                        value_array8);
        sal_memcpy(value_array8, entry_values->exclusive.lm_dm.max_delay_one_way, ONE_WAY_DELAY_SIZE);
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_MAX_DELAY_ONE_WAY, INST_SINGLE, value_array8);
        sal_memcpy(value_array8, entry_values->exclusive.lm_dm.min_delay_one_way, ONE_WAY_DELAY_SIZE);
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_MIN_DELAY_ONE_WAY, INST_SINGLE, value_array8);
        sal_memcpy(value_array8, entry_values->exclusive.lm_dm.last_delay_one_way, ONE_WAY_DELAY_SIZE);
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_LAST_DELAY_ONE_WAY_NEAR_END, INST_SINGLE,
                                        value_array8);
        sal_memcpy(value_array8, entry_values->exclusive.lm_dm.max_delay_one_way, ONE_WAY_DELAY_SIZE);
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_MAX_DELAY_ONE_WAY_NEAR_END, INST_SINGLE,
                                        value_array8);
        sal_memcpy(value_array8, entry_values->exclusive.lm_dm.min_delay_one_way, ONE_WAY_DELAY_SIZE);
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_MIN_DELAY_ONE_WAY_NEAR_END, INST_SINGLE,
                                        value_array8);
    }

    /** Write entry   */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    SHR_FREE(value_array8);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function adds a two-way DM_STAT type entry to
 *        the OAMP MEP DB.  This is an optional extension of a
 *        self-contained OAM endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [in] entry_values - struct containing the values to be
 *        written to the entry.
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
dnx_oam_oamp_mep_db_dm_stat_two_way_add(
    int unit,
    const uint32 entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values)
{
    uint32 entry_handle_id, flag;
    uint8 *value_array8 = NULL;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(value_array8, DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES, "Array for over 32-bit dbal fields",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DB, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_ENTRY_ID, entry_id);

    /** Set result type   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_OAMP_MEP_DB_DM_STAT_TWO_WAY);

    /** Set field values   */
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_LAST_ENTRY);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAST_ENTRY, INST_SINGLE, flag);

    if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_RESET_DM_STATISTICS))
    {
        /** Clear all "read only" DM fields */
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_LAST_DELAY, INST_SINGLE, value_array8);
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_MAX_DELAY, INST_SINGLE, value_array8);
        /** Reset minimum must be the maximum value */
        sal_memset(value_array8, 0xFF, DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES);
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_MIN_DELAY, INST_SINGLE, value_array8);
    }

    if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_COPY_DM_STATISTICS))
    {
        sal_memcpy(value_array8, entry_values->exclusive.lm_dm.last_delay, TWO_WAY_DELAY_SIZE);
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_LAST_DELAY, INST_SINGLE, value_array8);
        sal_memcpy(value_array8, entry_values->exclusive.lm_dm.max_delay, TWO_WAY_DELAY_SIZE);
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_MAX_DELAY, INST_SINGLE, value_array8);
        sal_memcpy(value_array8, entry_values->exclusive.lm_dm.min_delay, TWO_WAY_DELAY_SIZE);
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_MIN_DELAY, INST_SINGLE, value_array8);
    }

    /** Write entry   */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    SHR_FREE(value_array8);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function adds an extra data header entry to the
 *        OAMP MEP DB.  This can be used by multiple
 *        self-contained endpoints.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [in] entry_values - struct containing the values to be
 *        written to the entry.
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
dnx_oam_oamp_mep_db_extra_data_header_add(
    int unit,
    const uint32 entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DB, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_ENTRY_ID, entry_id);

    /** Set result type   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_OAMP_MEP_DB_EXTRA_DATA_HEADER);

    /** Set field values   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXT_DATA_LENGTH, INST_SINGLE,
                                 entry_values->exclusive.extra_data.extra_data_len);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODES_TO_PREPEND, INST_SINGLE,
                                 entry_values->exclusive.extra_data.opcode_bmp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CHECK_CRC_VALUE_1, INST_SINGLE,
                                 entry_values->exclusive.extra_data.crc_val1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CHECK_CRC_VALUE_2, INST_SINGLE,
                                 entry_values->exclusive.extra_data.crc_val2);
    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_EXTRA_DATA_SEGMENT, INST_SINGLE,
                                    (uint8 *) entry_values->exclusive.extra_data.data_segment);

    /** Write entry   */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function adds an extra data payload entry to
 *        the OAMP MEP DB.  This can be used by multiple
 *        self-contained endpoints.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [in] entry_values - struct containing the values to be
 *        written to the entry.
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
dnx_oam_oamp_mep_db_extra_data_payload_add(
    int unit,
    const uint32 entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DB, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_ENTRY_ID, entry_id);

    /** Set result type   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_OAMP_MEP_DB_EXTRA_DATA_PAYLOAD);

    /** Set field values   */
    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_EXTRA_DATA_SEGMENT, INST_SINGLE,
                                    (uint8 *) entry_values->exclusive.extra_data.data_segment);

    /** Write entry   */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function adds an entry to the OAMP MEP DB table
 *        that signifies an LM/DM offloaded OAM endpoint.
 *        These endpoints require this entry and at least
 *        two MDB entries.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [in] entry_values - struct containing the values to be
 *        written to the entry.
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
dnx_oam_oamp_mep_db_lm_dm_offloaded_part_1_add(
    int unit,
    const uint32 entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values)
{
    uint32 entry_handle_id, value, flag;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DB, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_ENTRY_ID, entry_id);

    /** Set result type   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_OAMP_MEP_DB_LM_DM_OFFLOADED_PART_1);

    /** Set field values   */

    /** Set MEP type   */
    switch (entry_values->mep_type)
    {
        case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM:
            value = DBAL_ENUM_FVAL_OFFLOADED_MEP_TYPE_SUFFIX_CCM_ETHERNET;
            break;

        case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_MPLSTP:
            value = DBAL_ENUM_FVAL_OFFLOADED_MEP_TYPE_SUFFIX_MPLSTP;
            break;

        case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE:
            value = DBAL_ENUM_FVAL_OFFLOADED_MEP_TYPE_SUFFIX_PWE;
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Internal error.  MEP type does not match result type.");
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OFFLOADED_MEP_TYPE_SUFFIX, INST_SINGLE, value);

    value = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_LM_ENABLE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LM_ENABLE, INST_SINGLE, value);
    value = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_DM_ENABLE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DM_ENABLE, INST_SINGLE, value);
    value = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_LM_STAT_ENABLE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LM_STAT_ENABLE, INST_SINGLE, value);
    value = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_DM_STAT_ENABLE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DM_STAT_ENABLE, INST_SINGLE, value);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_CORE_SELECT, INST_SINGLE,
                                 entry_values->crps_core_select);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PE_PROFILE, INST_SINGLE,
                                 entry_values->mep_pe_profile);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_RDI_FROM_SCANNER);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RDI_FROM_SCANNER, INST_SINGLE, flag);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_RDI_FROM_PACKET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RDI_FROM_PACKET, INST_SINGLE, flag);

    if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_RESET_LM_STATISTICS))
    {
        /** Clear all "read only" LM fields */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAST_LM_FAR, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAST_LM_NEAR, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_LM_FAR, INST_SINGLE, 0);
    }

    if (entry_values->flex_lm_dm_ptr != INVALID_ENTRY)
    {
        /**
         *  For LM/DM offloaded endpoints, the mandatory PART 1 entry can
         *  point to the optional PART 2 entry.
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PART_2_PTR, INST_SINGLE,
                                     entry_values->flex_lm_dm_ptr);
    }

    /** Write entry   */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function adds an entry to the OAMP MEP DB table
 *        that will be used for additional dynamic fields for an
 *        LM/DM offloaded OAM endpoint.  Adding this type of
 *        entry is optional.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [in] entry_values - struct containing the values to be
 *        written to the entry.
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
dnx_oam_oamp_mep_db_lm_dm_offloaded_part_2_add(
    int unit,
    const uint32 entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values)
{
    uint32 entry_handle_id;
    uint8 *delay_buffer = NULL;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(delay_buffer, DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES, "Array for over 32-bit dbal fields",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DB, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_ENTRY_ID, entry_id);

    /** Set result type   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_OAMP_MEP_DB_LM_DM_OFFLOADED_PART_2);

    if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_RESET_LM_STATISTICS))
    {
        /** Clear all "read only" LM fields */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_LM_NEAR, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACC_LM_FAR, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACC_LM_NEAR, INST_SINGLE, 0);
    }

    if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_RESET_DM_STATISTICS))
    {
        /** Clear all "read only" DM fields */
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_LAST_DELAY, INST_SINGLE, delay_buffer);
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_MAX_DELAY, INST_SINGLE, delay_buffer);
        /** Reset minimum must be the maximum value */
        sal_memset(delay_buffer, 0xFF, DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES);
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_MIN_DELAY, INST_SINGLE, delay_buffer);
    }

    /** Write entry   */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    SHR_FREE(delay_buffer);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function adds an 1/4 entry format OAM endpoint.
 *        At least one MDB entry is required.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [in] entry_values - struct containing the values to be
 *        written to the entry.
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
dnx_oam_oamp_mep_db_oam_q_entry_add(
    int unit,
    const uint32 entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values)
{
    uint32 entry_handle_id, value;
    dbal_fields_e result_type;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DB, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_ENTRY_ID, entry_id);

    /** Set result type   */
    if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_IS_UPMEP))
    {
        result_type = DBAL_RESULT_TYPE_OAMP_MEP_DB_Q_ENTRY_OAM_FORMAT_UP_MEP;
    }
    else
    {
        result_type = DBAL_RESULT_TYPE_OAMP_MEP_DB_Q_ENTRY_OAM_FORMAT_DOWN_MEP;
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);

    /** Set field values   */

    /** Set MEP type   */
    switch (entry_values->mep_type)
    {
        case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM:
            value = DBAL_ENUM_FVAL_SHORT_OAM_ENTRY_MEP_TYPE_SUFFIX_CCM_ETHERNET;
            break;

        case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_MPLSTP:
            value = DBAL_ENUM_FVAL_SHORT_OAM_ENTRY_MEP_TYPE_SUFFIX_MPLSTP;
            break;

        case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE:
            value = DBAL_ENUM_FVAL_SHORT_OAM_ENTRY_MEP_TYPE_SUFFIX_PWE;
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Internal error.  MEP type does not match result type.");
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SHORT_OAM_ENTRY_MEP_TYPE_SUFFIX, INST_SINGLE, value);

    /**
     * For JR2 A0, the port profile field had 9 bits.  From JR2 B0 onward,
     * this was increased to 10 bits.  To make room for the extra bits,
     * CCM interval was split into two fields: the original 3-bit fields
     * were reduced to 2 bits, and 1-bit interval fields were added in
     * different entries.  This was also done for BFD, with the Tx rate
     * fields.  See more examples below.
     */
    if (dnx_data_oam.oamp.modified_mep_db_structure_get(unit) == 1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CCM_INTERVAL, INST_SINGLE,
                                     entry_values->exclusive.oam_only.oam_ccm_interval >> 1);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CCM_INTERVAL, INST_SINGLE,
                                     entry_values->exclusive.oam_only.oam_ccm_interval);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ITMH_TC_DP_PROFILE, INST_SINGLE,
                                 entry_values->itmh_tc_dp_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ICC_MAP_INDEX, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_icc_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_ID, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_mep_id);
    value = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_RDI_FROM_SCANNER);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RDI_FROM_SCANNER, INST_SINGLE, value);
    value = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_RDI_FROM_PACKET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RDI_FROM_PACKET, INST_SINGLE, value);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE, INST_SINGLE, entry_values->mep_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PE_PROFILE, INST_SINGLE,
                                 entry_values->mep_pe_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_CORE_SELECT, INST_SINGLE,
                                 entry_values->crps_core_select);
    if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_IS_UPMEP)
        && (entry_values->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM))
    {
        /** Is this a CCM Ethernet up-MEP? */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, INST_SINGLE,
                                     entry_values->unified_port_access.ccm_eth_up_mep_port.pp_port);
        if (dnx_data_oam.oamp.core_nof_bits_get(unit) > 0)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_CORE, INST_SINGLE,
                                         entry_values->unified_port_access.ccm_eth_up_mep_port.port_core);
        }
        else
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_CORE, INST_SINGLE, 0);
        }
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, INST_SINGLE,
                                     entry_values->unified_port_access.port_profile);
    }

    /** Write entry   */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function adds an 1/4 entry format BFD endpoint.
 *        At least one MDB entry is required.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [in] entry_values - struct containing the values to be
 *        written to the entry.
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
dnx_oam_oamp_mep_db_bfd_q_entry_add(
    int unit,
    const uint32 entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values)
{
    uint32 entry_handle_id, mep_subtype;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DB, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_ENTRY_ID, entry_id);

    /** Set result type   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_OAMP_MEP_DB_Q_ENTRY_BFD_FORMAT);

    switch (entry_values->mep_type)
    {
        case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_1HOP:
            mep_subtype = DBAL_ENUM_FVAL_SHORT_BFD_ENTRY_MEP_TYPE_SUFFIX_1_HOP;
            break;

        case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP:
            mep_subtype = DBAL_ENUM_FVAL_SHORT_BFD_ENTRY_MEP_TYPE_SUFFIX_M_HOP;
            break;

        case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS:
            mep_subtype = DBAL_ENUM_FVAL_SHORT_BFD_ENTRY_MEP_TYPE_SUFFIX_MPLS;
            break;

        case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE:
            mep_subtype = DBAL_ENUM_FVAL_SHORT_BFD_ENTRY_MEP_TYPE_SUFFIX_PWE;
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Wrong internal function called");
    }

    /** Set field values   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SHORT_BFD_ENTRY_MEP_TYPE_SUFFIX, INST_SINGLE,
                                 mep_subtype);

    /** To make room for more port profile bits, the Tx rate field was split */
    if (dnx_data_oam.oamp.modified_mep_db_structure_get(unit) == 1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_TX_RATE_PROFILE, INST_SINGLE,
                                     entry_values->exclusive.bfd_only.bfd_tx_rate >> 1);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_TX_RATE_PROFILE, INST_SINGLE,
                                     entry_values->exclusive.bfd_only.bfd_tx_rate);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ITMH_TC_DP_PROFILE, INST_SINGLE,
                                 entry_values->itmh_tc_dp_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, INST_SINGLE,
                                 entry_values->unified_port_access.port_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_YOUR_DISC_LSB, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_your_disc);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PE_PROFILE, INST_SINGLE,
                                 entry_values->mep_pe_profile);
    /** Write entry   */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function sets a single MEP DB entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oamp_entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [in] entry_values - struct containing the values to be
 *        written to the entry.
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
dnx_oam_oamp_mep_db_entry_add(
    int unit,
    const uint32 oamp_entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_oam_oamp_mep_db_verify(unit, oamp_entry_id));
    SHR_NULL_CHECK(entry_values, _SHR_E_PARAM, "entry_values");

    /** Is the requested format "LM/DM offloaded?"   */
    if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_LM_DM_OFFLOADED))
    {
        /** The 1/4 entry flag becomes the part 1 or part 2 flag   */
        if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_Q_ENTRY))
        {
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_lm_dm_offloaded_part_2_add(unit, oamp_entry_id, entry_values));
        }
        else
        {
                /** The "Mep type" field must be an endpoint   */
            switch (entry_values->mep_type)
            {
                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM:
                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_MPLSTP:
                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE:
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_lm_dm_offloaded_part_1_add(unit, oamp_entry_id, entry_values));
                    break;

                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_1HOP:
                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP:
                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS:
                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Offloaded BFD endpoints are not supported - no LM/DM.\n");
                    break;

                default:
                        /** Not an endpoint type   */
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Type %d is not an endpoint.  Only endpoints can be offloaded.\n",
                                 entry_values->mep_type);
            }
        }
    }
    else
    {
        /** Not LM/DM offloaded.  Is it a 1/4 entry?   */
        if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_Q_ENTRY))
        {
            /** Different for OAM and BFD   */
            switch (entry_values->mep_type)
            {
                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM:
                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_MPLSTP:
                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE:
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_oam_q_entry_add(unit, oamp_entry_id, entry_values));
                    break;

                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_1HOP:
                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP:
                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS:
                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE:
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_bfd_q_entry_add(unit, oamp_entry_id, entry_values));
                    break;

                default:
                    /** Not an endpoint type   */
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Type %d is not an endpoint.  Only endpoints can be 1/4 entry.\n",
                                 entry_values->mep_type);
            }
        }
        else
        {
            /** Self contained entry type   */
            switch (entry_values->mep_type)
            {
                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM:
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_eth_oam_add(unit, oamp_entry_id, entry_values));
                    break;

                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_MPLSTP:
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_mplstp_oam_add(unit, oamp_entry_id, entry_values));
                    break;

                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE:
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_pwe_oam_add(unit, oamp_entry_id, entry_values));
                    break;

                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_1HOP:
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_bfd_one_hop_add(unit, oamp_entry_id, entry_values));
                    break;

                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP:
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_bfd_multi_hop_add(unit, oamp_entry_id, entry_values));
                    break;

                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS:
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_bfd_mpls_add(unit, oamp_entry_id, entry_values));
                    break;

                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE:
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_bfd_pwe_add(unit, oamp_entry_id, entry_values));
                    break;
                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374:
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_rfc_6374_add(unit, oamp_entry_id, entry_values));
                    break;

                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_DB:
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_lm_db_add(unit, oamp_entry_id, entry_values));
                    break;

                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_STAT:
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_lm_stat_add(unit, oamp_entry_id, entry_values));
                    break;

                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT:
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_dm_stat_two_way_add(unit, oamp_entry_id, entry_values));
                    break;

                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_EXT_DATA_HDR:
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_extra_data_header_add(unit, oamp_entry_id, entry_values));
                    break;

                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_EXT_DATA_PLD:
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_extra_data_payload_add(unit, oamp_entry_id, entry_values));
                    break;

                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT_ONE_WAY:
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_dm_stat_one_way_add(unit, oamp_entry_id, entry_values));
                    break;

                default:
                    /** Not a supported entry type   */
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Type %d is not supported.\n", entry_values->mep_type);
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads a single ethernet OAM CCM
 *        self-contained endpoint entry from the OAMP MEP DB
 *        table
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [out] entry_values - struct into which the fields read
 *        from the MEP DB entry will be written
 * \param [in] entry_handle_id - handle to the entry, which will
 *        be used to access the different fields.
 *
 * \return
 *   * shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mep_db_eth_oam_get(
    int unit,
    const uint32 entry_id,
    dnx_oam_oamp_mep_db_args_t * entry_values,
    uint32 entry_handle_id)
{
    uint32 value;
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    entry_values->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM;

    /** Get field values   */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_CCM_INTERVAL, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_ccm_interval = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ITMH_TC_DP_PROFILE, INST_SINGLE, &value));
    entry_values->itmh_tc_dp_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MDL, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_mdl = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MAID, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_maid = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ICC_MAP_INDEX, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_icc_index = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MEP_ID, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_mep_id = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RDI_FROM_SCANNER, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_RDI_FROM_SCANNER);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RDI_FROM_PACKET, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_RDI_FROM_PACKET);
    if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_IS_UPMEP))
    {
        /** For up-MEP, there is a reason for a "port core" field */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_PP_PORT, INST_SINGLE, &value));
        entry_values->unified_port_access.ccm_eth_up_mep_port.pp_port = value;
        if (dnx_data_oam.oamp.core_nof_bits_get(unit) > 0)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_PORT_CORE, INST_SINGLE, &value));
            entry_values->unified_port_access.ccm_eth_up_mep_port.port_core = value;
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, INST_SINGLE, &value));
        entry_values->unified_port_access.port_profile = value;

        if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_EGRESS_INJECTION))
        {
            /*
             * Mapping of VSI and OUT_LIF is different on inter-op mode Part of VSI in interop mode is stored in Gen
             * Mem
             */
            if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_VSI_JR1, INST_SINGLE, &value));
                entry_values->exclusive.oam_only.vsi = value;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF_JR1, INST_SINGLE, &value));
                entry_values->fec_id_or_glob_out_lif = value;

            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, &value));
                entry_values->exclusive.oam_only.vsi = value;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, &value));
                entry_values->fec_id_or_glob_out_lif = value;

            }
        }
    }

    /*
     * In inter-op mode Counter should be stamped in OAM-TS Header.
     * Therefore, counter fields should not be set for egress injection format
     * in inter-op mode.
     * Also, up-MEP server entry format has no counter fields.
     */
    if (!UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_UP_MEP_SERVER) &&
        !(UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_EGRESS_INJECTION) &&
          (system_headers_mode != DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_COUNTER_BASE, INST_SINGLE, &value));
        entry_values->counter_ptr = value;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_COUNTER_INTERFACE, INST_SINGLE, &value));
        entry_values->counter_interface = value;
    }
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_INNER_TPID_INDEX, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_inner_tpid_index = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_INNER_VID, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_inner_vid = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_INNER_DEI_PCP, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_inner_dei_pcp = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_OUTER_TPID_INDEX, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_outer_tpid_index = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_OUTER_VID, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_outer_vid = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_OUTER_DEI_PCP, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_outer_dei_pcp = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_NOF_VLAN_TAGS, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_nof_vlan_tags = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_UP_PTCH_OPAQUE_PT_ATTR_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_prt_qualifier_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PORT_STATUS_TLV_EN, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_PORT_TLV_EN);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PORT_STATUS_TLV_VAL, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_PORT_TLV_VAL);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_INTERFACE_STATUS_TLV_CODE, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_interface_status_tlv_code = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE, INST_SINGLE, &value));
    entry_values->mep_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_LMM_DA_LSB_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_lmm_da_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MEP_PE_PROFILE, INST_SINGLE, &value));
    entry_values->mep_pe_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SA_MAC_LSB, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_sa_mac_lsb = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SA_GEN_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_sa_mac_msb_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_EXTRA_DATA_PTR, INST_SINGLE, &value));
    entry_values->extra_data_ptr = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FLEX_LM_DM_PTR, INST_SINGLE, &value));
    entry_values->flex_lm_dm_ptr = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_DA_UC_MC_TYPE, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_MC_UC_SEL);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_CRPS_CORE_SELECT, INST_SINGLE, &value));
    entry_values->crps_core_select = value;
    if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_UP_MEP_SERVER))
    {
        /** Field mapped to "reserved" relevant only to UP-MEP in server mode */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_SERVER_DESTINATION, INST_SINGLE, &value));
        entry_values->exclusive.oam_only.server_destination = value;
    }
    /** This is not a real flag */
    UTILEX_SET_BIT(entry_values->flags, 0, OAMP_MEP_UP_MEP_SERVER);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [out] entry_values - struct into which the fields read
 *        from the MEP DB entry will be written
 * \param [in] entry_handle_id - handle to the entry, which will
 *        be used to access the different fields.
 *
 * \return
 *   * shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mep_db_mplstp_oam_get(
    int unit,
    const uint32 entry_id,
    dnx_oam_oamp_mep_db_args_t * entry_values,
    uint32 entry_handle_id)
{
    uint32 value;

    SHR_FUNC_INIT_VARS(unit);

    entry_values->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_MPLSTP;

    /** Get field values   */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_CCM_INTERVAL, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_ccm_interval = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ITMH_TC_DP_PROFILE, INST_SINGLE, &value));
    entry_values->itmh_tc_dp_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, INST_SINGLE, &value));
    entry_values->dest_sys_port_agr = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MDL, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_mdl = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MAID, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_maid = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ICC_MAP_INDEX, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_icc_index = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MEP_ID, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_mep_id = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RDI_FROM_SCANNER, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_RDI_FROM_SCANNER);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RDI_FROM_PACKET, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_RDI_FROM_PACKET);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF, INST_SINGLE,
                     &entry_values->fec_id_or_glob_out_lif));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, INST_SINGLE, &entry_values->mpls_pwe.label));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MPLS_PUSH_PROFILE, INST_SINGLE, &value));
    entry_values->mpls_pwe.push_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PORT_STATUS_TLV_EN, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_PORT_TLV_EN);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PORT_STATUS_TLV_VAL, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_PORT_TLV_VAL);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_INTERFACE_STATUS_TLV_CODE, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_interface_status_tlv_code = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE, INST_SINGLE, &value));
    entry_values->mep_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_COUNTER_BASE, INST_SINGLE, &value));
    entry_values->counter_ptr = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_COUNTER_INTERFACE, INST_SINGLE, &value));
    entry_values->counter_interface = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MEP_PE_PROFILE, INST_SINGLE, &value));
    entry_values->mep_pe_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_LOCAL_PORT_TO_OAMP_PE, INST_SINGLE, &value));
    entry_values->unified_port_access.port_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_EXTRA_DATA_PTR, INST_SINGLE, &value));
    entry_values->extra_data_ptr = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FLEX_LM_DM_PTR, INST_SINGLE, &value));
    entry_values->flex_lm_dm_ptr = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_CRPS_CORE_SELECT, INST_SINGLE, &value));
    entry_values->crps_core_select = value;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads a single OAM CCM over PWE
 *        self-contained endpoint entry from the OAMP MEP DB table
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [out] entry_values - struct into which the fields read
 *        from the MEP DB entry will be written
 * \param [in] entry_handle_id - handle to the entry, which will
 *        be used to access the different fields.
 *
 * \return
 *   * shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mep_db_pwe_oam_get(
    int unit,
    const uint32 entry_id,
    dnx_oam_oamp_mep_db_args_t * entry_values,
    uint32 entry_handle_id)
{
    uint32 value;

    SHR_FUNC_INIT_VARS(unit);

    entry_values->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE;

    /** Get field values   */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_CCM_INTERVAL, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_ccm_interval = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ITMH_TC_DP_PROFILE, INST_SINGLE, &value));
    entry_values->itmh_tc_dp_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, INST_SINGLE, &value));
    entry_values->dest_sys_port_agr = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MDL, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_mdl = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MAID, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_maid = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ICC_MAP_INDEX, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_icc_index = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MEP_ID, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_mep_id = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SIGNAL_DEGRADATION, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.tx_signal = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RDI_FROM_SCANNER, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_RDI_FROM_SCANNER);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RDI_FROM_PACKET, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_RDI_FROM_PACKET);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF, INST_SINGLE,
                     &entry_values->fec_id_or_glob_out_lif));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, INST_SINGLE, &entry_values->mpls_pwe.label));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MPLS_PUSH_PROFILE, INST_SINGLE, &value));
    entry_values->mpls_pwe.push_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PORT_STATUS_TLV_EN, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_PORT_TLV_EN);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PORT_STATUS_TLV_VAL, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_PORT_TLV_VAL);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_INTERFACE_STATUS_TLV_CODE, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_interface_status_tlv_code = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE, INST_SINGLE, &value));
    entry_values->mep_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_COUNTER_BASE, INST_SINGLE, &value));
    entry_values->counter_ptr = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_COUNTER_INTERFACE, INST_SINGLE, &value));
    entry_values->counter_interface = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MEP_PE_PROFILE, INST_SINGLE, &value));
    entry_values->mep_pe_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_LOCAL_PORT_TO_OAMP_PE, INST_SINGLE, &value));
    entry_values->unified_port_access.port_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_EXTRA_DATA_PTR, INST_SINGLE, &value));
    entry_values->extra_data_ptr = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FLEX_LM_DM_PTR, INST_SINGLE, &value));
    entry_values->flex_lm_dm_ptr = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_CRPS_CORE_SELECT, INST_SINGLE, &value));
    entry_values->crps_core_select = value;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads a single BFD single-hop
 *        self-contained endpoint entry from the OAMP MEP DB table
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [out] entry_values - struct into which the fields read
 *        from the MEP DB entry will be written
 * \param [in] entry_handle_id - handle to the entry, which will
 *        be used to access the different fields.
 *
 * \return
 *   * shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mep_db_bfd_one_hop_get(
    int unit,
    const uint32 entry_id,
    dnx_oam_oamp_mep_db_args_t * entry_values,
    uint32 entry_handle_id)
{
    uint32 value;

    SHR_FUNC_INIT_VARS(unit);

    entry_values->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_1HOP;

    /** Get field values   */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_TX_RATE_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_tx_rate = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ITMH_TC_DP_PROFILE, INST_SINGLE, &value));
    entry_values->itmh_tc_dp_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, INST_SINGLE, &value));
    entry_values->unified_port_access.port_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_YOUR_DISC, INST_SINGLE,
                                                        &entry_values->exclusive.bfd_only.bfd_your_disc));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_SRC_IP_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_src_ip_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_DETECT_MULT, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_detect_mult = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MIN_TX_INTERVAL_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_min_tx_interval_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MIN_RX_INTERVAL_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_min_rx_interval_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF, INST_SINGLE,
                     &entry_values->fec_id_or_glob_out_lif));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_DIAG_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_diag_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_TUNNEL_MPLS_ENABLE, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_BFD_TUNNEL_MPLS_ENABLE);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_FLAGS_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_flags_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_STATE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_sta = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MEP_PE_PROFILE, INST_SINGLE, &value));
    entry_values->mep_pe_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE, INST_SINGLE, &value));
    entry_values->mep_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_IPV4_DIP, INST_SINGLE,
                                                        &entry_values->exclusive.bfd_only.bfd_ipv4_dip));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_IP_SUBNET_LEN, INST_SINGLE, &value));
    entry_values->ip_subnet_len = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_EXTRA_DATA_PTR, INST_SINGLE, &value));
    entry_values->extra_data_ptr = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FLEX_LM_DM_PTR, INST_SINGLE, &value));
    entry_values->flex_lm_dm_ptr = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MICRO_BFD, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_MICRO_BFD);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_CRPS_CORE_SELECT, INST_SINGLE, &value));
    entry_values->crps_core_select = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SET_DISCR_MSB, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_SET_DISCR_MSB);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads a single BFD multi-hop
 *        self-contained endpoint entry from the OAMP MEP DB table
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [out] entry_values - struct into which the fields read
 *        from the MEP DB entry will be written
 * \param [in] entry_handle_id - handle to the entry, which will
 *        be used to access the different fields.
 *
 * \return
 *   * shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mep_db_bfd_multi_hop_get(
    int unit,
    const uint32 entry_id,
    dnx_oam_oamp_mep_db_args_t * entry_values,
    uint32 entry_handle_id)
{
    uint32 value;

    SHR_FUNC_INIT_VARS(unit);

    entry_values->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP;

    /** Get field values   */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_TX_RATE_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_tx_rate = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ITMH_TC_DP_PROFILE, INST_SINGLE, &value));
    entry_values->itmh_tc_dp_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, INST_SINGLE, &value));
    entry_values->unified_port_access.port_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_YOUR_DISC, INST_SINGLE,
                                                        &entry_values->exclusive.bfd_only.bfd_your_disc));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_IPV4_DIP, INST_SINGLE,
                                                        &entry_values->exclusive.bfd_only.bfd_ipv4_dip));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_SRC_IP_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_src_ip_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_IP_TTL_TOS_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_ip_ttl_tos_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_DETECT_MULT, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_detect_mult = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MIN_TX_INTERVAL_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_min_tx_interval_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MIN_RX_INTERVAL_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_min_rx_interval_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF, INST_SINGLE,
                     &entry_values->fec_id_or_glob_out_lif));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_DIAG_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_diag_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_FLAGS_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_flags_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_STATE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_sta = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE, INST_SINGLE, &value));
    entry_values->mep_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MEP_PE_PROFILE, INST_SINGLE, &value));
    entry_values->mep_pe_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_EXTRA_DATA_PTR, INST_SINGLE, &value));
    entry_values->extra_data_ptr = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FLEX_LM_DM_PTR, INST_SINGLE, &value));
    entry_values->flex_lm_dm_ptr = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_CRPS_CORE_SELECT, INST_SINGLE, &value));
    entry_values->crps_core_select = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SET_DISCR_MSB, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_SET_DISCR_MSB);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SET_SEAMLESS_PORTS, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_SEAMLESS_BFD);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads a single BFD over MPLS
 *        self-contained endpoint entry from the OAMP MEP DB table
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [out] entry_values - struct into which the fields read
 *        from the MEP DB entry will be written
 * \param [in] entry_handle_id - handle to the entry, which will
 *        be used to access the different fields.
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
dnx_oam_oamp_mep_db_bfd_mpls_get(
    int unit,
    const uint32 entry_id,
    dnx_oam_oamp_mep_db_args_t * entry_values,
    uint32 entry_handle_id)
{
    uint32 value;

    SHR_FUNC_INIT_VARS(unit);

    entry_values->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS;

    /** Get field values   */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_TX_RATE_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_tx_rate = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ITMH_TC_DP_PROFILE, INST_SINGLE, &value));
    entry_values->itmh_tc_dp_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, INST_SINGLE, &value));
    entry_values->unified_port_access.port_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_YOUR_DISC, INST_SINGLE,
                                                        &entry_values->exclusive.bfd_only.bfd_your_disc));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_SRC_IP_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_src_ip_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_DETECT_MULT, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_detect_mult = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MIN_TX_INTERVAL_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_min_tx_interval_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MIN_RX_INTERVAL_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_min_rx_interval_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF, INST_SINGLE,
                     &entry_values->fec_id_or_glob_out_lif));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_DIAG_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_diag_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, INST_SINGLE,
                                                        &entry_values->mpls_pwe.label));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MPLS_PUSH_PROFILE, INST_SINGLE, &value));
    entry_values->mpls_pwe.push_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_IP_TTL_TOS_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_ip_ttl_tos_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_FLAGS_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_flags_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_STATE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_sta = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MEP_PE_PROFILE, INST_SINGLE, &value));
    entry_values->mep_pe_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE, INST_SINGLE, &value));
    entry_values->mep_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_EXTRA_DATA_PTR, INST_SINGLE, &value));
    entry_values->extra_data_ptr = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FLEX_LM_DM_PTR, INST_SINGLE, &value));
    entry_values->flex_lm_dm_ptr = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_CRPS_CORE_SELECT, INST_SINGLE, &value));
    entry_values->crps_core_select = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SET_DISCR_MSB, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_SET_DISCR_MSB);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SET_SEAMLESS_PORTS, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_SEAMLESS_BFD);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads a single BFD over PWE
 *        self-contained endpoint entry from the OAMP MEP DB table
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [out] entry_values - struct into which the fields read
 *        from the MEP DB entry will be written
 * \param [in] entry_handle_id - handle to the entry, which will
 *        be used to access the different fields.
 *
 * \return
 *   * shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mep_db_bfd_pwe_get(
    int unit,
    const uint32 entry_id,
    dnx_oam_oamp_mep_db_args_t * entry_values,
    uint32 entry_handle_id)
{
    uint32 value;

    SHR_FUNC_INIT_VARS(unit);

    entry_values->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE;

    /** Get field values   */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_TX_RATE_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_tx_rate = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ITMH_TC_DP_PROFILE, INST_SINGLE, &value));
    entry_values->itmh_tc_dp_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, INST_SINGLE, &value));
    entry_values->dest_sys_port_agr = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_YOUR_DISC, INST_SINGLE,
                                                        &entry_values->exclusive.bfd_only.bfd_your_disc));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_DETECT_MULT, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_detect_mult = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MIN_TX_INTERVAL_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_min_tx_interval_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MIN_RX_INTERVAL_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_min_rx_interval_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF, INST_SINGLE,
                     &entry_values->fec_id_or_glob_out_lif));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_DIAG_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_diag_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, INST_SINGLE,
                                                        &entry_values->mpls_pwe.label));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MPLS_PUSH_PROFILE, INST_SINGLE, &value));
    entry_values->mpls_pwe.push_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_FLAGS_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_flags_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_STATE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_sta = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_ACH, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_ACH);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_GAL, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_GAL);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ROUTER_ALERT, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_ROUTER_ALERT);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MEP_PE_PROFILE, INST_SINGLE, &value));
    entry_values->mep_pe_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE, INST_SINGLE, &value));
    entry_values->mep_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ACH_SEL, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_ach_sel = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, INST_SINGLE, &value));
    entry_values->unified_port_access.port_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_EXTRA_DATA_PTR, INST_SINGLE, &value));
    entry_values->extra_data_ptr = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FLEX_LM_DM_PTR, INST_SINGLE, &value));
    entry_values->flex_lm_dm_ptr = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_CRPS_CORE_SELECT, INST_SINGLE, &value));
    entry_values->crps_core_select = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SET_DISCR_MSB, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_SET_DISCR_MSB);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads a single RFC 6374
 *        self-contained endpoint entry from the OAMP MEP DB table
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table
  * \param [out] entry_values - struct into which the fields read
 *        from the MEP DB entry will be written
 * \param [in] entry_handle_id - handle to the entry, which will
 *        be used to access the different fields.
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
dnx_oam_oamp_mep_db_rfc_6374_get(
    int unit,
    const uint32 entry_id,
    dnx_oam_oamp_mep_db_args_t * entry_values,
    uint32 entry_handle_id)
{
    uint32 value, flag;

    SHR_FUNC_INIT_VARS(unit);

    entry_values->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374;

    /** Get field values   */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_COUNTER_BASE, INST_SINGLE, &value));
    entry_values->counter_ptr = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_COUNTER_INTERFACE, INST_SINGLE, &value));
    entry_values->counter_interface = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SESSION_IDENTIFIER, INST_SINGLE, &value));
    entry_values->exclusive.rfc_6374_only.session_id = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_DM_T_FLAG, INST_SINGLE, &value));
    entry_values->exclusive.rfc_6374_only.dm_t_flag = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_DM_DS_FIELD, INST_SINGLE, &value));
    entry_values->exclusive.rfc_6374_only.dm_ds = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_TIMESTAMP_FORMAT, INST_SINGLE, &value));
    entry_values->exclusive.rfc_6374_only.timestamp_format = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_LM_CW_CHOOSE, INST_SINGLE, &value));
    entry_values->exclusive.rfc_6374_only.lm_cw_choose = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_LAST_ENTRY, INST_SINGLE, &flag));
    UTILEX_SET_BIT(entry_values->flags, flag, OAMP_MEP_LAST_ENTRY);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads an LM_DB type entry to the
 *        OAMP MEP DB.  This is an optional extension of a
 *        self-contained OAM endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [out] entry_values - struct into which the fields read
 *        from the MEP DB entry will be written
 * \param [in] entry_handle_id - handle to the entry, which will
 *        be used to access the different fields.
 *
 * \return
 *   * shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mep_db_lm_db_get(
    int unit,
    const uint32 entry_id,
    dnx_oam_oamp_mep_db_args_t * entry_values,
    uint32 entry_handle_id)
{
    uint32 flag;

    SHR_FUNC_INIT_VARS(unit);

    entry_values->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_DB;

    /** Get field values   */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_LM_MY_TX, INST_SINGLE,
                                                        &entry_values->exclusive.lm_dm.lm_my_tx));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_LM_MY_RX, INST_SINGLE,
                                                        &entry_values->exclusive.lm_dm.lm_my_rx));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_LM_PEER_TX, INST_SINGLE,
                                                        &entry_values->exclusive.lm_dm.lm_peer_tx));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_LM_PEER_RX, INST_SINGLE,
                                                        &entry_values->exclusive.lm_dm.lm_peer_rx));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_LAST_ENTRY, INST_SINGLE, &flag));
    UTILEX_SET_BIT(entry_values->flags, flag, OAMP_MEP_LAST_ENTRY);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_LM_CNTRS_VALID, INST_SINGLE, &flag));
    UTILEX_SET_BIT(entry_values->flags, flag, OAMP_MEP_LM_CNTRS_VALID);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MEASURE_NEXT_RECEIVED_SLR, INST_SINGLE, &flag));
    UTILEX_SET_BIT(entry_values->flags, flag, OAMP_MEP_MEASURE_NEXT_RECEIVED_SLR);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads an LM_STAT type entry to the
 *        OAMP MEP DB.  This is an optional extension of a
 *        self-contained OAM endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [out] entry_values - struct into which the fields read
 *        from the MEP DB entry will be written
 * \param [in] entry_handle_id - handle to the entry, which will
 *        be used to access the different fields.
 *
 * \return
 *   * shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mep_db_lm_stat_get(
    int unit,
    const uint32 entry_id,
    dnx_oam_oamp_mep_db_args_t * entry_values,
    uint32 entry_handle_id)
{
    uint32 value;

    SHR_FUNC_INIT_VARS(unit);

    entry_values->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_STAT;

    /** Get field values   */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_LAST_LM_FAR, INST_SINGLE, &value));
    entry_values->exclusive.lm_dm.last_lm_far = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_LAST_LM_NEAR, INST_SINGLE, &value));
    entry_values->exclusive.lm_dm.last_lm_near = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_ACC_LM_FAR, INST_SINGLE,
                                                        &entry_values->exclusive.lm_dm.acc_lm_far));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_ACC_LM_NEAR, INST_SINGLE,
                                                        &entry_values->exclusive.lm_dm.acc_lm_near));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MAX_LM_FAR, INST_SINGLE, &value));
    entry_values->exclusive.lm_dm.max_lm_far = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MAX_LM_NEAR, INST_SINGLE, &value));
    entry_values->exclusive.lm_dm.max_lm_near = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_LAST_ENTRY, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_LAST_ENTRY);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads a one-way DM_STAT type entry to
 *        the OAMP MEP DB.  This is an optional extension of a
 *        self-contained OAM endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [out] entry_values - struct into which the fields read
 *        from the MEP DB entry will be written
 * \param [in] entry_handle_id - handle to the entry, which will
 *        be used to access the different fields.
 *
 * \return
 *   * None
 *
 * \remark
 *   * shr_error_e
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mep_db_dm_stat_one_way_get(
    int unit,
    const uint32 entry_id,
    dnx_oam_oamp_mep_db_args_t * entry_values,
    uint32 entry_handle_id)
{
    uint8 *value_array8 = NULL;
    uint32 flag;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(value_array8, DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES, "Array for over 32-bit dbal fields",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    entry_values->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT_ONE_WAY;

    /** Get field values   */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get(unit, entry_handle_id,
                                                           DBAL_FIELD_LAST_DELAY_ONE_WAY, INST_SINGLE, value_array8));
    sal_memcpy(entry_values->exclusive.lm_dm.last_delay_one_way, value_array8, ONE_WAY_DELAY_SIZE);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get(unit, entry_handle_id,
                                                           DBAL_FIELD_MAX_DELAY_ONE_WAY, INST_SINGLE, value_array8));
    sal_memcpy(entry_values->exclusive.lm_dm.max_delay_one_way, value_array8, ONE_WAY_DELAY_SIZE);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get(unit, entry_handle_id,
                                                           DBAL_FIELD_MIN_DELAY_ONE_WAY, INST_SINGLE, value_array8));
    sal_memcpy(entry_values->exclusive.lm_dm.min_delay_one_way, value_array8, ONE_WAY_DELAY_SIZE);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get(unit, entry_handle_id,
                                                           DBAL_FIELD_LAST_DELAY_ONE_WAY_NEAR_END, INST_SINGLE,
                                                           value_array8));
    sal_memcpy(entry_values->exclusive.lm_dm.last_delay_one_way_near_end, value_array8, ONE_WAY_DELAY_SIZE);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get(unit, entry_handle_id,
                                                           DBAL_FIELD_MAX_DELAY_ONE_WAY_NEAR_END, INST_SINGLE,
                                                           value_array8));
    sal_memcpy(entry_values->exclusive.lm_dm.max_delay_one_way_near_end, value_array8, ONE_WAY_DELAY_SIZE);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get(unit, entry_handle_id,
                                                           DBAL_FIELD_MIN_DELAY_ONE_WAY_NEAR_END, INST_SINGLE,
                                                           value_array8));
    sal_memcpy(entry_values->exclusive.lm_dm.min_delay_one_way_near_end, value_array8, ONE_WAY_DELAY_SIZE);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_LAST_ENTRY, INST_SINGLE, &flag));
    UTILEX_SET_BIT(entry_values->flags, flag, OAMP_MEP_LAST_ENTRY);

exit:
    SHR_FREE(value_array8);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads a two-way DM_STAT type entry to
 *        the OAMP MEP DB.  This is an optional extension of a
 *        self-contained OAM endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [out] entry_values - struct into which the fields read
 *        from the MEP DB entry will be written
 * \param [in] entry_handle_id - handle to the entry, which will
 *        be used to access the different fields.
 *
 * \return
 *   * shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mep_db_dm_stat_two_way_get(
    int unit,
    const uint32 entry_id,
    dnx_oam_oamp_mep_db_args_t * entry_values,
    uint32 entry_handle_id)
{
    uint8 *value_array8 = NULL;
    uint32 flag;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(value_array8, DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES, "Array for over 32-bit dbal fields",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    entry_values->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT;

    /** Get field values   */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get(unit, entry_handle_id,
                                                           DBAL_FIELD_LAST_DELAY, INST_SINGLE, value_array8));
    sal_memcpy(entry_values->exclusive.lm_dm.last_delay, value_array8, TWO_WAY_DELAY_SIZE);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get(unit, entry_handle_id,
                                                           DBAL_FIELD_MAX_DELAY, INST_SINGLE, value_array8));
    sal_memcpy(entry_values->exclusive.lm_dm.max_delay, value_array8, TWO_WAY_DELAY_SIZE);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get(unit, entry_handle_id,
                                                           DBAL_FIELD_MIN_DELAY, INST_SINGLE, value_array8));
    sal_memcpy(entry_values->exclusive.lm_dm.min_delay, value_array8, TWO_WAY_DELAY_SIZE);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_LAST_ENTRY, INST_SINGLE, &flag));
    UTILEX_SET_BIT(entry_values->flags, flag, OAMP_MEP_LAST_ENTRY);

exit:
    SHR_FREE(value_array8);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads an extra data header entry from
 *        the OAMP MEP DB.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [out] entry_values - struct into which the fields read
 *        from the MEP DB entry will be written
 * \param [in] entry_handle_id - handle to the entry, which will
 *        be used to access the different fields.
 *
 * \return
 *   * shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mep_db_extra_data_header_get(
    int unit,
    const uint32 entry_id,
    dnx_oam_oamp_mep_db_args_t * entry_values,
    uint32 entry_handle_id)
{
    uint8 *value_array8 = NULL;
    uint32 value;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(value_array8, DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES, "Array for over 32-bit dbal fields",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    entry_values->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_EXT_DATA_HDR;

    /** Get field values   */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_EXT_DATA_LENGTH, INST_SINGLE, &value));
    entry_values->exclusive.extra_data.extra_data_len = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_OPCODES_TO_PREPEND, INST_SINGLE, &value));
    entry_values->exclusive.extra_data.opcode_bmp = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_CHECK_CRC_VALUE_1, INST_SINGLE, &value));
    entry_values->exclusive.extra_data.crc_val1 = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_CHECK_CRC_VALUE_2, INST_SINGLE, &value));
    entry_values->exclusive.extra_data.crc_val2 = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get(unit, entry_handle_id,
                                                           DBAL_FIELD_EXTRA_DATA_SEGMENT, INST_SINGLE, value_array8));
    sal_memcpy(entry_values->exclusive.extra_data.data_segment, value_array8, DATA_SEG_LEN);

exit:
    SHR_FREE(value_array8);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 *        the OAMP MEP DB.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [out] entry_values - struct into which the fields read
 *        from the MEP DB entry will be written
 * \param [in] entry_handle_id - handle to the entry, which will
 *        be used to access the different fields.
 *
 * \return
 *   * shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mep_db_extra_data_payload_get(
    int unit,
    const uint32 entry_id,
    dnx_oam_oamp_mep_db_args_t * entry_values,
    uint32 entry_handle_id)
{
    uint8 *value_array8 = NULL;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(value_array8, DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES, "Array for over 32-bit dbal fields",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    entry_values->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_EXT_DATA_PLD;

    /** Get field values   */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get(unit, entry_handle_id,
                                                           DBAL_FIELD_EXTRA_DATA_SEGMENT, INST_SINGLE, value_array8));
    sal_memcpy(entry_values->exclusive.extra_data.data_segment, value_array8, DATA_SEG_LEN);

exit:
    SHR_FREE(value_array8);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads a type 1 LM/DM offloaded entry
 *        from the OAMP MEP DB table.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [out] entry_values - struct into which the fields read
 *        from the MEP DB entry will be written
 * \param [in] entry_handle_id - handle to the entry, which will
 *        be used to access the different fields.
 *
 * \return
 *   * shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mep_db_lm_dm_offloaded_part_1_get(
    int unit,
    const uint32 entry_id,
    dnx_oam_oamp_mep_db_args_t * entry_values,
    uint32 entry_handle_id)
{
    uint32 value;
    SHR_FUNC_INIT_VARS(unit);

    /** Get field values   */

    /** Get MEP type */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_OFFLOADED_MEP_TYPE_SUFFIX, INST_SINGLE, &value));
    switch (value)
    {
        case DBAL_ENUM_FVAL_OFFLOADED_MEP_TYPE_SUFFIX_CCM_ETHERNET:
            entry_values->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM;
            break;

        case DBAL_ENUM_FVAL_OFFLOADED_MEP_TYPE_SUFFIX_MPLSTP:
            entry_values->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_MPLSTP;
            break;

        case DBAL_ENUM_FVAL_OFFLOADED_MEP_TYPE_SUFFIX_PWE:
            entry_values->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE;
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Internal error.  Suffix does not match result type.");
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_LM_ENABLE, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_LM_ENABLE);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_DM_ENABLE, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_DM_ENABLE);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_LM_STAT_ENABLE, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_LM_STAT_ENABLE);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_DM_STAT_ENABLE, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_DM_STAT_ENABLE);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RDI_FROM_SCANNER, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_RDI_FROM_SCANNER);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RDI_FROM_PACKET, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_RDI_FROM_PACKET);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_LM_MY_TX, INST_SINGLE,
                                                        &entry_values->exclusive.lm_dm.lm_my_tx));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_LM_MY_RX, INST_SINGLE,
                                                        &entry_values->exclusive.lm_dm.lm_my_rx));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_LM_PEER_TX, INST_SINGLE,
                                                        &entry_values->exclusive.lm_dm.lm_peer_tx));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_LM_PEER_RX, INST_SINGLE,
                                                        &entry_values->exclusive.lm_dm.lm_peer_rx));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_LM_CNTRS_VALID, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_LM_CNTRS_VALID);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_LAST_LM_FAR, INST_SINGLE, &value));
    entry_values->exclusive.lm_dm.last_lm_far = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_LAST_LM_NEAR, INST_SINGLE, &value));
    entry_values->exclusive.lm_dm.last_lm_near = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MAX_LM_FAR, INST_SINGLE, &value));
    entry_values->exclusive.lm_dm.max_lm_far = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_CRPS_CORE_SELECT, INST_SINGLE, &value));
    entry_values->crps_core_select = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MEP_PE_PROFILE, INST_SINGLE, &value));
    entry_values->mep_pe_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PART_2_PTR, INST_SINGLE, &value));
    entry_values->flex_lm_dm_ptr = value;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads a type 2 LM/DM offloaded entry
 *        from the OAMP MEP DB table.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [out] entry_values - struct into which the fields read
 *        from the MEP DB entry will be written
 * \param [in] entry_handle_id - handle to the entry, which will
 *        be used to access the different fields.
 *
 * \return
 *   * shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mep_db_lm_dm_offloaded_part_2_get(
    int unit,
    const uint32 entry_id,
    dnx_oam_oamp_mep_db_args_t * entry_values,
    uint32 entry_handle_id)
{
    uint8 *value_array8 = NULL;
    uint32 value;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(value_array8, DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES, "Array for over 32-bit dbal fields",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    entry_values->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_OFFLOADED_SECOND_ENTRY;

    /** Get field values   */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MAX_LM_NEAR, INST_SINGLE, &value));
    entry_values->exclusive.lm_dm.max_lm_near = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_ACC_LM_FAR, INST_SINGLE,
                                                        &entry_values->exclusive.lm_dm.acc_lm_far));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_ACC_LM_NEAR, INST_SINGLE,
                                                        &entry_values->exclusive.lm_dm.acc_lm_near));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get(unit, entry_handle_id,
                                                           DBAL_FIELD_LAST_DELAY, INST_SINGLE, value_array8));
    sal_memcpy(entry_values->exclusive.lm_dm.last_delay, value_array8, TWO_WAY_DELAY_SIZE);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get(unit, entry_handle_id,
                                                           DBAL_FIELD_MAX_DELAY, INST_SINGLE, value_array8));
    sal_memcpy(entry_values->exclusive.lm_dm.max_delay, value_array8, TWO_WAY_DELAY_SIZE);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get(unit, entry_handle_id,
                                                           DBAL_FIELD_MIN_DELAY, INST_SINGLE, value_array8));
    sal_memcpy(entry_values->exclusive.lm_dm.min_delay, value_array8, TWO_WAY_DELAY_SIZE);

exit:
    SHR_FREE(value_array8);
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads a 1/4 entry format OAM endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [out] entry_values - struct into which the fields read
 *        from the MEP DB entry will be written
 * \param [in] entry_handle_id - handle to the entry, which will
 *        be used to access the different fields.
 *
 * \return
 *   * shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mep_db_oam_q_entry_get(
    int unit,
    const uint32 entry_id,
    dnx_oam_oamp_mep_db_args_t * entry_values,
    uint32 entry_handle_id)
{
    uint32 value;
    SHR_FUNC_INIT_VARS(unit);

    /** Get field values   */

    /** Get MEP type */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SHORT_OAM_ENTRY_MEP_TYPE_SUFFIX, INST_SINGLE, &value));
    switch (value)
    {
        case DBAL_ENUM_FVAL_SHORT_OAM_ENTRY_MEP_TYPE_SUFFIX_CCM_ETHERNET:
            entry_values->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM;
            break;

        case DBAL_ENUM_FVAL_SHORT_OAM_ENTRY_MEP_TYPE_SUFFIX_MPLSTP:
            entry_values->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_MPLSTP;
            break;

        case DBAL_ENUM_FVAL_SHORT_OAM_ENTRY_MEP_TYPE_SUFFIX_PWE:
            entry_values->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE;
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Internal error.  Suffix does not match result type.");
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_CCM_INTERVAL, INST_SINGLE, &value));
    if (dnx_data_oam.oamp.modified_mep_db_structure_get(unit) == 1)
    {
        /** When the CCM interval is split between two places, do not change the LSB */
        entry_values->exclusive.oam_only.oam_ccm_interval =
            UTILEX_GET_BIT(entry_values->exclusive.oam_only.oam_ccm_interval,
                           1) | ((value << 1) & CCM_INT_BFD_RATE_MSB_MASK);
    }
    else
    {
        entry_values->exclusive.oam_only.oam_ccm_interval = value;
    }
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ITMH_TC_DP_PROFILE, INST_SINGLE, &value));
    entry_values->itmh_tc_dp_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ICC_MAP_INDEX, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_icc_index = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MEP_ID, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_mep_id = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RDI_FROM_SCANNER, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_RDI_FROM_SCANNER);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RDI_FROM_PACKET, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_RDI_FROM_PACKET);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE, INST_SINGLE, &value));
    entry_values->mep_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MEP_PE_PROFILE, INST_SINGLE, &value));
    entry_values->mep_pe_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_CRPS_CORE_SELECT, INST_SINGLE, &value));
    entry_values->crps_core_select = value;
    if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_IS_UPMEP))
    {
        /** Is this a CCM Ethernet up-MEP? */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_PP_PORT, INST_SINGLE, &value));
        entry_values->unified_port_access.ccm_eth_up_mep_port.pp_port = value;
        if (dnx_data_oam.oamp.core_nof_bits_get(unit) > 0)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_PORT_CORE, INST_SINGLE, &value));
            entry_values->unified_port_access.ccm_eth_up_mep_port.port_core = value;
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, INST_SINGLE, &value));
        entry_values->unified_port_access.port_profile = value;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads a 1/4 entry format BFD endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [out] entry_values - struct into which the fields read
 *        from the MEP DB entry will be written
 * \param [in] entry_handle_id - handle to the entry, which will
 *        be used to access the different fields.
 *
 * \return
 *   * shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mep_db_bfd_q_entry_get(
    int unit,
    const uint32 entry_id,
    dnx_oam_oamp_mep_db_args_t * entry_values,
    uint32 entry_handle_id)
{
    uint32 value;
    SHR_FUNC_INIT_VARS(unit);

    /** Get field values   */

    /** Get MEP type */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SHORT_BFD_ENTRY_MEP_TYPE_SUFFIX, INST_SINGLE, &value));
    switch (value)
    {
        case DBAL_ENUM_FVAL_SHORT_BFD_ENTRY_MEP_TYPE_SUFFIX_1_HOP:
            entry_values->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_1HOP;
            break;

        case DBAL_ENUM_FVAL_SHORT_BFD_ENTRY_MEP_TYPE_SUFFIX_M_HOP:
            entry_values->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP;
            break;

        case DBAL_ENUM_FVAL_SHORT_BFD_ENTRY_MEP_TYPE_SUFFIX_MPLS:
            entry_values->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS;
            break;

        case DBAL_ENUM_FVAL_SHORT_BFD_ENTRY_MEP_TYPE_SUFFIX_PWE:
            entry_values->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE;
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Internal error.  Suffix does not match result type.");
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_TX_RATE_PROFILE, INST_SINGLE, &value));
    if (dnx_data_oam.oamp.modified_mep_db_structure_get(unit) == 1)
    {
        /** When the Tx rate is split between two places, do not change the LSB */
        entry_values->exclusive.bfd_only.bfd_tx_rate =
            UTILEX_GET_BIT(entry_values->exclusive.bfd_only.bfd_tx_rate,
                           1) | ((value << 1) & CCM_INT_BFD_RATE_MSB_MASK);
    }
    else
    {
        entry_values->exclusive.bfd_only.bfd_tx_rate = value;
    }
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ITMH_TC_DP_PROFILE, INST_SINGLE, &value));
    entry_values->itmh_tc_dp_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, INST_SINGLE, &value));
    entry_values->unified_port_access.port_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_YOUR_DISC_LSB, INST_SINGLE,
                                                        &entry_values->exclusive.bfd_only.bfd_your_disc));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MEP_PE_PROFILE, INST_SINGLE, &value));
    entry_values->mep_pe_profile = value;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in oam_internal.h
 */
shr_error_e
dnx_oam_oamp_mep_db_get(
    int unit,
    const uint32 entry_id,
    dnx_oam_oamp_mep_db_args_t * entry_values)
{
    uint32 entry_handle_id;
    dbal_enum_value_result_type_oamp_mep_db_e resulting_format;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_oam_oamp_mep_db_verify(unit, entry_id));
    SHR_NULL_CHECK(entry_values, _SHR_E_PARAM, "entry_values");

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DB, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_ENTRY_ID, entry_id);

    /** Get all fields   */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** Read format   */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &resulting_format));

    entry_values->flags = 0;

    switch (resulting_format)
    {
        case DBAL_RESULT_TYPE_OAMP_MEP_DB_ETH_OAM_DOWN_MEP:
        case DBAL_RESULT_TYPE_OAMP_MEP_DB_ETH_OAM_DOWN_MEP_EGRESS_INJECTION:
        case DBAL_RESULT_TYPE_OAMP_MEP_DB_ETH_OAM_UP_MEP:
        case DBAL_RESULT_TYPE_OAMP_MEP_DB_ETH_OAM_UP_MEP_SERVER:
            UTILEX_SET_BIT(entry_values->flags, (DBAL_RESULT_TYPE_OAMP_MEP_DB_ETH_OAM_UP_MEP == resulting_format ||
                                                 DBAL_RESULT_TYPE_OAMP_MEP_DB_ETH_OAM_UP_MEP_SERVER ==
                                                 resulting_format), OAMP_MEP_IS_UPMEP);
            UTILEX_SET_BIT(entry_values->flags,
                           (DBAL_RESULT_TYPE_OAMP_MEP_DB_ETH_OAM_DOWN_MEP_EGRESS_INJECTION == resulting_format),
                           OAMP_MEP_EGRESS_INJECTION);
            UTILEX_SET_BIT(entry_values->flags,
                           (DBAL_RESULT_TYPE_OAMP_MEP_DB_ETH_OAM_UP_MEP_SERVER == resulting_format),
                           OAMP_MEP_UP_MEP_SERVER);
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_eth_oam_get(unit, entry_id, entry_values, entry_handle_id));
            break;

        case DBAL_RESULT_TYPE_OAMP_MEP_DB_Y1731_MPLSTP:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_mplstp_oam_get(unit, entry_id, entry_values, entry_handle_id));
            break;

        case DBAL_RESULT_TYPE_OAMP_MEP_DB_Y1731_PWE:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_pwe_oam_get(unit, entry_id, entry_values, entry_handle_id));
            break;

        case DBAL_RESULT_TYPE_OAMP_MEP_DB_BFD_IPV4_ONE_HOP:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_bfd_one_hop_get(unit, entry_id, entry_values, entry_handle_id));
            break;

        case DBAL_RESULT_TYPE_OAMP_MEP_DB_BFD_IPV4_MULTI_HOP:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_bfd_multi_hop_get(unit, entry_id, entry_values, entry_handle_id));
            break;

        case DBAL_RESULT_TYPE_OAMP_MEP_DB_BFD_MPLS:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_bfd_mpls_get(unit, entry_id, entry_values, entry_handle_id));
            break;

        case DBAL_RESULT_TYPE_OAMP_MEP_DB_BFD_PWE:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_bfd_pwe_get(unit, entry_id, entry_values, entry_handle_id));
            break;

        case DBAL_RESULT_TYPE_OAMP_MEP_DB_RFC_6374:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_rfc_6374_get(unit, entry_id, entry_values, entry_handle_id));
            break;

        case DBAL_RESULT_TYPE_OAMP_MEP_DB_LM_DB:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_lm_db_get(unit, entry_id, entry_values, entry_handle_id));
            break;

        case DBAL_RESULT_TYPE_OAMP_MEP_DB_LM_STAT:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_lm_stat_get(unit, entry_id, entry_values, entry_handle_id));
            break;

        case DBAL_RESULT_TYPE_OAMP_MEP_DB_DM_STAT_TWO_WAY:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_dm_stat_two_way_get(unit, entry_id, entry_values, entry_handle_id));
            break;

        case DBAL_RESULT_TYPE_OAMP_MEP_DB_EXTRA_DATA_HEADER:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_extra_data_header_get(unit, entry_id, entry_values, entry_handle_id));
            break;

        case DBAL_RESULT_TYPE_OAMP_MEP_DB_EXTRA_DATA_PAYLOAD:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_extra_data_payload_get(unit, entry_id, entry_values, entry_handle_id));
            break;

        case DBAL_RESULT_TYPE_OAMP_MEP_DB_DM_STAT_ONE_WAY:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_dm_stat_one_way_get(unit, entry_id, entry_values, entry_handle_id));
            break;

        case DBAL_RESULT_TYPE_OAMP_MEP_DB_LM_DM_OFFLOADED_PART_1:
            /**
             * An LM/DM offloaded entry must have an entry of this
             * type in the OAMP_MEP_DB table.  Its location corresponds
             * to the oam_id (which must be a multiple of 4 and above
             * the threshold)  Dynamic fields are here, while the
             * static fields are stored in at least 2 MDB entries.
             */
            UTILEX_SET_BIT(entry_values->flags, TRUE, OAMP_MEP_LM_DM_OFFLOADED);
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_lm_dm_offloaded_part_1_get
                            (unit, entry_id, entry_values, entry_handle_id));
            break;

        case DBAL_RESULT_TYPE_OAMP_MEP_DB_LM_DM_OFFLOADED_PART_2:
            /**
             * An LM/DM offloaded entry can have an entry of this type
             * if specified by the user.  The PART_1 entry points to it
             * with the PART_2_PTR field, and it can be placed in an
             * entry in any of the 12 banks.  It is used for more dynamic
             * fields./
             */
            UTILEX_SET_BIT(entry_values->flags, TRUE, OAMP_MEP_LM_DM_OFFLOADED);
            UTILEX_SET_BIT(entry_values->flags, TRUE, OAMP_MEP_Q_ENTRY);
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_lm_dm_offloaded_part_2_get
                            (unit, entry_id, entry_values, entry_handle_id));
            break;

        case DBAL_RESULT_TYPE_OAMP_MEP_DB_Q_ENTRY_OAM_FORMAT_DOWN_MEP:
        case DBAL_RESULT_TYPE_OAMP_MEP_DB_Q_ENTRY_OAM_FORMAT_UP_MEP:
            /**
             * Any 1/4 entry OAM endpoint must have a MEP DB entry
             * of this format occupying a quarter of the bits of the
             * full length of an entry, below the full entry threshold.
             * It contains the endpoint's dynamic fields, while the static
             * fields are in an entry in the MDB (with possible additional
             * MDB entries for extra data - 48b MAID, etc.)
             */

            UTILEX_SET_BIT(entry_values->flags, TRUE, OAMP_MEP_Q_ENTRY);
            UTILEX_SET_BIT(entry_values->flags,
                           (DBAL_RESULT_TYPE_OAMP_MEP_DB_Q_ENTRY_OAM_FORMAT_UP_MEP == resulting_format),
                           OAMP_MEP_IS_UPMEP);
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_oam_q_entry_get(unit, entry_id, entry_values, entry_handle_id));
            break;

        case DBAL_RESULT_TYPE_OAMP_MEP_DB_Q_ENTRY_BFD_FORMAT:
            /**
             * Any 1/4 entry BFD endpoint must have a MEP DB entry
             * of this format occupying a quarter of the bits of the
             * full length of an entry, below the full entry threshold.
             * It contains the endpoint's dynamic fields, while the static
             * fields are in an entry in the MDB.
             */

            UTILEX_SET_BIT(entry_values->flags, TRUE, OAMP_MEP_Q_ENTRY);
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_bfd_q_entry_get(unit, entry_id, entry_values, entry_handle_id));
            break;

        default:
                    /** Not a supported result type   */
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error.  Entry has an illegal result type %d\n", resulting_format);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in oam_internal.h
 */
shr_error_e
dnx_oam_oamp_mep_db_delete(
    int unit,
    const uint32 entry_id)
{
    uint32 entry_handle_id;
    int mep_db_threshold;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    mep_db_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_oam_oamp_mep_db_verify(unit, entry_id));

    /** Deallocate entry */
    SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_entry_dealloc(unit, entry_id));

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DB, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_ENTRY_ID, entry_id);

    /** Access the table */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

    /** In some cases, once is not enough - full entry only */
    if (entry_id >= mep_db_threshold)
    {
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function adds the first of two mandatory MDB
 *        entries for an LM/DM offloaded ethernet CCM entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oamp_entry_id - Physical location of this endpoint's
 *        entry in the OAMP MEP DB table, in quarter entries
 * \param [in] entry_values - struct containing the values to be
 *        written to the entry.
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
dnx_oam_oamp_mdb_lm_dm_eth_ccm_part_1_add(
    int unit,
    const uint32 oamp_entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values)
{
    uint32 entry_handle_id, flag, commit_flag = DBAL_COMMIT;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set update flag   */
    if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_UPDATE))
    {
        commit_flag = DBAL_COMMIT_UPDATE;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_STATIC_DATA_DB, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, oamp_entry_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ACCESS_INDEX, MDB_PART_1_ACCESS);

    /** Set result type   */
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_IS_UPMEP);

    /** Get table */
    if (flag == TRUE)
    {
        /** Endpoint is a CCM ethernet up-MEP */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_OAMP_MEP_STATIC_DATA_DB_MEP_DB_CCM_ETH_LM_DM_OFFLOAD_MDB_FORMAT_PART_1_UPMEP);

        /** This field(s) is the only difference between the two result types */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_PORT_TO_OAMP_PE, INST_SINGLE,
                                     (entry_values->unified_port_access.
                                      ccm_eth_up_mep_port.port_core << PORT_CORE_SHIFT) | entry_values->
                                     unified_port_access.ccm_eth_up_mep_port.pp_port);

    }
    else
    {
        /** Endpoint is a CCM ethernet down-MEP, MPLS-TP or PWE*/
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_OAMP_MEP_STATIC_DATA_DB_MEP_DB_CCM_ETH_LM_DM_OFFLOAD_MDB_FORMAT_PART_1_DOWNMEP);

        /** See above */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, INST_SINGLE,
                                     entry_values->unified_port_access.port_profile);
    }

    /** Set field values   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CCM_INTERVAL, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_ccm_interval);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ITMH_TC_DP_PROFILE, INST_SINGLE,
                                 entry_values->itmh_tc_dp_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDL, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_mdl);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAID, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_maid);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ICC_MAP_INDEX, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_icc_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_ID, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_mep_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UP_1_DOWN_0, INST_SINGLE, flag);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INNER_TPID_INDEX, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_inner_tpid_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INNER_VID, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_inner_vid);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INNER_DEI_PCP, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_inner_dei_pcp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_TPID_INDEX, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_outer_tpid_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_VID, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_outer_vid);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_DEI_PCP, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_outer_dei_pcp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_VLAN_TAGS, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_nof_vlan_tags);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UP_PTCH_OPAQUE_PT_ATTR_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_prt_qualifier_profile);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_PORT_TLV_EN);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_STATUS_TLV_EN, INST_SINGLE, flag);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_PORT_TLV_VAL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_STATUS_TLV_VAL, INST_SINGLE, flag);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INTERFACE_STATUS_TLV_CODE, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_interface_status_tlv_code);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE, INST_SINGLE, entry_values->mep_profile);

    /** Write entry   */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, commit_flag));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function adds the second of two mandatory MDB
 *        entries for an LM/DM offloaded ethernet CCM entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oamp_entry_id - Physical location of this endpoint's
 *        entry in the OAMP MEP DB table, in quarter entries
 * \param [in] entry_values - struct containing the values to be
 *        written to the entry.
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
dnx_oam_oamp_mdb_lm_dm_eth_ccm_part_2_add(
    int unit,
    const uint32 oamp_entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values)
{
    uint32 entry_handle_id, commit_flag = DBAL_COMMIT;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set update flag   */
    if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_UPDATE))
    {
        commit_flag = DBAL_COMMIT_UPDATE;
    }

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_STATIC_DATA_DB, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, oamp_entry_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ACCESS_INDEX, MDB_PART_2_ACCESS);

    /** Set result type   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_OAMP_MEP_STATIC_DATA_DB_MEP_DB_CCM_ETH_LM_DM_OFFLOAD_MDB_FORMAT_PART_2);

    /** Set field values   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_BASE, INST_SINGLE,
                                 entry_values->counter_ptr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_INTERFACE, INST_SINGLE,
                                 entry_values->counter_interface);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LMM_DA_LSB_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_lmm_da_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SA_GEN_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_sa_mac_msb_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SA_MAC_LSB, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_sa_mac_lsb);
    /** Ethernet OAM CCM packets are always multicast, unicast not supported */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DA_UC_MC_TYPE, INST_SINGLE,
                                 UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_MC_UC_SEL));

    /** Write entry   */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, commit_flag));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function adds the first of two mandatory MDB
 *        entries for an LM/DM offloaded OAM over MPLS-TP or PWE
 *        endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oamp_entry_id - Physical location of this endpoint's
 *        entry in the OAMP MEP DB table, in quarter entries
 * \param [in] entry_values - struct containing the values to be
 *        written to the entry.
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
dnx_oam_oamp_mdb_lm_dm_mplstp_pwe_part_1_add(
    int unit,
    const uint32 oamp_entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values)
{
    uint32 entry_handle_id, flag, commit_flag = DBAL_COMMIT;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set update flag   */
    if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_UPDATE))
    {
        commit_flag = DBAL_COMMIT_UPDATE;
    }

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_STATIC_DATA_DB, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, oamp_entry_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ACCESS_INDEX, MDB_PART_1_ACCESS);

    /** Set result type   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_OAMP_MEP_STATIC_DATA_DB_MEP_DB_MPLSTP_PWE_LM_DM_OFFLOAD_MDB_FORMAT_PART_1);

    /** Set field values   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CCM_INTERVAL, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_ccm_interval);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ITMH_TC_DP_PROFILE, INST_SINGLE,
                                 entry_values->itmh_tc_dp_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, INST_SINGLE,
                                 entry_values->dest_sys_port_agr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDL, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_mdl);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAID, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_maid);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ICC_MAP_INDEX, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_icc_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_ID, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_mep_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF, INST_SINGLE,
                                 entry_values->fec_id_or_glob_out_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, INST_SINGLE,
                                 entry_values->mpls_pwe.label);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_PUSH_PROFILE, INST_SINGLE,
                                 entry_values->mpls_pwe.push_profile);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_PORT_TLV_EN);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_STATUS_TLV_EN, INST_SINGLE, flag);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_PORT_TLV_VAL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_STATUS_TLV_VAL, INST_SINGLE, flag);

    /** Write entry   */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, commit_flag));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function adds the second of two mandatory MDB
 *        entries for an LM/DM offloaded OAM over MPLS-TP or PWE
 *        endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oamp_entry_id - Physical location of this endpoint's
 *        entry in the OAMP MEP DB table, in quarter entries
 * \param [in] entry_values - struct containing the values to be
 *        written to the entry.
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
dnx_oam_oamp_mdb_lm_dm_mplstp_pwe_part_2_add(
    int unit,
    const uint32 oamp_entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values)
{
    uint32 entry_handle_id, commit_flag = DBAL_COMMIT;
    uint16 local_port_to_oamp_pe;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set update flag   */
    if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_UPDATE))
    {
        commit_flag = DBAL_COMMIT_UPDATE;
    }

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_STATIC_DATA_DB, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, oamp_entry_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ACCESS_INDEX, MDB_PART_2_ACCESS);

    /** Set result type   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_OAMP_MEP_STATIC_DATA_DB_MEP_DB_MPLSTP_PWE_LM_DM_OFFLOAD_MDB_FORMAT_PART_2);

    /** Set field values   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INTERFACE_STATUS_TLV_CODE, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_interface_status_tlv_code);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE, INST_SINGLE, entry_values->mep_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_BASE, INST_SINGLE,
                                 entry_values->counter_ptr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_INTERFACE, INST_SINGLE,
                                 entry_values->counter_interface);
    if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_IS_UPMEP)
        && (entry_values->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM))
    {
        /** Is this a CCM Ethernet up-MEP? */
        local_port_to_oamp_pe = (entry_values->unified_port_access.ccm_eth_up_mep_port.pp_port) |
            ((entry_values->unified_port_access.ccm_eth_up_mep_port.port_core) << PORT_CORE_SHIFT);
    }
    else
    {
        local_port_to_oamp_pe = entry_values->unified_port_access.port_profile;
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_PORT_TO_OAMP_PE, INST_SINGLE,
                                 local_port_to_oamp_pe);

    /** Write entry   */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, commit_flag));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function adds the mandatory MDB entry for a 1/4
 *        entry OAM CCM ethernet endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oamp_entry_id - Physical location of this endpoint's
 *        entry in the OAMP MEP DB table, in quarter entries
 * \param [in] entry_values - struct containing the values to be
 *        written to the entry.
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
dnx_oam_oamp_mdb_eth_ccm_q_entry_add(
    int unit,
    const uint32 oamp_entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values)
{
    uint32 entry_handle_id, flag, commit_flag = DBAL_COMMIT;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set update flag   */
    if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_UPDATE))
    {
        commit_flag = DBAL_COMMIT_UPDATE;
    }

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_STATIC_DATA_DB, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, oamp_entry_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ACCESS_INDEX, MDB_Q_ENTRY_ACCESS);

    /** Set result type   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_OAMP_MEP_STATIC_DATA_DB_MEP_DB_CCM_ETH_SHORT_STATIC_MDB_FORMAT);

    /** Set field values   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDL, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_mdl);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAID, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_maid);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INNER_TPID_INDEX, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_inner_tpid_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INNER_VID, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_inner_vid);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INNER_DEI_PCP, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_inner_dei_pcp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_TPID_INDEX, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_outer_tpid_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_VID, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_outer_vid);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_DEI_PCP, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_outer_dei_pcp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_VLAN_TAGS, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_nof_vlan_tags);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UP_PTCH_OPAQUE_PT_ATTR_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_prt_qualifier_profile);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_PORT_TLV_EN);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_STATUS_TLV_EN, INST_SINGLE, flag);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_PORT_TLV_VAL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_STATUS_TLV_VAL, INST_SINGLE, flag);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INTERFACE_STATUS_TLV_CODE, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_interface_status_tlv_code);
    if (dnx_data_oam.oamp.modified_mep_db_structure_get(unit) == 1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CCM_INTERVAL_LSB, INST_SINGLE,
                                     UTILEX_GET_BIT(entry_values->exclusive.oam_only.oam_ccm_interval, 0));
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_BASE, INST_SINGLE,
                                     entry_values->counter_ptr);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_INTERFACE, INST_SINGLE,
                                     entry_values->counter_interface);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LMM_DA_LSB_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_lmm_da_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SA_GEN_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_sa_mac_msb_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SA_MAC_LSB, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_sa_mac_lsb);
    /** Ethernet OAM CCM packets are always multicast, unicast not supported */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DA_UC_MC_TYPE, INST_SINGLE,
                                 UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_MC_UC_SEL));

    /** Write entry   */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, commit_flag));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function adds the mandatory MDB entry for a 1/4
 *        entry OAM over MPLS-TP or PWE endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oamp_entry_id - Physical location of this endpoint's
 *        entry in the OAMP MEP DB table, in quarter entries
 * \param [in] entry_values - struct containing the values to be
 *        written to the entry.
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
dnx_oam_oamp_mdb_mplstp_pwe_q_entry_add(
    int unit,
    const uint32 oamp_entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values)
{
    uint32 entry_handle_id, flag, commit_flag = DBAL_COMMIT;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set update flag   */
    if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_UPDATE))
    {
        commit_flag = DBAL_COMMIT_UPDATE;
    }

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_STATIC_DATA_DB, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, oamp_entry_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ACCESS_INDEX, MDB_Q_ENTRY_ACCESS);

    /** Set result type   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_OAMP_MEP_STATIC_DATA_DB_MEP_DB_MPLSTP_PWE_SHORT_STATIC_MDB_FORMAT);

    /** Set field values   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, INST_SINGLE,
                                 entry_values->dest_sys_port_agr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDL, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_mdl);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAID, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_maid);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF, INST_SINGLE,
                                 entry_values->fec_id_or_glob_out_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, INST_SINGLE,
                                 entry_values->mpls_pwe.label);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_PUSH_PROFILE, INST_SINGLE,
                                 entry_values->mpls_pwe.push_profile);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_PORT_TLV_EN);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_STATUS_TLV_EN, INST_SINGLE, flag);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_PORT_TLV_VAL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_STATUS_TLV_VAL, INST_SINGLE, flag);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INTERFACE_STATUS_TLV_CODE, INST_SINGLE,
                                 entry_values->exclusive.oam_only.oam_interface_status_tlv_code);
    if (dnx_data_oam.oamp.modified_mep_db_structure_get(unit) == 1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CCM_INTERVAL_LSB, INST_SINGLE,
                                     UTILEX_GET_BIT(entry_values->exclusive.oam_only.oam_ccm_interval, 0));
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_BASE, INST_SINGLE,
                                     entry_values->counter_ptr);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_INTERFACE, INST_SINGLE,
                                     entry_values->counter_interface);
    }

    /** Write entry   */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, commit_flag));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function adds mandatory MDB entry for a 1/4
 *        entry single-hop BFD endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oamp_entry_id - Physical location of this endpoint's
 *        entry in the OAMP MEP DB table, in quarter entries
 * \param [in] entry_values - struct containing the values to be
 *        written to the entry.
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
dnx_oam_oamp_mdb_bfd_one_hop_q_entry_add(
    int unit,
    const uint32 oamp_entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values)
{
    uint32 entry_handle_id, flag, commit_flag = DBAL_COMMIT;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set update flag   */
    if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_UPDATE))
    {
        commit_flag = DBAL_COMMIT_UPDATE;
    }

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_STATIC_DATA_DB, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, oamp_entry_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ACCESS_INDEX, MDB_Q_ENTRY_ACCESS);

    /** Set result type   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_OAMP_MEP_STATIC_DATA_DB_MEP_DB_BFD_IPV4_1_HOP_SHORT_STATIC_MDB_FORMAT);

    /** Set field values   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_SRC_IP_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_src_ip_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DETECT_MULT, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_detect_mult);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIN_TX_INTERVAL_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_min_tx_interval_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIN_RX_INTERVAL_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_min_rx_interval_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF, INST_SINGLE,
                                 entry_values->fec_id_or_glob_out_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_DIAG_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_diag_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IP_SUBNET_LEN, INST_SINGLE,
                                 entry_values->ip_subnet_len);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_FLAGS_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_flags_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_STATE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_sta);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE, INST_SINGLE, entry_values->mep_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4_DIP, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_ipv4_dip);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_YOUR_DISC_MSB, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_your_disc);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_BFD_TUNNEL_MPLS_ENABLE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_TUNNEL_MPLS_ENABLE, INST_SINGLE, flag);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_CORE_SELECT, INST_SINGLE,
                                 entry_values->crps_core_select);

    if (dnx_data_oam.oamp.modified_mep_db_structure_get(unit) == 1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_RATE_LSB, INST_SINGLE,
                                     UTILEX_GET_BIT(entry_values->exclusive.bfd_only.bfd_tx_rate, 0));
    }

    /** Write entry   */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, commit_flag));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function adds mandatory MDB entry for a 1/4
 *        entry multi-hop BFD endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oamp_entry_id - Physical location of this endpoint's
 *        entry in the OAMP MEP DB table, in quarter entries
 * \param [in] entry_values - struct containing the values to be
 *        written to the entry.
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
dnx_oam_oamp_mdb_bfd_multi_hop_q_entry_add(
    int unit,
    const uint32 oamp_entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values)
{
    uint32 entry_handle_id, commit_flag = DBAL_COMMIT;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set update flag   */
    if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_UPDATE))
    {
        commit_flag = DBAL_COMMIT_UPDATE;
    }

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_STATIC_DATA_DB, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, oamp_entry_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ACCESS_INDEX, MDB_Q_ENTRY_ACCESS);

    /** Set result type   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_OAMP_MEP_STATIC_DATA_DB_MEP_DB_BFD_IPV4_M_HOP_SHORT_STATIC_MDB_FORMAT);

    /** Set field values   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_SRC_IP_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_src_ip_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DETECT_MULT, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_detect_mult);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIN_TX_INTERVAL_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_min_tx_interval_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIN_RX_INTERVAL_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_min_rx_interval_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF, INST_SINGLE,
                                 entry_values->fec_id_or_glob_out_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_DIAG_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_diag_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IP_TTL_TOS_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_ip_ttl_tos_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_FLAGS_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_flags_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_STATE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_sta);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE, INST_SINGLE, entry_values->mep_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4_DIP, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_ipv4_dip);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_YOUR_DISC_MSB, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_your_disc);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_CORE_SELECT, INST_SINGLE,
                                 entry_values->crps_core_select);

    if (dnx_data_oam.oamp.modified_mep_db_structure_get(unit) == 1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_RATE_LSB, INST_SINGLE,
                                     UTILEX_GET_BIT(entry_values->exclusive.bfd_only.bfd_tx_rate, 0));
    }

    /** Write entry   */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, commit_flag));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function adds mandatory MDB entry for a 1/4
 *        entry BFD over MPLS endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oamp_entry_id - Physical location of this endpoint's
 *        entry in the OAMP MEP DB table, in quarter entries
 * \param [in] entry_values - struct containing the values to be
 *        written to the entry.
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
dnx_oam_oamp_mdb_bfd_mpls_q_entry_add(
    int unit,
    const uint32 oamp_entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values)
{
    uint32 entry_handle_id, commit_flag = DBAL_COMMIT;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set update flag   */
    if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_UPDATE))
    {
        commit_flag = DBAL_COMMIT_UPDATE;
    }

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_STATIC_DATA_DB, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, oamp_entry_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ACCESS_INDEX, MDB_Q_ENTRY_ACCESS);

    /** Set result type   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_OAMP_MEP_STATIC_DATA_DB_MEP_DB_BFD_ON_MPLS_SHORT_STATIC_MDB_FORMAT);

    /** Set field values   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_SRC_IP_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_src_ip_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DETECT_MULT, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_detect_mult);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIN_TX_INTERVAL_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_min_tx_interval_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIN_RX_INTERVAL_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_min_rx_interval_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF, INST_SINGLE,
                                 entry_values->fec_id_or_glob_out_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_DIAG_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_diag_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_PUSH_PROFILE, INST_SINGLE,
                                 entry_values->mpls_pwe.push_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_FLAGS_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_flags_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_STATE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_sta);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE, INST_SINGLE, entry_values->mep_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, INST_SINGLE,
                                 entry_values->mpls_pwe.label);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_YOUR_DISC_MSB, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_your_disc);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IP_TTL_TOS_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_ip_ttl_tos_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_CORE_SELECT, INST_SINGLE,
                                 entry_values->crps_core_select);

    if (dnx_data_oam.oamp.modified_mep_db_structure_get(unit) == 1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_RATE_LSB, INST_SINGLE,
                                     UTILEX_GET_BIT(entry_values->exclusive.bfd_only.bfd_tx_rate, 0));
    }

    /** Write entry   */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, commit_flag));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function adds mandatory PWE entry for a 1/4
 *        entry BFD over MPLS endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oamp_entry_id - Physical location of this endpoint's
 *        entry in the OAMP MEP DB table, in quarter entries
 * \param [in] entry_values - struct containing the values to be
 *        written to the entry.
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
dnx_oam_oamp_mdb_bfd_pwe_q_entry_add(
    int unit,
    const uint32 oamp_entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values)
{
    uint32 entry_handle_id, flag, commit_flag = DBAL_COMMIT;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set update flag   */
    if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_UPDATE))
    {
        commit_flag = DBAL_COMMIT_UPDATE;
    }

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_STATIC_DATA_DB, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, oamp_entry_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ACCESS_INDEX, MDB_Q_ENTRY_ACCESS);

    /** Set result type   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_OAMP_MEP_STATIC_DATA_DB_MEP_DB_BFD_ON_PWE_SHORT_STATIC_MDB_FORMAT);

    /** Set field values   */
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_ROUTER_ALERT);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ROUTER_ALERT, INST_SINGLE, flag);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DETECT_MULT, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_detect_mult);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIN_TX_INTERVAL_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_min_tx_interval_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIN_RX_INTERVAL_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_min_rx_interval_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF, INST_SINGLE,
                                 entry_values->fec_id_or_glob_out_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_DIAG_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_diag_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_PUSH_PROFILE, INST_SINGLE,
                                 entry_values->mpls_pwe.push_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_FLAGS_PROFILE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_flags_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_STATE, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_sta);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE, INST_SINGLE, entry_values->mep_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, INST_SINGLE,
                                 entry_values->mpls_pwe.label);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_YOUR_DISC_MSB, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_your_disc);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_ACH);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACH, INST_SINGLE, flag);
    flag = UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_GAL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GAL, INST_SINGLE, flag);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACH_SEL, INST_SINGLE,
                                 entry_values->exclusive.bfd_only.bfd_ach_sel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, INST_SINGLE,
                                 entry_values->dest_sys_port_agr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_CORE_SELECT, INST_SINGLE,
                                 entry_values->crps_core_select);

    if (dnx_data_oam.oamp.modified_mep_db_structure_get(unit) == 1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_RATE_LSB, INST_SINGLE,
                                     UTILEX_GET_BIT(entry_values->exclusive.bfd_only.bfd_tx_rate, 0));
    }

    /** Write entry   */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, commit_flag));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function adds an extra data header entry to the
 *        OAMP MEP STATIC DATA DB table.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [in] entry_values - struct containing the values to be
 *        written to the entry.
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
dnx_oam_oamp_mdb_extra_data_header_add(
    int unit,
    const uint32 entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_STATIC_DATA_DB, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, entry_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ACCESS_INDEX, MDB_EXTRA_DATA_HDR_ACCESS);

    /** Set result type   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_OAMP_MEP_STATIC_DATA_DB_MEP_DB_EXTRA_DATA_IN_MDB_HEADER);

    /** Set field values   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODES_TO_PREPEND, INST_SINGLE,
                                 entry_values->exclusive.extra_data.opcode_bmp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CHECK_CRC_VALUE_1, INST_SINGLE,
                                 entry_values->exclusive.extra_data.crc_val1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CHECK_CRC_VALUE_2, INST_SINGLE,
                                 entry_values->exclusive.extra_data.crc_val2);
    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_VALUE, INST_SINGLE,
                                    (uint8 *) entry_values->exclusive.extra_data.data_segment);

    /** Write entry   */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function adds an extra data payload entry to
 *        the OAMP MEP DB.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - Physical location of the entry in the
 *        OAMP MEP DB table, in quarter entries
 * \param [in] entry_values - struct containing the values to be
 *        written to the entry.
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
dnx_oam_oamp_mdb_extra_data_payload_add(
    int unit,
    const uint32 entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_STATIC_DATA_DB, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, entry_id);

    /**
     * For offloaded/short entry endpoints, extra_data_len is
     * not in use, so we use it as segment index
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ACCESS_INDEX,
                               entry_values->exclusive.extra_data.extra_data_len);

    /** Set result type   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_OAMP_MEP_STATIC_DATA_DB_MEP_DB_EXTRA_DATA_IN_MDB_PAYLOAD);

    /** Set field values   */
    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_VALUE, INST_SINGLE,
                                    (uint8 *) entry_values->exclusive.extra_data.data_segment);

    /** Write entry   */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in oam_internal.h
 */
shr_error_e
dnx_oam_oamp_mdb_add(
    int unit,
    const uint32 oamp_entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_oam_oamp_mep_db_verify(unit, oamp_entry_id));
    SHR_NULL_CHECK(entry_values, _SHR_E_PARAM, "entry_values");

    /** Is the requested format "LM/DM offloaded?"   */
    if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_LM_DM_OFFLOADED))
    {
        /** The "Mep type" field must be an endpoint   */
        switch (entry_values->mep_type)
        {
            case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM:
                /** The 1/4 entry flag becomes the part 1 or part 2 flag   */
                if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_Q_ENTRY))
                {
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_lm_dm_eth_ccm_part_2_add(unit, oamp_entry_id, entry_values));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_lm_dm_eth_ccm_part_1_add(unit, oamp_entry_id, entry_values));
                }
                break;

            case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_MPLSTP:
            case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE:
                /** The 1/4 entry flag becomes the part 1 or part 2 flag   */
                if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_Q_ENTRY))
                {
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_lm_dm_mplstp_pwe_part_2_add(unit, oamp_entry_id, entry_values));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_lm_dm_mplstp_pwe_part_1_add(unit, oamp_entry_id, entry_values));
                }
                break;

            case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_1HOP:
            case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP:
            case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS:
            case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Offloaded BFD endpoints are not supported - no LM/DM.\n");
                break;

            case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_EXT_DATA_HDR:
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_extra_data_header_add(unit, oamp_entry_id, entry_values));
                break;

            case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_EXT_DATA_PLD:
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_extra_data_payload_add(unit, oamp_entry_id, entry_values));
                break;

            default:
                /** Not an endpoint type   */
                SHR_ERR_EXIT(_SHR_E_PARAM, "Type %d is not an endpoint.  Only endpoints can be LM/DM offloaded.\n",
                             entry_values->mep_type);
        }
    }
    else
    {
        /** Not LM/DM offloaded.  Is it a 1/4 entry?   */
        if (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_Q_ENTRY))
        {
            switch (entry_values->mep_type)
            {
                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM:
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_eth_ccm_q_entry_add(unit, oamp_entry_id, entry_values));
                    break;

                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_MPLSTP:
                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE:
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_mplstp_pwe_q_entry_add(unit, oamp_entry_id, entry_values));
                    break;

                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_1HOP:
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_bfd_one_hop_q_entry_add(unit, oamp_entry_id, entry_values));
                    break;

                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP:
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_bfd_multi_hop_q_entry_add(unit, oamp_entry_id, entry_values));
                    break;

                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS:
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_bfd_mpls_q_entry_add(unit, oamp_entry_id, entry_values));
                    break;

                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE:
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_bfd_pwe_q_entry_add(unit, oamp_entry_id, entry_values));
                    break;

                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_EXT_DATA_HDR:
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_extra_data_header_add(unit, oamp_entry_id, entry_values));
                    break;

                case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_EXT_DATA_PLD:
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_extra_data_payload_add(unit, oamp_entry_id, entry_values));
                    break;

                default:
                    /** Not an endpoint type   */
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Type %d is not an endpoint.  Only endpoints can be 1/4 entry.\n",
                                 entry_values->mep_type);
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Self contained endpoints cannot have MDB entries.\n");
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads the first of two mandatory MDB
 *        entries for an offloaded ethernet CCM entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] entry_values - struct into which the fields read
 *        from the MEP DB entry will be written
 * \param [in] entry_handle_id - handle to the entry, which will
 *        be used to access the different fields.
 *
 * \return
 *   * shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mdb_lm_dm_eth_ccm_part_1_get(
    int unit,
    dnx_oam_oamp_mep_db_args_t * entry_values,
    uint32 entry_handle_id)
{
    uint32 value;

    SHR_FUNC_INIT_VARS(unit);

    /** Get field values   */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_CCM_INTERVAL, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_ccm_interval = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ITMH_TC_DP_PROFILE, INST_SINGLE, &value));
    entry_values->itmh_tc_dp_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MDL, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_mdl = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MAID, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_maid = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ICC_MAP_INDEX, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_icc_index = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MEP_ID, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_mep_id = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_UP_1_DOWN_0, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_IS_UPMEP);
    if (value == TRUE)
    {
        /** Endpoint is CCM ethernet up-MEP */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_LOCAL_PORT_TO_OAMP_PE, INST_SINGLE, &value));
        entry_values->unified_port_access.ccm_eth_up_mep_port.port_core = (value >> PORT_CORE_SHIFT);
        entry_values->unified_port_access.ccm_eth_up_mep_port.pp_port =
            UTILEX_GET_BITS_RANGE(value, PORT_CORE_SHIFT - 1, 0);
    }
    else
    {
        /** Endpoint is CCM ethernet down-MEP, MPLS-TP or PWE */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, INST_SINGLE, &value));
        entry_values->unified_port_access.port_profile = value;
    }
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_INNER_TPID_INDEX, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_inner_tpid_index = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_INNER_VID, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_inner_vid = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_INNER_DEI_PCP, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_inner_dei_pcp = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_OUTER_TPID_INDEX, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_outer_tpid_index = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_OUTER_VID, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_outer_vid = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_OUTER_DEI_PCP, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_outer_dei_pcp = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_NOF_VLAN_TAGS, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_nof_vlan_tags = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_UP_PTCH_OPAQUE_PT_ATTR_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_prt_qualifier_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PORT_STATUS_TLV_EN, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_PORT_TLV_EN);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PORT_STATUS_TLV_VAL, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_PORT_TLV_VAL);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_INTERFACE_STATUS_TLV_CODE, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_interface_status_tlv_code = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE, INST_SINGLE, &value));
    entry_values->mep_profile = value;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads the second of two mandatory MDB
 *        entries for an offloaded ethernet CCM entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] entry_values - struct into which the fields read
 *        from the MEP DB entry will be written
 * \param [in] entry_handle_id - handle to the entry, which will
 *        be used to access the different fields.
 *
 * \return
 *   * shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mdb_lm_dm_eth_ccm_part_2_get(
    int unit,
    dnx_oam_oamp_mep_db_args_t * entry_values,
    uint32 entry_handle_id)
{
    uint32 value;

    SHR_FUNC_INIT_VARS(unit);
    /** Get field values   */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_COUNTER_BASE, INST_SINGLE, &value));
    entry_values->counter_ptr = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_COUNTER_INTERFACE, INST_SINGLE, &value));
    entry_values->counter_interface = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_LMM_DA_LSB_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_lmm_da_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SA_GEN_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_sa_mac_msb_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SA_MAC_LSB, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_sa_mac_lsb = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_DA_UC_MC_TYPE, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_MC_UC_SEL);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads the first of two mandatory MDB
 *        entries for an LM/DM offloaded OAM over MPLS-TP or PWE
 *        endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] entry_values - struct into which the fields read
 *        from the MEP DB entry will be written
 * \param [in] entry_handle_id - handle to the entry, which will
 *        be used to access the different fields.
 *
 * \return
 *   * shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mdb_lm_dm_mplstp_pwe_part_1_get(
    int unit,
    dnx_oam_oamp_mep_db_args_t * entry_values,
    uint32 entry_handle_id)
{
    uint32 value;
    SHR_FUNC_INIT_VARS(unit);
    /** Get field values   */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_CCM_INTERVAL, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_ccm_interval = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ITMH_TC_DP_PROFILE, INST_SINGLE, &value));
    entry_values->itmh_tc_dp_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, INST_SINGLE, &value));
    entry_values->dest_sys_port_agr = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MDL, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_mdl = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MAID, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_maid = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ICC_MAP_INDEX, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_icc_index = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MEP_ID, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_mep_id = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF, INST_SINGLE,
                     &entry_values->fec_id_or_glob_out_lif));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, INST_SINGLE, &entry_values->mpls_pwe.label));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MPLS_PUSH_PROFILE, INST_SINGLE, &value));
    entry_values->mpls_pwe.push_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PORT_STATUS_TLV_EN, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_PORT_TLV_EN);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PORT_STATUS_TLV_VAL, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_PORT_TLV_VAL);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads the second of two mandatory MDB
 *        entries for an LM/DM offloaded OAM over MPLS-TP or PWE
 *        endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] entry_values - struct into which the fields read
 *        from the MEP DB entry will be written
 * \param [in] entry_handle_id - handle to the entry, which will
 *        be used to access the different fields.
 *
 * \return
 *   * shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mdb_lm_dm_mplstp_pwe_part_2_get(
    int unit,
    dnx_oam_oamp_mep_db_args_t * entry_values,
    uint32 entry_handle_id)
{
    uint32 value;
    SHR_FUNC_INIT_VARS(unit);
    /** Get field values   */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_INTERFACE_STATUS_TLV_CODE, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_interface_status_tlv_code = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE, INST_SINGLE, &value));
    entry_values->mep_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_COUNTER_BASE, INST_SINGLE, &value));
    entry_values->counter_ptr = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_COUNTER_INTERFACE, INST_SINGLE, &value));
    entry_values->counter_interface = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_LOCAL_PORT_TO_OAMP_PE, INST_SINGLE, &value));
    entry_values->unified_port_access.port_profile = value;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads the mandatory MDB entry for a 1/4
 *        entry OAM CCM ethernet endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] entry_values - struct into which the fields read
 *        from the MEP DB entry will be written
 * \param [in] entry_handle_id - handle to the entry, which will
 *        be used to access the different fields.
 *
 * \return
 *   * shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mdb_eth_ccm_q_entry_get(
    int unit,
    dnx_oam_oamp_mep_db_args_t * entry_values,
    uint32 entry_handle_id)
{
    uint32 value;
    SHR_FUNC_INIT_VARS(unit);
    /** Get field values   */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MDL, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_mdl = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MAID, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_maid = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_INNER_TPID_INDEX, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_inner_tpid_index = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_INNER_VID, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_inner_vid = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_INNER_DEI_PCP, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_inner_dei_pcp = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_OUTER_TPID_INDEX, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_outer_tpid_index = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_OUTER_VID, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_outer_vid = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_OUTER_DEI_PCP, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_outer_dei_pcp = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_NOF_VLAN_TAGS, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_nof_vlan_tags = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_UP_PTCH_OPAQUE_PT_ATTR_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_prt_qualifier_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PORT_STATUS_TLV_EN, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_PORT_TLV_EN);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PORT_STATUS_TLV_VAL, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_PORT_TLV_VAL);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_INTERFACE_STATUS_TLV_CODE, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_interface_status_tlv_code = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_LMM_DA_LSB_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_lmm_da_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SA_GEN_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_sa_mac_msb_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SA_MAC_LSB, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_sa_mac_lsb = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_DA_UC_MC_TYPE, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_MC_UC_SEL);

    if (dnx_data_oam.oamp.modified_mep_db_structure_get(unit) == 1)
    {
        /** Only the LSB of the value is read here */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_CCM_INTERVAL_LSB, INST_SINGLE, &value));
        entry_values->exclusive.oam_only.oam_ccm_interval =
            (entry_values->exclusive.oam_only.oam_ccm_interval & CCM_INT_BFD_RATE_MSB_MASK) | UTILEX_GET_BIT(value, 0);
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_COUNTER_BASE, INST_SINGLE, &value));
        entry_values->counter_ptr = value;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_COUNTER_INTERFACE, INST_SINGLE, &value));
        entry_values->counter_interface = value;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads the mandatory MDB entry for a 1/4
 *        entry OAM over MPLS-TP or PWE endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] entry_values - struct into which the fields read
 *        from the MEP DB entry will be written
 * \param [in] entry_handle_id - handle to the entry, which will
 *        be used to access the different fields.
 *
 * \return
 *   * shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mdb_mplstp_pwe_q_entry_get(
    int unit,
    dnx_oam_oamp_mep_db_args_t * entry_values,
    uint32 entry_handle_id)
{
    uint32 value;
    SHR_FUNC_INIT_VARS(unit);
    /** Get field values   */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, INST_SINGLE, &value));
    entry_values->dest_sys_port_agr = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MDL, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_mdl = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MAID, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_maid = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF, INST_SINGLE,
                     &entry_values->fec_id_or_glob_out_lif));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, INST_SINGLE, &entry_values->mpls_pwe.label));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MPLS_PUSH_PROFILE, INST_SINGLE, &value));
    entry_values->mpls_pwe.push_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PORT_STATUS_TLV_EN, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_PORT_TLV_EN);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PORT_STATUS_TLV_VAL, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_PORT_TLV_VAL);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_INTERFACE_STATUS_TLV_CODE, INST_SINGLE, &value));
    entry_values->exclusive.oam_only.oam_interface_status_tlv_code = value;

    if (dnx_data_oam.oamp.modified_mep_db_structure_get(unit) == 1)
    {
        /** Only the LSB of the value is read here */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_CCM_INTERVAL_LSB, INST_SINGLE, &value));
        entry_values->exclusive.oam_only.oam_ccm_interval =
            (entry_values->exclusive.oam_only.oam_ccm_interval & CCM_INT_BFD_RATE_MSB_MASK) | UTILEX_GET_BIT(value, 0);
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_COUNTER_BASE, INST_SINGLE, &value));
        entry_values->counter_ptr = value;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_COUNTER_INTERFACE, INST_SINGLE, &value));
        entry_values->counter_interface = value;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads the mandatory MDB entry for a 1/4
 *        entry single-hop BFD endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] entry_values - struct into which the fields read
 *        from the MEP DB entry will be written
 * \param [in] entry_handle_id - handle to the entry, which will
 *        be used to access the different fields.
 *
 * \return
 *   * shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mdb_bfd_one_hop_q_entry_get(
    int unit,
    dnx_oam_oamp_mep_db_args_t * entry_values,
    uint32 entry_handle_id)
{
    uint32 value;
    SHR_FUNC_INIT_VARS(unit);
    /** Get field values   */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_SRC_IP_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_src_ip_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_DETECT_MULT, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_detect_mult = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MIN_TX_INTERVAL_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_min_tx_interval_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MIN_RX_INTERVAL_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_min_rx_interval_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF, INST_SINGLE,
                     &entry_values->fec_id_or_glob_out_lif));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_DIAG_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_diag_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_IP_SUBNET_LEN, INST_SINGLE, &value));
    entry_values->ip_subnet_len = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_FLAGS_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_flags_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_STATE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_sta = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE, INST_SINGLE, &value));
    entry_values->mep_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_IPV4_DIP, INST_SINGLE,
                                                        &entry_values->exclusive.bfd_only.bfd_ipv4_dip));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_YOUR_DISC_MSB, INST_SINGLE,
                                                        &entry_values->exclusive.bfd_only.bfd_your_disc));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_TUNNEL_MPLS_ENABLE, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_BFD_TUNNEL_MPLS_ENABLE);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_CRPS_CORE_SELECT, INST_SINGLE, &value));
    entry_values->crps_core_select = value;
    if (dnx_data_oam.oamp.modified_mep_db_structure_get(unit) == 1)
    {
        /** Only the LSB of the value is read here */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_TX_RATE_LSB, INST_SINGLE, &value));
        entry_values->exclusive.bfd_only.bfd_tx_rate =
            (entry_values->exclusive.bfd_only.bfd_tx_rate & CCM_INT_BFD_RATE_MSB_MASK) | UTILEX_GET_BIT(value, 0);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads the mandatory MDB entry for a 1/4
 *        entry multi-hop BFD endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] entry_values - struct into which the fields read
 *        from the MEP DB entry will be written
 * \param [in] entry_handle_id - handle to the entry, which will
 *        be used to access the different fields.
 *
 * \return
 *   * shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mdb_bfd_multi_hop_q_entry_get(
    int unit,
    dnx_oam_oamp_mep_db_args_t * entry_values,
    uint32 entry_handle_id)
{
    uint32 value;
    SHR_FUNC_INIT_VARS(unit);

    /** Get field values   */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_SRC_IP_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_src_ip_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_DETECT_MULT, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_detect_mult = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MIN_TX_INTERVAL_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_min_tx_interval_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MIN_RX_INTERVAL_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_min_rx_interval_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF, INST_SINGLE,
                     &entry_values->fec_id_or_glob_out_lif));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_DIAG_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_diag_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_IP_TTL_TOS_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_ip_ttl_tos_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_FLAGS_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_flags_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_STATE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_sta = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE, INST_SINGLE, &value));
    entry_values->mep_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_IPV4_DIP, INST_SINGLE,
                                                        &entry_values->exclusive.bfd_only.bfd_ipv4_dip));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_YOUR_DISC_MSB, INST_SINGLE,
                                                        &entry_values->exclusive.bfd_only.bfd_your_disc));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_CRPS_CORE_SELECT, INST_SINGLE, &value));
    entry_values->crps_core_select = value;
    if (dnx_data_oam.oamp.modified_mep_db_structure_get(unit) == 1)
    {
        /** Only the LSB of the value is read here */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_TX_RATE_LSB, INST_SINGLE, &value));
        entry_values->exclusive.bfd_only.bfd_tx_rate =
            (entry_values->exclusive.bfd_only.bfd_tx_rate & CCM_INT_BFD_RATE_MSB_MASK) | UTILEX_GET_BIT(value, 0);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads the mandatory MDB entry for a 1/4
 *        entry BFD over MPLS endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] entry_values - struct into which the fields read
 *        from the MEP DB entry will be written
 * \param [in] entry_handle_id - handle to the entry, which will
 *        be used to access the different fields.
 *
 * \return
 *   * shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mdb_bfd_mpls_q_entry_get(
    int unit,
    dnx_oam_oamp_mep_db_args_t * entry_values,
    uint32 entry_handle_id)
{
    uint32 value;
    SHR_FUNC_INIT_VARS(unit);

    /** Get field values   */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_SRC_IP_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_src_ip_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_DETECT_MULT, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_detect_mult = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MIN_TX_INTERVAL_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_min_tx_interval_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MIN_RX_INTERVAL_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_min_rx_interval_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF, INST_SINGLE,
                     &entry_values->fec_id_or_glob_out_lif));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_DIAG_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_diag_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MPLS_PUSH_PROFILE, INST_SINGLE, &value));
    entry_values->mpls_pwe.push_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_FLAGS_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_flags_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_STATE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_sta = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE, INST_SINGLE, &value));
    entry_values->mep_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, INST_SINGLE,
                                                        &entry_values->mpls_pwe.label));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_YOUR_DISC_MSB, INST_SINGLE,
                                                        &entry_values->exclusive.bfd_only.bfd_your_disc));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_IP_TTL_TOS_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_ip_ttl_tos_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_CRPS_CORE_SELECT, INST_SINGLE, &value));
    entry_values->crps_core_select = value;
    if (dnx_data_oam.oamp.modified_mep_db_structure_get(unit) == 1)
    {
        /** Only the LSB of the value is read here */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_TX_RATE_LSB, INST_SINGLE, &value));
        entry_values->exclusive.bfd_only.bfd_tx_rate =
            (entry_values->exclusive.bfd_only.bfd_tx_rate & CCM_INT_BFD_RATE_MSB_MASK) | UTILEX_GET_BIT(value, 0);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads the mandatory MDB entry for a 1/4
 *        entry BFD over PWE endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] entry_values - struct into which the fields read
 *        from the MEP DB entry will be written
 * \param [in] entry_handle_id - handle to the entry, which will
 *        be used to access the different fields.
 *
 * \return
 *   * shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mdb_bfd_pwe_q_entry_get(
    int unit,
    dnx_oam_oamp_mep_db_args_t * entry_values,
    uint32 entry_handle_id)
{
    uint32 value;
    SHR_FUNC_INIT_VARS(unit);

    /** Get field values   */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ROUTER_ALERT, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_ROUTER_ALERT);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_DETECT_MULT, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_detect_mult = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MIN_TX_INTERVAL_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_min_tx_interval_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MIN_RX_INTERVAL_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_min_rx_interval_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF, INST_SINGLE,
                     &entry_values->fec_id_or_glob_out_lif));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_DIAG_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_diag_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MPLS_PUSH_PROFILE, INST_SINGLE, &value));
    entry_values->mpls_pwe.push_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_FLAGS_PROFILE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_flags_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFD_STATE, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_sta = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE, INST_SINGLE, &value));
    entry_values->mep_profile = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, INST_SINGLE,
                                                        &entry_values->mpls_pwe.label));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_YOUR_DISC_MSB, INST_SINGLE,
                                                        &entry_values->exclusive.bfd_only.bfd_your_disc));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_ACH, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_ACH);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_GAL, INST_SINGLE, &value));
    UTILEX_SET_BIT(entry_values->flags, value, OAMP_MEP_GAL);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ACH_SEL, INST_SINGLE, &value));
    entry_values->exclusive.bfd_only.bfd_ach_sel = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, INST_SINGLE, &value));
    entry_values->dest_sys_port_agr = value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_CRPS_CORE_SELECT, INST_SINGLE, &value));
    entry_values->crps_core_select = value;
    if (dnx_data_oam.oamp.modified_mep_db_structure_get(unit) == 1)
    {
        /** Only the LSB of the value is read here */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_TX_RATE_LSB, INST_SINGLE, &value));
        entry_values->exclusive.bfd_only.bfd_tx_rate =
            (entry_values->exclusive.bfd_only.bfd_tx_rate & CCM_INT_BFD_RATE_MSB_MASK) | UTILEX_GET_BIT(value, 0);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads the header of extra data.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] entry_values - struct into which the fields read
 *        from the MEP DB entry will be written
 * \param [in] entry_handle_id - handle to the entry, which will
 *        be used to access the different fields.
 *
 * \return
 *   * shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mdb_extra_data_header_get(
    int unit,
    dnx_oam_oamp_mep_db_args_t * entry_values,
    uint32 entry_handle_id)
{
    uint32 value;
    uint8 value_array8[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    /** Get field values   */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_OPCODES_TO_PREPEND, INST_SINGLE, &value));
    entry_values->exclusive.extra_data.opcode_bmp = value;

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_CHECK_CRC_VALUE_1, INST_SINGLE, &value));
    entry_values->exclusive.extra_data.crc_val1 = value;

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_CHECK_CRC_VALUE_2, INST_SINGLE, &value));
    entry_values->exclusive.extra_data.crc_val2 = value;

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                    (unit, entry_handle_id, DBAL_FIELD_VALUE, INST_SINGLE, value_array8));
    sal_memcpy((uint8 *) entry_values->exclusive.extra_data.data_segment, value_array8, DATA_SEG_LEN);
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads the MDB extra data
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] entry_values - struct into which the fields read
 *        from the MEP DB entry will be written
 * \param [in] entry_handle_id - handle to the entry, which will
 *        be used to access the different fields.
 *
 * \return
 *   * shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mdb_extra_data_payload_get(
    int unit,
    dnx_oam_oamp_mep_db_args_t * entry_values,
    uint32 entry_handle_id)
{

    uint8 value_array8[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    /** Get field values   */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                    (unit, entry_handle_id, DBAL_FIELD_VALUE, INST_SINGLE, value_array8));
    sal_memcpy((uint8 *) entry_values->exclusive.extra_data.data_segment, value_array8, DATA_SEG_LEN);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in oam_internal.h
 */
shr_error_e
dnx_oam_oamp_mdb_get(
    int unit,
    const uint32 oamp_entry_id,
    const uint32 access_index,
    dnx_oam_oamp_mep_db_args_t * entry_values)
{
    uint32 entry_handle_id;
    dbal_enum_value_result_type_oamp_mep_static_data_db_e resulting_format;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_oam_oamp_mep_db_verify(unit, oamp_entry_id));
    SHR_NULL_CHECK(entry_values, _SHR_E_PARAM, "entry_values");

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_STATIC_DATA_DB, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, oamp_entry_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ACCESS_INDEX, access_index);

    /** Get all fields   */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** Read format   */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &resulting_format));

    /** The "Mep type" field must be an endpoint   */
    switch (resulting_format)
    {
        case DBAL_RESULT_TYPE_OAMP_MEP_STATIC_DATA_DB_MEP_DB_CCM_ETH_LM_DM_OFFLOAD_MDB_FORMAT_PART_1_DOWNMEP:
        case DBAL_RESULT_TYPE_OAMP_MEP_STATIC_DATA_DB_MEP_DB_CCM_ETH_LM_DM_OFFLOAD_MDB_FORMAT_PART_1_UPMEP:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_lm_dm_eth_ccm_part_1_get(unit, entry_values, entry_handle_id));
            break;

        case DBAL_RESULT_TYPE_OAMP_MEP_STATIC_DATA_DB_MEP_DB_CCM_ETH_LM_DM_OFFLOAD_MDB_FORMAT_PART_2:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_lm_dm_eth_ccm_part_2_get(unit, entry_values, entry_handle_id));
            break;

        case DBAL_RESULT_TYPE_OAMP_MEP_STATIC_DATA_DB_MEP_DB_MPLSTP_PWE_LM_DM_OFFLOAD_MDB_FORMAT_PART_1:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_lm_dm_mplstp_pwe_part_1_get(unit, entry_values, entry_handle_id));
            break;

        case DBAL_RESULT_TYPE_OAMP_MEP_STATIC_DATA_DB_MEP_DB_MPLSTP_PWE_LM_DM_OFFLOAD_MDB_FORMAT_PART_2:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_lm_dm_mplstp_pwe_part_2_get(unit, entry_values, entry_handle_id));
            break;

        case DBAL_RESULT_TYPE_OAMP_MEP_STATIC_DATA_DB_MEP_DB_CCM_ETH_SHORT_STATIC_MDB_FORMAT:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_eth_ccm_q_entry_get(unit, entry_values, entry_handle_id));
            break;

        case DBAL_RESULT_TYPE_OAMP_MEP_STATIC_DATA_DB_MEP_DB_MPLSTP_PWE_SHORT_STATIC_MDB_FORMAT:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_mplstp_pwe_q_entry_get(unit, entry_values, entry_handle_id));
            break;

        case DBAL_RESULT_TYPE_OAMP_MEP_STATIC_DATA_DB_MEP_DB_BFD_IPV4_1_HOP_SHORT_STATIC_MDB_FORMAT:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_bfd_one_hop_q_entry_get(unit, entry_values, entry_handle_id));
            break;

        case DBAL_RESULT_TYPE_OAMP_MEP_STATIC_DATA_DB_MEP_DB_BFD_IPV4_M_HOP_SHORT_STATIC_MDB_FORMAT:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_bfd_multi_hop_q_entry_get(unit, entry_values, entry_handle_id));
            break;

        case DBAL_RESULT_TYPE_OAMP_MEP_STATIC_DATA_DB_MEP_DB_BFD_ON_MPLS_SHORT_STATIC_MDB_FORMAT:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_bfd_mpls_q_entry_get(unit, entry_values, entry_handle_id));
            break;

        case DBAL_RESULT_TYPE_OAMP_MEP_STATIC_DATA_DB_MEP_DB_BFD_ON_PWE_SHORT_STATIC_MDB_FORMAT:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_bfd_pwe_q_entry_get(unit, entry_values, entry_handle_id));
            break;

        case DBAL_RESULT_TYPE_OAMP_MEP_STATIC_DATA_DB_MEP_DB_EXTRA_DATA_IN_MDB_HEADER:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_extra_data_header_get(unit, entry_values, entry_handle_id));
            break;

        case DBAL_RESULT_TYPE_OAMP_MEP_STATIC_DATA_DB_MEP_DB_EXTRA_DATA_IN_MDB_PAYLOAD:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_extra_data_payload_get(unit, entry_values, entry_handle_id));
            break;

        default:
            /** Unsupported value   */
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Result type %d read from entry is unsupported", resulting_format);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in oam_internal.h
 */
shr_error_e
dnx_oam_oamp_mdb_delete(
    int unit,
    const uint32 oamp_entry_id,
    const uint32 access_index)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_oam_oamp_mep_db_verify(unit, oamp_entry_id));

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_STATIC_DATA_DB, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, oamp_entry_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ACCESS_INDEX, access_index);

    /** Access the table */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in oam_internal.h
 */
shr_error_e
dnx_oam_oamp_mep_db_add(
    int unit,
    const uint32 oamp_entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values)
{
    int attempt_count = 0, failure = 0;
    dnx_oam_oamp_mep_db_args_t *read_entry_values = NULL;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_oam_oamp_mep_db_verify(unit, oamp_entry_id));
    SHR_NULL_CHECK(entry_values, _SHR_E_PARAM, "entry_values");

    if (!UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_UPDATE))
    {
        /** Allocate entry ID for usage */
        SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_entry_alloc(unit, (int *) &oamp_entry_id));
    }

    SHR_ALLOC_SET_ZERO(read_entry_values, sizeof(dnx_oam_oamp_mep_db_args_t),
                       "Struct for MEP DB entry value verification", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    do
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_entry_add(unit, oamp_entry_id, entry_values));

        /**
         * For offloaded and short entry endpoints there is no need to do this,
         * as the interesting values are in the MDB.  LM/DM entries also, for the
         * as there are mostly dynamic values
         */
        if ((UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_LM_DM_OFFLOADED) != 0) ||
            (UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_Q_ENTRY) != 0) ||
            (entry_values->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_DB) ||
            (entry_values->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_STAT) ||
            (entry_values->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT) ||
            (entry_values->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT_ONE_WAY))
        {
            failure = 0;
            break;
        }

        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oamp_entry_id, read_entry_values));

        if (entry_values->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_EXT_DATA_PLD)
        {
            failure = sal_memcmp(entry_values->exclusive.extra_data.data_segment,
                                 read_entry_values->exclusive.extra_data.data_segment,
                                 sizeof(entry_values->exclusive.extra_data.data_segment));
            if (failure)
            {
                attempt_count++;
                continue;
            }
            break;
        }

        /** Update flag will never be read back */
        UTILEX_SET_BIT(read_entry_values->flags, UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_UPDATE), OAMP_MEP_UPDATE);

        /** Value of ECHO flag is write-only */
        UTILEX_SET_BIT(read_entry_values->flags, UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_BFD_ECHO),
                       OAMP_MEP_BFD_ECHO);

        /** Value of RDI from scanner flag is read-only */
        UTILEX_SET_BIT(read_entry_values->flags, UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_RDI_FROM_SCANNER),
                       OAMP_MEP_RDI_FROM_SCANNER);

        /** Value of Down MEP Egress Injection is not a part of MEP DB */
        UTILEX_SET_BIT(read_entry_values->flags, UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_EGRESS_INJECTION),
                       OAMP_MEP_EGRESS_INJECTION);

        /** These flags are write-only or irrelevant to self-contained endpoints */
        read_entry_values->flags =
            UTILEX_GET_BITS_RANGE(read_entry_values->flags, OAMP_MEP_LM_CNTRS_VALID, OAMP_MEP_UPDATE) |
            (UTILEX_GET_BITS_RANGE(entry_values->flags, OAMP_MEP_COPY_DM_STATISTICS, OAMP_MEP_LM_ENABLE) <<
             OAMP_MEP_LM_ENABLE);

        /** Exception: tunnel enable flag relevant only for single-hop BFD */
        if (entry_values->mep_type != DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_1HOP)
        {
            UTILEX_SET_BIT(read_entry_values->flags,
                           UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_BFD_TUNNEL_MPLS_ENABLE),
                           OAMP_MEP_BFD_TUNNEL_MPLS_ENABLE);

            /** So is subnet length */
            read_entry_values->ip_subnet_len = entry_values->ip_subnet_len;
        }

        /** These values are only relevant to ethernet OAM */
        if ((entry_values->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_MPLSTP) ||
            (entry_values->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE))
        {
            read_entry_values->exclusive.oam_only.oam_inner_vid = entry_values->exclusive.oam_only.oam_inner_vid;
            read_entry_values->exclusive.oam_only.oam_inner_tpid_index =
                entry_values->exclusive.oam_only.oam_inner_tpid_index;
            read_entry_values->exclusive.oam_only.oam_outer_vid = entry_values->exclusive.oam_only.oam_outer_vid;
            read_entry_values->exclusive.oam_only.oam_outer_tpid_index =
                entry_values->exclusive.oam_only.oam_outer_tpid_index;
            read_entry_values->exclusive.oam_only.oam_sa_mac_lsb = entry_values->exclusive.oam_only.oam_sa_mac_lsb;
            read_entry_values->exclusive.oam_only.oam_sa_mac_msb_profile =
                entry_values->exclusive.oam_only.oam_sa_mac_msb_profile;
            read_entry_values->exclusive.oam_only.oam_inner_dei_pcp =
                entry_values->exclusive.oam_only.oam_inner_dei_pcp;
            read_entry_values->exclusive.oam_only.oam_outer_dei_pcp =
                entry_values->exclusive.oam_only.oam_outer_dei_pcp;
            read_entry_values->exclusive.oam_only.oam_nof_vlan_tags =
                entry_values->exclusive.oam_only.oam_nof_vlan_tags;
        }
        else
        {
            /** For other entries, these values are meaningless */
            read_entry_values->mpls_pwe.label = entry_values->mpls_pwe.label;
            read_entry_values->mpls_pwe.push_profile = entry_values->mpls_pwe.push_profile;
            read_entry_values->dest_sys_port_agr = entry_values->dest_sys_port_agr;
        }

        /** VSI is relevant for Down-MEP egress injection only */
        if (!UTILEX_GET_BIT(entry_values->flags, OAMP_MEP_EGRESS_INJECTION))
        {
            read_entry_values->exclusive.oam_only.vsi = entry_values->exclusive.oam_only.vsi;
        }

        /** fec_id_or_glob_out_lif is not relevant for Ethernet OAM */
        if (entry_values->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM)
        {
            read_entry_values->fec_id_or_glob_out_lif = entry_values->fec_id_or_glob_out_lif;
        }

        failure = sal_memcmp(entry_values, read_entry_values, sizeof(dnx_oam_oamp_mep_db_args_t));
        attempt_count++;
    }
    while ((failure != 0) && (attempt_count < MAX_WRITE_ATTEMPTS));

    if (failure)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Cannot write to MEP DB, all %d attempts have failed", attempt_count);
    }

exit:
    SHR_FREE(read_entry_values);
    SHR_FUNC_EXIT;
}
