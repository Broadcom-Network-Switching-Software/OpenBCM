/*! \file bcmha_mem.c
 *
 * High-availability memory allocation.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <shr/shr_debug.h>
#include <shr/shr_ha.h>

#include <bcmdrd/bcmdrd_types.h>

#include <bcmha/bcmha_mem.h>
#include <bcmha/bcmha_mem_diag.h>

/*******************************************************************************
 * Local definitions
 */

#ifndef ONE_KB
#define ONE_KB  1024
#endif

#ifndef ONE_MB
#define ONE_MB  (1024 * ONE_KB)
#endif

/* Default size of generic HA memory */
#define BCMHA_MEM_SIZE_GEN      (10 * ONE_KB)

/* Default size of per-unit HA memory */
#define BCMHA_MEM_SIZE_UNIT     (4 * ONE_MB)

typedef struct ha_mem_ctrl_s {
    void *ha_mm;
    bcmha_mem_pool_alloc_f pool_alloc;
    bcmha_mem_pool_free_f pool_free;
    void *pool_ctx;
    int null_init;
} ha_mem_ctrl_t;

/*******************************************************************************
 * Local data
 */

static ha_mem_ctrl_t ha_mem_ctrl[BCMDRD_CONFIG_MAX_UNITS + 1];

/*******************************************************************************
 * Private functions
 */

static inline ha_mem_ctrl_t *
ha_mem_ctrl_get(int unit)
{
    if (unit < 0) {
        return &ha_mem_ctrl[BCMDRD_CONFIG_MAX_UNITS];
    }
    if (unit < BCMDRD_CONFIG_MAX_UNITS) {
        return &ha_mem_ctrl[unit];
    }
    return NULL;
}

static void *
ha_mem_pool_alloc(void *ctx, uint32_t requested_size, uint32_t *actual_size)
{
    ha_mem_ctrl_t *hmc = (ha_mem_ctrl_t *)ctx;

    if (hmc && hmc->pool_alloc) {
        return hmc->pool_alloc(hmc->pool_ctx, requested_size, actual_size);
    }
    return NULL;
}

static void
ha_mem_pool_free(void *ctx, void *pool, uint32_t size)
{
    ha_mem_ctrl_t *hmc = (ha_mem_ctrl_t *)ctx;

    if (hmc && hmc->pool_free) {
        hmc->pool_free(hmc->pool_ctx, pool, size);
    }
}

static void *
heap_mem_pool_alloc(void *ctx, uint32_t size, uint32_t *actual_size)
{
    uint32_t chunk_size = ONE_MB;

    *actual_size = chunk_size * ((size + chunk_size - 1) / chunk_size);

    return sal_alloc(*actual_size, "bcmhaHeapMem");
}

static void
heap_mem_pool_free(void *ctx, void *pool, uint32_t len)
{
    sal_free(pool);
}

/*******************************************************************************
 * Public functions
 */

extern int
bcmha_mem_init(int unit,
               bcmha_mem_pool_alloc_f pool_alloc,
               bcmha_mem_pool_free_f pool_free,
               void *pool_ctx,
               void *pool_ptr,
               int pool_size)
{
    int rv;
    ha_mem_ctrl_t *hmc;

    shr_ha_mem_cb_set(BCMDRD_CONFIG_MAX_UNITS,
                      bcmha_mem_alloc,
                      bcmha_mem_realloc,
                      bcmha_mem_free);

    if ((hmc = ha_mem_ctrl_get(unit)) == NULL) {
        return SHR_E_UNIT;
    }

    if (pool_size == 0) {
        pool_size = (unit < 0) ? BCMHA_MEM_SIZE_GEN : BCMHA_MEM_SIZE_UNIT;
    }

    if (hmc->pool_alloc) {
        /*
         * Silently ignore dummy initialization from bcmmgmt, if real
         * HA driver has been installed already.
         */
        if (pool_alloc == NULL) {
            hmc->null_init++;
            return SHR_E_NONE;
        }
        return SHR_E_INIT;
    }

    if (pool_alloc) {
        hmc->pool_alloc = pool_alloc;
        hmc->pool_free = pool_free;
        hmc->pool_ctx = pool_ctx;
    } else {
        /* Fall back to heap memory */
        hmc->pool_alloc = heap_mem_pool_alloc;
        hmc->pool_free = heap_mem_pool_free;
        hmc->pool_ctx = NULL;
    }

    hmc->ha_mm = shr_ha_mm_create(pool_ptr, pool_size,
                                  ha_mem_pool_alloc, ha_mem_pool_free,
                                  hmc);
    if (hmc->ha_mm == NULL) {
        return SHR_E_MEMORY;
    }
    rv = shr_ha_mm_init(hmc->ha_mm);
    if (SHR_FAILURE(rv)) {
        sal_memset(hmc, 0, sizeof(*hmc));
    }
    return rv;
}

