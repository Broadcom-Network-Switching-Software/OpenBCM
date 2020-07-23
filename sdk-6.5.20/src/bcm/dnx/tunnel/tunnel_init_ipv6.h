/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/** \file tunnel_init_ipv6.h
 * Tunnel init functionality for DNX tunnel encapsaultion
*/

#ifndef _TUNNEL_INIT_IPV6_INCLUDED__
/*
 * {
 */
#define _TUNNEL_INIT_IPV6_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif /* BCM_DNX_SUPPORT */

/*
 * Include files.
 * {
 */
#include <bcm/types.h>
#include <bcm/tunnel.h>
#include <shared/shrextend/shrextend_debug.h>

/*
 * }
 */

/**
 * \brief 
 * The function indicate if a tunnel is ipv6
 * \param [in] unit -
 *     The unit number.
 * \param [in] tunnel_type - tunnel type
 * \param [out] is_ipv6 - is ipv6 indication
*/
void dnx_tunnel_init_ipv6_is_ipv6_tunnel(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    uint32 *is_ipv6);

/**
 * \brief
 * Write to IP tunnel encapsulation out-LIF data entry table.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] tunnel - A pointer to the struct that holds information for the IPv4 tunnel encapsulation entry,
 *     see \ref bcm_tunnel_initiator_t.
 *   \param [in] local_outlif -
 *     Local-Out-LIF whose entry should be added to out-LIF table.
 *    \param [in] next_local_outlif - next local out lif for
 *           outlif entry
 *
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 *
 * \remark
 *   * The entry is written to DBAL_TABLE_EEDB_DATA_ENTRY
 */
shr_error_e dnx_tunnel_initiator_ipv6_data_entry_table_set(
    int unit,
    bcm_tunnel_initiator_t * tunnel,
    uint32 next_local_outlif,
    uint32 *local_outlif);

/**
 * \brief
 * read IP tunnel encapsulation out-LIF data entry table.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] tunnel - A pointer to the struct that holds information for the IPv4 tunnel encapsulation entry,
 *     see \ref bcm_tunnel_initiator_t.
 *   \param [in] local_outlif -
 *     Local-Out-LIF whose entry should be added to out-LIF table.
 *    \param [in] local_next_outlif - next local out lif for
 *           outlif entry
 *
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 *
 * \remark
 *   * The entry is written to DBAL_TABLE_EEDB_DATA_ENTRY
 */
shr_error_e dnx_tunnel_initiator_ipv6_data_entry_table_get(
    int unit,
    bcm_tunnel_initiator_t * tunnel,
    uint32 local_outlif,
    uint32 *local_next_outlif);

#endif  /*_TUNNEL_INIT_IPV6_INCLUDED__ */
