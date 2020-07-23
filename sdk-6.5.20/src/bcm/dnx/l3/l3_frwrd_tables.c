/** \file l3_frwrd_tables.c
 *
 * L3 forwarding tables procedures for DNX
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
 * Include files currently used for DNX.
 * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/mdb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_elk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mpls.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/stat/stat_pp.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <bcm_int/dnx/mpls/mpls.h>
#include <bcm_int/dnx/switch/switch.h>

/*
 * }
 */
/*
 * Include files.
 * {
 */
#include <bcm/types.h>
#include <bcm/l3.h>
/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/*
 * Maximum number of route tables - this number includes private and public table.
 */
#define MAX_NOF_L3_ROUTE_TABLES 6

/**stat_id is 17 bits long */
#define L3_FRWRD_TABLES_MAX_STAT_ID_VALUE 0x1FFFF

/*
 * }
 */

/*
 * MACROs
 * {
 */
/*
 * Indicates whether the input address (addr) is an IPV4 unicast address (does not have a 4 bit prefix of 0xe)
 */
#define L3_IPV4_ADDRESS_IS_UNICAST(addr) (((addr) & 0xf0000000) != 0xe0000000)
/*
 * Indicates whether the input address (addr) is an IPV6 unicast address (first byte is not 0xff)
 */
#define L3_IPV6_ADDRESS_IS_UNICAST(addr) ((addr[0]) != 0xff)
/*
 * Whether an IPv4 entry is default or not, relevant only for route entries.
 */
#define L3_FRWRD_TABLES_IS_IPV4_DEFAULT_ENTRY(ip_mask) (ip_mask == 0)

/*
 * Whether an IPv6 entry is default or not, relevant only for route entries.
 * Assuming that the mask is valid if the first byte of the mask is 0 then all the other bytes should
 * be as well hence this is a default route.
 */
#define L3_FRWRD_TABLES_IS_IPV6_DEFAULT_ENTRY(ip_mask) (ip_mask[0] == 0)
/*
 * Define the list of supported flags for L3 APIs
 */
#define L3_SUPPORTED_BCM_L3_FLAGS (BCM_L3_IP6 | BCM_L3_REPLACE | BCM_L3_HOST_LOCAL | BCM_L3_ENCAP_SPACE_OPTIMIZED | BCM_L3_HIT_CLEAR | BCM_L3_HIT | BCM_L3_SRC_DISCARD | BCM_L3_INTERNAL_ROUTE)
/** Define the list of supported BCM_L3_FLAGS2_* flags for L3 APIs. */
#define L3_FORWARD_SUPPORTED_BCM_L3_FLAGS2 (BCM_L3_FLAGS2_RAW_ENTRY | BCM_L3_FLAGS2_FWD_ONLY | BCM_L3_FLAGS2_RPF_ONLY | BCM_L3_FLAGS2_SCALE_ROUTE | BCM_L3_FLAGS2_NO_PAYLOAD)
/*
 * Indicates whether the table is KBP
 */

#define L3_TABLE_IS_KBP(table) ( \
        (table == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD) || \
        (table == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF) || \
        (table == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD) || \
        (table == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF))
/*
 * Indicates whether the table is RPF
 */

#define L3_TABLE_IS_RPF(table) ( \
        (table == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF) || \
        (table == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF))
/*
 * Indicates whether the table is KBP IPV4
 */

#define L3_TABLE_IS_KBP_IPV4(table) ( \
        (table == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD) || \
        (table == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF))
/*
 * Indicates whether the table is KBP IPV6
 */

#define L3_TABLE_IS_KBP_IPV6(table) ( \
        (table == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD) || \
        (table == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF))
/*
 * Indicates whether the table is public
 */

#define L3_TABLE_IS_PUBLIC(table) ( \
        (table == DBAL_TABLE_IPV6_UNICAST_PRIVATE_LPM_FORWARD_2)     || \
        (table == DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD_2))

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

/** Indicate whether KBP is enabled for IPV4 and IPv6 route entries */
#define DNX_L3_KBP_IS_ENABLED(unit, flags) (((_SHR_IS_FLAG_SET(flags, BCM_L3_IP6) \
          && (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv6)))) \
        || (!_SHR_IS_FLAG_SET(flags, BCM_L3_IP6) && (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4))))

/** Determine whether the FEC ID is in the valid range */
#define DNX_L3_FEC_IS_IN_RANGE(unit, fec_index) (fec_index < dnx_data_l3.fec.nof_fecs_get(unit) && fec_index >= dnx_data_l3.fec.first_valid_fec_ecmp_id_get(unit))
#define DNX_L3_DEFAULT_FEC_IS_IN_RANGE(unit, fec_index) (fec_index <= dnx_data_l3.fec.max_default_fec_get(unit) && fec_index >= dnx_data_l3.fec.first_valid_fec_ecmp_id_get(unit))

/** Indicate whether the entry is supposed to be added to KBP */
#define DNX_L3_ENTRY_IS_FOR_KBP(_unit_, _info_) \
    (((_SHR_IS_FLAG_SET(_info_->l3a_flags, BCM_L3_IP6) && dnx_data_elk.application.feature_get(_unit_, dnx_data_elk_application_ipv6)) || \
     (!_SHR_IS_FLAG_SET(_info_->l3a_flags, BCM_L3_IP6) && dnx_data_elk.application.feature_get(_unit_, dnx_data_elk_application_ipv4))) && \
     !_SHR_IS_FLAG_SET(_info_->l3a_flags2, BCM_L3_FLAGS2_SCALE_ROUTE) && \
     !_SHR_IS_FLAG_SET(_info_->l3a_flags, BCM_L3_INTERNAL_ROUTE))

#define DNX_L3_ENTRY_IS_DEFAULT(_info_) \
    ((!_SHR_IS_FLAG_SET(_info_->l3a_flags, BCM_L3_IP6) && L3_FRWRD_TABLES_IS_IPV4_DEFAULT_ENTRY(_info_->l3a_ip_mask)) || \
     (_SHR_IS_FLAG_SET(_info_->l3a_flags, BCM_L3_IP6) && L3_FRWRD_TABLES_IS_IPV6_DEFAULT_ENTRY(_info_->l3a_ip6_mask)))

/** Defines for KBP additional forwarding data size */
#define DNX_L3_MAX_PMF_GENERIC_DATA_VALUE                       (SAL_BIT(4) - 1)
#define DNX_L3_MAX_PMF_GENERIC_DATA_VALUE_FOR_OPTIMIZED_RESULT  (SAL_BIT(2) - 1)

#define DNX_L3_RESULT_TYPE_IS_KBP_OPTIMIZED(_dbal_table_, _result_type_) \
    (((_dbal_table_ == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD) && (_result_type_ == DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_KBP_DEST_W_DEFAULT)) || \
     ((_dbal_table_ == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD) && (_result_type_ == DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_KBP_DEST_W_DEFAULT)) || \
     ((_dbal_table_ == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF) && (_result_type_ == DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF_KBP_DEST_W_DEFAULT)) || \
     ((_dbal_table_ == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF) && (_result_type_ == DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF_KBP_DEST_W_DEFAULT)) ? TRUE : FALSE)

/*
 * }
 */

/**
 * \brief
 *   Determines in which table the L3 route entry belongs.
 * \param [in] unit - The unit number.
 * \param [in] info - contains valid fields for determining correct table.
 *      - info->l3a_vrf -
 *          Virtual Router Instance - valid for Private table
 *      - info->l3a_flags -
 *          control flags (eg. BCM_L3_IP6)
 *      - info->l3a_flags2 -
 *          control flags (Currently, using BCM_L3_FLAGS2_RAW_ENTRY)
 * \param [in] is_kbp_entry - indication whether the entry is for KBP or for KAPS
 * \param [in] is_kbp_split_rpf_in_use - (KBP only) indication whether RPF tables are split from FWD
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
dnx_l3_route_dbal_table_select(
    int unit,
    bcm_l3_route_t * info,
    uint8 is_kbp_entry,
    uint8 is_kbp_split_rpf_in_use,
    uint32 *dbal_table)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Select KAPS table */
    if (!is_kbp_entry)
    {
        int kaps_public_capacity = 0;

        /** Get KAPS_2 capacity */
        if ((info->l3a_vrf == 0) || _SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_SCALE_ROUTE))
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_KAPS_2, &kaps_public_capacity));
        }

        if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
        {
            /*
             * Get a handle to the LPM table for IPv6 route entries.
             * In case the VRF > 0 or a single DB check is defined a private table is used and
             * the VRF is part of the key which is formed by <VRF, DIP>.
             * In the case of VRF = 0 the public route table is used where the key is composed only using the DIP.
             */
            if (kaps_public_capacity == 0)
            {
                *dbal_table = DBAL_TABLE_IPV6_UNICAST_PRIVATE_LPM_FORWARD;
            }
            else
            {
                if ((info->l3a_vrf == 0) || (info->l3a_flags2 & BCM_L3_FLAGS2_SCALE_ROUTE))
                {
                    *dbal_table = DBAL_TABLE_IPV6_UNICAST_PRIVATE_LPM_FORWARD_2;
                }
                else
                {
                    *dbal_table = DBAL_TABLE_IPV6_UNICAST_PRIVATE_LPM_FORWARD;
                }
            }
        }
        else
        {
            /*
             * Get an handle to the LPM table for IPv4 route entries.
             *
             * In case the VRF > 0 or only a private table is used and the entry is private.
             * Here the VRF is part of the key which is formed by VRF, DIP.
             * When the VRF = 0, the public table is being used.
             * The VRF is not part of the key which is composed only from the DIP.
             */
            if (kaps_public_capacity == 0)
            {
                *dbal_table = DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD;
            }
            else
            {
                if (info->l3a_vrf == 0 || _SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_SCALE_ROUTE))
                {
                    *dbal_table = DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD_2;
                }
                else
                {
                    *dbal_table = DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD;
                }
            }
        }
    }
    /** Select KBP table */
    else
    {
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
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verify the route entry add/get/delete flags input.
 *  Checks the use of KBP-only flags.
 * \param [in] unit - The unit number.
 * \param [in] info - The l3 route structure to be verified.
 * \param [in] is_kbp_entry - indication whether the entry is for KBP or for KAPS
 * \param [in] is_kbp_split_rpf_in_use - (KBP only) indication whether RPF tables are split from FWD
 * \return
 *   \retval Zero if no error was encountered
 *   \retval Non-zero in case of an error.
 * \see
 *   * dnx_l3_route_add_verify \n
 *   * dnx_l3_route_get \n
 *   * dnx_l3_route_delete \n
 */
static shr_error_e
dnx_l3_route_flags_verify(
    int unit,
    bcm_l3_route_t * info,
    uint8 is_kbp_entry,
    uint8 is_kbp_split_rpf_in_use)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify supported flags */
    if (_SHR_IS_FLAG_SET(info->l3a_flags, ~L3_SUPPORTED_BCM_L3_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal L3 flags are passed to bcm_l3_route_* API - %08lX",
                     (unsigned long) (info->l3a_flags2));
    }
    /** Verify supported flags2 */
    if (_SHR_IS_FLAG_SET(info->l3a_flags2, ~L3_FORWARD_SUPPORTED_BCM_L3_FLAGS2))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal L3 flags2 are passed to bcm_l3_route_* API - %08lX",
                     (unsigned long) (info->l3a_flags2));
    }

    /** Validate KBP-only flags */
    if (is_kbp_entry)
    {
        uint32 kbp_mngr_status;
        dbal_enum_value_field_system_headers_mode_e system_headers_mode;

        /** Get the KBP device status */
        SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));
        if ((kbp_mngr_status != DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "KBP device in use, but not locked\n");
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
    }
    /** Validate KBP-only flags are not used without KBP */
    else
    {
        if (_SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_FWD_ONLY)
            || _SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_RPF_ONLY)
            || _SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_NO_PAYLOAD)
            || _SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_SRC_DISCARD)
            || _SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_ENCAP_SPACE_OPTIMIZED))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "KBP only flags (BCM_L3_FLAGS2_FWD_ONLY, BCM_L3_FLAGS2_RPF_ONLY, BCM_L3_FLAGS2_NO_PAYLOAD, BCM_L3_SRC_DISCARD, BCM_L3_ENCAP_SPACE_OPTIMIZED) are used without KBP\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verify the route entry add/get/delete key input.
 * \param [in] unit - The unit number.
 * \param [in] info - The l3 route structure to be verified.
 * \return
 *   \retval Zero if no error was encountered
 *   \retval Non-zero in case of an error.
 * \see
 *   * dnx_l3_route_add_verify \n
 *   * dnx_l3_route_get \n
 *   * dnx_l3_route_delete \n
 */
