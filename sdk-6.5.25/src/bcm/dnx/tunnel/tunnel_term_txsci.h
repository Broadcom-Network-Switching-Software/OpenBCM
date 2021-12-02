/*
 * ! \file bcm_int/dnx/l3/l3.h Internal DNX TT APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2021 Broadcom Inc. All rights reserved.
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
shr_error_e dnx_tunnel_term_txsci_create_verify(
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
shr_error_e dnx_tunnel_term_txsci_lookup_add(
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
shr_error_e dnx_tunnel_term_txsci_lookup_get(
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
shr_error_e dnx_tunnel_term_txsci_lookup_delete(
    int unit,
    bcm_tunnel_terminator_t * info);

/**
 * \brief
 * Get key fields for traverse function.
 *
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] entry_handle_id
 *   dbal handle
 *   \param [in] info -
 *     A pointer to the struct that holds information for the TXSCI tunnel terminator entry.
 *   \remark
 */
shr_error_e dnx_tunnel_term_txsci_traverse_key_fields(
    int unit,
    uint32 entry_handle_id,
    bcm_tunnel_terminator_t * info);

/**
 * \brief
 * Get txsci type for traverse function.
 *
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] global_inlif
 *   global inlif
 *   \param [in] in_lif_profile -
 *   in lif profile
 *   \param [in] type -
 *     A pointer to type
 *   \remark
 */
shr_error_e dnx_tunnel_term_txsci_traverse_type_get(
    int unit,
    uint32 global_inlif,
    uint32 in_lif_profile,
    bcm_tunnel_type_t * type);
