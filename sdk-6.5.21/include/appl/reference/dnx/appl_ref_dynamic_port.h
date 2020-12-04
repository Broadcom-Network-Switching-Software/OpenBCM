/** \file appl_ref_dynamic_port.h
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef APPL_REF_DYNAMIC_PORT_H_INCLUDED
/*
 * {
 */
#define APPL_REF_DYNAMIC_PORT_H_INCLUDED

/*
 * }
 */

/*
* Include files.
* {
*/

#include <bcm/types.h>
#include <bcm/error.h>
#include <shared/error.h>
#include <shared/shrextend/shrextend_debug.h>
/*
 * }
 */

/*
 * Typedefs
 * {
 */


/*
 * }
 */

/*
 * DEFINEs
 * {
 */


/*
 * }
 */

/**
 * \brief - Create application for a new port:
 * 1. Create system port
 * 2. Create scheduling scheme
 *
 * \param [in] unit - Unit ID
 * \param [in] port - Port
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
appl_dnx_dynamic_port_create(
    int unit,
    int port);


/**
 * \brief - Destroy application for an existing port.
 *
 * \param [in] unit - Unit ID
 * \param [in] port - Port
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * this function is suitable only for systems with symmetric ports
 * \see
 *   * appl_dnx_dynamic_port_create() function
 */
shr_error_e
appl_dnx_dynamic_port_destroy(
    int unit,
    int port);

/**
 * \brief - Set initial port settings per ETH port
 * Set vlan domain and vlan membership-namespace per port
 * Add the port to BCM_VLAN_DEFAULT
 * Enable same interface filter per port
 * Set untagged packet default VLAN per port
 * Enable learning per poert
 * Set port default-LIF to simple bridge
 *
 * \param [in] unit - Unit ID
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
appl_dnx_dyn_ports_packet_processing_eth_init(
    int unit);

/**
 * \brief - Configure dynamically RCY port with basic PP configurations
 * Configure logical_port==TM port==PP port
 *
 * \param [in] unit - Unit ID
 * \param [in] port - Port ID, uses as logical, TM and  PP port
 * \param [in] core - Core ID
 * \param [in] channel - Logical port channel
 * \param [in] header_type_in - header type for in port, should be used with the BCM_SWITCH_PORT_HEADER_TYPE_XXX flags
 * \param [in] header_type_out - header type for out port, should be used with the BCM_SWITCH_PORT_HEADER_TYPE_XXX flags

 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_dnx_dyn_port_add_rcy(
    int unit,
    uint32 port,
    uint32 core,
    int channel,
    int header_type_in,
    int header_type_out);
/**
 * \brief - Set dynamic port eth initial settings for ETH ports
 *
 * \param [in] unit - Unit ID
 * \param [in] port - Port
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
appl_dnx_dyn_port_pp_eth_initial_settings_set(
    int unit,
    bcm_port_t port);

#endif /* APPL_REF_DYNAMIC_PORT_H_INCLUDED */
