/*! \file bcmlrd_unit_init.c
 *
 * Assign LRD mappings for all available units.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <bsl/bsl.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_internal.h>
#include <bcmlrd/bcmlrd_conf.h>
#include <bcmlrd/bcmlrd_table.h>

#define SCALAR_COUNT 1

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLRD_TABLE

/* Default LRD device configuration. */
bcmlrd_device_conf_t bcmlrd_default_device_conf = {
    .lrd_dev_conf = bcmlrd_default_dev_conf,
    .ltd_table_conf = &bcmltd_default_table_conf,
};

/* Analyze all attached units and assign mapping configuration */
int
bcmlrd_unit_init(void)
{
    int unit;
    int probe;
    int rv = SHR_E_NONE;
    bcmlrd_variant_t variant;
    uint32_t default_count;
    uint64_t default_variant = 0;

    const bcmlrd_dev_conf_t *lrd_dev_conf = NULL;

    lrd_dev_conf = bcmlrd_dev_conf_get();
    if (lrd_dev_conf == NULL) {
        return SHR_E_INTERNAL;
    }

    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {

        /* Look up base variant. */
        variant = bcmlrd_base_get(unit);
        if (variant == BCMLRD_VARIANT_T_NONE) {
            /* No base variant corresponding to unit, so no
               configuration. */
            continue;
        }

        if (lrd_dev_conf[variant].conf == NULL) {
            /* No configuration for this variant. */
            continue;
        }

        if (lrd_dev_conf[variant].conf->probe != NULL) {
            probe = lrd_dev_conf[variant].conf->probe(unit);
        } else {
            probe = 0;
        }

        if (SHR_FAILURE(probe)) {
            /* Probe failures are catastrophic as probe results
               are now deemed unreliable. */
            rv = probe;
            break;
        }

        if (probe == 0) {
            /* No such device. */
            break;
        }

        /* Set the base variant for this unit. Subsequent components
         may update the variant, but the base is enough to bootstrap
         initialization. */
        rv = bcmlrd_variant_set(unit, variant);
        if (SHR_FAILURE(rv)) {
            break;
        }

        /* Update variant configuration to a possibly non-BASE
           variant. This requires the BASE variant to be set
           already. */
        rv = bcmlrd_field_default_get(unit,
                                      DEVICE_CONFIGt,
                                      DEVICE_CONFIGt_VARIANTf,
                                      SCALAR_COUNT,
                                      &default_variant,
                                      &default_count);

        if (SHR_SUCCESS(rv) &&
            default_count == SCALAR_COUNT &&
            default_variant != 0) {

            variant = bcmlrd_variant_from_device_variant_t(default_variant);

            if (variant != BCMLRD_VARIANT_T_NONE) {
                rv = bcmlrd_variant_set(unit, variant);
                if (SHR_FAILURE(rv)) {
                    break;
                }
            } else {
                /* Set variant to BASE if default variant is unavailable. */
                variant = bcmlrd_base_get(unit);
                rv = bcmlrd_variant_set(unit, variant);
                if (SHR_FAILURE(rv)) {
                    break;
                }
            }
        } else if (rv == SHR_E_UNAVAIL) {
            /* DEVICE_CONFIG may not be available in some (test)
               environments. */
            rv = SHR_E_NONE;
        }

    }

    return rv;
}

