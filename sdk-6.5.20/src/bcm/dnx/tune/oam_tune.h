/** \file oam_tune.h
 * 
 * 
 * Internal initializations for oam 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _OAM_TUNE_H_INCLUDED_
/** { */
#define _OAM_TUNE_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * }
 */

/*
* Include files.
* {
*/
#include <shared/bsl.h>
#include <shared/bslenum.h>
#include <shared/error.h>
#include <shared/shrextend/shrextend_debug.h>

#include <bcm/oam.h>
#include <bcm/types.h>
#include <bcm/rx.h>
#include <bcm/error.h>
#include <bcm/port.h>

/**
 * \brief - Initialization for oam.
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *  None.
 *
 * \see
 *  None.
 */
shr_error_e dnx_tune_oam_init(
    int unit);

#endif /*_OAM_TUNE_H_INCLUDED_*/
