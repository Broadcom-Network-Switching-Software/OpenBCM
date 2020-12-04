/*! \file policer.c
 *
 * BCM level APIs for Policer.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/types.h>
#include <shr/shr_bitop.h>
#include <bcm_int/control.h>
#include <bcm/policer.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/issu.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/field.h>

#include <bcm_int/ltsw/init.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/policer_int.h>
#include <bcm_int/ltsw/mbcm/policer.h>
#include <bcmltd/chip/bcmltd_str.h>

#include <shr/shr_debug.h>
#include <sal/sal_mutex.h>

#define BSL_LOG_MODULE BSL_LS_BCM_POLICER

/* Policer database. */
typedef struct ltsw_policer_info_s {
    /* Policer module initialized flag. */
    int initialized;

    /* Policer module lock. */
    sal_mutex_t lock;

    /* Maximum number of meter pools available. */
    int         max_ing_pools;

    /* Maximum number of meter pools available. */
    int         max_egr_pools;

    /* Flags for ingress stage. */
    uint32_t    ing_stage_flags;

    /* Flags for egress stage. */
    uint32_t    egr_stage_flags;

    /* Group priority ID for each pool. */
    bcmint_policer_pool_state_t *pool_state_ing;

    /* Group priority ID for each pool. */
    bcmint_policer_pool_state_t *pool_state_egr;

    /*! List of ingress policer entries */
    bcmint_policer_entry_t *ing_entries;

    /*! List of egress policer entries */
    bcmint_policer_entry_t *egr_entries;

    /*! State of ingress policer. */
    bcmint_policer_state_t *ing_state;

    /*! State of egress policer. */
    bcmint_policer_state_t *egr_state;
} ltsw_policer_info_t;

static ltsw_policer_info_t ltsw_policer_info[BCM_MAX_NUM_UNITS];

#define POLICER_INFO(unit) (&ltsw_policer_info[unit])

#define POLICER_INIT_CHECK(unit) \
    do { \
        if (POLICER_INFO(unit)->initialized == 0) { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        } \
    } while(0)

#define POLICER_LOCK(unit) \
    do { \
        if (POLICER_INFO(unit)->lock) { \
            sal_mutex_take(POLICER_INFO(unit)->lock, SAL_MUTEX_FOREVER); \
        } else { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        } \
    } while(0)

#define POLICER_UNLOCK(unit) \
    do { \
        if (POLICER_INFO(unit)->lock) { \
            sal_mutex_give(POLICER_INFO(unit)->lock); \
        } \
    } while(0)

/******************************************************************************
 * Private functions
 */
static int
bcmint_ltsw_policer_entries_per_pool(
    int unit,
    bcmi_ltsw_policer_stage_t pol_stage,
    int *num_entries)
{
    int dunit = 0;
    uint64_t val = 0;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    if (pol_stage == bcmiPolicerStageEgress) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, METER_EGR_FP_DEVICE_INFOs, &entry_hdl));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, METER_ING_FP_DEVICE_INFOs, &entry_hdl));
    }
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL), SHR_E_NOT_FOUND);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, NUM_METERS_PER_POOLs, &val));

    *num_entries = val;

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
bcmint_ltsw_policer_entries_per_device(
    int unit,
    bcmi_ltsw_policer_stage_t pol_stage,
    int *num_entries)
{
    int dunit = 0;
    uint64_t val = 0;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    if (pol_stage == bcmiPolicerStageEgress) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, METER_EGR_FP_DEVICE_INFOs, &entry_hdl));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, METER_ING_FP_DEVICE_INFOs, &entry_hdl));
    }
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL), SHR_E_NOT_FOUND);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, NUM_METERSs, &val));

    *num_entries = val;

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/* Check if the policer_id exists. */
static int
bcmint_ltsw_policer_id_validate(
    int unit,
    bcmi_ltsw_policer_stage_t pol_stage,
    uint32_t pid,
    bool *exists)
{
    bcmint_policer_entry_t *ptr = NULL;

    SHR_FUNC_ENTER(unit);

    if (pol_stage == bcmiPolicerStageEgress) {
        ptr = &(POLICER_INFO(unit)->egr_entries[pid]);
    } else {
        ptr = &(POLICER_INFO(unit)->ing_entries[pid]);
    }

    *exists = ptr->enabled;

    SHR_FUNC_EXIT();
}

