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

    if (unit < 0) {
        hmc = &ha_mem_ctrl[BCMDRD_CONFIG_MAX_UNITS];
        if (pool_size == 0) {
            pool_size = BCMHA_MEM_SIZE_GEN;
        }
    } else if (unit < BCMDRD_CONFIG_MAX_UNITS) {
        hmc = &ha_mem_ctrl[unit];
        if (pool_size == 0) {
            pool_size = BCMHA_MEM_SIZE_UNIT;
        }
    } else {
        return SHR_E_UNIT;
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

    if (hmc == &ha_mem_ctrl[BCMDRD_CONFIG_MAX_UNITS]) {
        rv = shr_ha_gen_init(pool_ptr, pool_size,
                             ha_mem_pool_alloc, ha_mem_pool_free, hmc);
    } else {
        rv = shr_ha_unit_init(unit, pool_ptr, pool_size,
                              ha_mem_pool_alloc, ha_mem_pool_free, hmc);
    }
    if (SHR_FAILURE(rv)) {
        sal_memset(hmc, 0, sizeof(*hmc));
    }
    return rv;
}

int
bcmha_mem_cleanup(int unit)
{
    int rv;
    ha_mem_ctrl_t *hmc;

    if (unit < 0) {
        hmc = &ha_mem_ctrl[BCMDRD_CONFIG_MAX_UNITS];
    } else if (unit < BCMDRD_CONFIG_MAX_UNITS) {
        hmc = &ha_mem_ctrl[unit];
    } else {
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

    if (hmc == &ha_mem_ctrl[BCMDRD_CONFIG_MAX_UNITS]) {
        rv = shr_ha_gen_close();
    } else {
        rv = shr_ha_unit_close(unit);
    }
    if (SHR_SUCCESS(rv)) {
        sal_memset(hmc, 0, sizeof(*hmc));
    }
    return rv;
}

int
bcmha_mem_reset(int unit)
{
    if (unit < 0) {
        return shr_ha_gen_reset();
    }
    return shr_ha_unit_reset(unit);
}

int
bcmha_mem_content_dump(int unit, int (*write_f)(void *buf, size_t len))
{
    return shr_ha_content_dump(unit, write_f);
}

int
bcmha_mem_content_comp(int unit, int (*read_f)(void *buf, size_t len))
{
    return shr_ha_content_comp(unit, read_f);
}

int
bcmha_mem_usage(int unit, shr_ha_mod_id comp_id,
                uint32_t report_size, shr_ha_usage_report_t *reports)
{
    if (unit < 0) {
        return shr_ha_gen_comp_mem_usage(comp_id, report_size, reports);
    }
    if (unit < BCMDRD_CONFIG_MAX_UNITS) {
        return shr_ha_comp_mem_usage(unit, comp_id, report_size, reports);
    }
    return SHR_E_UNIT;
}
