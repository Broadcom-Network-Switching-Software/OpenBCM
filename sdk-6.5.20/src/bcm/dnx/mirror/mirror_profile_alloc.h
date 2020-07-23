/** \file mirror_profile_alloc.h
 * 
 * Internal DNX mirror profile resource manager APIs 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _ALGO_MIRROR_PROFILE_ALLOC_INCLUDED__
/*
 * { 
 */
#define _ALGO_MIRROR_PROFILE_ALLOC_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * INCLUDE FILES:
 * {
 */
#include <shared/shrextend/shrextend_error.h>
#include <bcm/mirror.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_mirror_access.h>
/*
 * }
 */

/*
 * MACROs
 * {
 */

/**
 * \brief - res mngr for mirror application
 */
#define DNX_ALGO_SNIF_RES_MNGR_INGRESS_PROFILES_MIRROR        "SNIF_INGRESS_PROFILES_MIRROR"
/**
 * \brief - res mngr for snoop application
 */
#define DNX_ALGO_SNIF_RES_MNGR_INGRESS_PROFILES_SNOOP         "SNIF_INGRESS_PROFILES_SNOOP"
/**
 * \brief - res mngr for statistical sampling application
 */
#define DNX_ALGO_SNIF_RES_MNGR_INGRESS_PROFILES_STAT_SAMPLING "SNIF_INGRESS_PROFILES_STAT_SAMPLING"
/*
 * }
 */

/**
 * \brief - allocate new vlan-id profile for ingress/egress sniffing from allocation manager.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   \param [in] core_id
 *   \param [in] flags - [BCM_MIRROR_PORT_INGRESS|BCM_MIRROR_PORT_EGRESS]
 *   \param [in] vlan_id
 *   \param [in] action_profile_id - Mirror destination (profile) index
 *   \param [in] first_reference - indicated whether this is the first reference of this vlan-id
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_mirror_vlan_profile_allocate(
    int unit,
    int core_id,
    uint32 flags,
    bcm_vlan_t vlan_id,
    int *action_profile_id,
    uint8 *first_reference);

/**
 * \brief - deallocate vlan-id profile for ingress/egress sniffing from allocation manager.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   \param [in] core_id
 *   \param [in] flags - [BCM_MIRROR_PORT_INGRESS|BCM_MIRROR_PORT_EGRESS]
 *   \param [in] vlan_id
 *   \param [in] last_reference - indicated whether this is the last reference of this vlan-id
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_mirror_vlan_profile_deallocate(
    int unit,
    uint32 core_id,
    uint32 flags,
    bcm_vlan_t vlan_id,
    uint8 *last_reference);

/**
 * \brief - gets vlan-id profile for ingress/egress sniffing from allocation manager.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   \param [in] core_id
 *   \param [in] flags - [BCM_MIRROR_PORT_INGRESS|BCM_MIRROR_PORT_EGRESS]
 *   \param [in] vlan_id
 *   \param [in] action_profile_id - Mirror destination (profile) index
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_mirror_vlan_profile_get(
    int unit,
    int core_id,
    uint32 flags,
    bcm_vlan_t vlan_id,
    int *action_profile_id);

/**
 * \brief - allocate new sniffing profile. Each application (mirror, snoop, statistical sample) has its own 
 * allocation manager. 
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   \param [in] mirror_dest - Mirror profile attributes
 *   \param [in] action_profile_id - Mirror profile ID
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_mirror_profile_allocate(
    int unit,
    bcm_mirror_destination_t * mirror_dest,
    int *action_profile_id);

/**
 * \brief - deallocate sniff profile. Each application (mirror, snoop, statistical sample) has its own
 * allocation manager.  
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   \param [in] mirror_dest_id - gport of mirror profile 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_mirror_profile_deallocate(
    int unit,
    bcm_gport_t mirror_dest_id);

/**
 * \brief - Check if given sniff profile is already allocated in resource manager
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit Id
 *   \param [in] mirror_dest_id - gport of mirror profile
 *   \param [in] is_allocated - return value:
 *     * 1 - profile is allocated \n
 *       0 - profile is not allocated
 *   
 * \par INDIRECT INPUT:
 *   * Snif allocation manager
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * dnx_mirror_profile_deallocate(), dnx_mirror_profile_allocate() functions \n
 *     for allocation and deallocation. 
 */
shr_error_e dnx_mirror_profile_is_allocated(
    int unit,
    bcm_gport_t mirror_dest_id,
    uint8 *is_allocated);

/**
 * \brief - Initialize resource manager for mirror module.
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * Snif resource manager tables
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_mirror_profile_res_mngr_init(
    int unit);

/*
 * } 
 */
#endif/*_ALGO_MIRROR_PROFILE_ALLOC_INCLUDED__*/