static shr_error_e
dnx_l3_route_key_verify(
    int unit,
    bcm_l3_route_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify the the VRF is in range. */
    if (info->l3a_vrf >= dnx_data_l3.vrf.nof_vrf_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "VRF %d is out of range, should be lower then %d.", info->l3a_vrf,
                     dnx_data_l3.vrf.nof_vrf_get(unit));
    }
    /*
     * Verify that the IP address is a unicast address.
     * If BCM_L3_IP6 flag is set, the field holding information about the IPv6 address (l3a_ip6_net) will be checked.
     * If the flag is not set, the field holding the IPv4 address (l3a_subnet) will be checked.
     */
    if (!L3_IPV4_ADDRESS_IS_UNICAST(info->l3a_subnet) && !_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "IPv4 address %d provided in l3a_ip_addr field is not a unicast address.",
                     info->l3a_subnet);
    }
    else if (!L3_IPV6_ADDRESS_IS_UNICAST(info->l3a_ip6_net) && _SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "IPv6 address, provided in l3a_ip6_addr field, is not a unicast address.");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verify the route entry add/get/delete payload input.
 * \param [in] unit - The unit number.
 * \param [in] info - The l3 route structure to be verified.
 * \param [in] is_kbp_entry - indication whether the entry is for KBP or for KAPS
 * \param [in] is_kbp_split_rpf_in_use - (KBP only) indication whether RPF tables are split from FWD
 * \param [in] dbal_table - The DBAL table where the entry will be added.
 * \param [out] dest_type - The returned destination type resolved from the DBALtable and key.
 * \return
 *   \retval Zero if no error was encountered
 *   \retval Non-zero in case of an error.
 * \see
 *   * dnx_l3_route_add \n
 */
static shr_error_e
dnx_l3_route_payload_verify(
    int unit,
    bcm_l3_route_t * info,
    uint8 is_kbp_entry,
    uint8 is_kbp_split_rpf_in_use,
    uint32 dbal_table,
    dbal_fields_e * dest_type)
{
    uint32 fec_index;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "l3_route");
    SHR_NULL_CHECK(dest_type, _SHR_E_PARAM, "dest_type");

    fec_index = BCM_L3_ITF_VAL_GET(info->l3a_intf);

    /** Additional forwarding data validation */
    if (info->l3a_lookup_class != 0)
    {
        if (!is_kbp_entry)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Additional forwarding data in l3a_lookup_class is only supported with KBP tables\n");
        }
        else
        {
            /** Additional forwarding data check for RPF */
            if (_SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_RPF_ONLY) && (info->l3a_lookup_class != 0))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Additional forwarding data in l3a_lookup_class is not supported with KBP RPF tables\n");
            }
            if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_ENCAP_SPACE_OPTIMIZED) ||
                (info->stat_pp_profile != STAT_PP_PROFILE_INVALID))
            {
                /** Result type is non-optimized */
                if (info->l3a_lookup_class > DNX_L3_MAX_PMF_GENERIC_DATA_VALUE)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot fit additional forwarding data %d. Max possible value is %d\n",
                                 info->l3a_lookup_class, DNX_L3_MAX_PMF_GENERIC_DATA_VALUE);
                }
            }
            else
            {
                /** Result type is optimized */
                if (info->l3a_lookup_class > DNX_L3_MAX_PMF_GENERIC_DATA_VALUE_FOR_OPTIMIZED_RESULT)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot fit additional forwarding data %d. Max possible value is %d\n",
                                 info->l3a_lookup_class, DNX_L3_MAX_PMF_GENERIC_DATA_VALUE_FOR_OPTIMIZED_RESULT);
                }
            }
        }
    }

    if (!_SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_RAW_ENTRY))
    {
        /** Non-raw entries */
        if (!is_kbp_entry)
        {
            /** KAPS entries */

            /** Non-default entries */
            if (!DNX_L3_ENTRY_IS_DEFAULT(info))
            {
                /** Check FEC range */
                if (!DNX_L3_FEC_IS_IN_RANGE(unit, fec_index))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "FEC index %d is out of range, should be bigger than 0 and smaller than %d.",
                                 fec_index, dnx_data_l3.fec.nof_fecs_get(unit));
                }
                *dest_type = DBAL_FIELD_FEC;
            }
            /** Default entries */
            else
            {
                /** FECs that are used for default entry has a different range of values. */
                if (!DNX_L3_DEFAULT_FEC_IS_IN_RANGE(unit, fec_index))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Default FEC index %d is out of range, should be bigger than 0 smaller then %d.",
                                 fec_index, dnx_data_l3.fec.max_default_fec_get(unit) + 1);
                }
                *dest_type = DBAL_FIELD_FEC_DEFAULT;
            }

            /** Statistics check */
            if ((info->stat_pp_profile != STAT_PP_PROFILE_INVALID))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Stat profile is not supported with LPM tables\n");
            }

            /** Additional forwarding data check */
            if (info->l3a_lookup_class != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Additional forwarding data in l3a_lookup_class is only supported with KBP tables\n");
            }
        }
        else
        {
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
                    if (DNX_L3_ENTRY_IS_DEFAULT(info))
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
    }
    else
    {
        uint32 field_size;
        int table_field_size;

        /*
         * If 'raw' entry is required then specify the whole field - KAPS_RESULT for KAPS and DESTINATION for KBP.
         * This indicates 'no encoding'.
         */
        *dest_type = (is_kbp_entry ? DBAL_FIELD_DESTINATION : DBAL_FIELD_KAPS_RESULT);

        /*
         * Validate the result size using the field size (KAPS_RESULT 20 bits; DESTINATION 21 bits).
         * Also check the table field size in case it's truncated.
         */
        SHR_IF_ERR_EXIT(dbal_fields_max_size_get(unit, *dest_type, &field_size));
        SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                        (unit, dbal_table, *dest_type, FALSE, 0, INST_SINGLE, &table_field_size));
        /** Take the smaller of the two sizes */
        field_size = (field_size > table_field_size ? table_field_size : field_size);
        if (fec_index >= SAL_BIT(field_size))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Raw value for 'FEC' (%d) is out of range, should be lower than %d.",
                         fec_index, SAL_BIT(field_size));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set the fields in the key for a UC LMP route table entry.
 * \param [in] unit -
 *   The unit number.
 * \param [in] info -
 *   contains valid fields for LPM Public and Private tables.
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
dnx_l3_route_key_set(
    int unit,
    bcm_l3_route_t * info,
    uint32 entry_handle_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Setting key fields:
     * If flag BCM_L3_IP6 is set, then valid fields for the entry are l3a_ip6_net, its mask (l3a_ip6_mask).
     * They are valid for IPv6 entries.
     * If the flag is not set, then an IPv4 entry fiels are valid - l3a_subnet, l3a_ip_mask.
     * VRF value (l3a_vrf) is valid for both IPv4 and IPv6 entries as long as they are private table entries.
     */
    if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
    {
        dbal_entry_key_field_arr8_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV6, info->l3a_ip6_net,
                                             info->l3a_ip6_mask);
    }
    else
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV4, info->l3a_subnet, info->l3a_ip_mask);
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VRF, info->l3a_vrf);

    SHR_FUNC_EXIT;
}

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
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_EEI_MPLS_SWAP_COMMAND, DBAL_FIELD_MPLS_LABEL,
                             &field_in_struct_encoded_val, &child_field_value));
            SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                            (unit, DBAL_FIELD_EEI, DBAL_FIELD_EEI_MPLS_SWAP_COMMAND, &child_field_value, &eei_val));

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EEI, INST_SINGLE, eei_val);
        }
        else
        {
            /** KBP has IS_DEFAULT indication that needs to be set */
            dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IS_DEFAULT, INST_SINGLE,
                                        DNX_L3_ENTRY_IS_DEFAULT(info));
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

    if (L3_TABLE_IS_RPF(dbal_table))
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
 *   Resolve the retrieved payload to raw or encoded and set it to l3a_intf.
 * \param [in] unit - The unit number.
 * \param [in] info - contains valid fields for LPM Public and Private tables.
 * \param [in] is_kbp_entry - indication whether the entry is for KBP or for KAPS
 * \param [in] dest_type - the destination field type (KAPS_RESULT for KAPS or DESTINATION for KBP)
 * \param [in] dbal_sub_field - the encoding of the destination field type
 * \param [in] encoded_payload - the encoded payload
 * \param [in] raw_payload - the raw payload
 * \return
 *  * Zero for success
 * \remark
 * \see
 *  * bcm_dnx_l3_route_get
 */
static shr_error_e
dnx_l3_route_payload_raw_resolve_get(
    int unit,
    bcm_l3_route_t * info,
    uint8 is_kbp_entry,
    dbal_fields_e dest_type,
    dbal_fields_e dbal_sub_field,
    uint32 encoded_payload,
    uint32 raw_payload)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Handle 'raw' entry and flag */
    if (!_SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_RAW_ENTRY))
    {
        /*
         * If 'raw' flag was NOT set at input then check:
         *   If setting is illegal then consider this as a 'raw' entry -
         *          return extracted value without encoding and set the 'raw' flag.
         *   Otherwise, encode extracted entry and return encoded value.
         */
        if (dest_type != dbal_sub_field)
        {
            info->l3a_flags2 |= BCM_L3_FLAGS2_RAW_ENTRY;
        }
        else
        {
            switch (dbal_sub_field)
            {
                case DBAL_FIELD_FEC:
                {
                    /*
                     * If encoding is 'FEC' (full 20 bits value) and value is higher
                     * than maximal for FEC then assume it is 'raw'.
                     * So, set 'raw' flag to one.
                     * Otherwise, assume it is standard FEC and, so, set 'raw' flag to zero.
                     */
                    if (encoded_payload >= dnx_data_l3.fec.nof_fecs_get(unit))
                    {
                        info->l3a_flags2 |= BCM_L3_FLAGS2_RAW_ENTRY;
                    }
                    break;
                }
                case DBAL_FIELD_FEC_DEFAULT:
                {
                    if (!is_kbp_entry)
                    {
                        /*
                         * If encoding is 'FEC_DEFAULT' (17 bits value) and value is higher
                         * than maximal for FEC_DEFAULT then assume it is 'raw'.
                         * So, set 'raw' flag to one.
                         * Otherwise, assume it is standard FEC_DEFAULT and, so, set 'raw' flag to zero.
                         */
                        if (encoded_payload > dnx_data_l3.fec.max_default_fec_get(unit))
                        {
                            info->l3a_flags2 |= BCM_L3_FLAGS2_RAW_ENTRY;
                        }
                    }
                    else
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal sub-field with ID %d found as result of L3 route get.\n",
                                     dbal_sub_field);
                    }
                    break;
                }
                default:
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal sub-field with ID %d found as result of L3 route get.\n",
                                 dbal_sub_field);
                    break;
                }
            }
        }
    }
    if (_SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_RAW_ENTRY))
    {
        /** If 'raw' flag was set at input then return extracted value as is, no encoding. */
        info->l3a_intf = raw_payload;
    }
    else
    {
        /** Set the value of the l3a_intf field using fec_index. */
        BCM_L3_ITF_SET(info->l3a_intf, BCM_L3_ITF_TYPE_FEC, encoded_payload);
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
 * \param [in] is_kbp_entry - indication whether the entry is for KBP or for KAPS
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
dnx_l3_route_payload_get(
    int unit,
    bcm_l3_route_t * info,
    uint8 is_kbp_entry,
    uint8 is_kbp_split_rpf_in_use,
    uint32 dbal_table,
    uint32 entry_handle_id)
{
    dbal_fields_e dest_type, dbal_sub_field;
    uint32 result_value, destination_val;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "l3_route");

    /** Handling of KAPS tables */
    if (!is_kbp_entry)
    {
        uint32 table_hb_enabled;
        uint32 hitbit = 0;

        /**  Setting pointer to 'result_value' variable to receive the field value from the table. */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_KAPS_RESULT, INST_SINGLE, &destination_val);

        /** Receive indication whether the hitbit is enabled for the selected table. */
        SHR_IF_ERR_EXIT(dbal_tables_indication_get(unit, dbal_table, DBAL_TABLE_IND_IS_HITBIT_EN, &table_hb_enabled));
        /** If the HB indication is enabled, retrieve the hitbit and clear it if BCM_L3_HIT_CLEAR flag was provided. */
        if (table_hb_enabled)
        {
            uint32 hitbit_flags = DBAL_ENTRY_ATTR_HIT_GET;
            hitbit_flags |= (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_HIT_CLEAR)) ? DBAL_ENTRY_ATTR_HIT_CLEAR : 0;
            SHR_IF_ERR_EXIT(dbal_entry_attribute_request(unit, entry_handle_id, hitbit_flags, &hitbit));
        }

        /** Preforming the action after this call the pointers that we set in field32_request() */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        if (hitbit != 0)
        {
            info->l3a_flags |= BCM_L3_HIT;
        }
        /*
         * Get the used sub-field and its value.
         * The sub-field ID value is assigned to dbal_sub_field variable and its decoded
         * value is assigned to result_value.
         */
        SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                        (unit, DBAL_FIELD_KAPS_RESULT, destination_val, &dbal_sub_field, &result_value));

        /** Set the expected destination type */
        dest_type = DNX_L3_ENTRY_IS_DEFAULT(info) ? DBAL_FIELD_FEC_DEFAULT : DBAL_FIELD_FEC;
    }
    /** Handling of KBP tables */
    else
    {
        uint8 source_drop = FALSE;
        uint32 result_type;
        uint32 is_default;

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

                STAT_PP_ENGINE_PROFILE_SET(info->stat_pp_profile, stat_pp_profile,
                                           bcmStatCounterInterfaceIngressReceivePp);
            }
        }
        /** If EEI is used.*/
        else if ((result_type == DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_DEST_EEI_W_DEFAULT)
                 || (result_type ==
                     DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_DEST_EEI_W_DEFAULT))
        {
            uint32 eei_val, sub_field_id, sub_field_val, mpls_val;

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_EEI, INST_SINGLE, &eei_val));
            SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                            (unit, DBAL_FIELD_EEI, eei_val, &sub_field_id, &sub_field_val));

            if (sub_field_id == DBAL_FIELD_EEI_MPLS_SWAP_COMMAND || sub_field_id == DBAL_FIELD_EEI_MPLS_PUSH_COMMAND)
            {
                SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                                (unit, sub_field_id, DBAL_FIELD_MPLS_LABEL, &mpls_val, &sub_field_val));

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
            if (L3_TABLE_IS_RPF(dbal_table))
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
        if (!L3_TABLE_IS_RPF(dbal_table))
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
    }

    /** Resolve raw payload and assign the relevant payload value to l3a_intf. */
    SHR_IF_ERR_EXIT(dnx_l3_route_payload_raw_resolve_get
                    (unit, info, is_kbp_entry, dest_type, dbal_sub_field, result_value, destination_val));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Adds/updates an IP route entry.
 * \param [in] unit - The unit number.
 * \param [in] info -
 *      L3 route information (key and payload) to be
 *      added/updated. \n
 *      info.l3a_vrf - Vritual Router Instance \n
 *      info.l3a_flags - Route entry control flags defined as
 *      BCM_L3_*** \n
 *      info.l3a_intf - L3 interface associated with the route\n
 *      info.l3a_subnet - IPv4 subnet address\n
 *      info.l3a_ip_mask - IPv4 subnet mask \n
 *      info.l3a_ip6_net - IPv6 subnet address\n
 *      info.l3a_ip6_mask - IPv6 subnet mask\n
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative is error was detected. See \ref
 *           shr_error_e
 * \see
 *   * bcm_l3_route_t
 *
 *   Tables to which the entry may be added are:
 *      * IPV6_UNICAST_PRIVATE_LPM_FORWARD
 *      * IPV4_UNICAST_PRIVATE_LPM_FORWARD
 *      * IPV6_UNICAST_PRIVATE_LPM_FORWARD_2
 *      * IPV4_UNICAST_PRIVATE_LPM_FORWARD_2
 */
