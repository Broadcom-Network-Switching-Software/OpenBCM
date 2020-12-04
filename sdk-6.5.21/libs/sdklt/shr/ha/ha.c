/*! \file ha.c
 *
 * HA API implementation.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include "ha_internal.h"

#define BSL_LOG_MODULE BSL_LS_SHR_HA


/*******************************************************************************
 * Local definitions
 */

/* Instance for every unit */
static void **ha_inst;

static int max_ha_units;

/*
 * The variables below are use to implement backward compatibility
 * when the HA memory managers are instantiated from a higher layer.
 * Please refer to the shr_ha_mem_cb_set function as well.
 */
static int ha_mem_cb_unit_max;
static void *(*ha_mem_alloc_cb)(int, shr_ha_mod_id, shr_ha_sub_id,
                                const char *, uint32_t *);
static void *(*ha_mem_realloc_cb)(int, void *, uint32_t);
static int (*ha_mem_free_cb)(int, void *);

/*******************************************************************************
 * Private functions
 */

/*******************************************************************************
 * Public functions
 */

void
shr_ha_mem_cb_set(int cb_unit_max,
                  void *(*alloc_cb)(int,
                                    shr_ha_mod_id, shr_ha_sub_id,
                                    const char *, uint32_t *),
                  void *(*realloc_cb)(int, void *, uint32_t),
                  int (*free_cb)(int, void *))
{
    ha_mem_cb_unit_max = cb_unit_max;
    ha_mem_alloc_cb = alloc_cb;
    ha_mem_realloc_cb = realloc_cb;
    ha_mem_free_cb = free_cb;
}

void *
shr_ha_mem_alloc(int unit,
                 shr_ha_mod_id comp_id,
                 shr_ha_sub_id sub_id,
                 const char *blk_id,
                 uint32_t *length)
{
    if (ha_mem_alloc_cb && unit < ha_mem_cb_unit_max) {
        return ha_mem_alloc_cb(unit, comp_id, sub_id, blk_id, length);
    }

    if (unit < 0 || unit >= max_ha_units || !ha_inst ||
        !ha_inst[unit] || !length) {
        return NULL;
    }

    return shr_ha_mm_alloc(ha_inst[unit],
                           comp_id,
                           sub_id,
                           blk_id,
                           length);
}

void *
shr_ha_mem_realloc(int unit, void *mem, uint32_t length)
{
    if (ha_mem_realloc_cb && unit < ha_mem_cb_unit_max) {
        return ha_mem_realloc_cb(unit, mem, length);
    }

    if (unit < 0 || unit >= max_ha_units || !ha_inst || !ha_inst[unit]) {
        return NULL;
    }
    return shr_ha_mm_realloc(ha_inst[unit], mem, length);
}

int
shr_ha_mem_free(int unit, void *mem)
{
    if (ha_mem_free_cb && unit < ha_mem_cb_unit_max) {
        return ha_mem_free_cb(unit, mem);
    }

    if (unit < 0 || unit >= max_ha_units || !ha_inst || !ha_inst[unit]) {
        return SHR_E_UNIT;
    }
    return shr_ha_mm_free(ha_inst[unit], mem);
}

int
shr_ha_unit_init(int unit,
                 void *mmap,
                 int size,
                 shr_ha_pool_alloc_f alloc_f,
                 shr_ha_pool_free_f free_f,
                 void *ctx)
{
    int rv;
    void **tmp_inst;

    if (unit < 0) {
        return SHR_E_UNIT;
    }
    if (unit >= max_ha_units) {
        /* Allocate HA instances array */
        tmp_inst = ha_inst;
        ha_inst = sal_alloc((unit+2) * sizeof(void *), "shrHaInst");
        if (!ha_inst) {
            ha_inst = tmp_inst;
            return SHR_E_MEMORY;
        }
        sal_memset(ha_inst, 0, (unit + 2) * sizeof(void *));
        if (tmp_inst) {
            sal_memcpy(ha_inst, tmp_inst, max_ha_units * sizeof(void *));
            sal_free(tmp_inst);
        }

        max_ha_units = unit + 2;
    } else if (ha_inst[unit]) {
        return SHR_E_INIT;   /* Already allocated */
    }

    ha_inst[unit] = shr_ha_mm_create(mmap, size, alloc_f, free_f, ctx);
    if (ha_inst[unit] == NULL) {
        return SHR_E_MEMORY;
    }
    rv = shr_ha_mm_init(ha_inst[unit]);
    if (SHR_FAILURE(rv)) {
        shr_ha_mm_destroy(ha_inst[unit]);
    }
    return rv;
}

int
shr_ha_unit_close(int unit)
{
    int j;

    if (unit < 0 || unit >= max_ha_units || !ha_inst || !ha_inst[unit]) {
        return SHR_E_UNIT;
    }
    shr_ha_mm_destroy(ha_inst[unit]);
    ha_inst[unit] = NULL;

    for (j = 0; j < max_ha_units; j++) {
        if (ha_inst[j]) {
            return SHR_E_NONE;
        }
    }

    /* No more units so free the memory that manages the units */
    sal_free(ha_inst);
    ha_inst = NULL;
    max_ha_units = 0;
    return SHR_E_NONE;
}

int
shr_ha_unit_reset(int unit)
{
    if (unit < 0 || unit >= max_ha_units || !ha_inst || !ha_inst[unit]) {
        return SHR_E_UNIT;
    }
    return shr_ha_mm_reset(ha_inst[unit]);
}

int
shr_ha_content_dump(int unit,
                    int (*write_f)(void *ctx, void *buf, size_t len),
                    void *ctx)
{
    if (unit < 0 || unit >= max_ha_units || !ha_inst || !ha_inst[unit]) {
        return SHR_E_UNIT;
    }
    return shr_ha_mm_dump(ha_inst[unit], write_f, ctx);
}

int
shr_ha_content_comp(int unit,
                    int (*read_f)(void *ctx, void *buf, size_t len),
                    void *ctx)
{
    if (unit < 0 || unit >= max_ha_units || !ha_inst || !ha_inst[unit]) {
        return SHR_E_UNIT;
    }
    return shr_ha_mm_comp(ha_inst[unit], read_f, ctx);
}

int
shr_ha_comp_mem_usage(int unit,
                      shr_ha_mod_id comp_id,
                      uint32_t report_size,
                      shr_ha_usage_report_t *reports)
{
    if (unit < 0 || unit >= max_ha_units || !ha_inst || !ha_inst[unit]) {
        return SHR_E_UNIT;
    }

    sal_memset(reports, 0, report_size * sizeof(shr_ha_usage_report_t));

    return shr_ha_mm_usage(ha_inst[unit],
                           comp_id,
                           report_size,
                           reports);
}
