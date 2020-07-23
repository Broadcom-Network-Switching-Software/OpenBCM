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

    /* Group priority ID for each pool. */
    bcmint_policer_pool_state_t *pool_state_ing;

    /* Group priority ID for each pool. */
    bcmint_policer_pool_state_t *pool_state_egr;

    /*! List of ingress policer entries */
    bcmint_policer_entry_t *ing_entries;

    /*! List of egress policer entries */
    bcmint_policer_entry_t *egr_entries;
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
/* Check if the policer_id exists. */
static int
bcmint_ltsw_policer_id_validate(
    int unit,
    bcmi_ltsw_policer_stage_t stage,
    uint32_t pid,
    bool *exists)
{
    bcmint_policer_entry_t *ptr = NULL;

    SHR_FUNC_ENTER(unit);

    if (stage == bcmiPolicerStageEgress) {
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
    bcmi_ltsw_policer_stage_t stage,
    uint32_t *pid)
{
    uint32_t num_meters = 0, idx = 0;
    bcmint_policer_entry_t *ptr = NULL;

    SHR_FUNC_ENTER(unit);

    if (*pid != 0) {
        /* If pid is specified, mark it used. */
        if (stage == bcmiPolicerStageEgress) {
            ptr = &(POLICER_INFO(unit)->egr_entries[*pid]);
        } else {
            ptr = &(POLICER_INFO(unit)->ing_entries[*pid]);
        }
        ptr->enabled = 1;
    } else {
        /* If pid is not specified, allocate a free id */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_policer_capacity_get(unit,
                                            stage,
                                            &num_meters));
        for (idx = 1; idx <= num_meters; idx++) {
            if (stage == bcmiPolicerStageEgress) {
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
    bcmi_ltsw_policer_stage_t stage,
    bcm_policer_config_t *pol_cfg,
    uint32_t pid)
{
    bcmint_policer_entry_t *ptr = NULL;

    SHR_FUNC_ENTER(unit);

    if (stage == bcmiPolicerStageEgress) {
        ptr = &(POLICER_INFO(unit)->egr_entries[pid]);
        ptr->policer_id = (pid | BCM_FIELD_EFP_ID_BASE);
    } else {
        ptr = &(POLICER_INFO(unit)->ing_entries[pid]);
        ptr->policer_id = (pid | BCM_FIELD_IFP_ID_BASE);
    }

    ptr->pol_cfg.flags = pol_cfg->flags;
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
    bcmi_ltsw_policer_stage_t stage,
    uint32_t pid,
    bcmint_policer_entry_t **pol_entry)
{
    bcmint_policer_entry_t *ptr = NULL;

    SHR_FUNC_ENTER(unit);

    if (stage == bcmiPolicerStageEgress) {
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
    bcmi_ltsw_policer_stage_t stage,
    uint32_t pid)
{
    bcmint_policer_entry_t *ptr = NULL;

    SHR_FUNC_ENTER(unit);

    if (stage == bcmiPolicerStageEgress) {
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
    int pipe,
    int prio,
    int *pool_id)
{
    bcmint_policer_pool_state_t *ptr = NULL;
    int idx = 0, max_pools = 0, start_idx = 0, end_idx = 0;

    SHR_FUNC_ENTER(unit);

    if (pol_stage == bcmiPolicerStageEgress) {
        max_pools = POLICER_INFO(unit)->max_egr_pools;
    } else {
        max_pools = POLICER_INFO(unit)->max_ing_pools;
    }

    if (*pool_id == -1) {
        /* Pool id is not passed from FP. Allocate a free pool. */
        if (pipe == -1) {
            start_idx = 0;
            end_idx = max_pools;
        } else {
            start_idx = (pipe * max_pools);
            end_idx = ((pipe + 1) * max_pools);
        }

        for (idx = start_idx; idx < end_idx; idx++) {
            if (pol_stage == bcmiPolicerStageEgress) {
                ptr = &POLICER_INFO(unit)->pool_state_egr[idx];
            } else {
                ptr = &POLICER_INFO(unit)->pool_state_ing[idx];
            }
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

                /* The pool is not free but is being used by same priority. */
                if (ptr->priority == prio) {
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
        /* Pool ID passed from FP. */
        if (*pool_id >= max_pools) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (pipe == -1) {
            idx = *pool_id;
        } else {
            idx = ((pipe * max_pools) + *pool_id);
        }
        if (pol_stage == bcmiPolicerStageEgress) {
            ptr = &POLICER_INFO(unit)->pool_state_egr[idx];
        } else {
            ptr = &POLICER_INFO(unit)->pool_state_ing[idx];
        }
        if (ptr->ref_count == 0) {
            ptr->priority = prio;
            ptr->ref_count++;
        } else {
            if ((stage == bcmiFieldStageIngress &&
                 ptr->pool_in_use == BCMI_LTSW_POLICER_POOL_USED_BY_EM) ||
                (stage == bcmiFieldStageExactMatch &&
                 ptr->pool_in_use == BCMI_LTSW_POLICER_POOL_USED_BY_IFP)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            /* The pool is not free but is being used by same priority. */
            if (ptr->priority == prio) {
                ptr->ref_count++;
            } else {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }
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
    bcmi_ltsw_policer_stage_t stage,
    int pipe,
    int pool)
{
    bcmint_policer_pool_state_t *ptr = NULL;
    int idx = 0, max_pools = 0;

    SHR_FUNC_ENTER(unit);

    if (stage == bcmiPolicerStageEgress) {
        max_pools = POLICER_INFO(unit)->max_egr_pools;
    } else {
        max_pools = POLICER_INFO(unit)->max_ing_pools;
    }

    if (pipe == -1) {
        idx = pool;
    } else {
        idx = ((pipe * max_pools) + pool);
    }

    if (stage == bcmiPolicerStageEgress) {
        ptr = &POLICER_INFO(unit)->pool_state_egr[idx];
    } else {
        ptr = &POLICER_INFO(unit)->pool_state_ing[idx];
    }

    ptr->ref_count--;

    /* This pool is now free. */
    if (ptr->ref_count == 0) {
        ptr->priority = 0;
        ptr->pool_in_use = BCMI_LTSW_POLICER_POOL_USED_BY_NONE;
    }

    SHR_FUNC_EXIT();
}

static int
bcmint_ltsw_policer_pool_prio_update(
    int unit,
    bcmi_ltsw_policer_stage_t stage,
    int pipe,
    int pool,
    int prio)
{
    bcmint_policer_pool_state_t *ptr = NULL;
    int idx = 0, max_pools = 0;

    SHR_FUNC_ENTER(unit);

    if (stage == bcmiPolicerStageEgress) {
        max_pools = POLICER_INFO(unit)->max_egr_pools;
    } else {
        max_pools = POLICER_INFO(unit)->max_ing_pools;
    }

    if (pipe == -1) {
        idx = pool;
    } else {
        idx = ((pipe * max_pools) + pool);
    }

    if (stage == bcmiPolicerStageEgress) {
        ptr = &POLICER_INFO(unit)->pool_state_egr[idx];
    } else {
        ptr = &POLICER_INFO(unit)->pool_state_ing[idx];
    }

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
bcmint_ltsw_policer_pool_in_use(int unit,
                                bcmi_ltsw_policer_stage_t stage,
                                int pool,
                                int pipe,
                                uint8_t *in_use_mode)
{
    bcmint_policer_pool_state_t *ptr = NULL;
    int idx = 0, max_pools = 0;

    SHR_FUNC_ENTER(unit);

    if (stage == bcmiPolicerStageEgress) {
        max_pools = POLICER_INFO(unit)->max_egr_pools;
    } else {
        max_pools = POLICER_INFO(unit)->max_ing_pools;
    }

    if (pipe == -1) {
        idx = pool;
    } else {
        idx = ((pipe * max_pools) + pool);
    }

    if (stage == bcmiPolicerStageEgress) {
        ptr = &POLICER_INFO(unit)->pool_state_egr[idx];
    } else {
        ptr = &POLICER_INFO(unit)->pool_state_ing[idx];
    }

    *in_use_mode = ptr->pool_in_use;

    SHR_FUNC_EXIT();
}

static int
bcmint_ltsw_policer_oper_mode_get(int unit,
                                  bcmi_ltsw_policer_stage_t stage,
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

    if (stage == bcmiPolicerStageEgress) {
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
    uint32_t num_meters = 0;
    uint32_t max_pool_idx = 0, num_pools = 0, num_elem = 0;
    void *ptr = NULL;
    bool support = false;
    int warm = 0, init = 0, rv = 0, num_pipes = 0;
    bcmi_ltsw_policer_mode_t oper_mode;

    SHR_FUNC_ENTER(unit);

    if (POLICER_INFO(unit)->lock == NULL) {
        POLICER_INFO(unit)->lock = sal_mutex_create("bcmLtsPolicerMutex");
        SHR_NULL_CHECK(POLICER_INFO(unit)->lock, SHR_E_MEMORY);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_policer_init(unit));

    warm = bcmi_warmboot_get(unit);
    init = bcmi_ltsw_init_state_get(unit);

    if (!init) {
        SHR_EXIT();
    }

    /* Check if IFP Policers are present. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_policer_supported(unit,
                                     bcmiPolicerStageIngress,
                                     &support));
    if (support == true) {
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
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_policer_supported(unit,
                                     bcmiPolicerStageEgress,
                                     &support));
    if (support == true) {
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
    int pipe, pool;
    bool exists = false, support = false;
    uint8_t in_use = 0;
    uint32_t pid = 0;
    bcmi_ltsw_policer_stage_t stage = bcmiPolicerStageIngress;
    bcmint_policer_entry_t *pol_entry = NULL;

    SHR_FUNC_ENTER(unit);
    POLICER_INIT_CHECK(unit);

    POLICER_LOCK(unit);

    /* Input parameters check. */
    SHR_NULL_CHECK(policer_id, SHR_E_PARAM);
    SHR_NULL_CHECK(pol_cfg, SHR_E_PARAM);

    if (pol_cfg->flags & BCM_POLICER_EGRESS) {
        stage = bcmiPolicerStageEgress;
    } else {
        stage = bcmiPolicerStageIngress;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_policer_supported(unit,
                                     stage,
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
            (bcmint_ltsw_policer_id_validate(unit, stage, pid, &exists));

        if (exists == true && !(pol_cfg->flags & BCM_POLICER_REPLACE)) {
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }

        if (exists == false && (pol_cfg->flags & BCM_POLICER_REPLACE)) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
    }

    /* Find a free pid. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_id_alloc(unit, stage, &pid));

    /* Copy the policer config into the sw state for this pid. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_entry_insert(unit, stage, pol_cfg, pid));

    /* Retrieve the sw state for this pid to check ref count. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_entry_get(unit, stage, pid, &pol_entry));

    /* Get the stage encoded format for this pid. */
    *policer_id = pol_entry->policer_id;

    if (pol_entry->ref_count != 0) {
        pipe = pol_entry->pipe;
        pool = pol_entry->pool_id;

        /*
         * If there are FP entries referencing this policer, it means
         * this has been installed in hardware. Call the install function to
         * update the HW fields also.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_policer_install(unit, *policer_id, pol_cfg, pipe, pool));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_policer_pool_in_use(unit, stage, pool, pipe, &in_use));

        /*
         * Update all FP entries that this policer_id is attached to.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_field_entry_policer_update_all(unit, *policer_id, in_use));
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (!SHR_FUNC_VAL_IS(SHR_E_EXISTS) && !SHR_FUNC_VAL_IS(SHR_E_INIT) &&
            !SHR_FUNC_VAL_IS(SHR_E_UNAVAIL)) {
            (void)(bcmint_ltsw_policer_entry_delete(unit, stage, pid));
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
    bcmi_ltsw_policer_stage_t stage;
    bcmint_policer_entry_t *pol_entry = NULL;

    SHR_FUNC_ENTER(unit);
    POLICER_INIT_CHECK(unit);

    POLICER_LOCK(unit);

    if ((policer_id & ~(BCM_FIELD_ID_MASK)) == BCM_FIELD_EFP_ID_BASE) {
        stage = bcmiPolicerStageEgress;
    } else {
        stage = bcmiPolicerStageIngress;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_policer_supported(unit,
                                     stage,
                                     &support));
    if (support != true) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    pid = (policer_id & BCM_FIELD_ID_MASK);

    /* Check if the policer exists. If not, return error. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_id_validate(unit, stage, pid, &exists));

    if (exists == false) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Fetch the configuration from s/w state. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_entry_get(unit, stage, pid, &pol_entry));

    /* Policer_id is not free, is being used some FP entries. */
    if (pol_entry->ref_count != 0) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    /* Delete the entry from s/w state. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_entry_delete(unit, stage, pid));
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
    int pipe, pool;
    uint8_t in_use = 0;
    uint32_t pid = 0;
    bool exists = false, support = false;
    bcmi_ltsw_policer_stage_t stage;
    bcmint_policer_entry_t *pol_entry = NULL;

    SHR_FUNC_ENTER(unit);
    POLICER_INIT_CHECK(unit);

    POLICER_LOCK(unit);

    if ((policer_id & ~(BCM_FIELD_ID_MASK)) == BCM_FIELD_EFP_ID_BASE) {
        stage = bcmiPolicerStageEgress;
    } else {
        stage = bcmiPolicerStageIngress;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_policer_supported(unit,
                                     stage,
                                     &support));
    if (support != true) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    pid = (policer_id & BCM_FIELD_ID_MASK);

    /* Check if the policeR_id exists, if not return error. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_id_validate(unit, stage, pid, &exists));

    if (exists == false) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Insert the entry into the s/w state. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_entry_insert(unit, stage, pol_cfg, pid));

    /* Fetch the s/w state corresponding to this policer_id. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_entry_get(unit, stage, pid, &pol_entry));

    if (pol_entry->ref_count != 0) {
        /*
         * ref_count != 0 means there are FP entries using this policer
         * and that the policer is installed in hardware. Update the
         * hardware tables.
         */
        pipe = pol_entry->pipe;
        pool = pol_entry->pool_id;
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_policer_install(unit, policer_id, pol_cfg, pipe, pool));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_policer_pool_in_use(unit, stage, pool, pipe, &in_use));

        /* Update the FP entries to which this policer is attached. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_field_entry_policer_update_all(unit, policer_id, in_use));
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
    bcmi_ltsw_policer_stage_t stage;
    bcmint_policer_entry_t *pol_entry = NULL;

    SHR_FUNC_ENTER(unit);
    POLICER_INIT_CHECK(unit);

    POLICER_LOCK(unit);

    if ((policer_id & ~(BCM_FIELD_ID_MASK)) == BCM_FIELD_EFP_ID_BASE) {
        stage = bcmiPolicerStageEgress;
    } else {
        stage = bcmiPolicerStageIngress;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_policer_supported(unit,
                                     stage,
                                     &support));
    if (support != true) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    pid = (policer_id & BCM_FIELD_ID_MASK);

    /* Check if the policer exists. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_id_validate(unit, stage, pid, &exists));

    if (exists == false) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Fetch the policer configuration from s/w state. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_entry_get(unit, stage, pid, &pol_entry));

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

int
bcmi_ltsw_policer_attach(int unit,
                         bcm_policer_t policer_id,
                         int pipe,
                         int pool_id)
{
    bcm_policer_config_t pol_cfg;
    uint32_t pid = 0;
    bool exists = false, support = false;
    bcmi_ltsw_policer_stage_t stage;
    bcmi_ltsw_policer_mode_t oper_mode = 0;
    bcmint_policer_entry_t *pol_entry = NULL;

    SHR_FUNC_ENTER(unit);
    POLICER_INIT_CHECK(unit);

    POLICER_LOCK(unit);

    if ((policer_id & ~(BCM_FIELD_ID_MASK)) == BCM_FIELD_EFP_ID_BASE) {
        stage = bcmiPolicerStageEgress;
    } else {
        stage = bcmiPolicerStageIngress;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_policer_supported(unit,
                                     stage,
                                     &support));
    if (support != true) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_oper_mode_get(unit,
                                           stage,
                                           &oper_mode));
    if ((oper_mode != bcmiPolicerModePipeUnique) && (pipe != -1)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    pid = (policer_id & BCM_FIELD_ID_MASK);

    /* Check if the policer exists. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_id_validate(unit, stage, pid, &exists));

    if (exists == false) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Fetch the configuration from s/w state. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_entry_get(unit, stage, pid, &pol_entry));

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
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_policer_install(unit, policer_id, &pol_cfg, pipe, pool_id));

        /* update actual_xxx fields. */
        sal_memset(&pol_cfg, 0, sizeof(pol_cfg));

        /*
         * Fetch the policer configuration from hardware
         * and update actual_xxx fields.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_policer_get(unit, policer_id, &pol_cfg));
        pol_entry->pipe = pipe;
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
        if (pool_id != pol_entry->pool_id) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
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
    bcmi_ltsw_policer_stage_t stage;
    bcmint_policer_entry_t *pol_entry = NULL;

    SHR_FUNC_ENTER(unit);
    POLICER_INIT_CHECK(unit);

    POLICER_LOCK(unit);

    if ((policer_id & ~(BCM_FIELD_ID_MASK)) == BCM_FIELD_EFP_ID_BASE) {
        stage = bcmiPolicerStageEgress;
    } else {
        stage = bcmiPolicerStageIngress;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_policer_supported(unit,
                                     stage,
                                     &support));
    if (support != true) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    pid = (policer_id & BCM_FIELD_ID_MASK);

    /* Check if the policer exists, if not return error. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_id_validate(unit, stage, pid, &exists));

    if (exists == false) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Fetch the policer configuration from s/w state. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_policer_entry_get(unit, stage, pid, &pol_entry));

    /* Decrement ref_count. */
    pol_entry->ref_count--;

    if (pol_entry->ref_count == 0) {
        /*
         * Policer being detached from the last FP entry,
         * uninstall from hardware.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_policer_destroy(unit, policer_id));
    }

exit:
    POLICER_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_policer_pool_alloc(int unit,
                             int stage,
                             int pipe,
                             int priority,
                             int *pool_id)
{
    bool support = false;
    bcmi_ltsw_policer_mode_t oper_mode = 0;
    bcmi_ltsw_policer_stage_t policer_stage;

    SHR_FUNC_ENTER(unit);
    POLICER_INIT_CHECK(unit);

    POLICER_LOCK(unit);

    if (stage == bcmiFieldStageEgress) {
        policer_stage = bcmiPolicerStageEgress;
    } else if ((stage == bcmiFieldStageIngress) ||
               (stage == bcmiFieldStageExactMatch)) {
        policer_stage = bcmiPolicerStageIngress;
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
                                        pipe,
                                        priority,
                                        pool_id));
exit:
    POLICER_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_policer_pool_free(int unit,
                            int stage,
                            int pipe,
                            int pool_id)
{
    bool support = false;
    bcmi_ltsw_policer_mode_t oper_mode = 0;
    bcmi_ltsw_policer_stage_t policer_stage;

    SHR_FUNC_ENTER(unit);
    POLICER_INIT_CHECK(unit);

    POLICER_LOCK(unit);

    if (stage == bcmiFieldStageEgress) {
        policer_stage = bcmiPolicerStageEgress;
    } else if ((stage == bcmiFieldStageIngress) ||
               (stage == bcmiFieldStageExactMatch)) {
        policer_stage = bcmiPolicerStageIngress;
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
exit:
    POLICER_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_policer_pool_prio_update(int unit,
                                   int stage,
                                   int pipe,
                                   int pool_id,
                                   int priority)
{
    bool support = false;
    bcmi_ltsw_policer_mode_t oper_mode = 0;
    bcmi_ltsw_policer_stage_t policer_stage;

    SHR_FUNC_ENTER(unit);
    POLICER_INIT_CHECK(unit);

    POLICER_LOCK(unit);

    if (stage == bcmiFieldStageEgress) {
        policer_stage = bcmiPolicerStageEgress;
    } else if ((stage == bcmiFieldStageIngress) ||
               (stage == bcmiFieldStageExactMatch)) {
        policer_stage = bcmiPolicerStageIngress;
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
exit:
    POLICER_UNLOCK(unit);
    SHR_FUNC_EXIT();

}

int
bcmi_ltsw_policer_num_meters_in_use_per_pool_get(int unit,
                                                 int stage,
                                                 int pipe,
                                                 int pool_id,
                                                 uint32 *num_meters_in_use)
{
    bcmi_ltsw_policer_stage_t pol_stage;
    uint32_t num_meters = 0, idx = 0;
    bool exists = false;
    bcmint_policer_entry_t *pol_entry = NULL;

    SHR_FUNC_ENTER(unit);
    POLICER_INIT_CHECK(unit);

    POLICER_LOCK(unit);

    if (stage == bcmiFieldStageEgress) {
        pol_stage = bcmiPolicerStageEgress;
    } else if ((stage == bcmiFieldStageIngress) ||
               (stage == bcmiFieldStageExactMatch)) {
        pol_stage = bcmiPolicerStageIngress;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_policer_capacity_get(unit,
                                        pol_stage,
                                        &num_meters));
    for (idx = 1; idx <= num_meters; idx++) {
        /* Check if the pid already exists. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_policer_id_validate(unit,
                                             pol_stage,
                                             idx,
                                             &exists));
        /* Retrieve the sw state for this pid to check ref count. */
        if (exists) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_policer_entry_get(unit,
                                               pol_stage,
                                               idx,
                                               &pol_entry));
            if ((pol_entry->pool_id == pool_id) &&
               (pol_entry->pipe == pipe)) {
                *num_meters_in_use += pol_entry->ref_count;
            }
        }
    }
exit:
    POLICER_UNLOCK(unit);
    SHR_FUNC_EXIT();
}