shr_error_e
bcm_dnx_l3_route_add(
    int unit,
    bcm_l3_route_t * info)
{
    dbal_fields_e dest_type = DBAL_FIELD_EMPTY;
    uint32 entry_handle_id;
    uint32 l3_intf;
    uint32 dbal_table;
    uint8 is_kbp_entry;
    uint8 is_kbp_split_rpf_in_use;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    l3_intf = BCM_L3_ITF_VAL_GET(info->l3a_intf);
    is_kbp_entry = DNX_L3_ENTRY_IS_FOR_KBP(unit, info);
    is_kbp_split_rpf_in_use = is_kbp_entry ? kbp_mngr_split_rpf_in_use(unit) : FALSE;

    /** Verify flags */
    SHR_IF_ERR_EXIT(dnx_l3_route_flags_verify(unit, info, is_kbp_entry, is_kbp_split_rpf_in_use));
    /** Verify key */
    SHR_IF_ERR_EXIT(dnx_l3_route_key_verify(unit, info));

    /** Select DBAL table */
    SHR_IF_ERR_EXIT(dnx_l3_route_dbal_table_select(unit, info, is_kbp_entry, is_kbp_split_rpf_in_use, &dbal_table));

    /** Verify payload */
    SHR_IF_ERR_EXIT(dnx_l3_route_payload_verify
                    (unit, info, is_kbp_entry, is_kbp_split_rpf_in_use, dbal_table, &dest_type));

    /** Allocate DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    /** Set the key */
    SHR_IF_ERR_EXIT(dnx_l3_route_key_set(unit, info, entry_handle_id));

    if (is_kbp_entry)
    {
        /*
         * KBP is using multiple result types that need to be set.
         * Some result types have additional result fields that also need to be set.
         */
        SHR_IF_ERR_EXIT(dnx_l3_route_kbp_payload_set(unit, info, dbal_table, entry_handle_id));
    }

    /** Set the destination */
    dbal_entry_value_field32_set(unit, entry_handle_id, dest_type, INST_SINGLE, l3_intf);

    /** Commit the entry. Create it if it doesn't exist, update it if it does. */
    if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_REPLACE))
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_FORCE));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Find an IP route entry
 * \param [in] unit - The unit number.
 * \param [in,out] info -
 *      The l3 route structure describing the entry.\n
 *      As input -
 *          info.l3a_vrf   -          \n
 *            Virtual Router Instance \n
 *          info.l3a_flags -                                   \n
 *            Control flags for route entries -                \n
 *              BCM_L3_*** (Eg. BCM_L3_IP6, BCM_L3_MULTIPATH, BCM_L3_HIT_CLEAR)  \n
 *          info.l3a_flags2 -                          \n
 *            Indicative Control flags -               \n
 *              BCM_L3_FLAGS2_RAW_ENTRY -              \n
 *                If set then required output format is 'raw'. Output (on 'info->l3a_intf')
 *                will NOT be encoded (as per BCM_L3_ITF_SET, BCM_L3_ITF_TYPE_FEC, ...)
 *          info.l3a_subnet -         \n
 *            IPv4 subnet address     \n
 *          info.l3a_ip_mask -        \n
 *            IPv4 subnet mask        \n
 *          info.l3a_ip6_net -        \n
 *            IPv6 subnet address     \n
 *          info.l3a_ip6_mask -       \n
 *            IPv6 subnet mask        \n
 *      As output -
 *          info.l3a_intf -                            \n
 *            Destination interface is returned as FEC \n
 *          info.l3a_flags2 -                          \n
 *            Indicative Control flags -               \n
 *              BCM_L3_FLAGS2_RAW_ENTRY -              \n
 *                If set at input then output value on 'info->l3a_intf' as 'raw'
 *                (without encoding) and flag remains as is.
 *                Otherwise, flag is set if procedure managed to identify entry
 *                as 'raw'. Otherwise, output value on 'info->l3a_intf'is encoded
 *                (as per BCM_L3_ITF_SET, BCM_L3_ITF_TYPE_FEC, ...) and flag is
 *                not set.
 * \return
 *   \retval Zero in case of no error.
 *   \retval Non-zero in case of an error
 * \remark
 *   * receive information from LPM tables:
 *      IPV4_UNICAST_PRIVATE_LPM_FORWARD_2
 *      IPV4_UNICAST_PRIVATE_LPM_FORWARD
 *      IPV6_UNICAST_PUBLIC_LPM_FORWARD
 *      IPV6_UNICAST_PRIVATE_LPM_FORWARD
 *      KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD
 *      KBP_IPV4_UNICAST_PUBLIC_LPM_FORWARD
 * \see
 *   * bcm_l3_route_t fields \n
 *   * BCM L3 Flags \n
 */
shr_error_e
bcm_dnx_l3_route_get(
    int unit,
    bcm_l3_route_t * info)
{
    uint32 entry_handle_id;
    uint32 dbal_table;
    uint8 is_kbp_entry;
    uint8 is_kbp_split_rpf_in_use;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    is_kbp_entry = DNX_L3_ENTRY_IS_FOR_KBP(unit, info);
    is_kbp_split_rpf_in_use = is_kbp_entry ? kbp_mngr_split_rpf_in_use(unit) : FALSE;

    /** Verify flags */
    SHR_IF_ERR_EXIT(dnx_l3_route_flags_verify(unit, info, is_kbp_entry, is_kbp_split_rpf_in_use));
    /** Verify key */
    SHR_IF_ERR_EXIT(dnx_l3_route_key_verify(unit, info));

    /*
     * Make sure that the output parameter is 0 before receiving data.
     * In case of an error receiving the result value, output value will be 0.
     */
    BCM_L3_ITF_SET(info->l3a_intf, BCM_L3_ITF_TYPE_FEC, 0);

    /** Select DBAL table */
    SHR_IF_ERR_EXIT(dnx_l3_route_dbal_table_select(unit, info, is_kbp_entry, is_kbp_split_rpf_in_use, &dbal_table));

    /** Allocate DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    /** Set the key */
    SHR_IF_ERR_EXIT(dnx_l3_route_key_set(unit, info, entry_handle_id));

    /** Get the payload */
    SHR_IF_ERR_EXIT(dnx_l3_route_payload_get
                    (unit, info, is_kbp_entry, is_kbp_split_rpf_in_use, dbal_table, entry_handle_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Delete an IP route entry
 * \param [in] unit - The unit number.
 * \param [in] info -
 *      The l3 route structure describing the entry.\n
 *      info.l3a_vrf - Virtual Router Instance \n
 *      info.l3a_flags - Control flags for route entries -
 *      BCM_L3_*** \n
 *      info.l3a_subnet - IPv4 subnet address \n
 *      info.l3a_ip_mask - IPv4 subnet mask \n
 *      info.l3a_ip6_net - IPv6 subnet address \n
 *      info.l3a_ip6_mask - IPv6 subnet mask \n
 * \return
 *   \retval Zero in case of no error.
 *   \retval Non-zero in case of an error.
 * \see
 *   * info above \n
 */
shr_error_e
bcm_dnx_l3_route_delete(
    int unit,
    bcm_l3_route_t * info)
{
    uint32 entry_handle_id;
    uint32 dbal_table;
    uint8 is_kbp_entry;
    uint8 is_kbp_split_rpf_in_use;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    is_kbp_entry = DNX_L3_ENTRY_IS_FOR_KBP(unit, info);
    is_kbp_split_rpf_in_use = is_kbp_entry ? kbp_mngr_split_rpf_in_use(unit) : FALSE;

    /** Verify flags */
    SHR_IF_ERR_EXIT(dnx_l3_route_flags_verify(unit, info, is_kbp_entry, is_kbp_split_rpf_in_use));
    /** Verify key */
    SHR_IF_ERR_EXIT(dnx_l3_route_key_verify(unit, info));

    /** Select DBAL table */
    SHR_IF_ERR_EXIT(dnx_l3_route_dbal_table_select(unit, info, is_kbp_entry, is_kbp_split_rpf_in_use, &dbal_table));

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

/**
 * \brief
 *   Determines to which table the L3 route entry belongs.
 * \param [in] unit - The unit number.
 * \param [in] info - contains valid fields for determining correct table.
 *      - info->l3a_flags - control flags (eg. BCM_L3_IP6)
 * \param [out] dbal_table - returns the name of the dbal table in which the entry will be added
 * \return
 *  * Zero for success
 * \see
 *  * bcm_dnx_l3_route_add
 *  * bcm_dnx_l3_route_delete
 *  * bcm_dnx_l3_route_get
 */
static shr_error_e
dnx_l3_host_dbal_table_select(
    int unit,
    bcm_l3_host_t * info,
    uint32 *dbal_table)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * If flag BCM_L3_IP6 is set, then the entry will be added to the IPV6_UNICAST_*_LPM_FORWARD table.
     * Valid fields for it are VRF and IPv6 address(l3a_ip6_addr).
     *
     * If flag BCM_L3_IP6 is NOT set, then the entry will be added to the IPV4_UNICAST_*_LPM_FORWARD table.
     * Valid fields for it are VRF and IPv4 address(l3a_ip_addr).
     */
    if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
    {
        /*
         * Get an handle to the IPv6 private host table and set the fields that form the key of the entry
         * When the KBP IPv6 forwarding application is enabled, use the KBP table handle
         */
        if (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv6))
        {
            *dbal_table = (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_HOST_LOCAL)) ?
                DBAL_TABLE_IPV6_UNICAST_PRIVATE_HOST : DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD;
        }
        else
        {
            *dbal_table = DBAL_TABLE_IPV6_UNICAST_PRIVATE_HOST;
        }
    }
    else
    {
        /*
         * Get an handle to the IPv4 private host table and set the fields that form the key of the entry
         * When the KBP IPv4 forwarding application is enabled, use the KBP table handle
         */
        if (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4))
        {
            *dbal_table = (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_HOST_LOCAL)) ?
                DBAL_TABLE_IPV4_UNICAST_PRIVATE_HOST : DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD;
        }
        else
        {
            *dbal_table = DBAL_TABLE_IPV4_UNICAST_PRIVATE_HOST;
        }
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verify the host entry add/find/delete input.
 * \param [in] unit - The unit number.
 * \param [in] info - The l3 route structure to be verified.
 * \return
 *   \retval Zero if no error was encountered
 *   \retval Non-zero in case of an error.
 * \see
 *   * dnx_l3_host_add_verify \n
 *   * dnx_l3_host_get \n
 *   * dnx_l3_host_delete \n
 */
static shr_error_e
dnx_l3_host_verify(
    int unit,
    bcm_l3_host_t * info)
{
    int kaps_public_capacity;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify the the l3a_nexthop_mac is zero.
     */
    if (!BCM_MAC_IS_ZERO(info->l3a_nexthop_mac))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "l3a_nexthop_mac is not supported for JR2.");
    }

    /*
     * Verify the the VRF is in range.
     */
    if (info->l3a_vrf >= dnx_data_l3.vrf.nof_vrf_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "VRF %d is out of range, should be lower than %d.", info->l3a_vrf,
                     dnx_data_l3.vrf.nof_vrf_get(unit));
    }
    /** Verify supported flags */
    if (_SHR_IS_FLAG_SET(info->l3a_flags, ~L3_SUPPORTED_BCM_L3_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal L3 flags are passed to bcm_l3_host_* API.");
    }

    /** Verify supported flags2 */
    if (_SHR_IS_FLAG_SET(info->l3a_flags2, ~L3_FORWARD_SUPPORTED_BCM_L3_FLAGS2))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal L3 flags2 are passed to the bcm_l3_host_* API - %u", info->l3a_flags2);
    }

    /*
     * Verify that the IP address is a unicast address.
     * If BCM_L3_IP6 flag is set, the field holding information about the IPv6 address (l3a_ip6_addr) will be checked.
     * If the flag is not set, the field holding the IPv4 address (l3a_ip_addr) will be checked.
     */
    if (!L3_IPV4_ADDRESS_IS_UNICAST(info->l3a_ip_addr) && !_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "IPv4 address %d provided in l3a_ip_addr field is not a unicast address.",
                     info->l3a_ip_addr);
    }
    else if (!L3_IPV6_ADDRESS_IS_UNICAST(info->l3a_ip6_addr) && _SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "IPv6 address, provided in l3a_ip6_addr field, is not a unicast address.");
    }

    /*
     * If capacity is more than 0, then a private and a public lookups are made.
     * However, for host entries there is only a Private table.
     * This means that if the property is not set, a VRF value of 0 is not allowed.
     * When the capacity of the MDB table is 0, then only a private lookup will be carried out.
     * Otherwise, both public and private are active.
     */
    if (info->l3a_vrf == 0)
    {
        SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_KAPS_2, &kaps_public_capacity));
        if (kaps_public_capacity > 0)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "Illegal VRF value 0 for UC host entry when table capacity indicates both public and private lookup.");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verify the host entry add input.
 * \param [in] unit - The unit number.
 * \param [in] info - The l3 host structure to be verified.
 * \param [in] dbal_table - The table that is selected for host entry.
 * \param [in] system_headers_mode - system headers mode.
 * \param [in] is_kbp_table - dbal table whether is KBP table.
 * \return
 *   \retval Zero in case of no error.
 *   \retval Non-zero in case of an error.
 * \see
 *   * info above \n
 */
