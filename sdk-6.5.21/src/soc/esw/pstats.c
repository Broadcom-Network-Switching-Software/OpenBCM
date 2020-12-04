/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Utility routines for Pktstats
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <sal/core/spl.h>
#include <sal/core/sync.h>
#include <sal/core/time.h>

#include <soc/defs.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/mem.h>

#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
#include <soc/trident3.h>
#include <soc/tomahawk.h>
#include <soc/tomahawk2.h>
#include <soc/pstats.h>
#ifdef BCM_TOMAHAWK3_SUPPORT
#include <soc/tomahawk3.h>
#endif

static sbusdma_desc_handle_t *pstats_tbl_handles[SOC_MAX_NUM_DEVICES] = {NULL};
static soc_pstats_tbl_ctrl_t **pstats_tbl_ctrl[SOC_MAX_NUM_DEVICES] = {NULL};

static void
soc_sbusdma_pstats_cb(int unit, int status, sbusdma_desc_handle_t handle, void *data)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_pstats_tbl_desc_t *tbl_desc = (soc_pstats_tbl_desc_t *)soc->pstats_tbl_desc;
    soc_pstats_tbl_ctrl_t *tbl_ctrl = pstats_tbl_ctrl[unit][PTR_TO_INT(data)];

    if (status != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "pstats sbusdma failed: blk: %d, index: %d, entries: %d, mode: %d.\n"),
                   tbl_ctrl->blk, tbl_ctrl->tindex, tbl_ctrl->entries, soc->pstats_mode));

        if (status == SOC_E_TIMEOUT) {
#ifdef BCM_TRIDENT3_SUPPORT
            /* Lockup recovery for MOR */
            if (SOC_IS_TRIDENT3X(unit) && tbl_desc[tbl_ctrl->tindex].mor_dma) {
                soc_trident3_mmu_splitter_reset(unit);
            }
#endif
#ifdef BCM_TOMAHAWK2_SUPPORT
            /* Lockup recovery for MOR */
            if (SOC_IS_TOMAHAWK2(unit) && tbl_desc[tbl_ctrl->tindex].mor_dma) {
                soc_tomahawk2_mmu_splitter_reset(unit);
            }
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit) && tbl_desc[tbl_ctrl->tindex].mor_dma) {
                soc_tomahawk3_mmu_splitter_reset(unit);
            }
#endif
        }
    }

    if (soc->pstats_mode == PSTATS_MODE_PKT_SYNC) {
        sal_sem_give(tbl_ctrl->dma_done);
    }
}

static void
soc_sbusdma_pstats_pc(int unit, sbusdma_desc_handle_t handle, void *data)
{
#ifdef BCM_TRIDENT3_SUPPORT
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_pstats_tbl_desc_t *tbl_desc = (soc_pstats_tbl_desc_t *)soc->pstats_tbl_desc;
    soc_pstats_tbl_ctrl_t *tbl_ctrl = pstats_tbl_ctrl[unit][PTR_TO_INT(data)];

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "pstats sbusdma preconfig: blk: %d, index: %d, entries: %d, mode: %d.\n"),
                 tbl_ctrl->blk, tbl_ctrl->tindex, tbl_ctrl->entries, soc->pstats_mode));

    if (SOC_IS_TRIDENT3X(unit) && tbl_desc[tbl_ctrl->tindex].mor_dma) {
        soc_trident3_mmu_pstats_mor_enable(unit);
    }
#endif
}

