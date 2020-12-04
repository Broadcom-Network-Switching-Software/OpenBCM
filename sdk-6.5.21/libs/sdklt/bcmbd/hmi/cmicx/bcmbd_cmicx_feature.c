/*! \file bcmbd_cmicx_feature.c
 *
 * Features for CMICx.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_feature.h>

#include <bcmbd/bcmbd_cmicx.h>

int
bcmbd_cmicx_feature_init(int unit)
{
    bcmdrd_feature_enable(unit, BCMDRD_FT_CMICX);

    return SHR_E_NONE;
}
