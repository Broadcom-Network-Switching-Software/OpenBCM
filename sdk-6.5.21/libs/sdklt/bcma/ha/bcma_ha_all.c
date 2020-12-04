/*! \file bcma_ha_all.c
 *
 * Consolidate HA initialization for all units and types. This file is
 * independent since it had dependencies on bcmdrd and bcmlt.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_error.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlt/bcmlt.h>
#include <bcma/ha/bcma_ha.h>

/*******************************************************************************
 * Deprecated public functions
 */

int
bcma_ha_open_all(bool enable, bool warm_boot, uint32_t instance)
{
    int rv;
    int unit;

    /* Initialize the generic HA instance. */
    rv = bcma_ha_gen_open_ext(DEFAULT_HA_GEN_FILE_SIZE, enable,
                              warm_boot, instance);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LS_APPL_HA,
                 (BSL_META("Failed to create generic HA memory\n")));
        return rv;
    }

    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        if (!bcmdrd_dev_exists(unit)) {
            continue;
        }
        if (bcmlt_device_attached(unit)) {
            /* Do not open HA file for attached units */
            continue;
        }
        rv = bcma_ha_unit_open_ext(unit, DEFAULT_HA_FILE_SIZE, enable,
                                   warm_boot, instance);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_HA,
                      (BSL_META_U(unit,
                                  "Failed to create HA memory\n")));
            return rv;
        }
        if (warm_boot) {
            char buf[80];
            rv = bcma_ha_mem_name_get(unit, sizeof(buf), buf);
            if (SHR_FAILURE(rv)) {
                LOG_WARN(BSL_LS_APPL_HA,
                         (BSL_META_U(unit,
                                     "Failed to get HA file (%d)\n"), rv));
            } else {
                LOG_INFO(BSL_LS_APPL_HA,
                         (BSL_META_U(unit,
                                     "Warm boot - Using HA file %s\n"), buf));
            }
        }
    }

    return SHR_E_NONE;
}

int
bcma_ha_close_all(bool keep_ha_file)
{
    int rv;
    int unit;

    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        if (!bcmdrd_dev_exists(unit)) {
            continue;
        }
        if (keep_ha_file) {
            char buf[80];
            rv = bcma_ha_mem_name_get(unit, sizeof(buf), buf);
            if (SHR_FAILURE(rv)) {
                LOG_WARN(BSL_LS_APPL_HA,
                         (BSL_META_U(unit,
                                     "Failed to get HA file (%d)\n"), rv));
            } else {
                LOG_INFO(BSL_LS_APPL_HA,
                         (BSL_META_U(unit,
                                     "Warm exit - Keeping HA file %s\n"), buf));
            }
        }
        if (!bcmlt_device_attached(unit)) {
            /* Don't close HA file for attached units */
            bcma_ha_unit_close(unit, keep_ha_file);
        }
    }

    bcma_ha_gen_close(keep_ha_file);

    return SHR_E_NONE;
}
