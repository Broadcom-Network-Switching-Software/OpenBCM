/*! \file bcmbd_fwm_parser.c
 *
 * Functions of parser of Firmware Management.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_libc.h>
#include <shr/shr_types.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>
#include <bcmdrd_config.h>
#include <bcmbd/bcmbd_fwm.h>
#include "./bcmbd_fwm_internal.h"

/* Initialize parser driver. */
int
bcmbd_fwm_parser_init(int unit)
{
    return SHR_E_NONE;
}

/* Cleanup parser driver. */
int
bcmbd_fwm_parser_cleanup(int unit)
{
    return SHR_E_NONE;
}


