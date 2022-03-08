/** \file l3_frwrd_kbp_tables.c
 *
 * L3 forwarding KBP tables procedures for DNX
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_L3

/*
 * Include files currently used for DNX.
 * {
 */
#include <bcm_int/dnx/l3/l3_frwrd_tables.h>

/*
 * }
 */
/*
 * Include files.
 * {
 */

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

 /*
  * Return the appropriate result type according to the KBP table
  */
#define L3_KBP_TABLE_TO_RESULT_TYPE_FWD_DEST(table, l3a_flags) ( \
        (table == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD) ? \
             ( _SHR_IS_FLAG_SET(l3a_flags, BCM_L3_ENCAP_SPACE_OPTIMIZED) ? DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_DEST_EEI_W_DEFAULT : \
              DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_KBP_DEST_W_DEFAULT) : \
        (table == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF) ? DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF_KBP_DEST_W_DEFAULT : \
        (table == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD) ? \
             ( _SHR_IS_FLAG_SET(l3a_flags, BCM_L3_ENCAP_SPACE_OPTIMIZED) ? DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_DEST_EEI_W_DEFAULT : \
              DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_KBP_DEST_W_DEFAULT) : \
        (table == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF) ? DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF_KBP_DEST_W_DEFAULT : -1)

/** Define for KBP additional forwarding data size */
#define DNX_L3_MAX_PMF_GENERIC_DATA_VALUE (SAL_BIT(10) - 1)

#define DNX_L3_RESULT_TYPE_IS_KBP_OPTIMIZED(_dbal_table_, _result_type_) \
    (((_dbal_table_ == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD) && (_result_type_ == DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_KBP_DEST_W_DEFAULT)) || \
     ((_dbal_table_ == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD) && (_result_type_ == DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_KBP_DEST_W_DEFAULT)) || \
     ((_dbal_table_ == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF) && (_result_type_ == DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF_KBP_DEST_W_DEFAULT)) || \
     ((_dbal_table_ == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF) && (_result_type_ == DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF_KBP_DEST_W_DEFAULT)) ? TRUE : FALSE)

/*
 * }
 */
#if defined(BCM_DNX2_SUPPORT)
/**
 * \brief
 *   Set the fields in the KBP payload.
 *   KBP requires multiple result types to be used and some result types have different fields that also need to be set.
 * \param [in] unit -
 *   The unit number.
 * \param [in] info -
 *   contains valid fields for LPM Public and Private tables.
 * \param [in] dbal_table -
 *   dbal table
 * \param [in] entry_handle_id -
 *   a handle to the LPM table (public or private based on
 *   the info->l3a_vrf value)
 * \return
 *  * Zero for success
 * \remark
 *   * This procedure assumes a previous procedure (dnx_l3_route_dbal_table_select())
 *     has checked validity:
 * \see
 *  * bcm_dnx_l3_route_add
 *  * bcm_dnx_l3_route_delete
 *  * bcm_dnx_l3_route_get
 */
