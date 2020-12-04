/*! \file bcmbd_cmicd_feature.c
 *
 * <description>
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_feature.h>

#include <bcmbd/bcmbd_cmicd.h>

int
bcmbd_cmicd_feature_init(int unit)
{
    bcmdrd_feature_enable(unit, BCMDRD_FT_CMICD);

    return SHR_E_NONE;
}
