/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        vswitch.h
 * Purpose:     vswitch internal definitions to the BCM library.
 */

#ifndef   _BCM_INT_DNX_VSWITCH_H_
#define   _BCM_INT_DNX_VSWITCH_H_

#include <bcm/types.h>

/**
* \brief 
* VSWITCH gport can be AC (VLAN-Port) or MPLS-Port or Tunnel or EXTENDER-Port
*/
#define _SHR_GPORT_IS_VSWITCH(_gport) (_SHR_GPORT_IS_VLAN_PORT(_gport) || (_SHR_GPORT_IS_MPLS_PORT(_gport)) || (_SHR_GPORT_IS_TUNNEL(_gport)) || (_SHR_GPORT_IS_EXTENDER_PORT(_gport)))

/**
 * \brief - function returns whether a table handle ID is P2P
 * \remark
 * The table handle needs to be allocated and "get all fields" was performed before calling this function
 */
shr_error_e dnx_vswitch_entry_is_p2p(
    int unit,
    uint32 entry_handle_id,
    int *is_p2p);

/**
 * \brief - function returns whether a given VLAN-Port / MPLS-Port / Tunnel gport is P2P
 */
shr_error_e dnx_vswitch_is_p2p(
    int unit,
    bcm_gport_t gport,
    int *is_p2p);

#endif /* _BCM_INT_DNX_VSWITCH_H_ */
