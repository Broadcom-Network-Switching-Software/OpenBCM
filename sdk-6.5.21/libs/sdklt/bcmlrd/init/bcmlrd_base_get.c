/*! \file bcmlrd_base_get.c
 *
 * Get the base variant ID corresponding to the given unit.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlrd/bcmlrd_conf.h>

/*******************************************************************************
 * Local definitions
 */

typedef struct {
    bcmdrd_dev_type_t   dev;
    bcmlrd_variant_t    variant;
} bcmlrd_device_variant_t;

#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    { BCMDRD_DEV_T_##_bd , BCMLRD_VARIANT_T_##_bd##_BASE },

static bcmlrd_device_variant_t bcmlrd_variant_base[] = {
#include <bcmlrd/chip/bcmlrd_variant.h>
};

/*******************************************************************************
 * Public functions
 */

/* Get base variant for a given unit. */
bcmlrd_variant_t
bcmlrd_base_get(int unit)
{
    bcmdrd_dev_type_t t = bcmdrd_dev_type(unit);
    bcmlrd_variant_t v = BCMLRD_VARIANT_T_NONE;

    if (t != BCMDRD_DEV_T_NONE) {
        int i;
        for (i=0; i<COUNTOF(bcmlrd_variant_base); i++) {
            if (bcmlrd_variant_base[i].dev == t) {
                v = bcmlrd_variant_base[i].variant;
                break;
            }
        }
    }

    return v;
}


