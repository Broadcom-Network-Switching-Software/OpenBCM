/** \file bcm_int/dnx/extender/extender.h
 *
 *  Internal DNX EXTENDER APIs, which are used for
 *  Set/Get/Delete operation of extender entity.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *  All Rights Reserved.
 */
#include <bcm_int/dnx_dispatch.h>
#ifndef EXTENDER_H_INCLUDED
/* { */
#define EXTENDER_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * DEFINEs
 * {
 */

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
 * DECLARATIONs
 * {
 */

/**
 * \brief - Initialize Extender module. \n
 * 1. Initiate the HW table for E-TAG identification and TAG swap parameter..
 * 2. Allocate a SIT profile for 802.1BR, update the relevant HW table
 *
 * \param [in] unit - Unit-ID
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *   * None
 * \see
 *  * None
 */
shr_error_e dnx_extender_init(
    int unit);

/**
 * \brief - Deinitialize Extender module. \n
 * \param [in] unit - Unit-ID
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *  Doing nothing at the moment.
 * \see
 *   * None
 */
shr_error_e dnx_extender_deinit(
    int unit);

/*
 * }
 */

#endif /* EXTENDER_H_INCLUDED */
