/** \file port_match.h
 * 
 *
 * Internal DNX Port APIs
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef PORT_MATCH_H_INCLUDED
/* { */
#define PORT_MATCH_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm_int/dnx/algo/port_pp/algo_port_pp.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
/*
 * Defines {
 */

/** }*/
/*
 * Structures {
 */

/**
 * \brief -
 *  Get VLAN domain and indication whther the assosiated in_lif_profile is LIF scope.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] gport_hw_resources - contains the global lif information
 * \param [out] vlan_domain - VLAN domain that can be used for key construction.
 * \param [out] is_intf_namespace - indicates whether the assosiated in_lif_profile is LIF scope.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *
 * \see
 *  None
 */
shr_error_e dnx_ingress_native_ac_interface_namespace_check(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
    uint32 *vlan_domain,
    uint32 *is_intf_namespace);

/**
 * \brief -
 *  Allocate or deallocate an SVTAG ESEM command call on the port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] gport -the tunnel ID or the TM port ID
 * \param [in] enable - enable or disable the ESEM CMD).
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  None
 *
 * \see
 *  None
 */
shr_error_e dnx_port_match_svtag_esem_cmd_set(
    int unit,
    bcm_gport_t gport,
    uint8 enable);

/**
 * \brief -
 *  Get the esem access command from a port or LIF.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - physical port, or out-lif-id in gport format.
 * \param [out] esem_cmd - esem access command that the port was using before.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  None
 *
 * \see
 *  None
 */
shr_error_e dnx_port_match_esem_cmd_port_get(
    int unit,
    bcm_gport_t port,
    int *esem_cmd);

/**
 * \brief - Support a modification of local In-LIF ID in all
 *  relevant port-match HW tables as part of a REPLACE
 *  operation. The function should handle all the ISEM and
 *  PP-Port entries that were created by the port-match APIs and
 *  point to the specified In-LIF ID that is to be modified.
 *
 * \param [in] unit - unit ID.
 * \param [in] algo_flags - algo_gpm flags that represent the
 *        local In-LIF in terms of SBC/DPC.
 * \param [in] is_virtual - Signal whether it's a virtual local
 *        In-LIF
 * \param [in] old_local_in_lif_id - The local In-LIF ID to be
 *        modified.
 * \param [in] new_local_in_lif_id - The new local In-LIF ID
 *        that needs to be set
 * \return DIRECT OUTPUT
 *   shr_error_e
 * \see
 *   dnx_vlan_port_match_inlif_update
 */
shr_error_e dnx_port_match_inlif_update(
    int unit,
    uint32 algo_flags,
    int is_virtual,
    uint32 old_local_in_lif_id,
    uint32 new_local_in_lif_id);

/*
 * }
 */

/*
 * }
 */
#endif /* PORT_MATCH_H_INCLUDED */
