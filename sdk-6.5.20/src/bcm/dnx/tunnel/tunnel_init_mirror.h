/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/** \file tunnel_init_mirror.h
 * Tunnel init functionality for DNX tunnel encapsaultion
*/

#ifndef _TUNNEL_INIT_ERSPAN_INCLUDED__
/*
 * {
 */
#define _TUNNEL_INIT_ERSPAN_INCLUDED__

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
 * Write to ERSPAN encapsulation out-LIF table.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] tunnel - A pointer to the struct that holds information for ERSPAN encapsulation entry,
 *     see \ref bcm_tunnel_initiator_t.
 *   \param [in] dbal_table_id - dbal table id
 *   \param [in] global_outlif - global outlif
 *   \param [in,out] local_outlif -
 *     Local-Out-LIF whose entry should be added to out-LIF table.
 *    \param [in] next_local_outlif - next local out lif for
 *           outlif entry
 *
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 *
 * \remark
 *   * The entry is written to DBAL_TABLE_EEDB_ERSPAN.
 */
shr_error_e dnx_tunnel_initiator_allocate_local_lif_and_erspan_table_set(
    int unit,
    bcm_tunnel_initiator_t * tunnel,
    dbal_tables_e dbal_table_id,
    uint32 global_outlif,
    uint32 next_local_outlif,
    uint32 *local_outlif);

/**
 * \brief
 * Write to RSPAN Advanced encapsulation out-LIF table.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] tunnel - A pointer to the struct that holds information for RSPAN Advanced encapsulation entry,
 *     see \ref bcm_tunnel_initiator_t.
 *   \param [in] dbal_table_id - dbal table id 
 *   \param [in,out] local_outlif -
 *     Local-Out-LIF whose entry should be added to out-LIF table.
 *   \param [in] global_outlif - global outlif
 *
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 *
 * \remark
 *   * The entry is written to DBAL_TABLE_EEDB_RSPAN.
 */
shr_error_e dnx_tunnel_initiator_allocate_local_lif_an_rspan_advanced_table_set(
    int unit,
    bcm_tunnel_initiator_t * tunnel,
    dbal_tables_e dbal_table_id,
    uint32 global_outlif,
    uint32 *local_outlif);

/**
 * \brief
 * Indicate if the tunnel is erspan or rspan
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] tunnel_type - tunnel type
 *   \param [out] is_erspan_or_rpsan - returned value to
 *          indicate if the tunnel is erspan or rspan
 */
void dnx_tunnel_initiator_is_mirror(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    uint32 *is_erspan_or_rpsan);

/**
 * \brief
 * read from ERSPAN encapsulation out-LIF table.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] dbal_table_id 
 *   \param [in] local_out_lif -
 *     Local-Out-LIF whose entry should be added to out-LIF table.
 *   \param [out] next_local_outlif - next local out lif for
 *           outlif entry
 *   \param [out] tunnel - A pointer to the struct that holds
 *     information for ERSPAN tunnel encapsulation entry, see
 *     \ref bcm_tunnel_initiator_t.
 *
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 *
 * \remark
 *   * The entry is read from DBAL_TABLE_EEDB_ERSPAN.
 */
shr_error_e dnx_tunnel_initiator_erspan_table_get(
    int unit,
    dbal_tables_e dbal_table_id,
    uint32 local_out_lif,
    uint32 *next_local_outlif,
    bcm_tunnel_initiator_t * tunnel);

/**
 * \brief
 * read from RSPAN Advanced encapsulation out-LIF table.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] local_outlif -
 *     Local-Out-LIF whose entry should be added to out-LIF table.
 *   \param [out] tunnel - A pointer to the struct that holds
 *     information for RSPAN Advanced tunnel encapsulation entry, see
 *     \ref bcm_tunnel_initiator_t.
 *
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 *
 * \remark
 *   * The entry is read from DBAL_TABLE_EEDB_RSPAN.
 */
shr_error_e dnx_tunnel_initiator_rspan_advanced_table_get(
    int unit,
    uint32 local_outlif,
    bcm_tunnel_initiator_t * tunnel);

/**
 * \brief
 * Write to RSPAN Advanced encapsulation out-LIF table.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] tunnel - A pointer to the struct that holds information for RSPAN Advanced encapsulation entry,
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
shr_error_e dnx_tunnel_initiator_mirror_tunnel_initiator_create(
    int unit,
    bcm_tunnel_initiator_t * tunnel,
    bcm_gport_t tunnel_gport,
    int global_outlif,
    uint32 arp_out_lif);

/**
 * \brief
 * clear ERSPAN tunnel initiator.
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
 *   * Clears ERSPAN entry in DBAL_TABLE_EEDB_ERSPAN.
 */
shr_error_e dnx_tunnel_initiator_erspan_clear(
    int unit,
    int global_outlif,
    int local_outlif);

/**
 * \brief
 * Write to RSPAN Advanced encapsulation out-LIF table.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] global_outlif - global outlif
 *   \param [in] local_outlif -
 *     local outlif.
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 *
 * \remark
 *   * Clears RSPAN Advanced entry in DBAL_TABLE_EEDB_RSPAN.
 */
shr_error_e dnx_tunnel_initiator_rspan_advanced_clear(
    int unit,
    int global_outlif,
    int local_outlif);

#endif  /*_TUNNEL_INIT_ERSPAN_INCLUDED__ */
