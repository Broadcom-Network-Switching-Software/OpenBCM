/** \file bcm_int/dnx/l3/l3_ecmp.h Internal DNX L3 APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef L3_ECMP_VIP_H_INCLUDED
/*
 * {
 */
#define L3_ECMP_VIP_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files.
 * {
 */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/l3.h>
/*
 * }
 */

/*
 * DEFINES
 * {
 */
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
 * }
 */
/*
 * Globals
 * {
 */

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
 *   Retrieve the minumum FEC-Id in the reserved VIP FEC range.
 * \param [in] unit - The unit number.
 * \param [out] fec_id_min - Minimum FEC-Id reserved for VIP FECs.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
dnx_l3_egress_ecmp_vip_fec_id_min_get(
    int unit,
    uint32 *fec_id_min);

/**
 * \brief
 *   Retrieve the maximum FEC-Id in the reserved VIP FEC range.
 * \param [in] unit - The unit number.
 * \param [out] fec_id_max - Maximum FEC-Id reserved for VIP FECs.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
dnx_l3_egress_ecmp_vip_fec_id_max_get(
    int unit,
    uint32 *fec_id_max);

/**
 * \brief
 *   Initialize the VIP SW DB and PEMLA virtual registers. Bypass for warmboot.
 * \param [in] unit - The unit number.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
dnx_l3_egress_ecmp_vip_init(int unit);

/**
 * \brief
 *   Create or update the VIP ECMP group.
 * \param [in] unit - The unit number.
 * \param [in] ecmp - The ECMP group information.
 * \param [in] intf_count - The number of ECMP members in the group.
 *                          For VIP ECMP, it must match the size configure with bcmSwitchVIPEcmpTableSize.
 * \param [in] intf_array - Array of the ECMP members.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
dnx_l3_egress_ecmp_vip_create(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp,
    int intf_count,
    bcm_if_t * intf_array);

/**
 * \brief
 *   Walk through all configured ECMP VIP groups and run the callback function.
 * \param [in] unit - The unit number.
 * \param [in] trav_fn - a pointer to the callback function.
 * \param [in] user_data  - opaque data passed to the callback function.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
dnx_l3_egress_ecmp_vip_traverse(
    int unit,
    bcm_l3_egress_ecmp_traverse_cb trav_fn,
    void *user_data);

/**
 * \brief
 *   Delete the VIP ECMP group.
 * \param [in] unit - The unit number.
 * \param [in] ecmp - The ECMP group information.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
dnx_l3_egress_ecmp_vip_destroy(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp);

/**
 * \brief
 *   Retrieve the member array of a VIP ECP group
 * \param [in] unit - The unit number.
 * \param [in] ecmp - The ECMP group information.
 * \param [in] intf_size  - The maximum size of the ECMP VIP group.
 *                          For VIP ECMP, it must match the size configure with bcmSwitchVIPEcmpTableSize.
 * \param [out] intf_array - Array of the ECMP members.
 * \param [out] intf_count - The number of members returned in intf_array.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
dnx_l3_egress_ecmp_vip_get(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp,
    int intf_size,
    bcm_if_t * intf_array,
    int *intf_count);

/**
 * \brief
 *  Adds and entry to the resilient table
 * \param [in] unit -
 *   The unit number.
 * \param [in] flags -
 *   supported flags:
 *   - BCM_L3_ECMP_RESILIENT_PCC: Add and entry to the internal PCC State table in LEM.
 *   - BCM_L3_ECMP_RESILIENT_PCC_EXTERNAL: Add and entry to the external PCC State table in KBP.
 * \param [in] entry -
 *   The entry data. The relevant fields
 *   - entry.ecmp.ecmp_intf -> VIP ECMP FEC ID
 *   - entry.hash_key       -> user flow signature (ECMP-LB-Key[2](16), NWK-LB-Key}
 *   - entry.intf           -> destination FEC ID
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
dnx_l3_egress_ecmp_pcc_resilient_add(
    int unit,
    uint32 flags,
    bcm_l3_egress_ecmp_resilient_entry_t *entry);

/**
 * \brief
 *  Deletes an entry from the resilient table
 * \param [in] unit -
 *   The unit number.
 * \param [in] flags -
 *   supported flags:
 *   - BCM_L3_ECMP_RESILIENT_PCC: Add and entry to the internal PCC State table in LEM.
 *   - BCM_L3_ECMP_RESILIENT_PCC_EXTERNAL: Add and entry to the external PCC State table in KBP.
 * \param [in] entry -
 *   The entry data. The relevant fields
 *   - entry.ecmp.ecmp_intf -> VIP ECMP FEC ID
 *   - entry.hash_key       -> user flow signature (ECMP-LB-Key[2](16), NWK-LB-Key}
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
dnx_l3_egress_ecmp_pcc_resilient_delete(
    int unit,
    uint32 flags,
    bcm_l3_egress_ecmp_resilient_entry_t *entry);

/**
 * \brief
 *  Replaces an entry in the resilient table
 * \param [in] unit -
 *   The unit number.
 * \param [in] flags -
 *   supported flags:
 *   - BCM_L3_ECMP_RESILIENT_PCC: Add and entry to the internal PCC State table in LEM.
 *   - BCM_L3_ECMP_RESILIENT_PCC_EXTERNAL: Add and entry to the external PCC State table in KBP.
 *   - BCM_L3_ECMP_RESILIENT_MATCH_HASH_KEY
 *   - BCM_L3_ECMP_RESILIENT_MATCH_ECMP
 *   - BCM_L3_ECMP_RESILIENT_MATCH_INTF
 *   - BCM_L3_ECMP_RESILIENT_REPLACE
 *   - BCM_L3_ECMP_RESILIENT_DELETE
 * \param [in] match_entry -
 *   The match_entry data. The relevant fields acccording to flags
 *   - entry.ecmp.ecmp_intf -> VIP ECMP FEC ID
 *   - entry.hash_key       -> user flow signature (ECMP-LB-Key[2](16), NWK-LB-Key}
 *   - entry.intf           -> destination FEC ID
 * \param [out] num_entries - the number of entries updated or deleted
 * \param [in] replace_entry
 *    The replace entry data
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
dnx_l3_egress_ecmp_pcc_resilient_replace(
    int unit,
    uint32 flags,
    bcm_l3_egress_ecmp_resilient_entry_t *match_entry,
    int *num_entries,
    bcm_l3_egress_ecmp_resilient_entry_t *replace_entry);

/**
 * \brief
 *  Traverses the resilient table, calling the specified callback function for each matching entry.
 * \param [in] unit -
 *   The unit number.
 * \param [in] flags -
 *   supported flags:
 *   - BCM_L3_ECMP_RESILIENT_PCC: Add and entry to the internal PCC State table in LEM.
 *   - BCM_L3_ECMP_RESILIENT_PCC_EXTERNAL: Add and entry to the external PCC State table in KBP.
 *   - BCM_L3_ECMP_RESILIENT_MATCH_HASH_KEY
 *   - BCM_L3_ECMP_RESILIENT_MATCH_INTF
 *   - BCM_L3_ECMP_RESILIENT_MATCH_ECMP
 * \param [in] match_entry -
 *   The entry data. The relevant fields
 *   - entry.ecmp.ecmp_intf -> VIP ECMP FEC ID
 *   - entry.hash_key       -> user flow signature (ECMP-LB-Key[2](16), NWK-LB-Key}
 *   - entry.intf           -> destination FEC ID
 * \param [in] trav_fn - callback function
 * \param [in] user_data - Opaque user data to be passed back to the callback
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
dnx_l3_egress_ecmp_pcc_resilient_traverse(
    int unit,
    uint32 flags,
    bcm_l3_egress_ecmp_resilient_entry_t *match_entry,
    bcm_l3_egress_ecmp_resilient_traverse_cb trav_fn,
    void *user_data);

/**
 * \brief - Configure the SLLB default destination
 *
 * \param [in] unit - Relevant unit.
 * \param [in] arg - gport of default destination
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e
dnx_switch_control_sllb_default_destination_set(
    int unit,
    int arg);

/**
 * \brief - Get the SLLB default destination
 *
 * \param [in] unit - Relevant unit.
 * \param [out] arg - gport of default destination
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *   None
 * \see
 *   * None
 */
shr_error_e
dnx_switch_control_sllb_default_destination_get(
    int unit,
    int * arg);

/**
 * \brief - Configure the SLLB VIP ECMP table size
 *
 * \param [in] unit - Relevant unit.
 * \param [in] arg - table size.
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *   SLLB Table Size may be set only when no VIP ECMP FECs have been allocated.
 * \see
 *   * None
 */
shr_error_e
dnx_switch_control_sllb_table_size_set(
    int unit,
    int arg);

/**
 * \brief - Get the SLLB VIP ECMP table size
 *
 * \param [in] unit - Relevant unit.
 * \param [out] arg - table size.
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *   None
 * \see
 *   * None
 */
shr_error_e
dnx_switch_control_sllb_table_size_get(
    int unit,
    int * arg);

/**
 * \brief - Configure the VIP ECMP minimum FEC-ID
 *
 * \param [in] unit - Relevant unit.
 * \param [in] arg - minimum FEC-ID in the range reserved for VIP ECPM FECs.
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e
dnx_switch_control_vip_l3_egress_id_min_set(
    int unit,
    int arg);

/*
 * }
 */
#endif /* L3_ECMP_VIP_H_INCLUDED */
