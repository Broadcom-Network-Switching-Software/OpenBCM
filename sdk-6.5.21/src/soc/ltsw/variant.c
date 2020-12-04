/*! \file variant.c
 *
 * Define chip variants driver list.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#if defined(BCM_LTSW_SUPPORT)

#include <soc/cm.h>
#include <bsl/bsl.h>
#include <bcmlrd/bcmlrd_conf.h>

/******************************************************************************
 * Local definitions
 */
#define TO_STRINGS(_a) #_a
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1)\
            TO_STRINGS(_bu##_##_ve),

static char *variant_drv_name[] = {
    NULL,
#include <bcmlrd/chip/bcmlrd_variant.h>
};

#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1)\
            TO_STRINGS(_ve),
static char *variant_name[] = {
    NULL,
#include <bcmlrd/chip/bcmlrd_variant.h>
};

/******************************************************************************
 * Private functions
 */


/******************************************************************************
 * Public Functions
 */
void
soc_ltsw_variant_drivers_show(void)
{
    int i;
    int count = sizeof(variant_drv_name) / sizeof(variant_drv_name[0]);
    LOG_CLI(("\nVariant drivers:\n"));
    for (i = 0; i < count;) {
        if (i == BCMLRD_VARIANT_T_NONE) {
            i++;
            continue;
        }
        if (i % 4 == 0) {
            LOG_CLI(("     "));
        }
        LOG_CLI(("%s", variant_drv_name[i]));
        i++;

        if (i == count) {
            LOG_CLI(("\n"));
            break;
        }

        if (i % 4) {
            LOG_CLI((", "));
        } else {
            LOG_CLI(("\n"));
        }
    }
    return;
}

int
soc_ltsw_variant_name_get(int unit, const char **name) {
    bcmlrd_variant_t variant_e = BCMLRD_VARIANT_T_NONE;
    int count = sizeof(variant_name) / sizeof(variant_name[0]);

    variant_e = bcmlrd_variant_get(unit);
    if (!((variant_e > BCMLRD_VARIANT_T_NONE) && (variant_e < count))) {
        return -1;
    }

   *name = variant_name[variant_e];

    return 0;
}
#endif /* BCM_LTSW_SUPPORT */
