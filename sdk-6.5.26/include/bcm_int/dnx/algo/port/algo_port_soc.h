/** \file algo_port_soc.h
 *
 *  PORT manager, Port utilities, and Port verifiers.
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef ALGO_PORT_SOC_H_INCLUDED
/*
 * {
 */
#define ALGO_PORT_SOC_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/*
 * INCLUDE FILES:
 * {
 */

#include <bcm/port.h>
/*
 * }
 */

/*
 * MACROs
 * {
 */

/*
 * }
 */
/*
 * Functions
 * {
 */

/**
 * \brief - add \ remove port from SOC_INFO port bitmaps
 *          To remove use interface_type BCM_PORT_IF_NULL
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] logical_port - logical port #
 *   \param [in] port_info - basic port info
 *   \param [in] tdm_mode - tdm mode of the relevant port (DNX_ALGO_PORT_TDM_MODE_NONE if not relevant)
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * SOC INFO port bitmaps
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_soc_info_port_bitmaps_set(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_info_s port_info,
    dnx_algo_port_tdm_mode_e tdm_mode);

/**
 * \brief - Update ETH bitmap by given logical_port and header_mode.
 *
 * \param [in] unit - Relevant unit
 * \param [in] local_port - Local port
 * \param [in] header_mode - Header mode
 * The header modes are:
 * DNX_ALGO_PORT_HEADER_MODE_NON_PP
 * DNX_ALGO_PORT_HEADER_MODE_ETH
 * DNX_ALGO_PORT_HEADER_MODE_RAW
 * DNX_ALGO_PORT_HEADER_MODE_INJECTED_2_PP
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_soc_info_port_eth_bitmap_set(
    int unit,
    bcm_port_t local_port,
    dnx_algo_port_header_mode_e header_mode);

/**
 * \brief - Get reference clock of a port.
 *
 * \param [in] unit - Relevant unit
 * \param [in] logical_port - Logical port
 * \param [out] ref_clk - reference clock
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_soc_ref_clk_get(
    int unit,
    bcm_port_t logical_port,
    int *ref_clk);

/**
 * \brief -
 * Set port name and alternative port name - used mostly be soc property read.
 * The name set according to port type and port interface type (read from dnx algo port DB)
 * This function will be called after port add and remove to update port name.
 *
 * to remove set port_info to DNX_ALGO_PORT_TYPE_INVALID
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] logical_port - required logical port.
 *   \param [in] port_info - port type of the added /removed port
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * SOC INFO port name
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_soc_info_port_name_set(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_info_s port_info);
/*
 * }
 */

#endif /** ALGO_PORT_SOC_H_INCLUDED*/
