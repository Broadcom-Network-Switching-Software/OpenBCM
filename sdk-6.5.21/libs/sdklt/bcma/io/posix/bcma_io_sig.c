/*! \file bcma_io_sig.c
 *
 * POSIX signal abstraction.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#include <bcma/io/bcma_io_sig.h>

int
bcma_io_send_sigint(void)
{
    return kill(getpid(), SIGINT);
}

int
bcma_io_send_sigquit(void)
{
    return kill(getpid(), SIGQUIT);
}
