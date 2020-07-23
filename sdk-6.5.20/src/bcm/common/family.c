/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        family.c
 * Purpose:     Implementation of bcm family management
 */

#include <bcm_int/common/family.h>
#include <bcm/types.h>
#include <bcm/error.h>

static bcm_chip_family_t _family[BCM_MAX_NUM_UNITS];

int 
bcm_chip_family_set(int unit, bcm_chip_family_t f)
{
    _family[unit] = f; 
    return BCM_E_NONE;
}

bcm_chip_family_t
bcm_chip_family_get(int unit)
{
    return _family[unit]; 
}

