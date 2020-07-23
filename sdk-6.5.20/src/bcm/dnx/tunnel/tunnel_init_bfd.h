/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/** \file tunnel_init_bfd.h
 * Tunnel init functionality for DNX tunnel encapsulation
*/

#ifndef _TUNNEL_INIT_BFD_INCLUDED__
/*
 * {
 */
#define _TUNNEL_INIT_BFD_INCLUDED__

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
 * Indicate whether tunnel type is for BFD SIP
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] tunnel_type - tunnel type
 *   \param [in] sip - Source IP
 *   \param [in] dip - Destination IP
 *   \param [in] arp_out_lif - arp entry
 *   \param [out] is_bfd_extra_sip - returned value to
 *          indicate if the tunnel is for BFD
 */
void dnx_tunnel_initiator_is_bfd_extra_sip(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    bcm_ip_t dip,
    bcm_ip_t sip,
    bcm_if_t arp_out_lif,
    uint32 *is_bfd_extra_sip);

/**
 * \brief
 * read from SIP BFD tunnel out-LIF table.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] dbal_table_id
 *   \param [in] local_out_lif -
 *     Local-Out-LIF whose entry should be added to out-LIF table.
 *   \param [out] next_local_outlif - next local out lif for
 *           outlif entry
 *   \param [out] tunnel - A pointer to the struct that holds
 *                         BFD EXTRA SIP
 *     \ref bcm_tunnel_initiator_t.
 *
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 *
 * \remark
 *   * The entry is read from DBAL_TABLE_IPV$_TUNNEL.
 */
shr_error_e dnx_tunnel_initiator_bfd_extra_sip_table_get(
    int unit,
    dbal_tables_e dbal_table_id,
    uint32 local_out_lif,
    uint32 *next_local_outlif,
    bcm_tunnel_initiator_t * tunnel);

/**
 * \brief
 * clear BFD SIP tunnel initiator.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] global_outlif - global outlif
 *   \param [in] local_outlif -
 *     local outlif.
 *
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 *
 * \remark
 *   * Clears BFD extra SIP entry.
 */
shr_error_e dnx_tunnel_initiator_bfd_extra_sip_clear(
    int unit,
    int global_outlif,
    int local_outlif);
/**
 * \brief
 * Write to BFD SIP tunnel out-LIF table.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] tunnel - A pointer to the struct that holds information BFD SIP entry,
 *     see \ref bcm_tunnel_initiator_t.
 *   \param [out] global_outlif -
 *     global-Out-LIF
 *   \param [in] tunnel_gport - tunnel gport
 *   \param [in] arp_out_lif -
 *   local arp out lif
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 *
 * \remark
 *   * The entry is written to DBAL_TABLE_EEDB_RSPAN.
 */
shr_error_e dnx_tunnel_bfd_initiator_create(
    int unit,
    bcm_tunnel_initiator_t * tunnel,
    bcm_gport_t tunnel_gport,
    int global_outlif,
    uint32 arp_out_lif);
#endif  /*_TUNNEL_INIT_BFD_INCLUDED__ */
