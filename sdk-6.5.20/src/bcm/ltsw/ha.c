/*! \file ha.c
 *
 * HA API wrapper.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/types.h>
#include <bcm_int/control.h>
#include <bcm_int/ltsw_dispatch.h>

#include <shr/shr_debug.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/dev.h>
#include <bcmdrd_config.h>
#include <sal/sal_libc.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCM_COMMON

/*
 * The HA lib supports up to 16 HA instances. In SDKLT, each unit takes
 * one HA instance. In BCM API Emulator(BAE), each unit uses separate share
 * file than files created by SDKLT, thus takes the HA instance with the
 * offset of BCMDRD_CONFIG_MAX_UNITS.
 */
#define HA_UNIT(_u) (bcmi_ltsw_dev_dunit(_u) + BCMDRD_CONFIG_MAX_UNITS)

void *
bcmi_ltsw_ha_mem_alloc(int unit,
                       shr_ha_mod_id comp_id,
                       shr_ha_sub_id sub_id,
                       const char *blk_id,
                       uint32_t *length)
{
    void *ptr = NULL;
    uint32_t len_req = 0;

    if (!bcmi_ltsw_dev_exists(unit)) {
        return NULL;
    }

    if (comp_id > BCMI_HA_COMP_ID_MAX) {
        return NULL;
    }

    if (!length) {
        return NULL;
    }

    len_req = *length;
    ptr = shr_ha_mem_alloc(HA_UNIT(unit), comp_id, sub_id, blk_id, length);
    if ((ptr == NULL) || (*length >= len_req)) {
        return ptr;
    }

    /* Request size is bigger than current size, need to realloc. */
    if (*length < len_req) {
        ptr = shr_ha_mem_realloc(HA_UNIT(unit), ptr, len_req);
        if (ptr != NULL) {
            *length = len_req;
        }
    }

    return ptr;
}

int
bcmi_ltsw_ha_mem_free(int unit, void *mem)
{
    if (!bcmi_ltsw_dev_exists(unit)) {
        return SHR_E_UNIT;
    }

    return shr_ha_mem_free(HA_UNIT(unit), mem);
}

uint32_t
bcmi_ltsw_ha_comp_id_from_name(const char *name)
{
    int comp_id;
    const char *comp_name[] = BCMI_HA_COMP_ID_STRINGS;

    for (comp_id = 0; comp_id < BCMI_HA_COMP_ID_MAX; comp_id++) {
        if (sal_strcasecmp(name, comp_name[comp_id]) == 0) {
            break;
        }
    }

    return comp_id;
}

const char *
bcmi_ltsw_ha_comp_name_from_id(uint32_t comp_id)
{
    const char *comp_name[] = BCMI_HA_COMP_ID_STRINGS;

    if (comp_id < BCMI_HA_COMP_ID_MAX) {
        return comp_name[comp_id];
    }

    return NULL;
}

