/*
 * ! \file bcm_int/dnx/mirror/mirror_header.h Internal DNX MIRROR HEADER APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNX_MIRROR_HEADER_INCLUDED__
/*
 * {
 */
#define _DNX_MIRROR_HEADER_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/mirror.h>
#include <shared/shrextend/shrextend_error.h>
#include <soc/dnx/dbal/dbal.h>

/**
 * \brief - Allocate and set system header information into HW a mirror profile
 *
 * \param [in] unit - Unit ID
 * \param [in] flags - If input flags have BCM_MIRROR_DEST_IS_SNOOP, this means snif command is snoop.
 *                     If input flags have BCM_MIRROR_DEST_IS_STAT_SAMPLE, this means snif command is stat.
 *                     Other input flags mean the snif command is mirror.
 *                     If input flags with BCM_MIRROR_DEST_EGRESS_ADD_ORIG_SYSTEM_HEADER,
 *                     this means keep original system header function will be enabled.
 *                     If input flags with BCM_MIRROR_DEST_OUT_MIRROR_DISABLE, this means out mirror disable
 *                     function will be enabled.
 * \param [in] mirror_dest_id - Gport representing the command id. gport: BCM_GPORT_MIRROR
 * \param [in] mirror_header_info - Mirror system header information
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *   * None
 * \see
 *   * Reference to bcm_dnx_mirror_system_header_set
 */
shr_error_e dnx_mirror_header_system_header_set(
    int unit,
    uint32 flags,
    bcm_gport_t mirror_dest_id,
    bcm_mirror_header_info_t * mirror_header_info);

/**
 * \brief - Get system header information from HW a mirror profile
 *
 * \param [in] unit - Unit ID
 * \param [in] mirror_dest_id - Gport representing the command id. gport: BCM_GPORT_MIRROR
 * \param [in,out] flags - If input flags have BCM_MIRROR_DEST_IS_SNOOP, this means snif command is snoop.
 *                         If input flags have BCM_MIRROR_DEST_IS_STAT_SAMPLE, this means sinif command is stat.
 *                         Other input flags mean the sinif command is mirror.
 *                         If out flags with BCM_MIRROR_DEST_EGRESS_ADD_ORIG_SYSTEM_HEADER,
 *                         this means keep original system header function is enabled.
 *                         If out flags with BCM_MIRROR_DEST_OUT_MIRROR_DISABLE, this means out mirror disable
 *                         function is enabled.
 * \param [out] mirror_header_info - Mirror system header information
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *  * None
 * \see
 *   * Reference to bcm_dnx_mirror_system_header_get
 */
shr_error_e dnx_mirror_header_system_header_get(
    int unit,
    bcm_gport_t mirror_dest_id,
    uint32 *flags,
    bcm_mirror_header_info_t * mirror_header_info);

/*
 * }
 */
#endif/*_DNX_MIRROR_HEADER_INCLUDED__*/