static shr_error_e
dnx_l3_route_kbp_payload_set(
    int unit,
    bcm_l3_route_t * info,
    uint32 dbal_table,
    uint32 entry_handle_id)
{
    uint32 result_type = -1;
    uint32 remark_profile;
    SHR_FUNC_INIT_VARS(unit);

    if (info->stat_pp_profile == STAT_PP_PROFILE_INVALID)
    {
        result_type = L3_KBP_TABLE_TO_RESULT_TYPE_FWD_DEST(dbal_table, info->l3a_flags);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);

        /** Set EEI if it is used.*/
        if (result_type == DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_DEST_EEI_W_DEFAULT ||
            result_type == DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_DEST_EEI_W_DEFAULT)
        {
            /** Only in JR2 mode, can it get here.*/
            uint32 field_in_struct_encoded_val, child_field_value, eei_val;

            field_in_struct_encoded_val = info->l3a_mpls_label;

            /** In JR2 mode, only swap and pop commands are available. SWAP is using for PUSH for L3 routing into VPN.*/
            remark_profile = DBAL_ENUM_FVAL_SWAP_QOS_PROFILE_DEFAULT;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_EEI_MPLS_MULTI_SWAP, DBAL_FIELD_SWAP_QOS_PROFILE, &remark_profile,
                             &child_field_value));
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_EEI_MPLS_MULTI_SWAP, DBAL_FIELD_MPLS_LABEL,
                             &field_in_struct_encoded_val, &child_field_value));
            SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                            (unit, DBAL_FIELD_EEI, DBAL_FIELD_EEI_MPLS_MULTI_SWAP, &child_field_value, &eei_val));

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EEI, INST_SINGLE, eei_val);
        }
        else
        {
            /** KBP has IS_DEFAULT indication that needs to be set */
            dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IS_DEFAULT, INST_SINGLE,
                                        DNX_L3_ENTRY_IS_DEFAULT(unit, info));
        }
    }
    else
    {
        if (dbal_table == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD)
        {
            result_type = DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_DEST_STAT_W_DEFAULT;
        }
        else if (dbal_table == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD)
        {
            result_type = DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_DEST_STAT_W_DEFAULT;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "The provided dbal table [%s] does not support STAT\n",
                         dbal_logical_table_to_string(unit, dbal_table));
        }
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, info->stat_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE,
                                     info->stat_pp_profile);
    }

    if (L3_TABLE_IS_KBP_RPF(dbal_table))
    {
        /** Set SOURCE DROP indication according to the flag */
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_DROP, INST_SINGLE,
                                    _SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_SRC_DISCARD));
        /** Additional forwarding data is not available for RPF tables */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBP_FORWARD_PMF_GENERIC_DATA, INST_SINGLE, 0);
    }
    else
    {
        /** Set additional forwarding data for FWD tables. */
        if (DNX_L3_RESULT_TYPE_IS_KBP_OPTIMIZED(dbal_table, result_type))
        {
            /** Set the LSB in Source drop */
            dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_DROP, INST_SINGLE,
                                        (info->l3a_lookup_class & 0x1));
            /** Set the remaining MSBs in PMF generic data */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBP_FORWARD_PMF_GENERIC_DATA, INST_SINGLE,
                                         (info->l3a_lookup_class >> 0x1));
        }
        else
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBP_FORWARD_PMF_GENERIC_DATA, INST_SINGLE,
                                         info->l3a_lookup_class);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verify the KBP route entry add/get/delete payload input.
 * \param [in] unit - The unit number.
 * \param [in] info - The l3 route structure to be verified.
 * \param [in] is_kbp_split_rpf_in_use - (KBP only) indication whether RPF tables are split from FWD
 * \param [in] is_replace - Indication to replace an existing entry
 * \param [in] dbal_table - The DBAL table where the entry will be added.
 * \param [out] dest_type - The returned destination type resolved from the DBALtable and key.
 * \return
 *   \retval Zero if no error was encountered
 *   \retval Non-zero in case of an error.
 * \see
 *   * dnx_l3_route_add \n
 */
