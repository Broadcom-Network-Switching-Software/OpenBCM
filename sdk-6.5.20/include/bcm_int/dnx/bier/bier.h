/*
 * ! \file bcm_int/dnx/multicast/multicast.h Internal DNX VLAN APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 */

#ifndef _DNX_BIER_API_INCLUDED__
/*
 * { 
 */
#define _DNX_BIER_API_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/multicast.h>

/*
 * MACROs
 * {
 */

/*
 * }
 */


typedef struct bcm_multicast_bier_config_e
{
    /*
     * number of BFR entries in the system
     */
    uint32 nof_bfr_entries;
} bcm_multicast_bier_config_t;

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
    bcm_multicast_t group_id);

/**
 * \brief - returns the number of MCDB entries occupied by BIER.
 *
 * \param [in] unit -  Unit-ID
 *
 * \remark
 *
 * \see
 *   * None
 */
int dnx_bier_mcdb_size_get(
    int unit);

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

/*
 * } 
 */
#endif/*_DNX_BIER_API_INCLUDED__*/
