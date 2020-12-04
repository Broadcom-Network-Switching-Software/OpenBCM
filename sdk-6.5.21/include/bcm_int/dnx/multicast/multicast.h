/**
 * \file bcm_int/dnx/multicast/multicast.h Internal DNX VLAN APIs
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 */

#ifndef _DNX_MULTICAST_API_INCLUDED__
/*
 * { 
 */
#define _DNX_MULTICAST_API_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/multicast.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/multicast_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_multicast.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>

#define DNX_MULTICAST_ALGO_RES_MCDB_STR                     "MCDB"
#define DNX_MULTICAST_ALGO_RES_IS_TDM_STR                   "MULTICAST_GROUP_IS_TDM"

#define DNX_MULTICAST_ING_INVALID_DESTINATION(unit)     ((1 << 21) - 1)
#define DNX_MULTICAST_INVALID_CUD                       dnx_data_lif.global_lif.egress_out_lif_null_value_get(unit)

#define DNX_MULTICAST_IS_BIER(flags)  (((flags & BCM_MULTICAST_BIER_64_GROUP) || \
                                        (flags & BCM_MULTICAST_BIER_128_GROUP) || \
                                        (flags & BCM_MULTICAST_BIER_256_GROUP)) ? 1 : 0)
#define DNX_MULTICAST_IS_INGRESS(flags) ((flags & BCM_MULTICAST_INGRESS_GROUP) ? 1 : 0)
#define DNX_MULTICAST_IS_EGRESS(flags) (((flags & BCM_MULTICAST_EGRESS_GROUP) || (flags & BCM_MULTICAST_EGRESS_TDM_GROUP)) ? 1 : 0)

/**
 * \brief - Initialize dnx multicast module
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 *   
 * \par INDIRECT INPUT:
 *   * DNX data related multicast module information
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_multicast_init(
    int unit);

/**
 * \brief - Deinitialize dnx multicast module
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_multicast_deinit(
    int unit);

/**
 * \brief
 * obtain gport from multicast destination
 *
 * \param [in] unit - Unit ID
 * \param [in] core_id - core ID
 * \param [in] is_egress - is destination egress
 * \param [in] destination - dbal representation of multicast destination
 * \param [out] gport - gport represensation of the destination
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_multicast_gport_from_encoded_destination_field(
    int unit,
    bcm_core_t core_id,
    int is_egress,
    uint32 destination,
    bcm_gport_t * gport);

shr_error_e dnx_multicast_utils_egr_invalid_destination_get(
    int unit,
    uint32 *egr_invalid_destination);

shr_error_e dnx_multicast_utils_ext_mode_get(
    int unit,
    int *ext_mode);

/*
 * MACROs
 * {
 */

/*
 * }
 */

/*
 * } 
 */
#endif/*_DNX_MULTICAST_API_INCLUDED__*/
