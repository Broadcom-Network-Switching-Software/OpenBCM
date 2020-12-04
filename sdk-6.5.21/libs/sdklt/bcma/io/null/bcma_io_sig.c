/*! \file bcma_io_sig.c
 *
 * NULL implementation of signal abstraction.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/io/bcma_io_sig.h>

int
bcma_io_send_sigint(void)
{
    return -1;
}

int
bcma_io_send_sigquit(void)
{
    return -1;
}
