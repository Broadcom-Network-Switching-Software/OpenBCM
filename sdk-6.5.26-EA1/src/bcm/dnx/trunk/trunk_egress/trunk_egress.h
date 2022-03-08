/** \file src/bcm/dnx/trunk/trunk_egress/trunk_egress.h
 *
 * Internal DNX egress TRUNK APIs to be used in trunk module
 *
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _BCM_DNX_TRUNK_TRUNKEGRESS_H_INCLUDED
/*
 * {
 */
#define _BCM_DNX_TRUNK_TRUNKEGRESS_H_INCLUDED

/**
 * \brief This file is only used by DNX (JR2 family). Including it by
 * software that is not specific to DNX is an error.
 */
#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/swstate/auto_generated/types/trunk_types.h>
#include <bcm/trunk.h>
#include <bcm_int/dnx/algo/trunk/algo_trunk.h>

/**
 * \brief - align egress trunks to current number and
 *        composition of members in the trunk - this is to allow
 *        egress MC proper behavior.
 *
 * \param [in] unit - unit number
 * \param [in] trunk_id - trunk id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_egress_align(
    int unit,
    bcm_trunk_t trunk_id);

/**
 * \brief - update the dbal with ranges of lb-keys. system ports
 *        which are mapped to local members will be updated
 *        according to their TM port on the relevant core. the
 *        other core or in case that the system port is not
 *        mapped to a local port, the value for the lb key will
 *        be a dedicated invalid value that should cause drop.
 *
 * \param [in] unit - unit number
 * \param [in] egress_trunk_id - egress trunk id - key for the
 *        dbal
 * \param [in] lb_key_ranges_arr_size - size of lb-key ranges
 *        array
 * \param [in] lb_key_ranges_arr - lb-key ranges array
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_egress_lb_key_ranges_dbal_update(
    int unit,
    int egress_trunk_id,
    int lb_key_ranges_arr_size,
    dnx_algo_trunk_member_lb_key_ranges_t * lb_key_ranges_arr);

/**
 * \brief - set hash range for given egress trunk member.
 *
 * \param [in] unit - unit number
 * \param [in] flags - flags
 * \param [in] trunk_id - trunk id
 * \param [in] member - trunk member
 * \param [in] range - range to set
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_egress_member_hash_range_set(
    int unit,
    uint32 flags,
    bcm_trunk_t trunk_id,
    bcm_gport_t member,
    bcm_trunk_member_range_t range);

/**
 * \brief - get hash range for given egress trunk member.
 *
 * \param [in] unit - unit number
 * \param [in] flags - flags
 * \param [in] trunk_id - trunk id
 * \param [in] member - trunk member
 * \param [out] range - range to get
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_egress_member_hash_range_get(
    int unit,
    uint32 flags,
    bcm_trunk_t trunk_id,
    bcm_gport_t member,
    bcm_trunk_member_range_t * range);

#endif /* _BCM_DNX_TRUNK_TRUNKEGRESS_H_INCLUDED */
