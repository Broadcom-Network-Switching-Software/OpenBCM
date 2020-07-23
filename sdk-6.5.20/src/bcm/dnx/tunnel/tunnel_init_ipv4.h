/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/** \file tunnel_init.h
 * Tunnel init functionality for DNX tunnel encapsaultion
*/

#ifndef _TUNNEL_INIT_IPV4_INCLUDED__
/*
 * {
 */
#define _TUNNEL_INIT_IPV4_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif /* BCM_DNX_SUPPORT */

/*
 * Include files.
 * {
 */
#include <bcm/types.h>
#include <bcm/tunnel.h>

/*
 * }
 */

/**
 * \brief
 * Indicate weather tunnel type is ipv4 
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] tunnel_type - tunnel type
 *   \param [out] is_ipv4 - returned value to
 *          indicate if the tunnel is ipv4 
 */
void dnx_tunnel_init_ipv4_is_ipv4_tunnel(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    uint32 *is_ipv4);

#endif  /*_TUNNEL_INIT_IPV4_INCLUDED__ */