static int
soc_pstats_sbusdma_desc_setup(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_pstats_tbl_desc_t *tbl_desc;
    soc_pstats_tbl_ctrl_t *tbl_ctrl;
    uint8 *buff = soc->pstats_tbl_buf;
    soc_sbusdma_desc_ctrl_t ctrl = {0};
    soc_sbusdma_desc_cfg_t *cfg;
    uint8 at;
    uint32 size;
    int ti, mi, i;
    int rv;

    if (!soc->pstats_tbl_desc ||
        !soc->pstats_tbl_desc_count ||
        !pstats_tbl_handles[unit]) {
        return SOC_E_INIT;
    }

    tbl_desc = (soc_pstats_tbl_desc_t *)soc->pstats_tbl_desc;

    pstats_tbl_ctrl[unit] = sal_alloc(soc->pstats_tbl_desc_count *
                                      sizeof(soc_pstats_tbl_ctrl_t *),
                                      "pstats_tbl_ctrl_p");
    if (pstats_tbl_ctrl[unit] == NULL) {
        return SOC_E_MEMORY;
    }

    for (ti = 0; ti < soc->pstats_tbl_desc_count; ti++) {
        mi = 0;
        while (tbl_desc[ti].desc[mi].mem != INVALIDm) {
            mi++;
        }
        if (!mi) {
            continue;
        }

        cfg = sal_alloc(mi * sizeof(soc_sbusdma_desc_cfg_t),
                        "pstats_sbusdma_desc_cfg");
        if (cfg == NULL) {
            return SOC_E_MEMORY;
        }
        sal_memset(cfg, 0, mi * sizeof(soc_sbusdma_desc_cfg_t));

        ctrl.flags = SOC_SBUSDMA_CFG_COUNTER_IS_MEM | SOC_SBUSDMA_CFG_USE_FLAGS;
        if (tbl_desc[ti].mor_dma) {
            if (soc->pstats_flags & (_TD3_MMU_PSTATS_MOR_EN | _TH2_MMU_PSTATS_MOR_EN)) {
                ctrl.flags |= SOC_SBUSDMA_CFG_MOR;
            }
            ctrl.pc = soc_sbusdma_pstats_pc;
            ctrl.pc_data = INT_TO_PTR(ti);
        } else {
            ctrl.pc = NULL;
            ctrl.pc_data = NULL;
        }
        ctrl.cfg_count = mi;
        ctrl.buff = buff;
        ctrl.cb = soc_sbusdma_pstats_cb;
        ctrl.data = INT_TO_PTR(ti);
#define PSTATS_TBLS "PSTATS TBLS"
        sal_strlcpy(ctrl.name, PSTATS_TBLS, sizeof(ctrl.name));

        tbl_ctrl = sal_alloc(sizeof(soc_pstats_tbl_ctrl_t), "pstats_tbl_ctrl");
        if (tbl_ctrl == NULL) {
            sal_free(cfg);
            return SOC_E_MEMORY;
        }
        tbl_ctrl->blk = tbl_desc[ti].blk;
        tbl_ctrl->tindex = ti;
        tbl_ctrl->entries = mi;
        tbl_ctrl->buff = buff;
        tbl_ctrl->size = 0;
        tbl_ctrl->flags = ctrl.flags;
        pstats_tbl_ctrl[unit][ti] = tbl_ctrl;

        for (i = 0; i < mi; i++) {
            cfg[i].flags = ctrl.flags;
            cfg[i].blk = SOC_MEM_BLOCK_ANY(unit, tbl_desc[ti].desc[i].mem);
            cfg[i].addr = soc_mem_addr_get(unit, tbl_desc[ti].desc[i].mem,
                                           0, cfg[i].blk, 0, &at);
            cfg[i].acc_type = at;
            cfg[i].blk = SOC_BLOCK2SCH(unit, cfg[i].blk);
            cfg[i].width = tbl_desc[ti].desc[i].width;
            cfg[i].count = tbl_desc[ti].desc[i].entries;
            cfg[i].addr_shift = tbl_desc[ti].desc[i].shift;
            size = cfg[i].count * cfg[i].width * 4;
            buff += size;
            tbl_ctrl->size += size;
            if (tbl_desc[ti].desc[i].mor_dma &&
                (!soc_property_get(unit, "pstats_mor_disable", 0))) {
                cfg[i].flags |= SOC_SBUSDMA_CFG_MOR;
            }
        }

        rv = soc_sbusdma_desc_create(unit, &ctrl, cfg, TRUE,
                                     &pstats_tbl_handles[unit][ti]);
        sal_free(cfg);
        if (rv != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "pstats sbusdma desc creation error for table: %d\n"),
                       ti));
            return rv;
        }
        tbl_ctrl->dma_done = sal_sem_create("pstats_sbusdma_done", sal_sem_BINARY, 0);
        if (tbl_ctrl->dma_done == NULL) {
            return SOC_E_MEMORY;
        }
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "pstats handle: %d for table: %d\n"),
                     pstats_tbl_handles[unit][ti], ti));
    }

    return SOC_E_NONE;
}