/* Allocate a free policer_id. */
static int
bcmint_ltsw_policer_id_alloc(
    int unit,
    bcmi_ltsw_policer_stage_t pol_stage,
    uint32_t *pid)
{
    int num_meters = 0, idx = 0;
    bcmint_policer_entry_t *ptr = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_entries_per_device(unit,
                                                pol_stage,
                                                &num_meters));
    if (*pid != 0) {
        if (*pid > num_meters) {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }
        /* If pid is specified, mark it used. */
        if (pol_stage == bcmiPolicerStageEgress) {
            ptr = &(POLICER_INFO(unit)->egr_entries[*pid]);
        } else {
            ptr = &(POLICER_INFO(unit)->ing_entries[*pid]);
        }
        ptr->enabled = 1;
    } else {
        /* If pid is not specified, allocate a free id */
        for (idx = 1; idx <= num_meters; idx++) {
            if (pol_stage == bcmiPolicerStageEgress) {
                ptr = &(POLICER_INFO(unit)->egr_entries[idx]);
            } else {
                ptr = &(POLICER_INFO(unit)->ing_entries[idx]);
            }
            if (ptr->enabled == 0) {
                *pid = idx;
                ptr->enabled = 1;
                break;
            }
        }

        if (idx > num_meters) {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/* Update the s/w state with the policer entry. */
static int
bcmint_ltsw_policer_entry_insert(
    int unit,
    bcmi_ltsw_policer_stage_t pol_stage,
    bcm_policer_config_t *pol_cfg,
    uint32_t pid)
{
    bcmint_policer_entry_t *ptr = NULL;

    SHR_FUNC_ENTER(unit);

    if (pol_stage == bcmiPolicerStageEgress) {
        ptr = &(POLICER_INFO(unit)->egr_entries[pid]);
        ptr->policer_id = (pid | BCM_FIELD_EFP_ID_BASE);
    } else {
        ptr = &(POLICER_INFO(unit)->ing_entries[pid]);
        ptr->policer_id = (pid | BCM_FIELD_IFP_ID_BASE);
    }

    ptr->pol_cfg.flags = pol_cfg->flags;
    if (pol_cfg->flags & BCM_POLICER_WITH_SHARED_POOL_ID) {
        ptr->pool_id = pol_cfg->pool_id;
    }
    ptr->pol_cfg.mode  = pol_cfg->mode;
    ptr->pol_cfg.ckbits_sec = pol_cfg->ckbits_sec;
    ptr->pol_cfg.cbits_sec_lower = pol_cfg->cbits_sec_lower;
    ptr->pol_cfg.ckbits_burst = pol_cfg->ckbits_burst;
    ptr->pol_cfg.cbits_burst_lower = pol_cfg->cbits_burst_lower;
    ptr->pol_cfg.pkbits_sec = pol_cfg->pkbits_sec;
    ptr->pol_cfg.pbits_sec_lower = pol_cfg->pbits_sec_lower;
    ptr->pol_cfg.pkbits_burst = pol_cfg->pkbits_burst;
    ptr->pol_cfg.pbits_burst_lower = pol_cfg->pbits_burst_lower;
    ptr->pol_cfg.actual_ckbits_sec = 0;
    ptr->pol_cfg.actual_cbits_sec_lower = 0;
    ptr->pol_cfg.actual_ckbits_burst = 0;
    ptr->pol_cfg.actual_cbits_burst_lower = 0;
    ptr->pol_cfg.actual_pkbits_sec = 0;
    ptr->pol_cfg.actual_pbits_sec_lower = 0;
    ptr->pol_cfg.actual_pkbits_burst = 0;
    ptr->pol_cfg.actual_pbits_burst_lower = 0;

    SHR_FUNC_EXIT();
}

/* Fetch the policer entry from s/w state. */
static int
bcmint_ltsw_policer_entry_get(
    int unit,
    bcmi_ltsw_policer_stage_t pol_stage,
    uint32_t pid,
    bcmint_policer_entry_t **pol_entry)
{
    bcmint_policer_entry_t *ptr = NULL;

    SHR_FUNC_ENTER(unit);

    if (pol_stage == bcmiPolicerStageEgress) {
        ptr = &(POLICER_INFO(unit)->egr_entries[pid]);
    } else {
        ptr = &(POLICER_INFO(unit)->ing_entries[pid]);
    }

    *pol_entry = ptr;

    SHR_FUNC_EXIT();
}

/* Delete the entry from s/w state and free the index. */
static int
bcmint_ltsw_policer_entry_delete(
    int unit,
    bcmi_ltsw_policer_stage_t pol_stage,
    uint32_t pid)
{
    bcmint_policer_entry_t *ptr = NULL;

    SHR_FUNC_ENTER(unit);

    if (pol_stage == bcmiPolicerStageEgress) {
        ptr = &(POLICER_INFO(unit)->egr_entries[pid]);
    } else {
        ptr = &(POLICER_INFO(unit)->ing_entries[pid]);
    }

    sal_memset(ptr, 0, sizeof(bcmint_policer_entry_t));

    SHR_FUNC_EXIT();
}

static int
bcmint_ltsw_policer_pool_alloc(
    int unit,
    int stage,
    bcmi_ltsw_policer_stage_t pol_stage,
    bool shared,
    int pipe,
    int prio,
    int *pool_id)
{
    bcmint_policer_pool_state_t *ptr = NULL;
    bcmint_policer_pool_state_t *pool_ptr = NULL;
    bcmint_policer_state_t *state_ptr = NULL;
    int idx = 0, max_pools = 0, start_idx = 0, end_idx = 0;
    int num_entries = 0;

    SHR_FUNC_ENTER(unit);

    if (pol_stage == bcmiPolicerStageEgress) {
        max_pools = POLICER_INFO(unit)->max_egr_pools;
        pool_ptr = POLICER_INFO(unit)->pool_state_egr;
        state_ptr =  POLICER_INFO(unit)->egr_state;
    } else {
        max_pools = POLICER_INFO(unit)->max_ing_pools;
        pool_ptr = POLICER_INFO(unit)->pool_state_ing;
        state_ptr =  POLICER_INFO(unit)->ing_state;
    }

    /* If pool is shared, then pool_id has to be specified. */
    if ((shared == true) && (*pool_id == -1)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_entries_per_pool(unit,
                                              pol_stage,
                                              &num_entries));

    /* Pool ID is not specified by user. */
    if (*pool_id == -1) {
        if (state_ptr->flags & BCMI_LTSW_POLICER_ALLOC_MODE_USER) {
            /*
             * If alloc mode for this policer stage is user
             * controlled, then pool_id has to be specified
             * for this group also.
             */
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        /* Set pool alloc mode as Dynamic. */
        state_ptr->flags |= BCMI_LTSW_POLICER_ALLOC_MODE_DYNAMIC;

        /* Pool id is not passed from FP. Allocate a free pool. */
        if (pipe == -1) {
            start_idx = 0;
            end_idx = max_pools;
        } else {
            start_idx = (pipe * max_pools);
            end_idx = ((pipe + 1) * max_pools);
        }

        for (idx = start_idx; idx < end_idx; idx++) {
            ptr = &pool_ptr[idx];
            /*
             * Pools with mode as 1 cannot be allocated, have to be
             * specified from FP group.
             */
            if (ptr->mode == BCMI_LTSW_POLICER_POOL_MODE_WITH_ID) {
                continue;
            }
            /* This pool is free. */
            if (ptr->ref_count == 0) {
                ptr->priority = prio;
                ptr->ref_count++;
                *pool_id = (idx % max_pools);
                state_ptr->pools_in_use++;
                break;
            } else {
                /*
                 * EM and IFP share same policer pools. Check if the pool
                 * is in use by IFP or EM.
                 */
                if ((stage == bcmiFieldStageIngress &&
                     ptr->pool_in_use == BCMI_LTSW_POLICER_POOL_USED_BY_EM) ||
                    (stage == bcmiFieldStageExactMatch &&
                     ptr->pool_in_use == BCMI_LTSW_POLICER_POOL_USED_BY_IFP)) {
                    continue;
                }

                /*
                 * The pool is not free but is being used by same priority.
                 * or has been auto-expanded for another group.
                 */
                if ((ptr->priority == prio) ||
                    !(ptr->mode & BCMI_LTSW_POLICER_POOL_MODE_FIXED)) {
                    ptr->priority = prio;
                    ptr->ref_count++;
                    *pool_id = (idx % max_pools);
                    break;
                }
            }
        }
        if (idx == end_idx) {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }
    } else {
        if (state_ptr->flags & BCMI_LTSW_POLICER_ALLOC_MODE_DYNAMIC) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        state_ptr->flags |= BCMI_LTSW_POLICER_ALLOC_MODE_USER;

        /* Pool ID passed from FP. */
        if (*pool_id >= max_pools) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (pipe == -1) {
            idx = *pool_id;
        } else {
            idx = ((pipe * max_pools) + *pool_id);
        }
        ptr = &pool_ptr[idx];
        if (ptr->ref_count == 0) {
            ptr->priority = prio;
            ptr->ref_count++;
            if (shared) {
                ptr->mode |= BCMI_LTSW_POLICER_POOL_MODE_SHARED;
            }
            state_ptr->pools_in_use++;
        } else {
            if ((stage == bcmiFieldStageIngress &&
                 ptr->pool_in_use == BCMI_LTSW_POLICER_POOL_USED_BY_EM) ||
                (stage == bcmiFieldStageExactMatch &&
                 ptr->pool_in_use == BCMI_LTSW_POLICER_POOL_USED_BY_IFP)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            /*
             * The pool is not free but can be allocated because
             *  - group priority is same (or)
             *  - pool is in shared mode
             */
            if ((ptr->priority == prio) ||
                ((ptr->mode & BCMI_LTSW_POLICER_POOL_MODE_SHARED) &&
                 shared)) {
                ptr->ref_count++;
            } else {
                SHR_ERR_EXIT(SHR_E_RESOURCE);
            }
        }
    }

    ptr->mode |= BCMI_LTSW_POLICER_POOL_MODE_FIXED;
    if (pol_stage == bcmiPolicerStageIngress) {
        if (stage == bcmiFieldStageExactMatch) {
            ptr->pool_in_use = BCMI_LTSW_POLICER_POOL_USED_BY_EM;
        } else {
            ptr->pool_in_use = BCMI_LTSW_POLICER_POOL_USED_BY_IFP;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmint_ltsw_policer_pool_expand(
    int unit,
    int stage,
    bcmi_ltsw_policer_stage_t pol_stage,
    int pipe,
    int prio,
    int *pool_id)
{
    bcmint_policer_pool_state_t *ptr = NULL;
    bcmint_policer_pool_state_t *pool_ptr = NULL;
    bcmint_policer_state_t *state_ptr = NULL;
    int idx = 0, max_pools = 0, start_idx = 0, end_idx = 0;
    int num_entries = 0;

    SHR_FUNC_ENTER(unit);

    if (pol_stage == bcmiPolicerStageEgress) {
        max_pools = POLICER_INFO(unit)->max_egr_pools;
        pool_ptr = POLICER_INFO(unit)->pool_state_egr;
        state_ptr =  POLICER_INFO(unit)->egr_state;
    } else {
        max_pools = POLICER_INFO(unit)->max_ing_pools;
        pool_ptr = POLICER_INFO(unit)->pool_state_ing;
        state_ptr =  POLICER_INFO(unit)->ing_state;
    }

    if (pipe == -1) {
        start_idx = 0;
        end_idx = max_pools;
    } else {
        start_idx = (pipe * max_pools);
        end_idx = ((pipe + 1) * max_pools);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_entries_per_pool(unit,
                                              pol_stage,
                                              &num_entries));

    for (idx = (end_idx - 1); idx >= start_idx; idx--) {
        ptr = &pool_ptr[idx];

        /* This pool is free. */
        if (ptr->ref_count == 0) {
            ptr->mode |= BCMI_LTSW_POLICER_POOL_MODE_AUTO_EXPAND;
            ptr->ref_count++;
            *pool_id = (idx % max_pools);
            state_ptr->pools_in_use++;
            break;
        } else {
            /*
             * EM and IFP share same policer pools. Check if the pool
             * is in use by IFP or EM.
             */
            if ((stage == bcmiFieldStageIngress &&
                 ptr->pool_in_use == BCMI_LTSW_POLICER_POOL_USED_BY_EM) ||
                (stage == bcmiFieldStageExactMatch &&
                 ptr->pool_in_use == BCMI_LTSW_POLICER_POOL_USED_BY_IFP)) {
                continue;
            }

            if (ptr->entry_cnt >= num_entries) {
                continue;
            }
            ptr->mode |= BCMI_LTSW_POLICER_POOL_MODE_AUTO_EXPAND;
            ptr->ref_count++;
            *pool_id = (idx % max_pools);
            break;
        }
    }
    if (*pool_id == -1) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    if (pol_stage == bcmiPolicerStageIngress) {
        if (stage == bcmiFieldStageExactMatch) {
            ptr->pool_in_use = BCMI_LTSW_POLICER_POOL_USED_BY_EM;
        } else {
            ptr->pool_in_use = BCMI_LTSW_POLICER_POOL_USED_BY_IFP;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmint_ltsw_policer_pool_clear(
    int unit,
    bcmi_ltsw_policer_stage_t pol_stage,
    int pipe,
    int pool)
{
    bcmint_policer_pool_state_t *ptr = NULL;
    bcmint_policer_pool_state_t *pool_ptr = NULL;
    bcmint_policer_state_t *state_ptr = NULL;
    int iter = 0;
    int idx = 0, max_pools = 0;

    SHR_FUNC_ENTER(unit);

    if (pol_stage == bcmiPolicerStageEgress) {
        max_pools = POLICER_INFO(unit)->max_egr_pools;
        pool_ptr = POLICER_INFO(unit)->pool_state_egr;
        state_ptr =  POLICER_INFO(unit)->egr_state;
    } else {
        max_pools = POLICER_INFO(unit)->max_ing_pools;
        pool_ptr = POLICER_INFO(unit)->pool_state_ing;
        state_ptr =  POLICER_INFO(unit)->ing_state;
    }

    if (pipe == -1) {
        idx = pool;
    } else {
        idx = ((pipe * max_pools) + pool);
    }

    ptr = &pool_ptr[idx];

    if (ptr->ref_count == 0) {
        SHR_EXIT();
    }

    ptr->ref_count--;
    /* This pool is now free. */
    if (ptr->ref_count == 0) {
        ptr->priority = 0;
        ptr->pool_in_use = BCMI_LTSW_POLICER_POOL_USED_BY_NONE;
        if (ptr->mode & BCMI_LTSW_POLICER_POOL_MODE_SHARED) {
            ptr->mode &= ~BCMI_LTSW_POLICER_POOL_MODE_SHARED;
        }
        if (ptr->mode & BCMI_LTSW_POLICER_POOL_MODE_AUTO_EXPAND) {
            ptr->mode &= ~BCMI_LTSW_POLICER_POOL_MODE_AUTO_EXPAND;
        }
        if (ptr->mode & BCMI_LTSW_POLICER_POOL_MODE_FIXED) {
            ptr->mode &= ~BCMI_LTSW_POLICER_POOL_MODE_FIXED;
        }
        state_ptr->pools_in_use--;
        /*
         * Check if this pool was the secondary
         * pool of another primary pool.
         * If so, clear the bitmap since the pool is
         * now free.
         */
        for (iter = 0; iter < max_pools; iter++) {
            if (pipe == -1) {
                idx = iter;
            } else {
                idx = ((pipe * max_pools) + iter);
            }
            ptr = &pool_ptr[idx];
            if (SHR_BITGET(&ptr->pool_bmp, pool)) {
                SHR_BITCLR(&ptr->pool_bmp, pool);
            }
        }
    }

    /* If all pools are free, reset the alloc mode. */
    if (state_ptr->pools_in_use == 0) {
        state_ptr->flags = 0;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmint_ltsw_policer_pool_prio_update(
    int unit,
    bcmi_ltsw_policer_stage_t pol_stage,
    int pipe,
    int pool,
    int prio)
{
    bcmint_policer_pool_state_t *ptr = NULL;
    bcmint_policer_pool_state_t *pool_state_ptr = NULL;
    int idx = 0, max_pools = 0;

    SHR_FUNC_ENTER(unit);

    if (pol_stage == bcmiPolicerStageEgress) {
        max_pools = POLICER_INFO(unit)->max_egr_pools;
        pool_state_ptr = POLICER_INFO(unit)->pool_state_egr;
    } else {
        max_pools = POLICER_INFO(unit)->max_ing_pools;
        pool_state_ptr = POLICER_INFO(unit)->pool_state_ing;
    }

    if (pipe == -1) {
        idx = pool;
    } else {
        idx = ((pipe * max_pools) + pool);
    }

    ptr = &pool_state_ptr[idx];

    /*
     * If only one group is using the pool, change priority.
     * Else, return error.
     */
    if (ptr->ref_count == 1) {
        ptr->priority = prio;
    } else {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmint_ltsw_policer_pool_entry_cnt_update(
    int unit,
    bcmi_ltsw_policer_stage_t pol_stage,
    int pipe,
    int pool,
    bool dual,
    bool inc)
{
    bcmint_policer_state_t *state_ptr = NULL;
    bcmint_policer_pool_state_t *ptr = NULL;
    bcmint_policer_pool_state_t *pool_state_ptr = NULL;
    int idx = 0, max_pools = 0;
    int count = 1;

    SHR_FUNC_ENTER(unit);

    if (pol_stage == bcmiPolicerStageEgress) {
        state_ptr = POLICER_INFO(unit)->egr_state;
        max_pools = POLICER_INFO(unit)->max_egr_pools;
        pool_state_ptr = POLICER_INFO(unit)->pool_state_egr;
    } else {
        state_ptr = POLICER_INFO(unit)->ing_state;
        max_pools = POLICER_INFO(unit)->max_ing_pools;
        pool_state_ptr = POLICER_INFO(unit)->pool_state_ing;
    }

    if (pipe == -1) {
        idx = pool;
    } else {
        idx = ((pipe * max_pools) + pool);
    }

    ptr = &pool_state_ptr[idx];

    if (dual) {
        count = 2;
    }

    if (inc) {
        ptr->entry_cnt = (ptr->entry_cnt + count);
    } else {
        ptr->entry_cnt = (ptr->entry_cnt - count);
        /* If this pool has been auto-expanded, free the pool. */
        if ((state_ptr->flags & BCMI_LTSW_POLICER_ALLOC_MODE_DYNAMIC) &&
            (ptr->entry_cnt == 0) &&
            (ptr->mode & BCMI_LTSW_POLICER_POOL_MODE_AUTO_EXPAND)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_policer_pool_clear(unit,
                                                pol_stage,
                                                pipe,
                                                pool));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmint_ltsw_policer_pool_bmp_update(
    int unit,
    bcmi_ltsw_policer_stage_t pol_stage,
    int pipe,
    int pool,
    uint32_t pool_bmp)
{
    bcmint_policer_pool_state_t *ptr = NULL;
    bcmint_policer_pool_state_t *pool_state_ptr = NULL;
    int idx = 0, max_pools = 0;

    SHR_FUNC_ENTER(unit);

    if (pol_stage == bcmiPolicerStageEgress) {
        max_pools = POLICER_INFO(unit)->max_egr_pools;
        pool_state_ptr = POLICER_INFO(unit)->pool_state_egr;
    } else {
        max_pools = POLICER_INFO(unit)->max_ing_pools;
        pool_state_ptr = POLICER_INFO(unit)->pool_state_ing;
    }

    if (pipe == -1) {
        idx = pool;
    } else {
        idx = ((pipe * max_pools) + pool);
    }

    ptr = &pool_state_ptr[idx];

    ptr->pool_bmp = pool_bmp;

    SHR_FUNC_EXIT();
}

static int
bcmint_ltsw_policer_pool_info_get(int unit,
                                  bcmi_ltsw_policer_stage_t pol_stage,
                                  int pipe,
                                  int pool,
                                  bcmint_policer_pool_state_t **pool_state)
{
    bcmint_policer_pool_state_t *pool_state_ptr = NULL;
    int idx = 0, max_pools = 0;

    SHR_FUNC_ENTER(unit);

    if (pol_stage == bcmiPolicerStageEgress) {
        max_pools = POLICER_INFO(unit)->max_egr_pools;
        pool_state_ptr = POLICER_INFO(unit)->pool_state_egr;
    } else {
        max_pools = POLICER_INFO(unit)->max_ing_pools;
        pool_state_ptr = POLICER_INFO(unit)->pool_state_ing;
    }

    if (pipe == -1) {
        idx = pool;
    } else {
        idx = ((pipe * max_pools) + pool);
    }

    *pool_state = &pool_state_ptr[idx];

    SHR_FUNC_EXIT();
}

static int
bcmint_ltsw_policer_oper_mode_get(int unit,
                                  bcmi_ltsw_policer_stage_t pol_stage,
                                  bcmi_ltsw_policer_mode_t *mode)
{
    int dunit = 0;
    uint64_t oper_mode = 0x0;
    bcmlt_entry_handle_t oper_mode_template = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mode, SHR_E_INTERNAL);

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Entry handle allocate for operational mode. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              METER_FP_CONFIGs,
                              &oper_mode_template));

    /* Lookup mode template using handle. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, oper_mode_template,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    if (pol_stage == bcmiPolicerStageEgress) {
        /* Retreive operation mode field from fetched entry. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(oper_mode_template,
                                   METER_EGR_OPERMODE_PIPEUNIQUEs,
                                   &oper_mode));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(oper_mode_template,
                                   METER_ING_OPERMODE_PIPEUNIQUEs,
                                   &oper_mode));
    }

    *mode = oper_mode;

exit:
    if (BCMLT_INVALID_HDL != oper_mode_template) {
        (void) bcmlt_entry_free(oper_mode_template);
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */
int
bcmi_ltsw_policer_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (POLICER_INFO(unit)->initialized == 0) {
        SHR_EXIT();
    }

    POLICER_LOCK(unit);

    POLICER_INFO(unit)->initialized = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_policer_deinit(unit));

exit:
    POLICER_UNLOCK(unit);

    if (POLICER_INFO(unit)->lock != NULL) {
        sal_mutex_destroy(POLICER_INFO(unit)->lock);
        POLICER_INFO(unit)->lock = NULL;
    }

    SHR_FUNC_EXIT();
}

int
bcm_ltsw_policer_init(int unit)
{
    uint32_t ha_req_size = 0, ha_alloc_size = 0;
    uint32_t num_meters = 0, stage_flags = 0;
    uint32_t max_pool_idx = 0, num_pools = 0, num_elem = 0;
    void *ptr = NULL;
    bool support = false;
    int warm = 0, rv = 0, num_pipes = 0;
    bcmi_ltsw_policer_mode_t oper_mode;

    SHR_FUNC_ENTER(unit);

    if (POLICER_INFO(unit)->lock == NULL) {
        POLICER_INFO(unit)->lock = sal_mutex_create("bcmLtsPolicerMutex");
        SHR_NULL_CHECK(POLICER_INFO(unit)->lock, SHR_E_MEMORY);
    }

    warm = bcmi_warmboot_get(unit);

    if (!warm) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_policer_init(unit));
    }

    /* Check if IFP Policers are present. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_policer_supported(unit,
                                     bcmiPolicerStageIngress,
                                     &support));
    if (support == true) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_policer_stage_flags_get(unit,
                                               bcmiPolicerStageIngress,
                                               &stage_flags));
        POLICER_INFO(unit)->ing_stage_flags = stage_flags;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_policer_oper_mode_get(unit,
                                               bcmiPolicerStageIngress,
                                               &oper_mode));
        if (oper_mode == bcmiPolicerModePipeUnique) {
            num_pipes = bcmi_ltsw_dev_max_pp_pipe_num(unit);
        } else {
            num_pipes = 1;
        }

        /* Get the max pool idx for ingress policer */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_policer_max_pool_idx_get(unit,
                                                bcmiPolicerStageIngress,
                                                &max_pool_idx));
        num_pools = (max_pool_idx + 1);
        POLICER_INFO(unit)->max_ing_pools = num_pools;

        /* Allocate memory for saving each pool's priority. */
        num_elem = (num_pools * num_pipes);
        ha_req_size = (num_elem * sizeof(bcmint_policer_pool_state_t));
        ha_alloc_size = ha_req_size;
        ptr = bcmi_ltsw_ha_mem_alloc(unit,
                                     BCMI_HA_COMP_ID_POLICER,
                                     bcmiPolicerSubCompPoolStateIng,
                                     "bcmPolicerPoolStateIng",
                                     &ha_alloc_size);
        SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
        if (ha_alloc_size < ha_req_size) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
        if (!warm) {
            sal_memset(ptr, 0, ha_req_size);
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_policer_pool_init(unit,
                                             ptr,
                                             num_pipes,
                                             num_pools));
        }
        /* Report the structure array to ISSU. */
        rv = bcmi_ltsw_issu_struct_info_report(unit,
                                               BCMI_HA_COMP_ID_POLICER,
                                               bcmiPolicerSubCompPoolStateIng,
                                               0,
                                               sizeof(bcmint_policer_pool_state_t),
                                               num_elem,
                                               BCMINT_POLICER_POOL_STATE_T_ID);
        if (rv != SHR_E_EXISTS) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
        POLICER_INFO(unit)->pool_state_ing = ptr;

        ptr = NULL;
        /* Get the maximum number of ingress policers supported. */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_policer_capacity_get(unit,
                                            bcmiPolicerStageIngress,
                                            &num_meters));

        /* Allocate memory for ing_policer entries. */
        ha_req_size = ((num_meters + 1) * sizeof(bcmint_policer_entry_t));
        ha_alloc_size = ha_req_size;
        ptr = bcmi_ltsw_ha_mem_alloc(unit,
                                     BCMI_HA_COMP_ID_POLICER,
                                     bcmiPolicerSubCompIngEntries,
                                     "bcmPolicerIngEntries",
                                     &ha_alloc_size);
        SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
        if (ha_alloc_size < ha_req_size) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
        if (!warm) {
            sal_memset(ptr, 0, ha_req_size);
        }
        /* Report the structure array to ISSU. */
        rv = bcmi_ltsw_issu_struct_info_report(unit,
                                               BCMI_HA_COMP_ID_POLICER,
                                               bcmiPolicerSubCompIngEntries,
                                               0,
                                               sizeof(bcmint_policer_entry_t),
                                               (num_meters + 1),
                                               BCMINT_POLICER_ENTRY_T_ID);
        if (rv != SHR_E_EXISTS) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }

        POLICER_INFO(unit)->ing_entries = ptr;
        ptr = NULL;

        /* Allocate memory for policer state. */
        ha_req_size = (sizeof(bcmint_policer_state_t));
        ha_alloc_size = ha_req_size;
        ptr = bcmi_ltsw_ha_mem_alloc(unit,
                                     BCMI_HA_COMP_ID_POLICER,
                                     bcmiPolicerStateIng,
                                     "bcmPolicerIngState",
                                     &ha_alloc_size);
        SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
        if (ha_alloc_size < ha_req_size) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
        if (!warm) {
            sal_memset(ptr, 0, ha_req_size);
        }
        /* Report the structure array to ISSU. */
        rv = bcmi_ltsw_issu_struct_info_report(unit,
                                               BCMI_HA_COMP_ID_POLICER,
                                               bcmiPolicerStateIng,
                                               0,
                                               sizeof(bcmint_policer_state_t),
                                               1,
                                               BCMINT_POLICER_STATE_T_ID);
        if (rv != SHR_E_EXISTS) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }

        POLICER_INFO(unit)->ing_state = ptr;
        ptr = NULL;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_policer_supported(unit,
                                     bcmiPolicerStageEgress,
                                     &support));
    if (support == true) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_policer_stage_flags_get(unit,
                                               bcmiPolicerStageEgress,
                                               &stage_flags));
        POLICER_INFO(unit)->egr_stage_flags = stage_flags;

        /* Get the operational mode of EFP. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_policer_oper_mode_get(unit,
                                               bcmiPolicerStageEgress,
                                               &oper_mode));
        if (oper_mode == bcmiPolicerModePipeUnique) {
            num_pipes = bcmi_ltsw_dev_max_pp_pipe_num(unit);
        } else {
            num_pipes = 1;
        }

        /* Get the max pool idx for egress policer */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_policer_max_pool_idx_get(unit,
                                                bcmiPolicerStageEgress,
                                                &max_pool_idx));
        num_pools = (max_pool_idx + 1);
        POLICER_INFO(unit)->max_egr_pools = num_pools;

        /* Allocate memory for saving each pool's priority. */
        num_elem = (num_pools * num_pipes);
        ha_req_size = (num_elem * sizeof(bcmint_policer_pool_state_t));
        ha_alloc_size = ha_req_size;
        ptr = bcmi_ltsw_ha_mem_alloc(unit,
                                     BCMI_HA_COMP_ID_POLICER,
                                     bcmiPolicerSubCompPoolStateEgr,
                                     "bcmPolicerPoolStateEgr",
                                     &ha_alloc_size);
        SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
        if (ha_alloc_size < ha_req_size) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
        if (!warm) {
            sal_memset(ptr, 0, ha_req_size);
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_policer_pool_init(unit,
                                             ptr,
                                             num_pipes,
                                             num_pools));
        }
        /* Report the structure array to ISSU. */
        rv = bcmi_ltsw_issu_struct_info_report(unit,
                                               BCMI_HA_COMP_ID_POLICER,
                                               bcmiPolicerSubCompPoolStateEgr,
                                               0,
                                               sizeof(bcmint_policer_pool_state_t),
                                               num_elem,
                                               BCMINT_POLICER_POOL_STATE_T_ID);
        if (rv != SHR_E_EXISTS) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
        POLICER_INFO(unit)->pool_state_egr = ptr;

        ptr = NULL;
        /* Get the maximum number of egress policers supported. */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_policer_capacity_get(unit,
                                            bcmiPolicerStageEgress,
                                            &num_meters));

        /* Allocate memory for egr_policer entries. */
        ha_req_size = ((num_meters + 1) * sizeof(bcmint_policer_entry_t));
        ha_alloc_size = ha_req_size;
        ptr = bcmi_ltsw_ha_mem_alloc(unit,
                                     BCMI_HA_COMP_ID_POLICER,
                                     bcmiPolicerSubCompEgrEntries,
                                     "bcmPolicerEgrEntries",
                                     &ha_alloc_size);
        SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
        if (ha_alloc_size < ha_req_size) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
        if (!warm) {
            sal_memset(ptr, 0, ha_req_size);
        }
        /* Report the structure array to ISSU. */
        rv = bcmi_ltsw_issu_struct_info_report(unit,
                                               BCMI_HA_COMP_ID_POLICER,
                                               bcmiPolicerSubCompEgrEntries,
                                               0,
                                               sizeof(bcmint_policer_entry_t),
                                               (num_meters + 1),
                                               BCMINT_POLICER_ENTRY_T_ID);
        if (rv != SHR_E_EXISTS) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }

        POLICER_INFO(unit)->egr_entries = ptr;
        ptr = NULL;

        /* Allocate memory for policer state. */
        ha_req_size = (sizeof(bcmint_policer_state_t));
        ha_alloc_size = ha_req_size;
        ptr = bcmi_ltsw_ha_mem_alloc(unit,
                                     BCMI_HA_COMP_ID_POLICER,
                                     bcmiPolicerStateEgr,
                                     "bcmPolicerEgrState",
                                     &ha_alloc_size);
        SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
        if (ha_alloc_size < ha_req_size) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
        if (!warm) {
            sal_memset(ptr, 0, ha_req_size);
        }
        /* Report the structure array to ISSU. */
        rv = bcmi_ltsw_issu_struct_info_report(unit,
                                               BCMI_HA_COMP_ID_POLICER,
                                               bcmiPolicerStateEgr,
                                               0,
                                               sizeof(bcmint_policer_state_t),
                                               1,
                                               BCMINT_POLICER_STATE_T_ID);
        if (rv != SHR_E_EXISTS) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }

        POLICER_INFO(unit)->egr_state = ptr;
        ptr = NULL;
    }

    POLICER_INFO(unit)->initialized = 1;

