/*! \file sal_udelay.h
 *
 * Delay API (for small delays).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SAL_UDELAY_H
#define SAL_UDELAY_H

#include <sal/sal_time.h>

/*!
 * \brief Busy-wait for N microseconds.
 *
 * Mainly intended for very small delays during hardware
 * initialization.
 *
 * \param [in] usec Number of microseconds to dealy execution.
 *
 * \return Nothing.
 */
extern void
sal_udelay(sal_usecs_t usec);

#endif /* SAL_UDELAY_H */