static shr_error_e
dnx_l3_route_kbp_payload_verify(
    int unit,
    bcm_l3_route_t * info,
    uint8 is_kbp_split_rpf_in_use,
    uint8 is_replace,
    uint32 dbal_table,
    dbal_fields_e * dest_type)
{
    uint8 is_default = 0;
    uint32 fec_index;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "l3_route");
    SHR_NULL_CHECK(dest_type, _SHR_E_PARAM, "dest_type");

    is_default = DNX_L3_ENTRY_IS_DEFAULT(unit, info);
    fec_index = BCM_L3_ITF_VAL_GET(info->l3a_intf);

    /** Additional forwarding data validation */
    if (info->l3a_lookup_class != 0)
    {
        /** Additional forwarding data check for RPF */
        if (_SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_RPF_ONLY))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Additional forwarding data in l3a_lookup_class is not supported with KBP RPF tables\n");
        }
        if (info->l3a_lookup_class > DNX_L3_MAX_PMF_GENERIC_DATA_VALUE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot fit additional forwarding data %d. Max possible value is %d\n",
                         info->l3a_lookup_class, DNX_L3_MAX_PMF_GENERIC_DATA_VALUE);
        }

    }

    if (!_SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_RAW_ENTRY))
    {
        /** Non-raw entries */
        /** KBP entries */

        /** Split RPF entries */
        if (is_kbp_split_rpf_in_use)
        {
            /** No payload entries (BCM_L3_FLAGS2_RPF_ONLY is already verified that it's set) */
            if (_SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_NO_PAYLOAD))
            {
                /** No payload entries are only allowed with FEC index 0 */
                if (fec_index != 0)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_L3_FLAGS2_NO_PAYLOAD is set, but fec index is not 0 (%u)\n",
                                 fec_index);
                }
                if (is_default)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Default entries cannot be added with no payload (BCM_L3_FLAGS2_NO_PAYLOAD is set)\n");
                }
            }
            /** FWD or RPF entries */
            else
            {
                /** Check FEC range */
                if (!DNX_L3_FEC_IS_IN_RANGE(unit, fec_index))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "FEC index %d is out of range, should be bigger than 0 and less than %d.",
                                 fec_index, dnx_data_l3.fec.nof_fecs_get(unit));
                }
            }
        }
        /** Non-split RPF entries */
        else
        {
            /** Check FEC range */
            if (!DNX_L3_FEC_IS_IN_RANGE(unit, fec_index))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "FEC index %d is out of range, should be bigger than 0 and less than %d.", fec_index,
                             dnx_data_l3.fec.nof_fecs_get(unit));
            }
        }

        /** Statistics check */
        if ((info->stat_pp_profile != STAT_PP_PROFILE_INVALID))
        {
            if (info->stat_id > L3_FRWRD_TABLES_MAX_STAT_ID_VALUE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Stat id %d, max value 0x%x", info->stat_id,
                             L3_FRWRD_TABLES_MAX_STAT_ID_VALUE);
            }
            SHR_IF_ERR_EXIT(dnx_stat_pp_crps_verify
                            (unit, BCM_CORE_ALL, info->stat_id, info->stat_pp_profile,
                             bcmStatCounterInterfaceIngressReceivePp));
        }

        *dest_type = DBAL_FIELD_FEC;

    }
    else
    {
        /*
         * If 'raw' entry is required then specify the whole field.
         * This indicates 'no encoding'.
         */
        *dest_type = DBAL_FIELD_DESTINATION;

        SHR_IF_ERR_EXIT(dnx_l3_route_validate_result_by_fields_size(unit, dbal_table, dest_type, fec_index));

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Determines in which table the KBP L3 route entry belongs.
 * \param [in] unit - The unit number.
 * \param [in] info - contains valid fields for determining correct table.
 *      - info->l3a_vrf -
 *          Virtual Router Instance - valid for Private table
 *      - info->l3a_flags -
 *          control flags (eg. BCM_L3_IP6)
 *      - info->l3a_flags2 -
 *          control flags (Currently, using BCM_L3_FLAGS2_RAW_ENTRY)
 * \param [in] is_kbp_split_rpf_in_use - indication whether RPF tables are split from FWD
 * \param [out] dbal_table - returns the name of the dbal table in which the entry will be added
 * \return
 *  * Zero for success
 * \remark
 *    * Currently, it is not allowed to specify BCM_L3_FLAGS2_RAW_ENTRY for KBP. An
 *      error is returned in this case.
 * \see
 *  * bcm_dnx_l3_route_add
 *  * bcm_dnx_l3_route_delete
 *  * bcm_dnx_l3_route_get
 */
static shr_error_e
dnx_l3_route_kbp_dbal_table_select(
    int unit,
    bcm_l3_route_t * info,
    uint8 is_kbp_split_rpf_in_use,
    uint32 *dbal_table)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * If flag BCM_L3_IP6 is set, then the entry will be looked for in the KBP_IPV6_UNICAST_*_LPM_* table.
     * Valid fields for it are VRF and IPv6 address(l3a_ip6_net) and its mask(l3a_ip6_mask).
     *
     * If flag BCM_L3_IP6 is NOT set, then the entry will be looked for in the KBP_IPV4_UNICAST_*_LPM_* table.
     * Valid fields for it are VRF and IPv4 address(l3a_subnet) and its mask(l3a_ip_mask).
     */
    if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
    {
        /*
         * Get an handle to the KBP table for IPv6 route entries.
         * Public entries are not available.
         */
        {
            if (is_kbp_split_rpf_in_use)
            {
                if (_SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_FWD_ONLY))
                {
                    *dbal_table = DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD;
                }
                else
                {
                    *dbal_table = DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF;
                }
            }
            else
            {
                *dbal_table = DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD;
            }
        }
    }
    else
    {
        /*
         * Get an handle to the KBP table for IPv4 route entries.
         * Public entries are not available.
         */
        {
            if (is_kbp_split_rpf_in_use)
            {
                if (_SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_FWD_ONLY))
                {
                    *dbal_table = DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD;
                }
                else
                {
                    *dbal_table = DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF;
                }
            }
            else
            {
                *dbal_table = DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD;
            }
        }
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verify the KBP route entry add/get/delete flags input.
 *  Checks the use of KBP-only flags.
 * \param [in] unit - The unit number.
 * \param [in] info - The l3 route structure to be verified.
 * \param [in] is_kbp_split_rpf_in_use - indication whether RPF tables are split from FWD
 * \return
 *   \retval Zero if no error was encountered
 *   \retval Non-zero in case of an error.
 * \see
 *   * dnx_l3_route_add_verify \n
 *   * dnx_l3_route_get \n
 *   * dnx_l3_route_delete \n
 */