exit:
    if (SHR_FUNC_ERR()) {
        if (!warm) {
            if (POLICER_INFO(unit)->ing_entries != NULL) {
                (void)bcmi_ltsw_ha_mem_free(unit,
                                            POLICER_INFO(unit)->ing_entries);
                POLICER_INFO(unit)->ing_entries = NULL;
            }

            if (POLICER_INFO(unit)->pool_state_ing != NULL) {
                (void)bcmi_ltsw_ha_mem_free(unit,
                                            POLICER_INFO(unit)->pool_state_ing);
                POLICER_INFO(unit)->pool_state_ing = NULL;
            }

            if (POLICER_INFO(unit)->ing_state != NULL) {
                (void)bcmi_ltsw_ha_mem_free(unit,
                                            POLICER_INFO(unit)->ing_state);
                POLICER_INFO(unit)->ing_state = NULL;
            }

            if (POLICER_INFO(unit)->egr_entries != NULL) {
                (void)bcmi_ltsw_ha_mem_free(unit,
                                            POLICER_INFO(unit)->egr_entries);
                POLICER_INFO(unit)->egr_entries = NULL;
            }

            if (POLICER_INFO(unit)->pool_state_egr != NULL) {
                (void)bcmi_ltsw_ha_mem_free(unit,
                                            POLICER_INFO(unit)->pool_state_egr);
                POLICER_INFO(unit)->pool_state_egr = NULL;
            }

            if (POLICER_INFO(unit)->egr_state != NULL) {
                (void)bcmi_ltsw_ha_mem_free(unit,
                                            POLICER_INFO(unit)->egr_state);
                POLICER_INFO(unit)->egr_state = NULL;
            }
        }
        if (POLICER_INFO(unit)->lock != NULL) {
            sal_mutex_destroy(POLICER_INFO(unit)->lock);
            POLICER_INFO(unit)->lock = NULL;
        }
    }

    SHR_FUNC_EXIT();
}