static int
soc_pstats_sbudma_desc_free(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    uint32 ti;
    int rv;

    if (!soc->pstats_tbl_desc ||
        !soc->pstats_tbl_desc_count ||
        !pstats_tbl_handles[unit]) {
        return SOC_E_INIT;
    }

    for (ti = 0; ti < soc->pstats_tbl_desc_count; ti++) {
        if (pstats_tbl_handles[unit][ti]) {
            do {
                rv = soc_sbusdma_desc_delete(unit, pstats_tbl_handles[unit][ti]);
                if (rv == SOC_E_BUSY) {
                    sal_usleep(SAL_BOOT_QUICKTURN ? 10000 : 10);
                }
            } while (rv == SOC_E_BUSY);
            if (rv != SOC_E_NONE) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "pstats sbusdma desc delete error for table: %d\n"),
                           ti));
                return rv;
            }
            pstats_tbl_handles[unit][ti] = 0;
            if (pstats_tbl_ctrl[unit][ti]) {
                if (pstats_tbl_ctrl[unit][ti]->dma_done) {
                    sal_sem_destroy(pstats_tbl_ctrl[unit][ti]->dma_done);
                    pstats_tbl_ctrl[unit][ti]->dma_done = NULL;
                }
                sal_free(pstats_tbl_ctrl[unit][ti]);
                pstats_tbl_ctrl[unit][ti] = NULL;
            }
        }
    }
    if (pstats_tbl_ctrl[unit]) {
        sal_free(pstats_tbl_ctrl[unit]);
        pstats_tbl_ctrl[unit] = NULL;
    }

    return SOC_E_NONE;
}

int
soc_pstats_init(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_pstats_tbl_desc_t *tbl_desc;
    uint32 entries, width, size = 0;
    int ti, mi;
    int rv = SOC_E_INIT;

    if (!soc->pstats_tbl_desc || !soc->pstats_tbl_desc_count) {
        return rv;
    }

    tbl_desc = (soc_pstats_tbl_desc_t *)soc->pstats_tbl_desc;

    for (ti = 0; ti < soc->pstats_tbl_desc_count; ti++) {
        mi = 0;
        while (tbl_desc[ti].desc[mi].mem != INVALIDm) {
            entries = tbl_desc[ti].desc[mi].entries;
            width = tbl_desc[ti].desc[mi].width;
            size += entries * width * 4;
            mi++;
        }
    }
    soc->pstats_tbl_size = size;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Total pstats tbls: %d, Total size: %d\n"),
                 soc->pstats_tbl_desc_count, size));

    if (pstats_tbl_handles[unit] == NULL) {
        pstats_tbl_handles[unit] = sal_alloc(soc->pstats_tbl_desc_count *
                                             sizeof(sbusdma_desc_handle_t),
                                             "pstats_tbl_handles");
        if (pstats_tbl_handles[unit] == NULL) {
            goto error;
        }
    }
    sal_memset(pstats_tbl_handles[unit], 0,
               soc->pstats_tbl_desc_count * sizeof(sbusdma_desc_handle_t));

    if (soc->pstats_tbl_buf == NULL) {
        soc->pstats_tbl_buf = soc_cm_salloc(unit, size, "pstats_tbl_buf");
        if (soc->pstats_tbl_buf == NULL) {
            goto error;
        }
    }
    sal_memset(soc->pstats_tbl_buf, 0, size);

    if (soc->pstatsMutex == NULL) {
        soc->pstatsMutex = sal_mutex_create("pstats");
        if (soc->pstatsMutex == NULL) {
            goto error;
        }
    }

    rv = soc_pstats_sbusdma_desc_setup(unit);
    if (rv != SOC_E_NONE) {
        goto error;
    }

    return SOC_E_NONE;

error:
    if (soc->pstatsMutex) {
        sal_mutex_destroy(soc->pstatsMutex);
        soc->pstatsMutex = NULL;
    }

    if (soc->pstats_tbl_buf) {
        soc_cm_sfree(unit, soc->pstats_tbl_buf);
        soc->pstats_tbl_buf = NULL;
    }

    if (pstats_tbl_handles[unit]) {
        sal_free(pstats_tbl_handles[unit]);
        pstats_tbl_handles[unit] = NULL;
    }

    return rv;
}

int
soc_pstats_deinit(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int rv;

    rv = soc_pstats_sbudma_desc_free(unit);
    if (rv != SOC_E_NONE) {
        return rv;
    }

    if (pstats_tbl_handles[unit]) {
        sal_free(pstats_tbl_handles[unit]);
        pstats_tbl_handles[unit] = NULL;
    }

    if (soc->pstats_tbl_buf) {
        soc_cm_sfree(unit, soc->pstats_tbl_buf);
        soc->pstats_tbl_buf = NULL;
    }

    if (soc->pstatsMutex) {
        sal_mutex_destroy(soc->pstatsMutex);
        soc->pstatsMutex = NULL;
    }

    return SOC_E_NONE;
}

