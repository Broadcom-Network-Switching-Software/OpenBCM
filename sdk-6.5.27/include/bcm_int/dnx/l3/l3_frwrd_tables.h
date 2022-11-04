/** \file bcm_int/dnx/l3/l3_frwrd_tables.h Internal DNX L3 APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef L3_FRWRD_TABLES_H_INCLUDED
/*
 * {
 */
#define L3_FRWRD_TABLES_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX only!"
#endif

/*
 * Include files.
 * {
 */
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/mdb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/stat/stat_pp.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <bcm_int/dnx/mpls/mpls.h>
#include <bcm_int/dnx/switch/switch.h>
#include <bcm_int/dnx/l3/l3.h>
#include <bcm/types.h>
#include <bcm/l3.h>
/*
 * }
 */

/*
 * DEFINES
 * {
 */

/**stat_id is 17 bits long */
#define L3_FRWRD_TABLES_MAX_STAT_ID_VALUE 0x1FFFF
/*
 * }
 */
/*
 * Enumeration.
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
 * Whether an IPv4 entry is default or not, relevant only for route entries.
 */
#define L3_FRWRD_TABLES_IS_IPV4_DEFAULT_ENTRY(ip_mask) (ip_mask == 0)
/*
 * Whether an IPv6 entry is default or not, relevant only for route entries.
 * Assuming that the mask is valid if the first byte of the mask is 0 then all the other bytes should
 * be as well hence this is a default route.
 */
#define L3_FRWRD_TABLES_IS_IPV6_DEFAULT_ENTRY(ip_mask) (ip_mask[0] == 0)

#define DNX_L3_ENTRY_IS_DEFAULT(_unit_,_info_) \
    (dnx_data_l3.feature.feature_get(_unit_, dnx_data_l3_feature_default_fec_limitation) && \
    ((!_SHR_IS_FLAG_SET(_info_->l3a_flags, BCM_L3_IP6) && L3_FRWRD_TABLES_IS_IPV4_DEFAULT_ENTRY(_info_->l3a_ip_mask)) || \
     (_SHR_IS_FLAG_SET(_info_->l3a_flags, BCM_L3_IP6) && L3_FRWRD_TABLES_IS_IPV6_DEFAULT_ENTRY(_info_->l3a_ip6_mask))))
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
#define L3_TABLE_IS_KBP_RPF(table) ( \
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

/** Determine whether the FEC ID is in the valid range */
#define DNX_L3_FEC_IS_IN_RANGE(unit, fec_index) (fec_index < dnx_data_l3.fec.nof_fecs_get(unit) && fec_index >= dnx_data_l3.fec.first_valid_fec_ecmp_id_get(unit))

/** Define for KBP additional forwarding data size */
#define DNX_L3_MAX_FWD_PMF_GENERIC_DATA_VALUE (SAL_BIT(10) - 1)
#define DNX_L3_MAX_RPF_PMF_GENERIC_DATA_VALUE (SAL_BIT(8) - 1)
#define DNX_L3_MAX_FWD_MPLS_PMF_GENERIC_DATA_VALUE (SAL_BIT(21) - 1)

#define DNX_L3_RESULT_TYPE_IS_KBP_OPTIMIZED(_dbal_table_, _result_type_) \
    (((_dbal_table_ == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD) && (_result_type_ == DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_KBP_DEST_W_DEFAULT)) || \
     ((_dbal_table_ == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD) && (_result_type_ == DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_KBP_DEST_W_DEFAULT)) || \
     ((_dbal_table_ == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF) && (_result_type_ == DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF_KBP_DEST_W_DEFAULT)) || \
     ((_dbal_table_ == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF) && (_result_type_ == DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF_KBP_DEST_W_DEFAULT)) ? TRUE : FALSE)

/*
 * }
 */
/*
 * Globals
 * {
 */
extern const dbal_entry_action_flags_e l3_frwrd_table_commit_actions[];
/*
 * }
 */
/*
 * Structures
 * {
 */

/*
 * }
 */
/*
 * Externs
 * {
 */

/*
 * }
 */
/*
 * Function declaration
 * {
 */
/**
 * \brief
 *   Validate the result size using the field size
 *   Also check the table field size in case it's truncated.
 * \param [in] unit - The unit number.
 * \param [in] dbal_table - The DBAL table where the entry will be added.
 * \param [in] result_type - The result type index of the entry.
 * \param [in] dest_type - The returned destination type resolved from the DBAL table and key.
 * \param [in] dest_value - The destination value.
 * \return
 *  * Zero for success
 * \remark
 * \see
 */
shr_error_e dnx_l3_route_validate_result_by_fields_size(
    int unit,
    uint32 dbal_table,
    uint32 result_type,
    dbal_fields_e * dest_type,
    uint32 dest_value);

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
shr_error_e dnx_l3_route_payload_raw_resolve_get(
    int unit,
    bcm_l3_route_t * info,
    uint8 is_kbp_entry,
    dbal_fields_e dest_type,
    dbal_fields_e dbal_sub_field,
    uint32 encoded_payload,
    uint32 raw_payload);

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
shr_error_e dnx_l3_route_key_set(
    int unit,
    bcm_l3_route_t * info,
    uint32 entry_handle_id);

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
shr_error_e dnx_l3_route_key_verify(
    int unit,
    bcm_l3_route_t * info);

/**
 * \brief
 *  Verify the route entry add/get/delete flags input.
 *  Checks the use of KBP-only flags.
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
shr_error_e dnx_l3_route_flags_verify(
    int unit,
    bcm_l3_route_t * info);

 /**
  * \brief
  *  Delete an IP KBP route entry
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
shr_error_e dnx_l3_kbp_route_delete(
    int unit,
    bcm_l3_route_t * info);

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
 * \param [in] is_traverse -
 *   In case of traverse, entry is retreived by dbal_iterator_get_next.
 * \return
 *  * Zero for success
 * \remark
 *   * This procedure assumes a previous procedure (dnx_l3_route_dbal_table_select())
 *     has checked validity:
 * \see
 *  * bcm_dnx_l3_route_add
 *  * bcm_dnx_l3_route_delete
 *  * bcm_dnx_l3_route_get
 *  * bcm_dnx_l3_route_traverse
 */
shr_error_e dnx_l3_route_kbp_payload_get(
    int unit,
    bcm_l3_route_t * info,
    uint8 is_kbp_split_rpf_in_use,
    uint32 dbal_table,
    uint32 entry_handle_id,
    uint8 is_traverse);

 /**
  * \brief
  *  Find an IP KBP route entry
  * \param [in] unit - The unit number.
  * \param [in,out] info - The l3 route structure describing the entry.\n
  * \return
  *   \retval Zero in case of no error.
  *   \retval Non-zero in case of an error
  * \remark
  *   receive information from LPM tables
  * \see
  *   * bcm_l3_route_t fields \n
  *   * BCM L3 Flags \n
  */
shr_error_e dnx_l3_kbp_route_get(
    int unit,
    bcm_l3_route_t * info);

 /**
  * \brief
  *  Adds/updates an IP KBP route entry.
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
  */
shr_error_e dnx_l3_kbp_route_add(
    int unit,
    bcm_l3_route_t * info);
/*
 * }
 */
#endif /* L3_FRWRD_TABLES_H_INCLUDED */