int
bcm_ltsw_policer_create(
    int unit,
    bcm_policer_config_t *pol_cfg,
    bcm_policer_t *policer_id)
{
    bool exists = false, support = false;
    uint32_t pid = 0, stage_flags = 0;
    int max_pools = 0;
    bcmi_ltsw_policer_stage_t pol_stage = bcmiPolicerStageIngress;
    bcmint_policer_entry_t *pol_entry = NULL;

    SHR_FUNC_ENTER(unit);
    POLICER_INIT_CHECK(unit);

    POLICER_LOCK(unit);

    /* Input parameters check. */
    SHR_NULL_CHECK(policer_id, SHR_E_PARAM);
    SHR_NULL_CHECK(pol_cfg, SHR_E_PARAM);

    if (pol_cfg->flags & BCM_POLICER_EGRESS) {
        pol_stage = bcmiPolicerStageEgress;
        stage_flags = POLICER_INFO(unit)->egr_stage_flags;
        max_pools = POLICER_INFO(unit)->max_egr_pools;
    } else {
        pol_stage = bcmiPolicerStageIngress;
        stage_flags = POLICER_INFO(unit)->ing_stage_flags;
        max_pools = POLICER_INFO(unit)->max_ing_pools;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_policer_supported(unit,
                                     pol_stage,
                                     &support));
    if (support != true) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (pol_cfg->flags & BCM_POLICER_WITH_ID) {
        pid = (*policer_id & BCM_FIELD_ID_MASK);
        if (pid == 0) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        /* Check if the pid already exists. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_policer_id_validate(unit, pol_stage, pid, &exists));

        if (exists == true && !(pol_cfg->flags & BCM_POLICER_REPLACE)) {
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }

        if (exists == true && (pol_cfg->flags & BCM_POLICER_WITH_SHARED_POOL_ID)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (exists == false && (pol_cfg->flags & BCM_POLICER_REPLACE)) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
    }

    if (pol_cfg->flags & BCM_POLICER_WITH_SHARED_POOL_ID) {
        if (!(stage_flags & BCMI_LTSW_POLICER_STAGE_SUPPORT_SHARED_POOL_ID)) {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
        if (pol_cfg->pool_id == -1 || pol_cfg->pool_id >= max_pools) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    /* Find a free pid. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_id_alloc(unit, pol_stage, &pid));

    /* Copy the policer config into the sw state for this pid. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_entry_insert(unit, pol_stage, pol_cfg, pid));

    /* Retrieve the sw state for this pid to check ref count. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_entry_get(unit, pol_stage, pid, &pol_entry));

    /* Get the stage encoded format for this pid. */
    *policer_id = pol_entry->policer_id;

    if (pol_entry->ref_count != 0) {
        /*
         * If there are FP entries referencing this policer, it means
         * this has been installed in hardware. Call the install function to
         * update the HW fields also.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_policer_install(unit,
                                       *policer_id,
                                       pol_cfg,
                                       pol_entry->pipe,
                                       pol_entry->pool_id));

        /*
         * Update all FP entries that this policer_id is attached to.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_field_entry_policer_update_all(unit,
                                                      *policer_id,
                                                      pol_entry->fp_stage));
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (!SHR_FUNC_VAL_IS(SHR_E_EXISTS) && !SHR_FUNC_VAL_IS(SHR_E_INIT) &&
            !SHR_FUNC_VAL_IS(SHR_E_UNAVAIL)) {
            (void)(bcmint_ltsw_policer_entry_delete(unit, pol_stage, pid));
        }
    }
    POLICER_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_policer_destroy(
    int unit,
    bcm_policer_t policer_id)
{
    uint32_t pid = 0;
    bool exists = false, support = false;
    bcmi_ltsw_policer_stage_t pol_stage;
    bcmint_policer_entry_t *pol_entry = NULL;

    SHR_FUNC_ENTER(unit);
    POLICER_INIT_CHECK(unit);

    POLICER_LOCK(unit);

    if ((policer_id & ~(BCM_FIELD_ID_MASK)) == BCM_FIELD_EFP_ID_BASE) {
        pol_stage = bcmiPolicerStageEgress;
    } else {
        pol_stage = bcmiPolicerStageIngress;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_policer_supported(unit,
                                     pol_stage,
                                     &support));
    if (support != true) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    pid = (policer_id & BCM_FIELD_ID_MASK);

    /* Check if the policer exists. If not, return error. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_id_validate(unit, pol_stage, pid, &exists));

    if (exists == false) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Fetch the configuration from s/w state. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_entry_get(unit, pol_stage, pid, &pol_entry));

    /* Policer_id is not free, is being used some FP entries. */
    if (pol_entry->ref_count != 0) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    /* Delete the entry from s/w state. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_entry_delete(unit, pol_stage, pid));
exit:
    POLICER_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_policer_destroy_all(int unit)
{
    bool exists = false, support = false;
    uint32_t num_meters = 0, idx = 0;
    bcm_policer_t policer_id;

    SHR_FUNC_ENTER(unit);
    POLICER_INIT_CHECK(unit);

    POLICER_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_policer_supported(unit,
                                     bcmiPolicerStageIngress,
                                     &support));
    if (support == true) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_policer_capacity_get(unit,
                                            bcmiPolicerStageIngress,
                                            &num_meters));
        /* Get all valid IFP policers and delete from s/w state. */
        for (idx = 1; idx <= num_meters; idx++) {
            exists = false;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_policer_id_validate(unit, false, idx, &exists));
            if (exists == false) {
                continue;
            }
            policer_id = (BCM_FIELD_IFP_ID_BASE | idx);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm_ltsw_policer_destroy(unit, policer_id));
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_policer_supported(unit,
                                     bcmiPolicerStageEgress,
                                     &support));
    if (support == true) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_policer_capacity_get(unit,
                                            bcmiPolicerStageEgress,
                                            &num_meters));
        /* Get all valid EFP policers and delete from s/w state. */
        for (idx = 1; idx <= num_meters; idx++) {
            exists = false;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_policer_id_validate(unit, true, idx, &exists));
            if (exists == false) {
                continue;
            }
            policer_id = (BCM_FIELD_EFP_ID_BASE | idx);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm_ltsw_policer_destroy(unit, policer_id));
        }
    }

