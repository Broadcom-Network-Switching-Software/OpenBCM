/*! \file bcmbd_otp.c
 *
 * APis for extracting OTP (One-Time Programming) information from
 * switch devices.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmbd/bcmbd.h>

int
bcmbd_otp_param_get(int unit, bcmbd_otp_param_t otp_param, uint32_t *val)
{
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);

    if (bd && bd->otp_param_get) {
        return bd->otp_param_get(unit, otp_param, val);
    }

    return SHR_E_UNAVAIL;
}
