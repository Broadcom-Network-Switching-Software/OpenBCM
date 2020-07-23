/*! \file sal_udelay.c
 *
 * Delay API (for small delays).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_time.h>

void
sal_udelay(sal_usecs_t usec)
{
    sal_usecs_t start = sal_time_usecs();

    while (sal_time_usecs() - start <= usec) {
        ;
    }
}
