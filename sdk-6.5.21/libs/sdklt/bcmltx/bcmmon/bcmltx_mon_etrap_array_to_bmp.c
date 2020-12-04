/*! \file bcmltx_mon_etrap_array_to_bmp.c
 *
 * ETRAP table's array to bitmap field Transform Handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bcmltx/bcmmon/bcmltx_mon_etrap_array_to_bmp.h>
#include <bcmltx/bcmport/bcmltx_port_array_to_pbmp.h>
#include <bcmdrd/bcmdrd_field.h>

int
bcmltx_mon_etrap_array_to_bmp_rev_transform(
    int unit,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_transform_arg_t *arg)
{
    return bcmltx_port_array_to_pbmp_rev_transform(unit, in, out, arg);
}

int
bcmltx_mon_etrap_array_to_bmp_transform(
    int unit,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_transform_arg_t *arg)
{
    return bcmltx_port_array_to_pbmp_transform(unit, in, out, arg);
}