exit:
    POLICER_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_policer_set(
    int unit,
    bcm_policer_t policer_id,
    bcm_policer_config_t *pol_cfg)
{
    uint32_t pid = 0;
    uint32_t stage_flags = 0;
    bool exists = false, support = false;
    bcmi_ltsw_policer_stage_t pol_stage;
    bcmint_policer_entry_t *pol_entry = NULL;

    SHR_FUNC_ENTER(unit);
    POLICER_INIT_CHECK(unit);

    POLICER_LOCK(unit);

    if ((policer_id & ~(BCM_FIELD_ID_MASK)) == BCM_FIELD_EFP_ID_BASE) {
        pol_stage = bcmiPolicerStageEgress;
        stage_flags = POLICER_INFO(unit)->egr_stage_flags;
    } else {
        pol_stage = bcmiPolicerStageIngress;
        stage_flags = POLICER_INFO(unit)->ing_stage_flags;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_policer_supported(unit,
                                     pol_stage,
                                     &support));
    if (support != true) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    pid = (policer_id & BCM_FIELD_ID_MASK);

    /* Check if the policeR_id exists, if not return error. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_id_validate(unit, pol_stage, pid, &exists));

    if (exists == false) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (pol_cfg->flags & BCM_POLICER_WITH_SHARED_POOL_ID) {
        if (!(stage_flags & BCMI_LTSW_POLICER_STAGE_SUPPORT_SHARED_POOL_ID)) {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Insert the entry into the s/w state. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_entry_insert(unit, pol_stage, pol_cfg, pid));

    /* Fetch the s/w state corresponding to this policer_id. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_entry_get(unit, pol_stage, pid, &pol_entry));

    if (pol_entry->ref_count != 0) {
        /*
         * ref_count != 0 means there are FP entries using this policer
         * and that the policer is installed in hardware. Update the
         * hardware tables.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_policer_install(unit,
                                       policer_id,
                                       pol_cfg,
                                       pol_entry->pipe,
                                       pol_entry->pool_id));

        /* Update the FP entries to which this policer is attached. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_field_entry_policer_update_all(unit,
                                                      policer_id,
                                                      pol_entry->fp_stage));
    }

exit:
    POLICER_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_policer_get(
    int unit,
    bcm_policer_t policer_id,
    bcm_policer_config_t *pol_cfg)
{
    uint32_t pid = 0;
    bool exists = false, support = false;
    bcmi_ltsw_policer_stage_t pol_stage;
    bcmint_policer_entry_t *pol_entry = NULL;

    SHR_FUNC_ENTER(unit);
    POLICER_INIT_CHECK(unit);

    POLICER_LOCK(unit);

    if ((policer_id & ~(BCM_FIELD_ID_MASK)) == BCM_FIELD_EFP_ID_BASE) {
        pol_stage = bcmiPolicerStageEgress;
    } else {
        pol_stage = bcmiPolicerStageIngress;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_policer_supported(unit,
                                     pol_stage,
                                     &support));
    if (support != true) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    pid = (policer_id & BCM_FIELD_ID_MASK);

    /* Check if the policer exists. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_id_validate(unit, pol_stage, pid, &exists));

    if (exists == false) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Fetch the policer configuration from s/w state. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_entry_get(unit, pol_stage, pid, &pol_entry));

    pol_cfg->flags = pol_entry->pol_cfg.flags;
    pol_cfg->mode  = pol_entry->pol_cfg.mode;
    pol_cfg->ckbits_sec = pol_entry->pol_cfg.ckbits_sec;
    pol_cfg->cbits_sec_lower = pol_entry->pol_cfg.cbits_sec_lower;
    pol_cfg->ckbits_burst = pol_entry->pol_cfg.ckbits_burst;
    pol_cfg->cbits_burst_lower = pol_entry->pol_cfg.cbits_burst_lower;
    pol_cfg->pkbits_sec = pol_entry->pol_cfg.pkbits_sec;
    pol_cfg->pbits_sec_lower = pol_entry->pol_cfg.pbits_sec_lower;
    pol_cfg->pkbits_burst = pol_entry->pol_cfg.pkbits_burst;
    pol_cfg->pbits_burst_lower = pol_entry->pol_cfg.pbits_burst_lower;
    pol_cfg->actual_ckbits_sec =
        pol_entry->pol_cfg.actual_ckbits_sec;
    pol_cfg->actual_cbits_sec_lower =
        pol_entry->pol_cfg.actual_cbits_sec_lower;
    pol_cfg->actual_ckbits_burst =
        pol_entry->pol_cfg.actual_ckbits_burst;
    pol_cfg->actual_cbits_burst_lower =
        pol_entry->pol_cfg.actual_cbits_burst_lower;
    pol_cfg->actual_pkbits_sec =
        pol_entry->pol_cfg.actual_pkbits_sec;
    pol_cfg->actual_pbits_sec_lower =
        pol_entry->pol_cfg.actual_pbits_sec_lower;
    pol_cfg->actual_pkbits_burst =
        pol_entry->pol_cfg.actual_pkbits_burst;
    pol_cfg->actual_pbits_burst_lower =
        pol_entry->pol_cfg.actual_pbits_burst_lower;
    if (pol_cfg->flags & BCM_POLICER_WITH_SHARED_POOL_ID) {
        pol_cfg->pool_id = pol_entry->pool_id;
    }

exit:
    POLICER_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_policer_traverse(
    int unit,
    bcm_policer_traverse_cb cb,
    void *user_data)
{
    int rv = 0;
    bool exists = false, support = false;
    uint32_t num_meters = 0, idx = 0;
    bcm_policer_t policer_id;
    bcm_policer_config_t pol_cfg;

    SHR_FUNC_ENTER(unit);
    POLICER_INIT_CHECK(unit);

    POLICER_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_policer_supported(unit,
                                     bcmiPolicerStageIngress,
                                     &support));
    if (support == true) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_policer_capacity_get(unit,
                                            bcmiPolicerStageIngress,
                                            &num_meters));
        /* Fetch the configuration of all IFP policers that are valid. */
        for (idx = 1; idx <= num_meters; idx++) {
            exists = false;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_policer_id_validate(unit, false, idx, &exists));
            if (exists == false) {
                continue;
            }
            policer_id = (BCM_FIELD_IFP_ID_BASE | idx);
            sal_memset(&pol_cfg, 0, sizeof(pol_cfg));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm_ltsw_policer_get(unit, policer_id, &pol_cfg));
            rv = cb(unit, policer_id, &pol_cfg, user_data);
            if (SHR_FAILURE(rv) && bcmi_ltsw_dev_cb_err_abort(unit)) {
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_policer_supported(unit,
                                     bcmiPolicerStageEgress,
                                     &support));
    if (support == true) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_policer_capacity_get(unit,
                                            bcmiPolicerStageEgress,
                                            &num_meters));
        /* Fetch the configuration of all EFP policers that are valid. */
        for (idx = 1; idx <= num_meters; idx++) {
            exists = false;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_policer_id_validate(unit, true, idx, &exists));
            if (exists == false) {
                continue;
            }
            policer_id = (BCM_FIELD_EFP_ID_BASE | idx);
            sal_memset(&pol_cfg, 0, sizeof(pol_cfg));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm_ltsw_policer_get(unit, policer_id, &pol_cfg));
            rv = cb(unit, policer_id, &pol_cfg, user_data);
            if (SHR_FAILURE(rv) && bcmi_ltsw_dev_cb_err_abort(unit)) {
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
        }
    }