static shr_error_e
dnx_l3_host_add_verify(
    int unit,
    bcm_l3_host_t * info,
    uint32 dbal_table,
    int system_headers_mode,
    int is_kbp_table)
{
    uint32 fec_index;
    int routed_learn_flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "l3_host");

    fec_index = BCM_L3_ITF_VAL_GET(info->l3a_intf);

    /** Additional forwarding data validation */
    if (info->l3a_lookup_class != 0)
    {
        if (!is_kbp_table)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Additional forwarding data in l3a_lookup_class is only supported with KBP tables\n");
        }
        else
        {
            if ((info->l3a_port_tgid == BCM_GPORT_TYPE_NONE) && (BCM_FORWARD_ENCAP_ID_VAL_GET(info->encap_id) == 0) &&
                (info->stat_pp_profile == STAT_PP_PROFILE_INVALID))
            {
                /** Result type is optimized */
                if (info->l3a_lookup_class > DNX_L3_MAX_PMF_GENERIC_DATA_VALUE_FOR_OPTIMIZED_RESULT)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot fit additional forwarding data %d. Max possible value is %d\n",
                                 info->l3a_lookup_class, DNX_L3_MAX_PMF_GENERIC_DATA_VALUE_FOR_OPTIMIZED_RESULT);
                }
            }
            else
            {
                /** Result type is non-optimized */
                if (info->l3a_lookup_class > DNX_L3_MAX_PMF_GENERIC_DATA_VALUE)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot fit additional forwarding data %d. Max possible value is %d\n",
                                 info->l3a_lookup_class, DNX_L3_MAX_PMF_GENERIC_DATA_VALUE);
                }
            }
        }
    }

    /*
     * Verify common fields of the host entry
     */
    SHR_IF_ERR_EXIT(dnx_l3_host_verify(unit, info));

    /*
     * Check that the FEC value is in range.
     * The NOF of FECs is not configurable.
     */
    if ((BCM_L3_ITF_TYPE_IS_FEC(info->l3a_intf) || info->l3a_port_tgid == BCM_GPORT_TYPE_NONE)
        && !DNX_L3_FEC_IS_IN_RANGE(unit, fec_index))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Invalid FEC destination for host entry - value must be bigger than 0 and smaller than %d\n",
                     dnx_data_l3.fec.nof_fecs_get(unit));
    }

    /*
     * Check encap_id is set correctly.
     */
    if ((info->l3a_flags & BCM_L3_ENCAP_SPACE_OPTIMIZED))
    {
        /** In this case, encap_id must be EEI.*/
        if (!BCM_FORWARD_ENCAP_ID_IS_EEI(info->encap_id))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "encap_id(0x%08X) must be encoded as EEI when BCM_L3_ENCAP_SPACE_OPTIMIZED is set.",
                         info->encap_id);
        }
        /** In JR2, EEI must be for push-command and the push-profile must be 0.*/
        if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
        {
            if (BCM_FORWARD_ENCAP_ID_EEI_USAGE_GET(info->encap_id) != BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT)
            {
                /** If it's an outLIF, do not encode it as EEI in JR2*/
                SHR_ERR_EXIT(_SHR_E_PARAM, "encap_id(0x%08X) encoded as an EEI must be used for mpls port.",
                             info->encap_id);
            }
            else if (BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_PUSH_PROFILE_GET(info->encap_id) != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "encap_id(0x%08X) encoded as an EEI can not include non-zero push profile.",
                             info->encap_id);
            }
        }
    }
    else
    {
        /** In this case, encap_id must include a outlif. Verify that the out-LIF value is in range*/
        if (BCM_FORWARD_ENCAP_ID_VAL_GET(info->encap_id) > dnx_data_lif.global_lif.nof_global_out_lifs_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "OUT_LIF %d is out of range, should be lower than %d.", info->encap_id,
                         dnx_data_lif.global_lif.nof_global_out_lifs_get(unit));
        }
    }

    /*
     * Check cases when l3a_port_tgid is defined.
     */
    if ((info->l3a_port_tgid != BCM_GPORT_TYPE_NONE)
        && (BCM_FORWARD_ENCAP_ID_VAL_GET(info->encap_id) != 0 || BCM_L3_ITF_VAL_GET(info->l3a_intf) != 0))
    {
        /** Verify that l3a_intf is not zero while encap_id is set. */
        if (BCM_FORWARD_ENCAP_ID_VAL_GET(info->encap_id) != 0 && BCM_L3_ITF_VAL_GET(info->l3a_intf) == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot have encap_id defined while l3a_intf is not set.");
        }
        /** Verify that l3a_intf is not with type FEC while l3a_port_tgid is set. */
        if (BCM_L3_ITF_TYPE_IS_FEC(info->l3a_intf))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Multiple destination types indicated - l3a_intf is of type FEC but l3a_port_tgid is defined");
        }

        /** In case of KBP, only one outlif is possible and stat is not supported.*/
        if (is_kbp_table)
        {
            if (info->encap_id != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "KBP support one outlif by l3a_intf only, encap_id (%d) must be 0.\n",
                             info->encap_id);
            }
            if (info->stat_pp_profile != STAT_PP_PROFILE_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "KBP does not support statistics profile in the result\n");
            }
        }
        else
        {
            if (info->l3a_flags & BCM_L3_ENCAP_SPACE_OPTIMIZED)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "HOST does not support outLIF and EEI in the result\n");
            }
        }
    }
    else if (info->l3a_port_tgid == BCM_GPORT_TYPE_NONE)
    {
        /*
         * In case l3a_port_tgid is not set:
         * l3a_intf must be FEC, encap_id can be outlif, EEI or NULL.
         * For KBP, stat is possible only when encap_id is not set.
         */
        if (info->encap_id != 0)
        {
            if (is_kbp_table)
            {
                if (info->stat_pp_profile != STAT_PP_PROFILE_INVALID)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "If destination is FEC, and encap_id is set, KBP can not support stat in the result.\n");
                }
            }
            else
            {
                if ((system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE) &&
                    (info->stat_pp_profile != STAT_PP_PROFILE_INVALID) &&
                    _SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_ENCAP_SPACE_OPTIMIZED))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "If destination is FEC, EEI can not be used together with stat!\n");
                }
            }
        }
    }

    /*
     * Statistics check
     */
    if ((info->stat_pp_profile != STAT_PP_PROFILE_INVALID))
    {
        if (info->stat_id > L3_FRWRD_TABLES_MAX_STAT_ID_VALUE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Stat id %d, max value 0x%x", info->stat_id, L3_FRWRD_TABLES_MAX_STAT_ID_VALUE);
        }
        SHR_IF_ERR_EXIT(dnx_stat_pp_crps_verify
                        (unit, BCM_CORE_ALL, info->stat_id, info->stat_pp_profile,
                         bcmStatCounterInterfaceIngressReceivePp));
    }

    /*
     * stat_id and stat_pp_profile should be default value when encap_id is EEI 
     */
    if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_ENCAP_SPACE_OPTIMIZED) ||
        (BCM_FORWARD_ENCAP_ID_IS_EEI(info->encap_id)
         && (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)))
    {
        if (info->stat_pp_profile != STAT_PP_PROFILE_INVALID || info->stat_id != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "stat_id %d and stat_pp_profile %d should be default value when forward result is dest_eei",
                         info->stat_id, info->stat_pp_profile);
        }
    }

    if (!is_kbp_table)
    {
        SHR_IF_ERR_EXIT(dnx_switch_control_routed_learn_get(unit, &routed_learn_flags));
        if (routed_learn_flags &&
            dnx_data_l2.feature.feature_get(unit, dnx_data_l2_feature_opportunistic_learning_always_transplant))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Couldn't add lem host table entry when routing learning is enabled.");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verify the input flags to host traverse
 * \param [in] unit - The unit number.
 * \param [in] flags - the control flags passed to the function
 * \return
 *   \retval Zero if no error was encountered
 *   \retval Non-zero in case of an error.
 * \see
 *   * bcm_dnx_l3_host_traverse \n
 */
static shr_error_e
dnx_l3_host_traverse_verify(
    int unit,
    uint32 flags)
{
    int routed_learn_flags = 0;

    SHR_FUNC_INIT_VARS(unit);
    if (flags != 0 && _SHR_IS_FLAG_SET(flags, ~L3_SUPPORTED_BCM_L3_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The flag provided to bcm_l3_host_traverse is not supported.");
    }

    SHR_IF_ERR_EXIT(dnx_switch_control_routed_learn_get(unit, &routed_learn_flags));
    if (routed_learn_flags &&
        dnx_data_l2.feature.feature_get(unit, dnx_data_l2_feature_opportunistic_learning_always_transplant) &&
        !(dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv6) ||
          dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4)))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "After enabling routing learning, couldn't call host traverse function in no-KBP device.");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verify the input flags to route traverse
 * \param [in] unit - The unit number.
 * \param [in] flags - the control flags passed to the function
 * \return
 *   \retval Zero if no error was encountered
 *   \retval Non-zero in case of an error.
 * \see
 *   * bcm_dnx_l3_route_traverse \n
 */
