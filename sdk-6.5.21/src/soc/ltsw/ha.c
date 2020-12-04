/*! \file ha.c
 *
 * High-availability memory allocation.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/cm.h>
#include <soc/ltsw/ha.h>

#include <shr/shr_error.h>
#include <bcmdrd_config.h>
#include <sal/sal_libc.h>

/*******************************************************************************
 * Local definitions
 */

typedef struct ha_mem_ctrl_s {
    void *ha_mm;
    soc_ltsw_ha_pool_alloc_f pool_alloc;
    soc_ltsw_ha_pool_free_f pool_free;
    void *pool_ctx;
    int null_init;
} ha_mem_ctrl_t;

typedef struct ha_ctrl_s {
    bool inited;
    soc_ltsw_ha_info_t *ha_info;
} ha_ctrl_t;

/*******************************************************************************
 * Local data
 */

static ha_mem_ctrl_t ha_mem_ctrl[SOC_MAX_NUM_DEVICES + 1];
static ha_ctrl_t ha_ctrl[SOC_MAX_NUM_DEVICES + 1];

/*******************************************************************************
 * Private functions
 */

static inline ha_mem_ctrl_t *
ha_mem_ctrl_get(int unit)
{
    if (unit < 0) {
        return &ha_mem_ctrl[SOC_MAX_NUM_DEVICES];
    }
    if (unit < SOC_MAX_NUM_DEVICES) {
        return &ha_mem_ctrl[unit];
    }
    return NULL;
}


/*******************************************************************************
 * Public functions
 */

int
soc_ltsw_ha_mem_init(int unit,
                     soc_ltsw_ha_pool_alloc_f pool_alloc,
                     soc_ltsw_ha_pool_free_f pool_free,
                     void *pool_ctx,
                     void *pool_ptr,
                     int pool_size)
{
    int dunit;
    if ((soc_cm_dev_num_get(unit, &dunit)) < 0) {
        return SHR_E_UNIT;
    }
    dunit += BCMDRD_CONFIG_MAX_UNITS;
    return shr_ha_unit_init(dunit,
                            pool_ptr, pool_size,
                            pool_alloc, pool_free,
                            pool_ctx);
}

int
soc_ltsw_ha_mem_cleanup(int unit)
{
    int dunit;
    if ((soc_cm_dev_num_get(unit, &dunit)) < 0) {
        return SHR_E_UNIT;
    }
    dunit += BCMDRD_CONFIG_MAX_UNITS;
    return shr_ha_unit_close(dunit);
}

int
soc_ltsw_ha_mem_reset(int unit)
{
    ha_mem_ctrl_t *hmc;
    int dunit;

    if ((soc_cm_dev_num_get(unit, &dunit)) < 0) {
        return SHR_E_UNIT;
    }
    dunit += BCMDRD_CONFIG_MAX_UNITS;

    if ((hmc = ha_mem_ctrl_get(unit)) == NULL) {
        return SHR_E_UNIT;
    }

    return shr_ha_unit_reset(dunit);
}

int
soc_ltsw_ha_mem_content_dump(int unit,
                             int (*write_f)(void *ctx, void *buf, size_t len),
                             void *ctx)
{
    int dunit;
    if ((soc_cm_dev_num_get(unit, &dunit)) < 0) {
        return SHR_E_UNIT;
    }
    dunit += BCMDRD_CONFIG_MAX_UNITS;
    return shr_ha_content_dump(dunit, write_f, ctx);
}

int
soc_ltsw_ha_mem_content_comp(int unit,
                             int (*read_f)(void *ctx, void *buf, size_t len),
                             void *ctx)
{
    int dunit;
    if ((soc_cm_dev_num_get(unit, &dunit)) < 0) {
        return SHR_E_UNIT;
    }
    dunit += BCMDRD_CONFIG_MAX_UNITS;
    return shr_ha_content_comp(dunit, read_f, ctx);
}

int
soc_ltsw_ha_mem_usage(int unit,
                      soc_ltsw_ha_mod_id_t comp_id,
                      uint32_t report_size,
                      soc_ltsw_ha_usage_report_t *reports)
{
    int dunit;
    if ((soc_cm_dev_num_get(unit, &dunit)) < 0) {
        return SHR_E_UNIT;
    }
    dunit += BCMDRD_CONFIG_MAX_UNITS;
    return shr_ha_comp_mem_usage(dunit, comp_id, report_size, reports);
}

void *
soc_ltsw_ha_mem_alloc(int unit,
                      soc_ltsw_ha_mod_id_t comp_id,
                      soc_ltsw_ha_sub_id_t sub_id,
                       const char *id_str, uint32_t *length)
{
    int dunit;
    if ((soc_cm_dev_num_get(unit, &dunit)) < 0) {
        return NULL;
    }
    dunit += BCMDRD_CONFIG_MAX_UNITS;
    return shr_ha_mem_alloc(dunit, comp_id, sub_id, id_str, length);
}

void *
soc_ltsw_ha_mem_realloc(int unit, void *mem, uint32_t length)
{
    int dunit;
    if ((soc_cm_dev_num_get(unit, &dunit)) < 0) {
        return NULL;
    }
    dunit += BCMDRD_CONFIG_MAX_UNITS;
    return shr_ha_mem_realloc(dunit, mem, length);
}

int
soc_ltsw_ha_mem_free(int unit, void *mem)
{
    int dunit;
    if ((soc_cm_dev_num_get(unit, &dunit)) < 0) {
        return SHR_E_UNIT;
    }
    dunit += BCMDRD_CONFIG_MAX_UNITS;
    return shr_ha_mem_free(dunit, mem);
}

int
soc_ltsw_ha_init(int unit, soc_ltsw_ha_info_t *ha_info)
{
    ha_ctrl_t *hc = &ha_ctrl[unit];

    if (hc->inited) {
        return SHR_E_NONE;
    }
    if (ha_info == NULL) {
        return SHR_E_PARAM;
    }

    hc->ha_info = ha_info;
    hc->inited = true;

    return SHR_E_NONE;
}

int
soc_ltsw_ha_cleanup(int unit)
{
    ha_ctrl_t *hc = &ha_ctrl[unit];

    if (!hc->inited) {
        return SHR_E_NONE;
    }
    hc->inited = false;
    hc->ha_info = NULL;

    return SHR_E_NONE;
}

int
soc_ltsw_ha_info_get(int unit, soc_ltsw_ha_info_t **ha_info)
{
    ha_ctrl_t *hc = &ha_ctrl[unit];

    if (!hc->inited) {
        return SHR_E_INIT;
    }

    *ha_info = hc->ha_info;

    return SHR_E_NONE;
}

