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

#include <soc/ltsw/ha.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/dev.h>

#include <shr/shr_debug.h>
#include <sal/sal_libc.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCM_COMMON

static soc_ltsw_ha_info_t ha_info = {BCMI_HA_COMP_ID_MAX, BCMI_HA_COMP_ID_STRINGS};

int
bcmi_ltsw_ha_init(int unit)
{
    if (!bcmi_ltsw_dev_exists(unit)) {
        return SHR_E_UNIT;
    }
    return soc_ltsw_ha_init(unit, &ha_info);
}

int
bcmi_ltsw_ha_cleanup(int unit)
{
   if (!bcmi_ltsw_dev_exists(unit)) {
       return SHR_E_UNIT;
   }
   return soc_ltsw_ha_cleanup(unit);
}

void *
bcmi_ltsw_ha_mem_alloc(int unit,
                       soc_ltsw_ha_mod_id_t comp_id,
                       soc_ltsw_ha_sub_id_t sub_id,
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
    ptr = soc_ltsw_ha_mem_alloc(unit, comp_id, sub_id, blk_id, length);
    if ((ptr == NULL) || (*length >= len_req)) {
        return ptr;
    }

    /* Request size is bigger than current size, need to realloc. */
    if (*length < len_req) {
        ptr = soc_ltsw_ha_mem_realloc(unit, ptr, len_req);
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

    return soc_ltsw_ha_mem_free(unit, mem);
}

