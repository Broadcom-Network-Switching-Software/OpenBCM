/*! \file bcm5699x_stubs.c
 *
 * BCM5699x stubbed subordinate drivers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>

/* Subordinate driver stub functions. */
#define BCM56990_A0_STUB_DRV_ENTRY(_dn,_mn,_bd) \
int _dn##_##_mn##_sub_drv_attach(int unit, void *hdl) \
{ \
    return SHR_E_NONE; \
}
#include "bcm5699x_stubs.h"
