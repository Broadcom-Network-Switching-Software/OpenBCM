/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/** \file tunnel_init_li.h
 * Tunnel init functionality for DNX tunnel encapsulation
*/

#ifndef _TUNNEL_INIT_LI_INCLUDED__
/*
 * {
 */
#define _TUNNEL_INIT_LI_INCLUDED__

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
 * Indicate weather tunnel type is for LI
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] tunnel_type - tunnel type
 *   \param [out] is_li - returned value to
 *          indicate if the tunnel is for LI
 */
void dnx_tunnel_initiator_is_li(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    uint32 *is_li);

/**
 * \brief
 * read from LI tunnel out-LIF table.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] dbal_table_id
 *   \param [in] local_out_lif -
 *     Local-Out-LIF whose entry should be added to out-LIF table.
 *   \param [out] next_local_outlif - next local out lif for
 *           outlif entry
 *   \param [out] tunnel - A pointer to the struct that holds LI tunnel
 *     \ref bcm_tunnel_initiator_t.
 *
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 *
 * \remark
 *   * The entry is read from DBAL_TABLE_EEDB_DATA_ENTRY.
 */
shr_error_e dnx_tunnel_initiator_li_table_get(
    int unit,
    dbal_tables_e dbal_table_id,
    uint32 local_out_lif,
    uint32 *next_local_outlif,
    bcm_tunnel_initiator_t * tunnel);

/**
 * \brief
 * clear LI tunnel initiator.
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
 *   * Clears LI entry.
 */
shr_error_e dnx_tunnel_initiator_li_clear(
    int unit,
    int global_outlif,
    int local_outlif);

/**
 * \brief
 * Write to LI tunnel out-LIF table.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] tunnel - A pointer to the struct that holds information LI entry,
 *     see \ref bcm_tunnel_initiator_t.
 *   \param [out] global_outlif - global-Out-LIF
 *   \param [in] tunnel_gport - tunnel gport
 *   \param [in] next_local_outlif - next out lif 
 *
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 *
 * \remark
 *   * The entry is written to DBAL_TABLE_EEDB_DATA_ENTRY.
 */
shr_error_e dnx_tunnel_initiator_li_initiator_create(
    int unit,
    bcm_tunnel_initiator_t * tunnel,
    bcm_gport_t tunnel_gport,
    int global_outlif,
    uint32 next_local_outlif);

#endif  /*_TUNNEL_INIT_LI_INCLUDED__ */