static shr_error_e
dnx_l3_route_traverse_verify(
    int unit,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);
    if (flags != 0 && _SHR_IS_FLAG_SET(flags, ~L3_SUPPORTED_BCM_L3_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The flag provided to bcm_l3_route_traverse is not supported.");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set the fields in the key for a UC LMP route table entry.
 * \param [in] unit - The unit number.
 * \param [in] info - contains valid fields for EM Private host
 *        table
 * \param [in] entry_handle_id - a handle to the LPM table
 *        (public or private based on the info->l3a_vrf value)
 * \return
 *  * Zero for success
 * \see
 *  * bcm_dnx_l3_host_add
 *  * bcm_dnx_l3_host_delete
 *  * bcm_dnx_l3_host_find
 */
static shr_error_e
dnx_l3_host_to_key(
    int unit,
    bcm_l3_host_t * info,
    uint32 *entry_handle_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * If flag BCM_L3_IP6 is set then the entry belongs to the IPv6 Host table.
     * Valid fields for the key of the entry are IPv6 address(l3a_ip6_addr) and VRF(l3a_vrf).
     *
     * If flag BCM_L3_IP6 is NOT set (else clause) then the entry belongs to the IPv4 Host table.
     * Valid fields for the key of the IPv4 entry key are IPv4 address(l3a_ip_addr) and VRF(l3a_vrf).
     */
    if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
    {
        dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_VRF, info->l3a_vrf);
        dbal_entry_key_field_arr8_set(unit, *entry_handle_id, DBAL_FIELD_IPV6, info->l3a_ip6_addr);
    }
    else
    {
        dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_VRF, info->l3a_vrf);
        dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_IPV4, info->l3a_ip_addr);
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Resolves the destination of the L3 host entry and fills in the appropriate fields in info structure.
 * \param [in] unit - The unit number.
 * \param [in] result_type - The DBAL result type of the entry.
 * \param [in] dbal_table_id - The DBAL table to which the entry belongs
 * \param [in] entry_handle_id - The DBAL handle to the entry.
 * \param [out] info - structure describing the entry. Valid out fields which can be filled in:
 *      * info->l3a_intf - L3 interface
 *      * info->encap_id - Encapsulation index.
 *      * info->l3a_port_tgid - Port or trunk ID.
 *      * info->stat_id - Statistics index
 *      * info->stat_pp_profile - statistics profile
 * \return
 *  * Zero for success
 * \see
 *  * bcm_dnx_l3_route_traverse
 *  * bcm_dnx_l3_route_get
 */
static shr_error_e
dnx_l3_host_resolve_destination(
    int unit,
    uint32 result_type,
    uint32 dbal_table_id,
    uint32 *entry_handle_id,
    bcm_l3_host_t * info)
{
    uint32 field_value;
    uint32 stat_pp_profile;
    uint32 sub_field_id;
    uint32 sub_field_val;
    dbal_enum_value_field_system_headers_mode_e system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Nullify the stucture members before beginning.
     * In case of an error receiving the result value, output value will be 0.
     */
    info->l3a_intf = 0;
    info->encap_id = 0;
    info->l3a_port_tgid = 0;
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    /** Resolve the destination fields of the info structure based on the result type and the table ID. */
    if (((result_type == DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST
          || result_type == DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_STAT)
         && dbal_table_id == DBAL_TABLE_IPV4_UNICAST_PRIVATE_HOST)
        || ((result_type == DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST
             || result_type == DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_STAT)
            && dbal_table_id == DBAL_TABLE_IPV6_UNICAST_PRIVATE_HOST)
        || ((result_type == DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_KBP_DEST_W_DEFAULT
             || result_type == DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_DEST_STAT_W_DEFAULT)
            && dbal_table_id == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD)
        || ((result_type == DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_KBP_DEST_W_DEFAULT
             || result_type == DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_DEST_STAT_W_DEFAULT)
            && dbal_table_id == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD))
    {
        /** If result type is FWD_Dest, then only DESTINATION field is used. */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, *entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, &field_value));
        BCM_L3_ITF_SET(info->l3a_intf, BCM_L3_ITF_TYPE_FEC, field_value);
    }
    else if (((result_type == DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_DOUBLE_OUTLIF ||
               result_type == DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_DOUBLE_OUTLIF_STAT)
              && dbal_table_id == DBAL_TABLE_IPV4_UNICAST_PRIVATE_HOST)
             || ((result_type == DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_DOUBLE_OUTLIF ||
                  result_type == DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_DOUBLE_OUTLIF_STAT)
                 && dbal_table_id == DBAL_TABLE_IPV6_UNICAST_PRIVATE_HOST))
    {
        /*
         * If the result type of the entry is FWD_DEST_DOUBLE_OUTLIF
         * then the fields that we can receive are gport, ETH-RIF and OUT_LIF.
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, *entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, &field_value));
        SHR_IF_ERR_EXIT(algo_gpm_gport_from_encoded_destination_field(unit, 0, field_value, &info->l3a_port_tgid));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, *entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, &field_value));
        BCM_L3_ITF_SET(info->l3a_intf, BCM_L3_ITF_TYPE_RIF, field_value);
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, *entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF_2ND, INST_SINGLE, &field_value));
        BCM_L3_ITF_SET(info->encap_id, BCM_L3_ITF_TYPE_LIF, field_value);
    }
    else if ((result_type == DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_EEI &&
              dbal_table_id == DBAL_TABLE_IPV4_UNICAST_PRIVATE_HOST) ||
             (result_type == DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_EEI &&
              dbal_table_id == DBAL_TABLE_IPV6_UNICAST_PRIVATE_HOST) ||
             (result_type == DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_DEST_EEI_W_DEFAULT &&
              (dbal_table_id == DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD ||
               dbal_table_id == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD)) ||
             (result_type == DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_DEST_EEI_W_DEFAULT &&
              (dbal_table_id == DBAL_TABLE_IPV6_UNICAST_PRIVATE_LPM_FORWARD ||
               dbal_table_id == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD)))
    {
        /** If result type is FWD_Dest_EEI, then DESTINATION + EEI fields are used. */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, *entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, &field_value));
        SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                        (unit, DBAL_FIELD_DESTINATION, field_value, &sub_field_id, &sub_field_val));
        BCM_L3_ITF_SET(info->l3a_intf, BCM_L3_ITF_TYPE_FEC, sub_field_val);

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, *entry_handle_id, DBAL_FIELD_EEI, INST_SINGLE, &field_value));
        SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                        (unit, DBAL_FIELD_EEI, field_value, &sub_field_id, &sub_field_val));
        /** For EEI, either EEI_ENCAPSULATION_POINTER is used or EEI_MPLS_*_COMMAND */

        /*
         * Jericho mode beginning
         * }
         */
        if (sub_field_id == DBAL_FIELD_EEI_ENCAPSULATION_POINTER)
        {
            if (sub_field_val)
            {
                
                /**BCM_FORWARD_ENCAP_ID_VAL_SET(info->encap_id, BCM_FORWARD_ENCAP_ID_TYPE_EEI,
                                             BCM_FORWARD_ENCAP_ID_EEI_USAGE_ENCAP_POINTER, sub_field_val); */
                BCM_L3_ITF_SET(info->encap_id, BCM_L3_ITF_TYPE_LIF, sub_field_val);
            }
        }
        else if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
        {
            uint32 mpls_label, push_cmd, encap_id;
            info->l3a_flags |= BCM_L3_ENCAP_SPACE_OPTIMIZED;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                            (unit, DBAL_FIELD_EEI_MPLS_PUSH_COMMAND, DBAL_FIELD_MPLS_LABEL, &mpls_label, &field_value));

            SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                            (unit, DBAL_FIELD_EEI_MPLS_PUSH_COMMAND, DBAL_FIELD_PUSH_CMD_ID, &push_cmd, &field_value));
            BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_SET(encap_id, mpls_label, push_cmd);
            BCM_FORWARD_ENCAP_ID_VAL_SET(info->encap_id, BCM_FORWARD_ENCAP_ID_TYPE_EEI,
                                         BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT, encap_id);
        }
        /*
         * Jericho mode end
         * }
         */
        else
        {
            uint32 mpls_label, encap_id;
            info->l3a_flags |= BCM_L3_ENCAP_SPACE_OPTIMIZED;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                            (unit, DBAL_FIELD_EEI_MPLS_SWAP_COMMAND, DBAL_FIELD_MPLS_LABEL, &mpls_label, &field_value));

            /** In JR2 mode, the encap_id is encoded as EEI with push_profile 0 always(the input push_profile is not used and lost)*/
            BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_SET(encap_id, mpls_label, 0);
            BCM_FORWARD_ENCAP_ID_VAL_SET(info->encap_id, BCM_FORWARD_ENCAP_ID_TYPE_EEI,
                                         BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT, encap_id);
        }
    }
    else if (((result_type == DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF
               || result_type == DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF_STAT)
              && dbal_table_id == DBAL_TABLE_IPV4_UNICAST_PRIVATE_HOST)
             || ((result_type == DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF
                  || result_type == DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF_STAT)
                 && dbal_table_id == DBAL_TABLE_IPV6_UNICAST_PRIVATE_HOST)
             || (result_type == DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_OUTLIF_W_DEFAULT
                 && dbal_table_id == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD)
             || (result_type == DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_OUTLIF_W_DEFAULT
                 && dbal_table_id == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD))
    {
        uint32 out_lif_val;
        uint32 fec_index;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, *entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF,
                                                            INST_SINGLE, &out_lif_val));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, *entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, &field_value));
        SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                        (unit, DBAL_FIELD_DESTINATION, field_value, &sub_field_id, &sub_field_val));
        if (sub_field_id == DBAL_FIELD_FEC)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, *entry_handle_id, DBAL_FIELD_FEC, INST_SINGLE, &fec_index));
            BCM_L3_ITF_SET(info->l3a_intf, BCM_L3_ITF_TYPE_FEC, fec_index);
            BCM_L3_ITF_SET(info->encap_id, BCM_L3_ITF_TYPE_LIF, out_lif_val);
        }
        else if (sub_field_id == DBAL_FIELD_PORT_ID || sub_field_id == DBAL_FIELD_LAG_ID)
        {
            info->l3a_intf = 0;
            SHR_IF_ERR_EXIT(algo_gpm_gport_from_encoded_destination_field(unit, 0, field_value, &info->l3a_port_tgid));
            BCM_L3_ITF_SET(info->l3a_intf, BCM_L3_ITF_TYPE_LIF, out_lif_val);
            info->encap_id = 0;
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Result type %d in table %s is not supported.",
                     result_type, dbal_logical_table_to_string(unit, dbal_table_id));
    }

    /** Get additional forwarding data */
    if (L3_TABLE_IS_KBP(dbal_table_id))
    {
        uint32 data_value = 0;

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, *entry_handle_id, DBAL_FIELD_KBP_FORWARD_PMF_GENERIC_DATA, INST_SINGLE, &data_value));

        if (DNX_L3_RESULT_TYPE_IS_KBP_OPTIMIZED(dbal_table_id, result_type))
        {
            uint8 source_drop_value = 0;

            /** Get the LSB from Source drop */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                            (unit, *entry_handle_id, DBAL_FIELD_SOURCE_DROP, INST_SINGLE, &source_drop_value));

            /** Shift the taken PMF generic data value and place the Source drop value in the LSB */
            data_value = ((data_value << 0x1) | (source_drop_value & 0x1));
        }

        info->l3a_lookup_class = data_value;
    }

    /** In case of an entry with stats - retrieve stats configuration values */
    if (dnx_stat_pp_result_type_verify(unit, dbal_table_id, result_type) == _SHR_E_NONE)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, *entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD,
                                                            INST_SINGLE, &stat_pp_profile));

        /*
         * Get statistic info only if stat profile is valid
         */
        if (stat_pp_profile != STAT_PP_PROFILE_INVALID)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                                *entry_handle_id,
                                                                DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE,
                                                                &info->stat_id));
            STAT_PP_ENGINE_PROFILE_SET(info->stat_pp_profile, stat_pp_profile, bcmStatCounterInterfaceIngressReceivePp);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Adds/updates an IP host entry.
 *  Result types of the host that are currently supported:
 *          * FEC
 *          * FEC + EEP(outLIF)
 *          * Port + ETH-RIF + ARP
 *          * FEC + ETH-RIF + ARP
 *
 * \param [in] unit - The unit number.
 * \param [in] info - L3 host information (key and payload) to
 *        be added/updated.
 *      * info->l3a_ip_addr - IPv4 UC address of host;
 *      * info->l3a_ip6_addr - IPv6 UC address of host;
 *      * info->l3a_vrf - Virtual Router Interface;
 *      * info->l3a_flags - control flags
 *              *BCM_L3_IP6 - If set, determines that an IPv6
 *               entry is being defined;
 *              *BCM_L3_REPLACE - If set, an existing entry will
 *               be updated;
 *      * info->l3a_intf - L3 interface used as destination of
 *        the entry (FEC);
 *      * info->encap_id - encapsulation ID (out-LIF);
 *      * info->l3a_port_tgid - when using host format NO-FEC this field should include
 *        gport information, when this field is assigned info->l3a_intf should hold outrif and not fec.
 *
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of error.
 * \see
 *  EM Tables in which the entry may be added:
 *   * IPV6_UNICAST_PRIVATE_HOST
 *   * IPV4_UNICAST_PRIVATE_HOST
 *   * KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD
 */
