/** \file src/bcm/dnx/trunk/trunk_verify.h
 * 
 * Internal DNX TRUNK verify APIs to be used in trunk module 
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _BCM_DNX_TRUNK_TRUNKVERIFY_H_INCLUDED
/*
 * { 
 */
#define _BCM_DNX_TRUNK_TRUNKVERIFY_H_INCLUDED

/**
 * \brief This file is only used by DNX (JR2 family). Including it by
 * software that is not specific to DNX is an error.
 */
#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <soc/dnx/swstate/auto_generated/types/trunk_types.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm/trunk.h>
#include <bcm/types.h>

/**
 * \brief - verify that trunk id is valid 
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
shr_error_e dnx_trunk_t_verify(
    int unit,
    bcm_trunk_t trunk_id);

/**
 * \brief - verify function to check if a trunk was created
 * 
 * \param [in] unit - unit number
 * \param [in] id_info - id info
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_was_created_verify(
    int unit,
    bcm_trunk_id_info_t * id_info);

/**
 * \brief - verify function to check if a trunk can contain a 
 *        certain amount of members
 * 
 * \param [in] unit - unit number
 * \param [in] id_info - id info
 * \param [in] new_nof_members - check if trunk can contain this 
 *        number of members
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_can_fit_new_amount_of_members_verify(
    int unit,
    bcm_trunk_id_info_t * id_info,
    int new_nof_members);

/**
 * \brief - verify function to check if a certain PSC is 
 *        supported
 * 
 * \param [in] unit - unit number
 * \param [in] psc - BCM port selection criteria
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_psc_verify(
    int unit,
    int psc);

/**
 * \brief - verify member flags are valid, currently valid flags 
 *        are BCM_TRUNK_MEMBER_INGRESS_DISABLE and
 *        BCM_TRUNK_MEMBER_EGRESS_DISABLE, in addition if the
 *        INGRESS_DISABLE flag is recievd it is replaced with
 *        the EGRESS_DISABLE one. this is because the previous
 *        is a legacy flag that users shouldn't use - however if
 *        they do it is replaced with the relevant one istead of
 *        returning an error
 * 
 * \param [in] unit - unit number
 * \param [in] flags - flags
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_member_flags_verify(
    int unit,
    uint32 *flags);

/**
 * \brief - Verify that the trunk header type is matching the new member's header type,
 *          if header type for the trunk was never set the default is invalid value
 *
 * \param [in] unit - unit number
 * \param [in] id_info - trunk id info
 * \param [in] gport - gport of the new member that is added to trunk
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_new_member_has_matching_header_type_verify(
    int unit,
    bcm_trunk_id_info_t * id_info,
    bcm_gport_t gport);

/**
 * \brief - Verify that the given system port (sysport) physical mapping (modport) also exists in one of the member system
 * ports (member_array).
 *
 * \param [in] unit - unit number
 * \param [in] sysport - modport gport to match
 * \param [in] member_count - amount of members in member_array
 * \param [in] member_array - array of trunk members
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_trunk_modport_membership_verify(
    int unit,
    bcm_gport_t sysport,
    int member_count,
    bcm_trunk_member_t *member_array);

#endif /* _BCM_DNX_TRUNK_TRUNKVERIFY_H_INCLUDED */
