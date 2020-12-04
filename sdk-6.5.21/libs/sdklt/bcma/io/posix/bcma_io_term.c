/*! \file bcma_io_term.c
 *
 * Basic terminal I/O.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <stdio.h>

#include <bcma/io/bcma_io_term.h>

int
bcma_io_term_read(void *buf, int max)
{
    return fread(buf, 1, max, stdin);
}

int
bcma_io_term_write(const void *buf, int count)
{
    int rv;

    rv = fwrite(buf, 1, count, stdout);
    fflush(stdout);
    return rv;
}
