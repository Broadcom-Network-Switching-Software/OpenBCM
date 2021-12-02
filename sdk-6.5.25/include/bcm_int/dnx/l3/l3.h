/*
 * ! \file bcm_int/dnx/l3/l3.h Internal DNX L3 APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifndef L3_H_INCLUDED
/*
 * {
 */
#define L3_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * DEFINES
 * {
 */

#define L3_MAC_ADDR_SIZE_IN_BYTES                       (6)

#define L3_IPV4_PROTOCOL_VERSION                        (4)
#define L3_IPV6_PROTOCOL_VERSION                        (6)
#define L3_MAX_NOF_ROUTING_TABLES_PER_PROTOCOL_VERSION  (4)

#define L3_MDB_PHYSICAL_DB_LPM_IF_0                     (0)
#define L3_MDB_PHYSICAL_DB_LPM_IF_1                     (1)

/** bits 1-6 are relevant so masking all the rest and shifting to right */
#define L3_INTF_MY_MAC_ETH_PREFIX(mymac) ((mymac >> 1) & 0x3F)

/* First non default RIF ID*/
#define L3_INTF_FIRST_NON_DEFAULT_RIF_ID               1

/*
 * }
 */

/*
 * ENUMS
 * {
 */
/**
 * A list of KAPS LPM strength profiles
 */
typedef enum dnx_l3_kaps_lpm_strength_profiles_e
{
    DNX_L3_KAPS_LPM_STRENGTH_PROFILE_DEFAULT = 0,
    DNX_L3_KAPS_LPM_STRENGTH_PROFILE_UC_IPV4 = 1,
    DNX_L3_KAPS_LPM_STRENGTH_PROFILE_UC_IPV6 = 2,
    DNX_L3_KAPS_LPM_STRENGTH_PROFILE_MC_IPV4 = 3,
    DNX_L3_KAPS_LPM_STRENGTH_PROFILE_MC_IPV6 = 4,
    DNX_L3_KAPS_LPM_STRENGTH_PROFILE_RESERVED_1 = 5,
    DNX_L3_KAPS_LPM_STRENGTH_PROFILE_RESERVED_2 = 6,
    DNX_L3_KAPS_LPM_STRENGTH_PROFILE_RESERVED_3 = 7
} dnx_l3_kaps_lpm_strength_profiles_e;
/*
 * }
 */

/*
 * TYPEDEFS
 * {
 */

/**
 * \brief - Initialize L3 module. \n
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 * \par INDIRECT INPUT:
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e dnx_l3_module_init(
    int unit);

/**
 * \brief
 *   Allow/Forbid the Strict UC RPF packets or the SIP-based MC
 *   RPF packets to be forwarded by default route entry.
 * \param [in] unit - The unit number.
 * \param [in] is_uc_switch -
 *      * 0 - the MC RPF switch will be modified.
 *      * 1 - the UC RPF switch will be modified.
 * \param [in] value -
 *      * 0 - forbid default route of strict UC RPF packets
 *      * 1 - allow default route of strict UC RPF packets
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcmSwitchL3UrpfDefaultRoute
 *  * bcmSwitchL3McRpfDefaultRoute
 */
shr_error_e dnx_l3_rpf_default_route_set(
    int unit,
    int is_uc_switch,
    int value);

/**
 * \brief
 *   Receive the setting of whether the Strict UC RPF packets
 *   or the SIP-based MC RPF packets are going to be forwarded
 *   by default route entry.
 * \param [in] unit - The unit number.
 * \param [in] is_uc_switch -
 *      * 0 - the MC RPF switch will be received.
 *      * 1 - the UC RPF switch will be received.
 * \param [out] value -
 *      * 0 - default route of RPF packets is forbidden.
 *      * 1 - default route of RPF packets is allowed.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcmSwitchL3UrpfDefaultRoute
 *  * bcmSwitchL3McRpfDefaultRoute
 */
shr_error_e dnx_l3_rpf_default_route_get(
    int unit,
    int is_uc_switch,
    int *value);

/**
* \brief
*  Exchange mac address prefixes in SW.
*   \param [in] unit     - The Unit number.
*   \param [in] src_mac_addr         -  Requested mac address to be checked.
*   \param [in] old_mymac_prefix         -  mac address prefix to be exchanged
*   \param [out] new_mymac_prefix         -  Valid pointer to mac address prefix provided by the algorithm.
*   \param [out] is_first_mymac_reference         -  if set 'new_mymac_prefix' is new new entry - need to allocate in HW as well
*   \param [out] is_last_mymac_reference         -  if set 'old_mymac_prefix' not used any more and freed in SW, free in HW as well
* \return
*   shr_error_e
*/
shr_error_e dnx_l3_intf_egress_mymac_prefix_exchange(
    int unit,
    bcm_mac_t src_mac_addr,
    int old_mymac_prefix,
    int *new_mymac_prefix,
    uint8 *is_first_mymac_reference,
    uint8 *is_last_mymac_reference);

/**
 * \brief 
 *   Write the source address MSBs to soruce_address table given the mymac index
 * \param [in] unit - 
 *   The unit number.
 * \param [in] is_arp -
 *   Whether it is mymac that is required for ARP entry.
 * \param [in] l3a_mac_addr - 
 *   Requested MSB mac address to be written. (not shifted)
 * \param [in] mymac_prefix_profile - 
 *  MAC prefix to be saved to Index to the table (retrieve from the source_address template manager)
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l3_intf_egress_mymac_set(
    int unit,
    uint8 is_arp,
    bcm_mac_t l3a_mac_addr,
    int mymac_prefix_profile);

/**
 * \brief 
 *   Clear the source address MSBs to soruce_address table given the mymac index
 * \param [in] unit - 
 *      The unit number.
 * \param [in] is_arp -
 *      Whether it is mymac that is required for ARP entry.
 * \param [in] mymac_prefix_profile - 
 *      MAC prefix to be saved to Index to the table (retrieve from the source_address template manager)
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l3_intf_egress_mymac_clear(
    int unit,
    uint8 is_arp,
    int mymac_prefix_profile);

/**
 * \brief
 *   Checks if intf was created
 * \param [in] unit -
 *      The unit number.
 * \param [in] intf_id -
 *      Interface ID to check.
 * \param [out] intf_exists -
 *      * 0 - the interface exists.
 *      * 1 - the interface does not exists.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   Checks VSI table in the corresponding entry,
 *   and validates that the my_mac prefix is valid by making sure the prefix profile
 *   doesn't point to the default invalid profile (in index 0 in prefix template manager).
 * \see
 *   * None
 */
shr_error_e dnx_l3_check_if_ingress_intf_exists(
    int unit,
    bcm_if_t intf_id,
    uint8 *intf_exists);
/*
 * }
 */
#endif /* L3_H_INCLUDED */