int
bcm_dnx_l3_host_add(
    int unit,
    bcm_l3_host_t * info)
{
    uint32 entry_handle_id;
    uint32 l3_intf;
    uint32 encap_id, eei_val, dest_val;
    uint32 result_type;
    uint32 dbal_table, destination;
    dbal_enum_value_field_system_headers_mode_e system_headers_mode;
    int is_kbp_table = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Determine the correct table in which the host entry needs to be added to.
     * Allocate handle to that table that will be used for adding the entry.
     */
    SHR_IF_ERR_EXIT(dnx_l3_host_dbal_table_select(unit, info, &dbal_table));
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    is_kbp_table = L3_TABLE_IS_KBP(dbal_table);

    /** Verify the inputs.*/
    SHR_INVOKE_VERIFY_DNX(dnx_l3_host_add_verify(unit, info, dbal_table, system_headers_mode, is_kbp_table));

    l3_intf = BCM_L3_ITF_VAL_GET(info->l3a_intf);
    encap_id = BCM_FORWARD_ENCAP_ID_VAL_GET(info->encap_id);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    /*
     * Build the key of the entry that will be created.
     * Valid fields for it are control flags, VRF value, IP address and its mask.
     */
    SHR_IF_ERR_EXIT(dnx_l3_host_to_key(unit, info, &entry_handle_id));

    /*
     * Build destination result based on the defined fields in the bcm_l3_host_t.
     * Payload types that are currently supported:
     *          * FEC
     *          * FEC + EEP(outLIF)
     *          * {System-Port, OutRIF, ARP (No-FEC)}
     *          * FEC + ETH-RIF + ARP
     */
    /*
     * if !BCM_L3_ITF_TYPE_IS_FEC and encap_id != 0 then the host format is no-fec.
     * l3a_port_tgid must be assigned and not equal to zero.
     * in this case we assign system-port,outrif,arp to the host
     */
    if (info->l3a_port_tgid != BCM_GPORT_TYPE_NONE)
    {
        /**
         * Choose result type field for the dbal table.
         * l3a_intf must be LIF or RIF, encap_id can be NULL or LIF.
         * If the table is KBP, then only result types with one LIF are supported, stat is not possible.
         * Otherwise, either one or two LIFs can be supported.
         */
        if (is_kbp_table)
        {
            if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
            {
                result_type = DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_OUTLIF_W_DEFAULT;
            }
            else
            {
                result_type = DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_OUTLIF_W_DEFAULT;;
            }
        }
        else
        {
            if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
            {
                if (encap_id != 0)
                {
                    result_type = (info->stat_pp_profile != STAT_PP_PROFILE_INVALID) ?
                        DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_DOUBLE_OUTLIF_STAT :
                        DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_DOUBLE_OUTLIF;
                }
                else
                {
                    result_type = (info->stat_pp_profile != STAT_PP_PROFILE_INVALID) ?
                        DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF_STAT :
                        DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF;
                }
            }
            else
            {
                if (encap_id != 0)
                {
                    result_type = (info->stat_pp_profile != STAT_PP_PROFILE_INVALID) ?
                        DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_DOUBLE_OUTLIF_STAT :
                        DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_DOUBLE_OUTLIF;
                }
                else
                {
                    result_type = (info->stat_pp_profile != STAT_PP_PROFILE_INVALID) ?
                        DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF_STAT :
                        DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF;
                }
            }
        }

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
        if (is_kbp_table || encap_id == 0)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, l3_intf);
        }
        else
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, l3_intf);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF_2ND, INST_SINGLE, encap_id);
        }
        SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                        (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, info->l3a_port_tgid, &destination));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, destination);
    }
    else
    {
        /*
         * In case l3a_port_tgid is not set:
         * l3a_intf must be FEC, encap_id can be NULL, LIF or EEI.
         */
        if (encap_id == 0)
        {
            if (is_kbp_table)
            {
                if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
                {
                    result_type = (info->stat_pp_profile != STAT_PP_PROFILE_INVALID) ?
                        DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_DEST_STAT_W_DEFAULT :
                        DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_KBP_DEST_W_DEFAULT;
                }
                else
                {
                    result_type = (info->stat_pp_profile != STAT_PP_PROFILE_INVALID) ?
                        DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_DEST_STAT_W_DEFAULT :
                        DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_KBP_DEST_W_DEFAULT;
                }
            }
            else
            {
                if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
                {
                    result_type = (info->stat_pp_profile != STAT_PP_PROFILE_INVALID) ?
                        DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_STAT :
                        DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST;
                }
                else
                {
                    result_type = (info->stat_pp_profile != STAT_PP_PROFILE_INVALID) ?
                        DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_STAT :
                        DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST;
                }
            }
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC, INST_SINGLE, l3_intf);
        }
        else
        {
            /*
             * If encap_id != 0 and the FEC is valid, then the destination of the entry will resolve to
             * a FEC and an out-LIF.
             * If BCM_L3_ENCAP_SPACE_OPTIMIZED is set, encap_id must be EEI.
             */
            uint8 frwd_with_eei = 0;
            if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_ENCAP_SPACE_OPTIMIZED) ||
                (BCM_FORWARD_ENCAP_ID_IS_EEI(info->encap_id)
                 && (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)))
            {
                frwd_with_eei = 1;
            }

            if (is_kbp_table)
            {
                if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
                {
                    result_type = !frwd_with_eei ?
                        DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_OUTLIF_W_DEFAULT :
                        DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_DEST_EEI_W_DEFAULT;
                }
                else
                {
                    result_type = !frwd_with_eei ?
                        DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_OUTLIF_W_DEFAULT :
                        DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_DEST_EEI_W_DEFAULT;
                }
            }
            else
            {
                if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
                {
                    if (!frwd_with_eei)
                    {
                        result_type = (info->stat_pp_profile != STAT_PP_PROFILE_INVALID) ?
                            DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF_STAT :
                            DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF;
                    }
                    else
                    {
                        result_type = DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_EEI;
                    }
                }
                else
                {
                    if (!frwd_with_eei)
                    {
                        result_type = (info->stat_pp_profile != STAT_PP_PROFILE_INVALID) ?
                            DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF_STAT :
                            DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF;
                    }
                    else
                    {
                        result_type = DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_EEI;
                    }
                }
            }

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
            if (frwd_with_eei)
            {
                if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_ENCAP_SPACE_OPTIMIZED) &&
                    BCM_FORWARD_ENCAP_ID_EEI_USAGE_GET(info->encap_id) == BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT)
                {
                    /** EEI is a MPLS command.*/
                    uint32 field_in_struct_encoded_val;

                    field_in_struct_encoded_val = BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_VC_GET(info->encap_id);
                    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
                    {
                        /** In JR2 mode, only swap and pop commands are available. SWAP is using for PUSH for L3 routing into VPN.*/
                        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                                        (unit, DBAL_FIELD_EEI_MPLS_SWAP_COMMAND, DBAL_FIELD_MPLS_LABEL,
                                         &field_in_struct_encoded_val, &eei_val));
                        SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                                        (unit, DBAL_FIELD_EEI, DBAL_FIELD_EEI_MPLS_SWAP_COMMAND, &encap_id, &eei_val));
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                                        (unit, DBAL_FIELD_EEI_MPLS_PUSH_COMMAND, DBAL_FIELD_MPLS_LABEL,
                                         &field_in_struct_encoded_val, &eei_val));

                        field_in_struct_encoded_val =
                            BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_PUSH_PROFILE_GET(info->encap_id);
                        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                                        (unit, DBAL_FIELD_EEI_MPLS_PUSH_COMMAND, DBAL_FIELD_PUSH_CMD_ID,
                                         &field_in_struct_encoded_val, &eei_val));

                        SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                                        (unit, DBAL_FIELD_EEI, DBAL_FIELD_EEI_MPLS_PUSH_COMMAND, &encap_id, &eei_val));
                    }
                }
                /** JR1 mode only begain */
                else if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
                {
                    /** EEI is a EEDB pointer.*/
                    SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                                    (unit, DBAL_FIELD_EEI, DBAL_FIELD_EEI_ENCAPSULATION_POINTER, &encap_id, &eei_val));

                }
                /** JR1 mode only end */

                SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                                (unit, DBAL_FIELD_DESTINATION, DBAL_FIELD_FEC, &l3_intf, &dest_val));

                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EEI, INST_SINGLE, eei_val);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, dest_val);
            }
            else
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, encap_id);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC, INST_SINGLE, l3_intf);
            }
        }
    }

    if (is_kbp_table)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_DEFAULT, INST_SINGLE, 0);

        /** Set additional forwarding data */
        if (DNX_L3_RESULT_TYPE_IS_KBP_OPTIMIZED(dbal_table, result_type))
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_DROP, INST_SINGLE,
                                         (info->l3a_lookup_class & 0x1));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBP_FORWARD_PMF_GENERIC_DATA, INST_SINGLE,
                                         (info->l3a_lookup_class >> 0x1));
        }
        else
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBP_FORWARD_PMF_GENERIC_DATA, INST_SINGLE,
                                         info->l3a_lookup_class);
        }
    }

    if (dnx_stat_pp_result_type_verify(unit, dbal_table, result_type) == _SHR_E_NONE)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, info->stat_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE,
                                     STAT_PP_PROFILE_ID_GET(info->stat_pp_profile));
    }

    /*
     * Create the entry if it doesn't exist, update it if it does.
     */
    if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_REPLACE))
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_FORCE));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Find an IP host entry
 * \param [in] unit - The unit number.
 * \param [in,out] info -
 *      The l3 host structure describing the entry.\n
 *      As input:
 *          info.l3a_vrf - Virtual Router Instance \n
 *          info.l3a_flags - Control flags for route entries -
 *          BCM_L3_*** (Eg. BCM_L3_IP6, BCM_L3_HIT_CLEAR)\n
 *          info.l3a_ip_addr - IPv4 subnet address \n
 *          info.l3a_ip6_addr - IPv6 subnet address \n
 *      As output:
 *          info.l3a_intf - Destination interface is
 *          returned as FEC \n
 *          info.encap_id - Destination LIF returned as out-LIF if
 *          present \n
 * \return
 *   \retval Zero in case of no error.
 *   \retval Non-zero in case of an error.
 * \remark
 *   * receive information from LPM table:
 *      IPV4_UNICAST_PRIVATE_HOST
 *      IPV6_UNICAST_PRIVATE_HOST
 *      KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD
 *
 * \see
 *   * bcm_l3_host_t fields \n
 *   * BCM L3 Flags \n
 */
int
bcm_dnx_l3_host_find(
    int unit,
    bcm_l3_host_t * info)
{
    uint32 entry_handle_id;
    uint32 result_type;
    uint32 dbal_table;
    uint32 hit_bit = 0;
    uint32 attr_type = DBAL_ENTRY_ATTR_HIT_GET;
    int routed_learn_flags = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l3_host_verify(unit, info));

    /*
     * Determine the correct table in which the host entry needs to be looked for.
     * Allocate handle to that table that will be used for finding the entry.
     */
    SHR_IF_ERR_EXIT(dnx_l3_host_dbal_table_select(unit, info, &dbal_table));
    if (!L3_TABLE_IS_KBP(dbal_table))
    {
        SHR_IF_ERR_EXIT(dnx_switch_control_routed_learn_get(unit, &routed_learn_flags));
        if (routed_learn_flags &&
            dnx_data_l2.feature.feature_get(unit, dnx_data_l2_feature_opportunistic_learning_always_transplant))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Couldn't find lem host table entry when routing learning is enabled.");
        }
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    /*
     * Build host entry key based on flags, VRF value and IP address.
     */
    SHR_IF_ERR_EXIT(dnx_l3_host_to_key(unit, info, &entry_handle_id));

    /*
     * KBP doesn't support hit bit
     */
    if (!L3_TABLE_IS_KBP(dbal_table))
    {
        if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_HIT_CLEAR))
        {
            attr_type |= DBAL_ENTRY_ATTR_HIT_CLEAR;
        }
        SHR_IF_ERR_EXIT(dbal_entry_attribute_request(unit, entry_handle_id, attr_type, &hit_bit));
    }
    /*
     * Getting all destination fields of the entry
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &result_type));

    if (hit_bit)
    {
        info->l3a_flags |= BCM_L3_HIT;
    }

    /** Copy the specific field value to a given pointer based on the result type */
    SHR_IF_ERR_EXIT(dnx_l3_host_resolve_destination(unit, result_type, dbal_table, &entry_handle_id, info));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Delete an IP host entry
 * \param [in] unit - The unit number.
 * \param [in] ip_addr -
 *      The l3 host structure describing the entry.\n
 *      ip_addr.l3a_vrf - Virtual Router Instance \n
 *      ip_addr.l3a_flags - Control flags for route entries
 *      defined as BCM_L3_*** (Eg. BCM_L3_IP6, BCM_L3_IPMC,
 *      BCM_L3_MULTIPATH, etc.)\n
 *      ip_addr.l3a_ip_addr - IPv4 subnet address\n
 *      ip_addr.l3a_ip6_addr - IPv6 subnet address\n
 * \return
 *   \retval Zero in case of no error.
 *   \retval Non-zero in case of an error.
 * \see
 *   * info above \n
 */
int
bcm_dnx_l3_host_delete(
    int unit,
    bcm_l3_host_t * ip_addr)
{
    uint32 entry_handle_id;
    uint32 dbal_table;
    int routed_learn_flags = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l3_host_verify(unit, ip_addr));

    /*
     * Determine the correct table in which the host entry needs to be added to.
     * Allocate handle to that table that will be used for adding the entry.
     */
    SHR_IF_ERR_EXIT(dnx_l3_host_dbal_table_select(unit, ip_addr, &dbal_table));
    if (!L3_TABLE_IS_KBP(dbal_table))
    {
        SHR_IF_ERR_EXIT(dnx_switch_control_routed_learn_get(unit, &routed_learn_flags));
        if (routed_learn_flags &&
            dnx_data_l2.feature.feature_get(unit, dnx_data_l2_feature_opportunistic_learning_always_transplant))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Couldn't delete lem host table entry when routing learning is enabled.");
        }
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    /*
     * Build the key of the entry that will be created.
     * Valid fields for it are control flags, VRF value, IP address and its mask.
     */
    SHR_IF_ERR_EXIT(dnx_l3_host_to_key(unit, ip_addr, &entry_handle_id));

    /** clearing the entry */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Indicates if a KBP dbal table is valid according to the route info
 * \param [in]  unit - The unit number.
 * \param [in]  info - The l3 route structure.
 * \param [in]  dbal_table - The dbal table ID
 * \param [out] is_valid - The indication if the KBP table is enabled
 * \return
 *   \retval Zero in case of no error.
 *   \retval Non-zero in case of an error.
 * \see
 *   * bcm_l3_route_t fields \n
 *   * BCM L3 Flags \n
 */
