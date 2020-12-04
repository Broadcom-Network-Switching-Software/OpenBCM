/** \file trap_tune.h
 * 
 * 
 * Internal initializations for trap 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _TRAP_TUNE_H_INCLUDED_
/** { */
#define _TRAP_TUNE_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * \brief - Initialization for trap.
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
shr_error_e dnx_tune_trap_init(
    int unit);

#endif /*_TRAP_TUNE_H_INCLUDED_*/
