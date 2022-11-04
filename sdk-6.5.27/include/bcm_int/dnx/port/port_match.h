/** \file port_match.h
 * 
 *
 * Internal DNX Port APIs
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef PORT_MATCH_H_INCLUDED
/* { */
#define PORT_MATCH_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
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

/** }*/

/**
 * \brief -
 *  Verifier for the ESEM default entry input given, as part of port_match_* verifier.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - The ESEM default entry in gport encoding
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *
 * \see
 *  None
 */
shr_error_e dnx_port_match_verify_egress_defualt_esem(
    int unit,
    bcm_gport_t port);

/**
 * \brief -
 *  Get VLAN domain and indication whether the associated in_lif_profile is LIF scope.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] gport - gport that represent the LIF, used to retrieve lif profile info
 * \param [in] gport_hw_resources - contains the global lif information
 * \param [out] vlan_domain - VLAN domain that can be used for key construction.
 * \param [out] is_intf_namespace - indicates whether the associated in_lif_profile is LIF scope.
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
    bcm_gport_t gport,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    uint32 *vlan_domain,
    uint32 *is_intf_namespace);

/**
 * \brief -
 *  Get the esem access command from a port or LAG.
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
shr_error_e dnx_port_match_esem_cmd_get(
    int unit,
    bcm_gport_t port,
    int *esem_cmd);

/**
 * \brief -
 *  Set an esem access command to a port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] physical_port - physical port, or out-lif-id in gport format.
 * \param [in] esem_cmd - esem access command that should be linked to the port
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
shr_error_e dnx_port_match_esem_cmd_port_hw_set(
    int unit,
    bcm_gport_t physical_port,
    int esem_cmd);

/**
 * \brief -
 *  Set an ESEM access command to a port or LIF.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] gport - physical port, or out-lif-id in gport format.
 * \param [in] esem_cmd - esem access command that should be linked to the port
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
shr_error_e dnx_port_match_esem_cmd_hw_set(
    int unit,
    bcm_gport_t gport,
    int esem_cmd);

/**
 * \brief -
 *  Get the esem access command per pp port
 *
 * \param [in] unit - the relevant unit.
 * \param [in] pp_port - PP port
 * \param [in] core_id - Core ID
 * \param [out] esem_cmd - esem access command
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
shr_error_e dnx_port_match_esem_cmd_per_pp_port_get(
    int unit,
    uint32 pp_port,
    bcm_core_t core_id,
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

/**
 * \brief - Convert the port match struct to the a lookup as
 * part of the VLAN-Port struct in order to later use VLAN-Port
 * functions.
 *   \param [in] unit - unit id
 *   \param [in] gport - The gport that is pointed by the match
 *          info.
 *   \param [in] port_match - Pointer to the port match struct
 *          to convert.
 *   \param [out] vlan_port - The result VLAN-Port struct.
 * \return DIRECT OUTPUT
 *   shr_error_e
 * \see
 *   bcm_dnx_port_match_add
 *   dnx_port_match_inlif_update
 */
shr_error_e dnx_port_match_ingress_match_to_vlan_port_match_convert(
    int unit,
    bcm_gport_t gport,
    bcm_port_match_info_t * port_match,
    bcm_vlan_port_t * vlan_port);

/**
 * \brief -
 * Update match info in the SW table.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - Physical port, virtual port or LIF-ID in gport format.
 * \param [in] match_info - pointer to match information.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *  Update SW table with the new GPORT.
 *  The entry should have been already added by bcm_port_match_add.
 *
 * \see
 * None
 */
shr_error_e dnx_port_match_ingress_match_info_sw_update(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * match_info);

/**
 * \brief -
 * Add the match info to SW table.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - Physical port, virtual port or LIF-ID in gport format.
 * \param [in] match_info - pointer to match information.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   Call it after adding the match information to HW. So that if the match information
 *   exists already, it won't be added to SW table too.
 *
 * \see
 *   * None
 */
shr_error_e dnx_port_match_ingress_match_info_sw_add(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * match_info);

/**
 * \brief -
 *  Delete the match info from SW table.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - Physical port, virtual port or LIF-ID in gport format.
 * \param [in] match_info - pointer to match information.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   None.
 *
 * \see
 *   * None
 */
shr_error_e dnx_port_match_ingress_match_info_sw_delete(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * match_info);
/*
 * }
 */

/*
 * }
 */
#endif /* PORT_MATCH_H_INCLUDED */
