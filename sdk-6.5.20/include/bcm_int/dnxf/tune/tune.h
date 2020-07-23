/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
 /*
  * ! \file bcm_int/dnx/tune/tune.h
  * Reserved.$ 
  */

#ifndef _DNXF_TUNE_H_INCLUDED_
/*
 * { 
 */
#define _DNXF_TUNE_H_INCLUDED_

#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by Ramon only!"
#endif

#include <bcm/types.h>

/**
 * \brief - perform  tune module init
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxf_tune_init(
    int unit);

/**
 * \brief - perform tune when a port is added
 *
 * \param [in] unit -
 *   Unit-ID
 * \param [in] port -
 *   Port-ID
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxf_tune_port_add(
    int unit,
    bcm_port_t port);

/**
 * \brief - perform tune when a port is removed
 *
 * \param [in] unit -
 *   Unit-ID
 * \param [in] port -
 *   Port-ID
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxf_tune_port_remove(
    int unit,
    bcm_port_t port);

/** } */
#endif /*_DNXF_TUNE_H_INCLUDED_*/
