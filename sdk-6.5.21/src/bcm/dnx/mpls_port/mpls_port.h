/** \file mpls_port.h
 *      Used by mpls_port files
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _MPLS_PORT_INCLUDED__
          /*
           * {
           */
#define _MPLS_L2VPN_COMMON_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif /* BCM_DNX_SUPPORT */

#include <soc/dnx/dbal/dbal.h>
#include <bcm/mpls.h>

/**
* \brief
* Insert requested 'cw' value to CW register CFG_MPLS_CONTROL_WORD.
*   \param [in] unit       -  Relevant unit.
*   \param [in] cw  -  Value to be saved in CFG_MPLS_CONTROL_WORD.
 * \return
 *   \retval - Non-zero in case of an error.
*/
shr_error_e dnx_mpls_cw_set(
    int unit,
    int cw);

/**
* \brief
* Get value from CW register CFG_MPLS_CONTROL_WORD.
*   \param [in] unit       -  Relevant unit.
*   \param [in] cw  -  Stores the retrieved value from CFG_MPLS_CONTROL_WORD.
 * \return
 *   \retval - Non-zero in case of an error.
*/
shr_error_e dnx_mpls_cw_get(
    int unit,
    int *cw);

#endif  /*_MPLS_PORT_INCLUDED__*/