int
soc_pstats_tbl_pattern_get(int unit, soc_pstats_tbl_desc_t *tdesc,
                           soc_mem_t *mem, uint32 *index, uint32 *count)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_pstats_tbl_desc_t *tbl_desc;
    soc_pstats_tbl_ctrl_t *tbl_ctrl;
    uint32 ti;

    if (!soc->pstats_tbl_desc ||
        !soc->pstats_tbl_desc_count ||
        !pstats_tbl_ctrl[unit]) {
        return SOC_E_INIT;
    }

    tbl_desc = (soc_pstats_tbl_desc_t *)soc->pstats_tbl_desc;

    for (ti = 0; ti < soc->pstats_tbl_desc_count; ti++) {
        if (!pstats_tbl_ctrl[unit][ti]) {
            continue;
        }
        tbl_ctrl = pstats_tbl_ctrl[unit][ti];
        if (&tbl_desc[tbl_ctrl->tindex] == tdesc) {
            break;
        }
    }
    if (ti >= soc->pstats_tbl_desc_count || !mem || !index || !count) {
        return SOC_E_PARAM;
    }

    SOC_PSTATS_LOCK(unit);

    if (tdesc->pattern_set) {
        *mem = tdesc->mem;
        *index = tdesc->index;
        *count = tdesc->count;
    } else {
        *mem = MEM_BLOCK_ANY;
        *index = 0;
        *count = tdesc->desc[0].entries;
    }

    SOC_PSTATS_UNLOCK(unit);
    return SOC_E_NONE;
}

int
soc_pstats_tbl_pattern_set(int unit, soc_pstats_tbl_desc_t *tdesc,
                           soc_mem_t mem, uint32 index, uint32 count)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_pstats_tbl_desc_t *tbl_desc;
    soc_pstats_tbl_ctrl_t *tbl_ctrl = NULL;
    soc_sbusdma_desc_ctrl_t ctrl = {0};
    soc_sbusdma_desc_cfg_t *cfg;
    uint8 at;
    uint32 size = 0;
    uint32 ti, mi, cnt, i, j;
    int rv;

    if (!soc->pstats_tbl_desc ||
        !soc->pstats_tbl_desc_count ||
        !pstats_tbl_ctrl[unit]) {
        return SOC_E_INIT;
    }

    tbl_desc = (soc_pstats_tbl_desc_t *)soc->pstats_tbl_desc;

    for (ti = 0; ti < soc->pstats_tbl_desc_count; ti++) {
        if (!pstats_tbl_ctrl[unit][ti]) {
            continue;
        }
        tbl_ctrl = pstats_tbl_ctrl[unit][ti];
        if (&tbl_desc[tbl_ctrl->tindex] == tdesc) {
            break;
        }
    }
    if (ti >= soc->pstats_tbl_desc_count) {
        return SOC_E_PARAM;
    }

    SOC_PSTATS_LOCK(unit);

    mi = 0;
    if (mem == MEM_BLOCK_ANY) {
        while (tdesc->desc[mi].mem != INVALIDm) {
            mi++;
        }
        if (!mi) {
            return SOC_E_PARAM;
        }
        cnt = mi;
    } else {
        while (tdesc->desc[mi].mem != INVALIDm) {
            if (tdesc->desc[mi].mem == mem) {
                break;
            }
            mi++;
        }
        if (tdesc->desc[mi].mem == INVALIDm ||
            count == 0 ||
            (index + count) > tdesc->desc[mi].entries) {
            return SOC_E_PARAM;
        }
        cnt = 1;
    }

    cfg = sal_alloc(cnt * sizeof(soc_sbusdma_desc_cfg_t), "pstats_sbusdma_desc_cfg");
    if (cfg == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(cfg, 0, cnt * sizeof(soc_sbusdma_desc_cfg_t));

    ctrl.flags = SOC_SBUSDMA_CFG_COUNTER_IS_MEM;
    if (tdesc->mor_dma) {
        if (soc->pstats_flags & (_TD3_MMU_PSTATS_MOR_EN | _TH2_MMU_PSTATS_MOR_EN)) {
            ctrl.flags |= SOC_SBUSDMA_CFG_MOR;
        }
        ctrl.pc = soc_sbusdma_pstats_pc;
        ctrl.pc_data = INT_TO_PTR(ti);
    }
    ctrl.cfg_count = cnt;
    ctrl.buff = tbl_ctrl->buff;
    ctrl.cb = soc_sbusdma_pstats_cb;
    ctrl.data = INT_TO_PTR(ti);

    for (i = 0; i < cnt; i++) {
        j = mem == MEM_BLOCK_ANY ? i : mi;
        cfg[i].blk = SOC_MEM_BLOCK_ANY(unit, tdesc->desc[j].mem);
        cfg[i].addr = soc_mem_addr_get(unit, tdesc->desc[j].mem,
                                       0, cfg[i].blk, index, &at);
        cfg[i].acc_type = at;
        cfg[i].blk = SOC_BLOCK2SCH(unit, cfg[i].blk);
        cfg[i].width = tdesc->desc[j].width;
        cfg[i].count = count;
        cfg[i].addr_shift = tdesc->desc[j].shift;
        size += cfg[i].count * cfg[i].width * 4;
    }

    if (pstats_tbl_handles[unit][ti]) {
        i = 0;
        do {
            rv = soc_sbusdma_desc_delete(unit, pstats_tbl_handles[unit][ti]);
            if (rv == SOC_E_BUSY) {
                sal_usleep(SAL_BOOT_QUICKTURN ? 10000 : 10);
            }
        } while (rv == SOC_E_BUSY && i++ < 100);
        if (rv != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "pstats sbusdma desc delete error for talble: %d\n"),
                       ti));
            sal_free(cfg);
            SOC_PSTATS_UNLOCK(unit);
            return rv;
        }
    }

    rv = soc_sbusdma_desc_create(unit, &ctrl, cfg, TRUE,
                                 &pstats_tbl_handles[unit][ti]);
    sal_free(cfg);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "pstats sbusdma desc creation error for table: %d\n"),
                   ti));
        SOC_PSTATS_UNLOCK(unit);
        return rv;
    }

    tbl_ctrl->size = size;
    tbl_ctrl->flags = ctrl.flags;
    tdesc->pattern_set = 1;
    tdesc->mem = mem;
    tdesc->index = index;
    tdesc->count = count;

    SOC_PSTATS_UNLOCK(unit);
    return SOC_E_NONE;
}

