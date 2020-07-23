/*! \file bcmbd_mcs_putc.c
 *
 * MCS console output function.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmbd/bcmbd_mcs.h>
#include <bcmbd/bcmbd_mcs_internal.h>

/*******************************************************************************
 * Local data
 */

/* MCS console output function */
static bcmbd_mcs_putc_f mcs_putc_func;

/*******************************************************************************
 * Public functions
 */

void
bcmbd_mcs_putc(int ch)
{
    if (mcs_putc_func) {
        (void)mcs_putc_func(ch);
    }
}

void
bcmbd_mcs_putc_func_set(bcmbd_mcs_putc_f putc_func)
{
    mcs_putc_func = putc_func;
}
