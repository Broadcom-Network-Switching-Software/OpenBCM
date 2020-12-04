/** \file nif_tune.h
 * 
 * 
 * Internal DNX Port APIs 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _NIF_TUNE_H_INCLUDED_
/** { */
#define _NIF_TUNE_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/types.h>

/**
 * \brief - set the default priority configuration for every new 
 *        port. this configuration can be later overriden by
 *        bcm_port_prioroity_config_set API.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int dnx_port_tune_port_add_priority_config(
    int unit,
    bcm_port_t port);

/**
 * \brief - set the default phy control configuration for every new
 *        port. this configuration can be later overriden by
 *        bcm_port_phy_control_set API.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int dnx_port_tune_port_add_phy_control_config(
    int unit,
    bcm_port_t port);

#endif /*_NIF_TUNE_H_INCLUDED_*/