static shr_error_e
dnx_l3_route_kbp_table_is_valid(
    int unit,
    bcm_l3_route_t * info,
    dbal_tables_e dbal_table,
    uint8 *is_valid)
{
    uint32 kbp_mngr_status;
    uint8 use_kbp_ipv4 = dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4) ? TRUE : FALSE;
    uint8 use_kbp_ipv6 = dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv6) ? TRUE : FALSE;
    uint8 kbp_ipv4_public_enabled = FALSE;
    uint8 kbp_ipv6_public_enabled = FALSE;
    uint8 kbp_split_fwd_rpf = kbp_mngr_split_rpf_in_use(unit);

    SHR_FUNC_INIT_VARS(unit);

    (*is_valid) = FALSE;

    /** No KBP Device */
    if (!dnx_kbp_device_enabled(unit))
    {
        SHR_EXIT();
    }

    if (!L3_TABLE_IS_KBP(dbal_table))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The provided dbal table is not a KBP table\n");
    }

    /** Get the KBP device status */
    SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));

    SHR_IF_ERR_EXIT(kbp_mngr_ipv4_public_enabled_get(unit, &kbp_ipv4_public_enabled));
    SHR_IF_ERR_EXIT(kbp_mngr_ipv6_public_enabled_get(unit, &kbp_ipv6_public_enabled));

    /** The table should pass all the checks to be considered enabled */

    /** Flags should be checked */
    if (info != NULL)
    {
        if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
        {
            /** Table is IPv4 or KBP IPv6 application is not enabled */
            if (L3_TABLE_IS_KBP_IPV4(dbal_table) || !use_kbp_ipv6)
            {
                SHR_EXIT();
            }
            /** IPv6 tables remain for this check; Table is public, but IPv6 public is not enabled */
            if (L3_TABLE_IS_PUBLIC(dbal_table) && !kbp_ipv6_public_enabled)
            {
                SHR_EXIT();
            }
        }
        else
        {
            /** Table is IPv6 or KBP IPv4 application is not enabled */
            if (L3_TABLE_IS_KBP_IPV6(dbal_table) || !use_kbp_ipv4)
            {
                SHR_EXIT();
            }
            /** IPv4 tables remain for this check; Table is public, but IPv4 public is not enabled */
            if (L3_TABLE_IS_PUBLIC(dbal_table) && !kbp_ipv4_public_enabled)
            {
                SHR_EXIT();
            }
        }
        /** Table is RPF, but FWD and RPF are not split or RPF only flag is not set */
        if (L3_TABLE_IS_RPF(dbal_table))
        {
            if (!kbp_split_fwd_rpf || !_SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_RPF_ONLY))
            {
                SHR_EXIT();
            }
        }
        /** Table is FWD, but FWD and RPF are split and the FWD only flag is not set */
        else if (kbp_split_fwd_rpf && !_SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_FWD_ONLY))
        {
            SHR_EXIT();
        }
    }
    /** No flags are checked */
    else
    {
        /** Table is IPv4 or IPv6, but applications are not enabled */
        if ((L3_TABLE_IS_KBP_IPV4(dbal_table) && !use_kbp_ipv4) || (L3_TABLE_IS_KBP_IPV6(dbal_table) && !use_kbp_ipv6))
        {
            SHR_EXIT();
        }
        /** Table is public, but public for IPv4 or IPv6 is not enabled */
        if (L3_TABLE_IS_PUBLIC(dbal_table)
            && ((L3_TABLE_IS_KBP_IPV4(dbal_table) && !kbp_ipv4_public_enabled)
                || (L3_TABLE_IS_KBP_IPV6(dbal_table) && !kbp_ipv6_public_enabled)))
        {
            SHR_EXIT();
        }
        /** Table is RPF, but FWD and RPF are not split */
        if (L3_TABLE_IS_RPF(dbal_table) && !kbp_split_fwd_rpf)
        {
            SHR_EXIT();
        }
    }

    if (kbp_mngr_status != DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "KBP device in use, but not locked\n");
    }

    (*is_valid) = TRUE;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Clear the L3 IP route table
 * \param [in] unit - The unit number.
 * \param [in] info -
 *      The l3 route structure.\n
 *      info.l3a_flags - Control flags for route entries -
 *          * BCM_L3_IP6 - If flag is provided, all IPv6 route
 *            entries will be deleted. If it is not given, IPv4
 *            route table will be cleared
 *      info.l3a_vrf - Vritual router instance
 * \return
 *   \retval Zero in case of no error.
 *   \retval Non-zero in case of an error.
 * \see
 *   * bcm_l3_route_t fields \n
 *   * BCM L3 Flags \n
 */
int
bcm_dnx_l3_route_delete_all(
    int unit,
    bcm_l3_route_t * info)
{
    uint32 *dbal_tables_to_delete;
    
    uint32 dbal_tables_ipv4[] = {
        DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD,
        DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD_2,
        /** KBP split tables must be last in the IPv4/IPv6 list for the rest of the logic to work. */
        DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD,
        DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF,
    };
    uint32 dbal_tables_ipv6[] = {
        DBAL_TABLE_IPV6_UNICAST_PRIVATE_LPM_FORWARD,
        DBAL_TABLE_IPV6_UNICAST_PRIVATE_LPM_FORWARD_2,
        DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD,
        DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF
    };
    uint32 table = 0;
    uint32 nof_tables;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * If info is empty, then both IPv4 and IPv6 tables will be cleared.
     * If info is not empty, then clearance will be made according to BCM_L3_IP6 flag
     */
    if (info != NULL)
    {
        if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
        {
            /** Only IPv6 routes will be cleared */
            dbal_tables_to_delete = (uint32 *) sal_alloc(sizeof(dbal_tables_ipv6), "IPv6 tables to delete");
            sal_memcpy(dbal_tables_to_delete, dbal_tables_ipv6, sizeof(dbal_tables_ipv6));
            nof_tables = sizeof(dbal_tables_ipv6) / sizeof(dbal_tables_ipv6[0]);
        }
        else
        {
            /** Only IPv4 routes will be cleared */
            dbal_tables_to_delete = (uint32 *) sal_alloc(sizeof(dbal_tables_ipv4), "IPV4 tables to delete");
            sal_memcpy(dbal_tables_to_delete, dbal_tables_ipv4, sizeof(dbal_tables_ipv4));
            nof_tables = sizeof(dbal_tables_ipv4) / sizeof(dbal_tables_ipv4[0]);
        }
    }
    else
    {
        dbal_tables_to_delete =
            (uint32 *) sal_alloc(sizeof(dbal_tables_ipv6) + sizeof(dbal_tables_ipv4), "All tables to delete");
        sal_memcpy(dbal_tables_to_delete, dbal_tables_ipv4, sizeof(dbal_tables_ipv4));
        nof_tables = sizeof(dbal_tables_ipv4) / sizeof(dbal_tables_ipv4[0]);
        sal_memcpy(&dbal_tables_to_delete[nof_tables], dbal_tables_ipv6, sizeof(dbal_tables_ipv6));
        nof_tables += sizeof(dbal_tables_ipv6) / sizeof(dbal_tables_ipv6[0]);
    }

    /*
     * If the pointer to bcm_l3_route_t structure is empty, all route entries are going to be cleared.
     * Otherwise, public and private table will be cleared based on the BCM_L3_IP6 flag -
     * if it is set, then public and private IPv6 tables will be cleared;
     * if it is not set, then public and private IPv4 tables will be cleared.
     */
    /** clear tables */
    for (; table < nof_tables; table++)
    {
        if (L3_TABLE_IS_KBP(dbal_tables_to_delete[table]))
        {
            uint8 is_valid;
            SHR_IF_ERR_EXIT(dnx_l3_route_kbp_table_is_valid(unit, info, dbal_tables_to_delete[table], &is_valid));
            if (!is_valid)
            {
                continue;
            }
        }

        SHR_IF_ERR_EXIT(dbal_table_clear(unit, dbal_tables_to_delete[table]));
    }

exit:
    if (dbal_tables_to_delete != NULL)
    {
        sal_free(dbal_tables_to_delete);
    }
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Delete all L3 IP host entries in DNX devices.
 * \param [in] unit - The unit number.
 * \param [in] info -
 *      The l3 route structure bearing the control flags
 *      info.l3a_flags - Control flags for route entries -
 *          * BCM_L3_IP6 - If present, all IPv6 host table
 *            will be cleared. If it is not present, all IPv4
 *            host entries will be deleted.
 * \return
 *   \retval Zero in case of no error.
 *   \retval Non-zero in case of an error.
 * \remark
 *   * clear all information from LPM tables:
 *      DBAL_TABLE_IPV4_UNICAST_PRIVATE_HOST
 *      DBAL_TABLE_IPV6_UNICAST_PRIVATE_HOST
 * \see
 *   * BCM L3 Flags \n
 */
int
bcm_dnx_l3_host_delete_all(
    int unit,
    bcm_l3_host_t * info)
{
    uint32 dbal_table[4] = {
        DBAL_TABLE_IPV4_UNICAST_PRIVATE_HOST,
        DBAL_TABLE_IPV6_UNICAST_PRIVATE_HOST,
        DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD,
        DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD
    };
    uint32 table;
    uint32 nof_tables = 4;
    uint8 use_kbp_ipv4 = dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4) ? TRUE : FALSE;
    uint8 use_kbp_ipv6 = dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv6) ? TRUE : FALSE;
    int routed_learn_flags = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_switch_control_routed_learn_get(unit, &routed_learn_flags));
    if (routed_learn_flags &&
        dnx_data_l2.feature.feature_get(unit, dnx_data_l2_feature_opportunistic_learning_always_transplant) &&
        !(use_kbp_ipv4 || use_kbp_ipv6))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "After enabling routing learning, couldn't call host delete all function in no-KBP device.");
    }

    /*
     * If the pointer to bcm_l3_host_t structure is empty, all host entries are going to be cleared.
     * Otherwise, tables will be cleared based on the BCM_L3_IP6 flag -
     * if it is set, then IPv6 tables will be cleared;
     * if it is not set, then IPv4 tables will be cleared.
     */
    /** clear tables */
    for (table = 0; table < nof_tables; table++)
    {
        if (info != NULL)
        {
            if ((_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6)
                 &&
                 ((dbal_table[table] == DBAL_TABLE_IPV4_UNICAST_PRIVATE_HOST
                   || dbal_table[table] == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD)
                  || (dbal_table[table] == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD && !use_kbp_ipv6)))
                || (!_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6)
                    &&
                    ((dbal_table[table] == DBAL_TABLE_IPV6_UNICAST_PRIVATE_HOST
                      || dbal_table[table] == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD)
                     || (dbal_table[table] == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD && !use_kbp_ipv4))))
            {
                continue;
            }
        }
        else
        {
            if ((dbal_table[table] == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD && !use_kbp_ipv4) ||
                (dbal_table[table] == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD && !use_kbp_ipv6))
            {
                continue;
            }
        }
        SHR_IF_ERR_EXIT(dbal_table_clear(unit, dbal_table[table]));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Traverse all L3 IP host entries in DNX devices and call a
 *  function with the data for each entry found.
 * \param [in] unit - The unit number.
 * \param [in] flags - control flags that indicate choice of
 *        table to be traversed:
 *          * BCM_L3_IP6 - if provided, it indicates that the
 *            IPv6 host table will be traversed, otherwise - the
 *            IPv4 host entries will be gone over.
 * \param [in] start - The index of the first valid entry that
 *        will be processed.
 * \param [in] end - The index of the last valid entry that will
 *        be processed
 * \param [in] cb - the name of the callback function that is
 *        going to be called for each valid entry
 * \param [in] user_data - data that is passed to the callback
 *        function
 * \return
 *   \retval Zero in case of no error.
 *   \retval Non-zero in case of an error.
 * \remark
 *      DBAL_TABLE_IPV4_UNICAST_PRIVATE_HOST
 *      DBAL_TABLE_IPV6_UNICAST_PRIVATE_HOST
 *      DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD
 *      DBAL_TABLE_
 * \see
 *   * BCM L3 Flags \n
 */
int
bcm_dnx_l3_host_traverse(
    int unit,
    uint32 flags,
    uint32 start,
    uint32 end,
    bcm_l3_host_traverse_cb cb,
    void *user_data)
{
    uint32 entry_handle_id;
    uint32 dbal_table;
    uint32 index = 0;
    uint32 *field_value;
    uint8 *field_value_eight_bits;
    int is_end;
    bcm_l3_host_t dbal_entry;
    uint32 result_type;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    field_value = NULL;
    field_value_eight_bits = NULL;
    /*
     * Allocate maximum size of array that may be used.
     */
    SHR_ALLOC(field_value, (sizeof(bcm_ip6_t) / sizeof(uint8)) * sizeof(uint32),
              "Array for reading table entry key or value", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(field_value_eight_bits, (sizeof(bcm_ip6_t) / sizeof(uint8)) * sizeof(uint8),
              "Array for reading table entry key or value", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_INVOKE_VERIFY_DNX(dnx_l3_host_traverse_verify(unit, flags));
    /*
     * Determine the correct table from which all host entries will be traversed based on the BCM_L3_IP6 flag.
     * If the BCM_IPMC_IP6 flag is not set, then IPv4 host table will be traversed.
     */
    dbal_entry.l3a_flags = flags;
    SHR_IF_ERR_EXIT(dnx_l3_host_dbal_table_select(unit, &dbal_entry, &dbal_table));

    /*
     * Allocate handle to the table of the iteration and initialise an iterator entity.
     * The iterator is in mode ALL, which means that it will consider all entries regardless
     * of them being default entries or not.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
    /*
     * Receive first entry in table.
     */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        bcm_l3_host_t_init(&dbal_entry);

        /*
         * Receive key and value fields of the entry.
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_VRF, field_value));
        dbal_entry.l3a_vrf = field_value[0];
        /*
         * If the BCM_L3_IP6 flag is set, then IPv6 address will be extracted from the key.
         * Otherwise the IPv4 address will be taken.
         */
        if (_SHR_IS_FLAG_SET(flags, BCM_L3_IP6))
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_get
                            (unit, entry_handle_id, DBAL_FIELD_IPV6, field_value_eight_bits));
            sal_memcpy(dbal_entry.l3a_ip6_addr, field_value_eight_bits, sizeof(bcm_ip6_t));
            dbal_entry.l3a_flags = BCM_L3_IP6;
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_IPV4, field_value));
            dbal_entry.l3a_ip_addr = field_value[0];
        }

        /*
         * Receive the result type of the entry and based on it will assign result values.
         * If the result type is HOST_DEST_OUTLIF then both the FEC and the out-LIF are used as result.
         * If the result type is FWD_DEST then only FEC value will be taken.
         * If the result type is neither one of those, then an error will be returned.
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                            &result_type));
        SHR_IF_ERR_EXIT(dnx_l3_host_resolve_destination(unit, result_type, dbal_table, &entry_handle_id, &dbal_entry));

        /*
         * If user provided a name of the callback function, it will be invoked with sending the dbal_entry structure
         * of the entry that was found.
         */
        if (cb != NULL)
        {
            /*
             * Invoke callback function
             */
            SHR_IF_ERR_EXIT((*cb) (unit, index, &dbal_entry, user_data));
        }
        /*
         * Receive next entry in table.
         */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        index++;
    }