exit:
    POLICER_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

static int
bcmint_ltsw_policer_install(int unit,
                            int fp_stage,
                            bcmi_ltsw_policer_stage_t pol_stage,
                            bcm_policer_t policer_id,
                            bcm_policer_config_t *pol_cfg,
                            int pipe,
                            bool fixed_pool,
                            uint32_t grp_pool_bmp,
                            int *pool_id)
{
    int rv = 0;
    int prio = 0;
    int max_pools = 0;
    int new_pool_id = -1;
    SHR_BITDCL *pool_bmp = NULL;
    bool dual = false;
    uint32_t alloc_sz = 0;
    bcmint_policer_pool_state_t *pool_ptr = NULL;
    bcmint_policer_state_t *state_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    if (pol_stage == bcmiPolicerStageEgress) {
        state_ptr = POLICER_INFO(unit)->egr_state;
        max_pools = POLICER_INFO(unit)->max_egr_pools;
    } else {
        state_ptr = POLICER_INFO(unit)->ing_state;
        max_pools = POLICER_INFO(unit)->max_ing_pools;
    }

    alloc_sz = SHR_BITALLOCSIZE(max_pools);
    SHR_ALLOC(pool_bmp, alloc_sz, "Policer pool bitmap");
    sal_memset(pool_bmp, 0, alloc_sz);

    rv = mbcm_ltsw_policer_install(unit, policer_id, pol_cfg, pipe, *pool_id);
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_RESOURCE) {
            if ((pol_cfg->flags & BCM_POLICER_WITH_SHARED_POOL_ID) ||
                (fixed_pool == true)) {
                SHR_ERR_EXIT(rv);
            }
            if (state_ptr->flags & BCMI_LTSW_POLICER_ALLOC_MODE_DYNAMIC) {
                /* check for the state and alloc more pools or use from pool_bmp*/
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_ltsw_policer_pool_info_get(unit,
                                                       pol_stage,
                                                       pipe,
                                                       *pool_id,
                                                       &pool_ptr));
                prio = pool_ptr->priority;
                *pool_bmp = pool_ptr->pool_bmp;
            } else {
                *pool_bmp = grp_pool_bmp;
            }
            if (*pool_bmp) {
                SHR_BIT_ITER(pool_bmp, max_pools, new_pool_id) {
                    rv = mbcm_ltsw_policer_install
                        (unit, policer_id, pol_cfg, pipe, new_pool_id);
                    if (SHR_SUCCESS(rv)) {
                        *pool_id = new_pool_id;
                        break;
                    }
                }
            }

            if (SHR_FAILURE(rv)) {
                if (rv == SHR_E_RESOURCE) {
                    if (state_ptr->flags & BCMI_LTSW_POLICER_ALLOC_MODE_DYNAMIC) {
                        SHR_IF_ERR_VERBOSE_EXIT
                            (bcmint_ltsw_policer_pool_expand(unit,
                                                             fp_stage,
                                                             pol_stage,
                                                             pipe,
                                                             prio,
                                                             &new_pool_id));
                        SHR_BITSET(pool_bmp, new_pool_id);
                        SHR_IF_ERR_VERBOSE_EXIT
                            (bcmint_ltsw_policer_pool_bmp_update(unit,
                                                                 pol_stage,
                                                                 pipe,
                                                                 *pool_id,
                                                                 *pool_bmp));
                        *pool_id = new_pool_id;
                        SHR_IF_ERR_VERBOSE_EXIT
                            (mbcm_ltsw_policer_install(unit,
                                                       policer_id,
                                                       pol_cfg,
                                                       pipe,
                                                       *pool_id));
                    } else {
                        SHR_ERR_EXIT(SHR_E_RESOURCE);
                    }
                } else {
                    SHR_ERR_EXIT(rv);
                }
            }
        } else {
            SHR_ERR_EXIT(rv);
        }
    }

    if ((pol_cfg->mode != bcmPolicerModeGreen) &&
        (pol_cfg->mode != bcmPolicerModeCommitted)) {
        dual = true;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_pool_entry_cnt_update(unit,
                                                   pol_stage,
                                                   pipe,
                                                   *pool_id,
                                                   dual,
                                                   true));
