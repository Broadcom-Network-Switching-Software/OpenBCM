/*
 * ! \file bcm_int/dnx/multicast/multicast.h Internal DNX VLAN APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2022 Broadcom Inc. All rights reserved. 
 */

#ifndef _DNX_BIER_API_INCLUDED__
/*
 * { 
 */
#define _DNX_BIER_API_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#include <bcm/multicast.h>

/*
 * MACROs
 * {
 */

/*
 * }
 */

/**
 * \brief - verify that multicast group at the correct range used by bcm_multicast_create API
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] flags - specifies whether group id is ingress/egress
 *   \param [in] group - multicast group id specifies the set
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 * \see
 *   * None
 */
shr_error_e dnx_multicast_bfr_id_verify(
    int unit,
    uint32 flags,
    bcm_multicast_t group);

/**
 * \brief - creates multicast BIER group.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] flags - specifies whether group id is ingress/egress
 *   \param [in] group_id - multicast group id specifies the set
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *
 * \see
 *   * None
 */
shr_error_e dnx_multicast_bier_create(
    int unit,
    uint32 flags,
    bcm_multicast_t group_id);

/**
 * \brief - destroys multicast group for BIER.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] flags - specifies whether group id is ingress/egress
 *   \param [in] group_id - multicast group id specifies the set
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *
 * \see
 *   * None
 */
shr_error_e dnx_multicast_bier_destroy(
    int unit,
    uint32 flags,
    bcm_multicast_t group_id);

/**
 * \brief - init BIER module with BFR parameters.
 *
 * \param [in] unit -  Unit-ID
 *
 * \remark
 *
 * \see
 *   * None
 */
shr_error_e dnx_bier_init(
    int unit);

/**
 * \brief - init BIER module PP function.
 *
 * \param [in] unit -  Unit-ID
 *
 * \remark
 *
 * \see
 *   * None
 */
shr_error_e dnx_bier_pp_init(
    int unit);

int dnx_multicast_bfr_id_add(
    int unit,
    uint32 flags,
    bcm_multicast_t group,
    uint32 bfr_id,
    bcm_multicast_replication_t replication);

int dnx_multicast_bfr_id_delete(
    int unit,
    uint32 flags,
    bcm_multicast_t group,
    uint32 bfr_id,
    bcm_multicast_replication_t replication);

int dnx_multicast_bfr_id_get(
    int unit,
    uint32 flags,
    bcm_multicast_t group,
    uint32 bfr_id,
    bcm_multicast_replication_t * replication);

/*
 * } 
 */
#endif/*_DNX_BIER_API_INCLUDED__*/
