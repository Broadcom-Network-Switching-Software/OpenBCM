/*
 * ! \file bcm_int/dnx/mirror/mirror_rule.h Internal DNX MIRROR RULE APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 */

#ifndef _DNX_MIRROR_RULE_INCLUDED__
/*
 * { 
 */
#define _DNX_MIRROR_RULE_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/mirror.h>
#include <shared/shrextend/shrextend_error.h>

/*
 * MACROs
 * {
 */

/*
 * }
 */

/**
 * \brief - maps port & vlan to a mirror destination profile
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   \param [in] port
 *   \param [in] vid - vlan id.
 *   \param [in] flags - [BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS | BCM_MIRROR_PORT_UNTAGGED_ONLY | BCM_MIRROR_PORT_TAGGED_DEFAULT]
 *   \param [in] destid - Mirror destination (profile) index
 *
 * \remark
 *   * BCM_VLAN_INVALID is an indicates for un-tagged mapping. at this case all of the vlan entries with the same port are mapped to specified destination
 * \see
 *   * None
 */
shr_error_e dnx_mirror_rule_add(
    int unit,
    bcm_port_t port,
    bcm_vlan_t vid,
    uint32 flags,
    bcm_gport_t destid);

/**
 * \brief - un-maps port & vlan from a mirror destination profile
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   \param [in] port
 *   \param [in] vid - vlan id.
 *   \param [in] flags - [BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS | BCM_MIRROR_PORT_UNTAGGED_ONLY | BCM_MIRROR_PORT_TAGGED_DEFAULT]
 *   \param [in] current_profile_id - current mirror destination (profile) index
 *
 * \remark
 *   * BCM_VLAN_INVALID is an indicates for un-tagged mapping. at this case all of the vlan entries with the same port are mapped to default destination
 * \see
 *   * None
 */
shr_error_e dnx_mirror_rule_delete(
    int unit,
    bcm_port_t port,
    bcm_vlan_t vid,
    uint32 flags,
    uint32 current_profile_id);

/**
 * \brief - the mapping of port & vlan to a mirror destination profile is returned
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   \param [in] port
 *   \param [in] vid - vlan id.
 *   \param [in] flags - [BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS | BCM_MIRROR_PORT_UNTAGGED_ONLY | BCM_MIRROR_PORT_TAGGED_DEFAULT]
 *   \param [in] snif_profile_id - Mirror destination (profile) index
 *
 * \remark
 *   * BCM_VLAN_INVALID is an indicates for un-tagged mapping. at this case the first vlan entry with the same port mapping is returned
 * \see
 *   * None
 */
shr_error_e dnx_mirror_rule_get(
    int unit,
    bcm_port_t port,
    bcm_vlan_t vid,
    uint32 flags,
    uint32 *snif_profile_id);
/*
 * } 
 */
#endif/*_DNX_MIRROR_RULE_INCLUDED__*/