static shr_error_e
dnx_l3_route_kbp_flags_verify(
    int unit,
    bcm_l3_route_t * info,
    uint8 is_kbp_split_rpf_in_use)
{
    dbal_enum_value_field_system_headers_mode_e system_headers_mode;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify supported KBP flags */
    if (_SHR_IS_FLAG_SET(info->l3a_flags, ~dnx_data_l3.fwd.uc_supported_kbp_route_flags_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal L3 flags are passed to bcm_l3_route_* API - %08lX",
                     (unsigned long) (info->l3a_flags));
    }

    /** Verify supported KBP flags2 */
    if (_SHR_IS_FLAG_SET(info->l3a_flags2, ~dnx_data_l3.fwd.uc_supported_kbp_route_flags2_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal L3 flags2 are passed to bcm_l3_route_* API - %08lX",
                     (unsigned long) (info->l3a_flags2));
    }

    if (is_kbp_split_rpf_in_use)
    {
        if (!_SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_FWD_ONLY)
            && !_SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_RPF_ONLY))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "KBP route APIs in split RPF mode should be used with one of the flags BCM_L3_FLAGS2_FWD_ONLY or BCM_L3_FLAGS2_RPF_ONLY\n");
        }

        if (_SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_FWD_ONLY)
            && _SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_RPF_ONLY))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "KBP route APIs in split RPF mode should use only one of the flags BCM_L3_FLAGS2_FWD_ONLY and BCM_L3_FLAGS2_RPF_ONLY, but not both\n");
        }

        if (_SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_RPF_ONLY))
        {
            if (_SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_NO_PAYLOAD)
                && _SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_SRC_DISCARD))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "KBP RPF routes can use only one of the flags BCM_L3_FLAGS2_NO_PAYLOAD and BCM_L3_SRC_DISCARD, but not both\n");
            }
            if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_ENCAP_SPACE_OPTIMIZED))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "KBP route flag BCM_L3_ENCAP_SPACE_OPTIMIZED is not supported for KBP RPF entries\n");
            }
            if ((info->stat_pp_profile != STAT_PP_PROFILE_INVALID))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Statistics are not supported for KBP RPF entries\n");
            }
        }
        else
        {
            if (_SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_NO_PAYLOAD))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "KBP route flag BCM_L3_FLAGS2_NO_PAYLOAD is only supported for RPF tables\n");
            }
            if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_SRC_DISCARD))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "KBP route flag BCM_L3_SRC_DISCARD is only supported for RPF tables\n");
            }
        }
    }
    else
    {
        if (_SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_FWD_ONLY)
            || _SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_RPF_ONLY)
            || _SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_NO_PAYLOAD))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "KBP route APIs in non-split RPF mode do not allow the use of the flags (BCM_L3_FLAGS2_FWD_ONLY and/or BCM_L3_FLAGS2_RPF_ONLY and/or BCM_L3_FLAGS2_NO_PAYLOAD)\n");
        }
    }

    if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_ENCAP_SPACE_OPTIMIZED))
    {
        if ((info->stat_pp_profile != STAT_PP_PROFILE_INVALID))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "KBP route flag BCM_L3_ENCAP_SPACE_OPTIMIZED is not supported if statistics is required\n");
        }
        if (!DNX_MPLS_LABEL_IN_RANGE(unit, info->l3a_mpls_label))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "KBP route flag BCM_L3_ENCAP_SPACE_OPTIMIZED is provided, but mpls label provided(%d) is invalid\n",
                         info->l3a_mpls_label);
        }

        system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
        if (system_headers_mode != DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
        {
            /** If support is required, for _replace need to get the existing entry for old push_profile first.*/
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "BCM_L3_ENCAP_SPACE_OPTIMIZED flag is not supported in Jericho1 inter-op mode currently!\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get the result fields from the payload.
 * \param [in] unit -
 *   The unit number.
 * \param [in] info -
 *   contains valid fields for LPM Public and Private tables.
 * \param [in] is_kbp_split_rpf_in_use - (KBP only) indication whether RPF tables are split from FWD
 * \param [in] dbal_table -
 *   dbal table
 * \param [in] entry_handle_id -
 *   a handle to the LPM table (public or private based on
 *   the info->l3a_vrf value)
 * \return
 *  * Zero for success
 * \remark
 *   * This procedure assumes a previous procedure (dnx_l3_route_dbal_table_select())
 *     has checked validity:
 * \see
 *  * bcm_dnx_l3_route_add
 *  * bcm_dnx_l3_route_delete
 *  * bcm_dnx_l3_route_get
 */
static shr_error_e
dnx_l3_route_kbp_payload_get(
    int unit,
    bcm_l3_route_t * info,
    uint8 is_kbp_split_rpf_in_use,
    uint32 dbal_table,
    uint32 entry_handle_id)
{
    dbal_fields_e dest_type, dbal_sub_field;
    uint32 result_value, destination_val;
    uint8 source_drop = FALSE;
    uint32 result_type;
    uint32 is_default;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "l3_route");

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    /** Get entry result type */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &result_type));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, &destination_val));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_IS_DEFAULT, INST_SINGLE, &is_default));

    SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                    (unit, DBAL_FIELD_DESTINATION, destination_val, &dbal_sub_field, &result_value));

    /** In case of an entry with stats - retrieve stats configuration values */
    if (dnx_stat_pp_result_type_verify(unit, dbal_table, result_type) == _SHR_E_NONE)
    {
        uint32 stat_pp_profile;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD,
                                                            INST_SINGLE, &stat_pp_profile));

        /** Get statistic info only if stat profile is valid */
        if (stat_pp_profile != STAT_PP_PROFILE_INVALID)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id,
                                                                DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE,
                                                                &info->stat_id));

            STAT_PP_ENGINE_PROFILE_SET(info->stat_pp_profile, stat_pp_profile, bcmStatCounterInterfaceIngressReceivePp);
        }
    }
    /** If EEI is used.*/
    else if ((result_type == DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_DEST_EEI_W_DEFAULT)
             || (result_type == DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_DEST_EEI_W_DEFAULT))
    {
        uint32 eei_val, sub_field_id, sub_field_val, mpls_val;

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_EEI, INST_SINGLE, &eei_val));
        SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                        (unit, DBAL_FIELD_EEI, eei_val, &sub_field_id, &sub_field_val));

        if (sub_field_id == DBAL_FIELD_EEI_MPLS_MULTI_SWAP)
        {
            uint32 remark_profile;

            SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                            (unit, DBAL_FIELD_EEI_MPLS_MULTI_SWAP, DBAL_FIELD_SWAP_QOS_PROFILE,
                             &remark_profile, &sub_field_val));
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                            (unit, DBAL_FIELD_EEI_MPLS_MULTI_SWAP, DBAL_FIELD_MPLS_LABEL, &mpls_val, &sub_field_val));
            info->l3a_flags |= BCM_L3_ENCAP_SPACE_OPTIMIZED;
            info->l3a_mpls_label = mpls_val;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal EEI format (%s) is found in route table.\n",
                         dbal_field_to_string(unit, sub_field_id));
        }
    }
    else
    {
        /** Source drop field is available only for RPF tables in KBP_FORWARD_DEST_W_DEFAULT result */
        if (L3_TABLE_IS_KBP_RPF(dbal_table))
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                            (unit, entry_handle_id, DBAL_FIELD_SOURCE_DROP, INST_SINGLE, &source_drop));

            if (source_drop)
            {
                info->l3a_flags |= BCM_L3_SRC_DISCARD;
            }
        }
    }

    /** Set the NO_PAYLOAD flags for RPF databases when the payload data is all zeros */
    if (is_kbp_split_rpf_in_use && _SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_RPF_ONLY) &&
        !source_drop && !is_default && !destination_val)
    {
        info->l3a_flags2 |= BCM_L3_FLAGS2_NO_PAYLOAD;
    }

    /** Get additional forwarding data only for non-RPF tables */
    if (!L3_TABLE_IS_KBP_RPF(dbal_table))
    {
        uint32 data_value = 0;

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_KBP_FORWARD_PMF_GENERIC_DATA, INST_SINGLE, &data_value));

        if (DNX_L3_RESULT_TYPE_IS_KBP_OPTIMIZED(dbal_table, result_type))
        {
            uint8 source_drop_value = 0;

            /** Get the LSB from Source drop */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                            (unit, entry_handle_id, DBAL_FIELD_SOURCE_DROP, INST_SINGLE, &source_drop_value));

            /** Shift the taken PMF generic data value and place the Source drop value in the LSB */
            data_value = ((data_value << 0x1) | (source_drop_value & 0x1));
        }

        info->l3a_lookup_class = data_value;
    }

    /** Set the expected destination type */
    dest_type = DBAL_FIELD_FEC;

    /** Resolve raw payload and assign the relevant payload value to l3a_intf. */
    SHR_IF_ERR_EXIT(dnx_l3_route_payload_raw_resolve_get
                    (unit, info, TRUE, dest_type, dbal_sub_field, result_value, destination_val));