exit:
    SHR_FREE(pool_bmp);
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_policer_attach(int unit,
                         int stage,
                         bcm_policer_t policer_id,
                         int pipe,
                         int grp_pool_id,
                         uint32_t grp_pool_bmp,
                         bool fixed_pool,
                         int *ret_pool_id)
{
    bcm_policer_config_t pol_cfg;
    uint32_t pid = 0;
    int pool_id = 0;
    bool exists = false, support = false;
    bcmi_ltsw_policer_stage_t pol_stage;
    bcmi_ltsw_policer_mode_t oper_mode = 0;
    bcmint_policer_entry_t *pol_entry = NULL;
    bcmint_policer_pool_state_t *pool_ptr = NULL;
    bcmint_policer_state_t *state_ptr = NULL;

    SHR_FUNC_ENTER(unit);
    POLICER_INIT_CHECK(unit);

    POLICER_LOCK(unit);

    if ((policer_id & ~(BCM_FIELD_ID_MASK)) == BCM_FIELD_EFP_ID_BASE) {
        pol_stage = bcmiPolicerStageEgress;
        state_ptr = POLICER_INFO(unit)->egr_state;
    } else {
        pol_stage = bcmiPolicerStageIngress;
        state_ptr = POLICER_INFO(unit)->ing_state;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_policer_supported(unit,
                                     pol_stage,
                                     &support));
    if (support != true) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_oper_mode_get(unit,
                                           pol_stage,
                                           &oper_mode));
    if ((oper_mode != bcmiPolicerModePipeUnique) && (pipe != -1)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    pid = (policer_id & BCM_FIELD_ID_MASK);

    /* Check if the policer exists. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_id_validate(unit, pol_stage, pid, &exists));

    if (exists == false) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Fetch the configuration from s/w state. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_entry_get(unit, pol_stage, pid, &pol_entry));

    if (pol_entry->ref_count == 0) {
        /*
         * ref_count == 0, means this is the first FP entry that the
         * policer is attached to. Policer is not yet installed in hardware.
         */

        sal_memset(&pol_cfg, 0, sizeof(pol_cfg));
        pol_cfg.flags = pol_entry->pol_cfg.flags;
        pol_cfg.mode  = pol_entry->pol_cfg.mode;
        pol_cfg.ckbits_sec = pol_entry->pol_cfg.ckbits_sec;
        pol_cfg.cbits_sec_lower = pol_entry->pol_cfg.cbits_sec_lower;
        pol_cfg.ckbits_burst = pol_entry->pol_cfg.ckbits_burst;
        pol_cfg.cbits_burst_lower = pol_entry->pol_cfg.cbits_burst_lower;
        pol_cfg.pkbits_sec = pol_entry->pol_cfg.pkbits_sec;
        pol_cfg.pbits_sec_lower = pol_entry->pol_cfg.pbits_sec_lower;
        pol_cfg.pkbits_burst = pol_entry->pol_cfg.pkbits_burst;
        pol_cfg.pbits_burst_lower = pol_entry->pol_cfg.pbits_burst_lower;

        /* Install the policer in the hardware. */
        if (pol_cfg.flags & BCM_POLICER_WITH_SHARED_POOL_ID) {
            pool_id = pol_entry->pool_id;
        } else {
            pool_id = grp_pool_id;
        }
        
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_policer_install(unit,
                                         stage,
                                         pol_stage,
                                         policer_id,
                                         &pol_cfg,
                                         pipe,
                                         fixed_pool,
                                         grp_pool_bmp,
                                         &pool_id));

        /* update actual_xxx fields. */
        sal_memset(&pol_cfg, 0, sizeof(pol_cfg));

        /*
         * Fetch the policer configuration from hardware
         * and update actual_xxx fields.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_policer_get(unit, policer_id, &pol_cfg));
        pol_entry->pipe = pipe;
        pol_entry->fp_stage = stage;
        pol_entry->pool_id = pool_id;
        pol_entry->pol_cfg.actual_ckbits_sec =
            pol_cfg.actual_ckbits_sec;
        pol_entry->pol_cfg.actual_cbits_sec_lower =
            pol_cfg.actual_cbits_sec_lower;
        pol_entry->pol_cfg.actual_ckbits_burst =
            pol_cfg.actual_ckbits_burst;
        pol_entry->pol_cfg.actual_cbits_burst_lower =
            pol_cfg.actual_cbits_burst_lower;
        pol_entry->pol_cfg.actual_pkbits_sec =
            pol_cfg.actual_pkbits_sec;
        pol_entry->pol_cfg.actual_pbits_sec_lower =
            pol_cfg.actual_pbits_sec_lower;
        pol_entry->pol_cfg.actual_pkbits_burst =
            pol_cfg.actual_pkbits_burst;
        pol_entry->pol_cfg.actual_pbits_burst_lower =
            pol_cfg.actual_pbits_burst_lower;
    } else {
        if (pipe != pol_entry->pipe) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (state_ptr->flags & BCMI_LTSW_POLICER_ALLOC_MODE_DYNAMIC) {
            /*
             * HSDK allocated pool. Get the expanded
             * pool bitmap from main pool.
             */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_policer_pool_info_get(unit,
                                                   pol_stage,
                                                   pipe,
                                                   grp_pool_id,
                                                   &pool_ptr));
            if ((grp_pool_id != pol_entry->pool_id) &&
                (!SHR_BITGET(&pool_ptr->pool_bmp, pol_entry->pool_id))) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        } else {
            if (fixed_pool) {
                /*
                 * User has specified policer pool during attach.
                 */
                if (grp_pool_id != pol_entry->pool_id) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
            } else {
                /*
                 * Policer pool not fixed. Check if pool is valid.
                 */
                if ((grp_pool_id != pol_entry->pool_id) &&
                    (!SHR_BITGET(&grp_pool_bmp, pol_entry->pool_id))) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
            }
        }
    }

    if (ret_pool_id != NULL) {
        *ret_pool_id = pol_entry->pool_id;
    }
    /* Increment ref count. */
    pol_entry->ref_count++;

