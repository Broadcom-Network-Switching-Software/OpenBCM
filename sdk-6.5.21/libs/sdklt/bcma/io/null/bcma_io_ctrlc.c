/*! \file bcma_io_ctrlc.c
 *
 * System inteface for handling Ctrl-C traps.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/io/bcma_io_ctrlc.h>

int
bcma_io_ctrlc_enable_set(int enable)
{
    return -1;
}

int
bcma_io_ctrlc_suspend_set(int suspend)
{
    return -1;
}

/*
 * Please refer to standard C sigsetjmp/siglongjmp documentation.
 */
int
bcma_io_ctrlc_exec(int (*func)(void *), void *data,
                   int (*ctrlc_sig_cb)(void *), void *cb_data,
                   int rv_intr)
{
    return -1;
}