exit:
    SHR_FUNC_EXIT;
}

/* see .h file */
shr_error_e
dnx_l3_kbp_route_delete(
    int unit,
    bcm_l3_route_t * info)
{
    uint32 entry_handle_id;
    uint32 dbal_table;
    uint8 is_kbp_split_rpf_in_use;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    is_kbp_split_rpf_in_use = kbp_mngr_split_rpf_in_use(unit);

    /** Verify flags */
    SHR_IF_ERR_EXIT(dnx_l3_route_kbp_flags_verify(unit, info, is_kbp_split_rpf_in_use));
    /** Verify key */
    SHR_IF_ERR_EXIT(dnx_l3_route_key_verify(unit, info));

    /** Select DBAL table */
    SHR_IF_ERR_EXIT(dnx_l3_route_kbp_dbal_table_select(unit, info, is_kbp_split_rpf_in_use, &dbal_table));

    /** Allocate DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    /** Set the key */
    SHR_IF_ERR_EXIT(dnx_l3_route_key_set(unit, info, entry_handle_id));

    /** Clear the entry */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* see .h file */
shr_error_e
dnx_l3_kbp_route_get(
    int unit,
    bcm_l3_route_t * info)
{
    uint32 entry_handle_id;
    uint32 dbal_table;
    uint8 is_kbp_split_rpf_in_use;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    is_kbp_split_rpf_in_use = kbp_mngr_split_rpf_in_use(unit);

    /** Verify flags */
    SHR_IF_ERR_EXIT(dnx_l3_route_kbp_flags_verify(unit, info, is_kbp_split_rpf_in_use));
    /** Verify key */
    SHR_IF_ERR_EXIT(dnx_l3_route_key_verify(unit, info));

    /*
     * Make sure that the output parameter is 0 before receiving data.
     * In case of an error receiving the result value, output value will be 0.
     */
    BCM_L3_ITF_SET(info->l3a_intf, BCM_L3_ITF_TYPE_FEC, 0);

    /** Select DBAL table */
    SHR_IF_ERR_EXIT(dnx_l3_route_kbp_dbal_table_select(unit, info, is_kbp_split_rpf_in_use, &dbal_table));

    /** Allocate DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    /** Set the key */
    SHR_IF_ERR_EXIT(dnx_l3_route_key_set(unit, info, entry_handle_id));

    SHR_IF_ERR_EXIT_NO_MSG(dnx_l3_route_kbp_payload_get
                           (unit, info, is_kbp_split_rpf_in_use, dbal_table, entry_handle_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* see .h file */
shr_error_e
dnx_l3_kbp_route_add(
    int unit,
    bcm_l3_route_t * info)
{
    dbal_fields_e dest_type = DBAL_FIELD_EMPTY;
    uint32 entry_handle_id;
    uint32 l3_intf;
    uint32 dbal_table;
    uint8 is_kbp_split_rpf_in_use;
    uint8 is_replace;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    l3_intf = BCM_L3_ITF_VAL_GET(info->l3a_intf);
    is_kbp_split_rpf_in_use = kbp_mngr_split_rpf_in_use(unit);
    is_replace = _SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_REPLACE);

    /** Verify flags */
    SHR_IF_ERR_EXIT(dnx_l3_route_kbp_flags_verify(unit, info, is_kbp_split_rpf_in_use));
    /** Verify key */
    SHR_IF_ERR_EXIT(dnx_l3_route_key_verify(unit, info));

    /** Select DBAL table */
    SHR_IF_ERR_EXIT(dnx_l3_route_kbp_dbal_table_select(unit, info, is_kbp_split_rpf_in_use, &dbal_table));

    /** Allocate DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    /** Set the key */
    SHR_IF_ERR_EXIT(dnx_l3_route_key_set(unit, info, entry_handle_id));

    /** Verify payload */
    SHR_IF_ERR_EXIT(dnx_l3_route_kbp_payload_verify
                    (unit, info, is_kbp_split_rpf_in_use, is_replace, dbal_table, &dest_type));
    /*
     * KBP is using multiple result types that need to be set.
     * Some result types have additional result fields that also need to be set.
     */
    SHR_IF_ERR_EXIT(dnx_l3_route_kbp_payload_set(unit, info, dbal_table, entry_handle_id));

    /** Set the destination */
    dbal_entry_value_field32_set(unit, entry_handle_id, dest_type, INST_SINGLE, l3_intf);

    /** Commit the entry. Create it if it doesn't exist, update it if it does. */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, l3_frwrd_table_commit_actions[is_replace]));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
#endif
