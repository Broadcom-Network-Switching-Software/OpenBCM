/*! \file bcma_io_term_is.c
 *
 * Test terminal attributes.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <unistd.h>

#include <bcma/io/bcma_io_term.h>

int
bcma_io_term_is_tty(void)
{
    return isatty(0);
}
