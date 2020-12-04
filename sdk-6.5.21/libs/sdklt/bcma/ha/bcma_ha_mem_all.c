/*! \file bcma_ha_mem_all.c
 *
 * Consolidate HA initialization for all units and types. This file is
 * independent since it has dependencies on bcmdrd and bcmlt.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlt/bcmlt.h>
#include <bcmha/bcmha_mem.h>

#include <bcma/ha/bcma_ha.h>

#define BSL_LOG_MODULE BSL_LS_APPL_HA

/*******************************************************************************
 * Public functions
 */

int
bcma_ha_mem_init_all(bool warm_boot, uint32_t instance)
{
    int rv;
    int unit;

    /* Initialize the generic HA instance. */
    rv = bcma_ha_mem_init(BCMHA_GEN_UNIT, DEFAULT_HA_GEN_FILE_SIZE,
                          warm_boot, instance);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Failed to create generic HA memory\n")));
        return rv;
    }

    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        if (!bcmdrd_dev_exists(unit)) {
            continue;
        }
        if (bcmlt_device_attached(unit)) {
            /* Do not initialize HA memory for attached units */
            continue;
        }
        rv = bcma_ha_mem_init(unit, DEFAULT_HA_FILE_SIZE,
                              warm_boot, instance);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Failed to create HA memory\n")));
            return rv;
        }
        if (warm_boot) {
            char buf[80];
            rv = bcma_ha_mem_name_get(unit, sizeof(buf), buf);
            if (SHR_FAILURE(rv)) {
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "Failed to get HA file (%s)\n"),
                          shr_errmsg(rv)));
            } else {
                LOG_INFO(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "Warm boot - using HA file %s\n"),
                          buf));
            }
        }
    }

    return SHR_E_NONE;
}

int
bcma_ha_mem_cleanup_all(bool keep_ha_file)
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
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "Failed to get HA file (%s)\n"),
                          shr_errmsg(rv)));
            } else {
                LOG_INFO(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "Warm exit - keeping HA file %s\n"),
                          buf));
            }
        }
        if (!bcmlt_device_attached(unit)) {
            /* Do not cleanup HA memory for attached units */
            bcma_ha_mem_cleanup(unit, keep_ha_file);
        }
    }

    bcma_ha_mem_cleanup(BCMHA_GEN_UNIT, keep_ha_file);

    return SHR_E_NONE;
}
