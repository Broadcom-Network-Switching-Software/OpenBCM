/*! \file bcmdrd_feature.c
 *
 * Track hardware features of switch devices.
 *
 * Driver behavior may change according to the feature set of a given
 * device.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_bitop.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>

/*! Device features. */
static struct features_s {
    SHR_BITDCLNAME(features, BCMDRD_FT_COUNT);
} bcmdrd_features[BCMDRD_CONFIG_MAX_UNITS];

void
bcmdrd_feature_enable(int unit, bcmdrd_feature_t feature)
{
    if (bcmdrd_dev_exists(unit)) {
        SHR_BITSET(bcmdrd_features[unit].features, feature);
    }
}

void
bcmdrd_feature_disable(int unit, bcmdrd_feature_t feature)
{
    if (bcmdrd_dev_exists(unit)) {
        if (feature == BCMDRD_FT_ALL) {
            sal_memset(&bcmdrd_features[unit], 0, sizeof(bcmdrd_features[unit]));
        } else {
            SHR_BITCLR(bcmdrd_features[unit].features, feature);
        }
    }
}

bool
bcmdrd_feature_enabled(int unit, bcmdrd_feature_t feature)
{
    if (bcmdrd_dev_exists(unit)) {
        return SHR_BITGET(bcmdrd_features[unit].features, feature);
    }
    return false;
}
