/** \file port_dyn.h
 * 
 * Dynamicaly adding and removing ports
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef DNX_PORT_DYN_INCLUDED__
/*
 * { 
 */
#define DNX_PORT_DYN_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#include <bcm/port.h>

/**
 * \brief - Convert port add soc properties to sequence of API calling to 'bcm_port_add'
 * \param [in] unit -
 *     Unit #
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_init_port_add(
    int unit);

/**
 * \brief - Free sw resources allocated in port add process ( dnx_port_init_port_add() )
 * \param [in] unit -
 *     Unit #
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_init_port_add_deinit(
    int unit);

/**
 * \brief - Convert per port soc properties to BCM API calls
 * \param [in] unit -
 *     Unit #
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_init_port_properties_set(
    int unit);

shr_error_e dnx_port_init_port_internal_add(
    int unit,
    int init_db_only);

/**
 * \brief - Free sw resources allocated in port properties process ( dnx_port_init_port_properties_init() )
 * \param [in] unit -
 *     Unit #
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_init_port_properties_deinit(
    int unit);

/**
 * \brief - Convert lane map soc properties to sequence of API calling to 'bcm_port_lane_to_serdes_map_set'
 * \param [in] unit -
 *     Unit #
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_init_port_lane_map_set(
    int unit);

/**
 * \brief - Free sw resources allocated in lane map process ( dnx_port_init_port_lane_map_set() )
 * \param [in] unit -
 *     Unit #
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_init_port_lane_map_deinit(
    int unit);

/**
 * \brief - set the phy tx parameters for every new
 *        port.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * This function should be called only on init sequence.
 * \see
 *   * None
 */
shr_error_e dnx_port_init_serdes_tx_taps_config(
    int unit,
    bcm_port_t port);

/**
 * \brief - Remove the port packet processing properties
 *          and free the coresponding pp ports.
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port
 * \param [in] is_ingress - remove ingress PP properties
 * \param [in] is_egress - remove egress PP properties
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_packet_processing_properties_remove(
    int unit,
    bcm_port_t port,
    int is_ingress,
    int is_egress);

/**
 * \brief - Remove packet processing properties from a certain pp port.
 *          
 *
 * \param [in] unit - Relevant unit
 * \param [in] pp_port - PP port
 * \param [in] core_id - Core id
 * \param [in] is_ingress - remove ingress PP properties
 * \param [in] is_egress - remove egress PP properties
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_packet_processing_properties_remove_internal(
    int unit,
    bcm_port_t pp_port,
    bcm_core_t core_id,
    int is_ingress,
    int is_egress);
/*
 * }
 */
#endif /* DNX_PORT_DYN_INCLUDED__ */
