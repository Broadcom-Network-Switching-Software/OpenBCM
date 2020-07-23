/** \file src/bcm/dnx/trunk/trunk_utils.h
 * 
 * Internal DNX TRUNK utils APIs to be used in trunk module 
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _BCM_DNX_TRUNK_TRUNKUTILS_H_INCLUDED
/*
 * { 
 */
#define _BCM_DNX_TRUNK_TRUNKUTILS_H_INCLUDED

/**
 * \brief This file is only used by DNX (JR2 family). Including it by
 * software that is not specific to DNX is an error.
 */
#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/swstate/auto_generated/types/trunk_types.h>
#include <bcm/trunk.h>
#include "trunk_temp_structs_to_skip_papi.h"

/**
 * \brief - convert trunk id to id info
 * 
 * \param [in] unit - unit number
 * \param [in] trunk_id - trunk ID
 * \param [in] id_info - ID info
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_utils_trunk_id_to_id_info_convert(
    int unit,
    int trunk_id,
    bcm_trunk_id_info_t * id_info);

/**
 * \brief - convert id info to trunk id
 * 
 * \param [in] unit - unit number
 * \param [in] trunk_id - turnk ID
 * \param [in] id_info - ID info
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_utils_id_info_to_trunk_id_convert(
    int unit,
    int *trunk_id,
    bcm_trunk_id_info_t * id_info);

/**
 * \brief - decoding the SPA to bank group and member index this
 *        decoding uses DNX Data to know where the partition bit
 *        is found to split between the group to member bits,
 *        this is different between banks.
 * 
 * \param [in] unit - uint number
 * \param [in] pool - ptr to result pool
 * \param [in] group - ptr to result group
 * \param [in] member - ptr to result member
 * \param [in] spa - input System Port Aggregate
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_utils_spa_decode(
    int unit,
    int *pool,
    int *group,
    int *member,
    uint32 spa);

/**
 * \brief - encoding the bank group and member number to a SPA, 
 *        this encoding uses DNX Data to know where the
 *        partition bit is found to split between the group to
 *        member bits, this is different between banks.
 * 
 * \param [in] unit - uint number
 * \param [in] pool - input pool index
 * \param [in] group - input group index
 * \param [in] member - input member index
 * \param [out] spa - ptr to result System Port Aggregate (SPA)
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_utils_spa_encode(
    int unit,
    int pool,
    int group,
    int member,
    uint32 *spa);

/*
 * } 
 */
#endif /* _BCM_DNX_TRUNK_TRUNKUTILS_H_INCLUDED */