int
soc_pstats_sync(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_pstats_tbl_desc_t *tbl_desc;
    soc_pstats_tbl_ctrl_t *tbl_ctrl;
    sbusdma_desc_handle_t desc_handle;
    soc_mem_t mem;
    uint32 index;
    uint32 count;
    uint32 ti;
    soc_timeout_t to;
    sal_usecs_t dma_timeout = 1000000;
    sal_usecs_t stime, etime;
    int rv;

    if (!soc->pstats_tbl_desc ||
        !soc->pstats_tbl_desc_count ||
        !pstats_tbl_ctrl[unit]) {
        return SOC_E_INIT;
    }

    SOC_PSTATS_LOCK(unit);

    if (soc->pstats_mode != PSTATS_MODE_PKT_BUFF &&
        soc->pstats_mode != PSTATS_MODE_PKT_SYNC) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "pstats is not in packetized stats mode.\n")));
        SOC_PSTATS_UNLOCK(unit);
        return SOC_E_INTERNAL;
    }

    tbl_desc = (soc_pstats_tbl_desc_t *)soc->pstats_tbl_desc;

    for (ti = 0; ti < soc->pstats_tbl_desc_count; ti++) {
        tbl_ctrl = pstats_tbl_ctrl[unit][ti];
        if (!tbl_desc[ti].mor_dma ||
            !!(soc->pstats_flags & (_TD3_MMU_PSTATS_MOR_EN | _TH2_MMU_PSTATS_MOR_EN)) ==
            !!(tbl_ctrl->flags & SOC_SBUSDMA_CFG_MOR)) {
            continue;
        }
        if ((rv = soc_pstats_tbl_pattern_get(unit, &tbl_desc[ti], &mem, &index, &count)) ||
            (rv = soc_pstats_tbl_pattern_set(unit, &tbl_desc[ti], mem, index, count))) {
            SOC_PSTATS_UNLOCK(unit);
            return rv;
        }
    }

    for (ti = 0; ti < soc->pstats_tbl_desc_count; ti++) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit, "pstats tbl %d\n"), ti));
        tbl_ctrl = pstats_tbl_ctrl[unit][ti];
        desc_handle = pstats_tbl_handles[unit][ti];
        if (tbl_ctrl && desc_handle) {
            soc_timeout_init(&to, 2 * dma_timeout, 0);
            do {
                stime = sal_time_usecs();
                rv = soc_sbusdma_desc_start(unit, desc_handle);
                if (rv == SOC_E_NONE) {
                    break;
                } else if (rv == SOC_E_BUSY) {
                    sal_usleep(SAL_BOOT_QUICKTURN ? 10000 : 10);
                } else {
                    SOC_PSTATS_UNLOCK(unit);
                    return SOC_E_INTERNAL;
                }
                if (soc_timeout_check(&to)) {
                    LOG_WARN(BSL_LS_SOC_COMMON,
                             (BSL_META_U(unit,
                                         "pstats start dma timeout\n")));
                    SOC_PSTATS_UNLOCK(unit);
                    return SOC_E_TIMEOUT;
                }
            } while (rv == SOC_E_BUSY);
            if (soc->pstats_mode == PSTATS_MODE_PKT_SYNC) {
                rv = sal_sem_take(tbl_ctrl->dma_done, dma_timeout);
                if (rv) {
                    LOG_WARN(BSL_LS_SOC_COMMON,
                             (BSL_META_U(unit,
                                         "pstats read operation timeout\n")));
                    SOC_PSTATS_UNLOCK(unit);
                    return SOC_E_TIMEOUT;
                }
            }
            etime = sal_time_usecs();
            LOG_INFO(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "soc_pstats_sync: unit=%d tnum-%d mode(%s) done in %d usec\n"),
                      unit, ti,
                      tbl_desc[ti].mor_dma && (soc->pstats_flags & (_TD3_MMU_PSTATS_MOR_EN | _TH2_MMU_PSTATS_MOR_EN)) ? "MOR" : "Non-MOR",
                      SAL_USECS_SUB(etime, stime)));
        }
    }

    SOC_PSTATS_UNLOCK(unit);
    return SOC_E_NONE;
}