exit:
    POLICER_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_policer_detach(int unit,
                         bcm_policer_t policer_id)
{
    uint32_t pid = 0;
    bool exists = false, support = false;
    bool dual = false;
    bcmi_ltsw_policer_stage_t pol_stage;
    bcmint_policer_entry_t *pol_entry = NULL;

    SHR_FUNC_ENTER(unit);
    POLICER_INIT_CHECK(unit);

    POLICER_LOCK(unit);

    if ((policer_id & ~(BCM_FIELD_ID_MASK)) == BCM_FIELD_EFP_ID_BASE) {
        pol_stage = bcmiPolicerStageEgress;
    } else {
        pol_stage = bcmiPolicerStageIngress;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_policer_supported(unit,
                                     pol_stage,
                                     &support));
    if (support != true) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    pid = (policer_id & BCM_FIELD_ID_MASK);

    /* Check if the policer exists, if not return error. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_id_validate(unit, pol_stage, pid, &exists));

    if (exists == false) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Fetch the policer configuration from s/w state. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_entry_get(unit, pol_stage, pid, &pol_entry));

    /* Decrement ref_count. */
    pol_entry->ref_count--;

    if (pol_entry->ref_count == 0) {
        /*
         * Policer being detached from the last FP entry,
         * uninstall from hardware.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_policer_destroy(unit, policer_id));

        if ((pol_entry->pol_cfg.mode != bcmPolicerModeGreen) &&
            (pol_entry->pol_cfg.mode != bcmPolicerModeCommitted)) {
            dual = true;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_policer_pool_entry_cnt_update(unit,
                                                       pol_stage,
                                                       pol_entry->pipe,
                                                       pol_entry->pool_id,
                                                       dual,
                                                       false));
    }

exit:
    POLICER_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_policer_pool_alloc(int unit,
                             int stage,
                             bool shared,
                             int pipe,
                             int priority,
                             int *pool_id,
                             uint32_t grp_pool_bmp)
{
    bool support = false;
    int idx = 0, max_pools = 0;
    uint32_t alloc_sz = 0;
    bcmi_ltsw_policer_mode_t oper_mode = 0;
    bcmi_ltsw_policer_stage_t policer_stage;
    SHR_BITDCL *pool_bmp = NULL;

    SHR_FUNC_ENTER(unit);
    POLICER_INIT_CHECK(unit);

    POLICER_LOCK(unit);

    if (stage == bcmiFieldStageEgress) {
        policer_stage = bcmiPolicerStageEgress;
        max_pools = POLICER_INFO(unit)->max_egr_pools;
    } else if ((stage == bcmiFieldStageIngress) ||
               (stage == bcmiFieldStageExactMatch)) {
        policer_stage = bcmiPolicerStageIngress;
        max_pools = POLICER_INFO(unit)->max_ing_pools;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_policer_supported(unit,
                                     policer_stage,
                                     &support));
    if (support != true) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    alloc_sz = SHR_BITALLOCSIZE(max_pools);
    SHR_ALLOC(pool_bmp, alloc_sz, "Policer pool bitmap");
    sal_memset(pool_bmp, 0, alloc_sz);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_oper_mode_get(unit,
                                           policer_stage,
                                           &oper_mode));
    if ((oper_mode != bcmiPolicerModePipeUnique) && (pipe != -1)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_pool_alloc(unit,
                                        stage,
                                        policer_stage,
                                        shared,
                                        pipe,
                                        priority,
                                        pool_id));

    *pool_bmp = grp_pool_bmp;
    SHR_BIT_ITER(pool_bmp, max_pools, idx) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_policer_pool_alloc(unit,
                                            stage,
                                            policer_stage,
                                            shared,
                                            pipe,
                                            priority,
                                            &idx));
    }

exit:
    SHR_FREE(pool_bmp);
    POLICER_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_policer_pool_free(int unit,
                            int stage,
                            int pipe,
                            int pool_id,
                            uint32_t grp_pool_bmp)
{
    bool support = false;
    int idx = 0, max_pools = 0;
    uint32_t alloc_sz = 0;
    bcmi_ltsw_policer_mode_t oper_mode = 0;
    bcmi_ltsw_policer_stage_t policer_stage;
    SHR_BITDCL *pool_bmp = NULL;

    SHR_FUNC_ENTER(unit);
    POLICER_INIT_CHECK(unit);

    POLICER_LOCK(unit);

    if (stage == bcmiFieldStageEgress) {
        policer_stage = bcmiPolicerStageEgress;
        max_pools = POLICER_INFO(unit)->max_egr_pools;
    } else if ((stage == bcmiFieldStageIngress) ||
               (stage == bcmiFieldStageExactMatch)) {
        policer_stage = bcmiPolicerStageIngress;
        max_pools = POLICER_INFO(unit)->max_ing_pools;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_policer_supported(unit,
                                     policer_stage,
                                     &support));
    if (support != true) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    alloc_sz = SHR_BITALLOCSIZE(max_pools);
    SHR_ALLOC(pool_bmp, alloc_sz, "Policer pool bitmap");
    sal_memset(pool_bmp, 0, alloc_sz);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_oper_mode_get(unit,
                                           policer_stage,
                                           &oper_mode));
    if ((oper_mode != bcmiPolicerModePipeUnique) && (pipe != -1)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_pool_clear(unit,
                                        policer_stage,
                                        pipe,
                                        pool_id));
    *pool_bmp = grp_pool_bmp;
    SHR_BIT_ITER(pool_bmp, max_pools, idx) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_policer_pool_clear(unit,
                                            policer_stage,
                                            pipe,
                                            idx));
    }

exit:
    SHR_FREE(pool_bmp);
    POLICER_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_policer_pool_prio_update(int unit,
                                   int stage,
                                   int pipe,
                                   int pool_id,
                                   uint32_t grp_pool_bmp,
                                   int priority)
{
    bool support = false;
    int idx = 0, max_pools = 0;
    uint32_t alloc_sz = 0;
    bcmi_ltsw_policer_mode_t oper_mode = 0;
    bcmi_ltsw_policer_stage_t policer_stage;
    SHR_BITDCL *pool_bmp = NULL;

    SHR_FUNC_ENTER(unit);
    POLICER_INIT_CHECK(unit);

    POLICER_LOCK(unit);

    if (stage == bcmiFieldStageEgress) {
        policer_stage = bcmiPolicerStageEgress;
        max_pools = POLICER_INFO(unit)->max_egr_pools;
    } else if ((stage == bcmiFieldStageIngress) ||
               (stage == bcmiFieldStageExactMatch)) {
        policer_stage = bcmiPolicerStageIngress;
        max_pools = POLICER_INFO(unit)->max_ing_pools;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_policer_supported(unit,
                                     policer_stage,
                                     &support));
    if (support != true) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    alloc_sz = SHR_BITALLOCSIZE(max_pools);
    SHR_ALLOC(pool_bmp, alloc_sz, "Policer pool bitmap");
    sal_memset(pool_bmp, 0, alloc_sz);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_oper_mode_get(unit,
                                           policer_stage,
                                           &oper_mode));
    if ((oper_mode != bcmiPolicerModePipeUnique) && (pipe != -1)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_pool_prio_update(unit,
                                              policer_stage,
                                              pipe,
                                              pool_id,
                                              priority));
    *pool_bmp = grp_pool_bmp;
    SHR_BIT_ITER(pool_bmp, max_pools, idx) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_policer_pool_prio_update(unit,
                                                  policer_stage,
                                                  pipe,
                                                  idx,
                                                  priority));
    }

exit:
    SHR_FREE(pool_bmp);
    POLICER_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_policer_pool_status_get(int unit,
                                  bcm_policer_t policer_id,
                                  bool *pool_status)
{
    uint32_t pid = 0;
    bool exists = false, support = false;
    bcmi_ltsw_policer_stage_t pol_stage;
    bcmint_policer_entry_t *pol_entry = NULL;

    SHR_FUNC_ENTER(unit);
    POLICER_INIT_CHECK(unit);

    POLICER_LOCK(unit);

    if ((policer_id & ~(BCM_FIELD_ID_MASK)) == BCM_FIELD_EFP_ID_BASE) {
        pol_stage = bcmiPolicerStageEgress;
    } else {
        pol_stage = bcmiPolicerStageIngress;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_policer_supported(unit,
                                     pol_stage,
                                     &support));
    if (support != true) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    pid = (policer_id & BCM_FIELD_ID_MASK);

    /* Check if the policer exists. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_id_validate(unit, pol_stage, pid, &exists));

    if (exists == false) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Fetch the policer configuration from s/w state. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_entry_get(unit, pol_stage, pid, &pol_entry));

    if (pol_entry->pol_cfg.flags & BCM_POLICER_WITH_SHARED_POOL_ID) {
        *pool_status = true;
    }

exit:
    POLICER_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_num_policer_entries_get(int unit,
                                 bcm_policer_t policer_id,
                                 uint32_t *num_entries)
{
    uint32_t pid = 0, count = 0;
    bool exists = false, support = false;
    bcmi_ltsw_policer_stage_t pol_stage;
    bcmint_policer_entry_t *pol_entry = NULL;

    SHR_FUNC_ENTER(unit);
    POLICER_INIT_CHECK(unit);

    POLICER_LOCK(unit);

    if ((policer_id & ~(BCM_FIELD_ID_MASK)) == BCM_FIELD_EFP_ID_BASE) {
        pol_stage = bcmiPolicerStageEgress;
    } else {
        pol_stage = bcmiPolicerStageIngress;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_policer_supported(unit,
                                     pol_stage,
                                     &support));
    if (support != true) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    pid = (policer_id & BCM_FIELD_ID_MASK);

    /* Check if the policer exists. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_id_validate(unit, pol_stage, pid, &exists));

    if (exists == false) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Fetch the policer configuration from s/w state. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_entry_get(unit, pol_stage, pid, &pol_entry));

    count = 1;
    if ((pol_entry->pol_cfg.mode != bcmPolicerModeGreen) &&
        (pol_entry->pol_cfg.mode != bcmPolicerModeCommitted)) {
        count++;
    }
    *num_entries = count;

exit:
    POLICER_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_policer_num_meters_in_use_get(int unit,
                                        int stage,
                                        int pipe,
                                        uint32 *num_meters_in_use)
{
    bcmi_ltsw_policer_stage_t pol_stage;
    int idx, max_pools = 0;
    bcmint_policer_pool_state_t *pool_ptr = NULL;

    SHR_FUNC_ENTER(unit);
    POLICER_INIT_CHECK(unit);

    POLICER_LOCK(unit);

    if (stage == bcmiFieldStageEgress) {
        pol_stage = bcmiPolicerStageEgress;
        max_pools = POLICER_INFO(unit)->max_egr_pools;
    } else if ((stage == bcmiFieldStageIngress) ||
               (stage == bcmiFieldStageExactMatch)) {
        pol_stage = bcmiPolicerStageIngress;
        max_pools = POLICER_INFO(unit)->max_ing_pools;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (idx = 0; idx < max_pools; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_policer_pool_info_get(unit,
                                               pol_stage,
                                               pipe,
                                               idx,
                                               &pool_ptr));
        if ((stage == bcmiFieldStageIngress &&
             pool_ptr->pool_in_use == BCMI_LTSW_POLICER_POOL_USED_BY_EM) ||
            (stage == bcmiFieldStageExactMatch &&
             pool_ptr->pool_in_use == BCMI_LTSW_POLICER_POOL_USED_BY_IFP)) {
            continue;
        }
        *num_meters_in_use += pool_ptr->entry_cnt;
    }

exit:
    POLICER_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_max_policer_entries_get(
    int unit,
    int stage,
    int *max_entries)
{
    bool support = false;
    bcmi_ltsw_policer_stage_t pol_stage;

    SHR_FUNC_ENTER(unit);

    if (stage == bcmiFieldStageEgress) {
        pol_stage = bcmiPolicerStageEgress;
    } else if ((stage == bcmiFieldStageIngress) ||
               (stage == bcmiFieldStageExactMatch)) {
        pol_stage = bcmiPolicerStageIngress;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_policer_supported(unit,
                                     pol_stage,
                                     &support));
    if (support) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_policer_entries_per_device(unit, pol_stage, max_entries));
    } else {
        *max_entries = 0;
    }

exit:
    SHR_FUNC_EXIT();
}
