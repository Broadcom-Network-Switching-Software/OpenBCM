/** \file appl_ref_srv6_field_init_deinit.h
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef APPL_REF_FIELD_SRV6_INIT_H_INCLUDED
/*
 * {
 */
#define APPL_REF_FIELD_SRV6_INIT_H_INCLUDED

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

/*
 * }
 */

/**
 * \brief
 *   This function configures the PMFs for SRV6 Tunnel End-Point and Egress processing
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e appl_dnx_field_srv6_init(
    int unit);

/**
 * \brief
 *   This function deinits SRV6 Tunnel End-Point and Egress processing
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e appl_dnx_field_srv6_deinit(
    int unit);

/*
 * }
 */
#endif /* APPL_REF_FIELD_SRV6_INIT_H_INCLUDED */