int
soc_pstats_mem_get(int unit, soc_mem_t mem, uint8 *buf, int try_num)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_pstats_tbl_desc_t *tbl_desc;
    soc_pstats_tbl_ctrl_t *tbl_ctrl = NULL;
    soc_pstats_mem_desc_t *mem_desc;
    soc_mem_t tmem;
    uint32 index, count;
    uint32 ti, ei;
    uint32 off = 0, size = 0, try = 0;
    int rv;

    if (!soc->pstats_tbl_desc ||
        !soc->pstats_tbl_desc_count ||
        !pstats_tbl_ctrl[unit]) {
        return SOC_E_INIT;
    }

    tbl_desc = (soc_pstats_tbl_desc_t *)soc->pstats_tbl_desc;

    for (ti = 0; ti < soc->pstats_tbl_desc_count; ti++) {
        if (!pstats_tbl_ctrl[unit][ti]) {
            continue;
        }
        tbl_ctrl = pstats_tbl_ctrl[unit][ti];
        rv = soc_pstats_tbl_pattern_get(unit, &tbl_desc[ti],
                                        &tmem, &index, &count);
        if (rv != SOC_E_NONE) {
            return rv;
        }
        off = 0;
        size = 0;
        for (ei = 0; ei < tbl_ctrl->entries; ei++) {
            mem_desc = &tbl_desc[ti].desc[ei];
            size = mem_desc->width * 4 *
                   (tbl_desc[ti].pattern_set ? count : mem_desc->entries);
            if (mem_desc->mem == mem) {
                if (try == try_num) {
                    break;
                } else {
                    try++;
                }
            }
            off += size;
        }
        if (ei < tbl_ctrl->entries) {
            if (tbl_desc[ti].pattern_set && tmem != MEM_BLOCK_ANY) {
                if (tmem != mem) {
                    return SOC_E_PARAM;
                }
                off = 0;
            }
            break;
        }
    }
    if (ti >= soc->pstats_tbl_desc_count || !buf) {
        return SOC_E_PARAM;
    }

    SOC_PSTATS_LOCK(unit);
    sal_memcpy(buf, tbl_ctrl->buff + off, size);
    SOC_PSTATS_UNLOCK(unit);
    return SOC_E_NONE;
}
#endif /* !BCM_TRIDENT3_SUPPORT !BCM_TOMAHAWK2_SUPPORT !BCM_TOMAHAWK3_SUPPORT*/

