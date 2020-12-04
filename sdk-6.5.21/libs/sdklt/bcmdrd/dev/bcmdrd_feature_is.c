/*! \file bcmdrd_feature_is.c
 *
 * Convenience functions for various feature combinations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_feature.h>

bool
bcmdrd_feature_is_real_hw(int unit)
{
    bool psim = bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM);
    bool asim = bcmdrd_feature_enabled(unit, BCMDRD_FT_ACTIVE_SIM);
    bool emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);

    if (psim || asim || emul) {
        return false;
    }
    return true;
}

bool
bcmdrd_feature_is_sim(int unit)
{
    bool psim = bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM);
    bool asim = bcmdrd_feature_enabled(unit, BCMDRD_FT_ACTIVE_SIM);

    if (psim || asim) {
        return true;
    }
    return false;
}
