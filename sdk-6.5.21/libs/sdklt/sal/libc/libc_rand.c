/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SDK libc absolute function implementations.
 */

#include <sal/sal_libc.h>

#ifndef sal_rand

#if !defined(RAND_MAX) || RAND_MAX != 32767
#error RAND_MAX must be 32767
#endif

static unsigned long next = 1;

int
sal_rand(void)
{
    next = next * 1103515245 + 12345;

    /* RAND_MAX assumed to be 32767 */
    return ((unsigned int)(next / 65536) % 32768);
}

void
sal_srand(unsigned int seed)
{
    next = seed;
}

#endif
