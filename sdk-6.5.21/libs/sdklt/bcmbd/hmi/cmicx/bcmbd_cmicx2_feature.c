/*! \file bcmbd_cmicx2_feature.c
 *
 * Features for CMICx v2.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_feature.h>

#include <bcmbd/bcmbd_cmicx2.h>

int
bcmbd_cmicx2_feature_init(int unit)
{
    bcmdrd_feature_enable(unit, BCMDRD_FT_CMICX);
    bcmdrd_feature_enable(unit, BCMDRD_FT_CMICX2);

    return SHR_E_NONE;
}
