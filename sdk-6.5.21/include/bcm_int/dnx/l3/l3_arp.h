/** \file bcm_int/dnx/l3/l3_arp.h
 * 
 * Internal DNX L3 ARP APIs 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _L3_ARP_API_INCLUDED__
/*
 * { 
 */
#define _L3_ARP_API_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/l3.h>

/*
 * MACROs
 * {
 */
/*
 * }
 */

/*
 * FUNCTIONS
 * {
 */

/**
* \brief
*   Creation function for bcm_l3_egress_create with BCM_L3_EGRESS_ONLY flag.
*
*    \param [in] unit - Relevant unit.
*    \param [in] flags - Similar to bcm_l3_egress_create api input
*    \param [in] egr - Similar to bcm_l3_egress_create api input
* \return
*    shr_error_e - 
*      Error return value
*/
shr_error_e dnx_l3_egress_create_arp(
    int unit,
    uint32 flags,
    bcm_l3_egress_t * egr);

/**
* \brief
* Retrieve a L3 egress object egress side information 
*
*   \param [in] unit       -  Relevant unit.
*   \param [in] intf  -    egress object ID .
*   \param [in] egr  -    structure holding the egress object .
*        [out]   egr.mac_addr - Next hop forwarding destination mac Next hop vlan id
*        [out]   egr.vlan - Next hop vlan id Encapsulation index
*        [out]   egr.encap_id - Encapsulation index
*        [out]   egr.port - Outgoing port id (if !BCM_L3_TGID)
*        [out]   egr.trunk - Outgoing Trunk id (if BCM_L3_TGID) General QOS map id
*        [out]   egr.qos_map_id - General QOS map id 
*        [out]   egr.failover_id - Failover Object Index Failover Egress Object index
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
shr_error_e dnx_l3_egress_arp_info_get(
    int unit,
    bcm_if_t intf,
    bcm_l3_egress_t * egr);

/**
* \brief
* Delete L3 egress object egress side information 
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
shr_error_e dnx_l3_egress_arp_info_delete(
    int unit,
    bcm_if_t intf);

/*
 * }
 */

/*
 * } 
 */
#endif/*_L3_ARP_API_INCLUDED__*/