exit:
    SHR_FREE(field_value);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Traverse all L3 IP route entries in DNX devices and call a
 *  function with the data for each entry found.
 * \param [in] unit - The unit number.
 * \param [in] flags - control flags that indicate choice of
 *        table to be traversed:
 *          * BCM_L3_IP6 - if provided, it indicates that the
 *            IPv6 route tables will be traversed, otherwise -
 *            the IPv4 route entries will be gone over.
 * \param [in] start - The index of the first valid entry that
 *        will be processed.
 * \param [in] end - The index of the last valid entry that will
 *        be processed
 * \param [in] trav_fn - the name of the callback function that
 *        is going to be called for each valid entry
 * \param [in] user_data - data that is passed to the callback
 *        function
 * \return
 *   \retval Zero in case of no error.
 *   \retval Non-zero in case of an error.
 * \remark
 *      DBAL_TABLE_IPV6_UNICAST_PRIVATE_LPM_FORWARD_2
 *      DBAL_TABLE_IPV6_UNICAST_PRIVATE_LPM_FORWARD
 *      DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD_2
        DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD
 * \see
 *   * BCM L3 Flags \n
 */
int
bcm_dnx_l3_route_traverse(
    int unit,
    uint32 flags,
    uint32 start,
    uint32 end,
    bcm_l3_route_traverse_cb trav_fn,
    void *user_data)
{
    uint32 entry_handle_id;
    uint32 kbp_mngr_status;
    uint32 dbal_tables[MAX_NOF_L3_ROUTE_TABLES];
    uint32 nof_tables;
    uint32 table;
    uint32 index;
    uint32 *field_value;
    uint32 result;
    int is_end;
    bcm_l3_route_t dbal_entry;
    uint32 table_hb_enabled;
    uint32 hitbit = 0;
    uint32 hitbit_flags = DBAL_ENTRY_ATTR_HIT_GET | DBAL_ENTRY_ATTR_HIT_PRIMARY | DBAL_ENTRY_ATTR_HIT_SECONDARY;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    field_value = NULL;
    nof_tables = 0;
    /*
     * Allocate maximum size of array that may be used.
     */
    SHR_ALLOC(field_value, (sizeof(bcm_ip6_t) / sizeof(uint8)) * sizeof(uint32),
              "Array for reading table entry key or value", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_INVOKE_VERIFY_DNX(dnx_l3_route_traverse_verify(unit, flags));

    /** Get the KBP device status */
    SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));

    /*
     * Determine the correct table from which all host entries will be traversed based on the BCM_L3_IP6 flag.
     * If the BCM_IPMC_IP6 flag is not set, then IPv4 host table will be traversed.
     */
    if (_SHR_IS_FLAG_SET(flags, BCM_L3_IP6))
    {
        dbal_tables[nof_tables++] = DBAL_TABLE_IPV6_UNICAST_PRIVATE_LPM_FORWARD_2;
        dbal_tables[nof_tables++] = DBAL_TABLE_IPV6_UNICAST_PRIVATE_LPM_FORWARD;

        if (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv6))
        {
            if ((kbp_mngr_status != DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "KBP device in use, but not locked\n");
            }
            dbal_tables[nof_tables++] = DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD;
            if (kbp_mngr_split_rpf_in_use(unit))
            {
                dbal_tables[nof_tables++] = DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF;
            }
        }
    }
    else
    {
        dbal_tables[nof_tables++] = DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD;
        dbal_tables[nof_tables++] = DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD_2;

        if (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4))
        {
            if ((kbp_mngr_status != DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "KBP device in use, but not locked\n");
            }
            dbal_tables[nof_tables++] = DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD;
            if (kbp_mngr_split_rpf_in_use(unit))
            {
                dbal_tables[nof_tables++] = DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF;
            }
        }
    }
    /*
     * Allocate DBAL handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_tables[0], &entry_handle_id));
    /*
     * Iterate over all tables of the selected IP version.
     */
    for (table = 0; table < nof_tables; table++)
    {
        index = 0;
        /*
         * Clear handle to the table of the iteration and initialize an iterator entity.
         * The iterator is in mode ALL, which means that it will consider all entries regardless
         * of them being default entries or not.
         *
         * If working on the first table, need to use DBAL_HANDLE_ALLOC.
         * Otherwise we change to which table the handle belongs using DBAL_HANDLE_CLEAR.
         */
        if (table == 0)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_tables[table], &entry_handle_id));
        }
        else
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_tables[table], entry_handle_id));
        }
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
        SHR_IF_ERR_EXIT(dbal_tables_indication_get
                        (unit, dbal_tables[table], DBAL_TABLE_IND_IS_HITBIT_EN, &table_hb_enabled));
        if (table_hb_enabled)
        {
            SHR_IF_ERR_EXIT(dbal_iterator_attribute_action_add(unit, entry_handle_id, hitbit_flags));
            if (_SHR_IS_FLAG_SET(flags, BCM_L3_HIT_CLEAR))
            {
                SHR_IF_ERR_EXIT(dbal_iterator_attribute_action_add(unit, entry_handle_id, DBAL_ENTRY_ATTR_HIT_CLEAR));
            }
        }
        /*
         * Receive first entry in table.
         */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        /*
         * is_end variable will be set when the last entry is traversed and iterator_get_next is called again.
         */
        while (!is_end)
        {
            bcm_l3_route_t_init(&dbal_entry);

            result = L3_TABLE_IS_KBP(dbal_tables[table]) ? DBAL_FIELD_DESTINATION : DBAL_FIELD_KAPS_RESULT;
            /*
             * Public tables do not have VRF field (VRF=0)
             */
            if (L3_TABLE_IS_PUBLIC(dbal_tables[table]))
            {
                dbal_entry.l3a_vrf = 0;
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_VRF, field_value));
                dbal_entry.l3a_vrf = field_value[0];
            }
            /*
             * If the BCM_L3_IP6 flag is set, then IPv6 address subnet value will be extracted.
             * Otherwise, the IPv4 subnet value will be extracted.
             */
            if (_SHR_IS_FLAG_SET(flags, BCM_L3_IP6))
            {
                dbal_entry.l3a_flags = BCM_L3_IP6;
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_masked_get
                                (unit, entry_handle_id, DBAL_FIELD_IPV6, dbal_entry.l3a_ip6_net,
                                 dbal_entry.l3a_ip6_mask));
            }
            else
            {
                 /* coverity[callee_ptr_arith:FALSE]  */
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                                (unit, entry_handle_id, DBAL_FIELD_IPV4, &dbal_entry.l3a_subnet,
                                 &dbal_entry.l3a_ip_mask));
            }

            /*
             * KBP routes are split to FWD and RPF.
             * Update the flags2 with BCM_L3_FLAGS2_FWD_ONLY and BCM_L3_FLAGS2_RPF_ONLY accordingly
             */
            if (L3_TABLE_IS_KBP(dbal_tables[table]))
            {
                if (L3_TABLE_IS_RPF(dbal_tables[table]))
                {
                    dbal_entry.l3a_flags2 |= BCM_L3_FLAGS2_RPF_ONLY;
                }
                else
                {
                    uint32 result_type;
                    uint32 stat_pp_profile;

                    dbal_entry.l3a_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;

                    /** check if RT include stat information */
                    /** Get entry result type */
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &result_type));

                    /** In case of an entry with stats - retrieve stats configuration values */
                    if (dnx_stat_pp_result_type_verify(unit, dbal_tables[table], result_type) == _SHR_E_NONE)
                    {
                        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                        (unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE,
                                         &stat_pp_profile));
                        /*
                         * Get statistic info only if stat profile is valid
                         */
                        if (stat_pp_profile != STAT_PP_PROFILE_INVALID)
                        {
                            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                                                entry_handle_id,
                                                                                DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE,
                                                                                &dbal_entry.stat_id));
                            STAT_PP_ENGINE_PROFILE_SET(dbal_entry.stat_pp_profile, stat_pp_profile,
                                                       bcmStatCounterInterfaceIngressReceivePp);
                        }
                    }
                    else
                    {
                        /** Check if EEI is valid in the entry.*/
                        if ((result_type ==
                             DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_DEST_EEI_W_DEFAULT)
                            || (result_type ==
                                DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_DEST_EEI_W_DEFAULT))
                        {
                            uint32 eei_val, sub_field_id, sub_field_val, mpls_val;

                            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                            (unit, entry_handle_id, DBAL_FIELD_EEI, INST_SINGLE, &eei_val));
                            SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                                            (unit, DBAL_FIELD_EEI, eei_val, &sub_field_id, &sub_field_val));
                            if (sub_field_id == DBAL_FIELD_EEI_MPLS_SWAP_COMMAND
                                || sub_field_id == DBAL_FIELD_EEI_MPLS_PUSH_COMMAND)
                            {
                                SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                                                (unit, sub_field_id, DBAL_FIELD_MPLS_LABEL, &sub_field_val, &mpls_val));

                                dbal_entry.l3a_flags |= BCM_L3_ENCAP_SPACE_OPTIMIZED;
                                dbal_entry.l3a_mpls_label = mpls_val;
                            }
                            else
                            {
                                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal EEI format (%s) is found in route table.\n",
                                             dbal_field_to_string(unit, sub_field_id));
                            }
                        }
                    }
                }
            }

            /*
             * If the current table is a KAPS one, then the result needs to be received from either field FEC or field
             * FEC_DEFAULT
             */
            if (result == DBAL_FIELD_KAPS_RESULT)
            {
                /*
                 * Verify whether the entry is default or not based on the l3a_ip6_mask(IPv6) or l3a_ip_mask(IPv4).
                 * If the mask indicates a default entry, the result will be received from the FEC_DEFAULT field.
                 */
                if (_SHR_IS_FLAG_SET(dbal_entry.l3a_flags, BCM_L3_IP6))
                {
                    result =
                        L3_FRWRD_TABLES_IS_IPV6_DEFAULT_ENTRY(dbal_entry.l3a_ip6_mask) ? DBAL_FIELD_FEC_DEFAULT :
                        DBAL_FIELD_FEC;
                }
                else
                {
                    result =
                        L3_FRWRD_TABLES_IS_IPV4_DEFAULT_ENTRY(dbal_entry.l3a_ip_mask) ? DBAL_FIELD_FEC_DEFAULT :
                        DBAL_FIELD_FEC;
                }
            }

            /** Receive result of entry */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                            (unit, entry_handle_id, result, INST_SINGLE, field_value));
            BCM_L3_ITF_SET(dbal_entry.l3a_intf, BCM_L3_ITF_TYPE_FEC, field_value[0]);
            /** If hitbit indication is enabled for this table, receive the attribute and set the BCM_L3_HIT flag if the hitbit is raised. */
            if (table_hb_enabled)
            {
                SHR_IF_ERR_EXIT(dbal_entry_attribute_get(unit, entry_handle_id, hitbit_flags, &hitbit));
                if (hitbit != 0)
                {
                    dbal_entry.l3a_flags |= BCM_L3_HIT;
                }
            }

            /** Set the flag for the RPF databases, when the payload data is all zeroes */
            if (L3_TABLE_IS_RPF(dbal_tables[table]))
            {
                uint8 source_drop = FALSE;

                if (kbp_mngr_split_rpf_in_use(unit))
                {
                    uint8 is_kbp_default;

                    dbal_entry_handle_key_field_arr8_get(unit, entry_handle_id, DBAL_FIELD_IS_DEFAULT, &is_kbp_default);

                    if (!is_kbp_default && !dbal_entry.l3a_intf)
                    {
                        dbal_entry.l3a_flags2 |= BCM_L3_FLAGS2_NO_PAYLOAD;
                    }
                }

                dbal_entry_handle_key_field_arr8_get(unit, entry_handle_id, DBAL_FIELD_SOURCE_DROP, &source_drop);
                if (source_drop)
                {
                    dbal_entry.l3a_flags |= BCM_L3_SRC_DISCARD;
                }
            }

            if (trav_fn != NULL)
            {
                /*
                 * Invoke callback function
                 */
                SHR_IF_ERR_EXIT((*trav_fn) (unit, index, &dbal_entry, user_data));
            }
            /*
             * Receive next entry in table.
             */
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
            index++;
        }
    }

exit:
    SHR_FREE(field_value);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
