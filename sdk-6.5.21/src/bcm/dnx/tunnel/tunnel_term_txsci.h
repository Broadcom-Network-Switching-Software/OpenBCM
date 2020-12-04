/*
 * ! \file bcm_int/dnx/l3/l3.h Internal DNX TT APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * Include files.
 * {
 */
#include <bcm/types.h>
#include <bcm/tunnel.h>
#include <bcm/error.h>
/*
 * }
 */

/*
 * DEFINEs
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
 * Global and Static
 */

/*
 * }
 */
/**
 * \brief
 * Part of the TT create verify, handle the TXSCI lookup verification.
 *
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] info -
 *     A pointer to the struct that holds information for the TXSCI tunnel terminator entry.
 *   \remark
 */
shr_error_e dnx_tunnel_terminator_create_txsci_verify(
    int unit,
    bcm_tunnel_terminator_t * info);

/**
 * \brief
 * Write tunnel termination VTT lookup to HW ISEM using TXSCI.
 *
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] info -
 *     A pointer to the struct that holds information for the TXSCI tunnel terminator entry.
 *   \param [in] local_inlif -
 *     Allocated Local-In-LIF.
 *   \remark
 */
shr_error_e dnx_tunnel_terminator_lookup_add_txsci(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 local_inlif);

/**
 * \brief
 * Get tunnel termination VTT lookup from HW ISEM.
 *
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] info -
 *     A pointer to the struct that holds information for the TXSCI tunnel terminator entry.
 *   \param [out] local_inlif -
 *     Allocated Local-in-LIF.
 *   \remark
 */
shr_error_e dnx_tunnel_terminator_lookup_get_txsci(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 *local_inlif);

/**
 * \brief
 * Delete tunnel termination VTT lookup from HW ISEM.
 *
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] info -
 *     A pointer to the struct that holds information for the TXSCI tunnel terminator entry.
 *   \remark
 */
shr_error_e dnx_tunnel_terminator_lookup_delete_txsci(
    int unit,
    bcm_tunnel_terminator_t * info);