int
bcmha_mem_cleanup(int unit)
{
    ha_mem_ctrl_t *hmc;

    if ((hmc = ha_mem_ctrl_get(unit)) == NULL) {
        return SHR_E_UNIT;
    }

    if (hmc->pool_alloc == NULL) {
        /* Nothing to clean up */
        return SHR_E_NONE;
    }

    if (hmc->null_init && hmc->pool_alloc != heap_mem_pool_alloc) {
        /* Silently ignore dummy cleanup from bcmmgmt */
        hmc->null_init--;
        return SHR_E_NONE;
    }

    shr_ha_mm_destroy(hmc->ha_mm);
    sal_memset(hmc, 0, sizeof(*hmc));

    return SHR_E_NONE;
}

int
bcmha_mem_reset(int unit)
{
    ha_mem_ctrl_t *hmc;

    if ((hmc = ha_mem_ctrl_get(unit)) == NULL) {
        return SHR_E_UNIT;
    }
    return shr_ha_mm_reset(hmc->ha_mm);
}

int
bcmha_mem_content_dump(int unit,
                       int (*write_f)(void *ctx, void *buf, size_t len),
                       void *ctx)
{
    ha_mem_ctrl_t *hmc;

    if ((hmc = ha_mem_ctrl_get(unit)) == NULL) {
        return SHR_E_UNIT;
    }

    return shr_ha_mm_dump(hmc->ha_mm, write_f, ctx);
}

int
bcmha_mem_content_comp(int unit,
                       int (*read_f)(void *ctx, void *buf, size_t len),
                       void *ctx)
{
    ha_mem_ctrl_t *hmc;

    if ((hmc = ha_mem_ctrl_get(unit)) == NULL) {
        return SHR_E_UNIT;
    }

    return shr_ha_mm_comp(hmc->ha_mm, read_f, ctx);
}

int
bcmha_mem_usage(int unit, shr_ha_mod_id comp_id,
                uint32_t report_size, shr_ha_usage_report_t *reports)
{
    ha_mem_ctrl_t *hmc;

    if ((hmc = ha_mem_ctrl_get(unit)) == NULL) {
        return SHR_E_UNIT;
    }

    return shr_ha_mm_usage(hmc->ha_mm, comp_id, report_size, reports);
}

void *
bcmha_mem_alloc(int unit,
                shr_ha_mod_id comp_id, shr_ha_sub_id sub_id,
                const char *id_str, uint32_t *length)
{
    ha_mem_ctrl_t *hmc;

    if ((hmc = ha_mem_ctrl_get(unit)) == NULL) {
        return NULL;
    }

    if (length == 0) {
        return NULL;
    }

    return shr_ha_mm_alloc(hmc->ha_mm, comp_id, sub_id, id_str, length);
}

void *
bcmha_mem_realloc(int unit, void *mem, uint32_t length)
{
    ha_mem_ctrl_t *hmc;

    if ((hmc = ha_mem_ctrl_get(unit)) == NULL) {
        return NULL;
    }

    return shr_ha_mm_realloc(hmc->ha_mm, mem, length);
}

int
bcmha_mem_free(int unit, void *mem)
{
    ha_mem_ctrl_t *hmc;

    if ((hmc = ha_mem_ctrl_get(unit)) == NULL) {
        return SHR_E_UNIT;
    }

    return shr_ha_mm_free(hmc->ha_mm, mem);
}
