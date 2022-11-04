/** \file src/bcm/dnx/trunk/trunk_protection.h
 * 
 * Internal DNX TRUNK protection APIs to be used in trunk module
 * 
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _BCM_DNX_TRUNK_TRUNKPROTECTION_H_INCLUDED
/*
 * { 
 */
#define _BCM_DNX_TRUNK_TRUNKPROTECTION_H_INCLUDED

/**
 * \brief This file is only used by DNX (JR2 family). Including it by
 * software that is not specific to DNX is an error.
 */
#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#include <soc/dnx/swstate/auto_generated/types/trunk_types.h>
#include <bcm/trunk.h>

/** protection remove flags */
/** indication for the function to ignore the member table when removing protection from a member */
#define DNX_TRUNK_PROTECTION_REMOVE_IGNORE_MEMBER_TABLE 0x1

/**
 * \brief - add protection to given member
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] member - member index
 * \param [in] updated_member - updated member
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_member_protection_add(
    int unit,
    int pool,
    int group,
    int member,
    bcm_trunk_member_t * updated_member);

/**
 * \brief - remove protection from given member
 *
 * \param [in] unit - unit number
 * \param [in] flags - protection remove flags
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] member - member index
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_member_protection_remove(
    int unit,
    uint32 flags,
    int pool,
    int group,
    int member);

/**
 * \brief - replace protection of given member
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] member - member index
 * \param [in] updated_member - updated member
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_member_protection_replace(
    int unit,
    int pool,
    int group,
    int member,
    bcm_trunk_member_t * updated_member);

/**
 * \brief - swap protection of given member, master and backup are swapped
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] member - member index
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_member_protection_swap(
    int unit,
    int pool,
    int group,
    int member);

#endif /* _BCM_DNX_TRUNK_TRUNKPROTECTION_H_INCLUDED */
