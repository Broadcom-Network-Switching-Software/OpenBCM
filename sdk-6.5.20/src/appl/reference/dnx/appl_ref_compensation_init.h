/** \file appl_ref_compensation_init.h
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef APPL_REF_COMPENSATION_INIT_H_INCLUDED
/*
 * {
 */
#define APPL_REF_COMPENSATION_INIT_H_INCLUDED

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
/*
 * DEFINEs
 * {
 */
#define APPL_DNX_REF_COMPENSATION_ING_PORT_DELTA (4)

/*
 * }
 */

/**
 * \brief
 * The function is initiate the compensation.
 *  currently, only counter processor and stif compensation per port is made during init
 *
 *\param [in] unit - Number of hardware unit used.
 *\return
 *  None
 *\see
 *   bcm_error_e
 */
shr_error_e appl_dnx_compensation_init(
    int unit);

/**
 * \brief
 * The function is set delta compensation per port.
 *
 *\param [in] unit - Number of hardware unit used.
 *\param [in] port - local port
 *\param [in] delta - delta value
 *\return
 *  None
 *\see
 *   bcm_error_e
 */
shr_error_e appl_dnx_compensation_port_delta_set(
    int unit,
    bcm_port_t port,
    int delta);

/*
 * }
 */
#endif /* APPL_REF_COMPENSATION_INIT_H_INCLUDED */
