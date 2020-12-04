/*! \file sal_sleep.h
 *
 * Sleep API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SAL_SLEEP_H
#define SAL_SLEEP_H

#include <sal/sal_time.h>

/*!
 * \brief Sleep for N seconds.
 *
 * \param [in] sec number of seconds to sleep.
 *
 * \return Nothing.
 */
extern void
sal_sleep(unsigned int sec);

/*!
 * \brief Sleep for N microseconds.
 *
 * \param [in] usec number of microseconds to sleep.
 *
 * \return Nothing.
 */
extern void
sal_usleep(sal_usecs_t usec);

#endif /* SAL_SLEEP_H */
