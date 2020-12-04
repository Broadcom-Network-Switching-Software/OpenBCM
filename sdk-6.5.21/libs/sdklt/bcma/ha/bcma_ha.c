/*! \file bcma_ha.c
 *
 * HA file I/O.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmha/bcmha_mem.h>
#include <bcma/ha/bcma_ha.h>

#define BSL_LOG_MODULE BSL_LS_APPL_HA

/*******************************************************************************
 * Deprecated public functions
 */

int
bcma_ha_unit_open_ext(int unit, int size, bool enabled,
                      bool warm, uint32_t instance)
{
    if (!enabled) {
        /* Use heap memory */
        return bcmha_mem_init(unit, NULL, NULL, NULL,
                              NULL, DEFAULT_HA_FILE_SIZE);
    }
    return bcma_ha_mem_init(unit, size, warm, instance);
}

int
bcma_ha_unit_open(int unit, int size, bool enabled, bool warm)
{
    return bcma_ha_unit_open_ext(unit, size, enabled, warm,
                                 BCMA_HA_INVALID_INSTANCE);
}

int
bcma_ha_unit_close(int unit, bool warm_next)
{
    return bcma_ha_mem_cleanup(unit, warm_next);
}

int
bcma_ha_gen_open_ext(int size, bool enabled, bool warm, uint32_t instance)
{
    if (!enabled) {
        /* Use heap memory */
        return bcmha_mem_init(BCMHA_GEN_UNIT, NULL, NULL, NULL,
                              NULL, DEFAULT_HA_FILE_SIZE);
    }
    return bcma_ha_mem_init(BCMHA_GEN_UNIT, size, warm, instance);
}

int
bcma_ha_gen_open(int size, bool enabled, bool warm)
{
    return bcma_ha_gen_open_ext(size, enabled, warm,
                                BCMA_HA_INVALID_INSTANCE);
}

int
bcma_ha_gen_close(bool warm_next)
{
    return bcma_ha_mem_cleanup(BCMHA_GEN_UNIT, warm_next);
}
