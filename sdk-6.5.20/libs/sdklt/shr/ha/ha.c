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

/* Generic instance */
static void *ha_gen_inst;

static int max_ha_units;

/*******************************************************************************
 * Private functions
 */

/*******************************************************************************
 * Public functions
 */

void *
shr_ha_mem_alloc(int unit,
                 shr_ha_mod_id comp_id,
                 shr_ha_sub_id sub_id,
                 const char *blk_id,
                 uint32_t *length)
{
    if (unit < 0 || unit >= max_ha_units || !ha_inst ||
        !ha_inst[unit] || !length) {
        return NULL;
    }

    shr_ha_id_check(blk_id);

    return shr_ha_mm_alloc(ha_inst[unit],
                           comp_id,
                           sub_id,
                           blk_id,
                           length);
}

void *
shr_ha_mem_realloc(int unit, void *mem, uint32_t length)
{
    if (unit < 0 || unit >= max_ha_units || !ha_inst || !ha_inst[unit]) {
        return NULL;
    }
    return shr_ha_mm_realloc(ha_inst[unit], mem, length);
}

int
shr_ha_mem_free(int unit, void *mem)
{
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

    rv = shr_ha_mm_create(mmap, size, alloc_f, free_f,
                          ctx, &ha_inst[unit]);
    if (rv != SHR_E_NONE) {
        return rv;
    }
    return shr_ha_mm_init(ha_inst[unit]);
}

int
shr_ha_unit_close(int unit)
{
    int j;

    if (unit < 0 || unit >= max_ha_units || !ha_inst || !ha_inst[unit]) {
        return SHR_E_UNIT;
    }
    shr_ha_mm_delete(ha_inst[unit]);
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
shr_ha_content_dump(int unit, int (*write_f)(void *buf, size_t len))
{
    if (unit < 0 || unit >= max_ha_units || !ha_inst || !ha_inst[unit]) {
        return SHR_E_UNIT;
    }
    return shr_ha_mm_dump(ha_inst[unit], write_f);
}

int
shr_ha_content_comp(int unit, int (*read_f)(void *buf, size_t len))
{
    if (unit < 0 || unit >= max_ha_units || !ha_inst || !ha_inst[unit]) {
        return SHR_E_UNIT;
    }
    return shr_ha_mm_comp(ha_inst[unit], read_f);
}

int
shr_ha_gen_init(void *mmap,
                int size,
                shr_ha_pool_alloc_f alloc_f,
                shr_ha_pool_free_f free_f,
                void *ctx)
{
    int rv;

    if (ha_gen_inst) {
        return SHR_E_INIT;
    }

    rv = shr_ha_mm_create(mmap, size, alloc_f, free_f,
                          ctx, &ha_gen_inst);
    if (rv != SHR_E_NONE) {
        return rv;
    }
    return shr_ha_mm_init(ha_gen_inst);
}

int
shr_ha_gen_close(void)
{
    if (!ha_gen_inst) {
        return SHR_E_INIT;
    }
    shr_ha_mm_delete(ha_gen_inst);

    ha_gen_inst = NULL;

    return SHR_E_NONE;
}

int
shr_ha_gen_reset(void)
{
    if (!ha_gen_inst) {
        return SHR_E_INIT;
    }
    return shr_ha_mm_reset(ha_gen_inst);
}

void *
shr_ha_gen_mem_alloc(shr_ha_mod_id comp_id,
                     shr_ha_sub_id sub_id,
                     const char *blk_id,
                     uint32_t *length)
{
    if (!ha_gen_inst || !length) {
        return NULL;
    }

    shr_ha_id_check(blk_id);

    return shr_ha_mm_alloc(ha_gen_inst,
                           comp_id,
                           sub_id,
                           blk_id,
                           length);
}

void *
shr_ha_gen_mem_realloc(void *mem, uint32_t length)
{
    if (!ha_gen_inst) {
        return NULL;
    }

    return shr_ha_mm_realloc(ha_gen_inst, mem, length);
}

int
shr_ha_gen_mem_free(void *mem)
{
    if (!ha_gen_inst) {
        return SHR_E_INIT;
    }

    return shr_ha_mm_free(ha_gen_inst, mem);
}

int
shr_ha_gen_content_dump(int (*write_f)(void *buf, size_t len))
{
    if (!ha_gen_inst) {
        return SHR_E_INIT;
    }

    return shr_ha_mm_dump(ha_gen_inst, write_f);
}

int
shr_ha_gen_content_comp(int (*read_f)(void *buf, size_t len))
{
    if (!ha_gen_inst) {
        return SHR_E_INIT;
    }

    return shr_ha_mm_comp(ha_gen_inst, read_f);
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

int
shr_ha_gen_comp_mem_usage(shr_ha_mod_id comp_id,
                          uint32_t report_size,
                          shr_ha_usage_report_t *reports)
{
    if (!ha_gen_inst) {
        return SHR_E_INIT;
    }

    sal_memset(reports, 0, report_size * sizeof(shr_ha_usage_report_t));
    return shr_ha_mm_usage(ha_gen_inst, comp_id,
                           report_size, reports);
}
